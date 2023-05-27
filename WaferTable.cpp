/////////////////////////////////////////////////////////////////
// WaferTable.cpp : interface of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "PreBondEvent.h"

#include "WAF_CWaferZoomWindowView.h"
#include "gdiplus.h"
#include "BH_Constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CWaferTable, CMS896AStn)

CWaferTable::CWaferTable():
							m_evEjectorReady(FALSE, TRUE, "EjReadyEvt"),
							m_evBhTReadyForWT(FALSE, TRUE, "BhTReadyForWTEvt"),		//v4.46T22
							m_evWTStable(FALSE, TRUE, "WtStableEvt"),
							m_evWTReady(FALSE, TRUE, "WtReadyEvt"),
							m_evWTStartMove(FALSE, TRUE, "WtStartMoveEvt"),			//v3.66		//DBH
						    m_evWTBacked(FALSE, TRUE, "WtBackedEvt"),
							m_evMoveBack(FALSE, TRUE, "WtMoveBackEvt"),
							m_evDieInfoRead(FALSE, TRUE, "WtDieInfoReadEvt"),
						    m_evDieReady(FALSE, TRUE, "WprDieReadyEvt"),
							m_evCompDone(FALSE, TRUE, "CompDoneEvt"),				//v4.34T10
     						m_evDieReadyForBT(FALSE, TRUE, "WtDieReadyForBTEvt"),
 						    m_evDiePicked(FALSE, TRUE, "BhDiePickedEvt"),
						    m_evDieBondedForWT(FALSE, TRUE, "BhDieBondedForWTEvt"),
	 						m_evBadDie(FALSE, TRUE, "WprBadDieEvt"),
							m_evBHInit(FALSE, TRUE, "BhInitEvt"),
							m_evWTInitT(FALSE, TRUE, "WTInitTEvt"),	
						    m_evPRStart(FALSE, TRUE, "WprStartEvt"),
						    m_evLFReady(FALSE, TRUE, "WprLFReadyEvt"),
							m_evBPRLatched(FALSE, TRUE, "BprLatchedEvt"),
							m_evExpInitForWt(FALSE, TRUE, "ExpInitForWtTEvt"),		//v2.63
							m_evPreCompensate(FALSE, TRUE, "PreCompensateEvt"),		//v2.96T3
							m_evInitAFZ(FALSE, TRUE, "InitAFZ"),
							m_evWTReadyForWPREmptyCheck(FALSE, TRUE, "WTReadyForWPREmptyCheckEvt"),		//v4.54A5
							m_evWPREmptyCheckDone(FALSE, TRUE, "WPREmptyCheckDoneEvt"),		//v4.54A5
							m_evWTStartToMoveForBT(FALSE, TRUE, "WTStartToMoveForBT"),
// prescan relative code
							m_evES101WldPreloadDone(FALSE, TRUE, "ES101WldPreloadDoneEvt"),
							m_evAllPrescanDone(FALSE, TRUE, "AllPrescanDoneEvt"),
							m_evSetPRTesting(FALSE, TRUE, "SetPRTesting"),
							m_evAutoChangeColletDone(FALSE, TRUE, "AutoChangeColletDone")
{
	m_szEventPrefix	= "WT";
	m_pBlkFunc = NULL;
	m_pBlkFunc2 = NULL;		//Block2
	m_p1stDieFinder = NULL;

	m_pWaferMapManager = WM_CWaferMap::Instance();
	m_pWaferMapManager->SetWaferMapWrapper(&m_WaferMapWrapper);
	m_pEjPinCleanRegion = new CEjectorPinCleanRegion;
	InitVariable();
}

CWaferTable::~CWaferTable()
{
	//delete[] m_pEjPinCleanRegion;
}


