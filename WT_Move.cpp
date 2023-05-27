/////////////////////////////////////////////////////////////////
// WT_Move.cpp : Move functions of the CWaferTable class
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
#include "WaferTable.h"
#include "BH_Constant.h"
#include "BondHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////////////////
//	X Axis 
////////////////////////////////////////////
BOOL CWaferTable::X_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWT)		//v3.61
	{
		return TRUE;
	}
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CWaferTable::X2_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWT)
	{
		return TRUE;
	}
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CWaferTable::X_Home()
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		try
		{
			//CMS896AStn::MotionPowerOff(WT_AXIS_X, &m_stWTAxis_X);
			//Sleep(50);
			//CMS896AStn::MotionResetController(WT_AXIS_X, &m_stWTAxis_X);

			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0)
			{
				m_bComm_X = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
			}

			if (!m_bComm_X)
			{
				m_bHome_X = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_X, &m_stWTAxis_X)) == gnOK)
				{
					m_bComm_X = TRUE;
				}
				else
				{
					nResult	= Err_WTableXCommutate;
					CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
				}
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(WT_AXIS_X, 1, 0, &m_stWTAxis_X);

				//if (nResult == gnAMS_OK)
				if ( (CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) == 0) && 
					X_IsPowerOn())
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(WT_AXIS_X, 0, &m_stWTAxis_X);
					m_bHome_X	= TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_WTableXMoveHome;
				}


				/*

				//if (m_bUseDualTablesOption)							//v4.38T2	//Prober
				//{
				//	// Prober rod motors
				//	X_SelectControl(PL_STATIC);
				//	CMS896AStn::MotionPowerOn(WT_AXIS_X, &m_stWTAxis_X);
				//	Sleep(50);

				//	if (m_bUseES101v11TableConfig )						//v4.38T2	//Prober
				//	//if (m_bUseES101v11TableConfig)
				//	{
				//		//Move forward by 10000 steps in order to move away from INDEX position
				//		if (!IsTableXYMagneticEncoder())
				//		{
				//			Sleep(500);
				//			CMS896AStn::MotionMove(WT_AXIS_X, 10000, SFM_WAIT, &m_stWTAxis_X);
				//			Sleep(500);
				//		}
				//		if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0) ||
				//			!CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X))
				//		{
				//			m_bComm_X	= FALSE;
				//			m_bHome_X	= FALSE;
				//			return Err_WTableXMoveHome;
				//		}

				//		if (IsTableXYMagneticEncoder())
				//		{
				//			//Search Home of Magnetic Encoder
				//			if (!IsMTRHomeActive(m_stWTAxis_X))
				//			{
				//				CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_HOME_X, &m_stWTAxis_X);
				//				CMS896AStn::MotionSearch(WT_AXIS_X, 1, SFM_WAIT, &m_stWTAxis_X, WT_SP_HOME_X, TRUE);
				//				CMS896AStn::MotionMove(WT_AXIS_X, 200, SFM_WAIT, &m_stWTAxis_X);
				//			}
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_HOME_X, &m_stWTAxis_X);
				//			CMS896AStn::MotionSearch(WT_AXIS_X, 1, SFM_WAIT, &m_stWTAxis_X, WT_SP_HOME_X);
				//		}
				//		else
				//		{
				//			//Search Index sensor & Drive In
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_INDEX_X, &m_stWTAxis_X);
				//			CMS896AStn::MotionSearch(WT_AXIS_X, 0, SFM_WAIT, &m_stWTAxis_X, WT_SP_INDEX_X);
				//		}

				//		if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0) ||
				//			!CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X))
				//		{
				//			m_bComm_X	= FALSE;
				//			m_bHome_X	= FALSE;
				//			return Err_WTableXMoveHome;
				//		}
				//	}
				//	else
				//	{
				//		//Search negative Limit sensor 
				//		if (!IsTableXYMagneticEncoder())
				//		{
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_NEG_LIMIT_X, &m_stWTAxis_X);
				//			CMS896AStn::MotionSearch(WT_AXIS_X, 0, SFM_WAIT, &m_stWTAxis_X, WT_SP_NEG_LIMIT_X);
				//		}
				//		if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0) ||
				//			!CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X))
				//		{
				//			m_bComm_X	= FALSE;
				//			m_bHome_X	= FALSE;
				//			return Err_WTableXMoveHome;
				//		}

				//		Sleep(100);
				//		CMS896AStn::MotionSetPosition(WT_AXIS_X, 0, &m_stWTAxis_X);
				//		Sleep(100);

				//		if (IsTableXYMagneticEncoder())
				//		{
				//			//Search Home of Magnetic Encoder
				//			if (!IsMTRHomeActive(m_stWTAxis_X))
				//			{
				//				CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_HOME_X, &m_stWTAxis_X);
				//				CMS896AStn::MotionSearch(WT_AXIS_X, 1, SFM_WAIT, &m_stWTAxis_X, WT_SP_HOME_X, TRUE);
				//				CMS896AStn::MotionMove(WT_AXIS_X, 200, SFM_WAIT, &m_stWTAxis_X);
				//			}
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_HOME_X, &m_stWTAxis_X);
				//			CMS896AStn::MotionSearch(WT_AXIS_X, 1, SFM_WAIT, &m_stWTAxis_X, WT_SP_HOME_X);
				//		}
				//		else
				//		{
				//			//Search Index sensor & Drive In
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_INDEX_X, &m_stWTAxis_X);
				//			CMS896AStn::MotionSearch(WT_AXIS_X, 1, SFM_WAIT, &m_stWTAxis_X, WT_SP_INDEX_X);
				//		}
				//		if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0) ||
				//			!CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X))
				//		{
				//			m_bComm_X	= FALSE;
				//			m_bHome_X	= FALSE;
				//			return Err_WTableXMoveHome;
				//		}

				//		Sleep(100);
				//		LONG lOffsetX	= CMS896AStn::MotionGetEncoderPosition(WT_AXIS_X, 1, &m_stWTAxis_X);
				//		m_lXNegLimit	= -1 * lOffsetX;
				//		m_lXPosLimit	= m_lXNegLimit + 1140000;
				//	
				//	}

				//	Sleep(100);
				//	CMS896AStn::MotionSetPosition(WT_AXIS_X, 0, &m_stWTAxis_X);
				//	X_SelectControl(PL_DYNAMIC);
				//	m_bHome_X	= TRUE;

				//	CString szLog;
				//	szLog.Format("WTX Limit (ES101) : U=%ld, L=%ld", m_lXPosLimit, m_lXNegLimit);
				//	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				//}
				//else
				//{
					CMS896AStn::MotionPowerOn(WT_AXIS_X, &m_stWTAxis_X);
					Sleep(50);

					//if (IsTableXYMagneticEncoder())
					//{
					//	//Search Home of Magnetic Encoder
					//	if (!IsMTRHomeActive(m_stWTAxis_X))
					//	{
					//		CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_HOME_X, &m_stWTAxis_X);
					//		CMS896AStn::MotionSearch(WT_AXIS_X, 1, SFM_WAIT, &m_stWTAxis_X, WT_SP_HOME_X, TRUE);
					//		CMS896AStn::MotionMove(WT_AXIS_X, 200, SFM_WAIT, &m_stWTAxis_X);
					//	}
					//	CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_HOME_X, &m_stWTAxis_X);
					//	CMS896AStn::MotionSearch(WT_AXIS_X, 1, SFM_WAIT, &m_stWTAxis_X, WT_SP_HOME_X);
					//}
					//else

					MotionLeaveHomeSensor(WT_AXIS_X, &m_stWTAxis_X);
					{
						nResult = CMS896AStn::MotionMoveHome(WT_AXIS_X, 1, 0, &m_stWTAxis_X);
					}

					if ( (CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) == 0) && 
						CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X))
					{
						Sleep(100);
						CMS896AStn::MotionSetPosition(WT_AXIS_X, 0, &m_stWTAxis_X);
						m_bHome_X	= TRUE;
					}
					else if (nResult == gnNOTOK)
					{
						nResult = Err_WTableXMoveHome;
					}
				}
				
				*/
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);

			m_bComm_X	= FALSE;
			nResult = Err_WTableXMoveHome;
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

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT Home X Failure");
	}
	else
	{
		SetMotionCE(FALSE, "WT Home X Sucess");
	}

	return nResult;
}


INT CWaferTable::X12_Home()		//ES100G2 dual table X1 & X2 homing sequence
{
	INT nResult = gnOK;

	//1. Home X1 towards POS Limit sensor
	if (m_fHardware && m_bSel_X)
	{
		try
		{
			CMS896AStn::MotionEnableProtection(WT_AXIS_X, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stWTAxis_X);
			CMS896AStn::MotionSetSoftwareLimit(WT_AXIS_X, 9999999, -9999999, &m_stWTAxis_X);

			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0)
			{
				m_bComm_X = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
			}

			if (!m_bComm_X)
			{
				m_bComm_X = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_X, &m_stWTAxis_X)) == gnOK)
				{
					m_bComm_X = TRUE;
				}
				else
				{
					return Err_WTableXCommutate;
				}
			}

			if (nResult == gnOK)
			{
				X_SelectControl(PL_STATIC);
				CMS896AStn::MotionPowerOn(WT_AXIS_X, &m_stWTAxis_X);

				if (m_bUseES101v11TableConfig)		//v4.28		//ES101 v1.1 WT config		
				{
					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_INDEX_X, &m_stWTAxis_X);
					CMS896AStn::MotionSearch(WT_AXIS_X, 0, SFM_WAIT, &m_stWTAxis_X, WT_SP_INDEX_X);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X))
					{
						m_bComm_X	= FALSE;
						m_bHome_X	= FALSE;
						return Err_WTableXMoveHome;
					}
				}
				else
				{
					//Search negative Limit sensor 
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_NEG_LIMIT_X, &m_stWTAxis_X);
					CMS896AStn::MotionSearch(WT_AXIS_X, 0, SFM_WAIT, &m_stWTAxis_X, WT_SP_NEG_LIMIT_X);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X))
					{
						m_bComm_X	= FALSE;
						m_bHome_X	= FALSE;
						return Err_WTableXMoveHome;
					}

					Sleep(100);
					CMS896AStn::MotionSetPosition(WT_AXIS_X, 0, &m_stWTAxis_X);
					Sleep(100);

					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, WT_SP_INDEX_X, &m_stWTAxis_X);
					CMS896AStn::MotionSearch(WT_AXIS_X, 1, SFM_WAIT, &m_stWTAxis_X, WT_SP_INDEX_X);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X))
					{
						m_bComm_X	= FALSE;
						m_bHome_X	= FALSE;
						return Err_WTableXMoveHome;
					}

					Sleep(100);
					LONG lOffsetX	= CMS896AStn::MotionGetEncoderPosition(WT_AXIS_X, 1, &m_stWTAxis_X);
					m_lXNegLimit	= -1 * lOffsetX;
					m_lXPosLimit	= m_lXNegLimit + 1140000;
				}

				Sleep(100);
				CMS896AStn::MotionSetPosition(WT_AXIS_X, 0, &m_stWTAxis_X);
				X_SelectControl(PL_DYNAMIC);
				m_bHome_X	= TRUE;

				CString szLog;
				szLog.Format("WTX Limit (ES101 12) : U=%ld, L=%ld", m_lXPosLimit, m_lXNegLimit);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
			m_bComm_X	= FALSE;
			m_bHome_X	= FALSE;
			return Err_WTableXMoveHome;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			m_bComm_X	= TRUE;
			m_bHome_X	= TRUE;
		}
	}


	//2. Home X2 towards POS Limit sensor
	if (m_fHardware && m_bSel_X2)
	{
		try
		{
			CMS896AStn::MotionEnableProtection(WT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stWTAxis_X2);
			CMS896AStn::MotionSetSoftwareLimit(WT_AXIS_X2, 9999999, -9999999, &m_stWTAxis_X2);

			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_X2, &m_stWTAxis_X2) != 0)
			{
				m_bComm_X2 = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_X2, &m_stWTAxis_X2);
			}

			if (!m_bComm_X2)
			{
				m_bComm_X2 = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_X2, &m_stWTAxis_X2)) == gnOK)
				{
					m_bComm_X2 = TRUE;
				}
				else
				{
					return Err_WTableXCommutate;
				}
			}

			if (nResult == gnOK)
			{
				X2_SelectControl(PL_STATIC);
				CMS896AStn::MotionPowerOn(WT_AXIS_X2, &m_stWTAxis_X2);

				if (m_bUseES101v11TableConfig)		//v4.28		//ES101 v1.1 WT config		
				{
					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X2, WT_SP_INDEX_X, &m_stWTAxis_X2);
					CMS896AStn::MotionSearch(WT_AXIS_X2, 0, SFM_WAIT, &m_stWTAxis_X2, WT_SP_INDEX_X);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X2, &m_stWTAxis_X2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2))
					{
						m_bComm_X2	= FALSE;
						m_bHome_X2	= FALSE;
						return Err_WTableXMoveHome;
					}
				}
				else
				{
					//Search negative Limit sensor 
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X2, WT_SP_POS_LIMIT_X, &m_stWTAxis_X2);
					CMS896AStn::MotionSearch(WT_AXIS_X2, 0, SFM_WAIT, &m_stWTAxis_X2, WT_SP_POS_LIMIT_X);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X2, &m_stWTAxis_X2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2))
					{
						m_bComm_X2	= FALSE;
						m_bHome_X2	= FALSE;
						return Err_WTableXMoveHome;
					}

					Sleep(100);
					CMS896AStn::MotionSetPosition(WT_AXIS_X2, 0, &m_stWTAxis_X2);
					Sleep(100);

					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X2, WT_SP_INDEX_X, &m_stWTAxis_X2);
					CMS896AStn::MotionSearch(WT_AXIS_X2, 1, SFM_WAIT, &m_stWTAxis_X2, WT_SP_INDEX_X);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X2, &m_stWTAxis_X2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2))
					{
						m_bComm_X2	= FALSE;
						m_bHome_X2	= FALSE;
						return Err_WTableXMoveHome;
					}

					Sleep(100);
					LONG lOffsetX	= CMS896AStn::MotionGetEncoderPosition(WT_AXIS_X2, 1, &m_stWTAxis_X2);
					m_lX2PosLimit	= -1 * lOffsetX;
					m_lX2NegLimit	= m_lX2PosLimit - 1140000;
				}

				Sleep(100);
				CMS896AStn::MotionSetPosition(WT_AXIS_X2, 0, &m_stWTAxis_X2);
				X2_SelectControl(PL_DYNAMIC);
				m_bHome_X2	= TRUE;

				CString szLog;
				szLog.Format("WTX2 Limit (ES101 12) : U=%ld, L=%ld", m_lX2PosLimit, m_lX2NegLimit);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
			CMS896AStn::MotionClearError(WT_AXIS_X2, &m_stWTAxis_X2);
			m_bComm_X2	= FALSE;
			m_bHome_X2	= FALSE;
			return Err_WTableXMoveHome;
		}
	}
	else
	{
		if (m_bSel_X2)
		{
			m_bComm_X2	= TRUE;
			m_bHome_X2	= TRUE;
		}
	}

	return nResult;
}

