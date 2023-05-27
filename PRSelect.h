#pragma once
#include "afxwin.h"


// PRSelect dialog

class PRSelect : public CDHtmlDialog
{
	DECLARE_DYNCREATE(PRSelect)

public:
	PRSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~PRSelect();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_PRSelect, IDH = IDR_HTML_PRSELECT1 };



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	CComboBox cbChannel;
	CComboBox cbType;
	long m_lID;
	afx_msg void OnEnChangeEdit2();
	long m_lCount;
	long m_lDelay;
	CString m_szChannel, m_szType;
	afx_msg void OnBnClickedOk();
	BOOL GetPara(CString &szChannel,CString &szType,LONG &lDelay,LONG &lCount,LONG &lID);

	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeCombo2();
};
