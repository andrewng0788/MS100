/////////////////////////////////////////////////////////////////
// BT_Move.cpp : Move functions of the CBinTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BondHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CBinTable::IsAllMotorsHomed()
{
	return (m_bHome_X && m_bHome_Y && m_bHome_T);
}


BOOL CBinTable::IsAllMotorsEnable()
{
	BOOL bXMotorOn = FALSE;
	BOOL bYMotorOn = FALSE;
	BOOL bTMotorOn = FALSE;

	//No BHT in MegaDa
	return FALSE;

	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableBH)		//v3.60
	{
		return FALSE;
	}

	bXMotorOn	= CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X);
	bYMotorOn	= CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y);
	bTMotorOn	= TRUE;//CMS896AStn::MotionIsPowerOn(BT_AXIS_T, &m_stBTAxis_T);

	return (bXMotorOn & bYMotorOn & bTMotorOn);
}

VOID CBinTable::SelectXProfile(const LONG lDistX)
{
	if ((abs(lDistX) > 30000))
	{
		X_Profile(LOW_PROF);
	}
	else
	{
		X_Profile(NORMAL_PROF);
	}
}

VOID CBinTable::SelectYProfile(const LONG lDistY)
{
	if ((abs(lDistY) > 30000))
	{
		Y_Profile(LOW_PROF);
	}
	else
	{
		Y_Profile(NORMAL_PROF);
	}
}


VOID CBinTable::SelectXYProfile(const LONG lDistX, const LONG lDistY)
{
	if ((State() == IDLE_Q) || (abs(lDistX) > 2000) || (abs(lDistY) > 2000))
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
	}
	else
	{
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
	}
}

LONG CBinTable::MoveXYTo(const LONG lPosnX, const LONG lPosnY)
{
	SelectXYProfile(lPosnX - m_lCurXPosn, lPosnY - m_lCurYPosn);

	LONG lRet = X_MoveTo(lPosnX, SFM_NOWAIT);

	if (lRet == gnOK)
	{
		lRet = Y_MoveTo(lPosnY, SFM_WAIT);
	}

	if (lRet == gnOK)
	{
		lRet = X_Sync();
	}

	return lRet;
}

////////////////////////////////////////////
//	X Axis 
////////////////////////////////////////////
BOOL CBinTable::X_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBT)	//v3.60
		return TRUE;
	if (CMS896AStn::m_bMS60NGPick)	//v4.57A1
	{
		return TRUE;
	}
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CBinTable::X_Home()
{
	INT nResult			= gnOK;

	//if (m_fHardware && m_pServo_X && m_bSel_X)
	if (m_fHardware && m_bSel_X)
	{
		try
		{
			CMS896AStn::MotionPowerOff(BT_AXIS_X, &m_stBTAxis_X);
			Sleep(50);
			CMS896AStn::MotionResetController(BT_AXIS_X, &m_stBTAxis_X);

#ifdef NU_MOTION
			CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, 9999999, -9999999, &m_stBTAxis_X);
#endif

			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X, &m_stBTAxis_X) != 0)
			{
				m_bComm_X = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);
			}

			if (!m_bComm_X)
			{
				m_bComm_X = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_X, &m_stBTAxis_X)) == gnOK)
				{
					m_bComm_X = TRUE;
				}
				else
				{
					nResult	= Err_BinTableXCommutate;
					CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);
				}
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionPowerOn(BT_AXIS_X, &m_stBTAxis_X);
				Sleep(50);

				MotionLeaveHomeSensor(BT_AXIS_X, &m_stBTAxis_X);
				{
					nResult = CMS896AStn::MotionMoveHome(BT_AXIS_X, 1, 0, &m_stBTAxis_X);
				}

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X, &m_stBTAxis_X) == 0) && 
					CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BT_AXIS_X, 0, &m_stBTAxis_X);
					m_bHome_X	= TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_BinTableXMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
			CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);

			m_bComm_X	= FALSE;
			nResult = Err_BinTableXMoveHome;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			Sleep(100);
			m_bComm_X	= TRUE;
			m_bHome_X	= TRUE;
		}
	}

   	return nResult;
}


INT CBinTable::X_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		try
		{
			if (!m_bHome_X)
			{
//				nResult	= X_Home();
				nResult	= gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_lX_ProfileType == MS896A_OBW_PROFILE)
				{
					if (abs(m_lCurXPosn - nPos) < 2000)
					{
						X_SelectControl(PL_DYNAMIC);
					}
					else
					{
						X_SelectControl(PL_DYNAMIC1);
					}
					CMS896AStn::MotionObwMoveTo(BT_AXIS_X, nPos, SFM_NOWAIT, &m_stBTAxis_X);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BT_AXIS_X, nPos, SFM_NOWAIT, &m_stBTAxis_X);
				}

				m_lCurXPosn = nPos;

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BT_AXIS_X, 10000, &m_stBTAxis_X);
					if (nResult != gnOK)
					{
						nResult = Err_BinTableXMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
			CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);

			m_bHome_X	= FALSE;
			nResult = Err_BinTableXMove;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT X move to");
	}

	return nResult;
}

INT CBinTable::X_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		try
		{
			if (!m_bHome_X)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				m_lCurXPosn += nPos;

				if (m_lX_ProfileType == MS896A_OBW_PROFILE)
				{
					if (abs(nPos) < 2000)
					{
						X_SelectControl(PL_DYNAMIC);
					}
					else
					{
						X_SelectControl(PL_DYNAMIC1);
					}
					CMS896AStn::MotionObwMove(BT_AXIS_X, nPos, SFM_NOWAIT, &m_stBTAxis_X);
				}
				else
				{
					CMS896AStn::MotionMove(BT_AXIS_X, nPos, SFM_NOWAIT, &m_stBTAxis_X);
				}

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BT_AXIS_X, 10000, &m_stBTAxis_X);
					if (nResult != gnOK)
					{
						nResult = Err_BinTableXMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
			CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);

			m_bHome_X	= FALSE;
			nResult = Err_BinTableXMove;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT X Move");
	}

	return nResult;
}

VOID CBinTable::GetCurXYPosn()
{
	Sleep(20);
	GetEncoderValue();
	m_lCurXPosn = m_lEnc_X;
	m_lCurYPosn = m_lEnc_Y;

	if (m_bUseDualTablesOption)
	{
		m_lCurX2Posn = m_lEnc_X2;
		m_lCurY2Posn = m_lEnc_Y2;
	}
}

INT CBinTable::X_SearchPosLimit(VOID)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		LONG lMotorDirectionX = GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_MOTOR_DIRECTION);				

		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X))
			{
				return Err_BinTableXPosLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X, BT_SP_POS_LIMIT_X, &m_stBTAxis_X);

			if ( lMotorDirectionX == -1 )
			{
				CMS896AStn::MotionSearch(BT_AXIS_X, 0, SFM_WAIT, &m_stBTAxis_X, BT_SP_POS_LIMIT_X);
			}
			else
			{
				CMS896AStn::MotionSearch(BT_AXIS_X, 1, SFM_WAIT, &m_stBTAxis_X, BT_SP_POS_LIMIT_X);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
			nResult = Err_BinTableXPosLimit;
		}
	}

	if (!X_IsPowerOn() || (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X, &m_stBTAxis_X) != 0) )	//v4.18T1
	{
		return gnNOTOK;
	}

	GetCurXYPosn();
	return nResult;
}

INT CBinTable::X_SearchBarcode(LONG lDir, LONG lSearchDist, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		LONG lMotorDirectionX = GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_MOTOR_DIRECTION);				

		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X))
			{
				return Err_BinTableXPosLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X, "spfBinTableXBarcode", &m_stBTAxis_X);

			if ( lDir == 0 )
			{
				CMS896AStn::MotionUpdateSearchProfile(BT_AXIS_X, "spfBinTableXBarcode", 2, -1 * lSearchDist, &m_stBTAxis_X);
				CMS896AStn::MotionSearch(BT_AXIS_X, 1, nMode, &m_stBTAxis_X, "spfBinTableXBarcode");
			}
			else
			{
				CMS896AStn::MotionUpdateSearchProfile(BT_AXIS_X, "spfBinTableXBarcode", 2, lSearchDist, &m_stBTAxis_X);
				CMS896AStn::MotionSearch(BT_AXIS_X, 1, nMode, &m_stBTAxis_X, "spfBinTableXBarcode");
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
			nResult = Err_BinTableXPosLimit;
		}
	}

	if (!X_IsPowerOn() || (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X, &m_stBTAxis_X) != 0) )	//v4.18T1
	{
		return gnNOTOK;
	}

	GetCurXYPosn();
	return nResult;
}

INT CBinTable::X_SearchNegLimit(VOID)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		LONG lMotorDirectionX = GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_MOTOR_DIRECTION);				

		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X))
			{
				return Err_BinTableXPosLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X, BT_SP_NEG_LIMIT_X, &m_stBTAxis_X);

			if ( lMotorDirectionX == -1 )
			{
				CMS896AStn::MotionSearch(BT_AXIS_X, 1, SFM_WAIT, &m_stBTAxis_X, BT_SP_NEG_LIMIT_X);
			}
			else
			{
				CMS896AStn::MotionSearch(BT_AXIS_X, 0, SFM_WAIT, &m_stBTAxis_X, BT_SP_NEG_LIMIT_X);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
			nResult = Err_BinTableXNegLimit;
		}
	}

	if (!X_IsPowerOn() || (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X, &m_stBTAxis_X) != 0) )	//v4.18T1
	{
		return gnNOTOK;
	}

	GetCurXYPosn();
	return nResult;
}


INT CBinTable::X_Sync()
{
	INT nResult = gnOK;

	if (m_bDisableBT)		//v4.57A1
	{
		return nResult;
	}

	//if (m_fHardware && m_pServo_X && m_bSel_X)
	if (m_fHardware && m_bSel_X)
	{
		try
		{
			//m_pServo_X->Synchronize(10000);
			CMS896AStn::MotionSync(BT_AXIS_X, 10000, &m_stBTAxis_X);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult("BinTableXAxis");
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
			nResult = gnNOTOK;
		}	
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT X Sync");
	}

	return nResult;
}

INT CBinTable::X_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_bDisableBT)
	{
		return nResult;
	}

	//if (m_fHardware && m_pServo_X && m_bSel_X)
	if (m_fHardware && m_bSel_X)
	{
		try 
		{
			if ( bOn == TRUE )
			{
				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BT_AXIS_X, &m_stBTAxis_X);
					m_bIsPowerOn_X = TRUE;
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) == TRUE)
				{
					CMS896AStn::MotionPowerOff(BT_AXIS_X, &m_stBTAxis_X);
					m_bIsPowerOn_X = FALSE;
					m_bComm_X = FALSE;
					m_bHome_X = FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CBinTable::X_Comm()
{
	//if (m_fHardware && m_pServo_X && m_bSel_X)
	if (m_fHardware && m_bSel_X)
	{
		
		//INT nResult = CommutateServo("BinTableXAxis");
		INT nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_X, &m_stBTAxis_X);
		if ( nResult == gnOK )
		{
			m_bComm_X = TRUE;
		}
		else
		{
			nResult	= Err_BinTableXCommutate;
			//ClearServoError("srvBinTableX");
			CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);
			m_bComm_X = FALSE;
			m_bHome_X = FALSE;
		}
	}
	return gnOK;
}