INT CWaferTable::X2_Home()
{
	INT nResult = gnOK;

	//Home X2 towards POS Limit sensor
	if (m_fHardware && m_bSel_X2)
	{
		try
		{
			CMS896AStn::MotionEnableProtection(WT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stWTAxis_X2);
			CMS896AStn::MotionSetSoftwareLimit(WT_AXIS_X2, 9999999, -9999999, &m_stWTAxis_X2);

			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_X2, &m_stWTAxis_X2) != 0)
			{
				m_bComm_X2 = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_X2, &m_stWTAxis_X2);
			}

			if (!m_bComm_X2)
			{
				m_bComm_X2 = FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_X2, &m_stWTAxis_X2)) == gnOK)
				{
					m_bComm_X2 = TRUE;
				}
				else
				{
					return Err_WTableXCommutate;
				}
			}

			if (nResult == gnOK)
			{
				X2_SelectControl(PL_STATIC);
				CMS896AStn::MotionPowerOn(WT_AXIS_X2, &m_stWTAxis_X2);

				if (m_bUseES101v11TableConfig)		//v4.28		//ES101 v1.1 table config
				{
					Sleep(500);
					CMS896AStn::MotionMove(WT_AXIS_X2, -10000, SFM_WAIT, &m_stWTAxis_X2);
					Sleep(500);

					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X2, &m_stWTAxis_X2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2))
					{
						m_bComm_X2	= FALSE;
						m_bHome_X2	= FALSE;
						return Err_WTableXMoveHome;
					}

					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X2, WT_SP_INDEX_X, &m_stWTAxis_X2);
					CMS896AStn::MotionSearch(WT_AXIS_X2, 0, SFM_WAIT, &m_stWTAxis_X2, WT_SP_INDEX_X);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X2, &m_stWTAxis_X2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2))
					{
						m_bComm_X2	= FALSE;
						m_bHome_X2	= FALSE;
						return Err_WTableXMoveHome;
					}
				}
				else
				{
					//Search negative Limit sensor 
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X2, WT_SP_POS_LIMIT_X, &m_stWTAxis_X2);
					CMS896AStn::MotionSearch(WT_AXIS_X2, 0, SFM_WAIT, &m_stWTAxis_X2, WT_SP_POS_LIMIT_X);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X2, &m_stWTAxis_X2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2))
					{
						m_bComm_X2	= FALSE;
						m_bHome_X2	= FALSE;
						return Err_WTableXMoveHome;
					}

					Sleep(100);
					CMS896AStn::MotionSetPosition(WT_AXIS_X2, 0, &m_stWTAxis_X2);
					Sleep(100);

//AfxMessageBox("X2 towards INDEX ...", MB_SYSTEMMODAL);
					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X2, WT_SP_INDEX_X, &m_stWTAxis_X2);
					CMS896AStn::MotionSearch(WT_AXIS_X2, 1, SFM_WAIT, &m_stWTAxis_X2, WT_SP_INDEX_X);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_X2, &m_stWTAxis_X2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2))
					{
						m_bComm_X2	= FALSE;
						m_bHome_X2	= FALSE;
						return Err_WTableXMoveHome;
					}

					Sleep(100);
					LONG lOffsetX	= CMS896AStn::MotionGetEncoderPosition(WT_AXIS_X2, 1, &m_stWTAxis_X2);
					m_lX2PosLimit	= -1 * lOffsetX;
					m_lX2NegLimit	= m_lX2PosLimit - 1140000;
				}

				Sleep(100);
				CMS896AStn::MotionSetPosition(WT_AXIS_X2, 0, &m_stWTAxis_X2);
				X2_SelectControl(PL_DYNAMIC);
				m_bHome_X2	= TRUE;

				CString szLog;
				szLog.Format("WTX2 Limit (ES101) : U=%ld, L=%ld", m_lX2PosLimit, m_lX2NegLimit);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
			CMS896AStn::MotionClearError(WT_AXIS_X2, &m_stWTAxis_X2);
			m_bComm_X2	= FALSE;
			m_bHome_X2	= FALSE;
			return Err_WTableXMoveHome;
		}
	}
	else
	{
		if (m_bSel_X2)
		{
			m_bComm_X2	= TRUE;
			m_bHome_X2	= TRUE;
		}
	}

	return nResult;
}

INT CWaferTable::X1_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_lMotorDirectionX == -1)
	{
		nPos = -1 * nPos;
	}

	if (m_fHardware && m_bSel_X)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableXMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_X)
			{
				nResult = gnNOTOK;
			}

			// Prober rod motors
			//if ( (IsMS60() == TRUE) )		//v4.48A4
			//{
				LONG lCommandPos = CMS896AStn::MotionGetCommandPosition(WT_AXIS_X, &m_stWTAxis_X);
				LONG lDistance = nPos - lCommandPos;
				lDistance = abs(lDistance);
				X1_SelectControlByDistance(lDistance);
			//}
				

			if (nResult == gnOK)
			{
				if (m_lX_ProfileType == MS896A_OBW_PROFILE)
				{
					CMS896AStn::MotionObwMoveTo(WT_AXIS_X, nPos, SFM_NOWAIT, &m_stWTAxis_X);
				}
				else
				{
					CMS896AStn::MotionMoveTo(WT_AXIS_X, nPos, SFM_NOWAIT, &m_stWTAxis_X);
				}

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_X, 10000, &m_stWTAxis_X)) != gnOK)
					{
						nResult = Err_WTableXMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);

			m_bHome_X	= FALSE;
			nResult = Err_WTableXMove;
		}
	}
	else
	{
		//if (m_bSel_X)
		//{
		if (nMode == SFM_WAIT)
		{
			if (IsMS60() || IsMS90())	//v4.49A11
				Sleep(15);
			else
				Sleep(30);
		}
		else
		{
			Sleep(1);
		}
		//}
	}

	//CheckResult(nResult, _T("X Axis - MoveTo"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT X Move Failure(X1_MoveTo)");
	}

	return nResult;
}

INT CWaferTable::X2_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_X2)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableXMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_X2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMoveTo(WT_AXIS_X2, nPos, SFM_NOWAIT, &m_stWTAxis_X2);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_X2, 10000, &m_stWTAxis_X2)) != gnOK)
					{
						nResult = Err_WTableXMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
			CMS896AStn::MotionClearError(WT_AXIS_X2, &m_stWTAxis_X2);

			m_bHome_X2	= FALSE;
			nResult = Err_WTableXMove;
		}
	}
	else
	{
		//if (m_bSel_X2)
		//{
			if (nMode == SFM_WAIT)
			{
				if (IsMS60() || IsMS90())	//v4.49A11
					Sleep(15);
				else
					Sleep(30);
			}
			else
			{
				Sleep(1);
			}
		//}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT X Move Failure(X2_MoveTo)");
	}
	return nResult;
}

INT CWaferTable::X1_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_lMotorDirectionX == -1)
	{
		nPos = -1 * nPos;
	}

	if (m_fHardware && m_bSel_X)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableXMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_X)
			{
				nResult = gnNOTOK;
			}
			if (nResult == gnOK)
			{
				if (m_lX_ProfileType == MS896A_OBW_PROFILE)
				{
					CMS896AStn::MotionObwMove(WT_AXIS_X, nPos, SFM_NOWAIT, &m_stWTAxis_X);
				}
				else
				{
					CMS896AStn::MotionMove(WT_AXIS_X, nPos, SFM_NOWAIT, &m_stWTAxis_X);
				}

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_X, 10000, &m_stWTAxis_X)) != gnOK)
					{
						nResult = Err_WTableXMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);

			m_bHome_X	= FALSE;
			nResult = Err_WTableXMove;
		}
	}
	else
	{
		//if (m_bSel_X)
		//{
			if (nMode == SFM_WAIT)
			{
				if (IsMS60() || IsMS90())	//v4.49A11
					Sleep(15);
				else
					Sleep(30);
			}
			else
			{
				Sleep(1);
			}
		//}
	}

	//CheckResult(nResult, _T("X Axis - Move"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT X Move Failure(X_Move)");
	}

	return nResult;
}

INT CWaferTable::X2_Move(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_X2)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableXMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_X2)
			{
				nResult = gnNOTOK;
			}
			if (nResult == gnOK)
			{
				CMS896AStn::MotionMove(WT_AXIS_X2, nPos, SFM_NOWAIT, &m_stWTAxis_X2);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_X2, 10000, &m_stWTAxis_X2)) != gnOK)
					{
						nResult = Err_WTableXMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
			CMS896AStn::MotionClearError(WT_AXIS_X2, &m_stWTAxis_X2);

			m_bHome_X2	= FALSE;
			nResult = Err_WTableXMove;
		}
	}
	else
	{
		//if (m_bSel_X2)
		//{
			if (nMode == SFM_WAIT)
			{
				if (IsMS60() || IsMS90())	//v4.49A11
					Sleep(15);
				else
					Sleep(30);
			}
			else
			{
				Sleep(1);
			}
		//}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT X Move Failure(X2_Move)");
	}
	return nResult;
}

INT CWaferTable::X1_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_X)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(WT_AXIS_X, 10000, &m_stWTAxis_X);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			nResult = gnNOTOK;
		}	
	}
	
	//CheckResult(nResult, _T("X Axis - Sync"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT X Sync Timeout (X1_Sync)");
	}

	return nResult;
}

INT CWaferTable::X2_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_X2)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(WT_AXIS_X2, 10000, &m_stWTAxis_X2);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
			nResult = gnNOTOK;
		}	
	}
	
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT X Sync Timeout (X2_Sync)");
	}
	return nResult;
}

INT CWaferTable::X_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_X)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if (CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X) == FALSE)
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_X, &m_stWTAxis_X);
					m_bIsPowerOn_X = TRUE;
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X) == TRUE)
				{
					CMS896AStn::MotionPowerOff(WT_AXIS_X, &m_stWTAxis_X);
					m_bIsPowerOn_X = FALSE;
					m_bComm_X	= FALSE;
					m_bHome_X	= FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			nResult = gnNOTOK;
		}	
	}
	//CheckResult(nResult, _T("X Axis - PowerOn"));
	return nResult;
}

INT CWaferTable::X2_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_X2)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if (CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2) == FALSE)
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_X2, &m_stWTAxis_X2);
					m_bIsPowerOn_X2 = TRUE;
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(WT_AXIS_X2, &m_stWTAxis_X2) == TRUE)
				{
					CMS896AStn::MotionPowerOff(WT_AXIS_X2, &m_stWTAxis_X2);
					m_bIsPowerOn_X2 = FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CWaferTable::X_Comm()
{
	if (m_fHardware && m_bSel_X)
	{
		INT nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_X, &m_stWTAxis_X);
		if ( nResult == gnOK )
		{
			m_bComm_X = TRUE;
		}	
		else
		{
			nResult	= Err_WTableXCommutate;
			CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
			m_bComm_X = FALSE;
		}
		//CheckResult(nResult, _T("X Axis - Commutate"));
	}
	return gnOK;
}

