/////////////////////////////////////////////////////////////////
// WL_LoadFrame.cpp : Common functions of the CWaferLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, November 25, 2004
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
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "math.h"
#include "FileUtil.h"
#include "TakeTime.h"
#include "MS896A.h"
#include "Windows.h"
#include "CTmpChange.h"
#include "MS_SecCommConstant.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Vacuum version
INT CWaferLoader::FilmFrameVacuumReady()
{
	if( m_bFrameOperationInAutoMode==FALSE )
	{
		if( WPR_MoveFocusToSafe()==FALSE )
		{
			return FALSE;
		}
	}

	//Home Motor
	MoveWaferThetaTo(0);
	X_MoveTo(0);

	//Reset Gripper I/O Status
	SetGripperState(WL_OFF);
	SetGripperLevel(WL_OFF);

	//Move bondarm to blow position
	if (BondArmMoveToBlow() == FALSE)
	{
		return FALSE;
	}

	//Move wafer table to unload position
	BOOL bTableStatus = TRUE;	//v3.60	//Production usage

	bTableStatus = MoveWaferTableLoadUnload(m_lUnloadPhyPosX, m_lUnloadPhyPosY, TRUE);	//v4.16T5	//MS100 9Inch

	if (!bTableStatus)
	{
		CString szErr = _T("UNLOAD position hits current wafer limit!");
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		szErr = "WL: Set UNLOAD pos hits wafer limit in FilmFrame VacuumReady()";
		SetErrorMessage(szErr);
		//Re-enabled for MS100P3 WH SanAn
		CMSLogFileUtility::Instance()->WL_LogStatus(szErr);
		return FALSE;
	}

	//Open expander & Gripper down
	SetFrameVacuum(WL_OFF);
	Sleep(800); 
	SetFrameLevel(WL_UP);
	SetGripperLevel(WL_ON);
	Sleep(500);

	return TRUE;
}


INT CWaferLoader::ExpanderVacuumPlatform(BOOL bOpen, BOOL bHotAir)
{
	if (bOpen == TRUE)
	{
		if ( (m_lHotBlowOpenTime > 0) && ( bHotAir == TRUE) )
		{
			SetHotAir(WL_ON);
			Sleep(m_lHotBlowOpenTime);
			SetHotAir(WL_OFF);
		}	
	}
	else
	{
		if ( (m_lHotBlowCloseTime > 0) && ( bHotAir == TRUE) ) 
		{
			SetHotAir(WL_ON);
			Sleep(m_lHotBlowCloseTime);
			SetHotAir(WL_OFF);
		}	
	}

	//Delay for mylar paper cool down
	if (m_lMylarCoolTime > 0)
	{
		Sleep(m_lMylarCoolTime);
	}

	return 1;
}
//end Vacuum

//V2.83T4 (With Sync/ Without Sync Choice)
//Gear Version
INT CWaferLoader::FilmFrameGearReady(INT nMode, BOOL bGripperLevelOn, BOOL bMoveTheta, BOOL bAtReadyPos)
{
	//Home Motor
	if (bMoveTheta == TRUE)
		MoveWaferThetaTo(0);

	if( IsMotionCE() )
	{
		CString szMsg;
		szMsg = "Motin Critical Error in Fileframe Gear Ready";
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		SetErrorMessage(szMsg);
		return FALSE;
	}

	if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)
		X_MoveTo(WL_GRIPPER_SRV_HOME_OFFSET_POS);		//v3.69T1
	else
	{
		if (m_bUseBLAsLoaderZ && bAtReadyPos)
			X_MoveTo(m_lReadyPos_X);
		else
			X_MoveTo(0);
	}

	//Reset Gripper I/O Status
	SetGripperState(WL_OFF);
	SetGripperLevel(WL_OFF);

	//Move bondarm to blow position
	if (BondArmMoveToBlow() == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("BH to BLOW fail in FilmFrame GearReady()");
		SetErrorMessage("WL: BH to BLOW fail in FilmFrame GearReady()");
		return FALSE;
	}
	
	BOOL bTableStatus = TRUE;
	bTableStatus = MoveWaferTableNoCheck(m_lUnloadPhyPosX, m_lUnloadPhyPosY, TRUE, nMode, FALSE);

	if (!bTableStatus)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WT move fail in FilmFrame GearReady()");
		SetErrorMessage("WL: WT move fail in FilmFrame GearReady()");
		return FALSE;
	}

	if (bGripperLevelOn == TRUE)
	{
		SetGripperLevel(WL_ON);
		Sleep(500);
	}

	return TRUE;
}

INT CWaferLoader::ExpanderGearPlatform(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, BOOL bReadBarCode)
{
	BOOL bEnableHotAir = FALSE;
	LONG lRunTime = 0;	
	LONG lHotAirTime = m_lHotBlowOpenTime;	
	LONG lDriveInTime = m_lOpenDriveInTime;

	// If load from gripper and close, check frame exist sensor in expander
	/*
	if (bCheckFrameExist == TRUE && bOpen == FALSE)
	{
		if (IsFrameExist() == FALSE)
		{
			return Err_NoFrameExist;
		}
	}
	*/

	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}

	if (GetExpType() == WL_EXP_NONE)		//v4.39T10	//Knowles MS109
	{
		(*m_psmfSRam)["MS896A"]["Expander Status"] = bOpen;
		m_bExpanderStatus = bOpen;
		return TRUE;
	}

	if (IsExpanderLock() == FALSE)
	{
		SetExpanderLock(WL_ON);
		Sleep(400);

		if (IsExpanderLock() == FALSE)
			return Err_ExpanderLockFailed;
	}

	if ( (IsExpanderOpen() == TRUE) && (bOpen == TRUE) )
	{
		return Err_ExpanderAlreadyOpen;
	}

	if ( (IsExpanderClose() == TRUE) && (bOpen == FALSE) )
	{
		return Err_ExpanderAlreadyClose;
	}

	if (bOpen == WL_DOWN)
	{
		lHotAirTime	= m_lHotBlowCloseTime;
		lDriveInTime = m_lCloseDriveInTime;

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		// If load from gripper and close, check frame exist sensor in expander
		//Check new Frame Exist Sensor (Osram)
#ifndef NU_MOTION
		if( pApp->GetCustomerName()=="OSRAM" && IsBurnIn()==FALSE && bCheckFrameExist )
		{
			BOOL bFrameExisted = IsFrameExist();
			CString szMsg;
			szMsg.Format("wl frame exist gear down = %d", bFrameExisted);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			if( bFrameExisted )
			{
				return Err_NoFrameExist;
			}
		}
#endif
	}

	//Start to acces DC motor
	SetDCMDirecction(!bOpen);	//Open = -ve; Close = +ve
	SetDCMPower(WL_ON);
	lRunTime = 1;

	//Check Hot Air is need or not
	if ( bHotAir == FALSE )
	{
		lHotAirTime = 0;
	}

	if (lHotAirTime > 0)
	{
		bEnableHotAir = TRUE;
		SetHotAir(WL_ON);
	}
	else
	{
		bEnableHotAir = FALSE;
		lHotAirTime = 0;
	}


	//Update Expander Status
	(*m_psmfSRam)["MS896A"]["Expander Status"] = bOpen;
	m_bExpanderStatus = bOpen;

	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_STATUS, (int)m_bExpanderStatus);

	if (bReadBarCode)
	{
		//alex
		m_szBarcodeName.Empty();
		ReadBarcode(&m_szBarcodeName, 100, 2);	// ~0.2sec
	}

	//DC Motor openning or closing
	while(1)
	{
		if ( (IsExpanderOpen() == bOpen) && (IsExpanderClose() == !bOpen) )
		{
			break;
		}

		Sleep(10);	//1);	//v3.93
		lRunTime++;

		//Protect expander for over drive
		if (lRunTime > WL_EXP_TIMEOUT) 
		{
			if (bEnableHotAir == TRUE)
			{
				SetHotAir(WL_OFF);
			}
			SetDCMPower(WL_OFF);
			Sleep(500);
			return Err_ExpanderOpenCloseFailed;
		}

		//Stop Hot Air if hit time
		if ( (bEnableHotAir == TRUE) && (lRunTime >= lHotAirTime) )
		{
			SetHotAir(WL_OFF);
		}
	}

	//Off DCM
	SetDCMPower(WL_OFF);

	//Continue Hot air if necessary
	if (bEnableHotAir == TRUE)
	{
		if (lHotAirTime > lRunTime)
		{
			Sleep(lHotAirTime - lRunTime);
			SetHotAir(WL_OFF);
		}
		else
		{
			SetHotAir(WL_OFF);
		}
	}


	//DriveIn Expander
	if (lDriveInTime > 0)
	{
		SetDCMPower(WL_ON);
		Sleep(lDriveInTime);
		SetDCMPower(WL_OFF);
	}


	//Delay for mylar paper cool down
	if (m_lMylarCoolTime > 0)
	{
		Sleep(m_lMylarCoolTime);
	}

	return TRUE;
}
//end Gear

INT CWaferLoader::ExpanderCylinderMotorPlatform(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, 
												BOOL bReadBarCode)
{
	BOOL bEnableHotAir = FALSE;
	LONG lRunTime = 0;	
	LONG lHotAirTime = m_lHotBlowOpenTime;	
	LONG lDriveInTime = m_lOpenDriveInTime;

	// If load from gripper and close, check frame exist sensor in expander
	/*
	if (bCheckFrameExist == TRUE && bOpen == FALSE)
	{
		if (IsFrameExist() == FALSE)
		{
			return Err_NoFrameExist;
		}
	}
	*/

	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}

	if ( (IsExpanderOpen() == TRUE) && (bOpen == TRUE) )
	{
		return Err_ExpanderAlreadyOpen;
	}

	if ( (IsExpanderClose() == TRUE) && (bOpen == FALSE) )
	{
		return Err_ExpanderAlreadyClose;
	}

	if (bOpen == WL_DOWN)
	{
		lHotAirTime	= m_lHotBlowCloseTime;
		lDriveInTime = m_lCloseDriveInTime;

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		// If load from gripper and close, check frame exist sensor in expander
		//Check new Frame Exist Sensor (Osram)
#ifndef NU_MOTION
		if( pApp->GetCustomerName()=="OSRAM" && IsBurnIn()==FALSE && bCheckFrameExist )
		{
			BOOL bFrameExisted = IsFrameExist();
			CString szMsg;
			szMsg.Format("wl frame exist gear down = %d", bFrameExisted);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			if( bFrameExisted )
			{
				return Err_NoFrameExist;
			}
		}
#endif
	}

	lRunTime = 1;
	SetPushUpTable(bOpen);

	//Check Hot Air is need or not
	if ( bHotAir == FALSE )
	{
		lHotAirTime = 0;
	}

	if (lHotAirTime > 0)
	{
		bEnableHotAir = TRUE;
		SetHotAir(WL_ON);
	}
	else
	{
		bEnableHotAir = FALSE;
		lHotAirTime = 0;
	}


	//Update Expander Status
	(*m_psmfSRam)["MS896A"]["Expander Status"] = bOpen;
	m_bExpanderStatus = bOpen;

	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_STATUS, (int)m_bExpanderStatus);

	if (bReadBarCode)
	{
		//alex
		m_szBarcodeName.Empty();
		ReadBarcode(&m_szBarcodeName, 100, 2);	// ~0.2sec
	}

	//DC Motor openning or closing
	while(1)
	{
		if ( (IsExpanderOpen() == bOpen) && (IsExpanderClose() == !bOpen) )
		{
			break;
		}

		Sleep(10);	//1);	//v3.93
		lRunTime++;

		//Protect expander for over drive
		if (lRunTime > WL_EXP_TIMEOUT) 
		{
			if (bEnableHotAir == TRUE)
			{
				SetHotAir(WL_OFF);
			}
			Sleep(500);
			return Err_ExpanderOpenCloseFailed;
		}

		//Stop Hot Air if hit time
		if ( (bEnableHotAir == TRUE) && (lRunTime >= lHotAirTime) )
		{
			SetHotAir(WL_OFF);
		}
	}

	//Continue Hot air if necessary
	if (bEnableHotAir == TRUE)
	{
		if (lHotAirTime > lRunTime)
		{
			Sleep(lHotAirTime - lRunTime);
			SetHotAir(WL_OFF);
		}
		else
		{
			SetHotAir(WL_OFF);
		}
	}

	//Delay for mylar paper cool down
	if (m_lMylarCoolTime > 0)
	{
		Sleep(m_lMylarCoolTime);
	}

	return TRUE;
}


