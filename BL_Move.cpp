/////////////////////////////////////////////////////////////////
// BL_Move.cpp : Move functions of the CBinLoader class
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
#include "BinLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

INT CBinLoader::X_Home()
{
	INT nResult			= gnOK;

	//v4.59A40	//MS50
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	
	{
		INT nResult	= Upper_Home();
		return nResult;
	}

	if (m_fHardware && m_bSel_X)
	{
		//v4.59A42
		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
		{
			if (!CheckMS90BugPusherAtSafePos())
			{
				CString szMsg = "BL X-HOME: BT Pusher XZ not at SAFE position";
				SetErrorMessage(szMsg);
				SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);	
				return Err_BinGripperMoveHome;
			}
		}

		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BL_AXIS_X, &m_stBLAxis_X) != 0)
			{
				CMS896AStn::MotionClearError(BL_AXIS_X, &m_stBLAxis_X);
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(BL_AXIS_X, 1, 0, &m_stBLAxis_X);

				if ((nResult == gnAMS_OK) && 
					(CMS896AStn::MotionReportErrorStatus(BL_AXIS_X, &m_stBLAxis_X) == 0) )//&& 
					//CMS896AStn::MotionIsPowerOn(BL_AXIS_X, &m_stBLAxis_X) )
				{
					Sleep(500);
					CMS896AStn::MotionSetPosition(BL_AXIS_X, 0, &m_stBLAxis_X);
					m_bHome_X	= TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_BinGripperMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BL_AXIS_X, &m_stBLAxis_X);
			CMS896AStn::MotionClearError(BL_AXIS_X, &m_stBLAxis_X);
			nResult = Err_BinGripperMoveHome;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			Sleep(100);
			m_bHome_X	= TRUE;
		}
	}

	return nResult;
}


INT CBinLoader::X_MoveTo(INT nPos, INT nMode)
{
/*
	//v4.59A40	//MS50
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	
	{
		INT nResult	= Upper_MoveTo(nPos, nMode);
		return nResult;
	}
*/
	//v4.59A42
	if (m_fHardware && m_bSel_X && (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER))
	{
		if (!CheckMS90BugPusherAtSafePos())
		{
			CString szMsg = "BL X-MOVETO: BT Pusher XZ not at SAFE position";
			SetErrorMessage(szMsg);
			SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);	
			return Err_BinGripperMove;
		}
	}

	INT nResult = MotorMoveTo(m_bSel_X, BL_AXIS_X, "", &m_stBLAxis_X,
							  nPos, nMode, m_bHome_X, TRUE, 100000);
	if (nResult == gnNOTOK)
	{
		nResult = Err_BinGripperMove;
	}
	return nResult;
}

INT CBinLoader::X_Sync()
{
	//v4.59A40	//MS50
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	
	{
		INT nResult = Upper_Sync();
		return nResult;
	}

	return MotorSync(m_bSel_X, BL_AXIS_X, &m_stBLAxis_X, "", TRUE, 100000);
}


////////////////////////////////////////////
//	X2 Axis (MS100 9Inch)
////////////////////////////////////////////
INT CBinLoader::X2_Home()
{
	INT nResult	= Lower_Home();
	return nResult;
}

INT CBinLoader::X2_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= Lower_MoveTo(nPos, nMode);
	return nResult;
}

INT CBinLoader::X2_Sync()
{
	INT nResult = Lower_Sync();
	return nResult;
}



////////////////////////////////////////////
//	Z Axis 
////////////////////////////////////////////
INT CBinLoader::Z_Home()
{
	INT nResult	= gnOK;
	int nCommRetry = 0;
	LONG lTmpBLZLimit;
	CString szLog;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	//MS60 AutoLine Z motor; uses same WL Z motor	//v4.56A1
//	if (m_bDisableLoaderY)
//	{
//		return Z_Home_AutoLine();
//	}

	if (m_fHardware && m_bSel_Z)
	{
		try
		{
#ifdef NU_MOTION
			//Enlarge the softawre limit before Home
			CMS896AStn::MotionSetSoftwareLimit(BL_AXIS_Z, 99999999, -99999999, &m_stBLAxis_Z);
#endif

			if (CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) != 0)
			{
				m_bComm_Z = FALSE;
				CMS896AStn::MotionClearError(BL_AXIS_Z, &m_stBLAxis_Z);
			}

			if ((pApp->GetCustomerName() == "AOT" || pApp->GetCustomerName() == "ABC") && !IsAllMagazineExist())	//ABC is used in AOT
			{
				return Err_BinLoaderMagazineNotExist;
			}

			if (!m_bComm_Z)
			{
				m_bHome_Z = FALSE;

				SHORT siOrgDACLimit = CMS896AStn::MotionGetDacLimit(BL_AXIS_Z, &m_stBLAxis_Z);
				SHORT siCurrentDAC = CMS896AStn::MotionGetDacLimit(BL_AXIS_Z, &m_stBLAxis_Z);

				while(nCommRetry < 3)
				{
					nResult = MotionDirectionalCommutateServo(BL_AXIS_Z, 8, 8000, HP_POSITIVE_DIR, &m_stBLAxis_Z);
					//nResult = CMS896AStn::MotionCommutateServo(BL_AXIS_Z, &m_stBLAxis_Z);

#ifdef NU_MOTION
					if ((nResult == gnOK) &&
						(CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) == 0) )
#else
					if (nResult == gnOK)
#endif
					{
						m_bComm_Z = TRUE;
						break;
					}
					else
					{
						siCurrentDAC = (SHORT)(siCurrentDAC * 1.15);
						CMS896AStn::MotionSetDacLimit(BL_AXIS_Z, siCurrentDAC, 
							CMS896AStn::MotionGetDacTimeLimit(BL_AXIS_Z, &m_stBLAxis_Z), 
							&m_stBLAxis_Z);
					}
					
					DisplayMessage("Retry sw comm on BinLoader Z");
					nCommRetry++;
					Sleep(300);
				}
				
				if ( nResult == gnOK )
				{
					//Restore pre-set DAC limit
					CMS896AStn::MotionSetDacLimit(BL_AXIS_Z, siOrgDACLimit, CMS896AStn::MotionGetDacTimeLimit(BL_AXIS_Z, &m_stBLAxis_Z), &m_stBLAxis_Z);
					m_bComm_Z = TRUE;
				}
				else
				{
					nResult	= Err_BinLoaderZCommutate;
					CMS896AStn::MotionClearError(BL_AXIS_Z, &m_stBLAxis_Z);
				}
			}

			if (nResult == gnOK)
			{
				if (CMS896AStn::m_bCEMark)	//v4.51A8	//Long	//Osram MS109
				{
					nResult = CMS896AStn::MotionMoveHome2(BL_AXIS_Z, 0, 0, &m_stBLAxis_Z);		//v4.50A23
				}
				else
				{
					nResult = CMS896AStn::MotionMoveHome(BL_AXIS_Z, 1, 0, &m_stBLAxis_Z);
				}

				INT nWait = Z_Sync(TRUE);
				if (nWait != gnOK)
				{
					m_bComm_Z = FALSE;
					m_bHome_Z = FALSE;
					return Err_BinLoaderZMoveHome;
				}

				nResult = CMS896AStn::MotionMoveHome(BL_AXIS_Z, 1, 0, &m_stBLAxis_Z);

				if ((nResult == gnAMS_OK) &&
					(CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) == 0) ) //&& 
					//CMS896AStn::MotionIsPowerOn(BL_AXIS_Z, &m_stBLAxis_Z) )
				{
#ifdef NU_MOTION
					Sleep(100);
					GetEncoderValue();
					lTmpBLZLimit = m_lEnc_Z;
#endif

					CMS896AStn::MotionSelectSearchProfile(BL_AXIS_Z, "spfBinLoaderZIndex", &m_stBLAxis_Z);
					CMS896AStn::MotionSearch(BL_AXIS_Z, 0, SFM_WAIT, &m_stBLAxis_Z, "spfBinLoaderZIndex");

					if ((CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) == 0) && 
						CMS896AStn::MotionIsPowerOn(BL_AXIS_Z, &m_stBLAxis_Z))
					{
						Sleep(50);
#ifdef NU_MOTION
						GetEncoderValue();
						lTmpBLZLimit = lTmpBLZLimit - m_lEnc_Z;
#endif
						CMS896AStn::MotionSetPosition(BL_AXIS_Z, 0, &m_stBLAxis_Z);
						m_bHome_Z = TRUE;
						m_dZHomeTime = GetCurTime();

#ifdef NU_MOTION
						Z_Sync();	//v4.46T4		//Chris Keung
						CMS896AStn::MotionSetSoftwareLimit(BL_AXIS_Z, BL_Z_POS_LIMIT_FROM_HOME+lTmpBLZLimit, BL_Z_NEG_LIMIT_FROM_HOME+lTmpBLZLimit, &m_stBLAxis_Z);
						szLog.Format("BLZ limit: %d, %d", BL_Z_POS_LIMIT_FROM_HOME+lTmpBLZLimit, BL_Z_NEG_LIMIT_FROM_HOME+lTmpBLZLimit);
						CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
#endif
					}
					else
					{
						CMS896AStn::MotionPowerOff(BL_AXIS_Z, &m_stBLAxis_Z);
						nResult = Err_BinLoaderZMoveHome;
					}
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_BinLoaderZMoveHome;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BL_AXIS_Z, &m_stBLAxis_Z);
			CMS896AStn::MotionClearError(BL_AXIS_Z, &m_stBLAxis_Z);
			m_bComm_Z = FALSE;
			nResult = Err_BinLoaderZMoveHome;
