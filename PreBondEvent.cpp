#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "HmiDataManager.h"
#include "BondHead.h"
#include "BinTable.h"
#include "WaferTable.h"
#include "prebondevent.h"
#include "FlushMessageThread.h"
#include "MESConnector.h"

ULONG CPreBondEvent::m_nMapOrginalCount = 0;
ULONG CPreBondEvent::m_ulMapOrgMinTotal = 0;
ULONG CPreBondEvent::m_ulMapOrgUnselTotal = 0;

CPreBondEvent::CPreBondEvent(void)
{
	m_pApp = NULL;
	m_pBondHead = NULL;
	m_pBinTable = NULL;
	m_pWaferTable = NULL;
	m_bDebug = FALSE;
}

CPreBondEvent::~CPreBondEvent(void)
{
}

void CPreBondEvent::SetApp(CMS896AApp* pApp)
{
	m_pApp = pApp;
}

void CPreBondEvent::SetBondHead(CBondHead* pBondHead)
{
	m_pBondHead = pBondHead;
}

void CPreBondEvent::SetBinTable(CBinTable* pBinTable)
{
	m_pBinTable = pBinTable;
}

void CPreBondEvent::SetWaferTable(CWaferTable* pWaferTable)
{
	m_pWaferTable = pWaferTable;
}

void CPreBondEvent::SetWaferMapWrapper(CWaferMapWrapper* pWaferMapWrapper)
{
	m_pWaferMapWrapper = pWaferMapWrapper;
}

BOOL CPreBondEvent::SelectGradeToPick()
{
	if (m_pBinTable != NULL)
	{
		if ( m_pBinTable->OptimizeBinGrade() == FALSE )
		{
			return FALSE;
		}

		ULONG i;
		INT j;
		unsigned long ulNoOfBinBlks = m_pBinTable->GetNoOfBinBlks();
		unsigned char aaGrades[256];
		BOOL bAdded[256];
		CUIntArray aulAvailableGradeList;
		CByteArray aaSelectedGrades;


		m_pWaferMapWrapper->GetAvailableGradeList(aulAvailableGradeList);

		UCHAR ucOffset = m_pWaferMapWrapper->GetGradeOffset();
		UCHAR ucNullBin = m_pWaferMapWrapper->GetNullBin();

		if( m_pWaferTable->m_bPrescanDefectAction &&
			m_pWaferTable->m_bPrescanDefectToNullBin==FALSE && 
			m_pWaferTable->m_bNgPickDefect )
		{
			UCHAR ucNgDefectGrade = m_pWaferTable->GetScanDefectGrade() + ucOffset;
			bool bFound = false;
			if( ucNgDefectGrade==ucNullBin )
				bFound = true;
			for (j = 0; j < aulAvailableGradeList.GetSize(); j++)
			{
				if( ucNgDefectGrade == aulAvailableGradeList[j] )
				{
					bFound = true;
					break;
				}
			}
			if( bFound==false )
			{
				aulAvailableGradeList.Add(ucNgDefectGrade);
			}
		}

		if( m_pWaferTable->m_bPrescanBadCutAction &&
			m_pWaferTable->m_bPrescanBadCutToNullBin==FALSE || 
			m_pWaferTable->m_bNgPickBadCut )
		{
			UCHAR ucNgBadcutGrade = m_pWaferTable->GetScanBadcutGrade() + ucOffset;
			bool bFound = false;
			if( ucNgBadcutGrade==ucNullBin )
				bFound = true;
			for (j = 0; j < aulAvailableGradeList.GetSize(); j++)
			{
				if( ucNgBadcutGrade == aulAvailableGradeList[j] )
				{
					bFound = true;
					break;
				}
			}
			if( bFound==false )
			{
				aulAvailableGradeList.Add(ucNgBadcutGrade);
			}
		}

		for (i=0; i<256; i++) 
		{
			bAdded[i] = FALSE;
		}

		for (i = 0; i < ulNoOfBinBlks; i++)
		{
			UCHAR ucGrade = m_pBinTable->GetBinBlkGrade(i+1);	
			unsigned long ulGrade = (unsigned long)ucGrade + m_pWaferMapWrapper->GetGradeOffset();
						
			/*if (ulGrade == (99+48))		//v3.31
			{
				//Grade 99 is IGNORE grade for OptoTech MS899 STD & DLA
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetCustomerName() == "OptoTech")
					continue;
			}*/

			if ( m_pBinTable->GetIfGradeIsAssigned(ucGrade) == TRUE )
			{
				for (j = 0; j < aulAvailableGradeList.GetSize(); j++)
				{
					ulGrade = min(ulGrade, 255);	//Klocwork	//v4.27
					if ((ulGrade == aulAvailableGradeList[j]) && (!bAdded[ulGrade]))
					{
						aaSelectedGrades.Add((unsigned char)ulGrade);
						bAdded[ulGrade] = TRUE;
						break;
					}
				}
			}
		}

		for (i=0; i < (ULONG)aaSelectedGrades.GetSize(); i++) 
		{
			aaGrades[i] = aaSelectedGrades.GetAt(i);
		}

		if (aaSelectedGrades.GetSize() > 0)
		{
			m_pWaferMapWrapper->SelectGrade(aaGrades, (unsigned long)aaSelectedGrades.GetSize());
		}

		m_pBinTable->DisplayOptimizeInfo();
	}

	return TRUE;
}


BOOL CPreBondEvent::SetIgnoreGrade()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->GetCustomerName() == "OptoTech")		//v3.28
	{
		//Set ignore grade for OptoTech.
		unsigned char arrIgnGrade[1];
		arrIgnGrade[0] = 48 + 99;					//Set 99 grade as IGNORE grade
		m_pWaferMapWrapper->SelectIgnoreGrade(arrIgnGrade, 1);
		return TRUE;
	}
	else if ( pApp->GetCustomerName() == CTM_SEMITEK )
	{
		if( m_pWaferTable->m_ucScanMapStartNgGrade>0 )
		{
			// Set grade as IGNORE grade
			unsigned char arrIgnGrade[1];
			arrIgnGrade[0] = m_pWaferMapWrapper->GetGradeOffset() + m_pWaferTable->m_ucScanMapStartNgGrade;
			m_pWaferMapWrapper->SelectIgnoreGrade(arrIgnGrade, 1);
		}
		return TRUE;
	}
	else if (pApp->GetCustomerName() == "TongFang")
	{
		unsigned char arrIgnGrade[1];
		arrIgnGrade[0] = m_pWaferMapWrapper->GetGradeOffset() + (UCHAR) CMS896AStn::m_ulIgnoreGrade;					//Set 150 grade as IGNORE grade
		//arrIgnGrade[0] = 48 + 150;					//Set 150 grade as IGNORE grade
		//AfxMessageBox("SetIgnore Grade", MB_SYSTEMMODAL);
		m_pWaferMapWrapper->SelectIgnoreGrade(arrIgnGrade, 1);
		return TRUE;
	}

	return TRUE;
}


