/////////////////////////////////////////////////////////////////
// WT_State.cpp : Operation State of the CWaferTable class
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
#include "WT_Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
//	Operation State Functions
/////////////////////////////////////////////////////////////////

VOID CWaferTable::IdleOperation()
{
	switch (Command())
	{
	case glAMS_NULL_COMMAND:
		Sleep(20);
		break;

	case glAMS_DIAGNOSTIC_COMMAND:
		State(DIAGNOSTICS_Q);
		Command(glAMS_NULL_COMMAND);
		break;

	case glAMS_INITIAL_COMMAND:
		State(SYSTEM_INITIAL_Q);
		Command(glAMS_NULL_COMMAND);
		break;

	case glAMS_PRESTART_COMMAND:
		State(PRESTART_Q);
		Command(glAMS_NULL_COMMAND);
		break;

	case glAMS_AUTO_COMMAND:
		State(AUTO_Q);
		Command(glAMS_NULL_COMMAND);
		break;

	case glAMS_MANUAL_COMMAND:
		m_lManualAction	= m_lAction;
		State(MANUAL_Q);
		Command(glAMS_NULL_COMMAND);
		break;

	case glAMS_DEMO_COMMAND:
		State(DEMO_Q);
		Command(glAMS_NULL_COMMAND);
		break;

	case glAMS_CYCLE_COMMAND:
		State(CYCLE_Q);
		Command(glAMS_NULL_COMMAND);
		break;

	case glAMS_STOP_COMMAND:
	case glAMS_RESET_COMMAND:
		Motion(FALSE);
		Command(glAMS_NULL_COMMAND);
		break;

	case glAMS_DEINITIAL_COMMAND:
		State(DE_INITIAL_Q);
		Command(glAMS_NULL_COMMAND);
		break;

	default:
		Command(glAMS_NULL_COMMAND);
		State(IDLE_Q);
		Result(gnNOTOK);
		SetAlert(IDS_SYS_INVALID_COMD);
		break;
	}
}

