#pragma once



// CFlushMessageThread

class CMS896AStn;

class CFlushMessageThread : public CWinThread
{
	DECLARE_DYNCREATE(CFlushMessageThread)

protected:
	CFlushMessageThread();           // protected constructor used by dynamic creation
	virtual ~CFlushMessageThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual BOOL OnIdle(LONG lCount);


	CMS896AStn* m_pStation;

protected:

	afx_msg void OnEndFlushMessageThread(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};


