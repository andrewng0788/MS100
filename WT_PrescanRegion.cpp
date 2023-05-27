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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferTable::PrescanUpdateRegionAlign(INT lX, INT lY, ULONG lRow, ULONG lCol)
{
	CString szMsg;
	LONG lDiePitchX_X = 0, lDiePitchX_Y = 0, lDiePitchY_X = 0, lDiePitchY_Y = 0;
	LONG lDiff_X = 0, lDiff_Y = 0;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	pSRInfo->SetRegionAligned(TRUE);
	pSRInfo->SetRegionEnd(FALSE);

	m_nPrescanDirection		= 1;
	m_nPrescanAlignScanStage= 1;
	m_nPrescanAlignPosnX	= m_nPrescanNextWftPosnX = m_nPrescanLastWftPosnX	= lX;	// update wafer region alignment
	m_nPrescanAlignPosnY	= m_nPrescanNextWftPosnY = m_nPrescanLastWftPosnY	= lY;
	m_nPrescanAlignMapRow	= m_nPrescanNextMapRow	= m_nPrescanLastMapRow		= lRow;
	m_nPrescanAlignMapCol	= m_nPrescanNextMapCol	= m_nPrescanLastMapCol		= lCol;

	CString szLogFileName = m_szPrescanLogPath + "_deb.csv";
	DeleteFile(szLogFileName);
	szLogFileName = m_szPrescanLogPath + "_KeyDie.csv";
	DeleteFile(szLogFileName);


	pUtl->SetAlignPosition(m_nPrescanAlignMapRow, m_nPrescanAlignMapCol, lX, lY);

	lDiePitchX_X = GetDiePitchX_X(); 
	lDiePitchX_Y = GetDiePitchX_Y();
	lDiePitchY_X = GetDiePitchY_X();
	lDiePitchY_Y = GetDiePitchY_Y();

	LONG lInRow = lRow;
	LONG lInCol = lCol;

	m_lRescanStartRow		= -1;
	m_lRescanStartCol		= -1;
	m_lRescanStartWftX		= 0;
	m_lRescanStartWftY		= 0;
	m_ulNewPickCounter		= 0;
	m_bJustAlign			= TRUE;

	m_bVerifyMapWaferFail	= FALSE;
	m_lVerifyMapWaferRow	= -10000;
	m_lVerifyMapWaferCol	= -10000;
	memset(m_laPCIVerifyFailRow, -10000, sizeof(m_laPCIVerifyFailRow));
	memset(m_laPCIVerifyFailCol, -10000, sizeof(m_laPCIVerifyFailCol));

	// check time and back up and go on with new file
	m_bIntoNgPickCycle		= FALSE;
	m_dScanTime				= 0.0;
	m_dCurrTime				= 0.0;
	m_dStopTime				= GetTime();
	m_nPickListIndex		= -1;
	m_bPrescanStarted		= FALSE;
	m_lRescanRunStage		= 0;	//	region alignment done
	m_lWftAdvSamplingStage	= 0;
	m_lWftAdvSamplingResult	= 0;
	m_stPrescanStartTime	= CTime::GetCurrentTime();	//	region prescan align time
	m_stStopTime = CTime::GetCurrentTime();	// after wafer alignment

	ULONG ulScnAlnRow = lInRow;
	ULONG ulScnAlnCol = lInCol;

	m_lBuildShortPathInScanning = 0;	// prescan update region align, trigger restart
	// sort mode, 2nd part align wafer complete
	m_bScnModeWaferAlignTwice	= FALSE;
	m_bSortGoingTo2ndPart		= FALSE;
	INT i=0, j=0;

	CString szLogMsg;
	LONG lOrgRow, lOrgCol;
	ConvertAsmToOrgUser(lInRow, lInCol, lOrgRow, lOrgCol);
	szLogMsg.Format("WFT: %s; predict=%d; FPC=%d; Asm map(%d,%d)org(%d,%d); WFT(%d,%d)", 
		m_szPrescanMapName, m_lPredictMethod, IsEnableFPC(),
		lInRow, lInCol, lOrgRow, lOrgCol, lX, lY);
	SaveScanTimeEvent(szLogMsg, TRUE);

	m_lRunPredScanPitchTolX = m_lPredScanPitchTolX;
	m_lRunPredScanPitchTolY = m_lPredScanPitchTolY;

	DOUBLE dUsedTime = GetTime();
	m_lTimeSlot[16] = (LONG)(GetTime()-dUsedTime);			//	16.	prescan update wafer alignment, IM change wafer used time.
	m_bPitchAlarmOnce = FALSE;
	m_bReSampleAsError	= FALSE;
	m_ulPitchAlarmCount	= 0;

	if( IsPrescanBlkPick() )
		SetPrescanBlkFuncEnable(FALSE);	// after align, block pick alignment only, disable it.

	pUtl->SetRealignPosition(ulScnAlnRow, ulScnAlnCol, lX, lY);
	pUtl->SetRealignGlobalTheta(GetGlobalT());
	pUtl->SetPrescanGlobalTheta(GetGlobalT());

	XY_SafeMoveTo(lX, lY);

	m_ucPrescanRealignMethod = 0;

	m_nPrescanAlignMapRow = m_nPrescanNextMapRow	= m_nPrescanLastMapRow	 = ulScnAlnRow;
	m_nPrescanAlignMapCol = m_nPrescanNextMapCol	= m_nPrescanLastMapCol	 = ulScnAlnCol;
	SetMapPhyPosn(ulScnAlnRow, ulScnAlnCol, lX, lY);

	m_nPrescanDirection		= 1;
	m_nPrescanAlignScanStage = 1;
	m_lDetectEdgeState		= 0;
	m_bIsPrescanning		= TRUE;

	m_bIsPrescanned			= FALSE;
	m_lPrescanHomeDieRow	= -1;
	m_lPrescanHomeDieCol	= -1;
	m_lPrescanHomeWaferX	= lX;
	m_lPrescanHomeWaferY	= lY;
	m_bPrescanningMode		= TRUE;

	m_bManualAddReferPoints	= FALSE;

	ULONG ulUlRow = 0, ulUlCol = 0;
	ULONG ulLrRow = 0, ulLrCol = 0;
	ULONG ulAlnUlRow = 0, ulAlnUlCol = 0;
	ULONG ulAlnLrRow = 0, ulAlnLrCol = 0;
	ULONG ulTgtUlRow = 0, ulTgtUlCol = 0;
	ULONG ulTgtLrRow = 0, ulTgtLrCol = 0;

	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
	ULONG ulAlnRegion = pSRInfo->GetAssistRegion();
/*
	if(	pSRInfo->IsWithinThisRegion(ulTgtRegion, lRow, lCol)==FALSE )
	{
		szMsg.Format("Align refer die (%d,%d) not in target region %d", lRow, lCol, ulTgtRegion);
		if( pApp->GetCustomerName()!=_T("Semitek") )
			HmiMessage_Red_Back(szMsg);
		SetErrorMessage(szMsg);
	}
*/

	if(	pSRInfo->IsWithinThisRegion(ulAlnRegion, lRow, lCol)==FALSE )
	{
		szMsg.Format("Align refer die (%d,%d) not in assist region %d", lRow, lCol, ulAlnRegion);
		if( pApp->GetCustomerName()!=_T("Semitek") )
			HmiMessage_Red_Back(szMsg);
		SetErrorMessage(szMsg);
	}

	if( IsOutMS90SortingPart(lRow, lCol) )
	{
		szMsg.Format("Align refer die (%d,%d) not in sorting part", lRow, lCol);
		HmiMessage_Red_Back(szMsg);
		SetErrorMessage(szMsg);
	}

	if( pSRInfo->GetRegion(ulAlnRegion, ulAlnUlRow, ulAlnUlCol, ulAlnLrRow, ulAlnLrCol)==FALSE ||
		pSRInfo->GetRegion(ulTgtRegion, ulTgtUlRow, ulTgtUlCol, ulTgtLrRow, ulTgtLrCol)==FALSE )
	{
		szMsg.Format("refer die (%d,%d) not in region %d", lRow, lCol, ulAlnRegion);
		HmiMessage_Red_Back(szMsg);
		SetErrorMessage(szMsg);
	}
	ulUlRow = min(ulAlnUlRow, ulTgtUlRow);
	ulUlCol = min(ulAlnUlCol, ulTgtUlCol);
	ulLrRow = max(ulAlnLrRow, ulTgtLrRow);
	ulLrCol = max(ulAlnLrCol, ulTgtLrCol);
	szMsg.Format("region alignment target is %d, assist is %d", ulTgtRegion, ulAlnRegion);
	pUtl->RegionOrderLog(szMsg);

	UINT unWafEdgeNum = pApp->GetPrescanWaferEdgeNum();
	if( ulAlnRegion != ulTgtRegion )
	{
		ulUlRow = min(ulTgtUlRow, lRow);
		ulUlCol = min(ulTgtUlCol, lCol);
		ulLrRow = max(ulTgtLrRow, lRow);
		ulLrCol = max(ulTgtLrCol, lCol);
	}
	UINT nEdgeX = abs(GetPrescanPitchX())*(unWafEdgeNum + unWafEdgeNum);
	UINT nEdgeY = abs(GetPrescanPitchY())*(unWafEdgeNum + unWafEdgeNum);
	szMsg.Format("region scan corner %d,%d, %d,%d", ulUlRow, ulUlCol, ulLrRow, ulLrCol);
	pUtl->RegionOrderLog(szMsg);
	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
	if( m_dPrescanLFSizeY>(dFOVY+5) && m_dPrescanLFSizeX>(dFOVX+5) )
	{
		nEdgeX	= nEdgeX/2;
		nEdgeY	= nEdgeY/2;
		szMsg.Format("region scan edge enlarge half edge number of scan FOV %d,%d", nEdgeX, nEdgeY);
		pUtl->RegionOrderLog(szMsg);
	}


	pSRInfo->CalcPrescanRegionSize(ulUlRow, ulUlCol, ulLrRow, ulLrCol,	
												   lRow, lCol, lX, lY, nEdgeX, nEdgeY,
												   lDiePitchX_X, lDiePitchX_Y, lDiePitchY_Y, lDiePitchY_X);


	LONG lUserRow=0, lUserCol=0;
	ConvertAsmToOrgUser(lRow, lCol, lUserRow, lUserCol); 
	szMsg.Format("MAP, Asm(%ld, %ld), user(%ld, %ld)", lRow, lCol, lUserRow, lUserCol);
	pUtl->PrescanMoveLog(szMsg, TRUE);
	szMsg.Format("WFT, %d, %d", GetPrescanAlignPosnX(), GetPrescanAlignPosnY());
	pUtl->PrescanMoveLog(szMsg, TRUE);
	szMsg.Format("RGN, (%d,%d) (%d,%d)\n\n\n", pSRInfo->GetRegionWftULX(), pSRInfo->GetRegionWftULY(), pSRInfo->GetRegionWftLRX(), pSRInfo->GetRegionWftLRY());
	pUtl->PrescanMoveLog(szMsg, TRUE);
	szMsg.Format("movex(-1),movey,row,col");
	pUtl->PrescanMoveLog(szMsg, TRUE);

	pUtl->RegionOrderLog("Update region alignment complete");

	DefineAreaAndWalkPath_RegnScan();

	return TRUE;
}