void CWaferTable::DiagOperation()
{
	m_nLastError	= gnOK;
	
	LONG	lAxis, lAction, lUnit;
	AxisAction(lAxis, lAction, lUnit);
	//SFM_CHipecAcServo*		pServo;

	try
	{
		CString	szAxisID = "";
		
		if (m_fHardware && !m_bDisableWT)	//v3.61
		{
			szAxisID = GetAxisID(lAxis);
		}

		CString csMsg;
		csMsg.Format("WT - Diagnostics - Axis %d, Action %d, Unit %d", lAxis, lAction, lUnit);
		DisplaySequence(csMsg);

		switch (lAction)
		{
		case glPOSITION_ACTION:
			switch (lAxis)
			{
			case 0:
				if (lUnit == HOME_POS)
				{
					SetInitHome(szAxisID, FALSE);
					if( IsWT2InUse() )
						m_nLastError = X12_Home();
					else
						m_nLastError = X_Home();
				}
				break;

			case 1:
				if (lUnit == HOME_POS)
				{
					SetInitHome(szAxisID, FALSE);
					if( IsWT2InUse() )
					{
						m_nLastError = Y2_Home();
					}
					else
					{
						m_nLastError = Y_Home();
					}
				}
				break;

			case 2:
				if (lUnit == HOME_POS)
				{
					SetInitHome(szAxisID, FALSE);
					if( IsWT2InUse() )
						m_nLastError = T2_Home();
					else
						m_nLastError = T_Home();
				}
				break;

			case 3:
				if (lUnit == HOME_POS)
				{
					m_nLastError = XY_Home();
				}
				break;
			}
			break;

		case glRELATIVE_ACTION:
			switch (lAxis)
			{
			case 0:
				m_nLastError = X_Move(lUnit);
				break;

			case 1:
				m_nLastError = Y_Move(lUnit);
				break;

			case 2:
				m_nLastError = T_Move(lUnit);
				break;
			}
			break;

		case glABSOLUTE_ACTION:
			switch (lAxis)
			{
			case 0:
				m_nLastError = X_MoveTo(lUnit);
				break;

			case 1:
				m_nLastError = Y_MoveTo(lUnit);
				break;

			case 2:
				m_nLastError = T_MoveTo(lUnit);
				break;
			}
			break;

		case  glFAST_HOME_ACTION:
			if (m_fHardware && !m_bDisableWT)	//v3.61
			{
				m_nLastError = MoveActuator(szAxisID, GetPositionID(szAxisID, FAST_HOME_POS, lUnit));
			}
			break;

		case glPOWER_ON_ACTION:
			if (m_fHardware && !m_bDisableWT)	//v3.61
			{
				if (lUnit == 0)
				{
					PowerOffAxis(szAxisID);
				}
				else
				{
					PowerOnAxis(szAxisID);
				}
			}
			break;

		case 88:
			INT i;
			for ( i=0; i < 10; i++ )
			{

				X_Move(lUnit, SFM_NOWAIT);
				Y_Move(lUnit, SFM_WAIT);

				X_Sync();

				X_Move(-lUnit, SFM_NOWAIT);
				Y_Move(-lUnit, SFM_WAIT);
				X_Sync();
			}
			break;

		case glSELECT_PROFILE_ACTION:
			switch (lAxis)
			{
			case 0:
				m_nLastError = X_Profile(lUnit);
				break;

			case 1:
				m_nLastError = Y_Profile(lUnit);
				break;

			case 2:
				m_nLastError = T_Profile(lUnit);
				break;
			}
			break;

		case glJOY_STICK_ACTION:
			if (m_fHardware && !m_bDisableWT)	//v3.61
			{
				if (lUnit == 0)
				{
					SetJoystickOn(FALSE);
				}
				else
				{
					SetJoystickOn(TRUE);
				}
			}
			break;
/*
		case glSELECT_JOY_PRF_ACTION:
			switch (lAxis)
			{
			case 0:
				m_nLastError = XJoyProfile(lUnit);
				break;

			case 1:
				m_nLastError = YJoyProfile(lUnit);
				break;
			}
			break;
*/
		case glCOMMUTATE_ACTION:
			if (m_fHardware && !m_bDisableWT)	//v3.61
			{
				m_nLastError = CommutateServo(szAxisID);

				switch (lAxis)
				{
				case 0:
					if (m_nLastError == gnOK)
					{
						m_bComm_X			= TRUE;
					}
					else
					{
						m_bComm_X			= FALSE;
						m_nLastError		= Err_WTableXCommutate;
					}
					break;

				case 1:
					if (m_nLastError == gnOK)
					{
						m_bComm_Y			= TRUE;
					}
					else
					{
						m_bComm_Y			= FALSE;
						m_nLastError		= Err_WTableYCommutate;
					}
					break;

				case 2:
					if (m_nLastError == gnOK)
					{
						m_bComm_T			= TRUE;
					}
					else
					{
						m_bComm_T			= FALSE;
						m_nLastError		= Err_WTableTCommutate;
					}
					break;
				}
			}
			break;

		case glCLEAR_SERVO_ACTION:
			if (m_fHardware && !m_bDisableWT)	//v3.61
			{
				ClearServoError(szAxisID);
			}
		}
	}
	catch (CAsmException e)
	{
		NeedReset(TRUE);
		DisplayException(e);
	}
	
//	SetAlert(m_nLastError);

	Motion(FALSE);
	State(IDLE_Q);
}

VOID CWaferTable::InitOperation()
{
	m_nLastError	= gnOK;

	try
	{
		m_nLastError = OpInitialize();

		if (m_nLastError != gnOK)
		{
			if (!IsAbort())
			{
				SetAlert(m_nLastError);
			}

			Result(gnNOTOK);
		}

		DisplaySequence("WT - Initialize Operation Completed");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		NeedReset(TRUE);
		Result(gnNOTOK);
	}

	Motion(FALSE);
	State(IDLE_Q);
}

