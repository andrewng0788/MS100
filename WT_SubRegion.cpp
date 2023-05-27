//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WT_SubRegion.h"
#include "math.h"
#include "DieRegion.h"
#include "MathFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//================================================================
// Constructor / destructor implementation section
//================================================================
	
WT_CSubRegionsInfo* WT_CSubRegionsInfo::m_pInstance = NULL;

WT_CSubRegionsInfo* WT_CSubRegionsInfo::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new WT_CSubRegionsInfo();
	return m_pInstance;
}

WT_CSubRegionsInfo::WT_CSubRegionsInfo()
{
	InitRegion();
	m_ulSubRows = 1;
	m_ulSubCols = 1;
	
	m_lRegionPickMinLimit	= 0;

	m_szCurrentRegionState_HMI	= "";
	m_ulCurrentSortingRegion_HMI = 0;

	m_bManualAlignRegion	=	FALSE;
	m_bRegionAligned		= FALSE;

	m_nRegionWftULX			= 0;
	m_nRegionWftULY			= 0;
	m_nRegionWftLRX			= 0;
	m_nRegionWftLRY			= 0;

	m_bIsRegionEnd			= FALSE;
	m_bIsRegionEnding		= FALSE;	// START UP S/W

	m_bRegionManualConfirm	= FALSE;

	m_ucSortingGrade	= 0;
	m_ulSortingRegion	= 0;

	m_ucPickingGrade	= 0;
	m_ulPickingRegion	= 0;
	m_RegionList.RemoveAll();
}

WT_CSubRegionsInfo::~WT_CSubRegionsInfo()
{
	m_RegionList.RemoveAll();
}


LONG WT_CSubRegionsInfo::_round(double val)
{
	return CMathFunc::Instance()->_round(val);
}

BOOL WT_CSubRegionsInfo::IsRegionPickingMode()
{
	return ((m_ulSubCols > 1) || (m_ulSubRows > 1));
}


ULONG WT_CSubRegionsInfo::GetSubRows()
{
 	if (m_ulSubRows <= 0)
	{
		m_ulSubRows = 1;
	}

	if (m_ulSubRows > WT_MAP_REGION_MAX_ROW)
	{
		return WT_MAP_REGION_MAX_ROW;
	}

	return m_ulSubRows;
}

ULONG WT_CSubRegionsInfo::GetSubCols()
{
	if (m_ulSubCols <= 0)
	{
		m_ulSubCols = 1;
	}

	if (m_ulSubCols > WT_MAP_REGION_MAX_COL)
	{
		return WT_MAP_REGION_MAX_COL;
	}

	return m_ulSubCols;
}


//============================================================================================================
//				HMI Releated function
//============================================================================================================
VOID WT_CSubRegionsInfo::GetRegionRowColumnNum(const CString szAlgorithm, const ULONG ulMaxRow, const ULONG ulMaxCol, LONG &lSubRowsInRegion, LONG &lSubColsInRegion)
{
	if ((GetSubRows() == 1) && (GetSubCols() == 1))
	{
		if( (szAlgorithm.Find("Shortest")	!= -1) ||			//v3.87		
			(szAlgorithm.Find("Sorting Path 1")	!= -1) ||
			(szAlgorithm.Find("(Cluster)") != -1) ||			//v4.13T5	//SUpport By-Region Cluster mode
			(szAlgorithm.Find("Continuous Pick") != -1) )		//v4.27T1	//COntinuous Pick ver 2
		{
			//Ref-Cross 1-region mode available for Shortest-Path algor only
			lSubRowsInRegion = ulMaxRow;
			lSubColsInRegion = ulMaxCol;
		}
		else
		{
			//SubRow/Col = 1 under Ref-Cross sub-region mode -> TLH in single region
			lSubRowsInRegion = 1;
			lSubColsInRegion = 1;
		}
	}
	else
	{
		lSubRowsInRegion = ((LONG)ulMaxRow + (GetSubRows() - 1)) / GetSubRows();
		lSubColsInRegion = ((LONG)ulMaxCol + (GetSubCols() - 1)) / GetSubCols();
	}

	if (GetSubRows() == 1 && GetSubCols() == 1)	// divide by zero
	{
		lSubRowsInRegion = (LONG)ulMaxRow;
		lSubColsInRegion  = (LONG)ulMaxCol;
	}
}


//============================================================================================================
//				HMI Releated function
//============================================================================================================
VOID WT_CSubRegionsInfo::SetCurrentRegionState_HMI(const CString szCurrentRegionState)
{
	m_szCurrentRegionState_HMI = szCurrentRegionState;
}

VOID WT_CSubRegionsInfo::SetCurrentSortingRegion_HMI(const ULONG ulCurrentSortingRegion)
{
	m_ulCurrentSortingRegion_HMI = ulCurrentSortingRegion;
}
//==============================================================================================================

//============================================================================================================
//				Manual Align Region or Auto Align Region
//============================================================================================================
VOID WT_CSubRegionsInfo::SetRegionAligned(const BOOL bRegionAligned)
{
	m_bRegionAligned = bRegionAligned;
}

BOOL WT_CSubRegionsInfo::IsRegionAligned()
{
	return m_bRegionAligned;
}
//==============================================================================================================

//============================================================================================================
//				Manual Align Region or Auto Align Region, which is controlled by HMI menu button
//============================================================================================================
VOID WT_CSubRegionsInfo::SetManualAlignRegion(const BOOL bManualAlignRegion)
{
	m_bManualAlignRegion = bManualAlignRegion;
}

BOOL WT_CSubRegionsInfo::IsManualAlignRegion()
{
	return m_bManualAlignRegion;
}
//==============================================================================================================


//============================================================================================================
//				Is Region End or not
//============================================================================================================
VOID WT_CSubRegionsInfo::SetRegionEnding(const BOOL bIsRegionEnding)
{
	m_bIsRegionEnding = bIsRegionEnding;
}

BOOL WT_CSubRegionsInfo::IsRegionEnding()
{
	return m_bIsRegionEnding;
}
//==============================================================================================================

//============================================================================================================
//				Is Region End or not
//============================================================================================================
VOID WT_CSubRegionsInfo::SetRegionEnd(const BOOL bIsRegionEnd)
{
	m_bIsRegionEnd = bIsRegionEnd;
}

BOOL WT_CSubRegionsInfo::IsRegionEnd()
{
	return m_bIsRegionEnd;
}
//==============================================================================================================


//============================================================================================================
//				Is Region Manual Confirm or not
//============================================================================================================
VOID WT_CSubRegionsInfo::SetRegionManualConfirm(const BOOL bRegionManualConfirm)
{
	m_bRegionManualConfirm = bRegionManualConfirm;
}

BOOL WT_CSubRegionsInfo::IsRegionManualConfirm()
{
	return m_bRegionManualConfirm;
}
//==============================================================================================================

//============================================================================================================
//				Calculate the region size of prescan
//============================================================================================================
VOID WT_CSubRegionsInfo::CalcPrescanRegionSize(const ULONG ulUlRow, const ULONG ulUlCol, const ULONG ulLrRow, const ULONG ulLrCol,	
										const ULONG ulRow, const ULONG ulCol, const LONG lX, const LONG lY, const LONG nEdgeX, const LONG nEdgeY,
										const LONG lDiePitchX_X, const LONG lDiePitchX_Y, const LONG lDiePitchY_Y, const LONG lDiePitchY_X)
{
	LONG lDiff_X = 0, lDiff_Y = 0;

	lDiff_X = ulUlCol - ulCol;
	lDiff_Y = ulUlRow - ulRow;
	m_nRegionWftULX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X + nEdgeX;
	m_nRegionWftULY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y + nEdgeY;
	lDiff_X = ulLrCol - ulCol;
	lDiff_Y = ulLrRow - ulRow;
	m_nRegionWftLRX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X - nEdgeX;
	m_nRegionWftLRY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y - nEdgeY;
}


INT WT_CSubRegionsInfo::GetRegionWftULX()
{
	return m_nRegionWftULX;
}

INT WT_CSubRegionsInfo::GetRegionWftULY()
{
	return m_nRegionWftULY;
}

INT WT_CSubRegionsInfo::GetRegionWftLRX()
{
	return m_nRegionWftLRX;
}

INT WT_CSubRegionsInfo::GetRegionWftLRY()
{
	return m_nRegionWftLRY;
}


