#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "math.h"
#include "FileUtil.h"
#include "MS896A.h"
#include "PrescanUtility.h"
#include "StrInputDlg.h"
#include "BH_Constant.h"

VOID CWaferTable::TC_LoadOption(CStringMapFile  *psmf)
{
	m_stTCEjt.m_bTCEnable		= (BOOL)(LONG)(*psmf)[WT_TC_OPTION][WT_TC_ENABLE];
	m_stTCEjt.m_bCheckTmp		= (BOOL)(LONG)(*psmf)[WT_TC_OPTION][WT_TC_CHECK_TEMP];
	m_stTCEjt.m_bCheckAlarmBit	= (BOOL)(LONG)(*psmf)[WT_TC_OPTION][WT_TC_CHECK_ALARM];
	m_stTCEjt.m_bOnPower		= (BOOL)(LONG)(*psmf)[WT_TC_OPTION][WT_TC_POWER_ON];
	m_stTCEjt.m_bTuneCalibrated	= (BOOL)(LONG)(*psmf)[WT_TC_OPTION][WT_TC_TUNE_CALIBRATED];

	m_stTCEjt.m_lTargetTmp		= (LONG)(*psmf)[WT_TC_DATA][WT_TC_TARGET_TEMP];
	m_stTCEjt.m_lTmpTolerance	= (LONG)(*psmf)[WT_TC_DATA][WT_TC_TEMP_TOLERANCE];
	m_stTCEjt.m_lTuneTmp		= (LONG)(*psmf)[WT_TC_DATA][WT_TC_TUNE_TEMP];
	m_stTCEjt.m_lActualTmp		= (LONG)(*psmf)[WT_TC_DATA][WT_TC_ACTUAL_TEMP];
}

VOID CWaferTable::TC_SaveData(CStringMapFile  *psmf)
{
	(*psmf)[WT_TC_OPTION][WT_TC_ENABLE]			 = m_stTCEjt.m_bTCEnable;
	(*psmf)[WT_TC_OPTION][WT_TC_CHECK_TEMP]		 = m_stTCEjt.m_bCheckTmp;
	(*psmf)[WT_TC_OPTION][WT_TC_CHECK_ALARM]	 = m_stTCEjt.m_bCheckAlarmBit;
	(*psmf)[WT_TC_OPTION][WT_TC_POWER_ON]		 = m_stTCEjt.m_bOnPower;
	(*psmf)[WT_TC_OPTION][WT_TC_TUNE_CALIBRATED] = m_stTCEjt.m_bTuneCalibrated;

	(*psmf)[WT_TC_DATA][WT_TC_TARGET_TEMP]		 = m_stTCEjt.m_lTargetTmp;
	(*psmf)[WT_TC_DATA][WT_TC_TEMP_TOLERANCE]	 = m_stTCEjt.m_lTmpTolerance;
	(*psmf)[WT_TC_DATA][WT_TC_TUNE_TEMP]		 = m_stTCEjt.m_lTuneTmp;
	(*psmf)[WT_TC_DATA][WT_TC_ACTUAL_TEMP]		 = m_stTCEjt.m_lActualTmp;
}

VOID CWaferTable::TC_InitVariables()
{
	m_stTCEjt.m_lTuneTmp = 200;
	m_stTCEjt.m_lActualTmp = 200;
	m_stTCEjt.m_bTuneCalibrated = FALSE;

	m_stTCEjt.m_bOnPower	= FALSE;
	m_stTCEjt.m_bTCEnable	= FALSE;
	m_stTCEjt.m_bCheckTmp	= FALSE;
	m_stTCEjt.m_bCheckAlarmBit	= FALSE;

	m_stTCEjt.m_lTargetTmp = 200;
	m_stTCEjt.m_lTmpTolerance = 30;

	m_bIsConnected = FALSE;
	m_bHMIEjectorHeaterOn = FALSE;
	m_bTCInUse = FALSE;

	m_bTCTuneStart = FALSE;
}