INT CWaferTable::X2_Comm()
{
	if (m_fHardware && m_bSel_X2)
	{
		INT nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_X2, &m_stWTAxis_X2);
		if ( nResult == gnOK )
		{
			m_bComm_X2 = TRUE;
		}	
		else
		{
			nResult	= Err_WTableXCommutate;
			CMS896AStn::MotionClearError(WT_AXIS_X2, &m_stWTAxis_X2);
			m_bComm_X2 = FALSE;
		}
	}
	return gnOK;
}


////////////////////////////////////////////
//	Y Axis 
////////////////////////////////////////////
BOOL CWaferTable::Y_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWT)	//v3.61
	{
		return TRUE;
	}
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CWaferTable::Y2_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWT)
	{
		return TRUE;
	}
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CWaferTable::Y_Home()
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		if (IsBlkFunc2Enable())
		{
			//Force to align wafer  when WT Home
			SetAlignmentStatus(FALSE);
		}

		try
		{
			CMS896AStn::MotionPowerOff(WT_AXIS_Y, &m_stWTAxis_Y);
			Sleep(50);
			CMS896AStn::MotionResetController(WT_AXIS_Y, &m_stWTAxis_Y);

			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y, &m_stWTAxis_Y) != 0)
			{
				m_bComm_Y = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
			}

			if (!m_bComm_Y)
			{
				m_bHome_Y		= FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_Y, &m_stWTAxis_Y)) == gnOK)
				{
					m_bComm_Y = TRUE;
				}
				else
				{
					nResult			= Err_WTableYCommutate;
					CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
				}
			}

			if (nResult == gnOK)
			{
				//if (m_bUseDualTablesOption)							//v4.38T2	//Prober
				//{
				//	Y_SelectControl(PL_STATIC);

				//	CMS896AStn::MotionPowerOn(WT_AXIS_Y, &m_stWTAxis_Y);
				//	Sleep(50);

				//	if (m_bUseES101v11TableConfig)						//v4.38T2	//Prober
				//	//if (m_bUseES101v11TableConfig )
				//	{	
				//		//Move forward by 10000 steps in order to move away from INDEX position
				//		if (!IsTableXYMagneticEncoder())
				//		{
				//			Sleep(500);
				//			CMS896AStn::MotionMove(WT_AXIS_Y, -10000, SFM_WAIT, &m_stWTAxis_Y);
				//			Sleep(500);
				//		}
				//		if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y, &m_stWTAxis_Y) != 0) ||
				//			!CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y))
				//		{
				//			m_bComm_Y	= FALSE;
				//			m_bHome_Y	= FALSE;
				//			return Err_WTableYMoveHome;
				//		}

				//		if (IsTableXYMagneticEncoder())
				//		{
				//			//Search Home of Magnetic Encoder
				//			if (!IsMTRHomeActive(m_stWTAxis_Y))
				//			{
				//				CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, WT_SP_HOME_Y, &m_stWTAxis_Y);
				//				CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y, WT_SP_HOME_Y, TRUE);
				//				CMS896AStn::MotionMove(WT_AXIS_Y, 200, SFM_WAIT, &m_stWTAxis_Y);
				//			}
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, WT_SP_HOME_Y, &m_stWTAxis_Y);
				//			CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y, WT_SP_HOME_Y);
				//		}
				//		else
				//		{
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, WT_SP_INDEX_Y, &m_stWTAxis_Y);
				//			CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y, WT_SP_INDEX_Y);
				//		}
				//		if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y, &m_stWTAxis_Y) != 0) ||
				//			!CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y))
				//		{
				//			m_bComm_Y	= FALSE;
				//			m_bHome_Y	= FALSE;
				//			return Err_WTableYMoveHome;
				//		}
				//	}
				//	else
				//	{
				//		if (!IsTableXYMagneticEncoder())
				//		{
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, WT_SP_NEG_LIMIT_Y, &m_stWTAxis_Y);
				//			CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y, WT_SP_NEG_LIMIT_Y);
				//		}

				//		if ( (CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y, &m_stWTAxis_Y) != 0) || 
				//			!CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y))
				//		{
				//			m_bComm_Y	= FALSE;
				//			m_bHome_Y	= FALSE;
				//			return Err_WTableYMoveHome;
				//		}

				//		Sleep(100);
				//		CMS896AStn::MotionSetPosition(WT_AXIS_Y, 0, &m_stWTAxis_Y);
				//		Sleep(100);

				//		if (IsTableXYMagneticEncoder())
				//		{
				//			//Search Home of Magnetic Encoder
				//			if (!IsMTRHomeActive(m_stWTAxis_Y))
				//			{
				//				CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, WT_SP_HOME_Y, &m_stWTAxis_Y);
				//				CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y, WT_SP_HOME_Y, TRUE);
				//				CMS896AStn::MotionMove(WT_AXIS_Y, 200, SFM_WAIT, &m_stWTAxis_Y);
				//			}
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, WT_SP_HOME_Y, &m_stWTAxis_Y);
				//			CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y, WT_SP_HOME_Y);
				//		}
				//		else
				//		{
				//			CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, WT_SP_INDEX_Y, &m_stWTAxis_Y);
				//			CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y, WT_SP_INDEX_Y);
				//		}

				//		if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y, &m_stWTAxis_Y) != 0) ||
				//			!CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y))
				//		{
				//			m_bComm_Y	= FALSE;
				//			m_bHome_Y	= FALSE;
				//			return Err_WTableYMoveHome;
				//		}

				//		Sleep(100);
				//		LONG lOffsetY	= CMS896AStn::MotionGetEncoderPosition(WT_AXIS_Y, 1, &m_stWTAxis_Y);
				//		m_lYPosLimit	= -1 * lOffsetY;
				//		m_lYNegLimit	= m_lYPosLimit - 400000;
				//	}

				//	Sleep(100);
				//	CMS896AStn::MotionSetPosition(WT_AXIS_Y, 0, &m_stWTAxis_Y);
				//	Y_SelectControl(PL_DYNAMIC);
				//	m_bHome_Y	= TRUE;

				//	CString szLog;
				//	szLog.Format("WTY Limit (ES101) : U=%ld, L=%ld", m_lYPosLimit, m_lYNegLimit);
				//	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				//}
				//else
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_Y, &m_stWTAxis_Y);
					Sleep(50);

					//if (IsTableXYMagneticEncoder())
					//{
					//	//Search Home of Magnetic Encoder
					//	if (!IsMTRHomeActive(m_stWTAxis_Y))
					//	{
					//		CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, WT_SP_HOME_Y, &m_stWTAxis_Y);
					//		CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y, WT_SP_HOME_Y, TRUE);
					//		CMS896AStn::MotionMove(WT_AXIS_Y, 200, SFM_WAIT, &m_stWTAxis_Y);
					//	}
					//	CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, WT_SP_HOME_Y, &m_stWTAxis_Y);
					//	CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y, WT_SP_HOME_Y);
					//}
					//else
					
					MotionLeaveHomeSensor(WT_AXIS_Y, &m_stWTAxis_Y);
					
					{
						nResult = CMS896AStn::MotionMoveHome(WT_AXIS_Y, 1, 0, &m_stWTAxis_Y);
					}

					//if (nResult == gnAMS_OK)
					if ( (CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y, &m_stWTAxis_Y) == 0) && 
						CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y))
					{
#ifndef NU_MOTION
						CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, "spfWaferYIndex", &m_stWTAxis_Y);
						CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y);
#endif
						Sleep(100);
						CMS896AStn::MotionSetPosition(WT_AXIS_Y, 0, &m_stWTAxis_Y);
						m_bHome_Y	= TRUE;
					}
					else if (nResult == gnNOTOK)
					{
						nResult = Err_WTableYMoveHome;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);

			m_bComm_Y	= FALSE;
			nResult = Err_WTableYMoveHome;
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

	//CheckResult(nResult, _T("Y Axis - Home"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT Y Move Failure(Y_Move)");
	}
	else
	{
		SetMotionCE(FALSE, "WT Y Move Sucess(Y_Move)");
	}

	return nResult;
}

INT CWaferTable::Y2_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y2, &m_stWTAxis_Y2) != 0)
			{
				m_bComm_Y2 = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_Y2, &m_stWTAxis_Y2);
			}

			if (!m_bComm_Y2)
			{
				m_bHome_Y2		= FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_Y2, &m_stWTAxis_Y2)) == gnOK)
				{
					m_bComm_Y2 = TRUE;
				}
				else
				{
					nResult			= Err_WTableYCommutate;
					CMS896AStn::MotionClearError(WT_AXIS_Y2, &m_stWTAxis_Y2);
				}
			}

			if (nResult == gnOK)
			{
				Y2_SelectControl(PL_STATIC);
				CMS896AStn::MotionPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2);

				if (m_bUseES101v11TableConfig)		//v4.28		//ES101 v1.1 table config
				{
//AfxMessageBox("WT: Y2 Home v1.1 INDEX ....", MB_SYSTEMMODAL);
					//Move forward by 10000 steps in order to move away from INDEX position
					Sleep(500);
					CMS896AStn::MotionMove(WT_AXIS_Y2, -10000, SFM_WAIT, &m_stWTAxis_Y2);
					Sleep(500);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y2, &m_stWTAxis_Y2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2))
					{
						m_bComm_Y2	= FALSE;
						m_bHome_Y2	= FALSE;
						return Err_WTableYMoveHome;
					}

					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y2, WT_SP_INDEX_Y, &m_stWTAxis_Y2);
					CMS896AStn::MotionSearch(WT_AXIS_Y2, 1, SFM_WAIT, &m_stWTAxis_Y2, WT_SP_INDEX_Y);
					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y2, &m_stWTAxis_Y2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2))
					{
						m_bComm_Y2	= FALSE;
						m_bHome_Y2	= FALSE;
						return Err_WTableYMoveHome;
					}
				}
				else
				{
//AfxMessageBox("WT: Y2 Home NEG Limit ....", MB_SYSTEMMODAL);
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y2, WT_SP_NEG_LIMIT_Y2, &m_stWTAxis_Y2);
					CMS896AStn::MotionSearch(WT_AXIS_Y2, 1, SFM_WAIT, &m_stWTAxis_Y2, WT_SP_NEG_LIMIT_Y2);

					if ( (CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y2, &m_stWTAxis_Y2) != 0) || 
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2) )
					{
						m_bComm_Y2	= FALSE;
						m_bHome_Y2	= FALSE;
						return Err_WTableYMoveHome;
					}

					Sleep(100);
					CMS896AStn::MotionSetPosition(WT_AXIS_Y2, 0, &m_stWTAxis_Y2);
					Sleep(100);

//AfxMessageBox("WT: Y2 Home INDEX ....", MB_SYSTEMMODAL);
					CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y2, WT_SP_INDEX_Y, &m_stWTAxis_Y2);
					CMS896AStn::MotionSearch(WT_AXIS_Y2, 1, SFM_WAIT, &m_stWTAxis_Y2, WT_SP_INDEX_Y);

					if ((CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y2, &m_stWTAxis_Y2) != 0) ||
						!CMS896AStn::MotionIsPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2))
					{
						m_bComm_Y2	= FALSE;
						m_bHome_Y2	= FALSE;
						return Err_WTableYMoveHome;
					}

					Sleep(100);
					LONG lOffsetY	= CMS896AStn::MotionGetEncoderPosition(WT_AXIS_Y2, 1, &m_stWTAxis_Y2);
					m_lY2PosLimit	= -1 * lOffsetY;
					m_lY2NegLimit	= m_lY2PosLimit - 400000;
				}

				Sleep(100);
				CMS896AStn::MotionSetPosition(WT_AXIS_Y2, 0, &m_stWTAxis_Y2);
				Y2_SelectControl(PL_DYNAMIC);
				m_bHome_Y2	= TRUE;

				CString szLog;
				szLog.Format("WTY2 Limit (ES101) : U=%ld, L=%ld", m_lY2PosLimit, m_lY2NegLimit);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			CMS896AStn::MotionClearError(WT_AXIS_Y2, &m_stWTAxis_Y2);

			m_bComm_Y2	= FALSE;
			nResult = Err_WTableYMoveHome;
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

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT Y Move Failure(Y2_Move)");
	}
	return nResult;
}


INT CWaferTable::Y1_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_lMotorDirectionY == -1)
	{
		nPos = -1 * nPos;
	}

	if (m_fHardware && m_bSel_Y)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableYMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_Y)
			{
				nResult = gnNOTOK;
			}

			LONG lCommandPos = CMS896AStn::MotionGetCommandPosition(WT_AXIS_Y, &m_stWTAxis_Y);
			LONG lDistance = nPos - lCommandPos;
			lDistance = abs(lDistance);
			Y1_SelectControlByDistance(lDistance);

			if (nResult == gnOK)
			{
				if (m_lY_ProfileType == MS896A_OBW_PROFILE)
				{
					CMS896AStn::MotionObwMoveTo(WT_AXIS_Y, nPos, SFM_NOWAIT, &m_stWTAxis_Y);
				}
				else
				{
					CMS896AStn::MotionMoveTo(WT_AXIS_Y, nPos, SFM_NOWAIT, &m_stWTAxis_Y);
				}

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y, 10000, &m_stWTAxis_Y)) != gnOK)
					{
						nResult = Err_WTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);

			m_bHome_Y	= FALSE;
			nResult = Err_WTableYMove;
		}
	}
	else
	{
		//if (m_bSel_Y)
		//{
			if (nMode == SFM_WAIT)
			{
				if (IsMS60() || IsMS90())	//v4.49A11
					Sleep(15);
				else
					Sleep(30);
			}
			else
			{
				Sleep(1);
			}
		//}
	}

	//CheckResult(nResult, _T("Y Axis - MoveTo"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT Y Move Failure(Y1_MoveTo)");
	}

	return nResult;
}

