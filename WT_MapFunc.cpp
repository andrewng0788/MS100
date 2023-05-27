#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "io.h"
#include "WaferPr.h"
#include "WaferLoader.h"
#include "MS_SecCommConstant.h"
#include "Utility.H"
#include "BinTable.h"
#include "MarkDieRegionManager.h"
#include <direct.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CWaferTable::UpdateAllSGVariables()
{
	if (!IsSecsGemInit()) 
	{
		return;
	}

	SendWaferMapSelectedGradeList(FALSE);
}

//Cross check functions
LONG CWaferTable::SetupReferenceCross(VOID)
{
	ULONG ulInternalRow = 0;
	ULONG ulInternalCol = 0;


	if (m_WaferMapWrapper.IsDieInMap_User(m_lRefCrossRow, m_lRefCrossCol) == FALSE)
	{
		return 0;		//Return out map limit
	}


	m_WaferMapWrapper.ConvertUserToInternal(m_lRefCrossRow, m_lRefCrossCol, ulInternalRow, ulInternalCol);
	m_WaferMapWrapper.SetAlgorithmParameter("Center Row", (LONG)ulInternalRow);
	m_WaferMapWrapper.SetAlgorithmParameter("Center Column", (LONG)ulInternalCol);

	if (m_lRefCrossWidth == 0)
	{
		m_WaferMapWrapper.SetAlgorithmParameter("Cross Width", 0);
	}
	else
	{
		m_WaferMapWrapper.SetAlgorithmParameter("Cross Width", (m_lRefCrossWidth * 2) - 1);
	}

	m_WaferMapWrapper.SetAlgorithmParameter("Take Cross", TRUE);

	return 1;
}


BOOL CWaferTable::MarkCMLTGrade99DicesInResortMap()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "CMLT")
		return FALSE;
	if (m_WaferMapWrapper.GetReader() == NULL) 
		return FALSE;
	if (!m_WaferMapWrapper.IsMapValid())
		return FALSE;

//AfxMessageBox("CMLT MarkCMLTGrade99DicesInResortMap", MB_SYSTEMMODAL);

	INT nIsOddRCol=0;
	CStdioFile oFile;
	CString szLine;
	CString szElectricInfo, szCurrWaferID;
	CString szLastWaferID = "";
	BOOL bMarkNextDieToGrade101 = FALSE;
	LONG lLastRow=0, lLastCol=0;


	unsigned long ulNumOfRows=0, ulNumOfCols=0;
	m_WaferMapWrapper.GetMapDimension(ulNumOfRows, ulNumOfCols);
	
	if (!oFile.Open(GetMapFileName(), CFile::modeRead|CFile::shareDenyNone))
		return FALSE;

	for (INT j = 0; j < (INT)ulNumOfCols; j++)
	{
		nIsOddRCol = j%2;			//Klocwork	

		if (nIsOddRCol == 0)		//Even row: 0, 2, 4, 6, ....
		{
			for (INT i = ulNumOfRows - 1; i >= 0; i--)
			{
				if (m_pWaferMapManager->IsMapHaveBin(i, j))
				{
					m_WaferMapWrapper.GetExtraInformation(&oFile, i, j, szElectricInfo);
					szCurrWaferID = szElectricInfo.Left(szElectricInfo.Find(","));

					if (bMarkNextDieToGrade101)
					{
						m_WaferMapWrapper.ChangeGrade(i, j, 101 + m_WaferMapWrapper.GetGradeOffset());
						bMarkNextDieToGrade101 = FALSE;
						szLastWaferID = szCurrWaferID;
						lLastRow = i;
						lLastCol = j;
						continue;
					}

					if ((szLastWaferID != "") && (szLastWaferID != szCurrWaferID))
					{
						m_WaferMapWrapper.ChangeGrade(i, j, 101 + m_WaferMapWrapper.GetGradeOffset());
						m_WaferMapWrapper.ChangeGrade(lLastRow, lLastCol, 101 + m_WaferMapWrapper.GetGradeOffset());
						bMarkNextDieToGrade101 = TRUE;
					}

					szLastWaferID = szCurrWaferID;
					lLastRow = i;
					lLastCol = j;
				}
			}
		}
		else						//Else ODD row: 1, 3, 5, 7, .....
		{
			for (INT i = 0; i < (INT)ulNumOfRows; i++)
			{
				if (m_pWaferMapManager->IsMapHaveBin(i, j))
				{
					m_WaferMapWrapper.GetExtraInformation(&oFile, i, j, szElectricInfo);
					szCurrWaferID = szElectricInfo.Left(szElectricInfo.Find(","));

					if (bMarkNextDieToGrade101)
					{
						m_WaferMapWrapper.ChangeGrade(i, j, 101 + m_WaferMapWrapper.GetGradeOffset());
						bMarkNextDieToGrade101 = FALSE;
						szLastWaferID = szCurrWaferID;
						lLastRow = i;
						lLastCol = j;
						continue;
					}

					if ((szLastWaferID != "") && (szLastWaferID != szCurrWaferID))
					{
						m_WaferMapWrapper.ChangeGrade(i, j, 101 + m_WaferMapWrapper.GetGradeOffset());
						m_WaferMapWrapper.ChangeGrade(lLastRow, lLastCol, 101 + m_WaferMapWrapper.GetGradeOffset());
						bMarkNextDieToGrade101 = TRUE;
					}

					szLastWaferID = szCurrWaferID;
					lLastRow = i;
					lLastCol = j;
				}
			}
		}
	}

	oFile.Close();
	return TRUE;
}


BOOL CWaferTable::DieIsNoPick(ULONG ulRow, ULONG ulCol)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( (GetKeepScnAlignDie() || pUtl->GetMarkDieUnpick()) && !m_bPickCrossCheckDie )
	{
		if ( m_WaferMapWrapper.IsMapValid() && (m_WaferMapWrapper.GetReader() != NULL) )
		{
			BOOL bDieIsMarked = m_WaferMapWrapper.GetReader()->IsMarkedDie(ulRow, ulCol);

			if( bDieIsMarked )
			{
				CString szLogText;
				szLogText.Format("Mark Die(%d,%d), state %d)",ulRow, ulCol, m_WaferMapWrapper.GetDieState(ulRow, ulCol));
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
			}

			return bDieIsMarked;
		}
	}

	return FALSE;
}


BOOL CWaferTable::DieIsSpecialRefGrade(ULONG ulRow, ULONG ulCol)
{
	if (!m_WaferMapWrapper.IsMapValid())
		return FALSE;
	if (m_WaferMapWrapper.GetReader() == NULL)
		return FALSE;
	if (m_lSpecialRefDieGrade <= 0)
		return FALSE;

	//If current grade equals special REF_DIE grade set by user
	UCHAR ucGrade = m_WaferMapWrapper.GetReader()->GetDieInformation(ulRow, ulCol) - m_WaferMapWrapper.GetGradeOffset();
	if (ucGrade == m_lSpecialRefDieGrade)
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CWaferTable::ResetNoPickDieState(VOID)
{
	ULONG ulRow, ulMaxRow=0; 
	ULONG ulCol, ulMaxCol=0;

	if ( (m_WaferMapWrapper.IsMapValid() == TRUE) && (m_WaferMapWrapper.GetReader() != NULL) )
	{
		if (!m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol))
		{
			return FALSE;
		}
	
		for (ulRow=0; ulRow<ulMaxRow; ulRow++)
		{
			for (ulCol=0; ulCol<ulMaxCol; ulCol++)
			{
				ULONG ulDieState = GetMapDieState(ulRow, ulCol);
				if( !IsDieUnpickScan(ulDieState) )
				{
					if ( m_WaferMapWrapper.GetReader()->IsMarkedDie(ulRow, ulCol) == TRUE ) 
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, FALSE);
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_DEFAULT);
					}
				}
			}
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}


LONG CWaferTable::SetupSubRegionMode(BOOL bLoadMap, BOOL bSecondHalfSortMode)
{
	ULONG ulMaxRow=0, ulMaxCol=0;
	int nCol = 0;
	CString szAlgorithm, szPathFinder;
	BOOL bIsRefCross = FALSE;
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	//v3.70T4	//PLLM special feature
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);


	if (!m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol))
	{
		return FALSE;
	}


	LONG ulSubCols = pSRInfo->GetSubCols();
	if (bLoadMap && pUtl->GetPrescanRegionMode() && (ulSubCols > 1 ) && (ulMaxCol % 2 == 1))
	{
		CString szMess;
		szMess = "Can not support the odd column number of wafer map for region mode\n if the column number of region is more than 1";
		HmiMessage_Red_Yellow(szMess, "Region Mode Setting");
		pSRInfo->m_ulSubCols = 1;
	}

	if (IsOutMS90SortingPart(ulMaxRow, ulMaxCol))
	{
		if (IsRowModeSeparateHalfMap())
		{
			ulMaxRow = GetMS90HalfMapMaxRow();
		}
		else
		{
			//ulMaxCol = GetMS90HalfMapMaxCol();
		}
	}

	//set wafer map under user control or not
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
//	if (pSRInfo->IsRegionPickingMode() && !pUtl->GetPrescanRegionMode() && (szAlgorithm.Find("Sorting Path 1") != -1))
//	{
//		m_pWaferMapManager->SetUserRegionPickingMode(TRUE);
//	}
//	else
	{
		m_pWaferMapManager->SetUserRegionPickingMode(FALSE);
	}


	nCol = szAlgorithm.Find("(Reference Cross)");
	if (nCol != -1 || pUtl->GetPrescanRegionMode() || GetRegionSortOuter() || pSRInfo->IsRegionPickingMode())
	{
		m_pWaferMapManager->DeleteAllBoundary();
		bIsRefCross = TRUE;
	}
	else if (szAlgorithm.Find("(Cluster)") != -1)			//v4.13T5	//SUpport By-Region Cluster mode
	{
		m_pWaferMapManager->DeleteAllBoundary();
		bIsRefCross = TRUE;
	}
	else if (szAlgorithm.Find("Continuous Pick") != -1)		//v4.16T2	//SUpport By-Region Cluster mode
	{
		m_pWaferMapManager->DeleteAllBoundary();
		bIsRefCross = TRUE;
	}

	if ((pUtl->GetPrescanRegionMode() || pSRInfo->IsRegionPickingMode() && m_pWaferMapManager->IsUserRegionPickingMode()) &&
		szAlgorithm.Find("Sorting Path 1") != -1)
	{
		//only sorting the first region
		m_WaferMapWrapper.SetAlgorithmParameter("Only First Region", TRUE);
	}
	else
	{
		//sorting the whole map
		m_WaferMapWrapper.SetAlgorithmParameter("Only First Region", FALSE);
	}


	//Set the time of sorting path
	if (szAlgorithm.Find("Sorting Path") != -1)
	{
		LONG lTime = 20;

		BOOL bUpdate	= FALSE;
		//UINT unCalTime	= pApp->GetProfileInt(gszPROFILE_SETTING, _T("Sorting Path Cal Time"), 0);
		UINT unCalTime	= pApp->GetFeatureValue(MS896A_FUNC_SORTING_PATH_CAL_TIME);
		if ((unCalTime >= 20) && (unCalTime <= 7200))	//in sec? max is 2 hrs = 7200sec
		{
			bUpdate = TRUE;
			lTime = unCalTime;		//v4.57A15
		}

		if (bUpdate)
		{
			CString szLog;
			szLog.Format("WT: SetupSubRegionMode: Sorting Path - BUILT Time = %d sec", lTime);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			m_WaferMapWrapper.SetAlgorithmParameter("Cal Time", lTime);
		}
	}

	//v4.58A5
	if (szAlgorithm.Find("Sorting Path 1 Offline") != -1)
	{
		pApp->ExecuteOfflineMapProgram();
	}


	if (!bIsRefCross)						//For other customers		//v4.22T8
	{
		nCol = szAlgorithm.Find("Shortest");
		if ( nCol != -1 )
		{
			m_pWaferMapManager->DeleteAllBoundary();
			bIsRefCross = TRUE;
		}
	}

	//calculate the size of region
	LONG lSubRowsInRegion = 0, lSubColsInRegion;
	pSRInfo->GetRegionRowColumnNum(szAlgorithm, ulMaxRow, ulMaxCol, lSubRowsInRegion, lSubColsInRegion);

	//Define Mark Die Region
	WM_CMarkDieRegionManager *pMarkDieRegionManager = WM_CMarkDieRegionManager::Instance();
	pMarkDieRegionManager->DefineMarkDieRegion(ulMaxRow, ulMaxCol);

	//Draw Wafermap boundary on wafemap control
	LONG lSubHeight = lSubRowsInRegion;
	LONG lSubWidth = lSubColsInRegion;

