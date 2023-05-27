/////////////////////////////////////////////////////////////////
// WL_Move.cpp : Move functions of the CWaferLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			BarryChu
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	X Axis 
////////////////////////////////////////////

BOOL CWaferLoader::X_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}

	if (m_bDisableWLWithExp == TRUE)	//STD machine with Expander
	{
		return TRUE;
	}
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(WL_AXIS_X, &m_stWLAxis_X);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CWaferLoader::X2_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWL)
	{
		return TRUE;
	}

	if( m_bSel_X2==FALSE )
		return TRUE;

	if (m_bDisableWLWithExp == TRUE)	//STD machine with Expander
	{
		return TRUE;
	}
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(WL_AXIS_X2, &m_stWLAxis_X2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}


//V2.83T4 (With Sync/ Without Sync Choice)
INT CWaferLoader::X_Home(INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(WL_AXIS_X, &m_stWLAxis_X) != 0)
			{
				m_bComm_X = FALSE;
				CMS896AStn::MotionClearError(WL_AXIS_X, &m_stWLAxis_X);
			}

			if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)	//v3.59
			{
				if (!m_bComm_X)
				{
					m_bHome_X = FALSE;

					if ((nResult = CMS896AStn::MotionCommutateServo(WL_AXIS_X, &m_stWLAxis_X)) == gnOK)
					{
						m_bComm_X = TRUE;
					}
					else
					{
						nResult	= Err_WaferGripperCommutate;
						CMS896AStn::MotionClearError(WL_AXIS_X, &m_stWLAxis_X);
					}
				}
			}
			else
			{
				m_bComm_X = TRUE;	//No need to commutate for stepper motor
			}

			m_bHome_X = FALSE;
			if (nResult == gnOK)
			{
				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionMoveHome(WL_AXIS_X, 1, 0, &m_stWLAxis_X)) == gnAMS_OK)
					{
						Sleep(200);
						if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)
						{
							Sleep(100);
							CMS896AStn::MotionMove(WL_AXIS_X, WL_GRIPPER_SRV_HOME_OFFSET_POS, SFM_WAIT, &m_stWLAxis_X);	//v3.66T1
							Sleep(100);
						}

						CMS896AStn::MotionSetPosition(WL_AXIS_X, 0, &m_stWLAxis_X);
						m_bHome_X	= TRUE;
					}
					else
					{
						nResult = Err_WaferGripperMoveHome; 
					}
				}
				else
				{
					nResult = CMS896AStn::MotionMoveHome(WL_AXIS_X, SFM_NOWAIT, 0, &m_stWLAxis_X);

					if (nResult == gnNOTOK)
					{
						nResult = Err_WaferGripperMoveHome; 
					}
					else
					{
						m_bHome_X = TRUE;
					}

				}

			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_X, &m_stWLAxis_X);
			CMS896AStn::MotionClearError(WL_AXIS_X, &m_stWLAxis_X);
			nResult = Err_WaferGripperMoveHome; 
			m_bHome_X = FALSE;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			Sleep(100);
			m_bHome_X	= TRUE;
			m_bComm_X	= TRUE;		//v3.59
		}
	}

	CheckResult(nResult, _T("X Axis - Home"));
	return nResult;
}