BOOL CWaferTable::AutoRegionAlignRefBlk()	// in prestart cycle to find the target region refer die
{
	CString szMsg;

	if ( IsMotionCE() == TRUE )
	{
		szMsg = "Wafer table motion error, please home the motors!";
		HmiMessage_Red_Back(szMsg, "Region Prescan");
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		pUtl->RegionOrderLog(szMsg);
		return FALSE;
	}

//	m_WaferMapWrapper.StartMap();

	X_Sync();
	Y_Sync();
	T_Sync();
	Sleep(50);

	// Move theta home
	T_MoveTo(GetGlobalT(), SFM_WAIT);
	Sleep(50);

	BOOL	bFindReferDie = FALSE;
	ULONG	ulMapRow = 0, ulMapCol = 0;
	ULONG	ulAlnRegion;
	LONG	lWftX = 0, lWftY = 0;
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	ulAlnRegion = pSRInfo->GetAssistRegion();

	if( m_bSingleHomeRegionScan )	//	region sort using scan align method with home die only.
	{
		bFindReferDie = FALSE;
		ULONG	ulMaxRow = 0, ulMaxCol = 0;
		m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
		LONG ulNearDist = ulMaxRow*ulMaxRow + ulMaxCol*ulMaxCol;
		ULONG lULRow = 0, lULCol = 0, lLRRow = 0, lLRCol = 0;
		if( pSRInfo->GetRegion(ulAlnRegion, lULRow, lULCol, lLRRow, lLRCol) )
		{
			lLRRow--;
			lLRCol--;
			szMsg.Format("Region sort with scan align, region %lu corner (%lu,%lu)  (%lu,%lu) home total %d", 
				ulAlnRegion, lULRow, lULCol, lLRRow, lLRCol, pUtl->GetNumOfReferPoints());
			pUtl->RegionOrderLog(szMsg);
			LONG	lDiePitchX_X = GetDiePitchX_X(); 
			LONG	lDiePitchX_Y = GetDiePitchX_Y();
			LONG	lDiePitchY_X = GetDiePitchY_X();
			LONG	lDiePitchY_Y = GetDiePitchY_Y();

			for(UINT i=0; i<pUtl->GetNumOfReferPoints(); i++)
			{
				LONG lRow = 0, lCol = 0, lWfX = 0, lWfY = 0;
				pUtl->GetReferPosition(i, lRow, lCol, lWfX, lWfY);
				szMsg.Format(" home die %ld,%ld posn %ld,%ld", lRow, lCol, lWfX, lWfY); 
				pUtl->RegionOrderLog(szMsg);
				LONG lChkRow = lULRow;
				LONG lChkCol = lULCol;
				LONG lDiff_X = lChkCol - lCol;
				LONG lDiff_Y = lChkRow - lRow;
				LONG ulRefDist = lDiff_Y*lDiff_Y + lDiff_X*lDiff_X;
				if( ulRefDist < ulNearDist )
				{
					ulNearDist = ulRefDist;
					lWftX = lWfX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
					lWftY = lWfY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
					ulMapRow = lChkRow;
					ulMapCol = lChkCol;
					bFindReferDie = TRUE;
				}
				lChkRow = lULRow;
				lChkCol = lLRCol;
				lDiff_X = lChkCol - lCol;
				lDiff_Y = lChkRow - lRow;
				ulRefDist = lDiff_Y*lDiff_Y + lDiff_X*lDiff_X;
				if( ulRefDist < ulNearDist )
				{
					ulNearDist = ulRefDist;
					ulMapRow = lChkRow;
					ulMapCol = lChkCol;
					lWftX = lWfX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
					lWftY = lWfY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
					bFindReferDie = TRUE;
				}
				lChkRow = lLRRow;
				lChkCol = lULCol;
				lDiff_X = lChkCol - lCol;
				lDiff_Y = lChkRow - lRow;
				ulRefDist = lDiff_Y*lDiff_Y + lDiff_X*lDiff_X;
				if( ulRefDist < ulNearDist )
				{
					ulNearDist = ulRefDist;
					ulMapRow = lChkRow;
					ulMapCol = lChkCol;
					lWftX = lWfX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
					lWftY = lWfY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
					bFindReferDie = TRUE;
				}
				lChkRow = lLRRow;
				lChkCol = lLRCol;
				lDiff_X = lChkCol - lCol;
				lDiff_Y = lChkRow - lRow;
				ulRefDist = lDiff_Y*lDiff_Y + lDiff_X*lDiff_X;
				if( ulRefDist < ulNearDist )
				{
					ulNearDist = ulRefDist;
					ulMapRow = lChkRow;
					ulMapCol = lChkCol;
					lWftX = lWfX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
					lWftY = lWfY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
					bFindReferDie = TRUE;
				}
				szMsg.Format("nearest corner %ld,%ld table %ld,%ld from %ld,%ld at %ld,%ld", 
					ulMapRow, ulMapCol, lWftX, lWftY, lRow, lCol, lWfX, lWfY);
				pUtl->RegionOrderLog(szMsg);
			}
		}
	}	//	region sort using scan align method with home die only.
	else if( IsRegionAlignRefBlk() )	// blk refer or find all irregular refer die for region prescan
	{
		// during refer die check, update the value
		UINT	i;
		LONG	lX, lY, lT, lRow, lCol, lWfX, lWfY;
		BOOL	bResult;
		BOOL	bHasRefer = FALSE;
		LONG	lFirstRow=0, lFirstCol=0, lFirstWfX=0, lFirstWfY=0;		//Klocwork
		ULONG	ulMaxRow = 0, ulMaxCol = 0, ulMinDist, ulRefDist;
		LONG	lHalfMaxRow, lHalfMaxCol;

		m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
		ulMinDist = ulMaxRow*ulMaxRow + ulMaxCol*ulMaxCol;
		lHalfMaxRow = ulMaxRow/2;
		lHalfMaxCol = ulMaxCol/2;
		
		bFindReferDie = FALSE;

		pUtl->RegionOrderLog("loop refer die old record to locate region");
		//Search RefDie
		for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
		{
			pUtl->GetReRefPosition(i, lRow, lCol, lWfX, lWfY);

			if( pSRInfo->IsWithinThisRegion(ulAlnRegion, lRow, lCol)==FALSE )	// auto alignment in blk pick
			{
				continue;
			}
			if( IsOutMS90SortingPart(lRow, lCol) )
			{
				continue;
			}
			if (XY_SafeMoveTo(lWfX, lWfY) == FALSE)
			{
				szMsg.Format("Auto Region Align Refer Blk to (%ld,%ld), map (%ld,%ld) error", lWfX, lWfY, lRow, lCol);
				pUtl->RegionOrderLog(szMsg);
				continue;
			}

			m_WaferMapWrapper.SetCurrentPosition(lRow, lCol);

			Sleep(50);

			GetEncoder(&lX,&lY,&lT);

			bResult = SearchCurrReferDie(&lX, &lY);

			SetMapPhyPosn(lRow, lCol, lX, lY);

			bHasRefer = TRUE;
			ulRefDist = (lRow-lHalfMaxRow)*(lRow-lHalfMaxRow) + (lCol-lHalfMaxCol)*(lCol-lHalfMaxCol);
			if( ulRefDist < ulMinDist )
			{
				ulMinDist = ulRefDist;
				lFirstRow = lRow;
				lFirstCol = lCol;
				lFirstWfX = lWfX;
				lFirstWfY = lWfY;
			}

			if ( bResult != TRUE )
				continue;

			pUtl->UpdateReRefPosition(i, lRow, lCol, lX, lY);
			pUtl->AddAssistPosition(lRow, lCol, lX, lY);
			// should have region refer die list, 
			if( ulRefDist == ulMinDist )
			{
				bFindReferDie = TRUE;
				ulMapRow = lRow;
				ulMapCol = lCol;
				lWftX = lX;
				lWftY = lY;
			}
			szMsg.Format("block refer find region map (%d,%d), wft (%d,%d)", lRow, lCol, lX, lY);
			pUtl->RegionOrderLog(szMsg);
			SearchNml3AtAlignPointForPrescan(lX, lY);
		}

		if( bFindReferDie==FALSE &&	bHasRefer==TRUE )
		{
			lRow = lFirstRow;
			lCol = lFirstCol;
			lWfX = lFirstWfX;
			lWfY = lFirstWfY;

			m_WaferMapWrapper.SetCurrentPosition(lRow, lCol);

			if( XY_SafeMoveTo(lWfX, lWfY)==FALSE )
			{
				szMsg = "Can not find first refer die for this region!\nPlease realign wafer again!";
				HmiMessage_Red_Back(szMsg, "Region Prescan");
				pUtl->RegionOrderLog(szMsg);
				return FALSE;
			}
			Sleep(50);

			GetEncoder(&lX,&lY,&lT);

			bResult = SearchCurrReferDie(&lX, &lY);

			if( bResult==FALSE )
			{
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(lRow, lCol, lUserRow, lUserCol);

				SetJoystickOn(TRUE);
				ChangeCameraToWaferBondMode(FALSE);
				szMsg.Format("Please manual locate refer die(%d,%d) table position by joy stick", lUserRow, lUserCol);
				HmiMessageEx_Red_Back(szMsg, "Region Prescan");
				pUtl->RegionOrderLog(szMsg);
				SetJoystickOn(FALSE);
				ChangeCameraToWaferBondMode(TRUE);
				ChangeCameraToWafer();
				Sleep(50);
				GetEncoder(&lX,&lY,&lT);
				if( labs(lX-lWfX)>labs(GetDiePitchX_X()*5)  ||
					labs(lY-lWfY)>labs(GetDiePitchY_Y()*5) )
				{
					szMsg = "Located refer is 5 times pitch away from original position, please check again";
					HmiMessage(szMsg, "Region Prescan");
					pUtl->RegionOrderLog(szMsg);
				}
				else
				{
					bResult = TRUE;
				}
			}

			if( bResult )
			{
				SetMapPhyPosn(lRow, lCol, lX, lY);
				pUtl->UpdateReRefPosition(i, lRow, lCol, lX, lY);
				pUtl->AddAssistPosition(lRow, lCol, lX, lY);
				szMsg.Format("block refer find region map (%d,%d), wft (%d,%d)", lRow, lCol, lX, lY);
				pUtl->RegionOrderLog(szMsg);

				// should have region refer die list, 
				bFindReferDie = TRUE;
				ulMapRow = lRow;
				ulMapCol = lCol;
				lWftX = lX;
				lWftY = lY;
				SearchNml3AtAlignPointForPrescan(lX, lY);
			}
		}
	}

	m_ulPrescanRefPoints = pUtl->GetAssistPointsNum();

	if( bFindReferDie )
	{
		szMsg.Format("refer region align found");
		pUtl->RegionOrderLog(szMsg);
		XY_SafeMoveTo(lWftX, lWftY);
		Sleep(50);
		WftCheckCurrentGoodDie();
		OpRegionScanStarter(ulMapRow, ulMapCol);
		szMsg.Format("refer region align map (%d,%d), wft (%d,%d)", ulMapRow, ulMapCol, lWftX, lWftY);
		pUtl->RegionOrderLog(szMsg);
	}
	else
	{
		szMsg = "no refer die found in region";
		HmiMessage_Red_Back(szMsg);
		szMsg.Format("%d", ulAlnRegion);
		HmiMessage_Red_Back(szMsg);
	}

	if( bFindReferDie==FALSE )
	{
		ChangeCameraToWaferBondMode(FALSE);
	}

	return bFindReferDie;
}