VOID CWaferTable::PreStartOperation()
{
 	//	PreStartCycle		start option
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	m_nLastError	= gnOK;

	if( IsAOIOnlyMachine() && IsBurnIn())
	{
		INT j;
		unsigned char aaGrades[256];
		CUIntArray aulAvailableGradeList;

		m_WaferMapWrapper.GetAvailableGradeList(aulAvailableGradeList);

		for(j=0; j<aulAvailableGradeList.GetSize(); j++)
			aaGrades[j] = aulAvailableGradeList[j];

		if (aulAvailableGradeList.GetSize() > 0)
		{
			m_WaferMapWrapper.SelectGrade(aaGrades, (unsigned long)aulAvailableGradeList.GetSize());
		}
	}

	m_lTDelay = (*m_psmfSRam)["WaferTable"]["TDelay"];
	try
	{
		m_nLastError = OpPreStart();

#ifdef NU_MOTION
		if (m_bEnableWTMotionLog)
		{
			if( IsESDualWT() )
			{
				LogAxisPerformance3(WT_AXIS_X2, WT_AXIS_Y2, WT_AXIS_T2, &m_stWTAxis_X2, &m_stWTAxis_Y2, &m_stWTAxis_T2, TRUE);
			}
			LogAxisPerformance3(WT_AXIS_X, WT_AXIS_Y, WT_AXIS_T, &m_stWTAxis_X, &m_stWTAxis_Y, &m_stWTAxis_T, TRUE);
		}
#endif

		if (m_nLastError != gnOK)
		{
			if (!IsAbort())
			{
				SetAlert(m_nLastError);
			}

			Result(gnNOTOK);
		}

		DisplaySequence("WT - PreStartOperation Completed");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		NeedReset(TRUE);
		Result(gnNOTOK);
	}

	Motion(FALSE);
	State(IDLE_Q);
	SaveScanTimeEvent("    WFT: prestart into idle q");
	//GetDie & GetScn Log
	CString szSrcFile = gszUSER_DIRECTORY + "\\History\\Alarm.log";
	CString szTgtFile = gszUSER_DIRECTORY + "\\History\\Alarm.bak";
	DeleteFile(szTgtFile);
	RenameFile(szSrcFile, szTgtFile);
	if( CMSLogFileUtility::Instance()->GetEnableMachineLog() && DEB_IsEnable() )
	{
		CMSLogFileUtility::Instance()->WT_OpenMylarShiftLog(m_stPrescanStartTime);
		if( m_bJustAlign )
		{
			CString szMsg;
			szMsg.Format("%ld,%ld,%ld,%ld,%ld,%ld", 
				GetDieSizeX(), GetDieSizeY(), GetDiePitchX_X(), GetDiePitchY_Y(), m_lPitchX_Tolerance, m_lPitchY_Tolerance);
			CMSLogFileUtility::Instance()->WT_WriteMylarShiftLog(szMsg);
		}
	}
	if ( CMSLogFileUtility::Instance()->GetEnableWtMachineLog() )
	{
		//Delete previous log file if map is aligned or now loaded
		if ( m_bJustAlign == TRUE )
		{
			DeleteFile(gszUSER_DIRECTORY + WT_GET_DIE_LOG);
			DeleteFile(gszUSER_DIRECTORY + WT_GET_IDX_LOG);
			DeleteFile(gszUSER_DIRECTORY + WT_GET_PZN_LOG);
			DeleteFile(gszUSER_DIRECTORY + "\\History\\WmInfo.log");
			REMOVE_BLOCK_ALIGN();	//v3.25T16
		}

		CString szMsg;
		if( IsAOIOnlyMachine()==FALSE || IsScanNgPick() )
		{
			CMSLogFileUtility::Instance()->WT_GetDieLogOpen();
			CMSLogFileUtility::Instance()->WT_GetIdxLogOpen();
			CMSLogFileUtility::Instance()->WT_PrLogOpen();	//PR statua log for bad-die status returned from WPR
		}
		CMSLogFileUtility::Instance()->WT_GetDieLog("");
		CMSLogFileUtility::Instance()->WT_GetDieLog("Start");
		szMsg.Format("PICK is %d; SKIP is %d; ALIGN is %d; CHECK is %d; INVALID is %d; INKED is %d; MISSING is %d; DEFECT is %d; SEPARATION is %d", 
			WAF_CDieSelectionAlgorithm::PICK,
			WAF_CDieSelectionAlgorithm::SKIP,
			WAF_CDieSelectionAlgorithm::ALIGN,
			WAF_CDieSelectionAlgorithm::CHECK,
			WAF_CDieSelectionAlgorithm::INVALID,
			WAF_CDieSelectionAlgorithm::INKED,
			WAF_CDieSelectionAlgorithm::MISSING,
			WAF_CDieSelectionAlgorithm::DEFECTIVE,
			WAF_CDieSelectionAlgorithm::SEPARATION);
		CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
		CMSLogFileUtility::Instance()->WT_GetDieLog(m_szMapFileName);

		CMSLogFileUtility::Instance()->WT_GetIdxLog("");
	//	CMSLogFileUtility::Instance()->WT_GetIdxLog("Start");
		LONG lMinT = labs((LONG)(*m_psmfSRam)["WaferPr"]["ThetaMiniumRotation"]);
		szMsg.Format("    WFT: Pitch X %ld(%ld), Y %ld(%ld) GT %d WTT delay %ld (minT %d) ", 
			GetDiePitchX_X(), GetDiePitchX_Y(), GetDiePitchY_Y(), GetDiePitchY_X(), GetGlobalT(), m_lTDelay, lMinT);
		SaveScanTimeEvent(szMsg + "map:" + m_szMapFileName);
		CMSLogFileUtility::Instance()->WT_GetIdxLog("Start" + szMsg + "map:" + m_szMapFileName);
		if( IsAdvRegionOffset() )
		{
			CMSLogFileUtility::Instance()->WT_GetAdvRgnOffsetLogOpen();
			CMSLogFileUtility::Instance()->WT_SetAdvRgnOffsetLogOpen();
		}

		//v3.41
		CMSLogFileUtility::Instance()->WT_GetPrLog("Start " + m_szMapFileName);
		CMSLogFileUtility::Instance()->WT_GetPrLog("DEFAULT=0,PICK=1,Defect=2,INK=3,Chip=4,BCut=5,Empty=6,Rotate=7\n");
	}

	CMSLogFileUtility::Instance()->WT_BackupPitchAlarmLog();

	if (m_bEnableMS100EjtXY && IsMS60())
	{
		//CMSLogFileUtility::Instance()->BPR_Arm1LogOpen();	//v4.42T15
		//CMSLogFileUtility::Instance()->BPR_Arm1Log("\n");	
		//CMSLogFileUtility::Instance()->BPR_Arm2LogOpen();	//v4.42T15
		//CMSLogFileUtility::Instance()->BPR_Arm2Log("\n");	
	}

	CMSLogFileUtility::Instance()->MS60_LogOpen();
	CMSLogFileUtility::Instance()->MS60_Log("\n");	

	if( IsAutoSampleSort() && m_bWaferPrUseMxNLFWnd )
	{
		//v4.46T13	//Semitek
		CString szMapFileName = m_szMapFileName;
		CMSLogFileUtility::Instance()->WT_GetCTLogOpen(szMapFileName);	//Andrew: PLEASE DO NOT REMOVE!!!!!!
	}

	//4.53D18 open file
	if(m_bWaferMapUseDieMixing)
	{
		AfxMessageBox("Open the CT log for Die Mixing", MB_SYSTEMMODAL);
		CString szMapFileName = m_szMapFileName;
		CMSLogFileUtility::Instance()->WT_GetCTLogOpen(szMapFileName);
	}

	//Wafermap Log
	CString szWmLogFile = gszUSER_DIRECTORY + "\\History\\WmInfo.log";
	if (m_WaferMapWrapper.GetReader() != NULL) 
	{
		//v4.06
		m_WaferMapWrapper.GetReader()->EnableDebug(CMSLogFileUtility::Instance()->GetEnableWtMachineLog(), szWmLogFile);
	}
}


