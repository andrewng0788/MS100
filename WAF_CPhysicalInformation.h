
#pragma once

#include <afxtempl.h>
// Physical location
class /*AFX_EXT_CLASS*/ WAF_CPhysicalInformation : public CObject
{
public:
	WAF_CPhysicalInformation();
	WAF_CPhysicalInformation(const long lX, const long lY, const LONG lDataListIndex, const BOOL bOverLapDie);
	WAF_CPhysicalInformation(const WAF_CPhysicalInformation& Src);
	virtual ~WAF_CPhysicalInformation();

	void SetX(const long lX);
	void SetY(const long lY);
	void SetDataListIndex(const LONG lDataListIndex);
	void SetOverLapDie(const BOOL bOverLapDie);

	void SetRow(const long lRow);
	void SetCol(const long lCol);

	long GetX() const;
	long GetY() const;
	LONG GetListIndex() const;
	BOOL GetOverLapDie() const;

	long GetRow() const;
	long GetCol() const;

	void SetProcessed(const BOOL bProcessed);
	BOOL GetProcessed() const;

	const WAF_CPhysicalInformation& operator=(const WAF_CPhysicalInformation& Src);
protected:
	long m_lX;
	long m_lY;
	long m_lRow;
	long m_lCol;
	LONG m_lDataListIndex;
	BOOL m_bOverLapDie;
	BOOL m_bProcessed;
};

// Physical location array
typedef CTypedPtrArray<CObArray, WAF_CPhysicalInformation*> WAF_CPhysicalInformationArray;

// Physical location map
// This class can be used to calculation the logical positions from the physical position of the dice
// Concept
// - CompleteSet is the set containing the dice with calculated logical positions
// - TempSet is the set containing the dice without the logical positions yet. 
// 1. We can get the set of nearest pitch from the CompleteSet. 
// 2. We search the dice in TempSet which falls in the nearest pitch set and add them to
//    the CompleteSet with proper logical positions assigned. 
//
// Example:
// 1. Start for the reference die (CompleteSet with 1 die). 
// 2. The 8 nearest pitches can be calculated. 
// 3. For each pitch, we search the dice which fall in them. 
// 4. Assign the logical position accordingly. 
// 5. Include those dice in the CompleteSet and continue. 
class /*AFX_EXT_CLASS*/ WAF_CPhysicalInformationMap : public CObject
{
public:
	WAF_CPhysicalInformationMap();
	virtual ~WAF_CPhysicalInformationMap();

	void RemoveAll();
	
	// Y will be sorted in decending order
	// SortAdd_Tail will add the coordinate starting from the end.
	// That is, it is better to add the big values first and then the small value
	void SortAdd_Tail(const long lX, const long lY, const LONG lIndex, const BOOL bOverLap);
	// SortAdd_Head will add the coordinate from the head
	// That is, it is better to add the big value later
	void SortAdd_Head(const long lX, const long lY, const LONG lIndex, const BOOL bOverLap);

	// This version of construct map uses a single reference die with given row and col to 
	// construct the whole logical map
	// (lRefPhyX, lRefPhyY, lRow, lCol) is the physical and logical position of the reference die
	// (DieSize tolerance) is the area to control whether the die is consider to be at that logical position. 
	//        Small tolerance gives more accurate logical map but may have more missing dice
	//        Bigger tolerance covers more dice but less accurate. 
	//        Adjust the size based on the actual die size. Use bigger tolerance for many die shifting. 
	// (Die Pitch) die pitch
	// nMaxIterateCount is the number of iterations to perform the search. -1 to do it until end. 
	//        Good wafer can find all dice in one iteration. 
	//        Broken wafer may take more iterations. 
	//        No guideline for this parameter. 
	// nMaxAllowLeft is the number of dice can be left with calculation. -1 to search every die. 
	// nMaxAllowPitch is how far the island is allowed to be in the calculation
	void ConstructMap(const long lRefPhyX, const long lRefPhyY, const long lRow, const long lCol, 
		const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
		const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, const BOOL bPreSorted, 
		const int nMaxIterateCount = -1, const int nMaxAllowLeft = -1, const int nMaxAllowPitch = -1);

	// This version of construct map uses a multiple reference dice with given rows and cols to 
	// construct the whole logical map. Typically it is used for broken wafer. 
	void ConstructMap(const CDWordArray& alRefPhyX, const CDWordArray& alRefPhyY, 
		const CDWordArray& alRow, const CDWordArray& alCol, 
		const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
		const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, 
		const BOOL bPreSorted, const int nMaxIterateCount = -1, const int nMaxAllowLeft = -1, const int nMaxAllowPitch = -1);

	// They are special algorithms that simply construct the logic map by pitch
	// For example, if the die is 3 pitch from the reference die, it will be 3 logic indices from the reference die. 
	void ConstructMapByPitch(const long lRefPhyX, const long lRefPhyY, const long lRow, const long lCol, 
		const unsigned long ulDiePitchX, const unsigned long ulDiePitchY);
	void ConstructMapByPitchEx(const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, 
		const unsigned long DupXSize = 1, const unsigned long ulDupYSize = 1, const BOOL bAppendDup = FALSE);

	// Return the logical index
	BOOL GetInfo(const unsigned long ulRow, const unsigned long ulCol, long& lX, long& lY, 
		LONG &lDataListIndex, BOOL &bOverLapDie);
		
	// Non-optimized code
	// If Y is not sorted, you can use the below function. Calculation time will be longer
	void Add(const long lX, const long lY, const LONG lIndex, const BOOL bOverLap);
	void Sort();

	VOID GetDimension(ULONG &ulRowMax, ULONG &ulColMax);

	// If the wafer is broken and you want to include all islands, set this to TRUE
	// KeepIsolatedDice(TRUE) will take longer to calculate
	void KeepIsolatedDice(const BOOL bKeepIsolated);

	// Save the calculation into a file for debugging
	void SetDebugOn(const BOOL bEnable);

	void SetScan2Pr(const BOOL bEnable);
	// Special case: for the wafer not cutting perpendicularly
	void SetXOffset(const long lXOffset);
protected:
	void PreSortFindDie(WAF_CPhysicalInformationArray& TempArray, WAF_CPhysicalInformationArray& CompleteArray, 
		WAF_CPhysicalInformationArray& Stack, BOOL** aabDone, const long lX, const long lY, 
		const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, const long lRow, const long lCol);

	void PreSortFindCluster(WAF_CPhysicalInformationArray& TempArray, WAF_CPhysicalInformationArray& CompleteArray, 
		BOOL** aabDone, const long lRows, const long lCols, const long lX, const long lY, const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
        const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, 
		const long lStartRow, const long lStartCol);

	void FindDie(WAF_CPhysicalInformationArray& TempArray, WAF_CPhysicalInformationArray& CompleteArray, 
		WAF_CPhysicalInformationArray& Stack, BOOL** aabDone, const long lX, const long lY, 
		const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, const long lRow, const long lCol);

	void FindCluster(WAF_CPhysicalInformationArray& TempArray, WAF_CPhysicalInformationArray& CompleteArray, 
		BOOL** aabDone, const long lRows, const long lCols, const long lX, const long lY, const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
        const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, 
		const long lStartRow, const long lStartCol);

	WAF_CPhysicalInformationArray m_PhysicalInformationArray;
	long** m_aalMapIndex;
	unsigned long m_ulRows;
	unsigned long m_ulCols;
	BOOL m_bKeepIsolatedDice;

	long m_lXOffset;

	BOOL m_bDebugOn;
	BOOL m_bScan2PrRecord;
};
