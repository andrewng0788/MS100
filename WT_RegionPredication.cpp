//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WT_RegionPredication.h"
#include "WT_SubRegion.h"
#include "math.h"
#include "LogFileUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	RUNNING_LAST

WT_CRegionPredication* WT_CRegionPredication::m_pInstance = NULL;

WT_CRegionPredication* WT_CRegionPredication::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new WT_CRegionPredication();
	return m_pInstance;
}

WT_CRegionPredication::WT_CRegionPredication()
{
}

WT_CRegionPredication::~WT_CRegionPredication()
{
}


BOOL WT_CRegionPredication::SetRegionOffset(ULONG ulSubRow, ULONG ulSubCol, LONG lOffsetX,  LONG lOffsetY, BOOL bToAll)
{
	m_bVerifyOffset = FALSE;
	m_lVerifyOffsetX = lOffsetX;
	m_lVerifyOffsetY = lOffsetY;

	if( lOffsetX==0 && lOffsetY==0 )
	{
		return TRUE;
	}
#ifdef	RUNNING_LAST
	bool bFindOne = false;
	for(int k=0; k<ADV_REGN_LAST_POINTS; k++)
	{
		if( bToAll )
		{
			m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetX[k] = lOffsetX;
			m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetY[k] = lOffsetY;
			bFindOne = TRUE;
		}
		else
		{
			if( m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetX[k]==0 &&
				m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetY[k]==0 )
			{
				m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetX[k] = lOffsetX;
				m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetY[k] = lOffsetY;
				bFindOne = TRUE;
				break;
			}
		}
	}

	if( bFindOne==false )
	{
		for(int k=0; k<ADV_REGN_LAST_POINTS-1; k++)
		{
			m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetX[k] = m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetX[k+1];
			m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetY[k] = m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetY[k+1];
		}
		m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetX[ADV_REGN_LAST_POINTS-1] = lOffsetX;
		m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetY[ADV_REGN_LAST_POINTS-1] = lOffsetY;
	}
#else
	if( bToAll )
	{
		m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnUpdIndex = 4;
	}
	else
	{
		if( m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnUpdIndex==0 )
		{
			m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnUpdIndex++;
		}
		else
		{
			lOffsetX = (m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnOffsetX * m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnUpdIndex + lOffsetX)/
				(m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnUpdIndex+1);
			if( m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnUpdIndex<=3 )
				m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnUpdIndex++;
		}
	}
	m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnOffsetX = lOffsetX;
	m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnOffsetY = lOffsetY;
#endif
	m_pRegionPreds[ulSubRow][ulSubCol].m_stPickedTime  = CTime::GetCurrentTime();

	return TRUE;
}

BOOL WT_CRegionPredication::GetRegionOffset(ULONG ulSubRow, ULONG ulSubCol, LONG &lOffsetX, LONG &lOffsetY, CTime &stPickTime)
{
	lOffsetX = 0;
	lOffsetY= 0;
	if( GetRegionPdcState(ulSubRow, ulSubCol)==WT_REGION_PRED_STATE_UNUSED )
	{
		stPickTime = CTime::GetCurrentTime();
		return FALSE;
	}

#ifdef	RUNNING_LAST
	int lNumber = 0;
	for(int k=0; k<ADV_REGN_LAST_POINTS; k++)
	{
		if( m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetX[k]!=0 ||
			m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetY[k]!=0 )
		{
			lOffsetX += m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetX[k];
			lOffsetY += m_pRegionPreds[ulSubRow][ulSubCol].m_laRegnOffsetY[k];
			lNumber++;
		}
	}

	if( lNumber!=0 )
	{
		lOffsetX = lOffsetX/lNumber;
		lOffsetY = lOffsetY/lNumber;
	}
#else
	lOffsetX = m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnOffsetX;
	lOffsetY = m_pRegionPreds[ulSubRow][ulSubCol].m_lRegnOffsetY;
#endif
	stPickTime = m_pRegionPreds[ulSubRow][ulSubCol].m_stPickedTime;
	return TRUE;
}

//================================================================
// Constructor / destructor implementation section
//================================================================
	
BOOL WT_CRegionPredication::InitRegionOffset()
{
	int i, j;

	for(i=0; i<WT_REGION_PRED_MAX_ROW; i++)
	{
		for(j=0; j<WT_REGION_PRED_MAX_COL; j++)
		{
#ifdef	RUNNING_LAST
			for(int k=0; k<ADV_REGN_LAST_POINTS; k++)
			{
				m_pRegionPreds[i][j].m_laRegnOffsetX[k] = 0;
				m_pRegionPreds[i][j].m_laRegnOffsetY[k] = 0;
			}
#else
			m_pRegionPreds[i][j].m_lRegnOffsetX = 0;
			m_pRegionPreds[i][j].m_lRegnOffsetY = 0;
			m_pRegionPreds[i][j].m_lRegnUpdIndex = 0;
#endif
			m_pRegionPreds[i][j].m_stPickedTime  = CTime::GetCurrentTime();
		}
	}

	return TRUE;
}