void CPreBondEvent::CopyMapFile(BOOL bScnLoaded)
{
	if (m_pWaferTable == NULL)
	{
		return;
	}

	CString szMapFileName;
	CString szMapOtherName;
	CStdioFile cfWaferMapFile;
	int nCol = 0;

	CString	szMapFilePath = m_pWaferTable->GetMapFileName();

	//Init Map names
	szMapFileName	= szMapFilePath;
	szMapOtherName	= "";

	//Search any other filename
	nCol = szMapFilePath.ReverseFind(';');
	if ( nCol != -1 )
	{
		szMapFileName	= szMapFilePath.Left(nCol);
		szMapOtherName	= szMapFilePath.Mid(nCol+1);

		//Find the source map path
		nCol = szMapFilePath.ReverseFind('\\');
		if ( nCol != -1 )
		{
			szMapOtherName = szMapFilePath.Left(nCol) + "\\" + szMapOtherName;
		}
	}

	//Remove existing files
	DeleteFile((MS_LCL_CURRENT_MAP_FILE));
	DeleteFile((gszUSER_DIRECTORY + "\\MapFile\\CurrentMap.def"));
	DeleteFile((gszUSER_DIRECTORY + "\\MapFile\\BackupMap.txt"));
	DeleteFile((gszUSER_DIRECTORY + "\\MapFile\\BackupMap.PSM"));

	//v4.51A20	//Klocwork
	//Copy Map file
	if (m_pWaferTable->CopyFileWithRetry(szMapFileName, MS_LCL_CURRENT_MAP_FILE, FALSE) == FALSE)
	{
		m_pWaferTable->SetLoadCurrentMapStatus(FALSE);
	}

	SetFileAttributes(MS_LCL_CURRENT_MAP_FILE, FILE_ATTRIBUTE_NORMAL);

	// try to access it for checking whether it can copy to local harddsik or not
	if (cfWaferMapFile.Open(MS_LCL_CURRENT_MAP_FILE, CFile::modeRead|CFile::shareDenyNone) == FALSE)
	{
		m_pWaferTable->SetLoadCurrentMapStatus(FALSE);
	}
	else
	{
		m_pWaferTable->SetLoadCurrentMapStatus(TRUE);
	}

	cfWaferMapFile.Close();

	//Copy other file (ex OSRAM def file)
	if ( szMapOtherName.IsEmpty() == FALSE )
	{
		m_pWaferTable->CopyFileWithRetry(szMapOtherName, (gszUSER_DIRECTORY + "\\MapFile\\CurrentMap.def"), FALSE);
		SetFileAttributes((gszUSER_DIRECTORY + "\\MapFile\\CurrentMap.def"), FILE_ATTRIBUTE_NORMAL);
	}


	if ( bScnLoaded == TRUE )
	{
		m_pWaferTable->DumpScnData(TRUE);
	}
}

BOOL CPreBondEvent::Prepare(const unsigned long ulEventID)
{
	BOOL bStatus = TRUE;

	//Reset current wafer counter in NVRAM
	if (ulEventID == WAFER_MAP_ON_LOAD_MAP)
	{
		//	check manual load and auto delete psm/scn for SanAn
		if (m_pWaferTable != NULL)
		{
			m_pWaferTable->DeleteCurrentMapPsmScn();
		}	//	check manual load and auto delete psm/scn for SanAn

		if (m_pBondHead != NULL)
		{
			m_pBondHead->ResetCurrWaferCounter();
		}
		CMS896AApp::m_bMapLoadingFinish	= FALSE;
		CMS896AApp::m_lPreBondEventReply = 0;

		//Suspend any algorithm preparation 
		m_pWaferMapWrapper->SuspendAlgorithmPreparation();
	}

	//Clear boundary if pre algorithm is as below
	if (ulEventID == WAFER_MAP_ON_SELECT_ALGORITHM)
	{
		CString szPreAlgorithm;
		CString szPrePath;
		int nCol = 0;
		m_pWaferMapWrapper->GetAlgorithm(szPreAlgorithm, szPrePath);

		nCol = szPreAlgorithm.Find("(Reference Cross)");
		if ( nCol != -1 )
		{
			m_pWaferMapWrapper->DeleteAllBoundary();
		}
		else if (szPreAlgorithm.Find("(Cluster)") != -1)	//v4.13T5	//Support new by-region Cluster mode
		{
			m_pWaferMapWrapper->DeleteAllBoundary();
		}
	}

	if( ulEventID == WAFER_MAP_ON_MODIFY_DIE )
	{
		if( m_pWaferTable != NULL )
		{
			m_pWaferTable->ChangeRegionGrade(0);
		}
	}

	if( ulEventID == WAFER_MAP_ON_WRITE_MAP )
	{
		// check update wafer map header and then write map
		if (m_pBinTable != NULL)
		{
			m_pBinTable->UpdateWaferMapHeader();
		}
	}

	return bStatus;
}

