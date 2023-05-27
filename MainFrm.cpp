/////////////////////////////////////////////////////////////////
// MainFrm.cpp : implementation of the CMainFrame class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, June 04, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MS896A.h"

#include "MainFrm.h"
#include "Tlhelp32.h"

BOOL KillProcess(CString exeName);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
//	KillProcess("VISIONNT.exe");
//	KillProcess("AsmHmi.exe");
//	KillProcess("HealthMonitor.exe");
//	KillProcess("MapSorter.exe");
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	if (cs.hMenu != NULL)
	{
		DestroyMenu(cs.hMenu);
		cs.hMenu = NULL;
	}

	cs.cx = 475;
	cs.cy = 420;

	return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::OnClose() 
{
 	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( pApp->m_bAppClosing == FALSE )
	{
		CString szTitle;
		CString szContent;

		szTitle.LoadString(HMB_MS_SYS_WARNING);
		szContent.LoadString(HMB_MS_SYS_EXIT);

		if (!pApp->m_bAppInit)		//v3.97
		{
			CFrameWnd::OnClose();
		}
		else if ( AfxMessageBox(szTitle + "\n" + szContent, MB_YESNO|MB_SYSTEMMODAL|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES )
		{
			CFrameWnd::OnClose();
		}
	}
	else
	{
		CFrameWnd::OnClose();
	}
}

BOOL KillProcess(CString exeName)
{
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet = FALSE; 
	PROCESSENTRY32 pe32 = {0}; 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap != INVALID_HANDLE_VALUE) 
	{
		//  Fill in the size of the structure before using it. 
		pe32.dwSize = sizeof(PROCESSENTRY32); 
		if (Process32First(hProcessSnap, &pe32)) 
		{ 
			BOOL          bGotModule = FALSE; 
			MODULEENTRY32 me32 = {0}; 
			do 
			{
				CString exeFile = pe32.szExeFile;
				if (!exeFile.Compare(exeName))
				{
					HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
					if (handle != NULL)
					{
						TerminateProcess(handle, 0);
						CloseHandle(handle); 
						bRet = TRUE;
					}	
				}
			} while (Process32Next(hProcessSnap, &pe32)); 
		}
		CloseHandle(hProcessSnap); 
	}
	return bRet;
}

