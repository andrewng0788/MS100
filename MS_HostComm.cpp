/////////////////////////////////////////////////////////////////
// MS_HostComm.cpp : interface of the MS896AStn class
//
//	Description:
//		MS896A Host Communication 
//
//	Date:		Wednesday, Nov 16, 2005
//	Revision:	1.00
//
//	By:			Barry Chu
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "MS896A.h"
#include "HostComm.h"
#include "FileUtil.h"
#include "MS_HostCommConstant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Host comm
BOOL CMS896AApp::ReadHostCommConfig()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	BOOL bFileExist = FALSE;
	short	i = 0;	

	// open config file
	if (pUtl->LoadHostCommConfig() == FALSE)
		return FALSE;

    // get file pointer
	psmf = pUtl->GetHostCommFile();

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("Read HostComm configuration");	//v4.11

	if ( psmf == NULL )
		return FALSE;

	//Read content
	m_szHostMachineID	= (*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_MCID];
	m_szHostAddress		= (*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_IPADDR];
	m_ulHostPortNo		= (*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_PORTNO];
	m_szHostLotNo		= (*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_LOTID];
	m_bHostAutoConnect	= (BOOL)(LONG)(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_AUTO_CONNECT];
	m_bHostCommEnabled	= (BOOL)(LONG)(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_ENABLE_COMM];
	m_bHostAutoReConnect= (BOOL)(LONG)(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_AUTO_RETRY];


    // close config file
	pUtl->CloseHostCommConfig();

	return TRUE;
}

BOOL CMS896AApp::WriteHostCommConfig()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadHostCommConfig() == FALSE)
		return FALSE;

    // get file pointer
	psmf = pUtl->GetHostCommFile();

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("Write HostComm configuration");	//v4.11

	if ( psmf == NULL )
		return FALSE;

	//update content
	(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_MCID]			= m_szHostMachineID;
	(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_IPADDR]		= m_szHostAddress;	
	(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_PORTNO]		= m_ulHostPortNo;
	(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_LOTID]			= m_szHostLotNo;
	(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_AUTO_CONNECT]	= m_bHostAutoConnect;
	(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_ENABLE_COMM]	= m_bHostCommEnabled;
	(*psmf)[MS_HCOM_GENERAL][MS_HCOM_GENERAL_AUTO_RETRY]	= m_bHostAutoReConnect;


    // close config file
	pUtl->UpdateHostCommConfig();
	pUtl->CloseHostCommConfig();

	return TRUE;
}


BOOL CMS896AApp::SaveHostCommContent(CString szName, CString szValue)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadHostCommConfig() == FALSE)
		return FALSE;

    // get file pointer
	psmf = pUtl->GetHostCommFile();

	if ( psmf == NULL )
		return FALSE;

	//update content
	(*psmf)[MS_HCOM_DATA][szName][MS_HCOM_DATA_CONTENT] = szValue;

    // close config file
	pUtl->UpdateHostCommConfig();
	pUtl->CloseHostCommConfig();

	return TRUE;
}

BOOL CMS896AApp::SaveHostCommStatus(CString szName, BOOL bStatus)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadHostCommConfig() == FALSE)
		return FALSE;

    // get file pointer
	psmf = pUtl->GetHostCommFile();

	if ( psmf == NULL )
		return FALSE;

	//update content
	(*psmf)[MS_HCOM_DATA][szName][MS_HCOM_DATA_SENDOK] = bStatus;

    // close config file
	pUtl->UpdateHostCommConfig();
	pUtl->CloseHostCommConfig();

	return TRUE;
}


VOID CMS896AApp::SetupHostComm()
{
	if (m_bEnableTCPComm == TRUE)
	{
		AfxSocketInit();
		ReadHostCommConfig();

		//Delete last log file
		CString szLogFile = gszUSER_DIRECTORY + "\\History\\HostComm.log";
		DeleteFile(szLogFile);

		if (m_bHostAutoConnect == TRUE)
		{
			ConnectHostComm(TRUE);
		}
	}
}


