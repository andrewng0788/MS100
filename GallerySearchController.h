#pragma once

#include "UWM.h"

#ifdef	ES101
	#define	MAX_PR_GALLERY_SEARCH_THREADS		12
	#define GALLERY_SEARCH_BASE_SID				0x42
	#define	GALLERY_SEARCH_BASE_RID				0xa2
#else
	#define MAX_PR_GALLERY_SEARCH_THREADS		8

	#define GALLERY_SEARCH_BASE_SID				0x44
	#define	GALLERY_SEARCH_BASE_RID				0xa4
#endif


#include <deque>
#include <map>
#include <set>
#include <vector>

using std::deque;
using std::map;
using std::set;
using std::vector;


template <class PRStn>
class CGallerySearchController : public CWinThread
{
	DECLARE_DYNCREATE(CGallerySearchController)

protected:
	CGallerySearchController();           // protected constructor used by dynamic creation
	virtual ~CGallerySearchController();

public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

	void SetPRStation(PRStn *parent);
	void PartialResumePR();
	void ResumePR();
	void DeleteRemainImages();
	bool RecoverScanChannels();
	void RemoveAllShareImages();
	void ShutDown();

	void SetSoraaAOIMode(bool bYesNo);
	void SetupSubImageNum(PR_UWORD uwSubImageNum);

	void SetupSearchCommand(PR_SRCH_DIE_CMD &cmd);
	void SetupSearchCommand2(PR_SRCH_DIE_CMD &cmd);

	BOOL	GrabShareImage(LONG lFrameCol, LONG lFrameRow, int encX, int encY, long ulMapRow = 0, long ulMapCol = 0);
	ULONG	GetShareImageID();
	ULONG	Get2ndShareImageID();
	DOUBLE	GetShareImageFocusScore();

	PR_UWORD	GalleryDeleteImageCmd(PR_ULWORD imageID, PR_BOOLEAN freeAll = PR_FALSE);
	LONG		GalleryNumImage();
	PR_UWORD	GalleryImageNumInMemory();

	BOOL	IsReadyToGrab();
	BOOL	IsIdle();

	int GetBufferImageLimit();
	int SetGrabImageLimit(const int nLimit);
	int SetSrchThreadNum(const int nNum);
	int GetSrchThreadNum();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnGallerySearchDone(WPARAM wSrchParam, LPARAM lRpyParam);

protected:
	PRStn *m_prStation;
	volatile bool m_running;

	PR_SRCH_DIE_CMD	m_stSrchCmd;
	PR_SRCH_DIE_CMD	m_stSrchCmd2;
	int			m_nUsedSrchThread;
	int			m_nBufferImageLimit;
	int			m_nCtrlSenderID;
	int			m_nCtrlReceiverID;
	bool		m_bSoraaAOI;
	ULONG		m_ul1stGrabImageID;
	ULONG		m_ul2ndGrabImageID;
	DOUBLE		m_dShareImageFocusScore;

	CWinThread		*m_searchThread[MAX_PR_GALLERY_SEARCH_THREADS];
	SearchAreaInfo	m_stSrchInfo[MAX_PR_GALLERY_SEARCH_THREADS];
	bool			m_bIsThreadIdle[MAX_PR_GALLERY_SEARCH_THREADS];
	unsigned long	m_ulThreadImageID[MAX_PR_GALLERY_SEARCH_THREADS];

	PR_UWORD		m_uwSubImgTotalNum;
	LONG			m_lSubImgCrlIdx[MAX_PR_GALLERY_SEARCH_THREADS];
	ULONG			m_ulImageDieSum[MAX_PR_GALLERY_SEARCH_THREADS];

	typedef struct ToBeProcessedInfo
	{
		ToBeProcessedInfo()	{	}
// AOI PR TEST
		double	m_dGrabPrX;
		double	m_dGrabPrY;
// AOI PR TEST
		int		m_nGrabEncX;
		int		m_nGrabEncY;
		long	m_ulGrabMapRow;
		long	m_ulGrabMapCol;
		int		m_nStationID;
		ULONG	m_ulImageID;
		ULONG	m_ulPrGrabID;
		long	m_lMapShowRow;
		long	m_lMapShowCol;

	}ToBeProcessedInfo;

	// there are several queues/lists that make this works.
	// This queue stores the info of all the grabbed images, that are to be processed.  Once all 
	//	dice in the FOV of the image are done (search command sent), the entry will be deleted from this queue. 
	//  it is processed as a queue (FIFO)
	typedef deque<ToBeProcessedInfo> ToBeProcessedQueue;
	ToBeProcessedQueue m_toBeProcessed;

	// need this cs because we are not doing this in a straight Windows message loop handling manner
	//CCriticalSection	m_csQueue;
	CMutex				m_csQueueMutex;
	CCriticalSection	m_csGrab;
	CCriticalSection	m_csAssignTask;

protected:

	void AutoMultiSendSearchCommand(int id);
	bool GrabMultiSendSearchCommand(int id, const int nThreadMax);
	bool IsRunThread(const ULONG ulToBeImageID, const int nThreadMax);
	bool FindIdleThread(const ULONG ulToBeImageID, const int nThreadMax);

	LONG GetSubImageIdx(CONST INT nThreadID);
	void C_PrescanCycleLog(CString szMsg, BOOL bSaveCycle = FALSE);
};
