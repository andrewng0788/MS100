//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "EjPinCleanRegion.h"
#include "io.h"
#include "WaferMap.h"
#include "MathFunc.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//================================================================
// Constructor / destructor implementation section
//================================================================
CEjectorPinCleanRegion::CEjectorPinCleanRegion()
{
	m_lCleanDirtMatrixRow = 1;
	m_lCleanDirtMatrixCol = 1;
	m_lCleanDirtUpleftPosX = 0;
	m_lCleanDirtUpleftPosY = 0;
	m_lCleanDirtLowerRightPosX = 0;
	m_lCleanDirtLowerRightPosY = 0;
	m_lCleanDirtRowIndex = 0;
	m_lCleanDirtColIndex = 0;

	m_lCleanDirtPinUpLevel = 200;
	m_lCleanDirtCycleAreaCount = 1;
	m_lCleanDirtCycleEjUpCount = 5;
	m_lCleanDirtSamplingCount = 10000;

	m_lCleanDirtEJPinEnable = FALSE;

	m_lCleanDirtSamplingIndex = 0;
}

CEjectorPinCleanRegion::~CEjectorPinCleanRegion()
{

}


VOID CEjectorPinCleanRegion::LoadData( CStringMapFile *psmf)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	m_lCleanDirtMatrixRow		= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_MATRIX_ROW];
	m_lCleanDirtMatrixCol		= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_MATRIX_COL];
	if (pApp->IsPortablePKGFile() == FALSE)
	{
		m_lCleanDirtUpleftPosX		= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_UP_LEFT_X];
		m_lCleanDirtUpleftPosY		= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_UP_LEFT_Y];
		m_lCleanDirtLowerRightPosX	= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_LOWER_RIGHT_X];
		m_lCleanDirtLowerRightPosY	= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_LOWER_RIGHT_Y];
	}
	m_lCleanDirtPinUpLevel		= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_EJ_PIN_UP_LEVEL];
	m_lCleanDirtCycleAreaCount	= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_CYCLE_AREA_COUNT];
	m_lCleanDirtCycleEjUpCount	= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_CYCLE_EJ_UP_COUNT];
	m_lCleanDirtSamplingCount	= (LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_SAMPLING_COUNT];

	m_lCleanDirtEJPinEnable		= (BOOL)(LONG)(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_ENABLE];
}

VOID CEjectorPinCleanRegion::SaveData( CStringMapFile *psmf)
{
	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_MATRIX_ROW]	= m_lCleanDirtMatrixRow;
	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_MATRIX_COL] = m_lCleanDirtMatrixCol;
	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_UP_LEFT_X]	= m_lCleanDirtUpleftPosX;
	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_UP_LEFT_Y]	= m_lCleanDirtUpleftPosY;
	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_LOWER_RIGHT_X]	= m_lCleanDirtLowerRightPosX;
	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_WT_AUTO_CLEAN_DIRT_LOWER_RIGHT_Y]	= m_lCleanDirtLowerRightPosY;

	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_EJ_PIN_UP_LEVEL]	= m_lCleanDirtPinUpLevel;
	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_CYCLE_AREA_COUNT]	= m_lCleanDirtCycleAreaCount;
	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_CYCLE_EJ_UP_COUNT]	= m_lCleanDirtCycleEjUpCount;
	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_SAMPLING_COUNT]	= m_lCleanDirtSamplingCount;

	(*psmf)[EJ_PIN_CLEAN][EJ_PIN_AUTO_CLEAN_DIRT_ENABLE]			= m_lCleanDirtEJPinEnable;
}


BOOL CEjectorPinCleanRegion::IsCleanDirtEJPinEnable()
{
	return m_lCleanDirtEJPinEnable;
}


BOOL CEjectorPinCleanRegion::IsReachCleanDirtEJPin()
{
	return (IsCleanDirtEJPinEnable() && (m_lCleanDirtSamplingIndex >= m_lCleanDirtSamplingCount));
}