BOOL CWaferTable::InitInstance()
{
	LONG lDynamicControlX	= 0;
	LONG lStaticControlX	= 0;
	LONG lDynamicControlY	= 0;
	LONG lStaticControlY	= 0;
	LONG lDynamicControlT	= 0;
	LONG lStaticControlT	= 0;
	LONG lMotorDirectionX	= 0;
	LONG lMotorDirectionY	= 0;
	LONG lMotorProtectionX	= 0;
	LONG lMotorProtectionY	= 0;

	CMS896AStn::InitInstance();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_pPrGeneral = pApp->GetPrGeneral();

	m_bSel_X	= TRUE;	
	m_bSel_Y	= TRUE;
	m_bSel_T	= FALSE;
	m_bSel_X2	= FALSE;	
	m_bSel_Y2	= FALSE;
	m_bSel_T2	= FALSE;
	
	if (m_bDisableWT)
	{
		m_bSel_X	= TRUE;	
		m_bSel_Y	= TRUE;
		m_bSel_T	= FALSE;
	}

	m_lXNegLimit	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_X,  MS896A_CFG_CH_MIN_DISTANCE);
	m_lXPosLimit	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_X,  MS896A_CFG_CH_MAX_DISTANCE);
	m_lYNegLimit	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_Y,  MS896A_CFG_CH_MIN_DISTANCE);
	m_lYPosLimit	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_Y,  MS896A_CFG_CH_MAX_DISTANCE);

	UINT unNegLimit = pApp->GetProfileInt(gszPROFILE_SETTING, _T("MS90 WFT Neg Limit Y"), 0);
	if( unNegLimit != 0 )
	{
		m_lYNegLimit = 0 - unNegLimit;
	}
	
	CMS896AStn::m_lWafXNegLimit = m_lXNegLimit;
	CMS896AStn::m_lWafXPosLimit = m_lXPosLimit;
	CMS896AStn::m_lWafYNegLimit = m_lYNegLimit;
	CMS896AStn::m_lWafYPosLimit = m_lYPosLimit;

	CString szMsg;
	szMsg.Format("WT XY Limit - X (%ld, %ld), Y (%ld, %ld)", 
					m_lXNegLimit, m_lXPosLimit, m_lYNegLimit, m_lYPosLimit);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	m_bEnableWTTheta = (BOOL) GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T,	MS896A_CFG_CH_ENABLE);


	try
	{
		if (m_fHardware && !m_bDisableWT)
		{
			GetAxisInformation();
			
			//Get Channel attribute from config file
			lMotorDirectionX	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_X, MS896A_CFG_CH_MOTOR_DIRECTION);				
			lMotorDirectionY	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_Y, MS896A_CFG_CH_MOTOR_DIRECTION);	
			m_lMotorDirectionX	= lMotorDirectionX;
			m_lMotorDirectionY	= lMotorDirectionY;

			m_lX_ProfileType	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_X,	MS896A_CFG_CH_PROFILE_TYPE);
			m_lY_ProfileType	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_Y,	MS896A_CFG_CH_PROFILE_TYPE);
			(*m_psmfSRam)["MS896A"]["WaferTableX_ProfileType"] = m_lX_ProfileType;
			(*m_psmfSRam)["MS896A"]["WaferTableY_ProfileType"] = m_lY_ProfileType;

			// Select the profile
			X1_Profile(NORMAL_PROF);
			Y1_Profile(NORMAL_PROF);
			T1_Profile(NORMAL_PROF);

			if ( WaitBHInit(120000) == FALSE )	
			{
				DisplayMessage("WaferTable - Timeout when wait BondHead init ");
				return FALSE;
			}

			// Home the motors
			//T_Home(TRUE);

			SetWTinitT(TRUE);

			// Wait expander down before XY Home	//v2.63
			BOOL bWaferLoader = (BOOL)(LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"];
			if ( bWaferLoader && WaitExpInitForWt(90000) == FALSE )		//v4.56A1		
			{
				DisplayMessage("WaferTable - Timeout when wait exp down");
				return FALSE;
			}

			X_Home();
			Y_Home();
			m_bHome_T = TRUE;

			// Clear the last warning
			CMS896AStn::MotionSetLastWarning(WT_AXIS_X, HP_SUCCESS, &m_stWTAxis_X);
			CMS896AStn::MotionSetLastWarning(WT_AXIS_Y, HP_SUCCESS, &m_stWTAxis_Y);
			CMS896AStn::MotionSetLastWarning(WT_AXIS_T, HP_SUCCESS, &m_stWTAxis_T);
		}
		else if (m_fHardware && m_bDisableWT)	//v3.61
		{
			SetWTinitT(TRUE);		//Allow WL to proceed expander checking
			m_bSel_X	= FALSE;	
			m_bSel_Y	= FALSE;
			m_bSel_T	= FALSE;
			m_bSel_X2	= FALSE;	
			m_bSel_Y2	= FALSE;
			m_bSel_T2	= FALSE;
		}

		m_bHardwareReady = TRUE;

	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	if (!CMS896AStn::InitWaferMap())
	{
		CMS896AStn::m_bWaferMapWarmStartFail = TRUE;	//v4.22T1
	}

	m_szMapFilePath = (*m_psmfSRam)["MS896A"]["MapFilePath"];
	m_szMapFileExtension = (*m_psmfSRam)["MS896A"]["MapFileExt"];

    //Block Class Instance 
	m_pBlkFunc = new CWT_BlkFunc;
	m_pBlkFunc->SetWaferTable(this);
	m_pBlkFunc2 = new CWT_BlkFunc2;		//Block2
	m_pBlkFunc2->SetWaferTable(this);
	//SWalk 1st-Die Finder object
	m_p1stDieFinder = new CWT_1stDieFinder;
	m_p1stDieFinder->SetWaferTable(this);

	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

	m_WaferMapEvent.SetWaferTable(this);
	m_BinMapEvent.SetWaferTableBin(this);
	m_stStopTime = CTime::GetCurrentTime();	// machine INIT instance
	m_stMachineStopTime = m_stStopTime;		//v4.59A41
	m_dStopTime		= GetTime();
	m_dIdleDiffTime	= 0;

	//Get stored SCN data
	DumpScnData(FALSE);

	m_bIsCP100Item2Enable = FALSE;
	m_bIsCP100Item4Enable = FALSE;

	m_bEnableAlarmLog	= pApp->GetProfileInt(gszPROFILE_SETTING,	_T("Enable Alarm Log"), 0);
	m_bACF_AlarmLogPath	= pApp->GetProfileInt(gszPROFILE_SETTING,	_T("ACF Alarm Log Path"), 0);
	m_szAlarmLogPath	= pApp->GetProfileString(gszPROFILE_SETTING,_T("Alarm Log Path"), _T(""));
	if( m_szAlarmLogPath.IsEmpty() )
	{
		m_szAlarmLogPath = gszUSER_DIRECTORY + "\\Alarm Log";
	}

	return TRUE;
}


