//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "WT_1stDieFinder.h"
#include "math.h"
#include "MS896A.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CWT_1stDieFinder::CWT_1stDieFinder()
{
	m_pWaferTableStn	= NULL;

	InitFinderData();

	// must NOT in the init data function
	m_ulTgtRegionUlRow = 0;
	m_ulTgtRegionUlCol = 0;
	m_ulTgtRegionLrRow = 0;
	m_ulTgtRegionLrCol = 0;
}

VOID CWT_1stDieFinder::InitFinderData()
{
	m_ulCurrRow			= 0;
	m_ulCurrCol			= 0;

	m_szBpAlgorithm		= "";
	m_szBpPathFinder	= "";
	m_ulBpDieStep		= 1;
	m_eBpPickMode		= WAF_CDieSelectionAlgorithm::MIXED_ORDER;
	m_bInit				= FALSE;

	m_ulEndAreaULRow	= 0;
	m_ulEndAreaULCol	= 0;
	m_ulEndAreaLRRow	= 0;
	m_ulEndAreaLRCol	= 0;
}

CWT_1stDieFinder::~CWT_1stDieFinder()
{
}

VOID CWT_1stDieFinder::SetWaferTable(CWaferTable* pWaferTable)
{
	m_pWaferTableStn = pWaferTable;
}

//================================================================
// GET Function implementation section
//================================================================

BOOL CWT_1stDieFinder::IsInit()
{
	return m_bInit;
}

BOOL CWT_1stDieFinder::GetLastPosn(ULONG& ulRow, ULONG& ulCol)
{
	ulRow	= GetCurRow();
	ulCol	= GetCurCol();
	return TRUE;
}

//================================================================
// Protected Member Function implementation section
//================================================================

BOOL CWT_1stDieFinder::StartFinder(ULONG ulStartRow, ULONG ulStartCol, DOUBLE dFOVX, DOUBLE dFOVY)
{
	InitFinderData();

	//Get current START position from map
	m_ulCurrRow = ulStartRow;
	m_ulCurrCol = ulStartCol;

	CString szMsg;
	CWaferMapWrapper* pWrapper = &m_pWaferTableStn->m_WaferMapWrapper;

	INT nColSize, nRowSize;

	if ((dFOVX < WPR_LF_SIZE) && (dFOVY < WPR_LF_SIZE))
	{
		szMsg.Format("PR FOV (%.2f,%.2f) less than %.2f, Please stop operation", dFOVX, dFOVY, WPR_LF_SIZE);
		AfxMessageBox(szMsg, MB_SYSTEMMODAL);
		return FALSE;
	}
	dFOVX = dFOVX - 1.0;
	dFOVY = dFOVY - 1.0;

	nColSize = (INT) ceil(dFOVX);
	nRowSize = (INT) ceil(dFOVY);
	if( nColSize%2==0 )
		nColSize--;
	if( nRowSize%2==0 )
		nRowSize--;

	// Locate target die in target region to PICK on this wafer
	ULONG i=0, j=0;
	BOOL bDieFound = FALSE;

	for(i=m_ulTgtRegionUlRow; i<m_ulTgtRegionLrRow; i++)
	{
		for(j=m_ulTgtRegionUlCol; j<m_ulTgtRegionLrCol; j++)
		{
			if (m_pWaferTableStn->m_pWaferMapManager->IsMapHaveBin(i, j) )
			{
				bDieFound = TRUE;
				break;
			}
		}

		if (bDieFound)
			break;
	}

	if (!bDieFound)
	{
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		pUtl->RegionOrderLog("To find target region target die area fail");
		return FALSE;
	}

	m_ulEndAreaULRow	= i;
	m_ulEndAreaULCol	= j;
	m_ulEndAreaLRRow	= i;
	m_ulEndAreaLRCol	= j;

	bDieFound = FALSE;
	for(i=m_ulTgtRegionLrRow-1; i>m_ulTgtRegionUlRow; i--)
	{
		for(j=m_ulTgtRegionLrCol-1; j>m_ulTgtRegionUlCol; j--)
		{
			if( m_pWaferTableStn->m_pWaferMapManager->IsMapHaveBin(i, j) )
			{
				bDieFound = TRUE;
				break;
			}
		}

		if (bDieFound)
			break;
	}

	if (bDieFound)
	{
		m_ulEndAreaLRRow	= i;
		m_ulEndAreaLRCol	= j;
	}


	//Backup current MAP settings
	pWrapper->GetAlgorithm(m_szBpAlgorithm, m_szBpPathFinder);
	m_ulBpDieStep = pWrapper->GetDieStep();
	m_eBpPickMode = pWrapper->GetPickMode();

	pWrapper->SelectAlgorithm("DeepSearch",	"Direct");

	//Specify target region in MAP library
	pWrapper->SetAlgorithmParameter("Search Start Row",		m_ulEndAreaULRow);
	pWrapper->SetAlgorithmParameter("Search Start Col",		m_ulEndAreaULCol);
	pWrapper->SetAlgorithmParameter("Search End Row",		m_ulEndAreaLRRow);
	pWrapper->SetAlgorithmParameter("Search End Col",		m_ulEndAreaLRCol);

	pWrapper->SetAlgorithmParameter("Search Region Row Size",	nRowSize);
	pWrapper->SetAlgorithmParameter("Search Region Col Size",	nColSize);

	m_bInit = TRUE;

	return TRUE;
}