INT CWaferLoader::ExpanderDCMotorPlatform(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, 
										  LONG lMoveDac, BOOL bReadBarCode, BOOL bCheckCover, BOOL bIsUnload)
{
	BOOL bEnableHotAir = FALSE;
	LONG lRunTime = 0;	
	LONG lDriveInTimeOut = 0;
	
	LONG lDacValue		= lMoveDac;
	LONG lDriveInOffsetZ= m_lExpEncoderOpenOffsetZ;
	LONG lHotAirTime	= m_lHotBlowOpenTime;	
	LONG lDriveInTime	= m_lOpenDriveInTime;
	if ((bOpen == WL_UP) && bIsUnload && (m_lUnloadOpenExtraTime > 0))	//v4.59A17
	{
		lDriveInTime = lDriveInTime + m_lUnloadOpenExtraTime;
	}

	//v4.16T5	//Support of MS100 9Inch
	LONG lMotorDirection = (BOOL) GetChannelInformation(MS896A_CFG_CH_WAFEXPDCMOTOR, MS896A_CFG_CH_MOTOR_DIRECTION);
	if (lMotorDirection<0)
		lDacValue = lDacValue * -1;		//minus DAC value for DC motor to MOVE UP

	if (m_bDisableWL)
	{
		return TRUE;
	}

	if (!m_bIsExpDCMotorExist)
	{
		return TRUE;
	}
	//v4.47T3
	if ( (GetExpType() != WL_EXP_GEAR_NUMOTION_DCMOTOR)	&&	
		 (GetExpType() != WL_EXP_GEAR_DIRECTRING) )	
	{
		return TRUE;
	}

	if (IsExpanderLock() == FALSE)
	{
		SetExpanderLock(WL_ON);
		Sleep(400);

		if (IsExpanderLock() == FALSE)
			return Err_ExpanderLockFailed;
	}

	if ( (IsExpanderOpen() == TRUE) && (bOpen == TRUE) )
	{
		SetExpanderLock(WL_OFF);
		return Err_ExpanderAlreadyOpen;
	}

	if ( (IsExpanderClose() == TRUE) && (bOpen == FALSE) )
	{
		SetExpanderLock(WL_OFF);
		return Err_ExpanderAlreadyClose;
	}

	if (bOpen == WL_DOWN)
	{
		lHotAirTime		= m_lHotBlowCloseTime;
		lDriveInTime	= m_lCloseDriveInTime;
		lDriveInOffsetZ = m_lExpEncoderCloseOffsetZ;

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		// If load from gripper and close, check frame exist sensor in expander
		//Check new Frame Exist Sensor (Osram)
#ifndef NU_MOTION
		if( pApp->GetCustomerName()=="OSRAM" && IsBurnIn()==FALSE && bCheckFrameExist )
		{
			BOOL bFrameExisted = IsFrameExist();
			CString szMsg;
			szMsg.Format("wl frame exist gear down = %d", bFrameExisted);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			if( bFrameExisted )
			{
				return Err_NoFrameExist;
			}
		}
#endif
	}
	else
	{
		lDacValue = lDacValue * -1;		//minus DAC value for DC motor to MOVE UP
	}

CString szTemp;
//szTemp.Format("DC Motor lDriveIn Offset Z = %ld", lDriveInOffsetZ);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);


	//v4.35T3	//Knowles MS109
	if ( (bOpen == WL_DOWN) && bHotAir && (lHotAirTime > 0) )
	{
		SetHotAir(WL_ON);
		Sleep(lHotAirTime);
		SetHotAir(WL_OFF);
	}


	//Update Expander Status
	(*m_psmfSRam)["MS896A"]["Expander Status"] = bOpen;
	m_bExpanderStatus = bOpen;
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_STATUS, (int)m_bExpanderStatus);

	if (bReadBarCode)
	{
		m_szBarcodeName.Empty();
		ReadBarcode(&m_szBarcodeName, 100, 2);	// ~0.2sec
	}

	if (bCheckCover)	//v4.50A20
	{
		SetExpanderLock(WL_OFF);
		CMSLogFileUtility::Instance()->WL_LogStatus("ExpanderDCMotorPlatform is aborted due to COVER OPEN");
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return Err_NoFrameExist;
	}

	//Turn on & move DC Motor
	CMS896AStn::MotionPowerOn(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
	Sleep(500);
	CMS896AStn::OpenDac(&m_stWLExpAxis_Z, lDacValue, 10, 1, WL_EXP_TIMEOUT * 10, 1);


	//DC Motor openning or closing
	while(1)
	{
		if ( (IsExpanderOpen() == bOpen) && (IsExpanderClose() == !bOpen) )
		{
			//v4.38T5	//Knowles MS109 Penang with Exp Z encoder control
			if (m_bExpDCMotorUseEncoder && (lDriveInOffsetZ > 0))
			{
				LONG lZ		= (LONG)(CMS896AStn::MotionGetEncoderPosition(WL_AXIS_EXP_Z, 0.01, &m_stWLExpAxis_Z) * 0.01);
				LONG lCurrZ	= lZ;
				while ( labs(lZ - lCurrZ) < lDriveInOffsetZ )
				{
					Sleep(10);
					lCurrZ	= (LONG)(CMS896AStn::MotionGetEncoderPosition(WL_AXIS_EXP_Z, 0.01, &m_stWLExpAxis_Z) * 0.01);
					lDriveInTimeOut++;
					if (lDriveInTimeOut > 200) 
					{
						CMS896AStn::MotionStop(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
						break;
					}
				}
			}
			else if (lDriveInTime > 0)
			{
				Sleep(lDriveInTime);
			}

			CMS896AStn::MotionStop(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
			break;
		}

		Sleep(10);	//1);	//v3.93
		lRunTime++;

		//Protect expander for over drive
		if (lRunTime > WL_EXP_TIMEOUT) 
		{
			//if (bEnableHotAir == TRUE)
			//{
			//	SetHotAir(WL_OFF);
			//}

			CMS896AStn::MotionStop(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
			Sleep(500);
			CMS896AStn::MotionPowerOff(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
			Sleep(100);
			SetExpanderLock(WL_OFF);
			return Err_ExpanderOpenCloseFailed;
		}

		//Stop Hot Air if hit time
		//if ( (bEnableHotAir == TRUE) && (lRunTime >= lHotAirTime) )
		//{
		//	SetHotAir(WL_OFF);
		//}
	}


	//Off DC Motor
	Sleep(500);
	CMS896AStn::MotionPowerOff(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);


	//v4.35T3	//Knowles MS109
	if ( (bOpen == WL_UP) && bHotAir && (lHotAirTime > 0) )
	{
		SetHotAir(WL_ON);
		Sleep(lHotAirTime);
		SetHotAir(WL_OFF);
	}

	//Delay for mylar paper cool down
	if (m_lMylarCoolTime > 0)
	{
		Sleep(m_lMylarCoolTime);
	}

	SetExpanderLock(WL_OFF);
	return TRUE;
}


INT CWaferLoader::HouseKeeping(BOOL bOnFrameVacuum, BOOL bGripperHitLimit, BOOL bHomeBHT, BOOL bHomeX, BOOL bDownExpanderAlert)
{
	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping - Start HouseKeeping");
	if (bGripperHitLimit == TRUE)
	{
		ResetGripperHitLimit();
	}

	//Off Clip
	Sleep(100);
	SetGripperState(WL_OFF);
	Sleep(100);

	//Move Gripper back to zero
	//v2.93T2
	if (bHomeX == TRUE)
	{
		try 
		{
			//Disable Limit sensor protection
			//BOOL bNeedPowerOn = FALSE;
			CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping - Before Home X");

#ifdef NU_MOTION
			if (!X_IsPowerOn())
			{
				//bNeedPowerOn = TRUE;
				m_bComm_X = FALSE;		//v3.59
				CMS896AStn::MotionClearError(WL_AXIS_X, &m_stWLAxis_X);
				Sleep(100);
				X_Home();
			}
			else
			{
				//v4.40T8	//PLLM MS109
				CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetCustomerName() == "Lumileds")	
					X_Home(SFM_NOWAIT);
				else
					X_Home();
			}
#else
			m_bComm_X = FALSE;
			X_Home();
#endif
		}
		catch(CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
	}


	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping - Before Close Expander");
	//Close expander & Gripper down
	if (GetExpType() == WL_EXP_VACUUM)
	{
		if (bDownExpanderAlert)
		{
			HmiMessage_Red_Yellow("Expander ready to down!");
		}
		SetFrameLevel(WL_DOWN);
		Sleep(500);
		SetFrameAlign(WL_ON);

		if ( m_lVacuumBefore > 0 )
		{
			Sleep(m_lVacuumBefore);
		}

		SetFrameVacuum(bOnFrameVacuum);
		SetFrameAlign(WL_OFF);
		Sleep(500);
	}
	else if ( (GetExpType() != WL_EXP_CYLINDER) && (GetExpType() != WL_EXP_NONE) )
	{
		SetExpanderLock(WL_OFF);
	}


	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping - Before Set Gripper Level");
	SetGripperLevel(WL_OFF);
	if (!bHomeBHT)		//v4.37T13
		Sleep(300);

	if ( bOnFrameVacuum  && (GetExpType() == WL_EXP_VACUUM) )
	{
		LONG lRunTime = 0;

		if (IsMS60())	//v4.46T28
		{
			Sleep(500);
		}
		else
		{
			while(1)
			{
				if ( IsExpanderVacuum() == TRUE )
				{
					break;
				}
				
				Sleep(10);	//v3.93
				lRunTime++;

				if (lRunTime > WL_EXP_TIMEOUT) 
				{
					return FALSE;
				}
			}
		}
		
		if ( m_lVacuumAfter > 0 )
		{
			Sleep((DWORD)m_lVacuumAfter);
		}
	}


	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping - Before BondArmMoveToPrePick");
	if (bHomeBHT && (m_bExpanderStatus == WL_DOWN))		//v3.27T1	//Re-use bHomeBHT
	{
		Sleep(100);		//v4.37T13
		BondArmMoveToPrePick();
	}


	//CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping - After BondArmMoveToPrePick");
	if ( ((GetExpType() != WL_EXP_VACUUM) && (GetExpType() != WL_EXP_CYLINDER) && (GetExpType() != WL_EXP_NONE)) && 
		 IsExpanderLock())	//v3.90
	{
		Sleep(1000);
		if (IsExpanderLock())
		{
			SetErrorMessage("Expander unlock failed");
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
		}
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping - Complete HouseKeeping");
	return TRUE;
}


INT CWaferLoader::FilmFrameDriveIn(INT siDirection, LONG lSearchDistance, LONG lSearchVelocity, BOOL bWaitComplete)
{
	CString szLog;

	try
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Start FilmFrameDriveIn");
		CMS896AStn::MotionSelectSearchProfile(WL_AXIS_X, "spfWaferGripperSearchJam", &m_stWLAxis_X);

		szLog.Format("Start MotionUpdateSearchProfile - SrchDist = %d, SrchVel = %d", lSearchDistance,  lSearchVelocity);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
		CMS896AStn::MotionUpdateSearchProfile(WL_AXIS_X, "spfWaferGripperSearchJam", lSearchVelocity, lSearchDistance, &m_stWLAxis_X);

		CMSLogFileUtility::Instance()->WL_LogStatus("Start MotionSearch");
		CMS896AStn::MotionSearch(WL_AXIS_X,!siDirection, SFM_NOWAIT, &m_stWLAxis_X, "spfWaferGripperSearchJam");
	}
	catch(CAsmException e)
	{
		DisplayException(e);
		CMSLogFileUtility::Instance()->WL_LogStatus("Exception: MotionSearch");
	}
	

	if (bWaitComplete == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Start MotionSearch WaitComplete");
		X_Sync();
		Sleep(100);
	}

	if (CMS896AStn::MotionIsPowerOn(WL_AXIS_X, &m_stWLAxis_X) == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("FilmFrameDriveIn CheckPower fail");
		return FALSE;
	}

	if ( IsFrameJam() )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("FilmFrameDriveIn checkJam fail");
		return FALSE;
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("FilmFrameDriveIn End");
	return 1;
}


INT CWaferLoader::FilmFrameSearch(VOID)
{
	try
	{
		CMS896AStn::MotionSelectSearchProfile(WL_AXIS_X, "spfWaferGripperSeachFrameDetect", &m_stWLAxis_X);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	CMS896AStn::MotionSearch(WL_AXIS_X, 0, SFM_NOWAIT, &m_stWLAxis_X, "spfWaferGripperSeachFrameDetect");

	if (CMS896AStn::MotionIsPowerOn(WL_AXIS_X, &m_stWLAxis_X) == FALSE)
	{
		return FALSE;
	}

	X_Sync();
	return 1;
}

INT CWaferLoader::FilmFrameSearchOnTable()
{
	try
	{
		CMS896AStn::MotionSelectSearchProfile(WL_AXIS_X, "spfWaferGripperSeachFrameOnTable", &m_stWLAxis_X);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	CMS896AStn::MotionSearch(WL_AXIS_X, 0, SFM_NOWAIT, &m_stWLAxis_X, "spfWaferGripperSeachFrameOnTable");
	X_Sync();
	LONG nErr = CMS896AStn::ReportUserErrorStatus(WL_AXIS_X, &m_stWLAxis_X);

	if (((nErr == 0) || (nErr == 1)) && 
		CMS896AStn::MotionIsPowerOn(WL_AXIS_X, &m_stWLAxis_X))
	{
		return gnOK;
	}

	return gnNOTOK;
}

BOOL CWaferLoader::MoveSearchFrame(INT nPos)
{
	INT nResult;

	nResult = CMS896AStn::MotionMove(WL_AXIS_X, nPos, SFM_NOWAIT, &m_stWLAxis_X);

	CMSLogFileUtility::Instance()->WL_LogStatus("Start Move Search Frame");

	while(1)
	{
		
		if ( nResult != TRUE )
		{
			break;
		}
		
		if ( CMS896AStn::MotionIsComplete(WL_AXIS_X, &m_stWLAxis_X) == TRUE )
		{
			X_Sync();
			break;
		}

		if (IsFrameDetect() == TRUE)
		{
			return TRUE;
		}

		if (!IsAllMotorsEnable())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at LOAD");
			SetErrorMessage("Gripper is OFF @ at LOAD");
			return Err_FrameJammed;
		}

		if ( IsFrameJam() == TRUE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to move search frame");
			SetErrorMessage("Gripper is Jam @ to LOAD");
			CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
			return Err_FrameJammed;
		}

		if ( IsCoverOpen() == TRUE )	//v4.50A17
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to LOAD by COVER OPEN");
			SetErrorMessage("Gripper is Jam @ to LOAD by COVER OPEN");
			CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
			return Err_FrameJammed;
		}

		Sleep(1);
	}

	return TRUE;

}

INT CWaferLoader::MoveSearchFrameExistAndJam(INT nPos)
{
	INT nResult;

	nResult = X_MoveTo(nPos, SFM_NOWAIT);
	//nResult = CMS896AStn::MotionMoveTo(WL_AXIS_X, nPos, SFM_NOWAIT, &m_stWLAxis_X);

	while(1)
	{
		if ( nResult != gnOK )
		{
			break;
		}
		
		if ( CMS896AStn::MotionIsComplete(WL_AXIS_X, &m_stWLAxis_X) == TRUE )
		{
			X_Sync();
			break;
		}

		if ( IsFrameJam() == TRUE )
		{
			//m_pStepper_X->Stop();
			CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
			return Err_FrameJammed;
		}

		if ( IsCoverOpen() == TRUE )	//v4.50A17
		{
			//m_pStepper_X->Stop();
			CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
			return Err_FrameJammed;
		}

		if (IsFrameDetect() == TRUE)
		{
			//m_pStepper_X->Stop();
			CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
			return Err_FrameExistInGripper;
		}
		Sleep(1);
	}

	return TRUE;
}

INT CWaferLoader::UnloadFilmFrame(const BOOL bStartFromHome, const BOOL bBurnIn, const BOOL bGetWIPMgzSlot_AutoLine, const BOOL bManualChangeWafer)
{
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	pBinTable->WaferEndFileGenerating("Unload Film Frame", TRUE);
	DOUBLE dYield = pBinTable->GetWaferYield("Unload File Frame");
	if (dYield < 95)
	{
		CString szMsg;
		szMsg.Format("Yield(%f) Is Low, Please Check Wafer!", dYield);
		HmiMessage_Red_Yellow(szMsg);
	}

	INT nRet = SubUnloadFilmFrameWithLock(bStartFromHome, bBurnIn, bGetWIPMgzSlot_AutoLine);
	if (!IsAutoLoadUnloadTest() && IsMSAutoLineMode())
	{
		CString szMsg;
		if (nRet == ERR_WL_BACK_GATE_NOT_CLOSED)
		{
			return nRet;
		}
		else if (nRet == ERR_OUTPUT_WIP_SLOTS_FULL)
		{
			if (!bManualChangeWafer)
			{
				nRet = WaitForAutoLineToRemoveOutputFrame();
			}
			if (nRet != TRUE)
			{
				CString szMsg;
				if (bGetWIPMgzSlot_AutoLine)
				{
					szMsg.Format("Not Available WIP Slot(#7~#8) in Wafer Cassette, Request EM to unload wafer after close alarm");
				}
				else
				{
					szMsg.Format("Not Available Output Slot(#4~#6) in Wafer Cassette, Request EM to unload wafer after close alarm");
				}
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				SetAlert_Msg_NoMaterial(IDS_WL_MAG_FULL, szMsg);		
				SetErrorMessage("WL error: Full frame in wafer cassette (AUTOLINE)");
				return FALSE;
			}
			nRet = SubUnloadFilmFrameWithLock(bStartFromHome, bBurnIn, bGetWIPMgzSlot_AutoLine);
		}
	}
	return nRet;
}


INT CWaferLoader::SubUnloadFilmFrameWithLock(const BOOL bStartFromHome, const BOOL bBurnIn, const BOOL bGetWIPMgzSlot_AutoLine)
{
	if (!LoadUnloadLockMutex())
	{
		CString szLog;

		szLog.Format("LoadUnloadLockMutex  UnloadFilmFrame  = %ld", m_lLoadUnloadSECSCmdObject);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	INT nRet = 0;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("CWaferLoader::SubUnloadFilmFrameWithLock CheckClosedALBackGate");
	nRet = CheckClosedALBackGate();
	if (nRet)
	{
		LoadUnloadUnlock();
		return nRet;
	}

	nRet = SubUnloadFilmFrame(bStartFromHome, bBurnIn, bGetWIPMgzSlot_AutoLine);

	LONG lRet1 = CloseALFrontGate();
	
	if (nRet == TRUE)
	{
		nRet = lRet1;
	}
	LoadUnloadUnlock();

	return nRet;
}

INT CWaferLoader::SubUnloadFilmFrame(const BOOL bStartFromHome, const BOOL bBurnIn, const BOOL bGetWIPMgzSlot_AutoLine)
{
	BOOL bFrameDetected = FALSE;
	LONG lLoadPosX, lUnloadPosX;
	LONG lUnloadFrameTime;
	BOOL bOpenGripperDuringUnload;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString	szOutputBarcodeName = "Unscan";
	LONG lReply = 0;
	BOOL bSemiAuto = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_SEMI_AUTO);	
	CString szAlarmMessage;

	//if unload frame, it need clear sorting grade value
	if (IsSecsGemInit()) 
	{
		(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SORTING_GRADE] = 0;  
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("");
	CMSLogFileUtility::Instance()->WL_LogStatus("Unload start");

	if( m_bFrameOperationInAutoMode==FALSE )
	{
		if( WPR_MoveFocusToSafe()==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL AOI AF Z not at safe");
			return Err_AutoFocus_Z_Fail;
		}
	}

	//Check Banana scope
	if ( IsScopeDown() == TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Scope is down");
		SetErrorMessage("Scope is down");
		
		SetAlert_Red_Yellow(IDS_WL_SCOPE_DOWN);
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return Err_ScopeDown;
	}

	//Check Magazine is inside the loader Z
	if ( !m_bUseBLAsLoaderZ &&			//v4.39T10	//Yealy
		 (IsMagazineExist() == FALSE) && (bBurnIn == FALSE) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Magazine not exist");
		SetErrorMessage("Magazine not exist");
		SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return Err_NoMagazineExist;
	}

	//v3.92
	// Check frame is on gripper before moving the gripper
	if (IsFrameDetect() == TRUE && m_bNoSensorCheck == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("wafer gripper detected frame on gripper before UNLOAD");
		SetErrorMessage("wafer gripper detected frame on gripper before UNLOAD");
		SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
		return Err_FrameExistInGripper;
	}

	//v3.97	//Lextar
	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER && GetExpType() != WL_EXP_NONE) && 
		 IsExpanderLock() )	//OsramPenang	//v4.15T4
	{
		SetExpanderLock(WL_OFF);
		Sleep(500);
		if (IsExpanderLock())
		{
			SetErrorMessage("Expander unlock fail at Unload Film Frame");
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			return Err_ExpanderLockFailed;
		}
	}

	long lOrgMgzNo = m_lCurrentMagNo;
	long lOrgSlotNo = m_lCurrentSlotNo;
	if (!IsAutoLoadUnloadTest() && IsMSAutoLineMode())
	{
		//Find the sorting slot for current slot
		if (!GetOutputWIPSlotWithStatus_AutoLine(WL_SLOT_USAGE_SORTING, m_lCurrentMagNo, m_lCurrentSlotNo))
		{
			SetAlert_Red_Yellow(IDS_WL_EXP_NO_FRMAE_EXIST);
			HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
			return Err_NoFrameExist;
		}

		//update
		lOrgMgzNo = m_lCurrentMagNo;
		lOrgSlotNo = m_lCurrentSlotNo;

		CString szLogMsg;
		szLogMsg.Format("GetUnloadMgzSlot_AutoLine Start to %d slot %d", m_lCurrentMagNo, m_lCurrentSlotNo);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		if (!GetUnloadMgzSlot_AutoLine(bGetWIPMgzSlot_AutoLine, m_lCurrentSlotNo))
		{
			szLogMsg.Format("GetUnloadMgzSlot_AutoLine failure to %d slot %d", m_lCurrentMagNo, m_lCurrentSlotNo);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);

//			HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
			m_bAutoLineUnloadDone = FALSE;
			return ERR_OUTPUT_WIP_SLOTS_FULL;
		}

		if (lOrgSlotNo != m_lCurrentSlotNo)
		{
			szLogMsg.Format("IsMSAutoLineMode : Unload Move to G mag %d slot %d", m_lCurrentMagNo, m_lCurrentSlotNo);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
//			MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, m_lUnloadOffset);
//			Z_Home();
//			MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, m_lUnloadOffset);
		}
	}


	//Move Elevator to current slot + unload offset
	CString szLogMsg;
	INT lStatus = TRUE;
	szLogMsg.Format("Unload Move to mag %d slot %d;%s", m_lCurrentMagNo, m_lCurrentSlotNo, m_szWT1YieldState);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);

	if ( IsESMachine() && (m_bUseMagazine3AsX || m_bUseAMagazineAsG) )	// X_Magazine unload 1
	{
		if ( m_bUseMagazine3AsX && m_szWT1YieldState == "N" )
		{
			if (m_lCurrentMagazineX1SlotNo > m_stWaferMagazine[m_lMagazineX_No-1].m_lNoOfSlots)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("move to WT1 X mgzn slot already full");
				HmiMessage_Red_Back("Low yield magazine 1 already full.", "AOI Scan");
				lStatus = FALSE;
			}
			else
			{
				szLogMsg.Format("Unload Move to X mag %d slot %d", m_lMagazineX_No, m_lCurrentMagazineX1SlotNo);
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				lStatus = MoveToMagazineSlot(m_lMagazineX_No, m_lCurrentMagazineX1SlotNo, m_lUnloadOffset, TRUE);
				if ( !m_bUseBLAsLoaderZ && (IsMagazineExist() == FALSE) && (bBurnIn == FALSE) )
				{
					CString szMsg = "WL1 Magazine X not exist";
					CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
					SetErrorMessage(szMsg);
					SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);
					HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
					return Err_NoMagazineExist;
				}

				m_lCurrentMagazineX1SlotNo += (m_lSkipSlotNo + 1);
				if (m_lCurrentMagazineX1SlotNo > m_stWaferMagazine[m_lMagazineX_No-1].m_lNoOfSlots)
				{
					HmiMessage_Red_Back("Low yield magazine 1 full.", "AOI Scan");
				}
				m_szWT1YieldState = "";
			}
		}
		else if ( m_bUseAMagazineAsG && m_szWT1YieldState == "Y" )
		{
			if (m_lMagazineG1CurrentSlotNo > m_stWaferMagazine[m_lMagazineG_No-1].m_lNoOfSlots)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("move to WT1 G mgzn slot already full");
				HmiMessage_Red_Back("Good yield magazine 1 already full.", "AOI Scan");
				lStatus = FALSE;
			}
			else
			{
				szLogMsg.Format("Unload Move to G mag %d slot %d", m_lMagazineG_No, m_lMagazineG1CurrentSlotNo);
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				lStatus = MoveToMagazineSlot(m_lMagazineG_No, m_lMagazineG1CurrentSlotNo, m_lUnloadOffset, TRUE);
				if ( !m_bUseBLAsLoaderZ && (IsMagazineExist() == FALSE) && (bBurnIn == FALSE) )
				{
					CString szMsg = "WL1 Magazine G not exist";
					CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
					SetErrorMessage(szMsg);
					SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);
					HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
					return Err_NoMagazineExist;
				}

				m_lMagazineG1CurrentSlotNo += (m_lSkipSlotNo + 1);
				if (m_lMagazineG1CurrentSlotNo > m_stWaferMagazine[m_lMagazineG_No-1].m_lNoOfSlots)
				{
					HmiMessage_Red_Back("Good yield magazine 1 full.", "AOI Scan");
				}
				m_szWT1YieldState = "";
			}
		}
		else
		{
			m_szWT1YieldState = "";
			lStatus = MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, m_lUnloadOffset, FALSE);	//v2.93T2
		}
	}
	else
	{
		lStatus = MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, m_lUnloadOffset, FALSE);	//v2.93T2
		m_szWT1YieldState = "";
	}

	if ( lStatus != TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("move to mgzn slot fail");
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return lStatus;
	}

	if (!IsAutoLoadUnloadTest() && IsMSAutoLineMode())
	{
		TransferUnloadMgznSlot_AutoLine(lOrgMgzNo - 1, lOrgSlotNo - 1, m_lCurrentMagNo - 1, m_lCurrentSlotNo - 1, bGetWIPMgzSlot_AutoLine);
	}

	if (m_lFrameToBeUnloaded == 1)
	{
		m_lFrameToBeUnloaded = 0;
	}

	//v2.83T4 (Move Gripper to pre-unload position & move wafer table at the same time)
	INT nXResult = gnOK;
	if (GetExpType() == WL_EXP_VACUUM)
	{
		INT nStatus = FilmFrameVacuumReady();
		if ( nStatus != TRUE)
		{
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderOpenCloseFailed;
		}

		if( IsMotionCE() )
		{
			CString szMsg;
			szMsg = "Motin Critical Error in unload film frame exp vacuum";
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			SetErrorMessage(szMsg);
			HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
			return FALSE;
		}

		if ( m_bMapWfrCass == FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Vacuum Expander Ready");
			ExpanderVacuumPlatform(WL_UP, TRUE);
			CMSLogFileUtility::Instance()->WL_LogStatus("Vacuum Expander UP");
		}
		else
		{
			ExpanderVacuumPlatform(WL_UP, FALSE);
		}

		//Align Frame
		AlignWaferFrame();
		//Enable Limit sensor protection
		//m_pStepper_X->EnableProtection(HP_LIMIT_SENSOR, TRUE);
		//CMS896AStn::MotionEnableProtection("WaferGripperAxis", HP_LIMIT_SENSOR, TRUE);

		//Move Gripper to unload pos & do searching
		if (IsMS60())	//v4.47T2
			nXResult = X_MoveTo_Auto(m_lUnloadPos_X + WL_GRIPPER_DIRECTRING_SEARCH_DIST_ON_TABLE);
		else
			nXResult = X_MoveTo_Auto(m_lUnloadPos_X+WL_GRIPPER_SEARCH_DIST);
	}
	else
	{
		// Check Expander open or not before moving gripper
		INT nStatus = 0;

		CMSLogFileUtility::Instance()->WL_LogStatus("Move WT T to 0");
		MoveWaferThetaTo(0);

		if( IsMotionCE() )
		{
			CString szMsg;
			szMsg = "Motin Critical Error in Unload film frame";
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			SetErrorMessage(szMsg);
			HmiMessage_Red_Back(szMsg, "Wafer Loader");
			return Err_ExpanderLockFailed;
		}

		if ( (GetExpType() != WL_EXP_CYLINDER) && (GetExpType() != WL_EXP_NONE) )		//v4.39T10
		{
			if (IsExpanderLock() == FALSE)
			{
				SetExpanderLock(WL_ON);
				Sleep(400);

				if (IsExpanderLock() == FALSE)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("WL expander lock fail");
					SetAlert_Red_Yellow(IDS_WL_EXP_LOCK_FAIL);
					return Err_ExpanderLockFailed;
				}
			}
		}

		if ( (IsExpanderOpen() == TRUE))
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander already open");
			SetErrorMessage("Expander already open");

			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderAlreadyOpen;
		}
	
		if (CMS896AApp::m_bMS100Plus9InchOption)		//v4.24T1
		{
			if (FilmFrameGearReady(SFM_WAIT, FALSE, FALSE) == FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("WL film frame gear ready false");
				HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
				return FALSE;
			}
		}
		else
		{
			if (FilmFrameGearReady(SFM_NOWAIT, FALSE, FALSE) == FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("WL film frame gear ready false");
				HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
				return FALSE;
			}
		}
		
		//Move Gripper to unload pos & do searching
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper to UNLOAD");
		
		if (MotionIsServo(WL_AXIS_X, &m_stWLAxis_X))	//MS899_SVO_MOTOR		//v3.59
		{
			if (m_lUnloadPos_X+WL_GRIPPER_SVO_SEARCH_DIST_ON_TABLE > 0)
			{
				nXResult = X_MoveTo_Auto(0, SFM_NOWAIT);
			}
			else
			{
				if (CMS896AStn::m_bCEMark)		//v4.50A20	//v4.50A24
					nXResult = X_MoveTo_Auto(m_lUnloadPos_X + WL_GRIPPER_SVO_SEARCH_DIST_ON_TABLE, SFM_WAIT);
				else
					nXResult = X_MoveTo_Auto(m_lUnloadPos_X + WL_GRIPPER_SVO_SEARCH_DIST_ON_TABLE, SFM_NOWAIT);
			}
		}
		else
		{
			if (GetExpType() == WL_EXP_CYLINDER)
			{
				lUnloadPosX = m_lUnloadPos_X + WL_GRIPPER_CYLINDER_SEARCH_DIST_ON_TABLE;	
			}
			else if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
			{
				lUnloadPosX = m_lUnloadPos_X + WL_GRIPPER_DIRECTRING_SEARCH_DIST_ON_TABLE;	
			}
			else 
			{
				if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.21T2	//MS100 9Inch	
					lUnloadPosX = m_lUnloadPos_X + WL_GRIPPER_SEARCH_DIST_ON_TABLE * 2;		//Gripper X ENC resolution is doubled
				else
					lUnloadPosX = m_lUnloadPos_X + WL_GRIPPER_SEARCH_DIST_ON_TABLE;
			}

			if (lUnloadPosX > 0)
			{
				lUnloadPosX = 0;
			}
			
			if (CMS896AStn::m_bCEMark)		//v4.50A20	//v4.50A24
				nXResult = X_MoveTo_Auto(lUnloadPosX, SFM_WAIT);
			else
				nXResult = X_MoveTo_Auto(lUnloadPosX, SFM_NOWAIT);
		}

		//v4.50A20	//Handle cover Open for Osram Germany	//v4.50A24
		if ( CMS896AStn::m_bCEMark && (nXResult != gnOK) )
		//if (  (nXResult != gnOK) )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is aborted to UNLOAD by COVER OPEN");
			SetErrorMessage("Gripper is aborted to UNLOAD by COVER OPEN");
			if (IsCoverOpen())
				SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
			else
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			return Err_FrameJammed;
		}


		// Sync Wafer Table Before Move
		SyncWaferTableXYT("XY");
		SetGripperLevel(WL_ON);

		// Open the Expander After Moved the Wafer Table
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander is moving UP");
		
		if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) ||		//v4.01	
			 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28T4
		{
			nStatus = ExpanderDCMotorPlatform(WL_UP, TRUE, TRUE, FALSE, m_lExpDCMotorDacValue,
												FALSE, FALSE, TRUE);	//v4.59A17	
		}
		else if (GetExpType() == WL_EXP_CYLINDER)
		{
			nStatus = ExpanderCylinderMotorPlatform(WL_UP, TRUE, TRUE, FALSE);
		}
		else
		{
			nStatus = ExpanderGearPlatform(WL_UP, TRUE, TRUE, FALSE);
		}

		if ( nStatus == Err_ExpanderAlreadyOpen )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander already open");
			SetErrorMessage("Expander already open");

			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderAlreadyOpen;
		}
		else if ( nStatus == Err_ExpanderOpenCloseFailed )		//v3.93
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander open timeout");
			SetErrorMessage("Expander open timeout");

			SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderOpenCloseFailed;
		}
		else if ( nStatus != TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot open");
			SetErrorMessage("Expander cannot open");

			SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);
			if (!IsCoverOpen())		//v4.50A20
				HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderOpenCloseFailed;
		}

		//v3.19T1
		if (!IsAllMotorsEnable())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Not all motors enable");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			return Err_FrameJammed;
		}

		nXResult = X_Sync_Auto();
	}

	/*if (nXResult == FALSE)		//v4.50A20
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to Unload by COVER OPEN");
		SetErrorMessage("Gripper is Jam @ to Unload by COVER OPEN");
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		//HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}*/

	if (IsGripperMissingSteps())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is missing steps @ to Unload");
		SetErrorMessage("Gripper is missing steps @ to Unload");

		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to Unload");
		SetErrorMessage("Gripper is Jam @ to Unload");

		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (GetExpType() == WL_EXP_VACUUM)
	{
		if (IsMS60())	//v4.46T28
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper search frame on table (MS60)");
			FilmFrameSearchOnTable();
		}
		else
			FilmFrameSearch();
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper search frame on table");
#ifndef NU_MOTION	//v4.04
		CMS896AStn::MotionEnableProtection(WL_AXIS_X, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stWLAxis_X);	//v3.59
