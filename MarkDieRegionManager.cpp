//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MarkDieRegion.h"
#include "MarkDieRegionManager.h"
#include "MathFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

WM_CMarkDieRegionManager* WM_CMarkDieRegionManager::m_pInstance = NULL;
//================================================================
// Constructor / destructor implementation section
//================================================================
WM_CMarkDieRegionManager::WM_CMarkDieRegionManager()
{
	m_bScanRegionMarkDieCaselogEnable =  FALSE;
	m_bScanRegionMarkDie = FALSE;
	m_ulMarkDieRegionRowNum = 0;
	m_ulMarkDieRegionColNum = 0;
	m_MarkDieRegionList.RemoveAll();
}

WM_CMarkDieRegionManager::~WM_CMarkDieRegionManager()
{
	RemoveMarkDieRegionList();
}


WM_CMarkDieRegionManager* WM_CMarkDieRegionManager::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new WM_CMarkDieRegionManager();
	return m_pInstance;
}

VOID WM_CMarkDieRegionManager::ToggleScanRegionMarkDieCaseLog()
{
	m_bScanRegionMarkDieCaselogEnable = m_bScanRegionMarkDieCaselogEnable & 0x01;
	m_bScanRegionMarkDieCaselogEnable = !m_bScanRegionMarkDieCaselogEnable;
}


BOOL WM_CMarkDieRegionManager::IsEnableScanRegionMarkDieCaseLog()
{
	return m_bScanRegionMarkDieCaselogEnable;
}


VOID WM_CMarkDieRegionManager::SetScanRegionMarkDie(BOOL bScanRegionMarkDie)
{
	m_bScanRegionMarkDie = bScanRegionMarkDie;
}


BOOL WM_CMarkDieRegionManager::IsScanRegionMarkDie()
{
	return m_bScanRegionMarkDie;
}


VOID WM_CMarkDieRegionManager::RemoveMarkDieRegionList()
{
	for (LONG i = 0; i < m_MarkDieRegionList.GetSize(); i++)
	{
		WM_CMarkDieRegion *pMarkDieRegion = GetMarkDieRegion(i);
		if (pMarkDieRegion != NULL)
		{
			delete pMarkDieRegion;
		}
	}
	m_MarkDieRegionList.RemoveAll();
}


WM_CMarkDieRegion *WM_CMarkDieRegionManager::GetMarkDieRegion(LONG lPos)
{
	WM_CMarkDieRegion *pMarkDieRegion = NULL;
	if (lPos < m_MarkDieRegionList.GetSize())
	{
		pMarkDieRegion = (WM_CMarkDieRegion*)m_MarkDieRegionList[lPos];
	}
	return pMarkDieRegion;
}

LONG WM_CMarkDieRegionManager::_round(double val)
{
	return CMathFunc::Instance()->_round(val);
}

