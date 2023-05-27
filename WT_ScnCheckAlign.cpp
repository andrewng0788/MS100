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
#include "WaferPr.h"
//#include "Encryption.h"

//#include "spswitch.h"			//v4.47T5
//using namespace AsmSw;
static long gslThisMachineHasRptFile = 0;
#define	SINGLE_ALIGN_POINT	1
#define	TABLE_ANGLE_ALL		0
#define	TABLE_ANGLE_NEW		0

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferTable::ConvertScnPosnByFPC(ULONG ulDieRow, ULONG ulDieCol, LONG &lPhyPosX, LONG &lPhyPosY, LONG &lScnIndex, BOOL bUseAngle)
{
	LONG	lDieScanX=0, lDieScanY=0;
	if( GetScnData(ulDieRow, ulDieCol, &lDieScanX, &lDieScanY)==FALSE )
	{
		return FALSE;
	}

	DOUBLE dNearDist = -1;
	LONG lNearIndex = -1;
	for(INT j=0; j<WT_ALN_MAXCHECK_SCN; j++)
	{
		if( m_stScnAlnPoints[j].m_bDieAFound==FALSE )
		{
			continue;
		}
		if( bUseAngle )
		{
			if( m_stScnAlnPoints[j].m_bAngleFound==FALSE )
			{
				continue;
			}
		}
		LONG lScanX = m_stScnAlnPoints[j].m_lDieAScanX;
		LONG lScanY = m_stScnAlnPoints[j].m_lDieAScanY;
		DOUBLE dDist = GetDistance(lDieScanX, lDieScanY, lScanX, lScanY);
		if( dNearDist==-1 || dDist<dNearDist )
		{
			dNearDist = dDist;
			lNearIndex = j;
		}
	}

	if( lNearIndex==-1 )
	{
		return FALSE;
	}
	DOUBLE dScanAngle_X = m_stScnAlnPoints[0].m_dScanAngle_X;
	DOUBLE dScanAngle_Y = m_stScnAlnPoints[0].m_dScanAngle_Y;
	DOUBLE dMsPrAngle_X = m_stScnAlnPoints[0].m_dMsPrAngle_X;
	DOUBLE dMsPrAngle_Y = m_stScnAlnPoints[0].m_dMsPrAngle_Y;

	LONG lScanDieA_X = m_stScnAlnPoints[lNearIndex].m_lDieAScanX;
	LONG lScanDieA_Y = m_stScnAlnPoints[lNearIndex].m_lDieAScanY;
	LONG lMsPrDieA_X = m_stScnAlnPoints[lNearIndex].m_lDieAMsPrX;
	LONG lMsPrDieA_Y = m_stScnAlnPoints[lNearIndex].m_lDieAMsPrY;
	if( bUseAngle )
	{
		dScanAngle_X = m_stScnAlnPoints[lNearIndex].m_dScanAngle_X;
		dScanAngle_Y = m_stScnAlnPoints[lNearIndex].m_dScanAngle_Y;
		dMsPrAngle_X = m_stScnAlnPoints[lNearIndex].m_dMsPrAngle_X;
		dMsPrAngle_Y = m_stScnAlnPoints[lNearIndex].m_dMsPrAngle_Y;
	}

	//Tansform SCN position related to angle	
	DOUBLE	dTemp1X	= (DOUBLE)(lDieScanX - lScanDieA_X);
	DOUBLE	dTemp1Y	= (DOUBLE)(lDieScanY - lScanDieA_Y);
	DOUBLE	dTemp2X	= (dTemp1X * dScanAngle_X) + (dTemp1Y * dScanAngle_Y);
	DOUBLE	dTemp2Y	= (dTemp1Y * dScanAngle_X) - (dTemp1X * dScanAngle_Y);

	//Re-Transform back to MS896 position
	dTemp1X		= (dTemp2X * dMsPrAngle_X) - (dTemp2Y * dMsPrAngle_Y);
	dTemp1Y		= (dTemp2X * dMsPrAngle_Y) + (dTemp2Y * dMsPrAngle_X);
	lPhyPosX	= lMsPrDieA_X - ConvertUnitToMotorStep(dTemp1X);
	lPhyPosY	= lMsPrDieA_Y - ConvertUnitToMotorStep(dTemp1Y);
	lScnIndex	= lNearIndex;

	return TRUE;
}

BOOL CWaferTable::ScanLShapeDieB(LONG lDiffRow, LONG lDiffCol)
{
	LONG	lDiePitchX_X = GetDiePitchX_X();
	LONG	lDiePitchX_Y = GetDiePitchX_Y();
	LONG	lDiePitchY_X = GetDiePitchY_X();
	LONG	lDiePitchY_Y = GetDiePitchY_Y();

	LONG	lDiff_X = 0;
	LONG	lDiff_Y = 0;
	int i = 0;
	if( lDiffRow>0 )
		lDiff_Y = 1;
	else
		lDiff_Y = -1;
	while( lDiffRow!=0 )
	{
		Sleep(50);
		GetEncoderValue();
		LONG lX = GetCurrX();
		LONG lY = GetCurrY();
		//Move table to updated position
		lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
		if (XY_SafeMoveTo(lX, lY) == FALSE)
		{
			return FALSE;		//return out wafer limit
		}
		Sleep(50);
		WftSearchNormalDie(lX, lY, TRUE);
		i++;
		if( i>=labs(lDiffRow) )
			break;
	}

	lDiff_Y = 0;
	i = 0;
	if( lDiffCol>0 )
		lDiff_X = 1;
	else
		lDiff_X = -1;
	while( lDiffCol!=0 )
	{
		Sleep(50);
		GetEncoderValue();
		LONG lX = GetCurrX();
		LONG lY = GetCurrY();
		//Move table to updated position
		lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
		if (XY_SafeMoveTo(lX, lY) == FALSE)
		{
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT_ALN_OUT_MAPLIMIT 4");	//v3.70T3
			return WT_ALN_OUT_WAFLIMIT;		//return out wafer limit
		}
		WftSearchNormalDie(lX, lY, TRUE);
		i++;
		if( i>=labs(lDiffCol) )
			break;
	}

	return TRUE;
}

BOOL CWaferTable::ConvertScanPosnByQuadrant(ULONG ulDieRow, ULONG ulDieCol, LONG &lPhyPosX, LONG &lPhyPosY, LONG &lIndex)
{
	lIndex = 0;
	LONG	lDieScanX=0, lDieScanY=0;
	if( GetScnData(ulDieRow, ulDieCol, &lDieScanX, &lDieScanY)==FALSE )
	{
		return FALSE;
	}

	DOUBLE dNearDist = GetDistance(m_dScnDieA_X, m_dScnDieA_Y, lDieScanX, lDieScanY);
	for(INT j=0; j<m_lScanPointIndex; j++)
	{
		LONG lScanX = m_lScanDieA_X[j];
		LONG lScanY = m_lScanDieA_Y[j];
		DOUBLE dDist = GetDistance(lDieScanX, lDieScanY, lScanX, lScanY);
		if( dDist<dNearDist )
		{
			dNearDist = dDist;
			lIndex = j;
		}
	}

	DOUBLE dScanAngle_X		= m_dESAngle_X[lIndex];
	DOUBLE dScanAngle_Y		= m_dESAngle_Y[lIndex];
	DOUBLE dMsPrAngle_X		= m_dMSAngle_X[lIndex];
	DOUBLE dMsPrAngle_Y		= m_dMSAngle_Y[lIndex];
	LONG	lScanDieA_X		= m_lScanDieA_X[lIndex];
	LONG	lScanDieA_Y		= m_lScanDieA_Y[lIndex];
	LONG	lMsPrDieA_X		= m_lMsPrDieA_X[lIndex];
	LONG	lMsPrDieA_Y		= m_lMsPrDieA_Y[lIndex];

	//Tansform SCN position related to angle	
	DOUBLE	dTemp1X	= (DOUBLE)(lDieScanX - lScanDieA_X);
	DOUBLE	dTemp1Y	= (DOUBLE)(lDieScanY - lScanDieA_Y);
	DOUBLE	dTemp2X	= (dTemp1X * dScanAngle_X) + (dTemp1Y * dScanAngle_Y);
	DOUBLE	dTemp2Y	= (dTemp1Y * dScanAngle_X) - (dTemp1X * dScanAngle_Y);

	//Re-Transform back to MS896 position
	dTemp1X		= (dTemp2X * dMsPrAngle_X) - (dTemp2Y * dMsPrAngle_Y);
	dTemp1Y		= (dTemp2X * dMsPrAngle_Y) + (dTemp2Y * dMsPrAngle_X);
	lPhyPosX	= lMsPrDieA_X - ConvertUnitToMotorStep(dTemp1X);
	lPhyPosY	= lMsPrDieA_Y - ConvertUnitToMotorStep(dTemp1Y);

	return TRUE;
}

BOOL CWaferTable::IsInnerDie(LONG lDieRow, LONG lDieCol, ULONG ulStart, ULONG ulEnd)
{
	if( IsInMapValidRange(lDieRow, lDieCol)==FALSE )
	{
		return FALSE;
	}

	LONG lTgtRow = 0, lTgtCol = 0, lX, lY;
	BOOL bFindInUL = FALSE, bFindInUR = FALSE, bFindInLL = FALSE, bFindInLR = FALSE;
	for(int i = 0; i <= (LONG)ulEnd; i++)
	{
		for(int j = 0; j <= (LONG)ulEnd; j++)
		{
			if (GetDistance(0, 0, i, j) < (double)ulStart)
			{
				continue;
			}
			// UL
			lTgtRow = lDieRow - i;
			lTgtCol = lDieCol - j;
			if (GetScnData(lTgtRow, lTgtCol, &lX, &lY) &&
				m_pWaferMapManager->IsMapHaveBin(lTgtRow, lTgtCol))
			{
				bFindInUL = TRUE;
			}

			// UR
			lTgtRow = lDieRow - i;
			lTgtCol = lDieCol + j;
			if (GetScnData(lTgtRow, lTgtCol, &lX, &lY) &&
				m_pWaferMapManager->IsMapHaveBin(lTgtRow, lTgtCol))
			{
				bFindInUR = TRUE;
			}

			// LL
			lTgtRow = lDieRow + i;
			lTgtCol = lDieCol - j;
			if (GetScnData(lTgtRow, lTgtCol, &lX, &lY) &&
				m_pWaferMapManager->IsMapHaveBin(lTgtRow, lTgtCol))
			{
				bFindInLL = TRUE;
			}

			// LR
			lTgtRow = lDieRow + i;
			lTgtCol = lDieCol + j;
			if (GetScnData(lTgtRow, lTgtCol, &lX, &lY) &&
				m_pWaferMapManager->IsMapHaveBin(lTgtRow, lTgtCol))
			{
				bFindInLR = TRUE;
			}
		}
	}

	return bFindInUL && bFindInUR && bFindInLL && bFindInLR;
}

VOID CWaferTable::CalculateES101Angle(LONG lScanAX, LONG lScanAY, LONG lScanBX, LONG lScanBY, DOUBLE &dAngleX, DOUBLE &dAngleY)
{
	//Calculate angle
	DOUBLE dDistX	= lScanAX - lScanBX;
	DOUBLE dDistY	= lScanAY - lScanBY;
	DOUBLE dDistXY = sqrt((dDistX*dDistX) + (dDistY*dDistY));
	if (dDistXY < 1.0)
	{
		dDistXY = 1.0;
	}

	dAngleX = dDistX / dDistXY;
	dAngleY = dDistY / dDistXY;

	//Calculate angle
	dDistX	= ConvertMotorStepToUnit(_round(dDistX));
	dDistY	= ConvertMotorStepToUnit(_round(dDistY));
	dDistXY	= ConvertMotorStepToUnit(_round(dDistXY));

	CString szMsg;
	szMsg.Format("Die A %8ld,%8ld, Die B %8ld,%8ld angle X %f, Y %f, dist x %f, dist y %f, dist %f",
		lScanAX, lScanAY, lScanBX, lScanBY, dAngleX, dAngleY, dDistX, dDistY, dDistXY);
	ScnAlignLog(szMsg);
}

BOOL CWaferTable::ScanAlignPoint(LONG ulDieRow, LONG ulDieCol, BOOL bCheckInner)
{
	if( IsInMapValidRange(ulDieRow, ulDieCol)==FALSE )
	{
		return FALSE;
	}

	if( bCheckInner )
	{
		if( IsInnerDie(ulDieRow, ulDieCol, 3, 5)==FALSE )
		{
			return FALSE;
		}
	}

	CString szMsg;
	szMsg.Format("Scn Align point %d,%d check inner %d", ulDieRow, ulDieCol, bCheckInner);
	ScnAlignLog(szMsg);
	LONG lTgtRow = ulDieRow;
	LONG lTgtCol = ulDieCol;
	BOOL bFindPointA = FALSE;

	m_WaferMapWrapper.SetCurrentPosition(lTgtRow, lTgtCol);
	LONG lDieScanAX = 0, lDieScanAY = 0, lDieMsPrAX = 0, lDieMsPrAY = 0;
	for (ULONG ulLoop = 0; ulLoop <= 1; ulLoop++)
	{
		LONG lMaxIndex = ulLoop * 2;

		//Move to RIGHT & PR search on current die
		if( ulLoop!=0 )
			lTgtCol += 1;
		LONG lIndex = 0;
		if( GetScnData(lTgtRow, lTgtCol, &lDieScanAX, &lDieScanAY) &&
			ConvertScanPosnByQuadrant(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY, lIndex) )
		{
			XY_SafeMoveTo(lDieMsPrAX, lDieMsPrAY);
			Sleep(100);
			if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
			{
				bFindPointA = TRUE;
				break;
			}
		}
		if( bFindPointA )
		{
			break;
		}

		// Right side Move to UP & PR search on current die
		for(LONG lIndex=0; lIndex < (lMaxIndex - 1); lIndex++)
		{
			lTgtRow -= 1;
			if( GetScnData(lTgtRow, lTgtCol, &lDieScanAX, &lDieScanAY) &&
				ConvertScanPosnByQuadrant(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY, lIndex) )
			{
				XY_SafeMoveTo(lDieMsPrAX, lDieMsPrAY);
				Sleep(100);
				if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
				{
					bFindPointA = TRUE;
					break;
				}
			}
		}
		if( bFindPointA )
		{
			break;
		}

		//up side Move to LEFT & PR search on current die
		for(LONG lIndex=0; lIndex < lMaxIndex; lIndex++)
		{
			lTgtCol -= 1;
			if( GetScnData(lTgtRow, lTgtCol, &lDieScanAX, &lDieScanAY) &&
				ConvertScanPosnByQuadrant(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY, lIndex) )
			{
				XY_SafeMoveTo(lDieMsPrAX, lDieMsPrAY);
				Sleep(100);
				if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
				{
					bFindPointA = TRUE;
					break;
				}
			}
		}
		if( bFindPointA )
		{
			break;
		}

		// left side Move to DOWN & PR search on current die
		for(LONG lIndex=0; lIndex < lMaxIndex ; lIndex++)
		{
			lTgtRow += 1;
			if( GetScnData(lTgtRow, lTgtCol, &lDieScanAX, &lDieScanAY) &&
				ConvertScanPosnByQuadrant(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY, lIndex) )
			{
				XY_SafeMoveTo(lDieMsPrAX, lDieMsPrAY);
				Sleep(100);
				if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
				{
					bFindPointA = TRUE;
					break;
				}
			}
		}
		if( bFindPointA )
		{
			break;
		}

		// down side Move to DOWN & PR search on current die
		for(LONG lIndex=0; lIndex < lMaxIndex; lIndex++)
		{
			lTgtCol += 1;
			if( GetScnData(lTgtRow, lTgtCol, &lDieScanAX, &lDieScanAY) &&
				ConvertScanPosnByQuadrant(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY, lIndex) )
			{
				XY_SafeMoveTo(lDieMsPrAX, lDieMsPrAY);
				Sleep(100);
				if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
				{
					bFindPointA = TRUE;
					break;
				}
			}
		}
		if( bFindPointA )
		{
			break;
		}
	}

	DOUBLE dNearDist = -1;
	LONG lNearIndex = -1;
	for(INT j=0; j<m_lScanPointIndex; j++)
	{
		LONG lDieAMsPrX = m_lMsPrDieA_X[j];
		LONG lDieAMsPrY = m_lMsPrDieA_Y[j];

		if( labs(lDieAMsPrX-lDieMsPrAX) >= labs(m_lSCNIndex_X*GetDiePitchX_X()) &&
			labs(lDieAMsPrY-lDieMsPrAY) >= labs(m_lSCNIndex_Y*GetDiePitchY_Y()) )
		{
			DOUBLE dDist = GetDistance(lDieAMsPrX, lDieAMsPrY, lDieMsPrAX, lDieMsPrAY);
			if( dNearDist==-1 || dNearDist>dDist)
			{
				dNearDist = dDist;
				lNearIndex = j;
			}
		}
	}

	LONG	lDieScanBX = m_dScnDieA_X;
	LONG	lDieScanBY = m_dScnDieA_Y;
	LONG	lDieMsPrBX = m_lMsDieA_X;
	LONG	lDieMsPrBY = m_lMsDieA_Y;
	if( lNearIndex!=-1 )
	{
		lDieScanBX = m_lScanDieA_X[lNearIndex];
		lDieScanBY = m_lScanDieA_Y[lNearIndex];
		lDieMsPrBX = m_lMsPrDieA_X[lNearIndex];
		lDieMsPrBY = m_lMsPrDieA_Y[lNearIndex];
	}

	if( bFindPointA && m_lScanPointIndex<=(SCN_POINTA_NUM-1))
	{
		DOUBLE	dEsAngleX, dEsAngleY, dMsAngleX, dMsAngleY;

		CalculateES101Angle(lDieScanAX, lDieScanAY, lDieScanBX, lDieScanBY, dEsAngleX, dEsAngleY);			
		CalculateES101Angle(lDieMsPrBX, lDieMsPrBY, lDieMsPrAX, lDieMsPrAY, dMsAngleX, dMsAngleY);	

		m_lScanDieA_X[m_lScanPointIndex] = lDieScanAX;
		m_lScanDieA_Y[m_lScanPointIndex] = lDieScanAY;
		m_lMsPrDieA_X[m_lScanPointIndex] = lDieMsPrAX;
		m_lMsPrDieA_Y[m_lScanPointIndex] = lDieMsPrAY;
		m_dESAngle_X[m_lScanPointIndex]	= dEsAngleX;
		m_dESAngle_Y[m_lScanPointIndex]	= dEsAngleY;
		m_dMSAngle_X[m_lScanPointIndex]	= dMsAngleX;
		m_dMSAngle_Y[m_lScanPointIndex]	= dMsAngleY;
		m_lScanPointIndex++;
	}

	return bFindPointA;
}


