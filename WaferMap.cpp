//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "WT_Constant.h"
#include "WaferMap.h"
#include "WT_SubRegion.h"
#include "io.h"
#include "MS896A_Constant.h"
#include "PrescanConstant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//================================================================
// Constructor / destructor implementation section
//================================================================

WM_CWaferMap* WM_CWaferMap::m_pInstance = NULL;

WM_CWaferMap* WM_CWaferMap::Instance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new WM_CWaferMap();
	}

	return m_pInstance;
}

WM_CWaferMap::WM_CWaferMap()
{
	m_bUserRegionPickingMode = FALSE;
	m_pWaferMapWrapper = NULL;
}

WM_CWaferMap::~WM_CWaferMap()
{
}

VOID WM_CWaferMap::SaveData(CMapElement *pElement)
{
	if (pElement)
	{
		(*pElement)[WM_SAMPING_RESCAN_IGNORE_GRADE_LIST]	= m_szSamplingRescanIgnoreGradeList;
	}
}


VOID WM_CWaferMap::LoadData(CMapElement *pElement)
{
	if (pElement)
	{
		m_szSamplingRescanIgnoreGradeList = (*pElement)[WM_SAMPING_RESCAN_IGNORE_GRADE_LIST];
	}
}


VOID WM_CWaferMap::SetUserRegionPickingMode(const BOOL bUserRegionPickingMode)
{
	m_bUserRegionPickingMode = bUserRegionPickingMode;
}


BOOL WM_CWaferMap::IsUserRegionPickingMode()
{
	return m_bUserRegionPickingMode;
}


void WM_CWaferMap::SetWaferMapWrapper(CWaferMapWrapper *pWaferMapWrapper)
{
	m_pWaferMapWrapper = pWaferMapWrapper;
}


BOOL WM_CWaferMap::GetWaferMapDimension(ULONG &ulMaxRow, ULONG &ulMaxCol)
{
	ulMaxRow = 0;
	ulMaxCol = 0;

	if (!m_pWaferMapWrapper->GetMapDimension(ulMaxRow, ulMaxCol))
	{
		return FALSE;
	}

	if ((ulMaxRow > MAP_MAX_ROW) || (ulMaxCol > MAP_MAX_COL))
	{
		//SetErrorMessage("No of Row/Col are out of range!");
		return FALSE;
	}

	return TRUE;
}


BOOL WM_CWaferMap::IsMapNullBin(const ULONG ulRow, const ULONG ulCol)
{
	return m_pWaferMapWrapper->GetGrade(ulRow, ulCol) == m_pWaferMapWrapper->GetNullBin();
}


BOOL WM_CWaferMap::IsMapHaveBin(const ULONG ulRow, const ULONG ulCol)
{
	return m_pWaferMapWrapper->GetGrade(ulRow, ulCol) != m_pWaferMapWrapper->GetNullBin();
}


BOOL WM_CWaferMap::IsMapValidDie(const ULONG ulRow, const ULONG ulCol)
{
	BOOL bMissingDie = m_pWaferMapWrapper->GetReader()->IsMissingDie(ulRow, ulCol);
	if (!bMissingDie && IsMapHaveBin(ulRow, ulCol))
	{
		return TRUE;
	}

	return FALSE;
}


