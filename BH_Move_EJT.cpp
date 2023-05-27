/////////////////////////////////////////////////////////////////
// BH_Move_EJT.cpp : Move functions of the CBondHead class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 28, 2013
//	Revision:	1.00
//
//	By:			Andrew Ng
//
//	Copyright @ ASM Assembly Automation Ltd., 2013.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "BondHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Ejector XY
////////////////////////////////////////////
INT CBondHead::EjX_Home()
{
	INT nResult		= gnOK;
	BOOL bComm_Ej	= FALSE;
	BOOL bHome_Ej	= FALSE;

	//v4.51A18	//MS90 Silan
	LONG lXHomeDir = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_HOME_DIRECTION);

	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)	
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		SetMotionCE(FALSE, "reset in EjX_Home");	//v4.59A19

		try
		{
			CMS896AStn::MotionPowerOff(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
			Sleep(50);
			CMS896AStn::MotionResetController(BH_AXIS_EJ_X, &m_stBHAxis_EjX);

			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJ_X, &m_stBHAxis_EjX) != 0)
			{
				bComm_Ej	= FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
			}

			if (!bComm_Ej)
			{
				bComm_Ej	= FALSE;
				nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_EJ_X, &m_stBHAxis_EjX);				

				if (nResult == gnOK)
				{
					bComm_Ej = TRUE;
				}
				else
				{
					nResult		= gnNOTOK;
					CMS896AStn::MotionClearError(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
				}
			}
		}
		catch (CAsmException e)
		{
			nResult = gnNOTOK;
			DisplayException(e);
		}

		try
		{
			if (nResult == gnOK)
			{
				CMS896AStn::MotionPowerOn(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
				Sleep(50);

				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_X, &m_stBHAxis_EjX))
				{
					//Search Home
					if (!IsMTRHomeActive(m_stBHAxis_EjX))
					{
						CMS896AStn::MotionSelectSearchProfile(BH_AXIS_EJ_X, BH_SP_E_HOME_X, &m_stBHAxis_EjX);
						CMS896AStn::MotionSearch(BH_AXIS_EJ_X, 1, SFM_WAIT, &m_stBHAxis_EjX, BH_SP_E_HOME_X, TRUE);
						CMS896AStn::MotionMove(BH_AXIS_EJ_X, 200, SFM_WAIT, &m_stBHAxis_EjX);
					}
					CMS896AStn::MotionSelectSearchProfile(BH_AXIS_EJ_X, BH_SP_E_HOME_X, &m_stBHAxis_EjX);
					CMS896AStn::MotionSearch(BH_AXIS_EJ_X, 1, SFM_WAIT, &m_stBHAxis_EjX, BH_SP_E_HOME_X);
				}
				if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJ_X, &m_stBHAxis_EjX) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_X, &m_stBHAxis_EjX))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BH_AXIS_EJ_X, 0, &m_stBHAxis_EjX);
					m_bHome_EjX	= TRUE;
					//Search Index sensor & Drive In
//					CMS896AStn::MotionSelectSearchProfile(BH_AXIS_EJ_X, BH_SP_E_INDEX_X, &m_stBHAxis_EjX);
//					CMS896AStn::MotionSearch(BH_AXIS_EJ_X, 1, SFM_WAIT, &m_stBHAxis_EjX, BH_SP_E_INDEX_X);
			
//AfxMessageBox("EJT X HOME done; start MOVE to -2000 ....", MB_SYSTEMMODAL);
					if (IsMS90())	//v4.51A18
					{
//						CMS896AStn::MotionMove(BH_AXIS_EJ_X, -2000/*2000*/, SFM_WAIT, &m_stBHAxis_EjX);
					}
					else
					{
						if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.53A20
							CMS896AStn::MotionMove(BH_AXIS_EJ_X, 100, SFM_WAIT, &m_stBHAxis_EjX);
						else
							CMS896AStn::MotionMove(BH_AXIS_EJ_X, -2000, SFM_WAIT, &m_stBHAxis_EjX);
					}
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_EjX	= FALSE;
					nResult		= gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_X,	&m_stBHAxis_EjX);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_X,	&m_stBHAxis_EjX);
			DisplayMessage("BH: EJ X Exception done.");
			bComm_Ej	= FALSE;
			m_bHome_EjX	= FALSE;
			nResult		= gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Ej && m_bMS100EjtXY)
		{
			bComm_Ej	= TRUE;
			m_bHome_EjX	= TRUE;
		}
	}

	return nResult;
}