BOOL CPreBondEvent::Execute(const unsigned long ulEventID, const BOOL bResult)
{
	int nYear, nMonth, nDay, nHour, nMinute, nSecond;
	int nCol = 0;

	CString szOperatorID = "";
	CString	szMapLotNo, szWaferId;
	CString szWaferName;
	CTime theTime;
	BOOL	bScnRtn = FALSE, bMapTypeCheck = FALSE;
	BOOL bUseFilenameAsWaferID = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


	if (ulEventID == WAFER_MAP_ON_LOAD_MAP)
	{
		if (m_pWaferTable == NULL)
		{
			return FALSE;
		}
		if (m_pApp == NULL)
		{
			return FALSE;
		}

 		if (!bResult)	//v4.50A16	//Cree HZ grade mapping fcn
		{
			pApp->SetErrorMessage("PreBondEvent: Load Map failure");
			CMS896AApp::m_bMapLoadingAbort = TRUE;
			return FALSE;
		}

		if (m_pBinTable == NULL)
		{
			pApp->SetErrorMessage("PreBondEvent: No BT");
			return FALSE;
		}
		if (m_pWaferMapWrapper == NULL)		//v4.51A20	//Klocwork
		{
			pApp->SetErrorMessage("PreBondEvent: No wafer map wrapper.");
			return FALSE;
		}

		CString szMapFullName = m_pWaferTable->GetMapFileName();
		m_pWaferTable->SetMapName();
		//m_pWaferTable->CheckWaferBonded();

		m_pWaferTable->m_usMapAngle	= m_pWaferMapWrapper->GetReader()->GetConfiguration().GetOrientedAngle();
		m_pWaferTable->m_bMapOHFlip	= m_pWaferMapWrapper->GetReader()->GetConfiguration().IsHorizontalFlip();
		m_pWaferTable->m_bMapOVFlip	= m_pWaferMapWrapper->GetReader()->GetConfiguration().IsVerticalFlip();
		
		//Skip other task if burn-in loadmap
		m_pWaferTable->SetAlignmentStatus(FALSE);	// RESET MAP ALIGNMENT AFTER LOAD A MAP
		m_pWaferTable->SetBlkFuncEnable(FALSE);
		m_pWaferTable->SetEnableSmartWalk(FALSE);
		m_pWaferTable->SetIgnoreGradeCountCheckStatus(FALSE);
		m_pWaferTable->SaveWaferMapAlgorithmSettings();	
		
		if ( m_pWaferTable->IsBurnIn() == TRUE )
		{
			SelectGradeToPick();
			m_pWaferMapWrapper->SelectAlgorithm("TLH", "Direct");
			m_pWaferMapWrapper->ResumeAlgorithmPreparation(TRUE);

			m_pWaferTable->SaveWaferMapAlgorithmSettings();	
			m_pWaferTable->MapLoaded();

			CMS896AApp::m_bMapLoadingFinish = TRUE;
			pApp->SetErrorMessage("PreBondEvent: burn in map loading");
			return TRUE;
		}


		//Check whether the selected field(s) mismatch with previous wafer (for Ubilux)		//v3.31T1
		/*if (! m_pBinTable->IfAllBinCountsAreCleared())
		{
			if (m_pWaferTable->AutoMapDieTypeCheck(TRUE) == FALSE)
			{
				pApp->SetErrorMessage("PreBondEvent: auto die type check fail.");
				return FALSE;
			}
		}
		else
		{
			m_pWaferTable->AutoMapDieTypeCheck(FALSE);
		}

		//v4.39T7	//Silan
		if (!m_pWaferTable->CheckMapNamePrefixSuffix(szMapFullName))
		{
			pApp->SetErrorMessage("PreBondEvent: check map name prefix fail.");
			return FALSE;
		}*/

		m_pWaferTable->PackageFileCheck();
		//m_pBinTable->ClearBatchIDInformation();

		//Set Wafer Start Date and Time
		theTime = CTime::GetCurrentTime();
		CString szDate, szTime, szHour, szMinute, szSecond;
		
		nYear = theTime.GetYear();
		nMonth = theTime.GetMonth();
		nDay = theTime.GetDay();
		/*if (pApp->GetCustomerName() == CTM_NICHIA)	//v4.40T5
		{
			if (nMonth < 10)
			{
				if (nDay < 10)
					szDate.Format("%d/0%d/0%d", nYear, nMonth, nDay);
				else
					szDate.Format("%d/0%d/%d", nYear, nMonth, nDay);
			}
			else
			{
				if (nDay < 10)
					szDate.Format("%d/%d/0%d", nYear, nMonth, nDay);
				else
					szDate.Format("%d/%d/%d", nYear, nMonth, nDay);
			}
		}
		else*/
		szDate.Format("%d-%d-%d", nYear, nMonth, nDay);

		nHour = theTime.GetHour();
		szHour.Format("%d", nHour);
		if (nHour < 10)
			szHour = "0" + szHour;
		nMinute = theTime.GetMinute();
		szMinute.Format("%d", nMinute);
		if (nMinute < 10)
			szMinute = "0" + szMinute;
		nSecond = theTime.GetSecond();
		szSecond.Format("%d", nSecond);
		if (nSecond < 10)
			szSecond = "0" + szSecond;

		szTime = szHour + ":" + szMinute + ":" + szSecond;

		m_pApp->m_bNewMapLoaded = TRUE;

		m_pWaferTable->SetLoadMapDateAndTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
		m_pWaferMapWrapper->GetWaferID(szWaferId);
		m_pWaferTable->SetWaferId(szWaferId);
		m_pApp->SaveLoadMapDate(szDate);
		m_pApp->SaveLoadMapTime(szTime);
		m_pApp->SetMachineTimeLoadNewMap(TRUE, szWaferId);

		//4.53D18 Set group size
		/*if (m_pWaferTable->m_bWaferMapUseDieMixing)
		{
			//LONG lSomeNum = 10;
			LONG lSomeNum = 99;
			//m_pWaferMapWrapper->SetAlgorithmParameter("Group Size", 12);
			//m_pWaferMapWrapper->SetAlgorithmParameter("Group Size", 15);
			//m_pWaferMapWrapper->SetAlgorithmParameter("Group Size", 16); //test
			m_pWaferMapWrapper->SetAlgorithmParameter("Group Size", 2); //test
			m_pWaferMapWrapper->SetAlgorithmParameter("Special Count", lSomeNum);
		}

		//Check walking algor
		CString szCurrentAlgorithm;
		CString szCurrentPath;
		/*
		m_pWaferMapWrapper->GetAlgorithm(szCurrentAlgorithm, szCurrentPath);
		if( (szCurrentAlgorithm == "Pick and Place: TLH-Circle") || 
			(szCurrentAlgorithm == "Pick and Place: BLH-Circle") || 
			(szCurrentAlgorithm == "Pick and Place: TLV-Circle") || 
			(szCurrentAlgorithm == "Pick and Place: TRV-Circle") )
		{
			szCurrentAlgorithm = "TLH";
			szCurrentPath = "LocalMinDistance";
			m_pWaferMapWrapper->SelectAlgorithm(szCurrentAlgorithm, szCurrentPath);
		}

		if ( (szCurrentAlgorithm == "Block Algorithm" && szCurrentPath == "Block Picking")
			|| (szCurrentAlgorithm == "Block Algorithm (without Ref Die)" && szCurrentPath == "Block Picking") )
		{
			m_pWaferTable->m_WaferMapWrapper.DeleteAllBoundary();
			m_pWaferTable->SetBlkFuncEnable(TRUE);
		}*/

		//Check if smart-Walk algorithm (xxxSW) is used
		/*if ((szCurrentAlgorithm.GetLength() >= 5) && 
			(szCurrentAlgorithm.GetAt(3) == 'S') && 
			(szCurrentAlgorithm.GetAt(4) == 'W'))
		{
			m_pWaferTable->SetEnableSmartWalk(TRUE);
			//SmartWalk can only support MIXED mode
			if (m_pWaferMapWrapper->GetPickMode() != WAF_CDieSelectionAlgorithm::MIXED_ORDER)		//If not MIX mode
			{
				m_pWaferMapWrapper->SetPickMode(WAF_CDieSelectionAlgorithm::MIXED_ORDER);
				m_pWaferTable->SaveWaferMapAlgorithmSettings();
			}
		}
		else
		{*/
		//}

		/*if (szCurrentAlgorithm == "Map Sequence")
		{
			m_pWaferTable->SetupMapSequenceParameters();
		}*/	

		if ( m_pApp->m_bIsChangeWafer == FALSE )			
		{
			m_pApp->SetProgressBarLimit(100);
			m_pApp->UpdateProgressBar(20);
		}

		//m_pBinTable->SetNotGenerateWaferEndFile(TRUE);
		//m_pBinTable->ResetWaferEndFileData();

		//Auto select grade with bin blocks taught
		if (m_pWaferTable->IsDisableWaferMapGradeSelect() == FALSE)
		{
			SelectGradeToPick();
			m_pWaferTable->SaveGradeMapTable();	
			//SetIgnoreGrade();		//Select IGNORE grade options
		}

		//Detect if wafer map format is changed
		if (m_pWaferMapWrapper->GetReader() != NULL) 
		{
			CString szCurrentFormat = m_pWaferMapWrapper->GetReader()->GetFormatName();

			//Get Wafermap LotID
			szMapLotNo = m_pWaferMapWrapper->GetReader()->GetHeader().GetLotID();
			CString szOriginalFormat = m_pWaferTable->GetWaferMapFormat();

			if (szCurrentFormat != szOriginalFormat)
			{
				m_pWaferTable->SetWaferMapFormat(szCurrentFormat);

				if (szOriginalFormat != "PickAndPlace")
				{
					if (!m_pBinTable->IfAllBinCountsAreCleared())
						m_pBinTable->SetIsAllBinCntCleared(FALSE);
					else
						m_pBinTable->SetIsAllBinCntCleared(TRUE);
				}
			}

			//Preserve a copy of wafermap grades in current memory	
			//SanAn mixedbin sorting
			//Wafermap library v1.67.00
			m_pWaferMapWrapper->CopyOriginalDieInformation();
		}	

		//Rotate & Flip map action & Init ScnData
		m_pWaferTable->RotateMap();
		m_pWaferTable->FlipMap();
		m_pWaferTable->SetCtmRefDieOptions();

		if ( m_pApp->m_bIsChangeWafer == FALSE )			
		{
			m_pApp->UpdateProgressBar(50);
		}

		// Load ScnMap 
		CString strTemp = szMapFullName;
		nCol = strTemp.ReverseFind(';');
		if (nCol != -1)
		{
			strTemp = strTemp.Left(nCol);
		}

		//CString strTmp2 = m_pWaferTable->GetMapNameOnly().MakeUpper();
		//m_cWSDecode.SetKeyword(strTmp2);

		m_pWaferTable->SetPsmEnable(FALSE);
		// must get before updated by scn info
		CalculateMapOriginalCount();

		m_pWaferTable->SetPsmMapMachineNo(FALSE, "");
		m_pWaferTable->SetScanDieNum(0, 0, 0, 0, 0);
		BOOL bPsmMap = GetNoRptPsmInfo(strTemp);
		m_pWaferTable->SetPsmEnable(bPsmMap);
		m_pWaferTable->InitScnData();	// load a new map
		//bScnRtn = GetWS896Info(strTemp);

		m_pWaferTable->ScnLoaded(FALSE);		//bScnRtn);
		m_pWaferTable->UpdateDieStateSummary();	// load a new map file

		CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
		if (m_pWaferMapWrapper->GetReader() != NULL)
		{
			if( pUtl->GetPrescanDummyMap() && (pUtl->GetPrescanBarWafer()==FALSE) )
			{
				m_pWaferMapWrapper->GetReader()->MapData("Save Dimension", "FALSE");
			}
			else
			{
				m_pWaferMapWrapper->GetReader()->MapData("Save Dimension", "TRUE");
			}
		}

		if ( m_pApp->m_bIsChangeWafer == FALSE )			
		{
			m_pApp->UpdateProgressBar(70);
		}

		m_pApp->UpdateMapHeaderInfo();		// Update Map header info (Bin Spec Version & BinParamter)
		//m_pApp->CloseLoadingAlert();		// Close the alert box
		m_pApp->UpdateMapLotNo(szMapLotNo);	// Save Map LotNo

		// 4.53D16 Loaing Map Comparing "Prject" Header with Package file name
		/*if (pApp->GetCustomerName() == "SeoulSemi") 
		{
			if( m_pWaferTable->ComparingProjectHeaderWithPackgeName() == FALSE)
			{
				pApp->SetErrorMessage("PreBondEvent: seul semi check PKG name fail.");
				return FALSE;
			}
		}

		if (!m_pWaferTable->CheckMapSpecNamePrefix(szMapFullName))  //4.52D15 after Update MapHeaderInfo
		{
			pApp->SetErrorMessage("PreBondEvent: check map psec name prefix fail.");
			return FALSE;
		}

		//v4.30T3	//CyOptics	/**This fcn must be called after UpdateMapHeaderInfo()** 
		m_pBinTable->CheckNewHoopsToLeaveEmptyRow();
		if (!m_pBinTable->CheckNichiaArrCodeInMap())
		{
			pApp->SetErrorMessage("PreBondEvent: check nichia code fail.");
			return FALSE;
		}
		*/

		if ( m_pApp->m_bIsChangeWafer == FALSE )			
		{
			m_pApp->UpdateProgressBar(100);
		}

		//INT nCode, nCol;
		//CString szPKGFile, szMsg, szMapDate;
		
		m_pWaferTable->MapLoaded();

		if (m_pWaferMapWrapper->GetReader() == NULL)
		{
			pApp->SetErrorMessage("PreBondEvent: map reader null.");
			return FALSE;
		}

		/*if (!m_pBinTable->LoadRankIDFromMap())
		{
			pApp->SetErrorMessage("PreBondEvent: load Rand ID fail");
			return FALSE;
		}
		if (!m_pBinTable->CheckValidRankIDFromCurrentMap())		//v4.21T4	//TongFang RankID checking
		{
			pApp->SetErrorMessage("PreBondEvent: check valid rank id from map fail");
			return FALSE;
		}

		m_pWaferTable->DynCreateHeaderAndOutputFileFormat();

		// Map Die Type Check First
		m_pWaferTable->MapDieTypeCheck();

		// MES Checking for customer: Testar
		m_pWaferTable->ClearMESWaferIDCheck();
		

		//CTM:Testar date time in map file date: 2008/12/18 AM 11:32:05,,108NJ44-1218,
		m_pWaferMapWrapper->GetHeaderInfo()->Lookup(WT_DATE_IN_MAP_FILE, szMapDate);
		nCol = szMapDate.Find(',');
		if (nCol != -1)
		{	
			szMapDate = szMapDate.Mid(nCol+1);
			nCol = szMapDate.Find(',');
			if (nCol != -1)
				szMapDate = szMapDate.Mid(nCol+1);
			szMapDate.Remove(',');
		}

		szPKGFile = m_pApp->GetPKGFilename();
		szOperatorID = m_pApp->GetUsername();
		
		//v4.42T6	//Testar
		if (CMESConnector::Instance()->IsMESConnectorEnable())
		{
			CMESConnector::Instance()->SendMessage(MES_CHECK_WAFER_ID_ID, m_pWaferTable->GetMapNameOnly() + "@" + szPKGFile + "@"
				+ szMapDate + "\n" + MES_OPERATOR_ID_INFO + "@" + szOperatorID);
			nCode = CMESConnector::Instance()->RecvMessage(szMsg);	
			if ( (nCode == TRUE) && (szMsg.GetLength() > 0) )
			{
				m_pApp->UpdateMapLotNo(szMsg);	//v4.42T6	//New Testar request
			}
		}
		*/

		//m_pWaferTable->MESWaferIDCheck(nCode, szMsg);
		bUseFilenameAsWaferID = m_pBinTable->GetUseFilenameAsWaferId();
		m_pWaferTable->LotInformation(bUseFilenameAsWaferID, szOperatorID);

		//Copy Select map file into "c:\mapsorter"
		// must complete copy file before set map loading finish
		CopyMapFile(bScnRtn);

		//Start any algorithm preparation 
		CString szCurrentAlgorithm, szCurrentPath;
		m_pWaferMapWrapper->GetAlgorithm(szCurrentAlgorithm, szCurrentPath);
		if ( szCurrentAlgorithm.Find("Sorting Path 1") == -1 || m_pWaferTable->IsPrescanEnable() == FALSE )
		{
			m_pWaferMapWrapper->ResumeAlgorithmPreparation(TRUE);
		}

		m_pWaferTable->SetDieSharp();
		m_pWaferTable->MapEdgeSize();

		if (m_pApp->m_bIsChangeWafer == FALSE)			
		{
			m_pApp->SetProgressBarLimit(0);
			m_pApp->UpdateProgressBar(0);
		}

		if (pUtl->GetPrescanDummyMap() == FALSE)
		{
			m_pWaferTable->SetAlignmentStatus(FALSE);	//2018.05.16
			m_pWaferTable->ClearPrescanRawData(TRUE);	// LOAD MAP FILE
		}

		CMSLogFileUtility::Instance()->BL_RemoveBinFrameStatusSummaryFile();
		CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Loaded Map:" + szWaferId);

		CMS896AApp::m_bMapLoadingFinish = TRUE;
		m_pWaferTable->m_bAutoLoadWaferMap = FALSE;	// done after map loaded
		SaveScanTimeEvent("    WFT: To load map file done");

		/*if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//2018.9.6 After load map and create wafer end file
			if (m_pBinTable != NULL)
			{
				if ((CMS896AStn::m_bAutoGenWaferEndFile == TRUE) || CMS896AStn::m_bIfGenWaferEndFile)
				{
					//Get Wafer End Date and Time
					m_pBinTable->GenerateWaferEndTime();
					m_pBinTable->WaferEndFileGenerating("Gen Waferend in Prebond Event");

					m_pBinTable->SetNotGenerateWaferEndFile(TRUE); //need create again
					m_pBinTable->ResetWaferEndFileData();
					//m_pBinTable->WaferEndFileGeneratingChecking();
				}
			}
		}*/

		return TRUE;

	} //end Event ID = WAFER_MAP_ON_LOAD_MAP

	if (ulEventID == WAFER_MAP_ON_SELECT_GRADE)
	{
		//Check if selected grade has bin block assigned
		CUIntArray aulSelectedGradeList, aulUnassignedGradeList, aulFilteredGradeList;
		INT i;
		UCHAR ucSelectedGrade;
		unsigned char aaGrades[256];

		if (m_pWaferTable != NULL)
		{
			if( m_pWaferTable->m_lPresortWftStatus>=8 ) //4.53D46
			{
				m_pWaferTable->m_lPresortWftStatus -= 8;
			}

			if (m_pWaferTable ->IsDisableWaferMapGradeSelect() == TRUE)
			{
				m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);

				for (i = 0; i < aulSelectedGradeList.GetSize(); i++)
				{		
					aulUnassignedGradeList.Add(aulSelectedGradeList.GetAt(i) - m_pWaferMapWrapper->GetGradeOffset());
				}

				if (aulUnassignedGradeList.GetSize() > 0)
				{
					for (i = 0; i < aulFilteredGradeList.GetSize(); i++)
					{
						aaGrades[i] = (unsigned char)aulFilteredGradeList.GetAt(i);
					}

					m_pWaferMapWrapper->SelectGrade(aaGrades, (unsigned long)aulFilteredGradeList.GetSize());
				}

				return TRUE;
			}
		}

		m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);

		for (i = 0; i < aulSelectedGradeList.GetSize(); i++)
		{
			ucSelectedGrade = (UCHAR)(aulSelectedGradeList.GetAt(i) - m_pWaferMapWrapper->GetGradeOffset());

			if (m_pBinTable != NULL)
			{
				if (!m_pBinTable->GetIfGradeIsAssigned(ucSelectedGrade))
				{
					aulUnassignedGradeList.Add(aulSelectedGradeList.GetAt(i) - m_pWaferMapWrapper->GetGradeOffset());
				}
				else
				{
					aulFilteredGradeList.Add(aulSelectedGradeList.GetAt(i));
				}
			}
		}

		if (aulUnassignedGradeList.GetSize() > 0)
		{
			for (i = 0; i < aulFilteredGradeList.GetSize(); i++)
			{
				aaGrades[i] = (unsigned char)aulFilteredGradeList.GetAt(i);
			}

			m_pWaferMapWrapper->SelectGrade(aaGrades, (unsigned long)aulFilteredGradeList.GetSize());
		}

		CString szMessage;
		//Save User selected grade list
		for (i = 0; i < aulFilteredGradeList.GetSize(); i++)
		{
			aaGrades[i] = (unsigned char)aulFilteredGradeList.GetAt(i);
			szMessage.AppendFormat("Select Grade = %d\n", aaGrades[i]);
		}

		m_pWaferTable->m_pWaferMapManager->SaveUserGradeList(aaGrades, (unsigned long)aulFilteredGradeList.GetSize());