INT CWaferLoader::X2_Home(INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_X2)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(WL_AXIS_X2, &m_stWLAxis_X2) != 0)
			{
				m_bComm_X2 = FALSE;
				CMS896AStn::MotionClearError(WL_AXIS_X2, &m_stWLAxis_X2);
			}

			if (CMS896AStn::MotionIsServo(WL_AXIS_X2, &m_stWLAxis_X2) == TRUE)
			{
				if (!m_bComm_X2)
				{
					m_bHome_X2 = FALSE;

					if ((nResult = CMS896AStn::MotionCommutateServo(WL_AXIS_X2, &m_stWLAxis_X2)) == gnOK)
					{
						m_bComm_X2 = TRUE;
					}
					else
					{
						nResult	= Err_WaferGripperCommutate;
						CMS896AStn::MotionClearError(WL_AXIS_X2, &m_stWLAxis_X2);
					}
				}
			}
			else
			{
				m_bComm_X2 = TRUE;	//No need to commutate for stepper motor
			}

			m_bHome_X2 = FALSE;
			if (nResult == gnOK)
			{
				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionMoveHome(WL_AXIS_X2, 1, 0, &m_stWLAxis_X2)) == gnAMS_OK)
					{
						Sleep(200);
						if (CMS896AStn::MotionIsServo(WL_AXIS_X2, &m_stWLAxis_X2) == TRUE)
						{
							Sleep(100);
							CMS896AStn::MotionMove(WL_AXIS_X2, WL_GRIPPER_SRV_HOME_OFFSET_POS, SFM_WAIT, &m_stWLAxis_X2);	//v3.66T1
							Sleep(100);
						}

						CMS896AStn::MotionSetPosition(WL_AXIS_X2, 0, &m_stWLAxis_X2);
						m_bHome_X2	= TRUE;
					}
				}
				else
				{
					nResult = CMS896AStn::MotionMoveHome(WL_AXIS_X2, SFM_NOWAIT, 0, &m_stWLAxis_X2);

					if (nResult == gnNOTOK)
					{
						nResult = Err_WaferGripperMoveHome; 
					}
					else
					{
						m_bHome_X2 = TRUE;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_X2, &m_stWLAxis_X2);
			CMS896AStn::MotionClearError(WL_AXIS_X2, &m_stWLAxis_X2);
			nResult = Err_WaferGripperMoveHome; 
			m_bHome_X2 = FALSE;
		}
	}
	else
	{
		if (m_bSel_X2)
		{
			Sleep(100);
			m_bHome_X2	= TRUE;
			m_bComm_X2	= TRUE;
		}
	}

	return nResult;
}

INT CWaferLoader::X_MoveTo_Auto(INT nPos, INT nMode, BOOL bFrameDetect)
{
	INT nResult	= gnOK;

	nResult = X_MoveTo(nPos, SFM_NOWAIT);

	if (nMode == SFM_WAIT)
	{
		if ((nResult != gnOK) && (nResult != TRUE))
		{
			HmiMessage_Red_Back("WL Gripper move motion error", "Wafer Loader");
			return gnNOTOK;
		}

		while (1)
		{
			if (!X_IsPowerOn())
			{
				HmiMessage_Red_Back("WL Gripper move lose power", "Wafer Loader");
				return gnNOTOK;
			}

			if ((CMS896AStn::MotionIsComplete(WL_AXIS_X, &m_stWLAxis_X) == TRUE))
			{
				X_Sync();
				if( IsGripperMissingSteps() )
				{
					return gnNOTOK;
				}
				return gnOK;
			}

			if (bFrameDetect)	//v4.50A21
			{
				if (!IsFrameDetect())
				{
					Sleep(50);
					if (!IsFrameDetect())
					{
						CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
						X_Sync();
						HmiMessage_Red_Back("WL Gripper no frame detect", "Wafer Loader");
						return gnNOTOK;
					}
				}
			}

			if (IsCoverOpen() == TRUE)
			{
				CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
				X_Sync();
				HmiMessage_Red_Back("WL Gripper cover open", "Wafer Loader");
				return gnNOTOK;
			}

			if (IsFrameJam() == TRUE)
			{
				CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
				X_Sync();
				HmiMessage_Red_Back("WL Gripper frame jam", "Wafer Loader");
				return gnNOTOK;
			}

			Sleep(10);
		}
	}

	return nResult;
}

INT CWaferLoader::X_MoveTo(INT nPos, INT nMode)
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
			
			/*
			if (nResult == gnOK)
			{
				m_pStepper_X->MoveTo(nPos, SFM_NOWAIT);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = SyncStepper(m_pStepper_X, 10000)) != gnOK)
					{
						nResult = Err_WaferGripperMove;
					}
				}
			}
			*/

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMoveTo(WL_AXIS_X, nPos, SFM_NOWAIT, &m_stWLAxis_X);

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(WL_AXIS_X, 10000, &m_stWLAxis_X);		//Klocwork
					if (nResult != gnOK)
					{
						nResult = Err_WaferGripperMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_X, &m_stWLAxis_X);
			CMS896AStn::MotionClearError(WL_AXIS_X, &m_stWLAxis_X);

			m_bComm_X = FALSE;
			m_bHome_X = FALSE;
			nResult = Err_WaferGripperMove;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(100);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	//CheckResult(nResult, _T("X Axis - MoveTo"));
	return nResult;
}

