#ifndef PB_MULTI_PROBE_LEVEL_H
#define PB_MULTI_PROBE_LEVEL_H

#define	PB_MPL_GRID_ROWS			15
#define	PB_MPL_GRID_COLS			15
#define	PB_MPL_OFFSET_INIT			-666666

//	Chip Prober probing position-level mapping class
typedef struct
{
	LONG	m_lPLState;
	LONG	m_lPLPointX;
	LONG	m_lPLPointY;
	LONG	m_lPL_Z;
}	PB_PL_PROPERTY;

class PB_CMultiProbeLevelInfo : public CObject
{
public:
//	static PB_CMultiProbeLevelInfo* Instance();

	VOID	InitPLPoints();

	// ===========================================================
	// Get Functions
	BOOL	GetPLState();
	LONG	GetPointPL(ULONG ulRow, ULONG ulCol, LONG &lScanX, LONG &lScanY, LONG &lPL_Z);
	BOOL	GetPLPointULCorner(CONST LONG lScanX, CONST LONG lScanY, ULONG &ulULRow, ULONG &ulULCol);
	BOOL	GetPLPointLRCorner(CONST LONG lScanX, CONST LONG lScanY, ULONG &ulLRRow, ULONG &ulLRCol);
	BOOL	GetNearestValidPL(LONG lScanX, LONG lScanY, ULONG &ulOutRow, ULONG &ulOutCol);

	// ===========================================================
	// Set Functions
	BOOL	SetPLState(BOOL bState);
	BOOL	SetPointPL(ULONG ulRow, ULONG ulCol, LONG lScanX, LONG lScanY, LONG lPL_Z, LONG lState);
	BOOL	ReorganizeSampleData(LONG lPLSpanX, LONG lPLSpanY);

	// ===========================================================
	// Constructor and destructor
	PB_CMultiProbeLevelInfo();
	~PB_CMultiProbeLevelInfo();

protected:
//	static PB_CMultiProbeLevelInfo* m_pInstance;
//	// ===========================================================
//	// Constructor and destructor
//	PB_CMultiProbeLevelInfo();
//	~PB_CMultiProbeLevelInfo();


private:
	BOOL			m_bPLState;
	PB_PL_PROPERTY	m_pPLPoints[PB_MPL_GRID_ROWS][PB_MPL_GRID_COLS];
};
//	Chip Prober probing position-level mapping class

#endif  // PB_MULTI_PROBE_LEVEL_H

//================================================================
// End of file PB_MULTI_PROBE_LEVEL_H.h
//================================================================