LONG CWaferTable::AlignScannerWafer(VOID)
{
	LONG	i = 0;
	CString szTitle, szContent;
	ULONG ulDieState = 0;
	LONG	m_lScnMaxSkipAllow			= 5;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	//Reset last mark die position
	ResetNoPickDieState();

	szTitle.LoadString(HMB_WT_ALIGN_WAFER);

	if (GetKeepScnAlignDie() )
	{
		//Mark die & set die state to unpick mode	
		m_WaferMapWrapper.MarkDie(m_ulCurrentRow, m_ulCurrentCol, TRUE);
		ulDieState = m_WaferMapWrapper.GetDieState(m_ulCurrentRow,m_ulCurrentCol );
		if( !IsDieUnpickAll(ulDieState) )
			ulDieState = WT_MAP_DIESTATE_UNPICK;
		m_WaferMapWrapper.SetDieState(m_ulCurrentRow, m_ulCurrentCol, ulDieState);
	}

	//Issue error if 1st ALIGN die is EMPTY die on map
	if (IsMapNullBin(m_ulCurrentRow, m_ulCurrentCol))
	{
		HmiMessage_Red_Yellow("Align Scan Wafer: 1st die is EMPTY die; please realign wafer manually.");
		ScnAlignLog("Align SCN wafer: 1st die is empty die");
		return WT_ALN_ERROR;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szTemp = pApp->GetProfileString(gszPROFILE_SETTING, _T("MS Aln Table Theta"), "0.0");	//	get when align
	m_dAlnTableTheta		= atof(szTemp)/180.0;
	CString szMsg;
	szMsg.Format("MS Table Theta degree %f,%s", m_dScnTableTheta*180.0, szTemp);
	ScnAlignLog(szMsg);

	ChangeCameraToWafer();

	if( m_lScnAlignMethod==3 && IsScnLoaded() )	//	manual align mode, need OP select map row/column and wafer table position
	{
		return AlignScannerWafer_Manual();
	}	//	OCR bar wafer scanned by AOI and to align/sorting without prescan

	BOOL bFindRefer = m_bFindAllRefer4Scan;
	ULONG ulMapNumOfReferDice = m_WaferMapWrapper.GetNumberOfReferenceDice();
	if( ulMapNumOfReferDice<2 )
	{
		bFindRefer = FALSE;
	}
	if( IsRealignScnnedWafer() )
	{
		bFindRefer = FALSE;
	}

	LONG lAX = 0, lAY = 0, lBX = 0, lBY = 0;
	if( bFindRefer )
	{
		BOOL bRefersFound = FindAndAlignAllReferDie();	//	SCN function
		if (!bRefersFound)
		{
			ScnAlignLog("Align SCN wafer no refer die found!  Alignment fails.");
			return WT_ALN_ERROR;
		}

		if( pUtl->GetNumOfReferPoints()<=1 )
		{
			ScnAlignLog("Align SCN wafer refer die is 0!  Alignment fails.");
			return WT_ALN_ERROR;
		}
	}
	else
	{
		pUtl->RemoveAllReferPoints();
		//Move to DOWN
		LONG lScnSkipCount = 0;
		BOOL bScnDieAFound = FALSE;		//v3.64
		LONG	lAlignDieResult	= 1;
		LONG	lUserConfirm = glHMI_CONTINUE;

		for (i=0; i<(m_lSCNIndex_Y-1); i++)
		{
			lAlignDieResult = SetAlignIndexPoint(TRUE, 3, 1, TRUE, TRUE);

			if (GetKeepScnAlignDie() )
			{
				//Mark die & set die state to unpick mode	
				m_WaferMapWrapper.MarkDie(m_ulCurrentRow, m_ulCurrentCol, TRUE);
				ulDieState = m_WaferMapWrapper.GetDieState(m_ulCurrentRow, m_ulCurrentCol);
				if( !IsDieUnpickAll(ulDieState) )
					ulDieState = WT_MAP_DIESTATE_UNPICK;
				m_WaferMapWrapper.SetDieState(m_ulCurrentRow, m_ulCurrentCol, ulDieState);
			}

			lUserConfirm = glHMI_CONTINUE;
			//Display Message if need user to confirm align wafer process
			switch(lAlignDieResult)
			{
			case WT_ALN_IS_DEFECT:
				lScnSkipCount++;
				if ( lScnSkipCount >= m_lScnMaxSkipAllow )
				{
					szContent.LoadString(HMB_WT_ALN_IS_DEFECT);
					ScnAlignLog("Align SCN wafer hits Y DEFECT counter");
					lUserConfirm = HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					lScnSkipCount = 0;

					if ( lUserConfirm == 1 )
					{
						ScnAlignLog("Align SCN wafer hits Y DEFECT counter continue...");
					}
				}
				else
				{
					lUserConfirm = glHMI_CONTINUE;
					ScnAlignLog("Align SCN wafer hits Y DEFECT counter continue");
				}
				break;

			case WT_ALN_IS_EMPTY:
				lScnSkipCount++;
				if ( lScnSkipCount >= m_lScnMaxSkipAllow )
				{
					szContent.LoadString(HMB_WT_ALN_IS_EMPTY);
					ScnAlignLog("Align SCN wafer hits Y EMPTY counter");
					lUserConfirm = HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					lScnSkipCount = 0;

					if ( lUserConfirm == 1 )
					{
						ScnAlignLog("Align SCN wafer hits Y EMPTY counter continue...");
					}
				}
				else
				{
					lUserConfirm = glHMI_CONTINUE;
					ScnAlignLog("Align SCN wafer hits Y EMPTY counter continue");
				}
				break;

			case WT_ALN_IS_NOGRADE:
				lScnSkipCount++;
				if ( lScnSkipCount >= m_lScnMaxSkipAllow )
				{
					szContent.LoadString(HMB_WT_ALN_IS_NO_GRADE);
					ScnAlignLog("Align SCN wafer hits Y NOGRADE counter");
					lUserConfirm = HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					lScnSkipCount = 0;

					if ( lUserConfirm == 1 )
					{
						ScnAlignLog("Align SCN wafer hits Y NOGRADE counter continue...");
					}
				}
				else
				{
					lUserConfirm = glHMI_CONTINUE;
					ScnAlignLog("Align SCN wafer hits Y NOGRADE counter continue");
				}
				break;

			case WT_ALN_OUT_WAFLIMIT:
				szContent.LoadString(HMB_WT_OUT_WAFER_LIMIT);
				lUserConfirm = HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				ScnAlignLog("Align SCN wafer hits Y WAF-LIMIT");
				return WT_ALN_ERROR;

			case WT_ALN_OUT_MAPLIMIT:
				ScnAlignLog("Align SCN wafer hits Y MAP-LIMIT");
				return WT_ALN_ERROR;

			case WT_ALN_SEARCH_DIE_ERROR:
				ScnAlignLog("Align SCN wafer hits Y SEARCH-DIE error");
				return WT_ALN_ERROR;

			default:	//WT_ALN_IS_GOOD
				lScnSkipCount = 0;
				lUserConfirm = glHMI_CONTINUE;
				ScnAlignLog("Align SCN wafer Index Y is good");
				break;
			}

			if (lUserConfirm != glHMI_CONTINUE)
			{
				//Stop align prcoss & return -1
				ScnAlignLog("Align SCN wafer Y aborted");

				//Reset last mark die position
				ResetNoPickDieState();
				return WT_ALN_ERROR;		
			}

			//Store SCN Align Die A data
			if ( bScnDieAFound == FALSE && ( (lAlignDieResult == WT_ALN_IS_GOOD) || (lAlignDieResult == WT_ALN_IS_DEFECT) ) ) 
			{
				bScnDieAFound	= TRUE;
				m_ulScnDieA_Row = m_ulCurrentRow;
				m_ulScnDieA_Col = m_ulCurrentCol;
				lAX		= m_lCurrent_X;
				lAY		= m_lCurrent_Y;
			}
		}

		if (!bScnDieAFound)		//v3.64
		{
			ScnAlignLog("Align SCN wafer Y SCN Die A not found!  Alignment fails.");
			return WT_ALN_ERROR;
		}

		//Move to RHS
		lScnSkipCount = 0;
		BOOL bScnDieBFound = FALSE;		//v3.64

		for (i=0; i<(m_lSCNIndex_X - 1); i++)
		{
			lAlignDieResult = SetAlignIndexPoint(TRUE, 1, 1, TRUE, TRUE);

			if (GetKeepScnAlignDie() == TRUE)
			{
				//Mark die & set die state to unpick mode	
				m_WaferMapWrapper.MarkDie(m_ulCurrentRow, m_ulCurrentCol, TRUE);
				ulDieState = m_WaferMapWrapper.GetDieState(m_ulCurrentRow,m_ulCurrentCol );
				if( !IsDieUnpickAll(ulDieState) )
					ulDieState = WT_MAP_DIESTATE_UNPICK;
				m_WaferMapWrapper.SetDieState(m_ulCurrentRow, m_ulCurrentCol, ulDieState);
			}

			lUserConfirm = glHMI_CONTINUE;
			//Display Message if need user to confirm align wafer process
			switch(lAlignDieResult)
			{
			case WT_ALN_IS_DEFECT:
				lScnSkipCount++;
				if ( lScnSkipCount >= m_lScnMaxSkipAllow )
				{
					szContent.LoadString(HMB_WT_ALN_IS_DEFECT);
					ScnAlignLog("Align SCN wafer hits X DEFECT counter");
					lUserConfirm = HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					lScnSkipCount = 0;

					if ( lUserConfirm == 1 )
					{
						ScnAlignLog("Align SCN wafer hits X DEFECT counter continue ...");
					}
				}
				else
				{
					lUserConfirm = glHMI_CONTINUE;
					ScnAlignLog("Align SCN wafer hits X DEFECT counter continue");
				}
				break;

			case WT_ALN_IS_EMPTY:
				lScnSkipCount++;
				if ( lScnSkipCount >= m_lScnMaxSkipAllow )
				{
					szContent.LoadString(HMB_WT_ALN_IS_EMPTY);
					ScnAlignLog("Align SCN wafer hits X EMPTY counter");
					lUserConfirm = HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					lScnSkipCount = 0;

					if ( lUserConfirm == 1 )
					{
						ScnAlignLog("Align SCN wafer hits X EMPTY counter continue...");
					}
				}
				else
				{
					lUserConfirm = glHMI_CONTINUE;
					ScnAlignLog("Align SCN wafer hits X EMPTY counter continue");
				}
				break;

			case WT_ALN_IS_NOGRADE:
				lScnSkipCount++;
				if ( lScnSkipCount >= m_lScnMaxSkipAllow )
				{
					szContent.LoadString(HMB_WT_ALN_IS_NO_GRADE);
					ScnAlignLog("Align SCN wafer hits X NOGRADE counter");
					lUserConfirm = HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					lScnSkipCount = 0;

					if ( lUserConfirm == 1 )
					{
						ScnAlignLog("Align SCN wafer hits X NOGRADE counter continue...");
					}
				}
				else
				{
					lUserConfirm = glHMI_CONTINUE;
					ScnAlignLog("Align SCN wafer hits X NOGRADE counter continue");
				}
				break;

			case WT_ALN_OUT_WAFLIMIT:
				szContent.LoadString(HMB_WT_OUT_WAFER_LIMIT);
				ScnAlignLog("Align SCN wafer hits X WAF-LIMIT");
				lUserConfirm = HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				return WT_ALN_ERROR;

			case WT_ALN_OUT_MAPLIMIT:
				ScnAlignLog("Align SCN wafer hits X MAP-LIMIT");
				return WT_ALN_ERROR;

			case WT_ALN_SEARCH_DIE_ERROR:
				ScnAlignLog("Align SCN wafer hits X SEARCH-DIE Error");
				return WT_ALN_ERROR;

			default:	//WT_ALN_IS_GOOD
				ScnAlignLog("Align SCN wafer index X is good");
				lScnSkipCount = 0;
				lUserConfirm = glHMI_CONTINUE;
				break;
			}

			if (lUserConfirm != glHMI_CONTINUE)
			{
				//Stop align prcoss & return -1
				ScnAlignLog("Align SCN wafer X aborted");
				return WT_ALN_ERROR;		
			}

			//Store SCN Align Die B data
			if ( (lAlignDieResult == WT_ALN_IS_GOOD) || (lAlignDieResult == WT_ALN_IS_DEFECT) )
			{
				bScnDieBFound	= TRUE;
				m_ulScnDieB_Row = m_ulCurrentRow;
				m_ulScnDieB_Col = m_ulCurrentCol;
				lBX		= m_lCurrent_X;
				lBY		= m_lCurrent_Y;
			}
		}

		if (!bScnDieBFound)		//v3.64
		{
			ScnAlignLog("Align SCN wafer X SCN Die B not found!  Alignment fails");
			return WT_ALN_ERROR;		
		}

		pUtl->AddAllReferPosition(m_ulScnDieA_Row, m_ulScnDieA_Col, lAX, lAY);
		pUtl->AddAllReferPosition(m_ulScnDieB_Row, m_ulScnDieB_Col, lBX, lBY);
	}

	if( m_bFindAllRefer4Scan && IsScnLoaded() )
	{
		return AlignScannerWafer_Refer();
	}
	else if( IsScnLoaded() )
	{
		ScnAlignLog("Scan file loaded and find Die A and B, now calculate offset");
		//Calculate WS896 & MS896 Angle
		if( CalculateWS896Angle()==FALSE )
		{
			ScnAlignLog("Calculate WS Angle error!  Alignment fails");
			return WT_ALN_ERROR;		
		}

		m_lMsDieA_X		= lAX;
		m_lMsDieA_Y		= lAY;
		m_lMsDieB_X		= lBX;
		m_lMsDieB_Y		= lBY;
		DOUBLE dAngleX = 0, dAngleY = 1.0;
		CalculateES101Angle(lBX, lBY, lAX, lAY, dAngleX, dAngleY);
		m_dMS896Angle_X = dAngleX;
		m_dMS896Angle_Y = dAngleY;

		ConvertWSDataToPrescanAndMap();

		switch( m_lScnAlignMethod )
		{
		case 2:	// first to check FPC points, then move from inner to outer to get local factors.
			ScanAlignInToOutSpiral();	//	orbit
			break;
		case 1:
			ScanAlignWithFivePoints();
			break;
		case 0:
		default:
			break;
		}
	}

	if( IsRealignScnnedWafer() )
	{
		m_lMsDieA_X		= lAX;
		m_lMsDieA_Y		= lAY;
		m_lMsDieB_X		= lBX;
		m_lMsDieB_Y		= lBY;
	}	//	for prescanned wafer do align again.

	return WT_ALN_SCN_OK;		
}

VOID CWaferTable::ScnAlignLog(CString szMsg)
{
	FILE *fp = NULL;
	CString szLogFile = gszUSER_DIRECTORY + "\\History\\ScnAln.txt";
	errno_t nErr = fopen_s(&fp, szLogFile, "a+");
	if ((nErr == 0) && (fp != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(fp, "(%2d:%2d:%2d)	%s\n",	theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LPCTSTR) szMsg);
		fclose(fp);
	}
}


BOOL CWaferTable::CalculateWS896Angle(VOID)
{
	LONG	lAX = 0, lAY = 0, lBX = 0, lBY = 0;

	//Get SCN data from wafermap function 
	if( GetScnData(m_ulScnDieA_Row, m_ulScnDieA_Col, &lAX, &lAY) )
	{
		m_dScnDieA_X = lAX;
		m_dScnDieA_Y = lAY;
		ScnAlignLog("Get WS A point ok");
	}
	else
	{
		HmiMessage_Red_Back("Align point A has no SCN information.");
		return FALSE;
	}

	if( GetScnData(m_ulScnDieB_Row, m_ulScnDieB_Col, &lBX, &lBY) )
	{
		ScnAlignLog("Get WS B point ok");
	}
	else
	{
		HmiMessage_Red_Back("Align point B has no SCN information.");
		return FALSE;
	}

	//Calculate angle
	DOUBLE dDistX	= m_dScnDieA_X - lBX;
	DOUBLE dDistY	= m_dScnDieA_Y - lBY;
	DOUBLE dDistXY	= sqrt((dDistX*dDistX) + (dDistY*dDistY));
	if (dDistXY < 1.0)
	{
		dDistXY = 1.0;
	}

	m_dWS896Angle_X = dDistX / dDistXY;
	m_dWS896Angle_Y = dDistY / dDistXY;
	CString szMsg;
	szMsg.Format("WS die A (%4d,%4d) %8d,%8d, die B (%4d,%4d) %8d,%8d angle X %f, Y %f, dist x %f, dist y %f, dist %f",
		m_ulScnDieA_Row, m_ulScnDieA_Col, m_dScnDieA_X, m_dScnDieA_Y, m_ulScnDieB_Row, m_ulScnDieB_Col, lBX, lBY, 
		m_dWS896Angle_X, m_dWS896Angle_Y, dDistX, dDistY, dDistXY);
	ScnAlignLog(szMsg);

	return TRUE;
}


BOOL CWaferTable::InitScnData(VOID)
{
	DeleteFile((gszUSER_DIRECTORY + "\\MapFile\\CurrentMap.svs"));
	DeleteFile(gszUSER_DIRECTORY + "\\History\\ScnAln.txt");

	m_ulScnRefDieRow = 0;
	m_ulScnRefDieCol = 0;

	DelPrescanRunPosn();	//	clear when init scn data.

	return TRUE;
}

BOOL CWaferTable::DumpScnData(BOOL bDumpFile)
{
	BOOL bFileOpen = FALSE;
	CStdioFile fScnFile;
	CString szFileName = gszUSER_DIRECTORY + "\\MapFile\\CurrentMap.svs";

	if ( bDumpFile == TRUE )
	{
		if( fScnFile.Open(szFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText) )
		{
			ScnAlignLog("WS dump scan date to file");
			bFileOpen = TRUE;
			CString szMsg;

			fScnFile.WriteString("raw data\n");
			for (ULONG i=0; i<WT_SCN_MAX_ROW; i++)
			{
				for (ULONG j=0; j<WT_SCN_MAX_COL; j++)
				{
					LONG lX = 0, lY = 0;
					if( GetScnData(i, j, &lX, &lY) )
					{
						szMsg.Format("%d,%d,%d,%d\n", i, j, lX, lY);
						fScnFile.WriteString(szMsg);
					}
				}
			}
			fScnFile.WriteString("refer data\n");
			szMsg.Format("%d,%d\n", m_ulScnRefDieRow, m_ulScnRefDieCol);
			fScnFile.WriteString(szMsg);
			fScnFile.Close();
		}
	}
	else
	{
		if( fScnFile.Open(szFileName, CFile::modeRead|CFile::shareDenyNone) )
		{
			ScnAlignLog("WS dump scan data read from file");
			bFileOpen = TRUE;
			CString szReading;
			CStringArray szaDataList;
			fScnFile.SeekToBegin();
			fScnFile.ReadString(szReading);

			if( szReading.CompareNoCase("raw data")==0 )
			{
				fScnFile.Close();
				return FALSE;
			}

			while( fScnFile.ReadString(szReading) )
			{
				if( szReading.CompareNoCase("refer data")==0 )
				{
					break;
				}
				szaDataList.RemoveAll();
				CUtility::Instance()->ParseRawData(szReading, szaDataList);
				if( szaDataList.GetSize()>=4 )
				{
					LONG lRow = atoi(szaDataList.GetAt(0));
					LONG lCol = atoi(szaDataList.GetAt(1));
					LONG lWfX = atoi(szaDataList.GetAt(2));
					LONG lWfY = atoi(szaDataList.GetAt(3));
					SetScnData(lRow, lCol, lWfX, lWfY, 1);
				}
			}

			if( fScnFile.ReadString(szReading) )
			{
				szaDataList.RemoveAll();
				CUtility::Instance()->ParseRawData(szReading, szaDataList);
				if( szaDataList.GetSize()>=2 )
				{
					m_ulScnRefDieRow = atoi(szaDataList.GetAt(0));
					m_ulScnRefDieCol = atoi(szaDataList.GetAt(1));
				}
			}

			fScnFile.Close();
		}
	}

	return bFileOpen;
}


BOOL CWaferTable::SetScnData(ULONG ulRow, ULONG ulCol, LONG lX, LONG lY, UCHAR ucGrade)
{
	if( ulRow>=WT_SCN_MAX_ROW || ulCol>=WT_SCN_MAX_COL )
		return FALSE;

	SetPrescanRunPosn(ulRow, ulCol, lX, lY);	//	after map loading load SCN and update

	return TRUE;
}


BOOL CWaferTable::GetScnData(ULONG ulRow, ULONG ulCol, LONG *lX, LONG *lY)
{
	if( ulRow>=WT_SCN_MAX_ROW || ulCol>=WT_SCN_MAX_COL )
		return FALSE;

	LONG encX = 0, encY = 0;
	bool bReturn = GetPrescanRunPosn(ulRow, ulCol, encX, encY);
	*lX = encX;
	*lY = encY;
#if		SINGLE_ALIGN_POINT
	*lX = _round(encX + encY * tan(m_dScnTableTheta));	//	normal optional
	*lY = _round(encY * cos(m_dScnTableTheta));			//	normal optional
#endif

	return bReturn;
}

BOOL CWaferTable::SetScnRefDie(ULONG ulRow, ULONG ulCol, DOUBLE dScnTableTheta)
{
	m_ulScnRefDieRow = ulRow;
	m_ulScnRefDieCol = ulCol;
	m_dScnTableTheta = dScnTableTheta/180.0;

	return TRUE;
}

BOOL CWaferTable::GetScnRefDie(ULONG *ulRow, ULONG *ulCol)
{
	*ulRow = m_ulScnRefDieRow;
	*ulCol = m_ulScnRefDieCol;

	return TRUE;
}

BOOL CWaferTable::ConvertPhysicalPos(unsigned long ulCurrRow, unsigned long ulCurrCol, LONG *lPhyPosX, LONG *lPhyPosY)
{
	DOUBLE	dTemp1X = 0;
	DOUBLE	dTemp1Y = 0;
	DOUBLE	dTemp2X = 0;
	DOUBLE	dTemp2Y = 0;
	DOUBLE	dScnPosX = 0;
	DOUBLE	dScnPosY = 0;
	LONG	lX=0, lY=0;

	if( GetScnData(ulCurrRow, ulCurrCol, &lX, &lY)==FALSE )
	{
		return FALSE;
	}
	dScnPosX = (DOUBLE)lX;
	dScnPosY = (DOUBLE)lY;

	DOUBLE dDistRatio = 1.0;
	//Tansform SCN position related to angle	
	dTemp1X		= (dScnPosX - m_dScnDieA_X) * dDistRatio;
	dTemp1Y		= (dScnPosY - m_dScnDieA_Y) * dDistRatio;
	dTemp2X		= (dTemp1X * m_dWS896Angle_X) + (dTemp1Y * m_dWS896Angle_Y);
	dTemp2Y		= (dTemp1Y * m_dWS896Angle_X) - (dTemp1X * m_dWS896Angle_Y);

	//Re-Transform back to MS896 position
	dTemp1X		= (dTemp2X * m_dMS896Angle_X) - (dTemp2Y * m_dMS896Angle_Y);
	dTemp1Y		= (dTemp2X * m_dMS896Angle_Y) + (dTemp2Y * m_dMS896Angle_X);
	*lPhyPosX	= m_lMsDieA_X - ConvertUnitToMotorStep(dTemp1X);
	*lPhyPosY	= m_lMsDieA_Y - ConvertUnitToMotorStep(dTemp1Y);

	return TRUE;
}

BOOL CWaferTable::IndexToSCNOffsetPos(ULONG *ulStartRow, ULONG *ulStartCol)
{
	if ( m_ulAlignOption != WT_MANUAL_HOMEDIE )
	{
		ULONG ulAlignRow;
		ULONG ulAlignCol;
		LONG lIndexCount = 0;
		LONG lIndexDir = 0;
		LONG lRowOffset;
		LONG lColOffset;

		GetAsmMapHomeDie(ulAlignRow, ulAlignCol);
		GetMapOffset(lRowOffset, lColOffset);
			
		m_ulCurrentRow = ulAlignRow; 
		m_ulCurrentCol = ulAlignCol;

		//Check Row & Col must be >= 0
		if ( ((LONG)ulAlignRow + lRowOffset) < 0 )
		{
			lRowOffset = 0;
		}

		if ( ((LONG)ulAlignCol + lColOffset) < 0 )
		{
			lColOffset = 0;
		}

		//Index to offset Y(Row) position
		lIndexCount = abs(lRowOffset);
		lIndexDir = 3;
		if ( lRowOffset < 0 )
		{
			lIndexDir = 2;
		}

		while(1)
		{
			if ( lIndexCount == 0 )
			{
				break;			
			}
			SetAlignIndexPoint(TRUE, lIndexDir, 1);
			lIndexCount--;	
		}

		//Index to offset X(Col) position
		lIndexCount = abs(lColOffset);
		lIndexDir = 1;
		if ( lColOffset < 0 )
		{
			lIndexDir = 0;
		}

		while(1)
		{
			if ( lIndexCount == 0 )
			{
				break;			
			}
			SetAlignIndexPoint(TRUE, lIndexDir, 1);
			lIndexCount--;	
		}

		*ulStartRow = ulAlignRow + (ULONG)lRowOffset;
		*ulStartCol = ulAlignCol + (ULONG)lColOffset;
	}

	return TRUE;
}

VOID CWaferTable::ScanAlignInToOutSpiral()
{
	m_lScanPointIndex = 0;
	m_lScanDieA_X[m_lScanPointIndex] = m_dScnDieA_X;
	m_lScanDieA_Y[m_lScanPointIndex] = m_dScnDieA_Y;
	m_lMsPrDieA_X[m_lScanPointIndex] = m_lMsDieA_X;
	m_lMsPrDieA_Y[m_lScanPointIndex] = m_lMsDieA_Y;
	m_dESAngle_X[m_lScanPointIndex]	= m_dWS896Angle_X;
	m_dESAngle_Y[m_lScanPointIndex]	= m_dWS896Angle_Y;
	m_dMSAngle_X[m_lScanPointIndex]	= m_dMS896Angle_X;
	m_dMSAngle_Y[m_lScanPointIndex]	= m_dMS896Angle_Y;
	m_lScanPointIndex++;

	if( IsEnableFPC() )
	{
		ScnAlignLog("To find the settings in FPC");
		for (int i=1; i<=m_lScnAlignTotalPoints; i++)
		{
			ULONG	ulDieARow = 0, ulDieACol = 0;
			LONG	lUserRow, lUserCol;
			if( GetScnFPCMapPosition(i, ulDieARow, ulDieACol, lUserRow, lUserCol) )
			{
				ScanAlignPoint(ulDieARow, ulDieACol);	// five point check
			}
		}
	}

	LONG lGridRow = CP_GetGridRows();
	LONG lGridCol = CP_GetGridCols();

	LONG lTgtRow = m_ulScnDieA_Row;
	LONG lTgtCol = m_ulScnDieA_Col;

	ULONG ulRowUpLoop = labs(GetMapValidMinRow() - lTgtRow)/lGridRow;
	ULONG ulRowDnLoop = labs(GetMapValidMaxRow() - lTgtRow)/lGridRow;
	ULONG ulColLtLoop = labs(GetMapValidMinCol() - lTgtCol)/lGridCol;
	ULONG ulColRtLoop = labs(GetMapValidMaxCol() - lTgtCol)/lGridCol;
	ULONG ulMaxLoop = 0;
	ulMaxLoop = max(ulMaxLoop, ulRowUpLoop);
	ulMaxLoop = max(ulMaxLoop, ulRowDnLoop);
	ulMaxLoop = max(ulMaxLoop, ulColLtLoop);
	ulMaxLoop = max(ulMaxLoop, ulColRtLoop);

	CString szMsg;
	szMsg.Format("To find the relationship parameters inner to outer; max loop is %d", ulMaxLoop);
	ScnAlignLog(szMsg);
	for (ULONG ulLoop = 1; ulLoop <= ulMaxLoop; ulLoop++)
	{
		LONG lMaxIndex = ulLoop * 2;

		//Move to RIGHT & PR search on current die
		lTgtCol += lGridCol;
		ScanAlignPoint(lTgtRow, lTgtCol);

		// Right side Move to UP & PR search on current die
		for(LONG lIndex=0; lIndex < (lMaxIndex - 1); lIndex++)
		{
			lTgtRow -= lGridRow;
			ScanAlignPoint(lTgtRow, lTgtCol);
		}

		//up side Move to LEFT & PR search on current die
		for(LONG lIndex=0; lIndex < (lMaxIndex); lIndex++)
		{
			lTgtCol -= lGridCol;
			ScanAlignPoint(lTgtRow, lTgtCol);
		}

		// left side Move to DOWN & PR search on current die
		for(LONG lIndex=0; lIndex < lMaxIndex ; lIndex++)
		{
			lTgtRow += lGridRow;
			ScanAlignPoint(lTgtRow, lTgtCol);
		}

		// down side Move to DOWN & PR search on current die
		for(LONG lIndex=0; lIndex < (lMaxIndex); lIndex++)
		{
			lTgtCol += lGridCol;
			ScanAlignPoint(lTgtRow, lTgtCol);
		}
	}

	for(INT j=0; j<m_lScanPointIndex; j++)
	{
		szMsg.Format("point %2d,%8ld,%8ld,%f,%f,%f,%f",	
			j, m_lMsPrDieA_X[j], m_lMsPrDieA_Y[j], m_dESAngle_X[j], m_dESAngle_Y[j], m_dMSAngle_X[j], m_dMSAngle_Y[j]);
		ScnAlignLog(szMsg);
	}
	ClearPrescanInfo();
	ResetMapPhyPosn();

	// set wafer scanner position to prescan result and run prescan and map physical postion
	for(LONG ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
	{
		for(LONG ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
		{
			LONG lPhyX = 0, lPhyY = 0, lIndex = 0;
			if( ConvertScanPosnByQuadrant(ulRow, ulCol, lPhyX, lPhyY, lIndex) )
			{
				SetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
				SetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY);
				LONG lHmiRow, lHmiCol;
				ConvertAsmToHmiUser(ulRow, ulCol, lHmiRow, lHmiCol);
				szMsg.Format("%3ld,%3ld; %8ld,%8ld, %2ld", lHmiRow, lHmiCol, lPhyX, lPhyY, lIndex);
			//	ScnAlignLog(szMsg);
			}
		}
	}
}

#define	FIND_POINT_B	0
VOID CWaferTable::ScanAlignWithFivePoints()	//	FPC scn align method
{
	ULONG ulNumRow = 0, ulNumCol = 0;
	m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);
	DOUBLE dStartTime = GetTime();
	LONG	i=0, j=0;
	for(i=0; i<WT_ALN_MAXCHECK_SCN; i++)
	{
		m_stScnAlnPoints[i].m_bDieAFound = FALSE;
		m_stScnAlnPoints[i].m_bAngleFound = FALSE;
	}
	m_stScnAlnPoints[j].m_bDieAFound	= TRUE;
	m_stScnAlnPoints[j].m_lDieAScanX	= m_dScnDieA_X;
	m_stScnAlnPoints[j].m_lDieAScanY	= m_dScnDieA_Y;
	m_stScnAlnPoints[j].m_ulDieARow		= m_ulScnDieA_Row;
	m_stScnAlnPoints[j].m_ulDieACol		= m_ulScnDieA_Col;
	m_stScnAlnPoints[j].m_lDieAMsPrX	= m_lMsDieA_X;
	m_stScnAlnPoints[j].m_lDieAMsPrY	= m_lMsDieA_Y;
	m_stScnAlnPoints[j].m_bAngleFound	= TRUE;
	m_stScnAlnPoints[j].m_dMsPrAngle_X	= m_dMS896Angle_X;
	m_stScnAlnPoints[j].m_dMsPrAngle_Y	= m_dMS896Angle_Y;
	m_stScnAlnPoints[j].m_dScanAngle_X	= m_dWS896Angle_X;
	m_stScnAlnPoints[j].m_dScanAngle_Y	= m_dWS896Angle_Y;
	j++;

	CString szMsg;
	LONG lPrDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];
	UCHAR ucReferGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();

	for (i=1; i<=m_lScnAlignTotalPoints; i++)
	{
		ULONG	ulDieARow = 0, ulDieACol = 0;
		LONG	lUserRow, lUserCol;
		LONG	lDieAMsPrX, lDieAMsPrY, lDieAScanX, lDieAScanY;
		if( GetScnFPCMapPosition(i, ulDieARow, ulDieACol, lUserRow, lUserCol) )
		{
			if( GetScnData(ulDieARow, ulDieACol, &lDieAScanX, &lDieAScanY)==FALSE )
			{
			//	to get its position by nearby normal scanned position. um not encoder. from scanned file
				if( GetDieValidScnPosn(ulDieARow, ulDieACol, 4, lDieAScanX, lDieAScanY)==FALSE )
				{
					continue;
				}
				SetScnData(ulDieARow, ulDieACol, lDieAScanX, lDieAScanY, ucReferGrade);
				szMsg.Format("FPC point %ld,%ld find a nearby position %ld,%ld",
					lUserRow, lUserCol, lDieAScanX, lDieAScanY);
				SetAlarmLog(szMsg);
			}
			if( GetPrescanWftPosn(ulDieARow, ulDieACol, lDieAMsPrX, lDieAMsPrY)==FALSE )
			{
				if( GetDieValidPrescanPosn(ulDieARow, ulDieACol, 4, lDieAMsPrX, lDieAMsPrY, FALSE)==FALSE )
				{
					continue;
				}
			}
			m_WaferMapWrapper.SetCurrentPosition(ulDieARow, ulDieACol);
			XY_SafeMoveTo(lDieAMsPrX, lDieAMsPrY);
			if( lPrDelay>0 )
				Sleep(lPrDelay);

			BOOL bFindReferInWafer = FALSE;
			if( m_bScnAlignUseRefer )
				bFindReferInWafer = SpiralSearchRegionReferDie(2, m_lScnAlignReferNo);
			else
				bFindReferInWafer = SpiralSearchRegionReferDie(2, m_lScnAlignReferNo);

			X_Sync();
			Y_Sync();
			GetEncoderValue();
			if( bFindReferInWafer==FALSE )
			{
				continue;
			}
			lDieAMsPrX = GetCurrX();
			lDieAMsPrY = GetCurrY();
			m_stScnAlnPoints[j].m_bDieAFound	= TRUE;
			m_stScnAlnPoints[j].m_lDieAMsPrX	= lDieAMsPrX;
			m_stScnAlnPoints[j].m_lDieAMsPrY	= lDieAMsPrY;
			m_stScnAlnPoints[j].m_lDieAScanX	= lDieAScanX;
			m_stScnAlnPoints[j].m_lDieAScanY	= lDieAScanY;
			m_stScnAlnPoints[j].m_ulDieARow		= ulDieARow;
			m_stScnAlnPoints[j].m_ulDieACol		= ulDieACol;

			BOOL bFindPointB = FALSE;
			ULONG	ulDieBRow = 0, ulDieBCol = 0;
			LONG	lDieBMsPrX = 0, lDieBMsPrY = 0, lDieBScanX = 0, lDieBScanY = 0;

#if	FIND_POINT_B
			if( ulDieARow>(ulNumRow/2) )
			{
				ulDieBRow = ulDieARow - m_lSCNIndex_Y;
			}
			else
			{
				ulDieBRow = ulDieARow + m_lSCNIndex_Y;
			}
			if( ulDieACol>(ulNumCol/2) )
			{
				ulDieBCol = ulDieACol - m_lSCNIndex_X;
			}
			else
			{
				ulDieBCol = ulDieACol + m_lSCNIndex_X;
			}
			if( GetScnData(ulDieBRow, ulDieBCol, &lDieBScanX, &lDieBScanY) )
			{
				bFindPointB = TRUE;
			}
			else
			{
				LONG lBRow = ulDieBRow;
				LONG lBCol = ulDieBCol ;
				for(LONG lTryRow=-1; lTryRow<=1; lTryRow++)
				{
					for(LONG lTryCol=-1; lTryCol<=1; lTryCol++)
					{
						ulDieBRow = lBRow + lTryRow;
						ulDieBCol = lBCol + lTryCol;
						if( GetScnData(ulDieBRow, ulDieBCol, &lDieBScanX, &lDieBScanY) )
						{
							bFindPointB = TRUE;
							break;
						}
					}
					if( bFindPointB )
					{
						break;
					}
				}
			}

			if( bFindPointB )
			{
			//	m_ulCurrentRow = ulDieARow;
			//	m_ulCurrentCol = ulDieACol;
			//	m_lCurrent_X = lDieAMsPrX;
			//	m_lCurrent_Y = lDieAMsPrY;
			//	m_WaferMapWrapper.SetCurrentPosition(ulDieBRow, ulDieBCol);
				LONG lDiffRow = (LONG)ulDieBRow - (LONG)ulDieARow;
				LONG lDiffCol = (LONG)ulDieBCol - (LONG)ulDieACol;
				bFindPointB = ScanLShapeDieB(lDiffRow, lDiffCol);
				Sleep(50);
				GetEncoderValue();
				lDieBMsPrX = GetCurrX();
				lDieBMsPrY = GetCurrY();
			//	LONG lIndex = 0;
			//	bFindPointB = ConvertScnPosnByFPC(ulDieBRow, ulDieBCol, lDieBMsPrX, lDieBMsPrY, lIndex, FALSE);
			//	if( GetPrescanWftPosn(ulDieBRow, ulDieBCol, , )==FALSE )
			//	{
			//		if( GetDieValidPrescanPosn(ulDieBRow, ulDieBCol, 4, lDieBMsPrX, lDieBMsPrY, FALSE)==FALSE )
			//		{
			//			continue;
			//		}
			//	}
			}
#endif
			if( bFindPointB )
			{
				DOUBLE	dAngleX, dAngleY;
				m_stScnAlnPoints[j].m_bAngleFound	= TRUE;
				CalculateES101Angle(lDieAScanX, lDieAScanY, lDieBScanX, lDieBScanY, dAngleX, dAngleY);			
				m_stScnAlnPoints[j].m_dScanAngle_X	= (dAngleX);
				m_stScnAlnPoints[j].m_dScanAngle_Y	= (dAngleY);
				CalculateES101Angle(lDieBMsPrX, lDieBMsPrY, lDieAMsPrX, lDieAMsPrY, dAngleX, dAngleY);			
				m_stScnAlnPoints[j].m_dMsPrAngle_X	= (dAngleX);
				m_stScnAlnPoints[j].m_dMsPrAngle_Y	= (dAngleY);
			}
			else
			{
				m_stScnAlnPoints[j].m_bAngleFound	= FALSE;
				m_stScnAlnPoints[j].m_dMsPrAngle_X	= m_dMS896Angle_X;
				m_stScnAlnPoints[j].m_dMsPrAngle_Y	= m_dMS896Angle_Y;
				m_stScnAlnPoints[j].m_dScanAngle_X	= m_dWS896Angle_X;
				m_stScnAlnPoints[j].m_dScanAngle_Y	= m_dWS896Angle_Y;
			}
			j++;
		}
	}

	for(INT j=1; j<WT_ALN_MAXCHECK_SCN; j++)
	{
		if( m_stScnAlnPoints[j].m_bDieAFound==FALSE )
		{
			continue;
		}
		if( m_stScnAlnPoints[j].m_bAngleFound )
		{
			continue;
		}
		LONG lDieAScanX = m_stScnAlnPoints[j].m_lDieAScanX;
		LONG lDieAScanY = m_stScnAlnPoints[j].m_lDieAScanY;
		LONG lDieAMsPrX = m_stScnAlnPoints[j].m_lDieAMsPrX;
		LONG lDieAMsPrY = m_stScnAlnPoints[j].m_lDieAMsPrY;
		LONG lDieARow	= m_stScnAlnPoints[j].m_ulDieARow;
		LONG lDieACol	= m_stScnAlnPoints[j].m_ulDieACol;

		DOUBLE dNearDist = -1;
		LONG lNearIndex = -1;
		for(INT i=0; i<WT_ALN_MAXCHECK_SCN; i++)
		{
			if( m_stScnAlnPoints[i].m_bDieAFound==FALSE )
			{
				continue;
			}
			LONG lDieBRow = m_stScnAlnPoints[i].m_ulDieARow;
			LONG lDieBCol = m_stScnAlnPoints[i].m_ulDieACol;
			if( labs(lDieARow-lDieBRow)>=m_lSCNIndex_Y &&
				labs(lDieACol-lDieBCol)>=m_lSCNIndex_X )
			{
				LONG lDieBScanX = m_stScnAlnPoints[i].m_lDieAScanX;
				LONG lDieBScanY = m_stScnAlnPoints[i].m_lDieAScanY;
				DOUBLE dDist = GetDistance(lDieAScanX, lDieAScanY, lDieBScanX, lDieBScanY);
				if( dNearDist==-1 || dNearDist>dDist)
				{
					dNearDist = dDist;
					lNearIndex = i;
				}
			}
		}
		if( lNearIndex!=-1 )
		{
			LONG lDieBScanX = m_stScnAlnPoints[lNearIndex].m_lDieAScanX;
			LONG lDieBScanY = m_stScnAlnPoints[lNearIndex].m_lDieAScanY;
			LONG lDieBMsPrX = m_stScnAlnPoints[lNearIndex].m_lDieAMsPrX;
			LONG lDieBMsPrY = m_stScnAlnPoints[lNearIndex].m_lDieAMsPrY;
			DOUBLE	dAngleX, dAngleY;
			CalculateES101Angle(lDieAScanX, lDieAScanY, lDieBScanX, lDieBScanY, dAngleX, dAngleY);			
			m_stScnAlnPoints[j].m_dScanAngle_X	= (dAngleX);
			m_stScnAlnPoints[j].m_dScanAngle_Y	= (dAngleY);
			CalculateES101Angle(lDieBMsPrX, lDieBMsPrY, lDieAMsPrX, lDieAMsPrY, dAngleX, dAngleY);			
			m_stScnAlnPoints[j].m_dMsPrAngle_X	= (dAngleX);
			m_stScnAlnPoints[j].m_dMsPrAngle_Y	= (dAngleY);
			m_stScnAlnPoints[j].m_bAngleFound	= TRUE;
		}
	}

	for(INT j=0; j<WT_ALN_MAXCHECK_SCN; j++)
	{
		if( m_stScnAlnPoints[j].m_bDieAFound==FALSE )
		{
			continue;
		}
		szMsg.Format("%2d,%3lu,%3lu,%8ld,%8ld,%8ld,%8ld,%f,%f,%f,%f",		//Klocwork	//v4.40T8
				j, m_stScnAlnPoints[j].m_ulDieARow, m_stScnAlnPoints[j].m_ulDieACol, 
				m_stScnAlnPoints[j].m_lDieAScanX, m_stScnAlnPoints[j].m_lDieAScanY,
				m_stScnAlnPoints[j].m_lDieAMsPrX, m_stScnAlnPoints[j].m_lDieAMsPrY,
				m_stScnAlnPoints[j].m_dScanAngle_X, m_stScnAlnPoints[j].m_dScanAngle_Y,
				m_stScnAlnPoints[j].m_dMsPrAngle_X, m_stScnAlnPoints[j].m_dMsPrAngle_Y);
		ScnAlignLog(szMsg);
	}

	ClearPrescanInfo();
	ResetMapPhyPosn();

	// set wafer scanner position to prescan result and run prescan and map physical postion
	for(ULONG ulRow=0; ulRow<=ulNumRow; ulRow++)
	{
		for(ULONG ulCol=0; ulCol<=ulNumCol; ulCol++)
		{
			LONG lPhyX = 0, lPhyY = 0, lScnIndex;
			if( ConvertScnPosnByFPC(ulRow, ulCol, lPhyX, lPhyY, lScnIndex, TRUE) )
			{
				SetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
				SetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY);
				szMsg.Format("%3lu,%3lu,%8ld,%8ld,%2ld", ulRow, ulCol, lPhyX, lPhyY, lScnIndex);
			//	ScnAlignLog(szMsg);
			}
		}
	}

	szMsg.Format("%ld ==> %f", 	m_lScnAlignTotalPoints, (GetTime() - dStartTime)/1000.0);
	ScnAlignLog(szMsg);
	CMSLogFileUtility::Instance()->WT_GetIdxLogClose();
}