//		m_pWaferTable->WriteUserParameterLog(szMessage);

	} //end Event ID = WAFER_MAP_ON_SELECT_GRADE


	if (ulEventID == WAFER_MAP_ON_SELECT_ALGORITHM)
	{
		CString szCurrentAlgorithm;
		CString szCurrentPath;

		m_pWaferMapWrapper->GetAlgorithm(szCurrentAlgorithm, szCurrentPath);

		m_pWaferTable->CheckRegionScanMode(szCurrentAlgorithm);

		m_pWaferTable->SetBlkFuncEnable(FALSE);
		
		/*if ( (szCurrentAlgorithm == "Block Algorithm" && szCurrentPath == "Block Picking")
			|| (szCurrentAlgorithm == "Block Algorithm (without Ref Die)" && szCurrentPath == "Block Picking") )
		{
			m_pWaferTable->SetBlkFuncEnable(TRUE);
		}*/

		//v2.99T1
		//Check if smart-Walk algorithm (xxxSW) is used
		if ((szCurrentAlgorithm.GetLength() >= 5) && 
			(szCurrentAlgorithm.GetAt(3) == 'S') && 
			(szCurrentAlgorithm.GetAt(4) == 'W'))
		{
			m_pWaferTable->SetEnableSmartWalk(TRUE);
			//SmartWalk can only support MIXED mode
			if (m_pWaferMapWrapper->GetPickMode() != WAF_CDieSelectionAlgorithm::MIXED_ORDER)		//If not MIX mode
			{
				m_pWaferMapWrapper->SetPickMode(WAF_CDieSelectionAlgorithm::MIXED_ORDER);
				m_pWaferTable->SaveWaferMapAlgorithmSettings();
			}
		}
		else
		{
			m_pWaferTable->SetEnableSmartWalk(FALSE);
		}

		//Update boundary id current algorithm is as below
		nCol = szCurrentAlgorithm.Find("(Reference Cross)");
		if ( nCol != -1 )
		{
			m_pWaferTable->SetupSubRegionMode();
		}
		else if (szCurrentAlgorithm.Find("(Cluster)") != -1)	//v4.13T5	//Support new by-region Cluster mode
		{
			m_pWaferMapWrapper->DeleteAllBoundary();
		}
	
		if (szCurrentAlgorithm == "Map Sequence")
		{
			m_pWaferTable->SetupMapSequenceParameters();
		}
		
		m_pWaferTable->SaveWaferMapAlgorithmSettings();
	}

	if ( ulEventID == WAFER_MAP_ON_SELECTED_POSITION )
	{
		m_pWaferTable->BlkGoToPosition();

		//Display Next Die path
		ULONG ulRow = 0, ulCol = 0;
		m_pWaferMapWrapper->GetSelectedPosition(ulRow, ulCol);
		m_pWaferTable->DisplayNextDirection(ulRow, ulCol);
		m_pWaferTable->DisplayDieState(ulRow, ulCol);
		m_pWaferTable->ClickMapAndTableGo();
	}


	if ( ulEventID == WAFER_MAP_ON_DELETE_DIE )
	{
		m_pWaferTable->SetAlignmentStatus(FALSE);	// DIE DELETED
	}
