#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "SecsComm.h"
#include "SecsMapCoord.h"
#include "SecsMapCoordData.h"
#include "LogFileUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



LONG CSecsComm::SG_CmdTimeOut(IPC_CServiceMessage& svMsg) // TIME_OUT
{
	if (!IsSecsGemInit())
		return -1;

    INT len;
    CString str;

	try
	{
        len = svMsg.GetMsgLen()/sizeof(INT);
		str.Format("SG_TimeOut:%d",len);
		DisplayMessage(str);
	}
	catch( CAsmException e)
	{
		//if( pInt!=NULL )
		//{
		//	delete[] pInt;
		//	pInt = NULL;
		//}
	}	

	return 1;

	//aaa111
/*
	if (!IsSecsGemInit())
		return -1;

    INT *pInt, len;
    CString str;

	try
	{
        len = svMsg.GetMsgLen()/sizeof(INT);
        if( len>0 )
        {
            pInt = new INT[len];
            svMsg.GetMsg(sizeof(pInt), pInt);

            str.Format("Timer: %d, %d CE On Queue:", pInt[0], pInt[1]);
            for(len=0; len<pInt[1]; len++)
            {
                str.Format(" %d,", pInt[len+2]);
            }
            DisplayMessage(str);

//******SSSS
            delete[] pInt;
        }
	}
	catch( CAsmException e)
	{
		if( pInt!=NULL )
		{
			delete[] pInt;
			pInt = NULL;
		}
	}	

	return 1;
*/
}

LONG CSecsComm::SG_CmdHostUpdateEC(IPC_CServiceMessage& svMsg)		// EQUIP_CONST_UPDATE S2F15
{
	if (!IsSecsGemInit())
		return -1;

	//v4.37T3
	// This fcn is triggered from Host to tell equipment that particular 
	// CEIDs are updated from Host side;

	INT len = svMsg.GetMsgLen()/sizeof(INT);

	INT *pInt;
	pInt = new INT[len];
	svMsg.GetMsg(sizeof(pInt), pInt);

	CString str, szValue;
	str.Format("Host update %d EC; ECID is: ", pInt[0]);
	for (INT nCount=1; nCount<=pInt[0]; nCount++)
	{
		str.AppendFormat(", %d",  pInt[nCount]);
	}
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t<<< S2F15 -- ") + str);


	//Retrieve CEID updated values from GemStation memory
	CString szLog;
	BOOL bEMRecallStartDateUpdated	=	FALSE;
	BOOL bEMRecallEndDateUpdated	=	FALSE;
	for (INT nCount=1; nCount<=pInt[0]; nCount++)
	{
		if (pInt[nCount] == 113)
		{
			GetGemValue(MS_SECS_SV_RECALL_STAT_START_DATE, szValue);
			szLog.Format("\t<<< S2F15 (CEID=%d) -- " + szValue, pInt[nCount]);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			bEMRecallStartDateUpdated = TRUE;
		}
		else if (pInt[nCount] == 114)
		{
			GetGemValue(MS_SECS_SV_RECALL_STAT_END_DATE, szValue);
			szLog.Format("\t<<< S2F15 (CEID=%d) -- " + szValue, pInt[nCount]);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			bEMRecallEndDateUpdated = TRUE;
		}
	}
/*
	//EquipMgr S2F15 Recall message to trigger our CEID 253 callback event
	if (bEMRecallStartDateUpdated && bEMRecallEndDateUpdated)
	{
		CString szStartDate, szEndDate;
		GetGemValue(MS_SECS_SV_RECALL_STAT_START_DATE, szStartDate);
		GetGemValue(MS_SECS_SV_RECALL_STAT_END_DATE,	szEndDate);

		RecallEquipmentStatistic(szStartDate, szEndDate);
	}
*/
	delete[] pInt;
	return 1;
}

LONG CSecsComm::SG_CmdHostSendRC(IPC_CServiceMessage& svMsg) // REMOTE_COMMAND_RECEIVE S2F21
{
	if (!IsSecsGemInit())
		return -1;

    INT len = svMsg.GetMsgLen();
    char *pCmd;
    pCmd = new char[len];
    svMsg.GetMsg(len, pCmd);
    HmiMessage(pCmd);
//****** SSSS
    delete[] pCmd;
    return 1;
}

LONG CSecsComm::SG_CmdHostDefineEvent(IPC_CServiceMessage& svMsg) // EVENT_REPORT_DEFINE S2F35
{
	if (!IsSecsGemInit())
		return -1;

	//HmiMessage("Host Define Event");
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("Host Define Event"); // 4.51D1 SesGem

	return 1;
}

LONG CSecsComm::SG_CmdHostTime(IPC_CServiceMessage& svMsg) // REQUEST_HOST_TIME S2F18
{
	if (!IsSecsGemInit())
		return -1;

	char *ptime;
	int len = svMsg.GetMsgLen();
	ptime = new char[len];
	svMsg.GetMsg(len, ptime);

	CString str;
	str.Format("Host Time is: %s", ptime);
	HmiMessage(str);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(str);

	//**** SSSS
	delete[] ptime;
	return 1;
}

LONG CSecsComm::SG_CmdHostReqOnOffLine(IPC_CServiceMessage& svMsg) // REQUEST_ON_OFF_LINE S1F15 S1F17
{
	if (!IsSecsGemInit())
		return -1;

    BOOL bOnLine = TRUE;
    svMsg.GetMsg(sizeof(BOOL), &bOnLine);

	CString szLog;
	szLog.Format("\t<<< S1F17 - [SG_CmdHostReqOnOffLine] : %d", bOnLine);  
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	m_bSecsOnline_HMI = bOnLine ? SECS_ON_LINE : SECS_OFF_LINE;
	m_bSecsOnline_HMI = !m_bSecsOnline_HMI;
	SaveData();

/*
	if( bOnLine==TRUE )
	{
		UpdateControlStatus(SECS_ON_LINE, SECS_ON_LINE_LOCAL);
	}
	else
	{
		UpdateControlStatus(SECS_OFF_LINE, SECS_HOST_OFF_LINE);
	}
*/
	return 1;
}

//================================================================
//		Defined Envent Report & Alarm Callback
//================================================================
LONG CSecsComm::SG_CmdHostDefineReport(IPC_CServiceMessage& svMsg) // DEFINE_REPORT S2F33
{
	if (!IsSecsGemInit())
		return -1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SECS: Host Define Report");
	return 1;
}

LONG CSecsComm::SG_CmdHostUpdateTime(IPC_CServiceMessage& svMsg) // DATE_TIME_CHANGE S2F31
{
	if (!IsSecsGemInit())
		return -1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SECS: Host Update System Date Time");
	return 1;
}

LONG CSecsComm::SG_CmdHostEnableAlarm(IPC_CServiceMessage& svMsg) // ENABLE_DISABLE_ALARM S5F3
{
	if (!IsSecsGemInit())
		return -1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SECS: Host Enable/Disable Alarm");
	return 1;
}

