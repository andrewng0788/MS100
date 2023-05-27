#ifndef WT_1STDIE_FINDER_H
#define WT_1STDIE_FINDER_H

//================================================================
// #include and #define
//================================================================

//Path-Finder RETURN Code
#define ERR_1DF__DONE					1
#define ERR_1DF__OK						0
#define ERR_1DF__NODIE					-1
#define ERR_1DF__WAFEREND				-2


//================================================================
// struct, enum definition
//================================================================

enum ENUM_FINDER_REGION_SIZE
{
    REGION_SIZE_5x3	= 1,
    REGION_SIZE_3x5	= 2,
    REGION_SIZE_3x3 = 3,
	REGION_SIZE_4x4 = 4,
	REGION_SIZE_5x5 = 5,
	REGION_SIZE_7x7	= 7,
};

//================================================================
// Class definition
//================================================================

class CWaferTable;

class CWT_1stDieFinder : public CObject
{
public:
	// ===========================================================
	// Constructor and destructor
	CWT_1stDieFinder();
	~CWT_1stDieFinder();
	VOID SetWaferTable(CWaferTable* oWaferTable);

	// ===========================================================
	// Get Functions
	BOOL IsInit();
	BOOL GetLastPosn(ULONG& ulRow, ULONG& ulCol);

	// ===========================================================
	// Set Functions
	VOID SetTargetSubRegion(ULONG ulUlRow, ULONG ulUlCol, ULONG ulLrRow, ULONG ulLrCol);

	// ===========================================================
	// Main Public Functions
	BOOL StartFinder(ULONG ulStartRow, ULONG ulStartCol, DOUBLE dFovX, DOUBLE dFovY);
	INT	 FindNextDie();
	VOID StopFinder();


protected:

	// ===========================================================
	// Protected Functions
	BOOL RestoreFinderAlgorithm();
	INT  WT_LFNextDie(BOOL bLatch);

	ULONG GetCurRow();
	ULONG GetCurCol();

	VOID InitFinderData();
	// ===========================================================
	// Protected Member Variables
	CWaferTable* m_pWaferTableStn;


private:

	BOOL					m_bInit;			// Is PathFinder being initialized?

	//MAP Backup parameters
	CString m_szBpAlgorithm;
	CString m_szBpPathFinder;
	ULONG   m_ulBpDieStep;
	WAF_CDieSelectionAlgorithm::WAF_EPickMode m_eBpPickMode;

	//RUntime variables
	ULONG	m_ulCurrRow;			// Runtime current row
	ULONG	m_ulCurrCol;			// Runtime current col 

	ULONG	m_ulEndAreaULRow;		// Target destination pt or region's UL corner row
	ULONG	m_ulEndAreaULCol;		// Target destination pt or region's UL corner col
	ULONG	m_ulEndAreaLRRow;		// Target destination pt or region's LR corner row
	ULONG	m_ulEndAreaLRCol;		// Target destination pt or region's LR corner col

	// sub region corner value
	ULONG	m_ulTgtRegionUlRow;		// target region boundary
	ULONG	m_ulTgtRegionUlCol;
	ULONG	m_ulTgtRegionLrRow;
	ULONG	m_ulTgtRegionLrCol;
};


#endif  // WT_1STDIE_FINDER_H

//================================================================
// End of file WT_1STDIE_FINDER_H.h
//================================================================