INT CWaferLoader::X2_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

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
				CMS896AStn::MotionMoveTo(WL_AXIS_X2, nPos, SFM_NOWAIT, &m_stWLAxis_X2);

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(WL_AXIS_X2, 10000, &m_stWLAxis_X2);	
					if (nResult != gnOK)
					{
						nResult = Err_WaferGripperMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_X2, &m_stWLAxis_X2);
			CMS896AStn::MotionClearError(WL_AXIS_X2, &m_stWLAxis_X2);

			m_bComm_X2 = FALSE;
			m_bHome_X2 = FALSE;
			nResult = Err_WaferGripperMove;
		}
	}
	else
	{
		if (m_bSel_X2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(100);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}

INT CWaferLoader::X_Sync()
{
	if (m_fHardware && m_bSel_X)
	{
		CMS896AStn::MotionSync(WL_AXIS_X, 10000, &m_stWLAxis_X);
	}
	return gnOK;
}

INT CWaferLoader::X_Sync_Auto()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_X)
	{
		//CMS896AStn::MotionSync(WL_AXIS_X, 10000, &m_stWLAxis_X);
		while (1)
		{
			if (!X_IsPowerOn())
			{
				return FALSE;
			}

			if ((CMS896AStn::MotionIsComplete(WL_AXIS_X, &m_stWLAxis_X) == TRUE))
			{
				X_Sync();
				break;
			}

			if (IsCoverOpen() == TRUE)
			{
				CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
				X_Sync();
				return FALSE;
			}

			if (IsFrameJam() == TRUE)
			{
				CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
				X_Sync();
				return FALSE;
			}

			Sleep(10);
		}

	}
	return nResult;
}

INT CWaferLoader::X2_Sync()
{
	if (m_fHardware && m_bSel_X2)
	{
		CMS896AStn::MotionSync(WL_AXIS_X2, 10000, &m_stWLAxis_X2);
	}
	return gnOK;
}


//Relative Movement On X
INT CWaferLoader::X_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		try
		{
			if (!m_bHome_X)
			{
				nResult	= X_Home();
			}

			/*
			if (nResult == gnOK)
			{
				m_pStepper_X->Move(nPos, SFM_NOWAIT);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = SyncStepper(m_pStepper_X, 10000)) != gnOK)
					{
						nResult = Err_WaferGripperMove;
					}
				}
			}
			*/

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMove(WL_AXIS_X, nPos, SFM_NOWAIT, &m_stWLAxis_X);
				
				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(WL_AXIS_X, 10000, &m_stWLAxis_X);		//Klocwork
					if (nResult != gnOK)
					{
						nResult = Err_WaferGripperMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_X, &m_stWLAxis_X);
			CMS896AStn::MotionClearError(WL_AXIS_X, &m_stWLAxis_X);

			m_bHome_X = FALSE;
			nResult = Err_WaferGripperMove;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(100);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	return nResult;
}

