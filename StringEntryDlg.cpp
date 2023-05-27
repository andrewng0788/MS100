/////////////////////////////////////////////////////////////////
//	StringEntryDlg.cpp : interface of the StringEntryDlg class
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
#include "resource.h"       // main symbols
#include "StringEntryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CStringEntryDlg dialog
/////////////////////////////////////////////////////////////////////////////

CStringEntryDlg::CStringEntryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStringEntryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStringEntryDlg)
	m_szString = _T("");
	//}}AFX_DATA_INIT

	m_szTitle = _T("String Entry");
}

void CStringEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStringEntryDlg)
	DDX_Text(pDX, IDC_STRING, m_szString);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStringEntryDlg, CDialog)
	//{{AFX_MSG_MAP(CStringEntryDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStringEntryDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CStringEntryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_szTitle);
	SetWindowPos(&this->wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
