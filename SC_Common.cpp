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
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL CSecsComm::LoadData(VOID)
{
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

    // open config file
    if (pUtl->LoadSGConfig() == FALSE)
		return FALSE;

    // get file pointer
    psmf = pUtl->GetSGConfigFile();
    
	//Check Load/Save Data
	if( psmf == NULL )
	{
        return FALSE;
	}

    // get data
	m_szHostIPAddress       = (*psmf)[SG_DATA][SG_HOSTIP];
	m_bAutoStart            = (*psmf)[SG_DATA][SG_AUTOSTART];
	m_ulHostPort            = (*psmf)[SG_DATA][SG_COMMPORT];
	m_szEquipId				= (*psmf)[SG_DATA][SG_EQUIP_ID];	//aaa111
    m_usHsmsConnectMode     = (_TINT)(LONG)((*psmf)[SG_DATA][SG_CONNMODE]);
	m_ulConverTimer         = (*psmf)[SG_DATA][SG_CONVTIME];
	m_ulHsmsT3Timeout       = (*psmf)[SG_DATA][SG_HSMST3]; // reply
	m_ulHsmsT5Timeout       = (*psmf)[SG_DATA][SG_HSMST5];
	m_ulHsmsT6Timeout       = (*psmf)[SG_DATA][SG_HSMST6];
	m_ulHsmsT7Timeout       = (*psmf)[SG_DATA][SG_HSMST7];
	m_ulHsmsT8Timeout       = (*psmf)[SG_DATA][SG_HSMST8];
	m_ulHsmsLinkTestTimer   = (*psmf)[SG_DATA][SG_HSMSLT];

	m_bSecsOnline			= (BOOL)(LONG)((*psmf)[SG_DATA][SG_SECSONLINE]);
	m_hSecsCtrlMode			= (SHORT)(LONG)((*psmf)[SG_DATA][SG_CTRLMODE]);
	m_bSecsOnline_HMI		= !m_bSecsOnline;

	//v4.37T3	//EquipManager	S2F41
	m_ulCNotifyIntervalInMinute			= (*psmf)[SG_DATA][SG_CYCLIC_REPORT_NOTIFY_INTERVAL];
	m_ulLastCyclicReportTimeInHour		= (*psmf)[SG_DATA][SG_CYCLIC_REPORT_TIME_HOUR];
	m_ulLastCyclicReportTimeInMinute	= (*psmf)[SG_DATA][SG_CYCLIC_REPORT_TIME_MINUTE];

    // close config file
    pUtl->CloseSGConfig();

    return TRUE;
}

BOOL CSecsComm::SaveData(VOID)
{
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

    // open config file
    if (pUtl->LoadSGConfig() == FALSE)
		return FALSE;

    // get file pointer
	psmf = pUtl->GetSGConfigFile();

	//Check Load/Save Data
    if( psmf==NULL )
	{
        return FALSE;
	}

    // save data
    (*psmf)[SG_DATA][SG_HOSTIP]     = m_szHostIPAddress;
    (*psmf)[SG_DATA][SG_AUTOSTART]  = m_bAutoStart;
	(*psmf)[SG_DATA][SG_EQUIP_ID]	= m_szEquipId;		//aaa111
    (*psmf)[SG_DATA][SG_COMMPORT]   = m_ulHostPort;
    (*psmf)[SG_DATA][SG_CONNMODE]   = m_usHsmsConnectMode;
    (*psmf)[SG_DATA][SG_CONVTIME]   = m_ulConverTimer;
    (*psmf)[SG_DATA][SG_HSMST3]     = m_ulHsmsT3Timeout;
    (*psmf)[SG_DATA][SG_HSMST5]     = m_ulHsmsT5Timeout;
    (*psmf)[SG_DATA][SG_HSMST6]     = m_ulHsmsT6Timeout;
    (*psmf)[SG_DATA][SG_HSMST7]     = m_ulHsmsT7Timeout;
    (*psmf)[SG_DATA][SG_HSMST8]     = m_ulHsmsT8Timeout;
    (*psmf)[SG_DATA][SG_HSMSLT]     = m_ulHsmsLinkTestTimer;

	(*psmf)[SG_DATA][SG_SECSONLINE]	= (LONG)m_bSecsOnline;
	(*psmf)[SG_DATA][SG_CTRLMODE]	= (LONG)m_hSecsCtrlMode;

	//v4.37T3	//EquipManager	S2F41
	(*psmf)[SG_DATA][SG_CYCLIC_REPORT_NOTIFY_INTERVAL]	= m_ulCNotifyIntervalInMinute;
	(*psmf)[SG_DATA][SG_CYCLIC_REPORT_TIME_HOUR]		= m_ulLastCyclicReportTimeInHour;
	(*psmf)[SG_DATA][SG_CYCLIC_REPORT_TIME_MINUTE]		= m_ulLastCyclicReportTimeInMinute;

	//Check Load/Save Data
    pUtl->UpdateSGConfig();
	
	// save & close config file
    pUtl->CloseSGConfig();

    return TRUE;
}

