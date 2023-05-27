/////////////////////////////////////////////////////////////////
// BondHead.cpp : Move functions of the CBondHead class
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
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "BinLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Ejector 
////////////////////////////////////////////

INT CBondHead::Ej_Home()
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Ej && !m_bDisableBH)		//v4.08		//v4.24	//ES101
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		SetMotionCE(FALSE, "reset in Ej_Home");	//v4.59A19
		
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJ, &m_stBHAxis_Ej) != 0)
			{
				m_bComm_Ej	= FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_EJ, &m_stBHAxis_Ej);
			}

			if (!m_bComm_Ej)
			{
				m_bHome_Ej	= FALSE;

				if (m_lE_CommMethod == MS896A_NO_SWCOMM)
				{
					nResult = gnOK;
				}
				else
				{
					nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_EJ, &m_stBHAxis_Ej);				
				}

				if (nResult == gnOK)
				{
					m_bComm_Ej = TRUE;
				}
				else
				{
					nResult		= Err_EjectorCommutate;
					CMS896AStn::MotionClearError(BH_AXIS_EJ, &m_stBHAxis_Ej);
				}
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(BH_AXIS_EJ, 1, 0, &m_stBHAxis_Ej);

				//if (nResult == gnAMS_OK)
				if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJ, &m_stBHAxis_Ej) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ, &m_stBHAxis_Ej))
				{
					Sleep(50);

					//v4.59A15	//Renesas MS90
					CMS896AApp* pApp	= (CMS896AApp*) AfxGetApp();
					LONG lEjHomeDir		= pApp->GetProfileInt(gszPROFILE_SETTING, _T("EJ HOME Dir"), 0);
					LONG lEjOffset		= pApp->GetProfileInt(gszPROFILE_SETTING, _T("EJ HOME Offset"), 0);

					if ( (lEjOffset != 0) && (labs(lEjOffset) <= 500))
					{
						if (lEjHomeDir == 0)
						{
							lEjOffset = -1 * lEjOffset;
						}

						CString szLog;
						szLog.Format("BH: Ej HOME use Offset = %ld, Dir = %ld", lEjOffset, lEjHomeDir);
						CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

						Sleep(100);
						if (m_lE_ProfileType == MS896A_OBW_PROFILE && (!CMS896AStn::MotionIsStepper(BH_AXIS_EJ, &m_stBHAxis_Ej)))	//Linear Ej
						{
							CMS896AStn::MotionObwMove(BH_AXIS_EJ, lEjOffset, SFM_WAIT, &m_stBHAxis_Ej);
						}
						else
						{
							CMS896AStn::MotionMove(BH_AXIS_EJ, lEjOffset, SFM_WAIT, &m_stBHAxis_Ej);
						}

						Sleep(500);
					}

					CMS896AStn::MotionSetPosition(BH_AXIS_EJ, 0, &m_stBHAxis_Ej);
					m_bHome_Ej		= TRUE;
					m_bIsPowerOn_Ej = TRUE;
					m_lCurLevel_Ej	= 0;
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_Ej		= FALSE;
					m_bIsPowerOn_Ej = TRUE;
					nResult = Err_EjectorMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej);
			CMS896AStn::MotionClearError(BH_AXIS_EJ, &m_stBHAxis_Ej);
			DisplayMessage("BH: EJ Exception done.");		//v3.92

			m_bComm_Ej	= FALSE;
			m_bHome_Ej	= FALSE;	//v3.92
			nResult = Err_EjectorMoveHome;
		}
	}
	else
	{
		if (m_bSel_Ej)
		{
			m_bComm_Ej	= TRUE;
			m_bHome_Ej	= TRUE;
		}
	}

	//CheckResult(nResult, _T("Ejector Axis - Home"));
	return nResult;
}


INT CBondHead::BinEj_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_BinEj && !m_bDisableBH && m_bEnableBinEjector)		//v4.08		//v4.24	//ES101
	{
		SetMotionCE(FALSE, "reset in BinEj_Home");	//v4.59A19
		
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj) != 0)
			{
				m_bComm_BinEj	= FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			}

			if (!m_bComm_BinEj)
			{
				m_bHome_BinEj	= FALSE;

				if (m_lBinE_CommMethod == MS896A_NO_SWCOMM)
				{
					nResult = gnOK;
				}
				else
				{
					nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);				
				}

				if (nResult == gnOK)
				{
					m_bComm_BinEj = TRUE;
				}
				else
				{
					nResult		= Err_EjectorCommutate;
					CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
				}
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(BH_AXIS_BIN_EJ, 1, 0, &m_stBHAxis_BinEj);

				if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj) == 0) && 
					 CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj))
				{
					Sleep(50);

					CMS896AApp* pApp	= (CMS896AApp*) AfxGetApp();
					LONG lEjHomeDir		= pApp->GetProfileInt(gszPROFILE_SETTING, _T("EJ HOME Dir"), 0);
					LONG lEjOffset		= pApp->GetProfileInt(gszPROFILE_SETTING, _T("EJ HOME Offset"), 0);

					if ( (lEjOffset != 0) && (labs(lEjOffset) <= 500))
					{
						if (lEjHomeDir == 0)
						{
							lEjOffset = -1 * lEjOffset;
						}

						CString szLog;
						szLog.Format("BH: Bin Ej HOME use Offset = %ld, Dir = %ld", lEjOffset, lEjHomeDir);
						CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

						Sleep(100);
						if ( m_lBinE_ProfileType == MS896A_OBW_PROFILE && (!CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj)))	//Linear Ej
						{
							CMS896AStn::MotionObwMove(BH_AXIS_BIN_EJ, lEjOffset, SFM_WAIT, &m_stBHAxis_BinEj);
						}
						else
						{
							CMS896AStn::MotionMove(BH_AXIS_BIN_EJ, lEjOffset, SFM_WAIT, &m_stBHAxis_BinEj);
						}

						Sleep(500);
					}

					CMS896AStn::MotionSetPosition(BH_AXIS_BIN_EJ, 0, &m_stBHAxis_BinEj);
					
					m_bHome_BinEj		= TRUE;
					m_bIsPowerOn_BinEj	= TRUE;
					m_lCurLevel_BinEj	= 0;
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_BinEj		= FALSE;
					m_bIsPowerOn_BinEj	= TRUE;
					nResult				= Err_EjectorMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			DisplayMessage("BH: BIN EJ Exception done.");		//v3.92

			m_bComm_BinEj	= FALSE;
			m_bHome_BinEj	= FALSE;	//v3.92
			nResult			= Err_EjectorMoveHome;
		}
	}
	else
	{
		if (m_bSel_BinEj)
		{
			m_bComm_BinEj	= TRUE;
			m_bHome_BinEj	= TRUE;
		}
	}

	return nResult;
}


INT CBondHead::EjT_Home()
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Ej && !m_bDisableBH && m_bEnableEjectorTheta)
	{
		SetMotionCE(FALSE, "reset in EjT_Home");
		
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJ_T, &m_stBHAxis_EjT) != 0)
			{
				m_bComm_EjT	= FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
			}

			if (!m_bComm_EjT)
			{
				m_bHome_EjT	= FALSE;

				if (CMS896AStn::MotionIsStepper(BH_AXIS_EJ_T, &m_stBHAxis_EjT))
				{
					m_bComm_EjT = TRUE;
				}
				else
				{
					nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_EJ_T, &m_stBHAxis_EjT);

					if (nResult == gnOK)
					{
						m_bComm_EjT = TRUE;
					}
					else
					{
						nResult	= Err_EjectorTCommutate;
						CMS896AStn::MotionClearError(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
					}
				}	
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(BH_AXIS_EJ_T, 1, 0, &m_stBHAxis_EjT);

				if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJ_T, &m_stBHAxis_EjT) == 0 && 
					CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_T, &m_stBHAxis_EjT))
				{
					Sleep(50);

					CMS896AStn::MotionSetPosition(BH_AXIS_EJ_T, 0, &m_stBHAxis_EjT);
					
					m_bHome_EjT			= TRUE;
					m_bIsPowerOn_EjT	= TRUE;
					m_lCurLevel_EjT		= 0;
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_EjT			= FALSE;
					m_bIsPowerOn_EjT	= TRUE;
					nResult				= Err_EjectorTMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
			DisplayMessage("BH: EJ T Exception done.");

			m_bComm_EjT	= FALSE;
			m_bHome_EjT	= FALSE;
			nResult		= Err_EjectorTMoveHome;
		}
	}
	else
	{
		if (m_bSel_Ej)
		{
			m_bComm_EjT	= TRUE;
			m_bHome_EjT	= TRUE;
		}
	}

	return nResult;
}


INT CBondHead::BinEjT_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_BinEj && !m_bDisableBH && m_bEnableBinEjector)
	{
		SetMotionCE(FALSE, "reset in BinEjT_Home");
		
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT) != 0)
			{
				m_bComm_BinEjT	= FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
			}

			if (!m_bComm_BinEjT)
			{
				m_bHome_BinEjT	= FALSE;

				if (CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT))
				{
					m_bComm_BinEjT = TRUE;
				}
				else
				{
					nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);

					if (nResult == gnOK)
					{
						m_bComm_BinEjT = TRUE;
					}
					else
					{
						nResult		= Err_EjectorTCommutate;
						CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
					}
				}	
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(BH_AXIS_BIN_EJ_T, 1, 0, &m_stBHAxis_BinEjT);

				if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT) == 0 && 
					CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT))
				{
					Sleep(50);

					CMS896AStn::MotionSetPosition(BH_AXIS_BIN_EJ_T, 0, &m_stBHAxis_BinEjT);
					
					m_bHome_BinEjT		= TRUE;
					m_bIsPowerOn_BinEjT	= TRUE;
					m_lCurLevel_BinEjT	= 0;
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_BinEjT		= FALSE;
					m_bIsPowerOn_BinEjT	= TRUE;
					nResult				= Err_EjectorTMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
			DisplayMessage("BH: Bin EJ T Exception done.");

			m_bComm_BinEjT	= FALSE;
			m_bHome_BinEjT	= FALSE;
			nResult			= Err_EjectorTMoveHome;
		}
	}
	else
	{
		if (m_bSel_BinEj)
		{
			m_bComm_BinEjT	= TRUE;
			m_bHome_BinEjT	= TRUE;
		}
	}

	return nResult;
}


INT CBondHead::EjCap_Home()
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Ej)
	{
		SetMotionCE(FALSE, "reset in EjCap_Home");
		
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJCAP, &m_stBHAxis_EjCap) != 0)
			{
				m_bComm_EjCap = FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
			}

			if (!m_bComm_EjCap)
			{
				m_bHome_EjCap = FALSE;

				if (CMS896AStn::MotionIsStepper(BH_AXIS_EJCAP, &m_stBHAxis_EjCap))
				{
					m_bComm_EjCap = TRUE;
				}
				else
				{
					nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);

					if (nResult == gnOK)
					{
						m_bComm_EjCap = TRUE;
					}
					else
					{
						nResult	= Err_EjectorCapCommutate;
						CMS896AStn::MotionClearError(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
					}
				}
			}

			if (nResult == gnOK)
			{
				//nResult = CMS896AStn::MotionSearch(BH_AXIS_EJCAP, 0, SFM_WAIT, &m_stBHAxis_EjCap, BH_SP_HOME_EJECTOR_CAP);
				nResult = CMS896AStn::MotionMoveHome(BH_AXIS_EJCAP, 1, 0, &m_stBHAxis_EjCap);

				if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJCAP, &m_stBHAxis_EjCap) == 0 && 
					 CMS896AStn::MotionIsPowerOn(BH_AXIS_EJCAP, &m_stBHAxis_EjCap))
				{
					Sleep(50);

					CMS896AStn::MotionSetPosition(BH_AXIS_EJCAP, 0, &m_stBHAxis_EjCap);
					
					m_bHome_EjCap		= TRUE;
					m_bIsPowerOn_EjCap	= TRUE;
					m_lCurLevel_EjCap	= 0;
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_EjCap		= FALSE;
					m_bIsPowerOn_EjCap	= FALSE;
					nResult = Err_EjectorCapMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
			CMS896AStn::MotionClearError(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
			DisplayMessage("BH: EJCAP Exception done.");

			m_bComm_EjCap	= FALSE;
			m_bHome_EjCap	= FALSE;
			nResult = Err_EjectorCapMoveHome;
		}
	}

	return nResult;
}


INT CBondHead::BinEjCap_Home()
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_BinEj && m_bEnableBinEjector && !m_bDisableBH)
	{
		SetMotionCE(FALSE, "reset in BinEjCap_Home");
		
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap) != 0)
			{
				m_bComm_BinEjCap = FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
			}

			if (!m_bComm_BinEjCap)
			{
				m_bHome_BinEjCap = FALSE;

				if (CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap))
				{
					m_bComm_BinEjCap = TRUE;
				}
				else
				{
					nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);

					if (nResult == gnOK)
					{
						m_bComm_BinEjCap = TRUE;
					}
					else
					{
						nResult	= Err_EjectorCapCommutate;
						CMS896AStn::MotionClearError(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
					}
				}
			}

			if (nResult == gnOK)
			{
				//nResult = CMS896AStn::MotionSearch(BH_AXIS_BIN_EJCAP, 0, SFM_WAIT, &m_stBHAxis_BinEjCap, BH_SP_HOME_BIN_EJECTOR_CAP);
				nResult = CMS896AStn::MotionMoveHome(BH_AXIS_BIN_EJCAP, 1, 0, &m_stBHAxis_BinEjCap);

				if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap) == 0 && 
					CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap))
				{
					Sleep(50);

					CMS896AStn::MotionSetPosition(BH_AXIS_BIN_EJCAP, 0, &m_stBHAxis_BinEjCap);
					
					m_bHome_BinEjCap		= TRUE;
					m_bIsPowerOn_BinEjCap	= TRUE;
					m_lCurLevel_BinEjCap	= 0;
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_BinEjCap		= FALSE;
					m_bIsPowerOn_BinEjCap	= FALSE;
					nResult = Err_EjectorCapMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
			DisplayMessage("BH: BINEJCAP Exception done.");

			m_bComm_BinEjCap	= FALSE;
			m_bHome_BinEjCap	= FALSE;
			nResult = Err_EjectorCapMoveHome;
		}
	}

	return nResult;
}


INT CBondHead::EjElevator_Home()
{
	INT nResult	= gnOK;

	if (!m_bES101Configuration && !m_bMS60EjElevator)	//v4.51A5
		return gnNOTOK;

	if (m_fHardware && m_bSel_Ej)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator) != 0)
			{
				CMS896AStn::MotionClearError(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
			}

			if (m_bMS60EjElevator)		//v4.51A5
			{
//AfxMessageBox("Init Ej Elevator COMM...", MB_SYSTEMMODAL);
				nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);	
			}


			if (nResult == gnOK)
			{
				if (m_bMS60EjElevator)		//v4.51A5
				{
//AfxMessageBox("BH: EJ Elevator Z Home start ...", MB_SYSTEMMODAL);

					nResult = CMS896AStn::MotionMoveHome(BH_AXIS_EJELEVATOR, 1, 0, &m_stBHAxis_EjElevator);

					if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator) == 0 && 
						CMS896AStn::MotionIsPowerOn(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator))
					{
						Sleep(50);

						CMS896AStn::MotionSetPosition(BH_AXIS_EJELEVATOR, 0, &m_stBHAxis_EjElevator);
						m_bHome_EjElevator		= TRUE;
						m_bIsPowerOn_EjElevator	= TRUE;
					}
					else if (nResult == gnNOTOK)
					{
						m_bHome_EjElevator		= FALSE;
						m_bIsPowerOn_EjElevator	= TRUE;
					}

					//nResult = CMS896AStn::MotionMoveHome(BH_AXIS_EJELEVATOR, 1, 1, &m_stBHAxis_EjElevator);

					////if (nResult == gnAMS_OK)
					//if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator) == 0) && 
					//	  CMS896AStn::MotionIsPowerOn(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator) )
					//{
					//	CMS896AStn::MotionSelectSearchProfile(BH_AXIS_EJELEVATOR, BH_SP_HOME_EJELZ, &m_stBHAxis_EjElevator);
					//	CMS896AStn::MotionSearch(BH_AXIS_EJELEVATOR, 1, SFM_WAIT, &m_stBHAxis_EjElevator, BH_SP_HOME_EJELZ);

					//	if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator) == 0) && 
					//		  CMS896AStn::MotionIsPowerOn(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator) )
					//	{
					//		Sleep(50);
					//		CMS896AStn::MotionSetPosition(BH_AXIS_EJELEVATOR, 0, &m_stBHAxis_EjElevator);
					//		m_bHome_EjElevator		= TRUE;
					//		m_bIsPowerOn_EjElevator	= TRUE;
					//	}
					//}
				}
				/*else
				{
					if ((nResult = CMS896AStn::MotionMoveHome(BH_AXIS_EJELEVATOR, 1, 0, &m_stBHAxis_EjElevator)) == gnAMS_OK)
					{
						Sleep(200);

						CMS896AStn::MotionSelectSearchProfile(BH_AXIS_EJELEVATOR, BH_SP_INDEXER_EJELZ, &m_stBHAxis_EjElevator);
						CMS896AStn::MotionSearch(BH_AXIS_EJELEVATOR, 1, SFM_WAIT, &m_stBHAxis_EjElevator, BH_SP_INDEXER_EJELZ);

						if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator) == 0) && 
								CMS896AStn::MotionIsPowerOn(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator))
						{
						
							CMS896AStn::MotionSetPosition(BH_AXIS_EJELEVATOR, 0, &m_stBHAxis_EjElevator);
							m_bHome_EjElevator		= TRUE;
							m_bIsPowerOn_EjElevator	= TRUE;
						}
					}
				}*/
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
			CMS896AStn::MotionClearError(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
			m_bHome_EjElevator		= FALSE;
			m_bIsPowerOn_EjElevator	= FALSE;
			nResult = gnNOTOK; 
		}

	}
	else if (m_bSel_Ej)
	{
		m_bHome_EjElevator		= TRUE;
		m_bIsPowerOn_EjElevator	= TRUE;
	}

	return nResult;
}


