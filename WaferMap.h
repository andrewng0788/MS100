#ifndef WM_WaferMap_H
#define WM_WaferMap_H

#include <WaferMapWrapper.h>
#include "StringMapFile.h"

#define	WM_SAMPING_RESCAN_IGNORE_GRADE_LIST	"Sampling Rescan Ignore Grade List"
//Sort die mode
#define WT_SORT_NORMAL				0
#define WT_SORT_MIN_DIE				1
#define WT_SORT_MAX_DIE				2
#define WT_SORT_USER_DEFINE			3

//================================================================
// Class definition
//================================================================

class WM_CWaferMap : public CObject
{
public:
	WM_CWaferMap(); //Constructor
	~WM_CWaferMap(); //Deconstructor
	static WM_CWaferMap *Instance();

	VOID SaveData(CMapElement *pElement);
	VOID LoadData(CMapElement *pElement);

	VOID SetUserRegionPickingMode(const BOOL bUserRegionPickingMode);
	BOOL IsUserRegionPickingMode();
	void SetWaferMapWrapper(CWaferMapWrapper *pWaferMapWrapper);
	BOOL GetWaferMapDimension(ULONG &ulMaxRow, ULONG &ulMaxCol);

	BOOL IsMapNullBin(const ULONG ulRow, const ULONG ulCol);
	BOOL IsMapHaveBin(const ULONG ulRow, const ULONG ulCol);
	BOOL IsMapValidDie(const ULONG ulRow, const ULONG ulCol);
	BOOL IsHasDie(ULONG ulStartRow, ULONG ulEndRow, ULONG ulStartColumn, ULONG ulEndColumn);
	void BatchClearDieStatus(ULONG ulStartRow, ULONG ulEndRow, ULONG ulStartColumn, ULONG ulEndColumn);
	void BatchSetMissingDie(ULONG ulStartRow, ULONG ulEndRow, ULONG ulStartColumn, ULONG ulEndColumn);

	CString SetRegionPickingSequence(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol, 
									 const LONG lStartPickingRegion, const BOOL bSetFirstRegion);
	BOOL IsRegionPickingDieExisted(const ULONG ulRegion, const BOOL bUseEmptyCheck, const LONG lMnNoDieGrade);
	void SetupRegionSize(const ULONG ulMaxRow, const ULONG ulMaxCol, const LONG lSubHeight, const LONG lSubWidth, const BOOL bSecondHalfSortMode,
						 const BOOL bPrescanRegionMode, const BOOL bRegionSortOuter, const BOOL bUseEmptyCheck, const LONG lMnNoDieGrade);
	VOID SetRegionAlgorithmParameter(const LONG lSubRowsInRegion, const LONG lSubColsInRegion, const BOOL bPickOneGradeRegionByRegion);
	void DrawAllRegionBoundary();
	void DeleteAllBoundary();
	void AddBoundary(const unsigned long ulStartRow, const unsigned long ulEndRow, 
					 const unsigned long ulStartColumn, const unsigned long ulEndColumn);
	void SuspendSortingPathAlgorithmPreparation();
	void SuspendAlgorithmPreparation();
	BOOL IsAlgorithmSuspended();
	VOID RotateInternal(unsigned long ulWmRow, unsigned long ulWmCol, short sAngle, unsigned long &ulWmNewRow, unsigned long &ulWmNewCol);


	LONG IsInGradeList(const UCHAR ucGrade, CUIntArray &aulSelectedGradeList);
	VOID GetRegionStatistics();
	VOID DecreaseRegionStatistics(const UCHAR ucGrade);
	ULONG GetRegionStatistics(const UCHAR ucGrade);
	VOID CalculateRegionStatistics(const ULONG ulStartRow, const ULONG ulStartCol, const ULONG ulEndRow, const ULONG ulEndCol);

	VOID SelectRegionGradeList(const ULONG ulStartRow, const ULONG ulStartCol, const ULONG ulEndRow, const ULONG ulEndCol, 
							   const CString szCustomerName, const BOOL bKeepLastUnloadGrade, const UCHAR ucLastPickDieGrade, 
							   const LONG lSpecialRefDieGrade, const BOOL bIgnoreRegion,
							   const LONG lSortingMode, const ULONG ulMinGradeCount, const ULONG ulMinGradeBinNo,  BOOL &bSortMultiToOne, 
							   CUIntArray &unArrMinGradeBin);

	ULONG UpdateSortingMode(const BOOL bIsRegionPicking, const CString szCustomerName, const BOOL bKeepLastUnloadGrade, const UCHAR ucLastPickDieGrade, 
							const LONG lSpecialRefDieGrade, const BOOL bIgnoreRegion,
							const LONG lSortingMode, const ULONG ulMinGradeCount, const ULONG ulMinGradeBinNo,  BOOL &bSortMultiToOne, 
							CUIntArray &unArrMinGradeBin);

	VOID RemoveInkDieGrade(const CString szCustomerName);
	BOOL UpdateLastUnloadSortingMode(const UCHAR ucLastPickDieGrade);
	BOOL UpdateSpecialRefDieGradeList(const LONG lSpecialRefDieGrade);
	VOID UpdateCustomerNameGradeList(const CString szCustomerName);


	VOID SelectSortGradeList(const LONG lSortingMode, const ULONG ulMinGradeCount, const ULONG ulMinGradeBinNo, 
							 BOOL &bSortMultiToOne, CUIntArray &aulSelectedGradeList, ULONG *pulTotal, ULONG *pulLeft, UCHAR *pGrade,
							 CUIntArray &unArrMinGradeBin);

	LONG GetSelectedUserGradeList(CUIntArray &aulSelectedGradeList, ULONG *pulTotal, UCHAR *pGrade);
	VOID GetSelectedSortGradeList(const LONG lSortingMode, const LONG nStartIndex, const LONG nListSize, ULONG *pulTotal, ULONG *pulLeft, UCHAR *pGrade);
	BOOL GetUserGradeList(CUIntArray& aulGradeList);
	BOOL SaveUserGradeList(const unsigned char* aucGrade, const unsigned long ulNumberOfGrades);
	BOOL GetGroupGradeList(CUIntArray& aulGroupGradeList);

	//Statistics
	BOOL GetNextSortingRegion(const UCHAR ucSortingGrade);
	UCHAR GetSortingNextGrade();
	BOOL GetNextRegionStatistics(const UCHAR ucGrade, ULONG &ulLeft, ULONG &ulTotal);
	VOID SetStartRegionPosition();
	VOID PrepareNextRegionSortingPath(const UCHAR ucGrade, const ULONG ulPickingRegion);
	UCHAR GetSortingGrade(const BOOL bFirst = TRUE, const UCHAR bPickingGrade = 0);
	BOOL PrepareNextSortingRegion();
	BOOL PrepareFirstPickingRegion();
	BOOL PrepareNextPickingRegion();


	VOID CreateConstructMapNGGradeFile(const CString szLogPath, UCHAR ucNgGradeList[256]);

	BOOL GetSamplingRescanIgnoreGradeList(CUIntArray &aulSelectedGradeList);
private:
	BOOL m_bUserRegionPickingMode;
public:
	CWaferMapWrapper *m_pWaferMapWrapper;
	CUIntArray m_aulRegionGradeList;
	CArray< long, long > m_alRegionTotalList;

	CString	m_szSamplingRescanIgnoreGradeList;

	static WM_CWaferMap *m_pInstance;
};

#endif  // WaferMap_H

//================================================================
// End of file WaferMap.h
//================================================================