/////////////////////////////////////////////////////////////////
// MS_State.cpp : Operation State of the CMS896AApp class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, April 21, 2005
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2005.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "HostComm.h"
#include "MESConnector.h"
#include "SecsComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
// CMS896AApp Override of SFM_CModule class functions
/////////////////////////////////////////////////////////////////
VOID CMS896AApp::Operation()
{
/*
	static INT	nScanCount = 0;

	// Scan Input Variable at about 100 ms interval
	if ((m_qState != UN_INITIALIZE_Q)	&&
		(m_qState != DE_INITIAL_Q)		&&
		++nScanCount >= 10)
	{
		nScanCount = 0;
		ScanInput();
	}
*/
	switch (m_qState)
	{
		case UN_INITIALIZE_Q:
			UnInitialOperation();
			break;

		case IDLE_Q:
			IdleOperation();
			break;

		case DIAGNOSTICS_Q:
			DiagOperation();
			break;

		case SYSTEM_INITIAL_Q:
			SystemInitialOperation();
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

		case CYCLE_Q:
			CycleOperation();
			break;

		case MANUAL_Q:
			ManualOperation();
			break;

		case STOPPING_Q:
			StopOperation();
			break;

		case DE_INITIAL_Q:
			DeInitialOperation();
			break;
	}

	//CMESConnector::Instance()->UpdateMachineStatusToIdle(m_qState);
	
	if ( (m_qState == IDLE_Q) || (m_qState == AUTO_Q) || (m_qState == DEMO_Q) || (m_qState == STOPPING_Q) || (m_qState == ERROR_Q))
	{
		try
		{	
			RefreshMachineTime();
			CopyAlarmReportEveryHour();
			GetGmpErrorLogToDrive();
		}
		catch (CAsmException e)
		{
			DisplayException(e);	
		}
	}

	if ((m_qState == IDLE_Q) || (m_qState == UN_INITIALIZE_Q))
	{
		try
		{
			UpdateOutput();
			OnSelectPackageFile();
		}
		catch (CAsmException e)
		{
			DisplayException(e);	
		}
	}

	if( m_qState == IDLE_Q && m_bAWET_Enabled && m_bAWET_Triggered && m_bAWET_AutoSetScreen ) 
	{
		SFM_CStation *pStation;
		CAppStation  *pAppStation;
		POSITION	  pos;
		CString		  szKey;
		BOOL bAllIdle = TRUE;
		for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
		{
			m_pStationMap.GetNextAssoc(pos, szKey, pStation);
			pAppStation = dynamic_cast<CAppStation*>(pStation);
			if (pAppStation->State() != IDLE_Q)
			{
				bAllIdle = FALSE;
				break;
			}
		}
		if( bAllIdle )
		{
			SetAlarmLamp_Status(ALARM_GREEN_YELLOW_RED_ON, TRUE, FALSE);
			m_bAWET_AutoSetScreen = FALSE;	// done
			IPC_CServiceMessage stReq;
			CHAR acPar[200];
			strcpy_s(acPar, sizeof(acPar), "AWET_Recover");
			stReq.InitMessage(((INT)strlen(acPar) + 1), acPar);
			m_comClient.SendRequest("HmiUserService", "HmiSetScreen", stReq);
		}
	}

	Sleep(10);
}

VOID CMS896AApp::UpdateOutput()
{
	static int nCount = 0;

	m_szState = gszMODULE_STATE[m_qState];

	//if ((m_qState == UN_INITIALIZE_Q) ||
	//	(m_qState == DE_INITIAL_Q))
	//{
	//	return;
	//}

	// TODO: Add your variables update here
	if (nCount++ < 15)
	{
		return;
	}
	else
	{
		nCount = 0;
	}

	// Check Host Comm status
	if ( (m_bEnableTCPComm == TRUE) && (m_bHostCommEnabled == TRUE) )
	{
		CheckHostCommStatus();
	}
}