INT CWaferTable::ExitInstance()
{
	//SaveData();			// Save data to file before exit
	if (m_fHardware && !m_bDisableWT)
	{
		//v3.15T5		//Avoid WT data to be overwritten by empty data due to improper startup
		try 
		{
			CMS896AStn::MotionSync(WT_AXIS_X, 10000, &m_stWTAxis_X);
			CMS896AStn::MotionJoyStickOn(WT_AXIS_X, FALSE, &m_stWTAxis_X);
			CMS896AStn::MotionPowerOff(WT_AXIS_X, &m_stWTAxis_X);
			m_bIsPowerOn_X = FALSE;
			CMS896AStn::MotionClearError(WT_AXIS_X, HP_MOTION_ABORT, &m_stWTAxis_X);
		}
		catch (CAsmException e)
		{
			// Nothing can do ...
		}

		try
		{
			CMS896AStn::MotionSync(WT_AXIS_Y, 10000, &m_stWTAxis_Y);
			CMS896AStn::MotionJoyStickOn(WT_AXIS_Y, FALSE, &m_stWTAxis_Y);
			CMS896AStn::MotionPowerOff(WT_AXIS_Y, &m_stWTAxis_Y);
			m_bIsPowerOn_Y = FALSE;
			CMS896AStn::MotionClearError(WT_AXIS_Y, HP_MOTION_ABORT, &m_stWTAxis_Y);

		}
		catch (CAsmException e)
		{
			// Nothing can do ...
		}

		try
		{
			CMS896AStn::MotionSync(WT_AXIS_T, 10000, &m_stWTAxis_T);
			CMS896AStn::MotionJoyStickOn(WT_AXIS_T, FALSE, &m_stWTAxis_T);
			CMS896AStn::MotionPowerOff(WT_AXIS_T, &m_stWTAxis_T);
			m_bIsPowerOn_T = FALSE;
			CMS896AStn::MotionClearError(WT_AXIS_T, HP_MOTION_ABORT, &m_stWTAxis_T);
		}
		catch (CAsmException e)
		{
			// Nothing can do ...
		}

		if ( IsESDualWT() )	//v4.24
		{
			try 
			{
				CMS896AStn::MotionSync(WT_AXIS_X2, 10000, &m_stWTAxis_X2);
				CMS896AStn::MotionJoyStickOn(WT_AXIS_X2, FALSE, &m_stWTAxis_X2);
				CMS896AStn::MotionPowerOff(WT_AXIS_X2, &m_stWTAxis_X2);
				m_bIsPowerOn_X2 = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_X2, HP_MOTION_ABORT, &m_stWTAxis_X2);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}

			try
			{
				CMS896AStn::MotionSync(WT_AXIS_Y2, 10000, &m_stWTAxis_Y2);
				CMS896AStn::MotionJoyStickOn(WT_AXIS_Y2, FALSE, &m_stWTAxis_Y2);
				CMS896AStn::MotionPowerOff(WT_AXIS_Y2, &m_stWTAxis_Y2);
				m_bIsPowerOn_Y2 = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_Y2, HP_MOTION_ABORT, &m_stWTAxis_Y2);

			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}

			try
			{
				CMS896AStn::MotionSync(WT_AXIS_T2, 10000, &m_stWTAxis_T2);
				CMS896AStn::MotionJoyStickOn(WT_AXIS_T2, FALSE, &m_stWTAxis_T2);
				CMS896AStn::MotionPowerOff(WT_AXIS_T2, &m_stWTAxis_T2);
				m_bIsPowerOn_T2 = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_T2, HP_MOTION_ABORT, &m_stWTAxis_T2);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}
	}

	// Temperature Controller
	TC_ExitInstance();

	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return CMS896AStn::ExitInstance();
}

