#ifndef WT_SUB_REGION_H
#define WT_SUB_REGION_H

#include "DieRegion.h"
//================================================================
// struct, enum definition
//================================================================

typedef struct
{
	ULONG	m_ulState;
	ULONG	m_ulULRow;
	ULONG	m_ulULCol;
	ULONG	m_ulLRRow;
	ULONG	m_ulLRCol;
	//	427TX	4
	LONG	m_lTL_FocusLevel;
	LONG	m_lTR_FocusLevel;
	LONG	m_lBL_FocusLevel;
	LONG	m_lBR_FocusLevel;
	//	427TX	4
} WT_SR_PROPERTY;

typedef struct
{
	ULONG	m_ulAssistRegion;
	ULONG	m_ulTargetRegion;
} WT_SR_PICKORDER;

#define	WT_SUBREGION_STATE_INIT			1
#define	WT_SUBREGION_STATE_BONDING		2
#define	WT_SUBREGION_STATE_BONDED		3
#define	WT_SUBREGION_STATE_SKIPPED		4

#define	WT_SUBREGION_INIT_ERROR			0
#define	WT_SUBREGION_INIT_LOADMAP		1
#define	WT_SUBREGION_INIT_ALIGN			2

#define	WT_MAP_REGION_MAX_ROW			50
#define	WT_MAP_REGION_MAX_COL			50

#define	WT_MAX_SUBREGIONS_LIMIT			(WT_MAP_REGION_MAX_ROW*WT_MAP_REGION_MAX_COL)

#define	WT_AF_LEVEL_INIT				-666666
#define	WT_AF_LEVEL_FAIL				-888888


//================================================================
// Class definition
//================================================================
class WT_CSubRegionsInfo : public CObject
{
public:
	static WT_CSubRegionsInfo* Instance();

	LONG _round(double val);
	BOOL IsRegionPickingMode();
	ULONG GetSubRows();
	ULONG GetSubCols();
	VOID GetRegionRowColumnNum(const CString szAlgorithm, const ULONG ulMaxRow, const ULONG ulMaxCol, LONG &lSubRowsInRegion, LONG &lSubColsInRegion);

	VOID SetCurrentRegionState_HMI(const CString szCurrentRegionState);
	VOID SetCurrentSortingRegion_HMI(const ULONG ulCurrentSortingRegion);

	VOID SetManualAlignRegion(const BOOL bManualAlignRegion);
	BOOL IsManualAlignRegion();

	VOID SetRegionAligned(const BOOL bRegionAligned);
	BOOL IsRegionAligned();

	VOID SetRegionEnd(const BOOL bIsRegionEnd);
	BOOL IsRegionEnd();

	VOID SetRegionEnding(const BOOL bIsRegionEnding);
	BOOL IsRegionEnding();

	VOID SetRegionManualConfirm(const BOOL bRegionManualConfirm);
	BOOL IsRegionManualConfirm();

	VOID CalcPrescanRegionSize(const ULONG ulUlRow, const ULONG ulUlCol, const ULONG ulLrRow, const ULONG ulLrCol,	
						const ULONG ulRow, const ULONG ulCol, const LONG lX, const LONG lY, const LONG nEdgeX, const LONG nEdgeY,
						const LONG lDiePitchX_X, const LONG lDiePitchX_Y, const LONG lDiePitchY_Y, const LONG lDiePitchY_X);
	INT GetRegionWftULX();
	INT GetRegionWftULY();
	INT GetRegionWftLRX();
	INT GetRegionWftLRY();
	BOOL IsScanWithinMapRegionRange(INT nPosnX, INT nPosnY);

	VOID	InitRegion();

	//	427TX	4
	VOID	InitRegionFocus();
	BOOL	GetRegionFocus(ULONG ulRegionNo, LONG &lTLFocusLevel, LONG &lTRFocusLevel,
											 LONG &lBLFocusLevel, LONG &lBRFocusLevel);
	BOOL	SetRegionFocusTL(ULONG ulRegionNo, LONG lTLFocusLevel);
	BOOL	SetRegionFocusTR(ULONG ulRegionNo, LONG lTRFocusLevel);
	BOOL	SetRegionFocusBL(ULONG ulRegionNo, LONG lBLFocusLevel);
	BOOL	SetRegionFocusBR(ULONG ulRegionNo, LONG lBRFocusLevel);
	//	427TX	4

	// ===========================================================
	// Get Functions
	BOOL	GetRegion(CONST ULONG ulRegion,
						ULONG& ulULRow, ULONG& ulULCol, ULONG& ulLRRow, ULONG& ulLRCol);
	ULONG	GetRegionState(ULONG ulRegion);
	BOOL	GetRegionSub(ULONG ulRegion, ULONG &ulSubRow, ULONG &ulSubCol);
	ULONG	GetTotalSubRegions();


