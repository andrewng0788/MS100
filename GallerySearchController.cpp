#include "stdafx.h"
#include "MS896A.h"
#include "GallerySearchController.h"
#include "GallerySearchThread.h"
#include "prheader.h"
#include "UWM.h"
#include "WaferPR.h"
#include "LastScanPosition.h"
#include "PrescanInfo.h"
#include "PrescanUtility.h"
#include "PRFailureCaseLog.h"
#include "PrZoomSensor.h"

#define M_RUNTIME_CLASS_T1(class_name, T1) ((CRuntimeClass*)(&class_name<T1>::class##class_name))

#define M_IMPLEMENT_RUNTIMECLASS_T1(class_name, T1, base_class_name, wSchema, pfnNew, class_init) \
	AFX_COMDAT const CRuntimeClass class_name<T1>::class##class_name = { \
		#class_name, sizeof(class class_name<T1>), wSchema, class_name<T1>::##pfnNew, \
			&base_class_name::GetThisClass, NULL, class_init }; \
	CRuntimeClass* PASCAL class_name<T1>::GetThisClass() \
		{ return M_RUNTIME_CLASS_T1(class_name, T1); } \
	CRuntimeClass* class_name<T1>::GetRuntimeClass() const \
		{ return M_RUNTIME_CLASS_T1(class_name, T1); }

#define M_IMPLEMENT_DYNCREATE_T1(class_name, T1, base_class_name) \
	template class class_name<T1>; \
	CObject* PASCAL class_name<T1>::CreateObject() \
		{ return new class_name<T1>; } \
	M_IMPLEMENT_RUNTIMECLASS_T1(class_name, T1, base_class_name, 0xFFFF, CreateObject, NULL )

