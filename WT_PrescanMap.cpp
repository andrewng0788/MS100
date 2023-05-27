//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "MS896A.h"
#include "PrescanInfo.h"
#include "WT_Log.h"
#include "LogFileUtil.h"
#include "PrescanUtility.h"
#include "WT_RegionPredication.h"
#include "Utility.H"
//#include "Encryption.h"

//#include "spswitch.h"		//v4.47T5
//using namespace AsmSw;
static long gslThisMachineHasRptFile = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

INT CWaferTable::OpSavePrescanMap()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	BOOL bGenPSM = GetPsmEnable();
	BOOL bGenSCN = GetScnEnable();
	BOOL bGenMap = TRUE;
	CTime theTime = CTime::GetCurrentTime();

	CString szMapExt = m_szMapFileExt1;
	if( m_bScanMapUseBackupExt )
	{
		if( m_szBackupMapExtension.IsEmpty() == FALSE)
		{
			szMapExt = "." + m_szBackupMapExtension;
		}
		else
		{
			szMapExt = "";
		}
	}

	CString szNewMapPath	= m_szPrescanLogPath + szMapExt;
	CString szNewPsmPath	= m_szPrescanLogPath + _T(".PSM");
	CString szNewScnPath	= m_szPrescanLogPath + _T(".SCN");

	CString szCreateMapPath = m_szPrescanLogPath + ".ma3";

	SaveScanTimeEvent("    WFT: to copy/save map file");
	if( bGenPSM )
	{
		if( pUtl->GetPrescanDummyMap() )
			m_WaferMapWrapper.WriteMap(m_szOnlyMapFileFullPath);	// not generate psm file after prescan
		if( m_szOnlyMapFileFullPath.CompareNoCase(szNewMapPath)!=0 )
		{
			CopyFileWithRetry(m_szOnlyMapFileFullPath, szNewMapPath, FALSE);
		}
		SaveScanTimeEvent("    WFT: to save psm file");
	}
	else
	{
		m_WaferMapWrapper.WriteMap(szNewMapPath);	// not generate psm file after prescan
		SaveScanTimeEvent("    WFT: write map file " + szNewMapPath);
	}

	if( bGenSCN )
	{
		SaveScanTimeEvent("    WFT: to save scn file");
	}

	if( IsAOIOnlyMachine() )
	{
		if( pApp->GetCustomerName()=="HuaLei" )
		{
			OpUpdateHuaLeiMap(szNewMapPath);
		}

		if( pApp->GetCustomerName()=="Huga" )
		{
			OpUpdateHugaMap(szNewMapPath);
		}

		if( pApp->GetCustomerName()=="HPO" )
		{
			OpUpdateHPOMap(szNewMapPath);
		}

		if( pApp->GetCustomerName()=="Focus" )
		{
			OpUpdateFocusMap(szNewMapPath);
		}

		if( m_ucScanMapTreatment==1 )	//	to remove map file header & mapdata, keep only raw data
		{
			OpRemoveMapHeader(szNewMapPath);
		}

		if( pApp->GetCustomerName()=="FATC" || m_szScanAoiMapFormat==_T("EpiTech") )
		{
			OpCreateScanOutputMap(szCreateMapPath);
		}
	}

