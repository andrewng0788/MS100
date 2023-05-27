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

//#ifndef MS_DEBUG	//v4.47T7
//	#include "spswitch.h"
//	using namespace AsmSw;
//#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferTable::DefineAreaAndWalkPath_Prescan()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMsg;

	BOOL bFinisarBar = (pApp->GetCustomerName()==CTM_FINISAR) && pUtl->GetPrescanBarWafer();
	bFinisarBar = bFinisarBar && (GetBarPitchX()!=0) && (GetBarPitchY()!=0);
	INT lAlnWfX = GetPrescanAlignPosnX();
	INT lAlnWfY = GetPrescanAlignPosnY();
	BOOL bBarScanLimit = FALSE;
	if( pUtl->GetPrescanBarWafer() && !bFinisarBar )
	{
		bBarScanLimit = TRUE;
	}

	if( IsPrescanSizeSet() )
	{
		m_nPrescanMapCtrX = lAlnWfX;
		m_nPrescanMapCtrY = lAlnWfY;
		LONG lDummyMapDiameter = GetPrescanDiameter();
		m_nPrescanMapWidth  = lDummyMapDiameter;
		m_nPrescanMapHeight = lDummyMapDiameter;
		m_nPrescanMapRadius = lDummyMapDiameter/2;
	}
	else if( IsPrescanAreaDefined() )
	{
		m_nPrescanMapCtrX = GetPrescanCenterX();
		m_nPrescanMapCtrY = GetPrescanCenterY();
		m_nPrescanMapRadius = GetPrescanDiameter()/2;
		if( m_ucPrescanMapShapeType==WT_MAP_SHAPE_CIRCLE )
		{
			m_nPrescanMapWidth  = GetPrescanDiameter();
			m_nPrescanMapHeight = GetPrescanDiameter();
		}
		else
		{
			m_nPrescanMapWidth  = labs(m_lScanAreaLowRightX - m_lScanAreaUpLeftX);
			m_nPrescanMapHeight = labs(m_lScanAreaLowRightY - m_lScanAreaUpLeftY);
		}
	}
	else if( IsToAutoSearchWaferLimit() && m_bAutoWaferLimitOk )
	{
		m_nPrescanMapCtrX = GetPrescanCenterX();
		m_nPrescanMapCtrY = GetPrescanCenterY();
		if( m_ucPrescanMapShapeType == WT_MAP_SHAPE_CIRCLE )
		{
			LONG lDummyMapDiameter = GetPrescanDiameter();
			m_nPrescanMapWidth  = lDummyMapDiameter;
			m_nPrescanMapHeight = lDummyMapDiameter;
			m_nPrescanMapRadius = lDummyMapDiameter/2;
		}
		else
		{
			m_nPrescanMapWidth  = m_lAutoWaferWidth;
			m_nPrescanMapHeight = m_lAutoWaferHeight;
			m_nPrescanMapRadius = max(m_nPrescanMapWidth, m_nPrescanMapHeight)/2;
		}
	}
	else if( IsAutoDetectEdge() )
	{
		m_nPrescanMapCtrX = GetWaferCenterX();
		m_nPrescanMapCtrY = GetWaferCenterY();
		m_nPrescanMapRadius = GetWaferDiameter()/2;
		m_nPrescanMapWidth  = GetWaferDiameter();
		m_nPrescanMapHeight = GetWaferDiameter();
	}
	else if( IsFastHomeScan() )
	{
		m_nPrescanMapCtrX = GetWaferCenterX();
		m_nPrescanMapCtrY = GetWaferCenterY();
		m_nPrescanMapRadius = GetWaferDiameter()/2;
		m_nPrescanMapWidth  = GetWaferDiameter();
		m_nPrescanMapHeight = GetWaferDiameter();
		m_ucScanWalkTour = WT_SCAN_WALK_HOME_HORI;
	}
	else if( IsScanWithWaferLimit() || m_bContourEdge || bBarScanLimit )
	{
		m_nPrescanMapCtrX = GetWaferCenterX();
		m_nPrescanMapCtrY = GetWaferCenterY();
		m_nPrescanMapRadius = GetWaferDiameter()/2;
		m_nPrescanMapWidth  = GetWaferDiameter();
		m_nPrescanMapHeight = GetWaferDiameter();
	}
	else if( bFinisarBar )
	{
		LONG lAlgnWfX = GetPrescanAlignPosnX();
		LONG lAlgnWfY = GetPrescanAlignPosnY();
		LONG lHomeCol = m_nPrescanAlignMapCol;
		LONG lHomeRow = m_nPrescanAlignMapRow;

		LONG lDiePitchX = GetDiePitchX_X(); 
		LONG lBarStreetX = GetBarPitchX();
		LONG lBarStreetY = GetBarPitchY();

		LONG lMinRow = 0, lMaxRow = 0;
		LONG lMinCol = 0, lMaxCol = 0;
		GetMapValidSize(lMaxRow, lMinRow, lMaxCol, lMinCol);


		LONG lULX = lAlgnWfX + (5 + lHomeCol - lMinCol) * lDiePitchX;
		LONG lULY = lAlgnWfY + (1 + lHomeRow - lMinRow) * lBarStreetY;
		LONG lLRX = lAlgnWfX - (5 + lMaxCol - lHomeCol) * lDiePitchX - lBarStreetX;
		LONG lLRY = lAlgnWfY - (1 + lMaxRow - lHomeRow) * lBarStreetY;

		m_nPrescanMapCtrX = (lULX+lLRX)/2;
		m_nPrescanMapCtrY = (lULY+lLRY)/2;

		UINT unWafEdgeNum = pApp->GetPrescanWaferEdgeNum();
		LONG nXWidth = (INT) labs(lULX - lLRX) + labs(GetPrescanPitchX())*unWafEdgeNum*2;
		LONG nYHeight= (INT) labs(lULY - lLRY) + labs(GetPrescanPitchY())*unWafEdgeNum*2;

		m_nPrescanMapRadius = _round(sqrt(pow((DOUBLE) nXWidth / 2, 2.0) + pow((DOUBLE) nYHeight / 2, 2.0)));
		m_nPrescanMapWidth  = abs(nXWidth);
		m_nPrescanMapHeight = abs(nYHeight);

		CString szMoveMsg;
		szMoveMsg = "scan align limit,";
		pUtl->PrescanMoveLog(szMoveMsg);
		szMoveMsg.Format(",%d,%d", lAlgnWfX, lAlgnWfY);
		pUtl->PrescanMoveLog(szMoveMsg);
		szMoveMsg.Format(",%d,%d", GetScanUL_X(), GetScanUL_Y());
		pUtl->PrescanMoveLog(szMoveMsg);
		szMoveMsg.Format(",%d,%d", GetScanLR_X(), GetScanLR_Y());
		pUtl->PrescanMoveLog(szMoveMsg);
		szMoveMsg.Format(",%d,%d", GetScanCtrX(), GetScanCtrY());
		pUtl->PrescanMoveLog(szMoveMsg);
	}
	else
	{
		if( IsMS90HalfSortMode() )
			GetMS90HalfSortMapScanArea();
		else
			GetMapScanArea();
	}

	if( IsMS90HalfSortMode() && IsScanWithWaferLimit()==FALSE && !bFinisarBar)
	{
		return DefineAreaAndWalkPath_HalfSortScan();
	}	// ms 90 half sort, up part

	LONG lUpLX = GetWaferCenterX() + GetWaferDiameter()/2;
	LONG lUpLY = GetWaferCenterY() + GetWaferDiameter()/2;

	for(INT nRow=SCAN_MATRIX_MIN_ROW; nRow<SCAN_MAX_MATRIX_ROW; nRow++)
	{
		for(INT nCol=SCAN_MATRIX_MIN_COL; nCol<SCAN_MAX_MATRIX_COL; nCol++)
		{
			m_baScanMatrix[nRow][nCol] = FALSE;
		}
	}

	m_lAlignFrameCol = (lUpLX-lAlnWfX)/GetPrescanPitchX();
	m_lAlignFrameRow = (lUpLY-lAlnWfY)/GetPrescanPitchY();

	if( m_lAlignFrameRow>=(SCAN_MAX_MATRIX_ROW-1) )
		m_lAlignFrameRow = SCAN_MAX_MATRIX_ROW-1;
	if( m_lAlignFrameRow<(SCAN_MATRIX_MIN_ROW+1) )
		m_lAlignFrameRow = SCAN_MATRIX_MIN_ROW+1;
	if( m_lAlignFrameCol>=(SCAN_MAX_MATRIX_COL-1) )
		m_lAlignFrameCol = SCAN_MAX_MATRIX_COL-1;
	if( m_lAlignFrameCol<(SCAN_MATRIX_MIN_COL+1) )
		m_lAlignFrameCol = SCAN_MATRIX_MIN_COL+1;

	m_lScanFrameMaxRow	= -1;
	m_lScanFrameMaxCol	= -1;
	m_lScanFrameMinRow	= 9999;
	m_lScanFrameMinCol	= 9999;

	LONG lScanMaxY = lAlnWfY;

	if( m_bContourEdge )
	{
		LONG lScanWalkMaxX = m_lCntrEdgeX_L[0];
		for(int i = 1; i < (int)m_ulContourEdgePoints; i++)
		{
			if (lScanWalkMaxX<m_lCntrEdgeX_L[i])
				lScanWalkMaxX = m_lCntrEdgeX_L[i];
		}

		LONG lScanWalkMaxY =	m_lCntrEdgeY_C[0];
		LONG lScanWalkMinY =	m_lCntrEdgeY_C[0];
		if( m_ulContourEdgePoints>0 )
			lScanWalkMinY =	m_lCntrEdgeY_C[m_ulContourEdgePoints-1];
		lScanWalkMinY -= GetPrescanPitchY()/2;
		LONG lContourScanPitchY = GetPrescanPitchY();
		if( m_ulContourEdgePoints>1 )
		{
			lContourScanPitchY = labs(m_lCntrEdgeY_C[1] - m_lCntrEdgeY_C[0]);
		}
		if( lContourScanPitchY != GetPrescanPitchY() )
		{
			lScanWalkMinY -= GetPrescanPitchY()/2;
		}

		szMsg.Format("contour max x %d, max y %d, min y %d align point %d,%d",
			lScanWalkMaxX, lScanWalkMaxY, lScanWalkMinY, lAlnWfX, lAlnWfY);
		pUtl->PrescanMoveLog(szMsg, TRUE);
		szMsg.Format("contour Y pitch %d, prescan Y pitch %d",
			lContourScanPitchY, GetPrescanPitchY());
		pUtl->PrescanMoveLog(szMsg, TRUE);

		LONG lAlnFrameRow = 0, lAlnFrameCol = 0;
		DOUBLE dDist = -1;
		INT nScanRow = 0;
		while( 1 )
		{
			INT nCntrRow = 0;
			INT nPosnY = lScanWalkMaxY - nScanRow * GetPrescanPitchY();
			nScanRow++;
			if( nPosnY<lScanWalkMinY )
			{
				szMsg.Format("Posn Y %d < scan min Y. done",
					nPosnY);
				pUtl->PrescanMoveLog(szMsg, TRUE);
				break;
			}

			LONG lMinDist = labs(GetPrescanPitchY()*10);
			for(int nLoop = 0; nLoop < (int)m_ulContourEdgePoints; nLoop++)
			{
				LONG lCtrY = m_lCntrEdgeY_C[nLoop];
				if( lMinDist>labs(lCtrY-nPosnY) )
				{
					nCntrRow = nLoop;
					lMinDist = labs(lCtrY-nPosnY);
				}
			}

			LONG lLftX = m_lCntrEdgeX_L[nCntrRow];
			LONG lRgtX = m_lCntrEdgeX_R[nCntrRow];

			szMsg.Format("Posn Y %d contour Y %d. X %d,%d",
					nPosnY, m_lCntrEdgeY_C[nCntrRow], lLftX, lRgtX);
			pUtl->PrescanMoveLog(szMsg, TRUE);

			for(INT nScanCol=SCAN_MATRIX_MIN_COL; nScanCol<SCAN_MAX_MATRIX_COL; nScanCol++)
			{
				INT nPosnX = lScanWalkMaxX - nScanCol * GetPrescanPitchX();
				if( nPosnX>lLftX )
				{
					continue;
				}
				if( nPosnX<lRgtX )
				{
					break;
				}

				if( IsWithinWaferLimit(nPosnX, nPosnY)==FALSE )
				{
					szMsg.Format("contour %d,%d out of wafer limit WT %d", nPosnX, nPosnY, IsWT2InUse()+1);
					pUtl->PrescanMoveLog(szMsg, TRUE);
					continue;
				}

				if( m_lScanFrameMinRow>nScanRow )
					m_lScanFrameMinRow = nScanRow;
				if( m_lScanFrameMinCol>nScanCol )
					m_lScanFrameMinCol = nScanCol;
				if( m_lScanFrameMaxRow<nScanRow )
					m_lScanFrameMaxRow = nScanRow;
				if( m_lScanFrameMaxCol<nScanCol )
					m_lScanFrameMaxCol = nScanCol;
				m_baScanMatrix[nScanRow][nScanCol]	= TRUE;
				m_laScanPosnX[nScanRow][nScanCol]	= nPosnX;
				m_laScanPosnY[nScanRow][nScanCol]	= nPosnY;
				if( dDist==-1 )
				{
					dDist = GetDistance(nPosnX, nPosnY, lAlnWfX, lAlnWfY);
					lAlnFrameRow = nScanRow;
					lAlnFrameCol = nScanCol;
				}
				else
				{
					DOUBLE dCurrDist = GetDistance(nPosnX, nPosnY, lAlnWfX, lAlnWfY);
					if( dDist>dCurrDist )
					{
						dDist = dCurrDist;
						lAlnFrameRow = nScanRow;
						lAlnFrameCol = nScanCol;
					}
				}
			}	//	finish for this scan bar area.
		}

		m_lAlignFrameCol = lAlnFrameCol;
		m_lAlignFrameRow = lAlnFrameRow;
	}
	else	// m_bContourEdge false case
	{
		LONG lScanMaxX = lAlnWfX;
		LONG lScanMinX = lAlnWfX;
		for(INT nRow=SCAN_MATRIX_MIN_ROW; nRow<SCAN_MAX_MATRIX_ROW; nRow++)
		{
			for(INT nCol=SCAN_MATRIX_MIN_COL; nCol<SCAN_MAX_MATRIX_COL; nCol++)
			{
				LONG lDistRow = nRow - GetAlignFrameRow();
				LONG lDistCol = nCol - GetAlignFrameCol();
				INT nPosnX = lAlnWfX - lDistCol * GetPrescanPitchX();
				INT nPosnY = lAlnWfY - lDistRow * GetPrescanPitchY();
				if( IsPosnWithinMapRange(nPosnX, nPosnY)==FALSE )
				{
					continue;
				}

				if( IsWithinWaferLimit(nPosnX, nPosnY)==FALSE )
				{
					if (!IsScanThenDivideMap() && !IsProber())
					{
						continue;
					}

					LONG nInPosnX  = nPosnX, nInPosnY = nPosnY;
					if( nPosnX > GetWaferCenterX() )
					{
						nInPosnX = nPosnX - GetPrescanPitchX()/2;
					}
					else
					{
						nInPosnX = nPosnX + GetPrescanPitchX()/2;
					}

					if( nPosnY > GetWaferCenterY() )
					{
						nInPosnY = nPosnY - GetPrescanPitchY()/2;
					}
					else
					{
						nInPosnY = nPosnY + GetPrescanPitchY()/2;
					}
					if( IsWithinWaferLimit(nInPosnX, nInPosnY)==FALSE )
					{
						continue;
					}
					if( nPosnX == nInPosnX || nPosnY == nInPosnY )
					{
						continue;
					}

					LONG lEdgeX = nPosnX, lEdgeY = nPosnY;
					while( 1 )
					{
						if( nPosnX>nInPosnX )
						{
							lEdgeX--;
						}
						else
						{
							lEdgeX++;
						}
						lEdgeY = (lEdgeX - nInPosnX)*(nPosnY-nInPosnY)/(nPosnX-nInPosnX) + nInPosnY;
						if( IsWithinWaferLimit(lEdgeX, lEdgeY) )
						{
							break;
						}
					}
					nPosnX = lEdgeX;
					nPosnY = lEdgeY;
				}

				if( m_lScanFrameMinRow>nRow )
					m_lScanFrameMinRow = nRow;
				if( m_lScanFrameMinCol>nCol )
					m_lScanFrameMinCol = nCol;
				if( m_lScanFrameMaxRow<nRow )
					m_lScanFrameMaxRow = nRow;
				if( m_lScanFrameMaxCol<nCol )
					m_lScanFrameMaxCol = nCol;
				if( lScanMaxX<nPosnX )
					lScanMaxX = nPosnX;
				if( lScanMinX>nPosnX )
					lScanMinX = nPosnX;
				if( lScanMaxY<nPosnY )
					lScanMaxY = nPosnY;
				m_baScanMatrix[nRow][nCol]	= TRUE;
				m_laScanPosnX[nRow][nCol]	= nPosnX;
				m_laScanPosnY[nRow][nCol]	= nPosnY;
			}
		}
		m_lAreaPickScanUL_X = lScanMaxX;	// area pick
		m_lAreaPickScanLR_X = lScanMinX;	// area pick
	}

	m_lLastFrameRow		= m_lNextFrameRow		= GetAlignFrameRow();
	m_lLastFrameCol		= m_lNextFrameCol		= GetAlignFrameCol();

	szMsg.Format("alignment point frame matrix %d,%d", GetAlignFrameRow(), GetAlignFrameCol());
	pUtl->PrescanMoveLog(szMsg, TRUE);
	szMsg.Format("    WFT: normal frame dimension %d,%d==>%d,%d", m_lScanFrameMinRow, m_lScanFrameMinCol, m_lScanFrameMaxRow, m_lScanFrameMaxCol);
	pUtl->PrescanMoveLog(szMsg, TRUE);
	SaveScanTimeEvent(szMsg);
	if( IsPrescanMapIndex()==FALSE )
	{
		szMsg.Format("frame table move position");
		pUtl->PrescanMoveLog(szMsg, TRUE);
		for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
		{
			CString szMsg, szTemp;
			for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
			{
				if( IsScanFrameInWafer(nRow, nCol) )
				{
					szTemp.Format("(%d,%d),", m_laScanPosnX[nRow][nCol], m_laScanPosnY[nRow][nCol]);
					szMsg += szTemp;
				}
			}
			pUtl->PrescanMoveLog(szMsg, TRUE);
		}

		szMsg.Format("frame move points");
		pUtl->PrescanMoveLog(szMsg, TRUE);
		for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
		{
			CString szMsg, szTemp;
			for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
			{
				if ( (nRow >= SCAN_MATRIX_MIN_ROW) && (nRow <= SCAN_MAX_MATRIX_ROW) && 
					 (nCol >= SCAN_MATRIX_MIN_COL) && (nCol <= SCAN_MAX_MATRIX_COL) )	//v4.47 Klocwork
				{
					szTemp.Format("%d", m_baScanMatrix[nRow][nCol]);
					szMsg += szTemp;
				}
			}
			pUtl->PrescanMoveLog(szMsg, TRUE);
		}
	}
	else
	{
		szMsg.Format("prescan map index mode is in use.");
		pUtl->PrescanMoveLog(szMsg, TRUE);
		SaveScanTimeEvent(szMsg);
	}

	if( pUtl->GetPrescanAreaPickMode() )
	{
		m_lAreaPickScanDirection = -1;
		INT nRow=0;
		INT nCol=0;
		bool bFindOne = FALSE;
		INT nPosnX = m_lAreaPickScanUL_X;
		INT nPosnY = lScanMaxY;
		for(nRow=SCAN_MATRIX_MIN_ROW; nRow<SCAN_MAX_MATRIX_ROW; nRow++)
		{
			for(nCol=SCAN_MATRIX_MIN_COL; nCol<SCAN_MAX_MATRIX_COL; nCol++)
			{
				if( IsScanFrameInWafer(nRow, nCol) )
				{
					bFindOne = TRUE;
					nPosnX = m_laScanPosnX[nRow][nCol];
					nPosnY = m_laScanPosnY[nRow][nCol];
					break;
				}
			}
			if( bFindOne==TRUE )
			{
				break;
			}
		}
		m_nPrescanNextWftPosnX	= m_nPrescanLastWftPosnX = nPosnX;
		m_nPrescanNextWftPosnY	= m_nPrescanLastWftPosnY = nPosnY;
		if( GetDiePitchX_X()!=0 || GetDiePitchY_Y()!=0 )
		{
			LONG lScanRow = (lAlnWfY - nPosnY)/GetDiePitchY_Y() + m_nPrescanAlignMapRow;
			LONG lScanCol = (lAlnWfX - nPosnX)/GetDiePitchX_X() + m_nPrescanAlignMapCol;
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
			m_nPrescanNextMapRow	= m_nPrescanLastMapRow	 = lScanRow;
			m_nPrescanNextMapCol	= m_nPrescanLastMapCol	 = lScanCol;
		}
		ClearGoodInfo();
	}

	m_ucRunScanWalkTour = m_ucScanWalkTour;
	if( IsAutoDetectEdge()==FALSE )
	{
		BOOL bFindFirst = FALSE;
		LONG lFirstRow = 0, lFirstCol = 0;
		switch( m_ucRunScanWalkTour )
		{
		case WT_SCAN_WALK_TOP_HORI:
			for(lFirstRow=SCAN_MATRIX_MIN_ROW; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
			{
				for(lFirstCol=SCAN_MATRIX_MIN_COL; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
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
			for(lFirstCol=SCAN_MATRIX_MIN_COL; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
			{
				for(lFirstRow=SCAN_MATRIX_MIN_ROW; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
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
		default:
			if( m_bContourEdge && IsPrescanMapIndex()==FALSE )
			{
				bFindFirst = TRUE;
				lFirstRow = GetAlignFrameRow();
				lFirstCol = GetAlignFrameCol();
			}
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
	}
	else
	{
		if( m_ucRunScanWalkTour == WT_SCAN_WALK_HOME_VERT || m_ucRunScanWalkTour == WT_SCAN_WALK_LEFT_VERT )
		{
			m_ucRunScanWalkTour = WT_SCAN_WALK_HOME_HORI;
		}
	}

//	if( pUtl->GetPrescanDebug() )
	{
		CString szMoveMsg;

		GetEncoderValue();
		szMoveMsg.Format("GTT, %ld Cur %ld", GetGlobalT(), GetCurrT());
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);

		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		szMoveMsg.Format("Sub Region %ld,%ld", pSRInfo->GetSubRows(), pSRInfo->GetSubCols());
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		LONG lUserRow=0, lUserCol=0;
		ULONG lAlnRow = m_ulAlignHomeDieRow;
		ULONG lAlnCol = m_ulAlignHomeDieCol;
		ConvertAsmToOrgUser(lAlnRow, lAlnCol, lUserRow, lUserCol); 
		szMoveMsg.Format("MAP, Asm(%ld, %ld), user(%ld, %ld)", lAlnRow, lAlnCol, lUserRow, lUserCol);
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("Walk path Map Valid Dimension, Row(%ld,%ld), Col(%ld,%ld)", GetMapValidMinRow(), GetMapValidMaxRow(), GetMapValidMinCol(), GetMapValidMaxCol());
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("WFT Alignment, %ld, %ld", lAlnWfX, lAlnWfY);
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("WFT Width and Height, %d, %d", m_nPrescanMapWidth, m_nPrescanMapHeight);
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("WFT Radius, %d", GetScanRadius());
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("EGE, %d, shape %d", pApp->GetPrescanWaferEdgeNum(), m_ucPrescanMapShapeType);
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
		szMoveMsg.Format("WFT CTR, %d, %d", GetScanCtrX(), GetScanCtrY());
		pUtl->PrescanMoveLog(szMoveMsg, TRUE);
	}

	return TRUE;
}

#define	BW_ROW_BASE		50
#define	BW_LEFT_BASE	50
#define	BW_RIGHT_BASE	150

BOOL CWaferTable::DefineAreaAndWalkPath_BarWafer()
{
	BOOL bPathReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMsg, szAlarmMessage = "";
	CString szFileName = m_szPrescanLogPath + PRESCAN_MOVE_POSN;
	remove(szFileName);

	ObtainEmcoreBarColumnStart();	//	define scan path, also check the assistant points enough or not.

	//	check bar wafer scan reference die added for every bar
	INT lAlnWfX = GetPrescanAlignPosnX();
	INT lAlnWfY = GetPrescanAlignPosnY();
	ULONG lAlnRow = m_ulAlignHomeDieRow;
	ULONG lAlnCol = m_ulAlignHomeDieCol;
	szMsg.Format("Bar Wafer align  %3ld,%3ld==>%8lu,%8lu", lAlnRow, lAlnCol, lAlnWfX, lAlnWfY);
	pUtl->PrescanMoveLog(szMsg, TRUE);
	for(ULONG ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		LONG lAssistRow = 0, lAssistCol = 0, lAssistWfX = 0, lAssistWfY = 0;
		if( pUtl->GetAssistPosition(ulIndex, lAssistRow, lAssistCol, lAssistWfX, lAssistWfY) )
		{
			szMsg.Format("Bar Wafer assist %3ld,%3ld==>%8ld,%8ld", lAssistRow, lAssistCol, lAssistWfX, lAssistWfY);
			pUtl->PrescanMoveLog(szMsg, TRUE);
		}
	}

	LONG lUpLX = GetWaferCenterX() + GetWaferDiameter()/2;
	LONG lUpLY = GetWaferCenterY() + GetWaferDiameter()/2;

	for(INT nRow=SCAN_MATRIX_MIN_ROW; nRow<SCAN_MAX_MATRIX_ROW; nRow++)
	{
		for(INT nCol=SCAN_MATRIX_MIN_COL; nCol<SCAN_MAX_MATRIX_COL; nCol++)
		{
			m_baScanMatrix[nRow][nCol] = FALSE;
		}
	}

	m_lAlignFrameCol = (lUpLX-lAlnWfX)/GetPrescanPitchX();
	m_lAlignFrameCol = min(m_lAlignFrameCol, (SCAN_MAX_MATRIX_COL-1));
	m_lAlignFrameCol = max(m_lAlignFrameCol, (SCAN_MATRIX_MIN_COL+1));
	m_lAlignFrameRow = (lUpLY-lAlnWfY)/GetPrescanPitchY();
	m_lAlignFrameRow = min(m_lAlignFrameRow, (SCAN_MAX_MATRIX_ROW-1));
	m_lAlignFrameRow = max(m_lAlignFrameRow, (SCAN_MATRIX_MIN_ROW+1));

	m_lScanFrameMaxRow	= -1;
	m_lScanFrameMaxCol	= -1;
	m_lScanFrameMinRow	= 9999;
	m_lScanFrameMinCol	= 9999;

	LONG lAlignFrameCol = m_lAlignFrameCol;
	LONG lAlignFrameRow = m_lAlignFrameRow;
	LONG lFrameBase[2];
	lFrameBase[0] = BW_LEFT_BASE;
	lFrameBase[1] = BW_RIGHT_BASE;
	for(ULONG ulRow = (ULONG)GetMapValidMinRow(); ulRow <= (ULONG)GetMapValidMaxRow(); ulRow++)
	{
		INT nFrameRow = BW_ROW_BASE + ulRow - GetMapValidMinRow();
		nFrameRow = min(nFrameRow, SCAN_MAX_MATRIX_ROW);
		ULONG ulRefCol = lAlnCol;
		LONG  lRefWftX = lAlnWfX;
		LONG  lRefWftY = lAlnWfY;
		ULONG ulFirstDie = GetMapValidMaxCol()+10;
		ULONG ulLastDie  = GetMapValidMaxCol();
		ULONG ul2ndStart = GetMapValidMaxCol();
		for(ULONG ulCol = (ULONG)GetMapValidMinCol(); ulCol <= (ULONG)GetMapValidMaxCol(); ulCol++)
		{
			if (m_pWaferMapManager->IsMapHaveBin(ulRow, ulCol))
			{
				if (ulFirstDie > (ULONG)GetMapValidMaxCol())
				{
					ulFirstDie = ulCol;
				}
				ulLastDie = ulCol;
			}
			if (m_WaferMapWrapper.GetDieState(ulRow, ulCol) == WT_MAP_DS_BAR_2ND_START)
			{
				ul2ndStart = ulCol;
			}
		}

		if (ulFirstDie > ulLastDie)
		{
			continue;
		}

		for(UCHAR ucLoop=0; ucLoop<2; ucLoop++)	//	two columns of bar in same row, 0 is left and 1 is right
		{
			//	found align or reference point
			BOOL bBaseFound = FALSE;
			if( (ulRow==lAlnRow) &&
				((ucLoop==0 && lAlnCol<ul2ndStart) || (ucLoop==1 && lAlnCol>=ul2ndStart) || (ul2ndStart>=ulLastDie)) )
			{
				ulRefCol = lAlnCol;
				lRefWftX = lAlnWfX;
				lRefWftY = lAlnWfY;
				bBaseFound = TRUE;
			}
			else
			{
				for(ULONG ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
				{
					LONG lAssistRow = 0, lAssistCol = 0, lAssistWfX = 0, lAssistWfY = 0;
					if( pUtl->GetAssistPosition(ulIndex, lAssistRow, lAssistCol, lAssistWfX, lAssistWfY) )
					{
						if (((ULONG)lAssistRow == ulRow) && 
							((ucLoop==0 && (ULONG)lAssistCol < ul2ndStart) || (ucLoop==1 && (ULONG)lAssistCol >= ul2ndStart) || (ul2ndStart >= ulLastDie)))
						{
							ulRefCol = lAssistCol;
							lRefWftX = lAssistWfX;
							lRefWftY = lAssistWfY;
							bBaseFound = TRUE;
							break;
						}
					}
				}
			}

			if( bBaseFound==FALSE )
			{
				bPathReturn = FALSE;
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ul2ndStart, lUserRow, lUserCol);
				szMsg.Format("Map Row %ld, Column %ld has no reference point at ", lUserRow, lUserCol);
				szAlarmMessage += szMsg;
				if( ucLoop==0 )
					szAlarmMessage += "left side\n";
				else
					szAlarmMessage += "right side\n";
			}
			szMsg.Format("Bar Wafer Scan row %3ld, part %d(%3lu) base %3ld,%3ld==>%8ld,%8ld, found %d",
				ulRow, ucLoop, ul2ndStart, ulRow, ulRefCol, lRefWftX, lRefWftY, bBaseFound);
			pUtl->PrescanMoveLog(szMsg, TRUE);

			if( bBaseFound==FALSE )
			{
				pUtl->PrescanMoveLog("Bar wafer this row part skipped", TRUE);
				continue;
			}

			INT nFramePosnY = lRefWftY;

			for(UCHAR ucIndexLoop=0; ucIndexLoop<2; ucIndexLoop++)
			{
				LONG lStartLoop = 0, lEndLoop = 0;
				if( ucIndexLoop==0 )
				{
					LONG lDistColSpan = 0;
					if( ucLoop==0 )	//	left
						lDistColSpan = labs(ulRefCol - ulFirstDie);
					else
						lDistColSpan = labs(ulRefCol - ul2ndStart);

					// left side of reference point
					lStartLoop = _round(fabs(lDistColSpan * GetDiePitchX_X() * 1.1) / GetPrescanPitchX() + 1);
					lStartLoop = lFrameBase[ucLoop] - min(lFrameBase[ucLoop], lStartLoop);
					lEndLoop = lFrameBase[ucLoop];
				}
				else
				{
					LONG lDistColSpan = 0;
					if( ucLoop==0 )	//	left part
						lDistColSpan = labs(ul2ndStart - 1 - ulRefCol);
					else
						lDistColSpan = labs(ulLastDie - ulRefCol);
					lEndLoop = lFrameBase[ucLoop] + _round(fabs(lDistColSpan * GetDiePitchX_X() * 1.1) / GetPrescanPitchX() + 1);
					lEndLoop = min(lEndLoop, SCAN_MAX_MATRIX_COL);
					lStartLoop = lFrameBase[ucLoop]+1;
				}
				for(INT nFrameCol=lStartLoop; nFrameCol<=lEndLoop; nFrameCol++)
				{
					LONG lDistCol = nFrameCol - lFrameBase[ucLoop];
					INT nPosnX = lRefWftX - lDistCol * GetPrescanPitchX();
					if( IsPosnWithinMapRange(nPosnX, nFramePosnY)==FALSE )
					{
						continue;
					}
					if( IsWithinWaferLimit(nPosnX, nFramePosnY)==FALSE )
					{
						continue;
					}

					if( m_lScanFrameMinRow>nFrameRow )
						m_lScanFrameMinRow = nFrameRow;
					if( m_lScanFrameMinCol>nFrameCol )
						m_lScanFrameMinCol = nFrameCol;
					if( m_lScanFrameMaxRow<nFrameRow )
						m_lScanFrameMaxRow = nFrameRow;
					if( m_lScanFrameMaxCol<nFrameCol )
						m_lScanFrameMaxCol = nFrameCol;

					m_baScanMatrix[nFrameRow][nFrameCol]	= TRUE;
					m_laScanPosnX[nFrameRow][nFrameCol]	= nPosnX;
					m_laScanPosnY[nFrameRow][nFrameCol]	= nFramePosnY;
					if( nPosnX==lAlnWfX && nFramePosnY==lAlnWfY )
					{
						lAlignFrameRow = nFrameRow;
						lAlignFrameCol = nFrameCol;
					}
				}
			}

			if( ul2ndStart>=ulLastDie )
			{
				break;
			}
		}
	}
	m_lAlignFrameRow = lAlignFrameRow;
	m_lAlignFrameCol = lAlignFrameCol;

	m_lLastFrameRow		= m_lNextFrameRow		= GetAlignFrameRow();
	m_lLastFrameCol		= m_lNextFrameCol		= GetAlignFrameCol();

	szMsg.Format("Bar Wafer frame matrix alignment point %d,%d", GetAlignFrameRow(), GetAlignFrameCol());
	pUtl->PrescanMoveLog(szMsg, TRUE);
	szMsg.Format("Bar Wafer normal frame dimension %d,%d==>%d,%d",
		m_lScanFrameMinRow, m_lScanFrameMinCol, m_lScanFrameMaxRow, m_lScanFrameMaxCol);
	pUtl->PrescanMoveLog(szMsg, TRUE);
	szMsg.Format("frame table move position");
	pUtl->PrescanMoveLog(szMsg, TRUE);
	for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
	{
		CString szTemp;
		for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
		{
			if( IsScanFrameInWafer(nRow, nCol) )
			{
				szTemp.Format("matrix,%3d,%3d,wft,%8d,%8d", nRow, nCol, m_laScanPosnX[nRow][nCol], m_laScanPosnY[nRow][nCol]);
				pUtl->PrescanMoveLog(szTemp, TRUE);
			}
		}
	}

	szMsg.Format("frame move points");
	pUtl->PrescanMoveLog(szMsg, TRUE);
	for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
	{
		CString szMsg = "", szTemp;
		for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
		{
			if( (nRow >= SCAN_MATRIX_MIN_ROW) && (nRow <= SCAN_MAX_MATRIX_ROW) && 
				(nCol >= SCAN_MATRIX_MIN_COL) && (nCol <= SCAN_MAX_MATRIX_COL) )	//v4.47 Klocwork
			{
				szTemp.Format("[%d]", m_baScanMatrix[nRow][nCol]);
				szMsg += szTemp;
			}
		}
		pUtl->PrescanMoveLog(szMsg, TRUE);
	}

	m_ucRunScanWalkTour = m_ucScanWalkTour;
	BOOL bFindFirst = FALSE;
	LONG lFirstRow = GetAlignFrameRow(), lFirstCol = GetAlignFrameCol();
	switch( m_ucRunScanWalkTour )
	{
	case WT_SCAN_WALK_TOP_HORI:
		for(lFirstRow=SCAN_MATRIX_MIN_ROW; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
		{
			for(lFirstCol=SCAN_MATRIX_MIN_COL; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
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
		for(lFirstCol=SCAN_MATRIX_MIN_COL; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
		{
			for(lFirstRow=SCAN_MATRIX_MIN_ROW; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
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
	default:
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
		m_nPrescanNextMapRow	= m_nPrescanLastMapRow	 = lScanRow;
		m_nPrescanNextMapCol	= m_nPrescanLastMapCol	 = lScanCol;
		m_nPrescanNextWftPosnX	= m_nPrescanLastWftPosnX = lFirstWfX;
		m_nPrescanNextWftPosnY	= m_nPrescanLastWftPosnY = lFirstWfY;
		m_lLastFrameRow			= m_lNextFrameRow		 = lFirstRow;
		m_lLastFrameCol			= m_lNextFrameCol		 = lFirstCol;
	}

	CString szMoveMsg;
	GetEncoderValue();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	szMoveMsg.Format("GTT, %ld Cur %ld; Sub Region %ld,%ld",
		GetGlobalT(), GetCurrT(), pSRInfo->GetSubRows(), pSRInfo->GetSubCols());
	pUtl->PrescanMoveLog(szMoveMsg, TRUE);
	LONG lUserRow=0, lUserCol=0;
	ConvertAsmToOrgUser(lAlnRow, lAlnCol, lUserRow, lUserCol); 
	szMoveMsg.Format("MAP, Asm(%ld, %ld), user(%ld, %ld)", lAlnRow, lAlnCol, lUserRow, lUserCol);
	pUtl->PrescanMoveLog(szMoveMsg, TRUE);
	szMoveMsg.Format("Walk path Map Valid Dimension, Row(%ld,%ld), Col(%ld,%ld)",
		GetMapValidMinRow(), GetMapValidMaxRow(), GetMapValidMinCol(), GetMapValidMaxCol());
	pUtl->PrescanMoveLog(szMoveMsg, TRUE);
	szMoveMsg.Format("WFT Alignment, %ld, %ld", lAlnWfX, lAlnWfY);
	pUtl->PrescanMoveLog(szMoveMsg, TRUE);
	szMoveMsg.Format("WFT Width and Height, %d, %d", m_nPrescanMapWidth, m_nPrescanMapHeight);
	pUtl->PrescanMoveLog(szMoveMsg, TRUE);
	szMoveMsg.Format("EGE, %d, shape %d", pApp->GetPrescanWaferEdgeNum(), m_ucPrescanMapShapeType);
	pUtl->PrescanMoveLog(szMoveMsg, TRUE);
	szMoveMsg.Format("WFT CTR, %d, %d Radius, %d", GetScanCtrX(), GetScanCtrY(), GetScanRadius());
	pUtl->PrescanMoveLog(szMoveMsg, TRUE);

	if( bPathReturn==FALSE )
	{
		if (HmiMessage_Red_Back(szAlarmMessage, "Bar Wafer Scan", glHMI_MBX_CONTINUESTOP) == glHMI_CONTINUE)
		{
			bPathReturn = TRUE;
		}
		SetErrorMessage(szAlarmMessage);
	}

	return bPathReturn;
}
