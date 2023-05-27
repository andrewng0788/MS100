#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "SecsComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//================================================================
// HMI Server Command function implementation section
//================================================================

LONG CSecsComm::UpdateAction(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CSecsComm::SC_CmdEnterBondPage(IPC_CServiceMessage& svMsg)
{
	if (State() == IDLE_Q)
	{
		if (IsMSAutoLineStandloneMode() || IsMSAutoLineMode())
		{
			m_lEquipmentStatus = glSG_SCHEDULE_DOWN;
		}
		else
		{
			m_lEquipmentStatus = glSG_STANDBY;
		}
		
		SetEquipState(m_lEquipmentStatus);
		SetProcessState(PROCESS_IDLE_LOCAL);
		theApp.m_bMS_StandByButtonPressed_HMI = FALSE;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CSecsComm::SC_CmdEnterBondSetupPage(IPC_CServiceMessage& svMsg)
{
	if (State() == IDLE_Q)
	{
		SC_CmdEnterSetupPage(svMsg);
	}
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CSecsComm::SC_CmdEnterSetupPage(IPC_CServiceMessage& svMsg)
{
    m_lEquipmentStatus = glSG_SCHEDULE_DOWN; //glSG_ENGINEERING;
    SetEquipState(m_lEquipmentStatus);
	SetProcessState(PROCESS_SETUP);
    return 1;
}

LONG CSecsComm::SC_CmdEnterServicePage(IPC_CServiceMessage& svMsg)
{
    m_lEquipmentStatus = glSG_SCHEDULE_DOWN; //glSG_ENGINEERING;
    SetEquipState(m_lEquipmentStatus);
	SetProcessState(PROCESS_SETUP);

    return 1;
}

LONG CSecsComm::SC_CmdEnterHelpPage(IPC_CServiceMessage& svMsg)
{
    m_lEquipmentStatus = glSG_SCHEDULE_DOWN;
    SetEquipState(m_lEquipmentStatus);
	SetProcessState(PROCESS_SETUP);
    return 1;
}

LONG CSecsComm::SC_CmdLoadSettings(IPC_CServiceMessage &svMsg)
{
    m_szPPFilePath_HMI  = m_szPPFilePath;
    m_szHostIP_HMI      = m_szHostIPAddress;
	m_szEquipId_HMI		= m_szEquipId;			//aaa111
    m_bAutoStart_HMI    = m_bAutoStart;
    m_ulHostPort_HMI    = m_ulHostPort;
    m_usHsmsConnectMode_HMI = m_usHsmsConnectMode;
    m_ulConverTimer_HMI     = m_ulConverTimer;
    m_ulHsmsT3Timeout_HMI   = m_ulHsmsT3Timeout/1000;
    m_ulHsmsT5Timeout_HMI   = m_ulHsmsT5Timeout/1000;
    m_ulHsmsT6Timeout_HMI   = m_ulHsmsT6Timeout/1000;
    m_ulHsmsT7Timeout_HMI   = m_ulHsmsT7Timeout/1000;
    m_ulHsmsT8Timeout_HMI   = m_ulHsmsT8Timeout/1000;
    m_ulHsmsLinkTestTimer_HMI   = m_ulHsmsLinkTestTimer/1000;

//======================================================================
	if (m_hSecsCtrlMode == CTRL_MODE_REMOTE)
	{
		m_szSecsCtrlMode_HMI = "Remote";
	}
	else
	{
		m_szSecsCtrlMode_HMI = "Local";
	}

	m_bSecsOnline_HMI = !m_bSecsOnline;
//=======================================================================
	SetHmiVariable("SG_V_szSecsCtrlMode");

    QueryCommState();

	if (!m_bSecsGemSetupParaOk)
	{
		HmiMessage("SecsGem Setup Parameters Has Wrong, Can Not Start SecsGem");
	}
    return 1;
}


VOID CSecsComm::SC_UpdateSettings(BOOL bDispMess)
{
    m_szHostIPAddress   = m_szHostIP_HMI;
    m_bAutoStart        = m_bAutoStart_HMI;
    m_ulHostPort        = m_ulHostPort_HMI;
	m_szEquipId			= m_szEquipId_HMI;		//aaa111
    m_usHsmsConnectMode = m_usHsmsConnectMode_HMI;
    m_ulConverTimer     = m_ulConverTimer_HMI;
    m_ulHsmsT3Timeout   = m_ulHsmsT3Timeout_HMI*1000;
    m_ulHsmsT5Timeout   = m_ulHsmsT5Timeout_HMI*1000;
    m_ulHsmsT6Timeout   = m_ulHsmsT6Timeout_HMI*1000;
    m_ulHsmsT7Timeout   = m_ulHsmsT7Timeout_HMI*1000;
    m_ulHsmsT8Timeout   = m_ulHsmsT8Timeout_HMI*1000;
    m_ulHsmsLinkTestTimer = m_ulHsmsLinkTestTimer_HMI*1000;

	if (m_szSecsCtrlMode_HMI.Compare("Remote") == 0)
	{
		m_hSecsCtrlMode = CTRL_MODE_REMOTE;
	}
	else
	{
		m_hSecsCtrlMode = CTRL_MODE_LOCAL;
	}
    SaveData();

	if (!IsSecsGemInit() && bDispMess)
	{
		HmiMessage("Press [Eanble SECS] button to initialize the secs/gem system again");
	}

//		if (IsSecsGemInit())
//		{
//			HmiMessage("restart your application to let settings take effect");
//		}
//		else
//		{
//			HmiMessage("Press [start secs gem] button to initialize the secs/gem system");
//		}
}

LONG CSecsComm::SC_CmdUpdateSettings(IPC_CServiceMessage& svMsg)
{
	SC_UpdateSettings(TRUE);
	return 1;
}

LONG CSecsComm::SC_CmdStartSecsGem(IPC_CServiceMessage& svMsg)
{
    if( m_bUseSecsGem==FALSE )
        return -1;

	if (!IsSecsGemInit())
	{
		SetupSecsComm();
		m_pGemStation->EnableSecs(); // S1F13
	}

	return 1;
}

LONG CSecsComm::SC_CmdChangeSecsControlMode(IPC_CServiceMessage &svMsg)
{
	INT len = svMsg.GetMsgLen();
	char *pMsg = new char[len];
	svMsg.GetMsg(len, pMsg);

	if (strcmp(pMsg, "Remote") == 0)
	{
		m_szSecsCtrlMode_HMI = "Remote";
		m_hSecsCtrlMode = CTRL_MODE_REMOTE;
	}
	else
	{
		m_szSecsCtrlMode_HMI = "Local";
		m_hSecsCtrlMode = CTRL_MODE_LOCAL;
	}
	SaveData();
	delete[] pMsg;
	return 1;
}

LONG CSecsComm::SC_CmdEnableSecs(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable;
	try
	{
		svMsg.GetMsg(sizeof(bEnable), &bEnable);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return -1;
	}
	
//	if (!IsSecsGemInit())
//		return -1;

	if (bEnable)
	{
		SetAlert_WarningMsg(IDS_MS_WARNING_SECS_GEM, "Waiting to Enable SECS/GEM...");
		SC_UpdateSettings(FALSE);
		SetupSecsComm();
		m_pGemStation->EnableSecs(); // S1F13
	}
	else
	{
		SetAlert_WarningMsg(IDS_MS_WARNING_SECS_GEM, "Waiting to Disable SECS/GEM...");		
		TurnOffSecsComm();
		SC_UpdateSettings(FALSE);
	}
	CloseAlarm();
	SetAlarmLamp_Yellow(FALSE, TRUE);
    QueryCommState();
	if (!m_bSecsGemSetupParaOk)
	{
		HmiMessage("SecsGem Setup Parameters Has Wrong, Can Not Start SecsGem");
	}	
	return 1;
}

LONG CSecsComm::SC_CmdSwitchControlState(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
	{
		return 1;
	}
/*
	INT len = svMsg.GetMsgLen();
	char *pMsg = new char[len];
	svMsg.GetMsg(len, pMsg);
		
	if( strcmp(pMsg, "OFF LINE")==0 )
	{
		UpdateControlStatus(SECS_OFF_LINE, SECS_EQUIP_OFF_LINE);
	}
	if( strcmp(pMsg, "ON LINE--LOCAL")==0 )
	{
		UpdateControlStatus(SECS_ON_LINE, SECS_ON_LINE_LOCAL);
	}
	if( strcmp(pMsg, "ON LINE--REMOTE")==0 )
	{
		UpdateControlStatus(SECS_ON_LINE, SECS_ON_LINE_REMOTE);
	}
//*****SSSS
	delete[] pMsg;

    return 1;
*/
	try
	{
		BOOL bTemp;
		svMsg.GetMsg(sizeof(BOOL), &bTemp);
		SetControlOnlineState(bTemp ? SECS_ON_LINE : SECS_OFF_LINE);
		SaveData();
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return -1;
	}
	return 1;

}

LONG CSecsComm::SC_CmdEnableEvent(IPC_CServiceMessage &svMsg)
{
    struct TempStruct
    {
        BOOL bEnable;
        ULONG nCEID;
    };
    TempStruct temp;

	try
	{
		svMsg.GetMsg(sizeof(temp), &temp);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return -1;
	}

	if (!IsSecsGemInit())
		return -1;

	if( temp.bEnable)
        m_pGemStation->EnableEvent(temp.nCEID);
	else
        m_pGemStation->DisableEvent(temp.nCEID);

	return 1;
}

LONG CSecsComm::SC_CmdRequestTime(IPC_CServiceMessage& svMsg)
{
    if( IsSecsGemInit()==FALSE )
        return -1;

    m_pGemStation->RequestTime(); // S2F17

    return 1;
}

LONG CSecsComm::SC_CmdAreYouThere(IPC_CServiceMessage& svMsg)
{
	BOOL bResult;

    if( IsSecsGemInit()==FALSE )
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
        return 1;
	}

    if( m_pGemStation->TestOnLine() ) //S1F1
        HmiMessage("host alive");
    else
        HmiMessage("host linkage broke");

	bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
    return 1;

	//aaa111
/*
    if( IsSecsGemInit()==FALSE )
        return -1;

    if( m_pGemStation->TestOnLine() ) //S1F1
        HmiMessage("host alive");
    else
        HmiMessage("host linkage broke");
    return 1;
*/
}

LONG CSecsComm::SC_CmdSendEvent(IPC_CServiceMessage &svMsg)
{
    ULONG ulCEID;

    svMsg.GetMsg(sizeof(ULONG), &ulCEID);

	SetGemValue(MS_SECS_SV_WAFER_ID, "WaferID");	//3003
	SetGemValue(MS_SECS_SV_SLOT_NO, 1);				//3004
	SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");	//3005

    SendEvent(ulCEID, TRUE);

    return 1;
}

LONG CSecsComm::SC_CmdSetAlarm(IPC_CServiceMessage &svMsg)
{
    ULONG ulAlarmID;

    svMsg.GetMsg(sizeof(ULONG), &ulAlarmID);

	SetStatusMessage("Machine low pressure");
	SetAlert_Red_Yellow(ulAlarmID);

    SetAlarm(ulAlarmID, FALSE);
    Sleep(2000);
    ClearAlarm(ulAlarmID, FALSE);

    return 1;
}


/*
RemoteCmmdType
To obtain the data type of the remote command.

Time out
This IPC message is sent whenever a SECS timeout occurs.

Host Update Equipment Constant
This IPC message is sent after receiving the “Update Equipment Constant?message from the host.

Host Send Remote Command
This IPC message is sent after receiving a remote command from the host.

Host Link Reports
This IPC message is sent after receiving a “Linked Event Report?message from the host.

Host’s System Time
If the equipment request for host system time, this IPC message will be sent after receiving the “Request Time Data?
reply message from the host.

Host Request On/Off Line
This IPC message is sent if the equipment control state is changed as required by the host.

Host Define Report
This IPC message is sent after receiving a “Define Report?message from the host.

Host Request For Update System Time
This message is sent after receiving a “Set Time Request?message from the host.

Host Enable/Disable Alarm
This IPC message is sent after receiving a “Enable/Disable Alarm Request?message from the host.

Host Enable/Disable Collection Event
This IPC message is sent after receiving a“Enable/Disable Event Report?message from the host.

Collection Event Send Acknowledge
This IPC message is sent after receiving the “Send Event Report Acknowledge?message from the host or
when the Event Report cannot be sent.

Communicating State
This IPC message is sent when the SECS communicating state is changed.

Host Command Send
This IPC message is sent after receiving a “Host Command Send?request from the host.

Alarm Send Acknowledge
This IPC message is sent after receiving the “Send Alarm Acknowledge?message from the host or when the Alarm
message cannot be sent.
*/


LONG CSecsComm::UpdateProgressBar(IPC_CServiceMessage& svMsg)
{
	LONG lStep;
	svMsg.GetMsg(sizeof(LONG), &lStep);

	m_lProgressBarStep = lStep;

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CSecsComm::SetProgressBarLimit(IPC_CServiceMessage& svMsg)
{
	LONG lLimit;
	svMsg.GetMsg(sizeof(LONG), &lLimit);

	m_lProgressBarLimit = lLimit;

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