VOID CMS896AApp::CopyAlarmReportEveryHour()
{
	CMS896AApp *pApp				= (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "EverVision" && m_bEverVisionUser ==1)
	{
		CTime theTime					= CTime::GetCurrentTime();
		LONG lTime = theTime.GetHour();
		if (m_lTime == -1 || lTime == m_lTime)
		{
			CString szLogPath				= "c:\\MapSorter\\UserData\\History\\AlarmReport.csv";

			CString szPath3;//					= m_smfSRam["BinTable"]["Output File Path 3"];
			CString szPathFile = "c:\\mapsorter\\pathfile.txt";
			CString szTemp;
			CStdioFile cfCheck;
			if( _access(szPathFile,0) != -1)
			{
				if( cfCheck.Open(szPathFile,CFile::modeRead))
				{
					cfCheck.ReadString(szTemp);
					cfCheck.ReadString(szTemp);
					if (szTemp == "")
					{
						HmiMessage("Path 3 Empty");
					}
					else
					{
						szPath3 = szTemp;
						if (_access(szPath3,0) == -1)
						{
							HmiMessage("Could Not Access Alarm Log Path:" + szPath3 + ",Save Alarm Log Fails");
						}
					}
					cfCheck.Close();		
				}
			}
			else
			{
				HmiMessage("c:\\mapsorter\\pathfile.txt cannot be found!");
			}

			CString szFileNameTime			 = theTime.Format("%Y_%m_%d_%H_%M_%S");
			CString szMachineNo				 = pApp->GetMachineNo();
			CString szFullPath				 = szPath3 + "\\AlarmReport_" + szMachineNo + "_" + szFileNameTime + ".csv";
			IPC_CServiceMessage sv;
			char szMsg[500];
			CMSLogFileUtility::Instance()->MS_LogOperation("Alarm Report Log:" + szFullPath);
			//CString szPath = szPath3 + "\\WhateverFile.txt";
			time_t toTime = time(NULL);
			time_t fromTime = toTime - 3600;

			int nIndex = 0;
			memcpy(szMsg, (LPCTSTR)szFullPath, szFullPath.GetLength() + 1);
			nIndex += szFullPath.GetLength() + 1;

			memcpy(&szMsg[nIndex], &fromTime, sizeof(time_t));
			nIndex += sizeof(time_t);
			memcpy(&szMsg[nIndex], &toTime, sizeof(time_t));
			nIndex += sizeof(time_t);

			sv.InitMessage(nIndex, szMsg);
			m_comClient.SendRequest("HmiAlarmStation", "HmiSaveEventLog", sv);

			remove(szLogPath);
			if (lTime == 23)
				m_lTime = 0;
			else
				m_lTime = lTime + 1;
		}
	}
	return;
}

