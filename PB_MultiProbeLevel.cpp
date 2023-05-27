//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "PB_MultiProbeLevel.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Chip Prober probing position-level mapping class
//PB_CMultiProbeLevelInfo* PB_CMultiProbeLevelInfo::m_pInstance = NULL;
//PB_CMultiProbeLevelInfo* PB_CMultiProbeLevelInfo::Instance()
//{
//	if (m_pInstance == NULL)
//	{
//		m_pInstance = new PB_CMultiProbeLevelInfo();
//	}
//	return m_pInstance;
//}

PB_CMultiProbeLevelInfo::PB_CMultiProbeLevelInfo()
{
	InitPLPoints();
}

PB_CMultiProbeLevelInfo::~PB_CMultiProbeLevelInfo()
{
}

VOID PB_CMultiProbeLevelInfo::InitPLPoints()
{
	m_bPLState		= FALSE;
	
	for(ULONG lRow=0; lRow<PB_MPL_GRID_ROWS; lRow++)
	{
		for(ULONG lCol=0; lCol<PB_MPL_GRID_COLS; lCol++)
		{
			m_pPLPoints[lRow][lCol].m_lPLState	= -1;
			m_pPLPoints[lRow][lCol].m_lPLPointX	= 0;
			m_pPLPoints[lRow][lCol].m_lPLPointY	= 0;
			m_pPLPoints[lRow][lCol].m_lPL_Z		= PB_MPL_OFFSET_INIT;
		}
	}
}

BOOL PB_CMultiProbeLevelInfo::SetPointPL(ULONG ulRow, ULONG ulCol, LONG lScanX, LONG lScanY, LONG lPL_Z, LONG lState)
{
	if( ulRow>=PB_MPL_GRID_ROWS || ulCol>=PB_MPL_GRID_COLS )
	{
		return FALSE;
	}

	m_pPLPoints[ulRow][ulCol].m_lPLState	= lState;
	m_pPLPoints[ulRow][ulCol].m_lPLPointX	= lScanX;
	m_pPLPoints[ulRow][ulCol].m_lPLPointY	= lScanY;
	m_pPLPoints[ulRow][ulCol].m_lPL_Z		= lPL_Z;

	return TRUE;
}

