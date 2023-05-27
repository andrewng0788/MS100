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
#include "PrZoomSensor.h"
#include "BondHead.h"
#define	WPR_ONE_SEARCH_MAX_DIE		1000
#ifdef NU_MOTION_MS60
	#define	RESCAN_MAX_PITCH_TOL		30
	#define	RESCAN_LOOP_FIND_LOST_TOL	35	//	rescan	XXX
#else
	#define	RESCAN_MAX_PITCH_TOL		45
	#define	RESCAN_LOOP_FIND_LOST_TOL	30
#endif
	#define	RESCAN_SAME_DIE_TOL			20	//	rescan	XXX

BOOL CWaferPr::AP_SetGoodDie(LONG nDieX, LONG nDieY, bool bIsBadCut, bool bIsDefect, DOUBLE dDieRot, CString szDieBin)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanAreaPickMode() )
	{
		BOOL bToGoodList = TRUE;
		if( m_bPrescanBadCutAction && bIsBadCut )
		{
			if( m_bPrescanBadCutToNullBin || GetScanBadcutGrade()!=m_ucDummyPrescanPNPGrade )
			{
				bToGoodList = FALSE;
			}
		}
		if( m_bPrescanDefectAction && bIsDefect )
		{
			if( m_bPrescanDefectToNullBin || GetScanDefectGrade()!=m_ucDummyPrescanPNPGrade )
			{
				bToGoodList = FALSE;
			}
		}
		if( DieIsOutAngle(dDieRot) )
		{
			bToGoodList = FALSE;
		}
		if( bToGoodList )
		{
		//	if( DieNeedRotate(dDieRot)==FALSE )	//	m_dMinDieAngle	//	m_dMaxDieAngle
		//		dDieRot  = 0;
			SetGoodPosnOnly(nDieX, nDieY, dDieRot, szDieBin);
			CString szLogText;
			szLogText.Format("AP scan %d,%d(%.2f)", nDieX, nDieY, dDieRot);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
		}
	}

	return TRUE;
}

void CWaferPr::GetSubWindow(LONG lIndex, PR_WORD &lULX, PR_WORD &lULY, PR_WORD &lLRX, PR_WORD &lLRY)
{
	ULONG ulIndex = lIndex;
	if( ulIndex>=100 )
		ulIndex = 99;
	if( ulIndex<1 )
		ulIndex = 1;

	lULX = m_stSubImageWindow[ulIndex].coCorner1.x;
	lULY = m_stSubImageWindow[ulIndex].coCorner1.y;
	lLRX = m_stSubImageWindow[ulIndex].coCorner2.x;
	lLRY = m_stSubImageWindow[ulIndex].coCorner2.y;
}

VOID CWaferPr::CalculatePrescanFov(BOOL bLogIt)
{
// prescan relative code
#define	PRESCAN_PR_EDGE_MARGIN		20
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szMsg;
	PR_COORD	stPrRegion, stMargin;
	PR_RCOORD	stTmpPixel;
	int			siXPos=0, siYPos=0;		//Klocwork	//v4.37T6

	UINT unValue = pApp->GetProfileInt(gszPROFILE_SETTING, REG_PRESCAN_WAFER_OVERLAP, 1);
	if (unValue <= 0)
	{
		unValue = 1;
	}
	if (unValue > 3)
	{
		unValue = 1;
	}
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_PRESCAN_WAFER_OVERLAP, unValue);
	UINT unOverLapX = unValue*100 + m_lExtraPrescanOverlapX;
	UINT unOverLapY = unValue*100 + m_lExtraPrescanOverlapY;

	stPrRegion.x = GetScanPRWinLRX() - GetScanPRWinULX();
	stPrRegion.y = GetScanPRWinLRY() - GetScanPRWinULY();
	if( !IsStitchMode() )
	{
		stPrRegion.x = stPrRegion.x - PRESCAN_PR_EDGE_MARGIN;
		stPrRegion.y = stPrRegion.y - PRESCAN_PR_EDGE_MARGIN;
	}

	m_dPrescanLFSizeX	= m_dLFSizeX;
	m_dPrescanLFSizeY	= m_dLFSizeY;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	int nPitchPixelYY = GetScanPitchPixelYY();
	int nPitchPixelXX = GetScanPitchPixelXX();
	ULONG ulDiePitchYY = GetDiePitchY_Y();
	if( pUtl->GetPrescanBarWafer() && m_nDieSizeX!=0 )
	{
		ulDiePitchYY = labs(GetDiePitchX_X()*m_nDieSizeY/m_nDieSizeX);
		nPitchPixelYY = labs(GetScanPitchPixelXX()*m_nDieSizeY/m_nDieSizeX);
	}

	if( nPitchPixelXX==0 || nPitchPixelYY==0 || ulDiePitchYY==0 || GetDiePitchX_X()==0 )
	{
		return ;
	}

	double dCalibX = GetScanCalibX();
	double dCalibY = GetScanCalibY();
	double dCalibXY= GetScanCalibXY();
	double dCalibYX= GetScanCalibYX();
	if( IsEnableZoom() )
	{
		stTmpPixel.x = stPrRegion.x;
		stTmpPixel.y = stPrRegion.y;
		RConvertPixelToMotorStep(stTmpPixel, &siXPos, &siYPos, dCalibX, dCalibY, dCalibXY, dCalibYX);
		if( GetDiePitchX_X()!=0.0 && ulDiePitchYY!=0.0 )
		{
			m_dPrescanLFSizeX	= (fabs((double)siXPos) / fabs((double)GetDiePitchX_X()));
			m_dPrescanLFSizeY	= (fabs((double)siYPos) / fabs((double)ulDiePitchYY));
		}
	}

	if( pUtl->GetPrescanBarWafer() && ulDiePitchYY!=0 )
	{
		m_dPrescanLFSizeY	= (fabs((double)siYPos) / fabs((double)ulDiePitchYY));
	}

	m_uwFrameSubImageNum = 1;
	m_stSubImageWindow[1].coCorner1.x = GetScanPRWinULX();
	m_stSubImageWindow[1].coCorner1.y = GetScanPRWinULY();
	m_stSubImageWindow[1].coCorner2.x = GetScanPRWinLRX();
	m_stSubImageWindow[1].coCorner2.y = GetScanPRWinLRY();

	ULONG ulLFSizeRow = (ULONG)m_dPrescanLFSizeY;
	ULONG ulLFSizeCol = (ULONG)m_dPrescanLFSizeX;
	ULONG ulTotalDie = ulLFSizeRow*ulLFSizeCol;
	ULONG MAX_DIE_IN_SRCH_WINDOW = 1000;	//	100	
	if( IsEnableZoom() )
	{
		MAX_DIE_IN_SRCH_WINDOW = 1000;
		if(	m_ucScanWalkTour!=WT_SCAN_WALK_LEFT_VERT && 
			m_ucScanWalkTour!=WT_SCAN_WALK_TOP_HORI)
		{
			m_ucScanWalkTour = WT_SCAN_WALK_TOP_HORI;
		}	// to speed up
		if( IsPrescanMapIndex() )
		{
			m_ucScanWalkTour = WT_SCAN_WALK_HOME_HORI;
		}
	}

	if( ulTotalDie>=MAX_DIE_IN_SRCH_WINDOW )
	{
		ulLFSizeRow++;
		ulLFSizeCol++;
		PR_UWORD ulDivideCol = 1, ulDivideRow = 1;
		INT nOverlapX = 0, nOverlapY = 0;
		if( ulLFSizeRow>=ulLFSizeCol )
		{
			ulDivideRow = (PR_UWORD)_round(ulLFSizeRow * ulLFSizeCol / (MAX_DIE_IN_SRCH_WINDOW - ulLFSizeCol) + 1);
			nOverlapY = abs(GetScanPitchPixelYY() + GetScanNmlSizePixelY())*3/4;
		}
		else
		{
			ulDivideCol = (PR_UWORD)_round(ulLFSizeRow * ulLFSizeCol / (MAX_DIE_IN_SRCH_WINDOW-ulLFSizeRow) + 1);
			nOverlapX = abs(GetScanPitchPixelXX() + GetScanNmlSizePixelX())*3/4;
		}
		PR_WORD wSubWindowX = (GetScanPRWinLRX() - GetScanPRWinULX())/ulDivideCol;
		PR_WORD wSubWindowY = (GetScanPRWinLRY() - GetScanPRWinULY())/ulDivideRow;
		m_uwFrameSubImageNum = ulDivideRow*ulDivideCol;

		CString szMsg;
		if( pUtl->GetPrescanDebug() )
		{
			szMsg.Format("WPR: Frame Shrink %.2f sub image total num is %d, overlap x %d, Y %d, FOV %d,%d", 
				m_dWprZoomRoiShrink/100.0, m_uwFrameSubImageNum, nOverlapX, nOverlapY, ulLFSizeRow, ulLFSizeCol);
			SaveScanTimeEvent(szMsg);
		}

		for(PR_UWORD ulRow = 1; ulRow<=ulDivideRow; ulRow++)
		{
			for(PR_UWORD ulCol = 1; ulCol<=ulDivideCol; ulCol++)
			{
				PR_WORD lULX = GetScanPRWinULX() + wSubWindowX*(ulCol-1);
				PR_WORD lULY = GetScanPRWinULY() + wSubWindowY*(ulRow-1);
				PR_WORD lLRX = lULX + wSubWindowX + nOverlapX;
				PR_WORD lLRY = lULY + wSubWindowY + nOverlapY;
				if( lLRX>= GetScanPRWinLRX() )
					lLRX = GetScanPRWinLRX();
				if( lLRY>= GetScanPRWinLRY() )
					lLRY = GetScanPRWinLRY();
				ULONG ulIndex = (ulRow-1)*ulDivideCol + ulCol;
				m_stSubImageWindow[ulIndex].coCorner1.x = lULX;
				m_stSubImageWindow[ulIndex].coCorner1.y = lULY;
				m_stSubImageWindow[ulIndex].coCorner2.x = lLRX;
				m_stSubImageWindow[ulIndex].coCorner2.y = lLRY;
				if( pUtl->GetPrescanDebug() )
				{
					szMsg.Format("WPR: Sub image %d (%d,%d) -- (%d,%d)", ulIndex, lULX, lULY, lLRX, lLRY);
					SaveScanTimeEvent(szMsg);
				}
			}
		}
	}

	if( IsEnableZoom() && m_uwFrameSubImageNum<3 && (GetScnZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF || IsDP()) )
	{
		m_uwFrameSubImageNum = 3;
		CString szMsg;
		if( pUtl->GetPrescanDebug() )
		{
			szMsg.Format("WPR: 5M Cam MS at least 3 sub window FOV %d,%d", ulLFSizeRow, ulLFSizeCol);
			SaveScanTimeEvent(szMsg);
		}
		if( ulLFSizeRow>=ulLFSizeCol )
		{
			INT nOverlapY = abs(GetScanPitchPixelYY() + GetScanNmlSizePixelY())*3/4;
			PR_WORD wSubWindowY = (GetScanPRWinLRY() - GetScanPRWinULY())/3;
			for(PR_UWORD ulIndex = 1; ulIndex<=3; ulIndex++)
			{
				PR_WORD lULX = GetScanPRWinULX();
				PR_WORD lLRX = GetScanPRWinLRX();
				PR_WORD lULY = GetScanPRWinULY() + wSubWindowY*(ulIndex-1);
				PR_WORD lLRY = lULY + wSubWindowY + nOverlapY;
				if( lLRY>= GetScanPRWinLRY() )
					lLRY = GetScanPRWinLRY();
				m_stSubImageWindow[ulIndex].coCorner1.x = lULX;
				m_stSubImageWindow[ulIndex].coCorner1.y = lULY;
				m_stSubImageWindow[ulIndex].coCorner2.x = lLRX;
				m_stSubImageWindow[ulIndex].coCorner2.y = lLRY;
				if( pUtl->GetPrescanDebug() )
				{
					szMsg.Format("WPR: Sub image %d (%d,%d) -- (%d,%d)", ulIndex, lULX, lULY, lLRX, lLRY);
					SaveScanTimeEvent(szMsg);
				}
			}
		}
		else
		{
			INT nOverlapX = abs(GetScanPitchPixelXX() + GetScanNmlSizePixelX())*3/4;
			PR_WORD wSubWindowX = (GetScanPRWinLRX() - GetScanPRWinULX())/3;

			for(PR_UWORD ulIndex = 1; ulIndex<=3; ulIndex++)
			{
				PR_WORD lULX = GetScanPRWinULX() + wSubWindowX*(ulIndex-1);
				PR_WORD lLRX = lULX + wSubWindowX + nOverlapX;
				PR_WORD lULY = GetScanPRWinULY();
				PR_WORD lLRY = GetScanPRWinLRY();
				if( lLRX>= GetScanPRWinLRX() )
					lLRX = GetScanPRWinLRX();
				m_stSubImageWindow[ulIndex].coCorner1.x = lULX;
				m_stSubImageWindow[ulIndex].coCorner1.y = lULY;
				m_stSubImageWindow[ulIndex].coCorner2.x = lLRX;
				m_stSubImageWindow[ulIndex].coCorner2.y = lLRY;
				if( pUtl->GetPrescanDebug() )
				{
					szMsg.Format("WPR: Sub image %d (%d,%d) -- (%d,%d)", ulIndex, lULX, lULY, lLRX, lLRY);
					SaveScanTimeEvent(szMsg);
				}
			}
		}
	}

	if( IsStitchMode() )
	{
		stMargin.x = (PR_WORD)m_lExtraPrescanOverlapX;
		stMargin.y = (PR_WORD)m_lExtraPrescanOverlapY;
	}
	else
	{
		stMargin.x = unOverLapX * abs(nPitchPixelXX + GetScanNmlSizePixelX())/2/100;
		stMargin.y = unOverLapY * abs(nPitchPixelYY + GetScanNmlSizePixelY())/2/100;
	}

	stTmpPixel.x = (PR_REAL)(stPrRegion.x - stMargin.x);
	stTmpPixel.y = (PR_REAL)(stPrRegion.y - stMargin.y);

	if( IsAOIOnlyMachine() )
	{
		if( stTmpPixel.x<=0 )
		{
			stTmpPixel.x = (PR_REAL)GetScanPitchPixelXX();
			m_dPrescanLFSizeX = 1.0;
		}
		if( stTmpPixel.y<=0 )
		{
			stTmpPixel.y = (PR_REAL)nPitchPixelYY;
			m_dPrescanLFSizeY = 1.0;
		}
	}
	else
	{
		if( stTmpPixel.x<=0 )
		{
			stTmpPixel.x = GetScanNmlSizePixelX();
		}
		if( stTmpPixel.y<=0 )
		{
			stTmpPixel.y = GetScanNmlSizePixelY();
		}
	}

	//v4.28T6	//Klocwork
	RConvertPixelToMotorStep(stTmpPixel, &siXPos, &siYPos, dCalibX, dCalibY, dCalibXY, dCalibYX);
	m_lPrescanMovePitchX	= siXPos;
	m_lPrescanMovePitchY	= siYPos;

	if( IsStitchMode() )
	{
		PR_COORD	stNmlMargin, stNmlPixel;
		stNmlMargin.x = abs(nPitchPixelXX + GetScanNmlSizePixelX())/2;
		stNmlMargin.y = abs(nPitchPixelYY + GetScanNmlSizePixelY())/2;
		stNmlPixel.x = stPrRegion.x - stNmlMargin.x;
		stNmlPixel.y = stPrRegion.y - stNmlMargin.y;
		if( stNmlPixel.x<=0 )
		{
			stNmlPixel.x = nPitchPixelXX;
		}
		if( stNmlPixel.y<=0 )
		{
			stNmlPixel.y = nPitchPixelYY;
		}
		if (fabs((double)stTmpPixel.x) > fabs((double)stNmlPixel.x) && 
			fabs((double)stTmpPixel.y) > fabs((double)stNmlPixel.y) )
		{
			m_lImageStichExtraDelay = 0;	//	xxxxxx 44*labs(stTmpPixel.x)*labs(stTmpPixel.y)/labs(stNmlPixel.x)/labs(stNmlPixel.y)-44;
		}
		else
		{
			m_lImageStichExtraDelay = 0;
		}
	}

	if( (GetDieShape()!=WPR_RHOMBUS_DIE) && (GetDieSizeX()>GetDiePitchX_X() || GetDieSizeY()>GetDiePitchY_Y()) )
	{
		//HmiMessage_Red_Back("Die pitch less than die size, please check");
	}

	if( pApp->GetCustomerName()=="Semitek" )
	{
		if( GetDiePitchX_X()>(GetDieSizeX()*(100+75)/100) ||
			GetDiePitchY_Y()>(GetDieSizeY()*(100+75)/100) )
		{
			HmiMessage_Red_Back("Die pitch over die size too much, please check");
		}
	}
	//v4.28T6	//Klocwork
	if( pUtl->GetPrescanDebug() )
	{
		szMsg.Format("WPR: FOV(%f,%f); Pr(%d,%d); Margin(%d,%d); Move(%d,%d); overlap(%d,%d) calib(%f,%f)", 
			m_dPrescanLFSizeX, m_dPrescanLFSizeY, 
			stPrRegion.x, stPrRegion.y, 
			stMargin.x, stMargin.y, 
			GetPrescanPitchX(), GetPrescanPitchY(),
			unOverLapX, unOverLapY,
			dCalibX, dCalibY);
		SaveScanTimeEvent(szMsg);
	}
}


BOOL CWaferPr::PrescanAutoMultiGrabDone(CDWordArray &dwList, BOOL bDrawDie, BOOL bIsFindGT, BOOL b2ndPR)
{
	if( IsBurnIn() && (m_bNoWprBurnIn || IsAOIOnlyMachine()) )
	{
		return TRUE;
	}

	LONG lZoomDone = -1;
	if( IsEnableZoom() )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Switch to scan zoom");
		lZoomDone =WPR_ToggleZoom(TRUE, FALSE, 12);
	}

	BOOL bReturn = ScanAutoMultiSearch5M(dwList, bDrawDie, bIsFindGT);

	if( IsPrecanWith2Pr() && b2ndPR )
	{
		ScanAutoMultiSearch2ndPR(bDrawDie);
	}

	if( bDrawDie )
	{
		CloseWaitingAlert();
	}

	if( bDrawDie && IsEnableZoom() )
	{
		if( GetScnZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF || IsDP() )
		{
			AfxMessageBox("Please check the search result!\nPress ok to continue.", MB_SYSTEMMODAL);
		}
	}
	if( lZoomDone==1 )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Switch to sort zoom");
		WPR_ToggleZoom(FALSE, FALSE, 13);
	}
	return bReturn;
}

VOID CWaferPr::CalcScanDiePosition(CONST LONG lGrabX, CONST LONG lGrabY, PR_RCOORD stDieOffset, INT &siStepX, INT &siStepY)
{
	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	BOOL bFindNear = FALSE;

	CString szGridDCLog;
	if( bFindNear==FALSE )
	{
		if( stDieOffset.x<0 || stDieOffset.x>PR_MAX_COORD || 
			stDieOffset.y<0 || stDieOffset.y>PR_MAX_COORD )
		{
			siStepX = 0;
			siStepY = 0;
			if( m_lScanPrAbnormalCount==0 )
			{
				szGridDCLog.Format("PR abnormal 1st at (%f,%f), calibX(%f,%f) calibY(%f,%f), grab %d,%d", 
					stDieOffset.x, stDieOffset.y, GetScanCalibX(), GetScanCalibXY(), GetScanCalibY(), GetScanCalibYX(), 
					lGrabX, lGrabY);
				SaveScanTimeEvent(szGridDCLog);
			}
			else
			{
				szGridDCLog.Format("PR abnormal at (%f,%f), grab %d,%d", stDieOffset.x, stDieOffset.y, lGrabX, lGrabY);
			}
		//	pUtl->SetPrAbnormal(TRUE);	//	die return pixel is out of range.
			pUtl->PrescanMoveLog(szGridDCLog);
			m_lScanPrAbnormalCount++;
		}
		else
			RCalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	}

	CalculateNDieOffsetXY(siStepX, siStepY);
}