VOID WM_CMarkDieRegionManager::DefineMarkDieRegion(const ULONG ulMaxRow, const ULONG ulMaxCol)
{
	//Define Mark Die region
	WM_CMarkDieRegion *pMarkDieRegion = NULL;
	LONG lRegionSize = MARK_DIE_REGION_SZIE;
	m_ulMarkDieRegionRowNum = _round((double)ulMaxRow / (double)lRegionSize);
	m_ulMarkDieRegionColNum = _round((double)ulMaxCol / (double)lRegionSize);
	if ((lRegionSize * m_ulMarkDieRegionRowNum) < ulMaxRow)
	{
		m_ulMarkDieRegionRowNum++;
	}
	if ((lRegionSize * m_ulMarkDieRegionColNum) < ulMaxCol)
	{
		m_ulMarkDieRegionColNum++;
	}

	LONG lRowsInMarkDieRegion = lRegionSize;
	LONG lColsInMarkDieRegion = lRegionSize;

	LONG lMarkDieRegionNum = 0;
	ULONG ulRegionNum = 0;

	m_MarkDieRegionList.RemoveAll();
	m_MarkDieRegionList.SetSize(m_ulMarkDieRegionRowNum * m_ulMarkDieRegionColNum + 1);
	for (ULONG ulRow = 0; ulRow < m_ulMarkDieRegionRowNum; ulRow++)
	{
		for (ULONG ulCol = 0; ulCol < m_ulMarkDieRegionColNum; ulCol++)
		{
			ulRegionNum =  ulRow * m_ulMarkDieRegionColNum + ulCol;
			pMarkDieRegion = new WM_CMarkDieRegion();
			pMarkDieRegion->SetRegion(ulRow * lRowsInMarkDieRegion,
									  ulCol * lColsInMarkDieRegion,
									  ulRow * lRowsInMarkDieRegion + lRowsInMarkDieRegion,
									  ulCol * lColsInMarkDieRegion + lColsInMarkDieRegion);
			ULONG ulMakeDieRow = ulRow * lRowsInMarkDieRegion + lRowsInMarkDieRegion / 2;
			ULONG ulMakeDieCol = ulCol * lColsInMarkDieRegion + lColsInMarkDieRegion / 2;
			pMarkDieRegion->SetMarkDieCoord(ulMakeDieRow, ulMakeDieCol);
			pMarkDieRegion->SetRegionNo(ulRegionNum, ulRow, ulCol);
			m_MarkDieRegionList[ulRegionNum] = pMarkDieRegion;
		}
	}
}


VOID WM_CMarkDieRegionManager::AutoUpdateMarkDieCoord()
{
	for (ULONG ulRow = 0; ulRow < m_ulMarkDieRegionRowNum; ulRow++)
	{
		for (ULONG ulCol = 0; ulCol < m_ulMarkDieRegionColNum; ulCol++)
		{
			LONG ulRegionNum =  ulRow * m_ulMarkDieRegionColNum + ulCol;
			WM_CMarkDieRegion *pMarkDieRegion = GetMarkDieRegion(ulRegionNum);
			if (pMarkDieRegion != NULL)
			{
				pMarkDieRegion->AutoUpdateMarkDieCoord();
			}
		}
	}
}


ULONG WM_CMarkDieRegionManager::GetMarkDieRegionRowNum()
{
	return m_ulMarkDieRegionRowNum;
}


ULONG WM_CMarkDieRegionManager::GetMarkDieRegionColNum()
{
	return m_ulMarkDieRegionColNum;
}


BOOL WM_CMarkDieRegionManager::SubGetMarkDieRegionPRInfo(const LONG lRegionNo, ULONG &ulMarkDieRow, ULONG &ulMarkDieCol, LONG &lMarkDieOffsetX, LONG &lMarkDieOffsetY)
{
	if (lRegionNo < 0)
	{
		return FALSE;
	}

	WM_CMarkDieRegion *pMarkDieRegion = GetMarkDieRegion(lRegionNo);

	if (pMarkDieRegion == NULL)
	{
		return FALSE;
	}

	ulMarkDieRow	= pMarkDieRegion->GetMarkDieRow();
	ulMarkDieCol	= pMarkDieRegion->GetMarkDieCol();

	lMarkDieOffsetX	= pMarkDieRegion->GetMarkDieOffsetX();
	lMarkDieOffsetY	= pMarkDieRegion->GetMarkDieOffsetY();

	return TRUE;
}


BOOL WM_CMarkDieRegionManager::GetMarkDieRegionPRInfo(const LONG lRegionNo, ULONG &ulMarkDieRow, ULONG &ulMarkDieCol, LONG &lMarkDieOffsetX, LONG &lMarkDieOffsetY)
{
	BOOL bRegionExist = TRUE;
	ulMarkDieRow = 0;
	ulMarkDieCol = 0;
	lMarkDieOffsetX = 0;
	lMarkDieOffsetY = 0;
	if (!((lRegionNo >= 0) && SubGetMarkDieRegionPRInfo(lRegionNo, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY)))
	{
		bRegionExist = FALSE;
	}

	return bRegionExist;
}