VOID CEjectorPinCleanRegion::IncCleanDirtEJPinCount()
{
	if (IsCleanDirtEJPinEnable())
	{
		m_lCleanDirtSamplingIndex++;
	}
	else
	{
		ResetCleanDirtEJPinCount();
	}
}


VOID CEjectorPinCleanRegion::ResetCleanDirtEJPinCount()
{
	m_lCleanDirtSamplingIndex = 0; 
}


VOID CEjectorPinCleanRegion::SetCleanDirtUpleftPos(const LONG lCleanDirtUpleftPosX, const LONG lCleanDirtUpleftPosY)
{
	m_lCleanDirtUpleftPosX = lCleanDirtUpleftPosX;
	m_lCleanDirtUpleftPosY = lCleanDirtUpleftPosY;
}


LONG CEjectorPinCleanRegion::GetCleanDirtUpleftPosX()
{
	return m_lCleanDirtUpleftPosX;
}


LONG CEjectorPinCleanRegion::GetCleanDirtUpleftPosY()
{
	return m_lCleanDirtUpleftPosY;
}


VOID CEjectorPinCleanRegion::SetCleanDirtLowerRightPos(const LONG lCleanDirtLowerRightPosX, const LONG lCleanDirtLowerRightPosY)
{
	m_lCleanDirtLowerRightPosX = lCleanDirtLowerRightPosX;
	m_lCleanDirtLowerRightPosY = lCleanDirtLowerRightPosY;
}


LONG CEjectorPinCleanRegion::GetCleanDirtLowerRightPosX()
{
	return m_lCleanDirtLowerRightPosX;
}


LONG CEjectorPinCleanRegion::GetCleanDirtLowerRightPosY()
{
	return m_lCleanDirtLowerRightPosY;
}


LONG CEjectorPinCleanRegion::GetCleanDirtCycleAreaCount()
{
	return m_lCleanDirtCycleAreaCount;
}


LONG CEjectorPinCleanRegion::GetCleanDirtCycleEjPinUpCount()
{
	return m_lCleanDirtCycleEjUpCount;
}


LONG CEjectorPinCleanRegion::GetCleanDirtPinUpLevel()
{
	return m_lCleanDirtPinUpLevel;
}


LONG CEjectorPinCleanRegion::GetCleanDirtSamplingCount()
{
	return m_lCleanDirtSamplingCount;
}


LONG CEjectorPinCleanRegion::_round(double val)
{
	return CMathFunc::Instance()->_round(val);
}


VOID CEjectorPinCleanRegion::GetCleanDirtXYPosn(LONG &lX, LONG &lY)
{
	if (m_lCleanDirtMatrixCol <= 0)
	{
		m_lCleanDirtMatrixCol = 1;
	}
	if (m_lCleanDirtMatrixRow <= 0)
	{
		m_lCleanDirtMatrixRow = 1;
	}
	double dPitchX = (m_lCleanDirtLowerRightPosX - m_lCleanDirtUpleftPosX) / m_lCleanDirtMatrixCol;
	double dPitchY = (m_lCleanDirtLowerRightPosY - m_lCleanDirtUpleftPosY) / m_lCleanDirtMatrixRow;
	lX = m_lCleanDirtUpleftPosX + _round(dPitchX * (m_lCleanDirtColIndex + 0.5));
	lY = m_lCleanDirtUpleftPosY + _round(dPitchY * (m_lCleanDirtRowIndex + 0.5));
}


VOID CEjectorPinCleanRegion::MoveToNextCleanDirtArea()
{
	m_lCleanDirtColIndex++;
	if (m_lCleanDirtColIndex > m_lCleanDirtMatrixCol)
	{
		m_lCleanDirtRowIndex++;
		m_lCleanDirtColIndex = 0;
		if (m_lCleanDirtRowIndex > m_lCleanDirtMatrixRow)
		{
			m_lCleanDirtColIndex = 0;
			m_lCleanDirtRowIndex = 0;
		}
	}
}