//Only support 16 Len Format
CString	CSecsComm::SecsGetTime()
{
	CString	szTime, szYear, szMonth, szDay, szHour, szMin, szSecond;
	INT		nYear,  nMonth, nDay,    nHour, nMin,   nSecond;

	CTime	tmCurTime = CTime::GetCurrentTime();

	nYear	= tmCurTime.GetYear();
	nMonth  = tmCurTime.GetMonth();
	nDay    = tmCurTime.GetDay();
	nHour   = tmCurTime.GetHour();
	nMin = tmCurTime.GetMinute();
	nSecond = tmCurTime.GetSecond();

	szYear.Format(_T("%d"),nYear);
	if (nYear < 10) szTime += UCHAR(0x30);
	szTime += szYear;

	szMonth.Format(_T("%d"),nMonth);
	if (nMonth < 10) szTime += UCHAR(0x30);
	szTime += szMonth;

	szDay.Format(_T("%d"),nDay);
	if (nDay < 10) szTime += UCHAR(0x30);
	szTime += szDay;

	szHour.Format(_T("%d"),nHour);
	if (nHour < 10) szTime += UCHAR(0x30);
	szTime += szHour;

	szMin.Format(_T("%d"),nMin);
	if (nMin < 10) szTime += UCHAR(0x30);
	szTime += szMin;

	szSecond.Format(_T("%d"),nSecond);
	if (nSecond < 10) szTime += UCHAR(0x30);
	szTime += szSecond;

	szTime += UCHAR(0x30);
	szTime += UCHAR(0x30);

	return szTime;
}

INT CSecsComm::StringLength(PCHAR pcBuffer)
{
	INT nLen = 0;
	while (*(pcBuffer + nLen) != 0)
	{
		nLen++;
	}

	return nLen;
}

VOID CSecsComm::SetDisplayText(INT nDisplayPage)
{
	INT nDisplayCnt = 0;
	POSITION pos;

	for(int i = 0; i < 10; i++)
	{
		nDisplayCnt = (nDisplayPage * 10) + i;

		if(nDisplayCnt < (m_nTMsgTextCnt))
		{
			if( ( pos = m_stTMsgTextList.FindIndex(nDisplayCnt)) != NULL )
			{
				m_szTMsgText[i] = m_stTMsgTextList.GetAt(pos);
			}
			else 
			{
				m_szTMsgText[i] = _T("");	
			}
		}
		else
		{
			m_szTMsgText[i] = _T("");			
		}
	}

	//Check can index next page	
	if(m_nTMsgTextCnt > (nDisplayCnt + 1))
	{
		m_bTMsgNextPage = TRUE;
	}
	else
	{
		m_bTMsgNextPage = FALSE;
	}	
	//Check can index prev page
	if(nDisplayPage > 0)
	{
		m_bTMsgPrevPage = TRUE;
	}
	else
	{
		m_bTMsgPrevPage = FALSE;
	}
}

VOID CSecsComm::SwitchHMIPage()
{
	INT						nLength;
	IPC_CServiceMessage		srvMsg;
 	PCHAR					pBuffer = NULL;
	CString					szMenuPageName = _T("Information::Terminal Msgs");

	try
	{
		nLength = (szMenuPageName.GetLength() + 1) * sizeof(TCHAR);
		pBuffer = new CHAR[nLength];
		memset(pBuffer, 0x00, nLength);
		strncpy_s(pBuffer, nLength, szMenuPageName, szMenuPageName.GetLength() + 1);

		srvMsg.InitMessage(nLength, pBuffer);
		m_comClient.SendRequest(_T("HmiUserService"),
			_T("HmiExecutePage"), srvMsg, 0, 0, 100);
//*****SSSS
		delete[] pBuffer;
		pBuffer = NULL;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		if (pBuffer != NULL)
		{
			delete[] pBuffer;
			pBuffer	= NULL;
		}
	}	
}


BOOL CSecsComm::S2F41HostCmd_StartCmmd(SHostCmmd HostCmmd, SHostCmmdErr &HostCmmdErr)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bLoadPKGFile = (BOOL)(LONG)((*m_psmfSRam)["MS896A"]["Load PKG File"]);
	CString szStatus;
	INT nStatus = 0;

	if (pApp->m_bInBondPage == TRUE )	
	{
		m_bS2F41RemoteStartCmd = TRUE;		//v4.59A11
	}
	else
	{
		HostCmmdErr.Ack = HCACK_CANNOT_PERFORM_NOW;  // Send the alarm to server
		szStatus = "\t\t<<< Failed Start remote command as Not in Bond page \n Please go to Bond Page ";
		SetErrorMessage(szStatus);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szStatus);
	}

	HostCmmdErr.nErr = nStatus;

	szStatus.Format("\t\t<<< -- Load Start Button: m_bInBondPage = %d, bLoadPKGFile = %d, STATUS = %d", pApp->m_bInBondPage, bLoadPKGFile, nStatus);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szStatus);
	return TRUE;
}