BOOL WT_CSubRegionsInfo::IsScanWithinMapRegionRange(INT nPosnX, INT nPosnY)
{
	if ( nPosnX > m_nRegionWftULX || nPosnX < m_nRegionWftLRX || nPosnY > m_nRegionWftULY || nPosnY < m_nRegionWftLRY )
	{
		return FALSE;
	}

	return TRUE;
}
//==========================================================================================================


//============================================================================================================
//				Is Region Manual Confirm or not
//============================================================================================================
BOOL WT_CSubRegionsInfo::IsInvalidRegion(ULONG ulRegion)
{
	if ((ulRegion <= 0) || (ulRegion > GetTotalSubRegions()))
	{
		return TRUE;
	}

	return FALSE;
}

VOID WT_CSubRegionsInfo::InitRegion()
{
	m_ulPickIndex	  = 0;	// init region
	m_ulHomeDieRegion = 0;
	SetInitState(WT_SUBREGION_INIT_LOADMAP);
	for (ULONG i = 0; i < WT_MAX_SUBREGIONS_LIMIT; i++)
	{
		SetRegionState(i + 1, WT_SUBREGION_STATE_INIT);
		SetPickOrder(i, 0, 0);
		SetRegion(i + 1, 0, 0, 0, 0);
	}
}

//================================================================
// SET Function implementation section
//================================================================
//========================Sorting Grade and Region==================================
VOID WT_CSubRegionsInfo::SetSortingRegionGrade(const UCHAR ucSortingGrade)
{
	m_ucSortingGrade = ucSortingGrade;
}


UCHAR WT_CSubRegionsInfo::GetSortingRegionGrade()
{
	return m_ucSortingGrade;
}


VOID WT_CSubRegionsInfo::SetSortingRegion(const ULONG ulSortingRegion)
{
	m_ulSortingRegion = ulSortingRegion;
}


ULONG WT_CSubRegionsInfo::GetSortingRegion()
{
	return m_ulSortingRegion;
}


//========================Picking Grade and Region==================================
VOID WT_CSubRegionsInfo::SetPickingRegionGrade(const ULONG ucPickingGrade)
{
	m_ucPickingGrade = (UCHAR)ucPickingGrade;
}


UCHAR WT_CSubRegionsInfo::GetPickingRegionGrade()
{
	return m_ucPickingGrade;
}


VOID WT_CSubRegionsInfo::SetPickingRegion(const ULONG ulPickingRegion)
{
	m_ulPickingRegion = ulPickingRegion;
}


ULONG WT_CSubRegionsInfo::GetPickingRegion()
{
	return m_ulPickingRegion;
}


VOID WT_CSubRegionsInfo::SetRegionSize(const LONG ulSize)
{
	m_RegionList.SetSize(ulSize + 1);
}

WM_CRegion *WT_CSubRegionsInfo::AddRegionList(const ULONG ulRegion, const ULONG ulULRow, const ULONG ulULCol, const ULONG ulLRRow, const ULONG ulLRCol)
{
	WM_CRegion *pRegion = new WM_CRegion();

	if (pRegion)
	{
		pRegion->SetRegion(ulULRow, ulULCol, ulLRRow, ulLRCol);
		m_RegionList[ulRegion] = pRegion;
	}

	return pRegion;
}


WM_CRegion *WT_CSubRegionsInfo::GetRegionList(const ULONG ulRegion)
{
	WM_CRegion *pRegion = (WM_CRegion *)m_RegionList[ulRegion];
	return pRegion;
}


WM_CRegion *WT_CSubRegionsInfo::GetNextSortingRegion()
{
	WM_CRegion *pRegion = NULL;
	
	while (!IsInvalidRegion(m_ulSortingRegion + 1))
	{
		if ((pRegion = GetRegionList(m_ulSortingRegion)) != NULL)
		{
			return pRegion;
		}
		m_ulSortingRegion++;
	}

	return NULL;
}

VOID WT_CSubRegionsInfo::MoveToNextSortingRegion()
{
	m_ulSortingRegion++;
}


WM_CRegion *WT_CSubRegionsInfo::GetNextRegion()
{
	WM_CRegion *pRegion = NULL;
	
	while (!IsInvalidRegion(m_ulPickingRegion + 1))
	{
		if ((pRegion = GetRegionList(m_ulPickingRegion)) != NULL)
		{
			return pRegion;
		}
		m_ulPickingRegion++;
	}

	return NULL;
}


BOOL WT_CSubRegionsInfo::SetRegion(ULONG ulRegion, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol)
{
	if (IsInvalidRegion(ulRegion))
	{
		return FALSE;
	}

	m_pRegions[ulRegion - 1].m_ulULRow		= ulULRow;
	m_pRegions[ulRegion - 1].m_ulULCol		= ulULCol;
	m_pRegions[ulRegion - 1].m_ulLRRow		= ulLRRow;
	m_pRegions[ulRegion - 1].m_ulLRCol		= ulLRCol;

	return TRUE;
}

BOOL WT_CSubRegionsInfo::SetRegionState(ULONG ulRegion, ULONG ulState)
{
	if (IsInvalidRegion(ulRegion))
	{
		return FALSE;
	}

	m_pRegions[ulRegion - 1].m_ulState = ulState;

	return TRUE;
}

BOOL WT_CSubRegionsInfo::SetHomeDieRegion(ULONG ulHomeDieRegion)
{
	if (IsInvalidRegion(ulHomeDieRegion))
	{
		return FALSE;
	}

	m_ulHomeDieRegion = ulHomeDieRegion;

	return TRUE;
}

//================================================================
// GET Function implementation section
//================================================================
BOOL WT_CSubRegionsInfo::GetRegion(CONST ULONG ulRegion, ULONG& ulULRow, ULONG& ulULCol, ULONG& ulLRRow, ULONG& ulLRCol)
{
	ulULRow = 0; 
	ulULCol = 0;
	ulLRRow = 0;
	ulLRCol = 0;
	if (IsInvalidRegion(ulRegion))
	{
		return FALSE;
	}

	ulULRow	= m_pRegions[ulRegion - 1].m_ulULRow;
	ulULCol	= m_pRegions[ulRegion - 1].m_ulULCol;
	ulLRRow	= m_pRegions[ulRegion - 1].m_ulLRRow;
	ulLRCol	= m_pRegions[ulRegion - 1].m_ulLRCol;

	return TRUE;
}


ULONG WT_CSubRegionsInfo::GetRegionState(ULONG ulRegion)
{
	if (IsInvalidRegion(ulRegion))
	{
		return WT_SUBREGION_STATE_BONDED;
	}

	return m_pRegions[ulRegion - 1].m_ulState;
}


VOID WT_CSubRegionsInfo::SetInitState(CONST ULONG ulInitState)
{
	m_ulInitState = ulInitState;
}

ULONG WT_CSubRegionsInfo::GetInitState()
{
	return m_ulInitState;
}

BOOL WT_CSubRegionsInfo::IsAllRegionsBonded(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol)
{
	return IsRightAllRegionSorted(bHalfSortMode, bRowModeSeparateHalfMap, ulHalfMaxRow, ulHalfMaxCol);

/*
	for (ULONG i = 1; i <= GetTotalSubRegions(); i++)
	{
		if (GetRegionState(i) != WT_SUBREGION_STATE_BONDED)
		{
			return FALSE;
		}
	}

	return TRUE;
*/
}