BOOL CWaferTable::TC_InitInstance()
{
	/*
	m_stReadTmpTime = CTime::GetCurrentTime();
	if( m_fHardware && !m_bIsConnected && m_bTCInUse )
	{
		try   
		{ 
			CString szPath;
			szPath = gszUSER_DIRECTORY + _T("\\Heater");
			CreateDirectory(szPath, NULL);

			m_bIsConnected = TRUE; 

			if( m_oTCtrl.Create(MS899_TC_SERIAL_PORT_NO_1) )
			{
				TC_LogMessage("Create comm port ok");
			}
			else
			{
				m_bIsConnected = FALSE;
				TC_LogMessage("Create comm port error");
			}
			m_oTCtrl.wmc_set_working_dir(szPath + _T("\\"));
			if( m_oTCtrl.wmc_open_commport() )
			{
				TC_LogMessage("Open comm port ok!");
			}
			else
			{
				m_bIsConnected = FALSE;
				TC_LogMessage("Open comm port error!");
			}

			Sleep(500);
			if(TC_IsEnable())
			{
				TC_SetTargetAndLimit();

				if(TC_IsOnPower())
				{
					TC_TurnPowerOn(TRUE);
				}
				if( TC_IsCheckAlarm() )
				{
					TC_TurnAlarmOn(TRUE);
				}

				TC_FixConvertion();	//Initial 2 points calibration
			}
	
			CString str;
			ULONG FirmwareVer, LibraryVer;
			MTC232 stMTC232;
			stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;
			SHORT nStatus = m_oTCtrl.wmc_get_version_info_ex(&stMTC232, &FirmwareVer, &LibraryVer);
			str.Format("get version info %d,%d return %d", FirmwareVer, LibraryVer, nStatus);
			TC_LogMessage(str);

			str.Format("%8x", LibraryVer);
			m_szTCLibraryVer = str.Left(2) + "." + str.Mid(2, 2) + "." + str.Right(4);
			str.Format("%8x", FirmwareVer);
			m_szTCFirmwareVer = str.Left(2) + "." + str.Mid(2, 2) + "." + str.Right(4);
			if( m_bIsConnected )
			{
				DisplayMessage("TC    Init Com 1 OK!");
			}
			else
			{
				DisplayMessage("TC    Init Com 1 FAIL!");
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CString str;
			str.Format("COM Port %d Error: T-Controller INIT fails.", MS899_TC_SERIAL_PORT_NO_1);
			TC_LogMessage(str);
		}
	}
	*/
	return TRUE;
}

VOID CWaferTable::TC_ExitInstance()
{
	/*
	if ( m_fHardware && m_bIsConnected && m_bTCInUse )
	{
		if( TC_IsEnable() )
		{
			TC_TurnPowerOn(FALSE);
			TC_TurnAlarmOn(FALSE);
		}
		m_oTCtrl.wmc_close_commport();
	}*/
}

VOID CWaferTable::TC_RegisterVariables()
{
	try
	{
		RegVariable(_T("TC_bHardwareInUse"),	&m_bTCInUse);
		RegVariable(_T("TC_bEnableHeater"),		&m_stTCEjt.m_bTCEnable);
		RegVariable(_T("TC_bIsConnected"),		&m_bIsConnected);
		RegVariable(_T("TC_bEjectorHeaterOn"),	&m_bHMIEjectorHeaterOn);

		RegVariable(_T("TC_lCurrentTmp"),		&m_lCurrentTmp);
		RegVariable(_T("TC_lHeaterInTmp"),		&m_lHeaterInTmp);
		RegVariable(_T("TC_bAlarmBitOn"),		&m_bAlarmBitOn);

		RegVariable(_T("TC_szLibraryVer"),		&m_szTCLibraryVer);
		RegVariable(_T("TC_szFirmwareVer"),		&m_szTCFirmwareVer);

		RegVariable(_T("TC_bPowerOn"),			&m_stTCEjt.m_bOnPower);
		RegVariable(_T("TC_bCheckHeater"),		&m_stTCEjt.m_bCheckTmp);
		RegVariable(_T("TC_bCheckAlarm"),		&m_stTCEjt.m_bCheckAlarmBit);

		RegVariable(_T("TC_lTargetTmp"),		&m_stTCEjt.m_lTargetTmp);
		RegVariable(_T("TC_lTmpTolerance"),		&m_stTCEjt.m_lTmpTolerance);


		RegVariable(_T("TC_bTuneClibrated"),	&m_stTCEjt.m_bTuneCalibrated);
		RegVariable(_T("TC_lTuneTmp"),			&m_stTCEjt.m_lTuneTmp);
		RegVariable(_T("TC_lActualTmp"),		&m_stTCEjt.m_lActualTmp);

		//-- Register SERVICE server commands --//
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Tc_PowerOnHeater"),		&CWaferTable::Tc_PowerOnHeater);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Tc_CheckAlarmBit"),		&CWaferTable::Tc_CheckAlarmBit);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Tc_SetTargetTmp"),		&CWaferTable::Tc_SetTargetTmp);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Tc_SetTempTolerance"),	&CWaferTable::Tc_SetTempTolerance);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Tc_StartTuneEjector"),	&CWaferTable::Tc_StartTuneEjector);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Tc_SetTuneActualTmp"),	&CWaferTable::Tc_SetTuneActualTmp);
	}
	catch(CAsmException e)
	{
		DisplayException(e); 
	}
}