VOID CWaferPr::RCalculateDieCompenate(PR_RCOORD stDieOffset, int *siStepX, int *siStepY)
{
	PR_RCOORD	stRelMove;

	stRelMove.x = (PR_REAL)GetPrCenterX() - stDieOffset.x;
	stRelMove.y = (PR_REAL)GetPrCenterY() - stDieOffset.y;

	double dCalibX = GetScanCalibX();
	double dCalibY = GetScanCalibY();
	double dCalibXY= GetScanCalibXY();
	double dCalibYX= GetScanCalibYX();

	RConvertPixelToMotorStep(stRelMove, siStepX, siStepY, dCalibX, dCalibY, dCalibXY, dCalibYX);
}

VOID CWaferPr::RConvertPixelToMotorStep(PR_RCOORD stPixel, int *siStepX, int *siStepY, 
									   double dCalibX, double dCalibY, double dCalibXY, double dCalibYX)
{
	*siStepX = (int)((DOUBLE)stPixel.x * dCalibX + (DOUBLE)stPixel.y * dCalibXY);
	*siStepY = (int)((DOUBLE)stPixel.y * dCalibY + (DOUBLE)stPixel.x * dCalibYX);
}

VOID CWaferPr::SetupScanSearchCmd2(UCHAR lDieNo, BOOL bInspect)
{
	PR_UBYTE	ubSenderID	 =  GetScnSenID();
	PR_UBYTE	ubReceiverID =  GetScnRecID();

	PR_WORD	lPrSrchID	= GetDiePrID(lDieNo);
	m_uwScanRecordID[1] = lPrSrchID;

	LONG		lAlignAccuracy;
	switch(m_lGenSrchAlignRes[lDieNo])
	{
	case 1:		
		lAlignAccuracy	= PR_LOW_DIE_ALIGN_ACCURACY;	
		break;

	default:	
		lAlignAccuracy	= PR_HIGH_DIE_ALIGN_ACCURACY;	
		break;
	}

	LONG		lGreyLevelDefect;
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

	PR_BOOLEAN	bCheckChip = PR_TRUE;
	if (m_dGenSrchChipArea[lDieNo] == 0.0)
	{
		bCheckChip = PR_FALSE;
	}
	if (m_bGenSrchEnableChipCheck[lDieNo] == FALSE)
	{
		bCheckChip = PR_FALSE;
	}

	PR_BOOLEAN	bCheckDefect = PR_TRUE;
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

	//Calculate chip die area, min & total defect area
	DOUBLE dMinChipArea			= ((DOUBLE)(GetScanDieSizePixelX(lDieNo) * GetScanDieSizePixelY(lDieNo)) / 100) * m_dGenSrchChipArea[lDieNo];
	DOUBLE dSingleDefectArea	= ((DOUBLE)(GetScanDieSizePixelX(lDieNo) * GetScanDieSizePixelY(lDieNo)) / 100) * m_dGenSrchSingleDefectArea[lDieNo];  
	DOUBLE dTotalDefectArea		= ((DOUBLE)(GetScanDieSizePixelX(lDieNo) * GetScanDieSizePixelY(lDieNo)) / 100) * m_dGenSrchTotalDefectArea[lDieNo]; 


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
	st2PointsAlignCmd.szPosConsistency.x		= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.szPosConsistency.y		= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.rDieRotTol				= GetScanRotTol(TRUE);

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
	stStreetAlignCmd.szPosConsistency.x			= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	stStreetAlignCmd.szPosConsistency.y			= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	stStreetAlignCmd.rDieRotTol					= GetScanRotTol(TRUE);

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
	stTmplInspCmd.ubDefectThreshold				= (PR_UBYTE)m_lGenSrchDefectThres[lDieNo]; 
	stTmplInspCmd.rMinInkSize					= PR_FALSE;     
	stTmplInspCmd.emDefectAttribute				= (PR_DEFECT_ATTRIBUTE)(lGreyLevelDefect);

	PR_DIE_INSP_CMD 				stDieInspCmd;
	PR_InitDieInspCmd(&stDieInspCmd);
	stDieInspCmd.stTmpl							= stTmplInspCmd;

	PR_DIE_ALIGN_PAR                stAlignPar;
	PR_InitDieAlignPar(&stAlignPar);
	stAlignPar.emEnableBackupAlign				= PR_FALSE;	//(PR_BOOLEAN)m_bSrchEnableBackupAlign;
	stAlignPar.rStartAngle						= GetScanStartAngle(TRUE);
	stAlignPar.rEndAngle						= GetScanEndAngle(TRUE);		

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
		PR_UBYTE lCorners = WPR_BADCUT_POINT;
		if( GetDieShape() == WPR_RHOMBUS_DIE )
		{
			lCorners = WPR_RHOMBUS_CORNERS;
		}	//	rhombus die
		else if( GetDieShape() == WPR_TRIANGULAR_DIE )
		{
			lCorners = WPR_TRIANGLE_CORNERS;
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

	//Klocwork	//v4.02T5
	stBadCutInspCmd.stEpoxy.stIndivExt1.ubNumOfExcessLine	= 0;
	stBadCutInspCmd.stEpoxy.stIndivExt1.uwMaxExcessSide		= 0;

	PR_SRCH_DIE_CMD stScanSrchCmd2;
	PR_InitSrchDieCmd(&stScanSrchCmd2);
	stScanSrchCmd2.emLatch			= PR_TRUE;
	stScanSrchCmd2.uwNRecordID		= 1;
	stScanSrchCmd2.auwRecordID[0]	= (PR_UWORD)(lPrSrchID);
	stScanSrchCmd2.emAlign			= PR_TRUE;
	stScanSrchCmd2.emDefectInsp		= (PR_BOOLEAN) bInspect;
	stScanSrchCmd2.emCameraNo		= GetScnCamID();
	stScanSrchCmd2.emVideoSource	= PR_IMAGE_BUFFER_A;
	stScanSrchCmd2.ulRpyControlCode	= PR_DEF_SRCH_DIE_RPY;

	stScanSrchCmd2.stDieAlign			= stDieAlignCmd;
	stScanSrchCmd2.stDieInsp			= stDieInspCmd;
	stScanSrchCmd2.stDieAlignPar		= stAlignPar;
	stScanSrchCmd2.stPostBondInsp		= stBadCutInspCmd;
#ifdef VS_5MCAM
	stScanSrchCmd2.stDieAlign.stStreet.emPartialDieSrch	= PR_FALSE;	//	PR_TRUE;
	stScanSrchCmd2.stDieAlign.st2Points.emPartialDieSrch	= PR_FALSE;	//	PR_TRUE;
#endif

	stScanSrchCmd2.stDieAlignPar.rStartAngle		= GetScanStartAngle(TRUE);	
	stScanSrchCmd2.stDieAlignPar.rEndAngle			= GetScanEndAngle(TRUE);

	LONG lPrescanSrchDieScore = m_lGenSrchDieScore[lDieNo];
	if( lPrescanSrchDieScore==0 )
	{
		lPrescanSrchDieScore = 80;
	}

	stScanSrchCmd2.stDieAlignPar.rMatchScore			= (PR_REAL)(lPrescanSrchDieScore);
	stScanSrchCmd2.stDieAlignPar.emIsDefaultMatchScore= PR_FALSE;

	stScanSrchCmd2.emGraphicInfo		= PR_NO_DISPLAY;
	stScanSrchCmd2.emLatchMode		= PR_LATCH_FROM_GALLERY;

	if ( (bInspect) && (m_bBadCutDetection == TRUE) && (m_lCurBadCutSizeX > 0) && (m_lCurBadCutSizeY > 0))
	{
		stScanSrchCmd2.emPostBondInsp = PR_TRUE;  
	}
	else
	{
		stScanSrchCmd2.emPostBondInsp = PR_FALSE;  
	}

	stScanSrchCmd2.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = GetScanPRWinULX();
	stScanSrchCmd2.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = GetScanPRWinULY();
	stScanSrchCmd2.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = GetScanPRWinLRX();
	stScanSrchCmd2.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = GetScanPRWinLRY();

	stScanSrchCmd2.emDieInspAlg		= GetLrnInspMethod(lDieNo);
	stScanSrchCmd2.emAlignAlg		= GetLrnAlignAlgo(lDieNo);

	m_stScanSrchCmd2 = stScanSrchCmd2;

	m_pPrescanPrCtrl->SetupSearchCommand2(m_stScanSrchCmd2);
}

PR_WORD	CWaferPr::GetScanPRWinULX()
{
	return m_stWprRoiEffView.coCorner1.x;
}

PR_WORD	CWaferPr::GetScanPRWinULY()
{
	return m_stWprRoiEffView.coCorner1.y;
}

PR_WORD	CWaferPr::GetScanPRWinLRX()
{
	return m_stWprRoiEffView.coCorner2.x;
}

PR_WORD	CWaferPr::GetScanPRWinLRY()
{
	return m_stWprRoiEffView.coCorner2.y;
}

PR_WIN CWaferPr::GetScanFovWindow()
{
	PR_WIN PRFovWin;
	PRFovWin.coCorner1.x = GetPRWinULX();
	PRFovWin.coCorner1.y = GetPRWinULY();
	PRFovWin.coCorner2.x = GetPRWinLRX();
	PRFovWin.coCorner2.y = GetPRWinLRY();

	m_bInitPreScanWaferCamFailed = FALSE;
#ifdef VS_5MCAM		//v4.49 Klocwork
	if( IsEnableZoom() && GetPrescanPrID()==3 && 
		(GetRunZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF || GetScnZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF) )
	{
		if( GetRunZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF )
		{
			PR_UWORD uwStatus	= PR_ERR_NOERR;
			PR_CAMERA_INFO_CMD	stCmd;
			PR_CAMERA_INFO_RPY	stRpy;

			PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
			PR_PURPOSE ubPpsG = GetRunPurposeG();

			PR_InitCameraInfoCmd(&stCmd);
			stCmd.emPurpose = (PR_PURPOSE)ubPpsG ;

			PR_CameraInfoCmd(&stCmd, ubSID, ubRID, &stRpy);

			CString szErrorMess;
			if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
			{
				uwStatus = stRpy.stStatus.uwCommunStatus;
				szErrorMess.Format("Failed to get effective view, Purpose = %d, PR Comm Error = %d", stCmd.emPurpose, uwStatus);
			}
			else if (stRpy.stStatus.uwPRStatus != PR_ERR_NOERR)
			{
				uwStatus = stRpy.stStatus.uwPRStatus;
				szErrorMess.Format("Failed to get effective view, Purpose = %d, PR Status Error = %d", stCmd.emPurpose, uwStatus);
			}

			if (uwStatus != PR_ERR_NOERR)
			{
				#ifdef OFFLINE
				DisplayMessage(szErrorMess);
				#else
				AfxMessageBox(szErrorMess);
				#endif
				m_bInitPreScanWaferCamFailed = TRUE;
			}

			PRFovWin = stRpy.stEffView;
		}
		else
		{
			PRFovWin.coCorner1.x = 0;
			PRFovWin.coCorner1.y = PR_MAX_WIN_ULC_Y;
			PRFovWin.coCorner2.x = PR_MAX_COORD;
			PRFovWin.coCorner2.y = PR_MAX_WIN_LRC_Y;
		}

		// if the scanCut is invaild or it is not Prober
		DOUBLE dShrinkRatio = 1.0;
		if( (m_ucFovShrinkMode == 0 || GetRunZoom() != CPrZoomSensorMode::PR_ZOOM_MODE_FF) && SubCutScanFOV(dShrinkRatio) )
		{
			PR_WORD CenterX = (PR_WORD)GetPrCenterX();
			PR_WORD CenterY = (PR_WORD)GetPrCenterY();

			PR_WORD wHalfWinX = (PR_WORD)(LONG)((PR_MAX_COORD + 1) * dShrinkRatio / 2.0);
			PR_WORD wHalfWinY = (PR_WORD)(LONG)(labs(PR_MAX_WIN_LRC_Y - PR_MAX_WIN_ULC_Y) * dShrinkRatio / 2.0);

			PRFovWin.coCorner1.x = CenterX - wHalfWinX;
			PRFovWin.coCorner1.y = CenterY - wHalfWinY;
			PRFovWin.coCorner2.x = CenterX + wHalfWinX;
			PRFovWin.coCorner2.y = CenterY + wHalfWinY;
		}

		CString szMsg;
		szMsg.Format("WPR prescan FOV ROI %d zoom %.f, UL %d,%d, LR %d,%d", m_ucFovShrinkMode, m_dWprZoomRoiShrink,
			PRFovWin.coCorner1.x, PRFovWin.coCorner1.y, PRFovWin.coCorner2.x, PRFovWin.coCorner2.y);
		SaveScanTimeEvent(szMsg);

		m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
		DrawRectangleBox(PRFovWin.coCorner1, PRFovWin.coCorner2, PR_COLOR_YELLOW);
	}
#endif

	m_stWprRoiEffView = PRFovWin;	//	change settings
	CalculateDieInView(TRUE);
	return PRFovWin;
}

BOOL CWaferPr::ScanAutoMultiSearch5M(CDWordArray &dwList, BOOL bDrawDie, BOOL bIsFindGT)
{
	LONG lX = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["X"];
	LONG lY = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["Y"];

	#define	FOV_5M_ROW_COL	100
	typedef	struct
	{
		BOOL	bFound;
		LONG	lDieX;
		LONG	lDieY;
		DOUBLE	dDieAngle;
		CString	szDieBin;
	}	ScanDieResult;
	ScanDieResult asDieList[FOV_5M_ROW_COL][FOV_5M_ROW_COL];

	PR_UBYTE	ubSenderID	 =  GetScnSenID();
	PR_UBYTE	ubReceiverID =  GetScnRecID();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	CMSLogFileUtility::Instance()->WL_LogStatus("5M multi search prepare");
	PR_GRAB_SHARE_IMAGE_CMD		stGrbCmd;
	PR_InitGrabShareImgCmd(&stGrbCmd);
	// AOI partial die step 3 During Prescan, 
	stGrbCmd.emPurpose			= GetScnPurpose();
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= m_stMultiSrchCmd.auwRecordID[0];
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_NORMAL;

	PR_GRAB_SHARE_IMAGE_RPY		stGrbRpy;
	PR_GrabShareImgCmd(&stGrbCmd, ubSenderID, ubReceiverID, &stGrbRpy);
	if (PR_COMM_NOERR != stGrbRpy.uwCommunStatus)
	{
		HmiMessage("One Search grab share image cmd fail");
		return FALSE;
	}

	PR_GRAB_SHARE_IMAGE_RPY2	stGrbRpy2;
	PR_GrabShareImgRpy(ubSenderID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		HmiMessage("One Search grab share image rpy fail");
		return FALSE;
	}

	int nSubImageNum = m_uwFrameSubImageNum;
	if( bIsFindGT && GetMapIndexStepRow()>10 && GetMapIndexStepCol()>10 )
	{
		nSubImageNum = 1;
	}
	FILE *fp = NULL;
	errno_t nErr = -1;
	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	CString szTime;
	szTime = ctDateTime.Format("%Y%m%d%H%M%S");
	if( pUtl->GetPrescanDebug() )
	{
		WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
		CString szLogFileName;
		szLogFileName.Format("%s\\History\\FovOneSearch_DC%d", gszUSER_DIRECTORY, pCPInfo->GetDCState());
		szLogFileName = szLogFileName + ".txt";
		nErr = fopen_s(&fp, szLogFileName, "w");
		if ((nErr == 0) && (fp != NULL))
		{
			fprintf(fp, "col,row,prx,pry,offx,offy,posnx,posny,via clb,%d\n", nSubImageNum);
		}
	}

	// now add the image to queue
	PR_ULWORD ulStationID	= stGrbRpy2.ulStationID;
	PR_ULWORD ulImageID		= stGrbRpy2.ulImageID;

	CMSLogFileUtility::Instance()->WL_LogStatus("5M multi search loop to find out all dice");
	for(int i = 1; i<=nSubImageNum; i++)
	{
		PR_UWORD uwRpy1Status, uwRpy1ComStt, uwRpy2ComStt;
		IMG_UWORD		uwCommStatus;
		PR_WORD lULX = 0, lULY = 0, lLRX = 0, lLRY = 0;
		GetSubWindow(i, lULX, lULY, lLRX, lLRY);
		if( bIsFindGT && GetMapIndexStepRow()>10 && GetMapIndexStepCol()>10 && GetDieSizeX()!=0 && GetDieSizeY()!=0 )
		{
			PR_WORD wScanPitchPixelX = (PR_WORD)_round(GetScanNmlSizePixelX() * GetDiePitchX_X() * 5 / GetDieSizeX());
			PR_WORD wScanPitchPixelY = (PR_WORD)_round(GetScanNmlSizePixelY() * GetDiePitchY_Y() * 5 / GetDieSizeY());

			lULX = PR_DEF_CENTRE_X - wScanPitchPixelX;
			lLRX = PR_DEF_CENTRE_X + wScanPitchPixelX;
			lULY = PR_DEF_CENTRE_Y - wScanPitchPixelY;
			lLRY = PR_DEF_CENTRE_Y + wScanPitchPixelY;
			if (lULX < GetScanPRWinULX())
			{
				lULX = GetScanPRWinULX();
			}
			if (lLRX > GetScanPRWinLRX())
			{
				lLRX = GetScanPRWinLRX();
			}
			if (lULY < GetScanPRWinULY())
			{
				lULY = GetScanPRWinULY();
			}
			if (lLRY > GetScanPRWinLRY())
			{
				lLRY = GetScanPRWinLRY();
			}

			if ((nErr == 0) && (fp != NULL))
			{
				fprintf(fp, "%d,%d,%d,%d,%d,%d\n", lULX, lULY, lLRX, lLRY, GetScanNmlSizePixelX(), GetScanNmlSizePixelY());
			}
		}
		PR_WORD lCTX = (lULX + lLRX)/2;
		PR_WORD lCTY = (lULY + lLRY)/2;

		if( bDrawDie )
		{
			PR_COLOR sColor = PR_COLOR_YELLOW;
			if( i%2==0 )
				sColor = PR_COLOR_CYAN;
			PR_COORD stCorner1;
			PR_COORD stCorner2;
			stCorner1.x = lULX;
			stCorner1.y = lULY;
			stCorner2.x = lLRX;
			stCorner2.y = lLRY;
			DrawRectangleBox(stCorner1, stCorner2, sColor);
		}

		PR_SRCH_DIE_CMD stInspCmd	= m_stMultiSrchCmd;

		stInspCmd.ulLatchImageID	= ulImageID;
		stInspCmd.uwLatchStationID	= (PR_UWORD) ulStationID;

		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = lULX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = lULY;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = lLRX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = lLRY;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.x		 = lCTX;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.y		 = lCTY;

		stInspCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
		stInspCmd.uwHostMallocMaxNoOfDieInRpy2			= WPR_ONE_SEARCH_MAX_DIE;
		if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy3		= WPR_ONE_SEARCH_MAX_DIE;
		if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy4		= WPR_ONE_SEARCH_MAX_DIE;
		PR_SrchDieCmd(&stInspCmd, ubSenderID, ubReceiverID, &uwCommStatus);

		if (uwCommStatus != PR_COMM_NOERR)
		{
			CString szMsg;
			szMsg.Format("One Search PR_MultiSrchDieCmd finished with status %u!", uwCommStatus);
			HmiMessage(szMsg);
			//v4.39T10	//Klocwork
			if ((nErr == 0) && (fp != NULL))
			{
				fprintf(fp, "\n\n");
				fclose(fp);
			}
			return FALSE;
		}

		PR_SRCH_DIE_RPY1 stInspRpy1;
		PR_SrchDieRpy1(ubSenderID, &stInspRpy1);
		uwRpy1ComStt = stInspRpy1.uwCommunStatus;
		uwRpy1Status = stInspRpy1.uwPRStatus;

		if (uwRpy1ComStt != PR_COMM_NOERR || PR_ERROR_STATUS(uwRpy1Status))
		{
			CString szMsg;
			szMsg.Format("One Search PR MultiSrchDieRpy1 finished with status %u, %u!\n",	uwRpy1ComStt, uwRpy1Status);
			HmiMessage(szMsg);
			//v4.39T10	//Klocwork
			if( fp!=NULL )
			{
				fprintf(fp, "\n\n");
				fclose(fp);
			}
			return FALSE;
		}

		PR_UWORD		usDieType;
		PR_SRCH_DIE_RPY2	stInspRpy2;

		stInspRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*)malloc (stInspCmd.uwHostMallocMaxNoOfDieInRpy2*sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign == NULL)
		{
			HmiMessage("One Search Init stDieAlign null pointer");
			break; 
		}
		PR_SrchDieRpy2(ubSenderID, &stInspRpy2);
		uwRpy2ComStt = stInspRpy2.stStatus.uwCommunStatus;
		if (uwRpy2ComStt != PR_COMM_NOERR)
		{
			if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
			{
				free(stInspRpy2.pstDieAlign);
			}
			CString szMsg;
			szMsg.Format("One Search PR MultiSrchDieRpy2 finished with status %u!\n", uwRpy2ComStt);
			HmiMessage(szMsg);
			break;
		}
		usDieType = stInspRpy2.stStatus.uwPRStatus;

		PR_SRCH_DIE_RPY3	stInspRpy3;
		PR_SRCH_DIE_RPY4	stInspRpy4;
		if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
		{
			stInspRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*)malloc( stInspCmd.uwHostMallocMaxNoOfDieInRpy3*sizeof(PR_DIE_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin==NULL )
			{
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				HmiMessage("One Search Init stDieAlign null pointer");
				break; 
			}
		}
		if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
		{
			stInspRpy4.pstPostBondInspMin = (PR_POST_INSP_MINIMAL_RPY*)malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy4*sizeof(PR_POST_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin==NULL )
			{
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				HmiMessage("One Search Init stDieAlign null pointer");
				break; 
			}
		}

		if( ((PR_TRUE==m_stMultiSrchCmd.emDefectInsp) || (PR_TRUE==m_stMultiSrchCmd.emPostBondInsp)) &&
			(PR_COMM_NOERR==uwRpy2ComStt) && !PR_ERROR_STATUS(usDieType) )
		{
			PR_SrchDieRpy3(ubSenderID,&stInspRpy3);
			PR_UWORD uwRpy3ComStt = stInspRpy3.stStatus.uwCommunStatus;
			if (uwRpy3ComStt != PR_COMM_NOERR)
			{
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
				{
					free(stInspRpy4.pstPostBondInspMin);
				}	// free point for one search
				CString szMsg;
				szMsg.Format("PR MultiSrchDieRpy3 finished with status %u!\n",	uwRpy3ComStt);
				HmiMessage(szMsg);
				break;
			}

			PR_UWORD uwRpy3Status = stInspRpy3.stStatus.uwPRStatus;

			// inspection result
			if( (PR_TRUE == m_stMultiSrchCmd.emPostBondInsp) &&
				PR_COMM_NOERR == uwRpy3ComStt &&
				!PR_ERROR_STATUS(uwRpy3Status) )
			{
				PR_SrchDieRpy4(ubSenderID,&stInspRpy4);

				PR_UWORD uwRpy4ComStt = stInspRpy4.stStatus.uwCommunStatus;
				if (uwRpy4ComStt != PR_COMM_NOERR)
				{
					if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
					{
						free(stInspRpy2.pstDieAlign);
					}
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
					{
						free(stInspRpy3.pstDieInspExtMin);
					}
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
					{
						free(stInspRpy4.pstPostBondInspMin);
					}	// free point for one search
					CString szMsg;
					szMsg.Format("PR_MultiSrchDieRpy4 finished with status %u!\n", uwRpy4ComStt);
					HmiMessage(szMsg);
					break;
				}
			}
		}

		USHORT usDieSum = stInspRpy2.uwNResults;
		if( fp!=NULL )
		{
			fprintf(fp, "found total dice is %d\n", usDieSum);
			fprintf(fp, "CalibX,%f,%f, calibY(%f,%f\n", 
				GetScanCalibX(), GetScanCalibXY(), GetScanCalibY(), GetScanCalibYX());
		}
		for(USHORT usIndex=0; usIndex<usDieSum; usIndex++)
		{
			CString szDieBin = "0";
			PR_RCOORD	rcDieCtr = stInspRpy2.pstDieAlign[usIndex].rcoDieCentre;
			PR_REAL		dDieRot  = stInspRpy2.pstDieAlign[usIndex].rDieRot;
			if( IsMS90HalfSortMode() )
			{
				if( dDieRot>90 )
					dDieRot -= 180;
				else if( dDieRot<-90 )
					dDieRot += 180;
			}
			PR_UWORD uwRpy3Status = 0, uwRpy4Status = 0;
			if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
			{
				uwRpy3Status = stInspRpy3.pstDieInspExtMin[usIndex].uwPRStatus;
			}
			if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
			{
				uwRpy4Status = stInspRpy4.pstPostBondInspMin[usIndex].uwPRStatus;
			}

			int siStepX = 0, siStepY = 0;
			CalcScanDiePosition(lX, lY, rcDieCtr, siStepX, siStepY);

			SetGoodPosnOnly(siStepX, siStepY, dDieRot, szDieBin, 0, 0);

			if (bIsFindGT == FALSE)
			{
				LONG lPosn = GetDieCoordinate(rcDieCtr);
				if( lPosn!=4 )
				{
					if( lPosn>10000 )
					{
						lPosn = lPosn - 10000;
						dwList.SetAt(3, 1);
					}
					if( lPosn>1000 )
					{
						lPosn = lPosn - 1000;
						dwList.SetAt(1, 1);
					}
					if( lPosn>100 )
					{
						lPosn = lPosn - 100;
						dwList.SetAt(2, 1);
					}
					if( lPosn>10 )
					{
						lPosn = lPosn - 10;
						dwList.SetAt(0, 1);
					}
				}
				else
				{
						dwList.SetAt(4, 1);
				}
			}


			if( bDrawDie )
			{
				PR_COLOR sColor;

				if(DieIsBadCut(uwRpy4Status))
				{
					sColor = PR_COLOR_BLUE;
				}
				else if ( ScanDieIsDefect(usDieType, uwRpy3Status) )
				{
					sColor = PR_COLOR_RED;
				}
				else
				{
					sColor = PR_COLOR_GREEN;
				}

				PR_COORD stCorner1;
				PR_COORD stCorner2;
				if( GetDieShape()==WPR_RHOMBUS_DIE )
				{
					stCorner1.x	= (PR_WORD)_round(rcDieCtr.x - GetScanNmlSizePixelX() / 4);
					stCorner1.y	= (PR_WORD)_round(rcDieCtr.y - GetScanNmlSizePixelY() / 4);
					stCorner2.x = (PR_WORD)_round(rcDieCtr.x + GetScanNmlSizePixelX() / 4);
					stCorner2.y = (PR_WORD)_round(rcDieCtr.y + GetScanNmlSizePixelY() / 4);
				}
				else
				{
					stCorner1.x	= (PR_WORD)_round(rcDieCtr.x - GetScanNmlSizePixelX() / 2);
					stCorner1.y	= (PR_WORD)_round(rcDieCtr.y - GetScanNmlSizePixelY() / 2);
					stCorner2.x = (PR_WORD)_round(rcDieCtr.x + GetScanNmlSizePixelX() / 2);
					stCorner2.y = (PR_WORD)_round(rcDieCtr.y + GetScanNmlSizePixelY() / 2);
				}
				DrawRectangleBox(stCorner1, stCorner2, sColor);
			}
		}

		//Free the reply 2
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
		{
			free(stInspRpy2.pstDieAlign);
		}
		//Free the reply 3
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
		{
			free(stInspRpy3.pstDieInspExtMin);
		}
		//Free the reply 4
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
		{
			free(stInspRpy4.pstPostBondInspMin);
		}	// free point for one search
	}

	if( fp!=NULL )
	{
		fclose(fp);
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("5M multi search finish and remove image");
	PR_REMOVE_SHARE_IMAGE_CMD     stRmvCmd;
	PR_REMOVE_SHARE_IMAGE_RPY     stRmvRpy;

	PR_InitRemoveShareImgCmd(&stRmvCmd);
	stRmvCmd.ulImageID = ulImageID;
	stRmvCmd.emFreeAll = PR_TRUE; // You can set this to PR_TRUE if you want to clear all Share Image for this channel
	PR_RemoveShareImgCmd(&stRmvCmd, ubSenderID, ubReceiverID, &stRmvRpy);

	if (PR_COMM_NOERR != stRmvRpy.uwCommunStatus)
	{
		HmiMessage("One Search fail to delete the image");
	}

	return TRUE;
}

BOOL CWaferPr::ScanAutoMultiSearch2ndPR(BOOL bDrawDie)
{
	LONG lX = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["X"];
	LONG lY = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["Y"];

	#define	FOV_5M_ROW_COL	100

	PR_UBYTE	ubSenderID	 =  GetScnSenID();
	PR_UBYTE	ubReceiverID =  GetScnRecID();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	CMSLogFileUtility::Instance()->WL_LogStatus("5M multi search 2nd PR prepare");
	PR_GRAB_SHARE_IMAGE_CMD		stGrbCmd;
	PR_InitGrabShareImgCmd(&stGrbCmd);
	// AOI partial die step 3 During Prescan, 
	stGrbCmd.emPurpose			= GetScnPurpose();
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= m_stScanSrchCmd2.auwRecordID[0];
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_NORMAL;

	PR_GRAB_SHARE_IMAGE_RPY		stGrbRpy;
	PR_GrabShareImgCmd(&stGrbCmd, ubSenderID, ubReceiverID, &stGrbRpy);
	if (PR_COMM_NOERR != stGrbRpy.uwCommunStatus)
	{
		HmiMessage("2nd PR grab share image cmd fail");
		return FALSE;
	}

	PR_GRAB_SHARE_IMAGE_RPY2	stGrbRpy2;
	PR_GrabShareImgRpy(ubSenderID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		HmiMessage("2nd PR grab share image rpy fail");
		return FALSE;
	}

	int nSubImageNum = m_uwFrameSubImageNum;
	FILE *fp = NULL;
	errno_t nFileErr = -1;
	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	CString szTime;
	szTime = ctDateTime.Format("%Y%m%d%H%M%S");
	if( pUtl->GetPrescanDebug() )
	{
		WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
		CString szLogFileName;
		szLogFileName.Format("%s\\History\\FovOneSearch_DC%d", gszUSER_DIRECTORY, pCPInfo->GetDCState());
		szLogFileName = szLogFileName + ".txt";
		nFileErr = fopen_s(&fp, szLogFileName, "a");
		if ((nFileErr == 0) && (fp != NULL))
		{
			fprintf(fp, "col,row,prx,pry,offx,offy,posnx,posny,via clb,%d\n", nSubImageNum);
		}
	}

	// now add the image to queue
	PR_ULWORD ulStationID	= stGrbRpy2.ulStationID;
	PR_ULWORD ulImageID		= stGrbRpy2.ulImageID;

	CMSLogFileUtility::Instance()->WL_LogStatus("5M multi search 2nd PR loop to find out all dice");
	for(int i = 1; i<=nSubImageNum; i++)
	{
		PR_UWORD uwRpy1Status, uwRpy1ComStt, uwRpy2ComStt;
		IMG_UWORD		uwCommStatus;
		PR_WORD lULX = 0, lULY = 0, lLRX = 0, lLRY = 0;
		GetSubWindow(i, lULX, lULY, lLRX, lLRY);
		PR_WORD lCTX = (lULX + lLRX)/2;
		PR_WORD lCTY = (lULY + lLRY)/2;

		if( bDrawDie )
		{
			PR_COLOR sColor = PR_COLOR_YELLOW;
			if( i%2==0 )
				sColor = PR_COLOR_CYAN;
			PR_COORD stCorner1;
			PR_COORD stCorner2;
			stCorner1.x = lULX;
			stCorner1.y = lULY;
			stCorner2.x = lLRX;
			stCorner2.y = lLRY;
			DrawRectangleBox(stCorner1, stCorner2, sColor);
		}

		PR_SRCH_DIE_CMD stInspCmd	= m_stScanSrchCmd2;

		stInspCmd.ulLatchImageID	= ulImageID;
		stInspCmd.uwLatchStationID	= (PR_UWORD) ulStationID;

		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = lULX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = lULY;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = lLRX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = lLRY;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.x		 = lCTX;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.y		 = lCTY;

		stInspCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
		stInspCmd.uwHostMallocMaxNoOfDieInRpy2			= WPR_ONE_SEARCH_MAX_DIE;
		if( m_stScanSrchCmd2.emDefectInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy3		= WPR_ONE_SEARCH_MAX_DIE;
		if( m_stScanSrchCmd2.emPostBondInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy4		= WPR_ONE_SEARCH_MAX_DIE;
		PR_SrchDieCmd(&stInspCmd, ubSenderID, ubReceiverID, &uwCommStatus);

		if ( uwCommStatus != PR_COMM_NOERR )
		{
			CString szMsg;
			szMsg.Format("2nd PR PR_MultiSrchDieCmd finished with status %u!", uwCommStatus);
			HmiMessage(szMsg);
			//v4.39T10	//Klocwork
			if ((nFileErr == 0) && (fp != NULL))
			{
				fprintf(fp, "\n\n");
				fclose(fp);
			}
			return FALSE;
		}

		PR_SRCH_DIE_RPY1 stInspRpy1;
		PR_SrchDieRpy1(ubSenderID, &stInspRpy1);
		uwRpy1ComStt = stInspRpy1.uwCommunStatus;
		uwRpy1Status = stInspRpy1.uwPRStatus;

		if (uwRpy1ComStt != PR_COMM_NOERR || PR_ERROR_STATUS(uwRpy1Status))
		{
			CString szMsg;
			szMsg.Format("2nd PR PR MultiSrchDieRpy1 finished with status %u, %u!\n",	uwRpy1ComStt, uwRpy1Status);
			HmiMessage(szMsg);
			if ((nFileErr == 0) && (fp != NULL))
			{
				fprintf(fp, "\n\n");
				fclose(fp);
			}
			return FALSE;
		}

		PR_UWORD		usDieType;
		PR_SRCH_DIE_RPY2	stInspRpy2;

		stInspRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*)malloc (stInspCmd.uwHostMallocMaxNoOfDieInRpy2*sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign == NULL)
		{
			HmiMessage("2nd PR Search Init stDieAlign null pointer");
			break; 
		}
		PR_SrchDieRpy2(ubSenderID, &stInspRpy2);
		uwRpy2ComStt = stInspRpy2.stStatus.uwCommunStatus;
		if (uwRpy2ComStt != PR_COMM_NOERR)
		{
			//Free the reply 2
			if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
			{
				free(stInspRpy2.pstDieAlign);
			}
			CString szMsg;
			szMsg.Format("2nd PR PR MultiSrchDieRpy2 finished with status %u!\n", uwRpy2ComStt);
			HmiMessage(szMsg);
			break;
		}
		usDieType = stInspRpy2.stStatus.uwPRStatus;

		PR_SRCH_DIE_RPY3	stInspRpy3;
		PR_SRCH_DIE_RPY4	stInspRpy4;
		if( m_stScanSrchCmd2.emDefectInsp==PR_TRUE )
		{
			stInspRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*)malloc( stInspCmd.uwHostMallocMaxNoOfDieInRpy3*sizeof(PR_DIE_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin==NULL )
			{
				//Free the reply 2
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				HmiMessage("2nd PR Search Init stDieAlign null pointer");
				break; 
			}
		}
		if( m_stScanSrchCmd2.emPostBondInsp==PR_TRUE )
		{
			stInspRpy4.pstPostBondInspMin = (PR_POST_INSP_MINIMAL_RPY*)malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy4*sizeof(PR_POST_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin==NULL )
			{
				//Free the reply 2
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				//Free the reply 3
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				HmiMessage("One Search Init stDieAlign null pointer");
				break; 
			}
		}

		if( ((PR_TRUE==m_stScanSrchCmd2.emDefectInsp) || (PR_TRUE==m_stMultiSrchCmd.emPostBondInsp)) &&
			(PR_COMM_NOERR==uwRpy2ComStt) && !PR_ERROR_STATUS(usDieType) )
		{
			PR_SrchDieRpy3(ubSenderID,&stInspRpy3);
			PR_UWORD uwRpy3ComStt = stInspRpy3.stStatus.uwCommunStatus;
			if (uwRpy3ComStt != PR_COMM_NOERR)
			{
				//Free the reply 2
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				//Free the reply 3
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				//Free the reply 4
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
				{
					free(stInspRpy4.pstPostBondInspMin);
				}	// free point for one search
				CString szMsg;
				szMsg.Format("PR MultiSrchDieRpy3 finished with status %u!\n",	uwRpy3ComStt);
				HmiMessage(szMsg);
				break;
			}

			PR_UWORD uwRpy3Status = stInspRpy3.stStatus.uwPRStatus;

			// inspection result
			if( (PR_TRUE == m_stScanSrchCmd2.emPostBondInsp) &&
				(PR_COMM_NOERR == uwRpy3ComStt) && !PR_ERROR_STATUS(uwRpy3Status) )
			{
				PR_SrchDieRpy4(ubSenderID,&stInspRpy4);

				PR_UWORD uwRpy4ComStt = stInspRpy4.stStatus.uwCommunStatus;
				if (uwRpy4ComStt != PR_COMM_NOERR)
				{
					//Free the reply 2
					if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
					{
						free(stInspRpy2.pstDieAlign);
					}
					//Free the reply 3
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
					{
						free(stInspRpy3.pstDieInspExtMin);
					}
					//Free the reply 4
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
					{
						free(stInspRpy4.pstPostBondInspMin);
					}	// free point for one search
					CString szMsg;
					szMsg.Format("PR_MultiSrchDieRpy4 finished with status %u!\n", uwRpy4ComStt);
					HmiMessage(szMsg);
					break;
				}
			}
		}

		USHORT usDieSum = stInspRpy2.uwNResults;
		if( fp!=NULL )
		{
			fprintf(fp, "found total dice is %d\n", usDieSum);
			fprintf(fp, "CalibX,%f,%f, calibY(%f,%f\n", 
				GetScanCalibX(), GetScanCalibXY(), GetScanCalibY(), GetScanCalibYX());
		}
		for(USHORT usIndex=0; usIndex<usDieSum; usIndex++)
		{
			CString szDieBin = "0";
			PR_RCOORD	rcDieCtr = stInspRpy2.pstDieAlign[usIndex].rcoDieCentre;
			PR_REAL		dDieRot  = stInspRpy2.pstDieAlign[usIndex].rDieRot;
			if( IsMS90HalfSortMode() )
			{
				if( dDieRot>90 )
					dDieRot -= 180;
				else if( dDieRot<-90 )
					dDieRot += 180;
			}
			PR_UWORD uwRpy3Status = 0, uwRpy4Status = 0;
			if( m_stScanSrchCmd2.emDefectInsp==PR_TRUE )
			{
				uwRpy3Status = stInspRpy3.pstDieInspExtMin[usIndex].uwPRStatus;
			}
			if( m_stScanSrchCmd2.emPostBondInsp==PR_TRUE )
			{
				uwRpy4Status = stInspRpy4.pstPostBondInspMin[usIndex].uwPRStatus;
			}

			if( bDrawDie )
			{
				PR_COLOR sColor;

				if(DieIsBadCut(uwRpy4Status))
				{
					sColor = PR_COLOR_BLUE;
				}
				else if ( ScanDieIsDefect(usDieType, uwRpy3Status) )
				{
					sColor = PR_COLOR_RED;
				}
				else
				{
					sColor = PR_COLOR_GREEN;
				}

				PR_COORD stCorner1;
				PR_COORD stCorner2;
				if( GetDieShape()==WPR_RHOMBUS_DIE )
				{
					stCorner1.x	= (PR_WORD)_round(rcDieCtr.x - GetScanNmlSizePixelX() / 4);
					stCorner1.y	= (PR_WORD)_round(rcDieCtr.y - GetScanNmlSizePixelY() / 4);
					stCorner2.x = (PR_WORD)_round(rcDieCtr.x + GetScanNmlSizePixelX() / 4);
					stCorner2.y = (PR_WORD)_round(rcDieCtr.y + GetScanNmlSizePixelY() / 4);
				}
				else
				{
					stCorner1.x	= (PR_WORD)_round(rcDieCtr.x - GetScanNmlSizePixelX() / 2);
					stCorner1.y	= (PR_WORD)_round(rcDieCtr.y - GetScanNmlSizePixelY() / 2);
					stCorner2.x = (PR_WORD)_round(rcDieCtr.x + GetScanNmlSizePixelX() / 2);
					stCorner2.y = (PR_WORD)_round(rcDieCtr.y + GetScanNmlSizePixelY() / 2);
				}
				DrawRectangleBox(stCorner1, stCorner2, sColor);
			}
		}

		//Free the reply 2
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
		{
			free(stInspRpy2.pstDieAlign);
		}
		//Free the reply 3
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
		{
			free(stInspRpy3.pstDieInspExtMin);
		}
		//Free the reply 4
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
		{
			free(stInspRpy4.pstPostBondInspMin);
		}	// free point for one search
	}

	if( fp!=NULL )
	{
		fclose(fp);
	}

	PR_REMOVE_SHARE_IMAGE_CMD     stRmvCmd;
	PR_REMOVE_SHARE_IMAGE_RPY     stRmvRpy;

	PR_InitRemoveShareImgCmd(&stRmvCmd);
	stRmvCmd.ulImageID = ulImageID;
	stRmvCmd.emFreeAll = PR_TRUE; // You can set this to PR_TRUE if you want to clear all Share Image for this channel
	PR_RemoveShareImgCmd(&stRmvCmd, ubSenderID, ubReceiverID, &stRmvRpy);

	if (PR_COMM_NOERR != stRmvRpy.uwCommunStatus)
	{
		HmiMessage("2nd PR Search fail to delete the image");
	}

	return TRUE;
}	//	scan multi search for second pr

BOOL CWaferPr::SubCutScanFOV(DOUBLE &dShrinkRatio)
{
#ifdef VS_5MCAM
	if( IsDP() )
	{
	}
	else
	{
		if( GetScnZoom()==GetNmlZoom() )
		{
			return FALSE;
		}

		if( GetScnZoom() != CPrZoomSensorMode::PR_ZOOM_MODE_FF && GetScnZoom() != CPrZoomSensorMode::PR_ZOOM_MODE_1X )
		{
			return FALSE;
		}
	}

	DOUBLE dGet = m_dWprZoomRoiShrink/100.0;

	// The nScanCut for 100-800 only
	if ( dGet <= 0.0 )
	{
		return FALSE;
	}
	if ( dGet > 1.0 )
		dShrinkRatio = 1.0;
	else
		dShrinkRatio = dGet;

	return TRUE;
#endif

	return FALSE;
}



BOOL CWaferPr::CheckRepeatDieInMatrix3X3(const ULONG ulRow, const ULONG ulCol, const LONG lNewWftX, const LONG lNewWftY, const ULONG ulDieSameTolX, const ULONG ulDieSameTolY)
{
	LONG lEndLoopRow = 1;
	LONG lEndLoopCol = 1;
	BOOL bFindRepeat = FALSE;
	LONG lNearByX = 0, lNearByY = 0;
	LONG lStartLoopRow = 0, lStartLoopCol = 0;
	if (ulRow > 0)
	{
		lStartLoopRow = -1;
	}

	if (ulCol > 0)
	{
		lStartLoopCol = -1;
	}

	for (LONG lLoopRow = lStartLoopRow; lLoopRow <= lEndLoopRow; lLoopRow++)
	{
		for (LONG lLoopCol = lStartLoopCol; lLoopCol <= lEndLoopCol; lLoopCol++)
		{
			if (GetPrescanPosition(ulRow + lLoopRow, ulCol + lLoopCol, lNearByX, lNearByY))
			{
				if (labs(lNearByX - lNewWftX) <= (LONG)ulDieSameTolX &&
					labs(lNearByY - lNewWftY) <= (LONG)ulDieSameTolY)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


BOOL CWaferPr::RescanConstructMap()	//	rescan X	3	MS only
{
	BOOL bReturn = TRUE;
#define	WPR_ADD_DIE_INDEX	-101
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp	*pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulIndex;
	CString szConLogFile;
	CUIntArray aulSelGradeList;
	m_WaferMapWrapper.GetSelectedGradeList(aulSelGradeList);
	//	PR too many no die, auto rescan
	m_ulNoDieSkipCount	= 0;
	(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 0;	//	rescan done

	UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + ucOffset;
	UCHAR ucReferGrade	= m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();

	BOOL  bEmptyAction	= m_bPrescanEmptyAction;
	BOOL  bFakeAction	= m_bScanDetectFakeEmpty;
	UCHAR ucFakeGrade	= m_ucScanFakeEmptyGrade;

	BOOL bShortPathNgDieToInvalid	= FALSE;

	if (m_bPrescanEmptyToNullBin)
	{
		ucEmptyGrade = ucNullBin;
	}

	CString szAlgorithm, szPathFinder;
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
	if (szAlgorithm.Find("Sorting Path 1") != -1)
	{
		bShortPathNgDieToInvalid = TRUE;
	}

	CString szMsg;
	szMsg.Format("WPR: rescan done to build map; total index counter %d", m_nPrescanIndexCounter);
	if( m_lScanPrAbnormalCount > 0 )
	{
		CString szTemp;
		szTemp.Format(" PR abnormal total %ld", m_lScanPrAbnormalCount);
		szMsg += szTemp;
		SetErrorMessage(szTemp);
	}
	SaveScanTimeEvent(szMsg);

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	// capture last image
	LONG lEncX, lEncY;

	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);

	ULONG ulDiePitchX_X = labs(GetDiePitchX_X());
	ULONG ulDiePitchY_Y = labs(GetDiePitchY_Y());

	// add all alignment point and refer points to list
	LONG lEdgeMinRow = GetMapValidMinRow();
	LONG lEdgeMaxRow = GetMapValidMaxRow();
	LONG lEdgeMinCol = GetMapValidMinCol();
	LONG lEdgeMaxCol = GetMapValidMaxCol();
	if (pUtl->GetPrescanRegionMode())
	{
		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
		ULONG lULRow, lULCol, lLRRow, lLRCol;
		if (pSRInfo->GetRegion(ulTgtRegion, lULRow, lULCol, lLRRow, lLRCol))
		{
			lEdgeMinRow = max((LONG)lULRow,		lEdgeMinRow);
			lEdgeMaxRow = min((LONG)lLRRow - 1, lEdgeMaxRow);
			lEdgeMinCol = max((LONG)lULCol,		lEdgeMinCol);
			lEdgeMaxCol = min((LONG)lLRCol - 1, lEdgeMaxCol);
		}
	}

	if (pUtl->GetPrescanDebug())
	{
		SaveScanTimeEvent("    WFT: rescan write old map");
		CString szNewMapPath;
		szNewMapPath.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), RESCAN_OLD_MAP_FILE);
		m_WaferMapWrapper.WriteMap(szNewMapPath);
	}
	ULONG ulRefDieCount = 0;
	FILE *fpBase = NULL;	//Klocwork
	errno_t nFileBaseErr = -1;
	if (pUtl->GetPrescanDebug())
	{
		szConLogFile.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), RESCAN_BASS_IN_FILE);
		nFileBaseErr = fopen_s(&fpBase, szConLogFile, "a");
	}

	if ((nFileBaseErr == 0) && (fpBase != NULL))
	{
		fprintf(fpBase, "\n");
		fprintf(fpBase, "Scanned found base die list\n");
	}

	LONG lIndex = 0;
	ULONG ulRow, ulCol;

	ULONG ulRsnBaseSize = (ULONG)m_dwaRsnBaseWfX.GetSize();
	for (UINT unBaseIndex = 0; unBaseIndex < ulRsnBaseSize; unBaseIndex++)
	{
		ulRow  = m_dwaRsnBaseRow.GetAt(unBaseIndex);
		ulCol  = m_dwaRsnBaseCol.GetAt(unBaseIndex);
		lEncX  = m_dwaRsnBaseWfX.GetAt(unBaseIndex);
		lEncY  = m_dwaRsnBaseWfY.GetAt(unBaseIndex);
		ulRefDieCount++;
		if (fpBase != NULL)
		{
			LONG lUserRow = 0, lUserCol = 0;
			ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
			fprintf(fpBase, "%8ld,%8ld,%4ld,%4ld,%4ld,%4ld,%4lu\n", lEncX, lEncY, ulRow, ulCol, lUserRow, lUserCol, ulRefDieCount);
		}
	}

	lIndex = WPR_ADD_DIE_INDEX;
	if (fpBase != NULL)
	{
		fprintf(fpBase, "\n");	//	unfound in scan list.
		fprintf(fpBase, "Scanned frame unfound base die list\n");
	}

	for (INT nRow = m_lScanFrameMinRow; nRow <= m_lScanFrameMaxRow; nRow++)
	{
		CString szMsg = "", szTemp = "";
		for (INT nCol = m_lScanFrameMinCol; nCol <= m_lScanFrameMaxCol; nCol++)
		{
			ULONG lBasePoints = 0;
			if (GetRescanFrameBaseNum(nRow, nCol, lBasePoints) == false)	//	get frame's row and column and find the assistant point and update and remove.
			{
				continue;
			}

			if (lBasePoints == 0)
			{
				continue;
			}

			for (UINT ulIndex=0; ulIndex<lBasePoints; ulIndex++)
			{
				LONG lBaseRow = 0, lBaseCol = 0, lBaseWfX = 0, lBaseWfY = 0;
				if (GetFrameRescanBasePoint(nRow, nCol, ulIndex, lBaseRow, lBaseCol, lBaseWfX, lBaseWfY))	//	remain
				{
					lIndex--;
					m_cPIMap.SortAdd_Tail(lBaseWfX, lBaseWfY, lIndex, FALSE);
					if (ulRsnBaseSize == 0)
					{
						m_dwaRsnBaseRow.Add(lBaseRow);
						m_dwaRsnBaseCol.Add(lBaseCol);
						m_dwaRsnBaseWfX.Add(lBaseWfX);
						m_dwaRsnBaseWfY.Add(lBaseWfY);
					}
					ulRefDieCount++;
					if (fpBase != NULL)
					{
						LONG lUserRow = 0, lUserCol = 0;
						ConvertAsmToOrgUser(lBaseRow, lBaseCol, lUserRow, lUserCol);
						fprintf(fpBase, "%8ld,%8ld,%4ld,%4ld,%4ld,%4ld,%4lu\n", lBaseWfX, lBaseWfY, lBaseRow, lBaseCol, lUserRow, lUserCol, ulRefDieCount);
					}
				}
			}
		}
	}

	//	also pass the rescanbase not found.
	ulRsnBaseSize = (ULONG)m_dwaRsnBaseWfX.GetSize();
	if( pUtl->GetRescanBasePointsNum()>0 && fpBase!=NULL )
	{
		fprintf(fpBase, "\n");	//	unfound in scan list.
		fprintf(fpBase, "Scanned unfound base die list\n");
	}

	for (UINT ulLeftBase = 0; ulLeftBase < pUtl->GetRescanBasePointsNum(); ulLeftBase++)
	{
		if (pUtl->GetRescanBasePoint(ulLeftBase, ulRow, ulCol, lEncX, lEncY))
		{
			lIndex--;
			m_cPIMap.SortAdd_Tail(lEncX, lEncY, lIndex, FALSE);
			if (ulRsnBaseSize == 0)
			{
				m_dwaRsnBaseRow.Add(ulRow);
				m_dwaRsnBaseCol.Add(ulCol);
				m_dwaRsnBaseWfX.Add(lEncX);
				m_dwaRsnBaseWfY.Add(lEncY);
			}
			ulRefDieCount++;
			if (fpBase != NULL)
			{
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				fprintf(fpBase, "%8ld,%8ld,%4ld,%4ld,%4ld,%4ld,%4lu\n", lEncX, lEncY, ulRow, ulCol, lUserRow, lUserCol, ulRefDieCount);
			}
		}
	}

	if( fpBase!=NULL )
	{
		fclose(fpBase);
	}

	SaveScanTimeEvent("WPR: rescan to save infor and clear good list.");

	SavePrescanInfoPr(GetNewPickCount());

	ClearGoodInfo();

//	RescanGetRemain		init save map and raw data memory, get out state
	DelFrameRescanBasePoints();		DelRescanFrameBaseNum();
	DelRescanLostData();	DelRescanRemainPoint();	//	rescan construct map begin.
	
	bool *pbGetOut;
	pbGetOut = new bool[WSGetPrescanTotalDie()+10];
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		pbGetOut[ulIndex] = false;
	}
//	RescanGetRemain		init done

	// build physical map
	int nMaxSpan = 5, nMaxIterateCount = -1, nMaxAllowLeft = 10;	// nMaxAllowPitch
	BOOL bPreSorted = TRUE;
	ULONG ulDieSizeXTol = (ULONG) (GetDieSizeX() * m_dPrescanPitchTolX/100.0);
	ULONG ulDieSizeYTol = (ULONG) (GetDieSizeY() * m_dPrescanPitchTolY/100.0);

	FILE *fpPassIn = NULL;	//Klocwork
	errno_t nErr = -1;
	if( pUtl->GetPrescanDebug() )
	{
		szConLogFile.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), RESCAN_PASS_IN_FILE);
		nErr = fopen_s(&fpPassIn, szConLogFile, "w");
	}
	if ((nErr == 0) && (fpPassIn != NULL))
	{
		fprintf(fpPassIn, "TolX,TolY,PchX,PchY,...\n");
		fprintf(fpPassIn, "%lu,%lu,%lu,%lu,%d,%d,%d,%d\n", 
			ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX_X, ulDiePitchY_Y, 
			bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);
		fprintf(fpPassIn, "Below is the base reference point list\n");
		for(ULONG ulIndex=0; ulIndex < (ULONG)m_dwaRsnBaseWfX.GetSize(); ulIndex++)
		{
			lEncX = m_dwaRsnBaseWfX.GetAt(ulIndex);
			lEncY = m_dwaRsnBaseWfY.GetAt(ulIndex);
			LONG lIndex = 0-ulIndex;
//			m_cPIMap.SortAdd_Tail(lEncX, lEncY, lIndex, FALSE);
			fprintf(fpPassIn, "%ld,%ld,%ld,%ld,%ld\n",
				lEncX, lEncY, m_dwaRsnBaseRow.GetAt(ulIndex), m_dwaRsnBaseCol.GetAt(ulIndex), lIndex);
		}

		fprintf(fpPassIn, "Below is the raw data list\n");
		for(ULONG ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if(	WSGetPrescanPosn(ulIndex, lEncX, lEncY) )
			{
				fprintf(fpPassIn, "%ld,%ld,%ld\n", lEncX, lEncY, ulIndex);
			}
		}
		fclose(fpPassIn);
	}

	SaveScanTimeEvent("WPR: rescan to construct normal map");

	m_cPIMap.KeepIsolatedDice(FALSE);
	if( GetDieShape()==WPR_RHOMBUS_DIE )
	{
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();
		m_cPIMap.SetXOffset(lDiePitchY_X);
		szMsg.Format("WPR: die pitch x %d,%d, y %d,%d", ulDiePitchX_X, lDiePitchX_Y, ulDiePitchY_Y, lDiePitchY_X);
		SaveScanTimeEvent(szMsg);
	}
	else
	{
		m_cPIMap.SetXOffset(0);
	}

	m_cPIMap.ConstructMap(m_dwaRsnBaseWfX, m_dwaRsnBaseWfY, m_dwaRsnBaseRow, m_dwaRsnBaseCol,
		ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX_X, ulDiePitchY_Y,
		bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);

	SaveScanTimeEvent("WPR: rescan construct map complete to clear prescan info");

	ClearPrescanInfo();
