#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "SecsComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//================================================================
//						EQUIPMENT ASK HOST
//================================================================
//================================================================
// Function Name: 		EquipUploadProcessProgram
// Input arguments:     filename(PPID)
// Output arguments:	None
// Description:   		Equip upload a process program to host and at most three times try it again
// Return:				GEMOK or GEMERR
// Remarks:				S7F1, S7F3 called by Menu
//================================================================
SHORT CSecsComm::EquipUploadProcessProgram(const char *szPPFile, const char *szPPID)
{
	SHORT sRes = FALSE;
	CString str;
	CFile fTemp;
	
	if (fTemp.Open(szPPFile, CFile::modeRead))
	{
		UINT ullSize = (UINT)fTemp.GetLength();
		char *pBuffer = new char[ullSize];
		fTemp.SeekToBegin();
		fTemp.Read(pBuffer, ullSize);
		fTemp.Close();
		m_pGemStation->SetMaxHsmsDataSize(15 * 1000 * 1000);
		sRes = m_pGemStation->DownloadProcessProgram(szPPID, ullSize, pBuffer);		// S7F1 S7F3, seems to send ppg to host
		delete[] pBuffer;
	}
	else 
	{
		sRes = 2;
	}
	return sRes;
}

//================================================================
// Function Name: 		SG_CmdDownloadPPAck
// Input arguments:     filename (PPID)
// Output arguments:	None
// Description:   		Callback from ASMGEM when the S7F4 (Process Program Acknowledge) is
// Return:				received from the host(DownloadProcessProgram)
// Remarks:				Callback function---Check the result of result for upload PPG
//================================================================
LONG CSecsComm::SG_CmdDownloadPPAck(IPC_CServiceMessage &svMsg) // S7F4
{
	UCHAR ucChar[2];
	try
	{
		svMsg.GetMsg(sizeof(ucChar), ucChar);
		m_bPPGMAck = ucChar[0];
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
	return 1;
}

CString CSecsComm::GetPPGMAckErrMsg()
{
	CString szErrMsg = "";
	switch (m_bPPGMAck)
	{
	case ACK7:
		break;
	case ACK7_PERMISSION_NOT_GRANT:
		szErrMsg = "Permission Not Grant";
		break;
	case ACK7_LENGTH_ERROR:
		szErrMsg = "Length Error";
		break;
	case ACK7_MATRIX_OVERFLOW:
		szErrMsg = "Matrix Overflow";
		break;
	case ACK7_PPID_NOT_FOUND:
		szErrMsg = "PPID Not Found";
		break;
	case ACK7_MODE_UNSUPPORTED:
		szErrMsg = "Mode Not Supported";
		break;
	default:
		szErrMsg = "TimeOut";
		break;
	}
	return szErrMsg;
}

LONG CSecsComm::SC_EquipUploadPPG(const char *szPPID)
{
	LONG start_t;
	SHORT sRes;
    CString szPPFile;
    szPPFile = m_szPPFilePath + "\\" + szPPID;


	SetPPGMAck(6); //TimeOut
	sRes = EquipUploadProcessProgram(szPPFile, szPPID);
	if (sRes == 2) 
	{
		return 2;    //Abort
	}   
	if (sRes == 0) 
	{
		return 0;    //SECS communication problem
	}   

	if (sRes == 1)
	{
		CString szMsg;
		szMsg = "Uploading Recipe...";
		SetAlert_WarningMsg(IDS_MS_WARNING_UPLOADING_RECIPE, szMsg);
		start_t = GetCurTime();
		while (PPGMAck() == 6)
		{
			if (elapse_time(start_t) > 60000 * 2)
			{
				break ;
			}
			Sleep(200);
		}
		SetAlarmLamp_Yellow(FALSE, TRUE);
		CloseAlarm();
	}
	return (PPGMAck() == ACK7) ? 1 : 3; //1--Sucessfully, 3--TimeOut
}

//===============================================================================
//   Equipment upload recipe to host by menu
//==============================================================================
LONG CMS896AApp::SC_CmdDownloadPPG(IPC_CServiceMessage& svMsg) // SC_CmdUploadPPG
{
    if (IsSecsGemInit() == FALSE)
	{
		HmiMessage("SECS/GEM Communication Problem");
        return -1;
	}

    INT len = svMsg.GetMsgLen();
    char *pPPID = new char[len];
    svMsg.GetMsg(len, pPPID);

	LONG lRet = m_pSecsComm->SC_EquipUploadPPG(pPPID);
    delete [] pPPID;

	CString szErrorMsg;

	switch (lRet)
	{
	case 0:
        HmiMessage("PP File Upload FAILED");
		break;
	case 1:
        HmiMessage("PP File Upload Completed");
		break;
	case 2:
        HmiMessage("No such ProcessPrgram ID");
		break;
	case 3:
		szErrorMsg = "PP File Upload FAILED--" + m_pSecsComm->GetPPGMAckErrMsg();
        HmiMessage(szErrorMsg);
		break;
	}
    return 1;
}

//===============================================================================
//   Equipment download recipe from host by menu
//==============================================================================
LONG CMS896AApp::SC_CmdRequestPPG(IPC_CServiceMessage& svMsg)
{
    if (!IsSecsGemInit() || (m_pSecsComm == NULL))
	{   
		HmiMessage("SECS/GEM Communication Problem");
        return -1;
	}

    INT len = svMsg.GetMsgLen();
    char *pPPID = new char[len];

    svMsg.GetMsg(len, pPPID);

	//S7F5: seems get process program from host; data is received through S7F6 below
	//m_pGemStation->ProcessProgramReq(pPPID); 
	LONG lRet = m_pSecsComm->SC_EquipDownloadPPG(pPPID);
	delete[] pPPID;

	CString szErrorMsg;
	
	switch (lRet)
	{
	case 0:
        HmiMessage("PP File Download FAILED");
		break;
	case 1:
        HmiMessage("PP File Download Completed");
/*
			csMsgA.Format("Load Process Program [%s] Into RAM ?", pPPID);
			if (gbDeviceChangeEvent || HmiUserMessage(csMsgA, "Load Package Confirmation", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
			{
				if (LoadProcessPgm(pPPID, bPP_SelectTextFormat))
				{
					SetPPExecName(pPPID);
						//sprintf_s(cErrText[0], sizeof(cErrText[0]), "System Message");
						//sprintf_s(cErrText[1], sizeof(cErrText[1]), "Process Pgm Loaded");
					i = AssignGeneralMsgString(30);
					ed_display_text(cErrText, i, TRUE, ED_TOKEN_INFO); //chi done
				}
			}
*/
			break;
	case 2:
        HmiMessage("No PPGM File Name In Host");
		break;
	case 3:
		szErrorMsg = "PP File Download FAILED--" + m_pSecsComm->GetPPGMAckErrMsg();
        HmiMessage(szErrorMsg);
		break;
	}
    return 1;
}

VOID CSecsComm::SetPPGMAck(SHORT bPPGMAck)
{
	m_bPPGMAck = bPPGMAck;
}

SHORT CSecsComm::PPGMAck()
{
	return m_bPPGMAck;
}

LONG CSecsComm::SC_EquipDownloadPPG(const char *pPPID)
{
	SetPPGMAck(6); //TimeOut

	//S7F5: seems get process program from host; data is received through S7F6 below
	SetPPGMAck(6); //TimeOut
	BOOL bRet = m_pGemStation->ProcessProgramReq(pPPID); 
	if (!bRet) 
	{
		return 0;    //SECS communication problem
	}   
	if (bRet)
	{
		CString szMsg;
		szMsg = "Downloading Recipe...";
		SetAlert_WarningMsg(IDS_MS_WARNING_DOWNLOADING_RECIPE, szMsg);
		LONG start_t = GetCurTime();
		while (PPGMAck() == 6)
		{
			if (elapse_time(start_t) > 60000 * 2)
			{
				break ;
			}
			Sleep(200);
		}
		CloseAlarm();

		if (PPGMAck() == ACK7_PPID_NOT_FOUND) 
		{
			return 2;    //No this file
		} 
	}

	if ((PPGMAck() == ACK7) && (m_pGemStation != NULL))
	{
//		(*m_pGemStation)[SG_SECS_NAME_SV_PGM_DOWNLOADED] = TRUE; //Clear downloading PPGM
//		m_pGemStation->SendEvent(SG_CEID_PPGM_DOWNLOADED);
//		SendRecipeEvent(SG_CEID_RECIPE_CHANGE, szPPID, 2); // Create
	}
	return (PPGMAck() == ACK7) ? 1 : 3;	//1--Sucessfully, 3--TimeOut
}



//================================================================
// Function Name: 		SG_CmdReceivePPRequest
// Input arguments:     filename (PPID)
// Output arguments:	None
// Description:   		Callback from ASMGEM when the S7F6 (Process Program Request) is
// Return:				received from the host(ProcessProgramReq)
// Remarks:				Callback function
//================================================================
LONG CSecsComm::SG_CmdReceivePPRequest(IPC_CServiceMessage& svMsg)	//S7F6
{
 	CString szLog;
	if (IsSecsGemInit() == FALSE)
	{
        return -1;
	}

	if (_access(m_szPPFilePath, 0) == -1)
	{
		szLog.Format("\t<<< S7F6 (MS received PPG) - Invalid path = %s", m_szPPFilePath);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		m_pGemStation->SendPPAck(ACK7_PPID_NOT_FOUND);	//S7F4
		//Download Recipe Failure
		SetPPGMAck(ACK7_PPID_NOT_FOUND);
		return -1;
	}

	//Read the process program blocks
	int nSize = svMsg.GetMsgLen();
	char* pTemp = new char[nSize];
	svMsg.GetMsg(nSize, pTemp);

	unsigned long ulTotalLength = 0;
	unsigned long ulIndex = 0;
	unsigned long ulBlockSize = 0;
	BOOL bLastBlock = FALSE;

	unsigned long ulItemIndex = 0;
	
	// Read the first 16 byte block information
	memcpy(&ulTotalLength,	pTemp+ulItemIndex,	sizeof(unsigned long));
	ulItemIndex += sizeof(unsigned long);
	memcpy(&ulIndex,		pTemp+ulItemIndex,	sizeof(unsigned long));
	ulItemIndex += sizeof(unsigned long);
	memcpy(&ulBlockSize,	pTemp+ulItemIndex,	sizeof(unsigned long));
	ulItemIndex += sizeof(unsigned long);
	memcpy(&bLastBlock,		pTemp+ulItemIndex,	sizeof(BOOL));
	ulItemIndex += sizeof(BOOL);

	//v4.59A31
	CString szPPID = pTemp + ulItemIndex; 
	ulItemIndex += szPPID.GetLength() + 1;

	// I choose to create a big buffer. <4 byte total length> is 
	// the total length of the whole process program.
	// Since the callback is called continuously, we should only create
	// the buffer once. 
	if (m_pPPGBuffer == NULL)
	{
		m_pPPGBuffer = new char[ulTotalLength];
		memset(m_pPPGBuffer, 0, ulTotalLength);
		m_nPPGBufferIndex = 0;
	}

	// Append this block to the buffer  
	memcpy(m_pPPGBuffer + m_nPPGBufferIndex, pTemp + ulItemIndex, ulBlockSize);
	m_nPPGBufferIndex += ulBlockSize;

	// Check if last block
	if (bLastBlock)
	{
		// This is the last block we save the buffer to the file
		CFile fTest;

		if (szPPID.GetLength() == 0)	//v4.57A3
		{
			szPPID = _T("autoline.pkg");
		}

		CString szFileName = m_szPPFilePath + "\\" + szPPID;

		szLog.Format("\t<<< S7F6 (MS received PPG) - PATH = %s, Name = %s", m_szPPFilePath, szPPID);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		if (fTest.Open(szFileName, CFile::modeWrite|CFile::modeCreate))
		{
			fTest.Write(m_pPPGBuffer, ulTotalLength);
			fTest.Close();
		}

		// Since last block, we reset the buffer
		delete[] m_pPPGBuffer;
		m_pPPGBuffer = NULL;
		m_nPPGBufferIndex = 0;

		szLog.Format("\t<<< S7F6 (MS received PPG) - Total-Length = %lu, BlkSize = %lu, IsLastBlk=%d, file=%s", 
							ulTotalLength, ulBlockSize, bLastBlock, szFileName);
		//Download Recipe Finished
		SetPPGMAck(ACK7);
	}
	else
	{
		szLog.Format("\t<<< S7F6 (MS received PPG) - Total-Length = %lu, BlkSize = %lu, IsLastBlk=%d", 
							ulTotalLength, ulBlockSize, bLastBlock);
	}
	
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	delete [] pTemp;
	return 1;
}


//============================================================
//						HOST ASK EQUIPMENT
//           Process Program Downloading and Uploading
//           before return callback(HOST Initiated)
//============================================================
//================================================================
// Function Name: 		SG_CmdHostReqGrantDnPPG
// Input arguments:     filename (PPID)
// Output arguments:	Null
// Description:   		First this is registered in callback function
//						Callback from ASMGEM when the S7F1 (Process Program Load Inquired) is
//						received from the host. 
// Return:				GEMOK or GEMERR
// Remarks:				S7F1 called By GEM/SECS Library
//================================================================
LONG CSecsComm::SG_CmdHostReqGrantDnPPG(IPC_CServiceMessage& svMsg) // REQ_GRANT_DN_PP S7F1
{
	if (!IsSecsGemInit())
		return -1;

    INT len, MsgLen;
    len = svMsg.GetMsgLen();
    char *pMsg = new char[len];
    char *pPPID = new char[len - sizeof(INT) + 1];
	memset(pPPID, 0, len - sizeof(INT) + 1);
    svMsg.GetMsg(len, pMsg);

	// Process program send
    //    PPGNT - OK
    //    PPGNT_ALREADY_HAVE - Already have
    //    PPGNT_NO_SPACE - No space
    //    PPGNT_INVALID_PPID - Invalid PPID
    //    PPGNT_BUSY - Busy, try later
    //    PPGNT_WILL_NOT_ACCEPT - Will not accept
    memcpy(&MsgLen, pMsg+len-sizeof(INT), sizeof(INT));
    strncpy_s(pPPID, len - sizeof(INT) + 1, pMsg, len-sizeof(INT));
	pPPID[len - sizeof(INT)] = '\0';

    CString m_szFPID;
	m_szFPID = pPPID;
	//Check PPID empty or not
	if (m_szFPID.IsEmpty())
	{
		m_pGemStation->SendPPGrant(PPGNT_INVALID_PPID);
		delete[] pPPID;
		delete[] pMsg;
		return 1;
	}
	LONG m_szFPID_FileSize = MsgLen;

    CString szPPFile;
    CFileStatus status;
    szPPFile = m_szPPFilePath+"\\"+pPPID;
    if (CFile::GetStatus(szPPFile, status))
	{
        m_pGemStation->SendPPGrant(PPGNT_ALREADY_HAVE); // S7F2
	}
    else
	{
        m_pGemStation->SendPPGrant(PPGNT);
	}

    delete[] pPPID;
    delete[] pMsg;

	return 1;
}

//================================================================
// Function Name: 		SG_CmdHost_EquipDownloadPPG
// Input arguments:     filename (PPID)
// Output arguments:	Null
// Description:   		First this is registered in callback function
//						Callback from ASMGEM when the S7F3 (Process Program Send) is
//						received from the host. 
// Return:				GEMOK or GEMERR
// Remarks:				S7F3 called By GEM/SECS Library
//
// Block Structure as the followings (16 characters)
//			Total PPBody Length (4 character)
//			Current Block Num (4 character)
//			Current Block Size (4 character)
//			Last Block Flag (4 character)
//================================================================
LONG CSecsComm::SG_CmdHost_EquipDownloadPPG(IPC_CServiceMessage &svMsg) // DOWNLOAD_PROCESS_PROG S7F3
{
 	CString szLog;
	if (IsSecsGemInit() == FALSE)
	{
        return -1;
	}

	//v4.57A3
	if (_access(m_szPPFilePath, 0) == -1)
	{
		szLog.Format("\t<<< S7F3 (HOST Send PPG) - Invalid path = %s", m_szPPFilePath);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		m_pGemStation->SendPPAck(ACK7_PPID_NOT_FOUND);	//S7F4
		return -1;
	}

	//Read the process program blocks
	int nSize = svMsg.GetMsgLen();
	char* pTemp = new char[nSize];
	svMsg.GetMsg(nSize, pTemp);

	unsigned long ulTotalLength = 0;
	unsigned long ulIndex = 0;
	unsigned long ulBlockSize = 0;
	BOOL bLastBlock = FALSE;

	//CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t<<< S7F3 (HOST Send PPG) - START");

	// Block structure: 
	//		<4 byte total length> 
	//		<4 byte block index>
	//		<4 byte block size>
	//		<4 byte Last-block flag>
	//		<data>						(with size of "ulBlockSize" bytes)
	//		<PPID>						(string data)

	unsigned long ulItemIndex = 0;
	
	// Read the first 16 byte block information
	memcpy(&ulTotalLength,	pTemp+ulItemIndex,	sizeof(unsigned long));
	ulItemIndex += sizeof(unsigned long);
	memcpy(&ulIndex,		pTemp+ulItemIndex,	sizeof(unsigned long));
	ulItemIndex += sizeof(unsigned long);
	memcpy(&ulBlockSize,	pTemp+ulItemIndex,	sizeof(unsigned long));
	ulItemIndex += sizeof(unsigned long);
	memcpy(&bLastBlock,		pTemp+ulItemIndex,	sizeof(BOOL));
	ulItemIndex += sizeof(BOOL);

	//v4.59A31
	CString szPPID = pTemp + ulItemIndex; 
	ulItemIndex += szPPID.GetLength() + 1;

	// I choose to create a big buffer. <4 byte total length> is 
	// the total length of the whole process program.
	// Since the callback is called continuously, we should only create
	// the buffer once. 
	if (m_pPPGBuffer == NULL)
	{
		m_pPPGBuffer = new char[ulTotalLength];
		memset(m_pPPGBuffer, 0, ulTotalLength);
		m_nPPGBufferIndex = 0;
	}

	// Append this block to the buffer  
	memcpy(m_pPPGBuffer + m_nPPGBufferIndex, pTemp + ulItemIndex, ulBlockSize);
	m_nPPGBufferIndex += ulBlockSize;

	// Check if last block
	if (bLastBlock)
	{
		// This is the last block we save the buffer to the file
		CFile fTest;
		//CString szPPID = pTemp + ulItemIndex + ulBlockSize;	//v4.59A31

		if (szPPID.GetLength() == 0)	//v4.57A3
		{
			szPPID = _T("autoline.pkg");
		}

		CString szFileName = m_szPPFilePath + "\\" + szPPID;

		szLog.Format("\t<<< S7F3 (HOST Send PPG) - PATH = %s, Name = %s", m_szPPFilePath, szPPID);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		if (fTest.Open(szFileName, CFile::modeWrite|CFile::modeCreate))
		{
			fTest.Write(m_pPPGBuffer, ulTotalLength);
			fTest.Close();
		}

		// Since last block, we reset the buffer
		delete[] m_pPPGBuffer;
		m_pPPGBuffer = NULL;
		m_nPPGBufferIndex = 0;

		szLog.Format("\t<<< S7F3 (HOST Send PPG) - Total-Length = %lu, BlkSize = %lu, IsLastBlk=%d, file=%s", 
							ulTotalLength, ulBlockSize, bLastBlock, szFileName);

		// If m_pGemStation->EnableManualS7F4(TRUE) is used, 
		// you need to call SendPPAck manually. Default will send
		// PPACK automatically
		// ((CSWTestApp *)AfxGetApp())->m_pGemStation->SendPPAck(0);
	}
	else
	{
		szLog.Format("\t<<< S7F3 (HOST Send PPG) - Total-Length = %lu, BlkSize = %lu, IsLastBlk=%d", 
							ulTotalLength, ulBlockSize, bLastBlock);
	}
	
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	delete [] pTemp;
	return 1;

/*
	CStdioFile fTemp;
    INT nTotal = svMsg.GetMsgLen();
    char *pMsg = new char[nTotal];
    svMsg.GetMsg(nTotal, pMsg);
    CString str;
    INT nBuffer;

    memcpy(&nBuffer, pMsg, sizeof(nBuffer));
    char *pName = new char[nTotal-nBuffer-16];
    strncpy(pName, pMsg+16, nTotal-nBuffer-16);
    CString szPPFile;
    szPPFile = m_szPPFilePath+"\\"+pName;
	fTemp.Open(szPPFile, CFile::modeCreate|CFile::modeWrite|
        CFile::shareExclusive|CFile::typeBinary);
    fTemp.Write(pMsg+nTotal-nBuffer, nBuffer);
    fTemp.Close();

	delete[] pMsg;
    delete[] pName;

    // Call this function to send an acknowledge code to the host computer 
    // after receiving a download process program request.
    m_pGemStation->SendPPAck(ACK7); // S7F4
	//	Process Program Acknowledge
    // ACK7 - Accepted
    // ACK7_PERMISSION_NOT_GRANT - Permission not granted.
    // ACK7_LENGTH_ERROR - Length Error
    // ACK7_MATRIX_OVERFLOW - Matrix overflow
    // ACK7_PPID_NOT_FOUND - PPID not found
    // ACK7_MODE_UNSUPPORTED - Mode unsupported

    return 1;
*/
}

//================================================================
// Function Name: 		SG_CmdHostUploadPPG
// Input arguments:     filename (PPID)
// Output arguments:	Null
// Description:   		First this is registered in callback function
//						Callback from ASMGEM when the S7F6 (Process Program Request) is
//						received from the host. 
// Return:				GEMOK or GEMERR
// Remarks:				S7F5 called By GEM/SECS Library
//================================================================
LONG CSecsComm::SG_CmdHost_EquipUploadPPG(IPC_CServiceMessage& svMsg) // UPLOAD_PROCESS_PROG S7F5
{
    if( IsSecsGemInit()==FALSE )
        return -1;
    INT len = svMsg.GetMsgLen();
    char *pPPID = new char[len];
    svMsg.GetMsg(len, pPPID);
    CString szPPFile;
    szPPFile = m_szPPFilePath+"\\"+pPPID;

	CFile fTemp;
    if( fTemp.Open(szPPFile, CFile::modeRead)==TRUE )
    {
        UINT ullSize = (UINT)fTemp.GetLength();
        char *pBuffer = new char[ullSize];
        fTemp.SeekToBegin();
        fTemp.Read(pBuffer, ullSize);
        fTemp.Close();
        m_pGemStation->UploadProcessProgram(pPPID, ullSize, pBuffer); // S7F6
//****SSSS
        delete[] pBuffer;
    }
    else
    {
        m_pGemStation->UploadProcessProgram(pPPID, 0, NULL);
    }
//**** SSSS
    delete[] pPPID;

    // DownloadPPToHost ???
    // one party to upload a process program to the other party after receiving a
    // call back function to request for process program upload.

    return 1;
}




LONG CSecsComm::SG_CmdDeletePPG(IPC_CServiceMessage& svMsg) // S7F17
{
    if( IsSecsGemInit()==FALSE )
        return -1;

    INT len = svMsg.GetMsgLen();
    char *pPPID = new char[len];
    svMsg.GetMsg(len, pPPID);
    CString szPPFile;

    INT i=0, sLen;
    CFileStatus rStatus;
    while( 1 )
    {
        sLen = (INT)strlen(pPPID+i);
        szPPFile = m_szPPFilePath+"\\"+(pPPID+i);
        if( CFile::GetStatus(szPPFile, rStatus)!=FALSE )
            CFile::Remove(szPPFile);
        i +=(sLen+1);
        if( i>=len )
            break;
    }

    // Call this function to send an acknowledge code to the host computer 
    // after receiving a delete process program request by the Cell Controller.
	m_pGemStation->SendDeletePPAck(ACK7); // S7F18 
//**********SSSS
	delete[] pPPID;

    return 1;
}



LONG CSecsComm::SC_CmdDeletePPG(IPC_CServiceMessage& svMsg)
{
    if( IsSecsGemInit()==FALSE )
        return -1;

    CStringList List;
    List.AddTail("abc");
    List.AddTail("bcd");
    List.AddTail("cde");
    m_pGemStation->DeleteProcessProgram(List); // S7F17

    return 1;
}

LONG CSecsComm::SG_CmdHostDeletePPIDAck(IPC_CServiceMessage& svMsg) // S7F18
{
    UCHAR ucChar[2];
    svMsg.GetMsg(sizeof(ucChar), ucChar);

    return 1;
}

LONG CSecsComm::SG_CmdDirectoryPPG(IPC_CServiceMessage& svMsg) // S7F19
{
    if( IsSecsGemInit()==FALSE )
        return -1;

    CStringList List;
    WIN32_FIND_DATA a;
    CString szPPFile;
    szPPFile = m_szPPFilePath+"\\*.*";
    HANDLE h;
    h = FindFirstFile(szPPFile, &a);
    if( h!=INVALID_HANDLE_VALUE )
    {
        if( strcmp(a.cFileName,".")!=0 && strcmp(a.cFileName, "..")!=0 )
            List.AddTail(a.cFileName);
        while( FindNextFile(h, &a) )
        {
            if( strcmp(a.cFileName,".")!=0 && strcmp(a.cFileName, "..")!=0 )
                List.AddTail(a.cFileName);
        }
        FindClose(h);
    }
	m_pGemStation->PPDirectory(List); // S7F20

    return 1;
}

LONG CSecsComm::SC_CmdRequestPPDirectory(IPC_CServiceMessage &svMsg)
{
    if( IsSecsGemInit()==FALSE )
        return -1;

    m_pGemStation->RequestPPDirectory(); // S7F19

    return 1;
}

LONG CSecsComm::SG_CmdReceiveRequestEPPD(IPC_CServiceMessage& svMsg) // S7F20
{
    INT len = svMsg.GetMsgLen();
    char *pPPID = new char[len];
    svMsg.GetMsg(len, pPPID);
//******* SSSS
	delete[] pPPID;

    return 1;
}


LONG CSecsComm::SC_CmdGetPPFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szPPFilePath);
		bReturn = TRUE;
        m_szPPFilePath_HMI = m_szPPFilePath;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