/*
	if (ulEventID == WAFER_MAP_ON_OPEN_BUTTON_DISABLED)
	{
		//AfxMessageBox("Please wafer end for this wafer first", MB_SYSTEMMODAL);
		if (m_pBinTable != NULL)
		{
			//Get Wafer End Date and Time
			m_pBinTable->GenerateWaferEndTime();
			m_pBinTable->WaferEndFileGeneratingChecking();
		}
	}
*/

	if( ulEventID == WAFER_MAP_ON_WRITE_MAP )
	{
		CString strFileName = m_strCurrentCommand;
		if( m_pWaferTable!=NULL )
		{
			if( m_pWaferTable->GetPsmEnable()==TRUE )
			{
				m_pWaferTable->UiSaveMapFile(strFileName);
			}
		}
	}

	if( ulEventID == WAFER_MAP_ON_INIT )
	{
		if( m_pWaferTable!=NULL )
		{
			m_pWaferTable->SetPsmEnable(FALSE);
		}
	}

	if( ulEventID == WAFER_MAP_ON_MODIFY_DIE )
	{
		if( m_pWaferTable != NULL )
		{
			m_pWaferTable->ChangeRegionGrade(1);
		}
	}

	return TRUE;
}

BOOL CPreBondEvent::CalculateMapOriginalCount()
{
	//---- start Get the total no of die in wafer before wafer scn ----//
	CUIntArray aulGradeList;
	ULONG ulTotal = 0;
	ULONG ulLeft = 0;
	ULONG ulPick = 0;

	CUIntArray aulSelectedGradeList;
	BOOL bInSelectedList = FALSE;
	ULONG ulMinCount = m_pWaferTable->m_ulMinGradeCount;

	//v4.38T7
	if (m_pWaferTable->m_ulMinGradeBinNo > 0 && m_pWaferTable->m_bSortMultiToOne==FALSE)
	{
		//Min BInNo > 0 -> those min bin  grades will be sorted to a mixed bin frame #BinNo instead
		ulMinCount = 0;
	}

	m_ulMapOrgMinTotal		= 0;
	m_ulMapOrgUnselTotal	= 0;

	m_nMapOrginalCount		=0;

	if (m_pWaferMapWrapper == NULL)
		return FALSE;

	m_pWaferMapWrapper->GetAvailableGradeList(aulGradeList);
	m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);

	for (int k=0; k<aulGradeList.GetSize(); k++)
	{
		m_pWaferMapWrapper->GetStatistics(aulGradeList.GetAt(k), ulLeft, ulPick, ulTotal);
		m_nMapOrginalCount = m_nMapOrginalCount + ulTotal;

		bInSelectedList = FALSE;
		for(int j=0; j<aulSelectedGradeList.GetSize(); j++)
		{
			if( aulGradeList.GetAt(k)==aulSelectedGradeList.GetAt(j) )
			{
				bInSelectedList = TRUE;
				break;
			}
		}

		if( bInSelectedList==FALSE )
		{
			m_ulMapOrgUnselTotal = m_ulMapOrgUnselTotal + ulTotal;
		}
		else
		{
			if (ulMinCount >= ulTotal)
			{
				m_ulMapOrgMinTotal = m_ulMapOrgMinTotal + ulTotal;
			}
		}
	}

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	if	(pUtl->LoadLastState("CPreBondEvent") == FALSE)
		return FALSE;

    psmf = pUtl->GetLastStateFile("CPreBondEvent");
	if (psmf != NULL)
	{
		(*psmf)[WAFER_DATA][DIE_COUNT_BF_SCN]			= m_nMapOrginalCount;
		(*psmf)[WAFER_DATA][DIE_MIN_COUNT_BF_SCN]		= m_ulMapOrgMinTotal;
		(*psmf)[WAFER_DATA][DIE_UNSELECTED_COUNT_BF_SCN]= m_ulMapOrgUnselTotal;
	}

	pUtl->UpdateLastState("CPreBondEvent");
	pUtl->CloseLastState("CPreBondEvent");
	return TRUE;
}