//AfxMessageBox("111", MB_SYSTEMMODAL);
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

	//CheckResult(nResult, _T("Z Axis - Home"));
	return nResult;
}


INT CBinLoader::Z_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && m_bSel_Z)	// && !m_bDisableBL)
	{
		CString szProfile	= "";

		try
		{
			switch (nProfile)
			{
				case BL_SLOW_PROF:
					szProfile = "mpfBinLoaderZSlow";
					break;

				case BL_NORMAL_PROF:
				default:
					szProfile = "mpfBinLoaderZ";
					break;
			}

			if (szProfile != "")
			{
				CMS896AStn::MotionSelectProfile(BL_AXIS_Z, szProfile, &m_stBLAxis_Z);
				nResult = gnOK;
			}			
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BL_AXIS_Z, &m_stBLAxis_Z);
			nResult = gnNOTOK;
		}
	}

	return nResult;
}

INT CBinLoader::Z_SMoveTo(INT nPos, INT nMode)
{
	Z_Profile(BL_SLOW_PROF);
	INT nStatus = Z_MoveTo(nPos, nMode);
	Z_Profile(BL_NORMAL_PROF);
	return nStatus;
}

INT CBinLoader::Z_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;
	LONG lEncoderPos;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if (m_fHardware && m_bSel_Z)
	{
		try
		{
			if (!Z_IsPowerOn())
			{
				SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				return Err_BinLoaderZNotPowerOn;
			}
			if (!m_bHome_Z)
			{
				SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				return Err_BinLoaderZNotHomed;
				//nResult = gnNOTOK;
			}
			
			if ((pApp->GetCustomerName() == "AOT" || pApp->GetCustomerName() == "ABC") && !IsAllMagazineExist())	//ABC is used in AOT
			{
				return Err_BinLoaderMagazineNotExist;
			}

			if (nResult == gnOK)
			{
				if (m_bDisableLoaderY)
					lEncoderPos = CMS896AStn::MotionGetCommandPosition(BL_AXIS_Z, &m_stBLAxis_Z);
				else
					lEncoderPos = CMS896AStn::MotionGetEncoderPosition(BL_AXIS_Z, 1, &m_stBLAxis_Z);
				
				if ( abs(lEncoderPos - nPos) <= MS_BL_MGZ_MAX_PITCH)
				{
					//Use slow profile if travel distance is less than MAX Pitch
					CMS896AStn::MotionSelectProfile(BL_AXIS_Z, "mpfBinLoaderZSlow", &m_stBLAxis_Z);
				}
				else
				{
					if ( lEncoderPos > nPos )
					{
						//Going up
						CMS896AStn::MotionSelectProfile(BL_AXIS_Z, "mpfBinLoaderZSlow", &m_stBLAxis_Z);
					}
					else
					{
						//Going down
						//m_pServo_Z->SelectProfile("mpfBinLoaderZ");

						//Always use slow speed for temp. Z hit DAC limit error
						CMS896AStn::MotionSelectProfile(BL_AXIS_Z, "mpfBinLoaderZSlow", &m_stBLAxis_Z);
					}
				}
				
				CMS896AStn::MotionMoveTo(BL_AXIS_Z, nPos, SFM_NOWAIT, &m_stBLAxis_Z);

				if (nMode == SFM_WAIT)
				{
/*					if ((nResult = CMS896AStn::MotionSync(BL_AXIS_Z, 100000, &m_stBLAxis_Z)) != gnOK)*/
					if (Z_Sync_Auto() != gnOK)
					{
						SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
						nResult = Err_BinLoaderZMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BL_AXIS_Z, &m_stBLAxis_Z);
			CMS896AStn::MotionClearError(BL_AXIS_Z, &m_stBLAxis_Z);
			m_bHome_Z = FALSE;
			SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
			nResult = Err_BinLoaderZMove;
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

	//CheckResult(nResult, _T("Z Axis - MoveTo"));
	return nResult;
}


INT CBinLoader::Z_Sync_Normal(BOOL bCheckCover)
{
	if (CMS896AStn::m_bCEMark && bCheckCover)	//v4.50A23
	{
		return Z_Sync_Auto();
	}

	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Z)
	{
		try
		{
			nResult = MotorSync(m_bSel_Z, BL_AXIS_Z, &m_stBLAxis_Z, "", TRUE, 100000);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BL_AXIS_Z, &m_stBLAxis_Z);
			nResult = gnNOTOK;
		}
	}
	return nResult;
}

INT CBinLoader::Z_Sync(BOOL bCheckCover)
{
	if (!bCheckCover)
	{
		return Z_Sync_Normal(bCheckCover);
	}

	return Z_Sync_Auto();
}

INT CBinLoader::Z_Sync_Auto()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Z)
	{
		while (1)
		{
			if (!Z_IsPowerOn())
			{
				return gnNOTOK;
			}

			if ((CMS896AStn::MotionIsComplete(BL_AXIS_Z, &m_stBLAxis_Z) == TRUE))
			{
				Z_Sync_Normal();
				return gnOK;
			}

			if ((IsElevatorCoverOpen() == TRUE)		//CEMark requires immediate motion-abort
				/*(IsCoverOpen())*/ )
			{
				CMS896AStn::MotionStop(BL_AXIS_Z, &m_stBLAxis_Z);
				Z_Sync_Normal();
				m_bHome_Z = FALSE;
//				if (IsCoverOpen())
//				{
//					SetAlert_Red_Yellow(IDS_BL_COVER_OPEN);
//				}
//				else
				{
					SetAlert_Red_Yellow(IDS_BL_ELEVATOR_COVER_OPEN);
				}
				return gnNOTOK;
			}

			Sleep(10);
		}
	}

	return nResult;
}


