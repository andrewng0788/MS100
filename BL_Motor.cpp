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

//================================================================
// Function Name: 		IsMotorPowerOn
// Input arguments:		szAxis, pAxisInfo
// Output arguments:	None
// Description:   		Check whether Motor is on or not
// Return:				TRUE -- OK, FALSE -- Failure
// Remarks:				None
//================================================================
BOOL CBinLoader::IsMotorPowerOn(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	if (m_bDisableBL)
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(szAxis, pAxisInfo);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

//================================================================
// Function Name: 		SetMotorPower
// Input arguments:		szAxis, pAxisInfo, bPowerOn 
// Output arguments:	None
// Description:   		Turn On/Off Motor
// Return:				TRUE -- OK, FALSE -- Failure
// Remarks:				None
//================================================================
INT CBinLoader::SetMotorPower(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, BOOL bPowerOn)
{
	INT nResult = gnOK;

	if (m_bDisableBL)
	{
		return nResult;
	}

	return CMS896AStn::SetMotorPower(szAxis, pAxisInfo, bPowerOn);
}

//================================================================
// Function Name: 		GetEncoder
// Input arguments:		szAxis, pAxisInfo, dResolution 
// Output arguments:	None
// Description:   		Get the encoder of motor
// Return:				Encoder value
// Remarks:				None
//================================================================
LONG CBinLoader::GetEncoder(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, const double dResolution)
{
	if (!m_fHardware)
	{
		return 0;
	}
	try
	{
		return CMS896AStn::MotionGetEncoderPosition(szAxis, dResolution, pAxisInfo);
	}
	catch (CAsmException e)
	{
		DisplayException(e);

		CMS896AStn::MotionCheckResult(szAxis, pAxisInfo);
	}
	return 0;
}


//================================================================
// Function Name: 		IsHomeSensorHigh
// Input arguments:		szAxis, pAxisInfo 
// Output arguments:	None
// Description:   		Get the Home Sensor Status of motor
// Return:				the Home Sensor Status 
// Remarks:				None
//================================================================
BOOL CBinLoader::IsHomeSensorHigh(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (!m_fHardware)
	{
		return 0;
	}
	try
	{
		return CMS896AStn::MotionIsHomeSensorHigh(szAxis,	pAxisInfo);
	}
	catch (CAsmException e)
	{
		DisplayException(e);

		CMS896AStn::MotionCheckResult(szAxis, pAxisInfo);
	}
	return 0;
}

//================================================================
// Function Name: 		Gripper_X_Home
// Input arguments:		bSelMotor, szAxis, szProfileID, pAxisInfo, bMotorHomed
// Output arguments:	None
// Description:   		Gripper Home
// Return:				0 -- OK, 1 -- Failure
// Remarks:				None
//================================================================
INT CBinLoader::Gripper_X_Home(const BOOL bSelMotor, const CString &szAxis, const CString& szProfileID, CMSNmAxisInfo *pAxisInfo, BOOL &bMotorHomed)
{
	INT nResult	= gnOK;

	bMotorHomed = FALSE;
	if (m_fHardware && bSelMotor)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(szAxis, pAxisInfo) != 0)
			{
				CMS896AStn::MotionClearError(szAxis, pAxisInfo);
			}

			nResult = CMS896AStn::MotionCommutateServo(szAxis, pAxisInfo);	
			if (nResult == gnOK)
			{
				if ( CMS896AStn::MotionIsPowerOn(szAxis, pAxisInfo) == FALSE )
				{
					CMS896AStn::MotionPowerOn(szAxis, pAxisInfo);
				}
				CMS896AStn::MotionSelectSearchProfile(szAxis, szProfileID, pAxisInfo);
				CMS896AStn::MotionSearch(szAxis, 1, SFM_WAIT, pAxisInfo, szProfileID);
				if ((CMS896AStn::MotionReportErrorStatus(szAxis, pAxisInfo) == 0) && 
					CMS896AStn::MotionIsPowerOn(szAxis, pAxisInfo))
				{
					Sleep(50);
					CMS896AStn::MotionSetPosition(szAxis, 0, pAxisInfo);
					bMotorHomed = TRUE;
				}
				else
				{
					CMS896AStn::MotionPowerOff(szAxis, pAxisInfo);
					nResult = gnNOTOK;
				}
				
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(szAxis, pAxisInfo);
			CMS896AStn::MotionClearError(szAxis, pAxisInfo);
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (bSelMotor)
		{
			Sleep(100);
			bMotorHomed	= TRUE;
		}
	}

	return nResult;
}