LONG CSecsComm::SG_CmdHostEnableEvent(IPC_CServiceMessage& svMsg) // ENABLE_DISABLE_EVENT S2F37
{
	if (!IsSecsGemInit())
		return -1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SECS: Host Enable/Disable Collection Event");
	return 1;
}

LONG CSecsComm::SG_CmdHostReceiveEventAck(IPC_CServiceMessage& svMsg) // EVENT_ACK_RECEIVE S6F11/F12
{
    CString str, strall;
    INT pInt[2];

    svMsg.GetMsg(sizeof(pInt), pInt);
    switch( pInt[0] )
    {
    case SECS_EVENT_ACK:
        str.Format("ACK : OK ");
        break;
    case SECS_EVENT_HOST_ACK_ERROR:
        str.Format("ACK : HOST ERROR ");
        break;
    case SECS_EVENT_DISABLE:
        str.Format("ACK : DISABLE ");
        break;
    case SECS_EVENT_OFFLINE:
        str.Format("ACK : OFFLINE ");
        break;
    case SECS_EVENT_SECS_DISABLE:
        str.Format("ACK : SECS DISABLE ");
        break;
    case SECS_EVENT_NOT_FOUND:
        str.Format("ACK : NOT FOUND ");
        break;
    }

    //strall.Format("CEID: %d, %s ", pInt[1], str);
	//HmiMessage(strall);
	strall.Format("\t<<< S6F12: %d, %s\n", pInt[0], str);	//OsramTrip 8/22
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(strall);
	return 1;
}

LONG CSecsComm::SG_CmdCommState(IPC_CServiceMessage& svMsg) // SECS_COMM_STATE 
{
	if (!IsSecsGemInit())
		return -1;

    BOOL bCommState;
    svMsg.GetMsg(sizeof(BOOL), &bCommState);

    QueryCommState();
	return 1;
}


