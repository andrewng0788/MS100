#ifndef WM_Region_H
#define WM_Region_H

typedef struct {
		UCHAR ucGrade;
		ULONG ulTotalCount;
		ULONG ulPickedCount;
		ULONG ulLeftCount;
} REGION_STATISTICS;

//================================================================
// Class definition
//================================================================
class WM_CRegion : public CObject
{
public:
	WM_CRegion(); //Constructor
	~WM_CRegion(); //Deconstructor

	VOID SetRegionNo(const ULONG ulRegionNo, const ULONG ulRegionRow, const ULONG ulRegionCol);
	ULONG GetRegionNum();
	ULONG GetRegionRow();
	ULONG GetRegionCol();

	VOID SetRegion(const ULONG ulUpperLeftRow, const ULONG ulUpperLeftCol, const ULONG ulLowerRightRow, const ULONG ulLowerRightCol);

	ULONG GetRegionTopRow();
	ULONG GetRegionBottomRow();
	ULONG GetRegionLeftCol();
	ULONG GetRegionRightCol();

	VOID GetRegionUpperRight(ULONG &ulUpperRightRow, ULONG &ulUpperRightCol);
	VOID GetRegionUpperLeft(ULONG &ulUpperLeftRow, ULONG &ulUpperLeftCol);
	VOID GetRegionLowerLeft(ULONG &ulLowerLeftRow, ULONG &ulLowerLeftCol);
	VOID GetRegionLowerRight(ULONG &ulLowerRightRow, ULONG &ulLowerRightCol);

	BOOL IsEmptyRegion();

	VOID SetStatistics(CUIntArray *paulRegionGradeList, CArray< long, long > *palRegionTotalList);
	VOID GetRegionStatistics(UCHAR ucGrade, ULONG &uleft, ULONG ulTotal);
protected:
	CArray<REGION_STATISTICS, REGION_STATISTICS> m_alRegionStatisticsList;

	ULONG m_ulUpperLeftRow;
	ULONG m_ulUpperLeftCol;
	ULONG m_ulLowerRightRow;
	ULONG m_ulLowerRightCol;

	BOOL  m_bEmptyRegion;
	ULONG m_ulRegionNo;
	ULONG m_ulRegionRow;
	ULONG m_ulRegionCol;
};

#endif  // WM_CRegion

//================================================================
// End of file Region.h
//================================================================