BOOL CSecsComm::S2F41HostCmd_StopCmmd(SHostCmmd HostCmmd, SHostCmmdErr &HostCmmdErr)
{
	IPC_CServiceMessage stMsg;
	INT nStatus = 0;
	CString szStatus;

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

	HostCmmdErr.nErr = nStatus;		//= 0;		//v4.51A12	

	//m_bStopByCommand = TRUE;
	return TRUE;
}


BOOL CSecsComm::S2F41HostCmd_ClearBin(SHostCmmd HostCmmd)	//v4.59A19	//Osram Penang
{
	CString szParam, szValue;
	CString szBinNo;
	CString szLog;
	LONG lBinNo = -1; 


	for (int i=0; i<HostCmmd.nParameter; i++) 
	{
		szParam = HostCmmd.arrCmmd[i].pchName;
		szParam.Replace("\n", "");
		szParam.Replace("\r", "");

		szValue = HostCmmd.arrCmmd[i].pchValue;
		szValue.Replace("\n", "");
		szValue.Replace("\r", "");

		CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t<<< S2F41 -- CLEARBIN (") + szParam + " = " + szValue + ")");

		if (szParam == "BINNO")
		{
			szBinNo	= szValue;
			lBinNo	= atol((LPCTSTR) szBinNo);
		}
	}

	if ((lBinNo < 0) || (lBinNo > BT_MAX_BINBLK_NO))
	{
		szLog.Format("SECSGEM: Invalid CLEARBIN -> BINNO = %ld received", lBinNo); 
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		SetErrorMessage(szLog);
		return FALSE;
	}

	ULONG ulBinNoToBeCleared = lBinNo;		//0 - 150

	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(ULONG), &ulBinNoToBeCleared);
	INT nConvID = 0;


	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "AutoClrBinCnt_SECSGEM", stMsg);
	//szLog.Format("\t\t<<< -- FRAME_UNLOAD (TYPE=%s, SLOTID=%s (%ld), BC=%s, GRADE=%s), IPC-ID=%d", 
	//				szFrameType, szSlotID, stInfo.lSlotID, szBarCode, szBinGrade, nConvID);

	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	return TRUE;
}

BOOL CSecsComm::S2F41HostCmd_StartLot(SHostCmmd HostCmmd)	//OsramTrip 8/22
{
	CString szParam, szValue;
	CString szLotNo;
	CString szLog;


	for (int i=0; i<HostCmmd.nParameter; i++) 
	{
		szParam = HostCmmd.arrCmmd[i].pchName;
		szParam.Replace("\n", "");
		szParam.Replace("\r", "");

		szValue = HostCmmd.arrCmmd[i].pchValue;
		szValue.Replace("\n", "");
		szValue.Replace("\r", "");

		CMSLogFileUtility::Instance()->MS_LogSECSGEM(CString("\t<<< S2F41 -- STARTLOT (") + szParam + " = " + szValue + ")");

		if (szParam == "LOTNO")
		{
			szLotNo	= szValue;
		}
	}

	if (szLotNo.GetLength() == 0)
	{
		szLog.Format("SECSGEM: Invalid STARTLOT -> LOTNO = %s received", szLotNo); 
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		SetErrorMessage(szLog);
		return FALSE;
	}

	(*m_psmfSRam)["MS"]["SECSGEM"]["WaferLotName"]	= szLotNo;

	IPC_CServiceMessage stMsg;
	INT nConvID = 0;
	BOOL bReply = TRUE;

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ReadWaferLotInfoFile_SECSGEM", stMsg);
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

	if (!bReply)
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("ReadWaferLotInfoFile_SECAGEM fails");
		return FALSE;
	}

	INT nConvID2 = m_comClient.SendRequest(WAFER_TABLE_STN, "ShowWaferLotDataCmd", stMsg);

	return TRUE;
}


BOOL CSecsComm::StartS2F41RemoteStartCmd()		//v4.59A11
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	pApp->m_bCycleStarted = TRUE;
	CMS896AStn::m_bWaferAlignComplete = TRUE;
	
	IPC_CServiceMessage stMsg;
	BOOL bReply = TRUE;

	INT nConvID = m_comClient.SendRequest("MapSorter", "PreStartCycle", stMsg);
/*
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
*/
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< Finished PreStartCycle()" );
	nConvID = m_comClient.SendRequest("MapSorter", "Demonstration", stMsg);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t<<< Finished Demonstration()" );

	m_bS2F41RemoteStartCmd = FALSE;
	return TRUE;
}