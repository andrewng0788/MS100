/////////////////////////////////////////////////////////////////
// MS896AView.cpp : implementation of the CMS896AView class
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
#include "MS896ADoc.h"
#include "MS896AView.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
//	Constant Definitions
/////////////////////////////////////////////////////////////////
const INT gnMAX_MESSAGE_LINES		= 1000;


/////////////////////////////////////////////////////////////////
// CMS896AView
/////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMS896AView, CFormView)

BEGIN_MESSAGE_MAP(CMS896AView, CFormView)
	ON_BN_CLICKED(IDC_GROUP_ID, OnGroupId)
	ON_BN_CLICKED(IDC_AUTO_START, OnAutoStart)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_SELECT_HARDWARE, OnSelectHardware)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_LOG_MSG, OnLogMsg)
	ON_BN_CLICKED(IDC_EXEC_HMI, OnExecuteHmi)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////
// CMS896AView construction/destruction
/////////////////////////////////////////////////////////////////
CMS896AView::CMS896AView()
	: CFormView(CMS896AView::IDD)
{
	// TODO: add construction code here
}

CMS896AView::~CMS896AView()
{
}

void CMS896AView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_LIST, m_lbMessage);
	DDX_Control(pDX, IDC_GROUP_ID, m_btnGroupId);
	DDX_Control(pDX, IDC_MODULE_NAME, m_btnModule);
	DDX_Control(pDX, IDC_AUTO_START, m_btnAutoStart);
	DDX_Control(pDX, IDC_START, m_btnStart);
	DDX_Control(pDX, IDC_LOG_MSG, m_btnLogMessage);
	DDX_Control(pDX, IDC_EXEC_HMI, m_btnExecHmi);
}

BOOL CMS896AView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CMS896AView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_btnGroupId.SetWindowText(pApp->GetGroupID());

	if (pApp->AutoStart())
	{
		m_btnAutoStart.SetCheck(1);
		m_btnStart.EnableWindow(FALSE);
	}
	else
	{
		m_btnAutoStart.SetCheck(0);
	}

	m_btnLogMessage.SetCheck(pApp->LogMessage());
	m_btnExecHmi.SetCheck(pApp->IsExecuteHmi());
}


/////////////////////////////////////////////////////////////////
// CMS896AView diagnostics
/////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CMS896AView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMS896AView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CMS896ADoc* CMS896AView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMS896ADoc)));
	return (CMS896ADoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMS896AView message handlers
/////////////////////////////////////////////////////////////////////////////
void CMS896AView::OnGroupId() 
{
/*
	CStringEntryDlg		dlg;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->State() != UN_INITIALIZE_Q)
	{
		CString szMessage = _T("Group ID cannot be changed while appln is Initialized");
		DisplayMessage(szMessage);
		return;
	}
	
	dlg.m_szTitle = _T("Group ID");
	m_btnGroupId.GetWindowText(dlg.m_szString);

	if (dlg.DoModal() == IDOK)
	{
		pApp->SetGroupId(dlg.m_szString);
		m_btnGroupId.SetWindowText(dlg.m_szString);
	}
*/
}

void CMS896AView::OnAutoStart() 
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	INT nState = m_btnAutoStart.GetCheck();
	
	BOOL bState = FALSE;
	if (nState == 1)
	{
		bState = TRUE;
	}
	pApp->SaveAutoStart(bState);

	if ((nState == 1) &&
		(pApp->State() == UN_INITIALIZE_Q))
	{
		m_btnStart.EnableWindow(FALSE);
		pApp->SetRun();
	}
}

void CMS896AView::OnStart() 
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	m_btnStart.EnableWindow(FALSE);
	pApp->SetRun();
}

void CMS896AView::OnSelectHardware() 
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SelectHardware();
}

void CMS896AView::OnOptions() 
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SetOptions();
}

void CMS896AView::OnLogMsg() 
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	INT nState = m_btnLogMessage.GetCheck();
	
	BOOL bState = FALSE;
	if (nState == 1)
	{
		bState = TRUE;
	}
	pApp->SetLogMessage(bState);
}

void CMS896AView::OnExecuteHmi()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	INT nState = m_btnExecHmi.GetCheck();
	
	BOOL bState = FALSE;
	if (nState == 1)
	{
		bState = TRUE;
	}
	pApp->SetExecuteHmiFlag(bState);
}

VOID CMS896AView::SetGroupID(CString& szGroupID)
{
	m_btnGroupId.SetWindowText(szGroupID);
}

VOID CMS896AView::SetModule(CString& szModule)
{
	m_btnModule.SetWindowText(szModule);
}


/////////////////////////////////////////////////////////////////////////////
// Message Display
/////////////////////////////////////////////////////////////////////////////
VOID CMS896AView::DisplayMessage(const CString& szMessage)
{
	INT nIndex = m_lbMessage.AddString(szMessage);
	m_lbMessage.PostMessage(LB_SETCURSEL, nIndex, 0);
	m_lbMessage.PostMessage(LB_SETCURSEL, -1, 0);

	if (m_lbMessage.GetCount() > gnMAX_MESSAGE_LINES)
	{
		m_lbMessage.DeleteString(0);
	}
}
