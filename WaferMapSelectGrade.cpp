//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "WaferMap.h"
#include "io.h"
#include "MS896A_Constant.h"
#include "Utility.H"
#include "LogFileUtil.h"
#include "WT_SubRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LONG WM_CWaferMap::IsInGradeList(const UCHAR ucGrade, CUIntArray &aulSelectedGradeList)
{
	LONG lGradeSize = (LONG)aulSelectedGradeList.GetSize();

	for (LONG i = 0; i < lGradeSize; i++)
	{
		if (ucGrade == (UCHAR)aulSelectedGradeList[i])
		{
			return i;
		}
	}
	return -1;
}

//===========================================================================================================================
//			Region Statistics Function
//===========================================================================================================================
//============================================================================================================
// Function Name: 		GetRegionStatistics
// Input arguments:		None
// Output arguments:	None
// Description:   		Get all left die number for each grade
// Return:				None
// Remarks:				None
//============================================================================================================
VOID WM_CWaferMap::GetRegionStatistics()
{
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
	ULONG ulStartRow = 0, ulStartCol = 0, ulEndRow = 0, ulEndCol = 0;
	pSRInfo->GetRegion(ulTgtRegion, ulStartRow, ulStartCol, ulEndRow, ulEndCol);
	//select grade to trigger the wafer map picking sequence 
	CalculateRegionStatistics(ulStartRow, ulStartCol, ulEndRow, ulEndCol);
}


//============================================================================================================
// Function Name: 		DecreaseRegionStatistics
// Input arguments:		ucGrade
// Output arguments:	None
// Description:   		Decrease one count for given grade
// Return:				None
// Remarks:				None
//============================================================================================================
VOID WM_CWaferMap::DecreaseRegionStatistics(const UCHAR ucGrade)
{
	UCHAR ucRegionGrade = 0;
	for (LONG i = 0; i < m_aulRegionGradeList.GetSize(); i++)
	{
		ucRegionGrade = (UCHAR)m_aulRegionGradeList[i];
		if (ucRegionGrade == ucGrade)
		{
			if (m_alRegionTotalList[i] > 0)
			{
				m_alRegionTotalList[i]--;
			}
		}
	}
}


//============================================================================================================
// Function Name: 		CalculateRegionStatistics
// Input arguments:		ucGrade
// Output arguments:	None
// Description:   		Get a left dice number for given grade
// Return:				None
// Remarks:				None
//============================================================================================================
ULONG WM_CWaferMap::GetRegionStatistics(const UCHAR ucGrade)
{
	UCHAR ucRegionGrade = 0;
	for (LONG i = 0; i < m_aulRegionGradeList.GetSize(); i++)
	{
		ucRegionGrade = (UCHAR)m_aulRegionGradeList[i];
		if (ucRegionGrade == ucGrade)
		{
			return m_alRegionTotalList[i];
		}
	}

	return 0;
}


//============================================================================================================
// Function Name: 		CalculateRegionStatistics
// Input arguments:		ulStartRow, ulStartCol, ulEndRow, ulEndCol  --- the size of region
// Output arguments:	None
// Description:   		Get all left dice number for each grade
// Return:				None
// Remarks:				None
//============================================================================================================
VOID WM_CWaferMap::CalculateRegionStatistics(const ULONG ulStartRow, const ULONG ulStartCol, const ULONG ulEndRow, const ULONG ulEndCol)
{
	CUIntArray aulSelectedGradeList;
	m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);

	m_aulRegionGradeList.RemoveAll();
	m_alRegionTotalList.RemoveAll();

	m_alRegionTotalList.SetSize(256);
	for (LONG i = 0; i < 255; i++)
	{
		m_alRegionTotalList[i] = 0;
	}

	for (ULONG ulRow = ulStartRow; ulRow < ulEndRow; ulRow++)
	{
		for (ULONG ulCol = ulStartCol; ulCol < ulEndCol; ulCol++)
		{
			if (IsMapHaveBin(ulRow, ulCol) && !m_pWaferMapWrapper->GetReader()->IsMissingDie(ulRow, ulCol))
			{
				UCHAR ucGrade = m_pWaferMapWrapper->GetGrade(ulRow, ulCol);
				
				if (IsInGradeList(ucGrade, aulSelectedGradeList) >= 0)
				{
					LONG lGradeIndex = -1;
					if ((lGradeIndex = IsInGradeList(ucGrade, m_aulRegionGradeList)) == -1)
					{
						m_aulRegionGradeList.Add(ucGrade);
						lGradeIndex = (LONG)m_aulRegionGradeList.GetSize() - 1;
					}
					m_alRegionTotalList[lGradeIndex] = m_alRegionTotalList[lGradeIndex] + 1;
				}
			}
		}
	}
}