//===============================================
//    2           1
//        center 
//    3(Current)  4 
//===============================================
VOID WM_CMarkDieRegionManager::GetUpperRightCornerPRInfoOfMarkDieRegion(ULONG ulRegionNum, double &dResultRow, double &dResultCol, double &dResultOffsetX, double &dResultOffsetY)
{
	WM_CMarkDieRegion *pMarkDieRegion = GetMarkDieRegion(ulRegionNum);
	ULONG ulUpperRightRow = 0, ulUpperRightCol = 0;
	pMarkDieRegion->GetRegionUpperRight(ulUpperRightRow, ulUpperRightCol);
	dResultRow = ulUpperRightRow;
	dResultCol = ulUpperRightCol;
	//===============================================
	//    2           1
	//        center 
	//    3(Current)  4 
	//===============================================
	double dDieRow[4], dDieCol[4];
	double dDiePROffsetX[4], dDiePROffsetY[4];

	//Current Region
	ULONG ulMarkDieRow = 0, ulMarkDieCol = 0;
	LONG lMarkDieOffsetX = 0, lMarkDieOffsetY = 0;

	GetMarkDieRegionPRInfo(ulRegionNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	dDieRow[LOWER_LEFT] = ulMarkDieRow; 
	dDieCol[LOWER_LEFT] = ulMarkDieCol;
	dDiePROffsetX[LOWER_LEFT] = lMarkDieOffsetX; 
	dDiePROffsetY[LOWER_LEFT] = lMarkDieOffsetY;

	//Lower Right Region
	BOOL bLeftRegionExist = FALSE;
	if (pMarkDieRegion->GetRegionCol() < GetMarkDieRegionColNum() - 1)
	{
		bLeftRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum + 1, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bLeftRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[LOWER_LEFT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[LOWER_LEFT];
		ulMarkDieRow = (LONG)dDieRow[LOWER_LEFT];
		ulMarkDieCol = ulUpperRightCol;
	}
	dDieRow[LOWER_RIGHT] = ulMarkDieRow; 
	dDieCol[LOWER_RIGHT] = ulMarkDieCol;
	dDiePROffsetX[LOWER_RIGHT] = lMarkDieOffsetX; 
	dDiePROffsetY[LOWER_RIGHT] = lMarkDieOffsetY;

	//Upper-Left Region
	BOOL bUpperLeftRegionExist = FALSE;
	if (pMarkDieRegion->GetRegionRow() > 0)
	{
		bUpperLeftRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum - (LONG)m_ulMarkDieRegionColNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bUpperLeftRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[LOWER_LEFT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[LOWER_LEFT];
		ulMarkDieRow = ulUpperRightRow;
		ulMarkDieCol = (LONG)dDieCol[LOWER_LEFT];
	}
	dDieRow[UPPER_LEFT] = ulMarkDieRow; 
	dDieCol[UPPER_LEFT] = ulMarkDieCol;
	dDiePROffsetX[UPPER_LEFT] = lMarkDieOffsetX; 
	dDiePROffsetY[UPPER_LEFT] = lMarkDieOffsetY;

	//Upper Right Region
	BOOL bUpperRegionExist = FALSE;
	if ((pMarkDieRegion->GetRegionRow() > 0) && (pMarkDieRegion->GetRegionCol() < GetMarkDieRegionColNum() - 1))
	{
		bUpperRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum - (LONG)m_ulMarkDieRegionColNum + 1, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bUpperRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[UPPER_LEFT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[UPPER_LEFT];
		ulMarkDieRow = ulUpperRightRow;
		ulMarkDieCol = ulUpperRightCol;
	}
	dDieRow[UPPER_RIGHT] = ulMarkDieRow; 
	dDieCol[UPPER_RIGHT] = ulMarkDieCol;
	dDiePROffsetX[UPPER_RIGHT] = lMarkDieOffsetX; 
	dDiePROffsetY[UPPER_RIGHT] = lMarkDieOffsetY;


	dResultOffsetX = 0;
	dResultOffsetY = 0;
	CalFourPointInterpolation(dDieRow, dDieCol, dDiePROffsetX, dDiePROffsetY, ulUpperRightRow, ulUpperRightCol, dResultOffsetX, dResultOffsetY);
}

//===============================================
//    2          1
//       center 
//    3          4(Current) 
//===============================================
VOID WM_CMarkDieRegionManager::GetUpperLeftCornerPRInfoOfMarkDieRegion(ULONG ulRegionNum, double &dResultRow, double &dResultCol, double &dResultOffsetX, double &dResultOffsetY)
{
	WM_CMarkDieRegion *pMarkDieRegion = GetMarkDieRegion(ulRegionNum);
	ULONG ulUpperLeftRow = 0, ulUpperLeftCol = 0;
	pMarkDieRegion->GetRegionUpperLeft(ulUpperLeftRow, ulUpperLeftCol);
	dResultRow = ulUpperLeftRow;
	dResultCol = ulUpperLeftCol;
//===============================================
//    2          1 
//       center 
//    3          4(Current)
//===============================================
	double dDieRow[4], dDieCol[4];
	double dDiePROffsetX[4], dDiePROffsetY[4];

	//Current Region
	ULONG ulMarkDieRow = 0, ulMarkDieCol = 0;
	LONG lMarkDieOffsetX = 0, lMarkDieOffsetY = 0;

	GetMarkDieRegionPRInfo(ulRegionNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	dDieRow[LOWER_RIGHT] = ulMarkDieRow; 
	dDieCol[LOWER_RIGHT] = ulMarkDieCol;
	dDiePROffsetX[LOWER_RIGHT] = lMarkDieOffsetX; 
	dDiePROffsetY[LOWER_RIGHT] = lMarkDieOffsetY;

	//Lower Left Region
	BOOL bLeftRegionExist = FALSE;
	if (pMarkDieRegion->GetRegionCol() > 0)
	{
		bLeftRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum - 1, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bLeftRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[LOWER_RIGHT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[LOWER_RIGHT];
		ulMarkDieRow = (LONG)dDieRow[LOWER_RIGHT];
		ulMarkDieCol = ulUpperLeftCol;
	}
	dDieRow[LOWER_LEFT] = ulMarkDieRow; 
	dDieCol[LOWER_LEFT] = ulMarkDieCol;
	dDiePROffsetX[LOWER_LEFT] = lMarkDieOffsetX; 
	dDiePROffsetY[LOWER_LEFT] = lMarkDieOffsetY;

	//Upper-Left Region
	BOOL bUpperLeftRegionExist = FALSE;
	if ((pMarkDieRegion->GetRegionCol() > 0) && (pMarkDieRegion->GetRegionRow() > 0))
	{
		bUpperLeftRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum - 1 - (LONG)m_ulMarkDieRegionColNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bUpperLeftRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[LOWER_LEFT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[LOWER_LEFT];
		ulMarkDieRow = ulUpperLeftRow;
		ulMarkDieCol = ulUpperLeftCol;
	}
	dDieRow[UPPER_LEFT] = ulMarkDieRow; 
	dDieCol[UPPER_LEFT] = ulMarkDieCol;
	dDiePROffsetX[UPPER_LEFT] = lMarkDieOffsetX; 
	dDiePROffsetY[UPPER_LEFT] = lMarkDieOffsetY;

	//Upper Right Region
	BOOL bUpperRegionExist = FALSE;
	if (pMarkDieRegion->GetRegionRow() > 0)
	{
		bUpperRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum - (LONG)m_ulMarkDieRegionColNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bUpperRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[LOWER_RIGHT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[LOWER_RIGHT];
		ulMarkDieRow = ulUpperLeftRow;
		ulMarkDieCol = (LONG)dDieCol[LOWER_RIGHT];
	}
	dDieRow[UPPER_RIGHT] = ulMarkDieRow; 
	dDieCol[UPPER_RIGHT] = ulMarkDieCol;
	dDiePROffsetX[UPPER_RIGHT] = lMarkDieOffsetX; 
	dDiePROffsetY[UPPER_RIGHT] = lMarkDieOffsetY;

	dResultOffsetX = 0;
	dResultOffsetY = 0;
	CalFourPointInterpolation(dDieRow, dDieCol, dDiePROffsetX, dDiePROffsetY, ulUpperLeftRow, ulUpperLeftCol, dResultOffsetX, dResultOffsetY);
}


//===============================================
//    2          1(Current)
//       center 
//    3          4 
//===============================================
VOID WM_CMarkDieRegionManager::GetLowerLeftCornerPRInfoOfMarkDieRegion(ULONG ulRegionNum, double &dResultRow, double &dResultCol, double &dResultOffsetX, double &dResultOffsetY)
{
	WM_CMarkDieRegion *pMarkDieRegion = GetMarkDieRegion(ulRegionNum);
	ULONG ulLowerLeftRow = 0, ulLowerLeftCol = 0;
	pMarkDieRegion->GetRegionLowerLeft(ulLowerLeftRow, ulLowerLeftCol);
	dResultRow = ulLowerLeftRow;
	dResultCol = ulLowerLeftCol;
//===============================================
//    2          1(Current)
//       center 
//    3          4
//===============================================
	double dDieRow[4], dDieCol[4];
	double dDiePROffsetX[4], dDiePROffsetY[4];

	//Current Region
	ULONG ulMarkDieRow = 0, ulMarkDieCol = 0;
	LONG lMarkDieOffsetX = 0, lMarkDieOffsetY = 0;

	GetMarkDieRegionPRInfo(ulRegionNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	dDieRow[UPPER_RIGHT] = ulMarkDieRow; 
	dDieCol[UPPER_RIGHT] = ulMarkDieCol;
	dDiePROffsetX[UPPER_RIGHT] = lMarkDieOffsetX; 
	dDiePROffsetY[UPPER_RIGHT] = lMarkDieOffsetY;

	//Upper Left Region
	BOOL bUpperLeftRegionExist = FALSE;
	if (pMarkDieRegion->GetRegionCol() > 0)
	{
		bUpperLeftRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum - 1, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bUpperLeftRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[UPPER_RIGHT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[UPPER_RIGHT];
		ulMarkDieRow = (LONG)dDieRow[UPPER_RIGHT];
		ulMarkDieCol = ulLowerLeftCol;
	}
	dDieRow[UPPER_LEFT] = ulMarkDieRow; 
	dDieCol[UPPER_LEFT] = ulMarkDieCol;
	dDiePROffsetX[UPPER_LEFT] = lMarkDieOffsetX; 
	dDiePROffsetY[UPPER_LEFT] = lMarkDieOffsetY;

	//Lower-Left Region
	BOOL bLowerLeftRegionExist = FALSE;
	if ((pMarkDieRegion->GetRegionRow() < GetMarkDieRegionRowNum() - 1) && (pMarkDieRegion->GetRegionCol() > 0))
	{
		bLowerLeftRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum - 1 + m_ulMarkDieRegionColNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bLowerLeftRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[UPPER_LEFT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[UPPER_LEFT];
		ulMarkDieRow = ulLowerLeftRow;
		ulMarkDieCol = ulLowerLeftCol;
	}
	dDieRow[LOWER_LEFT] = ulMarkDieRow; 
	dDieCol[LOWER_LEFT] = ulMarkDieCol;
	dDiePROffsetX[LOWER_LEFT] = lMarkDieOffsetX; 
	dDiePROffsetY[LOWER_LEFT] = lMarkDieOffsetY;

	//Lower Right Region
	BOOL bLowerRegionExist = FALSE;
	if (pMarkDieRegion->GetRegionRow() < GetMarkDieRegionRowNum() - 1)
	{
		bLowerRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum + m_ulMarkDieRegionColNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bLowerRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[UPPER_RIGHT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[UPPER_RIGHT];
		ulMarkDieRow = ulLowerLeftRow;
		ulMarkDieCol = (LONG)dDieCol[UPPER_RIGHT];
	}
	dDieRow[LOWER_RIGHT] = ulMarkDieRow; 
	dDieCol[LOWER_RIGHT] = ulMarkDieCol;
	dDiePROffsetX[LOWER_RIGHT] = lMarkDieOffsetX; 
	dDiePROffsetY[LOWER_RIGHT] = lMarkDieOffsetY;

	dResultOffsetX = 0;
	dResultOffsetY = 0;
	CalFourPointInterpolation(dDieRow, dDieCol, dDiePROffsetX, dDiePROffsetY, ulLowerLeftRow, ulLowerLeftCol, dResultOffsetX, dResultOffsetY);
}

//===============================================
//    2(Current) 1
//       center 
//    3          4 
//===============================================
VOID WM_CMarkDieRegionManager::GetLowerRightCornerPRInfoOfMarkDieRegion(ULONG ulRegionNum, double &dResultRow, double &dResultCol, double &dResultOffsetX, double &dResultOffsetY)
{
	WM_CMarkDieRegion *pMarkDieRegion = GetMarkDieRegion(ulRegionNum);
	ULONG ulLowerRightRow = 0, ulLowerRightCol = 0;
	if (pMarkDieRegion != NULL)
	{
		pMarkDieRegion->GetRegionLowerRight(ulLowerRightRow, ulLowerRightCol);
	}
	dResultRow = ulLowerRightRow;
	dResultCol = ulLowerRightCol;
//===============================================
//    2(Current) 1
//       center 
//    3          4
//===============================================
	double dDieRow[4], dDieCol[4];
	double dDiePROffsetX[4], dDiePROffsetY[4];

	//Current Region
	ULONG ulMarkDieRow = 0, ulMarkDieCol = 0;
	LONG lMarkDieOffsetX = 0, lMarkDieOffsetY = 0;

	GetMarkDieRegionPRInfo(ulRegionNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	dDieRow[UPPER_LEFT] = ulMarkDieRow; 
	dDieCol[UPPER_LEFT] = ulMarkDieCol;
	dDiePROffsetX[UPPER_LEFT] = lMarkDieOffsetX; 
	dDiePROffsetY[UPPER_LEFT] = lMarkDieOffsetY;

	//Upper Right Region
	BOOL bUpperRightRegionExist = FALSE;
	if (pMarkDieRegion->GetRegionCol() < GetMarkDieRegionColNum() - 1)
	{
		bUpperRightRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum + 1, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bUpperRightRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[UPPER_LEFT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[UPPER_LEFT];
		ulMarkDieRow = (LONG)dDieRow[UPPER_LEFT];
		ulMarkDieCol = ulLowerRightCol;
	}
	dDieRow[UPPER_RIGHT] = ulMarkDieRow; 
	dDieCol[UPPER_RIGHT] = ulMarkDieCol;
	dDiePROffsetX[UPPER_RIGHT] = lMarkDieOffsetX; 
	dDiePROffsetY[UPPER_RIGHT] = lMarkDieOffsetY;

	//Lower-Right Region
	BOOL bLowerRightRegionExist = FALSE;
	if ((pMarkDieRegion->GetRegionRow() < GetMarkDieRegionRowNum() - 1) && (pMarkDieRegion->GetRegionCol() < GetMarkDieRegionColNum() - 1))
	{
		bLowerRightRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum + 1 + m_ulMarkDieRegionColNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bLowerRightRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[UPPER_RIGHT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[UPPER_RIGHT];
		ulMarkDieRow = ulLowerRightRow;
		ulMarkDieCol = ulLowerRightCol;
	}
	dDieRow[LOWER_RIGHT] = ulMarkDieRow; 
	dDieCol[LOWER_RIGHT] = ulMarkDieCol;
	dDiePROffsetX[LOWER_RIGHT] = lMarkDieOffsetX; 
	dDiePROffsetY[LOWER_RIGHT] = lMarkDieOffsetY;

	//Lower Left Region
	BOOL bLowerLeftRegionExist = FALSE;
	if (pMarkDieRegion->GetRegionRow() < GetMarkDieRegionRowNum() - 1)
	{
		bLowerLeftRegionExist = GetMarkDieRegionPRInfo((LONG)ulRegionNum + m_ulMarkDieRegionColNum, ulMarkDieRow, ulMarkDieCol, lMarkDieOffsetX, lMarkDieOffsetY);
	}
	if (!bLowerLeftRegionExist)
	{
		lMarkDieOffsetX = (LONG)dDiePROffsetX[UPPER_LEFT];
		lMarkDieOffsetY = (LONG)dDiePROffsetY[UPPER_LEFT];
		ulMarkDieRow = ulLowerRightRow;
		ulMarkDieCol = (LONG)dDieCol[UPPER_LEFT];
	}
	dDieRow[LOWER_LEFT] = ulMarkDieRow; 
	dDieCol[LOWER_LEFT] = ulMarkDieCol;
	dDiePROffsetX[LOWER_LEFT] = lMarkDieOffsetX; 
	dDiePROffsetY[LOWER_LEFT] = lMarkDieOffsetY;

	dResultOffsetX = 0;
	dResultOffsetY = 0;
	CalFourPointInterpolation(dDieRow, dDieCol, dDiePROffsetX, dDiePROffsetY, ulLowerRightRow, ulLowerRightCol, dResultOffsetX, dResultOffsetY);
}


VOID WM_CMarkDieRegionManager::CalFourPointInterpolation(const double dDieRow[4], const double dDieCol[4], const double dDiePROffsetX[4], const double dDiePROffsetY[4],
														 const ULONG ulCenterRow, const ULONG ulCenterCol, double &dCenterResultOffsetX, double &dCenterResultOffsetY)
{
	//vertical 
	//Left
	double dRatio = 0;
	if (fabs(dDieRow[LOWER_LEFT] - dDieRow[UPPER_LEFT]) > 0.1)
	{
		dRatio = (double)(ulCenterRow - dDieRow[UPPER_LEFT]) / (double)(dDieRow[LOWER_LEFT] - dDieRow[UPPER_LEFT]);
	}
	double dMarkDieResultOffsetX1 = dDiePROffsetX[UPPER_LEFT] + (dDiePROffsetX[LOWER_LEFT] -  dDiePROffsetX[UPPER_LEFT]) * dRatio;
	double dMarkDieResultOffsetY1 = dDiePROffsetY[UPPER_LEFT] + (dDiePROffsetY[LOWER_LEFT] -  dDiePROffsetY[UPPER_LEFT]) * dRatio;
	double dCol1 = dDieCol[UPPER_LEFT] + (dDieCol[LOWER_LEFT] - dDieCol[UPPER_LEFT]) * dRatio;

	//Right
	dRatio = 0;
	if (fabs(dDieRow[LOWER_RIGHT] - dDieRow[UPPER_RIGHT]) > 0.1)
	{
		dRatio = (double)(ulCenterRow - dDieRow[UPPER_RIGHT]) / (double)(dDieRow[LOWER_RIGHT] - dDieRow[UPPER_RIGHT]);
	}
	double dMarkDieResultOffsetX2 = dDiePROffsetX[UPPER_RIGHT] + (dDiePROffsetX[LOWER_RIGHT] - dDiePROffsetX[UPPER_RIGHT]) * dRatio;
	double dMarkDieResultOffsetY2 = dDiePROffsetY[UPPER_RIGHT] + (dDiePROffsetY[LOWER_RIGHT] - dDiePROffsetY[UPPER_RIGHT]) * dRatio;
	double dCol2 = dDieCol[UPPER_RIGHT] + (dDieCol[LOWER_RIGHT] - dDieCol[UPPER_RIGHT]) * dRatio;

	//Horizontal
	dRatio = 0;
	if (fabs(dCol2 - dCol1) > 0.1)
	{
		dRatio = (double)(ulCenterCol - dCol1) / (double)(dCol2 - dCol1);
	}
	dCenterResultOffsetX = dMarkDieResultOffsetX1 + (dMarkDieResultOffsetX2 - dMarkDieResultOffsetX1) * dRatio;
	dCenterResultOffsetY = dMarkDieResultOffsetY1 + (dMarkDieResultOffsetY2 - dMarkDieResultOffsetY1) * dRatio;
}
