#include "stdafx.h"
#include "StringMapFile.h"
#include "PRFailureCaseLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPRFailureCaseLog* CPRFailureCaseLog::m_pInstance = NULL;

CPRFailureCaseLog* CPRFailureCaseLog::Instance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CPRFailureCaseLog();
	}

	return m_pInstance;
}

CPRFailureCaseLog::CPRFailureCaseLog()
{
	m_bBHMarkCaselogEnable = FALSE;
	m_bFailureCaselogEnable = FALSE;
	m_plImageNo = NULL;
}

CPRFailureCaseLog::~CPRFailureCaseLog()
{
}


VOID CPRFailureCaseLog::SetData(LONG *plImageNo)
{
	m_plImageNo = plImageNo;
	CheckImageNoValid();
}

VOID CPRFailureCaseLog::CheckImageNoValid()
{
	if ((*m_plImageNo >= 1000) || (*m_plImageNo < 0))
	{
		*m_plImageNo = 0;
	}
}

VOID CPRFailureCaseLog::SetData(BOOL bFailureCaselogEnable, CString szFailureCaselogPath)
{
	m_bBHMarkCaselogEnable  = bFailureCaselogEnable;
	m_bFailureCaselogEnable = bFailureCaselogEnable;
	m_szFailureCaselogPath	= szFailureCaselogPath;
}

VOID CPRFailureCaseLog::ToggleBHMarkCaselog()
{
	m_bBHMarkCaselogEnable = m_bBHMarkCaselogEnable & 0x01;
	m_bBHMarkCaselogEnable = !m_bBHMarkCaselogEnable;
}

BOOL CPRFailureCaseLog::IsEnableBHMarkCaselog()
{
	return m_bBHMarkCaselogEnable;
}


VOID CPRFailureCaseLog::ToggleFailureCaselog()
{
	m_bFailureCaselogEnable = m_bFailureCaselogEnable & 0x01;
	m_bFailureCaselogEnable = !m_bFailureCaselogEnable;
}

BOOL CPRFailureCaseLog::IsEnableFailureCaselog()
{
	return m_bFailureCaselogEnable;
}

VOID CPRFailureCaseLog::SetFailureCaselogPath(CString szFailureCaselogPath)
{
	m_szFailureCaselogPath = szFailureCaselogPath;
}


VOID CPRFailureCaseLog::ReadElement(CMapElement *pElement)
{
	m_bFailureCaselogEnable = (BOOL)(LONG)(*pElement)["bFailureCaselogEnable"];
	m_szFailureCaselogPath = (*pElement)["szFailureCaselogPath"];
}

VOID CPRFailureCaseLog::WriteElement(CMapElement *pElement)
{
	(*pElement)["bFailureCaselogEnable"] = m_bFailureCaselogEnable;
	(*pElement)["szFailureCaselogPath"] = m_szFailureCaselogPath;
}


LONG CPRFailureCaseLog::LogFailureCaseImage(PR_UWORD uwRecordID, PR_UBYTE ubSenderID, PR_UBYTE ubReceiverID, CString szFileName)
{
	LONG nErr = 0;

	if (m_bFailureCaselogEnable && (m_plImageNo != NULL) && m_szFailureCaselogPath.GetLength() > 0)
	{
		CString szFileFullPath;
		if (szFileName.IsEmpty())
		{
			szFileFullPath.Format("%s\\%03d.bmp", (const char*)m_szFailureCaselogPath, *m_plImageNo);
		}
		else
		{
			szFileFullPath.Format("%s\\%s.bmp", (const char*)m_szFailureCaselogPath, szFileName);
		}
		if (LogPRImage(szFileFullPath, uwRecordID, ubSenderID, ubReceiverID, nErr) == PR_ERR_NOERR)
		{
			(*m_plImageNo)++;
			CheckImageNoValid();
			return 0;
		}
	}
	return nErr;
}


LONG CPRFailureCaseLog::LogFailureCaseImage(PR_ULWORD ulImageID, PR_ULWORD ulStationID, PR_UBYTE ubSenderID, PR_UBYTE ubReceiverID, CString szFileName)
{
	LONG nErr = 0;

	if (m_bFailureCaselogEnable && (m_plImageNo != NULL) && m_szFailureCaselogPath.GetLength() > 0)
	{
		CString szFileFullPath;
		if (szFileName.IsEmpty())
		{
			szFileFullPath.Format("%s\\%03d.bmp", (const char*)m_szFailureCaselogPath, *m_plImageNo);
		}
		else
		{
			szFileFullPath.Format("%s\\%s.bmp", (const char*)m_szFailureCaselogPath, szFileName);
		}
		if (LogPRImage(szFileFullPath, ulImageID, ulStationID, ubSenderID, ubReceiverID, nErr) == PR_ERR_NOERR)
		{
			(*m_plImageNo)++;
			CheckImageNoValid();
			return 0;
		}
	}
	return nErr;
}