INT CBondHead::BinEjElevator_Home()
{
	INT nResult	= gnOK;

	if (!m_bMS60EjElevator)	//v4.51A5
		return gnNOTOK;

	if (m_fHardware && m_bSel_BinEj && m_bEnableBinEjector && !m_bDisableBH)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator) != 0)
			{
				CMS896AStn::MotionClearError(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
			}

			if (m_bMS60EjElevator)		//v4.51A5
			{
				nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);	
			}


			if (nResult == gnOK)
			{
				if (m_bMS60EjElevator)		//v4.51A5
				{
					nResult = CMS896AStn::MotionMoveHome(BH_AXIS_BIN_EJELEVATOR, 1, 0, &m_stBHAxis_BinEjElevator);

					if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator) == 0 && 
						CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator))
					{
						Sleep(50);

						CMS896AStn::MotionSetPosition(BH_AXIS_BIN_EJELEVATOR, 0, &m_stBHAxis_BinEjElevator);
						m_bHome_BinEjElevator		= TRUE;
						m_bIsPowerOn_BinEjElevator	= TRUE;
					}
					else if (nResult == gnNOTOK)
					{
						m_bHome_BinEjElevator		= FALSE;
						m_bIsPowerOn_BinEjElevator	= TRUE;
					}


					//nResult = CMS896AStn::MotionMoveHome(BH_AXIS_BIN_EJELEVATOR, 1, 1, &m_stBHAxis_BinEjElevator);

					////if (nResult == gnAMS_OK)
					//if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator) == 0 && 
					//	CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator))
					//{
					//	CMS896AStn::MotionSelectSearchProfile(BH_AXIS_BIN_EJELEVATOR, BH_SP_HOME_BIN_EJELZ, &m_stBHAxis_BinEjElevator);
					//	CMS896AStn::MotionSearch(BH_AXIS_BIN_EJELEVATOR, 1, SFM_WAIT, &m_stBHAxis_BinEjElevator, BH_SP_HOME_BIN_EJELZ);

					//	if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator) == 0) && 
					//		  CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator) )
					//	{
					//		Sleep(50);
					//		CMS896AStn::MotionSetPosition(BH_AXIS_BIN_EJELEVATOR, 0, &m_stBHAxis_BinEjElevator);
					//		m_bHome_BinEjElevator		= TRUE;
					//		m_bIsPowerOn_BinEjElevator	= TRUE;
					//	}
					//}
				}
				else
				{
					if ((nResult = CMS896AStn::MotionMoveHome(BH_AXIS_BIN_EJELEVATOR, 1, 0, &m_stBHAxis_BinEjElevator)) == gnAMS_OK)
					{
						Sleep(200);

						CMS896AStn::MotionSelectSearchProfile(BH_AXIS_BIN_EJELEVATOR, BH_SP_INDEXER_EJELZ, &m_stBHAxis_BinEjElevator);
						CMS896AStn::MotionSearch(BH_AXIS_BIN_EJELEVATOR, 1, SFM_WAIT, &m_stBHAxis_BinEjElevator, BH_SP_INDEXER_EJELZ);

						if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator) == 0 && 
							CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator))
						{
						
							CMS896AStn::MotionSetPosition(BH_AXIS_BIN_EJELEVATOR, 0, &m_stBHAxis_BinEjElevator);
							m_bHome_BinEjElevator		= TRUE;
							m_bIsPowerOn_BinEjElevator	= TRUE;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
			m_bHome_BinEjElevator		= FALSE;
			m_bIsPowerOn_BinEjElevator	= FALSE;
			nResult = gnNOTOK; 
		}

	}
	else if (m_bSel_BinEj)
	{
		m_bHome_BinEjElevator		= TRUE;
		m_bIsPowerOn_BinEjElevator	= TRUE;
	}

	return nResult;
}

INT CBondHead::Ej_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	//if( SPECIAL_DEBUG_LOG_BH )
	//{
	//	CString szMsg;
	//	szMsg.Format("BH - Ej move to %d, mode %d", nPos, nMode);
	//	DisplaySequence(szMsg);
	//}

	if (m_fHardware && m_bSel_Ej && !m_bDisableBH)		//v4.24
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_Ej)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( m_lE_ProfileType == MS896A_OBW_PROFILE && (!CMS896AStn::MotionIsStepper(BH_AXIS_EJ, &m_stBHAxis_Ej)) )	//Linear Ej
				{
					Ej_SelectObwProfile((LONG)(nPos - m_lCurLevel_Ej));
					Ej_SelectControl((LONG)(nPos - m_lCurLevel_Ej));
					CMS896AStn::MotionObwMoveTo(BH_AXIS_EJ, nPos, SFM_NOWAIT, &m_stBHAxis_Ej);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BH_AXIS_EJ, nPos, SFM_NOWAIT, &m_stBHAxis_Ej);
				}

				m_lCurLevel_Ej	= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_EJ, 5000, &m_stBHAxis_Ej)) != gnOK)
					{
						nResult = Err_EjectorMove;
					}
					m_bComplete_Ej = TRUE;		//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej);
			CMS896AStn::MotionClearError(BH_AXIS_EJ, &m_stBHAxis_Ej);

			m_bHome_Ej	= FALSE;
			nResult = Err_EjectorMove;
		}
	}
	else
	{
		if (m_bSel_Ej)
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

	//CheckResult(nResult, _T("Ejector Axis - MoveTo"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Ej_MoveTo fail");	//v4.59A19
	}

	return nResult;
}

INT CBondHead::BinEj_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_BinEj && !m_bDisableBH && m_bEnableBinEjector)		//v4.24
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_BinEj)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( m_lBinE_ProfileType == MS896A_OBW_PROFILE && (!CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj)) )	//Linear Ej
				{
					BinEj_SelectObwProfile((LONG)(nPos - m_lCurLevel_BinEj));
					BinEj_SelectControl((LONG)(nPos - m_lCurLevel_Ej));
					CMS896AStn::MotionObwMoveTo(BH_AXIS_BIN_EJ, nPos, SFM_NOWAIT, &m_stBHAxis_BinEj);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BH_AXIS_BIN_EJ, nPos, SFM_NOWAIT, &m_stBHAxis_BinEj);
				}

				m_lCurLevel_BinEj	= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_BIN_EJ, 5000, &m_stBHAxis_BinEj)) != gnOK)
					{
						nResult = Err_EjectorMove;
					}
					m_bComplete_BinEj = TRUE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);

			m_bHome_BinEj	= FALSE;
			nResult = Err_EjectorMove;
		}
	}
	else
	{
		if (m_bSel_BinEj)
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
		SetMotionCE(TRUE, "Ej_MoveTo fail");
	}

	return nResult;
}


INT CBondHead::EjCap_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware)
	{
		if (IsMotionCE() == TRUE)
		{
			nResult = Err_EjectorCapMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_EjCap)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( m_lECap_ProfileType == MS896A_OBW_PROFILE && (!CMS896AStn::MotionIsStepper(BH_AXIS_EJCAP, &m_stBHAxis_EjCap)) )	//Linear Ej
				{
					EjCap_SelectObwProfile(nPos - m_lCurLevel_EjCap);

					CString szNuControlParaID = m_stBHAxis_EjCap.m_stControl[PL_DYNAMIC].m_szID;
					CMS896AStn::MotionSelectControlParam(BH_AXIS_EJCAP, EJCAP_DYNAMIC_CONTROL_OBW , &m_stBHAxis_EjCap, szNuControlParaID);
				
					CMS896AStn::MotionObwMoveTo(BH_AXIS_EJCAP, nPos, SFM_NOWAIT, &m_stBHAxis_EjCap);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BH_AXIS_EJCAP, nPos, SFM_NOWAIT, &m_stBHAxis_EjCap);
				}
				
				m_lCurLevel_EjCap = nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_EJCAP, 5000, &m_stBHAxis_EjCap)) != gnOK)
					{
						nResult = Err_EjectorCapMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
			CMS896AStn::MotionClearError(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);

			m_bHome_EjCap	= FALSE;
			nResult			= Err_EjectorCapMove;
		}
	}
	else
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

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "EjCap_MoveTo fail");
	}

	return nResult;
}


INT CBondHead::BinEjCap_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_BinEj && m_bEnableBinEjector && !m_bDisableBH)
	{
		if (IsMotionCE() == TRUE)
		{
			nResult = Err_EjectorCapMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_BinEjCap)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( m_lECap_ProfileType == MS896A_OBW_PROFILE && (!CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap)) )
				{
					BinEjCap_SelectObwProfile(nPos - m_lCurLevel_BinEjCap);

					CString szNuControlParaID = m_stBHAxis_BinEjCap.m_stControl[PL_DYNAMIC].m_szID;
					CMS896AStn::MotionSelectControlParam(BH_AXIS_BIN_EJCAP, EJCAP_DYNAMIC_CONTROL_OBW , &m_stBHAxis_BinEjCap, szNuControlParaID);
				
					CMS896AStn::MotionObwMoveTo(BH_AXIS_BIN_EJCAP, nPos, SFM_NOWAIT, &m_stBHAxis_BinEjCap);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BH_AXIS_BIN_EJCAP, nPos, SFM_NOWAIT, &m_stBHAxis_BinEjCap);
				}

				m_lCurLevel_BinEjCap = nPos;
				
				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_BIN_EJCAP, 5000, &m_stBHAxis_BinEjCap)) != gnOK)
					{
						nResult = Err_EjectorCapMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);

			m_bHome_BinEjCap	= FALSE;
			nResult				= Err_EjectorCapMove;
		}
	}
	else
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

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "BinEjCap_MoveTo fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::EjT_MoveTo(INT nPos, INT nMode)
{
	INT nResult = gnOK;

	if (m_fHardware && m_bEnableEjectorTheta && !m_bDisableBH)
	{
		if (IsMotionCE() == TRUE)
		{
			nResult = Err_EjectorTMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_EjT)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_lET_ProfileType == MS896A_OBW_PROFILE && !CMS896AStn::MotionIsStepper(BH_AXIS_EJ_T, &m_stBHAxis_EjT))	//Linear Ej
				{
					EjT_SelectObwProfile(nPos - m_lCurLevel_EjT);
					EjT_SelectControl((LONG)(nPos - m_lCurLevel_EjT));
					CMS896AStn::MotionObwMoveTo(BH_AXIS_EJ_T, nPos, SFM_NOWAIT, &m_stBHAxis_EjT);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BH_AXIS_EJ_T, nPos, SFM_NOWAIT, &m_stBHAxis_EjT);
				}
				
				m_lCurLevel_EjT = nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_EJ_T, 5000, &m_stBHAxis_EjT)) != gnOK)
					{
						nResult = Err_EjectorTMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
			CMS896AStn::MotionClearError(BH_AXIS_EJ_T, &m_stBHAxis_EjT);

			m_bComm_EjT = FALSE;
			m_bHome_EjT	= FALSE;
			nResult		= Err_EjectorTMove;
		}
	}
	else
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

	//if (nResult != gnOK)
	//{
	//	SetMotionCE(TRUE, "EjT_MoveTo fail");
	//}
	return nResult;
}


INT CBondHead::BinEjT_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bEnableBinEjector && m_bSel_BinEj && !m_bDisableBH)
	{
		if (IsMotionCE() == TRUE)
		{
			nResult = Err_EjectorTMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_BinEjT)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_lBinET_ProfileType == MS896A_OBW_PROFILE && !CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT))	//Linear Ej
				{
					BinEjT_SelectObwProfile(nPos - m_lCurLevel_BinEjT);
					BinEjT_SelectControl((LONG)(nPos - m_lCurLevel_BinEjT));	
					CMS896AStn::MotionObwMoveTo(BH_AXIS_BIN_EJ_T, nPos, SFM_NOWAIT, &m_stBHAxis_BinEjT);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BH_AXIS_BIN_EJ_T, nPos, SFM_NOWAIT, &m_stBHAxis_BinEjT);
				}

				m_lCurLevel_BinEjT = nPos;
				
				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_BIN_EJ_T, 5000, &m_stBHAxis_BinEjT)) != gnOK)
					{
						nResult = Err_EjectorTMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);

			m_bHome_BinEjT	= FALSE;
			nResult			= Err_EjectorTMove;
		}
	}
	else
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

	if (nResult != gnOK)
	{
		SetMotionCE(TRUE, "BinEjT_MoveTo fail");
	}

	return nResult;
}


INT CBondHead::EjElevator_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (!m_bES101Configuration && !m_bMS60EjElevator)	//v4.51A5
		return gnNOTOK;

	if (m_fHardware && m_bSel_Ej)
	{
		try
		{
			if (!m_bHome_EjElevator)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_bMS60EjElevator)	//v4.51A5
				/*{
					m_lEnc_EjElevatorZ = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJELEVATOR, 0.8, &m_stBHAxis_EjElevator);
					if (m_lEnc_EjElevatorZ < nPos)
					{
						CMS896AStn::MotionSelectObwProfile(BH_AXIS_EJELEVATOR, BH_MP_E_OBW_DEFAULT, &m_stBHAxis_EjElevator);
					}
					else
					{
						CMS896AStn::MotionSelectObwProfile(BH_AXIS_EJELEVATOR, BH_MP_E_OBW_DOWN, &m_stBHAxis_EjElevator);
					}
					CMS896AStn::MotionObwMoveTo(BH_AXIS_EJELEVATOR, nPos, SFM_NOWAIT, &m_stBHAxis_EjElevator);
				}
				else*/
				CMS896AStn::MotionMoveTo(BH_AXIS_EJELEVATOR, nPos, SFM_NOWAIT, &m_stBHAxis_EjElevator);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_EJELEVATOR, 5000, &m_stBHAxis_EjElevator)) != gnOK)
					{
						nResult = gnNOTOK;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
			CMS896AStn::MotionClearError(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);

			m_bHome_EjElevator	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Ej)
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

	return nResult;
}


INT CBondHead::BinEjElevator_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (!m_bMS60EjElevator)
		return gnNOTOK;

	if (m_fHardware && m_bSel_BinEj && m_bEnableBinEjector && !m_bDisableBH)
	{
		try
		{
			if (!m_bHome_BinEjElevator)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (m_bMS60EjElevator)
				/*{
					m_lEnc_EjElevatorZ = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJELEVATOR, 0.8, &m_stBHAxis_EjElevator);
					if (m_lEnc_EjElevatorZ < nPos)
					{
						CMS896AStn::MotionSelectObwProfile(BH_AXIS_EJELEVATOR, BH_MP_E_OBW_DEFAULT, &m_stBHAxis_EjElevator);
					}
					else
					{
						CMS896AStn::MotionSelectObwProfile(BH_AXIS_EJELEVATOR, BH_MP_E_OBW_DOWN, &m_stBHAxis_EjElevator);
					}
					CMS896AStn::MotionObwMoveTo(BH_AXIS_EJELEVATOR, nPos, SFM_NOWAIT, &m_stBHAxis_EjElevator);
				}
				else*/
				CMS896AStn::MotionMoveTo(BH_AXIS_BIN_EJELEVATOR, nPos, SFM_NOWAIT, &m_stBHAxis_BinEjElevator);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_BIN_EJELEVATOR, 5000, &m_stBHAxis_BinEjElevator)) != gnOK)
					{
						nResult = gnNOTOK;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);

			m_bHome_BinEjElevator	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_BinEj)
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

	return nResult;
}