//	Sammy Ma
/*
2002
#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
      const AFX_MSGMAP* PASCAL theClass::GetThisMessageMap() \
           { return &theClass::messageMap; } \
      const AFX_MSGMAP* theClass::GetMessageMap() const \
            { return &theClass::messageMap; } \
      AFX_COMDAT const AFX_MSGMAP theClass::messageMap = \
      { &baseClass::GetThisMessageMap, &theClass::_messageEntries[0] }; \
      AFX_COMDAT const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
      { \

#define M_BEGIN_MESSAGE_MAP_T1(theClass, T1, baseClass) \
	const AFX_MSGMAP* PASCAL theClass<T1>::GetThisMessageMap() \
		{ return &theClass<T1>::messageMap; } \
	const AFX_MSGMAP* theClass<T1>::GetMessageMap() const \
		{ return &theClass<T1>::messageMap; } \
	AFX_COMDAT const AFX_MSGMAP theClass<T1>::messageMap = \
	{ &baseClass::GetThisMessageMap, &theClass<T1>::_messageEntries[0] }; \
	AFX_COMDAT const AFX_MSGMAP_ENTRY theClass<T1>::_messageEntries[] = \
	{ \


2010
 #define BEGIN_MESSAGE_MAP(theClass, baseClass) \
       PTM_WARNING_DISABLE \
      const AFX_MSGMAP* theClass::GetMessageMap() const \
            { return GetThisMessageMap(); } \
      const AFX_MSGMAP* PASCAL theClass::GetThisMessageMap() \
      { \
           typedef theClass ThisClass;                                    \
		   typedef baseClass TheBaseClass;                                \
            static const AFX_MSGMAP_ENTRY _messageEntries[] =  \
            {

*/
  #define M_BEGIN_MESSAGE_MAP_T1(theClass, T1, baseClass) \
       PTM_WARNING_DISABLE \
		template <typename T1>		\
		const AFX_MSGMAP* theClass<T1>::GetMessageMap() const \
            { return GetThisMessageMap(); } \
		template <typename T1>		\
		const AFX_MSGMAP* PASCAL theClass<T1>::GetThisMessageMap() \
		{ \
           typedef theClass<T1> ThisClass;                           \
		   typedef baseClass TheBaseClass;                       \
           static const AFX_MSGMAP_ENTRY _messageEntries[] =  \
            {


M_IMPLEMENT_DYNCREATE_T1(CGallerySearchController, CWaferPr, CWinThread)
//IMPLEMENT_DYNCREATE(CGallerySearchController, CWaferPr)


template<class PRStn>
CGallerySearchController<PRStn>::CGallerySearchController()
{
	m_bAutoDelete = FALSE;
}

template<class PRStn>
CGallerySearchController<PRStn>::~CGallerySearchController()
{
}

template<class PRStn>
BOOL CGallerySearchController<PRStn>::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	m_running = true;
	m_nUsedSrchThread = MAX_PR_GALLERY_SEARCH_THREADS;
	for (int i = 0; i < MAX_PR_GALLERY_SEARCH_THREADS; ++i)
	{
		m_searchThread[i] = AfxBeginThread(RUNTIME_CLASS(CGallerySearchThread), THREAD_PRIORITY_NORMAL);

		dynamic_cast<CGallerySearchThread*>(m_searchThread[i])->SetInfo(this, GALLERY_SEARCH_BASE_SID+i, GALLERY_SEARCH_BASE_RID+i);
		m_bIsThreadIdle[i] = true;
		m_stSrchInfo[i].m_threadID = i;
		m_ulThreadImageID[i] = 0;
		m_lSubImgCrlIdx[i]	= 1;
		m_ulImageDieSum[i]	= 0;
		m_stSrchInfo[i].m_lSubImgTgtIdx = 1;
		m_stSrchInfo[i].m_bSubImgEnable	= false;
	}

	SetGrabImageLimit(0);

	m_uwSubImgTotalNum		= 1;
	m_nCtrlSenderID			= m_prStation->GetScnSenID();
	m_nCtrlReceiverID		= m_prStation->GetScnRecID();
	m_bSoraaAOI				= false;
	m_ul1stGrabImageID		= 0;
	m_ul2ndGrabImageID		= 0;
	m_dShareImageFocusScore	= -1.0;

	srand( (unsigned)time( NULL ) );

	SetThreadPriority(THREAD_PRIORITY_HIGHEST);
	return TRUE;
}

template<class PRStn>
int CGallerySearchController<PRStn>::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

DECLARE_USER_MESSAGE(UWM_SETUP_SEARCH_COMMAND)
DECLARE_USER_MESSAGE(UWM_SETUP_SEARCH_COMMAND2)
DECLARE_USER_MESSAGE(UWM_GALLERY_START_SEARCH)
DECLARE_USER_MESSAGE(UWM_GALLERY_SEARCH_DONE)
DECLARE_USER_MESSAGE(UWM_SHUTDOWN)

M_BEGIN_MESSAGE_MAP_T1(CGallerySearchController, CWaferPr, CWinThread)
//BEGIN_MESSAGE_MAP(CGallerySearchController, CWaferPr)
ON_REGISTERED_THREAD_MESSAGE(UWM_GALLERY_SEARCH_DONE, OnGallerySearchDone)
END_MESSAGE_MAP()

// CGallerySearchController message handlers


template<class PRStn>
void CGallerySearchController<PRStn>::SetPRStation(PRStn *parent)
{
	m_prStation = parent;
}

template<class PRStn>	void CGallerySearchController<PRStn>::DeleteRemainImages()
{
	if( IsIdle() || GalleryImageNumInMemory()<=0 )
	{
		return ;
	}

	C_PrescanCycleLog("  C delete remain images");
	SaveScanTimeEvent("WPR Remove all share images");
	// pause PR first, wait until all threads are idle, then delete the list
	CSingleLock slCsLock(&m_csQueueMutex);
	slCsLock.Lock();

	m_running = true;
	slCsLock.Unlock();

	bool idle = false;
	int i;
	LONG lLoopCount = 0;
	do
	{
		idle = m_bIsThreadIdle[0];
		for (i = 1; i < GetSrchThreadNum() && idle; ++i)
		{
			idle &= m_bIsThreadIdle[i];
		}
		if (!idle)
			Sleep(20);
		lLoopCount++;
		if( lLoopCount>10000 )
		{
			break;
		}
	}
	while (!idle);

	slCsLock.Lock();
	m_toBeProcessed.clear();
	m_running = true;
	slCsLock.Unlock();

	GalleryDeleteImageCmd(0, PR_TRUE);
	for (int i = 0; i < MAX_PR_GALLERY_SEARCH_THREADS; ++i)
	{
		m_bIsThreadIdle[i]	 = true;
		m_ulThreadImageID[i] = 0;
		m_ulImageDieSum[i]	 = 0;
		m_stSrchInfo[i].m_lSubImgTgtIdx = 1;
	}

	CString szLogText;
	szLogText.Format("  C Delete all Image");
	C_PrescanCycleLog(szLogText, TRUE);
}

template<class PRStn>	bool CGallerySearchController<PRStn>::RecoverScanChannels()
{
	CString szMsg;
	if( IsIdle()==FALSE )
	{
		szMsg.Format("Recover scan channels not in idle");
		SaveScanTimeEvent(szMsg);
		return false;
	}

	bool bReturn = true;
	m_prStation->OpenWaitingAlert();
	Sleep(10000);
	PR_COORD stCenter;
	stCenter.x = 2048;
	stCenter.y = 4096;
	int nThreadNum = GetSrchThreadNum();
	for (int i = 0; i < nThreadNum; ++i)
	{
		bool bLoopTry = false;
		for(int j=0; j<5; j++)
		{
			INT uSendID = GALLERY_SEARCH_BASE_SID+i;
			INT uRecvID = GALLERY_SEARCH_BASE_RID+i;

			stCenter.x = 2048 + i*500;
			szMsg.Format("Recover scan channel S %d, R %d; PR draw home cursor at %d,%d",
			uSendID, uRecvID, stCenter.x, stCenter.y);
			SaveScanTimeEvent(szMsg);

			CString szErrMsg;
			LONG lRet = m_prStation->m_pPrGeneral->DrawHomeCursor(uSendID, uRecvID, stCenter.x, stCenter.y, szErrMsg);
			if (!lRet)
			{
				szMsg = _T("Recover scan channel ") + szErrMsg;
				SaveScanTimeEvent(szMsg);
				continue;
			}
			bLoopTry = true;
			break;
		}
		bReturn = bReturn && bLoopTry;
		if( bReturn==FALSE )
		{
			break;
		}
	}

	m_prStation->CloseWaitingAlert();

	return bReturn;
}

template<class PRStn>
void CGallerySearchController<PRStn>::ResumePR()
{
	CSingleLock slCsLock(&m_csQueueMutex);
	slCsLock.Lock();
	m_running = true;
	C_PrescanCycleLog("  C Resume PR");

	while( 1 )
	{
		if( IsIdle() )
			break;
		for (int i = 0; i < GetSrchThreadNum(); ++i)
		{
			AutoMultiSendSearchCommand(i);
		}
	}
}

template<class PRStn>
void CGallerySearchController<PRStn>::PartialResumePR()
{
	if( m_prStation->IsStitchMode() )
	{
		CSingleLock slCsLock(&m_csQueueMutex);
		slCsLock.Lock();
	//	m_running = true;
		BOOL bAllIdle = TRUE;
		for (int i = 0; i < GetSrchThreadNum(); ++i)
		{
			bAllIdle = bAllIdle && m_bIsThreadIdle[i];
		}
		if( bAllIdle )
		{
			C_PrescanCycleLog("  C Partial Resume PR");
			AutoMultiSendSearchCommand(0);
		}
	}
}

template<class PRStn>
void CGallerySearchController<PRStn>::RemoveAllShareImages()
{
	SaveScanTimeEvent("WPR Remove all share images");
	// pause PR first, wait until all threads are idle, then delete the list
	CSingleLock slCsLock(&m_csQueueMutex);
	slCsLock.Lock();

	m_running = true;
	slCsLock.Unlock();

	bool idle = false;
	int i;
	do
	{
		idle = m_bIsThreadIdle[0];
		for (i = 1; i < GetSrchThreadNum() && idle; ++i)
		{
			idle &= m_bIsThreadIdle[i];
		}
		if (!idle)
			Sleep(2);
	}
	while (!idle);

	slCsLock.Lock();
	m_toBeProcessed.clear();
	m_running = true;
	slCsLock.Unlock();

	GalleryDeleteImageCmd(0, PR_TRUE);
	CString szLogText;
	szLogText.Format("  C Delete all Image");
	C_PrescanCycleLog(szLogText, TRUE);
}

template<class PRStn>
void CGallerySearchController<PRStn>::ShutDown()
{
	HANDLE threads[MAX_PR_GALLERY_SEARCH_THREADS];
	for (int i = 0; i < MAX_PR_GALLERY_SEARCH_THREADS; ++i)
	{
		threads[i] = m_searchThread[i]->m_hThread;
		m_searchThread[i]->PostThreadMessage(UWM_SHUTDOWN, 0, 0);
	}
	  // wait for threads to exit
	::WaitForMultipleObjects(MAX_PR_GALLERY_SEARCH_THREADS, threads, true, INFINITE);
	TRACE(_T("CPRController shut down\n"));
	AfxEndThread(0);
}

template<class PRStn>
BOOL CGallerySearchController<PRStn>::GrabShareImage(LONG lFrameCol, LONG lFrameRow, int encX, int encY, long ulRow, long ulCol)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrAbnormal() )
	{
		return FALSE;
	}

	TakeTime((TIME_ENUM)SCAN_WPR_FOCUS);	// PR wants to grab, maybe it is deleting image, then block next action.
	C_PrescanCycleLog("  C share grab function begin");

	m_csGrab.Lock();	//	normal grab
	TakeTime((TIME_ENUM)SCAN_WPR_IMG_GRAB);	// prescan, begin to grab share image

	ULONG ulGrabTimes = 1;
	if( m_prStation->IsPrecanWith2Pr() )
	{
		ulGrabTimes = 2;
	}

	LONG lIndexDelayTime = m_prStation->m_lWftScanIndexAndDelayTime;
	DOUBLE dGrabRpy1Time = GetTime();