BOOL WM_CWaferMap::IsHasDie(ULONG ulStartRow, ULONG ulEndRow, ULONG ulStartColumn, ULONG ulEndColumn)
{
	for(ULONG ulRow = ulStartRow; ulRow < ulEndRow; ulRow++)
	{
		for(ULONG ulCol = ulStartColumn; ulCol < ulEndColumn; ulCol++)
		{ 
			if (IsMapValidDie(ulRow, ulCol))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


void WM_CWaferMap::BatchClearDieStatus(ULONG ulStartRow, ULONG ulEndRow, ULONG ulStartColumn, ULONG ulEndColumn)
{
	m_pWaferMapWrapper->StartBatchStateUpdate();
	for(ULONG ulRow = ulStartRow; ulRow < ulEndRow; ulRow++)
	{
		for(ULONG ulCol = ulStartColumn; ulCol < ulEndColumn; ulCol++)
		{
			m_pWaferMapWrapper->BatchClearDieStatus(ulRow, ulCol);
		}
	}
	m_pWaferMapWrapper->BatchClearDieStatusUpdate();
}


void WM_CWaferMap::BatchSetMissingDie(ULONG ulStartRow, ULONG ulEndRow, ULONG ulStartColumn, ULONG ulEndColumn)
{
	m_pWaferMapWrapper->StartBatchStateUpdate();
	
	for(ULONG ulRow = ulStartRow; ulRow < ulEndRow; ulRow++)
	{
		for(ULONG ulCol = ulStartColumn; ulCol < ulEndColumn; ulCol++)
		{ 
			if (IsMapHaveBin(ulRow, ulCol))
			{
				m_pWaferMapWrapper->BatchSetMissingDie(ulRow, ulCol);
//				m_pWaferMapWrapper->HideDie(ulRow, ulCol);
			}
		}
	}
	m_pWaferMapWrapper->BatchSetMissingDieUpdate();
}



VOID WM_CWaferMap::SetRegionAlgorithmParameter(const LONG lSubRowsInRegion, const LONG lSubColsInRegion, const BOOL bPickOneGradeRegionByRegion)
{
	m_pWaferMapWrapper->SetAlgorithmParameter("Sub Rows", lSubRowsInRegion);
	m_pWaferMapWrapper->SetAlgorithmParameter("Sub Cols", lSubColsInRegion);
	
	// bPickOneGradeRegionByRegion = 1, it will picked a grade one by one region
	// bPickOneGradeRegionByRegion = 0, it will picked all grades in the current region and move to the next region
	m_pWaferMapWrapper->SetAlgorithmParameter("Region By Region", bPickOneGradeRegionByRegion);
}


CString WM_CWaferMap::SetRegionPickingSequence(const BOOL bHalfSortMode, const BOOL bRowModeSeparateHalfMap, const ULONG ulHalfMaxRow, const ULONG ulHalfMaxCol, 
											   const LONG lStartPickingRegion, const BOOL bSetFirstRegion)
{
	CString szPickingSequence;
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	ULONG ulTotalRegionNum = pSRInfo->GetTotalSubRegions();

	BOOL bFirstRegion = bSetFirstRegion;
	szPickingSequence = pSRInfo->GetRegionPickingSequenceList(bHalfSortMode, bRowModeSeparateHalfMap, ulHalfMaxRow, ulHalfMaxCol, lStartPickingRegion, ulTotalRegionNum - 1, bFirstRegion);

	if (lStartPickingRegion > 0)
	{
		szPickingSequence += pSRInfo->GetRegionPickingSequenceList(bHalfSortMode, bRowModeSeparateHalfMap, ulHalfMaxRow, ulHalfMaxCol, 0, lStartPickingRegion - 1, bFirstRegion);
	}
	
	if (m_pWaferMapWrapper->GetReader() != NULL)
	{
		m_pWaferMapWrapper->GetReader()->SetCustomText(szPickingSequence);
	}

	return szPickingSequence;
}


BOOL WM_CWaferMap::IsRegionPickingDieExisted(const ULONG ulRegion, const BOOL bUseEmptyCheck, const LONG lMnNoDieGrade)
{
	ULONG ulULRow = 0, ulULCol = 0, ulLRRow = 0, ulLRCol = 0;
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	pSRInfo->GetRegion(ulRegion, ulULRow, ulULCol, ulLRRow, ulLRCol);

	CUIntArray aulSelectedGradeList;
	m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);
	//szMsg.Format("Total num of grades %d", aulSelectedGradeList.GetSize());
	//RegionPickLog(szMsg);

	UCHAR ucOffset  = m_pWaferMapWrapper->GetGradeOffset();
	UCHAR ucNullBin = m_pWaferMapWrapper->GetNullBin() - ucOffset;
	int mmmm = 0;
				
	for (ULONG ulRow = ulULRow; ulRow < ulLRRow; ulRow++)
	{
		for (ULONG ulCol = ulULCol; ulCol < ulLRCol; ulCol++)
		{
			UCHAR ucGrade = m_pWaferMapWrapper->GetGrade(ulRow, ulCol) - ucOffset;
			if (m_pWaferMapWrapper->IsReferenceDie(ulRow, ulCol))
			{
				continue;
			}

			if (bUseEmptyCheck && lMnNoDieGrade > 0 && ucGrade == lMnNoDieGrade)
			{
				continue;
			}

			if (ucGrade == ucNullBin)
			{
				continue;
			}

			if (IsInGradeList(m_pWaferMapWrapper->GetGrade(ulRow, ulCol), aulSelectedGradeList) >= 0)
			{
				mmmm++;
			}
					
			if (mmmm > pSRInfo->m_lRegionPickMinLimit)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
	


void WM_CWaferMap::SetupRegionSize(const ULONG ulMaxRow, const ULONG ulMaxCol, const LONG lSubHeight, const LONG lSubWidth, const BOOL bSecondHalfSortMode,
									const BOOL bPrescanRegionMode, const BOOL bRegionSortOuter, const BOOL bUseEmptyCheck, const LONG lMnNoDieGrade)
{
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	pSRInfo->InitRegion();
	pSRInfo->SetRegionSize(pSRInfo->GetSubRows() * pSRInfo->GetSubCols() + 1);
	ULONG ulRegionCount = 1;
	
	for (LONG j = 0; j < (LONG)pSRInfo->GetSubRows(); j++)
	{
		for (LONG i = 0; i < (LONG)pSRInfo->GetSubCols(); i++)
		{
			ULONG ulULRow = j * lSubHeight;
			ULONG ulULCol = i * lSubWidth;
			ULONG ulLRRow = (j + 1) * lSubHeight;
			ULONG ulLRCol = (i + 1) * lSubWidth;
			
			if (ulLRRow >= ulMaxRow)
			{
				ulLRRow = ulMaxRow;
			}

			if (ulLRCol >= ulMaxCol)
			{
				ulLRCol = ulMaxCol;
			}

			if (pSRInfo->SetRegion(ulRegionCount, ulULRow, ulULCol, ulLRRow, ulLRCol))
			{
				WM_CRegion *pRegion = pSRInfo->AddRegionList(ulRegionCount - 1, ulULRow, ulULCol, ulLRRow, ulLRCol);
				if ((pRegion != NULL) && IsUserRegionPickingMode())
				{
					pRegion->SetRegionNo(ulRegionCount - 1, j, i);
					CalculateRegionStatistics(ulULRow, ulULCol, ulLRRow, ulLRCol);
					pRegion->SetStatistics(&m_aulRegionGradeList, &m_alRegionTotalList);
				}
			}

			ULONG ulRegionState = WT_SUBREGION_STATE_INIT;

			//check there has the picking die existed or not 
			if (bPrescanRegionMode || bRegionSortOuter)
			{
				BOOL bFindDie = IsRegionPickingDieExisted(ulRegionCount, bUseEmptyCheck, lMnNoDieGrade);

				if (bFindDie || bSecondHalfSortMode)
				{
					ulRegionState = WT_SUBREGION_STATE_INIT;
				}
				else
				{
					ulRegionState = WT_SUBREGION_STATE_BONDED;
				}
			}
			pSRInfo->SetRegionState(ulRegionCount, ulRegionState);

			ulRegionCount++;
		}
	}
}


void WM_CWaferMap::DrawAllRegionBoundary()
{
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	DeleteAllBoundary();
	for (ULONG i = 1; i <= pSRInfo->GetTotalSubRegions(); i++)
	{
		ULONG ulStartRow, ulStartColumn, ulEndRow, ulEndColumn;
		if (pSRInfo->GetRegion(i, ulStartRow, ulStartColumn, ulEndRow, ulEndColumn))
		{
			AddBoundary(ulStartRow, ulEndRow, ulStartColumn, ulEndColumn);
		}
	}
}

void WM_CWaferMap::DeleteAllBoundary()
{
	m_pWaferMapWrapper->DeleteAllBoundary();
}

void WM_CWaferMap::AddBoundary(const unsigned long ulStartRow, const unsigned long ulEndRow, 
							   const unsigned long ulStartColumn, const unsigned long ulEndColumn)
{
	m_pWaferMapWrapper->AddBoundary(ulStartRow, ulEndRow, ulStartColumn, ulEndColumn);
}


void WM_CWaferMap::SuspendSortingPathAlgorithmPreparation()
{
	CString szAlgorithm, szPathFinder;
	m_pWaferMapWrapper->GetAlgorithm(szAlgorithm, szPathFinder);
	if( szAlgorithm.Find("Sorting Path 1")!=-1 )
	{
		m_pWaferMapWrapper->SuspendAlgorithmPreparation();
	}
}


void WM_CWaferMap::SuspendAlgorithmPreparation()
{
	m_pWaferMapWrapper->SuspendAlgorithmPreparation();
}


BOOL WM_CWaferMap::IsAlgorithmSuspended()
{
//	return m_pWaferMapWrapper->IsAlgorithmSuspended();
	return FALSE;
}


VOID WM_CWaferMap::RotateInternal(unsigned long ulWmRow, unsigned long ulWmCol, short sAngle, unsigned long &ulWmNewRow, unsigned long &ulWmNewCol)
{
	m_pWaferMapWrapper->RotateInternal(ulWmRow, ulWmCol, sAngle, ulWmNewRow, ulWmNewCol);
}


//========================================================================================================================
//		User Control Picking Wafer Map
//========================================================================================================================
//=====================================================================================================================
// Function Name: 		GetSortingGrade
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the first picking grade in the current wafer
// Return:				None
// Remarks:				Called once time after load a new map or change grade
//======================================================================================================================
UCHAR WM_CWaferMap::GetSortingGrade(const BOOL bFirst, const UCHAR bPickingGrade)
{
	CUIntArray aulSelectedGradeList;

	m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);

	ULONG ulGradeListSize = (LONG)aulSelectedGradeList.GetSize();
	ULONG ulLeft = 0;
	ULONG ulPick = 0;
	ULONG ulTotal = 0;
	UCHAR ucGrade = 0;
	ULONG i = 0;

	for (i = 0; i < ulGradeListSize; i++)
	{
		m_pWaferMapWrapper->GetStatistics((UCHAR)aulSelectedGradeList[i], ulLeft, ulPick, ulTotal);

		if (bFirst)
		{
			if (ulLeft > 0)
			{
				return (UCHAR)aulSelectedGradeList[i];
			}
		}
		else if (bPickingGrade == aulSelectedGradeList[i])
		{
			break;
		}
	}

	i++;
	if (i < ulGradeListSize)
	{
		m_pWaferMapWrapper->GetStatistics((UCHAR)aulSelectedGradeList[i], ulLeft, ulPick, ulTotal);
		if (ulLeft > 0)
		{
			return (UCHAR)aulSelectedGradeList[i];
		}
	}

	return 0;
}


//=====================================================================================================================
// Function Name: 		GetNextSortingRegion
// Input arguments:		ucGrade
// Output arguments:	None
// Description:   		Get the picking next region
// Return:				None
// Remarks:				Called once time after wafer end(current region end)
//======================================================================================================================
BOOL WM_CWaferMap::GetNextSortingRegion(const UCHAR ucSortingGrade)
{
	if (IsUserRegionPickingMode())
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		while (TRUE)
		{
			WM_CRegion *pRegion = pSRInfo->GetNextSortingRegion();
			if (pRegion != NULL)
			{
				ULONG ulULRow = pRegion->GetRegionTopRow();
				ULONG ulULCol = pRegion->GetRegionLeftCol();
				ULONG ulLRRow = pRegion->GetRegionBottomRow();
				ULONG ulLRCol = pRegion->GetRegionRightCol();

				m_pWaferMapWrapper->CalculateRegionStatistics(ulULRow, ulULCol, ulLRRow, ulLRCol);

				ULONG ulLeft = 0;
				ULONG ulTotal = 0;
				m_pWaferMapWrapper->GetRegionStatistics(ucSortingGrade, ulLeft, ulTotal);

				if (ulLeft > 0)
				{
					return TRUE;
				}
				pSRInfo->MoveToNextSortingRegion();
			}
			else
			{
				break;
			}
		}

		return FALSE;
	}

	return FALSE;
}

//=====================================================================================================================
// Function Name: 		GetSortingNextGrade
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the sorting next grade
// Return:				None
// Remarks:				Called once time after wafer end(current region end)
//======================================================================================================================
UCHAR WM_CWaferMap::GetSortingNextGrade()
{
	if (IsUserRegionPickingMode())
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		UCHAR ucSortingGrade = GetSortingGrade(FALSE, pSRInfo->GetPickingRegionGrade());
		return ucSortingGrade;
	}

	return 0;
}


//=====================================================================================================================
// Function Name: 		GetNextRegionStatistics
// Input arguments:		ucGrade
// Output arguments:	ulLeft, ulTotal
// Description:   		Get the picking next region
// Return:				None
// Remarks:				Called once time after wafer end(current region end)
//======================================================================================================================
BOOL WM_CWaferMap::GetNextRegionStatistics(const UCHAR ucGrade, ULONG &ulLeft, ULONG &ulTotal)
{
	if (IsUserRegionPickingMode())
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		WM_CRegion *pRegion = pSRInfo->GetNextRegion();
		if (pRegion != NULL)
		{
			ULONG ulULRow = pRegion->GetRegionTopRow();
			ULONG ulULCol = pRegion->GetRegionLeftCol();
			ULONG ulLRRow = pRegion->GetRegionBottomRow();
			ULONG ulLRCol = pRegion->GetRegionRightCol();

			m_pWaferMapWrapper->CalculateRegionStatistics(ulULRow, ulULCol, ulLRRow, ulLRCol);
			m_pWaferMapWrapper->GetRegionStatistics(ucGrade, ulLeft, ulTotal);

			return TRUE;
		}
	}

	return FALSE;
}


//=====================================================================================================================
// Function Name: 		SetStartRegionPosition
// Input arguments:		None
// Output arguments:	None
// Description:   		Set the start position of the current region
//						with Use “Sorting Path 1 Offline?for the algorithm
//						and generate the first offline path
// Return:				None
// Remarks:				Called once time after select a new region
//======================================================================================================================
VOID WM_CWaferMap::SetStartRegionPosition()
{
	if (IsUserRegionPickingMode())
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		ULONG ulPickingRegion = pSRInfo->GetPickingRegion();
		WM_CRegion *pRegion = pSRInfo->GetRegionList(ulPickingRegion);
	
		ULONG ulRegionRow = pRegion->GetRegionRow();
		ULONG ulRegionCol = pRegion->GetRegionCol();
		ULONG ulULRow = pRegion->GetRegionTopRow();
		ULONG ulULCol = pRegion->GetRegionLeftCol();

		// 1. Set the region row and region column in wafer map  
		m_pWaferMapWrapper->SetAlgorithmParameter("Read Region Row", ulRegionRow);
		m_pWaferMapWrapper->SetAlgorithmParameter("Read Region Col", ulRegionCol);

		// 2. set the start picking position of this region
		m_pWaferMapWrapper->SetStartPosition(ulULRow, ulULCol);
	}
}


