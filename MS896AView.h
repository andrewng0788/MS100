/////////////////////////////////////////////////////////////////
// MS896AView.h : interface of the CMS896AView class
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


class CMS896AView : public CFormView
{
protected: // create from serialization only
	CMS896AView();
	DECLARE_DYNCREATE(CMS896AView)

public:
	enum{ IDD = IDD_MS896A_FORM };
	CListBox m_lbMessage;
	CButton	 m_btnGroupId;
	CButton	 m_btnModule;
	CButton	 m_btnAutoStart;
	CButton	 m_btnStart;
	CButton	 m_btnLogMessage;
	CButton  m_btnExecHmi;

// Attributes
public:
	CMS896ADoc* GetDocument() const;

// Operations
public:
	VOID DisplayMessage(const CString& szMessage);
	VOID SetGroupID(CString& szGroupID);
	VOID SetModule(CString& szGroupID);

// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CMS896AView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnGroupId();
	afx_msg void OnAutoStart();
	afx_msg void OnStart();
	afx_msg void OnSelectHardware();
	afx_msg void OnOptions();
	afx_msg void OnLogMsg();
	afx_msg void OnExecuteHmi();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MS896AView.cpp
inline CMS896ADoc* CMS896AView::GetDocument() const
   { return reinterpret_cast<CMS896ADoc*>(m_pDocument); }
#endif

