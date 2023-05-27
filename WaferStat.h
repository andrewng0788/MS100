#pragma once
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "MS896A.h"
#include "FileUtil.h"


// CWaferStat dialog

class CWaferStat : public CDialog
{
	DECLARE_DYNAMIC(CWaferStat)

public:
	CWaferStat(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaferStat();

// Dialog Data
	enum { IDD = IDD_WAFERSTAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedExport();
	CListCtrl m_WaferStatListCtrl;
};