//=====================================================================================================================
// Function Name: 		PrepareNextRegionSortingPath
// Input arguments:		None
// Output arguments:	None
// Description:   		Set the start position of the current region
//						with Use “Sorting Path 1 Offline?for the algorithm
//						and generate the first offline path
// Return:				None
// Remarks:				Called once time after select a new region
//======================================================================================================================
VOID WM_CWaferMap::PrepareNextRegionSortingPath(const UCHAR ucGrade, const ULONG ulPickingRegion)
{
//	SuspendAlgorithmPreparation();

	// 1. Select the grade, unsigned char ucGrade[1] = {?’};
	unsigned char ucGradeList[1];
	ucGradeList[0] = ucGrade;
	m_pWaferMapWrapper->SelectGrade(ucGradeList, 1);

	// 2. Calculate 1 region only
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	WM_CRegion *pRegion = pSRInfo->GetRegionList(ulPickingRegion);
	ULONG ulRegionRow = pRegion->GetRegionRow();
	ULONG ulRegionCol = pRegion->GetRegionCol();
	ULONG ulULRow = pRegion->GetRegionTopRow();
	ULONG ulULCol = pRegion->GetRegionLeftCol();	

	CString szCustomText;
	szCustomText.Format("[%d,%d]", ulRegionRow, ulRegionCol);

	m_pWaferMapWrapper->SetAlgorithmParameter("Only First Region", TRUE);
	m_pWaferMapWrapper->SetAlgorithmParameter("Custom Order", TRUE);
	m_pWaferMapWrapper->GetReader()->SetCustomText(szCustomText);

	// 3. Generate an offline path
	m_pWaferMapWrapper->SetAlgorithmParameter("Save File", TRUE);
	m_pWaferMapWrapper->SetAlgorithmParameter("Save Region Row", ulRegionRow);
	m_pWaferMapWrapper->SetAlgorithmParameter("Save Region Col", ulRegionCol);

//	m_pWaferMapWrapper->GenerateOfflinePath("Sorting Path 1", "Direct", ulULRow, ulULCol); // startrow and startcol is the position in region [ulRegionRow,ulRegionCol]
	
	//Resume
//	m_pWaferMapWrapper->ResumeAlgorithmPreparation(TRUE);
}