// for ejector heater tuning and display temperature in idle state only
VOID CWaferTable::TC_AutoTuneEjector()
{
	/*
	if(m_bTCTuneStart)
	{
		if(m_bGeneralStopCycle==FALSE)
		{
			TC_TuneAbort();
			if (!TC_IsOnPower()) 
				TC_TurnPowerOn(FALSE);

			m_bTCTuneStart = FALSE;

			HmiMessage("= Tuning stopped =");
		}
		else
		{
			MTC232 stMTC232;
			CString szErrorMsg;
			CString szTemp;
			SHORT sStatus, tuneStatus;
			stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;
			sStatus = m_oTCtrl.wmc_tune_get_status(&stMTC232, &tuneStatus);
			CString str;
			str.Format("Get tuning status %d error (%d)!", tuneStatus, sStatus);
			TC_LogMessage(str);
			if(sStatus)
			{
				HmiMessage(str);
				TC_TuneAbort();
				m_bGeneralStopCycle =FALSE;
			}
			else
			{
				if(tuneStatus == 1)
				{
					m_bGeneralStopCycle = FALSE;
					float dkp, dki, dkd, skp, ski, skd;
					stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;
					sStatus = m_oTCtrl.wmc_tune_get_parameter(&stMTC232, TC_GetTuneTmp(), 0, 0, &dkp, &dkd, &dki, &skp, &skd, &ski);
					str.Format("Tuning calculate error (%d)!", sStatus);
					TC_LogMessage(str);
					if(sStatus)
					{
						HmiMessage(str);
					}
					else 
					{
						szErrorMsg.Format("= Tuned PID parameter =\n");
						szTemp.Format("Temperature Controller Ejector\n");
						szErrorMsg += szTemp;
						szTemp.Format("Dynamic [P, I, D]=[%.2f, %.2f, %.2f]\n", dkp, dki, dkd);
						szErrorMsg += szTemp;
						szTemp.Format("Static  [P, I, D]=[%.2f, %.2f, %.2f]\n", skp, ski, skd);
						szErrorMsg += szTemp;
						HmiMessage(szErrorMsg);
					}

					TC_SetTargetAndLimit();
					if (TC_IsOnPower()) 
						TC_TurnPowerOn(TRUE);
				}
				else if(tuneStatus)
				{
					TC_CheckEjectorState(FALSE);

					TC_TuneAbort();
					m_bGeneralStopCycle = FALSE;
				}
			}
			Sleep(500);
		}
	}

	// update HMI display
	CTimeSpan stTimeDiff = CTime::GetCurrentTime() - m_stReadTmpTime;
	if( stTimeDiff.GetTotalSeconds()> 1 )
	{
		TC_ReadTemperature();
		m_stReadTmpTime = CTime::GetCurrentTime();
	}
	m_bHMIEjectorHeaterOn = TC_IsEnable() && TC_IsOnPower();
	*/
}

BOOL CWaferTable::TC_CheckEjectorState(BOOL bPopMsg)
{
	if( TC_IsEnable()==FALSE || TC_IsOnPower()==FALSE )
		return TRUE;

	/*
	BYTE BStatus = 0;
	CString szErrorMsg, szTemp;

	MTC232 stMTC232;
	stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;
	SHORT sStatus = m_oTCtrl.wmc_get_status(&stMTC232, &BStatus);
	szTemp.Format("get status %d return %d", BStatus, sStatus);
	TC_LogMessage(szTemp);

	BStatus = BStatus & 0x0f76;

	if(BStatus == 0)
	{
		return TRUE;
	}

	szErrorMsg.Format("Temperature Controller Ejector\n");
	if (BStatus & lsbbit1)
	{
		szTemp = "Thermocouple connection error\n";
		szErrorMsg += szTemp;
	}
	if (BStatus & lsbbit2)
	{
		szTemp = "Cold junction sensor connection error\n";
		szErrorMsg += szTemp;
	}
	if (BStatus & lsbbit4)
	{
		szTemp = "Temperature hit limit\n";
		szErrorMsg += szTemp;
	}
	if (BStatus & lsbbit5)
	{
		szTemp = "Heater open error\n";
		szErrorMsg += szTemp;
	}
	if (BStatus & lsbbit6)
	{
		//v4.04		//Klocwork
		szTemp = "Heater protection trigger\n";
		szErrorMsg += szTemp;
		//}
	}

	if( bPopMsg )
		HmiMessage(szErrorMsg);
	else
		TC_LogMessage(szErrorMsg);
	*/
	return FALSE;
}