INT CWaferTable::Y2_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Y2)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableYMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_Y2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMoveTo(WT_AXIS_Y2, nPos, SFM_NOWAIT, &m_stWTAxis_Y2);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y2, 10000, &m_stWTAxis_Y2)) != gnOK)
					{
						nResult = Err_WTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			CMS896AStn::MotionClearError(WT_AXIS_Y2, &m_stWTAxis_Y2);

			m_bHome_Y2	= FALSE;
			nResult = Err_WTableYMove;
		}
	}
	else
	{
		//if (m_bSel_Y2)
		//{
			if (nMode == SFM_WAIT)
			{
				if (IsMS60() || IsMS90())	//v4.49A11
					Sleep(15);
				else
					Sleep(30);
			}
			else
			{
				Sleep(1);
			}
		//}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT Y Move Failure(Y2_MoveTo)");
	}
	return nResult;
}

INT CWaferTable::Y1_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_lMotorDirectionY == -1)
	{
		nPos = -1 * nPos;
	}

	if (m_fHardware && m_bSel_Y)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableYMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_Y)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				
				//if (IsMS60())
				//{
					LONG lCommandPos = CMS896AStn::MotionGetCommandPosition(WT_AXIS_Y, &m_stWTAxis_Y);
					LONG lDistance = nPos - lCommandPos;
					lDistance = abs(lDistance);
					Y1_SelectControlByDistance(abs(nPos));
				//}
				
				if (m_lX_ProfileType == MS896A_OBW_PROFILE)
				{
					CMS896AStn::MotionObwMove(WT_AXIS_Y, nPos, SFM_NOWAIT, &m_stWTAxis_Y);
				}
				else
				{
					CMS896AStn::MotionMove(WT_AXIS_Y, nPos, SFM_NOWAIT, &m_stWTAxis_Y);
				}

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y, 10000, &m_stWTAxis_Y)) != gnOK)
					{
						nResult = Err_WTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);

			m_bHome_Y	= FALSE;
			nResult = Err_WTableYMove;
		}
	}
	else
	{
		//if (m_bSel_Y)
		//{
			if (nMode == SFM_WAIT)
			{
				if (IsMS60() || IsMS90())	//v4.49A11
					Sleep(15);
				else
					Sleep(30);
			}
			else
			{
				Sleep(1);
			}
		//}
	}

	//CheckResult(nResult, _T("Y Axis - Move"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT Y Move Failure(Y1_Move)");
	}

	return nResult;
}

INT CWaferTable::Y1_ScfProfileMove(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableYMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_Y)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionScfProfileMove(WT_AXIS_Y, nPos, SFM_NOWAIT, &m_stWTAxis_Y);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y, 10000, &m_stWTAxis_Y)) != gnOK)
					{
						nResult = Err_WTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);

			m_bHome_Y	= FALSE;
			nResult = Err_WTableYMove;
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

	//CheckResult(nResult, _T("Y Axis - Move"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT Y Move Failure(Y1_ScfProfileMove)");
	}

	return nResult;
}

INT CWaferTable::Y2_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Y2)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableYMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_Y2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMove(WT_AXIS_Y2, nPos, SFM_NOWAIT, &m_stWTAxis_Y2);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y2, 10000, &m_stWTAxis_Y2)) != gnOK)
					{
						nResult = Err_WTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			CMS896AStn::MotionClearError(WT_AXIS_Y2, &m_stWTAxis_Y2);

			m_bHome_Y2	= FALSE;
			nResult = Err_WTableYMove;
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
		SetMotionCE(TRUE, "WT Y2 Move Failure(Y2_Move)");
	}
	return nResult;
}

INT CWaferTable::Y2_ScfProfileMove(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Y2)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableYMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_Y2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionScfProfileMove(WT_AXIS_Y2, nPos, SFM_NOWAIT, &m_stWTAxis_Y2);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y2, 10000, &m_stWTAxis_Y2)) != gnOK)
					{
						nResult = Err_WTableYMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			CMS896AStn::MotionClearError(WT_AXIS_Y2, &m_stWTAxis_Y2);

			m_bHome_Y2	= FALSE;
			nResult = Err_WTableYMove;
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
		SetMotionCE(TRUE, "WT Y2 Move Failure(Y2_ScfProfileMove)");
	}
	return nResult;
}

INT CWaferTable::Y1_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Y)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(WT_AXIS_Y, 10000, &m_stWTAxis_Y);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			nResult = gnNOTOK;
		}	
	}
	
	//CheckResult(nResult, _T("Y Axis - Sync"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT Y1 Sync(Y1_Sync)");
	}

	return nResult;
}

INT CWaferTable::Y2_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Y2)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(WT_AXIS_Y2, 10000, &m_stWTAxis_Y2);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			nResult = gnNOTOK;
		}	
	}
	
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT Y2 Sync(Y2_Sync)");
	}
	return nResult;
}

INT CWaferTable::Y_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y) == FALSE )
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_Y, &m_stWTAxis_Y);
					m_bIsPowerOn_Y = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y) == TRUE )
				{
					CMS896AStn::MotionPowerOff(WT_AXIS_Y, &m_stWTAxis_Y);
					m_bIsPowerOn_Y = FALSE;
					m_bComm_Y	= FALSE;
					m_bHome_Y	= FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			nResult = gnNOTOK;
		}	

	}
	//CheckResult(nResult, _T("Y Axis - PowerOn"));
	return nResult;
}

INT CWaferTable::Y2_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Y2)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2) == FALSE )
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2);
					m_bIsPowerOn_Y2 = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_Y2, &m_stWTAxis_Y2) == TRUE )
				{
					CMS896AStn::MotionPowerOff(WT_AXIS_Y2, &m_stWTAxis_Y2);
					m_bIsPowerOn_Y2 = FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			nResult = gnNOTOK;
		}	

	}
	return nResult;
}

INT CWaferTable::Y_Comm()
{
	if (m_fHardware && m_bSel_Y)
	{
		INT nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_Y, &m_stWTAxis_Y);
		if ( nResult == gnOK )
		{
			m_bComm_Y = TRUE;
		}	
		else
		{
			nResult	= Err_WTableYCommutate;
			CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
			m_bComm_Y = FALSE;
		}
	}
	return gnOK;
}

INT CWaferTable::Y2_Comm()
{
	if (m_fHardware && m_bSel_Y2)
	{
		INT nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_Y2, &m_stWTAxis_Y2);
		if ( nResult == gnOK )
		{
			m_bComm_Y2 = TRUE;
		}	
		else
		{
			nResult	= Err_WTableYCommutate;
			CMS896AStn::MotionClearError(WT_AXIS_Y2, &m_stWTAxis_Y2);
			m_bComm_Y2 = FALSE;
		}
	}
	return gnOK;
}


////////////////////////////////////////////
//	Theta  
////////////////////////////////////////////
BOOL CWaferTable::T_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWT)	//v3.61
	{
		return TRUE;
	}
	
	if (!m_bEnableWTTheta)
	{
		return TRUE;
	}


	try
	{
		return CMS896AStn::MotionIsPowerOn(WT_AXIS_T, &m_stWTAxis_T);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CWaferTable::T2_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWT)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(WT_AXIS_T2, &m_stWTAxis_T2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CWaferTable::T_Home(BOOL bFirstHome, BOOL bResetAlignFlag )
{
	INT nResult			= gnOK;
	if (m_fHardware && m_bSel_T && m_bEnableWTTheta)
	{
		if (IsBlkFunc2Enable())
		{
			//Force to align wafer  when WT Home
			SetAlignmentStatus(FALSE);
		}

		try
		{
			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_T, &m_stWTAxis_T) != 0)
			{
				m_bComm_T = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_T, &m_stWTAxis_T);
			}

			if (!m_bComm_T)
			{
				m_bHome_T			= FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_T, &m_stWTAxis_T)) == gnOK)
				{
					m_bComm_T		= TRUE;
				}
				else
				{
					nResult			= Err_WTableTCommutate;
					ClearServoError("srvWaferT");
				}
			}

			if (nResult == gnOK)
			{
				if ((nResult = CMS896AStn::MotionMoveHome(WT_AXIS_T, 1, 0, &m_stWTAxis_T)) == gnAMS_OK)
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(WT_AXIS_T, 0, &m_stWTAxis_T);
					m_bHome_T	= TRUE;
					(*m_psmfDevice)["Wafer"]["Rotate180Deg"] = (LONG) 0;
				
					//Move Theta to offset position (Load from config file)
					DOUBLE dHomeOffset = 0;	
					dHomeOffset = (DOUBLE)(GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_HOME_OFFSET));								

					T1_MoveTo((INT)(dHomeOffset / m_dThetaRes),SFM_WAIT);
					Sleep(100);
					CMS896AStn::MotionSetPosition(WT_AXIS_T, 0, &m_stWTAxis_T);

					//Get Working angle & Move to this working angle if not 1st time to home motor
					if ( (m_lWaferWorkingAngle != 0) && (bFirstHome == FALSE) )
					{
						T1_MoveTo((INT)((DOUBLE)m_lWaferWorkingAngle / m_dThetaRes),SFM_WAIT);
						Sleep(100);
						CMS896AStn::MotionSetPosition(WT_AXIS_T, 0, &m_stWTAxis_T);
					}
					
					if (bResetAlignFlag && IsAOIOnlyMachine()==FALSE )
					{
						//Reset global theta					
						m_lGlobalTheta = 0;
						SetAlignmentStatus(FALSE);
					}
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_WTableTMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
			CMS896AStn::MotionClearError(WT_AXIS_T, &m_stWTAxis_T);

			m_bComm_T	= FALSE;
			nResult = Err_WTableTMoveHome;
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
	
	//v4.48A9
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT T Home Failure(T_Home)");
	}
	else
	{
		SetMotionCE(FALSE, "WT T Home Sucess(T_Home)");
	}
	//CheckResult(nResult, _T("T Axis - Home"));
	return nResult;
}


INT CWaferTable::T2_Home(BOOL bFirstHome)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_T2)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_T2, &m_stWTAxis_T2) != 0)
			{
				m_bComm_T2 = FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_T2, &m_stWTAxis_T2);
			}

			if (!m_bComm_T2)
			{
				m_bHome_T2			= FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_T2, &m_stWTAxis_T2)) == gnOK)
				{
					m_bComm_T2		= TRUE;
				}
				else
				{
					nResult			= Err_WTableTCommutate;
					ClearServoError("srvWaferT2");
				}
			}

			if (nResult == gnOK)
			{
				if ((nResult = CMS896AStn::MotionMoveHome(WT_AXIS_T2, 1, 0, &m_stWTAxis_T2)) == gnAMS_OK)
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(WT_AXIS_T2, 0, &m_stWTAxis_T2);
					m_bHome_T2	= TRUE;
					//(*m_psmfDevice)["Wafer"]["Rotate180Deg"] = (LONG) 0;


					//Move Theta to offset position (Load from config file)
					DOUBLE dHomeOffset = 0;	
					dHomeOffset = (DOUBLE)(GetChannelInformation(MS896A_CFG_CH_WAFTABLE2_T, MS896A_CFG_CH_HOME_OFFSET));								

					Sleep(100);
					CMS896AStn::MotionSetPosition(WT_AXIS_T2, 0, &m_stWTAxis_T2);

					//Get Working angle & Move to this working angle if not 1st time to home motor
					if ( (m_lWaferWorkingAngle != 0) && (bFirstHome == FALSE) )
					{
						Sleep(100);
						CMS896AStn::MotionSetPosition(WT_AXIS_T2, 0, &m_stWTAxis_T2);
					}
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_WTableTMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
			CMS896AStn::MotionClearError(WT_AXIS_T2, &m_stWTAxis_T2);

			m_bComm_T2	= FALSE;
			nResult = Err_WTableTMoveHome;
		}
	}
	else
	{
		if (m_bSel_T2)
		{
			Sleep(100);
			m_bComm_T2	= TRUE;
			m_bHome_T2	= TRUE;
		}
	}
	
	return nResult;
}


INT CWaferTable::T1_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_T && m_bEnableWTTheta)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableTMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_T)
			{
				if ( CMS896AStn::MotionIsServo(WT_AXIS_T, &m_stWTAxis_T) )
				{
					nResult = gnNOTOK;
				}
				else
				{
					nResult	= T_Home();
				}
			}

			BOOL bMoveT = TRUE;

			if (nResult == gnOK && bMoveT)
			{
				CMS896AStn::MotionMoveTo(WT_AXIS_T, nPos, SFM_NOWAIT, &m_stWTAxis_T);
				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_T, 10000, &m_stWTAxis_T)) != gnOK)
					{
						nResult = Err_WTableTMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
			CMS896AStn::MotionClearError(WT_AXIS_T, &m_stWTAxis_T);

			m_bHome_T	= FALSE;
			nResult = Err_WTableTMove;
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
	CheckResult(nResult, _T("T Axis - MoveTo"));

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT T Move Failure(T1_MoveTo)");
	}

	return nResult;
}