//	LONG lMapSubRows = lSubRows;
//	if (IsMS90HalfSortMode() && pUtl->GetPrescanRegionMode() && lMapSubRows != 1)
//	{
//		lSubHeight = lSubHeight * 2;
//	}
//52,20->104,20
//for 2X2
//for half sort mode, it should set as row(#2), column(#1)
//if disable the half sort mode, it should be set as row(#2), column(#2).
	pUtl->SetRegionPickMode(FALSE);

	if ((m_lSubSortMode == 0) && 
		(IsAOIOnlyMachine() == FALSE || pUtl->GetPrescanRegionMode() || GetRegionSortOuter() || pSRInfo->IsRegionPickingMode() && m_pWaferMapManager->IsUserRegionPickingMode()))
	{
		//it will picked a grade one by one region
		m_pWaferMapManager->SetRegionAlgorithmParameter(lSubHeight, lSubWidth, TRUE);
		if (pUtl->GetPrescanRegionMode() == FALSE && GetRegionSortOuter() == FALSE)
		{
			pUtl->SetRegionPickMode(TRUE);
			RegionPickLog("Region Pick Mode enable");
			pSRInfo->AssignRegionOuterList(FALSE);
			CString szTotal, szMsg;
			for(ULONG i=0; i<pSRInfo->GetTotalSubRegions(); i++)
			{
				i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5

				ULONG ulSubRow, ulSubCol;
				ULONG ulRegion = pSRInfo->GetSortRegion(i);
				if( pSRInfo->GetRegionSub(ulRegion, ulSubRow, ulSubCol) )
				{
					szMsg.Format("%2d[%d,%d]    ", ulRegion, ulSubRow, ulSubCol);
					szTotal +=szMsg;
				}
			}
			RegionPickLog(szTotal);
		}
	}
	else
	{
		//it will picked all grades in the current region and move to the next region
		m_pWaferMapManager->SetRegionAlgorithmParameter(lSubHeight, lSubWidth, FALSE);
	}


	if (IsScnLoaded() && m_lScnAlignMethod==3 && (pSRInfo->GetSubRows() != 1 || pSRInfo->GetSubCols() != 1))
	{
		bIsRefCross = TRUE;
	}

	//	sub region info, initialization
	if (bLoadMap)
	{
		m_bFirstRegionInit = FALSE;
	}

	if (IsAOIOnlyMachine()==FALSE && (bLoadMap || m_bFirstRegionInit))	// init sub region
	{
		m_pWaferMapManager->SetupRegionSize(ulMaxRow, ulMaxCol, lSubHeight, lSubWidth, bSecondHalfSortMode,
											pUtl->GetPrescanRegionMode(), GetRegionSortOuter(), m_bUseEmptyCheck, m_lMnNoDieGrade);
	}


	if (pSRInfo->IsRegionPickingMode())
	{
		m_pWaferMapManager->DrawAllRegionBoundary();
	}

	// init sub region
	if ((pUtl->GetPrescanRegionMode() || GetRegionSortOuter()) && (bLoadMap || m_bFirstRegionInit))
	{
		//	set pick list, get home die region, set this region state AS HOME DIE REGION
		UCHAR ucRgnOrder	= 1;	// 0, far first; 1, sequencial TLH order; 2, BRH
		CString szMsg, szTotal;
		ULONG ulHomeRow = 0, ulHomeCol = 0, ulSubRow = 0, ulSubCol = 0;
		GetMapAlignHomeDie(ulHomeRow, ulHomeCol);	//	second part be careful
		if (GetRegionSortOuter())
		{
			ulHomeRow = ulMaxRow/2;
			ulHomeCol = ulMaxCol/2;
		}

		ULONG ulRegion = pSRInfo->GetWithinRegionNo(ulHomeRow, ulHomeCol);
		if (GetRegionSortOuter() == FALSE)
		{
			if (IsBlkPickAlign())
			{
				ulRegion = pSRInfo->GetTotalSubRegions();
				if (m_bRegionBlkPickOrder)
				{
					ulRegion = (pSRInfo->GetSubRows() - 1) / 2 * pSRInfo->GetSubCols() + pSRInfo->GetSubCols() / 2 + 1;
					if (pSRInfo->IsInvalidRegion(ulRegion))
					{
						ulRegion = pSRInfo->GetTotalSubRegions();
					}
				}
				else
				{
					ucRgnOrder = 0;
				}
			}
		}

		if (pSRInfo->SetHomeDieRegion(ulRegion) == FALSE)
		{
			pSRInfo->SetInitState(WT_SUBREGION_INIT_ERROR);
			pSRInfo->SetHomeDieRegion(1);
			szMsg.Format("Home region not valid %d (%d,%d)", ulRegion, ulHomeRow, ulHomeCol);
		}
		else
		{
			szMsg.Format("Home Region %d, (%d,%d)", ulRegion, ulHomeRow, ulHomeCol);
		}
		RegionPickLog(szMsg);

		// set the bonding sequance based on the home die region,
		// the home die region is last and the farest is first, 
		if (GetRegionSortOuter())
		{
			pSRInfo->AssignRegionOuterList(TRUE);
			ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
			pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDING);
		}
		else
		{
			if (pApp->GetCustomerName() == "Semitek")
			{
				ucRgnOrder = 2;
			}
			pSRInfo->AssignRegionPickList(ucRgnOrder);
		}

		// set algorithm for region prescan
		CString szAgtm, szPath;
		m_WaferMapWrapper.GetAlgorithm(szAgtm, szPath);	// Region scan, only Custom Order Region or short path.
		if (szAgtm.Find("Custom Order Region") == -1 && szAgtm.Find("Sorting Path 1") == -1)
		{
			szAgtm = "Custom Order Region";
			if ((pSRInfo->GetSubRows() == 1) && (pSRInfo->GetSubCols() == 1))
			{
				szAgtm = "TLH";
			}
			m_WaferMapWrapper.SelectAlgorithm(szAgtm, "Direct");
		}
		else
		{
			if (szAgtm.Find("Custom Order Region") != -1)
			{
				if ((pSRInfo->GetSubRows() == 1) && (pSRInfo->GetSubCols() == 1))
				{
					szAgtm = "TLH";
					m_WaferMapWrapper.SelectAlgorithm(szAgtm, "Direct");
				}
			}
		}

		szTotal = m_pWaferMapManager->SetRegionPickingSequence(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(), GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol(), 0, FALSE);

		if (m_WaferMapWrapper.GetReader() != NULL)
		{
			SaveScanTimeEvent("WFT: set custom tex and region mode; short");
			if (szAgtm.Find("Sorting Path 1") != -1)
				m_WaferMapWrapper.SetAlgorithmParameter("Custom Order", TRUE);
			else
				m_WaferMapWrapper.SetAlgorithmParameter("Custom Order", FALSE);
			szMsg.Format("region %s:%s IN_GRADE_ORDER ", szAgtm, szTotal);
			RegionPickLog(szMsg);
		}

		m_WaferMapWrapper.SetPickMode(WAF_CDieSelectionAlgorithm::IN_GRADE_ORDER);
		if (m_bFirstRegionInit)
		{
			pSRInfo->SetTargetRegionIndex((ULONG)m_lRegionPickIndex);	// SW restart and wafer bonded before
			for (ULONG i = 0; i < WT_MAX_SUBREGIONS_LIMIT; i++)
			{
				pSRInfo->SetRegionState(i+1, m_lSubRegionState[i]);
			}
			RegionPickLog("application restart without load map");
		}

		SaveRegionStateAndIndex();
		szMsg.Format("Region Sorting List:%s", pSRInfo->m_szRegionOrderList_HMI);
		RegionPickLog(szMsg);
		szMsg.Format("Region Sorting Stat:%s", pSRInfo->m_szRegionStateList_HMI);
		RegionPickLog(szMsg);
	}
	else
	{
		//reset customer order if change from region pick mode to wafer whole mode
		m_WaferMapWrapper.SetAlgorithmParameter("Custom Order", FALSE);
	}

	m_bFirstRegionInit	= FALSE;

	if ( (bPLLMSpecialFcn && (nPLLM == PLLM_REBEL)) ||		//xyz	//v3.77
		 (pApp->GetProductLine() == "Rebel") )				//v4.46T20	
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("WAFERMAP: PLLM REBEL Sub-Region mode is used");	
		m_WaferMapWrapper.SetAlgorithmParameter("Check Sub Ref", FALSE);	
		
		//v4.46T18
		m_WaferMapWrapper.SetAlgorithmParameter("Check Ref For Each Grade", FALSE);		//Ref-die checking-per-grade option under Ref-Cross sub-region mode

		//v3.87		//SHortest Path mapping fcn for PLLM REBEL only
		if (szAlgorithm.Find("Shortest") != -1)
		{
			m_WaferMapWrapper.SetAlgorithmParameter("Distance Criteria", 5);
		}
	}
	else if (pApp->GetProductLine() == "DieFab")	//v4.46T20
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("WAFERMAP: PLLM DieFab Sub-Region mode is used");	
		m_WaferMapWrapper.SetAlgorithmParameter("Check Sub Ref", TRUE);	
		m_WaferMapWrapper.SetAlgorithmParameter("Check Ref For Each Grade", TRUE);	//Ref-die checking-per-grade option under Ref-Cross sub-region mode
	}
	else
	{
		if( pApp->GetCustomerName()=="Lumileds" && m_bMultiWaferSupport )
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("WAFERMAP: Sub-Region mode #1 is used");		//v3.93
			m_WaferMapWrapper.SetAlgorithmParameter("Check Sub Ref", TRUE);
			m_WaferMapWrapper.SetAlgorithmParameter("Check Ref For Each Grade", FALSE);		//v4.14T8	//As suggested by Sammy	
		}
		else if( pApp->GetCustomerName()=="OSRAM" && m_bMultiWaferSupport )
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("WAFERMAP: Sub-Region mode REFER CHECK is used");		//v3.93
			m_WaferMapWrapper.SetAlgorithmParameter("Check Sub Ref", TRUE);
			m_WaferMapWrapper.SetAlgorithmParameter("Check Ref For Each Grade",	FALSE);	//	TRUE TESTED OK BUT As suggested by Sammy, IT SHOULD BE FALSE
		}
		else if( pApp->GetCustomerName() == "Avago")	//v4.48A33
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("WAFERMAP: Sub-Region mode (Avago) REFER CHECK is used");		//v3.93
			m_WaferMapWrapper.SetAlgorithmParameter("Check Sub Ref", TRUE);
			m_WaferMapWrapper.SetAlgorithmParameter("Check Ref For Each Grade",	FALSE);	
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("WAFERMAP: Sub-Region mode #2 is used");		//v3.93
			m_WaferMapWrapper.SetAlgorithmParameter("Check Sub Ref", FALSE);
		}
	}


	//v2.95T1
	if (szAlgorithm.Find("Clockwise Spiral") != -1)
	{
		BOOL bSubVert = FALSE;
		LONG lPitchX = labs(GetDiePitchX_X());
		LONG lPitchY = labs(GetDiePitchY_Y());
		LONG lDiffXY = labs(lPitchX - lPitchY);
		switch( m_ucSpiralPickDirection )
		{
		case 1:	//	YV
			bSubVert = TRUE;
			break;
		case 3:	//	Auto
			if( lPitchX>lPitchY && lDiffXY>=(lPitchY*20/100) )
			{
				bSubVert = TRUE;
			}
			break;
		case 0:	//	default:
		case 2:
		default:
			break;
		}

		m_WaferMapWrapper.SetAlgorithmParameter("Sub Vertical", bSubVert);
	}

	//BLOCKPICK
	if ( m_bFullRefBlock == FALSE )
	{
		nCol = szAlgorithm.Find("Block");
		if ( nCol != -1)
		{
			m_pBlkFunc->SetRefDieArray(lSubColsInRegion, lSubColsInRegion);
		}
	}

	//UpdateSortingMode();
	return 1;
}