//============================================================================================================
// Function Name: 		SelectRegionGradeList
// Input arguments:		ulStartRow, ulStartCol, ulEndRow, ulEndCol  --- the size of region
//						lSortingMode
// Output arguments:	None
// Description:   		re-select grade order according to the quantity of grade in order
// Return:				None
// Remarks:				None
//============================================================================================================
VOID WM_CWaferMap::SelectRegionGradeList(const ULONG ulStartRow, const ULONG ulStartCol, const ULONG ulEndRow, const ULONG ulEndCol, 
										 const CString szCustomerName, const BOOL bKeepLastUnloadGrade, const UCHAR ucLastPickDieGrade, 
										 const LONG lSpecialRefDieGrade, const BOOL bIgnoreRegion,
										 const LONG lSortingMode, const ULONG ulMinGradeCount, const ULONG ulMinGradeBinNo,  BOOL &bSortMultiToOne, 
										 CUIntArray &unArrMinGradeBin)
{
	CUIntArray aulSelectedGradeList;
/*
	m_pWaferMapWrapper->CalculateRegionStatistics(ulStartRow, ulStartCol, ulEndRow, ulEndCol);
	m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);
	if (lSortingMode == WT_SORT_MIN_DIE)
	{
		m_pWaferMapWrapper->GetRegionAscendGradeList(aulSelectedGradeList);
	}
	else
	{
		m_pWaferMapWrapper->GetRegionDescendGradeList(aulSelectedGradeList);
	}
*/
	CalculateRegionStatistics(ulStartRow, ulStartCol, ulEndRow, ulEndCol);
	for (LONG i = 0; i < m_aulRegionGradeList.GetSize(); i++)
	{
		aulSelectedGradeList.Add(m_aulRegionGradeList[i]);
	}

	ULONG ulWaferTotalDie = UpdateSortingMode(TRUE, szCustomerName, bKeepLastUnloadGrade, ucLastPickDieGrade, 
											  lSpecialRefDieGrade, bIgnoreRegion,
											  lSortingMode, ulMinGradeCount, ulMinGradeBinNo,  bSortMultiToOne, unArrMinGradeBin);

	
	if (ulWaferTotalDie == 0)
	{
		ULONG ulGradeListSize = (LONG)aulSelectedGradeList.GetSize();
		UCHAR *paucGrade = new UCHAR[ulGradeListSize];
		for (ULONG i = 0; i < ulGradeListSize; i++)
		{
			paucGrade[i] = (UCHAR)aulSelectedGradeList[i];
		}
		m_pWaferMapWrapper->SelectGrade(paucGrade, ulGradeListSize);
		delete[] paucGrade;
	}
}


