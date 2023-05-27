//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MarkDieRegion.h"
#include "io.h"
#include "WaferMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//================================================================
// Constructor / destructor implementation section
//================================================================
WM_CMarkDieRegion::WM_CMarkDieRegion()
{
	m_ulMakeDieRow		= 0;
	m_ulMakeDieCol		= 0;

	m_lMakeDieOffsetX	= 0;
	m_lMakeDieOffsetY	= 0;
}

WM_CMarkDieRegion::~WM_CMarkDieRegion()
{

}


VOID WM_CMarkDieRegion::SetMarkDieCoord(const ULONG ulMakeDieRow, const ULONG ulMakeDieCol)
{
	m_ulMakeDieRow = ulMakeDieRow;
	m_ulMakeDieCol = ulMakeDieCol;
	m_bEmptyRegion = FALSE;
	AutoUpdateMarkDieCoord();
}


VOID WM_CMarkDieRegion::AutoUpdateMarkDieCoord()
{
	WM_CWaferMap *pWaferMapManager = WM_CWaferMap::Instance();
	
	CUIntArray aulIgnoreGradeList;
	pWaferMapManager->GetSamplingRescanIgnoreGradeList(aulIgnoreGradeList);
	unsigned char ucGrade = pWaferMapManager->m_pWaferMapWrapper->GetGrade(m_ulMakeDieRow, m_ulMakeDieCol) - pWaferMapManager->m_pWaferMapWrapper->GetGradeOffset();

	if (!m_bEmptyRegion && 
		(pWaferMapManager->IsMapNullBin(m_ulMakeDieRow, m_ulMakeDieCol) || (pWaferMapManager->IsInGradeList(ucGrade, aulIgnoreGradeList) >= 0)))
	{
		LONG lShortDist = 100000000;
		ULONG ulShortRow = m_ulMakeDieRow;
		ULONG ulShortCol = m_ulMakeDieCol;
		m_bEmptyRegion = TRUE;
		for (ULONG i = m_ulUpperLeftRow; i < m_ulLowerRightRow; i++)
		{
			for (ULONG j = m_ulUpperLeftCol; j < m_ulLowerRightCol; j++)
			{
				ucGrade = pWaferMapManager->m_pWaferMapWrapper->GetGrade(i, j) - pWaferMapManager->m_pWaferMapWrapper->GetGradeOffset();
				if (pWaferMapManager->IsMapValidDie(i, j) && (pWaferMapManager->IsInGradeList(ucGrade, aulIgnoreGradeList) < 0))
				{
					if ((labs(i - m_ulMakeDieRow) * labs(i - m_ulMakeDieRow) + labs(j - m_ulMakeDieCol) * labs(j - m_ulMakeDieCol)) < lShortDist)
					{
						ulShortRow = i;
						ulShortCol = j;
					}
					m_bEmptyRegion = FALSE;
				}
			}
		}
		
		m_ulMakeDieRow = ulShortRow;
		m_ulMakeDieCol = ulShortCol;
	}
}


ULONG WM_CMarkDieRegion::GetMarkDieRow()
{
	return m_ulMakeDieRow;
}

ULONG WM_CMarkDieRegion::GetMarkDieCol()
{
	return m_ulMakeDieCol;
}


VOID WM_CMarkDieRegion::SetMarkDieOffset(LONG lMakeDieOffsetX, LONG lMakeDieOffsetY)
{
	m_lMakeDieOffsetX	= lMakeDieOffsetX;
	m_lMakeDieOffsetY	= lMakeDieOffsetY;
}

LONG WM_CMarkDieRegion::GetMarkDieOffsetX()
{
	return m_lMakeDieOffsetX;
}


LONG WM_CMarkDieRegion::GetMarkDieOffsetY()
{
	return m_lMakeDieOffsetY;
}