if( m_bToUploadScanMapFile )
{
	SaveScanTimeEvent("    WFT: to copy files to map backup path target folder");
	TRY
	{
		CString szRptBackupPath = m_szBackupMapPath;

		if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
		{
			CString szArrTemp[4];
			if( IsWT2InUse() )
			{
				szArrTemp[0] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_1Layer"];  // Top == First
				szArrTemp[1] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_2Layer"];
				szArrTemp[2] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_3Layer"];
				szArrTemp[3] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_LowestLayer"];
			}
			else
			{
				szArrTemp[0] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_1Layer"];  // Top == First
				szArrTemp[1] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_2Layer"];
				szArrTemp[2] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_3Layer"];
				szArrTemp[3] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_LowestLayer"];
			}

			for ( int i = 0 ; i < 4 ; i++ )
			{
				if( szArrTemp[i] != "" )
				{
					szRptBackupPath = szRptBackupPath + "\\" + szArrTemp[i];
					if ( _access(szRptBackupPath, 0) == -1 )
					{
						CreateDirectory(szRptBackupPath, NULL);
					}
				}
			}
		}

		if( IsAOIOnlyMachine() && pApp->GetCustomerName()=="SanAn" )
		{
			CString szSubFolder;
			m_WaferMapWrapper.GetHeaderInfo()->Lookup("Sorter", szSubFolder);
			if( szSubFolder.IsEmpty()==FALSE )
			{
				szRptBackupPath = m_szBackupMapPath + _T("\\") + szSubFolder;
				CreateDirectory(szRptBackupPath, NULL);
			}
		}

		if( bGenMap )
		{
			CString szGenMapBkupPath	= szRptBackupPath + _T("\\") + GetMapNameOnly();
			if( m_szBackupMapExtension.IsEmpty() == FALSE)
			{
				szGenMapBkupPath = szGenMapBkupPath + "." + m_szBackupMapExtension;
			}
			if( szNewMapPath.CompareNoCase(szGenMapBkupPath)!=0 )
			{
				CopyFileWithQueue(szNewMapPath, szGenMapBkupPath, FALSE);
			}
		}

		CString szMsg;
		szMsg.Format("WaferLabel-- Prescan - Open another Bk map path:%d", pApp->m_bWaferLabelFile);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);	
		//4.52D10Name
		if (pApp->GetCustomerName() == "Lumileds" && pApp->GetProductLine() == "Rebel" && pApp->m_bWaferLabelFile)
		{
			pApp->BackUpMapFileToOutputPath(szNewMapPath);
		}

		if( bGenPSM )
		{
			CString szGenPsmBkupPath	= szRptBackupPath + _T("\\") + GetMapNameOnly() + _T(".PSM");
			if( szNewPsmPath.CompareNoCase(szGenPsmBkupPath)!=0 )
			{
				CopyFileWithQueue(szNewPsmPath, szGenPsmBkupPath, FALSE);
			}
		}

		if( IsOcrAOIMode() && IsCharDieInUse() )
		{
			CString szNewOcrPath	= m_szPrescanLogPath + _T(".OCR");	// copy generated ocr file to server.
			CString szTgtOcrPath	= szRptBackupPath + _T("\\") + GetMapNameOnly() + _T(".OCR");
			if (_access(szNewOcrPath, 0) != -1)
			{
				if( szNewOcrPath.CompareNoCase(szTgtOcrPath)!=0 )
				{
					CopyFileWithQueue(szNewOcrPath, szTgtOcrPath, FALSE);
				}
			}
		}

		if( bGenSCN )
		{
			CString szGenScnBkupPath	= szRptBackupPath + _T("\\") + GetMapNameOnly() + _T(".SCN");
			if( szNewScnPath.CompareNoCase(szGenScnBkupPath)!=0 )
			{
				CopyFileWithQueue(szNewScnPath, szGenScnBkupPath, FALSE);
			}
		}

		if( IsAOIOnlyMachine() )
		{
			if( pApp->GetCustomerName()=="FATC" || m_szScanAoiMapFormat==_T("EpiTech") )
			{
				CString szCreateBackupPath = szRptBackupPath + _T("\\") + GetMapNameOnly() + ".ma3";
				if( szCreateMapPath.CompareNoCase(szCreateBackupPath)!=0 )
				{
					CopyFileWithQueue(szCreateMapPath, szCreateBackupPath, FALSE);
				}
			}
		}	// AOI updated/created map to target folder by copy
	}
	CATCH (CFileException, e)
	{
	} END_CATCH
}

	if( m_bPrescanBackupMap )
	{
		CString szSpcSubPath;
		szSpcSubPath = theTime.Format("LOG_%y%m%d");
		if( pApp->GetCustomerName()=="Walsin" )
		{
			if (m_WaferMapWrapper.GetReader() != NULL) 
			{
				m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_LOT_NO, szSpcSubPath);
			}	
		}
		if( pApp->GetCustomerName()=="FATC" )
		{
			szSpcSubPath = GetMapNameOnly() + theTime.Format("_%y%m%d%H%M%S");
		}

		SaveScanTimeEvent("    WFT: to prescan back up sub path " + szSpcSubPath);
		CreateDirectory(m_szPrescanBackupMapPath, NULL);
		CString szSpcBkupPath = m_szPrescanBackupMapPath + _T("\\") + szSpcSubPath;
		CreateDirectory(szSpcBkupPath, NULL);

		CString szSpcMapBkupPath = szSpcBkupPath + _T("\\") + GetMapNameOnly() + szMapExt;
		CString szSpcPsmBkupPath = szSpcBkupPath + _T("\\") + GetMapNameOnly() + _T(".PSM");
		CString szSpcScnBkupPath = szSpcBkupPath + _T("\\") + GetMapNameOnly() + _T(".SCN");

		CFile	objDevFile;
		BOOL bScanned = FALSE;
		if( objDevFile.Open(szSpcMapBkupPath, CFile::modeRead) )
		{
			bScanned = TRUE;
			CString szTimeStamp = theTime.Format("_%H%M%S");
			szSpcMapBkupPath = szSpcBkupPath + _T("\\") + GetMapNameOnly() + szTimeStamp + szMapExt;
			szSpcPsmBkupPath = szSpcBkupPath + _T("\\") + GetMapNameOnly() + szTimeStamp + _T(".PSM");
			szSpcScnBkupPath = szSpcBkupPath + _T("\\") + GetMapNameOnly() + szTimeStamp + _T(".SCN");
			objDevFile.Close();
		}

		TRY
		{
			if( szNewMapPath.CompareNoCase(szSpcMapBkupPath)!=0 )
			{
				CopyFileWithQueue(szNewMapPath, szSpcMapBkupPath, FALSE);
			}

			if( bScanned )
			{
				if( bGenPSM )
				{
					SaveScanTimeEvent("    WFT: to save new psm file " + szSpcPsmBkupPath);
				}
				if( bGenSCN )
				{
					SaveScanTimeEvent("    WFT: to save new scn file " + szSpcScnBkupPath);
				}
			}
			else
			{
				if( bGenPSM )
				{
					if( szNewPsmPath.CompareNoCase(szSpcPsmBkupPath)!=0 )
					{
						CopyFileWithQueue(szNewPsmPath, szSpcPsmBkupPath, FALSE);
					}
				}
				if( bGenSCN )
				{
					if( szNewScnPath.CompareNoCase(szSpcScnBkupPath)!=0 )
					{
						CopyFileWithQueue(szNewScnPath, szSpcScnBkupPath, FALSE);
					}
				}
			}
		}
		CATCH (CFileException, e)
		{
		} END_CATCH
	}

	//	offline program		4.       Copy prescan result wafer map (scn, psm, csv? to the folder as well
	if( IsAOIOnlyMachine() && m_bPrescanLogNgImage )
	{
		CString szScanNgImagePath = m_szScanNgImageLogPath;
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
		{
			CString szArrTemp[4];
			if( IsWT2InUse() )
			{
				szArrTemp[0] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_1Layer"];  // Top == First
				szArrTemp[1] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_2Layer"];
				szArrTemp[2] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_3Layer"];
				szArrTemp[3] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_LowestLayer"];
			}
			else
			{
				szArrTemp[0] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_1Layer"];  // Top == First
				szArrTemp[1] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_2Layer"];
				szArrTemp[2] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_3Layer"];
				szArrTemp[3] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_LowestLayer"];
			}
			szScanNgImagePath = szScanNgImagePath + "\\" + szArrTemp[0] + "\\" + szArrTemp[1] + "\\" + szArrTemp[2] + "\\" + szArrTemp[3];
		}

		if (IsPathExist(szScanNgImagePath))
		{
			CString szSpcBkupPath = szScanNgImagePath + "\\" + GetMapNameOnly();
			if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
			{
				szSpcBkupPath = szScanNgImagePath;
			}
			CreateDirectory(szSpcBkupPath, NULL);

			CString szSpcMapBkupPath = szSpcBkupPath + _T("\\") + GetMapNameOnly() + szMapExt;
			CString szSpcPsmBkupPath = szSpcBkupPath + _T("\\") + GetMapNameOnly() + _T(".PSM");

			TRY
			{
				if( IsAOIOnlyMachine() && ( pApp->GetCustomerName()=="FATC" || m_szScanAoiMapFormat==_T("EpiTech") ) )
				{
					CString szCreateBackupPath = szSpcBkupPath + _T("\\") + GetMapNameOnly() + ".ma3";
					if( szCreateMapPath.CompareNoCase(szCreateBackupPath)!=0 )
					{
						CopyFileWithQueue(szCreateMapPath, szCreateBackupPath, FALSE);
					}
				}	// AOI updated/created map to target folder by copy
				else
				{
					if( szNewMapPath.CompareNoCase(szSpcMapBkupPath)!=0 )
					{
						CopyFileWithQueue(szNewMapPath, szSpcMapBkupPath, FALSE);
					}

					if( bGenPSM )
					{
						if( szNewPsmPath.CompareNoCase(szSpcPsmBkupPath)!=0 )
						{
							CopyFileWithQueue(szNewPsmPath, szSpcPsmBkupPath, FALSE);
						}
					}
				}
			}
			CATCH (CFileException, e)
			{
			} END_CATCH
		}
		else
		{
			CString szMsg;
			szMsg.Format("Prescan Log Image path not set!\n%s", szScanNgImagePath);
			HmiMessage_Red_Back(szMsg, "Prescan");
		}
	}

	//	offline program		5.       Copy the whole folder to customer network path

	return TRUE;
}


