/////////////////////////////////////////////////////////////////
// BinLoader.cpp : interface of the CBinLoader class
//
//	Description:
//		
//
//	Date:		12 August 2004
//	Revision:	1.00
//
//	By:			
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., .
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NGGrade.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CNGGrade *CNGGrade::m_pInstance = NULL;

CNGGrade::CNGGrade()
{
	//For NG Grade
	m_bHaveNGGrade = FALSE;
	m_bNGGrade148 = FALSE;
	m_bNGGrade149 = FALSE;
	m_bNGGrade150 = FALSE;
	m_bNGGradeOther = FALSE;
	m_szNGGradeOtherList = "";

	m_lSrchNGDieScore		= 70;
}

CNGGrade::~CNGGrade()
{
}


//================================================================
// Function Name: 		Instance
// Input arguments:     None
// Output arguments:	None
// Description:   		Get singleton instance
// Return:				None
// Remarks:				None
//================================================================
CNGGrade *CNGGrade::Instance()
{
	if (m_pInstance == NULL) 
	{
		m_pInstance = new CNGGrade;
	}
	return m_pInstance;
}


VOID CNGGrade::WriteData(CMapElement *pElement)
{
	if (pElement)
	{
		(*pElement)[BPR_PB_HAVE_NG_GRADE]		= m_bHaveNGGrade;
		(*pElement)[BPR_PB_NG_GRADE_148_SET]	= m_bNGGrade148;
		(*pElement)[BPR_PB_NG_GRADE_149_SET]	= m_bNGGrade149;
		(*pElement)[BPR_PB_NG_GRADE_150_SET]	= m_bNGGrade150;
		(*pElement)[BPR_PB_NG_GRADE_Other_SET]	= m_bNGGradeOther;
		(*pElement)[BPR_PB_NG_GRADE_Other_LIST]	= m_szNGGradeOtherList;
		(*pElement)[BPR_SRH_NG_NDIE_PASSSCORE]	= m_lSrchNGDieScore;
		if (m_lSrchNGDieScore < 20)
		{
			m_lSrchNGDieScore = 20;
		}
	}
}


VOID CNGGrade::ReadData(CMapElement *pElement)
{
	if (pElement)
	{
		m_bHaveNGGrade				= (BOOL)(LONG)(*pElement)[BPR_PB_HAVE_NG_GRADE];
		m_bNGGrade148				= (BOOL)(LONG)(*pElement)[BPR_PB_NG_GRADE_148_SET];
		m_bNGGrade149				= (BOOL)(LONG)(*pElement)[BPR_PB_NG_GRADE_149_SET];
		m_bNGGrade150				= (BOOL)(LONG)(*pElement)[BPR_PB_NG_GRADE_150_SET];
		m_bNGGradeOther				= (BOOL)(LONG)(*pElement)[BPR_PB_NG_GRADE_Other_SET];
		m_szNGGradeOtherList		= (*pElement)[BPR_PB_NG_GRADE_Other_LIST];
	}
}


BOOL CNGGrade::IsHaveNGGrade()
{
	return m_bHaveNGGrade;
}


//================================================================
// Function Name: 		IsNGBlock
// Input arguments:     None
// Output arguments:	None
// Description:   		Is NG Block
// Return:				None
// Remarks:				None
//================================================================
BOOL CNGGrade::IsNGBlock(const ULONG ulBlkInUse)
{
	if((ulBlkInUse == 148) && m_bNGGrade148)
	{
		return TRUE;
	}
	else if((ulBlkInUse == 149) && m_bNGGrade149)
	{
		return TRUE;
	}
	else if((ulBlkInUse == 150) && m_bNGGrade150)
	{
		return TRUE;
	}
	else if(m_bNGGradeOther)
	{
		int found = m_szNGGradeOtherList.Find(",");
		if(found == -1)
		{
			if(ulBlkInUse == atol(m_szNGGradeOtherList))
			{
				return TRUE;
			}
		}
		else
		{
			CString temp; //save bin blk number
			CString temp2 = m_szNGGradeOtherList; // save and modify the ng grade list
			while(1)
			{
				temp = temp2.Left(found);
				if(ulBlkInUse == atol(temp))	// only one other bin blk
				{
					return TRUE;
					break;
				}
				else
				{
					temp2 = temp2.Right(temp2.GetLength() - found - 1);
					found = temp2.Find(",");
					if(found == -1)
					{
						if(ulBlkInUse == atol(temp2)) // last bin blk
						{
							return TRUE;
						}
						break;
					}
				}
			}
		}
	}
	return FALSE;
}