////////////////////////////////////////////
//	Arm Axis 
////////////////////////////////////////////
BOOL CBinLoader::Arm_IsPowerOn()
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableBL)
		return TRUE;
	
	try
	{
		return CMS896AStn::MotionIsPowerOn(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CBinLoader::Arm_Home()
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bSel_Arm)
	{
		if ( CMS896AStn::MotionIsServo(BL_AXIS_EXARM, &m_stBLAxis_ExArm) )		//v3.59
		{
			try
			{	
				if ( CMS896AStn::MotionReportErrorStatus(BL_AXIS_EXARM, &m_stBLAxis_ExArm) != 0 )
				{
					m_bComm_Arm = FALSE;
					CMS896AStn::MotionClearError(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
				}

				if (!m_bComm_Arm)
				{
					m_bHome_Arm = FALSE;

					if ((nResult = CMS896AStn::MotionCommutateServo(BL_AXIS_EXARM, &m_stBLAxis_ExArm)) == gnOK)
					{
						m_bComm_Arm = TRUE;
					}
					else
					{
						nResult	= Err_BinExchgArmCommutate;
						CMS896AStn::MotionClearError(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
					}
				}

				if (nResult == gnOK)
				{
					CMS896AStn::MotionSelectControlParam(BL_AXIS_EXARM, "ctrBinExchgArmSts", &m_stBLAxis_ExArm);
					CMS896AStn::MotionSelectControlType(BL_AXIS_EXARM, 46, 1, &m_stBLAxis_ExArm);

					if ((nResult = CMS896AStn::MotionMoveHome(BL_AXIS_EXARM, 1, 0, &m_stBLAxis_ExArm)) == gnAMS_OK)
					{
						Sleep(100);
						CMS896AStn::MotionSetPosition(BL_AXIS_EXARM, 0, &m_stBLAxis_ExArm);
						m_bHome_Arm	= TRUE;
					}
					else if (nResult == gnNOTOK)
					{
						nResult = Err_BinExchgArmMoveHome;
					}

					CMS896AStn::MotionSelectControlParam(BL_AXIS_EXARM, "ctrBinExchgArmDyn", &m_stBLAxis_ExArm);
					CMS896AStn::MotionSelectControlType(BL_AXIS_EXARM, 39, 2, &m_stBLAxis_ExArm);
				}
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
				CMS896AStn::MotionClearError(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
				nResult = Err_BinExchgArmMoveHome;
				m_bComm_Arm = FALSE;
				m_bHome_Arm	= FALSE;
			}
		}
		else
		{
			try
			{	
				if ( CMS896AStn::MotionReportErrorStatus(BL_AXIS_EXARM, &m_stBLAxis_ExArm) != 0)
				{
					CMS896AStn::MotionClearError(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
				}

				m_bComm_Arm = TRUE;

				if (nResult == gnOK)
				{
					if ((nResult = CMS896AStn::MotionMoveHome(BL_AXIS_EXARM, 1, 0, &m_stBLAxis_ExArm)) == gnAMS_OK)
					{
						Sleep(100);
						CMS896AStn::MotionSetPosition(BL_AXIS_EXARM, 0, &m_stBLAxis_ExArm);
						m_bHome_Arm	= TRUE;
					}
					else if (nResult == gnNOTOK)
					{
						nResult = Err_BinExchgArmMoveHome;
					}
				}
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
				CMS896AStn::MotionClearError(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
				nResult = Err_BinExchgArmMoveHome;
				m_bHome_Arm	= TRUE;
			}
		}	
	}
	else
	{
		if (m_bSel_Arm)
		{
			Sleep(100);
			m_bHome_Arm	= TRUE;
			m_bComm_Arm = TRUE;
		}
	}

	CheckResult(nResult, _T("Arm Axis - Home"));
	return nResult;
}


INT CBinLoader::Arm_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (!m_bIsExChgArmExist)	//v3.59
		m_bSel_Arm = FALSE;

	if (m_fHardware && m_bSel_Arm)
	{
		try
		{
			if (!m_bHome_Arm)
			{
				nResult = gnNOTOK;
			}

			
			if (nResult == gnOK)
			{
				//m_pStepper_Arm->MoveTo(nPos, SFM_NOWAIT);
				CMS896AStn::MotionMoveTo(BL_AXIS_EXARM, nPos, SFM_NOWAIT, &m_stBLAxis_ExArm);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BL_AXIS_EXARM, 100000, &m_stBLAxis_ExArm)) != gnOK)
					{
						nResult = Err_BinExchgArmMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
			CMS896AStn::MotionClearError(BL_AXIS_EXARM, &m_stBLAxis_ExArm);

			m_bHome_Arm = FALSE;
			nResult = Err_BinExchgArmMove;
		}
	}
	else
	{
		if (m_bSel_Arm)
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
	CheckResult(nResult, _T("Arm Axis - MoveTo"));
	return nResult;
}

INT CBinLoader::Arm_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Arm)
	{
		try
		{
			CMS896AStn::MotionSync(BL_AXIS_EXARM, 100000, &m_stBLAxis_ExArm);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BL_AXIS_EXARM, &m_stBLAxis_ExArm);
			nResult = gnNOTOK;
		}
	}

	CheckResult(nResult, _T("Arm Axis - Sync"));
	return nResult;
}



// ==========================================================================================================
//				Upper Gripper Move Related Function
// ==========================================================================================================
INT CBinLoader::Upper_Home()
{
	INT nResult	= Gripper_X_Home(m_bSel_Upper, BL_AXIS_UPPER, "spfBinLoaderUpperIndex", &m_stBLAxis_Upper, m_bHome_Upper);
	if (nResult	!= gnOK)
	{
		nResult = Err_BinGripperMoveHome;
	}
	return nResult;
}


INT CBinLoader::Upper_MoveTo(INT nPos, INT nMode, BOOL bMissingStepCheck)
{
	INT nResult = MotorMoveTo(m_bSel_Upper, BL_AXIS_UPPER, "mpfBinLoaderUpper", &m_stBLAxis_Upper,
							  nPos, nMode, m_bHome_Upper, TRUE, 10000, bMissingStepCheck);
	if (nResult == gnNOTOK)
	{
		nResult = Err_BinGripperMove;
	}
	return nResult;
}


INT CBinLoader::Upper_Sync()
{
	INT nResult = MotorSync(m_bSel_Upper, BL_AXIS_UPPER, &m_stBLAxis_Upper, "mpfBinLoaderUpper", TRUE);
	return nResult;
}

LONG CBinLoader::BinGripperMoveTo(INT nPos, BOOL bReadBarCode)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBL)
	{
		return TRUE;
	}

	LONG lEnc = 0;

	//v4.45T2
	if (!X_IsPowerOn())
	{
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;	
		return FALSE;
	}

	LONG lRet = gnOK;
	//v4.02T6	//New barcode prescan fcn
	if ( (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	&&	
			bReadBarCode && m_bUseBarcode)
	{	
		if (m_ucBarcodeModel == BL_BAR_SYMBOL || m_ucBarcodeModel == BL_BAR_DATALOGIC)
		{
			LONG lCurrEnc = GetGripperEncoder();

			LONG lProfileTime = CMS896AStn::MotionGetProfileTime(BL_AXIS_X, "mpfBinGripper", 
								(lCurrEnc - nPos), (lCurrEnc - nPos), HIPEC_SAMPLE_RATE, &m_stBLAxis_X);

			LONG lDelay = (LONG) lProfileTime - 200;	//300;		//v4.37

			lRet = Upper_MoveTo(nPos, SFM_NOWAIT);

			if (lDelay > 0)
			{
				Sleep(lDelay);
			}

			m_szBCName = "";
			m_szDBPreScanBCName = "";
			ReadBarcode(&m_szDBPreScanBCName, 50);	//v4.37
			
			CString szLog;
			szLog.Format("BL BinGripperMoveTo BC Symbol DL %s", m_szDBPreScanBCName);
			BL_DEBUGBOX(szLog);

			if (X_Sync() == gnNOTOK)
			{
				X_Home();
				return FALSE;
			}
			Sleep(100);
			lEnc = GetGripperEncoder();
		}
	}
	else
	{
		if (Upper_MoveTo(nPos, SFM_WAIT) != gnOK)
		{
			// Power off the gripper
			Upper_Home();		
			return FALSE;
		}
		Sleep(100);		//v3.65		//pllm
		lEnc = GetGripperEncoder();
	}

	if ((lRet == gnOK) && (abs(lEnc - nPos) > 200))
	{
		// do not turn on/off the gripper
		lRet = Upper_Home();
		if (lRet == gnOK)
		{
			lRet = Upper_MoveTo(nPos, SFM_WAIT);
		}

		if (lRet != gnOK)
		{
			//SetAlert_Red_Yellow(IDS_BL_GRIPPER_MISSINGSTEP);
			SetAlert_Red_Yellow(IDS_BL_UP_GRIPPER_MISSINGSTEP);

			CString szErr;
			szErr.Format("BL Gripper is missing step - ENC=%ld, POS=%ld", lEnc, nPos);
			SetErrorMessage(szErr);
		
			// Power off the gripper
			X_Home();
			//return IDS_BL_GRIPPER_MISSINGSTEP;
			return IDS_BL_UP_GRIPPER_MISSINGSTEP;
		}
	}

	return TRUE;
}