////////////////////////////////////////////
//	Y Axis 
////////////////////////////////////////////
BOOL CBinTable::Y_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBT)	//v3.60
		return TRUE;
	if (CMS896AStn::m_bMS60NGPick)	//v4.57A1
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CBinTable::Y_Home()
{
	INT nResult			= gnOK;

	//if (m_fHardware && m_pServo_Y && m_bSel_Y)
	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			CMS896AStn::MotionPowerOff(BT_AXIS_Y, &m_stBTAxis_Y);
			Sleep(50);
			CMS896AStn::MotionResetController(BT_AXIS_Y, &m_stBTAxis_Y);

#ifdef NU_MOTION
			CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, 9999999, -9999999, &m_stBTAxis_Y);
#endif
			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) != 0)
			{
				m_bHome_Y = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
			}

			if (!m_bHome_Y)
			{
				m_bHome_Y = FALSE;

#ifdef NU_MOTION
				CMS896AStn::MotionSetPosition(BT_AXIS_Y, 0, &m_stBTAxis_Y);
#endif

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_Y, &m_stBTAxis_Y)) == gnOK)
				{
					m_bHome_Y = TRUE;
				}
				else
				{
					nResult	= Err_BinTableYCommutate;
					CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
				}
			}



			if (nResult == gnOK)
			{
				CMS896AStn::MotionPowerOn(BT_AXIS_Y, &m_stBTAxis_Y);
				Sleep(50);

				MotionLeaveHomeSensor(BT_AXIS_Y, &m_stBTAxis_Y);
				{
					nResult = CMS896AStn::MotionMoveHome(BT_AXIS_Y, 1, 0, &m_stBTAxis_Y);
				}

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) == 0) && 
					CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BT_AXIS_Y, 0, &m_stBTAxis_Y);
					m_bHome_Y	= TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_BinTableYMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);

			m_bComm_Y	= FALSE;
			nResult = Err_BinTableYMoveHome;
		}
	}
	else
	{
		if (m_bSel_Y)
		{
			Sleep(100);
			m_bComm_Y	= TRUE;
			m_bHome_Y	= TRUE;
		}
	}

	return nResult;
}

INT CBinTable::Y_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (!m_bHome_Y)
			{
				nResult	= gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_lY_ProfileType == MS896A_OBW_PROFILE)
				{
					if (abs(m_lCurYPosn - nPos) < 2000)
					{
						Y_SelectControl(PL_DYNAMIC);
					}
					else
					{
						Y_SelectControl(PL_DYNAMIC1);
					}
					CMS896AStn::MotionObwMoveTo(BT_AXIS_Y, nPos, SFM_NOWAIT, &m_stBTAxis_Y);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BT_AXIS_Y, nPos, SFM_NOWAIT, &m_stBTAxis_Y);
				}

				m_lCurYPosn = nPos;

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BT_AXIS_Y, 10000, &m_stBTAxis_Y);
					if (nResult != gnOK)
					{
						nResult = Err_BinTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);

			m_bHome_Y	= FALSE;
			nResult = Err_BinTableYMove;
		}
	}
	else
	{
		if (m_bSel_Y)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT Y Move to");
	}

	return nResult;
}

INT CBinTable::Y_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (!m_bHome_Y)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				m_lCurYPosn += nPos;

				if (m_lY_ProfileType == MS896A_OBW_PROFILE)
				{
					if (abs(nPos) < 2000)
					{
						Y_SelectControl(PL_DYNAMIC);
					}
					else
					{
						Y_SelectControl(PL_DYNAMIC1);
					}
					CMS896AStn::MotionObwMove(BT_AXIS_Y, nPos, SFM_NOWAIT, &m_stBTAxis_Y);
				}
				else
				{
					CMS896AStn::MotionMove(BT_AXIS_Y, nPos, SFM_NOWAIT, &m_stBTAxis_Y);
				}

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BT_AXIS_Y, 10000, &m_stBTAxis_Y);
					if (nResult != gnOK)
					{
						nResult = Err_BinTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);

			m_bHome_Y	= FALSE;
			nResult = Err_BinTableYMove;
		}
	}
	else
	{
		if (m_bSel_Y)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT Y Move");
	}

	return nResult;
}


INT CBinTable::Y_SearchPosLimit(VOID)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
			{
				return Err_BinTableYPosLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y, BT_SP_POS_LIMIT_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionSearch(BT_AXIS_Y, 1, SFM_WAIT, &m_stBTAxis_Y, BT_SP_POS_LIMIT_Y);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			nResult = Err_BinTableYPosLimit;
		}
	}

	if (!Y_IsPowerOn() || (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) != 0) )	//v4.18T1
	{
		return gnNOTOK;
	}

	GetCurXYPosn();
	return nResult;
}


INT CBinTable::Y_SearchNegLimit(VOID)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
			{
				return Err_BinTableYNegLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y, BT_SP_NEG_LIMIT_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionSearch(BT_AXIS_Y, 0,SFM_WAIT, &m_stBTAxis_Y, BT_SP_NEG_LIMIT_Y);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);

			nResult = Err_BinTableYNegLimit;
		}
	}

	if (!Y_IsPowerOn() || (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) != 0) )	//v4.18T1
	{
		return gnNOTOK;
	}

	GetCurXYPosn();
	return nResult;
}

INT CBinTable::Y_SearchBarcode(LONG lDir, LONG lSearchDist, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
			{
				return Err_BinTableYPosLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y, "spfBinTableYBarcode", &m_stBTAxis_Y);

			if ( lDir == 0 )
			{
				CMS896AStn::MotionUpdateSearchProfile(BT_AXIS_Y, "spfBinTableYBarcode", 2, -1 * lSearchDist, &m_stBTAxis_Y);
				CMS896AStn::MotionSearch(BT_AXIS_Y, 1, nMode, &m_stBTAxis_Y, "spfBinTableYBarcode");
			}
			else
			{
				CMS896AStn::MotionUpdateSearchProfile(BT_AXIS_Y, "spfBinTableYBarcode", 2, lSearchDist, &m_stBTAxis_Y);
				CMS896AStn::MotionSearch(BT_AXIS_Y, 1, nMode, &m_stBTAxis_Y, "spfBinTableYBarcode");
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			nResult = Err_BinTableYPosLimit;
		}
	}

	if (!Y_IsPowerOn() || (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) != 0) )	//v4.18T1
	{
		return gnNOTOK;
	}

	GetCurXYPosn();
	return nResult;
}

INT CBinTable::Y_Sync()
{
	INT nResult = gnOK;

	if (m_bDisableBT)	//v4.57A1
	{
		return nResult;
	}

	//if (m_fHardware && m_pServo_Y && m_bSel_Y)
	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			//m_pServo_Y->Synchronize(10000);
			CMS896AStn::MotionSync(BT_AXIS_Y, 10000, &m_stBTAxis_Y);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			nResult = gnNOTOK;
		}	
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT Y Sync");
	}

	return nResult;
}

INT CBinTable::Y_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	//if (m_fHardware && m_pServo_Y && m_bSel_Y)
	if (m_fHardware && m_bSel_Y)
	{
		try 
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BT_AXIS_Y, &m_stBTAxis_Y);
					m_bIsPowerOn_Y = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BT_AXIS_Y, &m_stBTAxis_Y);
					m_bIsPowerOn_Y = FALSE;
					m_bComm_Y = FALSE;
					m_bHome_Y = FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CBinTable::Y_Comm()
{
	//if (m_fHardware && m_pServo_Y && m_bSel_Y)
	if (m_fHardware && m_bSel_Y)
	{
		//INT nResult = CommutateServo("BinTableYAxis");
		INT nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_Y, &m_stBTAxis_Y);
		if ( nResult == gnOK )
		{
			m_bComm_Y = TRUE;
		}	
		else
		{
			nResult	= Err_BinTableYCommutate;
			//ClearServoError("srvBinTableY");
			CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
			m_bComm_Y = FALSE;
		}
	}
	return gnOK;
}


////////////////////////////////////////////
//	X2 Axis (9INCH_MS)		//v4.16T1
////////////////////////////////////////////
BOOL CBinTable::X2_IsPowerOn()
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableBT)
		return TRUE;
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return TRUE;
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CBinTable::X2_Home()
{
	INT nResult			= gnOK;
	/*
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_X2)
	{
		try
		{
			//Enlarge the softawre limit before Home
			CMS896AStn::MotionEnableProtection(BT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X2);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X2, 99999999, -99999999, &m_stBTAxis_X2);

			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X2, &m_stBTAxis_X2) != 0)
			{
				m_bComm_X2 = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);
			}

			if (!m_bComm_X2)
			{
				m_bComm_X2 = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_X2, &m_stBTAxis_X2)) == gnOK)
				{
					m_bComm_X2 = TRUE;
				}
				else
				{
					nResult	= Err_BinTableXCommutate;
					CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);
				}
			}

			if (nResult == gnOK)
			{
#ifdef NU_MOTION
				CMS896AStn::MotionPowerOn(BT_AXIS_X2, &m_stBTAxis_X2);

				//Search Negative Limit sensor 
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X2, BT_SP_NEG_LIMIT_X, &m_stBTAxis_X2);
				CMS896AStn::MotionSearch(BT_AXIS_X2, 0, SFM_WAIT, &m_stBTAxis_X2, BT_SP_NEG_LIMIT_X);

				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2))
				{
					CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X2, BT_SP_INDEX_X, &m_stBTAxis_X2);
					CMS896AStn::MotionSearch(BT_AXIS_X2, 1, SFM_WAIT, &m_stBTAxis_X2, BT_SP_INDEX_X);
				}

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X2, &m_stBTAxis_X2) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BT_AXIS_X2, 0, &m_stBTAxis_X2);
					m_bHome_X2 = TRUE;
					m_lCurX2Posn = 0;

					if ( (m_lTableX2NegLimit != 0) && (m_lTableX2PosLimit != 0) )
						CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_X2, m_lTableX2NegLimit, m_lTableX2PosLimit, &m_stBTAxis_X2);
					CMS896AStn::MotionEnableProtection(BT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X2);
				}
				else
				{
					CMS896AStn::MotionPowerOff(BT_AXIS_X2, &m_stBTAxis_X2);
					nResult = Err_BinTableXMoveHome;
				}
#else
				if ((nResult = CMS896AStn::MotionMoveHome(BT_AXIS_X2, 1, 0, &m_stBTAxis_X2)) == gnAMS_OK)
				{
					CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X2, "spfBinTableXIndex", &m_stBTAxis_X2);
					CMS896AStn::MotionSearch(BT_AXIS_X2, 1, SFM_WAIT, &m_stBTAxis_X2, "spfBinTableXIndex");

					Sleep(100);
					CMS896AStn::MotionSetPosition(BT_AXIS_X2, 0, &m_stBTAxis_X2);
					m_bHome_X2 = TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_BinTableXMoveHome;
				}
#endif
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
			CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);

			m_bComm_X2	= FALSE;
			nResult = Err_BinTableXMoveHome;
		}
	}
	else
	{
		if (m_bSel_X2)
		{
			Sleep(100);
			m_bComm_X2	= TRUE;
			m_bHome_X2	= TRUE;
		}
	}
	*/
	return nResult;
}


