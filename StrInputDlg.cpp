// StrInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MS896A.h"
#include "StrInputDlg.h"


// CStrInputDlg dialog

IMPLEMENT_DYNAMIC(CStrInputDlg, CDialog)
CStrInputDlg::CStrInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStrInputDlg::IDD, pParent)
	, m_szData(_T(""))
{
	m_szTitle = "";
}

CStrInputDlg::~CStrInputDlg()
{
}

BOOL CStrInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//if (m_szTitle.GetLength() > 0)
	SetWindowText(m_szTitle);

	return TRUE;
}

void CStrInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STRING, m_szData);
}


BEGIN_MESSAGE_MAP(CStrInputDlg, CDialog)
END_MESSAGE_MAP()


// CStrInputDlg message handlers
