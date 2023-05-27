#include "StdAfx.h"
#include "GallerySearchThread.h"
#include "PrescanUtility.h"
#include "TakeTime.h"
#include "MS896A.h"

CCriticalSection CGallerySearchThread::m_csPostThreadMessage;

IMPLEMENT_DYNCREATE(CGallerySearchThread, CWinThread)

CGallerySearchThread::CGallerySearchThread()
{
	m_bAutoDelete = FALSE;
}

CGallerySearchThread::~CGallerySearchThread()
{
}

DECLARE_USER_MESSAGE(UWM_SETUP_SEARCH_COMMAND)
DECLARE_USER_MESSAGE(UWM_SETUP_SEARCH_COMMAND2)
DECLARE_USER_MESSAGE(UWM_GALLERY_SEARCH_DONE)
DECLARE_USER_MESSAGE(UWM_GALLERY_START_SEARCH)
DECLARE_USER_MESSAGE(UWM_SHUTDOWN)

BEGIN_MESSAGE_MAP(CGallerySearchThread, CWinThread)
ON_REGISTERED_THREAD_MESSAGE(UWM_SETUP_SEARCH_COMMAND,	OnSetupSearchCommand)
ON_REGISTERED_THREAD_MESSAGE(UWM_SETUP_SEARCH_COMMAND2,	OnSetupSearchCommand2)
ON_REGISTERED_THREAD_MESSAGE(UWM_GALLERY_START_SEARCH,	OnGalleryStartSearch)
ON_REGISTERED_THREAD_MESSAGE(UWM_SHUTDOWN,				OnShutDown)
END_MESSAGE_MAP()


BOOL CGallerySearchThread::InitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	PR_InitSrchDieCmd(&m_command);
	PR_InitSrchDieCmd(&m_SrchCmd2);
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	SetThreadPriority(THREAD_PRIORITY_HIGHEST);
	m_ulCurrImageID = 0;
	m_lSubImgCurIdx	= 1;
	m_replies.m_wSearchErrors = 0;

	return TRUE;
}

int CGallerySearchThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

void CGallerySearchThread::SetInfo(CWinThread *controller, int senderID, int receiverID)
{
	m_ulCurrImageID	= 0;
	m_lSubImgCurIdx	= 1;
	m_controller	= controller;
	m_senderID		= senderID;
	m_receiverID	= receiverID;
}

void CGallerySearchThread::OnSetupSearchCommand(WPARAM wParam, LPARAM lParam)
{
	PR_SRCH_DIE_CMD *command = (PR_SRCH_DIE_CMD*)wParam;
	m_command = *command;
}

void CGallerySearchThread::OnSetupSearchCommand2(WPARAM wParam, LPARAM lParam)
{
	PR_SRCH_DIE_CMD *command = (PR_SRCH_DIE_CMD*)wParam;
	m_SrchCmd2 = *command;
}