ULONG WM_CWaferMap::UpdateSortingMode(const BOOL bIsRegionPicking, const CString szCustomerName, const BOOL bKeepLastUnloadGrade, const UCHAR ucLastPickDieGrade, 
									  const LONG lSpecialRefDieGrade, const BOOL bIgnoreRegion, 
									  const LONG lSortingMode, const ULONG ulMinGradeCount, const ULONG ulMinGradeBinNo,  BOOL &bSortMultiToOne, 
									  CUIntArray &unArrMinGradeBin)
{
	RemoveInkDieGrade(szCustomerName);

	if ((lSortingMode == WT_SORT_NORMAL) && (ulMinGradeCount == 0))
	{
		return 0;
	}

	CUIntArray aulSelectedGradeList;
	ULONG	ulWaferTotalDie = 0;
	ULONG	ulPick = 0;

	//Get current selected grade no & total
	if (bIsRegionPicking)
	{
		for (LONG i = 0; i < m_aulRegionGradeList.GetSize(); i++)
		{
			aulSelectedGradeList.Add(m_aulRegionGradeList[i]);
		}
	}
	else
	{
		m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);
	}

	ULONG ulGradeListSize = (LONG)aulSelectedGradeList.GetSize();
	ULONG *paulLeft = new ULONG[ulGradeListSize];
	ULONG *paulTotal = new ULONG[ulGradeListSize];
	UCHAR *paucGrade = new UCHAR[ulGradeListSize];

	for (ULONG i = 0; i < ulGradeListSize; i++)
	{
		if (bIsRegionPicking)
		{
			paulLeft[i] = paulTotal[i] = m_alRegionTotalList[i];
		}
		else
		{
			m_pWaferMapWrapper->GetStatistics((UCHAR)aulSelectedGradeList[i], paulLeft[i], ulPick, paulTotal[i]);
		}
		
		paucGrade[i] = (UCHAR)aulSelectedGradeList[i];
		ulWaferTotalDie += paulTotal[i];
	}


	SelectSortGradeList(lSortingMode, ulMinGradeCount, ulMinGradeBinNo, bSortMultiToOne, aulSelectedGradeList, paulTotal, paulLeft, paucGrade, unArrMinGradeBin);

	// Set the last grade as the top of the list if 
	// 1. Keep Last Unload Grade and  is Enabled

	if (bKeepLastUnloadGrade == TRUE)
	{
		UpdateLastUnloadSortingMode(ucLastPickDieGrade);
	}

	// Move the special Ref die grade to the end
	UpdateSpecialRefDieGradeList(lSpecialRefDieGrade);

	UpdateCustomerNameGradeList(szCustomerName);

	m_pWaferMapWrapper->SetDieSelectMode(bIgnoreRegion);

	delete[] paulLeft;
	delete[] paulTotal;
	delete[] paucGrade;

	return ulWaferTotalDie;
}


VOID WM_CWaferMap::RemoveInkDieGrade(const CString szCustomerName)
{
	if (szCustomerName =="Cree")
	{
		//Get current selected grade no & total
		CUIntArray aulSelctdList;
		m_pWaferMapWrapper->GetSelectedGradeList(aulSelctdList);
		UCHAR	*pNewList;
		pNewList = new UCHAR[aulSelctdList.GetSize()];

		BOOL bInkDieIn = FALSE;
		INT		nNewListSize = 0;
		for (INT i=0; i<aulSelctdList.GetSize(); i++)
		{
			UCHAR ucTempGrade = aulSelctdList.GetAt(i);
			USHORT usOrgGrade = m_pWaferMapWrapper->GetOriginalGrade(ucTempGrade)- m_pWaferMapWrapper->GetGradeOffset();
			if( usOrgGrade==(65533) )	//	die grade -3
			{
				bInkDieIn = TRUE;
				continue;
			}
			pNewList[nNewListSize] = aulSelctdList.GetAt(i);
			nNewListSize++;
		}

		if( bInkDieIn )
		{
			m_pWaferMapWrapper->SelectGrade(pNewList, nNewListSize);
		}
		delete[] pNewList;
	}
}