LONG CBinLoader::BinGripperMoveTo_Auto(INT nPos, INT nWait)
{
	if (BinGripperSynMove_Auto() == FALSE)
	{
		return FALSE;
	}
	Upper_MoveTo(nPos, nWait);

	return TRUE;
}

LONG CBinLoader::BinGripperSynMove_Auto(VOID)
{
	if (!CMS896AStn::m_bCEMark)	
	{
		if (X_Sync() == gnOK)
		{
			if (m_bGripperUseEncoder)	//v4.59A39
			{
				LONG lEncX = GetGripperEncoder();
				LONG lCmdX = GetGripperCommandPosition();

				if (abs(lEncX - lCmdX) > 200)
				{
					LONG lRet = Upper_Home();
					if (lRet == gnOK)
					{
						lRet = Upper_MoveTo(lCmdX, SFM_WAIT);
					}
					if (lRet != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_GRIPPER_MISSINGSTEP);
						SetAlert_Red_Yellow(IDS_BL_UP_GRIPPER_MISSINGSTEP);

						CString szErr;
						szErr.Format("BL Gripper is missing step - ENC=%ld, POS=%ld", lEncX, lCmdX);
						SetErrorMessage(szErr);
					
						Upper_Home();	
						return FALSE;
					}
				}
			}

			return TRUE;
		}
	}
	else		//v4.49A6	//FreeScale MS109 with CEMark
	{
		while (1)
		{
			if (!X_IsPowerOn())
			{
				//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;	
				return FALSE;
			}

			if (IsGripperMotionComplete())
			{
				if (X_Sync()  == gnNOTOK)
				{
					X_Home();
					return FALSE;
				}
				return TRUE;
			}

			if (IsElevatorCoverOpen() == TRUE)		//CEMark requires immediate motion-abort
			{
				GripperMotionStop();
				X_Sync();
				return FALSE;
			}

			Sleep(10);
		}
	}

	return FALSE;
}

LONG CBinLoader::BinGripperSearchInClamp(INT siDirection, LONG lSearchDistance, LONG lDriveInDistance, 
		BOOL bWaitComplete, BOOL bReadBarcode, BOOL bSync)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBL)
	{
		return FALSE;
	}
	if (IsFrameInClamp())
	{
		return TRUE;
	}

	try
	{
		CMS896AStn::MotionSelectSearchProfile(BL_AXIS_UPPER, BL_SP_BIN_GRIPPER_SRCH_IN_CLAMP, &m_stBLAxis_Upper);
		CMS896AStn::MotionUpdateSearchProfile(BL_AXIS_UPPER, BL_SP_BIN_GRIPPER_SRCH_IN_CLAMP, 0, lSearchDistance, 0, lDriveInDistance, &m_stBLAxis_Upper);
		CMS896AStn::MotionSearch(BL_AXIS_UPPER, !siDirection, SFM_NOWAIT, &m_stBLAxis_Upper, BL_SP_BIN_GRIPPER_SRCH_IN_CLAMP);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		SetMotionCE(TRUE, "ERROR: CE Set by BL BinGripperSearchInClamp");		//v4.59A39
	}

	if (bWaitComplete == SFM_WAIT)
	{
		//X_Sync();
		//Sleep(100);
		BOOL bReadBarcodeOnce = FALSE;	//pllm
		while (1)
		{
			if (!Upper_IsPowerOn())
			{
				return FALSE;
			}

			if ((CMS896AStn::MotionIsComplete(BL_AXIS_UPPER, &m_stBLAxis_Upper) == TRUE))
			{
				if (bSync)
				{
					if (Upper_Sync() == gnNOTOK)
					{
						SetGripperState(FALSE);
						Upper_Home();
						return FALSE;
					}
				}
				break;	
			}
			if (bReadBarcode && !bReadBarcodeOnce)
			{
				if (m_ucBarcodeModel == BL_BAR_SYMBOL || m_ucBarcodeModel == BL_BAR_DATALOGIC)
				{
					ReadBarcode(&m_szBCName, 50);
					bReadBarcodeOnce = TRUE;
				}
				else if (m_ucBarcodeModel == BL_BAR_REGAL)
				{
					ReadBarcodeValueRegal(&m_szBCName, 10);
					bReadBarcodeOnce = TRUE;
				}
			}

			Sleep(10);
		}
	}

	if (!IsFrameInClamp())
	{
		Sleep(100);
		if (!IsFrameInClamp())
		{
			return FALSE;
		}
	}

	if (IsFrameJam())
	{
		return FALSE;
	}
	return TRUE;
}