INT CBinTable::X2_MoveTo(INT nPos, INT nMode)
{
	INT nResult = gnOK;
	/*
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_X2)
	{
		try
		{
			if (!m_bHome_X2)
			{
				nResult	= gnNOTOK;
			}

			if (nResult == gnOK)
			{
				m_lCurX2Posn = nPos;
				CMS896AStn::MotionMoveTo(BT_AXIS_X2, nPos, SFM_NOWAIT, &m_stBTAxis_X2);

				if (nMode == SFM_WAIT)
				{
					//Klocwork
					nResult = CMS896AStn::MotionSync(BT_AXIS_X2, 10000, &m_stBTAxis_X2);
					if (nResult != gnOK)
					{
						nResult = Err_BinTableXMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
			CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);

			m_bHome_X2	= FALSE;
			nResult = Err_BinTableXMove;
		}
	}
	else
	{
		if (m_bSel_X2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT X2 MoveTo");
	}
	*/
	return nResult;
}

INT CBinTable::X2_Move(INT nPos, INT nMode)
{
	INT nResult = gnOK;
	/*
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_X2)
	{
		try
		{
			if (!m_bHome_X2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				m_lCurX2Posn += nPos;
				CMS896AStn::MotionMove(BT_AXIS_X2, nPos, SFM_NOWAIT, &m_stBTAxis_X2);

				if (nMode == SFM_WAIT)
				{
					//Klocwork
					nResult = CMS896AStn::MotionSync(BT_AXIS_X2, 10000, &m_stBTAxis_X2);
					if (nResult != gnOK)
					{
						nResult = Err_BinTableXMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
			CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);

			m_bHome_X2	= FALSE;
			nResult		= Err_BinTableXMove;
		}
	}
	else
	{
		if (m_bSel_X2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT X2 move");
	}
	*/
	return nResult;
}


INT CBinTable::X2_SearchPosLimit(VOID)
{
	INT nResult			= gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_X2)
	{
		LONG lMotorDirectionX = GetChannelInformation(MS896A_CFG_CH_BINTABLE2_X, MS896A_CFG_CH_MOTOR_DIRECTION);				

		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2))
			{
				return Err_BinTableXPosLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X2, BT_SP_POS_LIMIT_X, &m_stBTAxis_X2);

			if ( lMotorDirectionX == -1 )
			{
				CMS896AStn::MotionSearch(BT_AXIS_X2, 0, SFM_WAIT, &m_stBTAxis_X2, BT_SP_POS_LIMIT_X);
			}
			else
			{
				CMS896AStn::MotionSearch(BT_AXIS_X2, 1, SFM_WAIT, &m_stBTAxis_X2, BT_SP_POS_LIMIT_X);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
			nResult = Err_BinTableXPosLimit;
		}
	}

	GetCurXYPosn();

	return nResult;
}


INT CBinTable::X2_SearchNegLimit(VOID)
{
	INT nResult	= gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_X2)
	{
		LONG lMotorDirectionX = GetChannelInformation(MS896A_CFG_CH_BINTABLE2_X, MS896A_CFG_CH_MOTOR_DIRECTION);				

		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2))
			{
				return Err_BinTableXPosLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X2, BT_SP_NEG_LIMIT_X, &m_stBTAxis_X2);

			if ( lMotorDirectionX == -1 )
			{
				CMS896AStn::MotionSearch(BT_AXIS_X2, 1, SFM_WAIT, &m_stBTAxis_X2, BT_SP_NEG_LIMIT_X);
			}
			else
			{
				CMS896AStn::MotionSearch(BT_AXIS_X2, 0, SFM_WAIT, &m_stBTAxis_X2, BT_SP_NEG_LIMIT_X);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
			nResult = Err_BinTableXNegLimit;
		}
	}

	GetCurXYPosn();
	return nResult;
}

INT CBinTable::X2_Sync()
{
	INT nResult = gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_X2)
	{
		try
		{
			CMS896AStn::MotionSync(BT_AXIS_X2, 10000, &m_stBTAxis_X2);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
			nResult = gnNOTOK;
		}	
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT X2 Sync");
	}

	return nResult;
}

INT CBinTable::X2_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_X2)
	{
		try 
		{
			if ( bOn == TRUE )
			{
				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BT_AXIS_X2, &m_stBTAxis_X2);
					m_bIsPowerOn_X2 = TRUE;
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2) == TRUE)
				{
					CMS896AStn::MotionPowerOff(BT_AXIS_X2, &m_stBTAxis_X2);
					m_bIsPowerOn_X2 = FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}

INT CBinTable::X2_Comm()
{
	INT nResult = gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_X2)
	{
		nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_X2, &m_stBTAxis_X2);
		if ( nResult == gnOK )
		{
			m_bComm_X2 = TRUE;
		}
		else
		{
			nResult	= Err_BinTableXCommutate;
			CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);
			m_bComm_X2 = FALSE;
		}
	}

	return gnOK;
}


////////////////////////////////////////////
//	Y2 Axis  (9INCH_MS)		//v4.16T1
////////////////////////////////////////////

BOOL CBinTable::Y2_IsPowerOn()
{
	if (!m_fHardware)
		return TRUE;
	if (m_bDisableBT)	//v3.60
		return TRUE;
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return TRUE;

	try
	{
		return CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CBinTable::Y2_Home()
{
	INT nResult	= gnOK;
	/*
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			//Enlarge the softawre limit before Home
			CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y2);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, 99999999, -99999999, &m_stBTAxis_Y2);

			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y2, &m_stBTAxis_Y2) != 0)
			{
				m_bHome_Y2 = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
			}

			if (!m_bHome_Y2)
			{
				m_bHome_Y2 = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_Y2, &m_stBTAxis_Y2)) == gnOK)
				{
					m_bHome_Y2 = TRUE;
				}
				else
				{
					nResult	= Err_BinTableYCommutate;
					CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
				}
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2);

				//Search Positive Limit sensor 
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y2, BT_SP_NEG_LIMIT_Y, &m_stBTAxis_Y2);
				CMS896AStn::MotionSearch(BT_AXIS_Y2, 1, SFM_WAIT, &m_stBTAxis_Y2, BT_SP_NEG_LIMIT_Y);

				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
				{
					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y2, BT_SP_INDEX_Y, &m_stBTAxis_Y2);
					CMS896AStn::MotionSearch(BT_AXIS_Y2, 0, SFM_WAIT, &m_stBTAxis_Y2, BT_SP_INDEX_Y);
				}

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y2, &m_stBTAxis_Y2) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BT_AXIS_Y2, 0, &m_stBTAxis_Y2);
					m_bHome_Y2 = TRUE;
					m_lCurY2Posn = 0;

					if ( (m_lTableY2NegLimit != 0) && (m_lTableY2PosLimit != 0) )
						CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, m_lTableY2PosLimit, m_lTableY2NegLimit, &m_stBTAxis_Y2);
					CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y2);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2,	&m_stBTAxis_Y2);
			CMS896AStn::MotionClearError(BT_AXIS_Y2,	&m_stBTAxis_Y2);

			m_bComm_Y2	= FALSE;
			nResult		= Err_BinTableYMoveHome;
		}
	}
	else
	{
		if (m_bSel_Y2)
		{
			Sleep(100);
			m_bComm_Y2	= TRUE;
			m_bHome_Y2	= TRUE;
		}
	}
	*/
	return nResult;
}


INT CBinTable::Y2_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;
	/*
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			if (!m_bHome_Y2)
			{
				nResult	= gnNOTOK;
			}

			if (nResult == gnOK)
			{
				m_lCurY2Posn = nPos;
				CMS896AStn::MotionMoveTo(BT_AXIS_Y2, nPos, SFM_NOWAIT, &m_stBTAxis_Y2);

				if (nMode == SFM_WAIT)
				{
					//Klocwork
					nResult = CMS896AStn::MotionSync(BT_AXIS_Y2, 10000, &m_stBTAxis_Y2);
					if (nResult != gnOK)
					{
						nResult = Err_BinTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2,	&m_stBTAxis_Y2);
			CMS896AStn::MotionClearError(BT_AXIS_Y2,	&m_stBTAxis_Y2);

			m_bHome_Y2	= FALSE;
			nResult		= Err_BinTableYMove;
		}
	}
	else
	{
		if (m_bSel_Y2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT Y2 move to");
	}
	*/
	return nResult;
}

INT CBinTable::Y2_Move(INT nPos, INT nMode)
{
	INT nResult	= gnOK;
	/*
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			if (!m_bHome_Y2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				m_lCurY2Posn += nPos;
				CMS896AStn::MotionMove(BT_AXIS_Y2, nPos, SFM_NOWAIT, &m_stBTAxis_Y2);

				if (nMode == SFM_WAIT)
				{
					//Klocwork
					nResult = CMS896AStn::MotionSync(BT_AXIS_Y2, 10000, &m_stBTAxis_Y2);
					if (nResult != gnOK)
					{
						nResult = Err_BinTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);

			m_bHome_Y2	= FALSE;
			nResult		= Err_BinTableYMove;
		}
	}
	else
	{
		if (m_bSel_Y2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT Y2 move");
	}
	*/
	return nResult;
}


INT CBinTable::Y2_SearchPosLimit(VOID)
{
	INT nResult	= gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
			{
				return Err_BinTableYPosLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y2, BT_SP_POS_LIMIT_Y, &m_stBTAxis_Y2);
			CMS896AStn::MotionSearch(BT_AXIS_Y2, 1, SFM_WAIT, &m_stBTAxis_Y2, BT_SP_POS_LIMIT_Y);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
			nResult = Err_BinTableYPosLimit;
		}
	}

	GetCurXYPosn();
	return nResult;
}


INT CBinTable::Y2_SearchNegLimit(VOID)
{
	INT nResult = gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
			{
				return Err_BinTableYNegLimit;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y2, BT_SP_NEG_LIMIT_Y, &m_stBTAxis_Y2);
			CMS896AStn::MotionSearch(BT_AXIS_Y2, 0,SFM_WAIT, &m_stBTAxis_Y2, BT_SP_NEG_LIMIT_Y);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
			CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);

			nResult = Err_BinTableYNegLimit;
		}
	}
	GetCurXYPosn();
	return nResult;
}

INT CBinTable::Y2_Sync()
{
	INT nResult = gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			CMS896AStn::MotionSync(BT_AXIS_Y2, 10000, &m_stBTAxis_Y2);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
			nResult = gnNOTOK;
		}	
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT Y2 Sync");
	}

	return nResult;
}

INT CBinTable::Y2_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
	{
		return nResult;
	}

	if (m_fHardware && m_bSel_Y2)
	{
		try 
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2);
					m_bIsPowerOn_Y2 = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BT_AXIS_Y2, &m_stBTAxis_Y2);
					m_bIsPowerOn_Y2 = FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}

INT CBinTable::Y2_Comm()
{
	INT nResult = gnOK;

	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_Y2)
	{
		nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_Y2, &m_stBTAxis_Y2);
		if ( nResult == gnOK )
		{
			m_bComm_Y2 = TRUE;
		}	
		else
		{
			nResult	= Err_BinTableYCommutate;
			CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
			m_bComm_Y2 = FALSE;
		}
	}
	return gnOK;
}


