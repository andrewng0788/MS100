// WaferStat.cpp : implementation file
//

#include "stdafx.h"
#include "WaferStat.h"


// CWaferStat dialog

IMPLEMENT_DYNAMIC(CWaferStat, CDialog)
CWaferStat::CWaferStat(CWnd* pParent /*=NULL*/)
	: CDialog(CWaferStat::IDD, pParent)
{
}

CWaferStat::~CWaferStat()
{
}

void CWaferStat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAFSTATLISTCTRL, m_WaferStatListCtrl);
}


BEGIN_MESSAGE_MAP(CWaferStat, CDialog)
//	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
ON_BN_CLICKED(IDC_EXPORT, OnBnClickedExport)
END_MESSAGE_MAP()


// CWaferStat message handlers

BOOL CWaferStat::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CStringMapFile WaferRecord;

	ULONG i = 0;	
	ULONG ulTotalWaferNo = 0;	
	ULONG ulBond = 0;
	ULONG ulEmpty = 0;
	ULONG ulDefect = 0;
	ULONG ulChip = 0;
	ULONG ulInk = 0;
	ULONG ulRotate = 0;
	ULONG ulBadCut = 0;
	ULONG ulMissing = 0;
	ULONG ulTotal = 0;
	DOUBLE dYield = 0.0;	

	CString	szWaferID = "";
	CString szText = "";	


	m_WaferStatListCtrl.InsertColumn(0, "No.", LVCFMT_CENTER, 40);
	m_WaferStatListCtrl.InsertColumn(1, "Wafer ID", LVCFMT_CENTER, 80);
	m_WaferStatListCtrl.InsertColumn(2, "Bond", LVCFMT_CENTER, 50);
	m_WaferStatListCtrl.InsertColumn(3, "Defect", LVCFMT_CENTER, 50);
	m_WaferStatListCtrl.InsertColumn(4, "Chip", LVCFMT_CENTER, 50);
	m_WaferStatListCtrl.InsertColumn(5, "Ink", LVCFMT_CENTER, 50);
	m_WaferStatListCtrl.InsertColumn(6, "Rotate", LVCFMT_CENTER, 50);
	m_WaferStatListCtrl.InsertColumn(7, "Badcut", LVCFMT_CENTER, 50);
	m_WaferStatListCtrl.InsertColumn(8, "Empty", LVCFMT_CENTER, 50);
	m_WaferStatListCtrl.InsertColumn(9, "Missing", LVCFMT_CENTER, 50);
	m_WaferStatListCtrl.InsertColumn(10, "Total Selected", LVCFMT_CENTER, 90);
	m_WaferStatListCtrl.InsertColumn(11, "Yield %", LVCFMT_CENTER, 50);

	if (WaferRecord.Open(MSD_WAFER_RECORD_FILE, FALSE, FALSE) == 1)
	{
		ulTotalWaferNo	= (WaferRecord)["Total Wafer"];
		for (i = 1; i<=ulTotalWaferNo; i++)
		{
			szWaferID	= (WaferRecord)[i]["WaferID"];
			ulEmpty		= (WaferRecord)[i]["Empty"];
			ulBond		= (WaferRecord)[i]["Bond"];
			ulDefect	= (WaferRecord)[i]["Defect"];
			ulChip		= (WaferRecord)[i]["Chip"];
			ulInk		= (WaferRecord)[i]["Ink"];
			ulRotate	= (WaferRecord)[i]["Rotate"];
			ulBadCut	= (WaferRecord)[i]["BadCut"];
			ulMissing	= (WaferRecord)[i]["MissingDie"];
			ulTotal		= (WaferRecord)[i]["TotalDie"];
			dYield		= (WaferRecord)[i]["Yield"];

			szText.Format("%d", i);
			m_WaferStatListCtrl.InsertItem(i-1, szText, 0);
			m_WaferStatListCtrl.SetItemText(i-1, 1, szWaferID);

			szText.Format("%d", ulBond);
			m_WaferStatListCtrl.SetItemText(i-1, 2, szText);

			szText.Format("%d", ulDefect);
			m_WaferStatListCtrl.SetItemText(i-1, 3, szText);

			szText.Format("%d", ulChip);
			m_WaferStatListCtrl.SetItemText(i-1, 4, szText);

			szText.Format("%d", ulInk);
			m_WaferStatListCtrl.SetItemText(i-1, 5, szText);

			szText.Format("%d", ulRotate);
			m_WaferStatListCtrl.SetItemText(i-1, 6, szText);

			szText.Format("%d", ulBadCut);
			m_WaferStatListCtrl.SetItemText(i-1, 7, szText);

			szText.Format("%d", ulEmpty);
			m_WaferStatListCtrl.SetItemText(i-1, 8, szText);

			szText.Format("%d", ulMissing);
			m_WaferStatListCtrl.SetItemText(i-1, 9, szText);

			szText.Format("%d", ulTotal);
			m_WaferStatListCtrl.SetItemText(i-1, 10, szText);

			szText.Format("%3.2f", dYield);
			m_WaferStatListCtrl.SetItemText(i-1, 11, szText);
		}

		WaferRecord.Close();
	}
	else
	{
		CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferRecord msd");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CWaferStat::OnBnClickedExport()
{
	// TODO: Add your control notification handler code here
	CStringMapFile WaferRecord;

	ULONG i = 0;	
	ULONG ulTotalWaferNo = 0;	
	ULONG ulBond = 0;
	ULONG ulEmpty = 0;
	ULONG ulDefect = 0;
	ULONG ulChip = 0;
	ULONG ulInk = 0;
	ULONG ulRotate = 0;
	ULONG ulBadCut = 0;
	ULONG ulMissing = 0;
	ULONG ulTotal = 0;
	DOUBLE dYield = 0.0;	

	CString	szWaferID = "";
	CString szText = "";	

	FILE *fp = NULL;	//Klocwork

	CFileDialog FileDialog(TRUE);
	
	FileDialog.m_pOFN->lpstrInitialDir = gszROOT_DIRECTORY + "\\Exe";
	FileDialog.m_pOFN->lpstrFilter = "ALL Files\0*.*\0\0";
	FileDialog.m_pOFN->nFilterIndex = 1;

	if (FileDialog.DoModal() == IDOK)
	{
		if (WaferRecord.Open(MSD_WAFER_RECORD_FILE, FALSE, FALSE) == 1)
		{
			errno_t nErr = fopen_s(&fp, (LPCTSTR)FileDialog.GetFileName(), "w");
			if ((nErr == 0) && (fp != NULL))		//Klocwork
			{
				fprintf(fp, "Wafer summary\n");
				fprintf(fp, "No., WaferID, Bond, Defect, Chip, Ink, Rotate, Badcut, Empty, MissingDie, Total, Yield\n");

				ulTotalWaferNo	= (WaferRecord)["Total Wafer"];
				for (i = 1; i<=ulTotalWaferNo; i++)
				{
					szWaferID	= (WaferRecord)[i]["WaferID"];
					ulEmpty		= (WaferRecord)[i]["Empty"];
					ulBond		= (WaferRecord)[i]["Bond"];
					ulDefect	= (WaferRecord)[i]["Defect"];
					ulChip		= (WaferRecord)[i]["Chip"];
					ulInk		= (WaferRecord)[i]["Ink"];
					ulRotate	= (WaferRecord)[i]["Rotate"];
					ulBadCut	= (WaferRecord)[i]["BadCut"];
					ulMissing	= (WaferRecord)[i]["MissingDie"];
					ulTotal		= (WaferRecord)[i]["TotalDie"];
					dYield		= (WaferRecord)[i]["Yield"];


					szText.Format("%d," + szWaferID + ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%3.2f\n", 
								i, ulBond, 
								ulDefect, ulChip, ulInk, 
								ulRotate, ulBadCut, ulEmpty, 
								ulMissing, ulTotal, dYield);

					fprintf(fp, szText);
				}

				fclose(fp);
			}

			WaferRecord.Close();
		}
		else
		{
			CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferRecord msd");
		}
	}

	//Set current directory back to original
	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
}