for(ULONG ulGrabCnt = 0; ulGrabCnt<ulGrabTimes; ulGrabCnt++)
{
	if( ulGrabCnt!=0 )
	{
		LONG lUsedTime = (LONG)(GetTime() - dGrabRpy1Time);
		if( lUsedTime<lIndexDelayTime )
		{
			Sleep(lIndexDelayTime-lUsedTime);
		}
	}
	PR_GRAB_SHARE_IMAGE_CMD		cmd;
	PR_GRAB_SHARE_IMAGE_RPY		rpy1;
	PR_InitGrabShareImgCmd(&cmd);
	cmd.emPurpose			= m_prStation->GetScnPurpose();
	cmd.emGrabFromRecord	= PR_TRUE;
	cmd.emOnTheFlyGrab		= PR_FALSE;
	cmd.emGrabMode			= PR_GRAB_MODE_NORMAL;
 	cmd.uwRecordID			= m_stSrchCmd.auwRecordID[0];
	if( ulGrabCnt==1 )
		cmd.uwRecordID			= m_stSrchCmd2.auwRecordID[0];
	CString szMsg;
	szMsg.Format("  C init share grab PR id %d", cmd.uwRecordID);
	C_PrescanCycleLog(szMsg);

	PR_GrabShareImgCmd(&cmd, m_nCtrlSenderID, m_nCtrlReceiverID, &rpy1);
	C_PrescanCycleLog("  C grab share rpy1");

	TakeTime((TIME_ENUM)SCAN_WPR_IMG_RPY1);	// prescan share rpy1, die ready, trigger wft move, ES101
	if (PR_COMM_NOERR != rpy1.uwCommunStatus)
	{
		CString szLogText;
		szLogText.Format("(PR_GrabShareImgCmd) PR abnormal C Grab share img rpy1 error = %d", rpy1.uwCommunStatus);
		C_PrescanCycleLog(szLogText);
		SaveScanTimeEvent(szLogText);
		m_prStation->SetErrorMessage(szLogText);
		pUtl->SetPrAbnormal(TRUE, szLogText);	// grab rpy1 error.
		m_csGrab.Unlock();	//	grab rpy1 error
		m_prStation->HmiMessage_Red_Back(szLogText, "Alarm");
		return FALSE;
	}

	if ((ulGrabCnt+1) == ulGrabTimes)
	{
		m_prStation->SetDieReady(TRUE);
	}

	dGrabRpy1Time = GetTime();

	PR_GRAB_SHARE_IMAGE_RPY2	rpy2;
	PR_GrabShareImgRpy(m_nCtrlSenderID, &rpy2);

	TakeTime((TIME_ENUM)SCAN_WPR_IMG_RPY2);	// prescan share rpy2 die ready, trigger wft move non es101

	if ((PR_COMM_NOERR != rpy2.stStatus.uwCommunStatus) || 
		(PR_ERR_NOERR != rpy2.stStatus.uwPRStatus))
	{
		CString szLogText;
		szLogText.Format("(PR_GrabShareImgRpy)PR abnormal C Grab share img rpy2 error com = %d; status = %d", rpy2.stStatus.uwCommunStatus, rpy2.stStatus.uwPRStatus);
		SaveScanTimeEvent(szLogText);
		m_prStation->SetErrorMessage(szLogText);
		pUtl->SetPrAbnormal(TRUE, szLogText);	// grab rpy2 error.
		C_PrescanCycleLog(szLogText);
		m_csGrab.Unlock();	//	grab rpy2 error
		m_prStation->HmiMessage_Red_Back(szLogText, "Alarm");
		return FALSE;
	}

	DOUBLE dPrX=0, dPrY=0;
	m_prStation->ConvertMotorStepToPrPixel(encX, encY, dPrX, dPrY);
	ToBeProcessedInfo info;
	info.m_ulPrGrabID	= ulGrabCnt+1;
	info.m_nStationID	= rpy2.ulStationID;
	info.m_ulImageID	= rpy2.ulImageID;
	info.m_nGrabEncX	= encX;
	info.m_nGrabEncY	= encY;
	if( m_prStation->IsAutoRescanWafer() )
	{
		info.m_ulGrabMapRow = lFrameRow;
		info.m_ulGrabMapCol = lFrameCol;
	}
	else
	{
		info.m_ulGrabMapRow = ulRow;
		info.m_ulGrabMapCol = ulCol;
	}
	info.m_dGrabPrX		= dPrX;
	info.m_dGrabPrY		= dPrY;
	info.m_lMapShowRow	= ulRow;
	info.m_lMapShowCol	= ulCol;

	if( ulGrabCnt==0 )
		m_ul1stGrabImageID	= rpy2.ulImageID;
	else
		m_ul2ndGrabImageID	= rpy2.ulImageID;
	m_dShareImageFocusScore	= -1.0;

	//2018.4.24
	CSingleLock slCsLock(&m_csQueueMutex);
	slCsLock.Lock();
	m_toBeProcessed.push_back(info);
	slCsLock.Unlock();

	CString szLogText;
	szLogText.Format("  C Grab image %d in rpy2, left(%d)", rpy2.ulImageID, GalleryImageNumInMemory());
	C_PrescanCycleLog(szLogText);
	szLogText.Format("image PR grab %lu by %d,%d",
		rpy2.ulImageID, m_nCtrlSenderID, m_nCtrlReceiverID);
	pUtl->ScanImageHistory(szLogText);	//	grab
}
	m_csGrab.Unlock();	//	grab rpy3 done

	TakeTime((TIME_ENUM)SCAN_WPR_IMG_RPY3);	// prescan share rpy2 die ready, trigger wft move non es101

	//Grab Image done function will call "GrabMultiSendSearchCommand"
	if (GalleryImageNumInMemory() <= 0)
	{
		return TRUE;
	}

	bool bHasIdle = false;
	int nThreadNum = GetSrchThreadNum();
	//if( m_prStation->IsEnableZoom() && m_prStation->GetScnZoom()!=m_prStation->GetNmlZoom() &&
	//	m_prStation->IsPrescanning() && m_prStation->IsPrescanEnded()==FALSE && FALSE )
	//{
	//	nThreadNum--;
	//	nThreadNum = max(1, nThreadNum);
	//}

	for (int j = 0; j < nThreadNum; ++j)
	{
		if (m_bIsThreadIdle[j])
		{
			bHasIdle = true;
		}
	}

	if (bHasIdle == false)
	{
		return TRUE;
	}

	// now add the image to queue
	m_csAssignTask.Lock();	// grab, assign job to thread
	// the follwoing SendSearch Command will be called only when:
	//	1. the first time in auto cycle
	//	2. when the PR search threads can keep up with the wafer indexing, so the threads are idle
	for (int j = 0; j < nThreadNum; ++j)
	{
		if (GrabMultiSendSearchCommand(j, nThreadNum))
		{
			break;
		}
	}
	m_csAssignTask.Unlock();

	TakeTime((TIME_ENUM)SCAN_WPR_IMG_RPY3);	// prescan share rpy2 die ready, trigger wft move non es101

	return TRUE;
}