VOID CMS896AApp::RetryHostComm()
{
	m_bHostReConnectting = FALSE;

	if ( (m_bHostCommAbortByHost == TRUE) && (m_bHostAutoReConnect == TRUE) )
	{
		m_bHostReConnectting = TRUE;

		static int nCount = 0;

		if ( nCount++ < 200 )
		{
			return;
		}
		else
		{
			nCount = 0;
		}

		ConnectHostComm(TRUE);

		if (m_pHostComm->m_bConnected == TRUE)
		{
			m_bHostCommAbortByHost = FALSE;
			m_bHostReConnectting = FALSE;
		}
	}
}


VOID CMS896AApp::DisplayHostCommMessage(const CString& szStatus, BOOL bDisplay)
{
	CString szLogFile = gszUSER_DIRECTORY + "\\History\\HostComm.log";

	CString szText;
	CTime CurTime = CTime::GetCurrentTime();

	szText = CurTime.Format("[%H:%M:%S %d/%m/%y] ") + szStatus;

	if ( bDisplay == TRUE )
	{
		m_szHostCommStatus = szText;
	}

	FILE *fpRecord = NULL;
	errno_t nErr = fopen_s(&fpRecord, szLogFile, "a+");
	if ((nErr == 0) && (fpRecord != NULL))
	{
		fprintf(fpRecord, szText + "\n");
		fclose(fpRecord);
	}	
}


BOOL CMS896AApp::CheckHostCommStatus()
{
	if (m_pHostComm == NULL)
	{
		m_bHostCommConnected = FALSE;
		return FALSE;
	}

	//Read Host Comm status
	m_bHostCommConnected = m_pHostComm->m_bConnected;
	return m_bHostCommConnected;
}


VOID CMS896AApp::ConnectHostComm(BOOL bConnect)
{
	if (bConnect == TRUE)
	{
		if ( (m_pHostComm == NULL) || (m_pHostComm->m_bConnected == FALSE) )
		{
			// Create communication 
			//delete m_pHostComm;		//Klocwork	//v4.04
			m_pHostComm = new CHostComm;
			m_pHostComm->m_pParent = this;
			m_pHostComm->Create();
			m_pHostComm->Connect(m_szHostAddress, (UINT)m_ulHostPortNo);

			CString szText;
			szText.Format("Connect @ %s:%d", m_szHostAddress, m_ulHostPortNo);
			DisplayHostCommMessage(szText, TRUE);

			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szText);	//v4.11
		}
	}
	else
	{
		m_bHostCommAbortByHost = FALSE;

		if (m_pHostComm->m_bConnected == TRUE)
		{
			m_pHostComm->Close();
			m_pHostComm->m_bConnected = FALSE;

			DisplayHostCommMessage("Close by User", TRUE);
		}
	}
}


BOOL CMS896AApp::SendMessageToHost(CString szMessage)
{
	if ( (m_bEnableTCPComm == TRUE) && (m_bHostCommEnabled == TRUE) )
	{
		if (CheckHostCommStatus() == TRUE)
		{
			char *acBuffer;

			acBuffer = new char[szMessage.GetLength() + 1];

			strcpy_s(acBuffer, szMessage.GetLength() + 1, szMessage);
			acBuffer[szMessage.GetLength()] = 10;

			m_pHostComm->Send(acBuffer, szMessage.GetLength()+1);
			delete [] acBuffer;

			Sleep(100);

			DisplayHostCommMessage("Tx: " + szMessage, TRUE);

			return TRUE;
		}
		//else
		//{
		//	HmiMessage("Communication is disconnected!\nPlease check", "TCP/IP Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL);
		//}
	}

	return FALSE;
}


BOOL CMS896AApp::SendHostMachineIDAck()
{
	BOOL bReturn = FALSE;

	m_szHostCommandID = MS_HCOM_CMD_ACK_MCCHECK;

	SaveHostCommContent(MS_HCOM_CMD_ACK_MCCHECK, m_szHostMachineID);

	bReturn = SendMessageToHost(m_szHostMachineID);

	SaveHostCommStatus(MS_HCOM_CMD_ACK_MCCHECK, bReturn);

	return bReturn;
}