BOOL CWaferTable::FivePointCheckDiePosition(VOID)
{
	LONG	i=0;
	LONG	lStatus = 0;
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
	ULONG	ulHomeDieRow, ulHomeDieCol;
	BOOL	bNullPointCheck = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG	laScnCheckRow[WT_ALN_MAXCHECK_SCN];
	LONG	laScnCheckCol[WT_ALN_MAXCHECK_SCN];
	INT		ErrCount = 0, ErrTemp = 0;
	CString szListErrorDie;
	m_lChangeLightSCNContinue = 0;

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	szTitle.LoadString(HMB_WT_CHECK_SCN_FUNC);

	BOOL bSemitek = pApp->GetCustomerName()=="Semitek";

	if( bSemitek && m_bManualAddReferPoints )	//	427TX	4
	{
		SaveScanTimeEvent("    WFT: Semitek add assist refer point, no 5 points check");
		return TRUE;
	}

	if( pUtl->GetPrescanRegionMode() || pApp->GetProductLine()=="ZJG" )
		bSemitek = FALSE;
	// validate the map file, check wether it is a null bin for home die
	UCHAR ucReferBin = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
	UCHAR	ucNullBin = m_WaferMapWrapper.GetNullBin();
	if (m_bCheckNullBinInMap == TRUE)
	{
		GetMapAlignHomeDie(ulHomeDieRow, ulHomeDieCol);
		UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulHomeDieRow, ulHomeDieCol);
		BOOL bIsReferDie = m_WaferMapWrapper.IsReferenceDie(ulHomeDieRow, ulHomeDieCol);
		if( (bIsReferDie==FALSE && ucGrade!=ucNullBin) || 
			(bIsReferDie && ucGrade!=ucReferBin) )
		{
			bNullPointCheck = FALSE;
			LONG lUserRow = 0, lUserCol = 0;
			LONG lHmiRow = 0, lHmiCol = 0;
			ConvertAsmToHmiUser(ulHomeDieRow, ulHomeDieCol, lHmiRow, lHmiCol);
			ConvertAsmToOrgUser(ulHomeDieRow, ulHomeDieCol, lUserRow, lUserCol);
			szText.Format("Die found on the map %ld %ld (%ld %ld) home position", lHmiRow, lHmiCol, lUserRow, lUserCol);
			
			HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			
			FivePointCheckLog(szText);

			return FALSE;
		}
	}

	LONG lScnFPCAlarmCount = 0;
	BOOL bAtLeastOneOk = FALSE;
	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	if (pWaferPr != NULL)
	{
		pWaferPr->ChangeDirectView(TRUE);
		//SelectWaferCamera();
	}
	//ChangeCameraToWafer();	// for five point check
	LONG lAlignFailureX = 0,lAlignFailureY = 0;
	for (i=1; i<=m_lTotalSCNCount; i++)
	{
		//v4.37T12
		/*
		szMsg.Format("5Pt-Check #%d - AlarmCount = %ld (%ld)", i, lScnFPCAlarmCount, m_lScnCheckAlarmLimit);
		FivePointCheckLog(szMsg);

		if( m_lScnCheckAlarmLimit>0 && (lScnFPCAlarmCount >= m_lScnCheckAlarmLimit) )	//v4.37T12	//Genesis	
		{
			szMsg.Format("Scan result check alarm %ld over limit %ld!\nneed ENGINEER to fix?", lScnFPCAlarmCount, m_lScnCheckAlarmLimit);
			FivePointCheckLog(szMsg);
			HmiMessage_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE);

			if( Check201277PasswordToGo() )
			{
				FivePointCheckLog("Scan check error over limit and engineer let continue.");
			}
			else
			{
				FivePointCheckLog("Scan check error over limit and no engineer password.");
				return FALSE;
			}
		}
		*/
		//ChangeCameraToWafer();	// for five point check @Matthew
		bPromptMessage = TRUE;	
		lStatus = WT_ALN_OK;
		lHmiStatus = glHMI_CONTINUE;

		i = min(i, WT_ALN_MAXCHECK_SCN-1);		//Klocwork	//v4.02T5

		// Always clear the flag (V3.26)
		if( GetFPCMapPosition(i, ulIntRow, ulIntCol, lScnCheckRow, lScnCheckCol)==FALSE )
		{
			laScnCheckRow[i] = lScnCheckRow;
			laScnCheckCol[i] = lScnCheckCol;
			szMsg.Format("Five Point Check %d row %ld, col %ld, out of map range", i, lScnCheckRow, lScnCheckCol);
			FivePointCheckLog(szMsg);
			if (GetErrorChooseGoFPC() )
			{
				lScnFPCAlarmCount++;
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
		laScnCheckRow[i] = lScnCheckRow;
		laScnCheckCol[i] = lScnCheckCol;

		if( pUtl->GetPrescanRegionMode() )
		{
			ULONG ulScanRegion = pSRInfo->GetTargetRegion();
			if( pSRInfo->IsWithinThisRegion(ulScanRegion, ulIntRow, ulIntCol)==FALSE )
				continue;
		}

		if( IsOutMS90SortingPart(ulIntRow, ulIntCol) )
		{
			continue;
		}

		//Call setcurrent pos twice because HMI need time to update
		m_WaferMapWrapper.SetCurrentPosition(ulIntRow, ulIntCol);
		Sleep(300);
		m_WaferMapWrapper.SetCurrentPosition(ulIntRow, ulIntCol);
		Sleep(600);

		szMsg.Format("%ld point %ld,%ld ==> %ld,%ld\n", i, lScnCheckRow, lScnCheckCol, ulIntRow, ulIntCol);
		FivePointCheckLog(szMsg);

		if (GetKeepScnAlignDie() == TRUE)
		{
			//Mark die & set die state to unpick mode	
			m_WaferMapWrapper.MarkDie(ulIntRow, ulIntCol, TRUE);
			ULONG ulDieState = m_WaferMapWrapper.GetDieState(ulIntRow, ulIntCol);
			if( !IsDieUnpickAll(ulDieState) )
				ulDieState = WT_MAP_DIESTATE_UNPICK;
			m_WaferMapWrapper.SetDieState(ulIntRow, ulIntCol, ulDieState);
		}

		lStatus = WT_ALN_MAP_NULL_BIN_OK;
		if (m_bCheckNullBinInMap == TRUE)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulIntRow, ulIntCol);
			BOOL  bIsReferDie = m_WaferMapWrapper.IsReferenceDie(ulIntRow, ulIntCol);
			if( (bIsReferDie==FALSE && ucGrade!=ucNullBin) || 
				(bIsReferDie && ucGrade!=ucReferBin) )
			{
				bNullPointCheck = FALSE;
				if (GetErrorChooseGoFPC() && i!=1 )
				{
					lScnFPCAlarmCount++;
					szText.Format("Die found on the map %ld %ld position, do you want to PR check?", lScnCheckRow, lScnCheckCol);
					lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CONTINUESTOP|0x80000000, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					FivePointCheckLog(szText);
				}
				else 
				{
					szText.Format("Die found on the map %ld %ld position", lScnCheckRow, lScnCheckCol);
					lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					FivePointCheckLog(szText);

					return FALSE;
				}

				if( lHmiStatus!=glHMI_CONTINUE )
				{
					lStatus = WT_ALN_MAP_NULL_BIN_NOTOK;
				}
			}	
		}

		if (lStatus == WT_ALN_MAP_NULL_BIN_OK)
		{
			//If no grade on select die pos, try to use surrounding die
			if( GetDieValidPrescanPosn(ulIntRow, ulIntCol, 1, lPhyX, lPhyY)==FALSE )
				lStatus = WT_ALN_SCN_NO_NAR_GRADE;
			else
				lStatus = WT_ALN_OK;
		}

		if (lStatus == WT_ALN_OK)
		{
			X_Sync();
			Y_Sync();
			Sleep(20);
			szMsg.Format("point %ld, wft(%ld,%ld)\n", i, lPhyX, lPhyY);		//Klocwork
			FivePointCheckLog(szMsg);
			if( XY_SafeMoveTo(lPhyX, lPhyY)==FALSE )
			{
				szText.Format("Point %d (%ld,%ld), table position (%ld,%ld), out of wafer limit",
					i, lScnCheckRow, lScnCheckCol, lPhyX, lPhyY); 
				FivePointCheckLog(szText);

				lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

				bFiveResult = FALSE;
				break;
			}
			Sleep(20);

			ErrTemp = ErrCount;
			bDiePrResult = ScnCheckDiePrResult(lScnCheckRow, lScnCheckCol, bPromptMessage, szText, szListErrorDie, ErrCount);
			if (ErrTemp != ErrCount)
			{
				lAlignFailureX = lPhyX;
				lAlignFailureY = lPhyY;
				szMsg.Format("Error, point %ld, remember encoder %ld,%ld\n", i, lAlignFailureX, lAlignFailureY);
				FivePointCheckLog(szMsg);
			}

			if( m_bScnCheckByPR )
			{
				(*m_psmfSRam)["MS896A"]["WaferMapRow"] = ulIntRow;
				(*m_psmfSRam)["MS896A"]["WaferMapCol"] = ulIntCol;
				(*m_psmfSRam)["MS896A"]["WaferTableX"] = lPhyX;
				(*m_psmfSRam)["MS896A"]["WaferTableY"] = lPhyY;

				bool bSaveImage = true;
				if( pApp->GetCustomerName()=="LatticePower" && i!=1 )
					bSaveImage = false;
				if( bSaveImage )
				{
					if( m_bScnCheckIsRefDie==FALSE )
					{
						GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_5PC);	// FIVE POINT CHECK
					}
					else
					{
						GrabAndSaveImage(1, m_lScnCheckRefDieNo, WPR_GRAB_SAVE_IMG_5PC);	// FIVE POINT CHECK
					}
				}
			}


			if (bDiePrResult == FALSE)
			{
				szMsg.Format("Five Point Check die (%ld,%ld) PR result fail", lScnCheckRow, lScnCheckCol);
				FivePointCheckLog(szMsg);
				bFiveResult = FALSE;
				break;
			}
			
			GetEncoderValue();
			SetMapPhyPosn(ulIntRow, ulIntCol, GetCurrX(), GetCurrY());
			szMsg.Format("point %ld, update table %ld,%ld\n", i, GetCurrX(), GetCurrY());
			FivePointCheckLog(szMsg);
		
		}
		else
		{
			if( bSemitek || (pApp->GetCustomerName()=="Semitek" && pUtl->GetPrescanRegionMode()) )
			{
				continue;
			}
		}


		if( (lStatus!=WT_ALN_OK) )
		{
			switch(lStatus)
			{
			case WT_ALN_SCN_NO_NAR_GRADE:
				szText.Format("No surrounding grade on %ld %ld position", lScnCheckRow, lScnCheckCol);
				break;

			// check whether the map is null grade at that position
			case WT_ALN_MAP_NULL_BIN_NOTOK:
				szText.Format("Die found on the map %ld %ld position", lScnCheckRow, lScnCheckCol);
				break;

			default:
				szText.Format("Unknown error on %ld %ld position", lScnCheckRow, lScnCheckCol);
				break;
			}

			if (GetErrorChooseGoFPC())
			{
				lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CONTINUESTOP|0x80000000, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			}
			else 
			{
				lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			}
			lScnFPCAlarmCount++;
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
		bAtLeastOneOk = TRUE;
	}


	//Ask user to continue or stop
	if (/* ( bPromptMessage == TRUE ) && */( ErrCount == 0 ) )
	{
/*
		if ( GetErrorChooseGoFPC() || m_bScnCheckByPR == FALSE )
		{
			lScnFPCAlarmCount++;
			lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CONTINUESTOP|0x80000000, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		}
		else
		{
			lHmiStatus = HmiMessage_Red_Back(szText, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		}
		FivePointCheckLog(szText);
*/
		FivePointCheckLog("Check SCN position OK");
	}
	else if ( ( ErrCount > 0 ) && (ErrCount > m_lScnCheckAlarmLimit) )	//Matthew 10312018
	{
		if (pApp->GetCustomerName()=="ChangeLight(XM)")
		{
			XY_SafeMoveTo(lAlignFailureX, lAlignFailureY);
			szMsg.Format("WT Move to last failure position,target(%d,%d),current(%d,%d)",lAlignFailureX,lAlignFailureY,GetCurrX(), GetCurrY());
			FivePointCheckLog(szMsg);
		}
		szMsg.Format("Scan result check alarm over limit! \n need ENGINEER to fix? \n ");
		lHmiStatus = SetAlert_Msg_Red_Back(IDS_WT_SCAN_CHECK_ERROR_OVER_LIMIT, szMsg,"Continue", "Stop", NULL, glHMI_ALIGN_CENTER);

		if (lHmiStatus == 1/* && pApp->CheckPasswordToGo(2)*/)
		{
			while (1)
			{
				BOOL bCancel = TRUE;
				if( pApp->CheckSananPasswordFile(TRUE, bCancel, 2) )
				{
					szMsg.Format("Scan result check alarm over limit!");
					SetAlert_Msg_Red_Back(IDS_WT_SCAN_CHECK_ERROR_OVER_LIMIT,szMsg);
					if (pApp->GetCustomerName()=="ChangeLight(XM)")
					{
						m_lChangeLightSCNContinue = 1;
					}
					szMsg.Format("Scan result check alarm %ld over limit %ld! \n need ENGINEER to fix? \n ", ErrCount, m_lScnCheckAlarmLimit);
					szListErrorDie = szMsg + szListErrorDie;
					SetErrorMessage(szListErrorDie);	//write to the Log
					szMsg = "Scan check error over limit and engineer let continue. \n";
					SetStatusMessage(szMsg);	
					SetErrorMessage(szMsg);	//write to the Log
					FivePointCheckLog(szMsg);
					break;
				}
				else
				{
					CString szAlarmMessage = "Wrong password, please approach engineer";
					HmiMessage_Red_Back(szAlarmMessage, "Wrong Password", glHMI_MBX_OK, glHMI_ALIGN_CENTER);

				}
		//		lHmiStatus = glHMI_CONTINUE;
			}
		}
		else
		{
			szMsg.Format("Scan result check alarm %ld over limit %ld! \n need ENGINEER to fix? \n ", ErrCount, m_lScnCheckAlarmLimit);
			szListErrorDie = szMsg + szListErrorDie;
			SetErrorMessage(szListErrorDie);	//write to the Log
			szMsg = "Scan check error over limit and no engineer password. \n";
			SetStatusMessage(szMsg);	
			SetErrorMessage(szMsg);	//write to the Log
			FivePointCheckLog(szMsg);
			return FALSE;
		}
	}
