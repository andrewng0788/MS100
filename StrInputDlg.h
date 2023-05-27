#pragma once


// CStrInputDlg dialog

class CStrInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CStrInputDlg)

public:
	CStrInputDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStrInputDlg();

// Dialog Data
	enum { IDD = IDD_STRING_ENTRY };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_szData;
	CString m_szTitle;	//v4.04
};