VOID CWaferTable::StopOperation()
{
	if (m_fHardware && !m_bDisableWT && m_bStopAllMotion == FALSE)	//v3.61
	{
		if( IsES101()==FALSE )
		{
			X_Sync();
			Y_Sync();
			T_Sync();
		}
	}

	if( IsES101()==FALSE )
	{
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
		T_Profile(NORMAL_PROF);
	}

	State(IDLE_Q);
	Motion(FALSE);

	SignalInform(m_szStationName, gnSTATION_COMPLETE);

	m_WaferMapWrapper.StopMap();		// Stop the wafer map operation
	m_WaferMapWrapper.SetCurrentPosition((ULONG)m_stLastDie.lY, (ULONG)m_stLastDie.lX);

	m_bLoadMap = FALSE;

#ifdef NU_MOTION
	if (m_bEnableWTMotionLog)
	{
		if( IsESDualWT() )
		{
			LogAxisPerformance3(WT_AXIS_X2, WT_AXIS_Y2, WT_AXIS_T2, &m_stWTAxis_X2, &m_stWTAxis_Y2, &m_stWTAxis_T2, FALSE);
		}
		LogAxisPerformance3(WT_AXIS_X, WT_AXIS_Y, WT_AXIS_T, &m_stWTAxis_X, &m_stWTAxis_Y, &m_stWTAxis_T, FALSE);
	}
#endif

	m_qSubOperation = 0;

	//GetDie & Get Scn Log
	if ( CMSLogFileUtility::Instance()->GetEnableWtMachineLog() )
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog("End");
		CMSLogFileUtility::Instance()->WT_GetDieLogClose();

		CMSLogFileUtility::Instance()->WT_GetIdxLog("End");
		CMSLogFileUtility::Instance()->WT_GetIdxLogClose();
		CMSLogFileUtility::Instance()->WT_GetPrLog("End");
		CMSLogFileUtility::Instance()->WT_GetPrLogClose();
		if( IsAdvRegionOffset() )
		{
			CMSLogFileUtility::Instance()->WT_GetAdvRgnOffsetLogClose();
			CMSLogFileUtility::Instance()->WT_SetAdvRgnOffsetLogClose();
		}
	}

	if( CMSLogFileUtility::Instance()->GetEnableMachineLog() && DEB_IsEnable()  )
	{
		CMSLogFileUtility::Instance()->WT_CloseMylarShiftLog();
	}	//	close mylar shift log


	CMSLogFileUtility::Instance()->MS60_Log("END\n");	
	CMSLogFileUtility::Instance()->MS60_LogClose();

	//if (m_bEnableMS100EjtXY)
	//{
		//CMSLogFileUtility::Instance()->BPR_Arm1Log("END\n");	
		//CMSLogFileUtility::Instance()->BPR_Arm1LogClose();	//v4.42T15
		//CMSLogFileUtility::Instance()->BPR_Arm2Log("END\n");	
		//CMSLogFileUtility::Instance()->BPR_Arm2LogClose();	//v4.42T15
	//}

	if( IsAutoSampleSort() && m_bWaferPrUseMxNLFWnd )
	{
		CMSLogFileUtility::Instance()->WT_GetCTLogClose();	//Andrew: PLEASE DO NOT REMOVE!!!!!!
		BOOL bWaferEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferEnd"];
		if (bWaferEnd)
		{
			CMSLogFileUtility::Instance()->BackupCTLogAtWaferEnd(m_szMapFileName);
		}
	}

	//4.53D18 close file
	if(m_bWaferMapUseDieMixing)
	{
		CMSLogFileUtility::Instance()->WT_GetCTLogClose();	//Andrew: PLEASE DO NOT REMOVE!!!!!!
	}

	if (IsEjtElvtInUse() && IsScanNgPick() )
	{
		OpStopES101NGPIckCycle();
	}
}
