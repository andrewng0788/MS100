#include "stdafx.h"
#include "MESConnector.h"


CMESConnector* CMESConnector::m_pInstance = NULL;

CMESConnector::CMESConnector()
{
	m_bEnableMESConnector = FALSE;
	m_nCurrentMachineStatus = UN_INITIALIZE_Q;
	m_bStopMachineRunning = FALSE;
	m_szIncomingPath = "";
	m_szOutgoingPath = "";
	m_szMESIncomingBackupPath = gszUSER_DIRECTORY + "\\History\\MES Incoming Backup";
	m_lMESTimeOut = 8;
}

CMESConnector::~CMESConnector()
{
} 

CMESConnector* CMESConnector::Instance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CMESConnector();

		return m_pInstance;
	}
	
	return m_pInstance;

}

BOOL CMESConnector::IsMESConnectorEnable()
{
	return m_bEnableMESConnector;
}

VOID CMESConnector::SetMESIncomingPath(CString szPath)
{
	m_szIncomingPath = szPath;
}

VOID CMESConnector::SetMESOutgoingPath(CString szPath)
{
	m_szOutgoingPath = szPath;
}

VOID CMESConnector::SetMESTimeout(LONG lTimeout)
{
	m_lMESTimeOut = lTimeout;
}

VOID CMESConnector::SetMachineNo(CString szMachineNo)
{
	m_szMachineNo = szMachineNo;
}

BOOL CMESConnector::SendMessage(CString szCmdID, CString szMsg)
{
	if (m_bEnableMESConnector == FALSE)
	{
		//MessageLog("MES connector disabled\n");	//v4.42T15
		return TRUE;
	}

	CString szFilename, szDateTime;
	CStdioFile cfMsgFile;
	CString szOutPath, szInPath;

	szOutPath = m_szOutgoingPath + "\\" + m_szMachineNo;
	szInPath = m_szIncomingPath + "\\" + m_szMachineNo;
	
	CreateDirectory(szOutPath, NULL);

	m_ctSendMsgTime = CTime::GetCurrentTime();

	szDateTime = m_ctSendMsgTime.Format("%y%m%d%H%M%S");

	// Remove All Files in Rx Folder
	FindnRemoveFiles(szInPath);

	szFilename = szOutPath;
	szFilename = szFilename + "\\SORT" + szDateTime + ".txt";
 
	try
	{
		if (cfMsgFile.Open(szFilename, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive) == FALSE)
		{
			MessageLog("Cannot open the file to send message\n");
			return FALSE;
		}

		cfMsgFile.WriteString(szCmdID + "@" + szMsg +"\n");
		cfMsgFile.Close();
	}
	catch (CFileException e)
	{
		MessageLog("Error: Cannot send file \n");
		return TRUE;
	}

	MessageLog(szCmdID + "@" + szMsg +"\n");
	
	return TRUE;
}


INT CMESConnector::RecvMessage(CString& szMsg)
{
	BOOL bRxFile = FALSE;
	INT nCount = 0;
	CString szFilename;
	CStdioFile cfMsgFile;
	CString szStatusTemp;
	CString szOutPath, szInPath;

	if (m_bEnableMESConnector == FALSE)
		return TRUE;

	szOutPath = m_szOutgoingPath + "\\" + m_szMachineNo;
	szInPath = m_szIncomingPath + "\\" + m_szMachineNo;

	// Create Dir for Rx Msg
	CreateDirectory(szInPath, NULL);

	// Create MES Incoming Backup
	CreateDirectory(m_szMESIncomingBackupPath, NULL);
	
	do
	{
		bRxFile = SearchFilesInFolder(szInPath, szFilename);
		
		if (bRxFile == TRUE)
		{
			try
			{
				if (cfMsgFile.Open(szFilename,CFile::modeRead|CFile::shareExclusive) == TRUE)
				{
					cfMsgFile.ReadString(szMsg);
					cfMsgFile.Close();
					
					if (szMsg.GetLength()>= 2)
					{
						szStatusTemp = szMsg.Left(2);
						
						if (szStatusTemp.MakeUpper() == "OK")
						{		
							MessageLog(szMsg +"\n");
							DeleteFile(szFilename);
							if (szMsg.Find("OK@") != -1)
							{
								szMsg.Replace("OK@", "");	//v4.43T4
							}
							return TRUE;
						}
						
						if (szStatusTemp.MakeUpper() == "NG")
						{
							MessageLog(szMsg +"\n");
							DeleteFile(szFilename);
							if (szMsg.Find("NG@") != -1)
							{
								szMsg.Replace("NG@", "");	//v4.49A10
							}
							return MES_NG_RX;
						}
					}
				}
			}
			catch (CFileException e)
			{
				MessageLog("Error: Cnnot Read MES Message \n");
			}
		}

		Sleep(100);
		nCount++;

	}while(nCount != (m_lMESTimeOut * 10));

	szMsg = "MES Time Out";
	MessageLog(szMsg +"\n");
	
	return MES_TIME_OUT_RX;
}

