#pragma once
#include "afxwin.h"


// CPromptBox dialog

class CPromptBox : public CDialog
{
	DECLARE_DYNAMIC(CPromptBox)

public:
	CPromptBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPromptBox();

// Dialog Data
	enum { IDD = IDD_PROMPT_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_InputBox;
	CString m_szInputValue;
	CStatic m_InputTitle;
	CString m_strCaption;
};
