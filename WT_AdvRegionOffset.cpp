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
//#include "Encryption.h"
#include "WT_RegionPredication.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferTable::GetRegionDieOffset(LONG lRow, LONG lCol, LONG &lOffsetX, LONG &lOffsetY)
{
	WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();
	ULONG ulSubRow = 0, ulSubCol = 0;

	pRgnPdc->GetInPdcRegionSub(lRow, lCol, ulSubRow, ulSubCol);
	if( pRgnPdc->IsValidPdcRegion(ulSubRow, ulSubCol)==FALSE )
	{
		return FALSE;
	}

	CTime stTime;
	return pRgnPdc->GetRegionOffset(ulSubRow, ulSubCol, lOffsetX, lOffsetY, stTime);
}

BOOL CWaferTable::UpdateRegionDieOffset(LONG lRow, LONG lCol, LONG lOffsetX, LONG lOffsetY, BOOL bToAll)
{
	WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();

	if( lOffsetX==0 && lOffsetY==0 )
	{
		return TRUE;
	}

	if( GetMapDieState(lRow, lCol)==WT_MAP_DIESTATE_SKIP_PREDICTED )
	{
		return TRUE;
	}

	ULONG ulSubRow = 0, ulSubCol = 0;
	if( pRgnPdc->GetInPdcRegionSub(lRow, lCol, ulSubRow, ulSubCol)==FALSE )
	{
		return TRUE;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szMsg;
	//	xu_semitek_2	should compare with previous offset value, if over %, alarm and need to check.
	LONG lOldOffsetX = 0, lOldOffsetY = 0;
	if( GetRegionDieOffset(lRow, lCol, lOldOffsetX, lOldOffsetY) && 
		(m_lPitchX_Tolerance>0 && m_lPitchY_Tolerance>0) && 
		IsDiePitchCheck() )
	{
		if( m_lMC[WTMC_AdvUpdUpPcntLmt]<150 )
		{
			m_lMC[WTMC_AdvUpdUpPcntLmt] = 150;
		}
		UINT unJumpFactor = m_lMC[WTMC_AdvUpdUpPcntLmt];

		LONG lRTolX = m_lPitchX_Tolerance*unJumpFactor/100;
		LONG lRTolY = m_lPitchY_Tolerance*unJumpFactor/100;
		LONG lRLimitX = labs(GetDiePitchX_X())*lRTolX/100;
		LONG lRLimitY = labs(GetDiePitchY_Y())*lRTolY/100;
		LONG lRDeltaX = labs(lOldOffsetX - lOffsetX);
		LONG lRDeltaY = labs(lOldOffsetY - lOffsetY);

		BOOL bRelativeOver = FALSE, bAbsoluteOver = FALSE;
		if( (lRDeltaX>lRLimitX) || (lRDeltaY>lRLimitY) )
		{
			bRelativeOver = TRUE;
		}

		LONG lALimitX = labs(GetDiePitchX_X())*45/100;
		LONG lALimitY = labs(GetDiePitchY_Y())*45/100;
		LONG lADeltaX = labs(lOffsetX);
		LONG lADeltaY = labs(lOffsetY);
		if( (lADeltaX>lALimitX) || (lADeltaY>lALimitY) )
		{
			bAbsoluteOver = TRUE;
		}

		if( bRelativeOver && bAbsoluteOver )
		{
			szMsg = "Region offset update fail, please check on map.";
			HmiMessage_Red_Back(szMsg, "Auto Sort");
			GrabAndSaveImage(0, 1, WPR_GSI_ADV_OFFSET_OVER);

			szMsg.Format("region offset update over, old %d,%d new %d,%d at %d,%d, tolerance %d,%d",
				lOldOffsetX, lOldOffsetY, lOffsetX, lOffsetY, lRow, lCol, lRTolX, lRTolY);
			CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

			szMsg.Format("%d,%d,%lu,%lu,%ld,%ld, jump over,%d,%d,%d,%d,%d,%d",
				lRow, lCol, ulSubRow, ulSubCol, lOffsetX, lOffsetY, 
				lOldOffsetX, lOldOffsetY, lRTolX, lRTolY, GetDiePitchX_X(), GetDiePitchY_Y());
			CMSLogFileUtility::Instance()->WT_SetAdvRgnOffsetLog(szMsg);
			return FALSE;
		}
	}

	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	lDiePitchX_X = labs(lDiePitchX_X);
	lDiePitchY_Y = labs(lDiePitchY_Y);
	if( labs(lOffsetX)>(lDiePitchX_X*3) ||
		labs(lOffsetY)>(lDiePitchY_Y*3) )
	{
		szMsg = "UPD REGN offset value out of range";
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		GrabAndSaveImage(0, 1, WPR_GSI_ADV_OFFSET_OVER);
		szMsg.Format("%d,%d,%lu,%lu,%ld,%ld, out of max range", lRow, lCol, ulSubRow, ulSubCol, lOffsetX, lOffsetY);
		CMSLogFileUtility::Instance()->WT_SetAdvRgnOffsetLog(szMsg);

		return FALSE;
	}

	szMsg.Format("%d,%d,%lu,%lu,%ld,%ld", lRow, lCol, ulSubRow, ulSubCol, lOffsetX, lOffsetY);
	CMSLogFileUtility::Instance()->WT_SetAdvRgnOffsetLog(szMsg);

	if( pApp->GetCustomerName()=="Semitek" && bToAll )
	{
		pRgnPdc->SetDieOffset(lRow, lCol, lOffsetX, lOffsetY, FALSE);	//v4.47 Klocwork
		pRgnPdc->SetDieOffset(lRow, lCol, lOffsetX, lOffsetY, FALSE);
		pRgnPdc->SetDieOffset(lRow, lCol, lOffsetX, lOffsetY, FALSE);
		return TRUE;
	}
	else
	{
		return pRgnPdc->SetDieOffset(lRow, lCol, lOffsetX, lOffsetY, bToAll);
	}
}

BOOL CWaferTable::GetRegionDiePosition( LONG lNextRow, LONG lNextCol, LONG lScanX, LONG lScanY, LONG &lOutPhX, LONG &lOutPhY)
{
	ULONG ulLastSubRow = 0, ulLastSubCol = 0, ulNextSubRow = 0, ulNextSubCol = 0;
	CTime stPickTime, stCurrTime;
	CTimeSpan stDiffTime;

	WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();
	CString szMsg;
	pRgnPdc->GetInPdcRegionSub(lNextRow, lNextCol, ulNextSubRow, ulNextSubCol);

	LONG lLastRow, lLastCol, lLastOffsetX, lLastOffsetY;
	pRgnPdc->GetLastDieData(ulLastSubRow, ulLastSubCol, lLastRow, lLastCol, lLastOffsetX, lLastOffsetY);
	if (labs(lNextRow - lLastRow) <= (LONG)m_ulAdvSampleJumpDistance && 
		labs(lNextCol - lLastCol) <= (LONG)m_ulAdvSampleJumpDistance)
	{
		lOutPhX = lScanX + lLastOffsetX;
		lOutPhY = lScanY + lLastOffsetY;
		szMsg.Format("%d,%d,%lu,%lu,%ld,%ld,%d", lNextRow, lNextCol, ulNextSubRow, ulNextSubCol, lLastOffsetX, lLastOffsetY, -1);
		CMSLogFileUtility::Instance()->WT_GetAdvRgnOffsetLog(szMsg);
		return TRUE;
	}

	LONG lOffsetX = 0, lOffsetY = 0, lCtrOffsetX = 0, lCtrOffsetY = 0, lCounter = 0;

	for(LONG i=0; i<(ADV_RGN_GET_OFFSET_HEIGHT*2+1); i++)
	{
		for(LONG j=0; j<(ADV_RGN_GET_OFFSET_WIDTH*2+1); j++)
		{
			LONG lX = 0, lY = 0;
			LONG lWorkRow = 0, lWorkCol = 0;
			lWorkRow = ((LONG)ulNextSubRow) + (i-ADV_RGN_GET_OFFSET_HEIGHT);
			lWorkCol = ((LONG)ulNextSubCol) + (j-ADV_RGN_GET_OFFSET_WIDTH);

			if( lWorkRow<0 )
				continue;
			if( lWorkCol<0 )
				continue;
			if( lWorkRow>=GetRegionPredicationRow() )
				continue;
			if( lWorkCol>=GetRegionPredicationCol() )
				continue;

			lX = 0;
			lY = 0;
			if( pRgnPdc->GetRegionOffset(lWorkRow, lWorkCol, lX, lY, stPickTime)==FALSE )
				continue;

			if( lX==0 && lY==0 )
				continue;

			if( lWorkRow==ulNextSubRow && lWorkCol==ulNextSubCol )
			{
				lCtrOffsetX = lX;
				lCtrOffsetY = lY;
				continue;
			}

			stCurrTime = CTime::GetCurrentTime();
			stDiffTime = (stCurrTime - stPickTime);

			// no deletion, for later purpose, use time as a factor
			if( stDiffTime.GetTotalSeconds()<m_ulAdvOffsetExpireTime || m_ulAdvOffsetExpireTime==0 )
			{
				lOffsetX = lOffsetX + lX;
				lOffsetY = lOffsetY + lY;
				lCounter++;
			}
		}
	}

	LONG	lCommX = 0;
	LONG	lCommY = 0;

	if( lCounter!=0 )
	{
		if( lCtrOffsetX!=0 && lCtrOffsetY!=0 )
		{
			lCommX = (lOffsetX/lCounter+lCtrOffsetX)/2;
			lCommY = (lOffsetY/lCounter+lCtrOffsetY)/2;
		}
		else
		{
			lCommX = lOffsetX/lCounter;
			lCommY = lOffsetY/lCounter;
		}
	}
	else
	{
		lCommX = lCtrOffsetX;
		lCommY = lCtrOffsetY;
	}

	lOutPhX = lScanX + lCommX;
	lOutPhY = lScanY + lCommY;

	szMsg.Format("%d,%d,%lu,%lu,%ld,%ld,%d", lNextRow, lNextCol, ulNextSubRow, ulNextSubCol, lCommX, lCommY, lCounter);
	CMSLogFileUtility::Instance()->WT_GetAdvRgnOffsetLog(szMsg);

	return TRUE;
}

// -1 or less than 0, all grades regions need sampling, otherwise, only region has target grade need sampling.
BOOL CWaferTable::UpdateAllRegionDieOffset(SHORT sTargetGrade, CONST BOOL bVerifyByFov)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szMsg;
	szMsg.Format("WT - ADV sampling begin at die picked %d", GetNewPickCount());
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLogOpen();
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	SetErrorMessage(szMsg);
	DisplaySequence(szMsg);
	// Move theta home
	LONG lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];
	INT nPrePickCount = 0;
	while (lBHAtPrePick != 1)
	{
		Sleep(10);
		lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];		
		nPrePickCount++;
		if (nPrePickCount >= 1000)
			break;
	}

	X_Sync();
	Y_Sync();
	T_Sync();
	SetEjectorVacuum(FALSE);
	Sleep(100);
	GetEncoderValue();

	LONG lOrigX = GetCurrX();
	LONG lOrigY = GetCurrY();
	LONG lOrigT = GetCurrT();

	szMsg = "WT - move bh to safe position";
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
	DisplaySequence(szMsg);
	WftMoveBondArmToSafe(TRUE);

	UINT unDelay = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Sample Begin Delay"), 0);
	if( unDelay>0 )
		Sleep(unDelay);
	T_MoveTo(GetGlobalT(), SFM_NOWAIT);
	T_Sync();

	UINT unBreak = m_lMC[WTMC_AdvSmpCtrFrqc];
	if( sTargetGrade>=0 )
		unBreak = 1;
	CTime stStartTime;
	stStartTime = CTime::GetCurrentTime();
	//Search RefDie
	szMsg.Format("WT - auto update region start time ; %02d:%02d:%02d;    %d; %d at die picked %d",
		stStartTime.GetHour(), stStartTime.GetMinute(), stStartTime.GetSecond(), sTargetGrade, unBreak, GetNewPickCount());
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
	SetStatusMessage(szMsg);
	DisplaySequence(szMsg);

	// should set all look forward result to none in case wrong result
	WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();
	ULONG ulULRow = 0, ulULCol = 0, ulLRRow = 0, ulLRCol = 0;
	ULONG ulTgtRow = 0, ulTgtCol = 0;
	LONG  lWfX = 0, lWfY = 0, lCurPhX = 0, lCurPhY = 0, lScanX = 0, lScanY = 0;
	LONG lOffsetX = 0, lOffsetY = 0;
	LONG lOldOffsetX = 0, lOldOffsetY = 0, lNewOffsetX = 0, lNewOffsetY = 0;
	CTimeSpan stDiffTime;

	ULONG ulWorkRowIdx = 0, ulWorkColIdx = 0;
	BOOL bBreakSampling = FALSE;
	CString szLog1;
	for(ulWorkRowIdx=0; ulWorkRowIdx<pRgnPdc->GetNumOfPdcRegionRows(); ulWorkRowIdx++)
	{
		if( bBreakSampling == TRUE )
		{
			break;
		}
		for(ULONG ulIdx=0; ulIdx<pRgnPdc->GetNumOfPdcRegionCols(); ulIdx++)
		{
			if( bBreakSampling == TRUE )
			{
				break;
			}

			CTime stPickTime, stSampTime;
			if( (ulWorkRowIdx%2)==0 )
				ulWorkColIdx = ulIdx;
			else
				ulWorkColIdx = pRgnPdc->GetNumOfPdcRegionCols() - 1 - ulIdx;

			m_dPreviousUpdateTime = GetTime();
			szMsg.Format("region %02d,%02d", ulWorkRowIdx, ulWorkColIdx);

			if( pRgnPdc->GetPdcRegionArea(ulWorkRowIdx, ulWorkColIdx, ulULRow, ulULCol, ulLRRow, ulLRCol)==FALSE )
			{
				continue;
			}

			if( (ulLRRow<ulULRow) || (ulLRCol<ulULCol) )
			{
				continue;
			}

			if( pRgnPdc->GetRegionOffset(ulWorkRowIdx, ulWorkColIdx, lOldOffsetX, lOldOffsetY, stPickTime)==FALSE )
			{
				continue;
			}

			if( unBreak>1 )
			{
				if( m_ulPdcRegionSampleCounter%unBreak!=0 )
				{
					if( pRgnPdc->GetRegionInnerState(ulWorkRowIdx, ulWorkColIdx) )
					{
						szLog1 = " : skip inner region";
						szMsg += szLog1;
						CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
						continue;
					}
				}
			}

			stSampTime = CTime::GetCurrentTime();
			stDiffTime = (stSampTime - stPickTime);
			LONGLONG llDiffSeconds = stDiffTime.GetTotalSeconds();

			szLog1.Format(" : last time(%02d:%02d:%02d)",
				stPickTime.GetHour(), stPickTime.GetMinute(), stPickTime.GetSecond() );
			szMsg += szLog1;

			ULONG ulToSmpTimeSpan = m_ulAdvOffsetUpdMinTime;
			if( sTargetGrade>=0 )
				ulToSmpTimeSpan = ulToSmpTimeSpan/2;
			if( ulToSmpTimeSpan>0 && llDiffSeconds<ulToSmpTimeSpan )
			{
				szLog1 = " : fresh and no update";
				szMsg += szLog1;
				CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
				continue;
			}

			szLog1.Format(" : find in block(%3d,%3d) (%3d,%3d)", ulULRow, ulULCol, ulLRRow, ulLRCol);
			szMsg += szLog1;
			CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);

			BOOL bFindRefer = FALSE;
			if( IsBlkPickAlign() || m_bFindAllRefer4Scan)
			{
				bFindRefer = GetValidReferInBlock(ulULRow, ulULCol, ulLRRow, ulLRCol, ulTgtRow, ulTgtCol);
			}

//	xu_semitek	// retry function.
			BOOL bFindADieOK = FALSE;
		for(UINT unRetry=0; unRetry<9; unRetry++)
		{
			bFindADieOK = FALSE;
			if( bFindRefer==FALSE || unRetry!=0 )
			{
				if( GetValidMapDieInBlock(sTargetGrade, ulULRow, ulULCol, ulLRRow, ulLRCol, ulTgtRow, ulTgtCol)==FALSE )
				{
					continue;
				}
			}

			if( GetPrescanWftPosn(ulTgtRow, ulTgtCol, lScanX, lScanY)==FALSE )
			{
				m_WaferMapWrapper.SetDieState(ulTgtRow, ulTgtCol, WT_MAP_DIESTATE_SKIP_PREDICTED);
				continue;
			}

			GetRegionDiePosition(ulTgtRow, ulTgtCol, lScanX, lScanY, lWfX, lWfY);
			if( (IsWithinWaferLimit(lWfX, lWfY) == FALSE) )
			{
				m_WaferMapWrapper.SetDieState(ulTgtRow, ulTgtCol, WT_MAP_DIESTATE_SKIP_PREDICTED);
				continue;
			}

			m_WaferMapWrapper.SetCurrentPosition(ulTgtRow, ulTgtCol);
			lCurPhX = lWfX;
			lCurPhY = lWfY;
			LONG lUserRow = 0, lUserCol = 0;
			ConvertAsmToOrgUser(ulTgtRow, ulTgtCol, lUserRow, lUserCol);
			szLog1.Format("die at map(%3ld,%3ld)(%4d,%4d);", ulTgtRow, ulTgtCol, lUserRow, lUserCol);
			if( bFindRefer==FALSE || unRetry!=0 )
			{
				XY_SafeMoveTo(lCurPhX, lCurPhY);
				if( WftMoveSearchDie(lCurPhX, lCurPhY, TRUE)==FALSE )
				{
					Sleep(1000);
					if( WftMoveSearchDie(lCurPhX, lCurPhY, TRUE)==FALSE )
					{
						szMsg = "    ";
						szMsg += szLog1;
						szLog1.Format(" : Can not find on wft(%ld,%ld), need to do auto recovery", lCurPhX, lCurPhY);
						szMsg += szLog1;
						CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
						m_WaferMapWrapper.SetDieState(ulTgtRow, ulTgtCol, WT_MAP_DIESTATE_SKIP_PREDICTED);
						if( CheckMapWaferByMultiSearch(ulTgtRow, ulTgtCol, lCurPhX, lCurPhY)==FALSE )
						{
							szMsg.Format("    can not locate sample point die");
							CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
							m_WaferMapWrapper.SetDieState(ulTgtRow, ulTgtCol, WT_MAP_DIESTATE_SKIP_PREDICTED);
							m_lWftAdvSamplingResult = 1;
							bBreakSampling = TRUE;
							break;
						}
					}
				}
			}
			else
			{
				if (XY_SafeMoveTo(lCurPhX, lCurPhY) == FALSE)
				{
					continue;
				}
				ChangeCameraToWafer();
				Sleep(50);
				if( BlkFunc2SearchCurrDie(&lCurPhX, &lCurPhY)==FALSE )
				{
					szMsg = "    ";
					szMsg += szLog1;
					szLog1.Format(" : Can not find refer on wft(%ld,%ld), need to retry on normal", lCurPhX, lCurPhY);
					szMsg += szLog1;
					CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
					continue;
				}
			}

			szMsg = "       ";
			szMsg += szLog1;
			szLog1.Format(" : find on wft(%ld,%ld)", lCurPhX, lCurPhY);
			szMsg += szLog1;
			CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
			bFindADieOK = TRUE;
			break;
		}

			if( bBreakSampling )
			{
				break;
			}
			if( bFindADieOK==FALSE )
			{
				continue;
			}
//	xu_semitek
			lOffsetX = lCurPhX - lWfX;
			lOffsetY = lCurPhY - lWfY;
			BOOL bXCheck = CheckPitchX(lOffsetX);
			BOOL bYCheck = CheckPitchY(lOffsetY);
			m_bGetAvgFOVOffset = FALSE;
			BOOL bCtrVerify = FALSE;
			if( pApp->GetCustomerName()=="Semitek" )
			{
				UINT unCtrVerify = m_lMC[WTMC_AdvSmpCtrBase];
				if( (ulToSmpTimeSpan==0) || (ulToSmpTimeSpan>0 && llDiffSeconds>ulToSmpTimeSpan*unCtrVerify) )
				{
					bCtrVerify = TRUE;
				}
			}

			if( (bXCheck==FALSE || bYCheck==FALSE) || bVerifyByFov || bCtrVerify )
			{
				// maybe need to check the match status even die is found and within die pitch tolerance.
				szMsg.Format("To verify die position because pitch error x=%d,y=%d", bXCheck, bYCheck);
				CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
				BOOL bCtrFovOnly = bVerifyByFov;
				if( bCtrVerify )
					bCtrFovOnly = TRUE;
				if( (bXCheck==FALSE || bYCheck==FALSE) )
					bCtrFovOnly = FALSE;
				if( CheckMapWaferByMultiSearch(ulTgtRow, ulTgtCol, lCurPhX, lCurPhY, bCtrFovOnly)==FALSE )
				{
					m_WaferMapWrapper.SetDieState(ulTgtRow, ulTgtCol, WT_MAP_DIESTATE_SKIP_PREDICTED);
					szMsg.Format("Over Pitch Limit and can not pass match check at  map(%ld,%ld); wft(%ld,%ld) to skip",
						ulTgtRow, ulTgtCol, lCurPhX, lCurPhY);
					CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
					m_lWftAdvSamplingResult = 1;
					bBreakSampling = TRUE;
					break;
				}
			}

			SetMapPhyPosn(ulTgtRow, ulTgtCol, lCurPhX, lCurPhY);
			lNewOffsetX = lCurPhX - lScanX;
			lNewOffsetY = lCurPhY - lScanY;
			if( m_bGetAvgFOVOffset )
			{
				lNewOffsetX = m_lAvgFovOffsetX;
				lNewOffsetY = m_lAvgFovOffsetY;
			}

			m_bGetAvgFOVOffset = FALSE;

			szMsg.Format("UPD REGN auto(%03d,%03d), wft(%d,%d)", ulTgtRow, ulTgtCol, lCurPhX, lCurPhY);
			CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
			if( UpdateRegionDieOffset(ulTgtRow, ulTgtCol, lNewOffsetX, lNewOffsetY, TRUE)==FALSE )
			{
				SetAlignmentStatus(FALSE);
				m_lWftAdvSamplingResult = -1;
				bBreakSampling = TRUE;
				break;
			}

			if( m_ulAdvOffsetUpdMinVary>0 )
			{
				LONG lOffsetTol_X = labs(GetDiePitchX_X())*m_ulAdvOffsetUpdMinVary/100;
				LONG lOffsetTol_Y = labs(GetDiePitchY_Y())*m_ulAdvOffsetUpdMinVary/100;
				LONG lOffsetDif_X = lNewOffsetX - lOldOffsetX;
				LONG lOffsetDif_Y = lNewOffsetY - lOldOffsetY;
				if( labs(lOffsetDif_X)<lOffsetTol_X &&
					labs(lOffsetDif_Y)<lOffsetTol_Y )
				{
					ULONG ulSubRow, ulSubCol;
					BOOL bGo = TRUE;
					// right hand
					ulSubRow = ulWorkRowIdx + 0;
					ulSubCol = ulWorkColIdx + 1;
					bGo = TRUE;
					if( bGo && pRgnPdc->IsValidPdcRegion(ulSubRow, ulSubCol) )
					{
						pRgnPdc->GetRegionOffset(ulSubRow, ulSubCol, lOldOffsetX, lOldOffsetY, stPickTime);
						lNewOffsetX = lOldOffsetX + lOffsetDif_X;
						lNewOffsetY = lOldOffsetY + lOffsetDif_Y;
						pRgnPdc->SetRegionOffset(ulSubRow, ulSubCol, lNewOffsetX, lNewOffsetY, TRUE);
						szMsg.Format("%d,%d,%lu,%lu,%ld,%ld", 0, 0, ulSubRow, ulSubCol, lNewOffsetX, lNewOffsetY);
						CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
						CMSLogFileUtility::Instance()->WT_SetAdvRgnOffsetLog(szMsg);
					}
					// lower; left hand
					ulSubRow = ulWorkRowIdx + 1;
					bGo = FALSE;
					if( ulWorkColIdx>=1 )
					{
						ulSubCol = ulWorkColIdx-1;
						bGo = TRUE;
					}
					if( bGo && pRgnPdc->IsValidPdcRegion(ulSubRow, ulSubCol) )
					{
						pRgnPdc->GetRegionOffset(ulSubRow, ulSubCol, lOldOffsetX, lOldOffsetY, stPickTime);
						lNewOffsetX = lOldOffsetX + lOffsetDif_X;
						lNewOffsetY = lOldOffsetY + lOffsetDif_Y;
						pRgnPdc->SetRegionOffset(ulSubRow, ulSubCol, lNewOffsetX, lNewOffsetY, TRUE);
						szMsg.Format("%d,%d,%lu,%lu,%ld,%ld", 0, 0, ulSubRow, ulSubCol, lNewOffsetX, lNewOffsetY);
						CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
						CMSLogFileUtility::Instance()->WT_SetAdvRgnOffsetLog(szMsg);
					}
					// lower, down
					ulSubRow = ulWorkRowIdx + 1;
					ulSubCol = ulWorkColIdx + 0;
					bGo = TRUE;
					if( bGo && pRgnPdc->IsValidPdcRegion(ulSubRow, ulSubCol) )
					{
						pRgnPdc->GetRegionOffset(ulSubRow, ulSubCol, lOldOffsetX, lOldOffsetY, stPickTime);
						lNewOffsetX = lOldOffsetX + lOffsetDif_X;
						lNewOffsetY = lOldOffsetY + lOffsetDif_Y;
						pRgnPdc->SetRegionOffset(ulSubRow, ulSubCol, lNewOffsetX, lNewOffsetY, TRUE);
						szMsg.Format("%d,%d,%lu,%lu,%ld,%ld", 0, 0, ulSubRow, ulSubCol, lNewOffsetX, lNewOffsetY);
						CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
						CMSLogFileUtility::Instance()->WT_SetAdvRgnOffsetLog(szMsg);
					}
					// lower, right hand
					ulSubRow = ulWorkRowIdx + 1;
					ulSubCol = ulWorkColIdx + 1;
					bGo = TRUE;
					if( bGo && pRgnPdc->IsValidPdcRegion(ulSubRow, ulSubCol) )
					{
						pRgnPdc->GetRegionOffset(ulSubRow, ulSubCol, lOldOffsetX, lOldOffsetY, stPickTime);
						lNewOffsetX = lOldOffsetX + lOffsetDif_X;
						lNewOffsetY = lOldOffsetY + lOffsetDif_Y;
						pRgnPdc->SetRegionOffset(ulSubRow, ulSubCol, lNewOffsetX, lNewOffsetY, TRUE);
						szMsg.Format("%d,%d,%lu,%lu,%ld,%ld", 0, 0, ulSubRow, ulSubCol, lNewOffsetX, lNewOffsetY);
						CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
						CMSLogFileUtility::Instance()->WT_SetAdvRgnOffsetLog(szMsg);
					}
				}
			}
		}
	}

	T_MoveTo(lOrigT, SFM_NOWAIT);
	XY_SafeMoveTo(lOrigX, lOrigY);
	T_Sync();

	if( unDelay>0 )
	{
		Sleep(unDelay);
	}
	if( m_bStop==FALSE )
	{
		WftMoveBondArmToSafe(FALSE);
	}
	if( unDelay>0 )
		Sleep(unDelay);

	CTime stEndTime;
	stEndTime = CTime::GetCurrentTime();
	stDiffTime = stEndTime - stStartTime;
	szMsg.Format("WT - auto update region  stop time ;%02d:%02d:%02d; %d",
		stEndTime.GetHour(), stEndTime.GetMinute(), stEndTime.GetSecond(), stDiffTime.GetTotalSeconds());
	SetStatusMessage(szMsg);
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg+"\n");
	DisplaySequence(szMsg);

	m_stLastAdvSampleTime	= CTime::GetCurrentTime();	// after update all regions
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLogClose();
	m_lRescanPECounter		= GetNewPickCount();	//	ADV done, remember last counter.
	m_bToDoSampleOncePE		= FALSE;

	szMsg.Format("WT - ADV sampling done  at die picked %d", GetNewPickCount());
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	SetErrorMessage(szMsg);
	DisplaySequence(szMsg);

	return TRUE;
}