BOOL CWaferTable::IsRegionAlignRefBlk()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (pUtl->GetPrescanRegionMode() == FALSE)
	{
		return FALSE;
	}

	return ( IsBlkPickAlign() || m_bFindAllRefer4Scan || m_bSingleHomeRegionScan );	// all irregular refer die region scan
}

VOID CWaferTable::SaveRegionStateAndIndex()
{
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMsg;

	for (ULONG i = 0; i < WT_MAX_SUBREGIONS_LIMIT; i++)
	{
		m_lSubRegionState[i] = pSRInfo->GetRegionState(i+1);
	}
	m_lRegionPickIndex = pSRInfo->GetTargetRegionIndex();
	SaveData();

	pUtl->RegionOrderLog("\n");
	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
	szMsg.Format("sorting region %d index %d", ulTgtRegion, m_lRegionPickIndex);
	pUtl->RegionOrderLog(szMsg);

	pSRInfo->SetCurrentSortingRegion_HMI(ulTgtRegion);

	if (!pSRInfo->WriteRegionStateFile())
	{
		return;
	}

	pSRInfo->GetRegionStateNameList();
}


BOOL CWaferTable::AutoRegionAlignManual()
{
//	m_WaferMapWrapper.StartMap();

	X_Sync();
	Y_Sync();
	T_Sync();
	Sleep(50);

	// Move theta home
	T_MoveTo(GetGlobalT(), SFM_WAIT);
	Sleep(50);

	ULONG	ulMapRow = 0, ulMapCol = 0;
	LONG	lWftX = 0, lWftY = 0;

	ulMapRow = m_ulAlignHomeDieRow;
	ulMapCol = m_ulAlignHomeDieCol;
	lWftX = m_nAlignHomeDieWftX;
	lWftY = m_nAlignHomeDieWftY;
	m_WaferMapWrapper.SetCurrentPosition(ulMapRow, ulMapCol);
	SetMapPhyPosn(ulMapRow, ulMapCol, lWftX, lWftY);

	XY_SafeMoveTo(lWftX, lWftY);
	Sleep(50);
	WftCheckCurrentGoodDie();
	OpRegionScanStarter(ulMapRow, ulMapCol);

	return TRUE;
}