VOID CMS896AApp::RefreshMachineTime()
{
	static int nLastMin = 0;
	static int nLastSec = 0;
	BOOL bGenReport = FALSE;	//v2.83T2

	//Only refresh when machine is started
	if ( m_bMachineStarted == FALSE )
	{
		return;
	}


	CTime theTime = CTime::GetCurrentTime();

	UINT unEnable = GetProfileInt(gszPROFILE_SETTING, _T("Equip Machine Time 2"), 0);
	if ( theTime.GetMinute() == nLastMin )
	{
		if ( theTime.GetSecond() == nLastSec )
		{
			return;
		}

		bGenReport = m_eqMachine.Refresh (FALSE);
		if (unEnable==1)		//WH Sanan	//v4.40T4
		{
			bGenReport = m_eqMachine2.Refresh(FALSE);
		}
	}
	else
	{
		//Only update the Raw data file on every minute
		bGenReport = m_eqMachine.Refresh (TRUE);
		if (unEnable)		//WH Sanan	//v4.40T4
		{
			bGenReport = m_eqMachine2.Refresh(TRUE);
		}
	}

	//v2.83T2
	if (bGenReport && m_bEqTimeAutoResetDieCounts)
	{
		BOOL bPrompt = FALSE;
		IPC_CServiceMessage svMsg;
		svMsg.InitMessage(sizeof(BOOL), &bPrompt);
		INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_ResetAllDieCount", svMsg);
	}

	//v4.39T12
	if (bGenReport && CMS896AStn::m_bIsSecsGemInit)
	{
//		CMS896AStn::m_ulEMLastOutput		= 0;
//		CMS896AStn::m_ulEMLastProdTime		= 0;
//		CMS896AStn::m_ulEMLastStandbyTime	= 0;
//		CMS896AStn::m_ulEMLastDownTime		= 0;
//		CMS896AStn::m_ulEMLastNumOfAssists	= 0;
//		CMS896AStn::m_ulEMLastNumOfFailures	= 0;
//		CMS896AStn::m_ulEMLastSetupTime		= 0;
	}

	nLastMin = theTime.GetMinute();
	nLastSec = theTime.GetSecond();

	m_szMachineUpTime		= m_eqMachine.GetTimeFormat(EQUIP_UP_TIME);
	m_szMachineIdleTime		= m_eqMachine.GetTimeFormat(EQUIP_IDLE_TIME);
	m_szMachineSetupTime	= m_eqMachine.GetTimeFormat(EQUIP_SETUP_TIME);

	if( m_bAWET_Enabled ) 
	{
		m_szMachineWaitingSetupTime	= m_eqMachine.GetTimeFormat(EQUIP_WAITING_SETUP_TIME);	//	get time total
	}
	m_szMachineRunTime		= m_eqMachine.GetTimeFormat(EQUIP_RUN_TIME);
	m_szMachineAlarmTime	= m_eqMachine.GetTimeFormat(EQUIP_ALARM_TIME);
	m_szMachineAssistTime	= m_eqMachine.GetTimeFormat(EQUIP_ASSIST_TIME);
	m_szMachineDownTime		= m_eqMachine.GetTimeFormat(EQUIP_DOWN_TIME);

	m_szMachineAlarmCount.Format("%d", m_eqMachine.GetNoOfAlarm());
	m_szMachineAssistCount.Format("%d", m_eqMachine.GetNoOfAssistance());

	m_szMachineBondedUnit.Format("%d", m_eqMachine.GetUnit());
}

VOID CMS896AApp::ScanInput()
{
	if ((m_qState == UN_INITIALIZE_Q) ||
		(m_qState == DE_INITIAL_Q))
	{
		return;
	}

	// TODO: Add your own variables scanning here
}

INT CMS896AApp::State()
{
	return m_qState;
}

VOID CMS896AApp::OnSelectPackageFile()
{
	static CString szPrevDeviceFile = "";
	
	if (m_szDeviceFile != szPrevDeviceFile && m_szDeviceFile != "")
	{
		CString szPath;

		if (m_ucDeviceFileType == MS_PACKAGE_FILE_TYPE)
		{
			szPath = m_szDevicePath;
		}
		else
		{
			szPath = m_szPortablePKGPath;
		}

		ChangePkgFilePreviewDiagram(szPath ,m_szDeviceFile);
		szPrevDeviceFile = m_szDeviceFile;
	}
}


/////////////////////////////////////////////////////////////////
//	State Operation Functions
/////////////////////////////////////////////////////////////////
VOID CMS896AApp::UnInitialOperation()
{
	if (m_bRunState)
	{
		if (InitHardware())
		{
			m_qState		= IDLE_Q;
			m_fDeleteMarkIDE= TRUE;
			DisplayMessage (_T("Hardware Initialized - System in IDLE state"));
#ifdef NU_MOTION	//v4.46T26
			CMSLogFileUtility::Instance()->MS_LogOperation("MS100 INIT Done\n");
#else
			CMSLogFileUtility::Instance()->MS_LogOperation("MS899 INIT Done\n");	
#endif
		}
		else
		{
			DisplayMessage( _T("Hardware fail to Initialize!!!"));
#ifdef NU_MOTION
			CMSLogFileUtility::Instance()->MS_LogOperation("MS100 INIT Fail\n");
#else
			CMSLogFileUtility::Instance()->MS_LogOperation("MS899 INIT Fail\n");	
#endif
		}
		m_bRunState = FALSE;
	}
}

