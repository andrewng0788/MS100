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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//================================================================
// Constructor / destructor implementation section
//================================================================
IMPLEMENT_DYNCREATE(CSecsComm, CMS896AStn)

CSecsComm::CSecsComm()
{
	m_stTMsgTextList.RemoveAll();
	m_nTMsgTextCnt = 0;
    m_szStnName         = SECS_COMM_STN;
 //   m_bIsSecsCommEnabled		= FALSE;
	m_bTriggerCommStateAlarm	= FALSE;
	m_szSecCtrlStatus	= "EQUIP-OFF-LINE";
	//m_nOnOfflineState	= SECS_OFF_LINE;
    m_szEquipState      = "STANDBY";
	m_szCommState	    = "SECS_DISABLE";

    m_szPPFilePath      = _T("C:\\MapSorter\\OutputFiles\\Param");
	m_szHostIPAddress	= (LPCTSTR) _T("10.101.101.142");
	m_ulHostPort		= 5001;
	m_szEquipId			= "1";			//aaa111
    m_usHsmsConnectMode = HSMS_CONNECT_ACTIVE; // HSMS_CONNECT_PASSIVE  HSMS_CONNECT_ALTERNATING
	m_szSecsFilePath	= (LPCTSTR) _T("c:\\MapSorter\\SecsGem\\MS899Secs.gsv");

	m_szSecsCtrlMode_HMI = "Remote";
	m_hSecsCtrlMode		= CTRL_MODE_REMOTE;
	SetControlOnlineState(SECS_OFF_LINE);

	m_bAutoStart		= FALSE;
	m_ulConverTimer		= 120000;
	m_ulHsmsT3Timeout	= 45000;
	m_ulHsmsT5Timeout	= 10000;
	m_ulHsmsT6Timeout	= 5000;
	m_ulHsmsT7Timeout	= 10000;
	m_ulHsmsT8Timeout	= 5000;
	m_ulHsmsLinkTestTimer=60000;

	m_bSecsGemSetupParaOk = TRUE;
	//v4.37T3	//EquipManager
	m_ulCyclicReportUpdateCount			= 0;
	m_ulCNotifyIntervalInMinute			= 0;
	m_ulLastCyclicReportTimeInHour		= 0;
	m_ulLastCyclicReportTimeInMinute	= 0;

	m_lProgressBarStep = 0;	
	m_lProgressBarLimit = 1;

	m_pPPGBuffer = NULL;
	m_nPPGBufferIndex = 0;

	m_bS2F41RemoteStartCmd = FALSE;

	m_ulType2MapRowCount	= 0;
	m_ulType2MapColCount	= 0;
	m_arrRefDie = NULL;

	m_bPPGMAck = ACK7_LENGTH_ERROR;
/*
	if (_access("C:\\MapSorter\\SECSGEM\\MS899Secs_Alm.gen", F_OK) == F_OK)
	{
		CFile::Remove("C:\\MapSorter\\SECSGEM\\MS899Secs_Alm.gen");
	}

	if (_access("C:\\MapSorter\\SECSGEM\\MS899Secs.gsp", F_OK) == F_OK)
	{
		CFile::Remove("C:\\MapSorter\\SECSGEM\\MS899Secs.gsp");
	}
*/
}

CSecsComm::~CSecsComm()
{
}

BOOL CSecsComm::InitInstance()
{
	CMS896AStn::InitInstance();

	m_bHardwareReady    = TRUE;

    LoadData();
	if( m_bUseSecsGem==TRUE && m_bAutoStart==TRUE )
		SetupSecsComm();

	return TRUE;
}

INT CSecsComm::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	if( IsSecsGemInit() && (m_pGemStation->m_hThread) )
	{
		SendLexter_S1F65(0);									//Send S1F65 OFFLINE

        //m_lEquipmentStatus = glSG_SCHEDULE_DOWN;
        //SendEquipStatus(m_lEquipmentStatus);
		SetProcessState(PROCESS_ERROR);

		m_bIsSecsGemInit = FALSE;
        m_pGemStation->DisableSecs();
		m_pGemStation->CloseDown();
        m_pGemStation->PostThreadMessage(WM_QUIT, 0, 0);
        WaitForSingleObject(m_pGemStation->m_hThread, INFINITE);
	}

	// TODO:  perform any per-thread cleanup here
	return CMS896AStn::ExitInstance();
}

