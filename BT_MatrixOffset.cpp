//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "BT_MatrixOffset.h"
#include "math.h"
#include "LogFileUtil.h"
#include "MS896A.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



BT_CMatrixOffsetInfo::BT_CMatrixOffsetInfo()
{
	BM_InitPoints();
}

BT_CMatrixOffsetInfo::~BT_CMatrixOffsetInfo()
{
}

VOID BT_CMatrixOffsetInfo::BM_InitPoints()
{
	m_bState_BM		= FALSE;
	m_ulNoOfRows_BM	= 0;
	m_ulNoOfCols_BM	= 0;

	for (ULONG lRow = 0; lRow < BM_MAX_ROWS; lRow++)
	{
		for (ULONG lCol = 0; lCol < BM_MAX_COLS; lCol++)
		{
			m_pPoints_BM[lRow][lCol].m_lBM_State	= -1;
			m_pPoints_BM[lRow][lCol].m_dBM_DistX	= 0;
			m_pPoints_BM[lRow][lCol].m_dBM_DistY	= 0;
			m_pPoints_BM[lRow][lCol].m_lBM_BT_X		= 0;
			m_pPoints_BM[lRow][lCol].m_lBM_BT_Y		= 0;
			m_pPoints_BM[lRow][lCol].m_lBM_OffsetX	= BT_MATRIX_INIT;
			m_pPoints_BM[lRow][lCol].m_lBM_OffsetY	= BT_MATRIX_INIT;
		}
	}
}

BOOL BT_CMatrixOffsetInfo::BM_SetPoint(ULONG ulRow, ULONG ulCol, LONG lBT_X, LONG lBT_Y, LONG lOffsetX, LONG lOffsetY, LONG lState)
{
	if ( ulRow >= BM_MAX_ROWS || ulCol >= BM_MAX_COLS )
	{
		return FALSE;
	}

	m_pPoints_BM[ulRow][ulCol].m_lBM_BT_X		= lBT_X;
	m_pPoints_BM[ulRow][ulCol].m_lBM_BT_Y		= lBT_Y;
	m_pPoints_BM[ulRow][ulCol].m_lBM_OffsetX	= lOffsetX;
	m_pPoints_BM[ulRow][ulCol].m_lBM_OffsetY	= lOffsetY;
	m_pPoints_BM[ulRow][ulCol].m_lBM_State		= lState;

	return TRUE;
}

BOOL BT_CMatrixOffsetInfo::BM_SetDrawingPoint(ULONG ulRow, ULONG ulCol, DOUBLE dDistX, DOUBLE dDistY, LONG lState)
{
	if ( ulRow >= BM_MAX_ROWS || ulCol >= BM_MAX_COLS )
	{
		return FALSE;
	}

	m_pPoints_BM[ulRow][ulCol].m_dBM_DistX	= dDistX;
	m_pPoints_BM[ulRow][ulCol].m_dBM_DistY	= dDistY;
	m_pPoints_BM[ulRow][ulCol].m_lBM_State	= lState;

	return TRUE;
}

LONG BT_CMatrixOffsetInfo::BM_GetDrawingPoint(ULONG ulRow, ULONG ulCol, DOUBLE &dDistX, DOUBLE &dDistY)
{
	if ( ulRow >= BM_MAX_ROWS || ulCol >= BM_MAX_COLS )
	{
		return -1;
	}

	if( m_pPoints_BM[ulRow][ulCol].m_lBM_State == -1 )
	{
		return -1;
	}

	dDistX = m_pPoints_BM[ulRow][ulCol].m_dBM_DistX;
	dDistY = m_pPoints_BM[ulRow][ulCol].m_dBM_DistY;

	return TRUE;
}

LONG BT_CMatrixOffsetInfo::BM_GetPoint(ULONG ulRow, ULONG ulCol, LONG &lBT_X, LONG &lBT_Y, LONG &lOffsetX, LONG &lOffsetY)
{
	if ( ulRow >= BM_MAX_ROWS || ulCol >= BM_MAX_COLS )
	{
		return -1;
	}

	lBT_X		= m_pPoints_BM[ulRow][ulCol].m_lBM_BT_X;
	lBT_Y		= m_pPoints_BM[ulRow][ulCol].m_lBM_BT_Y;
	lOffsetX	= m_pPoints_BM[ulRow][ulCol].m_lBM_OffsetX;
	lOffsetY	= m_pPoints_BM[ulRow][ulCol].m_lBM_OffsetY;
	LONG lState = m_pPoints_BM[ulRow][ulCol].m_lBM_State;
	if ( lOffsetX == BT_MATRIX_INIT || lOffsetY == BT_MATRIX_INIT )
	{
		lState = 0;
	}

	return lState;
}