//	RescanGetRemain		reset map move down

	ULONG ulDieLostTolX = (ULONG) (ulDiePitchX_X*RESCAN_LOOP_FIND_LOST_TOL/100.0);
	ULONG ulDieLostTolY = (ULONG) (ulDiePitchY_Y*RESCAN_LOOP_FIND_LOST_TOL/100.0);
	ULONG ulDieSameTolX = (ULONG) (ulDiePitchX_X*RESCAN_SAME_DIE_TOL/100.0);
	ULONG ulDieSameTolY = (ULONG) (ulDiePitchY_Y*RESCAN_SAME_DIE_TOL/100.0);

	szMsg.Format("WPR: to loop find new lost tolerance %lu,%lu, same tolerance %lu,%lu",
		ulDieLostTolX, ulDieLostTolY, ulDieSameTolX, ulDieSameTolY);
	SaveScanTimeEvent(szMsg);
	// get all phsical list infromation to prescan result list
	if( pUtl->GetPrescanDebug() )
	{
		CString szScanPmpFile;
		szScanPmpFile.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), RESCAN_PMP_MAP_FILE);
		FILE *fpMap = NULL;
		errno_t nErr = fopen_s(&fpMap, szScanPmpFile, "w");

		if ((nErr == 0) && (fpMap != NULL))
		{
			BOOL bFirstGood = FALSE;
			ULONG ulPyiRowMax = 0, ulPyiColMax = 0;
			m_cPIMap.GetDimension(ulPyiRowMax, ulPyiColMax);

			fprintf(fpMap, "PHY range Row(%lu,%lu)  Col(%lu,%lu)\n", 0, ulPyiRowMax, 0, ulPyiColMax);
			fprintf(fpMap, "MAP range (0,%lu)  (0,%lu)\n", lEdgeMaxRow, lEdgeMaxCol);
			fprintf(fpMap, "MAP range valid Row(%lu,%lu)  Col(%lu,%lu)\n", 
				lEdgeMinRow, lEdgeMaxRow, lEdgeMinCol, lEdgeMaxCol);
			fprintf(fpMap, "DataFileName,,%s\n", (LPCTSTR)szScanPmpFile);
			fprintf(fpMap, "LotNumber,,\nDeviceNumber,,\nwafer id=\nTestTime,\nMapFileName,,\nTransferTime,\n");
			fprintf(fpMap, "\n");

			for (ulRow = 0; ulRow <= ulPyiRowMax; ulRow++)
			{
				for(ulCol=0; ulCol<=ulPyiColMax; ulCol++)
				{
					BOOL bDieOverLap = FALSE;
					LONG lIndex = 0;
					if( m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap)==FALSE )
					{
						continue;
					}
					if( lIndex<=WPR_ADD_DIE_INDEX )
					{
						continue;
					}

					if( bFirstGood==FALSE )
					{
						fprintf(fpMap, "map data\n");
						fprintf(fpMap, "%lu,%lu,\n", ulCol, ulRow);
						bFirstGood = TRUE;
					}
					fprintf(fpMap, "%lu,%lu,%d\n", ulCol, ulRow, 1);
				}
			}

			fclose(fpMap);
		}
	}

	FILE *fpOut = NULL;
	errno_t nOutErr = -1;
	if (pUtl->GetPrescanDebug())
	{
		szConLogFile.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), RESCAN_GET_OUT_FILE);
		nOutErr = fopen_s(&fpOut, szConLogFile, "w");
		if ((nOutErr == 0) && (fpOut != NULL))
		{
			fprintf(fpOut, "%s\n", szMsg);
		}
	}

	ULONG ulRemainMapTotal = 0, ulMapTotal = 0, ulScanTotal = 0, ulMapLostTotal = 0;
	SaveScanTimeEvent("WPR: rescan to update scan info from phy map builder.");
	for(ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
	{
		for(ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
		{
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}

//	RescanGetRemain		check and skip null bin empty grade, refer die, no map phy die
			UCHAR ucCurGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucCurGrade==ucNullBin )
			{
				continue;
			}
			if( m_bPrescanEmptyToNullBin==FALSE && m_bPrescanEmptyAction && ucCurGrade==ucEmptyGrade )
			{
				continue;
			}

			BOOL bIsRefer = m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) && ucReferGrade==ucCurGrade;
			BOOL bIsSpRef = IsSpecialReferGrade(ulRow, ulCol);
			BOOL bIsNoDie = IsNoDieGrade(ulRow, ulCol);
			if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_DIMREFER )
			{
				bIsRefer = TRUE;
			}
			if( bIsRefer || bIsSpRef || bIsNoDie )
			{
				continue;
			}

			LONG lOldWfX = 0, lOldWfY = 0;
			if( GetMapPhyPosn(ulRow, ulCol, lOldWfX, lOldWfY)==FALSE )
			{
				continue;
			}
			if( IsAutoRescanAndDEB() )
			{
				DEB_GetDiePosnOnly(ulRow, ulCol, lOldWfX, lOldWfY);
			}