VOID CWaferTable::RotateMap(VOID)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDummyMap()==FALSE && (m_ucMapRotation > 0) )
	{
		//1 = 90; 2 = 180; 3 = 270
		m_WaferMapWrapper.Rotate((unsigned short)90*m_ucMapRotation);
	}
}


VOID CWaferTable::FlipMap(VOID)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDummyMap()==FALSE )
	{
		if (m_bMapHoriFlip == TRUE)
		{
			m_WaferMapWrapper.HorizontalFlip();
		}

		if (m_bMapVertFlip == TRUE)
		{
			m_WaferMapWrapper.VerticalFlip();
		}
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( IsAOIOnlyMachine() && pUtl->GetPrescanDummyMap() &&
		(pApp->GetCustomerName()=="FATC" || m_szScanAoiMapFormat==_T("EpiTech")) &&
		( m_lMa3MapDirX!=0 || m_lMa3MapDirY!=0 ) )
	{
		LONG lRow = 0, lCol = 0;
		if( m_WaferMapWrapper.GetReader()!=NULL )
		{
			lCol = m_WaferMapWrapper.GetReader()->GetOriginalHomeColumn();
			lRow = m_WaferMapWrapper.GetReader()->GetOriginalHomeRow();
		}
		m_WaferMapWrapper.SetHomePosition(lRow, lCol);
		if( m_lMa3MapDirY!=0 )
			m_WaferMapWrapper.SetVerticalDirection(TRUE);
		if( m_lMa3MapDirX!=0 )
			m_WaferMapWrapper.SetHorizontalDirection(TRUE);
	}
}



//================================================================
// Function Name: 		IsASCIIGradeDisplayMode
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the Grade Display Mode
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferTable::IsASCIIGradeDisplayMode()
{
	//0 : "Sequential"
	//1 : "ASCII"
	//2 : "Character"
	//3 : "Original"
	//4 : "Char+ASCII"
	//5 : "HEX"
	//6 : "Original Only"
	return ((m_lGradeDisplayMode == 1) || (m_lGradeDisplayMode == 5));
}

//================================================================
// Function Name: 		GetWaferMapSelectedGradeList
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the wafer map selected grade List
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferTable::GetWaferMapSelectedGradeList(CUIntArray &aulSelectdGradeList, CUIntArray &aulSelectdGradeLeftDieCountList)
{
	//Get current selected grade no & total
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectdGradeList);

	unsigned long ulLeft;
	unsigned long ulPicked;
	unsigned long ulTotal;

	LONG lSelectedGradeNum = (LONG)aulSelectdGradeList.GetSize();
	aulSelectdGradeLeftDieCountList.RemoveAll();
	for (LONG i = 0; i < lSelectedGradeNum; i++)
	{
		UCHAR ucTempGrade = aulSelectdGradeList.GetAt(i);
		m_WaferMapWrapper.GetStatistics(ucTempGrade, ulLeft, ulPicked, ulTotal);
		aulSelectdGradeLeftDieCountList.Add(ulLeft);
	}
}