INT CBondHead::Ej_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Ej && !m_bDisableBH)		//v4.24
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_Ej)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( m_lE_ProfileType == MS896A_OBW_PROFILE && (!CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj)))	//Linear Ej
				{
					//Ej_SelectObwProfile(nPos);
					//Ej_SelectControl(nPos);		//andrewng //2020-0605
					CMS896AStn::MotionObwMove(BH_AXIS_EJ, nPos, SFM_NOWAIT, &m_stBHAxis_Ej);
				}
				else
				{
					CMS896AStn::MotionMove(BH_AXIS_EJ, nPos, SFM_NOWAIT, &m_stBHAxis_Ej);
				}

				m_lCurLevel_Ej	+= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_EJ, 5000, &m_stBHAxis_Ej)) != gnOK)
					{
						nResult = Err_EjectorMove;
					}
					m_bComplete_Ej = TRUE;		//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej);
			CMS896AStn::MotionClearError(BH_AXIS_EJ, &m_stBHAxis_Ej);

			m_bHome_Ej	= FALSE;
			nResult = Err_EjectorMove;
		}
	}
	else
	{
		if (m_bSel_Ej)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(20);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	//CheckResult(nResult, _T("Ejector Axis - Move"));

	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Ej_Move fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::BinEj_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_BinEj && !m_bDisableBH && m_bEnableBinEjector)		//v4.24
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			if (!m_bHome_BinEj)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( m_lBinE_ProfileType == MS896A_OBW_PROFILE && (!CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj)))	//Linear Ej
				{
					//BinEj_SelectObwProfile(nPos);
					//BinEj_SelectControl(nPos);	//andrewng //2020-0605
					CMS896AStn::MotionObwMove(BH_AXIS_BIN_EJ, nPos, SFM_NOWAIT, &m_stBHAxis_BinEj);
				}
				else
				{
					CMS896AStn::MotionMove(BH_AXIS_BIN_EJ, nPos, SFM_NOWAIT, &m_stBHAxis_BinEj);
				}

				m_lCurLevel_BinEj	+= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_BIN_EJ, 5000, &m_stBHAxis_BinEj)) != gnOK)
					{
						nResult = Err_EjectorMove;
					}
					m_bComplete_BinEj = TRUE;		//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);

			m_bHome_BinEj	= FALSE;
			nResult = Err_EjectorMove;
		}
	}
	else
	{
		if (m_bSel_BinEj)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(20);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "Ej_Move fail");
	}

	return nResult;
}


INT CBondHead::EjCap_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware)
	{
		if (IsMotionCE())
		{
			return Err_EjectorCapMove;
		}

		try
		{
			CMS896AStn::MotionSync(BH_AXIS_EJCAP, 1000, &m_stBHAxis_EjCap);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::BinEjCap_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_BinEj && m_bEnableBinEjector && !m_bDisableBH)
	{
		if (IsMotionCE())
		{
			return Err_EjectorCapMove;
		}

		try
		{
			CMS896AStn::MotionSync(BH_AXIS_BIN_EJCAP, 1000, &m_stBHAxis_BinEjCap);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::EjT_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bEnableEjectorTheta && !m_bDisableBH)
	{
		if (IsMotionCE())
		{
			return Err_EjectorTMove;
		}

		try
		{
			CMS896AStn::MotionSync(BH_AXIS_EJ_T, 1000, &m_stBHAxis_EjT);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::BinEjT_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bEnableBinEjector && m_bSel_BinEj && !m_bDisableBH)
	{
		if (IsMotionCE())
		{
			return Err_EjectorTMove;
		}

		try
		{
			CMS896AStn::MotionSync(BH_AXIS_BIN_EJ_T, 1000, &m_stBHAxis_BinEjT);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::Ej_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Ej && !m_bDisableBH)		//v4.24
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			CMS896AStn::MotionSync(BH_AXIS_EJ, 1000, &m_stBHAxis_Ej);

			m_bComplete_Ej = TRUE;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej);
			nResult = gnNOTOK;
		}	
	}
	// For disable ejector case
	if (!m_bSel_Ej)
	{
		if (!m_bDisableBH)		//v3.64
			Sleep(m_lTime_Ej);
		m_bComplete_Ej = TRUE;
	}

	//CheckResult(nResult, _T("Ejector Axis - Sync"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Ej_Sync fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::BinEj_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_BinEj && !m_bDisableBH && m_bEnableBinEjector)		//v4.24
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_EjectorMove;
			return nResult;
		}
		try
		{
			CMS896AStn::MotionSync(BH_AXIS_BIN_EJ, 1000, &m_stBHAxis_BinEj);

			m_bComplete_BinEj = TRUE;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			nResult = gnNOTOK;
		}	
	}

	// For disable ejector case
	if (!m_bSel_BinEj)
	{
		if (!m_bDisableBH)		//v3.64
			Sleep(m_lTime_Ej);
		m_bComplete_Ej = TRUE;
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "Ej_Sync fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::EjElevator_Sync()
{
	INT nResult = gnOK;

	if (!m_bES101Configuration && !m_bMS60EjElevator)	//v4.51A5
		return gnNOTOK;

	if( m_fHardware && m_bSel_Ej)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(BH_AXIS_EJELEVATOR, 10000, &m_stBHAxis_EjElevator);
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::BinEjElevator_Sync()
{
	INT nResult = gnOK;

	if (!m_bMS60EjElevator)
		return gnNOTOK;

	if (m_fHardware && m_bSel_BinEj && m_bEnableBinEjector && !m_bDisableBH)
	{
		if (IsMotionCE() == TRUE)
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(BH_AXIS_BIN_EJELEVATOR, 10000, &m_stBHAxis_BinEjElevator);
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}



INT CBondHead::EjCap_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_fHardware)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_EJCAP, &m_stBHAxis_EjCap) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
					m_bIsPowerOn_EjCap = TRUE;
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_EJCAP, &m_stBHAxis_EjCap) == TRUE)
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
					m_bIsPowerOn_EjCap = FALSE;
					m_bComm_EjCap = FALSE;
					m_bHome_EjCap = FALSE;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::BinEjCap_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_fHardware)
	{
		try
		{
			if (bOn)
			{
				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
					m_bIsPowerOn_BinEjCap	= TRUE;
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap) == TRUE)
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
					m_bIsPowerOn_BinEjCap	= FALSE;
					m_bComm_BinEjCap		= FALSE;
					m_bHome_BinEjCap		= FALSE;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::EjT_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_fHardware && !m_bDisableBH && m_bEnableEjectorTheta)
	{
		try
		{
			if (bOn)
			{
				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_T, &m_stBHAxis_EjT) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
					m_bIsPowerOn_EjT = TRUE;
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_T, &m_stBHAxis_EjT) == TRUE)
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
					m_bIsPowerOn_EjT = FALSE;
					m_bComm_EjT		 = FALSE;
					m_bHome_EjT		 = FALSE;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::BinEjT_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_fHardware)
	{
		try
		{
			if (bOn)
			{
				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
					m_bIsPowerOn_BinEjT = TRUE;
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT) == TRUE)
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
					m_bIsPowerOn_BinEjT	 = FALSE;
					m_bComm_BinEjT		 = FALSE;
					m_bHome_BinEjT		 = FALSE;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::Ej_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Ej && !m_bDisableBH)	//v4.24
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ, &m_stBHAxis_Ej) == FALSE )
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_EJ, &m_stBHAxis_Ej);
					m_bIsPowerOn_Ej = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ, &m_stBHAxis_Ej) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_EJ, &m_stBHAxis_Ej);
					m_bIsPowerOn_Ej = FALSE;
					m_bComm_Ej = FALSE;
					m_bHome_Ej = FALSE;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej);
			nResult = gnNOTOK;
		}	
	}
	//CheckResult(nResult, _T("Ejector Axis - PowerOn"));
	return nResult;
}


INT CBondHead::BinEj_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_fHardware)	//v4.24
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj) == FALSE )
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
					m_bIsPowerOn_BinEj = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
					m_bIsPowerOn_BinEj = FALSE;
					m_bComm_BinEj = FALSE;
					m_bHome_BinEj = FALSE;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


INT CBondHead::EjElevator_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (!m_bES101Configuration && !m_bMS60EjElevator)	//v4.51A5
		return gnNOTOK;

	if (m_fHardware && m_bSel_Ej)
	{
		try 
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
					m_bIsPowerOn_EjElevator = TRUE; 
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
					m_bIsPowerOn_EjElevator = FALSE; 
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}


INT CBondHead::BinEjElevator_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (!m_bMS60EjElevator)	//v4.51A5
		return gnNOTOK;

	if (m_fHardware && m_bSel_BinEj && m_bEnableBinEjector && !m_bDisableBH)
	{
		try 
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
					m_bIsPowerOn_BinEjElevator = TRUE; 
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
					m_bIsPowerOn_BinEjElevator = FALSE; 
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}


INT CBondHead::Ej_Comm()
{
	INT nResult;

	if (m_fHardware && m_bSel_Ej && !m_bDisableBH)		//v4.24
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		SetMotionCE(FALSE, "reset in Ej_Comm");	//v4.59A19

		if (m_lE_CommMethod == MS896A_NO_SWCOMM)
		{
			nResult = gnOK;
		}
		else
		{
			//nResult = CommutateServo("EjectorAxis");
			nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_EJ, &m_stBHAxis_Ej);
		}

		if ( nResult == gnOK )
		{
			m_bComm_Ej = TRUE;
		}
		else
		{
			nResult		= Err_EjectorCommutate;
			//ClearServoError("srvEjector");
			CMS896AStn::MotionClearError(BH_AXIS_EJ, &m_stBHAxis_Ej);
			m_bComm_Ej = FALSE;
		}
		//CheckResult(nResult, _T("Ejector Axis - Commutate"));
	}
	return gnOK;
}


INT CBondHead::BinEj_Comm()
{
	INT nResult;

	if (m_fHardware && m_bSel_BinEj && !m_bDisableBH && m_bEnableBinEjector)
	{
		SetMotionCE(FALSE, "reset in BinEj_Comm");

		if (m_lBinE_CommMethod == MS896A_NO_SWCOMM)
		{
			nResult = gnOK;
		}
		else
		{
			nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
		}

		if ( nResult == gnOK )
		{
			m_bComm_BinEj = TRUE;
		}
		else
		{
			nResult			= Err_EjectorCommutate;
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			m_bComm_BinEj	= FALSE;
		}
	}
	return nResult;
}


INT CBondHead::EjCap_Comm()
{
	INT nResult;

	if (m_fHardware && m_bSel_Ej && !m_bDisableBH)
	{
		if (CMS896AStn::MotionIsStepper(BH_AXIS_EJCAP, &m_stBHAxis_EjCap))
		{
			m_bComm_EjCap = TRUE;
			return gnOK;
		}

		SetMotionCE(FALSE, "reset in EjCap_Comm");	//v4.59A19

		nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);

		if ( nResult == gnOK )
		{
			m_bComm_EjCap = TRUE;
		}
		else
		{
			nResult		= Err_EjectorCapCommutate;
			CMS896AStn::MotionClearError(BH_AXIS_EJ, &m_stBHAxis_Ej);
			m_bComm_EjCap = FALSE;
		}
	}

	return gnOK;
}


INT CBondHead::BinEjCap_Comm()
{
	INT nResult = gnOK;
	
	if (m_fHardware && m_bSel_BinEj && m_bEnableBinEjector && !m_bDisableBH)
	{
		if (CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap))
		{
			m_bComm_BinEjCap = TRUE;
			return nResult;
		}

		SetMotionCE(FALSE, "reset in BinEjCap_Comm");	//v4.59A19

		nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);

		if ( nResult == gnOK )
		{
			m_bComm_BinEjCap = TRUE;
		}
		else
		{
			nResult		= Err_EjectorCapCommutate;
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
			m_bComm_BinEjCap = FALSE;
		}
	}

	return nResult;
}


INT CBondHead::EjT_Comm()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_Ej && !m_bDisableBH && m_bEnableEjectorTheta)
	{
		if (CMS896AStn::MotionIsStepper(BH_AXIS_EJ_T, &m_stBHAxis_EjT))
		{
			m_bComm_EjT = TRUE;
			return nResult;
		}

		SetMotionCE(FALSE, "reset in EjT_Comm");

		nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_EJ_T, &m_stBHAxis_EjT);

		if (nResult == gnOK)
		{
			m_bComm_EjT = TRUE;
		}
		else
		{
			nResult		= Err_EjectorTCommutate;
			CMS896AStn::MotionClearError(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
			m_bComm_EjT	= FALSE;
		}
	}

	return nResult;
}


INT CBondHead::BinEjT_Comm()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_BinEj && !m_bDisableBH && m_bEnableBinEjector)
	{
		if (CMS896AStn::MotionIsStepper(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT))
		{
			m_bComm_BinEjT = TRUE;
			return nResult;
		}

		SetMotionCE(FALSE, "reset in BinEjT_Comm");

		nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);

		if (nResult == gnOK)
		{
			m_bComm_BinEjT = TRUE;
		}
		else
		{
			nResult			= Err_EjectorTCommutate;
			CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
			m_bComm_BinEjT	= FALSE;
		}
	}

	return nResult;
}


BOOL CBondHead::Ej_IsComplete()
{
	if (m_fHardware && m_bSel_Ej && !m_bDisableBH)		//v4.08
	{
		return CMS896AStn::MotionIsComplete(BH_AXIS_EJ, &m_stBHAxis_Ej);
	}
	else
	{
		return TRUE;
	}
}


BOOL CBondHead::BinEj_IsComplete()
{
	if (m_fHardware && m_bSel_BinEj && !m_bDisableBH && m_bEnableBinEjector)		//v4.08
	{
		return CMS896AStn::MotionIsComplete(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
	}
	else
	{
		return TRUE;
	}
}


BOOL CBondHead::Ej_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (!m_bSel_Ej)		//v4.24
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ, &m_stBHAxis_Ej);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}


BOOL CBondHead::BinEj_IsPowerOn()
{
	if (!m_fHardware)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
	
	return FALSE;
}