//================================================================
// GET function implementation section
//================================================================
BOOL CPreBondEvent::GetWS896Info(CString strFileName)
{
//	unsigned long	j;
	CStdioFile	InputFile;
	CStdioFile	LogFile;
	CString		strFile,strTemp;
	CString		strLine,strDecode;
//	PhyPos**	WS_Data;
	ULONG		ulMaxRow=0, ulMaxCol=0, ulRow = 0, ulCol = 0;
	LONG		lWsRow = 0, lWsCol = 0;
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

//	int		iPstnCol;
//	long	lCtrX,lCtrY;
	BOOL	bMapOrg = FALSE;

// prescan relative code
	if( m_pWaferTable->IsPrescanEnable() || m_pWaferTable->m_bEnableSCNFile==FALSE )
	{
		return FALSE;
	}
// prescan relative code

	return FALSE;	//v4.59A41

	/*
	//---- End of Get the total no of die in wafer before wafer scn ----//
	iPstnCol = strFileName.ReverseFind('.');
	if ( iPstnCol == -1 )
	{
		// No file extension
		strFile = strFileName + ".SCN";	
	}
	else
	{
		strTemp = strFileName.Left(iPstnCol);
		strFile = strTemp + ".SCN";	
	}

	if (InputFile.Open(strFile, CFile::modeRead) == NULL)
	{
		return FALSE;
	}

	if( LogFile.Open("ScnGet.txt", CFile::modeCreate|CFile::modeWrite)==NULL )
	{
		InputFile.Close();
		return FALSE;
	}

	LogFile.WriteString("Load SCN file " + strFile + "\n");
	m_pWaferMapWrapper->GetMapDimension(ulMaxRow, ulMaxCol);
	strLine.Format("map range %d, %d", ulMaxRow, ulMaxCol);
	LogFile.WriteString(strLine + "\n");
	WS_Data = new PhyPos*[ulMaxRow];
	for (i=0; i<ulMaxRow; i++) 
	{
		WS_Data[i] = new PhyPos[ulMaxCol];

		for (j=0; j<ulMaxCol; j++) 
		{
			WS_Data[i][j].t = 0;
		}
	}

	BOOL bFoundScn = FALSE;
    while (InputFile.ReadString(strLine)!= NULL)
	{
		short sResult = m_cWSDecode.DecWS896(strLine,strDecode,3);
		if (sResult == 0)
			continue;

		LogFile.WriteString(strDecode+"\n");

		if( strDecode == "[Psm Enable]" )
			m_pWaferTable->SetPsmEnable(TRUE);

		if( strDecode == "[User Map Enable]" )
			bMapOrg = TRUE;

		DOUBLE dScnTableTheta = 0.0;
		//	Get prescan table angle.
		if( strDecode.Find("[MS Aln Table Theta],")!=-1 )	//	get out from SCN file
		{
			strLine = strDecode;
			iPstnCol = strLine.Find(',');
			if ( iPstnCol != -1 )
			{
				strDecode = strLine.Right(strLine.GetLength() - iPstnCol - 1);
				strDecode.Remove('\n');
				dScnTableTheta = atof(strDecode);
			}
		}

		//Get RefDie position from SCN map
		if ( strDecode == "[RefDie]" )
		{
			InputFile.ReadString(strLine);

			sResult = m_cWSDecode.DecWS896(strLine,strDecode,3);
			if (sResult == 0)
				continue;
			strLine = strDecode;

			LogFile.WriteString(strDecode+"\n");

			// Ref Row & Col
			iPstnCol = strLine.Find(',');
			if( iPstnCol!=-1 )
				strTemp = strLine.Left(iPstnCol);
	
			lWsRow = atoi((LPCTSTR)strTemp);
			lWsCol = atoi((LPCTSTR)strLine.Mid(iPstnCol+1));
			if( bMapOrg )
			{
				m_pWaferTable->ConvertOrgUserToAsm(lWsRow, lWsCol, ulRow, ulCol);
			}
			else
			{
				ulRow = lWsRow;
				ulCol = lWsCol;
			}
			m_pWaferTable->SetScnRefDie(ulRow, ulCol, dScnTableTheta);
		}

		// Write die information header, die size and die pitch in X and Y.
		if ( strDecode == "[NormalDie]" )
		{
			// normal die size x and y
			InputFile.ReadString(strLine);
			sResult = m_cWSDecode.DecWS896(strLine,strDecode,3);
			if (sResult == 0)
				continue;
			strLine = strDecode;
			LogFile.WriteString(strDecode+"\n");

			iPstnCol = strLine.Find(',');
			if( iPstnCol!=-1 )
				strTemp = strLine.Left(iPstnCol);
			LONG lDieSizeX = atoi((LPCTSTR)strTemp);
			LONG lDieSizeY = atoi((LPCTSTR)strLine.Mid(iPstnCol+1));

			// normal die pitch x and y
			InputFile.ReadString(strLine);
			sResult = m_cWSDecode.DecWS896(strLine, strDecode, 3);
			if (sResult == 0)
				continue;
			strLine = strDecode;
			LogFile.WriteString(strDecode+"\n");

			// Pitch X_X
			iPstnCol = strLine.Find(',');
			if ( iPstnCol == -1 )
			{
				continue;
			}
			strTemp = strLine.Left(iPstnCol);
			LONG lPitchX_X = atoi((LPCTSTR)strTemp);
			strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);
			// Pitch X_Y
			iPstnCol = strLine.Find(',');
			if ( iPstnCol == -1 )
			{
				continue;
			}
			strTemp = strLine.Left(iPstnCol);
			LONG lPitchX_Y = atoi((LPCTSTR)strTemp);
			strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);
			// Pitch Y_Y
			iPstnCol = strLine.Find(',');
			if ( iPstnCol == -1 )
			{
				continue;
			}
			strTemp = strLine.Left(iPstnCol);
			LONG lPitchY_Y = atoi((LPCTSTR)strTemp);
			strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);
			// Pitch Y_X
			LONG lPitchY_X = atoi((LPCTSTR)strLine);
		}

		//Get SCN data
        if (strDecode == "[Die Detail]")
		{
			bFoundScn = TRUE;
			break;
		}
	}

	// Handle WS896 v1.40
    if ( bFoundScn == FALSE )
	{
		InputFile.SeekToBegin();
	}

	if ( (m_pApp != NULL) && (m_pApp->m_bIsChangeWafer == FALSE) )
	{
		m_pApp->UpdateProgressBar(50);
	}

	bFoundScn = FALSE;
	bool bDecodeOk = true;
	while (InputFile.ReadString(strLine)!= NULL)
	{
		short sResult = m_cWSDecode.DecWS896(strLine,strDecode,3);
		if (sResult == 0)
			continue;
		strLine = strDecode;

		LogFile.WriteString(strDecode+"\n");

		// Row
		iPstnCol = strLine.Find(',');
		if ( iPstnCol == -1 )
		{
			bDecodeOk = false;
			break;	
		}
		strTemp = strLine.Left(iPstnCol);
		lWsRow = atoi((LPCTSTR)strTemp);
		strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);
		
		// Col
		iPstnCol = strLine.Find(',');
		if ( iPstnCol == -1 )
		{
			bDecodeOk = false;
			break;	
		}
		strTemp = strLine.Left(iPstnCol);
		lWsCol = atoi((LPCTSTR)strTemp);
		strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);

		// Grade
		iPstnCol = strLine.Find(',');
		if ( iPstnCol == -1 )
		{
			bDecodeOk = false;
			break;	
		}
		strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);

		// X
		iPstnCol = strLine.Find(',');
		if ( iPstnCol == -1 )
		{
			bDecodeOk = false;
			break;	
		}
		strTemp = strLine.Left(iPstnCol);
		lCtrX = (long)atof((LPCTSTR)strTemp);
		strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);
		
		// Y
		iPstnCol = strLine.Find(',');
		if ( iPstnCol == -1 )
		{
			bDecodeOk = false;
			break;	
		}
		strTemp = strLine.Left(iPstnCol);
		lCtrY = (long)atof((LPCTSTR)strTemp);
		strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);

		if( bMapOrg )
		{
			m_pWaferTable->ConvertOrgUserToAsm(lWsRow, lWsCol, ulRow, ulCol);
		}
		else
		{
			ulRow = lWsRow;
			ulCol = lWsCol;
		}

		//Check Row & Col is valid inside the wafermap Max Row & Col
		if( (ulRow >= ulMaxRow) ||  (ulCol >= ulMaxCol) )
		{
			CString szText;
			szText.Format("Invalid Pos, Map Size = %d,%d, Get = %d,%d\n", ulMaxRow, ulMaxCol, ulRow, ulCol);
			LogFile.WriteString(szText);
			bDecodeOk = false;
			break;	
		}

		// Record in Wafer Map
		WS_Data[ulRow][ulCol].x = lCtrX;
		WS_Data[ulRow][ulCol].y = lCtrY;
		WS_Data[ulRow][ulCol].t = 1;
		bFoundScn = TRUE;
	}

	InputFile.Close();
	LogFile.Close();

	if( bDecodeOk==false )
	{
		for (i=0; i<ulMaxRow; i++)
			delete[] WS_Data[i];
		delete[] WS_Data;
		return FALSE;
	}

	if ( (m_pApp != NULL) && (m_pApp->m_bIsChangeWafer == FALSE) )
	{
		m_pApp->UpdateProgressBar(55);
	}

	if (LogFile.Open("ScnPhy.txt", CFile::modeCreate|CFile::modeWrite) == NULL)
	{
		for (i=0; i<ulMaxRow; i++)
			delete[] WS_Data[i];
		delete[] WS_Data;
		return FALSE;
	}

	// Set physical position into wafer map.
	UCHAR ucNullBin = m_pWaferMapWrapper->GetNullBin();
	UCHAR ucGrade;
	for (i=0; i<ulMaxRow; i++)
	{
		for (j=0; j<ulMaxCol; j++)
		{
			if (WS_Data[i][j].t == 1)
			{
				ucGrade = m_pWaferMapWrapper->GetGrade(i,j);
				m_pWaferTable->SetScnData(i,j,WS_Data[i][j].x,WS_Data[i][j].y,ucGrade);
				strTemp.Format("%3d,%3d,%3d,%6d,%6d\n", i, j, ucGrade, WS_Data[i][j].x, WS_Data[i][j].y);
				LogFile.WriteString(strTemp);
			}
			else if( m_pWaferTable->GetPsmEnable()==FALSE )
			{
				m_pWaferMapWrapper->ChangeGrade(i,j,ucNullBin);
			}
		}
	}
	LogFile.Close();

	if ( (m_pApp != NULL) && (m_pApp->m_bIsChangeWafer == FALSE) )
	{
		m_pApp->UpdateProgressBar(60);
	}

	for (i=0; i<ulMaxRow; i++)
		delete[] WS_Data[i];
	delete[] WS_Data;

	return bFoundScn;
	*/
}