VOID CWaferTable::TC_SetTargetAndLimit()
{
	/*
	MTC232 stMTC232;
	stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;

	SHORT sTargetTmp = 0;
	if (TC_IsTuneCalib())
		sTargetTmp = (TC_GetTargetTmp() * TC_GetTuneTmp()) / TC_GetRealTmp();
	else
		sTargetTmp = TC_GetTargetTmp();

	SHORT sStatus = m_oTCtrl.wmc_set_temp_with_bandlimit(&stMTC232, sTargetTmp, TC_GetTmpToler());
	CString szMsg;
	szMsg.Format("set temperature %d with band limnit %d return status %d",
		sTargetTmp, TC_GetTmpToler(), sStatus);
	TC_LogMessage(szMsg);
	// ?? set the tolerance and controller can send out an alarm bit to hipec to trigger error
	*/
}

VOID CWaferTable::TC_TurnPowerOn(BOOL bOn)
{
	/*
	MTC232 stMTC232;
	stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;

	TC_SetTargetAndLimit();
	SHORT sStatus = m_oTCtrl.wmc_temp_onoff(&stMTC232, bOn);
	CString szMsg;
	szMsg.Format("power on %d heater status %d", bOn, sStatus);
	TC_LogMessage(szMsg);
	*/
}

VOID CWaferTable::TC_TurnAlarmOn(BOOL bOn)
{
	/*
	MTC232 stMTC232;
	stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;
	SHORT sStatus = m_oTCtrl.wmc_alarm_onoff(&stMTC232, bOn);
	CString szMsg;
	szMsg.Format("Turn Alarm %d return status %d", bOn, sStatus);
	TC_LogMessage(szMsg);
	*/
}

VOID CWaferTable::TC_TuneAbort()
{
	/*
	MTC232 stMTC232;
	stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;
	SHORT sStatus = m_oTCtrl.wmc_tune_abort(&stMTC232);
	CString szMsg;
	szMsg.Format("Tune abourt return %d", sStatus);
	TC_LogMessage(szMsg);
	*/
}

VOID CWaferTable::TC_FixConvertion()
{
	/*
	MTC232 stMTC232;
	stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;
	m_oTCtrl.wmc_FixConvertion(&stMTC232, 1, 48313, -2774);
	*/
}

VOID CWaferTable::TC_ReadTemperature()
{
	if( TC_IsEnable())
	{
		/*
		MTC232 stMTC232;
		SHORT sTemp = 0;

		stMTC232.Channel.byMTCChannel  = MS899_TC_EJECTOR_CHANNEL;
		SHORT sStatus = m_oTCtrl.wmc_get_temp(&stMTC232, &sTemp);	// kernel coil temperatrue
		if (TC_IsTuneCalib())
			m_lCurrentTmp = (long) (sTemp * (TC_GetRealTmp()) / TC_GetTuneTmp());
		else
			m_lCurrentTmp = sTemp;
		m_lHeaterInTmp = sTemp;
		*/
	}
}

LONG CWaferTable::TC_CheckEjectorLimit()
{
	LONG lError = 0;
	if (TC_IsEnable() && TC_IsCheckTmp() && TC_IsOnPower() )
	{
		TC_ReadTemperature();
		if( (m_lCurrentTmp) > (TC_GetTargetTmp() + TC_GetTmpToler()) )
		{
			lError = 1;	// over upper limit
		}
		if( (m_lCurrentTmp) < (TC_GetTargetTmp() - TC_GetTmpToler()) )
		{
			lError += 2; // over lower limit
		}
	}

	return lError;
}

BOOL CWaferTable::TC_CheckAlarmOn()
{
	if( IsLayerPicking() && TC_IsCheckAlarm() )
	{
		return CMS896AStn::MotionReadInputBit(TC_SI_ALARM_BIT);
	}

	return FALSE;
}

BOOL CWaferTable::TC_IsEnable()	// LiTec hardware installed and enabled and initated
{
	return m_bTCInUse && m_stTCEjt.m_bTCEnable && m_bIsConnected;
}

VOID CWaferTable::TC_LogMessage(CString szMsg)
{
	return;
/*
	CString szNew;
	szNew = "TC    " + szMsg;
	FILE *fp;
	fp = fopen(gszUSER_DIRECTORY + _T("\\Heater\\TC_Status.txt"), "a");
	if( fp!=NULL )
	{
		fprintf(fp, "%s\n", (LPCTSTR) szNew);	//Klocwork
		fclose(fp);
	}
*/
}