INT CBondHead::EjY_Home()
{
	INT nResult		= gnOK;
	BOOL bComm_Ej	= FALSE;
	BOOL bHome_Ej	= FALSE;

	//v4.51A18	//MS90 Silan
	LONG lYHomeDir = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_Y, MS896A_CFG_CH_HOME_DIRECTION);

	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)	
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		SetMotionCE(FALSE, "reset in EjY_Home");	//v4.59A19

		try
		{
			CMS896AStn::MotionPowerOff(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
			Sleep(50);
			CMS896AStn::MotionResetController(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJ_Y, &m_stBHAxis_EjY) != 0)
			{
				bComm_Ej	= FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
			}

			if (!bComm_Ej)
			{
				bComm_Ej	= FALSE;
				nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);				

				if (nResult == gnOK)
				{
					bComm_Ej = TRUE;
				}
				else
				{
					nResult		= gnNOTOK;
					CMS896AStn::MotionClearError(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_Y,	&m_stBHAxis_EjY);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_Y,	&m_stBHAxis_EjY);
			nResult	= gnNOTOK;
		}


		try
		{
			if (nResult == gnOK)
			{
				CMS896AStn::MotionPowerOn(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
				Sleep(50);

				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_Y, &m_stBHAxis_EjY))
				{
					//Search Home
					if (!IsMTRHomeActive(m_stBHAxis_EjY))
					{
						CMS896AStn::MotionSelectSearchProfile(BH_AXIS_EJ_Y, BH_SP_E_HOME_Y, &m_stBHAxis_EjY);
						CMS896AStn::MotionSearch(BH_AXIS_EJ_Y, 1, SFM_WAIT, &m_stBHAxis_EjY, BH_SP_E_HOME_Y, TRUE);
						CMS896AStn::MotionMove(BH_AXIS_EJ_Y, 200, SFM_WAIT, &m_stBHAxis_EjY);
					}
					CMS896AStn::MotionSelectSearchProfile(BH_AXIS_EJ_Y, BH_SP_E_HOME_Y, &m_stBHAxis_EjY);
					CMS896AStn::MotionSearch(BH_AXIS_EJ_Y, 1, SFM_WAIT, &m_stBHAxis_EjY, BH_SP_E_HOME_Y);
				}

				if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJ_Y, &m_stBHAxis_EjY) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_Y, &m_stBHAxis_EjY))
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BH_AXIS_EJ_Y, 0, &m_stBHAxis_EjY);
					m_bHome_EjY	= TRUE;

					//Search Index hole
//					CMS896AStn::MotionSelectSearchProfile(BH_AXIS_EJ_Y, BH_SP_E_INDEX_Y, &m_stBHAxis_EjY);
//					CMS896AStn::MotionSearch(BH_AXIS_EJ_Y, 1,  SFM_WAIT, &m_stBHAxis_EjY, BH_SP_E_INDEX_Y);

//AfxMessageBox("EJT Y HOME done; start MOVE to 2000 ....", MB_SYSTEMMODAL);
					if (IsMS90())	//v4.51A18
					{
//						CMS896AStn::MotionMove(BH_AXIS_EJ_Y, 2000/*-2000*/, SFM_WAIT, &m_stBHAxis_EjY);
					}
					else
					{
						if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.53A20
							CMS896AStn::MotionMove(BH_AXIS_EJ_Y, 100, SFM_WAIT, &m_stBHAxis_EjY);
						else
							CMS896AStn::MotionMove(BH_AXIS_EJ_Y, 2000, SFM_WAIT, &m_stBHAxis_EjY);
					}
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_EjY	= FALSE;
					nResult		= gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_Y,	&m_stBHAxis_EjY);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_Y,	&m_stBHAxis_EjY);
			DisplayMessage("BH: EJ Y Exception done.");
			bComm_Ej	= FALSE;
			m_bHome_EjY	= FALSE;
			nResult		= gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Ej && m_bMS100EjtXY)
		{
			bComm_Ej	= TRUE;
			m_bHome_EjY	= TRUE;
		}
	}

	return nResult;
}