LONG CWaferTable::ChangeRegionSortOrder(IPC_CServiceMessage &svMsg)
{
	LONG lSelection = 0;
	LONG lPrevIndex = 0;
	ULONG ulRegionNo = 0;
	BOOL bOK = TRUE;
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMsg;

	svMsg.GetMsg(sizeof(LONG), &lSelection);

	if( pUtl->GetPrescanRegionMode()==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bOK);
		return 1;
	}

	lPrevIndex = pSRInfo->GetTargetRegionIndex();
	m_lRegionPickIndex = lSelection;

	// check region index valid
	if( pSRInfo->IsInvalidRegion(lSelection+1) )
	{
		HmiMessage("Invalid target region selection value");
		bOK = FALSE;
	}

	// check region no valid
	if( bOK )
	{
		ulRegionNo = pSRInfo->GetSortRegion(lSelection);
		if( pSRInfo->IsInvalidRegion(ulRegionNo) )
		{
			HmiMessage("Invalid target region no selection");
			bOK = FALSE;
		}
	}

	// check region state bonded or not
	if( bOK )
	{
		if( pSRInfo->GetRegionState(ulRegionNo)==WT_SUBREGION_STATE_BONDED )
		{
			HmiMessage("Target region is bonded already");
			bOK = FALSE;
		}
	}

	// check region dependancy/coupling with others
	if( bOK )
	{
		if( pSRInfo->CheckRegionCoupled(ulRegionNo) )
		{
			HmiMessage("Target region is coupled by others");
			bOK = FALSE;
		}
	}

	// provide selection list, to change order or to change status
	if( bOK )
	{
		CStringList szList;
		LONG lUserChoose = 0;
		szList.AddTail("Change Region Status to SORTED");
		if( IsRegionAlignRefBlk() )
			szList.AddTail("Change Region to Sorting FIRST");
		lUserChoose = HmiSelection("Region Status Modification", "Region Prescan", szList, lUserChoose);
		CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("Region Status Modification");		//v4.43T10
		switch( lUserChoose )
		{
		case 0:	// change to bonded
			szMsg.Format("Are you sure to change Region %d to SORTED", ulRegionNo);
			if( HmiMessage(szMsg, "Region Prescan", glHMI_MBX_YESNO)==glHMI_YES )
			{
				m_lBuildShortPathInScanning = 0;
				pUtl->RegionOrderLog(szMsg);
				pSRInfo->SetRegionState(ulRegionNo, WT_SUBREGION_STATE_BONDED);
				if( lPrevIndex==lSelection )
				{
					BOOL bFindNext = pSRInfo->FindNextAutoSortRegion(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(),
																	 GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol());
					pUtl->RegionOrderLog("Loop for next valid region(manual select current to SORTED)");

					if( bFindNext==FALSE )
					{
						if( pSRInfo->IsAllRegionsBonded(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(), GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol())==FALSE )
						{
							CString szTemp;
							ULONG k;
							szMsg = "Sorting need to manual align skipped regions ";
							for(k=1; k<=pSRInfo->GetTotalSubRegions(); k++)
							{
								if( pSRInfo->GetRegionState(k)==WT_SUBREGION_STATE_SKIPPED )
								{
									szTemp.Format("(%d) ", k);
									szMsg += szTemp;
								}
							}
							HmiMessage(szMsg, "Region Prescan");
						}
						else
						{
							HmiMessage("All regions sorting complete!", "Region Prescan");
						}
					}

					if( IsRegionAlignRefBlk()==FALSE )
						SetAlignmentStatus(FALSE);	// based on option, for block pick or refer die in region no need reset
					ClearPrescanRawData();	// region prescan, region sorting complete
				}
			}
			break;

		case 1:	// selected region become first to sort
			if( pSRInfo->GetRegionState(ulRegionNo)==WT_SUBREGION_STATE_SKIPPED )
			{
				szMsg.Format("Target region %d is skipped, can not used in auto mode!", ulRegionNo);
				HmiMessage(szMsg, "Region Prescan");
				bOK = FALSE;
				break;
			}
			szMsg.Format("Are you sure to change Region %d to sort first!", ulRegionNo);
			if( HmiMessage(szMsg, "Prescan", glHMI_MBX_YESNO)==glHMI_YES )
			{
				m_lBuildShortPathInScanning = 0;
				pUtl->RegionOrderLog(szMsg);
				if( pSRInfo->SetTargetRegion(ulRegionNo)==FALSE )
				{
					HmiMessage("Set Target Region fail");
					bOK = FALSE;
				}
				else
				{
					if( IsRegionAlignRefBlk()==FALSE )
						SetAlignmentStatus(FALSE);	// based on option, for block pick or refer die in region no need reset
					ClearPrescanRawData();	// region prescan, region sorting complete
				}
			}
			break;
		case -1:
		default:
			break;
		}
	}

	SaveRegionStateAndIndex();

	bOK = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOK);
	return 1;
}