BOOL WT_CSubRegionsInfo::HasNoRegionBonded()
{
	for (ULONG i = 1; i <= GetTotalSubRegions(); i++)
	{
		if( GetRegionState(i) == WT_SUBREGION_STATE_BONDED )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL WT_CSubRegionsInfo::SetTargetRegion(ULONG ulTgtRegion)
{
	if (IsInvalidRegion(ulTgtRegion))
	{
		return FALSE;
	}

	for (ULONG i = 0; i < GetTotalSubRegions(); i++)
	{
		LONG ulRegionNo = min(i, WT_MAX_SUBREGIONS_LIMIT - 1);	//Klocwork	//v4.02T5

		if (GetSortRegion(ulRegionNo) == ulTgtRegion)
		{
			m_ulPickIndex = ulRegionNo;	// set target region when manual align or change order by HMI
			return TRUE;
		}
	}

	return FALSE;
}


BOOL WT_CSubRegionsInfo::IsRightAllRegionSorted(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol)
{
/*
	if (GetTotalSubRegions() == 1)
	{
		if (GetRegionState(1) == WT_SUBREGION_STATE_BONDED)
		{
			return TRUE;
		}
	}
	else if (GetTotalSubRegions() == 2)
	{
		if (GetRegionState(2) == WT_SUBREGION_STATE_BONDED)
		{
			return TRUE;
		}
	}
	else if (GetTotalSubRegions() == 4)
	{
		if ((GetRegionState(2) == WT_SUBREGION_STATE_BONDED) && 
			(GetRegionState(4) == WT_SUBREGION_STATE_BONDED))
		{
			return TRUE;
		}
	}
	else if (GetTotalSubRegions() == 9)
	{
		BOOL bAllBonded = TRUE;
		for(ULONG i = 1; i <= 6 ; i++)
		{
			if (GetRegionState(i) != WT_SUBREGION_STATE_BONDED)
			{
				bAllBonded = FALSE;
			}
		}

		return bAllBonded;
	}

	return FALSE;
*/
	BOOL bAllBonded = TRUE;
	ULONG ulRegionNo = 0;

	for (ULONG i = 0; i < GetTotalSubRegions(); i++)
	{
		ulRegionNo = GetSortRegion(i);
		if (!IsInvalidRegion(ulRegionNo))
		{
			BOOL bOutMS90SortingPart = IsOutMS90SortingPart(bHalfSortMode, bRowModeSeparateHalfMap, ulHalfMaxRow, ulHalfMaxCol, ulRegionNo);

			if (!bOutMS90SortingPart &&
				GetRegionState(ulRegionNo) != WT_SUBREGION_STATE_BONDED &&
				GetRegionState(ulRegionNo) != WT_SUBREGION_STATE_SKIPPED)
			{
				bAllBonded = FALSE;
			}
		}
	}

	return bAllBonded;
}


VOID WT_CSubRegionsInfo::ResetRegionState(const ULONG ulStartRow, const ULONG ulEndRow, const ULONG ulStratCol, const ULONG ulEndCol)
{
	for (ULONG i = 0; i < GetTotalSubRegions(); i++)
	{
		BOOL bOutOfRange = FALSE;
		if ((m_pRegions[i].m_ulULRow < ulStartRow - 1) || (m_pRegions[i].m_ulULCol < ulStratCol - 1) ||
			(m_pRegions[i].m_ulLRRow >= ulEndRow + 1) || (m_pRegions[i].m_ulLRCol >= ulEndCol + 1))
		{
			bOutOfRange = TRUE;
		}
		
		if (bOutOfRange && (m_pRegions[i].m_ulState == WT_SUBREGION_STATE_BONDED))
		{
			m_pRegions[i].m_ulState = WT_SUBREGION_STATE_INIT;
		}
	}
}

VOID WT_CSubRegionsInfo::Rotate180RegionState()
{
	WT_SR_PROPERTY	Regions[WT_MAX_SUBREGIONS_LIMIT];
	for (ULONG i = 0; i < GetTotalSubRegions(); i++)
	{
		Regions[i] = m_pRegions[i];
	}

	if (GetTotalSubRegions() == 2)
	{
		m_pRegions[0].m_ulState = Regions[1].m_ulState;
		m_pRegions[1].m_ulState = Regions[0].m_ulState;
	}
	else if (GetTotalSubRegions() == 4)
	{
		m_pRegions[0].m_ulState = Regions[3].m_ulState;
		m_pRegions[1].m_ulState = Regions[2].m_ulState;
		m_pRegions[2].m_ulState = Regions[1].m_ulState;
		m_pRegions[3].m_ulState = Regions[0].m_ulState;
	}
	else if (GetTotalSubRegions() == 9)
	{
		m_pRegions[8].m_ulState = Regions[4].m_ulState;
		m_pRegions[7].m_ulState = Regions[3].m_ulState;
		m_pRegions[6].m_ulState = Regions[2].m_ulState;
		m_pRegions[1].m_ulState = Regions[5].m_ulState;
		m_pRegions[0].m_ulState = Regions[0].m_ulState;
		m_pRegions[5].m_ulState = Regions[1].m_ulState;
		m_pRegions[2].m_ulState = Regions[6].m_ulState;
		m_pRegions[3].m_ulState = Regions[7].m_ulState;
		m_pRegions[4].m_ulState = Regions[8].m_ulState;
	}
}

ULONG WT_CSubRegionsInfo::GetHomeDieRegion()
{
	return m_ulHomeDieRegion;
}

BOOL WT_CSubRegionsInfo::GetRegionSub(ULONG ulRegion, ULONG &ulSubRow, ULONG &ulSubCol)
{
	if (IsInvalidRegion(ulRegion))
	{
		return FALSE;
	}

	ulSubRow = (ulRegion - 1) / m_ulSubCols;
	ulSubCol = (ulRegion - 1) % m_ulSubCols;

	return TRUE;
}

VOID WT_CSubRegionsInfo::SetTargetRegionIndex(ULONG ulIndex)
{
	if (IsInvalidRegion(ulIndex + 1))
	{
		return ;
	}

	m_ulPickIndex = ulIndex;	// sw restart but not load map
}

ULONG WT_CSubRegionsInfo::GetTargetRegionIndex()
{
	return m_ulPickIndex;
}

ULONG WT_CSubRegionsInfo::GetTotalSubRegions()
{
	if ((m_ulSubRows * m_ulSubCols) > WT_MAX_SUBREGIONS_LIMIT)
	{
		return WT_MAX_SUBREGIONS_LIMIT;
	}

	return m_ulSubRows * m_ulSubCols;
}


//Region No. starts from 1
ULONG WT_CSubRegionsInfo::GetWithinRegionNo(ULONG ulRow, ULONG ulCol)
{
	ULONG i = 0;
	BOOL bFound = FALSE;

	for (i = 0; i < GetTotalSubRegions(); i++)
	{
		if (IsWithinThisRegion(i + 1, ulRow, ulCol))
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound)
	{
		return (i+1);
	}

	return 0;
}


BOOL WT_CSubRegionsInfo::IsWithinThisRegion(ULONG ulRegion, ULONG ulRow, ULONG ulCol)
{
	ULONG lULRow, lULCol, lLRRow, lLRCol;
	BOOL bFound = FALSE;

	if (GetRegion(ulRegion, lULRow, lULCol, lLRRow, lLRCol))
	{
		if( (ulRow >= lULRow) && (ulRow < lLRRow) && (ulCol >= lULCol) && (ulCol < lLRCol) )
		{
			bFound = TRUE;
		}
	}

	return bFound;
}



BOOL WT_CSubRegionsInfo::IsOutMS90SortingPart(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol,
											  const ULONG ulRegion)
{
	ULONG lULRow = 0, lULCol = 0, lLRRow = 0, lLRCol = 0;

	if (GetRegion(ulRegion, lULRow, lULCol, lLRRow, lLRCol))
	{
		if (bHalfSortMode)
		{
			if (bRowModeSeparateHalfMap)
			{
				if (lLRRow >= ulHalfMaxRow)
				{
					return TRUE;
				}
			}
			else
			{
				if ((lULCol < ulHalfMaxCol) && (lLRCol <= ulHalfMaxCol))
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}



//======================================================================================
//  find the next region for picking die
//  if this region is out of SortingPart
//======================================================================================
BOOL WT_CSubRegionsInfo::FindNextAutoSortRegion(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol)	// peek next region from beginning of 0
{
	ULONG ulLoopNo = 0;
	BOOL bFound = TRUE;

	m_ulPickIndex = 0;	// LOOP FOR NEXT VALID REGION (FROM BEGION)
	for (ULONG i = 0; i < GetTotalSubRegions(); i++)
	{
		ulLoopNo = GetTargetRegion();
		if (IsInvalidRegion(ulLoopNo))
		{
			bFound = FALSE;
			break;
		}

		BOOL bOutMS90SortingPart = IsOutMS90SortingPart(bHalfSortMode, bRowModeSeparateHalfMap, ulHalfMaxRow, ulHalfMaxCol, ulLoopNo);

		if (!bOutMS90SortingPart &&
			GetRegionState(ulLoopNo) != WT_SUBREGION_STATE_BONDED &&
			GetRegionState(ulLoopNo) != WT_SUBREGION_STATE_SKIPPED)
		{
			break;
		}

		m_ulPickIndex++;	// LOOP FOR NEXT VALID REGION (FROM BEGION)
	}

	if (m_ulPickIndex >= GetTotalSubRegions())
	{
		ULONG i = 0;
		for (i = 0; i < GetTotalSubRegions(); i++)
		{
			ulLoopNo = GetSortRegion(i);
			if (GetRegionState(ulLoopNo) != WT_SUBREGION_STATE_BONDED &&
				GetRegionState(ulLoopNo) != WT_SUBREGION_STATE_SKIPPED)
			{
				break;
			}
		}

		if (i >= GetTotalSubRegions())
		{
			bFound = FALSE;
		}
	}

	return bFound;
}


ULONG WT_CSubRegionsInfo::GetTargetRegion()
{
	ULONG ulRegionNo = GetSortRegion(GetTargetRegionIndex());
	if (IsInvalidRegion(ulRegionNo))
	{
		return 0;
	}

	return ulRegionNo;
}

ULONG WT_CSubRegionsInfo::GetAssistRegion()
{
	ULONG ulRegionNo = GetHelpRegion(GetTargetRegionIndex());
	if (IsInvalidRegion(ulRegionNo))
	{
		return 0;
	}

	return ulRegionNo;
}

BOOL WT_CSubRegionsInfo::CheckRegionCoupled(ULONG ulRegionNo)
{
	if (IsInvalidRegion(ulRegionNo))
	{
		return FALSE;
	}

	ULONG ulOtherNo = 0;
	for (ULONG i = 0; i<GetTotalSubRegions(); i++)
	{
		i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5

		ulOtherNo = GetSortRegion(i);
		if (ulOtherNo != ulRegionNo && GetRegionState(ulOtherNo) != WT_SUBREGION_STATE_BONDED)
		{
			if (GetHelpRegion(i) == ulRegionNo)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


ULONG WT_CSubRegionsInfo::GetSortRegion(int nIndex)
{
	ULONG ulRegionNo = m_pPickOrder[nIndex].m_ulTargetRegion;
	if (IsInvalidRegion(ulRegionNo))
	{
		return 0;
	}

	return ulRegionNo;
}

ULONG WT_CSubRegionsInfo::GetHelpRegion(int nIndex)
{
	ULONG ulRegionNo = m_pPickOrder[nIndex].m_ulAssistRegion;
	if (IsInvalidRegion(ulRegionNo))
	{
		return 0;
	}

	return ulRegionNo;
}


BOOL WT_CSubRegionsInfo::IsCanSortingRegion(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol, const ULONG ulRegionNo)
{
	LONG lTotalRegionNum = GetTotalSubRegions();

	//ignore the bonded region and skipped region
	if ((GetRegionState(ulRegionNo) == WT_SUBREGION_STATE_BONDED) ||
		(GetRegionState(ulRegionNo) == WT_SUBREGION_STATE_SKIPPED))
	{
		return FALSE;
	}

	BOOL bCanSortingRegion = TRUE;

	BOOL bOutMS90SortingPart = IsOutMS90SortingPart(bHalfSortMode, bRowModeSeparateHalfMap, ulHalfMaxRow, ulHalfMaxCol, ulRegionNo);

	if (bOutMS90SortingPart)
	{
		bCanSortingRegion = FALSE;
	}

/*
	if (lTotalRegionNum == 4)
	{
		//==================================
		// Region#1  Region#2
		// Region#3  Region#4
		//==================================
		if ((ulRegionNo == 1) || (ulRegionNo == 3))
		{
			bCanSortingRegion = FALSE;
		}
	}
	else if (lTotalRegionNum == 9)
	{
		//==================================
		// Region#9  Region#2  Region#3
		// Region#8  Region#1  Region#4
		// Region#7  Region#6  Region#5
		//==================================
		if ((ulRegionNo == 7) || (ulRegionNo == 8) || (ulRegionNo == 9))
		{
			bCanSortingRegion = FALSE;
		}
	}
*/
	return bCanSortingRegion;
}


VOID WT_CSubRegionsInfo::SetPickOrder(int i, ULONG ulTarget, ULONG ulAssist)
{
	m_pPickOrder[i].m_ulTargetRegion = ulTarget;
	m_pPickOrder[i].m_ulAssistRegion = ulAssist;
}

BOOL WT_CSubRegionsInfo::IsLastRegionBonded()
{
	if (IsInvalidRegion(m_ulPickIndex + 1))
	{
		return TRUE;
	}

	return FALSE;
}

//================================================================
// Function Name: 		AssignRegionPickList
// Input arguments:		ucRgnOrder
// Output arguments:	None
// Description:   		set the bonding sequance based on the home die region, the home die region is last and the farest is first
//						ucRgnOrder: 0, far first; 1, sequencial TLH order; 2, BRH
// Return:				None
// Remarks:				Called by SetupSubRegionMode
//================================================================
VOID WT_CSubRegionsInfo::AssignRegionPickList(CONST UCHAR ucRgnOrder)
{
	ULONG ulSortRegion;
	ULONG nRegionList[WT_MAX_SUBREGIONS_LIMIT];

	for (ULONG i = 0; i < WT_MAX_SUBREGIONS_LIMIT; i++)
	{
		nRegionList[i] = i + 1;
	}

	ULONG ulHomeRow, ulHomeCol;
	if (GetRegionSub(GetHomeDieRegion(), ulHomeRow, ulHomeCol) == FALSE)
	{
		GetRegionSub(GetTotalSubRegions(), ulHomeRow, ulHomeCol);
	}

	for (ULONG i = 0; i < GetTotalSubRegions(); i++)
	{
		i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5

		switch (ucRgnOrder)
		{
		case 2:
			ulSortRegion = GetTotalSubRegions() - i;
			break;
		case 1:
			ulSortRegion = i + 1;
			break;
		case 0:
		default:
			ULONG ulSubRow, ulSubCol;
			ULONG ulRegion, ulRegionJ;
			LONG  lRowDiff, lColDiff, lTempDist, lFarDist;
			lFarDist = -1;
			ulSortRegion = 0;
			ulRegionJ = WT_MAX_SUBREGIONS_LIMIT - 1;
			for (ULONG j = 0; j < GetTotalSubRegions(); j++)
			{
				ulRegion = nRegionList[j];
				if (ulRegion > 0 && GetRegionSub(ulRegion, ulSubRow, ulSubCol))
				{
					lRowDiff = (LONG)ulSubRow - (LONG)ulHomeRow;
					lColDiff = (LONG)ulSubCol - (LONG)ulHomeCol;
					lTempDist = lRowDiff*lRowDiff + lColDiff * lColDiff;
					if (lFarDist < lTempDist)
					{
						lFarDist = lTempDist;
						ulSortRegion = ulRegion;
						ulRegionJ = j;
					}
				}
			}
			nRegionList[ulRegionJ] = 0;
			break;
		}

		SetPickOrder(i, ulSortRegion, ulSortRegion);
	}
}

VOID WT_CSubRegionsInfo::ResetSkippedRegionList()
{
	WT_SR_PICKORDER ulRegionList[WT_MAX_SUBREGIONS_LIMIT];
	ULONG ulSubRow, ulSubCol;
	ULONG ulRegionNo = 0, i = 0, jToNoSkip = 0, kUnchange = 0, ulAssistNo = 0;
	ULONG ulNearRegion[4];
	BOOL  bSkipOk = FALSE;

	memset(ulRegionList, 0, sizeof(ulRegionList));
	for (i = 0; i < GetTotalSubRegions(); i++)
	{
		i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5

		ulRegionNo = GetSortRegion(i);
		bSkipOk = FALSE;
		if (GetRegionState(ulRegionNo) == WT_SUBREGION_STATE_SKIPPED)
		{
			GetRegionSub(ulRegionNo, ulSubRow, ulSubCol);
			ULONG j = 0;
			for (j = 0; j < 4; j++)
			{
				ulNearRegion[j] = 0;
			}

			if (ulSubCol > 0)
			{
				ulNearRegion[0] = ulRegionNo - 1;
			}

			if (ulSubCol < (m_ulSubCols - 1))
			{
				ulNearRegion[1] = ulRegionNo + 1;
			}

			if (ulSubRow > 0)
			{
				ulNearRegion[2] = ulRegionNo - m_ulSubCols;
			}

			if (ulSubRow < (m_ulSubRows - 1))
			{
				ulNearRegion[3] = ulRegionNo + m_ulSubCols;
			}

/*
			if (ulSubRow > 0)
			{
				//1. Upper-Left
				if (ulSubCol > 0)
				{
					ulNearRegion[0] = ulRegionNo - m_ulSubCols - 1;
				}

				//2. Upper
				ulNearRegion[1] = ulRegionNo - m_ulSubCols;
	
				//3. Upper-Right
				if (ulSubCol < (m_ulSubCols - 1))
				{
					ulNearRegion[2] = ulRegionNo - m_ulSubCols + 1;
				}
			}

			//4. Center-Left
			if (ulSubCol > 0)
			{
				ulNearRegion[3] = ulRegionNo - 1;
			}

			//5. Center
	
			//6. Center-Right
			if (ulSubCol < (m_ulSubCols - 1))
			{
				ulNearRegion[4] = ulRegionNo + 1;
			}

			if (ulSubRow < (m_ulSubRows - 1))
			{
				//7. Lower-Left
				if (ulSubCol > 0)
				{
					ulNearRegion[0] = ulRegionNo + m_ulSubCols - 1;
				}

				//8. Lower
				ulNearRegion[1] = ulRegionNo + m_ulSubCols;
	
				//9. Lower-Right
				if (ulSubCol < (m_ulSubCols - 1))
				{
					ulNearRegion[2] = ulRegionNo + m_ulSubCols + 1;
				}
			}
*/



			for (j = 0; j < 4; j++)
			{
				ulAssistNo = ulNearRegion[j];
				if (GetRegionState(ulAssistNo) == WT_SUBREGION_STATE_BONDING ||
					GetRegionState(ulAssistNo) == WT_SUBREGION_STATE_INIT)
				{
					SetPickOrder(jToNoSkip, ulRegionNo, ulAssistNo);
					jToNoSkip++;
					bSkipOk = TRUE;
					break;
				}
			}

			if (!bSkipOk)
			{
				//can not find the assist region

			}
		}
		if (bSkipOk)
		{
			continue;
		}

		ulRegionList[kUnchange].m_ulTargetRegion = ulRegionNo;
		ulRegionList[kUnchange].m_ulAssistRegion = ulRegionNo;
		kUnchange++;
	}

	for (i = 0; i < jToNoSkip; i++)
	{
		ulRegionNo = GetSortRegion(i);
		SetRegionState(ulRegionNo, WT_SUBREGION_STATE_INIT);
	}

	if (jToNoSkip > 0)
	{
		for (i = 0; i < kUnchange; i++)
		{
			SetPickOrder(jToNoSkip, ulRegionList[i].m_ulTargetRegion, ulRegionList[i].m_ulAssistRegion);
			jToNoSkip++;
		}
	}

	// reset the order, make bonding region firstly, then skipped region, at last bonded region
	WT_SR_PICKORDER ulSkipedList[WT_MAX_SUBREGIONS_LIMIT];
	WT_SR_PICKORDER ulBondedList[WT_MAX_SUBREGIONS_LIMIT];
	WT_SR_PICKORDER ulToSortList[WT_MAX_SUBREGIONS_LIMIT];
	ULONG jSkiped = 0, kBonded = 0, mToSort = 0;

	memset(ulSkipedList, 0, sizeof(ulSkipedList));
	memset(ulBondedList, 0, sizeof(ulBondedList));
	memset(ulToSortList, 0, sizeof(ulToSortList));

	for (i = 0; i<GetTotalSubRegions(); i++)
	{
		i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5

		ulRegionNo = GetSortRegion(i);
		switch (GetRegionState(ulRegionNo))
		{
		case WT_SUBREGION_STATE_SKIPPED:
			ulSkipedList[jSkiped].m_ulAssistRegion = GetHelpRegion(i);
			ulSkipedList[jSkiped].m_ulTargetRegion = GetSortRegion(i);
			jSkiped++;
			break;
		case WT_SUBREGION_STATE_BONDED:
			ulBondedList[kBonded].m_ulAssistRegion = GetHelpRegion(i);
			ulBondedList[kBonded].m_ulTargetRegion = GetSortRegion(i);
			kBonded++;
			break;
		default:
			ulToSortList[mToSort].m_ulAssistRegion = GetHelpRegion(i);
			ulToSortList[mToSort].m_ulTargetRegion = GetSortRegion(i);
			mToSort++;
			break;
		}
	}

	ULONG k = 0;
	for (i = 0; i < mToSort; i++)
	{
		k = min(k, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5
		SetPickOrder(k, ulToSortList[i].m_ulTargetRegion, ulToSortList[i].m_ulAssistRegion);
		k++;
	}
	for (i = 0; i < jSkiped; i++)
	{
		k = min(k, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5
		SetPickOrder(k, ulSkipedList[i].m_ulTargetRegion, ulSkipedList[i].m_ulAssistRegion);
		k++;
	}
	for (i = 0; i < kBonded; i++)
	{
		k = min(k, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5
		SetPickOrder(k, ulBondedList[i].m_ulTargetRegion, ulBondedList[i].m_ulAssistRegion);
		k++;
	}

//  2018.6.8 disable it
//	m_ulPickIndex = 0;	// after alignment, re-organize the order, set to first
}


VOID WT_CSubRegionsInfo::FindFirstPickingRegion(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol)
{
	for (LONG i = 0; i < (LONG)GetTotalSubRegions(); i++)
	{
		i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);

		ULONG ulRegionNo = GetSortRegion(i);
		BOOL bCanSortingRegion = IsCanSortingRegion(bHalfSortMode, bRowModeSeparateHalfMap, ulHalfMaxRow, ulHalfMaxCol, ulRegionNo);
		if (bCanSortingRegion)
		{
			m_ulPickIndex = i;
			return;
		}
	}
}


VOID WT_CSubRegionsInfo::AssignRegionOuterList(CONST BOOL bCtrBased)
{
	ULONG i, j, k, m;

	if (bCtrBased == FALSE)
	{
		for (i = 0; i < GetTotalSubRegions(); i++)
		{
			i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);

			SetPickOrder(i, i+1, i+1);
		}
		return ;
	}

	DOUBLE  dRowPitch = 0, dColPitch = 0, dCurDist = 0, dFarDist = 0;
	DOUBLE	dCtrRow = 0, dCtrCol = 0, dCurRow = 0, dCurCol = 0;

	ULONG ulFarRegion = 1, ulCurRegion = 1, ulRegionJ = 1;
	ULONG nRegionList[WT_MAX_SUBREGIONS_LIMIT];
	ULONG ulCurRow = 0, ulCurCol = 0;

	for (i = 0; i < WT_MAX_SUBREGIONS_LIMIT; i++)
	{
		nRegionList[i] = i+1;
	}

	dCtrRow = (m_ulSubRows - 1) * 1.0 / 2.0;
	dCtrCol = (m_ulSubCols - 1) * 1.0 / 2.0;

	i = 0;
	while ( 1 )
	{
		for (k = 0; k < 2; k++)
		{
			dFarDist = -1;
			ulFarRegion = 0;
			ulRegionJ = WT_MAX_SUBREGIONS_LIMIT-1;

			for (j = 0; j < GetTotalSubRegions(); j++)
			{
				if (k == 0)
				{
					m = j;
				}
				else
				{
					m = GetTotalSubRegions()-1-j;
				}

				m = min(m, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5
				ulCurRegion = nRegionList[m];
				if (ulCurRegion > 0 && GetRegionSub(ulCurRegion, ulCurRow, ulCurCol))
				{
					dCurRow = ulCurRow;
					dCurCol = ulCurCol;
					dRowPitch = dCurRow - dCtrRow;
					dColPitch = dCurCol - dCtrCol;
					dCurDist = dRowPitch*dRowPitch + dColPitch*dColPitch;
					if (dFarDist < dCurDist)
					{
						dFarDist = dCurDist;
						ulFarRegion = ulCurRegion;
						ulRegionJ = m;
					}
				}
			}

			nRegionList[ulRegionJ] = 0;
			SetPickOrder(i, ulFarRegion, ulFarRegion);
			i++;
			if (i >= GetTotalSubRegions())
			{
				break;
			}
			i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);	//Klocwork	//v4.02T5
		}
		if (i >= GetTotalSubRegions())
		{
			break;
		}
	}
}

//	427TX	4
VOID WT_CSubRegionsInfo::InitRegionFocus()
{
	for (ULONG i = 0; i < WT_MAX_SUBREGIONS_LIMIT; i++)
	{
		m_pRegions[i].m_lTL_FocusLevel	= WT_AF_LEVEL_INIT;
		m_pRegions[i].m_lTR_FocusLevel	= WT_AF_LEVEL_INIT;
		m_pRegions[i].m_lBL_FocusLevel	= WT_AF_LEVEL_INIT;
		m_pRegions[i].m_lBR_FocusLevel	= WT_AF_LEVEL_INIT;
	}
}

BOOL WT_CSubRegionsInfo::GetRegionFocus(ULONG ulRegionNo, LONG &lTLFocusLevel, LONG &lTRFocusLevel, 
														  LONG &lBLFocusLevel, LONG &lBRFocusLevel)
{
	if (IsInvalidRegion(ulRegionNo))
	{
		return FALSE;
	}

	lTLFocusLevel = m_pRegions[ulRegionNo - 1].m_lTL_FocusLevel;
	lTRFocusLevel = m_pRegions[ulRegionNo - 1].m_lTR_FocusLevel;
	lBLFocusLevel = m_pRegions[ulRegionNo - 1].m_lBL_FocusLevel;
	lBRFocusLevel = m_pRegions[ulRegionNo - 1].m_lBR_FocusLevel;

	return TRUE;
}

BOOL WT_CSubRegionsInfo::SetRegionFocusTL(ULONG ulRegionNo, LONG lTLFocusLevel)
{
	if (IsInvalidRegion(ulRegionNo))
	{
		return FALSE;
	}

	m_pRegions[ulRegionNo - 1].m_lTL_FocusLevel = lTLFocusLevel;

	return TRUE;
}


BOOL WT_CSubRegionsInfo::SetRegionFocusTR(ULONG ulRegionNo, LONG lTRFocusLevel)
{
	if (IsInvalidRegion(ulRegionNo))
	{
		return FALSE;
	}

	m_pRegions[ulRegionNo - 1].m_lTR_FocusLevel = lTRFocusLevel;

	return TRUE;
}

BOOL WT_CSubRegionsInfo::SetRegionFocusBL(ULONG ulRegionNo, LONG lBLFocusLevel)
{
	if (IsInvalidRegion(ulRegionNo))
	{
		return FALSE;
	}

	m_pRegions[ulRegionNo - 1].m_lBL_FocusLevel = lBLFocusLevel;

	return TRUE;
}

BOOL WT_CSubRegionsInfo::SetRegionFocusBR(ULONG ulRegionNo, LONG lBRFocusLevel)
{
	if (IsInvalidRegion(ulRegionNo))
	{
		return FALSE;
	}

	m_pRegions[ulRegionNo - 1].m_lBR_FocusLevel = lBRFocusLevel;

	return TRUE;
}


BOOL WT_CSubRegionsInfo::WriteRegionStateFile()
{
	FILE *fp;
	errno_t nErr = fopen_s(&fp, "c:\\mapsorter\\exe\\regionlist.csv", "w");
	if ((nErr != 0) || (fp == NULL))
	{
		return FALSE;
	}

	CString szState;
	for (ULONG i = 0; i < GetTotalSubRegions(); i++)
	{
		ULONG ulRegion = min(i, WT_MAX_SUBREGIONS_LIMIT - 1);	
		ulRegion = GetSortRegion(ulRegion);

		ULONG ulRegionState = GetRegionState(ulRegion);

		switch( ulRegionState )
		{
		case WT_SUBREGION_STATE_INIT:
			szState = "Waiting";
			break;
		case WT_SUBREGION_STATE_BONDING:
			szState = "Sorting";
			break;
		case WT_SUBREGION_STATE_SKIPPED:
			szState = "Skipped";
			break;
		case WT_SUBREGION_STATE_BONDED:
		default:
			szState = "Sorted";
			break;
		}
		fprintf(fp, "[%02ld]       %s,", ulRegion, (LPCTSTR) szState);	//Klocwork
	}
	fclose(fp);

	return TRUE;
}


VOID WT_CSubRegionsInfo::GetRegionStateName(const ULONG ulRegionIndex)
{
	CString szOrder, szState;

	szOrder.Empty();
	szState.Empty();

	ULONG ulRegion = min(ulRegionIndex, WT_MAX_SUBREGIONS_LIMIT - 1);	
	ulRegion = GetSortRegion(ulRegion);

	ULONG ulRegionState = GetRegionState(ulRegion);
	szOrder.Format("[%02d]=>", ulRegion);
	m_szRegionOrderList_HMI += szOrder;

	switch (ulRegionState)
	{
		case WT_SUBREGION_STATE_INIT:
			szState = "Wait  ";
			break;
		case WT_SUBREGION_STATE_BONDING:
			szState = "Going ";
			break;
		case WT_SUBREGION_STATE_SKIPPED:
			szState = "Skip  ";
			break;
		case WT_SUBREGION_STATE_BONDED:
		default:
			szState = "Over  ";
			break;
	}
	m_szRegionStateList_HMI += szState;
}


VOID WT_CSubRegionsInfo::GetRegionStateNameList()
{
	m_szRegionOrderList_HMI.Empty();
	m_szRegionStateList_HMI.Empty();
	ULONG ulTgtIndex = GetTargetRegionIndex();

	for (ULONG i = ulTgtIndex; i < GetTotalSubRegions(); i++)
	{
		GetRegionState(i);
	}

	for (ULONG i = 0; i < ulTgtIndex; i++)
	{
		GetRegionState(i);
	}
}



//================================================================
// Function Name: 		GetRegionPickingSequenceList
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the picking sequence list from the start region to the end region
// Return:				the picking sequence 
// Remarks:				called by SetRegionPickingSequence
//================================================================
CString WT_CSubRegionsInfo::GetRegionPickingSequenceList(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol, 
														 const LONG lStartPickingRegion, const LONG lEndPickingRegion, BOOL &bFirstRegion)
{
	CString szMsg, szPickingSequence;
	ULONG ulSubRow = 0, ulSubCol = 0;

	for (LONG i = lStartPickingRegion; i < lEndPickingRegion + 1; i++)
	{
		i = min(i, WT_MAX_SUBREGIONS_LIMIT-1);

		ULONG ulRegion = GetSortRegion(i);
		BOOL bCanSortingRegion = IsCanSortingRegion(bHalfSortMode, bRowModeSeparateHalfMap, ulHalfMaxRow, ulHalfMaxCol, ulRegion);
		if (bCanSortingRegion && GetRegionSub(ulRegion, ulSubRow, ulSubCol))
		{
			if (bFirstRegion == FALSE)
			{
				SetTargetRegion(ulRegion);
				bFirstRegion = TRUE;
			}
			szMsg.Format("[%d,%d]", ulSubRow, ulSubCol);
			szPickingSequence +=szMsg;
		}
	}

	return szPickingSequence;
}



//	427TX	4
//	Chip Prober probing position offset PO
WT_CPO_PointsInfo* WT_CPO_PointsInfo::m_pInstance = NULL;

LONG WT_CPO_PointsInfo::BiLinearInterpolation(LONG lX1, LONG lY1, LONG lX2, LONG lY2,
										LONG lZX1Y1, LONG lZX2Y1, LONG lZX1Y2, LONG lZX2Y2,
										CONST LONG lX0, CONST LONG lY0)
{
	LONG lZY1 = (lZX1Y1 + lZX2Y1)/2;
	LONG lZY2 = (lZX1Y2 + lZX2Y2)/2;
	if( lX1!=lX2 )
	{
		lZY1 = ((lX2-lX0)*lZX1Y1 + (lX0-lX1)*lZX2Y1)/(lX2-lX1);
		lZY2 = ((lX2-lX0)*lZX1Y2 + (lX0-lX1)*lZX2Y2)/(lX2-lX1);
	}

	LONG lZ0 = (lZY1 + lZY2)/2;
	if( lY2!=lY1 )
	{
		lZ0 = ((lY2-lY0)*lZY1 + (lY0-lY1)*lZY2)/(lY2-lY1);
	}

	return lZ0;
}

WT_CPO_PointsInfo* WT_CPO_PointsInfo::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new WT_CPO_PointsInfo();
	return m_pInstance;
}

WT_CPO_PointsInfo::WT_CPO_PointsInfo()
{
	SetGridSpan(5000, 5000);
	InitPOPoints(10, 10);
	InitDCPoints();
}

WT_CPO_PointsInfo::~WT_CPO_PointsInfo()
{
}

VOID WT_CPO_PointsInfo::InitPOPoints(ULONG ulGridRows, ULONG ulGridCols)
{
	m_bPODone		= FALSE;
	m_bPOState		= FALSE;
	m_ulGridRowNum	= ulGridRows;
	m_ulGridColNum	= ulGridCols;
	for(ULONG i=0; i<WT_MAX_POPOINTS_LIMIT; i++)
	{
		m_pPoPoints[i].m_lState	= 0;
		m_pPoPoints[i].m_lScanX	= 0;
		m_pPoPoints[i].m_lScanY	= 0;
		m_pPoPoints[i].m_lMoveX = 0;
		m_pPoPoints[i].m_lMoveY = 0;
		m_pPoPoints[i].m_lPO_X	= WT_PO_OFFSET_INIT;
		m_pPoPoints[i].m_lPO_Y	= WT_PO_OFFSET_INIT;
	}
}

BOOL WT_CPO_PointsInfo::SetPointPO(ULONG ulPointNo, LONG lScanX, LONG lScanY, LONG lPO_X, LONG lPO_Y, LONG lState)
{
	if( ulPointNo>WT_MAX_POPOINTS_LIMIT || ulPointNo<=0  )
		return FALSE;

	m_pPoPoints[ulPointNo-1].m_lState	= lState;
	m_pPoPoints[ulPointNo-1].m_lScanX	= lScanX;
	m_pPoPoints[ulPointNo-1].m_lScanY	= lScanY;
	m_pPoPoints[ulPointNo-1].m_lPO_X	= lPO_X;
	m_pPoPoints[ulPointNo-1].m_lPO_Y	= lPO_Y;

	return TRUE;
}

BOOL WT_CPO_PointsInfo::SetPointSP(ULONG ulPointNo, LONG lMoveX, LONG lMoveY)
{
	if( ulPointNo>WT_MAX_POPOINTS_LIMIT || ulPointNo<=0  )
		return FALSE;

	m_pPoPoints[ulPointNo-1].m_lMoveX	= lMoveX;
	m_pPoPoints[ulPointNo-1].m_lMoveY	= lMoveY;

	return TRUE;
}

LONG WT_CPO_PointsInfo::GetPointPO(ULONG ulPointNo, LONG &lScanX, LONG &lScanY, LONG &lPO_X, LONG &lPO_Y)
{
	if( ulPointNo>WT_MAX_POPOINTS_LIMIT || ulPointNo<=0 )
		return 0;

	lScanX = m_pPoPoints[ulPointNo-1].m_lScanX;
	lScanY = m_pPoPoints[ulPointNo-1].m_lScanY;
	lPO_X  = m_pPoPoints[ulPointNo-1].m_lPO_X;
	lPO_Y  = m_pPoPoints[ulPointNo-1].m_lPO_Y;
	return m_pPoPoints[ulPointNo-1].m_lState;
}

LONG WT_CPO_PointsInfo::GetPointSP(ULONG ulPointNo, LONG &lMoveX, LONG &lMoveY)
{
	if( ulPointNo>WT_MAX_POPOINTS_LIMIT || ulPointNo<=0 )
		return 0;

	lMoveX = m_pPoPoints[ulPointNo-1].m_lMoveX;
	lMoveY = m_pPoPoints[ulPointNo-1].m_lMoveY;
	return m_pPoPoints[ulPointNo-1].m_lState;
}

BOOL WT_CPO_PointsInfo::GetPOPoint_Region(CONST LONG lScanX, CONST LONG lScanY, ULONG &ulPointUL, ULONG &ulPointUR, ULONG &ulPointLL, ULONG &ulPointLR)
{
	LONG lCheckX = 0, lCheckY = 0, lPO_X, lPO_Y;

	BOOL bFindUL = FALSE, bFindUR = FALSE, bFindLL = FALSE, bFindLR = FALSE;
	DOUBLE dNearDistUL = -1, dNearDistUR = -1, dNearDistLL = -1, dNearDistLR = -1, dDist = 0;
	for(ULONG i=1; i<=WT_MAX_POPOINTS_LIMIT; i++)
	{
		if( GetPointPO(i, lCheckX, lCheckY, lPO_X, lPO_Y)==FALSE )
			continue;
		DOUBLE dDistX = lCheckX-lScanX;
		DOUBLE dDistY = lCheckY-lScanY;
		dDist = sqrt(dDistX*dDistX + dDistY*dDistY);
		if( lCheckX>=lScanX && lCheckY>=lScanY )
		{
			if( dNearDistUL==-1 )
			{
				dNearDistUL = dDist;
				ulPointUL = i;
			}
			if( dNearDistUL>dDist )
			{
				ulPointUL = i;
				dNearDistUL = dDist;
			}
			bFindUL = TRUE;
		}
		if( lCheckX<=lScanX && lCheckY>=lScanY )
		{
			if( dNearDistUR==-1 )
			{
				dNearDistUR = dDist;
				ulPointUR = i;
			}
			if( dNearDistUR>dDist )
			{
				ulPointUR = i;
				dNearDistUR = dDist;
			}
			bFindUR = TRUE;
		}
		if( lCheckX>=lScanX && lCheckY<=lScanY )
		{
			if( dNearDistLL==-1 )
			{
				dNearDistLL = dDist;
				ulPointLL = i;
			}
			if( dNearDistLL>dDist )
			{
				ulPointLL = i;
				dNearDistLL = dDist;
			}
			bFindLL = TRUE;
		}
		if( lCheckX<=lScanX && lCheckY<=lScanY )
		{
			if( dNearDistLR==-1 )
			{
				dNearDistLR = dDist;
				ulPointLR = i;
			}
			if( dNearDistLR>dDist )
			{
				ulPointLR = i;
				dNearDistLR = dDist;
			}
			bFindLR = TRUE;
		}
	}

	if( ulPointUL!=0 && (ulPointUL+1)==ulPointUR && 
		(ulPointUL+m_ulGridColNum+1)==ulPointLL && (ulPointLL+1)==ulPointLR )
	{
		return TRUE;
	}

	if( bFindUL )
	{
		GetPointPO(ulPointUL, lCheckX, lCheckY, lPO_X, lPO_Y);
		if( labs(lCheckX-lScanX)<=labs(m_lGridSpanX) && labs(lCheckY-lScanY)<=labs(m_lGridSpanY) )
		{
			ulPointUR = ulPointUL + 1;
			ulPointLL = ulPointUL + m_ulGridColNum + 1;
			ulPointLR = ulPointLL + 1;
			return TRUE;
		}
	}

	if( bFindLR )
	{
		GetPointPO(ulPointLR, lCheckX, lCheckY, lPO_X, lPO_Y);
		if( labs(lCheckX-lScanX)<=labs(m_lGridSpanX) && labs(lCheckY-lScanY)<=labs(m_lGridSpanY) )
		{
			ulPointLL = ulPointLR - 1;
			ulPointUL = ulPointLL - m_ulGridColNum - 1;
			ulPointUR = ulPointUL + 1;
			return TRUE;
		}
	}

	if( bFindLL )
	{
		GetPointPO(ulPointLL, lCheckX, lCheckY, lPO_X, lPO_Y);
		if( labs(lCheckX-lScanX)<=labs(m_lGridSpanX) && labs(lCheckY-lScanY)<=labs(m_lGridSpanY) )
		{
			ulPointLR = ulPointLL + 1;
			ulPointUL = ulPointLL - m_ulGridColNum - 1;
			ulPointUR = ulPointUL + 1;
			return TRUE;
		}
	}

	if( bFindUR )
	{
		GetPointPO(ulPointUR, lCheckX, lCheckY, lPO_X, lPO_Y);
		if( labs(lCheckX-lScanX)<=labs(m_lGridSpanX) && labs(lCheckY-lScanY)<=labs(m_lGridSpanY) )
		{
			ulPointUL = ulPointUR - 1;
			ulPointLL = ulPointUL + m_ulGridColNum + 1;
			ulPointLR = ulPointLL + 1;
			return TRUE;
		}
	}

	if( !bFindUL )
	{
		if( bFindUR )
			ulPointUL = ulPointUR;
		else if( bFindLL )
			ulPointUL = ulPointLL;
		else if( bFindLR )
			ulPointUL = ulPointLR;
	}
	if( !bFindUR )
	{
		if( bFindUL )
			ulPointUR = ulPointUL;
		else if( bFindLR )
			ulPointUR = ulPointLR;
		else if( bFindLL )
			ulPointUR = ulPointLL;
	}
	if( !bFindLL )
	{
		if( bFindUL )
			ulPointLL = ulPointUL;
		else if( bFindLR )
			ulPointLL = ulPointLR;
		else if( bFindUR )
			ulPointLL = ulPointUR;
	}
	if( !bFindLR )
	{
		if( bFindLL )
			ulPointLR = ulPointLL;
		else if( bFindUR )
			ulPointLR = ulPointUR;
		else if( bFindUL )
			ulPointLR = ulPointUL;
	}

	return TRUE;
}


BOOL	WT_CPO_PointsInfo::GetNearestPointPO(LONG lScanX, LONG lScanY, LONG &lPointX, LONG &lPointY, LONG &lPO_X, LONG &lPO_Y, ULONG &ulPointNo)
{
	DOUBLE lNearDist = -1, lDist = 0;

	ulPointNo = 0;
	for(ULONG i=1; i<=WT_MAX_POPOINTS_LIMIT; i++)
	{
		LONG lCheckX = 0, lCheckY = 0, lTempX = 0, lTempY = 0;
		if( GetPointPO(i, lCheckX, lCheckY, lTempX, lTempY)==FALSE )
		{
			continue;
		}

		DOUBLE dDistX = lCheckX-lScanX;
		DOUBLE dDistY = lCheckY-lScanY;
		lDist = sqrt(dDistX*dDistX + dDistY*dDistY);
		if( lNearDist==-1 )
		{
			ulPointNo = i;
			lNearDist = lDist;
		}
		if( lNearDist>lDist )
		{
			lNearDist = lDist;
		}
	}

	if( ulPointNo!=0 )
	{
		GetPointPO(ulPointNo, lPointX, lPointY, lPO_X, lPO_Y);
		return TRUE;
	}

	return FALSE;
}

BOOL	WT_CPO_PointsInfo::GetPOState()
{
	return m_bPOState;
}

BOOL	WT_CPO_PointsInfo::SetPOState(BOOL bState)
{
	m_bPOState = bState;
	return TRUE;
}

VOID WT_CPO_PointsInfo::SetPODone(BOOL bDone)
{
    m_bPODone = bDone;
}

BOOL WT_CPO_PointsInfo::GetPODone()
{
	return m_bPODone;
}

VOID	WT_CPO_PointsInfo::SetGridSpan(LONG lSpanX, LONG lSpanY)
{
	m_lGridSpanX	= lSpanX;
	m_lGridSpanY	= lSpanY;
}

VOID	WT_CPO_PointsInfo::GetGridSpan(LONG &lSpanX, LONG &lSpanY)
{
	lSpanX = m_lGridSpanX;
	lSpanY = m_lGridSpanY;
}

// local die calibration factor
VOID WT_CPO_PointsInfo::InitDCPoints()
{
	m_bDCDone	= FALSE;
	m_bDCState	= FALSE;
	for(ULONG i=0; i<WT_MAX_POPOINTS_LIMIT; i++)
	{
		m_pDcPoints[i].m_dDieClbX	= 0;
		m_pDcPoints[i].m_dDieClbXY	= 0;
		m_pDcPoints[i].m_dDieClbY	= 0;
		m_pDcPoints[i].m_dDieClbYX	= 0;
		m_pDcPoints[i].m_lDCState = 0;
		m_pDcPoints[i].m_lDCX	= 0;
		m_pDcPoints[i].m_lDCY	= 0;
	}
}

BOOL WT_CPO_PointsInfo::SetPointDC(ULONG ulPointNo, LONG lScanX, LONG lScanY, DOUBLE dClbX, DOUBLE dClbXY, DOUBLE dClbY, DOUBLE dClbYX, LONG lState)
{
	if( ulPointNo>WT_MAX_POPOINTS_LIMIT || ulPointNo<=0  )
		return FALSE;

	m_pDcPoints[ulPointNo-1].m_dDieClbX		= dClbX;
	m_pDcPoints[ulPointNo-1].m_dDieClbXY	= dClbXY;
	m_pDcPoints[ulPointNo-1].m_dDieClbY		= dClbY;
	m_pDcPoints[ulPointNo-1].m_dDieClbYX	= dClbYX;
	m_pDcPoints[ulPointNo-1].m_lDCState = lState;
	m_pDcPoints[ulPointNo-1].m_lDCX	= lScanX;
	m_pDcPoints[ulPointNo-1].m_lDCY	= lScanY;

	return TRUE;
}

LONG WT_CPO_PointsInfo::GetPointDC(ULONG ulPointNo, LONG &lScanX, LONG &lScanY, DOUBLE &dClbX, DOUBLE &dClbXY, DOUBLE &dClbY, DOUBLE &dClbYX)
{
	if( ulPointNo>WT_MAX_POPOINTS_LIMIT || ulPointNo<=0 )
		return 0;

	lScanX	= m_pDcPoints[ulPointNo-1].m_lDCX;
	lScanY	= m_pDcPoints[ulPointNo-1].m_lDCY;
	dClbX	= m_pDcPoints[ulPointNo-1].m_dDieClbX;
	dClbXY	= m_pDcPoints[ulPointNo-1].m_dDieClbXY;
	dClbY	= m_pDcPoints[ulPointNo-1].m_dDieClbY;
	dClbYX	= m_pDcPoints[ulPointNo-1].m_dDieClbYX;

	return	m_pDcPoints[ulPointNo-1].m_lDCState;
}

LONG WT_CPO_PointsInfo::GetPointDC(ULONG ulPointNo, DOUBLE &dClbX, DOUBLE &dClbXY, DOUBLE &dClbY, DOUBLE &dClbYX)
{
	LONG lScanX, lScanY;

	return GetPointDC(ulPointNo, lScanX, lScanY, dClbX, dClbXY, dClbY, dClbYX);
}

LONG WT_CPO_PointsInfo::GetPointDC(ULONG ulPointNo, LONG &lScanX, LONG &lScanY)
{
	DOUBLE dClbX, dClbXY, dClbY, dClbYX;

	return GetPointDC(ulPointNo, lScanX, lScanY, dClbX, dClbXY, dClbY, dClbYX);
}

ULONG WT_CPO_PointsInfo::GetNearestPointDC(LONG lScanX, LONG lScanY)
{
	DOUBLE dNearDist = -1, dDist = 0;

	ULONG ulPointNo = 0;
	for(ULONG i=1; i<=WT_MAX_POPOINTS_LIMIT; i++)
	{
		LONG lCheckX = 0, lCheckY = 0;
		if( GetPointDC(i, lCheckX, lCheckY)!=1 )
		{
			continue;
		}

		DOUBLE dDistX = lCheckX-lScanX;
		DOUBLE dDistY = lCheckY-lScanY;
		dDist = sqrt(dDistX*dDistX + dDistY*dDistY);
		if( dNearDist==-1 )
		{
			ulPointNo = i;
			dNearDist = dDist;
		}
		if( dNearDist>dDist )
		{
			ulPointNo = i;
			dNearDist = dDist;
		}
	}

	return ulPointNo;
}

BOOL	WT_CPO_PointsInfo::GetDCState()
{
	return m_bDCState;
}

BOOL	WT_CPO_PointsInfo::SetDCState(BOOL bState)
{
	m_bDCState = bState;
	return TRUE;
}

VOID WT_CPO_PointsInfo::SetDCDone(BOOL bDone)
{
	m_bDCDone = bDone;
}

BOOL WT_CPO_PointsInfo::GetDCDone()
{
	return m_bDCDone;
}
// local die calibration factor

//	Chip Prober probing position offset PO

//================================================================
// End of WT_CSubRegions.cpp
//================================================================