INT CWaferLoader::X2_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_X2)
	{
		try
		{
			if (!m_bHome_X2)
			{
				nResult	= X2_Home();
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMove(WL_AXIS_X2, nPos, SFM_NOWAIT, &m_stWLAxis_X2);
				
				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(WL_AXIS_X2, 10000, &m_stWLAxis_X2);		//Klocwork
					if (nResult != gnOK)
					{
						nResult = Err_WaferGripperMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_X2, &m_stWLAxis_X2);
			CMS896AStn::MotionClearError(WL_AXIS_X2, &m_stWLAxis_X2);

			m_bHome_X2 = FALSE;
			nResult = Err_WaferGripperMove;
		}
	}
	else
	{
		if (m_bSel_X2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(100);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}


////////////////////////////////////////////
//	Z Axis 
////////////////////////////////////////////
BOOL CWaferLoader::Z_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}

	if (m_bDisableWLWithExp == TRUE)	//STD machine with Expander
	{
		return TRUE;
	}
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(WL_AXIS_Z, &m_stWLAxis_Z);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CWaferLoader::Z2_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWL)
	{
		return TRUE;
	}

	if (m_bDisableWLWithExp == TRUE)	//STD machine with Expander
	{
		return TRUE;
	}
	if ( IsESDualWL()==FALSE )
		return TRUE;

	try
	{
		return CMS896AStn::MotionIsPowerOn(WL_AXIS_Z2, &m_stWLAxis_Z2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}


INT CWaferLoader::Z_Home()
{
	INT nResult	= gnOK;
	int nCommRetry = 0;


	if (m_fHardware && m_bSel_Z)
	{
		try
		{
#ifdef NU_MOTION
			//Enlarge the softawre limit before Home
			CMS896AStn::MotionSetSoftwareLimit(WL_AXIS_Z, 99999999, -99999999, &m_stWLAxis_Z);
#endif

			if (CMS896AStn::MotionReportErrorStatus(WL_AXIS_Z, &m_stWLAxis_Z) != 0)
			{
				m_bComm_Z = FALSE;		//v4.24		//ES101
				CMS896AStn::MotionClearError(WL_AXIS_Z, &m_stWLAxis_Z);
			}

			m_bHome_Z = FALSE;
			if (CMS896AStn::MotionIsServo(WL_AXIS_Z, &m_stWLAxis_Z) == TRUE)
			{
				if (!m_bComm_Z)
				{

					SHORT siOrgDACLimit = CMS896AStn::MotionGetDacLimit(WL_AXIS_Z, &m_stWLAxis_Z);
					SHORT siCurrentDAC = CMS896AStn::MotionGetDacLimit(WL_AXIS_Z, &m_stWLAxis_Z);

					while(nCommRetry < 3)
					{
						nResult = MotionDirectionalCommutateServo(WL_AXIS_Z, 8, 8000, HP_POSITIVE_DIR, &m_stWLAxis_Z);

						if ((nResult == gnOK) &&
							(CMS896AStn::MotionReportErrorStatus(WL_AXIS_Z, &m_stWLAxis_Z) == 0) )
						{
							m_bComm_Z = TRUE;
							break;
						}
						else
						{
							siCurrentDAC = (SHORT)(siCurrentDAC * 1.15);
							CMS896AStn::MotionSetDacLimit(WL_AXIS_Z, siCurrentDAC, 
								CMS896AStn::MotionGetDacTimeLimit(WL_AXIS_Z, &m_stWLAxis_Z), 
								&m_stWLAxis_Z);
						}
						
						DisplayMessage("Retry sw comm on WaferLoader Z");
						nCommRetry++;
						Sleep(300);
					}
					
					if ( nResult == gnOK )
					{
						//Restore pre-set DAC limit
						CMS896AStn::MotionSetDacLimit(WL_AXIS_Z, siOrgDACLimit, CMS896AStn::MotionGetDacTimeLimit(WL_AXIS_Z, &m_stWLAxis_Z), &m_stWLAxis_Z);
						m_bComm_Z = TRUE;
					}
					else
					{
						nResult	= Err_WaferLoaderZMoveHome;
						CMS896AStn::MotionClearError(WL_AXIS_Z, &m_stWLAxis_Z);
					}
				}
			}
			else
			{
				m_bComm_Z = TRUE;
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(WL_AXIS_Z, 1, 0, &m_stWLAxis_Z);
				if (nResult == gnAMS_OK)
				{
					if (CMS896AStn::MotionIsServo(WL_AXIS_Z, &m_stWLAxis_Z) == TRUE)	//ES101		//v4.24
					{
						CMS896AStn::MotionPowerOn(WL_AXIS_Z, &m_stWLAxis_Z);

						CMS896AStn::MotionSelectSearchProfile(WL_AXIS_Z, "spfWaferLoaderZIndex", &m_stWLAxis_Z);
						CMS896AStn::MotionSearch(WL_AXIS_Z, 0, SFM_WAIT, &m_stWLAxis_Z, "spfWaferLoaderZIndex");

						if ((CMS896AStn::MotionReportErrorStatus(WL_AXIS_Z, &m_stWLAxis_Z) == 0) )
						{
							Sleep(3000);
							CMS896AStn::MotionSetPosition(WL_AXIS_Z, 0, &m_stWLAxis_Z);
							m_bHome_Z = TRUE;
						}
					}
					else
					{
						Sleep(100);
						CMS896AStn::MotionSetPosition(WL_AXIS_Z, 0, &m_stWLAxis_Z);
						m_bHome_Z	= TRUE;
					}

#ifdef NU_MOTION
					Z_Sync();		//v4.46T4	//Chris Keung
					if (CMS896AApp::m_bMS100Plus9InchOption)			//v4.21T3	//MS100 9Inch	
					{
						LONG lUpLimit	= GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);
						LONG lLowLimit	= GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);
						CMS896AStn::MotionSetSoftwareLimit(WL_AXIS_Z, lUpLimit, lLowLimit, &m_stWLAxis_Z);
					}
					else if (CMS896AApp::m_bES100v2DualWftOption)		//v4.24		//ES101
					{
						LONG lUpLimit	= GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);
						LONG lLowLimit	= GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);
						CMS896AStn::MotionSetSoftwareLimit(WL_AXIS_Z, lUpLimit, lLowLimit, &m_stWLAxis_Z);
					}
					else
					{
						CMS896AStn::MotionSetSoftwareLimit(WL_AXIS_Z, WL_Z_POS_LIMIT_MS100, WL_Z_NEG_LIMIT_MS100, &m_stWLAxis_Z);
					}

					CString szLog;
					szLog.Format("WLZ limit: %d, %d", WL_Z_POS_LIMIT_MS100, WL_Z_NEG_LIMIT_MS100);
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
#endif
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_WaferLoaderZMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_Z, &m_stWLAxis_Z);
			CMS896AStn::MotionClearError(WL_AXIS_Z, &m_stWLAxis_Z);
			nResult = Err_WaferLoaderZMoveHome; 
			m_bHome_Z	= FALSE;	// EXCEPTION
		}
	}
	else
	{
		if (m_bSel_Z)
		{
			Sleep(100);
			m_bHome_Z	= TRUE;
		}
	}

	Z_MoveTo(m_lTopSlotLevel_Z);//Matt: go to top level after home()

	return nResult;
}

