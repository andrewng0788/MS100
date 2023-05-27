#pragma once


// CPrCrossHairDlg dialog

class CPrCrossHairDlg : public CDialog
{
	DECLARE_DYNAMIC(CPrCrossHairDlg)

public:
	CPrCrossHairDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPrCrossHairDlg();

	VOID SetCursorXY(LONG lx, LONG ly);

// Dialog Data
	enum { IDD = IDD_DIALOG_PR_CROSSHAIR };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL PRDrawCursor();

private:

	LONG m_lCursorX;
	LONG m_lCursorY;

	DECLARE_MESSAGE_MAP()
public:
	long m_lCursorStepSize;
	LONG GetCursorX();
	LONG GetCursorY();
	afx_msg void OnBnClickedButtonup();
	afx_msg void OnBnClickedButtondn();
	afx_msg void OnBnClickedButtonlt();
	afx_msg void OnBnClickedButtonrt();
};