#endif
		FilmFrameSearchOnTable();
#ifndef NU_MOTION	//v4.04
		CMS896AStn::MotionEnableProtection(WL_AXIS_X, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stWLAxis_X);	//v3.59
#endif
	}

	m_bFrameExistOnExp = FALSE;			//v2.64
	CMSLogFileUtility::Instance()->WL_LogStatus("Save Data");

	if (IsGripperMissingSteps())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is missing steps @ to Unload 2");
		SetErrorMessage("Gripper is missing steps @ to Unload 2");

		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to Unload 2");
		SetErrorMessage("Gripper is Jam @ to Unload 2");

		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	//v3.19T1
	if (!IsAllMotorsEnable())
	{
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		return Err_FrameJammed;
	}

	//Check Frame detect sensor
	CMSLogFileUtility::Instance()->WL_LogStatus("Check frame exist on gripper");

	bFrameDetected = IsFrameDetect();

	// hard code for test
	//bFrameDetected = TRUE;

	if (bBurnIn == TRUE)
	{
		bFrameDetected = TRUE;
	}
	else	//v2.81T4
	{
		if (!bFrameDetected)
		{
			Sleep(200);
			FilmFrameSearchOnTable();
			Sleep(200);
			bFrameDetected = IsFrameDetect();
		}
	}

	Z_Sync();	//v2.93R2

	if (bFrameDetected == TRUE || m_bNoSensorCheck == TRUE)
	{
		if (IsFrameJam() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ frame detected");
			SetErrorMessage("Gripper is Jam @ frame detected");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		//Offset for Unload Position
		/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Gripper step on frame");	/*******/ //v2.93T1
		//SetErrorMessage("Gripper step on frame");

		//m_pStepper_X->EnableProtection(HP_LIMIT_SENSOR, TRUE);		//v3.19T1
		//CMS896AStn::MotionEnableProtection(WL_AXIS_X, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stWLAxis_X);

		if (m_lStepOnUnloadPos != 0)
		{
			X_Move(m_lStepOnUnloadPos);
		}

		//On Clip
		if (m_bOpenGripperInUnload == FALSE)
		{
			SetGripperState(WL_ON);
		}
		else
		{
			SetGripperState(WL_OFF);
		}

		//Move Gripper to load pos
		/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Gripper move to LOAD");	/*******/ //v2.93T1

		//v3.19T1
		if (!IsAllMotorsEnable())
		{
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
#ifndef NU_MOTION	//v4.04
			CMS896AStn::MotionEnableProtection(WL_AXIS_X, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stWLAxis_X);
#endif
			return Err_FrameJammed;
		}

		lLoadPosX = m_lLoadPos_X;

		if (GetExpType() == WL_EXP_CYLINDER)
		{
			bOpenGripperDuringUnload = TRUE;
			GetEncoderValue();
			lUnloadFrameTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X, "mpfWaferGripper", 
				(m_lEnc_X - m_lLoadPos_X), (m_lEnc_X - m_lLoadPos_X), HIPEC_SAMPLE_RATE, &m_stWLAxis_X);
		}
		else
		{
			bOpenGripperDuringUnload = FALSE;
			lUnloadFrameTime = 0;
		}
		///////////////////////////////Scan BarCode @Matthew 20181128
		if ( ( m_bUseBarcode || bSemiAuto ) && (pApp ->GetCustomerName() == "ChangeLight(XM)") )		//v4.39T10
		{
			LONG lProfileTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X, "mpfWaferGripper", 
								(m_lBarcodePos_X - m_lLoadPos_X), (m_lBarcodePos_X - m_lLoadPos_X), 
								HIPEC_SAMPLE_RATE, &m_stWLAxis_X);
			LONG lDelay = 500;
			if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)
				lDelay = lProfileTime - 200;		//Servo motor speed runs faster than stepper motor
			else
				lDelay = lProfileTime - 250;		//Stepper motor runs slower
	
			if (lDelay < WL_BARCODE_SCAN_MIN_TIME)
				lDelay = WL_BARCODE_SCAN_MIN_TIME;

			for(int i = 0; i <= m_lScanRange * 3; i = i + m_lScanRange)
			{
				X_MoveTo(m_lWLGeneral_4 + i, SFM_NOWAIT);
				Sleep(lDelay);	//v4.01
				ReadBarcode(&szOutputBarcodeName, 50, 5);
				if(szOutputBarcodeName != "")
				{
					break;
				}
				if (i != 0)
				{
					X_MoveTo(m_lWLGeneral_4 - i, SFM_NOWAIT);
					Sleep(lDelay);	//v4.01
					ReadBarcode(&szOutputBarcodeName, 50, 5);
					if(szOutputBarcodeName != "")
					{
						break;
					}
				}
			}
			GetEncoderValue();
			if(szOutputBarcodeName != "")
			{
				CString szLogMsg;
				szLogMsg.Format("Gripper read barcode is %s, on X %ld", szOutputBarcodeName, m_lEnc_X);
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			}
			X_Sync();

			CString szTemp;
			if( szOutputBarcodeName != m_szBarcodeName )
			{
				if (pApp->GetCustomerName() == "ChangeLight(XM)")
				{
					szTemp.Format("Output barcode (%s) does not match the barcode (%s)", szOutputBarcodeName, m_szBarcodeName);
					SetErrorMessage(szTemp);
					szAlarmMessage = "Please check the wafer barcode is the same.\nIf yes, unload wafer normally.\nIf no, please find op leader to confirm again.\n";
					szAlarmMessage += "New,[@@" + szOutputBarcodeName + "@@]\nOld,[@@" + m_szBarcodeName + "@@]"; //HMI show Chinese message with variable
					//szAlarmMessage = "Please check the wafer barcode.\nContinue if the barcode match.\nStop if the barcode not match";
					//lReply = HmiMessage_Red_Back(szAlarmMessage, m_szBarcodeName, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER);
					lReply = SetAlert_Msg_Red_Back(IDS_WL_NO_BARCODE, szAlarmMessage, "Continue", "Abort", NULL, glHMI_ALIGN_LEFT);
					if ( lReply == 1 )
					{
						lReply = 1;
						if(szOutputBarcodeName == "")
						{
							szTemp.Format("Output barcode is not detected, confirm barcode (%s) match by op", m_szBarcodeName);
						}
						else
						{
							szTemp.Format("Output barcode (%s) does not match the barcode (%s)", szOutputBarcodeName, m_szBarcodeName);
						}
						SetErrorMessage(szTemp);
						CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
					}
					else
					{
						lReply = 2;
						if (szOutputBarcodeName == "")
						{
							szTemp.Format("Output barcode is not detected, confirm barcode (%s) match by op", m_szBarcodeName);
						}
						else
						{
							szTemp.Format("Output barcode (%s) does not match the barcode (%s)", szOutputBarcodeName, m_szBarcodeName);
						}
						SetErrorMessage(szTemp);
						CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);

						while(1)
						{
							BOOL bCancel = TRUE;
							SetAlert_Red_Yellow(IDS_WL_OUTPUT_BC_NOT_MATCH);
							if( pApp->CheckSananPasswordFile(TRUE, bCancel, 2) )
							{
								szTemp = "Check barcode by engineer successful";
								CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
								break;
							}
							else
							{						
								szAlarmMessage = "Wrong password, please approach engineer";
								HmiMessage_Red_Back(szAlarmMessage, "Wrong Password", glHMI_MBX_OK, glHMI_ALIGN_CENTER);
							}
						}
					}
				}
				else if (szOutputBarcodeName == "")
				{
					szAlarmMessage = "Please check the wafer barcode.\nContinue if the barcode match.\nStop if the barcode not match";
					lReply = HmiMessage_Red_Back(szAlarmMessage, m_szBarcodeName, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER);
					if ( lReply == glHMI_CONTINUE )
					{
						lReply = 1;
						szTemp.Format("Output barcode is not detected, confirm barcode (%s) match by op", m_szBarcodeName);
						SetErrorMessage(szTemp);
						CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
						//pApp->CheckPasswordToGo(1);
					}
					else
					{
						lReply = 2;
						szTemp.Format("Output barcode is not detected and barcode (%s) not match", m_szBarcodeName);
						SetErrorMessage(szTemp);
						CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
						SetAlert_Red_Yellow(IDS_WL_NO_OUTPUT_BC_AND_NOT_MATCH);
						/*while(1)
						{
							if( CheckOperatorPassWord(PRESCAN_PASSWORD_SUPER_USER) )
							{
								szTemp = "Superuser login successful";
								CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
								break;
							}
							else
							{						
								szAlarmMessage = "Wrong password, please approach engineer";
								HmiMessage_Red_Back(szAlarmMessage, "Wrong Password", glHMI_MBX_OK, glHMI_ALIGN_CENTER);
							}
						}*/
						//return Err_NoOutputBCAndNotMatch;
					}
				}
				else
				{
					lReply = 3;
					szTemp.Format("Output barcode (%s) does not match the barcode (%s)", szOutputBarcodeName, m_szBarcodeName);
					SetErrorMessage(szTemp);
					CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
					SetAlert_Red_Yellow(IDS_WL_OUTPUT_BC_NOT_MATCH);
					/*while(1)
					{
						if( CheckOperatorPassWord(PRESCAN_PASSWORD_SUPER_USER) )
						{
							szTemp = "Superuser login successful";					
							CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
							break;
						}
						else
						{
							
							//szAlarmMessage = "Wrong password, please approach engineer";
							//HmiMessage_Red_Back(szAlarmMessage, "Wrong Password", glHMI_MBX_OK, glHMI_ALIGN_CENTER);
							
							SetAlert_Red_Yellow(IDS_WL_OUTPUT_BC_NOT_MATCH);
						}
					}*/
					//SetAlert_Msg_Red_Back(IDS_WL_OUTPUT_BC_NOT_MATCH, m_szBarcodeName,"Continue", "Stop", NULL, glHMI_ALIGN_CENTER);
					//return Err_InputAndOutputBCNotMatch;
				}
			}
		}
		//////////////////////////////
		if (WaferGripperMoveSearchJam(lLoadPosX, bOpenGripperDuringUnload, lUnloadFrameTime) == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to unload film frame");
			SetErrorMessage("Gripper is Jam @ to load");

			if (GetExpType() != WL_EXP_VACUUM)
			{
				ResetGripperHitLimit();
				SetGripperState(WL_OFF);
				SetGripperPower(FALSE);		//v3.89	//Re-enable for safety purpose
			}

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			if( IsAOIOnlyMachine() )
			{
				if( HmiMessage_Red_Back("Please unload it into magazine by hand.", "Unload WT1", glHMI_MBX_YESNO)==glHMI_YES )
				{
					m_bFrameLoadedOnWT2	= FALSE;
				}
			}
			
			//v2.93T2
			if (GetExpType() == WL_EXP_VACUUM)
			{
				HouseKeeping(WL_ON, TRUE, FALSE, TRUE, TRUE);
			}
			else
			{
				ResetGripperHitLimit();
				SetGripperState(WL_OFF);
				
				if (GetExpType() != WL_EXP_CYLINDER)
				{
					SetExpanderLock(WL_OFF);
				}

				//if (X_IsPowerOn())
				//{
					//m_pStepper_X->DisableProtection(HP_LIMIT_SENSOR);
					//ClearStepperError("stpWaferGripper");
					//Sleep(500);
					//m_pStepper_X->PowerOn();
				//CMS896AStn::MotionEnableProtection(WL_AXIS_X, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stWLAxis_X);
				//CMS896AStn::MotionClearError(WL_AXIS_X, &m_stWLAxis_X);
				//Sleep(500);
				//SetGripperPower(FALSE);		//v3.89	//Re-enable for safety purpose
				//}

				Sleep(500);
				//X_Home();
				//m_pStepper_X->EnableProtection(HP_LIMIT_SENSOR, TRUE);
				//CMS896AStn::MotionEnableProtection("WaferGripperAxis", HP_LIMIT_SENSOR, TRUE, TRUE);
			}

			return Err_FrameJammed;
		}


		if (GetExpType() != WL_EXP_CYLINDER)
		{
			//Off Clip
			Sleep(100);
			SetGripperState(WL_OFF);
			Sleep(100);
		}
	
		CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(10);	//WAFER_UNLOAD	//v4.40T11

		//v3.19T1
		if (!IsAllMotorsEnable())
		{
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			return Err_FrameJammed;
		}

		//Move Gripper to load pos
		/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Gripper back to ready"); /*******/	//v2.93T1

		if (m_bUseBLAsLoaderZ && !bStartFromHome && (lLoadPosX < -20000))
		{
			X_MoveTo(lLoadPosX + 2000, SFM_NOWAIT);		//v4.31T12	//For MS100 Yealy with single load & buffer config
		}
		else
			X_MoveTo(m_lReadyPos_X, SFM_NOWAIT);
		SaveData();
		X_Sync();
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("No frame is detected");
		SetErrorMessage("No frame is detected");
		SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
	}


	//Disable Limit sensor protection
	//m_pStepper_X->DisableProtection(HP_LIMIT_SENSOR);
	//CMS896AStn::MotionEnableProtection("WaferGripperAxis", HP_LIMIT_SENSOR, TRUE, FALSE);

	if ((bFrameDetected == FALSE) || ((bStartFromHome == TRUE) && (bBurnIn == FALSE)))
	{
		if (bFrameDetected == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("No frame detected");	//v2.93T1
			SetErrorMessage("No frame is detected");
			HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
			return Err_NoFrameDetected;
		}
		else 
		{
			if (GetExpType() != WL_EXP_VACUUM)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("expander is moving down");	//v2.93T1
				//SetErrorMessage("expander is moving down");

				if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)
				{
					X_MoveTo(WL_GRIPPER_SRV_HOME_OFFSET_POS);	//v3.69T1
				}
				else
				{
					X_MoveTo(-1000);
				}

				INT nStatus = 0;
				if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||	//v4.01	
					 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )			//v4.28T4
				{
					nStatus = ExpanderDCMotorPlatform(WL_DOWN, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
				}
				else if (GetExpType() == WL_EXP_CYLINDER)
				{
					nStatus = ExpanderCylinderMotorPlatform(WL_DOWN, TRUE, FALSE, FALSE);
				}
				else
				{
					nStatus = ExpanderGearPlatform(WL_DOWN, TRUE, FALSE, FALSE);
				}

				if (nStatus != 1)
				{
					/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot close"); /*******/
					SetErrorMessage("Expander cannot close");
					SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);	
					HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
					return Err_ExpanderOpenCloseFailed;
				}

				HouseKeeping(WL_OFF, FALSE, TRUE, TRUE);
			}
			else
			{
				HouseKeeping(WL_OFF, FALSE, TRUE, TRUE);
			}
		}

		if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER && GetExpType() != WL_EXP_NONE) && 
			 (IsExpanderLock() == TRUE) )
		{
			Sleep(1000);
			if (IsExpanderLock())	//v3.90
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander unlock failed at UnLoadFilmFrame");
				SetErrorMessage("Expander unlock failed");
				SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
				return Err_ExpanderLockFailed;
			}
		}
	}

	if( ( szOutputBarcodeName != m_szBarcodeName ) && ( m_bUseBarcode || bSemiAuto ) && (pApp ->GetCustomerName() == "ChangeLight(XM)") )
	{
		if(lReply == 2)
		{
			HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
			return Err_NoOutputBCAndNotMatch;
		}
		else if(lReply == 3)
		{
			HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
			return Err_InputAndOutputBCNotMatch;
		}
	}

	CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(11);	//MOTION_END	//v4.40T11
	BOOL bMachineLog = CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(TRUE);	//v4.40T13

	/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Unload finish\n"); /*******/

	//Log into History file
	if (CMS896AStn::m_bForceClearMapAfterWaferEnd == FALSE && IsES101()==FALSE )
	{
		LogWaferEndInformation(m_lCurrentSlotNo, m_szBarcodeName);
	}
	m_bFrameLoadedOnWT1	= FALSE;

	return TRUE;
}