LONG CSecsComm::SG_CmdHostSendCommand(IPC_CServiceMessage& svMsg)	// HOST_COMMAND_RECEIVE	//S2F41 // v4.51D1 Secs
{
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Enter S2F41 - [SG_CmdHostSendCommand] ");
	if (!IsSecsGemInit())
	{
		return -1;
	}

	//v4.37T3
	INT nStatus = 0;
	CString szCmd, szParam, szValue;
	CString szStatus;
	CString szMsg;
	CString szErr = "Equipment Error";

	//BOOL bLoadPKGFile;

	SHostCmmd HostCmmd;
	svMsg.GetMsg(sizeof(SHostCmmd), &HostCmmd);
	szCmd = HostCmmd.pchName; //the maximum size of command is 20
	szCmd.Replace("\n", "");
	szCmd.Replace("\r", "");
	//CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t<<< Command = ") + szCmd);

	SHostCmmdErr HostCmmdErr;
	ZeroMemory(&HostCmmdErr, sizeof(HostCmmdErr));

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szCustomerName = pApp->GetCustomerName();
	szMsg.Format("\t<<< HOST Command = (%s); Customer = (%s)", (LPCTSTR) szCmd, (LPCTSTR) szCustomerName);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szMsg);


	//OsramTrip 8/22
	INT nCtrlState		= m_pGemStation->ControlState();
	INT nCtrlSubState	= m_pGemStation->ControlSubState();
	if ( (nCtrlState != SECS_ON_LINE) /*|| (nCtrlSubState != SECS_ON_LINE_REMOTE)*/ )
	{
		HostCmmdErr.Ack = 2;		//Cannot perform
		m_pGemStation->SendHCAck(HostCmmdErr);
		szMsg.Format("S2F41 disabled; Ctrl State = %d, SubState = %d", nCtrlState, nCtrlSubState);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szMsg);
		return 1;
	}

	if (szCmd == "PP-SELECT")	//Load Process Program -> Load PKG File
	{
		szMsg.Format("\t<<< Start PP-SELECT command... HostCmmd.nParameter = %d, szParam = %s", HostCmmd.nParameter, szParam);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szMsg);
		(*m_psmfSRam)["MS896A"]["Load PKG File"] = 0;//reset bLoadPKGFile

		CString szPkgName = "";
		for (int i=0; i<HostCmmd.nParameter; i++) 
		{
			szParam = HostCmmd.arrCmmd[i].pchName;
			szParam.Replace("\n", "");
			szParam.Replace("\r", "");

			szValue = HostCmmd.arrCmmd[i].pchValue;
			szValue.Replace("\n", "");
			szValue.Replace("\r", "");

			CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t<<< S2F41 -- ") + szCmd + " (" + szValue + ")");

			if (szParam == "PPID")
			{
				szPkgName = szValue;
			}
		}

		//if (m_lEquipmentStatus == glSG_STANDBY)
		if (m_lEquipmentStatus != glSG_PRODUCTIVE)		//OsramTrip 8/22
		{
			IPC_CServiceMessage stMsg;
			IPC_CServiceMessage stMsg2;
			BOOL bReply = TRUE;
			//stMsg.InitMessage(szPkgName.GetLength(), );

			INT nMsgLength = (szPkgName.GetLength() + 1) * sizeof(CHAR);
			char* pFilename;
			pFilename = new char[nMsgLength];
			strcpy_s(pFilename, nMsgLength, szPkgName);
			
			stMsg.InitMessage(nMsgLength, pFilename);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t\t<<< -- Check PKGFile (") + szPkgName + ")");
			INT nConvID = m_comClient.SendRequest("MapSorter", "CheckGenericDeviceFile", stMsg);

			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{	
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bReply);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (bReply)
			{
				stMsg2.InitMessage(nMsgLength, pFilename);
				CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t\t<<< -- LoadPKGFile (") + szPkgName + ")");
				nConvID = m_comClient.SendRequest("MapSorter", "LoadGenericDeviceFile", stMsg2);	
				CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< -- Finished SendRequest [LoadGenericDeviceFile]");
			}
			else
			{
				CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t\t<<< -- Check PKGFile (") + szPkgName + ") FAIL");
				
				//OsramTrip 8/22
				HostCmmdErr.Ack = 3;	//1;	
				szErr = "PPID";
				strncpy_s(HostCmmdErr.arrParaErr[0].pchName, sizeof(HostCmmdErr.arrParaErr[0].pchName), szErr, szErr.GetLength());
				HostCmmdErr.arrParaErr[0].ucAck = 1;

				nStatus = 1;	
				SetErrorMessage("Fail to find SECSGEM pkg file " + szPkgName);
			}
			delete [] pFilename;
		}
		else
		{
			HostCmmdErr.Ack = 1; 
			SetErrorMessage("Fail to load SECSGEM pkg file (S2F41); Equipment not at STANDBY state");
			nStatus = 1;	
		}

		szStatus.Format("\t\t<<< -- LoadPKGFile: STATUS = %d", nStatus);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szStatus);

	}
	else if (szCmd == "MAP_CASSETTE")		//Load Wafer
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start MAP_CASSETTE command...");
		BOOL bReturn = TRUE;

		pApp->EnableScreenButton( FALSE);

		IPC_CServiceMessage stMsg;
		BOOL bReply = TRUE;
		BOOL bLoad = TRUE; // For load frame; 
		
		stMsg.InitMessage(sizeof(BOOL), &bLoad);
		INT nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "ManualChangeFilmFrame", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{	
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReply);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		pApp->EnableScreenButton( TRUE);
		szStatus.Format("\t\t<<< -- Load Wafer: STATUS = %d", nStatus);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szStatus);
	}
	else if (szCmd == "START")				//Start AUTOBOND
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start START command..."); 
		S2F41HostCmd_StartCmmd(HostCmmd, HostCmmdErr);
	}
	else if (szCmd == "STOP")				//STOP AUTOBOND
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start STOP command...");
		S2F41HostCmd_StopCmmd(HostCmmd, HostCmmdErr);
	}
	
	else if (szCmd == "FRAME_LOAD")			//v4.55A7
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start FRAME_LOAD command...");
		S2F41HostCmd_FrameLoad(&HostCmmd, &HostCmmdErr);
	}
	else if (szCmd.Find("FRAME_LOAD_COMPLETE") != -1)	//v4.57A10	
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start FRAME_LOAD_COMPLETE command...");
		S2F41HostCmd_FrameLoadComplete(&HostCmmd, &HostCmmdErr);	
	}
	else if (szCmd == "FRAME_UNLOAD")	
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start FRAME_UNLOAD command...");
		S2F41HostCmd_FrameUnload(&HostCmmd, &HostCmmdErr);	
	}
	else if (szCmd.Find("FRAME_UNLOAD_COMPLET") != -1)		
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start FRAME_UNLOAD_COMPLETE command...");
		S2F41HostCmd_FrameUnloadComplete(&HostCmmd, &HostCmmdErr);	
	}
	else if (szCmd == "CLEARBIN")			//v4.59A19	//Osram Penang	
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start CLEARBIN command...");
		S2F41HostCmd_ClearBin(HostCmmd);	
	}
	else if (szCmd == "STARTLOT")			//OsramTrip 8/22	
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start STARTLOT command...");
		if (!S2F41HostCmd_StartLot(HostCmmd))
		{
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t\t<<< -- STARTLOT fail"));
			
			HostCmmdErr.Ack = 3;
			szErr = "LOTNO is not valid";
			strncpy_s(HostCmmdErr.arrParaErr[0].pchName, sizeof(HostCmmdErr.arrParaErr[0].pchName), szErr, szErr.GetLength());
			HostCmmdErr.arrParaErr[0].ucAck = 1;
			nStatus = 1;	
			SetErrorMessage("Fail to STARTLOT (SECSGEM)");
		}
	}
	else if (szCmd.MakeUpper() == "CONFIGCYCLERPT")
	{
/*
		szMsg.Format("\t<<< S2F41 ... HostCmmd.nParameter = %d", HostCmmd.nParameter);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szMsg);
		for (int i=0; i<HostCmmd.nParameter; i++) 
		{
			szCmd = HostCmmd.arrCmmd[i].pchName;
			szCmd.Replace("\n", "");
			szCmd.Replace("\r", "");
			szValue = HostCmmd.arrCmmd[i].pchValue;
			szValue.Replace("\n", "");
			szValue.Replace("\r", "");

			CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t<<< HOST command (STD) = ") + szCmd + " (" + szValue + ")");

			CTime startTime;
			CTime lastTime = CTime::GetCurrentTime();

			//szCmd = HostCmmd.arrCmmd[i].pchName;
			if (szCmd.Find(_T("StartReportAt")) != -1)
			{
				CMSLogFileUtility::Instance()->MS_LogSECSGEM("StartReportAt");
				CString szStartDate = szValue;
				CString szYear	= "0", szMonth ="0", szDay	="0", szHour	="0", szMin	="0", szSec	="0";
				int nCol = szStartDate.Find("-");
				if( nCol!=-1 )
					szYear	= szStartDate.Left(nCol);
				int nYear		= atoi((LPCTSTR) szYear);
				szStartDate		= szStartDate.Mid(nCol + 1);
				nCol = szStartDate.Find("-");
				if( nCol!=-1 )
					szMonth = szStartDate.Left(nCol);
				int nMonth		= atoi((LPCTSTR) szMonth);
				szStartDate		= szStartDate.Mid(nCol + 1);
				nCol = szStartDate.Find(" ");
				if( nCol!=-1 )
					szDay	= szStartDate.Left(nCol);
				int nDay		= atoi((LPCTSTR) szDay);
				szStartDate		= szStartDate.Mid(nCol + 1);
				nCol = szStartDate.Find(":");
				if(nCol!=-1 )
					szHour	= szStartDate.Left(nCol);
				int nHour		= atoi((LPCTSTR) szHour);
				szStartDate		= szStartDate.Mid(nCol + 1);
				nCol = szStartDate.Find(":");
				if( nCol!=-1 )
					szMin	= szStartDate.Left(nCol);
				int nMin		= atoi((LPCTSTR) szMin);
				szStartDate		= szStartDate.Mid(nCol + 1);
				szSec	= szStartDate;
				int nSec		= atoi((LPCTSTR) szSec);

				startTime = CTime(nYear, nMonth, nDay, nHour, nMin, nSec);

				m_ulLastCyclicReportTimeInYear		= startTime.GetYear();
				m_ulLastCyclicReportTimeInMonth		= startTime.GetMonth();
				m_ulLastCyclicReportTimeInDay		= startTime.GetDay();
				m_ulLastCyclicReportTimeInHour		= startTime.GetHour();
				m_ulLastCyclicReportTimeInMinute	= startTime.GetMinute();
				SaveData();
			}
			else if (szCmd.Find(_T("IntervalInMinutes")) != -1)
			{
				INT nNewIntervalInMin	= atoi((LPCTSTR)szValue);
				szValue.Format("\t\tNew EquipMgr CE101 Interval (min) = %d", nNewIntervalInMin); 
				CMSLogFileUtility::Instance()->MS_LogSECSGEM(szValue);

				if ( (nNewIntervalInMin > 0) && (nNewIntervalInMin < 1440) )
				{
					m_ulCNotifyIntervalInMinute = nNewIntervalInMin;
					SaveData();

					SetEquipmentStatistic(startTime, lastTime, m_ulCNotifyIntervalInMinute, FALSE);
				}
			}
		}	//End FOR
*/
		HOST_ConfigCycleRptCommand(&HostCmmd, &HostCmmdErr);

	}
	else if (szCmd == "RECALLSTATISTICS")
	{
		HOST_RecallStatistcsCommand(&HostCmmd, &HostCmmdErr);
	}
	else if (szCmd == "DELETESTATISTICS")
	{
		HOST_DeleteStatistcsCommand(&HostCmmd, &HostCmmdErr);
	}

	//End ELSE

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S2F41 - End of [SG_CmdHostSendCommand]");
	
	if (!((szCmd.MakeUpper() == "CONFIGCYCLERPT") || (szCmd == "RECALLSTATISTICS") ||
		(szCmd == "DELETESTATISTICS") ||
		(szCmd.MakeUpper() == "FRAME_LOAD") || (szCmd.MakeUpper() == "FRAME_UNLOAD") ||
		(szCmd.MakeUpper() == "FRAME_LOAD_COMPLETE") || (szCmd.MakeUpper() == "FRAME_UNLOAD_COMPLETE")))
	{
		//HostCmmdErr.Ack = 0;						//OsramTrip 8/22
		HostCmmdErr.nErr = nStatus;		//= 0;		//v4.51A12	
		if (nStatus == 0)							//OsramTrip 8/22
		{
			strncpy_s(HostCmmdErr.arrParaErr[nStatus].pchName, sizeof(HostCmmdErr.arrParaErr[nStatus].pchName), szErr, 20);
		}
	}

	m_pGemStation->SendHCAck(HostCmmdErr);
	return 1;
}