// set by UI use map original coordinate
BOOL CWaferTable::DefineAreaAndWalkPath_RegnScan()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	INT lAlnWfX = GetPrescanAlignPosnX();
	INT lAlnWfY = GetPrescanAlignPosnY();

	for(INT nRow=0; nRow<SCAN_MAX_MATRIX_ROW; nRow++)
	{
		for(INT nCol=0; nCol<SCAN_MAX_MATRIX_COL; nCol++)
		{
			m_baScanMatrix[nRow][nCol] = FALSE;
		}
	}

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	m_lAlignFrameCol = (pSRInfo->GetRegionWftULX() - lAlnWfX) / GetPrescanPitchX();
	m_lAlignFrameRow = (pSRInfo->GetRegionWftULY() - lAlnWfY) / GetPrescanPitchY();

	if( m_lAlignFrameRow>=(SCAN_MAX_MATRIX_ROW-1) )
		m_lAlignFrameRow = SCAN_MAX_MATRIX_ROW-1;
	if( m_lAlignFrameRow<1 )
		m_lAlignFrameRow = 1;
	if( m_lAlignFrameCol>=(SCAN_MAX_MATRIX_COL-1) )
		m_lAlignFrameCol = SCAN_MAX_MATRIX_COL-1;
	if( m_lAlignFrameCol<1 )
		m_lAlignFrameCol = 1;

	m_lLastFrameRow		= m_lNextFrameRow		= GetAlignFrameRow();
	m_lLastFrameCol		= m_lNextFrameCol		= GetAlignFrameCol();
	m_lScanFrameMaxRow	= -1;
	m_lScanFrameMaxCol	= -1;
	m_lScanFrameMinRow	= 9999;
	m_lScanFrameMinCol	= 9999;

	for(INT nRow=0; nRow<SCAN_MAX_MATRIX_ROW; nRow++)
	{
		for(INT nCol=0; nCol<SCAN_MAX_MATRIX_COL; nCol++)
		{
			LONG lDistRow = nRow - GetAlignFrameRow();
			LONG lDistCol = nCol - GetAlignFrameCol();
			INT nPosnX = lAlnWfX - lDistCol * GetPrescanPitchX();
			INT nPosnY = lAlnWfY - lDistRow * GetPrescanPitchY();
			if( IsPosnWithinMapRange(nPosnX, nPosnY) &&
				pSRInfo->IsScanWithinMapRegionRange(nPosnX, nPosnY) && 
				IsWithinWaferLimit(nPosnX, nPosnY) )
			{
				if( m_lScanFrameMinRow>nRow )
					m_lScanFrameMinRow = nRow;
				if( m_lScanFrameMinCol>nCol )
					m_lScanFrameMinCol = nCol;
				if( m_lScanFrameMaxRow<nRow )
					m_lScanFrameMaxRow = nRow;
				if( m_lScanFrameMaxCol<nCol )
					m_lScanFrameMaxCol = nCol;
				m_baScanMatrix[nRow][nCol]	= TRUE;
				m_laScanPosnX[nRow][nCol]	= nPosnX;
				m_laScanPosnY[nRow][nCol]	= nPosnY;
			}
		}
	}