LONG CWaferTable::GetRegionPredicationRow()
{
	if (m_ulPdcRegionSizeRow == 0)
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		m_ulPdcRegionSizeRow = pSRInfo->GetSubRows();
	}

	if (m_ulPdcRegionSizeRow > WT_REGION_PRED_MAX_ROW)
	{
		return WT_REGION_PRED_MAX_ROW;
	}

	return m_ulPdcRegionSizeRow;
}

LONG CWaferTable::GetRegionPredicationCol()
{
	if (m_ulPdcRegionSizeCol == 0)
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		m_ulPdcRegionSizeCol = pSRInfo->GetSubCols();
	}

	if( m_ulPdcRegionSizeCol>WT_REGION_PRED_MAX_COL )
		return WT_REGION_PRED_MAX_COL;

	return m_ulPdcRegionSizeCol;
}


BOOL CWaferTable::InitAdvRegionPredication()
{
	LONG i,j;
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	ULONG ulULRow, ulULCol, ulLRRow, ulLRCol;
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();

	ULONG ulMinRow = GetMapValidMinRow();
	ULONG ulMinCol = GetMapValidMinCol();
	ULONG ulMaxRow = GetMapValidMaxRow();
	ULONG ulMaxCol = GetMapValidMaxCol();
	if( pUtl->GetPrescanRegionMode() )
	{
		if( pSRInfo->GetRegion(pSRInfo->GetTargetRegion(), ulMinRow, ulMinCol, ulMaxRow, ulMaxCol)!=TRUE )
		{
			return FALSE;
		}
	}
	if( ulMinRow>0 )
		ulMinRow--;
	if( ulMinCol>0 )
		ulMinCol--;
	ulMaxRow++;
	ulMaxCol++;

	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLogOpen();
	LONG lSpanRow = (LONG) (ulMaxRow - ulMinRow);
	LONG lSpanCol = (LONG) (ulMaxCol - ulMinCol);

	LONG lSubHeight = (lSpanRow + GetRegionPredicationRow() - 1) / GetRegionPredicationRow();
	LONG lSubWidth  = (lSpanCol + GetRegionPredicationCol() - 1) / GetRegionPredicationCol();

	//	sub region info, initialization
	pRgnPdc->InitPdcRegions(GetRegionPredicationRow(), GetRegionPredicationCol());

	szMsg.Format("Init region size (%d,%d) map dim(%d,%d)(%d,%d,%d,%d)", 
		GetRegionPredicationRow(), GetRegionPredicationCol(), lSpanRow, lSpanCol,
		ulMinRow, ulMinCol, ulMaxRow, ulMaxCol);
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
	szMsg.Format("Pred. Time %d, Sample Time %d; Sample Tolerance %d",
		m_ulAdvOffsetExpireTime, m_ulAdvOffsetUpdMinTime, m_ulAdvOffsetUpdMinVary);
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
	szMsg.Format("Stage 1 %d,%d; Stage 2 %d,%d; Stage 3 %d,%d",
		m_ulAdvStage1StartCount, m_ulAdvStage1SampleSpan, m_ulAdvStage2StartCount, m_ulAdvStage2SampleSpan, m_ulAdvStage3StartCount, m_ulAdvStage3SampleSpan);
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);

	UCHAR ucOffset	= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	UINT unPcnt = m_lMC[WTMC_AdvSmpCtrToEdge];
	DOUBLE dPcnt = (DOUBLE)unPcnt/100.0;
	LONG lRowSpan	= labs(ulMaxRow - ulMinRow);
	DOUBLE dLimit	= sqrt((DOUBLE)(lRowSpan * lRowSpan / 4 + lRowSpan * lRowSpan / 4)) * dPcnt;
	szMsg.Format("Edge ration %d, %f, radius %f", unPcnt, dPcnt, dLimit);
	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);

	for (j=0; j<GetRegionPredicationRow(); j++)
	{
		for (i=0; i<GetRegionPredicationCol(); i++)
		{
			ulULRow = j*lSubHeight + ulMinRow;
			ulULCol = i*lSubWidth + ulMinCol;
			ulLRRow = ulULRow + lSubHeight;
			ulLRCol = ulULCol + lSubWidth;
			if( ulLRRow>=ulMaxRow )
				ulLRRow = ulMaxRow;
			if( ulLRCol>=ulMaxCol )
				ulLRCol = ulMaxCol;

			BOOL bInner =	IsWithinValidMapLimit(ulULRow, ulULCol, dPcnt) &&
							IsWithinValidMapLimit(ulULRow, ulLRCol, dPcnt) &&
							IsWithinValidMapLimit(ulLRRow, ulULCol, dPcnt) &&
							IsWithinValidMapLimit(ulLRRow, ulLRCol, dPcnt) ;
			pRgnPdc->SetRegionInnerState(j, i, bInner);
			pRgnPdc->SetPdcRegionArea(j, i, ulULRow, ulULCol, ulLRRow, ulLRCol);

			BOOL	bFindDie = FALSE;
			CUIntArray aulAllGradeList;
			int kk = 0, mmmm = 0;
			ULONG m, n;
			m_WaferMapWrapper.GetAvailableGradeList(aulAllGradeList);

			for(m=ulULRow; m<ulLRRow; m++)
			{
				for(n=ulULCol; n<ulLRCol; n++)
				{
					UCHAR ucGrade = m_WaferMapWrapper.GetGrade(m, n) - ucOffset;
					if( m_WaferMapWrapper.IsReferenceDie(m, n) )
					{
						continue;
					}

					if( m_bUseEmptyCheck && m_lMnNoDieGrade>0 && ucGrade==m_lMnNoDieGrade )
					{
						continue;
					}

					if( ucGrade==ucNullBin )
					{
						continue;
					}

					for(kk=0; kk<aulAllGradeList.GetSize(); kk++)
					{
						if( ((UCHAR)(aulAllGradeList.GetAt(kk)-ucOffset))==ucGrade )
						{
							mmmm++;
							break;
						}
					}
					if( mmmm>1 )
					{
						bFindDie = TRUE;
                        break;
					}
				}
				if( bFindDie )
				{
					break;
				}
			}

			if( bFindDie )
			{
				pRgnPdc->SetRegionPdcState(j, i, WT_REGION_PRED_STATE_INUSE);
			}
			else
			{
				pRgnPdc->SetRegionPdcState(j, i, WT_REGION_PRED_STATE_UNUSED);
			}

			szMsg.Format("region sub(%2d,%2d), UL(%3d,%3d), LR(%3d,%3d), Inner %d", 
				j, i, ulULRow, ulULCol, ulLRRow, ulLRCol, bInner);
			CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
		}
	}

	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLogClose();
	return TRUE;
}