template<class PRStn>
ULONG CGallerySearchController<PRStn>::GetShareImageID()
{
	return m_ul1stGrabImageID;
}

template<class PRStn>
ULONG CGallerySearchController<PRStn>::Get2ndShareImageID()
{
	return m_ul2ndGrabImageID;
}

template<class PRStn>
DOUBLE	 CGallerySearchController<PRStn>::GetShareImageFocusScore()
{
	return m_dShareImageFocusScore;
}

template<class PRStn>
PR_UWORD CGallerySearchController<PRStn>::GalleryDeleteImageCmd(PR_ULWORD imageID, PR_BOOLEAN freeAll)
{
	CString szLogText;
	szLogText.Format("  C to remove share image %lu", imageID);
	C_PrescanCycleLog(szLogText);

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	szLogText.Format("image PR remv %lu ", imageID);
	pUtl->ScanImageHistory(szLogText);	// image remove

	DOUBLE dTime = GetTime();
	m_csGrab.Lock();	//	delete
	TakeTime((TIME_ENUM)SCAN_WPR_IMG_KBGN);	// prescan_time too many image left, over Eagle limit, wait for a moment before memory is released
	PR_REMOVE_SHARE_IMAGE_CMD     cmd;
	PR_REMOVE_SHARE_IMAGE_RPY     rpy;

	C_PrescanCycleLog("  C remove share init");
	PR_InitRemoveShareImgCmd(&cmd);
	cmd.ulImageID = imageID;
	cmd.emFreeAll = freeAll; // You can set this to PR_TRUE if you want to clear all Share Image for this channel
	C_PrescanCycleLog("  C remove share image");
	PR_RemoveShareImgCmd(&cmd, m_nCtrlSenderID, m_nCtrlReceiverID, &rpy);
	LONG lKillTime = (LONG)(GetTime()-dTime);
szLogText.Format("  C Kill image %12d, left %d, used time %d", imageID, GalleryImageNumInMemory(), lKillTime);
C_PrescanCycleLog(szLogText);
	TakeTime((TIME_ENUM) SCAN_WPR_IMG_KILL);
	m_csGrab.Unlock();	//	delete done
	if (PR_COMM_NOERR != rpy.uwCommunStatus)
	{
		szLogText.Format("PR abnormal Kill image %12d, left %d, comm %d",
			imageID, GalleryImageNumInMemory(), rpy.uwCommunStatus);
		pUtl->SetPrAbnormal(TRUE, szLogText);
		m_prStation->SetErrorMessage(szLogText);
		return PR_COMM_ERR;
	}

	return PR_ERR_NOERR;
}