//	if( pUtl->GetPrescanDebug() )
	{
		CString szMsg;
		szMsg.Format("region frame dimension %d,%d==>%d,%d", m_lScanFrameMinRow, m_lScanFrameMinCol, m_lScanFrameMaxRow, m_lScanFrameMaxCol);
		pUtl->PrescanMoveLog(szMsg, TRUE);
		for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
		{
			CString szMsg, szTemp;
			for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
			{
				if( IsScanFrameInWafer(nRow, nCol) )
				{
					szTemp.Format("%d,%d,", m_laScanPosnX[nRow][nCol], m_laScanPosnY[nRow][nCol]);
					szMsg += szTemp;
				}
			}
			pUtl->PrescanMoveLog(szMsg, TRUE);
		}
	
		for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
		{
			nRow = min(nRow, 200);		//Klocwork	//v4.51A20
	
			CString szMsg, szTemp;
			for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
			{
				nCol = min(nCol, 200);		//Klocwork	//v4.51A20
	
				szTemp.Format("%d", m_baScanMatrix[nRow][nCol]);
				szMsg += szTemp;
			}
			pUtl->PrescanMoveLog(szMsg, TRUE);
		}
	}

	m_ucRunScanWalkTour = m_ucScanWalkTour;
	BOOL bFindFirst = FALSE;
	LONG lFirstRow = 0, lFirstCol = 0;
	switch( m_ucRunScanWalkTour )
	{
	case WT_SCAN_WALK_TOP_HORI:
		for(lFirstRow=0; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
		{
			for(lFirstCol=0; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
			{
				if( IsScanFrameInWafer(lFirstRow, lFirstCol) )
				{
					bFindFirst = TRUE;
					break;
				}
			}
			if( bFindFirst )
			{
				break;
			}
		}
		break;
	case WT_SCAN_WALK_LEFT_VERT:
		for(lFirstCol=0; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
		{
			for(lFirstRow=0; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
			{
				if( IsScanFrameInWafer(lFirstRow, lFirstCol) )
				{
					bFindFirst = TRUE;
					break;
				}
			}
			if( bFindFirst )
			{
				break;
			}
		}
		break;
	}
	if( bFindFirst )
	{
		LONG lFirstWfX = m_laScanPosnX[lFirstRow][lFirstCol];
		LONG lFirstWfY = m_laScanPosnY[lFirstRow][lFirstCol];
		LONG lScanRow = m_nPrescanAlignMapRow;
		LONG lScanCol = m_nPrescanAlignMapCol;

		if( GetDiePitchX_X()!=0 && GetDiePitchY_Y()!=0 )
		{
			lScanRow = (GetPrescanAlignPosnY() - lFirstWfX)/GetDiePitchY_Y() + m_nPrescanAlignMapRow;
			lScanCol = (GetPrescanAlignPosnX() - lFirstWfY)/GetDiePitchX_X() + m_nPrescanAlignMapCol;
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
		}
		m_nPrescanNextMapRow	= m_nPrescanLastMapRow	= lScanRow;
		m_nPrescanNextMapCol	= m_nPrescanLastMapCol	= lScanCol;
		m_nPrescanNextWftPosnX	= m_nPrescanLastWftPosnX = lFirstWfX;
		m_nPrescanNextWftPosnY	= m_nPrescanLastWftPosnY = lFirstWfY;
		m_lLastFrameRow		= m_lNextFrameRow	= lFirstRow;
		m_lLastFrameCol		= m_lNextFrameCol	= lFirstCol;
	}

//	if( pUtl->GetPrescanDebug() )
	{
		CString szMoveMsg;

		GetEncoderValue();
		szMoveMsg.Format("GTT, %ld Cur %ld", GetGlobalT(), GetCurrT());
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("Sub Region %ld,%ld", pSRInfo->GetSubRows(), pSRInfo->GetSubCols());
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		LONG lUserRow=0, lUserCol=0;
		ULONG lAlnRow = m_ulAlignHomeDieRow;
		ULONG lAlnCol = m_ulAlignHomeDieCol;
		ConvertAsmToOrgUser(lAlnRow, lAlnCol, lUserRow, lUserCol); 
		szMoveMsg.Format("MAP, Asm(%ld, %ld), user(%ld, %ld)", lAlnRow, lAlnCol, lUserRow, lUserCol);
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("region path Map Valid Dimension, Row(%ld,%ld), Col(%ld,%ld)", 
			GetMapValidMinRow(), GetMapValidMaxRow(), GetMapValidMinCol(), GetMapValidMaxCol());
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("WFT Alignment, %ld, %ld", lAlnWfX, lAlnWfY);
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("EGE, %d, shape %d", pApp->GetPrescanWaferEdgeNum(), m_ucPrescanMapShapeType);
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
	}

	return TRUE;
}

INT	CWaferTable::OpRegionScanStarter(ULONG ulMapRow, ULONG ulMapCol)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMsg;

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
/*
	//===================================================================================================
	//   select the picking grade's order according to the min&max quantity of grade
	//===================================================================================================
	if (pUtl->GetPrescanRegionMode() && !IsDisableWaferMapGradeSelect())
	{
//		m_WaferMapWrapper.StopMap();
		m_WaferMapEvent.SelectGradeToPick();
		m_WaferMapEvent.SetIgnoreGrade();
		m_pWaferMapManager->SuspendSortingPathAlgorithmPreparation();

		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
		ULONG ulStartRow = 0, ulStartCol = 0, ulEndRow = 0, ulEndCol = 0;
		pSRInfo->GetRegion(ulTgtRegion, ulStartRow, ulStartCol, ulEndRow, ulEndCol);
		//select grade to trigger the wafer map picking sequence 
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		m_pWaferMapManager->SelectRegionGradeList(ulStartRow, ulStartCol, ulEndRow, ulEndCol, pApp->GetCustomerName(), m_bKeepLastUnloadGrade, 
												  m_ucLastPickDieGrade, m_lSpecialRefDieGrade, m_bIgnoreRegion,
												  m_lSortingMode, m_ulMinGradeCount, m_ulMinGradeBinNo,  m_bSortMultiToOne, m_unArrMinGradeBin);

		if( szAlgorithm.Find("Sorting Path 1")!=-1 )
		{
			if( m_lBuildShortPathInScanning==0 )	// prestart, short path, first time trigger.
			{
				m_lBuildShortPathInScanning = 1;
				CString szMsg = "WFT: trigger normal short path building in prestart";
				SaveScanTimeEvent(szMsg);
				pUtl->RegionOrderLog(szMsg);
			}
		}
//		m_WaferMapWrapper.RestartMap();
	}
	else
	{
//		if( m_lBuildShortPathInScanning==0 )
//		{
//			//select grade to trigger the wafer map picking sequence 
//			m_WaferMapWrapper.ResetGrade();
//		}
	}
*/
	m_WaferMapWrapper.SetCurrentPosition(ulMapRow, ulMapCol);
	if( m_lBuildShortPathInScanning==0 )
	{
		m_WaferMapWrapper.SetStartPosition(ulMapRow, ulMapCol);
		m_WaferMapWrapper.EnableAutoAlign(TRUE);	//Start bonding at current START position
	}
	GetEncoderValue();
	m_lStart_X	= GetCurrX();
	m_lStart_Y	= GetCurrY();
	m_lStart_T	= GetCurrT();

	pUtl->RegionOrderLog(szMsg);
	PrescanUpdateRegionAlign(m_lStart_X, m_lStart_Y, ulMapRow, ulMapCol);

	//WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
	pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDING);
	SaveRegionStateAndIndex();

	// set algorithm for region prescan
	CString szAgtm, szPath;
	m_WaferMapWrapper.GetAlgorithm(szAgtm, szPath);

	// set the bonding sequance based on the preset region, current is first and bonded is last
	CString szTotal;
	ULONG ulTgtIndex = pSRInfo->GetTargetRegionIndex();
	szTotal = m_pWaferMapManager->SetRegionPickingSequence(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(), GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol(), ulTgtIndex, TRUE);
	szMsg.Format("Redefine %s:%s IN_GRADE_ORDER ", szAgtm, szTotal);
	pUtl->RegionOrderLog(szMsg);

	m_WaferMapWrapper.SetPickMode(WAF_CDieSelectionAlgorithm::IN_GRADE_ORDER);

	szMsg.Format("Redefine Region Sorting List:%s", pSRInfo->m_szRegionOrderList_HMI);
	pUtl->RegionOrderLog(szMsg);


	if( m_lBuildShortPathInScanning==0 )
	{
		//select grade to trigger the wafer map picking sequence 
		m_WaferMapWrapper.ResetGrade();
	}
	m_WaferMapWrapper.StartMap();

	szMsg.Format("Redefine Region Sorting Stat:%s",  pSRInfo->m_szRegionStateList_HMI);
	pUtl->RegionOrderLog(szMsg);

	return gnOK;
}

BOOL CWaferTable::IsNextRegionAutoAlign()	// in prestart cycle to find the target region refer die
{
	if (!IsRegionAlignRefBlk() || m_bManualRegionScanSort )
	{
		return FALSE;
	}

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	if (m_bAlignedWafer && !pSRInfo->IsManualAlignRegion() && m_bSingleHomeRegionScan)
	{
		return TRUE;
	}	//	Next region auto always.

	BOOL	bFindReferDie = FALSE;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	ULONG ulAlnRegion = pSRInfo->GetAssistRegion();

	// during refer die check, update the value
	LONG	lRow, lCol, lWfX, lWfY;

	//Search RefDie
	for(UINT i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		pUtl->GetReRefPosition(i, lRow, lCol, lWfX, lWfY);

		if( IsOutMS90SortingPart(lRow, lCol) )
		{
			continue;
		}
		if( pSRInfo->IsWithinThisRegion(ulAlnRegion, lRow, lCol) )	// auto alignment in blk pick
		{
			bFindReferDie = TRUE;
			break;
		}
	}

	return bFindReferDie;
}	// to check next region is auto or manual


BOOL CWaferTable::FindWaferRegionHomeDie()
{
	IPC_CServiceMessage stMsg;
	LONG lCurrentX, lCurrentY;
	//Align wafer angle
	CMSLogFileUtility::Instance()->WT_LogStatus("Correct wafer angle");
	//Request wafer table stn to update global angle
	Sleep(200);
	GetEncoderValue();
	SetGlobalTheta();

	//Request switch camera to wafer side
	ChangeCameraToWafer();

	CreeAutoSearchMapLimit();	// semi auto button

	//Find HOME die on wafer
	CMSLogFileUtility::Instance()->WT_LogStatus("Find Global Theta start");

	//Move to HomeDie Position
	lCurrentX = m_lHomeDieWftPosnX;
	lCurrentY = m_lHomeDieWftPosnY;
	if (XY_SafeMoveTo(lCurrentX, lCurrentY))
	{
	}
	else 
	{
		return FALSE;
	}

	if( m_bEnableGlobalTheta )
	{
		//Move to HomeDie Position
		LONG lGTX, lGTY;
		lGTX = lCurrentX;
		lGTY = lCurrentY;
		if( m_bNewGTPosition )
		{
			lGTX = m_lNewGTPositionX;
			lGTY = m_lNewGTPositionY;
		}
		if (XY_SafeMoveTo(lGTX, lGTY))
		{
		}
		else 
		{
			return FALSE;
		}

		if (FindGlobalAngle() == FALSE)
		{
			CMSLogFileUtility::Instance()->WT_LogStatus("Cannot correct wafer angle");
			SetErrorMessage("Cannot correct wafer angle");

			SetAlert_Red_Yellow(IDS_WL_CORRECT_ANGLE_FAIL);
			return FALSE;
		}
	}

	//Move to HomeDie Position
	if( m_bNewGTPosition )
	{
		XY_SafeMoveTo(lCurrentX, lCurrentY);
	}

	BOOL bHome = SearchHomeDie();

	// for cree with joystick to locate home die 
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( bHome==FALSE && (pApp->GetCustomerName()=="Cree") )
	{
		LONG lReturn ;
		CString szContent, szTitle;
		szTitle.LoadString(HMB_WT_ALIGN_WAFER);
		szContent.LoadString(HMB_WT_SET_ALIGN_POS);
		SetJoystickOn(TRUE);
		lReturn = HmiMessageEx_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		SetJoystickOn(FALSE);
		if ( lReturn == glHMI_CONTINUE)
		{
			bHome = SearchHomeDie();
		}
	}
	// for cree with joystick to locate home die 

	if( bHome==FALSE )
	{
		CMSLogFileUtility::Instance()->WT_LogStatus("Search Home die failed");
		SetErrorMessage("Search Home die failed");
		SetAlert_Red_Yellow(IDS_WL_SRCH_HOMEDIE_FAIL);
		return FALSE;
	}
	CMSLogFileUtility::Instance()->WT_LogStatus("Find home die finish");

	//Perform auto wafer alignment
	CMSLogFileUtility::Instance()->WT_LogStatus("Align wafer start in auto button");
	if( FullAutoAlignWafer(m_bSearchHomeOption, m_ulCornerSearchOption)==FALSE )
	{
		CMSLogFileUtility::Instance()->WT_LogStatus("Align wafer failed in auto button\n");
		SetErrorMessage("Align wafer failed\n");
		SetAlert_Red_Yellow(IDS_WL_ALIGNWAF_FAIL);
		return FALSE;
	}

	PrescanUpdateWaferAlignment(m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol, 6);	// region sort mode auto cycle

	CMSLogFileUtility::Instance()->WT_LogStatus("Align wafer finish");

	return TRUE;
}

LONG CWaferTable::AutoRegionAlignStdWafer(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMsg;

	m_dStartWaferAlignTime = GetTime();
	if( IsRegionAlignRefBlk() )
	{
		WT_CSubRegionsInfo::Instance()->SetRegionAligned(FALSE);	// for target region finder
		bReturn = IsAlignedWafer();
	}
	else
	{
		pUtl->RegionOrderLog("WT to do region align begin");
		SetAlignmentStatus(FALSE);
		bReturn = FindWaferRegionHomeDie();	// for region prescan alignment during sorting std machine
		szMsg.Format("WT to do region align complete result %d", bReturn);
		pUtl->RegionOrderLog(szMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferTable::SearchCurrReferDie(LONG *lPosX, LONG *lPosY)
{
	BOOL bCheckAllReferDie = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CheckAllReferDie"];
	LONG lReferPRID = 1;
	if( bCheckAllReferDie )
		lReferPRID = 0;

	BOOL bPrReturn = SearchAndAlignReferDie(lReferPRID);

	LONG lT = 0;
	GetEncoder(lPosX, lPosY, &lT);

	return bPrReturn;
}