//----------------------------------------------//
// Features :Update the Grading List So that it //
// will start with the last picked grade for	//
// the new wafer loaded							//
//----------------------------------------------//
BOOL WM_CWaferMap::UpdateLastUnloadSortingMode(const UCHAR ucLastPickDieGrade)
{
	CUIntArray aulReOrderGradeList,aulSelectedGradeList;
	UCHAR ucGrade = 0, ucLastUnloadGrade;
	BOOL bLastUnloadGradeFound = FALSE;
	UCHAR	*pGrade;
	INT nConvID = 0, k=0;
	
	// Get The Last Pick Die Grade
	if (ucLastPickDieGrade < m_pWaferMapWrapper->GetGradeOffset())
		return TRUE;

	ucLastUnloadGrade = ucLastPickDieGrade - m_pWaferMapWrapper->GetGradeOffset();
	if (ucLastUnloadGrade == 0)
	{
		return TRUE;
	}

	m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);

	for (k=0; k<aulSelectedGradeList.GetSize(); k++)
	{
		ucGrade = aulSelectedGradeList.GetAt(k) - m_pWaferMapWrapper->GetGradeOffset();

		if (ucGrade == ucLastUnloadGrade)
		{
			bLastUnloadGradeFound = TRUE;
			aulReOrderGradeList.Add(aulSelectedGradeList.GetAt(k));
			break;
		}
	}

	if (bLastUnloadGradeFound)
	{
		pGrade = new UCHAR[aulSelectedGradeList.GetSize()];

		for (int i=0; i<aulSelectedGradeList.GetSize(); i++)
		{
			if (i==k)
			{
			}
			else
			{
				aulReOrderGradeList.Add(aulSelectedGradeList.GetAt(i));
			}
		}

		for (int j=0; j<aulReOrderGradeList.GetSize(); j++)
		{
			pGrade[j] = aulReOrderGradeList.GetAt(j);
		}

		m_pWaferMapWrapper->SelectGrade(pGrade, (unsigned long)aulReOrderGradeList.GetSize());
		delete[] pGrade;
	}
	return TRUE;
}


//----------------------------------------------//
// Features : When special Ref grade is enabled	Move that grade to the end of the list		//
//												//
//----------------------------------------------//
BOOL WM_CWaferMap::UpdateSpecialRefDieGradeList(const LONG lSpecialRefDieGrade)
{
	if (lSpecialRefDieGrade == 0)
	{
		return TRUE;
	}

	BOOL bFoundRefDieGrade = FALSE;
	CUIntArray aulSelectedGradeList;

	m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);

	for (INT k = 0; k < aulSelectedGradeList.GetSize(); k++)
	{
		if (lSpecialRefDieGrade == (aulSelectedGradeList.GetAt(k) - m_pWaferMapWrapper->GetGradeOffset()))
		{
			aulSelectedGradeList.RemoveAt(k);
			bFoundRefDieGrade = TRUE;
			break;
		}
	}
	

	if (bFoundRefDieGrade == TRUE)
	{
		UCHAR *pGrade = new UCHAR[aulSelectedGradeList.GetSize()];
		
		aulSelectedGradeList.Add((lSpecialRefDieGrade + m_pWaferMapWrapper->GetGradeOffset()));

		for (INT j = 0; j < aulSelectedGradeList.GetSize(); j++)
		{
			pGrade[j] = aulSelectedGradeList.GetAt(j);
		}

		m_pWaferMapWrapper->SelectGrade(pGrade, (unsigned long)aulSelectedGradeList.GetSize());
		delete[] pGrade;
	}

	return TRUE;
}