BOOL CBondHead::EjCap_IsPowerOn()
{
	if (!m_fHardware)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CBondHead::BinEjCap_IsPowerOn()
{
	if (!m_fHardware)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}


BOOL CBondHead::EjT_IsPowerOn()
{
	if (!m_fHardware || !m_bEnableEjectorTheta || m_bDisableBH)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}


BOOL CBondHead::BinEjT_IsPowerOn()
{
	if (!m_fHardware && !m_bEnableBinEjector && m_bDisableBH)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
	
	return FALSE;
}


BOOL CBondHead::EjElevator_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (!m_bES101Configuration && !m_bMS60EjElevator)
	{
		return FALSE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}


BOOL CBondHead::BinEjElevator_IsPowerOn()
{
	if (!m_fHardware || !m_bDisableBH)
	{
		return TRUE;
	}
	if (!m_bMS60EjElevator || !m_bEnableBinEjector)
	{
		return FALSE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
	
	return FALSE;
}


INT CBondHead::EjElv_MoveToHome()
{
	INT nResult	= gnOK;

	if (EjElevator_IsPowerOn())	
	{
		nResult = EjElevator_MoveTo(0);
	}
	else
	{
		Sleep(500);
		SetErrorMessage("EjElv_MoveToHome HOME");
		nResult = EjElevator_Home();
		if (!EjElevator_IsPowerOn())		//v4.46T21 //Retry once
		{
			SetErrorMessage("EjElv_MoveToHome HOME retry");
			Sleep(1000);
			nResult = EjElevator_Home();
		}
	}

	if (!EjElevator_IsPowerOn())
	{
		nResult = gnNOTOK;
	}

	return nResult;
}

BOOL CBondHead::AutoLearnEjElvLevelUsingAirFlowSensor(BOOL bBHZ2)
{
	INT			nStatus;
	LONG		lStepSize = 20;
	//LONG		lMaxPos = 0;

	//Check min limit value
	//lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								

	//Test missing die sensor reply
	SetPickVacuum(TRUE);
	Sleep(1000);
	if (IsColletJam() == TRUE)
	{
		OpDisplayAlarmPage(TRUE);	//v4.53A13
		//HmiMessage("Collet jam or no pressure\nPlease check!", "Auto learn level Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_1);	//v4.50A1
		SetPickVacuum(FALSE);
		return FALSE;
	}

	if (bBHZ2)
	{
		m_lBPGeneral_B = 0;
		nStatus = EjElevator_MoveTo(m_lBPGeneral_B);
	}
	else
	{
		m_lBPGeneral_6 = 0;
		nStatus = EjElevator_MoveTo(m_lBPGeneral_6);
	}

	LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
	LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
	CString szLog;

	//Start Learning Missing Die Sensor
	if (bBHZ2)
	{
		SetPickVacuumZ2(TRUE);
		Sleep(500);

		while (1)
		{
			Sleep(50);

			if (IsColletJamZ2() == TRUE)
			{
				Sleep(50);
				if (IsColletJamZ2() == TRUE)
				{
					break;
				}
			}

			szLog.Format("AutoLearn Ej-CAP at BHZ2: encoder = %ld; air-flow = %ld", 
							m_lBPGeneral_B, m_ulBHZ2DAirFlowValue);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			m_lBPGeneral_B += lStepSize;
			if ((m_lBPGeneral_B <= lMinPos) || (m_lBPGeneral_B >= lMaxPos))
			{
				SetPickVacuum(FALSE);
				CString szErr;
				szErr.Format("AUTO-LEARN: EJ-CAP encoder value is out of limit! %ld, %ld, %ld", 
					m_lBPGeneral_B, lMinPos, lMaxPos);
				HmiMessage_Red_Back(szErr, "Auto Learn");
				return TRUE;
			}

			nStatus = EjElevator_MoveTo(m_lBPGeneral_B);

			if (nStatus != gnOK)
			{
				SetPickVacuumZ2(FALSE);
				//HmiMessage("Learn Failed", "Auto learn level Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
				return FALSE;
			}
		}

		SetPickVacuumZ2(FALSE);
	}
	else
	{
		SetPickVacuum(TRUE);
		Sleep(500);

		while (1)
		{
			Sleep(50);

			if (IsColletJam() == TRUE)
			{
				Sleep(50);
				if (IsColletJam() == TRUE)
				{
					break;
				}
			}

			szLog.Format("AutoLearn Ej-CAP at BHZ1: encoder = %ld; air-flow = %ld", 
							m_lBPGeneral_6, m_ulBHZ1DAirFlowValue);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			m_lBPGeneral_6 += lStepSize;
			if ((m_lBPGeneral_6 <= lMinPos) || (m_lBPGeneral_6 >= lMaxPos))
			{
				SetPickVacuum(FALSE);
				CString szErr;
				szErr.Format("AUTO-LEARN: EJ-CAP encoder value is out of limit! %ld, %ld, %ld", 
					m_lBPGeneral_6, lMinPos, lMaxPos);
				HmiMessage_Red_Back(szErr, "Auto Learn");
				return TRUE;
			}

			nStatus = EjElevator_MoveTo(m_lBPGeneral_6);

			if (nStatus != gnOK)
			{
				SetPickVacuum(FALSE);
				//HmiMessage("Learn Failed", "Auto learn level Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
				return FALSE;
			}
		}

		SetPickVacuum(FALSE);
	}

	SetStatusMessage("Auto learn EJ-CAP level completed");
	return TRUE;
}

BOOL CBondHead::NGPickT_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bES101Configuration && !m_bDisableBH)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_NGPICK, &m_stBHAxis_NGPick) != 0)
			{
				m_bHome_NGPick = FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
			}

			if (nResult == gnOK)
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_NGPICK, &m_stBHAxis_NGPick) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
					Sleep(100);
				}


				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_NGPICK, &m_stBHAxis_NGPick) == FALSE)
				{
					m_bIsPowerOn_NGPickT = FALSE;
					return gnNOTOK;
				}

				//Sleep(100);
				CMS896AStn::MotionSetPosition(BH_AXIS_NGPICK, 0, &m_stBHAxis_NGPick);
				m_bIsPowerOn_NGPickT = TRUE;
				m_bHome_NGPick	= TRUE;
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
			CMS896AStn::MotionClearError(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
			m_bIsPowerOn_NGPickT = FALSE;
			m_bHome_NGPick = FALSE;
			nResult = gnNOTOK; 
		}
	}
	else
	{
		if (m_bES101Configuration)
		{
			Sleep(100);
			m_bHome_NGPick			= TRUE;
			m_bIsPowerOn_NGPickT	= TRUE;
		}
	}
	return nResult;
}

BOOL CBondHead::NGPickT_IsPowerOn()
{
	//No BHT in Mega Da
	return TRUE;

	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (!m_bES101Configuration)	
	{
		return FALSE;
	}

	if (m_bDisableBH)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT	CBondHead::NGPickT_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_fHardware && m_bES101Configuration && !m_bDisableBH)
	{
		try 
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_NGPICK, &m_stBHAxis_NGPick) == FALSE)
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
					m_bIsPowerOn_NGPickT = TRUE; 
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_NGPICK, &m_stBHAxis_NGPick) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
					m_bIsPowerOn_NGPickT = FALSE; 
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
			m_bHome_NGPick		 = FALSE;
			m_bIsPowerOn_NGPickT = FALSE;
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CBondHead::NGPickT_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bES101Configuration && !m_bDisableBH)
	{
		if ( IsMotionCE() == TRUE )
		{
			return gnNOTOK;
		}

		try
		{
			if (!m_bHome_NGPick)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
//CString szLog;
//szLog.Format("Andrew: NGPickT_Move  %d", nPos);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

				CMS896AStn::MotionMove(BH_AXIS_NGPICK, nPos, SFM_NOWAIT, &m_stBHAxis_NGPick);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_NGPICK, 5000, &m_stBHAxis_NGPick)) != gnOK)
					{
						nResult = gnNOTOK;
					}

					Sleep(10);
					CMS896AStn::MotionSetPosition(BH_AXIS_NGPICK, 0, &m_stBHAxis_NGPick);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
			CMS896AStn::MotionClearError(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);

			m_bHome_NGPick	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bES101Configuration)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(20);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	return nResult;
}

INT CBondHead::NGPickT_Sync()
{
	INT nResult = gnOK;
	if( m_fHardware && m_bES101Configuration && !m_bDisableBH)
	{
		if ( IsMotionCE() == TRUE )
		{
			return gnNOTOK;
		}

		try
		{
			CMS896AStn::MotionSync(BH_AXIS_NGPICK, 10000, &m_stBHAxis_NGPick);

			Sleep(10);
			CMS896AStn::MotionSetPosition(BH_AXIS_NGPICK, 0, &m_stBHAxis_NGPick);
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_NGPICK, &m_stBHAxis_NGPick);
			m_bHome_NGPick	= FALSE;
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}


////////////////////////////////////////////
//	Bond Arm
////////////////////////////////////////////

INT CBondHead::T_Home(BOOL bProgramInit)
{
	INT nResult			= gnOK;
	return nResult;
	LONG lTmpBHTLimit	= 0;
	LONG lBHTLowerLimit = 0;


	if (m_fHardware && m_bSel_T && !m_bDisableBH)	//v4.08
	{
		//v4.55A8
		INT nAlarmCount = 0;
		while ((IsCoverOpen() == TRUE))
		{
			if (bProgramInit)
			{
				AfxMessageBox("BH Cover Sensor is opened; please close the cover to continue.", MB_SYSTEMMODAL);
			}
			else
			{
				SetAlert_Red_Back(IDS_BH_COVER_OPEN);
			}

			nAlarmCount++;

			if (nAlarmCount > 3)
			{
				//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
				SetMotionCE(TRUE, "T_Home exceeds ALARM count");	//v4.59A19
				m_bHome_T = FALSE;
				return gnNOTOK;
			}
		}

		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (!pBinLoader->IsBTFramePlatformDown())
		{
			HmiMessage_Red_Yellow("Frame Level Not Down When T Home");
		}

		(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		EnableBHTThermalControl(FALSE);				//v4.49A5
	
		try
		{
#ifdef NU_MOTION
			//Enlarge the softawre limit before Home
			CMS896AStn::MotionSetSoftwareLimit(BH_AXIS_T, 99999999, -99999999, &m_stBHAxis_T);
#endif

			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_T, &m_stBHAxis_T) != 0)
			{
				m_bComm_T	= FALSE;
				CMS896AApp::m_bBondHeadTThermalControl = FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);
			}

			if (!CMS896AStn::MotionIsPowerOn(BH_AXIS_T, &m_stBHAxis_T))
			{
				m_bComm_T	= FALSE;
			}

			if (!m_bComm_T)
			{
				m_bHome_T	= FALSE;
				CMS896AApp::m_bBondHeadTThermalControl = FALSE;

				switch(m_lT_CommMethod)
				{
					case MS896A_GEN_SWCOMM:					
						nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_T, &m_stBHAxis_T);
						break;

					case MS896A_DIR_SWCOMM:					
						nResult = CMS896AStn::MotionDirectionalCommutateServo(BH_AXIS_T, 8,8000, HP_NEGATIVE_DIR, &m_stBHAxis_T);
						break;

					case MS896A_OPEN_SWCOMM:					
						//Call directional comm 
/*						
						if (CMS896AStn::MotionIsPositiveLimitHigh(BH_AXIS_T, &m_stBHAxis_T))
						{
							HmiMessage_Red_Yellow("BHT is at upper limit sensor position, can not do commutation");
							m_bHome_T = FALSE;
							return gnNOTOK;
						}
						nResult = CMS896AStn::MotionDirectionalCommutateServo(BH_AXIS_T, 16, 500000, HP_NEGATIVE_DIR, &m_stBHAxis_T);
						if (nResult != gnOK)
						{
							nResult = CMS896AStn::MotionDirectionalCommutateServo(BH_AXIS_T, 16, 500000, HP_POSITIVE_DIR, &m_stBHAxis_T);
						}
						if (nResult == gnOK)
*/
						{
							nResult = CMS896AStn::MotionOpenCommutateServo(BH_AXIS_T, &m_stBHAxis_T, 16, 500000, 5000, 0, 1000, 3000, 2000, 5000, 0x500, -1, 14);
						}
						break;
					
					default:
						nResult = gnOK;
						break;
				}


				if (nResult == gnOK)
				{
					m_bComm_T = TRUE;
				}
				else
				{
					nResult		= Err_BhTCommutate;
					CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);
				}
			}

			if (nResult == gnOK)
			{
/*
				if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
					 CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z) )		//UBH
				{
					if ( CMS896AStn::MotionSetControlParam(BH_AXIS_T, BH_STATIC_CONTROL_HOME_OBW_T, &m_stBHAxis_T) )
					{
						CMS896AStn::MotionSelectControlParam(BH_AXIS_T, BH_STATIC_CONTROL_HOME_OBW_T, &m_stBHAxis_T);
					}
					
					CMS896AStn::MotionSetDacLimit(BH_AXIS_Z, 32766, 256, &m_stBHAxis_T);

					CMS896AStn::MotionPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);

					//Search Positive Limit sensor 
					CMS896AStn::MotionSelectSearchProfile(BH_AXIS_T, BH_SP_POS_LIMIT_T, &m_stBHAxis_T);
					CMS896AStn::MotionSearch(BH_AXIS_T, 0, SFM_WAIT, &m_stBHAxis_T);

					//Search Index sensor & Drive In
					CMS896AStn::MotionSelectSearchProfile(BH_AXIS_T, BH_SP_INDEX_T, &m_stBHAxis_T);
					CMS896AStn::MotionSearch(BH_AXIS_T, 1, SFM_WAIT, &m_stBHAxis_T);

					//Search Index sensor with very slow speed
					CMS896AStn::MotionSelectSearchProfile(BH_AXIS_T, BH_SP_INDEX_SLOW_T, &m_stBHAxis_T);
					CMS896AStn::MotionSearch(BH_AXIS_T, 0, SFM_WAIT, &m_stBHAxis_T);

					if ( CMS896AStn::MotionSetControlParam(BH_AXIS_T, BH_DYNAMIC_CONTROL_OBW_T, &m_stBHAxis_T) )
					{
						CMS896AStn::MotionSelectControlParam(BH_AXIS_T, BH_DYNAMIC_CONTROL_OBW_T, &m_stBHAxis_T);
					}
				}
				else
*/
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_T, &m_stBHAxis_T);

					if (m_lT_CommMethod == MS896A_OPEN_SWCOMM)
					{
						LONG lStaticControl	= GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_STAT_CONTROL);
						LONG lDynaControl	= GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_DYNA_CONTROL);
					
						//always use static control to home theta
						CMS896AStn::MotionSelectControlParam(BH_AXIS_T, BH_STATIC_CONTROL_OBW_T, &m_stBHAxis_T);
						CMS896AStn::MotionSelectControlType(BH_AXIS_T, lStaticControl, 2, &m_stBHAxis_T);

						//if (CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_T, &m_stBHAxis_T))
						if (CMS896AStn::MotionIsPositiveLimitHigh(BH_AXIS_T, &m_stBHAxis_T))
						{
							//Search limit sensor with very slow speed
							CMS896AStn::MotionSearch(BH_AXIS_T, 0, SFM_WAIT, &m_stBHAxis_T, BH_SP_POS_LIMIT_SLOW_T);
							m_bHome_T	= TRUE;
							nResult = T_Move(-50000, SFM_WAIT, BH_MP_T_OBW_SETUP);
							Sleep(100);
						}

						//Search Positive Limit sensor 
						CMS896AStn::MotionSelectSearchProfile(BH_AXIS_T, BH_SP_POS_LIMIT_T, &m_stBHAxis_T);
						CMS896AStn::MotionSearch(BH_AXIS_T, 1, SFM_WAIT, &m_stBHAxis_T, BH_SP_POS_LIMIT_T);

						if (!CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_T, &m_stBHAxis_T))
						{
							nResult = gnAMS_NOTOK;
						}

						if (nResult == gnAMS_OK)
						{
							m_bHome_T	= TRUE;
							T_Move(-312500, SFM_WAIT, BH_MP_T_OBW_SETUP);

							Sleep(100);
							GetEncoderValue();
							lTmpBHTLimit = m_lEnc_T;

							if (IsMS60() && T_IsPowerOn())
							{
								InitMS60ThermalCtrl();
							}


						//Search Index sensor & Drive In
	//					CMS896AStn::MotionSelectSearchProfile(BH_AXIS_T, BH_SP_INDEX_T, &m_stBHAxis_T);
	//					CMS896AStn::MotionSearch(BH_AXIS_T, 0, SFM_WAIT, &m_stBHAxis_T, BH_SP_INDEX_T);

							CMS896AStn::MotionSelectControlParam(BH_AXIS_T, BH_DYNAMIC_CONTROL_OBW_T, &m_stBHAxis_T);
							CMS896AStn::MotionSelectControlType(BH_AXIS_T, lDynaControl, 2, &m_stBHAxis_T);

							if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_T, &m_stBHAxis_T) == 0) && 
								CMS896AStn::MotionIsPowerOn(BH_AXIS_T, &m_stBHAxis_T))
							{
								nResult = gnAMS_OK;
							}
							else
							{
								nResult = gnNOTOK;
							}
						}
					}
					else
					{
						nResult = CMS896AStn::MotionMoveHome(BH_AXIS_T, 1, 0, &m_stBHAxis_T);
					}
				}

				if (nResult == gnAMS_OK)
				{
					Sleep(200);
					GetEncoderValue();
					lTmpBHTLimit = lTmpBHTLimit - m_lEnc_T;
					CMS896AStn::MotionSetPosition(BH_AXIS_T, 0, &m_stBHAxis_T);

					m_bHome_T	= TRUE;
					m_lCurPos_T	= 0;	//	after home T
					m_bStartContour_T = FALSE; 
					CMS896AApp::m_bBondHeadTThermalControl = TRUE;

					lBHTLowerLimit = GetChannelInformation(MS896A_CFG_CH_BONDARM, MS896A_CFG_CH_MAX_DISTANCE);
					// enlarge the limit a little bit
					lBHTLowerLimit = lBHTLowerLimit + BH_T_TRAVEL_RANGE_MS100_OFFSET;

					//CMS896AStn::MotionSetSoftwareLimit(BH_AXIS_T, lTmpBHTLimit-5000, lTmpBHTLimit-lBHTLowerLimit, &m_stBHAxis_T);
					CMS896AStn::MotionSetSoftwareLimit(BH_AXIS_T, 625000, -625000, &m_stBHAxis_T); //Set SW Limit by Matthew 20190508
					CString szLog;
					szLog.Format("BHT limit: %d, %d", 625000/*lTmpBHTLimit-5000*/, -625000/*lTmpBHTLimit-lBHTLowerLimit*/);
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

					EnableBHTThermalControl(TRUE);					//v4.49A5

				}
				else if (nResult == gnNOTOK)
				{
					DisplayMessage("    BHT: HOME fails");		//UBH
					nResult = Err_BhTMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);

			m_bComm_T	= FALSE;
			m_bHome_T	= FALSE;
			nResult = Err_BhTMoveHome;
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