BOOL BT_CMatrixOffsetInfo::BM_GetPointCornerTL(CONST LONG lBT_X, CONST LONG lBT_Y, ULONG &ulULRow, ULONG &ulULCol)
{
	LONG lCheckX = 0, lCheckY = 0, lOffsetX = 0, lOffsetY = 0;

	BOOL bFindUL = FALSE;
	DOUBLE dNearDistUL = -1, dDist = 0;
	for (ULONG ulRow = 0; ulRow < BM_MAX_ROWS; ulRow++)
	{
		for (ULONG ulCol = 0; ulCol < BM_MAX_COLS; ulCol++)
		{
			if( BM_GetPoint(ulRow, ulCol, lCheckX, lCheckY, lOffsetX, lOffsetY) != 1 )
			{
				continue;
			}

			if( lBT_X > (lCheckX + 500) || lBT_Y > (lCheckY + 500) )
			{
				continue;
			}

			DOUBLE dDistX = lCheckX - lBT_X;
			DOUBLE dDistY = lCheckY - lBT_Y;
			dDist = sqrt(dDistX * dDistX + dDistY * dDistY);
			
			if ( lCheckX >= lBT_X && lCheckY >= lBT_Y )
			{
				if ( dNearDistUL == -1 )
				{
					dNearDistUL = dDist;
					ulULRow = ulRow;
					ulULCol = ulCol;
				}
				if ( dNearDistUL > dDist )
				{
					ulULRow = ulRow;
					ulULCol = ulCol;
					dNearDistUL = dDist;
				}
				bFindUL = TRUE;
			}
		}
	}

	return bFindUL;
}

BOOL BT_CMatrixOffsetInfo::BM_GetPointCornerTR(CONST LONG lBT_X, CONST LONG lBT_Y, ULONG &ulTRRow, ULONG &ulTRCol)
{
	LONG lCheckX = 0, lCheckY = 0, lOffsetX = 0, lOffsetY = 0;

	BOOL bFind = FALSE;
	DOUBLE dNearDist = -1, dDist = 0;
	for (ULONG ulRow = 0; ulRow < BM_MAX_ROWS; ulRow++)
	{
		for (ULONG ulCol = 0; ulCol < BM_MAX_COLS; ulCol++)
		{
			if ( BM_GetPoint(ulRow, ulCol, lCheckX, lCheckY, lOffsetX, lOffsetY) != 1 )
			{
				continue;
			}

			if ( lBT_X < (lCheckX - 500) || lBT_Y > (lCheckY + 500) )
			{
				continue;
			}

			DOUBLE dDistX = lCheckX-lBT_X;
			DOUBLE dDistY = lCheckY-lBT_Y;
			dDist = sqrt(dDistX * dDistX + dDistY * dDistY);
			if ( lCheckX <= lBT_X && lCheckY >= lBT_Y )
			{
				if ( dNearDist == -1 )
				{
					dNearDist = dDist;
					ulTRRow = ulRow;
					ulTRCol = ulCol;
					//CMSLogFileUtility::Instance()->BT_TableIndexLog("this1");
				}
				if ( dNearDist > dDist )
				{
					ulTRRow = ulRow;
					ulTRCol = ulCol;
					dNearDist = dDist;
					//CMSLogFileUtility::Instance()->BT_TableIndexLog("this2");
				}
				bFind = TRUE;
			}
		}
	}

	return bFind;
}