VOID CSecsComm::RegisterVariables()
{
	try
	{
		//-- Register HMI commands --//
		m_comServer.IPC_REG_SERVICE_COMMAND("UpdateAction",				&CSecsComm::UpdateAction);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_EnterBondPage",         &CSecsComm::SC_CmdEnterBondPage);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_EnterSetupPage",        &CSecsComm::SC_CmdEnterSetupPage);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_EnterBondSetupPage", &CSecsComm::SC_CmdEnterBondSetupPage);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_EnterServicePage",      &CSecsComm::SC_CmdEnterServicePage);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_EnterHelpPage",         &CSecsComm::SC_CmdEnterHelpPage);
		m_comServer.IPC_REG_SERVICE_COMMAND("SC_LoadSetting",           &CSecsComm::SC_CmdLoadSettings);
		m_comServer.IPC_REG_SERVICE_COMMAND("SC_UpdateSetting",		    &CSecsComm::SC_CmdUpdateSettings);
		m_comServer.IPC_REG_SERVICE_COMMAND("SC_StartSecsGem",			&CSecsComm::SC_CmdStartSecsGem);

		m_comServer.IPC_REG_SERVICE_COMMAND("SC_ChangeSecsControlMode",	&CSecsComm::SC_CmdChangeSecsControlMode);
		m_comServer.IPC_REG_SERVICE_COMMAND("SC_EnableSecs",			&CSecsComm::SC_CmdEnableSecs);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_SwitchControl",         &CSecsComm::SC_CmdSwitchControlState);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_EnableEvent",           &CSecsComm::SC_CmdEnableEvent);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_AreYouThere",           &CSecsComm::SC_CmdAreYouThere);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_SendTerminalMessage",   &CSecsComm::SC_CmdSendTerminalMsg);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_SendEvent",             &CSecsComm::SC_CmdSendEvent);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_SetAlarm",              &CSecsComm::SC_CmdSetAlarm);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_RequestTime",           &CSecsComm::SC_CmdRequestTime);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_GetCString",            &CSecsComm::SC_CmdGetCString);
		m_comServer.IPC_REG_SERVICE_COMMAND("SC_DownLoadMapTest",		&CSecsComm::SC_CmdDownLoadMapTest);
		m_comServer.IPC_REG_SERVICE_COMMAND("SC_DownLoadMap",			&CSecsComm::SC_CmdDownLoadMap);
		m_comServer.IPC_REG_SERVICE_COMMAND("SC_SendBackupMap",			&CSecsComm::SC_CmdSendBackupMap);
        // process program
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_GetPPFilePath",         &CSecsComm::SC_CmdGetPPFilePath);
//		m_comServer.IPC_REG_SERVICE_COMMAND("SC_RequestPPG",          	&CSecsComm::SC_CmdRequestPPG);
//		m_comServer.IPC_REG_SERVICE_COMMAND("SC_DownloadPPG",           &CSecsComm::SC_CmdDownloadPPG); // "SC_UploadPPG"
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_RequestPPDirectory",    &CSecsComm::SC_CmdRequestPPDirectory);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_DeletePPG",             &CSecsComm::SC_CmdDeletePPG);

        // Register GEM call back purpose commands
        // general
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_TIME_OUT,                 &CSecsComm::SG_CmdTimeOut);

		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_SEL_EQUIP_STATUS,		&CSecsComm::SG_CmdHostRecSelectEquipmentStatus);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_SEL_EQUIP_CONST,		&CSecsComm::SG_CmdHostRecSelectEquipmentConst);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_REQ_STATUS_NM_LIST,	&CSecsComm::SG_CmdHostRecReqStatusNameList);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_REQ_EC_NM_LIST,		&CSecsComm::SG_CmdHostRecReqEquipmentConstNameList);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_REQ_EQUIP_CONST,		&CSecsComm::SG_CmdHostRecReqEquipmentConst);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_EQUIP_CONST_UPDATE,       &CSecsComm::SG_CmdHostUpdateEC);

		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REQUEST_ON_OFF_LINE,	    &CSecsComm::SG_CmdHostReqOnOffLine);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_SECS_COMM_STATE,		    &CSecsComm::SG_CmdCommState);
        // date time
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_DATE_TIME_CHANGE,	        &CSecsComm::SG_CmdHostUpdateTime);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REQUEST_HOST_TIME,		&CSecsComm::SG_CmdHostTime);
        // remote command
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_HOST_COMMAND_RECEIVE,		&CSecsComm::SG_CmdHostSendCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REMOTE_COMMAND_RECEIVE,	&CSecsComm::SG_CmdHostSendRC);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_ALARM_ACK_RECEIVE,        &CSecsComm::SG_CmdHostReceiveAlarmAck);

		// process program
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REQ_GRANT_DN_PP,	        &CSecsComm::SG_CmdHostReqGrantDnPPG);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_DOWNLOAD_PROCESS_PROG,    &CSecsComm::SG_CmdHost_EquipDownloadPPG);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_UPLOAD_PROCESS_PROG,		&CSecsComm::SG_CmdHost_EquipUploadPPG);

        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_DOWNLOAD_PP_ACK,      &CSecsComm::SG_CmdDownloadPPAck);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_DELETE_PROCESS_PROG,      &CSecsComm::SG_CmdDeletePPG);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_DIRECTORY_PROCESS_PROG,   &CSecsComm::SG_CmdDirectoryPPG);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_REQ_EPPD,             &CSecsComm::SG_CmdReceiveRequestEPPD);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_DELETE_PPID_ACK,      &CSecsComm::SG_CmdHostDeletePPIDAck);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_PP_REQ,               &CSecsComm::SG_CmdReceivePPRequest);


        // report event alarm
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_EVENT_REPORT_DEFINE,	    &CSecsComm::SG_CmdHostDefineEvent);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_DEFINE_REPORT,	        &CSecsComm::SG_CmdHostDefineReport);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_EVENT_ACK_RECEIVE,        &CSecsComm::SG_CmdHostReceiveEventAck);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_HOST_REQ_CEID_REC,        &CSecsComm::SG_CmdHostReqEvent);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_ENABLE_ALARM_ACK,     &CSecsComm::SG_CmdReceiveEnableAlarmAck);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_ENABLE_EVENT_ACK,     &CSecsComm::SG_CmdReceiveEnableEventAck);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_HOST_REQ_REPORT_REC,      &CSecsComm::SG_CmdReceiveHostRequestReport);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_ENABLE_DISABLE_ALARM,	    &CSecsComm::SG_CmdHostEnableAlarm);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_ENABLE_DISABLE_EVENT,	    &CSecsComm::SG_CmdHostEnableEvent);
        // terminal message service
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_TERMINAL_DISP_ACK,    &CSecsComm::SG_CmdTerminalMsgAck);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_TERMINAL_MB_MSG_RECEIVE,  &CSecsComm::SG_CmdHostSendTerminalMsg_MB);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_TERMINAL_SERVICE_RECEIVE, &CSecsComm::SG_CmdHostSendTerminalMsg);
        // wafer map
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_MAP_DATA_ACK,			&CSecsComm::SG_CmdMapDataAck);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_MAP_DATA_ACK2,		&CSecsComm::SG_CmdMapDataAck2);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_MAP_SETUP_DATA,		&CSecsComm::SG_CmdMapSetupData);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_MAP_DATA_TYPE1,	    &CSecsComm::SG_CmdDownLoadType1Map);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_MAP_DATA_TYPE2,	    &CSecsComm::SG_CmdDownLoadType2Map);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REC_MAP_DATA_TYPE3,	    &CSecsComm::SG_CmdDownLoadType3Map);
		//Package Unload / Download		//v4.31T9
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DownloadPackage"),			&CSecsComm::SG_CmdDownloadPackage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UploadPackage"),			&CSecsComm::SG_CmdUploadPackage);
        // spool
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_SPOOL_ACTIVATE,           &CSecsComm::SG_CmdSpoolActivate);
        m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_SPOOL_DEACTIVATE,         &CSecsComm::SG_CmdSpoolDeactivate);
		//Progress Bar
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateProgressBar"),		&CSecsComm::UpdateProgressBar);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetProgressBarLimit"),		&CSecsComm::SetProgressBarLimit);
		//Lexter	//custom SEVCSGEM commands	//v4.11T1
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_LOAD_UNLOAD_PORT_STATUS_QY,	&CSecsComm::SG_CmdLoadUnloadPortStatusQy);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REQ_ONLINE_COM,				&CSecsComm::SG_CmdRequestOnlineCom);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_ONLINE_SUM_STATUS_RP_REPLY,	&CSecsComm::SG_CmdOnlineSumStatusRpReply);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_CAS_LOAD_COMPLETE_RP_REPLY,	&CSecsComm::SG_CmdCasLoadCompleteRpReply);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_CAS_UNLOAD_REQ_COM_RP_REPLY,	&CSecsComm::SG_CmdCasUnloadReqComRpReply);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_EQU_STATUS_CHANGE_RP_REPLY,	&CSecsComm::SG_CmdEquStatusChangeRpReply);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_LOT_PROCESS_DATA_TRANSFER,	&CSecsComm::SG_CmdLotProcessDataTransfer);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_LOT_STATUS_CHANGE_RP_REPLY,	&CSecsComm::SG_CmdLotStatusChangeRpReply);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_WAFER_PROCESS_DATA_RP_REPLY,	&CSecsComm::SG_CmdWaferProcessDataRpReply);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_LOT_PROCESS_DATA_RP_REPLY,	&CSecsComm::SG_CmdLotProcessDataRpReply);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_ALARM_OCC_REM_RP_REPLY,		&CSecsComm::SG_CmdAlarmOccRemRpReply);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_REPLY_LOT_PROCESS_DATA_REQ,	&CSecsComm::SG_CmdReplyLotProcessDataReq);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_LOT_CANCEL_REQ_REPLY,			&CSecsComm::SG_CmdLotCancelReqReply);
		m_comServer.IPC_REG_SERVICE_COMMAND(SG_CB_ONLINE_REPLY,					&CSecsComm::SG_CmdOnlineReply);
		
		//v4.51A12
		m_comServer.IPC_REG_SERVICE_COMMAND("Test",		&CSecsComm::Test);
		m_comServer.IPC_REG_SERVICE_COMMAND("TestSecs",	&CSecsComm::TestSecs); //4.51D1 Secs

		//Equipment statistics Expired Days
		m_comServer.IPC_REG_SERVICE_COMMAND("EquipStateStatisticsExpiredDays",	&CSecsComm::EquipStateStatisticsExpiredDays);

		m_comServer.IPC_REG_SERVICE_COMMAND("EquipStateTimeLogDateInput",		&CSecsComm::EquipStateTimeLogDateInput);
		m_comServer.IPC_REG_SERVICE_COMMAND("LoadEquipStateTime",				&CSecsComm::LoadEquipStateTime);

		m_comServer.IPC_REG_SERVICE_COMMAND("EquipStateTimeExpireDateInput",	&CSecsComm::EquipStateTimeExpireDateInput);
		m_comServer.IPC_REG_SERVICE_COMMAND("DeleteEquipStateTime",				&CSecsComm::DeleteEquipStateTime);

		//Register Variable here 
        RegVariable(_T("SG_V_szPPFilePath"),            &m_szPPFilePath_HMI);
		RegVariable(_T("SG_V_szHostIP"),			    &m_szHostIP_HMI);
		RegVariable(_T("SG_V_szConnMode"),		        &m_szCommState);
		RegVariable(_T("SG_V_szSecsCtrlStatus"),	    &m_szSecCtrlStatus);
		RegVariable(_T("SG_V_szSecsProcessState"),		&m_szProcessState);	//Process State
        RegVariable(_T("SG_V_szSecsEquipState"),        &m_szEquipState);	//Equipment State

		RegVariable(_T("SG_V_szSecsCtrlMode"),			&m_szSecsCtrlMode_HMI);  //Machine Control Mode
		RegVariable(_T("SG_V_bIsSecsGemInit"),		    &m_bIsSecsGemInit);
		RegVariable(_T("SG_V_bIsSecsCommEnabled"),      &m_bIsSecsCommEnabled);
		RegVariable(_T("SG_V_bIsSecsOnLine"),			&m_bSecsOnline_HMI);
		RegVariable(_T("SG_szTempString"),				&m_szTempString);		//OsramTrip 8/22
		RegVariable(_T("SG_V_bAutoStart"),		        &m_bAutoStart_HMI);
		RegVariable(_T("SG_V_ulHostPort"),			    &m_ulHostPort_HMI);
		RegVariable(_T("SG_V_szEquipId"),				&m_szEquipId_HMI);
        RegVariable(_T("SG_V_usHsmsConnectMode"),       &m_usHsmsConnectMode_HMI);
		RegVariable(_T("SG_V_ulConverTimer"),		    &m_ulConverTimer_HMI);
		RegVariable(_T("SG_V_ulT3Timeout"),		        &m_ulHsmsT3Timeout_HMI);
		RegVariable(_T("SG_V_ulT5Timeout"),		        &m_ulHsmsT5Timeout_HMI);
		RegVariable(_T("SG_V_ulT6Timeout"),		        &m_ulHsmsT6Timeout_HMI);
		RegVariable(_T("SG_V_ulT7Timeout"),		        &m_ulHsmsT7Timeout_HMI);
		RegVariable(_T("SG_V_ulT8Timeout"),		        &m_ulHsmsT8Timeout_HMI);
		RegVariable(_T("SG_V_ulLinkTestTimer"),	        &m_ulHsmsLinkTestTimer_HMI);

		RegVariable(_T("ST_lProgressBarStep"),			&m_lProgressBarStep);
		RegVariable(_T("ST_lProgressBarLimit"),			&m_lProgressBarLimit);	

		RegisterVariables_EquipmentStaistics_VAR();
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}