//=====================================================================================================================
// Function Name: 		PrepareNextSortingRegion
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the next sorting region with same grade or 
//						the next region with the next sorting grade in the current wafer
// Return:				None
// Remarks:				Called once time after pick all the same grade in the current region 
//======================================================================================================================
BOOL WM_CWaferMap::PrepareNextSortingRegion()
{
	if (IsUserRegionPickingMode())
	{
		// 1. Get the current sorting grade & region
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		UCHAR ucSortingGrade = pSRInfo->GetSortingRegionGrade();
		ULONG ulSortingRegion = pSRInfo->GetSortingRegion();

		// 2. Find the next sorting region with same grade
		if (!GetNextSortingRegion(ucSortingGrade))
		{
			// 3. Find the next new sorting grade
			ucSortingGrade = GetSortingNextGrade();
			if (ucSortingGrade == 0)
			{
				pSRInfo->SetSortingRegionGrade(0);
				pSRInfo->SetSortingRegion(pSRInfo->GetPickingRegion());
				return FALSE; //Wafer End
			}

			// 4. find the next region with the new sorting grade from 0 to n region
			pSRInfo->SetSortingRegion(0);
			pSRInfo->SetSortingRegionGrade(ucSortingGrade);
			if (!GetNextSortingRegion(ucSortingGrade))
			{
				pSRInfo->SetSortingRegionGrade(0);
				pSRInfo->SetSortingRegion(pSRInfo->GetPickingRegion());
				return FALSE; //Wafer End
			}
		}
	}

	return TRUE;
}