BOOL BT_CMatrixOffsetInfo::BM_GetPointCornerBR(CONST LONG lBT_X, CONST LONG lBT_Y, ULONG &ulLRRow, ULONG &ulLRCol)
{
	LONG lCheckX = 0, lCheckY = 0, lOffsetX = 0, lOffsetY = 0;

	BOOL bFindLR = FALSE;
	DOUBLE dNearDistLR = -1, dDist = 0;
	for (ULONG ulRow = 0; ulRow < BM_MAX_ROWS; ulRow++)
	{
		for (ULONG ulCol = 0; ulCol < BM_MAX_COLS; ulCol++)
		{
			if ( BM_GetPoint(ulRow, ulCol, lCheckX, lCheckY, lOffsetX, lOffsetY) != 1 )
			{
				continue;
			}

			if ( lBT_X < (lCheckX - 500) || lBT_Y < (lCheckY - 500) )
			{
				continue;
			}

			DOUBLE dDistX = lCheckX-lBT_X;
			DOUBLE dDistY = lCheckY-lBT_Y;
			dDist = sqrt(dDistX*dDistX + dDistY*dDistY);
			if ( lCheckX <= lBT_X && lCheckY <= lBT_Y )
			{
				if ( dNearDistLR == -1 )
				{
					dNearDistLR = dDist;
					ulLRRow = ulRow;
					ulLRCol = ulCol;
				}
				if ( dNearDistLR > dDist )
				{
					ulLRRow = ulRow;
					ulLRCol = ulCol;
					dNearDistLR = dDist;
				}
				bFindLR = TRUE;
			}
		}
	}

	return bFindLR;
}	//	get LR corner

BOOL BT_CMatrixOffsetInfo::BM_GetPointCornerBL(CONST LONG lBT_X, CONST LONG lBT_Y, ULONG &ulBLRow, ULONG &ulBLCol)
{
	LONG lCheckX = 0, lCheckY = 0, lOffsetX = 0, lOffsetY = 0;

	BOOL bFind = FALSE;
	DOUBLE dNearDist = -1, dDist = 0;
	for (ULONG ulRow = 0; ulRow < BM_MAX_ROWS; ulRow++)
	{
		for (ULONG ulCol = 0; ulCol < BM_MAX_COLS; ulCol++)
		{
			if ( BM_GetPoint(ulRow, ulCol, lCheckX, lCheckY, lOffsetX, lOffsetY) != 1 )
			{
				continue;
			}

			if ( lBT_X > (lCheckX + 500) || lBT_Y < (lCheckY - 500) )
			{
				continue;
			}

			DOUBLE dDistX = lCheckX-lBT_X;
			DOUBLE dDistY = lCheckY-lBT_Y;
			dDist = sqrt(dDistX * dDistX + dDistY * dDistY);
			if( lCheckX >= lBT_X && lCheckY <= lBT_Y )
			{
				if ( dNearDist == -1 )
				{
					dNearDist = dDist;
					ulBLRow = ulRow;
					ulBLCol = ulCol;
					//CMSLogFileUtility::Instance()->BT_TableIndexLog("this1");
				}
				if ( dNearDist > dDist )
				{
					ulBLRow = ulRow;
					ulBLCol = ulCol;
					dNearDist = dDist;
					//CMSLogFileUtility::Instance()->BT_TableIndexLog("this2");
				}
			}
			bFind = TRUE;
		}
	}

	return bFind;
}	//	get BL corner

BOOL BT_CMatrixOffsetInfo::BM_GetNearestValid(LONG lBT_X, LONG lBT_Y, ULONG &ulOutRow, ULONG &ulOutCol)
{
	DOUBLE lNearDist = -1, lDist = 0;
	BOOL bFindNear = FALSE;

	for (ULONG ulRow = 0; ulRow < BM_MAX_ROWS; ulRow++)
	{
		for (ULONG ulCol = 0; ulCol < BM_MAX_COLS; ulCol++)
		{
			LONG lCheckX = 0, lCheckY = 0, lOffsetX = 0, lOffsetY = 0;
			if ( BM_GetPoint(ulRow, ulCol, lCheckX, lCheckY, lOffsetX, lOffsetY) != 1 )
			{
				continue;
			}

			DOUBLE dDistX = lCheckX-lBT_X;
			DOUBLE dDistY = lCheckY-lBT_Y;
			lDist = sqrt(dDistX * dDistX + dDistY * dDistY);
			if ( lNearDist == -1 )
			{
				ulOutRow = ulRow;
				ulOutCol = ulCol;
				lNearDist = lDist;
			}
			if ( lNearDist > lDist )
			{
				ulOutRow = ulRow;
				ulOutCol = ulCol;
				lNearDist = lDist;
			}
			bFindNear = TRUE;
		}
	}

	return bFindNear;
}