////////////////////////////////////////////
//	Mutliple Axis 
////////////////////////////////////////////
INT CBinTable::Y12_Home()
{
	INT nResult = gnOK;

	LONG lOffsetY1 = 0;
	LONG lOffsetY2 = 0;

	//1. Home Y1 towards NEG Limit sensor
	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, 9999999, -9999999, &m_stBTAxis_Y);

			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) != 0)
			{
				m_bComm_Y = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
			}

			if (!m_bComm_Y)
			{
				m_bComm_Y = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_Y, &m_stBTAxis_Y)) == gnOK)
				{
					m_bComm_Y = TRUE;
				}
				else
				{
					return Err_BinTableYCommutate;
				}
			}

			if (nResult == gnOK)
			{
				//Y_SelectControl(PL_STATIC);
				CMS896AStn::MotionPowerOn(BT_AXIS_Y, &m_stBTAxis_Y);

				//Search Positive Limit sensor 
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y, BT_SP_NEG_LIMIT_Y, &m_stBTAxis_Y);
				CMS896AStn::MotionSearch(BT_AXIS_Y, 0, SFM_WAIT, &m_stBTAxis_Y, BT_SP_NEG_LIMIT_Y);

				//Y_SelectControl(PL_DYNAMIC);

				if ((CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) != 0) ||
					!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
				{
					m_bComm_Y	= FALSE;
					m_bHome_Y	= FALSE;
					return Err_BinTableYMoveHome;
				}

				m_bComm_Y	= TRUE;
				Sleep(100);
				CMS896AStn::MotionSetPosition(BT_AXIS_Y, 0, &m_stBTAxis_Y);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
			m_bComm_Y	= FALSE;
			m_bHome_Y	= FALSE;
			return Err_BinTableYMoveHome;
		}
	}
	else
	{
		if (m_bSel_Y)
		{
			m_bComm_Y	= TRUE;
		}
	}

	//2. Home Y2 towards POS Limit sensor
	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			//Enlarge the softawre limit before Home
			CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y2);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, 99999999, -99999999, &m_stBTAxis_Y2);

			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y2, &m_stBTAxis_Y2) != 0)
			{
				m_bComm_Y2 = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
			}

			if (!m_bComm_Y2)
			{
				m_bComm_Y2 = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_Y2, &m_stBTAxis_Y2)) == gnOK)
				{
					m_bComm_Y2 = TRUE;
				}
				else
				{
					return Err_BinTableYCommutate;
				}
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2);

				//Search Negative Limit sensor 
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y2, BT_SP_POS_LIMIT_Y, &m_stBTAxis_Y2);
				CMS896AStn::MotionSearch(BT_AXIS_Y2, 0, SFM_WAIT, &m_stBTAxis_Y2, BT_SP_POS_LIMIT_Y);


				if ((CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y2, &m_stBTAxis_Y2) != 0) ||
					!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
				{
					m_bComm_Y2	= FALSE;
					m_bHome_Y2	= FALSE;
					return Err_BinTableYMoveHome;
				}

				m_bComm_Y2	= TRUE;
				Sleep(100);
				CMS896AStn::MotionSetPosition(BT_AXIS_Y2, 0, &m_stBTAxis_Y2);

			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
			CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
			m_bComm_Y2	= FALSE;
			m_bHome_Y2	= FALSE;
			return Err_BinTableYMoveHome;
		}
	}
	else
	{
		if (m_bSel_Y2)
		{
			m_bComm_Y2	= TRUE;
		}
	}


	//3. Home Y1 towards INDEX sensor
	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (m_bComm_Y)
			{
				//Y_SelectControl(PL_STATIC);
				//Search Index sensor & Drive In
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y, BT_SP_INDEX_Y, &m_stBTAxis_Y);
				CMS896AStn::MotionSearch(BT_AXIS_Y, 1, SFM_WAIT, &m_stBTAxis_Y, BT_SP_INDEX_Y);

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
				{
					Sleep(100);
					lOffsetY1 = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_Y, 1, &m_stBTAxis_Y);
					CMS896AStn::MotionSetPosition(BT_AXIS_Y, 0, &m_stBTAxis_Y);
					m_bHome_Y = TRUE;
					m_lCurYPosn = 0;

					CString szLog;
					szLog.Format("BTY1 limit-INDEX offset = %ld", lOffsetY1);
					DisplayMessage(szLog);

					CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y);

					m_lTableYNegLimit = -1 * lOffsetY1 + 2000;		//v4.35T4

					if ( (m_lTableYNegLimit != 0) && (m_lTableYPosLimit != 0) )
					{
						CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, m_lTableYPosLimit, m_lTableYNegLimit, &m_stBTAxis_Y);
						szLog.Format("BTY Limit (HOME) : U=%ld, L=%ld", m_lTableYPosLimit, m_lTableYNegLimit);
						CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
					}
					else
					{
						CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, 9999999, -9999999, &m_stBTAxis_Y);
						CMSLogFileUtility::Instance()->MS_LogOperation("BTY Limit (HOME) : DEFAULT");
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
			m_bComm_Y	= FALSE;
			m_bHome_Y	= FALSE;
			return Err_BinTableYMoveHome;
		}

		if ( (m_lBTUnloadPos_Y < (-lOffsetY1 + BT_DUAL_TABLE_PARKING_OFFSET_Y + 100000)) &&
			 (m_lBTUnloadPos_Y > (-lOffsetY1 + BT_DUAL_TABLE_PARKING_OFFSET_Y)) )
		{
			Y_Profile(LOW_PROF1);
			Y_MoveTo(m_lBTUnloadPos_Y);				//v4.35T1	//PLLM MS109
			Y_Profile(NORMAL_PROF);
		}
		else if (m_lTableYNegLimit < 0)
		{
			Y_Profile(LOW_PROF1);
			Y_MoveTo(m_lTableYNegLimit + BT_DUAL_TABLE_PARKING_OFFSET_Y);
			Y_Profile(NORMAL_PROF);
		}
		else
		{
			Y_Profile(LOW_PROF1);
			Y_MoveTo(-lOffsetY1 + BT_DUAL_TABLE_PARKING_OFFSET_Y);
			Y_Profile(NORMAL_PROF);
		}

	}
	else
	{
		if (m_bSel_Y)
		{
			Sleep(100);
			m_bHome_Y = TRUE;
		}
	}

	//5. Home Y2 towards INDEX sensor
	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			if (m_bComm_Y2)
			{
				//Search Index sensor & Drive In
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y2, BT_SP_INDEX_Y, &m_stBTAxis_Y2);
				CMS896AStn::MotionSearch(BT_AXIS_Y2, 1, SFM_WAIT, &m_stBTAxis_Y2, BT_SP_INDEX_Y);

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y2, &m_stBTAxis_Y2) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
				{
					Sleep(100);
					lOffsetY2 = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_Y2, 1, &m_stBTAxis_Y2);
					CMS896AStn::MotionSetPosition(BT_AXIS_Y2, 0, &m_stBTAxis_Y2);
					m_bHome_Y2 = TRUE;
					m_lCurY2Posn = 0;

					CString szLog;
					szLog.Format("BTY2 limit-INDEX offset = %ld", lOffsetY2);
					DisplayMessage(szLog);

					CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y2);

					m_lTableY2PosLimit = -1 * lOffsetY2 - 2000;		//v4.35T4

					if ( (m_lTableY2NegLimit != 0) && (m_lTableY2PosLimit != 0) )
					{
						CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, m_lTableY2PosLimit, m_lTableY2NegLimit, &m_stBTAxis_Y2);			
						//CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, 999999, -999999, &m_stBTAxis_Y2);
						szLog.Format("BTY2 Limit (HOME) : U=%ld, L=%ld", m_lTableY2PosLimit, m_lTableY2NegLimit);
						CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
					}
					else
					{
						CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, 999999, -999999, &m_stBTAxis_Y2);
						CMSLogFileUtility::Instance()->MS_LogOperation("BTY2 Limit (HOME) : DEFAULT");
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
			CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
			m_bComm_Y2	= FALSE;
			m_bHome_Y2	= FALSE;
			return Err_BinTableYMoveHome;
		}

		/*
		//6. Move X2 towards SAFE position
		if (m_lTableY2NegLimit < 0)
		{
			Y2_Profile(LOW_PROF1);
			Y2_MoveTo(m_lTableY2NegLimit + 20000);
			Y2_Profile(NORMAL_PROF);
		}
		else
		{
			Y2_Profile(LOW_PROF1);
			//if (labs(lOffsetX2) > 200000)
			//	X2_Move(-500000);
			//else
			Y2_Move(-1 * labs(lOffsetY2) + 20000);
			Y2_Profile(NORMAL_PROF);
		}
		*/
		if ( (m_lBTUnloadPos_Y2 > (abs(lOffsetY2) - BT_DUAL_TABLE_PARKING_OFFSET_Y - 100000)) &&
			 (m_lBTUnloadPos_Y2 < (abs(lOffsetY2) - BT_DUAL_TABLE_PARKING_OFFSET_Y)) )
		{
			Y2_Profile(LOW_PROF1);
			Y2_MoveTo(m_lBTUnloadPos_Y2);		//v4.35T1	//PLLM MS109
			Y2_Profile(NORMAL_PROF);
		}
		else if (m_lTableY2PosLimit > 0)
		{
			Y2_Profile(LOW_PROF1);
			Y2_MoveTo(m_lTableY2PosLimit - BT_DUAL_TABLE_PARKING_OFFSET_Y);
			Y2_Profile(NORMAL_PROF);
		}
		else
		{
			Y2_Profile(LOW_PROF1);
			Y2_MoveTo(abs(lOffsetY2) - BT_DUAL_TABLE_PARKING_OFFSET_Y);
			Y2_Profile(NORMAL_PROF);
		}
	}
	else
	{
		if (m_bSel_Y2)
		{
			Sleep(100);
			m_bHome_Y2	= TRUE;
		}
	}

	return nResult;
}


INT CBinTable::XY_Home()
{
	INT nResult			= gnOK;
	CString szCurrentAxis = "";

	//if (m_fHardware && m_pServo_X && m_pServo_Y && m_bSel_X && m_bSel_Y)
	if (m_fHardware && m_bSel_X && m_bSel_Y)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X, &m_stBTAxis_X) != 0)
			{
				m_bComm_X = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);
			}

			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) != 0)
			{
				m_bHome_Y = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
			}

			if (!m_bComm_X)
			{
				m_bComm_X = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_X, &m_stBTAxis_X)) == gnOK)
				{
					m_bComm_X = TRUE;
				}
				else
				{
					nResult	= Err_BinTableXCommutate;
					CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);
				}
			}

			if (!m_bComm_Y && (nResult == gnOK))
			{
				m_bHome_Y = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_Y, &m_stBTAxis_Y)) == gnOK)
				{
					m_bComm_Y = TRUE;
				}
				else
				{
					nResult	= Err_BinTableYCommutate;
					CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
				}
			}

			if (nResult == gnOK)
			{
				if (CMS896AApp::m_bIsPrototypeMachine == FALSE)
				{
					X_SelectControl(PL_STATIC);
					Y_SelectControl(PL_STATIC);
				}

				//X axis
				CMS896AStn::MotionPowerOn(BT_AXIS_X, &m_stBTAxis_X);

				//Search Positive Limit sensor 
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X, BT_SP_POS_LIMIT_X, &m_stBTAxis_X);
				CMS896AStn::MotionSearch(BT_AXIS_X, 0, SFM_WAIT, &m_stBTAxis_X, BT_SP_POS_LIMIT_X);

				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X))
				{
					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X, BT_SP_INDEX_X, &m_stBTAxis_X);
					CMS896AStn::MotionSearch(BT_AXIS_X, 1, SFM_WAIT, &m_stBTAxis_X, BT_SP_INDEX_X);
				}

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X, &m_stBTAxis_X) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BT_AXIS_X, 0, &m_stBTAxis_X);
					m_bHome_X = TRUE;
					m_lCurXPosn = 0;
				}

				//Y axis
				CMS896AStn::MotionPowerOn(BT_AXIS_Y, &m_stBTAxis_Y);

				//Search Positive Limit sensor 
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y, BT_SP_NEG_LIMIT_Y, &m_stBTAxis_Y);
				CMS896AStn::MotionSearch(BT_AXIS_Y, 1, SFM_WAIT, &m_stBTAxis_Y, BT_SP_NEG_LIMIT_Y);

				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
				{
					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y, BT_SP_INDEX_Y, &m_stBTAxis_Y);
					CMS896AStn::MotionSearch(BT_AXIS_Y, 0, SFM_WAIT, &m_stBTAxis_Y, BT_SP_INDEX_Y);
				}

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y, &m_stBTAxis_Y) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BT_AXIS_Y, 0, &m_stBTAxis_Y);
					m_bHome_Y = TRUE;
					m_lCurYPosn = 0;
				}

				if (CMS896AApp::m_bIsPrototypeMachine == FALSE)
				{
					X_SelectControl(PL_DYNAMIC);
					Y_SelectControl(PL_DYNAMIC);
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult(szCurrentAxis);
			if (szCurrentAxis == BT_AXIS_X)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_X);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_Y);

			CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);
			CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);

			m_bComm_X	= FALSE;
			m_bComm_Y	= FALSE;

			nResult = Err_BinTableXMoveHome;
		}
	}
	else
	{
		Sleep(100);
		m_bComm_X	= TRUE;
		m_bComm_Y	= TRUE;
		m_bHome_X	= TRUE;
		m_bHome_Y	= TRUE;
	}

	return nResult;
}