#define T_POSN_TOL		100	//	MS109 Lumileds Contamination
INT CBondHead::GetContourMoveState(LONG& lMoveState, INT nPos, INT& nRelativeDistance)
{
	if ((nPos == m_lPickPos_T) && labs(m_lCurPos_T - m_lBondPos_T)<=T_POSN_TOL )
	{
		nRelativeDistance = (INT)(m_lPickPos_T - m_lCurPos_T);
		lMoveState = BH_T_BOND_TO_PICK;
	}
	else if ((nPos == m_lBondPos_T) && labs(m_lCurPos_T - m_lPickPos_T)<=T_POSN_TOL )
	{
		nRelativeDistance = (INT)(m_lBondPos_T - m_lCurPos_T);
		lMoveState = BH_T_PICK_TO_BOND;
	}
	else if ((nPos == m_lPickPos_T) && labs(m_lCurPos_T - m_lPrePickPos_T)<=T_POSN_TOL)
	{
		nRelativeDistance = (INT) (m_lPickPos_T - m_lCurPos_T);
		lMoveState = BH_T_PREPICK_TO_PICK;
	}
	else if ((nPos == m_lBondPos_T) && labs(m_lCurPos_T - m_lPrePickPos_T)<=T_POSN_TOL)
	{
		nRelativeDistance = (INT) (m_lBondPos_T - m_lCurPos_T);
		lMoveState = BH_T_PREPICK_TO_BOND;
	}
	else
	{
		lMoveState = BH_NA;
	}

	return 1;
}

// interface for BH
INT CBondHead::BA_MoveTo(INT nPos, INT nMode, BOOL bCheckMotionSync)
{
	INT nResult			= gnOK;
	return nResult;
	LONG lStartOffset	= 0;
#ifndef ES101
#ifdef NU_MOTION
	if (IsEnableILC() == TRUE)
	{
		LONG lBondHeadState = BH_NA;
		UINT uiPointToUse;
		INT nRelativeDistance;
		
		GetContourMoveState(lBondHeadState, nPos, nRelativeDistance);

		if (IsMS60())
			T_IsContourMoveComplete(TRUE);		//v4.47T1	//v4.47A1
		else
			T_IsContourMoveComplete();
		ILC_LogRunTimeUpdateData(lBondHeadState);

		if (lBondHeadState == BH_T_PICK_TO_BOND)
		{
			lStartOffset = ILC_GetContourMoveStartOffset(lBondHeadState);
			uiPointToUse = m_stBAILCPTB.ILCConfigInfo.ApplyingLength;
			nResult = T_ContourMove(nRelativeDistance, uiPointToUse, lStartOffset , nMode, bCheckMotionSync);
		}
		else if (lBondHeadState == BH_T_BOND_TO_PICK)
		{
			lStartOffset = ILC_GetContourMoveStartOffset(lBondHeadState);
			uiPointToUse = m_stBAILCBTP.ILCConfigInfo.ApplyingLength;
			nResult = T_ContourMove(nRelativeDistance, uiPointToUse, lStartOffset , nMode, bCheckMotionSync);
		}
		else if (lBondHeadState == BH_T_PREPICK_TO_PICK)
		{
			T_MoveTo(m_lPickPos_T, nMode);		//v4.54A10
		}
		else if( lBondHeadState == BH_T_PREPICK_TO_BOND )
		{		
			T_MoveTo(m_lBondPos_T, nMode);		//v4.54A10
		}
		else
		{
			T_MoveTo(nPos, nMode);				//v4.54A10
		}
	}
	else
#endif
#endif
	{
		nResult = T_MoveTo(nPos, nMode);
	}
	
	return nResult;
}

INT CBondHead::T_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;
	return nResult;
	if (m_fHardware && m_bSel_T && !m_bDisableBH)	//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhTMove;
			return nResult;
		}

		if (!LoopCheckCoverOpen("T Move To"))
		{
			nResult = Err_BhTMove;
			return nResult;
		}

		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (!pBinLoader->IsBTFramePlatformDown())
		{
			HmiMessage_Red_Yellow("Frame Level Not Down When T Move To");
		}

		try
		{
			if (!m_bHome_T)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( (m_lT_ProfileType == MS896A_OBW_PROFILE || m_lT_ProfileType == MS896A_ILC_PROFILE) && 
					(!CMS896AStn::MotionIsStepper(BH_AXIS_T, &m_stBHAxis_T)) )
				{
					T_SelectObwProfile((LONG)(nPos - m_lCurPos_T));
					T_SelectControl((LONG)(nPos - m_lCurPos_T), BH_OBW_MV);	//Nu Motion
					
					if (IsEnableILC() == TRUE)
					{
						T_IsContourMoveComplete();
						ILC_CleanDataLog();
					}
				
					CMS896AStn::MotionObwMoveTo(BH_AXIS_T, nPos, SFM_NOWAIT, &m_stBHAxis_T);
				}
				else
				{

					if (IsEnableILC() == TRUE)
					{
						T_IsContourMoveComplete();
						ILC_CleanDataLog();
					}
					
					CMS896AStn::MotionMoveTo(BH_AXIS_T, nPos, SFM_NOWAIT, &m_stBHAxis_T);
				}

				m_lCurPos_T	= nPos;	//	T Move To

				if ((CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
					CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) // UBH
				{
					/*if ((nResult = T_HpDelay())!= HP_SUCCESS)
					{
						nResult = Err_BhTMove;
					}*/
				}

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BH_AXIS_T, 10000, &m_stBHAxis_T);
					if ( nResult != gnOK)
					{
						nResult = Err_BhTMove;
					}
					m_bComplete_T = TRUE;	//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);
			T_PowerOn(FALSE);
			m_bHome_T	= FALSE;
			nResult = Err_BhTMove;

#ifdef NU_MOTION
			if (m_bIsDataLogForBHDebug == TRUE)
			{
				NuMotionDataLogForBHDebug(FALSE);
				m_bIsDataLogForBHDebug = FALSE;
			}
#endif
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

	//CheckResult(nResult, _T("BondArm Axis - MoveTo"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "T_MoveTo fail");	//v4.59A19
	}

	return nResult;
}

//v4.53A14
INT CBondHead::T_SMoveTo(INT nPos, INT nMode)
{
	return 1;
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_T && !m_bDisableBH)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhTMove;
			return nResult;
		}

		if (!LoopCheckCoverOpen("T SMove To"))
		{
			nResult = Err_BhTMove;
			return nResult;
		}

		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (!pBinLoader->IsBTFramePlatformDown())
		{
			HmiMessage_Red_Yellow("Frame Level Not Down When T SMove To");
		}
		try
		{
			if (!m_bHome_T)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( (m_lT_ProfileType == MS896A_OBW_PROFILE || m_lT_ProfileType == MS896A_ILC_PROFILE) && 
					(!CMS896AStn::MotionIsStepper(BH_AXIS_T, &m_stBHAxis_T)) )
				{
					CMS896AStn::MotionObwMoveTo(BH_AXIS_T, nPos, SFM_NOWAIT, &m_stBHAxis_T);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BH_AXIS_T, nPos, SFM_NOWAIT, &m_stBHAxis_T);
				}

				m_lCurPos_T	= nPos;	//	T SMove To

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BH_AXIS_T, 10000, &m_stBHAxis_T);
					if ( nResult != gnOK)
					{
						nResult = Err_BhTMove;
					}
					m_bComplete_T = TRUE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);
			T_PowerOn(FALSE);
			m_bHome_T	= FALSE;
			nResult = Err_BhTMove;
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
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "T_SMoveTo fail");	//v4.59A19
	}
	return nResult;
}

INT CBondHead::T_Move(INT nPos, INT nMode, CString szProfile)
{
	INT nResult			= gnOK;
	return nResult;
	if (m_fHardware && m_bSel_T && !m_bDisableBH)	//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhTMove;
			return nResult;
		}

		if (!LoopCheckCoverOpen("T Move"))
		{
			nResult = Err_BhTMove;
			return nResult;
		}

		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (!pBinLoader->IsBTFramePlatformDown())
		{
			HmiMessage_Red_Yellow("Frame Level Not Down When T Move");
		}

		try
		{
			if (!m_bHome_T)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( (m_lT_ProfileType == MS896A_OBW_PROFILE || m_lT_ProfileType == MS896A_ILC_PROFILE ) && 
					(!CMS896AStn::MotionIsStepper(BH_AXIS_T, &m_stBHAxis_T)) )
				{
					T_SelectObwProfile((LONG)(nPos), szProfile);
					T_SelectControl((LONG)(nPos), BH_OBW_MV);	//Nu Motion
					
					if (IsEnableILC() == TRUE)
					{
						T_IsContourMoveComplete();
						ILC_CleanDataLog();
					}

					CMS896AStn::MotionObwMove(BH_AXIS_T, nPos, SFM_NOWAIT, &m_stBHAxis_T);
				}
				else
				{
					if (IsEnableILC() == TRUE)
					{
						T_IsContourMoveComplete();
						ILC_CleanDataLog();
					}

					CMS896AStn::MotionMove(BH_AXIS_T, nPos, SFM_NOWAIT, &m_stBHAxis_T);
				}

				m_lCurPos_T	+= nPos;	//	T Move

				if ((CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
					CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) // UBH
				{
					/*if ((nResult = T_HpDelay())!= HP_SUCCESS)
					{
						nResult = Err_BhTMove;
					}*/
				}

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_T, 10000, &m_stBHAxis_T)) != gnOK)
					{
						nResult = Err_BhTMove;
					}
					m_bComplete_T = TRUE;	//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);
			T_PowerOn(FALSE);
			m_bHome_T	= FALSE;
			nResult = Err_BhTMove;
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

	CheckResult(nResult, _T("BondArm Axis - Move"));

	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "T_Move fail");	//v4.59A19
	}

	return nResult;
}

INT CBondHead::T_ContourMove(INT nPos, INT nDataLength, INT nStartOffset, INT nMode, BOOL bCheckMotionSync)
{
	return 1;
	INT nResult	= gnOK;
	BOOL bSel_T = m_bSel_T;
	CString szNuControlParaID;
	
	if (m_fHardware && m_bSel_T && !m_bDisableBH)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhTMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_T)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				T_SelectControl(nPos, BH_CONTOUR);

				CMS896AStn::MotionContourMove(BH_AXIS_T, nDataLength, nStartOffset ,FALSE, &m_stBHAxis_T);
				
				m_lCurPos_T	+= nPos;	//	T Contour Move
				m_bStartContour_T = TRUE;

				if (nMode == SFM_WAIT)
				{
					if (bCheckMotionSync == TRUE)
					{
						if ((nResult = CMS896AStn::MotionSync(BH_AXIS_T, 20000, &m_stBHAxis_T)) != gnOK)
						{
							nResult = Err_BhTMove;
							m_bStartContour_T = FALSE;
						}
					}
					else
					{
						if ((nResult = CMS896AStn::MotionCommandPosSync(BH_AXIS_T, m_lCurPos_T, 500, &m_stBHAxis_T)) != gnOK)
						{
							nResult = Err_BhTMove;
							m_bStartContour_T = FALSE;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);

			m_bHome_T = FALSE;
			m_bStartContour_T = FALSE;
			nResult = Err_BhTMove;
		}
	}

	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "T_ContourMove fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::T_Sync()
{
	INT nResult = gnOK;
	return nResult;
	if (m_fHardware && m_bSel_T && !m_bDisableBH)
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhTMove;
			return nResult;
		}

		try
		{
			if (m_bStartContour_T == TRUE)
			{
				CMS896AStn::MotionCommandPosSync(BH_AXIS_T, m_lCurPos_T, 500, &m_stBHAxis_T);	

			}
			else
			{
				CMS896AStn::MotionSync(BH_AXIS_T, 1000, &m_stBHAxis_T);
			}

			m_bComplete_T = TRUE;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			nResult = gnNOTOK; 
		}	
	}
	// For disable BH T case
	if (!m_bSel_T)
	{
		if (!m_bDisableBH)			//v3.64
		{
			Sleep(m_lTime_T);
		}
		m_bComplete_T = TRUE;
	}

	//CheckResult(nResult, _T("BondArm Axis - Sync"));
	if ( nResult != gnOK )
	{
		SetAlert_Red_Yellow(IDS_BH_ARM_ILC_NOT_LEANRT);
		Ej_Home();
		T_PowerOn(FALSE);
		Z_PowerOn(FALSE);
		Z2_PowerOn(FALSE);
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "BHT_Sync fail");	//v4.59A19
	}
	return nResult;
}

INT CBondHead::T_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_T && !m_bDisableBH)	//v4.08
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_T, &m_stBHAxis_T) == FALSE )
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_T, &m_stBHAxis_T);
					m_bIsPowerOn_T = TRUE;
					EnableBHTThermalControl(TRUE);					//v4.49A5
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_T, &m_stBHAxis_T) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_T, &m_stBHAxis_T);
					m_bIsPowerOn_T = FALSE;
					m_bComm_T = FALSE;
					m_bHome_T = FALSE;
					EnableBHTThermalControl(FALSE);					//v4.49A5
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			nResult = gnNOTOK; 
		}	
	}
	CheckResult(nResult, _T("BondArm Axis - PowerOn"));
	return nResult;
}


INT CBondHead::T_Comm()
{
	return 1;
	INT nResult;

	if (m_fHardware && m_bSel_T && !m_bDisableBH)		//v4.08
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		SetMotionCE(FALSE, "reset in T_Comm");	//v4.59A19

		CMS896AApp::m_bBondHeadTThermalControl = FALSE;
		EnableBHTThermalControl(FALSE);					//v4.49A5

		//UBH
		if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_T, &m_stBHAxis_T) != 0)
		{
			m_bComm_T	= FALSE;
			CMS896AApp::m_bBondHeadTThermalControl = FALSE;
			CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);
		}

		switch(m_lT_CommMethod)
		{
			case MS896A_GEN_SWCOMM:					
				nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_T, &m_stBHAxis_T);
				break;

			case MS896A_DIR_SWCOMM:					
				nResult = CMS896AStn::MotionDirectionalCommutateServo(BH_AXIS_T, 8,8000, HP_NEGATIVE_DIR, &m_stBHAxis_T);
				break;

			case MS896A_OPEN_SWCOMM:
/*			
				if (CMS896AStn::MotionIsPositiveLimitHigh(BH_AXIS_T, &m_stBHAxis_T))
				{
					HmiMessage_Red_Yellow("BHT is at upper limit sensor position, can not do commutation");
					m_bComm_T = FALSE;
					m_bHome_T = FALSE;
					return gnNOTOK;
				}
*/
				nResult = CMS896AStn::MotionOpenCommutateServo(BH_AXIS_T, &m_stBHAxis_T, 16, 500000, 5000, 0, 1000, 3000, 2000, 5000, 0x500, -1, 14);
				break;
			
			default:
				nResult = gnOK;
				break;
		}

		if (nResult == gnOK)
		{
			m_bComm_T = TRUE;
			CMS896AApp::m_bBondHeadTThermalControl = TRUE;
			EnableBHTThermalControl(TRUE);					//v4.49A5
		}
		else
		{
			//UBH
			CString szTemp;
			szTemp.Format("    BHT: Commutate fail %d", m_lT_CommMethod);
			DisplayMessage(szTemp);

			nResult		= Err_BhTCommutate;
			//ClearServoError("srvBondHeadT");
			CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);
			m_bComm_T = TRUE;
		}
		CheckResult(nResult, _T("BondArm Axis - Commutate"));
	}
	return gnOK;
}

