/////////////////////////////////////////////////////////////////
// NL_Move.cpp : interface of the CNVCLoader class
//
//	Description:
//		
//	Date:		26 June 2020
//	Revision:	1.00
//
//	By:	Andrew Ng		
//				
//	Copyright @ ASM Pacific Technology Ltd., .
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NVCLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL CNVCLoader::IsAllMotorsEnable()
{
	BOOL bMotorsOn = FALSE;	
	
	if (!m_fHardware || m_bDisableNL)
	{	
		return TRUE;
	}

	bMotorsOn = (X_IsPowerOn() && Y_IsPowerOn() && Z1_IsPowerOn() && Z2_IsPowerOn());
	
	if (!bMotorsOn)
	{
		CString szLog;
		szLog.Format("NL IsAllMotorsEnable FAIL: %d %d %d %d", X_IsPowerOn(), Y_IsPowerOn(), Z1_IsPowerOn(), Z2_IsPowerOn());
		SetMotionCE(TRUE, szLog);
	}
	return bMotorsOn;
}

BOOL CNVCLoader::HomeNLModule()
{
	//AfxMessageBox("HomeNLModule: homing Z1 ...", MB_SYSTEMMODAL);
	Z1_Home();
	
	//AfxMessageBox("HomeNLModule: homing Z2 ...", MB_SYSTEMMODAL);
	Z2_Home();
	
	//AfxMessageBox("HomeNLModule: homing Y ...", MB_SYSTEMMODAL);
	Y_Home();
	
	//AfxMessageBox("HomeNLModule: homing X ...", MB_SYSTEMMODAL);
	X_Home();

	return TRUE;
}

BOOL CNVCLoader::InitMotorSwLimits()
{
	//LONG lMinPos = 0, lMaxPos = 0;
	m_lLLimit_X = GetChannelInformation(MS896A_CFG_CH_NVCLOADER_X, MS896A_CFG_CH_MIN_DISTANCE);
	m_lHLimit_X = GetChannelInformation(MS896A_CFG_CH_NVCLOADER_X, MS896A_CFG_CH_MAX_DISTANCE);
	
	m_lLLimit_Y = GetChannelInformation(MS896A_CFG_CH_NVCLOADER_Y, MS896A_CFG_CH_MIN_DISTANCE);
	m_lHLimit_Y = GetChannelInformation(MS896A_CFG_CH_NVCLOADER_Y, MS896A_CFG_CH_MAX_DISTANCE);
	
	m_lLLimit_Z1 = GetChannelInformation(MS896A_CFG_CH_NVCLOADER_Z1, MS896A_CFG_CH_MIN_DISTANCE);
	m_lHLimit_Z1 = GetChannelInformation(MS896A_CFG_CH_NVCLOADER_Z1, MS896A_CFG_CH_MAX_DISTANCE);
	
	m_lLLimit_Z2 = GetChannelInformation(MS896A_CFG_CH_NVCLOADER_Z2, MS896A_CFG_CH_MIN_DISTANCE);
	m_lHLimit_Z2 = GetChannelInformation(MS896A_CFG_CH_NVCLOADER_Z2, MS896A_CFG_CH_MAX_DISTANCE);

	return TRUE;
}

BOOL CNVCLoader::IsWithinXLimit(CONST LONG lPosX)
{
	if ( (m_lLLimit_X == 0) && (m_lHLimit_X == 0) )
	{
		return TRUE;	//limits not set in Machine MSD
	}
	if ( (m_lLLimit_X > lPosX) || (lPosX > m_lHLimit_X) )
	{
		CString szErr;
		szErr.Format("NL: IsWithinXLimit - X limit out of range: (%ld < %ld < %ld)", m_lLLimit_X, lPosX, m_lHLimit_X);
		SetErrorMessage(szErr);
		return FALSE;
	}
	return TRUE;
}

BOOL CNVCLoader::IsWithinYLimit(CONST LONG lPosY)
{
	if ( (m_lLLimit_Y == 0) && (m_lHLimit_Y == 0) )
	{
		return TRUE;	//limits not set in Machine MSD
	}
	if ( (m_lLLimit_Y > lPosY) || (lPosY > m_lHLimit_Y) )
	{
		CString szErr;
		szErr.Format("NL: IsWithinYLimit - Y limit out of range: (%ld < %ld < %ld)", m_lLLimit_Y, lPosY, m_lHLimit_Y);
		SetErrorMessage(szErr);
		return FALSE;
	}
	return TRUE;
}