BOOL WT_CRegionPredication::IsValidPdcRegion(ULONG ulSubRow, ULONG ulSubCol)
{
	if( ulSubRow<GetNumOfPdcRegionRows() && ulSubCol<GetNumOfPdcRegionCols() )
	{
		return TRUE;
	}

	return FALSE;
}

VOID WT_CRegionPredication::InitPdcRegions(CONST INT nNumOfRows, CONST INT nNumOfCols)
{
	for(ULONG i=0; i<WT_REGION_PRED_MAX_ROW; i++)
	{
		for(ULONG j=0; j<WT_REGION_PRED_MAX_COL; j++)
		{
			SetPdcRegionArea(i, j, 0, 0, 0, 0);
			SetRegionInnerState(i, j, FALSE);
			SetRegionPdcState(i, j, WT_REGION_PRED_STATE_UNUSED);
		}
	}

	m_bVerifyOffset		= FALSE;
	m_lVerifyOffsetX	= 0;
	m_lVerifyOffsetY	= 0;

	m_ulNumOfPdcRows = nNumOfRows;
	m_ulNumOfPdcCols = nNumOfCols;
}

//================================================================
// SET Function implementation section
//================================================================

BOOL WT_CRegionPredication::SetPdcRegionArea(ULONG ulSubRow, ULONG ulSubCol, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol)
{
	m_pRegionPreds[ulSubRow][ulSubCol].m_ulULPdcRow		= ulULRow;
	m_pRegionPreds[ulSubRow][ulSubCol].m_ulULPdcCol		= ulULCol;
	m_pRegionPreds[ulSubRow][ulSubCol].m_ulLRPdcRow		= ulLRRow;
	m_pRegionPreds[ulSubRow][ulSubCol].m_ulLRPdcCol		= ulLRCol;

	return TRUE;
}

BOOL WT_CRegionPredication::SetRegionPdcState(ULONG ulSubRow, ULONG ulSubCol, ULONG ulState)
{
	m_pRegionPreds[ulSubRow][ulSubCol].m_ulPdcState = ulState;

	return TRUE;
}


BOOL WT_CRegionPredication::SetVerifyOffset(LONG lOffsetX, LONG lOffsetY)
{
	m_bVerifyOffset = TRUE;
	m_lVerifyOffsetX = lOffsetX;
	m_lVerifyOffsetY = lOffsetY;

	return TRUE;
}

BOOL WT_CRegionPredication::GetVerifyOffset(LONG &lOffsetX, LONG &lOffsetY)
{
	lOffsetX = m_lVerifyOffsetX;
	lOffsetY = m_lVerifyOffsetY;
	return m_bVerifyOffset;
}


BOOL WT_CRegionPredication::SetLastDieData(LONG lRow, LONG lCol, LONG lOffsetX, LONG lOffsetY)
{
	ULONG ulSubRow = 0, ulSubCol = 0;
	if( GetInPdcRegionSub(lRow, lCol, ulSubRow, ulSubCol)==FALSE )
		return FALSE;

	m_stLastDieInfo.m_ulLastSubRow = ulSubRow;
	m_stLastDieInfo.m_ulLastSubCol = ulSubCol;
	m_stLastDieInfo.m_lLastMapRow  = lRow;
	m_stLastDieInfo.m_lLastMapCol  = lCol;
	m_stLastDieInfo.m_lLastOffsetX = lOffsetX;
	m_stLastDieInfo.m_lLastOffsetY = lOffsetY;
	return TRUE;
}

BOOL WT_CRegionPredication::GetLastDieData(ULONG &ulSubRow, ULONG &ulSubCol, LONG &lRow, LONG &lCol, LONG &lOffsetX, LONG &lOffsetY)
{
	ulSubRow = m_stLastDieInfo.m_ulLastSubRow;
	ulSubCol = m_stLastDieInfo.m_ulLastSubCol;
	lRow = m_stLastDieInfo.m_lLastMapRow;
	lCol = m_stLastDieInfo.m_lLastMapCol;
	lOffsetX = m_stLastDieInfo.m_lLastOffsetX;
	lOffsetY = m_stLastDieInfo.m_lLastOffsetY;

	return TRUE;
}