//================================================================
// Function Name: 		TurnOffSecsComm
// Input arguments:     None
// Output arguments:	None
// Description:   		Turn Off SECS Communication
// Return:				None
// Remarks:				None
//================================================================
VOID CSecsComm::TurnOffSecsComm()
{
	if (((m_pGemStation != NULL) && (m_pGemStation->m_hThread)) || IsSecsGemInit())
	{
		TRACE(_T("Now comes to GemStation Destructor\n"));
		try
		{
			SetControlOnlineState(SECS_OFF_LINE);
			UpdateControlState(FALSE);
			m_bIsSecsGemInit = FALSE;
			m_pGemStation->DisableSecs();
			m_pGemStation->CloseDown();
			m_pGemStation->PostThreadMessage(WM_QUIT, 0, 0);
			WaitForSingleObject(m_pGemStation->m_hThread, INFINITE);
			m_pGemStation = NULL;
			//CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
			//(pAppMod->m_pAppGemStation) = m_pGemStation;
		}
		catch (CAsmException e)
		{
			m_bIsSecsGemInit = FALSE;
			m_szSecCtrlStatus = "SECS-OFF-LINE";
			m_pGemStation = NULL;
			//CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
			//(pAppMod->m_pAppGemStation) = m_pGemStation;
			DisplayException(e);
			return;
		}
	}
}