BOOL CBondHead::T_IsComplete()
{
	return TRUE;
	if (m_fHardware && m_bSel_T && !m_bDisableBH)	//v4.08
	{
		return CMS896AStn::MotionIsComplete(BH_AXIS_T, &m_stBHAxis_T);
	}
	else
	{
		return TRUE;
	}
}

BOOL CBondHead::T_IsContourMoveComplete(BOOL bDisableILCCleanLog)
{
	return TRUE;
	if (m_fHardware && m_bSel_T && !m_bDisableBH)	//v4.08
	{
		if (m_bStartContour_T == TRUE)
		{
			if (T_IsComplete() == FALSE)
			{
				// abort the current motion and clear the error if not complete
				CMS896AStn::MotionQuickStop(BH_AXIS_T, &m_stBHAxis_T);
				m_bStartContour_T = FALSE;
				//CMSLogFileUtility::Instance()->BH_ILCEventLog("MotionQuickStop");
				if (bDisableILCCleanLog == FALSE)	//v4.47T1	//MS60
				{
					ILC_CleanDataLog();
				}
				return TRUE;	
			}
		}
		
		// reset start contour move flag
		m_bStartContour_T = FALSE;
	}
	else
	{
		m_bStartContour_T = FALSE;
		return TRUE;
	}

	return TRUE;
}

BOOL CBondHead::T_SetContourProfile(FLOAT* fPos, FLOAT* fVel, FLOAT* fAcc, FLOAT* fJerk, UINT uiNoofPoint, UINT uiStartOffset)
{
	INT nResult = gnOK;
	
	try
	{
		MotionSetContourProfile(BH_AXIS_T, fPos, fVel, fAcc, fJerk, uiNoofPoint, uiStartOffset, &m_stBHAxis_T);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return gnNOTOK;
	}

	return gnOK;
}

BOOL CBondHead::T_IsPowerOn()
{
	//No BHT in Mega Da
	return TRUE;

	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBH)	//v3.61
	{
		return TRUE;
	}
	
	if( m_bSel_T==FALSE )
		return TRUE;

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_T, &m_stBHAxis_T);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}


////////////////////////////////////////////
//	Bond Head 
////////////////////////////////////////////
BOOL CBondHead::ZHomeSearch(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, const CString& szProfileID)
{
	return TRUE;
	CString szCurrControl	= CMS896AStn::MotionSelectControlParam(szAxis, BH_STATIC_CONTROL_OBW_Z, pAxisInfo);
	DWORD dwCurrType		= CMS896AStn::MotionSelectControlType(szAxis, 46, 1, pAxisInfo);
			
	//Search Positive Limit sensor 
	//SelectSearchProfile(BH_AXIS_Z, "spfBondHeadZIndex");

	CMS896AStn::MotionSearch(szAxis, 0, SFM_WAIT, pAxisInfo, szProfileID, FALSE, TRUE);
	
	CMS896AStn::MotionSelectControlParam(szAxis, szCurrControl, pAxisInfo);
	CMS896AStn::MotionSelectControlType(szAxis, dwCurrType, 2, pAxisInfo);

	BOOL bSearchZIndexOK = FALSE;
	if ( (CMS896AStn::MotionReportErrorStatus(szAxis, pAxisInfo) == 0) && 
		  CMS896AStn::MotionIsPowerOn(szAxis, pAxisInfo))
	{
		bSearchZIndexOK = TRUE;
	}

	return bSearchZIndexOK;
}

INT CBondHead::Z_Home()
{
	return 1;
	INT nResult			= gnOK;

	m_bHome_Z = FALSE;
	m_bComm_Z = FALSE;
	if (m_fHardware && m_bSel_Z && !m_bDisableBH)		//v4.08
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		SetMotionCE(FALSE, "reset in Z_Home");	//v4.59A19

		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_Z, &m_stBHAxis_Z) != 0)
			{
				m_bComm_Z	= FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);
			}

			if (!m_bComm_Z)
			{
				m_bHome_Z	= FALSE;

				if (m_lZ_CommMethod == MS896A_NO_SWCOMM)
				{
					nResult = gnOK;
				}
				else
				{
					nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_Z, &m_stBHAxis_Z);
				}

				if (nResult == gnOK)
				{
					m_bComm_Z = TRUE;
				}
				else
				{
					nResult		= Err_BhZCommutate;
					CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);
				}
			}

			if (nResult == gnOK)
			{
				if ( CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z))
				{
					//UBH only
					//CString szCurrControl	= CMS896AStn::MotionSelectControlParam(BH_AXIS_Z, BH_STATIC_CONTROL_OBW_Z, &m_stBHAxis_Z);
					//DWORD dwCurrType		= CMS896AStn::MotionSelectControlType(BH_AXIS_Z, HP_GFL_FFC_PID_CONTROL_ID, 2, &m_stBHAxis_Z);
					//CMS896AStn::MotionSetDacLimit(BH_AXIS_Z, 32766, 256, &m_stBHAxis_Z);	//UBH only

					if (!CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z))
					{
						CMS896AStn::MotionPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);
					}
					else
					{
						CMS896AStn::MotionPowerOff(BH_AXIS_Z, &m_stBHAxis_Z);
						Sleep(1000);
						CMS896AStn::MotionPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);
					}

					Sleep(100);
/*
					if ( m_lZ_ProfileType == MS896A_OBW_PROFILE )
					{
						Z_SelectObwProfile(2000);
						CMS896AStn::MotionObwMove(BH_AXIS_Z, 2000, SFM_WAIT, &m_stBHAxis_Z);
					}
					else
					{
						CMS896AStn::MotionMove(BH_AXIS_Z, 2000, SFM_WAIT, &m_stBHAxis_Z);
					}
					Sleep(500);
*/

					//Down Search Z index hole
					BOOL bSearchZIndexOK = ZHomeSearch(BH_AXIS_Z,  &m_stBHAxis_Z, "spfBondHeadZIndex");
					if (!bSearchZIndexOK)
					{
						//Upper Search Z index hole
						
//						CMS896AStn::MotionPowerOff(BH_AXIS_Z, &m_stBHAxis_Z);
						Sleep(1000);
						CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);
//						CMS896AStn::MotionPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);

						bSearchZIndexOK = ZHomeSearch(BH_AXIS_Z,  &m_stBHAxis_Z, "spfBondHeadZUpperIndex");
						if (bSearchZIndexOK)
						{
							m_bHome_Z = TRUE;
							nResult = Z_Move(2000);
							Sleep(100);

							bSearchZIndexOK = ZHomeSearch(BH_AXIS_Z,  &m_stBHAxis_Z, "spfBondHeadZIndex");
						}
					}

					if (bSearchZIndexOK)
					{
						Sleep(100);
						CMS896AStn::MotionSetPosition(BH_AXIS_Z, 0, &m_stBHAxis_Z);
						m_bHome_Z		= TRUE;
						m_lCurLevel_Z	= 0;

						//v4.46T10	//Cree HuiZhou PkgSort
						//v4.46T27
						CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
						if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
						{
							Sleep(100);
							Z_Move(pApp->GetBHZ1HomeOffset());
						}
					}
					else
					{
						DisplayMessage("    BHZ: Home fails");		//UBH
						nResult = Err_BhZMoveHome;
					}
				}
				else
				{
#ifdef NU_MOTION
/*
					if (!CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z))
						CMS896AStn::MotionPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);
					else
					{
						CMS896AStn::MotionPowerOff(BH_AXIS_Z, &m_stBHAxis_Z);
						Sleep(1000);
						CMS896AStn::MotionPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);
					}

					Sleep(100);

					CString szCurrControl	= CMS896AStn::MotionSelectControlParam(BH_AXIS_Z, BH_STATIC_CONTROL_OBW_Z, &m_stBHAxis_Z);
					DWORD dwCurrType		= CMS896AStn::MotionSelectControlType(BH_AXIS_Z, 46, 1, &m_stBHAxis_Z);
				
					//Search Positive Limit sensor 
					//SelectSearchProfile(BH_AXIS_Z, "spfBondHeadZIndex");
					CMS896AStn::MotionSearch(BH_AXIS_Z, 0, SFM_WAIT, &m_stBHAxis_Z, "spfBondHeadZIndex");
	
					CMS896AStn::MotionSelectControlParam(BH_AXIS_Z, szCurrControl, &m_stBHAxis_Z);
					CMS896AStn::MotionSelectControlType(BH_AXIS_Z, dwCurrType, 2, &m_stBHAxis_Z);

					if ( (CMS896AStn::MotionReportErrorStatus(BH_AXIS_Z, &m_stBHAxis_Z) == 0) && 
						  CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z))
					{
						Sleep(50);
						CMS896AStn::MotionSetPosition(BH_AXIS_Z, 0, &m_stBHAxis_Z);
						m_bHome_Z		= TRUE;
						m_lCurLevel_Z	= 0;
					}
*/
#else
					if ((nResult = CMS896AStn::MotionMoveHome(BH_AXIS_Z, 1, 0, &m_stBHAxis_Z)) == gnAMS_OK)
					{
						Sleep(50);
						CMS896AStn::MotionSetPosition(BH_AXIS_Z, 0, &m_stBHAxis_Z);
						m_bHome_Z		= TRUE;
						m_lCurLevel_Z	= 0;

						//v4.44A5	//Cree HuiZhou PkgSort
						CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
						//v4.46T27
						if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
						{
							Sleep(100);
							Z_Move(pApp->GetBHZ1HomeOffset());
						}

					}
					else if (nResult == gnNOTOK)
					{
						nResult = Err_BhZMoveHome;
					}
#endif
				}
			}
			m_bIsPowerOn_Z = CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);

			m_bComm_Z	= FALSE;
			nResult = Err_BhZMoveHome;
		}
	}
	else
	{
		if (m_bSel_Z)
		{
			Sleep(100);
			m_bComm_Z	= TRUE;
			m_bHome_Z	= TRUE;
		}
	}

	//CheckResult(nResult, _T("BondHead Axis - Home"));
	return nResult;
}



INT CBondHead::Z2_Home()
{
	return 1;
	INT nResult			= gnOK;

	m_bHome_Z2	= FALSE;
	m_bComm_Z2	= FALSE;
	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)		//v4.08
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		SetMotionCE(FALSE, "reset in Z2_Home");	//v4.59A19

		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_Z2, &m_stBHAxis_Z2) != 0)
			{
				m_bComm_Z2	= FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_Z2, &m_stBHAxis_Z2);
			}

			if (!m_bComm_Z2)
			{
				m_bHome_Z2	= FALSE;

				if (m_lZ_CommMethod == MS896A_NO_SWCOMM)
				{
					nResult = gnOK;
				}
				else
				{
					nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_Z2, &m_stBHAxis_Z2);
				}

				if (nResult == gnOK)
				{
					m_bComm_Z2 = TRUE;
				}
				else
				{
					nResult		= Err_BhZCommutate;
					CMS896AStn::MotionClearError(BH_AXIS_Z2, &m_stBHAxis_Z2);
				}
			}

			if (nResult == gnOK)
			{
				if ( CMS896AStn::MotionIsDcServo(BH_AXIS_Z2, &m_stBHAxis_Z2))
				{
					if (!CMS896AStn::MotionIsPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2))
						CMS896AStn::MotionPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2);
					else
					{
						CMS896AStn::MotionPowerOff(BH_AXIS_Z2, &m_stBHAxis_Z2);
						Sleep(1000);
						CMS896AStn::MotionPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2);
					}

					Sleep(100);

					//Down Search Z index hole
					BOOL bSearchZIndexOK = ZHomeSearch(BH_AXIS_Z2,  &m_stBHAxis_Z2, "spfBondHeadZIndex");
					if (!bSearchZIndexOK)
					{
						//Upper Search Z index hole
						
//						CMS896AStn::MotionPowerOff(BH_AXIS_Z2, &m_stBHAxis_Z2);
						Sleep(1000);
						CMS896AStn::MotionClearError(BH_AXIS_Z2, &m_stBHAxis_Z2);
//						CMS896AStn::MotionPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2);

						bSearchZIndexOK = ZHomeSearch(BH_AXIS_Z2,  &m_stBHAxis_Z2, "spfBondHeadZUpperIndex");
						if (bSearchZIndexOK)
						{
							m_bHome_Z2	= TRUE;
							nResult = Z2_Move(2000);
							Sleep(100);

							bSearchZIndexOK = ZHomeSearch(BH_AXIS_Z2,  &m_stBHAxis_Z2, "spfBondHeadZIndex");
						}
					}

					if (bSearchZIndexOK)
					{
						//Set Home position
						Sleep(50);
						CMS896AStn::MotionSetPosition(BH_AXIS_Z2, 0, &m_stBHAxis_Z2);
						m_bHome_Z2		= TRUE;
						m_lCurLevel_Z2	= 0;

						//v4.46T10	//Cree HuiZhou PkgSort
						CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
						//v4.46T27
						if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
						{
							Sleep(100);
							Z2_Move(pApp->GetBHZ2HomeOffset());
						}
					}
					else
					{
						DisplayMessage("    BHZ: Home fails");		//UBH
						nResult = Err_BhZMoveHome;
					}
				}
				else
				{
					if ((nResult = CMS896AStn::MotionMoveHome(BH_AXIS_Z2, 1, 0, &m_stBHAxis_Z2)) == gnAMS_OK)
					{
						Sleep(50);
						CMS896AStn::MotionSetPosition(BH_AXIS_Z2, 0, &m_stBHAxis_Z2);
						m_bHome_Z2		= TRUE;
						m_lCurLevel_Z2	= 0;

						//v4.44A5	//Cree HuiZhou PkgSort
						CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
						//v4.46T27
						if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
						{
							Sleep(100);
							Z2_Move(pApp->GetBHZ2HomeOffset());
						}
					}
					else if (nResult == gnNOTOK)
					{
						nResult = Err_BhZMoveHome;
					}

				}
			}
			m_bIsPowerOn_Z2 = CMS896AStn::MotionIsPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
			CMS896AStn::MotionClearError(BH_AXIS_Z2, &m_stBHAxis_Z2);

			m_bComm_Z2	= FALSE;
			nResult = Err_BhZMoveHome;
		}
	}
	else
	{
		if (m_bSel_Z2)
		{
			Sleep(100);
			m_bComm_Z2	= TRUE;
			m_bHome_Z2	= TRUE;
		}
	}

	//CheckResult(nResult, _T("BondHead Axis 2 - Home"));
	return nResult;
}

