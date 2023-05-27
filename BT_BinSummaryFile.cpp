#include "stdafx.h" 
#include "BT_BinSummaryFile.h"
#include "stdio.h"
#include "StringMapFile.h"
#include "MS896A_Constant.h"
#include "LogFileUtil.h"
#include "MS896A.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CBinSummaryFile::CBinSummaryFile()
{
	m_szSummaryFilePkgName = "";
	m_szErrorCode = "";
	m_bEnableGradeAssign = FALSE;
	m_bSelectGradeBaseOnBinBlockGrade = FALSE;
}

BOOL CBinSummaryFile::LoadData()
{
	return TRUE;
}

BOOL CBinSummaryFile::GetEnableGradeAssign()
{
	return m_bEnableGradeAssign;
}

BOOL CBinSummaryFile::GetSelectGradeBaseOnBinBlockGrade()
{
	return m_bSelectGradeBaseOnBinBlockGrade;
}

BOOL CBinSummaryFile::ReadBinSummaryFile(CString szFilename, BOOL bUseOptBinCountDynAssignGrade)
{
	// Reset all prev status before loading the file
	DeleteFile(MSD_BIN_SUMMARY_FILE);

	//ClearTotalBinCount();
	//m_aGradeInfo.RemoveAll();
	ClearData();
	
	CStdioFile cfSetupFile;
	CStringMapFile BinSummaryFile;
	CString szContentHeader;
	CStringArray szaContentHeader;

	if (cfSetupFile.Open(szFilename, CFile::modeRead) == FALSE)
	{
		m_szErrorCode = "Fail to open summary file: " + szFilename;
		return FALSE;
	}

	if (BinSummaryFile.Open(MSD_BIN_SUMMARY_FILE, FALSE, TRUE) != 1)
	{
		m_szErrorCode = "Fail to read bin summary file: " + MSD_BIN_SUMMARY_FILE;
		cfSetupFile.Close();
		return FALSE;
	}

	if (ReadHeader(cfSetupFile, BinSummaryFile) == FALSE)
	{
		m_szErrorCode = "Fail to open summary file header";
		cfSetupFile.Close();
		BinSummaryFile.Update();
		BinSummaryFile.Close();
		return FALSE;
	}

	// for the customer that need both lextar and other format
	// Set lextar format as default and so is true 
	//and other format need to add a field to disable it
	// also guard by feature, no need to worry it will affect other customers
	m_bEnableGradeAssign = CheckNeedEnableGradeAssign(bUseOptBinCountDynAssignGrade);
	m_bSelectGradeBaseOnBinBlockGrade = CheckSelectGradeBaseOnBinBlockGrade();
	
	if (DecodeContentHeader(cfSetupFile, szaContentHeader) == FALSE)
	{
		m_szErrorCode = "Fail to read content header (Wafer IDs)";
		cfSetupFile.Close();
		BinSummaryFile.Update();
		BinSummaryFile.Close();
		return FALSE;
	}

	if (ReadConent(cfSetupFile, BinSummaryFile, szaContentHeader) == FALSE)
	{
		m_szErrorCode = "Fail to read content";
		cfSetupFile.Close();
		BinSummaryFile.Update();
		BinSummaryFile.Close();
		return FALSE;
	}

	cfSetupFile.Close();
	BinSummaryFile.Update();
	BinSummaryFile.Close();

	if (m_bEnableBinSumWaferLot == TRUE)
	{
		if (CreateWaferLotCheckDatabase() == FALSE)
		{
			m_szErrorCode = "Fail to create WaferLotCheckDatabase";
			return FALSE;
		}
	}

	return TRUE;
}


VOID CBinSummaryFile::ClearData()
{
	m_bSelectGradeBaseOnBinBlockGrade = FALSE;
	m_bEnableGradeAssign = FALSE;
	m_aGradeInfo.RemoveAll();
	m_szWaferIdsInLot.RemoveAll();
	m_szaHeaderInfo.RemoveAll();
}

