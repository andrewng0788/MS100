// FlushMessageThread.cpp : implementation file
//

#include "stdafx.h"
#include "MS896A.h"
#include "FlushMessageThread.h"
#include "BinTable.h"


// CFlushMessageThread

IMPLEMENT_DYNCREATE(CFlushMessageThread, CWinThread)

CFlushMessageThread::CFlushMessageThread()
{
	m_pStation = NULL;
}

CFlushMessageThread::~CFlushMessageThread()
{
}

BOOL CFlushMessageThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CFlushMessageThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BOOL CFlushMessageThread::OnIdle(LONG lCount)
{
	if (m_pStation != NULL) 
		m_pStation->FlushMessage();
	Sleep(10);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CFlushMessageThread, CWinThread)
	ON_THREAD_MESSAGE(IDC_END_FLUSH_MESSAGE_THREAD, OnEndFlushMessageThread)
END_MESSAGE_MAP()


// CFlushMessageThread message handlers
void CFlushMessageThread::OnEndFlushMessageThread(WPARAM, LPARAM)
{
	::PostQuitMessage(0);
}