INT CWaferLoader::Z2_Home()
{
	INT nResult	= gnOK;
	int nCommRetry = 0;


	if (m_fHardware && m_bSel_Z2)
	{
		try
		{
			//Enlarge the softawre limit before Home
			CMS896AStn::MotionSetSoftwareLimit(WL_AXIS_Z2, 99999999, -99999999, &m_stWLAxis_Z2);

			if (CMS896AStn::MotionReportErrorStatus(WL_AXIS_Z2, &m_stWLAxis_Z2) != 0)
			{
				m_bComm_Z2 = FALSE;	
				CMS896AStn::MotionClearError(WL_AXIS_Z2, &m_stWLAxis_Z2);
			}

			if (CMS896AStn::MotionIsServo(WL_AXIS_Z2, &m_stWLAxis_Z2) == TRUE)
			{
				if (!m_bComm_Z2)
				{
					m_bHome_Z2 = FALSE;

					SHORT siOrgDACLimit = CMS896AStn::MotionGetDacLimit(WL_AXIS_Z2, &m_stWLAxis_Z2);
					SHORT siCurrentDAC = CMS896AStn::MotionGetDacLimit(WL_AXIS_Z2, &m_stWLAxis_Z2);

					while(nCommRetry < 3)
					{
						nResult = MotionDirectionalCommutateServo(WL_AXIS_Z2, 8, 8000, HP_POSITIVE_DIR, &m_stWLAxis_Z2);

						if ((nResult == gnOK) &&
							(CMS896AStn::MotionReportErrorStatus(WL_AXIS_Z2, &m_stWLAxis_Z2) == 0) )
						{
							m_bComm_Z2 = TRUE;
							break;
						}
						else
						{
							siCurrentDAC = (SHORT)(siCurrentDAC * 1.15);
							CMS896AStn::MotionSetDacLimit(WL_AXIS_Z2, siCurrentDAC, 
								CMS896AStn::MotionGetDacTimeLimit(WL_AXIS_Z2, &m_stWLAxis_Z2), 
								&m_stWLAxis_Z2);
						}
						
						DisplayMessage("Retry sw comm on WaferLoader Z2");
						nCommRetry++;
						Sleep(300);
					}
					
					if ( nResult == gnOK )
					{
						//Restore pre-set DAC limit
						CMS896AStn::MotionSetDacLimit(WL_AXIS_Z2, siOrgDACLimit, CMS896AStn::MotionGetDacTimeLimit(WL_AXIS_Z2, &m_stWLAxis_Z2), &m_stWLAxis_Z2);
						m_bComm_Z2 = TRUE;
					}
					else
					{
						nResult	= Err_WaferLoaderZMoveHome;
						CMS896AStn::MotionClearError(WL_AXIS_Z2, &m_stWLAxis_Z2);
					}
				}
			}
			else
			{
				m_bComm_Z2 = TRUE;
			}


			m_bHome_Z2 = FALSE;
			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(WL_AXIS_Z2, 1, 0, &m_stWLAxis_Z2);
				if (nResult == gnAMS_OK)
				{
					if (CMS896AStn::MotionIsServo(WL_AXIS_Z2, &m_stWLAxis_Z2) == TRUE)
					{
						CMS896AStn::MotionPowerOn(WL_AXIS_Z2, &m_stWLAxis_Z2);

						CMS896AStn::MotionSelectSearchProfile(WL_AXIS_Z2, "spfWaferLoaderZIndex", &m_stWLAxis_Z2);
						CMS896AStn::MotionSearch(WL_AXIS_Z2, 0, SFM_WAIT, &m_stWLAxis_Z2, "spfWaferLoaderZIndex");
						if ((CMS896AStn::MotionReportErrorStatus(WL_AXIS_Z2, &m_stWLAxis_Z2) == 0) )
						{
							Sleep(3000);
							CMS896AStn::MotionSetPosition(WL_AXIS_Z2, 0, &m_stWLAxis_Z2);
							m_bHome_Z2 = TRUE;
						}
					}
					else
					{
						Sleep(100);
						CMS896AStn::MotionSetPosition(WL_AXIS_Z2, 0, &m_stWLAxis_Z2);
						m_bHome_Z2	= TRUE;
					}

					Z2_Sync();		//v4.46T4	//Chris Keung

					if (CMS896AApp::m_bES100v2DualWftOption)	
					{
						LONG lUpLimit	= GetChannelInformation(MS896A_CFG_CH_WAFLOADER2_Z, MS896A_CFG_CH_MAX_DISTANCE);
						LONG lLowLimit	= GetChannelInformation(MS896A_CFG_CH_WAFLOADER2_Z, MS896A_CFG_CH_MIN_DISTANCE);
						CMS896AStn::MotionSetSoftwareLimit(WL_AXIS_Z2, lUpLimit, lLowLimit, &m_stWLAxis_Z2);
					}
					else
					{
						CMS896AStn::MotionSetSoftwareLimit(WL_AXIS_Z2, WL_Z_POS_LIMIT_MS100, WL_Z_NEG_LIMIT_MS100, &m_stWLAxis_Z);
					}

					CString szLog;
					szLog.Format("WLZ2 limit: %d, %d", WL_Z_POS_LIMIT_MS100, WL_Z_NEG_LIMIT_MS100);
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_WaferLoaderZMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_Z2, &m_stWLAxis_Z2);
			CMS896AStn::MotionClearError(WL_AXIS_Z2, &m_stWLAxis_Z2);
			nResult = Err_WaferLoaderZMoveHome; 
			m_bHome_Z2 = FALSE;
		}
	}
	else
	{
		if (m_bSel_Z2)
		{
			Sleep(100);
			m_bHome_Z2	= TRUE;
		}
	}

	return nResult;
}