//=========================================================================
//  Sync the load/unload frame function with SECS load/unload command
//=========================================================================
BOOL CWaferLoader::WaitForLoadUnloadSECSCmdObject()
{
	while (m_lLoadUnloadSECSCmdObject == 1)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}
		if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
			(Action() == glABORT_STOP)))
		{
			return FALSE;
		}
		Sleep(100);
	}
	return TRUE;
}

BOOL CWaferLoader::IsLoadUnloadSECSCmdLocked()
{
	return (m_lLoadUnloadSECSCmdObject == 1);
}

VOID CWaferLoader::LoadUnloadSECSCmdLock()
{
	m_lLoadUnloadSECSCmdObject = 1;
}

VOID CWaferLoader::LoadUnloadSECSCmdUnlock()
{
	m_lLoadUnloadSECSCmdObject = 0;
}


BOOL CWaferLoader::WaitForLoadUnloadObject()
{
	long lCount = 0;
	while (m_lLoadUnloadObject == 1)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		Sleep(100);
		lCount++;
		if (lCount  > 50)
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CWaferLoader::LoadUnloadLockMutex()
{
	CSingleLock lock(&m_csLoadUnloadMutex);
	lock.Lock();

	if (!WaitForLoadUnloadSECSCmdObject())
	{
		return FALSE;
	}
	LoadUnloadLock();
	return TRUE;
}

VOID CWaferLoader::LoadUnloadLock()
{
	m_lLoadUnloadObject = 1;
}

VOID CWaferLoader::LoadUnloadUnlock()
{
	m_lLoadUnloadObject = 0;
}

BOOL CWaferLoader::IsAutoLoadUnloadTest()
{
	return m_bAutoLoadUnloadTest;
}

LONG CWaferLoader::CheckClosedALBackGate()
{
	if (IsMSAutoLineMode() && !CloseALBackGate(TRUE) && !IsCloseALBackGate())
	{
		CString szMsg;
		szMsg = _T("Wafer Loader Back Gate Not Closed or Front Gate Not Opened");
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		SetAlert_Msg_Red_Yellow(IDS_WL_BACK_GATE_NOT_CLOSED, szMsg);		
		SetErrorMessage("WL error: Wafer Loader Back Gate Not Closed or Front Gate Not Opened");
		return ERR_WL_BACK_GATE_NOT_CLOSED;
	}
	return 0;
}


//=========================================================================

INT CWaferLoader::LoadFilmFrame(BOOL bStartFromHome, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bSearhHomeDie, 
								BOOL bBarCodeOnTable, BOOL bDisplayMsg, BOOL bGetWIPMgzSlot_AutoLine, BOOL bManualChangeWafer)
{
	INT nRet = 0;
	
	while (TRUE)
	{
		nRet = SubLoadFilmFrameWithLock(bStartFromHome, bBurnIn, bSearchFrameInMagazine, bSearhHomeDie, 
										bBarCodeOnTable, bDisplayMsg, bGetWIPMgzSlot_AutoLine);

		if (!IsAutoLoadUnloadTest() && IsMSAutoLineMode())
		{
			CString szMsg;
			if (nRet == ERR_WL_BACK_GATE_NOT_CLOSED)
			{
				return nRet;
			}
			else if (nRet == ERR_NOT_AVALIABLE_INPUT_SLOTS)
			{
				if (!bManualChangeWafer)
				{
					nRet = WaitForAutoLineToInputWafer(bGetWIPMgzSlot_AutoLine);
				}
				if (nRet == TRUE)
				{
					continue;
				}

				if (nRet == ERR_NOT_AVALIABLE_INPUT_SLOTS)
				{
					if (bGetWIPMgzSlot_AutoLine)
					{
						szMsg.Format("Not Available Input Slot(#7~#8) in Wafer Cassette");
					}
					else
					{
						szMsg.Format("Not Available Input Slot(#1~#3) in Wafer Cassette");
					}
					CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
					SetAlert_Msg_Red_Yellow(IDS_WL_NO_WAFER_IN_LOT, szMsg);		
					SetErrorMessage("WL error: No Empty frame in wafer cassette (AUTOLINE)");
				}
			}
			else if (nRet == ERR_OUTPUT_WIP_SLOTS_FULL)
			{
				if (!bManualChangeWafer)
				{
					nRet = WaitForAutoLineToRemoveOutputFrame();
				}

				if (nRet == TRUE)
				{
					continue;
				}
				szMsg.Format("Output Slots are Full(#4~#6) in Wafer Cassette");
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				SetAlert_Msg_NoMaterial(IDS_WL_MAG_FULL, szMsg);		
				SetErrorMessage("WL error: Output slots are full in wafer cassette (AUTOLINE)");
			}
		}
		return nRet;
	}
	return nRet;
}


LONG CWaferLoader::WaitForAutoLineToInputWafer(BOOL bGetWIPMgzSlot_AutoLine)
{
	if (IsMSAutoLineMode() && !bGetWIPMgzSlot_AutoLine)
	{
		//Waiting for AutoLine
		CTmpChange<BOOL> bMsStopButtonPressed(&theApp.m_bMsStopButtonPressed, FALSE);
		CTmpChange<BOOL> bStopAlign(&CMS896AApp::m_bStopAlign, FALSE);
		CTmpChange<BOOL> bWaferAlignComplete(&m_bWaferAlignComplete, TRUE);
		CString szMsg;
		szMsg = "No Wafer in Wafer Cassette, Waiting for Smart Inline...";

		SetAlertTime(TRUE, EQUIP_ALARM_TIME, "", szMsg);
		CString szLog;
		LONG lCount = 0;
		while (TRUE)
		{
			m_comServer.ProcessRequest();

			MSG Msg; 
			if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
			{
				::DispatchMessage(&Msg);
			}

			if (lCount == 0)
			{
				szLog.Format("WaitForAutoLineToInputWafer: m_bAutoLineLoadDone = %ld", m_bAutoLineLoadDone);
				CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

				CloseAlarm();
				SetAlert_WarningMsg(IDS_MS_WARNING_INLINE_WAFER_LOADER_INPUT_SLOT_EMPTY, szMsg);
			}

			if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
				(Action() == glABORT_STOP)))
			{
				SetAlertTime(FALSE, EQUIP_ALARM_TIME, "", szMsg);
				SetAlarmLamp_Green(FALSE, TRUE);
				CloseAlarm();
				szLog.Format("WL: Command(), Action() = %ld, %d, %d", Command(), Action(), CMS896AApp::m_bStopAlign);
				CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
				break;
			}
			Sleep(100);
			if (++lCount == 100)
			{
				lCount = 0;
			}

			if (m_bAutoLineLoadDone)
			{
				SetAlertTime(FALSE, EQUIP_ALARM_TIME, "", szMsg);
				SetAlarmLamp_Green(FALSE, TRUE);
				CloseAlarm();
				szLog.Format("WL: WaitForAutoLineToInputWafer = %ld", m_bAutoLineLoadDone);
				CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

				if (!MoveToLoadMgzSlot_AutoLine(bGetWIPMgzSlot_AutoLine))
				{
					return ERR_NOT_AVALIABLE_INPUT_SLOTS;
				}
				m_bIsGetCurrentSlot = TRUE;
				LONG nErr = 0;

				nErr = OpenExpander();
				if (nErr != TRUE)
				{
					return nErr;
				}

				szLog.Format("WL: WaitForAutoLineToInputWafer finished = %ld", m_bAutoLineLoadDone);
				CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
				break;
			}
		}
		CloseAlarm();

		if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
			(Action() == glABORT_STOP)))
		{
			return FALSE;
		}
		return TRUE;
	}
	return ERR_NOT_AVALIABLE_INPUT_SLOTS;
}