INT CWaferTable::T2_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_T2)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableTMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_T2)
			{
				if ( CMS896AStn::MotionIsServo(WT_AXIS_T2, &m_stWTAxis_T2) )
				{
					nResult = gnNOTOK;
				}
				else
				{
					nResult	= T2_Home();
				}
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMoveTo(WT_AXIS_T2, nPos, SFM_NOWAIT, &m_stWTAxis_T2);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_T2, 10000, &m_stWTAxis_T2)) != gnOK)
					{
						nResult = Err_WTableTMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
			CMS896AStn::MotionClearError(WT_AXIS_T2, &m_stWTAxis_T2);

			m_bHome_T2	= FALSE;
			nResult = Err_WTableTMove;
		}
	}
	else
	{
		if (m_bSel_T2)
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
		SetMotionCE(TRUE, "WT T Move Failure(T2_MoveTo)");
	}
	return nResult;
}

INT CWaferTable::T1_Move(INT nPos, INT nMode, BOOL bProberManual)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_T && m_bEnableWTTheta)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableTMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_T)
			{
				if ( CMS896AStn::MotionIsServo(WT_AXIS_T, &m_stWTAxis_T) )
				{
					nResult = gnNOTOK;
				}
				else
				{
					nResult	= T_Home();
				}
			}
		
			BOOL bMoveT = TRUE;

			if (nResult == gnOK && bMoveT)
			{
				CMS896AStn::MotionMove(WT_AXIS_T, nPos, SFM_NOWAIT, &m_stWTAxis_T);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_T, 10000, &m_stWTAxis_T)) != gnOK)
					{
						nResult = Err_WTableTMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
			CMS896AStn::MotionClearError(WT_AXIS_T, &m_stWTAxis_T);

			m_bHome_T	= FALSE;
			nResult = Err_WTableTMove;
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
	
	//CheckResult(nResult, _T("T Axis - Move"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT T Move Failure(T1_Move)");
	}

	return nResult;
}

INT CWaferTable::T2_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_T2)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_WTableTMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_T2)
			{
				if ( CMS896AStn::MotionIsServo(WT_AXIS_T2, &m_stWTAxis_T2) )
				{
					nResult = gnNOTOK;
				}
				else
				{
					nResult	= T2_Home();
				}
			}
		
			if (nResult == gnOK)
			{
				CMS896AStn::MotionMove(WT_AXIS_T2, nPos, SFM_NOWAIT, &m_stWTAxis_T2);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_T2, 10000, &m_stWTAxis_T2)) != gnOK)
					{
						nResult = Err_WTableTMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
			CMS896AStn::MotionClearError(WT_AXIS_T2, &m_stWTAxis_T2);

			m_bHome_T2	= FALSE;
			nResult = Err_WTableTMove;
		}
	}
 	else
	{
		if (m_bSel_T2)
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
		SetMotionCE(TRUE, "WT T Move Failure(T2_Move)");
	}
	return nResult;
}

INT CWaferTable::T1_Sync()
{
	INT nResult = gnOK;
	if (!m_bEnableWTTheta)
	{
		return nResult;
	}

	try
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = gnNOTOK;
			return nResult;
		}

		CMS896AStn::MotionSync(WT_AXIS_T, 10000, &m_stWTAxis_T);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
		nResult = gnNOTOK;
	}	

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT T Sync Timeout(T1_Sync)");
	}
	return nResult;
}

INT CWaferTable::T2_Sync()
{
	INT nResult = gnOK;
	try
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = gnNOTOK;
			return nResult;
		}

		CMS896AStn::MotionSync(WT_AXIS_T2, 10000, &m_stWTAxis_T2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
		nResult = gnNOTOK;
	}	

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT T Sync Timeout(T2_Sync)");
	}
	return nResult;
}

INT CWaferTable::T_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_T && m_bEnableWTTheta)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_T, &m_stWTAxis_T) == FALSE )
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_T, &m_stWTAxis_T);
					m_bIsPowerOn_T = TRUE;
					m_bHome_T = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_T, &m_stWTAxis_T) == TRUE )
				{
					CMS896AStn::MotionPowerOff(WT_AXIS_T, &m_stWTAxis_T);
					m_bIsPowerOn_T = FALSE;
					m_bComm_T	= FALSE;
					m_bHome_T	= FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}

INT CWaferTable::T2_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_T2)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_T2, &m_stWTAxis_T2) == FALSE )
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_T2, &m_stWTAxis_T2);
					m_bIsPowerOn_T2 = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_T2, &m_stWTAxis_T2) == TRUE )
				{
					CMS896AStn::MotionPowerOff(WT_AXIS_T2, &m_stWTAxis_T2);
					m_bIsPowerOn_T2 = FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CWaferTable::T_Comm()
{
	if (m_fHardware && m_bSel_T && m_bEnableWTTheta)
	{
		INT nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_T, &m_stWTAxis_T);
		if ( nResult == gnOK )
		{
			m_bComm_T = TRUE;
		}	
		else
		{
			nResult	= Err_WTableTCommutate;
			CMS896AStn::MotionClearError(WT_AXIS_T, &m_stWTAxis_T);
			m_bComm_T = FALSE;
		}
	}
	return gnOK;
}

INT CWaferTable::T2_Comm()
{
	if (m_fHardware && m_bSel_T2)
	{
		INT nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_T2, &m_stWTAxis_T2);
		if ( nResult == gnOK )
		{
			m_bComm_T2 = TRUE;
		}	
		else
		{
			nResult	= Err_WTableTCommutate;
			CMS896AStn::MotionClearError(WT_AXIS_T2, &m_stWTAxis_T2);
			m_bComm_T2 = FALSE;
		}
	}
	return gnOK;
}


////////////////////////////////////////////
//	Mutliple Axis 
////////////////////////////////////////////

BOOL CWaferTable::HomeTable1(BOOL bToHome)
{
	if (!m_bUseDualTablesOption)
		return TRUE;

	X1_Profile(LOW_PROF1);
	Y1_Profile(LOW_PROF1);

	if( bToHome )
	{
		X1_MoveTo(0, FALSE);
		Y1_MoveTo(0, TRUE);
	}
	else
	{
	//	WFTTGoHomeUnderCam(FALSE);
		X1_MoveTo(m_lUnloadPhyPosX, FALSE);
		Y1_MoveTo(m_lUnloadPhyPosY, TRUE);
	}
	X1_Sync();

	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
	return TRUE;
}


BOOL CWaferTable::HomeTable2(BOOL bToHome)
{
	if( IsESDualWT()==FALSE )
		return TRUE;

	X2_Profile(LOW_PROF1);
	Y2_Profile(LOW_PROF1);

	if( bToHome )
	{
		X2_MoveTo(0, FALSE);
		Y2_MoveTo(0, TRUE);
	}
	else
	{
	//	WFTTGoHomeUnderCam(TRUE);
		X2_MoveTo(m_lUnloadPhyPosX2, FALSE);
		Y2_MoveTo(m_lUnloadPhyPosY2, TRUE);
	}
	X2_Sync();

	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);
	return TRUE;
}


INT CWaferTable::XY_Home()
{
	INT nResult			= gnOK;
	CString szCurrentAxis = "";

	if (m_fHardware && m_bSel_X && m_bSel_Y)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_X, &m_stWTAxis_X) != 0)
			{
				m_bComm_X	= FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
			}

			if (CMS896AStn::MotionReportErrorStatus(WT_AXIS_Y, &m_stWTAxis_Y) != 0)
			{
				m_bComm_Y	= FALSE;
				CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
			}

			if (!m_bComm_X)
			{
				m_bHome_X	= FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_X, &m_stWTAxis_X)) == gnOK)
				{
					m_bComm_X	= TRUE;
				}
				else
				{
					nResult		= Err_WTableXCommutate;
					CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
				}
			}

			if (!m_bComm_Y && (nResult == gnOK))
			{
				m_bHome_X	= FALSE;

				if ((nResult = CMS896AStn::MotionCommutateServo(WT_AXIS_Y, &m_stWTAxis_Y)) == gnOK)
				{
					m_bComm_Y	= TRUE;
				}
				else
				{
					nResult		= Err_WTableYCommutate;
					CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
				}
			}

			if (nResult == gnOK)
			{
				if ((nResult = CMS896AStn::MotionMoveHome(WT_AXIS_X, SFM_NOWAIT, 0, &m_stWTAxis_X)) == gnOK)
				{
					if ((nResult = CMS896AStn::MotionMoveHome(WT_AXIS_Y, 1, 0, &m_stWTAxis_Y)) != gnOK)
					{
						nResult = Err_WTableYMoveHome;
					}
				}
				else
				{
					nResult = Err_WTableXMoveHome;
				}

				if (nResult == gnOK)
				{
					if ((nResult = CMS896AStn::MotionSync(WT_AXIS_X, 10000, &m_stWTAxis_X)) == gnOK)
					{
						szCurrentAxis = WT_AXIS_X;
#ifndef NU_MOTION
						CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, "spfWaferXIndex", &m_stWTAxis_X);
						CMS896AStn::MotionSearch(WT_AXIS_X, 1, SFM_WAIT, &m_stWTAxis_X);
#endif

						Sleep(100);
						CMS896AStn::MotionSetPosition(WT_AXIS_X, 0, &m_stWTAxis_X);
						m_bHome_X	= TRUE;

						szCurrentAxis = WT_AXIS_Y;
#ifndef NU_MOTION
						CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y, "spfWaferYIndex", &m_stWTAxis_Y);
						CMS896AStn::MotionSearch(WT_AXIS_Y, 1, SFM_WAIT, &m_stWTAxis_Y);
#endif
						Sleep(100);
						CMS896AStn::MotionSetPosition(WT_AXIS_Y, 0, &m_stWTAxis_Y);
						m_bHome_Y	= TRUE;
					}
					else
					{
						nResult = Err_WTableXMoveHome;
					}
				}	
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			if (szCurrentAxis == WT_AXIS_X)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_X);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_Y);
			
			CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
			CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);

			m_bComm_X	= FALSE;
			m_bComm_Y	= FALSE;

			nResult = Err_WTableXMoveHome;
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

	//CheckResult(nResult, _T("XY Axis - Home"));
	return nResult;
}

INT CWaferTable::XY_SMoveTo(INT nPosX, INT nPosY, INT nMode)	//v4.11T3
{
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
	INT nStatus = XY_MoveTo(nPosX, nPosY, nMode);
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	return nStatus;
}

INT CWaferTable::XY1_MoveTo(INT nPosX, INT nPosY, INT nMode)
{
	INT nResult			= gnOK;
	CString	szCurrentAxis = "";						

	if (m_fHardware && m_bSel_X && m_bSel_Y)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			SetErrorMessage("WT: Critical error detected in XY1 MoveTo");	//v4.24
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			if (!m_bHome_X || !m_bHome_Y)
			{
				SetErrorMessage("WT: XY detected not HOME in XY1 MoveTo");	//v4.24
				nResult = gnNOTOK;
			}

			//v4.24T10
			if (m_bUseDualTablesOption)
			{
				if ( (nPosX < m_lXNegLimit) || (nPosX > m_lXPosLimit) )
				{
					CString szErr;
					szErr.Format("XY1_MoveTo: X1 exceeds limit: %ld (%ld, %ld)", nPosX, m_lXNegLimit, m_lXPosLimit);
					SetErrorMessage(szErr);
					nResult = gnNOTOK;
				}
				if ( (nPosY < m_lYNegLimit) || (nPosY > m_lYPosLimit) )
				{
					CString szErr;
					szErr.Format("XY1_MoveTo: Y1 exceeds limit: %ld (%ld, %ld)", nPosY, m_lYNegLimit, m_lYPosLimit);
					HmiMessage_Red_Yellow(szErr);
					SetErrorMessage(szErr);
					nResult = gnNOTOK;
				}
			}

			if (nResult == gnOK)
			{
				if (nPosX != WT_NOT_MOVE)
				{
					szCurrentAxis = WT_AXIS_X;
					if (m_lX_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMoveTo(WT_AXIS_X, nPosX, SFM_NOWAIT, &m_stWTAxis_X);
					}
					else
					{
						CMS896AStn::MotionMoveTo(WT_AXIS_X, nPosX, SFM_NOWAIT, &m_stWTAxis_X);
					}
				}

				if (nPosY != WT_NOT_MOVE)
				{
					szCurrentAxis = WT_AXIS_Y;
					if (m_lY_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMoveTo(WT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stWTAxis_Y);
					}
					else
					{
						CMS896AStn::MotionMoveTo(WT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stWTAxis_Y);
					}
				}

				if (nMode == SFM_WAIT)
				{
					if ((nResult == gnOK) && (nPosX != WT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(WT_AXIS_X, 5000, &m_stWTAxis_X)) != gnOK)		//v4.24T10		//Increase Syn motion timeout from 1 to 3sec for 9Inch option
						{
							SetErrorMessage("WT: MotionSync X error in XY1 MoveTo");	//v4.24T10
							nResult	= Err_WTableXMove;
						}
					}

					if ((nResult == gnOK) && (nPosY != WT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y, 5000, &m_stWTAxis_Y)) != gnOK)		//v4.24T10		//Increase Syn motion timeout from 1 to 3sec for 9Inch option
						{
							SetErrorMessage("WT: MotionSync Y error in XY1 MoveTo");	//v4.24T10
							nResult = Err_WTableYMove;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult(szCurrentAxis);
			if (szCurrentAxis == WT_AXIS_X)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_X);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_Y);
			
			if (nPosX != WT_NOT_MOVE)
			{
				CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
				m_bHome_X	= FALSE;
			}

			if (nPosY != WT_NOT_MOVE)
			{
				CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
				m_bHome_Y	= FALSE;
			}

			nResult	= Err_WTableXYMove;
		}
	}
	else
	{
		if (nMode == SFM_WAIT)
		{
			if (IsMS60() || IsMS90())	//v4.49A11
				Sleep(15);
			else
				Sleep(30);
		}
		else
		{
			Sleep(1);
		}
	}

	//CheckResult(nResult, _T("XY Axis - MoveTo"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT XY Move Failure(XY1_MoveTo)");
	}

	return nResult;
}