VOID WM_CWaferMap::UpdateCustomerNameGradeList(const CString szCustomerName)
{
	if (szCustomerName == "SeoulSemi")		//v4.47T5
	{
		m_pWaferMapWrapper->SetAlgorithmParameter("Pick Reference", TRUE);

		CString szTemp;
		ULONG ulNoOfRows=0, ulNoOfCols=0;
		m_pWaferMapWrapper->GetMapDimension(ulNoOfRows, ulNoOfCols);

		LONG lUserRow=0, lUserCol=0;
		if ( IsMapHaveBin(ulNoOfRows-1, 0) &&
			 (m_pWaferMapWrapper->IsReferenceDie(ulNoOfRows-1, 0)) )
		{
			UCHAR ucGrade		= 0;
			UCHAR ucRefGrade	= m_pWaferMapWrapper->GetGrade(ulNoOfRows-1, 0);
			
			LONG lUserRow=0, lUserCol=0;
			m_pWaferMapWrapper->ConvertInternalToUser(ulNoOfRows-1, 0, lUserRow, lUserCol);

			CUIntArray aulReOrderGradeList, aulSelectedGradeList;
			BOOL bRefGradeFound = FALSE;

			m_pWaferMapWrapper->GetSelectedGradeList(aulSelectedGradeList);

			for (INT k=0; k<aulSelectedGradeList.GetSize(); k++)
			{
				ucGrade = aulSelectedGradeList.GetAt(k);

				if (ucGrade == ucRefGrade)
				{
					bRefGradeFound = TRUE;
				}
				else
				{
					aulReOrderGradeList.Add(aulSelectedGradeList.GetAt(k));
				}
			}

			if (bRefGradeFound)
			{
				szTemp.Format("SeoulSemi: PickRefDie enabled at (%d, %d); grade = %d", 
					lUserRow, lUserCol, ucRefGrade - m_pWaferMapWrapper->GetGradeOffset());
				CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
				aulReOrderGradeList.Add(ucRefGrade);

				UCHAR	*pGrade;
				pGrade = new UCHAR[aulSelectedGradeList.GetSize()];
				for (LONG j=0; j<aulReOrderGradeList.GetSize(); j++)
				{
					pGrade[j] = aulReOrderGradeList.GetAt(j);
				}

				m_pWaferMapWrapper->SelectGrade(pGrade, (unsigned long)aulReOrderGradeList.GetSize());
				m_pWaferMapWrapper->SetDieSpecialFlag(ulNoOfRows-1, 0, DIE_SPECIAL_WAFER_END);
				delete[] pGrade;
			}
		}	//If RefDie found
	}	//If SeoulSemi
}