//================================================================
// Function Name: 		GetNGGradeBlockList
// Input arguments:     None
// Output arguments:	None
// Description:   		Get NG Grade List
// Return:				NG Grade List
// Remarks:				None
//================================================================
VOID CNGGrade::GetNGGradeBlockList(CUIntArray &aulNGGradeBlockList)
{
	LONG lNGGradeBlock = 1;
	m_bHaveNGGrade = FALSE;

	aulNGGradeBlockList.RemoveAll();
	if (m_bNGGrade148 == TRUE || m_bNGGrade149 == TRUE || m_bNGGrade150 == TRUE || m_bNGGradeOther == TRUE)
	{
		m_bHaveNGGrade = TRUE;
		if (m_bNGGrade148 == TRUE)
		{
			aulNGGradeBlockList.Add(148);
		}
		if (m_bNGGrade149 == TRUE)
		{
			aulNGGradeBlockList.Add(149);
		}
		if (m_bNGGrade150 == TRUE)
		{
			aulNGGradeBlockList.Add(150);
		}

		if (m_bNGGradeOther == TRUE && m_szNGGradeOtherList.IsEmpty() == FALSE)
		{
			int found = m_szNGGradeOtherList.Find(",");
			if (found == -1)
			{
				lNGGradeBlock = atol(m_szNGGradeOtherList);
				aulNGGradeBlockList.Add(lNGGradeBlock);
			}
			else
			{
				CString temp; //save bin blk number
				CString temp2 = m_szNGGradeOtherList; // save and modify the ng grade list
				while(1)
				{
					temp = temp2.Left(found);
					lNGGradeBlock = atol(temp);
					aulNGGradeBlockList.Add(lNGGradeBlock);
					temp2 = temp2.Right(temp2.GetLength() - found - 1);
					found = temp2.Find(",");
					if (found == -1)
					{
						temp = temp2.Left(found);
						lNGGradeBlock = atol(temp);
						aulNGGradeBlockList.Add(lNGGradeBlock);
						break;
					}
				}
			}
		}
	}
}


//================================================================
// Function Name: 		GetCurrentNGGradeBlock
// Input arguments:     None
// Output arguments:	None
// Description:   		Get Current NG Grade's Block
// Return:				NG Grade Block
// Remarks:				None
//================================================================
LONG CNGGrade::GetCurrentNGGradeBlock()
{
	LONG lCurrentNGGradeBlock = 1;

	if (m_bNGGrade148 == TRUE)
	{
		lCurrentNGGradeBlock = 148;
	}
	else if (m_bNGGrade149 == TRUE)
	{
		lCurrentNGGradeBlock = 149;
	}
	else if (m_bNGGrade150 == TRUE)
	{
		lCurrentNGGradeBlock = 150;
	}
	else if (m_bNGGradeOther == TRUE && m_szNGGradeOtherList.IsEmpty() == FALSE)
	{
		int found = m_szNGGradeOtherList.Find(",");
		if (found == -1)
		{
			lCurrentNGGradeBlock = atol(m_szNGGradeOtherList);
		}
		else
		{
			CString temp; //save bin blk number
			CString temp2 = m_szNGGradeOtherList; // save and modify the ng grade list
			while(1)
			{
				temp = temp2.Left(found);
				lCurrentNGGradeBlock = atol(temp);
				temp2 = temp2.Right(temp2.GetLength() - found - 1);
				found = temp2.Find(",");
				if (found == -1)
				{
					temp = temp2.Left(found);
					lCurrentNGGradeBlock = atol(temp);
					break;
				}
			}
		}
	}

	return lCurrentNGGradeBlock;
}


BOOL CNGGrade::IsInNGGradeBlockList(const LONG lBlockID, CUIntArray &aulNGGradeBlockList)
{
	for (LONG i = 0; i < aulNGGradeBlockList.GetSize(); i++)
	{
		LONG lNewBinBlock = (LONG)aulNGGradeBlockList[i];
		if (lBlockID == lNewBinBlock)
		{
			return TRUE;
		}
	}

	return FALSE;
}