//================================================================
// Function Name: 		SetupSecsPara
// Input arguments:     None
// Output arguments:	None
// Description:   		Setup SECS Communication Parameters
// Return:				None
// Remarks:				None
//================================================================
BOOL CSecsComm::SetupSecsPara()
{
	if (m_pGemStation == NULL) 
	{
		return FALSE;
	}

	CString szLog;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	// set communication state
	m_pGemStation->SetCommunicationState(TRUE);		

	szLog = "System: " + m_szEquipId;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);		//v4.13

	//m_pGemStation->InitSystem(_T("MS899"), _T("1.05"), _T("1")); // mdln, softrev and equipid
	//m_pGemStation->InitSystem(_T("MS899"), _T("4.13"), _T(m_szEquipId)); // mdln, softrev and equipid
	m_pGemStation->InitSystem(pAppMod->GetMachineModel(), pAppMod->GetSoftVersion(), _T(m_szEquipId) );
	m_pGemStation->InitSystem(SECS_EQUIP);				// as equipment

	szLog.Format("T(3)=%lu, T(5)=%lu, T(6)=%lu, T(7)=%lu, T(8)=%lu, Lnk=%lu", 
				 m_ulHsmsT3Timeout,  m_ulHsmsT5Timeout,  m_ulHsmsT6Timeout, 
				 m_ulHsmsT7Timeout,  m_ulHsmsT8Timeout,  m_ulHsmsLinkTestTimer);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);		//v4.13

	m_pGemStation->SetTimer(m_ulHsmsT3Timeout,  m_ulHsmsT5Timeout,  m_ulHsmsT6Timeout, 
							m_ulHsmsT7Timeout,  m_ulHsmsT8Timeout,  m_ulHsmsLinkTestTimer);
	m_pGemStation->SetConverTimer( m_ulConverTimer );
	m_pGemStation->SetMedia( PROTOCOL_HSMS ); // HSMS
	m_pGemStation->SetLogInformation(gszUSER_DIRECTORY + "\\History\\secsgem.log", 1000000, 3, TRUE);	//v4.12	//Implement SECSGEM log file

	//Connect
	m_pGemStation->Create(m_szSecsFilePath, TRUE);		// secs file and log flag	//v4.12		//TUrn ON logging (TRUE)
	//m_pGemStation->SetLogFileName(gszUSER_DIRECTORY + "\\History\\MS899SecsGem.log");
	//m_pGemStation->SetLogFileSize(2*1024*1024);
	m_pGemStation->SetNumBackupForLog(5);
	m_pGemStation->EnableLog();
	//m_pGemStation->DisableLog();

	szLog.Format("Init HSMS (Port=%lu) (Mode=%ld)", m_ulHostPort, m_usHsmsConnectMode);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);		//v4.13

	m_pGemStation->InitHsms(m_szHostIPAddress, m_ulHostPort, m_usHsmsConnectMode); // HSMS_CONNECT_ACTIVE

	//==============================================================================
	m_pGemStation->ResetControlState(TRUE);
	// To set the default Off Line control state when Attempt On Line request is failed.
	m_pGemStation->SetOnLineFailState(SECS_EQUIP_OFF_LINE); // SECS_HOST_OFF_LINE

	//v4.13T5
	m_pGemStation->UpdateControlState(SECS_OFF_LINE, SECS_EQUIP_OFF_LINE, TRUE); //Init
//	m_pGemStation->UpdateControlState(SECS_ON_LINE, SECS_ON_LINE_LOCAL, TRUE);
//	m_szTempString = _T("ON LINE--LOCAL");

	return TRUE;
}