BOOL CWaferTable::TC_IsOnPower()
{
	return m_stTCEjt.m_bOnPower;
}

BOOL CWaferTable::TC_IsCheckTmp()
{
	return m_stTCEjt.m_bCheckTmp;
}

BOOL CWaferTable::TC_IsCheckAlarm()
{
	return m_stTCEjt.m_bCheckAlarmBit;
}

BOOL CWaferTable::TC_IsTuneCalib()
{
	return m_stTCEjt.m_bTuneCalibrated;
}

SHORT CWaferTable::TC_GetTuneTmp()
{
	return (SHORT) m_stTCEjt.m_lTuneTmp;
}

SHORT CWaferTable::TC_GetRealTmp()
{
	//-- Init controller data --//
	if(m_stTCEjt.m_lActualTmp < 50 || m_stTCEjt.m_lActualTmp > 300)
		m_stTCEjt.m_lActualTmp = TC_GetTuneTmp();
	return (SHORT) m_stTCEjt.m_lActualTmp;
}

SHORT CWaferTable::TC_GetTargetTmp()
{
	return (SHORT) m_stTCEjt.m_lTargetTmp;
}

SHORT CWaferTable::TC_GetTmpToler()
{
	if( m_stTCEjt.m_lTmpTolerance<=0 || m_stTCEjt.m_lTmpTolerance>50 )
		m_stTCEjt.m_lTmpTolerance = 30;

	return (SHORT) m_stTCEjt.m_lTmpTolerance;
}






//================================================================
// HMI Server Command function implementation section
//================================================================

LONG CWaferTable::Tc_PowerOnHeater(IPC_CServiceMessage& svMsg)
{
	if( TC_IsEnable()==FALSE )
	{
		HmiMessage("TC not enable yet!");
		return 1;
	}

	TC_TurnPowerOn(TC_IsOnPower());

	return 1;
}

LONG CWaferTable::Tc_CheckAlarmBit(IPC_CServiceMessage& svMsg)
{
	if( TC_IsEnable()==FALSE )
	{
		return 1;
	}

	TC_TurnAlarmOn(TC_IsCheckAlarm());

	return 1;
}

LONG CWaferTable::Tc_SetTargetTmp(IPC_CServiceMessage& svMsg)
{
	if( TC_IsEnable()==FALSE )
	{
		HmiMessage("TC not enable yet!");
		return 1;
	}

	TC_SetTargetAndLimit();

	return 1;
}

LONG CWaferTable::Tc_SetTempTolerance(IPC_CServiceMessage& svMsg)
{
	if( TC_IsEnable()==FALSE )
	{
		HmiMessage("TC not enable yet!");
		return 1;
	}

	if(TC_IsOnPower()) 
	{
		TC_TurnPowerOn(TRUE);
	}
	if( TC_IsCheckAlarm() )
	{
		TC_TurnAlarmOn(TRUE);
	}
	TC_SetTargetAndLimit();

	return 1;
}

LONG CWaferTable::Tc_StartTuneEjector(IPC_CServiceMessage& svMsg)
{
	if( TC_IsEnable()==FALSE )
	{
		HmiMessage("TC not enable yet!");
		return 1;
	}

	if (HmiMessage("Start Tuning?", "TC Tunning", glHMI_MBX_YESNO) != glHMI_YES)
	{
		return 0;
	}

	/*
	if (TC_IsOnPower()) 
	{
		TC_TurnPowerOn(TRUE);
	}

	SHORT nErr;
	MTC232 stMTC232;

	stMTC232.Channel.byMTCChannel = MS899_TC_EJECTOR_CHANNEL;
	m_stTCEjt.m_bTuneCalibrated = FALSE;
	m_stTCEjt.m_lActualTmp = TC_GetTuneTmp();
	nErr = m_oTCtrl.wmc_tune_start(&stMTC232, TC_GetTuneTmp());
	CString str;
	str.Format("Tuning start error (%d)!", nErr);
	TC_LogMessage(str);
	if( nErr )
	{
		HmiMessage(str);
		return 0;
	}

	m_bTCTuneStart = TRUE;
	m_bGeneralStopCycle = TRUE;
	*/
	return 0;
}

LONG CWaferTable::Tc_SetTuneActualTmp(IPC_CServiceMessage& svMsg)
{
	if( TC_IsEnable()==FALSE )
	{
		HmiMessage("TC not enable yet!");
		return 1;
	}
	if( m_bTCTuneStart )
	{
		HmiMessage("Tuning not stopped yet, please wait...");
		return 1;
	}

	//m_stTCEjt.m_bTuneCalibrated = TRUE;
	//TC_SetTargetAndLimit();
	return 1;
}