//================================================================
// Function Name: 		SendWaferMapSelectedGradeList
// Input arguments:		None
// Output arguments:	None
// Description:   		Send the wafer map selected grade to EM
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferTable::SendWaferMapSelectedGradeList(BOOL bSendEvent)
{
	if (!IsSecsGemInit()) 
	{
		return;
	}

	UCHAR aucSelectedGradelist[255];
	ULONG aulSelectedGradeLeftDieCount[255];

	try
	{
		//Get current selected grade no & total
		CUIntArray aulSelctdList;
		m_WaferMapWrapper.GetSelectedGradeList(aulSelctdList);
		LONG lSelectedGradeNum = (LONG)aulSelctdList.GetSize();

		UCHAR aucGrade[175];

		memset(&aucGrade, 0x00, sizeof(175));
		for (LONG i = 0; i < lSelectedGradeNum; i++)
		{
			UCHAR ucTempGrade = aulSelctdList.GetAt(i);
			aucGrade[i] = ucTempGrade - 0x30;
//			if ((m_lGradeDisplayMode == 0) || (m_lGradeDisplayMode == 3) || (m_lGradeDisplayMode == 6))
//			{
//				ucTempGrade -= 0x30;
//			}

			aucSelectedGradelist[i] = ucTempGrade;

			unsigned long ulLeft;
			unsigned long ulPicked;
			unsigned long ulTotal;
			m_WaferMapWrapper.GetStatistics(ucTempGrade, ulLeft, ulPicked, ulTotal);
			aulSelectedGradeLeftDieCount[i] = ulLeft;

//			CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
//			if (pBinTable != NULL)
//			{
//				unsigned long ulGradeCapacity = pBinTable->GetGradeCapacity(ucTempGrade - 0x30);
//				(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_DIE_COUNT_LIMIT].Set(i, (unsigned int)ulGradeCapacity);
//			}
		}

		//Get the capacity list of each grade
		ULONG aulGradeCapacity[175];
		CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
		if (pBinTable != NULL)
		{
			pBinTable->GetGradeCapacity(lSelectedGradeNum, aucGrade, aulGradeCapacity);
		}


		LONG lSelectedValidGradeNum = 0;
		for (LONG i = 0; i < lSelectedGradeNum; i++)
		{
			if (aulSelectedGradeLeftDieCount[i] > max(m_ulMinGradeCount, 0))
			{
				lSelectedValidGradeNum++;
			}
		}

		//Set CEID#8025
		(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_LIST].SetAsArray();
		(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_LIST].SetSize(lSelectedValidGradeNum);

		(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_DIE_COUNT_LIST].SetAsArray();
		(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_DIE_COUNT_LIST].SetSize(lSelectedValidGradeNum);

		(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_DIE_COUNT_LIMIT].SetAsArray();
		(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_DIE_COUNT_LIMIT].SetSize(lSelectedValidGradeNum);

		LONG lIndex = 0;
		for (LONG i = 0; i < lSelectedGradeNum; i++)
		{
			if (aulSelectedGradeLeftDieCount[i] > max(m_ulMinGradeCount, 0))
			{
				if (bSendEvent && (lIndex == 0))
				{
					(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SORTING_GRADE] = aucSelectedGradelist[i];
				}

				(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_LIST].Set(lIndex, aucSelectedGradelist[i]);
				(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_DIE_COUNT_LIST].Set(lIndex, (unsigned int)aulSelectedGradeLeftDieCount[i]);
				(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_DIE_COUNT_LIMIT].Set(lIndex, (unsigned int)aulGradeCapacity[i]);
				lIndex++;
			}
		}

		if (bSendEvent)
		{
			//Just sorting map and update it
			SendEvent(SG_CEID_WaferMapSelectedGrade_AUTOLINE, TRUE);		//8025
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("CWaferTable::SendWaferMapSelectedGradeList Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CWaferTable::SendWaferMapSelectedGradeList Exception");
		DisplayException(e);
	}
}

VOID CWaferTable::CreateUploadBarcodeIDFile()
{
	if (m_szUploadBarcodeIDFilePath.IsEmpty())
	{
		return;
	}

	BOOL bUseBarcode = FALSE;
	CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
	if (pWaferLoader != NULL)
	{
		bUseBarcode = pWaferLoader->IsUseBarcode();
	}

	CString szFrameBarcode = (*m_psmfSRam)["WaferTable"]["WT1InBarcode"];
	CString szMapFileName = m_WaferMapWrapper.GetFileName();
	if (szFrameBarcode.IsEmpty() || !bUseBarcode)
	{
		LONG nCol = szMapFileName.Find(".");
		if (nCol > 0)
		{
			szMapFileName = szMapFileName.Left(nCol);
			nCol = szMapFileName.ReverseFind('\\');
			if (nCol > 0)
			{
				szMapFileName = szMapFileName.Right(szMapFileName.GetLength() - nCol - 1);
			}
			szFrameBarcode = szMapFileName;
		}
	}

	if (!szFrameBarcode.IsEmpty())
	{
		CString szUploadBarcodeIDFilePath = m_szUploadBarcodeIDFilePath;
		LONG nCol = szUploadBarcodeIDFilePath.ReverseFind('\\');
		if (nCol < szUploadBarcodeIDFilePath.GetLength() - 1)
		{
			szUploadBarcodeIDFilePath += "\\";
		}
		szUploadBarcodeIDFilePath += szFrameBarcode;
		szUploadBarcodeIDFilePath += ".txt";

		CStdioFile	pFileLog;
		CString szLocalUploadBarcodeIDFilePath = gszUSER_DIRECTORY + "\\UploadBarcodeID\\";
		if (_access(szLocalUploadBarcodeIDFilePath, 0) != 0)
		{
			_mkdir(szLocalUploadBarcodeIDFilePath);
		}
		szLocalUploadBarcodeIDFilePath += szFrameBarcode;
		szLocalUploadBarcodeIDFilePath += ".txt";

		CString szLocalFileName = gszUSER_DIRECTORY + "\\History\\" + "Barcode.txt";

		BOOL bOutput = pFileLog.Open(szLocalFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText);
		if (bOutput)
		{
			CString szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
			pFileLog.WriteString(szFrameBarcode);
			pFileLog.WriteString(",");
			pFileLog.WriteString(szMachineNo);
			pFileLog.Close();

			TRY 
			{
				CopyFile(szLocalFileName, szLocalUploadBarcodeIDFilePath, FALSE);
				CopyFile(szLocalFileName, szUploadBarcodeIDFilePath, FALSE);
			}
			CATCH (CFileException, e)
			{
				SetErrorMessage("Fail to upload barcode file to server path.");
				return;
			}
			END_CATCH

			TRY 
			{
				DeleteFile(szLocalFileName);
			}
			CATCH (CFileException, e)
			{
				SetErrorMessage("Fail to delete local barcode file");
			}
			END_CATCH
		}
	}
}


VOID CWaferTable::UpdateSortingMode(VOID)	//	call when map loaded
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulWaferTotalDie = m_pWaferMapManager->UpdateSortingMode(FALSE, pApp->GetCustomerName(), m_bKeepLastUnloadGrade, m_ucLastPickDieGrade, 
																  m_lSpecialRefDieGrade, m_bIgnoreRegion,
																  m_lSortingMode, m_ulMinGradeCount, m_ulMinGradeBinNo,  m_bSortMultiToOne, m_unArrMinGradeBin);
	if (ulWaferTotalDie > 0)
	{
		m_ulWaferMaxDefectCount = ulWaferTotalDie - (ULONG) (ulWaferTotalDie*m_dWaferGoodDieRatio/100.0);
		m_ulWaferDefectDieCount = 0;
		CString szMsg;
		szMsg.Format("Wafer Total Die %d, GoodDieRatio %f, Defect Limit %d", ulWaferTotalDie, m_dWaferGoodDieRatio, m_ulWaferMaxDefectCount);
		SetErrorMessage(szMsg);
		IPC_CServiceMessage stMsg;
		BOOL bReturn = FALSE;
		int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SavePrescanData", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}
}



//4.53D90fnc Check when 1st time
BOOL CWaferTable::CheckOptimizeBinCountPerWft()
{

	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetOptimizeBinCountPerWft", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
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


//xyz
VOID CWaferTable::ReverseSortingMode(VOID)
{
	if (m_lSortingMode != WT_SORT_NORMAL)	//Only make sense/usable in DEFAULT mode
		return;

	CUIntArray aulSelectedGradeList;
	UCHAR	*pGrade;
	CUIntArray aulReOrderGradeList;
	INT		i=0, j=0;

	//Get current selected grade no & total
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
	pGrade = new UCHAR[aulSelectedGradeList.GetSize()];

	//Reverse Grade 1-100 to 100-1 
	for (i = (INT)aulSelectedGradeList.GetSize() - 1; i >= 0; i--)
	{
		pGrade[j++] = (UCHAR)aulSelectedGradeList[i];
	}

	m_WaferMapWrapper.SelectGrade(pGrade, (unsigned long)aulSelectedGradeList.GetSize());

	if (m_bKeepLastUnloadGrade == TRUE)
	{
		m_pWaferMapManager->UpdateLastUnloadSortingMode(m_ucLastPickDieGrade);		//Re-order Last-Sort Grade if available & enabled
	}

	delete [] pGrade;	//Klocwork
}

BOOL CWaferTable::UpdateLastGradeSortingMode(VOID)	//v4.43T9	//Cree HuiZhou
{
	if (m_lSortingMode != WT_SORT_NORMAL)	//Only make sense/usable in DEFAULT mode
		return FALSE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "Cree")
	{
		return FALSE;
	}

//#ifdef NU_MOTION	//v4.55A1

	CString szLog;
	CUIntArray aulSelectedGradeList;
	UCHAR	*pGrade;
	CUIntArray aulReOrderGradeList;
	INT		i=0, j=0;
	UCHAR ucGrade=0, ucLastGrade=0;

	//Get current selected grade no & total
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
	if (aulSelectedGradeList.GetSize() <= 1)
		return FALSE;

	ULONG ulLeft=0, ulPicked=0, ulTotal=0;
	ULONG ulLargestTotal = 0;
	//Find grade with largest quantity in the list
	for (INT i=0; i<aulSelectedGradeList.GetSize(); i++)
	{
		m_WaferMapWrapper.GetStatistics(aulSelectedGradeList.GetAt(i), ulLeft, ulPicked, ulTotal);
		if (ulLargestTotal == 0)
		{
			ulLargestTotal = ulTotal;
			ucLastGrade = aulSelectedGradeList.GetAt(i);
		}
		else if (ulTotal > ulLargestTotal)
		{
			ulLargestTotal = ulTotal;
			ucLastGrade = aulSelectedGradeList.GetAt(i);
		}
	}

	if (ucLastGrade == 0)
		return FALSE;

	pGrade = new UCHAR[aulSelectedGradeList.GetSize()];

	for (INT i=0; i<aulSelectedGradeList.GetSize(); i++)
	{
		ucGrade = aulSelectedGradeList.GetAt(i);
		if (ucGrade == ucLastGrade)
			continue;
		pGrade[j++] = (UCHAR)aulSelectedGradeList[i];
	}

	pGrade[j++] = ucLastGrade;		//Put last grade to 1st of the list

	UCHAR ucLastGradeToDisplay = ucLastGrade - m_WaferMapWrapper.GetGradeOffset();
	szLog.Format("CREE: UpdateLastGradeSortingMode LastGrade = #%d; ID = " + GetMapFileName(), 
		ucLastGradeToDisplay);		//v4.48A8
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


	m_WaferMapWrapper.SelectGrade(pGrade, (unsigned long)aulSelectedGradeList.GetSize());
	delete [] pGrade;
	return TRUE;
//#else
//	return FALSE;
//#endif
}

BOOL CWaferTable::Select1stLFSortGrade()			//v4.59A15	//SanAn & Semitek
{
	//This fcn is requested by Roy Ip and Leo Lam to disable the LookForward cycle
	// for first few grades, so all dices indexing is using normal PR mode to avoid bad
	// placement on MS;

	CString szLog;
	szLog.Format("WT: Select1stLFSortGrade - MS60=%d, SortMode=%d", IsMS60(), m_lSortingMode);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (!IsMS60())
	{
		return FALSE;
	}
	if (m_lSortingMode != WT_SORT_MIN_DIE)		// Only used in ACCENDING mode
	{
		return FALSE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() != CTM_SANAN) && (pApp->GetCustomerName() != CTM_SEMITEK) )
	//{
	//	return FALSE;
	//}

	if (m_WaferMapWrapper.GetReader() == NULL)
	{
		return FALSE;
	}

	m_n1stLFGrade = 0;
	m_sz1stLFGrade = "";
	m_n1stLFCurrGrade = 0;

	BOOL bFewDicesFound = FALSE;
	INT nGrade = 0;
	CString szGrade, szTemp;
	ULONG ulLeft=0, ulPicked=0, ulTotal=0;
	CUIntArray aulSelectedGradeList;
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);

	//GEt limit from registry
	UINT unLimit = pApp->GetProfileInt(gszPROFILE_SETTING, _T("LF Min Count"), 0);
	if (unLimit == 0)
	{
		return FALSE;	//unLimit = 1000;
	}

	szLog.Format("WT: Select1stLFSortGrade - GradeLimit=%d, GradeListSize=%d", 
		unLimit, aulSelectedGradeList.GetSize());
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


	for (INT i=0; i<aulSelectedGradeList.GetSize(); i++)
	{
		m_WaferMapWrapper.GetStatistics(aulSelectedGradeList.GetAt(i), ulLeft, ulPicked, ulTotal);
		//Change from internal to user grade
		nGrade = aulSelectedGradeList.GetAt(i) - m_WaferMapWrapper.GetGradeOffset();

		if (i == 0)
			szTemp.Format(",%d,", nGrade);
		else
			szTemp.Format("%d,", nGrade);
		szGrade = szGrade + szTemp;

		szLog.Format("WT: Select1stLFSortGrade - grade found = %d, LEFT = %d", nGrade, ulLeft);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		if (ulLeft <= unLimit)
		{
			bFewDicesFound = TRUE;

			szLog.Format("WT: Select1stLFSortGrade - small grade = %d", nGrade);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
		else if (ulLeft > unLimit)
		{
			szLog.Format("WT: Select1stLFSortGrade - Big grade = %d", nGrade);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			if (bFewDicesFound)
			{
				m_n1stLFGrade	= nGrade;
				m_sz1stLFGrade	= szGrade;
			
				CString szLog;
				szLog.Format("WT: Select1stLFSortGrade (SanAn & Semitek) - Grade Found = %d (%s), CntLimit = %d", 
					m_n1stLFGrade, szGrade, unLimit);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}
			break;
		}
	}

	return TRUE;
}