/*
	else
	{
		lHmiStatus = glHMI_CONTINUE;
		if (bSemitek )
		{
			szMsg = "Last point checking ok!";
			lHmiStatus = HmiMessage_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP|0x80000000, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
			FivePointCheckLog(szMsg);
		}
	}
*/

	if ((State() != IDLE_Q) && (pWaferPr != NULL))
	{
		pWaferPr->ChangeDirectView(FALSE);
	}

	if( bSemitek )
	{
		BOOL bFindRepeat = FALSE;
		int j;
		for (i=1; i<m_lTotalSCNCount; i++)
		{
			i = min(i, WT_ALN_MAXCHECK_SCN-1);
			lScnCheckRow = laScnCheckRow[i];
			lScnCheckCol = laScnCheckCol[i];
			for(j=i+1; j<=m_lTotalSCNCount; j++)
			{
				j = min(j, WT_ALN_MAXCHECK_SCN-1);
				if( lScnCheckRow==laScnCheckRow[j] &&
					lScnCheckCol==laScnCheckCol[j] )
				{
					bFindRepeat = TRUE;
					break;
				}
			}
			if( bFindRepeat==TRUE )
			{
				break;
			}
		}
		if( bAtLeastOneOk==FALSE )
		{
			bFiveResult = FALSE;
		}
		if( bFindRepeat==FALSE )
		{
			szMsg = "Please check your 5 point check and should have repeat points";
			HmiMessage_Red_Back(szMsg, "Five Points Check");
			FivePointCheckLog(szMsg);
			bFiveResult = FALSE;
		}
	}

	if( bNullPointCheck==FALSE )
	{
		szMsg = "Check SCN position is failed because grade die found";
		FivePointCheckLog(szMsg);
		bFiveResult = FALSE;
	}

	if( bFiveResult )
	{
		szMsg.Format("Five Point Check die finished successfully");
		FivePointCheckLog(szMsg);
	}

	return bFiveResult;
}


						  
LONG CWaferTable::UpdateScnAlignInfo(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		BOOL	bUpdate;
		LONG	lDieNo;
	} SCN_CHECK;

	SCN_CHECK	stPos;
	svMsg.GetMsg(sizeof(SCN_CHECK), &stPos);

	if (stPos.bUpdate == TRUE)
	{
		m_laScnAlignRow[stPos.lDieNo] = m_lScnHmiRow;
		m_laScnAlignCol[stPos.lDieNo] = m_lScnHmiCol;
		SaveData();
		SaveWaferTblData();
	}
	else
	{
		m_lScnHmiRow = m_laScnAlignRow[stPos.lDieNo];
		m_lScnHmiCol = m_laScnAlignCol[stPos.lDieNo];
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


BOOL CWaferTable::GetScnCheckAdjustOffset(LONG &lRefHomeDieRowOffset, LONG &lRefHomeDieColOffset)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bUseHmiMap = pApp->IsUseHmiMap4HomeFPC();

	lRefHomeDieRowOffset = lRefHomeDieColOffset = 0;

	if (m_bEnableScnCheckAdjust)
	{
		if( pApp->GetCustomerName()==_T("BYD") && m_bFindHomeDieFromMap )
		{
		}
		else
		{
			LONG	lRefHomeRow = 0, lRefHomeCol = 0;
			ULONG ulHomeDieRow, ulHomeDieCol;

			if( IsScnLoaded() )
			{
				GetScnRefDie(&ulHomeDieRow, &ulHomeDieCol);
			}
			else
			{
				GetAsmMapHomeDie(ulHomeDieRow, ulHomeDieCol);
			}
			if( bUseHmiMap )
				ConvertAsmToHmiUser(ulHomeDieRow, ulHomeDieCol, lRefHomeRow, lRefHomeCol);
			else
				ConvertAsmToOrgUser(ulHomeDieRow, ulHomeDieCol, lRefHomeRow, lRefHomeCol);

			m_lRefHomeDieRowOffset =  lRefHomeRow - m_lRefHomeDieRow;
			m_lRefHomeDieColOffset =  lRefHomeCol - m_lRefHomeDieCol;	
		}
			
		lRefHomeDieRowOffset = m_lRefHomeDieRowOffset;
		lRefHomeDieColOffset = m_lRefHomeDieColOffset;

		return TRUE;
	}

	return FALSE;
}