BOOL CWaferTable::IsLFSizeOK()
{
	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
	DOUBLE dFov = min(dFOVX, dFOVY);

	return dFov>=WPR_LF_SIZE;
}

BOOL CWaferTable::GetFeaturePatternInBlock(ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol, ULONG &ulOutRow, ULONG &ulOutCol)
{
	BOOL	bFindTgtDie = FALSE;
	LONG	lTmpRow, lTmpCol;
	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;
	LONG lPhyX = 0, lPhyY = 0;
	ULONG ulNumRow = 0, ulNumCol = 0, ulLoop;
	BOOL	bFindFirst = FALSE;
	ULONG ulFirstRow = 0, ulFirstCol = 0;

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol))
	{
		return FALSE;
	}

	ULONG ulIntRow = (ulULRow + ulLRRow)/2;
	ULONG ulIntCol = (ulULCol + ulLRCol)/2;

	ulLoop = max(ulLRRow-ulULRow, ulLRCol-ulULCol);
	if( ulLoop>max(ulNumRow, ulNumCol) )
		ulLoop = max(ulNumRow, ulNumCol);
	ulLoop = ulLoop/2;

	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
		CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol)	)
	{
		if( CheckEmptyAroundDie(lTmpRow, lTmpCol, 1, 1) )
		{
			bFindTgtDie = TRUE;
		}
		if( bFindFirst==FALSE )
		{
			bFindFirst = TRUE;
			ulFirstRow = lTmpRow;
			ulFirstCol = lTmpCol;
		}
	}

	for (ulCurrentLoop=1; ulCurrentLoop<=ulLoop; ulCurrentLoop++)
	{
		if( bFindTgtDie )
		{
			break;
		}
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
			if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
				CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol) )
			{
				if( CheckEmptyAroundDie(lTmpRow, lTmpCol, 1, 1) )
				{
					bFindTgtDie = TRUE;
					break;
				}
				if( bFindFirst==FALSE )
				{
					bFindFirst = TRUE;
					ulFirstRow = lTmpRow;
					ulFirstCol = lTmpCol;
				}
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
				if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
					CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol) )
				{
					if( CheckEmptyAroundDie(lTmpRow, lTmpCol, 1, 1) )
					{
						bFindTgtDie = TRUE;
						break;
					}
					if( bFindFirst==FALSE )
					{
						bFindFirst = TRUE;
						ulFirstRow = lTmpRow;
						ulFirstCol = lTmpCol;
					}
				}
			}
		}
		if( bFindTgtDie )
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
				if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
					CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol) )
				{
					if( CheckEmptyAroundDie(lTmpRow, lTmpCol, 1, 1) )
					{
						bFindTgtDie = TRUE;
						break;
					}
					if( bFindFirst==FALSE )
					{
						bFindFirst = TRUE;
						ulFirstRow = lTmpRow;
						ulFirstCol = lTmpCol;
					}
				}
			}
		}
		if( bFindTgtDie )
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
				if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
					CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol) )
				{
					if( CheckEmptyAroundDie(lTmpRow, lTmpCol, 1, 1) )
					{
						bFindTgtDie = TRUE;
						break;
					}
					if( bFindFirst==FALSE )
					{
						bFindFirst = TRUE;
						ulFirstRow = lTmpRow;
						ulFirstCol = lTmpCol;
					}
				}
			}
		}
		if( bFindTgtDie )
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
				if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
					CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol) )
				{
					if( CheckEmptyAroundDie(lTmpRow, lTmpCol, 1, 1) )
					{
						bFindTgtDie = TRUE;
						break;
					}
					if( bFindFirst==FALSE )
					{
						bFindFirst = TRUE;
						ulFirstRow = lTmpRow;
						ulFirstCol = lTmpCol;
					}
				}
			}
		}
		if( bFindTgtDie )
		{
			break;
		}
	}

	if( bFindTgtDie )
	{
		ulOutRow = lTmpRow;
		ulOutCol = lTmpCol;
	}
	if( bFindFirst )
	{
		ulOutRow = ulFirstRow;
		ulOutCol = ulFirstCol;
	}

	return bFindTgtDie ||bFindFirst;
}