LONG CSecsComm::SG_CmdHostReceiveAlarmAck(IPC_CServiceMessage &svMsg) // ALARM_ACK_RECEIVE S5F2
{
    INT pAlarm[2];
    svMsg.GetMsg(sizeof(pAlarm), pAlarm);
    CString str, strall;
    switch( pAlarm[0] )
    {
    case SECS_ALARM_ACK:
        str.Format(" Ack: OK");
        break;
    case SECS_ALARM_HOST_ACK_ERROR:
        str.Format(" Ack: Host Error");
        break;
    case SECS_ALARM_DISABLE:
        str.Format(" Ack: DISABLE");
        break;
    case SECS_ALARM_OFFLINE:
        str.Format(" Ack: OFFLINE");
        break;
    case SECS_ALARM_SECS_DISABLE:
        str.Format(" Ack: SECS DISABLE");
        break;
    case SECS_ALARM_NOT_FOUND:
        str.Format(" Ack: NOT FOUND");
        break;
    case SECS_ALARM_NOT_SET:
        str.Format(" Ack: NOT SET");
        break;
    }

    strall.Format("Alarm ID: %d, %s", pAlarm[1], str);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(strall);
    return 1;
}

LONG CSecsComm::SG_CmdHostReqEvent(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return -1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SrvSgHostReqEvent");
	return 1;
}

LONG CSecsComm::SG_CmdReceiveEnableAlarmAck(IPC_CServiceMessage& svMsg)
{
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SEcsComm::SG_CmdReceiveEnableAlarmAck");	
    return 1;
}

LONG CSecsComm::SG_CmdReceiveEnableEventAck(IPC_CServiceMessage& svMsg)
{
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SEcsComm::SG_CmdReceiveEnableEventAck");	
    return 1;
}

LONG CSecsComm::SG_CmdReceiveHostRequestReport(IPC_CServiceMessage& svMsg)
{
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SEcsComm::SG_CmdReceiveHostRequestReport");	
    return 1;
}

LONG CSecsComm::SG_CmdSpoolActivate(IPC_CServiceMessage& svMsg)
{
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SEcsComm::SG_CmdSpoolActivate");	
    return 1;
}

LONG CSecsComm::SG_CmdSpoolDeactivate(IPC_CServiceMessage& svMsg)
{
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SEcsComm::SG_CmdSpoolDeactivate");	
    return 1;
}

LONG CSecsComm::SG_CmdLoadUnloadPortStatusQy(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S1F77");	
    return 1;
}

LONG CSecsComm::SG_CmdRequestOnlineCom(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S1F79");	
    return 1;
}

LONG CSecsComm::SG_CmdOnlineSumStatusRpReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S1F70");	
    return 1;
}

LONG CSecsComm::SG_CmdCasLoadCompleteRpReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S1F74");	
    return 1;
}

LONG CSecsComm::SG_CmdCasUnloadReqComRpReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S1F76");	
    return 1;
}

LONG CSecsComm::SG_CmdEquStatusChangeRpReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;

	UpdateLextarGemData(FALSE);

#ifndef MS50_64BIT
	if (m_stLexter.S1F66_GrantCode == 0)
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S1F66 (OK)");	
	else
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S1F66 (FAIL)");	
#endif
	return 1;
}

LONG CSecsComm::SG_CmdLotProcessDataTransfer(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S7F65");	
    return 1;
}

LONG CSecsComm::SG_CmdLotStatusChangeRpReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S1F68");	
	return 1;
}

LONG CSecsComm::SG_CmdWaferProcessDataRpReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S6F66");	
    return 1;
}

LONG CSecsComm::SG_CmdLotProcessDataRpReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S6F70");	
    return 1;
}

LONG CSecsComm::SG_CmdAlarmOccRemRpReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;

	UpdateLextarGemData(FALSE);

#ifndef MS50_64BIT
	if (m_stLexter.S5F66_AcknowledgeCode == 0)
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S5F66 (OK)");	
	else if (m_stLexter.S5F66_AcknowledgeCode == 0x4)
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S5F66 (OFFLINE)");	
	else
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S5F66 (FAIL)");	
#endif
    return 1;
}

LONG CSecsComm::SG_CmdReplyLotProcessDataReq(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S7F72");	
    return 1;
}

LONG CSecsComm::SG_CmdLotCancelReqReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S7F74");	
    return 1;
}

LONG CSecsComm::SG_CmdOnlineReply(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return 1;
	if (!m_bEnableLextarSecsGem)
		return 1;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S1F2");	
    return 1;
}


LONG CSecsComm::SC_CmdSendTerminalMsg(IPC_CServiceMessage& svMsg)
{
    if( IsSecsGemInit()==FALSE )
        return -1;

    long len;
    char *szMsg;
    len = svMsg.GetMsgLen();
    szMsg = new char[len];
    svMsg.GetMsg(len, szMsg);
    CString str;
    str.Format("%s", szMsg);
    m_pGemStation->SendTerminalMsg(str, 1); // S10F1
//******* SSSS
    delete[] szMsg;

    return 1;
}

