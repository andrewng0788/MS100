/////////////////////////////////////////////////////////////////
// SecsComm.cpp : interface of the CSecsComm class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, June 04, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "SecsComm.h"
#include "WaferLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CSecsComm::Operation()
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

    case AUTO_Q: // auto bonding
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

	if (State() == IDLE_Q || State() == UN_INITIALIZE_Q)	//v4.59A11
	{
		try
		{
			UpdateOutput();

			if (m_bUpdateProfile)
			{
				UpdateProfile();
			}

			if (m_bUpdatePosition)
			{
				UpdatePosition();
			}

			if (m_bS2F41RemoteStartCmd)		//v4.59A11	//AUTOLINE S2F41 START cmd
			{
				StartS2F41RemoteStartCmd();
			}


			EM_GenerateCyclicReport();		//v4.37T5	//EquipManager
		}
		catch (CAsmException e)
		{
			DisplayException(e);	
		}

		Sleep(10);
	}

	Sleep(10);
}

/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
VOID CSecsComm::UpdateOutput()
{
	if (!m_fHardware)
		return;

	if( (m_qState==UN_INITIALIZE_Q) || (m_qState==DE_INITIAL_Q) )
		return;

	//Update sensor 
	try
	{
		if (!theApp.m_bAppClosing)
		{
			QueryCommState();
			QueryEquipmentState();
			QueryProcessState();
			UpdateControlState(FALSE);
			//CheckControlStatus();	//OsramTrip 8/22
		}
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}



VOID CSecsComm::UpdateProfile()
{
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachine);

	m_bUpdateProfile = FALSE;

	slLock.Unlock();
}

VOID CSecsComm::UpdatePosition()
{
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CSingleLock slLock(&pAppMod->m_csDevice);
	
	m_bUpdatePosition = FALSE;

	slLock.Unlock();
}

VOID CSecsComm::AutoOperation()
{
	RunOperation();
}

VOID CSecsComm::DemoOperation()
{
	RunOperation();
}

INT CSecsComm::OpPreStart()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	INT nResult	= gnOK;
	CString szMess;

	try 
	{
		if (IsSecsGemInit())
		{
			DisplaySequence("SG - Operation Prestart Start");
			//v4.58A7	//MS60 AutoLine
			CString szFrameBarcode		= (*m_psmfSRam)["WaferTable"]["WT1InBarcode"];
			CString szOutputFilePath	= (*m_psmfSRam)["MS896A"]["OutputFilePath"];
	        
			if (pApp->m_bMSAutoLineMode == 1)
			{
				//Song if no barcode, get bar code from wafer map file
				if (szFrameBarcode.IsEmpty())
				{
					CString szFileName;
					szFileName =	GetMapFileName();
					CString szTemp1;
					int nIndex;
					nIndex = szFileName.ReverseFind('\\');
					szTemp1 = szFileName.Mid(nIndex + 1);
					nIndex = szTemp1.Find('.');
					szFrameBarcode = szTemp1.Left(nIndex);
				}	
				SetGemValue(MS_SECS_SV_WAFER_BARCODE,	szFrameBarcode);	//v4.59A11
			}
			else
			{
				SetGemValue(MS_SECS_SV_WAFER_ID,		szFrameBarcode);
			}

			SetGemValue(MS_SECS_SV_FRAME_ID,				GetMapFileName());
			SetGemValue(MS_SECS_SV_WAFER_BIN_MAP_FILE_NAME,	"");
			SetGemValue(MS_SECS_SV_WAFER_BIN_MAP_FILE_PATH,	szOutputFilePath);

			if (IsMS50())
			{
				SetGemValue(MS_SECS_SV_TOOL_TYPE,	"MS50");
			}
			else if (IsMS90())
			{
				SetGemValue(MS_SECS_SV_TOOL_TYPE,	"MS90");
			}
			else if (IsMS60())
			{
				SetGemValue(MS_SECS_SV_TOOL_TYPE,	"MS60");
			}
			else
			{
				SetGemValue(MS_SECS_SV_TOOL_TYPE,	"MS899");
			}

			SetGemValue(MS_SECS_SV_TOOL_ID,	m_szEquipId);

			SendEvent(SG_CEID_AB_START, FALSE);			//send CEID #7000
/*
			m_lEquipmentStatus = glSG_PRODUCTIVE;
			SetEquipState(m_lEquipmentStatus);			//send CEID #8 
			SetProcessState(PROCESS_EXE_LOCAL);
*/
			SendLexter_S1F65(2);										//Send S1F65 RUNNING

			//Set SECSGEM Value
			//SetCtmLogMessage("SG_CEID_StartBond");
			LONG lSlotNo = 1;
			SetGemValue(MS_SECS_SV_WAFER_ID, m_szWaferId);							//3003

			CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
			if (pWaferLoader != NULL)
			{
				lSlotNo = pWaferLoader->WL_GetCassCurSlot();
			}

			SetGemValue(MS_SECS_SV_SLOT_NO, lSlotNo);							//SV#3004
			SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");						//SV#3005
			ULONG ulTotalDie = m_WaferMapWrapper.GetSelectedTotalDice();
			SetGemValue(MS_SECS_SV_TOTAL_BIN_TABLE_DIE_ON_WAFER, ulTotalDie);	//SV#3040

			SendEvent(SG_CEID_StartBond, TRUE);							//Send CEID #8016
		}
	}
	catch (...) //CAsmException e)
	{
		DisplaySequence("CSecsComm::OpPreStart() failure");
	}

	DisplaySequence("SG - Operation Prestart");
	return nResult;
}