//=====================================================================================================================
// Function Name: 		PrepareFirstPickingRegion
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the first picking grade and region in the current wafer
// Return:				None
// Remarks:				Called once time after prescan done
//======================================================================================================================
BOOL WM_CWaferMap::PrepareFirstPickingRegion()
{
	//Get first region
	if (IsUserRegionPickingMode())
	{
		if (!IsAlgorithmSuspended())
		{
			SuspendAlgorithmPreparation();
		}

		// 1.Get the first picking grade in whole map after prescan map
		UCHAR ucSortingGrade = GetSortingGrade();
		if (ucSortingGrade == 0)
		{
			//Wafer End and stop bonding
			 return FALSE;
		}

		// 2.Initailize the picking region as 0 after prescan map
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		pSRInfo->SetSortingRegion(0);
		pSRInfo->SetSortingRegionGrade(ucSortingGrade);

		// 3.Get a picking region with this grade
		if (!GetNextSortingRegion(ucSortingGrade))
		{
			//Wafer End and stop bonding
			return FALSE;
		}

		//4. prepare the sorting path at current region 
		ULONG ulSortingRegion = pSRInfo->GetSortingRegion();
		PrepareNextRegionSortingPath(ucSortingGrade, ulSortingRegion);

		//5. Set the start position at current region
		pSRInfo->SetPickingRegionGrade(ucSortingGrade);
		pSRInfo->SetPickingRegion(pSRInfo->GetSortingRegion());
		SetStartRegionPosition();

		m_pWaferMapWrapper->ResumeAlgorithmPreparation(TRUE);
	}

	return TRUE;
}