// prescan relative code	B
BOOL CPreBondEvent::GetNoRptPsmInfo(CString strFileName)
{
	CStdioFile	InFile, LogFile, ErrFile;
	CString		strFile,strTemp;
	CString		strLine,strDecode;
	CString		szLog;
	CString		szLogFile, szErrFile;
	unsigned long	ulMaxRow=0, ulMaxCol=0;
	unsigned long	ulAsmRow=0, ulAsmCol=0;


	int		iPstnCol;
//	UCHAR	ucPsmGrade;
//	unsigned long ulState;
	long lUserRow=0, lUserCol=0;
//	short	sResult;
	bool	bFileOk = false;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( m_pWaferTable->GetPsmLoadAction()==1 )
	{
		return FALSE;
	}

	szLogFile = "PsmGet.txt";
	szErrFile = "PsmErr.txt";

	iPstnCol = strFileName.ReverseFind('.');
	if ( iPstnCol == -1 )
	{
		// No file extension
		strFile = strFileName + ".PSM";	
	}
	else
	{
		strTemp = strFileName.Left(iPstnCol);
		strFile = strTemp + ".PSM";	
	}

	if (m_pWaferTable->m_bAutoLoadWaferMap==FALSE && _access(strFile, 0) != -1)
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == CTM_SANAN)
		{
			if( m_pWaferTable->HmiMessage_Red_Back("PSM file exist, do you want to delete?", "Map Loading", glHMI_MBX_YESNO)==glHMI_YES )
			{
				DeleteFile(strFile);
				CString	szOperatorID = m_pApp->GetUsername();
				CString szLogMsg;
				szLogMsg = szOperatorID + " manual load map ask to delete psm " + strFile;
				CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg);
				return TRUE;
			}
		}
	}	//	check manual load and ask to delete psm for SanAn

#ifdef MS50_64BIT

	return FALSE;