template<class PRStn>
LONG CGallerySearchController<PRStn>::GalleryNumImage()
{
	return GalleryImageNumInMemory();
}

template<class PRStn>
PR_UWORD CGallerySearchController<PRStn>::GalleryImageNumInMemory()
{
	CSingleLock slCsLock(&m_csQueueMutex);
	slCsLock.Lock();
	PR_UWORD lSize = (PR_UWORD)m_toBeProcessed.size();

	return lSize;
}

template<class PRStn>
BOOL CGallerySearchController<PRStn>::IsReadyToGrab()
{
	if( GalleryImageNumInMemory()<GetBufferImageLimit() )
		return TRUE;
	else
		return FALSE;
}

template<class PRStn>
BOOL CGallerySearchController<PRStn>::IsIdle()
{
	if (GalleryImageNumInMemory() > 0)
		return false;

	bool ret = true;
	for (int i = 0; i < GetSrchThreadNum(); ++i)
	{
		ret &= m_bIsThreadIdle[i];
	}
	return ret;
}




template<class PRStn>
void CGallerySearchController<PRStn>::SetSoraaAOIMode(bool bYesNo)
{
	m_bSoraaAOI = bYesNo;
}

template<class PRStn>
void CGallerySearchController<PRStn>::SetupSubImageNum(PR_UWORD uwSubImageNum)
{
	m_uwSubImgTotalNum	= uwSubImageNum;

	for (int i = 0; i < MAX_PR_GALLERY_SEARCH_THREADS; ++i)
	{
		m_bIsThreadIdle[i] = true;
		m_ulThreadImageID[i] = 0;
		m_ulImageDieSum[i]	 = 0;
		m_stSrchInfo[i].m_lSubImgTgtIdx = 1;
		m_stSrchInfo[i].m_bSubImgEnable	= false;
	}
}

template<class PRStn>
void CGallerySearchController<PRStn>::SetupSearchCommand(PR_SRCH_DIE_CMD &cmd)
{
	m_stSrchCmd = cmd;
	for (int i = 0; i < MAX_PR_GALLERY_SEARCH_THREADS; ++i)
	{
		m_searchThread[i]->PostThreadMessage(UWM_SETUP_SEARCH_COMMAND, (WPARAM)&m_stSrchCmd, NULL);
	}
}

// *******************************
// below for multi search purpose
// *******************************
template<class PRStn>
void CGallerySearchController<PRStn>::C_PrescanCycleLog(CString szMsg, BOOL bSaveCycle)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetVisionCtrlLog() )
		OutputDebugString(szMsg);

	if( pUtl->GetPrAbnormal() )
	{
		SaveScanTimeEvent(szMsg);
	}
}

template<class PRStn>
void CGallerySearchController<PRStn>::AutoMultiSendSearchCommand(int id)
{
	if( m_running==false || GalleryImageNumInMemory()<=0 )
		return ;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrAbnormal() )
	{
		return ;
	}

	CSingleLock slCsLock(&m_csQueueMutex);
	slCsLock.Lock();

	CString szLogText;
	szLogText.Format("  C Auto multi send search command to thread %d", id);
	C_PrescanCycleLog(szLogText);

	ToBeProcessedInfo *info = NULL;
	ToBeProcessedQueue::iterator revIter = m_toBeProcessed.begin();
	if( m_prStation->IsStitchMode() )
	{
	}
	else
	{
		int nThreadNum = GetSrchThreadNum();
		//if( m_prStation->IsEnableZoom() && m_prStation->GetScnZoom()!=m_prStation->GetNmlZoom() &&
		//	m_prStation->IsPrescanning() && m_prStation->IsPrescanEnded()==FALSE && FALSE )
		//{
		//	nThreadNum--;
		//	nThreadNum = max(1, nThreadNum);
		//}
		while (revIter != m_toBeProcessed.end())
		{
			ULONG ulToBeImageID = revIter->m_ulImageID;
			if (m_bIsThreadIdle[id])
			{
				bool bFindOne = true;
				for(int i=0; i<nThreadNum; i++)
				{
					bFindOne = bFindOne && (m_ulThreadImageID[i] != ulToBeImageID);
				}
				if( bFindOne )
				{
					info = &(*revIter);
					m_ulThreadImageID[id] = ulToBeImageID;
					m_lSubImgCrlIdx[id] = 1;
					m_ulImageDieSum[id] = 0;
					szLogText.Format("  C Auto First the imageid %d for thread(%d)", m_ulThreadImageID[id], id);
					C_PrescanCycleLog(szLogText);
					break;
				}
			}
			else
			{
//2018.6.29
//if this sub-image was already processed becasue call grab image function and will pass the 1st sub-image to process task, so do not send it again if this image is the 1st sub-image
				if ((m_lSubImgCrlIdx[id] > 1) && (m_ulThreadImageID[id] == ulToBeImageID))
				{
					info = &(*revIter);
					szLogText.Format("  C processing image %d of thread(%d)", m_ulThreadImageID[id], id);
					C_PrescanCycleLog(szLogText);
					break;
				}
			}
			if( info )
				break;
			++revIter;
		}
	}

	if (info)
	{
		m_bIsThreadIdle[id] = false;

		m_stSrchInfo[id].m_stationID	= info->m_nStationID;
		m_stSrchInfo[id].m_imageID		= info->m_ulImageID;
		m_stSrchInfo[id].m_PrGrabID		= info->m_ulPrGrabID;
		m_stSrchInfo[id].m_dInspPrX		= info->m_dGrabPrX;
		m_stSrchInfo[id].m_dInspPrY		= info->m_dGrabPrY;

		if( m_uwSubImgTotalNum>1 )
		{
			m_stSrchInfo[id].m_bSubImgEnable	= true;
			m_stSrchInfo[id].m_lSubImgTgtIdx	= GetSubImageIdx(id);
			PR_WORD lULX, lULY, lLRX, lLRY;
			m_prStation->GetSubWindow(GetSubImageIdx(id), lULX, lULY, lLRX, lLRY);

			m_stSrchInfo[id].m_wSubImgULX = lULX;
			m_stSrchInfo[id].m_wSubImgULY = lULY;
			m_stSrchInfo[id].m_wSubImgLRX = lLRX;
			m_stSrchInfo[id].m_wSubImgLRY = lLRY;
		}

		szLogText.Format("image to auto %lu post to thread %d, sub win %ld",
			info->m_ulImageID, id, GetSubImageIdx(id));
		pUtl->ScanImageHistory(szLogText);	// auto pass when previous done

		szLogText.Format("  C Auto post image handling sub image %d to thread", GetSubImageIdx(id));
		C_PrescanCycleLog(szLogText);
		m_searchThread[id]->PostThreadMessage(UWM_GALLERY_START_SEARCH, (WPARAM)&m_stSrchInfo[id], NULL);
	}
}

