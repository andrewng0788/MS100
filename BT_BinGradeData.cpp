#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BT_BinGradeData.h"
#include "FileUtil.h"

#pragma once

/**************************** Class CBinGradeData Start ****************************/

CBinGradeData::CBinGradeData()
{
	m_pstNvBinGradeData		= NULL;
	//m_pstNvBinGradeData_HW	= NULL;		//v4.65A3

	m_ulGradeCapacity = 0;
	m_ulInputCount = 0;
	m_ucAliasGrade = 0;
	m_bIsAssigned = FALSE;
	m_ulStatRowIndex = 0;
	m_szRankID = "";
	m_szBlockSize = "";
	m_ulMinFrameDieCount = 0;
	m_ulMaxFrameDieCount = 0;
} //end of constructor


CBinGradeData::~CBinGradeData()
{
} //end of destructor


/*************************/
/*   Get/Set functions   */
/*************************/

ULONG CBinGradeData::GetGradeCapacity()
{
	return m_ulGradeCapacity;
} //end of GetGradeCapacity


BOOL CBinGradeData::SetGradeCapacity(ULONG ulGradeCapacity)
{
	m_ulGradeCapacity = ulGradeCapacity;
	return TRUE;
} //end of SetGradeCapacity


ULONG CBinGradeData::GetInputCount()
{
	return m_ulInputCount;
} //end GetInputCount


BOOL CBinGradeData::SetInputCount(ULONG ulInputCount)
{
	m_ulInputCount = ulInputCount;
	return TRUE;
} //end SetInputCount


UCHAR CBinGradeData::GetAliasGrade()
{
	return m_ucAliasGrade;
} //end of GetAliasGrade


BOOL CBinGradeData::SetAliasGrade(UCHAR ucAliasGrade)
{
	m_ucAliasGrade = ucAliasGrade;
	return TRUE;
} //end of SetAliasGrade


BOOL CBinGradeData::GetIsAssigned()
{
	return m_bIsAssigned;
} //end GetIsAssigned


BOOL CBinGradeData::SetIsAssigned(BOOL bIsAssigned)
{
	m_bIsAssigned = bIsAssigned;
	return TRUE;
} //end SetIsAssigned


ULONG CBinGradeData::GetStatRowIndex()
{
	return m_ulStatRowIndex;
} //end GetStatRowIndex


BOOL CBinGradeData::SetStatRowIndex(ULONG ulStatRowIndex)
{
	m_ulStatRowIndex = ulStatRowIndex;
	return TRUE;
} //end SetStatRowIndex


BOOL CBinGradeData::SetPSTNVBinGradeData(void *pvNVRAM, LONG lNVRAM_BinTable_Start, 
			LONG lNVRAM_BinTable_Size, UCHAR ucGrade)
{
	BT_NVBINGRADEDATA *pTemp;

	pTemp = (BT_NVBINGRADEDATA*)((ULONG)pvNVRAM + 
			lNVRAM_BinTable_Start + lNVRAM_BinTable_Size / 2);
	m_pstNvBinGradeData = pTemp + ucGrade;
	
	return TRUE;
} //end SetPSTNVBinGradeData

BOOL CBinGradeData::SetPSTNVBinGradeData_HW(void *pvNVRAM_HW, LONG lNVRAM_BinTable_Start, 
			LONG lNVRAM_BinTable_Size, UCHAR ucGrade)
{
	BT_NVBINGRADEDATA *pTemp;

	pTemp = (BT_NVBINGRADEDATA*)((ULONG)pvNVRAM_HW + lNVRAM_BinTable_Start + lNVRAM_BinTable_Size / 2);
	//m_pstNvBinGradeData_HW = pTemp + ucGrade;
	
	return TRUE;
} //end SetPSTNVBinGradeData


ULONG CBinGradeData::GetNVNoOfSortedDie()
{
	if (m_pstNvBinGradeData == NULL)
	{
		return 0;
	}

	return m_pstNvBinGradeData->ulNoOfSortedDie;
} //end GetNVNoOfSortedDie