BOOL CWaferTable::GetScnFPCMapPosition(LONG lIndex, ULONG &ulAsmRow, ULONG &ulAsmCol, LONG &lUserRow, LONG &lUserCol)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bUseHmiMap = pApp->IsUseHmiMap4HomeFPC();
	if (lIndex >= WT_ALN_MAXCHECK_SCN)
	{
		return FALSE;
	}

	lUserRow = m_laScnAlignRow[lIndex];
	lUserCol = m_laScnAlignCol[lIndex];

	LONG lRefHomeDieRowOffset = 0;
	LONG lRefHomeDieColOffset = 0;
	if (GetScnCheckAdjustOffset(lRefHomeDieRowOffset, lRefHomeDieColOffset))
	{
		lUserRow = lUserRow + lRefHomeDieRowOffset;
		lUserCol = lUserCol + lRefHomeDieColOffset;
	}

	// covert map display (after rotation not in () ) to asm row/col
	BOOL bReturn = TRUE;
	if( bUseHmiMap )
		bReturn = ConvertHmiUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol);
	else
		bReturn = ConvertOrgUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol);

	return bReturn;
}

LONG CWaferTable::GoToAllSCNDiePos(IPC_CServiceMessage& svMsg)
{
	if( IsEnableFPC() )
	{
		FivePointCheckDiePosition();
	}

	return 1;
}