INT CBinTable::XY_MoveTo(INT nPosX, INT nPosY, INT nMode)
{
	INT nResult			= gnOK;
	CString szCurrentAxis = "";

	//if (m_fHardware && m_pServo_X && m_pServo_Y && m_bSel_X && m_bSel_Y)
	//if (m_fHardware && m_bSel_X && m_bSel_Y)
	SelectXYProfile(nPosX - m_lCurXPosn, nPosY - m_lCurYPosn);
	if (State() == IDLE_Q)
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
	}

	
	if (m_fHardware)
	{
		try
		{
			if (!m_bHome_X || !m_bHome_Y)
			{
				nResult	= gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_bSel_X && (nPosX != BT_NOT_MOVE))
				{
					m_lCurXPosn = nPosX;
					szCurrentAxis = BT_AXIS_X;

					if (m_lX_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMoveTo(BT_AXIS_X, nPosX, SFM_NOWAIT, &m_stBTAxis_X);
					}
					else
					{
						CMS896AStn::MotionMoveTo(BT_AXIS_X, nPosX, SFM_NOWAIT, &m_stBTAxis_X);
					}
				}

				if (m_bSel_Y && (nPosY != BT_NOT_MOVE))
				{
					m_lCurYPosn = nPosY;
					szCurrentAxis = BT_AXIS_Y;

					if (m_lY_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMoveTo(BT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stBTAxis_Y);
					}
					else
					{
						CMS896AStn::MotionMoveTo(BT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stBTAxis_Y);
					}
				}

				if (nMode == SFM_WAIT)
				{
					if (m_bSel_X && (nPosX != BT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(BT_AXIS_X, 10000, &m_stBTAxis_X)) != gnOK)
						{
							nResult = Err_BinTableXMove;
						}
					}

					if (m_bSel_Y && (nPosY != BT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(BT_AXIS_Y, 10000, &m_stBTAxis_Y)) != gnOK)
						{
							nResult = Err_BinTableYMove;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult(szCurrentAxis);
			if (szCurrentAxis == BT_AXIS_X)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_X);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_Y);

			if (m_bSel_X && (nPosX != BT_NOT_MOVE))
			{
				//ClearServoError("srvBinTableX");
				CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);
				m_bHome_X	= FALSE;
			}

			if (m_bSel_Y && (nPosY != BT_NOT_MOVE))
			{
				//ClearServoError("srvBinTableY");
				CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
				m_bHome_Y	= FALSE;
			}

			nResult	= Err_BinTableXYMove;
		}
	}
	else
	{
		if (nMode == SFM_WAIT)
		{
			Sleep(30);
		}
		else
		{
			Sleep(1);
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT XY move to");
	}

	return nResult;
}

INT CBinTable::XY_Move(INT nPosX, INT nPosY, INT nMode)
{
	INT nResult			= gnOK;
	CString szCurrentAxis = "";

	SelectXYProfile(nPosX, nPosY);
	//if (m_fHardware && m_pServo_X && m_pServo_Y && m_bSel_X && m_bSel_Y)
	//if (m_fHardware && m_bSel_X && m_bSel_Y)
	if (m_fHardware)	//v3.67T5
	{
		try
		{
			if (!m_bHome_X || !m_bHome_Y)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_bSel_X && (nPosX != BT_NOT_MOVE))
				{
					m_lCurXPosn += nPosX;
					szCurrentAxis = BT_AXIS_X;

					if (m_lX_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMove(BT_AXIS_X, nPosX, SFM_NOWAIT, &m_stBTAxis_X);
					}
					else
					{
						CMS896AStn::MotionMove(BT_AXIS_X, nPosX, SFM_NOWAIT, &m_stBTAxis_X);
					}
				}

				if (m_bSel_Y && (nPosY != BT_NOT_MOVE))
				{
					m_lCurYPosn += nPosY;
					szCurrentAxis = BT_AXIS_Y;

					if (m_lY_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMove(BT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stBTAxis_Y);
					}
					else
					{
						CMS896AStn::MotionMove(BT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stBTAxis_Y);
					}
				}

				if (nMode == SFM_WAIT)
				{
					if (m_bSel_X && (nPosX != BT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(BT_AXIS_X, 10000, &m_stBTAxis_X)) != gnOK)
						{
							nResult = Err_BinTableXMove;
						}
					}

					if (m_bSel_Y && (nPosY != BT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(BT_AXIS_Y, 10000, &m_stBTAxis_Y)) != gnOK)
						{
							nResult = Err_BinTableYMove;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult(szCurrentAxis);
			if (szCurrentAxis == BT_AXIS_X)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_X);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_Y);

			if (m_bSel_X && (nPosX != BT_NOT_MOVE))
			{
				CMS896AStn::MotionClearError(BT_AXIS_X, &m_stBTAxis_X);
				m_bHome_X	= FALSE;
			}

			if (m_bSel_Y && (nPosY != BT_NOT_MOVE))
			{
				CMS896AStn::MotionClearError(BT_AXIS_Y, &m_stBTAxis_Y);
				m_bHome_Y	= FALSE;
			}

			nResult	= Err_BinTableXYMove;
		}
	}
	else
	{
		if (nMode == SFM_WAIT)
		{
			Sleep(30);
		}
		else
		{
			Sleep(1);
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT XY move");
	}

	return nResult;
}



////////////////////////////////////////////
//	Mutliple Axis XY2 (9INCH_MS)	//v4.16T1
////////////////////////////////////////////

INT CBinTable::XY2_Home()
{
	INT nResult			= gnOK;
	CString szCurrentAxis = "";
	/*
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware && m_bSel_X2 && m_bSel_Y2)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X2, &m_stBTAxis_X2) != 0)
			{
				m_bComm_X2 = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);
			}

			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y2, &m_stBTAxis_Y2) != 0)
			{
				m_bHome_Y2 = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
			}

			if (!m_bComm_X2)
			{
				m_bComm_X2 = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_X2, &m_stBTAxis_X2)) == gnOK)
				{
					m_bComm_X2 = TRUE;
				}
				else
				{
					nResult	= Err_BinTableXCommutate;
					CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);
				}
			}

			if (!m_bComm_Y2 && (nResult == gnOK))
			{
				m_bHome_Y2 = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_Y2, &m_stBTAxis_Y2)) == gnOK)
				{
					m_bComm_Y2 = TRUE;
				}
				else
				{
					nResult	= Err_BinTableYCommutate;
					CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
				}
			}

			if (nResult == gnOK)
			{
#ifdef NU_MOTION
				//X axis
				CMS896AStn::MotionPowerOn(BT_AXIS_X2, &m_stBTAxis_X2);

				//Search Positive Limit sensor 
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X2, BT_SP_POS_LIMIT_X, &m_stBTAxis_X2);
				CMS896AStn::MotionSearch(BT_AXIS_X2, 0, SFM_WAIT, &m_stBTAxis_X2, BT_SP_POS_LIMIT_X);

				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2))
				{
					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X2, BT_SP_INDEX_X, &m_stBTAxis_X2);
					CMS896AStn::MotionSearch(BT_AXIS_X2, 1, SFM_WAIT, &m_stBTAxis_X2, BT_SP_INDEX_X);
				}

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_X2, &m_stBTAxis_X2) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BT_AXIS_X2, 0, &m_stBTAxis_X2);
					m_bHome_X2 = TRUE;
					m_lCurX2Posn = 0;
				}

				//Y axis
				CMS896AStn::MotionPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2);

				//Search Positive Limit sensor 
				CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y2, BT_SP_NEG_LIMIT_Y, &m_stBTAxis_Y2);
				CMS896AStn::MotionSearch(BT_AXIS_Y2, 1, SFM_WAIT, &m_stBTAxis_Y2, BT_SP_NEG_LIMIT_Y);

				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
				{
					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y2, BT_SP_INDEX_Y, &m_stBTAxis_Y2);
					CMS896AStn::MotionSearch(BT_AXIS_Y2, 0, SFM_WAIT, &m_stBTAxis_Y2, BT_SP_INDEX_Y);
				}

				if ( (CMS896AStn::MotionReportErrorStatus(BT_AXIS_Y2, &m_stBTAxis_Y2) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BT_AXIS_Y2, 0, &m_stBTAxis_Y2);
					m_bHome_Y2 = TRUE;
					m_lCurY2Posn = 0;
				}

#else
				if ((nResult = CMS896AStn::MotionMoveHome(BT_AXIS_X2, SFM_NOWAIT, 0, &m_stBTAxis_X2)) == gnOK)
				{
					if ((nResult = CMS896AStn::MotionMoveHome(BT_AXIS_Y2, 1, 0, &m_stBTAxis_Y2)) != gnOK)
					{
						nResult = Err_BinTableYMoveHome;
					}
				}
				else
				{
					nResult = Err_BinTableXMoveHome;
				}

				if (nResult == gnOK)
				{
					if ((nResult = CMS896AStn::MotionSync(BT_AXIS_X2, 10000, &m_stBTAxis_X2)) == gnOK)
					{
						szCurrentAxis = BT_AXIS_X2;
						CMS896AStn::MotionSelectSearchProfile(BT_AXIS_X2, "spfBinTableXIndex", &m_stBTAxis_X2);
						CMS896AStn::MotionSearch(BT_AXIS_X2, 1, SFM_WAIT, &m_stBTAxis_X2, "spfBinTableXIndex");

						Sleep(100);
						CMS896AStn::MotionSetPosition(BT_AXIS_X2, 0, &m_stBTAxis_X2);
						m_bHome_X2	= TRUE;

						szCurrentAxis = BT_AXIS_Y2;
						CMS896AStn::MotionSelectSearchProfile(BT_AXIS_Y2, "spfBinTableYIndex", &m_stBTAxis_Y2);
						CMS896AStn::MotionSearch(BT_AXIS_Y2, 1, SFM_WAIT, &m_stBTAxis_Y2, "spfBinTableYIndex");

						Sleep(100);
						CMS896AStn::MotionSetPosition(BT_AXIS_Y2, 0, &m_stBTAxis_Y2);
						m_bHome_Y2	= TRUE;
					}
					else
					{
						nResult = Err_BinTableXMoveHome;
					}
				}
#endif
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult(szCurrentAxis);
			if (szCurrentAxis == BT_AXIS_X2)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_X);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_Y);

			CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);
			CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);

			m_bComm_X2	= FALSE;
			m_bComm_Y2	= FALSE;

			nResult = Err_BinTableXMoveHome;
		}
	}
	else
	{
		Sleep(100);
		m_bComm_X2	= TRUE;
		m_bComm_Y2	= TRUE;
		m_bHome_X2	= TRUE;
		m_bHome_Y2	= TRUE;
	}
	*/
	return nResult;
}