INT CBondHead::EjX_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_X, &m_stBHAxis_EjX) == FALSE )
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
					m_bIsPowerOn_EjX = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_X, &m_stBHAxis_EjX) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
					m_bIsPowerOn_EjX = FALSE;
					m_bHome_EjX = FALSE;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CBondHead::EjY_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_Y, &m_stBHAxis_EjY) == FALSE )
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
					m_bIsPowerOn_EjY = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_Y, &m_stBHAxis_EjY) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
					m_bIsPowerOn_EjY = FALSE;
					m_bHome_EjY = FALSE;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CBondHead::EjX_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_EjX)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMoveTo(BH_AXIS_EJ_X, nPos, SFM_NOWAIT, &m_stBHAxis_EjX);
				//m_lCurLevel_Ej	= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_EJ_X, 5000, &m_stBHAxis_EjX)) != gnOK)
					{
						nResult = Err_EjectorMove;
					}
					m_bComplete_EjX = TRUE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_X, &m_stBHAxis_EjX);

			m_bHome_EjX	= FALSE;
			nResult = Err_EjectorMove;
		}
	}
	else
	{
	}

	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "EjX_MoveTo fail");	//v4.59A19
	}

	return nResult;
}

INT CBondHead::EjY_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_EjY)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMoveTo(BH_AXIS_EJ_Y, nPos, SFM_NOWAIT, &m_stBHAxis_EjY);
				//m_lCurLevel_Ej	= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_EJ_Y, 5000, &m_stBHAxis_EjY)) != gnOK)
					{
						nResult = Err_EjectorMove;
					}
					m_bComplete_EjY = TRUE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_Y,	&m_stBHAxis_EjY);

			m_bHome_EjY	= FALSE;
			nResult = Err_EjectorMove;
		}
	}
	else
	{
	}

	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "EjY_MoveTo fail");	//v4.59A19
	}
	return nResult;
}

INT CBondHead::EjX_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_EjX || !m_bIsPowerOn_EjX)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMove(BH_AXIS_EJ_X, nPos, SFM_NOWAIT, &m_stBHAxis_EjX);
				//m_lCurLevel_Ej	+= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_EJ_X, 5000, &m_stBHAxis_EjX)) != gnOK)
					{
						nResult = Err_EjectorMove;
					}
					m_bComplete_EjX = TRUE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_X, &m_stBHAxis_EjX);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_X,	&m_stBHAxis_EjX);

			m_bHome_EjX	= FALSE;
			nResult		= Err_EjectorMove;
		}
	}
	else
	{
	}

	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "EjX_Move fail");	//v4.59A19
	}
	return nResult;
}

INT CBondHead::EjY_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_EjY || !m_bIsPowerOn_EjY)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMove(BH_AXIS_EJ_Y, nPos, SFM_NOWAIT, &m_stBHAxis_EjY);
				//m_lCurLevel_Ej	+= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_EJ_Y, 5000, &m_stBHAxis_EjY)) != gnOK)
					{
						nResult = Err_EjectorMove;
					}
					m_bComplete_EjY = TRUE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_Y, &m_stBHAxis_EjY);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_Y,	&m_stBHAxis_EjY);

			m_bHome_EjY	= FALSE;
			nResult		= Err_EjectorMove;
		}
	}
	else
	{
	}

	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "EjY_Move fail");	//v4.59A19
	}
	return nResult;
}

INT CBondHead::EjX_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			CMS896AStn::MotionSync(BH_AXIS_EJ_X, 1000, &m_stBHAxis_EjX);
			m_bComplete_EjX = TRUE;
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
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "EjX_Sync fail");	//v4.59A19
	}
	return nResult;
}

INT CBondHead::EjY_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Ej && m_bMS100EjtXY)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			CMS896AStn::MotionSync(BH_AXIS_EJ_Y, 1000, &m_stBHAxis_EjY);
			m_bComplete_EjY = TRUE;
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
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "EjY_Sync fail");	//v4.59A19
	}
	return nResult;
}

BOOL CBondHead::EjX_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (!m_bSel_Ej)	
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

BOOL CBondHead::EjY_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (!m_bSel_Ej)		//v4.24
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


