#include "stdafx.h"
#include "MarkConstant.h" 
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "WT_Log.h"
#include "math.h"
#include "PrescanUtility.h"
#include "PrescanInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CWaferTable::CleanProberTempData()
{
	m_szaRawData.RemoveAll();
	m_dwaGrade.RemoveAll();
	m_dwaRow.RemoveAll();
	m_dwaCol.RemoveAll();
}

BOOL CWaferTable::ParseProberTempFile()
{
	CStdioFile cfTempFile;

	if (cfTempFile.Open(CP_PROBE_TEMP_FILE, //	process raw data before gen. output map file
						CFile::modeNoTruncate | CFile::modeRead | CFile::shareExclusive | CFile::typeText) == FALSE)
	{
		return FALSE;
	}

	CString szData;
	while (cfTempFile.ReadString(szData))
	{
		if( szData.GetLength()==0 )
		{
			continue;
		}

		CString szRow = szData;
		INT nCol = szData.Find(",");
		if (nCol != -1)
		{
			szRow = szData.Left(nCol);
			szData = szData.Right(szData.GetLength() - nCol - 1);
		}

		CString szCol = szData;
		nCol = szData.Find(",");
		if (nCol != -1)
		{
			szCol = szData.Left(nCol);
			szData = szData.Right(szData.GetLength() - nCol - 1);
		}

		CString szGrade = szData;
		CString szRawData = "";
		nCol = szData.Find(",");
		if (nCol != -1)
		{
			szGrade = szData.Left(nCol);
			szRawData = szData.Right(szData.GetLength() - nCol - 1);
		}

		if (szRawData.GetLength() > 0)
		{
			nCol = szRawData.ReverseFind(';');
			if (nCol == szRawData.GetLength() - 1)
			{
				szRawData = szRawData.Left(nCol);
			}
		}

		LONG lUserRow = atoi(szRow);
		LONG lUserCol = atoi(szCol);
		LONG ucGrade  = atoi(szGrade);

		m_szaRawData.Add(szRawData);
		m_dwaGrade.Add(ucGrade);
		m_dwaRow.Add(lUserRow);
		m_dwaCol.Add(lUserCol);
	}

	cfTempFile.Close();

	return TRUE;
}

	
BOOL CWaferTable::WriteProberMapFile()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (m_ulPrescanMethod == CP_SCAN_CONT_PROBE_WAFER)
	{
		m_ulPrescanMethod = CP_SCAN_PROBE_EXACT_MAP;
		SaveData();
	}
	CleanProberTempData();	// clean raw data in memory
	if (ParseProberTempFile() == FALSE)
	{
		SetAlert(IDS_WT_GEN_MAP_FAIL);
		CleanProberTempData();
		SetErrorMessage("Fail to parse prober temp file");
		return FALSE;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (m_szProberOutputMapFilename.IsEmpty() == TRUE)
	{
		CString szBarcode = "";

		SetAlert(IDS_WT_EMPTY_OUTPUT_MAP_FILE_NAME);
		
		UpdateHmiVariableData("szWaferIdInput", "");
		while (szBarcode == "")
		{
			HmiStrInputKeyboard("Please input barcode", szBarcode);
		}

		if( pApp->GetCustomerName()=="ZhongGu" )
		{
			szBarcode = szBarcode.MakeUpper();
		}
		m_szProberOutputMapFilename = szBarcode;
	}

	if( pApp->GetCustomerName()=="ZhongGu" && m_szProberOperatorID.IsEmpty() )
	{
		CString szBarcode = " ";

		while (1)
		{
			HmiStrInputKeyboard("Please input operator ID", szBarcode);
			if( szBarcode!=" " && szBarcode!="" )
			{
				break;
			}
		}

		m_szProberOperatorID = szBarcode;
	}

	//Prescan total
	ULONG ulSampleLeft = 0, ulTotalTested =0;
	CUIntArray aulAvailableGradeList;
	m_WaferMapWrapper.GetAvailableGradeList(aulAvailableGradeList);
	for (int k=0; k<aulAvailableGradeList.GetSize(); k++)
	{
		ULONG ulDieTotal = 0;
		ULONG ulLeft = 0;
		ULONG ulPick = 0;
		ULONG ulGradeNoOffset = aulAvailableGradeList.GetAt(k) - m_WaferMapWrapper.GetGradeOffset();
		m_WaferMapWrapper.GetStatistics(aulAvailableGradeList.GetAt(k), ulLeft, ulPick, ulDieTotal);

		ulTotalTested = ulTotalTested + ulLeft;


		if (ulGradeNoOffset != m_ucPrescanDefectGrade && ulGradeNoOffset != m_ucPrescanBadCutGrade &&ulGradeNoOffset != m_ucPrescanEdgeGrade)
		{
			ulSampleLeft = ulSampleLeft + ulLeft;
		}
	}

	BOOL bGenOutput = TRUE;
	if (GetPSCMode() != PSC_NONE)
	{
		CTime tTime;
		tTime = CTime::GetCurrentTime();
		CString szDateTime = tTime.Format("%Y%m%d%H%M");
		CString szOutputMapFilename = m_szProberOutputMapFilename + szDateTime + ".csv";

		bGenOutput = OutputMapFileCustomerSelection(TRUE, szOutputMapFilename, ulSampleLeft, ulTotalTested);
		if (bGenOutput)
		{
			m_szOutputMapFullPath = m_szPrbDCOutputPath + "\\" + szOutputMapFilename;
			SaveWaferTblData();
			CString szFilename = PRESCAN_RESULT_FULL_PATH + szOutputMapFilename;
			CopyFileWithQueue(szFilename, m_szOutputMapFullPath, FALSE);
		}
	}
	else
	{
		CString szOutputMapFilename;
		INT nTemp = pApp->GetProfileInt(gszPROFILE_SETTING, _T("CP100 2 Output Files"), 0);
		if (nTemp == 1)
		{
			szOutputMapFilename = m_szProberOutputMapFilename + "_000" + ".csv";
			bGenOutput = OutputMapFileCustomerSelection(FALSE, szOutputMapFilename, ulSampleLeft, ulTotalTested);
		}
		if (bGenOutput)
		{
			szOutputMapFilename = m_szProberOutputMapFilename + ".csv";
			if( !IsProbingEnd() && pApp->GetCustomerName()=="ZhongGu" )
			{
				CTime tTime;
				tTime = CTime::GetCurrentTime();
				CString szDateTime = tTime.Format("_%Y%m%d%H%M");
				szOutputMapFilename = m_szProberOutputMapFilename + szDateTime + ".csv";
			}
			if( m_lCpProbeDieLimit>0 )	// output map file name change
			{
				szOutputMapFilename = m_szProberOutputMapFilename + "_EQC.csv";
			}
			bGenOutput = OutputMapFileCustomerSelection(FALSE, szOutputMapFilename, ulSampleLeft, ulTotalTested);
			if (bGenOutput)
			{
				CString szSrcFile = PRESCAN_RESULT_FULL_PATH + szOutputMapFilename;
				
				UINT unRunResort = pApp->GetProfileInt(gszPROFILE_SETTING, gszPROFILE_EXECUTE_RESORT, 1);
				if (unRunResort > 0)
				{
					if (pApp->IsApplicationRunning("Resorting.exe", 1, FALSE))
					{
						CString szTgtFile = gszUSER_DIR_MAPFILE_OUTPUT + "\\" + szOutputMapFilename;
						CopyFile(szSrcFile, szTgtFile, FALSE);
						Sleep(2000);

						CString szDataFile;
						for (LONG lCounter = 0; lCounter < 500; lCounter++)
						{
							WIN32_FIND_DATA FileData;
							HANDLE hSearch = FindFirstFile(gszUSER_DIR_MAPFILE_DATACSV + "\\" + "*.csv", &FileData);
							do
							{
								szDataFile = FileData.cFileName;
								if ((szDataFile != ".") && (szDataFile != "..") && szDataFile != "")
								{
									break;
								}
							} while (FindNextFile(hSearch, &FileData) == TRUE);
							// Close the search handle. 
							if (FindClose(hSearch) == FALSE) 
							{ 
								break;
							} 
							if ((szDataFile != ".") && (szDataFile != "..") && szDataFile.IsEmpty() == FALSE)
							{
								break;
							}
							Sleep(1000);
						}

						if (szDataFile.IsEmpty() == FALSE)
						{
							Sleep(1000);
							szSrcFile = gszUSER_DIR_MAPFILE_DATACSV + "\\" + szDataFile;
							if (m_szPrbCsvDataOutputPath.IsEmpty() == FALSE)
							{
								szTgtFile = m_szPrbCsvDataOutputPath + "\\" + szDataFile;
								CopyFileWithQueue(szSrcFile, szTgtFile, FALSE);
							}
							if (m_szPrbDataOutputPath.IsEmpty() == FALSE)
							{
								szTgtFile = m_szPrbDataOutputPath + "\\" + szDataFile;
								CopyFileWithQueue(szSrcFile, szTgtFile, FALSE);
							}
							DeleteFile(szSrcFile);
						}
						else
						{
							HmiMessage_Red_Back("Can not find Data.csv.\nPlease Check Resorting Program!", "CP100");
						}
					}
					else
					{
						HmiMessage_Red_Back("Online Resorting programe is gone.\nPlease check!", "CP100");
					}
				}
			}
		}
	}

	// here pass the path to tester for CSV_DATA and DATA
	CleanProberTempData();
	if (bGenOutput == FALSE)
	{
		SetAlert(IDS_WT_GEN_MAP_FAIL);
		SetErrorMessage("Fail to write the map file");
		return FALSE;
	}
	CopyFile(CP_PROBE_TEMP_FILE, CP_PROBE_BACKUP_FILE, FALSE);
	DeleteFile(CP_PROBE_TEMP_FILE);		DeleteFile(CP_ITEM_GRADE_DATA_FILE);

	BOOL bReturn = TRUE;
	if (GetPSCMode() != PSC_NONE)
	{
		CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
		pAppMod->ShowHmi();		// After input, restore the application to background
		OpenWaitingAlert();
		m_bDCResultFail	= FALSE;
		IPC_CServiceMessage stMsg;
		int nConvID = m_comClient.SendRequest("BondPrStn", "LoadSourceAndProbeData", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 500000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		if (m_bDCFailStop && bReturn == FALSE)
		{
			m_bDCResultFail	= TRUE;
		}
		CloseWaitingAlert();
	}

	if (IsBurnInCP() == FALSE)
	{
		LONG lAlarmStatus = GetAlarmLamp_Status();
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);		// SET ALARM

		SetAlert(IDS_WT_GEN_MAP_SUCCESS);		//WT0031
		SetStatusMessage("Manaul generate map file success");

		SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);			// SET BACK

		m_szProberOutputMapFilename = "";
		m_szProberOperatorID		= "";
	}

	return bReturn;
}

BOOL CWaferTable::OutputMapFileCustomerSelection(CONST BOOL bMapOrientation, CString szOutputMapFilename, ULONG ulSampleLeft, ULONG ulTotalTested)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szCustomerName = pApp->GetCustomerName();

	if (szCustomerName.GetLength() == 0)
	{
		HmiMessage_Red_Back("Generate Output file error! \nPlease Check Resorting Program!", "CP100");
		return FALSE;
	}

	BOOL bInMapOrientation = bMapOrientation;
	if (bInMapOrientation == FALSE)
	{
		if (!(m_ucProbedMapRotation > 0 || m_bProbedMapHoriFlip || m_bProbedMapVertFlip))
		{
			bInMapOrientation = TRUE;
		}
	}

	return TRUE;
}