VOID CMS896AApp::IdleOperation()
{
	static ULONG m_ulCount = 0;

	if (m_fDeleteMarkIDE)
	{
		//Purpose: To Delete all Data Created by MarkIDE
		INT	nCount = 0;
		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			CString	szName;
			SFM_CStation* pStation;
			m_pStationMap.GetNextAssoc(pos, szName, pStation);

			if (!pStation->IsInitialized())
				return;

			nCount++;
		}

		if (nCount == m_pStationMap.GetCount())
		{
			m_fDeleteMarkIDE	= FALSE;
			CMS896AStn::m_bAppInitialized	= TRUE;
			//m_pInitOperation->DeleteDataView();
			DisplayMessage (_T("Stations Initialized - Stations in IDLE state"));
		}
	}

	if ( m_bHmiExecuted == FALSE )
	{
		CString szFailStnName = "";
		if ( CheckAllHardwareReady(szFailStnName) == TRUE )
		{
			ExecuteHmi();
			m_bHmiExecuted = TRUE;
			m_ulCount = 0;
		}
		else
		{
			m_ulCount++;
			if ( m_ulCount > 1000 )
			{
				CString szLog;
				szLog.Format("Fail to init hardware (STN: %s). Please check!", szFailStnName);
				AfxMessageBox(szLog, MB_SYSTEMMODAL);
				m_bHmiExecuted = TRUE;
				m_ulCount = 0;
			}
			else
				Sleep(10);
		}
	}

	//Reconnect TCP/IP Connection
	if ( m_bEnableTCPComm == TRUE )
	{
		RetryHostComm();
	}

	Sleep(10);
}

VOID CMS896AApp::DiagOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Diagnostic operation completed"));
		m_qState = IDLE_Q;
	}
}

VOID CMS896AApp::SystemInitialOperation()
{
	// Signal all stations to perform prestart operation
	
	SFM_CStation* pStation;
	CAppStation*  pAppStation;
	POSITION	  pos;
	CString		  szKey;
	BOOL		  bError = FALSE;

	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Initialization completed"));

		//check for errror during Initialization
		for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
		{
			m_pStationMap.GetNextAssoc(pos, szKey, pStation);
			pAppStation = dynamic_cast<CAppStation*> (pStation);

			if (pAppStation->Result() == gnNOTOK)
			{
				bError = TRUE;
				break;
			}
		}

		if ((m_nOperation != NULL_OP) && (bError != TRUE))
		{
			// Signal all stations to perform prestart operation
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*> (pStation)) != NULL)
				{
					pAppStation->Motion();
					pAppStation->Command(glPRESTART_COMMAND);
					pAppStation->Result(gnOK);
				}
			}

			DisplayMessage(_T("Perform PreStart Command"));
			m_qState = PRESTART_Q;
		}
		else
		{
			m_qState	 = IDLE_Q;
			m_nOperation = NULL_OP;
			m_lAction	 = glNULL_COMMAND;
		}
	}
}

VOID CMS896AApp::PreStartOperation()
{
	//If PreStart Operation Completed, Signal All station to perform: Auto ot Demo
	//Operation (depending on the Command)

	SFM_CStation*	pStation;
	CAppStation*	pAppStation;
	POSITION		pos;
	CString			szKey;


	m_bIsChangeWafer = FALSE;

	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("PreStart operation completed"));
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS: PreStartOperation start");	//v3.93

		if ((m_nOperation == AUTO_OP) || (m_nOperation == DEMO_OP))
		{
			m_bCycleStarted = TRUE;
			m_bCycleNotStarted = FALSE;
			m_bCycleFKey = FALSE;
			m_bShortCutKey = FALSE;
			m_smfSRam["MS899"]["CriticalError"] = FALSE;

			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*> (pStation)) != NULL)
				{
					pAppStation->Motion();
					pAppStation->SetAxisAction(0, m_lAction, 0);
					pAppStation->Result(gnOK);

					switch (m_nOperation)
					{
					case AUTO_OP:
						pAppStation->Command(glAUTO_COMMAND);
						break;

					case DEMO_OP:
						pAppStation->Command(glDEMO_COMMAND);
						break;
					}
				}
			}

			switch (m_nOperation)
			{
			case AUTO_OP:
				DisplayMessage(_T("Perform Auto Operation"));
				m_qState = AUTO_Q;
				break;

			case DEMO_OP:
				DisplayMessage(_T("Perform Demo Operation"));
				m_qState = DEMO_Q;
				break;

			default:
				DisplayMessage(_T("Invalid Operation Encountered!"));
				m_qState = IDLE_Q;
				break;
			}

			//Log machine time
			//m_eqMachine.SetTime(TRUE, EQUIP_RUN_TIME);
			CMSLogFileUtility::Instance()->LogCycleStartStatus("MS: PreStartOperation done\n");	//v3.93

			/* Temp to disable since the logging file size is very large
			//Cycle Log
			if ( m_bEnableMachineLog == TRUE )
			{
				CString szLogFile;

				szLogFile = gszUSER_DIRECTORY + "\\History\\Cycle.log";
				if ( (m_pCycleLog = fopen(szLogFile, "a+")) != NULL )
				{
					CTime theTime = CTime::GetCurrentTime();
					fprintf(m_pCycleLog, "%d-%2d-%2d (%2d:%2d:%2d) - Start\n", 
										theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond()); 
				}
			}
			*/
		}
		else
		{
			m_nOperation	= NULL_OP;
			m_lAction		= glNULL_COMMAND;
			m_qState		= IDLE_Q;
			DisplayMessage(_T("System Initialization completed"));
		}
	}
}