BOOL CWaferTable::CheckIfDisableLFSortGrade(UCHAR ucCurrGrade)			//v4.59A15	//SanAn & Semitek
{
	//If return TRUE	-> disable LF
	//If return FALSE	-> do not disable LF
	CString szLog;

	if (!IsMS60())
	{
		return FALSE;
	}
	if (m_lSortingMode != WT_SORT_MIN_DIE)		// Only used in ACCENDING mode
	{
		return FALSE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() != CTM_SANAN) && (pApp->GetCustomerName() != CTM_SEMITEK) )
	//{
	//	return FALSE;
	//}

	if (m_WaferMapWrapper.GetReader() == NULL)
	{
		return FALSE;
	}

	if (m_n1stLFGrade == 0)
	{
		return FALSE;
	}
	if (m_sz1stLFGrade.GetLength() == 0)
	{
		return FALSE;
	}

	//Change from interal to user grade for comparison
	UCHAR ucGrade = ucCurrGrade - m_WaferMapWrapper.GetGradeOffset();

	//v4.59A18
	CString szGrade;
	szGrade.Format(",%d,", ucGrade);

	if (m_sz1stLFGrade.Find(szGrade) != -1)	//if grade found in the list
	{
		if (m_n1stLFCurrGrade != ucGrade)
		{
			m_n1stLFCurrGrade = ucGrade;
			szLog.Format("WT: CheckIfDisableLFSortGrade - curr grade is #%d", m_n1stLFCurrGrade);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}
	else
	{
		szLog.Format("WT: CheckIfDisableLFSortGrade - 1st LF grade is sorted = %d", ucGrade);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		m_n1stLFGrade = 0;
		return FALSE;
	}

	/*
	//v4.59A17
	if (m_n1stLFCurrGrade != ucGrade)
	{
		m_n1stLFCurrGrade = ucGrade;
		szLog.Format("WT: CheckIfDisableLFSortGrade - curr grade is #%d", m_n1stLFCurrGrade);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}

	if (m_n1stLFGrade == ucGrade)
	{
		szLog.Format("WT: CheckIfDisableLFSortGrade - 1st LF grade is sorted = %d", ucGrade);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		m_n1stLFGrade = 0;
		return FALSE;
	}*/

	return TRUE;
}

BOOL CWaferTable::ResetMapStauts(VOID)
{
	if( IsMS90HalfSortMode() )
	{
		return TRUE;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( !m_bResetMapStatus || IsEnablePNP() || pUtl->GetPrescanDummyMap() )
	{
		return TRUE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName() == "Cree" )
	{
		return TRUE;
	}

	ULONG ulRow, ulMaxRow=0; 
	ULONG ulCol, ulMaxCol=0;
	ULONG ulDieState= 0;
	LONG lMarkDieCount = 0;

	CDWordArray	dwaRow;
	CDWordArray dwaCol;
	CDWordArray dwaStt;

	dwaRow.SetSize(0);
	dwaCol.SetSize(0);
	dwaStt.SetSize(0);

	SaveScanTimeEvent("    WFT: To reset map status on MarkDie and reset state");
	if ( (m_WaferMapWrapper.IsMapValid() == TRUE) && (m_WaferMapWrapper.GetReader() != NULL) )
	{
		if (!m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol))
		{
			return FALSE;
		}

		for (ulRow=0; ulRow<ulMaxRow; ulRow++)
		{
			for (ulCol=0; ulCol<ulMaxCol; ulCol++)
			{
				if ( m_WaferMapWrapper.GetReader()->IsMarkedDie(ulRow, ulCol) == TRUE )
				{
					ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
					dwaRow.SetSize(lMarkDieCount + 1);
					dwaCol.SetSize(lMarkDieCount + 1);
					dwaStt.SetSize(lMarkDieCount + 1);

					dwaRow.SetAt(lMarkDieCount, ulRow);
					dwaCol.SetAt(lMarkDieCount, ulCol);
					dwaStt.SetAt(lMarkDieCount, ulDieState);

					lMarkDieCount++;
				}
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if( pApp->GetCustomerName()=="OSRAM" )
				{
					if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_DIMREFER )
					{
						m_WaferMapWrapper.SetReferenceDie(ulRow, ulCol, TRUE);
					}
				}
			}
		}
	}
	else
	{
		return FALSE;
	}

	m_WaferMapWrapper.ResetStatus();

	if ( (dwaRow.GetSize() > 0) && (dwaCol.GetSize() > 0) )
	{
		for (lMarkDieCount=0; lMarkDieCount<dwaRow.GetSize(); lMarkDieCount++)
		{
			ulRow = dwaRow.GetAt(lMarkDieCount);
			ulCol = dwaCol.GetAt(lMarkDieCount);
			ulDieState = dwaStt.GetAt(lMarkDieCount);

			m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
			if( !IsDieUnpickAll(ulDieState) )
				ulDieState = WT_MAP_DIESTATE_UNPICK;
			m_WaferMapWrapper.SetDieState(ulRow, ulCol, ulDieState);
		}

		dwaRow.RemoveAll();
		dwaCol.RemoveAll();
	}

	//andrew
	if ( m_pvNVRAM != NULL )
	{
		//Clear BH counters in NVRAM when MAP status is reset
		ULONG* pulTemp  = (ULONG*)((unsigned long) m_pvNVRAM + glNVRAM_BondHead_Start);
		*(pulTemp + 9)  = 0;
		*(pulTemp + 10) = 0;
		*(pulTemp + 11) = 0;
		*(pulTemp + 12) = 0;
		*(pulTemp + 13) = 0;
		*(pulTemp + 15) = 0;
		*(pulTemp + 16) = 0;
		*(pulTemp + 17) = 0;
	}

	return TRUE;
}