INT CWaferTable::XY_Move(INT nPosX, INT nPosY, INT nMode, BOOL b4WT2)
{
	INT nResult			= gnOK;
	CString szCurrentAxis = "";

	if( b4WT2 )	// table 2
	{
		if (m_fHardware && m_bSel_X2 && m_bSel_Y2)
		{
			nResult = X2_Move(nPosX, SFM_NOWAIT);
			if( nResult==gnOK )
				nResult = Y2_Move(nPosY, SFM_NOWAIT);
			if( nResult==gnOK )
				nResult = X2_Sync();
			if( nResult==gnOK )
				nResult = Y2_Sync();
		}
	}
	else	// table 1
	{
		if (m_fHardware && m_bSel_X && m_bSel_Y)
		{
			//v4.12T1	//Tyntek
			if ( IsMotionCE() == TRUE )
			{
				nResult = gnNOTOK;
				return nResult;
			}

			try
			{
				if (!m_bHome_X || !m_bHome_Y)
				{
					nResult = gnNOTOK;
				}

				if (nResult == gnOK)
				{
					if (nPosX != WT_NOT_MOVE)
					{
						szCurrentAxis = WT_AXIS_X;
						if (m_lX_ProfileType == MS896A_OBW_PROFILE)
						{
							CMS896AStn::MotionObwMove(WT_AXIS_X, nPosX, SFM_NOWAIT, &m_stWTAxis_X);
						}
						else
						{
							CMS896AStn::MotionMove(WT_AXIS_X, nPosX, SFM_NOWAIT, &m_stWTAxis_X);
						}
					}

					if (nPosY != WT_NOT_MOVE)
					{
						szCurrentAxis = WT_AXIS_Y;
						if (m_lY_ProfileType == MS896A_OBW_PROFILE)
						{
							CMS896AStn::MotionObwMove(WT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stWTAxis_Y);
						}
						else
						{
							CMS896AStn::MotionMove(WT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stWTAxis_Y);
						}
					}

					if (nMode == SFM_WAIT)
					{
						if ((nResult == gnOK) && (nPosX != WT_NOT_MOVE))
						{
						if ((nResult = CMS896AStn::MotionSync(WT_AXIS_X, 1000, &m_stWTAxis_X)) != gnOK)
							{
								nResult = Err_WTableXMove;
							}
						}

						if ((nResult == gnOK) && (nPosY != WT_NOT_MOVE))
						{
							if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y, 1000, &m_stWTAxis_Y)) != gnOK)
							{
								nResult = Err_WTableYMove;
							}
						}
					}
				}
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				if (szCurrentAxis == WT_AXIS_X)
					CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_X);
				else
					CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_Y);

				if (nPosX != WT_NOT_MOVE)
				{
					CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
					m_bHome_X	= FALSE;
				}

				if (nPosY != WT_NOT_MOVE)
				{
					CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
					m_bHome_Y	= FALSE;
				}

				nResult	= Err_WTableXYMove;
			}
		}
		else
		{
			if (nMode == SFM_WAIT)
			{
				if (IsMS60() || IsMS90())	//v4.49A11
					Sleep(15);
				else
					Sleep(30);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	//CheckResult(nResult, _T("XY Axis - Move"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT XY Move Failure(XY_Move)");
	}

	return nResult;
}

INT CWaferTable::XYT_Home()
{
	INT nResult			= gnOK;

	if( FALSE )	// table 2	4.24TX1
	{
		if (m_fHardware && m_bSel_X2 && m_bSel_Y2 && m_bSel_T2)
		{
			try
			{
				if (!m_bHome_X2 || !m_bHome_Y2)
				{
					nResult = gnNOTOK;
				}

				if ((nResult == gnOK) && !m_bHome_T2)
				{
					nResult = gnNOTOK;
				}
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				nResult	= Err_WTableXYTMoveHome;
			}
		}
	}
	else	// table 1
	{
		if (m_fHardware && m_bSel_X && m_bSel_Y && m_bSel_T)
		{
			try
			{
				if (!m_bHome_X || !m_bHome_Y)
				{
					nResult = gnNOTOK;
				}

				if ((nResult == gnOK) && !m_bHome_T)
				{
					nResult = gnNOTOK;
				}
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				nResult	= Err_WTableXYTMoveHome;
			}
		}
	}

	return nResult;
}

INT CWaferTable::XYT_MoveTo(INT nPosX, INT nPosY, INT nPosT, INT nMode)
{
	INT nResult			= gnOK;
	CString szCurrentAxis = "";

	if (m_fHardware && m_bSel_X && m_bSel_Y && m_bSel_T)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			if (!m_bHome_X || !m_bHome_Y)
			{
				nResult = gnNOTOK;
			}

			if ((nResult == gnOK) && !m_bHome_T)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (nPosX != WT_NOT_MOVE)
				{
					szCurrentAxis = WT_AXIS_X;
					if (m_lX_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMoveTo(WT_AXIS_X, nPosX, SFM_NOWAIT, &m_stWTAxis_X);
					}
					else
					{
						CMS896AStn::MotionMoveTo(WT_AXIS_X, nPosX, SFM_NOWAIT, &m_stWTAxis_X);
					}
				}

				if (nPosY != WT_NOT_MOVE)
				{
					szCurrentAxis = WT_AXIS_Y;
					if (m_lY_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMoveTo(WT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stWTAxis_Y);
					}
					else
					{
						CMS896AStn::MotionMoveTo(WT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stWTAxis_Y);
					}
				}

				if (nPosT != WT_NOT_MOVE)
				{
					szCurrentAxis = WT_AXIS_T;
					CMS896AStn::MotionMoveTo(WT_AXIS_T, nPosT, SFM_NOWAIT, &m_stWTAxis_T);
				}

				if (nMode == SFM_WAIT)
				{
					if ((nResult == gnOK) && (nPosT != WT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(WT_AXIS_T, 1000, &m_stWTAxis_T)) != gnOK)
						{
							nResult = Err_WTableTMove;
						}
					}

					if ((nResult == gnOK) && (nPosX != WT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(WT_AXIS_X, 1000, &m_stWTAxis_X)) != gnOK)
						{
							nResult = Err_WTableXMove;
						}
					}

					if ((nResult == gnOK) && (nPosY != WT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y, 1000, &m_stWTAxis_Y)) != gnOK)
						{
							nResult = Err_WTableYMove;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			if (szCurrentAxis == WT_AXIS_X)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_X);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_Y);

			if (nPosX != WT_NOT_MOVE)
			{
				CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
				m_bHome_X	= FALSE;
			}

			if (nPosY != WT_NOT_MOVE)
			{
				CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
				m_bHome_Y	= FALSE;
			}

			if (nPosT != WT_NOT_MOVE)
			{
				CMS896AStn::MotionClearError(WT_AXIS_T, &m_stWTAxis_T);

				m_bHome_T	= FALSE;
			}

			nResult	= Err_WTableXYTMove;
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

	//CheckResult(nResult, _T("XYT Axis - Move"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT XY Move Failure(XY_MoveTo)");
	}

	return nResult;
}

INT CWaferTable::XYT_Move(INT nPosX, INT nPosY, INT nPosT, INT nMode)
{
	INT nResult			= gnOK;
	CString szCurrentAxis = "";

	if (m_fHardware && m_bSel_X && m_bSel_Y && m_bSel_T)
	{
		//v4.12T1	//Tyntek
		if ( IsMotionCE() == TRUE )
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			if (!m_bHome_X || !m_bHome_Y)
			{
				nResult = gnNOTOK;
			}

			if ((nResult == gnOK) && !m_bHome_T)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (nPosX != WT_NOT_MOVE)
				{
					szCurrentAxis = WT_AXIS_X;
					if (m_lX_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMove(WT_AXIS_X, nPosX, SFM_NOWAIT, &m_stWTAxis_X);
					}
					else
					{
						CMS896AStn::MotionMove(WT_AXIS_X, nPosX, SFM_NOWAIT, &m_stWTAxis_X);
					}
				}

				if (nPosY != WT_NOT_MOVE)
				{
					szCurrentAxis = WT_AXIS_Y;
					if (m_lY_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMove(WT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stWTAxis_Y);
					}
					else
					{
						CMS896AStn::MotionMove(WT_AXIS_Y, nPosY, SFM_NOWAIT, &m_stWTAxis_Y);
					}
				}

				if (nPosT != WT_NOT_MOVE)
				{
					szCurrentAxis = WT_AXIS_T;
					CMS896AStn::MotionMove(WT_AXIS_T, nPosT, SFM_NOWAIT, &m_stWTAxis_T);
				}

				if (nMode == SFM_WAIT)
				{
					if ((nResult == gnOK) && (nPosT != WT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(WT_AXIS_T, 1000, &m_stWTAxis_T)) != gnOK)
						{
							nResult = Err_WTableTMove;
						}
					}

					if ((nResult == gnOK) && (nPosX != WT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(WT_AXIS_X, 1000, &m_stWTAxis_X)) != gnOK)
						{
							nResult = Err_WTableXMove;
						}
					}

					if ((nResult == gnOK) && (nPosY != WT_NOT_MOVE))
					{
						if ((nResult = CMS896AStn::MotionSync(WT_AXIS_Y, 1000, &m_stWTAxis_Y)) != gnOK)
						{
							nResult = Err_WTableYMove;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			if (szCurrentAxis == WT_AXIS_X)
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_X);
			else
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_Y);

			if (nPosX != WT_NOT_MOVE)
			{
				CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
				m_bHome_X	= FALSE;
			}

			if (nPosY != WT_NOT_MOVE)
			{
				CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
				m_bHome_Y	= FALSE;
			}

			if (nPosT != WT_NOT_MOVE)
			{
				CMS896AStn::MotionClearError(WT_AXIS_T, &m_stWTAxis_T);

				m_bHome_T	= FALSE;
			}

			nResult	= Err_WTableXYTMove;
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

	//CheckResult(nResult, _T("XYT Axis - Move"));
	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "WT XYT Move Failure(XYT_Move)");
	}

	return nResult;
}


// Turn on or off the joystick
// Remark: After joystick off, the motors are still power on
VOID CWaferTable::SetJoystickOn(BOOL bOn, BOOL bIsWT2)			
{
	CString szCurrentAxis = "";

	if ( !m_fHardware )
		return;
	
	if (m_bDisableWT)		//v3.61
		return;

	if ( GetLockJoystick() == TRUE)
	{
		bOn = FALSE;
	}

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		if( IsWT2InUse() || bIsWT2 == TRUE)	//	ES101_XU may be need to pass in WT2 in some case such as setup
		{
			m_lJsWftInUse = 1;	//WT2	
		}
		else
		{
			m_lJsWftInUse = 0;	//WT1
		}

		m_lJsTableMode	= 0;		//WT
		m_bJoystickOn	= bOn;
		m_bXJoystickOn	= bOn;
		m_bYJoystickOn	= bOn;

		return;
	}

	try
	{
		if (bOn == TRUE)		// Turn on
		{					
			if ( m_bSel_X )
			{
				szCurrentAxis = WT_AXIS_X;
				if (CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X) == FALSE )
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_X, &m_stWTAxis_X);
					m_bIsPowerOn_X = TRUE;
				}
				CMS896AStn::MotionJoyStickOn(WT_AXIS_X, bOn, &m_stWTAxis_X);
			}

			if ( m_bSel_Y )
			{
				szCurrentAxis = WT_AXIS_Y;
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y) == FALSE )
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_Y, &m_stWTAxis_Y);
					m_bIsPowerOn_Y = TRUE;
				}
				CMS896AStn::MotionJoyStickOn(WT_AXIS_Y, bOn, &m_stWTAxis_Y);
			}
		}
		else					// Turn off
		{
			// Even turn off the joystick, don't turn off the motor power
			szCurrentAxis = WT_AXIS_X;
			CMS896AStn::MotionJoyStickOn(WT_AXIS_X, bOn, &m_stWTAxis_X);

			szCurrentAxis = WT_AXIS_Y;
			CMS896AStn::MotionJoyStickOn(WT_AXIS_Y, bOn, &m_stWTAxis_Y);
		}
		m_bJoystickOn = bOn;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		if (szCurrentAxis == WT_AXIS_X)
			CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_X);
		else
			CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_Y);
		//throw;	//v3.94
	}

}


