// MachineStat.cpp : implementation file
//

#include "stdafx.h"
#include "MS896A.h"
#include "MachineStat.h"


// CMachineStat dialog

IMPLEMENT_DYNAMIC(CMachineStat, CDialog)
CMachineStat::CMachineStat(CWnd* pParent /*=NULL*/)
	: CDialog(CMachineStat::IDD, pParent)
{
	m_bShowClearAll = TRUE;
}

CMachineStat::~CMachineStat()
{
}

void CMachineStat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MC_LIST, m_MachineStatCtrlList);
	DDX_Control(pDX, IDEXPORT, m_ClearAll);
	DDX_Control(pDX, IDC_RICHEDIT, m_PlainTextRichCtrl);
}


BEGIN_MESSAGE_MAP(CMachineStat, CDialog)
	ON_BN_CLICKED(IDEXPORT, OnBnClickedExport)
END_MESSAGE_MAP()


// CMachineStat message handlers

void CMachineStat::OnBnClickedExport()
{
	// TODO: Add your control notification handler code here

	//Delete This File & clear all text
	if ( szFileName.IsEmpty() == FALSE )
	{
		DeleteFile(szFileName);
		m_MachineStatCtrlList.DeleteAllItems();
		m_MachineStatCtrlList.RedrawWindow();
	}
}

VOID CMachineStat::ShowClearAllButton(BOOL bSet)
{
	m_bShowClearAll = bSet;
}

BOOL CMachineStat::OnInitDialog()
{
	CDialog::OnInitDialog();


	// TODO:  Add extra initialization here
	CStdioFile cfHistoryFile;
	CString szData = "";
	CString szTitle = "";
	CString szValue = "";
	BOOL bFileExist = FALSE;
	int nCol = 0;
	int nTitleNo = 0;
	int nLineNo = 0;
	int nColWidth = 0;


	RECT stRect;
	CDialog::GetWindowRect(&stRect);
	CDialog::SetWindowPos(&CWnd::wndTopMost, nStartLeft, nStartTop, (stRect.right-stRect.left+1), (stRect.bottom-stRect.top+1), SWP_SHOWWINDOW);


	//Start to read file
	bFileExist = cfHistoryFile.Open(szFileName, 
		CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText);


	//If this file is not exist, create a new one
	if ( bFileExist == FALSE )
	{
		if ( (szFileName != BL_HISTORY_NAME) || (szFileName != WL_HISTORY_NAME) || (szFileName != BL_BINFRAME_STATUS_SUMMARY))
		{
			return FALSE;
		}

		bFileExist = cfHistoryFile.Open(szFileName, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);

		if ( szFileName == BL_HISTORY_NAME )
		{		
			szData = BL_HISTORY_TITLE;
		}
		else if (szFileName == BL_BINFRAME_STATUS_SUMMARY)
		{
			szData = BL_BINFRAME_STATUS_SUMMARY_TITLE;
		}
		else
		{
			szData = WL_HISTORY_TITLE;
		}

		cfHistoryFile.WriteString(szData);

		cfHistoryFile.WriteString("\n");
		cfHistoryFile.Close();
	}
	else
	{
		cfHistoryFile.Close();
	}


	if ( (szFileName == BL_HISTORY_NAME) || (szFileName == WL_HISTORY_NAME) || (szFileName == BL_BINFRAME_STATUS_SUMMARY))
	{
		bSingleColumnFile = FALSE;
	}
	else
	{
		bSingleColumnFile = TRUE;
	}

	//Disable ClearAll button if show single column file 
	if (m_bShowClearAll == TRUE)
	{
		m_ClearAll.ShowWindow(!bSingleColumnFile);
	}
	else
	{
		m_ClearAll.ShowWindow(FALSE);
	}

	m_MachineStatCtrlList.ShowWindow(!bSingleColumnFile);
	m_PlainTextRichCtrl.ShowWindow(bSingleColumnFile);


	//Get Title & list out data
	bFileExist = cfHistoryFile.Open(szFileName, 
		CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText);
	
	if ( bSingleColumnFile == FALSE )
	{
		if ( bFileExist == TRUE )
		{
			cfHistoryFile.ReadString(szData);

			nTitleNo = 0;
			szTitle = "RecNo";
			nColWidth = 3*m_MachineStatCtrlList.GetStringWidth(szTitle)/2;
			m_MachineStatCtrlList.InsertColumn(nTitleNo, szTitle, LVCFMT_CENTER, nColWidth);

			//Read Title
			nTitleNo = 1;
			while(1)
			{
				nCol = szData.Find(",");
				if ( nCol != -1 )
				{
					szTitle = szData.Left(nCol);
					nColWidth = 3*m_MachineStatCtrlList.GetStringWidth(szTitle)/2;
					m_MachineStatCtrlList.InsertColumn(nTitleNo, szTitle, LVCFMT_CENTER, nColWidth);
					nTitleNo++;
					szData = szData.Mid(nCol + 1);
				}
				else
				{
					if ( szData.IsEmpty() == FALSE )
					{
						nColWidth = 3*m_MachineStatCtrlList.GetStringWidth(szData)/2;
						m_MachineStatCtrlList.InsertColumn(nTitleNo, szData, LVCFMT_CENTER, nColWidth);
					}
					break;
				}
			}

			//Read Data
			nLineNo = 0;
			while (cfHistoryFile.ReadString(szData) != NULL)
			{
				//Display Line no.
				szValue.Format("%d", nLineNo+1); 
				m_MachineStatCtrlList.InsertItem(nLineNo, szValue, 0);

				//Display column value
				nTitleNo = 1;
				while(1)
				{
					nCol = szData.Find(",");
					if ( nCol != -1 )
					{
						szValue = szData.Left(nCol);
						m_MachineStatCtrlList.SetItemText(nLineNo, nTitleNo, szValue);
						szData = szData.Mid(nCol + 1);
						nTitleNo++;
					}
					else
					{
						if ( szData.IsEmpty() == FALSE )
						{
							m_MachineStatCtrlList.SetItemText(nLineNo, nTitleNo, szData);
						}
						break;
					}
				}
				nLineNo++;
			}
			cfHistoryFile.Close();
		}
	}
	else
	{
		if ( bFileExist == TRUE )
		{
			//Read Data
			szValue = "File " + szFileName + "\n\n";

			while (cfHistoryFile.ReadString(szData) != NULL)
			{
				szValue += (szData + "\n");
			}
			cfHistoryFile.Close();

			m_PlainTextRichCtrl.SetWindowText(szValue);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