BOOL CWaferTable::CheckEmptyAroundDie(LONG lRow, LONG lCol, LONG lLoopRow, LONG lLoopCol)
{
	for(LONG i=lRow-lLoopRow; i<=lRow+lLoopRow; i++)
	{
		for(LONG j=lCol-lLoopCol; j<=lCol+lLoopCol; j++)
		{
			if( CheckAlignMapWaferMatchHasDie(i, j)==FALSE )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CWaferTable::GetValidReferInBlock(ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol, ULONG &ulOutRow, ULONG &ulOutCol)
{
	LONG	lRow = 0, lCol = 0, lWfX = 0, lWfY = 0;
	CString szMsg;
	BOOL bReturn = FALSE;

	szMsg.Format("USE REFER no finding at map (%ld,%ld) (%ld,%ld)", ulULRow, ulULCol, ulLRRow, ulLRCol);
	//Search RefDie
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	for(UINT i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		pUtl->GetReRefPosition(i, lRow, lCol, lWfX, lWfY);

		if( (lRow >= (LONG)ulULRow) && (lRow < (LONG)ulLRRow) && (lCol >= (LONG)ulULCol) && (lCol < (LONG)ulLRCol) )
		{
			szMsg.Format("USE REFER (%ld,%ld) in map (%ld,%ld) (%ld,%ld)", lRow, lCol, ulULRow, ulULCol, ulLRRow, ulLRCol);
			if( GetMapPhyPosn(lRow, lCol, lWfX, lWfY) )
			{
				ulOutRow = lRow;
				ulOutCol = lCol;
				bReturn = TRUE;
				szMsg.Format("USE REFER find (%ld,%ld) in map (%ld,%ld) (%ld,%ld)", lRow, lCol, ulULRow, ulULCol, ulLRRow, ulLRCol);
				break;
			}

		}
	}

	CMSLogFileUtility::Instance()->WT_GetAdvRgnSampleLog(szMsg);
	return bReturn;
}

BOOL CWaferTable::CheckMapAndFindWaferMatch(LONG lTgtRow, LONG lTgtCol, LONG &lTgtX, LONG &lTgtY, BOOL bXCheck, BOOL bYCheck, BOOL bCtrFovOnly)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsVerifyMapWaferEnable()==FALSE )
	{
		return TRUE;
	}

	//	xu_semitek
	if( IsAdvRegionOffset() && IsLFSizeOK() )
	{
		return CheckMapWaferByMultiSearch(lTgtRow, lTgtCol, lTgtX, lTgtY, bCtrFovOnly);
	}

	LONG lMapRow = 0, lMapCol = 0;
	ConvertAsmToOrgUser(lTgtRow, lTgtCol, lMapRow, lMapCol);
	CString szMsg;
	szMsg.Format("PCI Check Map and Find Wafer Match: map %d,%d (%d,%d), mve(%d,%d); as x=%d,y=%d", 
					lTgtRow, lTgtCol, lMapRow, lMapCol, lTgtX, lTgtY, bXCheck, bYCheck);
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

	m_dPreviousUpdateTime = GetTime();
	LONG lX = lTgtX;
	LONG lY = lTgtY;
	WftMoveSearchDie(lX, lY, TRUE);

	// maybe need to check the match status even die is found and within die pitch tolerance.
	if( CheckMapWaferMatch(lTgtRow, lTgtCol, lX, lY, lX, lY) )
	{
		lTgtX = lX;
		lTgtY = lY;
		szMsg.Format("PCI Check Map and Find Wafer Match: new wft(%d,%d) same", lX, lY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return TRUE;
	}

	LONG lCurPhX = lX;
	LONG lCurPhY = lY;
	LONG lDiePitchX_X	= GetDiePitchX_X();
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiffCol = 0, lDiffRow = 0;
	BOOL bFindMatch = FALSE;
	LONG lStartRow = -1, lEndRow = 1, lStartCol = -1, lEndCol = 1;
	if( bXCheck==FALSE )
	{
		lStartCol = lEndCol = 0;
	}
	if( bYCheck==FALSE )
	{
		lStartRow = lEndRow = 0;
	}

	LONG lScanX = 0, lScanY = 0;
	GetPrescanWftPosn(lTgtRow, lTgtCol, lScanX, lScanY);
	szMsg.Format("PCI Check Map and Find Wafer Match: scan (%d,%d) move around X(%d,%d) Y(%d,%d)", 
		lScanX, lScanY, lStartRow, lEndRow, lStartCol, lEndCol);
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
	for(LONG lDiffRow=lStartRow; lDiffRow<=lEndRow; lDiffRow++)
	{
		for(LONG lDiffCol=lStartCol; lDiffCol<=lEndCol; lDiffCol++)
		{
			if( lDiffRow==0 && lDiffCol==0 )
			{
				continue;
			}

			lX = lCurPhX - (lDiffCol) * lDiePitchX_X - (lDiffRow) * lDiePitchY_X;
			lY = lCurPhY - (lDiffRow) * lDiePitchY_Y - (lDiffCol) * lDiePitchX_Y;
			if( WftMoveSearchDie(lX, lY, TRUE)==FALSE )
			{
				continue;
			}

			WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();

			// maybe need to check the match status even die is found and within die pitch tolerance.
			if( CheckMapWaferMatch(lTgtRow, lTgtCol, lX, lY, lX, lY) )
			{
				lTgtX = lX;
				lTgtY = lY;
				bFindMatch = TRUE;
				break;
			}
		}
		if( bFindMatch )
		{
			break;
		}
	}

	szMsg.Format("PCI Check Map and Find Wafer Match: new wft(%ld,%ld) ok=%d; at last",
		lTgtX, lTgtY, bFindMatch);
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

	return bFindMatch;
}

LONG CWaferTable::WaferTableDoAdvSampling(IPC_CServiceMessage &svMsg)
{
	LONG lTgtGrade = -1;
	svMsg.GetMsg(sizeof(LONG), &lTgtGrade);

	if( m_bWftDoSamplingByBT && IsAutoSampleSetupOK() )
	{
		if( IsAdvRegionOffset() && (GetNewPickCount()>=m_ulAdvStage1SampleSpan) )
		{
			DisplaySequence("WT - BT let WFT to do adv sampling");
			CTime stStartTime = CTime::GetCurrentTime();
			UpdateAllRegionDieOffset(-1);	//	lTgtGrade
			if( m_lWftAdvSamplingResult==0 )
			{
				RescanAdvOffsetNextLimit();
			}
			CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AdvSampleBT");
		}
		else if( DEB_IsUseable() && (GetNextXXXCounter()>1000) && 
					(GetNewPickCount()>=(GetNextXXXCounter()-1000)) )
		{
			DisplaySequence("WT - BT let WFT to do key die sampling");
			CTime stStartTime = CTime::GetCurrentTime();
			Sleep(200);
			m_ulPdcRegionSampleCounter++;
			BOOL bReturn = DEB_AutoRealignKeyDie();	// do sample when realign key die
			RescanAdvOffsetNextLimit();
			CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "RealignKeyDieBT");
		}
	}

	m_lWftAdvSamplingStage	= 0;
	m_bFirstInAutoCycle = TRUE;	//	BT change frame, after wafer table sampling done

	return 1;
}


BOOL CWaferTable::CheckMapWaferByMultiSearch(LONG lTgtRow, LONG lTgtCol, LONG &lOutX, LONG &lOutY, BOOL bCtrFovOnly)
{
	if( !IsVerifyMapWaferEnable() )
	{
		return TRUE;
	}

	if( !IsLFSizeOK() || !IsAdvRegionOffset() )
	{
		return FALSE;
	}

	LONG lMapRow = 0, lMapCol = 0;
	ConvertAsmToOrgUser(lTgtRow, lTgtCol, lMapRow, lMapCol);
	CString szMsg;
	szMsg.Format("PCM Check MapWafer map %d,%d (%d,%d), mve(%d,%d)\n", 
					lTgtRow, lTgtCol, lMapRow, lMapCol, lOutX, lOutY);
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

	ChangeCameraToWafer();

	m_dPreviousUpdateTime = GetTime();
	LONG lTgtX = lOutX;
	LONG lTgtY = lOutY;
	WftMoveSearchDie(lTgtX, lTgtY, TRUE);

	// maybe need to check the match status even die is found and within die pitch tolerance.
	if( MatchMapWaferByMultiSearch(lTgtRow, lTgtCol, lTgtX, lTgtY, bCtrFovOnly) )
	{
		lOutX = lTgtX;
		lOutY = lTgtY;
		XY_SafeMoveTo(lOutX, lOutY);
		szMsg.Format("PCM check Map and Wafer Match MS: new wft(%d,%d)\n", lOutX, lOutY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return TRUE;
	}
	else
	{
		if( PopVerifyMatchCheckMapError(lTgtRow, lTgtCol) )
		{
			szMsg.Format("PCM check Map and Wafer Match MS: verified manually before\n", lTgtRow, lTgtCol);
			CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
			return TRUE;
		}

		CMSLogFileUtility::Instance()->WT_PitchAlarmLog("PCM fail to check map and wafer match\n");
	}

	return FALSE;
}

BOOL CWaferTable::MatchMapWaferByMultiSearch(LONG lRow, LONG lCol, LONG &lTgtX, LONG &lTgtY, BOOL bCtrFovOnly)
{
	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
	CString szMsg;

	m_bGetAvgFOVOffset	= FALSE;
	m_lAvgFovOffsetX	= 0;
	m_lAvgFovOffsetY	= 0;

	//
	//	0	1	2
	//	3	4	5
	//	6	7	8
	//

	LONG lChkRow = lRow;
	LONG lChkCol = lCol;

	MultiSrchInitNmlDie1(FALSE);	// hmi multi search test button

	m_lAdvMostMatchRow = 1;
	m_lAdvMostMatchCol = 1;
	BOOL bMapMatchOK = FALSE;

	BOOL bMatch1OK = FALSE, bMatch2OK = FALSE;
	BOOL bAvgOffset1 = FALSE, bAvgOffset2 = FALSE;
	LONG lTgt1X = lTgtX, lTgt1Y = lTgtY, lTgt2X = lTgtX, lTgt2Y = lTgtY;
	LONG lAvgOffset1X = 0, lAvgOffset1Y = 0, lAvgOffset2X = 0, lAvgOffset2Y = 0;

for(int iMatchTry=1; iMatchTry<=2; iMatchTry++)
{
	for(int i=0; i<5; i++)
	{
		switch( i )
		{
		case 0:
			lChkRow = lRow;
			lChkCol = lCol;
			szMsg = "C Verify ";
			break;
		case 1:
			lChkRow = lRow;
			lChkCol = lCol - (LONG)dFOVX;
			szMsg = "L Verify ";
			break;
		case 2:
			lChkRow = lRow;
			lChkCol = lCol + (LONG)dFOVX;
			szMsg = "R Verify ";
			break;
		case 3:
			lChkRow = lRow - (LONG)dFOVY;
			lChkCol = lCol;
			szMsg = "U Verify ";
			break;
		case 4:
			lChkRow = lRow + (LONG)dFOVY;
			lChkCol = lCol;
			szMsg = "D Verify ";
			break;
		}

		if( !IsInMapValidRange(lChkRow, lChkCol) )
		{
			continue;
		}

		if( i!=0 )
		{
			ULONG lOutRow = lChkRow;
			ULONG lOutCol = lChkCol;
			if( FindExistMapDieInBlock(3, 3, 3, 3, lOutRow, lOutCol)!=1 )
			{
				continue;
			}
			else
			{
				lChkRow = lOutRow;
				lChkCol = lOutCol;
			}
		}
		BOOL bFindOK = FALSE;
		LONG lGetX=0, lGetY=0, lMapX = 0, lMapY = 0, lScanX=0, lScanY=0;
		if( GetMapPhyPosn(lChkRow, lChkCol, lMapX, lMapY) &&
			GetPrescanWftPosn(lChkRow, lChkCol, lScanX, lScanY) )
		{
			WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();
			LONG lVOffsetX = 0, lVOffsetY = 0;
			if( pRgnPdc->GetVerifyOffset(lVOffsetX, lVOffsetY) )
			{
				lGetX = lScanX + lVOffsetX;
				lGetY = lScanY + lVOffsetY;

				ULONG ulSubRow = 0, ulSubCol = 0;
				pRgnPdc->GetInPdcRegionSub(lChkRow, lChkCol, ulSubRow, ulSubCol);
				szMsg.Format("%d,%d,%lu,%lu,%ld,%ld,%d", lChkRow, lChkCol, ulSubRow, ulSubCol, lVOffsetX, lVOffsetY, -2);
				CMSLogFileUtility::Instance()->WT_GetAdvRgnOffsetLog(szMsg);

				bFindOK = TRUE;
			}
			else
			{
				bFindOK = GetRegionDiePosition(lChkRow, lChkCol, lScanX, lScanY, lGetX, lGetY);
			}
		}

		if( i==0 && !bFindOK )
		{
			break;
		}

		BOOL bCtrFOV = FALSE;
		if( i==0 )
		{
			bCtrFOV = TRUE;
		}
		if( !bFindOK )
		{
			LONG lWorkRow = lChkRow;
			LONG lWorkCol = lChkCol;
			for(int j=0; j<9; j++)
			{
				lWorkRow = lChkRow - 1 + j/3;
				lWorkCol = lChkCol - 1 + j%3;
				if( !IsInMapValidRange(lWorkRow, lWorkCol) )
				{
					continue;
				}

				if( GetMapPhyPosn(lWorkRow, lWorkCol, lMapX, lMapY) &&
					GetPrescanWftPosn(lWorkRow, lWorkCol, lScanX, lScanY) )
				{
					WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();
					LONG lVOffsetX = 0, lVOffsetY = 0;
					if( pRgnPdc->GetVerifyOffset(lVOffsetX, lVOffsetY) )
					{
						lGetX = lScanX + lVOffsetX;
						lGetY = lScanY + lVOffsetY;

						ULONG ulSubRow = 0, ulSubCol = 0;
						pRgnPdc->GetInPdcRegionSub(lWorkRow, lWorkCol, ulSubRow, ulSubCol);
						szMsg.Format("%d,%d,%lu,%lu,%ld,%ld,%d", lWorkRow, lWorkCol, ulSubRow, ulSubCol, lVOffsetX, lVOffsetY, -2);
						CMSLogFileUtility::Instance()->WT_GetAdvRgnOffsetLog(szMsg);
						bFindOK = TRUE;
					}
					else
					{
						bFindOK = GetRegionDiePosition(lWorkRow, lWorkCol, lScanX, lScanY, lGetX, lGetY);
					}
				}

				if( bFindOK )
				{
					lChkRow = lWorkRow;
					lChkCol = lWorkCol;
					break;
				}
			}
		}

		if( bFindOK==FALSE )
		{
			CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg + "NO die found");
			continue;
		}

		CString szTemp;
		szTemp.Format("begin at %d,%d wft %d,%d", lChkRow, lChkCol, lGetX, lGetY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg + szTemp);
		bMapMatchOK = CheckSubMapWaferByMultiSearch(lChkRow, lChkCol, lGetX, lGetY, bCtrFOV, iMatchTry);
		if( bMapMatchOK==FALSE )
		{
			(*m_psmfSRam)["MS896A"]["WaferTableX"] = lGetX;
			(*m_psmfSRam)["MS896A"]["WaferTableY"] = lGetY;
			(*m_psmfSRam)["MS896A"]["WaferMapRow"] = lChkRow;
			(*m_psmfSRam)["MS896A"]["WaferMapCol"] = lChkCol;
			GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_PCM);	// during pitch verification, pr grab and sceen save and log file

			break;
		}

		if( bCtrFOV )
		{
			if( iMatchTry==1 )
			{
				if( m_bGetAvgFOVOffset )
				{
					bAvgOffset1 = TRUE;
					lAvgOffset1X = m_lAvgFovOffsetX;
					lAvgOffset1Y = m_lAvgFovOffsetY;
				}
				lTgt1X = lGetX;
				lTgt1Y = lGetY;
			}
			else
			{
				if( m_bGetAvgFOVOffset )
				{
					bAvgOffset2 = TRUE;
					lAvgOffset2X = m_lAvgFovOffsetX;
					lAvgOffset2Y = m_lAvgFovOffsetY;
				}
				lTgt2X = lGetX;
				lTgt2Y = lGetY;
			}
		}

		if( m_lAdvMostMatchRow==1 && m_lAdvMostMatchCol==1 && bCtrFovOnly )
		{
			break;
		}	// for long time check
	}

	if( iMatchTry==1 )
		bMatch1OK = bMapMatchOK;
	else
		bMatch2OK = bMapMatchOK;
	if( m_lAdvMostMatchRow==1 && m_lAdvMostMatchCol==1 && bMapMatchOK )
	{
		break;
	}	// for long time check
}

// first time not at 1,1, must to do match 2 
	m_WaferMapWrapper.SetCurrentPosition(lRow, lCol);
	if( bMatch2OK )
	{
		bMapMatchOK = bMatch2OK;
		lTgtX = lTgt2X;
		lTgtY = lTgt2Y;
		m_bGetAvgFOVOffset = bAvgOffset1;
		m_lAvgFovOffsetX = lAvgOffset1X;
		m_lAvgFovOffsetY = lAvgOffset1Y;
	}
	else if( bMatch1OK )
	{
		bMapMatchOK = bMatch1OK;
		lTgtX = lTgt1X;
		lTgtY = lTgt1Y;
		m_bGetAvgFOVOffset = bAvgOffset2;
		m_lAvgFovOffsetX = lAvgOffset2X;
		m_lAvgFovOffsetY = lAvgOffset2Y;
	}
	else
	{
		bMapMatchOK = FALSE;
	}

	XY_SafeMoveTo(lTgtX, lTgtY);

	WftCheckCurrentGoodDie();

	return bMapMatchOK;
}