LONG CBinLoader::BinGripperMoveSearchJam(INT nPos, BOOL bCheckReAlign, BOOL bReadBarCode, BOOL bUnload)
{
	INT nResult;
	CString szLog;

	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBL)	//v3.60
	{
		return TRUE;
	}
	BOOL bMissingStepCheck = bUnload ? FALSE : TRUE;
	//v4.02T6	//Barcode prescan fcn for UNLOAD
	if (bReadBarCode)
	{
		LONG lCurrEnc = GetGripperEncoder();
		//LONG lProfileTime = CMS896AStn::MotionGetProfileTime(BL_AXIS_X, "mpfBinGripper", 
		//					(lCurrEnc - nPos), (lCurrEnc - nPos), HIPEC_SAMPLE_RATE, &m_stBLAxis_X);
		LONG lProfileTime = CMS896AStn::MotionGetProfileTime(BL_AXIS_UPPER, "mpfBinLoaderUpper", 
					(lCurrEnc - nPos), (lCurrEnc - nPos), HIPEC_SAMPLE_RATE, &m_stBLAxis_Upper);
		LONG lDelay = (LONG) lProfileTime - 300;

		nResult = Upper_MoveTo(nPos, SFM_NOWAIT, bMissingStepCheck);

		if (lDelay > 0)
		{
			Sleep(lDelay);
		}

		m_szBCName = "";
		m_szDBPreScanBCName = "";
		ReadBarcodeValueRegal(&m_szDBPreScanBCName, 20);

		szLog.Format("BL BinGripperMoveSearchJam BC Regal DL %s,Unload,%d,CheckMissingStep,%d", m_szDBPreScanBCName, bUnload, bMissingStepCheck);
	}
	else
	{
		szLog.Format("BL BinGripperMoveSearchJam WithoutBC,Unload,%d,CheckMissingStep,%d", bUnload, bMissingStepCheck);
		nResult = Upper_MoveTo(nPos, SFM_NOWAIT, bMissingStepCheck);
		Sleep(10);
	}
	BL_DEBUGBOX(szLog);
//	BOOL bRealignFrame = TRUE;

	while (1)
	{
		if (nResult != gnOK)
		{
			SetErrorMessage("BinGripperMoveSearchJam : nResult != gnOK");
			return FALSE;
		}

		if ((IsFrameJam() == TRUE) /*&& IsMotorMissingStep(BL_AXIS_UPPER, &m_stBLAxis_Upper)*/)
		{
			if (CheckIfGripperUnloadAlarm(bUnload))
			{
				SetErrorMessage("BinGripperMoveSearchJam : IsFrameJam() == TRUE");
				CMS896AStn::MotionStop(BL_AXIS_UPPER, &m_stBLAxis_Upper);
				return FALSE;
			}
			else
			{
				BL_DEBUGBOX("Bin Gripper Unload Jam without alarm");
				return TRUE;
			}
		}

		if (CMS896AStn::MotionIsComplete(BL_AXIS_UPPER, &m_stBLAxis_Upper) == TRUE)
		{
			if (Upper_Sync() == gnNOTOK)
			{
				SetErrorMessage("BinGripperMoveSearchJam : Upper_Sync() == gnNOTOK");
				SetGripperState(FALSE);
				Upper_Home();
				return FALSE;
			}
			break;
		}

		//v4.49A6	//CEMark requires motionAbort when cover is open
		if (CMS896AStn::m_bCEMark && IsElevatorCoverOpen())
		{
			SetErrorMessage("CMS896AStn::m_bCEMark && IsElevatorCoverOpen()");
			CMS896AStn::MotionStop(BL_AXIS_UPPER, &m_stBLAxis_Upper);
			return FALSE;
		}
/*
		//pllm
		if (bCheckReAlign)
		{
			if (m_nExArmReAlignBinConvID != 0)
			{
				bRealignFrame = RealignBinFrameRpy(TRUE); 
			}	
		}
*/
		Sleep(1);
	}
/*
	if (!bRealignFrame)
	{
		SetErrorMessage("!bRealignFrame");
		return FALSE;
	}
*/
	return TRUE;
}


LONG CBinLoader::BinGripperSearchJam(INT siDirection, LONG lSearchPosition, LONG lSearchVelocity, BOOL bWaitComplete)
{
	LONG lSearchDistance;
	INT siSearchDirection = siDirection;	//pllm

	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableBL)	//v3.60
	{
		return TRUE;
	}

	//lSearchDistance = abs(lSearchPosition-GetGripperEncoder());
	LONG lEncoder = GetGripperEncoder();
	lSearchDistance = abs(lSearchPosition - lEncoder);

	if (lSearchDistance < 100)	//andrew7
	{
		return TRUE;
	}

	//andrew12345
	if ((lSearchPosition - GetGripperEncoder()) < 0)
	{
		siSearchDirection = HP_NEGATIVE_DIR;
	}

	try
	{
		CMS896AStn::MotionSelectSearchProfile(BL_AXIS_UPPER, "spfBinGripperSearchJam", &m_stBLAxis_Upper);
//#ifndef NU_MOTION
		CMS896AStn::MotionUpdateSearchProfile(BL_AXIS_UPPER, "spfBinGripperSearchJam",
											  lSearchVelocity, lSearchDistance, &m_stBLAxis_Upper);
//#endif
		CMS896AStn::MotionSearch(BL_AXIS_UPPER, !siSearchDirection, SFM_NOWAIT, &m_stBLAxis_Upper, "spfBinGripperSearchJam");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	if (bWaitComplete == SFM_WAIT)
	{
		if (Upper_Sync() == gnNOTOK)
		{
			SetGripperState(FALSE);
			Upper_Home();
			return FALSE;
		}
		Sleep(100);
	}

	//v4.45T2
	if (!Upper_IsPowerOn())
	{
		return FALSE;
	}

	if (abs(GetGripperEncoder() - lSearchPosition) < 200)
	{
		return TRUE;
	}
	else
	{
		if (IsFrameJam())
		{
			Sleep(500);
			if (IsFrameJam())
			{
				return FALSE;
			}
		}

		lSearchDistance = abs(lSearchPosition - GetGripperEncoder());

		try
		{
			CMS896AStn::MotionSelectSearchProfile(BL_AXIS_UPPER, "spfBinGripperSearchJam", &m_stBLAxis_Upper);
			CMS896AStn::MotionUpdateSearchProfile(BL_AXIS_UPPER, "spfBinGripperSearchJam",
												  lSearchVelocity, lSearchDistance, &m_stBLAxis_Upper);

			CMS896AStn::MotionSearch(BL_AXIS_UPPER, !siSearchDirection, SFM_NOWAIT, &m_stBLAxis_Upper, "spfBinGripperSearchJam");
		}
		catch (CAsmException e)
		{
			DisplayException(e);
		}
	
		if (bWaitComplete == SFM_WAIT)
		{
			if (Upper_Sync() == gnNOTOK)
			{
				SetGripperState(FALSE);
				Upper_Home();
				return FALSE;
			}
			Sleep(100);
		}

		if (abs(GetGripperEncoder() - lSearchPosition) < 200)
		{
			return TRUE;
		}
		else
		{
			BL_DEBUGBOX("BinGripperSearchJam target position out of range");	//pllm
			return FALSE;
		}
	}

	return TRUE;
}