BOOL CNVCLoader::IsWithinZ1Limit(CONST LONG lPosZ)
{
	if ( (m_lLLimit_Z1 == 0) && (m_lHLimit_Z1 == 0) )
	{
		return TRUE;	//limits not set in Machine MSD
	}
	if ( (m_lLLimit_Z1 > lPosZ) || (lPosZ > m_lHLimit_Z1) )
	{
		CString szErr;
		szErr.Format("NL: IsWithinZ1Limit - Z1 limit out of range: (%ld < %ld < %ld)", m_lLLimit_Z1, lPosZ, m_lHLimit_Z1);
		SetErrorMessage(szErr);
		return FALSE;
	}
	return TRUE;
}

BOOL CNVCLoader::IsWithinZ2Limit(CONST LONG lPosZ)
{
	if ( (m_lLLimit_Z2 == 0) && (m_lHLimit_Z2 == 0) )
	{
		return TRUE;	//limits not set in Machine MSD
	}
	if ( (m_lLLimit_Z2 > lPosZ) || (lPosZ > m_lHLimit_Z2) )
	{
		CString szErr;
		szErr.Format("NL: IsWithinZ2Limit - Z2 limit out of range: (%ld < %ld < %ld)", m_lLLimit_Z2, lPosZ, m_lHLimit_Z2);
		SetErrorMessage(szErr);
		return FALSE;
	}
	return TRUE;
}

BOOL CNVCLoader::IsWithinXYLimit(CONST LONG lPosX, CONST LONG lPosY)
{
	if ( (m_lLLimit_X == 0) && (m_lHLimit_X == 0) )
	{
		return TRUE;	//limits not set in Machine MSD
	}
	if ( (m_lLLimit_Y == 0) && (m_lHLimit_Y == 0) )
	{
		return TRUE;	//limits not set in Machine MSD
	}

	if ( (m_lLLimit_X > lPosX) || (lPosX > m_lHLimit_X) )
	{
		CString szErr;
		szErr.Format("NL: IsWithinXYLimit - X limit out of range: (%ld < %ld < %ld)", m_lLLimit_X, lPosX, m_lHLimit_X);
		SetErrorMessage(szErr);
		return FALSE;
	}
	if ( (m_lLLimit_Y > lPosY) || (lPosY > m_lHLimit_Y) )
	{
		CString szErr;
		szErr.Format("NL: IsWithinXYLimit - Y limit out of range: (%ld < %ld < %ld)", m_lLLimit_Y, lPosY, m_lHLimit_Y);
		SetErrorMessage(szErr);
		return FALSE;
	}


	return TRUE;
}

