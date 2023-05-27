/////////////////////////////////////////////////////////////////
//	Hardware.cpp : implementation file for Dialog box used to configure
//		the controller hardware in the MS896A 
//		application
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
#include "resource.h"
#include "HardwareDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CHardwareDlg dialog
/////////////////////////////////////////////////////////////////////////////
CHardwareDlg::CHardwareDlg(CWnd* pParent /*=NULL*/)
			 : CDialog(CHardwareDlg::IDD, pParent)
			 , m_bDisableBHModule(FALSE)
			 , m_bDisableBTModule(FALSE)
			 , m_bDisableBLModule(FALSE)
			 , m_bDisableWLModule(FALSE)
			 , m_bDisableWTModule(FALSE)
			 , m_bDisableWLModuleWithExp(FALSE)
			 , m_bDisableNLModule(FALSE)
{
	//{{AFX_DATA_INIT(CHardware)
	m_bHardware = FALSE;
	//}}AFX_DATA_INIT
}

void CHardwareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHardwareDlg)
	DDX_Check(pDX, IDC_HW_SELECTION, m_bHardware);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_DISABLE_BH_MODULE, m_bDisableBHModule);
	DDX_Check(pDX, IDC_DISABLE_BT_MODULE, m_bDisableBTModule);
	DDX_Check(pDX, IDC_DISABLE_BL_MODULE, m_bDisableBLModule);
	DDX_Check(pDX, IDC_DISABLE_WL_MODULE, m_bDisableWLModule);
	DDX_Check(pDX, IDC_DISABLE_WT_MODULE, m_bDisableWTModule);
	DDX_Check(pDX, IDC_DISABLE_WL_MODULE_WITH_EXP, m_bDisableWLModuleWithExp);
	DDX_Check(pDX, IDC_DISABLE_NL_MODULE, m_bDisableNLModule);
}

BEGIN_MESSAGE_MAP(CHardwareDlg, CDialog)
	//{{AFX_MSG_MAP(CHardwareDlg)
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_DISABLE_WL_MODULE, OnBnClickedDisableWlModule)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHardwareDlg message handlers
/////////////////////////////////////////////////////////////////////////////

//void CHardwareDlg::OnBnClickedDisableWlModule()
//{
//	// TODO: Add your control notification handler code here
//}
