#ifndef EjectorPinCleanRegion_H
#define EjectorPinCleanRegion_H

#include "StringMapFile.h"

#define	EJ_PIN_CLEAN							"EJ Pin Clean"
#define EJ_PIN_WT_AUTO_CLEAN_DIRT_MATRIX_ROW	"EJ Pin Wafer Table Clean Dirt Matrix Row"
#define EJ_PIN_WT_AUTO_CLEAN_DIRT_MATRIX_COL	"EJ Pin Wafer Table Clean Dirt Matrix COL"
#define EJ_PIN_WT_AUTO_CLEAN_DIRT_UP_LEFT_X		"EJ Pin Wafer Table Clean Dirt UpLeft X"
#define EJ_PIN_WT_AUTO_CLEAN_DIRT_UP_LEFT_Y		"EJ Pin Wafer Table Clean Dirt UpLeft Y"
#define EJ_PIN_WT_AUTO_CLEAN_DIRT_LOWER_RIGHT_X "EJ Pin Wafer Table Clean Dirt LowerRight X"
#define EJ_PIN_WT_AUTO_CLEAN_DIRT_LOWER_RIGHT_Y "EJ Pin Wafer Table Clean Dirt LowerRight Y"
#define EJ_PIN_AUTO_CLEAN_DIRT_EJ_PIN_UP_LEVEL	"EJ Pin Clean Dirt Up Level"
#define EJ_PIN_AUTO_CLEAN_DIRT_CYCLE_AREA_COUNT	"EJ Pin Clean Dirt Cycle Area Count"
#define EJ_PIN_AUTO_CLEAN_DIRT_CYCLE_EJ_UP_COUNT "EJ Pin Clean Dirt Cycle Ej Up Count"
#define EJ_PIN_AUTO_CLEAN_DIRT_SAMPLING_COUNT	 "EJ Pin Clean Dirt Cycle Sampling Count"
#define EJ_PIN_AUTO_CLEAN_DIRT_ENABLE			"EJ Pin Clean Dirt Enable"
//================================================================
// Class definition
//================================================================
class CEjectorPinCleanRegion
{
public:
	CEjectorPinCleanRegion(); //Constructor
	~CEjectorPinCleanRegion(); //Deconstructor

	VOID LoadData( CStringMapFile *psmf);
	VOID SaveData( CStringMapFile *psmf);

	BOOL IsCleanDirtEJPinEnable();
	BOOL IsReachCleanDirtEJPin();

	VOID IncCleanDirtEJPinCount();
	VOID ResetCleanDirtEJPinCount();

	VOID SetCleanDirtUpleftPos(const LONG lCleanDirtUpleftPosX, const LONG lCleanDirtUpleftPosY);
	LONG GetCleanDirtUpleftPosX();
	LONG GetCleanDirtUpleftPosY();

	VOID SetCleanDirtLowerRightPos(const LONG lCleanDirtLowerRightPosX, const LONG lCleanDirtLowerRightPosY);
	LONG GetCleanDirtLowerRightPosX();
	LONG GetCleanDirtLowerRightPosY();

	LONG GetCleanDirtCycleAreaCount();
	LONG GetCleanDirtCycleEjPinUpCount();
	LONG GetCleanDirtPinUpLevel();
	LONG GetCleanDirtSamplingCount();

	LONG _round(double val);
	VOID GetCleanDirtXYPosn(LONG &lX, LONG &lY);
	VOID MoveToNextCleanDirtArea();

public:
	// Clean Dirt Row & Column
	LONG	m_lCleanDirtMatrixRow;
	LONG	m_lCleanDirtMatrixCol;
	LONG	m_lCleanDirtUpleftPosX;
	LONG	m_lCleanDirtUpleftPosY;
	LONG	m_lCleanDirtLowerRightPosX;
	LONG	m_lCleanDirtLowerRightPosY;
	LONG	m_lCleanDirtRowIndex;
	LONG	m_lCleanDirtColIndex;

	LONG	m_lCleanDirtPinUpLevel;
	LONG	m_lCleanDirtCycleAreaCount;
	LONG	m_lCleanDirtCycleEjUpCount;
	LONG	m_lCleanDirtSamplingCount;

	BOOL	m_lCleanDirtEJPinEnable;

private:
	BOOL	m_lCleanDirtSamplingIndex;
};

#endif  // WT_EjectorPinCleanRegion_H

//================================================================
// End of file WT_EjectorPinCleanRegion.h
//================================================================