BOOL CWaferTable::MapSyncMove(VOID)
{
	ULONG ulMaxRow = 0, ulMaxCol=0;
	ULONG ulCurrRow = 0, ulCurrCol=0;
	LONG lTempRow = 0;
	LONG lTempCol = 0;
	LONG lDiff_X = 0;
	LONG lDiff_Y = 0;
	LONG lStepSize = 1;
	LONG lDiePitchX_X = GetDiePitchX_X();
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	CString szTitle, szContent;


	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWT)	//v3.61
		return TRUE;

	szTitle.LoadString(HMB_WT_MAP_SYNC_MOVE);

	if (m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		//Off Sync Move & enable joystick
		m_bMapSyncMove = FALSE;
		m_bJoystickOn = TRUE; 
		SetJoystickOn(TRUE);

		szContent.LoadString(HMB_WT_MAP_FILE_EMPTY);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}


	//Get current map size & pos
	if (!m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol))
	{
		szContent.LoadString(HMB_WT_INVALID_MAP_SIZE);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);
	lTempRow = (LONG)ulCurrRow;
	lTempCol = (LONG)ulCurrCol;


	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		if ( m_ucDieShape == WPR_HEXAGON_DIE )
		{
			lStepSize = 2;
		}

		if (m_lMouseDragDirection == MOUSE_JOY_DIRECTION_NONE)
		{
			lDiff_X = 0;
			lDiff_Y = 0;
		}

		if (m_lMouseDragDirection == MOUSE_JOY_DIRECTION_DOWN)
		{
			lDiff_Y = 1 * lStepSize;
		}

		if (m_lMouseDragDirection == MOUSE_JOY_DIRECTION_UP)
		{
			lDiff_Y = -1 * lStepSize;
		}

		if (m_lMouseDragDirection == MOUSE_JOY_DIRECTION_RIGHT)
		{
			lDiff_X = 1 * lStepSize;
		}
		
		if (m_lMouseDragDirection == MOUSE_JOY_DIRECTION_LEFT)
		{
			lDiff_X = -1 * lStepSize;
		}

		if ( m_ucDieShape == WPR_HEXAGON_DIE )
		{
			//if ( !CMS896AStn::MotionReadInputBit("iJoyUp") && !CMS896AStn::MotionReadInputBit("iJoyLeft") )
			if (m_lMouseDragDirection == MOUSE_JOY_DIRECTION_DOWN_RIGHT)
			{
				lDiff_X = 1;
				lDiff_Y = 1;
			}
			//else if ( !CMS896AStn::MotionReadInputBit("iJoyUp") && !CMS896AStn::MotionReadInputBit("iJoyRight") )
			else if (m_lMouseDragDirection == MOUSE_JOY_DIRECTION_DOWN_LEFT)
			{
				lDiff_X = -1;
				lDiff_Y = 1;
			}
			//else if ( !CMS896AStn::MotionReadInputBit("iJoyDown") && !CMS896AStn::MotionReadInputBit("iJoyLeft") )
			else if (m_lMouseDragDirection == MOUSE_JOY_DIRECTION_UP_RIGHT)
			{
				lDiff_X = 1;
				lDiff_Y = -1;
			}
			//else if ( !CMS896AStn::MotionReadInputBit("iJoyDown") && !CMS896AStn::MotionReadInputBit("iJoyRight") )
			else if (m_lMouseDragDirection == MOUSE_JOY_DIRECTION_UP_LEFT)
			{
				lDiff_X = -1;
				lDiff_Y = -1;
			}
		}
	}
	else
	{
		if ( m_ucDieShape == WPR_HEXAGON_DIE )
		{
			lStepSize = 2;
		}
	
		//if (!m_pinJoyUp->IsHigh())
		if ( !CMS896AStn::MotionReadInputBit("iJoyUp") )
		{
			lDiff_Y = 1 * lStepSize;
		}
	
		//if (!m_pinJoyDn->IsHigh())
		if ( !CMS896AStn::MotionReadInputBit("iJoyDown") )
		{
			lDiff_Y = -1 * lStepSize;
		}
	
		//if (!m_pinJoyLt->IsHigh())
		if ( !CMS896AStn::MotionReadInputBit("iJoyLeft") )
		{
			lDiff_X = 1 * lStepSize;
		}
		
		//if (!m_pinJoyRt->IsHigh())
		if ( !CMS896AStn::MotionReadInputBit("iJoyRight") )
		{
			lDiff_X = -1 * lStepSize;
		}
	
		if ( m_ucDieShape == WPR_HEXAGON_DIE )
		{
			//if ( !m_pinJoyUp->IsHigh() && !m_pinJoyLt->IsHigh() )
			if ( !CMS896AStn::MotionReadInputBit("iJoyUp") && !CMS896AStn::MotionReadInputBit("iJoyLeft") )
			{
				lDiff_X = 1;
				lDiff_Y = 1;
			}
			//else if ( !m_pinJoyUp->IsHigh() && !m_pinJoyRt->IsHigh() )
			else if ( !CMS896AStn::MotionReadInputBit("iJoyUp") && !CMS896AStn::MotionReadInputBit("iJoyRight") )
			{
				lDiff_X = -1;
				lDiff_Y = 1;
			}
			//else if ( !m_pinJoyDn->IsHigh() && !m_pinJoyLt->IsHigh() )
			else if ( !CMS896AStn::MotionReadInputBit("iJoyDown") && !CMS896AStn::MotionReadInputBit("iJoyLeft") )
			{
				lDiff_X = 1;
				lDiff_Y = -1;
			}
			//else if ( !m_pinJoyDn->IsHigh() && !m_pinJoyRt->IsHigh() )
			else if ( !CMS896AStn::MotionReadInputBit("iJoyDown") && !CMS896AStn::MotionReadInputBit("iJoyRight") )
			{
				lDiff_X = -1;
				lDiff_Y = -1;
			}
		}
	}

	if ( (lDiff_X != 0) || (lDiff_Y != 0) )
	{
		lTempRow += lDiff_Y;
		lTempCol += lDiff_X;

		//Check map position is hit max row & col or less than 0
		if ( (lTempRow >= (LONG)ulMaxRow) || (lTempCol >= (LONG)ulMaxCol) 
		  || (lTempRow < 0) || (lTempCol < 0) )
		{
			return FALSE;		//Return out map limit
		}

		m_WaferMapWrapper.SetCurrentPosition((ULONG)lTempRow, (ULONG)lTempCol);
		DisplayNextDirection((ULONG)lTempRow, (ULONG)lTempCol);

		//Move table to updated position
		X_Sync();
		Y_Sync();
		T_Sync();
		GetEncoderValue();

		LONG lPosnX, lPosnY;
		lPosnX = GetCurrX() - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lPosnY = GetCurrY() - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;

		if (XY_SafeMoveTo(lPosnX, lPosnY) == FALSE)
		{
			//Off Sync Move & enable joystick
			m_bMapSyncMove = FALSE;
			m_bJoystickOn = TRUE; 
			SetJoystickOn(TRUE);

			szContent.LoadString(HMB_WT_OUT_WAFER_LIMIT);

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			return FALSE;		//return out wafer limit
		}

		m_lMouseDragDirection = MOUSE_JOY_DIRECTION_NONE;

		if( WftMoveSearchDie(lPosnX, lPosnY, TRUE)==FALSE )
		{
			return FALSE;
		}
	}


	return TRUE;
}


UCHAR CWaferTable::UpdateWaferGradeColor(BOOL bReset)
{
	BOOL bFileExist = FALSE;
	ULONG ulTempColor = 1;
	ULONG ulTargetColor = 1;
	UCHAR ucGrade = 0;	
	UCHAR ucGradeOffset = 0;

	CStdioFile cfColorFile;
	CString szContent;
	BOOL bIfFileExists;


	if ( m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		return 0;
	}

	ucGradeOffset = m_WaferMapWrapper.GetGradeOffset();

	if ( bReset == TRUE )
	{
		//Get Default wafer color & reset all grades color
		bIfFileExists = cfColorFile.Open(gszExeColorFile, CFile::modeRead|CFile::shareExclusive|CFile::typeText);

		if (bIfFileExists)
		{
			ucGrade = 0;
			cfColorFile.SeekToBegin();

			while (cfColorFile.ReadString(szContent) != NULL)
			{
				ulTempColor = atoi((LPCTSTR)szContent);
				m_WaferMapWrapper.SetGradeColor(ucGrade + ucGradeOffset, (COLORREF)ulTempColor);
				ucGrade++;
			}

			cfColorFile.Close();
		}
		else
		{
			return 0;
		}
	}
	else
	{
		//Get current wafer color & update other grades
		bIfFileExists = cfColorFile.Open(gszAsmHmiColorFile, CFile::modeRead|CFile::shareExclusive|CFile::typeText);

		if (bIfFileExists)
		{
			//Get target grade current color
			ucGrade = 0;
			cfColorFile.SeekToBegin();
			while (cfColorFile.ReadString(szContent) != NULL)
			{
				if ( ucGrade == m_ucChangeTargetGrade )
				{
					ulTargetColor = atoi((LPCTSTR)szContent);
					break;
				}
				ucGrade++;
			}

			//Set selected grade color
			ucGrade = 0;
			cfColorFile.SeekToBegin();
			while (cfColorFile.ReadString(szContent) != NULL)
			{
				ulTempColor = atoi((LPCTSTR)szContent);

				if ( (ucGrade <= m_ucChangeEndGrade) && (ucGrade >= m_ucChangeStartGrade) )
				{
					m_WaferMapWrapper.SetGradeColor(ucGrade + ucGradeOffset, (COLORREF)ulTargetColor);
				}
				else
				{
					m_WaferMapWrapper.SetGradeColor(ucGrade + ucGradeOffset, (COLORREF)ulTempColor);
				}
				ucGrade++;
			}
			

			cfColorFile.Close();
		}
		else
		{
			return 0;
		}
	}

	m_WaferMapWrapper.UpdateGradeColor();

	return ucGrade;
}


VOID CWaferTable::UpdateWaferMapControlTitle(LONG lPageNo)
{
	if ( lPageNo == 0 )
	{
		m_bWaferControlPageNo = FALSE;
	}
	else if ( lPageNo == 1 )
	{
		m_bWaferControlPageNo = TRUE;
	}
}


VOID CWaferTable::SetDieSharp(VOID)
{
	if ( CMS896AStn::m_bEnablePolyonDie == TRUE )
	{
		if( m_ucDieShape==WPR_HEXAGON_DIE )
		{
			m_WaferMapWrapper.ChangeShape(m_ucDieShape);
		}
	}

	if ( m_bShowFirstDie == TRUE )
	{
		ULONG ulRow = 0, ulCol = 0;
		m_WaferMapWrapper.GetFirstDiePosition(ulRow, ulCol);
		m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);	
		m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bRemoveHomeRefer = FALSE;
	if( pApp->GetCustomerName()=="LatticePower" )
	{
		bRemoveHomeRefer = TRUE;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	if( pApp->GetCustomerName()=="Semitek" && pUtl->GetPrescanRegionMode() && pSRInfo->GetTotalSubRegions()>1 )
	{
		bRemoveHomeRefer = TRUE;
	}

	if( bRemoveHomeRefer )
	{
		ULONG ulRow, ulCol;
		m_WaferMapWrapper.GetFirstDiePosition(ulRow, ulCol);
		if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) )
		{
			m_WaferMapWrapper.SetReferenceDie(ulRow, ulCol, FALSE);
		}
	}
}


VOID CWaferTable::MapEdgeSize(VOID)
{
	if ( m_ucMapEdgeSize > 50 )
	{
		m_ucMapEdgeSize = 0;
	}

	if ( m_ucMapEdgeSize != 0 )
	{
		m_WaferMapWrapper.SelectEdge(m_ucMapEdgeSize);
	}

	//v3.49T5
	if (!m_bIgnoreRegion)				//Pick SELECT region -> bIgnoreRegion = FALSE
	{
		for (INT i=1; i<WT_MAX_SELECT_REGION; i++)
		{
			if ( ((m_ulSelectRegionLRX[i] - m_ulSelectRegionULX[i]) > 0) && 
				 ((m_ulSelectRegionLRY[i] - m_ulSelectRegionULY[i]) > 0) )
			{
				ULONG ulULX = 0, ulULY = 0, ulLRX = 0, ulLRY = 0;
				m_WaferMapWrapper.ConvertUserToInternal(m_ulSelectRegionULY[i], m_ulSelectRegionULX[i], ulULY, ulULX);
				m_WaferMapWrapper.ConvertUserToInternal(m_ulSelectRegionLRY[i], m_ulSelectRegionLRX[i], ulLRY, ulLRX);
				m_WaferMapWrapper.SelectRegion(ulULY, ulULX, ulLRY, ulLRX);
			}
		}

		//v3.49T5
		if ( (m_ucSelectChangeGrade > 0) )
		{
			CString szMsg;
			szMsg.Format("Edge size and change grade to %d", m_ucSelectChangeGrade);
			SetErrorMessage(szMsg);
			SaveScanTimeEvent(szMsg);
			ULONG ulNumOfRows = 0, ulNumOfCols = 0;
			m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols);

			for (ULONG i=0; i<ulNumOfRows; i++)
			{
				for (ULONG j=0; j<ulNumOfCols; j++)
				{
					if (m_WaferMapWrapper.GetReader()->IsSelected(i, j))
					{
						m_WaferMapWrapper.ChangeGrade(i, j, m_WaferMapWrapper.GetGradeOffset() + m_ucSelectChangeGrade);
					}
				}
			}
		}
	}
}