BOOL CNVCLoader::CheckXEncCmdPos()
{
	if (m_bDisableNL)
	{
		return TRUE;
	}
	if (!m_bSel_X)
	{
		return TRUE;
	}
	if (!m_bXYUseEncoder)
	{
		return TRUE;
	}

	LONG lEncX = CMS896AStn::MotionGetEncoderPosition(NL_AXIS_X, 3.2, &m_stNLAxis_X);
	LONG lCmdX = CMS896AStn::MotionGetCommandPosition(NL_AXIS_X, &m_stNLAxis_X);

	if (labs(lEncX - lCmdX) > NL_ENC_TOL_X)
	{
		CString szLog;
		szLog.Format("NVC Loader X Encoder missing step error - Enc=%ld, Cmd=%ld, Tol=%ld",
			lEncX, lCmdX, NL_ENC_TOL_X);
		HmiMessage(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CNVCLoader::CheckYEncCmdPos()
{
	if (m_bDisableNL)
	{
		return TRUE;
	}
	if (!m_bSel_Y)
	{
		return TRUE;
	}
	if (!m_bXYUseEncoder)
	{
		return TRUE;
	}

	LONG lEncY = CMS896AStn::MotionGetEncoderPosition(NL_AXIS_Y, 3.2, &m_stNLAxis_Y);
	LONG lCmdY = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Y, &m_stNLAxis_Y);

	if (labs(lEncY - lCmdY) > NL_ENC_TOL_Y)
	{
		CString szLog;
		szLog.Format("NVC Loader Y Encoder missing step error - Enc=%ld, Cmd=%ld, Tol=%ld",
			lEncY, lCmdY, NL_ENC_TOL_Y);
		HmiMessage(szLog);
		return FALSE;
	}

	return TRUE;
}

INT CNVCLoader::X_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(NL_AXIS_X, &m_stNLAxis_X) != 0)
			{
				CMS896AStn::MotionClearError(NL_AXIS_X, &m_stNLAxis_X);
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(NL_AXIS_X, 1, 0, &m_stNLAxis_X);

				if ((nResult == gnAMS_OK) && 
					(CMS896AStn::MotionReportErrorStatus(NL_AXIS_X, &m_stNLAxis_X) == 0) )
				{
					Sleep(500);
					CMS896AStn::MotionSetPosition(NL_AXIS_X, 0, &m_stNLAxis_X);
					m_bHome_X = TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					nResult = gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_X, &m_stNLAxis_X);
			CMS896AStn::MotionClearError(NL_AXIS_X, &m_stNLAxis_X);
			nResult = gnNOTOK;
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


INT CNVCLoader::Y_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(NL_AXIS_Y, &m_stNLAxis_Y) != 0)
			{
				CMS896AStn::MotionClearError(NL_AXIS_Y, &m_stNLAxis_Y);
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(NL_AXIS_Y, 1, 0, &m_stNLAxis_Y);

				if ((nResult == gnAMS_OK) && 
					(CMS896AStn::MotionReportErrorStatus(NL_AXIS_Y, &m_stNLAxis_Y) == 0) )
				{
					Sleep(500);
					CMS896AStn::MotionSetPosition(NL_AXIS_Y, 0, &m_stNLAxis_Y);
					m_bHome_Y = TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					nResult = gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Y, &m_stNLAxis_Y);
			CMS896AStn::MotionClearError(NL_AXIS_Y, &m_stNLAxis_Y);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Y)
		{
			Sleep(100);
			m_bHome_Y	= TRUE;
		}
	}

	return nResult;
}


INT CNVCLoader::Z1_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Z1)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(NL_AXIS_Z1, &m_stNLAxis_Z1) != 0)
			{
				CMS896AStn::MotionClearError(NL_AXIS_Z1, &m_stNLAxis_Z1);
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(NL_AXIS_Z1, 1, 0, &m_stNLAxis_Z1);

				if ((nResult == gnAMS_OK) && 
					(CMS896AStn::MotionReportErrorStatus(NL_AXIS_Z1, &m_stNLAxis_Z1) == 0) )
				{
					Sleep(500);
					CMS896AStn::MotionSetPosition(NL_AXIS_Z1, 0, &m_stNLAxis_Z1);
					m_bHome_Z1 = TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					nResult = gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z1, &m_stNLAxis_Z1);
			CMS896AStn::MotionClearError(NL_AXIS_Z1, &m_stNLAxis_Z1);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Z1)
		{
			Sleep(100);
			m_bHome_Z1	= TRUE;
		}
	}

	return nResult;
}