BOOL CBinLoader::IsMotorMissingStep(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, CString szProfile)
{
	Sleep(10);
	LONG lEncX = CMS896AStn::MotionGetEncoderPosition(szAxis, pAxisInfo->m_dEncResolution, pAxisInfo);
	LONG lCmdX = CMS896AStn::MotionGetCommandPosition(szAxis, pAxisInfo);

	if (labs(lEncX - lCmdX) > 200)
	{
		Sleep(100);
		lEncX = CMS896AStn::MotionGetEncoderPosition(szAxis, pAxisInfo->m_dEncResolution, pAxisInfo);

		if (labs(lEncX - lCmdX) > 200)
		{
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CBinLoader::HomeGripperMotor(const CString& szAxis)
{
	LONG lRet = gnNOTOK;
	if (szAxis == BL_AXIS_UPPER)
	{
		if (State() != IDLE_Q)
		{
			m_clUpperGripperBuffer.SetTriggerThetaHome(TRUE);
			m_clUpperGripperBuffer.AddContTriggerThetaHomeCounter();
		}
		lRet = Upper_Home();
	}

	if (szAxis == BL_AXIS_LOWER)
	{
		if (State() != IDLE_Q)
		{
			m_clLowerGripperBuffer.SetTriggerThetaHome(TRUE);
			m_clLowerGripperBuffer.AddContTriggerThetaHomeCounter();
		}
		lRet = Lower_Home();
	}

	if (lRet == gnOK)
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CBinLoader::IsReachTriggerThetaHomeLimit(const CString& szAxis)
{
	if (szAxis == BL_AXIS_UPPER)
	{
		if (State() != IDLE_Q)
		{
			return m_clUpperGripperBuffer.IsReachTriggerThetaHomeLimit();
		}
	}

	if (szAxis == BL_AXIS_LOWER)
	{
		if (State() != IDLE_Q)
		{
			return m_clLowerGripperBuffer.IsReachTriggerThetaHomeLimit();
		}
	}

	return FALSE;
}


VOID CBinLoader::ResetContTriggerThetaHomeCounter(const CString& szAxis)
{
	if (szAxis == BL_AXIS_UPPER)
	{
		if (State() != IDLE_Q)
		{
			m_clUpperGripperBuffer.SetContTriggerThetaHomeCounter(0);
		}
	}

	if (szAxis == BL_AXIS_LOWER)
	{
		if (State() != IDLE_Q)
		{
			m_clLowerGripperBuffer.SetContTriggerThetaHomeCounter(0);
		}
	}
}


VOID CBinLoader::DisplayMissingStepError(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, BOOL bIsSync, CString szProfile)
{
	Sleep(10);
	LONG lEncX = CMS896AStn::MotionGetEncoderPosition(szAxis, pAxisInfo->m_dEncResolution, pAxisInfo);
	LONG lCmdX = CMS896AStn::MotionGetCommandPosition(szAxis, pAxisInfo);

	CString szErr;
	szErr.Format("%s motor is missing step - ENC=%ld, CMD=%ld, TOL=%ld, IsSync = %d, Profile = %d",
	(const char*)szAxis, lEncX, lCmdX, 1000, bIsSync, szProfile);
	SetErrorMessage(szErr);
		

	if ((szAxis == BL_AXIS_UPPER)/* || (szAxis == BL_AXIS_LOWER)*/)
	{
		//SetAlert_Msg_Red_Yellow(IDS_BL_GRIPPER_MISSINGSTEP, szErr);
		SetAlert_Msg_Red_Yellow(IDS_BL_UP_GRIPPER_MISSINGSTEP, szErr);
	}
	else if (szAxis == BL_AXIS_LOWER)
	{
		SetAlert_Msg_Red_Yellow(IDS_BL_LOW_GRIPPER_MISSINGSTEP, szErr);
	}
	else if (szAxis == BL_AXIS_THETA)
	{
		SetAlert_Msg_Red_Yellow(IDS_BL_MOTOR_THETA_MISSING_STEP, szErr);
	}
	else if (szAxis == BL_AXIS_Z)
	{
		SetAlert_Msg_Red_Yellow(IDS_BL_MOTOR_Z_MISSING_STEP, szErr);
	}
	else
	{
		SetAlert_Msg_Red_Yellow(IDS_BL_GRIPPER_MISSINGSTEP, szErr);
	}
	//HmiMessage_Red_Back(szErr);
}

//================================================================
// Function Name: 		MotorMoveTo
// Input arguments:		bSelMotor, szAxis, szProfileID, pAxisInfo, 
//						nPos, nMode, bMotorHomed, ulTimeout
// Output arguments:	None
// Description:   		Motor Move To
// Return:				0 -- OK, 1 -- Failure
// Remarks:				None
//================================================================
INT CBinLoader::MotorMoveTo(const BOOL bSelMotor, const CString& szAxis, const CString &szProfileID, CMSNmAxisInfo* pAxisInfo,
								 INT nPos, INT nMode, BOOL &bMotorHomed, BOOL bUseEncoder, ULONG ulTimeout,BOOL bMissingStepCheck)
{
	INT nResult			= gnOK;

	nResult = SubMotorMoveTo(bSelMotor, szAxis, szProfileID, pAxisInfo, nPos, nMode, bMotorHomed, bUseEncoder, ulTimeout);
	if (!bMissingStepCheck)
	{
		//BL_DEBUGBOX("zz3");
		return gnOK;
	}
	if (nResult == Err_MotorMissingStep && bMissingStepCheck)
	{
		BOOL bDispMissingStep = TRUE;
		if (!IsReachTriggerThetaHomeLimit(szAxis) && HomeGripperMotor(szAxis))
		{
			nResult = SubMotorMoveTo(bSelMotor, szAxis, szProfileID, pAxisInfo, nPos, nMode, bMotorHomed, bUseEncoder, ulTimeout);
			if (nResult == gnOK)
			{
				bDispMissingStep = FALSE;
			}
		}

		if (bDispMissingStep)
		{
			DisplayMissingStepError(szAxis, pAxisInfo, FALSE, szProfileID);
		}
	}
	else
	{
		ResetContTriggerThetaHomeCounter(szAxis);
	}

	return nResult;
}


//================================================================
// Function Name: 		SubMotorMoveTo
// Input arguments:		bSelMotor, szAxis, szProfileID, pAxisInfo, 
//						nPos, nMode, bMotorHomed, ulTimeout
// Output arguments:	None
// Description:   		Motor Move To
// Return:				0 -- OK, 1 -- Failure
// Remarks:				None
//================================================================
INT CBinLoader::SubMotorMoveTo(const BOOL bSelMotor, const CString& szAxis, const CString &szProfileID, CMSNmAxisInfo* pAxisInfo,
								 INT nPos, INT nMode, BOOL &bMotorHomed, BOOL bUseEncoder, ULONG ulTimeout)
{
	INT nResult			= gnOK;

	if (m_fHardware && bSelMotor)
	{
		try
		{
			if (!bMotorHomed)
			{
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				if (!szProfileID.IsEmpty())
				{
					CMS896AStn::MotionSelectProfile(szAxis, szProfileID, pAxisInfo);
				}
				CMS896AStn::MotionMoveTo(szAxis, nPos, SFM_NOWAIT, pAxisInfo);

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(szAxis, ulTimeout, pAxisInfo)) != gnOK)
					{
						nResult = gnNOTOK;
					}

					if (bUseEncoder)
					{
						if (IsMotorMissingStep(szAxis, pAxisInfo, szProfileID))
						{
							nResult = Err_MotorMissingStep;
						}
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(szAxis, pAxisInfo);
			CMS896AStn::MotionClearError(szAxis, pAxisInfo);

			bMotorHomed = FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (bSelMotor)
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

//================================================================
// Function Name: 		MotorSync
// Input arguments:		bSelMotor, szAxis, szProfileID, pAxisInfo, szProfileID
//						ulTimeout
// Output arguments:	None
// Description:   		Motor Move To
// Return:				0 -- OK, 1 -- Failure
// Remarks:				None
//================================================================
INT CBinLoader::MotorSync(const BOOL bSelMotor, const CString& szAxis, CMSNmAxisInfo* pAxisInfo, const CString szProfileID, BOOL bUseEncoder, ULONG ulTimeout)
{
	INT nResult = gnOK;

	nResult = SubMotorSync(bSelMotor, szAxis, pAxisInfo, bUseEncoder, ulTimeout);
	if (nResult == Err_MotorMissingStep)
	{
		BOOL bDispMissingStep = TRUE;
		if (!IsReachTriggerThetaHomeLimit(szAxis) && HomeGripperMotor(szAxis))
		{
			LONG nPos = CMS896AStn::MotionGetCommandPosition(szAxis, pAxisInfo);
			BOOL bMotorHomed = FALSE;
			nResult = SubMotorMoveTo(bSelMotor, szAxis, szProfileID, pAxisInfo, nPos, SFM_WAIT, bMotorHomed, bUseEncoder, ulTimeout);
			if (nResult == gnOK)
			{
				bDispMissingStep = FALSE;
			}
		}

		if (bDispMissingStep)
		{
			DisplayMissingStepError(szAxis, pAxisInfo, TRUE, "");
		}
	}
	else
	{
		ResetContTriggerThetaHomeCounter(szAxis);
	}

	return nResult;
}


//================================================================
// Function Name: 		SubMotorSync
// Input arguments:		bSelMotor, szAxis, szProfileID, pAxisInfo, 
//						ulTimeout
// Output arguments:	None
// Description:   		Motor Move To
// Return:				0 -- OK, 1 -- Failure
// Remarks:				None
//================================================================
INT CBinLoader::SubMotorSync(const BOOL bSelMotor, const CString& szAxis, CMSNmAxisInfo* pAxisInfo, BOOL bUseEncoder, ULONG ulTimeout)
{
	INT nResult = gnOK;
	if (m_fHardware && bSelMotor)
	{
		try
		{
			CMS896AStn::MotionSync(szAxis, ulTimeout, pAxisInfo);
			if (bUseEncoder)
			{
				if (IsMotorMissingStep(szAxis, pAxisInfo))
				{
					nResult = Err_MotorMissingStep;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(szAxis, pAxisInfo);
			nResult = gnNOTOK;
		}
	}
	return nResult;
}

//===================================================================================
//   Check Bin Loader Motor Is On or Off
//===================================================================================
BOOL CBinLoader::Upper_IsPowerOn()
{
	return IsMotorPowerOn(BL_AXIS_UPPER, &m_stBLAxis_Upper);
}

BOOL CBinLoader::Lower_IsPowerOn()
{
	return IsMotorPowerOn(BL_AXIS_LOWER, &m_stBLAxis_Lower);
}

BOOL CBinLoader::X_IsPowerOn()
{
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A40	//MS50
	{
		return Upper_IsPowerOn();
	}
	return IsMotorPowerOn(BL_AXIS_X, &m_stBLAxis_X);
}

BOOL CBinLoader::X2_IsPowerOn()
{
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return TRUE;
	}
	return Lower_IsPowerOn();
}

BOOL CBinLoader::Z_IsPowerOn()
{
	return IsMotorPowerOn(BL_AXIS_Z, &m_stBLAxis_Z);
}

BOOL CBinLoader::Theta_IsPowerOn()
{
	return IsMotorPowerOn(BL_AXIS_THETA, &m_stBLAxis_Theta);
}


//===================================================================================
//   Turn On/Off Bin Loader Motor
//===================================================================================
//Upper Gripper
INT CBinLoader::SetLoaderXPower(BOOL bSet)
{
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return SetMotorPower(BL_AXIS_UPPER, &m_stBLAxis_Upper, bSet);
	}
	return SetMotorPower(BL_AXIS_X, &m_stBLAxis_X, bSet);
}

//Lower Gripper
INT CBinLoader::SetLoaderX2Power(BOOL bSet)
{
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return SetMotorPower(BL_AXIS_LOWER, &m_stBLAxis_Lower, bSet);
	}
	return TRUE;
}


INT CBinLoader::SetLoaderZPower(BOOL bSet)
{
	return SetMotorPower(BL_AXIS_Z, &m_stBLAxis_Z, bSet);
}


INT CBinLoader::SetLoaderTPower(BOOL bSet)
{
	return SetMotorPower(BL_AXIS_THETA, &m_stBLAxis_Theta, bSet);
}


//===================================================================================
//   Get Bin Loader Motor' Command Position
//===================================================================================
LONG CBinLoader::GetGripperCommandPosition()
{
	if (!m_fHardware)
	{
		return 0;
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return CMS896AStn::MotionGetCommandPosition(BL_AXIS_UPPER, &m_stBLAxis_Upper);
	}
	return CMS896AStn::MotionGetCommandPosition(BL_AXIS_X, &m_stBLAxis_X);
}

//===================================================================================
//   Get Bin Loader Motor' Command Position
//===================================================================================
LONG CBinLoader::GetGripper2CommandPosition()
{
	if (!m_fHardware)
	{
		return 0;
	}

	return CMS896AStn::MotionGetCommandPosition(BL_AXIS_LOWER, &m_stBLAxis_Lower);
}

//===================================================================================
//   Get Bin Loader Motor' Encoder
//===================================================================================
LONG CBinLoader::GetGripperEncoder()
{
	if (!m_fHardware)
	{
		return 0;
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return GetEncoder(BL_AXIS_UPPER, &m_stBLAxis_Upper, m_dGripperXRes);
	}
	return CMS896AStn::MotionGetCommandPosition(BL_AXIS_UPPER, &m_stBLAxis_Upper);
}


LONG CBinLoader::GetGripper2Encoder()
{
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return 0;
	}

	if (!m_fHardware)
	{
		return 0;
	}

	return GetEncoder(BL_AXIS_LOWER, &m_stBLAxis_Lower, m_dGripperXRes);
}


// Get all encoder values from hardware
VOID CBinLoader::GetEncoderValue()
{
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		//For MS50
		//Upper Gripper
		m_lEnc_X = GetEncoder(BL_AXIS_UPPER, &m_stBLAxis_Upper, m_dGripperXRes);

		//Lower Gripper
		m_lEnc_X2 = GetEncoder(BL_AXIS_LOWER, &m_stBLAxis_Lower, m_dGripperXRes);

		//Theta
		m_lEnc_T = GetEncoder(BL_AXIS_THETA, &m_stBLAxis_Theta, m_dThetaXRes/*1.6*/);
	}
	else
	{
		m_lEnc_X = GetGripperEncoder();
	}

	m_lEnc_Z = GetEncoder(BL_AXIS_Z, &m_stBLAxis_Z, 1);
}

//======================================================================================

BOOL CBinLoader::IsGripperMotionComplete()
{
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return (CMS896AStn::MotionIsComplete(BL_AXIS_UPPER, &m_stBLAxis_Upper) == TRUE);
	}
	return (CMS896AStn::MotionIsComplete(BL_AXIS_X, &m_stBLAxis_X) == TRUE);
}

VOID CBinLoader::GripperMotionStop()
{
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		CMS896AStn::MotionStop(BL_AXIS_UPPER, &m_stBLAxis_Upper);
	}
	else
	{
		CMS896AStn::MotionStop(BL_AXIS_X, &m_stBLAxis_X);
	}
}