BOOL CWaferTable::MatchMapFile(CString szFilePath)
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	BOOL bSrchResult = FALSE;
	BOOL bSrchFinished = FALSE;
	CString szFileName;
	CString szSearched;
	CStringArray szaNameFound;

	int nCount = 0;
	int nCol = 0;
	int i;
	int nNamePos;



	//Start search file with input extension
	hSearch = FindFirstFile((szFilePath + "\\*." + m_szMapFileExtension), &FileData); 

	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		SetAlert_Red_Yellow(IDS_WT_MAP_EXT_NOT_MATCH);

		SetErrorMessage("No map file found with this extension");
		return FALSE;
	}

	while ( bSrchFinished == FALSE) 
	{	 
		if ( FindNextFile(hSearch, &FileData) == FALSE ) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				bSrchFinished = TRUE; 
			}
        } 
		else
		{
			szFileName = FileData.cFileName;

			if ( (szFileName.Find(m_szPrefixName) == 0) && (szFileName.Find(m_szBCMapFilename) != -1)  )
			{
				//Only Add file which prefix name is located @ begin of filename 
				szaNameFound.Add(szFileName);
			}
		}
    }
 
	// Close the search handle. 
	if ( FindClose(hSearch) == FALSE ) 
	{ 
		HmiMessage_Red_Yellow("Couldn't close search handle", "Search Wafermap file error"); 
		bSrchResult = FALSE;
	} 

	//Check any file contain map file name & prefix name
	if ( szaNameFound.GetSize() == 0 )
	{
		bSrchResult = FALSE;
	}
	else
	{
		nCount = 0;
		for (i=0; i<szaNameFound.GetSize(); i++)
		{
			szFileName = szaNameFound.GetAt(i);

			//Find map file name end position
			if ( m_szMapFileExtension.IsEmpty() == FALSE )
			{
				nNamePos = szFileName.Find(m_szBCMapFilename) + m_szBCMapFilename.GetLength() + m_szMapFileExtension.GetLength() + 1;
			}
			else
			{
				nNamePos = szFileName.Find(m_szBCMapFilename) + m_szBCMapFilename.GetLength();
			}

			nCol = szFileName.GetLength();

			if ( nCol == nNamePos )
			{
				if ( (nCol = szFileName.ReverseFind('.')) != -1 )
				{
					szSearched = szFileName.Left(nCol);
				}
				else
				{
					szSearched = szFileName;
				}
				nCount++;
			}
		}

		if ( nCount > 1 )
		{
			bSrchResult = FALSE;
		}
		else
		{
			m_szBCMapFilename = szSearched;
			bSrchResult = TRUE;
		}
	}

	//Alert user if no file found
	if ( bSrchResult == FALSE )
	{
		SetAlert_Red_Yellow(IDS_WT_MAP_EXT_NOT_MATCH);

		szFileName.Format(", Files = %d", nCount);
		szFileName = m_szBCMapFilename + szFileName;
			
		SetErrorMessage("No map file found with this filename " + szFileName);
	}

	return bSrchResult;
}


VOID CWaferTable::DisplayNextDirection(ULONG ulRow, ULONG ulCol)
{
	ULONG ulNextRow=0, ulNextCol=0;		//Klocwork
	LONG lRowDiff, lColDiff;
	UCHAR ucGrade = 0;					//Klocwork
	CString szAlgorithm, szPathFinder;

	//CString szText1=" ", szText2=" ";
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction = WAF_CDieSelectionAlgorithm::INVALID;	//Klocwork

	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
	if ( szAlgorithm.Find("Pick and Place") != -1 )
	{
		return;
	}

	if ( (szAlgorithm == "Block Algorithm") || (szAlgorithm == "Block Algorithm (without Ref Die)") )
	{
		return;
	}

	//v2.99T1	//Do not peek die if SmartWalk is used
	if ((szAlgorithm.GetLength() >= 5) && 
		(szAlgorithm.GetAt(3) == 'S') && 
		(szAlgorithm.GetAt(4) == 'W'))
	{
		return;
	}

	if ((szAlgorithm == "DeepSearch") || (szAlgorithm == "AssistSearch"))	//v3.13T3
		return;


	if ( m_WaferMapWrapper.PeekNextDie(ulRow, ulCol, ulNextRow, ulNextCol, ucGrade, eAction) == TRUE )
	{
		lRowDiff = (LONG)ulNextRow - (LONG)ulRow;
		lColDiff = (LONG)ulNextCol - (LONG)ulCol;
			
		if ( lRowDiff > 0 )
		{
			m_szNextPath = "D";
		}
		else if ( lRowDiff < 0 )
		{
			m_szNextPath = "U";
		}

		if ( lColDiff > 0 )
		{
			m_szNextPath = "R";
		}
		else if ( lColDiff < 0 )
		{
			m_szNextPath = "L";
		}
	}
	else
	{
		m_szNextPath = " ";
	}
}


BOOL CWaferTable::CheckSCNFileLoaded()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (IsPrescanEnable())
	{
		if( pUtl->GetPrescanRegionMode() || GetRegionSortOuter() )
		{
			if( pSRInfo->GetInitState() == WT_SUBREGION_INIT_ERROR )
			{
				HmiMessage_Red_Back("Prescan Home Die region error");
				SetErrorMessage("Prescan Home Die region error");
				return FALSE;
			}
		}

		if( pUtl->GetPrescanRegionMode()==FALSE && GetRegionSortOuter()==FALSE )
		{
			CString szAgtm, szPath;
			m_WaferMapWrapper.GetAlgorithm(szAgtm, szPath);
			if( szAgtm.Find("Custom Order Region") != -1 )
			{
				HmiMessage_Red_Back(" Custom Order Region not for this sorting\n Please reset map algorithm");
				SetErrorMessage(szAgtm + "  selection error");
				return FALSE;
			}
		}

		if ( pUtl->GetPrescanDummyMap() == FALSE && IsPrescanEnded() == FALSE )
		{
			if (GetPsmLoadAction() == 3 && GetPsmEnable() == FALSE)
			{
				HmiMessage_Red_Back("Wafer Map PSM file not loaded!");
				SetErrorMessage("Wafer Map PSM file not loaded!");
				return FALSE;
			}

			if ( GetPsmLoadAction() == 2 && GetPsmEnable() )
			{
				HmiMessage_Red_Back("Wafer Map PSM file EXIST and loaded!");
				SetErrorMessage("Wafer Map PSM file exist!");
				return FALSE;
			}
		}

		return TRUE;
	}

	if ( m_bEnableSCNFile && m_bCheckSCNLoaded && (IsScnLoaded() == FALSE) )
	{ 
		SetAlert_Red_Yellow(IDS_WT_NO_SCN_FILE);
		SetErrorMessage("SCN file is not found or invalid");
		return FALSE;
	}

	return TRUE;
}


//================================================================
// RestoreGradeMapTable()
//   Created-By  : Andrew Ng
//   Date        : 5/6/2008 11:04:55 AM
//   Description : Restore grade map table from MSD file to current MAP memory
//   Remarks     : For resolving PLLM dynamic grade-mapping problem
//================================================================
BOOL CWaferTable::RestoreGradeMapTable()
{
	CStringMapFile pFile;
	if ( pFile.Open(MSD_MAP_TABLE_FILE, FALSE, FALSE) != 1 )
	{
		return FALSE;
	}

	UCHAR ucGrade = 0;
	USHORT usOriginalGrade = 0;
	USHORT usGradeOffset = 0;

	m_WaferMapWrapper.ResetGradeMap();
	m_WaferMapWrapper.SetGradeToGradeMap(48, 48, "");	//v4.35T1	Add Para #3 in v1.65.01 lib

	for (int i=1; i<=MS_MAX_BIN; i++)
	{
		if (!!pFile["Map"][i])
		{
			usOriginalGrade = (USHORT)(LONG)(pFile)["Map"][i];

			if (usOriginalGrade != 0)
			{
				m_WaferMapWrapper.SetGradeToGradeMap(i + m_WaferMapWrapper.GetGradeOffset(), 
														usOriginalGrade + m_WaferMapWrapper.GetGradeOffset(),
														"");	//v4.35T1	Add Para #3 in v1.65.01 lib
			}
		}
	}

	m_WaferMapWrapper.Redraw();
	pFile.Close();
	return TRUE;
}


//================================================================
//   Created-By  : Andrew Ng
//   Date        : 5/6/2008 11:05:14 AM
//   Description : Save & backup current grade-map table to MSD file
//   Remarks     : For resolving PLLM dynamic grade-mapping problem
//================================================================
BOOL CWaferTable::SaveGradeMapTable()
{
	CUIntArray aulAvailableGradeList;
	m_WaferMapWrapper.GetAvailableGradeList(aulAvailableGradeList);
	if (aulAvailableGradeList.GetSize() <= 0)
		return FALSE;


	CMSLogFileUtility::Instance()->MAP_BackupGradeLog();
	CString szMsg;

	CMSLogFileUtility::Instance()->MAP_LogGradeMapping(GetMapFileName());
	CTime theTime = CTime::GetCurrentTime();
	szMsg = theTime.Format("%Y-%m-%d (%H:%M:%S)");
	CMSLogFileUtility::Instance()->MAP_LogGradeMapping(szMsg);

	m_bMapGradeMappingOK = TRUE;

	CStringMapFile pFile;
	if ( pFile.Open(MSD_MAP_TABLE_FILE, FALSE, TRUE) != 1 )
	{
		return FALSE;
	}

	UCHAR ucUserGrade = 0;
	USHORT usOriginalGrade = 0;
	for (int i=0; i<aulAvailableGradeList.GetSize(); i++)
	{
		ucUserGrade = aulAvailableGradeList.GetAt(i) - CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
		//usOriginalGrade = m_WaferMapWrapper.GetOriginalGrade(ucGrade);
		
		usOriginalGrade = m_WaferMapWrapper.GetOriginalGrade(ucUserGrade + m_WaferMapWrapper.GetGradeOffset())
								- m_WaferMapWrapper.GetGradeOffset();		

		szMsg.Format("%d		%d", ucUserGrade,	usOriginalGrade);
		CMSLogFileUtility::Instance()->MAP_LogGradeMapping(szMsg);
		(pFile)["Map"][ucUserGrade] = (LONG) usOriginalGrade;
	}

	CMSLogFileUtility::Instance()->MAP_LogGradeMapping("    ");
	pFile.Update();
	pFile.Close();
	return TRUE;
}


