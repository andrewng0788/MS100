// PrCrossHairDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MS896A.h"
#include "PrCrossHairDlg.h"
#include "MS896AStn.h"
#include "BondPr.h"


// CPrCrossHairDlg dialog

IMPLEMENT_DYNAMIC(CPrCrossHairDlg, CDialog)
CPrCrossHairDlg::CPrCrossHairDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrCrossHairDlg::IDD, pParent)
	, m_lCursorStepSize(10)
{
	m_lCursorX = 4096;
	m_lCursorY = 4096;
}

CPrCrossHairDlg::~CPrCrossHairDlg()
{
}

VOID CPrCrossHairDlg::SetCursorXY(LONG lx, LONG ly)
{
	m_lCursorX = lx;
	m_lCursorY = ly;
}

BOOL CPrCrossHairDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_lCursorStepSize = 100;
	m_lCursorStepSize = 10;

	PRDrawCursor();
	return TRUE;
}

void CPrCrossHairDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITSTEPSIZE, m_lCursorStepSize);
	DDV_MinMaxLong(pDX, m_lCursorStepSize, 1, 500);
}


BEGIN_MESSAGE_MAP(CPrCrossHairDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTONUP, OnBnClickedButtonup)
	ON_BN_CLICKED(IDC_BUTTONDN, OnBnClickedButtondn)
	ON_BN_CLICKED(IDC_BUTTONLT, OnBnClickedButtonlt)
	ON_BN_CLICKED(IDC_BUTTONRT, OnBnClickedButtonrt)
END_MESSAGE_MAP()


// PrCrossHairDlg message handlers
BOOL CPrCrossHairDlg::PRDrawCursor()
{
	PR_DRAW_SMALL_CURSOR_CMD centredraw;
	centredraw.coPosition.x = (PR_WORD)m_lCursorX;
	centredraw.coPosition.y = (PR_WORD)m_lCursorY;
	PR_DRAW_SMALL_CURSOR_RPY drawpstRpy;
 	PR_DrawSmallCursorCmd(&centredraw, PSPR_SENDER_ID, PSPR_RECV_ID, &drawpstRpy);
	return TRUE;
}

void CPrCrossHairDlg::OnBnClickedButtonup()
{
	// TODO: Add your control notification handler code here
	m_lCursorY = m_lCursorY - m_lCursorStepSize;
	PRDrawCursor();
}

void CPrCrossHairDlg::OnBnClickedButtondn()
{
	// TODO: Add your control notification handler code here
	m_lCursorY = m_lCursorY + m_lCursorStepSize;
	PRDrawCursor();
}

void CPrCrossHairDlg::OnBnClickedButtonlt()
{
	// TODO: Add your control notification handler code here
	m_lCursorX = m_lCursorX - m_lCursorStepSize;
	PRDrawCursor();
}

void CPrCrossHairDlg::OnBnClickedButtonrt()
{
	// TODO: Add your control notification handler code here
	m_lCursorX = m_lCursorX + m_lCursorStepSize;
	PRDrawCursor();
}

LONG CPrCrossHairDlg::GetCursorX()
{
	return m_lCursorX;
}

LONG CPrCrossHairDlg::GetCursorY()
{
	return m_lCursorY;
}


