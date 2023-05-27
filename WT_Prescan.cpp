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
//#include "Encryption.h"
#include "WL_Constant.h"
#include "Utility.h"
#include "WaferPr.h"

//#ifndef MS_DEBUG	//v4.47T7
//	#include "spswitch.h"
//	using namespace AsmSw;
//#endif

static long gslThisMachineHasRptFile = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	MSD_SCAN_YIELD_INFO_FILE		gszROOT_DIRECTORY + "\\Exe\\ScanYield.msd"
#define	MSD_SCAN_YIELD_HMI_FILE			gszROOT_DIRECTORY + "\\Exe\\ScanYield.csv"
#define	MSD_SCAN_WAFER_ID_FILE			gszROOT_DIRECTORY + "\\Exe\\ScanWafer.csv"
#define	MSD_SCAN_YIELD_HEAD				"ScanYield"
#define	MSD_SCAN_YIELD_TOTAL_WAFERS		"TotalWafers"
#define	MSD_SCAN_YIELD_WAFER_INDEX		"ScanWaferIndex"
#define	MSD_SCAN_YIELD_NAME				"MapName"
#define	MSD_SCAN_YIELD_TIME				"ScanTime"
#define	MSD_SCAN_YIELD_WFTNO			"WftNo"
#define	MSD_SCAN_YIELD_MGZNNO			"MgznNo"
#define	MSD_SCAN_YIELD_SLOTNO			"SlotNo"
#define	MSD_SCAN_YIELD_VALUE			"ScanYield"

#define	SCAN_GO_CHECK_NEAR_LIMIT		5
#define	WT_PITCH_VERIFY_SPAN_LIMIT		"Pitch Verify Span Limit"

#define	MSD_SCN_WAFER_LIST_FILE			gszROOT_DIRECTORY + "\\Exe\\WaferList.msd"
#define	MSD_SCN_WAFER_LIST_BU_FILE		gszROOT_DIRECTORY + "\\Exe\\WaferlistBackUp.msd"
#define	MSD_SCN_WAFER_HEAD				"SCN Wafer Header"
#define	MSD_SCN_WAFER_BONDED			"SCN Wafer Bonded"
#define	MSD_SCN_WAFER_TOTAL				"Scn Total Wafers"

BOOL CWaferTable::CheckWaferBondedSize()
{
	if( m_bScnPrescanIfBonded && IsPrescanEnable()==FALSE )
	{
		CStringMapFile smfWaferList, smfBackUp;
		//Check file exist 
		if ( smfWaferList.Open(MSD_SCN_WAFER_LIST_FILE, FALSE, TRUE) )
		{
			LONG lTotalWafers	= (smfWaferList)[MSD_SCN_WAFER_HEAD][MSD_SCN_WAFER_TOTAL];
			if( lTotalWafers>1000 )
			{
				if( smfBackUp.Open(MSD_SCN_WAFER_LIST_BU_FILE, FALSE, TRUE) )
				{
					for(INT i=1; i<=500; i++)
					{
						LONG lIndex = lTotalWafers-500 + i;
						CString szMapName = (smfWaferList)[MSD_SCN_WAFER_HEAD][lIndex];
						(smfBackUp)[MSD_SCN_WAFER_HEAD][szMapName] = 1;
						(smfBackUp)[MSD_SCN_WAFER_HEAD][i] = szMapName;
					}
					(smfBackUp)[MSD_SCN_WAFER_HEAD][MSD_SCN_WAFER_TOTAL] = 500;
				}
				smfBackUp.Update();
				smfBackUp.Close();
			}
			smfWaferList.Close();
			RenameFile(MSD_SCN_WAFER_LIST_BU_FILE, MSD_SCN_WAFER_LIST_FILE);
			CString szMsg;
			szMsg.Format("Back up wafers list file, limit to 500");
			SaveScanTimeEvent(szMsg);
		}
	}

	return TRUE;
}

BOOL CWaferTable::StoreWaferBonded()
{
	if( m_bScnPrescanIfBonded && IsPrescanEnable()==FALSE )
	{
		CString szMapName = GetMapNameOnly();
		szMapName = szMapName.MakeUpper();
		CStringMapFile smfWaferList;
		//Check file exist 
		if ( smfWaferList.Open(MSD_SCN_WAFER_LIST_FILE, FALSE, TRUE) )
		{
			LONG lTotalWafers	= (smfWaferList)[MSD_SCN_WAFER_HEAD][MSD_SCN_WAFER_TOTAL];
			BOOL bBonded = (BOOL)(LONG)(smfWaferList)[MSD_SCN_WAFER_HEAD][szMapName];
			if( bBonded==FALSE )
			{
				lTotalWafers++;
				(smfWaferList)[MSD_SCN_WAFER_HEAD][szMapName] = 1;
				(smfWaferList)[MSD_SCN_WAFER_HEAD][lTotalWafers] = szMapName;
				(smfWaferList)[MSD_SCN_WAFER_HEAD][MSD_SCN_WAFER_TOTAL] = lTotalWafers;
				smfWaferList.Update();
				CString szMsg;
				szMsg.Format("Store information done %d of map file %s", bBonded, szMapName);
				SaveScanTimeEvent(szMsg);
			}
		}

		smfWaferList.Close();
	}

	return TRUE;
}

BOOL CWaferTable::CheckWaferBonded()
{
	if( !m_bScnPrescanIfBonded || !m_bEnableSCNFile )
	{
		return TRUE;
	}

	CString szMapName = GetMapNameOnly();
	szMapName = szMapName.MakeUpper();
	CStringMapFile smfWaferList;
	//Check file exist 
	if ( smfWaferList.Open(MSD_SCN_WAFER_LIST_FILE, FALSE, TRUE) )
	{
		BOOL bBonded = (BOOL)(LONG)(smfWaferList)[MSD_SCN_WAFER_HEAD][szMapName];
		m_bEnablePrescan = bBonded;
		CString szMsg;
		szMsg.Format("Check information done %d of map file %s", bBonded, szMapName);
		SaveScanTimeEvent(szMsg);
		SaveData();
		m_bIsPrescanning = IsPrescanEnable();
	}
	smfWaferList.Close();

	return TRUE;
}

VOID CWaferTable::SetMapName()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsWaferMapValid() == FALSE )
	{
		SaveScanTimeEvent("    WT - map in valid when set map name");
		return;
	}

	CString szFullPath = GetMapFileName();
	
	// to find ; and remove
	int nCol = szFullPath.ReverseFind(';');
	if (nCol != -1)
	{
		szFullPath = szFullPath.Left(nCol);
	}

	int iCol1 = szFullPath.ReverseFind('\\');
	int iCol2 = szFullPath.ReverseFind('.');
	if (iCol2 != -1)
	{
		m_szMapFileName = szFullPath.Mid(iCol1 + 1, iCol2 - (iCol1 + 1));
		m_szMapFileExt1 = szFullPath.Mid(iCol2);
	}
	else
	{
		m_szMapFileName = szFullPath.Mid(iCol1+1);
		m_szMapFileExt1 = "";
	}
	m_szOnlyMapFileFullPath = szFullPath;

	m_szPrescanLogPath = PRESCAN_RESULT_FULL_PATH + GetMapNameOnly();
	pUtl->SetPrescanLogPath(m_szPrescanLogPath);
	
	
	//remove("C:\\MapSorter\\UserData\\History\\6InchWft.txt");
	
	// here pass the path and map file name to ATC LIB
	CString szLogFile;
	szLogFile = gszUSER_DIRECTORY + "\\History\\Blk2FDCRegn.log";
	//remove(szLogFile);

	if ((GetTime() - m_dPrescanResultRemoveTime) > (60 * 60 * 1000) )	// to remove if last time over 1 hour
	{
		USHORT usSpanDays = 60;
		usSpanDays = pApp->GetProfileInt(gszPROFILE_SETTING, _T("PrescanResultKeepDays"), 60);
		if (usSpanDays <= 0)
			usSpanDays = 60;
		/*if (usSpanDays == 60)
		{
			if (pApp->GetCustomerName() == CTM_SANAN)
			{
				usSpanDays = 15;
			}
			if( pApp->IsPLLMRebel() )
			{
				usSpanDays = 30;
			}
		}*/
		pApp->WriteProfileInt(gszPROFILE_SETTING, _T("PrescanResultKeepDays"), usSpanDays);

		UINT unImageDays = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Grab Save Image Keep Days"), 0);
		if (unImageDays == 0)
			unImageDays = usSpanDays;

		pApp->WriteProfileInt(gszPROFILE_SETTING, _T("Grab Save Image Keep Days"), unImageDays);

		pApp->SearchAndRemoveFiles(gszUSER_DIRECTORY + "\\History", 7);
		pApp->SearchAndRemoveFiles(gszUSER_DIRECTORY + "\\PrescanResult", usSpanDays);
		pApp->SearchAndRemoveFiles(gszUSER_DIRECTORY + "\\PrescanResult\\AI Log", 1);

		m_dPrescanResultRemoveTime = GetTime();
		if (m_bACF_SaveImages)
		{
			CString szSaveImagePath;
			szSaveImagePath = WPR_PR_DISPLAY_IMAGE_LOG;
			if (IsPathExist(m_szSaveImagePath))
			{
				szSaveImagePath = m_szSaveImagePath;
			}
			pApp->SearchAndRemoveFiles(szSaveImagePath, unImageDays);
		}
		else
		{
			pApp->SearchAndRemoveFiles(WPR_PR_DISPLAY_IMAGE_LOG, unImageDays);
		}
		if (m_bACF_MapBackupPath)
		{
			pApp->SearchAndRemoveFiles(m_szBackupMapPath, usSpanDays);
		}

		if (m_bEnableAlarmLog && m_bACF_AlarmLogPath && m_szAlarmLogPath.IsEmpty() == FALSE)
		{
			pApp->SearchAndRemoveFiles(m_szAlarmLogPath, usSpanDays);
		}
	}
}

VOID CWaferTable::ClearPrescanRawData(CONST BOOL bLoadMap, CONST BOOL bClearPosn)	// when load a map file called by event
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	SaveScanTimeEvent("    WFT: to clear prescan raw data");
	m_ulPrescanGoCheckCount = m_ulPrescanGoCheckLimit;
	m_szNextLocation	= "";
	m_ulPitchAlarmGoCheckCount = 999;
	m_ulPitchAlarmGoCheckLimit	= 0;
	m_aUIGoCheckRow.RemoveAll();
	m_aUIGoCheckCol.RemoveAll();
	m_ulCrossHomeDieRow = 0;
	m_ulCrossHomeDieCol = 0;
	m_bToDoSampleOncePE	= FALSE;
	m_lRescanPECounter	= 0;	//	last time rescan/ks die pick counter

	m_ulNewPickCounter = 0;
	m_ulNextAdvSampleCount	= m_ulAdvStage1StartCount;
	m_ulNextRescanCount	= m_ulRescanStartCount;

	m_bAutoWaferLimitOk		= FALSE;
	m_lAreaPickStage		= 0;

	WT_CSubRegionsInfo::Instance()->SetRegionAligned(FALSE);	//need manual align a new region before start
	m_ulReverifyReferRegion	= 0;
	m_bRebelManualAlign		= FALSE;
	m_lPrescanVerifyResult	= 0;
	m_lPresortWftStatus		= 0;
	m_lFastHome1stFrameDone	= FALSE;
	m_lFastHome1stFrameRow	= 0;
	m_lFastHome1stFrameCol	= 0;
	m_lDummyMapMaxRow		= 0;
	m_lDummyMapMaxCol		= 0;
	m_lFastHomeIndexOffsetRow	= 0;
	m_lFastHomeIndexOffsetCol	= 0;
	m_ulTotalProbeDie		= 0;

	SaveScanTimeEvent("    WFT: to clear WPR Prescan Data");
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage();
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ClearPrescanData", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if( bLoadMap || m_szPrescanMapName.IsEmpty() )
	{
		pUtl->RemoveAllReferPoints();	// when load a new map

		if( m_szPrescanMapName.IsEmpty() )
		{
			SetMapName();
		}

		m_szPrescanMapName = GetMapNameOnly();

		m_bIsPrescanned = FALSE;	// load map file or restart application
	}

	if( IsPrescanEnded() && GetRegionSortOuter()==FALSE && 
		pUtl->GetPrescanRegionMode()==FALSE && pUtl->GetPrescanDummyMap()==FALSE )
	{
		if( Is2PhasesSortMode()==FALSE && m_bAlignNeedRescan==FALSE )	// sort mode
		{
			LONG lHmi = HmiMessage_Red_Back("Do you want to PRESCAN wafer again?", "Prompt", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER);
			if( lHmi!=glHMI_YES )
				return ;
		}
	}

	m_bIsPrescanned = FALSE;
	if (IsPrescanBlkPick())
	{
		SetPrescanBlkFuncEnable(TRUE);	// before align, block pick alignment only, enable it.
	}

	m_bIsPrescanning = IsPrescanEnable();
	m_bIntoNgPickCycle	= FALSE;


	m_bPrescan2ndTimeStart	= FALSE;

	if( bClearPosn && GetNewPickCount()==0 )
	{
		SaveScanTimeEvent("    WFT: Clear Prescan posn Data");
		ClearPrescanInfo();
		SaveScanTimeEvent("    WFT: Clear Prescan map posn Data");
		ResetMapPhyPosn();
		SaveScanTimeEvent("    WFT: Prescan map posn Data cleared");
	}

	ClearWSPrescanInfo();	// cleared every time a new map loaded
	// Clear All Physical Position if Realign

	if( m_lScnAlignMethod==3 && IsScnLoaded() && bLoadMap==FALSE )
	{
	}
	else
	{
		pUtl->DelAssistPoints();	// should add manual refer point after wafer alignment
	}
	m_ulPrescanRefPoints = pUtl->GetAssistPointsNum();

	if (IsPrescanEnable())
	{
		//Set parameters
		if( m_dPrescanLFSizeX<3.0 || m_dPrescanLFSizeY<3.0 )
		{
			SetStatusMessage("Wafer PR FOV less than 3.0 for prescan, please check");
		}

		m_nPrescanDirection	= 1;
		m_nPrescanAlignScanStage = 1;
		m_lDetectEdgeState	= 0;
	}

	if( bLoadMap )
	{
		SetErrorMessage("load map file and clear prescan raw data");
		m_bSortingDefectDie = FALSE;
		pUtl->SetPickDefectDie(FALSE);
	}

	SaveScanTimeEvent("    WFT: Prescan Data Cleared OK");

	if( bLoadMap && pApp->GetCustomerName() == CTM_SANAN && pApp ->GetProductLine() == "XA" )
	{
		if (m_bAutoLoadWaferMap != TRUE)
		{
			HmiMessage_Red_Back("map loading complete.");
		}
		else
		{
			HmiMessage("map loading complete.", "", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 1000);
		}
	}
}

INT CWaferTable::OpGetWaferPos_Prescan()
{
	BOOL			bEndOfWafer = FALSE;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsPrescanMapIndex() )	// calculate position based on pr result
	{
		if( m_nPrescanAlignScanStage==1 )	// first half below part
		{
			bEndOfWafer = PrescanOpGetPosn_1DieIndex(1);
			if( bEndOfWafer==FALSE && IsAutoDetectEdge() )
			{
				while( 1 )
				{
					LONG lMoveX = GetScanNextPosnX();
					LONG lMoveY = GetScanNextPosnY();
					if( IsPosnWithinMapRange(lMoveX, lMoveY) && IsWithinWaferLimit(lMoveX, lMoveY) )
					{
						break;
					}
					m_WaferMapWrapper.SetCurrentPosition(m_nPrescanNextMapRow, m_nPrescanNextMapCol);
					CString szText;
					szText.Format("out of wafer limit, loop next and see result ok or not");
					pUtl->PrescanMoveLog(szText);
					m_nPrescanLastMapRow	= m_nPrescanNextMapRow;
					m_nPrescanLastMapCol	= m_nPrescanNextMapCol;
					m_nPrescanLastWftPosnX	= lMoveX;
					m_nPrescanLastWftPosnY	= lMoveY;
					bEndOfWafer = PrescanOpGetPosn_1DieIndex(1, TRUE);
					if( bEndOfWafer )
					{
						break;
					}
				}
			}

			if( bEndOfWafer==TRUE )
			{
				LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
				if( GetMapIndexStepCol()%2==0 )
					lGrabOffsetX = GetDiePitchX_X()/2;
				if( GetMapIndexStepRow()%2==0 )
					lGrabOffsetY = GetDiePitchY_Y()/2;
				m_nPrescanNextWftPosnX = GetPrescanAlignPosnX() + lGrabOffsetX;
				m_nPrescanNextWftPosnY = GetPrescanAlignPosnY() + lGrabOffsetY;
				m_nPrescanNextMapCol = m_nPrescanAlignMapCol;
				m_nPrescanNextMapRow = m_nPrescanAlignMapRow;
				m_nPrescanAlignScanStage = 2;
				bEndOfWafer = FALSE;
				m_nPrescanDirection = 2;
				pUtl->SetPrescanMapIndexIdle(FALSE); // lower part complete, jump to align to do upper part
				m_lAreaPickStage = 1;
			}
		}
		else
		{
			bEndOfWafer = PrescanOpGetPosn_1DieIndex(-1);
			if( bEndOfWafer==FALSE && IsAutoDetectEdge() )
			{
				while( 1 )
				{
					LONG lMoveX = GetScanNextPosnX();
					LONG lMoveY = GetScanNextPosnY();
					if( IsPosnWithinMapRange(lMoveX, lMoveY) && IsWithinWaferLimit(lMoveX, lMoveY) )
					{
						break;
					}
					CString szText;
					szText.Format("out of wafer limit, loop next and see result ok or not");
					pUtl->PrescanMoveLog(szText);
					m_nPrescanLastMapRow	= m_nPrescanNextMapRow;
					m_nPrescanLastMapCol	= m_nPrescanNextMapCol;
					m_nPrescanLastWftPosnX	= lMoveX;
					m_nPrescanLastWftPosnY	= lMoveY;
					bEndOfWafer = PrescanOpGetPosn_1DieIndex(-1, TRUE);
					if( bEndOfWafer )
					{
						break;
					}
				}
			}
		}
	}
	else
	{
		bEndOfWafer = PrescanOpGetTablePosn();
	}

	if( IsWprWithAF() && m_bAFGridSampling )	//	427TX	4	predicate next scan move focus value (absolute)
	{
		AF_PredicateNextScanFocusLevel(m_nPrescanNextMapRow, m_nPrescanNextMapCol);
	}

	if ( bEndOfWafer == TRUE ) 
	{
		return gnNOTOK;
	}

	return gnOK;
}

INT CWaferTable::OpMoveTable_Prescan()
{
	//	427TX	4
	m_nPrescanLastWftPosnX	= GetScanNextPosnX();	// index to prescan
	m_nPrescanLastWftPosnY	= GetScanNextPosnY();	// index to prescan
	m_nPrescanLastMapRow	= m_nPrescanNextMapRow;	// index to prescan
	m_nPrescanLastMapCol	= m_nPrescanNextMapCol;	// index to prescan
	if( GetLastFrameRow() >= SCAN_MATRIX_MIN_ROW && GetLastFrameRow() < SCAN_MAX_MATRIX_ROW &&
		GetLastFrameCol() >= SCAN_MATRIX_MIN_COL && GetLastFrameCol() < SCAN_MAX_MATRIX_COL )
	{
		m_baScanMatrix[GetLastFrameRow()][GetLastFrameCol()] = FALSE;
	}
	m_lLastFrameRow		= m_lNextFrameRow;
	m_lLastFrameCol		= m_lNextFrameCol;

	m_dPreviousUpdateTime = GetTime();

	XY_MoveToPrescanPosn();

	return gnOK;
}


BOOL CWaferTable::PrescanOpGetTablePosn()
{
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	short sDown = 1;
	if( m_nPrescanAlignScanStage!=1 )	// first half below part
	{
		sDown = -1;
	}

	LONG lNextIndexRow = GetLastFrameRow();
	LONG lNextIndexCol = GetLastFrameCol();
	BOOL bGetNext = OpGetScanNextFrame(sDown, lNextIndexRow, lNextIndexCol, lNextIndexRow, lNextIndexCol);

	LONG lCircle4Points = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_SANAN && m_ucPrescanMapShapeType==0 )
	{
		lCircle4Points = 1;
	}

	if( IsAutoDetectEdge() && m_lDetectEdgeState<(3+m_ucPrescanMapShapeType+lCircle4Points) )
	{
		if( m_lDetectEdgeState == 0 && 
			(lNextIndexRow != GetAlignFrameRow()) )
		{
			lNextIndexRow = GetAlignFrameRow();
			lNextIndexCol = GetAlignFrameCol()-1;
			m_nPrescanDirection = 2;
			m_lDetectEdgeState = 1;
		}
		else if( m_lDetectEdgeState == 1 && 
			(lNextIndexRow != GetAlignFrameRow()) )
		{
			lNextIndexRow = GetAlignFrameRow()+1;
			lNextIndexCol = GetAlignFrameCol();
			m_nPrescanDirection = 1;
			m_lDetectEdgeState = 2;
			m_ucRunScanWalkTour = WT_SCAN_WALK_HOME_VERT;
		}
		else if( m_lDetectEdgeState == 2 && 
			(lNextIndexCol != GetAlignFrameCol()) )
		{
			lNextIndexRow = GetAlignFrameRow()-1;
			lNextIndexCol = GetAlignFrameCol();
			m_nPrescanDirection = 2;
			m_lDetectEdgeState = 3;
		}
		else if( m_lDetectEdgeState == 3 && 
			(lNextIndexCol != GetAlignFrameCol()) )
		{
			m_lDetectEdgeState = 4;
		}

		if( m_lDetectEdgeState == (3+m_ucPrescanMapShapeType+lCircle4Points) )
		{
			while( 1 )
			{
				Sleep(10);
				if( pUtl->GetPrescanMapIndexIdle() && m_bPrescanLastGrabbed==TRUE  )
				{
					Sleep(10);
					break;
				}
			}

			OpCalculateADEWaferLimit();

			LONG lStartRow = lNextIndexRow, lStartCol = lNextIndexRow;
			SHORT sDir = 1;
			switch( m_ucScanWalkTour )
			{
			case WT_SCAN_WALK_LEFT_VERT:
			case WT_SCAN_WALK_HOME_VERT:
				if( m_ucPrescanMapShapeType == WT_MAP_SHAPE_CIRCLE )
				{
					m_ucRunScanWalkTour = WT_SCAN_WALK_HOME_VERT;
					m_nPrescanDirection = 1;
					m_nPrescanAlignScanStage = 1;
					sDir = 1;
					lStartRow = SCAN_MAX_MATRIX_ROW-2;
					lStartCol = GetAlignFrameCol();
				}
				else
				{
					m_ucRunScanWalkTour = WT_SCAN_WALK_HOME_VERT;
					m_nPrescanDirection = 1;
					m_nPrescanAlignScanStage = 1;
					sDir = 1;
					lStartRow = SCAN_MATRIX_MIN_ROW;
					lStartCol = GetAlignFrameCol()+1;
				}
				break;
			case WT_SCAN_WALK_TOP_HORI:
			case WT_SCAN_WALK_HOME_HORI:
			default:
				if( m_ucPrescanMapShapeType == WT_MAP_SHAPE_CIRCLE )
				{
					m_ucRunScanWalkTour = WT_SCAN_WALK_HOME_HORI;
					m_nPrescanDirection = 2;
					m_nPrescanAlignScanStage = 2;
					sDir = -1;
					lStartRow = SCAN_MAX_MATRIX_ROW;
					lStartCol = SCAN_MAX_MATRIX_COL;
				}
				else
				{
					m_ucRunScanWalkTour = WT_SCAN_WALK_TOP_HORI;
					m_nPrescanDirection = 1;
					m_nPrescanAlignScanStage = 1;
					sDir = 1;
					lStartRow = SCAN_MATRIX_MIN_ROW;
					lStartCol = SCAN_MATRIX_MIN_COL;
				}
				break;
			}
			bGetNext = OpGetScanNextFrame(sDir, lStartRow, lStartCol, lNextIndexRow, lNextIndexCol);
		}
	}

	if( bGetNext==FALSE && m_nPrescanAlignScanStage==1 )
	{
		m_nPrescanDirection = 2;
		if( m_ucRunScanWalkTour<WT_SCAN_WALK_TOP_HORI )
		{
			LONG l2ndFrameRow = GetAlignFrameRow();
			LONG l2ndFrameCol = GetAlignFrameCol();
			bGetNext = OpGetScanNextFrame(-1, l2ndFrameRow, l2ndFrameCol, lNextIndexRow, lNextIndexCol);
		}
		m_nPrescanAlignScanStage = 2;
		pUtl->SetPrescanMapIndexIdle(FALSE); // lower part complete, jump to align to do upper part
		m_lAreaPickStage = 1;
	}

	if( bGetNext )
	{
		m_lNextFrameRow = lNextIndexRow;
		m_lNextFrameCol = lNextIndexCol;
		m_nPrescanNextWftPosnX = m_laScanPosnX[lNextIndexRow][lNextIndexCol];
		m_nPrescanNextWftPosnY = m_laScanPosnY[lNextIndexRow][lNextIndexCol];
		if( GetDiePitchX_X()!=0 && GetDiePitchY_Y()!=0 )
		{
			LONG lScanRow = (GetPrescanAlignPosnY() - GetScanNextPosnY())/GetDiePitchY_Y() + m_nPrescanAlignMapRow;
			LONG lScanCol = (GetPrescanAlignPosnX() - GetScanNextPosnX())/GetDiePitchX_X() + m_nPrescanAlignMapCol;
			if( lScanRow<=0 )
				lScanRow = 0;
			if( lScanCol<0 )
				lScanCol = 0;
			ULONG ulMaxRow = 0, ulMaxCol = 0;
			m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
			if(lScanRow>=(LONG)ulMaxRow )
				lScanRow = ulMaxRow-1;
			if(lScanCol>=(LONG)ulMaxCol )
				lScanCol = ulMaxCol-1;
			m_nPrescanNextMapRow = lScanRow;
			m_nPrescanNextMapCol = lScanCol;
		}
	}

	return !bGetNext;
}

BOOL CWaferTable::IsPosnWithinMapRange(INT nPosnX, INT nPosnY)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if ( nPosnX > GetScanUL_X() || nPosnX < GetScanLR_X() || nPosnY > GetScanUL_Y() || nPosnY < GetScanLR_Y() )
	{
		return FALSE;
	}

	if( pUtl->GetPrescanRegionMode() )
	{
		return TRUE;
	}

	if ( m_ucPrescanMapShapeType == WT_MAP_SHAPE_CIRCLE )
	{
		DOUBLE dDistance = GetDistance(GetScanCtrX(), GetScanCtrY(), nPosnX, nPosnY);
		DOUBLE dMapRadius = GetScanRadius();

		if( m_bDummyQuarterWafer )	// check limit when prescan move
		{
			DOUBLE nDeltaX = (nPosnX - GetScanCtrX());
			DOUBLE nDeltaY = (nPosnY - GetScanCtrY());
			switch( m_ucQuarterWaferCenter ) // check limit when prescan move
			{
			case 0:	// UL
				if( nDeltaX>0 || nDeltaY>0 )
				{
					if( nDeltaX>0 && fabs(nDeltaX)<=dMapRadius/10 && nDeltaY<=dMapRadius/10 )
					{
						return TRUE;
					}
					if( nDeltaY>0 && fabs(nDeltaY)<=dMapRadius/10 && nDeltaX<=dMapRadius/10 )
					{
						return TRUE;
					}
					return FALSE;
				}
				break;
			case 1: // UR
				if( nDeltaX<0 || nDeltaY>0 )
				{
					if( nDeltaX<0 && fabs(nDeltaX)<=dMapRadius/10 && nDeltaY<=dMapRadius/10 )
					{
						return TRUE;
					}
					if( nDeltaY>0 && fabs(nDeltaY)<=dMapRadius/10 && nDeltaX>=-dMapRadius/10 )
					{
						return TRUE;
					}
					return FALSE;
				}
				break;
			case 2: // LL
				if( nDeltaX>0 || nDeltaY<0 )
				{
					if( nDeltaX>0 && fabs(nDeltaX)<=dMapRadius/10 && nDeltaY>=-dMapRadius/10 )
					{
						return TRUE;
					}
					if( nDeltaY<0 && fabs(nDeltaY)<=dMapRadius/10 && nDeltaX<=dMapRadius/10 )
					{
						return TRUE;
					}
					return FALSE;
				}
				break;
			case 3: // LR
				if( nDeltaX<0 || nDeltaY<0 )
				{
					if( nDeltaX<0 && fabs(nDeltaX)<=dMapRadius/10 && nDeltaY>=-dMapRadius/10 )
					{
						return TRUE;
					}
					if( nDeltaY<0 && fabs(nDeltaY)<=dMapRadius/10 && nDeltaX>=-dMapRadius/10 )
					{
						return TRUE;
					}
					return FALSE;
				}
				break;
			}
		}

		if( dDistance>dMapRadius )
			return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::PrescanUpdateWaferAlignment(INT lX, INT lY, ULONG lInRow, ULONG lInCol, USHORT usAlignEntry)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();

	CheckWaferBondedSize();	//	SCN if wafer bonded, auto enable prescan, 
	if( CheckEjectorPinLifeB4Start()==FALSE )
	{
		return FALSE;
	}

	SaveScanTimeEvent("    WFT: To Prescan Update wafer alignment");
	if (Is2PhasesSortMode() && !IsMS90Sorting2ndPart())	//	manual align set/reset some variables
	{
		m_b2PartsAllDone		= FALSE;	// do alignment, become false
		if( IsOutMS90SortingPart(lInRow, lInCol) )
		{
			SetAlignmentStatus(FALSE);
			HmiMessage_Red_Yellow(	"ERROR : Prescan Stopped as wrong align map point." , "Prescan Error");
			return FALSE;
		}
	}
	m_bSortGoingTo2ndPart	= FALSE;	// after alignment, set to FALSE

	if( m_bCheckMapHomeNullBin )
	{
		ULONG ulRow = 0, ulCol = 0;
		GetMapAlignHomeDie(ulRow, ulCol);
		if (m_pWaferMapManager->IsMapHaveBin(ulRow, ulCol))
		{
			SetAlignmentStatus(FALSE);
			HmiMessage_Red_Back("Home die in map is not a null bin.", "Wafer Map");
			return FALSE;
		}
	}

	BOOL bClearDieStatus = m_ucPrescanEdgeSize==0 && IsWaferMapValid() && (m_WaferMapWrapper.GetReader() != NULL);
	CString szAlgorithm, szPathFinder;
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
	if (szAlgorithm.Find("Sorting Path 1") == -1)
	{
		bClearDieStatus = FALSE;
	}
	else if( IsPrescanEnable() && m_bLoadedWaferScannedOnce==FALSE )
	{
		bClearDieStatus = FALSE;
	}
	// to reset die status after prescan in case realign has 2 L shape die marked
	if( bClearDieStatus )
	{
		SaveScanTimeEvent("    WFT: to clear die status");
		ULONG ulTgtRegion = 0, ulMaxRow = 0, ulMaxCol = 0;
		m_WaferMapWrapper.GetMapDimension(ulMaxRow, ulMaxCol);
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		if( pUtl->GetPrescanRegionMode() )
		{
			ulTgtRegion = pSRInfo->GetTargetRegion();
		}
		for (ULONG ulRow=0; ulRow<ulMaxRow; ulRow++)
		{
			for (ULONG ulCol=0; ulCol<ulMaxCol; ulCol++)
			{
				BOOL bSkip = IsOutMS90SortingPart(ulRow, ulCol);
				if( pUtl->GetPrescanRegionMode() )
				{
					if( pSRInfo->IsWithinThisRegion(ulTgtRegion, ulRow, ulCol)==FALSE )
					{
						bSkip = TRUE;
					}
				}
				if( bSkip )
				{
					continue;
				}
				if ( m_WaferMapWrapper.GetReader()->IsMarkedDie(ulRow, ulCol) == TRUE ) 
				{
					ULONG ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
					m_WaferMapWrapper.MarkDie(ulRow, ulCol, FALSE);
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_DEFAULT);
				}
				m_WaferMapWrapper.ClearDieStatus(ulRow, ulCol);
				//	m_WaferMapWrapper.HideDie(ulRow, ulCol);
			}
		}
//it will restore all picked die if call UnhideAllDie() function.
//		m_WaferMapWrapper.UnhideAllDie();
		SaveScanTimeEvent("    WFT: clear die status done");
	}

	if( m_ucSpiralPickDirection==4 )	//	NICHIA_TESTING
	{
		m_WaferMapWrapper.SetAlgorithmParameter("Single Dir", TRUE);
	}
	else
	{
		m_WaferMapWrapper.SetAlgorithmParameter("Single Dir", FALSE);
	}
	//m_WaferMapWrapper.SetAlgorithmParameter("Single Dir", TRUE);

	ULONG ulScnAlnRow = lInRow;
	ULONG ulScnAlnCol = lInCol;
	m_WaferMapWrapper.SetCurrentPosition(ulScnAlnRow, ulScnAlnCol);

	pUtl->SetRealignPosition(ulScnAlnRow, ulScnAlnCol, lX, lY);
	pUtl->SetRealignGlobalTheta(GetGlobalT());

	m_ucPrescanRealignMethod = 0;	//	realign scanned wafer begin
	pUtl->SetAlignPosition(ulScnAlnRow, ulScnAlnCol, lX, lY);
	pUtl->SetPrescanGlobalTheta(GetGlobalT());

	m_ulAlignHomeDieRow	= ulScnAlnRow;
	m_ulAlignHomeDieCol	= ulScnAlnCol;
	m_nPrescanAlignMapRow = m_nPrescanNextMapRow	= m_nPrescanLastMapRow	 = ulScnAlnRow;
	m_nPrescanAlignMapCol = m_nPrescanNextMapCol	= m_nPrescanLastMapCol	 = ulScnAlnCol;
	m_nAlignHomeDieWftX	= lX;
	m_nAlignHomeDieWftY	= lY;
	m_nPrescanAlignPosnX  = lX;
	m_nPrescanAlignPosnY  = lY;

	// 0 - manual align by HMI button; 1 - half auto by HMI; 2 - Fully auto align
	// 3 - rescan aoi wafer in cycle;  4 - region align home;5 - 
	if( usAlignEntry<4 && (IsScnLoaded() || IsRealignScnnedWafer()) )
	{
		CString szMsg;
		szMsg.Format("Prescan update align %d, SCN %d, Realign %d, begin to align scanned wafer", 
			usAlignEntry, IsScnLoaded(), IsRealignScnnedWafer());
		ScnAlignLog(szMsg);
		SaveScanTimeEvent("    WFT: " + szMsg);
		if( usAlignEntry==0 || usAlignEntry==1 || usAlignEntry==2 )
		{
			m_ulCurrentRow = lInRow;
			m_ulCurrentCol = lInCol;
			m_lCurrent_X = lX;
			m_lCurrent_Y = lY;

			XY_SafeMoveTo(lX, lY);
			LONG lRtn = AlignScannerWafer();
			if ( lRtn == WT_ALN_SCN_OK || lRtn == WT_ALN_SCN_WARNING )
			{
				m_ucPrescanRealignMethod = 2;	// manual align wafer, scnner ok
			}

			XY_SafeMoveTo(lX, lY);
			m_WaferMapWrapper.SetCurrentPosition(lInRow, lInCol);
			Sleep(50);
			m_WaferMapWrapper.SetCurrentPosition(lInRow, lInCol);
		}

		if( usAlignEntry==3 )
		{
			XY_SafeMoveTo(lX, lY);
			LONG lCurrX = 0, lCurrY = 0, lCurrT = 0;
			//Index to Offset Pos if need
			if( IsScnLoaded() )
				IndexToSCNOffsetPos(&m_ulAlignRow, &m_ulAlignCol);
			GetEncoder(&lCurrX, &lCurrY, &lCurrT);
			m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);	
			SetAlignStartPoint(FALSE, lCurrX, lCurrY, m_ulAlignRow, m_ulAlignCol);
			m_WaferMapWrapper.EnableAutoAlign(!m_bStartPoint);

			m_ulCurrentRow = m_ulAlignRow;
			m_ulCurrentCol = m_ulAlignCol;
			m_lCurrent_X = lCurrX;
			m_lCurrent_Y = lCurrY;

			LONG lRtn = AlignScannerWafer();
			if ( lRtn == WT_ALN_SCN_OK || lRtn == WT_ALN_SCN_WARNING )
			{
				m_ucPrescanRealignMethod = 2;	// semi_auto align wafer, scnner ok
			}

			//Return to Align position	
			SetAlignStartPoint(FALSE, lCurrX, lCurrY, m_ulAlignRow, m_ulAlignCol);

			//Update Wafertable & wafer map start position
			m_lStart_X	= lCurrX;
			m_lStart_Y	= lCurrY;
			m_WaferMapWrapper.SetStartPosition(m_ulAlignRow, m_ulAlignCol);
		}
	}

	m_lRescanStartRow			= -1;
	m_lRescanStartCol			= -1;
	m_lRescanStartWftX			= 0;
	m_lRescanStartWftY			= 0;
	m_ulNewPickCounter			= 0;

	if( IsPrescanEnable() && m_bSmallMapSortRpt && m_bGenRptOfSMS && 
		CPreBondEvent::m_nMapOrginalCount<=SCAN_SMS_DIE_MAX_LIMIT)
	{
		CString szSMSRptFile = m_szPrescanLogPath + ".sms";	// alignment, open the file and update the map.
		CStdioFile cfFile;
		if (cfFile.Open(szSMSRptFile,  
						CFile::modeNoTruncate | CFile::modeRead | CFile::shareExclusive | CFile::typeText) )
		{
			UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
			CString szData;
			cfFile.SeekToBegin();
			while( cfFile.ReadString(szData)!=NULL )
			{
				LONG lUserRow = 0, lUserCol = 0;
				ULONG ulRow = 0, ulCol = 0;
				CString szRow = "0";
				CString szCol = "0";

				INT nCol = szData.Find(",");
				if (nCol != -1)
				{
					szRow = szData.Left(nCol);
					szData = szData.Right(szData.GetLength() - nCol - 1);
				}

				nCol = szData.Find(",");
				if (nCol != -1)
				{
					szCol = szData.Left(nCol);
					szData = szData.Right(szData.GetLength() - nCol - 1);

				}

				lUserRow = atoi(szRow);
				lUserCol = atoi(szCol);

				ConvertOrgUserToAsm(lUserRow, lUserCol, ulRow, ulCol);
				if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol)==FALSE &&
					m_pWaferMapManager->IsMapHaveBin(ulRow, ulCol) )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNullBin);
				}
			}
			cfFile.Close();
		}
	}

	m_stLastAdvSampleTime	= CTime::GetCurrentTime();	// just after realignment.
	m_ulNextAdvSampleCount	= m_ulAdvStage1StartCount;
	m_ulLastRscnPickCount = 0;
	m_ulNextRescanCount	= m_ulRescanStartCount;

	pUtl->ScanImageHistory(GetMapNameOnly(), TRUE);	// alignment
	CString szMoveMsg;
	szMoveMsg.Format("Align before Map Valid Dimension, Row(%ld,%ld), Col(%ld,%ld)", GetMapValidMinRow(), GetMapValidMaxRow(), GetMapValidMinCol(), GetMapValidMaxCol());
	pUtl->PrescanMoveLog(szMoveMsg, TRUE);
	ObtainMapValidRange();	// wafer alignment
	szMoveMsg.Format("Align after  Map Valid Dimension, Row(%ld,%ld), Col(%ld,%ld)", GetMapValidMinRow(), GetMapValidMaxRow(), GetMapValidMinCol(), GetMapValidMaxCol());
	pUtl->PrescanMoveLog(szMoveMsg, TRUE);

	CString szLogFileName;
	szLogFileName = m_szPrescanLogPath + "_KeyDie.csv";			DeleteFile(szLogFileName);
	if( szAlgorithm.Find("Sorting Path 1")!=-1 && (m_lBuildShortPathInScanning==0) )
	{
		LONG lHoriFactor = 1, lVertFactor = 1;	//	the small value will move most.
		LONG lPitchX = labs(GetDiePitchX_X());
		LONG lPitchY = labs(GetDiePitchY_Y());
		LONG lDiffXY = labs(lPitchX - lPitchY);
		switch( m_ucSpiralPickDirection )	//	default, YV, X>, Auto
		{
		case 3:	//	auto
			if( lPitchX>lPitchY )
			{
				if( lDiffXY>=(lPitchY*20/100) )
				{
					lHoriFactor = 15;
					lVertFactor = 10;
				}
			}
			else
			{
				if( lDiffXY>=(lPitchX*20/100) )
				{
					lHoriFactor = 10;
					lVertFactor = 15;
				}
			}
			break;
		case 2:	//	X>
			lHoriFactor = 10;
			lVertFactor = 15;
			break;
		case 1:	//	YV
			lHoriFactor = 15;
			lVertFactor = 10;
			break;
		case 0:	//	default
		default:
			break;
		}

		m_WaferMapWrapper.SetAlgorithmParameter("Horizontal Factor", lHoriFactor);
		m_WaferMapWrapper.SetAlgorithmParameter("Vertical Factor",	 lVertFactor);

		CString szMsg;
		szMsg.Format("WFT: to reset grade, and auto align map, path 1 mode %d Hori %d, Vert %d based on pitch %ld,%ld",
			m_ucSpiralPickDirection, lHoriFactor, lVertFactor, lPitchX, lPitchY);
		SaveScanTimeEvent(szMsg);
		m_WaferMapWrapper.ResetGrade();
		m_WaferMapWrapper.SetStartPosition(lInRow, lInCol);
		m_WaferMapWrapper.EnableAutoAlign(!m_bStartPoint);
	}

	// check whether the colunm number loaded is equal to header number loaded
	if ( !CheckHeaderElectricData() )
	{
		SetAlignmentStatus(FALSE);
		HmiMessage(	"ERROR : Prescan Stopped." , "Prescan Error");
		return TRUE;
	}

	SwitchEncoderReader(IsWT2InUse());
	m_bVerifyMapWaferFail	= FALSE;
	m_lVerifyMapWaferRow	= -10000;
	m_lVerifyMapWaferCol	= -10000;
	memset(m_laPCIVerifyFailRow, -10000, sizeof(m_laPCIVerifyFailRow));
	memset(m_laPCIVerifyFailCol, -10000, sizeof(m_laPCIVerifyFailCol));

	if( pApp->GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA) )
	{
		if( m_WaferMapWrapper.IsReferenceDie(lInRow, lInCol) )
		{
			m_WaferMapWrapper.SetReferenceDie(lInRow, lInCol, FALSE);
		}
	}

	// check time and back up and go on with new file
	CString szFileName;
	if( pUtl->GetPrescanDummyMap()==FALSE )
	{
		BOOL bOK = TRUE;
		IPC_CServiceMessage stMsg;
		int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "CheckHolePatternInThisMap", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bOK);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if( bOK==FALSE )
		{
			SetAlignmentStatus(FALSE);
		}
	}

	m_lAreaPickStage	= 1;
	m_bPrescanningMode	= FALSE;
	m_bIntoNgPickCycle	= FALSE;
	m_dScanMoveTime		= 0.0;
	m_dScanTime			= 0.0;
	m_dCurrTime			= 0.0;
	m_dStopTime			= GetTime();
	m_nPickListIndex	= -1;
	m_bPrescanStarted	= FALSE;
	m_stPrescanStartTime	= CTime::GetCurrentTime();	//	normal prescan alignment time

	m_lRescanRunStage		= 0;	//	alignment
	m_lWftAdvSamplingStage	= 0;
	m_lWftAdvSamplingResult	= 0;

	m_stStopTime = CTime::GetCurrentTime();	// after wafer alignment
	m_stReadTmpTime = CTime::GetCurrentTime();

	ulScnAlnRow = lInRow;
	ulScnAlnCol = lInCol;

	m_lLastPickRow			= ulScnAlnRow;
	m_lLastPickCol			= ulScnAlnCol;
	m_bLastPickSet			= FALSE;
	m_bIsWaferEnded			= FALSE;

	// 0 is default, 1 is ready to build path, 2 is building path.
	m_lBuildShortPathInScanning = 0;	// prescan update wafer alignment, restart.
	// sort mode, 2nd part align wafer complete
	m_bScnModeWaferAlignTwice	= FALSE;
	DWTDSortLog("align ok and update prescan");
	INT i=0, j=0;

	CString szLogMsg;
	LONG lOrgRow, lOrgCol;
	ConvertAsmToOrgUser(lInRow, lInCol, lOrgRow, lOrgCol);
	szLogMsg.Format("    WFT: %s; Predict method=%d; FPC=%d; Asm map(%d,%d)org(%d,%d); WFT(%d,%d)", 
		GetMapNameOnly(), m_lPredictMethod, IsEnableFPC(),
		lInRow, lInCol, lOrgRow, lOrgCol, lX, lY);
	SaveScanTimeEvent(szLogMsg, TRUE);

	if( DEB_IsEnable() )
	{
		if( m_bGiveUpDebKeyDie )
			m_WaferMapWrapper.SetAlgorithmParameter("GIVEUP KEYDIE", TRUE);
		else
			m_WaferMapWrapper.SetAlgorithmParameter("GIVEUP KEYDIE", FALSE);
	}
	CString szMsg;
	m_bDEB_INIT_OK	= FALSE;

	CString szDateTime;
	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	szDateTime = ctDateTime.Format("%Y%m%d%H%M%S");

	m_lRunPredScanPitchTolX = m_lPredScanPitchTolX;
	m_lRunPredScanPitchTolY = m_lPredScanPitchTolY;

	DOUBLE dUsedTime = GetTime();
	m_lTimeSlot[16] = (LONG)(GetTime()-dUsedTime);			//	16.	prescan update wafer alignment, IM change wafer used time.
	m_bPitchAlarmOnce = FALSE;
	m_bReSampleAsError	= FALSE;
	m_ulPitchAlarmCount	= 0;

	if(	pUtl->GetPrescanRegionMode() )
	{
		m_pWaferMapManager->DrawAllRegionBoundary();
	}

	if( IsPrescanBlkPick() )
		SetPrescanBlkFuncEnable(FALSE);	// after align, block pick alignment only, disable it.

	pUtl->SetRealignPosition(ulScnAlnRow, ulScnAlnCol, lX, lY);
	pUtl->SetRealignGlobalTheta(GetGlobalT());

	if( IsScnLoaded() )	//	ADV, DEB, FPC
	{
		if( IsAdvRegionOffset() )
			pRgnPdc->InitRegionOffset();

		BOOL	bCheckDieResult = TRUE;
		m_WaferMapWrapper.Redraw();
		Sleep(1000);

		// update deb and key die
		DEB_UpdateAllDicePos();
		DEB_CheckKeyDicesOnWafer();
		if( IsEnableFPC() )
		{
			bCheckDieResult = FivePointCheckDiePosition();	//	SCN file
			if ( (m_bScnCheckByPR == TRUE) && (bCheckDieResult != TRUE) )
			{
				return TRUE;		
			}
		}

		SavePrescanInfo();
		SavePrescanInfoMap(1);
		return TRUE;
	}
	
	if( !IsPrescanEnable() )
	{
		return TRUE;
	}

	// Pass the data just like on load map event
	if( IsPrescanEnded()==FALSE )
	{
		if( (IsBlkPickAlign() || IsBlkFuncEnable()) && !pSRInfo->IsManualAlignRegion())
		{
			LONG lMapRow, lMapCol, lTableX, lTableY;
			ULONG i;
			for(i=1; i<=m_pBlkFunc2->GetTotalRefPoints(); i++)
			{
				if( m_pBlkFunc2->Blk2GetRefDiePosn(i, lMapRow, lMapCol, lTableX, lTableY) )
				{
					if( lMapRow==ulScnAlnRow && lMapCol==ulScnAlnCol )
					{
						lX = lTableX;
						lY = lTableY;
					}
				}
			}
		}
	}

	XY_SafeMoveTo(lX, lY);
	if( pUtl->GetPrescanRegionMode()==FALSE )
	{
		SearchNml3AtAlignPointForPrescan(lX, lY);
	}

	pUtl->SetRealignPosition(ulScnAlnRow, ulScnAlnCol, lX, lY);
	pUtl->SetRealignGlobalTheta(GetGlobalT());

	if( IsRealignScnnedWafer() )
	{
		if( IsBlkPickAlign() )
		{
			LONG lMapRow, lMapCol, lTableX, lTableY, lRefRow, lRefCol, lRefX, lRefY;
			ULONG i, j;
			for(j=0; j<pUtl->GetNumOfReferPoints(); j++)
			{
				pUtl->GetReferPosition(j, lRefRow, lRefCol, lRefX, lRefY);
				for(i=1; i<=m_pBlkFunc2->GetTotalRefPoints(); i++)
				{
					if( m_pBlkFunc2->Blk2GetRefDiePosn(i, lMapRow, lMapCol, lTableX, lTableY) )
					{
						if( lMapRow==lRefRow && lMapCol==lRefCol )
						{
							pUtl->UpdateReRefPosition(j, lMapRow, lMapCol, lTableX, lTableY);
						}
					}
				}
			}
			UpdatePrescanPosnAfterBlkPkRealign();
		}
		else
		{
			UpdatePrescanPosnAfterWaferRealign();
		}
		m_ucPrescanRealignMethod = 0;	//	realign scanned wafer done

		ScnAlignLog("update deb (if enable)");
		DEB_UpdateAllDicePos();
		DEB_CheckKeyDicesOnWafer();
		//Use 5-pt-check fcn from scanner mode for prescan result checking 
		if( IsEnableFPC() )
		{
			FivePointCheckDiePosition();
		}

		if( IsAdvRegionOffset() )
			pRgnPdc->InitRegionOffset();

		m_bScnModeWaferAlignTwice	= TRUE;

		EmptyDieGradeCheck();

		RectWaferCornerCheck();

		m_WaferMapWrapper.Redraw();
		Sleep(1000);

		SavePrescanInfo();
		SavePrescanInfoMap(1);

		return TRUE;
	}

	m_ucPrescanRealignMethod = 0;	//	reset for other mode
	pUtl->SetPrescanGlobalTheta(GetGlobalT());

	m_ulAlignHomeDieRow	= ulScnAlnRow;
	m_ulAlignHomeDieCol	= ulScnAlnCol;
	m_nAlignHomeDieWftX	= lX;
	m_nAlignHomeDieWftY	= lY;
	m_nPrescanAlignPosnX  = lX;
	m_nPrescanAlignPosnY  = lY;

	LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
	if( IsPrescanMapIndex() )
	{
		if( GetMapIndexStepCol()%2==0 )
			lGrabOffsetX = GetDiePitchX_X()/2;
		if( GetMapIndexStepRow()%2==0 )
			lGrabOffsetY = GetDiePitchY_Y()/2;
	}
	m_nPrescanNextWftPosnX	= m_nPrescanLastWftPosnX = lX + lGrabOffsetX;	// update wafer alignment
	m_nPrescanNextWftPosnY	= m_nPrescanLastWftPosnY = lY + lGrabOffsetY;

	m_nPrescanAlignMapRow = m_nPrescanNextMapRow	= m_nPrescanLastMapRow	 = ulScnAlnRow;
	m_nPrescanAlignMapCol = m_nPrescanNextMapCol	= m_nPrescanLastMapCol	 = ulScnAlnCol;
	SetMapPhyPosn(ulScnAlnRow, ulScnAlnCol, lX, lY);
	m_nPrescanDirection = 1;
	m_nPrescanAlignScanStage = 1;
	m_lDetectEdgeState	= 0;
	m_bIsPrescanning = TRUE;
	m_bIsPrescanned = FALSE;
	m_lPrescanHomeDieRow = -1;
	m_lPrescanHomeDieCol = -1;
	m_lPrescanHomeWaferX = lX;
	m_lPrescanHomeWaferY = lY;
	pUtl->SetAlignPosition(ulScnAlnRow, ulScnAlnCol, lX, lY);
	m_bPrescanningMode = TRUE;

	WT_CSubRegionsInfo::Instance()->SetRegionAligned(FALSE);	// after wafer alignment
	DefineAreaAndWalkPath_Prescan();

	LONG lDistX = GetPrescanPitchX();
	LONG lDistY = GetPrescanPitchY();
	LONG lTime_X = X_ProfileTime(LOW_PROF, lDistX, lDistX);
	LONG lTime_Y = Y_ProfileTime(LOW_PROF, lDistY, lDistY);
	if( IsESMachine() )		//v4.37T10
	{
		if (labs(lDistX) <= GetSlowProfileDist())
		{
			lTime_X = X_ProfileTime(NORMAL_PROF, lDistX, lDistX);
		}

		if (labs(lDistY) <= GetSlowProfileDist())
		{
			lTime_Y = Y_ProfileTime(NORMAL_PROF, lDistY, lDistY);
		}
		szMsg.Format("    WFT: Prescan index X dist %ld, time %ld; Y dist %ld, time %ld Delay %ld, table offset %d,%d",
			lDistX, lTime_X, lDistY, lTime_Y, m_lPrescanPrDelay, m_lWT2OffsetX, m_lWT2OffsetY);
	}
	else
	{
		szMsg.Format("    WFT: Prescan index X dist %ld, time %ld; Y dist %ld, time %ld Delay %ld",
			lDistX, lTime_X, lDistY, lTime_Y, m_lPrescanPrDelay);
	}
	SaveScanTimeEvent(szMsg);
	m_lWftScanIndexAndDelayTime = max(lTime_X, lTime_Y) + m_lPrescanPrDelay;

	if( pSRInfo->GetInitState()==WT_SUBREGION_INIT_LOADMAP && !IsBlkPickAlign() )
	{
		if( (m_bFindAllRefer4Scan) )
		{
			FindAndAlignAllReferDie();
		}
		else if (pApp->GetCustomerName()!=CTM_RENESAS && m_bFindHomeDieFromMap)
		{
			FindAndAlignAllReferDie();
		}
		else if (!pSRInfo->IsManualAlignRegion() && m_bSingleHomeRegionScan )	//	home die as refer for all regions
		{
			pUtl->RegionOrderLog("Add home die to refer list");
			pUtl->RemoveAllReferPoints();	// after wafer alignment to update values
			pUtl->AddAllReferPosition(ulScnAlnRow, ulScnAlnCol, lX, lY);
		}
		else
		{
			if( pUtl->GetPrescanRegionMode() )
			{
				WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
				pSRInfo->SetCurrentRegionState_HMI("M");
				HmiMessage_Red_Back("Region scan in manual mode, 'Find All irregular refer' option is disable", "Prescan");
			}
		}
	}

	if( (IsBlkPickAlign() || IsBlkFuncEnable()) && !pSRInfo->IsManualAlignRegion() && m_bSingleHomeRegionScan==FALSE )
	{
		LONG lMapRow, lMapCol, lTableX, lTableY;
		ULONG i;
		pUtl->RemoveAllReferPoints();	// after wafer alignment to update values
		for(i=1; i<=m_pBlkFunc2->GetTotalRefPoints(); i++)
		{
			if( m_pBlkFunc2->Blk2GetRefDiePosn(i, lMapRow, lMapCol, lTableX, lTableY) )
			{
				pUtl->AddAllReferPosition(lMapRow, lMapCol, lTableX, lTableY);
				CString szMsg;
				szMsg.Format("refer region %ld, %ld  %ld, %ld", lMapRow, lMapCol, lTableX, lTableY);
				pUtl->RegionOrderLog(szMsg);
			}
		}
	}

	if (IsRegionAlignRefBlk() && pSRInfo->GetInitState() == WT_SUBREGION_INIT_LOADMAP)
	{
		ULONG i;
		LONG lMapRow, lMapCol, lTableX, lTableY;
		BOOL bHasNoRef = FALSE;
		BOOL bFindRef = FALSE;
		CString szMsg;
		ULONG j;
		ULONG ulRegionNo = 0;
		for (i = 0; i < pSRInfo->GetTotalSubRegions(); i++)
		{
			i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);		//Klocwork	//v4.02T5

			bFindRef = FALSE;
			ulRegionNo = pSRInfo->GetSortRegion(i);
			for (j = 0; j < pUtl->GetNumOfReferPoints(); j++)
			{
				pUtl->GetReRefPosition(j, lMapRow, lMapCol, lTableX, lTableY);
				if( pSRInfo->IsWithinThisRegion(ulRegionNo, lMapRow, lMapCol) )	// check region has refer die of blk pick
				{
					bFindRef = TRUE;
					break;
				}
			}

			if (m_bManualRegionScanSort || pSRInfo->GetTotalSubRegions() == 1)
			{
				bFindRef = TRUE;
			}

			if (m_bSingleHomeRegionScan)	// no assistance region, scan align region sort
			{
				bFindRef = TRUE;
			}

			if (bFindRef == FALSE && pSRInfo->GetRegionState(ulRegionNo) != WT_SUBREGION_STATE_BONDED )
			{
				pSRInfo->SetRegionState(ulRegionNo, WT_SUBREGION_STATE_SKIPPED);
				szMsg.Format("Region %lu set to skipped", ulRegionNo);
				pUtl->RegionOrderLog(szMsg);
			}
		}

		// reset to make skip region assist or keep unchange, also re-order to last include bonded
		pSRInfo->ResetSkippedRegionList();
/*
		//if not manual align, need find the first sorting region
		if (usAlignEntry > 0)
		{
			BOOL bFindNext = pSRInfo->FindNextAutoSortRegion(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(),
																 GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol());
			if (!bFindNext)
			{
				//Display some error

			}
		}
*/
		pSRInfo->SetInitState(WT_SUBREGION_INIT_ALIGN);
//		m_bManualAlignRegion = TRUE;

		bHasNoRef = FALSE;
		CString szTemp, szList;
		for(i=0; i<pSRInfo->GetTotalSubRegions(); i++)
		{
			i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);		//Klocwork	//v4.02T5

			ulRegionNo = pSRInfo->GetSortRegion(i);
			BOOL bOutMS90SortingPart = pSRInfo->IsOutMS90SortingPart(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(), GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol(), ulRegionNo);
			if (!bOutMS90SortingPart && (pSRInfo->GetRegionState(ulRegionNo) == WT_SUBREGION_STATE_SKIPPED))
			{
				bHasNoRef = TRUE;
				szTemp.Format("(%lu)", ulRegionNo);
				szList += szTemp;
			}
		}

		if( bHasNoRef )
		{
			szMsg = "No any refer die in regions\nThese regions would be skipped in auto";
			HmiMessage_Red_Back(szMsg, "Region Prescan");
			pUtl->RegionOrderLog(szMsg);
			HmiMessage_Red_Back(szList, "Region Prescan");
			pUtl->RegionOrderLog(szList);
		}

		SaveRegionStateAndIndex();
		szMsg.Format("Align Region Sorting List:%s", (LPCTSTR) pSRInfo->m_szRegionOrderList_HMI);
		pUtl->RegionOrderLog(szMsg);
		szMsg.Format("Align Region Sorting Stat:%s", (LPCTSTR) pSRInfo->m_szRegionStateList_HMI);
		pUtl->RegionOrderLog(szMsg);
	}

	if( m_bCheckDieOrientation )
	{
		LONG lMoveX, lMoveY, i;
		BOOL bFindOK = FALSE;
		BOOL bGoodDie = FALSE;
//		DOUBLE dNormalValue = 0, dRotateValue = 0;
		for(i=0; i<=5; i++)
		{
			lMoveX = GetPrescanAlignPosnX() + GetPrescanPitchX()*2*i;
			lMoveY = GetPrescanAlignPosnY();
			if( IsWithinWaferLimit(lMoveX, lMoveY)==FALSE )
			{
				break;
			}
			XY_SafeMoveTo(lMoveX, lMoveY);
			Sleep(100);
			bFindOK = SearchAndAlignDie(TRUE, FALSE, TRUE);
			bGoodDie = 	(BOOL)(LONG)((*m_psmfSRam)["WaferTable"]["Die Orientation Good Die"]);
			bFindOK = bFindOK && bGoodDie;
			if( bFindOK )
				break;
		}

		if( bFindOK==FALSE )
		{
			for(i=0; i<=5; i++)
			{
				lMoveX = GetPrescanAlignPosnX() - GetPrescanPitchX()*2*i;
				lMoveY = GetPrescanAlignPosnY();
				if( IsWithinWaferLimit(lMoveX, lMoveY)==FALSE )
				{
					break;
				}
				XY_SafeMoveTo(lMoveX, lMoveY);
				Sleep(100);
				bFindOK = SearchAndAlignDie(TRUE, FALSE, TRUE);
				bGoodDie = 	(BOOL)(LONG)((*m_psmfSRam)["WaferTable"]["Die Orientation Good Die"]);
				bFindOK = bFindOK && bGoodDie;
				if( bFindOK )
					break;
			}
		}

		if (bFindOK)
		{
			CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
			if (pWaferPr->SearchAOTDie() != 0)
			{
				SetAlignmentStatus(FALSE);
				bFindOK = FALSE;
			}
/* 
			if( SearchAndAlignDie(TRUE, TRUE, FALSE) )
				dNormalValue = (DOUBLE)(*m_psmfSRam)["WaferTable"]["Die Orientation Score"];
			pUtl->SetSearchDieRoation(180);
			if( SearchAndAlignDie(TRUE, TRUE, FALSE) )
				dRotateValue = (DOUBLE)(*m_psmfSRam)["WaferTable"]["Die Orientation Score"];
			pUtl->SetSearchDieRoation(0);
			if( dNormalValue>dRotateValue )
				bFindOK = TRUE;
			else
				bFindOK = FALSE;
*/
		}
		else
		{
			SetAlignmentStatus(FALSE);
			CString szMsg = "Align Wafer";
			SetAlert_Msg_Red_Back(HMB_WT_CAN_NOT_FIND_ALIGNMENT_GOOD_DIE, szMsg);
			//SetAlert_Msg_Red_Back(HMB_WT_DIE_ORIENTATION_FAIL, szMsg);
		}

//		CString szMsg;
//		szMsg.Format("    WFT: Compare die orientation: normal %f, rotate %f", dNormalValue, dRotateValue);
//		SaveScanTimeEvent(szMsg);
		lMoveX = GetPrescanAlignPosnX();
		lMoveY = GetPrescanAlignPosnY();
		XY_SafeMoveTo(lMoveX, lMoveY);
	}

	if( IsWprWithAF() && m_bAFGridSampling )	//	427TX	4	after wafer alignment, sampling focus level
	{
	    CMS896AApp::m_bStopAlign = FALSE;
		m_lBackLightZStatus = 0;	//	4.24TX 4
		MoveES101BackLightZUpDn(TRUE);	// in prestart, if align ok, check current is safe, UP BL Z
		BOOL bReturn = TRUE;

		BOOL bOK = FALSE, bWft = FALSE;
		if( IsWT1UnderCamera() && !IsWT2InUse() )
		{
			bOK = TRUE;
			bWft = FALSE;
		}
		if( bOK )
		{
			MoveFocusToWafer(bWft);
		}

		AF_GridSampleFocusLevel();

		LONG lMoveX = GetPrescanAlignPosnX();
		LONG lMoveY = GetPrescanAlignPosnY();
		XY_SafeMoveTo(lMoveX, lMoveY);
		
		m_WaferMapWrapper.SetCurrentPosition(m_nPrescanAlignMapRow, m_nPrescanAlignMapCol);
		m_lBackLightZStatus = 2;	//	4.24TX 4
		MoveES101BackLightZUpDn(FALSE);	// in prestart, if align ok, check current is safe, UP BL Z
		if( bOK )
		{
			MoveFocusToWafer(bWft);
		}
	}

	m_bManualAddReferPoints	= FALSE;

	ObtainEmcoreBarColumnStart();

	return TRUE;
}

BOOL CWaferTable::KeepPrescanAlignDie()
{
	if( !GetKeepScnAlignDie() )	//Use/share the same SCN option in prescan mode
		return TRUE;

	LONG ulStartRow = m_ulAlignRow;
	LONG ulStartCol = m_ulAlignCol;
	LONG lRowOffset=0, lColOffset=0;
	ULONG ulDieState = 0;


	GetMapOffset(lRowOffset, lColOffset);	// kee align die L shape

	ulStartRow = ulStartRow + lRowOffset;
	ulStartCol = ulStartCol + lColOffset;

	//Check Row & Col must be >= 0
	if ( ulStartRow < 0 )
	{
		ulStartRow = 0;
	}

	if ( ulStartCol < 0 )
	{
		ulStartCol = 0;
	}

	LONG ulCurrRow	= ulStartRow;
	LONG ulCurrCol	= ulStartCol;

	//Mark die & set die state to unpick mode	
	m_WaferMapWrapper.MarkDie(ulStartRow, ulStartCol, TRUE);
	ulDieState = m_WaferMapWrapper.GetDieState(ulStartRow, ulStartCol);
	if( !IsDieUnpickAll(ulDieState) )
		ulDieState = WT_MAP_DIESTATE_UNPICK;
	m_WaferMapWrapper.SetDieState(ulStartRow, ulStartCol, ulDieState);

	//Move to DOWN
	INT i = 0;
	for (i=0; i<(m_lSCNIndex_Y-1); i++)
	{
		ulCurrRow = ulCurrRow + 1;

		//Mark die & set die state to unpick mode	
		m_WaferMapWrapper.MarkDie(ulCurrRow, ulCurrCol, TRUE);
		ulDieState = m_WaferMapWrapper.GetDieState(ulCurrRow, ulCurrCol);
		if( !IsDieUnpickAll(ulDieState) )
			ulDieState = WT_MAP_DIESTATE_UNPICK;
		m_WaferMapWrapper.SetDieState(ulCurrRow, ulCurrCol, ulDieState);
	}

	//Move to RHS
	for (i=0; i<(m_lSCNIndex_X-1); i++)
	{
		ulCurrCol = ulCurrCol + 1;

		//Mark die & set die state to unpick mode	
		m_WaferMapWrapper.MarkDie(ulCurrRow, ulCurrCol, TRUE);
		ulDieState = m_WaferMapWrapper.GetDieState(ulCurrRow, ulCurrCol);
		if( !IsDieUnpickAll(ulDieState) )
			ulDieState = WT_MAP_DIESTATE_UNPICK;
		m_WaferMapWrapper.SetDieState(ulCurrRow, ulCurrCol, ulDieState);
	}

	return TRUE;
}


VOID CWaferTable::SetPrescanOption(CONST BOOL bEnable)
{
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	m_bEnablePrescan = bEnable;

	SaveData();
	m_bIsPrescanning = IsPrescanEnable();

	if( bEnable==FALSE )
	{
		m_WaferMapWrapper.InitMap();	// clear wafer map if disable
		SetPsmEnable(FALSE);
	}
	else
	{
		SetAlignmentStatus(FALSE);
	}
}

BOOL CWaferTable::GetKeepScnAlignDie()
{
	if( IsScnLoaded() || IsPrescanEnable() )
		return m_bKeepSCNAlignDie;
	else
		return FALSE;
}

VOID CWaferTable::GetAsmMapHomeDie(ULONG &ulAlignRow, ULONG &ulAlignCol, BOOL bAlarm, BOOL bSecondPart)
{
	ULONG ulRow=0, ulCol=0;		//Klocwork
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bReturn = TRUE;

	switch(m_ulAlignOption)
	{
	case WT_ORGMAP_HOMEDIE:		//Home Die use original map position + offset (Get 1st map position)
		m_WaferMapWrapper.GetFirstDiePosition(ulRow, ulCol);
		break;

	case WT_SCNMAP_HOMEDIE:		//Home Die use SCN map position + offset	
		GetScnRefDie(&ulRow, &ulCol);
		break;

	case WT_MANUAL_HOMEDIE:
	default:
		if( pApp->IsUseHmiMap4HomeFPC() )
		{
			bReturn = ConvertHmiUserToAsm(m_lHomeDieMapRow, m_lHomeDieMapCol, ulRow, ulCol);
		}
		else
		{
			bReturn = ConvertOrgUserToAsm(m_lHomeDieMapRow, m_lHomeDieMapCol, ulRow, ulCol);
		}
		break;
	}

	if( IsMS90HalfSortMode() && (IsMS90Sorting2ndPart() || bSecondPart) )
	{
		if( m_lMS902ndHomeOffsetRow==0 && m_lMS902ndHomeOffsetCol==0 )
		{
			bReturn = ConvertOrgUserToAsm(m_lMS902ndHomeDieMapRow, m_lMS902ndHomeDieMapCol, ulRow, ulCol);
		}
		else
		{
			LONG lHomeRow = 0, lHomeCol = 0;
			ConvertAsmToOrgUser(ulRow, ulCol, lHomeRow, lHomeCol);
			lHomeRow = lHomeRow + m_lMS902ndHomeOffsetRow;
			lHomeCol = lHomeCol + m_lMS902ndHomeOffsetCol;
			bReturn = ConvertOrgUserToAsm(lHomeRow, lHomeCol, ulRow, ulCol);
		}
	}

	if( pUtl->GetPrescanDummyMap() && pApp->GetCustomerName()!="Epitop" && !(IsMS90Sorting2ndPart() || bSecondPart) )
	{
		m_WaferMapWrapper.GetFirstDiePosition(ulRow, ulCol);
	}
	else
	{
		if( bReturn==FALSE && bAlarm==TRUE )
		{
			HmiMessage_Red_Back("You set wrong manual home die position(out of range), please check!");
			ulRow = 0;
			ulCol = 0;
		}
	}

	if( IsBlkPickAlign()==FALSE && m_bFindAllRefer4Scan && m_bFindHomeDieFromMap && 
		m_ulCrossHomeDieRow!=0 && m_ulCrossHomeDieCol!=0 && pUtl->GetPrescanRegionMode()==FALSE )
	{
		ulRow = m_ulCrossHomeDieRow;
		ulCol = m_ulCrossHomeDieCol;
	}

	if( IsMS90HalfSortMode() && (m_bSearchHomeOption==WT_CORNER_SEARCH) )
	{
		switch (m_ulCornerSearchOption)
		{
		case WL_CORNER_BR:		//BR
			ulRow = GetMapValidMaxRow();
			ulCol = GetMapValidMaxCol();
			break;

		case WL_CORNER_BL:		//BL
			ulRow = GetMapValidMaxRow();
			ulCol = GetMapValidMinCol();
			break;

		case WL_CORNER_TR:		//TR
			ulRow = GetMapValidMinRow();
			ulCol = GetMapValidMaxCol();
			break;

		case WL_CORNER_TL:		//TL
		default:
			ulRow = GetMapValidMinRow();
			ulCol = GetMapValidMinCol();
			break;
		}
	}

	ulAlignRow = ulRow;
	ulAlignCol = ulCol;
}

VOID CWaferTable::GetMapOffset(LONG &lRowOffset, LONG &lColOffset)
{
	switch(m_ulAlignOption)
	{
	case WT_ORGMAP_HOMEDIE:		//Home Die use original map position + offset (Get 1st map position)
		lRowOffset = m_lOrgMapRowOffset;
		lColOffset = m_lOrgMapColOffset;
		break;

	case WT_SCNMAP_HOMEDIE:		//Home Die use SCN map position + offset	
		lRowOffset = m_lScnMapRowOffset;
		lColOffset = m_lScnMapColOffset;
		break;

	case WT_MANUAL_HOMEDIE:
	default:
		lRowOffset = 0;
		lColOffset = 0;
		break;
	}
}

VOID CWaferTable::GetMapAlignHomeDie(ULONG &ulHomeRow, ULONG &ulHomeCol, BOOL bAlarm)
{
	ULONG ulAlignRow, ulAlignCol;
	LONG lRowOffset=0, lColOffset=0;

	GetMapOffset(lRowOffset, lColOffset);	// to get map align die position
	GetAsmMapHomeDie(ulAlignRow, ulAlignCol, bAlarm);

	//Check Row & Col must be >= 0
	if ( ((LONG)ulAlignRow + lRowOffset) < 0 )
	{
		lRowOffset = 0;
	}

	if ( ((LONG)ulAlignCol + lColOffset) < 0 )
	{
		lColOffset = 0;
	}

	ulHomeRow = ulAlignRow + lRowOffset;
	ulHomeCol = ulAlignCol + lColOffset;
}

BOOL CWaferTable::ScnCheckDiePrResult(LONG lScnCheckRow, LONG lScnCheckCol, BOOL &bPromptMessage, CString &szText, CString &ListErrorDie, INT &ErrorCount)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	CString szMessage, szTitle, TempErrMessage;
	
	szTitle.LoadString(HMB_WT_CHECK_SCN_FUNC);

	REF_TYPE	stInfo;
	SRCH_TYPE	stSrchInfo;

	//Klocwork
	stInfo.bStatus	= FALSE;
	stInfo.bFullDie = FALSE;
	stInfo.bGoodDie = FALSE;
	
	Sleep(20);

	if ( m_bScnCheckByPR != TRUE )
	{
		GetEncoderValue();
		if (IsPrescanEnable())	//v3.55T1
		{
			WftCheckCurrentGoodDie();
		}
		szText.Format("Current map Position is %ld,%ld", lScnCheckRow, lScnCheckCol);
		bPromptMessage = TRUE;
		return TRUE;
	}

	LONG lRefDieLrn = (*m_psmfSRam)["WaferPr"]["RefDie"]["Count"];
	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	lDiePitchX_X = labs(lDiePitchX_X);
	lDiePitchY_Y = labs(lDiePitchY_Y);

	if ( m_bScnCheckIsRefDie == FALSE )
	{
		stSrchInfo.bShowPRStatus = FALSE;
		stSrchInfo.bNormalDie = TRUE;
		stSrchInfo.lRefDieNo = 1;
		stSrchInfo.bDisableBackupAlign = TRUE;

		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}
	else
	{
		if( m_bScnCheckNormalOnRefer )
		{
			stSrchInfo.bShowPRStatus = FALSE;
			stSrchInfo.bNormalDie = TRUE;
			stSrchInfo.lRefDieNo = 1;
			stSrchInfo.bDisableBackupAlign = TRUE;

			stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			if (stInfo.bStatus == FALSE)
			{
				szMessage.LoadString(HMB_WT_PR_SEARCH_ERROR);
				HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				return FALSE;
			}
			else
			{
				if (stInfo.bFullDie == TRUE)
				{
					szText.Format("NORMAL Die is found on (%ld,%ld) REFER die position", lScnCheckRow, lScnCheckCol);
					HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					return FALSE;
				}
			}
		}
		if ( lRefDieLrn == 0 )
		{
			szMessage.LoadString(HMB_WT_NO_REF_LEARNT);
			HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			return FALSE;
		}

		if( m_lScnCheckRefDieNo>lRefDieLrn )
		{
			szMessage = "Scn Check reference die is not learnt!";
			HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			return FALSE;
		}

		LONG lDieNo, lRefDieStart, lRefDieStop;
		lRefDieStart = 1;
		lRefDieStop  = lRefDieLrn;
		if( m_lScnCheckRefDieNo>0 )
		{
			lRefDieStart = lRefDieStop  = m_lScnCheckRefDieNo;
		}

		for ( lDieNo=lRefDieStart; lDieNo<=lRefDieStop; lDieNo++ )
		{
			stSrchInfo.bShowPRStatus = FALSE;
			stSrchInfo.bNormalDie = FALSE;
			stSrchInfo.lRefDieNo = lDieNo;
			stSrchInfo.bDisableBackupAlign = TRUE;

			stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if ( stInfo.bFullDie == TRUE )
			{
				break;
			}
		}
	}

	if (stInfo.bStatus == FALSE)
	{
		szMessage.LoadString(HMB_WT_PR_SEARCH_ERROR);
		HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	if (stInfo.bFullDie == FALSE)
	{
		//szText.Format("Empty die on %ld %ld position", lScnCheckRow, lScnCheckCol);
		//SetStatusMessage(szText);
		
		if ( ErrorCount % 3 == 2 )	//Matthew 10312018
		{
			TempErrMessage.Format("(%ld,%ld)\n", lScnCheckRow, lScnCheckCol);
		}
		else
		{
			TempErrMessage.Format("(%ld,%ld)", lScnCheckRow, lScnCheckCol);
		}
		ListErrorDie = ListErrorDie + _T(" ") + TempErrMessage;
		ErrorCount++;
	}
	else
	{
		LONG lOffsetX = stInfo.lX;
		LONG lOffsetY = stInfo.lY;
		if ( (m_lScnCheckXTol > 0) && ( labs(lOffsetX) > ( labs(lDiePitchX_X) * m_lScnCheckXTol * 0.01)) )
		{
			DOUBLE dOffsetX = ConvertMotorStepToUnit(lOffsetX);
			szMessage.Format("Die pitch X is failed on %ld %ld position, offset (um) %.2f", lScnCheckRow, lScnCheckCol, dOffsetX);
			HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

			return FALSE;
		}

		if ( (m_lScnCheckYTol > 0) && ( labs(lOffsetY) > ( labs(lDiePitchY_Y) * m_lScnCheckYTol * 0.01)) )
		{
			DOUBLE dOffsetY = ConvertMotorStepToUnit(lOffsetY);
			szMessage.Format("Die pitch Y is failed on %ld %ld position, offset (um) %.2f", lScnCheckRow, lScnCheckCol, dOffsetY);
			HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

			return FALSE;
		}

		GetEncoderValue();
		LONG lX=0, lY=0;	//Klocwork
		lX = GetCurrX() + lOffsetX;
		lY = GetCurrY() + lOffsetY;
		XY_SafeMoveTo(lX, lY);
		Sleep(20);

		szText.Format("Die is found on %ld %ld position", lScnCheckRow, lScnCheckCol);
		bPromptMessage = FALSE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( m_bScnCheckIsRefDie == FALSE && (pApp->GetCustomerName().Find("Electech3E") != -1) )	//v4.44A6
	{
		bPromptMessage = !bPromptMessage;
	}
	return TRUE;
}

VOID CWaferTable::SetPrescanBlkFuncEnable(BOOL bBlkFunc)
{
	m_bBlkFuncEnable = bBlkFunc;
}

//v4.28
BOOL CWaferTable::GetES101PrescanSortPosn(LONG &lCalcX, LONG &lCalcY, LONG &lCalcT)
{
	LONG lPrescanX, lPrescanY;
	LONG lStoredX = 0, lStoredY = 0;
	LONG lCurrRow, lCurrCol;
	CString szLog;
	CString szTemp;


	lCalcT = 0;
	lCurrRow = GetCurrentRow();
	lCurrCol = GetCurrentCol();

	m_bPrescanPosnOK = GetPrescanWftPosn(lCurrRow, lCurrCol, lPrescanX, lPrescanY);
	if( m_bPrescanPosnOK == FALSE )
	{
		//If Prescan dot not have this die pos, try to use nearby dices to calcualate die position
		m_bPrescanPosnOK = GetDieValidPrescanPosn(lCurrRow, lCurrCol, 2, lPrescanX, lPrescanY, FALSE);
	}

	if( !IsScanPosnOK() )
	{
		CString szErr;
		LONG lHmiRow = 0, lHmiCol = 0;
		ConvertAsmToHmiUser(lCurrRow, lCurrCol, lHmiRow, lHmiCol);

		szErr.Format("    WFT: GetES101 PrescanSortPosn fail 2 - (%ld, %ld)  x=%ld  y=%ld)",  lHmiRow, lHmiCol, lPrescanX, lPrescanY);
		SaveScanTimeEvent(szErr);				//v4.30T1
		return FALSE;
	}

	m_bMapPositionOK = IsScanPosnOK();
	
	lPrescanX	= lPrescanX + m_lEjtOffsetX;
	lPrescanY	= lPrescanY + m_lEjtOffsetY;

	m_lGetScnWftPosX = lPrescanX;
	m_lGetScnWftPosY = lPrescanY;


	T_Sync();
	GetEncoderValue();
	LONG lHmiRow = 0, lHmiCol = 0, lMapRow = 0, lMapCol = 0;
	ConvertAsmToHmiUser(lCurrRow, lCurrCol, lHmiRow, lHmiCol);
	ConvertAsmToOrgUser(lCurrRow, lCurrCol, lMapRow, lMapCol);
	szTemp.Format("; %ld,%ld (%ld,%ld)", lHmiRow, lHmiCol, lMapRow, lMapCol);
	UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lCurrRow, lCurrCol) - m_WaferMapWrapper.GetGradeOffset();
	szLog.Format("Get MapE %ld,%ld,T%ld(%ld), %ld,%ld, grd %d", lStoredX, lStoredY, lCalcT, GetCurrT(), lCurrRow, lCurrCol, ucGrade);
	szLog = szLog + szTemp;


	m_ulGetMveMapRow = lCurrRow;
	m_ulGetMveMapCol = lCurrCol;
	m_lGetScnWftPosX = lStoredX;
	m_lGetScnWftPosY = lStoredY;

	m_bRotate = FALSE;						//Always FALSE for ES101 NGPick sequence
	lCalcX = lPrescanX;
	lCalcY = lPrescanY;
	lCalcT = GetGlobalT() - GetCurrT();

	return IsMapPosnOK();
}


BOOL CWaferTable::GetPrescanSortPosn(LONG &lCalcX, LONG &lCalcY, LONG &lCalcT, BOOL bDisableDEB)
{
	LONG	lPrescanX = 0, lPrescanY = 0;
	LONG	lMapPosnX = 0, lMapPosnY = 0;
	LONG lCurrRow, lCurrCol;
	CString szLog;

	lCalcT = 0;
	lCurrRow = GetCurrentRow();
	lCurrCol = GetCurrentCol();

	m_bThisDieNotLFed = labs(lCurrRow-GetLastDieRow())>=(LONG)m_ulAdvSampleJumpDistance ||
						labs(lCurrCol-GetLastDieCol())>=(LONG)m_ulAdvSampleJumpDistance;

	if( IsBlkFunc2Enable() && IsPrescanEnable()==FALSE )	//Block2
	{
		m_bMapPositionOK = FALSE;
		m_bPrescanPosnOK = FALSE;
		m_bThisDieNeedVerify = FALSE;
		m_bThisDieNotLFed = labs(lCurrRow-GetLastDieRow())>1 && labs(lCurrCol-GetLastDieCol())>1;
		return FALSE;
	}

	m_bPrescanPosnOK = GetPrescanWftPosn(lCurrRow, lCurrCol, lPrescanX, lPrescanY);
	if( m_bPrescanPosnOK==FALSE )
	{
		m_bPrescanPosnOK = PredictGetDieValidPosn(lCurrRow, lCurrCol, 2, lPrescanX, lPrescanY, FALSE);
	}

	BOOL bPickLeft = FALSE;
	bPickLeft = (BOOL)(LONG)((*m_psmfSRam)["WaferMap"]["NgPick"]["PickLeft"]);
	if( m_bAOINgPickPartDie && bPickLeft && GetNewPickCount()!=0 )	// when get posn, use prescan value as the map would be updated when picked
	{
		m_bMapPositionOK = GetPrescanWftPosn(lCurrRow, lCurrCol, lMapPosnX, lMapPosnY);
	}
	else
	{
		m_bMapPositionOK = GetMapPhyPosn(lCurrRow, lCurrCol, lMapPosnX, lMapPosnY);
	}
	LONG lMapValuePos = 0;
	if( m_bMapPositionOK==FALSE && (IsPrescanEnable() || IsScnLoaded()) )
	{
		if( !bDisableDEB )
			CMSLogFileUtility::Instance()->WT_GetIdxLog("Get Near Die for sort position");
		m_bMapPositionOK = PredictGetDieValidPosn(lCurrRow, lCurrCol, 2, lMapPosnX, lMapPosnY, TRUE);	// same to pitch check
		lMapValuePos = 1;
		if( m_bMapPositionOK==FALSE && IsScanPosnOK() )
		{
			m_bMapPositionOK = TRUE;
			lMapPosnX = lPrescanX;
			lMapPosnY = lPrescanY;
			lMapValuePos = 2;
		}
	}

	T_Sync();
	
	//GetEncoderValue();
	LONG lHmiRow = 0, lHmiCol = 0, lMapRow = 0, lMapCol = 0;
	ConvertAsmToHmiUser(lCurrRow, lCurrCol, lHmiRow, lHmiCol);
	ConvertAsmToOrgUser(lCurrRow, lCurrCol, lMapRow, lMapCol);
	CString szTemp;
	szTemp.Format("; curr %ld,%ld (%ld,%ld)LF(%d,%d)", lHmiRow, lHmiCol, lMapRow, lMapCol, m_stLFDie.lWM_Y, m_stLFDie.lWM_X);
	UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lCurrRow, lCurrCol)-m_WaferMapWrapper.GetGradeOffset();
	if( IsMapPosnOK() )
	{
		szLog.Format("Get MapP %ld,%ld,T%ld(%ld), %ld,%ld, grd %d MapValuePos,%d", lMapPosnX, lMapPosnY, lCalcT, GetCurrT(), lCurrRow, lCurrCol, ucGrade, lMapValuePos);
	}
	else
	{
		LONG	lDiePitchX_X = 0, lDiePitchX_Y = 0, lDiePitchY_X = 0, lDiePitchY_Y = 0;
		LONG	lDiff_X = 0, lDiff_Y = 0;
		lDiePitchX_X = labs(GetDiePitchX_X()); 
		lDiePitchX_Y = GetDiePitchX_Y();
		lDiePitchY_X = GetDiePitchY_X();
		lDiePitchY_Y = labs(GetDiePitchY_Y());
	
		//andrewng //2020-0618	//Use DEFAULT die pitch for BURNIN mode
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (IsBurnIn() && !pApp->m_bBurnInGrabImage)	
		{
			lDiePitchX_X = NVC_WAF_BURNIN_DIE_PITCH_XX; 
			lDiePitchX_Y = NVC_WAF_BURNIN_DIE_PITCH_XY;
			lDiePitchY_X = NVC_WAF_BURNIN_DIE_PITCH_YX;
			lDiePitchY_Y = NVC_WAF_BURNIN_DIE_PITCH_YY;
		}

		lDiff_X	= lCurrCol - GetLastDieCol();
		lDiff_Y	= lCurrRow - GetLastDieRow();

		DOUBLE dDiffDegree = 0;
		LONG lCurrentAngle = (*m_psmfSRam)["WaferTable"]["Current"]["Theta"];

		lMapPosnX = GetLastDieEncX();
		lMapPosnY = GetLastDieEncY();
		szLog.Format("Get NEAR: die %ld,%ld grd %d from last(%ld,%ld) wft(%ld,%ld)", 
			lCurrRow, lCurrCol, ucGrade, GetLastDieRow(), GetLastDieCol(), lMapPosnX, lMapPosnY);

		LONG lTheta = 0;
		dDiffDegree = -(DOUBLE)m_lThetaMotorDirection*((DOUBLE)(GetGlobalT() - lCurrentAngle) * m_dThetaRes);
		RotateWaferTheta(&lMapPosnX, &lMapPosnY, &lTheta, dDiffDegree);

		if( IsPrescanEnable() )
		{
			lCalcX = lMapPosnX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lCalcY = lMapPosnY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
		}
		else
		{
			//v4.38T4
			LONG lBackupX = lMapPosnX;
			LONG lBackupY = lMapPosnY;

			//Calculate current die position from nearest die
			if( IsEnablePNP()==FALSE && PredictGetDieValidPosn(lCurrRow, lCurrCol, 4, lMapPosnX, lMapPosnY, TRUE) )
			{
				lCalcX = lMapPosnX;
				lCalcY = lMapPosnY;
			}
			else
			{
				// modified inside GetDieValidPrescanPosn even return FALSE;
				lMapPosnX = lBackupX;
				lMapPosnY = lBackupY;
				lCalcX = lMapPosnX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
				lCalcY = lMapPosnY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			}
		}
		lCalcT = lTheta;
		if( !bDisableDEB )
		{
			CString szCalc;
			szCalc.Format("Get Calc %ld,%ld, T(%ld)", lCalcX, lCalcY, lCalcT);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szCalc);
		}
		lMapPosnX = lCalcX;
		lMapPosnY = lCalcY;
	}

	m_ulGetMveMapRow = lCurrRow;
	m_ulGetMveMapCol = lCurrCol;
	if( IsScanPosnOK() )
	{
		m_lGetScnWftPosX = lPrescanX;
		m_lGetScnWftPosY = lPrescanY;
	}
	else
	{
		m_lGetScnWftPosX = lMapPosnX;
		m_lGetScnWftPosY = lMapPosnY;
	}

	szLog = szLog + szTemp;
	if( !bDisableDEB )
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szLog);

	if( IsMapPosnOK() )
	{
		if( !bDisableDEB && DEB_IsUseable() && IsScanPosnOK() )	//v4.02T4	//Disable DEB option to prevent double access
		{
			CString szAtcMsg;
			BOOL	bGetAtc = TRUE;
			INT nStatus = 0;
			LONG lGetX=0, lGetY=0;
			bGetAtc = DEB_GetDiePos(lCurrRow, lCurrCol, lGetX, lGetY, nStatus);
			szAtcMsg.Format("GET %ld\t%ld\t%ld\t%ld\t%d", lCurrRow, lCurrCol, lGetX, lGetY, nStatus);
			if( bGetAtc!=TRUE )
			{
				szAtcMsg = szAtcMsg + "FAIL";
				HmiMessage_Red_Back(szAtcMsg);
				SetErrorMessage(szAtcMsg);
			}
			else
			{
				if( m_bJustAlign==FALSE )
				{
					lMapPosnX = lGetX;
					lMapPosnY = lGetY;
				}
			}

			m_lGetAtcWftPosX		= lMapPosnX;
			m_lGetAtcWftPosY		= lMapPosnY;

			szLog.Format("Get DEBP %ld,%ld, %d USED %ld,%ld", lGetX, lGetY, bGetAtc, lMapPosnX, lMapPosnY);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szLog);
		}

		if( !bDisableDEB && IsAdvRegionOffset() )
		{
			GetRegionDiePosition(lCurrRow, lCurrCol, m_lGetScnWftPosX, m_lGetScnWftPosY, lMapPosnX, lMapPosnY);
			m_lGetRgnWftPosX		= lMapPosnX;
			m_lGetRgnWftPosY		= lMapPosnY;
		}

		m_bRotate = IsDiePRRotate();

		// Check if curr die position has been rotated by last LF result
		if (m_stLastLFDie.bFullDie && m_stLastLFDie.bRotate)
		{
			m_bRotate = TRUE;
		}

		lCalcX = lMapPosnX;
		lCalcY = lMapPosnY;
		lCalcT = GetGlobalT() - GetCurrT();
	}

	return IsMapPosnOK();
}

BOOL CWaferTable::CheckDieMapWaferPosition(ULONG ulRow, ULONG ulCol, LONG lEncX, LONG lEncY)
{
	//Call setcurrent pos twice because HMI need time to update
	m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);

	XY_SafeMoveTo(lEncX, lEncY);

	ChangeCameraToWafer();

	WftCheckCurrentGoodDie();

	return TRUE;
}

VOID CWaferTable::SetAllPrescanDone(BOOL bState)
{
	if (bState)
	{
		m_evAllPrescanDone.SetEvent();
	}
	else
	{
		m_evAllPrescanDone.ResetEvent();
	}
}

BOOL CWaferTable::WaitAllPrescanDone(INT nTimeout)
{
	CSingleLock slLock(&m_evAllPrescanDone);
	return slLock.Lock(nTimeout);
}

VOID CWaferTable::SetES101PreloadFrameDone(BOOL bState)
{
	if (bState)
	{
		m_evES101WldPreloadDone.SetEvent();
	}
	else
	{
		m_evES101WldPreloadDone.ResetEvent();
	}
}
//	4.24TX 4
BOOL CWaferTable::WaitES101PreloadFrameDone(INT nTimeout)
{
	CSingleLock slLock(&m_evES101WldPreloadDone);
	return slLock.Lock(nTimeout);
}

LONG CWaferTable::CheckPrescanEnable(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	if( IsPrescanEnable() )
		bReturn = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckOsramPrescanEnable(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_OSRAM && pApp->GetProductLine()!="Germany")
	{
		if( IsLayerPicking()==FALSE )
		{
			if( CheckOperatorPassWord(PRESCAN_PASSWORD_SUPER_USER)==FALSE )
				bReturn = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}



LONG CWaferTable::EnablePrescan(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable;

	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	if( IsEnablePNP() && bEnable )
	{
		m_bEnablePrescan = FALSE;
		HmiMessage("Please disable pick and place firstly!");
	}
	else
	{
		SetPrescanOption(bEnable);
		if( bEnable )
		{
			m_bScnPrescanIfBonded = FALSE;
		}
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::PrescanGotoHome(IPC_CServiceMessage& svMsg)
{
	ULONG ulRow, ulCol;
	LONG encX, encY, encT;
	LONG lUserRow = 0, lUserCol = 0;
	CString msg;
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsAlignedWafer()==FALSE )
	{
		SetAlert(IDS_WT_MAP_NOT_ALIGN);
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsBLInUse() )
	{
		if( MoveBackLightToSafeLevel()==FALSE )	//	4.24TX 4
		{
			BOOL bReturn=TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	LONG lAlignRow, lAlignCol, lAlignX, lAlignY;
	pUtl->GetAlignPosition(lAlignRow, lAlignCol, lAlignX, lAlignY);

	encT = pUtl->GetPrescanGlobalTheta();
	if( m_bScnModeWaferAlignTwice )
	{
		if( HmiMessage("Do you want to new home position?", "Prescan", glHMI_MBX_YESNO)==glHMI_YES )
		{
			pUtl->GetRealignPosition(lAlignRow, lAlignCol, lAlignX, lAlignY);
			encT = pUtl->GetRealignGlobalTheta();
		}
	}

	ulRow = lAlignRow;
	ulCol = lAlignCol;
	encX = lAlignX;
	encY = lAlignY;

	m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
	Sleep(300);
	m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
	Sleep(600);

	ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);

	SetJoystickOn(FALSE);
	GetEncoderValue();
	if( abs(GetCurrT()-encT)>=5 )
	{
		T_MoveTo(encT, SFM_NOWAIT);
		T_Sync();
	}
	if (XY_SafeMoveTo(encX, encY))
	{
	}
	else
	{
		msg.Format("Row %lu, Col %lu\nTarget Position (%ld,%ld) Out of Wafer Limit", ulRow, ulCol, encX, encY);
		HmiMessage(msg);
	}
	SetJoystickOn(TRUE);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CheckAOIOnlyMachine(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::PrescanES101Check(IPC_CServiceMessage &svMsg)
{
	if( IsPrescanEnded() && (IsESMachine() ) )	//v4.37T10
	{
		ES101ToAlignPosition();
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::PrescanGotoTargetDie(IPC_CServiceMessage& svMsg)
{
	UINT i;
	ULONG ulRow = 0, ulCol = 0, ulGoType; //, ulPosition;
	BOOL bPrdVerify = FALSE;
	BOOL bManVerify = FALSE;
	BOOL bUsePosOffset = FALSE;
	LONG encX = 0, encY = 0, encT = 0;
	LONG lUserRow = 0, lUserCol = 0;
	LONG lRow, lCol, lEncX, lEncY;
	CString szLogMsg, szBoxMsg, szMapDie, szDetail;
	BOOL bReturn=TRUE;
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

//	if( IsAlignedWafer()==FALSE )
//	{
//		SetAlert(IDS_WT_MAP_NOT_ALIGN);
//		bReturn = TRUE;
//		svMsg.InitMessage(sizeof(BOOL), &bReturn);
//		return 1;
//	}

	if (CMS896AApp::m_bMS100Plus9InchOption && IsEJTAtUnloadPosn())		//v4.47T12
	{
		SetAlert_Red_Yellow(IDS_WT_BL_NOT_IN_SAFE_POS);
		SetErrorMessage("AutoAlign: WT at UNLOAD position (MS109)");
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( m_bVerifyMapWaferFail && IsAdvRegionOffset() )
	{
		ULONG ulRow = 0, ulCol = 0;
		if( ConvertOrgUserToAsm(m_lVerifyMapWaferRow, m_lVerifyMapWaferCol, ulRow, ulCol) )
		{
			m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
			m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
			LONG lScanX = 0, lScanY = 0;
			if( GetPrescanWftPosn(ulRow, ulCol, lScanX, lScanY) )
			{
				LONG encX = 0, encY = 0;
				if( GetRegionDiePosition(ulRow, ulCol, lScanX, lScanY, encX, encY) )
				{
					m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
					Sleep(500);
					XY_SafeMoveTo(encX, encY);
					CString szMsg = "PCI Pitch error, verification fail, please check on map.";
					HmiMessage_Red_Back(szMsg, "Auto Sort");
				}
			}
			Sleep(200);
			m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
		}
		m_bVerifyMapWaferFail = FALSE;
	}	// end to display verify fail position

	m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);
	ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);

	if( IsPrescanEnable()==FALSE && IsScnLoaded()==FALSE )
	{
		if( IsBlkFunc2Enable() )	//Block2
		{
			SetJoystickOn(FALSE);
			LONG lX=0, lY=0, lT=0;		//Klocwork
			if( m_pBlkFunc2->Blk2GetNextDiePhyPos(ulRow, ulCol, 2000, 2000, lX, lY) )
			{
				lT = GetGlobalT();
				T_MoveTo(lT);
				XY_SafeMoveTo(lX, lY);
			}
			SetJoystickOn(TRUE);
		}
		else
		{
			HmiMessage("No scanned position available in this wafer!");
		}
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsPrescanEnable() && IsPrescanEnded()==FALSE )
	{
		if( pUtl->GetPrescanRegionMode()==FALSE )
		{
			HmiMessage("Prescan not complete yet!");
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	szMapDie.Format("Row %ld, Col %ld", lUserRow, lUserCol);

	INT nDebStatus = 0;
	BOOL ret = FALSE;
	BOOL bDebUpdate = FALSE;
	BOOL bRegUpdate = FALSE;
	LONG lScanX = 0, lScanY = 0;
	LONG lStoredX = 0, lStoredY = 0;


	svMsg.GetMsg(sizeof(ulGoType), &ulGoType);
	if( ulGoType>=2 )
	{
		if( IsAutoSampleSort()==FALSE )
		{
			ulGoType = 0;
		}
		if( IsAutoRescanOnly() )
		{
			ulGoType = 2;
		}
	}
	if( ulGoType==3 )
	{
		ulGoType = 2;
		bPrdVerify = TRUE;
	}
	if( ulGoType==4 )
	{
		ulGoType = 2;
		bManVerify = TRUE;
	}

	encT = pUtl->GetRealignGlobalTheta();
	switch(ulGoType)
	{
	case 0:
		if( IsPrescanEnable() )
		{
			ret = GetPrescanWftPosn(ulRow, ulCol, encX, encY);
			if( ret!=TRUE && pUtl->GetPrescanRegionMode() )
			{
				for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
				{
					if( pUtl->GetReRefPosition(i, lRow, lCol, lEncX, lEncY) )
					{
						if( lRow==ulRow && lCol==ulCol )
						{
							encX = lEncX;
							encY = lEncY;
							ret = TRUE;
							break;
						}
					}
				}
			}
			if( ret!=TRUE )
			{
				szDetail = "Do Not Have Prescan Information";
			}
		}
		else if( IsScnLoaded() )
		{
			ret = GetPrescanWftPosn(ulRow, ulCol, encX, encY);
			if( ret!=TRUE )
			{
				szDetail = "Do Not Have scanner Information";
			}
		}
		break;

	case 1:
		ret = GetMapPhyPosn(ulRow, ulCol, encX, encY);
		if( ret!=TRUE && pUtl->GetPrescanRegionMode() )
		{
			for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
			{
				if( pUtl->GetReRefPosition(i, lRow, lCol, lEncX, lEncY) )
				{
					if( lRow==ulRow && lCol==ulCol )
					{
						encX = lEncX;
						encY = lEncY;
						ret = TRUE;
						break;
					}
				}
			}
		}
		if( ret!=TRUE )
		{
			szDetail = "Do Not Have Physical Information";
		}
		break;

	case 2:
		if( IsAutoRescanOnly() )
		{
			ret = GetPrescanRunPosn(ulRow, ulCol, encX, encY);
			if( ret!=TRUE )
			{
				szDetail = "Do Not Have Prescan Information";
			}
			break;
		}

		if( GetMapPhyPosn(ulRow, ulCol, encX, encY) )
		{
			if( IsAdvRegionOffset() )
			{
				bRegUpdate = TRUE;
				if( GetPrescanWftPosn(ulRow, ulCol, lScanX, lScanY) )
				{
					ret = GetRegionDiePosition(ulRow, ulCol, lScanX, lScanY, encX, encY);
				}
				else
				{
					szDetail = "Do Not Have prescan Information";
				}
			}
			else if( DEB_IsUseable() )
			{
				ret = DEB_GetDiePos(ulRow, ulCol, encX, encY, nDebStatus);

				bDebUpdate = TRUE;

				GetMapPhyPosn(ulRow, ulCol, lStoredX, lStoredY);
				szLogMsg.Format("GET %lu\t%lu\t%ld\t%ld\t%d", ulRow, ulCol, encX, encY, nDebStatus);
				if( ret!=TRUE )
				{
					szLogMsg = szLogMsg + "FAIL";
					SetErrorMessage(szLogMsg);
				}

				if( ret!=TRUE )
				{
					szDetail = "Do Not Have Predicated Information";
				}
			}
		}
		else
		{
			szDetail = "is a wrong position!";
		}
		break;
	}

	BOOL bJSOff = FALSE;
	if (ret)
	{
		if( IsWithinWaferLimit(encX, encY, 1, bUsePosOffset) )
		{
			bJSOff = TRUE;
			SetJoystickOn(FALSE);
			GetEncoderValue();
			if( abs(GetCurrT()-encT)>=5 )
				T_MoveTo(encT, SFM_NOWAIT);

			LONG lPrdX = encX;
			LONG lPrdY = encY;
			if( XY_SafeMoveTo(encX, encY, bUsePosOffset)==FALSE )
			{
				szDetail = "Wafer table motion error";
				bDebUpdate = FALSE;
				bRegUpdate = FALSE;
				ret = FALSE;
			}
			T_Sync();

			//if( bDebUpdate || bRegUpdate )
			//{
			//	Sleep(100);
			//	BOOL bMove = GetNewPickCount()>5;
			//	if( IsAdvRegionOffset()==FALSE )
			//		bMove = FALSE;
			//	if( WftMoveSearchDie(encX, encY, bMove) )
			//		nDebStatus = MS_Align;
			//	else
			//		nDebStatus = MS_Empty;
			//}
			if( bDebUpdate )
			{
				DEB_UpdateDiePos(ulRow, ulCol, encX, encY, nDebStatus);
			}
			if( bRegUpdate )
			{
				m_bGetAvgFOVOffset = FALSE;
				if( bPrdVerify )
				{
					BOOL bXCheck = CheckPitchX(encX - lPrdX);
					BOOL bYCheck = CheckPitchY(encY - lPrdY);
					if( CheckMapAndFindWaferMatch(ulRow, ulCol, encX, encY, !bXCheck, !bYCheck)==FALSE )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_SKIP_PREDICTED);
						HmiMessage("Auto recovery fail, please use M. Recover to continue.", "Prescan");
						m_bVerifyMapWaferFail = FALSE;
						m_lVerifyMapWaferRow	= -10000;
						m_lVerifyMapWaferCol	= -10000;
					}
				}
				
				if( bManVerify )
				{
					SetJoystickOn(TRUE, IsWT2InUse());
					LONG lHMI = HmiMessageEx("Please move PR center to target die.", "Prescan", glHMI_MBX_OKCANCEL);
					SetJoystickOn(FALSE, IsWT2InUse());
					Sleep(100);
					if( lHMI==glHMI_OK )
					{
						GetEncoderValue();
						encX = GetCurrX();
						encY = GetCurrY();
						WftMoveSearchDie(encX, encY, TRUE);
					}
				}	// manual verify and recover

				LONG lNewOffsetX = encX-lScanX;
				LONG lNewOffsetY = encY-lScanY;
				if( m_bGetAvgFOVOffset )
				{
					lNewOffsetX = m_lAvgFovOffsetX;
					lNewOffsetY = m_lAvgFovOffsetY;
				}
				m_bGetAvgFOVOffset = FALSE;
				UpdateRegionDieOffset(ulRow, ulCol, lNewOffsetX, lNewOffsetY, bManVerify);

				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if( pApp->GetCustomerName()==CTM_SEMITEK && GetNewPickCount()<=5 )
				{
					LONG lDiePitchX_X = labs(GetDiePitchX_X()); 
					LONG lDiePitchY_Y = labs(GetDiePitchY_Y());

					CString szMessage, szTitle;
					szTitle = "Prescan Go";
					if ( (m_lScnCheckXTol > 0) && 
						( labs(lNewOffsetX) > ( labs(lDiePitchX_X) * m_lScnCheckXTol/100)) )
					{
						szMessage = "Die pitch X is failed on go check position.\nPlease align and scan wafer again.";
						HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
						SetAlignmentStatus(FALSE);
					}
					if ( (m_lScnCheckYTol > 0) && 
						( labs(lNewOffsetY) > ( labs(lDiePitchY_Y) * m_lScnCheckYTol/100)) )
					{
						szMessage = "Die pitch Y is failed on go check position.\nPlease align and scan wafer again.";
						HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
						SetAlignmentStatus(FALSE);
					}
				}
			}

			if( m_bCheckDiePrIn4X )
			{
				IPC_CServiceMessage stMsg;
				BOOL bTo4X = TRUE;
				stMsg.InitMessage(sizeof(BOOL), &bTo4X);
				int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ToggleNml4XCmd", stMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID, stMsg);
						break;
					}
					else
					{
						Sleep(10);
					}
				}
				bTo4X = FALSE;
				stMsg.InitMessage(sizeof(BOOL), &bTo4X);
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ToggleNml4XCmd", stMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID, stMsg);
						break;
					}
					else
					{
						Sleep(10);
					}
				}
			}
		}
		else
		{
			ret = FALSE;
			szDetail = "Target Position Out of Wafer Limit";
		}
	}

	if( ret )
	{
		if( m_ulPrescanGoCheckLimit>0 && m_ulPrescanGoCheckCount<m_ulPrescanGoCheckLimit )
		{
			BOOL bDoneBefore = FALSE;
			LONG lCheckTotal = min((LONG)m_aUIGoCheckRow.GetSize(), (LONG)m_aUIGoCheckCol.GetSize());
			for(LONG i=0; i<lCheckTotal; i++)
			{
				if( labs(ulRow-m_aUIGoCheckRow.GetAt(i))<=SCAN_GO_CHECK_NEAR_LIMIT && 
					labs(ulCol-m_aUIGoCheckCol.GetAt(i))<=SCAN_GO_CHECK_NEAR_LIMIT )
				{
					bDoneBefore = TRUE;
					HmiMessage("This die is near(<=5) to checked point!\nPlease select another point!", "Prescan");
					break;
				}
			}

			if( bDoneBefore==FALSE )
			{
				m_aUIGoCheckRow.Add(ulRow);
				m_aUIGoCheckCol.Add(ulCol);
				m_ulPrescanGoCheckCount++;	// not repeat, increase
			}
		}	// handle the go check points

		if( ( m_ulPitchAlarmGoCheckLimit>0 && m_ulPitchAlarmGoCheckCount<m_ulPitchAlarmGoCheckLimit) )
		{
			BOOL bDoneBefore = FALSE;
			LONG lCheckTotal = min((LONG)m_aUIGoCheckRow.GetSize(), (LONG)m_aUIGoCheckCol.GetSize());
			for(LONG i=0; i<lCheckTotal; i++)
			{
				if( labs(ulRow-m_aUIGoCheckRow.GetAt(i))<=0 && 
					labs(ulCol-m_aUIGoCheckCol.GetAt(i))<=0 )
				{
					bDoneBefore = TRUE;
					HmiMessage("This die is checked points already!\nPlease select another point!", "Prescan");
					break;
				}
			}

			if( bDoneBefore==FALSE )
			{
				m_aUIGoCheckRow.Add(ulRow);
				m_aUIGoCheckCol.Add(ulCol);
				m_ulPitchAlarmGoCheckCount++;
			}
		}	// handle the pitch go check points

		if( IsES201()==FALSE && IsES101()==FALSE && m_bEnableAlignWaferImageLog )		//v4.37T10
		{
			(*m_psmfSRam)["MS896A"]["WaferTableX"] = encX;
			(*m_psmfSRam)["MS896A"]["WaferTableY"] = encY;
			(*m_psmfSRam)["MS896A"]["WaferMapRow"] = ulRow;
			(*m_psmfSRam)["MS896A"]["WaferMapCol"] = ulCol;
			GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_UGO);	// user check scan result by go button
		}
	}
	else
	{
		szBoxMsg = szMapDie + "\n" + szDetail;
		HmiMessage(szBoxMsg, "Prompt");
	}

	if( bJSOff )
	{
		SetJoystickOn(TRUE);
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::WftSetDummyScanGrade(IPC_CServiceMessage &svMsg)
{
	UCHAR ucGrade = 101;
	svMsg.GetMsg(sizeof(UCHAR), &ucGrade);
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	m_ucDummyPrescanPNPGrade = ucGrade;

	BOOL	bReturn = CheckDummyBinGradeHasNoDie(m_ucDummyPrescanPNPGrade);
	CString szMsg;
	if( bReturn )
	{
		szMsg.Format("Dummy scan grade %d has die on bin frame, please clear it firstly!", ucGrade);
		HmiMessage(szMsg);
		if( pUtl->GetPrescanDummyMap() )
		{
			m_ulPrescanMethod = SCAN_METHOD_MAP_WHOLE_WAFER;
		}
	}
	else
	{
		szMsg.Format("Dummy scan grade %d", ucGrade);
	}
	SetErrorMessage(szMsg);
	SaveData();

	svMsg.InitMessage(sizeof(UCHAR), &bReturn);

	return bReturn;
}


LONG CWaferTable::WftSetPsmLoadAction(IPC_CServiceMessage &svMsg)
{
	UCHAR ucAction = 0;
	svMsg.GetMsg(sizeof(UCHAR), &ucAction);
	m_ucPSMLoadAction = ucAction;

	CString szMsg;
	switch( m_ucPSMLoadAction )
	{
	case 0:
		szMsg = "PRESCAN: Load PSM (if have) and check nothing";
		break;
	case 1:
		szMsg = "PRESCAN: NOT Load PSM (even have) and check nothing";
		break;
	case 2:
		szMsg = "PRESCAN: If PSM exist and can be loaded, alarm and stop running";
		break;
	case 3:
		szMsg = "PRESCAN: If PSM can not be loaded, alarm and stop running";
		break;
	default:
		szMsg = "PRESCAN: no this option can be set";
		break;
	}
	HmiMessage(szMsg, "Prescan Options");
	SetErrorMessage(szMsg);

	SaveData();

	BOOL	bReturn = FALSE;
	svMsg.InitMessage(sizeof(UCHAR), &bReturn);

	return bReturn;
}

UCHAR CWaferTable::GetPsmLoadAction()
{
	return m_ucPSMLoadAction;
}

LONG CWaferTable::WftSetPrescanPrMethod(IPC_CServiceMessage &svMsg)
{
	ULONG lScanMethod;
	svMsg.GetMsg(sizeof(ULONG), &lScanMethod);

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( lScanMethod==SCAN_METHOD_MAP_REGION_WAFER )
	{
		if( !IsBlkPickAlign() && m_bFindAllRefer4Scan==FALSE && m_bSingleHomeRegionScan==FALSE )
		{
			CString szMsg;
			szMsg.Format("This mode not available yet!");
			HmiMessage(szMsg);
			lScanMethod = SCAN_METHOD_MAP_WHOLE_WAFER;
		}
	}

	BOOL bOldDummyMode = pUtl->GetPrescanDummyMap();

	SetErrorMessage("change prescan method");
	m_ulPrescanMethod = lScanMethod;
	SaveData();

	if( bOldDummyMode==FALSE && pUtl->GetPrescanDummyMap() )
	{
		CString szMsg;
		BOOL	bReturn = CheckDummyBinGradeHasNoDie(m_ucDummyPrescanPNPGrade);
		if( bReturn )
		{
			szMsg.Format("Dummy scan grade %d has die on bin frame!", m_ucDummyPrescanPNPGrade);
			HmiMessage(szMsg);
			m_ulPrescanMethod = SCAN_METHOD_MAP_WHOLE_WAFER;
			SaveData();
		}
		else
		{
			szMsg.Format("Dummy scan grade %d !", m_ucDummyPrescanPNPGrade);
		}
		SetErrorMessage(szMsg);
	}

	m_lAutoWaferCenterX		= GetWaferCenterX();
	m_lAutoWaferCenterY		= GetWaferCenterY();
	m_lAutoWaferDiameter	= GetWaferDiameter();
	m_lAutoWaferWidth		= GetWaferDiameter();
	m_lAutoWaferHeight		= GetWaferDiameter();
	m_bAutoWaferLimitOnce	= FALSE;
	m_bRegionPrescan		= pUtl->GetPrescanRegionMode();

	return 1;
}

VOID CWaferTable::RegPrescanVarFunc()
{
	try
	{
		RegVariable(_T("WT_lWaferNegLimitY"),		&m_lYNegLimit);
		RegVariable(_T("WT_lMS902ndHomeDieMapRow"),	&m_lMS902ndHomeDieMapRow);	
		RegVariable(_T("WT_lMS902ndHomeDieMapCol"),	&m_lMS902ndHomeDieMapCol);	
		RegVariable(_T("WT_lMS902ndHomeOffsetRow"),	&m_lMS902ndHomeOffsetRow);	
		RegVariable(_T("WT_lMS902ndHomeOffsetCol"),	&m_lMS902ndHomeOffsetCol);	
		RegVariable(_T("WT_bCheckMapHomeNullBin"),	&m_bCheckMapHomeNullBin);

		RegVariable(_T("WT_bMS90HalfSortMode"),		&m_bMS90HalfSortMode);
		RegVariable(_T("WT_bAlignNeedRescan"),		&m_bAlignNeedRescan);
		RegVariable(_T("WT_bSorraSortMode"),		&m_bSoraaSortMode);
		RegVariable(_T("WT_bDualWayTipWafer"),		&m_bDualWayTipWafer);
		RegVariable(_T("WT_bMS90DisplayAndSelect"),	&m_bMS90DisplayAndSelect);
		RegVariable(_T("WT_bMS90DisplayWholeOnly"),	&m_bMS90DisplayWholeOnly);

		RegVariable(_T("WT_bAlarmTwice"),			&m_bAlarmTwice);
		RegVariable(_T("WT_bEnableAlarmLog"),		&m_bEnableAlarmLog);
		RegVariable(_T("WT_bACF_AlarmLogPath"),		&m_bACF_AlarmLogPath);
		RegVariable(_T("WT_szAlarmLogPath"),		&m_szAlarmLogPath);

		RegVariable(_T("WT_bToUseCharDie"),			&m_bHmiToUseCharDie);
		RegVariable(_T("WT_bEnableIM"),				&m_bEnableIM);
		RegVariable(_T("WT_bEnableMapDMFile"),		&m_bEnableMapDMFile);
		RegVariable(_T("WT_bScanPickedDieResort"),	&m_bScanPickedDieResort);
		RegVariable(_T("WT_bEnablePrescan"),		&m_bEnablePrescan);
		RegVariable(_T("WT_lPrescanFinishedAction"),&m_lPrescanFinishedAction);
		RegVariable(_T("WT_lScanYieldSpinIndex"),	&m_lScanYieldSpinIndex);
		RegVariable(_T("WT_ulPrescanPrMethod"),		&m_ulPrescanMethod);
		RegVariable(_T("WT_ulPrescanXYStripMode"),	&m_ulPrescanXYStripMode);
		RegVariable(_T("WT_ucScanWalkTour"),		&m_ucScanWalkTour);
		RegVariable(_T("WT_ucScanMapTreatment"),	&m_ucScanMapTreatment);
		RegVariable(_T("WT_bRunTimeDetectEdge"),	&m_bRunTimeDetectEdge);
		RegVariable(_T("WT_lADEEmptyFrameLimit"),	&m_lADEEmptyFrameLimit);
		RegVariable(_T("WT_bAutoSrchWaferLimit"),	&m_bAutoSearchWaferLimit);
		RegVariable(_T("WT_bFixDummyMapCenter"),	&m_bFixDummyMapCenter);
		RegVariable(_T("WT_ulPrescanRefPoints"),	&m_ulPrescanRefPoints);
		RegVariable(_T("WT_ulRealignPickMinDie"),	&m_ulAdvStage1StartCount);
		RegVariable(_T("WT_bWftDoSamplingByBT"),	&m_bWftDoSamplingByBT);
		RegVariable(_T("WT_ulRealignPickMaxDie"),	&m_ulAdvStage1SampleSpan);
		RegVariable(_T("WT_ulNewPickCounter"),		&m_ulNewPickCounter);
		RegVariable(_T("WT_ulGoodDieTotal"),		&m_ulGoodDieTotal);
		RegVariable(_T("WT_ulTotalDieCounter"),		&m_ulTotalDieCounter);
		RegVariable(_T("WT_dWaferYield"),			&m_dWaferYield);

		RegVariable(_T("WT_ulAdvStage2StartCount"),	&m_ulAdvStage2StartCount);
		RegVariable(_T("WT_ulAdvStage2SampleStep"),	&m_ulAdvStage2SampleSpan);
		RegVariable(_T("WT_ulAdvStage3StartCount"),	&m_ulAdvStage3StartCount);
		RegVariable(_T("WT_ulAdvStage3SampleStep"),	&m_ulAdvStage3SampleSpan);
		RegVariable(_T("WT_ulRescanSampleSpan"),	&m_ulRescanSampleSpan);
		RegVariable(_T("WT_ulRescanSampleStart"),	&m_ulRescanStartCount);

		RegVariable(_T("WT_lPrescanPrDelay"),		&m_lPrescanPrDelay);
		RegVariable(_T("WT_bPrescanAutoPitch"),		&m_bPrescanAutoDiePitch);
		RegVariable(_T("WT_lScnAlignMethod"),		&m_lScnAlignMethod);

		RegVariable(_T("WT_bScnAlignDieAsRefer"),	&m_bScnAlignDieAsRefer);
		RegVariable(_T("WT_bScnAlignReferDie"),		&m_bScnAlignUseRefer);
		RegVariable(_T("WT_lTotalSCNAlignCount"),	&m_lScnAlignTotalPoints);
		RegVariable(_T("WT_lScnAlignRefDieNo"),		&m_lScnAlignReferNo);
		RegVariable(_T("WT_bScnPrescanIfBonded"),	&m_bScnPrescanIfBonded);

		RegVariable(_T("WT_lRescanPitchCheckLimit"),	&m_lRescanPELimit);
		RegVariable(_T("WT_lLFPitchToleranceX"),	&m_lLFPitchToleranceX);
		RegVariable(_T("WT_lLFPitchToleranceY"),	&m_lLFPitchToleranceY);
		RegVariable(_T("WT_ulPitchAlarmCount"),		&m_ulPitchAlarmCount);
		RegVariable(_T("WT_bPitchAlarmOnce"),		&m_bPitchAlarmOnce);
		RegVariable(_T("WT_ulPitchAlarmLimit"),		&m_ulPitchAlarmLimit);
		RegVariable(_T("WT_bEnableAdvDieStep"),		&m_bAdvRegionOffset);
		RegVariable(_T("WT_bAutoRescanWafer"),		&m_bAutoRescanWafer);
		RegVariable(_T("WT_bAutoRescanAllDice"),	&m_bAutoRescanAllDice);
		RegVariable(_T("WT_bAutoSampleAfterPE"),	&m_bAutoSampleAfterPE);
		RegVariable(_T("WT_bAutoSampleAfterPR"),	&m_bAutoSampleAfterPR);
		RegVariable(_T("WT_ulPdcRegionSizeRow"),	&m_ulPdcRegionSizeRow);
		RegVariable(_T("WT_ulPdcRegionSizeCol"),	&m_ulPdcRegionSizeCol);
		RegVariable(_T("WT_lLFPitchErrorLimit"),	&m_lLFPitchErrorLimit);
		RegVariable(_T("WT_ulAdvOffsetUpdMinTime"),	&m_ulAdvOffsetUpdMinTime);
		RegVariable(_T("WT_ulAdvOffsetExpireTime"),	&m_ulAdvOffsetExpireTime);
		RegVariable(_T("WT_ulAdvSampleMinOffset"),	&m_ulAdvOffsetUpdMinVary);
		RegVariable(_T("WT_dDummyPrescanDiameter"),	&m_dPrescanAreaDiameter);
		RegVariable(_T("WT_dBarBridgeLength"),		&m_dBarBridgeLength);
		RegVariable(_T("WT_bDummyQuarterWafer"),	&m_bDummyQuarterWafer);
		RegVariable(_T("WT_bScanAreaConfig"),		&m_bScanAreaConfig);
		RegVariable(_T("WT_bScanWithWaferLimit"),	&m_bScanWithWaferLimit);
		RegVariable(_T("WT_bScanToHalfMap"),		&m_bScanToHalfMap);
		RegVariable(_T("WT_bSortMultiToOne"),		&m_bSortMultiToOne);
		RegVariable(_T("WT_bChangeGrade4PrNG"),			&m_bChangeGrade4PrNG);
		RegVariable(_T("WT_bIsPrescanning"),		&m_bPrescanningMode);
		RegVariable(_T("WT_dCurrTime"),				&m_dCurrTime);
		RegVariable(_T("WT_dScanTime"),				&m_dScanTime);
		RegVariable(_T("WT_dScanMoveTime"),			&m_dScanMoveTime);
		RegVariable(_T("WT_bScanMapUseBackupExt"),	&m_bScanMapUseBackupExt);
		RegVariable(_T("WT_bScanMapAddTimeStamp"),	&m_bScanMapAddTimeStamp);
		RegVariable(_T("WT_bCheckNormalOnRefer"),	&m_bScnCheckNormalOnRefer);
		RegVariable(_T("WT_lScnCheckAlarmLimit"),	&m_lScnCheckAlarmLimit);
		RegVariable(_T("WT_ulPrescanGoCheckLimit"),	&m_ulPrescanGoCheckLimit);
		RegVariable(_T("WT_bAutoGoAroundPE"),		&m_bAutoGoAroundPE);
		RegVariable(_T("WT_bVerifyMapWaferFail"),	&m_bVerifyMapWaferFail);
		RegVariable(_T("WT_lVerifyMapWaferRow"),	&m_lVerifyMapWaferRow);
		RegVariable(_T("WT_lVerifyMapWaferCol"),	&m_lVerifyMapWaferCol);
		RegVariable(_T("WT_bCheckDiePrIn4X"),		&m_bCheckDiePrIn4X);
		RegVariable(_T("WT_bChangeRegionGrade"),	&m_bChangeRegionGrade);

		RegVariable(_T("WT_lEmptyGradeMinCount"),	&m_lEmptyGradeMinCount);
		RegVariable(_T("WT_nPrescanIndexStepRow"),	&m_nPrescanIndexStepRow);
		RegVariable(_T("WT_nPrescanIndexStepCol"),	&m_nPrescanIndexStepCol);
		RegVariable(_T("WT_ulMapIndexLoop"),		&m_ulMapIndexLoop);
		RegVariable(_T("WT_dPrescanLFSizeRow"),		&m_dPrescanLFSizeY);
		RegVariable(_T("WT_dPrescanLFSizeCol"),		&m_dPrescanLFSizeX);
		RegVariable(_T("WT_ucPsnMapShapeType"),		&m_ucPrescanMapShapeType);
		RegVariable(_T("WT_bHmiDefectPick"),		&m_bHmiDefectPick);
		RegVariable(_T("WT_bSortDefectDie"),		&m_bToPickDefectDie);
		RegVariable(_T("WT_ucDefectiveDieGrade"),	&m_ucDefectiveDieGrade);
		RegVariable(_T("WT_bPscnBlkPickAlign"),		&m_bPrescanBlkPickAlign);
		RegVariable(_T("WT_bWaferEndUncheckRefer"),	&m_bWaferEndUncheckRefer);
		RegVariable(_T("WT_lBlk4ReferCheckScore"),	&m_lBlk4ReferCheckScore);
		RegVariable(_T("WT_dPrescanGoodPassScore"),	&m_dPrescanGoodPassScore);
		RegVariable(_T("WT_dPrescanFailPassScore"),	&m_dPrescanFailPassScore);
		RegVariable(_T("WT_ulMapBaseNum"),			&m_ulMapBaseNum);
		RegVariable(_T("WT_bAbleUploadScanFile"),	&m_bOfflineUploadScanFile);
		RegVariable(_T("WT_bPrescanTwiceEnable"),	&m_bPrescanTwiceEnable);
		RegVariable(_T("WT_bPrescanDebugEnable"),	&m_bPrescanDebugEnable);
		RegVariable(_T("WT_bEnableAtcLog"),			&m_bEnableAtcLog);
		RegVariable(_T("WT_bRegionPrescan"),		&m_bRegionPrescan);
		RegVariable(_T("WT_bPrescanCheckDefect"),	&m_bPrescanCheckDefect);
		RegVariable(_T("WT_bPrescanCheckBadCut"),	&m_bPrescanCheckBadCut);
		RegVariable(_T("WT_bACF_SaveImages"),		&m_bACF_SaveImages);
		RegVariable(_T("WT_bACF_MapBackupPath"),	&m_bACF_MapBackupPath);

		RegVariable(_T("WT_bNgPickBadCut"),			&m_bNgPickBadCut);
		RegVariable(_T("WT_bNgPickDefect"),			&m_bNgPickDefect);
		RegVariable(_T("WT_bNgPickEdgeDie"),		&m_bNgPickEdgeDie);

		RegVariable(_T("WT_bSingleHomeRegionScan"),	&m_bSingleHomeRegionScan);
		RegVariable(_T("WT_bManualRegionScanSort"),	&m_bManualRegionScanSort);
		RegVariable(_T("WT_bScnCheckDieAsRefDie"),	&m_bScnCheckDieAsRefDie);
		RegVariable(_T("WT_bPrescanNml3AtHome"),	&m_bPrescanNml3AtHomeDie);
		RegVariable(_T("WT_bFindAllRefer4Scan"),	&m_bFindAllRefer4Scan);
		RegVariable(_T("WT_lAllReferSpiralLoop"),	&m_lAllReferSpiralLoop);
		RegVariable(_T("WT_lMapPrescanAlignWafer"),	&m_lMapPrescanAlignWafer);
		RegVariable(_T("WT_bMapDummyPrescanAlign"),	&m_bMapDummyPrescanAlign);
		RegVariable(_T("WT_bMapDummyScanCornerCheck"),	&m_bMapDummyScanCornerCheck);
		RegVariable(_T("WT_bEnable6InchWT"),		&m_bEnable6InchWaferTable);
		RegVariable(_T("WT_dPrescanRotateDegree"),	&m_dPrescanRotateDegree);
		RegVariable(_T("WT_lReferPitchRow"),		&m_lReferPitchRow);
		RegVariable(_T("WT_lReferPitchCol"),		&m_lReferPitchCol);
		RegVariable(_T("WT_bRescanLogPrImage"),		&m_bRescanLogPrImage);
		RegVariable(_T("WT_bPrescanLogNgImage"),	&m_bPrescanLogNgImage);
		RegVariable(_T("WT_bCaptureScanMapImage"),	&m_bCaptureScanMapImage);
		RegVariable(_T("WT_szSaveMapImagePath"),	&m_szSaveMapImagePath);
		RegVariable(_T("WT_szScanNgImageLogPath"),	&m_szScanNgImageLogPath);

		// ES101: yealy generate picture
		RegVariable(_T("WT_lTabletPixelX"),			&m_lTabletPixelX);
		RegVariable(_T("WT_lTabletPixelY"),			&m_lTabletPixelY);
		RegVariable(_T("WT_dTabletDimInch"),		&m_dTabletDimInch);
		RegVariable(_T("WT_dTabletDimInch_X"),		&m_dTabletDimInch_X);	//new
		RegVariable(_T("WT_dTabletDimInch_Y"),		&m_dTabletDimInch_Y);	//new
		RegVariable(_T("WT_lPngPixelX"),			&m_lPngPixelX);
		RegVariable(_T("WT_lPngPixelY"),			&m_lPngPixelY);
		RegVariable(_T("WT_dTabletXPPI"),			&m_dTabletXPPI);		//new
		RegVariable(_T("WT_dTabletYPPI"),			&m_dTabletYPPI);		//new
		RegVariable(_T("WT_ulBoundaryDieColour"),	&m_ulBoundaryDieColour);	// Boundary die
		RegVariable(_T("WT_ulNGDieColourY"),		&m_ulNGDieColourY);		// NG die (Y)	
		RegVariable(_T("WT_ulNGDieColourE"),		&m_ulNGDieColourE);		// NG die (Y)	
		RegVariable(_T("WT_ulNGDieColourN"),		&m_ulNGDieColourN);		// NG die (N)	
		RegVariable(_T("WT_ulNGDieColourX"),		&m_ulNGDieColourX);		// NG die (X)	
		RegVariable(_T("WT_lPngDieExtraEdgeY"),		&m_lPngDieExtraEdgeY);
		RegVariable(_T("WT_lPngDieExtraEdgeE"),		&m_lPngDieExtraEdgeE);
		RegVariable(_T("WT_lPngDieExtraEdgeN"),		&m_lPngDieExtraEdgeN);
		RegVariable(_T("WT_lPngDieExtraEdgeX"),		&m_lPngDieExtraEdgeX);

		RegVariable(_T("WT_bPngEnableGridFunction"),&m_bPngEnableGrid);
		RegVariable(_T("WT_lPngGridRow"),			&m_lPngGridRow);
		RegVariable(_T("WT_lPngGridCol"),			&m_lPngGridCol);
		RegVariable(_T("WT_lPngGridThick"),			&m_lPngGridThick);

		RegVariable(_T("WT_bGridColour1On"),		&m_bGridColourOn1);
		RegVariable(_T("WT_bGridColour2On"),		&m_bGridColourOn2);
		RegVariable(_T("WT_bGridColour3On"),		&m_bGridColourOn3);
		RegVariable(_T("WT_bGridColour4On"),		&m_bGridColourOn4);
		RegVariable(_T("WT_bGridColour5On"),		&m_bGridColourOn5);
		RegVariable(_T("WT_ulGridColour1"),			&m_ulGridColourArray1);
		RegVariable(_T("WT_ulGridColour2"),			&m_ulGridColourArray2);
		RegVariable(_T("WT_ulGridColour3"),			&m_ulGridColourArray3);
		RegVariable(_T("WT_ulGridColour4"),			&m_ulGridColourArray4);
		RegVariable(_T("WT_ulGridColour5"),			&m_ulGridColourArray5);

		RegVariable(_T("WT_bAFGridSampling"),		&m_bAFGridSampling);
		RegVariable(_T("WT_bPitchCheckMapWafer"),	&m_bPitchCheckMapWafer);
		RegVariable(_T("WT_bCheckPSMLoaded"),		&m_bCheckPSMLoaded);
		RegVariable(_T("WT_bCheckPSMExist"),		&m_bCheckPSMExist);
		RegVariable(_T("WT_ucPSMLoadAction"),		&m_ucPSMLoadAction);
		RegVariable(_T("WT_bPsmEnableState"),		&m_bPsmEnableState);
		RegVariable(_T("WT_bSmallMapSortRpt"),		&m_bSmallMapSortRpt);
		RegVariable(_T("WT_bGenRptOfSMS"),			&m_bGenRptOfSMS);
		RegVariable(_T("WT_bES100EnablePsm"),		&m_bDisablePsm);
		RegVariable(_T("WT_ucDummyPrescanPNPGrade"),&m_ucDummyPrescanPNPGrade);
		RegVariable(_T("WT_bES100DisableSCN"),		&m_bES100DisableSCN);
		RegVariable(_T("WT_bCheckDieOrientation"),	&m_bCheckDieOrientation);
		RegVariable(_T("WT_bEnableCheckDieOrientation"),	&m_bEnableCheckDieOrientation);
		RegVariable(_T("WT_bES100ByMapIndex"),		&m_bES100ByMapIndex);
		RegVariable(_T("WT_szOemCustomerName"),		&m_szOemCustomerName);
		RegVariable(_T("WT_szOemProductName"),		&m_szOemProductName);
		RegVariable(_T("WT_szOemTestSystem"),		&m_szOemTestSystem);
		RegVariable(_T("WT_szPsmMapMachineNo"),		&m_szPsmMapMachineNo);
		RegVariable(_T("WT_szDieState"),			&m_szDieState);
		RegVariable(_T("WT_szNextLocation"),		&m_szNextLocation);
		RegVariable(_T("WT_szScanYieldWaferName"),	&m_szScanYieldWaferName);

		RegVariable(_T("WT_szDEB_LibVersion"),		&m_szDEB_LibVersion);
		RegVariable(_T("WT_bMapAdaptAlgorithm"),	&m_bAdaptPredictAlgorithm);
		RegVariable(_T("WT_bEnableAdaptPredict"),	&m_bEnableAdaptPredict);
		RegVariable(_T("WT_bDebKeyDieSampleOld"),	&m_bDebKeyDieSampleOld);
		RegVariable(_T("WT_bGiveUpDebKeyDie"),		&m_bGiveUpDebKeyDie);
		RegVariable(_T("WT_ucDEBKeyDieGrade"),		&m_ucDEBKeyDieGrade);
		RegVariable(_T("WT_lPredictMethod"),		&m_lPredictMethod);

		RegVariable(_T("WT_bRegionBlkOrder"),		&m_bRegionBlkPickOrder);
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		RegVariable(_T("WT_lRegionPickMinLimit"),	&pSRInfo->m_lRegionPickMinLimit);
		RegVariable(_T("WT_ulCurrentSortingRegion"),&pSRInfo->m_ulCurrentSortingRegion_HMI);
		RegVariable(_T("WT_szCurrentRegionState"),	&pSRInfo->m_szCurrentRegionState_HMI);
		RegVariable(_T("WT_szRegionOrderList"),		&pSRInfo->m_szRegionOrderList_HMI);
		RegVariable(_T("WT_szRegionStateList"),		&pSRInfo->m_szRegionStateList_HMI);
		RegVariable(_T("WT_lRegionPickIndex"),		&m_lRegionPickIndex);
		RegVariable(_T("WT_bErrorChooseGoFPC"),		&m_bErrorChooseGoFPC);
		RegVariable(_T("WT_lFindMapEdgeDieLimit"),	&m_lFindMapEdgeDieLimit);
		RegVariable(_T("WT_ulPrescanInspectType"),	&m_ulInspectGoType);
		RegVariable(_T("WT_ulPrescanGoToType"),		&m_ulPrescanGoToType);
		RegVariable(_T("WT_lMa3HomeRow"),			&m_lMa3AHomeRow);
		RegVariable(_T("WT_lMa3HomeCol"),			&m_lMa3AHomeCol);
		RegVariable(_T("WT_lMa3RHomeRow"),			&m_lMa3RHomeRow);
		RegVariable(_T("WT_lMa3RHomeCol"),			&m_lMa3RHomeCol);
		RegVariable(_T("WT_lMa3MapDirX"),			&m_lMa3MapDirX);
		RegVariable(_T("WT_lMa3MapDirY"),			&m_lMa3MapDirY);
		RegVariable(_T("WT_bSetDummyMa3HomeDie"),		&m_bSetDummyMa3HomeDie);
		RegVariable(_T("WT_szScanAoiMapFormat"),		&m_szScanAoiMapFormat);
		RegVariable(_T("WT_szPrescanBackupMapPath"),	&m_szPrescanBackupMapPath);
		RegVariable(_T("WT_bPrescanBackupMap"),			&m_bPrescanBackupMap);
		RegVariable(_T("WT_szOutputScanSummaryPath"),	&m_szOutputScanSummaryPath);
		RegVariable(_T("WT_lOutputScanSummary"),		&m_lOutputScanSummary);
		RegVariable(_T("WT_szScanMA1FilePath"),			&m_szScanMA1FilePath);
		RegVariable(_T("WT_dMA1ScanYieldLimit"),		&m_dMA1ScanYieldLimit);
		RegVariable(_T("HMI_OperationLogEnable"),		&m_bHmiOperationLogOption);
		RegVariable(_T("HMI_OperationLogPath"),			&m_szHmiOperationLogPath);
		RegVariable(_T("WT_ulAlertType"),				&m_ulAlertType);

		RegVariable(_T("WT_ucCornerPatternCheckMode"),	&m_ucCornerPatternCheckMode);
		RegVariable(_T("WT_ucCornerReferPrID"),			&m_ucCornerReferPrID);
		RegVariable(_T("WT_bCornerPatternCheckAll"),	&m_bCornerPatternCheckAll);
		RegVariable(_T("WT_lReferToCornerDistX"),		&m_lReferToCornerDistX);
		RegVariable(_T("WT_lReferToCornerDistY"),		&m_lReferToCornerDistY);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MS90SetWaferLimitY"),		&CWaferTable::MS90SetWaferLimitY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadAllMapNewReferDie"),	&CWaferTable::LoadAllMapNewReferDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GoToAllSCNDiePos"),			&CWaferTable::GoToAllSCNDiePos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GoSampleCheckScnPoints"),	&CWaferTable::GoSampleCheckScnPoints);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadAllScnCheckPosn"),		&CWaferTable::LoadAllScnCheckPosn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PassWLCurrentSlotNo"),		&CWaferTable::PassWLCurrentSlotNo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SpinScanYieldDisplay"),		&CWaferTable::SpinScanYieldDisplay);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UploadSelectedWaferFile"),	&CWaferTable::UploadSelectedWaferFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearScanYieldResult"),		&CWaferTable::ClearScanYieldResult);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowScanYieldResult"),		&CWaferTable::ShowScanYieldResult);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetDummyPrescanArea"),		&CWaferTable::SetDummyPrescanArea);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetPrescanMapIndexStep"),	&CWaferTable::SetPrescanMapIndexStep);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InspectSelectType"),		&CWaferTable::InspectSelectType);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InspectGotoTarget"),		&CWaferTable::InspectGotoTarget);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckES100Machine"),		&CWaferTable::CheckES100Machine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadES100Screen"),			&CWaferTable::LoadES100Screen);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckSemitekPasswordWithWeek"),		&CWaferTable::CheckSemitekPasswordWithWeek);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckSemitekPassword"),		&CWaferTable::CheckSemitekPassword);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckCreePassword"),		&CWaferTable::CheckCreePassword);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckPassword201277"),		&CWaferTable::CheckPassword201277);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckRebelPassword"),		&CWaferTable::CheckRebelPassword);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckRebelAndStop"),		&CWaferTable::CheckRebelAndStop);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnablePrescan"),			&CWaferTable::EnablePrescan);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetAlarmLogPath"),			&CWaferTable::GetAlarmLogPath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmAlarmLogSettings"),	&CWaferTable::ConfirmAlarmLogSettings);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrintScreenCmd"),			&CWaferTable::PrintScreenCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckPrescanEnable"),		&CWaferTable::CheckPrescanEnable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ForceToRescanWafer"),		&CWaferTable::ForceToRescanWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckOsramPrescanEnable"),	&CWaferTable::CheckOsramPrescanEnable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrescanGotoHome"),			&CWaferTable::PrescanGotoHome);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrescanES101Check"),		&CWaferTable::PrescanES101Check);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckAOIOnlyMachine"),		&CWaferTable::CheckAOIOnlyMachine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GotoNextToSortDie"),		&CWaferTable::GotoNextToSortDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrescanGotoTargetDie"),		&CWaferTable::PrescanGotoTargetDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectCheckCenterDie"),		&CWaferTable::SelectCheckCenterDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestAlertRedYellow"),		&CWaferTable::TestAlertRedYellow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WftSetPrescanPrMethod"),	&CWaferTable::WftSetPrescanPrMethod);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WftSetDummyScanGrade"),		&CWaferTable::WftSetDummyScanGrade);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WftSetPsmLoadAction"),		&CWaferTable::WftSetPsmLoadAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualAddRefPoint"),		&CWaferTable::ManualAddPrescanRefPoints);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AdjustReferPoints"),		&CWaferTable::AdjustReferPoints);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DEB_SetLogEnable"),			&CWaferTable::DEB_SetLogEnable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SampleKeyDie"),				&CWaferTable::DEB_SampleKeyDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnablePrescanDebug"),		&CWaferTable::EnablePrescanDebug);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnablePrescanTwice"),		&CWaferTable::EnablePrescanTwice);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RePrescanWafer"),			&CWaferTable::RePrescanWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeRegionSortOrder"),	&CWaferTable::ChangeRegionSortOrder);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateScnOption"),			&CWaferTable::UpdateScnOption);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckScnPrescanOption"),	&CWaferTable::CheckScnPrescanOption);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Enable6InchWaferTable"),	&CWaferTable::Enable6InchWaferTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckSuperUserPassword"),	&CWaferTable::CheckSuperUserPassword);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckSemitekSUPW"),			&CWaferTable::CheckSemitekSUPW);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckBWMachinePassword"),	&CWaferTable::CheckBWMachinePassword);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetMA1FilePath"),			&CWaferTable::GetMA1FilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InputSaveMapImagePath"),	&CWaferTable::InputSaveMapImagePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetSaveMapImagePath"),		&CWaferTable::GetSaveMapImagePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InputLogPrNgImagePath"),	&CWaferTable::InputLogPrNgImagePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetLogPrNgImagePath"),		&CWaferTable::GetLogPrNgImagePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMA3Format"),			&CWaferTable::SelectMA3Format);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WaferTableDoAdvSampling"),	&CWaferTable::WaferTableDoAdvSampling);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ESMoveTablesToUnload"),		&CWaferTable::ESMoveTablesToUnload);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWT1JoystickLimitCmd"),	&CWaferTable::SetWT1JoystickLimitCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWT2JoystickLimitCmd"),	&CWaferTable::SetWT2JoystickLimitCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetHmiOperationLogPath"),	&CWaferTable::SetHmiOperationLogPath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetHmiOperationLogPath"),	&CWaferTable::GetHmiOperationLogPath);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_SetContourTable"),		&CWaferTable::WT_SetContourTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Contour_Move"),			&CWaferTable::WT_Contour_Move);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowContourEdge"),			&CWaferTable::ShowContourEdge);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetYSlowMoveTime"),			&CWaferTable::GetYSlowMoveTime);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenMapPhyPosition"),		&CWaferTable::GenMapPhyPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeleteMapPsmScnCmd"),		&CWaferTable::DeleteMapPsmScnCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TickMultiSortToOnCmd"),		&CWaferTable::TickMultiSortToOnCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND("RotateWaferCmd",				&CWaferTable::RotateWaferCmd);
		//	SCN
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateScnAlignInfo"),		&CWaferTable::UpdateScnAlignInfo);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

void HDInfoCBFunctionFail(void)
{
	gslThisMachineHasRptFile = 0;
}

void HDInfoCBFunctionPass(void)
{
	gslThisMachineHasRptFile = 0;
}

VOID CWaferTable::InitPrescanVariables()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	m_bDisableWaferMapGradeSelect	= FALSE;
	m_bSoraaSortMode		= FALSE;
	m_bMS90HalfSortMode		= FALSE;
	m_bMS90DisplayAndSelect	= FALSE;
	m_bMS90DisplayWholeOnly	= FALSE;
	m_bFirstInAutoCycle = TRUE;	//	init variables
	m_bScnModeWaferAlignTwice	= FALSE;
	m_stPrescanStartTime	= CTime::GetCurrentTime();	//	init
	m_lReadyToSampleOnWft	= 0;
	m_lReadyToCleanEjPinOnWft	= 0;
	m_bReadyToRealignOnWft	= FALSE;
	m_bErrorChooseGoFPC		= FALSE;
	m_bPrescanTwiceEnable	= FALSE;
	m_bIsPrescanning		= FALSE;
	m_bIntoNgPickCycle		= FALSE;
	m_bPrescanningMode		= FALSE;
	m_bEnablePrescan		= FALSE;
	m_bPrescanStarted		= FALSE;
	m_ulAdvSampleJumpDistance	= 10;
	m_ulTotalProbeDie		= 0;
	m_lBuildShortPathInScanning = 0;	// init variable
	m_bFirstMapIsOldMap		= TRUE;

	m_lPrescanFinishedAction= PRESCAN_FINISHED_DO_NOTHING;
	m_ulPrescanMethod		= SCAN_METHOD_MAP_WHOLE_WAFER;
	m_ucScanMapTreatment	= 0;
	m_ucScanWalkTour		= WT_SCAN_WALK_HOME_HORI;
	m_ucRunScanWalkTour		= WT_SCAN_WALK_HOME_HORI;
	m_lBlk4ReferCheckScore	= 0;
	m_bOfflineUploadScanFile= FALSE;
	m_bToUploadScanMapFile	= FALSE;
	m_dPrescanGoodPassScore	= 0.0;
	m_dPrescanFailPassScore	= 0.0;
	m_dCurrentScore			= 0.0;
	m_szCurrentGrade		= "N";
	m_ulMapBaseNum			= 0;
	m_dPrescanRotateDegree	= 0.0;
	m_bFindAllRefer4Scan	= FALSE;
	m_bManualRegionScanSort	= FALSE;
	m_bSingleHomeRegionScan	= FALSE;
	m_lAllReferSpiralLoop	= 0;
	m_lRescanRunStage		= 0;	//	initialization
	m_lWftAdvSamplingStage	= 0;
	m_bEnableAutoCycleLog	= FALSE;
	m_bRescanLogPrImage		= FALSE;
	m_bPrescanLogNgImage	= FALSE;
	m_bCaptureScanMapImage	= FALSE;
	m_lTabletPixelX			= 1920;
	m_lTabletPixelY			= 1200;
	m_lPngPixelX			= 1920;
	m_lPngPixelY			= 1200;
	m_lPngDieExtraEdgeE		= 0;
	m_lPngDieExtraEdgeY		= 0;
	m_lPngDieExtraEdgeN		= 0;
	m_lPngDieExtraEdgeX		= 0;
	m_dTabletDimInch		= 7.0;
	m_dTabletDimInch_X		= sqrt(7.0*7.0 - 3.71*3.71);
	m_dTabletDimInch_Y		= 3.71;
	m_dTabletXPPI			= 323.45;
	m_dTabletYPPI			= 323.45;

	m_ulBoundaryDieColour	= RGB(255, 255, 255);	// Boundary die
	m_ulNGDieColourY		= RGB(0, 255, 0);		// NG die	
	m_ulNGDieColourN		= RGB(0, 0, 255);		// NG die	N
	m_ulNGDieColourX		= RGB(255, 0, 0);		// NG die	X
	m_ulNGDieColourE		= RGB(0, 0, 0);		// NG die	

	m_bPngEnableGrid = TRUE;
	m_lPngGridRow	= 2;
	m_lPngGridCol	= 3;
	m_lPngGridThick = 2;

	m_bGridColourOn1 = TRUE;
	m_bGridColourOn2 = TRUE;
	m_bGridColourOn3 = TRUE;
	m_bGridColourOn4 = TRUE;
	m_bGridColourOn5 = TRUE;
	m_ulGridColourArray1 = RGB(255,137,000);
	m_ulGridColourArray2 = RGB(000,255,000);
	m_ulGridColourArray3 = RGB(000,000,000);
	m_ulGridColourArray4 = RGB(000,000,255);
	m_ulGridColourArray5 = RGB(255,255,255);

	m_szSaveMapImagePath	= "";
	m_szScanNgImageLogPath	= "";
	m_bScanLowYield			= FALSE;
	m_lMapPrescanAlignWafer = SCAN_ALIGN_WAFER_DISABLE;
	m_bMapDummyPrescanAlign	= FALSE;
	m_bMapDummyScanCornerCheck	= FALSE;
	m_bEnable6InchWaferTable	= pApp->GetProfileInt(gszPROFILE_SETTING, "Enable 6 Inch Wafer Table", 0);
	m_bPrescanCheckDefect	= FALSE;
	m_bPrescanCheckBadCut	= FALSE;
	m_bACF_SaveImages		= FALSE;
	m_bACF_MapBackupPath	= FALSE;
	m_bNgPickBadCut			= FALSE;
	m_bNgPickDefect			= FALSE;
	m_bNgPickEdgeDie		= FALSE;
	m_ucPSMLoadAction		= 0;
	m_bPsmEnableState		= FALSE;
	m_bSmallMapSortRpt		= FALSE;
	m_bGenRptOfSMS			= FALSE;
	m_bDisablePsm			= FALSE;
	m_bES100DisableSCN		= FALSE;
	m_bCheckDieOrientation	= FALSE;
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//m_bCheckDieOrientation	= TRUE; //Default value to TRUE by Matthew 20190410
	}
	m_bEnableCheckDieOrientation = FALSE;
	if (pApp->GetCustomerName() == "AOT" || pApp->GetCustomerName() == "ABC") //ABC is used in AOT
	{
		m_bEnableCheckDieOrientation = TRUE;
	}
	m_bES100ByMapIndex		= FALSE;
	m_bPrescanNml3AtHomeDie	= FALSE;
	m_lMa3AHomeRow			= 0;
	m_lMa3AHomeCol			= 0;
	m_lMa3RHomeRow			= 0;
	m_lMa3RHomeCol			= 0;
	m_lMa3MapDirX			= 0;
	m_lMa3MapDirY			= 0;
	m_bSetDummyMa3HomeDie	= FALSE;
	m_bEnableClickMapAndTableGo	= FALSE;
	m_szOemCustomerName		= "";
	m_szOemProductName		= "";
	m_szOemTestSystem		= "";
	m_szPsmMapMachineNo		= "";
	m_szDieState			= " ";
	m_ulPrescanRefPoints	= 0;
	m_bPrescanMapDisplay	= true;
	m_lEmptyGradeMinCount	= 0;
	m_nPrescanIndexStepRow	= 1;
	m_nPrescanIndexStepCol	= 1;
	m_ulMapIndexLoop		= 1;
	m_ucPrescanMapShapeType = WT_MAP_SHAPE_CIRCLE;
	m_ulAlignHomeDieRow		= 0;
	m_ulAlignHomeDieCol		= 0;
	m_nAlignHomeDieWftX		= 0;
	m_nAlignHomeDieWftY		= 0;

	m_lFindMapEdgeDieLimit	= 0;
	m_ulInspectGoType		= 0;
	m_ulAlertType			= 0;
	m_bSearchWaferLimitDone	= FALSE;
	m_bRegionBlkPickOrder	= FALSE;
	m_bFirstRegionInit		= TRUE;
	m_ulReverifyReferRegion	= 0;
	m_bRebelManualAlign		= FALSE;
	m_lRegionPickIndex		= 0;
	memset(&m_lSubRegionState, 0, sizeof(m_lSubRegionState));
	m_dPrescanAreaDiameter	= 0.0;
	m_dBarBridgeLength		= 0.0;
	m_bDummyQuarterWafer	= FALSE;
	m_bScanMapUseBackupExt	= FALSE;
	m_bScanMapAddTimeStamp	= FALSE;
	m_ucQuarterWaferCenter	= 0;
	m_bScanWithWaferLimit	= FALSE;
	m_bScanToHalfMap		= FALSE;
	m_bSortMultiToOne		= FALSE;
	m_bChangeGrade4PrNG 	= FALSE;
	m_bScanAreaConfig		= FALSE;
	m_lScanAreaUpLeftX		= 0;
	m_lScanAreaUpLeftY		= 0;
	m_lScanAreaLowRightX	= 0;
	m_lScanAreaLowRightY	= 0;
	m_lReferPitchRow		= 10;
	m_lReferPitchCol		= 10;
	m_lReadFromWFT			= 0;

	m_lScnCheckAlarmLimit	= 0;
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		m_lScnCheckAlarmLimit	=	1;
	}
	m_bScnCheckNormalOnRefer	= FALSE;
	m_bAdvRegionOffset			= FALSE;
	m_lMC[WTMC_RscnPitchUpTol]	= 100;
	m_lMC[WTMC_MWMV_DieLimit]	= 1;
	m_lMC[WTMC_MWMV_PassScore]	= 75;
	m_lMC[WTMC_MWMV_DoLimit]	= 0;
	m_lMC[WTMC_DAR_IdleLowLmt]	= 3;
	m_lMC[WTMC_DAR_IdleUpLmt]	= 0;
	m_lMC[WTMC_AdvUpdUpPcntLmt]	= 175;
	m_lMC[WTMC_AdvSmpJumpLmt]	= 0;
	m_lMC[WTMC_AdvSmpCtrToEdge] = 90;
	m_lMC[WTMC_AdvSmpCtrFrqc]	= 1;
	m_lMC[WTMC_AdvSmpCtrBase]	= 5;
	m_lMC[WTMC_AP_C_MIN_T]		= 50;
	m_lMC[WTMC_AP_R_MIN_T]		= 25;
	m_lRescanStartRow			= -1;
	m_lRescanStartCol			= -1;
	m_lRescanStartWftX			= 0;
	m_lRescanStartWftY			= 0;
	m_lRescanLastDiePosnX		= 0;
	m_lRescanLastDiePosnY		= 0;
	m_bToDoSampleOncePE			= FALSE;
	m_bAutoRescanWafer			= FALSE;
	m_bAutoRescanAllDice		= FALSE;
	m_bAutoSampleAfterPE		= FALSE;
	m_bAutoSampleAfterPR		= FALSE;
	m_ulPdcRegionSampleCounter	= 0;
	m_ulPdcRegionSizeRow	= 0;
	m_ulPdcRegionSizeCol	= 0;
	m_nPrescanAlignMapCol	= 0;
	m_nPrescanAlignMapRow	= 0;
	m_nPrescanAlignPosnX	= 0;
	m_nPrescanAlignPosnY	= 0;
	m_nPrescanAlign2ndDistY	= 0;
	m_lPrescanPrDelay		= 10;
	m_nPrescanMapCtrX		= 0;
	m_nPrescanMapCtrY		= 0;
	m_nMapLastPrescanPosnX	= 0;
	m_nMapLastPrescanPosnY	= 0;
	m_nPrescanMapRadius		= 0;
	m_nPrescanMapWidth		= 0;
	m_nPrescanMapHeight		= 0;
	m_bAutoGoAroundPE		= FALSE;
	m_bCheckDiePrIn4X		= FALSE;
	m_ulPrescanGoCheckLimit	= 0;
	m_ulPrescanGoCheckCount	= 0;
	m_ulPitchAlarmGoCheckLimit = 0;
	m_ulPitchAlarmGoCheckCount = 999;
	m_szNextLocation		= "";
	m_aUIGoCheckRow.RemoveAll();
	m_aUIGoCheckCol.RemoveAll();
	m_bVerifyMapWaferFail	= FALSE;
	m_lVerifyMapWaferRow	= 0;
	m_lVerifyMapWaferCol	= 0;

	m_ulGrade151Total		= 0;	//	init
	m_bChangeRegionGrade	= FALSE;
	m_bPrescanAutoDiePitch	= FALSE;
	m_lPrescanDiePitchX_X	= 0;
	m_lPrescanDiePitchX_Y	= 0;
	m_lPrescanDiePitchY_Y	= 0;
	m_lPrescanDiePitchY_X	= 0;

	m_bAlignNeedRescan		= FALSE;
	m_bRunTimeDetectEdge	= FALSE;
	m_lADEEmptyFrameLimit	= 0;
	m_nPrescanDirection		= 1;
	m_nPrescanAlignScanStage= 1;
	m_lDetectEdgeState		= 0;
	m_bWaferEndUncheckRefer	= FALSE;
	m_bPrescanBlkPickAlign	= FALSE;
	m_bSortingDefectDie		= FALSE;
	m_bToPickDefectDie		= FALSE;
	m_bEnableMapDMFile		= FALSE;
	m_bScanPickedDieResort	= FALSE;
	m_ucDefectiveDieGrade	= 99;
	m_bHmiDefectPick		= TRUE; // pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_PICK_DEFECT_DIE);
	m_lGetRgnWftPosX		= 0;
	m_lGetRgnWftPosY		= 0;
	m_lGetAtcWftPosX		= 0;
	m_lGetAtcWftPosY		= 0;
	m_ulAdvStage1StartCount	= 0;
	m_bWftDoSamplingByBT	= FALSE;
	m_ulAdvStage1SampleSpan	= 0;
	m_ulAdvStage2StartCount	= 0;
	m_ulAdvStage2SampleSpan	= 0;
	m_ulAdvStage3StartCount	= 0;
	m_ulAdvStage3SampleSpan	= 0;
	m_ulAdvOffsetUpdMinTime	= 600;
	m_ulAdvOffsetExpireTime	= 600;
	m_ulAdvOffsetUpdMinVary	= 0;
	m_ulRescanSampleSpan	= 3000;
	m_ulRescanStartCount	= 3000;

	m_ulPitchAlarmCount		= 0;
	m_ulPitchAlarmLimit		= 0;
	m_lLFPitchToleranceX	= 0;
	m_lLFPitchToleranceY	= 0;
	m_lLFPitchErrorLimit	= 0;
	m_lLFPitchErrorCount	= 0;
	m_lLFPitchErrorCount2	= 0;
	m_lRescanPELimit		= 0;	//	die picked minimum limit after last time rescan/ks

	m_szPrescanLogPath.Empty();
	m_szPrescanMapName.Empty();
	m_ucPrescanRealignMethod= 0;
	m_bScnCheckDieAsRefDie	= FALSE;
	m_lPrescanVerifyResult	= 0;
	m_lPresortWftStatus		= 0;

	m_szMapServerFullPath		= "";
	m_szScanAoiMapFormat		= _T("none");
	m_szPrescanBackupMapPath	= gszROOT_DIRECTORY + _T("\\OutputFiles\\PrescanBackupMap");
	m_bPrescanBackupMap			= FALSE;
	m_szOutputScanSummaryPath	= gszROOT_DIRECTORY + _T("\\OutputFiles\\PrescanSummary");
	m_lOutputScanSummary		= 0;	//	0, none; 1, sort; 2, scan; 3, both
	m_szScanMA1FilePath			="";
	m_dMA1ScanYieldLimit		= 0.0;
	m_bHmiOperationLogOption	= FALSE;
	m_szHmiOperationLogPath		= "";

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	pUtl->SetPsmMapMcNo(m_szPsmMapMachineNo);
	m_bAutoSearchWaferLimit = FALSE;
	m_bFixDummyMapCenter  = FALSE;
	m_bAutoWaferLimitOk = FALSE;
	m_lAutoWaferCenterX = 0;
	m_lAutoWaferCenterY = 0;
	m_lAutoWaferDiameter = 0;
	m_bHmiToUseCharDie	= TRUE;

	m_bAlarmTwice		= FALSE;

	m_bAutoWaferLimitOnce = FALSE;
	m_lAutoWaferWidth	= 0;
	m_lAutoWaferHeight	= 0;
	m_bPitchCheckMapWafer = TRUE;
	m_lAreaPickStage	= 0;
	
	m_ulGoodDieTotal		= 0;
	m_ulTotalDieCounter		= 0;
	m_dWaferYield			= 0;

	m_bRegionPrescan	= pUtl->GetPrescanRegionMode();

	m_bEnableAtcLog	= FALSE;
	CString szTemp;
	//szTemp = m_oPstModel.GetSWVersion();
	m_szDEB_LibVersion = szTemp.Right(4);
	m_bEnableAdaptPredict = FALSE;
	m_bDebKeyDieSampleOld = FALSE;
	m_bGiveUpDebKeyDie	= FALSE;
	m_ucDEBKeyDieGrade	= 0;
	m_bPrescanDebugEnable	= 	pApp->GetProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_PRESCAN_DEBUG, 0);
	pUtl->SetPrescanDebug(m_bPrescanDebugEnable);
	m_bDEB_INIT_OK		= FALSE;
	m_ulPrescanGoToType	= 0;

	m_lPredictMethod	= 0;
	m_ulLastRscnPickCount	= 0;
	m_ulNextRescanCount	= 0;
//	SP_Switch(1000, &HDInfoCBFunctionPass, &HDInfoCBFunctionFail);

	m_lOrgDieA_X				= 0;
	m_lOrgDieA_Y				= 0;
	m_lOrgDieB_X				= 0;
	m_lOrgDieB_Y				= 0;
	m_dOrgAngleCos_X			= 0.0;
	m_dOrgAngleSin_Y			= 0.0;
	m_lRlnDieA_X				= 0;
	m_lRlnDieA_Y				= 0;
	m_lRlnDieB_X				= 0;
	m_lRlnDieB_Y				= 0;
	m_dRlnAngleCos_X			= 0.0;
	m_dRlnAngleSin_Y			= 0.0;

	m_bLoadedWaferScannedOnce	= FALSE;
	m_bAoiOcrLoaded				= FALSE;
	m_lScnAlignMethod			= 0;
	m_bScnAlignDieAsRefer		= FALSE;
	m_bScnAlignUseRefer			= FALSE;
	m_lScnAlignTotalPoints		= 0;
	m_lScnAlignReferNo			= 0;
	m_bScnPrescanIfBonded		= FALSE;
	memset(&m_laScnAlignRow, 0, sizeof(m_laScnAlignRow));
	memset(&m_laScnAlignCol, 0, sizeof(m_laScnAlignCol));

	m_dScanStartTime	= GetTime();
	m_dScanEndTime		= GetTime();
	m_stScanStartCTime	= CTime::GetCurrentTime();
	m_stScanEndCTime	= CTime::GetCurrentTime();
	m_dPrescanResultRemoveTime	= GetTime();
	for(int i=0; i<50; i++)
		m_lTimeSlot[i]	= 0;

	m_lReferMapTotal	= 0;
	memset(m_lReferMapRow, 0, sizeof(m_lReferMapRow));
	memset(m_lReferMapCol, 0, sizeof(m_lReferMapCol));

	// sort mode
	m_lMS90HalfDivideMapOrgRow	= 0;
	m_ulMS90HalfBorderMapRow	= 0;
	m_lMS902ndHomeDieMapRow		= 0;
	m_lMS902ndHomeDieMapCol		= 0;
	m_lMS902ndHomeOffsetRow		= 0;
	m_lMS902ndHomeOffsetCol		= 0;
	m_bSortGoingTo2ndPart		= FALSE;	// init
	m_b2PartsSortAutoAlignWay	= TRUE;	// init
	m_bGetAvgFOVOffset			= FALSE;
	m_lAvgFovOffsetX	= 0;
	m_lAvgFovOffsetY	= 0;
	m_bContourEdge		= FALSE;
	m_ulContourEdgePoints	= 0;
	memset(m_lCntrEdgeY_C, 0, sizeof(m_lCntrEdgeY_C));
	memset(m_lCntrEdgeX_L, 0, sizeof(m_lCntrEdgeX_L));
	memset(m_lCntrEdgeX_R, 0, sizeof(m_lCntrEdgeX_R));

	m_lBurnInMapDieCount	= 0;

	m_ucCornerPatternCheckMode	= 0;
	m_ucCornerReferPrID			= 1;
	m_bCornerPatternCheckAll	= FALSE;
	m_lReferToCornerDistX		= 0;
	m_lReferToCornerDistY		= 0;
	m_b2Parts1stPartDone		= pApp->GetProfileInt(gszPROFILE_SETTING, _T("MS90 Half Sort 1st Done"), 0);
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("MS90 Half Sort 1st Done"), m_b2Parts1stPartDone);
}

VOID CWaferTable::SavePrescanOptionFromWFT(CStringMapFile  *psmf)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	(*psmf)[WT_ALIGN_DATA]["ReadFromWaferTable"]	= 3;

	// stage 1
	if( m_ucDummyPrescanPNPGrade==0 )
		m_ucDummyPrescanPNPGrade = 1;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_MAP_USE_BACKUP_EXT] 	= m_bScanMapUseBackupExt;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_MAP_ADD_TIME_STAMP] 	= m_bScanMapAddTimeStamp;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_DUMMY_QUARTER_WAFER]	= m_bDummyQuarterWafer;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_DUMMY_PNP_GRADE]		= m_ucDummyPrescanPNPGrade;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_DUMMY_DIAMETER]		= m_dPrescanAreaDiameter;
	(*m_psmfSRam)["WaferTable"]["Dummy Prescan Area Size"]	= m_dPrescanAreaDiameter;
	(*psmf)[WT_ALIGN_DATA][WT_BARWAFER_BIRDGE_LENGTH]		= m_dBarBridgeLength;

	if (m_lMapPrescanAlignWafer > 0)
	{
		m_bScanWithWaferLimit = TRUE;
	}
	else
	{
		m_bScanWithWaferLimit = FALSE;
	}

	(*psmf)[WT_ALIGN_DATA][WT_SCAN_WITH_WAFER_LIMIT]	= m_bScanWithWaferLimit;
	(*psmf)[WT_ALIGN_DATA][WT_SCAN_DIVIDE_MS90_MAP]		= m_bScanToHalfMap;
	(*psmf)[WT_OPTION][WT_ENABLE_PR_NG_TO_GRADE]		= m_bChangeGrade4PrNG;
	(*psmf)[WT_OPTION][WT_MAP_SORT_MULTI_TO_SAME_BIN]	= m_bSortMultiToOne;
	// dummy define corner or center/diameter
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_CONFIG]	= m_bScanAreaConfig;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_UL_X]	= m_lScanAreaUpLeftX;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_UL_Y]	= m_lScanAreaUpLeftY;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_LR_X]	= m_lScanAreaLowRightX;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_LR_Y]	= m_lScanAreaLowRightY;

	// to update wafer map for AOI only machine.
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_OEM_CUSTOMER_NAME]	= m_szOemCustomerName;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_OEM_PRODUCT_NAME]		= m_szOemProductName;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_OEM_TEST_SYSTEM]		= m_szOemTestSystem;
	(*psmf)[WT_OPTION][WT_AUTO_GO_PE_AROUND]				= m_bAutoGoAroundPE;
	(*psmf)[WT_OPTION][WT_PRESCAN_GO_CHECK_LIMITS]			= m_ulPrescanGoCheckLimit;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AOI_MA3_HOME_ROW]		= m_lMa3AHomeRow;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AOI_MA3_HOME_COL]		= m_lMa3AHomeCol;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AOI_MA3_RHOME_ROW]	= m_lMa3RHomeRow;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AOI_MA3_RHOME_COL]	= m_lMa3RHomeCol;
	(*psmf)[WT_ALIGN_DATA][SCAN_DUMMY_MAP_COL_DIRECTION]	= m_lMa3MapDirX;
	(*psmf)[WT_ALIGN_DATA][SCAN_DUMMY_MAP_ROW_DIRECTION]	= m_lMa3MapDirY;
	(*psmf)[WT_ALIGN_DATA][SCAN_SET_DUMMYMA3_HOMEDIE]		= m_bSetDummyMa3HomeDie;
	(*psmf)[WT_OPTION][WT_GO_CHECK_DIE_PR_IN4X]				= m_bCheckDiePrIn4X;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_CHANGE_REGION_GRADE]	= m_bChangeRegionGrade;

	pUtl->SetPrescanMethod(GetScanMethod());
	// prescan relative settings
	(*psmf)[WT_ALIGN_DATA][WT_RESCAN_AFTER_REALING]	= m_bAlignNeedRescan;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_ENABLED]		= m_bEnablePrescan;
	pUtl->SetPrescanEnable(IsPrescanEnable());
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_PR_DELAY]		= m_lPrescanPrDelay;
	(*m_psmfSRam)["WaferTable"]["Prescan Pr Delay"] = m_lPrescanPrDelay;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_PR_METHOD] 	= m_ulPrescanMethod;
	(*psmf)[WT_ALIGN_DATA]["XY Strip Mode"] 		= m_ulPrescanXYStripMode;
	(*psmf)[WT_ALIGN_DATA][WT_SCAN_WALK_TOUR] 		= m_ucScanWalkTour;
	(*psmf)[WPR_PSCAN_OPTIONS][WT_AOI_REMOVE_MAP_HEADER]	= m_ucScanMapTreatment;
	(*psmf)[WT_ALIGN_DATA][WT_SCAN_RUNTIME_DETECT_EDGE]		= m_bRunTimeDetectEdge;
	(*psmf)[WT_ALIGN_DATA][WT_SCAN_ADE_EMPTY_FRAME_LIMIT]	= m_lADEEmptyFrameLimit;

	if( m_dPrescanFailPassScore>m_dPrescanGoodPassScore )
	{
		m_dPrescanFailPassScore = 0.0;
	}
	(*psmf)[WT_ALIGN_DATA][WT_ENABLE_MAP_DM_FILE]		= m_bEnableMapDMFile;
	(*psmf)[WT_OPTION][WT_SCAN_PICKED_DIE_RESORT]		= m_bScanPickedDieResort;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_FINISHEDACTION]	= m_lPrescanFinishedAction;
	(*psmf)[WT_OPTION][WT_PRESCAN_GOOD_PASSSCORE]		= m_dPrescanGoodPassScore;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_FAIL_PASSSCORE]	= m_dPrescanFailPassScore;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_MAP_BASE_NUM]	= m_ulMapBaseNum;
	(*psmf)[WT_OPTION][WT_SCAN_OFFLINE_UPLOAD_SCAN_FILE]= m_bOfflineUploadScanFile;
	(*psmf)[WT_OPTION][WT_PRESCAN_CHECK_BADCUT]			= m_bPrescanCheckBadCut;
	(*psmf)[WT_OPTION][WT_PRESCAN_CHECK_DEFECT]			= m_bPrescanCheckDefect;
	(*psmf)[WT_PSCAN_OPTIONS][WT_ACF_SAVEIMAGES]		= m_bACF_SaveImages;
	(*psmf)[WT_PSCAN_OPTIONS][WT_ACF_MAP_BACKUP]		= m_bACF_MapBackupPath;

	(*psmf)[WT_OPTION][WT_REGION_SCAN_WITH_HOME_ONLY]	= m_bSingleHomeRegionScan;
	(*psmf)[WT_OPTION][WT_REGION_SCAN_MANUAL_ALIGN]		= m_bManualRegionScanSort;
	(*psmf)[WT_OPTION][WT_FIND_ALL_REFER_4_SCAN]		= m_bFindAllRefer4Scan;
	(*psmf)[WT_OPTION][WT_ALL_REFER_SPIRAL_LOOP]		= m_lAllReferSpiralLoop;
	(*psmf)[WT_OPTION][WT_PRESCAN_LOG_PR_IMAGE]			= m_bRescanLogPrImage;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LOG_NG_IMAGE]	= m_bPrescanLogNgImage;
	(*psmf)[WT_OPTION][WT_GRAB_SCAN_MAP_IMAGE]			= m_bCaptureScanMapImage;
	(*psmf)[WT_OPTION][WT_GRAB_SAVE_MAP_IMAGE_PATH]		= m_szSaveMapImagePath ;
	(*psmf)[WT_OPTION][WT_LOG_SCAN_PR_NG_IMAGE_PATH]	= m_szScanNgImageLogPath;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_BLKPICK_ALIGN]	= m_bPrescanBlkPickAlign;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_MAP_SHAPE_TYPE]	= m_ucPrescanMapShapeType;
	(*psmf)[WT_ALIGN_DATA][WT_ORIGINAL_HORI_FLIP]		= m_bMapOHFlip;
	(*psmf)[WT_ALIGN_DATA][WT_ORIGINAL_VERT_FLIP]		= m_bMapOVFlip;
	(*psmf)[WT_ALIGN_DATA][WT_ORIGINAL_ROT_ANGLE]		= m_usMapAngle;
	(*psmf)[WT_ALIGN_DATA][WT_WAFEREND_UNCHECK_REFER]	= m_bWaferEndUncheckRefer;
	(*psmf)[WT_ALIGN_DATA][WT_REGION_BLKPICK_ORDER]		= m_bRegionBlkPickOrder;
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	(*psmf)[WT_ALIGN_DATA][WT_REGION_PICK_MIN_LIMIT]	= pSRInfo->m_lRegionPickMinLimit;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AUTO_MAP_LIMIT]	= m_lFindMapEdgeDieLimit;
	(*psmf)[WT_ALIGN_DATA][WT_REGION_PICK_INDEX]		= m_lRegionPickIndex;
	for(int i=0; i<WT_MAX_SUBREGIONS_LIMIT; i++)
	{
		(*psmf)[WT_ALIGN_DATA][WT_REGION_STATE_DATA][i]	= m_lSubRegionState[i];
	}
	if( m_lTabletPixelX==0 || m_lTabletPixelY==0 || m_dTabletDimInch_X == 0.0 || m_dTabletDimInch_Y == 0.0 )
	{
		m_lTabletPixelX			= 1920;
		m_lTabletPixelY			= 1200;
		m_dTabletDimInch		= 7.0;
		m_dTabletDimInch_X		= sqrt(7.0*7.0 - 3.71*3.71);
		m_dTabletDimInch_Y		= 3.71;
	}

	if( m_lPngPixelX<m_lTabletPixelX )
	{
		m_lPngPixelX = m_lTabletPixelX;
	}
	if( m_lPngPixelY<m_lTabletPixelY )
	{
		m_lPngPixelY = m_lTabletPixelY;
	}

	if( m_dTabletDimInch_X != 0.0 && m_dTabletDimInch_Y != 0.0)
	{
		m_dTabletDimInch	= sqrt(m_dTabletDimInch_X*m_dTabletDimInch_X + m_dTabletDimInch_Y*m_dTabletDimInch_Y);
		m_dTabletXPPI		= (DOUBLE)m_lTabletPixelX/m_dTabletDimInch_X;
		m_dTabletYPPI		= (DOUBLE)m_lTabletPixelY/m_dTabletDimInch_Y;
	}

	if( m_lPngDieExtraEdgeE < 0 )
	{
		m_lPngDieExtraEdgeE	= 0;
	}
	if( m_lPngDieExtraEdgeY < 0 )
	{
		m_lPngDieExtraEdgeY	= 0;
	}
	if( m_lPngDieExtraEdgeN < 0 )
	{
		m_lPngDieExtraEdgeN	= 0;
	}
	if( m_lPngDieExtraEdgeX < 0 )
	{
		m_lPngDieExtraEdgeX	= 0;
	}

	// ES101: generate wafer picture
	(*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_PIXEL_X]			= m_lTabletPixelX;
	(*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_PIXEL_Y]			= m_lTabletPixelY;
	(*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIE_EXTRA_EDGE_E]	= m_lPngDieExtraEdgeE;
	(*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIE_EXTRA_EDGE_Y]	= m_lPngDieExtraEdgeY;
	(*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIE_EXTRA_EDGE_N]	= m_lPngDieExtraEdgeN;
	(*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIE_EXTRA_EDGE_X]	= m_lPngDieExtraEdgeX;

	// Wafer Pricture Colour
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_ENABLE]		= m_bPngEnableGrid;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_ROW]			= m_lPngGridRow;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COL]			= m_lPngGridCol;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_THICK]		= m_lPngGridThick;

	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_1]	= m_bGridColourOn1;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_2]	= m_bGridColourOn2;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_3]	= m_bGridColourOn3;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_4]	= m_bGridColourOn4;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_5]	= m_bGridColourOn5;

	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_1]		= m_ulGridColourArray1;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_2]		= m_ulGridColourArray2;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_3]		= m_ulGridColourArray3;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_4]		= m_ulGridColourArray4;
	(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_5]		= m_ulGridColourArray5;


	(*psmf)[WT_PSCAN_OPTIONS][WT_BOUND_DIE_COLOUR]		= m_ulBoundaryDieColour;	// Boundary die
	(*psmf)[WT_PSCAN_OPTIONS][WT_NG_DIE_COLOUR_Y]		= m_ulNGDieColourY;			// NG die Y	
	(*psmf)[WT_PSCAN_OPTIONS][WT_NG_DIE_COLOUR_E]		= m_ulNGDieColourE;			// NG die E
	(*psmf)[WT_PSCAN_OPTIONS][WT_NG_DIE_COLOUR_N]		= m_ulNGDieColourN;			// NG die N
	(*psmf)[WT_PSCAN_OPTIONS][WT_NG_DIE_COLOUR_X]		= m_ulNGDieColourX;			// NG die X
	(*psmf)[WT_PSCAN_OPTIONS][WT_PNG_IMAGE_PIXEL_X]		= m_lPngPixelX;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PNG_IMAGE_PIXEL_Y]		= m_lPngPixelY;
	(*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIM_X_INCH]		= m_dTabletDimInch_X;
	(*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIM_Y_INCH]		= m_dTabletDimInch_Y;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LF_PITCH_TOL_X]	= m_lLFPitchToleranceX;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LF_PITCH_TOL_Y]	= m_lLFPitchToleranceY;
	(*psmf)[WT_PSCAN_OPTIONS][WT_RESCAN_PE_LIMIT]		= m_lRescanPELimit;
	(*psmf)[WT_PSCAN_OPTIONS][WT_LF_PE_LIMIT]			= m_lLFPitchErrorLimit;

	(*psmf)[WT_OPTION][WT_PRESCAN_NG_PICK_BADCUT]	= m_bNgPickBadCut;
	(*psmf)[WT_OPTION][WT_PRESCAN_NG_PICK_DEFECT]	= m_bNgPickDefect;
	(*psmf)[WT_OPTION][WT_PRESCAN_NG_PICK_EDGEDIE]	= m_bNgPickEdgeDie;

	(*psmf)[WT_OPTION][WT_PRESCAN_REGION_SIZE_ROW]	= m_ulPdcRegionSizeRow;
	(*psmf)[WT_OPTION][WT_PRESCAN_REGION_SIZE_COL]	= m_ulPdcRegionSizeCol;
	(*psmf)[WT_ALIGN_DATA][WT_SCAN_FPC_ALARM_LIMIT]	= m_lScnCheckAlarmLimit;
	(*psmf)[WT_ALIGN_DATA][WT_SCAN_FPC_CHECK_NORMAL_ON_REFER]	=	m_bScnCheckNormalOnRefer;

	// stage 2
	DEB_CheckEnableFeature(TRUE);	//Silan to use DEB v3.49	//v4.50A25	
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_ATCDEB_LOG]		= m_bEnableAtcLog;
	(*psmf)[WT_ALIGN_DATA][WT_DEB_GIVEUP_KEY_DIE]		= m_bGiveUpDebKeyDie;
	(*psmf)[WT_ALIGN_DATA][WT_DEB_SAMPLE_KEY_DIE_OLD]	= m_bDebKeyDieSampleOld;
	(*psmf)[WT_ALIGN_DATA][WT_DEB_KEY_DIE_PICK_GRADE]	= m_ucDEBKeyDieGrade;
	(*psmf)[WT_ALIGN_DATA][WT_WAFER_MYLAR_PREDICTION]	= m_lPredictMethod;
	(*psmf)[WT_ALIGN_DATA][WT_WAFER_MYLAR_PREDICTION]["1"]	= TRUE;
	(*psmf)[WT_OPTION][WT_PITCH_CHECK_ALARM_LIMIT]		= m_ulPitchAlarmLimit;
	(*psmf)[WT_OPTION][WT_AUTO_RESCAN_ALL_DICE]			= m_bAutoRescanAllDice;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_OFFSET_EXPIRE_TIME]	= m_ulAdvOffsetExpireTime;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_OFFSET_UPD_MINTIME]	= m_ulAdvOffsetUpdMinTime;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_OFFSET_MIN_SAMPLE]	= m_ulAdvOffsetUpdMinVary;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_DEB_SAMPLE_BY_BT]		= m_bWftDoSamplingByBT;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE1_START_COUNT]	= m_ulAdvStage1StartCount;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE1_SAMPLE_SPAN]	= m_ulAdvStage1SampleSpan;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE2_START_COUNT]	= m_ulAdvStage2StartCount;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE2_SAMPLE_SPAN]	= m_ulAdvStage2SampleSpan;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE3_START_COUNT]	= m_ulAdvStage3StartCount;
	(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE3_SAMPLE_SPAN]	= m_ulAdvStage3SampleSpan;
	(*psmf)[WT_OPTION][WT_AUTO_SAMPLE_AFTER_PE]			= m_bAutoSampleAfterPE;
	(*psmf)[WT_OPTION][WT_AUTO_SAMPLE_AFTER_PR]			= m_bAutoSampleAfterPR;

	if (m_ulRescanStartCount < 3000)
	{
		m_ulRescanStartCount = 3000;
	}

	if (m_ulRescanSampleSpan < 3000)
	{
		m_ulRescanSampleSpan = 3000;
	}
	(*psmf)[WT_ALIGN_DATA][WT_RESCAN_DEB_SAMPLE_SPAN]	= m_ulRescanSampleSpan;
	(*psmf)[WT_ALIGN_DATA][WT_RESCAN_DEB_SAMPLE_START]	= m_ulRescanStartCount;

	if( pApp->GetCustomerName()!="Huga" )
	{
		m_bErrorChooseGoFPC = FALSE;
	}

	(*psmf)[WT_ALIGN_DATA][WT_SCN_ALIGN_FPC_ENABLE]				= m_lScnAlignMethod;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_SET_AS_REFER]			= m_bScnAlignDieAsRefer;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_USE_REFER]				= m_bScnAlignUseRefer;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_TOTAL_COUNTS]			= m_lScnAlignTotalPoints;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_REFER_DIE_NO]			= m_lScnAlignReferNo;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_SCAN_BONDEDWAFER]		= m_bScnPrescanIfBonded;

	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_HMI_ROW]				= m_lScnHmiRow;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_HMI_COL]				= m_lScnHmiCol;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_PRESCAN_GO_TO_TYPE]	= m_ulPrescanGoToType;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_REF_ROW_OFFSET]		= m_lRefHomeDieRowOffset;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_REF_COL_OFFSET]		= m_lRefHomeDieColOffset;
	(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_TOTAL_CHECK_DIE]		= m_lTotalSCNCount;

	for (int i=1; i<WT_ALN_MAXCHECK_SCN; i++)
	{
		(*psmf)[WT_SCN_ALIGN_SETTINGS][i][WT_SA_POINT_ROW] = m_laScnAlignRow[i];
		(*psmf)[WT_SCN_ALIGN_SETTINGS][i][WT_SA_POINT_COL] = m_laScnAlignCol[i];
	}

	(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_POS][WT_SCN_CHECK_ENABLE] = m_bEnableSCNCheck;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_BY_VIS]		= m_bScnCheckByPR;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_IS_REFDIE]	= m_bScnCheckIsRefDie;
	(*psmf)[WT_OPTION][WT_SCNCHECK_DIE_AS_REFDIE]	= m_bScnCheckDieAsRefDie;
	(*psmf)[WT_OPTION][WT_ERROR_CHOOSE_GO_FPC]		= m_bErrorChooseGoFPC;

	(*psmf)[WT_ALIGN_DATA][WT_SCN_LOADED]		= m_bScnLoaded;
	if (m_lSCNIndex_X < 2)
		m_lSCNIndex_X = 2;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_INDEX_X]		= m_lSCNIndex_X;
	if (m_lSCNIndex_Y < 2)
		m_lSCNIndex_Y = 2;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_INDEX_Y]		= m_lSCNIndex_Y;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_KEEP_ALIGNDIE]	= m_bKeepSCNAlignDie;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_SCN_LOADED]	= m_bCheckSCNLoaded;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_REFDIE_NO]	= m_lScnCheckRefDieNo;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_X_TOL]		= m_lScnCheckXTol;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_Y_TOL]		= m_lScnCheckYTol;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_ADJ]		= m_bEnableScnCheckAdjust;
	(*psmf)[WT_ALIGN_DATA][WT_REF_HOME_DIE_ROW]		= m_lRefHomeDieRow;
	(*psmf)[WT_ALIGN_DATA][WT_REF_HOME_DIE_COL]		= m_lRefHomeDieCol;

	(*psmf)[WT_MAP_NEW_REFER][WT_MAP_NEW_REFER_NUM]		= m_lReferMapTotal;
	for (int i=0; i<WT_ALN_MAXCHECK_SCN; i++)
	{
		(*psmf)[WT_MAP_NEW_REFER][WT_MAP_NEW_REFER_ROW][i]	= m_lReferMapRow[i];
		(*psmf)[WT_MAP_NEW_REFER][WT_MAP_NEW_REFER_COL][i]	= m_lReferMapCol[i];
	}

	CString szCoorChain = "";
	for (int i=1; i<WT_ALN_MAXCHECK_SCN; i++)
	{
		(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_POS][i][WT_SCN_CHECK_ROW] = m_lScnCheckRow[i];
		(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_POS][i][WT_SCN_CHECK_COL] = m_lScnCheckCol[i];
		if ( i <= m_lTotalSCNCount ) {
			CString szTempWords1, szTempWords2;
			szTempWords1.Format( "%d" , m_lScnCheckRow[i] );
			szTempWords2.Format( "%d" , m_lScnCheckCol[i] );
			szCoorChain += "(";
			szCoorChain += szTempWords1;
			szCoorChain += ",";
			szCoorChain += szTempWords2;
			szCoorChain += ")";
		}
	}

	(*psmf)[WT_ALIGN_DATA][WT_PITCH_VERIFY_MAP_WAFER]	= m_bPitchCheckMapWafer;

	(*psmf)[WT_OPTION][WT_AOI_SCAN_MAP_FORMAT]		= m_szScanAoiMapFormat;
	(*psmf)[WT_OPTION][WT_SCAN_BACKUP_MAP_PATH]		= m_szPrescanBackupMapPath;
	(*psmf)[WT_OPTION][WT_SCAN_BACKUP_MAP_ENABLE]	= m_bPrescanBackupMap;
	(*psmf)[WT_OPTION][WT_OUTPUT_SCAN_SUMMARY_PATH]	= m_szOutputScanSummaryPath;
	(*psmf)[WT_OPTION][WT_OUPUT_SCAN_SUMMARY]		= m_lOutputScanSummary;
	(*psmf)[WT_OPTION][WT_SCAN_MA1_FILEPATH]		= m_szScanMA1FilePath;
	(*psmf)[WT_OPTION][WT_SCAN_MA1_YIELD_LIMIT]		= m_dMA1ScanYieldLimit;
	(*psmf)[WT_OPTION][WT_HMI_OPERATION_LOG_ENABLE]	= m_bHmiOperationLogOption;
	(*psmf)[WT_OPTION][WT_HMI_OPERATION_LOG_PATH]	= m_szHmiOperationLogPath;

	(*m_psmfSRam)["MS896A"]["WaferEndBackupMapFilePath"]	= m_szPrescanBackupMapPath;
	(*m_psmfSRam)["MS896A"]["PointCheckPosition"]			= szCoorChain;
}

VOID CWaferTable::LoadPrescanOptionFromWFT(CStringMapFile  *psmf)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

//	(*m_psmfSRam)["MS896A"]["Include Scan Setting"]	= ;

	m_lReadFromWFT			=		(LONG)(*psmf)[WT_ALIGN_DATA]["ReadFromWaferTable"];
	// stage 1
	m_ucDummyPrescanPNPGrade= (UCHAR)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_DUMMY_PNP_GRADE];
	if( m_ucDummyPrescanPNPGrade==0 )
		m_ucDummyPrescanPNPGrade = 1;
	m_dPrescanAreaDiameter	=	   (DOUBLE)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_DUMMY_DIAMETER];
	(*m_psmfSRam)["WaferTable"]["Dummy Prescan Area Size"] = m_dPrescanAreaDiameter;
	m_bDummyQuarterWafer	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_DUMMY_QUARTER_WAFER];
	m_dBarBridgeLength		= (DOUBLE)(*psmf)[WT_ALIGN_DATA][WT_BARWAFER_BIRDGE_LENGTH];

	m_bScanWithWaferLimit	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCAN_WITH_WAFER_LIMIT];
	m_bScanToHalfMap		= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCAN_DIVIDE_MS90_MAP];
	m_bChangeGrade4PrNG		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ENABLE_PR_NG_TO_GRADE];
	m_bSortMultiToOne		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_SORT_MULTI_TO_SAME_BIN];

	// dummy define corner or center/diameter
	m_bScanAreaConfig		= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_CONFIG];
	m_lScanAreaUpLeftX		=		(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_UL_X];
	m_lScanAreaUpLeftY		=		(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_UL_Y];
	m_lScanAreaLowRightX	=		(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_LR_X];
	m_lScanAreaLowRightY	=		(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AREA_LR_Y];

	m_bScanMapUseBackupExt	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_MAP_USE_BACKUP_EXT];
	m_bScanMapAddTimeStamp	= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_MAP_ADD_TIME_STAMP];

	// to update wafer map for AOI only machine.
	m_szOemTestSystem		= (*psmf)[WT_ALIGN_DATA][WT_PRESCAN_OEM_TEST_SYSTEM];
	m_szOemProductName		= (*psmf)[WT_ALIGN_DATA][WT_PRESCAN_OEM_PRODUCT_NAME];
	m_szOemCustomerName		= (*psmf)[WT_ALIGN_DATA][WT_PRESCAN_OEM_CUSTOMER_NAME];
	if( m_szOemCustomerName.IsEmpty() )
		m_szOemCustomerName = pApp->GetCustomerName();
	m_lMa3AHomeRow			= (*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AOI_MA3_HOME_ROW];
	m_lMa3AHomeCol			= (*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AOI_MA3_HOME_COL];
	m_lMa3RHomeRow			= (*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AOI_MA3_RHOME_ROW];
	m_lMa3RHomeCol			= (*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AOI_MA3_RHOME_COL];
	m_lMa3MapDirX			= (*psmf)[WT_ALIGN_DATA][SCAN_DUMMY_MAP_COL_DIRECTION];
	m_lMa3MapDirY			= (*psmf)[WT_ALIGN_DATA][SCAN_DUMMY_MAP_ROW_DIRECTION];
	m_bSetDummyMa3HomeDie	= (BOOL)(LONG)((*psmf)[WT_ALIGN_DATA][SCAN_SET_DUMMYMA3_HOMEDIE]);

	// prescan relative settings
	m_bAlignNeedRescan		=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_RESCAN_AFTER_REALING];
	m_bEnablePrescan		=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_ENABLED];
	m_lPrescanPrDelay		=		 (LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_PR_DELAY];
	m_ulPrescanMethod		=		 (LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_PR_METHOD];
	m_ulPrescanXYStripMode  =	(LONG)(*psmf)[WT_ALIGN_DATA]["XY Strip Mode"];
	m_ucScanWalkTour		=	(UCHAR)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCAN_WALK_TOUR];
	m_ucScanMapTreatment	=	(UCHAR)(LONG)(*psmf)[WPR_PSCAN_OPTIONS][WT_AOI_REMOVE_MAP_HEADER];
	m_bRunTimeDetectEdge	=	(BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCAN_RUNTIME_DETECT_EDGE];
	m_lADEEmptyFrameLimit	= (*psmf)[WT_ALIGN_DATA][WT_SCAN_ADE_EMPTY_FRAME_LIMIT];
	(*m_psmfSRam)["WaferTable"]["Prescan Pr Delay"] = m_lPrescanPrDelay;
	pUtl->SetPrescanEnable(IsPrescanEnable());

	pUtl->SetPrescanMethod(GetScanMethod());

	if( pUtl->GetPrescanDummyMap() )
	{
	//	MS90Set1stDone(FALSE);	//	load msd, reset to default for dummy wafer.
	}

	m_bScanPickedDieResort		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_SCAN_PICKED_DIE_RESORT];
	m_bEnableMapDMFile			= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ENABLE_MAP_DM_FILE];
	m_lPrescanFinishedAction	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_FINISHEDACTION];
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		m_lPrescanFinishedAction	=	PRESCAN_FINISHED_SORT_BY_GRADE;//(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_FINISHEDACTION];
	}
	m_bOfflineUploadScanFile= (BOOL)(LONG) (*psmf)[WT_OPTION][WT_SCAN_OFFLINE_UPLOAD_SCAN_FILE];
	m_dPrescanGoodPassScore	= (DOUBLE)(*psmf)[WT_OPTION][WT_PRESCAN_GOOD_PASSSCORE];
	m_dPrescanFailPassScore	= (DOUBLE)(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_FAIL_PASSSCORE];
	m_ulMapBaseNum			= (ULONG)(LONG)(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_MAP_BASE_NUM];
	m_bPrescanCheckBadCut	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_CHECK_BADCUT];
	m_bPrescanCheckDefect	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_CHECK_DEFECT];
	m_bACF_SaveImages		= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][WT_ACF_SAVEIMAGES];
	m_bACF_MapBackupPath	= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][WT_ACF_MAP_BACKUP];
	if( m_dPrescanFailPassScore>m_dPrescanGoodPassScore )
	{
		m_dPrescanFailPassScore = 0.0;
	}

	m_bNgPickBadCut			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_NG_PICK_BADCUT];
	m_bNgPickDefect			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_NG_PICK_DEFECT];
	m_bNgPickEdgeDie		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_NG_PICK_EDGEDIE];

	m_ulPdcRegionSizeRow	=(ULONG)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_REGION_SIZE_ROW];
	m_ulPdcRegionSizeCol	=(ULONG)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_REGION_SIZE_COL];
	m_lScnCheckAlarmLimit	= (*psmf)[WT_ALIGN_DATA][WT_SCAN_FPC_ALARM_LIMIT];
	m_bScnCheckNormalOnRefer = (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCAN_FPC_CHECK_NORMAL_ON_REFER];
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		m_bScnCheckNormalOnRefer	=	FALSE;
		m_lScnCheckAlarmLimit		=	1;
	}
	m_ulPrescanGoCheckLimit	=(ULONG)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_GO_CHECK_LIMITS];
	m_bAutoGoAroundPE		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_AUTO_GO_PE_AROUND];
	m_bCheckDiePrIn4X		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_GO_CHECK_DIE_PR_IN4X];
	m_bChangeRegionGrade	= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_CHANGE_REGION_GRADE];

	//	stage 2
	m_bEnableAtcLog			=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_ATCDEB_LOG];
	m_bDebKeyDieSampleOld	=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_DEB_SAMPLE_KEY_DIE_OLD];
	m_bGiveUpDebKeyDie		=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_DEB_GIVEUP_KEY_DIE];
	m_bPitchCheckMapWafer	=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PITCH_VERIFY_MAP_WAFER];
	m_ucDEBKeyDieGrade		=  (UCHAR)(LONG)(*psmf)[WT_ALIGN_DATA][WT_DEB_KEY_DIE_PICK_GRADE];
	m_lPredictMethod		=	(*psmf)[WT_ALIGN_DATA][WT_WAFER_MYLAR_PREDICTION];
	BOOL bSaved				=	(BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_WAFER_MYLAR_PREDICTION]["1"];
	m_ulRescanSampleSpan	=	(*psmf)[WT_ALIGN_DATA][WT_RESCAN_DEB_SAMPLE_SPAN];
	m_ulRescanStartCount	=	(*psmf)[WT_ALIGN_DATA][WT_RESCAN_DEB_SAMPLE_START];
	if (m_ulRescanStartCount < 3000)
	{
		m_ulRescanStartCount = 3000;
	}

	if (m_ulRescanSampleSpan < 3000)
	{
		m_ulRescanSampleSpan = 3000;
	}

	UINT nOldState = pApp->GetProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_PRESCAN_DEBUG, 2);
	if( nOldState==2 )
	{
		nOldState	=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_DEBUG_LOG];
		pApp->WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_PRESCAN_DEBUG, nOldState);
	}
	m_bPrescanDebugEnable	=  nOldState;
	pUtl->SetPrescanDebug(m_bPrescanDebugEnable);

	m_ulPitchAlarmLimit		= (ULONG)(LONG)(*psmf)[WT_OPTION][WT_PITCH_CHECK_ALARM_LIMIT];
	if( bSaved==FALSE )
	{
		m_bAdvRegionOffset		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_ADV_DIE_STEP];
		m_bAutoRescanWafer		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_AUTO_RESCAN_SORTING_WAFER];
		m_bEnableAdaptPredict	=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_ADAPTIVE_PREDICT];
	}
	m_bAutoRescanAllDice	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_AUTO_RESCAN_ALL_DICE];
	m_ulAdvOffsetExpireTime	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_OFFSET_EXPIRE_TIME];
	m_ulAdvOffsetUpdMinTime	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_OFFSET_UPD_MINTIME];
	m_ulAdvOffsetUpdMinVary	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_OFFSET_MIN_SAMPLE];
	m_bWftDoSamplingByBT	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_DEB_SAMPLE_BY_BT];
	m_ulAdvStage1StartCount	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE1_START_COUNT];
	m_ulAdvStage1SampleSpan	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE1_SAMPLE_SPAN];
	m_ulAdvStage2StartCount	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE2_START_COUNT];
	m_ulAdvStage2SampleSpan	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE2_SAMPLE_SPAN];
	m_ulAdvStage3StartCount	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE3_START_COUNT];
	m_ulAdvStage3SampleSpan	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ADV_STAGE3_SAMPLE_SPAN];
	m_bAutoSampleAfterPE	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_AUTO_SAMPLE_AFTER_PE];
	m_bAutoSampleAfterPR	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_AUTO_SAMPLE_AFTER_PR];

	m_lScnAlignMethod		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_ALIGN_FPC_ENABLE];
	m_bScnAlignDieAsRefer	= (BOOL)(LONG)(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_SET_AS_REFER];
	m_bScnAlignUseRefer		= (BOOL)(LONG)(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_USE_REFER];
	m_lScnAlignTotalPoints	= (LONG)(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_TOTAL_COUNTS];
	m_lScnAlignReferNo		= (LONG)(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_REFER_DIE_NO];
	m_bScnPrescanIfBonded	= (BOOL)(LONG)(*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SA_SCAN_BONDEDWAFER];

	m_lScnHmiRow			= (*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_HMI_ROW];
	m_lScnHmiCol			= (*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_HMI_COL];
	m_ulPrescanGoToType		= (*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_PRESCAN_GO_TO_TYPE];
	m_lRefHomeDieRowOffset	= (*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_REF_ROW_OFFSET];
	m_lRefHomeDieColOffset	= (*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_REF_COL_OFFSET];
	m_lTotalSCNCount		= (*psmf)[WT_SCN_ALIGN_SETTINGS][WT_SCN_TOTAL_CHECK_DIE];

	for (int i=1; i<WT_ALN_MAXCHECK_SCN; i++)
	{
		m_laScnAlignRow[i]	= (*psmf)[WT_SCN_ALIGN_SETTINGS][i][WT_SA_POINT_ROW];
		m_laScnAlignCol[i]	= (*psmf)[WT_SCN_ALIGN_SETTINGS][i][WT_SA_POINT_COL];
	}

	m_bEnableSCNCheck		= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_POS][WT_SCN_CHECK_ENABLE];
	m_bScnCheckByPR			= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_BY_VIS];
	m_bScnCheckIsRefDie		= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_IS_REFDIE];
	m_bScnCheckDieAsRefDie	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_SCNCHECK_DIE_AS_REFDIE];
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		//m_bEnableSCNCheck		= TRUE;
		m_bScnCheckByPR			= TRUE;
		m_bScnCheckIsRefDie		= TRUE;
		m_bScnCheckDieAsRefDie	= FALSE;
	}
	
	m_bErrorChooseGoFPC		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ERROR_CHOOSE_GO_FPC];
	if( pApp->GetCustomerName()!="Huga" )
	{
		m_bErrorChooseGoFPC = FALSE;
	}

	m_lReferMapTotal	= (*psmf)[WT_MAP_NEW_REFER][WT_MAP_NEW_REFER_NUM];
	for (int i=0; i<WT_ALN_MAXCHECK_SCN; i++)
	{
		m_lReferMapRow[i]	= (*psmf)[WT_MAP_NEW_REFER][WT_MAP_NEW_REFER_ROW][i];
		m_lReferMapCol[i]	= (*psmf)[WT_MAP_NEW_REFER][WT_MAP_NEW_REFER_COL][i];
	}

	DEB_CheckEnableFeature(bSaved);	//Silan to use DEB v3.49	//v4.50A25	

	m_bScnLoaded		= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_LOADED];
	m_lSCNIndex_X		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_INDEX_X];
	m_lSCNIndex_Y		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_INDEX_Y];
	m_bKeepSCNAlignDie	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_KEEP_ALIGNDIE];
	m_bCheckSCNLoaded	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_SCN_LOADED];
	m_lScnCheckRefDieNo	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_REFDIE_NO];
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		m_lScnCheckRefDieNo	=	1;
	}
	m_lScnCheckXTol		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_X_TOL];
	m_lScnCheckYTol		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_Y_TOL];
	m_bEnableScnCheckAdjust = (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_ADJ];
	m_lRefHomeDieRow	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_REF_HOME_DIE_ROW];
	m_lRefHomeDieCol	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_REF_HOME_DIE_COL];
	if (m_lSCNIndex_X < 2)
		m_lSCNIndex_X = 2;
	if (m_lSCNIndex_Y < 2)
		m_lSCNIndex_Y = 2;

	CString szCoorChain = "";
	for (int i=1; i<WT_ALN_MAXCHECK_SCN; i++)
	{
		m_lScnCheckRow[i]	= (*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_POS][i][WT_SCN_CHECK_ROW];
		m_lScnCheckCol[i]	= (*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_POS][i][WT_SCN_CHECK_COL];
		if ( i <= m_lTotalSCNCount ) {
			CString szTempWords1, szTempWords2;
			szTempWords1.Format( "%d" , m_lScnCheckRow[i] );
			szTempWords2.Format( "%d" , m_lScnCheckCol[i] );
			szCoorChain += "(";
			szCoorChain += szTempWords1;
			szCoorChain += ",";
			szCoorChain += szTempWords2;
			szCoorChain += ")";
		}
	}
	m_lScnCheckRow[0] = m_lScnHmiRow = m_lScnCheckRow[1];
	m_lScnCheckCol[0] = m_lScnHmiCol = m_lScnCheckCol[1];
	(*m_psmfSRam)["MS896A"]["PointCheckPosition"]			= szCoorChain;

	m_szScanAoiMapFormat= (*psmf)[WT_OPTION][WT_AOI_SCAN_MAP_FORMAT];
	if( m_szScanAoiMapFormat.IsEmpty() )
		m_szScanAoiMapFormat	= _T("none");
	m_szPrescanBackupMapPath	= gszROOT_DIRECTORY + _T("\\OutputFiles\\PrescanBackupMap");
	if ((!((*psmf)[WT_OPTION][WT_SCAN_BACKUP_MAP_PATH])) == FALSE)
	{
		m_szPrescanBackupMapPath= (*psmf)[WT_OPTION][WT_SCAN_BACKUP_MAP_PATH];
	}
	m_bPrescanBackupMap		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_SCAN_BACKUP_MAP_ENABLE];
	(*m_psmfSRam)["MS896A"]["WaferEndBackupMapFilePath"]	= m_szPrescanBackupMapPath;
	m_szScanMA1FilePath = "";
	if((!((*psmf)[WT_OPTION][WT_SCAN_MA1_FILEPATH])) == FALSE)
	{
		m_szScanMA1FilePath= (*psmf)[WT_OPTION][WT_SCAN_MA1_FILEPATH];
	}
	m_dMA1ScanYieldLimit	=   (DOUBLE)(*psmf)[WT_OPTION][WT_SCAN_MA1_YIELD_LIMIT];
	m_bHmiOperationLogOption	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_HMI_OPERATION_LOG_ENABLE];
	m_bHmiOperationLogOption	= TRUE;
	m_szHmiOperationLogPath		= (*psmf)[WT_OPTION][WT_HMI_OPERATION_LOG_PATH];
	if (_access(m_szHmiOperationLogPath,0) == -1)
	{
		m_szHmiOperationLogPath = gszUSER_DIRECTORY + "\\Operation";
		if (_access(m_szHmiOperationLogPath,0) == -1)
		{
			CreateDirectory(m_szHmiOperationLogPath, NULL);
		}
	}

	// stage 3
	m_bSingleHomeRegionScan	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_REGION_SCAN_WITH_HOME_ONLY];
	m_bManualRegionScanSort	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_REGION_SCAN_MANUAL_ALIGN];
	m_bFindAllRefer4Scan	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_FIND_ALL_REFER_4_SCAN];
	m_bPrescanBlkPickAlign	=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_BLKPICK_ALIGN];
	m_ucPrescanMapShapeType	= (UCHAR)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_MAP_SHAPE_TYPE];
	m_bWaferEndUncheckRefer =  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_WAFEREND_UNCHECK_REFER];
	m_bMapOHFlip			=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ORIGINAL_HORI_FLIP];
	m_bMapOVFlip			=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ORIGINAL_VERT_FLIP];
	m_usMapAngle			=(USHORT)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ORIGINAL_ROT_ANGLE];
	m_bRegionBlkPickOrder	=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_REGION_BLKPICK_ORDER];
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	pSRInfo->m_lRegionPickMinLimit	=	(LONG)(*psmf)[WT_ALIGN_DATA][WT_REGION_PICK_MIN_LIMIT];
	m_lFindMapEdgeDieLimit	=		 (LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_AUTO_MAP_LIMIT];
	m_lRegionPickIndex		=		 (LONG)(*psmf)[WT_ALIGN_DATA][WT_REGION_PICK_INDEX];
	for(INT i=0; i<WT_MAX_SUBREGIONS_LIMIT; i++)
	{
		m_lSubRegionState[i]=		 (LONG)(*psmf)[WT_ALIGN_DATA][WT_REGION_STATE_DATA][i];
	}
	// stage 3

	m_lAllReferSpiralLoop	=	(LONG)(*psmf)[WT_OPTION][WT_ALL_REFER_SPIRAL_LOOP];
	m_bRescanLogPrImage		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_LOG_PR_IMAGE];
	m_bPrescanLogNgImage	= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LOG_NG_IMAGE];

	m_bCaptureScanMapImage	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_GRAB_SCAN_MAP_IMAGE];
	m_szSaveMapImagePath = "";
	if ((!((*psmf)[WT_OPTION][WT_GRAB_SAVE_MAP_IMAGE_PATH])) == FALSE)
	{
		m_szSaveMapImagePath = (*psmf)[WT_OPTION][WT_GRAB_SAVE_MAP_IMAGE_PATH];
	}

	m_szScanNgImageLogPath = "";
	if ((!((*psmf)[WT_OPTION][WT_LOG_SCAN_PR_NG_IMAGE_PATH])) == FALSE)
	{
		m_szScanNgImageLogPath = (*psmf)[WT_OPTION][WT_LOG_SCAN_PR_NG_IMAGE_PATH];
	}
	if( m_szScanNgImageLogPath=="" )
	{
		m_szScanNgImageLogPath	= m_szSaveMapImagePath;
	}

	// ES101: generate wafer picture
	m_lTabletPixelX		= (*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_PIXEL_X];
	m_lTabletPixelY		= (*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_PIXEL_Y];
	m_lPngDieExtraEdgeE	= (*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIE_EXTRA_EDGE_E];
	m_lPngDieExtraEdgeY	= (*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIE_EXTRA_EDGE_Y];
	m_lPngDieExtraEdgeN = (*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIE_EXTRA_EDGE_N];
	m_lPngDieExtraEdgeX = (*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIE_EXTRA_EDGE_X];
	m_lPngPixelX		= (*psmf)[WT_PSCAN_OPTIONS][WT_PNG_IMAGE_PIXEL_X];
	m_lPngPixelY		= (*psmf)[WT_PSCAN_OPTIONS][WT_PNG_IMAGE_PIXEL_Y];
	m_dTabletDimInch_X	= (*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIM_X_INCH];
	m_dTabletDimInch_Y	= (*psmf)[WT_PSCAN_OPTIONS][WT_TABLET_DIM_Y_INCH];
	if( m_lTabletPixelX == 0 || m_lTabletPixelY == 0 ||  m_dTabletDimInch_X == 0.0 || m_dTabletDimInch_Y == 0.0 )
	{
		m_lTabletPixelX			= 1920;
		m_lTabletPixelY			= 1200;
		m_dTabletDimInch		= 7.0;
		m_dTabletDimInch_X		= sqrt(7.0*7.0 - 3.71*3.71);
		m_dTabletDimInch_Y		= 3.71;
	}

	if( m_lPngPixelX<m_lTabletPixelX )
	{
		m_lPngPixelX = m_lTabletPixelX;
	}
	if( m_lPngPixelY<m_lTabletPixelY )
	{
		m_lPngPixelY = m_lTabletPixelY;
	}

	if( m_dTabletDimInch_X != 0.0 && m_dTabletDimInch_Y != 0.0)
	{
		m_dTabletDimInch	= sqrt(m_dTabletDimInch_X*m_dTabletDimInch_X + m_dTabletDimInch_Y*m_dTabletDimInch_Y);
		m_dTabletXPPI		= (DOUBLE)m_lTabletPixelX/m_dTabletDimInch_X;
		m_dTabletYPPI		= (DOUBLE)m_lTabletPixelY/m_dTabletDimInch_Y;
	}

	m_bPngEnableGrid		= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_ENABLE];
	m_lPngGridRow			= (*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_ROW];
	m_lPngGridCol			= (*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COL];
	m_lPngGridThick			= (*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_THICK];

	m_bGridColourOn1		= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_1];
	m_bGridColourOn2		= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_2];
	m_bGridColourOn3		= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_3];
	m_bGridColourOn4		= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_4];
	m_bGridColourOn5		= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_ON_5];

	m_ulGridColourArray1	= (ULONG)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_1];
	m_ulGridColourArray2	= (ULONG)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_2];
	m_ulGridColourArray3	= (ULONG)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_3];
	m_ulGridColourArray4	= (ULONG)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_4];
	m_ulGridColourArray5	= (ULONG)(LONG)(*psmf)[WT_PSCAN_OPTIONS][ES_PNG_GRID_COLOUR_5];

	m_ulBoundaryDieColour	= (*psmf)[WT_PSCAN_OPTIONS][WT_BOUND_DIE_COLOUR];	// Boundary die
	m_ulNGDieColourY		= (*psmf)[WT_PSCAN_OPTIONS][WT_NG_DIE_COLOUR_Y];		// NG die	
	m_ulNGDieColourE		= (*psmf)[WT_PSCAN_OPTIONS][WT_NG_DIE_COLOUR_E];		// E NG die	
	m_ulNGDieColourN		= (*psmf)[WT_PSCAN_OPTIONS][WT_NG_DIE_COLOUR_N];	// NG die N
	m_ulNGDieColourX		= (*psmf)[WT_PSCAN_OPTIONS][WT_NG_DIE_COLOUR_X];	// NG die X
	if ( m_ulBoundaryDieColour == 0 || m_ulNGDieColourY == 0 || m_ulNGDieColourN == 0 || m_ulNGDieColourX == 0)
	{
		m_ulBoundaryDieColour	= RGB(255, 255, 255);	// Boundary die
		m_ulNGDieColourY		= RGB(0, 255, 0);		// NG die	
		m_ulNGDieColourN		= RGB(0, 0, 255);		// NG die	N
		m_ulNGDieColourX		= RGB(255, 0, 0);		// NG die	X
	}

	if( m_lPngDieExtraEdgeE < 0 )
	{
		m_lPngDieExtraEdgeE	= 0;
	}
	if( m_lPngDieExtraEdgeY < 0 )
	{
		m_lPngDieExtraEdgeY	= 0;
	}
	if( m_lPngDieExtraEdgeN < 0 )
	{
		m_lPngDieExtraEdgeN	= 0;
	}
	if( m_lPngDieExtraEdgeX < 0 )
	{
		m_lPngDieExtraEdgeX	= 0;
	}
	m_lLFPitchToleranceX	= (*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LF_PITCH_TOL_X];
	m_lLFPitchToleranceY	= (*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LF_PITCH_TOL_Y];
	m_lRescanPELimit		= (*psmf)[WT_PSCAN_OPTIONS][WT_RESCAN_PE_LIMIT];
	m_lLFPitchErrorLimit	= (*psmf)[WT_PSCAN_OPTIONS][WT_LF_PE_LIMIT];

	m_szOutputScanSummaryPath	= gszROOT_DIRECTORY + _T("\\OutputFiles\\PrescanSummary");
	if ((!((*psmf)[WT_OPTION][WT_OUTPUT_SCAN_SUMMARY_PATH])) == FALSE)
	{
		m_szOutputScanSummaryPath= (*psmf)[WT_OPTION][WT_OUTPUT_SCAN_SUMMARY_PATH];
	}

	m_lOutputScanSummary	= (LONG)(*psmf)[WT_OPTION][WT_OUPUT_SCAN_SUMMARY];
	if( DEB_IsEnable() || IsAdvRegionOffset() )
	{
		m_ulPrescanGoToType = 2;
	}
	if( IsAutoRescanOnly() )
	{
		m_ulPrescanGoToType = 0;
	}
}


VOID CWaferTable::CheckMapPrescanAlignWaferOption()
{
	if (m_lMapPrescanAlignWafer == SCAN_ALIGN_WAFER_WITHOUT_REF || m_lMapPrescanAlignWafer == SCAN_ALIGN_WAFER_WITH_REF)
	{
		m_bMapDummyPrescanAlign = TRUE;
	}
	else
	{
		m_bMapDummyPrescanAlign = FALSE;
	}
}

VOID CWaferTable::SavePrescanDataWf(CStringMapFile  *psmf)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="WH" )
	{
		m_bCheckDieOrientation	= TRUE;
	}

	if( pApp->GetCustomerName()!="Soraa" )
	{
		m_bSoraaSortMode	= FALSE;
		m_bDualWayTipWafer	= FALSE;
	}
	CheckMapPrescanAlignWaferOption();

	(*psmf)[WT_OPTION][WT_PRESCAN_ROTATE_DEGREE]	= m_dPrescanRotateDegree;
	(*psmf)[WT_OPTION][WT_SORT_DEFECTIVE_DIE]		= m_bToPickDefectDie;
	(*psmf)[WT_OPTION][WT_DEFECTIVE_DIE_SORT_GRADE]	= m_ucDefectiveDieGrade;
	(*psmf)[WT_OPTION][WT_PSM_FILE_LOADED]			= FALSE;	//	m_bCheckPSMLoaded
	(*psmf)[WT_OPTION][WT_PSM_FILE_EXIST]			= FALSE;	//	m_bCheckPSMExist
	(*psmf)[WT_OPTION][WT_AUTOFOCUS_GRID_SAMPLING]	= m_bAFGridSampling;
	(*psmf)[WT_OPTION][WT_PSM_LOAD_ACTION]			= m_ucPSMLoadAction;
	(*psmf)[WT_OPTION][WT_PRESCAN_PSM_STATE]		= m_bPsmEnableState;
	(*psmf)[WT_OPTION][WT_SMALL_MAP_SORT_RPT]		= m_bSmallMapSortRpt;
	(*psmf)[WT_OPTION][WT_GEN_RPT_OF_SMS]			= m_bGenRptOfSMS;
	(*psmf)[WT_OPTION][WT_ES100_ENABLE_PSM]			= m_bDisablePsm;
	(*psmf)[WT_OPTION][WT_ES100_DISABLE_SCN]		= m_bES100DisableSCN;
	(*psmf)[WT_OPTION][WT_CHECK_DIE_ORIENTATION]	= m_bCheckDieOrientation;
	(*psmf)[WT_OPTION][WT_ES100_MAP_INDEX]			= m_bES100ByMapIndex;
	(*psmf)[WT_OPTION][WT_PRESCAN_PSM_MAP_MC_NO]	= m_szPsmMapMachineNo;
	pUtl->SetPsmMapMcNo(m_szPsmMapMachineNo);
	(*psmf)[WT_OPTION][WT_PRESCAN_NORMAL3_ATHOME]	= m_bPrescanNml3AtHomeDie;
	(*psmf)[WT_OPTION][WT_PRESCAN_AUTO_DIEPITCH]	= m_bPrescanAutoDiePitch;
	(*psmf)[WT_OPTION][WT_PRESCAN_DUMMY_AUTO_LIMIT] = m_bAutoSearchWaferLimit;
	(*psmf)[WT_OPTION][WT_PRESCAN_FIX_MAP_CENTER]	= m_bFixDummyMapCenter;
	(*psmf)[WT_OPTION][WT_BLK2_ALIGN_HMIUSE_CHAR]	= m_bHmiToUseCharDie;
	(*psmf)[WT_OPTION][SORRA_SORT_MODE_ENABLE]		= m_bSoraaSortMode;
	(*psmf)[WT_OPTION][PRESCAN_DUAL_WAY_TIP_WAFER]	= m_bDualWayTipWafer;
	(*psmf)[WT_OPTION][WT_MAP_DUMMY_SCAN_ALIGN]		= m_lMapPrescanAlignWafer;
	//(*psmf)[WT_OPTION][WT_MAP_DUMMY_SCAN_ALIGN]		= m_bMapDummyPrescanAlign;
	(*psmf)[WT_OPTION][WT_MAP_DUMMY_SCAN_CORNER_CHECK]	= m_bMapDummyScanCornerCheck;
	(*psmf)[WT_OPTION][WT_BLK_4REFER_CHECKSCORE]	= m_lBlk4ReferCheckScore;
	if( IsMS90()==FALSE )
	{
		m_bMS90HalfSortMode = FALSE;
	}
	(*psmf)[WT_OPTION][MS90_HALF_SORT_MODE_ENABLE]	= m_bMS90HalfSortMode;
	(*psmf)[WT_OPTION][MS90_SCAN_DISPLAY_SELECT]	= m_bMS90DisplayAndSelect;
	(*psmf)[WT_OPTION][MS90_SCAN_DISPLAY_WHOLE]		= m_bMS90DisplayWholeOnly;
	(*psmf)[WT_MS90_HOME_OPTIONS][WT_MS90_HOME_2ND_ROW]			= m_lMS902ndHomeDieMapRow;
	(*psmf)[WT_MS90_HOME_OPTIONS][WT_MS90_HOME_2ND_COL]			= m_lMS902ndHomeDieMapCol;
	(*psmf)[WT_MS90_HOME_OPTIONS][WT_MS90_HOME_2ND_OFFSET_ROW]	= m_lMS902ndHomeOffsetRow;
	(*psmf)[WT_MS90_HOME_OPTIONS][WT_MS90_HOME_2ND_OFFSET_COL]	= m_lMS902ndHomeOffsetCol;

	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_INDEX_STEP_ROW]	= m_nPrescanIndexStepRow;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_INDEX_STEP_COL]	= m_nPrescanIndexStepCol;
	(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_MAP_INDEX_LOOP]	= m_ulMapIndexLoop;
	(*psmf)[WT_BLOCK][WT_EMPTY_GRADE_MIN_COUNT]	= m_lEmptyGradeMinCount;

	(*psmf)[WT_OPTION][WT_ALARM_RED_BUZZ_TWICE]			= m_bAlarmTwice;

	(*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_MODE]		= 	m_ucCornerPatternCheckMode;
	(*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_PR_ID]	= 	m_ucCornerReferPrID;
	(*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_ALL]		= 	m_bCornerPatternCheckAll;
	(*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_DIST_X]	= 	m_lReferToCornerDistX;
	(*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_DIST_Y]	= 	m_lReferToCornerDistY;

	if (m_lMapPrescanAlignWafer > 0)
	{
		m_bScanWithWaferLimit = TRUE;
	}
	else
	{
		m_bScanWithWaferLimit = FALSE;
	}
}


VOID CWaferTable::LoadPrescanDataWf(CStringMapFile  *psmf)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( m_lReadFromWFT<2 )	// stage 2
	{
		// prescan relative settings
		m_bEnablePrescan		=  (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_ENABLED];
		pUtl->SetPrescanEnable(IsPrescanEnable());
		m_dPrescanGoodPassScore	=	  (DOUBLE)(*psmf)[WT_OPTION][WT_PRESCAN_GOOD_PASSSCORE];
		m_bPrescanCheckBadCut	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_CHECK_BADCUT];
		m_bPrescanCheckDefect	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_CHECK_DEFECT];

		m_bEnableSCNCheck	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_POS][WT_SCN_CHECK_ENABLE];
		m_bScnCheckByPR		= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_BY_VIS];
		m_bScnCheckIsRefDie	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_IS_REFDIE];
		m_bScnCheckDieAsRefDie	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_SCNCHECK_DIE_AS_REFDIE];
		m_lScnCheckRefDieNo	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_REFDIE_NO];
		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			//m_bEnableSCNCheck		= TRUE;
			m_bScnCheckByPR			= TRUE;
			m_bScnCheckIsRefDie		= TRUE;
			m_bScnCheckDieAsRefDie	= FALSE;
			m_lScnCheckRefDieNo		= 1;
		}
		m_bErrorChooseGoFPC		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ERROR_CHOOSE_GO_FPC];
		if( pApp->GetCustomerName()!="Huga" )
		{
			m_bErrorChooseGoFPC = FALSE;
		}
		m_bKeepSCNAlignDie	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_KEEP_ALIGNDIE];
		m_bCheckSCNLoaded	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_SCN_LOADED];
		m_lScnCheckXTol		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_X_TOL];
		m_lScnCheckYTol		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_Y_TOL];
		m_bEnableScnCheckAdjust = (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_ADJ];
		m_lRefHomeDieRow	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_REF_HOME_DIE_ROW];
		m_lRefHomeDieCol	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_REF_HOME_DIE_COL];

		CString szCoorChain = "";
		for (int i=1; i<WT_ALN_MAXCHECK_SCN; i++)
		{
			m_lScnCheckRow[i]	= (*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_POS][i][WT_SCN_CHECK_ROW];
			m_lScnCheckCol[i]	= (*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_POS][i][WT_SCN_CHECK_COL];
			if ( i <= m_lTotalSCNCount ) {
				CString szTempWords1, szTempWords2;
				szTempWords1.Format( "%d" , m_lScnCheckRow[i] );
				szTempWords2.Format( "%d" , m_lScnCheckCol[i] );
				szCoorChain += "(";
				szCoorChain += szTempWords1;
				szCoorChain += ",";
				szCoorChain += szTempWords2;
				szCoorChain += ")";
			}
		}
		m_lScnHmiRow = m_lScnCheckRow[1];
		m_lScnHmiCol = m_lScnCheckCol[1];
		(*m_psmfSRam)["MS896A"]["PointCheckPosition"]			= szCoorChain;
	}

	m_dPrescanRotateDegree	=	  (DOUBLE)(*psmf)[WT_OPTION][WT_PRESCAN_ROTATE_DEGREE];
	m_lMapPrescanAlignWafer = (LONG)(*psmf)[WT_OPTION][WT_MAP_DUMMY_SCAN_ALIGN];
	//m_bMapDummyPrescanAlign	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_DUMMY_SCAN_ALIGN];
	m_bMapDummyScanCornerCheck	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_DUMMY_SCAN_CORNER_CHECK];
	m_bToPickDefectDie		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_SORT_DEFECTIVE_DIE];
	m_ucDefectiveDieGrade	=(UCHAR)(LONG)(*psmf)[WT_OPTION][WT_DEFECTIVE_DIE_SORT_GRADE];
	m_bPrescanNml3AtHomeDie	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_NORMAL3_ATHOME];
	m_bPsmEnableState		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_PSM_STATE];
	m_bSmallMapSortRpt		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_SMALL_MAP_SORT_RPT];
	m_bGenRptOfSMS			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_GEN_RPT_OF_SMS];
	m_bDisablePsm			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ES100_ENABLE_PSM];
	m_bES100DisableSCN		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ES100_DISABLE_SCN];
	m_bCheckDieOrientation	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_CHECK_DIE_ORIENTATION];	
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//m_bCheckDieOrientation	= TRUE; //Default value to TRUE by Matthew 20190410
	}
	m_bES100ByMapIndex		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ES100_MAP_INDEX];
	m_bCheckPSMLoaded		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PSM_FILE_LOADED];
	m_bCheckPSMExist		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PSM_FILE_EXIST];
	m_ucPSMLoadAction		= (UCHAR)(LONG)(*psmf)[WT_OPTION][WT_PSM_LOAD_ACTION];
	m_bAFGridSampling		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_AUTOFOCUS_GRID_SAMPLING];
	if( m_bCheckPSMExist )
		m_ucPSMLoadAction = 2;
	if( m_bCheckPSMLoaded )
		m_ucPSMLoadAction = 3;
	m_szPsmMapMachineNo		= (*psmf)[WT_OPTION][WT_PRESCAN_PSM_MAP_MC_NO];
	pUtl->SetPsmMapMcNo(m_szPsmMapMachineNo);
	m_bPrescanAutoDiePitch	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_AUTO_DIEPITCH];
	m_bAutoSearchWaferLimit	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_DUMMY_AUTO_LIMIT];
	m_bFixDummyMapCenter	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRESCAN_FIX_MAP_CENTER];
	m_bHmiToUseCharDie		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_BLK2_ALIGN_HMIUSE_CHAR];
	m_bAlarmTwice			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ALARM_RED_BUZZ_TWICE];
	m_bSoraaSortMode		= (BOOL)(LONG)(*psmf)[WT_OPTION][SORRA_SORT_MODE_ENABLE];
	m_bDualWayTipWafer		= (BOOL)(LONG)(*psmf)[WT_OPTION][PRESCAN_DUAL_WAY_TIP_WAFER];
	m_lBlk4ReferCheckScore	= (LONG)(*psmf)[WT_OPTION][WT_BLK_4REFER_CHECKSCORE];
	m_lEmptyGradeMinCount	= (*psmf)[WT_BLOCK][WT_EMPTY_GRADE_MIN_COUNT];
	m_nPrescanIndexStepRow	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_INDEX_STEP_ROW];
	m_nPrescanIndexStepCol	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_INDEX_STEP_COL];
	m_ulMapIndexLoop		= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_PRESCAN_MAP_INDEX_LOOP];
	m_bMS90HalfSortMode		= (BOOL)(LONG)(*psmf)[WT_OPTION][MS90_HALF_SORT_MODE_ENABLE];
	m_bMS90DisplayAndSelect	= (BOOL)(LONG)(*psmf)[WT_OPTION][MS90_SCAN_DISPLAY_SELECT];
	m_bMS90DisplayWholeOnly	= (BOOL)(LONG)(*psmf)[WT_OPTION][MS90_SCAN_DISPLAY_WHOLE];
	if( IsMS90()==FALSE )
	{
		m_bMS90HalfSortMode = FALSE;
	}
	m_lMS902ndHomeDieMapRow		= (LONG)(*psmf)[WT_MS90_HOME_OPTIONS][WT_MS90_HOME_2ND_ROW];		
	m_lMS902ndHomeDieMapCol		= (LONG)(*psmf)[WT_MS90_HOME_OPTIONS][WT_MS90_HOME_2ND_COL];		
	m_lMS902ndHomeOffsetRow		= (LONG)(*psmf)[WT_MS90_HOME_OPTIONS][WT_MS90_HOME_2ND_OFFSET_ROW];
	m_lMS902ndHomeOffsetCol		= (LONG)(*psmf)[WT_MS90_HOME_OPTIONS][WT_MS90_HOME_2ND_OFFSET_COL];

	m_ucCornerPatternCheckMode	= (UCHAR)(LONG)(*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_MODE];
	m_ucCornerReferPrID			= (UCHAR)(LONG)(*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_PR_ID];
	m_bCornerPatternCheckAll	= (BOOL)(LONG)(*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_ALL];
	m_lReferToCornerDistX		= (*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_DIST_X];
	m_lReferToCornerDistY		= (*psmf)[WT_PSCAN_OPTIONS][WT_CORNER_PATTERN_CHECK_DIST_Y];

	if( m_ulMapIndexLoop==0 )
		m_ulMapIndexLoop = 1;

	if( pApp->GetCustomerName()!="Soraa" )
	{
		m_bSoraaSortMode	= FALSE;
		m_bDualWayTipWafer	= FALSE;
	}

	CheckMapPrescanAlignWaferOption();

	m_bRegionPrescan	= pUtl->GetPrescanRegionMode();

	if( pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="WH" )
	{
		m_bCheckDieOrientation	= TRUE;
	}
}

LONG CWaferTable::PrescanAllDoneOnWft()	// for prescan only
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG	lContinueState = 0;

	if( IsPrescanBlkPick() )
		SetPrescanBlkFuncEnable(FALSE);	// after prescan, block pick alignment only, disable it.

	StartLoadingAlert();	// stop/close by wafer table

	m_bThisDieNeedVerify	= TRUE;
	BOOL bNeedGoCheck = TRUE;
	if( pUtl->GetPrescanRegionMode() )
	{
		SaveScanTimeEvent("    WFT: region to check first time or not");
		if (pSRInfo->HasNoRegionBonded() || pSRInfo->IsManualAlignRegion())
		{
			m_ulPrescanGoCheckCount = 0;
		}
		else
		{
			bNeedGoCheck = FALSE;
		}
	}
	else
	{
		m_ulPrescanGoCheckCount = 0;
	}

	m_aUIGoCheckRow.RemoveAll();
	m_aUIGoCheckCol.RemoveAll();
	LONG lAlignRow, lAlignCol, lAlignX, lAlignY;
	pUtl->GetAlignPosition(lAlignRow, lAlignCol, lAlignX, lAlignY);

	ObtainMapValidRange();	// prescan all done
	//	rescan get rescan area begin
	m_lScannedWaferUL_X = lAlignX;
	m_lScannedWaferUL_Y = lAlignY;
	m_lScannedWaferLR_X = lAlignX;
	m_lScannedWaferLR_Y = lAlignY;
	m_lScannedMapMinRow = GetMapValidMinRow();
	m_lScannedMapMaxRow = GetMapValidMaxRow();
	m_lScannedMapMinCol = GetMapValidMinCol();
	m_lScannedMapMaxCol = GetMapValidMaxCol();
	if( pUtl->GetPrescanRegionMode() )
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
		ULONG lULRow, lULCol, lLRRow, lLRCol;
		if( pSRInfo->GetRegion(ulTgtRegion, lULRow, lULCol, lLRRow, lLRCol) )
		{
			m_lScannedMapMinRow = max((LONG)lULRow,		m_lScannedMapMinRow);
			m_lScannedMapMaxRow = min((LONG)lLRRow - 1, m_lScannedMapMaxRow);
			m_lScannedMapMinCol = max((LONG)lULCol,		m_lScannedMapMinCol);
			m_lScannedMapMaxCol = min((LONG)lLRCol - 1, m_lScannedMapMaxCol);
		}
	}

	unsigned long ulDieState;
	for(ULONG ulRow = (ULONG)m_lScannedMapMinRow; ulRow <= (ULONG)m_lScannedMapMaxRow; ulRow++)
	{
		for(ULONG ulCol = (ULONG)m_lScannedMapMinCol; ulCol <= (ULONG)m_lScannedMapMaxCol; ulCol++)
		{
			LONG lPhyX = 0, lPhyY = 0;
			if (GetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY))
			{
				if (m_lScannedWaferUL_Y < lPhyY)
				{
					m_lScannedWaferUL_Y = lPhyY;
				}

				if (m_lScannedWaferLR_Y > lPhyY)
				{
					m_lScannedWaferLR_Y = lPhyY;
				}

				if (m_lScannedWaferUL_X < lPhyX)
				{
					m_lScannedWaferUL_X = lPhyX;
				}

				if (m_lScannedWaferLR_X > lPhyX)
				{
					m_lScannedWaferLR_X = lPhyX;
				}
			}
			if( IsOutMS90SortingPart(ulRow, ulCol) )
            {
                  continue;
            }
		}
	}

	m_lScannedWaferUL_X += GetPrescanPitchX();
	m_lScannedWaferLR_X -= GetPrescanPitchX();
	m_lScannedWaferUL_Y += GetPrescanPitchY();
	m_lScannedWaferLR_Y -= GetPrescanPitchY();

	CString szTemp;
	szTemp.Format("    WFT: scanned map   %d,%d,%d,%d", m_lScannedMapMinRow, m_lScannedMapMinCol, m_lScannedMapMaxRow, m_lScannedMapMaxCol);
	SaveScanTimeEvent(szTemp);
	szTemp.Format("    WFT: scanned Wafer %d,%d,%d,%d", m_lScannedWaferUL_X, m_lScannedWaferUL_Y, m_lScannedWaferLR_X, m_lScannedWaferLR_Y);
	SaveScanTimeEvent(szTemp);
	//	rescan get rescan area end

	m_ulPdcRegionSampleCounter	= 0;
	m_ulLastRscnPickCount = 0;
	m_ulNextAdvSampleCount	= m_ulAdvStage1StartCount;
	m_ulNextRescanCount	= m_ulRescanStartCount;
	if( IsAdvRegionOffset() )
	{
		SaveScanTimeEvent("    WFT: adv offset init");
		pRgnPdc->SetLastDieData(lAlignRow, lAlignCol, 0, 0);
		InitAdvRegionPredication();
	}

	if( pUtl->GetPrescanDummyMap() && pUtl->GetPrescanAoiWafer()==FALSE )
	{
		m_WaferMapWrapper.SetReferenceDie(m_ulAlignRow, m_ulAlignCol, FALSE);
	}

	if (!pSRInfo->IsManualAlignRegion() && IsScanAlignWafer() )
	{
		m_ulAlignRow = m_nPrescanAlignMapRow	= lAlignRow;
		m_ulAlignCol = m_nPrescanAlignMapCol	= lAlignCol;
		m_lStart_X	= m_nPrescanAlignPosnX	= lAlignX;
		m_lStart_Y	= m_nPrescanAlignPosnY	= lAlignY;

		m_WaferMapWrapper.SetStartPosition(lAlignRow, lAlignCol);
		DisplayNextDirection(lAlignRow, lAlignCol);

		if( m_bEnableAlignWaferImageLog )
		{
			(*m_psmfSRam)["MS896A"]["WaferMapRow"] = m_ulAlignRow;
			(*m_psmfSRam)["MS896A"]["WaferMapCol"] = m_ulAlignCol;
			(*m_psmfSRam)["MS896A"]["WaferTableX"] = m_lStart_X;
			(*m_psmfSRam)["MS896A"]["WaferTableY"] = m_lStart_Y;
			GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_MAN);
		}
	}

	m_bPrescanPosnOK	= FALSE;
	m_bMapPositionOK	= FALSE;
	m_lGetAtcWftPosX	= 0;
	m_lGetAtcWftPosY	= 0;
	m_lGetRgnWftPosX	= 0;
	m_lGetRgnWftPosY	= 0;

	if( m_bEnableAlignWaferImageLog )
	{
		SaveScanTimeEvent("    WFT: to zoom map viewer");
		m_WaferMapWrapper.SetZoomFactor(1);
	}

	if( pUtl->GetPrescanRegionMode()==FALSE )
		SetupSubRegionMode();	// prescan all done

	SaveData();

	if( IsAdvRegionOffset() )
		pRgnPdc->InitRegionOffset();

	BOOL bContinue = TRUE;

	m_ulReverifyReferRegion	= pSRInfo->GetTargetRegion();
	if( PrescanRealignWafer()==FALSE )
	{
		CString szMsg;
		szMsg.Format("Realignment fail after region %lu prescan!\nNo scan die in this region\n\nContinue sort this region?", m_ulReverifyReferRegion);
		SetErrorMessage(szMsg);
		pUtl->RegionOrderLog(szMsg);
		if( HmiMessage_Red_Back(szMsg, "Region Prescan", glHMI_MBX_YESNO)==glHMI_YES)
		{
			SetErrorMessage("user select to continue");
			pUtl->RegionOrderLog("user select to continue");
		}
		else
		{
			szMsg = "You need to set this region to bonded or manually align it";
			HmiMessage_Red_Back(szMsg);
			SetErrorMessage(szMsg);
			pUtl->RegionOrderLog(szMsg);
			m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_REALIGN_FAIL); // PRESCAN REALIGN FAIL
			bContinue = FALSE;
		}
	}

	if( pUtl->GetPrescanRegionMode()==FALSE )
	{
		KeepPrescanAlignDie();
	}

	CloseLoadingAlert();	// start/open by wafer pr station

	// grab save pr, screen and data info for Tong Hui
	if( m_bEnableAlignWaferImageLog )
	{
		SaveScanTimeEvent("    WFT: to grab and save PR image");
		(*m_psmfSRam)["MS896A"]["WaferTableX"] = lAlignX;
		(*m_psmfSRam)["MS896A"]["WaferTableY"] = lAlignY;
		(*m_psmfSRam)["MS896A"]["WaferMapRow"] = lAlignRow;
		(*m_psmfSRam)["MS896A"]["WaferMapCol"] = lAlignCol;
		GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_SCN);	// after prescan and realign 
	}

	//Use 5-pt-check fcn from scanner mode for prescan result checking 
	INT nWprDoFPC = pApp->GetProfileInt(gszPROFILE_SETTING, gszWprDoFPC, 0);
	if( IsEnableFPC() && (pUtl->GetPrescanDummyMap()==FALSE ) && nWprDoFPC==0)
	{
		SaveScanTimeEvent("    WFT: to do five point check");
		if ( m_bScnCheckByPR/* && m_bScnCheckIsRefDie*/ )
		{
			BOOL bNormal = FALSE;
			IPC_CServiceMessage stMsg;
			stMsg.InitMessage(sizeof(BOOL), &bNormal);
			int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "PrAutoBondScreenUpdate", stMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			ChangeCameraToWaferBondMode(FALSE);
		}

		if (PrescanGoodDiePercentageCheck() == FALSE && !pSRInfo->IsManualAlignRegion())
		{
			m_lPrescanVerifyResult += 2048; // Prescan Good Die Percentage check
			bContinue = FALSE;
		}

		BOOL bCheckDieResult = FALSE;
		if ((pSRInfo->IsManualAlignRegion() && pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="XA") || !bContinue)
		{
			bCheckDieResult = TRUE;
		}
		else
		{
			bCheckDieResult = FivePointCheckDiePosition();	//	prescan all done
		}

		if( pApp->GetCustomerName()==CTM_SEMITEK && pUtl->GetPrescanRegionMode()==FALSE && m_bManualAddReferPoints==FALSE )	//	427TX	4
		{
			CString szTitle, szText;
			szTitle.LoadString(HMB_WT_CHECK_SCN_FUNC);
			if( bCheckDieResult )
				szText.Format("Five points checking complete successfully!");
			else
				szText.Format("Five points checking failure!");
			if( GetScanAction()!=PRESCAN_FINISHED_SORT_BY_GRADE || !bCheckDieResult)
			{
				HmiMessage_Red_Back(szText, szTitle);
			}
		}

		if ( m_bScnCheckByPR /*&& m_bScnCheckIsRefDie */)
		{
			BOOL bNormal = TRUE;
			IPC_CServiceMessage stMsg;
			stMsg.InitMessage(sizeof(BOOL), &bNormal);
			int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "PrAutoBondScreenUpdate", stMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			ChangeCameraToWaferBondMode(TRUE);
		}
		if( bCheckDieResult!=TRUE )
		{
			m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_FPC);	 // five point check
			SetErrorMessage("prescan five point check fail");
			bContinue = FALSE;
		}
	}

	if( WaferCornerPatternCheck()!=TRUE )
	{
		SetErrorMessage("Prescan corner pattern check fail.");
		m_lPrescanVerifyResult += 32768; // corner pattern check
		bContinue = FALSE;
	}

	if( EmptyDieGradeCheck()!=TRUE )
	{
		SetErrorMessage("prescan empty die check fail");
		m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_EMPTY_DIE_CHECK); // empty die check
		bContinue = FALSE;
	}

	if (!pSRInfo->IsManualAlignRegion() && IsBlkPickAlign() && pUtl->GetPrescanRegionMode() )
	{
		SaveScanTimeEvent("    WFT: REGN after prescan reverify refer die");
		pUtl->RegionOrderLog("REGN after prescan reverify refer die");
		if( ReverifyBlockPickReferDie()!=TRUE )
		{
			SetErrorMessage("prescan reverify block refer die fail");
			m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_BLOCK_REFER_CHECK);	// reverify block pick refer die after prescan
			bContinue = FALSE;
			pUtl->RegionOrderLog("REGN reverify refer die fail, can not run");
		}
		pUtl->RegionOrderLog("REGN after prescan reverify refer die complete");
	}

	CString szAlgorithm, szPathFinder;
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
	if( DEB_IsEnable() )
	{
		SaveScanTimeEvent("    WFT: deb relative update and key die");
		DEB_UpdateAllDicePos();
		if( DEB_CheckKeyDicesOnWafer()==FALSE )
		{
			if( HmiMessage_Red_Back("Do you want to continue sorting", "Prescan", glHMI_MBX_YESNO)==glHMI_NO )
			{
				SetErrorMessage("prescan ATC check key die fail");
				m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_KEY_DIE); // PRESCAN CHEC KEY DIE
				bContinue = FALSE;
			}
		}

		if( DEB_IsUseable() )
		{
			//if( IsAutoSampleSetupOK() && szAlgorithm.Find("Sorting Path 1")!=-1 )
			//{
			//	m_WaferMapWrapper.SetAlgorithmParameter("DEB PreScan Finish", TRUE);
			//	m_WaferMapWrapper.SetAlgorithmParameter("PutKeydieBackward", TRUE);
			//	for(int i=0; i<m_unNumOfKeyDie; i++)
			//	{
			//		if( m_paDebKeyDie[i].iStatus == MS_Align )
			//		{
			//			ULONG ulRow = m_paDebKeyDie[i].uiRow;
			//			ULONG ulCol = m_paDebKeyDie[i].uiCol;
			//			if( m_WaferMapWrapper.GetReader()!=NULL )
			//			{
			//				m_WaferMapWrapper.GetReader()->SetSpecialFlag(ulRow, ulCol, DIE_SPECIAL_KEYDIE);
			//				CString szMsg;
			//				LONG lUserRow = 0, lUserCol = 0;
			//				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
			//				szMsg.Format("    WFT: list key die grade,%4d,%4d,%4d,%4d,%d",
			//					ulRow, ulCol, lUserRow, lUserCol, m_WaferMapWrapper.GetGrade(ulRow, ulCol)-m_WaferMapWrapper.GetGradeOffset());
			//				CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
			//			}
			//		}
			//	}
			//	SaveScanTimeEvent("    WFT: need to sample DEB key die, prepare to rebuuild map path");
			//	m_lBuildShortPathInScanning = 0;	// scan done, need to sample on key die, so rebuild path. with resumen lock
			//}

			//SaveScanTimeEvent("    WFT: DEB now usable");
			//SetErrorMessage("DEB model init success");
		}
		else
		{
			SaveScanTimeEvent("    WFT: DEB init failure");
			SetErrorMessage("DEB model init error or not enable");
		}
	}

	SavePrescanInfo();
	SavePrescanInfoMap(1);

	if( RectWaferCornerCheck()==FALSE )
	{
		SetErrorMessage("prescan corner check fail");
		m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_RECT_CORNER);	// react wafer corner check
		bContinue = FALSE;
	}

	SaveScanTimeEvent("    WFT: to check prescan result pass score");
	m_bScanLowYield = FALSE;
	if (PrescanResultVerify() == FALSE)
	{
		SetErrorMessage("prescan result pass score fail");
		m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_PASSSCORE);	// prescan result pass score check
		SaveScanTimeEvent("    WFT: No Continue 4");
		bContinue = FALSE;
	}

	if (pApp->GetCustomerName() == CTM_SANAN)
	{
		LONG lResult = m_lPrescanVerifyResult;
		lResult = lResult%4096;
		if (lResult >= 2048)
		{
			m_bScanLowYield = TRUE;
			if (m_bOfflineUploadScanFile)
			{
				m_bToUploadScanMapFile = FALSE;
			}
		}
	}

	if( m_bPrescanDiePitchCheck )
	{
		SaveScanTimeEvent("    WFT: prescan check neighbour die pitch begin");
		ULONG ulNumRow = 0, ulNumCol = 0, i = 0;
		ULONG ulRow = 0, ulCol = 0, ulCheckRow[4], ulCheckCol[4];
		LONG lWftX = 0, lWftY = 0, lCheckX[4], lCheckY[4];
		LONG lToleranceX, lToleranceY;
		m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);
		BOOL bCheckOK = TRUE;
		CString szMsg;
		for(ulRow=0; ulRow<=ulNumRow; ulRow++)
		{
			for(ulCol=0; ulCol<=ulNumCol; ulCol++)
			{
				if( GetPrescanWftPosn(ulRow, ulCol, lWftX, lWftY) )
				{
					ulCheckRow[0] = ulRow;
					ulCheckCol[0] = ulCol+1;
					ulCheckRow[1] = ulRow+1;
					ulCheckCol[1] = ulCol+1;
					ulCheckRow[2] = ulRow+1;
					ulCheckCol[2] = ulCol;
					ulCheckRow[3] = ulRow+1;
					if( ulCol>0 )
						ulCheckCol[3] = ulCol-1;
					else
						ulCheckCol[3] = ulCol;
					for(i=0; i<4; i++)
					{
						if( GetPrescanWftPosn(ulCheckRow[i], ulCheckCol[i], lCheckX[i], lCheckY[i]) )
						{
							lToleranceX = GetDiePitchX_X()*(labs(ulCheckCol[i]-ulCol)*100 + 75)/100;
							if( labs(lWftX-lCheckX[i])>labs(lToleranceX) )
							{
								bCheckOK = FALSE;
								szMsg.Format("X die pitch over at %lu,%lu (%lu,%lu)!", 
									ulCheckRow[i], ulCheckCol[i], ulRow, ulCol);
								SetErrorMessage(szMsg);
								szMsg.Format("WFT X %ld, Check %ld, %ld(%ld)", 
									lWftX, lCheckX[i], lToleranceX, GetDiePitchX_X());
								SetErrorMessage(szMsg);
								break;
							}
							lToleranceY = GetDiePitchY_Y()*(labs(ulCheckRow[i]-ulRow)*100 + 75)/100;
							if( labs(lWftY-lCheckY[i])>labs(lToleranceY) )
							{
								bCheckOK = FALSE;
								szMsg.Format("Y die pitch over at %lu,%lu (%lu,%lu)!",
									ulCheckRow[i], ulCheckCol[i], ulRow, ulCol);
								SetErrorMessage(szMsg);
								szMsg.Format("WFT Y %ld, Check %ld, %ld(%ld)", 
									lWftY, lCheckY[i], lToleranceY, GetDiePitchY_Y());
								SetErrorMessage(szMsg);
								break;
							}
						}
					}
				}
				if( bCheckOK==FALSE )
				{
					break;
				}
			}
			if( bCheckOK==FALSE )
			{
				break;
			}
		}
		SaveScanTimeEvent("    WFT: prescan check neighbour die pitch complete");
		if( bCheckOK==FALSE )
		{
			LONG lHmiRow = 0, lHmiCol = 0;
			ConvertAsmToHmiUser(ulRow, ulCol, lHmiRow, lHmiCol);
			szMsg.Format("Prescan neighbour die pitch over 0.75 at map row %ld, column %ld, Please check!", lHmiRow, lHmiCol);
			HmiMessage_Red_Back(szMsg, "Prescan");
			szMsg.Format("Prescan neighbour die pitch over 0.75 at %ld,%ld (%lu,%lu)!", lHmiRow, lHmiCol, ulRow, ulCol);
			SetErrorMessage(szMsg);
			m_lPrescanVerifyResult += 1024;	// prescan neighbour die pitch over 50% tolerance
		}
	}

	SetPsmEnable(TRUE);

	DOUBLE dUsedTime = GetTime();
	if( OpCheckMA1ToMA3() )
	{
		OpSavePrescanMap();
	}
	else
	{
		SaveScanTimeEvent("    WFT: No Continue 2");
		bContinue = FALSE;
		m_lPresortWftStatus += 2;
	}
	CString szMsg;
	m_lTimeSlot[10] = (LONG)(GetTime()-dUsedTime);			//	10.	to save all update map file/psm/scn and transfer to server.

	if( pUtl->GetPrescanDummyMap()==FALSE )
	{
		SaveScanTimeEvent("    WFT: to Update die status list");
		UpdateDieStateSummary();	// after prescan
	}

	m_bPrescanMapDisplay = true;

	UpdateBackLightDownState();
	if( (IsBLInUse()) && m_lBackLightZStatus==3 )	//	4.24TX 4
	{
		LONG lTimeDiff = (LONG)(GetTime()-m_dBackLightZDownTime);
		if( lTimeDiff<m_lBackLightTravelTime )
		{
			lTimeDiff = m_lBackLightTravelTime - lTimeDiff;
			Sleep(lTimeDiff);
		}
		m_lBackLightZStatus = 0;
		SaveScanTimeEvent("    WFT: BLZ down to standby by sleep");
	}
	else if( IsBLInUse() )
	{
		SaveScanTimeEvent("    WFT: down BL when all done");
		if( MoveES101BackLightZUpDn(FALSE)==FALSE )	//	prescan all done, down back light z
		{
			bContinue = FALSE;	// prescan all donw, DOWN BL Z
			m_lPresortWftStatus += 4;
		}
	}


	if( m_lPrescanVerifyResult>0 || m_lPresortWftStatus>0 )
	{
		SaveScanTimeEvent("    WFT: No Continue 1");
		bContinue = FALSE;
	}

	CString szLogMsg;

	if( pApp->GetCustomerName()=="CMLT" )
	{
		CString szOpId	= _T("");
		LONG lAlarmStatus = GetAlarmLamp_Status();

		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
		while( 1 )
		{
			szOpId	= _T("");
			CString szTitle		= "Please input your ID: ";
			BOOL bReturn = HmiStrInputKeyboard(szTitle, szOpId);
			if( bReturn && szOpId.IsEmpty()!=TRUE )
			{
				break;
			}
		}
		SetAlarmLamp_Back(lAlarmStatus, FALSE, TRUE);
		szLogMsg.Format("After prescan CMLT OP ID intput:%s", szOpId);
		SetErrorMessage(szLogMsg);
	}

	m_szMapServerFullPath.Empty();

	// pick NG
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();

	if( IsScanNgPick() )
	{
		UCHAR ucNgDefectGrade = GetScanDefectGrade() + ucOffset;
		if( m_bPrescanDefectToNullBin )
			ucNgDefectGrade = ucNullBin;
		UCHAR ucNgBadcutGrade = GetScanBadcutGrade() + ucOffset;
		if( m_bPrescanBadCutToNullBin )
			ucNgBadcutGrade = ucNullBin;

		UCHAR ucNgEdgeDieGrade = ucNullBin;
		if( m_ucPrescanEdgeSize>0 && m_ucPrescanEdgeGrade>0 )
			ucNgEdgeDieGrade = m_ucPrescanEdgeGrade + ucOffset;

		UCHAR aaTempGrades[5];
		UCHAR ucTotal = 0;
		if( m_bNgPickDefect && ucNgDefectGrade!=ucNullBin )
		{
			aaTempGrades[ucTotal] = ucNgDefectGrade;
			ucTotal++;
		}
		if( m_bNgPickBadCut && ucNgBadcutGrade!=ucNullBin )
		{
			BOOL bRepeatGrade = FALSE;
			for(UCHAR i=0; i<ucTotal; i++)
			{
				if( aaTempGrades[i]==ucNgBadcutGrade )
				{
					bRepeatGrade = TRUE;
					break;
				}
			}
			if( bRepeatGrade==FALSE )
			{
				aaTempGrades[ucTotal] = ucNgBadcutGrade;
				ucTotal++;
			}
		}
		if( m_bNgPickEdgeDie && ucNgEdgeDieGrade!=ucNullBin )
		{
			BOOL bRepeatGrade = FALSE;
			for(UCHAR i=0; i<ucTotal; i++)
			{
				if( aaTempGrades[i]==ucNgEdgeDieGrade )
				{
					bRepeatGrade = TRUE;
					break;
				}
			}
			if( bRepeatGrade==FALSE )
			{
				aaTempGrades[ucTotal] = ucNgEdgeDieGrade;
				ucTotal++;
			}
		}

		UCHAR ucExtraGrade  = m_ucScanExtraGrade + ucOffset;
		if( m_bNgPickExtra && m_bScanExtraDieAction && ucExtraGrade!=ucNullBin )
		{
			BOOL bRepeatGrade = FALSE;
			for(UCHAR i=0; i<ucTotal; i++)
			{
				if( aaTempGrades[i]==ucExtraGrade )
				{
					bRepeatGrade = TRUE;
					break;
				}
			}
			if( bRepeatGrade==FALSE )
			{
				aaTempGrades[ucTotal] = ucExtraGrade;
				ucTotal++;
			}
		}

		if( m_bAOINgPickPartDie )	//	425TX 1
		{
			SelectBondCamera();
			Sleep(500);
			SelectWaferCamera();
		}
		m_WaferMapWrapper.SelectGrade(aaTempGrades, ucTotal);
	}
	else if( (m_bPrescanDefectAction && m_bPrescanDefectToNullBin==FALSE && m_bNgPickDefect) || 
			 (m_bPrescanBadCutAction && m_bPrescanBadCutToNullBin==FALSE && m_bNgPickBadCut) )
	{
		UpdateSortSequenceAfterScan();
	}	//	prescan and do ng picking.


	switch( GetScanAction() )
	{
	case PRESCAN_FINISHED_PICK_NG:
		SaveScanTimeEvent("    WFT: Prescan finished.  Start picking NG");
		break;
	case PRESCAN_FINISHED_SORT_BY_GRADE:
		if( IsESMachine() )
			SaveScanTimeEvent("    WFT: Prescan finished.  stop and then pick NG grade");
		else
			SaveScanTimeEvent("    WFT: Prescan finished.  Start sorting by grade");
		break;
	case PRESCAN_FINISHED_DO_NOTHING:
	default:
		SaveScanTimeEvent("    WFT: All done and do nothing stop");
		break;
	}

	if( bContinue )
	{
		if (GetScanAction() == PRESCAN_FINISHED_DO_NOTHING || m_bStopBonding)
		{
			SaveScanTimeEvent("    WFT: select do nothing and operation stopped");
			lContinueState = 2;	// stop to house keeping
			m_lTimeSlot[17] = (LONG)(GetTime() - m_dScanEndTime);	//	17.	overhead time.
			m_lTimeSlot[18] = (LONG)(GetTime() - m_dScanStartTime);	//	18.	total time.
			for(int i=4; i<26; i++)
			{
				CString szMsg;
				szMsg.Format("%ld,", m_lTimeSlot[i]);
				LogScanDetailTime(szMsg);
			}
			LogScanDetailTime("\n");								//	0.	new line start
		}
		if( IsESMachine() && GetScanAction()==PRESCAN_FINISHED_SORT_BY_GRADE )
		{
			SaveScanTimeEvent("    WFT: select do stop then NG pick");
			lContinueState = 2;	// stop to house keeping
			m_lTimeSlot[17] = (LONG)(GetTime() - m_dScanEndTime);	//	17.	overhead time.
			m_lTimeSlot[18] = (LONG)(GetTime() - m_dScanStartTime);	//	18.	total time.
			for(int i=4; i<26; i++)
			{
				CString szMsg;
				szMsg.Format("%ld,", m_lTimeSlot[i]);
				LogScanDetailTime(szMsg);
			}
			LogScanDetailTime("\n");								//	0.	new line start
		}
	}
	else
	{
		SaveScanTimeEvent("    WFT: Prescan finished.  error detected");
		if( m_lPrescanVerifyResult>0 || m_lPresortWftStatus>0 )
		{
			SetAlignmentStatus(FALSE);
		}

		lContinueState = 2;	// error, stop to house keeping
	}

	if( pUtl->GetPrescanAreaPickMode() )
	{
		m_lAreaPickStage = 0;
		lContinueState = 1;	// area pick prescan finish is wafer end 
	}

	if( m_ulPrescanGoCheckLimit>0 && lContinueState!=2 && bNeedGoCheck )
	{
		lContinueState = 2; // force to check go button result
	}

	m_lRescanRunStage		= 0;	// prescan all done
	m_lWftAdvSamplingStage	= 0;
	m_lWftAdvSamplingResult	= 0;

	UpdateSMS(FALSE);

	if( m_bCaptureScanMapImage )	//	prescan setting, Grab scanned map image
	{
		CString szMapImagePath = m_szPrescanLogPath + ".JPG";
		if( m_szSaveMapImagePath!="" )
		{
			CString szSaveImagePath = m_szSaveMapImagePath;
			CreateDirectory(szSaveImagePath, NULL);
			if (IsPathExist(szSaveImagePath))
			{
				szMapImagePath = szSaveImagePath + "\\" + GetMapNameOnly() + ".JPG";
			}
		}

		SaveScanTimeEvent("    WFT: to capture scan map image");
		Sleep(3000);
		PrintPartScreen(szMapImagePath, 30, 395, 630, 965);
	}

	if( m_lBuildShortPathInScanning==0 && szAlgorithm.Find("Sorting Path 1")!=-1 )	// scan done, DEB rebuild with lock resume
	{
		SaveScanTimeEvent("    WFT: scan to let map build short path");
		m_bPrescanWaferEnd4ShortPath = TRUE;	// prescan done, short path, but need to rebuild, lock resume
		LONG lCount = 0;
		while( 1)
		{
			Sleep(100);
			if( m_bPrescanWaferEnd4ShortPath==FALSE )
			{
				break;
			}
			lCount++;
			if( lCount>1000 )
			{
				break;
			}
		}

		if( DEB_IsUseable() )
		{
			m_WaferMapWrapper.SetAlgorithmParameter("DEB PreScan Finish", FALSE);
		}
		SaveScanTimeEvent("    WFT: map build short path done after scan");
	}

	if( m_dStopTime>0 )
	{
		m_dScanTime += fabs(GetTime() - m_dStopTime)/1000.0;	// time after PR and physical map built
		m_dCurrTime = m_dScanTime;
		m_dStopTime = GetTime();
	}
	szLogMsg.Format("    WFT: prescan sw all done %.2f", m_dScanTime);
	SaveScanTimeEvent(szLogMsg);

	if( pApp->GetCustomerName()==_T("Enraytek") )
	{
		CSingleLock slLock(&(pApp->m_csMachineAlarmLamp));
		slLock.Lock();
		LONG lAlarmStatus = GetAlarmLamp_Status();
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);

		// please input your op id, store into ms_operation log
		CString szOpID	= _T("");
		CString szTitle		= "Please input your OP ID: ";
		while( 1 )
		{
			BOOL bKeyIn = HmiStrInputKeyboard(szTitle, szOpID);
			SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);
			if( bKeyIn )
			{
				if( szOpID.IsEmpty()==FALSE )
				{
					break;
				}
			}
		}

		slLock.Unlock();

		CTime theTime = CTime::GetCurrentTime();
		CString szTime = theTime.Format("%Y-%m-%d %H:%M:%S");
		CString szMsg;
		szMsg.Format("Prescan done at %s by %s", szTime, szOpID);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	}

	if( lContinueState==2 )
	{
		SaveScanTimeEvent("    WFT: all done, continue is 2");
		LONG lRow, lCol, lEncX, lEncY;
		lRow = m_ulAlignRow;
		lCol = m_ulAlignCol;
		pUtl->GetAlignPosition(lRow, lCol, lEncX, lEncY);
		m_stLastDie.lT_Enc = GetGlobalT();
		m_stLastDie.lX_Enc = lEncX;
		m_stLastDie.lY_Enc = lEncY;
		XY_SafeMoveTo(lEncX, lEncY);
		T_MoveTo(GetGlobalT(), SFM_NOWAIT);
		WFT_MoveToFocusLevel();
	
		GetEncoderValue();

		// Need to get map selected position here for correct index display in below
		if (m_lPrescanVerifyResult == 0 && m_lPresortWftStatus == 0)
		{
			SetAlert_Red_Yellow(IDS_WT_PRESCAN_FINISHED);		
		}
		else
		{
			SetAlert_Red_Yellow(IDS_WT_PRESCAN_FAILED);
		}

		UpdateWaferYield();
		IPC_CServiceMessage stMsg;
		INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ReLoadBinMapFcn", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		
		m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
	}

	//4.53D90 Run fnc
	BOOL ResetBinCountPerWft = pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT_PER_WAFER);
	if( ResetBinCountPerWft)
	{
		CheckOptimizeBinCountPerWft();
	}

	SetWTStable(FALSE);
	SetWTReady(FALSE, "false1");
	SetWTBacked(FALSE);
	SetMoveBack(FALSE);
	SetDieInfoRead(TRUE);
	SetDieReadyForBT(FALSE);
	SetWTStartMove(FALSE);		//v3.66	//DBH
	(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"]	= FALSE;	// prescan finish
	m_bThisDieNotLFed = FALSE;
	m_ulPickingBlockIndex	= 0;
	m_lBuildShortPathInScanning = 0;	//	prescan all done.

	m_bIsPrescanning = FALSE;
	m_bFirstInAutoCycle = TRUE;	//	scan wafer done

	szLogMsg.Format("    WFT: wait wafer table all done done continue %d", lContinueState);
	SaveScanTimeEvent(szLogMsg);
	CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(7);	//MAPPING_END		//v4.40T11
	m_stLastPickTime = CTime::GetCurrentTime();
	m_bLoadedWaferScannedOnce = TRUE;

	return lContinueState;
}

BOOL CWaferTable::RestartDefectDieSort()	//	after normal sort wafer end, sort the detective die (not Prescan NG Pick).
{
	if( m_bToPickDefectDie==FALSE )
	{
		return FALSE;
	}

	if ( m_bSortingDefectDie )
	{
		return FALSE;
	}
	m_bSortingDefectDie = TRUE;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	pUtl->SetPickDefectDie(TRUE);

	if( m_bScanPickedDieResort )
	{
		return NCP_Auto_ScanPickedDieResort();
	}

	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();

	UCHAR aNgPickGrades[50];
	UCHAR ucNgTotal = 0;
	UCHAR ucOldDefectiveGrade = 0;
	if( m_ucDefectiveDieGrade!=0 )
	{
		ucOldDefectiveGrade = ucOffset + m_ucDefectiveDieGrade;
		aNgPickGrades[ucNgTotal] = ucOldDefectiveGrade;
		ucNgTotal++;
	}
	if( IsPrescanEnable() )
	{
		UCHAR ucNgDefectGrade = GetScanDefectGrade() + ucOffset;
		if( m_bPrescanDefectToNullBin )
			ucNgDefectGrade = ucNullBin;
		if( m_bNgPickDefect && ucNgDefectGrade!=ucNullBin )
		{
			BOOL bRepeatGrade = FALSE;
			for(UCHAR i=0; i<ucNgTotal; i++)
			{
				if( aNgPickGrades[i]==ucNgDefectGrade )
				{
					bRepeatGrade = TRUE;
					break;
				}
			}
			if( bRepeatGrade==FALSE )
			{
				aNgPickGrades[ucNgTotal] = ucNgDefectGrade;
				ucNgTotal++;
			}
		}

		UCHAR ucNgBadcutGrade = GetScanBadcutGrade() + ucOffset;
		if( m_bPrescanBadCutToNullBin )
			ucNgBadcutGrade = ucNullBin;
		if( m_bNgPickBadCut && ucNgBadcutGrade!=ucNullBin )
		{
			BOOL bRepeatGrade = FALSE;
			for(UCHAR i=0; i<ucNgTotal; i++)
			{
				if( aNgPickGrades[i]==ucNgBadcutGrade )
				{
					bRepeatGrade = TRUE;
					break;
				}
			}
			if( bRepeatGrade==FALSE )
			{
				aNgPickGrades[ucNgTotal] = ucNgBadcutGrade;
				ucNgTotal++;
			}
		}

		UCHAR ucNgEdgeDieGrade = ucNullBin;
		if( m_ucPrescanEdgeSize>0 && m_ucPrescanEdgeGrade>0 )
			ucNgEdgeDieGrade = m_ucPrescanEdgeGrade + ucOffset;
		if( m_bNgPickEdgeDie && ucNgEdgeDieGrade!=ucNullBin )
		{
			BOOL bRepeatGrade = FALSE;
			for(UCHAR i=0; i<ucNgTotal; i++)
			{
				if( aNgPickGrades[i]==ucNgEdgeDieGrade )
				{
					bRepeatGrade = TRUE;
					break;
				}
			}
			if( bRepeatGrade==FALSE )
			{
				aNgPickGrades[ucNgTotal] = ucNgEdgeDieGrade;
				ucNgTotal++;
			}
		}

		UCHAR ucExtraGrade  = m_ucScanExtraGrade + ucOffset;
		if( m_bNgPickExtra && m_bScanExtraDieAction && ucExtraGrade!=ucNullBin )
		{
			BOOL bRepeatGrade = FALSE;
			for(UCHAR i=0; i<ucNgTotal; i++)
			{
				if( aNgPickGrades[i]==ucExtraGrade )
				{
					bRepeatGrade = TRUE;
					break;
				}
			}
			if( bRepeatGrade==FALSE )
			{
				aNgPickGrades[ucNgTotal] = ucExtraGrade;
				ucNgTotal++;
			}
		}	//	pick extra scanned die
	}

	LONG lEdgeMinRow = m_lScannedMapMinRow;
	LONG lEdgeMaxRow = m_lScannedMapMaxRow;
	LONG lEdgeMinCol = m_lScannedMapMinCol;
	LONG lEdgeMaxCol = m_lScannedMapMaxCol;
	m_WaferMapWrapper.StopMap();
	if( ucOldDefectiveGrade>0 )
	{
		for(ULONG ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
		{
			for(ULONG ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
			{
				bool bChangeGrade = false;
				UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
				if( m_WaferMapWrapper.GetReader()->IsDefectiveDie(ulRow, ulCol) && ucGrade!=ucNullBin )
				{
					bChangeGrade = true;
				}
				if( bChangeGrade==false && ucGrade!=ucNullBin )
				{
					for(UCHAR i=0; i<ucNgTotal; i++)
					{
						if( aNgPickGrades[i]==ucGrade )
						{
							bChangeGrade = true;
							break;
						}
					}
				}
				if( bChangeGrade )
				{
					m_WaferMapWrapper.ClearDieStatus(ulRow, ulCol);	//v3.61
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucOldDefectiveGrade);
				}
			}
		}
	}

	CString szMsg = "", szTemp;
	if( ucOldDefectiveGrade>0 )
	{
		szMsg.Format("    WFT: Restart sort defect die to %d from ", ucOldDefectiveGrade - ucOffset);
	}
	for(UCHAR i=0; i<ucNgTotal; i++)
	{
		szTemp.Format("%d,", aNgPickGrades[i] - ucOffset);
		szMsg += szTemp;
	}
	SaveScanTimeEvent(szMsg);

	if( ucNgTotal>0 )
	{
		m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aNgPickGrades[0]);
		m_WaferMapWrapper.SelectGrade(aNgPickGrades, ucNgTotal);
		m_WaferMapWrapper.ResumeAlgorithmPreparation(TRUE);
		m_WaferMapWrapper.StartMap();
	}

	return TRUE;
}

BOOL CWaferTable::PrescanResultVerify()
{
	ULONG ulEmptyDie	= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY];
	ULONG ulDefectDie	= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT];
	ULONG ulBadCutDie	= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT];
	ULONG ulGoodDie		= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD];
	ULONG ulTotalDie	= ulGoodDie + ulBadCutDie + ulDefectDie+ ulEmptyDie;
	ULONG ulScanGoodDie = ulGoodDie;

	m_bStopBonding = FALSE;
	ulGoodDie = ulTotalDie;

	ULONG ulNgTotal = ulEmptyDie + ulBadCutDie + ulDefectDie;
	LONG lUserRow=0, lUserCol=0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	BOOL bScoreCount = TRUE;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanAoiWafer() && m_bPrescanEmptyAction )
	{
		bScoreCount = FALSE;
	}

	if( bScoreCount )
		ulGoodDie -= ulEmptyDie;
	if( m_bPrescanCheckDefect )
		ulGoodDie -= ulDefectDie;
	if( m_bPrescanCheckBadCut )
		ulGoodDie -= ulBadCutDie;

	m_lTimeSlot[20]	= ulBadCutDie;	//	20.	bad cut die
	m_lTimeSlot[21]	= ulDefectDie;	//	21. defect
	m_lTimeSlot[22]	= ulEmptyDie;	//	22.	empty die
	m_lTimeSlot[23] = ulGoodDie;	//	23.	good die total
	m_lTimeSlot[24] = ulTotalDie;	//	24. total die counter
	m_lTimeSlot[25] = m_ulMapBaseNum;//	25. total die counter 
	double dResult = 0;
	if( ulTotalDie!=0 )
		dResult = ((1.0*ulGoodDie*100.0)/(1.0*ulTotalDie));
	double dBaseRatio = 0.0;
	if( m_ulMapBaseNum!=0 )
		dBaseRatio = ((1.0*ulGoodDie*100.0)/(1.0*m_ulMapBaseNum));

	BOOL bOldMapFile = FALSE;
	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	CStringMapFile smfScanYield;
	LONG lTotalWafers = 0, lWaferIndex = 0;
	if ( smfScanYield.Open(MSD_SCAN_YIELD_INFO_FILE, FALSE, TRUE) != 1 )
	{
		CMSFileUtility::Instance()->LogFileError("Cannot Save File: " + MSD_SCAN_YIELD_INFO_FILE);
	}
	else
	{
		CString szName, szTime, szWft, szMgzn, szSlot, szData;
		lTotalWafers	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][MSD_SCAN_YIELD_TOTAL_WAFERS];
		lWaferIndex		= (smfScanYield)[MSD_SCAN_YIELD_HEAD][MSD_SCAN_YIELD_WAFER_INDEX];
		lTotalWafers += 1;
		if( lTotalWafers<=0 )
			lTotalWafers = 1;

		for(int i=lWaferIndex; i>0; i--)
		{
			szName	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_NAME];
			if( szName.CompareNoCase(GetMapNameOnly())==0 )
			{
				bOldMapFile = TRUE;	//	OR just last one only.
				break;
			}
		}

		lWaferIndex += 1;
		if( lWaferIndex<=0 )
		{
			lWaferIndex = 1;
		}
		if( lWaferIndex>500 )
		{
			lWaferIndex = 500;
			for(int i=1; i<500; i++)
			{
				szName	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i+1][MSD_SCAN_YIELD_NAME];
				szTime	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i+1][MSD_SCAN_YIELD_TIME];
				szWft	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i+1][MSD_SCAN_YIELD_WFTNO];
				szMgzn	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i+1][MSD_SCAN_YIELD_MGZNNO];
				szSlot	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i+1][MSD_SCAN_YIELD_SLOTNO];
				szData	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i+1][MSD_SCAN_YIELD_VALUE];

				(smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_NAME]		= szName;
				(smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_TIME]		= szTime;
				(smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_WFTNO]	= szWft;
				(smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_MGZNNO]	= szMgzn;
				(smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_SLOTNO]	= szSlot;
				(smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_VALUE]	= szData;
			}
		}

		szName = GetMapNameOnly();
		szTime = ctDateTime.Format("%Y%m%d%H%M%S");
		szWft.Format("%ld", IsWT2InUse()+1);
		LONG lScanMgzn1 = (LONG)(*m_psmfSRam)["WaferTable"]["WT1LoadMgzn"];
		LONG lScanSlot1 = (LONG)(*m_psmfSRam)["WaferTable"]["WT1LoadSlot"];
		LONG lScanMgzn2 = (LONG)(*m_psmfSRam)["WaferTable"]["WT2LoadMgzn"];
		LONG lScanSlot2 = (LONG)(*m_psmfSRam)["WaferTable"]["WT2LoadSlot"];
		szMgzn.Format("%ld", lScanMgzn1);
		szSlot.Format("%ld", lScanSlot1);
		szData.Format("%.2f", dResult);

		(smfScanYield)[MSD_SCAN_YIELD_HEAD][MSD_SCAN_YIELD_TOTAL_WAFERS]		= lTotalWafers;
		(smfScanYield)[MSD_SCAN_YIELD_HEAD][MSD_SCAN_YIELD_WAFER_INDEX]			= lWaferIndex;
		(smfScanYield)[MSD_SCAN_YIELD_HEAD][lWaferIndex][MSD_SCAN_YIELD_NAME]	= szName;
		(smfScanYield)[MSD_SCAN_YIELD_HEAD][lWaferIndex][MSD_SCAN_YIELD_WFTNO]	= szWft;
		(smfScanYield)[MSD_SCAN_YIELD_HEAD][lWaferIndex][MSD_SCAN_YIELD_MGZNNO]	= szMgzn;
		(smfScanYield)[MSD_SCAN_YIELD_HEAD][lWaferIndex][MSD_SCAN_YIELD_SLOTNO]	= szSlot;
		(smfScanYield)[MSD_SCAN_YIELD_HEAD][lWaferIndex][MSD_SCAN_YIELD_TIME]	= szTime;
		(smfScanYield)[MSD_SCAN_YIELD_HEAD][lWaferIndex][MSD_SCAN_YIELD_VALUE]	= szData;
		smfScanYield.Update();
		smfScanYield.Close();
	}
	m_lScanYieldSpinIndex = 0;

	CString szMsg;
	szMsg.Format(" bad %d, defect %d, empty %d, good %d, total %d, yield %.2f, good %.2f, fail %.2f",
		ulBadCutDie, ulDefectDie, ulEmptyDie, ulGoodDie, ulTotalDie, dResult, m_dPrescanGoodPassScore, m_dPrescanFailPassScore);
	SaveScanTimeEvent("    WFT: map " +  GetMapNameOnly() + szMsg);
	CStdioFile pNgFile;
	CString szYYYYMM = ctDateTime.Format("_%Y%m");
	CString szMcNo = GetMachineNo();
	CString szScanSumName = "ScanSummary_" + szMcNo + szYYYYMM + ".csv";
	CString szLocalName = gszUSER_DIRECTORY + "\\Performance\\" + szScanSumName;
	if( pNgFile.Open(szLocalName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText) )
	{
		ULONGLONG lFileLen = pNgFile.GetLength();
		if( lFileLen==0 )
		{
			pNgFile.WriteString("Index,WaferNo,OP ID,Device Type,Lot No,Bad Cut,Defect,Empty,Scan NG,Good,Total,Yield,Bin Count,Load,Unload,Base Num,Base Yield,WT NO\n");
		}
		pNgFile.SeekToEnd();
		CString szOperatorID = (*m_psmfSRam)["MS896A"]["Operator Id"];
		CString szDeviceType = (*m_psmfSRam)["MS896A"]["PKG Filename"];
		szDeviceType.Trim(".pkg");
		CString szLotNo = (*m_psmfSRam)["MS896A"]["LotNumber"];	
		CString szData;
		szData.Format("%d,%s,%s,%s,%s,", lTotalWafers, GetMapNameOnly(), szOperatorID, szDeviceType, szLotNo);
		pNgFile.WriteString(szData);
		ULONG ulScanNg = ulBadCutDie + ulDefectDie + ulEmptyDie;
		szData.Format("%d,%d,%d,%d,", ulBadCutDie, ulDefectDie, ulEmptyDie, ulScanNg);
		pNgFile.WriteString(szData);
		szData.Format("%d,%d,%.2f%s,%d,", ulGoodDie, ulTotalDie, dResult, "%", ulGoodDie);
		pNgFile.WriteString(szData);
		// scan begin time (create map time).	13-01-04 01:27,
		szData = m_stReadTmpTime.Format("%Y-%m-%d %H:%M:%S,");
		pNgFile.WriteString(szData);
		szData = ctDateTime.Format("%Y-%m-%d %H:%M:%S,");
		pNgFile.WriteString(szData);
		szData.Format("%d,%.2f,%d", m_ulMapBaseNum, dBaseRatio, IsWT2InUse()+1);
		pNgFile.WriteString(szData);
		szData = "\n";
		pNgFile.WriteString(szData);
		// ulNgTotal, );
		pNgFile.Close();
	}

	if( m_lOutputScanSummary==2 || m_lOutputScanSummary==3 )
	{
		CString szTargetPath  = m_szOutputScanSummaryPath + _T("\\") + szScanSumName;
		// copy summary file to target folder
		if( szLocalName.CompareNoCase(szTargetPath)!=0 )
		{
			CopyFile(szLocalName, szTargetPath, FALSE);
		}
	}

	SaveScanTimeEvent("    WFT: after update scan summary file");

	ULONG ulGoodCount = (ULONG)_round(ulTotalDie * m_dPrescanGoodPassScore / 100.0);
	ULONG ulFailCount = (ULONG)_round(ulTotalDie * m_dPrescanFailPassScore / 100.0);
	DOUBLE dFailResult = 0.0;

	if (m_dPrescanGoodPassScore > 0 && ulGoodDie < ulGoodCount)
	{
		m_bScanLowYield = TRUE;
		CString szTemp;
		szTemp.Format("GOOD %ld TOTAL %ld (%2.2f) lower than %2.2f!", ulGoodDie, ulTotalDie, dResult, m_dPrescanGoodPassScore);

		UINT unEnable = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Stop if prescan Lower than PassScore"), 0);
		if( unEnable !=0 )
		{
			HmiMessage_Red_Back(szTemp, "Prescan Check", glHMI_MBX_CLOSE);
			SetErrorMessage(szTemp);
			SaveScanTimeEvent(szTemp);
			return FALSE;
		}

		if( bOldMapFile==FALSE && m_dPrescanFailPassScore>0 && ulGoodDie<ulFailCount )
		{
			szTemp.Format("GOOD %ld TOTAL lower than %ld(%2.2f) of fail limit!", ulGoodDie, ulTotalDie, ulFailCount, m_dPrescanFailPassScore);
			HmiMessage_Red_Back(szTemp, "Prescan Check", glHMI_MBX_CLOSE);
			SetErrorMessage(szTemp);
			SaveScanTimeEvent(szTemp);
			return FALSE;
		}

		szTemp += "Do you want to continue?";
		if( pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="XA" )
		{
			SaveScanTimeEvent(szTemp);
			szTemp = "Prescan good yield is lower than setting, continue?";
		}
		SetErrorMessage(szTemp);
		SaveScanTimeEvent(szTemp);

		LONG lHmi = 0;
		lHmi = SetAlert_Msg_Red_Back(IDS_WPR_PRESCAN_GOOD_LOW, szTemp, "Yes", "No", NULL, glHMI_ALIGN_LEFT);

		if( lHmi == 1 )
		{
			SetErrorMessage("User to continue");
			SaveScanTimeEvent("User to continue");
			
			if (pApp->CheckPasswordToGo(2))
			{
				SaveScanTimeEvent("WFT: good pass score password check ok");
			}
			else
			{
				SetErrorMessage("User password fail, had to stop");
				SaveScanTimeEvent("User password fail, had to stop");
				return FALSE;
			}

			/*
			UINT unEnable = pApp->GetProfileInt(gszPROFILE_SETTING, _T("ScanPassScorePassWord"), 0);
			if( unEnable !=0 )
			{
				if( CheckEngineerPasswordToGo() )
				{
					SaveScanTimeEvent("WFT: good pass score password check ok");
				}
				else
				{
					SetErrorMessage("User password fail, had to stop");
					SaveScanTimeEvent("User password fail, had to stop");
					return FALSE;
				}

				
				for(int i=0; i<3; i++)
				{
					if( Check201277PasswordToGo() )
					{
						SaveScanTimeEvent("WFT: good pass score password check ok");
						break;
					}
					SetAlert_Red_Yellow(IDS_MS_PASSWORD_NOT_CORRECT);
					if( i==2 )
					{
						SetErrorMessage("User password fail, had to stop");
						SaveScanTimeEvent("User password fail, had to stop");
						return FALSE;
					}
				}
				
			}
			*/
			m_bStopBonding = TRUE; //check good die low
		}
		else
		{
			SetErrorMessage("User to stop");
			SaveScanTimeEvent("User to stop");
			return FALSE;
		}
	}

	if( IsMS90HalfSortMode() && pApp->GetCustomerName()=="SiLan" )
	{
		if( m_bMS90DisplayWholeOnly && !IsMS90Sorting2ndPart() )	// to display half wafer twice or whole wafer once only.
		{
			return TRUE;
		}
		szMsg.Format("Scan good die %d, defect die %d", ulScanGoodDie, ulDefectDie);
		SetErrorMessage(szMsg);
		SaveScanTimeEvent(szMsg);
		if( m_bMS90DisplayAndSelect )	//	An Option to display and selection.
		{
			szMsg += "\nDo you want to continue?";
			if( HmiMessage_Red_Back(szMsg, "Prescan Result", glHMI_MBX_YESNO)!=glHMI_YES )
			{
				SetErrorMessage("User to stop");
				SaveScanTimeEvent("User to stop");
				return FALSE;
			}
		}
	}	//	Silan show scan result for MS90

	return TRUE;
}

VOID CWaferTable::SetPrescanAutoAlignMap()
{
	if( m_lBuildShortPathInScanning==0 )
	{
		if( IsPrescanning() )
		{
			m_WaferMapWrapper.EnableAutoAlign(FALSE);
		}
		else
		{
			m_WaferMapWrapper.EnableAutoAlign(!m_bStartPoint);
		}
	}
}

BOOL CWaferTable::SetMapPhyPosn(ULONG ulRow, ULONG ulCol, LONG lPhyX, LONG lPhyY)
{
	// should check the position within wafer limit
	return m_WaferMapWrapper.SetPhysicalPosition(ulRow, ulCol, lPhyX, lPhyY);
}

VOID CWaferTable::SavePrescanInfoMap(CONST INT nState)
{
	FILE *fp;
	CString szFileName;
	LONG lRow, lCol, lWfX = 0, lWfY = 0;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	ULONG ulNumRow = 0, ulNumCol = 0;
	ULONG ulRow, ulCol, i;
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();

	if( pUtl->GetPrescanDebug()==FALSE )
	{
		return ;
	}

	switch( nState )
	{
	case 1:
		szFileName = m_szPrescanLogPath + PRESCAN_PSN_MAPPHY;
		break;
	case 0:
		szFileName.Format("%s_%07lu%s", m_szPrescanLogPath, GetNewPickCount(), RESCAN_OLD_PHY_FILE);
		break;
	case 2:
	default:
		szFileName.Format("%s_%07lu%s", m_szPrescanLogPath, GetNewPickCount(), RESCAN_NEW_PHY_FILE);
		break;
	}

	errno_t nErr = fopen_s(&fp, szFileName, "w");

	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "ROW, COL, WFX, WFY, WFT\n");
		pUtl->GetAlignPosition(lRow, lCol, lWfX, lWfY);
		fprintf(fp, "Aln: %ld, %ld, %ld, %ld, %ld\n", lRow, lCol, lWfX, lWfY, pUtl->GetPrescanGlobalTheta());
		for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
		{
			pUtl->GetReferPosition(i, lRow, lCol, lWfX, lWfY);
			fprintf(fp, "Ref: %ld, %ld, %ld, %ld\n", lRow, lCol, lWfX, lWfY);
		}

		fprintf(fp, "\n");
		for(i=0; i<pUtl->GetAssistPointsNum(); i++)
		{
			pUtl->GetAssistPosition(i, lRow, lCol, lWfX, lWfY);
			fprintf(fp, "Ast: %ld, %ld, %ld, %ld\n", lRow, lCol, lWfX, lWfY);
		}

		fprintf(fp, "\n");
		pUtl->GetRealignPosition(lRow, lCol,lWfX, lWfY);
		fprintf(fp, "Rln: %ld, %ld, %ld, %ld, %ld\n", lRow, lCol, lWfX, lWfY, pUtl->GetRealignGlobalTheta());
		for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
		{
			pUtl->GetReRefPosition(i, lRow, lCol, lWfX, lWfY);
			fprintf(fp, "Rrf: %ld, %ld, %ld, %ld\n", lRow, lCol, lWfX, lWfY);
		}

		fprintf(fp, "\n");
		fprintf(fp, "ROW, COL, WFX, WFY\n");

		UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
		UCHAR ucGrade = 0;
		m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);
		for(ulRow=0; ulRow<=ulNumRow; ulRow++)
		{
			for(ulCol=0; ulCol<=ulNumCol; ulCol++)
			{
				if( pUtl->GetPrescanRegionMode() )
				{
					if( pSRInfo->IsWithinThisRegion(ulTgtRegion, ulRow, ulCol)==FALSE )
						continue;
				}
				if( IsOutMS90SortingPart(ulRow, ulCol) )
				{
					continue;
				}

				if( GetMapPhyPosn(ulRow, ulCol, lWfX, lWfY) )
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
					if( ucGrade!=ucNullBin )
					{
						fprintf(fp, "%lu, %lu, %ld, %ld\n", ulRow, ulCol, lWfX, lWfY);
					}
				}
			}
		}

		fclose(fp);
	}
}

BOOL CWaferTable::IsUseLF()
{
	if (IsESMachine())
	{
		return FALSE;
	}

	return (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["UseLookForward"];
}

// for adv region offset only
INT CWaferTable::CheckAdvRegionPitch()
{
	LONG	lDelta_X = 0, lDelta_Y = 0;
	CString szLog;

	// cree same wafer realign after prescan, the position would vary seriously,
	// so use update prescan position

	lDelta_X = m_lGetRgnWftPosX - m_lGetScnWftPosX;
	lDelta_Y = m_lGetRgnWftPosY - m_lGetScnWftPosY;
	BOOL bXCheck = CheckPredScanPitchX(lDelta_X);
	BOOL bYCheck = CheckPredScanPitchY(lDelta_Y);
	if( bXCheck==FALSE || bYCheck==FALSE )
	{
		szLog.Format("PCE move table: AdvRegion pitch X mapp(%ld) pred(%ld) Y mapp(%ld) pred(%ld)",
			m_lGetScnWftPosX, m_lGetRgnWftPosX, m_lGetScnWftPosY, m_lGetRgnWftPosY); 
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return WT_PITCH_ERR_SCN_PREDCT;	//	move table
	}

	return 0;
}

// for deb only
INT CWaferTable::CheckDebScanPitch()
{
	CString szLog;

	// cree same wafer realign after prescan, the position would vary seriously,
	// so use update prescan position

	LONG lMapPX = m_lGetScnWftPosX;
	LONG lMapPY = m_lGetScnWftPosY;
	LONG lDelta_X = m_lGetAtcWftPosX - lMapPX;
	LONG lDelta_Y = m_lGetAtcWftPosY - lMapPY;
	BOOL bXCheck = CheckPredScanPitchX(lDelta_X);
	BOOL bYCheck = CheckPredScanPitchY(lDelta_Y);
	if( bXCheck==FALSE || bYCheck==FALSE )
	{
		szLog.Format("PITCH CHECK ERROR move table: Predict pitch X mapp(%ld) pred(%ld) Y mapp(%ld) pred(%ld)",
			lMapPX, m_lGetAtcWftPosX, lMapPY, m_lGetAtcWftPosY); 
		SetErrorMessage(szLog);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return WT_PITCH_ERR_SCN_PREDCT;	//	move table
	}

	return 0;
}

// for non adv region offset and non deb only
INT CWaferTable::CheckPrescanPitch(LONG lMoveX, LONG lMoveY)
{
	LONG	lDelta_X = 0, lDelta_Y = 0;
	LONG	lMapPX, lMapPY;
	CString szLog;

	lMapPX = m_lGetScnWftPosX;
	lMapPY = m_lGetScnWftPosY;
	lDelta_X = lMapPX - lMoveX;
	lDelta_Y = lMapPY - lMoveY;

	BOOL bXCheck = CheckPredScanPitchX(lDelta_X);
	BOOL bYCheck = CheckPredScanPitchY(lDelta_Y);
	if( bXCheck==FALSE || bYCheck==FALSE )
	{
		szLog.Format("PITCH CHECK ERROR move table: Prescan pitch X move(%ld) mapp(%ld)", lMoveX, lMapPX); 
		SetErrorMessage(szLog);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		szLog.Format("PITCH CHECK ERROR move table: Prescan pitch Y move(%ld) mapp(%ld)", lMoveY, lMapPY); 
		SetErrorMessage(szLog);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return WT_PITCH_ERR_SCN_PREDCT;	//	move table
	}

	return 0;
}

BOOL CWaferTable::CheckLFPitchX(LONG lDeltaX, LONG lTolerance)
{
	if( !IsDiePitchCheck() )
	{
		return TRUE;
	}
	LONG lDiePitchX_X	= GetDiePitchX_X();
	//LONG lTolerance		= m_lLFPitchToleranceX;

	if( (lTolerance>0) && (labs(lDeltaX)>labs(lDiePitchX_X*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("LF Check X pitch %ld, tolerance %ld, delta %ld, UpLimit,%d", lDiePitchX_X, lTolerance, lDeltaX, labs(lDiePitchX_X*lTolerance/100));
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}
	return TRUE;
}

BOOL CWaferTable::CheckLFPitchY(LONG lDeltaY, LONG lTolerance)
{
	if( !IsDiePitchCheck() )
	{
		return TRUE;
	}

	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	//LONG lTolerance		= m_lLFPitchToleranceY;

	if( (lTolerance>0) && (labs(lDeltaY)>labs(lDiePitchY_Y*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("LF Check Y pitch %ld, tolerance %ld, delta %ld, UpLimit,%d", lDiePitchY_Y, lTolerance, lDeltaY, labs(lDiePitchY_Y*lTolerance/100));
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}
	return TRUE;
}

BOOL CWaferTable::CheckPitchX(LONG lDeltaX)
{
	if( !IsDiePitchCheck() )
	{
		return TRUE;
	}
	LONG lDiePitchX_X	= GetDiePitchX_X();
	LONG lTolerance		= m_lPitchX_Tolerance;

	if( (lTolerance>0) && (labs(lDeltaX)>labs(lDiePitchX_X*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("Check X pitch %ld, tolerance %ld, delta %ld", lDiePitchX_X, lTolerance, lDeltaX);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);

		LONG lMapRow = 0, lMapCol = 0;
		ConvertAsmToOrgUser(  m_ulGetMveMapRow, m_ulGetMveMapCol, lMapRow, lMapCol);
		szLog.Format("PITCH CHECK INFO X map: %d,%d (%d,%d), scn(%d,%d), mve(%d,%d), ", 
							m_ulGetMveMapRow, m_ulGetMveMapCol, lMapRow, lMapCol,
							m_lGetScnWftPosX, m_lGetScnWftPosY, 
							m_lGetMveWftPosX, m_lGetMveWftPosY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckPitchY(LONG lDeltaY)
{
	if( !IsDiePitchCheck() )
	{
		return TRUE;
	}

	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	LONG lTolerance		= m_lPitchY_Tolerance;

	if( (lTolerance>0) && (labs(lDeltaY)>labs(lDiePitchY_Y*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("Check Y pitch %ld, tolerance %ld, delta %ld", lDiePitchY_Y, lTolerance, lDeltaY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		LONG lMapRow = 0, lMapCol = 0;
		ConvertAsmToOrgUser(  m_ulGetMveMapRow, m_ulGetMveMapCol, lMapRow, lMapCol);
		szLog.Format("PITCH CHECK INFO Y map: %d,%d (%d,%d), scn(%d,%d), mve(%d,%d), ", 
							m_ulGetMveMapRow, m_ulGetMveMapCol, lMapRow, lMapCol,
							m_lGetScnWftPosX, m_lGetScnWftPosY, 
							m_lGetMveWftPosX, m_lGetMveWftPosY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckAlignPitchX(LONG lDeltaX, LONG lPassInTolerance)
{
	LONG lDiePitchX_X = GetDiePitchX_X();

	LONG lTolerance = lPassInTolerance;
	if (lTolerance == 0)
	{
		lTolerance = m_lPitchX_Tolerance;
	}

	if( (lTolerance>0) && (labs(lDeltaX)>labs(lDiePitchX_X*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("Align Check X pitch %ld, tolerance %ld, delta %ld", lDiePitchX_X, lTolerance, lDeltaX);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckAlignPitchY(LONG lDeltaY, LONG lPassInTolerance)
{
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	LONG lTolerance = lPassInTolerance;
	if (lTolerance == 0)
	{
		lTolerance = m_lPitchY_Tolerance;
	}


	if( (lTolerance>0) && (labs(lDeltaY)>labs(lDiePitchY_Y*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("Align Check Y pitch %ld, tolerance %ld, delta %ld", lDiePitchY_Y, lTolerance, lDeltaY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckPredScanPitchX(LONG lDeltaX)
{
	LONG lDiePitchX_X = GetDiePitchX_X(); 

	if( (m_lPredScanPitchTolX>0) && (labs(lDeltaX)>labs(lDiePitchX_X*m_lRunPredScanPitchTolX/100)) )
	{
		CString szLog;
		szLog.Format("Check PredScan X pitch %ld, tolerance %ld, delta %ld", lDiePitchX_X, m_lRunPredScanPitchTolX, lDeltaX);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		SetErrorMessage(szLog);
		LONG lHmiRow = 0, lHmiCol = 0, lMapRow = 0, lMapCol = 0;
		ConvertAsmToHmiUser(  m_ulGetMveMapRow, m_ulGetMveMapCol, lHmiRow, lHmiCol);
		ConvertAsmToOrgUser(  m_ulGetMveMapRow, m_ulGetMveMapCol, lMapRow, lMapCol);
		szLog.Format("PITCH CHECK INFO map: %d,%d (%d,%d)(%d,%d), scn(%d,%d), mve(%d,%d), ", 
							m_ulGetMveMapRow, m_ulGetMveMapCol, lHmiRow, lHmiCol, lMapRow, lMapCol,
							m_lGetScnWftPosX, m_lGetScnWftPosY, 
							m_lGetMveWftPosX, m_lGetMveWftPosY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckPredScanPitchY(LONG lDeltaY)
{
	LONG lDiePitchY_Y = GetDiePitchY_Y();

	if( (m_lPredScanPitchTolY>0) && (labs(lDeltaY)>labs(lDiePitchY_Y*m_lRunPredScanPitchTolY/100)) )
	{
		CString szLog;
		szLog.Format("Check PredScan Y pitch %ld, tolerance %ld, delta %ld", lDiePitchY_Y, m_lRunPredScanPitchTolY, lDeltaY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		SetErrorMessage(szLog);
		LONG lHmiRow = 0, lHmiCol = 0, lMapRow = 0, lMapCol = 0;
		ConvertAsmToHmiUser(  m_ulGetMveMapRow, m_ulGetMveMapCol, lHmiRow, lHmiCol);
		ConvertAsmToOrgUser(  m_ulGetMveMapRow, m_ulGetMveMapCol, lMapRow, lMapCol);
		szLog.Format("PITCH CHECK INFO map: %d,%d (%d,%d)(%d,%d), scn(%d,%d), mve(%d,%d), ", 
							m_ulGetMveMapRow, m_ulGetMveMapCol, lHmiRow, lHmiCol, lMapRow, lMapCol,
							m_lGetScnWftPosX, m_lGetScnWftPosY, 
							m_lGetMveWftPosX, m_lGetMveWftPosY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckPitchUpX(LONG lDeltaX)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_SANAN )
	{
		return TRUE;
	}

	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lTolerance = 49;

	if( (m_lPitchX_Tolerance>0) && (labs(lDeltaX)>labs(lDiePitchX_X*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("Check X pitch Up over tolerance %ld", lTolerance);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckPitchUpY(LONG lDeltaY)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_SANAN )
	{
		return TRUE;
	}

	LONG lDiePitchY_Y = GetDiePitchY_Y();
	LONG lTolerance = 49;

	if( (m_lPitchY_Tolerance>0) && (labs(lDeltaY)>labs(lDiePitchY_Y*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("Check Y pitch Up over tolerance %ld", lTolerance);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckTekCoreAutoAlignResult(LONG lRow, LONG lCol)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( ((pApp->GetCustomerName()=="Tekcore") || pApp->GetCustomerName()=="Walsin") && IsEnableFPC() )
	{
		INT i, iCheckCount = 0;
		ULONG ulHomeRow = 0, ulHomeCol = 0;
		ULONG ulAsmRow = 0, ulAsmCol = 0;
		LONG lScnCheckRow = 0, lScnCheckCol = 0;
		CString szMsg;

		iCheckCount = 0;
		ConvertOrgUserToAsm(0, 0, ulHomeRow, ulHomeCol);
		for (i=1; i<=m_lTotalSCNCount; i++)
		{
			i = min(i, WT_ALN_MAXCHECK_SCN-1);		//Klocwork	//v4.02T5

			if( GetFPCMapPosition(i, ulAsmRow, ulAsmCol, lScnCheckRow, lScnCheckCol)==FALSE )
			{
				szMsg.Format("point %d, row %ld, column %ld, not out of map range!", i, lScnCheckRow, lScnCheckCol);
				SetErrorMessage(szMsg);
				HmiMessage_Red_Back(szMsg);
				SetAlignmentStatus(FALSE);
				return FALSE;
			}
			if( ulAsmRow==ulHomeRow && ulAsmCol==ulHomeCol )
				continue;

			iCheckCount++;
			if( iCheckCount>4 )
				break;

			if (m_pWaferMapManager->IsMapHaveBin(ulAsmRow, ulAsmCol))
			{
				szMsg.Format("Refer Die %ld,%ld(%lu,%lu) has grade, it is not refer die", lScnCheckRow, lScnCheckCol, ulAsmRow, ulAsmCol);
				CMSLogFileUtility::Instance()->WT_LogStatus(szMsg);
				SetErrorMessage(szMsg);
				HmiMessage_Red_Back(szMsg);
				SetAlignmentStatus(FALSE);
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CWaferTable::GoToPrescanDieCheckPosition() // by press HMI button
{
	CString szText;
	CString szTitle;
	ULONG	ulIntRow = 0, ulIntCol = 0;
	LONG	lPhyX = 0, lPhyY = 0;
	BOOL	bOriginal = FALSE;

	LONG	lScnCheckRow, lScnCheckCol;
	LONG lHmiStatus = glHMI_CONTINUE;
	INT ErrCount = 0;
	CString szListErrorDie;

	ChangeCameraToWafer();

	if( IsBLInUse() )	//	4.24TX 4
	{
		if( MoveBackLightToSafeLevel()==FALSE )
			return FALSE;
	}

	if( GetFPCMapPosition(0, ulIntRow, ulIntCol, lScnCheckRow, lScnCheckCol)==FALSE )
	{
		szText.Format("row %ld, column %ld out of map range!", lScnCheckRow, lScnCheckCol);
		HmiMessage(szText);

		return FALSE;
	}

	szTitle.LoadString(HMB_WT_CHECK_SCN_FUNC);

	//Update wafermap position
	m_WaferMapWrapper.SetCurrentPosition(ulIntRow, ulIntCol);
	Sleep(500);

	//If no grade on select die pos, try to use surrounding die
	if( GetDieValidPrescanPosn(ulIntRow, ulIntCol, 1, lPhyX, lPhyY)==FALSE )
	{
		szText.Format("%ld %ld", m_lScnHmiRow, m_lScnHmiCol);
		SetAlert_Msg(IDS_WT_NO_SCN_DIE_GRADE, szText);

		return FALSE;
	}

	if ( m_bJoystickOn == TRUE ) 
	{
		SetJoystickOn(FALSE);
		bOriginal = TRUE;
	}

	XY_SafeMoveTo(lPhyX, lPhyY);
	BOOL bPromptMessage = TRUE;
	ScnCheckDiePrResult(lScnCheckRow, lScnCheckCol, bPromptMessage, szText, szListErrorDie, ErrCount);

	//Ask user to continue or stop
	if ( ( bPromptMessage == TRUE ) && ( ErrCount > 0 ) )
	{
		if ( GetErrorChooseGoFPC() && (ErrCount > m_lScnCheckAlarmLimit) )
		{
			lHmiStatus = HmiMessage(szListErrorDie, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			if( Check201277PasswordToGo() )
			{
				return TRUE;
				//FivePointCheckLog("Scan check error over limit and engineer let continue.");
			}
			else
			{
				//FivePointCheckLog("Scan check error over limit and no engineer password.");
				return FALSE;
			}
		}
		/*
		else
		{
			lHmiStatus = HmiMessage(szText, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		}
		*/
	}
	if (lHmiStatus != glHMI_CONTINUE)
	{
		szText.LoadString(HMB_GENERAL_STOPBYUSER);

		HmiMessage(szText, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	}

	if ( bOriginal == TRUE )
		SetJoystickOn(TRUE);

	return TRUE;
}

VOID CWaferTable::FivePointCheckLog(CString szLogMsg)
{
	FILE *fp;

	CString szLogFile;
	szLogFile = m_szPrescanLogPath + "_FivePoint.txt";

	errno_t nErr = fopen_s(&fp, szLogFile, "a");

	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%s\n", (LPCTSTR) szLogMsg);	//v4.47 Klocwork
		fclose(fp);
	}
}

BOOL CWaferTable::EmptyDieGradeCheck()
{
	ULONG	ulRow, ulCol;
	ULONG	ulNumOfRow=0, ulNumOfCol=0;
	LONG	lPosX, lPosY;
	LONG	lResult;
	ULONG	ulErrorCount, ulOkCount;
	ULONG	ulLoop = 0;
	CString strTemp;
	CString szTheme, szTitle, szDetail;
	CString szWaferId;

	LONG lEmptyGrade = m_lMnNoDieGrade;
	if( m_bUseEmptyCheck==FALSE || lEmptyGrade<=0 )
	{
		return TRUE;
	}

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRow, ulNumOfCol))
	{
		return TRUE;
	}

	szTitle = "Prescan Empty Die Check";
	szTheme.Format("Prescan Bin %ld empty check: ", lEmptyGrade);
	SetCtmLogMessage(szTheme + "Start!");


	// to handler for result set to files for CREE CIMS
	BOOL		bOutput;
	CString strFileNmBin, strFileNmLog;
	CString strLocalFileNmBin, strLocalFileNmLog;

	//Get Output Path
	bOutput = m_pBlkFunc2->Blk2GetIniOutputFileName(&strFileNmBin, &strFileNmLog, &strLocalFileNmBin, &strLocalFileNmLog);
	if ( !bOutput ) 
	{
		strTemp = szTheme + "\nFails to get output file path.\nFiles will be generated to local!";
		LONG lTemp = HmiMessage_Red_Back(strTemp, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
		SetCtmLogMessage(strTemp);

		if ( lTemp == glHMI_STOP)
		{
			SetCtmLogMessage("User chooses STOP!");
			return FALSE;
		}
		else
		{
			SetCtmLogMessage("User chooses CONTINUE!");
		}
	}
	// to handler for result set to files for CREE CIMS

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	ulErrorCount = 0;
	ulOkCount = 0;

	for(ulRow=0; ulRow<ulNumOfRow; ulRow++)
	{
		for(ulCol=0; ulCol<ulNumOfCol; ulCol++)
		{
			if( m_WaferMapWrapper.GetGrade(ulRow, ulCol) != (lEmptyGrade + m_WaferMapWrapper.GetGradeOffset()) )
				continue;

			if( pUtl->GetPrescanRegionMode() )
			{
				ULONG ulScanRegion = pSRInfo->GetTargetRegion();
				if( pSRInfo->IsWithinThisRegion(ulScanRegion, ulRow, ulCol)==FALSE )
					continue;
			}
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}

			LONG lUserRow, lUserCol;
			ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
			ulLoop++;
			szDetail.Format("die order = %lu map %lu,%lu(%ld,%ld)", ulLoop, ulRow, ulCol, lUserRow, lUserCol);
			SetCtmLogMessage(szTheme + szDetail);

			// Index
			if( GetDieValidPrescanPosn(ulRow, ulCol, 2, lPosX, lPosY)==FALSE )
				lResult = WT_ALN_SCN_NO_NAR_GRADE;
			else
				lResult = WT_ALN_OK;
			if ( lResult != WT_ALN_OK) 
			{
				if( m_lEmptyGradeMinCount==0 )
				{
					LONG lTmp;
					strTemp = szTheme + "\n" + szDetail + " Error!\nCan not locate die by surrounding!";
					SetCtmLogMessage(strTemp);
					lTmp = HmiMessage_Red_Back(strTemp, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
					if ( lTmp == glHMI_STOP)
					{
						SetCtmLogMessage("User chooses STOP!");
						return FALSE;
					}
					else
					{
						SetCtmLogMessage("User chooses CONTINUE!");
					}
				}
				continue;
			}

			m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
			Sleep(100);

			if( XY_SafeMoveTo(lPosX, lPosY)==FALSE )
			{
				strTemp = szTheme + szDetail + "\nMove to die table error!";
				SetCtmLogMessage(strTemp);
				HmiMessage_Red_Back(strTemp, szTitle);
				return FALSE;
			}
			Sleep(20);
			strTemp.Format("EmptyDie Posn %ld,%ld", lPosX, lPosY);
			SetCtmLogMessage(strTemp);

			if( WftSearchNormalDie(lPosX, lPosY, TRUE)!=TRUE )	// EMPTY DIE OR PR EROR OR OUT OF LIMIT
			{
				ulOkCount++;
				continue;
			}

			strTemp = szTheme + szDetail + " Error! Normal Die is found!";
			SetCtmLogMessage(strTemp);

			ulErrorCount++;

			// to handler for result set to files for CREE CIMS
			//**** Generate Bin 17 (for Cree) Error Log whenever error is found ****//
			//Write Sum file
			CStdioFile	pFileBin;
			bOutput = pFileBin.Open(strLocalFileNmBin, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText);
			if ( !bOutput )
			{
				strTemp = szTheme + "Fails to open Sum file.";
				HmiMessage_Red_Back(strTemp, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
				SetErrorMessage(strTemp);
				return FALSE;
			}
			pFileBin.SeekToEnd();
			strTemp.Format("%lu\n", ulErrorCount);
			pFileBin.WriteString(strTemp);
			pFileBin.Close();

			if ( CopyFile(strLocalFileNmBin, strFileNmBin, FALSE) == FALSE)
			{
				strTemp = szTheme + "Fail to copy Sum file to the network path!";
				SetErrorMessage(strTemp);
			}

			//Write SDS file
			CStdioFile	pFileLog;
			if (ulErrorCount == 1)		//Write the header and the first error found
			{
				bOutput = pFileLog.Open(strLocalFileNmLog, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText);
				if ( !bOutput )
				{
					strTemp = szTheme + "Fails to open SDS file.";
					HmiMessage_Red_Back(strTemp, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
					SetErrorMessage(strTemp);
					return FALSE;
				}
			
				pFileLog.SeekToEnd();
				
				//Write Header
				pFileLog.WriteString("Initial File Name, \n");
				pFileLog.WriteString("Bin Block Unload Date, \n");
				pFileLog.WriteString("Bin Block Unload Time, \n");
				strTemp.Format("Bin Block, %ld\n", lEmptyGrade);
				pFileLog.WriteString(strTemp);
				//Sorter
				CString szMcNo;
				szMcNo = GetMachineNo();
				strTemp.Format("Sorter, %s\n", (LPCTSTR) szMcNo);
				pFileLog.WriteString(strTemp);
				//Average die rotation
				pFileLog.WriteString("Average die rotation, 0.00\n");
				//Number of visual defects
				pFileLog.WriteString("Number of visual defects, 0\n");
				//Number of Good die
				pFileLog.WriteString("Number of Good die, 0\n");
				//Package File Name
				pFileLog.WriteString("Package File Name, \n");
				pFileLog.WriteString("\nNumerical data is as follows: WaferX, WaferY, DiesheetX, DieSheetY\n\n");
				//Application sw version
				GetRtnForSwVerNo(&strTemp);
				pFileLog.WriteString(strTemp+"\n");
				//Map file name
				szWaferId = GetMapNameOnly();
				//Ensure the WaferID is shown in the file
				if (szWaferId.IsEmpty() == TRUE)
				{
					m_WaferMapWrapper.GetWaferID(szWaferId);

					INT nIndex = szWaferId.ReverseFind('.');
						
					if ( nIndex != -1 )
					{
						szWaferId = szWaferId.Left(nIndex);
					}
				}

				pFileLog.WriteString("Begin "+szWaferId+"\n");
			}
			else	//Write the rest error found
			{
				bOutput = pFileLog.Open(strLocalFileNmLog, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText);

				if ( !bOutput ) 
				{
					strTemp = szTheme + "Fails to open SDS file.";
					HmiMessage_Red_Back(strTemp, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
					SetErrorMessage(strTemp);
					return FALSE;
				}

				pFileLog.SeekToEnd();
			}

			//Write Data
			strTemp.Format("%3ld,%3ld,%3lu,%3lu\n",	lUserCol, lUserRow,	ulCol, ulRow);
			pFileLog.WriteString(strTemp);

			pFileLog.Close();

			if ( CopyFile(strLocalFileNmLog, strFileNmLog, FALSE) == FALSE)
			{
				strTemp = szTheme + "Fail to copy SDS file to the network path!";
				SetErrorMessage(strTemp);
			}

			//**************************************************************//
			// to handler for result set to files for CREE CIMS
			if( m_lEmptyGradeMinCount==0 )
			{
				LONG lTmp;
				strTemp = szTheme + "Error!\nFound normal die!";
				lTmp = HmiMessage_Red_Back(strTemp, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
				SetCtmLogMessage(strTemp);
				if( lTmp == glHMI_STOP )
				{
					SetCtmLogMessage("User chooses STOP!");
					return FALSE;
				}
				else
				{
					SetCtmLogMessage("User chooses CONTINUE!");
				}
			}

			if (ulErrorCount>20)
			{
				break;
			}
		}
		if (ulErrorCount>20)
		{
			break;
		}
	}

	if( m_lEmptyGradeMinCount>0 && ulOkCount<(ULONG)m_lEmptyGradeMinCount && ulErrorCount>0 )
	{
		SetCtmLogMessage(szTheme + "FAIL!");
		return FALSE;
	}

	SetCtmLogMessage(szTheme + "FINISH!");

	return TRUE;
}


BOOL CWaferTable::GetPrescanRunWftPosn(unsigned long ulRow, unsigned long ulCol, LONG &lPhyX, LONG &lPhyY)
{
	return GetPrescanWftPosn(ulRow, ulCol, lPhyX, lPhyY);
}

BOOL CWaferTable::WftCheckCurrentGoodDie()	// search and display and get result (no offset)
{
	LONG lX = GetCurrX();
	LONG lY = GetCurrY();
	return WftSearchNormalDie(lX, lY, FALSE);
}

BOOL CWaferTable::WftSearchNormalDie(LONG &lPosX, LONG &lPosY, BOOL bDoComp)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	REF_TYPE	stInfo;
	SRCH_TYPE	stSrchInfo;

	//Init Message
	stSrchInfo.bNormalDie	= TRUE;
	stSrchInfo.lRefDieNo	= 1;
	stSrchInfo.bShowPRStatus = TRUE;
	stSrchInfo.bDisableBackupAlign = FALSE;

	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if( (stInfo.bStatus == FALSE) )	// PR error
	{
		return FALSE;
	}

	if( (stInfo.bFullDie == FALSE) )	// empty die
	{
		return FALSE;
	}

	lPosX += stInfo.lX;	
	lPosY += stInfo.lY;
	if( bDoComp )
	{
		//Move table to updated position
		if (IsWithinWaferLimit(lPosX, lPosY) == FALSE)	// OUT OF WAFER LIMIT
		{
			return FALSE;
		}

		XY_SafeMoveTo(lPosX, lPosY);
		Sleep(20);
	}

	if( (stInfo.bGoodDie==FALSE) )
	{
		return TRUE;	//Defect die found
	}

	return TRUE;
}


BOOL CWaferTable::CreateDummyMapForPrescanPicknPlace(CONST BOOL bAlignWithRefer, CONST BOOL bVirtualHome)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	ObtainContourEdgeDetail();
	if( m_bUseContour && IsBurnIn()==FALSE && m_bContourPreloadMap )
	{
		if( SwapContourPreloadMap() )
		{
			return TRUE;
		}
	}

	CString szFileName = m_szPrescanLogPath + PRESCAN_MOVE_POSN;
	remove(szFileName);

	ClearPrescanRawData();	// when do wafer alignment, clear data again.

	X_Sync();
	Y_Sync();
	T_Sync();
	GetEncoderValue();
	LONG lAlnWfX = GetCurrX();
	LONG lAlnWfY = GetCurrY();
	BOOL bNormalMap = pApp->GetCustomerName()!="Epitop";
	if( bVirtualHome && bNormalMap )
	{
		GetHomeDiePhyPosn(lAlnWfX, lAlnWfY);
	}
	else
	{
		AutoSearchWaferLimit();
		if( IsToAutoSearchWaferLimit() )
		{
			XY_SafeMoveTo(lAlnWfX, lAlnWfY);
			SaveScanTimeEvent("    WFT: Auto Search Wafer Limit complete!");
		}
	}

	if( pUtl->GetPrescanDummyMap()==FALSE )	// create dummy map for pick and place
	{
		return TRUE;
	}

	CString szLogMsg;
	szLogMsg.Format("    WFT: Dummy scan to create wafer map AlignWithRefer = %d; VirtualHome = %d; at %d,%d",
		bAlignWithRefer, bVirtualHome, lAlnWfX, lAlnWfY);
	SaveScanTimeEvent(szLogMsg);
	m_ucQuarterWaferCenter = 0;
	if( m_bDummyQuarterWafer )	// to decide the corner of quarter wafer
	{
		ULONG ulMaxRow, ulMaxCol, ulTickRow, ulTickCol;
		if( IsWaferMapValid()==FALSE || !m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol))
		{
			HmiMessage_Red_Back("No map file loaded or format is wrong, please reload it again!", "Wafer Align");
			return FALSE;
		}
		m_WaferMapWrapper.GetSelectedPosition(ulTickRow, ulTickCol);
		if(ulTickRow<ulMaxRow/2 )
		{
			if( ulTickCol<ulMaxCol/2 )
			{
				m_ucQuarterWaferCenter = 0;	// UP LEFT
			}
			else
			{
				m_ucQuarterWaferCenter = 1;	// UP RIGHT
			}
		}
		else
		{
			if( ulTickCol<ulMaxCol/2 )
			{
				m_ucQuarterWaferCenter = 2;	// LOW LEFT
			}
			else
			{
				m_ucQuarterWaferCenter = 3;	// LOW RIGHT
			}
		}
	}


	LONG lDiePitchX_X, lDiePitchX_Y, lDiePitchY_Y, lDiePitchY_X;
	LONG lLftX, lRgtX, lTopY, lBtmY;
	LONG ulNoOfRows, ulNoOfCols;
	LONG ulNoOfRowsTop = 0, ulNoOfRowsBtm = 0, ulNoOfColsLft = 0, ulNoOfColsRgt = 0;

	CString szMsg;
	LONG lDummyMapDiameter	= GetPrescanDiameter();
	LONG lDummyMapCenterX	= GetPrescanCenterX();
	LONG lDummyMapCenterY	= GetPrescanCenterY();
	if( IsPrescanSizeSet() )	// recreate dummy map, align position as map center
	{
		lDummyMapDiameter = GetPrescanDiameter();
		lDummyMapCenterX	= lAlnWfX;
		lDummyMapCenterY	= lAlnWfY;
	}
	lLftX = lDummyMapCenterX + lDummyMapDiameter/2;
	lRgtX = lDummyMapCenterX - lDummyMapDiameter/2;
	lTopY = lDummyMapCenterY + lDummyMapDiameter/2;
	lBtmY = lDummyMapCenterY - lDummyMapDiameter/2;
	if( m_ucPrescanMapShapeType!=WT_MAP_SHAPE_CIRCLE )
	{
		lLftX = lDummyMapCenterX + m_lAutoWaferWidth/2;
		lRgtX = lDummyMapCenterX - m_lAutoWaferWidth/2;
		lTopY = lDummyMapCenterY + m_lAutoWaferHeight/2;
		lBtmY = lDummyMapCenterY - m_lAutoWaferHeight/2;
	}

	CString szAlgorithm = "TLH";
	CString szPath = "LocalMinDistance";
	if( m_bDummyQuarterWafer )	// to define the limit area and algorithm
	{
		switch( m_ucQuarterWaferCenter )	// map increase direction is opposite to table direction, define limit and algorithm
		{
		case 0:
			lLftX = lDummyMapCenterX + lDummyMapDiameter/20;
			lRgtX = lDummyMapCenterX - lDummyMapDiameter/2;
			lTopY = lDummyMapCenterY + lDummyMapDiameter/20;
			lBtmY = lDummyMapCenterY - lDummyMapDiameter/2;
			szAlgorithm = "TLH";
		break;
		case 1:
			lLftX = lDummyMapCenterX + lDummyMapDiameter/2;
			lRgtX = lDummyMapCenterX - lDummyMapDiameter/20;
			lTopY = lDummyMapCenterY + lDummyMapDiameter/20;
			lBtmY = lDummyMapCenterY - lDummyMapDiameter/2;
			szAlgorithm = "TRH";
			break;
		case 2:
			lLftX = lDummyMapCenterX + lDummyMapDiameter/20;
			lRgtX = lDummyMapCenterX - lDummyMapDiameter/2;
			lTopY = lDummyMapCenterY + lDummyMapDiameter/2;
			lBtmY = lDummyMapCenterY - lDummyMapDiameter/20;
			szAlgorithm = "BLH";
			break;
		case 3:
			lLftX = lDummyMapCenterX + lDummyMapDiameter/2;
			lRgtX = lDummyMapCenterX - lDummyMapDiameter/20;
			lTopY = lDummyMapCenterY + lDummyMapDiameter/2;
			lBtmY = lDummyMapCenterY - lDummyMapDiameter/20;
			szAlgorithm = "BRH";
			break;
		}
	}

	lDiePitchX_X	= GetDiePitchX_X();
	lDiePitchX_Y	= GetDiePitchX_Y();
	lDiePitchY_X	= GetDiePitchY_X();
	lDiePitchY_Y	= GetDiePitchY_Y();
	lDiePitchX_X	= labs(lDiePitchX_X);
	lDiePitchY_Y	= labs(lDiePitchY_Y);

	if (lDiePitchY_Y != 0)
	{
		ulNoOfRowsTop = labs((lTopY-lAlnWfY) / lDiePitchY_Y);
		ulNoOfRowsBtm = labs((lAlnWfY-lBtmY) / lDiePitchY_Y);
		ulNoOfRowsTop += 5;
		ulNoOfRowsBtm += 5;
	}

	if (lDiePitchX_X != 0)
	{
		ulNoOfColsLft = labs((lLftX-lAlnWfX) / lDiePitchX_X);
		ulNoOfColsRgt = labs((lAlnWfX-lRgtX) / lDiePitchX_X);
		ulNoOfColsLft += 5;
		ulNoOfColsRgt += 5;
	}

	ulNoOfRows = ulNoOfRowsTop + ulNoOfRowsBtm;
	ulNoOfCols = ulNoOfColsLft + ulNoOfColsRgt;

	m_lDummyMapMaxRow = ulNoOfRows;
	m_lDummyMapMaxCol = ulNoOfCols;

	szLogMsg.Format("    WFT: Dummy map range UL %ld,%ld, LR %ld,%ld, sizeset %d, shape %d, RC %ld,%ld", 
		lLftX, lTopY, lRgtX, lBtmY, IsPrescanSizeSet(), m_ucPrescanMapShapeType,
		ulNoOfRows, ulNoOfCols);
	SaveScanTimeEvent(szLogMsg);

	LONG lAlnRow, lAlnCol;
	if( IsMS90HalfSortMode() || m_bFixDummyMapCenter )
	{
		lAlnRow = m_lHomeDieMapRow;
		lAlnCol = m_lHomeDieMapCol;
		if( IsMS90Sorting2ndPart() )
		{
			lAlnRow = m_lMS902ndHomeDieMapRow;
			lAlnCol = m_lMS902ndHomeDieMapCol;
		}
	}
	else
	{
		lAlnRow = ulNoOfRowsTop;
		lAlnCol = ulNoOfColsLft;
	}
	
	if( pApp->GetCustomerName()=="FATC" || m_szScanAoiMapFormat==_T("EpiTech") )
	{
		int nRowDir = 1, nColDir = 1;
		if( m_lMa3MapDirY!=0 )
			nRowDir = -1;
		if( m_lMa3MapDirX!=0 )
			nColDir = -1;
		lAlnRow = m_lMa3AHomeRow*nRowDir;
		lAlnCol = m_lMa3AHomeCol*nColDir;
	}

	CString szDummyScanMapPath;
	CString szDummyScanPsmPath, szDummyScanScnPath;

	//Clear Wafermap & reset grade map
	m_WaferMapWrapper.InitMap();
//	m_WaferMapWrapper.ResetGradeMap();
	SetPsmEnable(FALSE);

	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format("%Y%m%d%H%M%S");

	m_stReadTmpTime = theTime;

	if (m_szDummyWaferMapName.IsEmpty())
	{
		szDummyScanMapPath = szTime;
	}
	else
	{
		szDummyScanMapPath = m_szDummyWaferMapName;
		if( m_bScanMapAddTimeStamp )
		{
			szDummyScanMapPath += szTime;
		}
	}

	szDummyScanPsmPath = PRESCAN_RESULT_FULL_PATH + szDummyScanMapPath + ".PSM";
	szDummyScanScnPath = PRESCAN_RESULT_FULL_PATH + szDummyScanMapPath + ".SCN";
	if ( m_szMapFileExtension.IsEmpty() == FALSE)
	{
		szDummyScanMapPath = szDummyScanMapPath + "." + m_szMapFileExtension;
	}
	BOOL bESDummyBar = pUtl->GetPrescanDummyMap() && pUtl->GetPrescanAoiWafer() && pUtl->GetPrescanBarWafer();
	if( IsOcrAOIMode() && IsCharDieInUse() && bESDummyBar )
	{
		CString szSrvFullName	= m_szMapFilePath + "\\" + szDummyScanMapPath;
		CString szLclFullName	= PRESCAN_RESULT_FULL_PATH + "OcrBarWafer.xxx";
		DeleteFile(szLclFullName);
		CopyFileWithRetry(szSrvFullName, szLclFullName, FALSE);
	}

	szDummyScanMapPath = PRESCAN_RESULT_FULL_PATH + szDummyScanMapPath;

	m_szCurrentMapPath = szDummyScanMapPath;

	SaveScanTimeEvent("    WFT: PRESCAN Create dummy map " + szDummyScanMapPath);

	remove(szDummyScanPsmPath);
	remove(szDummyScanScnPath);

	FILE *fpMap = NULL;
	errno_t nErr = fopen_s(&fpMap, szDummyScanMapPath, "w");
	if ((nErr == 0) && (fpMap != NULL))
	{
		if( bNormalMap )
		{
			fprintf(fpMap, "DataFileName,,%s\n", (LPCTSTR) m_szCurrentMapPath);	//v4.28T6	//Klocwork
			fprintf(fpMap, "LotNumber,,\n");
			fprintf(fpMap, "DeviceNumber,,\n");
			fprintf(fpMap, "wafer id=\n");
			fprintf(fpMap, "TestTime,%s,%s\n", (LPCTSTR) theTime.Format("%Y%m%d"), (LPCTSTR) theTime.Format("%H%M%S"));
			fprintf(fpMap, "MapFileName,,%s\n", (LPCTSTR) m_szCurrentMapPath);		//v4.28T6	//Klocwork
			fprintf(fpMap, "TransferTime,%s,%s\n", (LPCTSTR) theTime.Format("%Y%m%d"), (LPCTSTR) theTime.Format("%H%M%S"));
			fprintf(fpMap, "\n");
			fprintf(fpMap, "map data\n");
			fprintf(fpMap, "%ld,%ld,\n", lAlnCol, lAlnRow);
		}
		else
		{
			fprintf(fpMap, "DataFileName,,,,,,,,,,,,,\n");
			fprintf(fpMap, "LotNumber,,,,,,,,,,,,,\n");
			fprintf(fpMap, "Specification,,,,,,,,,,,,,\n");
			fprintf(fpMap, "TestTime,,,,,,,,,,,,,\n");
			fprintf(fpMap, "TesterNumber,,,,,,,,,,,,,\n");
			fprintf(fpMap, "Operator,,,,,,,,,,,,,\n");
			fprintf(fpMap, "CustomerRemark,,,,,,,,,,,,,\n");
			fprintf(fpMap, ",,,,,,,,,,,,,\n");
			fprintf(fpMap, "TotalTested,,,,,,,,,,,,,\n");
			fprintf(fpMap, ",,,,,,,,,,,,,\n");
			fprintf(fpMap, "TEST,PosX,PosY,BIN,,,,,,,,,,\n");
		}
	}

	LONG lEncX,lEncY, lDiff_X, lDiff_Y, lMapRow, lMapCol;
	LONG lFirstRow =lAlnRow , lFirstCol = lAlnCol, lLastRow = lAlnRow, lLastCol = lAlnCol;
	BOOL bFirst = FALSE;
	// UL part
	LONG	lDieIndex = 0;
	LONG lCheckDiameter = lDummyMapDiameter;

	LONG lScanMapMaxX = lAlnWfX;
	LONG lScanMapMinX = lAlnWfX;
	LONG lScanMapMaxY = lAlnWfY;
	LONG lScanMapMinY = lAlnWfY;

	if( m_bContourEdge )
	{
		lScanMapMaxX = m_lCntrEdgeX_L[0];
		lScanMapMinX = m_lCntrEdgeX_R[0];
		for(int i = 1; i < (LONG)m_ulContourEdgePoints; i++)
		{
			if( lScanMapMaxX<m_lCntrEdgeX_L[i] )
				lScanMapMaxX = m_lCntrEdgeX_L[i];
			if( lScanMapMinX>m_lCntrEdgeX_R[i] )
				lScanMapMinX = m_lCntrEdgeX_R[i];
		}
		lScanMapMaxY =	m_lCntrEdgeY_C[0] + GetPrescanPitchY()/2;
		lScanMapMinY =	m_lCntrEdgeY_C[0] - GetPrescanPitchY()/2;
		if( m_ulContourEdgePoints>0 )
			lScanMapMinY =	m_lCntrEdgeY_C[m_ulContourEdgePoints-1] - GetPrescanPitchY()/2;
	}

	BOOL bCheckMapRange = FALSE;
	LONG lRangeUpRow = lAlnRow, lRangeUpCol = 0, lRangeDnRow = lAlnRow, lRangeDnCol = 0;

	for(LONG lRow=0; lRow<=ulNoOfRows; lRow++)
	{
		bFirst = FALSE;
		for(LONG lCol=0; lCol<=ulNoOfCols; lCol++)
		{
			lDiff_X = lCol - ulNoOfColsLft;
			lDiff_Y = lRow - ulNoOfRowsTop;

			lEncX = lAlnWfX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lEncY = lAlnWfY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;

			if( lDiff_X==0 && lDiff_Y==0 && bAlignWithRefer && pUtl->GetPrescanAoiWafer() )
			{
				if( m_bContourEdge==FALSE )
					continue;
			}

			if( m_bContourEdge )
			{
				if( lEncY>lScanMapMaxY || lEncY<lScanMapMinY )
				{
					continue;
				}
				if( lEncX>lScanMapMaxX || lEncX<lScanMapMinX )
				{
					continue;
				}
			}

			int nRowDir = 1, nColDir = 1;
			lMapRow = lDiff_Y*nRowDir + lAlnRow;
			lMapCol = lDiff_X*nColDir + lAlnCol;

			BOOL bPosnOK = FALSE;
			if( m_ucPrescanMapShapeType!=WT_MAP_SHAPE_CIRCLE )
			{
				if( lEncX<=lLftX && lEncX>=lRgtX && lEncY<=lTopY && lEncY>=lBtmY )
				{
					bPosnOK = TRUE;
				}
			}
			else
			{
				if( IsWithinLimit(lEncX, lEncY, lDummyMapCenterX, lDummyMapCenterY, lCheckDiameter) )
				{
					bPosnOK = TRUE;
				}
			}

			if( m_bContourEdge )
			{
				LONG lNearestRow = 0;
				LONG lNearestY = labs(lEncY - m_lCntrEdgeY_C[0]);
				for(int kk = 0; kk < (LONG)m_ulContourEdgePoints; kk++)
				{
					if( labs(lEncY-m_lCntrEdgeY_C[kk])<lNearestY )
					{
						lNearestY = labs(lEncY-m_lCntrEdgeY_C[kk]);
						lNearestRow = kk;
					}
				}

				if( lEncX>(m_lCntrEdgeX_L[lNearestRow]) || 
					lEncX<(m_lCntrEdgeX_R[lNearestRow]) )
				{
					bPosnOK = FALSE;
				}
			}

			if( bCheckMapRange )
			{
				if( lMapCol<lRangeUpCol || lMapCol>lRangeDnCol || lMapRow<lRangeUpRow || lMapRow>lRangeDnRow )
				{
					bPosnOK = FALSE;
				}
			}

			if( bPosnOK )
			{
				if( fpMap!=NULL && bFirst==FALSE )
				{
					lDieIndex++;
					if( bNormalMap )
					{
						fprintf(fpMap, "%ld,%ld,%d\n", lMapCol, lMapRow, m_ucDummyPrescanPNPGrade);
					}
					else
					{
						fprintf(fpMap, "%ld,%ld,%ld,%d,,,,,,,,,,\n", lDieIndex, lMapCol, lMapRow, m_ucDummyPrescanPNPGrade);
					}
				}
				if( bFirst==FALSE && bNormalMap )
				{
					lFirstRow = lMapRow;
					lFirstCol = lMapCol;
					if( !IsBurnIn() && bAlignWithRefer && pUtl->GetPrescanAoiWafer() && IsES101()==FALSE && m_bContourEdge==FALSE )
						bFirst = TRUE;
				}
				lLastRow = lMapRow;
				lLastCol = lMapCol;
			}
		}
		if( bFirst && lFirstCol!=lLastCol && fpMap!=NULL )
		{
			fprintf(fpMap, "%ld,%ld,%d\n", lLastCol, lLastRow, m_ucDummyPrescanPNPGrade);
		}
	}
	if( fpMap!=NULL )
	{
		fclose(fpMap);
	}

	LONG lLoopTimer = 0;
	CMS896AApp::m_bMapLoadingFinish	= FALSE;
	if( LoadWaferMap(szDummyScanMapPath, "")==FALSE )	// load dummy map
	{
		SaveScanTimeEvent("    WFT: Load map FALSE");
		return FALSE;
	}

	if( IsBurnIn() )
	{
		m_szBurnInMapFile = szDummyScanMapPath;
	}

	BOOL bWaitMap = TRUE;
	if( IsMS90HalfSortDummyScanAlign() )
	{
		bWaitMap = FALSE;
	}
	if( bWaitMap )
	{
		Sleep(500);
		lLoopTimer = 0;
		while( CMS896AApp::m_bMapLoadingFinish==FALSE )
		{
			Sleep(100);
			lLoopTimer++;
			if( lLoopTimer>=1200 )
			{
				SaveScanTimeEvent("    WFT: Load dummy map time out (loading un-finish)");
				return FALSE;
			}
		}
		lLoopTimer = 0;
		while (!IsWaferMapValid())
		{
			Sleep(100);
			lLoopTimer++;
			if( lLoopTimer>=1200 )
			{
				SaveScanTimeEvent("    WFT: Load dummy map time out (map invalid)");
				return FALSE;
			}
		}
		SaveScanTimeEvent("    WFT: after load dummy map complete");
		unsigned char aaTempGrades[1];
		aaTempGrades[0] = m_WaferMapWrapper.GetGradeOffset() + m_ucDummyPrescanPNPGrade;

		ULONG ulAlnRow, ulAlnCol;
		GetMapAlignHomeDie(ulAlnRow, ulAlnCol);
		m_WaferMapWrapper.SetSelectedPosition(ulAlnRow, ulAlnCol);
		m_WaferMapWrapper.SetCurrentPosition(ulAlnRow, ulAlnCol);

		m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aaTempGrades[0]);

		m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);

		if( m_bDummyQuarterWafer )	// to select algorithn
		{
			m_WaferMapWrapper.SelectAlgorithm(szAlgorithm, "Direct");
		}
		SetCtmRefDieOptions();	// set 5 point check die as refer die for dummy wafer prescan
		SaveScanTimeEvent("    WFT: dummy map creation complete");
	}

	return TRUE;
}

BOOL CWaferTable::RectWaferCornerCheck(VOID)
{
	if( m_bSearchHomeOption==WT_SPIRAL_SEARCH && (m_bMapDummyScanCornerCheck==FALSE) )
	{
		return TRUE;
	}

	if( IsMS90HalfSortMode() && (m_bMapDummyScanCornerCheck==FALSE) )
	{
		return TRUE;
	}

	LONG lMinRow = 0, lMaxRow = 0;
	LONG lMinCol = 0, lMaxCol = 0;
	GetMapValidSize(lMaxRow, lMinRow, lMaxCol, lMinCol);

	if ((lMaxRow - lMinRow) <= 1 && (lMaxCol - lMinCol) <= 1)
	{
		return TRUE;
	}
 
	SaveScanTimeEvent("    WFT: Rect Wafer Corner Check");

	MultiSrchInitNmlDie1();

	// check UP LEFT CORNER DIE
	ULONG	ulRow, ulCol;
	ulRow = lMinRow;
	ulCol = lMinCol;
	if( RectWaferSingleCornerCheck(ulRow, ulCol, 0)==FALSE )
	{
		return FALSE;
	}

	// check UP RIGHT CORNER DIE
	ulRow = lMinRow;
	ulCol = lMaxCol;
	if( RectWaferSingleCornerCheck(ulRow, ulCol, 1)==FALSE )
	{
		return FALSE;
	}

	if( IsMS90HalfSortMode() )
	{
		return TRUE;
	}

	// check LOW LEFT CORNER DIE
	ulRow = lMaxRow;
	ulCol = lMinCol;
	if( RectWaferSingleCornerCheck(ulRow, ulCol, 2)==FALSE )
	{
		return FALSE;
	}

	// check LOW RIGHT CORNER DIE
	ulRow = lMaxRow;
	for(ulCol = lMaxCol; ulCol>0; ulCol--)
	{
		if (m_pWaferMapManager->IsMapHaveBin(ulRow, ulCol))
			break;
	}
	if( ulCol==lMinCol )
	{
		return TRUE;
	}
	if( RectWaferSingleCornerCheck(ulRow, ulCol, 3)==FALSE )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::RectWaferSingleCornerCheck(ULONG ulRow, ULONG ulCol, UCHAR ucCorner)
{
	//
	//		1
	//	0	4	2
	//		3
	//
	LONG	lPosX, lPosY, lMoveX, lMoveY;
	CString strTemp, szTitle;
	szTitle = "Prescan Corner Check";

	LONG	lDiePitchX_X = 0, lDiePitchX_Y = 0, lDiePitchY_X = 0, lDiePitchY_Y = 0;
	LONG	lColDiff = 0, lRowDiff = 0;
	lDiePitchX_X = GetDiePitchX_X(); 
	lDiePitchX_Y = GetDiePitchX_Y();
	lDiePitchY_X = GetDiePitchY_X();
	lDiePitchY_Y = GetDiePitchY_Y();

	MULTI_SRCH_RESULT	stMsch;
	IPC_CServiceMessage stMsg;

	if( ucCorner==0 )
		strTemp.Format("UL Corner verify (%d,%d), \n", ulRow, ulCol);
	if( ucCorner==1 )
		strTemp.Format("UR Corner verify (%d,%d), \n", ulRow, ulCol);
	if( ucCorner==2 )
		strTemp.Format("LL Corner verify (%d,%d), \n", ulRow, ulCol);
	if( ucCorner==3 )
		strTemp.Format("LR Corner verify (%d,%d), \n", ulRow, ulCol);

	m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
	Sleep(100);
	m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
	Sleep(100);
	if( GetDieValidPrescanPosn(ulRow, ulCol, 10, lPosX, lPosY)!=FALSE )
	{
		WftMoveSearchDie(lPosX, lPosY, TRUE);
		GetEncoderValue();
		lPosX = GetCurrX();
		lPosY = GetCurrY();
		(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lPosX;
		(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lPosY;
		ChangeCameraToWafer();
		BOOL bDrawDie = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bDrawDie);
		int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MultiSearchNmlDie1", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		stMsg.GetMsg(sizeof(MULTI_SRCH_RESULT), &stMsch);

		if( ucCorner==0 ) // UL
		{
			if( stMsch.bDieState[0]!=0 || stMsch.bDieState[1]!=0 )
			{
				return HmiAlarmContinueMessage(strTemp + "Find Die outer by LF", szTitle);
			}
			else
			{
				if( IsLFSizeOK()==FALSE )
				{
					lColDiff = -1;
					lRowDiff = 0;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die left", szTitle)==FALSE )
							return FALSE;
					}

					lColDiff = -1;
					lRowDiff = -1;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die left up", szTitle)==FALSE )
							return FALSE;
					}

					lColDiff = 0;
					lRowDiff = -1;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die up", szTitle)==FALSE )
							return FALSE;
					}
				}
			}
		}

		if( ucCorner==1 ) // UR
		{
			if( stMsch.bDieState[1]!=0 || stMsch.bDieState[2]!=0 )
			{
				return HmiAlarmContinueMessage(strTemp + "Find Die outer by LF", szTitle);
			}
			else
			{
				if( IsLFSizeOK()==FALSE )
				{
					lColDiff = 0;
					lRowDiff = -1;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die up", szTitle)==FALSE )
							return FALSE;
					}

					lColDiff = 1;
					lRowDiff = -1;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die right up", szTitle)==FALSE )
							return FALSE;
					}

					lColDiff = 1;
					lRowDiff = 0;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die right", szTitle)==FALSE )
							return FALSE;
					}
				}
			}
		}

		if( ucCorner==2 ) // LL
		{
			if( stMsch.bDieState[0]!=0 || stMsch.bDieState[3]!=0 )
			{
				return HmiAlarmContinueMessage(strTemp + "Find Die outer by LF", szTitle);
			}
			else
			{
				if( IsLFSizeOK()==FALSE )
				{
					lColDiff = 0;
					lRowDiff = 1;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die down", szTitle)==FALSE )
							return FALSE;
					}

					lColDiff = -1;
					lRowDiff = 1;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die left down", szTitle)==FALSE )
							return FALSE;
					}

					lColDiff = -1;
					lRowDiff = 0;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die left", szTitle)==FALSE )
							return FALSE;
					}
				}
			}
		}

		if( ucCorner==3 ) // LR
		{
			if( stMsch.bDieState[2]!=0 || stMsch.bDieState[3]!=0 )
			{
				if( stMsch.bDieState[3]!=0 )
					return HmiAlarmContinueMessage(strTemp + "Find Die lower by LF", szTitle);
				ULONG	ulNumOfRow=0, ulNumOfCol=0;
				m_pWaferMapManager->GetWaferMapDimension(ulNumOfRow, ulNumOfCol);
				if( ulCol==ulNumOfCol-1 && stMsch.bDieState[2]!=0 )
					return HmiAlarmContinueMessage(strTemp + "Find Die outer by LF", szTitle);
			}
			else
			{
				if( IsLFSizeOK()==FALSE )
				{
					lColDiff = 1;
					lRowDiff = 0;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die right", szTitle)==FALSE )
							return FALSE;
					}

					lColDiff = 1;
					lRowDiff = 1;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die right down", szTitle)==FALSE )
							return FALSE;
					}

					lColDiff = 0;
					lRowDiff = 1;
					lMoveX = lPosX - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
					lMoveY = lPosY - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;

					if( WftMoveSearchDie(lMoveX, lMoveY) == TRUE )
					{
						if( HmiAlarmContinueMessage(strTemp + "Move Find Die down", szTitle)==FALSE )
							return FALSE;
					}
				}
			}
		}
	}
	else
	{
		return HmiAlarmContinueMessage(strTemp + "no physical position after prescan", szTitle);
	}

	return TRUE;
}


BOOL CWaferTable::HmiAlarmContinueMessage(CString szMsg, CString szTitle)
{
	LONG lTmp = HmiMessage_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	SetCtmLogMessage(szMsg);
	if (lTmp == glHMI_STOP)
	{
		return FALSE;
	}
	else
	{
		SetCtmLogMessage("User chooses CONTINUE!");
	}

	return TRUE;
}

BOOL CWaferTable::WftMoveSearchDie(LONG &lCurX, LONG &lCurY, BOOL bDoComp)
{
	if( XY_SafeMoveTo(lCurX, lCurY)==FALSE )
		return FALSE;

	ChangeCameraToWafer();
	Sleep(50);
	return WftSearchNormalDie(lCurX, lCurY, bDoComp);
}

VOID CWaferTable::ChangeCameraToWafer()
{
	IPC_CServiceMessage stMsg;
	BOOL bCamera = 0;
	stMsg.InitMessage(sizeof(BOOL), &bCamera);
	m_comClient.SendRequest(WAFER_PR_STN, "SwitchToTarget", stMsg);
}

VOID CWaferTable::ChangeCameraToWaferBondMode(CONST BOOL bBondMode)
{
	SetAutoBondMode(bBondMode);

	WftCheckCurrentGoodDie();
}

LONG CWaferTable::EnablePrescanDebug(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable;

	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	if( CheckOperatorPassWord(PRESCAN_PASSWORD_SUPER_USER) )
		m_bPrescanDebugEnable = bEnable;
	else
		m_bPrescanDebugEnable = !bEnable;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_PRESCAN_DEBUG, (int)m_bPrescanDebugEnable);
	pUtl->SetPrescanDebug(m_bPrescanDebugEnable);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferTable::SearchAndRemoveKeyFiles(CString szPath, CString szFileKeyWord)
{
	WIN32_FIND_DATA FileData;
	CString szFilename;
	
	HANDLE hSearch = FindFirstFile(szPath + "\\" + "*" + szFileKeyWord, &FileData);

	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return FALSE;
	}

	do 
	{
		szFilename = szPath + "\\" + FileData.cFileName;
		DeleteFile(szFilename);
	}while (FindNextFile(hSearch, &FileData) == TRUE);
 
	// Close the search handle. 
	if ( FindClose(hSearch) == FALSE) 
	{ 
		return FALSE;
	} 

	return TRUE;
}

LONG CWaferTable::EnablePrescanTwice(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

VOID CWaferTable::UiSaveMapFile(CString strFileName)
{
	CString		strFile,strTemp;
	CString szSrcFilePath;
	CString szCurrMapFile;

	if( GetPsmEnable()==FALSE )
		return ;

	szCurrMapFile = MS_LCL_CURRENT_MAP_FILE;
	if( strFileName.CompareNoCase(szCurrMapFile)!=0 )
	{
		remove(strFileName);
		CopyFile(szCurrMapFile, strFileName, FALSE);
	}

	int		iPstnCol;
	CString drive, directory, fileName, extension;

	_splitpath( strFileName.GetBuffer(MAX_PATH),
				drive.GetBuffer(_MAX_DRIVE), directory.GetBuffer(_MAX_DIR),
				fileName.GetBuffer(_MAX_FNAME), extension.GetBuffer(_MAX_EXT) );
	strFileName.ReleaseBuffer();
	drive.ReleaseBuffer();
	directory.ReleaseBuffer();
	fileName.ReleaseBuffer();
	extension.ReleaseBuffer();

	iPstnCol = strFileName.ReverseFind('.');
	if ( iPstnCol == -1 )
	{
		strFile = strFileName + ".PSM";	
	}
	else
	{
		strTemp = strFileName.Left(iPstnCol);
		strFile = strTemp + ".PSM";	
	}
}

VOID CWaferTable::SetPsmMapMachineNo(CONST BOOL bNew, CONST CString szMcNo)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( bNew==TRUE )
	{
		m_szPsmMapMachineNo = szMcNo;
	}
	else
	{
		m_szPsmMapMachineNo = GetMachineNo();
	}
	pUtl->SetPsmMapMcNo(m_szPsmMapMachineNo);
}

LONG CWaferTable::CheckSuperUserPassword(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	if( CheckOperatorPassWord(PRESCAN_PASSWORD_SUPER_USER) )
	{
		bReturn = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckSemitekSUPW(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_SEMITEK )
	{
		if( !CheckOperatorPassWord(PRESCAN_PASSWORD_SUPER_USER) )
		{
			bReturn = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CheckBWMachinePassword(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	if( CheckOperatorPassWord(PRESCAN_PASSWORD_SUPER_USER) )
	{
		bReturn = TRUE;
	}
	else
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CWaferTable::RePrescanWafer(IPC_CServiceMessage &svMsg)
{
	CString strTemp;
	strTemp.Format("Do you want to RE-PRESCAN wafer with current map?");
	LONG lTmp = HmiMessage(strTemp, "Re-prescan wafer", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300, NULL, NULL, NULL, NULL);
	if( lTmp==glHMI_YES)
	{
		SetAlignmentStatus(FALSE);
		strTemp = "Prescan result reset!\n\nPlease align wafer again.\nThen press start button to prescan wafer and sorting.";
		HmiMessage(strTemp, "Re-prescan wafer");
	}

	BOOL bReturn =TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


BOOL CWaferTable::IsDiePitchCheck()
{
	return m_bDiePitchCheck;
}


BOOL CWaferTable::IsDiePitchCheck(const LONG lRow, const LONG lCol)
{
	return IsDiePitchCheck() && !IsNGBlock(lRow, lCol);
}


BOOL CWaferTable::GetPrescanDiePitch()
{
	m_lPrescanDiePitchX_X = GetDiePitchX_X();
	m_lPrescanDiePitchX_Y = GetDiePitchX_Y();
	m_lPrescanDiePitchY_Y = GetDiePitchY_Y();
	m_lPrescanDiePitchY_X = GetDiePitchY_X();

	if( IsPrescanEnable() && m_bPrescanAutoDiePitch )
	{
		typedef struct 
		{
			int nPitchXX;
			int nPitchXY;
			int nPitchYY;
			int nPitchYX;
		} DIEPITCHDATA;
		DIEPITCHDATA stPitchResult;

		IPC_CServiceMessage stMsg;
		int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LFAutoLearnDiePitch", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		stMsg.GetMsg(sizeof(DIEPITCHDATA), &stPitchResult);

		m_lPrescanDiePitchX_X = stPitchResult.nPitchXX;
		m_lPrescanDiePitchX_Y = stPitchResult.nPitchXY;
		m_lPrescanDiePitchY_Y = stPitchResult.nPitchYY;
		m_lPrescanDiePitchY_X = stPitchResult.nPitchYX;
	}

	return TRUE;
}

LONG CWaferTable::UpdateScnOption(IPC_CServiceMessage &svMsg)
{
	CString szMsg;

	if( IsEnableFPC() )
	{
		szMsg.Format("enable");
	}
	else
	{
		szMsg.Format("disable");
	}
	SaveScanTimeEvent("    WFT: SCN result checking is " + szMsg);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferTable::BlkFunc2SearchCurrDie(LONG *lPosX, LONG *lPosY, BOOL bWholeWindow)
{
	BOOL bResult = FALSE;

	for(int i=0; i<2; i++)
	{
		if( bWholeWindow )
			bResult = m_pBlkFunc2->Blk2SearchReferOnlyInFOV(lPosX, lPosY, TRUE);
		else
			bResult = m_pBlkFunc2->Blk2SearchReferDieAtCurr(lPosX, lPosY);

		if( bResult )
			break;
		Sleep(200);
	}

	return bResult;
}

BOOL CWaferTable::ReverifyBlockPickReferDie()	// FOR REGION PRESCAN MAINLY
{
	if( m_bUseReVerifyRefDie==FALSE || IsBurnIn() )
		return TRUE;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	UINT			i;
	LONG		lX, lY, lT;
	LONG	lRow, lCol, lWfX, lWfY;
	BOOL		bResult;
	CString strTemp;
	CString szWaferId;
	CString		strFiducialsLog, strLocalFiducialsLog;
	BOOL		bOutput=FALSE;
	CStdioFile	pFileLog;
	LONG		lUserCol=0, lUserRow=0;
	LONG		lFiducialsFound = 0;

	SetCtmLogMessage("Prescan Start re-verify reference dice before wafer end.");

	BOOL bGenOutput = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( bGenOutput )
	{
		//Get Output Path
		strLocalFiducialsLog = "C:\\MapSorter\\OutputFiles\\Fiducials.txt";
		CString	strPathBin, strPathLog;
		bOutput = m_pBlkFunc2->Blk2GetIniOutputPath(&strPathBin, &strPathLog);
		if ( !bOutput ) 
		{
			strTemp.Format("    WFT: Prescan Fails to initial Fiducials log file path. Files will only be generated to local drive!");
			SaveScanTimeEvent(strTemp);
		}
		else
		{
			strFiducialsLog.Format("%s\\Fiducials.txt", strPathLog);
		}
	}

	//Search RefDie
	UINT uiReferNum = 0;
	if( pUtl->GetPrescanRegionMode() )
		uiReferNum = pUtl->GetReferMapPointsNum();
	else
		uiReferNum = pUtl->GetNumOfReferPoints();

	BOOL bWholeFOV = FALSE;
	if( IsOnlyRegionEnd() )
		bWholeFOV = TRUE;
	for(i=0; i<uiReferNum; i++)
	{
		if( pUtl->GetPrescanRegionMode() )
		{
			pUtl->GetReferMapPosition(i, lRow, lCol, lWfX, lWfY);
		}
		else
		{
			pUtl->GetReRefPosition(i, lRow, lCol, lWfX, lWfY);

			if( GetMapPhyPosn(lRow, lCol, lWfX, lWfY)==FALSE )
			{
				continue;
			}
		}

		if (XY_SafeMoveTo(lWfX, lWfY) == FALSE)
		{
			continue;
		}

		ChangeCameraToWafer();
		m_WaferMapWrapper.SetCurrentPosition(lRow, lCol);
		Sleep(50);
		m_WaferMapWrapper.SetCurrentPosition(lRow, lCol);

		GetEncoder(&lX,&lY,&lT);

		bResult = BlkFunc2SearchCurrDie(&lX, &lY, bWholeFOV);

		if( bResult )
		{
			continue;
		}

		lFiducialsFound++;
		ConvertAsmToOrgUser((ULONG)lRow, (ULONG)lCol,lUserRow,lUserCol);

		if( bGenOutput )
		{
			bOutput = pFileLog.Open(strLocalFiducialsLog, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText);
			if ( !bOutput )
			{
				SaveScanTimeEvent("    WFT: Prescan Fails to open Fiducials Log file");
			}
			pFileLog.SeekToEnd();

			if (lFiducialsFound == 1)
			{
				//Write Header
				pFileLog.WriteString("Initial File Name, \n");
				pFileLog.WriteString("Bin Block Unload Date, \n");
				pFileLog.WriteString("Bin Block Unload Time, \n");
				pFileLog.WriteString("Bin Block, \n");
				CString strMachineID	= GetMachineNo();
				strTemp.Format("Sorter, %s\n", strMachineID);
				pFileLog.WriteString(strTemp);
				pFileLog.WriteString("Average die rotation, 0.00\n");
				pFileLog.WriteString("Number of visual defects, 0\n");
				pFileLog.WriteString("Number of Good die, 0\n");
				pFileLog.WriteString("Package File Name, \n");
				pFileLog.WriteString("\nNumerical data is as follows: WaferX, WaferY, DiesheetX, DieSheetY\n\n");
				GetRtnForSwVerNo(&strTemp);
				pFileLog.WriteString(strTemp+"\n");

				szWaferId = GetMapNameOnly();
				//Ensure the WaferID is shown in the file
				if (szWaferId.IsEmpty() == TRUE)
				{
					m_WaferMapWrapper.GetWaferID(szWaferId);
					INT nIndex = szWaferId.ReverseFind('.');
					if ( nIndex != -1 )
					{
						szWaferId = szWaferId.Left(nIndex);
					}
				}
				pFileLog.WriteString("Begin "+szWaferId+"\n");
			}

			strTemp.Format("%3d,%3d,%3d,%3d\n", lUserCol, lUserRow, lCol, lRow);
			pFileLog.WriteString(strTemp);
			pFileLog.Close();

			if ( CopyFile(strLocalFiducialsLog, strFiducialsLog, FALSE) == FALSE)
			{
				SaveScanTimeEvent("    WFT: Fail to copy Fiducials Log file to the network path! Please Check!");
			}
		}

		strTemp.Format("    WFT: Fail to re-verify refer die (%3d, %3d)!\nContinue or Stop?", lUserCol, lUserRow);
		SaveScanTimeEvent(strTemp);
		SetJoystickOn(TRUE);
		ChangeCameraToWaferBondMode(FALSE);
		LONG lTmp = 0;
		lTmp = HmiMessageEx_Timeout_Red_Back(strTemp, "Re-verify Ref Die Warning", glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300, NULL, NULL, NULL, NULL);
		SetJoystickOn(FALSE);
		ChangeCameraToWaferBondMode(TRUE);
		ChangeCameraToWafer();

		if ( lTmp == glHMI_CONTINUE)
		{
			SetErrorMessage("Prescan Re-verify Fiducials fails but User chooses CONTINUE!");
		}
		else if ( lTmp == glHMI_STOP)
		{
			SetErrorMessage("Prescan Re-verify Fiducials fails and User chooses STOP!");
			return FALSE;
		}
	}

	return TRUE;
}

VOID CWaferTable::DisplayDieState(ULONG ulRow, ULONG ulCol)
{
	if (IsMapNullBin(ulRow, ulCol))
	{
		return ;
	}

	switch( GetMapDieState(ulRow, ulCol) )
	{
	case WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY:
	case WT_MAP_DS_UNPICK_REGRAB_EMPTY:
		m_szDieState = "E";
		break;
	case WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT:
		m_szDieState = "D";
		break;
	case WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT:
		m_szDieState = "B";
		break;
	default:
		m_szDieState = " ";
		break;
	}

	CString szDieOcrValue = "";
	if( (m_bAoiOcrLoaded) || (IsOcrAOIMode() && IsCharDieInUse()) )
	{
		if( m_bAoiOcrLoaded )
			szDieOcrValue = "****";
		LONG lUserRow = 0, lUserCol = 0;
		ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
		CString szTgtOcrPath	= m_szPrescanLogPath + _T(".OCR");	//	display die OCR value when click on map
		CStdioFile fpOcr;
		if( fpOcr.Open(szTgtOcrPath, CFile::modeRead|CFile::shareDenyNone) )
		{
			CString szReading, szPoint;
			szPoint.Format("%d,%d,", lUserRow, lUserCol);
			CStringArray szaDataList;
			fpOcr.SeekToBegin();
			while( fpOcr.ReadString(szReading)!=NULL )
			{
				if( szReading.Find(szPoint)!=-1 )
				{
					szaDataList.RemoveAll();
					CUtility::Instance()->ParseRawData(szReading, szaDataList);
					if( szaDataList.GetSize()>=3 )
					{
						szDieOcrValue = szaDataList.GetAt(2);
						break;
					}
				}
			}
			fpOcr.Close();
		}
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="emcore" && m_WaferMapWrapper.IsMapValid() )	//	emcore laser bar
	{
		CStdioFile oFile;
		if( oFile.Open(GetMapFileName(), CFile::modeRead|CFile::shareDenyNone) )
		{	// Get the sample electricial data
			CString szLineData = "", szOcrValue = "";
			CStringArray szLineList;
			m_WaferMapWrapper.GetExtraInformation(&oFile, ulRow, ulCol, szLineData);
			szLineList.RemoveAll();
			CUtility::Instance()->ParseRawData(szLineData, szLineList);
			if( szLineList.GetSize()>=1 )
				szOcrValue = szLineList.GetAt(0);
			oFile.Close();
			m_szNextLocation = szOcrValue;
			m_szDieState = m_szDieState + " " + szOcrValue;
		}
	}	//	emcore laser bar

	if( szDieOcrValue!="" )
	{
		if( m_bAoiOcrLoaded )
		{
			m_szNextLocation = szDieOcrValue;
		}

		if( (IsOcrAOIMode() && IsCharDieInUse()) )
		{
			m_szDieState = m_szDieState + " " + szDieOcrValue;
		}
	}
}

VOID CWaferTable::UpdateDieStateSummary()
{
	ULONG ulEmpty = 0, ulDefect = 0, ulBadCut = 0;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanRegionMode() )
	{
	//	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulGoodIdx;
	//	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA]	= ulExtraIdx;
		ulDefect = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT];
		ulBadCut = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT];
		ulEmpty  = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY];
	}
	else
	{
		ULONG ulMaxRow=0, ulMaxCol=0;
		m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);

		for(ULONG ulRow=0; ulRow<=ulMaxRow; ulRow++)
		{
			for(ULONG ulCol=0; ulCol<=ulMaxCol; ulCol++)
			{
				switch( GetMapDieState(ulRow, ulCol) )
				{
				case WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY:
				case WT_MAP_DS_UNPICK_REGRAB_EMPTY:
					ulEmpty++;
					break;
				case WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT:
					ulDefect++;
					break;
				case WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT:
					ulBadCut++;
					break;
				default:
					break;
				}
			}
		}
	}

	FILE *fp;
	errno_t nErr = fopen_s(&fp, "c:\\mapsorter\\exe\\DieStateList.csv", "w");
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "Empty  %lu,", ulEmpty);
		fprintf(fp, "Defect %lu,", ulDefect);
		fprintf(fp, "Badcut %lu,", ulBadCut);
		fclose(fp);
	}
}


BOOL CWaferTable::PrescanRealignWafer()
{
	ULONG		ulAlignRow=0, ulAlignCol=0;
	LONG		lEncX = 0, lEncY = 0, lRow=0, lCol=0;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	pUtl->GetAlignPosition(lRow, lCol, lEncX, lEncY);
	ulAlignRow = lRow;
	ulAlignCol = lCol;

	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
	ULONG ulAlnRegion = pSRInfo->GetAssistRegion();
	if( pUtl->GetPrescanRegionMode() && ulTgtRegion!=ulAlnRegion )
	{
		if( GetMapPhyPosn(lRow, lCol, lEncX, lEncY)==FALSE )
		{
			return FALSE;
		}
	}

	SaveScanTimeEvent("    WFT: to realign wafer");
	// just switch to wafer side and grab, but no result check
	if( IsES101()==FALSE && IsES201()==FALSE )	//	4.24TX 4	//v4.37T10
	{
		CheckDieMapWaferPosition(ulAlignRow, ulAlignCol, lEncX, lEncY);
	}

	//Get encoder start position
	if( IsESMachine() )	//	just use align position to avoid move
	{
		m_lStart_X		= lEncX;
		m_lStart_Y		= lEncY;
		m_lStart_T		= GetGlobalT();
	}
	else
	{
		GetEncoderValue();
		m_lStart_X		= GetCurrX();
		m_lStart_Y		= GetCurrY();
		m_lStart_T		= GetCurrT();
	}

	SetAlignmentStatus(TRUE);

	//Get current & set selected position
	m_WaferMapWrapper.SetCurrentPosition(ulAlignRow, ulAlignCol);
//	Sleep(50);
	m_WaferMapWrapper.SetSelectedPosition(ulAlignRow, ulAlignCol);

	// Need to get map selected position here for correct index display in below
	m_WaferMapWrapper.GetSelectedPosition(ulAlignRow, ulAlignCol);
	m_ulAlignRow = ulAlignRow;
	m_ulAlignCol = ulAlignCol;

	SetCtmRefDieOptions();			//v3.03

	//Re-calculate map 
	m_WaferMapWrapper.GetSelectedPosition(ulAlignRow, ulAlignCol);
	if( m_lBuildShortPathInScanning==0 )
	{
		m_WaferMapWrapper.EnableAutoAlign(!m_bStartPoint);
		m_WaferMapWrapper.ResetGrade();
		m_WaferMapWrapper.SetStartPosition(ulAlignRow, ulAlignCol);
	}

	//Refresh wafermap
	m_WaferMapWrapper.Redraw();

	return TRUE;
}

LONG CWaferTable::GrabAndSaveImage(CONST BOOL bDieType, CONST LONG lDiePrID, LONG lCheckType)
{
	typedef struct
	{
		BOOL bDieType;
		LONG lDiePrID;
		LONG lCheckType;
	}	GRABPR;
	GRABPR stData;

	if (m_bEnableAlignWaferImageLog != TRUE)
	{	
		return TRUE;
	}

//	Sleep(1000);	//Sleep(2000);	//v4.02T4

	stData.bDieType = bDieType;
	stData.lDiePrID = lDiePrID;
	stData.lCheckType = lCheckType;

	IPC_CServiceMessage stMsg;
	int		nConvID = 0;
	BOOL	bReturn = FALSE;

	stMsg.InitMessage(sizeof(GRABPR), &stData);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "GrabAndSaveImageCmd", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE )
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

	return bReturn;
}

BOOL CWaferTable::BlkFunc2ReferDieCheck(CONST ULONG ulY, CONST ULONG ulX)
{
	(*m_psmfSRam)["DieInfo"]["RefDie"]["Valid"]	= 1;

	if( IsBlkFunc2Enable() || IsBlkPickAlign() )	//Block2
	{
		INT iDig1 = -1, iDig2 = -1;
		BOOL bRtn = m_pBlkFunc2->Blk2IsReferDie(ulY, ulX, &iDig1, &iDig2);
		if( bRtn==TRUE )
		{
			(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"] = iDig1;
			(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"] = iDig2;
			(*m_psmfSRam)["DieInfo"]["RefDie"]["ulX"] = (LONG)ulX;
			(*m_psmfSRam)["DieInfo"]["RefDie"]["ulY"] = (LONG)ulY;
			return TRUE;
		}
		else
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["Valid"]	= 0;
		}
	}

	return FALSE;
}

VOID CWaferTable::SetScanDieNum(ULONG ulGoodNum, ULONG ulDefectNum, ULONG ulBadCutNum, ULONG ulExtraNum, ULONG ulEmptyNum)
{
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulGoodNum;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulDefectNum;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulBadCutNum;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA]	= ulExtraNum;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulEmptyNum;
}

BOOL CWaferTable::IsToAutoSearchWaferLimit()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsPrescanSizeSet() )	// most top option, auto disable area define and auto search
	{
		return FALSE;
	}

	if( IsPrescanAreaDefined() )	// dummy scan if scan area defined, auto disable auto search wafer limit
	{
		return FALSE;
	}

	if( IsScanWithWaferLimit() )
	{
		return FALSE;
	}

	if( (pUtl->GetPrescanDummyMap() || IsScanAlignWafer()) )
	{
		return m_bAutoSearchWaferLimit;
	}

	return FALSE;
}


BOOL CWaferTable::IsAutoDetectEdge()
{
	return m_bRunTimeDetectEdge;
}

BOOL CWaferTable::IsPrescanSizeSet()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	return (pUtl->GetPrescanDummyMap() || IsScanAlignWafer()) && m_dPrescanAreaDiameter!=0;
}

BOOL CWaferTable::IsPrescanAreaDefined()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	BOOL bBarScanLimit = FALSE;
	if( pUtl->GetPrescanBarWafer() )
	{
		bBarScanLimit = TRUE;
	}

	if( pUtl->GetPrescanDummyMap()==FALSE && IsScanAlignWafer()==FALSE && bBarScanLimit==FALSE )
	{
		return FALSE;
	}

	if( IsPrescanSizeSet() )
	{
		return FALSE;
	}

	if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
	{
		if( (m_lScanAreaUpLeftX==m_lScanAreaLowRightX) && (m_lScanAreaUpLeftY==m_lScanAreaLowRightY) )
		{
			return FALSE;
		}
	}
	else
	{
		if( (m_lScanAreaUpLeftX==m_lScanAreaLowRightX) || (m_lScanAreaUpLeftY==m_lScanAreaLowRightY) )
		{
			return FALSE;
		}
	}

	return m_bScanAreaConfig;
}

VOID CWaferTable::AutoSearchWaferLimit()
{
	m_bAutoWaferLimitOk = FALSE;

	if( IsToAutoSearchWaferLimit()==FALSE )
	{
		return ;
	}

	SaveScanTimeEvent("    WFT: Auto Search Wafer Limit start!");
	MultiSrchInitNmlDie1(FALSE);
	if( m_ucPrescanMapShapeType != WT_MAP_SHAPE_CIRCLE )
	{
		AutoSearchRectWaferEdge();
		return ;
	}

	LONG lPrDelay = m_lPrescanPrDelay;
	LONG lCtrX = m_lAutoWaferCenterX;
	LONG lCtrY = m_lAutoWaferCenterY;
	LONG lRadius = m_lAutoWaferDiameter/2;
	LONG lPosX, lPosY;
	DOUBLE dAngle;

	BOOL bAutoSrch = TRUE;
	if (IsPrescanEnable())
	{
		if( m_dPrescanLFSizeX<2.0 || m_dPrescanLFSizeY<2.0 )
		{
			bAutoSrch = FALSE;
		}
	}

	ChangeCameraToWafer();

	SetJoystickOn(FALSE);

	m_lWaferTmpX[0] = 0;
	m_lWaferTmpY[0] = 0;
	m_lWaferTmpX[1] = 0;
	m_lWaferTmpY[1] = 0;
	m_lWaferTmpX[2] = 0;
	m_lWaferTmpY[2] = 0;

	// left side edge
	SaveScanTimeEvent("    WFT: ASE move to LFT");
	dAngle = PI*2*WT_SRCH_EDGE_LFT/WT_SRCH_EDGE_MAX;
	lPosX = lCtrX - (LONG)(lRadius*cos(dAngle));
	lPosY = lCtrY + (LONG)(lRadius*sin(dAngle));
	if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
	{
	}
	else
	{
		XY_SafeMoveTo(GetCircleWaferMaxX(), GetWaferCenterY());
	}
	if( lPrDelay>0 )
		Sleep(lPrDelay);
	if( bAutoSrch )
	{
		SeearchWaferEdge(WT_SRCH_EDGE_LFT, m_lWaferTmpX[0], m_lWaferTmpY[0]);	// left
	}
	else
	{
		CString	szAskMsg = "Please Move wafer table to Left!";
		SetJoystickOn(TRUE);
		HmiMessageEx_Red_Back(szAskMsg, "Auto Wafer Edge");
		SetJoystickOn(FALSE);
		GetEncoderValue();
		m_lWaferTmpX[0] = GetCurrX();
		m_lWaferTmpY[0] = GetCurrY();
	}

	// top side edge
	SaveScanTimeEvent("    WFT: ASE move to TOP");
	dAngle = PI*2*WT_SRCH_EDGE_TOP/WT_SRCH_EDGE_MAX;
	lPosX = lCtrX - (LONG)(lRadius*cos(dAngle));
	lPosY = lCtrY + (LONG)(lRadius*sin(dAngle));
	if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
	{
	}
	else
	{
		XY_SafeMoveTo(GetWaferCenterX(), GetCircleWaferMaxY());
	}
	if( lPrDelay>0 )
		Sleep(lPrDelay);

	if( bAutoSrch )
	{
		SeearchWaferEdge(WT_SRCH_EDGE_TOP, m_lWaferTmpX[1], m_lWaferTmpY[1]);	// top
	}
	else
	{
		CString	szAskMsg = "Please Move wafer table to Top!";
		SetJoystickOn(TRUE);
		HmiMessageEx_Red_Back(szAskMsg, "Auto Wafer Edge");
		SetJoystickOn(FALSE);
		GetEncoderValue();
		m_lWaferTmpX[1] = GetCurrX();
		m_lWaferTmpY[1] = GetCurrY();
	}

	// right side edge
	SaveScanTimeEvent("    WFT: ASE move to RGT");
	dAngle = PI*2*WT_SRCH_EDGE_RGT/WT_SRCH_EDGE_MAX;
	lPosX = lCtrX - (LONG)(lRadius*cos(dAngle));
	lPosY = lCtrY + (LONG)(lRadius*sin(dAngle));
	if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
	{
	}
	else
	{
		XY_SafeMoveTo(GetCircleWaferMinX(), GetWaferCenterY());
	}
	if( lPrDelay>0 )
		Sleep(lPrDelay);

	if( bAutoSrch )
	{
		SeearchWaferEdge(WT_SRCH_EDGE_RGT, m_lWaferTmpX[2], m_lWaferTmpY[2]);	// right
	}
	else
	{
		CString	szAskMsg = "Please Move wafer table to Right!";
		SetJoystickOn(TRUE);
		HmiMessageEx_Red_Back(szAskMsg, "Auto Wafer Edge");
		SetJoystickOn(FALSE);
		GetEncoderValue();
		m_lWaferTmpX[2] = GetCurrX();
		m_lWaferTmpY[2] = GetCurrY();
	}

	// calculate the circular center and diameter
	LONG lPrevSize = GetPrescanDiameter();
	if( CalcWaferCircle() )
	{
		if( IsScanAlignWafer()==FALSE && labs(lPrevSize-GetWaferDiameter())>50 &&
			labs(lPrevSize-GetPrescanDiameter())>labs(30*lPrevSize/100) )
		{
			HmiMessage_Red_Back("auto search wafer limit varies over 0.30 than before");
		}
		m_bAutoWaferLimitOk = TRUE;
	}

	// verify the all other 5 corner to make sure cover enough
	if( m_bAutoWaferLimitOk )
	{
		lCtrX = m_lAutoWaferCenterX;
		lCtrY = m_lAutoWaferCenterY;
		for(int i=0; i<=WT_SRCH_EDGE_B_R; i++)
		{
			if( i==WT_SRCH_EDGE_RGT || i==WT_SRCH_EDGE_TOP || i==WT_SRCH_EDGE_LFT )
			{
				continue;
			}
			if( IsESMachine() )	// ES only search the bottom side	//v4.37T10
			{
				if( i!=WT_SRCH_EDGE_BTM )
					continue;
			}
			lRadius = m_lAutoWaferDiameter/2;
			dAngle = PI*2*i/WT_SRCH_EDGE_MAX;
			lPosX = lCtrX - (LONG)(lRadius*cos(dAngle));
			lPosY = lCtrY + (LONG)(lRadius*sin(dAngle));
			if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT)!=TRUE )
			{
				continue;
			}
			if( lPrDelay>0 )
				Sleep(lPrDelay);
			SeearchWaferEdge(i, lPosX, lPosY);
			if( IsWithinPrescanLimit(lPosX, lPosY, WT_SRCH_EDGE_LMT)==FALSE )
			{
				m_lAutoWaferDiameter = (LONG)(GetDistance(lCtrX, lCtrY, lPosX, lPosY)*2);
			}
		}
	}
}

LONG CWaferTable::GetScanCtrX()
{
	return m_nPrescanMapCtrX;
}

LONG CWaferTable::GetScanCtrY()
{
	return m_nPrescanMapCtrY;
}

LONG CWaferTable::GetScanRadius()
{
	return m_nPrescanMapRadius;
}

LONG CWaferTable::GetScanUL_X()
{
	return GetScanCtrX() + m_nPrescanMapWidth/2;
}

LONG CWaferTable::GetScanUL_Y()
{
	return GetScanCtrY() + m_nPrescanMapHeight/2;
}

LONG CWaferTable::GetScanLR_X()
{
	return GetScanCtrX() - m_nPrescanMapWidth/2;
}

LONG CWaferTable::GetScanLR_Y()
{
	return GetScanCtrY() - m_nPrescanMapHeight/2;
}


LONG CWaferTable::GetPrescanCenterX()
{
	if( IsPrescanAreaDefined() )
	{
		if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
			return m_lScanAreaUpLeftX;
		else
			return (m_lScanAreaUpLeftX+m_lScanAreaLowRightX)/2;
	}

	if( IsToAutoSearchWaferLimit() && m_bAutoWaferLimitOk )
	{
		return m_lAutoWaferCenterX;
	}

	return GetWaferCenterX();
}

LONG CWaferTable::GetPrescanCenterY()
{
	if( IsPrescanAreaDefined() )
	{
		if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
			return m_lScanAreaUpLeftY;
		else
			return (m_lScanAreaUpLeftY + m_lScanAreaLowRightY)/2;
	}

	if( IsToAutoSearchWaferLimit() && m_bAutoWaferLimitOk )
	{
		return m_lAutoWaferCenterY;
	}

	return GetWaferCenterY();
}

LONG CWaferTable::GetPrescanDiameter()
{
	if( IsPrescanSizeSet() )
	{
		return (LONG)(m_dPrescanAreaDiameter*25.4/m_dXYRes);
	}

	if( IsPrescanAreaDefined() )
	{
		DOUBLE lX = labs(m_lScanAreaLowRightX - m_lScanAreaUpLeftX);
		DOUBLE lY = labs(m_lScanAreaLowRightY - m_lScanAreaUpLeftY);
		//	return max(lX, lY);
		if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
			return (LONG)sqrt(pow(lX, 2.0) + pow(lY, 2.0))*2;
		else
			return (LONG)sqrt(pow(lX, 2.0) + pow(lY, 2.0));
	}

	if( IsToAutoSearchWaferLimit() && m_bAutoWaferLimitOk )
	{
		return m_lAutoWaferDiameter;
	}

	return GetWaferDiameter();
}

BOOL CWaferTable::IsWithinPrescanLimit(LONG lX, LONG lY, DOUBLE dScale)
{
	if( IsToAutoSearchWaferLimit() && m_bAutoWaferLimitOk && 
		GetPrescanDiameter()>0 && (m_ucWaferLimitType == WT_CIRCLE_LIMIT) )
	{
		LONG lWaferRadius = GetPrescanDiameter() / 2;
		DOUBLE	dDistance = 0.0, dX, dY;
		dX = ((double)(lX - GetPrescanCenterX())) * ((double)(lX - GetPrescanCenterX()));
		dY = ((double)(lY - GetPrescanCenterY())) * ((double)(lY - GetPrescanCenterY()));
		dDistance = sqrt( dX + dY );

		if ( dDistance <= ((DOUBLE)(lWaferRadius) * dScale) )
			return TRUE;
		else
			return FALSE;
	}

	return IsWithinWaferLimit(lX, lY, dScale) && IsWithinMapDieLimit(lX, lY, dScale);
}

BOOL CWaferTable::IsWithinLimit(LONG lX, LONG lY, LONG lCtrX, LONG lCtrY, LONG lDiameter, BOOL bAllType)
{
	if( bAllType && m_ucPrescanMapShapeType!=WT_MAP_SHAPE_CIRCLE )
	{
		if( IsWithinWaferLimit(lX, lY)==FALSE )
		{
			return FALSE;
		}

		LONG lLftX = lCtrX + lDiameter/2;
		LONG lRgtX = lCtrX - lDiameter/2;
		LONG lTopY = lCtrY + lDiameter/2;
		LONG lBtmY = lCtrY - lDiameter/2;
		if( lX<=lLftX && lX>=lRgtX && lY<=lTopY && lY>=lBtmY )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	DOUBLE dWaferRadius = (DOUBLE)(lDiameter / 2.0);
	DOUBLE	dDistance = 0.0, dX, dY;
	dX = pow((lX - lCtrX), 2.0);
	dY = pow((lY - lCtrY), 2.0);
	dDistance = sqrt( dX + dY );

	if ( dDistance <= dWaferRadius )
		return TRUE;
	else
		return FALSE;
}

BOOL CWaferTable::SeearchWaferEdge(SHORT sEdge, LONG &lEdgeX, LONG &lEdgeY)
{
	LONG	lPosX, lPosY, lDir;
	SHORT sState;
	BOOL	bHasNoDie;
	MULTI_SRCH_RESULT	stMsch;
	IPC_CServiceMessage stMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	DOUBLE dAngle = PI*2*sEdge/WT_SRCH_EDGE_MAX;
	sState = 0;
	BOOL bDrawDie = FALSE;
	SHORT sSrchLimit = 0;
	LONG lPrDelay = m_lPrescanPrDelay;
	while( 1 )
	{
		GetEncoderValue();
		lPosX = GetCurrX();
		lPosY = GetCurrY();
		(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lPosX;
		(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lPosY;
		stMsg.InitMessage(sizeof(BOOL), &bDrawDie);
		int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MultiSearchNmlDie1", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		stMsg.GetMsg(sizeof(MULTI_SRCH_RESULT), &stMsch);
		bHasNoDie = stMsch.bDieState[0]==0 && stMsch.bDieState[1]==0 &&
					stMsch.bDieState[2]==0 && stMsch.bDieState[3]==0;
		if( bHasNoDie )
		{
			lDir = -1;
			lEdgeX = lPosX;
			lEdgeY = lPosY;
			if( sState==0 )
			{
				sState = 11;
			}
			if( sState==22 )
			{
				return TRUE;
			}
		}
		else
		{
			lDir = 1;
			if( sState==0 )
			{
				lEdgeX = lPosX;
				lEdgeY = lPosY;
				sState = 22;
			}
			if( sState==11 )
			{
				return TRUE;
			}
		}

		lPosX = lPosX - (LONG)(GetPrescanPitchX()*cos(dAngle)*lDir);
		lPosY = lPosY + (LONG)(GetPrescanPitchY()*sin(dAngle)*lDir);
		if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT)!=TRUE )
		{
			break;
		}
		if( lPrDelay>0 )
			Sleep(lPrDelay);

		sSrchLimit++;
		if( sSrchLimit>100 )
		{
			pUtl->PrescanMoveLog("Search Wafer Edge try over 100");
			return FALSE;
		}
	}

	return TRUE;
}

//	Given three points, you can find the center of a circle that passes through all three.
//	This is a common problem that comes up in geometry and technical drawing classes.
//
//	Instructions
//
//	1
//	Assign each point (x,y) a number, 1 to 3, to keep track of which point is which.
//	Subsequent notation here will designate point 1, for example, as x1,y1.
//
//	2
//	Find the slope m of two lines, A and B, passing through two pairs of the three points.
//	Without loss of generality, the relevant formulas are m(A) = (y2-y1)/(x2-x1) and m(B) = (y3-y2)/(x3-x2).
//
//	3
//	Calculate the x-coordinate of the circle's center by calculating the formula
//	x = [m(A)m(B)(y1-y3) + m(B)(x1+x2) - m(A)(x2+x3)]/[2(m(B)-m(A))].
//
//	4
//	Solve for the y-coordinate of the center by plugging the center's x-coordinate into the formula
//	y = [(x1+x2)/2 - x]/m(A) + (y1+y2)/2.

#define	WT_DOBLE_MIN	0.0000001

BOOL CWaferTable::CalcWaferCircle()
{
	DOUBLE pt1x = m_lWaferTmpX[0];
	DOUBLE pt1y = m_lWaferTmpY[0];
	DOUBLE pt2x = m_lWaferTmpX[1];
	DOUBLE pt2y = m_lWaferTmpY[1];
	DOUBLE pt3x = m_lWaferTmpX[2];
	DOUBLE pt3y = m_lWaferTmpY[2];

	if( CalculateCircle(pt1x, pt1y, pt2x, pt2y, pt3x, pt3y) )
		return TRUE;

	if( CalculateCircle(pt1x, pt1y, pt3x, pt3y, pt2x, pt2y) )
		return TRUE;

	if( CalculateCircle(pt2x, pt2y, pt1x, pt1y, pt3x, pt3y) )
		return TRUE;

	if( CalculateCircle(pt2x, pt2y, pt3x, pt3y, pt1x, pt1y) )
		return TRUE;

	if( CalculateCircle(pt3x, pt3y, pt2x, pt2y, pt1x, pt1y) )
		return TRUE;

	if( CalculateCircle(pt3x, pt3y, pt1x, pt1y, pt2x, pt2y) )
		return TRUE;

	HmiMessage("The three points are perpendicular to axis");
	return FALSE;
}

BOOL CWaferTable::CalculateCircle(DOUBLE pt1x, DOUBLE pt1y, DOUBLE pt2x, DOUBLE pt2y, DOUBLE pt3x, DOUBLE pt3y) 
{ 
	DOUBLE yDelta_21 = pt2y - pt1y; 
	DOUBLE xDelta_21 = pt2x - pt1x; 
	DOUBLE yDelta_32 = pt3y - pt2y; 
	DOUBLE xDelta_32 = pt3x - pt2x; 
	LONG m_x, m_y, dR1, dR2, dR3;
	CString szMsg;

	// Check the given point are perpendicular to x or y axis 
	// checking whether the line of the two pts are vertical
	if( fabs(xDelta_21)<=WT_DOBLE_MIN && fabs(yDelta_32)<=WT_DOBLE_MIN )
	{
		//  11
		//
		//
		//  22    33
		HmiMessage("The points are pependicular and parallel to x-y axis");
		m_x= (LONG) (pt2x + pt3x)/2;
		m_y= (LONG) (pt1y + pt2y)/2;
		dR1 = (LONG) GetDistance(m_x, m_y, pt1x, pt1y);
		dR2 = (LONG) GetDistance(m_x, m_y, pt2x, pt2y);
		dR3 = (LONG) GetDistance(m_x, m_y, pt3x, pt3y);
		szMsg.Format("WT - ADE Original Center %ld,%ld, radius %ld", GetWaferCenterX(), GetWaferCenterY(), GetWaferDiameter()/2);
		SaveScanTimeEvent(szMsg);
		m_lAutoWaferCenterX = (LONG)m_x;
		m_lAutoWaferCenterY = (LONG)m_y;
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		UINT unWafEdgeNum = pApp->GetPrescanWaferEdgeNum();
		m_lAutoWaferDiameter = (LONG)dR1*2 + 2*unWafEdgeNum*GetPrescanPitchX();
		szMsg.Format("WT - ADE Center: (%ld,%ld) Radius: %ld %ld %ld", m_x, m_y, dR1, dR2, dR3);
		SaveScanTimeEvent(szMsg);
		szMsg.Format("WT - ADE Center (%d,%d), diameter %d", m_lAutoWaferCenterX, m_lAutoWaferCenterY, m_lAutoWaferDiameter);
		SaveScanTimeEvent(szMsg);
		return TRUE;
	}
	else
	{
		if (fabs(yDelta_21) <= WT_DOBLE_MIN || 
			fabs(yDelta_32) <= WT_DOBLE_MIN ||
			fabs(xDelta_21) <= WT_DOBLE_MIN ||
			fabs(xDelta_32) <= WT_DOBLE_MIN )
		{
			HmiMessage(" A line of two point are perpendicular to x-axis or y-axis");
			return FALSE;
		}
	}

	//  assure that xDelta(s) are not zero
	DOUBLE dMa = yDelta_21/xDelta_21;
	DOUBLE dMb = yDelta_32/xDelta_32;
	if (fabs(dMa-dMb) <= WT_DOBLE_MIN)
	{
		// checking whether the given points are colinear. 
		HmiMessage("The three pts are colinear\n");
		return FALSE;
	}
	// calc center 
	m_x = (LONG)( (dMa*dMb*(pt1y - pt3y) + dMb*(pt1x + pt2x) - dMa*(pt2x+pt3x))/ (2*(dMb-dMa)));
	m_y = (LONG)( (pt1y+pt2y)/2 - (m_x - (pt1x+pt2x)/2)/dMa);

	LONG lOldCX = GetWaferCenterX();
	LONG lOldCY = GetWaferCenterY();
	LONG lOldRs = GetWaferDiameter()/2;
	szMsg.Format("WT - ADE Original Center %ld,%ld, radius %ld, range (%d,%d), (%d,%d)", lOldCX, lOldCY, lOldRs,
		lOldCX + lOldRs, lOldCY+lOldRs, lOldCX-lOldRs, lOldCY-lOldRs);
	SaveScanTimeEvent(szMsg);
	dR1 = (LONG) GetDistance(m_x, m_y, pt1x, pt1y);
	dR2 = (LONG) GetDistance(m_x, m_y, pt2x, pt2y);
	dR3 = (LONG) GetDistance(m_x, m_y, pt3x, pt3y);
	szMsg.Format("WT - ADE three points (%f,%f) (%f,%f) (%f,%f)", pt1x, pt1y, pt2x, pt2y, pt3x, pt3y); 
	SaveScanTimeEvent(szMsg);
	szMsg.Format("WT - ADE Center: (%ld,%ld) Radius: %ld %ld %ld", m_x, m_y, dR1, dR2, dR3);
	SaveScanTimeEvent(szMsg);

	LONG lLongDist = max(dR1, dR2);
	lLongDist = max(lLongDist, dR3);
	m_lAutoWaferCenterX		= (LONG)m_x;
	m_lAutoWaferCenterY		= (LONG)m_y;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	UINT unWafEdgeNum = pApp->GetPrescanWaferEdgeNum();
	m_lAutoWaferDiameter	= lLongDist*2 + 2*unWafEdgeNum*GetPrescanPitchX();
	szMsg.Format("WT - ADE Center (%d,%d), diameter %d", m_lAutoWaferCenterX, m_lAutoWaferCenterY, m_lAutoWaferDiameter);
	SaveScanTimeEvent(szMsg);

	return TRUE;
} 

LONG CWaferTable::CheckScnPrescanOption(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn;

	if( m_bEnableSCNFile || IsPrescanEnable() )
	{
		bReturn = TRUE;
	}
	else
	{
		HmiMessage("This Settings for SCN file or prescan function only!", "Warning");
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferTable::WftMoveSearchReferDie(LONG &lCurX, LONG &lCurY, BOOL bDoComp, UCHAR ucReferID)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	REF_TYPE	stInfo;

	SRCH_TYPE	stSrchInfo;

	XY_SafeMoveTo(lCurX, lCurY);

	ChangeCameraToWafer();
	Sleep(120);
	//Init Message
	stSrchInfo.bNormalDie	= FALSE;
	stSrchInfo.lRefDieNo	= ucReferID;
	stSrchInfo.bShowPRStatus = TRUE;
	stSrchInfo.bDisableBackupAlign = FALSE;

	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsPLLMRebel() )
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie_PLLM_REBEL", stMsg);
	else
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (stInfo.bStatus == FALSE)
	{
		SetCtmLogMessage("info status is false");
		return FALSE;
	}
	else
	{
		if (stInfo.bFullDie == TRUE)
		{
			lCurX += stInfo.lX;	
			lCurY += stInfo.lY;
			if( bDoComp==TRUE )
			{
				//Move table to updated position
				if (XY_SafeMoveTo(lCurX, lCurY))
				{
					Sleep(50);
				}
			}
			if (stInfo.bGoodDie == FALSE)
			{
				SetCtmLogMessage("info is full die and good die");
				return TRUE;	//Defect die found
			}
		}
		else
		{
			SetCtmLogMessage("info is not a full die");
			return FALSE;		//Empty die found
		}
	}

	return TRUE;	//Good die found
}

BOOL CWaferTable::CheckSubMapWaferMatch(LONG lInRow, LONG lInCol, CString szState, BOOL bCtrBlock, BOOL &bCtrEnough)
{
	int i, j;
	ULONG ulNumRow = 0, ulNumCol = 0;
	m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	LONG lWorkRow, lWorkCol;
	BOOL bSubMapOK = TRUE;

	BOOL bFindOK = FALSE;
	LONG lGetX=0, lGetY=0;
	LONG lMapX = 0, lMapY = 0;
	INT nDieStatus = 0;
	LONG lScanX=0, lScanY=0;	//Klocwork	//v4.04

	for(i = 4; i<5; i++)
	{
		lWorkRow = lInRow;
		lWorkCol = lInCol;
		if( lWorkRow<0 || lWorkRow>(LONG)ulNumRow )
			continue;
		if( lWorkCol<0 || lWorkCol>(LONG)ulNumCol )
			continue;
		if( GetMapPhyPosn(lWorkRow, lWorkCol, lMapX, lMapY) )
		{
			if( DEB_IsUseable() )	//v4.02T4	//Disable DEB option to prevent double access
			{
				bFindOK = DEB_GetDiePos(lWorkRow, lWorkCol, lGetX, lGetY, nDieStatus);
			}
		}
	}
	if( bFindOK==FALSE )
	{
		for(i=0; i<9; i++)
		{
			lWorkRow = lInRow - 1 + i/3;
			lWorkCol = lInCol - 1 + i%3;
			if( lWorkRow<0 || lWorkRow>(LONG)ulNumRow )
				continue;
			if( lWorkCol<0 || lWorkCol>(LONG)ulNumCol )
				continue;
			if( GetMapPhyPosn(lWorkRow, lWorkCol, lMapX, lMapY) )
			{
				if( DEB_IsUseable() )	//v4.02T4	//Disable DEB option to prevent double access
				{
					bFindOK = DEB_GetDiePos(lWorkRow, lWorkCol, lGetX, lGetY, nDieStatus);
				}
			}
			if( bFindOK )
			{
				break;
			}
		}
	}

	CString szMsg, szTemp;
	if( bFindOK==FALSE )
	{
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szState + "NO die found");
		return TRUE;
	}

	m_WaferMapWrapper.SetCurrentPosition(lWorkRow, lWorkCol);
	szMsg.Format("Die (%d,%d) Result of posn (%d,%d)", lWorkRow, lWorkCol, lGetX, lGetY);
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

	if( XY_SafeMoveTo(lGetX, lGetY)==FALSE )
	{
		szMsg.Format("Safe move to posn (%d,%d) error", lGetX, lGetY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		SetErrorMessage(szState + szMsg);
		return FALSE;
	}
	Sleep(50);

	WT_LA_DIE_RESULT	stLADieState;
	memset(&stLADieState, 0, sizeof(WT_LA_DIE_RESULT));

	//if( WftMoveSearchDie(lGetX, lGetY, TRUE) )
	//{
	//	nDieStatus = MS_Align;
	//}
	//else
	//{
	//	nDieStatus = MS_Empty;
	//}

	LONG lCtrOffsetX = lGetX - lScanX;
	LONG lCtrOffsetY = lGetY - lScanY;

	if( DEB_IsUseable() )	//v4.02T4	//Disable DEB option to prevent double access
	{
		DEB_UpdateDiePos(lWorkRow, lWorkCol, lGetX, lGetY, nDieStatus);
	}

	(*m_psmfSRam)["MS896A"]["WaferTableX"] = lGetX;
	(*m_psmfSRam)["MS896A"]["WaferTableY"] = lGetY;
	(*m_psmfSRam)["MS896A"]["WaferMapRow"] = lWorkRow;
	(*m_psmfSRam)["MS896A"]["WaferMapCol"] = lWorkCol;
	GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_PCV);	// during pitch verification, pr grab and sceen save and log file

	IPC_CServiceMessage stMsg;
	int nConvID;
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "AutoLookAroundDie_Cmd", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(WT_LA_DIE_RESULT), &stLADieState);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	INT lWidth = (INT)(LONG)((*m_psmfSRam)["WaferPr"]["FOV"]["BaseCol"]);
	INT lHeight = (INT)(LONG)((*m_psmfSRam)["WaferPr"]["FOV"]["BaseRow"]);

	LONG lCurX, lCurY;
	LONG lCRow, lCCol;
	LONG lOffsetX = 0, lOffsetY = 0, lFindCount = 0, lMapDieCount = 0, lMapDieTotal = 0;
	szMsg.Format("Search Matrix Row %d, Col %d; map center (%d,%d)", 
		lWidth*2+1, lHeight*2+1, lWorkRow, lWorkCol);
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

	for(i=0; i<(lHeight*2+1); i++)
	{
		szMsg.Format("%d Map Waf:", i);
		for(j=0; j<(lWidth*2+1); j++)
		{
			lCRow = lWorkRow + (i-lHeight);
			lCCol = lWorkCol + (j-lWidth);

			if( lCRow<0 || lCCol<0 )
			{
				szMsg += " xx";
				continue;
			}
			if( lCRow>(LONG)ulNumRow || lCCol>(LONG)ulNumCol )
			{
				szMsg += " xx";
				continue;
			}

			lMapDieTotal++;
			BOOL bMapDie = FALSE;
			BOOL bFindDie = (stLADieState.m_sDieState[i][j]==2);
			if( GetPrescanWftPosn(lCRow, lCCol, lCurX, lCurY) &&
				GetMapPhyPosn(lCRow, lCCol, lMapX, lMapY) &&
				m_WaferMapWrapper.IsReferenceDie(lCRow, lCCol)==FALSE )
			{
				bMapDie = CheckAlignMapWaferMatchHasDie(lCRow, lCCol);
				if( bMapDie && bMapDie!=bFindDie )
				{
					bSubMapOK = FALSE;
				}
				if( bMapDie )
					lMapDieCount++;

				if( bMapDie && bFindDie && (i!=lHeight) && (j!=lWidth) )
				{
					LONG lPosnX = lGetX + stLADieState.m_sDiePosnX[i][j];
					LONG lPosnY = lGetY + stLADieState.m_sDiePosnY[i][j];
					lOffsetX += (lPosnX - lCurX);
					lOffsetY += (lPosnY - lCurY);
					lFindCount++;
				}
			}
			szTemp.Format(" %d%d", bMapDie, bFindDie);
			szMsg += szTemp;

			if( bSubMapOK==FALSE )
			{
				break;
			}
		}
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		if( bSubMapOK==FALSE )
		{
			break;
		}
	}

	if( bSubMapOK )
	{
		szMsg = "Pitch verify map-wafer OK";
		if( bCtrBlock && lFindCount>=2 )
		{
			m_bGetAvgFOVOffset	= TRUE;
			m_lAvgFovOffsetX	= lOffsetX/lFindCount;
			m_lAvgFovOffsetY	= lOffsetY/lFindCount;
			szMsg.Format("Pitch verify map-wafer OK; main avg offset is %d,%d", m_lAvgFovOffsetX, m_lAvgFovOffsetY);
		}
	}
	else
	{
		szMsg = "Pitch verify map-wafer FAIL";
	}
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szState + szMsg);

	if( lMapDieTotal > lMapDieCount*2 )
	{
		bCtrEnough = TRUE;
	}

	return bSubMapOK;
}

BOOL CWaferTable::CheckMapWaferMatch(LONG lRow, LONG lCol, LONG lOrgX, LONG lOrgY, LONG lTgtX, LONG lTgtY, CONST BOOL bAvgOffset)
{
	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szMsg;

	m_bGetAvgFOVOffset	= FALSE;
	m_lAvgFovOffsetX	= 0;
	m_lAvgFovOffsetY	= 0;

	if( IsLFSizeOK()==FALSE )
	{
		szMsg = "FOV less 3, no Check map wafer match";
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return FALSE;
	}

	if( XY_SafeMoveTo(lTgtX, lTgtY)==FALSE )
	{
		szMsg = "Pitch Verify move to center, motion error";
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return FALSE;
	}
	Sleep(50);

	//
	//	0	1	2
	//	3	4	5
	//	6	7	8
	//

	ChangeCameraToWafer();

	LONG lCkRow, lCkCol;

	BOOL bMapOK = FALSE;
	int i;
	for(i=0; i<5; i++)
	{
		switch( i )
		{
		case 0:
			lCkRow = lRow;
			lCkCol = lCol;
			szMsg = "C Verify ";
			break;
		case 1:
			lCkRow = lRow;
			lCkCol = lCol - (LONG)dFOVX;
			szMsg = "L Verify ";
			break;
		case 2:
			lCkRow = lRow;
			lCkCol = lCol + (LONG)dFOVX;
			szMsg = "R Verify ";
			break;
		case 3:
			lCkRow = lRow - (LONG)dFOVY;
			lCkCol = lCol;
			szMsg = "U Verify ";
			break;
		case 4:
			lCkRow = lRow + (LONG)dFOVY;
			lCkCol = lCol;
			szMsg = "D Verify ";
			break;
		}

		BOOL bGetAvgOffset = FALSE;
		BOOL bCtrEnough = FALSE;
		if( i==0 )
		{
			bGetAvgOffset = TRUE;
		}
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg + "begin");
		bMapOK = CheckSubMapWaferMatch(lCkRow, lCkCol, szMsg, bGetAvgOffset, bCtrEnough);
		if( bMapOK==FALSE )
		{
			break;
		}

		if( bCtrEnough && i==0 )
		{
			break;
		}
	}

	if( XY_SafeMoveTo(lOrgX, lOrgY)==FALSE )
	{
		szMsg = "Pitch Verify move to original motion error";
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return FALSE;
	}
	Sleep(50);

	WftCheckCurrentGoodDie();

	return bMapOK;
}

BOOL CWaferTable::HasMapValidDieAround(ULONG ulInRow, ULONG ulInCol, ULONG ulLoop)
{
	ULONG ulCurrentLoop = 1;
	BOOL	bFindOneDie = FALSE;
	LONG	lTmpRow, lTmpCol;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	lTmpRow = ulInRow;
	lTmpCol = ulInCol;
	if (m_pWaferMapManager->IsMapHaveBin(lTmpRow, lTmpCol))
	{
		return FALSE;
	}

	for(ulCurrentLoop=1; ulCurrentLoop<=ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop*2 + 1);
		lCol = (ulCurrentLoop*2 + 1);

		lCurrentIndex = 1;

		lDiff_X = 1;
		lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		if( lTmpRow>=0 && lTmpCol>=0 )
		{
			if (m_pWaferMapManager->IsMapHaveBin(lTmpRow, lTmpCol))
			{
				bFindOneDie = TRUE;
				break;
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if (m_pWaferMapManager->IsMapHaveBin(lTmpRow, lTmpCol))
				{
					bFindOneDie = TRUE;
					break;
				}
			}
		}
		if( bFindOneDie )
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if (m_pWaferMapManager->IsMapHaveBin(lTmpRow, lTmpCol))
				{
					bFindOneDie = TRUE;
					break;
				}
			}
		}
		if( bFindOneDie )
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if (m_pWaferMapManager->IsMapHaveBin(lTmpRow, lTmpCol))
				{
					bFindOneDie = TRUE;
					break;
				}
			}
		}
		if( bFindOneDie )
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if (m_pWaferMapManager->IsMapHaveBin(lTmpRow, lTmpCol))
				{
					bFindOneDie = TRUE;
					break;
				}
			}
		}
		if( bFindOneDie )
		{
			break;
		}
	}

	return bFindOneDie;
}

VOID CWaferTable::AutoSearchRectWaferEdge()
{
	LONG lCtrX, lCtrY, lWfWX, lWfHY, lPosX, lPosY;
	LONG lMinX, lMaxX, lMinY, lMaxY;
	LONG lAutoEdgeX[4], lAutoEdgeY[4];
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	//Klocwork
	for (int y=0; y<=3; y++)
	{
		lAutoEdgeX[y] = 0;
		lAutoEdgeY[y] = 0;
	}

	LONG lPrDelay = m_lPrescanPrDelay;
	X_Sync();
	Y_Sync();
	GetEncoderValue();

	lCtrX = GetCurrX();
	lCtrY = GetCurrY();
	lWfWX = m_lAutoWaferWidth;
	lWfHY = m_lAutoWaferHeight;

	ChangeCameraToWafer();

	SetJoystickOn(FALSE);

	// move to previous center of the rectangle
	lPosX = m_lAutoWaferCenterX;
	lPosY = m_lAutoWaferCenterY;
	if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
	{
		Sleep(50);
		if( SearchAndAlignDie(TRUE, FALSE, TRUE) )
		{
			X_Sync();
			Y_Sync();
			Sleep(50);
			GetEncoderValue();

			lCtrX = GetCurrX();
			lCtrY = GetCurrY();
			lWfWX = m_lAutoWaferWidth/2;
			lWfHY = m_lAutoWaferHeight/2;
			pUtl->PrescanMoveLog("Rect Wafer Edge use wafer center begin");
		}
		else
		{
			XY_SafeMoveTo(lCtrX, lCtrY);
			pUtl->PrescanMoveLog("Rect Wafer Edge use align position begin");
		}
	}

	BOOL bAutoSrch = TRUE;
	if (IsPrescanEnable())
	{
		if( m_dPrescanLFSizeX<2.0 || m_dPrescanLFSizeY<2.0 )
		{
			bAutoSrch = FALSE;
		}
	}
	int i;
	for(i=0; i<4; i++)
	{
		if( m_bAutoWaferLimitOnce==FALSE )
		{
			XY_SafeMoveTo(lCtrX, lCtrY);
			if( lPrDelay>0 )
				Sleep(lPrDelay);
		}
		switch( i )
		{
		case 0:	// right
			lPosX = lCtrX - lWfWX;
			lPosY = lCtrY + 0;
			pUtl->PrescanMoveLog("Rect Wafer Edge move to right");
			break;
		case 1:	// top
			lPosX = lCtrX + 0;
			lPosY = lCtrY + lWfHY;
			pUtl->PrescanMoveLog("Rect Wafer Edge move to top");
			break;
		case 2:	// left
			lPosX = lCtrX + lWfWX;
			lPosY = lCtrY + 0;
			pUtl->PrescanMoveLog("Rect Wafer Edge move to left");
			break;
		case 3:	// bottom
			lPosX = lCtrX - 0;
			lPosY = lCtrY - lWfHY;
			pUtl->PrescanMoveLog("Rect Wafer Edge move to bottom");
			break;
		}
		if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
		{
		}
		else
		{
			if( XY_SafeMoveTo(lCtrX, lCtrY) )
			{
				pUtl->PrescanMoveLog("Rect Wafer Edge move to center as edge not reachable");
			}
			else
			{
				continue;
			}
		}
		if( lPrDelay>0 )
			Sleep(lPrDelay);
		if(      bAutoSrch )
		{
			SeearchWaferEdge(i*2, lAutoEdgeX[i], lAutoEdgeY[i]);
		}
		else
		{
			CString szAskMsg = "Top";
			switch( i*2 )
			{
			case WT_SRCH_EDGE_RGT:
				szAskMsg = "Right";
				break;
			case WT_SRCH_EDGE_T_R:
				szAskMsg = "Top Right";
				break;
			case WT_SRCH_EDGE_TOP:
				szAskMsg = "Top";
				break;
			case WT_SRCH_EDGE_T_L:
				szAskMsg = "Top Left";
				break;
			case WT_SRCH_EDGE_LFT:
				szAskMsg = "Left";
				break;
			case WT_SRCH_EDGE_B_L:
				szAskMsg = "Bottom Left";
				break;
			case WT_SRCH_EDGE_BTM:
				szAskMsg = "Bottom";
				break;
			}
			szAskMsg = "Please Move wafer table to " + szAskMsg + "!";
			SetJoystickOn(TRUE);
			HmiMessageEx_Red_Back(szAskMsg, "Auto Wafer Edge");
			SetJoystickOn(FALSE);
			GetEncoderValue();
			lAutoEdgeX[i] = GetCurrX();
			lAutoEdgeY[i] = GetCurrY();
		}
	}
	// calculate the rectangle center and edges before verify
	lMinX = lMaxX = lAutoEdgeX[0];
	lMinY = lMaxY = lAutoEdgeY[0];
	for(i=0; i<4; i++)
	{
		if( lMinX>lAutoEdgeX[i] )
			lMinX = lAutoEdgeX[i];
		if( lMaxX<lAutoEdgeX[i] )
			lMaxX = lAutoEdgeX[i];
		if( lMinY>lAutoEdgeY[i] )
			lMinY = lAutoEdgeY[i];
		if( lMaxY<lAutoEdgeY[i] )
			lMaxY = lAutoEdgeY[i];
	}
	lWfWX = labs(lMaxX-lMinX)/2;
	lWfHY = labs(lMaxY-lMinY)/2;
	lCtrX = (lMaxX+lMinX)/2;
	lCtrY = (lMaxY+lMinY)/2;

	// verify the all other 4 corners to make sure cover enough
	for(i=0; i<4; i++)
	{
		switch( i )
		{
		case 0:		// right top
			lPosX = lCtrX - lWfWX;
			lPosY = lCtrY + lWfHY;
			pUtl->PrescanMoveLog("Rect Wafer Edge verify right top");
			break;
		case 1:		// left top
			lPosX = lCtrX + lWfWX;
			lPosY = lCtrY + lWfHY;
			pUtl->PrescanMoveLog("Rect Wafer Edge verify left top");
			break;
		case 2:		// left bottom
			lPosX = lCtrX + lWfWX;
			lPosY = lCtrY - lWfHY;
			pUtl->PrescanMoveLog("Rect Wafer Edge verify left bottom");
			break;
		case 3:		// right bottom
			lPosX = lCtrX - lWfWX;
			lPosY = lCtrY - lWfHY;
			pUtl->PrescanMoveLog("Rect Wafer Edge verify right bottom");
			break;
		}

		if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
		{
			if( lPrDelay>0 )
				Sleep(lPrDelay);
			SeearchWaferEdge(i*2+1, lAutoEdgeX[i], lAutoEdgeY[i]);
		}
	}
	// compare based verify corner and edge search to find most far points
	for(i=0; i<4; i++)
	{
		if( lMinX>lAutoEdgeX[i] )
			lMinX = lAutoEdgeX[i];
		if( lMaxX<lAutoEdgeX[i] )
			lMaxX = lAutoEdgeX[i];
		if( lMinY>lAutoEdgeY[i] )
			lMinY = lAutoEdgeY[i];
		if( lMaxY<lAutoEdgeY[i] )
			lMaxY = lAutoEdgeY[i];
	}
	lWfWX = labs(lMaxX-lMinX)/2;
	lWfHY = labs(lMaxY-lMinY)/2;
	lCtrX = (lMaxX+lMinX)/2;
	lCtrY = (lMaxY+lMinY)/2;

	// verify again to four edges
	for(i=0; i<4; i++)
	{
		switch( i )
		{
		case 0:	// right
			lPosX = lCtrX - lWfWX;
			lPosY = lCtrY + 0;
			pUtl->PrescanMoveLog("Rect Wafer Edge reverify right");
			break;
		case 1:	// top
			lPosX = lCtrX + 0;
			lPosY = lCtrY + lWfHY;
			pUtl->PrescanMoveLog("Rect Wafer Edge reverify top");
			break;
		case 2:	// left
			lPosX = lCtrX + lWfWX;
			lPosY = lCtrY + 0;
			pUtl->PrescanMoveLog("Rect Wafer Edge reverify left");
			break;
		case 3:	// bottom
			lPosX = lCtrX - 0;
			lPosY = lCtrY - lWfHY;
			pUtl->PrescanMoveLog("Rect Wafer Edge reverify bottom");
			break;
		}
		if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
		{
			if( lPrDelay>0 )
				Sleep(lPrDelay);
			SeearchWaferEdge(i*2, lAutoEdgeX[i], lAutoEdgeY[i]);
		}
	}

	// compare based verify corner and edge search to find most far points
	for(i=0; i<4; i++)
	{
		if( lMinX>lAutoEdgeX[i] )
			lMinX = lAutoEdgeX[i];
		if( lMaxX<lAutoEdgeX[i] )
			lMaxX = lAutoEdgeX[i];
		if( lMinY>lAutoEdgeY[i] )
			lMinY = lAutoEdgeY[i];
		if( lMaxY<lAutoEdgeY[i] )
			lMaxY = lAutoEdgeY[i];
	}
	lWfWX = labs(lMaxX-lMinX);
	lWfHY = labs(lMaxY-lMinY);
	lCtrX = (lMaxX+lMinX)/2;
	lCtrY = (lMaxY+lMinY)/2;

	LONG lPrevWX = m_lAutoWaferWidth;
	LONG lPrevHY = m_lAutoWaferHeight;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	UINT unWafEdgeNum = pApp->GetPrescanWaferEdgeNum();
	m_nPrescanMapWidth  = m_lAutoWaferWidth		= lWfWX + 2*unWafEdgeNum*GetPrescanPitchX();
	m_nPrescanMapHeight = m_lAutoWaferHeight	= lWfHY + 2*unWafEdgeNum*GetPrescanPitchY();
	m_lAutoWaferCenterX	= lCtrX;
	m_lAutoWaferCenterY	= lCtrY;
	m_nPrescanMapRadius = max(m_nPrescanMapWidth, m_nPrescanMapHeight)/2;
	m_lAutoWaferDiameter = m_nPrescanMapRadius*2;

	CString szLogMsg;
	szLogMsg.Format("Rect Wafer Edge center(%ld,%ld); dimension(%ld,%ld), previous(%ld,%ld)",
		lCtrX, lCtrY, m_lAutoWaferWidth, m_lAutoWaferHeight, lPrevWX, lPrevHY);
	pUtl->PrescanMoveLog(szLogMsg);

	if( IsScanAlignWafer()==FALSE && m_bAutoWaferLimitOnce )
	{
		if( labs(lPrevWX-m_lAutoWaferWidth)>labs(30*lPrevWX/100) )
		{
			HmiMessage_Red_Back("auto search wafer limit width 0.30 vary than before");
		}
		if( labs(lPrevHY-m_lAutoWaferHeight)>labs(30*lPrevHY/100) )
		{
			HmiMessage_Red_Back("auto search wafer limit height 0.30 vary than before");
		}
	}
	m_bAutoWaferLimitOk = TRUE;
	m_bAutoWaferLimitOnce = TRUE;
}

BOOL CWaferTable::IsThisDieDirectPick()
{
	if ((m_lReadyToSampleOnWft != 0) ||	(m_lReadyToCleanEjPinOnWft != 0))	//	check LF
	{
		return FALSE;
	}
	// look forward die and no confirm search and no rotation
	BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] && IsUseLF();	
	if( bLFNoComp && (IsVerifyMapWaferEnable()==FALSE || m_bThisDieNeedVerify==FALSE) )
	{
		return TRUE;
	}

	return FALSE;
}

LONG CWaferTable::GetCurrentRow()
{
	return m_stCurrentDie.lY;
}

LONG CWaferTable::GetCurrentCol()
{
	return m_stCurrentDie.lX;
}

LONG CWaferTable::GetCurrentEncX()
{
	return m_stCurrentDie.lX_Enc;
}

LONG CWaferTable::GetCurrentEncY()
{
	return m_stCurrentDie.lY_Enc;
}

WAF_CDieSelectionAlgorithm::WAF_EDieAction CWaferTable::GetCurrDieEAct()
{
	return m_stCurrentDie.eAction;
}

WAF_CDieSelectionAlgorithm::WAF_EDieAction CWaferTable::GetLastDieEAct()
{
	return m_stLastDie.eAction;
}

UCHAR CWaferTable::GetCurrDieGrade()
{
	return m_stCurrentDie.ucGrade;
}

LONG CWaferTable::GetLastDieRow()
{
	return m_stLastDie.lY;
}

LONG CWaferTable::GetLastDieCol()
{
	return m_stLastDie.lX;
}

LONG CWaferTable::GetLastDieEncX()
{
	return m_stLastDie.lX_Enc;
}

LONG CWaferTable::GetLastDieEncY()
{
	return m_stLastDie.lY_Enc;
}

BOOL CWaferTable::Check201277PasswordWithWeek()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	int nPass = (int) pApp->GetProfileInt(gszPROFILE_SETTING, _T("SemitekPassword"), 201277);
	CString szPass;
	CTime stTime = CTime::GetCurrentTime();
	szPass.Format("%d%d", nPass, stTime.GetDayOfWeek());

	return CheckOperatorPassWord(szPass);
}

LONG CWaferTable::CheckSemitekPasswordWithWeek(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( pApp->GetCustomerName()==CTM_SEMITEK )
	{
		bReturn = Check201277PasswordWithWeek();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckSemitekPassword(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( pApp->GetCustomerName()==CTM_SEMITEK )
	{
		bReturn = Check201277PasswordToGo();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckCreePassword(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( pApp->GetCustomerName()==CTM_CREE )
	{
		bReturn = Check201277PasswordToGo();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckPassword201277(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = Check201277PasswordToGo();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckRebelPassword(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bRebelLumileds = pApp->IsPLLMRebel();
	if( bRebelLumileds && pApp->CheckLoginLevel()<2 )
	{
		bReturn = Check201277PasswordToGo();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckRebelAndStop(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bRebelLumileds = pApp->IsPLLMRebel();
	if( bRebelLumileds )
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}	// rebel special

BOOL CWaferTable::CheckSubMapWaferMatch_Align(INT nDir, LONG lBaseRow, LONG lBaseCol, LONG lBaseX, LONG lBaseY)
{
	int i, j;
	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];

	LONG lWorkRow, lWorkCol, lInRow, lInCol;
	BOOL bAlignOK = TRUE;
	CString szMsg;

	CString szState;

	switch( nDir )
	{
	case 0:	// center
		lInRow = lBaseRow;
		lInCol = lBaseCol;
		szState = "C Verify ";
		break;
	case 1:	// left
		lInRow = lBaseRow;
		lInCol = lBaseCol - (LONG)dFOVX;
		szState = "L Verify ";
		break;
	case 2:	// right
		lInRow = lBaseRow;
		lInCol = lBaseCol + (LONG)dFOVX;
		szState = "R Verify ";
		break;
	case 3:	// up
		lInRow = lBaseRow - (LONG)dFOVY;
		lInCol = lBaseCol;
		szState = "U Verify ";
		break;
	case 4:	// down
		lInRow = lBaseRow + (LONG)dFOVY;
		lInCol = lBaseCol;
		szState = "D Verify ";
		break;
	default:
		lInRow = lBaseRow;
		lInCol = lBaseCol;
		szState = "C Verify ";
		break;
	}

	BOOL bFindOK = FALSE;
	lWorkRow = lInRow;
	lWorkCol = lInCol;
	bFindOK = CheckAlignMapWaferMatchHasDie(lWorkRow, lWorkCol);

	if( bFindOK==FALSE )
	{
		for(i=0; i<9; i++)
		{
			lWorkRow = lInRow - 1 + i/3;
			lWorkCol = lInCol - 1 + i%3;

			bFindOK = CheckAlignMapWaferMatchHasDie(lWorkRow, lWorkCol);

			if( bFindOK )
			{
				break;
			}
		}
	}

	if( bFindOK==FALSE )
	{
		return TRUE;
	}

	LONG lGetX=0, lGetY=0;
	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();
	LONG lDiePitchY_Y = GetDiePitchY_Y();

	LONG lDiff_X = lWorkCol - lBaseCol;
	LONG lDiff_Y = lWorkRow - lBaseRow;

	lGetX = lBaseX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
	lGetY = lBaseY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;

	if( XY_SafeMoveTo(lGetX, lGetY)==FALSE )
	{
		return FALSE;
	}
	Sleep(50);

	WT_LA_DIE_RESULT	stLADieState;
	memset(&stLADieState, 0, sizeof(WT_LA_DIE_RESULT));

	WftMoveSearchDie(lGetX, lGetY, TRUE);

	(*m_psmfSRam)["MS896A"]["WaferTableX"] = lGetX;
	(*m_psmfSRam)["MS896A"]["WaferTableY"] = lGetY;
	(*m_psmfSRam)["MS896A"]["WaferMapRow"] = lWorkRow;
	(*m_psmfSRam)["MS896A"]["WaferMapCol"] = lWorkCol;
	GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_ARP);	// add refer points, pr grab and sceen save and log file

	IPC_CServiceMessage stMsg;
	int nConvID;
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "AutoLookAroundDie_Cmd", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(WT_LA_DIE_RESULT), &stLADieState);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	INT lWidth = (INT)(LONG)((*m_psmfSRam)["WaferPr"]["FOV"]["BaseCol"]);
	INT lHeight = (INT)(LONG)((*m_psmfSRam)["WaferPr"]["FOV"]["BaseRow"]);

	BOOL bFindDie = FALSE;
	LONG lCRow, lCCol;
	CString szTemp;
	szMsg.Format("Search Matrix Row %d, Col %d;", lWidth*2+1, lHeight*2+1);
	for(i=0; i<(lHeight*2+1); i++)
	{
		for(j=0; j<(lWidth*2+1); j++)
		{
			lCRow = lWorkRow + (i-lHeight);
			lCCol = lWorkCol + (j-lWidth);

			bFindDie = (stLADieState.m_sDieState[i][j]==2);
			szTemp.Format(" point(%d,%d) map(%d,%d) %d;", i, j, lCRow, lCCol, bFindDie);
			szMsg += szTemp;
			if( CheckAlignMapWaferMatchHasDie(lCRow, lCCol) )
			{
				szMsg += " has die;";

				if( bFindDie!=TRUE )
				{
					bAlignOK = FALSE;
				}
			}

			if( bAlignOK==FALSE )
			{
				szTemp.Format(" not match");
				szMsg += szTemp;
				break;
			}
		}
		if( bAlignOK==FALSE )
		{
			break;
		}
	}

	if( bAlignOK )
	{
		szMsg = "Align verify map-wafer OK";
	}
	else
	{
		szMsg = "Align verify map-wafer FAIL";
	}

	return bAlignOK;
}

BOOL CWaferTable::CheckMapWaferMatch_Align(LONG lBaseRow, LONG lBaseCol, LONG lBaseX, LONG lBaseY)
{
	CString szMsg;
	BOOL bMapOK = FALSE;
	int i;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_SEMITEK )	//	427TX	4
	{
		return TRUE;
	}

	if( IsLFSizeOK()==FALSE )
	{
		return TRUE;
	}

	if( IsVerifyMapWaferEnable()==FALSE )
	{
		return TRUE;
	}

	for(i=0; i<5; i++)
	{
		bMapOK = CheckSubMapWaferMatch_Align(i, lBaseRow, lBaseCol, lBaseX, lBaseY);
		if( bMapOK==FALSE )
		{
			break;
		}
	}

	if( XY_SafeMoveTo(lBaseX, lBaseY)==FALSE )
	{
		return FALSE;
	}
	Sleep(50);

	return bMapOK;
}

BOOL CWaferTable::PrescanOpGetPosn_1DieIndex(const short sDown, const BOOL bVirtualHome)
{
	LONG	lTgtWfX = 0, lTgtWfY = 0;
	LONG	lTgtRow = 0, lTgtCol = 0;
	LONG	lDiffX = 0, lDiffY = 0;
	UCHAR	ucNullBin = m_WaferMapWrapper.GetNullBin();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsAutoDetectEdge() && m_lDetectEdgeState<(3+m_ucPrescanMapShapeType) )
	{
		CString szText;
		LONG	lTgtWfX = 0, lTgtWfY = 0, lTgtRow = 0, lTgtCol = 0, lDiffX = 0, lDiffY = 0;
		LONG lOldWfX = GetScanLastPosnX();
		LONG lOldWfY = GetScanLastPosnY();
		LONG lOldRow = m_nPrescanLastMapRow;
		LONG lOldCol = m_nPrescanLastMapCol;
		BOOL bNextFail = PrescanOpGetPosn_MapIndex(lOldRow, lOldCol, lOldWfX, lOldWfY, 
									lTgtRow, lTgtCol, lTgtWfX, lTgtWfY, m_lDetectEdgeState);
		if( (bNextFail) )
		{
			m_lDetectEdgeState++;
			bNextFail = TRUE;
			szText.Format("change direction %d", m_lDetectEdgeState);
			pUtl->PrescanMoveLog(szText);
		}

		if( bNextFail )
		{
			LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
			if( GetMapIndexStepCol()%2==0 )
				lGrabOffsetX = GetDiePitchX_X()/2;
			if( GetMapIndexStepRow()%2==0 )
				lGrabOffsetY = GetDiePitchY_Y()/2;
			lOldWfX	= GetPrescanAlignPosnX() + lGrabOffsetX;
			lOldWfY	= GetPrescanAlignPosnY() + lGrabOffsetY;
			lOldRow	= m_nPrescanAlignMapRow;
			lOldCol	= m_nPrescanAlignMapCol;
			bNextFail = PrescanOpGetPosn_MapIndex(lOldRow, lOldCol, lOldWfX, lOldWfY, 
									lTgtRow, lTgtCol, lTgtWfX, lTgtWfY, m_lDetectEdgeState);
		}

		if( m_lDetectEdgeState == (3+m_ucPrescanMapShapeType) || bNextFail )
		{
			Sleep(100);
			while( 1 )
			{
				Sleep(100);
				if( pUtl->GetPrescanMapIndexIdle() && 
					m_bPrescanLastGrabbed==TRUE  )
				{
					Sleep(10);
					break;
				}
			}
			szText.Format("calculate wafer limit old %d; (%d,%d) diameter %d", 
				m_lDetectEdgeState, m_lAutoWaferCenterX, m_lAutoWaferCenterY, m_lAutoWaferDiameter );
			pUtl->PrescanMoveLog(szText);
			OpCalculateADEWaferLimit();
			szText.Format("calculate wafer limit new %d; (%d,%d) diameter %d", 
				m_lDetectEdgeState, m_lAutoWaferCenterX, m_lAutoWaferCenterY, m_lAutoWaferDiameter );
			pUtl->PrescanMoveLog(szText);

			LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
			if( GetMapIndexStepCol()%2==0 )
				lGrabOffsetX = GetDiePitchX_X()/2;
			if( GetMapIndexStepRow()%2==0 )
				lGrabOffsetY = GetDiePitchY_Y()/2;
			m_nPrescanLastWftPosnX	= GetPrescanAlignPosnX() + lGrabOffsetX;
			m_nPrescanLastWftPosnY	= GetPrescanAlignPosnY() + lGrabOffsetY;
			m_nPrescanLastMapRow	= m_nPrescanAlignMapRow;
			m_nPrescanLastMapCol	= m_nPrescanAlignMapCol;
			m_nPrescanDirection		= 1;
		}
		else
		{
			m_nPrescanNextMapRow	= lTgtRow;
			m_nPrescanNextMapCol	= lTgtCol;
			m_nPrescanNextWftPosnX	= lTgtWfX;
			m_nPrescanNextWftPosnY	= lTgtWfY;

			return FALSE;
		}
	}

	LONG lOldWfX = GetScanLastPosnX();
	LONG lOldWfY = GetScanLastPosnY();
	LONG lOldRow = m_nPrescanLastMapRow;
	LONG lOldCol = m_nPrescanLastMapCol;

	CString szText;
	szText.Format("wt old, %5ld,%5ld,  nrm,  wft,%8ld,%8ld, dir %d", lOldRow, lOldCol, lOldWfX, lOldWfY, m_nPrescanDirection);
	pUtl->PrescanMoveLog(szText);

	BOOL bHoriIndex = TRUE;
	if( m_ucScanWalkTour==WT_SCAN_WALK_LEFT_VERT || m_ucScanWalkTour==WT_SCAN_WALK_HOME_VERT )
	{
		bHoriIndex = FALSE;
	}
	LONG lScnX = 0, lScnY = 0;
	LONG lStepCol = GetMapIndexStepCol();
	LONG lStepRow = GetMapIndexStepRow();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( m_bDualWayTipWafer )
	{
		bHoriIndex = TRUE;
		lStepCol = 2;
	}
	BOOL bHasDie = FALSE;
	// move map to right, table to left
	lDiffY = 0;
	lDiffX = 0;
	while( 1 )
	{
		if( bHoriIndex )
		{
			if( m_nPrescanDirection==1 )
			{
				lDiffX += lStepCol;
			}
			else
			{
				lDiffX -= lStepCol;
			}
			lTgtRow = lOldRow + lDiffY;
			lTgtCol = lOldCol + lDiffX;

			if( (m_nPrescanDirection==1) && (lTgtCol>(GetMapValidMaxCol()+(lStepCol+1)/2)) )
			{
				m_nPrescanDirection = 2;
				lDiffY += sDown*lStepRow;
			}

			if( (m_nPrescanDirection==2) && (lTgtCol<(GetMapValidMinCol()-(lStepCol+1)/2)) )
			{
				m_nPrescanDirection = 1;
				lDiffY += sDown*lStepRow;
			}

			if( lTgtRow>(GetMapValidMaxRow()+(lStepRow+1)/2) || 
				lTgtRow<(GetMapValidMinRow()-(lStepRow+1)/2) )
			{
				bHasDie = FALSE;
				break;
			}
			if( IsOutMS90SortingPart(lTgtRow, lTgtCol) )
			{
				bHasDie = FALSE;
				break;
			}
		}
		else
		{
			if( m_nPrescanDirection==1 )
			{
				lDiffY += lStepRow;
			}
			else
			{
				lDiffY -= lStepRow;
			}

			lTgtRow = lOldRow + lDiffY;
			lTgtCol = lOldCol + lDiffX;

			if( (m_nPrescanDirection==1) && (lTgtRow>(GetMapValidMaxRow()+(lStepRow+1)/2)) )
			{
				m_nPrescanDirection = 2;
				lDiffX += sDown*lStepCol;
			}

			if( (m_nPrescanDirection==2) && (lTgtRow<(GetMapValidMinRow()-(lStepRow+1)/2)) )
			{
				m_nPrescanDirection = 1;
				lDiffX += sDown*lStepCol;
			}

			if( lTgtCol>(GetMapValidMaxCol()+(lStepCol+1)/2) || 
				lTgtCol<(GetMapValidMinCol()-(lStepCol+1)/2) )
			{
				bHasDie = FALSE;
				break;
			}
			if( IsOutMS90SortingPart(lTgtRow, lTgtCol) )
			{
				bHasDie = FALSE;
				break;
			}
		}

		LONG lHalfRow = lStepRow/2;
		LONG lHalfCol = lStepCol/2;
		for(LONG jRow=(0-lHalfRow); jRow<=(0+lHalfRow); jRow++)
		{
			for(LONG iCol=(0-lHalfCol); iCol<=(0+lHalfCol); iCol++)
			{
				LONG lChkRow = jRow+lTgtRow;
				LONG lChkCol = iCol+lTgtCol;
				if( IsInMapValidRange(lChkRow, lChkCol)==FALSE )
				{
					continue;
				}
				if( IsAutoDetectEdge() && GetMapPhyPosn(lChkRow, lChkCol, lScnX, lScnY) )	// if already scanned, skip to check other dice
				{
					szText.Format("ade wt scned, %5ld,%5ld, normal wft  %5ld,%5ld", lChkRow, lChkCol, lScnX, lScnY);
					pUtl->PrescanMoveLog(szText);
					continue;
				}
				if (m_pWaferMapManager->IsMapHaveBin(lChkRow, lChkCol))
				{
					bHasDie = TRUE;
					break;
				}
			}
			if( bHasDie )
			{
				break;
			}
		}

		if( bHasDie )
		{
			break;
		}
	}

	if( bHasDie==FALSE )
	{
		pUtl->PrescanMoveLog("end of wafer");
		return TRUE;
	}

	CString szNear = "no die, ";
	BOOL bByMapNearby = FALSE;
	LONG lNearX = 0, lNearY = 0;
	LONG lLoopCounter = 0;
	LONG lMapIndexLoop = max(lStepCol, lStepRow)*m_ulMapIndexLoop;
	while( 1 )
	{
		if( OneDieIndexGetDieValidPosn(lTgtRow, lTgtCol, lMapIndexLoop, lNearX, lNearY, TRUE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByMapNearby = TRUE;
			szNear = "wt mp1, ";
			break;
		}
		if( bVirtualHome==FALSE )
		{
			Sleep(10);
		}
		if( pUtl->GetPrescanMapIndexIdle() && 
			m_lPrescanLastGrabRow==lOldRow && 
			m_lPrescanLastGrabCol==lOldCol && 
			m_bPrescanLastGrabbed==TRUE  )
		{
			if( bVirtualHome==FALSE )
			{
				Sleep(10);
			}
			break;
		}
		if( bVirtualHome && pUtl->GetPrescanMapIndexIdle() && m_bPrescanLastGrabbed )
		{
			break;
		}
		lLoopCounter++;
		if( lLoopCounter>1000 ) // > 0.5 seconds
		{
			break;
		}
	}

	if( lLoopCounter<=1000 && bByMapNearby==FALSE )
	{
		if( OneDieIndexGetDieValidPosn(lTgtRow, lTgtCol, lMapIndexLoop, lNearX, lNearY, TRUE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByMapNearby = TRUE;
			szNear = "wt mp2, ";
		}
	}

	if( bByMapNearby==FALSE )
	{
		LONG lNearX = 0, lNearY = 0;
		lMapIndexLoop = lMapIndexLoop *2;
		if( GetDieValidPrescanPosn(lTgtRow, lTgtCol, lMapIndexLoop, lNearX, lNearY, TRUE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByMapNearby = TRUE;
			szNear = "wt mp3, ";
			lLoopCounter = -1;
		}
	}

	if( bByMapNearby )
	{
		LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
		if( GetMapIndexStepCol()%2==0 )
			lGrabOffsetX = GetDiePitchX_X()/2;
		if( GetMapIndexStepRow()%2==0 )
			lGrabOffsetY = GetDiePitchY_Y()/2;
		lTgtWfX = lTgtWfX + lGrabOffsetX;
		lTgtWfY = lTgtWfY + lGrabOffsetY;
	}
	else
	{
		LONG lDiePitchX_X	= GetDiePitchX_X();
		LONG lDiePitchX_Y	= GetDiePitchX_Y();
		LONG lDiePitchY_Y	= GetDiePitchY_Y();
		LONG lDiePitchY_X	= GetDiePitchY_X();

		lTgtWfX = lOldWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
		lTgtWfY = lOldWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
	}
	LONG lHmiRow = 0, lHmiCol = 0;
	ConvertAsmToHmiUser(lTgtRow, lTgtCol, lHmiRow, lHmiCol);
	szText.Format("%s%5ld,%5ld,%5ld,%5ld,%8ld,%8ld, %ld(%ld)", (LPCTSTR)szNear, 
		lTgtRow, lTgtCol, lHmiRow, lHmiCol, lTgtWfX, lTgtWfY, lLoopCounter, lMapIndexLoop);
	pUtl->PrescanMoveLog(szText);
	m_nPrescanNextMapRow	= lTgtRow;
	m_nPrescanNextMapCol	= lTgtCol;
	m_nPrescanNextWftPosnX	= lTgtWfX;
	m_nPrescanNextWftPosnY	= lTgtWfY;

	return FALSE;
}

LONG CWaferTable::Enable6InchWaferTable(IPC_CServiceMessage &svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	m_bEnable6InchWaferTable	= FALSE;
	

	pApp->WriteProfileInt(gszPROFILE_SETTING, "Enable 6 Inch Wafer Table", m_bEnable6InchWaferTable);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckES100Machine(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::LoadES100Screen(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::InspectSelectType(IPC_CServiceMessage& svMsg)
{
	LONG lPhyX, lPhyY;
	ULONG ulType, ulRow = 0, ulCol = 0;
	svMsg.GetMsg(sizeof(ULONG), &ulType);
	m_ulInspectGoType = ulType;

	m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);

	BOOL bReturn=TRUE;
	if( IsAlignedWafer()==FALSE )
	{
		SetAlert(IDS_WT_MAP_NOT_ALIGN);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsPrescanEnded()==FALSE )
	{
		HmiMessage("Prescan not complete yet!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if( GetDieValidPrescanPosn(ulRow, ulCol, 2, lPhyX, lPhyY, TRUE) )
	{
		XY_SafeMoveTo(lPhyX, lPhyY);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::InspectGotoTarget(IPC_CServiceMessage& svMsg)
{
	BOOL bNextDie = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bNextDie);

	BOOL bReturn=TRUE;
	if( IsAlignedWafer()==FALSE )
	{
		SetAlert(IDS_WT_MAP_NOT_ALIGN);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsPrescanEnded()==FALSE )
	{
		HmiMessage("Prescan not complete yet!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	ULONG	ulOldRow = 0, ulOldCol = 0, ulTgtRow = 0, ulTgtCol = 0;
	ULONG	ulChkRow = 0, ulChkCol = 0, ulMaxRow = 0, ulMaxCol = 0;
	LONG	lNextRow = 0, lNextCol = 0;


	m_WaferMapWrapper.GetSelectedPosition(ulOldRow, ulOldCol);

	m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
	UCHAR	ucNullBin = m_WaferMapWrapper.GetNullBin();

	ULONG ulIndex;
	BOOL bFindNext = FALSE;
	CString szMsg;
	switch( m_ulInspectGoType )
	{
	case 1:	// defect
		szMsg = "No defect die";
		if( GetDefectSize()<=0 )
		{
			break;
		}
		if( bNextDie )
		{
			for(ulIndex=0; ulIndex<GetDefectSize(); ulIndex++)
			{
				if( GetDefectInfo(ulIndex, ulTgtRow, ulTgtCol) )
				{
					if( ulTgtRow==ulOldRow && ulTgtCol>ulOldCol )
					{
						bFindNext = TRUE;
						break;
					}
					if( ulTgtRow>ulOldRow )
					{
						bFindNext = TRUE;
						break;
					}
				}
			}
		}
		else
		{
			ULONG ii = 0;
			for(ii=0; ii<=(GetDefectSize()-1); ii++)
			{
				ulIndex = (GetDefectSize()-1)-ii;
				if( GetDefectInfo(ulIndex, ulTgtRow, ulTgtCol) )
				{
					if( ulTgtRow==ulOldRow && ulTgtCol<ulOldCol )
					{
						bFindNext = TRUE;
						break;
					}
					if( ulTgtRow<ulOldRow )
					{
						bFindNext = TRUE;
						break;
					}
				}
			}
		}
		break;

	case 2:	// bad cut
		szMsg = "No bad cut die";
		if( GetBadCutSize()<=0 )
		{
			break;
		}
		if( bNextDie )
		{
			for(ulIndex=0; ulIndex<GetBadCutSize(); ulIndex++)
			{
				if( GetBadCutInfo(ulIndex, ulTgtRow, ulTgtCol) )
				{
					if( ulTgtRow==ulOldRow && ulTgtCol>ulOldCol )
					{
						bFindNext = TRUE;
						break;
					}
					if( ulTgtRow>ulOldRow )
					{
						bFindNext = TRUE;
						break;
					}
				}
			}
		}
		else
		{
			ULONG ii = 0;
			for(ii=0; ii<=(GetBadCutSize()-1); ii++)
			{
				ulIndex=(GetBadCutSize()-1)-ii;
				if( GetBadCutInfo(ulIndex, ulTgtRow, ulTgtCol) )
				{
					if( ulTgtRow==ulOldRow && ulTgtCol<ulOldCol )
					{
						bFindNext = TRUE;
						break;
					}
					if( ulTgtRow<ulOldRow )
					{
						bFindNext = TRUE;
						break;
					}
				}
			}
		}
		break;

	case 3:	// empty
		szMsg = "No empty die";
		if( GetEmptySize()<=0 )
		{
			break;
		}
		if( bNextDie )
		{
			for(ulIndex=0; ulIndex<GetEmptySize(); ulIndex++)
			{
				if( GetEmptyInfo(ulIndex, ulTgtRow, ulTgtCol) )
				{
					if( ulTgtRow==ulOldRow && ulTgtCol>ulOldCol )
					{
						bFindNext = TRUE;
						break;
					}
					if( ulTgtRow>ulOldRow )
					{
						bFindNext = TRUE;
						break;
					}
				}
			}
		}
		else
		{
			ULONG ii=0;
			for(ii=0; ii<=(GetEmptySize()-1); ii++)
			{
				ulIndex=(GetEmptySize()-1)-ii;
				if( GetEmptyInfo(ulIndex, ulTgtRow, ulTgtCol) )
				{
					if( ulTgtRow==ulOldRow && ulTgtCol<ulOldCol )
					{
						bFindNext = TRUE;
						break;
					}
					if( ulTgtRow<ulOldRow )
					{
						bFindNext = TRUE;
						break;
					}
				}
			}
		}
		break;

	case 4:	//	twin dice/double die
		// move map to right, table to left
		lNextRow = ulOldRow;
		lNextCol = ulOldCol;
		szMsg = "No twin dice";
		while( 1 )
		{
			if( bNextDie )
			{
				lNextCol++;
				if( (lNextCol>((LONG)ulMaxCol)) )
				{
					lNextRow++;
					lNextCol = 0;
				}
				if( lNextRow>((LONG)ulMaxRow) )
				{
					break;
				}
			}
			else
			{
				lNextCol--;
				if( lNextCol<0 )
				{
					lNextRow--;
					lNextCol = ulMaxCol;
				}
				if( lNextRow<0 )
				{
					break;
				}
			}

			if( m_WaferMapWrapper.GetDieState(lNextRow, lNextCol)==WT_MAP_DS_SCAN_TWIN_DIE_FIRST )
			{
				LONG lWftX = 0, lWftY = 0;
				if( GetMapPhyPosn(lNextRow, lNextCol, lWftX, lWftY) )
				{
					bFindNext = TRUE;
					ulTgtRow = lNextRow;
					ulTgtCol = lNextCol;
					break;
				}
				if( GetPrescanRunPosn(lNextRow, lNextCol, lWftX, lWftY) )
				{
					bFindNext = TRUE;
					ulTgtRow = lNextRow;
					ulTgtCol = lNextCol;
					break;
				}
			}
		}
		break;

	case 0:	// good
	default:
	// move map to right, table to left
		lNextRow = ulOldRow;
		lNextCol = ulOldCol;
		szMsg = "No good die";
		while( 1 )
		{
			if( bNextDie )
			{
				lNextCol++;
				if( (lNextCol>((LONG)ulMaxCol)) )
				{
					lNextRow++;
					lNextCol = 0;
				}
				if( lNextRow>((LONG)ulMaxRow) )
				{
					break;
				}
			}
			else
			{
				lNextCol--;
				if( lNextCol<0 )
				{
					lNextRow--;
					lNextCol = ulMaxCol;
				}
				if( lNextRow<0 )
				{
					break;
				}
			}

			UCHAR ucDieGrade = m_WaferMapWrapper.GetGrade(lNextRow, lNextCol);
			if( IsScanMapNgGrade(ucDieGrade - m_WaferMapWrapper.GetGradeOffset()) )
			{
				continue;
			}
			if( ucDieGrade==ucNullBin )
			{
				continue;
			}
			if( m_WaferMapWrapper.IsReferenceDie(lNextRow, lNextCol) )
			{
				continue;
			}

			bFindNext = TRUE;
			for(ulIndex=0; ulIndex<GetBadCutSize(); ulIndex++)
			{
				if( GetBadCutInfo(ulIndex, ulChkRow, ulChkCol) )
				{
					if(ulChkRow==lNextRow && ulChkCol==lNextCol )
					{
						bFindNext = FALSE;
						break;
					}
				}
			}
			if( bFindNext==FALSE )
			{
				continue;
			}

			for(ulIndex=0; ulIndex<GetDefectSize(); ulIndex++)
			{
				if( GetDefectInfo(ulIndex, ulChkRow, ulChkCol) )
				{
					if(ulChkRow==lNextRow && ulChkCol==lNextCol )
					{
						bFindNext = FALSE;
						break;
					}
				}
			}
			if( bFindNext==FALSE )
			{
				continue;
			}

			for(ulIndex=0; ulIndex<GetEmptySize(); ulIndex++)
			{
				if( GetEmptyInfo(ulIndex, ulChkRow, ulChkCol) )
				{
					if(ulChkRow==lNextRow && ulChkCol==lNextCol )
					{
						bFindNext = FALSE;
						break;
					}
				}
			}
			if( bFindNext==FALSE )
			{
				continue;
			}

			ulTgtRow = lNextRow;
			ulTgtCol = lNextCol;
			break;
		}
		break;
	}

	if( bFindNext )
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( pApp->GetCustomerName()=="emcore" && m_WaferMapWrapper.IsMapValid() )	//	emcore laser bar
		{
			CStdioFile oFile;
			if( oFile.Open(GetMapFileName(), CFile::modeRead|CFile::shareDenyNone) )
			{	// Get the sample electricial data
				CString szLineData = "", szOcrValue = "";
				CStringArray szLineList;
				m_WaferMapWrapper.GetExtraInformation(&oFile, ulTgtRow, ulTgtCol, szLineData);
				szLineList.RemoveAll();
				CUtility::Instance()->ParseRawData(szLineData, szLineList);
				if( szLineList.GetSize()>=1 )
					szOcrValue = szLineList.GetAt(0);
				oFile.Close();
				m_szNextLocation = szOcrValue;
				m_szDieState = m_szDieState + " " + szOcrValue;
			}
		}	//	emcore laser bar

		m_WaferMapWrapper.SetCurrentPosition(ulTgtRow, ulTgtCol);
		Sleep(50);
		m_WaferMapWrapper.SetSelectedPosition(ulTgtRow, ulTgtCol);
		Sleep(300);
		LONG lPhyX, lPhyY;
		BOOL bGetOk = FALSE;
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		if( pUtl->GetPrescanBarWafer() )
		{
			bGetOk = GetDieValidInX(ulTgtRow, ulTgtCol, 2, lPhyX, lPhyY, TRUE);
		}
		else
		{
			bGetOk = GetDieValidPrescanPosn(ulTgtRow, ulTgtCol, 2, lPhyX, lPhyY, TRUE);
		}
		if( bGetOk )
		{
			XY_SafeMoveTo(lPhyX, lPhyY);
		}
		else
		{
			if( m_ulInspectGoType==4 )
			{
				if( GetPrescanRunPosn(ulTgtRow, ulTgtCol, lPhyX, lPhyY) )
				{
					XY_SafeMoveTo(lPhyX, lPhyY);
				}
			}
		}	//	for rescan to keep original scan die position
	}
	else
	{
		if( bNextDie )
			szMsg = szMsg + " forward!";
		else
			szMsg = szMsg + " backward!";
		HmiMessage(szMsg, "Prescan check");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferTable::OneDieIndexGetDieValidPosn(LONG ulIntRow, LONG ulIntCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap)
{
	BOOL	bFindNearDie = FALSE;
	LONG	lTmpRow, lTmpCol;
	BOOL	lStatus;
	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( m_bDualWayTipWafer==FALSE )
	{
		return GetDieValidPrescanPosn(ulIntRow, ulIntCol, ulLoop, lPhyX, lPhyY, bByMap);
	}

	lStatus = TRUE;

	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if( GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY) )
	{
		return TRUE;
	}

	for (ulCurrentLoop=1; ulCurrentLoop<=ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop*2 + 1);
		lCol = (ulCurrentLoop*2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		if( lTmpRow>=0 && lTmpCol>=0 )
		{
			if( GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY) )
			{
				bFindNearDie = TRUE;
				break;
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if( GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY) )
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if( bFindNearDie )
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if( GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY) )
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if( bFindNearDie )
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if( GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY) )
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if( bFindNearDie )
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if( GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY) )
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if( bFindNearDie )
		{
			break;
		}
	}

	if ( bFindNearDie == FALSE )
	{
		lStatus = FALSE;
	}
	else
	{
		LONG lDiePitchX_X = GetDiePitchX_X(); 
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_Y = GetDiePitchY_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();
		LONG lDieOffsetY_Y = 0;

		LONG lDiff_X = ulIntCol - lTmpCol;
		LONG lDiff_Y = ulIntRow - lTmpRow;
		LONG lDiffYY = lDiff_Y, lDiffAA = 0;

		if( labs(lDiff_Y)%2!=0 )
		{
			lDiffYY = ((lDiff_Y)/2) * 2;
			lDiffAA = lDiff_Y - lDiffYY;
			// should also take care move than 1 in Y
			if( m_nPrescanAlignScanStage==1 )
			{
				if( lTmpRow%2==m_nPrescanAlignMapRow%2 )
					lDieOffsetY_Y = abs(m_nPrescanAlign2ndDistY);
				else
					lDieOffsetY_Y = 2*labs(lDiePitchY_Y) - abs(m_nPrescanAlign2ndDistY);
			}
			else
			{
				if( lTmpRow%2!=m_nPrescanAlignMapRow%2 )
					lDieOffsetY_Y = abs(m_nPrescanAlign2ndDistY);
				else
					lDieOffsetY_Y = 2*labs(lDiePitchY_Y) - abs(m_nPrescanAlign2ndDistY);
			}
		}

		//Calculate original no grade die pos from surrounding die position
		BOOL bPhyStatus = FALSE;
		if( lTmpRow>=0 && lTmpCol>=0 )
			bPhyStatus = GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY);
		if( bPhyStatus )
		{
			lPhyX = lPhyX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lPhyY = lPhyY - lDiffYY * lDiePitchY_Y - lDiff_X * lDiePitchX_Y - lDiffAA * lDieOffsetY_Y;
		}
		else
		{
			lStatus = FALSE;
		}
	}

	return lStatus;
}

BOOL CWaferTable::RectWaferCornerDieCheck(UCHAR ucCorner)
{
	//
	//		1
	//	0	4	2
	//		3
	//
	LONG	lPosX, lPosY;
	CString strTemp, szTitle;
	szTitle = "Rect Wafer Corner Check";

	if( IsLFSizeOK()==FALSE )
		return TRUE;

	MultiSrchInitNmlDie1();

	LONG	lDiePitchX_X = 0, lDiePitchX_Y = 0, lDiePitchY_X = 0, lDiePitchY_Y = 0;
	LONG	lColDiff = 0, lRowDiff = 0;
	lDiePitchX_X = GetDiePitchX_X(); 
	lDiePitchX_Y = GetDiePitchX_Y();
	lDiePitchY_X = GetDiePitchY_X();
	lDiePitchY_Y = GetDiePitchY_Y();

	MULTI_SRCH_RESULT	stMsch;

	if( ucCorner==0 )
		strTemp.Format("UL Corner verify, ");
	if( ucCorner==1 )
		strTemp.Format("UR Corner verify, ");
	if( ucCorner==2 )
		strTemp.Format("LL Corner verify, ");
	if( ucCorner==3 )
		strTemp.Format("LR Corner verify, ");

	strTemp = strTemp + "\nFind Die outer by LF.\n" + "Continue?";
	GetEncoderValue();
	lPosX = GetCurrX();
	lPosY = GetCurrY();
	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lPosX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lPosY;
	ChangeCameraToWafer();
	BOOL bDrawDie = FALSE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bDrawDie);
	int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MultiSearchNmlDie1", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	stMsg.GetMsg(sizeof(MULTI_SRCH_RESULT), &stMsch);

	if( ucCorner==0 ) // UL
	{
		if( stMsch.bDieState[0]!=0 || stMsch.bDieState[1]!=0 )
		{
			return HmiAlarmContinueMessage(strTemp, szTitle);
		}
	}

	if( ucCorner==1 ) // UR
	{
		if( stMsch.bDieState[1]!=0 || stMsch.bDieState[2]!=0 )
		{
			return HmiAlarmContinueMessage(strTemp, szTitle);
		}
	}

	if( ucCorner==2 ) // LL
	{
		if( stMsch.bDieState[0]!=0 || stMsch.bDieState[3]!=0 )
		{
			return HmiAlarmContinueMessage(strTemp, szTitle);
		}
	}

	if( ucCorner==3 ) // LR
	{
		if( stMsch.bDieState[2]!=0 || stMsch.bDieState[3]!=0 )
		{
			return HmiAlarmContinueMessage(strTemp, szTitle);
		}
	}

	return TRUE;
}

LONG CWaferTable::RectWaferAroundDieCheck()
{
	//
	//		1
	//	0	4	2
	//		3
	//
	if( IsLFSizeOK()==FALSE )
		return 0;

	MultiSrchInitNmlDie1();

	MULTI_SRCH_RESULT	stMsch;

	GetEncoderValue();
	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = GetCurrX();
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = GetCurrY();
	ChangeCameraToWafer();
	BOOL bDrawDie = FALSE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bDrawDie);
	int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MultiSearchNmlDie1", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	stMsg.GetMsg(sizeof(MULTI_SRCH_RESULT), &stMsch);

	LONG lDieResult = 0;
	if( stMsch.bDieState[0]==1 )
		lDieResult += 1;
	if( stMsch.bDieState[1]==1 )
		lDieResult += 10;
	if( stMsch.bDieState[2]==1 )
		lDieResult += 100;
	if( stMsch.bDieState[3]==1 )
		lDieResult += 1000;
	if( stMsch.bDieState[4]==1 )
		lDieResult += 10000;

	return lDieResult;
}

VOID CWaferTable::MultiSrchInitNmlDie1(BOOL bInspect)
{
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(BOOL), &bInspect);
	int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MultiSearchInitNmlDie1", stMsg);	//	not for prescan, search angle can be large
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

// 4.24TX1	should choose profile based on move distance
BOOL CWaferTable::XY_MoveToPrescanPosn()	//	427TX	5
{
	LONG lPosX = GetScanLastPosnX();
	LONG lPosY = GetScanLastPosnY();
	LONG lRow  = m_nPrescanLastMapRow;
	LONG lCol  = m_nPrescanLastMapCol;
	LONG lX, lY, lDistX, lDistY;
	BOOL bX1st=TRUE, bY1st=TRUE, bXYOK = TRUE;
	BOOL bRtnMove = TRUE, bChgProfY = FALSE, bChgProfX = FALSE;

	m_WaferMapWrapper.SetCurrentPosition(lRow, lCol);
	lX = m_lLastScanGrabPosnX;
	lY = m_lLastScanGrabPosnY;
	bX1st = IsWithinWaferLimit(lPosX, lY)	 && IsWithinMapDieLimit(lPosX, lY);
	bY1st = IsWithinWaferLimit(lX, lPosY)	 && IsWithinMapDieLimit(lX, lPosY);
	bXYOK = IsWithinWaferLimit(lPosX, lPosY) && IsWithinMapDieLimit(lPosX, lPosY);

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szText;
	LONG lHmiRow = 0, lHmiCol = 0;
	ConvertAsmToHmiUser(lRow, lCol, lHmiRow, lHmiCol);

	if( bXYOK==FALSE )
	{
		pUtl->PrescanMoveLog("wt mve error out limit");
		return FALSE;
	}

	lDistX = labs(lX-lPosX);
	lDistY = labs(lY-lPosY);

	LONG lTime_X = 80;
	LONG lTime_Y = 80;

#ifdef NU_MOTION
	if( IsESMachine())		//v4.37T10
	{
		Y_Profile(NORMAL_PROF);
		X_Profile(NORMAL_PROF);
		lTime_X = X_ProfileTime(NORMAL_PROF, lDistX, lDistX);
		lTime_Y = Y_ProfileTime(NORMAL_PROF, lDistY, lDistY);
		{
			if( m_nProfile_X!=LOW_PROF )
			{
				if (lDistX > GetSlowProfileDist())
				{
					bChgProfX = TRUE;
				}
			}
			if( m_nProfile_Y!=LOW_PROF )
			{
				if (lDistY > GetSlowProfileDist())
				{
					Y_Profile(LOW_PROF);
					bChgProfY = TRUE;
				}
			}
		}
		if( bChgProfX )
			lTime_X = X_ProfileTime(LOW_PROF, lDistX, lDistX);
		if( bChgProfY )
			lTime_Y = Y_ProfileTime(LOW_PROF, lDistY, lDistY);
	}
	else
	{
		lTime_X = X_ProfileTime(LOW_PROF, lDistX, lDistX);
		lTime_Y = Y_ProfileTime(LOW_PROF, lDistY, lDistY);
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
	}
#else
	lTime_X = X_ProfileTime(LOW_PROF, lDistX, lDistX);
	lTime_Y = Y_ProfileTime(LOW_PROF, lDistY, lDistY);
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
#endif

	TakeTime((TIME_ENUM)SCAN_WFT_MOVE_1);		// prescan_time index move begin
	if( bXYOK && bX1st && bY1st )	// sync move mode
	{
		if( X_MoveTo(lPosX, SFM_NOWAIT)==Err_WTableXMove )
		{
			bRtnMove = FALSE;
		}
		if( Y_MoveTo(lPosY, SFM_NOWAIT)==Err_WTableYMove )
		{
			bRtnMove = FALSE;
		}

		LONG lTime = max(lTime_X, lTime_Y);
		if( lTime>0 )
		{
			Sleep(lTime);
		}
		else
		{
			X_Sync();
			Y_Sync();
		}
	}
	else
	{
		if ( bX1st == TRUE )
		{
			if( X_MoveTo(lPosX, SFM_WAIT) == Err_WTableXMove )
			{
				bRtnMove = FALSE;
			}
			if( Y_MoveTo(lPosY, SFM_WAIT) == Err_WTableYMove )
			{
				bRtnMove = FALSE;
			}
		}
		else if ( bY1st == TRUE )
		{
			if( Y_MoveTo(lPosY, SFM_WAIT) == Err_WTableYMove )
			{
				bRtnMove = FALSE;
			}
			if( X_MoveTo(lPosX, SFM_WAIT) == Err_WTableXMove )
			{
				bRtnMove = FALSE;
			}
		}
		else
		{
			bRtnMove = FALSE;
		}
	}

#ifdef NU_MOTION
	if( IsESMachine() )		//v4.37T10
	{
		if( bChgProfY )
			Y_Profile(NORMAL_PROF);
		if( bChgProfX )
			X_Profile(NORMAL_PROF);
		if( bChgProfX || bChgProfY )
		{
			Sleep(m_lPrescanPrDelay);
		}
	}
#else
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif
	Sleep(100);
	TakeTime((TIME_ENUM)SCAN_WFT_MOVE_2);		// prescan_time index move end

	m_lLastScanGrabPosnX = lPosX;
	m_lLastScanGrabPosnY = lPosY;
	szText.Format("wt mve, %5ld,%5ld,%5ld,%5ld,%8ld,%8ld", 
		lRow, lCol, lHmiRow, lHmiCol, lPosX, lPosY);
	pUtl->PrescanMoveLog(szText);
	return bRtnMove;
}

BOOL CWaferTable::PrescanOpGetPosn_SprialIndex()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	LONG lOldWfX = GetScanLastPosnX();
	LONG lOldWfY = GetScanLastPosnY();
	LONG lOldRow = m_nPrescanLastMapRow;
	LONG lOldCol = m_nPrescanLastMapCol;

	CString szText;
	szText.Format("wt old, %5ld,%5ld,  spr,  wft,%8ld,%8ld", 
		lOldRow, lOldCol, lOldWfX, lOldWfY);
	pUtl->PrescanMoveLog(szText);

	// move map to right, table to left
	BOOL	bFindNextDie = FALSE;
	LONG	lUpIndex = 0, lDnIndex = 0, lLtIndex = 0, lRtIndex = 0;
	LONG	lJumpRow = GetMapIndexStepRow();
	LONG	lJumpCol = GetMapIndexStepCol();
	LONG	lLoopRow = m_ulAlignRow;
	LONG	lLoopCol = m_ulAlignCol;

	BOOL bNextBreak = FALSE;
	ULONG lFastHomeLoopLimit	= max(GetMapValidMaxRow()-GetMapValidMinRow(), GetMapValidMaxCol()-GetMapValidMinCol());
	for(ULONG ulLoop=1; ulLoop<=lFastHomeLoopLimit; ulLoop++)
	{
		LONG lSpanDist = (ulLoop*2 + 1);

		//Move to RIGHT & PR search on current die
		if( lLoopRow==lOldRow && lLoopCol==lOldCol )
			bNextBreak = TRUE;
		lLoopCol += lJumpCol;	//Move table to RIGHT outer one frame
		if( lLoopRow>=0 && lLoopCol>=0 && bNextBreak )
		{
			bFindNextDie = TRUE;
			break;
		}

		if( lLoopRow>=(LONG)(2*m_ulAlignRow) && lLoopCol>=(LONG)(2*m_ulAlignCol) )
		{
			return TRUE;
		}
		if( lLoopRow<=0 && lLoopCol<=0 )
		{
			return TRUE;
		}

		//Move to UP & PR search on current die
		lUpIndex = lSpanDist-1-1;
		while(1)
		{
			if( lLoopRow==lOldRow && lLoopCol==lOldCol )
				bNextBreak = TRUE;
			lLoopRow -= lJumpRow;	//Move table to UP
			if( lLoopRow>=0 && lLoopCol>=0 && bNextBreak )
			{
				bFindNextDie = TRUE;
				break;
			}

			lUpIndex--;
			if (lUpIndex == 0)
			{
				break;
			}
		}
		if( bFindNextDie )
		{
			break;
		}

		if( lLoopRow>=(LONG)(2*m_ulAlignRow) && lLoopCol>=(LONG)(2*m_ulAlignCol) )
		{
			return TRUE;
		}
		if( lLoopRow<=0 && lLoopCol<=0 )
		{
			return TRUE;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = lSpanDist-1;
		while(1)
		{
			if( lLoopRow==lOldRow && lLoopCol==lOldCol )
				bNextBreak = TRUE;
			lLoopCol -= lJumpCol;	//Move table to LEFT
			if( lLoopRow>=0 && lLoopCol>=0 && bNextBreak )
			{
				bFindNextDie = TRUE;
				break;
			}

			lLtIndex--;
			if (lLtIndex == 0)
			{
				break;
			}
		}
		if( bFindNextDie )
		{
			break;
		}

		if( lLoopRow>=(LONG)(2*m_ulAlignRow) && lLoopCol>=(LONG)(2*m_ulAlignCol) )
		{
			return TRUE;
		}
		if( lLoopRow<=0 && lLoopCol<=0 )
		{
			return TRUE;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = lSpanDist-1;
		while(1)
		{
			if( lLoopRow==lOldRow && lLoopCol==lOldCol )
				bNextBreak = TRUE;
			lLoopRow += lJumpRow;	//Move table to DN
			if( lLoopRow>=0 && lLoopCol>=0 && bNextBreak )
			{
				bFindNextDie = TRUE;
				break;
			}

			lDnIndex--;
			if (lDnIndex == 0)
			{
				break;
			}
		}
		if( bFindNextDie )
		{
			break;
		}

		if( lLoopRow>=(LONG)(2*m_ulAlignRow) && lLoopCol>=(LONG)(2*m_ulAlignCol) )
		{
			return TRUE;
		}
		if( lLoopRow<=0 && lLoopCol<=0 )
		{
			return TRUE;
		}

		//Move to RIGHT & PR search on current die
		lRtIndex = lSpanDist-1;
		while(1)
		{
			if( lLoopRow==lOldRow && lLoopCol==lOldCol )
				bNextBreak = TRUE;
			lLoopCol += lJumpCol;	//Move table to map RIGHT
			if( lLoopRow>=0 && lLoopCol>=0 && bNextBreak )
			{
				bFindNextDie = TRUE;
				break;
			}

			lRtIndex--;
			if (lRtIndex == 0)
			{
				break;
			}
		}
		if( bFindNextDie )
		{
			break;
		}

		if( lLoopRow>=(LONG)(2*m_ulAlignRow) && lLoopCol>=(LONG)(2*m_ulAlignCol) )
		{
			return TRUE;
		}
		if( lLoopRow<=0 && lLoopCol<=0 )
		{
			return TRUE;
		}
	}

	if ( bFindNextDie == FALSE )
	{
		HmiMessage_Red_Back("Please stop machine and manual align wafer!", "AOI Prescan");
		return TRUE;
	}

	LONG	lTgtWfX = 0, lTgtWfY = 0;
	LONG	lTgtRow = lLoopRow;
	LONG	lTgtCol = lLoopCol;

	BOOL bByScanNearby = FALSE;
	LONG lCounter = 0;
	LONG lLoop = max(lJumpCol, lJumpRow)*2;
	LONG lNearX = 0, lNearY = 0;
	while( 1 )
	{
		if( OneDieIndexGetDieValidPosn(lTgtRow, lTgtCol, lLoop, lNearX, lNearY, FALSE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByScanNearby = TRUE;
			break;
		}
		Sleep(10);
	
		if( pUtl->GetPrescanMapIndexIdle() && 
			m_lPrescanLastGrabRow==lOldRow && 
			m_lPrescanLastGrabCol==lOldCol && 
			m_bPrescanLastGrabbed==TRUE  )
		{
			Sleep(10);
			break;
		}
		lCounter++;
		if( lCounter>1000 ) // > 0.5 seconds
		{
			break;
		}
	}

	lLoop = max(lJumpCol, lJumpRow)*2;
	if( bByScanNearby==FALSE )
	{
		if( GetDieValidPrescanPosn(lTgtRow, lTgtCol, lLoop, lNearX, lNearY, FALSE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByScanNearby = TRUE;
			lCounter = -1;
		}

		if( bByScanNearby==FALSE )
		{
			LONG lDiePitchX_X	= GetDiePitchX_X();
			LONG lDiePitchX_Y	= GetDiePitchX_Y();
			LONG lDiePitchY_Y	= GetDiePitchY_Y();
			LONG lDiePitchY_X	= GetDiePitchY_X();
			LONG lDiffCol = lTgtCol - lOldCol;
			LONG lDiffRow = lTgtRow - lOldRow;

			lTgtWfX = lOldWfX - (lDiffCol) * lDiePitchX_X - (lDiffRow) * lDiePitchY_X;
			lTgtWfY = lOldWfY - (lDiffRow) * lDiePitchY_Y - (lDiffCol) * lDiePitchX_Y;
		}
	}

	LONG lHmiRow = 0, lHmiCol = 0;
	ConvertAsmToHmiUser(lTgtRow, lTgtCol, lHmiRow, lHmiCol);
	if( bByScanNearby )
	{
		szText.Format("wt map, %5ld,%5ld,%5ld,%5ld,%8ld,%8ld, %ld", 
			lTgtRow, lTgtCol, lHmiRow, lHmiCol, lTgtWfX, lTgtWfY, lCounter);
	}
	else
	{
		szText.Format("no die, %5ld,%5ld,%5ld,%5ld,%8ld,%8ld, %ld", 
			lTgtRow, lTgtCol, lHmiRow, lHmiCol, lTgtWfX, lTgtWfY, lCounter);
	}
	pUtl->PrescanMoveLog(szText);

	m_nPrescanNextMapRow	= lTgtRow;
	m_nPrescanNextMapCol	= lTgtCol;
	m_nPrescanNextWftPosnX	= lTgtWfX;
	m_nPrescanNextWftPosnY	= lTgtWfY;

	return FALSE;
}	//	4.24TX 4

LONG CWaferTable::SetPrescanMapIndexStep(IPC_CServiceMessage& svMsg)
{
	DOUBLE dRowTol = 0, dColTol = 0;
	GetMapIndexTolerance(dRowTol, dColTol);
	if( m_nPrescanIndexStepRow>(LONG)(m_dPrescanLFSizeY-dRowTol) )
		m_nPrescanIndexStepRow = (LONG)(m_dPrescanLFSizeY-dRowTol);
	if( m_nPrescanIndexStepCol>(LONG)(m_dPrescanLFSizeX-dColTol) )
		m_nPrescanIndexStepCol = (LONG)(m_dPrescanLFSizeX-dColTol);

	if( m_nPrescanIndexStepCol<1 )
		m_nPrescanIndexStepCol = 1;
	if( m_nPrescanIndexStepRow<1 )
		m_nPrescanIndexStepRow = 1;

	SaveData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//	427TX	4
BOOL CWaferTable::AF_GridSampleFocusLevel()
{
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	pSRInfo->InitRegionFocus();
	for(LONG lRow = 0; lRow < (LONG)pSRInfo->GetSubRows(); lRow++)
	{
		for(LONG lCol = 0; lCol < (LONG)pSRInfo->GetSubCols(); lCol++)
		{
			ULONG i, ulULRow = 0, ulULCol = 0, ulLRRow = 0, ulLRCol = 0;
			LONG  lTLLevel, lTRLevel, lBLLevel, lBRLevel;
			i = lRow * pSRInfo->GetSubCols() + lCol + 1;
			if( pSRInfo->GetRegion(i, ulULRow, ulULCol, ulLRRow, ulLRCol)==FALSE )
				continue;
			szMsg.Format("AF smp, %5lu,%5lu,%5lu,%5lu,%5lu", i, ulULRow, ulULCol, ulLRRow, ulLRCol);
			pUtl->PrescanMoveLog(szMsg);
			pSRInfo->GetRegionFocus(i, lTLLevel, lTRLevel, lBLLevel, lBRLevel);
			if( lTLLevel==WT_AF_LEVEL_INIT )	// never checked before
			{
				if( AF_DetectTargetFocusLevel(ulULRow, ulULCol) )	// to do auto focus
				{
					lTLLevel = (*m_psmfSRam)["WPR Auto Focus"]["Sample Level"];
				}
				else
				{
					return FALSE;
				}
			}
			if( lTRLevel==WT_AF_LEVEL_INIT )	// never checked before
			{
				if( AF_DetectTargetFocusLevel(ulULRow, ulLRCol) )	// to do auto focus
				{
					lTRLevel = (*m_psmfSRam)["WPR Auto Focus"]["Sample Level"];
				}
				else
				{
					return FALSE;
				}
			}
			if( lBLLevel==WT_AF_LEVEL_INIT )	// never checked before
			{
				if( AF_DetectTargetFocusLevel(ulLRRow, ulULCol) )	// to do auto focus
				{
					lBLLevel = (*m_psmfSRam)["WPR Auto Focus"]["Sample Level"];
				}
				else
				{
					return FALSE;
				}
			}
			if( lBRLevel==WT_AF_LEVEL_INIT )	// never checked before
			{
				if( AF_DetectTargetFocusLevel(ulLRRow, ulLRCol) )	// to do auto focus
				{
					lBRLevel = (*m_psmfSRam)["WPR Auto Focus"]["Sample Level"];
				}
				else
				{
					return FALSE;
				}
			}
			pSRInfo->SetRegionFocusTL(i, lTLLevel);
			pSRInfo->SetRegionFocusTR(i, lTRLevel);
			pSRInfo->SetRegionFocusBL(i, lBLLevel);
			pSRInfo->SetRegionFocusBR(i, lBRLevel);
			if (lCol < (LONG)pSRInfo->GetSubCols() - 1)
			{
				pSRInfo->SetRegionFocusTL(i+1, lTRLevel);
				pSRInfo->SetRegionFocusBL(i+1, lBRLevel);
			}
			if (lRow < (LONG)pSRInfo->GetSubRows() - 1)
			{
				pSRInfo->SetRegionFocusTL(i + pSRInfo->GetSubCols(), lBLLevel);
				pSRInfo->SetRegionFocusTR(i + pSRInfo->GetSubCols(), lBRLevel);
			}
			if (lCol< ((LONG)pSRInfo->GetSubCols() - 1) && lRow< ((LONG)pSRInfo->GetSubRows() - 1))
			{
				pSRInfo->SetRegionFocusTL(i + 1 + pSRInfo->GetSubCols(), lBRLevel);
			}
			if( pApp->IsStopAlign() )
			{
				SetAlignmentStatus(FALSE);
				HmiMessage_Red_Back("Focus Level sampling operation stopped, please align wafer again!");
				return FALSE;
			}
		}
	}

	pUtl->PrescanMoveLog("\n");
	// IF NOT ASSIGNED, FIND THE NEAREST VALUE AND UPDATE
	return TRUE;
}

BOOL CWaferTable::IsDieInMapArea(LONG ulRow, LONG ulCol, DOUBLE dRatio)
{	//	427TX	5
	if( GetMapValidMaxRow()<=GetMapValidMinRow() )
		return FALSE;

	if( GetMapValidMaxCol()<=GetMapValidMinCol() )
		return FALSE;

	ULONG ulRowSpan = (GetMapValidMaxRow() - GetMapValidMinRow());
	ULONG ulColSpan = (GetMapValidMaxCol() - GetMapValidMinCol());
	DOUBLE dCtrRow  = (GetMapValidMinRow() + GetMapValidMaxRow())/2;
	DOUBLE dCtrCol  = (GetMapValidMinCol() + GetMapValidMaxCol())/2;

	if( m_ucPrescanMapShapeType == WT_MAP_SHAPE_CIRCLE )
	{
		DOUBLE dRadius = max(ulRowSpan, ulColSpan)/2.0;

		DOUBLE dDist = 0;
		if( ulRowSpan>ulColSpan )
		{
			dDist = sqrt( pow(ulRow-dCtrRow, 2.0) + pow((ulCol-dCtrCol)*ulRowSpan/ulColSpan, 2.0) );
		}
		else
		{
			dDist = sqrt( pow((ulRow-dCtrRow)*ulColSpan/ulRowSpan, 2.0) + pow(ulCol-dCtrCol, 2.0) );
		}
		if( dDist>=(dRadius*dRatio) )
		{
			return FALSE;
		}
	}
	else
	{
		if( ulRow>=(dCtrRow+ulRowSpan/2*dRatio) ||
			ulRow<=(dCtrRow-ulRowSpan/2*dRatio) )
		{
			return FALSE;
		}

		if( ulCol>=(dCtrCol+ulColSpan/2*dRatio) ||
			ulCol<=(dCtrCol-ulColSpan/2*dRatio) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CWaferTable::AF_DetectTargetFocusLevel(ULONG ulRow, ULONG ulCol)
{
	if( IsWprWithAF() )
	{
		if( IsDieInMapArea(ulRow, ulCol, 0.9)==FALSE )
		{
			(*m_psmfSRam)["WPR Auto Focus"]["Sample Level"] = WT_AF_LEVEL_FAIL;
			return TRUE;
		}
		m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
		LONG lDiePitchX_X = GetDiePitchX_X(); 
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();
		LONG lDiePitchY_Y = GetDiePitchY_Y();
		LONG lDiff_X = (LONG)ulCol - m_nPrescanAlignMapCol;
		LONG lDiff_Y = (LONG)ulRow - m_nPrescanAlignMapRow;
		LONG lCurrentX = GetPrescanAlignPosnX() - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		LONG lCurrentY = GetPrescanAlignPosnY() - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
		if( XY_SafeMoveTo(lCurrentX, lCurrentY)==FALSE )
		{
			(*m_psmfSRam)["WPR Auto Focus"]["Sample Level"] = WT_AF_LEVEL_FAIL;
			return TRUE;
		}

		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		BOOL bReturn = TRUE;
		INT nConvID;
		IPC_CServiceMessage stMsg;

		BOOL bByHmi = -1;
		stMsg.InitMessage(sizeof(BOOL), &bByHmi);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "AutoFocus", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

		if( bReturn==FALSE )
			return FALSE;

		CString szMsg;
		LONG lFocusLevel = (*m_psmfSRam)["WPR Auto Focus"]["Sample Level"];
		szMsg.Format("AF mve,      ,     ,     ,%5lu,%5lu,%8ld,%8ld,%8ld", ulRow, ulCol, lCurrentX, lCurrentY, lFocusLevel);
		pUtl->PrescanMoveLog(szMsg);
		return TRUE;
	}

	return FALSE;
}

BOOL CWaferTable::AF_PredicateNextScanFocusLevel(ULONG ulRow, ULONG ulCol)
{
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	ULONG ulRegionNo = pSRInfo->GetWithinRegionNo(ulRow, ulCol);

	if( pSRInfo->IsInvalidRegion(ulRegionNo) )
		return FALSE;

	ULONG ulTopRow = 0, ulLeftCol = 0, ulBtmRow = 0, ulRightCol = 0;
	LONG  lTLLevel, lTRLevel, lBLLevel, lBRLevel;
	if( pSRInfo->GetRegion(ulRegionNo, ulTopRow, ulLeftCol, ulBtmRow, ulRightCol)==FALSE )
		return FALSE;
	pSRInfo->GetRegionFocus(ulRegionNo, lTLLevel, lTRLevel, lBLLevel, lBRLevel);

	if( ulLeftCol==ulRightCol || ulTopRow==ulBtmRow )
		return FALSE;

	BOOL bTLBad = (lTLLevel==WT_AF_LEVEL_INIT || lTLLevel==WT_AF_LEVEL_FAIL);
	BOOL bTRBad = (lTRLevel==WT_AF_LEVEL_INIT || lTRLevel==WT_AF_LEVEL_FAIL);
	BOOL bBLBad = (lBLLevel==WT_AF_LEVEL_INIT || lBLLevel==WT_AF_LEVEL_FAIL);
	BOOL bBRBad = (lBRLevel==WT_AF_LEVEL_INIT || lBRLevel==WT_AF_LEVEL_FAIL);
	if( bTLBad && bTRBad && bBLBad && bBRBad )
		return FALSE;

	if( bTLBad )
	{
		if( bTRBad==FALSE )
			lTLLevel = lTRLevel;
		else if( bBLBad==FALSE )
			lTLLevel = lBLLevel;
		else
			lTLLevel = lBRLevel;
	}
	if( bTRBad )
	{
		if( bTLBad==FALSE )
			lTRLevel = lTLLevel;
		else if( bBRBad==FALSE )
			lTRLevel = lBRLevel;
		else
			lTRLevel = lBLLevel;
	}
	if( bBLBad )
	{
		if( bTLBad==FALSE )
			lBLLevel = lTLLevel;
		else if( bBRBad==FALSE )
			lBLLevel = lBRLevel;
		else
			lBLLevel = lTRLevel;
	}
	if( bBRBad )
	{
		if( bTRBad==FALSE )
			lBRLevel = lTRLevel;
		else if( bBLBad==FALSE )
			lBRLevel = lBLLevel;
		else
			lBRLevel = lTLLevel;
	}
	LONG lTopLevel = ((ulRightCol-ulCol)*lTLLevel + (ulCol-ulLeftCol)*lTRLevel)/(ulRightCol-ulLeftCol);
	LONG lBtmLevel = ((ulRightCol-ulCol)*lBLLevel + (ulCol-ulLeftCol)*lBRLevel)/(ulRightCol-ulLeftCol);

	LONG lTgtLevel = ((ulBtmRow-ulRow)*lTopLevel + (ulRow-ulTopRow)*lBtmLevel)/(ulBtmRow-ulTopRow);

	(*m_psmfSRam)["WPR Auto Focus"]["Sample Level"] = lTgtLevel;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMsg;
	szMsg.Format("AF pdc, %5lu,%5lu,%8ld,%8ld,==>%8ld", 
		ulRow, ulCol, GetScanNextPosnX(), GetScanNextPosnY(), lTgtLevel);
	pUtl->PrescanMoveLog(szMsg);

	return TRUE;
}

//	427TX	4
INT CWaferTable::ES101ToAlignPosition()
{
	if( XY_IsPowerOff() )
	{
		HmiMessage_Red_Back("Unload Wafer Table 1 XY not power ON!");
		return gnNOTOK;
	}
	if( IsWL1ExpanderSafeToMove() == FALSE || IsWL2ExpanderSafeToMove() == FALSE )
	{
		return gnNOTOK;
	}
	// Move to unload
	BOOL bMove = TRUE;
	if( IsWT1UnderCamera() )
	{
		bMove = FALSE;
	}

	if( bMove )
	{
		m_lBackLightZStatus = 2;
		MoveES101BackLightZUpDn(FALSE);

		if (MoveEjectorElevatorToSafeLevel() == FALSE)
		{
			return gnNOTOK;
		}

		//v4.28	//Must check expander status before moving table to HOME position!!
		if (IsWLExpanderOpen() == TRUE)
		{
			SetErrorMessage("WT:ES101ToAlignPosition() fails because expander is OPEN");
			return gnNOTOK;
		}

		LONG lCurrentX, lCurrentY;
		WFTTGoHomeUnderCam(TRUE);
		X2_Profile(LOW_PROF1);
		Y2_Profile(LOW_PROF1);
		XY2_MoveTo(m_lUnloadPhyPosX2, m_lUnloadPhyPosY2);
		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);

		GetHomeDiePhyPosn(lCurrentX, lCurrentY);
		X1_Profile(LOW_PROF1);
		Y1_Profile(LOW_PROF1);
		XY1_MoveTo(lCurrentX, lCurrentY);
		X1_Profile(NORMAL_PROF);
		Y1_Profile(NORMAL_PROF);
		RotateWFTTUnderCam(FALSE);

		if( XY_IsPowerOff() )
		{
			HmiMessage_Red_Back("Preload Wafer Table 2 XY not power ON!");
			return gnNOTOK;
		}
	}

	m_lBackLightZStatus = 0;
	MoveES101BackLightZUpDn(TRUE);
	WFT_MoveToFocusLevel();

	return gnOK;
}	//	4.24TX 4

BOOL CWaferTable::WFT_MoveToFocusLevel()
{
	BOOL bWft = FALSE;
	if( IsWT1UnderCamera() )
	{
		bWft = FALSE;
	}
	if( IsWT2UnderCamera() )
	{
		bWft = TRUE;
	}

	MoveFocusToWafer(bWft);

	return TRUE;
}

LONG CWaferTable::SetDummyPrescanArea(IPC_CServiceMessage& svMsg)
{
	if( IsESDualWT() )
	{
		HmiMessage("Dual WFT ES101 can not use this option!", "Prescan");
		return 1;
	}

	CString szMsg;
	LONG lTmp = 0;
	LONG lUpLeftX = m_lScanAreaUpLeftX;
	LONG lUpLeftY = m_lScanAreaUpLeftY;
	if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
	{
		szMsg = "Do you want to move to circular wafer center?";
		lTmp = HmiMessageEx(szMsg, "prescan",
			glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300,0,0,0,0, 300, 300);
		//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szMsg);		//v4.43T10
		if( lTmp==glHMI_YES )
		{
			lUpLeftX = GetWaferCenterX();
			lUpLeftY = GetWaferCenterY();
		}
	}

	SetJoystickOn(FALSE);
	XY_SafeMoveTo(lUpLeftX, lUpLeftY);
	SetJoystickOn(TRUE);
	if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
		szMsg = "Please move to circular wafer center with joystick!";
	else
		szMsg = "Please move to up-left scan corner with joystick!";
	lTmp = HmiMessageEx(szMsg, "prescan",
		glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300,0,0,0,0, 300, 300);
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szMsg);		//v4.43T10
	GetEncoderValue();
	if( lTmp==glHMI_CANCEL )
	{
		return 1;
	}

	lUpLeftX = GetCurrX();
	lUpLeftY = GetCurrY();

	SetJoystickOn(FALSE);
	XY_SafeMoveTo(m_lScanAreaLowRightX, m_lScanAreaLowRightY);
	SetJoystickOn(TRUE);
	if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
		szMsg = "Please move to circular wafer edge with joystick!";
	else
		szMsg = "Please move to low-right scan corner with joystick!";
	lTmp = HmiMessageEx(szMsg, "prescan",
		glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300,0,0,0,0, 300, 300);
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szMsg);		//v4.43T10
	GetEncoderValue();
	if( lTmp==glHMI_CANCEL )
	{
		return 1;
	}

	m_lScanAreaUpLeftX		= lUpLeftX;
	m_lScanAreaUpLeftY		= lUpLeftY;
	m_lScanAreaLowRightX	= GetCurrX();
	m_lScanAreaLowRightY	= GetCurrY();

	SaveData();

	LONG lCtrX = 0;
	if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
		lCtrX = m_lScanAreaUpLeftX;
	else
		lCtrX = (m_lScanAreaUpLeftX+m_lScanAreaLowRightX)/2;
	LONG lCtrY = 0;
	if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
		lCtrY = m_lScanAreaUpLeftY;
	else
		lCtrY = (m_lScanAreaUpLeftY + m_lScanAreaLowRightY)/2;
	XY_SafeMoveTo(lCtrX, lCtrY);

	return 1;
}

BOOL CWaferTable::UpdateScanYield()
{
	CStringMapFile smfScanYield;
	LONG lTotalWafers = 0, lWaferIndex = 0;
	BOOL bFileExist = TRUE;
	FILE *fp, *fpName;

	//Check file exist 
	if ((_access(MSD_SCAN_YIELD_INFO_FILE, 0 )) == -1)
	{
		bFileExist = FALSE;
	}
	else
	{
		if ( smfScanYield.Open(MSD_SCAN_YIELD_INFO_FILE, FALSE, FALSE) != 1 )
		{
			CMSFileUtility::Instance()->LogFileError("Cannot Open File: " + MSD_SCAN_YIELD_INFO_FILE);
			bFileExist = FALSE;
		}
		else
		{
			lTotalWafers	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][MSD_SCAN_YIELD_TOTAL_WAFERS];
			lWaferIndex		= (smfScanYield)[MSD_SCAN_YIELD_HEAD][MSD_SCAN_YIELD_WAFER_INDEX];
		}
	}

	LONG lSpinIndex = labs(m_lScanYieldSpinIndex-1) * 10;
	if( lSpinIndex>lWaferIndex )
		m_lScanYieldSpinIndex = 0 - lWaferIndex/10;
	lSpinIndex = labs(m_lScanYieldSpinIndex) * 10;

	errno_t nErr = fopen_s(&fp, MSD_SCAN_YIELD_HMI_FILE, "w");
	errno_t nErr1 = fopen_s(&fpName, MSD_SCAN_WAFER_ID_FILE, "w");
	if ((bFileExist == TRUE) && (nErr == 0) && (nErr1 == 0) && (fp != NULL) && (fpName != NULL))
	{
		CString szRecd, szName, szTime, szWft, szMgzn, szSlot, szData;
		LONG j = 0;
		LONG lStartIndex = lWaferIndex - lSpinIndex;
		LONG lEndIndex = 1;
		if( lStartIndex>10 )
			lEndIndex = lStartIndex - 10 + 1;
		for(LONG i=lStartIndex; i>=lEndIndex; i--)
		{
			szRecd.Format("%d", lTotalWafers - lSpinIndex - j);
			szName	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_NAME];
			szTime	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_TIME];
			szWft	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_WFTNO];
			szMgzn	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_MGZNNO];
			szSlot	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_SLOTNO];
			szData	= (smfScanYield)[MSD_SCAN_YIELD_HEAD][i][MSD_SCAN_YIELD_VALUE];
			fprintf(fp, "%s,%s,%s,%s,%s,%s,%s,\n", 
				(LPCTSTR)szRecd, (LPCTSTR)szName, (LPCTSTR)szTime, (LPCTSTR)szWft, (LPCTSTR)szMgzn, (LPCTSTR)szSlot, (LPCTSTR)szData);
			fprintf(fpName, "%s,", (LPCTSTR)szName);
			if( j==0 )
			{
				m_szScanYieldWaferName = szName;
			}
			j++;
		}
		smfScanYield.Close();
	}

	if (fp != NULL)
	{
		fclose(fp);
	}

	if (fpName != NULL)
	{
		fclose(fpName);
	}

	return TRUE;
}

LONG CWaferTable::SpinScanYieldDisplay(IPC_CServiceMessage& svMsg)
{
	LONG lScanYieldIndex = 0;
	svMsg.GetMsg(sizeof(LONG), &lScanYieldIndex);

	m_lScanYieldSpinIndex = lScanYieldIndex;
	UpdateScanYield();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;	
}

LONG CWaferTable::ShowScanYieldResult(IPC_CServiceMessage& svMsg)
{
	m_lScanYieldSpinIndex = 0;
	UpdateScanYield();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;	
}

LONG CWaferTable::ClearScanYieldResult(IPC_CServiceMessage& svMsg)
{
	if( HmiMessage("Are you sure to clear all Scan Results?", "Prescan Yield", glHMI_MBX_YESNO)==glHMI_YES )
	{
		remove(MSD_SCAN_YIELD_INFO_FILE);
		m_lScanYieldSpinIndex = 0;
		UpdateScanYield();
		CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "ScanTimeLog.csv";
		remove(szFileName);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;	
}

LONG CWaferTable::UploadSelectedWaferFile(IPC_CServiceMessage& svMsg)
{
	return 1;
}	// upload selected scan files to server for AOI machine

LONG CWaferTable::PassWLCurrentSlotNo(IPC_CServiceMessage& svMsg)
{
	LONG lCurrentSlotNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lCurrentSlotNo);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;	
}

BOOL CWaferTable::IsScanPosnOK()
{
	return m_bPrescanPosnOK;
}

BOOL CWaferTable::IsMapPosnOK()
{
	return m_bMapPositionOK;
}

BOOL CWaferTable::WftMoveBondArmToSafe(BOOL bToSafe)
{
	IPC_CServiceMessage stMsg;
	int nConvID;
	SetEjectorVacuum(FALSE);	Sleep(50);
	Sleep(50);
	stMsg.InitMessage(sizeof(BOOL), &bToSafe);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_AutoMoveToPrePick", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 500000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return TRUE;
}

BOOL CWaferTable::IsScanFrameInWafer(LONG lFrameRow, LONG lFrameCol)
{
	if( lFrameRow < SCAN_MATRIX_MIN_ROW || lFrameRow >= SCAN_MAX_MATRIX_ROW )
	{
		return FALSE;
	}

	if( lFrameCol < SCAN_MATRIX_MIN_COL || lFrameCol >= SCAN_MAX_MATRIX_COL )
	{
		return FALSE;
	}

	if( m_baScanMatrix[lFrameRow][lFrameCol] == FALSE )
	{
		return FALSE;
	}

	if( GetNewPickCount()>0 )
	{
		return TRUE;
	}	//	always for rescan

	LONG lFrameX = m_laScanPosnX[lFrameRow][lFrameCol];
	LONG lFrameY = m_laScanPosnY[lFrameRow][lFrameCol];

	if( IsPosnWithinMapRange(lFrameX, lFrameY) && IsWithinWaferLimit(lFrameX, lFrameY) )
		return TRUE;

	return FALSE;
}

BOOL CWaferTable::GetMapScanArea()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	INT lAlnWfX = GetPrescanAlignPosnX();
	INT lAlnWfY = GetPrescanAlignPosnY();

	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	if( abs(lDiePitchX_X)<abs(m_lPrescanDiePitchX_X) )
        lDiePitchX_X = m_lPrescanDiePitchX_X; 
	if( abs(lDiePitchX_Y)<abs(m_lPrescanDiePitchX_Y) )
		lDiePitchX_Y = m_lPrescanDiePitchX_Y;
	if( abs(lDiePitchY_Y)<abs(m_lPrescanDiePitchY_Y) )
		lDiePitchY_Y = m_lPrescanDiePitchY_Y;
	if( abs(lDiePitchY_X)<abs(m_lPrescanDiePitchY_X) )
		lDiePitchY_X = m_lPrescanDiePitchY_X;

	UINT unWafEdgeNum = pApp->GetPrescanWaferEdgeNum();

	LONG lRowNum = GetMapValidMaxRow() - GetMapValidMinRow();
	LONG lColNum = GetMapValidMaxCol() - GetMapValidMinCol();
	LONG lColCtr = (GetMapValidMinCol() + GetMapValidMaxCol())/2;
	LONG lRowCtr = (GetMapValidMinRow() + GetMapValidMaxRow())/2;
	LONG lDistCol = lColCtr - m_nPrescanAlignMapCol;
	LONG lDistRow = lRowCtr - m_nPrescanAlignMapRow;

	m_nPrescanMapCtrX = lAlnWfX - lDistCol * lDiePitchX_X - lDistRow * lDiePitchY_X;
	m_nPrescanMapCtrY = lAlnWfY - lDistRow * lDiePitchY_Y - lDistCol * lDiePitchX_Y;

	INT  nXWidth = (INT) labs(lColNum * lDiePitchX_X);
	INT  nYHeight= (INT) labs(lRowNum * lDiePitchY_Y);

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	if (!pSRInfo->IsManualAlignRegion() && IsScanAlignWafer() )
	{
		nXWidth  = nXWidth * 2;
		nYHeight = nYHeight * 2;
	}

	if( pApp->GetCustomerName()==CTM_SEMITEK && (lRowNum*lColNum)>(150*150) )
	{
		nXWidth = nXWidth*110/100;
		nYHeight= nYHeight*110/100;
	}
	m_nPrescanMapWidth  =  abs(nXWidth) + abs(GetPrescanPitchX())*unWafEdgeNum*2;
	m_nPrescanMapHeight = abs(nYHeight) + abs(GetPrescanPitchY())*unWafEdgeNum*2;
	m_nPrescanMapRadius = max(m_nPrescanMapWidth/2, m_nPrescanMapHeight/2);

	LONG i, j;	// MUST KEEP LONG, IF UNLONG, WOULD DEAD LOOP
	double dOffsetX = 0.0, dOffsetY = 0.0;
	LONG nMaxRadius = 0, nCrnRadius = 0;

	// below to calculate the scan area to compare with above
	for(i=GetMapValidMinRow(); i<=GetMapValidMaxRow(); i++)	// MUST KEEP LONG, IF UNLONG, WOULD DEAD LOOP
	{
		// first valid die in this row
		for(j=GetMapValidMinCol(); j<=GetMapValidMaxCol(); j++)	// MUST KEEP LONG, IF UNLONG, WOULD DEAD LOOP
		{
			if (m_pWaferMapManager->IsMapHaveBin(i, j))
			{
				lDistCol = j - lColCtr;
				lDistRow = i - lRowCtr;
				dOffsetX =  lDistCol * lDiePitchX_X + lDistRow * lDiePitchY_X;
				dOffsetY =  lDistRow * lDiePitchY_Y + lDistCol * lDiePitchX_Y;
				nCrnRadius = abs((int) sqrt(pow(dOffsetX,2.0) + pow(dOffsetY,2.0)));
				if( nCrnRadius>nMaxRadius )
					nMaxRadius = nCrnRadius;
				break;
			}
		}

		// last valid die in this row
		for(j=GetMapValidMaxCol(); j>=GetMapValidMinCol(); j--)
		{
			if (m_pWaferMapManager->IsMapHaveBin(i, j))
			{
				lDistCol = j - lColCtr;
				lDistRow = i - lRowCtr;
				dOffsetX =  lDistCol * lDiePitchX_X + lDistRow * lDiePitchY_X;
				dOffsetY =  lDistRow * lDiePitchY_Y + lDistCol * lDiePitchX_Y;
				nCrnRadius = abs((int) sqrt(pow(dOffsetX,2.0) + pow(dOffsetY,2.0)));
				if( nCrnRadius>nMaxRadius )
					nMaxRadius = nCrnRadius;
				break;
			}
		}
	}

	dOffsetX = GetPrescanPitchX()*unWafEdgeNum;
	dOffsetY = GetPrescanPitchY()*unWafEdgeNum;
	int nEdgeCheck = abs((int) sqrt(pow(dOffsetX, 2.0) + pow(dOffsetY, 2.0)));
	nCrnRadius = abs(nMaxRadius) + nEdgeCheck;
	if( (nCrnRadius)>(m_nPrescanMapRadius) )
		m_nPrescanMapRadius = nCrnRadius;
	if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
	{
		m_nPrescanMapWidth  = m_nPrescanMapRadius*2;
		m_nPrescanMapHeight = m_nPrescanMapRadius*2;
	}

	return TRUE;
}

LONG CWaferTable::GetScanNextPosnX()
{
	return m_nPrescanNextWftPosnX;
}

LONG CWaferTable::GetScanNextPosnY()
{
	return m_nPrescanNextWftPosnY;
}

BOOL CWaferTable::OpGetScanNextFrame(CONST SHORT sDown, CONST LONG lLastRow, CONST LONG lLastCol, LONG &lNextRow, LONG &lNextCol)
{
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	LONG lNextIndexRow = lLastRow;
	LONG lNextIndexCol = lLastCol;
	BOOL bGetNext = FALSE;
	switch( m_ucRunScanWalkTour )
	{
	case WT_SCAN_WALK_LEFT_VERT:
	case WT_SCAN_WALK_HOME_VERT:	// Aln Vert, from aln to right, then from aln to left
		while( 1 )
		{
			switch( m_nPrescanDirection )
			{
			case 1:	// from left to right
				lNextIndexRow++;
				break;
			case 2:
				lNextIndexRow--;
				break;
			}
			if( lNextIndexRow>=SCAN_MAX_MATRIX_ROW || lNextIndexRow<0 )
			{
				break;
			}
			else
			{
				if( IsScanFrameInWafer(lNextIndexRow, lNextIndexCol) )
				{
					bGetNext = TRUE;
					break;
				}
			}
		}

		while( bGetNext==FALSE )
		{
			lNextIndexCol += sDown;
			if( lNextIndexCol<SCAN_MATRIX_MIN_COL || lNextIndexCol>=SCAN_MAX_MATRIX_COL )
			{
				break;
			}

			switch( m_nPrescanDirection )
			{
			case 1:	// left to right
				m_nPrescanDirection = 2;
				for(lNextIndexRow=SCAN_MAX_MATRIX_ROW-1; lNextIndexRow>=SCAN_MATRIX_MIN_ROW; lNextIndexRow--)
				{
					if( IsScanFrameInWafer(lNextIndexRow, lNextIndexCol) )
					{
						bGetNext = TRUE;
						m_nPrescanDirection = 2;
						break;
					}
				}
				break;
			case 2:
				m_nPrescanDirection = 1;
				for(lNextIndexRow=SCAN_MATRIX_MIN_ROW; lNextIndexRow<SCAN_MAX_MATRIX_ROW; lNextIndexRow++)
				{
					if( IsScanFrameInWafer(lNextIndexRow, lNextIndexCol) )
					{
						bGetNext = TRUE;
						break;
					}
				}
				break;
			}
			if( bGetNext )
			{
				break;
			}
		}
		break;
	case WT_SCAN_WALK_TOP_HORI:	// top down horizontally
	case WT_SCAN_WALK_HOME_HORI:	// from aln to bottom, left to right; then from aln to top, right to left.
	default:
		while( 1 )
		{
			switch( m_nPrescanDirection )
			{
			case 1:	// from left to right
				lNextIndexCol++;
				break;
			case 2:
				lNextIndexCol--;
				break;
			}
			if( lNextIndexCol>=SCAN_MAX_MATRIX_COL || lNextIndexCol<SCAN_MATRIX_MIN_COL )
			{
				break;
			}
			else
			{
				if( IsScanFrameInWafer(lNextIndexRow, lNextIndexCol) )
				{
					bGetNext = TRUE;
					break;
				}
			}
		}

		while( bGetNext==FALSE )
		{
			lNextIndexRow += sDown;
			if( lNextIndexRow<SCAN_MATRIX_MIN_ROW || lNextIndexRow>=SCAN_MAX_MATRIX_ROW )
			{
				break;
			}

			switch( m_nPrescanDirection )
			{
			case 1:	// left to right
				m_nPrescanDirection = 2;
				for(lNextIndexCol=SCAN_MAX_MATRIX_COL-1; lNextIndexCol>=SCAN_MATRIX_MIN_COL; lNextIndexCol--)
				{
					if( IsScanFrameInWafer(lNextIndexRow, lNextIndexCol) )
					{
						bGetNext = TRUE;
						break;
					}
				}
				break;
			case 2:
				m_nPrescanDirection = 1;
				for(lNextIndexCol=SCAN_MATRIX_MIN_COL; lNextIndexCol<SCAN_MAX_MATRIX_COL; lNextIndexCol++)
				{
					if( IsScanFrameInWafer(lNextIndexRow, lNextIndexCol) )
					{
						bGetNext = TRUE;
						break;
					}
				}
				break;
			}
			if( bGetNext )
			{
				break;
			}
		}
		break;
	}

	lNextRow = lNextIndexRow;
	lNextCol = lNextIndexCol;

	return bGetNext;
}


VOID CWaferTable::LogScanDetailTime(CString szLogMsg)
{
	// new file for sequencial loading
	CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "ScanTimeLog.csv";
	CStdioFile fLogFile;
	bool bAddHead = false;
	if (fLogFile.Open(szFileName, CFile::modeRead) == FALSE)
	{
		bAddHead = true;
	}
	else
	{
		fLogFile.Close();
	}
	

	FILE *pfFile = NULL;
	errno_t nErr = fopen_s(&pfFile, szFileName, "a+");
	if ((nErr == 0) && (pfFile != NULL))
	{
		if( bAddHead )
		{						//	01.		02.		03.		04.			05.			06.			07.			08.			09.
			fprintf(pfFile, "Wafer ID,Scan Start,BL time,Scan Grab Time,PR HouseKeeping,PR End Lot,Build Map,Update Map,Pr Summary,");
								//	10.			11.		12.			13.			14.			15.			16.			17.		18.
			fprintf(pfFile, "Save MapPsmScn,AF Prestart,AF Learn,Find GT,Auto Learn Wafer,Find Home,IM ChangeWafer,OverHead Time,Total Time,");
								//	19.	20.		21.		22.	23.		24	25
			fprintf(pfFile, "Frames Num,BadCut,Defect,Empty,Good,Total,Base Num,\n");
		}
		fprintf(pfFile, "%s", (LPCTSTR)szLogMsg);
		fclose(pfFile);
	}
}

BOOL CWaferTable::OpGetScanPosn_MapIndexFastHome(const short sDown)
{
	LONG	lTgtWfX = 0, lTgtWfY = 0;
	LONG	lTgtRow = 0, lTgtCol = 0;
	UCHAR	ucNullBin = m_WaferMapWrapper.GetNullBin();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( m_bPrescan2ndTimeStart==FALSE )	// to index and find home die by spiral when prescan
	{
		return  PrescanOpGetPosn_SprialIndex();	//	4.24TX 4
	}

	LONG lOldWfX = GetScanLastPosnX();
	LONG lOldWfY = GetScanLastPosnY();
	LONG lOldRow = m_nPrescanLastMapRow;
	LONG lOldCol = m_nPrescanLastMapCol;

	LONG lMinRow = GetMapValidMinRow();
	LONG lMaxRow = GetMapValidMaxRow();
	LONG lMinCol = GetMapValidMinCol();
	LONG lMaxCol = GetMapValidMaxCol();
	if( pUtl->GetPrescanDummyMap() )
	{
		lMinRow = m_lDummyMapMaxRow + m_lFastHomeIndexOffsetRow;
		lMaxRow = m_lDummyMapMaxRow + m_lFastHomeIndexOffsetRow + m_lDummyMapMaxRow;
		lMinCol = m_lDummyMapMaxCol + m_lFastHomeIndexOffsetCol;
		lMaxCol = m_lDummyMapMaxCol + m_lFastHomeIndexOffsetCol + m_lDummyMapMaxCol;
	}
	LONG LCTRROW = (lMinRow + lMaxRow)/2;
	LONG LCTRCOL = (lMinCol + lMaxCol)/2;

	CString szText;
	szText.Format("wt old, %5ld,%5ld,  hme,  wft,%8ld,%8ld", lOldRow, lOldCol, lOldWfX, lOldWfY);
	pUtl->PrescanMoveLog(szText);

	BOOL bHoriIndex = TRUE;
	if( m_ucScanWalkTour==WT_SCAN_WALK_LEFT_VERT || m_ucScanWalkTour==WT_SCAN_WALK_HOME_VERT )
	{
		bHoriIndex = FALSE;
	}
	LONG lScnX = 0, lScnY = 0;
	LONG lStepCol = GetMapIndexStepCol();
	LONG lStepRow = GetMapIndexStepRow();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( m_bDualWayTipWafer )
	{
		bHoriIndex = TRUE;
		lStepCol = 2;
	}
	BOOL bHasDie = FALSE;
	// move map to right, table to left
	LONG	lDiffX = 0, lDiffY = 0;
	LONG lDiePitchX_X	= GetDiePitchX_X();
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	while( 1 )
	{
		if( bHoriIndex )
		{
			if( m_nPrescanDirection==1 )
			{
				lDiffX += lStepCol;
			}
			else
			{
				lDiffX -= lStepCol;
			}
			lTgtRow = lOldRow + lDiffY;
			lTgtCol = lOldCol + lDiffX;

			BOOL bOutLimit = FALSE;

			if( (m_nPrescanDirection==1) )
			{
				if( pUtl->GetPrescanDummyMap() && lTgtCol>LCTRCOL )
				{
					lTgtWfX = lOldWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
					lTgtWfY = lOldWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
					if( IsWithinWaferLimit(lTgtWfX, lTgtWfY, 1.2)==FALSE )
					{
						bOutLimit = TRUE;
					}
				}
				BOOL bChangeLine = lTgtCol>(lMaxCol + (lStepCol+1)/2);
				if( bChangeLine || bOutLimit )
				{
					m_nPrescanDirection = 2;
					lDiffY += sDown*lStepRow;
				}
			}
			else
			{
				if( pUtl->GetPrescanDummyMap() && lTgtCol<LCTRCOL )
				{
					lTgtWfX = lOldWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
					lTgtWfY = lOldWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
					if( IsWithinWaferLimit(lTgtWfX, lTgtWfY, 1.2)==FALSE )
					{
						bOutLimit = TRUE;
					}
				}
				BOOL bChangeLine = lTgtCol<(lMinCol - (lStepCol+1)/2);
				if( bChangeLine || bOutLimit )
				{
					m_nPrescanDirection = 1;
					lDiffY += sDown*lStepRow;
				}
			}

			if( bOutLimit )
				lTgtRow = lOldRow + lDiffY;
			if( lTgtRow>(lMaxRow+(lStepRow+1)/2) || lTgtRow<(lMinRow-(lStepRow+1)/2) )
			{
				szText.Format("wt LMT, %5ld,%5ld,%5ld,%5ld", lTgtRow, lMaxRow, lMinRow, (lStepRow+1)/2);
				pUtl->PrescanMoveLog(szText);
				bHasDie = FALSE;
				break;
			}
		}
		else
		{
			if( m_nPrescanDirection==1 )
			{
				lDiffY += lStepRow;
			}
			else
			{
				lDiffY -= lStepRow;
			}

			lTgtRow = lOldRow + lDiffY;
			lTgtCol = lOldCol + lDiffX;

			BOOL bOutLimit = FALSE;

			if( (m_nPrescanDirection==1) )
			{
				if( pUtl->GetPrescanDummyMap() && lTgtRow>LCTRROW )
				{
					lTgtWfX = lOldWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
					lTgtWfY = lOldWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
					if( IsWithinWaferLimit(lTgtWfX, lTgtWfY, 1.2)==FALSE )
					{
						bOutLimit = TRUE;
					}
				}
				BOOL bChangeLine = lTgtRow>(lMaxRow+(lStepRow+1)/2);
				if( bChangeLine || bOutLimit )
				{
					m_nPrescanDirection = 2;
					lDiffX += sDown*lStepCol;
				}
			}
			else
			{
				if( pUtl->GetPrescanDummyMap() && lTgtRow<LCTRROW )
				{
					lTgtWfX = lOldWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
					lTgtWfY = lOldWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
					if( IsWithinWaferLimit(lTgtWfX, lTgtWfY, 1.2)==FALSE )
					{
						bOutLimit = TRUE;
					}
				}
				BOOL bChangeLine = lTgtRow<(lMinRow - (lStepRow+1)/2);
				if( bChangeLine || bOutLimit )
				{
					m_nPrescanDirection = 1;
					lDiffX += sDown*lStepCol;
				}
			}

			if( bOutLimit )
				lTgtRow = lOldRow + lDiffY;
			if( (lTgtCol>(lMaxCol+(lStepCol+1)/2)) || (lTgtCol<(lMinCol-(lStepCol+1)/2)) )
			{
				szText.Format("wt LMT, %5ld,%5ld,%5ld,%5ld", lTgtCol, lMaxCol, lMinCol, (lStepCol+1)/2);
				pUtl->PrescanMoveLog(szText);
				bHasDie = FALSE;
				break;
			}
		}

		LONG lHalfRow = lStepRow/2;
		LONG lHalfCol = lStepCol/2;
		for(LONG jRow=(0-lHalfRow); jRow<=(0+lHalfRow); jRow++)
		{
			for(LONG iCol=(0-lHalfCol); iCol<=(0+lHalfCol); iCol++)
			{
				LONG lChkRow = jRow+lTgtRow;
				LONG lChkCol = iCol+lTgtCol;
				if( (lChkRow<lMinRow) || (lChkCol<lMinCol) || (lChkRow>lMaxRow) || (lChkCol>lMaxCol) )
				{
					continue;
				}
				if( GetMapPhyPosn(lChkRow, lChkCol, lScnX, lScnY) )	// if already scanned, skip to check other dice
				{
					continue;
				}
				if (m_pWaferMapManager->IsMapHaveBin(lChkRow, lChkCol))
				{
					bHasDie = TRUE;
					break;
				}
			}
			if( bHasDie )
			{
				break;
			}
		}

		if( bHasDie )
		{
			break;
		}
	}

	if( bHasDie==FALSE )
	{
		return TRUE;
	}

	CString szNear = "no fh0, ";
	BOOL bByMapNearby = FALSE;
	LONG lNearX = 0, lNearY = 0;
	LONG lLoopCounter = 0;
	LONG lMapIndexLoop = max(lStepCol, lStepRow)*m_ulMapIndexLoop;
	while( 1 )
	{
		if( OneDieIndexGetDieValidPosn(lTgtRow, lTgtCol, lMapIndexLoop, lNearX, lNearY, TRUE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByMapNearby = TRUE;
			szNear = "wt fh1, ";
			break;
		}
		Sleep(10);
		if( pUtl->GetPrescanMapIndexIdle() && 
			m_lPrescanLastGrabRow==lOldRow && 
			m_lPrescanLastGrabCol==lOldCol && 
			m_bPrescanLastGrabbed==TRUE  )
		{
			Sleep(10);
			break;
		}
		lLoopCounter++;
		if( lLoopCounter>1000 ) // > 0.5 seconds
		{
			break;
		}
	}

	if( lLoopCounter<=1000 && bByMapNearby==FALSE )
	{
		if( OneDieIndexGetDieValidPosn(lTgtRow, lTgtCol, lMapIndexLoop, lNearX, lNearY, TRUE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByMapNearby = TRUE;
			szNear = "wt fh2, ";
		}
	}

	if( bByMapNearby==FALSE )
	{
		LONG lNearX = 0, lNearY = 0;
		lMapIndexLoop = lMapIndexLoop *2;
		if( GetDieValidPrescanPosn(lTgtRow, lTgtCol, lMapIndexLoop, lNearX, lNearY, TRUE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByMapNearby = TRUE;
			szNear = "wt fh3, ";
			lLoopCounter = -1;
		}
	}

	if( bByMapNearby )
	{
		LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
		if( GetMapIndexStepCol()%2==0 )
			lGrabOffsetX = GetDiePitchX_X()/2;
		if( GetMapIndexStepRow()%2==0 )
			lGrabOffsetY = GetDiePitchY_Y()/2;
		lTgtWfX = lTgtWfX + lGrabOffsetX;
		lTgtWfY = lTgtWfY + lGrabOffsetY;
	}
	else
	{
		LONG lDiePitchX_X	= GetDiePitchX_X();
		LONG lDiePitchX_Y	= GetDiePitchX_Y();
		LONG lDiePitchY_Y	= GetDiePitchY_Y();
		LONG lDiePitchY_X	= GetDiePitchY_X();

		lTgtWfX = lOldWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
		lTgtWfY = lOldWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
	}

	LONG lHmiRow = 0, lHmiCol = 0;
	ConvertAsmToHmiUser(lTgtRow, lTgtCol, lHmiRow, lHmiCol);
	szText.Format("%s%5ld,%5ld,%5ld,%5ld,%8ld,%8ld, %ld(%ld)", (LPCTSTR)szNear, 
		lTgtRow, lTgtCol, lHmiRow, lHmiCol, lTgtWfX, lTgtWfY, lLoopCounter, lMapIndexLoop);
	pUtl->PrescanMoveLog(szText);
	m_nPrescanNextMapRow	= lTgtRow;
	m_nPrescanNextMapCol	= lTgtCol;
	m_nPrescanNextWftPosnX	= lTgtWfX;
	m_nPrescanNextWftPosnY	= lTgtWfY;

	return FALSE;
}

BOOL CWaferTable::PrescanOpGetPosn_MapIndex(LONG lOldRow, LONG lOldCol, LONG lOldWfX, LONG lOldWfY, 
											LONG &lTgtRow, LONG &lTgtCol, LONG &lTgtWfX, LONG &lTgtWfY, const LONG sDir)
{
	LONG	lDiffX = 0, lDiffY = 0;
	UCHAR	ucNullBin = m_WaferMapWrapper.GetNullBin();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	CString szText;
	szText.Format("wt old, %5ld,%5ld,  ade,  ade,  %8ld,%8ld", lOldRow, lOldCol, lOldWfX, lOldWfY);
	pUtl->PrescanMoveLog(szText);

	LONG lScnX = 0, lScnY = 0;
	LONG lStepCol = GetMapIndexStepCol();
	LONG lStepRow = GetMapIndexStepRow();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( m_bDualWayTipWafer )
	{
		lStepCol = 2;
	}
	BOOL bHasDie = FALSE;
	// move map to right, table to left
	lDiffY = 0;
	lDiffX = 0;
	while( 1 )
	{
		if( sDir==0 )			// to right
		{
			lDiffX += lStepCol;
			lTgtRow = lOldRow + lDiffY;
			lTgtCol = lOldCol + lDiffX;
			if( (lTgtCol>(GetMapValidMaxCol()+(lStepCol+1)/2)) )
			{
				return TRUE;
			}
		}
		else if( sDir==1 )		// to left
		{
			lDiffX -= lStepCol;
			lTgtRow = lOldRow + lDiffY;
			lTgtCol = lOldCol + lDiffX;
			if( (lTgtCol<(GetMapValidMinCol()-(lStepCol+1)/2)) )
			{
				return TRUE;
			}
		}
		else if( sDir==2 )		// to bottom
		{
			lDiffY += lStepRow;
			lTgtRow = lOldRow + lDiffY;
			lTgtCol = lOldCol + lDiffX;
			if( (lTgtRow>(GetMapValidMaxRow()+(lStepRow+1)/2)) )
			{
				return TRUE;
			}
		}
		else					// to top
		{
			lDiffY -= lStepRow;
			lTgtRow = lOldRow + lDiffY;
			lTgtCol = lOldCol + lDiffX;
			if( (lTgtRow<(GetMapValidMinRow()-(lStepRow+1)/2)) )
			{
				return TRUE;
			}
		}

/*
if( lTgtRow>(GetMapValidMaxRow()+(lStepRow+1)/2) || 
	lTgtRow<(GetMapValidMinRow()-(lStepRow+1)/2) )
{
	bHasDie = FALSE;
	break;
}

if( lTgtCol>(GetMapValidMaxCol()+(lStepCol+1)/2) || 
	lTgtCol<(GetMapValidMinCol()-(lStepCol+1)/2) )
{
	bHasDie = FALSE;
	break;
}
*/
		LONG lHalfRow = lStepRow/2;
		LONG lHalfCol = lStepCol/2;
		for(LONG jRow=(0-lHalfRow); jRow<=(0+lHalfRow); jRow++)
		{
			for(LONG iCol=(0-lHalfCol); iCol<=(0+lHalfCol); iCol++)
			{
				LONG lChkRow = jRow+lTgtRow;
				LONG lChkCol = iCol+lTgtCol;
				if( IsInMapValidRange(lChkRow, lChkCol)==FALSE )
				{
					continue;
				}
				if (m_pWaferMapManager->IsMapHaveBin(lChkRow, lChkCol))
				{
					bHasDie = TRUE;
					break;
				}
			}
			if( bHasDie )
			{
				break;
			}
		}

		if( bHasDie )
		{
			break;
		}
	}

	if( bHasDie==FALSE )
	{
		return TRUE;
	}

	CString szNear = "no ad0, ";
	BOOL bByMapNearby = FALSE;
	LONG lNearX = 0, lNearY = 0;
	LONG lLoopCounter = 0;
	LONG lMapIndexLoop = max(lStepCol, lStepRow)*m_ulMapIndexLoop;
	while( 1 )
	{
		if( OneDieIndexGetDieValidPosn(lTgtRow, lTgtCol, lMapIndexLoop, lNearX, lNearY, TRUE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByMapNearby = TRUE;
			szNear = "wt ad1, ";
			break;
		}
		Sleep(10);
		if( pUtl->GetPrescanMapIndexIdle() && 
			m_lPrescanLastGrabRow==lOldRow && 
			m_lPrescanLastGrabCol==lOldCol && 
			m_bPrescanLastGrabbed==TRUE  )
		{
			Sleep(10);
			break;
		}
		lLoopCounter++;
		if( lLoopCounter>1000 ) // > 0.5 seconds
		{
			break;
		}
	}

	if( lLoopCounter<=1000 && bByMapNearby==FALSE )
	{
		if( OneDieIndexGetDieValidPosn(lTgtRow, lTgtCol, lMapIndexLoop, lNearX, lNearY, TRUE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByMapNearby = TRUE;
			szNear = "wt ad2, ";
		}
	}

	if( bByMapNearby==FALSE )
	{
		LONG lNearX = 0, lNearY = 0;
		lMapIndexLoop = lMapIndexLoop *2;
		if( GetDieValidPrescanPosn(lTgtRow, lTgtCol, lMapIndexLoop, lNearX, lNearY, TRUE) )
		{
			lTgtWfX = lNearX;
			lTgtWfY = lNearY;
			bByMapNearby = TRUE;
			szNear = "wt ad3, ";
			lLoopCounter = -1;
		}
	}

	if( bByMapNearby )
	{
		LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
		if( GetMapIndexStepCol()%2==0 )
			lGrabOffsetX = GetDiePitchX_X()/2;
		if( GetMapIndexStepRow()%2==0 )
			lGrabOffsetY = GetDiePitchY_Y()/2;
		lTgtWfX = lTgtWfX + lGrabOffsetX;
		lTgtWfY = lTgtWfY + lGrabOffsetY;
	}
	else
	{
		LONG lDiePitchX_X	= GetDiePitchX_X();
		LONG lDiePitchX_Y	= GetDiePitchX_Y();
		LONG lDiePitchY_Y	= GetDiePitchY_Y();
		LONG lDiePitchY_X	= GetDiePitchY_X();

		lTgtWfX = lOldWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
		lTgtWfY = lOldWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
	}
	LONG lHmiRow = 0, lHmiCol = 0;
	ConvertAsmToHmiUser(lTgtRow, lTgtCol, lHmiRow, lHmiCol);
	szText.Format("%s%5ld,%5ld,%5ld,%5ld,%8ld,%8ld, %ld(%ld)", (LPCTSTR)szNear, 
		lTgtRow, lTgtCol, lHmiRow, lHmiCol, lTgtWfX, lTgtWfY, lLoopCounter, lMapIndexLoop);
	pUtl->PrescanMoveLog(szText);

	return FALSE;
}

BOOL CWaferTable::OpCalculateADEWaferLimit()
{
	//get the edge die and calculate circular or rectagnel;
	if( m_ucPrescanMapShapeType == WT_MAP_SHAPE_CIRCLE )
	{
		LONG lEdgeX[4], lEdgeY[4];
		for(int i=0; i<4; i++)
		{
			lEdgeX[i] = GetPrescanAlignPosnX();
			lEdgeY[i] = GetPrescanAlignPosnY();
		}
		for(ULONG ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			LONG lEncX = 0, lEncY = 0;
			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY) )
			{
				if( lEncX>lEdgeX[0] )
				{
					lEdgeX[0] = lEncX;
					lEdgeY[0] = lEncY;
				}
				if( lEncX<lEdgeX[1] )
				{
					lEdgeX[1] = lEncX;
					lEdgeY[1] = lEncY;
				}
				if( lEncY<lEdgeY[2] )
				{
					lEdgeX[2] = lEncX;
					lEdgeY[2] = lEncY;
				}
				if( lEncY>lEdgeY[3] )
				{
					lEdgeX[3] = lEncX;
					lEdgeY[3] = lEncY;
				}
			}
		}
		CString szMsg, szTemp;
		szMsg.Format("WT - ADE 4 edge points ");
		for(int i=0; i<4; i++)
		{
			szTemp.Format("(%d,%d); ", lEdgeX[i], lEdgeY[i]);
			szMsg += szTemp;
		}
		SaveScanTimeEvent(szMsg);

		if( m_lDetectEdgeState>3 )
		{
			// 0 1 2
			m_lWaferTmpX[0] = lEdgeX[0];
			m_lWaferTmpY[0] = lEdgeY[0];
			m_lWaferTmpX[1] = lEdgeX[3];
			m_lWaferTmpY[1] = lEdgeY[3];
			m_lWaferTmpX[2] = lEdgeX[2];
			m_lWaferTmpY[2] = lEdgeY[2];
			if( CalcWaferCircle() )
			{
				LONG lEdgeDiameter = (LONG)GetDistance(m_lAutoWaferCenterX, m_lAutoWaferCenterY,
					lEdgeX[1], lEdgeY[1])*2;
				m_nPrescanMapCtrX = m_lAutoWaferCenterX;
				m_nPrescanMapCtrY = m_lAutoWaferCenterY;
				if( m_lAutoWaferDiameter<lEdgeDiameter )
					m_lAutoWaferDiameter = lEdgeDiameter;
				m_nPrescanMapRadius = m_lAutoWaferDiameter/2;
				szMsg.Format("WT - ADE Center (%d,%d), diameter %d", m_lAutoWaferCenterX, m_lAutoWaferCenterY, m_lAutoWaferDiameter);
				SaveScanTimeEvent(szMsg);
			}
		}
		else
		{
			// 0 1 2
			m_lWaferTmpX[0] = lEdgeX[0];
			m_lWaferTmpY[0] = lEdgeY[0];
			m_lWaferTmpX[1] = lEdgeX[1];
			m_lWaferTmpY[1] = lEdgeY[1];
			m_lWaferTmpX[2] = lEdgeX[2];
			m_lWaferTmpY[2] = lEdgeY[2];
			if( CalcWaferCircle() )
			{
				m_nPrescanMapCtrX = m_lAutoWaferCenterX;
				m_nPrescanMapCtrY = m_lAutoWaferCenterY;
				m_nPrescanMapRadius = m_lAutoWaferDiameter/2;
			}
		}
		m_nPrescanMapWidth  = m_nPrescanMapRadius*2;
		m_nPrescanMapHeight = m_nPrescanMapRadius*2;
	}
	else
	{
		LONG lMinX, lMaxX, lMinY, lMaxY;
		lMinX = lMaxX = GetPrescanAlignPosnX();
		lMinY = lMaxY = GetPrescanAlignPosnY();

		for(ULONG ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			LONG lEncX = 0, lEncY = 0;
			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY) )
			{
				if( lMaxX<lEncX )
					lMaxX = lEncX;
				if( lMinX>lEncX )
					lMinX = lEncX;
				if( lMaxY<lEncY )
					lMaxY = lEncY;
				if( lMinY>lEncY )
					lMinY = lEncY;
			}
		}
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		UINT unWafEdgeNum	= pApp->GetPrescanWaferEdgeNum();
		m_nPrescanMapWidth  = labs(lMaxX-lMinX) + 2*unWafEdgeNum*GetPrescanPitchX();
		m_nPrescanMapHeight = labs(lMaxY-lMinY) + 2*unWafEdgeNum*GetPrescanPitchY();
		m_nPrescanMapCtrX	= m_lAutoWaferCenterX	= (lMaxX+lMinX)/2;
		m_nPrescanMapCtrY	= m_lAutoWaferCenterY = (lMaxY+lMinY)/2;
		m_nPrescanMapRadius		= max(m_nPrescanMapWidth, m_nPrescanMapHeight)/2;
		m_lAutoWaferDiameter	= m_nPrescanMapRadius*2;
	}

	CString szMsg;
	szMsg.Format("wafer center (%d,%d), range (%d,%d), (%d,%d)", 
		GetScanCtrX(), GetScanCtrY(), GetScanUL_X(), GetScanUL_Y(), GetScanLR_X(), GetScanLR_Y());
	SaveScanTimeEvent(szMsg);

	return TRUE;
}

BOOL CWaferTable::OpGetScanPosn_Sprial()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	LONG	lOldFrameRow = GetLastFrameRow();
	LONG	lOldFrameCol = GetLastFrameCol();
	LONG	lLoopFrameRow = GetAlignFrameRow();
	LONG	lLoopFrameCol = GetAlignFrameCol();

	CString szText;
	szText.Format("wt old, %5ld,%5ld,  spr,  wft,%8ld,%8ld", lOldFrameRow, lOldFrameCol, GetScanLastPosnX(), GetScanLastPosnY());
	pUtl->PrescanMoveLog(szText);

	// move map to right, table to left
	BOOL	bFindNextFrame = FALSE;
	LONG	lUpIndex = 0, lDnIndex = 0, lLtIndex = 0, lRtIndex = 0;

	BOOL bNextOkBreak = FALSE;
	ULONG lFastHomeLoopLimit	= max(SCAN_MAX_MATRIX_ROW, SCAN_MAX_MATRIX_COL);
	for(ULONG ulLoop=1; ulLoop<=lFastHomeLoopLimit; ulLoop++)
	{
		LONG lSpanDist = (ulLoop*2 + 1);

		//Move to RIGHT & PR search on current die
		if( lLoopFrameRow==lOldFrameRow && lLoopFrameCol==lOldFrameCol )
			bNextOkBreak = TRUE;
		lLoopFrameCol += 1;	//Move table to RIGHT outer one frame
		if( bNextOkBreak && IsScanFrameInWafer(lLoopFrameRow, lLoopFrameCol) )
		{
			bFindNextFrame = TRUE;
			break;
		}

		//Move to UP & PR search on current die
		lUpIndex = lSpanDist-1-1;
		while(1)
		{
			if( lLoopFrameRow==lOldFrameRow && lLoopFrameCol==lOldFrameCol )
				bNextOkBreak = TRUE;
			lLoopFrameRow -= 1;	//Move table to UP
			if( bNextOkBreak && IsScanFrameInWafer(lLoopFrameRow, lLoopFrameCol) )
			{
				bFindNextFrame = TRUE;
				break;
			}

			lUpIndex--;
			if (lUpIndex == 0)
			{
				break;
			}
		}
		if( bFindNextFrame )
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = lSpanDist-1;
		while(1)
		{
			if( lLoopFrameRow==lOldFrameRow && lLoopFrameCol==lOldFrameCol )
				bNextOkBreak = TRUE;
			lLoopFrameCol -= 1;	//Move table to LEFT
			if( bNextOkBreak && IsScanFrameInWafer(lLoopFrameRow, lLoopFrameCol) )
			{
				bFindNextFrame = TRUE;
				break;
			}

			lLtIndex--;
			if (lLtIndex == 0)
			{
				break;
			}
		}
		if( bFindNextFrame )
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = lSpanDist-1;
		while(1)
		{
			if( lLoopFrameRow==lOldFrameRow && lLoopFrameCol==lOldFrameCol )
				bNextOkBreak = TRUE;
			lLoopFrameRow += 1;	//Move table to DN
			if( bNextOkBreak && IsScanFrameInWafer(lLoopFrameRow, lLoopFrameCol) )
			{
				bFindNextFrame = TRUE;
				break;
			}

			lDnIndex--;
			if (lDnIndex == 0)
			{
				break;
			}
		}
		if( bFindNextFrame )
		{
			break;
		}

		//Move to RIGHT & PR search on current die
		lRtIndex = lSpanDist-1;
		while(1)
		{
			if( lLoopFrameRow==lOldFrameRow && lLoopFrameCol==lOldFrameCol )
				bNextOkBreak = TRUE;
			lLoopFrameCol += 1;	//Move table to map RIGHT
			if( bNextOkBreak && IsScanFrameInWafer(lLoopFrameRow, lLoopFrameCol) )
			{
				bFindNextFrame = TRUE;
				break;
			}

			lRtIndex--;
			if (lRtIndex == 0)
			{
				break;
			}
		}
		if( bFindNextFrame )
		{
			break;
		}
	}

	if ( bFindNextFrame == FALSE )
	{
		HmiMessage_Red_Back("Please stop machine and manual align wafer!", "AOI Prescan");
		return TRUE;
	}

	m_lNextFrameRow = lLoopFrameRow;
	m_lNextFrameCol = lLoopFrameCol;
	m_nPrescanNextWftPosnX = m_laScanPosnX[lLoopFrameRow][lLoopFrameCol];
	m_nPrescanNextWftPosnY = m_laScanPosnY[lLoopFrameRow][lLoopFrameCol];

	if( GetDiePitchX_X()!=0 && GetDiePitchY_Y()!=0 )
	{
		LONG lScanRow = (GetPrescanAlignPosnY() - GetScanNextPosnY())/GetDiePitchY_Y() + m_nPrescanAlignMapRow;
		LONG lScanCol = (GetPrescanAlignPosnX() - GetScanNextPosnX())/GetDiePitchX_X() + m_nPrescanAlignMapCol;
		if( lScanRow<=0 )
			lScanRow = 0;
		if( lScanCol<0 )
			lScanCol = 0;
		ULONG ulMaxRow = 0, ulMaxCol = 0;
		m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
		if(lScanRow>=(LONG)ulMaxRow )
			lScanRow = ulMaxRow-1;
		if(lScanCol>=(LONG)ulMaxCol )
			lScanCol = ulMaxCol-1;
		m_nPrescanNextMapRow = lScanRow;
		m_nPrescanNextMapCol = lScanCol;
	}

	szText.Format("wt map, %5ld,%5ld,%5ld,%5ld,%8ld,%8ld", 
		m_nPrescanNextMapRow, m_nPrescanNextMapCol, lLoopFrameRow, lLoopFrameCol, GetScanNextPosnX(), GetScanNextPosnY());
	pUtl->PrescanMoveLog(szText);

	return FALSE;
}

LONG CWaferTable::GetMA1FilePath(IPC_CServiceMessage &svMsg)	// Get the MA1 file path via pop-up dialog
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szScanMA1FilePath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::InputSaveMapImagePath(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szSaveImagePath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szSaveImagePath = &pBuffer[0];

	delete[] pBuffer;

	if (CreateDirectory(szSaveImagePath, NULL) == 0)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			bReply = TRUE;
		}
		else
		{
			CString szContent;
			szContent.LoadString(HMB_GENERAL_INVALID_PATH);
			HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);			
			bReply = FALSE;
		}
	}
	else
	{
		bReply = TRUE;
	}

	if( bReply )
	{
		m_szSaveMapImagePath = szSaveImagePath;
	}
	else
	{
		m_szSaveMapImagePath = "";
	}

	SaveWaferTblData();
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 

LONG CWaferTable::GetSaveMapImagePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if (pAppMod != NULL)
	{
		pAppMod->GetPath(m_szSaveMapImagePath);
		bReturn = TRUE;
		SaveWaferTblData();
	}
	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::InputLogPrNgImagePath(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szLogNgImagePath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szLogNgImagePath = &pBuffer[0];

	delete[] pBuffer;

	if (CreateDirectory(szLogNgImagePath, NULL) == 0)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			bReply = TRUE;
		}
		else
		{
			CString szContent;
			szContent.LoadString(HMB_GENERAL_INVALID_PATH);
			HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);			
			bReply = FALSE;
		}
	}
	else
	{
		bReply = TRUE;
	}
	if( bReply )
	{
		m_szScanNgImageLogPath = szLogNgImagePath;
	}
	else
	{
		m_szScanNgImageLogPath = "";
	}

	SaveWaferTblData();
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 

LONG CWaferTable::GetLogPrNgImagePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if (pAppMod != NULL)
	{
		pAppMod->GetPath(m_szScanNgImageLogPath);
		bReturn = TRUE;
		SaveWaferTblData();
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CWaferTable::SelectMA3Format(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	m_szScanAoiMapFormat = &pBuffer[0];

	delete[] pBuffer;

	SaveWaferTblData();

	BOOL 	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 

VOID CWaferTable::ClickMapAndTableGo()
{
	if( IsPrescanEnable()==FALSE )
	{
		return ;
	}

	if( m_bEnableClickMapAndTableGo==FALSE )
	{
		return ;
	}

	ULONG ulRow, ulCol;
	LONG lPhyX, lPhyY;
	m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);
	BOOL bToGo = FALSE;
	if( GetPrescanWftPosn(ulRow, ulCol, lPhyX, lPhyY) )
	{
		bToGo = TRUE;
	}
	else
	{
		if( OneDieIndexGetDieValidPosn(ulRow, ulCol, 5, lPhyX, lPhyY, TRUE) )
		{
			bToGo = TRUE;
		}
	}

	if( bToGo )
	{
		XY_SafeMoveTo(lPhyX, lPhyY);
		if( m_ulPrescanGoCheckLimit>0 && m_ulPrescanGoCheckCount<m_ulPrescanGoCheckLimit )
		{
			BOOL bDoneBefore = FALSE;
			LONG lCheckTotal = min((LONG)m_aUIGoCheckRow.GetSize(), (LONG)m_aUIGoCheckCol.GetSize());
			for(LONG i=0; i<lCheckTotal; i++)
			{
				if( labs(ulRow-m_aUIGoCheckRow.GetAt(i))<=SCAN_GO_CHECK_NEAR_LIMIT && 
					labs(ulCol-m_aUIGoCheckCol.GetAt(i))<=SCAN_GO_CHECK_NEAR_LIMIT )
				{
					bDoneBefore = TRUE;
					break;
				}
			}

			if( bDoneBefore==FALSE )
			{
				m_aUIGoCheckRow.Add(ulRow);
				m_aUIGoCheckCol.Add(ulCol);
				m_ulPrescanGoCheckCount++;	// not repeat, increase
			}
		}	// handle the go check points
		if( m_ulPitchAlarmGoCheckLimit>0 && m_ulPitchAlarmGoCheckCount<m_ulPitchAlarmGoCheckLimit )
		{
			BOOL bDoneBefore = FALSE;
			LONG lCheckTotal = min((LONG)m_aUIGoCheckRow.GetSize(), (LONG)m_aUIGoCheckCol.GetSize());
			for(LONG i=0; i<lCheckTotal; i++)
			{
				if( labs(ulRow-m_aUIGoCheckRow.GetAt(i))<=0 && 
					labs(ulCol-m_aUIGoCheckCol.GetAt(i))<=0 )
				{
					bDoneBefore = TRUE;
					break;
				}
			}

			if( bDoneBefore==FALSE )
			{
				m_aUIGoCheckRow.Add(ulRow);
				m_aUIGoCheckCol.Add(ulCol);
				m_ulPitchAlarmGoCheckCount++;
			}
		}	// handle the pitch go check points
		SetAlarmLamp_RedYellow_Only(FALSE, FALSE);
	}
	else
	{
		HmiMessage_Red_Back("The selected die is dummy, please choose another.", "Prescan Go");
	}
}

LONG CWaferTable::GotoNextToSortDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;

	if( IsPrescanEnable() && m_szNextLocation.IsEmpty()==FALSE )
	{
		CStringArray szDataList;
		CUtility::Instance()->ParseRawData(m_szNextLocation, szDataList);
		if( szDataList.GetSize()>=2 )
		{
			ULONG ulRow = 0, ulCol = 0;
			LONG lUserRow = atoi(szDataList.GetAt(0));
			LONG lUserCol = atoi(szDataList.GetAt(1));
			if( ConvertOrgUserToAsm(lUserRow, lUserCol, ulRow, ulCol) )
			{
				m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
				m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
				bReturn = TRUE;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::ESMoveTablesToUnload(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	BOOL bWaferLoader = (BOOL) ((LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"]);
	if (bWaferLoader)
	{
		if ( IsWL1ExpanderSafeToMove() == FALSE )
		{
			svMsg.InitMessage(sizeof(BOOL),	&bReturn);
			return 1;
		}

		if( IsESDualWL() )
		{
			if ( IsWL2ExpanderSafeToMove() == FALSE )
			{
				svMsg.InitMessage(sizeof(BOOL),	&bReturn);
				return 1;
			}
		}
	}
	
	SetJoystickOn(FALSE);

	if (MoveBackLightToSafeLevel() == FALSE)
	{	
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	// WT1 Move To Unload
#ifdef NU_MOTION
	X1_Profile(LOW_PROF1);
	Y1_Profile(LOW_PROF1);
#endif

	WFTTGoHomeUnderCam(FALSE);
	XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);

#ifdef NU_MOTION
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
#endif

	// move WT2 to unload
	if( IsESDualWT() )
	{
	#ifdef NU_MOTION
		X2_Profile(LOW_PROF1);
		Y2_Profile(LOW_PROF1);
	#endif

		WFTTGoHomeUnderCam(TRUE);
		XY2_MoveTo(m_lUnloadPhyPosX2, m_lUnloadPhyPosY2, SFM_WAIT);

	#ifdef NU_MOTION
		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);
	#endif
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::SetWT1JoystickLimitCmd(IPC_CServiceMessage &svMsg)
{
	BOOL	bMax, bOK=TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bMax);

	try
	{
		LONG lMinX = GetWft1CenterX() - GetWaferDiameter()/2;
		LONG lMaxX = GetWft1CenterX() + GetWaferDiameter()/2;
		LONG lMinY = GetWft1CenterY() - GetWaferDiameter()/2;
		LONG lMaxY = GetWft1CenterY() + GetWaferDiameter()/2;
		if (bMax == TRUE)
		{
			lMinX = WT_JOY_MAXLIMIT_NEG_X;
			lMaxX = WT_JOY_MAXLIMIT_POS_X;
			lMinY = WT_JOY_MAXLIMIT_NEG_Y;
			lMaxY = WT_JOY_MAXLIMIT_POS_Y;
		}
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, lMinX, lMaxX, &m_stWTAxis_X);		
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, lMinY, lMaxY, &m_stWTAxis_Y);		
		if( bMax )
		{
			if ( (m_ulJoyStickMode == MS899_JS_MODE_PR) )
				m_bCheckWaferLimit = FALSE;	
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
		bOK = FALSE;
	}	

	svMsg.InitMessage(sizeof(BOOL), &bOK);

	return 1;
}

LONG CWaferTable::SetWT2JoystickLimitCmd(IPC_CServiceMessage &svMsg)
{
	BOOL	bMax, bOK=TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bMax);

	try
	{
		LONG lMinX = GetWft2CenterX() - GetWaferDiameter()/2;
		LONG lMaxX = GetWft2CenterX() + GetWaferDiameter()/2;
		LONG lMinY = GetWft2CenterY() - GetWaferDiameter()/2;
		LONG lMaxY = GetWft2CenterY() + GetWaferDiameter()/2;

		if (bMax == TRUE)
		{
			lMinX = WT_JOY_MAXLIMIT_NEG_X;
			lMaxX = WT_JOY_MAXLIMIT_POS_X;
			lMinY = WT_JOY_MAXLIMIT_NEG_Y;
			lMaxY = WT_JOY_MAXLIMIT_POS_Y;
		}

		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X2, lMinX, lMaxX, &m_stWTAxis_X2);		
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y2, lMinY, lMaxY, &m_stWTAxis_Y2);		
		if( bMax )
		{
			if ( (m_ulJoyStickMode == MS899_JS_MODE_PR) )
			{
				m_bCheckWaferLimit = FALSE;	
			}
		}
	}

	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
		bOK = FALSE;
	}	

	svMsg.InitMessage(sizeof(BOOL), &bOK);

	return 1;
}


LONG CWaferTable::WT_SetContourTable(IPC_CServiceMessage &svMsg)
{
	UCHAR ucID;

	svMsg.GetMsg(sizeof(UCHAR), &ucID);

	if( ucID==4 )
	{
		(*m_psmfSRam)["MS896A"]["ContourTable2"] = 0;
	}
	else
	{
		(*m_psmfSRam)["MS896A"]["ContourTable2"] = 1;
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::WT_Contour_Move(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		LONG  lDir;
		LONG  lDist;
	}	WT_CONTOUR_MOVE;

	BOOL bWT2 = (BOOL)(LONG)((*m_psmfSRam)["MS896A"]["ContourTable2"]);
	WT_CONTOUR_MOVE stMove;
	svMsg.GetMsg(sizeof(WT_CONTOUR_MOVE), &stMove);
	LONG lPos = labs(stMove.lDist);

	if( bWT2==0 )	// WT1
	{
		SetJoystickOn(FALSE, FALSE);
		switch(stMove.lDir)
		{
		case 3:	// Y DOWN
			Y1_Move(0-lPos);
			break;
		case 2:	// Y UP
			Y1_Move(lPos);
			break;
		case 1:	// X RIGHT
			X1_Move(0-lPos);
			break;
		case 0:	// X LEFT
			X1_Move(lPos);
			break;
		}
		SetJoystickOn(TRUE, FALSE);
	}
	else 	// WT2
	{
		SetJoystickOn(FALSE, TRUE);
		switch(stMove.lDir)
		{
		case 3:	// Y DOWN
			Y2_Move(0-lPos);
			break;
		case 2:	// Y UP
			Y2_Move(lPos);
			break;
		case 1:	// X RIGHT
			X2_Move(0-lPos);
			break;
		case 0:	// X LEFT
			X2_Move(lPos);
			break;
		}
		SetJoystickOn(TRUE, TRUE);
	}
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::ShowContourEdge(IPC_CServiceMessage &svMsg)
{
	BOOL bUseBL = FALSE;
	CString szTitle, szMsg;
	szTitle = "Show Contour";

	if( m_bUseContour==FALSE )
	{
		HmiMessage("Contour option is disable.", szTitle);
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsBLInUse() )
	{
		if( HmiMessage("Do you want to let BL up?\nMaybe the BL crash with expander.",
			szTitle, glHMI_MBX_YESNO)==glHMI_YES )
		{
			bUseBL = TRUE;
		}
		BOOL bOK = FALSE;
		BOOL bWft = FALSE;
		if( IsWT1UnderCamera() && !IsWT2InUse() )
		{
			bOK = TRUE;
			bWft = FALSE;
		}
		if( bOK )
		{
			OpenWaitingAlert();
			if( bUseBL )
			{
				m_lBackLightZStatus = 0;	//	4.24TX 4
				MoveES101BackLightZUpDn(TRUE);	// in prestart, if align ok, check current is safe, UP BL Z
			}
			MoveFocusToWafer(bWft);
			CloseWaitingAlert();
		}
		else
		{
			szMsg = "WT1 in use, can not show contour of WT2";
			HmiMessage(szMsg, szTitle);

			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	if( IsAlignedWafer() )
	{
		//  S->-
		// /\  |
		//	|  V
		//  -<-|
		//
		for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMinRow; nRow++)
		{
			for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
			{
				if( m_baScanMatrix[nRow][nCol] )
				{
					LONG lPosnX = m_laScanPosnX[nRow][nCol];
					LONG lPosnY = m_laScanPosnY[nRow][nCol];
					XY_SafeMoveTo(lPosnX, lPosnY);
					Sleep(1000);
				}
			}
		}

		for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
		{
			for(INT nCol=m_lScanFrameMaxCol; nCol>=m_lScanFrameMinCol; nCol--)
			{
				if( m_baScanMatrix[nRow][nCol] )
				{
					LONG lPosnX = m_laScanPosnX[nRow][nCol];
					LONG lPosnY = m_laScanPosnY[nRow][nCol];
					XY_SafeMoveTo(lPosnX, lPosnY);
					Sleep(1000);
					break;
				}
			}
		}

		for(INT nRow=m_lScanFrameMaxRow; nRow>=m_lScanFrameMaxRow; nRow--)
		{
			for(INT nCol=m_lScanFrameMaxCol; nCol>=m_lScanFrameMinCol; nCol--)
			{
				if( m_baScanMatrix[nRow][nCol] )
				{
					LONG lPosnX = m_laScanPosnX[nRow][nCol];
					LONG lPosnY = m_laScanPosnY[nRow][nCol];
					XY_SafeMoveTo(lPosnX, lPosnY);
					Sleep(1000);
				}
			}
		}

		for(INT nRow=m_lScanFrameMaxRow; nRow>=m_lScanFrameMinRow; nRow--)
		{
			for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
			{
				if( m_baScanMatrix[nRow][nCol] )
				{
					LONG lPosnX = m_laScanPosnX[nRow][nCol];
					LONG lPosnY = m_laScanPosnY[nRow][nCol];
					XY_SafeMoveTo(lPosnX, lPosnY);
					Sleep(1000);
					break;
				}
			}
		}
	}
	else
	{
		ULONG	ulContourEdgePoints	= 0;
		LONG	lCntrEdgeY_C[SCAN_MAX_MATRIX_ROW+1];
		LONG	lCntrEdgeX_L[SCAN_MAX_MATRIX_ROW+1];
		LONG	lCntrEdgeX_R[SCAN_MAX_MATRIX_ROW+1];

		CString szFilename = ES_CONTOUR_WL1_FILE;
		CStdioFile fpEdge;
		CString szReading;
		CStringArray szaDataList;
		if( fpEdge.Open(szFilename, CFile::modeRead|CFile::shareDenyNone) )
		{
			fpEdge.SeekToBegin();
			// get the fail counter limit
			fpEdge.ReadString(szReading);

			while( fpEdge.ReadString(szReading)!=NULL )
			{
				INT nPosnY = 0;
				LONG lLftX, lRgtX, lCtrY;
				szaDataList.RemoveAll();
				CUtility::Instance()->ParseRawData(szReading, szaDataList);
				if( szaDataList.GetSize()>=5 )
				{
					lLftX = atoi(szaDataList.GetAt(3));	// left out x
					lCtrY = atoi(szaDataList.GetAt(1));	// center y
					lRgtX = atoi(szaDataList.GetAt(4));	// right x
					lCntrEdgeY_C[ulContourEdgePoints] = lCtrY;
					lCntrEdgeX_L[ulContourEdgePoints] = lLftX;
					lCntrEdgeX_R[ulContourEdgePoints] = lRgtX;
					ulContourEdgePoints++;
					if( ulContourEdgePoints>=SCAN_MAX_MATRIX_ROW )
					{
						break;
					}
				}
			}
		}
		else
		{
			HmiMessage("Contour file can not open", szTitle);
		}
		fpEdge.Close();

		//  S->-
		// /\  |
		//	|  V
		//  -<-|
		//
		LONG lDistX = GetPrescanPitchX();
		if( lDistX==0 )
			ulContourEdgePoints = 0;
		if( ulContourEdgePoints>0 )
		{
			// top horizontal move from left to right
			LONG lXPoints = labs(lCntrEdgeX_R[0]-lCntrEdgeX_L[0])/lDistX + 1;
			for(INT nLoop=0; nLoop<=lXPoints; nLoop++)
			{
				LONG lPosnX = lCntrEdgeX_L[0] - lDistX*nLoop;
				if( lPosnX<lCntrEdgeX_R[0] )
					lPosnX = lCntrEdgeX_R[0];
				LONG lPosnY =lCntrEdgeY_C[0];
				XY_SafeMoveTo(lPosnX, lPosnY);
				Sleep(1000);
			}

			// vertival move down at right hand from top to bottom
			for(INT nRow = 1; nRow < (LONG)ulContourEdgePoints; nRow++)
			{
				LONG lPosnX = lCntrEdgeX_R[nRow];
				LONG lPosnY = lCntrEdgeY_C[nRow];
				XY_SafeMoveTo(lPosnX, lPosnY);
				Sleep(1000);
			}

			// bottom horizontal move from right to left
			lXPoints = labs(lCntrEdgeX_R[ulContourEdgePoints-1]-lCntrEdgeX_L[ulContourEdgePoints-1])/lDistX + 1;
			for(INT nLoop=1; nLoop<=lXPoints; nLoop++)
			{
				LONG lPosnX = lCntrEdgeX_R[ulContourEdgePoints-1] - lDistX*nLoop;
				if( lPosnX>lCntrEdgeX_L[ulContourEdgePoints-1] )
					lPosnX = lCntrEdgeX_L[ulContourEdgePoints-1];
				LONG lPosnY =lCntrEdgeY_C[ulContourEdgePoints-1];
				XY_SafeMoveTo(lPosnX, lPosnY);
				Sleep(1000);
			}

			// vertival move up at left hand from bottom to top
			for(INT nRow=ulContourEdgePoints-2; nRow>=0; nRow--)
			{
				LONG lPosnX = lCntrEdgeX_L[nRow];
				LONG lPosnY = lCntrEdgeY_C[nRow];
				XY_SafeMoveTo(lPosnX, lPosnY);
				Sleep(1000);
			}
		}
	}

	Sleep(2000);
	if( bUseBL )
	{
		m_lBackLightZStatus = 2;
		MoveES101BackLightZUpDn(FALSE);	// in prestart, if align ok, check current is safe, UP BL Z
	}

	XY_SafeMoveTo(GetWaferCenterX(), GetWaferCenterY());

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::DeleteMapPsmScnCmd(IPC_CServiceMessage &svMsg)
{
	DeleteCurrentMapPsmScn();
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::TickMultiSortToOnCmd(IPC_CServiceMessage &svMsg)
{
	if( m_bSortMultiToOne )
	{
		CUIntArray aulGroupGradeList;
		aulGroupGradeList.RemoveAll();
		//Use user define grade list
		if (m_pWaferMapManager->GetGroupGradeList(aulGroupGradeList) == FALSE)
		{
			m_bSortMultiToOne = FALSE;
		}
		aulGroupGradeList.RemoveAll();
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

CString CWaferTable::GetMapFilePath()
{
	CString szMapPath = m_szMapFilePath;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szSubFolder		= (*m_psmfSRam)["MS896A"]["MapSubFolderName"];

	//Add subfolder if sub-folder exist
	if ( szSubFolder.IsEmpty() == FALSE )	
	{
		if (m_lOTraceabilityFormat == 1)		//PLLM LED
			szMapPath = m_szMapFilePath + "\\shipment" + szSubFolder;
		else if (pApp->GetCustomerName() == "Avago")	//v4.48A33
			szMapPath = m_szMapFilePath;
		else
			szMapPath = m_szMapFilePath + "\\" + szSubFolder;
	}
	else
	{
		if( m_bBarCodeSearchFolder )
		{
			CString szMsg;
			if( SearchFolderInMapPath(szMapPath, m_szMapFilePath, m_szSearchFolderName) )
			{
				szMsg.Format("Map path %s, lot no %s, at last find %s", m_szMapFilePath, m_szSearchFolderName, szMapPath);
			}
			else
			{
				szMsg.Format("Map path %s, lot no %s, no find target folder", m_szMapFilePath, m_szSearchFolderName);
				szMapPath = m_szMapFilePath;
			}
			SetErrorMessage(szMsg);
		}
		else
		{
			szMapPath = m_szMapFilePath;
		}
	}

	return szMapPath;
}

BOOL CWaferTable::DeleteCurrentMapPsmScn()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bDeleteIt = FALSE;
	if( pApp->GetCustomerName() == CTM_SANAN )
	{
		if( m_bAutoLoadWaferMap==FALSE )
			bDeleteIt = TRUE;
	//	else if( pApp->GetProductLine()!="WH" && pApp->GetProductLine()!="XA" )
	//		bDeleteIt = TRUE;
	}
	if( bDeleteIt )
	{
		CString	szOperatorID = pApp->GetUsername();
		CString szMapFile = GetMapFileName();
		CString szPsmFile = szMapFile, szScnFile = szMapFile;
		CString szLogMsg;
		szLogMsg = szOperatorID + " Delete psm and scn of " + szMapFile;
		CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg);
		// to find ; and remove
		int iCol = szMapFile.ReverseFind(';');
		if( iCol!=-1 )
		{
			szMapFile = szMapFile.Left(iCol);
		}
		iCol = szMapFile.ReverseFind('.');
		if ( iCol != -1 )
		{
			szMapFile = szMapFile.Left(iCol);
		}
		szPsmFile = szMapFile + ".psm";	
		szScnFile = szMapFile + ".scn";

		szLogMsg = "to delete " + szPsmFile;
		if (_access(szPsmFile, 0) != -1)
		{
			DeleteFile(szPsmFile);
			szLogMsg = szLogMsg + " done";
		}
		CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg);

		szLogMsg = " to delete " + szScnFile;
		if (_access(szScnFile, 0) != -1)
		{
			DeleteFile(szScnFile);
			szLogMsg = szLogMsg + " done";
		}
		CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg);

		iCol = szMapFile.ReverseFind('\\');
		if( iCol!=-1 )
		{
			szMapFile = szMapFile.Mid(iCol+1);
		}
		CString szMapOrgPath = GetMapFilePath();
		szPsmFile = szMapOrgPath + "\\" + szMapFile + ".psm";	
		szScnFile = szMapOrgPath + "\\" + szMapFile + ".scn";

		szLogMsg = " to delete " + szPsmFile;
		if (_access(szPsmFile, 0) != -1)
		{
			DeleteFile(szPsmFile);
			szLogMsg = szLogMsg + " done";
		}
		CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg);

		szLogMsg = " to delete " + szScnFile;
		if (_access(szScnFile, 0) != -1)
		{
			DeleteFile(szScnFile);
			szLogMsg = szLogMsg + " done";
		}
		CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg);
	}	//	check manual load and auto delete psm/scn for SanAn

	return TRUE;
}

LONG CWaferTable::GetYSlowMoveTime(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		BOOL bWT2;
		LONG lDist;
	}	WT_MOVE_TIME;

	WT_MOVE_TIME stTime;

	svMsg.GetMsg(sizeof(WT_MOVE_TIME), &stTime);

	LONG lTime = 0;
	if( stTime.bWT2 )
	{
		lTime = Y2_ProfileTime(LOW_PROF1, stTime.lDist, stTime.lDist);
	}
	else
	{
		lTime = Y1_ProfileTime(LOW_PROF1, stTime.lDist, stTime.lDist);
	}

	svMsg.InitMessage(sizeof(LONG), &lTime);

	return 1;
}

LONG CWaferTable::SearchNml3AtAlignPointForPrescan(LONG lX, LONG lY)
{
	if( m_bPrescanNml3AtHomeDie )
	{
		Sleep(50);
		(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lX;
		(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lY;

		BOOL bDrawDie = FALSE;
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(BOOL), &bDrawDie);
		int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MultiSearchNml3Die", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		LONG	lDieX = 0, lDieY = 0;
		DOUBLE	dDieAngle;
		ULONG	ulIdx;

		for(ulIdx=1; ulIdx<=GetGoodTotalDie(); ulIdx++)
		{
			CString szDieBin = "0";
			if( GetGoodPosnBin(ulIdx, lDieX, lDieY, dDieAngle, szDieBin) )
			{
				lDieX	= lX + lDieX;	
				lDieY	= lY + lDieY;
				WSSetPrescanPosition(lDieX, lDieY, dDieAngle, szDieBin, 666);
			}
		}
	}

	return TRUE;
}	// to multi search and find normal die 3


LONG CWaferTable::GenMapPhyPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	SavePrescanInfoMap(0);

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

VOID CWaferTable::ObtainContourEdgeDetail()
{
	m_bContourEdge		= FALSE;
	m_ulContourEdgePoints	= 0;
	memset(m_lCntrEdgeY_C, 0, sizeof(m_lCntrEdgeY_C));
	memset(m_lCntrEdgeX_L, 0, sizeof(m_lCntrEdgeX_L));
	memset(m_lCntrEdgeX_R, 0, sizeof(m_lCntrEdgeX_R));

	if( m_bUseContour==FALSE )
	{
		return ;
	}

	SaveScanTimeEvent("    WFT: - contour to define walking matrix");

	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szFilename = ES_CONTOUR_WL1_FILE;
	CStdioFile fpEdge;
	CString szReading;
	CStringArray szaDataList;
	if( fpEdge.Open(szFilename, CFile::modeRead|CFile::shareDenyNone) )
	{
		fpEdge.SeekToBegin();
		// get the fail counter limit
		if( fpEdge.ReadString(szReading) )
		{
			szaDataList.RemoveAll();
			CUtility::Instance()->ParseRawData(szReading, szaDataList);
			if( szaDataList.GetSize()<4 )
			{
				pUtl->PrescanMoveLog("Contour file format wrong");
			}
			else
			{
				m_bContourEdge = TRUE;
			}
		}

		while( m_bContourEdge )
		{
			INT nPosnY = 0;
			LONG lLftX, lRgtX, lCtrY;
			if( fpEdge.ReadString(szReading) )
			{
				szaDataList.RemoveAll();
				CUtility::Instance()->ParseRawData(szReading, szaDataList);
				if( szaDataList.GetSize()<3 )
				{
					m_bContourEdge = FALSE;
					pUtl->PrescanMoveLog("Contour file data wrong");
					break;
				}
				else
				{
					lLftX = atoi(szaDataList.GetAt(0));	//	left x
					lCtrY = atoi(szaDataList.GetAt(1));	//	center y
					lRgtX = atoi(szaDataList.GetAt(2));	//	right x
					m_lCntrEdgeY_C[m_ulContourEdgePoints] = lCtrY;
					m_lCntrEdgeX_L[m_ulContourEdgePoints] = lLftX;
					m_lCntrEdgeX_R[m_ulContourEdgePoints] = lRgtX;
					m_ulContourEdgePoints++;
					if( m_ulContourEdgePoints>=SCAN_MAX_MATRIX_ROW )
					{
						break;
					}
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		pUtl->PrescanMoveLog("Contour file can not open");
	}

	fpEdge.Close();

	SaveScanTimeEvent("    WFT: - Contour define walking matrix done");
}

LONG CWaferTable::GetSlowProfileDist()
{
	if( IsESMachine() )
	{
		m_lSlowProfileTrigger = min(GetPrescanPitchX(), GetPrescanPitchY())*2;
		if( m_lSlowProfileTrigger<=500 )
			m_lSlowProfileTrigger = 4000;
	}

	return m_lSlowProfileTrigger;
}

LONG CWaferTable::GetVerySlowDist()
{
	return 20*1000*1000;	// 20 meter
}

LONG CWaferTable::SetHmiOperationLogPath(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szOperationLogPath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szOperationLogPath = &pBuffer[0];

	delete[] pBuffer;

	if (CreateDirectory(szOperationLogPath, NULL) == 0)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			bReply = TRUE;
		}
		else
		{
			CString szContent;
			szContent.LoadString(HMB_GENERAL_INVALID_PATH);
			HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);			
			bReply = FALSE;
		}
	}
	else
	{
		bReply = TRUE;
	}

	if( bReply )
	{
		m_szHmiOperationLogPath = szOperationLogPath;
	}
	else
	{
		m_szHmiOperationLogPath = "";
	}

	if( m_szHmiOperationLogPath.IsEmpty() )
		m_bHmiOperationLogOption = FALSE;

	SaveData();
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 

LONG CWaferTable::GetHmiOperationLogPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if (pAppMod != NULL)
	{
		bReturn = pAppMod->GetPath(m_szHmiOperationLogPath);
	}


	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
	if( bReturn )
	{
	//	m_szHmiOperationLogPath = m_szHmiOperationLogPath + "\\OperationLog.txt";
	}

	if( m_szHmiOperationLogPath.IsEmpty() )
	{
		m_bHmiOperationLogOption = FALSE;
		CString szContent;
		szContent.LoadString(HMB_GENERAL_INVALID_PATH);
		HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);			
	}

	SaveData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


static CString g_aszMcnCstnName[40] =
{
	_T("Rescan die up tolerance"),				//WTMC_RscnPitchUpTol
	_T("PCM Map no die Wafer has limit"),		//WTMC_MWMV_DieLimit
	_T("PCM verify die pass score"),			//WTMC_MWMV_PassScore,
	_T("Adv Offset Map Wafer Verify Limit"),	//WTMC_MWMV_DoLimit
	_T("ADV_DEB_RESCAN idle low limit(min)"),	//WTMC_DAR_IdleLowLmt
	_T("ADV_DEB_RESCAN idle up limit(min)"),	//WTMC_DAR_IdleUpLmt
	_T("Adv Offset Updata Jump Limit"),			//WTMC_AdvUpdUpPcntLmt
	_T("Adv Sample Jump Distance"),				//WTMC_AdvSmpJumpLmt
	_T("Adv Offset Sample Inner Center to Edge"),	//WTMC_AdvSmpCtrToEdge
	_T("Adv Offset Center Sample Frequency"),	//WTMC_AdvSmpCtrFrqc
	_T("Adv Sample Center Verify Base"),		//WTMC_AdvSmpCtrBase
	_T("Prescan Area Pick Confirm T Min Range"),//WTMC_AP_C_MIN_T
	_T("Prescan Area Pick Rotate T Min Range")	//WTMC_AP_R_MIN_T
};
#define	WT_MC_HEADER_NAME		"WaferTable"

#define WT_szMcSMFileName		"C:\\Mapsorter\\EXE\\McnCon.msd"
#define	WT_szMcExImFileName		"C:\\Mapsorter\\EXE\\McnCon.txt"

VOID CWaferTable::ReloadMachineConstant()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CStringMapFile  fMcSMFile;
	BOOL bFileExist = TRUE;
	if (_access(WT_szMcSMFileName, 0) == -1)
	{
		bFileExist = FALSE;
	}
	else
	{
		if (fMcSMFile.Open(WT_szMcSMFileName, FALSE, FALSE)!=1)
		{
			bFileExist = FALSE;
		}
	}

	if( bFileExist )
	{
		for(int i=WTMC_ENUM_START; i<WTMC_ENUM_END; i++)
		{
			m_lMC[i]		= (fMcSMFile)[WT_MC_HEADER_NAME][g_aszMcnCstnName[i]];
		}
		fMcSMFile.Close();
	}
	else
	{
		for(int i=WTMC_ENUM_START; i<WTMC_ENUM_END; i++)
		{
			m_lMC[i]		= pApp->GetProfileInt(gszPROFILE_SETTING, g_aszMcnCstnName[i], m_lMC[i]);
		}
	}

	SaveMachineConstant();
}

VOID CWaferTable::ExportMachineConstant()
{
	CStdioFile fFile;

	if( fFile.Open(WT_szMcExImFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		CString szMsg;
		szMsg.Format("%s%s", WT_szMcExImFileName, " Can not open!");
		HmiMessage_Red_Back(szMsg, "Machine Constant");
		return ;
	}

	CString szData;
	for(int i=WTMC_ENUM_START; i<WTMC_ENUM_END; i++)
	{
		szData.Format("%s,%s,%ld\n", WT_MC_HEADER_NAME, g_aszMcnCstnName[i], m_lMC[i]);
		fFile.WriteString((LPCTSTR) szData);
	}

	fFile.Close();
}

VOID CWaferTable::ImportMachineConstant()
{
	CStdioFile fFile;
	// read DC std file
	if( fFile.Open(WT_szMcExImFileName, CFile::modeRead|CFile::shareDenyNone) )
	{
		CStringArray szaDataList;
		CString szReading;
		fFile.SeekToBegin();
		while( fFile.ReadString(szReading) )
		{
			szaDataList.RemoveAll();
			CUtility::Instance()->ParseRawData(szReading, szaDataList);
			if( szaDataList.GetSize()>=3 && szaDataList.GetAt(0)==WT_MC_HEADER_NAME )
			{
				for(int i=WTMC_ENUM_START; i<WTMC_ENUM_END; i++)
				{
					if( szaDataList.GetAt(1) == g_aszMcnCstnName[i] )
					{
						m_lMC[i] = atol(szaDataList.GetAt(2));
						break;
					}
				}
			}
		}
		fFile.Close();
		SaveMachineConstant();
	}
	else
	{
		HmiMessage("Import file failure.", "Export Machine Data");
	}
}

VOID CWaferTable::SaveMachineConstant()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CStringMapFile  fMcSMFile;

	if( m_lMC[WTMC_RscnPitchUpTol]==0 )
		m_lMC[WTMC_RscnPitchUpTol] = 100;

	if( m_lMC[WTMC_AdvSmpCtrBase]<2 )
	{
		m_lMC[WTMC_AdvSmpCtrBase] = 2;
	}

	if( m_lMC[WTMC_AdvSmpCtrFrqc]<1 )
	{
		m_lMC[WTMC_AdvSmpCtrFrqc] = 1;
	}
	if( pApp->GetCustomerName()==CTM_SEMITEK )
	{
		m_lMC[WTMC_AdvSmpCtrFrqc] = 1;
	}

	if( m_lMC[WTMC_AdvSmpCtrToEdge] <0 && m_lMC[WTMC_AdvSmpCtrToEdge]>100 )
	{
		m_lMC[WTMC_AdvSmpCtrToEdge] = 90;
	}

	if( m_lMC[WTMC_AdvSmpJumpLmt]<30 && pApp->GetCustomerName()==CTM_SEMITEK )
	{
		m_lMC[WTMC_AdvSmpJumpLmt] = 30;
	}

	if (m_lMC[WTMC_AP_R_MIN_T] == 0 || m_lMC[WTMC_AP_R_MIN_T] > 100)
	{
		m_lMC[WTMC_AP_R_MIN_T] = 25;
	}

	if (m_lMC[WTMC_AP_C_MIN_T] == 0 || m_lMC[WTMC_AP_C_MIN_T] > 100)
	{
		m_lMC[WTMC_AP_C_MIN_T] = 50;
	}

	if( m_lMC[WTMC_MWMV_PassScore]==0 || m_lMC[WTMC_MWMV_PassScore]>100 )
	{
		m_lMC[WTMC_MWMV_PassScore] = 75;
	}
	if( m_lMC[WTMC_AdvUpdUpPcntLmt]<150 )
	{
		m_lMC[WTMC_AdvUpdUpPcntLmt] = 150;
	}

	if( fMcSMFile.Open(WT_szMcSMFileName, FALSE, TRUE) )
	{
		for(int i=WTMC_ENUM_START; i<WTMC_ENUM_END; i++)
		{
			(fMcSMFile)[WT_MC_HEADER_NAME][g_aszMcnCstnName[i]]	= m_lMC[i];
		}
		fMcSMFile.Update();
		fMcSMFile.Close();
	}
}

LONG CWaferTable::TestAlertRedYellow(IPC_CServiceMessage& svMsg)
{
	LONG unAlertCode = 0;
	svMsg.GetMsg(sizeof(LONG), &unAlertCode);

	CString szMsg;
	szMsg.Format("Input %d", unAlertCode);

	switch( m_ulAlertType )
	{
	case 0:	//	SetAlert_NoMsg
		SetAlert_NoMsg(unAlertCode, szMsg);
		break;
	case 1:	//	SetAlert
		SetAlert(unAlertCode);
		break;
	case 2:	//	SetAlert_Msg
		SetAlert_Msg_Red_Back(unAlertCode, szMsg);
		break;
	case 3:	//	SetError
		SetError(unAlertCode);
		break;
	case 4:	//	alert and SE
		SetAlert_SE_Red_Yellow(unAlertCode);
		break;
	default:
		HmiMessage("no definition for this type");
		break;
	}

	return 1;
}

BOOL CWaferTable::WaferCornerPatternCheck()
{
	if( m_ucCornerPatternCheckMode==0 )
	{
		return TRUE;
	}
	if( m_ucCornerReferPrID==0 )
	{
		HmiMessage_Red_Back("Corner refer PR ID not set!", "Corner Pattern Check");
		return TRUE;
	}
	if( (m_lReferToCornerDistX==0 && m_lReferToCornerDistY==0) )
	{
		HmiMessage_Red_Back("Corner to refer pattern distance not set!", "Corner Pattern Check");
		return TRUE;
	}

	if( IsMS90HalfSortMode() )
	{
		m_bCornerPatternCheckAll = FALSE;
		if( m_ucCornerPatternCheckMode>2 )
			m_bCornerPatternCheckAll = 2;
	}
	SaveScanTimeEvent("    WFT: corner pattern check wafer orientation.");

	BOOL bResult = TRUE;
	LONG lMinRow = GetMapValidMinRow();
	LONG lMaxRow = GetMapValidMaxRow();
	LONG lMinCol = GetMapValidMinCol();
	LONG lMaxCol = GetMapValidMaxCol();
	LONG lDistX  = ConvertUnitToMotorStep(m_lReferToCornerDistX);
	LONG lDistY  = ConvertUnitToMotorStep(m_lReferToCornerDistY);
	for(int i=1; i<=4; i++)
	{
		LONG lX = 0, lY = 0;
		BOOL bReturn = FALSE;
		CString szTemp;
		switch( i )
		{
		case 1:	// UL
			szTemp = " UL ";
			bReturn = GetDieValidPrescanPosn(lMinRow, lMinCol, 10, lX, lY);
			if( bReturn )
			{
				lX = lX + lDistX;
				lY = lY + lDistY;
			}
			break;
		case 2:	//	UR
			szTemp = " UR ";
			bReturn = GetDieValidPrescanPosn(lMinRow, lMaxCol, 10, lX, lY);
			if( bReturn )
			{
				lX = lX - lDistX;
				lY = lY + lDistY;
			}
			break;
		case 3:	// LR
			szTemp = " LR ";
			bReturn = GetDieValidPrescanPosn(lMaxRow, lMaxCol, 10, lX, lY);
			if( bReturn )
			{
				lX = lX - lDistX;
				lY = lY - lDistY;
			}
			break;
		case 4:	//	LL
			szTemp = " LL ";
			bReturn = GetDieValidPrescanPosn(lMaxRow, lMinCol, 10, lX, lY);
			if( bReturn )
			{
				lX = lX + lDistX;
				lY = lY - lDistY;
			}
			break;
		}


		if( m_ucCornerPatternCheckMode==i || m_bCornerPatternCheckAll )
		{
			CString szMsg = "";
			if( bReturn )
			{
				bReturn = XY_SafeMoveTo(lX, lY);
				if( bReturn )
				{
					Sleep(100);
					bReturn = SearchAndAlignDie(FALSE, FALSE, TRUE, m_ucCornerReferPrID);
					if( m_ucCornerPatternCheckMode==i )
					{
						bReturn = !bReturn;
					}
					if( bReturn==FALSE )
					{
						szMsg = szTemp + "refer die pattern mismatch.";
					}
				}
				else
				{
					szMsg = "SW can not move to" + szTemp + "refer position.";
				}
			}
			else
			{
				szMsg = "SW can not locate" + szTemp + "refer position.";
			}

			if( bReturn==FALSE )
			{
				bResult = FALSE;
				HmiMessage_Red_Back(szMsg, "Corner Pattern Check");
				SetErrorMessage(szMsg);
			}
		}
	}

	return bResult;
}

BOOL CWaferTable::UpdateSMS(CONST BOOL bWaferEnd)
{
	CString szSMSRptFile = m_szPrescanLogPath + ".sms";	// after prescan/stop, delete the file, then update file
	DeleteFile(szSMSRptFile);

	if( bWaferEnd==FALSE && IsPrescanEnable() && m_bSmallMapSortRpt && m_bGenRptOfSMS && 
		CPreBondEvent::m_nMapOrginalCount<=SCAN_SMS_DIE_MAX_LIMIT)
	{
		CStdioFile cfFile;
		if (cfFile.Open(szSMSRptFile, 
					CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText))
		{
			cfFile.SeekToEnd();
			ULONG ulMaxRow = 0, ulMaxCol = 0;
			m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
			UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
			for(ULONG ulRow=0; ulRow<=ulMaxRow; ulRow++)
			{
				for(ULONG ulCol=0; ulCol<=ulMaxCol; ulCol++)
				{
					if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) )
					{
						continue;
					}
					if (IsMapNullBin(ulRow, ulCol) ||
						GetMapDieState(ulRow, ulCol)==WT_MAP_DS_UNPICK_REGRAB_EMPTY 	||
						GetMapDieState(ulRow, ulCol)==WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY )
					{
						CString szData;
						LONG lUserRow = 0, lUserCol = 0;
						ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
						szData.Format("%ld,%ld,%lu,%lu\n", lUserRow, lUserCol, ulRow, ulCol);
						cfFile.WriteString(szData);
					}
				}
			}
			cfFile.Close();
		}
	}

	return TRUE;
}

ULONG CWaferTable::GetScanMethod()
{
	return m_ulPrescanMethod;
}

LONG CWaferTable::GetAlarmLogPath(IPC_CServiceMessage& svMsg)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if (pAppMod != NULL)
	{
		pAppMod->GetPath(m_szAlarmLogPath);
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	ConfirmAlarmLogSettings(svMsg);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CWaferTable::LoadAllMapNewReferDie(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;

	m_lReferMapTotal	= 0;
	memset(m_lReferMapRow, 0, sizeof(m_lReferMapRow));
	memset(m_lReferMapCol, 0, sizeof(m_lReferMapCol));

	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;
	static char szFilters[]= "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.*", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pMainWnd, 0);
	dlgFile.m_ofn.lpstrInitialDir = gszUSER_DIRECTORY;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	if ( nReturn != IDOK )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szFilename = dlgFile.GetPathName();	// full path and name and ext. best.
	SaveScanTimeEvent("Load new refer file " + szFilename);

	if ((_access(szFilename, 0 )) == -1)
	{
		HmiMessage_Red_Back("Select file can not access!", "Scan check Wafer");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szReading = "";
	CString szMsg = "", szTemp;

	CStdioFile fScan;
	if( fScan.Open(szFilename, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		HmiMessage_Red_Back("Select file can not be open", "Scan Check Wafer");
		return FALSE;
	}

	fScan.SeekToBegin();
	while( fScan.ReadString(szReading)!=NULL )
	{
		CStringArray szaRawData;
		szaRawData.RemoveAll();
		CUtility::Instance()->ParseRawData(szReading, szaRawData);
		if( szaRawData.GetSize()>=2 )
		{
			LONG lScanRow = (LONG)atoi( szaRawData.GetAt(0) );
			LONG lScanCol = (LONG)atoi( szaRawData.GetAt(1) );

			m_lReferMapRow[m_lReferMapTotal] = lScanRow;
			m_lReferMapCol[m_lReferMapTotal] = lScanCol;
			m_lReferMapTotal++;

			szTemp.Format("[%d,%d]", lScanRow, lScanCol);
			szMsg += szTemp;
		}
		if( m_lReferMapTotal>=(WT_ALN_MAXCHECK_SCN-1) )
		{
			break;
		}
	}
	fScan.Close();

	szTemp.Format("Auto Load new refer Setting File total points %d", 	m_lReferMapTotal);
	szMsg += szTemp;
	SetAlarmLog(szMsg);

	SaveData();
	SaveWaferTblData();

	HmiMessage("new refer File loading complete");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	load new added refer die list file


LONG CWaferTable::ConfirmAlarmLogSettings(IPC_CServiceMessage& svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( m_szAlarmLogPath.IsEmpty() )
	{
		m_szAlarmLogPath = gszUSER_DIRECTORY + "\\Alarm Log";
	}

	pApp->WriteProfileInt(gszPROFILE_SETTING,	_T("Enable Alarm Log"),		m_bEnableAlarmLog);
	pApp->WriteProfileInt(gszPROFILE_SETTING,	_T("ACF Alarm Log Path"),	m_bACF_AlarmLogPath);
	pApp->WriteProfileString(gszPROFILE_SETTING,_T("Alarm Log Path"),		m_szAlarmLogPath);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
//three to registry.

LONG CWaferTable::PrintScreenCmd(IPC_CServiceMessage& svMsg)
{
	CTime stTime = CTime::GetCurrentTime();
	CString szScreenPath = gszUSER_DIRECTORY + "\\PrescanResult\\PrintScren";
	CreateDirectory(szScreenPath, NULL);
	CString szScreenTime = stTime.Format("_%y%m%d%H%M%S.JPG");
	CString szScreenName = szScreenPath + "\\PS_" + GetMachineNo() + szScreenTime;
	PrintScreen(szScreenName);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SelectCheckCenterDie(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	if( IsPrescanEnable()==FALSE || IsPrescanEnded()==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (CMS896AApp::m_bMS100Plus9InchOption && IsEJTAtUnloadPosn())
	{
		SetAlert_Red_Yellow(IDS_WT_BL_NOT_IN_SAFE_POS);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	ULONG ulRow = 0, ulCol = 0;
	m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	LONG encT = pUtl->GetRealignGlobalTheta();
	LONG encX = 0, encY = 0;
	BOOL ret = GetDieValidPrescanPosn(ulRow, ulCol, 3, encX, encY);

	if( ret && IsWithinWaferLimit(encX, encY, 1, FALSE) )
	{
		SetJoystickOn(FALSE);
		GetEncoderValue();
		if( abs(GetCurrT()-encT)>=5 )
		{
			T_MoveTo(encT, SFM_NOWAIT);
		}

		XY_SafeMoveTo(encX, encY, FALSE);
		T_Sync();
		SetJoystickOn(TRUE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

VOID CWaferTable::ChangeRegionGrade(LONG lState)
{
	if( m_bChangeRegionGrade && m_ulGrade151Total!=-1 )
	{
		ULONG ulLeft = 0, ulPicked = 0, ulTotal = 0;
		UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
		m_WaferMapWrapper.GetStatistics(151+ucOffset, ulLeft, ulPicked, ulTotal);
		if( lState==0 )
		{
			m_ulGrade151Total = ulTotal;
		}
		else if( lState==1 )
		{
			if( m_ulGrade151Total!=ulTotal )
			{
				m_ulGrade151Total = -1;
			}
		}
	}	//	SanAn TJ, after prescan, change region grade.
}

BOOL CWaferTable::CheckEjectorPinLifeB4Start(BOOL bAlign)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();

	if( bAlign )
	{
		m_lPrescanSortingTotal = m_WaferMapWrapper.GetSelectedTotalDice();
		if( pUtl->GetPrescanRegionMode() || IsMS90HalfSortMode() )
		{
			m_lPrescanSortingTotal	= 0; 
			CUIntArray aulSelGradeList;
			m_WaferMapWrapper.GetSelectedGradeList(aulSelGradeList);
/*
			LONG lEdgeMinRow = GetMapValidMinRow();
			LONG lEdgeMaxRow = GetMapValidMaxRow();
			LONG lEdgeMinCol = GetMapValidMinCol();
			LONG lEdgeMaxCol = GetMapValidMaxCol();
			if( IsMS90HalfSortMode() )
			{
				lEdgeMaxRow = GetMS90HalfMapMaxRow()-1;
			}
*/
			LONG lEdgeMinRow = 0, lEdgeMaxRow = 0;
			LONG lEdgeMinCol = 0, lEdgeMaxCol = 0;
			GetMapValidSize(lEdgeMaxRow, lEdgeMinRow, lEdgeMaxCol, lEdgeMinCol);
			if( IsMS90HalfSortMode() )
			{
				lEdgeMaxRow--;
			}

			if( pUtl->GetPrescanRegionMode() )
			{
				ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
				ULONG lULRow, lULCol, lLRRow, lLRCol;
				if( pSRInfo->GetRegion(ulTgtRegion, lULRow, lULCol, lLRRow, lLRCol) )
				{
					lEdgeMinRow = max((LONG)lULRow,		lEdgeMinRow);
					lEdgeMaxRow = min((LONG)lLRRow - 1, lEdgeMaxRow);
					lEdgeMinCol = max((LONG)lULCol,		lEdgeMinCol);
					lEdgeMaxCol = min((LONG)lLRCol - 1, lEdgeMaxCol);
				}
			}

			UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
			for(LONG ulRow=lEdgeMinRow; ulRow<=lEdgeMaxRow; ulRow++)
			{
				for(LONG ulCol=lEdgeMinCol; ulCol<=lEdgeMaxCol; ulCol++)
				{
					UCHAR ucPickGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
					if( ucPickGrade==ucNullBin )
					{
						continue;
					}
					for(int j=0; j<aulSelGradeList.GetSize(); j++)
					{
						if( ucPickGrade==aulSelGradeList.GetAt(j) )
						{
							m_lPrescanSortingTotal++;
							break;
						}
					}
				}
			}
		}
	}

	// Check the Ejector LiftTime + MapCount is exceed the limit
	if( pApp->GetFeatureStatus(MS896A_FUNC_EJ_REAPLCE_AT_WAFERSTART) )
	{
		if ( !WTCheckEjtLife() )
		{
			if( pApp->GetCustomerName() == "Electech3E(DL)" )
			{
				SetAlert_Red_Yellow(IDS_BH_REPLACE_EJECTOR);
				SetErrorMessage("Checked EJ pin lifetime and replacement");
			}
			else
			{
				CString szContent, szTitle;
				szContent = "This Map exceed the ejector lifetime Limit! Please load another map!";
				CMSLogFileUtility::Instance()->WL_LogStatus(szContent);
				SetErrorMessage(szContent);
				szContent = "This Map exceed the ejector lifetime Limit! \nPlease load another map!";
				szTitle.LoadString(HMB_WT_ALIGN_WAFER);
				HmiMessage_Red_Back(szContent, szTitle);
			}
			return FALSE;
		}	
	}

	return TRUE;
}


BOOL CWaferTable::PrescanGoodDiePercentageCheck()
{
	CString szMsg, szTitle;
	DOUBLE dGoodDiePercentage;

	ULONG ulEmptyDie	= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY];
	ULONG ulDefectDie	= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT];
	ULONG ulBadCutDie	= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT];
	ULONG ulGoodDie		= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD];
	ULONG ulTotalDie	= ulGoodDie + ulBadCutDie + ulDefectDie + ulEmptyDie;

	if (ulTotalDie == 0)
	{
		return TRUE;
	}

	dGoodDiePercentage = ((DOUBLE)ulGoodDie / (DOUBLE)ulTotalDie) * 100;
	if (dGoodDiePercentage <= 10)
	{
		//szTitle = "Good Die Percentage check";
		szMsg = "Prescan Good Die Percentage check fail.";
		//HmiMessage_Red_Back(szMsg, szTitle);
		SetErrorMessage(szMsg);
		return FALSE;
	}
	
	return TRUE;
}