BOOL CWaferTable::InitData()
{
	if (IsLoadingPortablePKGFile() == TRUE && IsLoadingPKGFile() == TRUE)
	{
		LoadWaferTblData(TRUE);	// Load wafer Table Data
		SaveWaferTblData();
		LoadData();					// Load Data from String Map
		LoadBlkData();				// Load Block Function Data
		SaveData();
	}
	else
	{
		LoadWaferTblData(FALSE);	// Load wafer Table Data
		LoadData();					// Load Data from String Map
		LoadBlkData();				// Load Block Function Data
	}

	// load the wafer limit template selection
	if ((CMS896AStn::m_bEnableWaferSizeSelect == TRUE) && !IsMS90())
	{
		OnSelectWaferLimit(m_ucWaferLimitTemplateNoSel);
	}
	
	// Show the Wafer Lot Info on HMI
	ShowWaferLotData();

	//Reset wafer alignment status in case of user-switching or Warm Start
	SetAlignmentStatus(FALSE);	//pllm

	//Move to Working angle if necessary
	BOOL bWLExist = (BOOL)(LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"];

	if ( bWLExist == TRUE )
	{
		m_lWaferWorkingAngle = 0;
	}
	else
	{
		if ( (m_fHardware == TRUE) && (m_lWaferWorkingAngle != 0) )
		{
			if( IsWT2InUse() )
				T2_Home();
			else
				T_Home();
		}

		//v3.06
		if (m_fHardware)
		{
		}
	}

	//xyz
	//Auto rotate wafer if option is enabled
	if( (m_ucAutoWaferRotation > 0) )
	{
		INT nPos = GetAutoWaferT();
		T_MoveTo(nPos, SFM_WAIT);	// Move table
	}

	if( IsMS90HalfSortMode() )
	{
		if( IsMS90Sorting2ndPart() )	//	MS90 half sort mode, move to another part.
		{
			INT nPos = 0 - m_lThetaMotorDirection * (LONG)(180.0/m_dThetaRes);
			T_Move(nPos, SFM_WAIT);	// Move table
			MS90HalfSortMapAction(FALSE);
		}
		else
		{
			MS90HalfSortMapAction(TRUE);
		}
	}

	//v3.11T1
	LoadWaferMapAlgorithmSettings();

	SendLexter_S1F65(1);	//Init S1F65 in WARMSTART for Lextar	//Send S1F65 IDLE

	//Enable MOVE joystick mode

	CMSLogFileUtility::Instance()->WT_BackupLogStatus();
	CheckDiskSpace("C:\\", 20.0);		//v4.50A3

	return TRUE;
}

BOOL CWaferTable::LoadWftData()
{
	CMSLogFileUtility::Instance()->WL_LogStatus("to load data of wafer table");
	LoadWaferTblData(TRUE);			// Load wafer Table Data

	SaveWaferTblData();

	CMSLogFileUtility::Instance()->WL_LogStatus("Save data of wafer table");
	return TRUE;
}	// for run time to load PR records and prescan settings

VOID CWaferTable::ClearData()
{
	if ( m_pvNVRAM != NULL )
	{
		// NVRAM is only for wafer map. Simply clear all to zero
		char *pcTemp = (char*)((unsigned long)m_pvNVRAM + glNVRAM_WaferMap_Start);

		for ( LONG i=0; i < glNVRAM_WaferMap_Size; i++ )
			*(pcTemp+i) = 0;
	}
}

/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CWaferTable::Operation()
{
	switch(State())
	{		
		case IDLE_Q:
			IdleOperation();
			break;

		case DIAGNOSTICS_Q:
			DiagOperation();
			break;

		case SYSTEM_INITIAL_Q:
			InitOperation();
			break;

		case PRESTART_Q:
			PreStartOperation();
			break;

		case AUTO_Q:
			AutoOperation();
			break;

		case DEMO_Q:
			DemoOperation();
			break;

		case MANUAL_Q:
			ManualOperation();
			break;

		case ERROR_Q:
			ErrorOperation();
			break;

		case STOPPING_Q:
			StopOperation();
			break;

		case DE_INITIAL_Q:
			DeInitialOperation();
			break;

		default:
			State(IDLE_Q);
			Motion(FALSE);
			break;
	}

	//Move to CSafetyStn		//v3.13T4
	//m_WaferMapWrapper.Run();

	if ((State() == IDLE_Q) || (State() == UN_INITIALIZE_Q))
	{
		try
		{
			if (m_bUpdateOutput)
			{
				UpdateOutput();
			}

			if (m_bUpdateProfile)
			{
				UpdateProfile();
			}

			if (m_bUpdatePosition)
			{
				UpdatePosition();
			}

			if (m_bStartCalibrate)
			{
				MoveWaferTheta();
			}

			if (m_bStartCalibrate2)
			{
				MoveWafer2Theta();
			}

			MotionTest();

			if (m_bMapSyncMove)
			{
				MapSyncMove();
			}

			if (m_bStartGoToAlignDie)	//v3.30T1
			{
				GoToAlignDie();
			}
// Temperature Controller
			if ( m_fHardware && TC_IsEnable() )
			{
				TC_AutoTuneEjector();
			}
			if ( m_fHardware && IsLayerPicking() )
			{
				m_bAlarmBitOn = TC_CheckAlarmOn();
			}

			NVC_MotionTest();		//NVC v0.02

		}
		catch (CAsmException e)
		{
			DisplayException(e);	
		}
		Sleep(10);
	}
	else if (State() == ERROR_Q)
	{
		Sleep(10);
	}
	else
	{
		Sleep(0);
	}
}

// Get all sensor values from hardware
VOID CWaferTable::GetSensorValue()
{
	if (m_fHardware && !m_bDisableWT)	//v3.61
	{
		try
		{
			m_bHomeSnr_X	= CMS896AStn::MotionIsHomeSensorHigh(WT_AXIS_X,		&m_stWTAxis_X);
			m_bHomeSnr_Y	= CMS896AStn::MotionIsHomeSensorHigh(WT_AXIS_Y,		&m_stWTAxis_Y);
			m_bHomeSnr_T	= !CMS896AStn::MotionIsHomeSensorHigh(WT_AXIS_T,		&m_stWTAxis_T);
			//m_bLimitSnr_X	= CMS896AStn::MotionIsPositiveLimitHigh(WT_AXIS_X,	&m_stWTAxis_X);
			//m_bLimitSnr_Y	= CMS896AStn::MotionIsPositiveLimitHigh(WT_AXIS_Y,	&m_stWTAxis_Y);
			
			//if ( IsESDualWT() )		//	ES101_XU
			//{
			//	m_bHomeSnr_X2	= CMS896AStn::MotionIsHomeSensorHigh(WT_AXIS_X2,	&m_stWTAxis_X2);
			//	m_bHomeSnr_Y2	= CMS896AStn::MotionIsHomeSensorHigh(WT_AXIS_Y2,	&m_stWTAxis_Y2);
			//	m_bHomeSnr_T2	= CMS896AStn::MotionIsHomeSensorHigh(WT_AXIS_T2,	&m_stWTAxis_T2);
			//	m_bLimitSnr_X2	= CMS896AStn::MotionIsPositiveLimitHigh(WT_AXIS_X2,	&m_stWTAxis_X2);
			//	m_bLimitSnr_Y2	= CMS896AStn::MotionIsPositiveLimitHigh(WT_AXIS_Y2,	&m_stWTAxis_Y2);
			//}

			//if (CMS896AApp::m_bMS100Plus9InchOption)	//MS109		//v4.38T6
			//{
			//	m_bEJTLLimit = IsEJTAtUnloadPosn();
			//	m_bEJTULimit = !IsEJTAtUnloadPosn();
			//}

		}
		catch(CAsmException e)
		{
			DisplayException(e);
		}
	}
}

VOID CWaferTable::GetCmdValue()
{
	if (m_fHardware && !m_bDisableWT)
	{
		try
		{
			//Report Wafer Y encoder
			m_lCmd_X = CMS896AStn::MotionGetCommandPosition(WT_AXIS_X, &m_stWTAxis_X);
			m_lCmd_Y1 = CMS896AStn::MotionGetCommandPosition(WT_AXIS_Y, &m_stWTAxis_Y);

			if ( m_bSel_Y2 )
			{
				m_lCmd_Y2 = CMS896AStn::MotionGetCommandPosition(WT_AXIS_Y2, &m_stWTAxis_Y2);
			}

			if( IsWT2InUse() )
			{
				m_lCmd_Y = m_lCmd_Y2;
			}
			else
			{
				m_lCmd_Y = m_lCmd_Y1;
			}
		}
		catch(CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
			if( IsESDualWT() )
			{
				CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
				CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
				CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
			}
		}
	}
}

// Get all encoder values from hardware
VOID CWaferTable::GetEncoderValue()
{
	if (m_fHardware && !m_bDisableWT)
	{
		try
		{
			//Report Wafer X encoder
			if (CMS896AStn::MotionIsServo(WT_AXIS_X, &m_stWTAxis_X))
			{
				m_lEnc_X1 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_X, 1, &m_stWTAxis_X);
			}
			else
			{
				m_lEnc_X1 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_X, 0.8, &m_stWTAxis_X);
			}

			if (m_lMotorDirectionX == -1)
			{
				m_lEnc_X1 = -1 * m_lEnc_X1;
			}

			//Report Wafer Y encoder
			if (CMS896AStn::MotionIsServo(WT_AXIS_Y, &m_stWTAxis_Y))
			{
				m_lEnc_Y1 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_Y, 1, &m_stWTAxis_Y);
			}
			else
			{
				m_lEnc_Y1 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_Y, 0.8, &m_stWTAxis_Y);
			}
			
			if (m_lMotorDirectionY == -1)
			{
				m_lEnc_Y1 = -1 * m_lEnc_Y1;
			}

			if (m_bEnableWTTheta)
			{
				//Report Wafer Theta encoder
				if (CMS896AStn::MotionIsServo(WT_AXIS_T, &m_stWTAxis_T))
				{
					m_lEnc_T1 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_T, 1, &m_stWTAxis_T);
				}
				else
				{
					if (IsMS90())				//v4.48A27
					{
						m_lEnc_T1 = CMS896AStn::MotionGetCommandPosition(WT_AXIS_T, &m_stWTAxis_T);
					}
					else if ( CMS896AApp::m_bMS100Plus9InchOption)	//v4.16T3	//MS100 9Inch
						m_lEnc_T1 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_T, 1.6, &m_stWTAxis_T);
					else
						m_lEnc_T1 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_T, 0.8, &m_stWTAxis_T);
				}
			}

			if ( IsESDualWT() )		//v4.24
			{
				m_lEnc_X2 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_X2, 1,		&m_stWTAxis_X2);
				m_lEnc_Y2 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_Y2, 1,		&m_stWTAxis_Y2);
				m_lEnc_T2 = CMS896AStn::MotionGetEncoderPosition(WT_AXIS_T2, 0.8,	&m_stWTAxis_T2);
			}

			if( IsWT2InUse() )
			{
				m_lEnc_X = m_lEnc_X2;
				m_lEnc_Y = m_lEnc_Y2;
				m_lEnc_T = m_lEnc_T2;
			}
			else
			{
				m_lEnc_X = m_lEnc_X1;
				m_lEnc_Y = m_lEnc_Y1;
				m_lEnc_T = m_lEnc_T1;
			}
		}
		catch(CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
			if( IsESDualWT() )
			{
				CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
				CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
				CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
			}
		}
	}
}