BOOL PB_CMultiProbeLevelInfo::ReorganizeSampleData(LONG lPLSpanX, LONG lPLSpanY)
{
	LONG lPLCtrRow = PB_MPL_GRID_ROWS/2;
	LONG lPLCtrCol = PB_MPL_GRID_COLS/2;
	LONG lCtrRow = -1, lCtrCol = -1, lCtr_X = 0, lCtr_Y = 0;
	LONG lPL_X = 0, lPL_Y = 0, lPL_Z;
	DOUBLE dDist = 0, dMinDist = -1;

	for(LONG lRow=0; lRow<PB_MPL_GRID_ROWS; lRow++)
	{
		for(LONG lCol=0; lCol<PB_MPL_GRID_COLS; lCol++)
		{
            if( GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z)>0 )
			{
				DOUBLE dTemp = (DOUBLE)((lRow - lPLCtrRow) * (lRow - lPLCtrRow) + (lCol - lPLCtrCol) * (lCol - lPLCtrCol));
				dDist = sqrt(dTemp);
				if( dMinDist==-1 )
				{
					lCtrRow = lRow;
					lCtrCol = lCol;
					dMinDist = dDist;
					lCtr_X = lPL_X;
					lCtr_Y = lPL_Y;
				}
				if( dMinDist>dDist )
				{
					lCtrRow = lRow;
					lCtrCol = lCol;
					dMinDist = dDist;
					lCtr_X = lPL_X;
					lCtr_Y = lPL_Y;
				}
			}
		}
	}

	if( lCtrRow==-1 || lCtrCol==-1 )
	{
		return FALSE;
	}

	for(LONG lRow=0; lRow<PB_MPL_GRID_ROWS; lRow++)
	{
		for(LONG lCol=0; lCol<PB_MPL_GRID_COLS; lCol++)
		{
            LONG lState = GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
			lPL_X = lCtr_X + (lCtrCol - lCol) * lPLSpanX;
			lPL_Y = lCtr_Y + (lCtrRow - lRow) * lPLSpanY;
			if( lState<1 )
				lState = 0;
			SetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z, lState);
		}
	}

	for(LONG lRow=0; lRow<PB_MPL_GRID_ROWS; lRow++)
	{
		for(LONG lCol=0; lCol<PB_MPL_GRID_COLS; lCol++)
		{
			LONG lState = GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
			if( lState==0 )
			{
				LONG lPL_Z_C = PB_MPL_OFFSET_INIT;
				if( lCol>0 && lCol<(PB_MPL_GRID_COLS-1) )
				{
					LONG lPL_Z_1 = PB_MPL_OFFSET_INIT, lPL_Z_2 = PB_MPL_OFFSET_INIT;
					LONG lPL_X1, lPL_Y1;
					if( GetPointPL(lRow, lCol-1, lPL_X1, lPL_Y1, lPL_Z_1)>0 &&
						GetPointPL(lRow, lCol+1, lPL_X1, lPL_Y1, lPL_Z_2)>0 )
					{
						lPL_Z_C = (lPL_Z_1 + lPL_Z_2)/2;
					}
				}

				LONG lPL_Z_R = PB_MPL_OFFSET_INIT;
				if( lRow>0 && lRow<(PB_MPL_GRID_ROWS-1) )
				{
					LONG lPL_Z_1 = PB_MPL_OFFSET_INIT, lPL_Z_2 = PB_MPL_OFFSET_INIT;
					LONG lPL_X1, lPL_Y1;
					if( GetPointPL(lRow-1, lCol, lPL_X1, lPL_Y1, lPL_Z_1)>0 &&
						GetPointPL(lRow+1, lCol, lPL_X1, lPL_Y1, lPL_Z_2)>0 )
					{
						lPL_Z_R = (lPL_Z_1 + lPL_Z_2)/2;
					}
				}

				if( lPL_Z_C!=PB_MPL_OFFSET_INIT || lPL_Z_R!=PB_MPL_OFFSET_INIT )
				{
					if( lPL_Z_C!=PB_MPL_OFFSET_INIT && lPL_Z_R!=PB_MPL_OFFSET_INIT )
					{
						lPL_Z = (lPL_Z_C + lPL_Z_R)/2;
					}
					else if( lPL_Z_C!=PB_MPL_OFFSET_INIT )
					{
						lPL_Z = lPL_Z_C;
					}
					else if( lPL_Z_R!=PB_MPL_OFFSET_INIT )
					{
						lPL_Z = lPL_Z_R;
					}

					lState = 2;
					SetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z, lState);
				}
			}
		}
	}

	// need to enlarge the area and move map center to logic center, all die position would be covered.
	return TRUE;
}

LONG PB_CMultiProbeLevelInfo::GetPointPL(ULONG ulRow, ULONG ulCol, LONG &lScanX, LONG &lScanY, LONG &lPL_Z)
{
	if( ulRow>=PB_MPL_GRID_ROWS || ulCol>=PB_MPL_GRID_COLS )
	{
		return -1;
	}

	LONG lState = 0;
	lScanX = m_pPLPoints[ulRow][ulCol].m_lPLPointX;
	lScanY = m_pPLPoints[ulRow][ulCol].m_lPLPointY;
	lPL_Z  = m_pPLPoints[ulRow][ulCol].m_lPL_Z;
	lState = m_pPLPoints[ulRow][ulCol].m_lPLState;
	if( lPL_Z==PB_MPL_OFFSET_INIT )
		lState = 0;

	return lState;
}