VOID CWaferTable::X_SetJoystickOn(BOOL bXOn, BOOL bIsWT2)
{
	CString szCurrentAxis = "";

	if ( !m_fHardware )
		return;
	if (m_bDisableWT)		//v3.61
		return;

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		if( IsWT2InUse() || bIsWT2)
		{
			m_lJsWftInUse = 1;	//WT2
		}
		else
		{
			m_lJsWftInUse = 0;	//WT1
		}

		m_lJsTableMode = 0;		//WT
		m_bXJoystickOn = bXOn;
		return;
	}

	try
	{
		if (bXOn == TRUE)		// Turn on
		{				
			if ( m_bSel_X )
			{
				szCurrentAxis = WT_AXIS_X;
				if (CMS896AStn::MotionIsPowerOn(WT_AXIS_X, &m_stWTAxis_X) == FALSE )
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_X, &m_stWTAxis_X);
					m_bIsPowerOn_X = TRUE;
				}
				CMS896AStn::MotionJoyStickOn(WT_AXIS_X, bXOn, &m_stWTAxis_X);
			}
		}
		else					// Turn off
		{
			// Even turn off the joystick, don't turn off the motor power
			szCurrentAxis = WT_AXIS_X;
			CMS896AStn::MotionJoyStickOn(WT_AXIS_X, bXOn, &m_stWTAxis_X);

		}
		m_bJoystickOn = bXOn;
		m_bXJoystickOn = bXOn;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_X);
		//throw;	//v3.94
	}
} //end X_SetJoystickOn


VOID CWaferTable::Y_SetJoystickOn(BOOL bYOn, BOOL bIsWT2)
{
	CString szCurrentAxis = "";

	if ( !m_fHardware )
		return;
	if (m_bDisableWT)		//v3.61
		return;

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		if( IsWT2InUse() || bIsWT2)
		{
			m_lJsWftInUse = 1;	//WT2
		}
		else
		{
			m_lJsWftInUse = 0;	//WT1
		}

		m_lJsTableMode = 0;		//WT
		m_bYJoystickOn = bYOn;
		return;
	}

	try
	{
		if (bYOn == TRUE)		// Turn on
		{		
			if ( m_bSel_Y )
			{
				szCurrentAxis = WT_AXIS_Y;
				if ( CMS896AStn::MotionIsPowerOn(WT_AXIS_Y, &m_stWTAxis_Y) == FALSE )
				{
					CMS896AStn::MotionPowerOn(WT_AXIS_Y, &m_stWTAxis_Y);
					m_bIsPowerOn_Y = TRUE;
				}
				CMS896AStn::MotionJoyStickOn(WT_AXIS_Y, bYOn, &m_stWTAxis_Y);
			}
		}
		else					// Turn off
		{
			// Even turn off the joystick, don't turn off the motor power
			szCurrentAxis = WT_AXIS_Y;
			CMS896AStn::MotionJoyStickOn(WT_AXIS_Y, bYOn, &m_stWTAxis_Y);
		}
		m_bJoystickOn = bYOn;
		m_bYJoystickOn = bYOn;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stWTAxis_Y);
		//throw;		//v3.94
	}
} //end Y_SetJoystickOn


//----- Search the barcode in X direction -----//
INT CWaferTable::SearchBarcodeMoveX(INT siDirectionX, LONG lSearchDistanceX, BOOL bWaitComplete)
{
	INT nReturn = gnOK;

	if (m_fHardware && !m_bDisableWT && m_bSel_X)
	{
		try
		{
			LONG lMoveTime = 2000;
#ifdef NU_MOTION
			if (lSearchDistanceX !=0)
			{
				X1_Move(lSearchDistanceX);
				lMoveTime = X1_ProfileTime(m_nProfile_X, lSearchDistanceX, lSearchDistanceX);
			}
#else
			if (lSearchDistanceX !=0)
			{
				CMS896AStn::MotionSelectSearchProfile(WT_AXIS_X, "spfWaferXSearch", &m_stWTAxis_X);
				CMS896AStn::MotionSetSearchLimit(WT_AXIS_X, "spfWaferXSearch", lSearchDistanceX, &m_stWTAxis_X);
				CMS896AStn::MotionSearch(WT_AXIS_X, siDirectionX, SFM_NOWAIT, &m_stWTAxis_X, "spfWaferXSearch");
			}
#endif
			(*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"] = lMoveTime;
			if (bWaitComplete)
			{
				X1_Sync();
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			CMS896AStn::MotionClearError(WT_AXIS_X, &m_stWTAxis_X);
			nReturn = Err_WTableSearchBarCodeFailed;
		}
	}

	return nReturn;
}

// ------- Search the barcode in Y direction -------//
INT CWaferTable::SearchBarcodeMoveY(INT siDirectionY, LONG lSearchDistanceY, BOOL bWaitComplete)
{
	INT nReturn = gnOK;

	if (m_fHardware && !m_bDisableWT && m_bSel_Y)
	{
		try
		{
			LONG lMoveTime = 2000;
#ifdef NU_MOTION
			if (lSearchDistanceY !=0)
			{
				Y1_Move(lSearchDistanceY);
				lMoveTime = Y1_ProfileTime(m_nProfile_Y, lSearchDistanceY, lSearchDistanceY);
			}
#else
			if (lSearchDistanceY !=0)
			{
				CMS896AStn::MotionSelectSearchProfile(WT_AXIS_Y,"spfWaferYSearch", &m_stWTAxis_Y);
				CMS896AStn::MotionSetSearchLimit(WT_AXIS_Y, "spfWaferYSearch", lSearchDistanceY, &m_stWTAxis_Y);
				CMS896AStn::MotionSearch(WT_AXIS_Y, siDirectionY, SFM_NOWAIT, &m_stWTAxis_Y, "spfWaferYSearch");
			}
#endif
			(*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"] = lMoveTime;
			if (bWaitComplete)
			{
				Y1_Sync();
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			CMS896AStn::MotionClearError(WT_AXIS_Y, &m_stWTAxis_Y);
			nReturn = Err_WTableSearchBarCodeFailed;
		}
	}

	return nReturn;
}

//----- Search the barcode in T direction -----//
INT CWaferTable::SearchBarcodeMoveT(INT siDirectionT, LONG lSearchDistanceT, BOOL bWaitComplete)
{
	INT nReturn = gnOK;

	if (m_fHardware && !m_bDisableWT && m_bSel_T)
	{
		try
		{
			LONG lMoveTime = 2000;
#ifdef NU_MOTION
			if (lSearchDistanceT !=0)
			{
				T1_Profile(LOW_PROF);
				T1_Move(lSearchDistanceT, bWaitComplete);
				T1_Profile(NORMAL_PROF);

				lMoveTime = T1_ProfileTime(LOW_PROF, lSearchDistanceT, lSearchDistanceT);
			}
#else
			if (lSearchDistanceT !=0)
			{
				CMS896AStn::MotionSelectSearchProfile(WT_AXIS_T,"spfWaferTSearch", &m_stWTAxis_T);
				CMS896AStn::MotionSetSearchLimit(WT_AXIS_T, "spfWaferTSearch", lSearchDistanceT, &m_stWTAxis_T);
				CMS896AStn::MotionSearch(WT_AXIS_T, siDirectionT, SFM_NOWAIT, &m_stWTAxis_T, "spfWaferTSearch");
			}
#endif
			(*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"] = lMoveTime;
			if (bWaitComplete)
			{
				T1_Sync();
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
			CMS896AStn::MotionClearError(WT_AXIS_T, &m_stWTAxis_T);
			nReturn = Err_WTableSearchBarCodeFailed;
		}
	}

	return nReturn;
}

//----- Search the barcode in X direction -----//
INT CWaferTable::SearchBarcodeMoveX2(INT siDirectionX, LONG lSearchDistanceX, BOOL bWaitComplete)
{
	INT nReturn = gnOK;

	if (m_fHardware && !m_bDisableWT && m_bSel_X2)
	{
		try
		{
			if (lSearchDistanceX !=0)
			{
				X2_Move(lSearchDistanceX, bWaitComplete);
			}
			LONG lMoveTime = X2_ProfileTime(m_nProfile_X2, lSearchDistanceX, lSearchDistanceX);
			(*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"] = lMoveTime;
			if (bWaitComplete)
			{
				X2_Sync();
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
			CMS896AStn::MotionClearError(WT_AXIS_X2, &m_stWTAxis_X2);
			nReturn = Err_WTableSearchBarCodeFailed;
		}
	}

	return nReturn;
}

// ------- Search the barcode in Y direction -------//
INT CWaferTable::SearchBarcodeMoveY2(INT siDirectionY, LONG lSearchDistanceY, BOOL bWaitComplete)
{
	INT nReturn = gnOK;

	if (m_fHardware && !m_bDisableWT && m_bSel_Y2)
	{
		try
		{
			if (lSearchDistanceY !=0)
			{
				Y2_Move(lSearchDistanceY, bWaitComplete);
			}
			LONG lMoveTime = Y2_ProfileTime(m_nProfile_Y2, lSearchDistanceY, lSearchDistanceY);
			(*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"] = lMoveTime;
			if (bWaitComplete)
			{
				Y2_Sync();
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			CMS896AStn::MotionClearError(WT_AXIS_Y2, &m_stWTAxis_Y2);
			nReturn = Err_WTableSearchBarCodeFailed;
		}
	}

	return nReturn;
}

INT CWaferTable::SearchBarcodeMoveT2(INT siDirectionT, LONG lSearchDistanceT, BOOL bWaitComplete)
{
	INT nReturn = gnOK;

	if (m_fHardware && !m_bDisableWT && m_bSel_T2)
	{
		try
		{
			if (lSearchDistanceT !=0)
			{
				T2_Profile(LOW_PROF);
				T2_Move(lSearchDistanceT, bWaitComplete);
				T2_Profile(NORMAL_PROF);
			}
			LONG lMoveTime = T2_ProfileTime(LOW_PROF, lSearchDistanceT, lSearchDistanceT);
			(*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"] = lMoveTime;

			if (bWaitComplete)
			{
				T2_Sync();
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
			CMS896AStn::MotionClearError(WT_AXIS_T2, &m_stWTAxis_T2);
			nReturn = Err_WTableSearchBarCodeFailed;
		}
	}

	return nReturn;
}


INT CWaferTable::X_SelectControl(INT nControlID)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

#ifdef NU_MOTION
	if (m_fHardware && !m_bDisableWT)
	{
		try
		{
			switch (nControlID)
			{
				case PL_STATIC:
					szNuControlParaID = m_stWTAxis_X.m_stControl[PL_STATIC].m_szID;
					break;

				case PL_STATIC1:
					szNuControlParaID = m_stWTAxis_X.m_stControl[PL_STATIC1].m_szID;
					break;

				case PL_DYNAMIC:
					szNuControlParaID = m_stWTAxis_X.m_stControl[PL_DYNAMIC].m_szID;
					break;

				case PL_DYNAMIC1:
					szNuControlParaID = m_stWTAxis_X.m_stControl[PL_DYNAMIC1].m_szID;
					break;

				case PL_DYNAMIC2:
					szNuControlParaID = m_stWTAxis_X.m_stControl[PL_DYNAMIC2].m_szID;
					break;
			
				default:
					szNuControlParaID = m_stWTAxis_X.m_stControl[PL_DYNAMIC].m_szID;
			}

			CMS896AStn::MotionSelectControlParam(WT_AXIS_X, "", &m_stWTAxis_X, szNuControlParaID);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	CheckResult(nResult, _T("WTX Axis - Select Control"));
#endif
	return nResult;
}

INT CWaferTable::X2_SelectControl(INT nControlID)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

	if (m_fHardware && !m_bDisableWT && IsESDualWT())
	{
		try
		{
			switch (nControlID)
			{
			case PL_STATIC:
				szNuControlParaID = m_stWTAxis_X2.m_stControl[PL_STATIC].m_szID;
				break;

			case PL_DYNAMIC:
				szNuControlParaID = m_stWTAxis_X2.m_stControl[PL_DYNAMIC].m_szID;
				break;

			case PL_DYNAMIC1:
				szNuControlParaID = m_stWTAxis_X2.m_stControl[PL_DYNAMIC1].m_szID;
				break;
			
			default:
				szNuControlParaID = m_stWTAxis_X2.m_stControl[PL_DYNAMIC].m_szID;
			}

			CMS896AStn::MotionSelectControlParam(WT_AXIS_X2, "", &m_stWTAxis_X2, szNuControlParaID);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}


INT CWaferTable::Y_SelectControl(INT nControlID)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

#ifdef NU_MOTION
	if (m_fHardware && !m_bDisableWT)
	{
		try
		{
			switch (nControlID)
			{
			case PL_STATIC:
				szNuControlParaID = m_stWTAxis_Y.m_stControl[PL_STATIC].m_szID;
				break;

			case PL_STATIC1:
				szNuControlParaID = m_stWTAxis_Y.m_stControl[PL_STATIC1].m_szID;
				break;

			case PL_DYNAMIC:
				szNuControlParaID = m_stWTAxis_Y.m_stControl[PL_DYNAMIC].m_szID;
				break;

			case PL_DYNAMIC1:
				szNuControlParaID = m_stWTAxis_Y.m_stControl[PL_DYNAMIC1].m_szID;
				break;

			case PL_DYNAMIC2:
				szNuControlParaID = m_stWTAxis_Y.m_stControl[PL_DYNAMIC2].m_szID;
				break;

			default:
				szNuControlParaID = m_stWTAxis_Y.m_stControl[PL_DYNAMIC].m_szID;
			}

			CMS896AStn::MotionSelectControlParam(WT_AXIS_Y, "", &m_stWTAxis_Y, szNuControlParaID);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	CheckResult(nResult, _T("WTY Axis - Select Control"));
#endif
	return nResult;
}

INT CWaferTable::Y2_SelectControl(INT nControlID)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

	if (m_fHardware && !m_bDisableWT && IsESDualWT())
	{
		try
		{
			switch (nControlID)
			{
			case PL_STATIC:
				szNuControlParaID = m_stWTAxis_Y2.m_stControl[PL_STATIC].m_szID;
				break;

			case PL_DYNAMIC:
				szNuControlParaID = m_stWTAxis_Y2.m_stControl[PL_DYNAMIC].m_szID;
				break;

			case PL_DYNAMIC1:
				szNuControlParaID = m_stWTAxis_Y2.m_stControl[PL_DYNAMIC1].m_szID;
				break;

			default:
				szNuControlParaID = m_stWTAxis_Y2.m_stControl[PL_DYNAMIC].m_szID;
			}

			CMS896AStn::MotionSelectControlParam(WT_AXIS_Y2, "", &m_stWTAxis_Y2, szNuControlParaID);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}

// 4.24TX1
BOOL CWaferTable::XY_SafeMoveTo(LONG lPosX, LONG lPosY, DOUBLE dScale, BOOL bUsePosOffset)
{
	BOOL bReturn = FALSE;
	if( IsWT2InUse() )
		bReturn = XY2_SafeMoveTo(lPosX, lPosY, dScale, bUsePosOffset);
	else
		bReturn = XY1_SafeMoveTo(lPosX, lPosY, dScale, bUsePosOffset);

	return bReturn;
}

BOOL CWaferTable::XY_SafeMoveTo(LONG lPosX, LONG lPosY, BOOL bUsePosOffset)
{
	return XY_SafeMoveTo(lPosX, lPosY, 1.0, bUsePosOffset);
}

INT CWaferTable::XY_MoveTo(INT nPosX, INT nPosY, INT nMode)
{
	if( IsWT2InUse() )
		return XY2_MoveTo(nPosX, nPosY, nMode);
	else
		return XY1_MoveTo(nPosX, nPosY, nMode);
}

INT CWaferTable::XY2_MoveTo(INT nPosX, INT nPosY, INT nMode)
{
	INT nResult			= gnOK;
	CString	szCurrentAxis = "";						

	if (m_fHardware && m_bSel_X2 && m_bSel_Y2)
	{
		//v4.24T10
		if ( (nPosX < m_lX2NegLimit) || (nPosX > m_lX2PosLimit) )
		{
			CString szErr;
			szErr.Format("XY2 MoveTo: X2 exceeds limit: %ld (%ld, %ld)", nPosX, m_lX2NegLimit, m_lX2PosLimit);
			SetErrorMessage(szErr);
			return Err_WTableXYMove;
		}
		if ( (nPosY < m_lY2NegLimit) || (nPosY > m_lY2PosLimit) )
		{
			CString szErr;
			szErr.Format("XY2 MoveTo: Y2 exceeds limit: %ld (%ld, %ld)", nPosY, m_lY2NegLimit, m_lY2PosLimit);
			HmiMessage_Red_Yellow(szErr);
			SetErrorMessage(szErr);
			return Err_WTableXYMove;
		}


		INT nResultX = X2_MoveTo(nPosX, SFM_NOWAIT);
		INT nResultY = Y2_MoveTo(nPosY, SFM_NOWAIT);

		if( nResultX!=gnOK && nResultY!=gnOK )
		{
			nResult = Err_WTableXYMove;
		}
		else
		{
			if( nResultX!=gnOK )
				nResult = Err_WTableXMove;
			else if( nResultY!=gnOK )
				nResult = Err_WTableYMove;
		}
		if( nResult==gnOK )
		{
			if (nMode == SFM_WAIT)
			{
				nResultX = X2_Sync();
				nResultY = Y2_Sync();
				if( nResultX!=gnOK && nResultY!=gnOK )
				{
					nResult = Err_WTableXYMove;
				}
				else
				{
					if( nResultX!=gnOK )
						nResult = Err_WTableXMove;
					else if( nResultY!=gnOK )
						nResult = Err_WTableYMove;
				}
			}
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
		SetMotionCE(TRUE, "WT XY2 Move Failure(XY2_MoveTo)");
	}

	return nResult;
}

BOOL CWaferTable::XY_IsPowerOff()
{
	if( IsESDualWT())
	{
		if (!X2_IsPowerOn() || !Y2_IsPowerOn())
		{
			return TRUE;
		}
	}

	if (!X_IsPowerOn() || !Y_IsPowerOn())
	{
		return TRUE;
	}

	return FALSE;
}

INT CWaferTable::X_MoveTo(INT nPos, INT nMode)
{
	if( IsWT2InUse() )
		return X2_MoveTo(nPos, nMode);
	else
		return X1_MoveTo(nPos, nMode);
}

INT CWaferTable::Y_MoveTo(INT nPos, INT nMode)
{
	if( IsWT2InUse() )
		return Y2_MoveTo(nPos, nMode);
	else
		return Y1_MoveTo(nPos, nMode);
}

INT CWaferTable::T_MoveTo(INT nPos, INT nMode)
{
	if( IsWT2InUse() )
		return T2_MoveTo(nPos, nMode);
	else
		return T1_MoveTo(nPos, nMode);
}

INT CWaferTable::X_Move(INT nPos, INT nMode)
{
	if( IsWT2InUse() )
		return X2_Move(nPos, nMode);
	else
		return X1_Move(nPos, nMode);
}

INT CWaferTable::Y_Move(INT nPos, INT nMode)
{
	if( IsWT2InUse() )
		return Y2_Move(nPos, nMode);
	else
		return Y1_Move(nPos, nMode);
}

INT CWaferTable::Y_ScfProfileMove(INT nPos, INT nMode)
{
	if( IsWT2InUse() )
		return Y2_ScfProfileMove(nPos, nMode);
	else
		return Y1_ScfProfileMove(nPos, nMode);
}

INT CWaferTable::T_Move(INT nPos, INT nMode)
{
	if( IsWT2InUse() )
		return T2_Move(nPos, nMode);
	else
		return T1_Move(nPos, nMode);
}

INT CWaferTable::X_Sync()
{
	if( IsWT2InUse() )
		return X2_Sync();
	else
		return X1_Sync();
}

INT CWaferTable::Y_Sync()
{
	if( IsWT2InUse() )
		return Y2_Sync();
	else
		return Y1_Sync();
}

INT CWaferTable::T_Sync()
{
	if( IsWT2InUse() )
		return T2_Sync();
	else
		return T1_Sync();
}


///////////////////////////////////////////////////////////////////
// EjtXY motion fcns for MS50; originally controlled in BH task
///////////////////////////////////////////////////////////////////

BOOL CWaferTable::EjX_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (!m_bMS100EjtXY)	
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	return FALSE;
}


BOOL CWaferTable::EjY_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (!m_bMS100EjtXY)	
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	return FALSE;
}

INT CWaferTable::EjX_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionMoveTo(BH_AXIS_EJ_X, nPos, SFM_NOWAIT, &m_stBHAxis_EjX);

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
			nResult = Err_EjectorMove;
		}
	}

	return nResult;
}

INT CWaferTable::EjY_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionMoveTo(BH_AXIS_EJ_Y, nPos, SFM_NOWAIT, &m_stBHAxis_EjY);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_Y,	&m_stBHAxis_EjY);

			//m_bHome_EjY	= FALSE;
			nResult = Err_EjectorMove;
		}
	}

		return nResult;
}

INT CWaferTable::EjX_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			CMS896AStn::MotionSync(BH_AXIS_EJ_X, 1000, &m_stBHAxis_EjX);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
			nResult = gnNOTOK;
		}	
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "EjX_Sync fail");	//v4.59A19
	}
	return nResult;
}

INT CWaferTable::EjY_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			CMS896AStn::MotionSync(BH_AXIS_EJ_Y, 1000, &m_stBHAxis_EjY);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
			nResult = gnNOTOK;
		}	
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "EjY_Sync fail");	//v4.59A19
	}
	return nResult;
}