LONG CWaferTable::GoSampleCheckScnPoints(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=TRUE;
	if( IsAlignedWafer()==FALSE )
	{
		SetAlert(IDS_WT_MAP_NOT_ALIGN);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsPrescanEnable()==FALSE && IsScnLoaded()==FALSE )
	{
		HmiMessage("No scanned position available in this wafer!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsPrescanEnable() && IsPrescanning() )
	{
		HmiMessage("Prescan not complete yet!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	ChangeCameraToWafer();	// for five point check
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CTime stTime = CTime::GetCurrentTime();
	CString szTime = stTime.Format("%y%m%d%H%M%S.csv");
	CString szFileName;
	szFileName = "C:\\MapSorter\\UserData\\History\\ScnSample_" + szTime;
	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szFileName, "wt");
	ULONG ulGridRows = CP_GetGridRows();
	ULONG ulGridCols = CP_GetGridCols();
	if( GetDiePitchY_Y()!=0 )
		ulGridRows = ulGridCols * GetDiePitchX_X() / GetDiePitchY_Y();
	if( ulGridRows == 0 )
	{
		ulGridRows = 1;
	}


	LONG lUserRow = 0, lUserCol = 0;
	ULONG ulSampleCounter = 0, ulRowCounter = 0;
	CMS896AApp::m_bStopAlign = FALSE;
	LONG lPhyX = 0, lPhyY = 0, lNewX = 0, lNewY = 0;
	ULONG ulMaxCol = GetMapValidMinCol() + (GetMapValidMaxCol()-GetMapValidMinCol())/ulGridCols*ulGridCols;
	for (ULONG ulRow = (ULONG)GetMapValidMinRow(); ulRow <= (ULONG)GetMapValidMaxRow(); ulRow += ulGridRows)
	{
		if( pApp->IsStopAlign() )
		{
			break;
		}
		LONG ulCol = GetMapValidMinCol() - ulGridCols;
		if( ulRowCounter%2!=0 )
		{
			ulCol = ulMaxCol + ulGridCols;
		}
		while( 1 )
		{
			if( ulRowCounter%2==0 )
			{
				ulCol += ulGridCols;
				if ((ULONG)ulCol > ulMaxCol)
				{
					break;
				}
			}
			else
			{
				ulCol -= ulGridCols;
				if( ulCol<GetMapValidMinCol() )
				{
					break;
				}
			}
			if( pApp->IsStopAlign() )
			{
				break;
			}
			if( IsMapNullBin(ulRow, ulCol) )
			{
				continue;
			}

			//Call setcurrent pos twice because HMI need time to update
			m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);

			//If no grade on select die pos, try to use surrounding die
			if( GetDieValidPrescanPosn(ulRow, ulCol, 0, lPhyX, lPhyY)==FALSE )
			{
				continue;
			}

			X_Sync();
			Y_Sync();
			if( XY_SafeMoveTo(lPhyX, lPhyY)==FALSE )
			{
				continue;
			}
			Sleep(10);

			ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
			lNewX = lPhyX;
			lNewY = lPhyY;
			BOOL bSrch = WftSearchNormalDie(lNewX, lNewY, FALSE);
			if( bSrch==FALSE )
			{
				Sleep(500);
				bSrch = WftSearchNormalDie(lNewX, lNewY, FALSE);
			}

			LONG lOffsetX = lNewX-lPhyX;
			LONG lOffsetY = lNewY-lPhyY;
			BOOL bXCheck = CheckPitchX(lOffsetX);
			BOOL bYCheck = CheckPitchY(lOffsetY);
			if( bSrch==FALSE )
			{
				lOffsetX = 999;
				lOffsetY = 999;
				bXCheck = FALSE;
				bYCheck = FALSE;
			}

			if ((nErr == 0) && (fp != NULL))
			{
				fprintf(fp, "%4lu,%4ld,%4ld,%4ld,%8ld,%8ld,%4ld,%4ld,%d,%d\n",
					ulRow, ulCol, lUserRow, lUserCol, lPhyX, lPhyY, lOffsetX, lOffsetY, bXCheck, bYCheck);
			}
			ulSampleCounter++;
		}
		ulRowCounter++;
	}

	if( fp!=NULL )
	{
		fprintf(fp, "%lu\n", ulSampleCounter);
		fclose(fp);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::LoadAllScnCheckPosn(IPC_CServiceMessage& svMsg)
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

	CString szFilename = dlgFile.GetPathName();	// full path and name and ext. best.
	SaveScanTimeEvent("Load scan check file " + szFilename);

	if ((_access(szFilename, 0 )) == -1)
	{
		HmiMessage_Red_Back("Select file can not access!", "Scan check Wafer");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CStdioFile fScan;
	if( fScan.Open(szFilename, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE )
	{
		HmiMessage_Red_Back("Select file can not be open", "Scan Check Wafer");
		return FALSE;
	}

	fScan.SeekToBegin();
	CString szReading = "";
	CString szMsg = "", szTemp;

	memset(&m_lScnCheckRow, 0, sizeof(m_lScnCheckRow));
	memset(&m_lScnCheckCol, 0, sizeof(m_lScnCheckCol));

	m_lTotalSCNCount			= 0;

	while( fScan.ReadString(szReading)!=NULL )
	{
		CStringArray szaRawData;
		szaRawData.RemoveAll();
		CUtility::Instance()->ParseRawData(szReading, szaRawData);
		if( szaRawData.GetSize()>=2 )
		{
			m_lTotalSCNCount++;
			LONG lScanRow = (LONG)atoi( szaRawData.GetAt(0) );
			LONG lScanCol = (LONG)atoi( szaRawData.GetAt(1) );

			//Get original user coordinate
			m_lScnCheckRow[m_lTotalSCNCount] = lScanRow;
			m_lScnCheckCol[m_lTotalSCNCount] = lScanCol;

			szTemp.Format("[%d,%d]", lScanRow, lScanCol);
			szMsg += szTemp;
		}
		if( m_lTotalSCNCount>=(WT_ALN_MAXCHECK_SCN-1) )
		{
			break;
		}
	}

	m_lScnCheckRow[0] = m_lScnHmiRow = m_lScnCheckRow[1];
	m_lScnCheckCol[0] = m_lScnHmiCol = m_lScnCheckCol[1];

	szTemp.Format("Auto Load Scan Check Setting File total points %d", 	m_lTotalSCNCount);
	szMsg += szTemp;
	SetAlarmLog(szMsg);
	fScan.Close();

	SaveData();
	SaveWaferTblData();

	HmiMessage("Scan Check File loading complete");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::UpdateCheckSCNInfo(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		BOOL	bUpdate;
		LONG	lDieNo;
	} SCN_CHECK;

	SCN_CHECK	stPos;
	svMsg.GetMsg(sizeof(SCN_CHECK), &stPos);

	if (stPos.bUpdate == TRUE)
	{
		m_lScnCheckRow[stPos.lDieNo] = m_lScnHmiRow;
		m_lScnCheckCol[stPos.lDieNo] = m_lScnHmiCol;

		SaveData();
		SaveWaferTblData();
	}
	else
	{
		m_lScnHmiRow = m_lScnCheckRow[stPos.lDieNo];
		m_lScnHmiCol = m_lScnCheckCol[stPos.lDieNo];
        SetGemValue("AB_lScnResultDieNo", stPos.lDieNo); // SG_CEID_WT_SCNRESULT
	}
	m_lScnCheckRow[0] = m_lScnHmiRow;
	m_lScnCheckCol[0] = m_lScnHmiCol;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

    SendCE_ScanSetting();
	return 1;
}

BOOL CWaferTable::GetFPCMapPosition(LONG lIndex, ULONG &ulAsmRow, ULONG &ulAsmCol, LONG &lUserRow, LONG &lUserCol)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//BOOL bUseHmiMap = pApp->IsUseHmiMap4HomeFPC();
	lUserRow = m_lScnCheckRow[lIndex];
	lUserCol = m_lScnCheckCol[lIndex];
	
	LONG lRefHomeDieRowOffset = 0;
	LONG lRefHomeDieColOffset = 0;
	if (GetScnCheckAdjustOffset(lRefHomeDieRowOffset, lRefHomeDieColOffset))
	{
		lUserRow = lUserRow + lRefHomeDieRowOffset;
		lUserCol = lUserCol + lRefHomeDieColOffset;
	}

	// covert map display (after rotation not in () ) to asm row/col
	BOOL bReturn = TRUE;
//	if( bUseHmiMap )
//		bReturn = ConvertHmiUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol);
//	else
		bReturn = ConvertOrgUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol);

	return bReturn;
}

BOOL CWaferTable::GetDieValidScnPosn(LONG ulIntRow, LONG ulIntCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY)
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

	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if (GetScnData(lTmpRow, lTmpCol, &lPhyX, &lPhyY))
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
			if (GetScnData(lTmpRow, lTmpCol, &lPhyX, &lPhyY))
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
				if (GetScnData(lTmpRow, lTmpCol, &lPhyX, &lPhyY))
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
				if (GetScnData(lTmpRow, lTmpCol, &lPhyX, &lPhyY))
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
				if (GetScnData(lTmpRow, lTmpCol, &lPhyX, &lPhyY))
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
				if (GetScnData(lTmpRow, lTmpCol, &lPhyX, &lPhyY))
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
		LONG lDiePitchX_X = (LONG) ConvertMotorStepToUnit(GetDiePitchX_X()); 
		LONG lDiePitchX_Y = (LONG) ConvertMotorStepToUnit(GetDiePitchX_Y());
		LONG lDiePitchY_Y = (LONG) ConvertMotorStepToUnit(GetDiePitchY_Y());
		LONG lDiePitchY_X = (LONG) ConvertMotorStepToUnit(GetDiePitchY_X());

		LONG lDiff_X = ulIntCol - lTmpCol;
		LONG lDiff_Y = ulIntRow - lTmpRow;

		//Calculate original no grade die pos from surrounding die position
		BOOL bPhyStatus = FALSE;
		if (lTmpRow >= 0 && lTmpCol >= 0)
		{
			bPhyStatus = GetScnData(lTmpRow, lTmpCol, &lPhyX, &lPhyY);
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

VOID CWaferTable::ObtainMS_ES_Angle(DOUBLE dMsAB_X, DOUBLE dMsAB_Y, DOUBLE dEsAB_X, DOUBLE dEsAB_Y, DOUBLE &dSinTheta, DOUBLE &dCosTheta)
{
	DOUBLE dDist = (dEsAB_X*dEsAB_X+dEsAB_Y*dEsAB_Y);
	if( dDist==0 )
		dDist = 1.0;
	dSinTheta = (dMsAB_Y*dEsAB_X - dMsAB_X*dEsAB_Y)/dDist;
	dCosTheta = (dMsAB_X*dEsAB_X + dMsAB_Y*dEsAB_Y)/dDist;
}

BOOL CWaferTable::ConvertScanPosnByTheta(ULONG ulDieRow, ULONG ulDieCol, LONG &lPhyPosX, LONG &lPhyPosY)
{
	LONG	lDieScanX=0, lDieScanY=0;
	if( GetScnData(ulDieRow, ulDieCol, &lDieScanX, &lDieScanY)==FALSE )
	{
		return FALSE;
	}

	LONG lIndex = 0;
	DOUBLE dNearDist = -1.0;
	for(INT j=0; j<m_lScanPointIndex; j++)
	{
		LONG lScanX = m_lEsScanDieA_X[j];
		LONG lScanY = m_lEsScanDieA_Y[j];
		DOUBLE dDist = GetDistance(lScanX, lScanY, lDieScanX, lDieScanY);
		if( j==0 )
		{
			dNearDist = dDist;
			lIndex = j;
		}
		else
		{
			if( dDist<dNearDist )
			{
				dNearDist = dDist;
				lIndex = j;
			}
		}
	}

	DOUBLE dSinTheta	= m_dSinThetaTran[lIndex];
	DOUBLE dCosTheta	= m_dCosThetaTran[lIndex];

	//Tansform SCN position related to the new angle
	DOUBLE	dEsX	= (DOUBLE)ConvertUnitToMotorStep(lDieScanX - m_lEsScanDieA_X[lIndex]);
	DOUBLE	dEsY	= (DOUBLE)ConvertUnitToMotorStep(lDieScanY - m_lEsScanDieA_Y[lIndex]);

	//Re-Transform back to MS position
	lPhyPosX	= _round(m_lMsSrchDieA_X[lIndex] + dEsX*dCosTheta - dEsY*dSinTheta);
	lPhyPosY	= _round(m_lMsSrchDieA_Y[lIndex] + dEsX*dSinTheta + dEsY*dCosTheta);

	return TRUE;
}

LONG CWaferTable::AlignScannerWafer_Manual(VOID)
{
	CString szTitle, szContent;
	szTitle.LoadString(HMB_WT_ALIGN_WAFER);

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetAssistPointsNum()<2 )
	{
		HmiMessage("This mode needs you to add Refer Points firstly.", "Bar SCN align");
		return WT_ALN_ERROR;
	}

	UINT unScnPairs = pUtl->GetAssistPointsNum()/2; 
	m_lScanPointIndex = 0;
	for(ULONG i=0; i<unScnPairs; i++)
	{
		LONG lARow = 0, lACol = 0, lBRow = 0, lBCol = 0;
		LONG lAMsX = 0, lAMsY = 0, lBMsX = 0, lBMsY = 0;
		LONG lAEsX = 0, lAEsY = 0, lBEsX = 0, lBEsY = 0;

		pUtl->GetAssistPosition(i*2,   lARow, lACol, lAMsX, lAMsY);
		pUtl->GetAssistPosition(i*2+1, lBRow, lBCol, lBMsX, lBMsY);
		if( lARow==lBRow && lACol==lBCol )
		{
			szContent.Format("pair %d Refer Point A is same to Refer Point B", i+1);
			HmiMessage_Red_Yellow(szContent, szTitle);
			ScnAlignLog(szContent);
			continue;
		}

		//Get SCN data from wafermap function 
		if( GetScnData(lARow, lACol, &lAEsX, &lAEsY)==FALSE )
		{
			continue;
		}
		if( GetScnData(lBRow, lBCol, &lBEsX, &lBEsY)==FALSE )
		{
			continue;
		}

		if( m_lScanPointIndex==0 )
			m_ulScnDieA_Row = lARow;
		CString szMsg;
		szMsg.Format("MP point A %8ld,%8ld point B %8ld,%8ld",	lARow, lACol, lBRow, lBCol);
		ScnAlignLog(szMsg);
		szMsg.Format("MS point A %8ld,%8ld point B %8ld,%8ld",	lAMsX, lAMsY, lBMsX, lBMsY);
		ScnAlignLog(szMsg);
		szMsg.Format("ES point A %8ld,%8ld point B %8ld,%8ld",	lAEsX, lAEsY, lBEsX, lBEsY);
		ScnAlignLog(szMsg);
		DOUBLE dMsAB_X = lBMsX - lAMsX;
		DOUBLE dMsAB_Y = lBMsY - lAMsY;
		DOUBLE dEsAB_X = ConvertUnitToMotorStep(lBEsX - lAEsX);
		DOUBLE dEsAB_Y = ConvertUnitToMotorStep(lBEsY - lAEsY);
		DOUBLE dSinTheta = 0;
		DOUBLE dCosTheta = 1;
		ObtainMS_ES_Angle(dMsAB_X, dMsAB_Y, dEsAB_X, dEsAB_Y, dSinTheta, dCosTheta);

		szMsg.Format("MS point AB %f,%f ES point AB %f,%f, sin %f, cos %f",	
			dMsAB_X, dMsAB_Y, dEsAB_X, dEsAB_Y, dSinTheta, dCosTheta);
		ScnAlignLog(szMsg);

		m_lEsScanDieA_X[m_lScanPointIndex]	= lAEsX;
		m_lEsScanDieA_Y[m_lScanPointIndex]	= lAEsY;
		m_lMsSrchDieA_X[m_lScanPointIndex]	= lAMsX;
		m_lMsSrchDieA_Y[m_lScanPointIndex]	= lAMsY;
		m_dCosThetaTran[m_lScanPointIndex]	= dCosTheta;
		m_dSinThetaTran[m_lScanPointIndex]	= dSinTheta;
		m_lScanPointIndex++;

		m_lEsScanDieA_X[m_lScanPointIndex]	= lBEsX;
		m_lEsScanDieA_Y[m_lScanPointIndex]	= lBEsY;
		m_lMsSrchDieA_X[m_lScanPointIndex]	= lBMsX;
		m_lMsSrchDieA_Y[m_lScanPointIndex]	= lBMsY;
		m_dCosThetaTran[m_lScanPointIndex]	= dCosTheta;
		m_dSinThetaTran[m_lScanPointIndex]	= dSinTheta;
		m_lScanPointIndex++;

		// need to check the point A and B in the region, or loop to find the point A & B in this region.
		if( m_lScanPointIndex>=(SCN_ADD_REF_MAX-1) )
		{
			break;
		}
	}

	if( m_lScanPointIndex==0 )
	{
		szContent = "Refer Point A and Refer Point B are invalid.";
		HmiMessage_Red_Yellow(szContent, szTitle);
		ScnAlignLog(szContent);
		return WT_ALN_ERROR;
	}

	CString szMsg;

	ClearPrescanInfo();
	ResetMapPhyPosn();
	//Calculate WS896 & MS896 Angle
	ULONG ulNumRow = 0, ulNumCol = 0;
	m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);

	ULONG ulCtrCol = ulNumCol/2;

	CString szOcrFullName = m_szPrescanLogPath + ".OCR";	//	to get the ocr value file.
	if (m_bAoiOcrLoaded && _access(szOcrFullName, 0) != -1)
	{
		CStdioFile fpOcr;
		if( fpOcr.Open(szOcrFullName, CFile::modeRead|CFile::shareDenyNone) )
		{
			CString szReading;
			fpOcr.SeekToBegin();
			while( fpOcr.ReadString(szReading)!=NULL )
			{
				if( szReading.Find("Map Center")!=-1 )
				{
					CStringArray szaDataList;
					szaDataList.RemoveAll();
					CUtility::Instance()->ParseRawData(szReading, szaDataList);
					if( szaDataList.GetSize()>=3 )
					{
						LONG lUserRow = atoi(szaDataList.GetAt(1));
						LONG lUserCol = atoi(szaDataList.GetAt(2));
						ULONG ulAsmRow = 0, ulAsmCol = 0;
						if( ConvertOrgUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol) )
						{
							ulCtrCol = ulAsmCol;
						}
					}
					break;
				}
			}
		}
		fpOcr.Close();
	}

	// set wafer scanner position to prescan result and run prescan and map physical postion
	LONG lFirstCol[2], lFirstOffsetX[2], lFirstOffsetY[2];
	DOUBLE dDeltaX[2], dDeltaY[2];
	memset(lFirstCol,		0, sizeof(lFirstCol));
	memset(lFirstOffsetX,	0, sizeof(lFirstOffsetX));
	memset(lFirstOffsetY,	0, sizeof(lFirstOffsetY));
	memset(dDeltaX,			0, sizeof(dDeltaX));
	memset(dDeltaY,			0, sizeof(dDeltaY));
	for (LONG ulRow = (LONG)m_ulScnDieA_Row; ulRow <= (LONG)ulNumRow; ulRow++)
	{
		for(int i=0; i<2; i++)
		{
			LONG lStartCol = 0;
			LONG lEndCol = ulCtrCol;
			if( i==1 )
			{
				lStartCol = ulCtrCol;
				lEndCol = ulNumCol;
			}
			LONG lPhyX = 0, lPhyY = 0;
			BOOL bFindFirstOk = FALSE;
			// find first good die and get offset, location.
			for(LONG ulCol=lStartCol; ulCol<lEndCol; ulCol++)
			{
				if( ConvertScanPosnByTheta(ulRow, ulCol, lPhyX, lPhyY) )
				{
					m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
					LONG lCurrX = lPhyX, lCurrY = lPhyY;
					lCurrX = _round(lCurrX + lFirstOffsetX[i] + (ulCol-lFirstCol[i])*dDeltaX[i]);
					lCurrY = _round(lCurrY + lFirstOffsetY[i] + (ulCol-lFirstCol[i])*dDeltaY[i]);
					XY_SafeMoveTo(lCurrX, lCurrY);
					Sleep(10);
					szMsg.Format("map %d,%d position %d,%d to %d,%d", ulRow, ulCol, lPhyX, lPhyY, lCurrX, lCurrY);
					ScnAlignLog(szMsg);
					if( WftMoveSearchDie(lCurrX, lCurrY) )
					{
						bFindFirstOk = TRUE;
						lFirstCol[i] = ulCol;
						lFirstOffsetX[i] = lCurrX - lPhyX;
						lFirstOffsetY[i] = lCurrY - lPhyY;
						szMsg.Format("offset %d,%d ", lFirstOffsetX[i], lFirstOffsetY[i]);
						ScnAlignLog(szMsg);
						break;
					}
				}
			}
			// find last good die and get offset. calculate ratio
			for(LONG  ulCol=lEndCol; ulCol>lStartCol; ulCol--)
			{
				if( ConvertScanPosnByTheta(ulRow, ulCol, lPhyX, lPhyY) )
				{
					m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
					LONG lCurrX = lPhyX, lCurrY = lPhyY;
					lCurrX = _round(lCurrX + lFirstOffsetX[i] + (ulCol-lFirstCol[i])*dDeltaX[i]);
					lCurrY = _round(lCurrY + lFirstOffsetY[i] + (ulCol-lFirstCol[i])*dDeltaY[i]);
					XY_SafeMoveTo(lCurrX, lCurrY);
					Sleep(10);
					szMsg.Format("map %d,%d position %d,%d to %d,%d", ulRow, ulCol, lPhyX, lPhyY, lCurrX, lCurrY);
					ScnAlignLog(szMsg);
					if( WftMoveSearchDie(lCurrX, lCurrY) )
					{
						if( ulCol!=lFirstCol[i] && bFindFirstOk )
						{
							dDeltaX[i] = (DOUBLE)(lCurrX - lPhyX - lFirstOffsetX[i])/(DOUBLE)(ulCol-lFirstCol[i]);
							dDeltaY[i] = (DOUBLE)(lCurrY - lPhyY - lFirstOffsetY[i])/(DOUBLE)(ulCol-lFirstCol[i]);
							szMsg.Format("delta %f,%f", dDeltaX[i], dDeltaY[i]);
							ScnAlignLog(szMsg);
						}
						break;
					}
				}
			}

			// calculate die position in current row.
			for(LONG ulCol=lStartCol; ulCol<=lEndCol; ulCol++)
			{
				if( ConvertScanPosnByTheta(ulRow, ulCol, lPhyX, lPhyY) )
				{
					lPhyX = _round(lPhyX + lFirstOffsetX[i] + (ulCol-lFirstCol[i])*dDeltaX[i]);
					lPhyY = _round(lPhyY + lFirstOffsetY[i] + (ulCol-lFirstCol[i])*dDeltaY[i]);
					SetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
					SetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY);
				}
			}
		}
	}

	memset(lFirstCol,		0, sizeof(lFirstCol));
	memset(lFirstOffsetX,	0, sizeof(lFirstOffsetX));
	memset(lFirstOffsetY,	0, sizeof(lFirstOffsetY));
	memset(dDeltaX,			0, sizeof(dDeltaX));
	memset(dDeltaY,			0, sizeof(dDeltaY));
	for(LONG ulRow=(m_ulScnDieA_Row-1); ulRow>0; ulRow--)
	{
		for(int i=0; i<2; i++)
		{
			LONG lStartCol = 0;
			LONG lEndCol = ulCtrCol;
			if( i==1 )
			{
				lStartCol = ulCtrCol;
				lEndCol = ulNumCol;
			}
			LONG lPhyX = 0, lPhyY = 0;
			BOOL bFindFirstOk = FALSE;
			// find first good die and get offset, location.
			for(LONG ulCol=lStartCol; ulCol<lEndCol; ulCol++)
			{
				if( ConvertScanPosnByTheta(ulRow, ulCol, lPhyX, lPhyY) )
				{
					m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
					LONG lCurrX = lPhyX, lCurrY = lPhyY;
					lCurrX = _round(lCurrX + lFirstOffsetX[i] + (ulCol-lFirstCol[i])*dDeltaX[i]);
					lCurrY = _round(lCurrY + lFirstOffsetY[i] + (ulCol-lFirstCol[i])*dDeltaY[i]);
					XY_SafeMoveTo(lCurrX, lCurrY);
					Sleep(10);
					szMsg.Format("map %d,%d position %d,%d to %d,%d", ulRow, ulCol, lPhyX, lPhyY, lCurrX, lCurrY);
					ScnAlignLog(szMsg);
					if( WftMoveSearchDie(lCurrX, lCurrY) )
					{
						bFindFirstOk = TRUE;
						lFirstCol[i] = ulCol;
						lFirstOffsetX[i] = lCurrX - lPhyX;
						lFirstOffsetY[i] = lCurrY - lPhyY;
						szMsg.Format("offset %d,%d ", lFirstOffsetX[i], lFirstOffsetY[i]);
						ScnAlignLog(szMsg);
						break;
					}
				}
			}
			// find last good die and get offset. calculate ratio
			for(LONG  ulCol=lEndCol; ulCol>lStartCol; ulCol--)
			{
				if( ConvertScanPosnByTheta(ulRow, ulCol, lPhyX, lPhyY) )
				{
					m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
					LONG lCurrX = lPhyX, lCurrY = lPhyY;
					lCurrX = _round(lCurrX + lFirstOffsetX[i] + (ulCol-lFirstCol[i])*dDeltaX[i]);
					lCurrY = _round(lCurrY + lFirstOffsetY[i] + (ulCol-lFirstCol[i])*dDeltaY[i]);
					XY_SafeMoveTo(lCurrX, lCurrY);
					Sleep(10);
					szMsg.Format("map %d,%d position %d,%d to %d,%d", ulRow, ulCol, lPhyX, lPhyY, lCurrX, lCurrY);
					ScnAlignLog(szMsg);
					if( WftMoveSearchDie(lCurrX, lCurrY) )
					{
						if( ulCol!=lFirstCol[i] && bFindFirstOk )
						{
							dDeltaX[i] = (DOUBLE)(lCurrX - lPhyX - lFirstOffsetX[i])/(DOUBLE)(ulCol-lFirstCol[i]);
							dDeltaY[i] = (DOUBLE)(lCurrY - lPhyY - lFirstOffsetY[i])/(DOUBLE)(ulCol-lFirstCol[i]);
							szMsg.Format("delta %f,%f", dDeltaX[i], dDeltaY[i]);
							ScnAlignLog(szMsg);
						}
						break;
					}
				}
			}

			// calculate die position in current row.
			for(LONG ulCol=lStartCol; ulCol<=lEndCol; ulCol++)
			{
				if( ConvertScanPosnByTheta(ulRow, ulCol, lPhyX, lPhyY) )
				{
					lPhyX = _round(lPhyX + lFirstOffsetX[i] + (ulCol-lFirstCol[i])*dDeltaX[i]);
					lPhyY = _round(lPhyY + lFirstOffsetY[i] + (ulCol-lFirstCol[i])*dDeltaY[i]);
					SetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
					SetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY);
				}
			}
		}
	}

	HmiMessage_Red_Back("SCN wafer alignment done!", "SCN align");
	pUtl->DelAssistPoints();	// should add manual refer point after wafer alignment
	m_ulPrescanRefPoints = pUtl->GetAssistPointsNum();

	return WT_ALN_SCN_OK;		
}