INT CNVCLoader::Z2_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Z2)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(NL_AXIS_Z2, &m_stNLAxis_Z2) != 0)
			{
				CMS896AStn::MotionClearError(NL_AXIS_Z2, &m_stNLAxis_Z2);
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(NL_AXIS_Z2, 1, 0, &m_stNLAxis_Z2);

				if ((nResult == gnAMS_OK) && 
					(CMS896AStn::MotionReportErrorStatus(NL_AXIS_Z2, &m_stNLAxis_Z2) == 0) )
				{
					Sleep(500);
					CMS896AStn::MotionSetPosition(NL_AXIS_Z2, 0, &m_stNLAxis_Z2);
					m_bHome_Z2 = TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					nResult = gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z2, &m_stNLAxis_Z2);
			CMS896AStn::MotionClearError(NL_AXIS_Z2, &m_stNLAxis_Z2);
			nResult = gnNOTOK;
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

BOOL CNVCLoader::X_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bSel_X == FALSE)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(NL_AXIS_X, &m_stNLAxis_X);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CNVCLoader::Y_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bSel_Y == FALSE)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(NL_AXIS_Y, &m_stNLAxis_Y);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CNVCLoader::Z1_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bSel_Z1 == FALSE)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(NL_AXIS_Z1, &m_stNLAxis_Z1);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CNVCLoader::Z2_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bSel_Z2 == FALSE)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(NL_AXIS_Z2, &m_stNLAxis_Z2);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CNVCLoader::X_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_X)
	{
		try
		{
			if (bOn == TRUE)
			{
				if ( CMS896AStn::MotionIsPowerOn(NL_AXIS_X, &m_stNLAxis_X) == FALSE )
				{
					CMS896AStn::MotionPowerOn(NL_AXIS_X, &m_stNLAxis_X);
					//m_bIsPowerOn_X = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(NL_AXIS_X, &m_stNLAxis_X) == TRUE )
				{
					CMS896AStn::MotionPowerOff(NL_AXIS_X, &m_stNLAxis_X);
					//m_bIsPowerOn_X = FALSE;
					m_bHome_X = FALSE;
				}
			}
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_X, &m_stNLAxis_X);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CNVCLoader::Y_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (bOn == TRUE)
			{
				if ( CMS896AStn::MotionIsPowerOn(NL_AXIS_Y, &m_stNLAxis_Y) == FALSE )
				{
					CMS896AStn::MotionPowerOn(NL_AXIS_Y, &m_stNLAxis_Y);
					//m_bIsPowerOn_X = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(NL_AXIS_Y, &m_stNLAxis_Y) == TRUE )
				{
					CMS896AStn::MotionPowerOff(NL_AXIS_Y, &m_stNLAxis_Y);
					//m_bIsPowerOn_X = FALSE;
					m_bHome_Y = FALSE;
				}
			}
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Y, &m_stNLAxis_Y);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CNVCLoader::Z1_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Z1)
	{
		try
		{
			if (bOn == TRUE)
			{
				if ( CMS896AStn::MotionIsPowerOn(NL_AXIS_Z1, &m_stNLAxis_Z1) == FALSE )
				{
					CMS896AStn::MotionPowerOn(NL_AXIS_Z1, &m_stNLAxis_Z1);
					//m_bIsPowerOn_X = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(NL_AXIS_Z1, &m_stNLAxis_Z1) == TRUE )
				{
					CMS896AStn::MotionPowerOff(NL_AXIS_Z1, &m_stNLAxis_Z1);
					//m_bIsPowerOn_X = FALSE;
					m_bHome_Z1 = FALSE;
				}
			}
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z1, &m_stNLAxis_Z1);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CNVCLoader::Z2_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bSel_Z2)
	{
		try
		{
			if (bOn == TRUE)
			{
				if ( CMS896AStn::MotionIsPowerOn(NL_AXIS_Z2, &m_stNLAxis_Z2) == FALSE )
				{
					CMS896AStn::MotionPowerOn(NL_AXIS_Z2, &m_stNLAxis_Z2);
					//m_bIsPowerOn_X = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(NL_AXIS_Z2, &m_stNLAxis_Z2) == TRUE )
				{
					CMS896AStn::MotionPowerOff(NL_AXIS_Z2, &m_stNLAxis_Z2);
					//m_bIsPowerOn_X = FALSE;
					m_bHome_Z2 = FALSE;
				}
			}
 		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z2, &m_stNLAxis_Z2);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CNVCLoader::X_Profile(INT nProfile)		
{
	INT nResult = gnNOTOK;

	if (m_fHardware && m_bSel_X)
	{
		CString szProfile = "";

		try
		{
			switch (nProfile)
			{
			case NL_SLOW_PROF:
			case NL_NORMAL_PROF:
			case NL_FAST_PROF:
				szProfile = "mpfNVCLoaderXDefault";
				break;

			default:
				break;
			}

			if (szProfile != "")
			{
				CMS896AStn::MotionSelectProfile(NL_AXIS_X, szProfile, &m_stNLAxis_X);
				nResult = gnOK;
			}			
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_X, &m_stNLAxis_X);
			nResult = gnNOTOK;
		}
	}

	return nResult;
}

INT CNVCLoader::Y_Profile(INT nProfile)	
{
	INT nResult = gnNOTOK;

	if (m_fHardware && m_bSel_Y)
	{
		CString szProfile = "";

		try
		{
			switch (nProfile)
			{
			case NL_SLOW_PROF:
			case NL_NORMAL_PROF:
			case NL_FAST_PROF:
				szProfile = "mpfNVCLoaderYDefault";
				break;

			default:
				break;
			}

			if (szProfile != "")
			{
				CMS896AStn::MotionSelectProfile(NL_AXIS_Y, szProfile, &m_stNLAxis_Y);
				nResult = gnOK;
			}			
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Y, &m_stNLAxis_Y);
			nResult = gnNOTOK;
		}
	}

	return nResult;
}

INT CNVCLoader::Z1_Profile(INT nProfile)	
{
	INT nResult = gnNOTOK;

	if (m_fHardware && m_bSel_Z1)
	{
		CString szProfile = "";

		try
		{
			switch (nProfile)
			{
			case NL_SLOW_PROF:
			case NL_NORMAL_PROF:
			case NL_FAST_PROF:
				szProfile = "mpfNVCLoaderZDefault";
				break;

			default:
				break;
			}

			if (szProfile != "")
			{
				CMS896AStn::MotionSelectProfile(NL_AXIS_Z1, szProfile, &m_stNLAxis_Z1);
				nResult = gnOK;
			}			
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z1, &m_stNLAxis_Z1);
			nResult = gnNOTOK;
		}
	}

	return nResult;
}

INT CNVCLoader::Z2_Profile(INT nProfile)	
{
	INT nResult = gnNOTOK;

	if (m_fHardware && m_bSel_Z2)
	{
		CString szProfile = "";

		try
		{
			switch (nProfile)
			{
			case NL_SLOW_PROF:
			case NL_NORMAL_PROF:
			case NL_FAST_PROF:
				szProfile = "mpfNVCLoaderZDefault";
				break;

			default:
				break;
			}

			if (szProfile != "")
			{
				CMS896AStn::MotionSelectProfile(NL_AXIS_Z2, szProfile, &m_stNLAxis_Z2);
				nResult = gnOK;
			}			
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z2, &m_stNLAxis_Z2);
			nResult = gnNOTOK;
		}
	}

	return nResult;
}