BOOL CWaferTable::CheckSubMapWaferByMultiSearch(LONG lWorkRow, LONG lWorkCol, LONG &lGetX, LONG &lGetY, BOOL bCtrBlock, INT iMatchTry)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	WT_CRegionPredication *pRgnPdc = WT_CRegionPredication::Instance();

	CString szMsg, szTemp;

	m_WaferMapWrapper.SetCurrentPosition(lWorkRow, lWorkCol);

	if( XY_SafeMoveTo(lGetX, lGetY)==FALSE )
	{
		szMsg.Format("PCM fail Safe move to posn error");
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return FALSE;
	}
	Sleep(50);

	LONG lScanX = 0, lScanY = 0;
	if( GetPrescanWftPosn(lWorkRow, lWorkCol, lScanX, lScanY)==FALSE )
	{
		szMsg.Format("PCM fail die %d,%d has no valid prescan position", lWorkRow, lWorkCol);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return FALSE;
	}

	if( WftSearchNormalDie(lGetX, lGetY, TRUE) )
	{
		if( bCtrBlock )
		{
			pRgnPdc->SetVerifyOffset(lGetX - lScanX, lGetY - lScanY);
		}
	}

	LONG lGrabX = lGetX;
	LONG lGrabY = lGetY;
	WT_LA_DIE_RESULT	stMapDieState, stWafDieState;
	memset(&stMapDieState, 0, sizeof(WT_LA_DIE_RESULT));
	memset(&stWafDieState, 0, sizeof(WT_LA_DIE_RESULT));

	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];

	INT lWidth, lHeight;

	lWidth = (LONG) dFOVX;
	lHeight = (LONG) dFOVY;

	if (lWidth / 2 == 0)
	{
		lWidth = lWidth - 1;
	}
	if (lHeight / 2 == 0)
	{
		lHeight = lHeight - 1;
	}

	lWidth = lWidth / 2 - 1;
	lHeight = lHeight / 2 - 1;

	if (lWidth < 1)
	{
		lWidth = 1;
	}
	if (lWidth > WT_LA_CTR_DIE)
	{
		lWidth = WT_LA_CTR_DIE;
	}
	if (lHeight < 1)
	{
		lHeight = 1;
	}
	if (lHeight > WT_LA_CTR_DIE)
	{
		lHeight = WT_LA_CTR_DIE;
	}

	szMsg.Format("Search Matrix Row %d, Col %d; map center (%d,%d)", 
		lHeight*2+1, lWidth*2+1, lWorkRow, lWorkCol);
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

	for(INT i=WT_LA_CTR_DIE-lHeight; i<=(WT_LA_CTR_DIE+lHeight); i++)
	{
		for(INT j=WT_LA_CTR_DIE-lWidth; j<=(lWidth+WT_LA_CTR_DIE); j++)
		{
			stWafDieState.m_sDieState[i][j] = 1;
		}
	}

	//v4.47 Klocwork
	LookAroundByMultiSearch(stWafDieState);	//==FALSE )
	//{
	//	szMsg = "PCM fail Look around multi search can not find die in center area";
	//	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
	//	if( bCtrBlock )
	//		return FALSE;
	//	return TRUE;
	//}

	UCHAR ucNullGrade	= m_WaferMapWrapper.GetNullBin();
	UCHAR ucBinOffset	= m_WaferMapWrapper.GetGradeOffset();
	LONG lMapMaxDieCount = 0, lMapHasDieCount = 0, lHasMatchCount = 0, lEmptyHasCount = 0;
	for(INT i=WT_LA_CTR_DIE-lHeight; i<=(WT_LA_CTR_DIE+lHeight); i++)
	{
		for(INT j=WT_LA_CTR_DIE-lWidth; j<=(lWidth+WT_LA_CTR_DIE); j++)
		{
			LONG lCRow = lWorkRow + (i-WT_LA_CTR_DIE);
			LONG lCCol = lWorkCol + (j-WT_LA_CTR_DIE);

			lMapMaxDieCount++;
			SHORT sReturn = CheckDieStatus(lCRow, lCCol);
			stMapDieState.m_sDieState[i][j] = sReturn;
			if( sReturn==2 )
			{
				lMapHasDieCount++;
			}
		}
	}

	for(INT i=WT_LA_CTR_DIE-lHeight; i<=(WT_LA_CTR_DIE+lHeight); i++)
	{
		szMsg.Format("%d Map:", i);
		for(INT j=WT_LA_CTR_DIE-lWidth; j<=(lWidth+WT_LA_CTR_DIE); j++)
		{
			szTemp.Format(" %d", stMapDieState.m_sDieState[i][j]);
			szMsg += szTemp;
		}
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
	}

	for(INT i=0; i<WT_LA_MAX_DIE; i++)
	{
		szMsg.Format("%d Waf:", i);
		for(INT j=0; j<WT_LA_MAX_DIE; j++)
		{
			szTemp.Format(" %d", stWafDieState.m_sDieState[i][j]);
			szMsg += szTemp;
		}
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
	}

	// check
	LONG lPickedMatch[3][3], lRemainMatch[3][3];
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			lPickedMatch[i][j] = 0;
			lRemainMatch[i][j] = 0;
		}
	}

	for(INT i= (WT_LA_CTR_DIE-lHeight); i<=(WT_LA_CTR_DIE+lHeight); i++)
	{
		for(INT j=(WT_LA_CTR_DIE-lWidth); j<=(WT_LA_CTR_DIE+lWidth); j++)
		{
			SHORT sMapDieState = stMapDieState.m_sDieState[i][j];
			if( sMapDieState!=1 && sMapDieState!=2 )
			{
				continue;
			}

			for(INT m=0; m<=2; m++)	// for row compare
			{
				INT k = i + m-1;
				if( k<0 )
				{
					continue;
				}
				if( k>=WT_LA_MAX_DIE )
				{
					break;
				}

				for(INT n=0; n<=2; n++)	// for column compare
				{
					INT w = j + n-1;
					if( w<0 )
						continue;
					if( w>=WT_LA_MAX_DIE )
						break;

					SHORT sWafDieState = stWafDieState.m_sDieState[k][w];
					if( sWafDieState==0 )	// no check, skipped
					{
						continue;
					}

					if( sMapDieState==1 && sWafDieState==1 )	// no die, but has before
					{
						lPickedMatch[m][n]++;
					}

					if( sMapDieState==2 && sWafDieState==2 )	// has die,
					{
						lRemainMatch[m][n]++;
					}
				}
			}
		}
	}

	INT nKRow = 1, nKCol = 1;
	LONG lRemainLarge = lRemainMatch[1][1];
	LONG lPickedLarge = lPickedMatch[1][1];
	for(INT m=0; m<=2; m++)
	{
		szMsg = "(remain,picked):";
		for(INT n=0; n<=2; n++)
		{
			if( lRemainMatch[m][n]>lRemainLarge )
			{
				lRemainLarge = lRemainMatch[m][n];
				lPickedLarge = lPickedMatch[m][n];
				nKRow = m;
				nKCol = n;
			}
			else if( lRemainMatch[m][n]==lRemainLarge )
			{
				if( lPickedMatch[m][n]>lPickedLarge )
				{
					lPickedLarge = lPickedMatch[m][n];
					nKRow = m;
					nKCol = n;
				}
			}
			szTemp.Format(" (%2d,%2d)", lRemainMatch[m][n], lPickedMatch[m][n]);
			szMsg += szTemp;
		}
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
	}

	if( bCtrBlock )
	{
		if( iMatchTry==1 )
		{
			m_lAdvMostMatchRow = nKRow;
			m_lAdvMostMatchRow = nKCol;
		}
		else
		{
			nKRow = 1;
			nKCol = 1;
		}
	}

	lGetX = lGrabX + stWafDieState.m_sDiePosnX[WT_LA_CTR_DIE-1+nKRow][WT_LA_CTR_DIE-1+nKCol];
	lGetY = lGrabY + stWafDieState.m_sDiePosnY[WT_LA_CTR_DIE-1+nKRow][WT_LA_CTR_DIE-1+nKCol];
	szMsg.Format("match at offset %d,%d of %d,%d; die posn %d,%d; most match %d,%d", 
		nKRow, nKCol, WT_LA_CTR_DIE, WT_LA_CTR_DIE, lGetX, lGetY, lRemainLarge, lPickedLarge);
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

	LONG lOffsetX = 0, lOffsetY = 0;

	for(INT i= (WT_LA_CTR_DIE-lHeight); i<=(WT_LA_CTR_DIE+lHeight); i++)
	{
		for(INT j=(WT_LA_CTR_DIE-lWidth); j<=(WT_LA_CTR_DIE+lWidth); j++)
		{
			SHORT sMapDieState = stMapDieState.m_sDieState[i][j];
			if( sMapDieState!=1 && sMapDieState!=2 )
			{
				continue;
			}

			INT m = i+nKRow-1;
			INT n = j+nKCol-1;
			if( m<0 || n<0 )
				continue;
			if( m>=WT_LA_MAX_DIE || n>=WT_LA_MAX_DIE )
				continue;

			SHORT sWafDieState = stWafDieState.m_sDieState[m][n];
			if( sWafDieState!=2 )	// invalid or empty, skip
				continue;

			if( sMapDieState==1 )
			{
				lEmptyHasCount++;
				continue;
			}

			LONG lScanX, lScanY;
			LONG lCurrX = lGrabX + stWafDieState.m_sDiePosnX[m][n];
			LONG lCurrY = lGrabY + stWafDieState.m_sDiePosnY[m][n];
			LONG lCRow = lWorkRow + i - WT_LA_CTR_DIE;
			LONG lCCol = lWorkCol + j - WT_LA_CTR_DIE;

			if( GetPrescanWftPosn(lCRow, lCCol, lScanX, lScanY) )
			{
				lOffsetX += (lCurrX - lScanX);
				lOffsetY += (lCurrY - lScanY);
				lHasMatchCount++;
			}
		}
	}

	szMsg.Format("PR %d, Map %d(max %d), MapEmptyWaferHave %d; offset total (%d,%d);",
		lHasMatchCount, lMapHasDieCount, lMapMaxDieCount, lEmptyHasCount, lOffsetX, lOffsetY);
	szTemp = "";
	if( bCtrBlock && lHasMatchCount>=2 )
	{
		m_bGetAvgFOVOffset	= TRUE;
		m_lAvgFovOffsetX	= lOffsetX/lHasMatchCount;
		m_lAvgFovOffsetY	= lOffsetY/lHasMatchCount;
		pRgnPdc->SetVerifyOffset(m_lAvgFovOffsetX, m_lAvgFovOffsetY);
		szTemp.Format(" avg %d,%d", m_lAvgFovOffsetX, m_lAvgFovOffsetY);
	}
	szMsg = szMsg + szTemp + "\n";
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

	// xu_semitek should compare has die number and match die number, 
	//	if over a limit, then think it is fail.
	INT unPassScore = m_lMC[WTMC_MWMV_PassScore];

	if( lHasMatchCount < (lMapHasDieCount * unPassScore /100)  )
	{
		szMsg.Format("PCM fail because low than score %d", unPassScore);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return FALSE;
	}

	INT nHasBaseLimit = m_lMC[WTMC_MWMV_DieLimit];
	if( lEmptyHasCount>nHasBaseLimit )
	{
		szMsg.Format("PCM fail because Map have no die but on wafer %d(%d)", lEmptyHasCount, nHasBaseLimit);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return FALSE;
	}

	if( bCtrBlock==FALSE )
	{
		if( nKRow!=1 || nKCol!=1 )
		{
			szMsg.Format("PCM fail Edge check center not at 1,1");
			CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
			return FALSE;
		}
		
	}

	return TRUE;
}