INT CWaferLoader::Z_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Z)
	{
		try
		{
			if (!m_bHome_Z)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				//m_pStepper_Z->MoveTo(nPos, SFM_NOWAIT);
				CMS896AStn::MotionMoveTo(WL_AXIS_Z, nPos, SFM_NOWAIT, &m_stWLAxis_Z);

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(WL_AXIS_Z, 20000, &m_stWLAxis_Z);
					if (nResult != gnOK)
					{
						nResult = Err_WaferLoaderZMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_Z, &m_stWLAxis_Z);
			CMS896AStn::MotionClearError(WL_AXIS_Z, &m_stWLAxis_Z);
			m_bHome_Z = FALSE;
			nResult = Err_WaferLoaderZMove;
		}
	}
	else
	{
		if (m_bSel_Z)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(100);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	//CheckResult(nResult, _T("Z Axis - MoveTo"));
	return nResult;
}

INT CWaferLoader::Z2_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Z2)
	{
		try
		{
			if (!m_bHome_Z2)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMoveTo(WL_AXIS_Z2, nPos, SFM_NOWAIT, &m_stWLAxis_Z2);

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(WL_AXIS_Z2, 20000, &m_stWLAxis_Z2);
					if (nResult != gnOK)
					{
						nResult = Err_WaferLoaderZMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WL_AXIS_Z2, &m_stWLAxis_Z2);
			CMS896AStn::MotionClearError(WL_AXIS_Z2, &m_stWLAxis_Z2);
			m_bHome_Z2 = FALSE;
			nResult = Err_WaferLoaderZMove;
		}
	}
	else
	{
		if (m_bSel_Z2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(100);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	return nResult;
}


INT CWaferLoader::Z_Sync()
{
	if (m_fHardware && m_bSel_Z)
	{
		//m_pStepper_Z->Synchronize(10000);
		CMS896AStn::MotionSync(WL_AXIS_Z, 20000, &m_stWLAxis_Z);
	}

	return gnOK;
}

INT CWaferLoader::Z2_Sync()
{
	if (m_fHardware && m_bSel_Z2)
	{
		CMS896AStn::MotionSync(WL_AXIS_Z2, 20000, &m_stWLAxis_Z2);
	}

	return gnOK;
}