// Return all encoder values
VOID CWaferTable::GetEncoder(LONG *lX, LONG *lY, LONG *lT)
{
	if (m_fHardware && !m_bDisableWT)	//v3.61
	{
		GetEncoderValue();
		*lX = GetCurrX();
		*lY = GetCurrY();
		*lT = GetCurrT();
	}
}

LONG CWaferTable::GetCurrX()
{
	return m_lEnc_X;
}

LONG CWaferTable::GetCurrY()
{
	return m_lEnc_Y;
}

LONG CWaferTable::GetCurrCmdY()
{
	return m_lCmd_Y;
}

LONG CWaferTable::GetCurrT()
{
	return m_lEnc_T;
}

LONG CWaferTable::GetCurrX2()
{
	return m_lEnc_X2;
}

LONG CWaferTable::GetCurrY2()
{
	return m_lEnc_Y2;
}

LONG CWaferTable::GetCurrCmdY2()
{
	return m_lCmd_Y2;
}

LONG CWaferTable::GetCurrT2()
{
	return m_lEnc_T2;
}

// Get Theta Resolution
VOID CWaferTable::GetThetaRes(DOUBLE *dRes)
{
	*dRes = m_dThetaRes;
}


VOID CWaferTable::GetAxisInformation()
{
	InitAxisData(m_stWTAxis_X);
	m_stWTAxis_X.m_szName				= WT_AXIS_X;
	m_stWTAxis_X.m_szTag				= MS896A_CFG_CH_WAFTABLE_X;
	m_stWTAxis_X.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stWTAxis_X);
	m_stWTAxis_X.m_dEncResolution		= 1;

	InitAxisData(m_stWTAxis_Y);
	m_stWTAxis_Y.m_szName				= WT_AXIS_Y;
	m_stWTAxis_Y.m_szTag				= MS896A_CFG_CH_WAFTABLE_Y;
	m_stWTAxis_Y.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stWTAxis_Y);
	m_stWTAxis_Y.m_dEncResolution		= 1;

	if (m_bEnableWTTheta)
	{
		InitAxisData(m_stWTAxis_T);
		m_stWTAxis_T.m_szName				= WT_AXIS_T;
		m_stWTAxis_T.m_szTag				= MS896A_CFG_CH_WAFTABLE_T;
		m_stWTAxis_T.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stWTAxis_T);
		m_stWTAxis_T.m_dEncResolution		= 1;
	}

	if ( IsESDualWT() )
	{
		InitAxisData(m_stWTAxis_X2);
		m_stWTAxis_X2.m_szName				= WT_AXIS_X2;
		m_stWTAxis_X2.m_szTag				= MS896A_CFG_CH_WAFTABLE2_X;
		m_stWTAxis_X2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stWTAxis_X2);
		m_stWTAxis_X2.m_dEncResolution		= 1;

		InitAxisData(m_stWTAxis_Y2);
		m_stWTAxis_Y2.m_szName				= WT_AXIS_Y2;
		m_stWTAxis_Y2.m_szTag				= MS896A_CFG_CH_WAFTABLE2_Y;
		m_stWTAxis_Y2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stWTAxis_Y2);
		m_stWTAxis_Y2.m_dEncResolution		= 1;

		InitAxisData(m_stWTAxis_T2);
		m_stWTAxis_T2.m_szName				= WT_AXIS_T2;
		m_stWTAxis_T2.m_szTag				= MS896A_CFG_CH_WAFTABLE2_T;
		m_stWTAxis_T2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stWTAxis_T2);
		m_stWTAxis_T2.m_dEncResolution		= 1;

		PrintAxisData(m_stWTAxis_X2);
		PrintAxisData(m_stWTAxis_Y2);
		PrintAxisData(m_stWTAxis_T2);
	}

	//v4.59A39	//MS50
	m_bMS100EjtXY				= (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X,		MS896A_CFG_CH_ENABLE);
	if (m_bMS100EjtXY)
	{
		InitAxisData(m_stBHAxis_EjX);
		m_stBHAxis_EjX.m_szName				= BH_AXIS_EJ_X;
		m_stBHAxis_EjX.m_szTag				= MS896A_CFG_CH_EJECTOR_X;
		m_stBHAxis_EjX.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBHAxis_EjX);
		m_stBHAxis_EjX.m_dEncResolution		= 1;
	
		InitAxisData(m_stBHAxis_EjY);
		m_stBHAxis_EjY.m_szName				= BH_AXIS_EJ_Y;
		m_stBHAxis_EjY.m_szTag				= MS896A_CFG_CH_EJECTOR_Y;
		m_stBHAxis_EjY.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBHAxis_EjY);
		m_stBHAxis_EjY.m_dEncResolution		= 1;


		PrintAxisData(m_stBHAxis_EjX);
		PrintAxisData(m_stBHAxis_EjY);
	}

	//For debug only
	PrintAxisData(m_stWTAxis_X);
	PrintAxisData(m_stWTAxis_Y);
	if (m_bEnableWTTheta)
	{
		PrintAxisData(m_stWTAxis_T);
	}
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
VOID CWaferTable::UpdateOutput()
{
	static int nCount = 0;

	if (nCount++ < 15)
	{
		return;
	}
	else
	{
		nCount = 0;
	}

	try
	{
		if (m_fHardware && !m_bDisableWT)	//v3.61
		{
			GetSensorValue();
			GetEncoderValue();

			m_bIsPowerOn_X = CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X);
			m_bIsPowerOn_Y = CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y);
			if (m_bEnableWTTheta)
			{
				m_bIsPowerOn_T = CMS896AStn::MotionIsPowerOn(WT_AXIS_T, &m_stWTAxis_T);
			}
			if ( IsESDualWT() )		//v4.24
			{
				m_bIsPowerOn_X2 = CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2);
				m_bIsPowerOn_Y2 = CMS896AStn::MotionIsPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2);
				m_bIsPowerOn_T2 = CMS896AStn::MotionIsPowerOn(WT_AXIS_T2, &m_stWTAxis_T2);
			}
		}
	}
	catch(CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
		CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
		if( IsESDualWT() )
		{
			CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
		}
	}
}