BOOL CWaferLoader::WaitForAutoLineToRemoveOutputFrame()
{
	//Waiting for AutoLine
	CTmpChange<BOOL> bMsStopButtonPressed(&theApp.m_bMsStopButtonPressed, FALSE);
	CTmpChange<BOOL> bStopAlign(&CMS896AApp::m_bStopAlign, FALSE);
	CTmpChange<BOOL> bWaferAlignComplete(&m_bWaferAlignComplete, TRUE);
	CString szMsg;
	szMsg = "Output Slots are Full, Waiting for Smart Inline...";

	SetAlertTime(TRUE, EQUIP_ALARM_TIME, "", szMsg);
	CString szLog;
	LONG lCount = 0;
	while (TRUE)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		if (lCount == 0)
		{
			szLog.Format("WaitForAutoLineToRemoveOutputFrame: m_bAutoLineUnloadDone = %ld", m_bAutoLineUnloadDone);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

			CloseAlarm();
			SetAlert_WarningMsg(IDS_MS_WARNING_INLINE_WAFER_LOADER_OUTPUT_SLOT_FULL, szMsg);
		}

		if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
			(Action() == glABORT_STOP)))
		{
			SetAlertTime(FALSE, EQUIP_ALARM_TIME, "", szMsg);
			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();
			szLog.Format("WL: Command(), Action() = %ld, %d, %d", Command(), Action(), CMS896AApp::m_bStopAlign);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			return FALSE;
		}
		Sleep(100);
		if (++lCount == 100)
		{
			lCount = 0;
		}

		if (m_bAutoLineUnloadDone)
		{
			SetAlertTime(FALSE, EQUIP_ALARM_TIME, "", szMsg);
			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();

			szLog.Format("WL: WaitForAutoLineToRemoveOutputFrame = %ld", m_bAutoLineUnloadDone);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			return TRUE;
		}
	}
	return FALSE;
}

INT CWaferLoader::SubLoadFilmFrameWithLock(BOOL bStartFromHome, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bSearhHomeDie, 
										   BOOL bBarCodeOnTable, BOOL bDisplayMsg, BOOL bGetWIPMgzSlot_AutoLine)
{
	if (!LoadUnloadLockMutex())
	{
		CString szLog;

		szLog.Format("LoadUnloadLockMutex  LoadFilmFrame  = %ld", m_lLoadUnloadSECSCmdObject);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	INT nRet = 0;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM("CWaferLoader::SubLoadFilmFrameWithLock");
	nRet = CheckClosedALBackGate();
	if (nRet)
	{
		LoadUnloadUnlock();
		return nRet;
	}

	nRet = SubLoadFilmFrame(bStartFromHome, bBurnIn, bSearchFrameInMagazine, bSearhHomeDie, 
							 bBarCodeOnTable, bDisplayMsg, bGetWIPMgzSlot_AutoLine);

	LONG lRet1 = CloseALFrontGate();
	
	if (nRet == TRUE)
	{
		nRet = lRet1;
	}
	LoadUnloadUnlock();

	return nRet;
}

INT CWaferLoader::OpenExpander()
{
	if (GetExpType() == WL_EXP_VACUUM)
	{
		INT nStatus = FilmFrameVacuumReady();
		if ( nStatus != TRUE)
		{
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderOpenCloseFailed;
		}
		if( IsMotionCE() )
		{
			CString szMsg;
			szMsg = "Motin Critical Error in load film frame exp vacuum";
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			SetErrorMessage(szMsg);
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return FALSE;
		}
	}
	else
	{
		if (FilmFrameGearReady()== FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL load fileframe gear ready fail");
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return FALSE;
		}

//if (CMS896AApp::m_bMS100Plus9InchOption)		//v4.23T2
//{
//	HmiMessage("Andrew: Before expander UP at LOAD ....");
//}

		INT nStatus = 0;

		CMSLogFileUtility::Instance()->WL_LogStatus("Expander starts to gear UP...");
		//SetErrorMessage("Expander starts to gear UP...");

		if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||	//v4.01	
			(GetExpType() == WL_EXP_GEAR_DIRECTRING) )			//v4.28T4
		{
			nStatus = ExpanderDCMotorPlatform(WL_UP, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
		}
		else if (GetExpType() == WL_EXP_CYLINDER)
		{
			nStatus = ExpanderCylinderMotorPlatform(WL_UP,TRUE, FALSE, FALSE);
		}
		else
		{
			nStatus = ExpanderGearPlatform(WL_UP, TRUE, FALSE, FALSE);		//v2.56
		}

		if ( nStatus == Err_ExpanderAlreadyOpen )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander already open");
			SetErrorMessage("Expander already open");
			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderAlreadyOpen;
		}
		else if ( nStatus != TRUE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot open");
			SetErrorMessage("Expander cannot open");

			SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);	
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderOpenCloseFailed;
		}

		CMSLogFileUtility::Instance()->WL_LogStatus("Expander at UP posn");
	}

	return TRUE;
}