BOOL CBinGradeData::SetNVNoOfSortedDie(ULONG ulNvNoOfSortedDie)
{
	if (m_pstNvBinGradeData == NULL)
	{
		return TRUE;
	}

	m_pstNvBinGradeData->ulNoOfSortedDie = ulNvNoOfSortedDie;
	//v4.65A3
	/*if (m_pstNvBinGradeData_HW != NULL)
	{
		m_pstNvBinGradeData_HW->ulNoOfSortedDie = ulNvNoOfSortedDie;
	}*/

	return TRUE;
} //end SetNVNoOfSortedDie


ULONG CBinGradeData::GetNVBlkInUse()
{
	if (m_pstNvBinGradeData == NULL)
	{
		return 0;
	}

	return m_pstNvBinGradeData->ulBlkInUse;
} //end GetNVBlkInUse


BOOL CBinGradeData::SetNVBlkInUse(ULONG ulNvBlkInUse)
{
	if (m_pstNvBinGradeData == NULL)
	{
		return TRUE;
	}

	m_pstNvBinGradeData->ulBlkInUse = ulNvBlkInUse;
	//v4.65A3
	/*if (m_pstNvBinGradeData_HW != NULL)
	{
		m_pstNvBinGradeData_HW->ulBlkInUse = ulNvBlkInUse;
	}*/

	return TRUE;
} //end SetNVBlkInUse


BOOL CBinGradeData::GetNVIsFull()
{
	if (m_pstNvBinGradeData == NULL)
	{
		return TRUE;
	}

	return m_pstNvBinGradeData->bIsFull;
} //end GetNVIsFull


BOOL CBinGradeData::SetNVIsFull(BOOL bNvIsFull)
{
	if (m_pstNvBinGradeData == NULL)
	{
		return TRUE;
	}
	//v4.65A3
	/*if (m_pstNvBinGradeData_HW != NULL)
	{
		m_pstNvBinGradeData_HW->bIsFull = bNvIsFull;
	}*/
	m_pstNvBinGradeData->bIsFull = bNvIsFull;
	return TRUE;
} //end SetNVIsFull


CString CBinGradeData::GetRankID()
{
	return m_szRankID;
}


VOID CBinGradeData::SetRankID(CString szRankID)
{
	m_szRankID = szRankID;
}

CString CBinGradeData::GetBlockSize()
{
	return m_szBlockSize;
}

VOID CBinGradeData::SetBlockSize(CString szBlockSize)
{
	m_szBlockSize = szBlockSize;
}


/*************************/
/*   Clear Grade Info    */
/*************************/
VOID CBinGradeData::ClrGradeInfo()
{
	m_ulGradeCapacity = 0;
	m_ulInputCount = 0;
	m_ucAliasGrade = 0;
	m_pstNvBinGradeData->ulNoOfSortedDie = 0;
	m_pstNvBinGradeData->ulBlkInUse = 0;
	m_bIsAssigned = FALSE;
	m_pstNvBinGradeData->bIsFull = FALSE;
	m_ulStatRowIndex = 0;
} //end ClrGradeInfo


VOID CBinGradeData::SetLotRemainingCount(ULONG ulRemainCount)
{
	m_ulLotRemainingCount = ulRemainCount;
}

ULONG CBinGradeData::GetLotRemainingCount()
{
	return m_ulLotRemainingCount;
}

VOID CBinGradeData::SetMinFrameDieCount(ULONG ulMinFrameDieCount)
{
	m_ulMinFrameDieCount = ulMinFrameDieCount;
}

ULONG CBinGradeData::GetMinFrameDieCount()
{
	return m_ulMinFrameDieCount;
}

VOID CBinGradeData::SetMaxFrameDieCount(ULONG ulMaxFrameDieCount)
{
	m_ulMaxFrameDieCount = ulMaxFrameDieCount;
}

ULONG CBinGradeData::GetMaxFrameDieCount()
{
	return m_ulMaxFrameDieCount;
}

/***************************** Class CBinGradeData End *****************************/