VOID CWaferTable::UpdateProfile()
{
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CSingleLock slLock(&pAppMod->m_csDevice);	//???
	m_bUpdateProfile = FALSE;

	try
	{
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
	slLock.Unlock();		//???
}

VOID CWaferTable::UpdatePosition()
{
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CSingleLock slLock(&pAppMod->m_csDevice);
	
	m_bUpdatePosition = FALSE;

//	m_bSelTable	= (LONG) (*m_psmfDevice)["Select"]["WaferTable"];

	slLock.Unlock();
}

/*
//Check Motor Type
LONG CWaferTable::CheckThetaMotorType(void)
{
	if (m_pServo_T && (m_pStepper_T == NULL))
	{
		return WT_SVO_MOTOR;
	}
	else
	{
		return WT_STP_MOTOR;
	}
}

LONG CWaferTable::CheckXMotorType(void)
{
	if (m_pServo_X && (m_pStepper_X == NULL))
	{
		return WT_SVO_MOTOR;
	}
	else
	{
		return WT_STP_MOTOR;
	}
}

LONG CWaferTable::CheckYMotorType(void)
{
	if (m_pServo_Y && (m_pStepper_Y == NULL))
	{
		return WT_SVO_MOTOR;
	}
	else
	{
		return WT_STP_MOTOR;
	}
}
*/

//Get return of wafertable member
IPC_CClientCom& CWaferTable::GetIPCClientCom()
{
	return m_comClient;
}

BOOL CWaferTable::GetRtnForCheckWaferLimit(LONG lX, LONG lY)
{
	return IsWithinWaferLimit(lX,lY) && IsWithinMapDieLimit(lX, lY);
}

INT CWaferTable::GetRtnForTMoveTo(INT nPos, INT nMode)	//Block2
{
	return T_MoveTo(nPos, nMode);
}

BOOL CWaferTable::GetRtnForRotateWaferTheta(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree)
{
	return RotateWaferTheta(lX, lY, lTheta, dDegree);
}

BOOL CWaferTable::GetRtnForMachineNo(CString *strMchNo)
{
	*strMchNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
	return TRUE;
}

BOOL CWaferTable::GetRtnForSwVerNo(CString *strSwVerNo)
{
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	*strSwVerNo = pAppMod->m_szAppSWVersion;
	return TRUE;
}

BOOL CWaferTable::GetRtnForMapFileNm(CString *strMapFileNm)
{
	*strMapFileNm = m_szMapFileName;
	return TRUE;
}

BOOL CWaferTable::GetRtnSetCtmLogMessage(const CString& szStatus)	//Block2
{
	SetCtmLogMessage(szStatus);

	return TRUE;
}

BOOL CWaferTable::GetRtnSetErrorLogMessage(const CString& szStatus)	//block2
{
	SetErrorMessage(szStatus);

	return TRUE;
}

BOOL CWaferTable::GetDiePitchX(LONG *lX, LONG *lY)
{
	*lX	= GetDiePitchX_X();
	*lY	= GetDiePitchX_Y();
	return TRUE;
}

BOOL CWaferTable::GetDiePitchY(LONG *lX, LONG *lY)
{
	*lX	= GetDiePitchY_X();
	*lY	= GetDiePitchY_Y();
	return TRUE;
}

VOID CWaferTable::GetRtnForSetJoystick(BOOL bOn)
{
	SetJoystickOn(bOn);

	if( m_ulJoyStickMode==MS899_JS_MODE_PR )
	{
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(BOOL), &bOn);
		int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetMouseControlCmd", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				break;
			}
			else
			{
				Sleep(10);	
			}
		}
	}
}