INT CWaferTable::OpUpdateHugaMap(CString szLocalMapPath)
{
	CString szRead, szUpdate;
	CStdioFile pReadFile, pWriteFile;
	CString szTempPath;

	szTempPath = m_szPrescanLogPath + "123.abc";
	remove(szTempPath);

	if( pReadFile.Open(szLocalMapPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		return FALSE;
	}

	if( pWriteFile.Open(szTempPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		pReadFile.Close();
		return FALSE;
	}

	pReadFile.SeekToBegin();
	pWriteFile.SeekToBegin();

	while( pReadFile.ReadString(szRead)!=NULL )
	{
		szUpdate = szRead;
		if( szRead.Find("PROBE")!=-1 )
		{
			szUpdate.Replace("PROBE", "AOI");
		}
		szUpdate += "\n";
		pWriteFile.WriteString(szUpdate);
		if( szRead.Find("TEST,BIN,CONTA,CONTC,POLAR,VF1,")!=-1 )
		{
			break;
		}
	}

	ULONG ulRow, ulCol, i;
	int	 j , k, m;
	bool bFindOnce;
	LONG lFileRow = 0, lFileCol = 0;
	ULONG ulEmptySize, ulDefectSize, ulBadCutSize;
	ULONG ulSrhIndex = 0, ulUpdIndex = 0;
	LONG *laEmptyRow, *laEmptyCol, *laDefectRow, *laDefectCol, *laBadCutRow, *laBadCutCol;
	BOOL *baEmptyUpd, *baDefectUpd, *baBadCutUpd;
	CString szTrim, szGet;

	ulEmptySize		= GetEmptySize();
	ulDefectSize	= GetDefectSize();
	ulBadCutSize	= GetBadCutSize();

	laEmptyRow	= new LONG[ulEmptySize+10];
	laEmptyCol	= new LONG[ulEmptySize+10];
	baEmptyUpd	= new BOOL[ulEmptySize+10];

	laDefectRow	= new LONG[ulDefectSize+10];
	laDefectCol	= new LONG[ulDefectSize+10];
	baDefectUpd	= new BOOL[ulDefectSize+10];

	laBadCutRow	= new LONG[ulBadCutSize+10];
	laBadCutCol	= new LONG[ulBadCutSize+10];
	baBadCutUpd	= new BOOL[ulBadCutSize+10];

	CString szLog;
	for(i=0; i<ulEmptySize; i++)
	{
		GetEmptyInfo(i, ulRow, ulCol);
		ConvertAsmToOrgUser(ulRow, ulCol, lFileRow, lFileCol);
		laEmptyRow[i] = lFileRow;
		laEmptyCol[i] = lFileCol;
		baEmptyUpd[i] = false;
	}
	for(i=0; i<ulDefectSize; i++)
	{
		GetDefectInfo(i, ulRow, ulCol);
		ConvertAsmToOrgUser(ulRow, ulCol, lFileRow, lFileCol);
		laDefectRow[i] = lFileRow;
		laDefectCol[i] = lFileCol;
		baDefectUpd[i] = false;
	}
	for(i=0; i<ulBadCutSize; i++)
	{
		GetBadCutInfo(i, ulRow, ulCol);
		ConvertAsmToOrgUser(ulRow, ulCol, lFileRow, lFileCol);
		laBadCutRow[i] = lFileRow;
		laBadCutCol[i] = lFileCol;
		baBadCutUpd[i] = false;
	}

	while( pReadFile.ReadString(szRead)!=NULL )
	{
		szUpdate = szRead;
		bFindOnce = FALSE;
		k = 0;
		szGet = szRead;
		m = szGet.GetLength()-1;
		for(j=m; j>0; j--)
		{
			if( szGet.GetAt(j)==',' )
			{
				szTrim = szGet.Right(m-j);
				k++;
				if( k==1 )
				{
					lFileRow = atoi(szTrim);
				}
				if( k==2 )
				{
					lFileCol = atoi(szTrim);
					break;
				}
			}
		}
		if( k!=2 )
		{
			bFindOnce = TRUE;
		}

		for(i=0; i<ulEmptySize; i++)
		{
			if( bFindOnce==TRUE )
			{
				break;
			}
			if( baEmptyUpd[i]==TRUE )
			{
				continue;
			}
			if( lFileRow==laEmptyRow[i] && lFileCol==laEmptyCol[i] )
			{
				baEmptyUpd[i] = TRUE;
				k=0;
				for(j=0; j<szRead.GetLength(); j++)
				{
					if( szRead.GetAt(j)==',' )
					{
						k++;
						if( k==9 )
						{
							break;
						}
					}
				}
				if( k==9 )
				{
					szUpdate.Insert(j+1, "9.99");
					bFindOnce = TRUE;
					ulUpdIndex++;
					break;
				}
			}
			ulSrhIndex++;
		}

		for(i=0; i<ulDefectSize; i++)
		{
			if( bFindOnce==TRUE )
			{
				break;
			}
			if( baDefectUpd[i]==TRUE )
			{
				continue;
			}

			if( lFileRow==laDefectRow[i] && lFileCol==laDefectCol[i] )
			{
				baDefectUpd[i] = TRUE;
				k=0;
				for(j=0; j<szRead.GetLength(); j++)
				{
					if( szRead.GetAt(j)==',' )
					{
						k++;
						if( k==9 )
						{
							break;
						}
					}
				}
				if( k==9 )
				{
					szUpdate.Insert(j+1, "9.99");
					bFindOnce = TRUE;
					ulUpdIndex++;
					break;
				}
			}
			ulSrhIndex++;
		}

		for(i=0; i<ulBadCutSize; i++)
		{
			if( bFindOnce==TRUE )
			{
				break;
			}
			if( baBadCutUpd[i]==TRUE )
			{
				continue;
			}
			if( lFileRow==laBadCutRow[i] && lFileCol==laBadCutCol[i] )
			{
				baBadCutUpd[i] = TRUE;
				k=0;
				for(j=0; j<szRead.GetLength(); j++)
				{
					if( szRead.GetAt(j)==',' )
					{
						k++;
						if( k==9 )
						{
							break;
						}
					}
				}
				if( k==9 )
				{
					szUpdate.Insert(j+1, "9.99");
					bFindOnce = TRUE;
					ulUpdIndex++;
					break;
				}
			}
			ulSrhIndex++;
		}

		szUpdate += "\n";
		pWriteFile.WriteString(szUpdate);
	}

	delete [] laEmptyRow;
	delete [] laEmptyCol;
	delete [] baEmptyUpd;
	delete [] laDefectRow;
	delete [] laDefectCol;
	delete [] baDefectUpd;
	delete [] laBadCutRow;
	delete [] laBadCutCol;
	delete [] baBadCutUpd;

	pReadFile.Close();
	pWriteFile.Close();

	CopyFile(szTempPath, szLocalMapPath, FALSE);
	remove(szTempPath);

	return TRUE;
} // new for aoi update map

// request: prescan result to update map AOI column
//	ruler: OK = A; NG = R; Empty = O; //MISS = M
INT CWaferTable::OpUpdateHPOMap(CString szLocalMapPath)
{
	CString szRead, szSave, szAOIType;
	CStdioFile pReadFile, pSaveFile;
	CString szSavePath;
	szSavePath = m_szPrescanLogPath + "123.abc";
	remove(szSavePath);

	if( pReadFile.Open(szLocalMapPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		return FALSE;
	}

	if( pSaveFile.Open(szSavePath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		pReadFile.Close();
		return FALSE;
	}

	pReadFile.SeekToBegin();
	pSaveFile.SeekToBegin();
	//	FileName,,"HH220131#07#CP11#120227165226#"

	if( pReadFile.ReadString(szRead)!=NULL )
	{
		int nIndex = szRead.Find(",\"");
		if( nIndex!=-1 )
		{
			szSave = szRead.Left(nIndex+2) + GetMapNameOnly() + "\"" + "\n";
			pSaveFile.WriteString(szSave);
		}
	}
	while( pReadFile.ReadString(szRead)!=NULL )
	{
		szSave = szRead + "\n";
		pSaveFile.WriteString(szSave);
		if( szRead.Find("AOI,")!=-1 )
		{
			break;
		}
	}

	int nAoiPosn = 0, nAoiOrder = 0;
	int	 j, nLen, nFindIndex;

	nFindIndex = szRead.Find("AOI,");
	for(j=0; j<nFindIndex; j++)
	{
		if( szRead.GetAt(j)==',' )
			nAoiOrder++;
	}

	ULONG ulAsmRow = 0, ulAsmCol = 0;
	bool bFindType, bFindAOI;
	ULONG ulEmptySize, ulDefectSize, ulBadCutSize;
	LONG *laEmptyRow, *laEmptyCol, *laDefectRow, *laDefectCol, *laBadCutRow, *laBadCutCol;
	BOOL *baEmptyUpd, *baDefectUpd, *baBadCutUpd;
	CString szTrim, szGet;

	ulEmptySize		= GetEmptySize();
	ulDefectSize	= GetDefectSize();
	ulBadCutSize	= GetBadCutSize();

	laEmptyRow	= new LONG[ulEmptySize+10];
	laEmptyCol	= new LONG[ulEmptySize+10];
	baEmptyUpd	= new BOOL[ulEmptySize+10];

	laDefectRow	= new LONG[ulDefectSize+10];
	laDefectCol	= new LONG[ulDefectSize+10];
	baDefectUpd	= new BOOL[ulDefectSize+10];

	laBadCutRow	= new LONG[ulBadCutSize+10];
	laBadCutCol	= new LONG[ulBadCutSize+10];
	baBadCutUpd	= new BOOL[ulBadCutSize+10];

	ULONG i;
	for(i=0; i<ulEmptySize; i++)
	{
		GetEmptyInfo(i, ulAsmRow, ulAsmCol);
		laEmptyRow[i] = ulAsmRow;
		laEmptyCol[i] = ulAsmCol;
		baEmptyUpd[i] = false;
	}
	for(i=0; i<ulDefectSize; i++)
	{
		GetDefectInfo(i, ulAsmRow, ulAsmCol);
		laDefectRow[i] = ulAsmRow;
		laDefectCol[i] = ulAsmCol;
		baDefectUpd[i] = false;
	}
	for(i=0; i<ulBadCutSize; i++)
	{
		GetBadCutInfo(i, ulAsmRow, ulAsmCol);
		laBadCutRow[i] = ulAsmRow;
		laBadCutCol[i] = ulAsmCol;
		baBadCutUpd[i] = false;
	}

	LONG lUserRow, lUserCol;
	while( pReadFile.ReadString(szRead)!=NULL )
	{
		szSave = szRead + "\n";
		nLen = szRead.GetLength()-1;

		// to find map column and row, conver to asm row and col
		szGet = szRead;
		nFindIndex = szGet.Find(',');
		if( nFindIndex!=-1 )
			szTrim = szGet.Left(nFindIndex);
		lUserCol = atoi(szTrim);
		szGet = szGet.Mid(nFindIndex+1);
		nFindIndex = szGet.Find(',');
		if( nFindIndex!=-1 )
			szTrim = szGet.Left(nFindIndex);
		lUserRow = atoi(szTrim);
		ConvertOrgUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol);

		// to find AOI column start position
		int k = 0;
		bFindType = FALSE;
		bFindAOI = FALSE;
		szAOIType = "A";	//	OK = A
		for(j=0; j<=nLen; j++)
		{
			if( szRead.GetAt(j)==',' )
			{
				k++;
				if( k==nAoiOrder )
				{
					bFindAOI = TRUE;
					nAoiPosn = j+1;
					break;
				}
			}
		}
		if( bFindAOI==FALSE )
		{
			szAOIType = "M";	//	MISS = M
		}

		for(i=0; i<ulEmptySize; i++)
		{
			if( bFindAOI==FALSE || bFindType==TRUE )
			{
				break;
			}
			if( baEmptyUpd[i]==TRUE )
			{
				continue;
			}
			if( ulAsmRow==laEmptyRow[i] && ulAsmCol==laEmptyCol[i] )
			{
				baEmptyUpd[i] = TRUE;
				bFindType = TRUE;
				szAOIType = "O";	//	Empty = O
				break;
			}
		}

		for(i=0; i<ulDefectSize; i++)
		{
			if( bFindAOI==FALSE || bFindType==TRUE )
			{
				break;
			}
			if( baDefectUpd[i]==TRUE )
			{
				continue;
			}

			if( ulAsmRow==laDefectRow[i] && ulAsmCol==laDefectCol[i] )
			{
				baDefectUpd[i] = TRUE;
				szAOIType = "R";	//	NG = R
				bFindType = TRUE;
				break;
			}
		}

		for(i=0; i<ulBadCutSize; i++)
		{
			if( bFindAOI==FALSE || bFindType==TRUE )
			{
				break;
			}
			if( baBadCutUpd[i]==TRUE )
			{
				continue;
			}
			if( ulAsmRow==laBadCutRow[i] && ulAsmCol==laBadCutCol[i] )
			{
				baBadCutUpd[i] = TRUE;
				bFindType = TRUE;
				szAOIType = "R";	//	NG = R
				break;
			}
		}

		if( bFindAOI )
			szSave.Insert(nAoiPosn, szAOIType);
		pSaveFile.WriteString(szSave);
	}

	delete [] laEmptyRow;
	delete [] laEmptyCol;
	delete [] baEmptyUpd;
	delete [] laDefectRow;
	delete [] laDefectCol;
	delete [] baDefectUpd;
	delete [] laBadCutRow;
	delete [] laBadCutCol;
	delete [] baBadCutUpd;

	pReadFile.Close();
	pSaveFile.Close();

	CopyFile(szSavePath, szLocalMapPath, FALSE);
	remove(szSavePath);

	return TRUE;
} // HPO for aoi update map

INT CWaferTable::OpUpdateFocusMap(CString szLocalMapPath)
{
	return TRUE;
}

INT	CWaferTable::OpUpdateHuaLeiMap(CString szLocalMapPath)
{
	CString szRead, szSave;
	CStdioFile pReadFile, pSaveFile;
	CString szSavePath = m_szPrescanLogPath + "123.abc";
	remove(szSavePath);

	if( pReadFile.Open(szLocalMapPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		return FALSE;
	}

	if( pSaveFile.Open(szSavePath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		pReadFile.Close();
		return FALSE;
	}

	pReadFile.SeekToBegin();
	pSaveFile.SeekToBegin();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString	szOperatorID = pApp->GetUsername();
	CString szMachineNo = GetMachineNo();

	while( pReadFile.ReadString(szRead)!=NULL )
	{
		int nIndex = szRead.Find("Operator ID,");
		if( nIndex!=-1 )
		{
			szRead = szRead + "," + szOperatorID;
		}
		nIndex = szRead.Find("Tester M/C NO.,");
		if( nIndex!=-1 )
		{
			szRead = szRead + "," + szMachineNo;
		}
		szSave = szRead + "\n";
		pSaveFile.WriteString(szSave);
	}

	pReadFile.Close();
	pSaveFile.Close();

	CopyFile(szSavePath, szLocalMapPath, FALSE);
	remove(szSavePath);

	return TRUE;
}

//	ChangeLight, remove map header and only keep raw data.
INT CWaferTable::OpRemoveMapHeader(CString szLocalMapPath)
{
	CString szRead, szSave;
	CStdioFile pReadFile, pSaveFile;
	CString szMapPath = m_szPrescanLogPath + "123.abc";
	RenameFile(szLocalMapPath, szMapPath);

	if( pReadFile.Open(szMapPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		return FALSE;
	}

	if( pSaveFile.Open(szLocalMapPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		pReadFile.Close();
		return FALSE;
	}

	pReadFile.SeekToBegin();
	pSaveFile.SeekToBegin();

	while( pReadFile.ReadString(szRead)!=NULL )
	{
		int nIndex = szRead.Find("map data");
		if( nIndex!=-1 )
		{
			pReadFile.ReadString(szRead);
			break;
		}
	}
	while( pReadFile.ReadString(szRead)!=NULL )
	{
		szSave = szRead + "\n";
		pSaveFile.WriteString(szSave);
	}

	pReadFile.Close();
	pSaveFile.Close();

	remove(szMapPath);

	return TRUE;
} // remove map header and only keep raw data for aoi update map






BOOL CWaferTable::OpCreateScanOutputMap(CString szLocalMapPath)
{
	CTime stEndTime = CTime::GetCurrentTime();
	CStdioFile pSaveFile;

	remove(szLocalMapPath);

	if( pSaveFile.Open(szLocalMapPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		CString szMsg;
		szMsg = "Can not open file to create MA3.\n" + szLocalMapPath;
		HmiMessage_Red_Back(szMsg, "AOI Prescan");
		return FALSE;
	}

	CString szTemp, szData;

	BOOL bEpiTechFormat = (m_szScanAoiMapFormat==_T("EpiTech"));
	// customer name
	szData = m_szOemCustomerName + "\t\n\n\n\n\n\n";
	pSaveFile.WriteString(szData);

	// lot number
	szTemp = GetMapNameOnly();	//	(*m_psmfSRam)["MS896A"]["LotNumber"];
	szData.Format("LOT\t%s\n\n\n", szTemp);
	pSaveFile.WriteString(szData);

	// wafer name
	szData.Format("WAFER\t%s\n\n\n", GetMapNameOnly());
	pSaveFile.WriteString(szData);

	// product setting
	szData.Format("PRODUCT\t%s\n\n\n",	m_szOemProductName);
	pSaveFile.WriteString(szData);

	// machine no
	szTemp = GetMachineNo();
	szData.Format("DEVICE\t%s\n\n\n", szTemp);
	pSaveFile.WriteString(szData);

	// reader, wafer name
	if( bEpiTechFormat )
		szData.Format("READER\t%s\n\n\n", GetMapNameOnly());
	else
		szData.Format("READER\t%s\n\n\n\n\n\n", GetMapNameOnly());
	pSaveFile.WriteString(szData);

	// scan begin time (create map time).
	if( bEpiTechFormat )
		szData = m_stReadTmpTime.Format("%Y-%m-%d\n\n\n");
	else
		szData = m_stReadTmpTime.Format("STIME\t%Y/%m/%d\t%H:%M:%S\n\n\n");
	pSaveFile.WriteString(szData);
	if( bEpiTechFormat )
	{
		szData = m_stReadTmpTime.Format("%H:%M:%S\n\n\n");
		pSaveFile.WriteString(szData);
	}

	// scan map total die
	szData.Format("%lu\t\n\n\n", GetMapTotalDie());
	pSaveFile.WriteString(szData);

	// Cost Time
	CTimeSpan stTime = stEndTime - m_stReadTmpTime;
	szData.Format("%02d:%02d:%02d\t\n", stTime.GetHours(), stTime.GetMinutes(), stTime.GetSeconds());
	if( !bEpiTechFormat )
		szData = "CostTime = " + szData;
	pSaveFile.WriteString(szData);
	for(int i=1; i<=26; i++)
	{
		pSaveFile.WriteString("\n");
	}

	// End Date
	if( bEpiTechFormat )
		szData = stEndTime.Format("EDATE\t%Y-%m-%d\n\n\n");
	else
		szData = stEndTime.Format("EDATE\t\n\n\n");
	pSaveFile.WriteString(szData);

	// End Time
	if( bEpiTechFormat )
		szData = stEndTime.Format("ETIME\t%H:%M:%S\n\n\n");
	else
		szData = stEndTime.Format("ETIME\t%Y/%m/%d\t%H:%M:%S\n\n\n");
	pSaveFile.WriteString(szData);

	// Operator
	CString szOperatorID = (*m_psmfSRam)["MS896A"]["Operator Id"];
	if( bEpiTechFormat )
		szOperatorID = "";
	szData.Format("OPERATOR\t%s\n\n\n", szOperatorID);
	pSaveFile.WriteString(szData);

	// setup file
	szData = "SETUP FILE\t\n\n\n";
	pSaveFile.WriteString(szData);

	// test system
	szTemp = "";
	if( bEpiTechFormat )
		szTemp = m_szOemTestSystem;
	szData.Format("TEST SYSTEM\t%s\n\n\n", szTemp);
	pSaveFile.WriteString(szData);

	// Test Date
	szData = "TEST DATE\t\n\n\n";
	pSaveFile.WriteString(szData);

	// Probe card
	szTemp = "";
	if( bEpiTechFormat )
		szTemp = m_szOemProductName;
	szData.Format("PROBE CARD\t%s\n\n\n", szTemp);
	pSaveFile.WriteString(szData);

	// probe
	szData = "PROBER\t\n";
	pSaveFile.WriteString(szData);
	for(int i=1; i<=5; i++)
	{
		pSaveFile.WriteString("\n");
	}

	ULONG ulRow = 0, ulCol = 0, ulMaxRow = 0, ulMaxCol = 0;
	LONG lUserRow = 0, lUserCol = 0, lAlnUserRow = 0, lAlnUserCol = 0;
	UCHAR ucGrade = 0, ucNullGrade = 0, ucGradeOffset = 0;
	LONG lRow, lCol, lX = 0, lY = 0;
	BOOL bIsDefect = FALSE, bIsBadcut = FALSE;
	double dAngle = 0;
	CString szDieBin = "0";
	USHORT usDieType = 0;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	pUtl->GetAlignPosition(lRow, lCol, lX, lY);
	if( pUtl->GetPrescanAoiWafer() && m_bSetDummyMa3HomeDie )
	{
		CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
		CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
		slLock.Lock();

		LONG lAlarmStatus = GetAlarmLamp_Status();

		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);

		if( IsBLInUse() )
		{
			BOOL bWft = FALSE;
			if( IsWT1UnderCamera() && !IsWT2InUse() )
			{
				bWft = FALSE;
			}
			if( IsWT2UnderCamera() && IsWT2InUse() )
			{
				bWft = TRUE;
			}
			m_lBackLightZStatus = 0;
			MoveES101BackLightZUpDn(TRUE);
			MoveFocusToWafer(bWft);
		}
		XY_SafeMoveTo(lX, lY);
		m_WaferMapWrapper.SetSelectedPosition(lRow, lCol);
		m_WaferMapWrapper.SetCurrentPosition(lRow, lCol);
		if ( IsBurnIn() == FALSE )
		{
			m_bEnableClickMapAndTableGo	= TRUE;
			ChangeCameraToWaferBondMode(FALSE);
			while( 1 )
			{
				SelectWaferCamera();
				int nReply = AfxMessageBox("Please click the new align home die on map!", MB_SYSTEMMODAL|MB_OKCANCEL);
				if( nReply == IDOK )
				{
					break;
				}
				else
				{
					Sleep(500);
					SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);
					return FALSE;
				}	
			}
			m_bEnableClickMapAndTableGo = FALSE;
		}
		if( IsBLInUse() )
		{
			m_lBackLightZStatus = 2;
			MoveES101BackLightZUpDn(FALSE);
		}

		SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);
		ULONG ulAlnRow, ulAlnCol;
		m_WaferMapWrapper.GetSelectedPosition(ulAlnRow, ulAlnCol);
		m_WaferMapWrapper.SetCurrentPosition(ulAlnRow, ulAlnCol);

		slLock.Unlock();
		lRow = ulAlnRow;
		lCol = ulAlnCol;
	}
	ConvertAsmToOrgUser(lRow, lCol, lAlnUserRow, lAlnUserCol);

	ucNullGrade = m_WaferMapWrapper.GetNullBin();
	ucGradeOffset = m_WaferMapWrapper.GetGradeOffset();
	m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);

	int nRowDir = 1, nColDir = 1;

	for(ulCol=0; ulCol<ulMaxCol; ulCol++)
	{
		for(ulRow=0; ulRow<ulMaxRow; ulRow++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucGrade==ucNullGrade )
			{
				continue;
			}

			if( GetMapDieState(ulRow, ulCol)==WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY )
			{
				continue;
			}

			if( GetMapDieState(ulRow, ulCol)==WT_MAP_DIESTATE_SCAN_EDGE )
			{
				continue;
			}

			if( GetPrescanInfo(ulRow, ulCol, lX, lY, dAngle, szDieBin, bIsDefect, bIsBadcut) )
			{
				if( szDieBin=="0" )
					usDieType = 1;
				else
					usDieType = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				lUserRow = (lUserRow-lAlnUserRow) * nRowDir + m_lMa3AHomeRow;
				lUserCol = (lUserCol-lAlnUserCol) * nColDir + m_lMa3AHomeCol;
				if( m_lMa3RHomeRow!=0 && m_lMa3RHomeCol!=0 )
				{
					if( lUserRow==(m_lMa3RHomeRow) && lUserCol==m_lMa3RHomeCol )
					{
						continue;
					}
				}
				szData.Format("X%ldY%ld\t%s\t%d\n", lUserCol, lUserRow, szDieBin, usDieType);
				pSaveFile.WriteString(szData);
			}
		}
	}

	pSaveFile.Close();

	return TRUE;
}	//	Create Fatc AOI Map

BOOL CWaferTable::OpCheckMA1ToMA3()
{
	if( IsAOIOnlyMachine()==FALSE )
	{
		return TRUE;
	}

	if( m_dMA1ScanYieldLimit<=0.0 || m_szScanMA1FilePath.IsEmpty() )
	{
		return TRUE;
	}

	BOOL bMA1MA3 = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="FATC" || m_szScanAoiMapFormat==_T("EpiTech") )
	{
		bMA1MA3 = TRUE;
	}
	if( bMA1MA3==FALSE )
	{
		return TRUE;
	}

	CString szErrMsg;
	BOOL bReturn = TRUE;

	CString szMA1FilePath = m_szScanMA1FilePath + "\\" + GetMapNameOnly() + ".ma1";
	CStdioFile fCheckFile;
	if( fCheckFile.Open(szMA1FilePath, CFile::modeRead|CFile::shareDenyNone) )
	{
		int nRowDir = 1, nColDir = 1;

		fCheckFile.SeekToBegin();
		CString szReading;
		CStringArray szaDataList;
		ULONG ulTotalDie = 0, ulBadDie = 0;
		ULONG ulRow = 0, ulCol = 0;
		LONG lUserRow = 0, lUserCol = 0, lAlnUserRow = 0, lAlnUserCol = 0;
		LONG lRow, lCol, lX = 0, lY = 0;
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		pUtl->GetAlignPosition(lRow, lCol, lX, lY);
		ConvertAsmToOrgUser(lRow, lCol, lAlnUserRow, lAlnUserCol);
		UCHAR ucNullGrade = m_WaferMapWrapper.GetNullBin();

		// get the fail counter limit
		while( fCheckFile.ReadString(szReading) )
		{
			szaDataList.RemoveAll();
			CUtility::Instance()->ParseRawData(szReading, szaDataList);
			if( szaDataList.GetSize()>=2 )
			{
				lUserCol = atoi(szaDataList.GetAt(0));
				lUserRow = atoi(szaDataList.GetAt(1));
				ulTotalDie++;
				lRow = (lUserRow - m_lMa3AHomeRow) * nRowDir + lAlnUserRow;
				lCol = (lUserCol - m_lMa3AHomeCol) * nColDir + lAlnUserCol;
				ConvertOrgUserToAsm(lRow, lCol, ulRow, ulCol);
				UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
				if( ucGrade==ucNullGrade )
				{
					ulBadDie++;
					continue;
				}

				if( GetMapDieState(ulRow, ulCol)==WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY )
				{
					ulBadDie++;
					continue;
				}

				if( GetPrescanPosition(ulRow, ulCol, lX, lY)==FALSE )
				{
					ulBadDie++;
					continue;
				}
			}
		}

		ULONG ulGoodDie = ulTotalDie - ulBadDie;
		CString szMsg;
		szMsg.Format("MA1 Total die %d, bad %d, good %d, set limit %.2f", ulTotalDie, ulBadDie, ulGoodDie, m_dMA1ScanYieldLimit);
		SetErrorMessage(szMsg);
		if( ulTotalDie>0 )
		{
			if( ulGoodDie<(ulTotalDie*m_dMA1ScanYieldLimit/100.0) )
			{
				szErrMsg.Format("MA1 GOOD %ld TOTAL %ld lower than %2.2f!", ulGoodDie, ulTotalDie, m_dMA1ScanYieldLimit);
				bReturn = FALSE;
			}
		}
		else
		{
			szErrMsg = GetMapNameOnly() + ".MA1 no die found.";
			bReturn = FALSE;
		}
	}
	else
	{
		szErrMsg = GetMapNameOnly() + ".MA1 file not found.";
		bReturn = FALSE;
	}

	if( bReturn==FALSE )
	{
		CString szMsg = "Scan MA1 result compare is fail.\n" + szErrMsg + "\n" + "MA3 will not begenerated!";
		HmiMessage_Red_Back(szMsg, "Prescan");
		SaveScanTimeEvent("WFT: " + szMsg);
		SetErrorMessage(szMsg);
	}

	return bReturn;
}

BOOL CWaferTable::SwapContourPreloadMap()
{
	if( m_WaferMapWrapper.IsCachedMapCreated()==FALSE )
	{
		SaveScanTimeEvent("    WFT: no preloaded cached map");
		return FALSE;
	}
	SaveScanTimeEvent("    WFT: swap preload dummy map begin");
	//Clear Wafermap & reset grade map
	SetPsmEnable(FALSE);

	m_stReadTmpTime = CTime::GetCurrentTime();

	m_WaferMapWrapper.InitMap();
	m_WaferMapWrapper.SwapCachedMap();
	m_WaferMapWrapper.InitCachedMap();

	CMS896AApp::m_bMapLoadingFinish = TRUE;

	SaveScanTimeEvent("    WFT: swap preload dummy map complete");

	CString szCurrentMapPath = PRESCAN_RESULT_FULL_PATH + m_szDummyWaferMapName;
	if ( m_szMapFileExtension.IsEmpty() == FALSE)
	{
		szCurrentMapPath = szCurrentMapPath + "." + m_szMapFileExtension;
	}
	m_szCurrentMapPath = GetMapFileName();
	SaveScanTimeEvent("    WFT: swap map " + m_szCurrentMapPath);
	SaveScanTimeEvent("    WFT: code map " + szCurrentMapPath);
	if( m_szCurrentMapPath.CompareNoCase(szCurrentMapPath)!=0  )
	{
		return FALSE;
	}

	ClearPrescanRawData(TRUE);
	if( m_WaferMapWrapper.IsMapCreated() )		 
	{
		unsigned char aaTempGrades[1];
		aaTempGrades[0] = m_WaferMapWrapper.GetGradeOffset() + m_ucDummyPrescanPNPGrade;

		ULONG ulAlnRow, ulAlnCol;
		GetMapAlignHomeDie(ulAlnRow, ulAlnCol);
		m_WaferMapWrapper.SetSelectedPosition(ulAlnRow, ulAlnCol);
		m_WaferMapWrapper.SetCurrentPosition(ulAlnRow, ulAlnCol);
		m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aaTempGrades[0]);
		m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);

		SaveScanTimeEvent("    WFT: swap select grades complete");

		SwapCachedMapTidyUp();
		SaveScanTimeEvent("    WFT: swap map house keeping complete");
		return TRUE;
	}

	return FALSE;
}