BOOL CMESConnector::SearchFilesInFolder(CString szPath, CString& szFilename)
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CTimeSpan TimeDiff;
	
	//BOOL bFoundFiles = FALSE;
	
	hSearch = FindFirstFile(szPath + "\\" + "*.txt", &FileData);

	if ( hSearch == INVALID_HANDLE_VALUE) 
	{ 
		return FALSE;
	}

	CString szName = (LPCTSTR) FileData.cFileName;		//Klocwork	//v4.27
	szFilename = szPath + "\\" + szName;

	// Copy the file to backup folder
	CopyFile(szFilename, m_szMESIncomingBackupPath + "\\" + FileData.cFileName,FALSE);

	FindClose(hSearch);		//Klocwork	//v4.27
	
	/*
	do 
	{
		szFilename = szPath + "\\" + FileData.cFileName;
		bFoundFiles = TRUE;
		break;		
	}while (FindNextFile(hSearch, &FileData) == TRUE);
	
 
	// Close the search handle. 
	if ( FindClose(hSearch) == FALSE || bFoundFiles == FALSE) 
	{ 
		return FALSE;
	}
	*/

	return TRUE;
}

INT CMESConnector::UpdateMachineStatus(INT nState, CString& szMsg)
{
	if (m_bEnableMESConnector == FALSE)
		return TRUE;

	if (nState == IDLE_Q)
	{
		//m_nCurrentMachineStatus = IDLE_Q;
		SendMessage(MES_CHANGE_MACHINE_STATUS_ID ,"I");
		//nReply = RecvMessage();
		return TRUE;
	}

	else if (nState == PRESTART_Q)
	{
		//m_nCurrentMachineStatus = PRESTART_Q;
		// Machine Start to Run
		SendMessage(MES_CHANGE_MACHINE_STATUS_ID ,"R");
	
		return TRUE;	
	}
	
	return TRUE;

}


VOID CMESConnector::UpdateSendMsgTime()
{
	if (m_bEnableMESConnector == FALSE)
		return;

	m_ctSendMsgTime = CTime::GetCurrentTime();
}

BOOL CMESConnector::FindnRemoveFiles(CString szPath)
{
	if (m_bEnableMESConnector == FALSE)
		return TRUE;

	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString szFilename;
	
	hSearch = FindFirstFile(szPath + "\\" + "*", &FileData);

	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return FALSE;
	}

	do 
	{
		szFilename = szPath + "\\" + FileData.cFileName;
		DeleteFile(szFilename);
				
	}while (FindNextFile(hSearch, &FileData) == TRUE);
 
	// Close the search handle. 
	if ( FindClose(hSearch) == FALSE) 
	{ 
		return FALSE;
	} 

	return TRUE;
}

CString CMESConnector::GetMESOutgoingPath()
{
	if (m_bEnableMESConnector)
	{
		return m_szOutgoingPath +  "\\" + m_szMachineNo;
	}
	else
	{
		return "";
	}
}

VOID CMESConnector::EnableMESConnector(BOOL bSet)
{
	m_bEnableMESConnector = bSet;
}

BOOL CMESConnector::MessageLog(CString szMsg)
{
	CString szLogFileName  = gszUSER_DIRECTORY + "\\History\\MES_Message.log";

	CStdioFile cfLogFile;

	if (cfLogFile.Open(szLogFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|
		CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		return FALSE;
	}

	CString szDateTime;
	CTime curTime = CTime::GetCurrentTime();
	szDateTime = curTime.Format("[%Y-%m-%d %H:%M:%S]");
	cfLogFile.SeekToEnd();
	cfLogFile.WriteString(szDateTime + "\t" + szMsg);
	cfLogFile.Close();
	return TRUE;
}