LONG CPRFailureCaseLog::LogPRImage(CString szPath, PR_UWORD uwRecordID, PR_UBYTE ubSID, PR_UBYTE ubRID, LONG &nErr)
{
	PR_PROC_IMG_SIMPLE_CMD		stCmd;
	PR_UWORD      uwCommunStatus = PR_COMM_ERR;
	PR_PROC_IMG_SIMPLE_RPY1		stRpy1;
	PR_PROC_IMG_SIMPLE_RPY2		stRpy2;

	nErr = 0;
	PR_InitProcImgSimpleCmd(&stCmd);
	stCmd.stImg.emLatch = PR_LATCH_NO_LATCH;                                    
	stCmd.stImg.emGrab = PR_GRAB_FROM_RECORD_ID;
	stCmd.stImg.uwRecordID = uwRecordID;   // same as that of the previous PR command whose image is to be saved
	stCmd.emIsDisplay = PR_FALSE;
	stCmd.emIsSaveToFile = PR_TRUE;
	strncpy_s((char*)stCmd.aubFilename, 99, (const char*)szPath, 99);
	PR_ProcImgSimpleCmd(&stCmd, ubSID, ubRID, &uwCommunStatus);            // ubSenderID, ubReceiverID must be the same as the previous PR command whose image is to be saved
	if (uwCommunStatus != PR_COMM_NOERR)
	{
		//printf("PR_ProcImgSimpleCmd is finished with commun status %u!\n", uwCommunStatus);
		nErr = 1;
		return uwCommunStatus;
	}
	PR_ProcImgSimpleRpy1(ubSID, &stRpy1);
	if (stRpy1.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		nErr = 2;
		//printf("PR_ProcImgSimpleRpy1 is finished with status %u, %u!\n",  stRpy1.uwCommunStatus, stRpy1.uwPRStatus);
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR) 
		{
			return stRpy1.uwCommunStatus;
		}
		return PR_ERROR_STATUS(stRpy1.uwPRStatus);
	}
	PR_ProcImgSimpleRpy2(ubSID, &stRpy2);
	if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		nErr = 3;
		//printf("PR_ProcImgSimpleRpy2 is finished with status %u, %u!\n",stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus);
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) 
		{
			return stRpy2.stStatus.uwCommunStatus;
		}
		return PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus);
	}
	return PR_ERR_NOERR;
}


LONG CPRFailureCaseLog::LogPRImage(CString szPath, PR_ULWORD ulImageID, PR_ULWORD ulStationID, PR_UBYTE ubSID, PR_UBYTE ubRID, LONG &nErr)
{
	PR_PROC_IMG_SIMPLE_CMD		stCmd;
	PR_UWORD      uwCommunStatus = PR_COMM_ERR;
	PR_PROC_IMG_SIMPLE_RPY1		stRpy1;
	PR_PROC_IMG_SIMPLE_RPY2		stRpy2;

	nErr = 0;
	PR_InitProcImgSimpleCmd(&stCmd);
	//stCmd.stImg.emLatch = PR_LATCH_NO_LATCH;                                    
	//stCmd.stImg.emGrab = PR_GRAB_FROM_RECORD_ID;
	//stCmd.stImg.uwRecordID = uwRecordID;   // same as that of the previous PR command whose image is to be saved
	stCmd.stImg.emLatch = PR_LATCH_FROM_GALLERY;
	stCmd.stImg.emGrab = PR_GRAB_FROM_PURPOSE;
	stCmd.stImg.uwStationID = ulStationID;
	stCmd.stImg.ulGalleryImageID = ulImageID;
	stCmd.emIsDisplay = PR_FALSE;
	stCmd.emIsSaveToFile = PR_TRUE;
	strncpy_s((char*)stCmd.aubFilename, 99, (const char*)szPath, 99);
	PR_ProcImgSimpleCmd(&stCmd, ubSID, ubRID, &uwCommunStatus);            // ubSenderID, ubReceiverID must be the same as the previous PR command whose image is to be saved
	if (uwCommunStatus != PR_COMM_NOERR)
	{
		//printf("PR_ProcImgSimpleCmd is finished with commun status %u!\n", uwCommunStatus);
		nErr = 1;
		return uwCommunStatus;
	}
	PR_ProcImgSimpleRpy1(ubSID, &stRpy1);
	if (stRpy1.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		nErr = 2;
		//printf("PR_ProcImgSimpleRpy1 is finished with status %u, %u!\n",  stRpy1.uwCommunStatus, stRpy1.uwPRStatus);
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR) 
		{
			return stRpy1.uwCommunStatus;
		}
		return PR_ERROR_STATUS(stRpy1.uwPRStatus);
	}
	PR_ProcImgSimpleRpy2(ubSID, &stRpy2);
	if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		nErr = 3;
		//printf("PR_ProcImgSimpleRpy2 is finished with status %u, %u!\n",stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus);
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) 
		{
			return stRpy2.stStatus.uwCommunStatus;
		}
		return PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus);
	}
	return PR_ERR_NOERR;
}


LONG CPRFailureCaseLog::LogFailureCaseDumpFile(PR_UBYTE ubSenderID, PR_UBYTE ubReceiverID, ULONG ulMaxNumOfLogCase)
{
	PR_DUMP_APP_LOG_CMD		stCmd;
	PR_DUMP_APP_LOG_RPY		stRpy;

	if (/*m_bFailureCaselogEnable &&*/ m_szFailureCaselogPath.GetLength() > 0)
	{
		PR_InitDumpAppLogCmd(&stCmd);

		strncpy_s((char*)stCmd.aubDesLogPath, 99, (const char*)m_szFailureCaselogPath, 99);
		stCmd.wIsSaveRecord = 1;  //Yes
		stCmd.uwStationID = 0;  //dump all station
		stCmd.uwMaxNumOfLogCase = (PR_UWORD)ulMaxNumOfLogCase;

		PR_DumpAppLogCmd(&stCmd, ubSenderID, ubReceiverID, &stRpy);

		if (stRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy.uwPRStatus))
		{
			if (stRpy.uwCommunStatus != PR_COMM_NOERR) 
			{
				return stRpy.uwCommunStatus;
			}
			return PR_ERROR_STATUS(stRpy.uwPRStatus);
		}
	}
	return PR_ERR_NOERR;
}