INT CWaferLoader::SubLoadFilmFrame(BOOL bStartFromHome, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bSearhHomeDie, 
								BOOL bBarCodeOnTable, BOOL bDisplayMsg, BOOL bGetWIPMgzSlot_AutoLine)
{
	INT nStatus = 0;
	BOOL bFrameDetected = FALSE;
	BOOL bFrameExisted = FALSE;
	BOOL bWaferNameFound = TRUE;
	BOOL bRepeatLoadFrame = FALSE;
	LONG nSearchStatus = 0;
	INT nFrameStatus = 0;
	INT nBarcodeStatus = 0;
	BOOL bPushBackAbort	= FALSE;
	BOOL bRtn = FALSE, bReturn = FALSE;
	BOOL bScanBarcodeOnTable = bBarCodeOnTable;	//FALSE;
	if (!m_bUseBarcode)		
		bScanBarcodeOnTable = FALSE;			//v4.46T19
	if( IsAOIOnlyMachine() )
		bScanBarcodeOnTable = FALSE;

	CString szCompareBarcode;
	BOOL bKeepBarcodePos = FALSE;
	INT nConvLoadMap = 0;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bSemiAuto = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_SEMI_AUTO);		//v4.48A21	//Avago

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	CMSLogFileUtility::Instance()->WPR_BackupAlignWaferLog();

	if (m_bUseBarcode == TRUE )
	{
		if (WaferEndFileGeneratingCheckingCmd() == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Wafer end file generating check failed");
			return Err_WaferEndGenFail;
		}

		BOOL bIsWaferEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferEnd"];
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( (bIsWaferEnd || IsPrescanEnded()==FALSE || pApp->GetCustomerName()!="OSRAM") && IsES101()==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Load frame, wafer end and not osram,clear map file.");
			ClearMapFile();
		}
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("");
	CMSLogFileUtility::Instance()->WL_LogStatus("Load start");

	if( m_bFrameOperationInAutoMode==FALSE )
	{
		if( WPR_MoveFocusToSafe()==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL load AOI move focus to safe fail");
			return Err_AutoFocus_Z_Fail;
		}
	}

	//Check Banana scope
	if ( IsScopeDown() == TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Scope is down");
		SetErrorMessage("Scope is down");

		SetAlert_Red_Yellow(IDS_WL_SCOPE_DOWN);
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return Err_ScopeDown;
	}

	//Make sure gripper on safe posn before performing LOAD
	if (!X_IsPowerOn())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("wafer gripper power not ON");
		HmiMessage_Red_Yellow("Gripper power not ON!", "Load Film Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return Err_WaferGripperPowerOff;
	}

	//Check Magazine is inside the loader Z
	if ( (IsMagazineExist() == FALSE) && (bBurnIn == FALSE) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Magazine is not exist");
		SetErrorMessage("Magazine is not exist");

		SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);	
		HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
		return Err_NoMagazineExist;
	}

	//v3.92
	// Check frame is on gripper before moving the gripper
	if (IsFrameDetect() == TRUE && m_bNoSensorCheck == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("wafer gripper detected frame on gripper before LOAD");
		SetErrorMessage("wafer gripper detected frame on gripper before LOAD");
		SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
		return Err_FrameExistInGripper;
	}

	//v3.97	//Lextar
	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER && GetExpType() != WL_EXP_NONE) && 
		IsExpanderLock() )	//v4.15T4	//OsramPenang
	{
		SetExpanderLock(WL_OFF);
		Sleep(500);
		if (IsExpanderLock())
		{
			SetErrorMessage("Expander unlock fail at Load FilmFrame");
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			return Err_ExpanderLockFailed;
		}
	}

	CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(2);	//WAFER_ACCESS	//v4.40T11

	if (bStartFromHome == TRUE)
	{
		INT nErr = OpenExpander();
		

		if (nErr != TRUE)
		{
			return nErr;
		}

		//Move Gripper to load pos
		nSearchStatus = MoveSearchFrameExistAndJam(m_lReadyPos_X);
		if ( nSearchStatus == Err_FrameJammed )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to ready");
			SetErrorMessage("Gripper is Jam @ to ready");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE, TRUE);
			return Err_FrameJammed;
		}
		else if ( nSearchStatus == Err_FrameExistInGripper )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Frame exist on gripper");
			SetErrorMessage("Frame exist on gripper");

			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONGRIPPER);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameExistInGripper;
		}

		if (IsFrameJam() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to ready");
			SetErrorMessage("Gripper is Jam @ to ready");
			if (IsCoverOpen())		//v4.50A20
				SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
			else
			{
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			}
			return Err_FrameJammed;
		}

		if (!IsAllMotorsEnable())		//v3.27T1
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ to ready");
			SetErrorMessage("Gripper is OFF @ to ready");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
#ifndef NU_MOTION	//v3.86
			CMS896AStn::MotionEnableProtection(WL_AXIS_X, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stWLAxis_X);