VOID WM_CWaferMap::SelectSortGradeList(const LONG lSortingMode, const ULONG ulMinGradeCount, const ULONG ulMinGradeBinNo, 
									   BOOL &bSortMultiToOne, CUIntArray &aulSelectedGradeList, ULONG *pulTotal, ULONG *pulLeft, UCHAR *pGrade,
									   CUIntArray &unArrMinGradeBin)
{
	unArrMinGradeBin.RemoveAll();
	BOOL bEnableMinCountSort = FALSE;
	if ((ulMinGradeCount > 0 || bSortMultiToOne) && (ulMinGradeBinNo > 0))
	{
		bEnableMinCountSort = TRUE;
	}

	LONG nListSize = (LONG)aulSelectedGradeList.GetSize();

	if (lSortingMode == WT_SORT_USER_DEFINE)
	{
		//Use user define grade list
		nListSize = GetSelectedUserGradeList(aulSelectedGradeList, pulTotal, pGrade);
	}
	else if ((lSortingMode == WT_SORT_MIN_DIE) || (lSortingMode == WT_SORT_MAX_DIE))
	{
		//Sort grade in order if necessary
		GetSelectedSortGradeList(lSortingMode, 0, nListSize, pulTotal, pulLeft, pGrade);
	}

	CUIntArray aulGroupGradeList;

	if (bSortMultiToOne)
	{
		aulGroupGradeList.RemoveAll();
		//Use user define grade list
		if (GetGroupGradeList(aulGroupGradeList) == FALSE)
		{
			bSortMultiToOne = FALSE;
		}
	}


	//Re-order grade from user input
	CUIntArray aulReOrderGradeList;

	for (LONG i = 0; i < nListSize; i++)
	{
		if ( (pulTotal[i] > ulMinGradeCount) || bEnableMinCountSort)	//v4.38T7	//Mixed bin sorting
		{
			aulReOrderGradeList.Add((UINT)pGrade[i]);
		}

		//v4.38T7	//Mixed bin sorting
		if (bSortMultiToOne)
		{
			//Use user define grade list
			for (LONG j = 0; j < aulGroupGradeList.GetSize(); j++) 
			{
				if (aulGroupGradeList[j] == pGrade[i])
				{
					unArrMinGradeBin.Add((UINT) pGrade[i]);

					CString szLog;
					szLog.Format("Grade added to mixed bin array: %d", 
						((UINT) pGrade[i]) - m_pWaferMapWrapper->GetGradeOffset());
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
					break;
				}
			}
		}
		else
		{
			if ((pulTotal[i] <= ulMinGradeCount) && bEnableMinCountSort)
			{
				unArrMinGradeBin.Add((UINT) pGrade[i]);

				CString szLog;
				szLog.Format("Min Grade added to mixed bin array: %d", 
					((UINT) pGrade[i]) - m_pWaferMapWrapper->GetGradeOffset());
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}
		}
	}

	CString szLogMsg = "Pick list", szTemp;
	for (LONG i = 0; i < aulReOrderGradeList.GetSize(); i++)
	{
		pGrade[i] = aulReOrderGradeList.GetAt(i);
		szTemp.Format("    %d", aulReOrderGradeList.GetAt(i) - m_pWaferMapWrapper->GetGradeOffset());
		szLogMsg += szTemp;
	}
	//SaveScanTimeEvent(szLogMsg);

	m_pWaferMapWrapper->SelectGrade(pGrade, (unsigned long)aulReOrderGradeList.GetSize());
}

LONG WM_CWaferMap::GetSelectedUserGradeList(CUIntArray &aulSelectedGradeList, ULONG *pulTotal, UCHAR *pGrade)
{
	CUIntArray aulUserGradeList;
	LONG	nListSize = 0;
	ULONG *pulOutputTotal;

	pulOutputTotal = new ULONG[aulSelectedGradeList.GetSize()];
	for (LONG i = 0; i < (LONG)aulSelectedGradeList.GetSize(); i++)
	{
		pulOutputTotal[i] = pulTotal[i];
	}

	//Use user define grade list
	if (GetUserGradeList(aulUserGradeList) == TRUE)
	{
		for (LONG j = 0; j < (LONG)aulUserGradeList.GetSize(); j++) 
		{
			for (LONG i = 0; i < (LONG)aulSelectedGradeList.GetSize(); i++)
			{
				if (aulUserGradeList[j] == aulSelectedGradeList[i])
				{
					pGrade[nListSize] = (UCHAR)aulUserGradeList[j];
					pulTotal[nListSize] = pulOutputTotal[i];
					nListSize++;
				}
			}
		}
	}
	return nListSize;
}


VOID WM_CWaferMap::GetSelectedSortGradeList(const LONG lSortingMode, const LONG nStartIndex, const LONG nListSize, ULONG *pulTotal, ULONG *pulLeft, UCHAR *pGrade)
{
	ULONG	ulTempCount = 0;
	UCHAR	ucTempGrade = 0;
	BOOL	bUpdate = FALSE;	

	if ( (lSortingMode == WT_SORT_MIN_DIE) || (lSortingMode == WT_SORT_MAX_DIE) )
	{
		//Sort grade in order if necessary
		for (LONG i = nStartIndex; i < nListSize - 1; i++)
		{
			for (LONG j = i + 1; j < nListSize; j++)
			{
				bUpdate = FALSE;
				if (lSortingMode == WT_SORT_MIN_DIE)
				{
					//if (pTotal[i] > pTotal[j])
					if (pulLeft[i] > pulLeft[j])
					{		
						bUpdate = TRUE;
					}
				}
				else
				{
					//if (pTotal[i] < pTotal[j])
					if (pulLeft[i] < pulLeft[j])
					{		
						bUpdate = TRUE;
					}
				}

				if (bUpdate == TRUE)
				{
					ucTempGrade = pGrade[i];
					pGrade[i]	= pGrade[j];
					pGrade[j]	= ucTempGrade;

					ulTempCount = pulTotal[i];
					pulTotal[i]	= pulTotal[j];
					pulTotal[j]	= ulTempCount;

					ulTempCount = pulLeft[i];
					pulLeft[i]	= pulLeft[j];
					pulLeft[j]	= ulTempCount;
				}
			}
		}
	}
}