LONG CBinLoader::BinGripperSearchScan(INT siDirection, LONG lSearchDistance)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableBL)	//v3.60
	{
		return TRUE;
	}

	try
	{
		CMS896AStn::MotionSelectSearchProfile(BL_AXIS_UPPER, BL_SP_BIN_GRIPPER_SRCH_JAM, &m_stBLAxis_Upper);
		CMS896AStn::MotionUpdateSearchProfile(BL_AXIS_UPPER, BL_SP_BIN_GRIPPER_SRCH_JAM,
											  0.5, lSearchDistance, &m_stBLAxis_Upper);
		CMS896AStn::MotionSearch(BL_AXIS_UPPER, siDirection, SFM_NOWAIT, &m_stBLAxis_Upper, BL_SP_BIN_GRIPPER_SRCH_JAM);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	if (IsFrameJam())
	{
		return FALSE;
	}

	return 1;
}

// ==========================================================================================================
//				Lower Gripper Move Related Function
// ==========================================================================================================
INT CBinLoader::Lower_Home()
{
	INT nResult	= Gripper_X_Home(m_bSel_Lower, BL_AXIS_LOWER, "spfBinLoaderLowerIndex", &m_stBLAxis_Lower, m_bHome_Lower);
	if (nResult	!= gnOK)
	{
		nResult = Err_BinGripperMoveHome;
	}
	return nResult;
}

INT CBinLoader::Lower_MoveTo(INT nPos, INT nMode, BOOL bMissingStepCheck)
{
	INT nResult = MotorMoveTo(m_bSel_Lower, BL_AXIS_LOWER, "mpfBinLoaderLower", &m_stBLAxis_Lower,
							  nPos, nMode, m_bHome_Lower, TRUE, 10000, bMissingStepCheck);
	if (nResult == gnNOTOK)
	{
		nResult = Err_BinGripperMove;
	}
	return nResult;
}

INT CBinLoader::Lower_Sync()
{
	INT nResult = MotorSync(m_bSel_Lower, BL_AXIS_LOWER, &m_stBLAxis_Lower, "mpfBinLoaderLower", TRUE);
	return nResult;
}

LONG CBinLoader::BinGripper2MoveTo(INT nPos, BOOL bReadBarCode)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	if (m_bDisableBL)
	{
		return TRUE;
	}

	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return TRUE;
	}

	LONG lEnc = 0;

	if (!Lower_IsPowerOn())
	{
		SetMotionCE(TRUE, "Bin Loader Lower Gripper Not Power (BinGripper2MoveTo)");
		return FALSE;
	}

	LONG lRet = gnOK;
	if (bReadBarCode && m_bUseBarcode)
	{	
		if (GetBCModel() == BL_BAR_SYMBOL || GetBCModel() == BL_BAR_DATALOGIC)
		{
			LONG lCurrEnc = GetGripper2Encoder();

			LONG lProfileTime = CMS896AStn::MotionGetProfileTime(BL_AXIS_X2, "mpfBinGripper", 
									(lCurrEnc - nPos), (lCurrEnc - nPos), HIPEC_SAMPLE_RATE, &m_stBLAxis_X2);

			LONG lDelay = (LONG) lProfileTime - 200;	

			CString szLog;
			szLog.Format("BL BinGripper2MoveTo BC motion time = %ld; Delay = %ld; dist = %ld", lProfileTime, lDelay, (lCurrEnc - nPos));
			//SetErrorMessage(szLog);

			lRet = Lower_MoveTo(nPos, SFM_NOWAIT);

			if (lDelay > 0)
			{
				Sleep(lDelay);
			}

			m_szBCName2 = "";
			m_szDBPreScanBCName = "";
			ReadBarcode2(&m_szDBPreScanBCName, 50);

			Lower_Sync();
			Sleep(100);
			lEnc = GetGripper2Encoder();
		}
	}
	else
	{
		if (Lower_MoveTo(nPos, SFM_WAIT) != gnOK)
		{
			// Power off the gripper
			Lower_Home();		
			return FALSE;
		}
		
		Sleep(100);
		lEnc = GetGripper2Encoder();
	}

	if (abs(lEnc - nPos) > 200)
	{
		// do not turn on/off the gripper
		lRet = Lower_Home();
		if (lRet == gnOK)
		{
			lRet = Lower_MoveTo(nPos, SFM_WAIT);
		}
		if (lRet != gnOK)
		{
			//SetAlert_Red_Yellow(IDS_BL_GRIPPER_MISSINGSTEP);
			SetAlert_Red_Yellow(IDS_BL_LOW_GRIPPER_MISSINGSTEP);

			CString szErr;
			szErr.Format("BL Gripper2 (LOWER) is missing step - ENC=%ld, POS=%ld", lEnc, nPos);
			SetErrorMessage(szErr);
		
			// Power off the gripper
			Lower_Home();		
			//return IDS_BL_GRIPPER_MISSINGSTEP;
			return IDS_BL_LOW_GRIPPER_MISSINGSTEP;
		}
	}

	return TRUE;
}

LONG CBinLoader::BinGripper2SearchInClamp(INT siDirection, LONG lSearchDistance, LONG lDriveInDistance, 
										  BOOL bWaitComplete, BOOL bReadBarcode, BOOL bSync)
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableBL)		
		return FALSE;
    if( IsFrameInClamp2() )
		return TRUE;
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
		return TRUE;

	try
	{
		CMS896AStn::MotionSelectSearchProfile(BL_AXIS_LOWER, BL_SP_BIN_GRIPPER_SRCH_IN_CLAMP, &m_stBLAxis_Lower);
		CMS896AStn::MotionUpdateSearchProfile(BL_AXIS_LOWER, BL_SP_BIN_GRIPPER_SRCH_IN_CLAMP, 0, lSearchDistance, 0, lDriveInDistance,&m_stBLAxis_Lower);
		CMS896AStn::MotionSearch(BL_AXIS_LOWER, !siDirection, SFM_NOWAIT, &m_stBLAxis_Lower, BL_SP_BIN_GRIPPER_SRCH_IN_CLAMP);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	if (bWaitComplete == SFM_WAIT)
	{
		BOOL bReadBarcodeOnce = FALSE;
		while(1)
		{
			if ( (CMS896AStn::MotionIsComplete(BL_AXIS_LOWER, &m_stBLAxis_Lower) == TRUE) )
			{
				if (bSync)
				{
					if (Lower_Sync() == gnNOTOK)
					{
						SetGripper2State(FALSE);
						Lower_Home();
						return FALSE;
					}
				}
				break;
			}
			if (bReadBarcode && !bReadBarcodeOnce && (GetBCModel() == BL_BAR_SYMBOL || GetBCModel() == BL_BAR_DATALOGIC))
			{
				ReadBarcode2(&m_szBCName2, 50);
				bReadBarcodeOnce = TRUE;
			}

			Sleep(10);
		}
	}

    if (!IsFrameInClamp2())
    {
		Sleep(100);
		if (!IsFrameInClamp2())
		{
			return FALSE;
		}
    }

    if( IsFrameJam2() )
    {
		return FALSE;
	}

	return TRUE;
}