INT CBondHead::Z_MoveTo(INT nPos, INT nMode, BOOL bForceToMove, BOOL bToBond, short sProcListSaveID)
{
	INT nResult			= gnOK;
	return nResult;
	//v4.51A17
	//v4.51A6
	LONG lEncoder = nPos;
	//if (m_bMS60EjElevator && !bForceToMove)
	//	lEncoder = 0;

	if (m_fHardware && m_bSel_Z && !m_bDisableBH)	//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhZMove;
			return nResult;
		}

		//v3.62		//Use new special profile for BOND Z motion under SpeedMode
		// used by profile only
		BOOL bAtBond = FALSE;
		if ((CMS896AApp::m_lCycleSpeedMode > 0) && (lEncoder == m_lBondLevel_Z))
		{
			bAtBond = TRUE;
		}

		// used by control only
		BOOL bIsBondPos = FALSE;
		if ((m_lCurLevel_Z == m_lBondLevel_Z) || (lEncoder == m_lBondLevel_Z ) ||bToBond)
		{
			bIsBondPos = TRUE;
		}

		try
		{
			if (!m_bHome_Z)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( (m_lZ_ProfileType == MS896A_OBW_PROFILE) && (!CMS896AStn::MotionIsStepper(BH_AXIS_Z, &m_stBHAxis_Z)) )
				{
					Z_SelectObwProfile((LONG)(lEncoder - m_lCurLevel_Z), bAtBond);		//v3.62
#ifdef NU_MOTION
					Z_SelectControl((LONG)(lEncoder - m_lCurLevel_Z), bIsBondPos);
#endif
					CMS896AStn::MotionObwMoveTo(BH_AXIS_Z, lEncoder, SFM_NOWAIT, &m_stBHAxis_Z, sProcListSaveID);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BH_AXIS_Z, lEncoder, SFM_NOWAIT, &m_stBHAxis_Z, sProcListSaveID);
				}

				m_lCurLevel_Z	= lEncoder;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_Z, 20000, &m_stBHAxis_Z)) != gnOK)
					{
						nResult = Err_BhZMove;
					}
					m_bComplete_Z = TRUE;	//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);

			m_bHome_Z	= FALSE;
			nResult = Err_BhZMove;
		}
	}
	else
	{
		if (m_bSel_Z)
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

	//CheckResult(nResult, _T("BondHead Axis - MoveTo"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Z_MoveTo fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::Z2_MoveTo(INT nPos, INT nMode, BOOL bForceToMove, BOOL bToBond, short sProcListSaveID)
{
	INT nResult			= gnOK;
	return nResult;
	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)		//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhZMove;
			return nResult;
		}

		//v4.51A17
		//v4.51A6
		LONG lEncoder = nPos;
		//if (m_bMS60EjElevator && !bForceToMove)
		//	lEncoder = 0;

		try
		{
			if (!m_bHome_Z2)
			{
				nResult = gnNOTOK;
			}

			BOOL bIsBondPos = FALSE;

			if ((m_lCurLevel_Z2 == m_lBondLevel_Z2) || (lEncoder == m_lBondLevel_Z2)||bToBond)
			{
				bIsBondPos = TRUE;
			}

			if (nResult == gnOK)
			{
				if ( (m_lZ_ProfileType == MS896A_OBW_PROFILE) && (!CMS896AStn::MotionIsStepper(BH_AXIS_Z2, &m_stBHAxis_Z2)) )
				{
					Z2_SelectObwProfile((LONG)(lEncoder - m_lCurLevel_Z2), bIsBondPos);		//v4.36T1
#ifdef NU_MOTION
					Z2_SelectControl((LONG)(lEncoder - m_lCurLevel_Z2), bIsBondPos);	
#endif
					CMS896AStn::MotionObwMoveTo(BH_AXIS_Z2, lEncoder, SFM_NOWAIT, &m_stBHAxis_Z2, sProcListSaveID);
				}
				else
				{
					CMS896AStn::MotionMoveTo(BH_AXIS_Z2, lEncoder, SFM_NOWAIT, &m_stBHAxis_Z2, sProcListSaveID);
				}

				m_lCurLevel_Z2	= lEncoder;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_Z2, 20000, &m_stBHAxis_Z2)) != gnOK)
					{
						nResult = Err_BhZMove;
					}
					m_bComplete_Z2 = TRUE;	//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
			CMS896AStn::MotionClearError(BH_AXIS_Z2, &m_stBHAxis_Z2);

			m_bHome_Z2	= FALSE;
			nResult = Err_BhZMove;
		}
	}
	else
	{
		if (m_bSel_Z2)
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

	//CheckResult(nResult, _T("BondHead Axis 2 - MoveTo"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Z2_MoveTo fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::Z_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;
	return nResult;
	if (m_fHardware && m_bSel_Z && !m_bDisableBH)	//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhZMove;
			return nResult;
		}

		//v4.51A17
		//v4.51A6
		LONG lEncoder = nPos;
		//if (m_bMS60EjElevator)
		//	lEncoder = 0;

		try
		{
			if (!m_bHome_Z)
			{
				nResult = gnNOTOK;
			}

			// Use new special profile for BOND Z motion under SpeedMode
			if (nResult == gnOK)
			{
				if ( m_lZ_ProfileType == MS896A_OBW_PROFILE  && (!CMS896AStn::MotionIsStepper(BH_AXIS_Z, &m_stBHAxis_Z)) )
				{
					Z_SelectObwProfile((LONG)(lEncoder), FALSE);
#ifdef NU_MOTION
					Z_SelectControl((LONG)(lEncoder), FALSE);
#endif
					CMS896AStn::MotionObwMove(BH_AXIS_Z, lEncoder, SFM_NOWAIT, &m_stBHAxis_Z);
				}
				else
				{
					CMS896AStn::MotionMove(BH_AXIS_Z, lEncoder, SFM_NOWAIT, &m_stBHAxis_Z);
				}

				m_lCurLevel_Z	+= lEncoder;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_Z, 20000, &m_stBHAxis_Z)) != gnOK)
					{
						nResult = Err_BhZMove;
					}
					m_bComplete_Z = TRUE;	//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);

			m_bHome_Z	= FALSE;
			nResult = Err_BhZMove;
		}
	}
	else
	{
		if (m_bSel_Z)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(40);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	//CheckResult(nResult, _T("BondHead Axis - Move"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Z_Move fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::Z2_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;
	return nResult;
	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)		//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhZMove;
			return nResult;
		}

		//v4.51A17
		//v4.51A6
		LONG lEncoder = nPos;
		//if (m_bMS60EjElevator)
		//	lEncoder = 0;

		try
		{
			if (!m_bHome_Z2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if ( m_lZ_ProfileType == MS896A_OBW_PROFILE  && (!CMS896AStn::MotionIsStepper(BH_AXIS_Z2, &m_stBHAxis_Z2)) )
				{
					Z2_SelectObwProfile((LONG)(lEncoder));
#ifdef NU_MOTION
					Z2_SelectControl((LONG)(lEncoder), FALSE);	
#endif
					CMS896AStn::MotionObwMove(BH_AXIS_Z2, lEncoder, SFM_NOWAIT, &m_stBHAxis_Z2);
				}
				else
				{
					CMS896AStn::MotionMove(BH_AXIS_Z2, lEncoder, SFM_NOWAIT, &m_stBHAxis_Z2);
				}

				m_lCurLevel_Z2	+= lEncoder;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_Z2, 20000, &m_stBHAxis_Z2)) != gnOK)
					{
						nResult = Err_BhZMove;
					}
					m_bComplete_Z2 = TRUE;	//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
			CMS896AStn::MotionClearError(BH_AXIS_Z2, &m_stBHAxis_Z2);

			m_bHome_Z2	= FALSE;
			nResult = Err_BhZMove;
		}
	}
	else
	{
		if (m_bSel_Z2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(40);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	//CheckResult(nResult, _T("BondHead Axis 2 - Move"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Z2_Move fail");	//v4.59A19
	}

	return nResult;
}

INT CBondHead::Z_MoveToSearch(INT nPos, INT nMode, BOOL bToBond)
{
	return 1;
	INT nResult			= gnOK;
	INT nMoveToPosn	= 0;

	if (m_fHardware && m_bSel_Z && !m_bDisableBH)	//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhZMove;
			return nResult;
		}

		//v3.62		//Use new special profile for BOND Z motion under SpeedMode
		// used by profile only
		BOOL bAtBond = FALSE;
		if ((CMS896AApp::m_lCycleSpeedMode > 0))
		{
			bAtBond = bToBond;
		}

		try
		{
			if (!m_bHome_Z)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				DOUBLE dRes = GetChannelResolution(MS896A_CFG_CH_BONDHEAD)*1000.0;// M/Count ==> um/count
				if ( dRes == 0 )
				{
					dRes = 0.5;
				}	
				
				// Soft Touch Distance
				LONG lDistance = 0;
				if ( bToBond == TRUE )
				{
					lDistance = (LONG)(m_lSoftTouchBondDistance/dRes);
				}
				else
				{
					lDistance = (LONG)(m_lSoftTouchPickDistance/dRes);
				}				
				
				nMoveToPosn = nPos + lDistance;

				// Soft Touch Velocity
				DOUBLE dSearchV = 1.0;
				if ( m_lSoftTouchVelocity <= 0 )
				{
					//HmiMessage("The Soft Touch Velocity is ZERO!");
					//return gnNOTOK;
					m_lSoftTouchVelocity = 1;
				}
				else if ( m_lSoftTouchVelocity > 8 )
				{
					m_lSoftTouchVelocity = 8;
				}

				dSearchV = (DOUBLE)(m_lSoftTouchVelocity/dRes)/8.0;

				if ( m_lZ_ProfileType == MS896A_OBW_PROFILE )
				{
					Z_SelectObwProfile((LONG)(nMoveToPosn - m_lCurLevel_Z), bAtBond);		//	move to search
					Z_SelectControl((LONG)(nMoveToPosn - m_lCurLevel_Z), bToBond);
				}

				CMS896AStn::MotionUpdateSearchProfile(BH_AXIS_Z, BH_SP_SOFT_TOUCH_Z, dSearchV, lDistance, 0.0, 0, &m_stBHAxis_Z);
				CMS896AStn::MotionSelectSearchProfile(BH_AXIS_Z, BH_SP_SOFT_TOUCH_Z, &m_stBHAxis_Z);
				CMS896AStn::MotionMoveToSearch(BH_AXIS_Z, nMoveToPosn, SFM_NOWAIT, &m_stBHAxis_Z);
				
				m_lCurLevel_Z	= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_Z, 20000, &m_stBHAxis_Z)) != gnOK)
					{
						nResult = Err_BhZMove;
					}
					m_bComplete_Z = TRUE;	//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);

			m_bHome_Z	= FALSE;
			nResult = Err_BhZMove;
		}
	}
	else
	{
		if (m_bSel_Z)
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

	//CheckResult(nResult, _T("BondHead Axis - MoveTo"));
	if (nResult != gnOK)
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Z_MoveToSearch fail");	//v4.59A19
	}

	if (nResult == gnOK)
	{
		if (bToBond)
			m_lCurLevel_Z = m_lBondLevel_Z;		//move to search to bond, run time change, so need to modify
	}
	return nResult;
}

INT CBondHead::Z2_MoveToSearch(INT nPos, INT nMode, BOOL bToBond)
{
	return 1;
	INT nResult			= gnOK;
	INT nMoveToPosn	= 0;

	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)		//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhZMove;
			return nResult;
		}

		try
		{
			if (!m_bHome_Z2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
/*
				// Soft Touch Distance
				LONG lDistance = 0;
				if ( bIsBond == TRUE )
				{
					lDistance = m_lSoftTouchBondDistance;
				}
				else
				{
					lDistance = m_lSoftTouchPickDistance;
				}
				// Soft Touch Velocity
				DOUBLE dSearchV = 1.0;
				if ( m_lSoftTouchVelocity <= 0 )
				{
					//HmiMessage("The Soft Touch Velocity is ZERO!");
					//return gnNOTOK;
					m_lSoftTouchVelocity = 1;
				}
				else if ( m_lSoftTouchVelocity > 8 )
				{
					m_lSoftTouchVelocity = 8;
				}
				DOUBLE dRes = GetChannelResolution(MS896A_CFG_CH_BONDHEAD2)*1000.0;// M/Count ==> um/count
				if ( dRes == 0 )
				{
					dRes = 0.5;
				}
				dSearchV = (DOUBLE)(m_lSoftTouchVelocity/dRes)/8.0;

				nMoveDistance = nPos + lDistance;
*/
				DOUBLE dRes = GetChannelResolution(MS896A_CFG_CH_BONDHEAD2)*1000.0;// M/Count ==> um/count
				if ( dRes == 0 )
				{
					dRes = 0.5;
				}	
				
				// Soft Touch Distance
				LONG lDistance = 0;
				if ( bToBond == TRUE )
				{
					lDistance = (LONG)(m_lSoftTouchBondDistance/dRes);
				}
				else
				{
					lDistance = (LONG)(m_lSoftTouchPickDistance/dRes);
				}				
				
				nMoveToPosn = nPos + lDistance;

				// Soft Touch Velocity
				DOUBLE dSearchV = 1.0;
				if ( m_lSoftTouchVelocity <= 0 )
				{
					//HmiMessage("The Soft Touch Velocity is ZERO!");
					//return gnNOTOK;
					m_lSoftTouchVelocity = 1;
				}
				else if ( m_lSoftTouchVelocity > 8 )
				{
					m_lSoftTouchVelocity = 8;
				}

				dSearchV = (DOUBLE)(m_lSoftTouchVelocity/dRes)/8.0;

				if (m_lZ_ProfileType == MS896A_OBW_PROFILE)
				{
					Z2_SelectObwProfile((LONG)(nMoveToPosn - m_lCurLevel_Z2));	//	move to search
					Z2_SelectControl((LONG)(nMoveToPosn - m_lCurLevel_Z2), bToBond);
				}

				CMS896AStn::MotionUpdateSearchProfile(BH_AXIS_Z2, BH_SP_SOFT_TOUCH_Z, dSearchV, lDistance, 0.0, 0, &m_stBHAxis_Z2);
				CMS896AStn::MotionSelectSearchProfile(BH_AXIS_Z2, BH_SP_SOFT_TOUCH_Z, &m_stBHAxis_Z2);
				CMS896AStn::MotionMoveToSearch(BH_AXIS_Z2, nMoveToPosn, SFM_NOWAIT, &m_stBHAxis_Z2);

				m_lCurLevel_Z2	= nPos;	//	move to search

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_Z2, 20000, &m_stBHAxis_Z2)) != gnOK)
					{
						nResult = Err_BhZMove;
					}
					m_bComplete_Z2 = TRUE;	//v4.11T5
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
			CMS896AStn::MotionClearError(BH_AXIS_Z2, &m_stBHAxis_Z2);

			m_bHome_Z2	= FALSE;
			nResult = Err_BhZMove;
		}
	}
	else
	{
		if (m_bSel_Z2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(40);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	//CheckResult(nResult, _T("BondHead Axis 2 - Move"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Z2_MoveToSearch fail");	//v4.59A19
	}

	if( nResult==gnOK )
	{
		if( bToBond )
		{
			m_lCurLevel_Z2 = m_lBondLevel_Z2;	//	move to search to bond, run time change, so need to modify
		}
	}

	return nResult;
}

