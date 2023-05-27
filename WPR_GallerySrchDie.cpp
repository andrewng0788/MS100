#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "GallerySearchController.h"
#include "WAF_CPhysicalInformation.h"
#include "PrescanUtility.h"
#include "PrescanInfo.h"
#include "LastScanPosition.h"
#include "TakeTime.h"
#include "WT_SubRegion.h"
#include "MS896A.h"
#include "WaferTable.h"
#include "WL_Constant.h"
#include "Utility.H"
#include "PRFailureCaseLog.h"

#define	WPR_ADD_DIE_INDEX	-101
#define	WPR_SCAN_MAP_OFFSET	20

LONG CWaferPr::CheckHolePatternInThisMap(IPC_CServiceMessage &svMsg)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	ULONG lTotalPoints = m_ulScanAlignTotalPoints;
	BOOL bCheckHoleOK = TRUE;

	if( m_bScanAlignAutoFromMap || CMS896AStn::m_oNichiaSubSystem.IsEnabled() || pUtl->GetPrescanDummyMap())
	{
		svMsg.InitMessage(sizeof(BOOL), &bCheckHoleOK);

		return 1;
	}

	if( IsWaferMapValid() && m_bScanAlignFromFile && lTotalPoints>0 )
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( pApp->GetCustomerName()==CTM_SANAN && GetMapNameOnly().Find(".")!=-1 )
		{
			SetAlarmLog("Not a fully wafer");
			svMsg.InitMessage(sizeof(BOOL), &bCheckHoleOK);
			return 1;
		}

		CString szMsg, szTemp;
		ULONG ulMapRow = 0, ulMapCol = 0;
		ULONG ulTotalInMap = 0;
		LONG lErrorRow = 0, lErrorCol = 0;
		szMsg.Format("Check Hole Pattern In This Map total points %d", lTotalPoints);
		for(ULONG i=0; i<lTotalPoints; i++)
		{
			if (i >= (ULONG)m_awPointsListRow.GetSize())
			{
				break;
			}
			LONG lUserRow = 0, lUserCol = 0;
			lUserRow = (LONG)m_awPointsListRow.GetAt(i);
			lUserCol = (LONG)m_awPointsListCol.GetAt(i);
			//	;
			if( ConvertOrgUserToAsm(lUserRow, lUserCol, ulMapRow, ulMapCol) )
			{
				ulTotalInMap++;
				if (WM_CWaferMap::Instance()->IsMapHaveBin(ulMapRow, ulMapCol)!=m_ucPointsListSta.GetAt(i))
				{
					lErrorRow = lUserRow;
					lErrorCol = lUserCol;
					bCheckHoleOK = FALSE;
					szTemp.Format("die(%d,%d) NG ", lUserRow, lUserCol);
				}
				else
				{
					szTemp.Format("die(%d,%d) ok ", lUserRow, lUserCol);
				}
				szMsg += szTemp;
			}
			else
			{
				szTemp.Format("die(%d,%d) out ", lUserRow, lUserCol);
				szMsg += szTemp;
				break;
			}
		}

		SetAlarmLog(szMsg);

		if( ulTotalInMap == lTotalPoints )
		{
			if( bCheckHoleOK==FALSE )
			{
				CString szMsg;
				szMsg.Format("Empty Hole %d,%d has die in map(%d)!\nPlease Check the map file.", lErrorRow, lErrorCol, lTotalPoints);
				HmiMessage_Red_Back(szMsg, "Prescan Map");
				SetAlarmLog(szMsg);
			}
		}
		else
		{
			bCheckHoleOK = TRUE; 
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bCheckHoleOK);

	return 1;
}

LONG CWaferPr::ClearPrescanData(IPC_CServiceMessage& svMsg)
{
	SaveScanTimeEvent("WFR: to clear prescan data");
	m_pPrescanPrCtrl->ResumePR();
	m_pPrescanPrCtrl->RemoveAllShareImages();
	GetImageNumInGallery();
	m_dScanPrCleanTime		= 0.0;
	m_lScanIndexStopCount	= 0;
	m_lScanPrAbnormalCount	= 0;

	CalculatePrescanFov(FALSE);	// clear prescan raw data

	DelGrabFocusData();
	WSClearScanRemainDieList();
	m_cPIMap.RemoveAll();
	m_cPIMap.SetScan2Pr(FALSE);
	m_cPIMap.KeepIsolatedDice(FALSE);

	m_stAF_Data.m_dLastFocusScore = 0.0;
	m_stAF_Data.m_lFrameFocusDir = 0;
	m_stAF_Data.m_lFrameFocusStep = 0;
	m_stAF_Data.m_lFrameMinCol	= 1000;
	m_stAF_Data.m_lFrameMaxCol	= 0;
	m_stAF_Data.m_lFrameMinRow	= 1000;
	m_stAF_Data.m_lFrameMaxRow	= 0;
	m_nPrescanIndexCounter = 0;

	m_dwaRsnBaseRow.RemoveAll();
	m_dwaRsnBaseCol.RemoveAll();
	m_dwaRsnBaseWfX.RemoveAll();
	m_dwaRsnBaseWfY.RemoveAll();

	m_bScanFrameRangeSent	= FALSE;
	m_bPrescanTwicePrDone	= TRUE;

	SaveScanTimeEvent("WFR: clear prescan data done");
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::SavePrescanData(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	SavePrData(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::PrescanDieAction(IPC_CServiceMessage &svMsg)
{
	LONG lValue = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	svMsg.GetMsg(sizeof(LONG), &lValue);

	switch(lValue)
	{
	case 0:	// defect die
		if( m_bPrescanDefectAction )
		{
			m_bPrescanDefectToNullBin = !m_bPrescanDefectToNullBin;
		}
		else
		{
			m_bPrescanDefectMarkUnPick = !m_bPrescanDefectMarkUnPick;
		}
		break;

	case 1:	// empty die
		if( m_bPrescanEmptyAction )
		{
			m_bPrescanEmptyToNullBin = !m_bPrescanEmptyToNullBin;
		}
		else
		{
			if( pApp->GetFeatureStatus(MS896A_FUNC_PRESCAN_EMPTY_UNMARK) || IsSorraSortMode() )
				m_bPrescanEmptyMarkUnPick = !m_bPrescanEmptyMarkUnPick;
		}
		break;

	case 2:	// bad cut die
		if( m_bPrescanBadCutAction )
		{
			m_bPrescanBadCutToNullBin = !m_bPrescanBadCutToNullBin;
		}
		else
		{
			m_bPrescanBadCutMarkUnPick = !m_bPrescanBadCutMarkUnPick;
		}
		break;

	default:
		break;
	}

	if( pApp->GetFeatureStatus(MS896A_FUNC_PRESCAN_EMPTY_UNMARK)!=TRUE && IsSorraSortMode()!=TRUE )
		m_bPrescanEmptyMarkUnPick = !m_bPrescanEmptyAction;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	BOOL bUnpick = m_bPrescanDefectMarkUnPick || m_bPrescanEmptyMarkUnPick || m_bPrescanBadCutMarkUnPick;
	pUtl->SetMarkDieUnpick(bUnpick);

	SavePrData(FALSE);
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferPr::UpdatePrescanRefDiePos(ULONG ulRow, ULONG ulCol, LONG lReferID, LONG lInEncX, LONG lInEncY)
{
	BOOL bResult = TRUE;
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	LONG lEncX = lInEncX;
	LONG lEncY = lInEncY;

	SetMapPhyPosn(ulRow, ulCol, lEncX, lEncY);
	SetPrescanPosition(ulRow, ulCol, lEncX, lEncY);

	if( m_bPrescanPrCheckReferDie==FALSE )
	{
		return TRUE;
	}

	m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
	if( MoveWaferTable(lEncX, lEncY)==FALSE )
	{
		LONG lHmiRow = 0, lHmiCol = 0;
		ConvertAsmToHmiUser(ulRow, ulCol, lHmiRow, lHmiCol);
		szMsg.Format("Move to Refer Die (%ld,%ld)[%lu,%lu] has motion error.", lHmiRow, lHmiCol, ulRow, ulCol);
		HmiMessage_Red_Back(szMsg, "Prescan");
		return FALSE;
	}

	Sleep(50);
	BOOL bDieResult = FALSE;
	LONG lDieOffsetX = 0, lDieOffsetY = 0;
	if( pUtl->GetPrescanDummyMap() )
	{
		// for furture to search die and pass the row/col of map to vision
		bDieResult = IM_DummySearchDie(ulRow, ulCol, lDieOffsetX, lDieOffsetY);
	}
	else
	{
		LONG	lRefDieStart = lReferID;
		LONG	lRefDieStop  = lReferID;
		if( lReferID<=0 )
		{
			if( m_bAllRefDieCheck )
			{
				lRefDieStart = 1;
				lRefDieStop  = m_lLrnTotalRefDie;
			}
			else
			{
				lRefDieStart = 1;
				lRefDieStop  = 1;
			}
		}

		for (int i = lRefDieStart; i <= lRefDieStop; i++)
		{
			bDieResult = WprSearchDie(FALSE, i, lDieOffsetX, lDieOffsetY);
			if( bDieResult )
			{
				break;
			}
		}

		if( bDieResult==FALSE )
		{
			Sleep(50);
			for (int i = lRefDieStart; i <= lRefDieStop; i++)
			{
				bDieResult = WprSearchDie(FALSE, i, lDieOffsetX, lDieOffsetY);
				if( bDieResult )
				{
					break;
				}
			}
		}
	}

	if( bDieResult==FALSE )		//If no GOOD/REF die is found
	{
		UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();

		m_WaferMapWrapper.SetReferenceDie(ulRow, ulCol, FALSE);
		m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNullBin);

		LONG lHmiRow = 0, lHmiCol = 0;
		ConvertAsmToHmiUser(ulRow, ulCol, lHmiRow, lHmiCol);
		szMsg.Format("Refer Die (%ld,%ld)[%lu,%lu] Check PR search fail\nDo you want to continue?", lHmiRow, lHmiCol, ulRow, ulCol);
		if( HmiMessage_Red_Back(szMsg, "Prescan", glHMI_MBX_YESNO)==glHMI_NO )
		{
			bResult = FALSE;
		}
	}
	else
	{
		//Save pos into map for this ref die
		lEncX = lEncX + lDieOffsetX;
		lEncY = lEncY + lDieOffsetY;
	}

	SetMapPhyPosn(ulRow, ulCol, lEncX, lEncY);
	SetPrescanPosition(ulRow, ulCol, lEncX, lEncY);

	return bResult;
}


VOID CWaferPr::SetNextHalfMapAsMissingDie(const BOOL bRowModeSeparateHalfMap, const ULONG ulAsmHalfRow, const LONG ulAsmHalfCol)
{
	CString szMsg;
	ULONG ulMapRowMax, ulMapColMax;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);

	if (bRowModeSeparateHalfMap)
	{
		//		MS90 scan align, to get new scan area physical range and set this as the half sort area.
		//		for first part only. should inner several rows by looping check and check within wafer tablelimit.

		LONG lOrgMapCol = 0;
		ConvertAsmToOrgUser(ulAsmHalfRow, ulAsmHalfCol, m_lMS90HalfDivideMapOrgRow, lOrgMapCol);
		szMsg.Format("MS90 1ST prescan map and hide second half divide map at %ld,%ld, asm (%lu,%lu)",
			m_lMS90HalfDivideMapOrgRow, lOrgMapCol, ulAsmHalfRow, ulAsmHalfCol);
		DWTDSortLog(szMsg);

		m_ulMS90HalfBorderMapRow = ulAsmHalfRow;

		m_WaferMapWrapper.StartBatchStateUpdate();
		for(ULONG ulRow = ulAsmHalfRow; ulRow<=ulMapRowMax; ulRow++)
		{
			for(ULONG ulCol=0; ulCol<=ulMapColMax; ulCol++)
			{
				if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol))
				{
					m_WaferMapWrapper.BatchSetMissingDie(ulRow, ulCol);
	//				m_WaferMapWrapper.HideDie(ulRow, ulCol);
				}
			}
		}
		m_WaferMapWrapper.BatchSetMissingDieUpdate();
	}
	else
	{
		//		MS50 scan align, to get new scan area physical range and set this as the half sort area.
		//		for first part only. should inner several rows by looping check and check within wafer tablelimit.

		LONG lOrgMapRow = 0;
		ConvertAsmToOrgUser(ulAsmHalfRow, ulAsmHalfCol, lOrgMapRow, m_lMS90HalfDivideMapOrgCol);
		szMsg.Format("MS90 1ST prescan map and hide second half divide map at %ld,%ld, asm (%lu,%lu)",
			lOrgMapRow, m_lMS90HalfDivideMapOrgCol, ulAsmHalfRow, ulAsmHalfCol);
		DWTDSortLog(szMsg);

		m_ulMS90HalfBorderMapCol = ulAsmHalfCol;

		//m_pWaferMapManager->BatchSetMissingDie(0, ulMapRowMax, 0, GetMS90HalfMapMaxCol());
		m_WaferMapWrapper.StartBatchStateUpdate();
		for (ULONG ulCol = 0; ulCol < GetMS90HalfMapMaxCol(); ulCol++)
		{
			for (ULONG ulRow = 0; ulRow <= ulMapRowMax; ulRow++)
			{
				if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol))
				{
					m_WaferMapWrapper.BatchSetMissingDie(ulRow, ulCol);
				}
			}
		}
		m_WaferMapWrapper.BatchSetMissingDieUpdate();
	}

	m_WaferMapWrapper.Redraw();
}


VOID CWaferPr::GetMinMaxRowCol(const ULONG ulTgtRow, const ULONG ulTgtCol,
							   ULONG &ulLastScnMaxRow, ULONG &ulLastScnMinRow,
							   ULONG &ulLastScnMaxCol, ULONG &ulLastScnMinCol)
{
	if (ulLastScnMaxRow < ulTgtRow)
	{
		ulLastScnMaxRow = ulTgtRow;
	}

	if (ulLastScnMaxCol < ulTgtCol)
	{
		ulLastScnMaxCol = ulTgtCol;
	}

	if (ulLastScnMinRow > ulTgtRow)
	{
		ulLastScnMinRow = ulTgtRow;
	}

	if (ulLastScnMinCol > ulTgtCol)
	{
		ulLastScnMinCol = ulTgtCol;
	}
}



VOID CWaferPr::ConstructPrescanMap()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulIndex;
	BOOL bInfo;
	ULONG lRefDieCount = 0;
	CString szConLogFile;

	UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGoodGrade	= m_ucPrescanGoodGrade + ucOffset;
	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + ucOffset;
	UCHAR ucDefectGrade	= GetScanDefectGrade() + ucOffset;
	UCHAR ucBadCutGrade	= GetScanBadcutGrade() + ucOffset;
	UCHAR ucReferGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();

	BOOL  bBadCutAction	= m_bPrescanBadCutAction;
	BOOL  bDefectAction	= m_bPrescanDefectAction;
	BOOL  bEmptyAction	= m_bPrescanEmptyAction;
	BOOL  bGoodAction	= m_bPrescanGoodAction;
	BOOL  bFakeAction	= m_bScanDetectFakeEmpty;
	UCHAR ucFakeGrade	= m_ucScanFakeEmptyGrade;
	BOOL  bExtraAction  = m_bScanExtraDieAction;
	UCHAR ucExtraGrade	= m_ucScanExtraGrade + ucOffset;
	if( bExtraAction==FALSE )
		ucExtraGrade = ucNullBin;

	if( m_bPrescanEmptyToNullBin )
		ucEmptyGrade = ucNullBin;
	if( m_bPrescanDefectToNullBin )
		ucDefectGrade = ucNullBin;
	if (m_bPrescanBadCutToNullBin)
		ucBadCutGrade = ucNullBin;

	LONG lCounter = 0;
	if( m_lBuildShortPathInScanning==2 )
	{
		while( 1 )
		{
			//	0 = RESUME_ALGORITHM_NOT_START means the algorithm is still in suspending state.
			//	1 = RESUME_ALGORITHM_STARTED means the resume is started.
			//	2 = RESUME_ALGORITHM_COMPLETED means the resume is finished. 
			if( m_WaferMapWrapper.GetResumeAlgorithmStatus()==RESUME_ALGORITHM_COMPLETED )
			{
				SaveScanTimeEvent("WPR: short path finished");
				break;
			}

			Sleep(100);
			lCounter++;

			if( lCounter>600 )
			{
				SaveScanTimeEvent("WPR: short path timeout");
				break;
			}
		}
	}

	BOOL bShortPathNgDieToInvalid	= FALSE;
	CString szAlgorithm, szPathFinder;
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
	if( szAlgorithm.Find("Sorting Path 1")!=-1 )
	{
		bShortPathNgDieToInvalid = TRUE;
	}

	if( pApp->GetCustomerName()=="NSS" )
	{
		bShortPathNgDieToInvalid = TRUE;
	}

	m_bToUploadScanMapFile = TRUE;

	CString szMsg;
	szMsg.Format("WPR: scan map change grade: Badcut %d(%d); Defect %d(%d); Empty %d(%d); Good %d(%d); extra %d(%d); Fake %d(%d)", 
		bBadCutAction,	ucBadCutGrade,
		bDefectAction,	ucDefectGrade,
		bEmptyAction,	ucEmptyGrade,
		bGoodAction,	ucGoodGrade,
		bExtraAction,	ucExtraGrade,
		bFakeAction,	ucFakeGrade);
	SaveScanTimeEvent(szMsg);
	szMsg.Format("WPR: prescan done to build map; total index counter %d", m_nPrescanIndexCounter);
	if( m_lScanPrAbnormalCount>0 )
	{
		CString szTemp;
		szTemp.Format(", PR abnormal %ld", m_lScanPrAbnormalCount);
		SetErrorMessage(szTemp);
		szMsg += szTemp;
	}
	SaveScanTimeEvent(szMsg);

	DOUBLE dUsedTime = GetTime();

	m_lPrescanSortingTotal	= 0; 
	m_lRescanMissingTotal	= 0;
	m_lPrescanVerifyResult	= 0;
	if( m_bScanRunTimeDisplay )
	{
		szMsg.Format("WPR: to restore run time updated map");
		SaveScanTimeEvent(szMsg);
		LONG lRow = 0, lCol = 0;
		ULONG ulGrade = 0;
		for(ulIndex=1; ulIndex<=WSGetScanRemainDieTotal(); ulIndex++)
		{
			if( WSGetScanRemainDieFromList(ulIndex, lCol, lRow, ulGrade) )
			{
				m_WaferMapWrapper.ChangeGrade(lRow, lCol, (UCHAR)ulGrade);
			}
		}
		WSClearScanRemainDieList();
	}

	DelScanMapRecordID();
	pUtl->DelReferMapPoints();

	DOUBLE dTime = GetTime();
	if( m_bSendWaferIDToPR )	//	prescan setting, Send Wafer ID to PR side
	{
		for(int i=0; i<MAX_PR_GALLERY_SEARCH_THREADS; i++)
		{
			PR_PROCESS_MGT_CMD            stCmd;
			PR_PROCESS_MGT_RPY            stRpy;
			// end before start again
			PR_InitProcessMgtCmd(&stCmd);
			stCmd.emProcessCmd = PR_PROCESS_CMD_END_LOT;

			sprintf_s((char*)(stCmd.aubID), sizeof(stCmd.aubID), m_szToPrWaferID); 
			PR_ProcessMgtCmd(&stCmd, GALLERY_SEARCH_BASE_SID+i, GALLERY_SEARCH_BASE_RID+i, &stRpy);
		}
		SaveScanTimeEvent("WPR: wafer scanning complete: " + m_szToPrWaferID);
	}
	m_lTimeSlot[6] = (LONG)(GetTime()-dTime);		//	06.	Process Mgt Cmd time before build map.

	// capture last image
	LONG lEncX, lEncY;
	if( IsES101()==FALSE && IsES201()==FALSE )	//	4.24TX 4
		WprSearchDie(TRUE, 1, lEncX, lEncY);

	SavePrescanInfoPr();
	m_bBinMapShow		= FALSE;
	m_awMapReferListRow.RemoveAll();
	m_awMapReferListCol.RemoveAll();
	m_awWaferRefListRow.RemoveAll();
	m_awWaferRefListCol.RemoveAll();


	if( pUtl->GetPrescanDummyMap() && pUtl->GetPrescanAoiWafer()==FALSE )
	{
		return ConstructPrescanMap_Dummy2();
	}

	if( pUtl->GetPrescanBarWafer() )
	{
		return ConstructPrescanMap_BarWafer();
	}

	CUIntArray aulAllGradeList, aulSelGradeList, aulMinGradeList;
	ULONG ulDieTotal = 0;
	ULONG ulLeft = 0;
	ULONG ulPick = 0;
	ULONG ulMinCount = 0;

	ulMinCount =	(LONG)(*m_psmfSRam)["Wafer Table Options"]["Min Grade Count"];
	BOOL bInSelectedList = FALSE;

	m_WaferMapWrapper.GetAvailableGradeList(aulAllGradeList);
	m_WaferMapWrapper.GetSelectedGradeList(aulSelGradeList);

	for (int k=0; k<aulAllGradeList.GetSize(); k++)
	{
		m_WaferMapWrapper.GetStatistics(aulAllGradeList.GetAt(k), ulLeft, ulPick, ulDieTotal);

		bInSelectedList = FALSE;
		for(int j=0; j<aulSelGradeList.GetSize(); j++)
		{
			if( aulAllGradeList.GetAt(k)==aulSelGradeList.GetAt(j) )
			{
				bInSelectedList = TRUE;
				break;
			}
		}

		if( bInSelectedList )
		{
			if( ulMinCount>=ulDieTotal )
			{
				aulMinGradeList.Add(aulAllGradeList.GetAt(k));
			}
		}
	}


//============================================================================================================================================================
//  Construct Map End
//============================================================================================================================================================
//======================================================================================================
//		Find reference die
//======================================================================================================
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	CDWordArray dwaRefWfX, dwaRefWfY, dwaRefRow, dwaRefCol;
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);

	ULONG ulMapRowMax, ulMapColMax;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);

	ULONG ulDiePitchX = 0, ulDiePitchY = 0;

	ulDiePitchX = labs(GetDiePitchX_X());
	ulDiePitchY = labs(GetDiePitchY_Y());
	dwaRefWfX.RemoveAll();
	dwaRefWfY.RemoveAll();
	dwaRefRow.RemoveAll();
	dwaRefCol.RemoveAll();

	//Get available wafermap grade & its total count
	CUIntArray aulAllGradesList, aulGradeTotalCount;
	aulGradeTotalCount.RemoveAll();
	UCHAR ucCheckGrade;
	ULONG ulGradeLeft, ulGradePick, ulGradeTotal, jGrade;
	m_WaferMapWrapper.GetAvailableGradeList(aulAllGradesList);
	for(jGrade=0; jGrade<((ULONG)aulAllGradesList.GetSize()); jGrade++)
	{
		ucCheckGrade = (UCHAR)aulAllGradesList[jGrade];
		m_WaferMapWrapper.GetStatistics(ucCheckGrade, ulGradeLeft, ulGradePick, ulGradeTotal);
		aulGradeTotalCount.Add(ulGradeTotal);
	}

	LONG	lMapScanOffsetRow = 0 - WPR_SCAN_MAP_OFFSET;
	LONG	lMapScanOffsetCol = 0 - WPR_SCAN_MAP_OFFSET;
	m_lMapScanOffsetRow = lMapScanOffsetRow;	// map - scan
	m_lMapScanOffsetCol = lMapScanOffsetCol;	// map - scan
	CString szScnMapLog, szScnTemp, szSpace;
	szSpace = "\n             	      	";
	szScnMapLog.Format("Map Scn offset %ld,%ld", lMapScanOffsetRow, lMapScanOffsetCol);

	// add all alignment point and refer points to list
	LONG lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY;
	pUtl->GetAlignPosition(lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY);
	LONG lHomeScnRow = lHomeOriRow + WPR_SCAN_MAP_OFFSET;
	LONG lHomeScnCol = lHomeOriCol + WPR_SCAN_MAP_OFFSET;
	if (!pSRInfo->IsManualAlignRegion() && IsScanAlignWafer() )
	{
		lHomeScnRow += ulMapRowMax;
		lHomeScnCol += ulMapColMax;
	}

	dwaRefWfX.Add(lHomeWfX);
	dwaRefWfY.Add(lHomeWfY);
	dwaRefRow.Add(lHomeScnRow);
	dwaRefCol.Add(lHomeScnCol);
	lRefDieCount++;
/*
	//2019.03.06 remeove it
	if( IsOutMS90SortingPart((ULONG)lHomeOriRow, (ULONG)lHomeOriCol) )
	{
		CString szMsg;
		LONG lUserRow, lUserCol;
		ConvertAsmToOrgUser(lHomeOriRow, lHomeOriCol, lUserRow, lUserCol);
		szMsg.Format("Align refer die %d,%d,(%d,%d) not in sorting part", lHomeOriRow, lHomeOriCol, lUserRow, lUserCol);
		HmiMessage_Red_Back(szMsg);
		SetErrorMessage(szMsg);
	}
*/
	LONG lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY;
	ULONG ulTgtRegion = 0, ulAlnRegion = 0;
	if( pUtl->GetPrescanRegionMode() )
	{
		ulTgtRegion = pSRInfo->GetTargetRegion();
		ulAlnRegion = pSRInfo->GetAssistRegion();
/*
		//2018.4.24
		if (pSRInfo->IsWithinThisRegion(ulTgtRegion, (ULONG)lHomeOriRow, (ULONG)lHomeOriCol)==FALSE )
		{
			CString szMsg;
			szMsg.Format("Align refer die (%d,%d) not in align region %d", lHomeOriRow, lHomeOriCol, ulTgtRegion);
			if( pApp->GetCustomerName()!=CTM_SEMITEK )
				HmiMessage_Red_Back(szMsg);
			SetErrorMessage(szMsg);
		}
*/

		if( pSRInfo->IsWithinThisRegion(ulAlnRegion, (ULONG)lHomeOriRow, (ULONG)lHomeOriCol)==FALSE )
		{
			CString szMsg;
			szMsg.Format("Align refer die (%d,%d) not in align region %d", lHomeOriRow, lHomeOriCol, ulAlnRegion);
			if( pApp->GetCustomerName()!=CTM_SEMITEK )
				HmiMessage_Red_Back(szMsg);
			SetErrorMessage(szMsg);
		}
	}
	else
	{
		for(ulIndex=0; ulIndex<pUtl->GetNumOfReferPoints(); ulIndex++)
		{
			if( pUtl->GetReferPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
			{
				dwaRefWfX.Add(lAlgnWfX);
				dwaRefWfY.Add(lAlgnWfY);
				if( IsScanAlignWafer() )
				{
					lAlgnRow += ulMapRowMax;
					lAlgnCol += ulMapColMax;
				}
				lAlgnRow += WPR_SCAN_MAP_OFFSET;
				lAlgnCol += WPR_SCAN_MAP_OFFSET;
				dwaRefRow.Add(lAlgnRow);
				dwaRefCol.Add(lAlgnCol);
				lRefDieCount++;
			}
		}
	}

	for(ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		if( pUtl->GetAssistPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
		{
			if( pUtl->GetPrescanRegionMode() )
			{
				if( pSRInfo->IsWithinThisRegion(ulTgtRegion, lAlgnRow, lAlgnCol)==FALSE &&
					pSRInfo->IsWithinThisRegion(ulAlnRegion, lAlgnRow, lAlgnCol)==FALSE )
				{
					continue;
				}
			}
			if( IsOutMS90SortingPart(lAlgnRow, lAlgnCol) )
			{
				continue;
			}
			if( lAlgnRow==lHomeOriRow && lAlgnCol==lHomeOriCol )
				continue;
			dwaRefWfX.Add(lAlgnWfX);
			dwaRefWfY.Add(lAlgnWfY);
			if (!pSRInfo->IsManualAlignRegion() && IsScanAlignWafer() )
			{
				lAlgnRow += ulMapRowMax;
				lAlgnCol += ulMapColMax;
			}
			lAlgnRow += WPR_SCAN_MAP_OFFSET;
			lAlgnCol += WPR_SCAN_MAP_OFFSET;
			dwaRefRow.Add(lAlgnRow);
			dwaRefCol.Add(lAlgnCol);
			lRefDieCount++;
		}
	}

	ClearGoodInfo();
//======================================================================================================
//		Find reference die end
//======================================================================================================


//======================================================================================================
//		Create reference die prescan file
//======================================================================================================
	szConLogFile = szLogPath + PRESCAN_PASSIN_PSN;
	FILE *fpPassIn = NULL;
	errno_t nErr = -1;
	if( pUtl->GetPrescanDebug() )
	{
		nErr = fopen_s(&fpPassIn, szConLogFile, "w");
	}

	BOOL bNgGradeAdded = FALSE;
	UCHAR ucNgGradeList[256];
	memset(ucNgGradeList, 255, sizeof(ucNgGradeList));

	// add all prescan die data list to physical map builder include refer points
	for(ulIndex=0; ulIndex<lRefDieCount; ulIndex++)
	{
		lEncX = dwaRefWfX.GetAt(ulIndex);
		lEncY = dwaRefWfY.GetAt(ulIndex);
		LONG lIndex = 0-ulIndex;
		if( ulIndex==0 && m_bSearchHomeOption==WT_CORNER_SEARCH && m_ucCornerAlignDieState==1 )	//	corner no die
		{
			lIndex = WPR_ADD_DIE_INDEX - 400;
		}
		m_cPIMap.SortAdd_Tail(lEncX, lEncY, lIndex, FALSE);
		if ((nErr == 0) && (fpPassIn != NULL))
		{
			fprintf(fpPassIn, "%ld,%ld,%ld,%ld,%ld\n",
				lEncX, lEncY, dwaRefRow.GetAt(ulIndex), dwaRefCol.GetAt(ulIndex), lIndex);
		}
	}
//======================================================================================================
//		Create reference die prescan file end
//======================================================================================================

	m_ucCornerAlignDieState = 0;	//	reset to default

	// build physical map
	int nMaxSpan = 5;	// nMaxAllowPitch
	BOOL bPreSorted = TRUE;
	int nMaxIterateCount = -1;
	int nMaxAllowLeft = 10;
	ULONG ulDieSizeXTol = (ULONG) (GetDieSizeX() * m_dPrescanPitchTolX/100.0);
	ULONG ulDieSizeYTol = (ULONG) (GetDieSizeY() * m_dPrescanPitchTolY/100.0);

	SaveScanTimeEvent("WPR: begin pass massive data to builder");

	BOOL bLargeDie = CPreBondEvent::m_nMapOrginalCount<=SCAN_SMS_DIE_MAX_LIMIT && m_bSmallMapSortRpt && WSGetPrescanTotalDie()<=(SCAN_SMS_DIE_MAX_LIMIT*2);
	BOOL bGetRemain = FALSE;
	if( bLargeDie )
	{
		bGetRemain = TRUE;	//	bar wafer or large die
	}
	if( bLargeDie )
	{
		nMaxSpan = 2;	//	auto refill
	}
	BOOL bFindAlnNearby = FALSE;
	if( bGetRemain )
	{
		bFindAlnNearby = TRUE;
	}

	if ((nErr == 0) && (fpPassIn != NULL))
	{
		fprintf(fpPassIn, "%lu,%lu,%lu,%lu,%d,%d,%d,%d,%ld,%ld,%ld,%ld\n", 
			ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY, 
			bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan, GetDieSizeX(), GetDieSizeY(), GetDiePitchX_X(), GetDiePitchY_Y());
	}


	//	Put prescan result data into construct map function (m_cPIMap)
	if( IsPrecanWith2Pr() )	// pass raw data to PIMap builder
	{
		ULONG *pulReferBadList;
		pulReferBadList = new ULONG[WSGetPrescanTotalDie() + 10];
		memset(pulReferBadList, 0, sizeof(pulReferBadList));
		ULONG lReferBadIndex = 0;
		// refer scan, make sure good die not a refer die and 
		//						 refer die not a good die and 
		//						 refer die to refer die less 2 size and large 0.2 
		if( IsPrescanReferDie() )
		{
			for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
			{
				USHORT uwDiePrID = 0;
				if( WSGetPosnPrID(ulIndex, lEncX, lEncY, uwDiePrID)==false )
					continue;

				if( uwDiePrID==m_uwScanRecordID[0] )	// good, to check not is a refer die.
				{
					for(ULONG ulChkIndex=1; ulChkIndex<=WSGetScanReferTotalDie(); ulChkIndex++)
					{
						LONG lChkX, lChkY;
						if( WSGetScanReferPosition(ulChkIndex, lChkX, lChkY)!=true )
						{
							continue;
						}

						if( abs(lEncX-lChkX)<abs(GetDieSizeX()*3/4) && abs(lEncY-lChkY)<abs(GetDieSizeY()*3/4) )
						{
							pulReferBadList[lReferBadIndex] = ulIndex;
							lReferBadIndex++;
							break;
						}
					}	// find in reference list
				}	// good, to check not is a refer die.
				else	// refer, can not be a good die and can not too near to another refer die
				{
					for(ULONG ulChkIndex=1; ulChkIndex<=WSGetScanReferTotalDie(); ulChkIndex++)
					{
						LONG lChkX, lChkY;
						if( WSGetScanReferPosition(ulChkIndex, lChkX, lChkY)!=true )
						{
							continue;
						}

						if( (abs(lEncX-lChkX)>abs(GetDieSizeX()/5) || abs(lEncY-lChkY)>abs(GetDieSizeY()/5)) &&
							(abs(lEncX-lChkX)<abs(GetDieSizeX()*2) && abs(lEncY-lChkY)<abs(GetDieSizeY()*2)) )
						{
							pulReferBadList[lReferBadIndex] = ulIndex;
							lReferBadIndex++;
							break;
						}
					}	// find in reference list, not too near
					for(ULONG ulChkIndex=1; ulChkIndex<=WSGetPrescanTotalDie(); ulChkIndex++)
					{
						LONG lChkX, lChkY;
						USHORT uwChkPrID = 0;
						if( WSGetPosnPrID(ulChkIndex, lChkX, lChkY, uwChkPrID)==false )
						{
							continue;
						}

						if( uwChkPrID==m_uwScanRecordID[0] )
						{
							if( abs(lEncX-lChkX)<abs(GetDieSizeX()*3/4) && abs(lEncY-lChkY)<abs(GetDieSizeY()*3/4) )
							{
								pulReferBadList[lReferBadIndex] = ulIndex;
								lReferBadIndex++;
								break;
							}
						}
					}	// check in good list, if it is a good die, skip
				}	// refer, can not be a good die and can not too near to another refer die
			}	// all list
		}	// refer die scan

		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
			{
				continue;
			}

			if( IsPrescanReferDie() && lReferBadIndex>0 )
			{
				for(ULONG ulChkIndex=0; ulChkIndex<lReferBadIndex; ulChkIndex++)
				{
					if( pulReferBadList[ulChkIndex]==ulIndex )
					{
						continue;
					}
				}
			}

			m_cPIMap.SortAdd_Tail(lEncX, lEncY, ulIndex, FALSE);
			if ((nErr == 0) && (fpPassIn != NULL))
			{
				fprintf(fpPassIn, "%ld,%ld,%lu\n", lEncX, lEncY, ulIndex);
			}
		}
		delete[] pulReferBadList;
	}	//	end of 2nd pr prescan
	else
	{
		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			USHORT uwDiePrID;

			if(	WSGetPosnPrID(ulIndex, lEncX, lEncY, uwDiePrID)==FALSE )
				continue;

			if ((nErr == 0) && (fpPassIn != NULL))
			{
				fprintf(fpPassIn, "%ld,%ld,%ld\n", lEncX, lEncY, ulIndex);
			}

			if( bFindAlnNearby==FALSE &&
				labs(lHomeWfX-lEncX)<=labs(ulDieSizeXTol+ulDiePitchX) && 
				labs(lHomeWfX-lEncX)>labs(ulDieSizeXTol) &&
				labs(lHomeWfY-lEncY)<=labs(ulDieSizeYTol+ulDiePitchY) &&
				labs(lHomeWfY-lEncY)>labs(ulDieSizeYTol) )
			{
				bFindAlnNearby = TRUE;
			}

			if( uwDiePrID==666 )
			{
				m_cPIMap.SortAdd_Tail(lEncX, lEncY, ulIndex, FALSE);
			}
		}
	}

	if( bFindAlnNearby==FALSE || (m_lLookAroundRow>0 || m_lLookAroundCol>0) )
	{
		LONG lCtrRow = 1;
		LONG lCtrCol = 1;
		if( m_lLookAroundRow>0 || m_lLookAroundCol>0 )
		{
			lCtrRow = m_lLookAroundRow;
			lCtrCol = m_lLookAroundCol;
		}
		if( bLargeDie )
		{
			lCtrRow = lCtrCol = 0;
		}

		LONG lDiePitchX_X	= GetDiePitchX_X();
		LONG lDiePitchX_Y	= GetDiePitchX_Y();
		LONG lDiePitchY_Y	= GetDiePitchY_Y();
		LONG lDiePitchY_X	= GetDiePitchY_X();

		for(LONG iRow=0; iRow<(lCtrRow*2+1); iRow++)
		{
			for(LONG jCol=0; jCol<(lCtrCol*2+1); jCol++)
			{
				if(iRow==lCtrRow && jCol==lCtrCol )
					continue;
				LONG lDiffX = jCol - lCtrCol;
				LONG lDiffY = iRow - lCtrRow;
				lEncX = lHomeWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
				lEncY = lHomeWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
				LONG lIndex = WPR_ADD_DIE_INDEX - iRow*(lCtrCol*2+1) - jCol;
				if( bGetRemain )
				{
					BOOL bFindNearby = FALSE;
					for(ULONG ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
					{
						USHORT uwDiePrID;
						LONG lScanX, lScanY;
						if(	WSGetPosnPrID(ulIndex, lScanX, lScanY, uwDiePrID)==FALSE )
						{
							continue;
						}

						if( labs(lScanX-lEncX)<=labs(ulDieSizeXTol) && 
							labs(lScanY-lEncY)<=labs(ulDieSizeYTol) )
						{
							bFindNearby = TRUE;
							break;
						}
					}
					if( bFindNearby )
					{
						continue;
					}
				}
				m_cPIMap.SortAdd_Tail(lEncX, lEncY, lIndex, FALSE);
				if ((nErr == 0) && (fpPassIn != NULL))
				{
					fprintf(fpPassIn, "%ld,%ld,%ld\n", lEncX, lEncY, lIndex);
				}
			}
		}
	}

	if ((nErr == 0) && (fpPassIn != NULL))
	{
		fclose(fpPassIn);
	}
//======================================================================================================
//		close reference die prescan file
//======================================================================================================


	// BGETREMAIN
	bool *pbGetOut;
	pbGetOut = new bool[WSGetPrescanTotalDie()+10];
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		pbGetOut[ulIndex] = false;
	}
	// BGETREMAIN

	if( IsPrecanWith2Pr() )
	{
		m_cPIMap.KeepIsolatedDice(FALSE);
		m_cPIMap.SetScan2Pr(TRUE);
	}
	else
	{
		m_cPIMap.KeepIsolatedDice(FALSE);
		m_cPIMap.SetScan2Pr(FALSE);
	}

	SaveScanTimeEvent("WPR: to construct normal map");
	if( GetDieShape()==WPR_RHOMBUS_DIE )
	{
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();
		m_cPIMap.SetXOffset(lDiePitchY_X);
		szMsg.Format("WPR: die pitch x %d,%d, y %d,%d", ulDiePitchX, lDiePitchX_Y, ulDiePitchY, lDiePitchY_X);
		SaveScanTimeEvent(szMsg);
	}
	else
	{
		m_cPIMap.SetXOffset(0);
	}

	//Construct new map according to prescan result.
	m_cPIMap.ConstructMap(dwaRefWfX, dwaRefWfY, dwaRefRow, dwaRefCol,
		ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY,
		bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);


	SaveScanTimeEvent("WPR: construct map complete");
//============================================================================================================================================================
//  Construct Map End
//============================================================================================================================================================


	m_lTimeSlot[7] = (LONG)(GetTime()-dUsedTime);	//	07.	constuct map used time.
	SaveScanTimeEvent("WPR: AFTER build PHY MAP");

	ULONG ulRow, ulCol;
	UCHAR ucPrescanEdgeSize = m_ucPrescanEdgeSize;

	SaveScanTimeEvent("WPR: to clear prescan info");
	ClearPrescanInfo();
	SaveScanTimeEvent("WPR: to reset map phy posn");
	ResetMapPhyPosn();

	SaveScanTimeEvent("WPR: clear physical prescan and map info done");

	// get all phsical list infromation to prescan result list
	FILE *fpMap = NULL;
	nErr = -1;
	CString szScanPmpFile = szLogPath + "_Scan.pmp";
	DeleteFile(szScanPmpFile);
	if( pUtl->GetPrescanDebug() || IsScanAlignWafer() )
	{
		nErr = fopen_s(&fpMap, szScanPmpFile, "w");
	}

	BOOL bFirstGood = FALSE;
	ULONG ulScnMinColEdge = 0;
	ULONG ulScnMaxRow = 0, ulScnMaxCol = 0, ulScnMinRow = 9999, ulScnMinCol = 9999;
	ULONG ulMapMaxRow = GetMapValidMaxRow();
	ULONG ulMapMaxCol = GetMapValidMaxCol();
	ULONG ulMapMinRow = GetMapValidMinRow();
	ULONG ulMapMinCol = GetMapValidMinCol();
	ULONG ulPyiRowMax = 0, ulPyiColMax = 0;
	m_cPIMap.GetDimension(ulPyiRowMax, ulPyiColMax);

	//Get the scan range(MinCol~MaxCol, MinRow~MaxRow)
	if ((nErr == 0) && (fpMap != NULL))
	{
		fprintf(fpMap, "PHY range Row max %lu  Col max %lu\n", ulPyiRowMax, ulPyiColMax);
		fprintf(fpMap, "MAP range Row max %lu  Col max %lu\n", ulMapRowMax, ulMapColMax);
		fprintf(fpMap, "DataFileName,,%s\n", (LPCTSTR)szScanPmpFile);
		fprintf(fpMap, "LotNumber,,\nDeviceNumber,,\nwafer id=\nTestTime,\nMapFileName,,\nTransferTime,\n");
		fprintf(fpMap, "\n");
	}

	//========================================================================================================
	//   Find the region (min scan-row, max scan-row, min scan-column, max scan-column)
	//========================================================================================================
	BOOL bFindLimit = FALSE;
	LONG lScanTotalDieCounter = 0;
	for(ulCol = ulPyiColMax; ulCol > 0; ulCol--)
	{
		for(ulRow = 0; ulRow <= ulPyiRowMax; ulRow++)
		{
			BOOL bDieOverLap = FALSE;
			LONG lIndex = 0;
			bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if( bInfo == FALSE )
			{
				continue;
			}

			if (lIndex <= WPR_ADD_DIE_INDEX)
			{
				continue;
			}
			
			if (!IsWithinScanLimit(lEncX, lEncY))
			{
				//if it exceed the wafer limit in the current column, the scan minimum column should move 1 column in right direction
				if( IsScanThenDivideMap() && IsMS90HalfSortMode() && (IsMS90Sorting2ndPart() == FALSE))
				{
/*
					LONG lEdgeX = labs(GetDiePitchXX()*2);
					if (lEdgeX > 1500)
					{
						lEdgeX = 1500;
					}
					LONG lPosLimitX = m_lWafXPosLimit - lEdgeX;
					if (lEncX > lPosLimitX)
					{
						if (ulScnMinCol == ulCol && ulCol > 0)
						{
							ulScnMinCol = ulCol + 1;
							bFindLimit = TRUE;
                            break;
						}
					}
*/
					if ((ulRow > 0) && (ulScnMinCol == ulCol) && (ulCol > 0))
					{
						ulScnMinCol = ulCol + 1;
						ulScnMinColEdge = ulScnMinCol;
					}
					bFindLimit = TRUE;
					break;
				}
				continue;
			}

			UCHAR ucMapGrade = 1;
			if( IsScanAlignWafer() && IsPrecanWith2Pr() && IsPrescanReferDie() )
			{
				if( lIndex<0 )
				{
					continue;
				}
				PR_UWORD uwDiePrID = 0;
				if( WSGetPosnPrID(lIndex, lEncX, lEncY, uwDiePrID) )
				{
					if( uwDiePrID==m_uwScanRecordID[1] )
					{
						m_awWaferRefListRow.Add(ulRow);
						m_awWaferRefListCol.Add(ulCol);
						ucMapGrade = 2;
					}
				}
			}

			if ((nErr == 0) && (fpMap != NULL))
			{
				if( bFirstGood==FALSE )
				{
					fprintf(fpMap, "map data\n");
					fprintf(fpMap, "%lu,%lu,\n", ulCol, ulRow);
					bFirstGood = TRUE;
				}
				fprintf(fpMap, "%lu,%lu,%d\n", ulCol, ulRow, ucMapGrade);
				lScanTotalDieCounter++;
			}

			if (ulRow > ulScnMaxRow)
			{
				ulScnMaxRow = ulRow;
			}
			if (ulCol > ulScnMaxCol)
			{
				ulScnMaxCol = ulCol;
			}
			if (ulRow < ulScnMinRow)
			{
				ulScnMinRow = ulRow;
			}
			if ((ulCol < ulScnMinCol) && (ulCol > ulScnMinColEdge))
			{
				ulScnMinCol = ulCol;
			}
		}

		if( bFindLimit )
		{
			break;
		}
	}

	if ((nErr == 0) && (fpMap != NULL))
	{
		fclose(fpMap);
	}
	//========================================================================================================


	szMsg.Format("WPR: get out PHY range Row(%lu,%lu)  Col(%lu,%lu)",
		ulScnMinRow, ulScnMaxRow, ulScnMinCol, ulScnMaxCol);
	SaveScanTimeEvent(szMsg);
	szMsg.Format("WPR: get out MAP range Row(%lu,%lu)  Col(%lu,%lu)",
		ulMapMinRow, ulMapMaxRow, ulMapMinCol, ulMapMaxCol);
	SaveScanTimeEvent(szMsg);

	if( IsScanThenDivideMap() && IsMS90HalfSortMode() && IsMS90Sorting2ndPart()==FALSE )
	{
		//		MS90 scan align, to get new scan area physical range and set this as the half sort area.
		//		for first part only. should inner several rows by looping check and check within wafer tablelimit.

		ULONG ulAsmHalfRow = 0;
		ULONG ulAsmHalfCol = 0;

		ULONG ulMapRowMax, ulMapColMax;
		WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);

		BOOL bRowModeSeparateHalfMap = IsRowModeSeparateHalfMap();
		if (bRowModeSeparateHalfMap)
		{
			ulAsmHalfRow = lHomeOriRow + ulScnMaxRow - lHomeScnRow;
			ulAsmHalfCol = ulMapMaxCol/2;
		}
		else
		{
			ulAsmHalfRow = ulMapMaxRow;
			//ulAsmHalfCol = lHomeOriCol + ulScnMaxCol - lHomeScnCol;
			//LONG lLeftSideEndCol = ulMapColMax - ulAsmHalfCol;
			LONG lLeftSideEndCol = lHomeOriCol + ulScnMinCol - lHomeScnCol + _round(m_nPrescanIndexStepCol / 2);
			if (lLeftSideEndCol < 0)
			{
				lLeftSideEndCol = 0;
			}
			ulAsmHalfCol = lLeftSideEndCol;
		}

		SetNextHalfMapAsMissingDie(bRowModeSeparateHalfMap, ulAsmHalfRow, ulAsmHalfCol);
	}

	//v4.40T10	//Nichia auto wafer alignment fcn
	//Create WaferHole txt file dynamically for each map file loaded
	if (m_bScanAlignFromFile && CMS896AStn::m_oNichiaSubSystem.IsEnabled())
	{
		CString szFileName = gszROOT_DIRECTORY + "\\Exe\\WaferHole.txt";
		AutoLoadScanAlignSettingFile(szFileName, FALSE);
	}


	//========================================================================================================
	//   Find Reference die or hole
	//========================================================================================================
	BOOL bCheckFail = FALSE;
	if (!pSRInfo->IsManualAlignRegion() && IsScanAlignWafer())
	{
		if (IsPrecanWith2Pr() && IsPrescanReferDie())
		{	//	NICHIA	here to check, ER10 is refer die in map file
			//	CString szBasePt = CMS896AStn::m_oNichiaSubSystem.GetMapBasePt();
			//	if( szBasePt!="A" )	// full wafer.
			SaveScanTimeEvent("WPR: map-wafer refer match in use");
			szMsg = "";
			for (ULONG ulRow = ulMapMinRow; ulRow <= ulMapMaxRow; ulRow++)
			{
				for(ULONG ulCol = ulMapMinCol; ulCol <= ulMapMaxCol; ulCol++)
				{
					if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) )
					{
						m_awMapReferListRow.Add(ulRow);
						m_awMapReferListCol.Add(ulCol);
					}
				}
			}
			SaveScanTimeEvent("WPR: sort scan align refer die from map");
			if( m_awMapReferListRow.GetSize()>0 && m_awMapReferListCol.GetSize()>0 )
			{
				SaveScanTimeEvent("WPR: check refer in physical wafer");
				BOOL bFindMatchHome = ScanAlignWaferMapWithRefer(ulScnMinRow, ulScnMinCol, ulScnMaxRow, ulScnMaxCol, lMapScanOffsetRow, lMapScanOffsetCol);
				if( bFindMatchHome==FALSE )
				{
					szMsg = "No match refer found in scan wafer and map.";
					bCheckFail = TRUE;
				}
				szScnTemp.Format("%sWithRefer %ld,%ld", szSpace, lMapScanOffsetRow, lMapScanOffsetCol);
				szScnMapLog += szScnTemp;
			}
			else
			{
				szMsg = "Refer scan align map file wrong.";
				bCheckFail = TRUE;
			}

			SaveScanTimeEvent("WPR: sort scan align refer finished");
		}
		else
		{
			if( m_bScanAlignFromFile || m_bScanAlignAutoFromMap )
			{
				SaveScanTimeEvent("WPR: map-wafer hole match check in use");
				szMsg = "";
				LONG lFindMatchCounter = 0;
				if ( m_bScanAlignAutoFromMap)
				{
					szMsg.Format("WPR: auto sort scan align hole die from map: Pattern points min %ld max %ld, total points min %ld. holes per %f",
						m_lScanAlignPatternHolesMin, m_lScanAlignPatternHolesMax, m_lScanAlignTotalHolesMin, m_dScanAlignMatchLowPercent);
					SaveScanTimeEvent(szMsg);
					szMsg = "";
					lFindMatchCounter = AutoScanAlignWaferWithMap(ulScnMinRow, ulScnMinCol, ulScnMaxRow, ulScnMaxCol, lMapScanOffsetRow, lMapScanOffsetCol, szMsg);
					SaveScanTimeEvent("WPR: Auto Scan Align Wafer WithMap done");
					szScnTemp.Format("%sWithMap %ld,%ld", szSpace, lMapScanOffsetRow, lMapScanOffsetCol);
					szScnMapLog += szScnTemp;
					if (lFindMatchCounter != 1)
					{
						lFindMatchCounter = 0;
						SaveScanTimeEvent("WPR: " + szMsg);
						bCheckFail = TRUE;
					}
				}
				else if( m_bScanAlignFromFile )
				{
					BOOL 	bScanAlignMap = SortScanAlignHoleOrder();
					SaveScanTimeEvent("WPR: sort scan align hole die from file");
					if( bScanAlignMap )
					{
						SaveScanTimeEvent("WPR: check hole in physical wafer");
						lFindMatchCounter = ScanAlignPhysicalWaferWithHoles(ulScnMinRow, ulScnMinCol, ulScnMaxRow, ulScnMaxCol, lMapScanOffsetRow, lMapScanOffsetCol);
						if( lFindMatchCounter==0 )
						{
							szMsg = "No match hole found in scan wafer and map.";
							bCheckFail = TRUE;
						}
						szMsg.Format("With holes found %ld in scan wafer and map with offset %ld,%ld.",
							lFindMatchCounter, lMapScanOffsetRow, lMapScanOffsetCol);
						szScnTemp.Format("%s%s", szSpace, szMsg);
						szScnMapLog += szScnTemp;
					}
					else
					{
						szMsg = "Scan align map setting file wrong.";
						bCheckFail = TRUE;
					}
				}
				SaveScanTimeEvent("WPR: " + szMsg);

				SaveScanTimeEvent("WPR: sort scan align die finished");
				// SanAn partial wafer (broken wafer) barcode has a dot
				if( lFindMatchCounter==0 && pApp->GetCustomerName()==CTM_SANAN && GetMapNameOnly().Find(".")!=-1 )
				{
					SaveScanTimeEvent("No hole found in map file " + GetMapNameOnly());
					CString szMsg;
					// the cross hair coordinate is 0,0
					ULONG ulHomeRow = 0, ulHomeCol = 0;
					LONG lScanRow = 0, lScanCol = 0;
					if( FindSanAnCrossPatternInMap(0, 0, ulHomeRow, ulHomeCol) &&
						MatchScanAnCrossPatternInWafer(ulScnMinRow, ulScnMinCol, ulScnMaxRow, ulScnMaxCol, lScanRow, lScanCol) )
					{
						lFindMatchCounter = 1;
						lMapScanOffsetRow = ((LONG)ulHomeRow - lScanRow);
						lMapScanOffsetCol = ((LONG)ulHomeCol - lScanCol);
						szMsg.Format("SanAn cross scan align offset %d,%d; map %d,%d scan %d,%d", lMapScanOffsetRow, lMapScanOffsetCol,
							ulHomeRow, ulHomeCol, lScanRow, lScanCol);
						szScnTemp.Format("%s%s", szSpace, szMsg);
						szScnMapLog += szScnTemp;
						SaveScanTimeEvent(szMsg);
						bCheckFail = FALSE;
					}
				}	// San An try broken wafer auto match with cross
			}
			else
			{
				SaveScanTimeEvent("WPR: map dummy scan align in use to check map-wafer match in dimension");
				szMsg = "";
				
				lMapScanOffsetRow = ((LONG)ulMapMinRow - (LONG)ulScnMinRow);
				lMapScanOffsetCol = ((LONG)ulMapMinCol - (LONG)ulScnMinCol);
				szScnTemp.Format("%sWithDimension %ld,%ld", szSpace, lMapScanOffsetRow, lMapScanOffsetCol);
				szScnMapLog += szScnTemp;

				LONG lMapWidth	= ulMapMaxCol - ulMapMinCol;
				LONG lMapHeight	= ulMapMaxRow - ulMapMinRow;
				LONG lScnWidth	= ulScnMaxCol - ulScnMinCol;
				LONG lScnHeight	= ulScnMaxRow - ulScnMinRow;

				szMsg.Format(" Scan wafer WxH(%ld,%ld); Map WxH(%ld,%ld).\n", lScnWidth, lScnHeight, lMapWidth, lMapHeight);
				BOOL bCompareFail = FALSE;
				switch( m_ucScanAlignVerify )	// Scan <==> Map dimension check
				{
				case 5:	// S > M
					if( lScnWidth<=lMapWidth || lScnHeight<=lMapHeight )
					{
						szMsg += " Scan wafer should large than map!";
						bCompareFail = TRUE;
					}
					break;
				case 4:	// S >= M
					if( lScnWidth<lMapWidth || lScnHeight<lMapHeight )
					{
						szMsg += " Scan wafer is small than map!";
						bCompareFail = TRUE;
					}
					break;
				case 3:	// S == M
					if( lScnWidth!=lMapWidth || lScnHeight!=lMapHeight )
					{
						szMsg += " Scan wafer not equal to map!";
						bCompareFail = TRUE;
					}
					break;
				case 2:	//	S <= M
					if( lScnWidth>lMapWidth || lScnHeight>lMapHeight )
					{
						szMsg += " Scan wafer is large than map!";
						bCompareFail = TRUE;
					}
					break;
				case 1:	//	S <  M
					if( lScnWidth>=lMapWidth || lScnHeight>=lMapHeight )
					{
						szMsg += " Scan wafer should small than map!";
						bCompareFail = TRUE;
					}
					break;
				case 0:	// not check map/scan dimension match result
				default:
					break;
				}

				if( bCompareFail )
				{
					SaveScanTimeEvent(szMsg);
					HmiMessage_Red_Back(szMsg, "Prescan");
					m_lPrescanVerifyResult += 2048;
				}
			}
		}
	}

	m_lMapScanOffsetRow = lMapScanOffsetRow;	// map - scan
	m_lMapScanOffsetCol = lMapScanOffsetCol;	// map - scan
	//========================================================================================================
	//   Find Reference die or hole end
	//========================================================================================================


	//	0 = none; 1 = via file; 2 = via map; 3 = auto find pattern in map.
	if( !IsScanAlignWafer() && m_ucScanCheckMapOnWafer>0 )
	{
		SaveScanTimeEvent("WPR: check map holes with wafer");
		szMsg = "";
		BOOL bMatchPass = FALSE;
		if( m_ucScanCheckMapOnWafer==3 && m_bScanAlignAutoFromMap )	//	check map holes match with wafer
		{
			szMsg.Format("WPR: Check Holes In Wafer With Map: Pattern points min %ld max %ld, total points min %ld. holes per %f",
				m_lScanAlignPatternHolesMin, m_lScanAlignPatternHolesMax, m_lScanAlignTotalHolesMin, m_dScanAlignMatchLowPercent);
			SaveScanTimeEvent(szMsg);
			szMsg = "";
			bMatchPass = CheckHolesInWaferWithMap(ulScnMinRow, ulScnMinCol, ulScnMaxRow, ulScnMaxCol, szMsg);
		}
		else if( m_ucScanCheckMapOnWafer<3 && m_bScanAlignFromFile )	//	check map holes match with wafer via file
		{
			SaveScanTimeEvent("WPR: Check Holes In Wafer With file");
			if( SortScanAlignHoleOrder() )
			{
				bMatchPass = CheckHolesInWaferWithFile(ulScnMinRow, ulScnMinCol, ulScnMaxRow, ulScnMaxCol, szMsg);
			}
			else
			{
				szMsg = "WPR: Check Holes In Wafer With file wrong.";
			}
		}

		if( bMatchPass==FALSE )
		{
			SaveScanTimeEvent("WPR: " + szMsg);
			HmiMessage_Red_Back(szMsg, "Check Map Holes on Wafer");
			m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_EMPTY_DIE_CHECK); // empty die check
		//	bCheckFail = TRUE;
		}
	}

	BOOL bWprFpcOK = ScanCheckFivePoints(TRUE);
	if( bWprFpcOK==FALSE )
	{
		bCheckFail = TRUE;
		szMsg = "WPR: five points check failure.";
	}

	if (bCheckFail)	//	for scan align wafer fail and to manual locate alignment point
	{
		SaveScanTimeEvent(szMsg);
		if( IsScanAlignWafer() && IsPrecanWith2Pr() && IsPrescanReferDie() )
			szMsg += "\nPlease stop and check!";
		else
			szMsg += "\nDo you want to continue with Manual Align?";
		SetErrorMessage(szMsg);
		LONG lHmiSel = glHMI_NO;
		if( IsScanAlignWafer() && IsPrecanWith2Pr() && IsPrescanReferDie() )
		{
			lMapScanOffsetRow = 500;
			lMapScanOffsetCol = 500;

			SaveScanTimeEvent("To load scanned map as refer scan align fail.");
			m_WaferMapWrapper.StopMap();
			if( m_bBinMapShow==FALSE )
			{	// check wafer map and scan map to align wafer.
				m_ulPrWindowWidth	= 0;
				m_ulPrWindowHeight	= 0;
				m_bBinMapShow		= TRUE;
				HmiMessage("Map ing", "ES101", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 1000);
			}

			CPreBondEventBin::m_bMapLoaded = FALSE;
			m_BinMapWrapper.InitMap();
			m_BinMapWrapper.LoadMap(szScanPmpFile, "");
			Sleep(500);
			LONG lTimer = 0;
			while( 1 )
			{
				if( CPreBondEventBin::m_bMapLoaded )
				{
					break;
				}
				if( m_BinMapWrapper.IsMapValid() )
				{
					break;
				}
				Sleep(100);
				lTimer++;
				if( lTimer>100 )
				{
					break;
				}
			}

			Sleep(100);
			LONG lAlarmStatus = GetAlarmLamp_Status();
			SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
			AfxMessageBox(szMsg, MB_SYSTEMMODAL);
			SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);

			SaveScanTimeEvent("WPR: load scan map complete");

			HmiMessage("PR ing", "ES101", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 1000);
			m_ulPrWindowHeight	= 480;
			m_ulPrWindowWidth	= 512;
			m_bBinMapShow		= FALSE;
		}
		else
		{		
			// if the prescan good die rate is too low, do not do Manual Align
			if (!PrescanGoodDiePercentageCheck(lScanTotalDieCounter) && !pSRInfo->IsManualAlignRegion())
			{
				lHmiSel = 12; // Prescan Good Die Percentage is too low
			}
			else
			{
				lHmiSel = HmiMessage_Red_Back(szMsg, "Prescan", glHMI_MBX_YESNO);
			}
		}

		if (lHmiSel == glHMI_NO)
		{
			m_lPrescanVerifyResult += 2048;
			SaveScanTimeEvent("User select stop");
		}
		else if (lHmiSel == 12)
		{
			m_lPrescanVerifyResult += 4096;
			SaveScanTimeEvent("Prescan Good Die Percentage is too low");
		}
		else
		{
			SaveScanTimeEvent("User select continue to manual align");
			LONG lPrEncX, lPrEncY;
			IPC_CServiceMessage stMsg;
			int nConvID = 0;

			m_WaferMapWrapper.StopMap();
			SaveScanTimeEvent("To load scanned map");
			if( m_bBinMapShow==FALSE )
			{
				// check wafer map and scan map to align wafer.
				m_ulPrWindowWidth	= 0;
				m_ulPrWindowHeight	= 0;
				m_bBinMapShow		= TRUE;
				HmiMessage("Map ing", "ES101", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 1000);
			}
			CPreBondEventBin::m_bMapLoaded = FALSE;
			m_BinMapWrapper.InitMap();
			m_BinMapWrapper.LoadMap(szScanPmpFile, "");
			if( bWprFpcOK )
			{
				IPC_CServiceMessage stMsg;
				GotoFocusLevel(stMsg);
				MoveBackLightWithTableCheck(TRUE);
			}
			Sleep(500);
			LONG lTimer = 0;
			while( 1 )
			{
				if( CPreBondEventBin::m_bMapLoaded )
				{
					break;
				}
				if( m_BinMapWrapper.IsMapValid() )
				{
					break;
				}
				Sleep(100);
				lTimer++;
				if( lTimer>100 )
				{
					break;
				}
			}

			Sleep(100);
			CString szLogMsg;
			SaveScanTimeEvent("WPR: load scan map complete");
			UCHAR ucBinGrade = 1+m_BinMapWrapper.GetGradeOffset();
			ULONG ulLeft, ulPicked, ulScanMapTotal;
			m_BinMapWrapper.GetStatistics(ucBinGrade, ulLeft, ulPicked, ulScanMapTotal);
			ULONG ulScanMapMaxRows = 0, ulScanMapMaxCols = 0;
			m_BinMapWrapper.GetMapDimension(ulScanMapMaxRows, ulScanMapMaxCols);
			INT nLoop=0;
			for(nLoop=0; nLoop<=3; nLoop++)
			{
				if( m_bBinMapShow==FALSE )
				{
					// check wafer map and scan map to align wafer.
					m_ulPrWindowWidth	= 0;
					m_ulPrWindowHeight	= 0;
					m_bBinMapShow		= TRUE;

					HmiMessage("Map ing", "ES101", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 1000);
				}
				ULONG ulAlnRow = 0, ulAlnCol = 0;
				LONG  lScanRow = 0, lScanCol = 0;
				ULONG ulMapCheckRow = 0, ulMapCheckCol = 0;
				LONG  lScanCheckRow = 0, lScanCheckCol = 0;

				while( 1 )
				{
					ULONG ulSetRow = 0, ulSetCol = 0;
					if( pApp->GetCustomerName()!=CTM_SANAN )
						HmiMessage_Red_Back("Please click the align die on both maps in next message!", "Prescan");
					AfxMessageBox("Please click the align die on map!", MB_SYSTEMMODAL);
					m_WaferMapWrapper.GetSelectedPosition(ulAlnRow, ulAlnCol);
					m_WaferMapWrapper.SetCurrentPosition(ulAlnRow, ulAlnCol);

					m_BinMapWrapper.GetSelectedPosition(ulSetRow, ulSetCol);
					m_BinMapWrapper.SetCurrentPosition(ulSetRow, ulSetCol);
					m_BinMapWrapper.ConvertInternalToOriginalUser(ulSetRow, ulSetCol, FALSE, FALSE, 0, lScanRow, lScanCol);
					if( GetScanPosn(lScanRow, lScanCol, lEncX, lEncY) )
					{
						MoveWaferTable(lEncX, lEncY);
						break;
					}
					else
					{
						szLogMsg = "Selection is not a good die.\nDo you want to select it again?";
						if( HmiMessage_Red_Back(szLogMsg, "Prescan", glHMI_MBX_YESNO)==glHMI_NO )
						{
							break;
						}
					}
				}

				if( pApp->GetCustomerName()==CTM_SANAN )
				{
					ULONG ulSetRow = 0, ulSetCol = 0;
					AfxMessageBox("Please click the check die on map!", MB_SYSTEMMODAL);
					m_WaferMapWrapper.GetSelectedPosition(ulMapCheckRow, ulMapCheckCol);
					m_WaferMapWrapper.SetCurrentPosition(ulMapCheckRow, ulMapCheckCol);

					m_BinMapWrapper.GetSelectedPosition(ulSetRow, ulSetCol);
					m_BinMapWrapper.SetCurrentPosition(ulSetRow, ulSetCol);
					m_BinMapWrapper.ConvertInternalToOriginalUser(ulSetRow, ulSetCol, FALSE, FALSE, 0, lScanCheckRow, lScanCheckCol);

					if( ((LONG)ulAlnRow - lScanRow)==((LONG)ulMapCheckRow - lScanCheckRow) &&
						((LONG)ulAlnCol - lScanCol)==((LONG)ulMapCheckCol - lScanCheckCol) &&
						(ulAlnRow!=ulMapCheckRow || ulAlnCol!=ulMapCheckCol) )
					{
						if( GetScanPosn(lScanCheckRow, lScanCheckCol, lEncX, lEncY) )
						{
							MoveWaferTable(lEncX, lEncY);
						}
					}
					else
					{
						szLogMsg = "Checking is not a good, please retry.";
						HmiMessage_Red_Back(szLogMsg, "Prescan");
						continue;
					}
				}

				m_ulPrWindowHeight	= 480;
				m_ulPrWindowWidth	= 512;
				m_bBinMapShow		= FALSE;

				HmiMessage("PR ing", "ES101", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 1000);
				szLogMsg.Format("Map click align %d,%d; Scan %d,%d", ulAlnRow, ulAlnCol, lScanRow, lScanCol);
				SaveScanTimeEvent(szLogMsg);

				AutoBondScreen(FALSE);

				DrawSearchBox(PR_COLOR_GREEN);

				WPR_ToggleZoom(FALSE, FALSE, 10);
				WprSearchDie(TRUE, 1, lPrEncX, lPrEncY);
				// If video test enabled, not show the die size rectangle
				
				if (m_bDebugVideoTest == FALSE)
				{
					DrawRectangleDieSize(PR_COLOR_GREEN);
				}

				PR_COORD					stCrossHair;
				stCrossHair.x = (PR_WORD)GetPrCenterX();
				stCrossHair.y = (PR_WORD)GetPrCenterY();
				DrawHomeCursor(stCrossHair);

				Sleep(100);
				szLogMsg = "Is the die OK on wafer?";
				LONG glHMI = HmiMessage(szLogMsg, "ES101",
							glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300, 0, 0, 0, 0, 700, 400);
				if( glHMI!=glHMI_YES )
				{
					continue;
				}

				BOOL bGo2ndCheck = TRUE;
				INT nWprDoFPC = pApp->GetProfileInt(gszPROFILE_SETTING, gszWprDoFPC, 0);
				if( IsEnableFPC() && pUtl->GetPrescanDummyMap()==FALSE && nWprDoFPC==1)
					bGo2ndCheck = FALSE;
				if( pApp->GetCustomerName()==CTM_SANAN )
					bGo2ndCheck = FALSE;
				if( bGo2ndCheck )
				{
					while( 1 )
					{
						if( pApp->GetCustomerName()!=CTM_SANAN )
							HmiMessage_Red_Back("Please click the check die on left wafer map in next message!", "Prescan");
						AfxMessageBox("Please click the check die on left wafer map!", MB_SYSTEMMODAL);
						m_WaferMapWrapper.GetSelectedPosition(ulMapCheckRow, ulMapCheckCol);
						m_WaferMapWrapper.SetCurrentPosition(ulMapCheckRow, ulMapCheckCol);
					
						if( ulAlnRow==ulMapCheckRow && ulAlnCol==ulMapCheckCol )
						{
							szLogMsg = "Selection POINT SAME TO ALIGNMENT POINT";
							HmiMessage_Red_Back(szLogMsg, "Prescan");
						}
						else
						{
							break;
						}
					}


					lScanCheckRow = (LONG)ulMapCheckRow + lScanRow - (LONG)ulAlnRow;
					lScanCheckCol = (LONG)ulMapCheckCol + lScanCol - (LONG)ulAlnCol;
					szLogMsg.Format("Map click check %d,%d; Scan %d,%d", ulMapCheckRow, ulMapCheckCol, lScanCheckRow, lScanCheckCol);
					SaveScanTimeEvent(szLogMsg);
	
					if( GetScanPosn(lScanCheckRow, lScanCheckCol, lEncX, lEncY) )
					{
						MoveWaferTable(lEncX, lEncY);
						szLogMsg = "Is the die OK on wafer?";
					}
					else
					{
						szLogMsg = "Check point is not a good die.\nIs your selection OK?";
					}
	
					WPR_ToggleZoom(FALSE, FALSE, 11);
					WprSearchDie(TRUE, 1, lPrEncX, lPrEncY);

					glHMI = HmiMessageEx(szLogMsg, "ES101",
								glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300, 0, 0, 0, 0, 700, 400);
					if( glHMI!=glHMI_YES )
					{
						continue;
					}
				}

				lMapScanOffsetRow = ((LONG)ulAlnRow - lScanRow);
				lMapScanOffsetCol = ((LONG)ulAlnCol - lScanCol);
				szLogMsg.Format("user map to phy scan offset %d,%d", lMapScanOffsetRow, lMapScanOffsetCol);
				SaveScanTimeEvent(szLogMsg);
				szScnTemp.Format("%s%s", szSpace, szLogMsg);
				szScnMapLog += szScnTemp;

				ULONG ulScanAlignTotal = 0;
				for(ULONG ulScanRow = 0; ulScanRow<ulScanMapMaxRows; ulScanRow++)
				{
					for(ULONG ulScanCol = 0; ulScanCol<ulScanMapMaxCols; ulScanCol++)
					{
						if( m_BinMapWrapper.GetGrade(ulScanRow, ulScanCol)!=m_BinMapWrapper.GetNullBin() )
						{
							LONG lScanRow = 0, lScanCol = 0;
							m_BinMapWrapper.ConvertInternalToOriginalUser(ulScanRow, ulScanCol, FALSE, FALSE, 0, lScanRow, lScanCol);
							LONG ulMapRow = 0, ulMapCol = 0;
							ulMapRow = lMapScanOffsetRow + lScanRow;
							ulMapCol = lMapScanOffsetCol + lScanCol;
							if( (ulMapRow)>=0 && (ulMapCol)>=0 )
							{
								if (WM_CWaferMap::Instance()->IsMapHaveBin(ulMapRow, ulMapCol))
								{
									ulScanAlignTotal++;
								}
							}
						}
					}
				}

				szLogMsg.Format("Scan find %d, total %d", ulScanAlignTotal, ulScanMapTotal);
				SaveScanTimeEvent(szLogMsg);

				BOOL bScanAlignOK = TRUE;
				if (m_dScanAlignMPassScore > 0.0)
				{
					if (ulScanAlignTotal < (ulScanMapTotal * m_dScanAlignMPassScore / 100.0))
					{
						bScanAlignOK = FALSE;
					}
				}
				if( bScanAlignOK )
				{
					break;
				}
				else
				{
					DOUBLE dScore = 0.0;
					if( ulScanMapTotal!=0 )
					{
						dScore = ulScanAlignTotal*100/ulScanMapTotal;
					}
					szMsg.Format("Find die %d < scan die %d.\nPass score %.2f < setting %.2f!\nDo you want to align again?",
						ulScanAlignTotal, ulScanMapTotal, dScore, m_dScanAlignMPassScore);
					LONG lHmiSel = HmiMessage_Red_Back(szMsg, "Prescan", glHMI_MBX_YESNO);
					if( lHmiSel==glHMI_NO )
					{
						m_lPrescanVerifyResult += 2048;
						SaveScanTimeEvent("Scan align low pass score, User select stop");
						break;
					}
				}
			}	// nLoop

			if( nLoop>3 )
			{
				m_lPrescanVerifyResult += 2048;
			}

			if( ScanCheckFivePoints(FALSE)==FALSE )
			{
				m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_FPC);	 // five point check
				SetErrorMessage("WPR: prescan five point check fail");
			}
			AutoBondScreen(TRUE);
			MoveBackLightWithTableCheck(FALSE);
			SaveScanTimeEvent("WPR: manual align scanned map complete");
		}	// physical on wafer and logical on map to manual align wafer
	}	// scan align fail, manual alignment done

	SaveScanTimeEvent(szScnMapLog);
	m_lMapScanOffsetRow = lMapScanOffsetRow;	// map - scan
	m_lMapScanOffsetCol = lMapScanOffsetCol;	// map - scan

	m_bBinMapShow		= FALSE;

	//Create output file
	ULONG ulFrameID = 0;
	DOUBLE dDieAngle;
	CString szDieBin = "0";
	szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
	FILE *fpOut = NULL;
	nErr = -1;
	if( pUtl->GetPrescanDebug() )
	{
		nErr = fopen_s(&fpOut, szConLogFile, "w");
	}

	dUsedTime = GetTime();

	if ((nErr == 0) && (fpOut != NULL))
	{
		fprintf(fpOut, "MAP Scan Offset (%ld,%ld), below is phy map result\n", m_lMapScanOffsetRow, m_lMapScanOffsetCol);
	}


	SaveScanTimeEvent("WPR: before set scan info done from builder.");
	{
		LONG lDieSizeTolX = (LONG) ulDieSizeXTol;
		LONG lDieSizeTolY = (LONG) ulDieSizeYTol;
		for(ulRow=ulScnMinRow; ulRow<=ulScnMaxRow; ulRow++)
		{
			for(ulCol=ulScnMinCol; ulCol<=ulScnMaxCol; ulCol++)
			{
				LONG lIndex = 0;
				BOOL bDieOverLap = FALSE;
				bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
				if( bInfo==FALSE )
				{
					continue;
				}

				if( IsWithinScanLimit(lEncX, lEncY)==FALSE )
				{
					continue;
				}

				// BGETREMAIN
				if( lIndex<=WPR_ADD_DIE_INDEX )
					continue;
				if( lIndex>0 )
					pbGetOut[lIndex] = true;
				// BGETREMAIN

				LONG ulInRow = (LONG)ulRow + m_lMapScanOffsetRow;
				LONG ulInCol = (LONG)ulCol + m_lMapScanOffsetCol;

				if( pUtl->GetPrescanRegionMode() )
				{
					if( pSRInfo->IsWithinThisRegion(ulTgtRegion, ulInRow, ulInCol)==FALSE )
					{
						continue;
					}
				}

				if( IsOutMS90SortingPart(ulInRow, ulInCol) )
				{
					continue;
				}

				USHORT usDieState = 0;
				PR_UWORD uwScanDiePrID = 0;
				if( lIndex>0 )
				{
					LONG lDummyX = 0, lDummyY = 0;
					WSGetScanPosition(lIndex, lDummyX, lDummyY, dDieAngle, szDieBin, usDieState, uwScanDiePrID, ulFrameID);
				}
				else
				{
					dDieAngle = 0.0;
					szDieBin = "0";
					uwScanDiePrID = 0;
					if( ulRow==lHomeScnRow && ulCol==lHomeScnCol &&
						lEncX==lHomeWfX && lEncY==lHomeWfY )
					{
						ulFrameID = pUtl->GetAlignPrFrameID();
					}
					else
					{
						ulFrameID = 0;
					}
				}

				if( IsPrecanWith2Pr() && !m_bKeepScan2ndPrDie && IsScanAlignWafer()==FALSE )
				{
					if( uwScanDiePrID==m_uwScanRecordID[1] )
					{
						continue;
					}
				}

				SetScanMapRecordID(ulInRow, ulInCol, (UCHAR) uwScanDiePrID);
				SetScanInfo(ulInRow, ulInCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);	//	from physical map builder
				SetPrescanFrameInfo(ulInRow, ulInCol, ulFrameID);

				if ((nErr == 0) && (fpOut != NULL))
				{
					fprintf(fpOut, "%ld,%ld,%lu,%lu,%ld,%ld\n", ulInRow, ulInCol, ulRow, ulCol, lEncX, lEncY);
				}
			}
		}
	}
	SaveScanTimeEvent("WPR: after set scan info done from builder.");


	// BGETREMAIN
	LONG lDieTolX = ulDieSizeXTol;
	LONG lDieTolY = ulDieSizeYTol;
	WSClearScanRemainDieList();
	if( bGetRemain )
	{
		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if( pbGetOut[ulIndex]==true )
			{
				if ((nErr == 0) && (fpOut != NULL))
				{
					fprintf(fpOut, "indexed is %lu\n", ulIndex);	//v4.49 Klocwork
				}
				continue;
			}

			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
			{
				continue;
			}

			LONG lEncX1 = 0, lEncY1 = 0;
			for(ULONG j=0; j<lRefDieCount; j++)
			{
				lEncX1 = dwaRefWfX.GetAt(j);
				lEncY1 = dwaRefWfY.GetAt(j);
				if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
				{
					pbGetOut[ulIndex] = true;
					if ((nErr == 0) && (fpOut != NULL))	
					{
						fprintf(fpOut, "indexed is %lu done at refer %lu\n", ulIndex, j);
					}
					break;
				}
			}

			if( pbGetOut[ulIndex]==true )
			{
				continue;
			}

			for(ULONG j=1; j<=WSGetPrescanTotalDie(); j++)
			{
				if( WSGetPrescanPosn(j, lEncX1, lEncY1) )
				{
					if( j<ulIndex )
					{
						if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
						{
							pbGetOut[ulIndex] = true;
							if( fpOut!=NULL )
								fprintf(fpOut, "indexed is %lu done at %lu\n", ulIndex, j);
							break;
						}
					}
					else if( j>ulIndex )
					{
						if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
						{
							if( pbGetOut[j]==true )
							{
								pbGetOut[ulIndex] = true;
								if ((nErr == 0) && (fpOut != NULL))
								{
									fprintf(fpOut, "indexed is %lu done at %lu\n", ulIndex, j);
								}
								break;
							}
						}
					}
				}
			}
			if( pbGetOut[ulIndex]==true )
			{
				continue;
			}
			if ((nErr == 0) && (fpOut != NULL))
			{
				fprintf(fpOut, "index %lu to remain list,%ld,%ld\n", ulIndex, lEncX, lEncY);
			}
			pbGetOut[ulIndex] = true;
			WSAddScanRemainDieIntoList(lEncX, lEncY, ulIndex);
		}
	}

	if ((nErr == 0) && (fpOut != NULL))
	{
		fclose(fpOut);
	}

	ULONG 	ulLastScnMinRow = ulScnMinRow;
	ULONG 	ulLastScnMaxRow = ulScnMaxRow;
	ULONG 	ulLastScnMaxCol = ulScnMaxCol;
	ULONG 	ulLastScnMinCol = ulScnMinCol;
	LONG	lLastHomeScnRow = lHomeScnRow;
	LONG	lLastHomeScnCol = lHomeScnCol;
	if( bGetRemain )
	{
		LONG  lRow, lCol, lCurrentIndex = 0;
		LONG lDiff_X, lDiff_Y;
		LONG lUpIndex = 0;
		LONG lDnIndex = 0;
		LONG lLtIndex = 0;
		LONG lRtIndex = 0;

		ULONG ulMaxLoopRow = max(ulMapMaxRow-lHomeOriRow, lHomeOriRow-ulMapMinRow);
		ULONG ulMaxLoopCol = max(ulMapMaxCol-lHomeOriCol, lHomeOriCol-ulMapMinCol);
		ulMaxLoopRow = max(ulMapMaxRow, ulMaxLoopRow);
		ulMaxLoopCol = max(ulMapMaxCol, ulMaxLoopCol);
		ULONG ulLoop = max(ulMaxLoopRow, ulMaxLoopCol);

		CString szMsg;
		szMsg.Format("max loop row %d, max loop col %d, max loop %d at %d,%d", ulMaxLoopRow, ulMaxLoopCol, ulLoop, lHomeOriRow, lHomeOriCol);
		SaveScanTimeEvent(szMsg);
		LONG lTgtRow = lHomeOriRow;
		LONG lTgtCol = lHomeOriCol;
		lLastHomeScnRow = lHomeOriRow;
		lLastHomeScnCol = lHomeOriCol;

		ulLastScnMaxRow = 0;
		ulLastScnMaxCol = 0;
		ulLastScnMinRow = 9999;
		ulLastScnMinCol = 9999;
		if( RefillScanInfoViaRemainDice(lTgtRow, lTgtCol, ulLoop) )
		{
			GetMinMaxRowCol((ULONG)lTgtRow, (ULONG)lTgtCol,
							ulLastScnMaxRow, ulLastScnMinRow,
							ulLastScnMaxCol, ulLastScnMinCol);
		}

		for(ULONG ulCurrentLoop = 1; ulCurrentLoop <= ulLoop; ulCurrentLoop++)
		{
			lRow = (ulCurrentLoop * 2 + 1);
			lCol = (ulCurrentLoop * 2 + 1);

			//Move to RIGHT & PR search on current die
			lCurrentIndex = 1;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTgtRow += lDiff_X;
			lTgtCol += lDiff_Y;

			if( RefillScanInfoViaRemainDice(lTgtRow, lTgtCol, ulLoop) )
			{
				GetMinMaxRowCol((ULONG)lTgtRow, (ULONG)lTgtCol,
								ulLastScnMaxRow, ulLastScnMinRow,
								ulLastScnMaxCol, ulLastScnMinCol);
			}

			//Move to UP & PR search on current die
			lUpIndex = (lRow - 1) - lCurrentIndex;
			while (1)
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
				lTgtRow += lDiff_X;
				lTgtCol += lDiff_Y;

				if( RefillScanInfoViaRemainDice(lTgtRow, lTgtCol, ulLoop) )
				{
					GetMinMaxRowCol((ULONG)lTgtRow, (ULONG)lTgtCol,
									ulLastScnMaxRow, ulLastScnMinRow,
									ulLastScnMaxCol, ulLastScnMinCol);
				}
			}

			//Move to LEFT & PR search on current die
			lLtIndex = (lCol - 1) - lCurrentIndex;
			while (1)
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
				lTgtRow += lDiff_X;
				lTgtCol += lDiff_Y;

				if (RefillScanInfoViaRemainDice(lTgtRow, lTgtCol, ulLoop))
				{
					GetMinMaxRowCol((ULONG)lTgtRow, (ULONG)lTgtCol,
									ulLastScnMaxRow, ulLastScnMinRow,
									ulLastScnMaxCol, ulLastScnMinCol);
				}
			}

			//Move to DOWN & PR search on current die
			lDnIndex = (lRow - 1) - lCurrentIndex;
			while (1)
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
				lTgtRow += lDiff_X;
				lTgtCol += lDiff_Y;

				if( RefillScanInfoViaRemainDice(lTgtRow, lTgtCol, ulLoop) )
				{
					GetMinMaxRowCol((ULONG)lTgtRow, (ULONG)lTgtCol,
									ulLastScnMaxRow, ulLastScnMinRow,
									ulLastScnMaxCol, ulLastScnMinCol);
				}
			}

			//Move to DOWN & PR search on current die
			lRtIndex = (lCol - 1) - lCurrentIndex;
			while (1)
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
				lTgtRow += lDiff_X;
				lTgtCol += lDiff_Y;

				if( RefillScanInfoViaRemainDice(lTgtRow, lTgtCol, ulLoop) )
				{
					GetMinMaxRowCol((ULONG)lTgtRow, (ULONG)lTgtCol,
									ulLastScnMaxRow, ulLastScnMinRow,
									ulLastScnMaxCol, ulLastScnMinCol);
				}
			}
		}

		szMsg.Format("WPR: to get out PHY range Row(%lu,%lu)  Col(%lu,%lu); home scn %ld,%ld", 
			ulLastScnMinRow, ulLastScnMaxRow, ulLastScnMinCol, ulLastScnMaxCol, lLastHomeScnRow, lLastHomeScnCol);
		SaveScanTimeEvent(szMsg);
	}
	delete [] pbGetOut;
	// BGETREMAIN

	SaveScanTimeEvent("WPR: to remove all data of physical map builder.");

	m_cPIMap.RemoveAll();
	m_cPIMap.SetScan2Pr(FALSE);

	// transfer data from prescan result to map physical position array
	LONG lUserRow = 0, lUserCol = 0;
	ULONG ulNewRow = 0, ulNewCol = 0, ulWSIndex = 0;
	LONG  lNewWftX = 0, lNewWftY = 0;
	BOOL  bNewFind = FALSE;

	if( pUtl->GetPrescanAoiWafer() && ucPrescanEdgeSize>0 )
	{
		for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)
		{
			for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
			{
				if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow, ulCol))
				{
					continue;
				}

				if( GetPrescanPosition(ulRow, ulCol, lEncX, lEncY)==FALSE )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNullBin);
				}
			}
		}
	}	// end prescan map edge
	AddPrescanEdge();	//	for non Daily Check mode

	CStringArray	szaPrSummaryList;
	LONG lAlnUserRow = 0, lAlnUserCol = 0;
	ConvertAsmToOrgUser(lHomeOriRow, lHomeOriCol, lAlnUserRow, lAlnUserCol);
	INT nRowDir = 1;
	INT nColDir = 1;
	LONG m_lMa3AHomeRow = 128;
	LONG m_lMa3AHomeCol = 128;

	UINT aunSelCount[256];
	memset(aunSelCount, 0, sizeof(aunSelCount));
	ULONG ulPickEmptyCount = 0, ulPickDefectCount = 0, ulPickBadCutCount = 0, ulPickGoodCount = 0;
	ULONG ulEmptyIdx = 0, ulDefectIdx = 0, ulBadCutIdx = 0, ulExtraIdx = 0, ulGoodIdx = 0;
	ULONG ulMapTotal = 0, ulMapNgTotal = 0, ulMapNgEmpty = 0;
	ULONG ulEdgeEmptyIdex = 0;

	SaveScanTimeEvent("WPR: get out complete and to update wafer map");
	LONG lTolX = (ulDiePitchX*50/100);
	LONG lTolY = (ulDiePitchY*50/100);

	DelPrescanRunPosn();
	SaveScanTimeEvent("WPR: loop to fill in map position.");

	for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			BOOL bIsIgnore = FALSE;
			BOOL bToUpdatePosn = TRUE;
			BOOL bToChangeGrade = TRUE;
			UCHAR ucB4ScanGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);

			//	m_WaferMapWrapper.GetReader()->SetInkedDie(, )
			USHORT usOrgGrade = m_WaferMapWrapper.GetOriginalGrade(ucB4ScanGrade)- m_WaferMapWrapper.GetGradeOffset();
			BOOL bInkedDie = m_WaferMapWrapper.GetReader()->IsInkedDie(ulRow, ulCol) || 
							 usOrgGrade==(65533);	//	die grade -3

			BOOL bIsExtra = FALSE;
			if( ucB4ScanGrade==ucNullBin )
			{
				if( bExtraAction )
				{
					bIsExtra = TRUE;
				}
				else
				{
					bIsIgnore = TRUE;
					bToUpdatePosn = FALSE;
				}
			}

			if( bIsExtra!=TRUE && m_bPrescanEmptyToNullBin==FALSE && m_bPrescanEmptyAction && ucB4ScanGrade==ucEmptyGrade )
			{
				bIsIgnore = TRUE;
			}

			if( ucPrescanEdgeSize>0 && m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_SCAN_EDGE)
			{
				bIsIgnore = TRUE;
			}

			if( IsScanMapNgGrade(ucB4ScanGrade-ucOffset) )
			{
				bToChangeGrade = FALSE;
				if (IsMS90HalfSortMode())
				{
					bToChangeGrade = TRUE;
				}
			}

			USHORT usDieState = 0;
			BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
			bInfo = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
			if( IsPrecanWith2Pr() && !m_bKeepScan2ndPrDie && IsScanAlignWafer() )
			{
				if( GetScanMapRecordID(ulRow, ulCol)==m_uwScanRecordID[1] )
				{
					bInfo = FALSE;
				}
			}
			DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);

			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				bIsIgnore = TRUE;
				bToUpdatePosn = FALSE;
			}

			if( pUtl->GetPrescanRegionMode() )
			{
				if( pSRInfo->IsWithinThisRegion(ulTgtRegion, ulRow, ulCol)==FALSE )
				{
					bIsIgnore = TRUE;
					bToUpdatePosn = FALSE;
				}

				if( bInfo && pSRInfo->IsWithinThisRegion(ulAlnRegion, ulRow, ulCol) )
				{
					ULONG k;
					LONG lKRow, lKCol, lKX, lKY;
					for(k=0; k<pUtl->GetNumOfReferPoints(); k++)
					{
						pUtl->GetReRefPosition(k, lKRow, lKCol, lKX, lKY);
						if( lKRow==ulRow && lKCol==ulCol )
						{
							pUtl->AddReferMapPosition(ulRow, ulCol, lEncX, lEncY);
							break;
						}
					}
				}
			}

			if( bInfo )
			{
				if( bToUpdatePosn )
				{
					m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
					SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
				}
				else
				{
					if( pUtl->GetPrescanRegionMode()==FALSE && IsInMapValidRange(ulRow, ulCol) )
					{
						SetExtraInfo(ulExtraIdx, ulRow, ulCol);
						ulExtraIdx++;
					}
				}
			}

			if( bInfo==FALSE && bIsExtra )
			{
				bIsIgnore = TRUE;
			}
			if( bIsIgnore )
			{
				continue;
			}

			BOOL bIsRefer = FALSE;
			if( ucReferGrade==ucB4ScanGrade )
				bIsRefer = m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol);
			BOOL bIsSpRef = IsSpecialReferGrade(ulRow, ulCol);
			BOOL bIsNoDie = IsNoDieGrade(ulRow, ulCol);
			if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_DIMREFER )
				bIsRefer = TRUE;

			if( bIsRefer )
			{
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if( pApp->GetCustomerName()=="Huga" )	// MARK REFER DIE
				{
					COLORREF crUserColor = RGB(255,5,5); //(in Red color)
					m_WaferMapWrapper.AddDescription(ulRow, ulCol, 96, crUserColor, "R");
				}
			}

			if( bIsRefer || bIsSpRef || bIsNoDie )
			{
				continue;
			}

			BOOL bScoreCount = TRUE;
			if( pUtl->GetPrescanDummyMap() && bInfo==FALSE )
			{
				bScoreCount = FALSE;
			}

			if( IsScanMapNgGrade(ucB4ScanGrade-ucOffset) )
			{
				bScoreCount = FALSE;
			}

			if( m_WaferMapWrapper.GetReader()!=NULL && m_WaferMapWrapper.GetReader()->IsInvalidDie(ulRow, ulCol) )
			{
				bScoreCount = FALSE;
			}

			if( bIsExtra )
			{
				bScoreCount = FALSE;
			}

			if( bScoreCount )
			{
				ulMapTotal++;
			}

			// BGETREMAIN
			if( bGetRemain && bInfo==FALSE )
			{
				BOOL bGetPred = GetDieValidPrescanPosn(ulRow, ulCol, nMaxSpan, lEncX, lEncY, FALSE);

				if( bGetPred )
				{
					PR_UWORD uwDiePrID = 0;
					if( FindAndFillFromRemainList(ulRow, ulCol, lDieTolX, lDieTolY, lEncX, lEncY, uwDiePrID) )
					{
						bInfo = TRUE;
						m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
						SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
					}
				}
			}
			// BGETREMAIN

			BOOL bIsInGood = FALSE;
			BOOL bFindGood = FALSE;
			for(int xxx=0; xxx<aulSelGradeList.GetSize(); xxx++)
			{
				if( aulSelGradeList.GetAt(xxx)==ucB4ScanGrade )
				{
					bFindGood = TRUE;
					for(int uuu=0; uuu<aulMinGradeList.GetSize(); uuu++)
					{
						if( aulMinGradeList.GetAt(uuu)==ucB4ScanGrade )
						{
							bFindGood = FALSE;
							break;
						}
					}
				}
				if( bFindGood )
				{
					break;
				}
			}
			if( bFindGood )
			{
				bIsInGood = TRUE;
			}

			CString szDieOcrValue = "";
			if( bInfo )
			{
				ulNewRow = ulRow;	ulNewCol = ulCol;
				lNewWftX = lEncX;	lNewWftY = lEncY;
				bNewFind = TRUE;

				BOOL bNeedChangeGrade = FALSE;
				UCHAR ucChangeGrade = ucB4ScanGrade;
				BOOL bNgDieGrade = FALSE;

				if( bIsExtra )
				{
					SetExtraInfo(ulExtraIdx, ulRow, ulCol);
					ulExtraIdx++;

					if( IsNoPickExtra() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_UNPICK_SCAN_EXTRA);
					}

					if( bToChangeGrade )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucExtraGrade;
					}
				}
				else if (bIsBadCut)
				{
					if( bScoreCount )
					{
						SetBadCutFull(ulBadCutIdx, ulRow, ulCol, ucB4ScanGrade);
						ulBadCutIdx++;
					}

					if( IsNoPickBadCut() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT);
					}

					if( bIsInGood )
					{
						ulPickBadCutCount++;
					}

					if( bBadCutAction )
					{
						if( bToChangeGrade )
						{
							bNeedChangeGrade = TRUE;
							ucChangeGrade = ucBadCutGrade;
							bNgDieGrade = TRUE;
						}
					}
					else
					{
						if( m_bPrescanBadCutMarkUnPick && bInkedDie==FALSE )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
					if( bShortPathNgDieToInvalid && bInkedDie==FALSE )
					{
						m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
						if (m_bPrescanBadCutToNullBin)
							m_WaferMapWrapper.HideDie(ulRow, ulCol);
					}
				}
				else if( bIsFakeEmpty )
				{
					if( bScoreCount )
					{
						SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
						ulDefectIdx++;
					}

					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_BACKUP_ALIGN);

					if( bIsInGood )
					{
						ulPickDefectCount++;
					}

					if( bFakeAction )
					{
						if( bToChangeGrade )
						{
							bNeedChangeGrade = TRUE;
							ucChangeGrade = ucFakeGrade;
							bNgDieGrade = TRUE;
						}
					}
				}
				else if( bIsDefect )
				{
					if( bScoreCount )
					{
						SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
						ulDefectIdx++;
					}

					if( IsNoPickDefect() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT);
					}

					if( bIsInGood )
					{
						ulPickDefectCount++;
					}

					if( bDefectAction )
					{
						if( bToChangeGrade )
						{
							bNeedChangeGrade = TRUE;
							ucChangeGrade = ucDefectGrade;
							bNgDieGrade = TRUE;
						}
					}
					else
					{
						if( m_bPrescanDefectMarkUnPick && bInkedDie==FALSE )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
					if( bShortPathNgDieToInvalid && bInkedDie==FALSE )
					{
						m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
						if( m_bPrescanDefectToNullBin )
							m_WaferMapWrapper.HideDie(ulRow, ulCol);
					}
				}
				else
				{
					for(int xxx=0; xxx<aulSelGradeList.GetSize(); xxx++)
					{
						if( aulSelGradeList.GetAt(xxx)==ucB4ScanGrade )
						{
							aunSelCount[ucB4ScanGrade]++;
							break;
						}
					}

					if( bScoreCount )
					{
						ulGoodIdx++;
					}
					if( bIsInGood )
					{
						ulPickGoodCount++;
					}
					if( bGoodAction && bToChangeGrade )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucGoodGrade;
					}
				}

				UCHAR ucDieBin = 0;
				if (m_bSaveNgDieSubGrade && bNeedChangeGrade && (GetPsmEnable() == FALSE))
				{
					ucChangeGrade += ucDieBin;
				}

				if (IsUseGradeConvert() && m_lGradeMappingLimit > 0)
				{
					CString szTemp, szAppend = "";
					if( m_bUseAoiGradeConvert )
					{
						szTemp.Format("map (%d,%d) grade %d DieBin %s Bin %d ==> ", ulRow, ulCol, (ucB4ScanGrade - ucOffset), szDieBin, ucDieBin);
						for(int i=0; i<m_lGradeMappingLimit; i++)
						{
							if( m_sMapOrgGrade[i]==(ucB4ScanGrade - ucOffset) && m_sPrInspectGrade[i]==ucDieBin )
							{
								ucChangeGrade = (UCHAR)m_sMapNewGrade[i];
								bNeedChangeGrade = TRUE;
								szAppend.Format("map grade aoi convert to %d", ucChangeGrade);
								ucChangeGrade = ucChangeGrade + ucOffset;
								break;
							}
						}
					}

					if (m_bScanNgGradeConvert)
					{
						UCHAR ucChkGrade = ucB4ScanGrade - ucOffset;
						szTemp.Format("map (%d,%d) grade %d ==> ", ulRow, ulCol, ucChkGrade);
						if( m_sMapOrgGrade[ucChkGrade]==ucChkGrade )
						{
							if( bNeedChangeGrade )
							{
								ucChangeGrade = (UCHAR)m_sMapNewGrade[ucChkGrade];
							}
							else
							{
								ucChangeGrade = (UCHAR)m_sPrInspectGrade[ucChkGrade];
							}
							szAppend.Format("map grade convert to %d", ucChangeGrade);
							ucChangeGrade = ucChangeGrade + ucOffset;
						}
					}
					szTemp += szAppend;
				//	SetAlarmLog(szTemp);
				}

				if (bNeedChangeGrade)
				{
					if( bInkedDie==FALSE )
						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucChangeGrade);
					if( bNgDieGrade )
					{
						UCHAR ucSaveGrade = (UCHAR)(ucChangeGrade - ucOffset);
						ucNgGradeList[ucSaveGrade] = ucSaveGrade;
						bNgGradeAdded = TRUE;
					}
				}
				if (IsAutoRescanEnable())
				{
					UCHAR ucPickGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
					if( ucPickGrade!=ucNullBin )
					{
						for (int j=0; j<aulSelGradeList.GetSize(); j++)
						{
							if (ucPickGrade==aulSelGradeList.GetAt(j))
							{
								m_lPrescanSortingTotal++;
								break;
							}
						}
					}
				}	//	SanAn TJ 2017
			}
			else // set to empty grade
			{
				lEncX = lEncY = 0;
				if( bScoreCount )
				{
					if( (m_ucPrescanEdgeSizeX!=0 || m_ucPrescanEdgeSizeY!=0) && m_dEdgeGoodScore>0.0 )
					{
						if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DS_SCAN_EDGE_CHECK )
						{
							ulEdgeEmptyIdex++;
						}
					}
					SetEmptyFull(ulEmptyIdx, ulRow, ulCol, ucB4ScanGrade);
					ulEmptyIdx++;
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
					if( bIsInGood )
					{
						ulPickEmptyCount++;
					}
				}

				if( bEmptyAction )
				{
					if( bToChangeGrade && bInkedDie==FALSE )
					{
						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
					}
				}
				else
				{
					if( m_bPrescanEmptyMarkUnPick && bInkedDie==FALSE )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						UpdateDie(ulRow, ulCol, ucB4ScanGrade, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
					}
				}
				if( bShortPathNgDieToInvalid && bInkedDie==FALSE )
				{
					m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
					if( m_bPrescanEmptyToNullBin )
						m_WaferMapWrapper.HideDie(ulRow, ulCol);
				}	//	ng to invalid
			}
		}
	}

	m_WaferMapWrapper.BatchPhysicalPositionUpdate();
	if( bNgGradeAdded )
	{
		WM_CWaferMap::Instance()->CreateConstructMapNGGradeFile(szLogPath, ucNgGradeList);
	}

	if( ulMinCount>0 && pUtl->GetPrescanRegionMode() && bShortPathNgDieToInvalid )
	{
		szMsg.Format("WPR: region scan sorting path 1 kill grades count < %d", ulMinCount);
		SaveScanTimeEvent(szMsg);
		for(int xxx=0; xxx<aulSelGradeList.GetSize(); xxx++)
		{
			UCHAR ucCurGrade = aulSelGradeList.GetAt(xxx);
			if( aunSelCount[ucCurGrade]==0 )
			{
				continue;
			}
			if( aunSelCount[ucCurGrade] >= ulMinCount )
			{
				aunSelCount[ucCurGrade] = 0;
			}
			if( aunSelCount[ucCurGrade]>0 )
			{
				szMsg.Format("WPR: grade %d ==> die counter %d", ucCurGrade - ucOffset, aunSelCount[ucCurGrade]);
				SaveScanTimeEvent(szMsg);
			}
		}

		for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)
		{
			for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
			{
				UCHAR ucCurGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);

				if( ucCurGrade==ucNullBin )
				{
					continue;
				}

				if( pSRInfo->IsWithinThisRegion(ulTgtRegion, ulRow, ulCol)==FALSE )
				{
					continue;
				}

				if( IsOutMS90SortingPart(ulRow, ulCol) )
				{
					continue;
				}

				if( aunSelCount[ucCurGrade] == 0 )
				{
					continue;
				}

				m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
	//			m_WaferMapWrapper.HideDie(ulRow, ulCol);
			}
		}
	}	// set less quantity grade die to missing to avoid pick again.

	LONG	lMinDieDistX = m_lMinDieDistX;
	LONG	lMinDieDistY = m_lMinDieDistY;

	// add protection check to make sure the dist of X and Y is larger than die size in X and Y
	if( lMinDieDistX!=0 || lMinDieDistY!=0 )
	{
		SavePrData(FALSE);
		szMsg.Format("WPR: MinDieDist X:%d  Y:%d", lMinDieDistX, lMinDieDistY);
		SaveScanTimeEvent(szMsg);
		CString szMapFilename = GetMapNameOnly();
		SaveBadCutEvent(szMsg, TRUE);
		for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)
		{
			for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
			{
				if( pUtl->GetPrescanRegionMode() )
				{
					if( pSRInfo->IsWithinThisRegion(ulTgtRegion, ulRow, ulCol)==FALSE )
					{
						continue;
					}
				}

				if( IsOutMS90SortingPart(ulRow, ulCol) )
				{
					continue;
				}

				LONG lCurrX = 0, lCurrY = 0, lRightX = 0, lRightY = 0, lDownX = 0, lDownY = 0;
				LONG lUserRow = 0,lUserCol = 0;
				if( GetMapPhyPosn(ulRow, ulCol, lCurrX, lCurrY)==FALSE )
				{
					continue;
				}     //    current die has no position.

				//    check at right and down
				if( GetMapPhyPosn(ulRow, ulCol+1, lRightX, lRightY) && lMinDieDistX>0 )
				{  //    normal pick the good die grade
					if( labs(lRightX-lCurrX)<labs(lMinDieDistX) )
					{
						// current die
						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNullBin);
						if( bShortPathNgDieToInvalid )
						{
							m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
						}
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						if( m_WaferMapWrapper.GetDieState(ulRow, ulCol+1)==WT_MAP_DS_SCAN_TWIN_DIE_JOINT )
						{
							m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_TWIN_DIE_JOINT);
							ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
							szMsg.Format(",CaseX12,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,RightX,%ld,CurrX,%ld",
								szMapFilename, ulRow, ulCol, lUserRow, lUserCol, lRightX, lCurrX);
							SaveBadCutEvent(szMsg);
						}
						if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)!=WT_MAP_DS_SCAN_TWIN_DIE_JOINT && 
							m_WaferMapWrapper.GetDieState(ulRow, ulCol)!=WT_MAP_DS_SCAN_TWIN_DIE_FIRST )
						{
							m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_TWIN_DIE_FIRST);
							ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
							szMsg.Format(",CaseX10,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,RightX,%ld,CurrX,%ld",
								szMapFilename, ulRow, ulCol, lUserRow, lUserCol, lRightX, lCurrX);
							SaveBadCutEvent(szMsg);
						}
						// right hand die
						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol+1, ucNullBin);
						if( bShortPathNgDieToInvalid )
						{
							m_WaferMapWrapper.SetMissingDie(ulRow, ulCol+1);
						}
						m_WaferMapWrapper.MarkDie(ulRow, ulCol+1, TRUE);
						m_WaferMapWrapper.SetDieState(ulRow, ulCol+1, WT_MAP_DS_SCAN_TWIN_DIE_JOINT);
						ConvertAsmToOrgUser(ulRow, ulCol+1, lUserRow, lUserCol);
						szMsg.Format(",CaseX11,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,RightX,%ld,CurrX,%ld",
							szMapFilename, ulRow, ulCol+1, lUserRow, lUserCol, lRightX, lCurrX);
						SaveBadCutEvent(szMsg);
					}
				}	//	X direction check

				if( GetMapPhyPosn(ulRow+1, ulCol, lDownX, lDownY) && lMinDieDistY>0 )
				{  //    normal pick the good die grade
					if( labs(lDownY-lCurrY)<labs(lMinDieDistY) )
					{
						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNullBin);
						if( bShortPathNgDieToInvalid )
						{
							m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
						}
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)!=WT_MAP_DS_SCAN_TWIN_DIE_JOINT && 
							m_WaferMapWrapper.GetDieState(ulRow, ulCol)!=WT_MAP_DS_SCAN_TWIN_DIE_FIRST )
						{
							m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_TWIN_DIE_FIRST);
							ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
							szMsg.Format(",CaseY10,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,DownY,%ld,CurrY,%ld",
								szMapFilename, ulRow, ulCol, lUserRow, lUserCol, lDownY, lCurrY);
							SaveBadCutEvent(szMsg);
						}
						m_WaferMapWrapper.ChangeGrade(ulRow+1, ulCol, ucNullBin);
						if( bShortPathNgDieToInvalid )
						{
							m_WaferMapWrapper.SetMissingDie(ulRow+1, ulCol);
						}
						m_WaferMapWrapper.MarkDie(ulRow+1, ulCol, TRUE);
						m_WaferMapWrapper.SetDieState(ulRow+1, ulCol, WT_MAP_DS_SCAN_TWIN_DIE_JOINT);
						ConvertAsmToOrgUser(ulRow+1, ulCol, lUserRow, lUserCol);
						szMsg.Format(",CaseY11,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,DownY,%ld,CurrY,%ld",
							szMapFilename,ulRow+1, ulCol,lUserRow, lUserCol, lDownY, lCurrY);
						SaveBadCutEvent(szMsg);
					}
				}	//	Y direction check

				//        case for this and change to null because above kept.
				if( GetMapPhyPosn(ulRow, ulCol+1, lRightX, lRightY) && lMinDieDistX<0 )
				{	// reverse pick the NG die grade.
					if( labs(lRightX-lCurrX)<labs(lMinDieDistX) )
					{
						if( m_WaferMapWrapper.GetDieState(ulRow, ulCol+1)==WT_MAP_DS_SCAN_TWIN_DIE_JOINT ||
							m_WaferMapWrapper.GetDieState(ulRow, ulCol+1)==WT_MAP_DS_SCAN_TWIN_DIE_FIRST )
						{
							m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNullBin);
							if( bShortPathNgDieToInvalid )
							{
								m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
							}
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
							m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_TWIN_DIE_JOINT);
							ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
							szMsg.Format(",CaseX22,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,RightX,%ld,CurrX,%ld",
								szMapFilename, ulRow, ulCol, lUserRow, lUserCol, lRightX, lCurrX);
							SaveBadCutEvent(szMsg);
						}

						if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)!=WT_MAP_DS_SCAN_TWIN_DIE_JOINT && 
							m_WaferMapWrapper.GetDieState(ulRow, ulCol)!=WT_MAP_DS_SCAN_TWIN_DIE_FIRST )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
							m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_TWIN_DIE_FIRST);
							ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
							szMsg.Format(",CaseX20,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,RightX,%ld,CurrX,%ld",
								szMapFilename,ulRow, ulCol,lUserRow, lUserCol, lRightX, lCurrX);
							SaveBadCutEvent(szMsg);

							m_WaferMapWrapper.MarkDie(ulRow, ulCol+1, TRUE);
							m_WaferMapWrapper.SetDieState(ulRow, ulCol+1, WT_MAP_DS_SCAN_TWIN_DIE_JOINT);
							ConvertAsmToOrgUser(ulRow, ulCol+1, lUserRow, lUserCol);
							szMsg.Format(",CaseX21,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,RightX,%ld,CurrX,%ld",
								szMapFilename,ulRow, ulCol+1,lUserRow, lUserCol, lRightX, lCurrX);
							SaveBadCutEvent(szMsg);
							if( IsInSelectedGrades(ulRow, ulCol) )
							{
								m_WaferMapWrapper.ChangeGrade(ulRow, ulCol+1, ucNullBin);
								if( bShortPathNgDieToInvalid )
								{
									m_WaferMapWrapper.SetMissingDie(ulRow, ulCol+1);
								}
							}
							else
							{
								m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNullBin);
								if( bShortPathNgDieToInvalid )
								{
									m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
								}
								if( !IsInSelectedGrades(ulRow, ulCol+1) )
								{
									m_WaferMapWrapper.ChangeGrade(ulRow, ulCol+1, ucNullBin);
									if( bShortPathNgDieToInvalid )
									{
										m_WaferMapWrapper.SetMissingDie(ulRow, ulCol+1);
									}
								}
							}
						}
					}
				}	//	X direction check

				if( GetMapPhyPosn(ulRow+1, ulCol, lDownX, lDownY) && lMinDieDistY<0 )
				{	// reverse pick the NG die grade.
					if( labs(lDownY-lCurrY)<labs(lMinDieDistY) )
					{
						if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)!=WT_MAP_DS_SCAN_TWIN_DIE_JOINT && 
							m_WaferMapWrapper.GetDieState(ulRow, ulCol)!=WT_MAP_DS_SCAN_TWIN_DIE_FIRST )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
							m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_TWIN_DIE_FIRST);
							ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
							szMsg.Format(",CaseY20,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,DownY,%ld,CurrY,%ld",
								szMapFilename, ulRow, ulCol, lUserRow, lUserCol, lDownY, lCurrY);
							SaveBadCutEvent(szMsg);
						}

						m_WaferMapWrapper.MarkDie(ulRow+1, ulCol, TRUE);
						m_WaferMapWrapper.SetDieState(ulRow+1, ulCol, WT_MAP_DS_SCAN_TWIN_DIE_JOINT);
						ConvertAsmToOrgUser(ulRow+1, ulCol, lUserRow, lUserCol);
						szMsg.Format(",CaseY21,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,DownY,%ld,CurrY,%ld",
							szMapFilename,ulRow+1, ulCol, lUserRow, lUserCol,lDownY, lCurrY);
						SaveBadCutEvent(szMsg);
						if( IsInSelectedGrades(ulRow, ulCol) )
						{
							m_WaferMapWrapper.ChangeGrade(ulRow+1, ulCol, ucNullBin);
							if( bShortPathNgDieToInvalid )
							{
								m_WaferMapWrapper.SetMissingDie(ulRow+1, ulCol);
							}
						}
						else
						{
							m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNullBin);
							if( bShortPathNgDieToInvalid )
							{
								m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
							}
							if( !IsInSelectedGrades(ulRow+1, ulCol) )
							{
								m_WaferMapWrapper.ChangeGrade(ulRow+1, ulCol, ucNullBin);
								if( bShortPathNgDieToInvalid )
								{
									m_WaferMapWrapper.SetMissingDie(ulRow+1, ulCol);
								}
							}
						}
					}
				}	//	Y direction check
			}
		}
	} // die dist check between neighbour die.


	szMsg.Format("WPR: get out complete and to update wafer map done, ScanAlign is %d, Scn Map check %d",
		IsScanAlignWafer(), m_ucScanAlignVerify);
	SaveScanTimeEvent(szMsg);
	m_lTimeSlot[8] = (LONG)(GetTime()-dUsedTime);	//	08.	update wafer map grade and position

	if( IsScanAlignWafer()==FALSE && m_ucScanAlignVerify>0 )
	{
		SaveScanTimeEvent("WPR: to check map-wafer match in dimension");
		LONG lMapWidth	= ulMapMaxCol - ulMapMinCol;
		LONG lMapHeight	= ulMapMaxRow - ulMapMinRow;
		LONG lScnWidth	= ulLastScnMaxCol - ulLastScnMinCol;
		LONG lScnHeight	= ulLastScnMaxRow - ulLastScnMinRow;

		CString szMsg;
		szMsg.Format("PHY range row(%lu,%lu), col(%lu,%lu); MAP range row(%lu,%lu), col(%lu,%lu)",
			ulLastScnMinRow, ulLastScnMaxRow, ulLastScnMinCol, ulLastScnMaxCol,
			ulMapMinRow, ulMapMaxRow, ulMapMinCol, ulMapMaxCol);
		SaveScanTimeEvent(szMsg);
		szMsg.Format("PHY width %ld, height %ld, home %ld,%ld; Map width %ld, height %ld, home %ld,%ld",
			lScnWidth, lScnHeight, lLastHomeScnRow, lLastHomeScnCol, lMapWidth, lMapHeight, lHomeOriRow, lHomeOriCol);
		SaveScanTimeEvent(szMsg);

		LONG lMapLft	= lHomeOriCol - ulMapMinCol;
		LONG lMapRgt	= ulMapMaxCol - lHomeOriCol;
		LONG lMapTop	= lHomeOriRow - ulMapMinRow;
		LONG lMapBtm	= ulMapMaxRow - lHomeOriRow;
		LONG lScnLft	= lLastHomeScnCol - ulLastScnMinCol;
		LONG lScnRgt	= ulLastScnMaxCol - lLastHomeScnCol;
		LONG lScnTop	= lLastHomeScnRow - ulLastScnMinRow;
		LONG lScnBtm	= ulLastScnMaxRow - lLastHomeScnRow;

		BOOL bCompareFail = FALSE;
		switch( m_ucScanAlignVerify )	// Scan <==> Map dimension check
		{
		case 6:	// S<>M		"Scan wafer should be large than Map symmetrically!";
			if( lScnWidth<lMapWidth || lScnHeight<lMapHeight )
			{
				szMsg = " Scan wafer is small than map!";
				bCompareFail = TRUE;
			}
			else
			{
				if( (lScnLft-lMapLft) != (lScnRgt-lMapRgt) || 
					(lScnTop-lMapTop) != (lScnBtm-lMapBtm)  )
				{
					szMsg = " Scan wafer shifted to map!";
					bCompareFail = TRUE;
				}
			}
			break;
		case 5:	// S > M	"Scan wafer should be large than Map!";
			if( lScnWidth<=lMapWidth || lScnHeight<=lMapHeight )
			{
				szMsg = " Scan wafer should large than map!";
				bCompareFail = TRUE;
			}
			break;
		case 4:	// S >= M	"Scan wafer should not be small than Map!";
			if( lScnWidth<lMapWidth || lScnHeight<lMapHeight )
			{
				szMsg = " Scan wafer is small than map!";
				bCompareFail = TRUE;
			}
			break;
		case 3:	// S == M	"Scan wafer should be equal to Map!";
			if( lScnWidth!=lMapWidth || lScnHeight!=lMapHeight )
			{
				szMsg = " Scan wafer not equal to map!";
				bCompareFail = TRUE;
			}
			else
			{
				if( lMapLft!=lScnLft || lMapRgt!=lScnRgt || 
					lMapTop!=lScnTop || lMapBtm!=lScnBtm  )
				{
					szMsg = " Scan wafer shifted to map!";
					bCompareFail = TRUE;
				}
			}
			break;
		case 2:	//	S <= M	"Scan wafer should not large than Map!";
			if( lScnWidth>lMapWidth || lScnHeight>lMapHeight )
			{
				szMsg = " Scan wafer is large than map!";
				bCompareFail = TRUE;
			}
			break;
		case 1:	//	S <  M	"Scan wafer should small than Map!";
			if( lScnWidth>=lMapWidth || lScnHeight>=lMapHeight )
			{
				szMsg = " Scan wafer should small than map!";
				bCompareFail = TRUE;
			}
			break;
		case 0:	// not check map/scan dimension match result
		default:
			break;
		}

		if( bCompareFail )
		{
			SaveScanTimeEvent(szMsg);
			SetErrorMessage(szMsg);

			if (HmiMessage_Red_Back(szMsg, "Prescan", glHMI_MBX_CONTINUESTOP) == glHMI_STOP)
			{
				m_lPrescanVerifyResult += 2048;
			}
			else
			{
				SetErrorMessage("User chooses CONTINUE!");
			}

			szMsg.Format(" Scan wafer WxH(%ld,%ld); Map WxH(%ld,%ld).", lScnWidth, lScnHeight, lMapWidth, lMapHeight);
			SaveScanTimeEvent(szMsg);
		}
	}

	SaveScanTimeEvent("WPR: PASS DATA AND UPDATE GRADE LIST");

	lHomeScnRow += m_lMapScanOffsetRow;
	lHomeScnCol += m_lMapScanOffsetCol;

	pUtl->SetAlignPosition(lHomeScnRow, lHomeScnCol, lHomeWfX, lHomeWfY);

	if( IsVerifyMapWaferEnable() && bEmptyAction && ucEmptyGrade!=ucNullBin )
	{
		unsigned char arrIgnGrade[1];
		arrIgnGrade[0] = ucEmptyGrade;
		m_WaferMapWrapper.SelectIgnoreGrade(arrIgnGrade, 1);
	}

	if( IsMS90Sorting2ndPart() )
	{
		ULONG ulUpGood	 = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD];
		ULONG ulUpDefect = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT];
		ULONG ulUpBadcut = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT];
		ULONG ulUpEmpty  = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY];
		ULONG ulUpTotal	 = (*m_psmfSRam)["WaferTable"]["PRESCAN_TOTAL_DIE"];
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulUpGood   + ulGoodIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulUpDefect + ulDefectIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulUpBadcut + ulBadCutIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulUpEmpty  + ulEmptyIdx;
		SetMapTotalDie(ulMapTotal + ulUpTotal);
	}
	else
	{
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulGoodIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulDefectIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulBadCutIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulEmptyIdx;
		(*m_psmfSRam)["WaferTable"]["PRESCAN_TOTAL_DIE"]		= ulMapTotal;
		SetMapTotalDie(ulMapTotal);
	}
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA]	= ulExtraIdx;
	if( pApp->GetCustomerName()=="TongHui" || pApp->GetCustomerName()==CTM_SANAN )
	{
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulPickGoodCount;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulPickBadCutCount;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulPickDefectCount;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulPickEmptyCount;
	}

	szaPrSummaryList.RemoveAll();

	if( pUtl->GetPrescanRegionMode()==FALSE && IsESMachine()==FALSE )
	{
		SaveScanTimeEvent("WPR: to check grade counter abnormal increase");
		CUIntArray aulNewGradesList;
		m_WaferMapWrapper.GetAvailableGradeList(aulNewGradesList);
		CString szList, szMsg;
		BOOL bGradeCountChange = FALSE;
		szList = "Grade die counter changed after prescan\n\n";
		for(jGrade=0; jGrade<((ULONG)aulAllGradesList.GetSize()); jGrade++)
		{
			BOOL bAlive = FALSE;
			for(INT j=0; j<aulNewGradesList.GetSize(); j++)
			{
				if( aulNewGradesList.GetAt(j)==aulAllGradesList[jGrade] )
				{
					bAlive = TRUE;
					break;
				}
			}
			if( bAlive==FALSE )
			{
				continue;
			}
			ucCheckGrade = (UCHAR)aulAllGradesList[jGrade];
			if( bBadCutAction	&& (ucCheckGrade==ucBadCutGrade) )
			{
				continue;
			}
			if( bDefectAction	&& (ucCheckGrade==ucDefectGrade) )
			{
				continue;
			}
			if( bGoodAction		&& (ucCheckGrade==ucGoodGrade) )
			{
				continue;
			}
			if( bEmptyAction	&& (ucCheckGrade==ucEmptyGrade) )
			{
				continue;
			}

			m_WaferMapWrapper.GetStatistics(ucCheckGrade, ulGradeLeft, ulGradePick, ulGradeTotal);
			if( ulGradeTotal>aulGradeTotalCount.GetAt(jGrade) )
			{
				bGradeCountChange = TRUE;
				szMsg.Format("Grade %d, Current %lu, Original %lu\n", 
					ucCheckGrade, ulGradeTotal, aulGradeTotalCount.GetAt(jGrade));
				szList += szMsg;
			}
		}
		if( bGradeCountChange )
		{
			szList += "Do you want to continue SORTING";
			SetErrorMessage(szList);
			if( HmiMessage_Red_Back(szList, "Prescan", glHMI_MBX_YESNO)!=glHMI_YES )
			{
				SetErrorMessage("prescan User select to stop sorting");
				m_lPrescanVerifyResult += 512;	// grade die counter increase after prescan
			}
			else
			{
				SetErrorMessage("User select to continue sorting");
			}
		}
	}

	if( pUtl->GetPrescanRegionMode() && ( ulTgtRegion!=ulAlnRegion ) )
	{
		if( bNewFind )
		{
			pUtl->SetAlignPosition(ulNewRow, ulNewCol, lNewWftX, lNewWftY);
		}
		else
		{
			pUtl->RegionOrderLog("REGN after prescan, assist region no die found!");
		}
	}


	// special grade check
	BOOL bRefCheck = HasSpRefDieCheck() || HasReferDieCheck();
	if( bRefCheck )
	{
		SaveScanTimeEvent("WPR: to do refer check (if)");
		if( m_bCurrentCamera!=WPR_CAM_WAFER )
			ChangeCamera(WPR_CAM_WAFER, FALSE);
	}

	if( HasSpRefDieCheck() )
	{
		BOOL bReferOk = TRUE;
		SaveScanTimeEvent("WPR: special refer grade check");
		for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)
		{
			for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
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
				if( IsSpecialReferGrade(ulRow, ulCol) )
				{
					//Use Ref-DieID to search die again and update posn if necessary
					// Must Use PR Record 2 For Special Grade Check
					LONG lEncX, lEncY;
					if( GetDieValidPrescanPosn(ulRow, ulCol, 2, lEncX, lEncY) )
					{
						if( UpdatePrescanRefDiePos(ulRow, ulCol, 2, lEncX, lEncY)==FALSE )
						{
							bReferOk = FALSE;
							break;
						}
					}
					else if( m_bPrescanPrCheckReferDie )
					{
						if( HmiMessage_Red_Back("Special Refer Die has no physical position\nDo you want to continue?", "Prescan", glHMI_MBX_YESNO)==glHMI_NO )
						{
							bReferOk = FALSE;
							break;
						}
					}
				}
			}
			if( bReferOk==FALSE )
			{
				break;
			}
		}
		if( bReferOk==FALSE )
		{
			m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_SP_REFER);	// special refer check fail
			SetErrorMessage("prescan Special refer die check fail");
		}
	}

	// refer die check
	if( HasReferDieCheck() )
	{
		BOOL bReferOk = TRUE;
		LONG lEncX = 0, lEncY = 0;
		BOOL bIsRefer = TRUE;
		BOOL bReferPosn = TRUE;
		SaveScanTimeEvent("WPR: normal refer die check");
		for (ulRow = 0; ulRow <= ulMapRowMax; ulRow++)
		{
			for (ulCol = 0; ulCol <= ulMapColMax; ulCol++)
			{
				if (pUtl->GetPrescanRegionMode())
				{
					if (pSRInfo->IsWithinThisRegion(ulTgtRegion, ulRow, ulCol) == FALSE)
					{
						continue;
					}
				}
				if (IsOutMS90SortingPart(ulRow, ulCol))
				{
					continue;
				}
				bIsRefer = m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol);
				if (m_WaferMapWrapper.GetDieState(ulRow, ulCol) == WT_MAP_DIESTATE_DIMREFER)
					bIsRefer = TRUE;
				if (bIsRefer)
				{
					//Use Ref-DieID to search die again and update posn if necessary
					bReferPosn = GetDieValidPrescanPosn(ulRow, ulCol, _round(m_dLFSize), lEncX, lEncY);
					if (bReferPosn)
					{
						if (UpdatePrescanRefDiePos(ulRow, ulCol, 0, lEncX, lEncY) == FALSE)
						{
							bReferOk = FALSE;
							break;
						}
					}

					if (bReferPosn==FALSE && m_bPrescanPrCheckReferDie)
					{
						CString szMsg;
						LONG lHmiRow, lHmiCol;
						ConvertAsmToHmiUser(ulRow, ulCol, lHmiRow, lHmiCol);
						szMsg.Format("Refer Die (%ld,%ld)[%lu,%lu] has no physical position\nDo you want to continue?", lHmiRow, lHmiCol, ulRow, ulCol);
						if (HmiMessage_Red_Back(szMsg, "Prescan", glHMI_MBX_YESNO) == glHMI_NO)
						{
							bReferOk = FALSE;
							break;
						}
					}
				}
			}
			if (bReferOk == FALSE)
			{
				break;
			}
		}
		if (bReferOk == FALSE)
		{
			SetErrorMessage("prescan refer die check fail");
			m_lPrescanVerifyResult += GetScanErrorCode(SCAN_ERR_NML_REFER);	// refer die check fail
		}
	}

	if( (m_ucPrescanEdgeSizeX!=0 || m_ucPrescanEdgeSizeY!=0) && m_dEdgeGoodScore>0.0 && m_ulEdgeGoodDieTotal>0 )
	{
		DOUBLE dRemain = (m_ulEdgeGoodDieTotal - ulEdgeEmptyIdex);
		DOUBLE dTotal  = m_ulEdgeGoodDieTotal;
		DOUBLE dGood = dRemain*100/dTotal;
		szMsg.Format("Edge scanned good %lu < map good %lu (%.2f<%.2f).", 
			m_ulEdgeGoodDieTotal - ulEdgeEmptyIdex, m_ulEdgeGoodDieTotal, dGood, m_dEdgeGoodScore);
		if( dGood<m_dEdgeGoodScore )
		{
			HmiMessage_Red_Back(szMsg, "Prescan");
			m_lPrescanVerifyResult += 65536;
		}
		SaveScanTimeEvent(szMsg);
	}

	// build prescan map complete
	ClearGoodInfo();	ClearWSPrescanInfo();	WSClearScanRemainDieList();	// BGETREMAIN
	DelPrescanFrameInfo();

	SaveScanTimeEvent("WPR: CLEAR BUILDing materials");

	aulGradeTotalCount.RemoveAll();

	dwaRefWfX.RemoveAll();
	dwaRefWfY.RemoveAll();
	dwaRefRow.RemoveAll();
	dwaRefCol.RemoveAll();

	if (m_bEnable2ndPrSearch)
	{
		m_bEnable2ndPrSearch = FALSE;
	}
	if (IsAutoRescanEnable())
	{
		CString szMsg;
		szMsg.Format("%s,%s,prescan,%ld,%ld,%ld,%ld,", GetMachineNo(), GetMapNameOnly(),
				m_lPrescanSortingTotal, m_lPrescanSortingTotal, 0, m_lRescanMissingTotal);
		CString szFileName = gszUSER_DIR_MAPFILE_OUTPUT + "\\" + GetMachineNo() + "_RescanResult.csv";
		CMSLogFileUtility::Instance()->CheckAndBackupSingleLogFile(szFileName);
		CMSLogFileUtility::Instance()->AppendLogWithTime(szFileName, "\n");
		CMSLogFileUtility::Instance()->AppendLogWithTime(szFileName, szMsg);
	}	//	SanAn TJ 2017
}


INT	CWaferPr::OpPrescanInit(UCHAR ucDieNo, BOOL bFastHomeMerge)
{
	m_pPrescanPrCtrl->ResumePR();

	if( IsThisDieLearnt(ucDieNo-1)!=TRUE )
	{
		CString szMsg;
		szMsg.Format("Normal die PR %d not learnt yet!", ucDieNo);
		HmiMessage_Red_Back(szMsg);
		return gnOK;
	}

	remove(gszUSER_DIRECTORY + "\\History\\FovSearch_Map.txt");
	BOOL bInspect = m_bAoiPrescanInspction;
	if( GetNewPickCount()>0 )
		bInspect = FALSE;
	SetupMultiSearchDieCmd(ucDieNo, bFastHomeMerge, bInspect, TRUE);

	if( IsEnableZoom() )
	{
		CalculateDieInView(TRUE);	//	Init prescan
	}

	CString szMsg;
	szMsg.Format("WPR: init prescan PR %d, setup multi search die cmd inspec=%d done, sub win %d", ucDieNo, bInspect, m_uwFrameSubImageNum);
	SaveScanTimeEvent(szMsg);

	return gnOK;
}


LONG CWaferPr::GetImageNumInGallery()
{
	m_lPrescanImageCount = m_pPrescanPrCtrl->GalleryNumImage();
	return m_lPrescanImageCount;
}

BOOL CWaferPr::AutoGrabShareImage()
{
	int siEncX = GetScanLastPosnX();	//	427TX	4
	int siEncY = GetScanLastPosnY();
	LONG ulScnRow = m_nPrescanLastMapRow;
	LONG ulScnCol = m_nPrescanLastMapCol;	//	427TX	4
	LONG lFrameRow = GetLastFrameRow();
	LONG lFrameCol = GetLastFrameCol();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	pUtl->SetPrescanMapIndexIdle(FALSE);
	m_lLastGrabWfX	= siEncX;
	m_lLastGrabWfY	= siEncY;
	m_lPrescanLastGrabRow	= ulScnRow;
	m_lPrescanLastGrabCol	= ulScnCol;
	m_bPrescanLastGrabbed	= FALSE;

	if( IsStitchMode() && m_lImageStichExtraDelay > 0 )
	{
		Sleep(m_lImageStichExtraDelay);
	}

	BOOL bGrabResult = TRUE;
	if( IsBurnIn() && (m_bNoWprBurnIn) )
	{
		Sleep(20);
		SetDieReady(TRUE);
	}
	else
	{
		bGrabResult = m_pPrescanPrCtrl->GrabShareImage(lFrameCol, lFrameRow, siEncX, siEncY, ulScnRow, ulScnCol);
	}

	CString szText;
	LONG lHmiRow = 0, lHmiCol = 0;
	ConvertAsmToHmiUser(ulScnRow, ulScnCol, lHmiRow, lHmiCol);
	szText.Format("pr gbd, %5ld,%5ld,%5ld,%5ld,%8d,%8d,%5ld,%5ld, %lu",
			ulScnRow, ulScnCol, lHmiRow, lHmiCol, siEncX, siEncY, lFrameRow, lFrameCol, m_pPrescanPrCtrl->GetShareImageID());
	pUtl->PrescanMoveLog(szText);

	m_bPrescanLastGrabbed	= TRUE;
	if( IsPrescanMapIndex() || pUtl->GetPrescanAreaPickMode() )
	{
		pUtl->SetPrescanMapIndexIdle( IsScanPrIdle() );
	}
	
	return bGrabResult;
}

VOID CWaferPr::AF_ObtainScore_MoveToLevel()
{
	if( IsWprWithAF()==FALSE )
	{
		return ;
	}

	if( m_bAFGridSampling==FALSE && m_bAFDynamicAdjust==FALSE && m_bAFRescanBadFrame==FALSE )
	{
		return ;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	LONG ulLastCol = GetLastFrameCol();
	LONG ulLastRow = GetLastFrameRow();
	LONG ulNextCol = m_lNextFrameCol;
	LONG ulNextRow = m_lNextFrameRow;

	if( IsPrescanMapIndex() )
	{
		ulLastRow = m_lPrescanLastGrabRow;
		ulLastCol = m_lPrescanLastGrabCol;
		ulNextRow = m_nPrescanNextMapRow;
		ulNextCol = m_nPrescanNextMapCol;
	}

	CString szMsg;
	ULONG ulImageID = m_pPrescanPrCtrl->GetShareImageID();
	DOUBLE dLastFocusScore = m_pPrescanPrCtrl->GetShareImageFocusScore();
	DOUBLE dAvgScore = 0;
	if( m_bAFDynamicAdjust || m_bAFRescanBadFrame )	// get focus score and update into memory
	{	//	get the focus score for dynamic adjust or rescan bad frame after prescan.
		// set focus score to storage memory
		if( m_stAF_Data.m_lFrameMinCol>ulLastCol )
			m_stAF_Data.m_lFrameMinCol	= ulLastCol;
		if( m_stAF_Data.m_lFrameMaxCol<ulLastCol )
			m_stAF_Data.m_lFrameMaxCol	= ulLastCol;
		if( m_stAF_Data.m_lFrameMinRow>ulLastRow )
			m_stAF_Data.m_lFrameMinRow	= ulLastRow;
		if( m_stAF_Data.m_lFrameMaxRow<ulLastRow )
			m_stAF_Data.m_lFrameMaxRow	= ulLastRow;

		SetGrabFocusData(ulLastRow, ulLastCol, m_lLastGrabWfX, m_lLastGrabWfY, ulImageID, dLastFocusScore);
		CString szList = "", szTemp;
		if( m_bAFDynamicAdjust )	//	auto cycle move, calcualte average focus score
		{
			DOUBLE dFocusScore = -1.0;
			LONG lCounter = 0;
			LONG lGetRow, lGetCol;
			for(LONG lRow=0; lRow<7; lRow++)
			{
				for(LONG lCol=0; lCol<7; lCol++)
				{
					if( IsPrescanMapIndex()==FALSE )
					{
						lGetRow = (lRow-3)*1 + ulLastRow;
						lGetCol = (lCol-3)*1 + ulLastCol;
					}
					else
					{
						lGetRow = (lRow-3)*GetMapIndexStepRow() + ulLastRow;
						lGetCol = (lCol-3)*GetMapIndexStepCol() + ulLastCol;
					}
					if( lGetRow>=0 && lGetCol>=0 )
					{
						dFocusScore = GetGrabFocusScore(lGetRow, lGetCol);
						if( dFocusScore>0 )
						{
							dAvgScore += dFocusScore;
							lCounter++;
							szTemp.Format(" (%ld,%ld)=>%.2f", lGetRow, lGetCol, dFocusScore);
							szList += szTemp;
						}
					}
				}
			}
			if( lCounter>0 )
				dAvgScore = dAvgScore/lCounter;
		}
		szMsg.Format("AF frm, %5lu,%5lu,%5lu,%5lu,%8ld,%8ld, %lu, cur %.2f, avg %.2f from %s",
			ulLastRow, ulLastCol, ulNextRow, ulNextCol, m_lLastGrabWfX, m_lLastGrabWfY, ulImageID, dLastFocusScore, dAvgScore, szList);
		pUtl->PrescanMoveLog(szMsg);
	}

	if( m_bAFGridSampling || m_bAFDynamicAdjust )	// auto cycle move
	{
		BOOL bMoveZ = FALSE;
		LONG lAdjLevel = 0;
		LONG lPdcLevel = (*m_psmfSRam)["WPR Auto Focus"]["Sample Level"];
		if( m_bAFGridSampling )	// auto cycle move
		{
			bMoveZ = TRUE;
		}
		else
		{
			lPdcLevel = m_lAF_RuntimeLevel;
		}

		if( m_bAFDynamicAdjust )	//	auto cycle move
		{
			// based on next to calculate adjust deltaz
			if( ulNextRow==GetAlignFrameRow() && ulNextRow==GetAlignFrameCol() )
			{
				m_stAF_Data.m_dLastFocusScore = GetGrabFocusScore(ulNextRow, ulNextCol);
				m_stAF_Data.m_lFrameFocusDir = 0;
				m_stAF_Data.m_lFrameFocusStep = 0;
			}
			else
			{
				if( dLastFocusScore>0.05 && (dLastFocusScore<m_dAFDynamicAdjustLimit || dLastFocusScore<dAvgScore) )
				{
					if( dLastFocusScore<m_stAF_Data.m_dLastFocusScore )
					{
						if( m_stAF_Data.m_lFrameFocusDir<0 )
						{
							m_stAF_Data.m_lFrameFocusDir = 1;
						}
						else if( m_stAF_Data.m_lFrameFocusDir==0 )
						{
							m_stAF_Data.m_lFrameFocusDir = 1;
						}
						else if( m_stAF_Data.m_lFrameFocusDir>0 )
						{
							m_stAF_Data.m_lFrameFocusDir = -1;
						}
					}

					if( m_stAF_Data.m_lFrameFocusDir<0 )
					{
						if( m_bAFGridSampling )
							m_stAF_Data.m_lFrameFocusStep--;
						else
							m_stAF_Data.m_lFrameFocusStep = -1;
					}
					else if( m_stAF_Data.m_lFrameFocusDir==0 )
					{
						if( !m_bAFGridSampling )
							m_stAF_Data.m_lFrameFocusStep = 0;
					}
					else if( m_stAF_Data.m_lFrameFocusDir>0 )
					{
						if( m_bAFGridSampling )
							m_stAF_Data.m_lFrameFocusStep++;
						else
							m_stAF_Data.m_lFrameFocusStep = 1;
					}
				}
				else
				{
					m_stAF_Data.m_lFrameFocusStep = 0;
				}
				m_stAF_Data.m_dLastFocusScore = dLastFocusScore;
			}
			lAdjLevel = m_stAF_Data.m_lFrameFocusStep*m_lAFDynamicAdjustStep; // 1 um adjust range
			bMoveZ = TRUE;
		}

		if( bMoveZ )
		{
			LONG lTgtLevel = lPdcLevel + lAdjLevel;
			LONG lMoveZ = lTgtLevel - m_lAF_RuntimeLevel;
			LONG lZTime = MotionGetProfileTime(WAFERPR_AXIS_Z, "mpfWaferPrZNormal", lMoveZ, lMoveZ, HIPEC_SAMPLE_RATE, &m_stZoomAxis_Z);
			szMsg.Format("AF clc, %5lu,%5lu,%5ld,%5ld,%8ld,%8ld,%8ld,%8ld,time %ld",
				ulNextRow, ulNextCol, m_stAF_Data.m_lFrameFocusDir, m_stAF_Data.m_lFrameFocusStep, lPdcLevel, lAdjLevel, lTgtLevel, lMoveZ, lZTime);
			pUtl->PrescanMoveLog(szMsg);
			Z_MoveTo(lTgtLevel, SFM_NOWAIT);
			m_lAF_RuntimeLevel = lTgtLevel;
		}
	}
}

VOID CWaferPr::SetupMultiSearchDieCmd(LONG lInputDieNo, BOOL bFastHomeMerge, BOOL bInspect, BOOL bUseScanAngle)
{
	PR_BOOLEAN	bBackupAlign = PR_FALSE;
	PR_BOOLEAN	bCheckChip = PR_TRUE;
	PR_BOOLEAN	bCheckDefect = PR_TRUE;
	PR_UBYTE	ucDefectThres = PR_NORMAL_DEFECT_THRESHOLD;
	LONG		lAlignAccuracy;
	LONG		lGreyLevelDefect;
	DOUBLE		dMinChipArea = 0.0;
	DOUBLE		dSingleDefectArea = 0.0;
	DOUBLE		dTotalDefectArea = 0.0;
	PR_UBYTE	ubSenderID	 =  GetScnSenID();
	PR_UBYTE	ubReceiverID =  GetScnRecID();

	UCHAR							lDieNo = 0;
	PR_UWORD						lPrSrchID;
	if (lInputDieNo <= 0)
		lInputDieNo = 1;
	lDieNo			= WPR_GEN_NDIE_OFFSET + (UCHAR)lInputDieNo;
	lPrSrchID	= GetDiePrID(lDieNo);

	if (lPrSrchID <= 0)
	{
		CString szMsg;
		szMsg = _T("Prescan PR ID Not Leant");
		HmiMessage_Red_Back(szMsg);
		return;
	}
	
	if (lDieNo >= WPR_MAX_DIE)		//Klocwork	//v4.46
	{
		return;
	}

	switch(m_lGenSrchAlignRes[lDieNo])
	{
	case 1:		
		lAlignAccuracy	= PR_LOW_DIE_ALIGN_ACCURACY;	
		break;

	default:	
		lAlignAccuracy	= PR_HIGH_DIE_ALIGN_ACCURACY;	
		break;
	}

	switch(m_lGenSrchGreyLevelDefect[lDieNo])
	{
	case 1: 
		lGreyLevelDefect = PR_DEFECT_ATTRIBUTE_BRIGHT;
		break;
	
	case 2:
		lGreyLevelDefect = PR_DEFECT_ATTRIBUTE_DARK;
		break;

	default:
		lGreyLevelDefect = PR_DEFECT_ATTRIBUTE_BOTH;
		break;
	}

	ucDefectThres = (PR_UBYTE)m_lGenSrchDefectThres[lDieNo];

	if (m_dGenSrchChipArea[lDieNo] == 0.0)
	{
		bCheckChip = PR_FALSE;
	}

	if (m_bGenSrchEnableChipCheck[lDieNo] == FALSE)
	{
		bCheckChip = PR_FALSE;
	}

	if ((m_dGenSrchSingleDefectArea[lDieNo] == 0.0) || (m_dGenSrchTotalDefectArea[lDieNo] == 0.0))
	{
		bCheckDefect = PR_FALSE;
	}

	if (m_bGenSrchEnableDefectCheck[lDieNo] == FALSE)
	{
		bCheckDefect = PR_FALSE;
	}

	//Check Input parameter
	if ( (bInspect == FALSE) )
	{
		bCheckDefect = PR_FALSE;
		bCheckChip = PR_FALSE;
	}

	bBackupAlign = (PR_BOOLEAN)m_bSrchEnableBackupAlign;

	//Calculate chip die area, min & total defect area
	dMinChipArea		= ((DOUBLE)(GetScanDieSizePixelX(lDieNo) * GetScanDieSizePixelY(lDieNo)) / 100) * m_dGenSrchChipArea[lDieNo];
	dSingleDefectArea	= ((DOUBLE)(GetScanDieSizePixelX(lDieNo) * GetScanDieSizePixelY(lDieNo)) / 100) * m_dGenSrchSingleDefectArea[lDieNo];  
	dTotalDefectArea	= ((DOUBLE)(GetScanDieSizePixelX(lDieNo) * GetScanDieSizePixelY(lDieNo)) / 100) * m_dGenSrchTotalDefectArea[lDieNo]; 


	PR_2_POINTS_DIE_ALIGN_CMD       st2PointsAlignCmd;
	PR_Init2PointsDieAlignCmd(&st2PointsAlignCmd);
	//Setup Search Die parameter
	st2PointsAlignCmd.emConsistentDiePos		= PR_TRUE;
	st2PointsAlignCmd.emBackupTmpl				= PR_FALSE;      
	st2PointsAlignCmd.emSingleTmplSrch			= PR_FALSE;
	st2PointsAlignCmd.emSingleDieSrch			= PR_TRUE;
	st2PointsAlignCmd.emPartialDieSrch			= PR_FALSE;     
	st2PointsAlignCmd.emDieRotChk				= PR_TRUE;
	st2PointsAlignCmd.emMaxDieSizeChk			= PR_FALSE;
	st2PointsAlignCmd.emMinDieSizeChk			= PR_FALSE;
	st2PointsAlignCmd.emDieEdgeChk				= PR_TRUE;
	st2PointsAlignCmd.emPositionChk				= PR_FALSE;
	st2PointsAlignCmd.coRefPoint.x				= 0;
	st2PointsAlignCmd.coRefPoint.y				= 0;
	st2PointsAlignCmd.rRefAngle					= 0;
	st2PointsAlignCmd.coMaxShift.x				= 0;
	st2PointsAlignCmd.coMaxShift.y				= 0;
	st2PointsAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	st2PointsAlignCmd.szPosConsistency.x		= (PR_LENGTH) (PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.szPosConsistency.y		= (PR_LENGTH) (PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.rDieRotTol				= GetScanRotTol(bUseScanAngle);

	PR_STREET_DIE_ALIGN_CMD         stStreetAlignCmd;
	PR_InitStreetDieAlignCmd(&stStreetAlignCmd);
	stStreetAlignCmd.emConsistentDiePos			= PR_TRUE;
	stStreetAlignCmd.emSingleDieSrch			= PR_TRUE;
	stStreetAlignCmd.emPartialDieSrch			= PR_FALSE;
	stStreetAlignCmd.emDieRotChk				= PR_TRUE;
	stStreetAlignCmd.emMaxDieSizeChk			= PR_FALSE;
	stStreetAlignCmd.emMinDieSizeChk			= PR_FALSE;
	stStreetAlignCmd.emDieEdgeChk				= PR_TRUE;
	stStreetAlignCmd.emPositionChk				= PR_FALSE;
	stStreetAlignCmd.coRefPoint.x				= 0;
	stStreetAlignCmd.coRefPoint.y				= 0;
	stStreetAlignCmd.coMaxShift.x				= 0;
	stStreetAlignCmd.coMaxShift.y				= 0;
	stStreetAlignCmd.rRefAngle					= 0;
	stStreetAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	stStreetAlignCmd.szPosConsistency.x			= (PR_LENGTH) (PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	stStreetAlignCmd.szPosConsistency.y			= (PR_LENGTH) (PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	stStreetAlignCmd.rDieRotTol					= GetScanRotTol(bUseScanAngle);

	PR_DIE_ALIGN_CMD                stDieAlignCmd;
	PR_InitDieAlignCmd(&stDieAlignCmd);
	stDieAlignCmd.st2Points						= st2PointsAlignCmd;
	stDieAlignCmd.stStreet						= stStreetAlignCmd;

	PR_TMPL_DIE_INSP_CMD            stTmplInspCmd;
	PR_InitTmplDieInspCmd(&stTmplInspCmd);
	stTmplInspCmd.emInspIncompleteDie			= PR_FALSE;     
	if (m_bEnableLineDefect)
	{
		stTmplInspCmd.emEnableLineDefectDetection		= PR_TRUE;
		stTmplInspCmd.uwNumLineDefects					= (PR_UWORD) m_lMinNumOfLineDefects;
		stTmplInspCmd.rMinLongestSingleLineDefectLength = (PR_REAL) ConvertScanDUnitToPixel(m_dMinLineDefectLength);		//mil -> pixel
		stTmplInspCmd.rMinLineClassLength				= (PR_REAL) ConvertScanDUnitToPixel(m_dMinLineDefectLength) / 2;
		stTmplInspCmd.rMinTotalLineDefectLength			= (PR_REAL) ConvertScanDUnitToPixel(m_dMinLineDefectLength) * 2;
	}
	stTmplInspCmd.emChipDieChk					= bCheckChip;
	stTmplInspCmd.emGeneralDefectChk			= bCheckDefect;
	stTmplInspCmd.emInkOnlyChk					= PR_FALSE;     
	stTmplInspCmd.aeMinSingleDefectArea			= (PR_AREA)dSingleDefectArea;
	stTmplInspCmd.aeMinTotalDefectArea			= (PR_AREA)dTotalDefectArea;  
	stTmplInspCmd.aeMinChipArea					= (PR_AREA)dMinChipArea; 
	stTmplInspCmd.emNewDefectCriteria			= PR_TRUE;
	stTmplInspCmd.ubIntensityVariation			= PR_DEF_INTENSITY_VARIATION;
	stTmplInspCmd.ubDefectThreshold				= ucDefectThres; 
	stTmplInspCmd.rMinInkSize					= PR_FALSE;     
	stTmplInspCmd.emDefectAttribute				= (PR_DEFECT_ATTRIBUTE)(lGreyLevelDefect);

	PR_DIE_INSP_CMD 				stDieInspCmd;
	PR_InitDieInspCmd(&stDieInspCmd);
	stDieInspCmd.stTmpl							= stTmplInspCmd;

	PR_DIE_ALIGN_PAR                stAlignPar;
	PR_InitDieAlignPar(&stAlignPar);
	stAlignPar.emEnableBackupAlign				= bBackupAlign;
	stAlignPar.rStartAngle						= GetScanStartAngle(bUseScanAngle);	
	stAlignPar.rEndAngle						= GetScanEndAngle(bUseScanAngle);		

	PR_POST_INDIV_INSP_CMD			stBadCutCmd;
	PR_InitPostIndivInspCmd(&stBadCutCmd);
	PR_POST_INSP_CMD 			    stBadCutInspCmd;

	PR_InitPostInspCmd(&stBadCutInspCmd);

	PR_POST_INDIV_INSP_EXT1_CMD		stBadCutExtCmd = stBadCutInspCmd.stEpoxy.stIndivExt1;

	//Setup badcut parameter
	if ( GetDieShape() == WPR_RECTANGLE_DIE )
	{
		stBadCutCmd.emEpoxyInsuffChk				= PR_TRUE;
		stBadCutCmd.emEpoxyExcessChk				= PR_FALSE;
		stBadCutCmd.uwMaxInsuffSide					= 0;
		stBadCutCmd.stMinPercent.x					= (PR_REAL)(100 - m_stBadCutTolerenceX);
		stBadCutCmd.stMinPercent.y					= (PR_REAL)(100 - m_stBadCutTolerenceY);
		stBadCutCmd.szScanWidth.x					= (PR_LENGTH)m_stBadCutScanWidthX;
		stBadCutCmd.szScanWidth.y					= (PR_LENGTH)m_stBadCutScanWidthY;
		stBadCutCmd.stEpoxyInsuffWin.coCorner1		= m_stBadCutPoint[0];
		stBadCutCmd.stEpoxyInsuffWin.coCorner2		= m_stBadCutPoint[1];
		stBadCutCmd.stEpoxyInsuffWin.coObjCentre.x	= (PR_WORD) GetPrCenterX();
		stBadCutCmd.stEpoxyInsuffWin.coObjCentre.y	= (PR_WORD) GetPrCenterY();

		stBadCutInspCmd.emEpoxyAlg					= PR_INDIV_INSP;
		stBadCutInspCmd.stEpoxy.stIndiv				= stBadCutCmd;
	}
	else
	{
		PR_UBYTE lCorners = WPR_HEXAGON_CORNERS;
		if ( GetDieShape() == WPR_RHOMBUS_DIE )
		{
			lCorners	= WPR_RHOMBUS_CORNERS;				
		}	//	rhombus die
		else if( GetDieShape() == WPR_TRIANGULAR_DIE )
		{
			lCorners	= WPR_TRIANGLE_CORNERS;				
		}
		PR_LINE stLine;
		for (LONG i=0; i<lCorners; i++)
		{
			stLine.coPoint1 = m_stBadCutPoint[i];
	
			if ( i >= (lCorners-1) )
			{
				stLine.coPoint2 = m_stBadCutPoint[0];
			}
			else
			{
				stLine.coPoint2 = m_stBadCutPoint[i+1];
			}

			stBadCutExtCmd.auwScanWidth[i]	= (PR_LENGTH)m_stBadCutScanWidthX;
			stBadCutExtCmd.aubMinPercent[i] = (PR_UBYTE)(100 - m_stBadCutTolerenceX);
			stBadCutExtCmd.astInsuffLine[i]	= stLine;
		}
		stBadCutExtCmd.ubNumOfInsuffLine	= lCorners;

		stBadCutExtCmd.emEpoxyInsuffChk		= PR_TRUE;
		stBadCutExtCmd.emEpoxyExcessChk		= PR_FALSE;
		stBadCutExtCmd.uwMaxInsuffSide		= 0;
		stBadCutExtCmd.coObjCentre.x		= (PR_WORD) GetPrCenterX();
		stBadCutExtCmd.coObjCentre.y		= (PR_WORD) GetPrCenterY();
		stBadCutExtCmd.rObjAngle			= 0.0;

		stBadCutInspCmd.emEpoxyAlg			= PR_INDIV_INSP_EXT1;
		stBadCutInspCmd.stEpoxy.stIndivExt1	= stBadCutExtCmd;
	}
	stBadCutInspCmd.emEpoxyChk			= PR_TRUE;
#ifndef VS_5MCAM
	if( m_bBadcutBiDetect )	//	new library missing, need added in if in need in future.
	{
	//	stBadCutInspCmd.emSegMode	= PR_POST_EPOXY_SEG_MODE_BINARY;
	//	stBadCutInspCmd.ubThd		= (PR_UBYTE) m_lBadcutBiThreshold;
	}
#endif

	//Klocwork	//v4.02T5
	stBadCutInspCmd.stEpoxy.stIndivExt1.ubNumOfExcessLine	= 0;
	stBadCutInspCmd.stEpoxy.stIndivExt1.uwMaxExcessSide		= 0;

	PR_SRCH_DIE_CMD stMultiSrchCmd;
	PR_InitSrchDieCmd(&stMultiSrchCmd);
	stMultiSrchCmd.emLatch			= PR_TRUE;
	stMultiSrchCmd.uwNRecordID		= 1;
	stMultiSrchCmd.auwRecordID[0]		= (PR_UWORD)(lPrSrchID);
	stMultiSrchCmd.emAlign			= PR_TRUE;
	stMultiSrchCmd.emDefectInsp		= (PR_BOOLEAN) bInspect;
	stMultiSrchCmd.emCameraNo			= GetScnCamID();
	stMultiSrchCmd.emVideoSource		= PR_IMAGE_BUFFER_A;
	stMultiSrchCmd.ulRpyControlCode	= PR_DEF_SRCH_DIE_RPY;

	stMultiSrchCmd.stDieAlign			= stDieAlignCmd;
	stMultiSrchCmd.stDieInsp			= stDieInspCmd;
	stMultiSrchCmd.stDieAlignPar		= stAlignPar;
	stMultiSrchCmd.stPostBondInsp		= stBadCutInspCmd;
#ifdef VS_5MCAM
	stMultiSrchCmd.stDieAlign.stStreet.emPartialDieSrch	= PR_FALSE;	//	PR_TRUE;
	stMultiSrchCmd.stDieAlign.st2Points.emPartialDieSrch	= PR_FALSE;	//	PR_TRUE;
#endif

	stMultiSrchCmd.stDieAlignPar.rStartAngle		= GetScanStartAngle(bUseScanAngle);	
	stMultiSrchCmd.stDieAlignPar.rEndAngle			= GetScanEndAngle(bUseScanAngle);	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
	{
		if (m_lGenSrchDieScore[WPR_NORMAL_DIE] < 70)		//v2.83T58
		{
			m_lGenSrchDieScore[WPR_NORMAL_DIE] = 70;
		}
		m_lPrescanSrchDieScore = m_lGenSrchDieScore[WPR_NORMAL_DIE];
	}
	LONG	lPrescanSrchDieScore = m_lPrescanSrchDieScore;
	if( lInputDieNo==3 )
	{
		lPrescanSrchDieScore = m_lGenSrchDieScore[lDieNo];
	}
	if( lPrescanSrchDieScore==0 )
	{
		lPrescanSrchDieScore = m_lGenSrchDieScore[WPR_NORMAL_DIE];
	}
	stMultiSrchCmd.stDieAlignPar.rMatchScore			= (PR_REAL)(lPrescanSrchDieScore);
	stMultiSrchCmd.stDieAlignPar.emIsDefaultMatchScore= PR_FALSE;

	stMultiSrchCmd.emGraphicInfo		= PR_NO_DISPLAY;
	stMultiSrchCmd.emLatchMode		= PR_LATCH_FROM_GALLERY;

	if ( (bInspect) && (m_bBadCutDetection == TRUE) && (m_lCurBadCutSizeX > 0) && (m_lCurBadCutSizeY > 0))
	{
		stMultiSrchCmd.emPostBondInsp = PR_TRUE;  
	}
	else
	{
		stMultiSrchCmd.emPostBondInsp = PR_FALSE;  
	}

	stMultiSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = GetScanPRWinULX();
	stMultiSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = GetScanPRWinULY();
	stMultiSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = GetScanPRWinLRX();
	stMultiSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = GetScanPRWinLRY();

	stMultiSrchCmd.emDieInspAlg		= GetLrnInspMethod(lDieNo);
	stMultiSrchCmd.emAlignAlg		= GetLrnAlignAlgo(lDieNo);

#if	0	//	don't remove
	//	Fast Rescan
	if( IsAutoRescanEnable() && GetNewPickCount()>0 && m_bAccurateRescan==FALSE )
	{
		SaveScanTimeEvent("WPR: rescan enable fast mode");
		stMultiSrchCmd.stDieAlignPar.ubIsEnableFastPreScan = PR_TRUE;
	}
	else
	{
		SaveScanTimeEvent("WPR: scan enable normal mode");
		stMultiSrchCmd.stDieAlignPar.ubIsEnableFastPreScan = PR_FALSE;
	}
	//	Fast Rescan
#endif

	m_stMultiSrchCmd	= stMultiSrchCmd;

	m_pPrescanPrCtrl->SetupSearchCommand(m_stMultiSrchCmd);
	m_pPrescanPrCtrl->SetupSubImageNum(m_uwFrameSubImageNum);

	m_uwScanRecordID[0] = lPrSrchID;
	m_uwScanRecordID[1] = 0;
	if( IsPrecanWith2Pr() )	// MS init PR record ID 2 multi-search parameters.
	{
		UCHAR ucDieNoIndex	= 0;
		if( IsPrescanReferDie() )
		{
			ucDieNoIndex	= WPR_GEN_NDIE_OFFSET + 4;	// refer 1st die
		}
		else
		{
			ucDieNoIndex = GetPrescan2ndPrID() - 1;	// prescan 2nd pr id index
			if( !IsThisDieLearnt(ucDieNoIndex) )
			{
				ucDieNoIndex	= 0;
			}
		}
		DisplaySequence("WPR - down and init 2nd PR scan search record");
		SetupScanSearchCmd2(ucDieNoIndex, FALSE);
	}
}

VOID CWaferPr::SetAllPrescanDone(BOOL bState)
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

BOOL CWaferPr::WaitAllPrescanDone(INT nTimeout)
{
	CSingleLock slLock(&m_evAllPrescanDone);
	return slLock.Lock(nTimeout);
}

VOID CWaferPr::RegPrescanVarFunc()
{
	try
	{
		// Prescan	command
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IntoMapScanCheckPage"),			&CWaferPr::IntoMapScanCheckPage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectCheckCenterDie"),			&CWaferPr::SelectCheckCenterDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearAllMapScanPoints"),		&CWaferPr::ClearAllMapScanPoints);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmMapScanPoints"),			&CWaferPr::ConfirmMapScanPoints);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveScanCheckOption"),			&CWaferPr::SaveScanCheckOption);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AddScanAlignPattern4MS90"),		&CWaferPr::AddScanAlignPattern4MS90);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ScanAlignPatternFromMapTest"),	&CWaferPr::ScanAlignPatternFromMapTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetScanAlignVerifyMode"),		&CWaferPr::SetScanAlignVerifyMode);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckHolePatternInThisMap"),	&CWaferPr::CheckHolePatternInThisMap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrescanDieResult"),				&CWaferPr::PrescanDieResult);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MultiSearchInit"),				&CWaferPr::MultiSearchInit);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CP100MultiSearchDie"),			&CWaferPr::CP100MultiSearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MultiSearchDie"),				&CWaferPr::MultiSearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearPrescanData"),				&CWaferPr::ClearPrescanData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SavePrescanData"),				&CWaferPr::SavePrescanData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrescanDieAction"),				&CWaferPr::PrescanDieAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LFAutoLearnDiePitch"),			&CWaferPr::LFAutoLearnDiePitch);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrAutoBondScreenUpdate"),		&CWaferPr::PrAutoBondScreenUpdate);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RefreshPrScreen"),				&CWaferPr::RefreshPrScreen);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GrabAndSaveImageCmd"),			&CWaferPr::GrabAndSaveImageCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MultiSearchNml3Die"),			&CWaferPr::MultiSearchNml3Die);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetSrchThreadNumCmd"),			&CWaferPr::SetSrchThreadNumCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MultiSearchInitNmlDie1"),		&CWaferPr::MultiSearchInitNmlDie1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MultiSearchNmlDie1"),			&CWaferPr::MultiSearchNmlDie1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReconstructMapByFile"),			&CWaferPr::ReconstructMapByFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckNormalDiePattern"),		&CWaferPr::CheckNormalDiePattern);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadScanAlignSettingFile"),		&CWaferPr::LoadScanAlignSettingFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadSkipNgGrades"),				&CWaferPr::LoadSkipNgGrades);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WprDumpLog"),					&CWaferPr::WprDumpLog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GridLearnDieCalibration"),		&CWaferPr::GridLearnDieCalibration);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GridLearnDieCalibration_AutoMode"),	&CWaferPr::GridLearnDieCalibration_AutoMode);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CP100CheckTableLinearity"),		&CWaferPr::CP100CheckTableLinearity);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CPCheckOpticsDistortion"),		&CWaferPr::CPCheckOpticsDistortion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DrawBlockProbeDie"),			&CWaferPr::DrawBlockProbeDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchDieInBigWindow"),			&CWaferPr::SearchDieInBigWindow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleFailureCaselog"),			&CWaferPr::ToggleFailureCaselog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleBHMarkCaselog"),			&CWaferPr::ToggleBHMarkCaselog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetDebugViewerLog"),			&CWaferPr::SetDebugViewerLog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetPrDebugFlag"),				&CWaferPr::SetPrDebugFlag);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectAoiOcrDieNo"),			&CWaferPr::SelectAoiOcrDieNo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetOcrDieDetectWindow"),		&CWaferPr::SetOcrDieDetectWindow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetOcrDieDetectWindow"),		&CWaferPr::GetOcrDieDetectWindow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetOcrDieWindows"),			&CWaferPr::ResetOcrDieWindows);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadAoiGradeConvertFile"),		&CWaferPr::LoadAoiGradeConvertFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CounterCheckReferOnNormalDie"),	&CWaferPr::CounterCheckReferOnNormalDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ScanSampleKeyDice"),			&CWaferPr::ScanSampleKeyDice);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleScanSortZoom"),			&CWaferPr::ToggleScanSortZoom);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WalkOnTwinDice"),				&CWaferPr::WalkOnTwinDice);
		//	WL contour camera.
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SwitchContourLighting"),		&CWaferPr::SwitchContourLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WLUsePRMouse"),					&CWaferPr::WLUsePRMouse);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WLDrawComplete"),				&CWaferPr::WLDrawComplete);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WLCancelMouseDraw"),			&CWaferPr::WLCancelMouseDraw);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WLMovePRCursor"),				&CWaferPr::WLMovePRCursor);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WLCalibrateToCorner"),			&CWaferPr::WLCalibrateToCorner);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WLConfirmCalibrate"),			&CWaferPr::WLConfirmCalibrate);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WLTeachCameraOffset"),			&CWaferPr::WLTeachCameraOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetContourImagePath"),			&CWaferPr::SetContourImagePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetContourImagePath"),			&CWaferPr::GetContourImagePath);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetOcrDieViewWindow"),			&CWaferPr::SetOcrDieViewWindow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetOcrDieViewWindow"),			&CWaferPr::GetOcrDieViewWindow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LearnBarPitch"),				&CWaferPr::LearnBarPitch);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BarScanGoDie"),					&CWaferPr::BarScanGoDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeMapOcrValue"),			&CWaferPr::ChangeMapOcrValue);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBarResult"),				&CWaferPr::ConfirmBarResult);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SkipBarResult"),				&CWaferPr::SkipBarResult);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmAllDone"),				&CWaferPr::ConfirmAllDone);

		// Prescan	variable
		RegVariable(_T("WPR_szBarFirstDieImage"),		&m_szBarFirstDieImage);	
		RegVariable(_T("WPR_szBarLastDieImage"),		&m_szBarLastDieImage);	
		RegVariable(_T("WPR_lOcrImageRotation"),		&m_lOcrImageRotation);
		RegVariable(_T("WPR_bOcrConfirmControl"),		&m_bOcrConfirmControl);
		RegVariable(_T("WPR_bOcrConfirmButton"),		&m_bOcrConfirmButton);
		RegVariable(_T("WPR_lOcrPassScore"),			&m_lOcrPassScore);
		for(int i=0; i<PAGE_BAR_MAX; i++)
		{
			CString szTemp;
			szTemp.Format("%02d", i+1);
			RegVariable(_T("WPR_szOcrBarName"+szTemp),		&m_szOcrBarName[i]);
			RegVariable(_T("WPR_szOcrValueLFBar"+szTemp),	&m_szOcrValueLFBar[i]);
			RegVariable(_T("WPR_szOcrValueLLBar"+szTemp),	&m_szOcrValueLLBar[i]);
			RegVariable(_T("WPR_szOcrValueRFBar"+szTemp),	&m_szOcrValueRFBar[i]);
			RegVariable(_T("WPR_szOcrValueRLBar"+szTemp),	&m_szOcrValueRLBar[i]);
			szTemp.Format("%d", i+1);
			RegVariable(_T("WPR_bOcrResultBarL"+szTemp),		&m_bOcrResultBarL[i]);
			RegVariable(_T("WPR_bOcrResultBarR"+szTemp),		&m_bOcrResultBarR[i]);
		}
		//
		RegVariable(_T("WPR_lPrescanIndexRowReduce"),	&m_lPrescanIndexRowReduce);
		RegVariable(_T("WPR_lPrescanIndexColReduce"),	&m_lPrescanIndexColReduce);
		RegVariable(_T("WPR_bEnable2ndPrSearch"),		&m_bEnable2ndPrSearch);
		RegVariable(_T("WPR_bKeepScan2ndPrDie"),		&m_bKeepScan2ndPrDie);
		RegVariable(_T("WPR_lSelect2ndPrID"),			&m_lSelect2ndPrID);
		RegVariable(_T("WPR_bAoiPscnInsp"),				&m_bAoiPrescanInspction);
		RegVariable(_T("WPR_bSendWaferIDToPR"),			&m_bSendWaferIDToPR);
		RegVariable(_T("WPR_lPrescanImageLimit"),		&m_lPrescanImageLimit);
		RegVariable(_T("WPR_lPrescanImageCount"),		&m_lPrescanImageCount);
		RegVariable(_T("WPR_lScanIndexStopCount"),		&m_lScanIndexStopCount);
		RegVariable(_T("WPR_dScanPrCleanTime"),			&m_dScanPrCleanTime);
		RegVariable(_T("WPR_lPScnSrchDieScore"),		&m_lPrescanSrchDieScore);
		RegVariable(_T("WPR_lUsedSrchThreadNum"),		&m_lUsedSrchThreadNum);
		RegVariable(_T("WPR_lWaitingImageLimit"),		&m_lWaitingImageLimit);
		RegVariable(_T("WPR_lPrescanOverlapX"),			&m_lExtraPrescanOverlapX);
		RegVariable(_T("WPR_lPrescanOverlapY"),			&m_lExtraPrescanOverlapY);
		RegVariable(_T("WPR_lLookAroundRow"),			&m_lLookAroundRow);
		RegVariable(_T("WPR_lLookAroundCol"),			&m_lLookAroundCol);
		RegVariable(_T("WPR_ucPrescanDiePrID"),			&m_ucPrescanDiePrID);
		RegVariable(_T("WPR_ucMapDummyScanVerify"),		&m_ucScanAlignVerify);
		RegVariable(_T("WPR_bFovToFindGT"),				&m_bFovToFindGT);
		RegVariable(_T("WPR_bFovFindGTMove"),			&m_bFovFindGTMove);
		RegVariable(_T("WPR_szWT1YieldState"),			&m_szWT1YieldState);
		RegVariable(_T("WPR_szWT2YieldState"),			&m_szWT2YieldState);
		RegVariable(_T("WPR_dScanAcceptAngle"),			&m_dScanAcceptAngle);
		RegVariable(_T("WPR_dRescanGoodRatio"),			&m_dRescanGoodRatio);

		RegVariable(_T("WPR_bPrescanNoTipDieMerge"),	&m_bPrescanNoTipDieMerge);
		RegVariable(_T("WPR_bPrescanGoodAction"),		&m_bPrescanGoodAction);
		RegVariable(_T("WPR_bPrescanDefectAction"),		&m_bPrescanDefectAction);
		RegVariable(_T("WPR_bPrescanEmptyAction"),		&m_bPrescanEmptyAction);
		RegVariable(_T("WPR_bPrescanBadCutAction"),		&m_bPrescanBadCutAction);
		RegVariable(_T("WPR_bPrescanSkipNgGrade"),		&m_bPrescanSkipNgGrade);
		RegVariable(_T("WPR_bScanRunTimeDisplay"),		&m_bScanRunTimeDisplay);
		RegVariable(_T("WPR_bBinMapShow"),				&m_bBinMapShow);

		RegVariable(_T("WPR_bScanExtraDieAction"),		&m_bScanExtraDieAction);
		RegVariable(_T("WPR_bNgPickExtra"),				&m_bNgPickExtra);
		RegVariable(_T("WPR_ucScanExtraGrade"),			&m_ucScanExtraGrade);
		RegVariable(_T("WPR_bScanDetectFakeEmpty"),		&m_bScanDetectFakeEmpty);
		RegVariable(_T("WPR_ucScanFakeEmptyGrade"),		&m_ucScanFakeEmptyGrade);

		RegVariable(_T("WPR_bSummaryOnlyNewGrade"),		&m_bSummaryOnlyNewGrade);

		RegVariable(_T("WPR_ucPrescanGoodGrade"),		&m_ucPrescanGoodGrade);
		RegVariable(_T("WPR_ucPrescanDefectGrade"),		&m_ucPrescanDefectGrade);
		RegVariable(_T("WPR_ucPrescanEmptyGrade"),		&m_ucPrescanEmptyGrade);
		RegVariable(_T("WPR_ucPrescanBadCutGrade"),		&m_ucPrescanBadCutGrade);
		RegVariable(_T("WPR_lMapNgPassScore"),			&m_lMapNgPassScore);
		RegVariable(_T("WPR_ucPrescanMapNgGrade"),		&m_ucScanMapStartNgGrade);
		RegVariable(_T("WPR_ucScanMapEndNgGrade"),		&m_ucScanMapEndNgGrade);
		RegVariable(_T("WPR_ucPrescanEdgeSize"),		&m_ucPrescanEdgeSizeX);
		RegVariable(_T("WPR_ucPrescanEdgeSizeY"),		&m_ucPrescanEdgeSizeY);
		RegVariable(_T("WPR_ucPrescanEdgeGrade"),		&m_ucPrescanEdgeGrade);
		RegVariable(_T("WPR_dEdgeGoodScore"),			&m_dEdgeGoodScore);
		RegVariable(_T("WPR_ucPrescanMapAOINgGrade"),	&m_ucScanMapStartAOINgGrade);
		RegVariable(_T("WPR_ucScanMapEndAOINgGrade"),	&m_ucScanMapEndAOINgGrade);
		RegVariable(_T("WPR_bPrescanSkipAOINgGrade"),	&m_bPrescanSkipAOINgGrade);
		RegVariable(_T("WPR_szPrescanResult"),			&m_szScanResult);
		RegVariable(_T("WPR_lScnZoomFactor"),			&m_lScnZoomFactor);

		RegVariable(_T("PBT_ucMapEdgeSize_inDailyCheck"),		&m_ucMapEdgeSize_inDailyCheck);

		RegVariable(_T("WPR_bFastHomeScanEnable"),		&m_bFastHomeScanEnable);
		RegVariable(_T("WPR_bIMPrescanReferDie"),		&m_bIMPrescanReferDie);
		RegVariable(_T("WPR_bPrescanDefectToNullBin"),	&m_bPrescanDefectToNullBin);
		RegVariable(_T("WPR_bPrescanEmptyToNullBin"),	&m_bPrescanEmptyToNullBin);
		RegVariable(_T("WPR_bPrescanBadCutToNullBin"),	&m_bPrescanBadCutToNullBin);
		RegVariable(_T("WPR_bPrescanDefectMarkUnPick"),	&m_bPrescanDefectMarkUnPick);
		RegVariable(_T("WPR_bPrescanEmptyMarkUnPick"),	&m_bPrescanEmptyMarkUnPick);
		RegVariable(_T("WPR_bPrescanBadCutMarkUnPick"),	&m_bPrescanBadCutMarkUnPick);
		RegVariable(_T("WPR_bPrescanPrCheckReferDie"),	&m_bPrescanPrCheckReferDie);
		RegVariable(_T("WPR_bPrescanKeepPsmUnPickDie"),	&m_bPrescanKeepPsmUnPickDie);
		RegVariable(_T("WPR_bShowNgDieSubGrade"),		&m_bShowNgDieSubGrade);
		RegVariable(_T("WPR_bSaveNgDieSubGrade"),		&m_bSaveNgDieSubGrade);
		RegVariable(_T("WPR_bAccurateRescan"),			&m_bAccurateRescan);
		RegVariable(_T("WPR_bAutoRefillRescan"),		&m_bAutoRefillRescan);

		RegVariable(_T("WPR_ulHoleSkipPointLimit"),		&m_ulHoleSkipPointLimit);

		RegVariable(_T("WPR_dPrescanPitchTolX"),		&m_dPrescanPitchTolX);
		RegVariable(_T("WPR_dPrescanPitchTolY"),		&m_dPrescanPitchTolY);

		RegVariable(_T("WPR_bHomeDieUniqueCheck"),		&m_bHomeDieUniqueCheck);
		RegVariable(_T("WPR_bScanAlignFromFile"),		&m_bScanAlignFromFile);

		RegVariable(_T("WPR_bScanCheckPointUL"),		&m_bScanCheckPoints[0]);
		RegVariable(_T("WPR_bScanCheckPointUC"),		&m_bScanCheckPoints[1]);
		RegVariable(_T("WPR_bScanCheckPointUR"),		&m_bScanCheckPoints[2]);
		RegVariable(_T("WPR_bScanCheckPointCL"),		&m_bScanCheckPoints[3]);
		RegVariable(_T("WPR_bScanCheckPointCC"),		&m_bScanCheckPoints[4]);
		RegVariable(_T("WPR_bScanCheckPointCR"),		&m_bScanCheckPoints[5]);
		RegVariable(_T("WPR_bScanCheckPointDL"),		&m_bScanCheckPoints[6]);
		RegVariable(_T("WPR_bScanCheckPointDC"),		&m_bScanCheckPoints[7]);
		RegVariable(_T("WPR_bScanCheckPointDR"),		&m_bScanCheckPoints[8]);

		RegVariable(_T("WPR_ucScanCheckMapOnWafer"),		&m_ucScanCheckMapOnWafer);
		RegVariable(_T("WPR_bScanAlignAutoFromMap"),		&m_bScanAlignAutoFromMap);
		RegVariable(_T("WPR_lScanAlignTotalHolesMin"),		&m_lScanAlignTotalHolesMin);
		RegVariable(_T("WPR_lScanAlignPatternHolesMin"),	&m_lScanAlignPatternHolesMin);
		RegVariable(_T("WPR_dSAMatchPercentHoles"),			&m_dScanAlignMatchLowPercent);
		RegVariable(_T("WPR_lScanAlignPatternHolesMax"),	&m_lScanAlignPatternHolesMax);
		RegVariable(_T("WPR_dSAMatchPercentPatterns"),		&m_dSAMatchPatternsPercent);

		RegVariable(_T("WPR_lMinDieDistX"),	&m_lMinDieDistX);
		RegVariable(_T("WPR_lMinDieDistY"),	&m_lMinDieDistY);

		RegVariable(_T("WPR_bRunPartialDie"),			&m_bRunPartialDie);
		RegVariable(_T("WPR_lLCMoveDistance"),			&m_lLCMoveDistance);
		RegVariable(_T("WPR_lLCMoveDelay"),				&m_lLCMoveDelay);
		RegVariable(_T("WPR_lLCMoveTotalLimit"),		&m_lLCMoveTotalLimit);
		RegVariable(_T("WPR_bLCUseBondCam"),			&m_bLCUseBondCam);
		RegVariable(_T("WPR_dScanAlignMPassScore"),		&m_dScanAlignMPassScore);
		RegVariable(_T("WPR_bNoWprBurnInCP"),			&m_bNoWprBurnIn);
		RegVariable(_T("WPR_bAoiEnableOcrDie"),			&m_bAoiEnableOcrDie);
		RegVariable(_T("WPR_bAoiOcrDieLearnt"),			&m_bAoiOcrDieLearnt);
		RegVariable(_T("WPR_bUseAoiGradeConvert"),		&m_bUseAoiGradeConvert);
		RegVariable(_T("WPR_bScanNgGradeConvert"),		&m_bScanNgGradeConvert);
		RegVariable(_T("WPR_bOCRBarwaferNoMap"),		&m_bOCRBarwaferNoMap);
		RegVariable(_T("WPR_szAoiOcrPrValue"),			&m_szAoiOcrPrValue);
		RegVariable(_T("WPR_szAoiLearnOcrValue"),		&m_szAoiLearnOcrValue);
		RegVariable(_T("WPR_szContourImagePath"),		&m_szContourImagePath);
		RegVariable(_T("WPR_lAoiOcrBoxHeight"),			&m_lAoiOcrBoxHeight);
		RegVariable(_T("WPR_lAoiOcrBoxWidth"),			&m_lAoiOcrBoxWidth);

		// contour camera
		RegVariable(_T("WPR_bWL1Calibrated"),	&m_bWL1Calibrated);
		RegVariable(_T("WPR_dWL1CalibXX"),		&m_dWL1CalibXX);
		RegVariable(_T("WPR_dWL1CalibXY"),		&m_dWL1CalibXY);
		RegVariable(_T("WPR_dWL1CalibYY"),		&m_dWL1CalibYY);
		RegVariable(_T("WPR_dWL1CalibYX"),		&m_dWL1CalibYX);

		RegVariable(_T("WPR_bWL2Calibrated"),	&m_bWL2Calibrated);
		RegVariable(_T("WPR_dWL2CalibXX"),		&m_dWL2CalibXX);
		RegVariable(_T("WPR_dWL2CalibXY"),		&m_dWL2CalibXY);
		RegVariable(_T("WPR_dWL2CalibYY"),		&m_dWL2CalibYY);
		RegVariable(_T("WPR_dWL2CalibYX"),		&m_dWL2CalibYX);

		RegVariable(_T("WPR_lWL1WaferOffsetX"),	&m_lWL1WaferOffsetX);
		RegVariable(_T("WPR_lWL1WaferOffsetY"),	&m_lWL1WaferOffsetY);
		RegVariable(_T("WPR_lWL2WaferOffsetX"),	&m_lWL2WaferOffsetX);
		RegVariable(_T("WPR_lWL2WaferOffsetY"),	&m_lWL2WaferOffsetY);
		// CPCheckOpticDistortion
		RegVariable(_T("WPR_dDieThicknessInput"),	&m_dDieThicknessInput);

		RegVariable(_T("WPR_bPRFailureCaseLog"),	&CPRFailureCaseLog::Instance()->m_bFailureCaselogEnable);
		RegVariable(_T("WPR_bBHMarkCaseLog"),		&CPRFailureCaseLog::Instance()->m_bBHMarkCaselogEnable);
	}

	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

VOID CWaferPr::InitPrescanVariables()
{
	m_lMapCtrCol			= 90;
	m_lOcrBarIndex			= 0;
	m_lOcrBarCurrRow		= 0;
	m_lOcrBarCurrMapCol		= 0;
	m_lOcrBarCurrScnCol		= 0;
	m_lOcrImageRotation		= 0;
	m_bOcrBarRightSide		= FALSE;
	m_bOcrConfirmControl	= FALSE;
	m_ucOcrBarMapStage		= 0;
	m_bOcrConfirmButton		= FALSE;
	m_szBarFirstDieImage	= BAR_OCR_NO_IMG_PATH;
	m_szBarLastDieImage		= BAR_OCR_NO_IMG_PATH;
	memset(&m_stOcrViewWindow, 0, sizeof(m_stOcrViewWindow));
	m_siBarPitchX	= 0;
	m_siBarPitchY	= 0;
	m_lOcrPassScore				= 85;
	for(int i=0; i<PAGE_BAR_MAX; i++)
	{
		m_szOcrBarName[i].Format("Bar %02d", i+1);
		m_szOcrValueLFBar[i].Format("LF %02d", i+1);
		m_szOcrValueLLBar[i].Format("LL %02d", 50+i+1);
		m_szOcrValueRFBar[i].Format("RF %02d", i+1);
		m_szOcrValueRLBar[i].Format("RL %02d", 50+i+1);
		m_bOcrResultBarL[i]	= FALSE;
		m_bOcrResultBarR[i]	= FALSE;
	}
	m_bScanFrameRangeSent	= FALSE;
	m_uwFrameSubImageNum	= 1;
	m_lUsedSrchThreadNum	= MAX_PR_GALLERY_SEARCH_THREADS;
	m_lWaitingImageLimit	= 0;
	m_dPrescanLFSizeX		= 0;
	m_dPrescanLFSizeY		= 0;
	// Prescan die variable init value
	// now create the controller thread
	m_dPrescanPitchTolX		= 30;
	m_dPrescanPitchTolY		= 30;
	m_nDieSizeX				= 0;
	m_nDieSizeY				= 0;
	m_lPrescanSrchDieScore		= 70;
	m_bAoiPrescanInspction		= TRUE;
	m_bSendWaferIDToPR			= FALSE;
	m_ucCornerAlignDieState		= 0;	//	init
	m_lPrescanIndexRowReduce	= 0;
	m_lPrescanIndexColReduce	= 0;
	m_bKeepScan2ndPrDie			= FALSE;
	m_lSelect2ndPrID			= 0;
	m_bEnable2ndPrSearch		= FALSE;
	m_bFovToFindGT				= FALSE;
	m_bFovFindGTMove			= FALSE;
	m_bFovFoundWaferAngle		= FALSE;
	m_lPrescanImageCount	= 0;
	m_lScanIndexStopCount	= 0;
	m_lScanPrAbnormalCount	= 0;
	m_dScanPrCleanTime		= 0.0;
	m_lPrescanImageLimit	= 2000;
#ifdef	VS_5MCAM
	m_lPrescanImageLimit	= 150;
#endif
	m_lExtraPrescanOverlapX	= 0;
	m_lExtraPrescanOverlapY	= 0;
	m_lLookAroundRow		= 0;
	m_lLookAroundCol		= 0;
	m_ucPrescanDiePrID		= 1;
	m_ucScanAlignVerify		= 0;
	m_bPrescanNoTipDieMerge	= FALSE;
	m_bPrescanGoodAction	= FALSE;
	m_bPrescanDefectAction	= FALSE;
	m_bPrescanEmptyAction	= FALSE;
	m_bPrescanBadCutAction	= FALSE;
	m_bPrescanSkipNgGrade	= FALSE;
	m_bScanRunTimeDisplay	= FALSE;
	m_bBinMapShow			= FALSE;
	m_dScanAcceptAngle		= 0;
	m_dRescanGoodRatio		= 0;

	m_bSummaryOnlyNewGrade	= FALSE;
	m_lBackLightTravelTime 	= 5000;	// between up and standby
	m_dBackLightZDownTime 	= 0;

	m_ucPrescanGoodGrade	= 91;
	m_ucPrescanDefectGrade	= 92;
	m_ucPrescanEmptyGrade	= 93;
	m_ucPrescanBadCutGrade	= 94;
	m_bNgPickExtra			= FALSE;
	m_bScanExtraDieAction	= FALSE;
	m_ucScanExtraGrade		= 95;
	m_bScanDetectFakeEmpty	= FALSE;
	m_ucScanFakeEmptyGrade	= 96;
	m_lMapNgPassScore		= 0;
	m_ucScanMapStartNgGrade	= 100;
	m_ucScanMapEndNgGrade	= 100;
	m_ucScanMapNgListSize	= 0;
	memset(m_ucScanMapNgList, 0, 255);
	m_ucPrescanEdgeSize		= 0;
	m_ucPrescanEdgeSizeX	= 0;
	m_ucPrescanEdgeSizeY	= 0;
	m_ucPrescanEdgeGrade	= 0;
	m_dEdgeGoodScore		= 0.0;
	m_ulEdgeGoodDieTotal	= 0;
	m_ucMapEdgeSize_inDailyCheck= 0;
	// AOI NG Grade
	m_ucScanMapStartAOINgGrade	= 100;
	m_ucScanMapEndAOINgGrade	= 100;

	m_uwIMMergeNewRecordID		= 0;
	for(int i=0; i<WPR_MAX_DIE; i++)
		m_uwIMMergeRecordID[i]	= 0;
	m_uwScanRecordID[0] = 0;
	m_uwScanRecordID[1] = 0;
	m_bIMPrescanReferDie		= FALSE;
	m_bFastHomeScanEnable		= FALSE;
	m_bPrescanDefectToNullBin	= FALSE;
	m_bPrescanEmptyToNullBin	= FALSE;
	m_bPrescanBadCutToNullBin	= FALSE;
	m_bPrescanDefectMarkUnPick	= FALSE;
	m_bPrescanEmptyMarkUnPick	= TRUE;
	m_bPrescanBadCutMarkUnPick	= FALSE;
	m_bPrescanKeepPsmUnPickDie	= FALSE;

	m_bAccurateRescan			= FALSE;
	m_bAutoRefillRescan			= FALSE;
	m_bPrescanPrCheckReferDie	= TRUE;
	m_bWprInAutoSortingMode		= FALSE;
	m_bHomeDieUniqueCheck		= FALSE;
	m_bHomeDieUniqueResult		= TRUE;
	m_bScanAlignFromFile		= FALSE;
	m_bScanAlignAutoFromMap		= FALSE;
	memset(m_bScanCheckPoints, FALSE, sizeof(m_bScanCheckPoints));
	m_ucScanCheckMapOnWafer		= 0;
	m_lScanAlignTotalHolesMin	= 0;
	m_lScanAlignPatternHolesMin	= 0;
	m_lScanAlignPatternHolesMax	= 0;
	m_dSAMatchPatternsPercent	= 0;
	m_dScanAlignMatchLowPercent	= 0;
	m_ulFoundHoleCounter		= 0;
	for(INT i=0; i<SA_HOLES_MAX; i++)
	{
		m_astHoleList[i].m_awHoleRow.RemoveAll();
		m_astHoleList[i].m_awHoleCol.RemoveAll();
		m_astHoleList[i].m_ucHoleNul.RemoveAll();
	}
	m_ulScanAlignTotalPoints	= 0;
	m_ulHoleSkipPointLimit		= 0;
	m_awPointsListCol.RemoveAll();
	m_awPointsListRow.RemoveAll();
	m_ucPointsListSta.RemoveAll();

	m_lMinDieDistX		= 0;
	m_lMinDieDistY		= 0;
	m_bRunPartialDie	= FALSE;
	m_lImageStichExtraDelay	= 0;
	m_lLCMoveTotalLimit	= 1;
	m_lLCMoveDistance	= 2000;
	m_lLCMoveDelay		= 200;
	m_bLCUseBondCam		= FALSE;
	m_dPrDieCenterX		= 0;
	m_dPrDieCenterY		= 0;

	m_lAoiOcrBoxHeight	= 0;
	m_lAoiOcrBoxWidth	= 0;
	m_bOCRBarwaferNoMap		= FALSE;
	m_bAoiOcrDieLearnt		= FALSE;
	m_bUseAoiGradeConvert	= FALSE;
	m_bScanNgGradeConvert	= FALSE;
	m_lGradeMappingLimit	= 0;
	memset(m_sMapOrgGrade,		0, sizeof(m_sMapOrgGrade));
	memset(m_sPrInspectGrade,	0, sizeof(m_sPrInspectGrade));
	memset(m_sMapNewGrade,		0, sizeof(m_sMapNewGrade));
	m_bShowNgDieSubGrade	= FALSE;
	m_bSaveNgDieSubGrade	= FALSE;

	// contour camera
	m_lWL1CoaxLightLevel	= 0;
	m_lWL1RingLightLevel	= 0;
	m_lWL1SideLightLevel	= 0;
	m_lWL1BackLightLevel	= 0;
	m_lWL2CoaxLightLevel	= 0;
	m_lWL2RingLightLevel	= 0;
	m_lWL2SideLightLevel	= 0;
	m_lWL2BackLightLevel	= 0;
	memset(&m_stCalibCornerPos, 0, sizeof(m_stCalibCornerPos));
	PR_WORD lHalfWidth	= PR_MAX_COORD * 3 / 16;
	PR_WORD lHalfHeight	= PR_MAX_COORD * 3 / 16;
	m_stCalibCornerPos[PR_UPPER_LEFT].x	 = (PR_DEF_CENTRE_X - lHalfWidth);
	m_stCalibCornerPos[PR_UPPER_LEFT].y	 = (PR_DEF_CENTRE_Y - lHalfHeight);
	m_stCalibCornerPos[PR_LOWER_RIGHT].x = (PR_DEF_CENTRE_X + lHalfWidth);
	m_stCalibCornerPos[PR_LOWER_RIGHT].y = (PR_DEF_CENTRE_Y + lHalfHeight);

	m_lContourULX			= 0;
	m_lContourULY			= 0;
	m_lContourLRX			= 0;
	m_lContourLRY			= 0;

	m_bWL1Calibrated		= FALSE;
	m_dWL1CalibXX			= 0;
	m_dWL1CalibXY			= 0;
	m_dWL1CalibYY			= 0;
	m_dWL1CalibYX			= 0;

	m_bWL2Calibrated		= FALSE;
	m_dWL2CalibXX			= 0;
	m_dWL2CalibXY			= 0;
	m_dWL2CalibYY			= 0;
	m_dWL2CalibYX			= 0;

	m_lWL1WaferOffsetX		= 0 - 407*2000;
	m_lWL1WaferOffsetY		= 0;

	m_lWL2WaferOffsetX		= 487*2000;
	m_lWL2WaferOffsetY		= 0;
	// contour camera

	m_szAoiLearnOcrValue = "0";
	memset(&m_stOcrDetectWin, 0, sizeof(m_stOcrDetectWin));

	m_dScanAlignMPassScore	= 0.0;

	m_bNoWprBurnIn	= IsAOIOnlyMachine();

	m_szScanResult.Empty();

	m_pPrescanPrCtrl = dynamic_cast<CGallerySearchController<CWaferPr>*>(AfxBeginThread(RUNTIME_CLASS(CGallerySearchController<CWaferPr>)));
	m_pPrescanPrCtrl->SetPRStation(this);

	m_szContourImagePath = "";
	m_dwaRsnBaseRow.RemoveAll();
	m_dwaRsnBaseCol.RemoveAll();
	m_dwaRsnBaseWfX.RemoveAll();
	m_dwaRsnBaseWfY.RemoveAll();
	m_lPrescanSortingTotal	= 0; 
	m_lRescanMissingTotal	= 0;
}

BOOL CWaferPr::IsPrescanEndToPickNg()
{
	return IsPrescanEnded() && IsScanNgPick();
}

LONG CWaferPr::PrescanDieResult(IPC_CServiceMessage &svMsg)
{
	ULONG ulRow = 0, ulCol = 0;
	LONG encX, encY;

	double dDieAngle;
	CString szDieBin = "0";
	BOOL	bShowAoi = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bShowAoi);

	if( IsPrescanEnded() )
	{
		m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);

		USHORT usDieState = 0;
		if( GetScanInfo(ulRow, ulCol, encX, encY, dDieAngle, szDieBin, usDieState) )
		{
			BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
			DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);
			if (bIsBadCut)
			{
				m_szScanResult = "BadCut";
			}
			else if( bIsDefect )
			{
				m_szScanResult = "Defect";
			}
			else if( bIsFakeEmpty )
			{
				m_szScanResult = "BU ALN";
			}
			else
			{
				m_szScanResult = "Good";
			}
			if( IsPrecanWith2Pr() && m_bKeepScan2ndPrDie )
			{
				CString szTemp;
				UCHAR ucRecordID = (UCHAR)GetScanMapRecordID(ulRow, ulCol);
				szTemp.Format(":%d", ucRecordID);
				m_szScanResult += szTemp;
			}
		}
		else
		{
			m_szScanResult = "Empty";
		}
	}
	else
	{
		m_szScanResult = "Unscan";
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::MultiSearchInit(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=TRUE;
	if( IsThisDieLearnt(GetPrescanPrID()-1)!=TRUE )
	{
		CString szMsg;
		szMsg.Format("Normal die PR %d not learnt yet!", GetPrescanPrID());
		HmiMessage_Red_Back(szMsg);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	OpPrescanInit(GetPrescanPrID());	// hmi multi search test button

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::CP100MultiSearchDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	if( IsThisDieLearnt(GetPrescanPrID()-1)!=TRUE )
	{
		CString szMsg;
		szMsg.Format("Normal die PR %d not learnt yet!", GetPrescanPrID());
		HmiMessage_Red_Back(szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	OpPrescanInit(GetPrescanPrID());	//	CP hmi multi search test button
	LONG lLoopCounter = 1;
	while( 1 )
	{
		CString szOpId	= _T("1");
		CString szTitle		= "Please input loop test counter: ";
		BOOL bReturn = HmiStrInputKeyboard(szTitle, szOpId);
		if( bReturn && szOpId.IsEmpty()!=TRUE )
		{
			lLoopCounter = atoi(szOpId);
			break;
		}
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMS896AApp::m_bStopAlign = FALSE;
	LONG lPrDelay = 100;
	svMsg.GetMsg(sizeof(LONG), &lPrDelay);

	int lX = 0, lY=0, lT = 0;
	GetWaferTableEncoder(&lX, &lY, &lT);
	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lY;

for(int j=0; j<lLoopCounter; j++)
{
	DOUBLE dTime = GetTime();
	CDWordArray dwList;
	for(int i=0; i<5; i++)
	{
		dwList.SetSize(i+1);
		dwList.SetAt(i, 0);
	}
	ClearGoodInfo();

	PrescanAutoMultiGrabDone(dwList, FALSE);
	if( pApp->IsStopAlign() )
	{
		break;
	}
	LONG lTime = lPrDelay - (LONG)(GetTime() - dTime);
	if( lTime>0 )
	{
		Sleep(lTime);
	}
}
	HmiMessage("Multi search done!");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::MultiSearchDie(IPC_CServiceMessage &svMsg)
{
	MULTI_SRCH_RESULT	stMsch;

	BOOL bDrawDie = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bDrawDie);
	if( bDrawDie==TRUE )
	{
		OpenWaitingAlert();
		m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	}
	int i;
	CDWordArray dwList;
	for(i=0; i<5; i++)
	{
		dwList.SetSize(i+1);
		dwList.SetAt(i, 0);
		stMsch.bDieState[i] = 0;
	}

	int lX = 0, lY=0, lT = 0;
	GetWaferTableEncoder(&lX, &lY, &lT);
	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lY;
	ClearGoodInfo();

	PrescanAutoMultiGrabDone(dwList, bDrawDie, FALSE, TRUE);

	for(i=0; i<5; i++)
	{
		stMsch.bDieState[i] = dwList.GetAt(i);
	}

	if( bDrawDie==TRUE )
	{
		CloseWaitingAlert();
	}

	svMsg.InitMessage(sizeof(MULTI_SRCH_RESULT), &stMsch);

	return 1;
}

VOID CWaferPr::SavePrescanDataPr(CStringMapFile  *psmf)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( m_bScanAlignAutoFromMap && m_bScanAlignFromFile )
	{
		m_bScanAlignAutoFromMap = m_bScanAlignFromFile = FALSE;
	}

	if( IsScanAlignWafer() )
	{
		m_ucScanCheckMapOnWafer = 0;
	}

	if( IsStitchMode() )
	{
		m_lExtraPrescanOverlapX = labs(m_lExtraPrescanOverlapX);
		m_lExtraPrescanOverlapY = labs(m_lExtraPrescanOverlapY);

		if(	m_ucScanWalkTour!=WT_SCAN_WALK_LEFT_VERT && 
			m_ucScanWalkTour!=WT_SCAN_WALK_TOP_HORI)
		{
			m_ucScanWalkTour = WT_SCAN_WALK_TOP_HORI;
		}
	}

	if( m_bUseAoiGradeConvert )
	{
		m_bSaveNgDieSubGrade = FALSE;
		m_bScanNgGradeConvert	= FALSE;
	}

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_MAP_INDEX_REDUCE_ROW]			= m_lPrescanIndexRowReduce;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_MAP_INDEX_REDUCE_COL]			= m_lPrescanIndexColReduce;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_WITH_TWO_PR_RECORDS]		= m_bEnable2ndPrSearch ;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_2ND_PR_DIE_TO_KEEP]			= m_bKeepScan2ndPrDie;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_USE_19_AS_2NDPR_ID]			= m_lSelect2ndPrID;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_NDIE_PRESCAN_INSPECTION]		= m_bAoiPrescanInspction;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_SEND_WAFERID_TO_PR]				= m_bSendWaferIDToPR;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_ES100_SRCH_THREAD_NUM]			= m_lUsedSrchThreadNum;
	m_lWaitingImageLimit = min(m_lWaitingImageLimit, m_lPrescanImageLimit/2);
	m_lWaitingImageLimit = m_pPrescanPrCtrl->SetGrabImageLimit(m_lWaitingImageLimit);
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_GRAB_IMAGE_LIMIT]			= m_lWaitingImageLimit;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_SRH_NDIE_PASSSCORE][1]	= m_lPrescanSrchDieScore;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_DIE_PR_ID]				= GetPrescanPrID();
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_MAP_DUMMY_SCAN_RESULT_VERIFY]	= m_ucScanAlignVerify;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_EXTRA_OVERLAPX]			= m_lExtraPrescanOverlapX;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_EXTRA_OVERLAPY]			= m_lExtraPrescanOverlapY;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_HOME_LOOK_AROUND_ROW]			= m_lLookAroundRow;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_HOME_LOOK_AROUND_COL]			= m_lLookAroundCol;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_HOME_UNIQUE_CHECK]			= m_bHomeDieUniqueCheck;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_FOV_FIND_WAFER_ANGLE]			= m_bFovToFindGT;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_FOV_FIND_WAFER_ANGLE_MOVE]		= m_bFovFindGTMove;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_CHECK_MAP_HOLES_WITH_WAFER]		= m_ucScanCheckMapOnWafer;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_ALIGN_FROM_FILE]			= m_bScanAlignFromFile;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_HOLE_AUTO_FROM_MAP]	= m_bScanAlignAutoFromMap;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_TOTAL_HOLES_MIN]		= m_lScanAlignTotalHolesMin;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_PATTERN_HOLES_MIN]	= m_lScanAlignPatternHolesMin;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_PATTERN_HOLES_MAX]	= m_lScanAlignPatternHolesMax;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_MATCH_PATTERNS_PERCENT]	= m_dSAMatchPatternsPercent;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_MATCH_LOW_PERCENT]	= m_dScanAlignMatchLowPercent;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_RUN_PARTIAL_DIE_PR]			= m_bRunPartialDie;
	(*psmf)[WPR_SRH_NDIE_DATA]["min die distance X for badcut die"]	= m_lMinDieDistX;
	(*psmf)[WPR_SRH_NDIE_DATA]["min die distance Y for badcut die"]	= m_lMinDieDistY;	

	(*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_POINTS_NUM]		= m_ulScanAlignTotalPoints;
	(*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_HOLE_SKIP_LIMIT]	= m_ulHoleSkipPointLimit;
	for (ULONG i = 0; i < m_ulScanAlignTotalPoints; i++)
	{
		if (i >= (ULONG)m_awPointsListRow.GetSize())
		{
			break;
		}
		LONG lRow = (LONG)m_awPointsListRow.GetAt(i);
		LONG lCol = (LONG)m_awPointsListCol.GetAt(i);
		BYTE ucSta= m_ucPointsListSta.GetAt(i);
		(*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_HOLE_ROW][i]	= lRow;
		(*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_HOLE_COL][i]	= lCol;
		(*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_HOLE_STA][i]	= ucSta;
	}

	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_NO_ORIENTATION_DIE_MERGE]	= m_bPrescanNoTipDieMerge;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_HAVE_GOOD_DIE_ACTION]		= m_bPrescanGoodAction;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_HAVE_DEFECT_DIE_ACTION]	= m_bPrescanDefectAction;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_HAVE_EMPTY_DIE_ACTION]		= m_bPrescanEmptyAction;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_HAVE_BAD_CUT_DIE_ACTION]	= m_bPrescanBadCutAction;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_EXTRA_NG_PICK]		= m_bNgPickExtra;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_EXTRA_DIE_ACTION]		= m_bScanExtraDieAction;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_EXTRA_DIE_GRADE]		= m_ucScanExtraGrade;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_FAKE_EMPTY_ACTION]	= m_bScanDetectFakeEmpty;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_FAKE_EMPTY_GRADE]		= m_ucScanFakeEmptyGrade;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SKIP_MAP_NG_GRADE_DIE]		= m_bPrescanSkipNgGrade;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_RUN_TIME_DISPLAY]		= m_bScanRunTimeDisplay;

	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SUMMARY_ONLY_NEW_GRADE]	= m_bSummaryOnlyNewGrade;

	if( m_ucPrescanGoodGrade==0 )
		m_ucPrescanGoodGrade = 91;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_GOOD_DIE_GRADE]			= m_ucPrescanGoodGrade;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_DEFECT_DIE_GRADE]			= m_ucPrescanDefectGrade;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_EMPTY_DIE_GRADE]			= m_ucPrescanEmptyGrade;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_BAD_CUT_DIE_GRADE]			= m_ucPrescanBadCutGrade;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_MAP_NG_PASS_SCORE]			= m_lMapNgPassScore;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_MAP_NG_GRADE_VALUE]		= (LONG) m_ucScanMapStartNgGrade;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_MAP_END_NG_GRADE_VALUE]	= (LONG) m_ucScanMapEndNgGrade;
	// AOI NG Grade
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_MAP_AOI_NG_GRADE_VALUE]	= (LONG)m_ucScanMapStartAOINgGrade;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_MAP_END_AOI_NG_GRADE_VALUE]= (LONG)m_ucScanMapEndAOINgGrade;

	//if( m_ucScanMapNgListSize>255 )		//Klocwork	//v4.46
	//	m_ucScanMapNgListSize = 255;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_NG_GRADE_LIST_SIZE]			= (LONG) m_ucScanMapNgListSize;

	for (LONG i = 0; i < 255; i++)
	{
		(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_NG_GRADE_LIST][i + 1]		= (LONG) m_ucScanMapNgList[i];
	}
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_MAP_EDGE_SIZE]		= (LONG) m_ucPrescanEdgeSizeX;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_MAP_EDGE_SIZE_Y]	= (LONG) m_ucPrescanEdgeSizeY;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_MAP_EDGE_GRADE]	= (LONG)m_ucPrescanEdgeGrade;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_MAP_EDGE_GOODSCORE]= m_dEdgeGoodScore;
	m_ucPrescanEdgeSize		= max(m_ucPrescanEdgeSizeX, m_ucPrescanEdgeSizeY);

	(*psmf)[WPR_DEFECT_SCAN_SETTING][PB_MAP_EDGE_SIZE_IN_DAILY_CHECK]	= m_ucMapEdgeSize_inDailyCheck;

	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_IM_FASTHOME_SCAN_DIE]		= m_bFastHomeScanEnable;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_IM_PRESCAN_REFER_DIE]		= m_bIMPrescanReferDie;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_DEFECT_DIE_TO_NULL_BIN]	= m_bPrescanDefectToNullBin;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_EMPTY_DIE_TO_NULL_BIN]		= m_bPrescanEmptyToNullBin;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_BAD_CUT_DIE_TO_NULL_BIN]	= m_bPrescanBadCutToNullBin;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_DEFECT_DIE_MARK_UNPICK]	= m_bPrescanDefectMarkUnPick;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_SHOW_NG_DIE_SUB_GRADE]			= m_bShowNgDieSubGrade;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_SAVE_NG_DIE_SUB_GRADE]			= m_bSaveNgDieSubGrade;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetFeatureStatus(MS896A_FUNC_PRESCAN_EMPTY_UNMARK)!=TRUE && IsSorraSortMode()!=TRUE )
		m_bPrescanEmptyMarkUnPick = !m_bPrescanEmptyAction;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_EMPTY_DIE_MARK_UNPICK]		= m_bPrescanEmptyMarkUnPick;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_BAD_CUT_DIE_MARK_UNPICK]	= m_bPrescanBadCutMarkUnPick;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_KEEP_PSM_UNPICK]	= m_bPrescanKeepPsmUnPickDie;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PR_CHECK_REFER_DIE]		= m_bPrescanPrCheckReferDie;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_ACCURATE_RESCAN]			= m_bAccurateRescan;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_AUTO_REFILL_RESCAN]			= m_bAutoRefillRescan;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_RESCAN_GOOD_RATIO]				= m_dRescanGoodRatio;

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_REFSCAN_MERGE_ID]				= (LONG)m_uwIMMergeNewRecordID;
	for (LONG i = 0; i < WPR_MAX_DIE; i++)
	{
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_REFSCAN_MERGE_ELEMENTS][i + 1]	= (LONG)m_uwIMMergeRecordID[i];
	}

	if( m_dPrescanPitchTolX<10.0 )
		m_dPrescanPitchTolX = 30.0;
	if( m_dPrescanPitchTolY<10.0 )
		m_dPrescanPitchTolY = 30.0;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_PITCH_TOL_X]		= m_dPrescanPitchTolX;
	(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_PITCH_TOL_Y]		= m_dPrescanPitchTolY;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_PSCAN_SCAN_ALIGN_PASSSCORE]		= m_dScanAlignMPassScore;
	for(int i=0; i<WPR_OCR_MAX_WINDOWS; i++)
	{
		CString szTemp;
		szTemp.Format("Window%d_Corner1_X", i+1);
		(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS][szTemp]	= m_stOcrDetectWin[i].coCorner1.x;
		szTemp.Format("Window%d_Corner1_Y", i+1);
		(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS][szTemp]	= m_stOcrDetectWin[i].coCorner1.y;
		szTemp.Format("Window%d_Corner2_X", i+1);
		(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS][szTemp]	= m_stOcrDetectWin[i].coCorner2.x;
		szTemp.Format("Window%d_Corner2_Y", i+1);
		(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS][szTemp]	= m_stOcrDetectWin[i].coCorner2.y;
	}
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_SRH_OCR_PASSSCORE]			= m_lOcrPassScore;
	CheckOcrViewWindow();
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["ViewULX"]	= m_stOcrViewWindow.coCorner1.x;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["ViewULY"]	= m_stOcrViewWindow.coCorner1.y;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["ViewLRX"]	= m_stOcrViewWindow.coCorner2.x;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["ViewLRY"]	= m_stOcrViewWindow.coCorner2.y;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["BarPitchX"]	= m_siBarPitchX;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["BarPitchY"]	= m_siBarPitchY;

	if( m_dScanAcceptAngle==0 )
		 m_dScanAcceptAngle= m_dAcceptDieAngle;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_ACCEPT_ANGLE_1SEARCH]	= m_dScanAcceptAngle;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_GRADE_CONVERT]			= m_bUseAoiGradeConvert;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCN_NG_GRADE_CONVERT]		= m_bScanNgGradeConvert;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_BARWAFER_ALIGN_WITH_OCR]		= m_bOCRBarwaferNoMap;
	(*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_GRADE_MAPPLING_LIMIT]	= m_lGradeMappingLimit;
	for(int i=0; i<WPR_AOI_GRADE_MAX_MAPPING; i++)
	{
		(*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_MAP_ORIGE_GRADE][i]	= m_sMapOrgGrade[i];
		(*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_PR_INSPECT_GRADE][i]	= m_sPrInspectGrade[i];
		(*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_MAP_NEW_GRADE][i]	= m_sMapNewGrade[i];
	}

	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1_COAXLIGHT_LEVEL] = m_lWL1CoaxLightLevel;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1_RINGLIGHT_LEVEL] = m_lWL1RingLightLevel;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1_SIDELIGHT_LEVEL] = m_lWL1SideLightLevel;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1_BACKLIGHT_LEVEL] = m_lWL1BackLightLevel;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2_COAXLIGHT_LEVEL] = m_lWL2CoaxLightLevel;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2_RINGLIGHT_LEVEL] = m_lWL2RingLightLevel;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2_SIDELIGHT_LEVEL] = m_lWL2SideLightLevel;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2_BACKLIGHT_LEVEL] = m_lWL2BackLightLevel;

	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBRATED]	= m_bWL1Calibrated;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBXX]		= m_dWL1CalibXX;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBXY]		= m_dWL1CalibXY;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBYY]		= m_dWL1CalibYY;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBYX]		= m_dWL1CalibYX;

	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBRATED]	= m_bWL2Calibrated;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBXX]		= m_dWL2CalibXX;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBXY]		= m_dWL2CalibXY;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBYY]		= m_dWL2CalibYY;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBYX]		= m_dWL2CalibYX;

	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1WAFEROFFSETX] = m_lWL1WaferOffsetX;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1WAFEROFFSETY] = m_lWL1WaferOffsetY;

	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2WAFEROFFSETX] = m_lWL2WaferOffsetX;
	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2WAFEROFFSETY] = m_lWL2WaferOffsetY;

	(*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_CONTOUR_IMAGE_PATH]		= m_szContourImagePath ;
}

VOID CWaferPr::LoadPrescanSetupData(CStringMapFile  *psmf)
{
	m_lPrescanIndexRowReduce= (LONG)((*psmf)[WPR_SRH_NDIE_DATA][WPR_MAP_INDEX_REDUCE_ROW]);
	m_lPrescanIndexColReduce= (LONG)((*psmf)[WPR_SRH_NDIE_DATA][WPR_MAP_INDEX_REDUCE_COL]);
	m_bKeepScan2ndPrDie		= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_2ND_PR_DIE_TO_KEEP]);
	m_lSelect2ndPrID		= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_USE_19_AS_2NDPR_ID];
	m_bEnable2ndPrSearch	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_WITH_TWO_PR_RECORDS]);
	m_bAoiPrescanInspction	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_NDIE_PRESCAN_INSPECTION]);
	m_bSendWaferIDToPR		= (BOOL)((LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_SEND_WAFERID_TO_PR]);

	m_lPrescanSrchDieScore	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_SRH_NDIE_PASSSCORE][1];
	m_ucPrescanDiePrID		= (UCHAR)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_DIE_PR_ID]);
	m_ucScanAlignVerify		= (UCHAR)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_MAP_DUMMY_SCAN_RESULT_VERIFY]);
	m_lExtraPrescanOverlapX	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_EXTRA_OVERLAPX];
	m_lExtraPrescanOverlapY	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_EXTRA_OVERLAPY];
	m_lLookAroundRow		= (*psmf)[WPR_PSCAN_OPTIONS][WPR_HOME_LOOK_AROUND_ROW];
	m_lLookAroundCol		= (*psmf)[WPR_PSCAN_OPTIONS][WPR_HOME_LOOK_AROUND_COL];
	m_bHomeDieUniqueCheck	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_HOME_UNIQUE_CHECK]);
	m_bFovToFindGT			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_FOV_FIND_WAFER_ANGLE]);
	m_bFovFindGTMove		= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_FOV_FIND_WAFER_ANGLE_MOVE]);
	m_ucScanCheckMapOnWafer	= (UCHAR)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_CHECK_MAP_HOLES_WITH_WAFER]);
	m_bScanAlignFromFile	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_ALIGN_FROM_FILE]);
	m_bScanAlignAutoFromMap	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_HOLE_AUTO_FROM_MAP]);
	m_lScanAlignTotalHolesMin	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_TOTAL_HOLES_MIN];
	m_lScanAlignPatternHolesMin	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_PATTERN_HOLES_MIN];
	m_lScanAlignPatternHolesMax	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_PATTERN_HOLES_MAX];
	m_dSAMatchPatternsPercent	= (DOUBLE)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_MATCH_PATTERNS_PERCENT];
	m_dScanAlignMatchLowPercent	= (DOUBLE)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_ALIGN_MATCH_LOW_PERCENT];
	m_bRunPartialDie		= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_RUN_PARTIAL_DIE_PR]);
	m_lMinDieDistX			= (*psmf)[WPR_SRH_NDIE_DATA]["min die distance X for badcut die"];
	m_lMinDieDistY			= (*psmf)[WPR_SRH_NDIE_DATA]["min die distance Y for badcut die"];
	if( m_bRunPartialDie )
	{
		m_lExtraPrescanOverlapX = labs(m_lExtraPrescanOverlapX);
		m_lExtraPrescanOverlapY = labs(m_lExtraPrescanOverlapY);

		if(	m_ucScanWalkTour!=WT_SCAN_WALK_LEFT_VERT && 
			m_ucScanWalkTour!=WT_SCAN_WALK_TOP_HORI)
		{
			m_ucScanWalkTour = WT_SCAN_WALK_TOP_HORI;
		}
	}

	ULONG i;
	m_ulScanAlignTotalPoints	= (ULONG)(LONG)(*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_POINTS_NUM];
	m_ulHoleSkipPointLimit		= (ULONG)(LONG)(*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_HOLE_SKIP_LIMIT];
	for(i=0; i<m_ulScanAlignTotalPoints; i++)
	{
		LONG lRow = (*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_HOLE_ROW][i];
		LONG lCol = (*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_HOLE_COL][i];
		BYTE ucSta = (BYTE)(LONG)(*psmf)[WPR_SCAN_ALIGN_WAFER][WPR_SCAN_ALIGN_HOLE_STA][i];
		m_awPointsListRow.Add(lRow);
		m_awPointsListCol.Add(lCol);
		m_ucPointsListSta.Add(ucSta);
	}


	m_bPrescanNoTipDieMerge	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_NO_ORIENTATION_DIE_MERGE]);
	m_bPrescanGoodAction	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_HAVE_GOOD_DIE_ACTION]);
	m_bPrescanDefectAction	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_HAVE_DEFECT_DIE_ACTION]);
	m_bPrescanEmptyAction	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_HAVE_EMPTY_DIE_ACTION]);
	m_bPrescanBadCutAction	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_HAVE_BAD_CUT_DIE_ACTION]);
	m_bNgPickExtra			= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_EXTRA_NG_PICK]);
	m_bScanExtraDieAction	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_EXTRA_DIE_ACTION]);
	m_ucScanExtraGrade		= (UCHAR)(LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_EXTRA_DIE_GRADE];
	m_bScanDetectFakeEmpty	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_FAKE_EMPTY_ACTION]);
	m_ucScanFakeEmptyGrade	= (UCHAR)(LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_FAKE_EMPTY_GRADE];
	m_bPrescanSkipNgGrade	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SKIP_MAP_NG_GRADE_DIE]);
	m_bScanRunTimeDisplay	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SCAN_RUN_TIME_DISPLAY]);

	m_bSummaryOnlyNewGrade	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_SUMMARY_ONLY_NEW_GRADE]);

	m_ucPrescanGoodGrade	= (UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_GOOD_DIE_GRADE]);
	m_ucPrescanDefectGrade	= (UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_DEFECT_DIE_GRADE]);
	m_ucPrescanEmptyGrade	= (UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_EMPTY_DIE_GRADE]);
	m_ucPrescanBadCutGrade	= (UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_BAD_CUT_DIE_GRADE]);
	m_lMapNgPassScore		= (*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_MAP_NG_PASS_SCORE];
	m_ucScanMapStartNgGrade	= (UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_MAP_NG_GRADE_VALUE]);
	m_ucScanMapEndNgGrade	= (UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_MAP_END_NG_GRADE_VALUE]);
	// AOI NG Grade
	m_ucScanMapStartAOINgGrade	=	(UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_MAP_AOI_NG_GRADE_VALUE]);
	m_ucScanMapEndAOINgGrade	=	(UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_MAP_END_AOI_NG_GRADE_VALUE]);
	m_ucScanMapNgListSize	= (UCHAR)((LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_NG_GRADE_LIST_SIZE]);
	//if( m_ucScanMapNgListSize>255 )	//Klocwork	//v4.46
	//	m_ucScanMapNgListSize = 255;
	for(i=0; i<255; i++)
	{
		m_ucScanMapNgList[i]= (UCHAR)((LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_NG_GRADE_LIST][i+1]);
	}
	m_ucPrescanEdgeSizeX	= (UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_MAP_EDGE_SIZE]);
	m_ucPrescanEdgeSizeY	= (UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_MAP_EDGE_SIZE_Y]);
	m_ucPrescanEdgeGrade	= (UCHAR)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_MAP_EDGE_GRADE]);
	m_dEdgeGoodScore		= (*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_MAP_EDGE_GOODSCORE];
	m_ucPrescanEdgeSize		= max(m_ucPrescanEdgeSizeX, m_ucPrescanEdgeSizeY);

	m_ucMapEdgeSize_inDailyCheck	= (*psmf)[WPR_DEFECT_SCAN_SETTING][PB_MAP_EDGE_SIZE_IN_DAILY_CHECK];

	m_bFastHomeScanEnable		= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_IM_FASTHOME_SCAN_DIE]);
	m_bIMPrescanReferDie		= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_IM_PRESCAN_REFER_DIE]);
	m_bPrescanDefectToNullBin	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_DEFECT_DIE_TO_NULL_BIN]);
	m_bPrescanEmptyToNullBin	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_EMPTY_DIE_TO_NULL_BIN]);
	m_bPrescanBadCutToNullBin	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_BAD_CUT_DIE_TO_NULL_BIN]);
	m_bPrescanDefectMarkUnPick	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_DEFECT_DIE_MARK_UNPICK]);
	m_bPrescanEmptyMarkUnPick	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_EMPTY_DIE_MARK_UNPICK]);
	m_bShowNgDieSubGrade		=  (BOOL)((LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_SHOW_NG_DIE_SUB_GRADE]);
	m_bSaveNgDieSubGrade		=  (BOOL)((LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_SAVE_NG_DIE_SUB_GRADE]);

	if( m_ucPrescanGoodGrade==0 )
		m_ucPrescanGoodGrade = 91;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetFeatureStatus(MS896A_FUNC_PRESCAN_EMPTY_UNMARK)!=TRUE && IsSorraSortMode()!=TRUE )
		m_bPrescanEmptyMarkUnPick = !m_bPrescanEmptyAction;

	m_dRescanGoodRatio			= (*psmf)[WPR_PSCAN_OPTIONS][WPR_RESCAN_GOOD_RATIO];
	m_bAccurateRescan			= (BOOL)((LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_ACCURATE_RESCAN]);
	m_bAutoRefillRescan			= (BOOL)((LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_AUTO_REFILL_RESCAN]);
	m_bPrescanBadCutMarkUnPick	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_BAD_CUT_DIE_MARK_UNPICK]);
	m_bPrescanKeepPsmUnPickDie	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_KEEP_PSM_UNPICK]);
	m_bPrescanPrCheckReferDie	= (BOOL)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PR_CHECK_REFER_DIE]);
	m_dPrescanPitchTolX			= (DOUBLE)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_PITCH_TOL_X]);
	m_dPrescanPitchTolY			= (DOUBLE)((LONG)(*psmf)[WPR_DEFECT_SCAN_SETTING][WPR_PRESCAN_PITCH_TOL_Y]);
	if( m_dPrescanPitchTolX<10.0 )
		m_dPrescanPitchTolX = 30.0;
	if( m_dPrescanPitchTolY<10.0 )
		m_dPrescanPitchTolY = 30.0;
	m_uwIMMergeNewRecordID		= (PR_UWORD)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_REFSCAN_MERGE_ID]);
	m_dScanAlignMPassScore		= (*psmf)[WPR_PSCAN_OPTIONS][WPR_PSCAN_SCAN_ALIGN_PASSSCORE];
	for(int i=0; i<WPR_OCR_MAX_WINDOWS; i++)
	{
		CString szTemp;
		szTemp.Format("Window%d_Corner1_X", i+1);
		m_stOcrDetectWin[i].coCorner1.x	= (PR_WORD)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS][szTemp]);
		szTemp.Format("Window%d_Corner1_Y", i+1);
		m_stOcrDetectWin[i].coCorner1.y	= (PR_WORD)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS][szTemp]);
		szTemp.Format("Window%d_Corner2_X", i+1);
		m_stOcrDetectWin[i].coCorner2.x	= (PR_WORD)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS][szTemp]);
		szTemp.Format("Window%d_Corner2_Y", i+1);
		m_stOcrDetectWin[i].coCorner2.y	= (PR_WORD)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS][szTemp]);
	}

	m_stOcrViewWindow.coCorner1.x = (PR_WORD)(LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["ViewULX"];
	m_stOcrViewWindow.coCorner1.y = (PR_WORD)(LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["ViewULY"];
	m_stOcrViewWindow.coCorner2.x = (PR_WORD)(LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["ViewLRX"];
	m_stOcrViewWindow.coCorner2.y = (PR_WORD)(LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["ViewLRY"];
	CheckOcrViewWindow();

	m_siBarPitchX	= (LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["BarPitchX"];
	m_siBarPitchY	= (LONG)(*psmf)[WPR_PSCAN_OPTIONS][WPR_ES101_OCR_WINDOWS]["BarPitchY"];
	m_lOcrPassScore			= (*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_SRH_OCR_PASSSCORE];
	if( m_lOcrPassScore==0 )
		m_lOcrPassScore = 85;

	m_dScanAcceptAngle		= (*psmf)[WPR_PSCAN_OPTIONS][WPR_SCAN_ACCEPT_ANGLE_1SEARCH];
	if( m_dScanAcceptAngle==0 )
		 m_dScanAcceptAngle= m_dAcceptDieAngle;
	m_bUseAoiGradeConvert	= (BOOL)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_GRADE_CONVERT]);
	m_bScanNgGradeConvert	= (BOOL)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_SCN_NG_GRADE_CONVERT]);
	m_bOCRBarwaferNoMap		= (BOOL)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_BARWAFER_ALIGN_WITH_OCR]);
	m_lGradeMappingLimit	= (*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_GRADE_MAPPLING_LIMIT];
	for(int i=0; i<WPR_AOI_GRADE_MAX_MAPPING; i++)
	{
		m_sMapOrgGrade[i]	= (SHORT)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_MAP_ORIGE_GRADE][i]);
		m_sPrInspectGrade[i]= (SHORT)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_PR_INSPECT_GRADE][i]);
		m_sMapNewGrade[i]	= (SHORT)(LONG)((*psmf)[WPR_PSCAN_OPTIONS][WPR_AOI_MAP_NEW_GRADE][i]);
	}

	m_lWL1CoaxLightLevel = (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1_COAXLIGHT_LEVEL];
	m_lWL1RingLightLevel = (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1_RINGLIGHT_LEVEL];
	m_lWL1SideLightLevel = (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1_SIDELIGHT_LEVEL];
	m_lWL1BackLightLevel = (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1_BACKLIGHT_LEVEL];

	m_lWL2CoaxLightLevel = (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2_COAXLIGHT_LEVEL];
	m_lWL2RingLightLevel = (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2_RINGLIGHT_LEVEL];
	m_lWL2SideLightLevel = (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2_SIDELIGHT_LEVEL];
	m_lWL2BackLightLevel = (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2_BACKLIGHT_LEVEL];

	m_szContourImagePath = "";
	if ((!((*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_CONTOUR_IMAGE_PATH])) == FALSE)
	{
		m_szContourImagePath = (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_CONTOUR_IMAGE_PATH];
	}

	if( m_szContourImagePath.IsEmpty() )
		m_szContourImagePath = m_szSaveMapImagePath;

	m_bWaferAutoFocus	= (BOOL)(LONG)(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_WAFER];
	m_lAFUpLimit		= (*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_UP_LIMIT];
	m_lAFLowLimit		= (*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_LOW_LIMIT];
	m_lAFPrDelay		= (*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_PR_DELAY];
	m_lAFErrorTol		= (*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_ERROR_TOL];
	m_lAFMinDist		= (*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_MIN_DIST];
	m_dAFrValueLimit	= (DOUBLE)(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_RVALUE_LIMIT];

	m_lBLZAutoFocusDelay		= (*psmf)[WPR_CAMERA_DATA][WPR_BACKLIGHT_AUTO_FOCUS_DELAY];

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	BOOL bUnpick = m_bPrescanDefectMarkUnPick || m_bPrescanEmptyMarkUnPick || m_bPrescanBadCutMarkUnPick;
	pUtl->SetMarkDieUnpick(bUnpick);
	if( m_bUseAoiGradeConvert )
	{
		m_bScanNgGradeConvert	= FALSE;
		m_bSaveNgDieSubGrade	= FALSE;
	}
	if( m_bScanAlignAutoFromMap && m_bScanAlignFromFile )
	{
		m_bScanAlignAutoFromMap = m_bScanAlignFromFile = FALSE;
	}
	if( IsScanAlignWafer() )
	{
		m_ucScanCheckMapOnWafer = 0;
	}
}

VOID CWaferPr::LoadPrescanDataPr(CStringMapFile  *psmf)
{
	INT i =0;

	m_lUsedSrchThreadNum	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_ES100_SRCH_THREAD_NUM];

	for (i=0; i<WPR_MAX_DIE; i++)
	{
		m_uwIMMergeRecordID[i]	= (PR_UWORD)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_REFSCAN_MERGE_ELEMENTS][i+1]);
	}

	if( m_lUsedSrchThreadNum<=0 )
		m_lUsedSrchThreadNum = MAX_PR_GALLERY_SEARCH_THREADS;
	m_lUsedSrchThreadNum	= m_pPrescanPrCtrl->SetSrchThreadNum(m_lUsedSrchThreadNum);
	m_lWaitingImageLimit	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SCAN_GRAB_IMAGE_LIMIT];
	m_lWaitingImageLimit	= min(m_lWaitingImageLimit, m_lPrescanImageLimit/2);
	m_lWaitingImageLimit	= m_pPrescanPrCtrl->SetGrabImageLimit(m_lWaitingImageLimit);
}

BOOL CWaferPr::IsLastReply(PR_COMMON_RPY stStatus)
{
	return PR_IsLastReply(&stStatus)==PR_TRUE;
}

BOOL CWaferPr::IsLFSizeOK()
{
	if( m_dLFSize >= WPR_LF_SIZE )
		return TRUE;
	else
		return FALSE;
}

BOOL CWaferPr::IsThisDieLearnt(UCHAR ucDieIndex)
{
	if( IsBurnIn() && m_bNoWprBurnIn )
	{
		return TRUE;
	}

	ucDieIndex = min(ucDieIndex, WPR_MAX_DIE-1);
	return m_bGenDieLearnt[ucDieIndex];
}

PR_WORD CWaferPr::GetDiePrID(UCHAR ucDieIndex)
{
	ucDieIndex = min(ucDieIndex, WPR_MAX_DIE-1);
	return	m_ssGenPRSrchID[ucDieIndex];
}

BOOL CWaferPr::IsUseLF()
{
	if ( IsES101() || IsES201() )		//v4.28
		return FALSE;
	else
		return (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["UseLookForward"];
}


BOOL CWaferPr::IsSpecialReferGrade(ULONG ulRow, ULONG ulCol)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if (!IsWaferMapValid())
		return FALSE;

	if (m_WaferMapWrapper.GetReader() == NULL)
		return FALSE;

	if( pUtl->GetSpcReferGrade()<=0 )
		return FALSE;

	if( HasSpRefDieCheck() )
	{
		//If current grade equals special REF_DIE grade set by user
		UCHAR ucGrade = m_WaferMapWrapper.GetReader()->GetDieInformation(ulRow, ulCol) - m_WaferMapWrapper.GetGradeOffset();
		if( ucGrade == pUtl->GetSpcReferGrade() )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CWaferPr::IsNoDieGrade(ULONG ulRow, ULONG ulCol)
{
	LONG lNoDieGrade = m_lMnNoDieGrade;

	return	(m_bUseEmptyCheck) && (lNoDieGrade>0) && 
			(m_WaferMapWrapper.GetGrade(ulRow, ulCol) == (lNoDieGrade + m_WaferMapWrapper.GetGradeOffset()));
}

LONG CWaferPr::GetDieCoordinate(PR_RCOORD coDieCtr)
{
	int nPitchX = GetScanNmlSizePixelX();
	int nPitchY = GetScanNmlSizePixelY();
	int nPosnX = (int) coDieCtr.x;
	int nPosnY = (int) coDieCtr.y;

	LONG lValue = 4;

	if( nPosnX<(GetPrCenterX()-nPitchX) )
		lValue += 10;

	if( nPosnX>(GetPrCenterX()+nPitchX) )
		lValue += 100;

	if( nPosnY<(GetPrCenterY()-nPitchY) )
		lValue += 1000;

	if( nPosnY>(GetPrCenterY()+nPitchY) )
		lValue += 10000;

	return lValue;
}

// AOI PR TEST
BOOL CWaferPr::ConvertMotorStepToPrPixel(LONG lX, LONG lY, DOUBLE &dPrX, DOUBLE &dPrY)
{
	DOUBLE dDivid = 0;
	double dCalibX = GetCalibX();
	double dCalibY = GetCalibY();
	double dCalibXY= GetCalibXY();
	double dCalibYX= GetCalibYX();

	dDivid = dCalibX*dCalibY - dCalibXY*dCalibYX;

	if( fabs(dDivid)>0.000001 )
	{
		dPrX = (dCalibY*lX - dCalibXY*lY)/dDivid;
		dPrY = (dCalibX*lY - dCalibYX*lX)/dDivid;
	}
	else
	{
		if( fabs(dCalibX)>0.000001 && fabs(dCalibY)>0.000001 )
		{
			dPrX = lX/dCalibX;
			dPrY = lY/dCalibY;
		}
		else
		{
			if( IsES101()==FALSE && IsES201()==FALSE )
				HmiMessage("WPR calibration not good, please do it again");
			return FALSE;
		}
	}

	return TRUE;
}

VOID CWaferPr::SetMapPhysicalPosition(LONG ulMapRow, LONG ulMapCol, LONG lWftX, LONG lWftY)
{
	SetMapPhyPosn(ulMapRow, ulMapCol, lWftX, lWftY);
}

// AOI PR TEST

LONG CWaferPr::LFAutoLearnDiePitch(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		int nPitchXX;
		int nPitchXY;
		int nPitchYY;
		int nPitchYX;
	} DIEPITCHDATA;

	DIEPITCHDATA stPitchResult;
	stPitchResult.nPitchXX = GetDiePitchXX();
	stPitchResult.nPitchXY = GetDiePitchXY();
	stPitchResult.nPitchYY = GetDiePitchYY();
	stPitchResult.nPitchYX = GetDiePitchYX();

	if( PR_NotInit() || (m_bDieCalibrated==FALSE) ||
		(IsNormalDieLearnt()==FALSE) ||	!IsLFSizeOK() )
	{
		svMsg.InitMessage(sizeof(DIEPITCHDATA), &stPitchResult);
		return 1;
	}

	int				nFlipX = 1;
	int				nFlipY = 1;

	int	stDiePosX[7], stDiePosY[7];

	//Check LF can be use if user enable LF option
	//Store the die is found or not in specific region
	BOOL bDieFound[7];
	LONG nPos[7] = {WPR_CT_DIE, WPR_LT_DIE, WPR_UL_DIE, WPR_DL_DIE, WPR_RT_DIE, WPR_UR_DIE, WPR_DR_DIE};

	stDiePosX[0] = 0;
	stDiePosY[0] = 0;
	for (int i=0; i<7 ;i++)
	{
		stDiePosX[i] = stDiePosX[0];
		stDiePosY[i] = stDiePosY[0];
		PR_BOOLEAN bLatch = (i==0)?PR_TRUE:PR_FALSE;
		bDieFound[i] = IndexAndSearchNormalDie(&stDiePosX[i], &stDiePosY[i], nPos[i], nPos[i], FALSE, FALSE, bLatch);
	}

	bool bFindLDie = false;
	if (bDieFound[0] && bDieFound[1])
	{
		// Use Left Upper path to learn Die
		if (bDieFound[2])
		{
			bFindLDie = true;
		}
		// Use Left Lower path to learn Die
		else if (bDieFound[3])
		{
			nFlipY = -1;
			stDiePosX[2] = stDiePosX[3];
			stDiePosY[2] = stDiePosY[3];
			bFindLDie = true;
		}
	}

	//Draw Home cursor
	//PR_DRAW_HOME_CURSOR_CMD 		stHomeCmd;
	PR_COORD						stCrossHair;
	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);

	stCrossHair.x = (PR_WORD) GetPrCenterX();
	stCrossHair.y = (PR_WORD) GetPrCenterY();
	DrawHomeCursor(stCrossHair);
	DrawSearchBox( PR_COLOR_TRANSPARENT);

	if( bFindLDie==false )
	{
		HmiMessage_Red_Back("Prescan auto die pitch: no die found.", "Prescan");
		svMsg.InitMessage(sizeof(DIEPITCHDATA), &stPitchResult);
		return 1;
	}

	//Calculate die pitch in motor step
	int nDiePitchXX = nFlipX*(stDiePosX[1] - stDiePosX[0]);				
	int nDiePitchXY = nFlipX*(stDiePosY[1] - stDiePosY[0]);				
	int nDiePitchYY = nFlipY*(stDiePosY[2] - stDiePosY[1]);
	int nDiePitchYX = nFlipY*(stDiePosX[2] - stDiePosX[1]);				

	if ( GetDieShape() == WPR_HEXAGON_DIE )
	{
		nDiePitchXX = nDiePitchXX/2;				
		nDiePitchYY = nDiePitchYY/2;				
	}
	CString szMsg;

	BOOL bPitchFail = FALSE;
	if( abs(nDiePitchXX-GetDiePitchXX()) > abs(GetDiePitchXX()*(LONG)m_dPrescanPitchTolX/100) )
	{
		nDiePitchXX = GetDiePitchXX();
		bPitchFail = TRUE;
	}
	if( abs(nDiePitchYY-GetDiePitchYY()) > abs(GetDiePitchYY()*(LONG)m_dPrescanPitchTolY/100) )
	{
		nDiePitchYY = GetDiePitchYY();
		bPitchFail = TRUE;
	}

	if( bPitchFail )
	{
		HmiMessage_Red_Back("Prescan auto die pitch: over limit.", "Prescan");
	}
//	if( abs(nDiePitchXY)<abs(GetDiePitchXY()) )
//	{
//		nDiePitchXY = GetDiePitchXY();
//	}
//	if( abs(nDiePitchYX)<abs(GetDiePitchYX()) )
//	{
//		nDiePitchYX = GetDiePitchYX();
//	}

	stPitchResult.nPitchXX = nDiePitchXX;
	stPitchResult.nPitchXY = nDiePitchXY;
	stPitchResult.nPitchYY = nDiePitchYY;
	stPitchResult.nPitchYX = nDiePitchYX;

	if( IsPrescanMapIndex() )
	{
		DOUBLE dPrescanLFSizeX	= m_dLFSizeX;
		DOUBLE dPrescanLFSizeY	= m_dLFSizeY;
	}

	svMsg.InitMessage(sizeof(DIEPITCHDATA), &stPitchResult);

	return 1;
}

LONG CWaferPr::RefreshPrScreen(IPC_CServiceMessage& svMsg)
{
	m_bSetAutoBondScreen = TRUE;
	return 1;
}

LONG CWaferPr::PrAutoBondScreenUpdate(IPC_CServiceMessage& svMsg)
{
	BOOL bNormalDie = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bNormalDie);

	svMsg.InitMessage(sizeof(BOOL), &bNormalDie);

	return 1;
}

BOOL CWaferPr::AutoBondWaferScreenUpdate(PR_BOOLEAN bNormalDie)
{
	return TRUE;
}

BOOL CWaferPr::SetMapPhyPosn(ULONG ulRow, ULONG ulCol, LONG lPhyX, LONG lPhyY)
{
	return m_WaferMapWrapper.SetPhysicalPosition(ulRow, ulCol, lPhyX, lPhyY);
}

LONG CWaferPr::GrabAndSaveImageCmd(IPC_CServiceMessage& svMsg)
{
	typedef struct
	{
		BOOL bDieType;
		LONG lDiePrID;
		LONG lCheckType;
	}	GRABPR;
	GRABPR stData;

	svMsg.GetMsg(sizeof(GRABPR), &stData);

	GrabAndSaveImage(stData.bDieType, stData.lDiePrID, stData.lCheckType);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferPr::GrabAndSaveImage(BOOL bDieType, LONG lDiePrID, LONG lCheckType)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szNamePrefix;
	CString szNameType;
	PR_WORD wResult = 0;
	PR_UWORD		usDieType;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;

	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX()-1, GetPRWinLRY()-1};

	if (m_bEnableAlignWaferImageLog == FALSE)
	{
		return TRUE;
	}

	Sleep(100);

	CString szSaveImagePath;
	CString szLogPath;
	CString szWaferName = GetMapNameOnly();	// already remove ext . when assign vale

	szSaveImagePath = WPR_PR_DISPLAY_IMAGE_LOG;
	if (IsPathExist(m_szSaveImagePath))
	{
		szSaveImagePath = m_szSaveImagePath;
	}
	CreateDirectory(szSaveImagePath, NULL);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	
	//if( pApp->GetCustomerName()==CTM_SEMITEK )
	pApp->SearchAndRemoveFiles(szSaveImagePath, 7);	//v0.08

	CString szLocalImagesPath = PRESCAN_RESULT_FULL_PATH + "SaveImages\\" + GetMapNameOnly();
	BOOL bHmiOnly = FALSE;
	switch( lCheckType )
	{
	case WPR_GRAB_SAVE_IMG_5PC:	// five point check
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\5PC";
		break;

	case WPR_GRAB_SAVE_IMG_MAN:	// manual align
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\MAN";
		break;

	case WPR_GRAB_SAVE_IMG_UGO:	// user press go button to check prescan result
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\UGO";
		break;

	case WPR_GRAB_SAVE_IMG_FHD:	// find home die in auto or hmi
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\FHD";
		break;

	case WPR_GRAB_SAVE_IMG_SCN:	// prescan finished and realign at wft side
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\SCN";
		break;

	case WPR_GRAB_SAVE_IMG_PCV:	// check map/wafer map when pitch alarm
		szNameType = szLocalImagesPath + "_PCV";
		break;

	case WPR_GRAB_SAVE_IMG_PCM:	// check pitch and verify with multi search
		szNameType = szLocalImagesPath + "_PCM";
		bHmiOnly = TRUE;
		break;
	case WPR_GRAB_SAVE_IMG_ARP:	// add refer points
		szNameType = szLocalImagesPath + "_ARP";
		break;

	case WPR_GRAB_SAVE_IMG_ALL:	// ALL done after prescan to realign
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\ALL";
		break;

	case WPR_GRAB_SAVE_IMG_N2RC:
		bHmiOnly = TRUE;
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\N2RC";
		break;

	case WPR_GRAB_SAVE_IMG_RC2N:
		bHmiOnly = TRUE;
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\RC2N";
		break;

	case WPR_GRAB_SAVE_IMG_N2RX:
		bHmiOnly = TRUE;
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\N2RX";
		break;

	case WPR_GRAB_SAVE_IMG_RX2N:
		bHmiOnly = TRUE;
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\RX2N";
		break;

	case WPR_GSI_ADV_OFFSET_OVER:
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\ADVOUT";
		break;

	case WPR_GSI_ADV_VERIFY_FAIL:
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\VFAIL";
		break;

	case WPR_GRAB_SAVE_IMG_NOUT:
		bHmiOnly = TRUE;
		szLogPath.Format("%s\\%s", szSaveImagePath, szWaferName);
		CreateDirectory(szLogPath, NULL);
		szNameType = szLogPath + "\\NOUT";
		break;

	default:
		szNameType = szLocalImagesPath + "_UFO";
		break;
	}

	if ( (bDieType == WPR_REFERENCE_DIE) )
	{
		szNameType += "_R";
		LONG	lRefDieStart = lDiePrID-1;
		LONG	lRefDieStop  = lDiePrID-1;
		if( lDiePrID<=0 )
		{
			lRefDieStart = 0;
			lRefDieStop  = m_lLrnTotalRefDie-1;
		}

		for (int i = lRefDieStart; i <= lRefDieStop; i++)
		{
			if( bHmiOnly==FALSE )
				wResult = SemiSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE + i, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
			
			if (wResult != -1)
			{
				//Sleep(8000); //Wait the wafer map loading then cap the screen
				LONG lTimeOut = 10000; // Timeout 10s
				LONG lCurrentTime = GetCurTime();
				while(GetCurTime() < lCurrentTime + lTimeOut)
				{
					if (m_WaferMapWrapper.IsMapValid() == TRUE)
					{
						break;
					}
					else
					{
						Sleep(1);
					}
				}
				szNamePrefix.Format("%s_%d", szNameType, i+1);
				SaveImageScreenData(szNamePrefix, bHmiOnly);
			}
		}
	}
	else
	{
		szNameType += "_N";

		if( bHmiOnly==FALSE )
		{
			wResult = ManualSearchDie(bDieType, lDiePrID, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
		}

		if (wResult != -1)
		{
			//Sleep(8000); //Wait the wafer map loading then cap the screen
			LONG lTimeOut = 10000; // Timeout 10s
			LONG lCurrentTime = GetCurTime();
			while(GetCurTime() < lCurrentTime + lTimeOut)
			{
				if (m_WaferMapWrapper.IsMapValid() == TRUE)
				{
					break;
				}
				else
				{
					Sleep(1);
				}
			}
			szNamePrefix.Format("%s_%d", szNameType, lDiePrID);
			SaveImageScreenData(szNamePrefix, bHmiOnly);
		}
	}

	return TRUE;
}

BOOL CWaferPr::SaveImageScreenData(CString szNamePrefix, BOOL bHmiOnly)
{
	CTime ctDateTime = CTime::GetCurrentTime();
	CString szDateTime = ctDateTime.Format("%Y%m%d%H%M%S");

	CString szScreenFile;

	int i = 0;
	for(i=0; i<100; i++)
	{
		szScreenFile.Format(   "%s_%s_%02d_hmi.JPG", szNamePrefix, szDateTime, i);
		if( (_access(szScreenFile,		0) == -1) )
		{
			break;
		}
	}

	// capture whole screen.]
	if( szScreenFile.Find("_PCV_")==-1 )
		PrintScreen(szScreenFile);

	if( bHmiOnly )
	{
		return TRUE;
	}

	CString szImageFilename;
	CString szLogFileName;

	szLogFileName.Format(  "%s_%s_%02d.txt", szNamePrefix, szDateTime, i);
	szImageFilename.Format("%s_%s_%02d.JPG", szNamePrefix, szDateTime, i);

	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFileName, "w");
	if ((nErr == 0) && (fp != NULL))
	{
		LONG lUserRow = 0, lUserCol = 0;
		LONG lOrgRow  = 0, lOrgCol  = 0;
		LONG lWftPosX = 0, lWftPosY = 0;
		ULONG ulRow, ulCol;

		lWftPosX	= (LONG) (*m_psmfSRam)["MS896A"]["WaferTableX"];
		lWftPosY	= (LONG) (*m_psmfSRam)["MS896A"]["WaferTableY"];
		ulRow		= (ULONG)(LONG)	((*m_psmfSRam)["MS896A"]["WaferMapRow"]);
		ulCol		= (ULONG)(LONG)	((*m_psmfSRam)["MS896A"]["WaferMapCol"]);

		ConvertAsmToHmiUser(ulRow, ulCol, lUserRow, lUserCol); 
		ConvertAsmToOrgUser(ulRow, ulCol, lOrgRow, lOrgCol);

		fprintf(fp, "Asm Map (%ld,%ld); Die Map (Row,Column): %ld, %ld; (%ld, %ld)\n", ulRow, ulCol, lUserRow, lUserCol, lOrgRow, lOrgCol);
		fprintf(fp, "Die Pos (X,Y): (%ld, %ld)\n", lWftPosX, lWftPosY);
		CString szExtraMsg	= (*m_psmfSRam)["MS896A"]["SaveImageExtraInfo"];
		if( szExtraMsg.IsEmpty()==FALSE )
		{
			fprintf(fp, "%s\n", (LPCTSTR) szExtraMsg);
		}
		fclose(fp);
	}

	PR_SAVE_DISP_IMG_CMD stSaveDispCmd;
	PR_SAVE_DISP_IMG_RPY stRpy;

	CString szTempPath = "C:\\MapSorter\\UserData\\Capture.JPG";//in case szImageFilename is too long
	PR_InitSaveDispImgCmd(&stSaveDispCmd);

	//strcpy_s((char*) stSaveDispCmd.aubSourceLogFileNameWithPath, sizeof(stSaveDispCmd.aubSourceLogFileNameWithPath),  (LPCTSTR) szImageFilename);
	strcpy_s((char*) stSaveDispCmd.aubSourceLogFileNameWithPath, sizeof(stSaveDispCmd.aubSourceLogFileNameWithPath),  (LPCTSTR) szTempPath);

	PR_SaveDispImgCmd(&stSaveDispCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stRpy);

	BOOL bReturn = TRUE;
	if( (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR) ||
		(stRpy.stStatus.uwPRStatus!= PR_ERR_NOERR) )
	{
		bReturn = FALSE;
	}
	CopyFile(szTempPath,szImageFilename,FALSE);
	DeleteFile(szTempPath);

	return bReturn;
}

PR_DIE_ALIGN_ALG CWaferPr::GetLrnAlignAlgo(LONG lDieNo)
{
	PR_DIE_ALIGN_ALG usAlignAlg;

	lDieNo = min(lDieNo, WPR_MAX_DIE-1);	//Klocwork	//v4.02T5

	switch (m_lGenLrnAlignAlgo[lDieNo])
	{
	case 1:		//Pattern matching	
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_DIE_PATTERN;
		break;
	case 2:		//Edge matching	 
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_DIE_EDGES_WO_PATTERN_GUIDED;
		break;
	case 3:		//BOTH matching	 
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_BOTH;		//v3.76
		break;
	case 4:		// Edge Synthetic Matching
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_SYNTHETIC_DIE_EDGE;
		break;
	case 5:
		usAlignAlg = PR_BLOB_ANALYSIS_PLUS_LINE_FITTING;
		break;
	default:	//Pattern Guided
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_DIE_EDGES;
		break;

	}

	return usAlignAlg;
}

CString CWaferPr::GetLrnAlignAlgoInString(LONG lDieNo)
{
	CString szAlignAlg = "";

	lDieNo = min(lDieNo, WPR_MAX_DIE-1);

	switch (m_lGenLrnAlignAlgo[lDieNo])
	{
	case 1:		//Pattern matching	
		szAlignAlg = "PR_EDGE_POINTS_MATCHING_ON_DIE_PATTERN";
		break;
	case 2:		//Edge matching	 
		szAlignAlg = "PR_EDGE_POINTS_MATCHING_ON_DIE_EDGES_WO_PATTERN_GUIDED";
		break;
	case 3:		//BOTH matching	 
		szAlignAlg = "PR_EDGE_POINTS_MATCHING_ON_BOTH";	
		break;
	case 4:		// Edge Synthetic Matching
		szAlignAlg = "PR_EDGE_POINTS_MATCHING_ON_SYNTHETIC_DIE_EDGE";
		break;
	case 5:
		szAlignAlg = "PR_BLOB_ANALYSIS_PLUS_LINE_FITTING";
		break;
	default:	//Pattern Guided
		szAlignAlg = "PR_EDGE_POINTS_MATCHING_ON_DIE_EDGES";
		break;
	}

	return szAlignAlg;
}

PR_BACKGROUND_INFO CWaferPr::GetLrnBackGround(LONG lDieNo)
{
	PR_BACKGROUND_INFO emBGInfo;

	lDieNo = min(lDieNo, WPR_MAX_DIE-1);	//Klocwork	//v4.02T5

	switch( m_lGenLrnBackgroud[lDieNo] )
	{
	case 1:
		emBGInfo = PR_BACKGROUND_INFO_BRIGHT;
		break;
	case 2:
		emBGInfo = PR_BACKGROUND_INFO_AUTO;
		break;
	default:
		emBGInfo = PR_BACKGROUND_INFO_DARK;
		break;
	}

	return emBGInfo;
}

PR_DIE_INSP_ALG	CWaferPr::GetLrnInspMethod(LONG lDieNo)
{
	PR_DIE_INSP_ALG usInspAlg;
	
	lDieNo = min(lDieNo, WPR_MAX_DIE-1);	//Klocwork	//v4.02T5

	switch (m_lGenLrnInspMethod[lDieNo])
	{
	case 1:  
		usInspAlg = PR_DIE_INSP_ALG_BIN;
		break;

	default: 
		usInspAlg = PR_GOLDEN_DIE_TMPL;
		break;
	}
	return usInspAlg;
}

CString CWaferPr::GetLrnInspMethodInString(LONG lDieNo)
{
	CString szInspAlg = "";
	
	lDieNo = min(lDieNo, WPR_MAX_DIE-1);

	switch (m_lGenLrnInspMethod[lDieNo])
	{
	case 1:  
		szInspAlg = "PR_DIE_INSP_ALG_BIN";
		break;

	default: 
		szInspAlg = "PR_GOLDEN_DIE_TMPL";
		break;
	}
	return szInspAlg;
}

BOOL CWaferPr::IsPrescanReferDie()
{
	return ( m_bIMPrescanReferDie && (m_lLrnTotalRefDie>=1) );
}

BOOL CWaferPr::IsPrecanWith2Pr()
{
	if( GetNewPickCount()>0 && IsPrescanEnded() )
		return FALSE;
	return m_bEnable2ndPrSearch;
}

BOOL CWaferPr::IsStitchMode()
{
	return FALSE;
}

BOOL CWaferPr::IM_DummySearchDie(ULONG ulRow, ULONG ulCol, LONG &lOffsetX, LONG &lOffsetY)
{
	LONG	i, lStartID, lEndID;
	BOOL bSrchDieGood = FALSE;

	LONG lRefDieCheck	= (*m_psmfSRam)["DieInfo"]["RefDie"]["Check"];
	LONG lSpRefDieCheck = (*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"];

	(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"]	= 0;
	if( m_bCheckAllNmlDie )
	{
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]		= 0;
		lStartID = 2;
		lEndID = 2;
	}
	else
	{
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]		= 1;
		if( m_bAllRefDieCheck )
		{
			lStartID = 1;
			lEndID = m_lLrnTotalRefDie;
		}
		else
		{
			lStartID = 1;
			lEndID = 1;
		}
	}

	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Action"] = WAF_CDieSelectionAlgorithm::CHECK;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"] = ulRow;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"] = ulCol;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Grade"] = 1;

	for (i=lStartID; i<=lEndID; i++)
	{
		m_lCurrSrchRefDieId = i;

		if ( AutoSearchDie() == TRUE )
		{
			Sleep(100);
			if ( AutoGrabDone() == TRUE )
			{
				Sleep(100);
				if ( AutoDieResult() == TRUE )
				{
					bSrchDieGood = TRUE;
					lOffsetX = GetDiePROffsetX();
					lOffsetY = GetDiePROffsetY();
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]		= lRefDieCheck;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"]	= lSpRefDieCheck;
	m_lCurrSrchRefDieId = 1;

	return bSrchDieGood;
}

LONG CWaferPr::MultiSearchInitNmlDie1(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=TRUE;
	if( IsThisDieLearnt(0)!=TRUE )
	{
		HmiMessage_Red_Back("Normal die PR 1 not learnt yet!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BOOL bInspect = FALSE;
	svMsg.GetMsg(sizeof(BOOL),	&bInspect);

	PrescanNormalInit(GetPrescanPrID(), bInspect);	//	wafer table side trigger.

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::MultiSearchNmlDie1(IPC_CServiceMessage &svMsg)
{
	MULTI_SRCH_RESULT	stMsch;

	BOOL bDrawDie = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bDrawDie);
	if( bDrawDie==TRUE )
	{
		OpenWaitingAlert();
		m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	}
	int i;
	CDWordArray dwList;
	for(i=0; i<5; i++)
	{
		dwList.SetSize(i+1);
		dwList.SetAt(i, 0);
		stMsch.bDieState[i] = 0;
	}

	ClearGoodInfo();

	PrescanAutoMultiGrabDone(dwList, bDrawDie);

	for(i=0; i<5; i++)
	{
		stMsch.bDieState[i] = dwList.GetAt(i);
	}

	if( bDrawDie==TRUE )
	{
		CloseWaitingAlert();
	}

	svMsg.InitMessage(sizeof(MULTI_SRCH_RESULT), &stMsch);

	return 1;
}

LONG CWaferPr::MultiSearchNml3Die(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	ClearGoodInfo();

	if( IsThisDieLearnt(2) != TRUE )
	{
		HmiMessage_Red_Back("Normal die PR 3 not learnt yet!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	OpPrescanInit(3);	//	for normal die record 3.

	BOOL bDrawDie = FALSE;
	CDWordArray dwList;

	svMsg.GetMsg(sizeof(BOOL), &bDrawDie);
	if( bDrawDie==TRUE )
	{
		OpenWaitingAlert();
		m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	}

	int i;
	for(i=0; i<5; i++)
	{
		dwList.SetSize(i+1);
		dwList.SetAt(i, 0);
	}

	PrescanAutoMultiGrabDone(dwList, bDrawDie);

	OpPrescanInit(GetPrescanPrID());	// download again as normal 3 updated the setting

	if( bDrawDie==TRUE )
	{
		CloseWaitingAlert();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SetSrchThreadNumCmd(IPC_CServiceMessage& svMsg)
{
	LONG lThreadNum;

	svMsg.GetMsg(sizeof(LONG), &lThreadNum);

	m_pPrescanPrCtrl->ResumePR();
	m_pPrescanPrCtrl->RemoveAllShareImages();
	GetImageNumInGallery();

	m_lUsedSrchThreadNum = lThreadNum;
	m_lUsedSrchThreadNum = m_pPrescanPrCtrl->SetSrchThreadNum(m_lUsedSrchThreadNum);

	SavePrData(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferPr::WprSearchDie(CONST BOOL bNormalDie, CONST LONG lPrID, LONG &lDieOffsetX, LONG &lDieOffsetY)
{
	IPC_CServiceMessage stMsg;

	typedef struct {
		BOOL bStatus;
		BOOL bGoodDie;
		BOOL bFullDie;
		LONG lRefDieNo;
		LONG lX;
		LONG lY;
	} REF_TYPE;
	REF_TYPE	stInfo;

	typedef struct {
		BOOL		bShowPRStatus;
		BOOL		bNormalDie;
		LONG		lRefDieNo;
		BOOL		bDisableBackupAlign;
	} SRCH_TYPE;
	SRCH_TYPE	stSrchInfo;

	stSrchInfo.bShowPRStatus	= TRUE;
	stSrchInfo.bNormalDie		= bNormalDie;
	stSrchInfo.lRefDieNo		= lPrID;
	stSrchInfo.bDisableBackupAlign = TRUE;
	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

	BOOL bPLLMSpecialFcn = FALSE;
	if( bNormalDie==FALSE )	// if reference die, need large search window.
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		bPLLMSpecialFcn = pApp->IsPLLMRebel();
	}

	if( bPLLMSpecialFcn == FALSE )
	{
		SearchCurrentDie(stMsg);
	}
	else
	{
		SearchCurrentDie_PLLM_REBEL(stMsg);
	}

	stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
	if ((!stInfo.bStatus) || (!stInfo.bFullDie))		//If no GOOD/REF die is found
	{
		lDieOffsetX = 0;
		lDieOffsetY = 0;
		return FALSE;
	}
	else
	{
		lDieOffsetX = stInfo.lX;
		lDieOffsetY = stInfo.lY;
		return TRUE;
	}
}

BOOL CWaferPr::GetLFSearchWindow(LONG lDiffRow, LONG lDiffCol, BOOL bMxN, PR_WIN &stWnd)
{
	BOOL bOK = TRUE;
	stWnd.coCorner1.x = (PR_WORD)(m_stSearchArea.coCorner1.x + lDiffCol * GetPitchPixelXX() + lDiffRow * GetPitchPixelYX());
	stWnd.coCorner2.x = (PR_WORD)(m_stSearchArea.coCorner2.x + lDiffCol * GetPitchPixelXX() + lDiffRow * GetPitchPixelYX());
	stWnd.coCorner1.y = (PR_WORD)(m_stSearchArea.coCorner1.y + lDiffRow * GetPitchPixelYY() + lDiffCol * GetPitchPixelXY());
	stWnd.coCorner2.y = (PR_WORD)(m_stSearchArea.coCorner2.y + lDiffRow * GetPitchPixelYY() + lDiffCol * GetPitchPixelXY());

	if( bMxN )
	{
		bOK = VerifyPRRegion_MxN(&stWnd);
	}
	else
	{
		bOK = VerifyPRRegion(&stWnd);
	}
	return bOK;
}

//	zoomview	begin
int  CWaferPr::GetPitchPixelXX()
{
	int nDiePitchPixelYY = 0;
	//Calculate die pitch in PR pixel
	if (GetCalibX() != 0.0)	// divide by zero
	{
		nDiePitchPixelYY = abs((int)((DOUBLE)m_siDiePitchXX / GetCalibX()));
	}

	return nDiePitchPixelYY;
}

int	 CWaferPr::GetPitchPixelYY()
{
	int nDiePixelYY = 0;
	if (GetCalibY() != 0.0)	// divide by zero
	{
		nDiePixelYY = abs((int)((DOUBLE)m_siDiePitchYY / GetCalibY()));
	}
	return nDiePixelYY;
}

int  CWaferPr::GetPitchPixelXY()
{
	int nDiePitchPixelXY = 0;
	//Calculate die pitch in PR pixel
	if( GetCalibX()!=0.0 && GetDieShape()==WPR_RHOMBUS_DIE )	// divide by zero
	{
		nDiePitchPixelXY = abs((int)((DOUBLE)m_siDiePitchXY / GetCalibX()));
	}

	return nDiePitchPixelXY;
}
	//int			m_siDiePixelXX;				// Die Pitch X (X) PR Pixel
	//int			m_siDiePixelXY;				// Die Pitch X (Y) PR Pixel
	//int			m_siDiePixelYX;				// Die Pitch Y (X) PR Pixel
	//int			m_siDiePixelYY;				// Die Pitch Y (X) PR Pixel

int	 CWaferPr::GetPitchPixelYX()
{
	int nDiePixelYX = 0;
	if( GetCalibY()!=0.0 && GetDieShape()==WPR_RHOMBUS_DIE )	// divide by zero
	{
		nDiePixelYX = abs((int)((DOUBLE)m_siDiePitchYX / GetCalibY()));
	}
	return nDiePixelYX;
}

DOUBLE CWaferPr::GetCalibX()
{
	return m_dCalibX;
}

DOUBLE CWaferPr::GetCalibY()
{
	return m_dCalibY;
}

DOUBLE CWaferPr::GetCalibXY()
{
	return m_dCalibXY;
}
	
DOUBLE CWaferPr::GetCalibYX()
{
	return m_dCalibYX;
}

PR_WORD CWaferPr::GetNmlSizePixelX()
{
	return GetDieSizePixelX(WPR_NORMAL_DIE_1ST);
}

PR_WORD CWaferPr::GetNmlSizePixelY()
{
	return GetDieSizePixelY(WPR_NORMAL_DIE_1ST);
}
//	zoomview	end	done

LONG CWaferPr::CheckNormalDiePattern(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	if( m_lGenLrnAlignAlgo[0]==2 ) // edge matching
	{
		HmiMessage("Edge matching not suitable for die orientation check");
		bReturn = FALSE;
		m_bCheckDieOrientation = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

//v4.40T10
BOOL CWaferPr::AutoLoadScanAlignSettingFile(CONST CString szFileName, BOOL bDisplayMsg)
{
	if ((_access(szFileName, 0) ) == -1)
	{
		if (bDisplayMsg)
		{
			HmiMessage_Red_Back("WaferHole Select file can not access", "Scan Align Wafer");
		}
		return FALSE;
	}

	CStdioFile fScan;
	if( fScan.Open(szFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		if (bDisplayMsg)
		{
			HmiMessage_Red_Back("Select file can not be open", "Scan Align Wafer");
		}
		return FALSE;
	}

	BOOL bReturn = TRUE;
	m_ulScanAlignTotalPoints = 0;
	m_awPointsListRow.RemoveAll();
	m_awPointsListCol.RemoveAll();
	m_ucPointsListSta.RemoveAll();
	LONG lScanRow = 0, lScanCol = 0;
	fScan.SeekToBegin();
	CString szReading = "";
	CString szMsg = "", szTemp;
	while( fScan.ReadString(szReading)!=NULL )
	{
		CStringArray szAList;
		szAList.RemoveAll();
		CUtility::Instance()->ParseRawData(szReading, szAList);
		if( szAList.GetSize()>1 )
		{
			BYTE ucSta = 0;
			lScanRow = (LONG)atoi(szAList.GetAt(0));
			lScanCol = (LONG)atoi(szAList.GetAt(1));
			if( szAList.GetSize()>=3 )
				ucSta = (BYTE)atoi(szAList.GetAt(2));
			m_awPointsListRow.Add(lScanRow);
			m_awPointsListCol.Add(lScanCol);
			m_ucPointsListSta.Add(ucSta);
			szTemp.Format("[%d,%d][%d]", lScanRow, lScanCol, ucSta);
			szMsg += szTemp;
			m_ulScanAlignTotalPoints++;
		}
		if( m_ulScanAlignTotalPoints>500 )
			break;
	}
	szTemp.Format("Auto Load ScanAlign Setting File total points %d", m_ulScanAlignTotalPoints);
	szMsg += szTemp;
	SetAlarmLog(szMsg);
	fScan.Close();
	SavePrData(FALSE);

	bReturn = SortScanAlignHoleOrder();

	if (bDisplayMsg)
	{
		HmiMessage("File loading complete");
	}
	return bReturn;
}

LONG CWaferPr::LoadScanAlignSettingFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;

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

	//	dlgFile.GetFileName();	// filename with ext only, no folder information.
	CString szFilename = dlgFile.GetPathName();	// full path and name and ext. best.
	SaveScanTimeEvent("Load scan align file " + szFilename);
	if ((_access(szFilename, 0 )) == -1)
	{
		HmiMessage_Red_Back("Select file can not access!", "Scan Align Wafer");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	bReturn = AutoLoadScanAlignSettingFile(szFilename, TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::LoadSkipNgGrades(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	if( m_ucScanMapStartNgGrade!=255 || m_ucScanMapEndNgGrade!=255 )
	{
		HmiMessage("Please set the start and end grade to 255.\nBefore you load the setting file.", "Ng grade list");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
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
	if ((_access(szFilename, 0 )) == -1)
	{
		HmiMessage_Red_Back("Select file can not access!", "Ng grade list");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_ucScanMapNgListSize = 0;
	memset(m_ucScanMapNgList, 0, 255);
	CStdioFile fCheckFile;
	if( fCheckFile.Open(szFilename, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szReading;
		CStringArray szaDataList;
		fCheckFile.SeekToBegin();
		// get the fail counter limit
		if( fCheckFile.ReadString(szReading) )
		{
			szaDataList.RemoveAll();
			CUtility::Instance()->ParseRawData(szReading, szaDataList);
			m_ucScanMapNgListSize = (UCHAR)szaDataList.GetSize();
			for(LONG i = 0; i < (LONG)szaDataList.GetSize(); i++)
			{
				m_ucScanMapNgList[i] = (UCHAR)atoi(szaDataList.GetAt(i));
			}
		}
	}
	fCheckFile.Close();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ReconstructMapByFile(IPC_CServiceMessage &svMsg)
{
	WAF_CPhysicalInformationMap cFileMap;
	CDWordArray dwaReferWfX, dwaReferWfY, dwaReferRow, dwaReferCol;
	BOOL bDummyScan = 1;
	BOOL bDieOverLap = FALSE;
	BOOL bPreSorted = TRUE;
	ULONG ulDieSizeXTol = 0, ulDieSizeYTol = 0, ulDiePitchX = 0, ulDiePitchY = 0;
	ULONG ulRefDieCount = 0;
	int nMaxIterateCount = -1, nMaxAllowLeft = 10, nMaxSpan = 5;
	CString szGetOutFile;
	CString szSrcFile;
	FILE *fpGetOut = NULL;
	FILE *fpSrc = NULL;
	FILE *fpInput = NULL;
	LONG lAlignRow=0, lAlignCol=0, lAlignWfX=0, lAlignWfY=0;	//Klocwork	//v4.17T7
	ULONG ulPyiRowMax = 0, ulPyiColMax = 0;

	ULONG ulIndex;
	LONG lEncX, lEncY;
	ULONG ulRow, ulCol;
	LONG lIndex = 0;

	szSrcFile = "c:\\mapsorter\\userdata\\prescanresult\\Verify_SrcData.txt";
	errno_t nErr = fopen_s(&fpSrc, szSrcFile, "r");
	if ((nErr != 0) || (fpSrc == NULL))
	{
		HmiMessage("source data can not be open!");
		return 0;
	}
	nErr = fopen_s(&fpInput, "c:\\mapsorter\\userdata\\prescanresult\\Verify_PassIn.txt", "w");
	if ((nErr != 0) || (fpInput == NULL))		//Klocwork	//v4.71T7
	{
		fclose(fpSrc);
		HmiMessage("PassIn data can not be open!");
		return 0;
	}

	fscanf(fpSrc, "%d\n", &bDummyScan);
	fscanf(fpSrc, "%d\n", &ulRefDieCount);
	fprintf(fpInput, "%d\n", bDummyScan);
	fprintf(fpInput, "%lu\n", ulRefDieCount);

	ulRefDieCount = min(ulRefDieCount, 999);		//Klocwork

	for(ulIndex=0; ulIndex<ulRefDieCount; ulIndex++)
	{
		fscanf(fpSrc, "%ld,%ld,%ld,%ld\n", &lEncX, &lEncY, &ulRow, &ulCol);
		dwaReferWfX.Add(lEncX);
		dwaReferWfY.Add(lEncY);
		dwaReferRow.Add(ulRow);
		dwaReferCol.Add(ulCol);
		if( ulIndex==0 )
		{
			lAlignRow = ulRow;
			lAlignCol = ulCol;
			lAlignWfX = lEncX;
			lAlignWfY = lEncY;
		}
		fprintf(fpInput, "%ld,%ld,%ld,%ld\n", lEncX, lEncY, ulRow, ulCol);
	}
	fscanf(fpSrc, "%lu,%lu,%lu,%lu,%d,%d,%d,%d\n", 
			&ulDieSizeXTol, &ulDieSizeYTol,
			&ulDiePitchX, &ulDiePitchY,
			&bPreSorted, &nMaxIterateCount,
			&nMaxAllowLeft, &nMaxSpan);
	fprintf(fpInput, "%lu,%lu,%lu,%lu,%d,%d,%d,%d\n", 
			ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY, 
			bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);

	cFileMap.RemoveAll();

	ulDiePitchX = min(ulDiePitchX, 999999);		//Klocwork
	ulDiePitchY = min(ulDiePitchY, 999999);		//Klocwork


	// add all prescan die data list to physical map builder include refer points
	for(ulIndex=0; ulIndex<ulRefDieCount; ulIndex++)
	{
		lEncX = dwaReferWfX.GetAt(ulIndex);
		lEncY = dwaReferWfY.GetAt(ulIndex);
		cFileMap.SortAdd_Tail(lEncX, lEncY, 0-ulIndex, bDieOverLap);
		fprintf(fpInput, "%ld,%ld,%lu\n", lEncX, lEncY, 0-ulIndex);
	}

	// build physical map
	cFileMap.KeepIsolatedDice(bDummyScan);
	ClearWSPrescanInfo();	ClearPrescanInfo();
	while( feof(fpSrc)==0 )	// not end of file
	{
		fscanf(fpSrc, "%ld,%ld,%lu\n", &lEncX, &lEncY, &ulIndex);
		cFileMap.SortAdd_Tail(lEncX, lEncY, ulIndex, bDieOverLap);
		fprintf(fpInput, "%ld,%ld,%lu\n", lEncX, lEncY, ulIndex);
	}
	fclose(fpSrc);
	fclose(fpInput);

	LONG lMcnDiePitchX_X = GetDiePitchX_X(); 
	LONG lMcnDiePitchY_Y = GetDiePitchY_Y();
	(*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"] = (LONG)(ulDiePitchX); 
	(*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"] = (LONG)(ulDiePitchY);

	if( bDummyScan )
	{
		cFileMap.ConstructMapByPitchEx(ulDiePitchX, ulDiePitchY);
	}
	else
	{
		cFileMap.ConstructMap(dwaReferWfX, dwaReferWfY, dwaReferRow, dwaReferCol,
			ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY,
			bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);
	}

	cFileMap.GetDimension(ulPyiRowMax, ulPyiColMax);

	szGetOutFile = "c:\\mapsorter\\userdata\\prescanresult\\Verify_GetOut.txt";
	nErr = fopen_s(&fpGetOut, szGetOutFile, "w");

	if ((nErr == 0) && (fpGetOut != NULL))
	{
		fprintf(fpGetOut, "%lu,%lu\n", ulPyiRowMax, ulPyiColMax);
	}

	// get all phsical list infromation to prescan result list
	for(ulRow=0; ulRow<=ulPyiRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<=ulPyiColMax; ulCol++)
		{
			if( cFileMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap) )
			{
				if( lIndex<=WPR_ADD_DIE_INDEX )
					continue;

				if ((nErr == 0) && (fpGetOut != NULL))
				{
					fprintf(fpGetOut, "%lu,%lu,%ld,%ld\n", ulRow, ulCol, lEncX, lEncY);
				}
				if( bDummyScan )
				{
					if( labs(lAlignWfX-lEncX)<50 && labs(lAlignWfY-lEncY)<50 )
					{
						lAlignRow = ulRow;
						lAlignCol = ulCol;
					}
				}
				SetPrescanPosition(ulRow, ulCol, lEncX, lEncY);
			}
		}
	}

	if( bDummyScan )
	{
		if ((nErr == 0) && (fpGetOut != NULL))
		{
			fprintf(fpGetOut, "home die is %ld,%ld,%ld,%ld\n", lAlignRow, lAlignCol, lAlignWfX, lAlignWfY);
		}
	}

	WSClearScanRemainDieList();

	dwaReferWfX.RemoveAll();
	dwaReferWfY.RemoveAll();
	dwaReferRow.RemoveAll();
	dwaReferCol.RemoveAll();

	UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGoodGrade	= m_ucPrescanGoodGrade + ucOffset;
	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + ucOffset;

	BOOL  bEmptyAction	= m_bPrescanEmptyAction;
	BOOL  bGoodAction	= m_bPrescanGoodAction;

	if( m_bPrescanEmptyToNullBin )
		ucEmptyGrade = ucNullBin;

	ULONG ulMapRowMax = 0, ulMapColMax = 0;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);

	// transfer data from prescan result to map physical position array
	for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			BOOL bIsIgnore = FALSE;
			UCHAR ucCurGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucCurGrade==ucNullBin && bDummyScan==FALSE )
				bIsIgnore = TRUE;

			if( m_bPrescanEmptyToNullBin==FALSE && ucCurGrade==ucEmptyGrade )
				bIsIgnore = TRUE;

			if( bDummyScan )
				bIsIgnore = FALSE;

			BOOL bInfo = cFileMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if( lIndex<=WPR_ADD_DIE_INDEX )
				continue;

			if( bIsIgnore )
			{
				continue;
			}

			BOOL bIsRefer = m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol);
			BOOL bIsSpRef = IsSpecialReferGrade(ulRow, ulCol);
			BOOL bIsNoDie = IsNoDieGrade(ulRow, ulCol);
			if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_DIMREFER )
				bIsRefer = TRUE;

			if( bIsRefer || bIsSpRef || bIsNoDie )
			{
				continue;
			}

			if( bInfo )
			{
				if( bGoodAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucGoodGrade);
				}
			}
			else // set to empty grade
			{
				m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
				if( bEmptyAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
				}
				else
				{
					if( m_bPrescanEmptyMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						UpdateDie(ulRow, ulCol, ucCurGrade, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
					}
				}
			}
		}
	}
	ClearWSPrescanInfo();	ClearPrescanInfo();		WSClearScanRemainDieList();

	(*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"] = (LONG)(lMcnDiePitchX_X); 
	(*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"] = (LONG)(lMcnDiePitchY_Y);

	if ((nErr == 0) && (fpGetOut != NULL))
	{
		fclose(fpGetOut);
	}

	cFileMap.RemoveAll();
	HmiMessage("reconstruct prescan raw data and map file complete!");
	return 1;
}


// flush prescan pr summary for IM eagle 
VOID CWaferPr::GeneratePrSummaryHeader()
{
}

VOID CWaferPr::SendPrSummaryFileToEagle()
{
}

VOID CWaferPr::ConstructPrescanMap_Dummy2()
{
	WAF_CPhysicalInformationMap cDm2Map;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	ULONG ulIndex;
	BOOL bInfo;
	FILE *fpInOut = NULL;	//Klocwork
	CString szConLogFile;
	BOOL bDieOverLap = FALSE;
#define	EXTRA_DUMMY_EDGE	20
#define	DUMMY_SCAN2_LOOP	5
	UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGoodGrade	= m_ucPrescanGoodGrade + ucOffset;
	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + ucOffset;
	UCHAR ucDefectGrade	= GetScanDefectGrade() + ucOffset;
	UCHAR ucBadCutGrade	= GetScanBadcutGrade() + ucOffset;

	BOOL  bBadCutAction	= m_bPrescanBadCutAction;
	BOOL  bDefectAction	= m_bPrescanDefectAction;
	BOOL  bGoodAction	= m_bPrescanGoodAction;

	if( m_bPrescanDefectToNullBin )
		ucDefectGrade = ucNullBin;
	if (m_bPrescanBadCutToNullBin)
		ucBadCutGrade = ucNullBin;

	DOUBLE dScanSize = (DOUBLE)(*m_psmfSRam)["WaferTable"]["Dummy Prescan Area Size"];
	if( dScanSize==0 || m_bPrescanEmptyToNullBin )
		ucEmptyGrade = ucNullBin;
	if( IsBurnIn() )
		ucEmptyGrade = ucNullBin;
	LONG lEncX, lEncY;

	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);

	szConLogFile = szLogPath + PRESCAN_PASSIN_PSN;
	fpInOut = NULL;
	errno_t nErr = -1;
	if (pUtl->GetPrescanDebug())
	{
		nErr = fopen_s(&fpInOut, szConLogFile, "w");
	}

	LONG ulDiePitchX = labs(GetDiePitchX_X());
	LONG ulDiePitchY = labs(GetDiePitchY_Y());
	ULONG ulDieSizeXTol = (ULONG) (GetDieSizeX() * m_dPrescanPitchTolX / 100.0);
	ULONG ulDieSizeYTol = (ULONG) (GetDieSizeY() * m_dPrescanPitchTolY / 100.0);

	// add all alignment point and refer points to list
	cDm2Map.RemoveAll();

	ClearGoodInfo();

	// build physical map
	LONG lIndex = 0;
	int nMaxSpan = 10;	// nMaxAllowPitch
	BOOL bPreSorted = TRUE;
	int nMaxIterateCount = -1;

	cDm2Map.KeepIsolatedDice(FALSE);		//v4.08

	if ((nErr == 0) && (fpInOut != NULL))
	{
		fprintf(fpInOut, "%lu,%lu,%lu,%lu,%d,%d,%d,%d\n", 
			ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY, 
			bPreSorted, nMaxIterateCount, 0, nMaxSpan);
	}

	LONG lHomeWfX, lHomeWfY, lNewHomeRow, lNewHomeCol;
	// add all prescan die data list to physical map builder include refer points
	pUtl->GetAlignPosition(lNewHomeRow, lNewHomeCol, lHomeWfX, lHomeWfY);
	cDm2Map.SortAdd_Tail(lHomeWfX, lHomeWfY, 0, bDieOverLap);
	if ((nErr == 0) && (fpInOut != NULL))
	{
		fprintf(fpInOut, "0\n");
		fprintf(fpInOut, "1\n");
		fprintf(fpInOut, "%ld,%ld,%d,%d\n", lHomeWfX, lHomeWfY, 0, 0);
	}

	char *pcFindOut;
	pcFindOut = new char[WSGetPrescanTotalDie()+10];
	BOOL bFindAlnNearby = FALSE;

	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		pcFindOut[ulIndex] = 0;
		if( WSGetPrescanPosn(ulIndex, lEncX, lEncY) )
		{
			cDm2Map.SortAdd_Tail(lEncX, lEncY, ulIndex, bDieOverLap);
			if( fpInOut!=NULL )
				fprintf(fpInOut, "%ld,%ld,%ld\n", lEncX, lEncY, ulIndex);
			if( bFindAlnNearby == FALSE &&
				labs(lHomeWfX-lEncX)<=labs(ulDieSizeXTol+ulDiePitchX) && 
				labs(lHomeWfY-lEncY)<=labs(ulDieSizeYTol+ulDiePitchY) )
			{
				if( labs(lHomeWfX-lEncX)>labs(ulDieSizeXTol) &&
					labs(lHomeWfY-lEncY)>labs(ulDieSizeYTol) )
				{
					bFindAlnNearby = TRUE;
				}
			}
		}
	}

	double dDieAngle = 0;
	CString szDieBin = "0";
	ULONG ulFrameID = 0;
	PR_UWORD uwDiePrID = 0;
	CDWordArray dwaDm2WfX, dwaDm2WfY, dwaDm2Row, dwaDm2Col;

	ULONG ulDm2MapRowMax = 0, ulDm2MapColMax = 0;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulDm2MapRowMax, ulDm2MapColMax);
	ULONG ulDm2PyiRowMax = 0, ulDm2PyiColMax = 0;

	CString szMsg;
	szMsg.Format("WPR: home map (%d,%d)", lNewHomeRow, lNewHomeCol);
	SaveScanTimeEvent(szMsg);

	dwaDm2WfX.RemoveAll();
	dwaDm2WfY.RemoveAll();
	dwaDm2Row.RemoveAll();
	dwaDm2Col.RemoveAll();

	dwaDm2WfX.Add(lHomeWfX);
	dwaDm2WfY.Add(lHomeWfY);
	dwaDm2Row.Add(lNewHomeRow);
	dwaDm2Col.Add(lNewHomeCol);

	if( bFindAlnNearby==FALSE )
	{
		int i, j;
		for(i=0; i<3; i++)
		{
			for(j=0; j<3; j++)
			{
				if(i==1 && j==1 )
					continue;
				lEncX = lHomeWfX + (j-1)*(LONG)ulDiePitchX;
				lEncY = lHomeWfY + (i-1)*(LONG)ulDiePitchY;
				lIndex = WPR_ADD_DIE_INDEX - i*3 - j;
				cDm2Map.SortAdd_Tail(lEncX, lEncY, lIndex, bDieOverLap);
				if ((nErr == 0) && (fpInOut != NULL))
				{
					fprintf(fpInOut, "%ld,%ld,%ld\n", lEncX, lEncY, lIndex);
				}
			}
		}
	}

	if ((nErr == 0) && (fpInOut != NULL))
	{
		fclose(fpInOut);
	}

	SaveScanTimeEvent("WPR: to construct dummy scan2 map");
	cDm2Map.ConstructMap(dwaDm2WfX, dwaDm2WfY, dwaDm2Row, dwaDm2Col,
		ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY,
		bPreSorted, nMaxIterateCount, 10, nMaxSpan);

	SaveScanTimeEvent("WPR: construct dummy scan2 map complete");

	ClearPrescanInfo();

	cDm2Map.GetDimension(ulDm2PyiRowMax, ulDm2PyiColMax);
	if( ulDm2PyiRowMax>ulDm2MapRowMax )
		ulDm2PyiRowMax = ulDm2MapRowMax;
	if( ulDm2PyiColMax>ulDm2MapColMax )
		ulDm2PyiColMax = ulDm2MapColMax;

	SaveScanTimeEvent("WPR: to get out dummy2 map posn");

	ULONG ulRow, ulCol;
	for(ulRow=0; ulRow<=ulDm2PyiRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<=ulDm2PyiColMax; ulCol++)
		{
			bInfo = cDm2Map.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if( bInfo==FALSE )
			{
				continue;
			}
			if( lIndex<=WPR_ADD_DIE_INDEX )
				continue;
			if( lIndex>=0 )
			{
				pcFindOut[lIndex] = 1;
			}

			USHORT usDieState = 0;
			if( lIndex>0 )
			{
				LONG lDummyX = 0, lDummyY = 0;
				WSGetScanPosition(lIndex, lDummyX, lDummyY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulFrameID);
			}
			else
			{
				ulFrameID = 0;
			}

			SetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
		}
	}

	SaveScanTimeEvent("WPR: to get out dummy2 map remain");
	for(int i=1; i<4; i++)
	{
		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if( pcFindOut[ulIndex]>0 )
			{
				continue;
			}

			USHORT usDieState = 0;
			if( WSGetScanPosition(ulIndex, lEncX, lEncY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulFrameID)==FALSE )
			{
				continue;
			}

			LONG lNewRow, lNewCol, lNewX, lNewY;
			lNewCol = (lHomeWfX - lEncX)/ulDiePitchX + lNewHomeCol;
			lNewRow = (lHomeWfY - lEncY)/ulDiePitchY + lNewHomeRow;
			if( CheckDieAssignedAround(lNewRow, lNewCol, DUMMY_SCAN2_LOOP, lEncX, lEncY, FALSE) )
			{
				pcFindOut[ulIndex] = 2;
				continue;
			}	// already assigned nearby, skip

			if( GetDieValidPrescanPosn(lNewRow, lNewCol, DUMMY_SCAN2_LOOP, lNewX, lNewY, FALSE) )
			{
				lNewCol = (lNewX - lEncX)/ulDiePitchX + lNewCol;
				lNewRow = (lNewY - lEncY)/ulDiePitchY + lNewRow;
				if( CheckDieAssignedAround(lNewRow, lNewCol, DUMMY_SCAN2_LOOP, lEncX, lEncY, FALSE) )
				{
					pcFindOut[ulIndex] = 2;
					continue;
				} // assigned nearby continue
			}

			if( i==1 )
			{
				if( GetDieValidPrescanPosn(lNewRow, lNewCol, 0, lNewX, lNewY, FALSE) )
				{
					continue;
				}
			}
			else
			{
				if( GetDieInvalidNearMapPosn(lNewRow, lNewCol, DUMMY_SCAN2_LOOP*i, lNewRow, lNewCol, FALSE)==FALSE )
				{
					continue;
				}
			}

			SetScanInfo(lNewRow, lNewCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
			pcFindOut[ulIndex] = 3;
		}
	}

	SaveScanTimeEvent("WPR: get out dummy scan2 complete");

	cDm2Map.RemoveAll();
	dwaDm2WfX.RemoveAll();
	dwaDm2WfY.RemoveAll();
	dwaDm2Row.RemoveAll();
	dwaDm2Col.RemoveAll();

	fpInOut = NULL;
	nErr = -1;
	if( pUtl->GetPrescanDebug() )
	{
		szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
		nErr = fopen_s(&fpInOut, szConLogFile, "w");
	}

	if ((nErr == 0) && (fpInOut != NULL))
	{
		SaveScanTimeEvent("WPR: log dummy scan2 result begin");
		fprintf(fpInOut, "PHY range (%lu,%lu)\n", ulDm2PyiRowMax, ulDm2PyiColMax);
		fprintf(fpInOut, "MAP range (%lu,%lu)\n", ulDm2MapRowMax, ulDm2MapColMax);
		// physical map sorted list
		fprintf(fpInOut, "\n");
		fprintf(fpInOut, "SORTED DIE LIST\n");
		lIndex = 0;
		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if( pcFindOut[ulIndex]!=1 )
			{
				continue;
			}
			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY) )
			{
				lIndex++;
				fprintf(fpInOut, "%ld,%lu,%ld,%ld\n", lIndex, ulIndex, lEncX, lEncY);
			}
		}
		// already assigned list, duplicated
		fprintf(fpInOut, "\n");
		fprintf(fpInOut, "ASSIGNED DIE LIST\n");
		lIndex = 0;
		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if( pcFindOut[ulIndex]!=2 )
			{
				continue;
			}
			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY) )
			{
				lIndex++;
				fprintf(fpInOut, "%ld,%lu,%ld,%ld\n", lIndex, ulIndex, lEncX, lEncY);
			}
		}
		// INSERTED list, maybe not exactly right position
		fprintf(fpInOut, "\n");
		fprintf(fpInOut, "INSERT DIE LIST\n");
		lIndex = 0;
		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if( pcFindOut[ulIndex]!=3 )
			{
				continue;
			}
			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY) )
			{
				lIndex++;
				fprintf(fpInOut, "%ld,%lu,%ld,%ld\n", lIndex, ulIndex, lEncX, lEncY);
			}
		}
		// LEFT list, no position can be inserted
		fprintf(fpInOut, "\n");
		fprintf(fpInOut, "LEFT DIE LIST\n");
		lIndex = 0;
		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if( pcFindOut[ulIndex]!=0 )
			{
				continue;
			}
			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY) )
			{
				lIndex++;
				fprintf(fpInOut, "%ld,%lu,%ld,%ld\n", lIndex, ulIndex, lEncX, lEncY);
			}
		}

		fclose(fpInOut);
		SaveScanTimeEvent("WPR: log dummy scan2 result end");
	}

	ULONG ulDefectIdx = 0, ulBadCutIdx = 0, ulGoodIdx = 0;
	ULONG ulTotal = 0;

	SaveScanTimeEvent("WPR: to update dummy 2 map");

	// transfer data from prescan result to map physical position array
	for(ulRow=0; ulRow<ulDm2MapRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<ulDm2MapColMax; ulCol++)
		{
			UCHAR ucCurGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucCurGrade==ucNullBin )
				continue;

			BOOL bIsBadCut = FALSE, bIsDefect = FALSE;
			bInfo = GetPrescanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, bIsDefect, bIsBadCut);
			if( bInfo==FALSE )
			{
				if( ucCurGrade!=ucNullBin )
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
				continue;
			}

			ulTotal++;
			SetMapPhyPosn(ulRow, ulCol, lEncX, lEncY);

			if (bIsBadCut)
			{
				SetBadCutFull(ulBadCutIdx, ulRow, ulCol, ucCurGrade);
				ulBadCutIdx++;
				m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT);
				if( bBadCutAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucBadCutGrade);
				}
				else
				{
					if( m_bPrescanBadCutMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
					}
				}
			}
			else if( bIsDefect )
			{
				SetDefectFull(ulDefectIdx, ulRow, ulCol, ucCurGrade);
				ulDefectIdx++;
				m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT);
				if( bDefectAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucDefectGrade);
				}
				else
				{
					if( m_bPrescanDefectMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
					}
				}
			}
			else
			{
				ulGoodIdx++;
				if( bGoodAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucGoodGrade);
				}
			}
		}
	}

	pUtl->SetAlignPosition(lNewHomeRow, lNewHomeCol, lHomeWfX, lHomeWfY);

	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulGoodIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulDefectIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulBadCutIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA]	= 0;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= 0;

	SetMapTotalDie(ulTotal);

	// build prescan map complete
	ClearGoodInfo();
	ClearWSPrescanInfo();
	delete [] pcFindOut;

	SaveScanTimeEvent("WPR: all dummy scan2 complete");
}	// Dummy2()


LONG CWaferPr::RectWaferAroundDieCheck()
{	// remember to call the down load multi-search ID command before call this function
	//
	//		10
	//	1	10K	100
	//		1000

	if( IsLFSizeOK()==FALSE )
		return 0;

	LONG lFindResult = 0;
	int i;
	CDWordArray dwList;
	for(i=0; i<5; i++)
	{
		dwList.SetSize(i+1);
		dwList.SetAt(i, 0);
	}

	ClearGoodInfo();

	PrescanAutoMultiGrabDone(dwList, FALSE);

	if( dwList.GetAt(0)!=0 )
		lFindResult += 1;
	if( dwList.GetAt(1)!=0 )
		lFindResult += 10;
	if( dwList.GetAt(2)!=0 )
		lFindResult += 100;
	if( dwList.GetAt(3)!=0 )
		lFindResult += 1000;
	if( dwList.GetAt(4)!=0 )
		lFindResult += 10000;

	return lFindResult;
}

BOOL CWaferPr::ScanDieIsDefect(PR_UWORD usAlignType, PR_UWORD usInspType)
{
	return (usAlignType==PR_ERR_LOCATED_DEFECTIVE_DIE) || DieIsDefective(usInspType) || DieIsInk(usInspType) || DieIsChip(usInspType);
}


INT	CWaferPr::PrescanNormalInit(UCHAR ucDieNo, BOOL bInspect)	//	no angle limit by Scan Accept setting
{
	SaveScanTimeEvent("WPR: init prescan normal PR, resume pr");
	m_pPrescanPrCtrl->ResumePR();
	if( IsThisDieLearnt(ucDieNo-1)!=TRUE )
	{
		CString szMsg;
		szMsg.Format("Normal die PR %d not learnt yet!", ucDieNo);
		HmiMessage_Red_Back(szMsg);
		return gnOK;
	}

	remove(gszUSER_DIRECTORY + "\\History\\FovSearch_Map.txt");

	SaveScanTimeEvent("WPR: init prescan normal PR, setup multi search die cmd");
	SetupMultiSearchDieCmd(ucDieNo, TRUE, bInspect, FALSE);

	CString szMsg;
	szMsg.Format("WPR: init prescan normal PR %d, setup multi search die cmd inspec=%d done", ucDieNo, bInspect);
	SaveScanTimeEvent(szMsg);

	return gnOK;
}

BOOL CWaferPr::GetDieValidInY(LONG ulIntRow, LONG ulIntCol, LONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap)
{
	LONG lTryRow = ulIntRow;
	LONG lTryCol = ulIntCol;
	if( GetWftPosn(bByMap, lTryRow, lTryCol, lPhyX, lPhyY) )
	{
		return TRUE;
	}

	LONG ulCounter = 1;
	while( 1 )
	{
		if( ulLoop>0 )
			lTryRow++;
		else
			lTryRow--;

		if( lTryRow>=0 && lTryCol>=0 )
		{
			if( GetWftPosn(bByMap, lTryRow, lTryCol, lPhyX, lPhyY) )
			{
				break;
			}
		}
		ulCounter++;
		if( ulCounter>labs(ulLoop) )
		{
			break;
		}
	}

	BOOL bFound = FALSE;
	if ( ulCounter<=labs(ulLoop) && lTryRow>=0 && lTryCol>=0 )
	{
		//Calculate original no grade die pos from surrounding die position
		if( GetWftPosn(bByMap, lTryRow, lTryCol, lPhyX, lPhyY) )
		{
			LONG lDiePitchY_Y = GetDiePitchY_Y(); 
			LONG lDiePitchY_X = GetDiePitchY_X();

			LONG lDiff_Y = ulIntRow - lTryRow;
			lPhyX = lPhyX - lDiff_Y * lDiePitchY_X;
			lPhyY = lPhyY - lDiff_Y * lDiePitchY_Y;
			bFound = TRUE;
		}
	}

	return bFound;
}

BOOL CWaferPr::FindAndFillFromRemainList(LONG ulRow, LONG ulCol, LONG lTolX, LONG lTolY, LONG &lEncX, LONG &lEncY, PR_UWORD &uwDiePrID)
{
	LONG  lNewWftX = 0, lNewWftY = 0;

	for(ULONG ulIndex=1; ulIndex<=WSGetScanRemainDieTotal(); ulIndex++)
	{
		ULONG ulWSIndex = 0;
		if( WSGetScanRemainDieFromList(ulIndex,  lNewWftX, lNewWftY, ulWSIndex) )
		{
			if( labs(lNewWftX-lEncX)<lTolX && labs(lNewWftY-lEncY)<lTolY )
			{
				USHORT usDieState = 0;
				DOUBLE dDieAngle = 0;
				CString szDieBin = "";
				ULONG ulFrameID = 0;
				WSGetScanPosition(ulWSIndex, lNewWftX, lNewWftY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulFrameID);
				BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
				DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);
				SetScanInfo(ulRow, ulCol, lNewWftX, lNewWftY, dDieAngle, szDieBin, usDieState);
				SetPrescanFrameInfo(ulRow, ulCol, ulFrameID);
				lEncX = lNewWftX;
				lEncY = lNewWftY;

				FILE *fpOut = NULL;
				errno_t nErr = -1;
				CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
				CString szLogPath;
				pUtl->GetPrescanLogPath(szLogPath);
				CString szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
				if( pUtl->GetPrescanDebug() )
				{
					nErr = fopen_s(&fpOut, szConLogFile, "a");
				}

				if ((nErr == 0) && (fpOut != NULL))
				{
					fprintf(fpOut, "%ld,%ld,%lu,%lu,%ld,%ld,FindAndFillInRemain %lu\n",
						ulRow, ulCol, ulRow, ulCol, lNewWftX, lNewWftY, ulWSIndex);
					fclose(fpOut);
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}

VOID CWaferPr::FillUpBarWaferMap(LONG lStartRow, LONG lEndRow, LONG lStartCol, LONG lEndCol, BOOL bAlnToDown, BOOL bCtrToLeft, LONG lTolX, LONG lTolY)
{
	LONG ulRow = lStartRow;
	BOOL bFirstRow = TRUE;
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	INT nMaxLoop = 5;
	LONG lEncX = 0, lEncY = 0;

	if( bAlnToDown )
		nMaxLoop = -5;

	while( 1 )
	{
		// LOOP down or up by row
		if( bFirstRow==FALSE )
		{
			if( bAlnToDown )
			{
				ulRow++;
				if( ulRow>lEndRow )
				{
					break;
				}
			}
			else
			{
				ulRow--;
				if( ulRow<lEndRow )
				{
					break;
				}
			}
		}
		bFirstRow = FALSE;

		// find a good sorted die in this row
		LONG lFirstDieCol = lStartCol;
		BOOL bFirstCol = TRUE, bFindDieInRow = FALSE;
		LONG ulCol = lStartCol;
		while( 1 )
		{
			if( bFirstCol==FALSE )
			{
				if( bCtrToLeft )
				{
					ulCol--;
					if( ulCol<lEndCol )
					{
						break;
					}
				}
				else
				{
					ulCol++;
					if( ulCol>lEndCol )
					{
						break;
					}
				}
			}
			bFirstCol = FALSE;

			if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow, ulCol))
			{
				continue;
			}

			if( GetPrescanPosition(ulRow, ulCol, lEncX, lEncY) )
			{
				lFirstDieCol = ulCol;
				bFindDieInRow = TRUE;
				break;
			}
		}

		// try to locate a nearby die by neighbour row die
		if( bFindDieInRow==FALSE )
		{
			BOOL bFirstCol = TRUE;
			LONG ulCol = lStartCol;
			while( 1 )
			{
				if( bFirstCol==FALSE )
				{
					if( bCtrToLeft )
					{
						ulCol--;
						if( ulCol<lEndCol )
						{
							break;
						}
					}
					else
					{
						ulCol++;
						if( ulCol>lEndCol )
						{
							break;
						}
					}
				}
				bFirstCol = FALSE;

				if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow, ulCol))
				{
					continue;
				}

				BOOL bGetPred = GetDieValidInY(ulRow, ulCol, nMaxLoop, lEncX, lEncY, FALSE);
				if( bGetPred )
				{
					PR_UWORD uwDiePrID = 0;
					if( FindAndFillFromRemainList(ulRow, ulCol, lTolX, lTolY, lEncX, lEncY, uwDiePrID) )
					{
						bFindDieInRow = TRUE;
					}
				}

				if( bFindDieInRow )
				{
					lFirstDieCol = ulCol;
					break;
				}
			}
		}

		// no any die position in this row, continue to next row
		if( bFindDieInRow==FALSE )
		{
			continue;
		}

		// try to locate other in this row, if left part, to right; if right part, to left.
		// xxxxxxxxxx*xxxxxCTRxxxxx*xxxxxxxxxx
		for(int kkk=0; kkk<2; kkk++)
		{
			bFirstCol = TRUE;
			ulCol = lFirstDieCol;
			LONG lXSpan = 5;

			while( 1 )
			{
				if( kkk==0 )
				{
					if( bCtrToLeft )
						lXSpan = -5;
					else
						lXSpan = 5;
					if( bFirstCol==FALSE )
					{
						if( bCtrToLeft )
						{
							ulCol++;
							if( ulCol>lStartCol )
							{
								break;
							}
						}
						else
						{
							ulCol--;
							if( ulCol<lStartCol )
							{
								break;
							}
						}
					}
					bFirstCol = FALSE;
				}
				else
				{
					if( bCtrToLeft )
					{
						lXSpan = 5;
						ulCol--;
						if( ulCol<lEndCol )
						{
							break;
						}
					}
					else
					{
						lXSpan = -5;
						ulCol++;
						if( ulCol>lEndCol )
						{
							break;
						}
					}
				}

				if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow, ulCol))
				{
					continue;
				}

				BOOL bInfo = GetPrescanPosition(ulRow, ulCol, lEncX, lEncY);

				BOOL bSecond = FALSE;
				if( bInfo==FALSE )	// BGETREMAIN
				{
					bSecond = TRUE;
					BOOL bGetPred = GetDieValidInX(ulRow, ulCol, lXSpan, lEncX, lEncY, FALSE);
					if( bGetPred )
					{
						PR_UWORD uwDiePrID = 0;
						if( FindAndFillFromRemainList(ulRow, ulCol, lTolX, lTolY, lEncX, lEncY, uwDiePrID) )
						{
							bInfo = TRUE;
							SetMapPhyPosn(ulRow, ulCol, lEncX, lEncY);
							if( IsPrescanReferDie() && uwDiePrID!=m_uwIMMergeRecordID[0] )
							{
								m_WaferMapWrapper.SetReferenceDie(ulRow, ulCol, TRUE);
							}
						}
					}
				}	// BGETREMAIN

				if( bInfo )
				{
					SetMapPhyPosn(ulRow, ulCol, lEncX, lEncY);
					if( bSecond )
					{
					//	m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
					//	m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, 100+ucOffset);
					}
				}
			}	// while for local column loop
		}	// while for column loop in both directions
	}	// while for row loop
}


VOID CWaferPr::AddPrescanEdge()
{
	UCHAR ucPrescanEdgeSizeX = m_ucPrescanEdgeSizeX;
	UCHAR ucPrescanEdgeSizeY = m_ucPrescanEdgeSizeY;
	if( ucPrescanEdgeSizeX==0 && ucPrescanEdgeSizeY==0 )
	{
		return ;
	}

	ucPrescanEdgeSizeY = ucPrescanEdgeSizeX;
	SaveScanTimeEvent("WPR: add prescan edge die");
	if( m_dEdgeGoodScore>0.0 )
	{
		return CheckPrescanEdgeDie();
	}

	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucNewGrade = 0;
	if( m_ucPrescanEdgeGrade>0 )
		ucNewGrade = m_ucPrescanEdgeGrade + ucOffset;
	UCHAR ucDieGrade = ucNullBin;
	ULONG ulRow = 0, ulCol = 0;
	ULONG ulMapRowMax, ulMapColMax;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);
	CString szMsg;
	for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)	// map edge
	{
		ULONG ulDieState;
		LONG lFirstGoodCol = -1, lLastGoodCol = -1;
		for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
			if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow, ulCol) && ulDieState!=WT_MAP_DIESTATE_SCAN_EDGE)
			{
				continue;
			}

			if( lFirstGoodCol==-1 )
				lFirstGoodCol = (LONG)ulCol;
			lLastGoodCol = (LONG)ulCol;
		}
		if( lFirstGoodCol>-1 && lLastGoodCol>-1 )
		{
			UCHAR ucLoop = 0;
			for(ucLoop=0; ucLoop<ucPrescanEdgeSizeX; ucLoop++)
			{
				ulCol = lFirstGoodCol + ucLoop;
				ucDieGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
				ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
				if( ulCol<=(ULONG)lLastGoodCol && ulDieState!=WT_MAP_DIESTATE_SCAN_EDGE )
				{
					if( ucDieGrade!=ucNullBin )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						if( ucNewGrade>0 )
						{
							m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNewGrade);
							ucDieGrade = ucNewGrade;
						}
						UpdateDie(ulRow, ulCol, ucDieGrade, WT_MAP_DIESTATE_SCAN_EDGE);
					}
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_SCAN_EDGE);
				}
				if( lLastGoodCol>=ucLoop )
				{
					ulCol = lLastGoodCol - ucLoop;
					ucDieGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
					ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
					if( ulCol>=(ULONG)lFirstGoodCol && ulDieState!=WT_MAP_DIESTATE_SCAN_EDGE )
					{
						if( ucDieGrade!=ucNullBin )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
							if( ucNewGrade>0 )
							{
								m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNewGrade);
								ucDieGrade = ucNewGrade;
							}
							UpdateDie(ulRow, ulCol, ucDieGrade, WT_MAP_DIESTATE_SCAN_EDGE);
						}
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_SCAN_EDGE);
					}
				}
			}
		}
	}

	for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
	{
		ULONG ulDieState;
		LONG lFirstGoodRow = -1, lLastGoodRow = -1;
		for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)	// map edge
		{
			ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
			if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow, ulCol) && ulDieState!=WT_MAP_DIESTATE_SCAN_EDGE)
			{
				continue;
			}
			if( lFirstGoodRow==-1 )
				lFirstGoodRow = (LONG)ulRow;
			lLastGoodRow = (LONG)ulRow;
		}
		if( lFirstGoodRow>-1 && lLastGoodRow>-1 )
		{
			UCHAR ucLoop = 0;
			for(ucLoop=0; ucLoop<ucPrescanEdgeSizeY; ucLoop++)
			{
				ulRow = lFirstGoodRow + ucLoop;
				ucDieGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
				ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
				if( ulRow<=(ULONG)lLastGoodRow && ulDieState!=WT_MAP_DIESTATE_SCAN_EDGE )
				{
					if( ucDieGrade!=ucNullBin )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						if( ucNewGrade>0 )
						{
							m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNewGrade);
							ucDieGrade = ucNewGrade;
						}
						UpdateDie(ulRow, ulCol, ucDieGrade, WT_MAP_DIESTATE_SCAN_EDGE);
					}
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_SCAN_EDGE);
				}
				if( lLastGoodRow>=ucLoop )
				{
					ulRow = lLastGoodRow - ucLoop;
					ucDieGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
					ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
					if( ulRow>=(ULONG)lFirstGoodRow && ulDieState!=WT_MAP_DIESTATE_SCAN_EDGE )
					{
						if( ucDieGrade!=ucNullBin )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
							if( ucNewGrade>0 )
							{
								m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNewGrade);
								ucDieGrade = ucNewGrade;
							}
							UpdateDie(ulRow, ulCol, ucDieGrade, WT_MAP_DIESTATE_SCAN_EDGE);
						}
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_SCAN_EDGE);
					}
				}
			}
		}
	}
}

LONG CWaferPr::WprDumpLog(IPC_CServiceMessage &svMsg)
{
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_DumpCommLog(ubSID, ubRID);

	return 1;
}


//=======================================================================================================================
//Save Scanning die information to cPIMap list
//=======================================================================================================================
VOID CWaferPr::SetScanPosition(LONG nDieX, LONG nDieY, DOUBLE dDieAngle, CString szDieBin, bool bIsDefect, bool bIsBadCut, bool bIsFakeEmpty, ULONG ulImageID, USHORT uwDiePrID)
{
	USHORT usDieState = EncodeDieState(bIsDefect, bIsBadCut, bIsFakeEmpty);
	WSSetScanPosition(nDieX, nDieY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulImageID);
	if( IsPrecanWith2Pr() )	//	not update to PIMap
	{
		if( uwDiePrID==m_uwScanRecordID[1] && IsPrescanReferDie() )
			WSSetScanReferPosition(nDieX, nDieY);
	}
	else
	{
		ULONG ulIndex = WSGetPrescanTotalDie();
		m_cPIMap.SortAdd_Tail(nDieX, nDieY, ulIndex, FALSE);
	}
}
//===========================================================================================================================

LONG CWaferPr::ToggleFailureCaselog(IPC_CServiceMessage &svMsg)
{
	CPRFailureCaseLog::Instance()->ToggleFailureCaselog();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::ToggleBHMarkCaselog(IPC_CServiceMessage &svMsg)
{
	CPRFailureCaseLog::Instance()->ToggleBHMarkCaselog();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SetDebugViewerLog(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		LONG lType;
		BOOL bEnable;
	} LOGTYPE;

	LOGTYPE stLog;

	svMsg.GetMsg(sizeof(LOGTYPE), &stLog);

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	BOOL bEnable = stLog.bEnable;
	switch( stLog.lType )
	{
	case 3:	// thread of vision
		pUtl->SetVisionThrdLog(bEnable);
		break;
	case 2:	// control of vision
		pUtl->SetVisionCtrlLog(bEnable);
		break;
	case 1:	// blk2 debug
		pUtl->SetBlk2DebugLog(bEnable);
		break;
	case 0:	// auto cycle
	default:
		pUtl->SetAutoCycleDebugLog(bEnable);
		break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	// debug viewer output part selection

LONG CWaferPr::SetPrDebugFlag(IPC_CServiceMessage &svMsg)
{
	LONG lMode = 0;
	svMsg.GetMsg(sizeof(LONG), &lMode);
	PR_SetDebugFlag((PR_DEBUG)lMode);

	CString szMsg;
	szMsg.Format("WFR: rescan aoi wafer PR log %d", lMode);
	SaveScanTimeEvent(szMsg);

	return 1;
}


BOOL CWaferPr::ScanCheckFivePoints(BOOL bFirstTime)
{
	if( IsEnableFPC()==FALSE )
	{
		return TRUE;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDummyMap() )
	{
		return TRUE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	INT nWprDoFPC = pApp->GetProfileInt(gszPROFILE_SETTING, gszWprDoFPC, 0);
	if( nWprDoFPC!=1)
	{
		return TRUE;
	}

	IPC_CServiceMessage stMsg;
	if( bFirstTime )
	{
		GotoFocusLevel(stMsg);
		MoveBackLightWithTableCheck(TRUE);
	}

	SaveScanTimeEvent("WPR: to do five point check");
	AutoBondScreen(FALSE);
	LONG lEncX, lEncY;
	WprSearchDie(TRUE, 1, lEncX, lEncY);

	BOOL bCheckDieResult = WprFivePointCheckDiePosition();

	AutoBondScreen(TRUE);
	WprSearchDie(TRUE, 1, lEncX, lEncY);

	if( bFirstTime && bCheckDieResult )
	{
		MoveBackLightWithTableCheck(FALSE);
	}

	return bCheckDieResult;
}

BOOL CWaferPr::WprFivePointCheckDiePosition(VOID)
{
	LONG	lPhyX = 0;
	LONG	lPhyY = 0;
	LONG	lHmiStatus = 0;
	ULONG	ulIntRow = 0, ulIntCol = 0;
	CString	szText;
	CString szMsg;
	CString szTitle, szContent;
	BOOL	bPromptMessage = TRUE;
	BOOL	bFiveResult = TRUE;
	BOOL	bDiePrResult = TRUE;
	LONG	lScnCheckRow, lScnCheckCol;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	szTitle.LoadString(HMB_WT_CHECK_SCN_FUNC);

	// validate the map file, check wether it is a null bin for home die
	UCHAR ucReferBin = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
	UCHAR	ucNullBin = m_WaferMapWrapper.GetNullBin();

	for (LONG i=1; i<=m_lTotalSCNCount; i++)
	{
		bPromptMessage = TRUE;	
		lHmiStatus = glHMI_CONTINUE;
		i = min(i, WT_ALN_MAXCHECK_SCN-1);		//Klocwork	//v4.02T5

		// Always clear the flag (V3.26)
		if( GetFPCMapPositionWpr(i, ulIntRow, ulIntCol, lScnCheckRow, lScnCheckCol)==FALSE )
		{
			szMsg.Format("Five Point Check %d row %ld, col %ld, out of map range", i, lScnCheckRow, lScnCheckCol);
			FivePointCheckLog(szMsg);
			if (GetErrorChooseGoFPC() )
			{
				lHmiStatus = HmiMessage_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP|0x80000000, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			}
			else
			{
				HmiMessage_Red_Back(szMsg, szTitle);
				return FALSE;
			}
			if( lHmiStatus!=glHMI_CONTINUE )
			{
				return FALSE;
			}
		}

		//Call setcurrent pos twice because HMI need time to update
		m_WaferMapWrapper.SetCurrentPosition(ulIntRow, ulIntCol);
		Sleep(300);
		m_WaferMapWrapper.SetCurrentPosition(ulIntRow, ulIntCol);
		Sleep(600);

		szMsg.Format("%ld point %ld,%ld ==> %ld,%ld\n", i, lScnCheckRow, lScnCheckCol, ulIntRow, ulIntCol);
		FivePointCheckLog(szMsg);

		if (m_bKeepSCNAlignDie == TRUE)
		{
			//Mark die & set die state to unpick mode	
			m_WaferMapWrapper.MarkDie(ulIntRow, ulIntCol, TRUE);
			ULONG ulDieState = m_WaferMapWrapper.GetDieState(ulIntRow, ulIntCol);
			if( !IsDieUnpickAll(ulDieState) )
				ulDieState = WT_MAP_DIESTATE_UNPICK;
			m_WaferMapWrapper.SetDieState(ulIntRow, ulIntCol, ulDieState);
		}

		//If no grade on select die pos, try to use surrounding die
		if ( WprGetDieValidPrescanPosn(ulIntRow, ulIntCol, 3, lPhyX, lPhyY) )
		{
			szMsg.Format("point %ld, wft(%ld,%ld)\n", i, lPhyX, lPhyY);		//Klocwork
			FivePointCheckLog(szMsg);
			if( MoveWaferTable(lPhyX, lPhyY)==FALSE )
			{
				szText.Format("Point %d (%ld,%ld), table position (%ld,%ld), out of wafer limit",
					i, lScnCheckRow, lScnCheckCol, lPhyX, lPhyY); 
				FivePointCheckLog(szText);

				lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

				bFiveResult = FALSE;
				break;
			}
			Sleep(20);

			bDiePrResult = WprScnCheckDiePrResult(lScnCheckRow, lScnCheckCol, bPromptMessage, szText);
			if( m_bScnCheckByPR )
			{
				(*m_psmfSRam)["MS896A"]["WaferMapRow"] = ulIntRow;
				(*m_psmfSRam)["MS896A"]["WaferMapCol"] = ulIntCol;
				(*m_psmfSRam)["MS896A"]["WaferTableX"] = lPhyX;
				(*m_psmfSRam)["MS896A"]["WaferTableY"] = lPhyY;

				if( m_bScnCheckIsRefDie==FALSE )
				{
					GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_5PC);	// FIVE POINT CHECK
				}
				else
				{
					GrabAndSaveImage(1, m_lScnCheckRefDieNo, WPR_GRAB_SAVE_IMG_5PC);	// FIVE POINT CHECK
				}
			}

			if( bDiePrResult==FALSE )
			{
				szMsg.Format("Five Point Check die (%ld,%ld) PR result fail", lScnCheckRow, lScnCheckCol);
				FivePointCheckLog(szMsg);
				bFiveResult = FALSE;
                break;
			}

			int nCurrX, nCurrY, nCurrT;
			GetWaferTableEncoder(&nCurrX, &nCurrY, &nCurrT);
			if( IsVerifyMapWaferEnable()==FALSE )
			{
				SetMapPhyPosn(ulIntRow, ulIntCol, nCurrX, nCurrY);
			}
			szMsg.Format("point %ld, update table %ld,%ld\n", i, nCurrX, nCurrY);
			FivePointCheckLog(szMsg);

			//Ask user to continue or stop
			if ( bPromptMessage == TRUE )
			{
				if ( GetErrorChooseGoFPC() || m_bScnCheckByPR==FALSE )
				{
					lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CONTINUESTOP|0x80000000, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				}
				else
				{
					lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				}
				FivePointCheckLog(szText);
			}
			else
			{
				lHmiStatus = glHMI_CONTINUE;
			}
		}
		else
		{
			szText.Format("No surrounding grade on %ld %ld position", lScnCheckRow, lScnCheckCol);

			if (GetErrorChooseGoFPC())
			{
				lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CONTINUESTOP|0x80000000, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			}
			else 
			{
				lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			}
			FivePointCheckLog(szText);
		}

		if (lHmiStatus != glHMI_CONTINUE)
		{
			szContent.LoadString(HMB_GENERAL_STOPBYUSER);
			szMsg.Format("Five Point Check die fail: %s", (LPCTSTR) szContent);
			FivePointCheckLog(szMsg);
			HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			bFiveResult = FALSE;
			break;
		}
	}

	if( bFiveResult )
	{
		szMsg.Format("Five Point Check die finished successfully");
		FivePointCheckLog(szMsg);
	}

	return bFiveResult;
}

VOID CWaferPr::FivePointCheckLog(CString szLogMsg)
{
	CString szLogFile;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);
	szLogFile = szLogPath + "_FivePoint.txt";

	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFile, "a");

	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%s\n", (LPCTSTR) szLogMsg);	//Klocwork	//v4.46
		fclose(fp);
	}
}

BOOL CWaferPr::WprScnCheckDiePrResult(LONG lScnCheckRow, LONG lScnCheckCol, BOOL &bPromptMessage, CString &szText)
{
	LONG lOffX, lOffY;

	if ( m_bScnCheckByPR != TRUE )
	{
		WprSearchDie(TRUE, 1, lOffX, lOffY);
		szText.Format("Current map Position is %ld,%ld", lScnCheckRow, lScnCheckCol);
		bPromptMessage = TRUE;
		return TRUE;
	}

	CString szMessage, szTitle;
	szTitle.LoadString(HMB_WT_CHECK_SCN_FUNC);
	BOOL bPrStatus = FALSE;

	if ( m_bScnCheckIsRefDie == FALSE )
	{
		bPrStatus = WprSearchDie(TRUE, 1, lOffX, lOffY);
	}
	else
	{
		LONG lRefDieLrn = (*m_psmfSRam)["WaferPr"]["RefDie"]["Count"];
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

		LONG lRefDieStart = 1;
		LONG lRefDieStop  = lRefDieLrn;
		if( m_lScnCheckRefDieNo>0 )
		{
			lRefDieStart = lRefDieStop  = m_lScnCheckRefDieNo;
		}

		for ( LONG lDieNo=lRefDieStart; lDieNo<=lRefDieStop; lDieNo++ )
		{
			bPrStatus = WprSearchDie(FALSE, lDieNo, lOffX, lOffY);

			if ( bPrStatus == TRUE )
			{
				break;
			}
		}
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (bPrStatus == FALSE)
	{
		bPromptMessage = TRUE;
		szText.Format("Empty die on %ld %ld position", lScnCheckRow, lScnCheckCol);
		SetStatusMessage(szText);
		szMessage.LoadString(HMB_WT_PR_SEARCH_ERROR);
		HmiMessage_Red_Back(szMessage, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	szText.Format("Die is found on %ld %ld position", lScnCheckRow, lScnCheckCol);

	bPromptMessage = FALSE;

	return TRUE;
}

BOOL CWaferPr::WprGetDieValidPrescanPosn(LONG ulMapRow, LONG ulMapCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY)
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
	lStatus = TRUE;

	LONG ulIntRow = ulMapRow - m_lMapScanOffsetRow;
	LONG ulIntCol = ulMapCol - m_lMapScanOffsetCol;
	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if (GetScanPosn(lTmpRow, lTmpCol, lPhyX, lPhyY))
	{
		return TRUE;
	}

	for (ulCurrentLoop = 1; ulCurrentLoop <= ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop * 2 + 1);
		lCol = (ulCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		if (lTmpRow >= 0 && lTmpCol >= 0)
		{
			if (GetScanPosn(lTmpRow, lTmpCol, lPhyX, lPhyY))
			{
				bFindNearDie = TRUE;
				break;
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
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

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetScanPosn(lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
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

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if ( GetScanPosn(lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
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

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetScanPosn(lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
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

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetScanPosn(lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}
	}

	if (bFindNearDie == FALSE)
	{
		lStatus = FALSE;
	}
	else
	{
		LONG lDiePitchX_X = GetDiePitchX_X(); 
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_Y = GetDiePitchY_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();

		LONG lDiff_X = ulIntCol - lTmpCol;
		LONG lDiff_Y = ulIntRow - lTmpRow;

		//Calculate original no grade die pos from surrounding die position
		BOOL bPhyStatus = FALSE;
		if (lTmpRow >= 0 && lTmpCol >= 0)
		{
			bPhyStatus = GetScanPosn(lTmpRow, lTmpCol, lPhyX, lPhyY);
		}
		if (bPhyStatus)
		{
			lPhyX = lPhyX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lPhyY = lPhyY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
		}
		else
		{
			lStatus = FALSE;
		}
	}

	return lStatus;
}

BOOL CWaferPr::GetScanPosn(LONG ulRow, LONG ulCol, LONG &lPhyX, LONG &lPhyY)
{	//	the calling part for this already add the offset from map base.
	LONG lIndex;
	BOOL bOverLap;

	BOOL bReturn = m_cPIMap.GetInfo(ulRow, ulCol, lPhyX, lPhyY, lIndex, bOverLap);
	if( bReturn )
	{
		if( IsWithinScanLimit(lPhyX, lPhyY)==FALSE )
		{
			bReturn = FALSE;
		}
	}

	return bReturn;
}

LONG CWaferPr::SetScanAlignVerifyMode(IPC_CServiceMessage &svMsg)
{
	UCHAR ucCheck = 0;
	svMsg.GetMsg(sizeof(UCHAR), &ucCheck);

	m_ucScanAlignVerify = ucCheck;

	CString szMsg = "Scan Align Result Check condition.\n";

	switch( m_ucScanAlignVerify )	// Scan <==> Map check description
	{
	case 6:	// S<>M
		szMsg += "Scan wafer should be large than Map symmetrically!";
		break;
	case 5:	// S > M
		szMsg += "Scan wafer should be large than Map!";
		break;
	case 4:	// S >= M
		szMsg += "Scan wafer should not be small than Map!";
		break;
	case 3:	// S == M
		szMsg += "Scan wafer should be equal to Map!";
		break;
	case 2:	//	S <= M
		szMsg += "Scan wafer should not large than Map!";
		break;
	case 1:	//	S <  M
		szMsg += "Scan wafer should small than Map!";
		break;
	case 0:	// not check map/scan dimension match result
	default:
		szMsg += "No checking between wafer and map!";
		break;
	}

	SavePrData(FALSE);

	HmiMessage(szMsg, "Scan Align");
	SaveScanTimeEvent(szMsg);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferPr::IsScanPrIdle()
{
	if( IsBurnIn() && m_bNoWprBurnIn )
	{
		return TRUE;
	}

	return m_pPrescanPrCtrl->IsIdle();
}

BOOL CWaferPr::IsScanPrReadyToGrab()
{
	if( IsBurnIn() && m_bNoWprBurnIn )
	{
		return TRUE;
	}

	if( IsAutoRescanWafer() && IsPrecanWith2Pr() )
	{
		if( GetImageNumInGallery()<=m_lUsedSrchThreadNum )
			return TRUE;
		else
			return FALSE;
	}
	return	m_pPrescanPrCtrl->IsReadyToGrab();
}

LONG CWaferPr::SelectAoiOcrDieNo(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;
	//Get digital no from HMI	
	if( svMsg.GetMsgLen()!=2 )
	{
		HmiMessage("Please input 1 character only!", "AOI learn OCR");
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	char *pBuffer;
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);
	CString szChar =&pBuffer[0];
	UCHAR ucChar = pBuffer[0];
	delete[] pBuffer;

	LONG lDigitNo = -1;
	if( ucChar>='0' && ucChar<='9' )
	{
		lDigitNo = ucChar - '0';
	}
	else if( ucChar>='a' && ucChar<='z' )
	{
		lDigitNo = ucChar -'a' + 10;
	}
	else if( ucChar>='A' && ucChar<='Z' )
	{
		lDigitNo = ucChar -'A' + 10;
	}

	if( lDigitNo<0 || lDigitNo>35 )
	{
		HmiMessage("Please input 0-9 or a-z or A-Z!", "AOI learn OCR");
	}
	else
	{
		m_lCurRefDieNo = lDigitNo + WPR_GEN_OCR_DIE_START;
		m_lLrnAlignAlgo = 1;    // Force to use Pattern Matching method for char die
		m_bAoiOcrDieLearnt = IsThisDieLearnt((UCHAR)m_lCurRefDieNo);
		m_szAoiLearnOcrValue = szChar.MakeUpper();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferPr::LoadAoiGradeConvertFile(IPC_CServiceMessage &svMsg)	// just after enable the option
{
	BOOL bReturn = FALSE;

	if( IsUseGradeConvert()==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

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

	m_lGradeMappingLimit	= 0;
	memset(m_sMapOrgGrade,		0, sizeof(m_sMapOrgGrade));
	memset(m_sPrInspectGrade,	0, sizeof(m_sPrInspectGrade));
	memset(m_sMapNewGrade,		0, sizeof(m_sMapNewGrade));
	SavePrData(FALSE);
	if ( nReturn != IDOK )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szFilename = dlgFile.GetPathName();	// full path and name and ext. best.
	if ((_access(szFilename, 0 )) == -1)
	{
		HmiMessage_Red_Back("Select file can not access!", "AOI Grade");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
// check format and pass data into list.
	LONG lIndex = 0;
	CStdioFile fCheckFile;
	if( fCheckFile.Open(szFilename, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szReading;
		CStringArray szaDataList;
		fCheckFile.SeekToBegin();
		// get the fail counter limit
		while( fCheckFile.ReadString(szReading) )
		{
			szaDataList.RemoveAll();
			CUtility::Instance()->ParseRawData(szReading, szaDataList);
			if( szaDataList.GetSize()>=3 )
			{
				UCHAR ucOrgGrade = (UCHAR)atoi(szaDataList.GetAt(0));
				UCHAR ucIspGrade = (UCHAR)atoi(szaDataList.GetAt(1));
				UCHAR ucNewGrade = (UCHAR)atoi(szaDataList.GetAt(2));
				if( m_bScanNgGradeConvert )
				{
					m_sMapOrgGrade[ucOrgGrade]		= ucOrgGrade;
					m_sPrInspectGrade[ucOrgGrade]	= ucIspGrade;
					m_sMapNewGrade[ucOrgGrade]		= ucNewGrade;
				}
				else
				{
					m_sMapOrgGrade[lIndex]		= ucOrgGrade;
					m_sPrInspectGrade[lIndex]	= ucIspGrade;
					m_sMapNewGrade[lIndex]		= ucNewGrade;
				}
				lIndex++;
				if( lIndex>=WPR_AOI_GRADE_MAX_MAPPING )
				{
					break;
				}
			}
		}
		fCheckFile.Close();
	}
	m_lGradeMappingLimit	= lIndex;
	SavePrData(FALSE);

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SetOcrDieDetectWindow(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	LONG lWindowNo = 0;

	//Get Region No from HMI
	svMsg.GetMsg(sizeof(LONG), &lWindowNo);
	if( lWindowNo < 1 || lWindowNo>WPR_OCR_MAX_WINDOWS )
	{
		HmiMessage("Please input valid window no.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		HmiMessage("Vision system not initializaed.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bUseMouse)
	{
		HmiMessage("Please press the Confirm button firstly.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG lOffsetX = 0, lOffsetY = 0;
	if( WprSearchDie(TRUE, 1, lOffsetX, lOffsetY)==FALSE )
	{
		HmiMessage("Please locate PR cursor to a normal die.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	INT nWftX = 0, nWftY = 0, nWftT = 0;
	GetWaferTableEncoder(&nWftX, &nWftY, &nWftT);
	nWftX += lOffsetX;
	nWftY += lOffsetY;
	MoveWaferTable(nWftX, nWftY);

	PR_COORD coCorner1, coCorner2;

	for(int i=0; i<WPR_OCR_MAX_WINDOWS; i++)
	{
		coCorner1 = m_stOcrDetectWin[i].coCorner1;
		coCorner2 = m_stOcrDetectWin[i].coCorner2;
		// Draw the defined region
		DrawRectangleBox(coCorner1, coCorner2, PR_COLOR_BLUE);
	}

	coCorner1 = m_stOcrDetectWin[lWindowNo - 1].coCorner1;
	coCorner2 = m_stOcrDetectWin[lWindowNo - 1].coCorner2;
	if( coCorner1.x==0 || coCorner1.y==0 || coCorner2.x==0 ||coCorner2.y==0 )
	{
		coCorner1.x = (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
		coCorner1.y = (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
		coCorner2.x = (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
		coCorner2.y = (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
	}

	// Erase the original region
	DrawAndEraseCursor(coCorner1, PR_UPPER_LEFT, 0);
	DrawAndEraseCursor(coCorner2, PR_LOWER_RIGHT, 0);

	// Define by using mouse
	m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_YELLOW);

	// Set the mouse size first
	m_pPrGeneral->MouseSet2PointRegion(coCorner1, coCorner2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_YELLOW);

	m_bUseMouse = TRUE;

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::GetOcrDieDetectWindow(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	LONG lWindowNo = 0;

	//Get Region No from HMI
	svMsg.GetMsg(sizeof(LONG), &lWindowNo);

	if( lWindowNo < 1 || lWindowNo>WPR_OCR_MAX_WINDOWS )
	{
		HmiMessage("Please input valid window no.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		HmiMessage("Vision system not initializaed.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bUseMouse == FALSE)
	{
		HmiMessage("Please press the Set button firstly.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	// Get the mouse defined region
	LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stOcrDetectWin[lWindowNo - 1].coCorner1,
													   m_stOcrDetectWin[lWindowNo - 1].coCorner2);
	if (uwNumOfCorners == 2)
	{
		// Draw the defined region
		DrawRectangleBox(m_stOcrDetectWin[lWindowNo - 1].coCorner1, m_stOcrDetectWin[lWindowNo - 1].coCorner2, PR_COLOR_BLUE);
	}

	m_bUseMouse = FALSE;

	SavePrData(TRUE);

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ResetOcrDieWindows(IPC_CServiceMessage &svMsg)
{
	if( HmiMessage("All OCR windows would be reset to 0.\nAre you sure to continue?", "OCR die", glHMI_MBX_YESNO)==glHMI_YES )
	{
		memset(&m_stOcrDetectWin, 0, sizeof(m_stOcrDetectWin));
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


PR_UWORD CWaferPr::SearchAoiOcrDie(PR_COORD *stDieOffset, PR_REAL *fDieRotate, BOOL bDebugLog)
{
	if( IsOcrAOIMode()==FALSE )
		return FALSE;

	m_szAoiOcrPrValue	= "******";
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_szAoiMapInValue = pApp->GetProfileString(gszPROFILE_SETTING, "AOI OCR die map format", "B2B2B2");

	PR_SRCH_DIE_CMD	stSrchDieCmd;

	LONG emInfo;
	if ( m_bDebugVideoTest == TRUE )
		emInfo = PR_SHOW_DEFECT;
	else
		emInfo = PR_NO_DISPLAY;
	PR_COORD	stSrchCorner1 = GetSrchArea().coCorner1;
	PR_COORD	stSrchCorner2 = GetSrchArea().coCorner2;

	GetRefDieSrchCmd(&stSrchDieCmd, WPR_REFERENCE_DIE, 1, stSrchCorner1, stSrchCorner2, PR_TRUE, PR_TRUE, PR_FALSE, emInfo);

	PR_SRCH_NUMBER_DIE_CMD		stSrchOcrCmd;
	PR_InitSrchNumberDie( &stSrchOcrCmd );
	CString szMsg;
	szMsg.Format("Search Refer angle start %f, end %f", 
		stSrchDieCmd.stDieAlignPar.rStartAngle, stSrchDieCmd.stDieAlignPar.rEndAngle);
	SetAlarmLog(szMsg);
	stSrchOcrCmd.stSrchRefDieCmd = stSrchDieCmd;

//	PR_WIN stArea;
//	GetSearchDieArea(&stArea, WPR_GEN_RDIE_OFFSET + 1);

	CString szLetter = "", szNumber = "";
	INT nOcrNo = 0;
	for(int nWinNo = 0; nWinNo < m_szAoiMapInValue.GetLength(); nWinNo++)
	{
		if( nWinNo>=WPR_OCR_MAX_WINDOWS )
		{
			break;
		}

		PR_RGN_CHAR_CMD		stRgnCmd;
		PR_InitRgnCharCmd( &stRgnCmd );

		stRgnCmd.stRgnWin = m_stOcrDetectWin[nWinNo];
		if( stRgnCmd.stRgnWin.coCorner1.x==0 || stRgnCmd.stRgnWin.coCorner1.y==0 ||
			stRgnCmd.stRgnWin.coCorner2.x==0 || stRgnCmd.stRgnWin.coCorner2.y==0 )
		{
			break;
		}

		stRgnCmd.aeMaxDefectArea	= 0;
		stRgnCmd.rStartAngle		= GetSearchStartAngle();
		stRgnCmd.rEndAngle			= GetSearchEndAngle();
		stRgnCmd.emGraphicInfo		= PR_NO_DISPLAY;
		stRgnCmd.rPassScore			= (PR_REAL)m_lOcrPassScore;	//	85.0;
		stRgnCmd.emSameView			= PR_TRUE;			
		CString szLog;
		szLog.Format("search OCR angle start %f, end %f", stRgnCmd.rStartAngle, stRgnCmd.rEndAngle);
		SetAlarmLog(szLog);
		UCHAR ucLrnChar = 0;
		CString szMsg = "";
		CString szTemp;
		if( m_szAoiMapInValue.GetAt(nWinNo)>='A' && m_szAoiMapInValue.GetAt(nWinNo)<='Z' )
		{
			szMsg = "Letter ";
			for(UCHAR ucChar=0; ucChar<26; ucChar++)
			{
				UCHAR ucDieNo = ucChar+10+WPR_GEN_OCR_DIE_START;
				if( IsThisDieLearnt(ucDieNo) )
				{
					memset(stRgnCmd.astCharInfo[ucLrnChar].aubString,  NULL, sizeof(stRgnCmd.astCharInfo[ucLrnChar].aubString));
					stRgnCmd.astCharInfo[ucLrnChar].aubString[0] =  ucChar + 'A';
					stRgnCmd.astCharInfo[ucLrnChar].uwRecordID = GetDiePrID(ucDieNo);
					ucLrnChar++;
					szTemp.Format("%c(%2d) record %2d    ", ucChar+'A', ucDieNo, GetDiePrID(ucDieNo));
					szMsg += szTemp;
				}
			}
			szTemp.Format("total learn no %d", ucLrnChar);
			szMsg += szTemp;
			szLetter = szMsg;
		}
		else
		{
			szMsg = "Number ";
			for(UCHAR ucChar=0; ucChar<10; ucChar++)
			{
				UCHAR ucDieNo = ucChar+WPR_GEN_OCR_DIE_START;
				if( IsThisDieLearnt(ucDieNo) )
				{
					memset(stRgnCmd.astCharInfo[ucLrnChar].aubString,  NULL, sizeof(stRgnCmd.astCharInfo[ucLrnChar].aubString));
					stRgnCmd.astCharInfo[ucLrnChar].aubString[0] =  ucChar + '0';
					stRgnCmd.astCharInfo[ucLrnChar].uwRecordID = GetDiePrID(ucDieNo);
					ucLrnChar++;
					szTemp.Format("%c(%2d) record %2d    ", ucChar+'0', ucDieNo, GetDiePrID(ucDieNo));
					szMsg += szTemp;
				}
			}
			szTemp.Format("total learn no %d", ucLrnChar);
			szMsg += szTemp;
			szNumber += szMsg;
		}
		stRgnCmd.uwNumOfCharInfo = ucLrnChar;

		stSrchOcrCmd.astRgnCharCmd[nWinNo] = stRgnCmd;
		nOcrNo++;
	}
	if( bDebugLog )
	{
		SetAlarmLog(szLetter);
		SetAlarmLog(szNumber);
	}
	m_szAoiMapInValue = "";

	stSrchOcrCmd.emEnableSrchRgnClip	= PR_FALSE;
	stSrchOcrCmd.uwNumOfChar			= nOcrNo;
	stSrchOcrCmd.coRefDieCenter.x		= PR_DEF_CENTRE_X;
	stSrchOcrCmd.coRefDieCenter.y		= PR_DEF_CENTRE_Y;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	// get reply1 of search ocr die
	PR_SRCH_NUMBER_DIE_RPY1		stSrchOcrRpy1;
	PR_SrchNumberDieExt(&stSrchOcrCmd, ubSID, ubRID, &stSrchOcrRpy1);
	if( stSrchOcrRpy1.uwCommunStatus != PR_COMM_NOERR )
	{
		CString szMsg;
		szMsg.Format("PR SrchNumberDieExt() is finished with com status %u!\n", stSrchOcrRpy1.uwCommunStatus);
		if( bDebugLog )
			SetAlarmLog("User search ocr, " + szMsg);
		return FALSE;
	}

	// 32821, partial ocr found, unknow is _
	if( (stSrchOcrRpy1.uwPRStatus!=PR_ERR_NOERR) && (stSrchOcrRpy1.uwPRStatus!=32821) )
	{
		CString szMsg;
		szMsg.Format("PR SrchNumberDieExt() is finished with PR status %u!\n", stSrchOcrRpy1.uwPRStatus);
		if( bDebugLog )
			SetAlarmLog("User search ocr, " + szMsg);
		return FALSE;
	}

	// get reply2 of search ocr die
	PR_SRCH_NUMBER_DIE_RPY2		stSrchOcrRpy2;
	PR_SrchNumberDieExtRpy(ubSID, &stSrchOcrRpy2);
	if( stSrchOcrRpy2.stStatus.uwCommunStatus!=PR_COMM_NOERR )
	{
		CString szMsg;
		szMsg.Format("PR_SrchNumberDieExtRpy stSrchOcrRpy2.stStatus.uwCommunStatus %u", stSrchOcrRpy2.stStatus.uwCommunStatus);
		if( bDebugLog )
			SetAlarmLog("User search ocr, " + szMsg);
		return FALSE;
	}

	if( stSrchOcrRpy2.stStatus.uwPRStatus!=PR_ERR_NOERR && stSrchOcrRpy2.stStatus.uwPRStatus!=32821 )
	{
		CString szMsg;
		szMsg.Format("PR_SrchNumberDieExtRpy stSrchOcrRpy2.stStatus.uwPRStatus %u", stSrchOcrRpy2.stStatus.uwPRStatus);
		if( bDebugLog )
			SetAlarmLog("User search ocr, " + szMsg);
		return FALSE;
	}

	//Check result
	*fDieRotate = stSrchOcrRpy2.rDieRot;
	m_szAoiOcrPrValue = "";
	CString szOcrScore = " ", szTemp;
	for ( int i = 0; i < stSrchOcrRpy2.uwNumOfChar; i++ )
	{
		if (stSrchOcrRpy2.aaubString[0] != NULL)
			m_szAoiOcrPrValue += (char *)stSrchOcrRpy2.aaubString[i];
		else
			m_szAoiOcrPrValue += "*";
		szTemp.Format("%d;", (LONG)stSrchOcrRpy2.arCharScore[i]);
		szOcrScore += szTemp;
	}

	if( bDebugLog )
	{
		SetAlarmLog("Find OCR " + m_szAoiOcrPrValue + szOcrScore);
	}

	stDieOffset->x = stSrchOcrRpy2.coCentre.x;
	stDieOffset->y = stSrchOcrRpy2.coCentre.y;

	return TRUE;
}



LONG CWaferPr::CounterCheckReferOnNormalDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReferOnNormal = TRUE;
	if( IsScanAlignWafer() && IsPrecanWith2Pr() && IsPrescanReferDie() )
	{
		LONG lOffsetX, lOffsetY;
		if( WprSearchDie(FALSE, 1, lOffsetX, lOffsetY) )
		{
            bReferOnNormal = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL),	&bReferOnNormal);

	return 1;
}

LONG CWaferPr::SetContourImagePath(IPC_CServiceMessage &svMsg)
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
		m_szContourImagePath = szSaveImagePath;
	}
	else
	{
		m_szContourImagePath = "";
	}

	SavePrData();
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 

LONG CWaferPr::GetContourImagePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	if (pAppMod != NULL)
	{
		pAppMod->GetPath(m_szContourImagePath);
		bReturn = TRUE;
		SavePrData();
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


BOOL CWaferPr::IsIgnoreDie(LONG lTgtRow, LONG lTgtCol)
{
	UCHAR ucCurGrade = m_WaferMapWrapper.GetGrade(lTgtRow, lTgtCol);
	UCHAR ucNullBin  = m_WaferMapWrapper.GetNullBin();

	if( ucCurGrade==ucNullBin )
	{
		return TRUE;
	}

	if( IsNoDieGrade(lTgtRow, lTgtCol) )
	{
		return TRUE;
	}

	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + m_WaferMapWrapper.GetGradeOffset();
	if( m_bPrescanEmptyToNullBin==FALSE && m_bPrescanEmptyAction && ucCurGrade==ucEmptyGrade )
	{
		return TRUE;
	}

	if( m_ucPrescanEdgeSize>0 && 
		m_WaferMapWrapper.GetDieState(lTgtRow, lTgtCol)==WT_MAP_DIESTATE_SCAN_EDGE)
	{
		return TRUE;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanRegionMode() )
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
		if( pSRInfo->IsWithinThisRegion(ulTgtRegion, lTgtRow, lTgtCol)==FALSE )
		{
			return TRUE;
		}
	}
	if( IsOutMS90SortingPart(lTgtRow, lTgtCol) )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CWaferPr::RefillScanInfoViaRemainDice(LONG lTgtRow, LONG lTgtCol, ULONG ulLoop)
{
	if (IsInMapValidRange(lTgtRow, lTgtCol)==FALSE)
	{
		return FALSE;
	}

	if( IsIgnoreDie(lTgtRow, lTgtCol) )
	{
		return FALSE;
	}

	LONG lEncX = 0, lEncY = 0, lNewWftX, lNewWftY;
	if (GetWftPosn(FALSE, lTgtRow, lTgtCol, lEncX, lEncY))
	{
		return TRUE;
	}

	BOOL bFindOne = FALSE;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	LONG lDieTolX = labs(GetDieSizeX()*5/10);
	LONG lDieTolY = labs(GetDieSizeY()*5/10);
	if( GetDieValidPrescanPosn(lTgtRow, lTgtCol, ulLoop, lEncX, lEncY, FALSE) )
	{
		for(ULONG ulIndex=1; ulIndex<=WSGetScanRemainDieTotal(); ulIndex++)
		{
			ULONG ulWSIndex = 0;
			if( WSGetScanRemainDieFromList(ulIndex,  lNewWftX, lNewWftY, ulWSIndex) )
			{
				if( ulWSIndex==0 )
					continue;
				if( labs(lNewWftX-lEncX)<lDieTolX && 
					labs(lNewWftY-lEncY)<lDieTolY )
				{
					double dDieAngle;
					CString szDieBin = "0";
					USHORT uwDiePrID;
					ULONG ulFrameID;
					CString szLogPath;
					pUtl->GetPrescanLogPath(szLogPath);
					CString szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
					FILE *fpOut = NULL;
					errno_t nErr = -1;
					if( pUtl->GetPrescanDebug() )
					{
						nErr = fopen_s(&fpOut, szConLogFile, "a");
					}

					USHORT usDieState = 0;
					WSGetScanPosition(ulWSIndex, lNewWftX, lNewWftY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulFrameID);
					SetScanInfo(lTgtRow, lTgtCol, lNewWftX, lNewWftY, dDieAngle, szDieBin, usDieState);
					SetPrescanFrameInfo(lTgtRow, lTgtCol, ulFrameID);
					if ((nErr == 0) && (fpOut != NULL))
					{
						fprintf(fpOut, "%ld,%ld,%lu,%lu,%ld,%ld,RefileViaRemain\n", lTgtRow, lTgtCol, lTgtRow, lTgtCol, lNewWftX, lNewWftY);
						fclose(fpOut);
					}
					WSUpdateScanRemainDieInList(ulIndex, lNewWftX, lNewWftY, 0);
					bFindOne = TRUE;
					break;
				}
			}
		}
	}

	return bFindOne;
}

PR_REAL CWaferPr::GetSearchStartAngle()
{
	PR_REAL rStart = PR_SRCH_START_ANGLE;
	if (IsMS90Sorting2ndPart())	// sort mode
	{
		rStart += 180;
	}

	return rStart;
}

PR_REAL CWaferPr::GetSearchEndAngle()
{
	PR_REAL rEnd = PR_SRCH_END_ANGLE;
	if (IsMS90Sorting2ndPart())	// sort mode
	{
		rEnd += 180;
	}

	return rEnd;
}

PR_REAL CWaferPr::GetScanRotTol(BOOL bUseScanAngle)
{
	DOUBLE dAcptAngle = PR_SRCH_ROT_TOL;

	if( bUseScanAngle )	//	IsEnableZoom() && 
	{
		dAcptAngle = fabs(m_dScanAcceptAngle);
		if( dAcptAngle==0 )
			dAcptAngle = 5.0;
	}

	return (PR_REAL) dAcptAngle;
}

PR_REAL CWaferPr::GetScanStartAngle(BOOL bUseScanAngle)
{
	PR_REAL rStart = PR_SRCH_START_ANGLE;

	if (bUseScanAngle)	//	IsEnableZoom() && 
	{
		rStart = (PR_REAL)(0 - fabs(m_dScanAcceptAngle));
		if (rStart == 0)
		{
			rStart = 0 - 5.0;
		}
	}

	if (IsMS90Sorting2ndPart())	// sort mode
	{
		rStart += 180;
	}

	return rStart;
}

PR_REAL CWaferPr::GetScanEndAngle(BOOL bUseScanAngle)
{
	PR_REAL rEnd = PR_SRCH_END_ANGLE;

	if (bUseScanAngle)	//	IsEnableZoom() && 
	{
		rEnd = (PR_REAL)fabs(m_dScanAcceptAngle);
		if (rEnd == 0)
			rEnd = 5.0;
	}
	if (IsMS90Sorting2ndPart())	// sort mode
	{
		rEnd += 180;
	}

	return rEnd;
}

BOOL CWaferPr::IsWithinScanLimit(LONG lEncX, LONG lEncY)
{
	LONG lEdgeDieNum = 2;
	DOUBLE dScale = 1.0;
	LONG lEdgeX = GetDiePitchXX() * lEdgeDieNum;
	LONG lEdgeY = GetDiePitchYY() * lEdgeDieNum;

//	if( lEdgeX>1500 )
//		lEdgeX = 1500;
//	if( lEdgeY>1500 )
//		lEdgeY = 1500;

	if( GetWaferDiameter()!=0 )
	{
		DOUBLE dReduce = (DOUBLE) max(GetPrescanPitchX(), GetPrescanPitchY());
		if( GetScnZoom()!=GetNmlZoom() || IsDP() )
		{
			dReduce = (DOUBLE) max(m_dLFSizeX*GetDiePitchXX(), m_dLFSizeY*GetDiePitchYY());
		}
		if( IsMS90() && IsPrescanMapIndex()==FALSE )
		{
			dReduce = (DOUBLE) max(GetDiePitchXX(), GetDiePitchYY()) * 6.0;	//	3 dice shift
		}
		if( dReduce>max(lEdgeX, lEdgeY) )
			dReduce = max(lEdgeX, lEdgeY);
		DOUBLE dWafer  = (DOUBLE) GetWaferDiameter();
		dScale = 1.0 - dReduce / dWafer;
	}

	BOOL bReturn = IsWithinInputWaferLimit(lEncX, lEncY, dScale);
	if( bReturn && IsMS90() && (m_lWafXPosLimit!=0) && (m_lWafXNegLimit!=0) && (m_lWafYPosLimit!=0) && (m_lWafYNegLimit!=0) )
	{ 
/*
		LONG lPosLimitX = m_lWafXPosLimit - lEdgeX;
		LONG lNegLimitX = m_lWafXNegLimit + lEdgeX;
		LONG lPosLimitY = m_lWafYPosLimit - lEdgeY;
		LONG lNegLimitY = m_lWafYNegLimit + lEdgeY;
*/
		LONG lPosLimitX = m_lWafXPosLimit + lEdgeX;
		LONG lNegLimitX = m_lWafXNegLimit - lEdgeX;
		LONG lPosLimitY = m_lWafYPosLimit + lEdgeY;
		LONG lNegLimitY = m_lWafYNegLimit - lEdgeY;

		if( (lEncX > lPosLimitX) || (lEncX < lNegLimitX) ||
			(lEncY > lPosLimitY) || (lEncY < lNegLimitY) )
		{
			bReturn = FALSE;
		}
	}

	return bReturn;
}

LONG CWaferPr::WalkOnTwinDice(IPC_CServiceMessage &svMsg)
{
	if( IsPrescanEnded()==FALSE )
	{
		HmiMessage("Prescan not complete yet.", "Prescan");
		return 1;
	}

	ULONG ulMapRowMax, ulMapColMax;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);
	for(ULONG ulRow=0; ulRow<=ulMapRowMax; ulRow++)
	{
		for(ULONG ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DS_SCAN_TWIN_DIE_FIRST )
			{
				LONG lWftX = 0, lWftY = 0;
				if( GetMapPhyPosn(ulRow, ulCol, lWftX, lWftY) )
				{
					m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
					MoveWaferTable(lWftX, lWftY);
					Sleep(1000);
				}
			}
		}
	}

	return 1;
}

BOOL CWaferPr::IsUseGradeConvert()
{
	return m_bUseAoiGradeConvert || m_bScanNgGradeConvert;
}

VOID CWaferPr::CheckPrescanEdgeDie()
{
	UCHAR ucPrescanEdgeSizeX = m_ucPrescanEdgeSizeX;
	UCHAR ucPrescanEdgeSizeY = m_ucPrescanEdgeSizeY;
	if( ucPrescanEdgeSizeX==0 && ucPrescanEdgeSizeY==0 )
	{
		return ;
	}
	ucPrescanEdgeSizeY = ucPrescanEdgeSizeX;

	m_ulEdgeGoodDieTotal = 0;
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	ULONG ulDieState = 0;
	ULONG ulMapRowMax, ulMapColMax;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);

	for(ULONG ulRow=0; ulRow<=ulMapRowMax; ulRow++)	// map edge
	{
		LONG lFirstGoodCol = -1, lLastGoodCol = -1;
		for(ULONG ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow, ulCol))
			{
				continue;
			}

			if( lFirstGoodCol==-1 )
				lFirstGoodCol = (LONG)ulCol;
			lLastGoodCol = (LONG)ulCol;
		}

		if( lFirstGoodCol>-1 && lLastGoodCol>-1 )
		{
			ULONG ulColL = lFirstGoodCol, ulColR = lLastGoodCol;
			for(UCHAR ucLoop=0; ucLoop<ucPrescanEdgeSizeX; ucLoop++)
			{
				ulColL = lFirstGoodCol + ucLoop;
				if( ulColL<=ulColR )
				{
					if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulColL))
					{
						m_ulEdgeGoodDieTotal++;
						m_WaferMapWrapper.SetDieState(ulRow, ulColL, WT_MAP_DS_SCAN_EDGE_CHECK);
					}
				}	//	row left part

				ulColR = lLastGoodCol - ucLoop;
				if( ulColR>ulColL )
				{
					if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulColR))
					{
						m_ulEdgeGoodDieTotal++;
						m_WaferMapWrapper.SetDieState(ulRow, ulColR, WT_MAP_DS_SCAN_EDGE_CHECK);
					}
				}	//	row right part

				if( ulColR<=ulColL )
				{
					break;
				}
			}
		}
	}

	for(ULONG ulCol=0; ulCol<=ulMapColMax; ulCol++)
	{
		LONG lFirstGoodRow = -1, lLastGoodRow = -1;
		for(ULONG ulRow=0; ulRow<=ulMapRowMax; ulRow++)	// map edge
		{
			if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow, ulCol))
			{
				continue;
			}
			if( lFirstGoodRow==-1 )
				lFirstGoodRow = (LONG)ulRow;
			lLastGoodRow = (LONG)ulRow;
		}

		if( lFirstGoodRow>-1 && lLastGoodRow>-1 )
		{
			ULONG ulRowT = lFirstGoodRow, ulRowB = lLastGoodRow;
			for(UCHAR ucLoop=0; ucLoop<ucPrescanEdgeSizeY; ucLoop++)
			{
				ulRowT = lFirstGoodRow + ucLoop;
				if( ulRowT<=ulRowB )
				{
					ulDieState = m_WaferMapWrapper.GetDieState(ulRowT, ulCol);
					if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRowT, ulCol) && ulDieState!=WT_MAP_DS_SCAN_EDGE_CHECK )
					{
						m_ulEdgeGoodDieTotal++;
						m_WaferMapWrapper.SetDieState(ulRowT, ulCol, WT_MAP_DS_SCAN_EDGE_CHECK);
					}
				}

				ulRowB = lLastGoodRow - ucLoop;
				if( ulRowB>ulRowT )
				{
					ulDieState = m_WaferMapWrapper.GetDieState(ulRowB, ulCol);
					if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRowB, ulCol) && ulDieState!=WT_MAP_DS_SCAN_EDGE_CHECK)
					{
						m_ulEdgeGoodDieTotal++;
						m_WaferMapWrapper.SetDieState(ulRowB, ulCol, WT_MAP_DS_SCAN_EDGE_CHECK);
					}
				}
			}
		}
	}
}	//	check edge total die counter.

VOID CWaferPr::SaveBadCutEvent(CString szLogMsg, BOOL bBackUp)	// prescan_time
{
	if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
	{
		return ;
	}

	// new file for sequencial loading
	CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "BadCutEvent.txt";
	if( bBackUp )
	{
		CStdioFile fLogFile;
		if( fLogFile.Open(szFileName, CFile::modeRead) )
		{
			ULONGLONG nFileSize = fLogFile.GetLength();
			fLogFile.Close();
			// Case of no need to backup the log file
			CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "BadCutEvent.bak";
			if( nFileSize>LOG_FILE_SIZE_LIMIT )
			{
				CopyFile(szFileName, szBkupName, FALSE);
				DeleteFile(szFileName);
			}
		}
	}
	else
	{
		FILE *pfFile = NULL;
		errno_t nErr = fopen_s(&pfFile, szFileName, "a");
		if ((nErr == 0) && (pfFile != NULL))
		{
			CTime theTime = CTime::GetCurrentTime();
			fprintf(pfFile, "%2d (%2d:%2d:%2d)	%s\n",
				theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LPCTSTR)szLogMsg);
			fclose(pfFile);
		}
	}
}

LONG CWaferPr::MultiSearchDieForLearnDieProcess(LONG lDieNo)
{
	CString szMsg, szTitle;
	BOOL bDrawDie;
	IPC_CServiceMessage stMsg;

	OpPrescanInit(lDieNo); // IDieNo = 1 means Normal 1 PR
	
	bDrawDie = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bDrawDie);
	MultiSearchDie(stMsg);
	
	/*szMsg = "Please check the multi search die result";
	szTitle = "Multi search die";
	HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL, 500, 300);*/
	
	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);

	return 1;
}