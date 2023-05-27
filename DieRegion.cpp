//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "DieRegion.h"
#include "io.h"
#include "WaferMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//================================================================
// Constructor / destructor implementation section
//================================================================
WM_CRegion::WM_CRegion()
{
	m_ulUpperLeftRow	= 0;
	m_ulUpperLeftCol	= 0;
	m_ulLowerRightRow	= 0;
	m_ulLowerRightCol	= 0;

	m_bEmptyRegion		= FALSE;
	m_ulRegionNo		= 0;
	m_ulRegionRow		= 0;
	m_ulRegionCol		= 0;

	m_alRegionStatisticsList.RemoveAll();
}

WM_CRegion::~WM_CRegion()
{
	m_alRegionStatisticsList.RemoveAll();
}


VOID WM_CRegion::SetRegionNo(const ULONG ulRegionNo, const ULONG ulRegionRow, const ULONG ulRegionCol)
{
	m_ulRegionNo		= ulRegionNo;
	m_ulRegionRow		= ulRegionRow;
	m_ulRegionCol		= ulRegionCol;
}

ULONG WM_CRegion::GetRegionNum()
{
	return m_ulRegionNo;
}

ULONG WM_CRegion::GetRegionRow()
{
	return m_ulRegionRow;
}

ULONG WM_CRegion::GetRegionCol()
{
	return m_ulRegionCol;
}

VOID WM_CRegion::SetRegion(const ULONG ulUpperLeftRow, const ULONG ulUpperLeftCol, const ULONG ulLowerRightRow, const ULONG ulLowerRightCol)
{
	m_ulUpperLeftRow	= ulUpperLeftRow;
	m_ulUpperLeftCol	= ulUpperLeftCol;
	m_ulLowerRightRow	= ulLowerRightRow;
	m_ulLowerRightCol	= ulLowerRightCol;
}


ULONG WM_CRegion::GetRegionTopRow()
{
	return m_ulUpperLeftRow;
}


ULONG WM_CRegion::GetRegionLeftCol()
{
	return m_ulUpperLeftCol;
}


ULONG WM_CRegion::GetRegionBottomRow()
{
	return m_ulLowerRightRow;
}


ULONG WM_CRegion::GetRegionRightCol()
{
	return m_ulLowerRightCol;
}


VOID WM_CRegion::GetRegionUpperRight(ULONG &ulUpperRightRow, ULONG &ulUpperRightCol)
{
	ulUpperRightRow = m_ulUpperLeftRow;
	ulUpperRightCol = m_ulLowerRightCol;
}


VOID WM_CRegion::GetRegionUpperLeft(ULONG &ulUpperLeftRow, ULONG &ulUpperLeftCol)
{
	ulUpperLeftRow = m_ulUpperLeftRow;
	ulUpperLeftCol = m_ulUpperLeftCol;
}

VOID WM_CRegion::GetRegionLowerLeft(ULONG &ulLowerLeftRow, ULONG &ulLowerLeftCol)
{
	ulLowerLeftRow = m_ulLowerRightRow;
	ulLowerLeftCol = m_ulUpperLeftCol;
}

VOID WM_CRegion::GetRegionLowerRight(ULONG &ulLowerRightRow, ULONG &ulLowerRightCol)
{
	ulLowerRightRow = m_ulLowerRightRow;
	ulLowerRightCol = m_ulLowerRightCol;
}


BOOL WM_CRegion::IsEmptyRegion()
{
	return m_bEmptyRegion;
}


VOID WM_CRegion::SetStatistics(CUIntArray *paulRegionGradeList, CArray< long, long > *palRegionTotalList)
{
	REGION_STATISTICS stRegionStatisticsItem;

	UCHAR ucRegionGrade = 0;
	for (LONG i = 0; i < (*paulRegionGradeList).GetSize(); i++)
	{
		stRegionStatisticsItem.ucGrade			= (UCHAR)(*paulRegionGradeList)[i];
		stRegionStatisticsItem.ulTotalCount		= (*palRegionTotalList)[stRegionStatisticsItem.ucGrade];
		stRegionStatisticsItem.ulLeftCount		= 0;
		stRegionStatisticsItem.ulPickedCount	= 0;
		m_alRegionStatisticsList.Add(stRegionStatisticsItem);
	}
}


VOID WM_CRegion::GetRegionStatistics(UCHAR ucGrade, ULONG &uleft, ULONG ulTotal)
{
	REGION_STATISTICS stRegionStatisticsItem;
	for (LONG i = 0; i < m_alRegionStatisticsList.GetSize(); i++)
	{
		stRegionStatisticsItem = (REGION_STATISTICS)m_alRegionStatisticsList[i];
		if (stRegionStatisticsItem.ucGrade == ucGrade)
		{
			uleft	= stRegionStatisticsItem.ulLeftCount;
			ulTotal	= stRegionStatisticsItem.ulTotalCount;
		}
	}
}