LONG CSecsComm::SG_CmdHostSendTerminalMsg(IPC_CServiceMessage& svMsg) // TERMINAL_SERVICE_RECEIVE S10F3
{
    if( IsSecsGemInit()==FALSE )
        return -1;
    UCHAR ucTID;
    INT len = svMsg.GetMsgLen();
    char *pstr;
    pstr = new char[len];
    svMsg.GetMsg(len, pstr);
    CString str;
    memcpy(&ucTID, pstr, sizeof(ucTID));
    str.Format("terminal %d, %s", ucTID, pstr+sizeof(ucTID));

//***********SSSS
	delete[] pstr;

    if( IsSecsGemInit()==FALSE )
        return -1;
    m_pGemStation->SendTerminalMsgAck(ACK10); // S10F4
    HmiMessage(str);
	// Terminal Service Acknowledge
//	ACK10						=	0x0;
// 	ACK10_MSG_WILL_NOT_DISPLAY	=	0x01;
//	ACK10_TERMINAL_NOT_AVAIL	=	0x02;

	return 1;
}

LONG CSecsComm::SG_CmdHostSendTerminalMsg_MB(IPC_CServiceMessage& svMsg) // TERMINAL_MULTI_BLK_MSG_RECEIVE S10F5
{
    if( IsSecsGemInit()==FALSE )
        return -1;

	INT nIndex, nMsgLen, nSizeINT, nSizeTCHAR;
	CString szMsg, szTID;
	UCHAR TID;
	CHAR* pBuffer = NULL;

	nSizeINT = sizeof(INT);
	nSizeTCHAR = sizeof(TCHAR);

	try
	{
		if((nMsgLen = svMsg.GetMsgLen()) > 0)
		{
			m_nDisplayPageNum = 0;
			m_stTMsgTextList.RemoveAll();
			nIndex = 0;
			pBuffer = new CHAR[nMsgLen];
			svMsg.GetMsg(nMsgLen, pBuffer);

			TID = *((UCHAR*)(pBuffer));
			nIndex += nSizeTCHAR;

			int nTextLen = StringLength(pBuffer+nIndex);
			while (nMsgLen > nIndex)
			{
				szMsg = ("");
				szMsg += CharToString(pBuffer+nIndex, nTextLen/(sizeof(TCHAR)));
				nIndex += (nTextLen + sizeof(TCHAR));
				nTextLen = StringLength(pBuffer+nIndex);

				m_stTMsgTextList.AddTail(szMsg);			
			}

            HmiSelection("Host Multi Block Message", "SECS GEM", m_stTMsgTextList);

			// m_nTMsgTextCnt = m_stTMsgTextList.GetCount();
			// if(m_nTMsgTextCnt > 10)
			{
			//	m_bTMsgNextPage = TRUE;
			}
			// SetDisplayText(m_nDisplayPageNum);
			// SwitchHMIPage();	//Switch to Terminal Message Page.
		}
		if(pBuffer != NULL)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		if(pBuffer != NULL)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
	}
    // Call this function when the terminal can not handle a multiblock message.
    // if can not handle multi block message
//    m_pGemStation->SendMBMsgNotAllow(); // S10F7
    m_pGemStation->SendMultiBlockMsgAck(ACK10); // S10F6

	return 1;
}

LONG CSecsComm::SG_CmdTerminalMsgAck(IPC_CServiceMessage& svMsg)
{
	if (!IsSecsGemInit())
		return -1;
	return 1;
}

LONG CSecsComm::SC_CmdGetCString(IPC_CServiceMessage& svMsg)
{
	BOOL bResult = TRUE;
    char *TempString;
    long len = svMsg.GetMsgLen();
    TempString = new char[len];
    svMsg.GetMsg(len, TempString);
    m_szSCDownMapFileName.Format("%s", TempString);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("CmdGetCString:" + m_szSCDownMapFileName);	
//************SSSS
    delete[] TempString;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
    return 1;
}

LONG CSecsComm::SC_CmdDownLoadMap(IPC_CServiceMessage& svMsg)			
{
	//This fcn is called by our MS program to auto-retrieve map from HOST to
	//MS; used in AUTOBOND mode under FULLY-AUTO mode by WL station;

    if( !IsSecsGemInit() )
        return -1;

	BOOL bResult = TRUE;

	char *pBuffer;
	CString szWaferBarCode;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szWaferBarCode = &pBuffer[0];
	delete [] pBuffer;

	if ( szWaferBarCode.GetLength() <= 0 )
    {
		HmiMessage("SECS: invalid barcode in SC_CmdDownLoadMap");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
    }

	CMS896AStn::m_bSECSWaferMapLoadFinish	= FALSE;

	UCHAR ucRotation = (*m_psmfSRam)["MS896A"]["Map Rotation"];

	m_szSCDownMapFileName = szWaferBarCode;
	m_lMapType = 2;

	// Map setup S12F3
	CSecsWaferMapData MapData;
    MapData.SetMaterialID(m_szSCDownMapFileName);
    MapData.SetIDType(0);
	MapData.SetMapDataFormat(1);
	MapData.SetOrigLoc(2);
	MapData.SetNullBinCode(".");
	MapData.SetBinCodeEquiv("");

	//Map rotation always 0 degree requested from HOST; actual rotation is performed
	// by MS instead; this is transparent to HOST;
	USHORT usRotate = 0;
	MapData.SetFlatNotchLoc(usRotate);

	CString szLog;
	szLog.Format("\t<<< S12F3 -- SendMapDataReq (AUTOBOND): TYPE = %d, map = %s", m_lMapType, m_szSCDownMapFileName);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

    m_pGemStation->SendMapDataReq(&MapData);	//S12F3

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}