INT CBinTable::XY2_MoveTo(INT nPosX, INT nPosY, INT nMode)
{
	INT nResult	= gnOK;
	CString szCurrentAxis = "";
	/*
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware)	
	{
		try
		{
			if (!m_bHome_X2 || !m_bHome_Y2)
			{
				nResult	= gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_bSel_X2 && (nPosX != BT_NOT_MOVE))
				{
					m_lCurX2Posn = nPosX;
					szCurrentAxis = BT_AXIS_X2;
					CMS896AStn::MotionMoveTo(BT_AXIS_X2, nPosX, SFM_NOWAIT, &m_stBTAxis_X2);
				}

				if (m_bSel_Y2 && (nPosY != BT_NOT_MOVE))
				{
					m_lCurY2Posn = nPosY;
					szCurrentAxis = BT_AXIS_Y2;
					CMS896AStn::MotionMoveTo(BT_AXIS_Y2, nPosY, SFM_NOWAIT, &m_stBTAxis_Y2);
				}

				if (nMode == SFM_WAIT)
				{
					if (m_bSel_X2 && (nPosX != BT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(BT_AXIS_X2, 1000, &m_stBTAxis_X2)) != gnOK)
						{
							nResult = Err_BinTableXMove;
						}
					}

					if (m_bSel_Y2 && (nPosY != BT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(BT_AXIS_Y2, 1000, &m_stBTAxis_Y2)) != gnOK)
						{
							nResult = Err_BinTableYMove;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			if (szCurrentAxis == BT_AXIS_X2)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_X2);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_Y2);

			if (m_bSel_X2 && (nPosX != BT_NOT_MOVE))
			{
				CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);
				m_bHome_X2	= FALSE;
			}

			if (m_bSel_Y2 && (nPosY != BT_NOT_MOVE))
			{
				CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
				m_bHome_Y2	= FALSE;
			}

			nResult	= Err_BinTableXYMove;
		}
	}
	else
	{
		if (nMode == SFM_WAIT)
		{
			Sleep(30);
		}
		else
		{
			Sleep(1);
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT XY2 move to");
	}
	*/
	return nResult;
}

INT CBinTable::XY2_Move(INT nPosX, INT nPosY, INT nMode)
{
	INT nResult	= gnOK;
	CString szCurrentAxis = "";
	/*
	if (m_ulMachineType != BT_MACHTYPE_DTABLE)
		return nResult;

	if (m_fHardware)
	{
		try
		{
			if (!m_bHome_X2 || !m_bHome_Y2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_bSel_X2 && (nPosX != BT_NOT_MOVE))
				{
					m_lCurX2Posn += nPosY;
					szCurrentAxis = BT_AXIS_X2;
					CMS896AStn::MotionMove(BT_AXIS_X2, nPosX, SFM_NOWAIT, &m_stBTAxis_X2);
				}

				if (m_bSel_Y2 && (nPosY != BT_NOT_MOVE))
				{
					m_lCurY2Posn += nPosY;
					szCurrentAxis = BT_AXIS_Y2;
					CMS896AStn::MotionMove(BT_AXIS_Y2, nPosY, SFM_NOWAIT, &m_stBTAxis_Y2);
				}

				if (nMode == SFM_WAIT)
				{
					if (m_bSel_X2 && (nPosX != BT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(BT_AXIS_X2, 1000, &m_stBTAxis_X2)) != gnOK)
						{
							nResult = Err_BinTableXMove;
						}
					}

					if (m_bSel_Y2 && (nPosY != BT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(BT_AXIS_Y2, 1000, &m_stBTAxis_Y2)) != gnOK)
						{
							nResult = Err_BinTableYMove;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			if (szCurrentAxis == BT_AXIS_X2)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_X2);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_Y2);

			if (m_bSel_X2 && (nPosX != BT_NOT_MOVE))
			{
				CMS896AStn::MotionClearError(BT_AXIS_X2, &m_stBTAxis_X2);
				m_bHome_X2	= FALSE;
			}

			if (m_bSel_Y2 && (nPosY != BT_NOT_MOVE))
			{
				CMS896AStn::MotionClearError(BT_AXIS_Y2, &m_stBTAxis_Y2);
				m_bHome_Y2	= FALSE;
			}

			nResult	= Err_BinTableXYMove;
		}
	}
	else
	{
		if (nMode == SFM_WAIT)
		{
			Sleep(30);
		}
		else
		{
			Sleep(1);
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT XY2 move");
	}
	*/
	return nResult;
}



// Turn on or off the joystick
// Remark: After joystick off, the motors are still power on
VOID CBinTable::SetJoystickOn(BOOL bOn, BOOL bCheckLimit, BOOL bUserBT2)			
{
	CString szCurrentAxis = "";

	if ( !m_fHardware )
		return;
	if (m_bDisableBT)		//v3.60
		return;

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		/*if (bUserBT2 && CMS896AApp::m_bMS100Plus9InchOption)
		{
			m_lJsBinTableInUse = 1;	//BT2
		}
		else
		{*/
		m_lJsBinTableInUse = 0;		//BT1
		//}

		m_lJsTableMode	= 1;		//BT
		CMS896AStn::m_bJoystickOn = bOn;
		m_bXJoystickOn	= bOn;
		m_bYJoystickOn	= bOn;

		if( IsMS90() )
		{
			(*m_psmfSRam)["MS896A"]["Current Camera"] = 1;
			IPC_CServiceMessage stMsg;
			stMsg.InitMessage(sizeof(BOOL), &bOn);
			int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetMouseControlCmd", stMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bOn);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}
		return;
	}

	try
	{
		if (bOn == TRUE)		// Turn on
		{					
			if ( m_bSel_X )
			{
				szCurrentAxis = BT_AXIS_X;

				if ( CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) == FALSE ) 
					CMS896AStn::MotionPowerOn(BT_AXIS_X, &m_stBTAxis_X);

				//Do not switch to static control for Cree MS896-DL only
				if (!CMS896AApp::m_bNoStaticControlForBTJoystick)
				{
					//v3.65		//Use static control for joystick profile
					if ( CMS896AStn::MotionSetControlParam(BT_AXIS_X, BT_STATIC_CONTROL_X, &m_stBTAxis_X) )
					{
						CMS896AStn::MotionSelectControlParam(BT_AXIS_X, BT_STATIC_CONTROL_X, &m_stBTAxis_X);
					}
				}
					
				if (bCheckLimit)
				{
					if ( (m_lTableXNegLimit != 0) && (m_lTableXPosLimit != 0) )
					{
						CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_X, m_lTableXNegLimit, m_lTableXPosLimit, &m_stBTAxis_X);
					}
				}
				CMS896AStn::MotionJoyStickOn(BT_AXIS_X, bOn, &m_stBTAxis_X);

			}

			if ( m_bSel_Y )
			{
				szCurrentAxis = BT_AXIS_Y;

				if ( CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y) == FALSE ) 
					CMS896AStn::MotionPowerOn(BT_AXIS_Y, &m_stBTAxis_Y);

				//Do not switch to static control for Cree MS896-DL only
				if (!CMS896AApp::m_bNoStaticControlForBTJoystick)
				{
					//v3.65		//Use static control for joystick profile
					if ( CMS896AStn::MotionSetControlParam(BT_AXIS_Y, BT_STATIC_CONTROL_Y, &m_stBTAxis_Y) )
					{
						CMS896AStn::MotionSelectControlParam(BT_AXIS_Y, BT_STATIC_CONTROL_Y, &m_stBTAxis_Y);
					}
				}

				if (bCheckLimit)
				{
					if ( (m_lTableYNegLimit != 0) && (m_lTableYPosLimit != 0) )
					{
						CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_Y, m_lTableYNegLimit, m_lTableYPosLimit, &m_stBTAxis_Y);
					}
				}
				CMS896AStn::MotionJoyStickOn(BT_AXIS_Y, bOn, &m_stBTAxis_Y);
			}
		}
		else					// Turn off
		{
			// Even turn off the joystick, don't turn off the motor power
			szCurrentAxis = BT_AXIS_X;
			//m_pServo_X->JoystickOff();
			CMS896AStn::MotionJoyStickOn(BT_AXIS_X, bOn, &m_stBTAxis_X);
			
			//Do not switch to static control for Cree MS896-DL only
			if (!CMS896AApp::m_bNoStaticControlForBTJoystick)
			{
				if ( CMS896AStn::MotionSetControlParam(BT_AXIS_X, BT_DYNAMIC_CONTROL_X, &m_stBTAxis_X) )
				{
					CMS896AStn::MotionSelectControlParam(BT_AXIS_X, BT_DYNAMIC_CONTROL_X, &m_stBTAxis_X);
				}
			}

			szCurrentAxis = BT_AXIS_Y;
			//m_pServo_Y->JoystickOff();
			CMS896AStn::MotionJoyStickOn(BT_AXIS_Y, bOn, &m_stBTAxis_Y);

			//Do not switch to static control for Cree MS896-DL only
			if (!CMS896AApp::m_bNoStaticControlForBTJoystick)
			{
				if ( CMS896AStn::MotionSetControlParam(BT_AXIS_Y, BT_DYNAMIC_CONTROL_Y, &m_stBTAxis_Y) )
				{
					CMS896AStn::MotionSelectControlParam(BT_AXIS_Y, BT_DYNAMIC_CONTROL_Y, &m_stBTAxis_Y);
				}
			}
		}

		m_bJoystickOn = bOn;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//CheckHiPECResult(szCurrentAxis);
		if (szCurrentAxis == BT_AXIS_X)
			CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_X);
		else
			CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stBTAxis_Y);
		//throw;	//v3.94
	}
}