//	RescanGetRemain		check done

			ulMapTotal++;
			LONG lIndex = 0;
			BOOL bDieOverLap = FALSE;
			BOOL bGetInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if( bGetInfo==FALSE || lIndex<=WPR_ADD_DIE_INDEX )
			{
//	RescanGetRemain		not in phy map, save it
				if( IsInSelectedGrades(ulRow, ulCol) )
				{
					SetRescanLostData(ulRow, ulCol, lOldWfX, lOldWfY);
					ulRemainMapTotal++;
				}
				continue;
			}

			DOUBLE dDieAngle = 0.0;
			CString szDieBin = "0";
			USHORT usDieState = 0;
			if( lIndex>0 )
			{
//	RescanGetRemain		found in phy map, set to true
				pbGetOut[lIndex] = true;
				LONG lDummyX = 0, lDummyY = 0;
				ULONG ulFrameInfo = 0;
				WSGetScanPosnAngleFrame(lIndex, lDummyX, lDummyY, dDieAngle, ulFrameInfo);
			}

			//Get from Builder map
			ulScanTotal++;	//	by physical map builder
			SetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
			if ((nOutErr == 0) && (fpOut != NULL) && pUtl->GetPrescanDebug() )
			{
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				fprintf(fpOut, "%ld,%ld,%ld,%ld,%ld,%ld\n", ulRow, ulCol, lUserRow, lUserCol, lEncX, lEncY);
			}
		}
	}
	szMsg.Format("WPR: rescan TRY 0 at %ld, map total %lu, remain %lu, scan raw %lu, at last scan found %lu",
		GetNewPickCount(), ulMapTotal, ulRemainMapTotal, WSGetPrescanTotalDie(), ulScanTotal);
	SaveScanTimeEvent(szMsg);