LONG CWaferTable::AlignScannerWafer_Refer(VOID)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	CString szMsg;
	UINT iAPoint = 0, jBPoint = 1;
	ULONG lFarDist = 0;
	m_lScanPointIndex = 0;
	DOUBLE dTemp1 = 0, dTemp2 = 0;

	for(UINT i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		ULONG liFarDist = 0;
		UINT iiAPoint = 0, ijBPoint = 1;
		LONG liRow = 0, liCol = 0, liEncX = 0, liEncY = 0;
		LONG ljRow = 0, ljCol = 0, ljEncX = 0, ljEncY = 0;
		if( pUtl->GetReferPosition(i, liRow, liCol, liEncX, liEncY)==FALSE )
		{
			continue;
		}
		LONG lX = 0, lY = 0;
		BOOL bGetScnOK = GetScnData_Refer(liRow, liCol, lX, lY);
		szMsg.Format("Find Die %2d MS %4ld,%4ld,%8ld,%8ld, WS %8ld,%8ld", i+1, liRow, liCol, liEncX, liEncY, lX, lY);
		ScnAlignLog(szMsg);
		if( bGetScnOK==FALSE )
		{
			continue;
		}

		for(UINT j=0; j<pUtl->GetNumOfReferPoints(); j++)
		{
			if( pUtl->GetReferPosition(j, ljRow, ljCol, ljEncX, ljEncY)==FALSE )
			{
				continue;
			}
			if( GetScnData_Refer(ljRow, ljCol, lX, lY)==FALSE )
			{
				continue;
			}

			dTemp1	= (DOUBLE) (labs(ljEncY-liEncY));
			dTemp2	= (DOUBLE) (labs(ljEncX-liEncX));
			//ULONG lCurDist  = (ULONG) sqrt(pow(labs(ljEncY-liEncY), 2.0) + pow(labs(ljEncX-liEncX), 2.0));
			ULONG lCurDist  = (ULONG) sqrt(pow(dTemp1, 2.0) + pow(dTemp2, 2.0));
			if( lCurDist>lFarDist )
			{
				lFarDist = lCurDist;
				iAPoint = i;
				jBPoint = j;
			}
			if( lCurDist>liFarDist )
			{
				liFarDist = lCurDist;
				iiAPoint = i;
				ijBPoint = j;
			}
		}

		LONG liARow = 0, liACol = 0, liAWftX = 0, liAWftY = 0, liAScnX = 0, liAScnY = 0;
		LONG liBRow = 0, liBCol = 0, liBWftX = 0, liBWftY = 0, liBScnX = 0, liBScnY = 0;
		DOUBLE	dEsAngleX = 0, dEsAngleY = 1, dMsAngleX = 0, dMsAngleY = 1;

		pUtl->GetReferPosition(iiAPoint, liARow, liACol, liAWftX, liAWftY);
		pUtl->GetReferPosition(ijBPoint, liBRow, liBCol, liBWftX, liBWftY);
		LONG lThetaAX = liAWftX;
		LONG lThetaAY = liAWftY;
		LONG lThetaBX = liBWftX;
		LONG lThetaBY = liBWftY;
#if	TABLE_ANGLE_ALL	//	refer die, calcualte angle
#endif
		CalculateES101Angle(lThetaBX, lThetaBY, lThetaAX, lThetaAY, dMsAngleX, dMsAngleY);	
		GetScnData_Refer(liARow, liACol, liAScnX, liAScnY);
		GetScnData_Refer(liBRow, liBCol, liBScnX, liBScnY);
		CalculateES101Angle(liAScnX, liAScnY, liBScnX, liBScnY, dEsAngleX, dEsAngleY);			

		m_lScanDieA_X[m_lScanPointIndex] = liAScnX;
		m_lScanDieA_Y[m_lScanPointIndex] = liAScnY;
		m_lMsPrDieA_X[m_lScanPointIndex] = liAWftX;
		m_lMsPrDieA_Y[m_lScanPointIndex] = liAWftY;
		m_dESAngle_X[m_lScanPointIndex]	= dEsAngleX;
		m_dESAngle_Y[m_lScanPointIndex]	= dEsAngleY;
		m_dMSAngle_X[m_lScanPointIndex]	= dMsAngleX;
		m_dMSAngle_Y[m_lScanPointIndex]	= dMsAngleY;
		m_lScanPointIndex++;
	}

	LONG lARow = 0, lACol = 0, lAWfX = 0, lAWfY = 0;
	LONG lBRow = 0, lBCol = 0, lBWfX = 0, lBWfY = 0;
	pUtl->GetReferPosition(iAPoint, lARow, lACol, lAWfX, lAWfY);
	pUtl->GetReferPosition(jBPoint, lBRow, lBCol, lBWfX, lBWfY);
#if	TABLE_ANGLE_ALL	//	refer die, farest die
#endif
	//Store SCN Align Die A data
	m_ulScnDieA_Row = lARow;
	m_ulScnDieA_Col = lACol;
	m_lMsDieA_X		= lAWfX;
	m_lMsDieA_Y		= lAWfY;
	//Store SCN Align Die B data
	m_ulScnDieB_Row = lBRow;
	m_ulScnDieB_Col = lBCol;
	m_lMsDieB_X		= lBWfX;
	m_lMsDieB_Y		= lBWfY;

	szMsg.Format("Find Die Far A %4ld,%4ld,%8ld,%8ld ==> %8ld,%8ld", lARow, lACol, lAWfX, lAWfY, m_lMsDieA_X, m_lMsDieA_Y);
	ScnAlignLog(szMsg);
	szMsg.Format("Find Die Far B %4ld,%4ld,%8ld,%8ld ==> %8ld,%8ld", lBRow, lBCol, lBWfX, lBWfY, m_lMsDieB_X, m_lMsDieB_Y);
	ScnAlignLog(szMsg);

	//Calculate WS896 & MS896 Angle
	LONG	lAX = 0, lAY = 0, lBX = 0, lBY = 0;
	//Get SCN data from wafermap function 
	if( GetScnData_Refer(m_ulScnDieA_Row, m_ulScnDieA_Col, lAX, lAY) )
	{
		m_dScnDieA_X = lAX;
		m_dScnDieA_Y = lAY;
		ScnAlignLog("Get WS A point ok");
	}
	else
	{
		HmiMessage_Red_Back("Align point A has no SCN information.");
		return WT_ALN_ERROR;		
	}

	if( GetScnData_Refer(m_ulScnDieB_Row, m_ulScnDieB_Col, lBX, lBY) )
	{
		ScnAlignLog("Get WS B point ok");
	}
	else
	{
		HmiMessage_Red_Back("Align point B has no SCN information.");
		return WT_ALN_ERROR;		
	}

	//Calculate angle
	LONG dDistX	= m_dScnDieA_X - lBX;
	LONG dDistY	= m_dScnDieA_Y - lBY;
	DOUBLE dTemp = (DOUBLE) (dDistX*dDistX) + (dDistY*dDistY);		//v4.59A45
	//LONG dDistXY = sqrt((dDistX*dDistX) + (dDistY*dDistY));
	DOUBLE dDistXY = sqrt(dTemp);
	if (dDistXY < 1.0)
	{
		dDistXY = 1.0;
	}

	szMsg.Format("WS die A (%4d,%4d) %8d,%8d, die B (%4d,%4d) %8d,%8d dist x %ld, dist y %ld, dist %ld",
		m_ulScnDieA_Row, m_ulScnDieA_Col, m_dScnDieA_X, m_dScnDieA_Y, m_ulScnDieB_Row, m_ulScnDieB_Col, lBX, lBY, 
		dDistX, dDistY, dDistXY);
	ScnAlignLog(szMsg);

	switch( m_lScnAlignMethod )
	{
	case 2:	// first to check FPC points, then move from inner to outer to get local factors.
		ScnAlignSpiralToOuter();	//	orbit
		break;
	case 1:
		ScanAlignWithFivePoints();
		break;
	case 0:
	default:
		break;
	}

	// set wafer scanner position to prescan result and run prescan and map physical postion
	ConvertWSDataToMS_Refer();

	ScnAlignLog("Now SCN alignment and calculate done\n");
	return WT_ALN_SCN_OK;		
}

