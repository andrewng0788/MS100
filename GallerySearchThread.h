#pragma once
#include <afxmt.h>
#include "UWM.h"
#include <prheader.h>

#define	WPR_ONE_SEARCH_MAX_DIE	1000

typedef struct PRGallerySrchReplies
{
	PR_UWORD		m_wSearchErrors;
	PR_UWORD		m_cmdCommStatus;

	PR_SRCH_DIE_RPY1	m_reply1;	// command reply
	PR_SRCH_DIE_RPY2	m_reply2;	// align reply
	PR_SRCH_DIE_RPY3	m_reply3;	// inspect reply
	PR_SRCH_DIE_RPY4	m_reply4;	// bad cut
	PR_DIE_ALIGN_MINIMAL_RPY	m_stDieAlign[WPR_ONE_SEARCH_MAX_DIE];
	PR_DIE_INSP_MINIMAL_RPY		m_stDieInspc[WPR_ONE_SEARCH_MAX_DIE];
	PR_POST_INSP_MINIMAL_RPY	m_stPostBond[WPR_ONE_SEARCH_MAX_DIE];

	bool CmdCommNoError()
	{
		return true;	//v4.53	Klocwork
	}

	void ResetReplies()
	{
		m_cmdCommStatus =
		m_reply1.uwCommunStatus =
		m_reply2.stStatus.uwCommunStatus =
		m_reply3.stStatus.uwCommunStatus =
		m_reply4.stStatus.uwCommunStatus = PR_COMM_NOERR;

		m_reply1.uwPRStatus =
		m_reply2.stStatus.uwPRStatus =
		m_reply3.stStatus.uwPRStatus =
		m_reply4.stStatus.uwPRStatus = PR_ERR_NOERR;

		m_reply2.uwNResults = 0;
	}

} PRGallerySrchReplies;

class CGallerySearchThread :	public CWinThread
{
	DECLARE_DYNCREATE(CGallerySearchThread)
public:
	CGallerySearchThread();
	virtual ~CGallerySearchThread(void);

	void SetInfo(CWinThread *controller, int senderID, int receiverID);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnGalleryStartSearch(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShutDown(WPARAM wParam, LPARAM lParam);

	afx_msg void OnSetupSearchCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetupSearchCommand2(WPARAM wParam, LPARAM lParam);

protected:
	CWinThread		*m_controller;
	unsigned long	m_ulCurrImageID;

	void S_PrescanCycleLog(CString szMsg);

	LONG		m_lSubImgCurIdx;
	int			m_senderID;
	int			m_receiverID;

	PR_SRCH_DIE_CMD		m_command;
	PR_SRCH_DIE_CMD		m_SrchCmd2;
	PRGallerySrchReplies	m_replies;

	static CCriticalSection	m_csPostThreadMessage;
public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
};