	BOOL	IsOutMS90SortingPart(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol,
								 const ULONG ulRegion);
	BOOL	FindNextAutoSortRegion(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol);
	ULONG	GetSortRegion(int nIndex);	// start from 0
	ULONG	GetHelpRegion(int nIndex);	// start from 0
	BOOL	IsCanSortingRegion(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol, const ULONG ulRegionNo);
	ULONG	GetTargetRegionIndex();
	ULONG	GetTargetRegion();
	ULONG	GetAssistRegion();
	VOID	SetPickOrder(int i, ULONG ulTarget, ULONG ulAssist);
	VOID	AssignRegionPickList(CONST UCHAR ucRgnOrder);
	VOID	AssignRegionOuterList(CONST BOOL bCtrBased=FALSE);
	VOID	ResetSkippedRegionList();
	VOID	FindFirstPickingRegion(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol);
	VOID	SetTargetRegionIndex(ULONG ulIndex);
	BOOL	SetTargetRegion(ULONG ulTgtRegion);
	BOOL	IsRightAllRegionSorted(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol);
	VOID	ResetRegionState(const ULONG ulStartRow, const ULONG ulEndRow, const ULONG ulStratCol, const ULONG ulEndCol);
	VOID	Rotate180RegionState();
	BOOL	CheckRegionCoupled(ULONG ulRegionNo);

	BOOL	IsLastRegionBonded();
	BOOL	IsAllRegionsBonded(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol);
	BOOL	HasNoRegionBonded();

	// ===========================================================
	// Set Functions
	VOID SetSortingRegionGrade(const UCHAR ucGrade);
	UCHAR GetSortingRegionGrade();

	VOID SetSortingRegion(const ULONG ulSortingRegion);
	ULONG GetSortingRegion();

	VOID SetPickingRegionGrade(const ULONG ucGrade);
	UCHAR GetPickingRegionGrade();
	VOID SetPickingRegion(const ULONG ulPickingRegion);
	ULONG GetPickingRegion();

	VOID SetRegionSize(const LONG ulSize);
	WM_CRegion *AddRegionList(const ULONG ulRegion, const ULONG ulULRow, const ULONG ulULCol, const ULONG ulLRRow, const ULONG ulLRCol);
	WM_CRegion *GetRegionList(const ULONG ulRegion);
	WM_CRegion *GetNextSortingRegion();
	VOID MoveToNextSortingRegion();
	WM_CRegion *GetNextRegion();

	BOOL SetRegion(ULONG ulRegion, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol);
	BOOL SetRegionState(ULONG ulRegion, ULONG nstate);
	BOOL SetHomeDieRegion(ULONG ulHomeRegion);

	// ===========================================================
	// Main Public Functions
	ULONG	GetWithinRegionNo(ULONG ulRow, ULONG ulCol);
	BOOL	IsWithinThisRegion(ULONG ulRegion, ULONG ulRow, ULONG ulCol);

	BOOL 	IsInvalidRegion(ULONG ulRegion);
	ULONG	GetHomeDieRegion();
	ULONG	GetInitState();
	VOID	SetInitState(CONST ULONG ulInitState);

	BOOL WriteRegionStateFile();
	VOID GetRegionStateName(const ULONG ulRegionIndex);
	VOID GetRegionStateNameList();

	CString GetRegionPickingSequenceList(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol,
										 const LONG lStartPickingRegion, const LONG lEndPickingRegion, BOOL &bFirstRegion);
protected:
	// ===========================================================
	// Constructor and destructor
	WT_CSubRegionsInfo();
	~WT_CSubRegionsInfo();

	static WT_CSubRegionsInfo* m_pInstance;

private:
	UCHAR		m_ucSortingGrade;
	ULONG		m_ulSortingRegion;

	UCHAR		m_ucPickingGrade;
	ULONG		m_ulPickingRegion;
	CObArray	m_RegionList;

	WT_SR_PROPERTY	m_pRegions[WT_MAX_SUBREGIONS_LIMIT];
	WT_SR_PICKORDER	m_pPickOrder[WT_MAX_SUBREGIONS_LIMIT];
	ULONG			m_ulPickIndex;

	ULONG		m_ulHomeDieRegion;
	ULONG		m_ulInitState;

	BOOL	m_bManualAlignRegion;

	// for prescan relative variables
	INT		m_nRegionWftULX;
	INT		m_nRegionWftULY;
	INT		m_nRegionWftLRX;
	INT		m_nRegionWftLRY;
	BOOL	m_bRegionAligned;

	BOOL	m_bIsRegionEnd;
	BOOL	m_bIsRegionEnding;

	BOOL	m_bRegionManualConfirm;

public:
	ULONG		m_ulSubRows;   //the number of sub regions in row
	ULONG		m_ulSubCols;  //the number of sub regions in column
	LONG		m_lRegionPickMinLimit;
	CString m_szCurrentRegionState_HMI;
	ULONG	m_ulCurrentSortingRegion_HMI;
	CString	m_szRegionOrderList_HMI;
	CString	m_szRegionStateList_HMI;

};