LONG CSecsComm::SC_CmdDownLoadMapTest(IPC_CServiceMessage &svMsg)
{
	//THis fcn is available in SECSGEM menu (Download map Test button)
	//to retrieve map from HOST to MS and put it into local HD; the map is
	//also loaded on HMI wafermap display;

    if( !IsSecsGemInit() )
        return -1;

	BOOL bResult = TRUE;
    LONG lMapType;
	CSecsWaferMapData MapData;

    svMsg.GetMsg(sizeof(LONG), &lMapType);

    if( lMapType<1 || lMapType>3 )
    {
		HmiMessage("SECS: Wrong map type (1-3 only)");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
    }

	m_lMapType = lMapType;
	UCHAR ucRotation = (*m_psmfSRam)["MS896A"]["Map Rotation"];

	// Map setup S12F3
    MapData.SetMaterialID(m_szSCDownMapFileName);
    MapData.SetIDType(0);
	MapData.SetMapDataFormat(1);
	MapData.SetOrigLoc(2);
	MapData.SetNullBinCode(".");
	MapData.SetBinCodeEquiv("");

	//Map rotation always 0 degree requested from HOST; actual rotation is performed
	// by MS instead; this is transparent to HOST;
	USHORT usRotate = 0;	//0 degree; may also have 90, 180, 270
	MapData.SetFlatNotchLoc(usRotate);		

	CString szLog;
	szLog.Format("\t<<< S12F3 -- SendMapDataReq: TYPE = %d, map = %s", lMapType, m_szSCDownMapFileName);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

    m_pGemStation->SendMapDataReq(&MapData);	//S12F3

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CSecsComm::SG_CmdMapSetupData(IPC_CServiceMessage& svMsg)			//S12F4
{
	CString szLog;
	CSecsWaferMapData MapData;
	unsigned long ulLen = svMsg.GetMsgLen();

	if (ulLen <= 0)
	{
		return 0;
	}

	szLog.Format("2 -> size = %d", ulLen);

	unsigned char* pBuffer = new unsigned char[ulLen];
	int nIndex = 0;

	svMsg.GetMsg(ulLen, pBuffer);
	memcpy(&MapData, pBuffer, sizeof(CSecsWaferMapData));
	nIndex += sizeof(CSecsWaferMapData);

	if (m_arrRefDie != NULL)
	{
		//delete[] m_arrRefDie;
	}
	int i=0;

	CString szMID;
	MapData.MaterialID(szMID);

	if (!szMID.IsEmpty())
	{
		if (ulLen >= nIndex+sizeof(INT))
		{
			m_nNumberOfMapRefDie = *(INT *)(pBuffer + nIndex);
			nIndex += sizeof(INT);

			if (m_nNumberOfMapRefDie > 0)
			{
				m_arrRefDie = new WAF_CMapDieInformation [m_nNumberOfMapRefDie];

				for (i=0; i<m_nNumberOfMapRefDie; i++) 
				{
					CSecsMapCoord RefCoord;
					RefCoord = *(CSecsMapCoord *)(pBuffer + nIndex);
					nIndex += sizeof(CSecsMapCoord);

					m_arrRefDie[i].SetRow(RefCoord.PointY());
					m_arrRefDie[i].SetColumn(RefCoord.PointX());
				}
			}
		}

		m_ucNullBin				= 46;	//MapData.NullBinCode();
		m_ulType2MapRowCount	= MapData.RowCountInc();
		m_ulType2MapColCount	= MapData.ColCountInc();

		CSecsMapID MapID;
		MapID.SetMaterialID(m_szSCDownMapFileName);

		switch ( m_lMapType )
		{
		case 1:
			szLog.Format("\t<<< S12F13 -- RequestMapDataType1, map = %s", m_szSCDownMapFileName);
			m_pGemStation->RequestMapDataType1(&MapID, IDTYP_WAFER_ID);		//S12F13
			break;

		case 2:
			szLog.Format("\t<<< S12F15 -- RequestMapDataType2, map = %s, NullBin = %d, ROW = %lu, COL = %lu REF = %ld", 
				m_szSCDownMapFileName, m_ucNullBin, 
				m_ulType2MapRowCount, m_ulType2MapColCount, m_nNumberOfMapRefDie);
			m_pGemStation->RequestMapDataType2(&MapID, 0);					//S12F15
			break;

		case 3:
			szLog.Format("\t<<< S12F17 -- RequestMapDataType3, map = %s", m_szSCDownMapFileName);
			m_pGemStation->RequestMapDataType3(&MapID, IDTYP_WAFER_ID, 0);	//S12F17
			break;
		}

		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	}
	else
	{
		szLog = "\t<<<< S12F4: no map to download";
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	}

	delete [] pBuffer;
	return 0;
}

LONG CSecsComm::SG_CmdDownLoadType1Map(IPC_CServiceMessage& svMsg) // s12f13
{
	char* strTemp;
	int nLen = svMsg.GetMsgLen();

	strTemp = new char[nLen];
	svMsg.GetMsg(nLen, strTemp);

      m_WaferMapWrapper.InterpretMap("Secs12Type1", m_szSCDownMapFileName, strTemp, nLen, 46, 19, 19); 

	  HmiMessage("down load type1 map to " + m_szSCDownMapFileName);
//**********SSSS
      delete[] strTemp;
	  m_szWaferId		   = m_szSCDownMapFileName;
      return 1;
}

LONG CSecsComm::SG_CmdDownLoadType2Map(IPC_CServiceMessage& svMsg)	// s12f15	//S12F16
{
	char* strTemp;

	int nLen = svMsg.GetMsgLen();
	strTemp = new char[nLen];
	svMsg.GetMsg(nLen, strTemp);

	//TYPE2 map currently used by Renesas MS90	//v4.59A13

	CString szMapPath = (*m_psmfSRam)["MS896A"]["MapFilePath"];
	CString szFullPath = szMapPath + "\\" + m_szSCDownMapFileName;

	CString szLog;
	szLog.Format("t<<< S12F16 -- InterpretMap, map = %s, LENGTH = %d, NULL = %d, ROW = %lu, COL = %lu, REF = %d", 
					szFullPath, nLen, m_ucNullBin, 
					m_ulType2MapRowCount, m_ulType2MapColCount, m_nNumberOfMapRefDie);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	m_WaferMapWrapper.InterpretMap("Secs12Type2", szFullPath, strTemp, nLen, 
		m_ucNullBin, m_ulType2MapRowCount, m_ulType2MapColCount, 
		m_arrRefDie, m_nNumberOfMapRefDie); 

	m_WaferMapWrapper.SetGradeOffset(48);
	m_WaferMapWrapper.SetFlatAngle(0);

	delete[] strTemp;

	CMS896AStn::m_bSECSWaferMapLoadFinish	= TRUE;
	m_szWaferId		   = m_szSCDownMapFileName;
	//HmiMessage("SECS: download TYPE2 map to: " + szFullPath);
	return 0;
}

LONG CSecsComm::SG_CmdDownLoadType3Map(IPC_CServiceMessage& svMsg) // s12f17
{
	char* strTemp;
	int nLen = svMsg.GetMsgLen();

	strTemp = new char[nLen];
	svMsg.GetMsg(nLen, strTemp);

	m_WaferMapWrapper.InterpretMap("Secs12Type3", m_szSCDownMapFileName, strTemp, nLen, 46, 19, 19); 

	HmiMessage("down load type3 map to "+m_szSCDownMapFileName);
	//******SSSS
	delete[] strTemp;
	m_szWaferId		   = m_szSCDownMapFileName;
	return 0;
}

LONG CSecsComm::SC_CmdSendBackupMap(IPC_CServiceMessage& svMsg)
{
	//This fcn is used by MS to upload backup map file from MS to
	//HOST
	BOOL bResult = TRUE;
	CString szLog;

    if( !IsSecsGemInit() )
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	m_szSCDownMapFileName = m_szWaferId;

	CSecsWaferMapData MapData;
	MapData.SetMaterialID(m_szSCDownMapFileName);
	MapData.SetIDType(0);
	CSecsMapCoordList RefCoordList;

	if (m_WaferMapWrapper.GetReader() != NULL) 
	{
		CMS896AStn::m_bSECSWaferMapUpLoadAbort	= FALSE;
		CMS896AStn::m_bSECSWaferMapUpLoadFinish = FALSE;

		unsigned long ulNumRef = m_WaferMapWrapper.GetNumberOfReferenceDice();
		WAF_CMapDieInformation *aDieInformation = new WAF_CMapDieInformation[ulNumRef];

		UCHAR ucRotation = (*m_psmfSRam)["MS896A"]["Map Rotation"];
		USHORT usRotate = 0;
		if (ucRotation == 1)
			usRotate = 90;
		else if (ucRotation == 2)
			usRotate = 180;
		else if (ucRotation == 3)
			usRotate = 270;
		m_WaferMapWrapper.GetStdOrientRefDieList(aDieInformation, ulNumRef, 360 - usRotate);


		unsigned long i=0; 

		for (i = 0; i < ulNumRef; i++) 
		{
			CSecsMapCoord RefCoord;
			RefCoord.SetPoint(aDieInformation[i].GetRow(), aDieInformation[i].GetColumn());
			RefCoordList.AddTail(&RefCoord);
		}

		delete[] aDieInformation;

		unsigned short usFlatAngle = 0;
		unsigned long ulRowCount;
		unsigned long ulColCount;

		m_WaferMapWrapper.GetStdOrientMapInfo(usFlatAngle, ulRowCount, ulColCount);

		MapData.SetFlatNotchLoc(0);
		MapData.SetMapDataFormat(1);
		MapData.SetOrigLoc(2);
		MapData.SetNullBinCode(".");
		MapData.SetBinCodeEquiv("");
		MapData.SetRowCountInc(ulRowCount);
		MapData.SetColCountInc(ulColCount);
		MapData.SetProcDieCount(m_WaferMapWrapper.GetSelectedTotalDice());

		m_pGemStation->MapSetUpDataSend(&MapData, &RefCoordList);		//S12F3

		szLog.Format("SC_CmdSendBackupMap Done - MAP = %s", m_szSCDownMapFileName);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	}
	else
	{
		CMS896AStn::m_bSECSWaferMapUpLoadFinish = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CSecsComm::SG_CmdMapDataAck(IPC_CServiceMessage& svMsg)
{
	ULONG ulRowCount=0, ulColCount=0;
	INT i=0, j=0;

	if (m_WaferMapWrapper.GetReader() != NULL) 
	{
		CSecsMapCoordData CoordData;
		
		CString szCoordData;
		m_WaferMapWrapper.GetStdOrientMapContent(szCoordData);
		CoordData.SetBinCode(szCoordData);

		CSecsMapID MapID;
		MapID.SetMaterialID(m_szSCDownMapFileName);
		m_pGemStation->SendMapDataType2(&MapID, 0, &CoordData);		//S12F15

		//CMS896AStn::m_bSECSWaferMapUpLoadFinish = TRUE;
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("SG_CmdMapDataAck Done");
	}

	return 0;
}

LONG CSecsComm::SG_CmdMapDataAck2(IPC_CServiceMessage& svMsg)
{
	UCHAR ucAck		= 0;
	USHORT usTemp	= 0;

	//svMsg.GetMsg(sizeof(UCHAR), &ucAck);
	svMsg.GetMsg(sizeof(USHORT), &usTemp);
	ucAck = (UCHAR)(usTemp & 0xFF);

	if (ucAck == 0)
	{
		CMS896AStn::m_bSECSWaferMapUpLoadFinish = TRUE;
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("SG_CmdMapDataAck2 - Done");
	}
	else
	{
		CMS896AStn::m_bSECSWaferMapUpLoadFinish = FALSE;
		CMS896AStn::m_bSECSWaferMapUpLoadAbort	= TRUE;

		CString szLog;
		szLog.Format("SG_CmdMapDataAck2 - fail in UploadMap = %d", ucAck);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		SetAlert_Red_Yellow(IDS_SEC_WL_UPLOADMAP_ACK2_ERR);
		//HmiMessage_Red_Yellow("SG_CmdMapDataAck2 Fail");	//IDS_SEC_WL_UPLOADMAP_ACK2_ERR
	}

	return 0;
}

LONG CSecsComm::SG_CmdDownloadPackage(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szPackageFile;

	char *pBuffer;
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);
	szPackageFile = &pBuffer[0];
	delete [] pBuffer;

    if( !IsSecsGemInit() )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("DOWNLOAD PKG: " + szPackageFile);	
	DownloadPkgFile(szPackageFile);				//Download package file to equipment

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CSecsComm::SG_CmdUploadPackage(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szPackageFile;

	char *pBuffer;
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);
	szPackageFile = &pBuffer[0];
	delete [] pBuffer;

    if( !IsSecsGemInit() )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

//	if (!IsSecsGemEnabled())	//v4.59A32
//	{
//		svMsg.InitMessage(sizeof(BOOL), &bReturn);
//		return 1;
//	}


	CMSLogFileUtility::Instance()->MS_LogSECSGEM("UPLOAD PKG: " + szPackageFile);	
//	if (IsMSAutoLineMode())		//v4.59A34
		UploadPkgFile(szPackageFile, FALSE);		//UPload package file to Host
//	else
//		UploadPkgFile(szPackageFile, TRUE);			//UPload package file to Host upon host requries

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CSecsComm::Test(IPC_CServiceMessage& svMsg)  // 4.51D1 Secs
{
	BOOL bReturn = TRUE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	//HmiMessage("SECSGEM: Test start ....");



				//stMsg.InitMessage(szPkgName.GetLength(), );
		//CString szPkgName = "dummy";
		//INT nMsgLength = (szPkgName.GetLength() + 1) * sizeof(CHAR);
		//char* pFilename;
		//pFilename = new char[nMsgLength];
		//strcpy_s(pFilename, nMsgLength, szPkgName);
		//stMsg.InitMessage(nMsgLength, pFilename);
		//INT nConvID = m_comClient.SendRequest("MapSorter", "LoadGenericDeviceFile", stMsg);
		//		//if (pApp->LoadPKGFile(FALSE, szPkgName, FALSE, FALSE, TRUE, FALSE) == FALSE)
		//		//{
		//		//	SetAlert(IDS_WL_LOAD_PKG_FILE_FAIL);
		//		//	SetErrorMessage("Fail to load pkg file");
		//			//nStatus = 1;
		//		//}

		


	//////////////////START command
	//CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start START command..."); 
	//INT nStatus = 0;
	//BOOL bLoadPKGFile;
	//bLoadPKGFile = (BOOL)(LONG)((*m_psmfSRam)["MS896A"]["Load PKG File"]);
	//if(pApp->m_bInBondPage == TRUE )		
	//{
	//	if(bLoadPKGFile == TRUE)
	//	{

	//		pApp->m_bCycleStarted = TRUE;
	//		CMS896AStn::m_bWaferAlignComplete = TRUE;
	//		

	//		IPC_CServiceMessage stMsg;
	//		BOOL bReply = TRUE;


	//		INT nConvID = m_comClient.SendRequest("MapSorter", "PreStartCycle", stMsg);
	//		while (1)
	//		{
	//			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
	//			{	
	//				m_comClient.ReadReplyForConvID(nConvID, stMsg);
	//				stMsg.GetMsg(sizeof(BOOL), &bReply);
	//				break;
	//			}
	//			else
	//			{
	//				Sleep(10);
	//			}
	//		}
	//		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< Finished PreStartCycle()" );

	//		nConvID = m_comClient.SendRequest("MapSorter", "Demonstration", stMsg);
	//		CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< Finished Demonstration()" );
	//		HmiMessage("SECSGEM: Test is DONE");
	//	}
	//	else
	//	{
	//		nStatus = 1;
	//		HmiMessage("Cannot use Start remote command as Loading the Package file");
	//	}
	//}
	//else
	//{
	//	//HostCmmdErr.Ack = 1;  // Send the alarm to server
	//	//SetErrorMessage("Fail to push button (SECSGEM S2F41)");
	//	nStatus = 1;
	//	HmiMessage("Please go back to Bond Page first."); 
	//}
	//
	//CString szStatus;
	//szStatus.Format("\t\t<<< -- Load Start Button: m_bInBondPage = %d, bLoadPKGFile = %d, STATUS = %d", pApp->m_bInBondPage, bLoadPKGFile, nStatus);
	//CMSLogFileUtility::Instance()->MS_LogSECSGEM(szStatus);
	///////////////////////////end of START Command

	/////// MAP_CASSETTE command (load wafer)
	//INT nStatus = 0;
	//CString szStatus;

	//CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start MAP_CASSETTE command...");
	////BOOL bReturn = TRUE;

	//pApp->EnableScreenButton( FALSE);

	//IPC_CServiceMessage stMsg;
	//BOOL bReply = TRUE;
	//BOOL bLoad = TRUE; // For load frame; 
	//stMsg.InitMessage(sizeof(BOOL), &bLoad);
	//INT nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "ManualChangeFilmFrame", stMsg);
	//while (1)
	//{
	//	if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
	//	{	
	//		m_comClient.ReadReplyForConvID(nConvID, stMsg);
	//		stMsg.GetMsg(sizeof(BOOL), &bReply);
	//		break;
	//	}
	//	else
	//	{
	//		Sleep(10);
	//	}
	//}
	//pApp->EnableScreenButton( TRUE);
	//szStatus.Format("\t\t<<< -- Load Wafer: STATUS = %d", nStatus);
	//CMSLogFileUtility::Instance()->MS_LogSECSGEM(szStatus);
	//////////// End of  MAP_CASSETTE command

	//////////// Stop Command
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start STOP command...");
	
	INT nStatus = 0;
	CString szStatus;
	IPC_CServiceMessage stMsg;

	INT nConvID = m_comClient.SendRequest(SAFETY_STN, "PressStopButton", stMsg);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< Finished SafetyStn - PressStopButton()" );
	
	nConvID = m_comClient.SendRequest("MapSorter", "Stop", stMsg);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< Finished MapSorter - Stop()" );
	
	LONG lLevel = 2; 
	stMsg.InitMessage(sizeof(LONG), &lLevel);
	nConvID = m_comClient.SendRequest(SAFETY_STN, "SetAlarmLamp", stMsg);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< Finished SafetyStn - SetAlarmLamp()" );
	szStatus.Format("\t\t<<< -- STOP: STATUS = %d", nStatus);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szStatus);
	///////////End of Stop Command

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CSecsComm::TestSecs(IPC_CServiceMessage& svMsg)  // 4.51D1 Secs
{
	////////////////START command
	BOOL bReturn = TRUE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< Start START command..."); 
	INT nStatus = 0;
	BOOL bLoadPKGFile;
	bLoadPKGFile = (BOOL)(LONG)((*m_psmfSRam)["MS896A"]["Load PKG File"]);
	if(pApp->m_bInBondPage == TRUE )		
	{
		if(bLoadPKGFile == TRUE)
		{
			pApp->m_bCycleStarted = TRUE;
			CMS896AStn::m_bWaferAlignComplete = TRUE;

			IPC_CServiceMessage stMsg;
			BOOL bReply = TRUE;

			INT nConvID = m_comClient.SendRequest("MapSorter", "PreStartCycle", stMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{	
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bReply);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< Finished PreStartCycle()" );

			nConvID = m_comClient.SendRequest("MapSorter", "Demonstration", stMsg);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< Finished Demonstration()" );
			HmiMessage("SECSGEM: Test is DONE");
		}
		else
		{
			nStatus = 1;
			HmiMessage("Cannot use Start remote command as Loading the Package file");
		}
	}
	else
	{
		//HostCmmdErr.Ack = 1;  // Send the alarm to server
		//SetErrorMessage("Fail to push button (SECSGEM S2F41)");
		nStatus = 1;
		HmiMessage("Please go back to Bond Page first."); 
	}
	
	CString szStatus;
	szStatus.Format("\t\t<<< -- Load Start Button: m_bInBondPage = %d, bLoadPKGFile = %d, STATUS = %d", pApp->m_bInBondPage, bLoadPKGFile, nStatus);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szStatus);
	/////////////////////////end of START Command

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//===========================================================================
//  Update SVID, ECID etc
//===========================================================================
LONG CSecsComm::UpdateSGVariables()
{
	if (!IsSecsGemInit()) 
	{
		return 1;
	}

	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	pAppMod->UpdateAllSGVariables();
	return 1;
}

LONG CSecsComm::SG_CmdHostRecSelectEquipmentStatus(IPC_CServiceMessage &svMsg)
{
	return UpdateSGVariables();
}

LONG CSecsComm::SG_CmdHostRecSelectEquipmentConst(IPC_CServiceMessage &svMsg)
{
	return UpdateSGVariables();
}

LONG CSecsComm::SG_CmdHostRecReqStatusNameList(IPC_CServiceMessage &svMsg)
{
	return UpdateSGVariables();
}

LONG CSecsComm::SG_CmdHostRecReqEquipmentConstNameList(IPC_CServiceMessage &svMsg)
{
//	return UpdateSGVariables();
	return 1;
}

LONG CSecsComm::SG_CmdHostRecReqEquipmentConst(IPC_CServiceMessage &svMsg)
{
//	return UpdateSGVariables();
	return 1;
}


VOID CSecsComm::UpdateAllSGVariables()
{
	if (!IsSecsGemInit()) 
	{
		return;
	}
//	SetGemValue(MS_SECS_SV_CONTROL_MODE_STATUS, m_hSecsCtrlMode);
	
	UpdateEquipmentBasicStatistics();
	try
	{
		SetGemValue(MS_SECS_SV_E10_STATE,	m_lEquipState);
	}
	catch(...)
	{
		DisplaySequence("CSecsComm::UpdateAllSGVariables failure");
	}

}

BOOL CSecsComm::UpdateSGEquipmentConst(INT nECID)
{
	BOOL bRet = TRUE;
	if (!IsSecsGemInit()) 
	{
		return bRet;
	}
	return TRUE;
}