LONG CWaferTable::GetRtnForHmiMessage(const CString& szText,const CString& szTitle,
							LONG lType,
							LONG lAlignment,
							LONG lTimeout,
							LONG lMsgMode,
							LONG* plMsgID,
							LONG lWidth,
							LONG lHeight,
							CString* pszBitmapFile,
							CString* pszCustomText1,
							CString* pszCustomText2,
							CString* pszCustomText3)
{
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szText);		//v4.43T10
	return HmiMessage( szText,szTitle,lType,lAlignment,lTimeout,lMsgMode,
				plMsgID,lWidth,lHeight,pszBitmapFile,pszCustomText1,pszCustomText2,pszCustomText3);
}

LONG CWaferTable::GetRtnForHmiMessageEx(const CString& szText,const CString& szTitle,
							LONG lType,
							LONG lAlignment,
							LONG lTimeout,
							LONG lMsgMode,
							LONG* plMsgID,
							LONG lWidth,
							LONG lHeight,
							CString* pszBitmapFile,
							CString* pszCustomText1,
							CString* pszCustomText2,
							CString* pszCustomText3)
{
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szText);		//av4.43T10
	return HmiMessageEx( szText,szTitle,lType,lAlignment,lTimeout,lMsgMode,
				plMsgID,lWidth,lHeight,pszBitmapFile,pszCustomText1,pszCustomText2,pszCustomText3);
}