//	Chip Prober probing position offset PO
typedef struct
{
	LONG	m_lMoveX;
	LONG	m_lMoveY;
	LONG	m_lState;
	LONG	m_lScanX;
	LONG	m_lScanY;
	LONG	m_lPO_X;
	LONG	m_lPO_Y;
} WT_PO_PROPERTY;

typedef struct
{
	LONG	m_lDCState;
	LONG	m_lDCX;
	LONG	m_lDCY;
	DOUBLE	m_dDieClbX;
	DOUBLE	m_dDieClbXY;
	DOUBLE	m_dDieClbY;
	DOUBLE	m_dDieClbYX;
} WT_DC_PROPERTY;

#define	CP_MAP_GRID_MAX_ROW			101
#define	CP_MAP_GRID_MAX_COL			101
#define	WT_MAX_POPOINTS_LIMIT		(CP_MAP_GRID_MAX_ROW-1)*(CP_MAP_GRID_MAX_COL-1)
#define	WT_PO_OFFSET_INIT			-666666
#define	WT_PO_OFFSET_FAIL			-888888
#define	WT_PO_FIND_DIE_RANGE		1

class WT_CPO_PointsInfo : public CObject
{
public:
	static WT_CPO_PointsInfo* Instance();

	LONG BiLinearInterpolation(LONG lX1, LONG lY1, LONG lX2, LONG lY2, LONG lZX1Y1, LONG lZX2Y1, LONG lZX1Y2, LONG lZX2Y2, CONST LONG lX0, CONST LONG lY0);

	VOID	InitPOPoints(ULONG ulGridRows, ULONG ulGridCols);

	// ===========================================================
	// Get Functions
	BOOL	GetPODone();
	BOOL	GetPOState();
	LONG	GetPointPO(ULONG ulPointNo, LONG &lScanX, LONG &lScanY, LONG &lPO_X, LONG &lPO_Y);
	BOOL	GetPOPoint_Region(CONST LONG lScanX, CONST LONG lScanY, ULONG &ulPointUL, ULONG &ulPointUR, ULONG &ulPointLL, ULONG &ulPointLR);
	BOOL	GetNearestPointPO(LONG lScanX, LONG lScanY, LONG &lPointX, LONG &lPointY, LONG &lPO_X, LONG &lPO_Y, ULONG &ulPointNo);
	LONG	GetPointSP(ULONG ulPointNo, LONG &lMoveX, LONG &lMoveY);
	VOID	GetGridSpan(LONG &lSpanX, LONG &lSpanY);

	// ===========================================================
	// Set Functions
	VOID	SetPODone(BOOL bDone);
	BOOL	SetPOState(BOOL bState);
	BOOL	SetPointPO(ULONG ulPointNo, LONG lScanX, LONG lScanY, LONG lPO_X, LONG lPO_Y, LONG lState);
	BOOL	SetPointSP(ULONG ulPointNo, LONG lMoveX, LONG lMoveY);
	VOID	SetGridSpan(LONG lSpanX, LONG lSpanY);


	VOID	InitDCPoints();
	BOOL	GetDCState();
	BOOL	SetDCState(BOOL bState);
	VOID	SetDCDone(BOOL bDone);
	BOOL	GetDCDone();
	LONG	GetPointDC(ULONG ulPointNo, LONG &lScanX, LONG &lScanY, DOUBLE &dClbX, DOUBLE &dClbXY, DOUBLE &dClbY, DOUBLE &dClbYX);
	LONG	GetPointDC(ULONG ulPointNo, DOUBLE &dClbX, DOUBLE &dClbXY, DOUBLE &dClbY, DOUBLE &dClbYX);
	LONG	GetPointDC(ULONG ulPointNo, LONG &lScanX, LONG &lScanY);
	ULONG	GetNearestPointDC(LONG lScanX, LONG lScanY);
	BOOL	SetPointDC(ULONG ulPointNo, LONG lScanX, LONG lScanY, DOUBLE dClbX, DOUBLE dClbXY, DOUBLE dClbY, DOUBLE dClbYX, LONG lState);

protected:
	// ===========================================================
	// Constructor and destructor
	WT_CPO_PointsInfo();
	~WT_CPO_PointsInfo();

	static WT_CPO_PointsInfo* m_pInstance;

private:

	BOOL			m_bPODone;
	BOOL			m_bPOState;
	ULONG			m_ulGridRowNum;
	ULONG			m_ulGridColNum;
	LONG			m_lGridSpanX;
	LONG			m_lGridSpanY;
	WT_PO_PROPERTY	m_pPoPoints[WT_MAX_POPOINTS_LIMIT+1];

	BOOL			m_bDCDone;
	BOOL			m_bDCState;
	WT_DC_PROPERTY	m_pDcPoints[WT_MAX_POPOINTS_LIMIT+1];
};
//	Chip Prober probing position offset PO

#endif  // WT_SUB_REGION_H

//================================================================
// End of file WT_SUB_REGION_H.h
//================================================================