BOOL CBinSummaryFile::IsContent(CString szData)
{
	INT nCol;
	
	nCol = szData.Find(BIN_BIN_CODE);
	if (nCol != -1)
	{
		return TRUE;
	}

	//v4.44T4	//Genesis 3F pkgSort
	nCol = szData.Find(BIN_BIN_CODE2);
	if (nCol != -1)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBinSummaryFile::IsEndOfFile(CString szData)
{
	INT nCol;
	
	// End of file is "Total count" for Lextar
	nCol = szData.Find(EOF_TOTAL_COUNT);

	if (nCol != -1)
	{
		return TRUE;
	}

	// End of file is "TotalCount" for Epistar
	nCol = szData.Find(EOF_TOTAL_COUNT2);

	if (nCol != -1)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBinSummaryFile::IsGradeAssignField(CString szData)
{
	INT nCol;
	
	nCol = szData.Find(GRADE_ASSIGN);

	if (nCol != -1)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBinSummaryFile::IsSelectGradeBaseOnBinBlockGrade(CString szData)
{
	INT nCol;
	
	nCol = szData.Find(GRADE_BASE_ON_BINBLK);

	if (nCol != -1)
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CBinSummaryFile::ReadHeader(CStdioFile& cfSetupFile, CStringMapFile& BinSummaryFile)
{
	CStringArray szaHeader;
	CString szData, szKey;

	if (DecodeHeaderInfo(cfSetupFile) == FALSE)
	{
		return FALSE;
	}

	SaveHeaderData(BinSummaryFile);
	
	return TRUE;
}

BOOL CBinSummaryFile::DecodeHeaderInfo(CStdioFile& cfSetupFile)
{
	CString szData;
	CString szHeader;
	CString szContent;

	INT nCol = -1;

	while (cfSetupFile.ReadString(szData))
	{
		szContent = "";

		// Check reach content part header i.e. BIN,Bin_CODE,WaferID1, WaferID2 , .... ,TotalCount
		if (IsContent(szData) == TRUE)
		{
			//szContentHeader = szData;
			return TRUE;
		}

		nCol = szData.Find(",");
		
		if (nCol != -1 )
		{
			szHeader = szData.Left(nCol);
			
			if (szData.GetLength() > nCol +1)
			{
				szContent = szData.Mid(nCol + 1);
			}

			m_szaHeaderInfo.SetAt(szHeader, szContent);
		}
	}


	return FALSE;
}

BOOL CBinSummaryFile::SaveHeaderData(CStringMapFile& BinSummaryFile)
{
	POSITION oPos;
	CString szKey;
	CString szData;
	
	for	(oPos = m_szaHeaderInfo.GetStartPosition(); oPos != NULL; )
	{
		m_szaHeaderInfo.GetNextAssoc( oPos, szKey, szData );
		(BinSummaryFile)[HEADER][szKey] = szData;
	}

	return TRUE;
}

BOOL CBinSummaryFile::DecodeContentHeader(CStdioFile& cfSetupFile, CStringArray& szaContentHeader)
{
	CString szData;
	cfSetupFile.SeekToBegin();
	
	while (cfSetupFile.ReadString(szData))
	{
		// Search for content part
		// i.e. BIN,Bin_CODE,WaferID1, WaferID2 , .... ,TotalCount
		if (IsContent(szData) == TRUE)
		{
			GetDataFromContentString(szData,szaContentHeader);
			
			SaveLotWaferIds(szaContentHeader);

			if (szaContentHeader.GetSize() != 0)
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CBinSummaryFile::CheckNeedEnableGradeAssign(BOOL bUseOptBinCountDynAssignGrade)
{
	/*
	CString szValue,szData;

	cfSetupFile.SeekToBegin();
	
	while (cfSetupFile.ReadString(szData))
	{
		if (IsGradeAssignField(szData) == TRUE)
		{
			GetDataFromHeaderString(szData, szValue);
			
			if (szValue == TRUE_STR)
			{
				return TRUE;
			}
		
			return FALSE;
		}
	}
	*/


	POSITION oPos;
	CString szKey;
	CString szData;
	BOOL bReturn = FALSE;

	// default is true for Lextar cases
	if (bUseOptBinCountDynAssignGrade == TRUE)
	{
		bReturn =  TRUE;
	}

	// for FATC case check the file header, must need to have a key to specify
	for	(oPos = m_szaHeaderInfo.GetStartPosition(); oPos != NULL; )
	{
		m_szaHeaderInfo.GetNextAssoc( oPos, szKey, szData );
		
		if (IsGradeAssignField(szKey) == TRUE)
		{
			if (szData == TRUE_STR)
			{
				bReturn = TRUE;
			}
			else
			{
				bReturn = FALSE;
			}
		}
	}

	return bReturn;
}

BOOL CBinSummaryFile::CheckSelectGradeBaseOnBinBlockGrade()
{
	POSITION oPos;
	CString szKey;
	CString szData;
	
	for	(oPos = m_szaHeaderInfo.GetStartPosition(); oPos != NULL; )
	{
		m_szaHeaderInfo.GetNextAssoc( oPos, szKey, szData );
		
		if (IsSelectGradeBaseOnBinBlockGrade(szKey) == TRUE)
		{
			if (szData == TRUE_STR)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL CBinSummaryFile::ReadConent(CStdioFile& cfSetupFile, CStringMapFile& BinSummaryFile, CStringArray& szaContentHeader)
{
	CString szData;
	CStringArray szaContent;

	while (cfSetupFile.ReadString(szData))
	{
		szaContent.RemoveAll();

		if  (IsEndOfFile(szData) == TRUE)
		{
			return TRUE;
		}

		GetDataFromContentString(szData, szaContent);

		// compare the content length
		if (szaContent.GetSize() != szaContentHeader.GetSize())
		{
			return FALSE;
		}
		
		// update to corresponding arrays and struct
		if (UpdateAndSaveContent(BinSummaryFile, szaContentHeader, szaContent) == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBinSummaryFile::SaveLotWaferIds(CStringArray& szaContentHeader)
{
	CString szWaferId = "";

	for (INT i=0; i<szaContentHeader.GetSize(); i++)
	{
		szWaferId = szaContentHeader.GetAt(i);
//4.53D26 modify wafer ID No
		// Ignore BIN, BIN_CODE, TOTAL_COUNT items
		if (CompareString(szWaferId, BINCODE) == TRUE || CompareString(szWaferId, EOF_TOTAL_COUNT2) == TRUE
			|| CompareString(szWaferId, BIN) == TRUE || CompareString(szWaferId, PKG_NAME) == TRUE )
		{
			continue;
		}

		//AfxMessageBox(szWaferId);

		m_szWaferIdsInLot.Add(szWaferId);
	}

	return TRUE;
}

BOOL CBinSummaryFile::UpdateAndSaveContent(CStringMapFile& BinSummaryFile, CStringArray& szaContentHeader, 
											 CStringArray& szaContent)
{
	BOOL bFoundBinId = FALSE;
	CString szBinId;
	BIN_FILE_INFO stInfo;
	
	stInfo.ucGrade = '0';
	stInfo.ulDieCount = 0;
	
	for (INT i=0; i<szaContentHeader.GetSize(); i++)
	{
		if ((szaContentHeader.GetAt(i) == "BIN") || 
			(szaContentHeader.GetAt(i) == "Bin"))		//v4.44A4 //Genesis 3F PkgSort
		{
			szBinId = szaContent.GetAt(i);
			bFoundBinId = TRUE;
			stInfo.usOriginalGrade = atoi(szBinId);
			stInfo.ucGrade = (UCHAR)atoi(szBinId);
			break;
		}
	}

	if (bFoundBinId == FALSE)
		return FALSE;

	for (INT i=0; i<szaContentHeader.GetSize(); i++)
	{		
		if (szaContentHeader.GetAt(i) == BIN_CODE)
		{
			(BinSummaryFile)[BIN_INFO][atoi(szBinId)][BIN_CODE] = szaContent.GetAt(i);
			//SetBinCode(atoi(szBinId), szaContent.GetAt(i));
		}
		else if ((szaContentHeader.GetAt(i) == TOTAL_COUNT)		||
				 (szaContentHeader.GetAt(i) == TOTAL_COUNT2))
		{
			//(BinSummaryFile)[BIN_INFO][atoi(szBinId)][TOTAL_COUNT] = atoi(szaContent.GetAt(i));
			//SetTotalBinCount(atoi(szBinId) , atoi(szaContent.GetAt(i)));
			stInfo.ulDieCount = atoi(szaContent.GetAt(i));
		}
		else if(szaContentHeader.GetAt(i) == PKG_NAME) //4.53D26
		{
			m_szSummaryFilePkgName = szaContent.GetAt(i);
		}
	}

	if ((m_ulMinLotCount != 0) && (stInfo.ulDieCount < m_ulMinLotCount))
	{
		return TRUE;
	}

	//Skip grades beyond 150
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Lextar")	//v4.50A9
	{
	}
	else
	{
		if ((stInfo.ucGrade > BT_MAX_BINBLK_NO) || (stInfo.ucGrade == 0))	//v4.44A4	//Genesis 3F pkgSort
			return TRUE;
	}

	m_aGradeInfo.Add(stInfo);

	CString szLog;
	szLog.Format("OtimizeBin: Bin %d, Count = %lu", stInfo.ucGrade, stInfo.ulDieCount);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	return TRUE;
}

BOOL CBinSummaryFile::GetDataFromHeaderString(CString szData, CString& szContent)
{
	INT nCol = -1;

	nCol = szData.Find(",");

	if (nCol != -1)
	{
		szContent = szData.Right(szData.GetLength() - nCol -1);
	}

	return TRUE;
}

BOOL CBinSummaryFile::GetDataFromContentString(CString szData, CStringArray& szaContent)
{
	INT nCol = -1;
	CString szContent;

	while ((nCol = szData.Find(",")) != -1)
	{
		szContent = szData.Left(nCol);
		
		if (szData.GetLength() > nCol +1)
		{
			szData = szData.Mid(nCol + 1);
		}
		
		szaContent.Add(szContent);
	}

	szaContent.Add(szData);

	return TRUE;
}

BOOL CBinSummaryFile::CompareString(CString szStr1, CString szStr2 )
{
	szStr1 = szStr1.MakeUpper();
	szStr2 = szStr2.MakeUpper();

	if (szStr1 == szStr2)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBinSummaryFile::GetGadeInfo(CArray<BIN_FILE_INFO, BIN_FILE_INFO>& aGradeInfo)
{
	aGradeInfo.Copy(m_aGradeInfo);
	return TRUE;
}

BOOL CBinSummaryFile::CreateWaferLotCheckDatabase()
{
	CStringMapFile	WaferLotInfo; 
	LONG lTotalNoOfWafer = 0;

	DeleteFile(MSD_WAFER_LOT_INFO_FILE);
	
	if (WaferLotInfo.Open(MSD_WAFER_LOT_INFO_FILE, FALSE, TRUE) != 1)
	{
		return FALSE;
	}

	// Update total no of wafers
	lTotalNoOfWafer = (LONG) m_szWaferIdsInLot.GetSize();
	(WaferLotInfo)[LOT_HEADER_A_WAFER_NO] = lTotalNoOfWafer;

	for (INT i =0; i<m_szWaferIdsInLot.GetSize(); i++)
	{
		(WaferLotInfo)[LOT_WAFER_NO][i+1] =  m_szWaferIdsInLot.GetAt(i);
		(WaferLotInfo)[LOT_WAFER_NO][i+1][LOT_WAFER_LOADED] = FALSE;
	}

	WaferLotInfo.Update();
	WaferLotInfo.Close();

	return TRUE;
}


VOID CBinSummaryFile::SetMinLotCount(ULONG ulMinLotCount)
{
	m_ulMinLotCount = ulMinLotCount;
}

VOID CBinSummaryFile::SetEnableBinSumWaferLot(BOOL bEnable)
{
	m_bEnableBinSumWaferLot = bEnable;
}

CString CBinSummaryFile::GetLastError()
{
	return m_szErrorCode;
}

CString CBinSummaryFile::GetPkgName()
{
	return m_szSummaryFilePkgName;
}