VOID CMS896AApp::AutoOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Auto Operation completed"));

		m_qState	 = IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	 = glNULL_COMMAND;
	}
}

VOID CMS896AApp::DemoOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Demonstration Operation completed"));
		//SetStatusMessage("Demo is running...");

		m_qState	 = IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	 = glNULL_COMMAND;
	}
}

VOID CMS896AApp::CycleOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Cycle Test completed"));

		m_qState	 = IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	 = glNULL_COMMAND;
	}
}

VOID CMS896AApp::ManualOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		m_qState	 = IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	 = glNULL_COMMAND;

		DisplayMessage(_T("Manual operation completed"));
	}
}

VOID CMS896AApp::StopOperation()
{
	BOOL bStopMachine = TRUE;

	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		LogCycleStopStatus("APP: into stop operation");
/*
		if (m_lAction == glSYSTEM_EXIT)
		{
			((CFrameWnd*) m_pMainWnd)->PostMessage(WM_CLOSE);
		}
*/
		if (m_fStopMsg)
		{
			try
			{
				DisplayMessage(_T("Operation Stopped!"));
				TRACE0("\n****** Stop Operation ******\n");
			}
			catch (CAsmException e)
			{
				DisplayException(e);
			}
		}

		m_qState	 = IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	 = glNULL_COMMAND;

		if (m_fStopMsg)
		{
			LogCycleStopStatus("APP: stop update NVRAM.");
			BackupToNVRAM();

			IPC_CServiceMessage svMsg;

			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE, TRUE);	//anichia002
			LONG lStatus = ChangeWaferFrame(m_bBurnIn, m_bCycleStarted);
			if ( m_bBurnIn == TRUE )
			{
				if ( (lStatus == -1) || (lStatus == 1) )
				{
					LogCycleStopStatus("APP: burn in change frame into demo command");
					if( m_bES100v2DualWftOption==FALSE )
						BurnInPreStartOperation();
					SrvDemoCommand(svMsg);	// burn in re-start
				}
				else
				{
					LogCycleStopStatus("APP: burn in change frame into stop");
					AutoStopBinLoader(TRUE);	//v4.42T4
					SrvStopBurnInCommand(svMsg);
				}
				bStopMachine = FALSE;
			}
			else
			{	
				if ( (lStatus == 1) )	//&& (bResult == TRUE) )
				{
					m_bPreStartCycleOK = TRUE;
					CMSLogFileUtility::Instance()->MS_LogOperation("change wafer frame in stop Operation");
					LogCycleStopStatus("APP: change frame into demo command");
					SrvDemoCommand(svMsg);	// auto sort re-start
					bStopMachine = FALSE;
				}
			}
			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);			//anichia002

			// Not change wafer frame case
			if (bStopMachine == TRUE)
			{
				RestoreBurnInStatus();
				if (m_pSecsComm)
				{
					m_pSecsComm->SetStopOperationStatus();
				}

				if (CMS896AStn::m_bEnableMachineReport == TRUE)
				{
					LogCycleStopStatus("APP: generate parameter list");
					GenerateParametersRecord();
					LogCycleStopStatus("APP: generate parameter list done");
				}

				if (m_eqMachine.IsAutoDeleteEMExpiredEquipStateTimeRecord())
				{
					LogCycleStopStatus("APP: Auto Delete Expired Equipment Statistics Data...");
					//Auto Delete the EM expired log equipment statistics
					m_eqMachine.AutoDeleteExpiredEquipStateTimeRecord();
					LogCycleStopStatus("APP: Auto Delete Expired Equipment Statistics Data Done");
				}

				//Log machine time
				m_eqMachine.SetTime (FALSE, EQUIP_RUN_TIME, "", "Machine Stop");
				m_eqMachine2.SetTime(FALSE, EQUIP_RUN_TIME, "", "Machine Stop");	//WH Sanan	//v4.40T4
				SetAlarmLamp_Yellow(FALSE, TRUE);
			}
			
			LogCycleStopStatus("After stop Operation");
			AfterStopOperation();
			LogCycleStopStatus("Cycle is stopped");
		}

		m_bDisableStopButton	= FALSE;	//v4.13T1
		m_bCycleStarted			= FALSE;
		m_bCycleNotStarted		= TRUE;
		m_bCycleFKey			= TRUE;
		m_bShortCutKey			= TRUE;
		LogCycleStopStatus("MC in idle state");		//v4.42T15

		CString szMsg;
		CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

		if (m_fStopMsg == TRUE && m_bCoverSensorProtectionMessage == TRUE && bStopMachine == TRUE)
		{
			CString szStr;
			szStr.LoadString(HMB_MS_MACHINE_STOP);
			HmiMessage(szStr);
		}
	}	
}