//================================================================
// Protected function implementation section
//================================================================
VOID CSecsComm::SetupSecsComm()
{
	m_bSecsGemSetupParaOk = TRUE;
	if (((m_pGemStation != NULL) && (m_pGemStation->m_hThread)) || IsSecsGemInit())
	{
		TurnOffSecsComm();
	}

	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("Setup SECSGEM ....");		//v4.11

	try
	{
        // get gem station object pointer
		m_pGemStation = (CGemStation*) (AfxBeginThread(RUNTIME_CLASS(CGemStation), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED));
		if (m_pGemStation == NULL)
		{
			CString szErr;
			szErr = "ERROR: Start SECS/GEM Failed";
			SetErrorMessage(szErr);
			HmiMessage_Red_Yellow(szErr);
			return;
		}

		SetupSecsPara();

		CMSLogFileUtility::Instance()->MS_LogSECSGEM("Register IPC config ....");		//v4.11
		//register command for SecsGem to call
        // for the call back function to application
        // general
//		m_pGemStation->IPC_Config(TIME_OUT,				    m_szStnName,	SG_CB_TIME_OUT);
		m_pGemStation->IPC_Config(EQUIP_CONST_UPDATE,	    m_szStnName,	SG_CB_EQUIP_CONST_UPDATE);
		m_pGemStation->IPC_Config(REQUEST_ON_OFF_LINE,	    m_szStnName,	SG_CB_REQUEST_ON_OFF_LINE);
		m_pGemStation->IPC_Config(SECS_COMM_STATE,		    m_szStnName,	SG_CB_SECS_COMM_STATE);

		m_pGemStation->IPC_Config(REC_SEL_EQUIP_STATUS,		m_szStnName,	SG_CB_REC_SEL_EQUIP_STATUS);
		m_pGemStation->IPC_Config(REC_SEL_EQUIP_CONST,		m_szStnName,	SG_CB_REC_SEL_EQUIP_CONST);
		m_pGemStation->IPC_Config(REC_REQ_STATUS_NM_LIST,	m_szStnName,	SG_CB_REC_REQ_STATUS_NM_LIST);
		m_pGemStation->IPC_Config(REC_REQ_EC_NM_LIST,		m_szStnName,	SG_CB_REC_REQ_EC_NM_LIST);
		m_pGemStation->IPC_Config(REC_REQ_EQUIP_CONST,		m_szStnName,	SG_CB_REC_REQ_EQUIP_CONST);

        //        REC_OFFLINE_ACK						=	40;
        //        REC_ONLINE_ACK						=	41;
        //        REC_REQ_EQUIP_STATUS				=	35;
        //        REC_REQ_STATUS_NM_LIST				=	36;
        //        REC_REQ_EC_NM_LIST					=	37;
        //        REC_REQ_EQUIP_CONST					=	38;
        // time
		m_pGemStation->IPC_Config(REQUEST_HOST_TIME,	    m_szStnName,	SG_CB_REQUEST_HOST_TIME);
		m_pGemStation->IPC_Config(DATE_TIME_CHANGE,		    m_szStnName,	SG_CB_DATE_TIME_CHANGE);
        //        REC_SET_DATETIME_ACK				=	42;
        // report event alarm
		m_pGemStation->IPC_Config(DEFINE_REPORT,		    m_szStnName,	SG_CB_DEFINE_REPORT);
		m_pGemStation->IPC_Config(EVENT_REPORT_DEFINE,	    m_szStnName,	SG_CB_EVENT_REPORT_DEFINE);
		m_pGemStation->IPC_Config(ENABLE_DISABLE_EVENT,     m_szStnName,	SG_CB_ENABLE_DISABLE_EVENT);
		m_pGemStation->IPC_Config(EVENT_ACK_RECEIVE,	    m_szStnName,	SG_CB_EVENT_ACK_RECEIVE);
		m_pGemStation->IPC_Config(ENABLE_DISABLE_ALARM,     m_szStnName,	SG_CB_ENABLE_DISABLE_ALARM);
        m_pGemStation->IPC_Config(ALARM_ACK_RECEIVE,        m_szStnName,    SG_CB_ALARM_ACK_RECEIVE);
		m_pGemStation->IPC_Config(HOST_REQ_CEID_REC,	    m_szStnName,	SG_CB_HOST_REQ_CEID_REC);
        m_pGemStation->IPC_Config(REC_ENABLE_ALARM_ACK,     m_szStnName,    SG_CB_REC_ENABLE_ALARM_ACK);
        m_pGemStation->IPC_Config(REC_ENABLE_EVENT_ACK,     m_szStnName,    SG_CB_REC_ENABLE_EVENT_ACK);
        m_pGemStation->IPC_Config(HOST_REQ_REPORT_REC,      m_szStnName,    SG_CB_HOST_REQ_REPORT_REC);
        //        REC_REQ_ALARM_INFO					=	39;
        //        REC_DEFINE_REPORT_ACK				=	44;
        //        REC_LINK_EVTRPT_ACK					=	45;
        //        HOST_RECEIVE_CEID
        //        HOST_RECEIVE_ALARM
        //        REC_ALARM_ENABLE_LIST
        // trace data and limit monitoring
        //        REC_TRACE_INIT_ACK					=	46;
        //        REC_TRACE_DATA						=	47;
        //        REC_REQ_LIMIT_ATTB					=	48;
        //        REC_DEFINE_LIMIT_ACK				=	49;
        // remote command
        //        REC_HOST_CMMD_ACK					=	30;
		m_pGemStation->IPC_Config(REMOTE_COMMAND_RECEIVE,   m_szStnName,	SG_CB_REMOTE_COMMAND_RECEIVE);
		m_pGemStation->IPC_Config(HOST_COMMAND_RECEIVE,     m_szStnName,	SG_CB_HOST_COMMAND_RECEIVE);
        // terminal message service
		m_pGemStation->IPC_Config(TERMINAL_SERVICE_RECEIVE, m_szStnName,	SG_CB_TERMINAL_SERVICE_RECEIVE);
		m_pGemStation->IPC_Config(TERMINAL_MULTI_BLK_MSG_RECEIVE,   m_szStnName, SG_CB_TERMINAL_MB_MSG_RECEIVE);
		m_pGemStation->IPC_Config(REC_TERMINAL_DISP_ACK,    m_szStnName,	SG_CB_REC_TERMINAL_DISP_ACK);
        //        REC_TERMINAL_REQ					=	52;
        // process program
        m_pGemStation->IPC_Config(UPLOAD_PROCESS_PROG,      m_szStnName,    SG_CB_UPLOAD_PROCESS_PROG);
        m_pGemStation->IPC_Config(DOWNLOAD_PROCESS_PROG,    m_szStnName,    SG_CB_DOWNLOAD_PROCESS_PROG);
		m_pGemStation->IPC_Config(REQ_GRANT_DN_PP,		    m_szStnName,	SG_CB_REQ_GRANT_DN_PP);
        m_pGemStation->IPC_Config(DELETE_PROCESS_PROG,      m_szStnName,    SG_CB_DELETE_PROCESS_PROG);
        m_pGemStation->IPC_Config(DIRECTORY_PROCESS_PROG,   m_szStnName,    SG_CB_DIRECTORY_PROCESS_PROG);
        m_pGemStation->IPC_Config(REC_DELETE_PPID_ACK,      m_szStnName,    SG_CB_REC_DELETE_PPID_ACK);
        m_pGemStation->IPC_Config(REC_PP_REQ,               m_szStnName,    SG_CB_REC_PP_REQ);
        m_pGemStation->IPC_Config(REC_DOWNLOAD_PP_ACK,      m_szStnName,    SG_CB_REC_DOWNLOAD_PP_ACK);
        m_pGemStation->IPC_Config(REC_REQ_EPPD,             m_szStnName,    SG_CB_REC_REQ_EPPD);
        // spool
        m_pGemStation->IPC_Config(SPOOL_ACTIVATE,           m_szStnName,    SG_CB_SPOOL_ACTIVATE);
        m_pGemStation->IPC_Config(SPOOL_DEACTIVATE,         m_szStnName,    SG_CB_SPOOL_DEACTIVATE);
        //        REC_RESET_SPOOL_ACK					=	50;
        //        REC_SPOOL_DATA_ACK					=	51;
        // wafer map
        //        REC_MAP_SETUP_ACK					=	60;
        //        REC_MAP_SETUP_DATA					=	61;
        //        REC_MAP_DATA_ACK1					=	63;
        //        REC_MAP_DATA_ACK2					=	65;
        //        REC_MAP_DATA_ACK3					=	67;
        //        REC_REQ_MAP_DATA_TYPE1				=	69;
        //        REC_REQ_MAP_DATA_TYPE2				=	70;
        //        REC_REQ_MAP_DATA_TYPE3				=	71;
        //        REC_MAP_ERROR_REPORT				=	72;
        //        REC_MAP_NEG_GRANT_ACK				=	74;

        m_pGemStation->IPC_Config(REC_MAP_SETUP_DATA,		m_szStnName,	SG_CB_REC_MAP_SETUP_DATA);
        m_pGemStation->IPC_Config(REC_MAP_SETUP_ACK,		m_szStnName,	SG_CB_REC_MAP_DATA_ACK);
        m_pGemStation->IPC_Config(REC_MAP_DATA_ACK2,		m_szStnName,	SG_CB_REC_MAP_DATA_ACK2);
		m_pGemStation->IPC_Config(REC_MAP_DATA_TYPE1,		m_szStnName,	SG_CB_REC_MAP_DATA_TYPE1);
		m_pGemStation->IPC_Config(REC_MAP_DATA_TYPE2,		m_szStnName,	SG_CB_REC_MAP_DATA_TYPE2);
		m_pGemStation->IPC_Config(REC_MAP_DATA_TYPE3,		m_szStnName,	SG_CB_REC_MAP_DATA_TYPE3);
        // others
        // LOOPBACK_DIAGNOSTIC_REQUEST
        // REC_NEW_EC_SET_ACK
        // REC_MULTIBLK_NOT_ALLOWED
        // REC_TRANSFER_JOB
        // REC_TRANSFER_COMMAND
        // REC_JOB_ALERT_ACK
        // REC_HANDOFF_VER
        // REC_HANDOFF_RDY
        // REC_ATTR_DATA
        // REC_DEFINE_MESSAGE

		//v4.11T1	//Lexter Requirement
		//m_pGemStation->IPC_Config(LOAD_UNLOAD_PORT_STATUS_QY,		m_szStnName,	SG_CB_LOAD_UNLOAD_PORT_STATUS_QY);	//S1F77
		//m_pGemStation->IPC_Config(REQ_ONLINE_COM,					m_szStnName,	SG_CB_REQ_ONLINE_COM);				//S1F79
		//m_pGemStation->IPC_Config(ONLINE_SUM_STATUS_RP_REPLY,		m_szStnName,	SG_CB_ONLINE_SUM_STATUS_RP_REPLY);	//S1F70
		//m_pGemStation->IPC_Config(CAS_LOAD_COMPLETE_RP_REPLY,		m_szStnName,	SG_CB_CAS_LOAD_COMPLETE_RP_REPLY);	//S1F74
		//m_pGemStation->IPC_Config(CAS_UNLOAD_REQ_COM_RP_REPLY,	m_szStnName,	SG_CB_CAS_UNLOAD_REQ_COM_RP_REPLY);	//S1F76
		//m_pGemStation->IPC_Config(EQU_STATUS_CHANGE_RP_REPLY,		m_szStnName,	SG_CB_EQU_STATUS_CHANGE_RP_REPLY);	//S1F66
		//m_pGemStation->IPC_Config(LOT_PROCESS_DATA_TRANSFER,		m_szStnName,	SG_CB_LOT_PROCESS_DATA_TRANSFER);	//S1F65
		//m_pGemStation->IPC_Config(LOT_STATUS_CHANGE_RP_REPLY,		m_szStnName,	SG_CB_LOT_STATUS_CHANGE_RP_REPLY);	//S1F68
		//m_pGemStation->IPC_Config(WAFER_PROCESS_DATA_RP_REPLY,	m_szStnName,	SG_CB_WAFER_PROCESS_DATA_RP_REPLY);	//S6F66
		//m_pGemStation->IPC_Config(LOT_PROCESS_DATA_RP_REPLY,		m_szStnName,	SG_CB_LOT_PROCESS_DATA_RP_REPLY);	//S6F70
		//m_pGemStation->IPC_Config(ALARM_OCC_REM_RP_REPLY,			m_szStnName,	SG_CB_ALARM_OCC_REM_RP_REPLY);		//S5F66
		//m_pGemStation->IPC_Config(REPLY_LOT_PROCESS_DATA_REQ,		m_szStnName,	SG_CB_REPLY_LOT_PROCESS_DATA_REQ);	//S7F72
		//m_pGemStation->IPC_Config(LOT_CANCEL_REQ_REPLY,			m_szStnName,	SG_CB_LOT_CANCEL_REQ_REPLY);		//S7F74
		//m_pGemStation->IPC_Config(ONLINE_REPLY,					m_szStnName,	SG_CB_ONLINE_REPLY);				//S1F2

        // Register the status variable name
		m_pGemStation->RegisterVarName(SECS_NAME_ECID,				    SG_SECS_NAME_ECID);
		m_pGemStation->RegisterVarName(SECS_NAME_ALARMID,			    SG_SECS_NAME_ALARMID);
		m_pGemStation->RegisterVarName(SECS_NAME_COMM_TIMER,		    SG_SECS_NAME_COMM_TIMER);
		m_pGemStation->RegisterVarName(SECS_NAME_SPOOL_START_TM,	    SG_SECS_NAME_SPOOL_START_TM);
		m_pGemStation->RegisterVarName(SECS_NAME_SPOOL_FULL_TM,		    SG_SECS_NAME_SPOOL_FULL_TM);
		m_pGemStation->RegisterVarName(SECS_NAME_MAX_SPOOL_TRANSMIT,    SG_SECS_NAME_MAX_SPOOL_TRANSMIT);
		m_pGemStation->RegisterVarName(SECS_NAME_OVER_WRITE_SPOOL,	    SG_SECS_NAME_OVER_WRITE_SPOOL);
		m_pGemStation->RegisterVarName(SECS_NAME_SPOOL_COUNT_TOTAL,     SG_SECS_NAME_SPOOL_COUNT_TOTAL);
		m_pGemStation->RegisterVarName(SECS_NAME_SPOOL_COUNT_ACTUAL,    SG_SECS_NAME_SPOOL_COUNT_ACTUAL);
		m_pGemStation->RegisterVarName(SECS_NAME_PROCESS_STATE,		    SG_SECS_NAME_PROCESS_STATE);
		m_pGemStation->RegisterVarName(SECS_NAME_TIME_FORMAT,		    SG_SECS_NAME_TIME_FORMAT);
		m_pGemStation->RegisterVarName(SECS_NAME_ALM_LIST,			    SG_SECS_NAME_ALM_LIST);
		m_pGemStation->RegisterVarName(SECS_NAME_EVENT_ENABLE_LIST,     SG_SECS_NAME_EVENT_ENABLE_LIST);
		m_pGemStation->RegisterVarName(SECS_NAME_ALM_ENABLE_LIST,	    SG_SECS_NAME_ALM_ENABLE_LIST);
		m_pGemStation->RegisterVarName(SECS_NAME_CLOCK,				    SG_SECS_NAME_CLOCK);
		m_pGemStation->RegisterVarName(SECS_NAME_LIMIT_TRANSITION_TYPE, SG_SECS_NAME_LIMIT_TRANSITION_TYPE);
		m_pGemStation->RegisterVarName(SECS_NAME_LIMIT_VARIABLE,	    SG_SECS_NAME_LIMIT_VARIABLE);
		m_pGemStation->RegisterVarName(SECS_NAME_LIMIT_LIST,		    SG_SECS_NAME_LIMIT_LIST);
		m_pGemStation->RegisterVarName(SECS_NAME_ALARM_CLOCK,		    SG_SECS_NAME_ALARM_CLOCK);
		m_pGemStation->RegisterVarName(SECS_NAME_SPOOL_ENABLE,		    SG_SECS_NAME_SPOOL_ENABLE);
		m_pGemStation->RegisterVarName(SECS_NAME_CONTROL_STATE,		    SG_SECS_NAME_CONTROL_STATE);
		//OsramTrip 8/22
		m_pGemStation->RegisterVarName(SECS_NAME_PRE_CONTROL_STATE,		SG_SECS_NAME_PRE_CONTROL_STATE);
		m_pGemStation->RegisterVarName(SECS_NAME_CONTROL_SUBSTATE,		SG_SECS_NAME_CONTROL_SUBSTATE);
		m_pGemStation->RegisterVarName(SECS_NAME_PRE_CONTROL_SUBSTATE,	SG_SECS_NAME_PRE_CONTROL_SUBSTATE);

        // To register a CEID to the GemStation. all type below
        // GemStation needs to send a correct Collection Event to the host
        // when some special event occurs.
        m_pGemStation->RegisterCEID(SECS_CEID_SPOOL_TRANSMIT_FAILURE,   11);
		m_pGemStation->RegisterCEID(SECS_CEID_OPERATOR_UPDATE_EC,       7000);
        m_pGemStation->RegisterCEID(SECS_CEID_SPOOL_DEACTIVATE,         4321);
        m_pGemStation->RegisterCEID(SECS_CEID_SPOOL_ACTIVATE,           1234);

		m_pGemStation->RegisterCEID(SECS_CEID_EQUIP_OFFLINE,            SG_CEID_CONTROL_STATE_CHANGE); //50
		m_pGemStation->RegisterCEID(SECS_CEID_ONLINE_REMOTE,            SG_CEID_CONTROL_STATE_CHANGE); //52
		m_pGemStation->RegisterCEID(SECS_CEID_ONLINE_LOCAL,             SG_CEID_CONTROL_STATE_CHANGE); //51

        // register limit monitor
        //m_pGemStation->RegisterLimitMonitor(nSVID, nCEID, nMaxLimit, nEventLimitID);

		m_pGemStation->SetWMDataSize(4,4,4,4,4,4,4);	//v4.59A13
        // set trace data collection property
        // set max spool size

        // kicked started from suspended after all initialization done
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("Resume connection ....");		//v4.11

		(pAppMod->m_pAppGemStation) = m_pGemStation;

		m_pGemStation->SetMaxHsmsDataSize(8000000); //8M size
		m_pGemStation->EnableS1F3Wait(TRUE);
		//initialize equipment status
		(*m_pGemStation)[MS_SECS_SV_E10_STATE] = m_lEquipState;
		m_pGemStation->ResumeThread();
		m_bIsSecsGemInit = TRUE;

		SetControlOnlineState(SECS_ON_LINE);
		//v4.59A32
		m_bEquipmentManager = pAppMod->GetFeatureStatus(MS896A_FUNC_HOST_COMM_EQUIP_MANAGER);

		SetProcessState(PROCESS_SETUP);

		QueryCommState();
		SetEquipState(m_lEquipmentStatus);
		//CheckControlStatus();		//OsramTrip 8/22

		//v4.59A33
		CString szPKGFilename;
		szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];
		SetGemValue(MS_SECS_SV_CURR_RECIPE_NAME, szPKGFilename);
		SendEvent(SG_CEID_PROCESS_PROGRAM_SELECTED);

	}
	catch (CAsmException e)
	{
		m_bIsSecsGemInit = FALSE;
        m_szSecCtrlStatus = "EQUIP-OFF-LINE";
		m_bSecsGemSetupParaOk = FALSE;
		DisplayMessage("SecsGem Setup Parameters Has Wrong, Can Not Start SecsGem");
		DisplayException(e);
		return;
	}

	//v4.11T1
	if (pAppMod->GetCustomerName() == "Lextar")
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("Enable LEXTAR configuration ....");		//v4.11
		EnableLextarSecsGemFcns(TRUE);
		//SendLexter_S1F65(1);				//Send S1F65 IDLE
	}
	else
	{
		EnableLextarSecsGemFcns(FALSE);
	}

	SetEquipmentManagementInfo(TRUE);
}