// update all phy positions in the image for corresponding die(row/col)
BOOL CWaferTable::LookAroundByMultiSearch(WT_LA_DIE_RESULT	&stFovWafDieState)
{
	LONG lCtrX, lCtrY, lCtrT;
	GetEncoder(&lCtrX, &lCtrY, &lCtrT);

	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lCtrX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lCtrY;

	IPC_CServiceMessage stMsg;
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

//	WftCheckCurrentGoodDie();

	CString szMsg;
	DOUBLE dDieAngle = 0.0;
	LONG lOffsetX = 0, lOffsetY = 0;
	LONG lNearest = -1, lNearIdx = -1;
	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();
	LONG lDiePitchY_Y = GetDiePitchY_Y();

	for(ULONG ulIdx=1; ulIdx<=GetGoodTotalDie(); ulIdx++)
	{
		if( GetGoodPosnOnly(ulIdx, lOffsetX, lOffsetY, dDieAngle) )
		{
			if( labs(lOffsetX)>lDiePitchX_X*3/4 || labs(lOffsetY)>lDiePitchY_Y*3/4 )
			{
				continue;
			}

			LONG lDist = (LONG) GetDistance(lOffsetX, lOffsetY, 0, 0);
			if( lNearest ==- 1 )
			{
				lNearest = lDist;
				lNearIdx = ulIdx;
			}
			if( lNearest>lDist )
			{
				lNearIdx = ulIdx;
				lNearest = lDist;
			}
		}
	}

	LONG lNearOffsetX = 0, lNearOffsetY = 0;
	if( lNearIdx!=-1 )
	{
		GetGoodPosnOnly(lNearIdx, lNearOffsetX, lNearOffsetY, dDieAngle);
	}
	else
	{
		szMsg = "no center around, use grab position as center";
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
	//	return FALSE;
	}

	stFovWafDieState.m_sDiePosnX[WT_LA_CTR_DIE][WT_LA_CTR_DIE] = lNearOffsetX;
	stFovWafDieState.m_sDiePosnY[WT_LA_CTR_DIE][WT_LA_CTR_DIE] = lNearOffsetY;
	stFovWafDieState.m_sDieState[WT_LA_CTR_DIE][WT_LA_CTR_DIE] = 2;

	// xu_semitek
	for(INT m=1; m<=WT_LA_CTR_DIE; m++)
	{
		for(int i=(WT_LA_CTR_DIE-m); i<=(WT_LA_CTR_DIE+m); i++)
		{
			for(int j=(WT_LA_CTR_DIE-m); j<=(WT_LA_CTR_DIE+m); j++)
			{
				if( stFovWafDieState.m_sDieState[i][j]==2 )
				{
					continue;
				}

				DOUBLE dNear = -1;
				INT kk = WT_LA_CTR_DIE, ww = WT_LA_CTR_DIE;
				for(int k=(WT_LA_CTR_DIE-m); k<=(WT_LA_CTR_DIE+m); k++)
				{
					for(int w=(WT_LA_CTR_DIE-m); w<=(WT_LA_CTR_DIE+m); w++)
					{
						if( stFovWafDieState.m_sDieState[k][w]!=2 )
							continue;
						DOUBLE dDist = GetDistance(k, w, i, j);
						if( dNear==-1 )
						{
							dNear = dDist;
							kk = k;
							ww = w;
						}
						if( dNear>dDist )
						{
							dNear = dDist;
							kk = k;
							ww = w;
						}
					}
				}

				LONG lDiffRow = i-kk;
				LONG lDiffCol = j-ww;
				lNearOffsetX = stFovWafDieState.m_sDiePosnX[kk][ww];
				lNearOffsetY = stFovWafDieState.m_sDiePosnY[kk][ww];
				LONG lX = lNearOffsetX - lDiffCol * lDiePitchX_X - lDiffRow * lDiePitchY_X;
				LONG lY = lNearOffsetY - lDiffRow * lDiePitchY_Y - lDiffCol * lDiePitchX_Y;
				for(ULONG ulIdx=1; ulIdx<=GetGoodTotalDie(); ulIdx++)
				{
					if( ulIdx==lNearIdx )
					{
						continue;
					}

					if( GetGoodPosnOnly(ulIdx, lOffsetX, lOffsetY, dDieAngle) )
					{
						if( labs(lOffsetX-lX)<=lDiePitchX_X*2/4 &&
							labs(lOffsetY-lY)<=lDiePitchY_Y*2/4 )
						{
							stFovWafDieState.m_sDiePosnX[i][j] = lOffsetX;
							stFovWafDieState.m_sDiePosnY[i][j] = lOffsetY;
							stFovWafDieState.m_sDieState[i][j] = 2;
						//	szMsg.Format("%d,%d of %d,%d based on %d,%d of wft %d,%d", i, j, lOffsetX, lOffsetY, kk, ww, 
						//		lNearOffsetX, lNearOffsetY);
						//	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
							break;
						}
					}
				}
			}
		}
	}

	if( lNearIdx==-1 )
	{
		stFovWafDieState.m_sDieState[WT_LA_CTR_DIE][WT_LA_CTR_DIE] = 1;
	}

	return TRUE;
}