//	RescanGetRemain		store the remain raw data die, loop find missing from scan data
	ULONG ulRemainRawTotal = 0;
	ULONG ulGrabFrameTotal[SCAN_MAX_MATRIX_ROW][SCAN_MAX_MATRIX_COL];
	for(ULONG ulGrabRow=0; ulGrabRow<SCAN_MAX_MATRIX_ROW; ulGrabRow++)
	{
		for(ULONG ulGrabCol=0; ulGrabCol<SCAN_MAX_MATRIX_COL; ulGrabCol++)
		{
			ulGrabFrameTotal[ulGrabRow][ulGrabCol] = 0;
		}
	}
	if ((nOutErr == 0) && (fpOut != NULL))
	{
		fprintf(fpOut, "Raw data missing list out\n");
	}
	SaveScanTimeEvent("WPR: rescan to find missing die in raw data.");
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		if( pbGetOut[ulIndex]==true )
		{
			continue;
		}
		ULONG ulPassInImageID = 0;
		DOUBLE dDieAngle = 0;
		if( WSGetScanPosnAngleFrame(ulIndex, lEncX, lEncY, dDieAngle, ulPassInImageID)==false )
		{
			continue;
		}

		//#define MAKEWORD(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xff))) << 8))
		UCHAR ucGrabRow = HIBYTE(ulPassInImageID);	//#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
		UCHAR ucGrabCol = LOBYTE(ulPassInImageID);	//#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
		if( ucGrabRow>=SCAN_MAX_MATRIX_ROW || ucGrabCol>=SCAN_MAX_MATRIX_COL )
		{
			ucGrabRow = 0;
			ucGrabCol = 0;
		}
		ulGrabFrameTotal[ucGrabRow][ucGrabCol]++;
		ulRemainRawTotal++;
		SetRescanRemainPoint(ucGrabRow, ucGrabCol, ulGrabFrameTotal[ucGrabRow][ucGrabCol], ulIndex, lEncX, lEncY);
		if ((nOutErr == 0) && (fpOut != NULL))
		{
			fprintf(fpOut, "scan,%ld,%ld,%lu,%lu,%ld,%ld\n", ulPassInImageID, ulGrabFrameTotal[ucGrabRow][ucGrabCol], ucGrabRow, ucGrabCol, lEncX, lEncY);
		}
	}

	LONG lAlignMapRow, lAlignMapCol, lAlignX, lAlignY;
	pUtl->GetAlignPosition(lAlignMapRow, lAlignMapCol, lAlignX, lAlignY);
	LONG lAlignFrameRow = GetAlignFrameRow();
	LONG lAlignFrameCol = GetAlignFrameCol();
	LONG lMapLoopStepRow	= GetMapIndexStepRow();
	LONG lMapLoopStepCol	= GetMapIndexStepCol();
	if ((nOutErr == 0) && (fpOut != NULL))
	{
		fprintf(fpOut, "\nMap missing and to find in remain raw list\n");
		fprintf(fpOut, "align map %ld,%ld, grab %ld,%ld, step %ld,%ld\n",
			lAlignMapRow, lAlignMapCol, lAlignFrameRow, lAlignFrameCol, lMapLoopStepRow, lMapLoopStepCol);
	}
	DOUBLE dMapLoopStepRow	= lMapLoopStepRow;
	DOUBLE dMapLoopStepCol	= lMapLoopStepCol;
	DOUBLE dAlignMapRow		= lAlignMapRow;
	DOUBLE dAlignMapCol		= lAlignMapCol;
	SaveScanTimeEvent("WPR: rescan to match map missing with missing die in raw data.");
	for(ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
	{
		if( ulRemainRawTotal==0 )
		{
			break;
		}
		for(ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
		{
			LONG lOldWftX = 0, lOldWftY = 0;
			if( GetRescanLostData(ulRow, ulCol, lOldWftX, lOldWftY)==FALSE )
			{	//	old map position but missing after scan builder.
				continue;
			}

			UCHAR ucGrabRow = 0, ucGrabCol = 0;
			DOUBLE dRow = ulRow, dCol = ulCol;
			if( dRow>=dAlignMapRow )
			{
				ucGrabRow = (UCHAR) ((LONG)((dRow - dAlignMapRow)/dMapLoopStepRow + 0.5) + lAlignFrameRow);
			}
			else
			{
				ucGrabRow = (UCHAR) ((LONG)((dRow - dAlignMapRow)/dMapLoopStepRow - 0.5) + lAlignFrameRow);
			}
			if( dCol>=dAlignMapCol )
			{
				ucGrabCol = (UCHAR) ((LONG)((dCol -dAlignMapCol)/dMapLoopStepCol + 0.5) + lAlignFrameCol);
			}
			else
			{
				ucGrabCol = (UCHAR) ((LONG)((dCol -dAlignMapCol)/dMapLoopStepCol - 0.5) + lAlignFrameCol);
			}
			if (ucGrabRow < m_lScanFrameMinRow || ucGrabRow > m_lScanFrameMaxRow)
			{
				ucGrabRow = (UCHAR)lAlignFrameRow;
			}

			if (ucGrabCol < m_lScanFrameMinCol || ucGrabCol > m_lScanFrameMaxCol)
			{
				ucGrabCol = (UCHAR)lAlignFrameCol;
			}

			BOOL bFindMatchOne = FALSE;
			for(ulIndex=1; ulIndex<=ulGrabFrameTotal[ucGrabRow][ucGrabCol]; ulIndex++)
			{
				LONG lNewWftX = 0, lNewWftY = 0;
				LONG lIndex = 0;
				if( GetRescanRemainPoint(ucGrabRow, ucGrabCol, ulIndex, lIndex, lNewWftX, lNewWftY)==false )
				{
					continue;
				}

				if (labs(lOldWftX - lNewWftX) <= (LONG)ulDieLostTolX && labs(lOldWftY - lNewWftY) <= (LONG)ulDieLostTolY )
				{
					if (CheckRepeatDieInMatrix3X3(ulRow, ulCol, lNewWftX, lNewWftY, ulDieSameTolX, ulDieSameTolY))
					{
						continue;
					}
//No Repeat
					ulScanTotal++;	//	by checking in grab FOV
					CutRescanRemainPoint(ucGrabRow, ucGrabCol, ulIndex);
					SetScanInfo(ulRow, ulCol, lNewWftX, lNewWftY, 0.0, "0", 0);
					if( fpOut!=NULL )
					{
						LONG lUserRow = 0, lUserCol = 0;
						ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
						fprintf(fpOut, "grab,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n", 
							ucGrabRow, ucGrabCol, ulRow, ulCol, lUserRow, lUserCol, lNewWftX, lNewWftY, lOldWftX, lOldWftY);
					}
					CutRescanLostData(ulRow, ulCol);
					bFindMatchOne = TRUE;
					break;	//	find match, break
				}
			}	//	loop find in remain raw data die list

			if( bFindMatchOne==FALSE )
			{
				UCHAR ucStartGrabRow = (UCHAR)(ucGrabRow - 1);
				UCHAR ucStartGrabCol = (UCHAR)(ucGrabCol - 1);
				UCHAR ucEndGrabRow = (UCHAR)(ucGrabRow + 1);
				UCHAR ucEndGrabCol = (UCHAR)(ucGrabCol + 1);
				if( ucStartGrabRow<m_lScanFrameMinRow || ucStartGrabRow>m_lScanFrameMaxRow )
					ucStartGrabRow = (UCHAR)lAlignFrameRow;
				if( ucStartGrabCol<m_lScanFrameMinCol || ucStartGrabCol>m_lScanFrameMaxCol )
					ucStartGrabCol = (UCHAR)lAlignFrameCol;
				if( ucEndGrabRow<m_lScanFrameMinRow || ucEndGrabRow>m_lScanFrameMaxRow )
					ucEndGrabRow = (UCHAR)lAlignFrameRow;
				if( ucEndGrabCol<m_lScanFrameMinCol || ucEndGrabCol>m_lScanFrameMaxCol )
					ucEndGrabCol = (UCHAR)lAlignFrameCol;
				for(UCHAR ucLoopGrabRow=ucStartGrabRow; ucLoopGrabRow<=ucEndGrabRow; ucLoopGrabRow++)
				{
					for(UCHAR ucLoopGrabCol=ucStartGrabCol; ucLoopGrabCol<=ucEndGrabCol; ucLoopGrabCol++)
					{
						for(ulIndex=1; ulIndex<=ulGrabFrameTotal[ucLoopGrabRow][ucLoopGrabCol]; ulIndex++)
						{
							LONG lNewWftX = 0, lNewWftY = 0;
							LONG lIndex = 0;
							if( GetRescanRemainPoint(ucLoopGrabRow, ucLoopGrabCol, ulIndex, lIndex, lNewWftX, lNewWftY)==false )
							{
								continue;
							}

							if (labs(lOldWftX - lNewWftX) <= (LONG)ulDieLostTolX && labs(lOldWftY - lNewWftY) <= (LONG)ulDieLostTolY )
							{
								if (CheckRepeatDieInMatrix3X3(ulRow, ulCol, lNewWftX, lNewWftY, ulDieSameTolX, ulDieSameTolY))
								{
									continue;
								}

								ulScanTotal++;	//	by checking in nearby 3x3 FOV
								CutRescanRemainPoint(ucLoopGrabRow, ucLoopGrabCol, ulIndex);
								SetScanInfo(ulRow, ulCol, lNewWftX, lNewWftY, 0.0, "0", 0);
								if( fpOut!=NULL )
								{
									LONG lUserRow = 0, lUserCol = 0;
									ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
									fprintf(fpOut, "wide,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n", 
										ucLoopGrabRow, ucLoopGrabCol, ulRow, ulCol, lUserRow, lUserCol, lNewWftX, lNewWftY, lOldWftX, lOldWftY);
								}
								CutRescanLostData(ulRow, ulCol);
								bFindMatchOne = TRUE;
								break;	//	find match, break
							}
						}	//	loop find in remain raw data die list
						if( bFindMatchOne )
						{
							break;
						}
					}
					if( bFindMatchOne )
					{
						break;
					}
				}
			}

			if( bFindMatchOne==FALSE )
			{
				UCHAR ucPickGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
				if( ucPickGrade!=ucNullBin )
				{
					for(int j=0; j<aulSelGradeList.GetSize(); j++)
					{
						if( ucPickGrade==aulSelGradeList.GetAt(j) )
						{
							ulMapLostTotal++;
							break;
						}
					}
				}
			}
		}
	}
	szMsg.Format("WPR: rescan TRY 1 scan found %lu", ulScanTotal);
	SaveScanTimeEvent(szMsg);

	//	continue to loop find missing die if over 100
	if( ulMapLostTotal>100 && ulMapLostTotal<=10000 )
	{
		ulMapLostTotal = 0;
		for (ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
		{
			for (ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
			{
				LONG lOldWftX = 0, lOldWftY = 0;
				if( GetRescanLostData(ulRow, ulCol, lOldWftX, lOldWftY)==FALSE )
				{
					continue;
				}
	
				BOOL bFindMatchOne = FALSE;
				for (UCHAR ucGrabRow = (UCHAR)m_lScanFrameMinRow; ucGrabRow <= (UCHAR)m_lScanFrameMaxRow; ucGrabRow++)
				{
					for (UCHAR ucGrabCol = (UCHAR)m_lScanFrameMinCol; ucGrabCol <= (UCHAR)m_lScanFrameMaxCol; ucGrabCol++)
					{
						for (ulIndex = 1; ulIndex <= ulGrabFrameTotal[ucGrabRow][ucGrabCol]; ulIndex++)
						{
							LONG lNewWftX = 0, lNewWftY = 0;
							LONG lIndex = 0;
							if( GetRescanRemainPoint(ucGrabRow, ucGrabCol, ulIndex, lIndex, lNewWftX, lNewWftY)==false )
							{
								continue;
							}
	
							if (labs(lOldWftX - lNewWftX) <= (LONG)ulDieLostTolX && labs(lOldWftY - lNewWftY) <= (LONG)ulDieLostTolY )
							{
								if (CheckRepeatDieInMatrix3X3(ulRow, ulCol, lNewWftX, lNewWftY, ulDieSameTolX, ulDieSameTolY))
								{
									continue;
								}

								ulScanTotal++;	//	by looping find in remain raw die list
								CutRescanRemainPoint(ucGrabRow, ucGrabCol, ulIndex);
								SetScanInfo(ulRow, ulCol, lNewWftX, lNewWftY, 0.0, "0", 0);
								if ((nOutErr == 0) && (fpOut != NULL))
								{
									LONG lUserRow = 0, lUserCol = 0;
									ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
									fprintf(fpOut, "full,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n", 
										ucGrabRow, ucGrabCol, ulRow, ulCol, lUserRow, lUserCol, lNewWftX, lNewWftY, lOldWftX, lOldWftY);
								}
								CutRescanLostData(ulRow, ulCol);
								bFindMatchOne = TRUE;
								break;	//	find match, break
							}
						}	//	loop find in remain raw data die list
						if( bFindMatchOne )
						{
							break;
						}
					}
					if( bFindMatchOne )
					{
						break;
					}
				}
	
				if( bFindMatchOne==FALSE )
				{
					UCHAR ucPickGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
					if( ucPickGrade!=ucNullBin )
					{
						for(int j=0; j<aulSelGradeList.GetSize(); j++)
						{
							if( ucPickGrade==aulSelGradeList.GetAt(j) )
							{
								ulMapLostTotal++;
								break;
							}
						}
					}
				}
			}	
		}
	}
	szMsg.Format("WPR: rescan TRY 2 scan found %lu", ulScanTotal);
	SaveScanTimeEvent(szMsg);

//	NewAddedSanAnXA
	if( ulMapLostTotal>10 )
	{
		LONG lScanFovRow = GetMapIndexStepRow();
		LONG lScanFovCol = GetMapIndexStepCol();
		INT jPtns = 0;
		CDWordArray aLostRow, aLostCol;
		aLostRow.RemoveAll();
		aLostCol.RemoveAll();
		for (ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
		{
			for (ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
			{
				LONG lOldWftX = 0, lOldWftY = 0;
				if( GetRescanLostData(ulRow, ulCol, lOldWftX, lOldWftY) )
				{
					aLostRow.Add(ulRow);
					aLostCol.Add(ulCol);
					if ((nOutErr == 0) && (fpOut != NULL))
					{
						fprintf(fpOut, "rescan grab %d,%d,%d,%d\n", ulRow, ulCol, lOldWftX, lOldWftY);
					}
				}
			}
		}

//		#define	MAX_PTNS		500
		SCAN_ALIGN_HOLE_LIST	aRescanPtnList[MAX_PTNS];
		for(INT i = 0; i < MAX_PTNS; i++)
		{
			aRescanPtnList[i].m_awHoleRow.RemoveAll();
			aRescanPtnList[i].m_awHoleCol.RemoveAll();
		}
		//find the hole pattern and continue.
		//find the hole of the pattern with a FOV.
		for(INT i=0; i<aLostRow.GetSize(); i++)
		{
			ulRow = aLostRow.GetAt(i);
			ulCol = aLostCol.GetAt(i);
			BOOL bFindOne = FALSE;
			for(INT j=0; j<jPtns; j++)
			{
				if( labs(ulRow-aRescanPtnList[j].m_awHoleRow.GetAt(0))<lScanFovRow &&
					labs(ulCol-aRescanPtnList[j].m_awHoleCol.GetAt(0))<lScanFovCol )
				{
					aRescanPtnList[j].m_awHoleRow.Add(ulRow);
					aRescanPtnList[j].m_awHoleCol.Add(ulCol);
					bFindOne = TRUE;
					break;
				}
			}
			if( bFindOne==FALSE )
			{
				aRescanPtnList[jPtns].m_awHoleRow.Add(ulRow);
				aRescanPtnList[jPtns].m_awHoleCol.Add(ulCol);
				jPtns++;
				if( jPtns>=MAX_PTNS )
				{
					break;
				}
			}
		}

		//	move to old and use DEB, get the target, do FOV search, find all die.
		DelRescanRetry();
		LONG lDiePitchX_X	= GetDiePitchX_X();
		LONG lDiePitchX_Y	= GetDiePitchX_Y();
		LONG lDiePitchY_Y	= GetDiePitchY_Y();
		LONG lDiePitchY_X	= GetDiePitchY_X();
		
		for(INT j=0; j<jPtns; j++)
		{
			LONG lBaseRow = aRescanPtnList[j].m_awHoleRow.GetAt(0);
			LONG lBaseCol = aRescanPtnList[j].m_awHoleCol.GetAt(0);
			LONG lMinEdgeRow = lBaseRow;
			LONG lMaxEdgeRow = lBaseRow;
			LONG lMinEdgeCol = lBaseCol;
			LONG lMaxEdgeCol = lBaseCol;
			LONG lBaseWfX = 0, lBaseWfY = 0;
			if ((nOutErr == 0) && (fpOut != NULL))
			{
				fprintf(fpOut, "Pattern %d, base %d,%d\n", j, lBaseRow, lBaseCol);
			}
			if( GetRescanLostData(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY)==FALSE )
			{
				continue;
			}
			if( aRescanPtnList[j].m_awHoleCol.GetSize()<=1 )
			{
				continue;
			}
			for(INT i = 0; i < aRescanPtnList[j].m_awHoleCol.GetSize(); i++)
			{
				ulRow = aRescanPtnList[j].m_awHoleRow.GetAt(i);
				ulCol = aRescanPtnList[j].m_awHoleCol.GetAt(i);
				lMinEdgeRow = min(lMinEdgeRow, (LONG)ulRow);
				lMaxEdgeRow = max(lMaxEdgeRow, (LONG)ulRow);
				lMinEdgeCol = min(lMinEdgeCol, (LONG)ulCol);
				lMaxEdgeCol = max(lMaxEdgeCol, (LONG)ulCol);
				if ((nOutErr == 0) && (fpOut != NULL))
				{
					fprintf(fpOut, "%d,%d;", ulRow, ulCol);
				}
			}

			LONG lCtrRow = lMinEdgeRow + lScanFovRow/2;
			LONG lCtrCol = lMinEdgeCol + lScanFovCol/2;
			LONG lDiffX = lCtrCol - lBaseCol;
			LONG lDiffY = lCtrRow - lBaseRow;
			LONG lWfX = lBaseWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
			LONG lWfY = lBaseWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
			MoveWaferTable(lWfX, lWfY);
			Sleep(200);
			(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lWfX;
			(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lWfY;

			if ((nOutErr == 0) && (fpOut != NULL))
			{
				fprintf(fpOut, "\n");
				fprintf(fpOut, "move and grab %d,%d,%d,%d\n", lCtrRow, lCtrCol, lWfX, lWfY);
			}
			RescanAutoMultiSearch5M();
			if( (lMinEdgeRow+lScanFovRow)>lMaxEdgeRow )
			{
				if( (lMinEdgeCol+lScanFovCol)<=lMaxEdgeCol )
				{
					lCtrCol = lMaxEdgeCol - lScanFovCol/2;
					LONG lDiffX = lCtrCol - lBaseCol;
					LONG lDiffY = lCtrRow - lBaseRow;
					LONG lWfX = lBaseWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
					LONG lWfY = lBaseWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
					MoveWaferTable(lWfX, lWfY);
					Sleep(200);
					(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lWfX;
					(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lWfY;
					if ((nOutErr == 0) && (fpOut != NULL))
					{
						fprintf(fpOut, "\n");
						fprintf(fpOut, "move and grab %d,%d,%d,%d\n", lCtrRow, lCtrCol, lWfX, lWfY);
					}
					RescanAutoMultiSearch5M();
				}
			}
			else
			{
				lCtrRow = lMaxEdgeRow - lScanFovRow/2;
				LONG lDiffX = lCtrCol - lBaseCol;
				LONG lDiffY = lCtrRow - lBaseRow;
				LONG lWfX = lBaseWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
				LONG lWfY = lBaseWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
				MoveWaferTable(lWfX, lWfY);
				Sleep(200);
				(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lWfX;
				(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lWfY;
				if ((nOutErr == 0) && (fpOut != NULL))
				{
					fprintf(fpOut, "\n");
					fprintf(fpOut, "move and grab %d,%d,%d,%d\n", lCtrRow, lCtrCol, lWfX, lWfY);
				}
				RescanAutoMultiSearch5M();

				if( (lMinEdgeCol+lScanFovCol)<=lMaxEdgeCol )
				{
					lCtrCol = lMaxEdgeCol - lScanFovCol/2;
					lCtrRow = lMinEdgeRow + lScanFovRow/2;
					LONG lDiffX = lCtrCol - lBaseCol;
					LONG lDiffY = lCtrRow - lBaseRow;
					LONG lWfX = lBaseWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
					LONG lWfY = lBaseWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
					MoveWaferTable(lWfX, lWfY);
					Sleep(200);
					(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lWfX;
					(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lWfY;
					if ((nOutErr == 0) && (fpOut != NULL))
					{
						fprintf(fpOut, "\n");
						fprintf(fpOut, "move and grab %d,%d,%d,%d\n", lCtrRow, lCtrCol, lWfX, lWfY);
					}
					RescanAutoMultiSearch5M();

					lCtrRow = lMaxEdgeRow - lScanFovRow/2;
					lDiffX = lCtrCol - lBaseCol;
					lDiffY = lCtrRow - lBaseRow;
					lWfX = lBaseWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
					lWfY = lBaseWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
					MoveWaferTable(lWfX, lWfY);
					Sleep(200);
					(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lWfX;
					(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lWfY;
					if ((nOutErr == 0) && (fpOut != NULL))
					{
						fprintf(fpOut, "\n");
						fprintf(fpOut, "move and grab %d,%d,%d,%d\n", lCtrRow, lCtrCol, lWfX, lWfY);
					}
					RescanAutoMultiSearch5M();
				}
			}
		}

		for(ULONG ulIndex=1; ulIndex<=GetRescanRetryTotal(); ulIndex++)
		{
			LONG lNewWftX = 0, lNewWftY = 0;
			if( GetRescanRetry(ulIndex, lNewWftX, lNewWftY) )
			{
				if ((nOutErr == 0) && (fpOut != NULL))
				{
					fprintf(fpOut, "grab die list %d,%d\n", lNewWftX, lNewWftY);
				}
			}
		}

		//	match to save.
		for(INT i = 0; i < aLostRow.GetSize(); i++)
		{
			ulRow = aLostRow.GetAt(i);
			ulCol = aLostCol.GetAt(i);
			LONG lOldWftX = 0, lOldWftY = 0;
			if( GetRescanLostData(ulRow, ulCol, lOldWftX, lOldWftY)==FALSE )
			{	//	old map position but missing after scan builder.
				continue;
			}

			// find in grab die list, match, assign and break
			for(ULONG ulIndex=1; ulIndex<=GetRescanRetryTotal(); ulIndex++)
			{
				LONG lNewWftX = 0, lNewWftY = 0;
				if( GetRescanRetry(ulIndex, lNewWftX, lNewWftY)==FALSE )
				{
					continue;
				}
				if (labs(lOldWftX - lNewWftX) <= (LONG)ulDieLostTolX && labs(lOldWftY - lNewWftY) <= (LONG)ulDieLostTolY)
				{
					if (CheckRepeatDieInMatrix3X3(ulRow, ulCol, lNewWftX, lNewWftY, ulDieSameTolX, ulDieSameTolY))
					{
						continue;
					}
					ulScanTotal++;	//	by checking in grab FOV
					if( ulMapLostTotal>0 )
						ulMapLostTotal--;
					SetScanInfo(ulRow, ulCol, lNewWftX, lNewWftY, 0.0, "0", 0);
					CutRescanLostData(ulRow, ulCol);
					if ((nOutErr == 0) && (fpOut != NULL))
					{
						fprintf(fpOut, "lost die %d,%d,%d,%d ==> %d,%d\n",
							ulRow, ulCol, lOldWftX, lOldWftY, lNewWftX, lNewWftY);
					}
					break;
				}
			}
		}
		aLostRow.RemoveAll();
		aLostCol.RemoveAll();
		DelRescanRetry();
	}
//	NewAddedSanAnXA

	if ((nOutErr == 0) && (fpOut != NULL))
	{
		fprintf(fpOut, "\n");
	}
	for (ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
	{
		if( ulMapLostTotal==0 )
		{
			break;
		}
		for (ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
		{
			LONG lOldWftX = 0, lOldWftY = 0;
			if( GetRescanLostData(ulRow, ulCol, lOldWftX, lOldWftY) )
			{
				if ((nOutErr == 0) && (fpOut != NULL))
				{
					LONG lUserRow = 0, lUserCol = 0;
					ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
					fprintf(fpOut, "lost,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld\n", 
						-1, -1, ulRow, ulCol, lUserRow, lUserCol, lOldWftX, lOldWftY);
				}
			}
		}
	}
	delete [] pbGetOut;
//	RescanGetRemain		rematch done

	SaveScanTimeEvent("WPR: rescan update scan info complete, remove phy map builder");

	m_cPIMap.RemoveAll();

	LONG lLastRow = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"];
	LONG lLastCol = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"];
	if( GetPrescanPosition(lLastRow, lLastCol, lEncX, lEncY)==FALSE )
	{
		LONG lOldWfX = 0, lOldWfY = 0;
		if( GetMapPhyPosn(lLastRow, lLastCol, lOldWfX, lOldWfY) )
		{
			ulScanTotal++;	//	for last moved die before rescan.
			if( IsAutoRescanAndDEB() )
			{
				DEB_GetDiePosnOnly(lLastRow, lLastCol, lOldWfX, lOldWfY);
			}
			SetScanInfo(lLastRow, lLastCol, lOldWfX, lOldWfY, 0.0, "0", 0);
		}
	}

	//	reverse to refill in prescan position from map physical
	if( m_bAutoRefillRescan )	//	Fast Rescan
	{
		DelRescanRefillData();		//	to get missing die position by scanned surrounding die 
		for (ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
		{
			for (ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
			{
				UCHAR ucCurGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
				if( ucCurGrade==ucNullBin )
				{
					continue;
				}

				if( m_bPrescanEmptyToNullBin==FALSE && m_bPrescanEmptyAction && ucCurGrade==ucEmptyGrade )
				{
					continue;
				}

				if( IsOutMS90SortingPart(ulRow, ulCol) )
				{
					continue;
				}

				if( IsNoDieGrade(ulRow, ulCol) )
				{
					continue;
				}

				LONG lOldWftX = 0, lOldWftY = 0;
				if( GetRescanLostData(ulRow, ulCol, lOldWftX, lOldWftY)==FALSE )
				{
					continue;
				}
				if( GetPrescanPosition(ulRow, ulCol, lEncX, lEncY) )
				{
					continue;
				}
				//BOOL bIsRefer = m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) && ucReferGrade==ucCurGrade;
				//BOOL bIsSpRef = IsSpecialReferGrade(ulRow, ulCol);
				//if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_DIMREFER )
				//	bIsRefer = TRUE;
				//if( bIsRefer || bIsSpRef )
				//{
				//	continue;
				//}

				//	Get Valid Die Position from nearby scanned result
				if( GetDieValidPrescanPosn(ulRow, ulCol, 1, lEncX, lEncY, FALSE) )
				{
					if (labs(lOldWftX - lEncX) < (LONG)ulDieLostTolX && labs(lOldWftY - lEncY) < (LONG)ulDieLostTolY )
					{	//	check with map position, within limit
						SetRescanRefillData(ulRow, ulCol, lEncX, lEncY);
					}
				}
			}
		}

		for (ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
		{
			for (ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
			{
				if( GetPrescanPosition(ulRow, ulCol, lEncX, lEncY)==FALSE )
				{
					LONG lNewX = 0, lNewY = 0;
					BOOL bInfo = GetRescanRefillData(ulRow, ulCol, lNewX, lNewY);
					if( bInfo )
					{
						SetPrescanPosition(ulRow, ulCol, lNewX, lNewY);
					}
				}
			}
		}
		DelRescanRefillData();	//	to get missing die position by scanned surrounding die 
	}	//	Fast Rescan

	//	Begin to log the new rescan position compare with previous one, with first prescan.
	if( pUtl->GetPrescanDebug() )
	{
		CString szFile;
		szFile.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), RESCAN_OFFSET_FILE);
		FILE *fpOffset = NULL;
		errno_t nOffsetErr = fopen_s(&fpOffset, szFile, "w");
		szFile.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), RESCAN_MYLAR_SHIFT_FILE);
		FILE *fpMShift = NULL;
		errno_t nMShiftErr = fopen_s(&fpMShift, szFile, "w");
		CTime ctDateTime = CTime::GetCurrentTime();
		CString szTime = ctDateTime.Format("%Y%m%d%H%M%S");
		if ((nOffsetErr == 0) && (fpOffset != NULL))
		{
			fprintf(fpOffset, "%s\n", szTime);
		}

		if ((nOffsetErr == 0) && (fpMShift != NULL))
		{
			fprintf(fpMShift, "%s\n", szTime);
		}

		for (ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
		{
			for (ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
			{
				LONG lNewX = 0, lNewY = 0, lOldX = 0, lOldY = 0;
				LONG lUserRow = 0 , lUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				if( GetMapPhyPosn(ulRow, ulCol, lOldX, lOldY) )
				{
					LONG lOffsetX = 0, lOffsetY = 0, lDirX = 0, lDirY = 0;
					BOOL bOK = FALSE;
					if( GetPrescanPosition(ulRow, ulCol, lNewX, lNewY) )
					{
						bOK = TRUE;
					}
				//	else if( GetDieValidPrescanPosn(ulRow, ulCol, 1, lNewX, lNewY, FALSE) )
					{
				//		bOK = TRUE;
					}

					if (bOK && (nOffsetErr == 0) && fpOffset != NULL)
					{
						lOffsetX = lNewX - lOldX;
						lOffsetY = lNewY - lOldY;
						if( lNewX>lOldX )
							lDirX = 1;
						if( lNewX<lOldX )
							lDirX = -1;
						if( lNewY>lOldY )
							lDirY = 1;
						if( lNewY<lOldY )
							lDirY = -1;
						fprintf(fpOffset, "%3d,%3d,%3d,%3d,%8d,%8d,%2d,%2d\n", ulRow, ulCol, lUserRow, lUserCol, lOffsetX, lOffsetY, lDirX, lDirY);
						if (labs(lOffsetX) > (LONG)ulDieLostTolX || labs(lOffsetY) > (LONG)ulDieLostTolY )
						{
							CString szMsg;
							szMsg.Format("rescan shift large %3d,%3d,%3d,%3d,%8d,%8d,%2d,%2d", ulRow, ulCol, lUserRow, lUserCol, lOffsetX, lOffsetY, lDirX, lDirY);
							SaveScanTimeEvent(szMsg);
						}
					}
				}

				if( GetPrescanRunPosn(ulRow, ulCol, lOldX, lOldY) )
				{
					BOOL bOK = FALSE;
					LONG lOffsetX = 0, lOffsetY = 0, lDirX = 0, lDirY = 0;
					if( GetPrescanPosition(ulRow, ulCol, lNewX, lNewY) )
					{
						bOK = TRUE;
					}
					if( bOK && (nMShiftErr == 0) && fpMShift!=NULL )
					{
						lOffsetX = lNewX - lOldX;
						lOffsetY = lNewY - lOldY;
						if( lNewX>lOldX )
							lDirX = 1;
						if( lNewX<lOldX )
							lDirX = -1;
						if( lNewY>lOldY )
							lDirY = 1;
						if( lNewY<lOldY )
							lDirY = -1;
						fprintf(fpMShift, "%3d,%3d,%3d,%3d,%8d,%8d,%2d,%2d\n", ulRow, ulCol, lUserRow, lUserCol, lOffsetX, lOffsetY, lDirX, lDirY);
					}
				}
			}
		}
		if ((nMShiftErr == 0) && (fpMShift != NULL))
		{
			fclose(fpMShift);
		}

		if ((nOffsetErr == 0) && (fpOffset != NULL))
		{
			fclose(fpOffset);
		}
	}
	//	Begin to log the new rescan position compare with previous one, with first prescan.

	//	to get the offset between original prescan and current Fast Rescan.
	//	to get the offset between previous/updated and Current Fast Rescan.
	if( pUtl->GetPrescanDebug() )
	{
	#define	RESCAN_SHIFT_P_C				"_Rsn_ShiftPrevCurr.txt"	//	Previous to Current Position Shift
	#define	RESCAN_SHIFT_O_C				"_Rsn_ShiftPscnCurr.txt"	//	Prescan  to Current Position Shift
		CString szLogNamePC, szLogNameOC;
		FILE *fpLogPC = NULL, *fpLogOC = NULL;
		szLogNamePC.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), RESCAN_SHIFT_P_C);
		szLogNameOC.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), RESCAN_SHIFT_O_C);
		errno_t nLogPCErr = fopen_s(&fpLogPC, szLogNamePC, "w");
		errno_t nLogOCErr = fopen_s(&fpLogOC, szLogNameOC, "w");
		for (ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
		{
			for (ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
			{
				LONG lEndX = 0, lEncY = 0, lPhyX = 0, lPhyY = 0, lOldX = 0, lOldY = 0;
				if( GetPrescanPosition(ulRow, ulCol, lEncX, lEncY)==FALSE )
				{
					continue;
				}

				if( GetPrescanRunPosn(ulRow, ulCol, lOldX, lOldY)==FALSE )
				{
					continue;
				}
				if ((nLogOCErr == 0) && (fpLogOC != NULL))
				{
					LONG lDiffX = lEncX - lOldX;
					LONG lDiffY = lEncY - lOldY;
					fprintf(fpLogOC, "%4ld,%4ld,%5ld,%5ld\n", ulRow, ulCol, lDiffX, lDiffY);
				}

				if( GetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY)==FALSE )
				{
					continue;
				}
				if ((nLogPCErr == 0) && (fpLogPC != NULL))
				{
					LONG lDiffX = lEncX - lPhyX;
					LONG lDiffY = lEncY - lPhyY;
					LONG lOKX = labs(lDiffX) < (LONG)ulDieSizeXTol;
					LONG lOKY = labs(lDiffY) < (LONG)ulDieSizeXTol;
					fprintf(fpLogPC, "%4ld,%4ld,%5ld,%5ld,%2ld,%2ld\n", ulRow, ulCol, lDiffX, lDiffY, lOKX, lOKY);
				}
			}
		}

		if ((nLogPCErr == 0) && (fpLogPC != NULL))
		{
			fclose(fpLogPC);
		}
		if ((nLogOCErr == 0) && (fpLogOC != NULL))
		{
			fclose(fpLogOC);
		}
	}

	//	RescanGetRemain		reset map phy posn
	SaveScanTimeEvent("WPR: rescan to reset map phy posn");
	ResetMapPhyPosn();

	SaveScanTimeEvent("WPR: rescan to update wafer map");

	for (ulRow = (ULONG)lEdgeMinRow; ulRow <= (ULONG)lEdgeMaxRow; ulRow++)
	{
		for (ulCol = (ULONG)lEdgeMinCol; ulCol <= (ULONG)lEdgeMaxCol; ulCol++)
		{
			UCHAR ucCurGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucCurGrade==ucNullBin )
			{
				continue;
			}

			if( m_bPrescanEmptyToNullBin==FALSE && m_bPrescanEmptyAction && ucCurGrade==ucEmptyGrade )
			{
				continue;
			}

			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}

			BOOL bIsRefer = m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) && ucReferGrade==ucCurGrade;
			BOOL bIsSpRef = IsSpecialReferGrade(ulRow, ulCol);
			BOOL bIsNoDie = IsNoDieGrade(ulRow, ulCol);
			if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_DIMREFER )
				bIsRefer = TRUE;

			BOOL bInfo = GetPrescanPosition(ulRow, ulCol, lEncX, lEncY);
			if( bInfo )
			{
				m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
			}
		
			if( bIsNoDie )
			{
				continue;
			}

			if( bIsRefer || bIsSpRef )
			{
				continue;
			}

			if( bInfo )
			{
				continue;
			}

			// set to empty grade
			BOOL bScoreCount = TRUE;
			if( m_bPrescanSkipNgGrade && m_ucScanMapStartNgGrade==0 && ucCurGrade==ucOffset )
			{
				bScoreCount = FALSE;
			}
			if( m_WaferMapWrapper.GetReader()!=NULL && m_WaferMapWrapper.GetReader()->IsInvalidDie(ulRow, ulCol) )
			{
				bScoreCount = FALSE;
			}
			if( bScoreCount )
			{
				m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
				//WriteUnpickInfo(WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY,ulRow,ulCol);
			}

			if( bShortPathNgDieToInvalid )
			{
				m_WaferMapWrapper.SetMissingDie(ulRow, ulCol);
				if( m_bPrescanEmptyToNullBin )
					m_WaferMapWrapper.HideDie(ulRow, ulCol);
			}	//	ng to invalid
			else	//	RescanGetRemain	empty die
			{
				if( bEmptyAction )
				{
					if( IsScanMapNgGrade(ucCurGrade-ucOffset) )
					{
					}
					else
					{
						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
					}
				}
				else
				{
					if( m_bPrescanEmptyMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						UpdateDie(ulRow, ulCol, ucCurGrade, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
						//WriteUnpickInfo(WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY,ulRow,ulCol);
					}
				}
			}	//		RescanGetRemain	empty die
		}
	}

	m_WaferMapWrapper.BatchPhysicalPositionUpdate();

	m_lRescanMissingTotal += ulMapLostTotal;

	szMsg.Format("WPR: rescan at %ld, map total %lu, remain %lu, missing %lu, at last scan found %lu, remain raw %lu",
		GetNewPickCount(), ulMapTotal, ulRemainMapTotal, ulMapLostTotal, ulScanTotal, ulRemainRawTotal);
	if ((nOutErr == 0) && (fpOut != NULL) && pUtl->GetPrescanDebug())
	{
		fprintf(fpOut, "%s\n", szMsg);
	}
	if ((nOutErr == 0) && (fpOut != NULL))
	{
		fclose(fpOut);
	}
	SaveScanTimeEvent(szMsg);

	ULONG ulMapLostLimit = (100-95) * ulMapTotal/100;
	if( ulMapLostLimit>2000 )
	{
		ulMapLostLimit = 2000;
	}
	if( m_dRescanGoodRatio>0.0 && m_dRescanGoodRatio<=100.0 )
	{
		ulMapLostLimit = (ULONG)((100.0 - m_dRescanGoodRatio) * (DOUBLE)ulMapTotal/100.0);
	}
	if( ulMapLostLimit<5 )
	{
		ulMapLostLimit = 5;
	}
	if( ulMapLostTotal>ulMapLostLimit )
	{
		szMsg.Format("rescan die loss %lu is large, please help to check.", ulMapLostTotal);
		HmiMessage_Red_Back(szMsg);
		SetErrorMessage(szMsg);
		szMsg.Format("rescan die loss %lu is large, map %lu, ratio %f, limit %lu.",
			ulMapLostTotal, ulMapTotal, m_dRescanGoodRatio, ulMapLostLimit);
		SaveScanTimeEvent(szMsg);
		SetErrorMessage(szMsg);
		if( m_dRescanGoodRatio>0.0 && m_dRescanGoodRatio<100.0 )
		{
			bReturn = FALSE;
		}
	}

	szMsg.Format("%s,%s,rescan,%ld,%ld,%ld,%ld,", GetMachineNo(), GetMapNameOnly(),
			m_lPrescanSortingTotal, ulMapTotal, ulMapLostTotal, m_lRescanMissingTotal);
	CString szFileName = gszUSER_DIR_MAPFILE_OUTPUT + "\\" + GetMachineNo() + "_RescanResult.csv";
	CMSLogFileUtility::Instance()->AppendLogWithTime(szFileName, szMsg);

	UINT nPerRatio = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Rescan Total Loss Ration in 10000"), 0);
	ulMapLostLimit = m_lPrescanSortingTotal*nPerRatio/10000;
	if (nPerRatio >= 10 && m_lRescanMissingTotal > (LONG)ulMapLostLimit )
	{
		szMsg = "rescan die total loss is large, please help to check.";
		HmiMessage_Red_Back(szMsg);
		SetErrorMessage(szMsg);
		szMsg.Format("rescan die total loss %lu is large, map %ld, ratio %f, limit %lu.",
			m_lRescanMissingTotal, m_lPrescanSortingTotal, 0.15, ulMapLostLimit);
		SaveScanTimeEvent(szMsg);
		SetErrorMessage(szMsg);
		bReturn = FALSE;
		m_lRescanMissingTotal = 0;
		szMsg.Format("%s,%s,reset,%ld,%ld,%ld,%ld,", GetMachineNo(), GetMapNameOnly(),
				m_lPrescanSortingTotal, ulMapTotal, ulMapLostTotal, m_lRescanMissingTotal);
		CString szFileName = gszUSER_DIR_MAPFILE_OUTPUT + "\\" + GetMachineNo() + "_RescanResult.csv";
		CMSLogFileUtility::Instance()->AppendLogWithTime(szFileName, szMsg);
	}	//	rescan yield

	// build prescan map complete
	ClearWSPrescanInfo();
	DelRescanLostData();	DelRescanRemainPoint();	//	rescan construct map done.
	m_lRescanRunStage		= 3;	//	rescan construct map
	m_dwaRsnBaseRow.RemoveAll();
	m_dwaRsnBaseCol.RemoveAll();
	m_dwaRsnBaseWfX.RemoveAll();
	m_dwaRsnBaseWfY.RemoveAll();

	return bReturn;
}	// rescan to contruct map

//	rescan need to remove.
LONG CWaferPr::ReScanRunTimeDisplayMapIndexForMS(LONG ulGrabRow, LONG ulGrabCol, INT nOffsetX, INT nOffsetY, LONG nDieWfX, LONG nDieWfY, bool bIsBadCut, bool bIsDefect)
{
	LONG lDiePitchX_X = GetDiePitchX_X();
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();

	if( IsAutoRescanWafer() )
	{
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		ULONG lBasePoints = 0;
		if( GetRescanFrameBaseNum(ulGrabRow, ulGrabCol, lBasePoints)==false )	//	get frame's row and column and find the assistant point and update and remove.
		{
			return 0;
		}
		if( lBasePoints==0 )
		{
			return 0;
		}

		LONG lToleranceX	= labs((LONG)(RESCAN_LOOP_FIND_LOST_TOL) * lDiePitchX_X/100);	//	RESCAN_MAX_PITCH_TOL
		LONG lToleranceY	= labs((LONG)(RESCAN_LOOP_FIND_LOST_TOL) * lDiePitchY_Y/100);	//	RESCAN_MAX_PITCH_TOL

		for(UINT ulIndex=0; ulIndex<lBasePoints; ulIndex++)
		{
			LONG lBaseRow = 0, lBaseCol = 0, lBaseWfX = 0, lBaseWfY = 0;
			if( GetFrameRescanBasePoint((UCHAR)ulGrabRow, (UCHAR)ulGrabCol, ulIndex, lBaseRow, lBaseCol, lBaseWfX, lBaseWfY) )	// check
			{
				//	auto rescan sorting wafer	loop to find in map and update
				if( labs(lBaseWfX-nDieWfX)>lToleranceX || labs(lBaseWfY-nDieWfY)>lToleranceY )
				{
					continue;
				}
				m_dwaRsnBaseRow.Add(lBaseRow);
				m_dwaRsnBaseCol.Add(lBaseCol);
				m_dwaRsnBaseWfX.Add(nDieWfX);
				m_dwaRsnBaseWfY.Add(nDieWfY);
				CutFrameRescanBasePoint((UCHAR)ulGrabRow, (UCHAR)ulGrabCol, ulIndex);	//	found, remove it
				break;
			}
		}

		return 0;
	}	// 0 locate fail, 1 locate ok

	if( !IsPrescanMapIndex() && !m_bScanRunTimeDisplay )
	{
		return 0;
	}

	BOOL bLocateDie = FALSE;
	ULONG ulFindRow = 0, ulFindCol = 0;

	LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
	if( GetMapIndexStepCol()%2==0 )
		lGrabOffsetX = lDiePitchX_X/2;
	if( GetMapIndexStepRow()%2==0 )
		lGrabOffsetY = lDiePitchY_Y/2;
	LONG lFindDieX = nOffsetX+lGrabOffsetX, lLoopDieX = 0;
	LONG lFindDieY = nOffsetY+lGrabOffsetY, lLoopDieY = 0;

	LONG lDieOffsetX	= 0;
	LONG lDieOffsetY	= 0;
	LONG lMapLoopRow	= 0;
	LONG lMapLoopCol	= 0;

	if( IsPrescanMapIndex() )
	{
		lMapLoopRow	= (GetMapIndexStepRow()+1)/2;
		lMapLoopCol	= (GetMapIndexStepCol()+1)/2;
	}
	else
	{
		lMapLoopRow	= (LONG)(m_dPrescanLFSizeY)/2;
		lMapLoopCol	= (LONG)(m_dPrescanLFSizeX)/2;
	}

	LONG lToleranceX	= _round(fabs((GetDieSizeX() * m_dPrescanPitchTolX) / 100));
	LONG lToleranceY	= _round(fabs((GetDieSizeY() * m_dPrescanPitchTolY) / 100));

	for (LONG iRow = 0 - lMapLoopRow; iRow <= lMapLoopRow; iRow++)
	{
		for (LONG jCol = 0 - lMapLoopCol; jCol <= lMapLoopCol; jCol++)
		{
			if ((iRow + ulGrabRow) >= 0 && (jCol + ulGrabCol) > 0)
			{
				lDieOffsetX = labs(lFindDieX + jCol * lDiePitchX_X + iRow * lDiePitchY_X);
				lDieOffsetY = labs(lFindDieY + iRow * lDiePitchY_Y + jCol * lDiePitchX_Y);
				if( lDieOffsetX<=lToleranceX && lDieOffsetY<=lToleranceY )
				{
					ulFindRow = iRow + ulGrabRow;
					ulFindCol = jCol + ulGrabCol;
					bLocateDie = TRUE;
					break;
				}
			}
			if( bLocateDie )
			{
				break;
			}
		}
		if( bLocateDie )
		{
			break;
		}
	}

	if( bLocateDie )
	{
		SetMapPhysicalPosition(ulFindRow, ulFindCol, nDieWfX, nDieWfY);
		if( m_bScanRunTimeDisplay && ((m_bPrescanBadCutAction && bIsBadCut) || (m_bPrescanDefectAction && bIsDefect)) )
		{
			UCHAR ucCurGrade = m_WaferMapWrapper.GetGrade(ulFindRow, ulFindCol);
			UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
			UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
			UCHAR ucDefectGrade	= GetScanDefectGrade() + ucOffset;
			UCHAR ucBadCutGrade	= GetScanBadcutGrade() + ucOffset;
			if( m_bPrescanDefectToNullBin )
				ucDefectGrade = ucNullBin;
			if (m_bPrescanBadCutToNullBin)
				ucBadCutGrade = ucNullBin;
			if( IsScanMapNgGrade(ucCurGrade-ucOffset)==FALSE && ucCurGrade!=ucNullBin &&
				ucCurGrade!=ucDefectGrade && ucCurGrade!=ucBadCutGrade)
			{
				// for map display change and restore to original after scan finish
				WSAddScanRemainDieIntoList(ulFindCol, ulFindRow, ucCurGrade);
				if( bIsBadCut )
				{
					m_WaferMapWrapper.ChangeGrade(ulFindRow, ulFindCol, ucBadCutGrade);
				}
				if( bIsDefect )
				{
					m_WaferMapWrapper.ChangeGrade(ulFindRow, ulFindCol, ucDefectGrade);
				}
			}
		}
	}

	return bLocateDie;	// 0 normal locate fail, 1 locate ok, 2, rescan sorting locate fail.
}

BOOL CWaferPr::RConvertMotorStepToPixel(LONG lX, LONG lY, PR_COORD &stPixel)
{
	DOUBLE dStepX = lX;
	DOUBLE dStepY = lY;
	stPixel.x = 0;
	stPixel.y = 0;
	double dCalibX = GetScanCalibX();
	double dCalibY = GetScanCalibY();
/*
	DOUBLE dDivid = 0;
	double dCalibXY= GetScanCalibXY();
	double dCalibYX= GetScanCalibYX();

	dDivid = dCalibX*dCalibY - dCalibXY*dCalibYX;

	dPrX = 0;
	dPrY = 0;
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
	}
*/
	if (dCalibX != 0)
	{
		stPixel.x = (PR_WORD)(dStepX / dCalibX);
	}
	if (dCalibY != 0)
	{
		stPixel.y = (PR_WORD)(dStepY / dCalibY);
	}

	return TRUE;
}

LONG CWaferPr::ScanSampleKeyDice(IPC_CServiceMessage &svMsg)
{
	INT nGrabImageLoop = 1;
	svMsg.GetMsg(sizeof(INT), &nGrabImageLoop);

	BOOL bDrawCheckDie = FALSE;
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	szMsg.Format("Scan Sample key die begin for grab %d", nGrabImageLoop);
	pUtl->PrescanMoveLog(szMsg);
	if( bDrawCheckDie )
	{
		m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	}

	PR_UBYTE	ubSenderID	 =  GetScnSenID();
	PR_UBYTE	ubReceiverID =  GetScnRecID();

	PR_GRAB_SHARE_IMAGE_CMD		stGrbCmd;
	PR_InitGrabShareImgCmd(&stGrbCmd);
	stGrbCmd.emPurpose			= GetScnPurpose();
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= m_stMultiSrchCmd.auwRecordID[0];
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_NORMAL;

	PR_GRAB_SHARE_IMAGE_RPY		stGrbRpy;
	PR_GrabShareImgCmd(&stGrbCmd, ubSenderID, ubReceiverID, &stGrbRpy);
	if (PR_COMM_NOERR != stGrbRpy.uwCommunStatus)
	{
		szMsg.Format("Scan Sample grab share image rpy1 fail com %d", stGrbRpy.uwCommunStatus);
		HmiMessage_Red_Back(szMsg);
		pUtl->PrescanMoveLog(szMsg);
		m_lGetSetLogic = 2;
		return 1;
	}

	m_lGetSetLogic = 1;

	PR_GRAB_SHARE_IMAGE_RPY2	stGrbRpy2;
	PR_GrabShareImgRpy(ubSenderID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		szMsg.Format("Scan Sample grab share image rpy2 fail com %d, status %d",
			stGrbRpy2.stStatus.uwCommunStatus, stGrbRpy2.stStatus.uwPRStatus);
		HmiMessage_Red_Back(szMsg);
		pUtl->PrescanMoveLog(szMsg);
		m_lGetSetLogic = 2;
		return 1;
	}

	// now add the image to queue
	PR_ULWORD ulStationID	= stGrbRpy2.ulStationID;
	PR_ULWORD ulImageID		= stGrbRpy2.ulImageID;

	LONG lGrabX = 0, lGrabY = 0, lSrchX = 0, lSrchY = 0;
	if( GetScanKeyDie(0, nGrabImageLoop, lGrabX, lGrabY)==false )
	{
		m_lGetSetLogic = 2;
		return 1;
	}

	szMsg.Format("Scan Sample grab %d at %ld,%ld", nGrabImageLoop, lGrabX, lGrabY);
	pUtl->PrescanMoveLog(szMsg);

	for(LONG lLoop=1; lLoop<100; lLoop++)
	{
		if( GetScanKeyDie(lLoop, nGrabImageLoop, lSrchX, lSrchY)==false )
		{
			break;
		}

		PR_COORD stPixel;
		RConvertMotorStepToPixel(lGrabX-lSrchX, lGrabY - lSrchY, stPixel);

		PR_WORD lULX = (PR_WORD)(GetPrCenterX() + stPixel.x - GetScanNmlSizePixelX() * 2);
		PR_WORD lULY = (PR_WORD)(GetPrCenterY() + stPixel.y - GetScanNmlSizePixelY() * 2);
		PR_WORD lLRX = (PR_WORD)(GetPrCenterX() + stPixel.x + GetScanNmlSizePixelX() * 2);
		PR_WORD lLRY = (PR_WORD)(GetPrCenterY() + stPixel.y + GetScanNmlSizePixelY() * 2);

		lULX = max(GetScanPRWinULX(), lULX);
		lULY = max(GetScanPRWinULY(), lULY);
		lLRX = min(GetScanPRWinLRX(), lLRX);
		lLRY = min(GetScanPRWinLRY(), lLRY);
		PR_WORD lCTX = (lULX + lLRX)/2;
		PR_WORD lCTY = (lULY + lLRY)/2;

		szMsg.Format("Scan Sample %8ld,%8ld ==> %8ld,%8ld, PR pixel %5d,%5d, window(%4d,%4d = %4d,%4d)", 
			lGrabX, lGrabY, lSrchX, lSrchY, stPixel.x, stPixel.y, lULX, lULY, lLRX, lLRY);
		pUtl->PrescanMoveLog(szMsg);

		if( bDrawCheckDie )
		{
			PR_COLOR sColor = PR_COLOR_YELLOW;
			PR_COORD stCorner1;
			PR_COORD stCorner2;
			stCorner1.x = lULX;
			stCorner1.y = lULY;
			stCorner2.x = lLRX;
			stCorner2.y = lLRY;
			DrawRectangleBox(stCorner1, stCorner2, sColor);
		}

		IMG_UWORD		uwCommStatus;
		PR_SRCH_DIE_CMD stInspCmd	= m_stMultiSrchCmd;

		stInspCmd.ulLatchImageID	= ulImageID;
		stInspCmd.uwLatchStationID	= (PR_UWORD) ulStationID;

		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = lULX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = lULY;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = lLRX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = lLRY;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.x		 = lCTX;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.y		 = lCTY;

		stInspCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
		stInspCmd.uwHostMallocMaxNoOfDieInRpy2			= WPR_ONE_SEARCH_MAX_DIE;
		if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy3		= WPR_ONE_SEARCH_MAX_DIE;
		if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy4		= WPR_ONE_SEARCH_MAX_DIE;
		PR_SrchDieCmd(&stInspCmd, ubSenderID, ubReceiverID, &uwCommStatus);
		if ( uwCommStatus != PR_COMM_NOERR )
		{
			szMsg.Format("Scan Sample PR_MultiSrchDieCmd finished with com %u!", uwCommStatus);
			HmiMessage_Red_Back(szMsg);
			pUtl->PrescanMoveLog(szMsg);
			continue;
		}

		PR_SRCH_DIE_RPY1 stInspRpy1;
		PR_SrchDieRpy1(ubSenderID, &stInspRpy1);
		PR_UWORD uwRpy1ComStt = stInspRpy1.uwCommunStatus;
		PR_UWORD uwRpy1Status = stInspRpy1.uwPRStatus;

		if (uwRpy1ComStt != PR_COMM_NOERR || PR_ERROR_STATUS(uwRpy1Status))
		{
			szMsg.Format("Scan Sample PR MultiSrchDieRpy1 finished with status %u, %u!\n",	uwRpy1ComStt, uwRpy1Status);
			HmiMessage_Red_Back(szMsg);
			pUtl->PrescanMoveLog(szMsg);
			continue;
		}

		PR_SRCH_DIE_RPY2	stInspRpy2;
		stInspRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*)malloc (stInspCmd.uwHostMallocMaxNoOfDieInRpy2*sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign == NULL)
		{
			szMsg = "Scan Sample Init stDieAlign null pointer";
			HmiMessage_Red_Back(szMsg);
			pUtl->PrescanMoveLog(szMsg);
			continue; 
		}

		PR_SrchDieRpy2(ubSenderID, &stInspRpy2);
		PR_UWORD uwRpy2ComStt = stInspRpy2.stStatus.uwCommunStatus;
		if (uwRpy2ComStt != PR_COMM_NOERR)
		{
			//Free the reply 2
			if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
			{
				free(stInspRpy2.pstDieAlign);
			}
			szMsg.Format("Scan Sample PR MultiSrchDieRpy2 finished with status %u!\n", uwRpy2ComStt);
			HmiMessage_Red_Back(szMsg);
			pUtl->PrescanMoveLog(szMsg);
			continue;
		}

		PR_UWORD usDieType = stInspRpy2.stStatus.uwPRStatus;

		PR_SRCH_DIE_RPY3	stInspRpy3;
		if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
		{
			stInspRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*)malloc( stInspCmd.uwHostMallocMaxNoOfDieInRpy3*sizeof(PR_DIE_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin==NULL )
			{
				//Free the reply 2
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				szMsg = "Scan Sample Init stDieAlign null pointer";
				HmiMessage_Red_Back(szMsg);
				pUtl->PrescanMoveLog(szMsg);
				continue; 
			}
		}

		PR_SRCH_DIE_RPY4	stInspRpy4;
		if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
		{
			stInspRpy4.pstPostBondInspMin = (PR_POST_INSP_MINIMAL_RPY*)malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy4*sizeof(PR_POST_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin==NULL )
			{
				//Free the reply 2
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				//Free the reply 3
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				szMsg = "Scan Sample Init stDieAlign null pointer";
				HmiMessage_Red_Back(szMsg);
				pUtl->PrescanMoveLog(szMsg);
				continue; 
			}
		}

		if( ((PR_TRUE==m_stMultiSrchCmd.emDefectInsp) || (PR_TRUE==m_stMultiSrchCmd.emPostBondInsp)) &&
			(PR_COMM_NOERR==uwRpy2ComStt) && !PR_ERROR_STATUS(usDieType) )
		{
			PR_SrchDieRpy3(ubSenderID,&stInspRpy3);
			PR_UWORD uwRpy3ComStt = stInspRpy3.stStatus.uwCommunStatus;
			if (uwRpy3ComStt != PR_COMM_NOERR)
			{
				//Free the reply 2
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				//Free the reply 3
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				//Free the reply 4
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
				{
					free(stInspRpy4.pstPostBondInspMin);
				}	// free point for one search
				szMsg.Format("Scan Sample PR MultiSrchDieRpy3 finished with status %u!\n",	uwRpy3ComStt);
				HmiMessage_Red_Back(szMsg);
				pUtl->PrescanMoveLog(szMsg);
				continue;
			}

			PR_UWORD uwRpy3Status = stInspRpy3.stStatus.uwPRStatus;
			// inspection result
			if( (PR_TRUE == m_stMultiSrchCmd.emPostBondInsp) &&
				PR_COMM_NOERR == uwRpy3ComStt && !PR_ERROR_STATUS(uwRpy3Status) )
			{
				PR_SrchDieRpy4(ubSenderID,&stInspRpy4);

				PR_UWORD uwRpy4ComStt = stInspRpy4.stStatus.uwCommunStatus;
				if (uwRpy4ComStt != PR_COMM_NOERR)
				{
					//Free the reply 2
					if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
					{
						free(stInspRpy2.pstDieAlign);
					}
					//Free the reply 3
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
					{
						free(stInspRpy3.pstDieInspExtMin);
					}
					//Free the reply 4
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
					{
						free(stInspRpy4.pstPostBondInspMin);
					}	// free point for one search
					szMsg.Format("Scan Sample PR_MultiSrchDieRpy4 finished with status %u!\n", uwRpy4ComStt);
					HmiMessage_Red_Back(szMsg);
					pUtl->PrescanMoveLog(szMsg);
					continue;
				}
			}
		}

		USHORT usDieSum = stInspRpy2.uwNResults;
		for(USHORT usIndex=0; usIndex<usDieSum; usIndex++)
		{
			CString szDieBin = "0";
			PR_RCOORD	rcDieCtr = stInspRpy2.pstDieAlign[usIndex].rcoDieCentre;
			PR_REAL		dDieRot  = stInspRpy2.pstDieAlign[usIndex].rDieRot;
			if( IsMS90HalfSortMode() )
			{
				if( dDieRot>90 )
					dDieRot -= 180;
				else if( dDieRot<-90 )
					dDieRot += 180;
			}
			PR_UWORD uwRpy3Status = 0, uwRpy4Status = 0;
			if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
			{
				uwRpy3Status = stInspRpy3.pstDieInspExtMin[usIndex].uwPRStatus;
			}
			if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
			{
				uwRpy4Status = stInspRpy4.pstPostBondInspMin[usIndex].uwPRStatus;
			}

			int siStepX = 0, siStepY = 0;
			CalcScanDiePosition(lGrabX, lGrabY, rcDieCtr, siStepX, siStepY);

			siStepX = lGrabX + siStepX;
			siStepY = lGrabY + siStepY;
			SetGoodPosnOnly(siStepX, siStepY, dDieRot, szDieBin, 0, 0);
			szMsg.Format("Scan Sample found die,%8ld,%8ld,%f", siStepX, siStepY, dDieRot);
			pUtl->PrescanMoveLog(szMsg);

			if( bDrawCheckDie )
			{
				PR_COLOR sColor = PR_COLOR_GREEN;
				PR_COORD stCorner1;
				PR_COORD stCorner2;
				if( GetDieShape()==WPR_RHOMBUS_DIE )
				{
					stCorner1.x	= (PR_WORD)rcDieCtr.x - GetScanNmlSizePixelX()/4;
					stCorner1.y	= (PR_WORD)rcDieCtr.y - GetScanNmlSizePixelY()/4;
					stCorner2.x = (PR_WORD)rcDieCtr.x + GetScanNmlSizePixelX()/4;
					stCorner2.y = (PR_WORD)rcDieCtr.y + GetScanNmlSizePixelY()/4;
				}
				else
				{
					stCorner1.x	= (PR_WORD)rcDieCtr.x - GetScanNmlSizePixelX()/2;
					stCorner1.y	= (PR_WORD)rcDieCtr.y - GetScanNmlSizePixelY()/2;
					stCorner2.x = (PR_WORD)rcDieCtr.x + GetScanNmlSizePixelX()/2;
					stCorner2.y = (PR_WORD)rcDieCtr.y + GetScanNmlSizePixelY()/2;
				}
				DrawRectangleBox(stCorner1, stCorner2, sColor);
			}
		}

		//Free the reply 2
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
		{
			free(stInspRpy2.pstDieAlign);
		}
		//Free the reply 3
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
		{
			free(stInspRpy3.pstDieInspExtMin);
		}
		//Free the reply 4
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
		{
			free(stInspRpy4.pstPostBondInspMin);
		}	// free point for one search
	}

	PR_REMOVE_SHARE_IMAGE_CMD     stRmvCmd;
	PR_InitRemoveShareImgCmd(&stRmvCmd);
	stRmvCmd.ulImageID = ulImageID;
	stRmvCmd.emFreeAll = PR_TRUE;

	PR_REMOVE_SHARE_IMAGE_RPY     stRmvRpy;
	PR_RemoveShareImgCmd(&stRmvCmd, ubSenderID, ubReceiverID, &stRmvRpy);

	if (PR_COMM_NOERR != stRmvRpy.uwCommunStatus)
	{
		szMsg.Format("Scan Sample fail com %d to delete the image", stRmvRpy.uwCommunStatus);
		HmiMessage_Red_Back(szMsg);
		pUtl->PrescanMoveLog(szMsg);
	}
	if( bDrawCheckDie )
	{
		Sleep(2000);
	}
	pUtl->PrescanMoveLog("Scan Sample finish and remove image done\n");

	m_lGetSetLogic = 2;
	return 1;
}

LONG CWaferPr::ToggleScanSortZoom(IPC_CServiceMessage &svMsg)
{
	LONG bToScan = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bToScan);
	if( IsEnableZoom() )
	{
		BOOL bGrab = FALSE;
		if ((State() == AUTO_Q) || (State() == DEMO_Q))
		{
			bGrab = TRUE;
		}
		if( bToScan )
		{
			CMSPrescanUtility::Instance()->PrescanMoveLog("Scan Sample Switch to scan zoom\n");
			WPR_ToggleZoom(TRUE, bGrab, 14);
		}
		else
		{
			CMSPrescanUtility::Instance()->PrescanMoveLog("Scan Sample Switch to sort zoom");
			WPR_ToggleZoom(FALSE, bGrab, 15);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferPr::UpdateDie(const ULONG ulRow, const ULONG ulCol, const UCHAR ucGrade,  const ULONG ulDieState)
{
	return m_WaferMapWrapper.Update(ulRow, ulCol, ucGrade, WAF_CDieSelectionAlgorithm::SKIP, FALSE, ulDieState);
}


BOOL CWaferPr::RescanAutoMultiSearch5M()
{
	CString szMsg;
	PR_UBYTE	ubSenderID	 =  GetScnSenID();
	PR_UBYTE	ubReceiverID =  GetScnRecID();

	PR_GRAB_SHARE_IMAGE_CMD		stGrbCmd;
	PR_GRAB_SHARE_IMAGE_RPY		stGrbRpy;
	PR_InitGrabShareImgCmd(&stGrbCmd);
	// AOI partial die step 3 During Prescan, 
	stGrbCmd.emPurpose			= GetScnPurpose();
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= m_stMultiSrchCmd.auwRecordID[0];
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_NORMAL;

	PR_GrabShareImgCmd(&stGrbCmd, ubSenderID, ubReceiverID, &stGrbRpy);
	if (PR_COMM_NOERR != stGrbRpy.uwCommunStatus)
	{
		szMsg.Format("One Search grab share image cmd fail, com %d", stGrbRpy.uwCommunStatus);
	//	HmiMessage(szMsg);
		SaveScanTimeEvent(szMsg);
		return FALSE;
	}

	PR_GRAB_SHARE_IMAGE_RPY2	stGrbRpy2;
	PR_GrabShareImgRpy(ubSenderID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		szMsg.Format("One Search grab share image rpy fail, com %d, sta %d", stGrbRpy2.stStatus.uwCommunStatus, stGrbRpy2.stStatus.uwPRStatus);
	//	HmiMessage(szMsg);
		SaveScanTimeEvent(szMsg);
		return FALSE;
	}

	int nSubImageNum = m_uwFrameSubImageNum;
	// now add the image to queue
	PR_ULWORD ulStationID	= stGrbRpy2.ulStationID;
	PR_ULWORD ulImageID		= stGrbRpy2.ulImageID;

	for(int i = 1; i<=nSubImageNum; i++)
	{
		PR_UWORD	uwRpy1Status, uwRpy1ComStt, uwRpy2ComStt;
		IMG_UWORD	uwCommStatus;
		PR_WORD lULX = 0, lULY = 0, lLRX = 0, lLRY = 0;
		GetSubWindow(i, lULX, lULY, lLRX, lLRY);
		PR_WORD lCTX = (lULX + lLRX)/2;
		PR_WORD lCTY = (lULY + lLRY)/2;

		PR_SRCH_DIE_CMD stInspCmd	= m_stMultiSrchCmd;

		stInspCmd.ulLatchImageID	= ulImageID;
		stInspCmd.uwLatchStationID	= (PR_UWORD) ulStationID;

		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = lULX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = lULY;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = lLRX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = lLRY;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.x		 = lCTX;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.y		 = lCTY;

		stInspCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
		stInspCmd.uwHostMallocMaxNoOfDieInRpy2			= WPR_ONE_SEARCH_MAX_DIE;
		if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy3		= WPR_ONE_SEARCH_MAX_DIE;
		if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy4		= WPR_ONE_SEARCH_MAX_DIE;
		PR_SrchDieCmd(&stInspCmd, ubSenderID, ubReceiverID, &uwCommStatus);

		if ( uwCommStatus != PR_COMM_NOERR )
		{
			CString szMsg;
			szMsg.Format("One Search PR_MultiSrchDieCmd finished with status %u!", uwCommStatus);
		//	HmiMessage(szMsg);
			SaveScanTimeEvent(szMsg);
			return FALSE;
		}

		PR_SRCH_DIE_RPY1 stInspRpy1;
		PR_SrchDieRpy1(ubSenderID, &stInspRpy1);
		uwRpy1ComStt = stInspRpy1.uwCommunStatus;
		uwRpy1Status = stInspRpy1.uwPRStatus;

		if (uwRpy1ComStt != PR_COMM_NOERR || PR_ERROR_STATUS(uwRpy1Status))
		{
			szMsg.Format("One Search PR MultiSrchDieRpy1 finished with status %u, %u!\n",	uwRpy1ComStt, uwRpy1Status);
		//	HmiMessage(szMsg);
			SaveScanTimeEvent(szMsg);
			return FALSE;
		}

		PR_UWORD		usDieType;
		PR_SRCH_DIE_RPY2	stInspRpy2;

		stInspRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*)malloc (stInspCmd.uwHostMallocMaxNoOfDieInRpy2*sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign == NULL)
		{
			szMsg = "One Search Init stDieAlign null pointer";
		//	HmiMessage(szMsg);
			SaveScanTimeEvent(szMsg);
			break; 
		}
		PR_SrchDieRpy2(ubSenderID, &stInspRpy2);
		uwRpy2ComStt = stInspRpy2.stStatus.uwCommunStatus;
		if (uwRpy2ComStt != PR_COMM_NOERR)
		{
			if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
			{
				free(stInspRpy2.pstDieAlign);
			}
			szMsg.Format("One Search PR MultiSrchDieRpy2 finished with status %u!\n", uwRpy2ComStt);
		//	HmiMessage(szMsg);
			SaveScanTimeEvent(szMsg);
			break;
		}
		usDieType = stInspRpy2.stStatus.uwPRStatus;

		PR_SRCH_DIE_RPY3	stInspRpy3;
		PR_SRCH_DIE_RPY4	stInspRpy4;
		if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
		{
			stInspRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*)malloc( stInspCmd.uwHostMallocMaxNoOfDieInRpy3*sizeof(PR_DIE_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin==NULL )
			{
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				szMsg = "One Search Init stDieAlign null pointer";
			//	HmiMessage(szMsg);
				SaveScanTimeEvent(szMsg);
				break; 
			}
		}
		if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
		{
			stInspRpy4.pstPostBondInspMin = (PR_POST_INSP_MINIMAL_RPY*)malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy4*sizeof(PR_POST_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin==NULL )
			{
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				szMsg = "One Search Init stDieAlign null pointer";
			//	HmiMessage(szMsg);
				SaveScanTimeEvent(szMsg);
				break; 
			}
		}

		if( ((PR_TRUE==m_stMultiSrchCmd.emDefectInsp) || (PR_TRUE==m_stMultiSrchCmd.emPostBondInsp)) &&
			(PR_COMM_NOERR==uwRpy2ComStt) && !PR_ERROR_STATUS(usDieType) )
		{
			PR_SrchDieRpy3(ubSenderID,&stInspRpy3);
			PR_UWORD uwRpy3ComStt = stInspRpy3.stStatus.uwCommunStatus;
			if (uwRpy3ComStt != PR_COMM_NOERR)
			{
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
				{
					free(stInspRpy4.pstPostBondInspMin);
				}	// free point for one search
				szMsg.Format("PR MultiSrchDieRpy3 finished with status %u!\n",	uwRpy3ComStt);
			//	HmiMessage(szMsg);
				SaveScanTimeEvent(szMsg);
				break;
			}

			PR_UWORD uwRpy3Status = stInspRpy3.stStatus.uwPRStatus;

			// inspection result
			if( (PR_TRUE == m_stMultiSrchCmd.emPostBondInsp) &&
				PR_COMM_NOERR == uwRpy3ComStt && !PR_ERROR_STATUS(uwRpy3Status) )
			{
				PR_SrchDieRpy4(ubSenderID,&stInspRpy4);

				PR_UWORD uwRpy4ComStt = stInspRpy4.stStatus.uwCommunStatus;
				if (uwRpy4ComStt != PR_COMM_NOERR)
				{
					if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
					{
						free(stInspRpy2.pstDieAlign);
					}
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
					{
						free(stInspRpy3.pstDieInspExtMin);
					}
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
					{
						free(stInspRpy4.pstPostBondInspMin);
					}	// free point for one search
					szMsg.Format("PR_MultiSrchDieRpy4 finished with status %u!\n", uwRpy4ComStt);
				//	HmiMessage(szMsg);
					SaveScanTimeEvent(szMsg);
					break;
				}
			}
		}

		USHORT usDieSum = stInspRpy2.uwNResults;
		for(USHORT usIndex=0; usIndex<usDieSum; usIndex++)
		{
			CString szDieBin = "0";
			PR_RCOORD	rcDieCtr = stInspRpy2.pstDieAlign[usIndex].rcoDieCentre;
			PR_REAL		dDieRot  = stInspRpy2.pstDieAlign[usIndex].rDieRot;
			if( IsMS90HalfSortMode() )
			{
				if( dDieRot>90 )
					dDieRot -= 180;
				else if( dDieRot<-90 )
					dDieRot += 180;
			}
			PR_UWORD uwRpy3Status = 0, uwRpy4Status = 0;
			if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
			{
				uwRpy3Status = stInspRpy3.pstDieInspExtMin[usIndex].uwPRStatus;
			}
			if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
			{
				uwRpy4Status = stInspRpy4.pstPostBondInspMin[usIndex].uwPRStatus;
			}

			LONG lGrabX = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["X"];
			LONG lGrabY = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["Y"];
			int siStepX = 0, siStepY = 0;
			CalcScanDiePosition(lGrabX, lGrabY, rcDieCtr, siStepX, siStepY);
			siStepX = lGrabX + siStepX;
			siStepY = lGrabY + siStepY;
			SetRescanRetry(siStepX, siStepY);
		}

		//Free the reply 2
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
		{
			free(stInspRpy2.pstDieAlign);
		}
		//Free the reply 3
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
		{
			free(stInspRpy3.pstDieInspExtMin);
		}
		//Free the reply 4
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
		{
			free(stInspRpy4.pstPostBondInspMin);
		}	// free point for one search
	}

	PR_REMOVE_SHARE_IMAGE_CMD     stRmvCmd;
	PR_REMOVE_SHARE_IMAGE_RPY     stRmvRpy;
	PR_InitRemoveShareImgCmd(&stRmvCmd);
	stRmvCmd.ulImageID = ulImageID;
	stRmvCmd.emFreeAll = PR_TRUE; // You can set this to PR_TRUE if you want to clear all Share Image for this channel
	PR_RemoveShareImgCmd(&stRmvCmd, ubSenderID, ubReceiverID, &stRmvRpy);

	if (PR_COMM_NOERR != stRmvRpy.uwCommunStatus)
	{
		szMsg = "One Search fail to delete the image";
	//	HmiMessage(szMsg);
		SaveScanTimeEvent(szMsg);
	}

	return TRUE;
}