BOOL PB_CMultiProbeLevelInfo::GetPLPointULCorner(CONST LONG lScanX, CONST LONG lScanY, ULONG &ulULRow, ULONG &ulULCol)
{
	LONG lCheckX = 0, lCheckY = 0, lPL_Z;

	BOOL bFindUL = FALSE;
	DOUBLE dNearDistUL = -1, dDist = 0;
	for(ULONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
	{
		for(ULONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
		{
			if( GetPointPL(ulRow, ulCol, lCheckX, lCheckY, lPL_Z)==-1 )
			{
				continue;
			}

			DOUBLE dDistX = lCheckX-lScanX;
			DOUBLE dDistY = lCheckY-lScanY;
			dDist = sqrt(dDistX*dDistX + dDistY*dDistY);
			if( lCheckX>=lScanX && lCheckY>=lScanY )
			{
				if( dNearDistUL==-1 )
				{
					dNearDistUL = dDist;
					ulULRow = ulRow;
					ulULCol = ulCol;
				}
				if( dNearDistUL>dDist )
				{
					ulULRow = ulRow;
					ulULCol = ulCol;
					dNearDistUL = dDist;
				}
				bFindUL = TRUE;
			}
		}
	}

	return bFindUL;
}

BOOL PB_CMultiProbeLevelInfo::GetNearestValidPL(LONG lScanX, LONG lScanY, ULONG &ulOutRow, ULONG &ulOutCol)
{
	DOUBLE lNearDist = -1, lDist = 0;
	BOOL bFindNear = FALSE;

	for(ULONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
	{
		for(ULONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
		{
			LONG lCheckX = 0, lCheckY = 0, lPL_Z = 0;
			if( GetPointPL(ulRow, ulCol, lCheckX, lCheckY, lPL_Z)<1 )
			{
				continue;
			}

			DOUBLE dDistX = lCheckX-lScanX;
			DOUBLE dDistY = lCheckY-lScanY;
			lDist = sqrt(dDistX*dDistX + dDistY*dDistY);
			if( lNearDist==-1 )
			{
				ulOutRow = ulRow;
				ulOutCol = ulCol;
				lNearDist = lDist;
			}
			if( lNearDist>lDist )
			{
				ulOutRow = ulRow;
				ulOutCol = ulCol;
				lNearDist = lDist;
			}
			bFindNear = TRUE;
		}
	}

	return bFindNear;
}

BOOL PB_CMultiProbeLevelInfo::GetPLPointLRCorner(CONST LONG lScanX, CONST LONG lScanY, ULONG &ulLRRow, ULONG &ulLRCol)
{
	LONG lCheckX = 0, lCheckY = 0, lPL_Z;

	BOOL bFindLR = FALSE;
	DOUBLE dNearDistLR = -1, dDist = 0;
	for(ULONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
	{
		for(ULONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
		{
			if( GetPointPL(ulRow, ulCol, lCheckX, lCheckY, lPL_Z)==-1 )
			{
				continue;
			}

			DOUBLE dDistX = lCheckX-lScanX;
			DOUBLE dDistY = lCheckY-lScanY;
			dDist = sqrt(dDistX*dDistX + dDistY*dDistY);
			if( lCheckX<lScanX && lCheckY<lScanY )
			{
				if( dNearDistLR==-1 )
				{
					dNearDistLR = dDist;
					ulLRRow = ulRow;
					ulLRCol = ulCol;
				}
				if( dNearDistLR>dDist )
				{
					ulLRRow = ulRow;
					ulLRCol = ulCol;
					dNearDistLR = dDist;
				}
				bFindLR = TRUE;
			}
		}
	}

	return bFindLR;
}	//	get LR corner

BOOL	PB_CMultiProbeLevelInfo::GetPLState()
{
	return m_bPLState;
}

BOOL	PB_CMultiProbeLevelInfo::SetPLState(BOOL bState)
{
	m_bPLState = bState;
	return TRUE;
}
//	Chip Prober probing position-level mapping class
