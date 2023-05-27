/////////////////////////////////////////////////////////////////
//	Hardware.h : header file for Dialog box used to configure
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

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CHardwareDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CHardwareDlg : public CDialog
{
public:
	CHardwareDlg(CWnd* pParent = NULL);

	// Dialog Data
	//{{AFX_DATA(CHardwareDlg)
	enum { IDD = IDD_HARDWARE };
	BOOL	m_bHardware;
	BOOL	m_bMod1Card1;
	BOOL	m_bMod1Card2;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHardwareDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CHardwareDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bDisableBHModule;
	BOOL m_bDisableBTModule;
	BOOL m_bDisableBLModule;
//	afx_msg void OnBnClickedDisableWlModule();
	BOOL m_bDisableWLModule;
	BOOL m_bDisableWTModule;
	BOOL m_bDisableWLModuleWithExp;
	BOOL m_bDisableNLModule;
};