BOOL WM_CWaferMap::GetUserGradeList(CUIntArray& aulGradeList)
{
	if ((_access(gszUSER_DIRECTORY + "\\MapFile\\GradeList.txt", 0 )) == -1)
	{
		return FALSE;
	}

	FILE *fList = NULL;
	ULONG i = 0;
	INT nGrade;
	ULONG ulTotalGrades = 0;

	errno_t nErr = fopen_s(&fList, (gszUSER_DIRECTORY + "\\MapFile\\GradeList.txt"), "r");
	if ((nErr == 0) && (fList != NULL))
	{
		fscanf_s(fList, "%d\n", &ulTotalGrades, sizeof(ulTotalGrades));

		//Klocwork	//v4.02T5
		ulTotalGrades = min(ulTotalGrades, 1000);
		ulTotalGrades = max(ulTotalGrades, 0);

		for (i=0; i<ulTotalGrades; i++)
		{
			fscanf_s(fList, "%d\n", &nGrade, sizeof(nGrade));		//v3.45	//Use INT instead of UCHAR
			aulGradeList.Add((UINT) nGrade);
		}

		fclose(fList);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}



BOOL WM_CWaferMap::SaveUserGradeList(const unsigned char* aucGrade, const unsigned long ulNumberOfGrades)
{
	FILE *fList = NULL;
	ULONG i=0;

	DeleteFile((gszUSER_DIRECTORY + "\\MapFile\\GradeList.txt"));	

	errno_t nErr = fopen_s(&fList, (gszUSER_DIRECTORY + "\\MapFile\\GradeList.txt"), "w");
	if ((nErr == 0) && (fList != NULL))
	{
		fprintf(fList, "%ld\n", ulNumberOfGrades);
		
		for (i=0; i<ulNumberOfGrades; i++ )
		{
			fprintf(fList, "%d\n", aucGrade[i]);	//v3.45	//Use INT instead of UCHAR
		}

		fclose(fList);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}


BOOL WM_CWaferMap::GetGroupGradeList(CUIntArray& aulGroupGradeList)
{
	CString szFileName = gszUSER_DIRECTORY + "\\MapFile\\GroupGradeList.txt";
	CStdioFile fFile;
	ULONG ulTotalGrades = 0;

	if (fFile.Open(szFileName, CFile::modeRead | CFile::shareDenyNone))
	{
		CStringArray szaDataList;
		CString szReading;
		fFile.SeekToBegin();
		while (fFile.ReadString(szReading))
		{
			szaDataList.RemoveAll();
			CUtility::Instance()->ParseRawData(szReading, szaDataList);
			for (INT i = 0; i < szaDataList.GetSize(); i++)
			{
				ulTotalGrades++;
				INT nGrade = atoi(szaDataList.GetAt(i)) + m_pWaferMapWrapper->GetGradeOffset();
				aulGroupGradeList.Add((UINT) nGrade);
			}
		}
		fFile.Close();
	}

	if (ulTotalGrades == 0)
	{
		//HmiMessage_Red_Back("Group grade sorting file failure.", "Export Machine Data");
		return FALSE;
	}

	return TRUE;
}