//	-1	no die at all, no scan result, original map has no die
//	 0	has die before, but picked or set to invalid now, or it is a reference die
//	 1	has die now, it has a grade but maybe not in the picking list, but not a reference die
//	 2
LONG CWaferTable::FindExistMapDieInBlock(ULONG ulHalfL, ULONG ulHalfR, ULONG ulHalfU, ULONG ulHalfD, ULONG &ulOutRow, ULONG &ulOutCol)
{
	ULONG ulURow = GetMapValidMinRow();
	ULONG ulLCol = GetMapValidMinCol();
	ULONG ulDRow = GetMapValidMaxRow();
	ULONG ulRCol = GetMapValidMaxCol();

	// not in valid map range, but if circle, not exactly good.
	if( !IsInMapValidRange(ulOutRow, ulOutCol) )
	{
		return -1;
	}

	if( ulOutRow>(ulURow+ulHalfU) )
		ulURow = ulOutRow - ulHalfU;
	if( ulOutCol>(ulLCol+ulHalfL) )
		ulLCol = ulOutCol - ulHalfL;
	if( (ulOutRow+ulHalfD)<ulDRow )
		ulDRow = ulOutRow + ulHalfD;
	if( (ulOutCol+ulHalfR)<ulRCol )
		ulRCol = ulOutCol + ulHalfR;

	LONG	lFindTgtDie = -1;

	LONG lTmpRow = ulOutRow;
	LONG lTmpCol = ulOutCol;

	LONG lCheckResult = CheckExistDieInBlock(lTmpRow, lTmpCol, ulURow, ulLCol, ulDRow, ulRCol);
	if( lCheckResult==1 )
	{
		lFindTgtDie = 1;
		ulOutRow = lTmpRow;
		ulOutCol = lTmpCol;
		return lFindTgtDie;
	}
	else if( lCheckResult==0 )
	{
		lFindTgtDie = 0;
	}

	ULONG ulLoop = max(ulDRow-ulURow, ulRCol-ulLCol)/2;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	for (ULONG ulCurrentLoop=1; ulCurrentLoop<=ulLoop; ulCurrentLoop++)
	{
		LONG lRow = (ulCurrentLoop*2 + 1);
		LONG lCol = (ulCurrentLoop*2 + 1);

		//Move to RIGHT & PR search on current die
		LONG lCurrentIndex = 1;

		//Move table to RIGHT
		LONG lDiff_X = 1;
		LONG lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		LONG lCheckResult = CheckExistDieInBlock(lTmpRow, lTmpCol, ulURow, ulLCol, ulDRow, ulRCol);
		if(	lCheckResult==1 )
		{
			lFindTgtDie = 1;
			break;
		}
		else if( lCheckResult==0 )
		{
			lFindTgtDie = 0;
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

			LONG lCheckResult = CheckExistDieInBlock(lTmpRow, lTmpCol, ulURow, ulLCol, ulDRow, ulRCol);
			if(	lCheckResult==1 )
			{
				lFindTgtDie = 1;
				break;
			}
			else if( lCheckResult==0 )
			{
				lFindTgtDie = 0;
			}
		}
		if( lFindTgtDie==1 )
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

			LONG lCheckResult = CheckExistDieInBlock(lTmpRow, lTmpCol, ulURow, ulLCol, ulDRow, ulRCol);
			if(	lCheckResult==1 )
			{
				lFindTgtDie = 1;
				break;
			}
			else if( lCheckResult==0 )
			{
				lFindTgtDie = 0;
			}
		}
		if( lFindTgtDie==1 )
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

			LONG lCheckResult = CheckExistDieInBlock(lTmpRow, lTmpCol, ulURow, ulLCol, ulDRow, ulRCol);
			if(	lCheckResult==1 )
			{
				lFindTgtDie = 1;
				break;
			}
			else if( lCheckResult==0 )
			{
				lFindTgtDie = 0;
			}
		}
		if( lFindTgtDie==1 )
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

			LONG lCheckResult = CheckExistDieInBlock(lTmpRow, lTmpCol, ulURow, ulLCol, ulDRow, ulRCol);
			if(	lCheckResult==1 )
			{
				lFindTgtDie = 1;
				break;
			}
			else if( lCheckResult==0 )
			{
				lFindTgtDie = 0;
			}
		}
		if( lFindTgtDie==1 )
		{
			break;
		}
	}

	if( lFindTgtDie==1 )
	{
		ulOutRow = lTmpRow;
		ulOutCol = lTmpCol;
	}

	return lFindTgtDie;
}