LONG CBinLoader::BinGripper2MoveSearchJam(INT nPos, BOOL bCheckReAlign, BOOL bReadBarCode, BOOL bUnload)
{
	INT nResult;
	CString szLog;
	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableBL)
		return TRUE;
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
		return TRUE;
	
	BOOL bMissingStepCheck = (bUnload == TRUE) ? FALSE : TRUE;
	if (bReadBarCode)
	{
		LONG lCurrEnc = GetGripper2Encoder();
		//LONG lProfileTime = CMS896AStn::MotionGetProfileTime(BL_AXIS_LOWER, "mpfBinGripper2", 
		//						(lCurrEnc - nPos), (lCurrEnc - nPos), HIPEC_SAMPLE_RATE, &m_stBLAxis_Lower);
		LONG lProfileTime = CMS896AStn::MotionGetProfileTime(BL_AXIS_LOWER, "mpfBinLoaderLower", 
						(lCurrEnc - nPos), (lCurrEnc - nPos), HIPEC_SAMPLE_RATE, &m_stBLAxis_Lower);
		LONG lDelay = (LONG) lProfileTime - 300;
	
		nResult = Lower_MoveTo(nPos, SFM_NOWAIT, bMissingStepCheck);
		if (lDelay > 0)
		{
			Sleep(lDelay);
		}

		m_szBCName2 = "";
		m_szDBPreScanBCName = "";
		ReadBarcode2(&m_szDBPreScanBCName, 20);
		szLog.Format("BL BinGripper2MoveSearchJam BC Regal DL %s", m_szDBPreScanBCName);
	}
	else
	{
		szLog.Format("BL BinGripper2MoveSearchJam WithoutBC,Unload,%d,CheckMissingStep,%d", m_szDBPreScanBCName, bUnload, bMissingStepCheck);
		nResult = Lower_MoveTo(nPos, SFM_NOWAIT, bMissingStepCheck);
		Sleep(10);
	}
	BL_DEBUGBOX(szLog);
//	BOOL bRealignFrame = TRUE;

	while(1)
	{
		if (nResult != gnOK)
		{
			SetErrorMessage("BinGripper2MoveSearchJam : nResult != gnOK");
			return FALSE;
		}

		if ((IsFrameJam2() == TRUE) /*&& IsMotorMissingStep(BL_AXIS_LOWER, &m_stBLAxis_Lower)*/)
		{
			if (CheckIfGripper2UnloadAlarm(bUnload))
			{
				SetErrorMessage("BinGripper2MoveSearchJam : IsFrameJam2() == TRUE");
				CMS896AStn::MotionStop(BL_AXIS_LOWER, &m_stBLAxis_Lower);
				return FALSE;
			}
			else
			{
				BL_DEBUGBOX("Bin Gripper2 Unload Jam without alarm");
				return TRUE;
			}
		}

		if ( CMS896AStn::MotionIsComplete(BL_AXIS_LOWER, &m_stBLAxis_Lower) == TRUE )
		{
			if (Lower_Sync() == gnNOTOK)
			{
				SetErrorMessage("BinGripper2MoveSearchJam : Lower_Sync() == gnNOTOK");
				SetGripper2State(FALSE);
				Lower_Home();
				return FALSE;
			}
			break;
		}

		//v4.49A6	//CEMark requires motionAbort when cover is open
		if (CMS896AStn::m_bCEMark && IsElevatorCoverOpen())
		{
			CMS896AStn::MotionStop(BL_AXIS_LOWER, &m_stBLAxis_Lower);
			return FALSE;
		}
/*
		if (bCheckReAlign)
		{
			if ( m_nExArmReAlignBinConvID != 0 )
				bRealignFrame = RealignBinFrameRpy(TRUE);	
		}
*/
		Sleep(1);
	}
/*
	if (bRealignFrame == FALSE)
		return FALSE;	
*/
	return TRUE;
}

LONG CBinLoader::BinGripper2SearchScan(INT siDirection, LONG lSearchDistance)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableBL)	//v3.60
		return TRUE;

	try
	{
		CMS896AStn::MotionSelectSearchProfile(BL_AXIS_LOWER, BL_SP_BIN_GRIPPER_SRCH_JAM, &m_stBLAxis_Lower);
		CMS896AStn::MotionUpdateSearchProfile(BL_AXIS_LOWER, BL_SP_BIN_GRIPPER_SRCH_JAM,
			0.5, lSearchDistance, &m_stBLAxis_Lower);
		CMS896AStn::MotionSearch(BL_AXIS_LOWER, siDirection, SFM_NOWAIT, &m_stBLAxis_Lower, BL_SP_BIN_GRIPPER_SRCH_JAM);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

    if( IsFrameJam2() )
    {
		return FALSE;
	}

	return 1;
}

LONG CBinLoader::BinGripper2MoveTo_Auto(INT nPos, INT nWait)
{
    if (BinGripper2SynMove_Auto()==FALSE)
	{
        return FALSE;
	}

    if (Lower_MoveTo(nPos, nWait) != gnOK)
	{
		return FALSE;
	}
    return TRUE;
}

LONG CBinLoader::BinGripper2SynMove_Auto(VOID)
{
	if (!CMS896AStn::m_bCEMark)	
	{
		if (Lower_Sync() == gnOK)
		{
			if (m_bGripperUseEncoder)	//v4.59A39
			{
				LONG lEncX = GetGripper2Encoder();
				LONG lCmdX = GetGripper2CommandPosition();

				if (abs(lEncX - lCmdX) > 200)
				{
					LONG lRet = Lower_Home();
					if (lRet == gnOK)
					{
						lRet = Lower_MoveTo(lCmdX, SFM_WAIT);
					}
					if (lRet != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_GRIPPER_MISSINGSTEP);
						SetAlert_Red_Yellow(IDS_BL_LOW_GRIPPER_MISSINGSTEP);

						CString szErr;
						szErr.Format("BL Gripper is missing step - ENC=%ld, POS=%ld", lEncX, lCmdX);
						SetErrorMessage(szErr);
					
						Lower_Home();	
						return FALSE;
					}
				}
			}
			return TRUE;
		}
	}
	else		//v4.49A6	//CEMark
	{
		while (1)
		{
			if (!Lower_IsPowerOn())
			{
				//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;	
				return FALSE;
			}

			if ((CMS896AStn::MotionIsComplete(BL_AXIS_LOWER, &m_stBLAxis_Lower) == TRUE))
			{
				if (Lower_Sync() == gnNOTOK)
				{
					SetGripper2State(FALSE);
					Lower_Home();
					return FALSE;
				}
				return TRUE;
			}

			if (IsElevatorCoverOpen() == TRUE)		//CEMark requires immediate motion-abort
			{
				CMS896AStn::MotionStop(BL_AXIS_LOWER, &m_stBLAxis_Lower);
				Lower_Sync();
				return FALSE;
			}

			Sleep(10);
		}
	}

    return FALSE;
}