//=====================================================================================================================
// Function Name: 		PrepareNextPickingRegion
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the next picking region with same grade or 
//						the next region with the next grade in the current wafer
// Return:				None
// Remarks:				Called once time after pick all the same grade in the current region 
//======================================================================================================================
BOOL WM_CWaferMap::PrepareNextPickingRegion()
{
	//Get first region
	if (IsUserRegionPickingMode())
	{
		if (!IsAlgorithmSuspended())
		{
			SuspendAlgorithmPreparation();
		}

		// 2.Initailize the picking region as 0 after prescan map
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		UCHAR ucSortingGrade = pSRInfo->GetSortingRegionGrade();
		ULONG ulSortingRegion = pSRInfo->GetSortingRegion();

		UCHAR ucPickingGrade = pSRInfo->GetPickingRegionGrade();
		ULONG ulPickingRegion = pSRInfo->GetPickingRegion();
		if ((ulSortingRegion == ulPickingRegion) && (ucSortingGrade == 0))
		{
			return FALSE; //Wafer End
		}

		pSRInfo->SetPickingRegionGrade(ucSortingGrade);
		pSRInfo->SetPickingRegion(ulSortingRegion);

		// 3.Get a picking region with this grade
//		ULONG ulLeft = 0;
//		ULONG ulTotal = 0;
//		if (!GetNextRegionStatistics(ucSortingGrade, ulLeft, ulTotal))
//		{
//			//Wafer End and stop bonding
//			return FALSE;
//		}

		//4. prepare the sorting path at current region 
		SetStartRegionPosition();

		m_pWaferMapWrapper->ResumeAlgorithmPreparation(TRUE);
	}

	return TRUE;
}