BOOL CWT_1stDieFinder::RestoreFinderAlgorithm()
{
	CWaferMapWrapper* pWrapper = &m_pWaferTableStn->m_WaferMapWrapper;

	if (m_szBpAlgorithm.GetLength() <= 0)
		return FALSE;
	if (m_szBpPathFinder.GetLength() <= 0)
		return FALSE;

	//Restore Backup map settings
	pWrapper->SelectAlgorithm(m_szBpAlgorithm, m_szBpPathFinder);
	pWrapper->SetDieStep(m_ulBpDieStep);
	pWrapper->SetPickMode(m_eBpPickMode);

	return TRUE;
}


//================================================================
//================================================================
INT CWT_1stDieFinder::WT_LFNextDie(BOOL bLatch)
{
	CWaferMapWrapper* pWrapper = &m_pWaferTableStn->m_WaferMapWrapper;

    WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
	ULONG ulRow=0, ulCol=0;
	UCHAR ucGrade=0;
	BOOL bEndOfWafer=FALSE;	

	//Get NEXT-DIE position from MAP
	m_pWaferTableStn->PeekMapDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
	ULONG ulState = pWrapper->GetDieState(ulRow, ulCol);
	//Detect WAFER_END error
	if (bEndOfWafer)
	{
		return ERR_1DF__WAFEREND;
	}

	//Check if new die die-state has been set before by LF, and get next one instead
	while( !bEndOfWafer &&
			((ulState == WT_MAP_DIESTATE_DEFECT)	|| 
			(ulState == WT_MAP_DIESTATE_INK)		||
			(ulState == WT_MAP_DIESTATE_CHIP)		||
			(ulState == WT_MAP_DIESTATE_BADCUT)		||
			 m_pWaferTableStn->IsDieEmpty(ulState)	||
			(ulState == WT_MAP_DIESTATE_ROTATE)		||
			(ulState == WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY) ) )
	{
		m_pWaferTableStn->GetMapDie(ulRow, ulCol, ucGrade, eAction, bEndOfWafer);

		m_pWaferTableStn->UpdateMapDie(ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
		
		//Peek another new die after updating curr die state
		m_pWaferTableStn->PeekMapDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);	
		ulState = pWrapper->GetDieState(ulRow, ulCol);
	}

	//Detect WAFER_END error
	if (bEndOfWafer)
	{
		return ERR_1DF__WAFEREND;
	}

	//Update die status on MAP
	m_pWaferTableStn->GetMapDie(ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
	m_pWaferTableStn->UpdateMapDie(ulRow, ulCol, ucGrade, eAction, bEndOfWafer);

	m_pWaferTableStn->LookAroundPosn(ulRow, ulCol, GetCurRow(), GetCurCol(), bLatch);

	// Update CURRENT posn if OK
	m_ulCurrRow = ulRow;
	m_ulCurrCol = ulCol;

	return ERR_1DF__OK;
}



ULONG CWT_1stDieFinder::GetCurRow()
{
	return m_ulCurrRow;
}

ULONG CWT_1stDieFinder::GetCurCol()
{
	return m_ulCurrCol;
}

//================================================================
// Public Member Function implementation section
//================================================================
//================================================================
//================================================================


VOID CWT_1stDieFinder::StopFinder()
{
	RestoreFinderAlgorithm();
	m_bInit = FALSE;
}


//================================================================
//================================================================
INT CWT_1stDieFinder::FindNextDie()
{
	//Continue looking for next LF die until a die is found
	INT nLFDieStatus = WT_LFNextDie(TRUE);

	//v4.04		//Klocwork
	if( nLFDieStatus == ERR_1DF__WAFEREND )
	{
		return ERR_1DF__WAFEREND;				//QUit if Wafer-End is found
	}
/*
	while (nLFDieStatus != ERR_1DF__OK)
	{
		if( nLFDieStatus==ERR_1DF__WAFEREND )
		{
			return ERR_1DF__WAFEREND;				//QUit if Wafer-End is found
		}

		nLFDieStatus = WT_LFNextDie(FALSE);	//If curr posn no die, try to locate next die in same image
	}
*/

	//Check if Target END region/pt already within END region and stop, Path-Finder DONE!!
	if( (GetCurRow() >= m_ulEndAreaULRow) && 
		(GetCurRow() <= m_ulEndAreaLRRow) &&
		(GetCurCol() >= m_ulEndAreaULCol) &&
		(GetCurCol() <= m_ulEndAreaLRCol) )
	{
		return ERR_1DF__DONE ;
	}

	return ERR_1DF__OK;
}

VOID CWT_1stDieFinder::SetTargetSubRegion(ULONG ulUlRow, ULONG ulUlCol, ULONG ulLrRow, ULONG ulLrCol)
{
	m_ulTgtRegionUlRow = ulUlRow;
	m_ulTgtRegionUlCol = ulUlCol;
	m_ulTgtRegionLrRow = ulLrRow;
	m_ulTgtRegionLrCol = ulLrCol;
}

//================================================================
// End of WT_1stDieFinder.cpp
//================================================================

//================================================================
//================================================================
	// ROW & COL coordinate
	//
	// (1,1)
	//   +-------> Col
	//   |
	//   |
	//   |
	//   v
	//  Row

	// 3x3 region index
	//=================
	//  1  2  3
	//  4  5  6
	//  7  8  9

	// 5x3 region index
	//=================
	//  1   2   3   4   5
	//  6   7   8   9  10
	// 11  12  13  14  15

	// 5x5 region index
	//=================
	//  1   2   3   4   5
	//  6   7   8   9  10
	// 11  12  13  14  15
	// 16  17  18  19  20
	// 21  22  23  24  25