INT CNVCLoader::X_Move(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		try
		{
			if (!X_IsPowerOn())
			{
				//SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				nResult = gnNOTOK;
			}
			if (!m_bHome_X)
			{
				nResult = gnNOTOK;
			}
			
			if (nResult == gnOK)
			{				
				//CMS896AStn::MotionMoveTo(NL_AXIS_X, nPos, SFM_NOWAIT, &m_stNLAxis_X);
				CMS896AStn::MotionMove(NL_AXIS_X, nPos, SFM_NOWAIT, &m_stNLAxis_X);

				if (nMode == SFM_WAIT)
				{
					if (X_Sync() != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
						nResult = gnNOTOK;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_X, &m_stNLAxis_X);
			CMS896AStn::MotionClearError(NL_AXIS_X, &m_stNLAxis_X);
			m_bHome_X = FALSE;
			SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(10);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}

INT CNVCLoader::Y_Move(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (!Y_IsPowerOn())
			{
				//SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				nResult = gnNOTOK;
			}
			if (!m_bHome_Y)
			{
				nResult = gnNOTOK;
			}
			
			if (nResult == gnOK)
			{				
				//CMS896AStn::MotionMoveTo(NL_AXIS_Y, nPos, SFM_NOWAIT, &m_stNLAxis_Y);
				CMS896AStn::MotionMove(NL_AXIS_Y, nPos, SFM_NOWAIT, &m_stNLAxis_Y);

				if (nMode == SFM_WAIT)
				{
					if (Y_Sync() != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
						nResult = gnNOTOK;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Y, &m_stNLAxis_Y);
			CMS896AStn::MotionClearError(NL_AXIS_Y, &m_stNLAxis_Y);
			m_bHome_Y = FALSE;
			//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Y)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(10);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}

INT CNVCLoader::Z1_Move(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Z1)
	{
		try
		{
			if (!Z1_IsPowerOn())
			{
				//SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				nResult = gnNOTOK;
			}
			if (!m_bHome_Z1)
			{
				nResult = gnNOTOK;
			}
			
			if (nResult == gnOK)
			{				
				//CMS896AStn::MotionMoveTo(NL_AXIS_Z1, nPos, SFM_NOWAIT, &m_stNLAxis_Z1);
				CMS896AStn::MotionMove(NL_AXIS_Z1, nPos, SFM_NOWAIT, &m_stNLAxis_Z1);

				if (nMode == SFM_WAIT)
				{
					if (Z1_Sync() != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
						nResult = gnNOTOK;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z1, &m_stNLAxis_Z1);
			CMS896AStn::MotionClearError(NL_AXIS_Z1, &m_stNLAxis_Z1);
			m_bHome_Z1 = FALSE;
			//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Z1)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(10);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}

INT CNVCLoader::Z2_Move(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Z2)
	{
		try
		{
			if (!Z2_IsPowerOn())
			{
				//SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				nResult = gnNOTOK;
			}
			if (!m_bHome_Z2)
			{
				nResult = gnNOTOK;
			}
			
			if (nResult == gnOK)
			{				
				//CMS896AStn::MotionMoveTo(NL_AXIS_Z2, nPos, SFM_NOWAIT, &m_stNLAxis_Z2);
				CMS896AStn::MotionMove(NL_AXIS_Z2, nPos, SFM_NOWAIT, &m_stNLAxis_Z2);

				if (nMode == SFM_WAIT)
				{
					if (Z2_Sync() != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
						nResult = gnNOTOK;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z2, &m_stNLAxis_Z2);
			CMS896AStn::MotionClearError(NL_AXIS_Z2, &m_stNLAxis_Z2);
			m_bHome_Z2 = FALSE;
			//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Z2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(10);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}

INT CNVCLoader::X_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_X)
	{
		try
		{
			if (!X_IsPowerOn())
			{
				//SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				nResult = gnNOTOK;
			}
			if (!m_bHome_X)
			{
				nResult = gnNOTOK;
			}
			
			if (nResult == gnOK)
			{				
				CMS896AStn::MotionMoveTo(NL_AXIS_X, nPos, SFM_NOWAIT, &m_stNLAxis_X);

				if (nMode == SFM_WAIT)
				{
					if (X_Sync() != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
						nResult = gnNOTOK;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_X, &m_stNLAxis_X);
			CMS896AStn::MotionClearError(NL_AXIS_X, &m_stNLAxis_X);
			m_bHome_X = FALSE;
			SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_X)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(10);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}

INT CNVCLoader::Y_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		try
		{
			if (!Y_IsPowerOn())
			{
				//SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				nResult = gnNOTOK;
			}
			if (!m_bHome_Y)
			{
				nResult = gnNOTOK;
			}
			
			if (nResult == gnOK)
			{				
				CMS896AStn::MotionMoveTo(NL_AXIS_Y, nPos, SFM_NOWAIT, &m_stNLAxis_Y);

				if (nMode == SFM_WAIT)
				{
					if (Y_Sync() != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
						nResult = gnNOTOK;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Y, &m_stNLAxis_Y);
			CMS896AStn::MotionClearError(NL_AXIS_Y, &m_stNLAxis_Y);
			m_bHome_Y = FALSE;
			//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Y)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(10);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}

INT CNVCLoader::XY_MoveTo(INT nPosX, INT nPosY, INT nMode)
{
	INT nResult = gnOK;
	CString	szCurrentAxis = "";						

	if (m_fHardware && m_bSel_X && m_bSel_Y)
	{
		if ( IsMotionCE() == TRUE )
		{
			SetErrorMessage("NL: Critical error detected in XY MoveTo");
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			if (!m_bHome_X || !m_bHome_Y)
			{
				SetErrorMessage("NL: XY detected not HOME in XY MoveTo");	
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				szCurrentAxis = NL_AXIS_Y;
				CMS896AStn::MotionMoveTo(NL_AXIS_Y, nPosY, SFM_NOWAIT, &m_stNLAxis_Y);
				
				Sleep(10);

				szCurrentAxis = NL_AXIS_X;
				CMS896AStn::MotionMoveTo(NL_AXIS_X, nPosX, SFM_NOWAIT, &m_stNLAxis_X);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(NL_AXIS_X, 5000, &m_stNLAxis_X)) != gnOK)	
					{
						SetErrorMessage("NL: MotionSync X error in XY MoveTo");
						nResult	= gnNOTOK;
					}

					if ((nResult = CMS896AStn::MotionSync(NL_AXIS_Y, 5000, &m_stNLAxis_Y)) != gnOK)
					{
						SetErrorMessage("NL: MotionSync Y error in XY MoveTo");
						nResult = gnNOTOK;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			if (szCurrentAxis == NL_AXIS_X)
			{
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stNLAxis_X);
				m_bHome_X	= FALSE;
			}
			else
			{
				CMS896AStn::MotionCheckResult(szCurrentAxis, &m_stNLAxis_Y);
				m_bHome_Y	= FALSE;
			}

			nResult	= gnNOTOK;
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
		SetMotionCE(TRUE, "NL XY Move Failure(XY_MoveTo)");
	}

	return nResult;
}

INT CNVCLoader::Z1_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Z1)
	{
		try
		{
			if (!Z1_IsPowerOn())
			{
				//SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				nResult = gnNOTOK;
			}
			if (!m_bHome_Z1)
			{
				nResult = gnNOTOK;
			}
			
			if (nResult == gnOK)
			{				
				CMS896AStn::MotionMoveTo(NL_AXIS_Z1, nPos, SFM_NOWAIT, &m_stNLAxis_Z1);

				if (nMode == SFM_WAIT)
				{
					if (Z1_Sync() != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
						nResult = gnNOTOK;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z1, &m_stNLAxis_Z1);
			CMS896AStn::MotionClearError(NL_AXIS_Z1, &m_stNLAxis_Z1);
			m_bHome_Z1 = FALSE;
			//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Z1)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(10);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}

INT CNVCLoader::Z2_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSel_Z2)
	{
		try
		{
			if (!Z2_IsPowerOn())
			{
				//SetAlert_Red_Yellow(IDS_BL_Z_NOT_HOMED);
				nResult = gnNOTOK;
			}
			if (!m_bHome_Z2)
			{
				nResult = gnNOTOK;
			}
			
			if (nResult == gnOK)
			{				
				CMS896AStn::MotionMoveTo(NL_AXIS_Z2, nPos, SFM_NOWAIT, &m_stNLAxis_Z2);

				if (nMode == SFM_WAIT)
				{
					if (Z2_Sync() != gnOK)
					{
						//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
						nResult = gnNOTOK;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z2, &m_stNLAxis_Z2);
			CMS896AStn::MotionClearError(NL_AXIS_Z2, &m_stNLAxis_Z2);
			m_bHome_Z2 = FALSE;
			//SetAlert_Red_Yellow(IDS_BL_Z_MOVE_ERR);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (m_bSel_Z2)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(10);
			}
			else
			{
				Sleep(1);
			}
		}
	}
	return nResult;
}

INT	CNVCLoader::X_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_X)
	{
		if (IsMotionCE())
		{
			return gnNOTOK;
		}

		try
		{
			CMS896AStn::MotionSync(NL_AXIS_X, 5000, &m_stNLAxis_X);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_X, &m_stNLAxis_X);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}

INT	CNVCLoader::Y_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_Y)
	{
		if (IsMotionCE())
		{
			return gnNOTOK;
		}

		try
		{
			CMS896AStn::MotionSync(NL_AXIS_Y, 5000, &m_stNLAxis_Y);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Y, &m_stNLAxis_Y);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}

INT	CNVCLoader::Z1_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_Z1)
	{
		if (IsMotionCE())
		{
			return gnNOTOK;
		}

		try
		{
			CMS896AStn::MotionSync(NL_AXIS_Z1, 3000, &m_stNLAxis_Z1);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z1, &m_stNLAxis_Z1);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}

INT	CNVCLoader::Z2_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_Z2)
	{
		if (IsMotionCE())
		{
			return gnNOTOK;
		}

		try
		{
			CMS896AStn::MotionSync(NL_AXIS_Z2, 3000, &m_stNLAxis_Z2);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(NL_AXIS_Z2, &m_stNLAxis_Z2);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}