//===========================================================================================================
//
//===========================================================================================================
VOID WM_CWaferMap::CreateConstructMapNGGradeFile(const CString szLogPath, UCHAR ucNgGradeList[256])
{
	CString szConLogFile;

	FILE *fpNgGrade = NULL;
	szConLogFile = szLogPath + PRESCAN_NGGRADE_TXT;
	errno_t nErr = fopen_s(&fpNgGrade, szConLogFile, "w");
	if ((nErr == 0) && (fpNgGrade != NULL))
	{
		for(UCHAR ucLoop = 0; ucLoop < 255; ucLoop++)
		{
			if (ucNgGradeList[ucLoop] == ucLoop )
			{
				fprintf(fpNgGrade, "%d\n", ucLoop);
			}
		}
		fclose(fpNgGrade);
	}
}



//======================================================================================================
//		Sampling Rescan Ignore Grade
//======================================================================================================
BOOL WM_CWaferMap::GetSamplingRescanIgnoreGradeList(CUIntArray &aulSelectedGradeList)
{
	BOOL bRet = TRUE;
	LONG lGrade = 0;
	aulSelectedGradeList.RemoveAll();
	CString szSamplingRescanIgnoreGradeList = m_szSamplingRescanIgnoreGradeList;
	CString szGrade;

	while (!szSamplingRescanIgnoreGradeList.IsEmpty())
	{
		int nPos = szSamplingRescanIgnoreGradeList.Find(",");
		if (nPos == -1)
		{
			lGrade = atol(szSamplingRescanIgnoreGradeList);
			if ((lGrade > 0) && (lGrade < 256))
			{
				aulSelectedGradeList.Add(lGrade);
			}
			else
			{
				bRet = FALSE;
			}
			szSamplingRescanIgnoreGradeList.Empty();
		}
		else
		{
			szGrade = szSamplingRescanIgnoreGradeList.Left(nPos);
			lGrade = atol(szGrade);
			if ((lGrade > 0) && (lGrade < 256))
			{
				aulSelectedGradeList.Add(lGrade);
			}
			else
			{
				bRet = FALSE;
			}
			szSamplingRescanIgnoreGradeList = szSamplingRescanIgnoreGradeList.Right(szSamplingRescanIgnoreGradeList.GetLength() - nPos - 1);
		}
	}

	return bRet;
}