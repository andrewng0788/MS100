// PromptBox.cpp : implementation file
//

#include "stdafx.h"
#include "MS896A.h"
#include "PromptBox.h"


// CPromptBox dialog

IMPLEMENT_DYNAMIC(CPromptBox, CDialog)
CPromptBox::CPromptBox(CWnd* pParent /*=NULL*/)
	: CDialog(CPromptBox::IDD, pParent)
	, m_szInputValue(_T(""))
{
}

CPromptBox::~CPromptBox()
{
}

void CPromptBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INPUT_BOX, m_InputBox);
	DDX_Text(pDX, IDC_INPUT_BOX, m_szInputValue);
	DDX_Control(pDX, IDC_INPUT_TITLE, m_InputTitle);

	if (pDX->m_bSaveAndValidate)
	{
		if (m_szInputValue.IsEmpty())
			pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(CPromptBox, CDialog)
END_MESSAGE_MAP()


// CPromptBox message handlers

BOOL CPromptBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_InputBox.SetLimitText(7);
	m_InputTitle.SetWindowText(m_strCaption);
	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