#endif
			return Err_FrameJammed;
		}
	}

	//Get next frame & scan barcode
	CMSLogFileUtility::Instance()->WL_LogStatus("Get next frame start");

	if (GetExpType() == WL_EXP_CYLINDER)
	{
		SetGripperState(WL_ON);
	}

	INT nRetryCount = 0;
	BOOL bMgznEmpty = FALSE;

	while(1)
	{
		if (!IsAutoLoadUnloadTest() && IsMSAutoLineMode())
		{
			if (!MoveToLoadMgzSlot_AutoLine(bGetWIPMgzSlot_AutoLine))
			{
				HouseKeeping(WL_ON, FALSE, FALSE, TRUE);
				m_bAutoLineLoadDone = FALSE;
				return ERR_NOT_AVALIABLE_INPUT_SLOTS;
			}

			long lOutputSlotNo = -1;
			lOutputSlotNo = GetEmptyOutputSlot_AutoLine(MS_WL_AUTO_LINE_MGZN_NUM - 1);
			if (lOutputSlotNo < 0)
			{
				HouseKeeping(WL_ON, FALSE, FALSE, TRUE);
				m_bAutoLineUnloadDone = FALSE;
				return ERR_OUTPUT_WIP_SLOTS_FULL;
			}
		}

		bRepeatLoadFrame = FALSE;

		nRetryCount++;
		if (nRetryCount > 3)						//v4.44T3	//Requested by SeoulSemi
		{
			SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
			return Err_NoFrameExist;
		}

		// Check frame is on gripper before moving the magazine
		if (IsFrameDetect() == TRUE && m_bNoSensorCheck == FALSE)
		{
			SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
			return Err_FrameExistInGripper;
		}

		//Get next Frame
		CString szErr;
		nFrameStatus = GetNextFilmFrame(bGetWIPMgzSlot_AutoLine, bBurnIn, bSearchFrameInMagazine, bDisplayMsg, bMgznEmpty);
		switch(nFrameStatus)
		{
			case TRUE:	 
				bFrameDetected = TRUE;
				break;

			case FALSE:
				bFrameDetected = FALSE;
				break;
			case ERR_NOT_AVALIABLE_INPUT_SLOTS:
				if (!IsAutoLoadUnloadTest() && IsMSAutoLineMode() && !bGetWIPMgzSlot_AutoLine)
				{
					m_bAutoLineLoadDone = FALSE;
					return ERR_NOT_AVALIABLE_INPUT_SLOTS;
				}

			default:
				SaveData();
				szErr.Format("Get next frame fail; CODE = %d", nFrameStatus);
				CMSLogFileUtility::Instance()->WL_LogStatus(szErr);		//xyz
				return nFrameStatus;
		}
		

		if (!IsAllMotorsEnable())		//v3.27T1
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at LOAD");
			SetErrorMessage("Gripper is OFF @ at LOAD");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		//v4.50A20	//Osram Germany CEMark MS109
		if (IsCoverOpen())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is aborted at LOAD by COVER OPEN");
			SetErrorMessage("Gripper is aborted at LOAD by COVER OPEN");
			SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
			return Err_FrameJammed;
		}

		m_bIsGetCurrentSlot = FALSE;

		//Get frame inside the expander
		if (bFrameDetected == TRUE)
		{
			//Offset for Load Position & On Clip
			X_Move(m_lStepOnLoadPos);

			if (GetExpType() != WL_EXP_CYLINDER)
			{
				SetGripperState(WL_ON);
				Sleep(200);
			}

			if ( (m_bUseBarcode || bSemiAuto) && !bScanBarcodeOnTable )		//v4.39T10
			{
				if (m_bUseBCInCassetteFile)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Start barcode scan in cassette ...");
					nBarcodeStatus = GetBarcodeInCassette(m_szBarcodeName, m_lCurrentSlotNo);

					if (nBarcodeStatus == FALSE)
					{
						SetAlert_Red_Yellow(IDS_WL_BCFILE_NO_BARCODE);
					}
				}
				else if (bSemiAuto)		//Avago	//v4.48A21
				{
					m_szBarcodeName.Empty();	//v4.48A26

					//CPromptBox PromptBox;
					CMSLogFileUtility::Instance()->WL_LogStatus("Start BC scan in semi-auto mode, BC = " + m_szBarcodeName);
				
					for (INT i = 0; ((i < 1) && (m_szBarcodeName.GetLength() == 0)); i++)	//avoid empty input (3 times)
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("No BC found in semi-auto mode");
						if (m_bUseBarcode == TRUE)
						{
							HmiMessage_Red_Back("No Barcode is read!", "Scanning Barcode Message");
						}
						else
						{
							HmiMessage_Red_Back("Barcode scanning is disabled!", "Scanning Barcode Message");	//v3.32T18
						}

						CString szContent	= "Fail to read Barcode! Please input Barcode:";
						HmiMessage_Red_Back(szContent);
						CString szNewBarcode	= _T("");
						if( WL_InputBarcode(1, szNewBarcode) )
						{
							m_szBarcodeName = szNewBarcode;
							SaveBarcodeName();
							break;
						}
					}

					CMSLogFileUtility::Instance()->WL_LogStatus("End BC scan in semi-auto mode, BC = " + m_szBarcodeName);
				}
				else
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Start barcode scan...");
					nBarcodeStatus = ScanningBarcode(TRUE, FALSE, m_bCompareBarcode);
					if (nBarcodeStatus != Err_No_Error && m_bReadBarCodeOnTable)
					{
						bScanBarcodeOnTable = TRUE;
					}

					if (bScanBarcodeOnTable == FALSE)
					{
						if (nBarcodeStatus == Err_No_Error && m_bCompareBarcode == TRUE)
						{
							szCompareBarcode = m_szBarcodeName;

							nBarcodeStatus = ScanningBarcode(TRUE, TRUE, m_bCompareBarcode);
							
							if (szCompareBarcode != m_szBarcodeName)
							{
								nBarcodeStatus = Err_COMPARE_BC_FAILED;
							}
						}
					}
				}

				//Skip below action if burn-in
				if (bScanBarcodeOnTable == FALSE)
				{
					//Check Wafer Lot
					if ((bWaferNameFound = CheckWaferLotData(m_szBarcodeName)) == FALSE) 
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Check WaferLot Data");

						CString szMsg;
						szMsg.Format("Slot %d, Wafer ( ", m_lCurrentSlotNo);
						szMsg += m_szBarcodeName + " ) not found in Lot\nPlease Check!\n\n\nCaution: Wafer will push back into magazine!";
							
						CMSLogFileUtility::Instance()->WL_LogStatus("No WaferName Found on LotInfo");
						SetErrorMessage("No WaferName Found on LotInfo");
						SetAlert_Msg_Red_Yellow(IDS_WL_NO_WAFER_IN_LOT, szMsg);		

						//Push frame back into magazine
						nFrameStatus = PushFrameBack();
						if ( nFrameStatus != TRUE )
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("load frame status not true");
							return nFrameStatus;
						}

						bPushBackAbort = TRUE;

						//OsramTrip 8/22
						if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) ||		
							(GetExpType() == WL_EXP_GEAR_DIRECTRING) )	
						{
							ExpanderDCMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE, m_lExpDCMotorDacValue);
						}
						else if (GetExpType() == WL_EXP_CYLINDER)
						{
							nStatus = ExpanderCylinderMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE);
						}
						else
						{
							ExpanderGearPlatform(WL_DOWN, FALSE, FALSE, FALSE);
						}

						Sleep(200);
						HouseKeeping(WL_ON, FALSE, FALSE, TRUE);
						return Err_COMPARE_BC_FAILED;
					}

					if ( nBarcodeStatus != Err_No_Error )
					{
						if ( nBarcodeStatus == Err_COMPARE_BC_FAILED )
						{
							SetAlert_Red_Yellow(IDS_WL_COMPARE_BC_FAILED);
							SetErrorMessage("Compare Barcode Failed! First Barcode:"+ szCompareBarcode 
											+ "Second Barcode:" + m_szBarcodeName);
						}
						else if ( nBarcodeStatus == ERR_READ_BC_FAILED )
						{
							SetAlert_Red_Yellow(IDS_WL_MAP_NO_NAME);
							SetErrorMessage("Cannot read barcode");
						}
						nFrameStatus = PushFrameBack();
						if ( nFrameStatus == Err_No_Error )
						{
							HouseKeeping(WL_ON, FALSE, FALSE, TRUE);
							return nBarcodeStatus;
						}
						else
						{
							return nFrameStatus;
						}
					}
					//Empty barcode check
					if ((IsMSAutoLineMode() || (m_bBarcodeCheck == TRUE)) && ((nBarcodeStatus == FALSE) || 
						(IsMSAutoLineMode() && (m_szBarcodeName != m_stWaferMagazine[m_lCurrentMagNo - 1].m_SlotBCName[m_lCurrentSlotNo - 1]))))
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Empty Barcode checking");

						CString szText = "Continue : return current wafer and get next one\nStop : return wafer and stop\nIgnore : continue loading current wafer and stop";	
						LONG lSelection = 0; 
					
						if (IsMSAutoLineMode())
						{
							//SetAlert_Red_Yellow(IDS_WL_NOT_MATCHED_SIS_BARCODE);
							// set as reject status for the current slot
							SetSlotRejectStatus(m_lCurrentMagNo - 1, m_lCurrentSlotNo - 1,
												(nBarcodeStatus == FALSE)? MS_FRAME_NO_BARCODE : MS_FRAME_BARCODE_NOT_MATCHED_SIS);
							lSelection = 1;
						}
						else
						{
							SetAlert_Red_Yellow(IDS_WL_NO_BARCODE);
							lSelection = SetAlert_Msg(IDS_WL_RETRY_GET_FRAME, szText, "Continue", "Stop", "Ignore");
						}

						//Push frame back into magazine if (Continue or Stop) is pressed
						if ( (lSelection == 1) || (lSelection == 5) )
						{
							if (lSelection == 5)	//ABORT		//v2.78T1
							{
								bPushBackAbort = TRUE;
								//bFrameDetected = FALSE;		//v4.08	//If enable this then expander will not DOWN
							}

							//v4.59A15	//Renesas MS90
							if (!IsFrameDetect() && m_bNoSensorCheck == FALSE)
							{
								//If no frame on gripper, don't push back or gripper jam may encounter !!!
								CMSLogFileUtility::Instance()->WL_LogStatus("Push Back detects NO-FRAME to abort");
								SetGripperState(WL_OFF);	//shiraishi01
								SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);	//shiraishi01
								return Err_NoFrameDetected;
							}

							nFrameStatus = PushFrameBack();
							if ( nFrameStatus != TRUE )
							{
								CMSLogFileUtility::Instance()->WL_LogStatus("load frame status not true");
								return nFrameStatus;
							}
						}

						//Search next frame in magazine if continue is pressed
						if ( lSelection == 1 )		//CONTINUE
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("Get Next Frame");

							X_MoveTo_Auto(m_lReadyPos_X);		//v4.50A17
							Sleep(500);
							bSearchFrameInMagazine = TRUE;
							bRepeatLoadFrame = TRUE;
							continue;
						}
						else
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("Stop or Ignore");
						}
					}
				

					//v2.78T1
					CString szMsg = "WT1 Barcode loaded = " + m_szBarcodeName;
					CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);


					LONG lCurrSlotNo = GetCurrSlotNo();
					m_szWfrBarcodeInCass[lCurrSlotNo] = m_szBarcodeName;
					
					//AUTOLINE
					if (lCurrSlotNo > 0)
					{
						//Slot1 -> INDEX = 0 (0-based)
						m_stWaferMagazine[0].m_SlotBCName[lCurrSlotNo-1] = m_szBarcodeName;
					}
				}
			}
			else
			{
				if (CMS896AApp::m_bEnableGripperLoadUnloadTest == TRUE)
				{
					m_szBarcodeName = "LoadUnloadTest";
					SaveBarcodeName();
				}
			}
		}

		CMSLogFileUtility::Instance()->WL_LogStatus("Get next frame finish");
		SaveData();

		//v4.50A20	//Osram Germany CEMark MS109
		if (IsCoverOpen())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is aborted at LOAD by COVER OPEN");
			SetErrorMessage("Gripper is aborted at LOAD by COVER OPEN");
			SetGripperState(WL_OFF);	//v4.50A21
			SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
			return Err_FrameJammed;
		}

		if (bFrameDetected == TRUE || m_bNoSensorCheck == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper to UNLOAD...");

			//Move Gripper to load pos
			INT nXResult = gnOK;
			if (CMS896AStn::m_bCEMark)	//v4.50A24
				nXResult = X_MoveTo_Auto(m_lUnloadPos_X, SFM_WAIT, TRUE);		//v4.5021
			else
				nXResult = X_MoveTo_Auto(m_lUnloadPos_X);						//v4.50A17

			if (CMS896AStn::m_bCEMark && (nXResult != gnOK))		//bv4.50A20
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is aborted to UNLOAD by COVER OPEN");
				SetErrorMessage("Gripper is aborted to UNLOAD by COVER OPEN");
				SetGripperState(WL_OFF);						//v4.50A21
				SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
				return Err_FrameJammed;
			}
			if(m_bWLXHasEncoder && (nXResult != gnOK) )
			{
				CString szMsg;
				szMsg = "Gripper is aborted to UNLOAD by some error";
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				SetErrorMessage(szMsg);
			//	HmiMessage_Red_Back(szMsg, "Wafer Loader");
				SetGripperState(WL_OFF);						//v4.50A21
				return Err_FrameJammed;
			}
			GetEncoderValue();
			LONG lEncX1 = m_lEnc_X;

			//Get more into expander
			if (GetExpType() == WL_EXP_VACUUM)
			{
				FilmFrameDriveIn(HP_POSITIVE_DIR, 30, 50, TRUE);
			}
			else if (GetExpType() != WL_EXP_GEAR_DIRECTRING)
			{
				FilmFrameDriveIn(HP_POSITIVE_DIR, 200, 50, TRUE);
			}

			GetEncoderValue();
			LONG lEncX2 = m_lEnc_X;
			//v4.59A35
			CString szLog;
			szLog.Format("Gripper to UNLOAD Complete; EncX at UNLOAD = %ld, after DRIVEIN = %ld",
							lEncX1, lEncX2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

			if (GetExpType() == WL_EXP_VACUUM)
			{
				if (IsMS60())	//v4.46T28
				{
					bFrameExisted = IsFrameDetect();	//TRUE;		//v4.48A4
				}
				else
				{
					bFrameExisted = (IsFramePosition() && IsFrameProtection());	//Check Frame Position sensor
				}
			}
			else
			{
				CString szMsg;
				if (pApp->GetCustomerName() == "OSRAM")
				{
					bFrameDetected = IsFrameDetect();
					szMsg.Format("OSRAM: WL frame detect = %d", bFrameDetected);
					CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				}

				CMSLogFileUtility::Instance()->WL_LogStatus("Check Frame Exist");
				bFrameExisted = IsFrameExist();		//Check Frame Exist sensor
				CMSLogFileUtility::Instance()->WL_LogStatus("Check Frame Exist Complete");
				szMsg.Format("WL frame exist = %d, frame detect = %d", bFrameExisted, IsFrameDetect());
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			}

			if ( (bBurnIn == TRUE) || (bWaferNameFound == FALSE) )
			{
				bFrameExisted = TRUE;
			}
		}
		else
		{
			if (!bPushBackAbort)	//v4.08	//However, PushBack Abort will not go here because bFrameDetect is still TRUE at this point
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("No frame is detected");
				SetErrorMessage("No frame is detected");
				if (!bMgznEmpty)	//v4.59A16
				{
					SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
				}
			}
		}

		if (((bFrameDetected == TRUE) && (bFrameExisted = TRUE)) || m_bNoSensorCheck == TRUE)
		{
			if (GetExpType() == WL_EXP_VACUUM)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander starts to move down...");
				ExpanderVacuumPlatform(WL_DOWN, TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander at down posn...:A");

				//Off Clip
				Sleep(100);
				SetGripperState(WL_OFF);
				Sleep(100);

				//Align Frame
				AlignWaferFrame(TRUE);
				if ( (HouseKeeping(WL_ON, FALSE, TRUE, TRUE) != 1) && (bBurnIn == FALSE) )
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot close");
					SetErrorMessage("Expander cannot close");
					SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);	
					return Err_ExpanderOpenCloseFailed;
				}
			}
			else
			{
				//Off Clip
				Sleep(100);
				BOOL bIsFrameDetect = FALSE;
				INT nCounter = 0;
				if( GetExpType()==WL_EXP_GEAR_DIRECTRING )
				{
					while( bIsFrameDetect==FALSE )
					{
						if (IsFrameDetect() == TRUE)
						{
							bIsFrameDetect = TRUE;
							CMSLogFileUtility::Instance()->WL_LogStatus("WL frame in clipper before open");
							break;
						}

						Sleep(10);
						nCounter++;

						if( nCounter>=10 )
						{
							bIsFrameDetect = FALSE;
							break;
						}
					}
				}
				CMSLogFileUtility::Instance()->WL_LogStatus("Start Open Gripper State");
				SetGripperState(WL_OFF);
				Sleep(100);

				// add retry on frame detect
				nCounter = 0;
				while( bIsFrameDetect==FALSE )
				{
					if (IsFrameDetect() == TRUE)
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("WL frame in clipper after open");
						bIsFrameDetect = TRUE;
						break;
					}

					Sleep(10);
					nCounter++;
					if( nCounter>=100 )
					{
						bIsFrameDetect = FALSE;
						break;
					}
				}
			
				//Check Frame detect sensor again to ensure the frame is in right position
				if ((bIsFrameDetect == FALSE && bBurnIn == FALSE) && m_bNoSensorCheck == FALSE)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("WL frame detect fail on wafer table");
					if (!bPushBackAbort)		//v4.08
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Cannot detect frame again");
						SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
					}
					
					if (bPushBackAbort)		//v4.01T1
					{
						//safe to Down expander here if pushback frame to STOP	//Cree China
						if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) ||		//v4.01		
							(GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28
						{
							ExpanderDCMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE, m_lExpDCMotorDacValue);
						}
						else if (GetExpType() == WL_EXP_CYLINDER)
						{
							nStatus = ExpanderCylinderMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE);
						}
						else
						{
							ExpanderGearPlatform(WL_DOWN, FALSE, FALSE, FALSE);
						}
					}

					Sleep(200);
					HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);	//expander doesn't close to avoid breaking plastic frame
					return Err_NoFrameExist;
				}

				if( (m_bUseBarcode && (nBarcodeStatus==TRUE) && !bScanBarcodeOnTable) && IsES101()==FALSE )
				{
					nConvLoadMap = LoadMapFileWithoutSyn(m_szBarcodeName);
				}
				else if (bSemiAuto)		//v4.48A33
				{
					if (m_szBarcodeName.GetLength() > 0)
					{
						nConvLoadMap = LoadMapFileWithoutSyn(m_szBarcodeName);
						if (!SyncLoadMapFile(nConvLoadMap))
						{
							Sleep(200);
							HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);	//expander doesn't close to avoid breaking plastic frame
							return Err_NoFrameExist;
						}
					}
				}

	#ifdef NU_MOTION
				X_MoveTo(0);	//-250);	//v3.86	//modified as advised by production
	#else
				if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Start Move to Home Offset");

					X_MoveTo(WL_GRIPPER_SRV_HOME_OFFSET_POS);	//v3.69T1

					CMSLogFileUtility::Instance()->WL_LogStatus("End Move to Home Offset");
				}
				else
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Start Move to Home Offset");

					X_MoveTo(-1000);

					CMSLogFileUtility::Instance()->WL_LogStatus("End Move to Home Offset");
				}

	#endif

				CMSLogFileUtility::Instance()->WL_LogStatus("Expander starts to gear down...");

				if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||		//v4.01	
					(GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28
				{
					nStatus = ExpanderDCMotorPlatform(WL_DOWN, TRUE, bWaferNameFound, TRUE, m_lExpDCMotorDacValue, FALSE);
				}
				else if (GetExpType() == WL_EXP_CYLINDER)
				{
					nStatus = ExpanderCylinderMotorPlatform(WL_DOWN, TRUE, bWaferNameFound, TRUE);
				}
				else
				{
					nStatus = ExpanderGearPlatform(WL_DOWN, TRUE, bWaferNameFound, TRUE);
				}

				if (nStatus != 1)
				{
#ifndef NU_MOTION		//Klocwork	//v4.46
					if (nStatus == Err_NoFrameExist)
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("No Frame Exist");
						SetErrorMessage("No Frame Exist");
						SetAlert_Red_Yellow(IDS_WL_EXP_NO_FRMAE_EXIST);
						HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
						return Err_NoFrameExist;
					}
					else
#endif
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot close");
						SetErrorMessage("Expander cannot close");
						SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);
						HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
						return Err_ExpanderOpenCloseFailed;
					}
				}

				CMSLogFileUtility::Instance()->WL_LogStatus("Expander at down posn...:B");
				HouseKeeping(WL_ON, FALSE, TRUE, TRUE);

				if (bScanBarcodeOnTable == TRUE)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Scan Barcode On Table..");

					if ((nBarcodeStatus = ScanningBarcodeOnTableWithTheta(m_szBarcodeName)) == TRUE)
					{				
						if ((bWaferNameFound = CheckWaferLotData(m_szBarcodeName)) == FALSE) 
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("Check WaferLot Data");

							CString szMsg;
							szMsg.Format("Slot %d, Wafer ( ", m_lCurrentSlotNo);
							szMsg += m_szBarcodeName + " ) not found in Lot\nPlease Check!\n\n\nCaution: Wafer will push back into magazine!";
								
							CMSLogFileUtility::Instance()->WL_LogStatus("No WaferName Found on LotInfo");
							SetErrorMessage("No WaferName Found on LotInfo");
							SetAlert_Msg_Red_Yellow(IDS_WL_NO_WAFER_IN_LOT, szMsg);		
							
							return Err_NoFrameFoundOnLot;
						}		
					}

					if ((IsMSAutoLineMode() || (m_bBarcodeCheck == TRUE)) && ((nBarcodeStatus == FALSE) || 
						(IsMSAutoLineMode() && (m_szBarcodeName != m_stWaferMagazine[m_lCurrentMagNo - 1].m_SlotBCName[m_lCurrentSlotNo - 1]))))
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Empty Barcode checking on table");

						CString szText = " ";	
						LONG lSelection = 0; 

						if (IsMSAutoLineMode())
						{
							//	SetAlert_Red_Yellow(IDS_WL_NOT_MATCHED_SIS_BARCODE);
							// set as reject status for the current slot
							SetSlotRejectStatus(m_lCurrentMagNo - 1, m_lCurrentSlotNo - 1,
												(nBarcodeStatus == FALSE)? MS_FRAME_NO_BARCODE : MS_FRAME_BARCODE_NOT_MATCHED_SIS);
							lSelection = 1;
						}
						else
						{
							SetAlert_Red_Yellow(IDS_WL_NO_BARCODE);
							lSelection = SetAlert_Msg(IDS_WL_RETRY_GET_FRAME, szText, "Continue", "Stop", "Ignore");
						}

						//Push frame back into magazine if (Continue or Stop) is pressed
						if ( (lSelection == 1) || (lSelection == 5) )
						{
							if (lSelection == 5)	//ABORT		//v2.78T1
							{
								return FALSE;
							}

							nFrameStatus = PushFrameBackFromTable(bStartFromHome, bBurnIn);
							if ( nFrameStatus != TRUE )
							{
								CMSLogFileUtility::Instance()->WL_LogStatus("Push frame back status not true");
								return nFrameStatus;
							}
						}

						//Search next frame in magazine if continue is pressed
						if ( lSelection == 1 )		//CONTINUE
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("Get Next Frame");

							X_MoveTo_Auto(m_lReadyPos_X);	//v4.50A17
							Sleep(500);
							bSearchFrameInMagazine = TRUE;
							bRepeatLoadFrame = TRUE;
						}
						else
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("Stop or Ignore");
						}
					}
				}

				if( m_bUseContour )
				{
					//	CONTOUR	search/learn contour edge, expander down, barcode reading done
					LearnWLContourAuto(FALSE);
				}
			}
		}
		else
		{
			if ((bFrameDetected == FALSE) || (bFrameExisted == FALSE))
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("WL: no frame is detected");
				if (!bMgznEmpty)	//v4.59A16
				{
					SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
				}
				HouseKeeping(WL_ON, FALSE, FALSE, TRUE);
			}
			else if (IsCoverOpen())	//v4.50A21
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is aborted at LOAD by COVER OPEN 2");
				SetErrorMessage("Gripper is aborted at LOAD by COVER OPEN 2");
				SetGripperState(WL_OFF);	//v4.50A21
				SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
				return Err_FrameJammed;
			}
			else
			{
				HouseKeeping(WL_ON, FALSE, FALSE, TRUE);
			}
		}

		if (bRepeatLoadFrame == FALSE)
		{
			break;
		}
	}

	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER) && (IsExpanderLock() == TRUE) )
	{
		Sleep(1000);
		if (IsExpanderLock())	//v3.90
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander unlock failed at Load FilmFrame");
			SetErrorMessage("Expander unlock failed");

			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);
			return Err_ExpanderLockFailed;
		}
	}

	//Exist if burn-in is enable
	if ( bBurnIn == TRUE )
	{
		//v4.31T11
		if (CMS896AApp::m_bMS100Plus9InchOption)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("9Inch table to HOME (BURNIN)");
			MoveWaferTableLoadUnload(0, 0, FALSE);
		}

		if( m_bUseDualTablesOption==FALSE )
			WPR_MoveToFocusLevel();
		if (bPushBackAbort)
			m_bFrameExistOnExp = FALSE;			//v2.78T1
		else
			m_bFrameExistOnExp = TRUE;	
		CMSLogFileUtility::Instance()->WL_LogStatus("Load finish (BURNIN)");
		m_bFrameLoadedOnWT1	= TRUE;
		return Err_No_Error;
	}


	//Check Frame is exit on expander or not
	if (GetExpType() == WL_EXP_VACUUM)
	{
		if (IsMS60())	//v4.46T28
		{
			//v4.48A4
			//bFrameExisted	= TRUE;
			if (bFrameExisted)
				m_bFrameExistOnExp	= TRUE;
			else
				m_bFrameExistOnExp	= FALSE;
		}
		else
		{
			bFrameExisted		= IsExpanderVacuum();	
			m_bFrameExistOnExp	= bFrameExisted;
		}
	}
	else
	{
		bFrameExisted = TRUE;		//IsFrameExist();	
		if (bPushBackAbort)
			m_bFrameExistOnExp = FALSE;					//v2.78T1
		else
			m_bFrameExistOnExp = bFrameExisted;			//v2.64
	}

	SaveData();

	if (bFrameExisted == FALSE && m_bNoSensorCheck == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("No frame is detected inside expander");
		SetErrorMessage("No frame is detected inside expander");
		SetAlert_Red_Yellow(IDS_WL_FRAME_LOAD_FAIL);
		return Err_NoFrameExist;
	}

	if ( bFrameDetected == FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("frame detect false");
		return Err_NoFrameDetected;
	}

	if ( bWaferNameFound == FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("frame found false");
		return Err_WaferNameNotFound;
	}

	//Load the frame to wafer table
	SendGemEvent_WL_Load(TRUE);

	m_lLoadedSlotNo = m_lCurrentMagNo; //only for load event CEID# 8007
	//===================================================================================
	//if the frame(WIP&Input) was loaded into table, it should be set the slot as EMPTY, for RGV to load frame
	//find a output slot and move the input slot information to this output slot.
	// set the status as "BONDING"
	CString szMsg;
	szMsg.Format("SubLoadFilmFrame IsInputSlot_AutoLine(AUTOLINE) is %ld, %d", m_lCurrentMagNo, m_lCurrentSlotNo);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	if (!IsAutoLoadUnloadTest() && IsMSAutoLineMode())
	{
		LONG lOutputSlotNo = GetEmptyOutputSlot_AutoLine(m_lCurrentMagNo - 1);
		szMsg.Format("SubLoadFilmFrame GetEmptyOutputSlot_AutoLine(AUTOLINE) is %ld, %d", m_lCurrentMagNo, lOutputSlotNo + 1);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		if (lOutputSlotNo >= 0)
		{
			TransferBondingMgznSlot_AutoLine(m_lCurrentMagNo - 1, m_lCurrentSlotNo - 1, m_lCurrentMagNo - 1, lOutputSlotNo);
			m_lCurrentSlotNo = lOutputSlotNo + 1;
		}
	}
	//===================================================================================

	CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(3);	//WAFER_LOAD	//v4.40T5

	//v4.21T2	//MS100 9Inch	
	if (CMS896AApp::m_bMS100Plus9InchOption)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("9Inch table to HOME");
		MoveWaferTableLoadUnload(0, 0, FALSE);
	}

	if (bScanBarcodeOnTable && GetExpType() == WL_EXP_GEAR_DIRECTRING)
	{
		bKeepBarcodePos = TRUE;
	}

	//rotate WT before load map
	INT nRotateUnderCam = pApp->GetProfileInt(gszPROFILE_SETTING, gszES_ROTATE_T_UNDER_CAM, 0);
	if( (IsES101()==FALSE && IsES201()==FALSE) || (nRotateUnderCam==0) )
	{
		AutoRotateWafer(bKeepBarcodePos);
	}

	(*m_psmfSRam)["WaferTable"]["WT1LoadMgzn"]	= m_lCurrentMagNo;
	(*m_psmfSRam)["WaferTable"]["WT1LoadSlot"]	= m_lCurrentSlotNo;
	(*m_psmfSRam)["WaferTable"]["WT1InBarcode"]	= m_szBarcodeName;

	m_bFrameLoadedOnWT1	= TRUE;
	CString szConvLoadMap;
	szConvLoadMap.Format("%d",nConvLoadMap);
	CMSLogFileUtility::Instance()->WL_LogStatus("Align Fram Wafer Start; barcode:" + m_szBarcodeName + "," + szConvLoadMap);
	m_szLoadAlignBarcodeName = m_szBarcodeName;
	return AlignFrameWafer(bFrameDetected, bFrameExisted, bSearhHomeDie, TRUE, nConvLoadMap);

	return Err_No_Error;
}