BOOL CWaferTable::OpMoveEjectorTableXY(BOOL bStart, INT nMode)
{
	if (!IsMS60())
	{
		return TRUE;
	}
	//EjtXY move in WT only available for MS60 LF sequence;
	// original LF sequence still use the EjtXY Move in DBH task.
	if (!m_bWaferPrMS60LFSequence)
	{
		return TRUE;
	}
	if (!m_bEnableMS100EjtXY)
	{
		return TRUE;
	}

	if (!EjX_IsPowerOn())
	{
//		SetErrorMessage("ERROR: WT EJTX is not powered ON");
//		return FALSE;
		SetErrorMessage("ERROR: WT EJTX loses power 1, HOME");
		CBondHead *pBondHead = theApp.GetBondHead();
		if (pBondHead != NULL)
		{
			pBondHead->EjX_Home();
			Sleep(200);
		}
		if (!EjX_IsPowerOn())
		{
			SetErrorMessage("ERROR: WT EJTX loses power 2; fail");
			return FALSE;
		}
	}
	if (!EjY_IsPowerOn())
	{
//		SetErrorMessage("ERROR: WT EJTY is not powered ON");
//		return FALSE;
		SetErrorMessage("ERROR: WT EJTY loses power 1, HOME");
		CBondHead *pBondHead = theApp.GetBondHead();
		if (pBondHead != NULL)
		{
			pBondHead->EjY_Home();
			Sleep(200);
		}
		if (!EjY_IsPowerOn())
		{
			SetErrorMessage("ERROR: WT EJTY loses power 2; fail");
			return FALSE;
		}
	}


	LONG lCDiePos_EjX = (*m_psmfSRam)["BondHead"]["CDiePos_EjX"];
	LONG lCDiePos_EjY = (*m_psmfSRam)["BondHead"]["CDiePos_EjY"];
	LONG lEjtX = lCDiePos_EjX;
	LONG lEjtY = lCDiePos_EjY;

	//LONG lColletOffsetX = 0, lColletOffsetY = 0;
	CString szLog;
	CString szTemp;

	LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
	LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
	GetEjtColletOffsetCount(lEjtX, lEjtY, lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dEjtXYRes, TRUE, (m_nWTAtColletPos == 2) ? TRUE : FALSE, 4);

	if (bStart)
	{
		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_WAIT);
		EjX_Sync();
		
		if (!EjX_IsPowerOn() || !EjY_IsPowerOn())
		{
			return FALSE;
		}
		return TRUE;
	}

	if (m_nWTAtColletPos == 2)	
	{
		szLog.Format("WT - MoveEJTT to BHZ2: XY (%ld, %ld); C2Offset (%ld, %ld),LFCount,%d",
			lEjtX, lEjtY, lCollet2OffsetX, lCollet2OffsetY,m_nMS60CycleCount);
		lEjtX = lEjtX + lCollet2OffsetX;
		lEjtY = lEjtY + lCollet2OffsetY;
		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_NOWAIT);
		SaveBHMark2(szLog);
	}
	else if (m_nWTAtColletPos == 1)
	{
		szLog.Format("WT - MoveEJTT to BHZ1: XY (%ld, %ld); C1Offset (%ld, %ld),LFCount,%d",
			lEjtX, lEjtY, lCollet1OffsetX, lCollet1OffsetY,m_nMS60CycleCount);

		lEjtX = lEjtX + lCollet1OffsetX;
		lEjtY = lEjtY + lCollet1OffsetY;
		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_NOWAIT);
		SaveBHMark1(szLog);
	}
	else
	{
		szLog.Format("WT - MoveEJTT to CENTER: XY (%ld, %ld)",lEjtX, lEjtY);
		//lEjtX = lEjtX + lCollet1OffsetX;
		//lEjtY = lEjtY + lCollet1OffsetY;
		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_NOWAIT);
	}

	szTemp.Format("; FINAL (%ld, %ld)", lEjtX, lEjtY);
	szLog += szTemp;
	DisplaySequence(szLog);	
	CMSLogFileUtility::Instance()->MS60_Log(szLog);

	if (nMode == SFM_WAIT)
	{
		EjX_Sync();
		EjY_Sync();
	}

	return TRUE;
}