BOOL CBinTable::HomeTable1()
{
	if (X_IsPowerOn())
	{
		X_Profile(LOW_PROF1);
		//Y_MoveTo(0);

		if (m_lTableXNegLimit != 0)
		{
			X_MoveTo(m_lTableXNegLimit + BT_DUAL_TABLE_PARKING_OFFSET_X);		//v4.24T8	//Avoid collision with DBH
		}
		else
		{
			X_MoveTo(BT_DUAL_TABLE_PARKING_DEFAULT_POSX);						//v4.24T8	//DEFAULT parking pos to avoid collision with DBH
		}

		X_Profile(NORMAL_PROF);
	}
	else
	{
		X_Home();
		Sleep(200);
		
		if (m_lTableXNegLimit != 0)
		{
			X_MoveTo(m_lTableXNegLimit + BT_DUAL_TABLE_PARKING_OFFSET_X);		//v4.24T8	//Avoid collision with DBH
		}
		else
		{
			X_MoveTo(BT_DUAL_TABLE_PARKING_DEFAULT_POSX);						//v4.24T8	//DEFAULT parking pos to avoid collision with DBH
		}
	}

	if (Y_IsPowerOn() && (m_lTableYNegLimit < 0))
	{
		Y_Profile(LOW_PROF1);

		if ( (m_lTableXNegLimit < -100000) &&
			 (m_lBTUnloadPos_Y < (m_lTableYNegLimit + 100000)) && 
			 (m_lBTUnloadPos_Y > m_lTableYNegLimit) )	
		{
			Y_MoveTo(m_lBTUnloadPos_Y);						//v4.35T1	//PLLM MS109
		}
		else
		{
			Y_MoveTo(m_lTableYNegLimit + BT_DUAL_TABLE_PARKING_OFFSET_Y);
		}

		Y_Profile(NORMAL_PROF);
	}
	else
	{
		Y12_Home();
	}

	return TRUE;
}

BOOL CBinTable::HomeTable2()
{
	/*
	if (!m_bUseDualTablesOption)
		return FALSE;

	if (X2_IsPowerOn())
	{
		X2_Profile(LOW_PROF1);
		//Y2_MoveTo(0);
		if (m_lTableX2NegLimit != 0)
		{
			X2_MoveTo(m_lTableX2NegLimit + BT_DUAL_TABLE_PARKING_OFFSET_X);		//v4.24T8	//Avoid collision with DBH
		}
		else
		{
			X2_MoveTo(BT_DUAL_TABLE2_PARKING_DEFAULT_POSX);						//v4.24T8	//DEFAULT parking pos to avoid collision with DBH
		}

		X2_Profile(NORMAL_PROF);
	}
	else
	{
		X2_Home();
		Sleep(200);
		if (m_lTableX2NegLimit != 0)
		{
			X2_MoveTo(m_lTableX2NegLimit + BT_DUAL_TABLE_PARKING_OFFSET_X);		//v4.24T8	//Avoid collision with DBH
		}
		else
		{
			X2_MoveTo(BT_DUAL_TABLE2_PARKING_DEFAULT_POSX);						//v4.24T8	//DEFAULT parking pos to avoid collision with DBH
		}
	}

	if (Y2_IsPowerOn() && (m_lTableY2PosLimit > 0))
	{
		Y2_Profile(LOW_PROF1);

		if ( (m_lTableY2PosLimit > 100000) &&
			 (m_lBTUnloadPos_Y2 > (m_lTableY2PosLimit - 100000)) && 
			 (m_lBTUnloadPos_Y2 < m_lTableY2PosLimit) )							
		{
			Y2_MoveTo(m_lBTUnloadPos_Y2);				//v4.35T1	//PLLM MS109
		}
		else
		{
			Y2_MoveTo(m_lTableY2PosLimit - BT_DUAL_TABLE_PARKING_OFFSET_Y);
		}

		Y2_Profile(NORMAL_PROF);
	}
	else
	{
		Y12_Home();
	}
	*/
	return TRUE;
}

BOOL CBinTable::IsWithinTable1Limit(LONG lX, LONG lY)
{
	CString str;
    if( lX>m_lTableXPosLimit || lX<m_lTableXNegLimit )
    {
		str.Format("BT X exceeds sw limit: %ld (%ld, %ld)", lX, m_lTableXNegLimit, m_lTableXPosLimit);
		SetErrorMessage(str);
		//HmiMessage(str, "Re-align Binblock Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
        return FALSE;
    }
    if( lY>m_lTableYPosLimit || lY<m_lTableYNegLimit )
    {
		str.Format("BT Y exceeds sw limit: %ld (%ld, %ld)", lY, m_lTableYNegLimit, m_lTableYPosLimit);
		SetErrorMessage(str);
		//HmiMessage(str, "Re-align Binblock Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
        return FALSE;
    }
	return TRUE;
}

BOOL CBinTable::IsWithinTable2Limit(LONG lX, LONG lY)
{
	/*
	if (!m_bUseDualTablesOption)
		return FALSE;
	CString str;
    if( lX>m_lTableX2PosLimit || lX<m_lTableX2NegLimit )
    {
		str.Format("BT X2 exceeds sw limit: %ld (%ld, %ld)", lX, m_lTableX2NegLimit, m_lTableX2PosLimit);
		SetErrorMessage(str);
		//HmiMessage(str, "Re-align Binblock Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
        return FALSE;
    }
    if( lY>m_lTableY2PosLimit || lY<m_lTableY2NegLimit )
    {
		str.Format("BT Y2 exceeds sw limit: %ld (%ld, %ld)", lY, m_lTableY2NegLimit, m_lTableY2PosLimit);
		SetErrorMessage(str);
		//HmiMessage(str, "Re-align Binblock Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
        return FALSE;
    }*/
	return TRUE;
}

BOOL CBinTable::IsTable1InBondRegion()
{
	GetEncoderValue();
	LONG lULX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftX(1));
	LONG lULY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftY(1));
	LONG lLRX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkLowerRightX(1));
	LONG lLRY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkLowerRightY(1));

	CString szLog;
	//szLog.Format("Check BT1 In Bond-Region: (%ld %ld %ld)", lLRX, m_lEnc_X, lULX);
	//SetErrorMessage(szLog);

	if ( m_lEnc_Y < lLRY &&  m_lEnc_Y > lULY)
	{
		szLog.Format("BT X detected in BOND region: (%ld, %ld, %ld)", lLRY, m_lEnc_Y, lULY);
		SetErrorMessage(szLog);
		//HmiMessage(szLog, "Check BT", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return TRUE;
	}
	return FALSE;
}

BOOL CBinTable::IsTable2InBondRegion()
{
	/*
	if (!m_bUseDualTablesOption)
		return FALSE;

	GetEncoderValue();
	LONG lULX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftX(1))  + m_lBT2OffsetX;
	LONG lULY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftY(1))  + m_lBT2OffsetY;
	LONG lLRX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkLowerRightX(1)) + m_lBT2OffsetX;
	LONG lLRY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkLowerRightY(1)) + m_lBT2OffsetY;

	CString szLog;
	//szLog.Format("Check BT2 In Bond-Region: (%ld %ld %ld)", lLRX, m_lEnc_X2, lULX);
	//SetErrorMessage(szLog);

	if ( m_lEnc_Y2 < lLRY &&  m_lEnc_Y2 > lULY)
	{
		szLog.Format("BT2 X detected in BOND region: (%ld, %ld, %ld)", lLRY, m_lEnc_Y2, lULY);
		SetErrorMessage(szLog);
		//HmiMessage(szLog, "Check BT2", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return TRUE;
	}
	return FALSE;*/
	return TRUE;
}

INT CBinTable::T_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_T)
	{
		try
		{
			/*CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
			if (pBondHead && !pBondHead->IsMS50ChgColletZAtHome())
			{
				pBondHead->MS50ChgColletZ_MoveTo(0);
			}*/

			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_T, &m_stBTAxis_T) != 0)
			{
				m_bComm_T = FALSE;
				CMS896AStn::MotionClearError(BT_AXIS_T, &m_stBTAxis_T);
			}

			if (!CMS896AStn::MotionIsServo(BT_AXIS_T, &m_stBTAxis_T))
			{
				if (!m_bComm_T)
				{
					m_bHome_T	= FALSE;
					if ((nResult = CMS896AStn::MotionCommutateServo(BT_AXIS_T, &m_stBTAxis_T)) == gnAMS_OK)
					{
						m_bComm_T = TRUE;
					}
				}
			}
			else
			{
				m_bComm_T = TRUE;
			}

			if (nResult == gnAMS_OK)
			{
//AfxMessageBox("BT T Home ...", MB_SYSTEMMODAL); 

				if ((nResult = CMS896AStn::MotionMoveHome(BT_AXIS_T, 1, 0, &m_stBTAxis_T)) == gnAMS_OK)
				{
					Sleep(100);
					LONG lHomePosn = 0;

					CMS896AStn::MotionSetPosition(BT_AXIS_T, lHomePosn, &m_stBTAxis_T);
					/*LONG lTHomeOffsetCount = -2578;
					if (IsBinTableTHighResolution())
					{
						lTHomeOffsetCount = lTHomeOffsetCount * 2;
					}

					CMS896AStn::MotionMove(BT_AXIS_T, lTHomeOffsetCount, SFM_WAIT, &m_stBTAxis_T);
					CMS896AStn::MotionSetPosition(BT_AXIS_T, lHomePosn, &m_stBTAxis_T);*/
					m_bHome_T		= TRUE;
					m_bIsPowerOn_T	= TRUE;
					m_lCurPos_T		= lHomePosn;
//					else
//					{
//						CMS896AStn::MotionMove(BT_AXIS_T, -2578, SFM_WAIT, &m_stBTAxis_T);
//						m_lCurPos_T		= GetTEncoderValue(BT_AXIS_T);
//					}
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_T	= FALSE;
					nResult		= gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_T, &m_stBTAxis_T);
			CMS896AStn::MotionClearError(BT_AXIS_T, &m_stBTAxis_T);

			m_bComm_T	= FALSE;
			m_bHome_T	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_T)
		{
			Sleep(100);
			m_bComm_T	= TRUE;
			m_bHome_T	= TRUE;
		}
	}
	
	return nResult;
}

INT CBinTable::T_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_T)
	{
		try
		{
			if (!m_bHome_T)
			{
				nResult	= gnNOTOK;
			}

			//v4.59A44
			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_T, &m_stBTAxis_T) != 0)
			{
				//CMS896AStn::MotionClearError(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
				m_bHome_T	= FALSE;
				nResult		= gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
				if (pBondHead && !pBondHead->IsMS50ChgColletZAtHome())
				{
					pBondHead->MS50ChgColletZ_MoveTo(0);
				}

				CMS896AStn::MotionMoveTo(BT_AXIS_T, m_lThetaMotorDirection * nPos, SFM_NOWAIT, &m_stBTAxis_T);

				m_lCurPos_T	= m_lThetaMotorDirection * nPos;		//v4.59A33

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BT_AXIS_T, 30000, &m_stBTAxis_T);
					if (nResult != gnOK)
					{
						nResult = gnNOTOK;
					}
					else
					{
						if (m_bUseTEncoder)	//v4.59A42
						{
							Sleep(10);
							LONG lEncT = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_T, m_BT_T_RESOLUTION * 0.8, &m_stBTAxis_T);
							LONG lCmdT = CMS896AStn::MotionGetCommandPosition(BT_AXIS_T, &m_stBTAxis_T);

							LONG lBT_ENCODER_TOL_T = IsBinTableTHighResolution() ? BT_ENCODER_TOL_T * 2 : BT_ENCODER_TOL_T;
							if (labs(lEncT - lCmdT) > lBT_ENCODER_TOL_T)
							{
								Sleep(100);
								lEncT = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_T, m_BT_T_RESOLUTION * 0.8, &m_stBTAxis_T);

								if (labs(lEncT - lCmdT) > lBT_ENCODER_TOL_T)
								{
									CString szErr;
									szErr.Format("BT THETA motor is missing step - ENC=%ld, CMD=%ld, TOL=%ld", 
										lEncT, lCmdT, 1000);
									SetErrorMessage(szErr);
								
									HmiMessage_Red_Back(szErr);
									nResult = gnNOTOK;
								}
							}
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_T, &m_stBTAxis_T);
			CMS896AStn::MotionClearError(BT_AXIS_T, &m_stBTAxis_T);

			m_bHome_T	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_T)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT T moveto");
	}

	return nResult;
}