BOOL CWaferTable::MoveBackLightToSafeLevel()
{
	BOOL bReturn = TRUE;

	if( IsBLInUse() )	// 4.24TX
	{
		IPC_CServiceMessage stMsg;

		INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MoveBackLightToSafeLevel", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		stMsg.GetMsg(sizeof(BOOL), &bReturn);

		if (bReturn == FALSE)
		{
			SetAlert_Red_Yellow(IDS_WT_BL_NOT_IN_SAFE_POS);
		}
	}

	return bReturn;
}

BOOL CWaferTable::MoveEjectorElevatorToSafeLevel()
{
	BOOL bReturn = TRUE;

	if( IsEjtElvtInUse() )	// 4.24TX
	{
		IPC_CServiceMessage stMsg;

		INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "MoveEjeElvtToSafeLevel", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		stMsg.GetMsg(sizeof(BOOL), &bReturn);

		if (bReturn == FALSE)
		{
			SetAlert_Red_Yellow(IDS_WT_EJEL_NOT_IN_SAFE_POS);
		}
	}

	return bReturn;

}

BOOL CWaferTable::NVC_MotionTest()
{
	if (CMS896AStn::m_ucMotionAxis == 0)
	{
		return TRUE;
	}
	if ((CMS896AStn::m_ucMotionAxis != 9) && (CMS896AStn::m_ucMotionAxis != 10))
	{
		return TRUE;
	}

	BOOL bAbortTest = FALSE;

	if (CMS896AStn::m_bMoveDirection)
	{
		CString szLog;
		szLog.Format("Motion WT Test #%d +ive MOVE = %ld", m_ucMotionAxis, m_lMotionTarEncPosn);
		//HmiMessage(szLog);

		switch (m_ucMotionAxis)	//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
		{
		case 9:			//WT X
			X_MoveTo(m_lMotionTarEncPosn);
			break;
		case 10:		//WT Y
			Y_MoveTo(m_lMotionTarEncPosn);
			break;
		}

		m_bMoveDirection = FALSE;
	}
	else
	{
		CString szLog;
		szLog.Format("Motion WT Test #%d -ive MOVE = %ld", m_ucMotionAxis, m_lMotionOrigEncPosn);
		//HmiMessage(szLog);

		switch (m_ucMotionAxis)	//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
		{
		case 9:			//WT X
			X_MoveTo(m_lMotionOrigEncPosn);
			break;
		case 10:		//WT Y
			Y_MoveTo(m_lMotionOrigEncPosn);
			break;
		}

		m_bMoveDirection = TRUE;

		m_lMotionCycleCount++;
		if ((m_lMotionCycleCount >= m_lMotionCycle) || (m_lMotionCycleCount >= 100))
		{
			m_ucMotionAxis = 0;
			m_lMotionCycleCount = 0;
			bAbortTest = TRUE;
		}

		if (m_bMotionCycleStop)
		{
			m_ucMotionAxis = 0;
			m_lMotionCycleCount = 0;
			m_bMotionCycleStop = FALSE;
			bAbortTest = TRUE;
		}

		if (bAbortTest)
		{
			switch (m_ucMotionAxis)	//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
			{
			case 9:			//WT X
				//LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, FALSE, 5, m_stDataLog);
				break;
			case 10:		//WT Y
				//LogAxisPerformance(BH_AXIS_EJ_T, &m_stBHAxis_EjT, FALSE, 5, m_stDataLog);
				break;
			}
		}

	}

	if ((m_lMotionTestDelay >= 100) && (m_lMotionTestDelay <= 5000))
	{
		Sleep(m_lMotionTestDelay);
	}
	else
	{
		Sleep(1000);
	}

	return TRUE;
}