// ==========================================================================================================
//				Theta Gripper Move Related Function
// ==========================================================================================================
INT CBinLoader::Theta_Home()
{
	INT nResult	= gnOK;

	try
	{
		if (CMS896AStn::MotionReportErrorStatus(BL_AXIS_THETA, &m_stBLAxis_Theta) != 0)
		{
			CMS896AStn::MotionClearError(BL_AXIS_THETA, &m_stBLAxis_Theta);
		}

		//nResult = CMS896AStn::MotionCommutateServo(BL_AXIS_THETA, &m_stBLAxis_Theta);	
		if (nResult == gnOK)
		{
			if ( CMS896AStn::MotionIsPowerOn(BL_AXIS_THETA, &m_stBLAxis_Theta) == FALSE )
			{
				CMS896AStn::MotionPowerOn(BL_AXIS_THETA, &m_stBLAxis_Theta);
			}

			//CMS896AStn::MotionSelectSearchProfile(BL_AXIS_THETA, "spfBinLoaderThetaIndex", &m_stBLAxis_Theta);
			//CMS896AStn::MotionSearch(BL_AXIS_THETA, 1, SFM_WAIT, &m_stBLAxis_Theta, "spfBinLoaderThetaIndex");
			nResult = CMS896AStn::MotionMoveHome(BL_AXIS_THETA, 1, 0, &m_stBLAxis_Theta);

			
			if ( (CMS896AStn::MotionReportErrorStatus(BL_AXIS_THETA, &m_stBLAxis_Theta) == 0) && 
					CMS896AStn::MotionIsPowerOn(BL_AXIS_THETA, &m_stBLAxis_Theta) )
			{
				Sleep(100);
				CMS896AStn::MotionSetPosition(BL_AXIS_THETA, 0, &m_stBLAxis_Theta);
			}

			m_bHome_Theta = TRUE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(BL_AXIS_THETA, &m_stBLAxis_Theta);
		CMS896AStn::MotionClearError(BL_AXIS_THETA, &m_stBLAxis_Theta);
		nResult = gnNOTOK; 
	}

	return nResult;
}

INT CBinLoader::Theta_SlowMoveTo(INT nPos, INT nMode)
{
	INT nResult = MotorMoveTo(m_bSel_Theta, BL_AXIS_THETA, "mpfBinLoaderThetaSlow", &m_stBLAxis_Theta,
							  nPos, nMode, m_bHome_Theta, TRUE);
	return nResult;
}

INT CBinLoader::Theta_MoveTo(INT nPos, INT nMode)
{
	INT nResult = MotorMoveTo(m_bSel_Theta, BL_AXIS_THETA, "mpfBinLoaderTheta", &m_stBLAxis_Theta,
							  nPos, nMode, m_bHome_Theta, TRUE);
	return nResult;
}

INT CBinLoader::Theta_Sync()
{
	return MotorSync(m_bSel_Theta, BL_AXIS_THETA, &m_stBLAxis_Theta, "", TRUE, 100000);
}

LONG CBinLoader::CheckIfGripperUnloadAlarm(BOOL bUnload)
{
	LONG lCurrEnc = GetGripperEncoder();
	if (!bUnload)
	{
		return TRUE;
	}
	if (lCurrEnc > 20000)
	{
		SetGripperState(FALSE);
		Sleep(100);
		Upper_Home();
		if (!IsFrameOutOfMgz())
		{
			BL_DEBUGBOX("Gripper1 Jam but OutMgz not trigger after Home, no alarm");
			return FALSE;
		}
	}
	BL_DEBUGBOX("Gripper1 Jam and OutMgz  trigger after Home,alarm");
	return TRUE;
}

LONG CBinLoader::CheckIfGripper2UnloadAlarm(BOOL bUnload)
{
	LONG lCurrEnc = GetGripper2Encoder();
	if (!bUnload)
	{
		return TRUE;
	}
	if (lCurrEnc > 20000)
	{
		SetGripper2State(FALSE);
		Sleep(100);
		Lower_Home();
		if (!IsFrameOutOfMgz())
		{
			BL_DEBUGBOX("Gripper2 Jam but OutMgz not trigger after Home, no alarm");
			return FALSE;
		}
	}
	BL_DEBUGBOX("Gripper2 Jam and OutMgz  trigger after Home,alarm");
	return TRUE;
}


INT CBinLoader::Z_SerachUpperLimitSensor()
{
	INT nResult	= gnOK;
	int nCommRetry = 0;
	LONG lTmpBLZLimit;
	CString szLog;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION
			//Enlarge the softawre limit before Home
			CMS896AStn::MotionSetSoftwareLimit(BL_AXIS_Z, 99999999, -99999999, &m_stBLAxis_Z);
#endif

			if (CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) != 0)
			{
				m_bComm_Z = FALSE;
				CMS896AStn::MotionClearError(BL_AXIS_Z, &m_stBLAxis_Z);
			}

			if (nResult == gnOK)
			{
				if (nResult == gnAMS_OK && CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) == 0)
				{
					CMS896AStn::MotionSelectSearchProfile(BL_AXIS_Z, BL_SP_UPPER_LIMIT_Z, &m_stBLAxis_Z);
					CMS896AStn::MotionSearch(BL_AXIS_Z, 0, SFM_WAIT, &m_stBLAxis_Z, BL_SP_UPPER_LIMIT_Z);

					if ((CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) == 0) && 
						CMS896AStn::MotionIsPowerOn(BL_AXIS_Z, &m_stBLAxis_Z))
					{
						Sleep(50);
					}
					else
					{
						CMS896AStn::MotionPowerOff(BL_AXIS_Z, &m_stBLAxis_Z);
						nResult = Err_BinLoaderZMove;
					}
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_BinLoaderZMove;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BL_AXIS_Z, &m_stBLAxis_Z);
			CMS896AStn::MotionClearError(BL_AXIS_Z, &m_stBLAxis_Z);
			m_bComm_Z = FALSE;
			nResult = Err_BinLoaderZMove;
		}
	}

	return nResult;
}


INT CBinLoader::Z_SerachHomeLimitSensor()
{
	INT nResult	= gnOK;
	int nCommRetry = 0;
	LONG lTmpBLZLimit;
	CString szLog;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION
			//Enlarge the softawre limit before Home
			CMS896AStn::MotionSetSoftwareLimit(BL_AXIS_Z, 99999999, -99999999, &m_stBLAxis_Z);
#endif

			if (CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) != 0)
			{
				m_bComm_Z = FALSE;
				CMS896AStn::MotionClearError(BL_AXIS_Z, &m_stBLAxis_Z);
			}

			if (nResult == gnOK)
			{
				if (nResult == gnAMS_OK && CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) == 0)
				{
					CMS896AStn::MotionSelectSearchProfile(BL_AXIS_Z, BL_SP_HOME_SENSOR_Z, &m_stBLAxis_Z);
					CMS896AStn::MotionSearch(BL_AXIS_Z, 0, SFM_WAIT, &m_stBLAxis_Z, BL_SP_HOME_SENSOR_Z);

					if ((CMS896AStn::MotionReportErrorStatus(BL_AXIS_Z, &m_stBLAxis_Z) == 0) && 
						CMS896AStn::MotionIsPowerOn(BL_AXIS_Z, &m_stBLAxis_Z))
					{
						Sleep(50);
					}
					else
					{
						CMS896AStn::MotionPowerOff(BL_AXIS_Z, &m_stBLAxis_Z);
						nResult = Err_BinLoaderZMove;
					}
				}
				else if (nResult == gnNOTOK)
				{
					nResult = Err_BinLoaderZMove;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BL_AXIS_Z, &m_stBLAxis_Z);
			CMS896AStn::MotionClearError(BL_AXIS_Z, &m_stBLAxis_Z);
			m_bComm_Z = FALSE;
			nResult = Err_BinLoaderZMove;
		}
	}

	return nResult;
}