INT CBinTable::T_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_T)
	{
		try
		{
			if (!m_bHome_T)
			{
				nResult = gnNOTOK;
			}

			//v4.59A44
			if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_T, &m_stBTAxis_T) != 0)
			{
				//CMS896AStn::MotionClearError(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
				m_bHome_T	= FALSE;
				nResult		= gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
				if (pBondHead && !pBondHead->IsMS50ChgColletZAtHome())
				{
					pBondHead->MS50ChgColletZ_MoveTo(0);
				}

				CMS896AStn::MotionMove(BT_AXIS_T, m_lThetaMotorDirection * nPos, SFM_NOWAIT, &m_stBTAxis_T);

				m_lCurPos_T	+= m_lThetaMotorDirection * nPos;	//v4.59A33

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BT_AXIS_T, 30000, &m_stBTAxis_T);
					if (nResult != gnOK)
					{
						nResult = gnNOTOK;
					}

					if (m_bUseTEncoder)	//v4.59A42
					{
						Sleep(10);
						LONG lEncT = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_T, m_BT_T_RESOLUTION * 0.8, &m_stBTAxis_T);
						LONG lCmdT = CMS896AStn::MotionGetCommandPosition(BT_AXIS_T, &m_stBTAxis_T);

						LONG lBT_ENCODER_TOL_T = IsBinTableTHighResolution() ? BT_ENCODER_TOL_T * 2 : BT_ENCODER_TOL_T;
						if (labs(lEncT - lCmdT) > lBT_ENCODER_TOL_T)
						{
							Sleep(100);
							lEncT = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_T, m_BT_T_RESOLUTION * 0.8, &m_stBTAxis_T);

							if (labs(lEncT - lCmdT) > lBT_ENCODER_TOL_T)
							{
								CString szErr;
								szErr.Format("BT THETA motor is missing step - ENC=%ld, CMD=%ld, TOL=%ld", 
									lEncT, lCmdT, 1000);
								SetErrorMessage(szErr);
							
								HmiMessage_Red_Back(szErr);
								nResult = gnNOTOK;
							}
						}
					}
				
				}

			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_T, &m_stBTAxis_T);

			m_bHome_T	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_T)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT T move");
	}

	return nResult;
}

INT CBinTable::T_Sync()
{
	INT nResult = gnOK;
	//if (m_fHardware && m_pServo_Y && m_bSel_Y)
	if (m_fHardware && m_bSel_T)
	{
		try
		{
			CMS896AStn::MotionSync(BT_AXIS_T, 30000, &m_stBTAxis_T);
			//m_lCurPos_T	= CMS896AStn::MotionGetCommandPosition(BT_AXIS_T, &m_stBTAxis_T);

			if (m_bUseTEncoder)	//v4.59A42
			{
				Sleep(10);
				LONG lEncT = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_T, m_BT_T_RESOLUTION * 0.8, &m_stBTAxis_T);
				LONG lCmdT = CMS896AStn::MotionGetCommandPosition(BT_AXIS_T, &m_stBTAxis_T);

				LONG lBT_ENCODER_TOL_T = IsBinTableTHighResolution() ? BT_ENCODER_TOL_T * 2 : BT_ENCODER_TOL_T;
				if (labs(lEncT - lCmdT) > lBT_ENCODER_TOL_T)
				{
					Sleep(100);
					lEncT = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_T, m_BT_T_RESOLUTION * 0.8, &m_stBTAxis_T);

					if (labs(lEncT - lCmdT) > lBT_ENCODER_TOL_T)
					{
						CString szErr;
						szErr.Format("BT THETA motor is missing step (SYNC) - ENC=%ld, CMD=%ld, TOL=%ld", 
							lEncT, lCmdT, 1000);
						SetErrorMessage(szErr);
					
						HmiMessage_Red_Back(szErr);
						nResult = gnNOTOK;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_T, &m_stBTAxis_T);
			nResult = gnNOTOK;
		}	
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BT T Sync");
	}

	return nResult;
}

INT CBinTable::T_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_T)
	{
		try 
		{
			if ( bOn == TRUE )
			{
				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_T, &m_stBTAxis_T) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BT_AXIS_T, &m_stBTAxis_T);
					m_bIsPowerOn_T = TRUE;
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(BT_AXIS_T, &m_stBTAxis_T) == TRUE)
				{
					CMS896AStn::MotionPowerOff(BT_AXIS_T, &m_stBTAxis_T);
					m_bIsPowerOn_T = FALSE;
					m_bHome_T = FALSE;
					m_bComm_T = FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_T, &m_stBTAxis_T);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

BOOL CBinTable::T_IsPowerOn()
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (!m_bEnable_T)
		return FALSE;
	if (!m_bSel_T)
		return TRUE;

	try
	{
		return CMS896AStn::MotionIsPowerOn(BT_AXIS_T, &m_stBTAxis_T);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CBinTable::T_MoveByDegree(DOUBLE dDegree, INT nMode)
{
	if (m_dThetaRes == 0)
	{
		SetErrorMessage("T_MoveByDegree: m_dThetaRes = 0");
		return FALSE;
	}

	if (IsMS90())	//v4.48A30
	{
		if (fabs(dDegree) > 360)		//max. allowable angle of rotation for Bt T is +/- 360 degree (HW limit)
		{
			SetErrorMessage("BT: T_MoveByDegree: angle > 360");
			return FALSE;
		}
	}
	else
	{
		if (fabs(dDegree) > 15)		//max. allowable angle of rotation for Bt T is +/- 15 degree (HW limit)
		{
			SetErrorMessage("BT: T_MoveByDegree: angle > 15");
			return FALSE;
		}
	}

	//m_dThetaRes = "angle (degree) per count" = 0.01186 (Machine MSD)
	//dDegree > 0	-> table platform rotates counterclockwise, so need tp multiple encoder by -1

	INT nPos = (INT) _round(dDegree / m_dThetaRes);
	//CString szTemp;
	//szTemp.Format("T_Move encoder = %d, Res = %.5f", nPos, m_dThetaRes);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	if (T_Move(nPos, nMode) != gnOK)
	{
		SetErrorMessage("T_MoveByDegree: T_Move fails ");
		return FALSE;
	}

	return TRUE;
}

INT CBinTable::T_SearchBarcode(LONG lDir, LONG lSearchDist, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_T)
	{
		try
		{
			if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_T, &m_stBTAxis_T))
			{
				return gnNOTOK;
			}

			CMS896AStn::MotionSelectSearchProfile(BT_AXIS_T, "spfBinTableTBarcode", &m_stBTAxis_T);

			if ( lDir == 0 )
			{
				CMS896AStn::MotionUpdateSearchProfile(BT_AXIS_T, "spfBinTableTBarcode", 2, -1 * lSearchDist, &m_stBTAxis_T);
				CMS896AStn::MotionSearch(BT_AXIS_T, 1, nMode, &m_stBTAxis_T, "spfBinTableTBarcode");
			}
			else
			{
				CMS896AStn::MotionUpdateSearchProfile(BT_AXIS_T, "spfBinTableTBarcode", 2, lSearchDist, &m_stBTAxis_T);
				CMS896AStn::MotionSearch(BT_AXIS_T, 1, nMode, &m_stBTAxis_T, "spfBinTableTBarcode");
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_T, &m_stBTAxis_T);
			nResult = gnNOTOK;
		}
	}

	m_lCurPos_T	= CMS896AStn::MotionGetCommandPosition(BT_AXIS_T, &m_stBTAxis_T);

	if (!T_IsPowerOn() || (CMS896AStn::MotionReportErrorStatus(BT_AXIS_T, &m_stBTAxis_T) != 0) )
		return gnNOTOK;
	return nResult;
}

/*
LONG CBinTable::SetupDataLogX(VOID)
{
	Hp_mcb	*pModule = NULL;
	short	ssChannel1 = 0;
	short	ssReturn = 0;
	byte	ucByteNo;

	//pModule		= m_pServo_Ej->m_HpCcb.hp_servo_ch.p;
	//ssChannel1	= (short) m_pServo_Ej->GetChannelId()-1;
	try
	{
		SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(BT_AXIS_X));
		if (pServo != NULL)
		{
			pModule		= pServo->m_HpCcb.hp_servo_ch.p;
			ssChannel1	= (short) pServo->GetChannelId()-1;
		}

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		AfxMessageBox("BT: exception in SetupDataLog", MB_SYSTEMMODAL);
		return ssReturn;
	}

	short siPortList[4];
	siPortList[0] = HP_CH_ENCPOS_PORT_0 + ssChannel1;
	siPortList[1] = HP_CH_CMDPOS_PORT_0 + ssChannel1;
	siPortList[2] = HP_CH_MODE_PORT_0   + ssChannel1;
	siPortList[3] = HP_CH_MTRDAC_PORT_0   + ssChannel1;

	ssReturn = hp_set_datalog_trigger_channel(pModule, ssChannel1+1);

	ssReturn = hp_set_datalog_static_sample(pModule, 100);
	ssReturn = hp_set_datalog_entries(pModule, 4, siPortList, &ucByteNo);
	ssReturn = hp_set_datalog_mode(pModule, 1);

	return (LONG)ssReturn;
}


LONG CBinTable::EnableDataLogX(BOOL bLog)
{
	Hp_mcb	*pModule = NULL;
	short	ssReturn = 0;
	
	try
	{
		SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(BT_AXIS_X));		
		if (pServo != NULL)
		{
			pModule		= pServo->m_HpCcb.hp_servo_ch.p;
		}

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		AfxMessageBox("BT: exception in EnableDataLog", MB_SYSTEMMODAL);
		return ssReturn;
	}


	if (bLog == TRUE)
	{
		ssReturn = hp_datalog(pModule, HP_ENABLE);
	}
	else
	{
		int	siData[0x8000];
		memset(siData, 0, sizeof(siData));

		int	i;
		unsigned short	usNumOfSamples = 0;
		unsigned char	ucStatus = 0;

		ssReturn = hp_datalog(pModule, HP_DISABLE);
		hp_report_datalog_status(pModule, &usNumOfSamples, &ucStatus);
		ssReturn = hp_upload_datalog(pModule, siData, usNumOfSamples);
		
		CStdioFile oFile;
		CString szData;

		if (oFile.Open(_T("c:\\MapSorter\\UserData\\DataLogBtX.txt"), 
						CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
		{
			CString szSampleSize;
			szSampleSize.Format("%d",usNumOfSamples);
			oFile.WriteString("Sample Size:" + szSampleSize + "\n");
			for (i=0; i<usNumOfSamples; i++)
			{
				szData.Format("%d,%d,%d,%d\n", siData[i*4], siData[i*4+1], siData[i*4+2], siData[i*4+3]);
				//szData.Format("%d,%d\n", siData[i*2], siData[i*2+1]);
				oFile.WriteString(szData);
			}

			oFile.Close();
		}
	}

	return (LONG)ssReturn;
}
*/