INT CBondHead::Z1_MoveToSearchBond(INT nPos, INT nMode)
{
	return 1;
	if ( IsMotionCE() == TRUE )
	{
		return Err_BhZMove;
	}

	INT nResult			= gnOK;
	try
	{
		if (!m_bHome_Z)
		{
			nResult = gnNOTOK;
		}
		else
		{
			DOUBLE dRes = GetChannelResolution(MS896A_CFG_CH_BONDHEAD)*1000.0;// M/Count ==> um/count
			if ( dRes == 0 )
			{
				dRes = 0.5;
			}	
			LONG lSrchDist = (LONG)(m_lSoftTouchBondDistance/dRes);	// Soft Touch Distance
			if ( m_lSoftTouchVelocity <= 0 )
			{
				m_lSoftTouchVelocity = 1;
			}
			else if ( m_lSoftTouchVelocity > 8 )
			{
				m_lSoftTouchVelocity = 8;
			}
			DOUBLE dSearchV = (DOUBLE)(m_lSoftTouchVelocity/dRes)/8.0;	// Soft Touch Velocity
			CMS896AStn::MotionUpdateSearchProfile(BH_AXIS_Z, BH_SP_SOFT_TOUCH_Z, dSearchV, 0-lSrchDist, 0.0, 0, &m_stBHAxis_Z);

			INT nMoveToBond = nPos + lSrchDist;
			Z_MoveTo(nMoveToBond, SFM_WAIT);
			Sleep(5);
			SetPickVacuum(FALSE);				//Else Turn off BHZ1 Vac on BOND side
			SetStrongBlow(TRUE);    //NEUTRAL state
			Sleep(m_lFloatBlowDelay);
			SetStrongBlow(FALSE);    //NEUTRAL state

			CMS896AStn::MotionSearch(BH_AXIS_Z, 1, SFM_NOWAIT, &m_stBHAxis_Z, BH_SP_SOFT_TOUCH_Z);

			m_lCurLevel_Z	= nPos;

			if (nMode == SFM_WAIT)
			{
				if ((nResult = CMS896AStn::MotionSync(BH_AXIS_Z, 20000, &m_stBHAxis_Z)) != gnOK)
				{
					nResult = Err_BhZMove;
				}
				m_bComplete_Z = TRUE;	//v4.11T5
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
		CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);

		m_bHome_Z	= FALSE;
		nResult = Err_BhZMove;
	}

	if (nResult != gnOK)
	{
		SetMotionCE(TRUE, "Z1_MoveToSearchBond fail");	//v4.59A19
	}
	else
	{
		m_lCurLevel_Z = m_lBondLevel_Z;		//move to search to bond, run time change, so need to modify
	}

	return nResult;
}

INT CBondHead::Z2_MoveToSearchBond(INT nPos, INT nMode)
{
	return 1;
	if ( IsMotionCE() == TRUE )
	{
		return Err_BhZMove;
	}

	INT nResult			= gnOK;
	try
	{
		if (!m_bHome_Z2)
		{
			nResult = gnNOTOK;
		}
		else
		{
			DOUBLE dRes = GetChannelResolution(MS896A_CFG_CH_BONDHEAD2)*1000.0;// M/Count ==> um/count
			if ( dRes == 0 )
			{
				dRes = 0.5;
			}
			// Soft Touch Distance
			LONG lSrchDist = (LONG)(m_lSoftTouchBondDistance/dRes);
			// Soft Touch Velocity
			DOUBLE dSearchV = 1.0;
			if ( m_lSoftTouchVelocity <= 0 )
			{
				m_lSoftTouchVelocity = 1;
			}
			else if ( m_lSoftTouchVelocity > 8 )
			{
				m_lSoftTouchVelocity = 8;
			}
			dSearchV = (DOUBLE)(m_lSoftTouchVelocity/dRes)/8.0;
			CMS896AStn::MotionUpdateSearchProfile(BH_AXIS_Z2, BH_SP_SOFT_TOUCH_Z, dSearchV, 0-lSrchDist, 0.0, 0, &m_stBHAxis_Z2);

			INT nMoveToBond = nPos + lSrchDist;
			Z2_MoveTo(nMoveToBond, SFM_WAIT);
			Sleep(5);
			SetPickVacuumZ2(FALSE);				//Turn off BHZ2 vac on BOND side
			SetStrongBlowZ2(TRUE);    //NEUTRAL state
			Sleep(m_lFloatBlowDelay);
			SetStrongBlowZ2(FALSE);    //NEUTRAL state

			CMS896AStn::MotionSearch(BH_AXIS_Z2, 1, SFM_NOWAIT, &m_stBHAxis_Z2, BH_SP_SOFT_TOUCH_Z);

			m_lCurLevel_Z2	= nPos;	//	move to search

			if (nMode == SFM_WAIT)
			{
				if ((nResult = CMS896AStn::MotionSync(BH_AXIS_Z2, 20000, &m_stBHAxis_Z2)) != gnOK)
				{
					nResult = Err_BhZMove;
				}
				m_bComplete_Z2 = TRUE;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
		CMS896AStn::MotionClearError(BH_AXIS_Z2, &m_stBHAxis_Z2);

		m_bHome_Z2	= FALSE;
		nResult = Err_BhZMove;
	}

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "Z2_MoveToSearchBond fail");
	}
	else
	{
		m_lCurLevel_Z2 = m_lBondLevel_Z2;	//	move to search to bond, run time change, so need to modify
	}

	return nResult;
}


INT CBondHead::Z_Sync()
{
	INT nResult = gnOK;
	return nResult;

	if (m_fHardware && m_bSel_Z && !m_bDisableBH)	//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhZMove;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(BH_AXIS_Z, 1000, &m_stBHAxis_Z);
			m_bComplete_Z = TRUE;
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			nResult = gnNOTOK;
		}	
	}
	// For disable BH Z case
	if (!m_bSel_Z)
	{
		if (!m_bDisableBH)		//v3.64
		{
			if ( m_lPickTime_Z > m_lBondTime_Z )
				Sleep(m_lPickTime_Z);
			else
				Sleep(m_lBondTime_Z);
		}
		m_bComplete_Z = TRUE;
	}

	//CheckResult(nResult, _T("BondHead Axis - Sync"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Z_Sync fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::Z2_Sync()
{
	INT nResult = gnOK;
	return nResult;

	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)		//v4.08
	{
		if ( IsMotionCE() == TRUE )
		{
			nResult = Err_BhZMove;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(BH_AXIS_Z2, 1000, &m_stBHAxis_Z2);
			m_bComplete_Z2 = TRUE;
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
			nResult = gnNOTOK;
		}	
	}
	// For disable BH Z case
	if (!m_bSel_Z2)
	{
		if ( m_lPickTime_Z2 > m_lBondTime_Z2 )
			Sleep(m_lPickTime_Z2);
		else
			Sleep(m_lBondTime_Z2);
		m_bComplete_Z2 = TRUE;
	}

	//CheckResult(nResult, _T("BondHead Axis 2 - Sync"));
	if ( nResult != gnOK )
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "Z2_Sync fail");	//v4.59A19
	}

	return nResult;
}


INT CBondHead::Z_PowerOn(BOOL bOn)
{
	return 1;
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Z && !m_bDisableBH)	//v4.08
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z) == FALSE )
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);
					m_bIsPowerOn_Z = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_Z, &m_stBHAxis_Z);
					m_bIsPowerOn_Z = FALSE;
					m_bComm_Z = FALSE;
					m_bHome_Z = FALSE;
				}
			}
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			nResult = gnNOTOK;
		}	
	}
	//CheckResult(nResult, _T("BondHead Axis - PowerOn"));
	return nResult;
}


INT CBondHead::Z2_PowerOn(BOOL bOn)
{
	return 1;
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)	//v4.08
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2) == FALSE )
				{
					CMS896AStn::MotionPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2);
					m_bIsPowerOn_Z2 = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BH_AXIS_Z2, &m_stBHAxis_Z2);
					m_bIsPowerOn_Z2 = FALSE;
					m_bComm_Z2 = FALSE;
					m_bHome_Z2 = FALSE;
				}
			}
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
			nResult = gnNOTOK;
		}	
	}
	//CheckResult(nResult, _T("BondHead Axis 2 - PowerOn"));
	return nResult;
}


INT CBondHead::Z_Comm()
{
	return 1;
	INT nResult;

	if (m_fHardware && m_bSel_Z && !m_bDisableBH)	//v4.08
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		SetMotionCE(FALSE, "reset in Z_Comm");	//v4.59A19

		if (m_lZ_CommMethod == MS896A_NO_SWCOMM)
		{
			nResult = gnOK;
		}
		else
		{
			//nResult = CommutateServo("BondHeadZAxis");
			nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_Z, &m_stBHAxis_Z);
		}

		if (nResult == gnOK)
		{
			m_bComm_Z = TRUE;
		}
		else
		{
			nResult		= Err_BhZCommutate;
			//ClearServoError("srvBondHeadZ");
			CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);
			m_bComm_Z = FALSE;
		}
		//CheckResult(nResult, _T("BondHead Axis - Commutate"));
	}
	return gnOK;
}


INT CBondHead::Z2_Comm()
{
	return 1;
	INT nResult;

	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)		//v4.08
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = FALSE;
		SetMotionCE(FALSE, "reset in Z2_Comm");	//v4.59A19

		if (m_lZ_CommMethod == MS896A_NO_SWCOMM)
		{
			nResult = gnOK;
		}
		else
		{
			nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_Z2, &m_stBHAxis_Z2);
		}

		if (nResult == gnOK)
		{
			m_bComm_Z2 = TRUE;
		}
		else
		{
			nResult		= Err_BhZCommutate;
			CMS896AStn::MotionClearError(BH_AXIS_Z2, &m_stBHAxis_Z2);
			m_bComm_Z2 = FALSE;
		}
		//CheckResult(nResult, _T("BondHead Axis 2 - Commutate"));
	}
	return gnOK;
}


BOOL CBondHead::Z_IsComplete()
{
	return TRUE;
	if (m_fHardware && m_bSel_Z && !m_bDisableBH)	//v4.08
	{
		return CMS896AStn::MotionIsComplete(BH_AXIS_Z, &m_stBHAxis_Z);
	}
	else
	{
		return TRUE;
	}
}


BOOL CBondHead::Z2_IsComplete()
{
	return TRUE;
	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)	//v4.08
	{
		return CMS896AStn::MotionIsComplete(BH_AXIS_Z2, &m_stBHAxis_Z2);
	}
	else
	{
		return TRUE;
	}
}


BOOL CBondHead::Z_IsPowerOn()
{
	//No BHZ in Mega Da
	return TRUE;

	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableBH)
		return TRUE;
	if( m_bSel_Z==FALSE )
		return TRUE;

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}


BOOL CBondHead::Z2_IsPowerOn()
{
	//No BHZ in Mega Da
	return TRUE;

	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableBH)
	{
		return TRUE;
	}
	
	if (!m_bIsArm2Exist)
	{
		return TRUE;
	}
	
	if( m_bSel_Z2==FALSE )
		return TRUE;

	try
	{
		return CMS896AStn::MotionIsPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

//Open DAC with velocity damping
INT CBondHead::Z_OpenDACwithVelDamping(LONG lDACValue, LONG lHoldTime)
{
	INT nResult = gnOK;
	return nResult;

	if ( m_bEnableBHZOpenDac == FALSE )
	{
		return nResult;
	}

	if (m_fHardware == FALSE)
		return nResult;
	if (m_bDisableBH)
		return nResult;

	if( m_bSel_Z==FALSE )
		return nResult;

	try
	{
		nResult = MotionOpenDACwithVelDamping(BH_AXIS_Z, &m_stBHAxis_Z, lDACValue, lHoldTime, MS896A_SW_OPENDAC_Z1);	//Klocwork
		if ( nResult != gnOK )
		{
			nResult = gnNOTOK;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
		nResult = gnNOTOK;
	}

	return nResult;
}

INT CBondHead::Z2_OpenDACwithVelDamping(LONG lDACValue, LONG lHoldTime)
{
	INT nResult = gnOK;
	return nResult;

	if ( m_bEnableBHZOpenDac == FALSE )
	{
		return nResult;
	}

	if (m_bSel_Z2 == FALSE)
	{
		return nResult;
	}

	if (m_fHardware == FALSE)
		return nResult;
	if (m_bDisableBH)
		return nResult;

	try
	{
		nResult = MotionOpenDACwithVelDamping(BH_AXIS_Z2, &m_stBHAxis_Z2, lDACValue, lHoldTime, MS896A_SW_OPENDAC_Z2);	//Klocwork
		if ( nResult != gnOK )
		{
			nResult = gnNOTOK;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
		nResult = gnNOTOK;
	}

	return nResult;
}

INT CBondHead::Z_SetOpenDACSwPort(BOOL bState)
{
	INT nResult = gnOK;
	return nResult;

	if ( m_bEnableBHZOpenDac == FALSE )
	{
		return nResult;
	}

	if (m_fHardware == FALSE)
		return nResult;
	if (m_bDisableBH)
		return nResult;

	if( m_bSel_Z==FALSE )
		return nResult;

	try
	{
		nResult = MotionWriteSoftwarePort(CMS896AApp::m_NmSWPort[MS896A_SW_OPENDAC_Z1].m_szName, 0, bState);	//Klocwork
		if ( nResult != gnOK )
		{
			nResult = gnNOTOK;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
		nResult = gnNOTOK;
	}

	return nResult;
}

INT CBondHead::Z2_SetOpenDACSwPort(BOOL bState)
{
	INT nResult = gnOK;
	return nResult;

	if ( m_bEnableBHZOpenDac == FALSE )
	{
		return nResult;
	}

	if (m_bSel_Z2 == FALSE)
	{
		return nResult;
	}

	if (m_fHardware == FALSE)
		return nResult;
	if (m_bDisableBH)
		return nResult;

	try
	{
		nResult = MotionWriteSoftwarePort(CMS896AApp::m_NmSWPort[MS896A_SW_OPENDAC_Z2].m_szName, 0, bState);	//Klocwork
		if ( nResult != gnOK )
		{
			nResult = gnNOTOK;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
		nResult = gnNOTOK;
	}

	return nResult;
}

INT CBondHead::Z2_MoveToHome()
{
	INT nResult			= gnOK;
	return nResult;

	if (Z2_IsPowerOn())	
	{
		nResult = Z2_MoveTo(0);
	}
	else
	{
		Sleep(500);
		SetErrorMessage("Z2_MoveToHome HOME");
		nResult = Z2_Home();
		if (!Z2_IsPowerOn() || !m_bHome_Z2)		//v4.46T21 //Retry once
		{
			SetErrorMessage("Z2_MoveToHome HOME retry");
			Sleep(1000);
			nResult = Z2_Home();
		}
	}

	//v4.46T10	//Cree HuiZhou PkgSort
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (	//(pApp->GetCustomerName() == "Cree") &&		//v4.47A5
			(pApp->GetBHZ2HomeOffset() > 0) &&
			(pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) && m_bHome_Z2 && Z2_IsPowerOn())		//v4.46T21
	{
		Sleep(100);
		if (!m_bMS60EjElevator)		//v4.51A6
			Z2_Move(pApp->GetBHZ2HomeOffset());
	}

	if (!Z2_IsPowerOn())
	{
		nResult = gnNOTOK;
	}

	return nResult;
}

INT CBondHead::Z_MoveToHome()
{
	INT nResult			= gnOK;
	return nResult;

	if( Z_IsPowerOn() )
	{
		nResult = Z_MoveTo(0);
	}
	else
	{
		Sleep(500);
		SetErrorMessage("Z_MoveToHome HOME");
		nResult = Z_Home();
		if (!Z_IsPowerOn() || !m_bHome_Z)		//v4.46T21 //Retry once
		{
			SetErrorMessage("Z_MoveToHome HOME retry");
			Sleep(1000);
			nResult = Z_Home();
		}
	}

	//v4.46T20
	//v4.46T10	//Cree HuiZhou PkgSort
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (	//(pApp->GetCustomerName() == "Cree") && 	//v4.47A5
			(pApp->GetBHZ1HomeOffset() > 0) &&
			(pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) && m_bHome_Z && Z_IsPowerOn())	//v4.46T21
	{
		Sleep(100);
		if (!m_bMS60EjElevator)		//v4.51A6
			Z_Move(pApp->GetBHZ1HomeOffset());
	}

	if (!Z_IsPowerOn())
	{
		nResult = gnNOTOK;
	}

	return nResult;
}

INT CBondHead::AC_Z1P_MoveToOrSearch(INT nPosn, INT nMode)
{
	INT nReturn = gnOK;
	return nReturn;

	if (m_lSoftTouchPickDistance == 0)
	{
		nReturn = AC_Z1_MoveTo(nPosn, nMode);
	}
	else
	{
		nReturn = Z_MoveToSearch(nPosn, nMode, FALSE);
	}

	return nReturn;
}

INT CBondHead::AC_Z1B_MoveToOrSearch(INT nPosn, INT nMode, short sProcListSaveID)
{
	INT nReturn = gnOK;
	return nReturn;

	if (m_lSoftTouchBondDistance == 0)
	{
		nReturn = AC_Z1_MoveTo(nPosn, nMode, TRUE, sProcListSaveID);
	}
	else
	{
		if( IsMS90() && m_bNeutralVacState && m_lFloatBlowDelay>0 && m_fHardware && !m_bDisableBH && m_bSel_Z )
			nReturn = Z1_MoveToSearchBond(nPosn, nMode);
		else
			nReturn = Z_MoveToSearch(nPosn, nMode, TRUE);
	}

	return nReturn;
}

INT CBondHead::AC_Z2P_MoveToOrSearch(INT nPosn, INT nMode)
{
	INT nReturn = gnOK;
	return nReturn;

	if (m_lSoftTouchPickDistance == 0)
	{
		nReturn = AC_Z2_MoveTo(nPosn, nMode);
	}
	else
	{
		nReturn = Z2_MoveToSearch(nPosn, nMode, FALSE);
	}

	return nReturn;
}


INT CBondHead::AC_Z2B_MoveToOrSearch(INT nPosn, INT nMode, short sProcListSaveID)
{
	INT nReturn = gnOK;
	return nReturn;

	if (m_lSoftTouchBondDistance == 0)
	{
		nReturn = AC_Z2_MoveTo(nPosn, nMode, TRUE, sProcListSaveID);
	}
	else
	{
		if( IsMS90() && m_bNeutralVacState && m_lFloatBlowDelay>0 && m_fHardware && !m_bDisableBH && m_bSel_Z2 )
		{
			nReturn = Z2_MoveToSearchBond(nPosn, nMode);
		}
		else
		{
			nReturn = Z2_MoveToSearch(nPosn, nMode, TRUE);
		}
	}

	return nReturn;
}

INT CBondHead::AC_Z1_MoveTo(INT nPos, INT nMode, BOOL bToBond, short sProcListSaveID)
{
	INT nResult = Z_MoveTo(nPos, nMode, bToBond, FALSE, sProcListSaveID);//Matt: for different control selection MS50

	if( nResult==gnOK && bToBond )
	{
		//v4.55A8
		m_lCurLevel_Z = m_lBondLevel_Z;		//	move to bond, run time change, so need to modify
	}

	return nResult;
}

INT CBondHead::AC_Z2_MoveTo(INT nPos, INT nMode, BOOL bToBond, short sProcListSaveID)
{
	INT nResult = Z2_MoveTo(nPos, nMode, bToBond, FALSE, sProcListSaveID);

	if( nResult==gnOK && bToBond )
	{
		m_lCurLevel_Z2 = m_lBondLevel_Z2;	//	move to bond, run time change, so need to modify
	}

	return nResult;
}