BOOL CWaferTable::SwapCachedMapTidyUp()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CTime theTime;

	if (m_WaferMapWrapper.GetReader() == NULL)	//If MAP is cleared in then return FALSE!
	{
		return FALSE;
	}

	m_usMapAngle	= m_WaferMapWrapper.GetReader()->GetConfiguration().GetOrientedAngle();
	m_bMapOHFlip	= m_WaferMapWrapper.GetReader()->GetConfiguration().IsHorizontalFlip();
	m_bMapOVFlip	= m_WaferMapWrapper.GetReader()->GetConfiguration().IsVerticalFlip();
	//Skip other task if burn-in loadmap
	SetAlignmentStatus(FALSE);	// RESET MAP ALIGNMENT AFTER LOAD A MAP
	SetBlkFuncEnable(FALSE);

	//Set Wafer Start Date and Time
	theTime = CTime::GetCurrentTime();
	CString szDate, szTime;
	
	int nYear = theTime.GetYear();
	int nMonth = theTime.GetMonth();
	int nDay = theTime.GetDay();
	szDate.Format("%d-%d-%d", nYear, nMonth, nDay);

	int nHour = theTime.GetHour();
	int nMinute = theTime.GetMinute();
	int nSecond = theTime.GetSecond();
	szTime.Format("%02d:%02d:%02d", nHour, nMinute, nSecond);

	pApp->m_bNewMapLoaded = TRUE;

	CString szWaferId;
	SetLoadMapDateAndTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
	m_WaferMapWrapper.GetWaferID(szWaferId);
	m_szWaferId = szWaferId;

	pApp->SaveLoadMapDate(szDate);
	pApp->SaveLoadMapTime(szTime);
	pApp->SetMachineTimeLoadNewMap(TRUE, szWaferId);

	SetEnableSmartWalk(FALSE);

	SetIgnoreGradeCountCheckStatus(FALSE);

	if (m_WaferMapWrapper.GetReader() != NULL) 
	{
		m_WaferMapWrapper.CopyOriginalDieInformation();
	}

	SetMapName();

	SetPsmEnable(FALSE);
	SetPsmMapMachineNo(FALSE, "");
	SetScanDieNum(0, 0, 0, 0, 0);
	SetPsmEnable(FALSE);
	InitScnData();	// swap to a new map

	m_bAutoLoadWaferMap = FALSE;
	//Set bRtn into ......

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( m_WaferMapWrapper.GetReader() != NULL )
	{
		if( pUtl->GetPrescanDummyMap() || IsAOIOnlyMachine() )
		{
			m_WaferMapWrapper.GetReader()->MapData("Save Dimension", "FALSE");
		}
		else
		{
			m_WaferMapWrapper.GetReader()->MapData("Save Dimension", "TRUE");
		}
	}

	//Start any algorithm preparation 
	m_WaferMapWrapper.ResumeAlgorithmPreparation(TRUE);

	//Set map edge size
	MapEdgeSize();

	CMSLogFileUtility::Instance()->BL_RemoveBinFrameStatusSummaryFile();
	CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Loaded Map:" + szWaferId);

	m_bEnablePickAndPlace = FALSE;
	m_lBuildShortPathInScanning = 0;
	m_bFirstMapIsOldMap		= FALSE;
	// Clear the information for last wafer map
	m_dScnDieA_X			= 0;
	m_dScnDieA_Y			= 0;
	m_ulScnDieA_Row			= 0;
	m_ulScnDieA_Col			= 0;
	m_ulScnDieB_Row			= 0;
	m_ulScnDieB_Col			= 0;
	m_dWS896Angle_X			= 0.0;
	m_dWS896Angle_Y			= 0.0;
	m_lMsDieA_X				= 0;
	m_lMsDieA_Y				= 0;
	m_lMsDieB_X				= 0;
	m_lMsDieB_Y				= 0;
	m_dMS896Angle_X			= 0.0;
	m_dMS896Angle_Y			= 0.0;

	m_lLookForwardCounter	= 0;
	m_lNoLookForwardCounter = 0;
	m_bSearchWaferLimitDone	= FALSE;

	DeleteFile(MSD_WAFER_SORTED_BIN_BC_FILE);
	
	m_szCurrentMapPath = GetMapFileName();

	CString szFullPath = m_szCurrentMapPath.MakeUpper();
	CString szMapPath = m_szMapFilePath.MakeUpper();
	if( szFullPath.Find(szMapPath)!=-1 )
	{
		m_szMapServerFullPath = m_szCurrentMapPath;
	}

	WT_CSubRegionsInfo*  pSRInfo = WT_CSubRegionsInfo::Instance();
	pSRInfo->SetCurrentRegionState_HMI("");

	CMS896AApp::m_bMapLoadingFinish = TRUE;

	return TRUE;
}