BOOL WT_CRegionPredication::SetDieOffset(LONG lRow, LONG lCol, LONG lOffsetX, LONG lOffsetY, BOOL bToAll)
{
	ULONG ulSubRow = 0, ulSubCol = 0;
	if( GetInPdcRegionSub(lRow, lCol, ulSubRow, ulSubCol)==FALSE )
		return TRUE;

	if( IsValidPdcRegion(ulSubRow, ulSubCol) )
	{
		// xu_semitek need to take care the multi points, try to set a value, but fail, then restore old
		SetRegionOffset(ulSubRow, ulSubCol, lOffsetX, lOffsetY, bToAll);
	}

	SetLastDieData(lRow, lCol, lOffsetX, lOffsetY);

	return TRUE;
}

//================================================================
// GET Function implementation section
//================================================================

BOOL WT_CRegionPredication::GetPdcRegionArea(ULONG ulSubRow, ULONG ulSubCol, ULONG& ulULRow, ULONG& ulULCol, ULONG& ulLRRow, ULONG& ulLRCol)
{
	if( IsValidPdcRegion(ulSubRow, ulSubCol)==FALSE )
		return FALSE;

	ulULRow	= m_pRegionPreds[ulSubRow][ulSubCol].m_ulULPdcRow;
	ulULCol	= m_pRegionPreds[ulSubRow][ulSubCol].m_ulULPdcCol;
	ulLRRow	= m_pRegionPreds[ulSubRow][ulSubCol].m_ulLRPdcRow;
	ulLRCol	= m_pRegionPreds[ulSubRow][ulSubCol].m_ulLRPdcCol;

	return TRUE;
}

ULONG WT_CRegionPredication::GetRegionPdcState(ULONG ulSubRow, ULONG ulSubCol)
{
	if( IsValidPdcRegion(ulSubRow, ulSubCol)==FALSE )
		return WT_REGION_PRED_STATE_UNUSED;

	return m_pRegionPreds[ulSubRow][ulSubCol].m_ulPdcState;
}

VOID WT_CRegionPredication::SetRegionInnerState(ULONG ulSubRow, ULONG ulSubCol, BOOL bInner)
{
	if( IsValidPdcRegion(ulSubRow, ulSubCol)==FALSE )
		return ;

	m_pRegionPreds[ulSubRow][ulSubCol].m_bInnerRegion	= bInner;
}

BOOL WT_CRegionPredication::GetRegionInnerState(ULONG ulSubRow, ULONG ulSubCol)
{
	if( IsValidPdcRegion(ulSubRow, ulSubCol)==FALSE )
		return FALSE;

	return m_pRegionPreds[ulSubRow][ulSubCol].m_bInnerRegion;
}	//

ULONG WT_CRegionPredication::GetNumOfPdcRegionRows()
{
	if( m_ulNumOfPdcRows>=WT_REGION_PRED_MAX_ROW )
		m_ulNumOfPdcRows = WT_REGION_PRED_MAX_ROW;
	return m_ulNumOfPdcRows;
}

ULONG WT_CRegionPredication::GetNumOfPdcRegionCols()
{
	if( m_ulNumOfPdcCols>=WT_REGION_PRED_MAX_COL )
		m_ulNumOfPdcCols = WT_REGION_PRED_MAX_COL;
	return m_ulNumOfPdcCols;
}

BOOL WT_CRegionPredication::GetInPdcRegionSub(LONG lRow, LONG lCol, ULONG &ulSubRow, ULONG &ulSubCol)
{
	ULONG i = 0, j = 0;
	BOOL bFindIn = FALSE;
	ULONG ulULRow, ulULCol, ulLRRow, ulLRCol;

	for(i=0; i<GetNumOfPdcRegionRows(); i++)
	{
		for(j=0; j<GetNumOfPdcRegionCols(); j++)
		{
			if( GetPdcRegionArea(i, j, ulULRow, ulULCol, ulLRRow, ulLRCol) )
			{
				if( (lRow>=(LONG)ulULRow) && (lRow<(LONG)ulLRRow) && (lCol>=(LONG)ulULCol) && (lCol<(LONG)ulLRCol) )
				{
					bFindIn = TRUE;
					ulSubRow = i;
					ulSubCol = j;
					break;
				}
			}
		}
		if( bFindIn )
		{
			break;
		}
	}

	return bFindIn;
}


BOOL WT_CRegionPredication::IsWithinThisPdcRegion(ULONG ulSubRow, ULONG ulSubCol, ULONG ulRow, ULONG ulCol)
{
	ULONG lULRow, lULCol, lLRRow, lLRCol;
	BOOL bFound = FALSE;

	if( GetPdcRegionArea(ulSubRow, ulSubCol, lULRow, lULCol, lLRRow, lLRCol) )
	{
		if( (ulRow >= lULRow) && (ulRow < lLRRow) && (ulCol >= lULCol) && (ulCol < lLRCol) )
		{
			bFound = TRUE;
		}
	}

	return bFound;
}

//================================================================
// End of WT_CSubRegions.cpp
//================================================================
