#ifndef BT_MATRIX_OFFSET_H
#define BT_MATRIX_OFFSET_H

#define	BM_MAX_ROWS			200
#define	BM_MAX_COLS			200
#define	BT_MATRIX_INIT			-999999

enum eErrMapMarkPos {BM_MARK_TL = 1, BM_MARK_TR = 2, BM_MARK_BL = 3, BM_MARK_BR = 4};


//	Chip Prober probing position-level mapping class
typedef struct
{
	LONG	m_lBM_State;		//	-1, this point not exist; 0, not sample or sample fail; 1, ok, ready to use
	DOUBLE	m_dBM_DistX;		//	the ditance to first of two cross marks, in cm.
	DOUBLE	m_dBM_DistY;		//	the ditance to first of two cross marks, in cm.
	LONG	m_lBM_BT_X;			//	the ideal encoder position based on first mark encode and distance
	LONG	m_lBM_BT_Y;			//	the ideal encoder position based on first mark encode and distance
	LONG	m_lBM_OffsetX;		//	the offset after move with above and PR search.
	LONG	m_lBM_OffsetY;		//	the offset after move with above and PR search.
}	BM_PROPERTY;


#include "SFM_CMatrix.h"		//v4.69A12



class BT_CMatrixOffsetInfo : public CObject
{
public:
	VOID	BM_InitPoints();

	// ===========================================================
	// Get Functions
	BOOL	BM_GetState();
	ULONG	BM_GetMaxRows();
	ULONG	BM_GetMaxCols();
	LONG	BM_GetDrawingPoint(ULONG ulRow, ULONG ulCol, DOUBLE &dDistX, DOUBLE &dDistY);
	LONG	BM_GetPoint(ULONG ulRow, ULONG ulCol, LONG &lBT_X, LONG &lBT_Y, LONG &lOffsetX, LONG &lOffsetY);
	BOOL	BM_GetPointCornerTL(CONST LONG lBT_X, CONST LONG lBT_Y, ULONG &ulULRow, ULONG &ulULCol);
	BOOL	BM_GetPointCornerTR(CONST LONG lBT_X, CONST LONG lBT_Y, ULONG &ulULRow, ULONG &ulULCol);
	BOOL	BM_GetPointCornerBR(CONST LONG lBT_X, CONST LONG lBT_Y, ULONG &ulLRRow, ULONG &ulLRCol);
	BOOL	BM_GetPointCornerBL(CONST LONG lBT_X, CONST LONG lBT_Y, ULONG &ulLRRow, ULONG &ulLRCol);
	BOOL	BM_GetNearestValid(LONG lBT_X, LONG lBT_Y, ULONG &ulOutRow, ULONG &ulOutCol);

	// ===========================================================
	// Set Functions
	BOOL	BM_SetState(BOOL bState);
	BOOL	BM_SetScope(BOOL bState, ULONG ulNoOfRows, ULONG ulNoOfCols);
	BOOL	BM_SetDrawingPoint(ULONG ulRow, ULONG ulCol, DOUBLE dDistX, DOUBLE dDistY, LONG lState);
	BOOL	BM_SetPoint(ULONG ulRow, ULONG ulCol, LONG lBT_X, LONG lBT_Y, LONG lOffsetX, LONG lOffsetY, LONG lState);

	// ===========================================================
	// Constructor and destructor
	BT_CMatrixOffsetInfo();
	~BT_CMatrixOffsetInfo();


private:
	
	BOOL		m_bState_BM;
	ULONG		m_ulNoOfRows_BM;
	ULONG		m_ulNoOfCols_BM;
	BM_PROPERTY	m_pPoints_BM[BM_MAX_ROWS][BM_MAX_COLS];

};




class BT_CErrMapBTMarkCompInfo : public CObject
{
public:

	VOID	InitPoints();
	VOID	ResetRunTimeMemory();

	// ===========================================================
	// Get Functions
	BOOL GetBTMarkPosXY(	eErrMapMarkPos ePos, LONG &lEncX, LONG &lEncY);
	BOOL GetCurrMarkEncXY(	eErrMapMarkPos ePos, LONG &lEncX, LONG &lEncY);

	// ===========================================================
	// Set Functions
	VOID SetBTMarkPosXY(	eErrMapMarkPos ePos, LONG lEncX, LONG lEncY);
	VOID SetCurrMarkEncXY(	eErrMapMarkPos ePos, LONG lEncX, LONG lEncY);

	// ===========================================================
	// Constructor and destructor
	BT_CErrMapBTMarkCompInfo();
	~BT_CErrMapBTMarkCompInfo();

	BOOL UpdateBTMarkPos(BOOL bUpdateCurrPos = FALSE);
	BOOL PerformAffineTransform();
	BOOL CalculateAfflineOffsetXY(LONG &lEncX, LONG &lEncY);


private:

	BOOL CalculateAfflineOffsetMatrix();


private:

	LONG	m_lErrMapBTMarkTLX;
	LONG	m_lErrMapBTMarkTLY;
	LONG	m_lErrMapBTMarkTRX;
	LONG	m_lErrMapBTMarkTRY;
	LONG	m_lErrMapBTMarkBLX;
	LONG	m_lErrMapBTMarkBLY;
	LONG	m_lErrMapBTMarkBRX;
	LONG	m_lErrMapBTMarkBRY;

	LONG	m_lTLEncX;
	LONG	m_lTLEncY;
	LONG	m_lTREncX;
	LONG	m_lTREncY;
	LONG	m_lBLEncX;
	LONG	m_lBLEncY;
	LONG	m_lBREncX;
	LONG	m_lBREncY;

	DOUBLE m_dA;
	DOUBLE m_dB;
	DOUBLE m_dC;
	DOUBLE m_dD;
	DOUBLE m_dE;
	DOUBLE m_dF;


public:

	BOOL	m_bEnableBTMarkComp;
	LONG	m_lBTMarkCompLimit;
	LONG	m_lBTMarkCompCount;

};


#endif  // BT_MATRIX_OFFSET_H
