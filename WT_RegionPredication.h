#ifndef WT_REGION_PREDICATION_H
#define WT_REGION_PREDICATION_H

//================================================================
// struct, enum definition
//================================================================

#define	WT_REGION_PRED_STATE_UNUSED		0
#define	WT_REGION_PRED_STATE_INUSE		1

#define	WT_REGION_PRED_MAX_ROW			50
#define	WT_REGION_PRED_MAX_COL			50

#define	WT_REGION_PRED_MAX_LIMIT		(WT_REGION_PRED_MAX_ROW*WT_REGION_PRED_MAX_COL)
#define	ADV_RGN_OFFSET_MIN_DIST			5
#define	ADV_RGN_GET_OFFSET_WIDTH		1
#define	ADV_RGN_GET_OFFSET_HEIGHT		1

#define	ADV_REGN_LAST_POINTS		9

typedef struct
{
	ULONG	m_ulPdcState;
	ULONG	m_ulULPdcRow;
	ULONG	m_ulULPdcCol;
	ULONG	m_ulLRPdcRow;
	ULONG	m_ulLRPdcCol;
	BOOL	m_bInnerRegion;
	LONG	m_lRegnUpdIndex;
	LONG	m_laRegnOffsetX[ADV_REGN_LAST_POINTS];
	LONG	m_laRegnOffsetY[ADV_REGN_LAST_POINTS];
	LONG	m_lRegnOffsetX;
	LONG	m_lRegnOffsetY;
	CTime	m_stPickedTime;
} WT_RP_PROPERTY;

typedef	struct
{
	LONG	m_lLastMapRow;
	LONG	m_lLastMapCol;
	ULONG	m_ulLastSubRow;
	ULONG	m_ulLastSubCol;
	LONG	m_lLastOffsetX;
	LONG	m_lLastOffsetY;
} DIE_PICK_INFO;

typedef struct
{
	long	m_lOffsetX;
	long	m_lOffsetY;
	short	m_sSetState;
}	RegionDieOffset;

//================================================================
// Class definition
//================================================================

class WT_CRegionPredication : public CObject
{
public:
	static WT_CRegionPredication* Instance();

	VOID	InitPdcRegions(CONST INT nNumOfRows, CONST INT nNumOfCols);
	BOOL	InitRegionOffset();

	// ===========================================================
	// Get Functions
	BOOL	GetPdcRegionArea(ULONG ulSubRow, ULONG ulSubCol, ULONG& ulULRow, ULONG& ulULCol, ULONG& ulLRRow, ULONG& ulLRCol);
	ULONG	GetRegionPdcState(ULONG ulSubRow, ULONG ulSubCol);
	BOOL	GetLastDieData(ULONG &ulSubRow, ULONG &ulSubCol, LONG &lRow, LONG &lCol, LONG &lOffsetX, LONG &lOffsetY);
	BOOL	SetRegionOffset(ULONG ulSubRow, ULONG ulSubCol, LONG lOffsetX,  LONG lOffsetY, BOOL bToAll = FALSE);
	ULONG	GetNumOfPdcRegionRows();
	ULONG	GetNumOfPdcRegionCols();
	VOID	SetRegionInnerState(ULONG ulSubRow, ULONG ulSubCol, BOOL bInner);
	BOOL	GetRegionInnerState(ULONG ulSubRow, ULONG ulSubCol);

	// ===========================================================
	// Set Functions
	BOOL	SetPdcRegionArea(ULONG ulSubRow, ULONG ulSubCol, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol);
	BOOL	SetRegionPdcState(ULONG ulSubRow, ULONG ulSubCol, ULONG nstate);
	BOOL	SetLastDieData(LONG lRow, LONG lCol, LONG lOffsetX, LONG lOffsetY);
	BOOL	SetVerifyOffset(LONG lOffsetX, LONG lOffsetY);
	BOOL	GetVerifyOffset(LONG &lOffsetX, LONG &lOffsetY);

	BOOL	SetDieOffset(LONG lRow, LONG lCol, LONG lOffsetX, LONG lOffsetY, BOOL bToAll);
	BOOL	GetRegionOffset(ULONG ulSubRow, ULONG ulSubCol, LONG &lOffsetX, LONG &lOffsetY, CTime &stGetTime);

	// ===========================================================
	// Main Public Functions
	BOOL	GetInPdcRegionSub(LONG lRow, LONG lCol, ULONG &ulSubRow, ULONG &ulSubCol);
	BOOL	IsWithinThisPdcRegion(ULONG ulSubRow, ULONG ulSubCol, ULONG ulRow, ULONG ulCol);

	BOOL	IsValidPdcRegion(ULONG ulSubRow, ULONG ulSubCol);

protected:
	// ===========================================================
	// Constructor and destructor
	WT_CRegionPredication();
	~WT_CRegionPredication();

	static WT_CRegionPredication* m_pInstance;

private:

	BOOL	m_bVerifyOffset;
	LONG	m_lVerifyOffsetX;
	LONG	m_lVerifyOffsetY;
	DIE_PICK_INFO		m_stLastDieInfo;
	WT_RP_PROPERTY		m_pRegionPreds[WT_REGION_PRED_MAX_ROW][WT_REGION_PRED_MAX_COL];
//	RegionDieOffset		m_pDieOffsets[MAPMAXROW2][MAPMAXCOL2];

	ULONG		m_ulNumOfPdcRows;
	ULONG		m_ulNumOfPdcCols;
};

#endif  // WT_SUB_REGION_H

//================================================================
// End of file WT_SUB_REGION_H.h
//================================================================