BOOL BT_CMatrixOffsetInfo::BM_SetState(BOOL bState)
{
	m_bState_BM		= bState;
	return TRUE;
}

BOOL BT_CMatrixOffsetInfo::BM_SetScope(BOOL bState, ULONG ulNoOfRows, ULONG ulNoOfCols)
{
	m_bState_BM		= bState;
	m_ulNoOfRows_BM	= ulNoOfRows;
	m_ulNoOfCols_BM = ulNoOfCols;
	return TRUE;
}

BOOL BT_CMatrixOffsetInfo::BM_GetState()
{
	return m_bState_BM;
}

ULONG BT_CMatrixOffsetInfo::BM_GetMaxRows()
{
	return m_ulNoOfRows_BM;
}

ULONG BT_CMatrixOffsetInfo::BM_GetMaxCols()
{
	return m_ulNoOfCols_BM;
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

BT_CErrMapBTMarkCompInfo::BT_CErrMapBTMarkCompInfo()
{
	InitPoints();
}

BT_CErrMapBTMarkCompInfo::~BT_CErrMapBTMarkCompInfo()
{
}


VOID BT_CErrMapBTMarkCompInfo::InitPoints()
{
	m_bEnableBTMarkComp	= FALSE;
	m_lBTMarkCompLimit	= 0;
	m_lBTMarkCompCount	= 0;

	m_lErrMapBTMarkTLX	= 0;
	m_lErrMapBTMarkTLY	= 0;
	m_lErrMapBTMarkTRX	= 0;
	m_lErrMapBTMarkTRY	= 0;
	m_lErrMapBTMarkBLX	= 0;
	m_lErrMapBTMarkBLY	= 0;
	m_lErrMapBTMarkBRX	= 0;
	m_lErrMapBTMarkBRY	= 0;

	m_lTLEncX	= 0;
	m_lTLEncY	= 0;
	m_lTREncX	= 0;
	m_lTREncY	= 0;
	m_lBLEncX	= 0;
	m_lBLEncY	= 0;
	m_lBREncX	= 0;
	m_lBREncY	= 0;

	m_dA	= 0;
    m_dB	= 0;
    m_dE	= 0;
    m_dC	= 0;
    m_dD	= 0;
    m_dF	= 0;
}

VOID BT_CErrMapBTMarkCompInfo::ResetRunTimeMemory()
{
	m_lBTMarkCompCount	= 0;

	m_lTLEncX	= 0;
	m_lTLEncY	= 0;
	m_lTREncX	= 0;
	m_lTREncY	= 0;
	m_lBLEncX	= 0;
	m_lBLEncY	= 0;
	m_lBREncX	= 0;
	m_lBREncY	= 0;

	m_dA	= 0;
    m_dB	= 0;
    m_dE	= 0;
    m_dC	= 0;
    m_dD	= 0;
    m_dF	= 0;
}

BOOL BT_CErrMapBTMarkCompInfo::GetBTMarkPosXY(eErrMapMarkPos ePos, LONG &lEncX, LONG &lEncY)
{
	BOOL bStatus = FALSE;

	switch (ePos)
	{
	case BM_MARK_TL:
		lEncX = m_lErrMapBTMarkTLX;
		lEncY = m_lErrMapBTMarkTLY;
		bStatus = TRUE;
		break;

	case BM_MARK_TR:
		lEncX = m_lErrMapBTMarkTRX;
		lEncY = m_lErrMapBTMarkTRY;
		bStatus = TRUE;
		break;

	case BM_MARK_BL:
		lEncX = m_lErrMapBTMarkBLX;
		lEncY = m_lErrMapBTMarkBLY;
		bStatus = TRUE;
		break;

	case BM_MARK_BR:
		lEncX = m_lErrMapBTMarkBRX;
		lEncY = m_lErrMapBTMarkBRY;
		bStatus = TRUE;
		break;
	}

	return bStatus;
}


VOID BT_CErrMapBTMarkCompInfo::SetBTMarkPosXY(eErrMapMarkPos ePos, LONG lEncX, LONG lEncY)
{
	switch (ePos)
	{
	case BM_MARK_TL:
		m_lErrMapBTMarkTLX = lEncX;
		m_lErrMapBTMarkTLY = lEncY;
		break;

	case BM_MARK_TR:
		m_lErrMapBTMarkTRX = lEncX;
		m_lErrMapBTMarkTRY = lEncY;
		break;

	case BM_MARK_BL:
		m_lErrMapBTMarkBLX = lEncX;
		m_lErrMapBTMarkBLY = lEncY;
		break;

	case BM_MARK_BR:
		m_lErrMapBTMarkBRX = lEncX;
		m_lErrMapBTMarkBRY = lEncY;
		break;
	}
}

BOOL BT_CErrMapBTMarkCompInfo::GetCurrMarkEncXY(eErrMapMarkPos ePos, LONG &lEncX, LONG &lEncY)
{
	BOOL bStatus = FALSE;

	switch (ePos)
	{
	case BM_MARK_TL:
		lEncX = m_lTLEncX;
		lEncY = m_lTLEncY;
		bStatus = TRUE;
		break;

	case BM_MARK_TR:
		lEncX = m_lTREncX;
		lEncY = m_lTREncY;
		bStatus = TRUE;
		break;

	case BM_MARK_BL:
		lEncX = m_lBLEncX;
		lEncY = m_lBLEncY;
		bStatus = TRUE;
		break;

	case BM_MARK_BR:
		lEncX = m_lBREncX;
		lEncY = m_lBREncY;
		bStatus = TRUE;
		break;
	}

	return bStatus;
}

VOID BT_CErrMapBTMarkCompInfo::SetCurrMarkEncXY(eErrMapMarkPos ePos, LONG lEncX, LONG lEncY)
{
	switch (ePos)
	{
	case BM_MARK_TL:
		m_lTLEncX = lEncX;
		m_lTLEncY = lEncY;
		break;

	case BM_MARK_TR:
		m_lTREncX = lEncX;
		m_lTREncY = lEncY;
		break;

	case BM_MARK_BL:
		m_lBLEncX = lEncX;
		m_lBLEncY = lEncY;
		break;

	case BM_MARK_BR:
		m_lBREncX = lEncX;
		m_lBREncY = lEncY;
		break;
	}
}


BOOL BT_CErrMapBTMarkCompInfo::UpdateBTMarkPos(BOOL bUpdateCurrPos)
{
	CString szLog;

	if (bUpdateCurrPos)
	{
		szLog.Format("BT AFFLINE Transform curr POS updated - TL(%ld, %ld), TR(%ld, %ld), BL(%ld, %ld), BR(%ld, %ld)",
				m_lTLEncX, m_lTLEncY, 
				m_lTREncX, m_lTREncY, 
				m_lBLEncX, m_lBLEncY,
				m_lBREncX, m_lBREncY);
	}
	else
	{
		szLog.Format("BT AFFLINE Transform POS updated - TL(%ld, %ld), TR(%ld, %ld), BL(%ld, %ld), BR(%ld, %ld)",
				m_lErrMapBTMarkTLX, m_lErrMapBTMarkTLY, 
				m_lErrMapBTMarkTRX, m_lErrMapBTMarkTRY, 
				m_lErrMapBTMarkBLX, m_lErrMapBTMarkBLY,
				m_lErrMapBTMarkBRX, m_lErrMapBTMarkBRY);
	}

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return TRUE;
}

BOOL BT_CErrMapBTMarkCompInfo::PerformAffineTransform()
{
	if (!m_bEnableBTMarkComp)
	{
		return TRUE;
	}

	if ( (m_lErrMapBTMarkTLX == 0) && (m_lErrMapBTMarkTLY == 0) )
	{
		return TRUE;
	}
	if ( (m_lTLEncX == 0) && (m_lTREncY == 0) )
	{
		return TRUE;
	}

	return CalculateAfflineOffsetMatrix();
}

	
BOOL BT_CErrMapBTMarkCompInfo::CalculateAfflineOffsetMatrix()
{
	if (!m_bEnableBTMarkComp)
	{
		return FALSE;
	}
	if (m_lBTMarkCompLimit <= 0)
	{
		return FALSE;
	}

	m_dA	= 0;
    m_dB	= 0;
    m_dE	= 0;
    m_dC	= 0;
    m_dD	= 0;
    m_dF	= 0;


    INT nRow = 4;					//4 corners' ENC positions
    INT nTotalRow = nRow * 2;
    LONG lX = 0, lY = 0;
    LONG lCurrX = 0, lCurrY = 0;
	CString szLog;
	
	SFM_CMatrix mA(nTotalRow, 6);

    for (INT i = 0; i < nRow; ++i)
    {
		GetBTMarkPosXY(		(eErrMapMarkPos) (i + 1), lX,		lY);			//OLd enc pos
		//GetCurrMarkEncXY(	(eErrMapMarkPos) i, lCurrX,	lCurrY);		//New enc pos

		szLog.Format("BT AFFLINE Transform OLD POS #%d - (%ld, %ld)", i + 1, lX, lY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


		mA(i, 0) = lX;
		mA(i, 1) = lY;

		mA(i, 2) = 1;
		mA(i, 3) = 0;
		mA(i, 4) = 0;
		mA(i, 5) = 0;
		//mA(i, 6) = -lX * lCurrX;
		//mA(i, 7) = -lY * lCurrX;

		INT j = i + nRow;
		mA(j, 0) = 0;
		mA(j, 1) = 0;
		mA(j, 2) = 0;
		mA(j, 3) = lX;
		mA(j, 4) = lY;
		mA(j, 5) = 1;
		//mA(j, 6) = -lX * lCurrY;
		//mA(j, 7) = -lY * lCurrY;
    }

    SFM_CMatrix mB(nTotalRow, 1);
    for (INT i = 0; i < nRow; ++i)
    {
		GetCurrMarkEncXY(	(eErrMapMarkPos) (i + 1), lCurrX,	lCurrY);		//New enc pos

		szLog.Format("BT AFFLINE Transform NEW POS #%d - (%ld, %ld)", i + 1, lCurrX, lCurrY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		mB(i, 0) = lCurrX;

		INT j = i + nRow;
		mB(j, 0) = lCurrY;
    }

	try
	{
		// To solve A v = B by lease square method
		SFM_CMatrix mTmp = (1 / (~mA * mA)) * ~mA;
		SFM_CMatrix mV = mTmp * mB;

		m_dA	= mV(0, 0);
		m_dB	= mV(1, 0);
		m_dE	= mV(2, 0);
		m_dC	= mV(3, 0);
		m_dD	= mV(4, 0);
		m_dF	= mV(5, 0);

	}
	catch (CAsmException e)
	{
		// Although it is unlikely in your calibration operation, 
		// the measurement data are not linearly independent and cause a singular matrix.
		//DisplayException(e);
		
		return FALSE;
	}

	szLog.Format("BT AFFLINE Transform updated - Matrix (A=%f, B=%f, C=%f, D=%f, E=%f, F=%f)", m_dA, m_dB, m_dC, m_dD, m_dE, m_dF);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	return TRUE;
}


BOOL BT_CErrMapBTMarkCompInfo::CalculateAfflineOffsetXY(LONG &lEncX, LONG &lEncY)
{
	if (!m_bEnableBTMarkComp)
	{
		return FALSE;
	}
	if (m_lBTMarkCompLimit <= 0)
	{
		return FALSE;
	}

	CString szLog;
	LONG lOldEncX = lEncX;
	LONG lOldEncY = lEncY;

	if ((m_dA == 0) || (m_dD == 0))
	{
		szLog.Format("BT AFFLINE Transform: OLD (%ld, %ld), NEW (%ld, %ld) - not used (A and C = 0)",
				lOldEncX, lOldEncY, lEncX, lEncY);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		return FALSE;
	}


	LONG lNewEncX = (LONG) (m_dA * lOldEncX + m_dB * lOldEncY + m_dE);
	LONG lNewEncY = (LONG) (m_dC * lOldEncX + m_dD * lOldEncY + m_dF);

	szLog.Format("BT AFFLINE Transform: OLD (%ld, %ld), NEW (%ld, %ld) - M (A=%f, B=%f, C=%f, D=%f, E=%f, F=%f)",
			lOldEncX, lOldEncY, lNewEncX, lNewEncY, m_dA, m_dB, m_dC, m_dD, m_dE, m_dF);
	
	CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	//v4.69A15
	lEncX = lNewEncX;
	lEncY = lNewEncY;

	return TRUE;
}



