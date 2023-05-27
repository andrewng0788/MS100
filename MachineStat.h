#pragma once
#include "afxcmn.h"
#include "BL_Constant.h"
#include "WL_Constant.h"
#include "afxwin.h"


// CMachineStat dialog

class CMachineStat : public CDialog
{
	DECLARE_DYNAMIC(CMachineStat)

public:
	CMachineStat(CWnd* pParent = NULL);   // standard constructor
	VOID ShowClearAllButton(BOOL bSet);
	virtual ~CMachineStat();

// Dialog Data
	enum { IDD = IDD_MC_HISTORY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExport();
	virtual BOOL OnInitDialog();
	CListCtrl m_MachineStatCtrlList;
	CString szFileName;
	BOOL bSingleColumnFile;
	int nStartLeft, nStartTop;
	CButton m_ClearAll;
	BOOL	m_bShowClearAll;
	CRichEditCtrl m_PlainTextRichCtrl;
};