VOID CSecsComm::SetControlOnlineState(BOOL bSecsOnline)
{
	if (this == NULL) 
	{
		return;
	}
	m_bSecsOnline = bSecsOnline;
	m_bSecsOnline_HMI = !m_bSecsOnline;
}


/*
// On_Off Line State
const	INT		SECS_ON_LINE					= 0;
const	INT		SECS_OFF_LINE					= 1;

const	INT		SECS_EQUIP_OFF_LINE				= 1;
const	INT		SECS_ATTEMPT_ON_LINE			= 2;
const	INT		SECS_HOST_OFF_LINE				= 3;
const	INT		SECS_ON_LINE_LOCAL				= 4;
const	INT		SECS_ON_LINE_REMOTE				= 5;
*/
/*
BOOL CSecsComm::UpdateControlStatus(INT nCtrl, INT nSubCtrl, BOOL bInit)
{
    if( IsSecsGemInit()==FALSE )
        return FALSE;

	INT nNewCtrl	= nCtrl;
	INT nNewSubCtrl	= nSubCtrl;

    switch( nSubCtrl )
    {
    case SECS_ON_LINE_LOCAL:
        m_szSecCtrlStatus = "ON-LINE-LOCAL";
		if (m_nOnOfflineState == SECS_OFF_LINE)
		{
			nNewCtrl	= SECS_OFF_LINE;
			nNewSubCtrl = SECS_ATTEMPT_ON_LINE;
		}
        break;

    case SECS_ON_LINE_REMOTE:
        m_szSecCtrlStatus = "ON-LINE-REMOTE";
		if (m_nOnOfflineState == SECS_OFF_LINE)
		{
			nNewCtrl	= SECS_OFF_LINE;
			nNewSubCtrl = SECS_ATTEMPT_ON_LINE;
			m_szSecCtrlStatus = "ON-LINE-LOCAL";
		}
        break;

    case SECS_EQUIP_OFF_LINE:
        m_szSecCtrlStatus = "EQUIP-OFF-LINE";
        break;

    case SECS_HOST_OFF_LINE:
        m_szSecCtrlStatus = "HOST-OFF-LINE";
        break;
    }

    m_pGemStation->UpdateControlState(nNewCtrl, nNewSubCtrl, bInit);

	//OsramTrip 8/22
	CString szTime;
	CTime theTime = CTime::GetCurrentTime();
	szTime = theTime.Format("%m/%d/%Y %I:%M %p");

	SetGemValue(SG_SECS_NAME_CONTROL_STATE,		nCtrl);
	SetGemValue(SG_SECS_NAME_CONTROL_SUBSTATE,	nSubCtrl);
	SendEvent(SG_CEID_CONTROL_STATE_CHANGE);

	CString szLog;
	szLog.Format("UpdateControlState - new SubState = %d (%s), new CtrlState = %d (%d)",
			nSubCtrl, m_szSecCtrlStatus, nCtrl, m_nOnOfflineState);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	m_nOnOfflineState	= nCtrl;

    return TRUE;
}
*/