VOID CWaferTable::ConvertWSDataToPrescanAndMap()
{
	ClearPrescanInfo();
	ResetMapPhyPosn();

	// set wafer scanner position to prescan result and run prescan and map physical postion
	for(LONG ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
	{
		for(LONG ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
		{
			LONG lPhyY = 0, lPhyX = 0;
		//	if( ConvertPhysicalPos(ulRow, ulCol, &lPhyX, &lPhyY) )
			if( ConvertPhysicalPos_Refer(ulRow, ulCol, lPhyX, lPhyY) )
			{
				SetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
				m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lPhyX, lPhyY);
			}
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();
}

BOOL CWaferTable::ConvertScanPosnByNear(ULONG ulDieRow, ULONG ulDieCol, LONG &lPhyPosX, LONG &lPhyPosY)
{
	LONG	lDieScanX=0, lDieScanY=0;
	if( GetScnData_Refer(ulDieRow, ulDieCol, lDieScanX, lDieScanY)==FALSE )
	{
		return FALSE;
	}

	LONG lIndex = 0;
	DOUBLE dNearDist = GetDistance(m_dScnDieA_X, m_dScnDieA_Y, lDieScanX, lDieScanY);
	for(INT j=0; j<m_lScanPointIndex; j++)
	{
		LONG lScanX = m_lScanDieA_X[j];
		LONG lScanY = m_lScanDieA_Y[j];
		DOUBLE dDist = GetDistance(lDieScanX, lDieScanY, lScanX, lScanY);
		if( dDist<dNearDist )
		{
			dNearDist = dDist;
			lIndex = j;
		}
	}

	DOUBLE dScanAngle_X		= m_dESAngle_X[lIndex];
	DOUBLE dScanAngle_Y		= m_dESAngle_Y[lIndex];
	DOUBLE dMsPrAngle_X		= m_dMSAngle_X[lIndex];
	DOUBLE dMsPrAngle_Y		= m_dMSAngle_Y[lIndex];
	LONG	lScanDieA_X		= m_lScanDieA_X[lIndex];
	LONG	lScanDieA_Y		= m_lScanDieA_Y[lIndex];
	LONG	lMsPrDieA_X		= m_lMsPrDieA_X[lIndex];
	LONG	lMsPrDieA_Y		= m_lMsPrDieA_Y[lIndex];

	//Tansform SCN position related to angle	
	DOUBLE	dTemp1X	= (DOUBLE)(lDieScanX - lScanDieA_X);
	DOUBLE	dTemp1Y	= (DOUBLE)(lDieScanY - lScanDieA_Y);
	DOUBLE	dTemp2X	= (dTemp1X * dScanAngle_X) + (dTemp1Y * dScanAngle_Y);
	DOUBLE	dTemp2Y	= (dTemp1Y * dScanAngle_X) - (dTemp1X * dScanAngle_Y);

	//Re-Transform back to MS896 position
	dTemp1X		= (dTemp2X * dMsPrAngle_X) - (dTemp2Y * dMsPrAngle_Y);
	dTemp1Y		= (dTemp2X * dMsPrAngle_Y) + (dTemp2Y * dMsPrAngle_X);
	DOUBLE dAlnTranX = ConvertUnitToMotorStep(dTemp1X);
	DOUBLE dAlnTranY = ConvertUnitToMotorStep(dTemp1Y);
	DOUBLE dCosTheta	= 1.0;
	if( cos(m_dAlnTableTheta)!=0 )
		dCosTheta		= cos(m_dAlnTableTheta);
#if	TABLE_ANGLE_NEW
	DOUBLE dAlnPosnX	= lMsPrDieA_X - dAlnTranX;
	DOUBLE dAlnPosnY	= lMsPrDieA_Y - dAlnTranY;
	DOUBLE dAlnLastX	= dAlnPosnX - dAlnPosnY * tan(m_dAlnTableTheta);
	DOUBLE dAlnLastY	= dAlnPosnY / dCosTheta;
#else
	DOUBLE dAlnPosnX	= dAlnTranX - dAlnTranY * tan(m_dAlnTableTheta);
	DOUBLE dAlnPosnY	= dAlnTranY / dCosTheta;
	DOUBLE dAlnLastX	= lMsPrDieA_X - dAlnPosnX;
	DOUBLE dAlnLastY	= lMsPrDieA_Y - dAlnPosnY;
#endif
	lPhyPosX	= (LONG) dAlnLastX;
	lPhyPosY	= (LONG) dAlnLastY;

	return TRUE;
}

VOID CWaferTable::ConvertWSDataToMS_Refer()
{
	ClearPrescanInfo();
	ResetMapPhyPosn();

	// set wafer scanner position to prescan result and run prescan and map physical postion
	for(LONG ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
	{
		for(LONG ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
		{
			LONG lPhyX = 0, lPhyY = 0;
			if( ConvertScanPosnByNear(ulRow, ulCol, lPhyX, lPhyY) )
			{
				SetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
				m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lPhyX, lPhyY);
			}
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();
}

BOOL CWaferTable::ConvertPhysicalPos_Refer(ULONG ulCurrRow, ULONG ulCurrCol, LONG &lPhyPosX, LONG &lPhyPosY)
{
	LONG lX = 0, lY = 0;
	if( GetScnData(ulCurrRow, ulCurrCol, &lX, &lY)==FALSE )
	{
		return FALSE;
	}

	DOUBLE dDistRatio = 1.0;
	//Tansform SCN position related to angle	
	DOUBLE dScnPosnX	= ((DOUBLE)lX - m_dScnDieA_X) * dDistRatio;
	DOUBLE dScnPosnY	= ((DOUBLE)lY - m_dScnDieA_Y) * dDistRatio;
	DOUBLE dScnTranX	= (dScnPosnX * m_dWS896Angle_X) + (dScnPosnY * m_dWS896Angle_Y);
	DOUBLE dScnTranY	= (dScnPosnY * m_dWS896Angle_X) - (dScnPosnX * m_dWS896Angle_Y);
	//Re-Transform back to MS896 position
	DOUBLE dAlnTranX	= (dScnTranX * m_dMS896Angle_X) - (dScnTranY * m_dMS896Angle_Y);
	DOUBLE dAlnTranY	= (dScnTranX * m_dMS896Angle_Y) + (dScnTranY * m_dMS896Angle_X);
	dAlnTranX = ConvertUnitToMotorStep(dAlnTranX);
	dAlnTranY = ConvertUnitToMotorStep(dAlnTranY);
	DOUBLE dCosTheta	= 1.0;
	if( cos(m_dAlnTableTheta)!=0 )
		dCosTheta		= cos(m_dAlnTableTheta);
#if	TABLE_ANGLE_NEW
	DOUBLE dAlnPosnX	= m_lMsDieA_X - dAlnTranX;
	DOUBLE dAlnPosnY	= m_lMsDieA_Y - dAlnTranY;
	DOUBLE dAlnLastX	= dAlnPosnX - dAlnPosnY * tan(m_dAlnTableTheta);
	DOUBLE dAlnLastY	= dAlnPosnY / dCosTheta;
#else
	DOUBLE dAlnPosnX	= dAlnTranX - dAlnTranY * tan(m_dAlnTableTheta);
	DOUBLE dAlnPosnY	= dAlnTranY / dCosTheta;
	DOUBLE dAlnLastX	= m_lMsDieA_X - dAlnPosnX;
	DOUBLE dAlnLastY	= m_lMsDieA_Y - dAlnPosnY;
#endif

	lPhyPosX	= (LONG) dAlnLastX;
	lPhyPosY	= (LONG) dAlnLastY;

	return TRUE;
}

VOID CWaferTable::ScnAlignSpiralToOuter()
{
	LONG lGridRow = CP_GetGridRows();
	LONG lGridCol = CP_GetGridCols();
	if( GetDiePitchX_X()!=0 )
		lGridCol = lGridRow * GetDiePitchY_Y()/GetDiePitchX_X();
	if( lGridCol==0 )
		lGridCol = 1;

	ULONG ulGridLoop = min(lGridCol, lGridRow)/2;
	LONG lTgtRow = m_ulScnDieA_Row;
	LONG lTgtCol = m_ulScnDieA_Col;

	ULONG ulRowUpLoop = labs(GetMapValidMinRow() - lTgtRow)/lGridRow;
	ULONG ulRowDnLoop = labs(GetMapValidMaxRow() - lTgtRow)/lGridRow;
	ULONG ulColLtLoop = labs(GetMapValidMinCol() - lTgtCol)/lGridCol;
	ULONG ulColRtLoop = labs(GetMapValidMaxCol() - lTgtCol)/lGridCol;
	ULONG ulMaxLoop = 0;
	ulMaxLoop = max(ulMaxLoop, ulRowUpLoop);
	ulMaxLoop = max(ulMaxLoop, ulRowDnLoop);
	ulMaxLoop = max(ulMaxLoop, ulColLtLoop);
	ulMaxLoop = max(ulMaxLoop, ulColRtLoop);

	CString szMsg;
	szMsg.Format("Inner %ld,%ld to outer; %ld,%ld, block loop %d(4) max loop is %d",
		lTgtRow, lTgtCol, lGridRow, lGridCol, ulGridLoop, ulMaxLoop);
	ScnAlignLog(szMsg);
	ulGridLoop = 4;
	for (ULONG ulLoop = 1; ulLoop <= ulMaxLoop; ulLoop++)
	{
		LONG lMaxIndex = ulLoop * 2;

		//Move to RIGHT & PR search on current die
		lTgtCol += lGridCol;
		ScnAlignPoint_Refer(lTgtRow, lTgtCol, ulGridLoop);

		// Right side Move to UP & PR search on current die
		for(LONG lIndex=0; lIndex < (lMaxIndex - 1); lIndex++)
		{
			lTgtRow -= lGridRow;
			ScnAlignPoint_Refer(lTgtRow, lTgtCol, ulGridLoop);
		}

		//up side Move to LEFT & PR search on current die
		for(LONG lIndex=0; lIndex < (lMaxIndex); lIndex++)
		{
			lTgtCol -= lGridCol;
			ScnAlignPoint_Refer(lTgtRow, lTgtCol, ulGridLoop);
		}

		// left side Move to DOWN & PR search on current die
		for(LONG lIndex=0; lIndex < lMaxIndex ; lIndex++)
		{
			lTgtRow += lGridRow;
			ScnAlignPoint_Refer(lTgtRow, lTgtCol, ulGridLoop);
		}

		// down side Move to DOWN & PR search on current die
		for(LONG lIndex=0; lIndex < (lMaxIndex); lIndex++)
		{
			lTgtCol += lGridCol;
			ScnAlignPoint_Refer(lTgtRow, lTgtCol, ulGridLoop);
		}
	}

	for(INT j=0; j<m_lScanPointIndex; j++)
	{
		szMsg.Format("SCN point %3d,%8ld,%8ld,%f,%f,%f,%f",	
			j, m_lMsPrDieA_X[j], m_lMsPrDieA_Y[j], m_dESAngle_X[j], m_dESAngle_Y[j], m_dMSAngle_X[j], m_dMSAngle_Y[j]);
		ScnAlignLog(szMsg);
	}
}

BOOL CWaferTable::CheckAndFindTargetDie(CONST LONG lTgtRow, CONST LONG lTgtCol, LONG &lTgtX, LONG &lTgtY)
{
	LONG lCurPhX = lTgtX;
	LONG lCurPhY = lTgtY;

	if( XY_SafeMoveTo(lCurPhX, lCurPhY)==FALSE )
	{
		return FALSE;
	}
	Sleep(10);
	if( WftSearchNormalDie(lCurPhX, lCurPhY, FALSE) )
	{
		lTgtX = lCurPhX;
		lTgtY = lCurPhY;
		return TRUE;
	}

	return FALSE;	//	no need to auto look around as this may lead to a wrong position
	
	REF_TYPE	stInfo;
	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "AutoLookAroundNearDie", stMsg);
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

	if( (stInfo.bStatus) && (stInfo.bFullDie) )	// PR error	// empty die
	{
		lCurPhX += stInfo.lX;	
		lCurPhX += stInfo.lY;

		if( WftMoveSearchDie(lCurPhX, lCurPhY) )
		{
			lTgtX = lCurPhX;
			lTgtY = lCurPhY;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CWaferTable::ScnAlignPoint_Refer(LONG ulDieRow, LONG ulDieCol, ULONG ulGridLoop)
{
	if( ulDieRow<0 || ulDieCol<0 )
	{
		return FALSE;
	}

	if( IsInMapValidRange(ulDieRow, ulDieCol)==FALSE )
	{
		return FALSE;
	}

//	if( IsInnerDie(ulDieRow, ulDieCol, 3, 5)==FALSE )
//	{
//		return FALSE;
//	}

	LONG lTgtRow = ulDieRow;
	LONG lTgtCol = ulDieCol;
	BOOL bFindPointA = FALSE;

	LONG lGridCounter = 0;
	m_WaferMapWrapper.SetCurrentPosition(lTgtRow, lTgtCol);
	LONG lDieScanAX = 0, lDieScanAY = 0, lDieMsPrAX = 0, lDieMsPrAY = 0;
	for (ULONG ulLoop = 0; ulLoop <= ulGridLoop; ulLoop++)
	{
		LONG lMaxIndex = ulLoop * 2;

		//Move to RIGHT & PR search on current die
		if( ulLoop!=0 )
			lTgtCol += 1;
		if( GetScnData_Refer(lTgtRow, lTgtCol, lDieScanAX, lDieScanAY) &&
			ConvertScanPosnByNear(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
		{
			lGridCounter++;
			m_WaferMapWrapper.SetCurrentPosition(lTgtRow, lTgtCol);
			if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
			{
				bFindPointA = TRUE;
				break;
			}
		}
		if( bFindPointA )
		{
			break;
		}
		if( lGridCounter>=20 )
		{
			break;
		}

		// Right side Move to UP & PR search on current die
		for(LONG lIndex=0; lIndex < (lMaxIndex - 1); lIndex++)
		{
			lTgtRow -= 1;
			if( GetScnData_Refer(lTgtRow, lTgtCol, lDieScanAX, lDieScanAY) &&
				ConvertScanPosnByNear(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
			{
				lGridCounter++;
				m_WaferMapWrapper.SetCurrentPosition(lTgtRow, lTgtCol);
				if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
				{
					bFindPointA = TRUE;
					break;
				}
			}
		}
		if( bFindPointA )
		{
			break;
		}
		if( lGridCounter>=20 )
		{
			break;
		}

		//up side Move to LEFT & PR search on current die
		for(LONG lIndex=0; lIndex < lMaxIndex; lIndex++)
		{
			lTgtCol -= 1;
			if( GetScnData_Refer(lTgtRow, lTgtCol, lDieScanAX, lDieScanAY) &&
				ConvertScanPosnByNear(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
			{
				lGridCounter++;
				m_WaferMapWrapper.SetCurrentPosition(lTgtRow, lTgtCol);
				if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
				{
					bFindPointA = TRUE;
					break;
				}
			}
		}
		if( bFindPointA )
		{
			break;
		}
		if( lGridCounter>=20 )
		{
			break;
		}

		// left side Move to DOWN & PR search on current die
		for(LONG lIndex=0; lIndex < lMaxIndex ; lIndex++)
		{
			lTgtRow += 1;
			if( GetScnData_Refer(lTgtRow, lTgtCol, lDieScanAX, lDieScanAY) &&
				ConvertScanPosnByNear(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
			{
				lGridCounter++;
				m_WaferMapWrapper.SetCurrentPosition(lTgtRow, lTgtCol);
				if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
				{
					bFindPointA = TRUE;
					break;
				}
			}
		}
		if( bFindPointA )
		{
			break;
		}
		if( lGridCounter>=20 )
		{
			break;
		}

		// down side Move to DOWN & PR search on current die
		for(LONG lIndex=0; lIndex < lMaxIndex; lIndex++)
		{
			lTgtCol += 1;
			if( GetScnData_Refer(lTgtRow, lTgtCol, lDieScanAX, lDieScanAY) &&
				ConvertScanPosnByNear(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
			{
				lGridCounter++;
				m_WaferMapWrapper.SetCurrentPosition(lTgtRow, lTgtCol);
				if( CheckAndFindTargetDie(lTgtRow, lTgtCol, lDieMsPrAX, lDieMsPrAY) )
				{
					bFindPointA = TRUE;
					break;
				}
			}
		}
		if( bFindPointA )
		{
			break;
		}
		if( lGridCounter>=20 )
		{
			break;
		}
	}

	if( bFindPointA==FALSE )
	{
		return FALSE;
	}

#if	TABLE_ANGLE_ALL	//	jump to find base point
#endif

	LONG	lDieMsPrBX = m_lMsDieA_X;
	LONG	lDieMsPrBY = m_lMsDieA_Y;
	LONG	lDieScanBX = m_dScnDieA_X;
	LONG	lDieScanBY = m_dScnDieA_Y;
	//Get SCN data from wafermap function 
	DOUBLE dDistA = GetDistance(m_lMsDieA_X, m_lMsDieA_Y, lDieMsPrAX, lDieMsPrAY);
	DOUBLE dDistB = GetDistance(m_lMsDieB_X, m_lMsDieB_Y, lDieMsPrAX, lDieMsPrAY);
	if( dDistB>dDistA )
	{
		lDieMsPrBX = m_lMsDieB_X;
		lDieMsPrBY = m_lMsDieB_Y;
		LONG lBX = 0, lBY = 0;
		GetScnData_Refer(m_ulScnDieB_Row, m_ulScnDieB_Col, lBX, lBY);
		lDieScanBX = lBX;
		lDieScanBY = lBY;
	}

	/*
#if	0	//	find the farest, but may be not as still finding.
	DOUBLE dFarDist = -1;
	LONG lFarIndex = -1;
	for(INT j=0; j<m_lScanPointIndex; j++)
	{
		LONG lMsBX = m_lMsPrDieA_X[j];
		LONG lMsBY = m_lMsPrDieA_Y[j];

		DOUBLE dDist = sqrt(pow(labs(lMsBX - lDieMsPrAX), 2) + pow(labs(lMsBY - lDieMsPrAY), 2));
		if( dDist>dFarDist )
		{
			dFarDist = dDist;
			lFarIndex = j;
		}
	}
	if( lFarIndex!=-1 )
	{
		lDieMsPrBX = m_lMsPrDieA_X[lFarIndex];
		lDieMsPrBY = m_lMsPrDieA_Y[lFarIndex];
		lDieScanBX = m_lScanDieA_X[lFarIndex];
		lDieScanBY = m_lScanDieA_Y[lFarIndex];
	}
#endif
	*/

	if( m_lScanPointIndex<=(SCN_POINTA_NUM-1))
	{
		DOUBLE	dEsAngleX, dEsAngleY, dMsAngleX, dMsAngleY;

		CalculateES101Angle(lDieScanAX, lDieScanAY, lDieScanBX, lDieScanBY, dEsAngleX, dEsAngleY);			
		CalculateES101Angle(lDieMsPrBX, lDieMsPrBY, lDieMsPrAX, lDieMsPrAY, dMsAngleX, dMsAngleY);	

		m_lScanDieA_X[m_lScanPointIndex] = lDieScanAX;
		m_lScanDieA_Y[m_lScanPointIndex] = lDieScanAY;
		m_lMsPrDieA_X[m_lScanPointIndex] = lDieMsPrAX;
		m_lMsPrDieA_Y[m_lScanPointIndex] = lDieMsPrAY;
		m_dESAngle_X[m_lScanPointIndex]	= dEsAngleX;
		m_dESAngle_Y[m_lScanPointIndex]	= dEsAngleY;
		m_dMSAngle_X[m_lScanPointIndex]	= dMsAngleX;
		m_dMSAngle_Y[m_lScanPointIndex]	= dMsAngleY;
		m_lScanPointIndex++;
	}

	return TRUE;
}	//	SCN align refer 

BOOL CWaferTable::GetScnData_Refer(ULONG ulRow, ULONG ulCol, LONG &lX, LONG &lY)
{
	if( ulRow>=WT_SCN_MAX_ROW || ulCol>=WT_SCN_MAX_COL )
	{
		return FALSE;
	}

	LONG encX = 0, encY = 0;
	bool bReturn = GetPrescanRunPosn(ulRow, ulCol, encX, encY);
	lX = encX;
	lY = encY;
	lX = _round(encX + encY * tan(m_dScnTableTheta));	//	refer special mode
	lY = _round(encY * cos(m_dScnTableTheta));			//	refer special mode

	return bReturn;
}