VOID CWaferTable::UpdateSortSequenceAfterScan(VOID)
{
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	BOOL bFindNew = FALSE;
	CUIntArray aulNewSortGradeList;

	m_WaferMapWrapper.GetSelectedGradeList(aulNewSortGradeList);

	UCHAR ucNgDefectGrade = GetScanDefectGrade() + ucOffset;
	UCHAR ucNgBadcutGrade = GetScanBadcutGrade() + ucOffset;

	if( m_bPrescanDefectAction && ucNgDefectGrade!=ucNullBin)
	{
		bool bFound = false;
		ULONG ulLeft, ulPicked, ulTotal;
		m_WaferMapWrapper.GetStatistics(ucNgDefectGrade, ulLeft, ulPicked, ulTotal);
		if( ulTotal==0 )
			bFound = true;
		if( bFound==false )
		{
			for (INT j = 0; j < aulNewSortGradeList.GetSize(); j++)
			{
				if( ucNgDefectGrade == aulNewSortGradeList.GetAt(j) )
				{
					bFound = true;
					break;
				}
			}
		}
		if( bFound==false )
		{
			bFindNew = TRUE;
			aulNewSortGradeList.Add(ucNgDefectGrade);
			CString szMsg;
			szMsg.Format("WFT: scan defect grade %d need to be pick, push into list", ucNgDefectGrade - ucOffset);
			SaveScanTimeEvent(szMsg);
		}
	}
	else
	{
		ucNgDefectGrade = ucNullBin;
	}

	if( m_bPrescanBadCutAction && ucNgBadcutGrade!=ucNullBin )
	{
		bool bFound = false;
		ULONG ulLeft, ulPicked, ulTotal;
		m_WaferMapWrapper.GetStatistics(ucNgBadcutGrade, ulLeft, ulPicked, ulTotal);
		if( ulTotal==0 )
			bFound = true;
		if( bFound==false )
		{
			for (INT j = 0; j < aulNewSortGradeList.GetSize(); j++)
			{
				if( ucNgBadcutGrade == aulNewSortGradeList.GetAt(j) )
				{
					bFound = true;
					break;
				}
			}
		}
		if( bFound==false )
		{
			bFindNew = TRUE;
			aulNewSortGradeList.Add(ucNgBadcutGrade);
			CString szMsg;
			szMsg.Format("WFT: scan defect grade %d need to be pick, push into list", ucNgBadcutGrade - ucOffset);
			SaveScanTimeEvent(szMsg);
		}
	}
	else
	{
		ucNgBadcutGrade = ucNullBin;
	}

	if( bFindNew )
	{
		unsigned char aaGrades[256];
		for (INT i=0; i < aulNewSortGradeList.GetSize(); i++) 
		{
			aaGrades[i] = aulNewSortGradeList.GetAt(i);
		}

		if (aulNewSortGradeList.GetSize() > 0)
		{
			m_WaferMapWrapper.SelectGrade(aaGrades, (unsigned long)aulNewSortGradeList.GetSize());
		}
	}

	if ( (m_lSortingMode != WT_SORT_MIN_DIE) && (m_lSortingMode != WT_SORT_MAX_DIE) )
	{
		return ;
	}

	if( m_bSortMultiToOne )
	{
		return ;
	}

	if ( (m_ulMinGradeCount == 0) || (m_ulMinGradeBinNo == 0) )
	{
		return;
	}

	m_unArrMinGradeBin.RemoveAll();

	CUIntArray aulSequenceGradeList;
	ULONG	ulPick = 0;
	ULONG	*paulGradeTotal, *paulLeft;
	UCHAR	*paucGradeList;
	BOOL	bUpdate = FALSE;	

	//Get current selected grade no & total
	INT nListSize = (INT)aulNewSortGradeList.GetSize();

	paulLeft = new ULONG[nListSize];
	paulGradeTotal = new ULONG[nListSize];
	paucGradeList = new UCHAR[nListSize];

	INT		nStartIndex = 0;
	if( ucNgDefectGrade!=ucNullBin )
	{
		aulSequenceGradeList.Add(ucNgDefectGrade);
		m_WaferMapWrapper.GetStatistics(ucNgDefectGrade, paulLeft[nStartIndex], ulPick, paulGradeTotal[nStartIndex]);
		paucGradeList[nStartIndex] = ucNgDefectGrade;
		nStartIndex++;
	}
	if( ucNgBadcutGrade!=ucNullBin )
	{
		aulSequenceGradeList.Add(ucNgBadcutGrade);
		m_WaferMapWrapper.GetStatistics(ucNgBadcutGrade, paulLeft[nStartIndex], ulPick, paulGradeTotal[nStartIndex]);
		paucGradeList[nStartIndex] = ucNgBadcutGrade;
		nStartIndex++;
	}

	INT i = nStartIndex;
	for (int j=0; j<nListSize; j++)
	{
		UCHAR ucGrade = aulNewSortGradeList.GetAt(j);
		if( ucGrade!=ucNgDefectGrade && ucGrade!=ucNgBadcutGrade )
		{
			m_WaferMapWrapper.GetStatistics(ucGrade, paulLeft[i], ulPick, paulGradeTotal[i]);
			aulSequenceGradeList.Add(ucGrade);
			paucGradeList[i] = ucGrade;
			i++;
		}
	}

	//Sort grade in order if necessary
	m_pWaferMapManager->GetSelectedSortGradeList(m_lSortingMode, nStartIndex, nListSize, paulGradeTotal, paulLeft, paucGradeList);

	//Re-order grade from user input
	CString szLogMsg, szTemp;
	szLogMsg.Format("Min (%d) Grades list ", m_ulMinGradeCount);
	for (i=nStartIndex; i<nListSize; i++)
	{
		if ((paulGradeTotal[i] <= m_ulMinGradeCount))
		{
			m_unArrMinGradeBin.Add((UINT) paucGradeList[i]);
			szTemp.Format(",%d", paucGradeList[i]-ucOffset);
			szLogMsg += szTemp;
		}
	}
	SaveScanTimeEvent(szLogMsg);

	szLogMsg = "Pick list ";
	for (INT j=0; j<nListSize; j++)
	{
		UCHAR ucGrade = paucGradeList[j] - ucOffset;
		szTemp.Format(",%d(%d)", ucGrade, paulGradeTotal[j]);
		szLogMsg += szTemp;
	}
	SaveScanTimeEvent(szLogMsg);

	m_WaferMapWrapper.SelectGrade(paucGradeList, nListSize);

	delete[] paulLeft;
	delete[] paulGradeTotal;
	delete[] paucGradeList;
}