VOID CSecsComm::PreStartOperation()
{
	m_nLastError	= gnOK;

	try
	{
		m_nLastError = OpPreStart();

		if (m_nLastError != gnOK)
		{
			if (!IsAbort())
			{
				SetAlert(m_nLastError);
			}

			Result(gnNOTOK);
		}

		DisplaySequence("SG - PreStartOperation Completed");
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


VOID CSecsComm::RunOperation()
{
    if( (Command()==glSTOP_COMMAND) && (Action()==glABORT_STOP) )
	{
        //DisplayMessage("stopped cycle by press button, send root class");
		DisplaySequence("SG - STOP");
//        m_lEquipmentStatus = glSG_SCHEDULE_DOWN;
		State(STOPPING_Q);
	}
	else		//	else if (!m_bJogMode || m_bStep)
	{
		try
		{
			if (m_bStop)
			{
				DisplaySequence("SG - House Keeping Q");
				if (GetHouseKeepingToken(SECS_COMM_STN) == TRUE)
				{
					SetHouseKeepingToken(SECS_COMM_STN, FALSE);

					StopCycle(SECS_COMM_STN);
					State(STOPPING_Q);

					SetHouseKeepingToken(SECS_COMM_STN, TRUE);
				}
			}
		}
		catch(CAsmException e)
		{
			DisplayException(e);
			Result(gnNOTOK);
			State(ERROR_Q);
			NeedReset(TRUE);
			StopCycle(SECS_COMM_STN);
			SetMotionCE(TRUE, "SECS GEM RunOperation Stop");
		}

		Sleep(10);
	}
}

VOID CSecsComm::StopOperation()
{
	Sleep(100);
    if (IsSecsGemInit())
    {
        SendEvent(SG_CEID_AB_STOP, FALSE);

		SendLexter_S1F65(1);									//Send S1F65 IDLE

		//Set SECSGEM Value
		//SetCtmLogMessage("SG_CEID_StopBond");
		LONG lSlotNo = 1;
		CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
		if (pWaferLoader != NULL)
		{
			lSlotNo = pWaferLoader->WL_GetCassCurSlot();
		}

		SetGemValue(MS_SECS_SV_SLOT_NO, lSlotNo);							//SV#3004
		SetGemValue(MS_SECS_SV_WAFER_ID, m_szWaferId);						//SV#3003		
		SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");						//SV#3005
		ULONG ulTotalDie = m_WaferMapWrapper.GetSelectedTotalDice();
		SetGemValue(MS_SECS_SV_TOTAL_BIN_TABLE_DIE_ON_WAFER, ulTotalDie);	//SV#3040

		SendEvent(SG_CEID_StopBond, TRUE);						//Send CEID #8019
    }

	Command(glAMS_NULL_COMMAND);
	State(IDLE_Q);
	Motion(FALSE);
	SignalInform(m_szStationName, gnSTATION_COMPLETE);
}


VOID CSecsComm::SetStartOperationStatus()
{
    if (IsSecsGemInit())
    {
		m_lEquipmentStatus = glSG_PRODUCTIVE;
		SetEquipState(m_lEquipmentStatus);			//send CEID #8 
		SetProcessState(PROCESS_EXE_LOCAL);
	}
}

VOID CSecsComm::SetStopOperationStatus()
{
    if (IsSecsGemInit())
    {
		//Notice the SIS to stop bonding, MS_SECS_SV_START_STOP_PRESSED : 1 -- Start bonding, 0 -- Stop Bonding
		SetGemValue(MS_SECS_SV_START_STOP_PRESSED, 0);	//5031
		SendEvent(SG_CEID_START_STOP, FALSE);

		if (IsMSAutoLineStandloneMode() || IsMSAutoLineMode())
		{
			m_lEquipmentStatus = glSG_SCHEDULE_DOWN;
		}
		else
		{
			m_lEquipmentStatus = glSG_STANDBY;
		}
		
		//m_bStopByCommand = FALSE;
	    SetEquipState(m_lEquipmentStatus);		//send CEID #8 
		SetProcessState(PROCESS_IDLE_LOCAL);
	}
}


VOID CSecsComm::QueryCommState()
{
    if (IsSecsGemInit()==FALSE)
	{
		m_szCommState = "SECS_DISABLE";
        return ;
	}

	switch (m_pGemStation->CommState())
	{
	case SECS_ENABLE:				// 0
		m_szCommState = "SECS_ENABLE";
		break;
	case SECS_DISABLE:				// 1
		m_szCommState = "SECS_DISABLE";
		break;
	case SECS_NOT_COMMUNICATING:	// 2
		m_szCommState = "SECS_NOT_COMMUNICATING";
		break;
	case SECS_COMMUNICATING:		// 3
		m_szCommState = "SECS_COMMUNICATING";
		if (m_bTriggerCommStateAlarm)
			m_bTriggerCommStateAlarm = FALSE;		//v4.42T16
		break;
	case SECS_WAIT_DELAY:			// 4
		m_szCommState = "SECS_WAIT_DELAY";
		break;
	case SECS_WAIT_CRA:				// 5
		m_szCommState = "SECS_WAIT_CRA";
		break;
	case SECS_WAIT_CR:				// 6
		m_szCommState = "SECS_WAIT_CR";
		break;
    default:
        m_szCommState = "SECS_DISABLE";
        break;
	}

	if (m_pGemStation->SecsStatus() == 0)
	{
		m_bIsSecsCommEnabled = TRUE;
	}
	else
	{
		m_bIsSecsCommEnabled = FALSE;
	}
}


BOOL CSecsComm::CheckCommState()	//v4.42T16
{
    if( IsSecsGemInit()==FALSE )
        return TRUE;
	if (m_bTriggerCommStateAlarm)
		return TRUE;

	if (m_bIsSecsCommEnabled == TRUE)
	{
		if (m_pGemStation->CommState() == SECS_NOT_COMMUNICATING)
		{
			CString szErr;
			szErr = "ERROR: SECS communication link is broken; please re-connect to continue.";
			SetErrorMessage(szErr);
			HmiMessage_Red_Yellow(szErr);
			m_bTriggerCommStateAlarm = TRUE;
			return FALSE;
		}
	}

	return TRUE;
}


VOID CSecsComm::QueryEquipmentState()
{
	switch (m_lEquipmentStatus)
	{
		case glSG_STANDBY:
			m_szEquipState = "STANDBY";
			break;
		case glSG_PRODUCTIVE:
			m_szEquipState = "PRODUCTIVE";
			break;
		case glSG_ENGINEERING:
			m_szEquipState = "ENGINEERING";
			break;
		case glSG_SCHEDULE_DOWN:
			m_szEquipState = "SCHEDULE DOWN";
			break;
		case glSG_UNSCHEDULE_DOWN:
			m_szEquipState = "UNSCHEDULE_DOWN";
			break;
		default:
			m_szEquipState = "UNKNOWN STATE";
			break;
	}
}


USHORT CSecsComm::GetProcessState()
{
	USHORT hProcessState = PROCESS_IDLE_LOCAL;
	GetGemValue(CString("ProcessState"), hProcessState);
	return hProcessState;
}

VOID CSecsComm::QueryProcessState()
{
	USHORT hProcessState = PROCESS_IDLE_LOCAL;

	if (!IsSecsGemInit())
	{
		m_szProcessState = "UNKNOWN STATE";
		return;
	}
	hProcessState = GetProcessState();
	AssignHMIProcessState(hProcessState);
}


VOID CSecsComm::AssignHMIProcessState(USHORT hProcessState)
{
	switch (hProcessState)
	{
	case PROCESS_IDLE_LOCAL:
		m_szProcessState = "Idle Local";
		break;
	case PROCESS_IDLE_REMOTE:
		m_szProcessState = "Idle Remote";
		break;
	case PROCESS_SETUP:
		m_szProcessState = "Setup";
		break;
	case PROCESS_INITIALIZING:
		m_szProcessState = "Initializing";
		break;
	case PROCESS_EXE_LOCAL:
		m_szProcessState = "Execute Local";
		break;
	case PROCESS_EXE_REMOTE:
		m_szProcessState = "Execute Remote";
		break;
	case PROCESS_ERROR:
		m_szProcessState = "Error";
		break;
	default:
		m_szProcessState = "UNKNOWN STATE";
		break;
	}
}


// On_Off Line State
//const	INT		SECS_ON_LINE					= 0;
//const	INT		SECS_OFF_LINE					= 1;

//const	INT		SECS_EQUIP_OFF_LINE				= 1;
//const	INT		SECS_ATTEMPT_ON_LINE			= 2;
//const	INT		SECS_HOST_OFF_LINE				= 3;
//const	INT		SECS_ON_LINE_LOCAL				= 4;
//const	INT		SECS_ON_LINE_REMOTE				= 5;
VOID CSecsComm::UpdateControlState(BOOL bInit)
{
	USHORT hControlSubState;

	if (!IsSecsGemInit())
	{
		m_szSecCtrlStatus = "EQUIP_OFF_LINE";
		return;
	}

	if (bInit)
	{
		if (m_bSecsOnline == SECS_ON_LINE)
		{
			m_pGemStation->UpdateControlState(SECS_ON_LINE, (m_hSecsCtrlMode == CTRL_MODE_REMOTE) ? SECS_ON_LINE_REMOTE : SECS_ON_LINE_LOCAL, bInit);
		}
		else
		{
			m_pGemStation->UpdateControlState(SECS_OFF_LINE, SECS_EQUIP_OFF_LINE, bInit);
		}
	}

	//Control Sub State
	hControlSubState = m_pGemStation->ControlSubState();

	if (hControlSubState == SECS_ON_LINE_REMOTE)
	{
		if (m_pGemStation->CommState() != SECS_COMMUNICATING)
		{
			hControlSubState = SECS_EQUIP_OFF_LINE;
			m_szSecCtrlStatus = "SECS_EQUIP_OFF_LINE";
		}
		else
		{
			m_szSecCtrlStatus = "SECS_ON_LINE_REMOTE";
		}
	}
	else if (hControlSubState == SECS_ON_LINE_LOCAL)
	{
		m_szSecCtrlStatus = "SECS_ON_LINE_LOCAL";
	}
	else if (hControlSubState == SECS_HOST_OFF_LINE)
	{
		m_szSecCtrlStatus = "SECS_HOST_OFF_LINE";
	}
	else if (hControlSubState == SECS_ATTEMPT_ON_LINE)
	{
		m_szSecCtrlStatus = "SECS_ATTEMPT_ON_LINE";
	}
	else if (hControlSubState == SECS_EQUIP_OFF_LINE)
	{
		m_szSecCtrlStatus = "SECS_EQUIP_OFF_LINE";
	}
	else
	{
		hControlSubState = SECS_EQUIP_OFF_LINE;
		m_szSecCtrlStatus = "SECS_EQUIP_OFF_LINE";
	}

	if ((m_bSecsOnline == SECS_OFF_LINE)/* && (hControlSubState != SECS_HOST_OFF_LINE)*/) 
	{
		m_pGemStation->UpdateControlState(SECS_OFF_LINE, SECS_EQUIP_OFF_LINE, FALSE);
	}
	else if (m_bSecsOnline == SECS_ON_LINE)
	{
		if (hControlSubState == SECS_EQUIP_OFF_LINE) //Offline->Local
		{
			Sleep(2);
			m_pGemStation->UpdateControlState(SECS_OFF_LINE, SECS_ATTEMPT_ON_LINE, FALSE);
		}
		else if ((hControlSubState == SECS_ON_LINE_LOCAL) && (m_hSecsCtrlMode == CTRL_MODE_REMOTE)) //Local->Remote
		{
			m_pGemStation->UpdateControlState(SECS_ON_LINE, SECS_ON_LINE_REMOTE, FALSE);
		}
		else if ((hControlSubState == SECS_ON_LINE_REMOTE) && (m_hSecsCtrlMode == CTRL_MODE_LOCAL)) //Remote->Local
		{
			m_pGemStation->UpdateControlState(SECS_ON_LINE, SECS_ON_LINE_LOCAL, FALSE);
		}
	}
	return;
}

/*
BOOL CSecsComm::CheckControlStatus()
{
    if (IsSecsGemInit() == FALSE)
	{
        return FALSE;
	}

	INT nCtrlState			= m_pGemStation->ControlState();
	INT nControlSubState	= m_pGemStation->ControlSubState();

    switch (nControlSubState)
    {
    case SECS_ON_LINE_REMOTE:
        m_szSecCtrlStatus = "SECS-ON-LINE-REMOTE";
        break;

    case SECS_ON_LINE_LOCAL:
        m_szSecCtrlStatus = "SECS-ON-LINE-LOCAL";
        break;

    case SECS_HOST_OFF_LINE:
        m_szSecCtrlStatus = "SECS-HOST-OFF-LINE";
        break;

	case SECS_ATTEMPT_ON_LINE:
		m_szSecCtrlStatus = "SECS-ATTEMPT_ON_LINE";
		break;

    case SECS_EQUIP_OFF_LINE:
        m_szSecCtrlStatus = "SECS-EQUIP-OFF-LINE";
        break;
	
	default:
        m_szSecCtrlStatus = "SECS-EQUIP-OFF-LINE";
        break;

    }

	SetGemValue(SG_SECS_NAME_CONTROL_STATE,		nCtrlState);
	SetGemValue(SG_SECS_NAME_CONTROL_SUBSTATE,	nSubState);
	return TRUE;
}
*/