LONG CWaferTable::CheckExistDieInBlock(LONG lRow, LONG lCol, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol)
{
	if( lRow<0 || lCol<0 )
	{
		return -1;
	}

	if( m_WaferMapWrapper.IsReferenceDie(lRow, lCol) )
	{
		return -1;
	}

	LONG lPhyX = 0, lPhyY = 0;
	if(	GetPrescanWftPosn(lRow, lCol, lPhyX, lPhyY)==FALSE ||
		GetMapPhyPosn(lRow, lCol, lPhyX, lPhyY)==FALSE )
	{
		return -1;
	}

	ULONG ulDieState = GetMapDieState(lRow, lCol);
	if(	ulDieState==WT_MAP_DIESTATE_SKIP_PREDICTED )
	{
		return -1;
	}

	if( (lRow < (LONG)ulULRow) || (lRow > (LONG)ulLRRow) || (lCol < (LONG)ulULCol) || (lCol > (LONG)ulLRCol) )
	{
		return 0;
	}

	if( ulDieState==WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT ||
		ulDieState==WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT )
	{
		return 0;
	}

	if( IsScanBadCutGrade(lRow, lCol) )
	{
		return 0;
	}

	if( IsScannedDefectGrade(lRow, lCol) )
	{
		return 0;
	}

	if( CheckAlignMapWaferMatchHasDie(lRow, lCol) )
	{
		return	1;
	}

	return 0;
}

//	0 no die,	1 picked	2	remain	3 skipped	4 missing
SHORT CWaferTable::CheckDieStatus(LONG lRow, LONG lCol)
{
	// not in valid map range, but if circle, not exactly good.
	if( !IsInMapValidRange(lRow, lCol) )
	{
		return 0;
	}

	if( m_WaferMapWrapper.IsReferenceDie(lRow, lCol) )
	{
		return 0;
	}

	LONG lPhyX = 0, lPhyY = 0;
	if(	GetPrescanWftPosn(lRow, lCol, lPhyX, lPhyY)==FALSE ||
		GetMapPhyPosn(lRow, lCol, lPhyX, lPhyY)==FALSE )
	{
		return 0;
	}

	ULONG ulDieState = GetMapDieState(lRow, lCol);
	UCHAR ucNullGrade	= m_WaferMapWrapper.GetNullBin();
	UCHAR ucBinOffset	= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGrade		= m_WaferMapWrapper.GetGrade(lRow, lCol);

	if( ulDieState==WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY )
	{
		return 0;
	}

	if(	ulDieState==WT_MAP_DIESTATE_SKIP_PREDICTED )
	{
		return 3;
	}

	if( ulDieState>=WT_MAP_DIESTATE_MISSING )	//	WAF_CDieSelectionAlgorithm::MISSING )	// pick missing die
	{
		return 4;
	}

	if( ulDieState==WT_MAP_DIESTATE_UNPICK )
	{
		return 5;
	}

	if( ulDieState==WT_MAP_DIESTATE_LF_NOT_FULL_DIE )
	{
		return 6;
	}

	if( ulDieState==WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT ||
		ulDieState==WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT )
	{
		return 2;
	}

	if( IsScanBadCutGrade(lRow, lCol) )
	{
		return 2;
	}

	if( IsScannedDefectGrade(lRow, lCol) )
	{
		return 2;
	}

	if( ulDieState==WAF_CDieSelectionAlgorithm::INVALID )
	{
		return 0;
	}

	if( ucGrade==ucNullGrade )
	{
		return 1;
	}

	return 2;
}

BOOL CWaferTable::PopVerifyMatchCheckMapError(LONG lAsmRow, LONG lAsmCol)
{
	LONG lMapRow = 0, lMapCol = 0;
	if( ConvertAsmToOrgUser(lAsmRow, lAsmCol, lMapRow, lMapCol)==FALSE )
	{
		return FALSE;
	}

	for(int i=0; i<10; i++)
	{
		if( m_laPCIVerifyFailRow[i]==lMapRow && m_laPCIVerifyFailCol[i]==lMapCol )
		{
			return TRUE;
		}
	}

	CString szMsg;
	szMsg = "PCI Pitch error, verification fail, please check on map.";
	HmiMessage_Red_Back(szMsg, "Auto Sort");
	CString szLog;
	szLog.Format("%d,%d", lMapRow, lMapCol);
	HmiMessage_Red_Back(szLog, "Auto Sort");
	szMsg += szLog;
	CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
	m_bVerifyMapWaferFail	= TRUE;
	m_lVerifyMapWaferRow	= lMapRow;
	m_lVerifyMapWaferCol	= lMapCol;

	BOOL bFindSpace = FALSE;
	for(int i=0; i<10; i++)
	{
		if( m_laPCIVerifyFailRow[i]==-10000 && m_laPCIVerifyFailCol[i]==-10000 )
		{
			m_laPCIVerifyFailRow[i] = lMapRow;
			m_laPCIVerifyFailCol[i] = lMapCol;
			bFindSpace = TRUE;
			break;
		}
	}

	if( bFindSpace==FALSE )
	{
		for(int i=0; i<9; i++)
		{
			m_laPCIVerifyFailRow[i] = m_laPCIVerifyFailRow[i+1];
			m_laPCIVerifyFailCol[i] = m_laPCIVerifyFailCol[i+1];
		}
		m_laPCIVerifyFailRow[9] = lMapRow;
		m_laPCIVerifyFailCol[9] = lMapCol;
	}

	GrabAndSaveImage(0, 1, WPR_GSI_ADV_VERIFY_FAIL);

	return FALSE;
}	// pop out warning message.


BOOL CWaferTable::IsAutoSampleSetupOK()
{
	return m_ulAdvStage1StartCount>0 && m_ulAdvStage1SampleSpan>0;
}