INT CWaferLoader::AlignWaferFrame(BOOL bSearchFrame)
{
	LONG i;

	if (IsMS60())	//v4.46T28
		return TRUE;

	for (i=0; i<m_lWTAlignFrameCount; i++)
	{
		Sleep(200);
		SetFrameAlign(WL_ON);
		Sleep(200);
		SetFrameAlign(WL_OFF);
	}
	Sleep(200);

	if ( bSearchFrame == TRUE )
	{
		//Re-search filmframe to avoid frame is shifted after frame align action
		if ( IsFrameDetect() == FALSE )
		{
			FilmFrameSearch();
			Sleep(200);
			if ( IsFrameDetect() == TRUE )
			{
				X_Move(m_lStepOnLoadPos);

				//On Clip
				Sleep(100);
				SetGripperState(WL_ON);
				Sleep(100);

				//v4.50A17	//Check Cover Sensor
				//Move Gripper to load pos
				INT nResult = X_MoveTo_Auto(m_lUnloadPos_X);
				if (nResult == FALSE)
				{
					return Err_NoFrameExist;
				}

				//Get more into expander
				FilmFrameDriveIn(HP_POSITIVE_DIR, 200, 50, TRUE);

				//Off Clip
				Sleep(100);
				SetGripperState(WL_OFF);
				Sleep(100);
			}
			else
			{
				return Err_NoFrameExist;
			}
		}
	}

	return TRUE;
}


// ------- Signal Wafer Table to Search the barcode ---------//
// -- nXYDirection = 0 for Moving X, nXYDirection = 1 for Moving Y -- //
BOOL CWaferLoader::MoveWaferTableForBarCode(INT nXYDirection,INT siDirection, LONG lSearchDistance, BOOL bWaitComplete)
{
	IPC_CServiceMessage stMsg;
	int	nConvID = 0;
	BOOL bReturn;

	typedef struct
	{
		INT	nXYDirection;
		INT siDirection;
		LONG lSearchDistance;
		BOOL bWaitComplete;
			
	}TABLE_MOVE ;

	TABLE_MOVE stInfo;
	stInfo.nXYDirection = nXYDirection;
	stInfo.siDirection = siDirection;
	stInfo.lSearchDistance	= lSearchDistance;
	stInfo.bWaitComplete = bWaitComplete;

	stMsg.InitMessage(sizeof(TABLE_MOVE), &stInfo );

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ReadBarCodeOnTable", stMsg);
	
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID,30000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
	}

	return bReturn;
}


// ------- Signal Wafer Table to Sync Motors X, Y , T for Wafer Table -------// 
BOOL CWaferLoader::SyncWaferTableXYT(CString szDirection)
{
	IPC_CServiceMessage stMsg;
	INT nConvID = 0;

	BOOL bReturn = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bReturn );

	if (szDirection.MakeUpper() == "X")
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "X_SyncCmd", stMsg);
	}
	else if (szDirection.MakeUpper() == "Y")
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "Y_SyncCmd", stMsg);
	}
	else if (szDirection.MakeUpper() == "T")
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T_SyncCmd", stMsg);
	}
	//aV2.83T4
	else if (szDirection.MakeUpper() == "XY")
	{
		nConvID	= m_comClient.SendRequest(WAFER_TABLE_STN, "XY_SyncCmd", stMsg);
	}

	if (nConvID > 0)	//v2.93T1
	{
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 100000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(1);
			}
		}
	}

	return bReturn;
}

INT CWaferLoader::CheckBarcodeInMgzn(BOOL bStartFromHome, BOOL bSearchFrameInMagazine, BOOL bStopAtHome)
{
	BOOL bFrameDetected = FALSE;
	BOOL bFrameExisted = FALSE;
	BOOL bWaferNameFound = TRUE;
	LONG lCurrentX = 0;
	LONG lCurrentY = 0;
	LONG nSearchStatus = 0;
	INT nStatus = 0;
	LONG lLoadPosX = 0;

	IPC_CServiceMessage stMsg;
	int		nConvID = 0;

	if( WPR_MoveFocusToSafe()==FALSE )
	{
		return Err_AutoFocus_Z_Fail;
	}

	//Check Banana scope
	if ( IsScopeDown() == TRUE )
	{
		SetAlert_Red_Yellow(IDS_WL_SCOPE_DOWN);
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return Err_ScopeDown;
	}

	//Check Magazine is inside the loader Z
	if ( (IsMagazineExist() == FALSE) )
	{
		SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);	
		HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
		return Err_NoMagazineExist;
	}

	SetWaferTableJoystick(FALSE);

	if (bStartFromHome == TRUE)
	{
		if (GetExpType() == WL_EXP_VACUUM)
		{
			INT nStatus = FilmFrameVacuumReady();
			if ( nStatus != TRUE)
			{
				HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
				return Err_ExpanderOpenCloseFailed;
			}
		}
		else
		{
			FilmFrameGearReady();
			INT nStatus = 0;

			if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||		//v4.01	
				 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28
			{
				nStatus = ExpanderDCMotorPlatform(WL_UP, TRUE, TRUE, FALSE, m_lExpDCMotorDacValue);
			}
			else if (GetExpType() == WL_EXP_CYLINDER)
			{
				nStatus = ExpanderCylinderMotorPlatform(WL_UP, TRUE, TRUE, FALSE);
			}
			else
			{
				nStatus = ExpanderGearPlatform(WL_UP, TRUE, TRUE, FALSE);
			}
			
			if ( nStatus == Err_ExpanderAlreadyOpen )
			{
				SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
				HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
				return Err_ExpanderAlreadyOpen;
			}
			else if ( nStatus != TRUE )
			{
				SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);	
				HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
				return Err_ExpanderOpenCloseFailed;
			}
		}

		//Move Gripper to load pos
		nSearchStatus = MoveSearchFrameExistAndJam(m_lReadyPos_X);
		if ( nSearchStatus == Err_FrameJammed )
		{
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE, TRUE);
			return Err_FrameJammed;
		}
		else if ( nSearchStatus == Err_FrameExistInGripper )
		{
			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONGRIPPER);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameExistInGripper;
		}

		if (IsFrameJam() == TRUE)
		{
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}
	}

	
	//Searching FilmFrame until end
	while(1)
	{
		if (bSearchFrameInMagazine == TRUE)
		{
			if (m_lCurrentSlotNo > m_lTotalSlotNo)
			{
				m_lCurrentSlotNo = 1;
				MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
				Z_Home();
				MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);

//				if (bBurnIn == FALSE)
				{
					CString szMsg;
					SetAlert_Msg_NoMaterial(IDS_WL_MAG_FULL, szMsg);
					HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);

					//Close expander
					if ( GetExpType() == WL_EXP_VACUUM )
					{
						ExpanderVacuumPlatform(WL_DOWN, FALSE);
					}
					else
					{
						if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||		//v4.01		
							 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28
						{
							nStatus = ExpanderDCMotorPlatform(WL_DOWN, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
						}
						else if (GetExpType() == WL_EXP_CYLINDER)
						{
							nStatus = ExpanderCylinderMotorPlatform(WL_DOWN, TRUE, FALSE, FALSE);
						}
						else
						{
							nStatus = ExpanderGearPlatform(WL_DOWN, TRUE, FALSE, FALSE);
						}
						
						if (nStatus != 1 )
						{
							SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);
						}
					}
					HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);

					return FALSE; 
				}
			}
		}


		//Enable Limit sensor protection
#ifndef NU_MOTION	//v3.86
		CMS896AStn::MotionEnableProtection(WL_AXIS_X, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stWLAxis_X);
#endif
		//Move Gripper to load pos & do searching
		if (GetExpType() == WL_EXP_CYLINDER)
		{
			lLoadPosX = m_lLoadPos_X + WL_GRIPPER_CYLINDER_SEARCH_DIST;	
		}
		else 
		{
			lLoadPosX = m_lLoadPos_X + WL_GRIPPER_SEARCH_DIST;
		}

		if (lLoadPosX > 0)
		{
			lLoadPosX = 0;
		}

		X_MoveTo(lLoadPosX);

		if( IsGripperMissingSteps() )
		{
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		if (IsFrameJam() == TRUE)
		{
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		//Disable Limit sensor protection
		//m_pStepper_X->DisableProtection(HP_LIMIT_SENSOR);
#ifndef NU_MOTION	//v3.86
		CMS896AStn::MotionEnableProtection(WL_AXIS_X, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stWLAxis_X);
#endif
		FilmFrameSearch();

		//Check Frame detect sensor
		bFrameDetected = IsFrameDetect();

		if (bFrameDetected == TRUE)
		{
			break;
		}
		else
		{
			if (bSearchFrameInMagazine == TRUE)
			{
				X_MoveTo(m_lReadyPos_X);
			}
			else
			{
				break;
			}
		}
	}//End search FilmFrame

	//Enable Limit sensor protection
	//m_pStepper_X->EnableProtection(HP_LIMIT_SENSOR, TRUE);
	//CMS896AStn::MotionEnableProtection("WaferGripperAxis", HP_LIMIT_SENSOR, TRUE, TRUE);

	if (bFrameDetected == TRUE)
	{
		//Offset for Load Position
		X_Move(m_lStepOnLoadPos);

		//On Clip
		SetGripperState(WL_ON);

		if (m_bUseBarcode == TRUE)
		{
			ScanningBarcode();

			//if ( m_bMapWfrCass == TRUE )
			//{
			LONG lCurrSlotNo = GetCurrSlotNo();
			m_szWfrBarcodeInCass[lCurrSlotNo] = m_szBarcodeName;

			//v4.59A10	//AUTOLINE
			if (lCurrSlotNo > 0)
			{
				//Slot1 -> INDEX = 0 (0-based)
				m_stWaferMagazine[0].m_SlotBCName[lCurrSlotNo-1] = m_szBarcodeName;
			}
			//}
		}

	}
	else
	{
	}

	//Move Gripper to load pos
	X_MoveTo(m_lLoadPos_X);
	if( IsGripperMissingSteps() )
	{
		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}
	if (IsFrameJam() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	//Disable Limit sensor protection
	//Off Clip
	Sleep(100);
	SetGripperState(WL_OFF);
	Sleep(100);

	//Move Gripper to load pos
	X_MoveTo(m_lReadyPos_X);

		
	if (bStopAtHome == TRUE)
	{
		if (GetExpType() != WL_EXP_VACUUM)
		{
			X_MoveTo(-1000);

			INT nStatus = 0;
			if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||		//v4.01	
				 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28
			{
				nStatus = ExpanderDCMotorPlatform(WL_DOWN, TRUE, TRUE, FALSE, m_lExpDCMotorDacValue);
			}
			else if (GetExpType() == WL_EXP_CYLINDER)
			{
				nStatus = ExpanderCylinderMotorPlatform(WL_DOWN, TRUE, TRUE, FALSE);
			}
			else
			{
				nStatus = ExpanderGearPlatform(WL_DOWN, TRUE, TRUE, FALSE);
			}

			if (nStatus != 1)
			{
				SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);	
				HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
				return Err_ExpanderOpenCloseFailed;
			}

			HouseKeeping(WL_OFF, FALSE, TRUE, TRUE);
		}
		else
		{
			HouseKeeping(WL_OFF, FALSE, TRUE, TRUE);
		}
	}

	return TRUE;
}


LONG CWaferLoader::WaferGripperMoveSearchJam(INT nPos, BOOL bOpenGripper, LONG lMotionTime)
{
	INT nResult;

	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableWL)	//v3.60
		return TRUE;
	if (m_bDisableWLWithExp)	//v3.61
	{
		return TRUE;
	}

	INT nCount = 0;
	INT nJamCount = 0;
	DOUBLE dStartTime = GetTime();
	nResult = X_MoveTo(nPos, SFM_NOWAIT);
	
	while(1)
	{
		if ( nResult != gnOK )
		{
			return FALSE;
		}
		
		//if ( m_pStepper_X->IsComplete() == TRUE )
		if ( CMS896AStn::MotionIsComplete(WL_AXIS_X, &m_stWLAxis_X) == TRUE )
		{
			X_Sync();
			break;
		}

		if ( !X_IsPowerOn() )	//v3.80
		{
			return FALSE;
		}

		if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
		{
			if ( IsFrameJam() == TRUE )
			{
				nJamCount++;
				//Sleep(100);
	
				if (nJamCount >= 50)
				{
					//m_pStepper_X->Stop();
					CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
					return FALSE;
				}
			}
		}
		else
		{
			if ( IsFrameJam() == TRUE )
			{
				//m_pStepper_X->Stop();
				CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
				return FALSE;
			}

			if ( IsCoverOpen() == TRUE )	//v4.50A17
			{
				CMS896AStn::MotionStop(WL_AXIS_X, &m_stWLAxis_X);
				return FALSE;
			}
		}

		if (bOpenGripper == TRUE)
		{
			LONG lDiff = (LONG)(GetTime() - dStartTime);

			if (lDiff >= lMotionTime/2)
			{
				//AfxMessageBox("Gripper Open", MB_SYSTEMMODAL);
				SetGripperState(WL_OFF);
			}
		}

		//v3.80
		nCount++;
		if ( nCount > 5000 )
		{
			return FALSE;
		}

		Sleep(1);
	}

	return TRUE;
}