#else

	if (InFile.Open(strFile, CFile::modeRead) == NULL)
	{
		return FALSE;
	}

	if( LogFile.Open(szLogFile, CFile::modeCreate | CFile::modeWrite)==NULL )
	{
		InFile.Close();
		return FALSE;
	}

	if( ErrFile.Open(szErrFile, CFile::modeCreate | CFile::modeWrite)==NULL )
	{
		InFile.Close();
		LogFile.Close();
		return FALSE;
	}

	ULONG ulGoodNum = 0, ulDefectNum = 0, ulBadCutNum = 0, ulExtraNum = 0, ulEmptyNum = 0;

	CString szTemp;
	szTemp = "[ScanMcNo],";
	m_pWaferMapWrapper->GetMapDimension(ulMaxRow, ulMaxCol);
	bFileOk = false;
	bool bMapOrg = false;
	bool bMarkState = false;
	BOOL bDieMark = false;
    while (InFile.ReadString(strLine)!= NULL)
	{
		if( strLine.GetLength()<szTemp.GetLength() )
			continue;
		sResult = m_cWSDecode.DecWS896(strLine,strDecode,3);
		if (sResult == 0)
			continue;

		// Get flag
		LogFile.WriteString(strDecode + "\n");
        if (strDecode == "[Grade Detail]")
		{
			bFileOk = true;
			break;
		}
		else if( strDecode.Find("[ScanPsmNew],MapOrgUser")!=-1 )
		{
			bMapOrg = true;
		}
		else if( strDecode.Find("[ScanPsmMark],MarkDieState")!=-1 )
		{
			bMarkState = true;
		}
		else if( strDecode.Find("[Scan Good],")!=-1 )	// scan good count
		{
			strLine = strDecode;
			iPstnCol = strLine.Find(',');
			if( iPstnCol!=-1 )
			{
				strDecode = strLine.Right(strLine.GetLength() - iPstnCol - 1);
				strDecode.Remove('\n');
				ulGoodNum = atoi((LPCTSTR)strDecode);
			}
		}
		else if( strDecode.Find("[Scan Defect],")!=-1 )	// scan defect count
		{
			strLine = strDecode;
			iPstnCol = strLine.Find(',');
			if( iPstnCol!=-1 )
			{
				strDecode = strLine.Right(strLine.GetLength() - iPstnCol - 1);
				strDecode.Remove('\n');
				ulDefectNum = atoi((LPCTSTR)strDecode);
			}
		}
		else if( strDecode.Find("[Scan BadCut],")!=-1 )	// scan bad cut count
		{
			strLine = strDecode;
			iPstnCol = strLine.Find(',');
			if( iPstnCol!=-1 )
			{
				strDecode = strLine.Right(strLine.GetLength() - iPstnCol - 1);
				strDecode.Remove('\n');
				ulBadCutNum = atoi((LPCTSTR)strDecode);
			}
		}
		else if( strDecode.Find("[Scan Extra],")!=-1 )	// scan extra count
		{
			strLine = strDecode;
			iPstnCol = strLine.Find(',');
			if( iPstnCol!=-1 )
			{
				strDecode = strLine.Right(strLine.GetLength() - iPstnCol - 1);
				strDecode.Remove('\n');
				ulExtraNum = atoi((LPCTSTR)strDecode);
			}
		}
		else if( strDecode.Find("[Scan Empty],")!=-1 )	// scan empty count
		{
			strLine = strDecode;
			iPstnCol = strLine.Find(',');
			if( iPstnCol!=-1 )
			{
				strDecode = strLine.Right(strLine.GetLength() - iPstnCol - 1);
				strDecode.Remove('\n');
				ulEmptyNum = atoi((LPCTSTR)strDecode);
			}
		}
		else if( strDecode.Find("[ScanMcNo],")!=-1 )
		{
			strLine = strDecode;
			// mc no
			iPstnCol = strLine.Find(',');
			if ( iPstnCol != -1 )
			{
				strDecode = strLine.Right(strLine.GetLength() - iPstnCol - 1);
				strDecode.Remove('\n');
				m_pWaferTable->SetPsmMapMachineNo(TRUE, strDecode);
			}
		}
	}

	m_pWaferTable->SetScanDieNum(ulGoodNum, ulDefectNum, ulBadCutNum, ulExtraNum, ulEmptyNum);

	szLog = "org row,col,grd,stt,mark\tdecode row,col,grd,stt,mark\tasm row,col,grade\n";
	LogFile.WriteString(szLog);
	ErrFile.WriteString(szLog);
	UCHAR ucNullGrade = m_pWaferMapWrapper->GetNullBin();
	UCHAR ucMapGrade;
	if( bFileOk )
	{
		// get data
		bFileOk = false;
		while (InFile.ReadString(strLine)!= NULL)
		{
			sResult = m_cWSDecode.DecWS896(strLine,strDecode,3);
			if (sResult == 0 )
				continue;
			strLine = strDecode;

			// Row
			iPstnCol = strLine.Find(',');
			if ( iPstnCol == -1 )
			{
				bFileOk = false;
				break;
			}
			strTemp = strLine.Left(iPstnCol);
			lUserRow = atoi((LPCTSTR)strTemp);
			strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);
			
			// Col
			iPstnCol = strLine.Find(',');
			if ( iPstnCol == -1 )
			{
				bFileOk = false;
				break;	
			}
			strTemp = strLine.Left(iPstnCol);
			lUserCol = atoi((LPCTSTR)strTemp);
			strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);
			
			// Grade
			iPstnCol = strLine.Find(',');
			if ( iPstnCol == -1 )
			{
				bFileOk = false;
				break;	
			}
			strTemp = strLine.Left(iPstnCol);
			ucPsmGrade = (UCHAR) atoi((LPCTSTR)strTemp);
			strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);

			// State
			iPstnCol = strLine.Find(',');
			if ( iPstnCol == -1 )
			{
				bFileOk = false;
				break;	
			}
			strTemp = strLine.Left(iPstnCol);
			ulState = atoi((LPCTSTR)strTemp);
			strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);

			// mark or not
			if( bMarkState )
			{
				iPstnCol = strLine.Find(',');
				if ( iPstnCol == -1 )
				{
					bFileOk = false;
					break;	
				}
				strTemp = strLine.Left(iPstnCol);
				bDieMark = (BOOL)(atoi((LPCTSTR)strTemp));
				strLine = strLine.Right(strLine.GetLength() - iPstnCol - 1);
			}

			if( bMapOrg )
				m_pWaferTable->ConvertOrgUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol);
			else
				m_pWaferMapWrapper->ConvertOriginalUserToInternal(lUserRow, lUserCol, 0, 1, 0, ulAsmRow, ulAsmCol);

			//Check Row & Col is valid inside the wafermap Max Row & Col
			if ( (ulAsmRow >= ulMaxRow) ||  (ulAsmCol >= ulMaxCol) )
			{
				bFileOk = false;
				break;
			}

			ucMapGrade = m_pWaferMapWrapper->GetGrade(ulAsmRow, ulAsmCol);
			m_pWaferMapWrapper->ChangeGrade(ulAsmRow, ulAsmCol, ucPsmGrade);
			if( bMarkState )
			{
				if( bDieMark )
					m_pWaferMapWrapper->MarkDie(ulAsmRow, ulAsmCol, TRUE);
			}
			else
			{
				if( m_pWaferTable->IsDieUnpickAll(ulState) )
					m_pWaferMapWrapper->MarkDie(ulAsmRow, ulAsmCol, TRUE);
			}
			m_pWaferMapWrapper->SetDieState(ulAsmRow, ulAsmCol, ulState);

			szLog.Format("%s\t%d,%d,%d,%d,%d\t%d,%d,%d\n", strDecode, lUserRow, lUserCol, ucPsmGrade, ulState, bDieMark, ulAsmRow, ulAsmCol, ucMapGrade);
			LogFile.WriteString(szLog);
			if( ucMapGrade!=ucPsmGrade && ucPsmGrade!=ucNullGrade )
			{
				ErrFile.WriteString(szLog);
			}
			bFileOk = true;
		}
	}

	if( bFileOk==false )
	{
		ErrFile.WriteString("read psm file and decoding wrong\n");
	}

	ErrFile.Close();
	LogFile.Close();
	InFile.Close();

	if( bFileOk==false )
		return FALSE;
	return TRUE;

#endif
}
// prescan relative code	E


//================================================================
// Class Functions
//================================================================
BOOL	CPreBondEventBin::m_bMapLoaded = FALSE;

CPreBondEventBin::CPreBondEventBin(void)
{
	m_pWaferTableBin = NULL;
}

CPreBondEventBin::~CPreBondEventBin(void)
{
}

void CPreBondEventBin::SetMapWrapperBin(CWaferMapWrapper* pMapWrapper)
{
	m_pMapWrapperBin = pMapWrapper;
}

void CPreBondEventBin::SetWaferTableBin(CWaferTable* pWaferTable)
{
	m_pWaferTableBin = pWaferTable;
}

BOOL CPreBondEventBin::Prepare(const unsigned long ulEventID)
{
	//Reset current wafer counter in NVRAM
	if (ulEventID == WAFER_MAP_ON_LOAD_MAP)
	{	
	}

	//Clear boundary if pre algorithm is as below
	if (ulEventID == WAFER_MAP_ON_SELECT_ALGORITHM)
	{
	}

	return TRUE;
}

BOOL CPreBondEventBin::Execute(const unsigned long ulEventID, const BOOL bResult)
{
	if (ulEventID == WAFER_MAP_ON_LOAD_MAP)
	{
		if( bResult == TRUE && m_pMapWrapperBin!=NULL )
		{
			m_pMapWrapperBin->Redraw();
			m_bMapLoaded	= TRUE;
		}
	}

	return TRUE;
}