BOOL CMS896AApp::SendHostLotNoAck()
{
	BOOL bReturn = FALSE;
	CString szTxMessage;
	CString szMachineID;
	int nLength;

	nLength = m_szHostMachineID.GetLength();
	if (nLength > 1)
	{
		szMachineID = m_szHostMachineID.Right(nLength-1);
	}
	else
	{
		szMachineID = m_szHostMachineID;
	}

	m_szHostCommandID = MS_HCOM_CMD_ACK_LOTNO;

	szTxMessage = m_szHostCommandID + ":" + szMachineID + ":" + m_szHostLotNo;

	SaveHostCommContent(MS_HCOM_CMD_ACK_LOTNO, szTxMessage);
	
	bReturn = SendMessageToHost(szTxMessage);

	SaveHostCommStatus(MS_HCOM_CMD_ACK_LOTNO, bReturn);

	return bReturn;
}


BOOL CMS896AApp::SendHostWaferEnd(CString szMessage)
{
	BOOL bReturn = FALSE;
	CString szTxMessage;
	CString szMachineID;
	int nLength;

	nLength = m_szHostMachineID.GetLength();
	if (nLength > 1)
	{
		szMachineID = m_szHostMachineID.Right(nLength-1);
	}
	else
	{
		szMachineID = m_szHostMachineID;
	}

	m_szHostCommandID = MS_HCOM_CMD_TX_WAFEREND;

	szTxMessage	= m_szHostCommandID + ":" + szMachineID + ":" + m_szHostLotNo + ":" +szMessage;

	SaveHostCommContent(MS_HCOM_CMD_TX_WAFEREND, szTxMessage);

	bReturn = SendMessageToHost(szTxMessage);

	SaveHostCommStatus(MS_HCOM_CMD_TX_WAFEREND, bReturn);

	return bReturn;
}


BOOL CMS896AApp::SendHostClearBin(CString szMessage)
{
	BOOL bReturn = FALSE;
	CString szTxMessage;
	CString szMachineID;
	int nLength;

	nLength = m_szHostMachineID.GetLength();
	if (nLength > 1)
	{
		szMachineID = m_szHostMachineID.Right(nLength-1);
	}
	else
	{
		szMachineID = m_szHostMachineID;
	}

	m_szHostCommandID = MS_HCOM_CMD_TX_BINFULL;

	szTxMessage = m_szHostCommandID + ":" + szMachineID + ":" + m_szHostLotNo + ":" +szMessage;

	SaveHostCommContent(MS_HCOM_CMD_TX_BINFULL, szTxMessage);

	bReturn = SendMessageToHost(szTxMessage);

	SaveHostCommStatus(MS_HCOM_CMD_TX_BINFULL, bReturn);

	return bReturn;
}


BOOL CMS896AApp::DecodeHostLotNo(CString szMessage)
{
	BOOL bReturn = FALSE;
	CString szCommand;
	CString szContent;
	int nCol = 0;

	nCol = szMessage.Find(":");
	if (nCol != -1)
	{
		szContent = szMessage.Mid(nCol+1);
		nCol = szContent.Find(":");
		if (nCol != -1)
		{
			m_szHostLotNo = szContent.Mid(nCol+1);

			//update LotNo
			WriteHostCommConfig();
			bReturn = TRUE;
		}		
		else
		{
			bReturn = FALSE;
		}
	}

	return bReturn;
}


BOOL CMS896AApp::ReceiveHostUnknownMessage()
{
	BOOL bReturn = FALSE;

	DisplayHostCommMessage("Rx: Receive Unknown Message", FALSE);


	return bReturn;
}


LONG CMS896AApp::UpdateHostLotID(CString szLotID)
{
	LONG lReturn = -1;

	m_bHostLotIDMatch = TRUE;

	if ( (m_bEnableTCPComm == TRUE) && (m_bHostCommEnabled == TRUE) )
	{
		if (CheckHostCommStatus() == TRUE)
		{
			if (m_szHostLotNo == szLotID)
			{
				lReturn = 1;
				m_bHostLotIDMatch = TRUE;
			}
			else
			{
				lReturn = 0;
				m_bHostLotIDMatch = FALSE;
			}
		}
	}

	return lReturn;
}