template<class PRStn>
LONG CGallerySearchController<PRStn>::GetSubImageIdx(CONST INT nThreadID)
{
	INT nIndex = min(nThreadID, MAX_PR_GALLERY_SEARCH_THREADS-1);
	return m_lSubImgCrlIdx[nIndex];
}


template<class PRStn>
bool CGallerySearchController<PRStn>::IsRunThread(const ULONG ulToBeImageID, const int nThreadMax)
{
	for(int id = 0; id < nThreadMax; id++)
	{
		if (!m_bIsThreadIdle[id])
		{
			if (m_ulThreadImageID[id] == ulToBeImageID)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}


template<class PRStn>
bool CGallerySearchController<PRStn>::FindIdleThread(const ULONG ulToBeImageID, const int nThreadMax)
{
	if (IsRunThread(ulToBeImageID, nThreadMax))
	{
		return FALSE;
	}

	for(int id = 0; id < nThreadMax; id++)
	{
		if (m_bIsThreadIdle[id])
		{
			if (m_ulThreadImageID[id] != ulToBeImageID)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}


template<class PRStn>
bool CGallerySearchController<PRStn>::GrabMultiSendSearchCommand(int id, const int nThreadMax)
{
	if (m_running == false || GalleryImageNumInMemory() <= 0 || m_bIsThreadIdle[id] == false)
	{
		return false;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (pUtl->GetPrAbnormal())
	{
		return FALSE;
	}

	CSingleLock slCsLock(&m_csQueueMutex);
	slCsLock.Lock();

	bool bFound = false;
	ToBeProcessedInfo *info = NULL;
	ToBeProcessedQueue::iterator revIter = m_toBeProcessed.begin();

	while ((revIter != m_toBeProcessed.end()))
	{
		ULONG ulToBeImageID = revIter->m_ulImageID;
		BOOL bFindOne = FindIdleThread(ulToBeImageID, nThreadMax);
		if (bFindOne)
		{
			info = &(*revIter);

			m_bIsThreadIdle[id]	= false;
			m_lSubImgCrlIdx[id] = 1;
			m_ulImageDieSum[id] = 0;
			m_ulThreadImageID[id] = ulToBeImageID;

			m_stSrchInfo[id].m_stationID	= info->m_nStationID;
			m_stSrchInfo[id].m_imageID		= ulToBeImageID;
			m_stSrchInfo[id].m_PrGrabID		= info->m_ulPrGrabID;
			m_stSrchInfo[id].m_dInspPrX		= info->m_dGrabPrX;
			m_stSrchInfo[id].m_dInspPrY		= info->m_dGrabPrY;

			if (m_uwSubImgTotalNum > 1)
			{
				m_stSrchInfo[id].m_bSubImgEnable	= true;
				m_stSrchInfo[id].m_lSubImgTgtIdx = GetSubImageIdx(id);
				PR_WORD lULX, lULY, lLRX, lLRY;
				m_prStation->GetSubWindow(GetSubImageIdx(id), lULX, lULY, lLRX, lLRY);

				m_stSrchInfo[id].m_wSubImgULX = lULX;
				m_stSrchInfo[id].m_wSubImgULY = lULY;
				m_stSrchInfo[id].m_wSubImgLRX = lLRX;
				m_stSrchInfo[id].m_wSubImgLRY = lLRY;
			}
			CString szLogText;
			szLogText.Format("image grab ps %lu post to thread %d, sub win %ld",
				ulToBeImageID, id, GetSubImageIdx(id));
			pUtl->ScanImageHistory(szLogText);	// grab done and find a idle thread
			szLogText.Format("  C Grab image %d post sub img %d to thread %d",
				m_ulThreadImageID[id], GetSubImageIdx(id), id);
			C_PrescanCycleLog(szLogText);
			m_searchThread[id]->PostThreadMessage(UWM_GALLERY_START_SEARCH, (WPARAM)&m_stSrchInfo[id], NULL);
			bFound = true;
			break;
		}
		++revIter;
	}

	return bFound;
}

template<class PRStn>
void CGallerySearchController<PRStn>::OnGallerySearchDone(WPARAM wSrchParam, LPARAM lRpyParam)
{
	CSingleLock slCsLock(&m_csQueueMutex);
	slCsLock.Lock();

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	PRGallerySrchReplies *replies = (PRGallerySrchReplies*)lRpyParam;
	SearchAreaInfo stSrchInfo;
	// make a copy immediately, as srch parameter is actually a pointer to member of CGallerySearchController,
	//	so this function will modify it eventually
	memcpy(&stSrchInfo, (SearchAreaInfo*)wSrchParam, sizeof(SearchAreaInfo));
	// now check if ImageGallery has anything in it.
	int   nThreadID = stSrchInfo.m_threadID;
	ULONG ulImageID = stSrchInfo.m_imageID;
	ULONG ulPrGrabID = stSrchInfo.m_PrGrabID;

	CString szMsg;
	szMsg.Format("  C thread %d search return, get detail die info of %lu", nThreadID, ulImageID);
	C_PrescanCycleLog(szMsg);
	szMsg.Format("image cm back %lu from thread %d cmdCommStatus =%d DieNumOfResult %d", ulImageID, nThreadID, replies->m_cmdCommStatus, replies->m_reply2.uwNResults);
	pUtl->ScanImageHistory(szMsg);	// search done and return back

	if( nThreadID>=GetSrchThreadNum() )
	{
		CString szMsg;
		szMsg = "PR abnormal Critical Error, thread over limit";
		SaveScanTimeEvent(szMsg);
		m_prStation->SetErrorMessage(szMsg);
	}

	ToBeProcessedQueue::iterator revIter = m_toBeProcessed.begin();
	if (m_toBeProcessed.size() > 0)
	{
		bool bBadNews = true;
		// now search the m_to BeProcessed queue for imageID
		while (revIter != m_toBeProcessed.end())
		{
			if (ulImageID == revIter->m_ulImageID)
			{
				bBadNews = false;
				break;
			}
			++revIter;
		}
		if( bBadNews )
		{
			szMsg.Format("(BadNews) find image in list error");
			pUtl->ScanImageHistory(szMsg);	// search done and return back

			CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
			if (pPRFailureCaseLog->IsEnableFailureCaselog())
			{
				//CloseAlarm();
				//CString szMsg;
				//szMsg = "Log PR Failure Case...";
				//SetAlert_WarningMsg(szMsg);
//				pPRFailureCaseLog->LogFailureCaseDumpFile(GALLERY_SEARCH_BASE_SID + nThreadID, GALLERY_SEARCH_BASE_RID + nThreadID, 200);
				//SetAlarmLamp_Green(FALSE, TRUE);
				//CloseAlarm();
				pUtl->SetPrAbnormal(TRUE, "(BadNews) find image in list error");
			}
			ASSERT(FALSE);
			C_PrescanCycleLog("  C find image in list error");
			return;
		}
	}

	bool bIsLastDie = false;
	if (revIter != m_toBeProcessed.end())
	{
		int	nOffsetX=0, nOffsetY=0, nDieX=0, nDieY=0;
		// alignment result		// now process the result
		int nGrabX		= revIter->m_nGrabEncX;
		int nGrabY		= revIter->m_nGrabEncY;
		long ulGrabRow	= revIter->m_ulGrabMapRow;
		long ulGrabCol	= revIter->m_ulGrabMapCol;
		LONG lMapShowRow = revIter->m_lMapShowRow;
		LONG lMapShowCol = revIter->m_lMapShowCol;

		PR_UWORD usRpy2DieType = replies->m_reply2.stStatus.uwPRStatus;
		if (usRpy2DieType == PR_ERR_NOERR)
		{
			usRpy2DieType = PR_ERR_GOOD_DIE;
		}

		BOOL b5MReturn = TRUE;

		CString szLogText;
		bIsLastDie = true;
		// for multi search, m_wnRow and m_wnCol should keep as center one ????
		CString szDieScore = "1";
		PR_RCOORD rcDieCtr;
		PR_REAL   dDieRot;
		PR_UWORD uwDiePrID = 1;

		if( ulPrGrabID==1 )
			uwDiePrID	= m_stSrchCmd.auwRecordID[0];
		else
			uwDiePrID	= m_stSrchCmd2.auwRecordID[0];

		if( pUtl->GetAlignPrFrameID()==0 )
		{
			pUtl->SetAlignPrFrameID(ulImageID);
		}
		USHORT	usDieSum = replies->m_reply2.uwNResults;
		if( replies->m_reply2.stStatus.uwPRStatus == PR_ERR_NO_SYS_MEM )
		{
			b5MReturn = FALSE;
			szLogText.Format("WPR return PR abnormal memory insufficient of image %d sub image %d by thread %d", ulImageID, GetSubImageIdx(nThreadID), nThreadID);
			SaveScanTimeEvent(szLogText);
			Sleep(50);
		}

		if (replies->m_wSearchErrors > 0)
		{
			szLogText.Format("WPR return num %d PR abnormal search error of image %d sub image %d by thread %d",
				usDieSum, ulImageID, GetSubImageIdx(nThreadID), nThreadID);
			SaveScanTimeEvent(szLogText);
			usDieSum = 0;
			b5MReturn = FALSE;
			Sleep(50);
			if (replies->m_wSearchErrors > 5)
			{
				replies->m_wSearchErrors = 0;
				b5MReturn = TRUE;
			}
		}

		m_ulImageDieSum[nThreadID] += usDieSum;
		if( ((m_ulImageDieSum[nThreadID]==0 && GetSubImageIdx(nThreadID)==m_uwSubImgTotalNum) || replies->m_cmdCommStatus != 0) &&
			m_prStation->IsEnableZoom() && m_prStation->GetScnZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF )	//	if die found is 0, log table, map, grab matrix.
		{
			szLogText.Format("Image Die Counter ZERO abnormal image %lu die num %d, by thread %d, grab at wft %d,%d, grab %ld,%ld, map %ld,%ld",
				ulImageID, m_ulImageDieSum[nThreadID], nThreadID, nGrabX, nGrabY,ulGrabRow, ulGrabCol, lMapShowRow, lMapShowCol);
			pUtl->ScanImageHistory(szLogText);
		//	SaveScanTimeEvent(szLogText);
		}	//	also image no.

		DOUBLE dTime = GetTime();
		for(USHORT usIndex=0; usIndex<usDieSum; usIndex++)
		{
			bool bIsDefect = false;
			rcDieCtr	= replies->m_stDieAlign[usIndex].rcoDieCentre;
			dDieRot		= replies->m_stDieAlign[usIndex].rDieRot;
			m_prStation->CalcScanDiePosition(nGrabX, nGrabY, rcDieCtr, nOffsetX, nOffsetY);

			nDieX = nGrabX+nOffsetX;
			nDieY = nGrabY+nOffsetY;
			if( m_stSrchCmd.emDefectInsp==PR_TRUE )
			{
				PR_UWORD usRpy3DieType = replies->m_stDieInspc[usIndex].uwPRStatus;
				bIsDefect	= m_prStation->ScanDieIsDefect(usRpy2DieType, usRpy3DieType)?true:false;
			}
			bool bIsBadCut = false;
			if( m_stSrchCmd.emPostBondInsp==PR_TRUE )
			{
				PR_UWORD usBadcutValue = replies->m_stPostBond[usIndex].uwPRStatus;
				bIsBadCut		= m_prStation->DieIsBadCut(usBadcutValue)?true:false;
			}
			ULONG ulPassInImageID = ulImageID;
			//	now convert to frame id from grab map and column, 
			if (m_prStation->IsAutoRescanWafer())
			{
				ulPassInImageID = MAKEWORD((UCHAR)ulGrabCol, (UCHAR)ulGrabRow);
			}
			m_prStation->SetScanPosition(nDieX, nDieY, dDieRot, szDieScore, bIsDefect, bIsBadCut, false, ulPassInImageID, uwDiePrID);
			m_prStation->ReScanRunTimeDisplayMapIndexForMS(ulGrabRow, ulGrabCol, nOffsetX, nOffsetY, nDieX, nDieY, bIsBadCut, bIsDefect);
		}	// usIndex loop

		if (bIsLastDie == true)
		{
			if (b5MReturn && m_uwSubImgTotalNum > 1 && GetSubImageIdx(nThreadID) < m_uwSubImgTotalNum)
			{
				m_lSubImgCrlIdx[nThreadID]++;
				bIsLastDie = false;
				szLogText.Format("  C image %d sub image + to %d", ulImageID, GetSubImageIdx(nThreadID));
				C_PrescanCycleLog(szLogText, TRUE);
			}

			if (b5MReturn == FALSE)
			{
				bIsLastDie = false;
				szLogText.Format("  C image %d PR memory sub image retry %d", ulImageID, GetSubImageIdx(nThreadID));
				C_PrescanCycleLog(szLogText, TRUE);
			}	//	PR side malloc error
		}

		if (m_prStation->IsStitchMode() && bIsLastDie)
		{
			GalleryDeleteImageCmd(ulImageID);	
			CString szLogText;
			szLogText.Format("  C Delete ImageID %d of thread %d", ulImageID, nThreadID);
			C_PrescanCycleLog(szLogText, TRUE);
		}

	//	slCsLock.Unlock();

	//	m_csAssignTask.Lock();	// task remove	// done, auto find job for thread itself

		if( pUtl->GetPrAbnormal() )
		{
			bIsLastDie = TRUE;
		}

		if (bIsLastDie)
		{
			if (m_toBeProcessed.size() > 0)
			{
				m_toBeProcessed.erase(revIter);
			}
			szLogText.Format("image mm kill %lu sub image + to %d thread %d, total %d",
				ulImageID, GetSubImageIdx(nThreadID), nThreadID, m_ulImageDieSum[nThreadID]);
			pUtl->ScanImageHistory(szLogText);	// erase from memory
			m_bIsThreadIdle[nThreadID]			= true;
			m_ulThreadImageID[nThreadID]	= 0;
			m_lSubImgCrlIdx[nThreadID]		= 1;
			m_ulImageDieSum[nThreadID]		= 0;
			szLogText.Format("  C image %d finished sub image + to %d thread %d", ulImageID, GetSubImageIdx(nThreadID), nThreadID);
			C_PrescanCycleLog(szLogText, TRUE);
		}
	}
	//	m_csAssignTask.Unlock();

	slCsLock.Unlock();

	if (GalleryImageNumInMemory() > 0 && pUtl->GetPrAbnormal() != TRUE)
	{
		// now check if needs to delete this image		
		AutoMultiSendSearchCommand(nThreadID);
	}


	if (m_prStation->IsStitchMode() == FALSE && bIsLastDie)
	{
		GalleryDeleteImageCmd(ulImageID);	
		CString szLogText;
		szLogText.Format("  C Delete ImageID %d of thread %d", ulImageID, nThreadID);
		C_PrescanCycleLog(szLogText, TRUE);
	}
}

template<class PRStn>
int CGallerySearchController<PRStn>::GetSrchThreadNum()
{
	return m_nUsedSrchThread;
}

template<class PRStn>
int CGallerySearchController<PRStn>::SetGrabImageLimit(const int nLimit)
{
	int nImageLimit = nLimit;
	if( nImageLimit<=0 )
	{
	#ifdef	VS_5MCAM
		nImageLimit =	60;	//	120
	#else
		nImageLimit =	400;
	#endif
	}

	m_nBufferImageLimit = max(nImageLimit, GetSrchThreadNum());

	return m_nBufferImageLimit;
}

template<class PRStn>
int CGallerySearchController<PRStn>::GetBufferImageLimit()
{
	return max(m_nBufferImageLimit, GetSrchThreadNum());
}

template<class PRStn>
int CGallerySearchController<PRStn>::SetSrchThreadNum(const int nNum)
{
	int nThreadNum = nNum;

	nThreadNum = max(nThreadNum, 1);
	nThreadNum = min(nThreadNum, MAX_PR_GALLERY_SEARCH_THREADS);

	if( m_prStation->IsEnableZoom() && m_prStation->GetScnZoom()!=m_prStation->GetNmlZoom() )
	{
		nThreadNum = min(nThreadNum, 8);
	}
	m_nUsedSrchThread = nThreadNum;

	return m_nUsedSrchThread;
}

template<class PRStn>
void CGallerySearchController<PRStn>::SetupSearchCommand2(PR_SRCH_DIE_CMD &cmd)
{
	m_stSrchCmd2 = cmd;
	for (int i = 0; i < MAX_PR_GALLERY_SEARCH_THREADS; ++i)
	{
		m_searchThread[i]->PostThreadMessage(UWM_SETUP_SEARCH_COMMAND2, (WPARAM)&m_stSrchCmd2, NULL);
	}
}