VOID CMS896AApp::DeInitialOperation()
{
	SFM_CStation* pStation;
	CAppStation*  pAppStation;
	POSITION	  pos;
	CString		  szKey;
	CString		  szName;
	INT			  nCount = 0;

	for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
	{
		m_pStationMap.GetNextAssoc(pos, szKey, pStation);
		pAppStation = dynamic_cast<CAppStation*> (pStation);

		if (pAppStation->IsReInitialized())
		{
			nCount++;
		}
		else
		{
			break;
		}
	}

	if (nCount == m_pStationMap.GetCount())
	{
		//m_pInitOperation->DeleteDataView();
		//m_pInitOperation->EnableHrdwException();

		m_qState = IDLE_Q;
		DisplayMessage("De-Initialize/Re-Initialize Operation Completed, Systems in IDLE state");
	}
}


// Execute for cycle is stopped.
VOID CMS896AApp::AfterStopOperation()
{
	BOOL bInput = FALSE;
	//BOOL bIsPickAndPlaceOn = FALSE;
	LONG lPickAndPlaceMode = MS_PICK_N_PLACE_DISABLED;
	int nConvID;
	IPC_CServiceMessage rReqMsg;

	//Check use Pick & Place or not
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "IsPickAndPlaceOn", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
			rReqMsg.GetMsg(sizeof(LONG), &lPickAndPlaceMode);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (lPickAndPlaceMode != MS_PICK_N_PLACE_DISABLED)
	{
		//Set Pick & Place mode
		bInput = FALSE;
		rReqMsg.InitMessage(sizeof(BOOL), &bInput);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "StartPickAndPlace", rReqMsg);
		m_comClient.ScanReplyForConvID(nConvID,5000);
		m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
	}	
}

// PreStart Action burn in cycle.
LONG CMS896AApp::BurnInPreStartOperation()
{
	IPC_CServiceMessage svMsg;
	int nConvID;
	BOOL bSet=TRUE;


	// ClearBin
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "BurnInClearCounter", svMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,svMsg);
			svMsg.GetMsg(sizeof(BOOL), &bSet);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	// Load map
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "LoadMapFileForBurnIn", svMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,svMsg);
			svMsg.GetMsg(sizeof(BOOL), &bSet);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	// Set map start point
	while(1)
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetMapStartPointForBurnIn", svMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,svMsg);
				svMsg.GetMsg(sizeof(BOOL), &bSet);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if ( bSet == TRUE )
		{
			break;	
		}
		Sleep(100);
	}

	return 1;
}