BOOL CWaferTable::DeleteGradeMapTable()
{
	TRY 
	{
		DeleteFile(MSD_MAP_TABLE_FILE);
		CString szMsg;
		CTime theTime = CTime::GetCurrentTime();
		szMsg = theTime.Format("%Y-%m-%d (%H:%M:%S)");
		CMSLogFileUtility::Instance()->MAP_LogGradeMapping(szMsg + " delete map table\n");
	}
	CATCH (CFileException, e) {
		return FALSE;
	}END_CATCH
	return TRUE;
}


//================================================================
// DeleteLastLocalMapFile()
//   Created-By  : Andrew Ng
//   Date        : 5/17/2008 10:46:22 AM
//   Description : Delete all map files in the .\Exe\Map sub-folder
//   Remarks     : 
//================================================================
BOOL CWaferTable::DeleteLastLocalMapFile()
{
	CString szMapPath	= LOCALHD_MAP_PATH + "\\";		//v2.83T31		
	WIN32_FIND_DATA FileData; 
	HANDLE			hSearch; 
	CString szFile;


	hSearch = FindFirstFile(szMapPath + "*.*", &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		return FALSE;
	} 

	BOOL bFound = TRUE;

	do
	{
		szFile = FileData.cFileName;

		if ((szFile != ".") && (szFile != ".."))
		{
			TRY {
				DeleteFile(szMapPath + szFile);

			} CATCH (CFileException, e) {
			}
			END_CATCH
		}
			
		bFound = FindNextFile(hSearch, &FileData);

	} while (bFound);

	FindClose(hSearch);
	return TRUE;
}

BOOL CWaferTable::GetRegionSortOuter()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsPrescanEnable()==FALSE )
		return FALSE;

	if( pUtl->GetPrescanRegionMode() )
		return FALSE;

	if( m_bRegionBlkPickOrder && (m_lSubSortMode == 0) )	// BY REGION AND FAR FIRST
		return TRUE;

	return FALSE;
}

VOID CWaferTable::RegionPickLog(CONST CString szLogMsg)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( (pUtl->GetPrescanRegionMode() || GetRegionSortOuter() || pUtl->GetRegionPickMode()) && 
		IsAOIOnlyMachine()==FALSE )
		pUtl->RegionOrderLog(szLogMsg, TRUE);
}

BOOL CWaferTable::InitKnowlesNGMap()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "Knowles")
		return TRUE;
	if (m_WaferMapWrapper.GetReader() == NULL) 
		return FALSE;
	if (!m_WaferMapWrapper.IsMapValid())
		return FALSE;

//AfxMessageBox("Init Knowles NG map ...", MB_SYSTEMMODAL);
	CMSLogFileUtility::Instance()->MS_LogOperation("Init Knowles NG map - " + GetMapFileName());

	INT i=0, j=0;
	INT nIsOddRCol=0;
	CString szLine;
	CString szElectricInfo, szCurrWaferID;
	CString szLastWaferID = "";
	BOOL bMarkNextDieToGrade101 = FALSE;
	LONG lLastRow=0, lLastCol=0;
	unsigned char ucGrade = 0;


	unsigned long ulNumOfRows=0, ulNumOfCols=0;
	m_WaferMapWrapper.GetMapDimension(ulNumOfRows, ulNumOfCols);
	
	for (INT j = 0; j < (INT)ulNumOfCols; j++)
	{
		for (INT i = 0; i < (INT)ulNumOfRows - 1; i++)
		{
			if (m_pWaferMapManager->IsMapHaveBin(i, j))
			{
				ucGrade = m_WaferMapWrapper.GetGrade(i, j) - m_WaferMapWrapper.GetGradeOffset();
				
				if ( (ucGrade != 17) && (ucGrade != 36) )
				{
					m_WaferMapWrapper.ChangeGrade(i, j, 99 + m_WaferMapWrapper.GetGradeOffset());
				}
			}
		}
	}

	unsigned char aaTempGrades[1];
	aaTempGrades[0] = 99+ m_WaferMapWrapper.GetGradeOffset();
	m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);
	m_WaferMapWrapper.RestartMap();
	return TRUE;
}

BOOL CWaferTable::CheckMapSpecNamePrefix(CONST CString szFileName)//4.52D15
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szMzg;
	CString szSpecification;
	szSpecification = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SPECIFICATION]; 
	if ( m_bEnablePrefix && m_bEnablePrefixCheck && (m_szPrefixName.IsEmpty() == FALSE) )
	{
		//if (CheckPrefixNameForEpitop(szSpecification,m_szPrefixName) == FALSE)
		if (pApp->GetCustomerName() == "Epitop")
		{
			if( szSpecification != m_szPrefixName )
			{
				m_WaferMapWrapper.InitMap();	//Clear map if prefix not match
				szMzg.Format("WT: Load map check Spec prefix fail - \n Prefix=%s vs Spec=%s \n Please Add Prefix as the same name of Specification.",m_szPrefixName,szSpecification);
				SetErrorMessage(szMzg);
				HmiMessage_Red_Yellow(szMzg);		
				m_WaferMapWrapper.InitMap();	//Clear map if prefix not match
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CWaferTable::CheckMapNamePrefixSuffix(CONST CString szFileName)		//v4.39T7	//Silan
{
	CString szMapBCFileName = szFileName;
 	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();		
//AfxMessageBox("CheckMapNamePrefixSuffix = " + szMapBCFileName, MB_SYSTEMMODAL);

	CMSLogFileUtility::Instance()->MS_LogOperation("CheckMapNamePrefixSuffix:" + szMapBCFileName + "," + m_szPrefixName);
	if ( m_bEnablePrefix && m_bEnablePrefixCheck && (m_szPrefixName.IsEmpty() == FALSE) )		//Check prefix existence	//v3.61	//NeoNeon
	{
		if (pApp->GetCustomerName() == "Epitop")
		{
		}
		else
		{
			if (szMapBCFileName.Find(m_szPrefixName) == -1)
			{
				SetErrorMessage("WT: barcode Load map check prefix fail - BC= " + szMapBCFileName + " - " + m_szPrefixName);
				HmiMessage_Red_Yellow("WT: barcode auto Load map check prefix fail");		//v3.65
				return FALSE;
			}
		}
	}


	if ( m_bEnableSuffix && m_bEnableSuffixCheck && (m_szSuffixName.GetLength() > 0) )
	{
		if (szMapBCFileName.Find(m_szSuffixName) == -1)
		{
			SetErrorMessage("ERROR: Load map check suffix fail - BC = " + szMapBCFileName + " - " + m_szSuffixName);
			HmiMessage_Red_Yellow("WT: barcode auto Load map check suffix fail");
			m_WaferMapWrapper.InitMap();		
			return FALSE;
		}
	}

	return TRUE;
}

VOID CWaferTable::CheckRegionScanMode(CString szAlgorithm)
{
}

//4.53D16 fnc compare names
BOOL CWaferTable::ComparingProjectHeaderWithPackgeName()
{
	if(!m_bEnableMatchMapHeader)
	{
		return TRUE;
	}
	CString szHeaderProject;
	CString szPKGFilename;
//	LONG lHmiMsgReply;
	CString szContent;
	CString szMsg;

	szHeaderProject	 = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER]["Project"];
	szPKGFilename	 = (*m_psmfSRam)["MS896A"]["PKG Filename"];

	szMsg.Format("Project Name:%s, Package Name:%s",szHeaderProject, szPKGFilename );
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	if( szHeaderProject != szPKGFilename)
	{
		szContent.Format("Project Name: %s \nPackage Name: %s\n\n\t Please unload wafer checking", szHeaderProject, szPKGFilename);
		//lHmiMsgReply = HmiMessage(szContent, "Comparing Wafer file with Package file name", 103, 3); 
		HmiMessage(szContent, "Comparing Wafer file with Package file name"); 
			SetErrorMessage("ERROR: Load map check project header fail -- " + szMsg);
			return FALSE;

		//if (lHmiMsgReply == 3) //3 = Yes
		//{
		//	return TRUE;
		//}
		//else
		//{
		//	m_WaferMapWrapper.InitMap();	//Clear map if reply No
		//	SetErrorMessage("ERROR: Load map check project header fail -- " + szMsg);
		//	return FALSE;
		//}
	}


	return TRUE;
}

BOOL CWaferTable::IsMapNullBin(const ULONG ulRow, const ULONG ulCol)
{
	return m_pWaferMapManager->IsMapNullBin(ulRow, ulCol);
}



BOOL CWaferTable::PrepareNextRegionStatistics()
{
	if (m_pWaferMapManager->IsUserRegionPickingMode())
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		ULONG ulSortingRegion = pSRInfo->GetSortingRegion();
		UCHAR ucGrade = pSRInfo->GetSortingRegionGrade();
		m_pWaferMapManager->PrepareNextRegionSortingPath(ucGrade, ulSortingRegion);
	}

	return TRUE;
}


UINT PrepareNextRegionStatisticsThread(LPVOID lpVoid)
{
	CWaferTable	*pWaferTable = (CWaferTable*)(lpVoid);
	if (pWaferTable)
	{
		pWaferTable->PrepareNextRegionStatistics();
	}
	return 0;
}


BOOL CWaferTable::CreatePrepareNextRegionStatisticsThread(const UCHAR ucSortingGrade)
{
	if (m_pWaferMapManager->IsUserRegionPickingMode())
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		pSRInfo->SetSortingRegionGrade(ucSortingGrade);
		CWinThread *pThread = AfxBeginThread(PrepareNextRegionStatisticsThread, (LPVOID)(this));
		
		if (pThread != 0)
		{
	//		ofstream ofs("c:\\AD830A\\Log\\ThreadID.log", ios::app); // the only entry into the log WITHOUT append
	//		ofs << pThread->m_nThreadID << "(0x" << hex << pThread->m_nThreadID << ")\CreatePrepareNextRegionStatisticsThread" << endl;
		}

		return TRUE;
	}

	return FALSE;
}