void CGallerySearchThread::OnGalleryStartSearch(WPARAM wSrchParam, LPARAM lParam)
{
	DOUBLE dStartTime = GetTime();
	SearchAreaInfo		*info = (SearchAreaInfo*)wSrchParam;
	CMSPrescanUtility	*pUtl = CMSPrescanUtility::Instance();
	CMS896AApp			*pApp = (CMS896AApp*) AfxGetApp();
	CString szMsg;

	m_replies.ResetReplies();

	int nImageID	= info->m_imageID;
	int nStationID	= info->m_stationID;
	ULONG ulPrGrabID = info->m_PrGrabID;

	BOOL doInsp = (m_command.emDefectInsp==PR_TRUE);
	BOOL doPost = (m_command.emPostBondInsp==PR_TRUE);

	BOOL bSearchError = FALSE;
	CString szSrchWin, szSrchCmd, szRpy1Cmd, szRpy2Cmd, szRpy3Cmd, szRpy4Cmd;

	if( (m_ulCurrImageID!=nImageID) || (info->m_bSubImgEnable) || 
		(info->m_bSubImgEnable && info->m_lSubImgTgtIdx!=m_lSubImgCurIdx) )
	{
		PR_SRCH_DIE_CMD		stInspCmd;
		m_ulCurrImageID = nImageID;
		stInspCmd = m_command;
		if( ulPrGrabID==2 )
		{
			stInspCmd =	m_SrchCmd2;
		}

		if( info->m_bSubImgEnable )
		{
			PR_WORD lULX = info->m_wSubImgULX;
			PR_WORD lULY = info->m_wSubImgULY;
			PR_WORD lLRX = info->m_wSubImgLRX;
			PR_WORD lLRY = info->m_wSubImgLRY;
			PR_WORD lCTX = (lULX + lLRX)/2;
			PR_WORD lCTY = (lULY + lLRY)/2;
			stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = lULX;
			stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = lULY;
			stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = lLRX;
			stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = lLRY;
			stInspCmd.stDieAlign.stStreet.coProbableDieCentre.x		 = lCTX;
			stInspCmd.stDieAlign.stStreet.coProbableDieCentre.y		 = lCTY;
			m_lSubImgCurIdx	= info->m_lSubImgTgtIdx;
			szSrchWin.Format("sd %d PR SrchDieCmd window Sub Image Idx %d, Corner %d,%d -- %d,%d", m_senderID,
				m_lSubImgCurIdx, info->m_wSubImgULX, info->m_wSubImgULY, info->m_wSubImgLRX, info->m_wSubImgLRY);
			S_PrescanCycleLog(szSrchWin);
		}

		stInspCmd.ulLatchImageID	= nImageID;
		stInspCmd.uwLatchStationID	= nStationID;

		szSrchCmd.Format("sd %d(%d) PR SrchDieCmd imgid %d, pstn %d, prindex %d; T %ld",	
			m_senderID, m_receiverID, nImageID, nStationID, ulPrGrabID, (LONG)(GetTime()-dStartTime));
		S_PrescanCycleLog(szSrchCmd);
		stInspCmd.stDieAlign.stStreet.emPartialDieSrch	= PR_FALSE;	// PR_TRUE
		stInspCmd.stDieAlign.st2Points.emPartialDieSrch	= PR_FALSE;	// PR_TRUE
		stInspCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
		stInspCmd.uwHostMallocMaxNoOfDieInRpy2			= WPR_ONE_SEARCH_MAX_DIE;
		if( doInsp )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy3		= WPR_ONE_SEARCH_MAX_DIE;
		if( doPost )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy4		= WPR_ONE_SEARCH_MAX_DIE; 
		PR_SrchDieCmd(			&stInspCmd, m_senderID, m_receiverID, &m_replies.m_reply1.uwCommunStatus);
		if (m_replies.m_reply1.uwCommunStatus != PR_COMM_NOERR )
		{
			CString szTemp;
			szTemp.Format(" comm %d", m_replies.m_reply1.uwCommunStatus);
			szSrchCmd += szTemp;		bSearchError = TRUE;	//	srch die comm error
		}

		szRpy1Cmd.Format("sd %d, srch die rpy 1 T %ld", m_senderID, (LONG)(GetTime()-dStartTime));
		S_PrescanCycleLog(szRpy1Cmd);
		PR_SrchDieRpy1(m_senderID, &m_replies.m_reply1);
		if (m_replies.m_reply1.uwCommunStatus != PR_COMM_NOERR)
		{
			CString szTemp;
			szTemp.Format(" comm %d", m_replies.m_reply1.uwCommunStatus);
			szRpy1Cmd += szTemp;		bSearchError = TRUE;	//	rpy1 comm error
		}
		if ( PR_ERROR_STATUS(m_replies.m_reply1.uwPRStatus) )
		{
			CString szTemp;
			szTemp.Format(" status %d", m_replies.m_reply1.uwPRStatus);
			szRpy1Cmd += szTemp;		bSearchError = TRUE;	//	rpy1 stat error
		}
	}

	szRpy2Cmd.Format("sd %d, srch rpy2, img %d, sub win %d T %ld",
		m_senderID, nImageID, m_lSubImgCurIdx, (LONG)(GetTime()-dStartTime));
	S_PrescanCycleLog(szRpy2Cmd);

	m_replies.m_reply2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*)malloc (WPR_ONE_SEARCH_MAX_DIE*sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
	if( m_replies.m_reply2.pstDieAlign == NULL )
	{
		szMsg = "PR abnormal Thread malloc pstDieAlign is null";
		SaveScanTimeEvent(szMsg);
		pApp->SetErrorMessage(szMsg);
		pUtl->SetPrAbnormal(TRUE, szMsg);	// memory lack
	}
	if( doInsp )
	{
		m_replies.m_reply3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*)malloc(WPR_ONE_SEARCH_MAX_DIE*sizeof(PR_DIE_INSP_MINIMAL_RPY));
	}
	if( doPost )
	{
		m_replies.m_reply4.pstPostBondInspMin = (PR_POST_INSP_MINIMAL_RPY*)malloc(WPR_ONE_SEARCH_MAX_DIE*sizeof(PR_POST_INSP_MINIMAL_RPY));
	}

	PR_SrchDieRpy2(m_senderID, &m_replies.m_reply2);
	BOOL bNotLastDie	= PR_IsLastReply(&m_replies.m_reply2.stStatus)!=PR_TRUE;

	PR_UWORD uwRpy2Comm = m_replies.m_reply2.stStatus.uwCommunStatus;
	if( uwRpy2Comm!=PR_COMM_NOERR )
	{
		szMsg.Format("PRY2 PR abnormal comm error %d img %d, sub stn %d done", 
			uwRpy2Comm, nImageID, m_senderID);
		S_PrescanCycleLog(szMsg);
		SaveScanTimeEvent(szMsg);
		CString szTemp;
		szTemp.Format(" comm %d", uwRpy2Comm);
		szRpy2Cmd += szTemp;		bSearchError = TRUE;	//	rpy2 comm error
	}

	PR_UWORD uwRpy2Stat = m_replies.m_reply2.stStatus.uwPRStatus;
	if( PR_ERROR_STATUS(uwRpy2Stat) )
	{
		if(	uwRpy2Stat!=34	&&	//	 34: PR_ERR_NO_DIE, Object Not Found
			uwRpy2Stat!=98	&&	//	 98: PR_ERR_ROTATION_EXCEED_SPEC, Rotation Angle Out of Specification
			uwRpy2Stat!=268 &&	//	268: street or such like this.
			uwRpy2Stat!=266	&&	//	266: PR_ERR_OUT_OF_SEARCH_RANGE, Object out of Search Region
			uwRpy2Stat!=265	&&	//	265: PR_ERR_OUT_OF_SEARCH_RANGE, Object out of Search Region
			uwRpy2Stat!=264 &&	//	264: PR_ERR_SCORE_OUT_OF_SPEC, Search Score rejected
			uwRpy2Stat!= 62)	//	
		{
			m_replies.m_cmdCommStatus = 1;
			CString szTemp;
			szTemp.Format(" status %d", uwRpy2Stat);
			szRpy2Cmd += szTemp;		bSearchError = TRUE;	//	rpy2 stat error
			szMsg.Format("PRY2 PR abnormal status error %d img %d, sub stn %d done", 
				uwRpy2Stat, nImageID, m_senderID);
			SaveScanTimeEvent(szMsg);
		}	//	check status
	}

	if( ulPrGrabID==2 )
	{
		doInsp = (m_SrchCmd2.emDefectInsp==PR_TRUE);
		doPost = (m_SrchCmd2.emPostBondInsp==PR_TRUE);
	}

	if( bNotLastDie && (doInsp || doPost) )
	{
		szRpy3Cmd.Format("sd %d, srch rpy3 T %ld", m_senderID, (LONG)(GetTime()-dStartTime));
		S_PrescanCycleLog(szRpy3Cmd);
		PR_SrchDieRpy3(m_senderID, &m_replies.m_reply3);	// inspection result
		PR_UWORD uwRpy3Comm = m_replies.m_reply3.stStatus.uwCommunStatus;
		PR_UWORD uwRpy3Stat = m_replies.m_reply3.stStatus.uwPRStatus;
		if (uwRpy3Comm != PR_COMM_NOERR)
		{
			CString szTemp;
			szTemp.Format(" comm %d", uwRpy3Comm);
			szRpy3Cmd += szTemp;		bSearchError = TRUE;	//	rpy3 comm error
		}
		if( PR_ERROR_STATUS(uwRpy3Stat) )
		{
			CString szTemp;
			szTemp.Format(" state %d", uwRpy3Stat);
			szRpy3Cmd += szTemp;		bSearchError = TRUE;	//	rpy3 state error
		}
		bNotLastDie	= PR_IsLastReply(&m_replies.m_reply3.stStatus)!=PR_TRUE;
		if( bNotLastDie && doPost )
		{
			szRpy4Cmd.Format("sd %d, srch rpy4 T %ld", m_senderID, (LONG)(GetTime()-dStartTime));
			S_PrescanCycleLog(szRpy4Cmd);
			PR_SrchDieRpy4(m_senderID, &m_replies.m_reply4);	// badcut detection
			PR_UWORD uwRpy4Comm = m_replies.m_reply4.stStatus.uwCommunStatus;
			PR_UWORD uwRpy4Stat = m_replies.m_reply4.stStatus.uwPRStatus;
			if (uwRpy4Comm != PR_COMM_NOERR)
			{
				CString szTemp;
				szTemp.Format(" comm %d", uwRpy4Comm);
				szRpy4Cmd += szTemp;		bSearchError = TRUE;	//	rpy4 comm error
			}
			if( PR_ERROR_STATUS(uwRpy4Stat) )
			{
				CString szTemp;
				szTemp.Format(" state %d", uwRpy4Stat);
				szRpy4Cmd += szTemp;		bSearchError = TRUE;	//	rpy4 state error
			}
		}
	}

	USHORT usDieSum = m_replies.m_reply2.uwNResults;
	if ( m_replies.m_reply2.pstDieAlign != NULL)
	{
		memcpy(m_replies.m_stDieAlign, m_replies.m_reply2.pstDieAlign, WPR_ONE_SEARCH_MAX_DIE*sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
		for(USHORT usIndex=0; usIndex<usDieSum; usIndex++)
		{
			PR_RCOORD rcDieCtr	= m_replies.m_reply2.pstDieAlign[usIndex].rcoDieCentre;
			if( rcDieCtr.x<0 || rcDieCtr.x>PR_MAX_COORD || rcDieCtr.y<0 || rcDieCtr.y>PR_MAX_COORD )
			{
				szMsg.Format("PR abnormal thread %d in image %d, at (%f,%f) total %d", 
					m_senderID, nImageID, rcDieCtr.x, rcDieCtr.y, usDieSum);
				SaveScanTimeEvent(szMsg);
				bSearchError = TRUE;	//	PR return pixel position invalid
			}
		}
		free(m_replies.m_reply2.pstDieAlign);
	}
	else
	{
		szMsg = "PR abnormal reply 2 is null";
		SaveScanTimeEvent(szMsg);
		pApp->SetErrorMessage(szMsg);
		pUtl->SetPrAbnormal(TRUE, szMsg);	//	search rpy2 pointer is null
	}

	if( doInsp )
	{
		memcpy(m_replies.m_stDieInspc, m_replies.m_reply3.pstDieInspExtMin, WPR_ONE_SEARCH_MAX_DIE*sizeof(PR_DIE_INSP_MINIMAL_RPY));
		if ( m_replies.m_reply3.pstDieInspExtMin != NULL)
		{
			free(m_replies.m_reply3.pstDieInspExtMin);
		}
	}

	if( doPost )
	{
		memcpy(m_replies.m_stPostBond, m_replies.m_reply4.pstPostBondInspMin, WPR_ONE_SEARCH_MAX_DIE*sizeof(PR_POST_INSP_MINIMAL_RPY));
		if ( m_replies.m_reply4.pstPostBondInspMin != NULL)
		{
			free(m_replies.m_reply4.pstPostBondInspMin);
		}
	}	// post bond, bad cut.

	szMsg.Format("sd %d to rtn 2 found die %d used time %d", m_senderID, usDieSum, (LONG)(GetTime()-dStartTime));
	S_PrescanCycleLog(szMsg);

	if( bSearchError )
	{
		m_replies.m_reply2.uwNResults = 0;
		SaveScanTimeEvent(szSrchWin);
		SaveScanTimeEvent(szSrchCmd);
		SaveScanTimeEvent(szRpy1Cmd);
		SaveScanTimeEvent(szRpy2Cmd);
		if( doInsp || doPost )
		{
			SaveScanTimeEvent(szRpy3Cmd);
			SaveScanTimeEvent(szRpy4Cmd);
		}
		UINT unAutoTry = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Auto Retry for scan PR error"), 0);
		if( unAutoTry==0 )
		{
			pUtl->SetPrAbnormal(TRUE, szRpy2Cmd);	// search rpy1/2/3/4 com and state error
		}
		else
		{
			m_replies.m_wSearchErrors++;
			for(INT i=0; i<3; i++)
			{
				Sleep(1000);

				CString szErrMsg;
				LONG lRet = pApp->GetPrGeneral()->DrawHomeCursor(m_senderID, m_receiverID, 4096, 4096, szErrMsg);
				if (lRet != 0)
				{
					szMsg.Format("sd %d ", m_senderID);
					szMsg += szErrMsg;
					SaveScanTimeEvent(szMsg);
					continue;
				}
				break;
			}
		}
	}
	else
	{
		m_replies.m_wSearchErrors = 0;
	}


	szMsg.Format("PostThreadMessage nImageID = %d, nStationID = %d, threadID = %d, DieNumOfResult = %d", nImageID, nStationID, info->m_threadID, m_replies.m_reply2.uwNResults);
	pUtl->ScanImageHistory(szMsg);
	m_csPostThreadMessage.Lock();
	m_controller->PostThreadMessage(UWM_GALLERY_SEARCH_DONE, wSrchParam, (LPARAM)&m_replies);
	m_csPostThreadMessage.Unlock();
}


void CGallerySearchThread::OnShutDown(WPARAM wParam, LPARAM lParam)
{
	AfxEndThread(0);
}

void CGallerySearchThread::S_PrescanCycleLog(CString szMsg)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetVisionThrdLog() )
		OutputDebugString("       S " + szMsg);

	if( pUtl->GetPrAbnormal() )
	{
		SaveScanTimeEvent(szMsg);
	}
}
