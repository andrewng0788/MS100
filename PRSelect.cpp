// PRSelect.cpp : implementation file
//

#include "stdafx.h"
#include "MS896A.h"
#include "PRSelect.h"


// PRSelect dialog

IMPLEMENT_DYNCREATE(PRSelect, CDHtmlDialog)

PRSelect::PRSelect(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(PRSelect::IDD, PRSelect::IDH, pParent)
	, m_lID(1)
	, m_lCount(10)
	, m_lDelay(0)
{
}

PRSelect::~PRSelect()
{
}

void PRSelect::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, cbChannel);
	DDX_Control(pDX, IDC_COMBO2, cbType);
	DDX_Text(pDX, IDC_EDIT1, m_lID);
	DDX_Text(pDX, IDC_EDIT2, m_lCount);
	DDX_Text(pDX, IDC_EDIT3, m_lDelay);
}

BOOL PRSelect::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	cbChannel.AddString("Wafer");
	cbChannel.AddString("Bond");

	cbType.AddString("Normal");
	cbType.AddString("Reference");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(PRSelect, CDHtmlDialog)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_OK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnCbnSelchangeCombo2)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(PRSelect)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// PRSelect message handlers

HRESULT PRSelect::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	AfxMessageBox("Ori OK");
	cbChannel.GetWindowText(m_szChannel);
	cbType.GetWindowText(m_szType);
	
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT PRSelect::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

void PRSelect::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDHtmlDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

BOOL PRSelect::GetPara(CString &szChannel,CString &szType,LONG &lDelay,LONG &lCount,LONG &lID)
{
	UpdateData();
	szChannel = m_szChannel;
	szType = m_szType;
	lDelay = m_lDelay;
	lCount = m_lCount;
	lID = m_lID;
	return TRUE;
}

void PRSelect::OnBnClickedOk()
{
	OnOK();
	AfxMessageBox("Ori OK2");
	cbChannel.GetWindowText(m_szChannel);
	cbType.GetWindowText(m_szType);
	
	return;  // return TRUE  unless you set the focus to a control
}

void PRSelect::OnCbnSelchangeCombo1()
{
	UpdateData();
	//AfxMessageBox(m_szChannel);
	//cbChannel.GetWindowText(m_szChannel);
	// TODO: Add your control notification handler code here
}

void PRSelect::OnCbnSelchangeCombo2()
{
	UpdateData();
	//AfxMessageBox(m_szType);
	//cbType.GetWindowText(m_szType);
	// TODO: Add your control notification handler code here
}
