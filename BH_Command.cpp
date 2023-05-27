/////////////////////////////////////////////////////////////////
// BH_Command.cpp : HMI Registered Command of the CBondHead class
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
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "HmiDataManager.h"
#include "WaferStat.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "SecsComm.h"
#include "BinTable.h"
#include "WaferPr.h"
#include "BondPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CBondHead::RegisterVariables()
{
	try
	{
		// Register variable
		RegVariable(_T("BH_bDisableBH"),			&m_bDisableBH);
		RegVariable(_T("BH_bNuMotionSystem"),		&m_bNuMotionSystem);
		RegVariable(_T("BH_bHomeSnr_Z"),			&m_bHomeSnr_Z);
		RegVariable(_T("BH_bHomeSnr_T"),			&m_bHomeSnr_T);
		RegVariable(_T("BH_bHomeSnr_Ej"),			&m_bHomeSnr_Ej);
		RegVariable(_T("BH_bHomeSnr_EjElevatorZ"),	&m_bHomeSnr_EjElevatorZ);
		RegVariable(_T("BH_bHomeSnr_EjCap"),		&m_bHomeSnr_EjCap);
		RegVariable(_T("BH_bHomeSnr_EjT"),			&m_bHomeSnr_EjT);

		RegVariable(_T("BH_bHomeSnr_BinEj"),			&m_bHomeSnr_BinEj);
		RegVariable(_T("BH_bHomeSnr_BinEjElevatorZ"),	&m_bHomeSnr_BinEjElevatorZ);
		RegVariable(_T("BH_bHomeSnr_BinEjCap"),			&m_bHomeSnr_BinEjCap);
		RegVariable(_T("BH_bHomeSnr_BinEjT"),			&m_bHomeSnr_BinEjT);

		RegVariable(_T("BH_bContactSensor"),		&m_bContactSensor);
		RegVariable(_T("BH_bContactSensorZ2"),		&m_bContactSensorZ2);
		RegVariable(_T("BH_bMissingDieSnr"),		&m_bMissingDieSnr);
		RegVariable(_T("BH_bMissingDieSnrZ2"),		&m_bMissingDieSnrZ2);
		RegVariable(_T("BH_bAirFlowSensor"),		&m_bPressureSnr);
		RegVariable(_T("BH_bVacuumFlowSensor"),		&m_bVacuumFlowSnr);

		RegVariable(_T("BH_CEMark"),				&m_bCEMark);

		RegVariable(_T("BH_bCleanColletSafetySensor"),	&m_bCleanColletSafetySensor_HMI);
		RegVariable(_T("SYS_bFrontCover"),			&m_bMainFrontCover_HMI);
		RegVariable(_T("SYS_bFrontLeftCover"),		&m_bMainFrontLeftCover_HMI);
		RegVariable(_T("SYS_bFrontMiddleCover"),	&m_bMainFrontMiddleCover_HMI);
		RegVariable(_T("SYS_bRearLeftCover"),		&m_bMainRearLeftCover_HMI);
		RegVariable(_T("SYS_bRearRightCover"),		&m_bMainRearRightCover_HMI);
		RegVariable(_T("SYS_bLeftCover"),			&m_bMainLeftCover_HMI);
/*
		RegVariable(_T("BH_bMachineCoverSensor"),	&m_bMachineFrontCover_HMI);
		RegVariable(_T("BH_bIsFrontLeftCoverOpen"),	&m_bFrontLeftCoverOpen_HMI);
		RegVariable(_T("BH_bIsFrontRightCoverOpen"), &m_bFrontMiddleCoverOpen_HMI);

		RegVariable(_T("BH_bIsRightCoverOpen"),		&m_bFrontMiddleCoverOpen_HMI);
		RegVariable(_T("BH_bIsSideCoverOpen"),		&m_bIsSideCoverOpen);
*/


		RegVariable(_T("BH_bPosLimitSnr_Z"),		&m_bPosLimitSnr_Z);
		RegVariable(_T("BH_bNegLimitSnr_Z"),		&m_bNegLimitSnr_Z);
		RegVariable(_T("BH_bPosLimitSnr_Z2"),		&m_bPosLimitSnr_Z2);
		RegVariable(_T("BH_bNegLimitSnr_Z2"),		&m_bNegLimitSnr_Z2);
		RegVariable(_T("BH_bPosLimitSnr_T"),		&m_bPosLimitSnr_T);
		RegVariable(_T("BH_bNegLimitSnr_T"),		&m_bNegLimitSnr_T);
		RegVariable(_T("BH_bFan"),					&m_bBHFan);
		//MS60 BHT Theraml Fan Sensor bits		//v4.48A10
		RegVariable(_T("BH_bMS60BHZFan1"),			&m_bMS60BHZFan1);
		RegVariable(_T("BH_bMS60BHZFan2"),			&m_bMS60BHZFan2);
		RegVariable(_T("BH_bMS60BHTUpFan1"),		&m_bMS60BHTUpFan1);
		RegVariable(_T("BH_bMS60BHTUpFan2"),		&m_bMS60BHTUpFan2);
		RegVariable(_T("BH_bMS60BHTLowFan1"),		&m_bMS60BHTLowFan1);
		RegVariable(_T("BH_bMS60BHTLowFan2"),		&m_bMS60BHTLowFan2);
		//v4.59A12	//Renesas MS90
		RegVariable(_T("BH_bBH1PressureSnr"),		&m_bBH1PressureSnr);
		RegVariable(_T("BH_bBH2PressureSnr"),		&m_bBH2PressureSnr);
		RegVariable(_T("BH_bEjPressureSnr"),		&m_bEjPressureSnr);
		RegVariable(_T("BH_bInputIonizerSnr"),		&m_bInputIonizerSnr);
		RegVariable(_T("BH_bOutputIonizerSnr"),		&m_bOutputIonizerSnr);
		RegVariable(_T("BH_bCatchAndBlowSnrZ"),		&m_bCatchAndBlowSnrZ);
		RegVariable(_T("BH_bCatchAndBlowSnrY"),		&m_bCatchAndBlowSnrY);
		RegVariable(_T("BH_bCheckEjPressureSnr"),	&m_bCheckEjPressureSnr);	//v4.59A26
		RegVariable(_T("BH_bCheckBH1PressureSnr"),	&m_bCheckBH1PressureSnr);	//v4.59A26
		RegVariable(_T("BH_bCheckBH2PressureSnr"),	&m_bCheckBH2PressureSnr);	//v4.59A26

		//v4.24T10	//ES101
		RegVariable(_T("BH_bNGPickDnSnr"),			&m_bNGPickDnSnr);
		RegVariable(_T("BH_bNGPickUpSnr"),			&m_bNGPickUpSnr);
		RegVariable(_T("BH_bTensionCheck"),			&m_bTensionCheck);
		RegVariable(_T("BH_bTapeEmpty"),			&m_bTapeEmpty);
		//v4.51A5
		RegVariable(_T("BH_bMS60EjElevator"),		&m_bMS60EjElevator);

		RegVariable(_T("BH_lEnc_Z"),				&m_lEnc_Z);
		RegVariable(_T("BH_lEnc_Z2"),				&m_lEnc_Z2);
		RegVariable(_T("BH_lEnc_T"),				&m_lEnc_T);
		RegVariable(_T("BH_lEnc_Ej"),				&m_lEnc_Ej);
		RegVariable(_T("BH_lEnc_EjX"),				&m_lEnc_EjX);
		RegVariable(_T("BH_lEnc_EjY"),				&m_lEnc_EjY);
		RegVariable(_T("BH_lEnc_EjElevatorZ"),		&m_lEnc_EjElevatorZ);
		RegVariable(_T("BH_lEnc_EjCap"),			&m_lEnc_EjCap);	
		RegVariable(_T("BH_lEnc_EjT"),				&m_lEnc_EjT);	
		RegVariable(_T("BH_lEnc_BinEj"),			&m_lEnc_BinEj);
		RegVariable(_T("BH_lEnc_BinEjElevatorZ"),	&m_lEnc_BinEjElevatorZ);
		RegVariable(_T("BH_lEnc_BinEjCap"),			&m_lEnc_BinEjCap);	
		RegVariable(_T("BH_lEnc_BinEjT"),			&m_lEnc_BinEjT);

		RegVariable(_T("BH_lCmd_ChgColletT"),		&m_lCurPos_ChgColletT);
		RegVariable(_T("BH_lCmd_ChgColletZ"),		&m_lCurPos_ChgColletZ);
		
		RegVariable(_T("BH_bSel_Z"),				&m_bSel_Z);
		RegVariable(_T("BH_bSel_Z2"),				&m_bSel_Z2);
		RegVariable(_T("BH_bSel_T"),				&m_bSel_T);
		RegVariable(_T("BH_bSel_Ej"),				&m_bSel_Ej);
		RegVariable(_T("BH_bSel_BinEj"),			&m_bSel_BinEj);
		
		RegVariable(_T("BH_bIsPowerOn_Z"),			&m_bIsPowerOn_Z);
		RegVariable(_T("BH_bIsPowerOn_Z2"),			&m_bIsPowerOn_Z2);
		RegVariable(_T("BH_bIsPowerOn_T"),			&m_bIsPowerOn_T);
		RegVariable(_T("BH_bIsPowerOn_Ej"),			&m_bIsPowerOn_Ej);
		RegVariable(_T("BH_bIsPowerOn_EjT"),		&m_bIsPowerOn_EjT);
		RegVariable(_T("BH_bIsPowerOn_EjX"),		&m_bIsPowerOn_EjX);
		RegVariable(_T("BH_bIsPowerOn_EjY"),		&m_bIsPowerOn_EjY);
		RegVariable(_T("BH_bIsPowerOn_EjElevator"),	&m_bIsPowerOn_EjElevator);
		RegVariable(_T("BH_bIsPowerOn_EjCap"),		&m_bIsPowerOn_EjCap);
		RegVariable(_T("BH_bIsPowerOn_BinEj"),			&m_bIsPowerOn_BinEj);
		RegVariable(_T("BH_bIsPowerOn_BinEjT"),			&m_bIsPowerOn_BinEjT);
		RegVariable(_T("BH_bIsPowerOn_BinEjElevator"),	&m_bIsPowerOn_BinEjElevator);
		RegVariable(_T("BH_bIsPowerOn_BinEjCap"),		&m_bIsPowerOn_BinEjCap);

		RegVariable(_T("BH_bIsPowerOn_ChgColletT"),		&m_bIsPowerOn_ChgColletT);
		RegVariable(_T("BH_bIsPowerOn_ChgColletZ"),		&m_bIsPowerOn_ChgColletZ);

		//Motion Test
		RegVariable(_T("BH_ucMotionAxis"),				&m_ucMotionAxis);
		//RegVariable(_T("BH_bMotionCycleStop"),			&m_bMotionCycleStop);
		RegVariable(_T("BH_lMotionCycle"),				&m_lMotionCycle);
		RegVariable(_T("BH_lMotionTestDelay"),			&m_lMotionTestDelay);

		// collet record
		RegVariable(_T("BH_bEnableToolsUsageRecord"),	&CMS896AStn::m_bEnableToolsUsageRecord);	//v4.53A25
		RegVariable(_T("BH_bShowColletDiagPage"),		&m_oColletRecord.m_bShowColletDiagPage);
		RegVariable(_T("BH_szColletDiagOperatorId"),	&m_oColletRecord.m_szOperatorId);
		RegVariable(_T("BH_szColletDiagReason"),		&m_oColletRecord.m_szReason);
		RegVariable(_T("BH_szColletDiagRemark"),		&m_oColletRecord.m_szRemark);
		RegVariable(_T("BH_szColletDiagShiftNo"),		&m_oColletRecord.m_szShiftNo);
		RegVariable(_T("BH_szColletDiagType"),			&m_oColletRecord.m_szType);
		RegVariable(_T("BH_szColletColletMode"),		&m_oColletRecord.m_szColletMode);

		// ejector record
		RegVariable(_T("BH_bShowEjectorDiagPage"),		&m_oEjectorRecord.m_bShowEjectorDiagPage);
		RegVariable(_T("BH_szEjectorDiagOperatorId"),	&m_oEjectorRecord.m_szOperatorId);
		RegVariable(_T("BH_szEjectorDiagReason"),		&m_oEjectorRecord.m_szReason);
		RegVariable(_T("BH_szEjectorDiagRemark"),		&m_oEjectorRecord.m_szRemark);
		RegVariable(_T("BH_szEjectorDiagShiftNo"),		&m_oEjectorRecord.m_szShiftNo);
		RegVariable(_T("BH_szEjectorDiagType"),			&m_oEjectorRecord.m_szType);
		RegVariable(_T("BH_szEjectorDiagUsageType"),	&m_oEjectorRecord.m_szUsageType);
		RegVariable(_T("BH_szEjSoftTouch"),				&m_szEjSoftTouch);
		RegVariable(_T("BH_szEjCapSoftTouch"),			&m_szEjCapSoftTouch);
		RegVariable(_T("BH_szEjSoftTouchMsg"),			&m_szEjSoftTouchMsg);

		RegVariable(_T("BH_bIsArm2Exist"),				&m_bIsArm2Exist);					// for BHA
		RegVariable(_T("BH_lCycleSpeedMode"),			&CMS896AApp::m_lCycleSpeedMode);	//v3.80
		RegVariable(_T("BH_lDBHBondHeadMode"),			&m_lDBHBondHeadMode);	
		RegVariable(_T("BH_bMS100EjtXY"),				&m_bMS100EjtXY);					//v4.42T1
		RegVariable(_T("BH_dCycleTime"),				&m_dCycleTime);
		RegVariable(_T("BH_dAvgCycleTime"),				&m_dAvgCycleTime);

		RegVariable(_T("BH_lSwingOffset_Z"),			&m_lSwingOffset_Z);
		RegVariable(_T("BH_lSwingOffset_Z2"),			&m_lSwingOffset_Z2);
		// Auto Clean Collet
		RegVariable(_T("BH_lCleanCollet_Z"),			&m_lAutoCleanCollet_Z);
		RegVariable(_T("BH_lCleanCollet_T"),			&m_lAutoCleanCollet_T);
		RegVariable(_T("BH_lBPAccPosT"),				&m_lBPAccPosT);
		RegVariable(_T("BH_lBPAccLvlZ"),				&m_lBPAccLvlZ);
		RegVariable(_T("BH_lAutoCleanOffsetZ"),			&m_lAutoCleanOffsetZ);
		RegVariable(_T("BH_lAutoCleanAirBlowLimit"),	&m_lAutoCleanAirBlowLimit);			
		RegVariable(_T("BH_lAutoCleanAutoLearnZLimit"),	&m_lAutoCleanAutoLearnZLimit);		//v4.46T21
		RegVariable(_T("BH_lAutoCleanAutoLearnZCounter"),&m_lAutoCleanAutoLearnZCounter);	//v4.46T21
		RegVariable(_T("BH_lAutoCleanAutoLearnZOffset"),&m_lAutoCleanAutoLearnZOffset);		//v4.46T21
		RegVariable(_T("BH_bAutoCleanAutoLearnZLevels"),&m_bAutoCleanAutoLearnZLevels);		//v4.49A6

		RegVariable(_T("BH_lCDiePos_EjX"),				&m_lCDiePos_EjX);
		RegVariable(_T("BH_lCDiePos_EjY"),				&m_lCDiePos_EjY);
		//v4.42T15
		RegVariable(_T("BH_bEnableMS100EjtXY"),			&m_bEnableMS100EjtXY);
		RegVariable(_T("BH_lArm1Pos_EjX"),				&m_lArm1Pos_EjX);
		RegVariable(_T("BH_lArm1Pos_EjY"),				&m_lArm1Pos_EjY);
		RegVariable(_T("BH_lArm2Pos_EjX"),				&m_lArm2Pos_EjX);
		RegVariable(_T("BH_lArm2Pos_EjY"),				&m_lArm2Pos_EjY);

		RegVariable(_T("BH_lBT2OffsetZ"),				&m_lBT2OffsetZ);					//v4.21
		RegVariable(_T("BH_lNGPickIndexT"),				&m_lNGPickIndexT);					//v4.24

		//Wafer Ejector StandBy and Up Level
//		RegVariable(_T("BH_lUpLevel_El"),				&m_lUpLevel_El);
		RegVariable(_T("BH_lStandby_El"),				&m_lStandby_EJ_Cap);
		RegVariable(_T("BH_lDnOffset_El"),				&m_lDnOffset_EJ_Cap);
		RegVariable(_T("BH_lUpLevel_El"),				&m_lUpLevel_EJ_Cap);
		RegVariable(_T("BH_lStandby_EjElv"),			&m_lStandby_EJ_Elevator);
		RegVariable(_T("BH_lUpLevel_EjElv"),			&m_lUpLevel_EJ_Elevator);
		RegVariable(_T("BH_lStandby_EjT"),				&m_lStandby_EJ_Theta);
		RegVariable(_T("BH_lUpLevel_EjT"),				&m_lUpLevel_EJ_Theta);

		RegVariable(_T("BH_bLearnStandby_EJ_Pin"),		&m_bLearnStandby_EJ_Pin);
		RegVariable(_T("BH_bLearnContact_EJ_Pin"),		&m_bLearnContact_EJ_Pin);
		RegVariable(_T("BH_bLearnStandby_EJ_Cap"),		&m_bLearnStandby_EJ_Cap);
		RegVariable(_T("BH_bLearnStandby_EJ_Elevator"),	&m_bLearnStandby_EJ_Elevator);
		RegVariable(_T("BH_bLearnStandby_EJ_Theta"),	&m_bLearnStandby_EJ_Theta);
		RegVariable(_T("BH_bLearnUpLevel_EJ_Pin"),		&m_bLearnUpLevel_EJ_Pin);
		RegVariable(_T("BH_bLearnUpLevel_EJ_Cap"),		&m_bLearnUpLevel_EJ_Cap);
		RegVariable(_T("BH_bLearnUpLevel_EJ_Elevator"),	&m_bLearnUpLevel_EJ_Elevator);
		RegVariable(_T("BH_bLearnUpLevel_EJ_Theta"),	&m_bLearnUpLevel_EJ_Theta);

		//Bin Ejector StandBy and Up Level
		RegVariable(_T("BH_lStandby_BinEjCap"),			&m_lStandby_Bin_EJ_Cap);
		RegVariable(_T("BH_lUplevel_BinEjCap"),			&m_lUpLevel_Bin_EJ_Cap);
		RegVariable(_T("BH_lStandby_BinEjElv"),			&m_lStandby_Bin_EJ_Elevator);
		RegVariable(_T("BH_lUplevel_BinEjElv"),			&m_lUpLevel_Bin_EJ_Elevator);
		RegVariable(_T("BH_lStandby_BinEjT"),			&m_lStandby_Bin_EJ_Theta);
		RegVariable(_T("BH_lUplevel_BinEjT"),			&m_lUpLevel_Bin_EJ_Theta);
		RegVariable(_T("BH_lStandby_BinEj"),			&m_lStandby_Bin_EJ_Pin);
		RegVariable(_T("BH_lUplevel_BinEj"),			&m_lUpLevel_Bin_EJ_Pin);

		//RegVariable(_T("BH_lBPGeneral_0"),  &m_lBPGeneral_0);
		RegVariable(_T("BH_lBPGeneral_1"),		&m_lBPGeneral_1);
		RegVariable(_T("BH_lBPGeneral_2"),		&m_lBPGeneral_2);
		RegVariable(_T("BH_lBPGeneral_3"),		&m_lBPGeneral_3);
		RegVariable(_T("BH_lBPGeneral_4"),		&m_lBPGeneral_4);
		RegVariable(_T("BH_lBPGeneral_5"),		&m_lBPGeneral_5);
		RegVariable(_T("BH_lBPGeneral_6"),		&m_lBPGeneral_6);
		RegVariable(_T("BH_lBPGeneral_7"),		&m_lBPGeneral_7);
		RegVariable(_T("BH_lBPGeneral_8"),		&m_lBPGeneral_8);
		RegVariable(_T("BH_lBPGeneral_9"),		&m_lBPGeneral_9);
		RegVariable(_T("BH_bBeginPadsLevel"),	&m_bBeginPadsLevel);
		RegVariable(_T("BH_lEjOffset"),			&m_lEjMoveDistance);

		RegVariable(_T("BH_lBPGeneral_A"),		&m_lBPGeneral_A);
		RegVariable(_T("BH_lBPGeneral_B"),		&m_lBPGeneral_B);
		RegVariable(_T("BH_lBPGeneral_C"),		&m_lBPGeneral_C);
		RegVariable(_T("BH_lBPGeneral_D"),		&m_lBPGeneral_D);
		RegVariable(_T("BH_lBPGeneral_E"),		&m_lBPGeneral_E);
		RegVariable(_T("BH_lBPGeneral_F"),		&m_lBPGeneral_F);
		RegVariable(_T("BH_lBPGeneral_H"),		&m_lBPGeneral_H);
		RegVariable(_T("BH_lBPGeneral_G"),		&m_lBPGeneral_G);
		RegVariable(_T("BH_lBPGeneral_TmpA"),	&m_lBPGeneral_TmpA);
		RegVariable(_T("BH_lBPGeneral_TmpB"),	&m_lBPGeneral_TmpB);
		RegVariable(_T("BH_lBPGeneral_TmpC"),	&m_lBPGeneral_TmpC);
		RegVariable(_T("BH_lBPGeneral_TmpD"),	&m_lBPGeneral_TmpD);
		RegVariable(_T("BH_lBPGeneral_TmpE"),	&m_lBPGeneral_TmpE);
		RegVariable(_T("BH_lBPGeneral_BHZ1"),	&m_lBPGeneral_BHZ1);
		RegVariable(_T("BH_lBPGeneral_BHZ2"),	&m_lBPGeneral_BHZ2);
		RegVariable(_T("BH_lBPGeneral_BHZ3"),	&m_lBPGeneral_BHZ3);
		RegVariable(_T("BH_lBPGeneral_BHZ4"),	&m_lBPGeneral_BHZ4);

		RegVariable(_T("BH_bOnVacuumAtPick"),	&m_bOnVacuumAtPick);
		RegVariable(_T("BH_lBinTableDelay"),	&m_lBinTableDelay);
		RegVariable(_T("BH_lPRDelay"),			&m_lPRDelay);
		RegVariable(_T("BH_lWTTDelay"),			&m_lWTTDelay);
		RegVariable(_T("BH_lWTSettlingDelay"),	&m_lWTSettlingDelay);
		RegVariable(_T("BH_lWeakBlowDelay"),	&m_lWeakBlowOffDelay);
		RegVariable(_T("BH_lHighPressureBlowDelay"), &m_lHighBlowTime);
		RegVariable(_T("BH_lArmPickDelay"),		&m_lArmPickDelay);
		RegVariable(_T("BH_lHeadPickDelay"),	&m_lHeadPickDelay);
		RegVariable(_T("BH_lEjectorUpDelay"),	&m_lEjectorUpDelay);
		RegVariable(_T("BH_lPickDelay"),		&m_lPickDelay);
		RegVariable(_T("BH_lArmBondDelay"),		&m_lArmBondDelay);
		RegVariable(_T("BH_lEjectorDownDelay"), &m_lEjectorDownDelay);
		RegVariable(_T("BH_lHeadBondDelay"),	&m_lHeadBondDelay);
		RegVariable(_T("BH_lBondDelay"),		&m_lBondDelay);
		RegVariable(_T("BH_lFloatBlowDelay"),	&m_lFloatBlowDelay);
		RegVariable(_T("BH_lBlockCameraDelay"),	&m_lBlockCameraDelay);		//v4.52A3
		RegVariable(_T("BH_lPickDriveIn"),		&m_lPickDriveIn);
		RegVariable(_T("BH_lBondDriveIn"),		&m_lBondDriveIn);
		RegVariable(_T("BH_szPickDriveIn"),		&m_szPickDriveInDisplay);
		RegVariable(_T("BH_szBondDriveIn"),		&m_szBondDriveInDisplay);
		RegVariable(_T("BH_lEjectorDriveIn"),	&m_lEjectorDriveIn);
		RegVariable(_T("BH_lEjectorLimitType"),	&m_lEjectorLimitType);
		RegVariable(_T("BH_bUseDefaultDelays"),	&m_bUseDefaultDelays);		//v2.97T1
		RegVariable(_T("BH_bIsLinearEjector"),	&m_bIsLinearEjector);		//Linear Ej
		RegVariable(_T("BH_lEjVacOffDelay"),	&m_lEjVacOffDelay);			//v3.78
		RegVariable(_T("BH_lHeadPrePickDelay"),	&m_lHeadPrePickDelay);		//v3.79
		RegVariable(_T("BH_lSyncPickDelay"),	&m_lSyncPickDelay);			//v3.83
		RegVariable(_T("BH_lTCEjtUpPrDelay"),	&m_lTCUpPreheatTime);
		RegVariable(_T("BH_lTCDnOnVacDelay"),	&m_lTCDnOnVacDelay);
		RegVariable(_T("BH_lTCUpHeatingTime"),	&m_lTCUpHeatingTime);
		RegVariable(_T("BH_bTCLayerPick"),		&m_bTCLayerPick);
		RegVariable(_T("BH_lUplookDelay"),		&m_lUplookDelay);
		RegVariable(_T("BH_lNeutralDelay"),		&m_lNeutralDelay);

		
		RegVariable(_T("BH_bEjectorCapVacuumOn"),	&m_bEjectorCapVacuumOn);
		RegVariable(_T("BH_bCheckColletJam"),	&m_bCheckColletJam);
		RegVariable(_T("BH_bCheckMissingDie"),	&m_bCheckMissingDie);
		RegVariable(_T("BH_bUseBHAutoLearn"),	&m_bUseBHAutoLearn);
		RegVariable(_T("BH_bUseEjAutoLearn"),	&m_bUseEjAutoLearn);
		RegVariable(_T("BH_bCheckCoverSensor"),	&m_bCheckCoverSensor);
		RegVariable(_T("BH_bScanCheckCoverSensor"),	&m_bScanCheckCoverSensor);
		RegVariable(_T("BH_bCoverSensorAlwaysOn"),	&m_bCoverSensorAlwaysOn);		//v4.28T6
		RegVariable(_T("BH_lAutobondTimeInMin"),	&m_lAutobondTimeInMin);			//v2.83T6
		RegVariable(_T("BH_bUseCTSensorLearn"),		&m_bUseCTSensorLearn);			//v2.96T4
		RegVariable(_T("BH_bAutoCleanCollet"),		&m_bAutoCleanCollet);			//v3.64		//Lexter
		RegVariable(_T("BH_bACCSearchColletHole"),	&m_bACCSearchColletHole);		//v4.53A23
		RegVariable(_T("BH_bNeutralVacState"),		&m_bNeutralVacState);			//v3.81
		RegVariable(_T("BH_lAccLiquidExpireTime"),	&m_lLiquidExpireTime);			//	seconds

		RegVariable(_T("BH_bCheckPr2DCode"),		&m_bCheckPr2DCode);				//v4.40T1
		RegVariable(_T("BH_lPr2DCodeCheckLimit"),	&m_lPr2DCodeCheckLimit);		//v4.41T3

		RegVariable(_T("BH_ulColletCount"),			&m_ulColletCount);
		RegVariable(_T("BH_ulCollet2Count"),		&m_ulCollet2Count);				//v3.92
		RegVariable(_T("BH_ulCleanCount"),			&m_ulCleanCount);
		RegVariable(_T("BH_ulEjectorCount"),		&m_ulEjectorCount);
		RegVariable(_T("BH_ulMaxColletCount"),		&m_ulMaxColletCount);
		RegVariable(_T("BH_ulMaxCollet2Count"),		&m_ulMaxCollet2Count);			//v3.92
		RegVariable(_T("BH_ulMaxEjectorCount"),		&m_ulMaxEjectorCount);
		RegVariable(_T("BH_ulMaxCleanCount"),		&m_ulMaxCleanCount);
		RegVariable(_T("BH_ulCleanColletTimeOut"),	&m_ulCleanColletTimeOut);

		RegVariable(_T("BH_lEjectorKOffset"),			&m_lEjectorKOffset);
		RegVariable(_T("BH_lEjectorKCount"),			&m_lEjectorKCount);
		RegVariable(_T("BH_lEjectorKOffsetLimit"),		&m_lEjectorKOffsetLimit);
		RegVariable(_T("BH_lEjectorKOffsetTotal"),		&m_lEjectorKOffsetTotal);
		RegVariable(_T("BH_lEjectorKOffsetForBHZ1MD"),	&m_lEjectorKOffsetForBHZ1MD);//M68
		RegVariable(_T("BH_lEjectorKOffsetBHZ1Total"),	&m_lEjectorKOffsetBHZ1Total);
		RegVariable(_T("BH_lEjectorKOffsetForBHZ2MD"),	&m_lEjectorKOffsetForBHZ2MD);//M68
		RegVariable(_T("BH_lEjectorKOffsetBHZ2Total"),	&m_lEjectorKOffsetBHZ2Total);
		RegVariable(_T("BH_lEjSubRegionKOffset"),		&m_lEjSubRegionKOffset);
		RegVariable(_T("BH_lEjSubRegionKCount"),		&m_lEjSubRegionKCount);
		RegVariable(_T("BH_lEjSubRegionKOffsetTotal"),	&m_lEjSubRegionKOffsetTotal);
		RegVariable(_T("BH_lEjSubRegionSKOffset"),		&m_lEjSubRegionSKOffset);
		RegVariable(_T("BH_lEjSubRegionSKCount"),		&m_lEjSubRegionSKCount);
		RegVariable(_T("BH_lEjSubRegionSKOffsetTotal"),	&m_lEjSubRegionSKOffsetTotal);
		//v4.53A25
		RegVariable(_T("BH_lZ1SmartKOffset"),			&m_lZ1SmartKOffset);
		RegVariable(_T("BH_lZ1SmartKCount"),			&m_lZ1SmartKCount);
		RegVariable(_T("BH_lZ1SmartKOffsetTotal"),		&m_lZ1SmartKOffsetTotal);
		RegVariable(_T("BH_lZ2SmartKOffset"),			&m_lZ2SmartKOffset);
		RegVariable(_T("BH_lZ2SmartKCount"),			&m_lZ2SmartKCount);
		RegVariable(_T("BH_lZ2SmartKOffsetTotal"),		&m_lZ2SmartKOffsetTotal);
		RegVariable(_T("BH_bCheckK1K2"),				&m_bCheckK1K2);					//M68
		RegVariable(_T("BH_lEjectorKOffsetResetCount"),	&m_lEjectorKOffsetResetCount);					//M69
		RegVariable(_T("BH_bAutoChangeColletOnOff"),	&m_bAutoChangeColletOnOff);	

		RegVariable(_T("BH_lDAFlowBH1KOffset"),		&m_lDAFlowBH1KOffset);
		RegVariable(_T("BH_lDAFlowBH1KCount"),		&m_lDAFlowBH1KCount);
		RegVariable(_T("BH_lDAFlowBH1KOffsetLimit"),&m_lDAFlowBH1KOffsetLimit);
		RegVariable(_T("BH_lDAFlowBH1KOffsetTotal"),&m_lDAFlowBH1KOffsetTotal);
		RegVariable(_T("BH_lDAFlowBH2KOffset"),		&m_lDAFlowBH2KOffset);
		RegVariable(_T("BH_lDAFlowBH2KCount"),		&m_lDAFlowBH2KCount);
		RegVariable(_T("BH_lDAFlowBH2KOffsetLimit"),&m_lDAFlowBH2KOffsetLimit);
		RegVariable(_T("BH_lDAFlowBH2KOffsetTotal"),&m_lDAFlowBH2KOffsetTotal);
		RegVariable(_T("BH_lMissingDie_Retry"),		&m_lMissingDie_Retry);
		RegVariable(_T("BH_ulColletJam_Retry"),		&m_ulColletJam_Retry);
		RegVariable(_T("BH_lMDCycleLimit"),			&m_lMDCycleLimit);
		RegVariable(_T("BH_lNGNoDieLimit"),			&m_lNGNoDieLimit);
		RegVariable(_T("BH_lMDCycleCount1"),		&m_lMDCycleCount1);
		RegVariable(_T("BH_lMDCycleCount2"),		&m_lMDCycleCount2);
		RegVariable(_T("BH_lBHUplookPrFailLimit"),	&m_lBHUplookPrFailLimit);			//v4.59A5

		RegVariable(_T("BH_bMDRetryOn"),			&m_bMDRetryOn);						//v4.22T1	//HMI display RETRY sensor
		RegVariable(_T("BH_bMD2RetryOn"),			&m_bMD2RetryOn);					//v4.24T12	//HMI display RETRY sensor
		RegVariable(_T("BH_bCJRetryOn"),			&m_bCJRetryOn);						//v4.24T12	//HMI display RETRY sensor
		RegVariable(_T("BH_bCJ2RetryOn"),			&m_bCJ2RetryOn);					//v4.24T12	//HMI display RETRY sensor
		RegVariable(_T("BH_bEnableCustomCount1"),	&m_bEnableCustomCount1);
		RegVariable(_T("BH_bEnableCustomCount2"),	&m_bEnableCustomCount2);
		RegVariable(_T("BH_bEnableCustomCount3"),	&m_bEnableCustomCount3);
		RegVariable(_T("BH_ulCustomCounter1"),		&m_ulCustomCounter1);
		RegVariable(_T("BH_ulCustomCounter2"),		&m_ulCustomCounter2);
		RegVariable(_T("BH_ulCustomCounter3"),		&m_ulCustomCounter3);
		RegVariable(_T("BH_ulCustomCounter1Limit"),		&m_ulCustomCounter1Limit);
		RegVariable(_T("BH_ulCustomCounter2Limit"),		&m_ulCustomCounter2Limit);
		RegVariable(_T("BH_ulCustomCounter3Limit"),		&m_ulCustomCounter3Limit);
		RegVariable(_T("BH_lCleanColletSwingTime"),		&m_lAutoCleanColletSwingTime);		//v3.77
		RegVariable(_T("BH_lAccLastDropDownTime"),		&m_lAccLastDropDownTime);
		RegVariable(_T("BH_bResetEjKOffsetAtWaferEnd"),	&m_bResetEjKOffsetAtWaferEnd);		//v4.44A4
		RegVariable(_T("BH_bMDRetryUseAutoCleanCollet"),&m_bMDRetryUseAutoCleanCollet);		//v4.49A3

		RegVariable(_T("BH_bEnableSyncMove"),	&m_bEnableSyncMove);
		RegVariable(_T("BH_lSyncDistance"),		&m_lSyncDistance);
		RegVariable(_T("BH_dSyncSpeed"),		&m_dSyncSpeed);
		RegVariable(_T("BH_lSyncTime"),			&m_lSyncTime);

		RegVariable(_T("BH_ulPickDieCount"),	&m_ulPickDieCount);
		RegVariable(_T("BH_ulBondDieCount"),	&m_ulBondDieCount);
		RegVariable(_T("BH_ulDefectDieCount"),	&m_ulDefectDieCount);
		RegVariable(_T("BH_ulBadCutDieCount"),	&m_ulBadCutDieCount);
		RegVariable(_T("BH_ulMissingDieCount"),	&m_ulMissingDieCount);
		RegVariable(_T("BH_ulRotateDieCount"),	&m_ulRotateDieCount);
		RegVariable(_T("BH_ulEmptyDieCount"),	&m_ulEmptyDieCount);

		RegVariable(_T("BH_ulCurrWaferBondDieCount"),	&m_ulCurrWaferBondDieCount);
		RegVariable(_T("BH_ulCurrWaferDefectDieCount"),	&m_ulCurrWaferDefectDieCount);
		RegVariable(_T("BH_ulCurrWaferChipDieCount"),	&m_ulCurrWaferChipDieCount);
		RegVariable(_T("BH_ulCurrWaferInkDieCount"),	&m_ulCurrWaferInkDieCount);
		RegVariable(_T("BH_ulCurrWaferBadCutDieCount"),	&m_ulCurrWaferBadCutDieCount);
		RegVariable(_T("BH_ulCurrWaferRotateDieCount"),	&m_ulCurrWaferRotateDieCount);
		RegVariable(_T("BH_ulCurrWaferEmptyDieCount"),	&m_ulCurrWaferEmptyDieCount);
		RegVariable(_T("BH_dCurrWaferSelGradeYield"),	&m_dCurrWaferSelGradeYield);
		RegVariable(_T("BH_ulCurrWaferMissingDieCount"),	&m_ulCurrWaferMissingDieCount);

		RegVariable(_T("BH_lStepMove"),					&m_lStepMove);

		// Register for global use (static variables)
		RegVariable(_T("MS_bStepMoveEnable"),			&m_bStepMoveEnable);
		RegVariable(_T("MS_bHeapCheck"),				&m_bHeapCheck);

		//Main cover
		RegVariable(_T("BH_bIsFrontCoverExist"),		&m_bIsFrontCoverExist);
		RegVariable(_T("BH_bIsSideCoverExist"),			&m_bIsSideCoverExist);
		RegVariable(_T("BH_bIsBinElevatorCoverExist"),	&m_bIsBinElevatorCoverExist);
		RegVariable(_T("BH_bEnableFrontCoverLock"),		&m_bEnableFrontCoverLock);
		RegVariable(_T("BH_bEnableSideCoverLock"),		&m_bEnableSideCoverLock);
		RegVariable(_T("BH_bEnableBinElevatorCoverLock"), &m_bEnableBinElevatorCoverLock);
		RegVariable(_T("BH_bFrontCoverLock"),			&m_bFrontCoverLock);
		RegVariable(_T("BH_bSideCoverLock"),			&m_bSideCoverLock);
		RegVariable(_T("BH_bBinElevatorCoverLock"),		&m_bBinElevatorCoverLock);

		//BH Air-Flow sensor test
		RegVariable(_T("BH_bEnableAirFlowTest"),		&m_bEnableAirFlowTest);	
		RegVariable(_T("BH_bMDResponseChecking"),		&m_bEnableMDResponseChecking);		//v3.94
		RegVariable(_T("BH_lAirFlowTestTimeOut"),		&m_lAirFlowTestTimeOut);
		RegVariable(_T("BH_lAirFlowMdTime"),			&m_lAirFlowMdTime);	
		RegVariable(_T("BH_lAirFlowJcTime"),			&m_lAirFlowJcTime);	
		RegVariable(_T("BH_lTTravelTime"),				&m_lTTravelTime);
		//BH-Z OPEN-DAC Test
		RegVariable(_T("BH_lBHZOpenDacValue"),			&m_lBHZOpenDacValue);
		RegVariable(_T("BH_bEnableOpenDacBondOption"),	&m_bEnableOpenDacBondOption);

		RegVariable(_T("BH_lBondHeadDelay"),			&m_lBondHeadDelay);
		RegVariable(_T("BH_szSqOfDacFromBToP"),			&m_szSqOfDacFromBondToPick);
		RegVariable(_T("BH_szSqOfDacFromPToB"),			&m_szSqOfDacFromPickToBond);
		RegVariable(_T("BH_szSumOfDac"),				&m_szSumOfDac);

		// UBH Variable
		RegVariable(_T("BH_lPreEjectorUpDelay"),			&m_lPreEjectorUpDelay);
		RegVariable(_T("BH_bUBHContactSearch"),				&m_bUBHContactSearch);
		RegVariable(_T("BH_lUBHPickLevelDac_Z"),			&m_lUBHPickLevelDac_Z);
		RegVariable(_T("BH_lUBHOpenDacOffset_Z"),			&m_lUBHOpenDacOffset_Z);
		RegVariable(_T("BH_lUBHOpenDacStepSize_Z"),			&m_lUBHOpenDacStepSize_Z);
		RegVariable(_T("BH_lUBHOpenDacMax_Z"),				&m_lUBHOpenDacMax_Z);
		RegVariable(_T("BH_lUBHStepOffset_Z"),				&m_lUBHStepOffset_Z);
		RegVariable(_T("BH_lUBHSearchSpeed_Z"),				&m_lUBHSearchSpeed_Z);
		RegVariable(_T("BH_lUBHSearchDriveInSpeed_Z"),		&m_lUBHSearchDriveInSpeed_Z);
		RegVariable(_T("BH_lUBHSearchDriveInSample_Z"),		&m_lUBHSearchDriveInSample_Z);
		RegVariable(_T("BH_lUBHMinPosErrTriggerSample_Z"),	&m_lUBHMinPosErrTriggerSample_Z);
		RegVariable(_T("BH_lUBHSearchTriggerPosErr"),		&m_lUBHSearchTriggerPosErr);
		RegVariable(_T("BH_bEnableSeparteSettlingBlk_T"),	&m_bEnableSeparteSettlingBlk_T);
		RegVariable(_T("BH_lUBHSettlingTime_T"),			&m_lUBHSettlingTime_T);
		RegVariable(_T("BH_lUBHMoveTime_Z"),				&m_lUBHMoveTime_Z);
		RegVariable(_T("BH_lUBHEnc_Z"),						&m_lUBHEnc_Z);

		RegVariable(_T("BH_bEnableThermalTest"),			&m_bEnableThermalTest);
		RegVariable(_T("BH_lCollectHoleCatpureTime"),		&m_lCollectHoleCatpureTime);
		//v4.49A5
		RegVariable(_T("BH_bDBHHeatingCoilFcn"),			&CMS896AStn::m_bDBHHeatingCoilFcn);
		RegVariable(_T("BH_bDBHThermostat"),				&CMS896AStn::m_bDBHThermostat);
		RegVariable(_T("BH_bNGPick"),						&CMS896AStn::m_bNGPick);		//v4.51A19	//Silan MS90
		RegVariable(_T("BH_bEnableMotionLogT"),				&m_bEnableMotionLogT);
		RegVariable(_T("BH_bEnableMotionLogZ"),				&m_bEnableMotionLogZ);
		RegVariable(_T("BH_bEnableMotionLogZ2"),			&m_bEnableMotionLogZ2);
		RegVariable(_T("BH_bEnableMotionLogEJ"),			&m_bEnableMotionLogEJ);
		RegVariable(_T("BH_bEnableMotionLogEJTX"),			&m_bEnableMotionLogEJTX);
		RegVariable(_T("BH_bEnableMotionLogEJTY"),			&m_bEnableMotionLogEJTY);

		//BondHead ToleranceLevel
		RegVariable(_T("BH_lBondHeadToleranceLevel"),		&m_lBondHeadToleranceLevel);
		RegVariable(_T("BH_lZPBLevelOffsetLimit"),			&m_lZPBLevelOffsetLimit);
		
		//Open DAC
		RegVariable(_T("BH_bEnableBHZOpenDac"),		&m_bEnableBHZOpenDac);
		RegVariable(_T("BH_lBHZ1_PickDAC"),			&m_lBHZ1_PickDAC);
		RegVariable(_T("BH_lBHZ1_BondDAC"),			&m_lBHZ1_BondDAC);
		RegVariable(_T("BH_lBHZ2_PickDAC"),			&m_lBHZ2_PickDAC);
		RegVariable(_T("BH_lBHZ2_BondDAC"),			&m_lBHZ2_BondDAC);
		RegVariable(_T("BH_lBHZ1_PickDACOffset"),	&m_lBHZ1_PickDACOffset);
		RegVariable(_T("BH_lBHZ1_BondDACOffset"),	&m_lBHZ1_BondDACOffset);
		RegVariable(_T("BH_lBHZ2_PickDACOffset"),	&m_lBHZ2_PickDACOffset);
		RegVariable(_T("BH_lBHZ2_BondDACOffset"),	&m_lBHZ2_BondDACOffset);

		RegVariable(_T("BH_lWafDieHeight"),			&m_lWafDieHeight);			//v3.94T4
		RegVariable(_T("BH_lEjCTLevelOffset"),		&m_lEjCTLevelOffset);		//v3.98T1
		RegVariable(_T("BH_bRefreshScreen"),		&m_bRefreshScreen);			//v3.94T4

		//Open DAC Calibration
		RegVariable(_T("BH_bIsOpenDACCalibStart"),	&m_bIsOpenDACCalibStart);
		RegVariable(_T("BH_bIsOpenDACForceCheckStart"),	&m_bIsOpenDACForceCheckStart);
		RegVariable(_T("BH_lOpenDACForceCheckValue"),	&m_lOpenDACForceCheckValue);

		RegVariable(_T("BH_lWithBHDown"), &m_lWithBHDown);
		// Auto Clean Collet
		RegVariable(_T("BH_bCleanColletPocketOn"),	&CMS896AStn::m_bCleanColletPocketOn);

		//Sync Move Via Ej Cmd Dist
		RegVariable(_T("BH_bEnableSyncMotionViaEjCmd"),	&m_bEnableSyncMotionViaEjCmd);
		RegVariable(_T("BH_lSyncTriggerValue"),			&m_lSyncTriggerValue);
		RegVariable(_T("BH_lSyncZOffset"),				&m_lSyncZOffset);
		RegVariable(_T("BH_lSoftTouchPickDistance"),	&m_lSoftTouchPickDistance);
		RegVariable(_T("BH_lSoftTouchBondDistance"),	&m_lSoftTouchBondDistance);
		RegVariable(_T("BH_lSoftTouchVelocity"),		&m_lSoftTouchVelocity);
		
		RegVariable(_T("BH_bILCAutoLearn"),				&m_bILCAutoLearn);
		RegVariable(_T("BH_lRestartUpdateMinTime"),		&m_lRestartUpdateMinTime);
		RegVariable(_T("BH_lAutoLearnLevelsIdleTime"),	&m_lAutoLearnLevelsIdleTime);
		//v4.43T9
		RegVariable(_T("BH_bMS100DigitalAirFlowSnr"),		&m_bMS100DigitalAirFlowSnr);	
		RegVariable(_T("BH_bEnableDAFlowRunTimeCheck"),		&m_bEnableDAFlowRunTimeCheck);		//v4.53A1
		RegVariable(_T("BH_ulBHZ1DAirFlowSetZeroOffset"),	&m_ulBHZ1DAirFlowSetZeroOffset);
		RegVariable(_T("BH_ulBHZ2DAirFlowSetZeroOffset"),	&m_ulBHZ2DAirFlowSetZeroOffset);
		RegVariable(_T("BH_ulBHZ1DAirFlowBlockValue"),		&m_ulBHZ1DAirFlowBlockValue);
		RegVariable(_T("BH_ulBHZ1DAirFlowUnBlockValue"),	&m_ulBHZ1DAirFlowUnBlockValue);
		RegVariable(_T("BH_ulBHZ1DAirFlowThreshold"),		&m_ulBHZ1DAirFlowThreshold);
		RegVariable(_T("BH_ulBHZ1DAirFlowThresholdCJ"),		&m_ulBHZ1DAirFlowThresholdCJ);
		RegVariable(_T("BH_ulBHZ2DAirFlowBlockValue"),		&m_ulBHZ2DAirFlowBlockValue);
		RegVariable(_T("BH_ulBHZ2DAirFlowUnBlockValue"),	&m_ulBHZ2DAirFlowUnBlockValue);
		RegVariable(_T("BH_ulBHZ2DAirFlowThreshold"),		&m_ulBHZ2DAirFlowThreshold);
		RegVariable(_T("BH_ulBHZ2DAirFlowThresholdCJ"),		&m_ulBHZ2DAirFlowThresholdCJ);
		RegVariable(_T("BH_ulBHZ1DAirFlowValue"),			&m_ulBHZ1DAirFlowValue);
		RegVariable(_T("BH_ulBHZ2DAirFlowValue"),			&m_ulBHZ2DAirFlowValue);
		//v4.50A2
		RegVariable(_T("BH_dBHZ1ThresholdPercent"),			&m_dBHZ1ThresholdPercent);
		RegVariable(_T("BH_dBHZ2ThresholdPercent"),			&m_dBHZ2ThresholdPercent);
		RegVariable(_T("BH_lBHZ1ThresholdLimit"),			&m_lBHZ1ThresholdLimit);
		RegVariable(_T("BH_lBHZ2ThresholdLimit"),			&m_lBHZ2ThresholdLimit);

		RegVariable(_T("BH_ulBHZ1DAirFlowCleanColletThreshold"),	&m_ulBHZ1DAirFlowCleanColletThreshold);
		RegVariable(_T("BH_ulBHZ2DAirFlowCleanColletThreshold"),	&m_ulBHZ2DAirFlowCleanColletThreshold);
		RegVariable(_T("BH_lBHZ1ThresholdUpperLimit"),				&m_lBHZ1ThresholdUpperLimit);

		RegVariable(_T("BH_ulBHTThermostatADC"),			&m_ulBHTThermostatADC);			//MS60	//v4.49A5
		RegVariable(_T("BH_ulBHTThermostatReading"),		&m_ulBHTThermostatReading);		//MS60	//v4.49A5

		RegVariable(_T("BH_dBHZ1UpLookCalibFactor"),		&m_dBHZ1UplookCalibFactor);
		RegVariable(_T("BH_dBHZ2UpLookCalibFactor"),		&m_dBHZ2UplookCalibFactor);

		RegVariable(_T("BH_lBHZ1HomeOffset"),				&m_lBHZ1HomeOffset);
		RegVariable(_T("BH_lBHZ2HomeOffset"),				&m_lBHZ2HomeOffset);
		RegVariable(_T("BH_lBHZOffsetByPBEmptyCheck"),		&m_lBHZOffsetbyPBEmptyCheck);	//v4.48A8 //3E DL & WH SanAn
		RegVariable(_T("BH_lBHZAutoLearnDriveInLimit"),		&m_lBHZAutoLearnDriveInLimit);

		RegVariable(_T("BH_ulStartLotCheckCount"),		&m_ulStartLotCheckCount);
		//New DatalogList
		RegVariable(_T("BH_bDataLogListInNewMode"),			&m_bDataLogListInNewMode);
		RegVariable(_T("BH_bMS60HeatingCoilFcn"),			&m_bMS60ThermalCtrl);
		RegVariable(_T("BH_dIS"),							&m_dIS);
		RegVariable(_T("BH_dMS60Thermal_P"),				&m_dMS60Thermal_P);
		RegVariable(_T("BH_dMS60Thermal_I"),				&m_dMS60Thermal_I);
		RegVariable(_T("BH_dMS60Thermal_D"),				&m_dMS60Thermal_D);

		//Alarm Page
		RegVariable(_T("BH_bShowAlarmPage"),				&m_bShowAlarmPage);
		RegVariable(_T("BH_bILCSlowProfile"),				&m_bILCSlowProfile);

		RegVariable(_T("PBT_bUseMultiProbeLevel"),		&m_bUseMultiProbeLevel);

		RegVariable(_T("BH_lAutoUpdateBHPickPosnMode"),			&m_lAutoUpdateBHPickPosnMode);
		RegVariable(_T("BH_bEnableBHMark"),						&m_bEnableBHMark_HMI);
		RegVariable(_T("BH_bEnableAutoUpdateBHPickPosnMode"),	&m_bEnableAutoUpdateBHPickPosnMode_HMI);
		RegVariable(_T("BH_lCheckBHMarkCycle"),					&m_lCheckBHMarkCycle);
		RegVariable(_T("BH_lCheckBHMarkDelay"),					&m_lCheckBHMarkDelay);

		DisplayMessage("BondHeadStn variables registered ...");

		// Register command
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_UpdateAction"),			&CBondHead::BH_UpdateAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),				&CBondHead::UpdateAction);		//v4.53A23	//Osram Germany UI bug
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogItems"),					&CBondHead::LogItems);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleLearnPadsLevel"),		&CBondHead::ToggleLearnPadsLevel);		//v4.53A23	//Osram Germany UI bug


		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeArmPosition"),			&CBondHead::BH_ChangeArmPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_KeyInArmPosition"),			&CBondHead::BH_KeyInArmPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MovePosArmPosition"),		&CBondHead::BH_MovePosArmPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNegArmPosition"),		&CBondHead::BH_MoveNegArmPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmBondArmSetup"),		&CBondHead::BH_ConfirmBondArmSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CancelBondArmSetup"),		&CBondHead::BH_CancelBondArmSetup);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_EjtMoveTo"),					&CBondHead::BH_EjtMoveTo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoLearnHeadPosn"),			&CBondHead::BH_AutoLearnHeadPosn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeHeadPosition"),		&CBondHead::BH_ChangeHeadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_KeyInHeadPosition"),			&CBondHead::BH_KeyInHeadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MovePosHeadPosition"),		&CBondHead::BH_MovePosHeadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNegHeadPosition"),		&CBondHead::BH_MoveNegHeadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmBondHeadSetup"),		&CBondHead::BH_ConfirmBondHeadSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CancelBondHeadSetup"),		&CBondHead::BH_CancelBondHeadSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_UpdateSwingOffset"),			&CBondHead::BH_UpdateSwingOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoLearnBHZ1BondLevels"),	&CBondHead::BH_AutoLearnBHZ1BondLevels);		//v3.94T4
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoLearnBHZ1BondLevelsBT2"), &CBondHead::BH_AutoLearnBHZ1BondLevels_BT2);	//v4.34T1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoLearnLevels"),			&CBondHead::BH_AutoLearnLevels);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_UpdateRegistry"),			&CBondHead::BH_UpdateRegistry);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_TnZToPick1"),				&CBondHead::BH_TnZToPick1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_TnZToPick2"),				&CBondHead::BH_TnZToPick2);

		//Z2 Setting
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoLearnHeadPosnZ2"),		&CBondHead::BH_AutoLearnHeadPosnZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeHeadPositionZ2"),		&CBondHead::BH_ChangeHeadPositionZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_KeyInHeadPositionZ2"),		&CBondHead::BH_KeyInHeadPositionZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MovePosHeadPositionZ2"),		&CBondHead::BH_MovePosHeadPositionZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNegHeadPositionZ2"),		&CBondHead::BH_MoveNegHeadPositionZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmBondHeadSetupZ2"),	&CBondHead::BH_ConfirmBondHeadSetupZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CancelBondHeadSetupZ2"),		&CBondHead::BH_CancelBondHeadSetupZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_UpdateSwingOffsetZ2"),		&CBondHead::BH_UpdateSwingOffsetZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoLearnBHZ2BondLevels"),	&CBondHead::BH_AutoLearnBHZ2BondLevels);		//v3.94T4
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoLearnBHZ2BondLevelsBT2"), &CBondHead::BH_AutoLearnBHZ2BondLevels_BT2);	//v4.34T1

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLearnEjUpLevel"),			&CBondHead::BH_AutoLearnEjUpLevel);				//v4.48A3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoLearnEjectorPosn"),		&CBondHead::BH_AutoLearnEjectorPosn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeEjectorPosition"),		&CBondHead::BH_ChangeEjectorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeNVCEjectorPosition"),	&CBondHead::BH_ChangeNVCEjectorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeNVCBinEjectorPosition"), &CBondHead::BH_ChangeNVCBinEjectorPosition);
			
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeEjtCollet1"),			&CBondHead::BH_ChangeEjtCollet1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeEjtCollet2"),			&CBondHead::BH_ChangeEjtCollet2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Ejt_Z_Move_Up"),				&CBondHead::BH_Ejt_Z_Move_Up);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Ejt_Z_Move_Down"),			&CBondHead::BH_Ejt_Z_Move_Down);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_EjtCollet_Move_Up"),			&CBondHead::BH_EjtCollet_Move_Up);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_EjtCollet_Move_Down"),		&CBondHead::BH_EjtCollet_Move_Down);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_EjtCollet_Move_Left"),		&CBondHead::BH_EjtCollet_Move_Left);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_EjtCollet_Move_Right"),		&CBondHead::BH_EjtCollet_Move_Right);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeEjectorX"),			&CBondHead::BH_ChangeEjectorX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeEjectorY"),			&CBondHead::BH_ChangeEjectorY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_KeyInEjectorPosition"),		&CBondHead::BH_KeyInEjectorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MovePosEjectorPosition"),	&CBondHead::BH_MovePosEjectorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNegEjectorPosition"),	&CBondHead::BH_MoveNegEjectorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MovePosEjectorPositionY"),	&CBondHead::BH_MovePosEjectorPositionY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNegEjectorPositionY"),	&CBondHead::BH_MoveNegEjectorPositionY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmEjectorSetup"),		&CBondHead::BH_ConfirmEjectorSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CancelEjectorSetup"),		&CBondHead::BH_CancelEjectorSetup);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmNVCEjectorSetup"),	&CBondHead::BH_ConfirmNVCEjectorSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CancelNVCEjectorSetup"),		&CBondHead::BH_CancelNVCEjectorSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNVCEjectorPosition"),	&CBondHead::BH_MoveNVCEjectorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_KeyInNVCEjectorPosition"),	&CBondHead::BH_KeyInNVCEjectorPosition);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmNVCBinEjectorSetup"),	&CBondHead::BH_ConfirmNVCBinEjectorSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CancelNVCBinEjectorSetup"),	&CBondHead::BH_CancelNVCBinEjectorSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNVCBinEjectorPosition"),	&CBondHead::BH_MoveNVCBinEjectorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_KeyInNVCBinEjectorPosition"),&CBondHead::BH_KeyInNVCBinEjectorPosition);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeEjElevatorPosition"),	&CBondHead::BH_ChangeEjElevatorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_KeyInEjElevatorPosition"),	&CBondHead::BH_KeyInEjElevatorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MovePosEjElevatorPosition"), &CBondHead::BH_MovePosEjElevatorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNegEjElevatorPosition"), &CBondHead::BH_MoveNegEjElevatorPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmEjElevatorSetup"),	&CBondHead::BH_ConfirmEjElevatorSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CancelEjElevatorSetup"),		&CBondHead::BH_CancelEjElevatorSetup);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmSetup"),		&CBondHead::BH_ConfirmSetup);
		//1.08S
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmMaxColletCountSetup"),		&CBondHead::BH_ConfirmMaxColletCountSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmMaxCleanCount"),		&CBondHead::BH_ConfirmMaxCleanCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmMaxCollet2CountSetup"),		&CBondHead::BH_ConfirmMaxCollet2CountSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfirmMaxEjectorCountSetup"),		&CBondHead::BH_ConfirmMaxEjectorCountSetup);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ColletVacuum"),		&CBondHead::BH_ColletVacuum);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ColletVacuumZ2"),	&CBondHead::BH_ColletVacuumZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_StrongBlow"),		&CBondHead::BH_StrongBlow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_StrongBlowZ2"),		&CBondHead::BH_StrongBlowZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_HomeEjector"),		&CBondHead::BH_HomeEjector);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_HomeEjElevator"),	&CBondHead::BH_HomeEjElevator);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_HomeBondHead"),		&CBondHead::BH_HomeBondHead);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_HomeBondArm"),		&CBondHead::BH_HomeBondArm);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetAll"),			&CBondHead::BH_ResetAll);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveToBlow"),		&CBondHead::BH_MoveToBlow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveToPrePick"),		&CBondHead::BH_MoveToPrePick);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveToPick"),		&CBondHead::BH_MoveToPick);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveToPickLevel"),	&CBondHead::BH_MoveToPickLevel);	//v3.81
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveEjToUpLevel"),	&CBondHead::BH_MoveEjToUpLevel);	//v4.08
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveBHToPick1"),		&CBondHead::BH_MoveBHToPick1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveBHToPick"),		&CBondHead::BH_MoveBHToPick);		//v3.34
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveToBond"),		&CBondHead::BH_MoveToBond);			//v2.60
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveBHToPick_Z2"),	&CBondHead::BH_MoveBHToPick_Z2);	//Dual Arm learn collet
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveToBond_Z2"),		&CBondHead::BH_MoveToBond_Z2);		//Dual Arm learn collet
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoMoveToPrePick"),	&CBondHead::BH_AutoMoveToPrePick);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_EnableThermalControl"),	&CBondHead::BH_EnableThermalControlCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_UpdateThermalTemperature"),	&CBondHead::BH_UpdateThermalTemperature);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetCommandPosition"),	&CBondHead::GetCommandPosition);

		//HMI user routine
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetChannelTravelLimit"),	&CBondHead::GetChannelTravelLimit);

		// Reset the Collet and Ejector Count
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetColletCountDiag"),	&CBondHead::BH_ResetColletCountDiag);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetCollet1CountDiag"),	&CBondHead::BH_ResetCollet1CountDiag);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetCollet2CountDiag"),	&CBondHead::BH_ResetCollet2CountDiag);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetColletCount"),		&CBondHead::BH_ResetColletCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetCollet2Count"),		&CBondHead::BH_ResetCollet2Count);		//v3.92	//MS100
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetEjectorCountDiag"),	&CBondHead::BH_ResetEjectorCountDiag);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetEjectorCount"),		&CBondHead::BH_ResetEjectorCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetAllDieCount"),		&CBondHead::BH_ResetAllDieCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetAllWaferDieCount"), &CBondHead::BH_ResetAllWaferDieCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetCustomCounter"),	&CBondHead::BH_ResetCustomCounter);		//v3.68T3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetEjKOffsetCount"),	&CBondHead::BH_ResetEjKOffsetCount);	//v4.46T15
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetBHZKOffsetCount"),	&CBondHead::BH_ResetBHZKOffsetCount);	//v4.53A25

		// Display wafer information
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ShowWaferInformation"),	&CBondHead::BH_ShowWaferInformation);

		// Update current wafer yield
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_UpdateCurrentWaferYield"), &CBondHead::BH_UpdateCurrentWaferYield);

		// Clean Collet
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CleanCollet"), &CBondHead::BH_CleanCollet);

		// Home module motor
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_HomeMotorSelection"), &CBondHead::BH_HomeMotorSelection);

		// Check bondarm in safe pos
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_IsBondArmSafe"), &CBondHead::BH_IsBondArmSafe);

		// On/Off Front & side Cover lock
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_FrontCoverLock"),		&CBondHead::BH_FrontCoverLock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_SideCoverLock"),			&CBondHead::BH_SideCoverLock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_BinElvatorCoverLock"),	&CBondHead::BH_BinElvatorCoverLock);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrontCoverLock"),		&CBondHead::FrontCoverLock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SideCoverLock"),		&CBondHead::SideCoverLock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BinElevatorCoverLock"), &CBondHead::BinElevatorCoverLock);
		//v4.24T10	//ES101 SIO fcns
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("NGPickUpDn"),			&CBondHead::NGPickUpDn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("NGPickReelStop"),		&CBondHead::NGPickReelStop);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("NGPickReloadReel"),		&CBondHead::NGPickReloadReel);		//v4.27
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("NGPick_IndexReelT"),	&CBondHead::NGPick_IndexReelT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("NGPick_HomeReelT"),		&CBondHead::NGPick_HomeReelT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartEjElevator"),		&CBondHead::StartEjElevator);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveEjtElvtToUpDown"),	&CBondHead::MoveEjtElvtToUpDown);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveEjeElvtToSafeLevel"),		&CBondHead::MoveEjeElvtToSafeLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveEjectorElevatorRoutine"),	&CBondHead::MoveEjectorElevatorRoutine);
		//v4.42T3	//EjtXY table fcns
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetEjtXYEncoderCmd"),		&CBondHead::GetEjtXYEncoderCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EjtXY_MoveToCmd"),			&CBondHead::EjtXY_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EjtXY_MoveToColletOffset"),	&CBondHead::EjtXY_MoveToColletOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EjtXY_Setup"),				&CBondHead::EjtXY_Setup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EjtXY_SearchBHColletHoles"),&CBondHead::EjtXY_SearchBHColletHoles);

		// Auto Clean Collet Pocket Fcn
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BHSyringeOnce"),		&CBondHead::BHSyringeOnce);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetColletCleanPocket"),	&CBondHead::SetColletCleanPocket);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BHAccTest"),			&CBondHead::BHAccTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ACCMoveToPosnT"),	&CBondHead::BH_ACCMoveToPosnT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_UpdateACCData"),		&CBondHead::BH_UpdateACCData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_BlockButtonCmd"),	&CBondHead::BH_BlockButtonCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_SetSuckingHead"),	&CBondHead::BH_SetSuckingHead);		//v4.05

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_SyncArmColletVariable"),	&CBondHead::BH_SyncArmColletVariable);

		//--- Diagnostic Functions ---
		/* PowerOn Functions 
				Parameter: BOOL bOn */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_T"),	&CBondHead::Diag_PowerOn_T);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_Z"),	&CBondHead::Diag_PowerOn_Z);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_Z2"),	&CBondHead::Diag_PowerOn_Z2);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_Ej"),	&CBondHead::Diag_PowerOn_Ej);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_EjCap"),&CBondHead::Diag_PowerOn_EjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_EjT"),	&CBondHead::Diag_PowerOn_EjT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_EjX"),	&CBondHead::Diag_PowerOn_EjX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_EjY"),	&CBondHead::Diag_PowerOn_EjY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_EjElevator"), &CBondHead::Diag_PowerOn_EjElevator);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_BinEj"),	&CBondHead::Diag_PowerOn_BinEj);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_BinEjCap"),&CBondHead::Diag_PowerOn_BinEjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_BinEjT"),	&CBondHead::Diag_PowerOn_BinEjT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_BinEjElevator"), &CBondHead::Diag_PowerOn_BinEjElevator);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Diag_PowerOn_ChgColletT"), &CBondHead::Diag_PowerOn_ChgColletT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Diag_PowerOn_ChgColletZ"), &CBondHead::Diag_PowerOn_ChgColletZ);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_All"),	&CBondHead::Diag_PowerOn_All);
		/* Commutate Functions 
				Parameter: None */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_T"),		&CBondHead::Diag_Comm_T);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_Z"),		&CBondHead::Diag_Comm_Z);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_Z2"),		&CBondHead::Diag_Comm_Z2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_Ej"),		&CBondHead::Diag_Comm_Ej);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_EjCap"),	&CBondHead::Diag_Comm_EjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_EjT"),		&CBondHead::Diag_Comm_EjT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_BinEj"),	&CBondHead::Diag_Comm_BinEj);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_BinEjCap"),&CBondHead::Diag_Comm_BinEjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_BinEjT"),	&CBondHead::Diag_Comm_BinEjT);

		/* Home Functions 
				Parameter: None */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_T"),		&CBondHead::Diag_Home_T);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_Z"),		&CBondHead::Diag_Home_Z);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_Z2"),		&CBondHead::Diag_Home_Z2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_Ej"),		&CBondHead::Diag_Home_Ej);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_EjT"),		&CBondHead::Diag_Home_EjT);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_EjCap"),	&CBondHead::Diag_Home_EjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_EjX"),		&CBondHead::Diag_Home_EjX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_EjY"),		&CBondHead::Diag_Home_EjY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_EjElevator"),	&CBondHead::Diag_Home_EjElevator);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_BinEj"),		&CBondHead::Diag_Home_BinEj);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_BinEjT"),		&CBondHead::Diag_Home_BinEjT);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_BinEjCap"),	&CBondHead::Diag_Home_BinEjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_BinEjElevator"),&CBondHead::Diag_Home_BinEjElevator);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Diag_Home_MS50ChgColletT"),	&CBondHead::Diag_Home_MS50ChgColletT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Diag_Home_MS50ChgColletZ"),	&CBondHead::Diag_Home_MS50ChgColletZ);
		/* Move Functions (Relative)
				Parameter: LONG lPos (Motor Counts) */
		//m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_T"),		&CBondHead::Diag_Move_T);
		//m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_Z"),		&CBondHead::Diag_Move_Z);
		//m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_Z2"),		&CBondHead::Diag_Move_Z2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_Ej"),		&CBondHead::Diag_Move_Ej);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_NegMove_Ej"),	&CBondHead::Diag_NegMove_Ej);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_EjT"),		&CBondHead::Diag_Move_EjT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_NegMove_EjT"),	&CBondHead::Diag_NegMove_EjT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_EjCap"),	&CBondHead::Diag_Move_EjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_NegMove_EjCap"),	&CBondHead::Diag_NegMove_EjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_EjX"),		&CBondHead::Diag_Move_EjX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_EjY"),		&CBondHead::Diag_Move_EjY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_EjElevator"),	&CBondHead::Diag_Move_EjElevator);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_NegMove_EjElevator"),	&CBondHead::Diag_NegMove_EjElevator);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_BinEj"),			&CBondHead::Diag_Move_BinEj);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_NegMove_BinEj"),		&CBondHead::Diag_NegMove_BinEj);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_BinEjT"),			&CBondHead::Diag_Move_BinEjT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_NegMove_BinEjT"),		&CBondHead::Diag_NegMove_BinEjT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_BinEjCap"),		&CBondHead::Diag_Move_BinEjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_NegMove_BinEjCap"),		&CBondHead::Diag_NegMove_BinEjCap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_BinEjElevator"),	&CBondHead::Diag_Move_BinEjElevator);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_NegMove_BinEjElevator"),	&CBondHead::Diag_NegMove_BinEjElevator);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Diag_Move_MS50ChgColletT"),		&CBondHead::Diag_Move_MS50ChgColletT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Diag_Move_MS50ChgColletZ"),		&CBondHead::Diag_Move_MS50ChgColletZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveEjectorStandbyPosition"), &CBondHead::BH_MoveEjectorStandbyPosition);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateOutput"),				&CBondHead::UpdateOutput);
		
		/* OPEN DAC Test */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CheckPreBondStatus"),	&CBondHead::CheckPreBondStatus);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CheckPreBondChecking"),	&CBondHead::CheckPreBondChecking);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_StartTDacTest"),			&CBondHead::StartTDacTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_StopTDacTest"),			&CBondHead::StopTDacTest);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_StartAirFlowSensorTest"), &CBondHead::StartAirFlowSensorTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateDieCounterCmd"),		&CBondHead::UpdateDieCounterCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckStartThermalTest"),	&CBondHead::CheckStartThermalTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StaticMotionLog"),			&CBondHead::StaticMotionLog);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckExArmInSafePos"),		&CBondHead::CheckExArmInSafePos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenerateParameterList"),	&CBondHead::GenerateParameterList);		//v3.86
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenerateConfigData"),		&CBondHead::GenerateConfigData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GeneratePkgDataFile"),		&CBondHead::GeneratePkgDataFile);		//v4.21T7	//Walsin China

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleILCLog"),				&CBondHead::ToggleILCLog);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OpenDACCalibrationCmd"),	&CBondHead::OpenDACCalibrationCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OpenDACForceCheckCmd"),		&CBondHead::OpenDACForceCheckCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetOpenDACCalibRatio"),	&CBondHead::ResetOpenDACCalibRatio);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DACDistanceCalibrationCmd"),	&CBondHead::DACDistanceCalibrationCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetDACDistanceCalibRatio"),	&CBondHead::ResetDACDistanceCalibRatio);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckMDResponseTime"),		&CBondHead::CheckMDResponseTime);		//v3.94
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLearnBT2OffsetZ"),		&CBondHead::BH_AutoLearnBT2OffsetZ);	//v4.21
	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartBHZ1MotionTest"),		&CBondHead::StartBHZ1MotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartBHZ2MotionTest"),		&CBondHead::StartBHZ2MotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartBArmMotionTest"),		&CBondHead::StartBArmMotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartEjMotionTest"),		&CBondHead::StartEjMotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartPBTZMotionTest"),		&CBondHead::StartPBTZMotionTest);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ILC_AutoLearnCmd"),			&CBondHead::ILC_AutoLearnCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ILC_RestartUpdateCmd"),		&CBondHead::ILC_RestartUpdateCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ILC_BondArmPreStartCheck"),	&CBondHead::ILC_BondArmPreStartCheck);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReadDigitalAirFlowSensor"),	&CBondHead::ReadDigitalAirFlowSensor);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckTheEjectorLifeTime"),	&CBondHead::CheckTheEjectorLifeTime);			//v4.44T4 Sanan
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLearnBHZHomeOffset"),	&CBondHead::AutoLearnBHZHomeOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBondHeadFan"),			&CBondHead::SetBondHeadFanCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckCoverAlarm"),			&CBondHead::CheckCoverAlarm);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EjSoftTouch"),				&CBondHead::EjSoftTouch);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HC_UpdateISValue"),			&CBondHead::HC_UpdateISValue);			//v4.50A6
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HC_UpdatePIDValue"),		&CBondHead::HC_UpdatePIDValue);			//v4.50A6
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualPickDieToUpLookPR"),	&CBondHead::ManualPickDieToUpLookPR);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_BondLeftDie"),			&CBondHead::BH_BondLeftDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoopTestBondArm"),			&CBondHead::LoopTestBondArm);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchColletHoleTest"),		&CBondHead::SearchColletHoleTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchBondColletEpoxyCmd"),	&CBondHead::SearchBondColletEpoxyCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CleanColletCmd"),			&CBondHead::CleanColletCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetDefaultKValue"),					&CBondHead::SetDefaultKValue);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveContourProfileLearnCollet"),	&CBondHead::MoveContourProfileLearnCollet);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MHCTesting"),						&CBondHead::MHCTesting);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EjElevatorZGoToDownPosition"),		&CBondHead::EjElevatorZGoToDownPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EjElevatorZGoToUpPosition"),		&CBondHead::EjElevatorZGoToUpPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BinEjElevatorZGoToDownPosition"),	&CBondHead::BinEjElevatorZGoToDownPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BinEjElevatorZGoToUpPosition"),		&CBondHead::BinEjElevatorZGoToUpPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("NVCSingleBondDie"),					&CBondHead::NVCSingleBondDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EjectorThetaShakeVisionTest"),		&CBondHead::EjectorThetaShakeVisionTest);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBTLevelSampleArea"),			&CBondHead::SetBTLevelSampleArea);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MBL_GetBondLevel"),			&CBondHead::BH_MBL_GetBondLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_DetectBinTableTilting"),		&CBondHead::BH_DetectBinTableTilting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBondPadsArea"),				&CBondHead::SetBondPadsArea);

		RegisterAGCCommand();		//v4.50A6	
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Test"),						&CBondHead::Test);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}

LONG CBondHead::BH_ChangeArmPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	INT nTStatus = gnOK;
	IPC_CServiceMessage rReqMsg;
	IPC_CServiceMessage stMsg;

	typedef struct 
	{
		LONG	lPosition;
		BOOL	bFromZero;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//v3.93
	Z_MoveToHome();
	if (m_bIsArm2Exist == TRUE)
	{
		Z2_MoveToHome();
	}

	//NuMotionDataLogForBHDebug(TRUE);
	//m_bIsDataLogForBHDebug = TRUE;

	if (!IsBondArmAbleToMove())
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}
	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lPrePickPos_T);		//v4.53A14

	//If learn from Home Position
	if (stInfo.bFromZero == TRUE)
	{
		m_lBPGeneral_TmpA = 0;

		T_MoveTo(0);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_2 = 0;	
				break;	//Pick
			case 3:		
				m_lBPGeneral_4 = 0;	
				break;	//Bond
			case 4:		
				m_lBPGeneral_5 = 0;	
				break;	//Clean Collet
			case 5:		
				m_lBPAccPosT	= 0;	
				break;	//Auto Clean Collet
			default:	
				m_lBPGeneral_1 = 0;	
				break;	//PrePick
		}
	}

	INT nConvID = 0;
	switch (stInfo.lPosition)
	{
		case 1:		//Pick
			if (stInfo.bFromZero == FALSE)
			{
				nTStatus = T_SMoveTo(m_lPickPos_T);
				if (nTStatus == gnOK)
				{
					Z_MoveTo(m_lPickLevel_Z);
				}
				m_lBPGeneral_TmpA = m_lPickPos_T;
			}
			break;

		case 3:		//Bond
			if (stInfo.bFromZero == FALSE)
			{
				nTStatus = T_SMoveTo(m_lBondPos_T);
				if (nTStatus == gnOK)
				{
					Z_MoveTo(m_lBondLevel_Z);
				}
				m_lBPGeneral_TmpA = m_lBondPos_T;
			}

			//v4.59A7
			if (IsMS90())
			{
				if (HmiMessage("Move BinTable to Clean-Collet position?", "BondArm Setup", glHMI_MBX_YESNO) == glHMI_YES)
				{
					BOOL bToGarbageBin = TRUE;
					BOOL bReturn = TRUE;
					stMsg.InitMessage(sizeof(BOOL), &bToGarbageBin);
					INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("MoveBinTableToMS90GBinPosn"), stMsg);
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(BOOL), &bReturn);
							break;
						}
						else
						{
							Sleep(10);
						}
					}

					if (!bReturn)
					{
						HmiMessage_Red_Yellow("Manual-Clean-Collet moves BT fails 3. (MS90)");
						SetErrorMessage("Manual-Clean-Collet moves BT fails 3. (MS90)");
					}
				}
			}

			break;

		case 4:		//collet
			if (stInfo.bFromZero == FALSE)
			{
				nTStatus = T_SMoveTo(m_lCleanColletPos_T);
				m_lBPGeneral_TmpA = m_lCleanColletPos_T;
			}
			break;

		case 5:	// Auto Clean Collet
#ifndef NU_MOTION
			if (stInfo.bFromZero == FALSE)
			{
				T_SMoveTo(m_lAutoCleanCollet_T, SFM_WAIT);
				Sleep(50);
			}

			// let bin table move to clean position
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("BTMoveToACCBrush"), stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
#endif
			break;

		default:		//PrePick
			if (stInfo.bFromZero == FALSE)
			{
				if (CMS896AStn::m_bDBHHeatingCoilFcn)
				{	
#ifdef NU_MOTION_MS60		//v4.50A16	//compatible with MS100Plus2/3
					CString szMsg;

					LONG lPrePickPos_T = (m_lPickPos_T + m_lBondPos_T)/2;

					//szMsg.Format("Pick Position:%d, Bond Position:%d, Prepick:%d", m_lPickPos_T,m_lBondPos_T,m_lPrePickPos_T);
					//AfxMessageBox(szMsg);
					nTStatus = T_SMoveTo(lPrePickPos_T);
					//szMsg.Format("Calibrate PREPICK position from centre point T = %ld ...", lPrePickPos_T);
					//HmiMessage(szMsg);

					//int nCompansation = CMS896AStn::CalibratePrepickPositionForPreHeat(BH_AXIS_T, &m_stBHAxis_T);

					//if (nCompansation < 130209)
					//	lPrePickPos_T = lPrePickPos_T - nCompansation + 52083;
					//else
					//	lPrePickPos_T = lPrePickPos_T - nCompansation + 52083 + 156250;

					//szMsg.Format("PREPICK position calibrated to T = %ld", lPrePickPos_T);
					//HmiMessage(szMsg);

					m_lPrePickPos_T = lPrePickPos_T;
					nTStatus = T_SMoveTo(lPrePickPos_T);
					m_lBPGeneral_TmpA = lPrePickPos_T;
					m_lBPGeneral_1	  = lPrePickPos_T;
#endif
				}
				else
				{
					nTStatus = T_SMoveTo(m_lPrePickPos_T);
					m_lBPGeneral_TmpA = m_lPrePickPos_T;
				}
				//CMS896AStn::MotionSetMotorThermalCompDAC(
			}
			break;
	}

	if (nTStatus != gnOK)
	{
		bResult = FALSE;
	}

#ifdef NU_MOTION
	if (m_bIsDataLogForBHDebug == TRUE)
	{
		NuMotionDataLogForBHDebug(FALSE);
		m_bIsDataLogForBHDebug = FALSE;
	}
#endif

	T_Profile(NORMAL_PROF);				
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_KeyInArmPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxDistance = 0;
	LONG lDistA = 0;
	LONG lDistB = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}


	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF

	lDistA = abs(GetChannelInformation(MS896A_CFG_CH_BONDARM, MS896A_CFG_CH_MAX_DISTANCE));								
	lDistB = abs(GetChannelInformation(MS896A_CFG_CH_BONDARM, MS896A_CFG_CH_MIN_DISTANCE));

#ifdef NU_MOTION
	lMaxDistance = max(lDistA, lDistB) + 10000;		//v3.86T5	//Add extra 20000 steps for MS1000 BH1 & BH2
#else
	lMaxDistance = max(lDistA, lDistB);
#endif

	LONG lHalfDistT = m_lPickPos_T - (m_lPickPos_T - m_lBondPos_T) / 2;

	switch (stInfo.lPosition)
	{
		case 1:		//Pick
			if ((abs(stInfo.lStep) + abs(m_lBondPos_T)) > lMaxDistance)
			{
				break;
			}
			else
			{
				m_lBPGeneral_2 = stInfo.lStep;
				T_SMoveTo(m_lBPGeneral_2);
			}
			break;

		case 3:		//Bond
			if ((abs(stInfo.lStep) + abs(m_lPickPos_T)) > lMaxDistance)
			{
				break;
			}
			else
			{
				m_lBPGeneral_4 = stInfo.lStep;
				T_SMoveTo(m_lBPGeneral_4);
			}
			break;

		case 4:		//collet
			m_lBPGeneral_5 = stInfo.lStep;
			T_SMoveTo(m_lBPGeneral_5);
			break;
		case 5:	// Auto Clean Collet
			m_lBPAccPosT = stInfo.lStep;
			T_SMoveTo(m_lBPAccPosT);
			break;

		default:	//PrePick
			//if ((abs(stInfo.lStep) + abs(m_lBondPos_T)) > lMaxDistance)
			if ((stInfo.lStep < lHalfDistT)	||	//Pre-PICK pos cannot be less than half-dist between PICK & BOND	//v2.97T4
					(stInfo.lStep > (m_lPickPos_T - 1000)))
			{
				break;
			}
			else
			{
				m_lBPGeneral_1 = stInfo.lStep;
				T_SMoveTo(m_lBPGeneral_1);
			}
			break;
	}

	T_Profile(NORMAL_PROF);				

	//v4.53A14
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_MovePosArmPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF

	//Get limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BONDARM, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BONDARM, MS896A_CFG_CH_MAX_DISTANCE);								
	CString szTemp;

#ifdef NU_MOTION
	lMinPos = lMinPos - 10000;
	lMaxPos = lMaxPos + 10000;
#endif

	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_2 + stInfo.lStep;	
			if ((abs(lCurrentPos) + abs(m_lBondPos_T)) > lMaxPos)
			{
				lCurrentPos = m_lBPGeneral_2;	//lMaxPos + 1;
			}
			break;	//Pick

		case 3:		
			lCurrentPos = m_lBPGeneral_4 + stInfo.lStep;	
			if ((abs(lCurrentPos) + abs(m_lPickPos_T)) > lMaxPos)//< lMinPos)
			{
				lCurrentPos = m_lBPGeneral_4;	//lMaxPos + 1;
			}
			break;	//Bond

		case 4:		
			lCurrentPos = m_lBPGeneral_5 + stInfo.lStep;	
			break;	//collet
		case 5:
			lCurrentPos = m_lBPAccPosT + stInfo.lStep;
			break;

		default:	
			lCurrentPos = m_lBPGeneral_1 + stInfo.lStep;
			if (lCurrentPos > (m_lPickPos_T - 1000))	//v2.97T4
			{
				lCurrentPos = m_lPickPos_T - 1000;
			}
			/*
			if ((abs(lCurrentPos) + abs(m_lBondPos_T)) > lMaxPos)
			{
				lCurrentPos = lMaxPos + 1;
			}
			*/
			break;	//PrePick
	}


	//Check limit range
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		T_SMoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_2 = lCurrentPos;	
				break;
			case 3:		
				m_lBPGeneral_4 = lCurrentPos;	
				break;
			case 4:		
				m_lBPGeneral_5 = lCurrentPos;	
				break;
			case 5:		
				m_lBPAccPosT = lCurrentPos;	
				break;
			default:	
				m_lBPGeneral_1 = lCurrentPos;	
				break;
		}
	}

	T_Profile(NORMAL_PROF);	

	//v4.53A14
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_MoveNegArmPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}


	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF

	//Get Limit range	
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BONDARM, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BONDARM, MS896A_CFG_CH_MAX_DISTANCE);								

	LONG lHalfDistT = m_lPickPos_T - (m_lPickPos_T - m_lBondPos_T) / 2;
	CString szTemp;

#ifdef NU_MOTION
	lMinPos = lMinPos - 10000;
	lMaxPos = lMaxPos + 10000;
#endif

	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_2 - stInfo.lStep;	
			if ((abs(lCurrentPos) + abs(m_lBondPos_T)) > lMaxPos)
			{
				lCurrentPos = m_lBPGeneral_2;	//lMaxPos + 1;
			}
			break;	//Pick

		case 3:		
			lCurrentPos = m_lBPGeneral_4 - stInfo.lStep;	
			if ((abs(lCurrentPos) + abs(m_lPickPos_T)) > lMaxPos)	//< lMinPos)
			{
				lCurrentPos = m_lBPGeneral_4;	//lMaxPos + 1;
			}
			break;	//Bond

		case 4:		
			lCurrentPos = m_lBPGeneral_5 - stInfo.lStep;	
			break;	//collet
		case 5:
			lCurrentPos = m_lBPAccPosT - stInfo.lStep;
			break; // Auto Clean Collet

		default:
			lMinPos = lHalfDistT;
			lCurrentPos = m_lBPGeneral_1 - stInfo.lStep;	
			if (lCurrentPos < lMinPos)		//v2.97T4
			{
				lCurrentPos = lMinPos;
			}
/*
				if ((abs(lCurrentPos) + abs(m_lBondPos_T)) > lMaxPos)
				{
					lCurrentPos = lMaxPos + 1;
				}
*/
			break;	//PrePick
	}

	//Check limit range
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		T_SMoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_2 = lCurrentPos;	
				break;
			case 3:		
				m_lBPGeneral_4 = lCurrentPos;	
				break;
			case 4:		
				m_lBPGeneral_5 = lCurrentPos;	
				break;
			case 5:		
				m_lBPAccPosT = lCurrentPos;	
				break;
			default:	
				m_lBPGeneral_1 = lCurrentPos;	
				break;
		}
	}

	T_Profile(NORMAL_PROF);	

	//v4.53A14
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_ConfirmBondArmSetup(IPC_CServiceMessage &svMsg)
{
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		return TRUE;
	}

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	StartLoadingAlert();	//v3.86

	Z_MoveToHome();

	if (m_bIsArm2Exist == TRUE)
	{
		Z2_MoveToHome();
	}

	Sleep(500);

	//v3.96T1
	//Extra protection to protect against BH collision/safety
	INT nCount = 0;
	BOOL bHomeT = TRUE;
	while (IsCoverOpen())
	{
		CloseLoadingAlert();
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		SetErrorMessage("Machine cover open detected in CONFIRM BA setup");

		nCount++;
		if (nCount >= 3)
		{
			LONG lHmiStatus = HmiMessage("Cover sensor still open in CONFIRM BA setup; continue to HOME T motor?", 
										 "BondArm Setup", glHMI_MBX_CONTINUESTOP | 0x80000000);
			if (lHmiStatus == glHMI_CONTINUE)
			{
				bHomeT = FALSE;
				break;		//v4.52A14
			}
		}
	}

	if (bHomeT)
	{
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
	}


	CloseLoadingAlert();	//v3.86


	if (m_lPickPos_T != m_lBPGeneral_2)
	{
		m_lPickPos_T = m_lBPGeneral_2;
		LogItems(PICK_POSITION);

		ILC_ResetAutoLearnComplete();
	}
	else if (m_lBondPos_T != m_lBPGeneral_4)
	{
		m_lBondPos_T = m_lBPGeneral_4;
		LogItems(BOND_POSITION);

		ILC_ResetAutoLearnComplete();

	}
	else if (m_lPrePickPos_T != m_lBPGeneral_1)
	{
		m_lPrePickPos_T = m_lBPGeneral_1;

		ILC_ResetAutoLearnComplete();
	}

	//Update variable
	m_lPrePickPos_T			= m_lBPGeneral_1;
	m_lPickPos_T			= m_lBPGeneral_2;
	m_lBondPos_T			= m_lBPGeneral_4;	
	m_lCleanColletPos_T		= m_lBPGeneral_5;
	m_lChangeCarrierPos_T	= m_lBPGeneral_5;
	m_lAutoCleanCollet_T	= m_lBPAccPosT;

	if (CMS896AStn::m_bDBHHeatingCoilFcn)
	{	
#ifdef NU_MOTION_MS60		//v4.50A16	//compatible with MS100Plus2/3
		CString szMsg;

		LONG lPrePickPos_T = (m_lPickPos_T + m_lBondPos_T)/2;

		//szMsg.Format("Pick Position:%d, Bond Position:%d, Prepick:%d", m_lPickPos_T,m_lBondPos_T,m_lPrePickPos_T);
		//AfxMessageBox(szMsg);
		int nTStatus = T_MoveTo(lPrePickPos_T);
		szMsg.Format("Calibrate PREPICK position from centre point T = %ld ...", lPrePickPos_T);

		//HmiMessage(szMsg);
		//int nCompansation = CMS896AStn::CalibratePrepickPositionForPreHeat(BH_AXIS_T, &m_stBHAxis_T);
		//if (nCompansation < 130209)
		//	lPrePickPos_T = lPrePickPos_T - nCompansation + 52083;
		//else
		//	lPrePickPos_T = lPrePickPos_T - nCompansation + 52083 + 156250;
		//szMsg.Format("PREPICK position calibrated to T = %ld", lPrePickPos_T);
		//HmiMessage(szMsg);

		m_lPrePickPos_T = lPrePickPos_T;
		nTStatus = T_SMoveTo(lPrePickPos_T);
		m_lBPGeneral_1	  = lPrePickPos_T;
#endif
	}

	T_Profile(NORMAL_PROF);
	SaveBhData();

	SetStatusMessage("Bondarm position is updated");

	// 3461
	SetGemValue("BA_PrePickPosn", m_lPrePickPos_T);
	SetGemValue("BA_PickDiePosn", m_lPickPos_T);
	SetGemValue("BA_BondDiePosn", m_lBondPos_T);
	SetGemValue("BA_PreBondDiePosn", m_lCleanColletPos_T);
	SetGemValue("BA_CleanColletPosn", m_lCleanColletPos_T);
	// 7600
	SendEvent(SG_CEID_BA_POSN, FALSE);

	//v4.53A14
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_CancelBondArmSetup(IPC_CServiceMessage &svMsg)
{
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		return TRUE;
	}

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	StartLoadingAlert();	//v3.86

	Z_MoveToHome();
	if (m_bIsArm2Exist == TRUE)
	{
		Z2_MoveToHome();
	}
	Sleep(500);


	//v3.96T1
	//Extra protection to protect against BH collision/safety
	INT nCount = 0;
	BOOL bHomeT = TRUE;
	while (IsCoverOpen())
	{
		CloseLoadingAlert();
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		SetErrorMessage("Machine cover open detected in CANCEL BA setup");

		nCount++;
		if (nCount >= 3)
		{
			nCount = 0;
			LONG lHmiStatus = HmiMessage("Cover sensor still open in CANCEL BA setup; continue to HOME T motor?", 
										 "BondArm Setup", glHMI_MBX_CONTINUESTOP | 0x80000000);
			if (lHmiStatus == glHMI_CONTINUE)
			{
				bHomeT = FALSE;
				break;
			}
		}
	}

	if (bHomeT)
	{
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
	}


	CloseLoadingAlert();	//v3.86

	//Restore HMI variable
	m_lBPGeneral_1			= m_lPrePickPos_T;
	m_lBPGeneral_2			= m_lPickPos_T;
	m_lBPGeneral_4			= m_lBondPos_T;	
	m_lBPGeneral_5			= m_lCleanColletPos_T;
	m_lBPAccPosT			= m_lAutoCleanCollet_T;
	if (CMS896AStn::m_bDBHHeatingCoilFcn)
	{	
#ifdef NU_MOTION_MS60		//v4.50A16	//compatible with MS100Plus2/3
		CString szMsg;

		LONG lPrePickPos_T = (m_lPickPos_T + m_lBondPos_T)/2;

		//szMsg.Format("Pick Position:%d, Bond Position:%d, Prepick:%d", m_lPickPos_T,m_lBondPos_T,m_lPrePickPos_T);
		//AfxMessageBox(szMsg);
		int nTStatus = T_MoveTo(lPrePickPos_T);
		//szMsg.Format("Calibrate PREPICK position from centre point T = %ld ...", lPrePickPos_T);
		//HmiMessage(szMsg);
		//int nCompansation = CMS896AStn::CalibratePrepickPositionForPreHeat(BH_AXIS_T, &m_stBHAxis_T);
		//if (nCompansation < 130209)
		//	lPrePickPos_T = lPrePickPos_T - nCompansation + 52083;
		//else
		//	lPrePickPos_T = lPrePickPos_T - nCompansation + 52083 + 156250;
		//szMsg.Format("PREPICK position calibrated to T = %ld", lPrePickPos_T);
		//HmiMessage(szMsg);

		m_lPrePickPos_T = lPrePickPos_T;
		nTStatus = T_MoveTo(lPrePickPos_T);
		m_lBPGeneral_1	  = lPrePickPos_T;
#endif
	}

	T_Profile(NORMAL_PROF);
	
	//v4.53A14
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_ChangeHeadPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	INT	nConvID;
	IPC_CServiceMessage stMsg;

	typedef struct 
	{
		LONG	lPosition;
		BOOL	bFromZero;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}


	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//v3.93
	Z_MoveToHome();
	if (m_bIsArm2Exist == TRUE)
	{
		Z2_MoveToHome();
	}
	if (m_bMS60EjElevator)		//v4.51A6
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}


#ifdef NU_MOTION
	//NuMotionDataLogForBHDebug(TRUE);
	//m_bIsDataLogForBHDebug = TRUE;
#endif

	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lPrePickPos_T);

	//If learn from Home Position
	if (stInfo.bFromZero == TRUE)
	{
		m_lBPGeneral_TmpB = 0;

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_7 = 0;	
				break;	//Bond
			case 2:		
				m_lBPGeneral_8 = 0;	
				break;	//Replace
			case 3:		
				m_lBPAccLvlZ = 0; 
				break;	// Auto Clean Collet
			default:	
				m_lBPGeneral_6 = 0;	
				break;	//Pick
		}
	}

	switch (stInfo.lPosition)
	{
		case 1:		//Bond
			m_lBondHead1TempLearnLevel = m_lBondLevel_Z;
			T_SMoveTo(m_lBondPos_T);
			Sleep(1000);
			if (stInfo.bFromZero == FALSE)
			{
				Z_MoveTo(m_lBondLevel_Z);
				m_lBPGeneral_TmpB = m_lBondLevel_Z;
			}
			break;

		case 2:		//Replace
			T_SMoveTo(m_lPickPos_T);
			Sleep(1000);
			if (stInfo.bFromZero == FALSE)
			{
				Z_MoveTo(m_lReplaceLevel_Z);
				m_lBPGeneral_TmpB = m_lReplaceLevel_Z;
			}
			break;
		case 3:
			T_SMoveTo(m_lAutoCleanCollet_T, SFM_WAIT);
			Sleep(50);

			// let bin table move to clean position
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("BTMoveToACCBrush"), stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			Sleep(1000);
			if (stInfo.bFromZero == FALSE)
			{
				Z_MoveTo(m_lAutoCleanCollet_Z);
				m_lBPGeneral_TmpB = m_lAutoCleanCollet_Z;
			}
			break;

		default:	//Pick
			m_lBondHead1TempLearnLevel = m_lPickLevel_Z;
			UserSearchDieToPrCenter();
			T_SMoveTo(m_lPickPos_T);
			Sleep(1000);

			if (m_bMS60EjElevator)
			{
				EjElevator_MoveTo(m_lStandby_EJ_Elevator);
			}
				
			if (stInfo.bFromZero == FALSE)
			{
				Z_MoveTo(m_lPickLevel_Z);
				m_lBPGeneral_TmpB = m_lPickLevel_Z;
			}
			else
			{
				//if (m_bMS60EjElevator)		//v4.51A6
				//{
					//m_lSwingLevel_Z = 0 - m_lSwingOffset_Z;
				//	Z_MoveTo(-1 * m_lSwingOffset_Z, 1, TRUE);	//v4.51A8
				//}
			}
			break;
	}

#ifdef NU_MOTION
	if (m_bIsDataLogForBHDebug == TRUE)
	{
		NuMotionDataLogForBHDebug(FALSE);
		m_bIsDataLogForBHDebug = FALSE;
	}
#endif

	T_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_TnZToPick1(IPC_CServiceMessage &svMsg)
{
	T_Profile(SETUP_PROF);
	T_SMoveTo(m_lPickPos_T);
	Sleep(1000);
	EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	T_Profile(NORMAL_PROF);
	Z_MoveTo(m_lPickLevel_Z);
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_TnZToPick2(IPC_CServiceMessage &svMsg)
{
	T_SMoveTo(m_lBondPos_T);
	Sleep(1000);
	EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	Z2_MoveTo(m_lPickLevel_Z2);
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_KeyInHeadPosition(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	
	if (m_bMS60EjElevator && !EjElevator_IsPowerOn())		//v4.51A6
	{
		SetErrorMessage("EJ-CAP is not power on in BH_KeyInHeadPosition");
		CString szErr = "\nEJ-CAP is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}
	if (!Z_IsPowerOn())		//v4.46T21	//Cree XML
	{
		SetErrorMessage("BHZ not power on in BH_KeyInHeadPosition");
		CString szErr = "\nBHZ is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}


	//	check when key in
	if( m_bAutoLearningBHZ && (stInfo.lPosition==0 || stInfo.lPosition==1) )
	{
		if( stInfo.lStep<(m_lBondHead1TempLearnLevel - m_lBHZAutoLearnDriveInLimit) )
		{
			HmiMessage_Red_Back("Input value out of limit!", "Auto Learn");
			return TRUE;
		}
	}

	//v4.51A6
	/*
	if (m_bMS60EjElevator && (stInfo.lPosition == 0))
	{
		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								

		if ((stInfo.lStep < lMinPos) || (stInfo.lStep > lMaxPos))
		{
			CString szErr;
			szErr.Format("EJ-CAP value is out of limit! %ld, %ld, %ld", 
				stInfo.lStep, lMinPos, lMaxPos);
			HmiMessage_Red_Back(szErr, "Auto Learn");
			return TRUE;
		}
	}*/


	switch (stInfo.lPosition)
	{
		case 1:		//Bond
			m_lBPGeneral_7 = stInfo.lStep;
			Z_MoveTo(m_lBPGeneral_7);
			break;

		case 2:		//Replace
			m_lBPGeneral_8 = stInfo.lStep;
			Z_MoveTo(m_lBPGeneral_8);
			break;
		case 3:	// Auto Clean Collet
			m_lBPAccLvlZ = stInfo.lStep;
			Z_MoveTo(m_lBPAccLvlZ);
			break;

		default:	//Pick
			m_lBPGeneral_6 = stInfo.lStep;
			Z_MoveTo(m_lBPGeneral_6);
			break;
	}

	if (m_bEnableBHZOpenDac)	//auto learn DAC value
	{
		Z_Sync();
		Sleep(500);
		if (stInfo.lPosition == 0)		//pick
		{
			AutoLearnDAC(TRUE, BH_MS100_BHZ1);
		}
		else if (stInfo.lPosition == 1)	//bond
		{
			AutoLearnDAC(FALSE, BH_MS100_BHZ1);
		}
	}

	return TRUE;
}


LONG CBondHead::BH_MovePosHeadPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	BOOL bLearnPick = FALSE;	//v4.51A6
	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_7 + stInfo.lStep;	
			break;	//Bond
		case 2:		
			lCurrentPos = m_lBPGeneral_8 + stInfo.lStep;	
			break;	//Replace
		case 3:		
			lCurrentPos = m_lBPAccLvlZ + stInfo.lStep;		
			break;	// Auto Clean Collet
		default:	
			bLearnPick = TRUE;
			lCurrentPos = m_lBPGeneral_6 + stInfo.lStep;	
			break;	//Pick
	}


	//v4.46T21	//Cree XML
	if (m_bMS60EjElevator && bLearnPick && !EjElevator_IsPowerOn())		//v4.51A6
	{
		SetErrorMessage("EJ-CAP is not power on in BH_MovePosHeadPosition");
		CString szErr = "\nEJ-CAP is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}
	else if (!Z_IsPowerOn())
	{
		SetErrorMessage("BHZ not power on in BH_MovePosHeadPosition");
		CString szErr = "\nBHZ is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}


	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MAX_DISTANCE);		

	/*
	if ( m_bMS60EjElevator && bLearnPick )		//v4.51A6
	{
		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
		if ((lCurrentPos < lMinPos) || (lCurrentPos > lMaxPos))
		{
			CString szErr;
			szErr.Format("EJ-CAP value is out of limit! %ld, %ld, %ld", lCurrentPos, lMinPos, lMaxPos);
			HmiMessage_Red_Back(szErr, "Auto Learn");
			return TRUE;
		}

		EjElevator_MoveTo(lCurrentPos);
		m_lBPGeneral_6 = lCurrentPos;

		CString szLog;
		szLog.Format("EJ-CAP MOVE-POS to %ld", m_lBPGeneral_6);
		HmiMessage(szLog);
	}
	else*/ 
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		Z_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_7 = lCurrentPos;	
				break;
			case 2:		
				m_lBPGeneral_8 = lCurrentPos;	
				break;
			case 3:		
				m_lBPAccLvlZ	= lCurrentPos;	
				break;
			default:	
				m_lBPGeneral_6 = lCurrentPos;	
				break;
		}
	}

	if (m_bEnableBHZOpenDac)	//auto learn DAC value
	{
		Z_Sync();
		Sleep(500);
		if (stInfo.lPosition == 0)		//pick
		{
			AutoLearnDAC(TRUE, BH_MS100_BHZ1);
		}
		else if (stInfo.lPosition == 1)	//bond
		{
			AutoLearnDAC(FALSE, BH_MS100_BHZ1);
		}
	}

	return TRUE;
}

LONG CBondHead::BH_MoveNegHeadPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	BOOL bLearnPick = FALSE;	//v4.51A6
	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_7 - stInfo.lStep;	
			break;	//Bond
		case 2:		
			lCurrentPos = m_lBPGeneral_8 - stInfo.lStep;	
			break;	//Replace
		case 3:		
			lCurrentPos = m_lBPAccLvlZ - stInfo.lStep;		
			break;	// Auto Clean Collet
		default:	
			bLearnPick = TRUE;
			lCurrentPos = m_lBPGeneral_6 - stInfo.lStep;	
			break;	//Pick
	}

	// check when drive in down
	if( m_bAutoLearningBHZ && (stInfo.lPosition==0 || stInfo.lPosition==1) )
	{
		if( lCurrentPos<(m_lBondHead1TempLearnLevel - m_lBHZAutoLearnDriveInLimit) )
		{
			HmiMessage_Red_Back("Input value out of limit!", "Auto Learn");
			return TRUE;
		}
	}

	
	if (m_bMS60EjElevator && bLearnPick && !EjElevator_IsPowerOn())		//v4.51A6
	{
		SetErrorMessage("EJ-CAP is not power on in BH_MoveNegHeadPosition");
		CString szErr = "\nEJ-CAP is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}
	else if (!Z_IsPowerOn())	//v4.46T21	//Cree XML
	{
		SetErrorMessage("BHZ not power on in BH_MoveNegHeadPosition");
		CString szErr = "\nBHZ is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MAX_DISTANCE);								
	
	/*if ( m_bMS60EjElevator && bLearnPick )		//v4.51A6
	{
		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
		if ((lCurrentPos < lMinPos) || (lCurrentPos > lMaxPos))
		{
			CString szErr;
			szErr.Format("EJ-CAP value is out of limit! %ld, %ld, %ld", lCurrentPos, lMinPos, lMaxPos);
			HmiMessage_Red_Back(szErr, "Auto Learn");
			return TRUE;
		}

		EjElevator_MoveTo(lCurrentPos);
		m_lBPGeneral_6 = lCurrentPos;	
		
		CString szLog;
		szLog.Format("EJ-CAP MOVE-NEG to %ld", m_lBPGeneral_6);
HmiMessage(szLog);
	}	
	else */
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		Z_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_7 = lCurrentPos;	
				break;
			case 2:		
				m_lBPGeneral_8 = lCurrentPos;	
				break;
			case 3:		
				m_lBPAccLvlZ	= lCurrentPos;	
				break;
			default:	
				m_lBPGeneral_6 = lCurrentPos;	
				break;
		}
	}

	if (m_bEnableBHZOpenDac)	//auto learn DAC value
	{
		Z_Sync();
		Sleep(500);
		if (stInfo.lPosition == 0)		//pick
		{
			AutoLearnDAC(TRUE, BH_MS100_BHZ1);
		}
		else if (stInfo.lPosition == 1)	//bond
		{
			AutoLearnDAC(FALSE, BH_MS100_BHZ1);
		}
	}

	return TRUE;
}


LONG CBondHead::BH_EjtMoveTo(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn;
	svMsg.GetMsg(sizeof(BOOL), &bReturn);

	//v4.08
	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (bReturn == TRUE)
	{
		Ej_MoveTo(m_lEjectLevel_Ej, SFM_WAIT);
	}
	else
	{
		//	SetEjectorVacuum(FALSE);
		Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondHead::BH_AutoLearnHeadPosn(IPC_CServiceMessage &svMsg)
{
	LONG		lPosition;
	CString		szTest;
	BOOL		bReturn = TRUE;


	svMsg.GetMsg(sizeof(LONG), &lPosition);

	/*if (m_bMS60EjElevator && (lPosition == 0))		//v4.51A6
	{
		if (AutoLearnEjElvLevelUsingAirFlowSensor() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);	
			return 1;
		}
	}
	else*/
	//{
	if (AutoLearnBondLevelUsingCTSensor(lPosition) == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);	
		return 1;
	}
	//}

	if (m_bEnableBHZOpenDac)	//auto learn DAC value
	{
		Z_Sync();
		Sleep(500);
		if (lPosition == 0)		//pick
		{
			AutoLearnDAC(TRUE, BH_MS100_BHZ1);
		}
		else if (lPosition == 1)	//bond
		{
			AutoLearnDAC(FALSE, BH_MS100_BHZ1);
		}
	}

	//v4.48A9
	if ( (lPosition == 1) )	//Bond
	{	
		m_lBPGeneral_7 += m_lBondDriveIn;
		Z_MoveTo(m_lBPGeneral_7, SFM_WAIT, TRUE);
	}

	if ( (lPosition == 0) )	//Pick
	{
		m_lBPGeneral_6 += m_lPickDriveIn;
		Z_MoveTo(m_lBPGeneral_6);
	}

	//	when press the auto learn button
	m_bAutoLearningBHZ = FALSE;
	if( (lPosition==0 || lPosition==1) && m_lBHZAutoLearnDriveInLimit!=0 )
	{
		m_bAutoLearningBHZ = TRUE;
	}

	CString szContent;
	szContent.LoadString(HMB_GENERAL_COMPLETED);
	HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BH_ConfirmBondHeadSetup(IPC_CServiceMessage &svMsg)
{
	m_bAutoLearningBHZ				= FALSE;	//	confirm
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		return TRUE;
	}

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bReturn = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	StartLoadingAlert();		//v3.86
	INT nStatus = Z_MoveToHome();
	if (m_bMS60EjElevator)		//v4.51A6
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}
	Sleep(500);

	//v3.96T1
	//Extra protection to protect against BH collision/safety
	CString szLog;
	INT nCount = 0;
	BOOL bHomeT = TRUE;
	while (IsCoverOpen())
	{
		CloseLoadingAlert();
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		SetErrorMessage("Machine cover open detected in CONFIRM BH setup");

		nCount++;
		if (nCount >= 3)
		{
			LONG lHmiStatus = HmiMessage("Cover sensor still open in CONFIRM BH setup; continue to move T motor?", 
										 "BondHead Setup", glHMI_MBX_CONTINUESTOP | 0x80000000);
			if (lHmiStatus != glHMI_CONTINUE)
			{
				bHomeT = FALSE;
			}
		}
	}

	if (bHomeT && (nStatus == gnOK))	//v4.46T21
	{
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
	}


	if (m_lPickLevel_Z != m_lBPGeneral_6)
	{
	}
	else if (m_lBondLevel_Z != m_lBPGeneral_7)
	{
	}
	else if (m_lAutoCleanCollet_Z != m_lBPAccLvlZ)
	{
		m_lAutoCleanCollet_Z = m_lBPAccLvlZ;
		LogItems(CLEAN_COLLET_LEVEL);
	}


	//Update variable
	LONG lLearnZ = 0;
	svMsg.GetMsg(sizeof(LONG), &lLearnZ);
	//xx check new/old and pick/bond level difference.
	if( lLearnZ==0 && m_lBondHead1TempLearnLevel!=0 )
	{
		if ( CheckNewOldLevelOffset(labs(m_lBondHead1TempLearnLevel - m_lBPGeneral_6)) )
		{
			szLog.Format("BHZ PICK Level updated from %ld to %ld", m_lPickLevel_Z, m_lBPGeneral_6);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			LogItems(PICK_LEVEL_Z1);	
		}
		else 
		{
			szLog.Format("BHZ PICK Level cancelled from %ld to %ld.\nBecause offset over %d steps.",
				m_lBPGeneral_6, m_lPickLevel_Z, m_lBondHeadToleranceLevel);
			HmiMessage(szLog);

			szLog.Format("BHZ PICK Level cancelled from %ld to %ld", m_lBPGeneral_6, m_lPickLevel_Z);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			m_lBPGeneral_6 = m_lPickLevel_Z;
		}
	}
	if( lLearnZ==1 && m_lBondHead1TempLearnLevel!=0 )	//	bond
	{
		if ( CheckNewOldLevelOffset(labs(m_lBondHead1TempLearnLevel - m_lBPGeneral_7)) )
		{
			szLog.Format("BHZ BOND Level updated from %ld to %ld", m_lBondLevel_Z, m_lBPGeneral_7);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			LogItems(BOND_LEVEL_Z1);
		}
		else 
		{
			szLog.Format("BHZ BOND Level cancelled from %ld to %ld.\nBecause offset over %d steps.",
				m_lBPGeneral_7, m_lBondLevel_Z, m_lBondHeadToleranceLevel);
			HmiMessage(szLog);

			szLog.Format("BHZ BOND Level cancelled from %ld to %ld", m_lBPGeneral_7, m_lBondLevel_Z);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			m_lBPGeneral_7 = m_lBondLevel_Z;
		}
	}
	m_lBondHead1TempLearnLevel = 0;

	if( m_bUseMultiProbeLevel && lLearnZ==1 )
	{
		//	should know it is confirm for Z1 bond level.
		//	should know it confirms for Z2 bond level, then get BT position.
		LONG lX = 0;
		LONG lY = 0;
		BT_Get_XY_Posn(lX, lY);
		//	get bin table current position X and Y.
		m_lZ1BondLevelBT_X = lX;
		m_lZ1BondLevelBT_Y = lY;
		m_bZ1BondLevelBT_R = IsMS90BTRotated();
		BH_MBL_LogLevel();
		CString szMsg;
		szMsg.Format("Setup BHZ1 new BT %d,%d, BT Rotate %d", lX, lY, IsMS90BTRotated());
		SetAlarmLog(szMsg);
	}
	m_lPickLevel_Z		= m_lBPGeneral_6;
	m_lBondLevel_Z		= m_lBPGeneral_7; 
	m_lReplaceLevel_Z	= m_lBPGeneral_8; 
	m_lAutoCleanCollet_Z = m_lBPAccLvlZ;
	m_lSwingLevel_Z		= max(m_lPickLevel_Z, m_lBondLevel_Z) + m_lSwingOffset_Z;
	if( lLearnZ==1 )
	{
		SetBondPadLevel(FALSE);	//	confirm bhz1
	}

	//xx check new/old and pick/bond level difference.
	CheckZ1PickBondLevelOffset();

	T_Profile(NORMAL_PROF);
	SaveBhData();

	CloseLoadingAlert();	//v3.86

	SetStatusMessage("Bondhead level is updated");

	// 3466
	SetGemValue("BH_PickDieLevel",		m_lPickLevel_Z);
	SetGemValue("BH_BondDieLevel",		m_lBondLevel_Z);
	SetGemValue("BH_ReplaceDieLevel",	m_lReplaceLevel_Z);
	SetGemValue("BH_SwingDieLevel",		m_lSwingLevel_Z);
	// 7601
	SendEvent(SG_CEID_BH_LEVEL, FALSE);

	//v4.53A14
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_CancelBondHeadSetup(IPC_CServiceMessage &svMsg)
{
	m_bAutoLearningBHZ				= FALSE;	//	cancel
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		return TRUE;
	}

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bReturn = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	StartLoadingAlert();		//v3.86
	INT nStatus = Z_MoveToHome();
	if (m_bMS60EjElevator)		//v4.51A6
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}
	Sleep(500);

	m_lBondHead1TempLearnLevel = 0;

	//v3.96T1
	//Extra protection to protect against BH collision/safety
	INT nCount = 0;
	BOOL bHomeT = TRUE;
	if( LoopCheckCoverOpen("CANCEL BH setup")==FALSE )
	{
		bHomeT = FALSE;
		return FALSE;   //
	}

	if (bHomeT && (nStatus == gnOK))	//v4.46T21
	{		
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
	}


	//Restore HMI variable
	m_lBPGeneral_6 = m_lPickLevel_Z;
	m_lBPGeneral_7 = m_lBondLevel_Z;
	m_lBPGeneral_8 = m_lReplaceLevel_Z;
	m_lBPAccLvlZ   = m_lAutoCleanCollet_Z;

	T_Profile(NORMAL_PROF);
	
	CloseLoadingAlert();	//v3.86

	//v4.53A14
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

/*******************************************************************************/
/******************       Bond Head Z2 Setting Commands       ******************/
/*******************************************************************************/
LONG CBondHead::BH_ChangeHeadPositionZ2(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		BOOL	bFromZero;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}


#ifdef NU_MOTION
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//v3.93
	Z_MoveToHome();
	if (m_bIsArm2Exist == TRUE)
	{
		Z2_MoveToHome();
	}
	if (m_bMS60EjElevator)		//v4.51A6
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//NuMotionDataLogForBHDebug(TRUE);
	//m_bIsDataLogForBHDebug = TRUE;

	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lPrePickPos_T);

	//If learn from Home Position
	if (stInfo.bFromZero == TRUE)
	{
		m_lBPGeneral_TmpB = 0;

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_C = 0;	
				break;	//Bond
			case 2:		
				m_lBPGeneral_D = 0;	
				break;	//Replace
			default:	
				m_lBPGeneral_B = 0;	
				break;	//Pick
		}
	}

	switch (stInfo.lPosition)
	{
	case 1:		//Bond
		m_lBondHead2TempLearnLevel = m_lBondLevel_Z2;
		T_SMoveTo(m_lPickPos_T);
		Sleep(1000);
		if (stInfo.bFromZero == FALSE)
		{
			Z2_MoveTo(m_lBondLevel_Z2);
			m_lBPGeneral_TmpB = m_lBondLevel_Z2;
		}
		break;

	case 2:		//Replace
		T_SMoveTo(m_lBondPos_T);
		Sleep(1000);
		if (stInfo.bFromZero == FALSE)
		{
			Z2_MoveTo(m_lReplaceLevel_Z2);
			m_lBPGeneral_TmpB = m_lReplaceLevel_Z2;
		}
		break;

	default:	//Pick
		m_lBondHead2TempLearnLevel = m_lPickLevel_Z2;
		UserSearchDieToPrCenter(TRUE);		//v4.44T2
		T_SMoveTo(m_lBondPos_T);
		Sleep(1000);

		if (m_bMS60EjElevator)		//v4.51A17
		{
			EjElevator_MoveTo(m_lStandby_EJ_Elevator);	
		}

		if (stInfo.bFromZero == FALSE)
		{
			Z2_MoveTo(m_lPickLevel_Z2);
			m_lBPGeneral_TmpB = m_lPickLevel_Z2;
		}
			else
			{
				//if (m_bMS60EjElevator)		//v4.51A6
				//{
					//m_lSwingLevel_Z2 = 0 - m_lSwingOffset_Z2;
				//	Z2_MoveTo(-1 * m_lSwingOffset_Z2, 1, TRUE);		//v4.51A8
				//}
			}
		break;
	}

	if (m_bIsDataLogForBHDebug == TRUE)
	{
		NuMotionDataLogForBHDebug(FALSE);
		m_bIsDataLogForBHDebug = FALSE;
	}

	T_Profile(NORMAL_PROF);
#endif
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_KeyInHeadPositionZ2(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//v4.46T21	//Cree XML
	if (m_bMS60EjElevator && !EjElevator_IsPowerOn())
	{
		SetErrorMessage("EJ-CAP not power on in BH_KeyInHeadPositionZ2");
		CString szErr = "\nEJ-CAP is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}
	if (!Z2_IsPowerOn())
	{
		SetErrorMessage("BHZ2 not power on in BH_KeyInHeadPositionZ2");
		CString szErr = "\nBHZ 2 is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}

	// check when key in
	if( m_bAutoLearningBHZ && (stInfo.lPosition==0 || stInfo.lPosition==1) )
	{
		if( stInfo.lStep<(m_lBondHead2TempLearnLevel - m_lBHZAutoLearnDriveInLimit) )
		{
			HmiMessage_Red_Back("Input value out of limit!", "Auto Learn");
			return TRUE;
		}
	}

	/*
	if (m_bMS60EjElevator && (stInfo.lPosition == 0))
	{
		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
		if ((stInfo.lStep < lMinPos) || (stInfo.lStep > lMaxPos))
		{
			CString szErr;
			szErr.Format("EJ-CAP value is out of limit! %ld, %ld, %ld", 
				stInfo.lStep, lMinPos, lMaxPos);
			HmiMessage_Red_Back(szErr, "Auto Learn");
			return TRUE;
		}
	}*/


#ifdef NU_MOTION
	switch (stInfo.lPosition)
	{
		case 1:		//Bond
			m_lBPGeneral_C = stInfo.lStep;
			Z2_MoveTo(m_lBPGeneral_C);
			break;

		case 2:		//Replace
			m_lBPGeneral_D = stInfo.lStep;
			Z2_MoveTo(m_lBPGeneral_D);
			break;

		default:	//Pick
			m_lBPGeneral_B = stInfo.lStep;
			Z2_MoveTo(m_lBPGeneral_B);
			break;
	}

	if (m_bEnableBHZOpenDac)	//auto learn DAC value
	{
		Z2_Sync();
		Sleep(500);
		if (stInfo.lPosition == 0)		//pick
		{
			AutoLearnDAC(TRUE, BH_MS100_BHZ2);
		}
		else if (stInfo.lPosition == 1)	//bond
		{
			AutoLearnDAC(FALSE, BH_MS100_BHZ2);
		}
	}
#endif
	return TRUE;
}


LONG CBondHead::BH_MovePosHeadPositionZ2(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}


#ifdef NU_MOTION
	BOOL bLearnPick = FALSE;		//v4.51A6
	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_C + stInfo.lStep;	
			break;	//Bond
		case 2:		
			lCurrentPos = m_lBPGeneral_D + stInfo.lStep;	
			break;	//Replace
		default:	
			bLearnPick = TRUE;
			lCurrentPos = m_lBPGeneral_B + stInfo.lStep;	
			break;	//Pick
	}

	
	if (m_bMS60EjElevator && bLearnPick && !EjElevator_IsPowerOn())	//v4.51A6
	{
		SetErrorMessage("EJ-CAP is not power on in BH_MovePosHeadPositionZ2");
		CString szErr = "\nEJ CAP is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}
	else if (!Z2_IsPowerOn())							//v4.46T21	//Cree XML
	{
		SetErrorMessage("BHZ2 not power on in BH_MovePosHeadPositionZ2");
		CString szErr = "\nBHZ 2 is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MAX_DISTANCE);								
	
	/*
	if (m_bMS60EjElevator && bLearnPick)			//v4.51A6
	{
		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
		if ((lCurrentPos < lMinPos) || (lCurrentPos > lMaxPos))
		{
			CString szErr;
			szErr.Format("EJ-CAP value is out of limit! %ld, %ld, %ld", 
				lCurrentPos, lMinPos, lMaxPos);
			HmiMessage_Red_Back(szErr, "Auto Learn");
			return TRUE;
		}

		EjElevator_MoveTo(lCurrentPos);
		m_lBPGeneral_B = lCurrentPos;	
	}
	else */
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		Z2_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_C = lCurrentPos;	
				break;
			case 2:		
				m_lBPGeneral_D = lCurrentPos;	
				break;
			default:	
				m_lBPGeneral_B = lCurrentPos;	
				break;
		}
	}

	if (m_bEnableBHZOpenDac)	//auto learn DAC value
	{
		Z2_Sync();
		Sleep(500);
		if (stInfo.lPosition == 0)		//pick
		{
			AutoLearnDAC(TRUE, BH_MS100_BHZ2);
		}
		else if (stInfo.lPosition == 1)	//bond
		{
			AutoLearnDAC(FALSE, BH_MS100_BHZ2);
		}
	}
#endif
	return TRUE;
}

LONG CBondHead::BH_MoveNegHeadPositionZ2(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//For Mega Da
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

#ifdef NU_MOTION
	BOOL bLearnPick = FALSE;
	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_C - stInfo.lStep;	
			break;	//Bond
		case 2:		
			lCurrentPos = m_lBPGeneral_D - stInfo.lStep;	
			break;	//Replace
		default:	
			bLearnPick = TRUE;
			lCurrentPos = m_lBPGeneral_B - stInfo.lStep;	
			break;	//Pick
	}

	// check when drive in down
	if( m_bAutoLearningBHZ && (stInfo.lPosition==0 || stInfo.lPosition==1) )
	{
		if( lCurrentPos<(m_lBondHead2TempLearnLevel - m_lBHZAutoLearnDriveInLimit) )
		{
			HmiMessage_Red_Back("Input value out of limit!", "Auto Learn");
			return TRUE;
		}
	}

	
	if (m_bMS60EjElevator && bLearnPick && !EjElevator_IsPowerOn())		//v4.51A6
	{
		SetErrorMessage("EJ-CAP is not power on in BH_MoveNegHeadPositionZ2");
		CString szErr = "\nEJ CAP is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}
	if (!Z2_IsPowerOn())		//v4.46T21	//Cree XML
	{
		SetErrorMessage("BHZ2 not power on in BH_MoveNegHeadPositionZ2");
		CString szErr = "\nBHZ 2 is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		return TRUE;
	}


	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MAX_DISTANCE);								
	
	/*if (m_bMS60EjElevator && bLearnPick)			//v4.51A6
	{
		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
		if ((lCurrentPos < lMinPos) || (lCurrentPos > lMaxPos))
		{
			CString szErr;
			szErr.Format("EJ-CAP value is out of limit! %ld, %ld, %ld", 
				lCurrentPos, lMinPos, lMaxPos);
			HmiMessage_Red_Back(szErr, "Auto Learn");
			return TRUE;
		}

		EjElevator_MoveTo(lCurrentPos);
		m_lBPGeneral_B = lCurrentPos;	
	}	
	else */
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		Z2_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_C = lCurrentPos;	
				break;
			case 2:		
				m_lBPGeneral_D = lCurrentPos;	
				break;
			default:	
				m_lBPGeneral_B = lCurrentPos;	
				break;
		}
	}

	if (m_bEnableBHZOpenDac)	//auto learn DAC value
	{
		Z2_Sync();
		Sleep(500);
		if (stInfo.lPosition == 0)		//pick
		{
			AutoLearnDAC(TRUE, BH_MS100_BHZ2);
		}
		else if (stInfo.lPosition == 1)	//bond
		{
			AutoLearnDAC(FALSE, BH_MS100_BHZ2);
		}
	}
#endif

	return TRUE;
}


LONG CBondHead::BH_AutoLearnHeadPosnZ2(IPC_CServiceMessage &svMsg)
{
	LONG		lPosition;
	CString		szTest;
	BOOL		bReturn = TRUE;
	//For Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	//v4.08
	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


#ifdef NU_MOTION

	/*if (m_bMS60EjElevator && (lPosition == 0))		//v4.51A6
	{
		if (AutoLearnEjElvLevelUsingAirFlowSensor(TRUE) == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);	
			return 1;
		}
	}
	else*/
	//{
		if (AutoLearnBondLevelUsingCTSensorZ2(lPosition) == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);	
			return 1;
		}
	//}

	if (m_bEnableBHZOpenDac)	//auto learn DAC value
	{
		Z2_Sync();
		Sleep(500);
		if (lPosition == 0)		//pick
		{
			AutoLearnDAC(TRUE, BH_MS100_BHZ2);
		}
		else if (lPosition == 1)	//bond
		{
			AutoLearnDAC(FALSE, BH_MS100_BHZ2);
		}
	}

	//v4.48A9
	if ( (lPosition == 1) )			//Bond
	{	
		m_lBPGeneral_C += m_lBondDriveIn;
		Z2_MoveTo(m_lBPGeneral_C, 1, TRUE);
	}
	else if ( (lPosition == 0) )	//Pick
	{
		m_lBPGeneral_B += m_lPickDriveIn;
		Z2_MoveTo(m_lBPGeneral_B);
	}

	//	when press the auto learn button
	m_bAutoLearningBHZ = FALSE;
	if( (lPosition==0 || lPosition==1) && m_lBHZAutoLearnDriveInLimit!=0 )
	{
		m_bAutoLearningBHZ = TRUE;
	}

	CString szContent;
	szContent.LoadString(HMB_GENERAL_COMPLETED);
	HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BH_ConfirmBondHeadSetupZ2(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	//For Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;

	m_bAutoLearningBHZ				= FALSE;	//	confirm
	//v4.08
	if (m_bDisableBH)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

#ifdef NU_MOTION
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		//bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	StartLoadingAlert();	//v3.86

	INT nStatus = Z2_MoveToHome();
	if (m_bMS60EjElevator)		//v4.51A6
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}
	Sleep(1000);


	//v3.96T1
	//Extra protection to protect against BH collision/safety
	CString szLog;
	INT nCount = 0;
	BOOL bHomeT = TRUE;
	while (IsCoverOpen())
	{
		CloseLoadingAlert();
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		SetErrorMessage("Machine cover open detected in CONFIRM BH setup");

		nCount++;
		if (nCount >= 3)
		{
			LONG lHmiStatus = HmiMessage("Cover sensor still open in CONFIRM BH setup; continue to move T motor?", 
										 "BondHead Setup", glHMI_MBX_CONTINUESTOP | 0x80000000);
			if (lHmiStatus != glHMI_CONTINUE)
			{
				bHomeT = FALSE;
			}
		}
	}


	if (bHomeT && (nStatus == gnOK))	//v4.46T21
	{
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
	}

	CloseLoadingAlert();	//v3.86

	//Update variable
	LONG lLearnZ = 0;
	svMsg.GetMsg(sizeof(LONG), &lLearnZ);
	//xx check new/old and pick/bond level difference.
	if( lLearnZ==0 && m_lBondHead2TempLearnLevel!=0 )	//	pick
	{
		if ( CheckNewOldLevelOffset(labs(m_lBondHead2TempLearnLevel - m_lBPGeneral_B)) )
		{
			szLog.Format("BHZ2 PICK Level updated from %ld to %ld", m_lPickLevel_Z2, m_lBPGeneral_B);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			LogItems(PICK_LEVEL_Z2);
		}
		else 
		{
			szLog.Format("BHZ2 PICK Level cancelled from %ld to %ld.\nBecause offset over %d steps.",
				m_lBPGeneral_B, m_lPickLevel_Z2, m_lBondHeadToleranceLevel);
			HmiMessage(szLog);
			szLog.Format("BHZ2 PICK Level cancelled from %ld to %ld", m_lBPGeneral_B, m_lPickLevel_Z2);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			m_lBPGeneral_B = m_lPickLevel_Z2;
		}
	}
	if( lLearnZ==1 && m_lBondHead2TempLearnLevel!=0 )	//	bond
	{
		if ( CheckNewOldLevelOffset(labs(m_lBondHead2TempLearnLevel - m_lBPGeneral_C)) )
		{
			szLog.Format("BHZ2 BOND Level updated from %ld to %ld", m_lBondLevel_Z2, m_lBPGeneral_C);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			LogItems(BOND_LEVEL_Z2);
		}
		else 
		{
			szLog.Format("BHZ2 BOND Level cancelled from %ld to %ld.\nBecause offset over %d steps.",
				m_lBPGeneral_C, m_lBondLevel_Z2, m_lBondHeadToleranceLevel);
			HmiMessage(szLog);
			szLog.Format("BHZ2 BOND Level cancelled from %ld to %ld", m_lBPGeneral_C, m_lBondLevel_Z2);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			m_lBPGeneral_C = m_lBondLevel_Z2;
		}
	}
	m_lBondHead2TempLearnLevel = 0;

	if( m_bUseMultiProbeLevel && lLearnZ==1 )
	{
		//	should know it confirms for Z2 bond level, then get BT position.
		LONG lX = 0;
		LONG lY = 0;
		BT_Get_XY_Posn(lX, lY);
		//	get bin table current position X and Y.
		m_lZ2BondLevelBT_X = lX;
		m_lZ2BondLevelBT_Y = lY;
		m_bZ2BondLevelBT_R = IsMS90BTRotated();
		BH_MBL_LogLevel();
		CString szMsg;
		szMsg.Format("Setup BHZ2 new BT %d,%d, BT Rotate %d", lX, lY, IsMS90BTRotated());
		SetAlarmLog(szMsg);
	}
	m_lPickLevel_Z2		= m_lBPGeneral_B;
	m_lBondLevel_Z2		= m_lBPGeneral_C; 
	m_lReplaceLevel_Z2	= m_lBPGeneral_D; 
	m_lSwingLevel_Z2	= max(m_lPickLevel_Z2, m_lBondLevel_Z2) + m_lSwingOffset_Z2;
	//xx check new/old and pick/bond level difference.
	CheckZ2PickBondLevelOffset();

	if( lLearnZ==1 )
	{
		SetBondPadLevel(TRUE);	//	confirm bhz2
	}

	T_Profile(NORMAL_PROF);

	SaveBhData();

	SetStatusMessage("Bondhead level is updated");

	// 3466
	SetGemValue("BH_PickDieLevel", m_lPickLevel_Z2);
	SetGemValue("BH_BondDieLevel", m_lBondLevel_Z2);
	SetGemValue("BH_ReplaceDieLevel", m_lReplaceLevel_Z2);
	SetGemValue("BH_SwingDieLevel", m_lSwingLevel_Z2);
	// 7601
	SendEvent(SG_CEID_BH_LEVEL, FALSE);
#endif

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_CancelBondHeadSetupZ2(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	//For Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;

	m_bAutoLearningBHZ				= FALSE;	//	cancel
	//v4.08
	if (m_bDisableBH)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	m_lBondHead2TempLearnLevel = 0;

#ifdef NU_MOTION
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	StartLoadingAlert();	//v3.86
	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF

	INT nStatus = Z2_MoveToHome();
	if (m_bMS60EjElevator)		//v4.51A6
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}
	Sleep(1000);

	//v4.00T1
	//Extra protection to protect against BH collision/safety
	INT nCount = 0;
	BOOL bHomeT = TRUE;
	if( LoopCheckCoverOpen("CANCEL BH setup")==FALSE )
	{
		bHomeT = FALSE;
		return FALSE;
	}

	if (bHomeT && (nStatus == gnOK))	//v4.46T21
	{
		T_SMoveTo(m_lPrePickPos_T);
	}
		
	CloseLoadingAlert();	//v3.86

	//Restore HMI variable
	m_lBPGeneral_B = m_lPickLevel_Z2;
	m_lBPGeneral_C = m_lBondLevel_Z2;
	m_lBPGeneral_D = m_lReplaceLevel_Z2;

	T_Profile(NORMAL_PROF);
#endif
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_UpdateSwingOffsetZ2(IPC_CServiceMessage &svMsg)
{
#ifdef NU_MOTION
	m_lSwingLevel_Z2	= max(m_lPickLevel_Z2, m_lBondLevel_Z2) + m_lSwingOffset_Z2;
	//if (m_bMS60EjElevator)		//v4.51A6
	//{
	//	m_lSwingLevel_Z2 = 0;	// - m_lSwingOffset_Z2;		//v4.51A8
	//}
	SaveBhData();
#endif
	return TRUE;
}


LONG CBondHead::BH_AutoLearnBT2OffsetZ(IPC_CServiceMessage &svMsg)	
{
	BOOL bReturn = TRUE;
	LONG lBT2OffsetZ = 0;
	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	BOOL bResult		= TRUE;
	LONG lEncZ			= 0;
	LONG lEncZForBT1	= 0;
	LONG lEncZForBT2	= 0;

	//For Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

	if (!CMS896AApp::m_bMS100Plus9InchOption)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//Check expander status
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Wafer Expander not closed");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("CHANGE BH setup")==FALSE )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//v4.51A23
	if (CMS896AStn::m_bAutoChangeCollet && !IsColletPusherAtSafePos())
	{
		HmiMessage_Red_Yellow("AGC: Pusher is not at UP position");
		SetErrorMessage("AGC(4): Pusher is not at UP position");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//Check BT frame level
	BOOL bBinLoader		= IsBLEnable();
	if (bBinLoader && !IsBT1FrameLevel())
	{
		SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
		SetErrorMessage("BT frame level not DOWN");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//#1. Prompt dialog to continue or abort
	CString szTitle		= "BT2";
	CString szContent	= "Teach BT2 Offset Z level; continue?";
	LONG lHmiMsgReply	= HmiMessage(szContent, szTitle, glHMI_MBX_YESNO); 
	if (lHmiMsgReply == glHMI_NO)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
		
	CString szLog;
	szLog.Format("Teach BT2 Offset Z start - OLD (%ld)", lBT2OffsetZ);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	Z_MoveTo(0);
	Z2_MoveTo(0);

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() == "Cree") )
	//{
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
#ifdef NU_MOTION
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
#endif
	//}

	T_MoveTo(m_lPickPos_T);


	//#2. Ask BT1 to move to Center position
	LONG lPosn = 0;
	rReqMsg.InitMessage(sizeof(LONG), &lPosn);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToBinArea4Corners", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if (bResult == FALSE)
	{
		Z2_Home();
		T_MoveTo(m_lPrePickPos_T);

		CString szErr;
		szErr = "ERROR: Auto Learn BHZ2 levels fail BT-Move at BT1 corner #0";
		SetErrorMessage(szErr);
		HmiMessage(szErr);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	lEncZForBT1 = 0;	//v4.49A9

	//#3. BHZ2 SEARCH down to find Z level for BT1
	if (!AutoLearnBHZ2BondLevel(lEncZForBT1))
	{
		Z2_Home();
		T_MoveTo(m_lPrePickPos_T);

		CString szErr;
		szErr = "ERROR: Auto Learn BHZ2 levels fail at BT1 corner #0";
		SetErrorMessage(szErr);
		HmiMessage(szErr);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	Z2_Home();


	//#4. Ask BT2 to move to Center position
	lPosn = 0;
	rReqMsg.InitMessage(sizeof(LONG), &lPosn);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToBinArea4Corners2", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if (bResult == FALSE)
	{
		Z2_Home();
		T_MoveTo(m_lPrePickPos_T);

		CString szErr;
		szErr = "ERROR: Auto Learn BHZ2 levels fail BT-Move at BT2 corner #0";
		SetErrorMessage(szErr);
		HmiMessage(szErr);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	lEncZForBT2 = 0;	//v4.49A9

	//#5. BHZ2 SEARCH down to find Z level for BT2
	if (!AutoLearnBHZ2BondLevel(lEncZForBT2))
	{
		Z2_Home();
		T_MoveTo(m_lPrePickPos_T);

		CString szErr;
		szErr = "ERROR: Auto Learn BHZ2 levels fail at BT2 corner #0";
		SetErrorMessage(szErr);
		HmiMessage(szErr);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	Z2_Home();
	T_MoveTo(m_lPrePickPos_T);

	LONG lOffsetZ	= lEncZForBT2 - lEncZForBT1;
	m_lBT2OffsetZ	= lOffsetZ;
	SaveBhData();

	CString szMsg;
	szMsg.Format("BT2 Offset Z = %ld", lOffsetZ);
	HmiMessage(szMsg, "Learn BT2 Z Offset");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_AutoLearnBHZ2BondLevels(IPC_CServiceMessage &svMsg)		//v3.94T4
{
	LONG lEncZ	= 0;
	LONG lEncZ0 = 0;
	LONG lEncZ1 = 0;
	LONG lEncZ2 = 0;
	LONG lEncZ3 = 0;
	LONG lEncZ4 = 0;

	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	BOOL bResult = TRUE;
	LONG lPosn = 0;
	BOOL bReturn = TRUE;

	//For Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

#ifndef NU_MOTION
	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
#endif

	//v4.08
	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	HmiMessage("Start auto-learn BHZ2 levels ...");

	Z_MoveTo(0);
	Z2_MoveTo(0);

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
	//{
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
#ifdef NU_MOTION
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
#endif
	//}

	T_MoveTo(m_lPickPos_T);


	for (INT i = 0; i <= 4; i++)	//v4.08
	{
		lPosn = i;
		rReqMsg.InitMessage(sizeof(LONG), &lPosn);
		
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToBinArea4Corners", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult == FALSE)
		{
			Z2_Home();
			T_MoveTo(m_lPrePickPos_T);

			CString szErr;
			szErr.Format("ERROR: Auto Learn BHZ2 levels fail BT-Move at #%d corner", i);
			HmiMessage(szErr);

			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		lEncZ = 0;	//v4.49A9

		if (!AutoLearnBHZ2BondLevel(lEncZ))
		{
			Z2_Home();
			T_MoveTo(m_lPrePickPos_T);

			CString szErr;
			szErr.Format("ERROR: Auto Learn BHZ2 levels fail at #%d corner", i);
			HmiMessage(szErr);

			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (i == 0)
		{
			lEncZ0 = lEncZ;
		}
		else if (i == 1)
		{
			lEncZ1 = lEncZ;
		}
		else if (i == 2)
		{
			lEncZ2 = lEncZ;
		}
		else if (i == 3)
		{
			lEncZ3 = lEncZ;
		}
		else
		{
			lEncZ4 = lEncZ;
		}

		Z2_MoveTo(0);

		//v4.46T20	//Cree
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
		//{
			if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
			{
				Sleep(100);
				Z2_Move(pApp->GetBHZ2HomeOffset());
			}
		//}
	}

	Z2_Home();
	T_MoveTo(m_lPrePickPos_T);

	CString szMsg;
	szMsg.Format("Auto-Learn BHZ2 levels: %ld(C)  %ld(UL)  %ld(LL)  %ld(LR)  %ld(UR)", lEncZ0, lEncZ1, lEncZ2, lEncZ3, lEncZ4);
	HmiMessage(szMsg);

	//BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_AutoLearnBHZ2BondLevels_BT2(IPC_CServiceMessage &svMsg)		//v4.34T1
{
	LONG lEncZ	= 0;
	LONG lEncZ0 = 0;
	LONG lEncZ1 = 0;
	LONG lEncZ2 = 0;
	LONG lEncZ3 = 0;
	LONG lEncZ4 = 0;

	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	BOOL bResult = TRUE;
	LONG lPosn = 0;
	BOOL bReturn = TRUE;
	//For Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

#ifndef NU_MOTION
	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
#endif

	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (!CMS896AApp::m_bMS100Plus9InchOption)		//MS100 9Inch only
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	HmiMessage("Start auto-learn BHZ2 levels on BT2 ...");

	Z_MoveTo(0);
	Z2_MoveTo(0);

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
	//{
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
#ifdef NU_MOTION
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
#endif
	//}

	T_MoveTo(m_lPickPos_T);


	for (INT i = 0; i <= 4; i++)	//v4.08
	{
		lPosn = i;
		rReqMsg.InitMessage(sizeof(LONG), &lPosn);
		
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToBinArea4Corners2", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult == FALSE)
		{
			Z2_Home();
			T_MoveTo(m_lPrePickPos_T);

			CString szErr;
			szErr.Format("ERROR: Auto Learn BHZ2 levels fail BT-Move at BT2 #%d corner", i);
			HmiMessage(szErr);

			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		lEncZ = 0;	//v4.49A9

		if (!AutoLearnBHZ2BondLevel(lEncZ))
		{
			Z2_Home();
			T_MoveTo(m_lPrePickPos_T);

			CString szErr;
			szErr.Format("ERROR: Auto Learn BHZ2 levels fail at BT2 #%d corner", i);
			HmiMessage(szErr);

			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (i == 0)
		{
			lEncZ0 = lEncZ;
		}
		else if (i == 1)
		{
			lEncZ1 = lEncZ;
		}
		else if (i == 2)
		{
			lEncZ2 = lEncZ;
		}
		else if (i == 3)
		{
			lEncZ3 = lEncZ;
		}
		else
		{
			lEncZ4 = lEncZ;
		}

		Z2_MoveTo(0);

		//v4.46T20	//Cree
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
		//{
			if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
			{
				Sleep(100);
				Z2_Move(pApp->GetBHZ2HomeOffset());
			}
		//}
	}

	Z2_Home();
	T_MoveTo(m_lPrePickPos_T);

	CString szMsg;
	szMsg.Format("Auto-Learn BT2 BHZ2 levels: %ld(C)  %ld(UL)  %ld(LL)  %ld(LR)  %ld(UR)", lEncZ0, lEncZ1, lEncZ2, lEncZ3, lEncZ4);
	HmiMessage(szMsg);

	//BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


/********************************** End of Bond Head Z2 Setup Command **********************************/


LONG CBondHead::BH_ChangeEjectorPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		BOOL	bWithBH;
		BOOL	bFromZero;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (m_bDisableBH)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	
	// Disable Wafer Table Joystick
	SetWaferTableJoystick(FALSE);
	// Lock Wafer Table Joystick
	LockWaferTableJoystick(TRUE);
	LockPRMouseJoystick(TRUE);
	Ej_MoveTo(0);

	//v4.28
	if (IsESMachine())
	{
		INT nConvID = 0;
		IPC_CServiceMessage stMsg;
		BOOL bToEjtOffset = TRUE;

		CString szText;
		CStringList szSelection;
		LONG lSelection = 0, lResult;
		szSelection.AddTail("HOME position");
		szSelection.AddTail("NG-PICK position");
		lResult = HmiSelection("Please select TABLE1 position", "Change Ejector Setup", szSelection, lSelection);

		if (lResult < 0)
		{
			bResult = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return TRUE;
		}
		else if (lResult == 0)	//HOME
		{
			stMsg.InitMessage(sizeof(BOOL), &bToEjtOffset);
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "MoveTable1ToHome", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);	
				}
			}
		}
		else					//EJ Offset XY for NGPick
		{
			stMsg.InitMessage(sizeof(BOOL), &bToEjtOffset);
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "MoveTable1ToEjtOffset", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);	
				}
			}
		}

		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}
	else if (m_bMS60EjElevator)
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}


//v3.83
#ifdef NU_MOTION
	CStringList szSelection;
	szSelection.AddTail("BH 1");
	szSelection.AddTail("BH 2");
	LONG lSelection = 0;

	m_lWithBHDown = 0;
	if ((stInfo.bWithBH == TRUE) && m_bIsArm2Exist)
	{
		LONG lResult = 1;//HmiSelection("Please select BH to PICK level", "BH Selection", szSelection, lSelection);
		
		switch (lResult)
		{
			case 1:		//BH2
				m_lWithBHDown = 1;
				break;
			case 0:		//BH1
				m_lWithBHDown = 0;
				break;
			case -1:	//CANCEL
			default:
				bResult = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bResult);
				return 1;
		}
	}
#endif


	//v3.96T1
	//Extra protection to protect against BH collision/safety
	INT nCount = 0;
	if (stInfo.bWithBH == TRUE) 
	{
		if( LoopCheckCoverOpen("CHANGE EJ setup")==FALSE )
		{
			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return TRUE;
		}
	}


	//If learn from Home Position
	if (stInfo.bFromZero == TRUE)
	{
		m_lBPGeneral_TmpC = 0;

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_A = 0;	
				break;	//Standby Level
			default:	
				m_lBPGeneral_9 = 0;	
				break;	//Up Level
		}
	}

	DisableEjectorDisplay(stInfo.lPosition, EJ_PIN_LEARN_LEVEL);
	switch (stInfo.lPosition)
	{
		case 1:		//Standby
			if (stInfo.bFromZero == FALSE)
			{
				Ej_MoveTo(m_lStandbyLevel_Ej);
				m_lBPGeneral_TmpC = m_lStandbyLevel_Ej;
			}
			break;

		default:	//Up Level
			//If move BH is selected
			if (stInfo.bWithBH == TRUE)
			{
				if (m_bIsArm2Exist == TRUE)
				{
					switch (m_lWithBHDown)
					{
						case 1:		//BH2
							T_MoveTo(m_lBondPos_T);
							Z2_MoveTo(m_lPickLevel_Z2);
							break;

						default:	//BH1
							T_MoveTo(m_lPickPos_T);
							Z_MoveTo(m_lPickLevel_Z);
							break;
					}
				}
				else
				{
					T_MoveTo(m_lPickPos_T);
					Z_MoveTo(m_lPickLevel_Z);
				}
			}

			if (stInfo.bFromZero == FALSE)
			{
				Ej_MoveTo(m_lEjectLevel_Ej);
				m_lBPGeneral_TmpC = m_lEjectLevel_Ej;
			}
			break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_ChangeNVCBinEjectorPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	LONG lUpLevel = 0, lStandbyLevel = 0;

	typedef struct 
	{
		LONG	lMotorType;	//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;	//Up level or Standby Level
		BOOL	bFromZero;	//Is go to home position to learn
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (m_bDisableBH)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	// Disable Wafer Table Joystick
	SetWaferTableJoystick(FALSE);

	// Lock Wafer Table Joystick
	LockWaferTableJoystick(TRUE);
	LockPRMouseJoystick(TRUE);

	if (m_bMS60EjElevator)
	{
		BinEjElevator_MoveTo(m_lStandby_Bin_EJ_Elevator);
	}

	//If learn from Home Position
	if (stInfo.bFromZero == TRUE)
	{
		if (stInfo.lPosition == CHANGE_EJ_STANDBY)	//Standby Level
		{
			lStandbyLevel = 0;
		}
		else	//Up Level
		{
			lUpLevel = 0;
		}
	}
	else
	{
		if (stInfo.lPosition == CHANGE_EJ_STANDBY)	//Standby Level
		{
			switch (stInfo.lMotorType)
			{
				case EJ_PIN_LEARN_LEVEL:
					lStandbyLevel = m_lStandby_Bin_EJ_Pin;
					break;
				case EJ_THETA_LEARN_LEVEL:	
					lStandbyLevel = m_lStandby_Bin_EJ_Theta;
					break;
				case EJ_CAP_LEARN_LEVEL:	
					lStandbyLevel = m_lStandby_Bin_EJ_Cap;
					break;	
				default:
					lStandbyLevel = m_lStandby_Bin_EJ_Elevator;
					break;
			}
		}
		else	//Up Level
		{
			switch (stInfo.lMotorType)
			{
				case EJ_PIN_LEARN_LEVEL:
					lUpLevel = m_lUpLevel_Bin_EJ_Pin;
					break;
				case EJ_THETA_LEARN_LEVEL:
					lUpLevel = m_lUpLevel_Bin_EJ_Theta;
					break;
				case EJ_CAP_LEARN_LEVEL:
					lUpLevel = m_lUpLevel_Bin_EJ_Cap;
					break;	
				default:
					lUpLevel = m_lUpLevel_Bin_EJ_Elevator;
					break;
			}
		}
	}

	if (stInfo.lPosition == CHANGE_EJ_STANDBY)	//Standby Level
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_PIN_LEARN_LEVEL);
				BinEj_MoveTo(lStandbyLevel);
				break;
			case EJ_THETA_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_THETA_LEARN_LEVEL);
				BinEjT_MoveTo(lStandbyLevel);
				break;
			case EJ_CAP_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_CAP_LEARN_LEVEL);
				BinEjCap_MoveTo(lStandbyLevel);
				break;	
			default:
				DisableEjectorDisplay(stInfo.lPosition, EJ_ELEVATOR_LEARN_LEVEL);
				BinEjElevator_MoveTo(lStandbyLevel);
				break;
		}
		m_lBPGeneral_TmpC = lStandbyLevel;
	}
	else	//Up Level
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_PIN_LEARN_LEVEL);
				BinEj_MoveTo(lUpLevel);
				break;
			case EJ_THETA_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_THETA_LEARN_LEVEL);
				BinEjT_MoveTo(lUpLevel);
				break;
			case EJ_CAP_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_CAP_LEARN_LEVEL);
				BinEjCap_MoveTo(lUpLevel);
				break;
			default:
				DisableEjectorDisplay(stInfo.lPosition, EJ_ELEVATOR_LEARN_LEVEL);
				BinEjElevator_MoveTo(lUpLevel);
				break;
		}
		m_lBPGeneral_TmpC = lUpLevel;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_ChangeNVCEjectorPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	LONG lUpLevel = 0, lStandbyLevel = 0, lContactLevel = 0;

	typedef struct 
	{
		LONG	lMotorType;		//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;		//Up level or Standby Level or Contact Level
		BOOL	bFromZero;		//Is go to home position to learn
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (m_bDisableBH)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	/*if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}*/

	// Disable Wafer Table Joystick
	SetWaferTableJoystick(FALSE);

	// Lock Wafer Table Joystick
	LockWaferTableJoystick(TRUE);
	LockPRMouseJoystick(TRUE);

	if (m_bMS60EjElevator)
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}

	//If learn from Home Position
	if (stInfo.bFromZero == TRUE)
	{
		if (stInfo.lPosition == CHANGE_EJ_STANDBY)			//Standby Level
		{
			lStandbyLevel = 0;
		}
		else if (stInfo.lPosition == CHANGE_EJ_CONTACT)		//CONTACT Level
		{
			lContactLevel = 0;
		}
		else	//Up Level
		{
			lUpLevel = 0;
		}
	}
	else
	{
		if (stInfo.lPosition == CHANGE_EJ_STANDBY)			//Standby Level
		{
			switch (stInfo.lMotorType)
			{
				case EJ_PIN_LEARN_LEVEL:
					lStandbyLevel = m_lStandbyLevel_Ej;
					break;
				case EJ_THETA_LEARN_LEVEL:	
					lStandbyLevel = m_lStandby_EJ_Theta;
					break;
				case EJ_CAP_LEARN_LEVEL:	
					lStandbyLevel = m_lStandby_EJ_Cap;
					break;	
				default:
					lStandbyLevel = m_lStandby_EJ_Elevator;
					break;
			}
		}
		else if (stInfo.lPosition == CHANGE_EJ_CONTACT)		//CONTACT Level		//andrewng //2020 0603
		{
			switch (stInfo.lMotorType)
			{
				case EJ_PIN_LEARN_LEVEL:
				default:
					lContactLevel = m_lContactLevel_Ej;
					break;
			}
		}
		else	//Up Level
		{
			switch (stInfo.lMotorType)
			{
				case EJ_PIN_LEARN_LEVEL:
					lUpLevel = m_lEjectLevel_Ej;
					break;
				case EJ_THETA_LEARN_LEVEL:	
					lUpLevel = m_lUpLevel_EJ_Theta;
					break;
				case EJ_CAP_LEARN_LEVEL:	
					lUpLevel = m_lUpLevel_EJ_Cap;
					break;	
				default:
					lUpLevel = m_lUpLevel_EJ_Elevator;
					break;
			}
		}
	}

	if (stInfo.lPosition == CHANGE_EJ_STANDBY)	//Standby Level
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_PIN_LEARN_LEVEL);
				Ej_MoveTo(lStandbyLevel);
				break;
			case EJ_THETA_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_THETA_LEARN_LEVEL);
				EjT_MoveTo(lStandbyLevel);
				break;
			case EJ_CAP_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_CAP_LEARN_LEVEL);
				EjCap_MoveTo(lStandbyLevel);
				break;	
			default:
				DisableEjectorDisplay(stInfo.lPosition, EJ_ELEVATOR_LEARN_LEVEL);
				EjElevator_MoveTo(lStandbyLevel);
				break;
		}
		m_lBPGeneral_TmpC = lStandbyLevel;
	}
	else if (stInfo.lPosition == CHANGE_EJ_CONTACT)		//CONTACT Level
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
			default:
				DisableEjectorDisplay(stInfo.lPosition, EJ_PIN_LEARN_LEVEL);
				Ej_MoveTo(lContactLevel);
				break;
		}
		m_lBPGeneral_TmpC = lContactLevel;
	}
	else	//Up Level
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_PIN_LEARN_LEVEL);
				Ej_MoveTo(lUpLevel);
				break;
			case EJ_THETA_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_THETA_LEARN_LEVEL);
				EjT_MoveTo(lUpLevel);
				break;
			case EJ_CAP_LEARN_LEVEL:
				DisableEjectorDisplay(stInfo.lPosition, EJ_CAP_LEARN_LEVEL);
				EjCap_MoveTo(lUpLevel);
				break;
			default:
				DisableEjectorDisplay(stInfo.lPosition, EJ_ELEVATOR_LEARN_LEVEL);
				EjElevator_MoveTo(lUpLevel);
				break;
		}
		m_lBPGeneral_TmpC = lUpLevel;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_ChangeEjtCollet1(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	if ( m_bChangeCollet2 )
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	if (m_bMS100EjtXY)
	{
		LONG lCollet1OffsetX	= GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY	= GetEjtCollet1OffsetY();
		LONG lWaferCenterX		= (LONG)(*m_psmfSRam)["WaferPr"]["CursorCenter"]["X"];
		LONG lWaferCenterY		= (LONG)(*m_psmfSRam)["WaferPr"]["CursorCenter"]["Y"];

		m_bChangeCollet1 = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_ChangeEjtCollet2(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	if ( m_bChangeCollet1 )
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	if (m_bMS100EjtXY)
	{
		LONG lWaferCenterX		= (*m_psmfSRam)["WaferPr"]["CursorCenter"]["X"];
		LONG lWaferCenterY		= (*m_psmfSRam)["WaferPr"]["CursorCenter"]["Y"];
		LONG lCollet2OffsetX	= GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY	= GetEjtCollet2OffsetY();

		m_bChangeCollet2 = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_Ejt_Z_Move_Up(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	LONG lStep = 0;
	svMsg.GetMsg(sizeof(LONG), &lStep);

	GetEncoderValue();
	if (m_lEnc_T > (m_lPickPos_T - 5000))
	{
		if ( (lStep > 0) && (lStep <= 500) )
		{
			//Z_MoveTo(m_lEnc_Z + lStep);
			m_lBPGeneral_BHZ1 = m_lBPGeneral_BHZ1 + lStep;
			Z_MoveTo(m_lBPGeneral_BHZ1);
		}
	}
	else
	{
		if ( (lStep > 0) && (lStep <= 500) )
		{
			//Z_MoveTo(m_lEnc_Z + lStep);
			m_lBPGeneral_BHZ2 = m_lBPGeneral_BHZ2 + lStep;
			Z2_MoveTo(m_lBPGeneral_BHZ2);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_Ejt_Z_Move_Down(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	LONG lStep = 0;
	svMsg.GetMsg(sizeof(LONG), &lStep);

	GetEncoderValue();
	if (m_lEnc_T > (m_lPickPos_T - 5000))
	{
		if ( (lStep > 0) && (lStep <= 500) )
		{
			m_lBPGeneral_BHZ1 = m_lBPGeneral_BHZ1 - lStep;
			//Z_MoveTo(m_lEnc_Z - lStep);
			Z_MoveTo(m_lBPGeneral_BHZ1);
		}
	}
	else
	{
		if ( (lStep > 0) && (lStep <= 500) )
		{
			m_lBPGeneral_BHZ2 = m_lBPGeneral_BHZ2 - lStep;
			//Z_MoveTo(m_lEnc_Z - lStep);
			Z2_MoveTo(m_lBPGeneral_BHZ2);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_EjtCollet_Move_Up(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if ( m_bChangeCollet1 )
	{
		m_lArm1Pos_EjY -= stInfo.lStep;
	}
	else if ( m_bChangeCollet2 )
	{
		m_lArm2Pos_EjY -= stInfo.lStep;
	}
	else 
	{
		AfxMessageBox("ERROR!", MB_SYSTEMMODAL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_EjtCollet_Move_Down(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if ( m_bChangeCollet1 )
	{
		m_lArm1Pos_EjY += stInfo.lStep;
	}
	else if ( m_bChangeCollet2 )
	{
		m_lArm2Pos_EjY += stInfo.lStep;
	}
	else 
	{
		AfxMessageBox("ERROR!", MB_SYSTEMMODAL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_EjtCollet_Move_Left(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if ( m_bChangeCollet1 )
	{
		m_lArm1Pos_EjX -= stInfo.lStep;
	}
	else if ( m_bChangeCollet2 )
	{
		m_lArm2Pos_EjX -= stInfo.lStep;
	}
	else 
	{
		AfxMessageBox("ERROR!", MB_SYSTEMMODAL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_EjtCollet_Move_Right(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if ( m_bChangeCollet1 )
	{
		m_lArm1Pos_EjX += stInfo.lStep;
	}
	else if ( m_bChangeCollet2 )
	{
		m_lArm2Pos_EjX += stInfo.lStep;
	}
	else 
	{
		AfxMessageBox("ERROR!", MB_SYSTEMMODAL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_ChangeEjectorX(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	if (m_bMS100EjtXY)
	{
		m_bChangeEjtX = TRUE;
		EjX_MoveTo(m_lCDiePos_EjX);
		EjY_MoveTo(m_lCDiePos_EjY);
		Ej_MoveTo(m_lStandbyLevel_Ej);		//v4.43T6
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_ChangeEjectorY(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	if (m_bMS100EjtXY)
	{
		m_bChangeEjtY = TRUE;
		EjX_MoveTo(m_lCDiePos_EjX);
		EjY_MoveTo(m_lCDiePos_EjY);
		Ej_MoveTo(m_lStandbyLevel_Ej);		//v4.43T6
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_KeyInNVCBinEjectorPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	typedef struct 
	{
		LONG	lMotorType;	//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;	//Up level or Standby Level	
		LONG	lStep;
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	switch (stInfo.lMotorType)
	{
		case EJ_PIN_LEARN_LEVEL:
			if (stInfo.lPosition == CHANGE_EJ_STANDBY)
			{
				m_lStandby_Bin_EJ_Pin = stInfo.lStep;
			}
			else	//Up Level
			{
				m_lUpLevel_Bin_EJ_Pin = stInfo.lStep;	
			}
			BinEj_MoveTo(stInfo.lStep);
			break;
		case EJ_THETA_LEARN_LEVEL:
			if (stInfo.lPosition == CHANGE_EJ_STANDBY)
			{
				m_lStandby_Bin_EJ_Theta = stInfo.lStep;
			}
			else	//Up Level
			{
				m_lUpLevel_Bin_EJ_Theta = stInfo.lStep;	
			}
			BinEjT_MoveTo(stInfo.lStep);
			break;
		case EJ_CAP_LEARN_LEVEL:
			if (stInfo.lPosition == CHANGE_EJ_STANDBY)
			{
				m_lStandby_Bin_EJ_Cap = stInfo.lStep;
			}
			else	//Up Level
			{
				m_lUpLevel_Bin_EJ_Cap = stInfo.lStep;	
			}
			BinEjCap_MoveTo(stInfo.lStep);
			break;	
		default:
			if (stInfo.lPosition == CHANGE_EJ_STANDBY)
			{
				m_lStandby_Bin_EJ_Elevator = stInfo.lStep;
			}
			else	//Up Level
			{
				m_lUpLevel_Bin_EJ_Elevator = stInfo.lStep;	
			}
			BinEjElevator_MoveTo(stInfo.lStep);
			break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_KeyInNVCEjectorPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	typedef struct 
	{
		LONG	lMotorType;	//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;	//Up level or Standby Level	
		LONG	lStep;
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	switch (stInfo.lMotorType)
	{
		case EJ_PIN_LEARN_LEVEL:
			if (stInfo.lPosition == CHANGE_EJ_STANDBY)
			{
				m_lBPGeneral_A = stInfo.lStep;
			}
			else if (stInfo.lPosition == CHANGE_EJ_CONTACT)		//andrewng //2020-0603
			{
				m_lBPGeneral_B = stInfo.lStep;
			}
			else	//Up Level
			{
				m_lBPGeneral_9 = stInfo.lStep;	
			}
			Ej_MoveTo(stInfo.lStep);
			break;
		case EJ_THETA_LEARN_LEVEL:
			if (stInfo.lPosition == CHANGE_EJ_STANDBY)
			{
				m_lStandby_EJ_Theta = stInfo.lStep;
			}
			else	//Up Level
			{
				m_lUpLevel_EJ_Theta = stInfo.lStep;	
			}
			EjT_MoveTo(stInfo.lStep);
			break;
		case EJ_CAP_LEARN_LEVEL:
			if (stInfo.lPosition == CHANGE_EJ_STANDBY)
			{
				m_lStandby_EJ_Cap = stInfo.lStep;
			}
			else	//Up Level
			{
				m_lUpLevel_EJ_Cap = stInfo.lStep;	
			}
			EjCap_MoveTo(stInfo.lStep);
			break;	
		default:
			if (stInfo.lPosition == CHANGE_EJ_STANDBY)
			{
				m_lStandby_EJ_Elevator = stInfo.lStep;
			}
			else	//Up Level
			{
				m_lUpLevel_EJ_Elevator = stInfo.lStep;	
			}
			EjElevator_MoveTo(stInfo.lStep);
			break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_KeyInEjectorPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	if (m_bChangeEjtX)
	{
		EjX_MoveTo(stInfo.lStep);
	}
	else if (m_bChangeEjtY)
	{
		EjY_MoveTo(stInfo.lStep);
	}
	else
	{
		switch (stInfo.lPosition)
		{
			case 1:		//Standby
				m_lBPGeneral_A = stInfo.lStep;
				Ej_MoveTo(m_lBPGeneral_A);
				break;

			default:	//Up Level
				m_lBPGeneral_9 = stInfo.lStep;
				Ej_MoveTo(m_lBPGeneral_9);
				break;
		}
	}

	return TRUE;
}

LONG CBondHead::BH_MoveEjectorStandbyPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	if (m_bDisableBH)
	{
		return TRUE;
	}

	Ej_MoveTo(m_lBPGeneral_A);
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_MoveNVCBinEjectorPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0, lMinPos = 0, lCurrentPos = 0, lMoveStep = 0;
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lMotorType;	//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;	//Up level or Standby Level		
		LONG	lStep;
		BOOL	bIsNegativeMove;
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	lMoveStep = stInfo.lStep;
	if (stInfo.bIsNegativeMove)
	{
		lMoveStep = lMoveStep * -1;
	}

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	if (stInfo.lPosition == CHANGE_EJ_STANDBY)
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				lCurrentPos = m_lStandby_Bin_EJ_Pin + lMoveStep;
				break;
			case EJ_THETA_LEARN_LEVEL:
				lCurrentPos = m_lStandby_Bin_EJ_Theta + lMoveStep;
				break;
			case EJ_CAP_LEARN_LEVEL:
				lCurrentPos = m_lStandby_Bin_EJ_Cap + lMoveStep;
				break;	
			default:
				lCurrentPos = m_lStandby_Bin_EJ_Elevator + lMoveStep;
				break;
		}
	}
	else
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				lCurrentPos = m_lUpLevel_Bin_EJ_Pin + lMoveStep;
				break;
			case EJ_THETA_LEARN_LEVEL:
				lCurrentPos = m_lUpLevel_Bin_EJ_Theta + lMoveStep;
				break;
			case EJ_CAP_LEARN_LEVEL:
				lCurrentPos = m_lUpLevel_Bin_EJ_Cap + lMoveStep;
				break;	
			default:
				lCurrentPos = m_lUpLevel_Bin_EJ_Elevator + lMoveStep;
				break;
		}
	}

	switch (stInfo.lMotorType)
	{
		case EJ_PIN_LEARN_LEVEL:
			lMinPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
			if (lCurrentPos >= lMinPos && lCurrentPos <= lMaxPos)
			{
				BinEj_MoveTo(lCurrentPos);

				if (stInfo.lPosition == CHANGE_EJ_STANDBY)
				{
					m_lStandby_Bin_EJ_Pin = lCurrentPos;	
				}
				else
				{
					m_lUpLevel_Bin_EJ_Pin = lCurrentPos;	
				}
			}
			break;
		case EJ_THETA_LEARN_LEVEL:
			lMinPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR_T, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR_T, MS896A_CFG_CH_MAX_DISTANCE);								
			if (lCurrentPos >= lMinPos && lCurrentPos <= lMaxPos)
			{
				BinEjT_MoveTo(lCurrentPos);

				if (stInfo.lPosition == CHANGE_EJ_STANDBY)
				{
					m_lStandby_Bin_EJ_Theta = lCurrentPos;	
				}
				else
				{
					m_lUpLevel_Bin_EJ_Theta = lCurrentPos;	
				}
			}
			break;
		case EJ_CAP_LEARN_LEVEL:
			lMinPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR_CAP, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR_CAP, MS896A_CFG_CH_MAX_DISTANCE);								
			if (lCurrentPos >= lMinPos && lCurrentPos <= lMaxPos)
			{
				BinEjCap_MoveTo(lCurrentPos);

				if (stInfo.lPosition == CHANGE_EJ_STANDBY)
				{
					m_lStandby_Bin_EJ_Cap = lCurrentPos;	
				}
				else
				{
					m_lUpLevel_Bin_EJ_Cap = lCurrentPos;	
				}
			}
			break;	
		default:
			lMinPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
			if (lCurrentPos >= lMinPos && lCurrentPos <= lMaxPos)
			{
				BinEjElevator_MoveTo(lCurrentPos);

				if (stInfo.lPosition == CHANGE_EJ_STANDBY)
				{
					m_lStandby_Bin_EJ_Elevator = lCurrentPos;	
				}
				else
				{
					m_lUpLevel_Bin_EJ_Elevator = lCurrentPos;	
				}
			}
			break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}



LONG CBondHead::BH_MoveNVCEjectorPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0, lMinPos = 0, lCurrentPos = 0, lMoveStep = 0;
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lMotorType;	//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;	//Up level or Standby Level		
		LONG	lStep;
		BOOL	bIsNegativeMove;
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	lMoveStep = stInfo.lStep;
	if (stInfo.bIsNegativeMove)
	{
		lMoveStep = lMoveStep * -1;
	}

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	if (stInfo.lPosition == CHANGE_EJ_STANDBY)
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				lCurrentPos = m_lBPGeneral_A + lMoveStep;
				break;
			case EJ_THETA_LEARN_LEVEL:
				lCurrentPos = m_lStandby_EJ_Theta + lMoveStep;
				break;
			case EJ_CAP_LEARN_LEVEL:
				lCurrentPos = m_lStandby_EJ_Cap + lMoveStep;
				break;	
			default:
				lCurrentPos = m_lStandby_EJ_Elevator + lMoveStep;
				break;
		}
	}
	else if (stInfo.lPosition == CHANGE_EJ_CONTACT)		//andrewng //2020-0603
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
			default:
				lCurrentPos = m_lBPGeneral_B + lMoveStep;
				break;
		}
	}
	else
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				lCurrentPos = m_lBPGeneral_9 + lMoveStep;
				break;
			case EJ_THETA_LEARN_LEVEL:
				lCurrentPos = m_lUpLevel_EJ_Theta + lMoveStep;
				break;
			case EJ_CAP_LEARN_LEVEL:
				lCurrentPos = m_lUpLevel_EJ_Cap + lMoveStep;
				break;	
			default:
				lCurrentPos = m_lUpLevel_EJ_Elevator + lMoveStep;
				break;
		}
	}

	switch (stInfo.lMotorType)
	{
		case EJ_PIN_LEARN_LEVEL:
			lMinPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
			if (lCurrentPos >= lMinPos && lCurrentPos <= lMaxPos)
			{
				Ej_MoveTo(lCurrentPos);

				if (stInfo.lPosition == CHANGE_EJ_STANDBY)
				{
					m_lBPGeneral_A = lCurrentPos;	
				}
				else if (stInfo.lPosition == CHANGE_EJ_CONTACT)		//andrewng //2020-0603
				{
					m_lBPGeneral_B = lCurrentPos;	
				}
				else
				{
					m_lBPGeneral_9 = lCurrentPos;	
				}
			}
			break;
		case EJ_THETA_LEARN_LEVEL:
			lMinPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR_T, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR_T, MS896A_CFG_CH_MAX_DISTANCE);								
			if (lCurrentPos >= lMinPos && lCurrentPos <= lMaxPos)
			{
				EjT_MoveTo(lCurrentPos);

				if (stInfo.lPosition == CHANGE_EJ_STANDBY)
				{
					m_lStandby_EJ_Theta = lCurrentPos;	
				}
				else
				{
					m_lUpLevel_EJ_Theta = lCurrentPos;	
				}
			}
			break;
		case EJ_CAP_LEARN_LEVEL:
			lMinPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR_CAP, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR_CAP, MS896A_CFG_CH_MAX_DISTANCE);								
			if (lCurrentPos >= lMinPos && lCurrentPos <= lMaxPos)
			{
				EjCap_MoveTo(lCurrentPos);

				if (stInfo.lPosition == CHANGE_EJ_STANDBY)
				{
					m_lStandby_EJ_Cap = lCurrentPos;	
				}
				else
				{
					m_lUpLevel_EJ_Cap = lCurrentPos;	
				}
			}
			break;	
		default:
			lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
			if (lCurrentPos >= lMinPos && lCurrentPos <= lMaxPos)
			{
				EjElevator_MoveTo(lCurrentPos);

				if (stInfo.lPosition == CHANGE_EJ_STANDBY)
				{
					m_lStandby_EJ_Elevator = lCurrentPos;	
				}
				else
				{
					m_lUpLevel_EJ_Elevator = lCurrentPos;	
				}
			}
			break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_MovePosEjectorPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	if (m_bChangeEjtX || m_bChangeEjtY)
	{
		GetEncoderValue();
		EjX_MoveTo(m_lEnc_EjX + stInfo.lStep);
	}
	else
	{
		switch (stInfo.lPosition)
		{
			case 1:		
				lCurrentPos = m_lBPGeneral_A + stInfo.lStep;	
				break;	//Standby
			default:	
				lCurrentPos = m_lBPGeneral_9 + stInfo.lStep;	
				break;	//Up Level
		}

		//Check limit range
		lMinPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			Ej_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 1:		
					m_lBPGeneral_A = lCurrentPos;	
					break;
				default:	
					m_lBPGeneral_9 = lCurrentPos;	
					break;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CBondHead::BH_MoveNegEjectorPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//v4.42T3	//v4.51A17
	if (m_bChangeEjtX || m_bChangeEjtY)
	{
		GetEncoderValue();
		EjX_MoveTo(m_lEnc_EjX - stInfo.lStep);
	}
	else
	{
		switch (stInfo.lPosition)
		{
			case 1:		
				lCurrentPos = m_lBPGeneral_A - stInfo.lStep;	
				break;	//Standby
			default:	
				lCurrentPos = m_lBPGeneral_9 - stInfo.lStep;	
				break;	//Up Level
		}

		//Check limit range
		lMinPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			Ej_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 1:		
					m_lBPGeneral_A = lCurrentPos;	
					break;
				default:	
					m_lBPGeneral_9 = lCurrentPos;	
					break;
			}
		}
	}

	return TRUE;
}

LONG CBondHead::BH_MovePosEjectorPositionY(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//v4.42T3	//v4.51A17
	if (m_bChangeEjtX || m_bChangeEjtY)
	{
		GetEncoderValue();
		EjY_MoveTo(m_lEnc_EjY + stInfo.lStep);
	}
	else
	{
		switch (stInfo.lPosition)
		{
			case 1:		
				lCurrentPos = m_lBPGeneral_A + stInfo.lStep;	
				break;	//Standby
			default:	
				lCurrentPos = m_lBPGeneral_9 + stInfo.lStep;	
				break;	//Up Level
		}

		//Check limit range
		lMinPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			Ej_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 1:		
					m_lBPGeneral_A = lCurrentPos;	
					break;
				default:	
					m_lBPGeneral_9 = lCurrentPos;	
					break;
			}
		}
	}

	return TRUE;
}


LONG CBondHead::BH_MoveNegEjectorPositionY(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//v4.42T3	//v4.51A17
	if (m_bChangeEjtX || m_bChangeEjtY)
	{
		GetEncoderValue();
		EjY_MoveTo(m_lEnc_EjY - stInfo.lStep);
	}
	else
	{
		switch (stInfo.lPosition)
		{
			case 1:		
				lCurrentPos = m_lBPGeneral_A - stInfo.lStep;	
				break;	//Standby
			default:	
				lCurrentPos = m_lBPGeneral_9 - stInfo.lStep;	
				break;	//Up Level
		}

		//Check limit range
		lMinPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			Ej_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 1:		
					m_lBPGeneral_A = lCurrentPos;	
					break;
				default:	
					m_lBPGeneral_9 = lCurrentPos;	
					break;
			}
		}
	}

	return TRUE;
}

LONG CBondHead::BH_SyncArmColletVariable(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	//v4.52A14
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	if (!m_bMS100EjtXY && !bColletOffsetWoEjtXY)
	{		
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
/*
	typedef struct 
	{
		LONG	l_Arm1X;
		LONG	l_Arm1Y;
		LONG	l_Arm2X;
		LONG	l_Arm2Y;
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);
*/
	BOOL bUpdateBHZLevel = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUpdateBHZLevel);

	LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
	LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
	LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
	LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

	if (bColletOffsetWoEjtXY)	//v4.52A14
	{
		m_lArm1Pos_EjX = lCollet1OffsetX;
		m_lArm1Pos_EjY = lCollet1OffsetY;
		m_lArm2Pos_EjX = lCollet2OffsetX;
		m_lArm2Pos_EjY = lCollet2OffsetY;
	}
	else
	{
		if (m_bEnableMS100EjtXY)
		{
			m_lArm1Pos_EjX = m_lCDiePos_EjX + lCollet1OffsetX;
			m_lArm1Pos_EjY = m_lCDiePos_EjY + lCollet1OffsetY;
			m_lArm2Pos_EjX = m_lCDiePos_EjX + lCollet2OffsetX;
			m_lArm2Pos_EjY = m_lCDiePos_EjY + lCollet2OffsetY;

			if (CMS896AStn::m_bAutoChangeCollet)	//v4.50A30
			{
				m_lArm1Pos_EjX = m_lArm1Pos_EjX;// + m_lAGCCollet1OffsetX;
				m_lArm1Pos_EjY = m_lArm1Pos_EjY;// + m_lAGCCollet1OffsetY;
				m_lArm2Pos_EjX = m_lArm2Pos_EjX;// + m_lAGCCollet2OffsetX;
				m_lArm2Pos_EjY = m_lArm2Pos_EjY;// + m_lAGCCollet2OffsetY;
			}

			if (bUpdateBHZLevel)
			{
				m_lBPGeneral_BHZ1 = m_lPickLevel_Z  + 500;
				m_lBPGeneral_BHZ2 = m_lPickLevel_Z2 + 500;
			}
		}
		else
		{
			m_lArm1Pos_EjX = m_lCDiePos_EjX;
			m_lArm1Pos_EjY = m_lCDiePos_EjY;
			m_lArm2Pos_EjX = m_lCDiePos_EjX;
			m_lArm2Pos_EjY = m_lCDiePos_EjY;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;	
}

LONG CBondHead::BH_AutoLearnEjUpLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

#ifndef NU_MOTION
	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
#endif

	StartLoadingAlert();

	Z_MoveToHome();
	Z2_MoveToHome();
	Ej_Home();
	UserSearchDieToPrCenter();
	T_MoveTo(m_lPickPos_T);
	Z_MoveTo(m_lPickLevel_Z + 200);

	LONG lEnc = 0;

	if (AutoLearnEjUpLevelWithAitFlowSnr(lEnc, 200) == FALSE)
	{
		CloseLoadingAlert();

	//	HmiMessage("ERROR: Ej auto-search UP-level fails; operation is aborted.");	//	reduce surplus alarm
		SetPickVacuum(FALSE);
		Z_Home();
		Ej_Home();
		T_MoveTo(m_lPrePickPos_T);

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);	
		return 1;
	}

	Z_MoveToHome();
	Z2_MoveToHome();
	Ej_Home();
	T_MoveTo(m_lPrePickPos_T);
	CloseLoadingAlert();
	
	//m_lEjectLevel_Ej = lEnc;
	//m_lBPGeneral_9	= lEnc;
	//m_lBPGeneral_A	= ?;
	SaveBhData();

	CString szMsg;
	szMsg.Format("Ej Auto Search UP level = %ld", lEnc);
	HmiMessage(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;	
}


//v3.94T4
LONG CBondHead::BH_AutoLearnEjectorPosn(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	LONG lPosition = 0;
	svMsg.GetMsg(sizeof(LONG), &lPosition);		//1 = STDBy Level, else UP level

	//v4.08
	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

HmiMessage("Start ...");

	Z_MoveTo(0);
#ifdef NU_MOTION
	Z2_MoveTo(0);
#endif
	
	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
	//{
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
#ifdef NU_MOTION
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
#endif
	//}

	T_MoveTo(m_lPickPos_T);
	
	/*******/ 
	StartLoadingAlert(); /******/
	LONG lMaxEjLvl = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								

	//Z_MoveTo(m_lPickLevel_Z);
	LONG lEnc = 0;
	SetPickVacuum(TRUE);
	Sleep(500);

	if (AutoLearnBHZ1BondLevel(lEnc, FALSE) == FALSE)
	{
		/*******/ CloseLoadingAlert(); /******/

		HmiMessage("ERROR: BH1 Z-level auto-search fails; operation is aborted.");
		SetPickVacuum(FALSE);
		Z_Home();
		T_MoveTo(m_lPrePickPos_T);

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);	
		return 1;
	}

	GetEncoderValue();
	CString szMsg;
	szMsg.Format("BH Z at Enc = %ld, .....", m_lEnc_Z);
	HmiMessage(szMsg);

	if (!Z_IsPowerOn())
	{
		/*******/ CloseLoadingAlert(); /******/

		HmiMessage("ERROR: BH1 Z power off; operation is aborted.");
		SetPickVacuum(FALSE);
		Z_Home();
		T_MoveTo(m_lPrePickPos_T);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);	
		return 1;
	}

	//Sleep(100);
	//SetPickVacuum(FALSE);

	Sleep(400);
	if (Ej_IsPowerOn())
	{
		Ej_MoveTo(0);
	}
	else
	{
		Ej_Home();
	}
	Sleep(500);


	INT nLoopCount		= 10;
	INT nMxLoopCount	= 20;

	while (nLoopCount <= nMxLoopCount)
	{
		/////////////////////////////////////////////////
		/////////////////////////////////////////////////
		LogAxisPerformance2(BH_AXIS_Z, BH_AXIS_EJ, &m_stBHAxis_Z, &m_stBHAxis_Ej, TRUE);

		SetEjectorVacuum(TRUE);
		Sleep(125);

		CMS896AStn::MotionUpdateSearchProfile(BH_AXIS_EJ, "spfEjectorSlowMove", 2, nLoopCount * 100, &m_stBHAxis_Ej);	//v4.48A3
		CMS896AStn::MotionSearch(BH_AXIS_EJ, 1, SFM_WAIT, &m_stBHAxis_Ej, "spfEjectorSlowMove");
		Sleep(125);

		GetEncoderValue();
		LONG lZDiff = m_lPickLevel_Z - m_lEnc_Z;

		LogAxisPerformance2(BH_AXIS_Z, BH_AXIS_EJ, &m_stBHAxis_Z, &m_stBHAxis_Ej, FALSE, TRUE, TRUE);
		/////////////////////////////////////////////////
		/////////////////////////////////////////////////


		CString szTemp;
		szTemp.Format("#%d: EJ stop at ENC = %d (%d)",  nLoopCount, m_lEnc_Ej, m_lEnc_Ej - m_lStandbyLevel_Ej);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
		CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);

		if (Ej_IsPowerOn())
		{
			Ej_MoveTo(0);
		}
		else
		{
			Ej_Home();
		}
		SetEjectorVacuum(FALSE);


		LONG lEjContactLevel = 0;
		CStdioFile oFile;
		CString szLine;
		CString szCount, szBHZDAC, szEjEnc;
		INT nIndex;
		SHORT nBHZDAC, nBHZInitDAC = 0;
		BOOL bFound = FALSE;

		INT nDacCount = 0;

		if (!oFile.Open(gszUSER_DIRECTORY + "\\Diagnostics\\NuMotionLog_Sp.csv",
						CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText))
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		oFile.SeekToBegin();
		while (oFile.ReadString(szLine))
		{
			nIndex	= szLine.Find(",");
			if( nIndex!=-1 )
				szCount = szLine.Left(nIndex);

			szLine = szLine.Mid(nIndex + 1);
			nIndex = szLine.Find(",");
			if( nIndex!=-1 )
				szBHZDAC = szLine.Left(nIndex);
			nBHZDAC = (SHORT) atol(szBHZDAC);

			if (nBHZInitDAC == 0)
			{
				nBHZInitDAC = nBHZDAC;
				//CString szTemp;
				//szTemp.Format("Init DAC found = %d", nBHZInitDAC);
				//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
				continue;
			}

			if (nBHZDAC < (nBHZInitDAC - 1000))
			{
				if (nDacCount == 0)
				{
					szLine = szLine.Mid(nIndex + 1);
					//szEjEnc = szLine.Left(nIndex);
					lEjContactLevel = atol(szLine);
				}

				nDacCount++;

				if (nDacCount > 25)
				{
					szLine		= szLine.Mid(nIndex + 1);
					LONG lEjEnc = atol(szLine);
					LONG lEjTop = nLoopCount * 200;

					//v3.99T1
					if (lEjEnc < (lEjTop - 20))
					{
						bFound = TRUE;
						break;
					}
					else
					{
						nDacCount = 0;
					}
				}
			}
			else
			{
				nDacCount = 0;
			}

		}
		oFile.Close();

		if (bFound)
		{
			/*******/ CloseLoadingAlert(); /******/

			CString szMsg;
			szMsg.Format("Auto-Learn Ej Contact level done at enc = %d (Init PICK DAC = %d)", lEjContactLevel, nBHZInitDAC);
			//HmiMessage(szMsg);

			//GetEncoderValue();

			m_lContactLevel_Ej	= lEjContactLevel;
		
			//if (lPosition == 1)
			m_lBPGeneral_A		= lEjContactLevel - m_lEjCTLevelOffset;		//StandBy Level
			m_lBPGeneral_9		= m_lBPGeneral_A + m_lWafDieHeight;			//UP level
			SaveBhData();

			//v4.01
			CStdioFile oLogFile;
			
			if (oLogFile.Open(gszUSER_DIRECTORY + "\\Diagnostics\\auto_learn_ej.txt",
							  CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText))
			{
				CString szLine;
				CTime ts	= CTime::GetCurrentTime();
				szLine		= ts.Format("%Y-%m-%d %H:%M:%S");
				oLogFile.WriteString(szLine + "\n\n");

				szLine.Format("BHZ auto-learn Pick level = %d (step)\n", m_lEnc_Z);
				oLogFile.WriteString(szLine);

				szLine.Format("BHZ Pick DAC = %d\n", nBHZInitDAC);
				oLogFile.WriteString(szLine);

				szLine.Format("EJ auto-learn Peak level = %d (step)\n", m_lEnc_Ej);
				oLogFile.WriteString(szLine);

				szLine.Format("EJ Contact level = %d (step)\n", lEjContactLevel);
				oLogFile.WriteString(szLine);

				szLine.Format("EJ Contact level offset = %d (step)\n", m_lEjCTLevelOffset);
				oLogFile.WriteString(szLine);

				szLine.Format("Die Height = %d (step)\n", m_lWafDieHeight);
				oLogFile.WriteString(szLine);

				oLogFile.Close();
			}


			break;
		}
		else
		{
			nLoopCount++;
			if (nLoopCount >= nMxLoopCount)
			{
				/*******/ CloseLoadingAlert(); /******/
				HmiMessage("ERROR: EJ CT level not found; please check BHZ PICK-level and die height value!");
				break;
			}
			if ((lMaxEjLvl > 0) && (lMaxEjLvl < nLoopCount*100))
			{
				/*******/ CloseLoadingAlert(); /******/
				HmiMessage("ERROR: EJ CT level not found; please check BHZ PICK-level and die height value!");
				break;
			}
		}
	}
		

	Ej_Home();
	Z_MoveToHome();
	T_MoveTo(m_lPrePickPos_T);
	
	m_bRefreshScreen = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

}


LONG CBondHead::BH_ConfirmEjectorSetup(IPC_CServiceMessage &svMsg)
{
	CString szLog;
	BOOL bReturn = TRUE;

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();

	LockWaferTableJoystick(FALSE);
	LockPRMouseJoystick(FALSE);
	SetWaferTableJoystick(FALSE);

	Ej_MoveTo(0);
	if (m_bMS60EjElevator)
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}
	Z_MoveTo(0);
#ifdef NU_MOTION
	Z2_MoveTo(0);
#endif

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
	{
		Sleep(100);
		Z_Move(pApp->GetBHZ1HomeOffset());
	}
#ifdef NU_MOTION
	if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
	{
		Sleep(100);
		Z2_Move(pApp->GetBHZ2HomeOffset());
	}
#endif


	if (IsESMachine() == FALSE)
	{
		INT nCount = 0;
		BOOL bHomeT = TRUE;
		if (m_bIsArm2Exist == TRUE)
		{
			while (IsCoverOpen())
			{
				CloseLoadingAlert();
				SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
				SetErrorMessage("Machine cover open detected in CONFIRM EJ setup");

				nCount++;
				if (nCount >= 3)
				{
					nCount = 0;
					LONG lHmiStatus = HmiMessage("Cover sensor still open in CONFIRM EJ setup; continue to HOME T motor?", 
												 "Ejector Setup", glHMI_MBX_CONTINUESTOP | 0x80000000);
					if (lHmiStatus == glHMI_CONTINUE)
					{
						bHomeT = FALSE;
						break;
					}
				}
			}
		}

		if (bHomeT)
		{
			T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
			T_SMoveTo(m_lPrePickPos_T);
			T_Profile(NORMAL_PROF);
		}
	}

	CloseLoadingAlert();

	Sleep(100);

	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	if (m_bChangeEjtX)
	{
		GetEncoderValue();
		if (m_bHome_EjX && m_bIsPowerOn_EjX)
		{
			m_lCDiePos_EjX = m_lEnc_EjX;
		}
		if (m_bHome_EjY && m_bIsPowerOn_EjY)
		{
			m_lCDiePos_EjY = m_lEnc_EjY;
		}

		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		if (bColletOffsetWoEjtXY)
		{
			m_lArm1Pos_EjX = lCollet1OffsetX;
			m_lArm1Pos_EjY = lCollet1OffsetY;
			m_lArm2Pos_EjX = lCollet2OffsetX;
			m_lArm2Pos_EjY = lCollet2OffsetY;
		}
		else
		{
			m_lArm1Pos_EjX = m_lCDiePos_EjX + lCollet1OffsetX;
			m_lArm1Pos_EjY = m_lCDiePos_EjY + lCollet1OffsetY;
			m_lArm2Pos_EjX = m_lCDiePos_EjX + lCollet2OffsetX;
			m_lArm2Pos_EjY = m_lCDiePos_EjY + lCollet2OffsetY;
		}

	}
	else if (m_bChangeEjtY)
	{
		GetEncoderValue();
		if (m_bHome_EjX && m_bIsPowerOn_EjX)
		{
			m_lCDiePos_EjX = m_lEnc_EjX;
		}
		if (m_bHome_EjY && m_bIsPowerOn_EjY)
		{
			m_lCDiePos_EjY = m_lEnc_EjY;
		}

		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		if (bColletOffsetWoEjtXY)
		{
			m_lArm1Pos_EjX = lCollet1OffsetX;
			m_lArm1Pos_EjY = lCollet1OffsetY;
			m_lArm2Pos_EjX = lCollet2OffsetX;
			m_lArm2Pos_EjY = lCollet2OffsetY;
		}
		else
		{
			m_lArm1Pos_EjX = m_lCDiePos_EjX + lCollet1OffsetX;
			m_lArm1Pos_EjY = m_lCDiePos_EjY + lCollet1OffsetY;
			m_lArm2Pos_EjX = m_lCDiePos_EjX + lCollet2OffsetX;
			m_lArm2Pos_EjY = m_lCDiePos_EjY + lCollet2OffsetY;
		}
	}
	else if (m_bChangeCollet1)
	{
		GetEncoderValue();
		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		SaveEjtCollet1Offset(m_lArm1Pos_EjX - m_lCDiePos_EjX, m_lArm1Pos_EjY - m_lCDiePos_EjY, m_dEjtXYRes);
	}
	else if (m_bChangeCollet2)
	{
		GetEncoderValue();
		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		SaveEjtCollet2Offset(m_lArm2Pos_EjX - m_lCDiePos_EjX, m_lArm2Pos_EjY - m_lCDiePos_EjY, m_dEjtXYRes);
	}
	else
	{
		if (m_lEjectLevel_Ej != m_lBPGeneral_9)
		{
			szLog.Format("EJ UP Level updated from %ld to %ld", m_lEjectLevel_Ej, m_lBPGeneral_9);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			LogItems(EJECTOR_UP_LEVEL);
		}

		if (m_lStandbyLevel_Ej != m_lBPGeneral_A)
		{
			szLog.Format("EJ Standby Level updated from %ld to %ld", m_lStandbyLevel_Ej, m_lBPGeneral_A);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			LogItems(EJECTOR_STANDBY_LEVEL);
		}
	}

#ifdef NU_MOTION
	if (m_bEnableSyncMotionViaEjCmd == TRUE)
	{
		if( m_lBPGeneral_A + m_lSyncTriggerValue >= m_lBPGeneral_9 )
		{
			m_lBPGeneral_9		= m_lEjectLevel_Ej;
			m_lBPGeneral_A		= m_lStandbyLevel_Ej;
			CString szMsg;
			szMsg.Format("Up Level(%d) cannot smaller than Standby Level(%d) + Trigger Level(%d)\nPlease Setup Again!"
				, m_lBPGeneral_9, m_lBPGeneral_A, m_lSyncTriggerValue);
			HmiMessage(szMsg, "Sync Move");
		}
	}
#endif
	//Update variable
	m_lEjectLevel_Ej		= m_lBPGeneral_9;
	m_lStandbyLevel_Ej		= m_lBPGeneral_A;
	SaveBhData();

	if ( m_bChangeEjtX	|| m_bChangeEjtY )
	{
		m_bChangeCollet1		= FALSE;
		m_bChangeCollet2		= FALSE;
	}
	m_bChangeEjtX			= FALSE;
	m_bChangeEjtY			= FALSE;


	m_lEjMoveDistance = m_lEjectLevel_Ej - m_lStandbyLevel_Ej;
	SetStatusMessage("Ejector level is updated");

	// 3471
	SetGemValue("EJ_UpLevel", m_lEjectLevel_Ej);
	SetGemValue("EJ_UpLevelWithBHDown", m_lEjectLevel_Ej);
	SetGemValue("EJ_StandByLevel", m_lStandbyLevel_Ej);
	SendEvent(SG_CEID_EJ_POSN, FALSE);			// 7602

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_ConfirmNVCBinEjectorSetup(IPC_CServiceMessage &svMsg)
{
	CString szLog;
	BOOL bReturn = TRUE;
	BOOL bLevelChange = FALSE;

	typedef struct 
	{
		LONG	lMotorType;	//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;	//Up level or Standby Level
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();

	LockWaferTableJoystick(FALSE);
	LockPRMouseJoystick(FALSE);
	SetWaferTableJoystick(FALSE);

	if (m_bMS60EjElevator)
	{
		BinEjElevator_MoveTo(m_lStandby_Bin_EJ_Elevator);
	}

	CloseLoadingAlert();

	Sleep(100);

	if (stInfo.lPosition == CHANGE_EJ_STANDBY)
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				if (m_lStandby_Bin_EJ_Pin != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("BIN EJ PIN Standby Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lStandby_Bin_EJ_Pin);				
				}
				break;
			case EJ_THETA_LEARN_LEVEL:
				if (m_lStandby_Bin_EJ_Theta != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("BIN EJ THETA Standby Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lStandby_Bin_EJ_Theta);				
				}
				break;
			case EJ_CAP_LEARN_LEVEL:
				if (m_lStandby_Bin_EJ_Cap != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("BIN EJ CAP Standby Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lStandby_Bin_EJ_Cap);				
				}
				break;	
			default:
				if (m_lStandby_Bin_EJ_Elevator != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("EJ ELV Standby Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lStandby_Bin_EJ_Elevator);				
				}
				break;
		}

		if (bLevelChange)
		{
			LogItems(EJECTOR_STANDBY_LEVEL);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}
	else
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:		
				if (m_lUpLevel_Bin_EJ_Theta != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("BIN EJ PIN UP Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lUpLevel_Bin_EJ_Pin);
				}
				break;
			case EJ_THETA_LEARN_LEVEL:
				if (m_lUpLevel_Bin_EJ_Theta != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("BIN EJ THETA UP Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lUpLevel_Bin_EJ_Theta);
				}
				break;
			case EJ_CAP_LEARN_LEVEL:
				if (m_lUpLevel_Bin_EJ_Cap != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("BIN EJ CAP UP Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lUpLevel_Bin_EJ_Cap);
				}
				break;	
			default:
				if (m_lUpLevel_Bin_EJ_Elevator != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("BIN EJ ELV UP Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lUpLevel_Bin_EJ_Elevator);
				}
				break;
		}

		if (bLevelChange)
		{
			LogItems(EJECTOR_UP_LEVEL);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}

	//Update variable
	SaveBhData();
	DisableEjectorDisplay(FALSE, 0, TRUE);	//Enable Display

	if (stInfo.lMotorType == EJ_PIN_LEARN_LEVEL)
	{
		m_lEjMoveDistance = m_lEjectLevel_Ej - m_lStandbyLevel_Ej;
		SetStatusMessage("Ejector level is updated");

		// 3471
		SetGemValue("EJ_UpLevel", m_lEjectLevel_Ej);
		SetGemValue("EJ_UpLevelWithBHDown", m_lEjectLevel_Ej);
		SetGemValue("EJ_StandByLevel", m_lStandbyLevel_Ej);
		SendEvent(SG_CEID_EJ_POSN, FALSE);			// 7602
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_ConfirmNVCEjectorSetup(IPC_CServiceMessage &svMsg)
{
	CString szLog;
	BOOL bReturn = TRUE;
	BOOL bLevelChange = FALSE;

	typedef struct 
	{
		LONG	lMotorType;	//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;	//Up level or Standby Level
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();

	LockWaferTableJoystick(FALSE);
	LockPRMouseJoystick(FALSE);
	SetWaferTableJoystick(FALSE);

	if (m_bMS60EjElevator)
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}

	CloseLoadingAlert();

	Sleep(100);

	if (stInfo.lPosition == CHANGE_EJ_STANDBY)
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				if (m_lStandbyLevel_Ej != m_lBPGeneral_A)
				{
					m_lStandbyLevel_Ej = m_lBPGeneral_A;
					bLevelChange = TRUE;
					szLog.Format("EJ PIN Standby Level updated from %ld to %ld", m_lStandbyLevel_Ej, m_lBPGeneral_A);				
				}
				break;
			case EJ_THETA_LEARN_LEVEL:
				if (m_lStandby_EJ_Theta != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("EJ THETA Standby Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lStandby_EJ_Theta);				
				}
				break;
			case EJ_CAP_LEARN_LEVEL:
				if (m_lStandby_EJ_Cap != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("EJ CAP Standby Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lStandby_EJ_Cap);				
				}
				break;	
			default:
				if (m_lStandby_EJ_Elevator != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("EJ ELV Standby Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lStandby_EJ_Elevator);				
				}
				break;
		}

		if (bLevelChange)
		{
			LogItems(EJECTOR_STANDBY_LEVEL);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}
	else if (stInfo.lPosition == CHANGE_EJ_CONTACT)		//andrewng //2020-0603
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				if (m_lContactLevel_Ej != m_lBPGeneral_B)
				{
					szLog.Format("EJ PIN CONTACT Level updated from %ld to %ld", m_lContactLevel_Ej, m_lBPGeneral_B);
					m_lContactLevel_Ej = m_lBPGeneral_B;
					bLevelChange = TRUE;				
				}
				break;
		}

		if (bLevelChange)
		{
			LogItems(EJECTOR_STANDBY_LEVEL);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}
	else
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:		
				if (m_lEjectLevel_Ej != m_lBPGeneral_9)
				{
					m_lEjectLevel_Ej = m_lBPGeneral_9;
					bLevelChange = TRUE;
					szLog.Format("EJ PIN UP Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lEjectLevel_Ej);
				}
				break;
			case EJ_THETA_LEARN_LEVEL:
				if (m_lUpLevel_EJ_Theta != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("EJ THETA UP Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lUpLevel_EJ_Theta);
				}
				break;
			case EJ_CAP_LEARN_LEVEL:
				if (m_lUpLevel_EJ_Cap != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("EJ CAP UP Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lUpLevel_EJ_Cap);
				}
				break;	
			default:
				if (m_lUpLevel_EJ_Elevator != m_lBPGeneral_TmpC)
				{
					bLevelChange = TRUE;
					szLog.Format("EJ ELV UP Level updated from %ld to %ld", m_lBPGeneral_TmpC, m_lUpLevel_EJ_Elevator);
				}
				break;
		}

		if (bLevelChange)
		{
			LogItems(EJECTOR_UP_LEVEL);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}

	//Update variable
	SaveBhData();
	DisableEjectorDisplay(FALSE, 0, TRUE);	//Enable Display

	if (stInfo.lMotorType == EJ_PIN_LEARN_LEVEL)
	{
		m_lEjMoveDistance = m_lEjectLevel_Ej - m_lStandbyLevel_Ej;
		SetStatusMessage("Ejector level is updated");

		// 3471
		SetGemValue("EJ_UpLevel",			m_lEjectLevel_Ej);
		SetGemValue("EJ_UpLevelWithBHDown", m_lEjectLevel_Ej);
		SetGemValue("EJ_StandByLevel",		m_lStandbyLevel_Ej);
		SendEvent(SG_CEID_EJ_POSN,			FALSE);		// 7602
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::BH_CancelEjectorSetup(IPC_CServiceMessage &svMsg)
{
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		return TRUE;
	}

	//v4.08
	if (m_bDisableBH)
	{
		//bReturn = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();	//v3.86

	LockWaferTableJoystick(FALSE);
	LockPRMouseJoystick(FALSE);
	SetWaferTableJoystick(FALSE);

	if (IsESMachine() == FALSE)
	{
		//Move Arm & Head also
		if (m_bIsArm2Exist == TRUE)
		{
			switch (m_lWithBHDown)
			{
				case 1:		//BH2
					Z2_MoveToHome();
					break;

				default:	//BH1
					Z_MoveToHome();
					break;
			}
		}
		else
		{
			Z_MoveToHome();
		}
	}

	Ej_MoveTo(0);
	if (m_bMS60EjElevator)
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}
	Z_MoveTo(0);
#ifdef NU_MOTION
	Z2_MoveTo(0);
#endif

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
	//{
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
#ifdef NU_MOTION
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
#endif
	//}

	if (IsESMachine() == FALSE)
	{
		//v3.96T1
		//Extra protection to protect against BH collision/safety
		INT nCount = 0;
		BOOL bHomeT = TRUE;
		if (m_bIsArm2Exist == TRUE)
		{
			while (IsCoverOpen())
			{
				CloseLoadingAlert();
				SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
				SetErrorMessage("Machine cover open detected in CONFIRM EJ setup");

				nCount++;
				if (nCount >= 3)
				{
					nCount = 0;
					LONG lHmiStatus = HmiMessage("Cover sensor still open in CONFIRM EJ setup; continue to HOME T motor?", 
												 "Ejector Setup", glHMI_MBX_CONTINUESTOP | 0x80000000);
					if (lHmiStatus == glHMI_CONTINUE)
					{
						bHomeT = FALSE;
						break;
					}
				}
			}
		}

		if (bHomeT)
		{
			T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
			T_SMoveTo(m_lPrePickPos_T);
			T_Profile(NORMAL_PROF);
		}
	}

	//v4.28
	if (IsESMachine())
	{
		EjElevator_MoveTo(0);
		Sleep(500);

		IPC_CServiceMessage stMsg;
		BOOL bToEjtOffset = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bToEjtOffset);
		INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "MoveTable1ToEjtOffset", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);	
			}
		}
	}

	CloseLoadingAlert();	//v3.86

	//Restore HMI variable
	m_lBPGeneral_9	= m_lEjectLevel_Ej;
	m_lBPGeneral_A	= m_lStandbyLevel_Ej;
	m_lEjMoveDistance = m_lEjectLevel_Ej - m_lStandbyLevel_Ej;
	CString abc;
	//abc.Format("%d,%d,%d,3",m_lEjMoveDistance,m_lEjectLevel_Ej,m_lStandbyLevel_Ej);
	//AfxMessageBox(abc);
	
	//v4.52A14
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	if (m_bChangeEjtX || m_bChangeEjtY)		//v4.22T3
	{
		EjX_MoveTo(m_lCDiePos_EjX);
		EjY_MoveTo(m_lCDiePos_EjY);
		//v4.42T15
		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		if (bColletOffsetWoEjtXY)	//v4.52A14
		{
			m_lArm1Pos_EjX = lCollet1OffsetX;
			m_lArm1Pos_EjY = lCollet1OffsetY;
			m_lArm2Pos_EjX = lCollet2OffsetX;
			m_lArm2Pos_EjY = lCollet2OffsetY;
		}
		else
		{
			m_lArm1Pos_EjX = m_lCDiePos_EjX + lCollet1OffsetX;
			m_lArm1Pos_EjY = m_lCDiePos_EjY + lCollet1OffsetY;
			m_lArm2Pos_EjX = m_lCDiePos_EjX + lCollet2OffsetX;
			m_lArm2Pos_EjY = m_lCDiePos_EjY + lCollet2OffsetY;
		}
	}
	else if (m_bChangeCollet2 || m_bChangeCollet1)
	{
		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		m_lArm1Pos_EjX = m_lCDiePos_EjX + lCollet1OffsetX;
		m_lArm1Pos_EjY = m_lCDiePos_EjY + lCollet1OffsetY;
		m_lArm2Pos_EjX = m_lCDiePos_EjX + lCollet2OffsetX;
		m_lArm2Pos_EjY = m_lCDiePos_EjY + lCollet2OffsetY;
	}

	if ( m_bChangeEjtX	|| m_bChangeEjtY )
	{
		m_bChangeCollet1		= FALSE;
		m_bChangeCollet2		= FALSE;
	}

	m_bChangeEjtX	= FALSE;
	m_bChangeEjtY	= FALSE;
	//m_bChangeCollet2	= FALSE;
	//m_bChangeCollet1	= FALSE;

	//v4.53A14
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_CancelNVCBinEjectorSetup(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lMotorType;	//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;	//Up level or Standby Level
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();	//v3.86

	LockWaferTableJoystick(FALSE);
	LockPRMouseJoystick(FALSE);
	SetWaferTableJoystick(FALSE);
	
	if (m_bMS60EjElevator)
	{
		BinEjElevator_MoveTo(m_lStandby_Bin_EJ_Elevator);
	}

	CloseLoadingAlert();

	//Restore HMI variable
	if (stInfo.lPosition == CHANGE_EJ_STANDBY)
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				m_lStandby_Bin_EJ_Pin = m_lBPGeneral_TmpC;
				break;
			case EJ_THETA_LEARN_LEVEL:
				m_lStandby_Bin_EJ_Theta = m_lBPGeneral_TmpC;
				break;
			case EJ_CAP_LEARN_LEVEL:
				m_lStandby_Bin_EJ_Cap = m_lBPGeneral_TmpC;
				break;	
			default:
				m_lStandby_Bin_EJ_Elevator = m_lBPGeneral_TmpC;
				break;
		}
	}
	else
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:		
				m_lUpLevel_Bin_EJ_Pin = m_lBPGeneral_TmpC;
				break;
			case EJ_THETA_LEARN_LEVEL:
				m_lUpLevel_Bin_EJ_Theta = m_lBPGeneral_TmpC;
				break;
			case EJ_CAP_LEARN_LEVEL:
				m_lUpLevel_Bin_EJ_Cap = m_lBPGeneral_TmpC;
				break;	
			default:
				m_lUpLevel_Bin_EJ_Elevator = m_lBPGeneral_TmpC;
				break;
		}
	}

	DisableEjectorDisplay(FALSE, 0, TRUE);	//Enable Display

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_CancelNVCEjectorSetup(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lMotorType;	//Ejector Pin, Theta, Cap, Elevator
		LONG	lPosition;	//Up level or Standby Level
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();	//v3.86

	LockWaferTableJoystick(FALSE);
	LockPRMouseJoystick(FALSE);
	SetWaferTableJoystick(FALSE);
	
	if (m_bMS60EjElevator)
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}

	CloseLoadingAlert();

	//Restore HMI variable
	if (stInfo.lPosition == CHANGE_EJ_STANDBY)
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				m_lBPGeneral_A	= m_lStandbyLevel_Ej;
				m_lEjMoveDistance = m_lEjectLevel_Ej - m_lStandbyLevel_Ej;
				break;
			case EJ_THETA_LEARN_LEVEL:
				m_lStandby_EJ_Theta = m_lBPGeneral_TmpC;
				break;
			case EJ_CAP_LEARN_LEVEL:
				m_lStandby_EJ_Cap = m_lBPGeneral_TmpC;
				break;	
			default:
				m_lStandby_EJ_Elevator = m_lBPGeneral_TmpC;
				break;
		}


	}
	else if (stInfo.lPosition == CHANGE_EJ_CONTACT)		//andrewng //2020-0603
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
			default:
				m_lBPGeneral_B	= m_lContactLevel_Ej;
				//m_lEjMoveDistance = m_lEjectLevel_Ej - m_lStandbyLevel_Ej;
				break;
		}
	}
	else
	{
		switch (stInfo.lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:		
				m_lBPGeneral_A	= m_lStandbyLevel_Ej;
				m_lEjMoveDistance = m_lEjectLevel_Ej - m_lStandbyLevel_Ej;
				break;
			case EJ_THETA_LEARN_LEVEL:
				m_lUpLevel_EJ_Theta = m_lBPGeneral_TmpC;
				break;
			case EJ_CAP_LEARN_LEVEL:
				m_lUpLevel_EJ_Cap = m_lBPGeneral_TmpC;
				break;	
			default:
				m_lUpLevel_EJ_Elevator = m_lBPGeneral_TmpC;
				break;
		}
	}

	DisableEjectorDisplay(FALSE, 0, TRUE);	//Enable Display

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_UpdateSwingOffset(IPC_CServiceMessage &svMsg)
{
	m_lSwingLevel_Z	= max(m_lPickLevel_Z, m_lBondLevel_Z) + m_lSwingOffset_Z;
	//if (m_bMS60EjElevator)		//v4.51A6
	//{
	//	m_lSwingLevel_Z = 0;	// - m_lSwingOffset_Z;		//v4.51A8
	//}
	SaveBhData();

	return TRUE;
}

LONG CBondHead::BH_AutoLearnBHZ1BondLevels(IPC_CServiceMessage &svMsg)		//v3.94T5
{
	LONG lEncZ	= 0;
	LONG lEncZ0 = 0;
	LONG lEncZ1 = 0;
	LONG lEncZ2 = 0;
	LONG lEncZ3 = 0;
	LONG lEncZ4 = 0;

	BOOL bReturn = TRUE;
	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	BOOL bResult = TRUE;
	LONG lPosn = 0;

	//For Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (!IsBondArmAbleToMove())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	HmiMessage("Start auto-learn BHZ1 levels ...");

	Z_MoveTo(0);
#ifdef NU_MOTION
	Z2_MoveTo(0);
#endif

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
	//{
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
#ifdef NU_MOTION
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
#endif
	//}

	if( T_SafeMoveToLoop(m_lBondPos_T, "BHZ1 Bond Level")==FALSE )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	for (INT i = 0; i <= 4; i++)	//v4.08
	{
		lPosn = i;
		rReqMsg.InitMessage(sizeof(LONG), &lPosn);
		
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToBinArea4Corners", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult == FALSE)
		{
			Z_Home();
			T_MoveTo(m_lPrePickPos_T);

			CString szErr;
			szErr.Format("ERROR: Auto Learn BHZ1 levels fail BT-Move at #%d corner", i);
			HmiMessage(szErr);

			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		lEncZ = 0;	//v4.49A6

		if (!AutoLearnBHZ1BondLevel(lEncZ))
		{
			Z_Home();
			T_MoveTo(m_lPrePickPos_T);

			CString szErr;
			szErr.Format("ERROR: Auto Learn BHZ1 levels fail at #%d corner", i);
			HmiMessage(szErr);

			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (i == 0)
		{
			lEncZ0 = lEncZ;
		}
		else if (i == 1)
		{
			lEncZ1 = lEncZ;
		}
		else if (i == 2)
		{
			lEncZ2 = lEncZ;
		}
		else if (i == 3)
		{
			lEncZ3 = lEncZ;
		}
		else
		{
			lEncZ4 = lEncZ;
		}

		Z_MoveTo(0);

		//v4.46T20	//Cree
		//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
		//{
			if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
			{
				Sleep(100);
				Z_Move(pApp->GetBHZ1HomeOffset());
			}
		//}
	}

	Z_Home();
	T_MoveTo(m_lPrePickPos_T);

	CString szMsg;
	szMsg.Format("Auto-Learn BHZ1 levels: %ld(C)  %ld(UL)  %ld(LL)  %ld(LR)  %ld(UR)", lEncZ0, lEncZ1, lEncZ2, lEncZ3, lEncZ4);
	HmiMessage(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::BH_AutoLearnBHZ1BondLevels_BT2(IPC_CServiceMessage &svMsg)	//v3.94T5
{
	LONG lEncZ	= 0;
	LONG lEncZ0 = 0;
	LONG lEncZ1 = 0;
	LONG lEncZ2 = 0;
	LONG lEncZ3 = 0;
	LONG lEncZ4 = 0;

	BOOL bReturn = TRUE;
	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	BOOL bResult = TRUE;
	LONG lPosn = 0;

	//For Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

	//v4.08
	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (!CMS896AApp::m_bMS100Plus9InchOption)		//MS100 9Inch only
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	HmiMessage("Start auto-learn BHZ1 levels on BT2 ...");

	Z_MoveTo(0);
#ifdef NU_MOTION
	Z2_MoveTo(0);
#endif

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
	//{
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
#ifdef NU_MOTION
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
#endif
	//}

	T_MoveTo(m_lBondPos_T);


	for (INT i = 0; i <= 4; i++)	//v4.08
	{
		lPosn = i;
		rReqMsg.InitMessage(sizeof(LONG), &lPosn);
		
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToBinArea4Corners2", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult == FALSE)
		{
			Z_Home();
			T_MoveTo(m_lPrePickPos_T);

			CString szErr;
			szErr.Format("ERROR: Auto Learn BHZ1 levels fail BT-Move at BT2 #%d corner", i);
			HmiMessage(szErr);

			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		lEncZ = 0;	//v4.49A6

		if (!AutoLearnBHZ1BondLevel(lEncZ))
		{
			Z_Home();
			T_MoveTo(m_lPrePickPos_T);

			CString szErr;
			szErr.Format("ERROR: Auto Learn BHZ1 levels fail at BT2 #%d corner", i);
			HmiMessage(szErr);

			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (i == 0)
		{
			lEncZ0 = lEncZ;
		}
		else if (i == 1)
		{
			lEncZ1 = lEncZ;
		}
		else if (i == 2)
		{
			lEncZ2 = lEncZ;
		}
		else if (i == 3)
		{
			lEncZ3 = lEncZ;
		}
		else
		{
			lEncZ4 = lEncZ;
		}

		Z_MoveTo(0);
	}

	Z_Home();
	T_MoveTo(m_lPrePickPos_T);

	CString szMsg;
	szMsg.Format("Auto-Learn BHZ1 levels on BT2 : %ld(C)  %ld(UL)  %ld(LL)  %ld(LR)  %ld(UR)", lEncZ0, lEncZ1, lEncZ2, lEncZ3, lEncZ4);
	HmiMessage(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_ConfirmSetup(IPC_CServiceMessage &svMsg)
{
	//v4.43T5
	if (m_bMS100EjtXY && !m_bEnableMS100EjtXY)
	{
		m_lArm1Pos_EjX = m_lCDiePos_EjX;
		m_lArm1Pos_EjY = m_lCDiePos_EjY;
		m_lArm2Pos_EjX = m_lCDiePos_EjX;
		m_lArm2Pos_EjY = m_lCDiePos_EjY;
	}

	SaveBhData();
	m_bEnableBHMark_HMI = IsEnableBHMark();
	m_bEnableAutoUpdateBHPickPosnMode_HMI = IsEnableBHMark() || IsEnableColletHole();
#ifdef NU_MOTION
	//v4.42T6
	CString szLog;
	szLog.Format("MS100 BHZ Sync-pick trigger values updated to %ld, %ld", m_lSyncZOffset, m_lSyncTriggerValue);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
#endif

	// update delay
	// 3481
	SetGemValue("BH_PickDelay",			m_lPickDelay);
	SetGemValue("BH_BondDelay",			m_lBondDelay);
	SetGemValue("BH_ArmPickDelay",		m_lArmPickDelay);
	SetGemValue("BH_ArmBondDelay",		m_lArmBondDelay);
	SetGemValue("BH_HeadPickDelay",		m_lHeadPickDelay);
	SetGemValue("BH_HeadBondDelay",		m_lHeadBondDelay);
	// 3482
	SetGemValue("BH_PRDelay",			m_lPRDelay);
	SetGemValue("BH_EjectorUpDelay",	m_lEjectorUpDelay);
	SetGemValue("BH_WTTDelay",			m_lWTTDelay);
	SetGemValue("BH_BTDelay",			m_lBinTableDelay);
	SetGemValue("BH_lWTSettlingDelay",	m_lWTSettlingDelay);
	// 3483
	SetGemValue("BH_HighBlowDelay",		m_lHighBlowTime);
	SetGemValue("BH_WeakBlowDelay",		m_lWeakBlowOffDelay);
	// 7604
	SendEvent(SG_CEID_BH_DELAY, FALSE);

	SendCE_EjKOffset();
	SendCE_ToolLife();

	// 3486
	SetGemValue("AB_bMissDieCheck", m_bCheckMissingDie);
	SetGemValue("AB_bJamColletCheck", m_bCheckColletJam);
	SetGemValue("AB_MissRetryCount", m_lMissingDie_Retry);
	SetGemValue("AB_JamRetryCount", m_ulColletJam_Retry);
	// 7700
	SendEvent(SG_CEID_AB_OPTION, FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}
//1.08S
LONG CBondHead::BH_ConfirmMaxColletCountSetup(IPC_CServiceMessage &svMsg)
{
	LogItems(CHANGE_COLLET_LIMIT_Z1);
	SaveBhData();
	// 7700
	SendEvent(SG_CEID_AB_OPTION, FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	
	return TRUE;
}

LONG CBondHead::BH_ConfirmMaxCleanCount(IPC_CServiceMessage &svMsg)
{
	LogItems(CLEAN_COLLET_LIMIT);
	SaveBhData();
	// 7700
	SendEvent(SG_CEID_AB_OPTION, FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::BH_ConfirmMaxCollet2CountSetup(IPC_CServiceMessage &svMsg)
{
	LogItems(CHANGE_COLLET_LIMIT_Z2);
	SaveBhData();
	// 7700
	SendEvent(SG_CEID_AB_OPTION, FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CBondHead::BH_ConfirmMaxEjectorCountSetup(IPC_CServiceMessage &svMsg)
{
	LogItems(CHANGE_EJECTOR_LIMIT);
	SaveBhData();
	// 7700
	SendEvent(SG_CEID_AB_OPTION, FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BH_ChangeEjElevatorPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		BOOL	bFromZero;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	
	//v4.08
	if (m_bDisableBH)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}
	

	if( IsESMachine() )
	{
		// MOVE up NG pick Solenoid module.
		SetNGPickUpDn(FALSE);

		//	check expander open or not, the ng pick module is down or not.
		BOOL bReturn= TRUE;
		IPC_CServiceMessage stMsg;

		int nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "IsExpanderSafeToMove", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bReturn == FALSE)
		{
			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return TRUE;
		}

		nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "IsExpander2SafeToMove", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bReturn == FALSE)
		{
			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return TRUE;
		}
	}

	// Disable Wafer Table Joystick
	SetWaferTableJoystick(FALSE);
	// Lock Wafer Table Joystick
	LockWaferTableJoystick(TRUE);
	LockPRMouseJoystick(TRUE);
	EjElevator_MoveTo(0);

	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	BOOL bToEjtOffset = TRUE;

	CString szText;
	CStringList szSelection;
	LONG lSelection = 0, lResult;
	szSelection.AddTail("HOME position");
	szSelection.AddTail("NG-PICK position");

	lResult = HmiSelection("Please select TABLE1 position", "Change Ej Elevator Setup", szSelection, lSelection);

	if (lResult < 0)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}
	else if (lResult == 0)	//HOME
	{
		stMsg.InitMessage(sizeof(BOOL), &bToEjtOffset);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "MoveTable1ToHome", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);	
			}
		}
	}
	else					//EJ Offset XY for NGPick
	{
		stMsg.InitMessage(sizeof(BOOL), &bToEjtOffset);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "MoveTable1ToEjtOffset", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);	
			}
		}
	}


	//If learn from Home Position
	if (stInfo.bFromZero == TRUE)
	{
		m_lBPGeneral_TmpD = 0;

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_F = 0;	
				break;	//Standby Level
			default:	
				m_lBPGeneral_E = 0;	
				break;	//Up Level
		}
	}

	switch (stInfo.lPosition)
	{
		case 1:		//Standby
			if (stInfo.bFromZero == FALSE)
			{
				EjElevator_MoveTo(m_lStandby_EJ_Elevator);
				m_lBPGeneral_TmpD = m_lStandby_EJ_Cap;
			}
			break;

		default:	//Up Level
//			if (stInfo.bFromZero == FALSE)
//			{
//				EjElevator_MoveTo(m_lStandby_EJ_Elevator);
//				m_lBPGeneral_TmpD = m_lStandby_EJ_Cap;
//			}
			break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_KeyInEjElevatorPosition(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);


	
	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}
	

	switch (stInfo.lPosition)
	{
		case 1:		//Standby
			m_lBPGeneral_F = stInfo.lStep;
			EjElevator_MoveTo(m_lBPGeneral_F);
			break;

		default:	//Up Level
			m_lBPGeneral_E = stInfo.lStep;
			EjElevator_MoveTo(m_lBPGeneral_E);
			break;
	}

	return TRUE;
}

LONG CBondHead::BH_MovePosEjElevatorPosition(IPC_CServiceMessage &svMsg)
{
	
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	
	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_F + stInfo.lStep;	
			break;	//Standby
		default:	
			lCurrentPos = m_lBPGeneral_E + stInfo.lStep;	
			break;	//Up Level
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
	
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		EjElevator_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_F = lCurrentPos;	
				break;
			default:	
				m_lBPGeneral_E = lCurrentPos;	
				break;
		}
	}

	return TRUE;
}

LONG CBondHead::BH_MoveNegEjElevatorPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_F - stInfo.lStep;	
			break;	//Standby
		default:	
			lCurrentPos = m_lBPGeneral_E - stInfo.lStep;	
			break;	//Up Level
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
	
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		EjElevator_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_F = lCurrentPos;	
				break; //stand by
			default:	
				m_lBPGeneral_E = lCurrentPos;	
				break; // up level
		}
	}

	return TRUE;
}

LONG CBondHead::BH_ConfirmEjElevatorSetup(IPC_CServiceMessage &svMsg)
{
	
	//v4.08
	if (m_bDisableBH)
	{
		//bReturn = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();					//v3.86

	LockWaferTableJoystick(FALSE);
	LockPRMouseJoystick(FALSE);
	SetWaferTableJoystick(FALSE);

	EjElevator_MoveTo(0);

	CloseLoadingAlert();	//v3.86

	//Update variable
//	m_lUpLevel_El = m_lBPGeneral_E;
	m_lStandby_EJ_Cap = m_lBPGeneral_F;


	//v4.28
	IPC_CServiceMessage stMsg;
	BOOL bToEjtOffset = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bToEjtOffset);
	INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "MoveTable1ToEjtOffset", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);	
		}
	}


	SaveBhData();
	SetStatusMessage("Ejector Elevator level is updated");

	return TRUE;
}

LONG CBondHead::BH_CancelEjElevatorSetup(IPC_CServiceMessage &svMsg)
{
	//v4.08
	if (m_bDisableBH)
	{
		//bReturn = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();	//v3.86

	LockWaferTableJoystick(FALSE);
	LockPRMouseJoystick(FALSE);
	SetWaferTableJoystick(FALSE);

	EjElevator_MoveTo(0);

	CloseLoadingAlert();	//v3.86

	//v4.28
	IPC_CServiceMessage stMsg;
	BOOL bToEjtOffset = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bToEjtOffset);
	INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "MoveTable1ToEjtOffset", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);	
		}
	}

	//Restore HMI variable
//	m_lBPGeneral_E = m_lUpLevel_El;
	m_lBPGeneral_F = m_lStandby_EJ_Cap;

	return TRUE;
}

LONG CBondHead::BH_ColletVacuum(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetPickVacuum(bOn);

	return TRUE;
}

LONG CBondHead::BH_ColletVacuumZ2(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (m_bIsArm2Exist == FALSE)
	{
		return TRUE;
	}

#ifdef NU_MOTION

	SetPickVacuumZ2(bOn);
#endif

	return TRUE;
}

LONG CBondHead::BH_StrongBlow(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetStrongBlow(bOn);

	return TRUE;
}

LONG CBondHead::BH_StrongBlowZ2(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (m_bIsArm2Exist == FALSE)
	{
		return TRUE;
	}

#ifdef NU_MOTION
	SetStrongBlowZ2(bOn);
#endif

	return TRUE;
}

LONG CBondHead::BH_HomeEjector(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = (Ej_Home()== gnOK);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CBondHead::BH_HomeEjElevator(IPC_CServiceMessage &svMsg)
{
	EjElevator_Home();

	return TRUE;
}

LONG CBondHead::BH_HomeBondHead(IPC_CServiceMessage &svMsg)
{
	Z_Home();
	if (m_bIsArm2Exist == TRUE)		//v4.22T8
	{
		Z2_Home();
	}

	return TRUE;
}


LONG CBondHead::BH_HomeBondArm(IPC_CServiceMessage &svMsg)
{
	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	BOOL bResult = TRUE;

	//Check ExArm position to avoid crashing with Bond Arm
	BOOL bBinLoaderEnabled = IsBLEnable();


	//v4.34T9	//Avoid myler scratch by Ej pin
	if (Ej_IsPowerOn())
	{
		Ej_MoveTo(0);
	}
	else
	{
		Ej_Home();
	}

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	T_Home();

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_ResetAll(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();

	Z_MoveToHome();
#ifdef NU_MOTION
	Z2_MoveToHome();
#endif

	if (Ej_IsPowerOn())		//v3.92
	{
		Ej_MoveTo(0);
	}
	else
	{
		Ej_Home();
	}

	BOOL bResult = TRUE;
	//v4.55A5
	if (IsBondArmAbleToMove())
	{
		T_Home();
	}

	CloseLoadingAlert();
	return TRUE;
}


//Used HMI user routine
LONG CBondHead::GetChannelTravelLimit(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG lT_Min;
		LONG lT_Max;
		LONG lZ_Min;
		LONG lZ_Max;
		LONG lE_Min;
		LONG lE_Max;
	} PARA_TYPE;

	PARA_TYPE	stInfo;

	stInfo.lT_Min = GetChannelInformation(MS896A_CFG_CH_BONDARM, MS896A_CFG_CH_MIN_DISTANCE);								
	stInfo.lT_Max = GetChannelInformation(MS896A_CFG_CH_BONDARM, MS896A_CFG_CH_MAX_DISTANCE);								
	stInfo.lZ_Min = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);								
	stInfo.lZ_Max = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MAX_DISTANCE);								
	stInfo.lE_Min = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);								
	stInfo.lE_Max = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								


	svMsg.InitMessage(sizeof(PARA_TYPE), &stInfo);
	return TRUE;
}

LONG CBondHead::BH_ResetColletCountDiag(IPC_CServiceMessage &svMsg)
{
	CString szContent, szTitle;
	BOOL bMsg = TRUE;
	BOOL bReturn = TRUE;
	//BOOL bCancel = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bMsg);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if (bMsg == TRUE)
	{
		if (m_oColletRecord.m_szShiftNo.IsEmpty())
		{
			szContent = "Please choose Shift!";
			szTitle = "Empty Selection";
			HmiMessage(szContent, szTitle);	
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else if (m_oColletRecord.m_szReason.IsEmpty())
		{
			szContent = "Please choose Reason!";
			szTitle = "Empty Selection";
			HmiMessage(szContent, szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else if (m_oColletRecord.m_szType.IsEmpty())
		{
			szContent = "Please choose Collet Type!";
			szTitle = "Empty Selection";
			HmiMessage(szContent, szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else if (m_oColletRecord.m_szColletMode.IsEmpty())
		{
			szContent = "Please choose Collet Mode!";
			szTitle = "Empty Selection";
			HmiMessage(szContent, szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		/*if (!pApp->CheckSananPasswordFile(TRUE, bCancel, 2))
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}*/

		szTitle.LoadString(HMB_BH_CHANGE_COLLET_CNT);
		szContent.LoadString(HMB_BH_RESET_COLLET_CONF);
		
		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
		{
			if (m_nHmiResetCollet == 2)		//v3.92
			{
				ResetColletAndGenRecord(TRUE);
			}
			else
			{
				ResetColletAndGenRecord(FALSE);
			}

			szContent.LoadString(HMB_GENERAL_RESET);
			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
		}
	
		szTitle.LoadString(HMB_BH_CLEAN_COLLET_CNT);
		szContent.LoadString(HMB_BH_RESET_CLEAN_CONF);

		if ((m_ulMaxCleanCount > 0) && (m_ulCleanCount > 0))	//v4.40T12
		{
			if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
			{
				bReturn = TRUE;
				ResetCleanColletCount();

				szContent.LoadString(HMB_GENERAL_RESET);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			}
		}
	}

	ResetColletRecord();	// disable the message box
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetCollet1CountDiag(IPC_CServiceMessage &svMsg)
{
	//v4.53A25
	//This fcn is used for Semitek as a shortcut button on Alarm panel
	BOOL bReturn = TRUE;
	CString szContent, szTitle;

	ResetColletAndGenRecord(FALSE);
	szContent.LoadString(HMB_GENERAL_RESET);
	HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);

	ResetColletRecord();	// disable the message box

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetCollet2CountDiag(IPC_CServiceMessage &svMsg)
{
	//v4.53A25
	//This fcn is used for Semitek as a shortcut button on Alarm panel
	BOOL bReturn = TRUE;
	CString szContent, szTitle;

	ResetColletAndGenRecord(TRUE);

	szContent.LoadString(HMB_GENERAL_RESET);
	HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);

	ResetColletRecord();	// disable the message box

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetColletCount(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;
	BOOL bReturn = FALSE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();


	szTitle.LoadString(HMB_BH_CHANGE_COLLET_CNT);
	szContent.LoadString(HMB_BH_RESET_COLLET_CONF);

	if (CMS896AStn::m_bEnableToolsUsageRecord == TRUE)
	{
		ResetColletRecord();	// reset dialog variable
		m_oColletRecord.m_bShowColletDiagPage = TRUE;
		bReturn = TRUE;
		m_nHmiResetCollet = 1;	//v3.92
		m_bAutoChgCollet1Fail = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	else
	{
		if (pApp->GetCustomerName() == "Semitek")	//v4.53A24
		{
			m_bAutoChgCollet1Fail = FALSE;
			ResetColletCount();
			CMSLogFileUtility::Instance()->MS_LogOperation("Collet 1 lifetime is reset (Semitek)");
		}
		else
		{
			if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
			{
				m_bAutoChgCollet1Fail = FALSE;
				ResetColletCount();
				szContent.LoadString(HMB_GENERAL_RESET);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			}
		}
	}

	szTitle.LoadString(HMB_BH_CLEAN_COLLET_CNT);
	szContent.LoadString(HMB_BH_RESET_CLEAN_CONF);

	if ((m_ulMaxCleanCount > 0) && (m_ulCleanCount > 0))	//v4.40T12
	{
		if (pApp->GetCustomerName() == "Semitek")	//v4.53A24
		{
			bReturn = TRUE;
			//ResetCleanColletCount();
			//CMSLogFileUtility::Instance()->MS_LogOperation("Collet 1 Clean Collet Count is reset (Semitek)");
		}
		else
		{
			if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
			{
				bReturn = TRUE;
				ResetCleanColletCount();

				szContent.LoadString(HMB_GENERAL_RESET);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetCollet2Count(IPC_CServiceMessage &svMsg)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szTitle, szContent;
	BOOL bReturn = TRUE;

	szTitle.LoadString(HMB_BH_CHANGE_COLLET_CNT);
	szContent.LoadString(HMB_BH_RESET_COLLET_CONF);

	if (CMS896AStn::m_bEnableToolsUsageRecord == TRUE)
	{
		ResetColletRecord();	// reset dialog variable
		m_oColletRecord.m_bShowColletDiagPage = TRUE;
		m_nHmiResetCollet = 2;	//v3.92
		m_bAutoChgCollet2Fail = FALSE;
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	else
	{
		if (pApp->GetCustomerName() == "Semitek")	//v4.53A24
		{
			ResetCollet2Count();
			m_bAutoChgCollet2Fail = FALSE;
			CMSLogFileUtility::Instance()->MS_LogOperation("Collet 2 lifetime is reset (Semitek)");
		}
		else
		{
			if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
			{
				ResetCollet2Count();
				m_bAutoChgCollet2Fail = FALSE;
				szContent.LoadString(HMB_GENERAL_RESET);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetEjectorCountDiag(IPC_CServiceMessage &svMsg)
{
	BOOL bMsg = TRUE;
	BOOL bReturn = TRUE;
	//BOOL bCancel = FALSE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	svMsg.GetMsg(sizeof(BOOL), &bMsg);

	if (bMsg == TRUE)
	{
		CString szTitle, szContent;	
		if (m_oEjectorRecord.m_szShiftNo.IsEmpty())
		{
			szContent = "Please choose Shift!";
			szTitle = "Empty Selection";
			HmiMessage(szContent, szTitle);	
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else if (m_oEjectorRecord.m_szReason.IsEmpty())
		{
			szContent = "Please choose Reason!";
			szTitle = "Empty Selection";
			HmiMessage(szContent, szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else if (m_oEjectorRecord.m_szType.IsEmpty())
		{
			szContent = "Please choose Ejector Type!";
			szTitle = "Empty Selection";
			HmiMessage(szContent, szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else if (m_oEjectorRecord.m_szUsageType.IsEmpty())
		{
			szContent = "Please choose Usage Type!";
			szTitle = "Empty Selection";
			HmiMessage(szContent, szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		
		/*if (!pApp->CheckSananPasswordFile(TRUE, bCancel, 2))
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}*/

		szTitle.LoadString(HMB_BH_EJT_COUNT);
		szContent.LoadString(HMB_BH_RESET_EJT_CONF);

		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
		{
			m_oEjectorRecord.m_szEjectorUpLevel.Format("%d", m_lEjectLevel_Ej);
			m_oEjectorRecord.m_szEjectorPinCount.Format("%d", m_ulEjectorCount);
			m_oEjectorRecord.m_szProductType	 = (*m_psmfSRam)["MS896A"]["PKG Filename"];
			m_oEjectorRecord.m_szOperatorLogInId = (*m_psmfSRam)["MS896A"]["PasswordID"];
			m_oEjectorRecord.m_szMachineNo		 = (*m_psmfSRam)["MS896A"]["MachineNo"];
			if ((pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="XA")|| pApp->GetCustomerName() == "ChangeLight(XM)")
				m_oEjectorRecord.m_szOperatorLogInId = (*m_psmfSRam)["MS896A"]["PasswordID"];//(*m_psmfSRam)["MS896A"]["Operator Id"];
			else
				m_oEjectorRecord.m_szOperatorLogInId = (*m_psmfSRam)["MS896A"]["Operator Id"];
			
			if( pApp->GetCustomerName()==CTM_SANAN || 
				pApp->GetCustomerName()=="SiLan" )
			{
				m_oEjectorRecord.m_szRemark = pApp->GetPKGFilename();
			}
			pApp->GenerateEjectorUsageRecordFile(m_oEjectorRecord);
			m_oEjectorRecord.m_szEjtTypeOld	= m_oEjectorRecord.m_szType;
			m_oEjectorRecord.m_szEjtRemarkOld	= m_oEjectorRecord.m_szRemark;
			SaveFileHWD();

			ResetEjectorCount();
			
			szContent.LoadString(HMB_GENERAL_RESET);
			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
		}
	}
	
	ResetEjectorRecord();	// disable the message box
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetEjectorCount(IPC_CServiceMessage &svMsg)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szTitle, szContent;
	BOOL bReturn = FALSE;

	m_bEjNeedReplacement = FALSE;

	//andrewng //2020-0528
	/*if (CMS896AStn::m_bEnableToolsUsageRecord == TRUE)
	{
		ResetEjectorRecord();	// reset the dialog variable
		m_oEjectorRecord.m_bShowEjectorDiagPage = TRUE;
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	else*/
	{
		szTitle.LoadString(HMB_BH_EJT_COUNT);
		szContent.LoadString(HMB_BH_RESET_EJT_CONF);

		if (pApp->GetCustomerName() == "Semitek")	//v4.53A24
		{
			bReturn = ResetEjectorCount();
			CMSLogFileUtility::Instance()->MS_LogOperation("Ejector Count is reset (Semitek)");
		}
		else
		{
			if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
			{
				bReturn = ResetEjectorCount();
				szContent.LoadString(HMB_GENERAL_RESET);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetAllDieCount(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;	
	BOOL bReturn = FALSE;

	//v2.83T2
	//Support auto-reset in MS_State by EquipTime report generation
	BOOL bPrompt = TRUE;
	try 
	{
		svMsg.GetMsg(sizeof(BOOL), &bPrompt);
	} 
	catch (CAsmException e) 
	{
		bPrompt = TRUE;
	}

	szTitle.LoadString(HMB_BH_GENERAL_DIE_CNT);
		
	szContent.LoadString(HMB_BH_RESET_GENERAL_CNT);

	BOOL bClearAll = FALSE;
	if( !bPrompt )
		bClearAll = TRUE;
	if (bPrompt && (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES))
	{
		bClearAll = TRUE;
		szContent.LoadString(HMB_GENERAL_RESET);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}
	
	if ( bClearAll )
	{
		//v4.47A5	//XM SanAn
		CString szLog;
		szLog.Format("Die counters are reset - Pick=%lu, Bond=%lu, Def=%lu, BC=%lu, MD=%lu, Rot=%lu, EMPTY=%lu",
						m_ulPickDieCount, m_ulBondDieCount, m_ulDefectDieCount, m_ulBadCutDieCount, 
						m_ulMissingDieCount, m_ulRotateDieCount, m_ulEmptyDieCount);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		//for EM
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		//if (pApp->m_eqMachine.IsEquipmentManager())
		//{
		//	pApp->m_eqMachine.SetEMHourlyRecipe(pApp->m_szDeviceFile);
		//	pApp->m_eqMachine.ResetAll();
		//	pApp->m_eqMachine2.ResetAll();	//WH Sanan	//v4.40T4
		//}

		m_ulPickDieCount			= 0;		// Picked Die
		m_ulBondDieCount			= 0;		// Bonded Die
		m_ulDefectDieCount			= 0;		// Defective Die
		m_ulBadCutDieCount			= 0;		// Bad-cut Die
		m_ulMissingDieCount			= 0;		// Missing Die
		m_ulRotateDieCount			= 0;		// Rotate Die
		m_ulEmptyDieCount			= 0;		// Empty Die

		m_lPBIShiftCounter			= 0;		//PBI Shift Reject
		m_lPBIAngleCounter			= 0;		//PBI Rotation Reject

		if (m_pulStorage != NULL)
		{
			*(m_pulStorage + BH_PICKDIE_OFFSET) = 0;
			*(m_pulStorage + BH_BONDDIE_OFFSET) = 0;
			*(m_pulStorage + BH_DEFECTDIE_OFFSET) = 0;
			*(m_pulStorage + BH_BADCUTDIE_OFFSET) = 0;
			*(m_pulStorage + BH_MISSINGDIE_OFFSET) = 0;
			*(m_pulStorage + BH_ROTATEDIE_OFFSET) = 0;
			*(m_pulStorage + BH_EMPTYDIE_OFFSET) = 0;
		}
		bReturn = TRUE;
		SetStatusMessage("All die counts are reset");

		if (CMS896AStn::m_bGenerateAccumulateDieCountReport == TRUE)
		{
			GenerateAccumlateDieCountReport();
		}

		SendCE_BondCounter();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BH_ResetAllWaferDieCount(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;
	BOOL bPrompt = FALSE;
	BOOL bReturn = FALSE;
	LONG lStatus = 0;

	svMsg.GetMsg(sizeof(BOOL), &bPrompt);


	szTitle.LoadString(HMB_BH_WAFER_DIE_CNT);
	
	szContent.LoadString(HMB_BH_RESET_WAFER_CNT);

	if (bPrompt == TRUE)
	{
		lStatus = HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
	}
	else
	{
		lStatus = glHMI_YES;
	}

	if (lStatus == glHMI_YES)
	{
		m_ulCurrWaferBondDieCount	= 0;		// Bond Die			
		m_ulCurrWaferDefectDieCount	= 0;		// Defective Die	
		m_ulCurrWaferChipDieCount	= 0;		// Chip Die
		m_ulCurrWaferInkDieCount	= 0;		// Ink Die
		m_ulCurrWaferBadCutDieCount	= 0;		// Bad-cut Die		
		m_ulCurrWaferRotateDieCount	= 0;		// Rotate Die		
		m_ulCurrWaferEmptyDieCount	= 0;		// Empty Die
		m_ulCurrWaferMissingDieCount = 0;
		m_dCurrWaferSelGradeYield	= 0.0;		// Yield

		//(*m_psmfSRam)["BondHead"]["CurrentWafer"]["MissingDie"] = m_ulCurrWaferMissingDieCount;


		if (m_pulStorage != NULL)
		{
			*(m_pulStorage + BH_CURWAF_BONDEDDIE_OFFSET) = 0;
			*(m_pulStorage + BH_CURWAF_DEFECTDIE_OFFSET) = 0;
			*(m_pulStorage + BH_CURWAF_CHIPDIE_OFFSET) = 0;
			*(m_pulStorage + BH_CURWAF_INKDIE_OFFSET) = 0;
			*(m_pulStorage + BH_CURWAF_BADCUTDIE_OFFSET) = 0;
			*(m_pulStorage + BH_CURWAF_ROTATEDIE_OFFSET) = 0;
			*(m_pulStorage + BH_CURWAF_EMPTYDIE_OFFSET)	= 0;
			*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET) = 0;
		}
		bReturn = TRUE;
		SetStatusMessage("All wafer die counts are reset");

		if (bPrompt == TRUE)
		{
			szContent.LoadString(HMB_GENERAL_RESET);

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
		}

		DeleteFile(MSD_WAFER_RECORD_FILE);
		DeleteFile(MSD_WAFER_RECORD_BKF);

		SendCE_CurrWFCounter();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetCustomCounter(IPC_CServiceMessage &svMsg)
{
	LONG lIndex = 0;
	svMsg.GetMsg(sizeof(LONG), &lIndex);

	switch (lIndex)
	{
		case 2:
			m_ulCustomCounter2 = 0;
			if (m_pulStorage != NULL)
			{
				*(m_pulStorage + BH_CUSTOM_COUNT_2)	= m_ulCustomCounter2;
			}
			HmiMessage("Custom Counter #2 is reset.");
			break;
	
		case 3:
			m_ulCustomCounter3 = 0;
			if (m_pulStorage != NULL)
			{
				*(m_pulStorage + BH_CUSTOM_COUNT_3)	= m_ulCustomCounter3;
			}
			HmiMessage("Custom Counter #3 is reset.");
			break;

		case 1:
			//default:
			m_ulCustomCounter1 = 0;
			if (m_pulStorage != NULL)
			{
				*(m_pulStorage + BH_CUSTOM_COUNT_1)	= m_ulCustomCounter1;
			}
			HmiMessage("Custom Counter #1 is reset.");
			break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetEjKOffsetCount(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;
	BOOL bReturn = FALSE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
	if (!pUtl->GetPrescanRegionMode())	 
	//(m_lEjSubRegionKCount > 0) && (m_lEjSubRegionKOffset > 0) )
	{
		//bReturn = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return 1;
	}

	szTitle.LoadString(HMB_BH_EJT_KOFFSET);
	szContent.LoadString(HMB_BH_RESET_EJT_KOFFSET_CONF);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		//v4.46T26	//SanAn
		m_ulEjectorCountForKOffset		= 0;
		m_lEjectorKOffsetTotal			= 0;
		m_lEjectorKOffsetBHZ1Total		= 0;	//v4.52A10
		m_lEjectorKOffsetBHZ2Total		= 0;	//v4.52A10
		m_ulEjCountForSubRegionKOffset	= 0;
		m_lEjSubRegionKOffsetTotal		= 0;
		m_ulEjCountForSubRegionSKOffset	= 0;
		m_lEjSubRegionSKOffsetTotal		= 0;
		m_lEjSubRegionSmartValue		= 0;
		//v4.53A25
		m_ulZ1CountForSmartKOffset		= 0;
		m_lZ1SmartKOffsetTotal			= 0;
		m_lZ1SmartValue					= 0;
		m_ulZ2CountForSmartKOffset		= 0;
		m_lZ2SmartKOffsetTotal			= 0;
		m_lZ2SmartValue					= 0;

		bReturn = TRUE;

		SetStatusMessage("Ejector KOffset and Smart KOffset are reset");

		szContent.LoadString(HMB_GENERAL_RESET);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetBHZKOffsetCount(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;
	BOOL bReturn = FALSE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();

	//szTitle.LoadString(HMB_BH_EJT_KOFFSET);
	//szContent.LoadString(HMB_BH_RESET_EJT_KOFFSET_CONF);
	szTitle		= _T("BHZ Smart K-Offset");
	szContent	= _T("Reset both BHZ Smart K-Offset counters?");

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		m_lZ1SmartValue			= 0;		// Z K-Offset no. of total steps accumulated
		m_lZ1SmartKOffsetTotal	= 0;		// Z K-Offset actual encoder comp value
		m_lZ2SmartValue			= 0;		// Z K-Offset no. of total steps accumulated
		m_lZ2SmartKOffsetTotal	= 0;		// Z K-Offset actual encoder comp value

		bReturn = TRUE;

		SetStatusMessage("BHZ Smart KOffsets are reset");

		szContent.LoadString(HMB_GENERAL_RESET);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_UpdateCurrentWaferYield(IPC_CServiceMessage &svMsg)
{
	CUIntArray aulGradeList;

	ULONG ulTotalDie = 0;
	ULONG ulTotal = 0;
	ULONG ulLeft = 0;
	ULONG ulPick = 0;
	BOOL bReturn = TRUE;
	int i;

	m_WaferMapWrapper.GetSelectedGradeList(aulGradeList);

	for (i = 0; i < aulGradeList.GetSize(); i++)
	{
		m_WaferMapWrapper.GetStatistics((UCHAR)aulGradeList[i], ulLeft, ulPick, ulTotal);
		ulTotalDie += ulTotal;
	}

	if (ulTotalDie == 0)
	{
		m_dCurrWaferSelGradeYield = 0.0;
	}
	else
	{
		m_dCurrWaferSelGradeYield = ((DOUBLE)m_ulCurrWaferBondDieCount / (DOUBLE)ulTotalDie) * 100;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_MoveToBlow(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	//No BHT in Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

	if (m_fHardware == FALSE)	//v3.60
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bDisableBH)	//v3.60
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsAllMotorsEnable() == FALSE)
	{
		SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
		SetErrorMessage("BondArm module is off power");

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	Z_MoveTo(0);
	if (m_bIsArm2Exist)
	{
		Z2_MoveTo(0);
	}

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
	//{
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
#ifdef NU_MOTION
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
#endif
	//}

	GetEncoderValue();
	if (m_lEnc_T >= m_lCleanColletPos_T + 30000 || m_lEnc_T <= m_lCleanColletPos_T - 30000)
	{
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lCleanColletPos_T);
		T_Profile(NORMAL_PROF);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_MoveToPrePick(IPC_CServiceMessage &svMsg)
{
	if (m_lSwingLevel_Z > 0)	//Cree HuiZhou MS899DLA180	//v4.17T5
	{
		Z_MoveTo(m_lSwingLevel_Z);
		if (m_bIsArm2Exist)
		{
			Z2_MoveTo(m_lSwingLevel_Z2);	//v4.44A1	//Semitek
		}
	}
	else
	{
		Z_MoveTo(0);
		if (m_bIsArm2Exist)
		{
			Z2_MoveTo(0);
		}
	}

	//OsramTrip 8/22
	//if (IsBondArmAbleToMove())
	//{
	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lPrePickPos_T);
	T_Profile(NORMAL_PROF);
	//}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondHead::BH_MoveToPick(IPC_CServiceMessage &svMsg)
{
	Z_MoveTo(0);
	if (m_bIsArm2Exist)
	{
		Z2_MoveTo(0);
	}

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
	{
		Sleep(100);
		Z_Move(pApp->GetBHZ1HomeOffset());
	}
#ifdef NU_MOTION
	if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
	{
		Sleep(100);
		Z2_Move(pApp->GetBHZ2HomeOffset());
	}
#endif

	if (IsBondArmAbleToMove())
	{
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPickPos_T);
		T_Profile(NORMAL_PROF);
	}

	return 1;
}

LONG CBondHead::BH_MoveToPickLevel(IPC_CServiceMessage &svMsg)
{
	//v3.83
	INT nCount = 0;
	BOOL bBH2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBH2);

	Z_MoveTo(0);

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
	{
		Sleep(100);
		Z_Move(pApp->GetBHZ1HomeOffset());
	}

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	if (bBH2)
	{
		T_SMoveTo(m_lBondPos_T);
	}
	else
	{
		T_SMoveTo(m_lPickPos_T);
	}
	T_Profile(NORMAL_PROF);


	if (bBH2)
	{
		Z_MoveTo(m_lBondLevel_Z);    //v3.86 //bug-fix
	}	
	else
	{
		Z_MoveTo(m_lPickLevel_Z);
	}

	return 1;
}

LONG CBondHead::BH_MoveEjToUpLevel(IPC_CServiceMessage &svMsg)
{
	//v3.83
	BOOL bEj = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEj);

	Ej_MoveTo(0);

	//T_Profile(CLEAN_COLLET_PROF);
	if (bEj)
	{
		Ej_MoveTo(m_lEjectLevel_Ej);
	}
	else
	{
		Ej_Home();
	}
	//Ej_MoveTo(0);
	//T_Profile(NORMAL_PROF);

	return 1;
}

LONG CBondHead::BH_MoveBHToPick1(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		BOOL		bPick;
		LONG		lOffset;
	} PICK_TYPE;

	PICK_TYPE stInfo;

	svMsg.GetMsg(sizeof(PICK_TYPE), &stInfo);

	BOOL bReturn = BH_SubMoveBHToPick(stInfo.bPick, stInfo.lOffset);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_MoveBHToPick(IPC_CServiceMessage &svMsg)
{
	BOOL bPick		= FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bPick);

	BOOL bReturn = BH_SubMoveBHToPick(bPick, 0);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BH_MoveToBond(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn	= TRUE;
	BOOL bBond		= FALSE;
	INT nCount		= 0;

	svMsg.GetMsg(sizeof(BOOL), &bBond);

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( LoopCheckCoverOpen("Move To Pick Level")==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (bBond)
	{
		Z_MoveTo(0);

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}

		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lBondPos_T);
		T_Profile(NORMAL_PROF);
		Z_MoveTo(m_lBondLevel_Z);
	}
	else
	{
		Z_MoveTo(0);

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}

		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
		T_Profile(NORMAL_PROF);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//for Dual Arm learn collet
LONG CBondHead::BH_MoveBHToPick_Z2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn	= TRUE;
	BOOL bPick		= FALSE;
	INT nCount		= 0;

	svMsg.GetMsg(sizeof(BOOL), &bPick);

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( LoopCheckCoverOpen("Move To Pick Level")==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (bPick)
	{
		Z2_MoveTo(0);

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}

		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lBondPos_T);
		T_Profile(NORMAL_PROF);
		Z2_MoveTo(m_lPickLevel_Z2 + 500);	//v4.43T5
	}
	else
	{
		Z2_MoveTo(0);

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}

		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
		T_Profile(NORMAL_PROF);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//for Dual Arm learn collet
LONG CBondHead::BH_MoveToBond_Z2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn	= TRUE;
	BOOL bBond		= FALSE;
	INT nCount		= 0;

	svMsg.GetMsg(sizeof(BOOL), &bBond);

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( LoopCheckCoverOpen("Move To Pick Level")==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (bBond)
	{
		Z2_Home();
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPickPos_T);		//bond pos of arm 2
		T_Profile(NORMAL_PROF);
		Z2_MoveTo(m_lBondLevel_Z2);
	}
	else
	{
		Z2_Home();
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
		T_Profile(NORMAL_PROF);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_UpdateAction(IPC_CServiceMessage &svMsg)
{
	Sleep(500);
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::UpdateAction(IPC_CServiceMessage &svMsg)
{
	Sleep(500);
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::LogItems(IPC_CServiceMessage &svMsg)
{
	LONG lEventNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lEventNo);

	LogItems(lEventNo);

	return 1;
}

LONG CBondHead::BH_ShowWaferInformation(IPC_CServiceMessage &svMsg)
{
	CWaferStat dlg;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	CWinThread *pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	((CFlushMessageThread*)pThread)->m_pStation = dynamic_cast<CMS896AStn*>(this);

	if (pAppMod != NULL)
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	dlg.DoModal();	// Show the file dialog box

	if (pAppMod != NULL)
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BH_CleanCollet(IPC_CServiceMessage &svMsg)
{
	IPC_CServiceMessage stMsg;
	BOOL bReturn = TRUE;
	//No BHT in Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_fHardware == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	if (m_bDisableBH)	//v3.60
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsAllMotorsEnable() == FALSE)
	{
		SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
		SetErrorMessage("BondArm module is off power");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szContents, szTitle;
	CStringList szList;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	LONG lSelection = 0;

	if (m_bAutoCleanCollet == TRUE)
	{
		szContents = "Please select Collet Mode!";
		szTitle = "Clean Collet";
//		szList.AddTail("Manual Clean Collet");
		szList.AddTail("Auto Clean Collet");

		//v4.36T1	//PLLM MS109
		if (CMS896AApp::m_bMS100Plus9InchOption && (pApp->GetCustomerName() == "Lumileds"))
		{
			szList.AddTail("Manual Change Collet");
		}

		lSelection = HmiSelection(szContents, szTitle, szList, 0);
		lSelection++;
	}
	else
	{
		szContents = "Please select Collet Mode!";
		szTitle = "Clean Collet";
		szList.AddTail("Manual Clean Collet");
		
		//v4.36T1	//PLLM MS109
		if (CMS896AApp::m_bMS100Plus9InchOption && (pApp->GetCustomerName() == "Lumileds"))
		{
			szList.AddTail("Manual Change Collet");
			lSelection = HmiSelection(szContents, szTitle, szList, 0);
		}
	}
	

	switch (lSelection)
	{
		case 0:
			CMSLogFileUtility::Instance()->MS_LogOperation("Manual Clean-Collet");
			ManualCleanColletShortcutFunc();	
			break;
	
		case 1:
			if (m_bAutoCleanCollet == TRUE)
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("Manual Auto-Clean-Collet");
				BHAccTest(stMsg);
			}
			else
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("Manual Clean-Collet for MS109 (PLLM)");
				ManualCleanCollet_PLLM_MS109();
			}
			break;
	
		case 2:		//v4.36T1	//PLLM MS109 manual-change collet on BL side
			CMSLogFileUtility::Instance()->MS_LogOperation("Manual Clean-Collet for MS109 (PLLM)");
			ManualCleanCollet_PLLM_MS109();
			break;
	
		default:
			break;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("Manual Clean-Collet Done");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BH_HomeMotorSelection(IPC_CServiceMessage &svMsg)
{
	IPC_CServiceMessage stMsg;

	LONG lResult = 0;
	BOOL bState = FALSE;
	BOOL bReturn = TRUE;
	BOOL bBinLoaderEnabled = FALSE;
	BOOL bMotorPower = TRUE;
	int nConvID = 0;
	CStringList szSelection;
	CString szText;
	INT nCount = 0;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	INT nHomeState = gnOK;

	bMotorPower = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Motor Power"];
	bBinLoaderEnabled	= IsBLEnable();

	if (bMotorPower == TRUE)
	{
		szText = "Motors Power ON/OFF - Current is ON";
	}
	else
	{
		szText = "Motors Power ON/OFF - Current is OFF";
	}
	
	szSelection.AddTail(szText);					//0

	szSelection.AddTail("Home WaferTable");			//1
	if( !m_bDisableBH )
	{
		szSelection.AddTail("Home BondHead");		//2
	}
	if( !m_bDisableBT )
		szSelection.AddTail("Home BinTable");		//3

	if (IsEnableWL() == TRUE)
	{
		szSelection.AddTail("Home WaferLoader");	//4
	}

	if (bBinLoaderEnabled == TRUE)
	{
		szSelection.AddTail("Home BinLoader");		//5
		szSelection.AddTail("Move BinLoader Theta&Z to Home");	//6
	}

	if (IsEnableWL() == TRUE)
	{
		szSelection.AddTail("Home Wafer Gripper");	//7
	}

	if (bBinLoaderEnabled == TRUE)
	{
		szSelection.AddTail("Home Bin Gripper");	//8
	}


	lResult = HmiSelection("Please select item", "Motor Selection", szSelection, 0);
	if( lResult==-1 )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (lResult == 0)
	{
		if (IsWLExpanderOpen() == TRUE)
		{
			SetErrorMessage("Expander not closed");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	if (lResult != 0)
	{
		StartLoadingAlert();
	}

	switch (lResult)
	{
	case 0:
	default:		//Motor power on/off
		bState = !bMotorPower;
		(*m_psmfSRam)["MS896A"]["Motor Power"] = bState;

		//BondHead Module
		T_PowerOn(bState);
		Z_PowerOn(bState);
		Ej_PowerOn(bState);
		Sleep(250);

		//BinTable Module
		stMsg.InitMessage(sizeof(BOOL), &bState);
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "BT_Diag_PowerOn_All", stMsg);
		Sleep(250);

		//WaferTable Module
		stMsg.InitMessage(sizeof(BOOL), &bState);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "WT_Diag_PowerOn_All", stMsg);
		Sleep(250);

		if (IsEnableWL() == TRUE)
		{
			stMsg.InitMessage(sizeof(BOOL), &bState);
			nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "PowerX", stMsg);
			Sleep(250);

			stMsg.InitMessage(sizeof(BOOL), &bState);
			nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "PowerZ", stMsg);
			Sleep(250);
		}

		if (bBinLoaderEnabled == TRUE)
		{
			stMsg.InitMessage(sizeof(BOOL), &bState);
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "PowerX", stMsg);
			Sleep(250);

			stMsg.InitMessage(sizeof(BOOL), &bState);
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "PowerX2", stMsg);
			Sleep(250);

			stMsg.InitMessage(sizeof(BOOL), &bState);
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "PowerZ", stMsg);
			Sleep(250);

			stMsg.InitMessage(sizeof(BOOL), &bState);
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "PowerTheta", stMsg);
			Sleep(250);
		}
		break;

	case 1:			//WaferTable
		// check the expander before do anything
		if (IsWLExpanderOpen() == TRUE)
		{
			SetErrorMessage("Expander not closed");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SelectWaferCamera", stMsg);

		if( Z_Home()!=gnOK )
			nHomeState = gnNOTOK;
		if (m_bIsArm2Exist)
		{
			if( Z2_Home()!=gnOK )
				nHomeState = gnNOTOK;
		}

		if( Ej_Home()!=gnOK )
			nHomeState = gnNOTOK;

		bState = FALSE;
		BT_Set_JoyStick(FALSE);

		if( nHomeState==gnOK )
		{
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_HomeCmd", stMsg);
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T_HomeCmd", stMsg);
		}

		BT_Set_JoyStick(TRUE);
		bState = TRUE;
		break;

	case 2:			//BondHead
		if( Z_Home()!=gnOK )
			nHomeState = gnNOTOK;
		if (m_bIsArm2Exist)
		{
			if( Z2_Home()!=gnOK )
				nHomeState = gnNOTOK;
		}

		if( Ej_Home()!=gnOK )
			nHomeState = gnNOTOK;
		if (!IsMS60EjtHome())	//v4.55A6
		{
			HmiMessage_Red_Back("EJT Z motor is not HOME properly !");
		}

		bReturn = TRUE;

		//v4.55A11
		if (!IsBondArmAbleToMove())
		{
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		T_Home();
		Sleep(500);

		if (IsBondArmAbleToMove() && T_IsPowerOn() && m_bHome_T)
		{
			T_MoveTo(m_lPrePickPos_T);
		}
		break;

	case 3:			//BinTable
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SelectBondCamera", stMsg);

		bState = FALSE;
		BT_Set_JoyStick(bState);

		if( Z_Home()!=gnOK )
			nHomeState = gnNOTOK;
		if (m_bIsArm2Exist)
		{
			if( Z2_Home()!=gnOK )
				nHomeState = gnNOTOK;
		}

		if( nHomeState==gnOK )
		{
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "XY_HomeCmd", stMsg);
		}

		bState = TRUE;
		BT_Set_JoyStick(bState);
		break;

	case 4:			//WaferLoader
		nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "HomeX", stMsg);
		nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "HomeZ", stMsg);
		break;

	case 5:			//BinLoader
		// check the bin loader status first before home all motors
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "CheckBinLoaderInSafeStatusCmd", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bReturn == FALSE)
		{
			bReturn = TRUE;
			break;
		}

		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "HomeX", stMsg);
		if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.34
		{
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "HomeX2", stMsg);
		}
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "HomeZ", stMsg);
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "HomeY", stMsg);
		//nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "HomeToReady", stMsg);	//pllm
		break;

	case 6:			//BinLoader Theta&Z to READY
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "DownElevatorThetaZToHome", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		break;

	case 7:			//Wafer Gripper
		nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "HomeX", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		break;

	case 8:			//Bin Gripper
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "HomeX", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		break;
	}

	if (lResult != 0)
	{
		CloseLoadingAlert();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BH_IsBondArmSafe(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	//No BHT in Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

	LONG lCurrentPos = 0;
	BOOL bPowerOn = TRUE;
	LONG lSafePosition = (m_lBondPos_T + m_lPickPos_T) / 2; //Safe Pos is around middle of travel range 
	LONG lSafePositionTowardsPick = 0;	//For MS100		//v4.11T3	

#ifdef NU_MOTION
	lSafePositionTowardsPick	= lSafePosition + 80000;	//v4.11T3	//BHZ1 Towards PICK-side
	lSafePosition				= lSafePosition - 80000;	//v3.86		//BHZ1 Towards BOND-side
#else
	lSafePosition				= lSafePosition - 20000;
#endif

	if (m_fHardware == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bDisableBH)	//v3.60
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!m_bSel_T)	//shiraishi03
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

/*
	if (CMS896AStn::MotionIsServo(BH_AXIS_T, &m_stBHAxis_T))
	{	
		lCurrentPos = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_T, 1, &m_stBHAxis_T);
	}
	else
	{
		lCurrentPos = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_T, 0.8, &m_stBHAxis_T);
	}
	
	bPowerOn = IsAllMotorsEnable();		//v4.11T3	//Walsin China

	if (!bPowerOn)		//v3.10T1
	{
		bReturn = FALSE;
	}
#ifdef NU_MOTION
	else if ((lCurrentPos > lSafePositionTowardsPick) || (lCurrentPos < lSafePosition))	//v4.11T3	//Walsin China
	{
		bReturn = FALSE;
	}
#else
	else if (lCurrentPos < lSafePosition)
	{
		bReturn = FALSE;
	}
#endif
*/
	INT nCount = 0;
	do		//Nichia//v4.43T7
	{
		if (CMS896AStn::MotionIsServo(BH_AXIS_T, &m_stBHAxis_T))
		{	
			lCurrentPos = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_T, 1, &m_stBHAxis_T);
		}
		else
		{
			lCurrentPos = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_T, 0.8, &m_stBHAxis_T);
		}
		
		bPowerOn = IsAllMotorsEnable();		//v4.11T3	//Walsin China

		if (!bPowerOn)		//v3.10T1
		{
			bReturn = FALSE;
			break;
		}
#ifdef NU_MOTION
		else if ((lCurrentPos > lSafePositionTowardsPick) || (lCurrentPos < lSafePosition))	//v4.11T3	//Walsin China
		{
			bReturn = FALSE;
		}
#else
		else if (lCurrentPos < lSafePosition)
		{
			bReturn = FALSE;
		}
#endif
		if (bReturn == FALSE)
		{
			Sleep(100);
		}

		nCount++;
		if (nCount > 10)
		{
			break;
		}

	} while (bReturn == FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BH_FrontCoverLock(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetFrontCoverLock(bOn);
	return TRUE;
}


LONG CBondHead::BH_SideCoverLock(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetSideCoverLock(bOn);
	return TRUE;
}

LONG CBondHead::BH_BinElvatorCoverLock(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetBinElevatorCoverLock(bOn);
	return TRUE;
}


LONG CBondHead::FrontCoverLock(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (m_bIsFrontCoverExist && m_bEnableFrontCoverLock)
	{
		SetFrontCoverLock(bOn);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::SideCoverLock(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (m_bIsSideCoverExist && m_bEnableSideCoverLock)
	{
		SetSideCoverLock(bOn);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BinElevatorCoverLock(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (m_bIsSideCoverExist && m_bEnableSideCoverLock && m_bEnableBinElevatorCoverLock)
	{
		SetBinElevatorCoverLock(bOn);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::NGPickUpDn(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (IsESMachine())
	{
		SetNGPickUpDn(bOn);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::NGPickReelStop(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (IsESMachine())
	{
		SetNGPickReelStop(bOn);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::NGPickReloadReel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (!IsESMachine())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (bOn)
	{
	}
	else
	{
		NGPickT_PowerOn(FALSE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::StartEjElevator(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
/*
	INT nReturn = gnOK;
	BOOL bUseReel = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUseReel);
	
	if (!IsESMachine())	
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	if (bUseReel)
	{
		SetNGPickReelStop(TRUE);
		SetNGPickUpDn(TRUE);
		Sleep(200);
	}


	//Move EJ Table to UP pos
	nReturn = EjElevator_MoveTo(m_lUpLevel_El);
	Sleep(100);
	if (!EjElevator_IsPowerOn())
	{
		nReturn = EjElevator_Home();
		HmiMessage_Red_Yellow("Ejector elevator is power OFF!!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	SetEjectorVacuum(TRUE);
	Sleep(50);
	Ej_MoveTo(m_lEjectLevel_Ej);
	Sleep(m_lPickDelay);
	Ej_MoveTo(m_lStandbyLevel_Ej);
	SetEjectorVacuum(FALSE);
	Sleep(100);

	//Move EJ to StandBy pos
	if (bUseReel)
	{
		SetNGPickReelStop(FALSE);
		SetNGPickUpDn(FALSE);
	}

	if (!EjElevator_IsPowerOn())
	{
		nReturn = EjElevator_Home();
	}
	else
	{
		nReturn = EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}

	if (nReturn != gnOK)
	{
		bReturn = FALSE;
	}
*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::MoveEjtElvtToUpDown(IPC_CServiceMessage &svMsg)
{
	BOOL bToUp = FALSE;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bToUp);

	if (bToUp)
	{
		bReturn = (EjElevator_MoveTo(m_lStandby_EJ_Cap) == gnOK);
	}
	else
	{
		bReturn = (EjElevator_MoveTo(m_lStandby_EJ_Cap - m_lDnOffset_EJ_Cap) == gnOK);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


// ejr need to change
LONG CBondHead::MoveEjeElvtToSafeLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	bReturn = (EjElevator_MoveTo(m_lStandby_EJ_Cap) == gnOK);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

// ejr need to change
LONG CBondHead::MoveEjectorElevatorRoutine(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bToUp;
	svMsg.GetMsg(sizeof(BOOL), &bToUp);

	/*if (IsES101() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}*/

	//v4.42T15
	if (m_bMS100EjtXY)
	{
		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		m_lArm1Pos_EjX = m_lCDiePos_EjX + lCollet1OffsetX;
		m_lArm1Pos_EjY = m_lCDiePos_EjY + lCollet1OffsetY;
		m_lArm2Pos_EjX = m_lCDiePos_EjX + lCollet2OffsetX;
		m_lArm2Pos_EjY = m_lCDiePos_EjY + lCollet2OffsetY;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::GetEjtXYEncoderCmd(IPC_CServiceMessage &svMsg)	
{
	BOOL bReturn = TRUE;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} ENCVAL;
	ENCVAL stEnc;

	if (m_bMS100EjtXY)
	{
		GetEncoderValue();
		stEnc.lX = m_lEnc_EjX;
		stEnc.lY = m_lEnc_EjY;
	}

CString szLog;
szLog.Format("EJT XY GetEncoder:  %ld  %ld",  stEnc.lX, stEnc.lY);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

	svMsg.InitMessage(sizeof(ENCVAL), &stEnc);
	return 1;
}

LONG CBondHead::EjtXY_MoveToCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} RELPOS;
	
	RELPOS stPos;
	svMsg.GetMsg(sizeof(RELPOS), &stPos);
	
	if (!m_bMS100EjtXY)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!EjX_IsPowerOn() || !EjY_IsPowerOn())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

CString szLog;
szLog.Format("EJT XY MoveTo:  %ld  %ld",  stPos.lX, stPos.lY);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

	CMS896AStn::MotionSelectProfile(BH_AXIS_EJ_X, BH_MP_E_X_SLOW, &m_stBHAxis_EjX);
	CMS896AStn::MotionSelectProfile(BH_AXIS_EJ_Y, BH_MP_E_Y_SLOW, &m_stBHAxis_EjY);

	EjX_MoveTo(stPos.lX, SFM_NOWAIT);
	EjY_MoveTo(stPos.lY, SFM_WAIT);
	EjX_Sync();

	CMS896AStn::MotionSelectProfile(BH_AXIS_EJ_X, BH_MP_E_X_NORMAL, &m_stBHAxis_EjX);
	CMS896AStn::MotionSelectProfile(BH_AXIS_EJ_Y, BH_MP_E_Y_NORMAL, &m_stBHAxis_EjY);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::EjtXY_MoveToColletOffset(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bBH2 = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bBH2);
	
	if (!m_bMS100EjtXY)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (EjX_IsPowerOn() && EjY_IsPowerOn())
	{
		LONG lEjtX = m_lCDiePos_EjX;
		LONG lEjtY = m_lCDiePos_EjY;

		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		if (bBH2)
		{
			lEjtX = lEjtX + lCollet2OffsetX;
			lEjtY = lEjtY + lCollet2OffsetY;
		}
		else
		{
			lEjtX = lEjtX + lCollet1OffsetX;
			lEjtY = lEjtY + lCollet1OffsetY;
		}

		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_WAIT);
		EjX_Sync();
	}
		
	/* bye!! @ Harry Ho 20181114
	//v4.43T5
	LONG lReturn = HmiMessageEx("Move EJ to UP position?", 
		"Learn EJT Collet Offset", 
		glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (lReturn == glHMI_YES)
	{
		SetEjectorVacuum(TRUE);
		Sleep(1000);
		Ej_MoveTo(m_lEjectLevel_Ej);

		HmiMessageEx("Press CLOSE to move Ej back to HOME", 
						"Learn EJT Collet Offset", 
						glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);

		Ej_MoveTo(0);		//m_lAtPickPosnEj
		SetEjectorVacuum(FALSE);
	}
	*/

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::EjtXY_Setup(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
/*
	if (m_bMS100EjtXY)
	{
		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		m_lArm1Pos_EjX = m_lCDiePos_EjX + lCollet1OffsetX;
		m_lArm1Pos_EjY = m_lCDiePos_EjY + lCollet1OffsetY;
		m_lArm2Pos_EjX = m_lCDiePos_EjX + lCollet2OffsetX;
		m_lArm2Pos_EjY = m_lCDiePos_EjY + lCollet2OffsetY;
	}
*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::EjtXY_SearchBHColletHoles(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bMS100EjtXY)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	typedef struct {
		BOOL	bStatus;
		INT		nOffsetX;
		INT		nOffsetY;
	} SRCH_CHOLE_RESULT;


	CString szMsg;
	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;

	LONG lSelection = HmiMessage("Did you place reflective plates under Wafer and Bond optics?", 
									"Bondarm Setup", glHMI_MBX_YESNO);
	if (lSelection != glHMI_YES)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	LONG lPickT = m_lPickPos_T;
	LONG lBondT = m_lBondPos_T;
	LONG lWPRdX = 0, lBPRdX = 0;
	LONG lAbsWPRdX = 0, lAbsBPRdX = 0;
	BOOL bPositive = TRUE;
	BOOL bLastPositive = TRUE;
	BOOL bFail = FALSE;
	
	INT nCount = 0;
	DOUBLE dCount = 0;
	INT nMaxCount = 40;				//v4.48A22
	DOUBLE dMaxCount = nMaxCount;
	LONG lOffsetT = 50;
	BOOL bDecc = FALSE;

	do 
	{
//HmiMessage("To PICK .....");

		dCount = nCount;

		if (bPositive != bLastPositive)
		{
			bDecc = TRUE;
		}

		if (nCount == 0)
		{
			lPickT = m_lPickPos_T;
			lBondT = m_lBondPos_T;
		}
		else if (bPositive)
		{
			if (bDecc)
				lOffsetT = (LONG) (lOffsetT * ((dMaxCount - dCount) / dMaxCount));	//v4.48A21
			lPickT = m_lPickPos_T;
			lBondT = lBondT + lOffsetT;
		}
		else
		{
			if (bDecc)
				lOffsetT = (LONG) (lOffsetT * ((dMaxCount - dCount) / dMaxCount));	//v4.48A21
			lPickT = m_lPickPos_T;
			lBondT = lBondT - lOffsetT;
		}

		Z_MoveToHome();
		Z2_MoveToHome();
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(lPickT);
		T_Profile(NORMAL_PROF);
		Z_MoveTo(m_lPickLevel_Z);
		Z2_MoveTo(m_lBondLevel_Z2);
		Sleep(1000);

		SRCH_CHOLE_RESULT stWResult1;
		stWResult1.bStatus = FALSE;
		stWResult1.nOffsetX = 0;
		stWResult1.nOffsetY = 0;
		BOOL bBH2 = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bBH2);

		INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchColletHole", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(SRCH_CHOLE_RESULT), &stWResult1);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (!stWResult1.bStatus)
		{
			HmiMessage("BH#1 Collet Hole at WPR is not found");
			bFail = TRUE;
			break;
		}
		else
		{
			szMsg.Format("WPR BH#1 Collot Hole (%d, %d)", stWResult1.nOffsetX, stWResult1.nOffsetY);
			//HmiMessage(szMsg);
			//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}


		SRCH_CHOLE_RESULT stBResult1;
		stBResult1.bStatus = FALSE;
		stBResult1.nOffsetX = 0;
		stBResult1.nOffsetY = 0;
		
		bBH2 = TRUE;
		stMsg.InitMessage(sizeof(BOOL), &bBH2);

		nConvID = m_comClient.SendRequest(BOND_PR_STN, "SearchColletHole", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(SRCH_CHOLE_RESULT), &stBResult1);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (!stBResult1.bStatus)
		{
			HmiMessage("BH#2 Collet Hole at BPR is not found");
			bFail = TRUE;
			break;
		}
		else
		{
			szMsg.Format("BPR BH#2 Collot Hole (%d, %d)", stBResult1.nOffsetX, stBResult1.nOffsetY);
			//HmiMessage(szMsg);
			//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}

//HmiMessage("To BOND .....");
		szMsg.Format("Cycle #%d: BOND T = %ld, Offset T = %ld (%d) (%.2f)", 
			nCount, lBondT, lOffsetT, bPositive, ((dMaxCount - dCount) / dMaxCount));
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		Z_MoveToHome();
		Z2_MoveToHome();
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(lBondT);
		T_Profile(NORMAL_PROF);
		Z2_MoveTo(m_lPickLevel_Z2);
		Z_MoveTo(m_lBondLevel_Z);
		Sleep(1000);
		
		SRCH_CHOLE_RESULT stWResult2;
		stWResult2.bStatus = FALSE;
		stWResult2.nOffsetX = 0;
		stWResult2.nOffsetY = 0;
		bBH2 = TRUE;
		stMsg.InitMessage(sizeof(BOOL), &bBH2);

		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchColletHole", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(SRCH_CHOLE_RESULT), &stWResult2);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (!stWResult2.bStatus)
		{
			HmiMessage("BH#2 Collet Hole at WPR is not found");
			bFail = TRUE;
			break;
		}
		else
		{
			szMsg.Format("WPR BH#2 Collot Hole (%d, %d)", stWResult2.nOffsetX, stWResult2.nOffsetY);
			//HmiMessage(szMsg);
			//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}


		SRCH_CHOLE_RESULT stBResult2;
		stBResult2.bStatus = FALSE;
		stBResult2.nOffsetX = 0;
		stBResult2.nOffsetY = 0;
		bBH2 = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bBH2);

		nConvID = m_comClient.SendRequest(BOND_PR_STN, "SearchColletHole", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(SRCH_CHOLE_RESULT), &stBResult2);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (!stBResult2.bStatus)
		{
			HmiMessage("BH#1 Collet Hole at BPR is not found");
			bFail = TRUE;
			break;
		}
		else
		{
			szMsg.Format("BPR BH#1 Collot Hole (%d, %d)", stBResult1.nOffsetX, stBResult1.nOffsetY);
			//HmiMessage(szMsg);
			//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}

		lBPRdX = stBResult2.nOffsetX - stBResult1.nOffsetX;
		lWPRdX = stWResult2.nOffsetX - stWResult1.nOffsetX;

		lAbsBPRdX = labs(lBPRdX);
		lAbsWPRdX = labs(lWPRdX);

		szMsg.Format("Cycle #%d: WPR dX = %ld, BPR dX = %ld", nCount+1, lWPRdX, lBPRdX);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//HmiMessage(szMsg);

		bLastPositive = bPositive;
		if (lAbsWPRdX > lAbsBPRdX)
		{
			if (lWPRdX > 0)
				bPositive = TRUE;
			else
				bPositive = FALSE;
		}
		else
		{
			if (lBPRdX > 0)
				bPositive = FALSE;
			else
				bPositive = TRUE;
		}
		if (nCount == 0)
		{
			bLastPositive = bPositive;
		}

		nCount++;
		if (nCount >= nMaxCount)
			break;


	} while (labs(lAbsBPRdX - lAbsWPRdX) > 30);		//v4.48A21


	if (!bFail && (labs(lAbsBPRdX - lAbsWPRdX) <= 30))
	{
		szMsg.Format("BA Calibration OK (Final): WPR dX = %ld, BPR dX = %ld, Cnt = %d, new BOND Pos = %ld", 
			lWPRdX, lBPRdX, nCount+1, lBondT);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage(szMsg);
	}
	else if (nCount == nMaxCount)					//v4.48A22
	{
		szMsg = _T("Pick Bond Position Out of Limit");
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage_Red_Yellow(szMsg);
	}
	else
	{
		szMsg.Format("BA Calibration fail (Final): WPR dX = %ld, BPR dX = %ld, Cnt = %d, final BOND Pos = %ld", 
			lWPRdX, lBPRdX, nCount+1, lBondT);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage_Red_Yellow(szMsg);
	}

	Z_MoveToHome();
	Z2_MoveToHome();
	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lPrePickPos_T);
	T_Profile(NORMAL_PROF);
	HmiMessage("BH Collet Hole test is done.");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::NGPick_IndexReelT(IPC_CServiceMessage &svMsg)
{
	if (IsESMachine())
	{
		if (!NGPickT_IsPowerOn())
		{
			NGPickT_PowerOn(TRUE);
			Sleep(100);
		}
		NGPickT_Move(m_lNGPickIndexT);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::NGPick_HomeReelT(IPC_CServiceMessage &svMsg)
{
	NGPickT_Home();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


// Auto Clean Collet 
LONG CBondHead::SetColletCleanPocket(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetCleanColletPocket(bOn);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BHSyringeOnce(IPC_CServiceMessage &svMsg)
{
	CleanColletPocketAlcoholOnce();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::BHAccTest(IPC_CServiceMessage &svMsg)
{
//	INT nConvID;
	IPC_CServiceMessage stMsg;
	BOOL bReturn = TRUE;

	//No Collet in Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

	BOOL bBTJSOn = FALSE;
	BT_Set_JoyStick(FALSE);

	//v4.01T1
	//BH protection
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_fHardware == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_bDisableBH)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (IsAllMotorsEnable() == FALSE)
	{
		SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
		SetErrorMessage("BondArm module is off power");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsCoverOpen())
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		SetErrorMessage("Machine cover open when do auto clean collet test");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szContents, szTitle;
	CStringList szList;
	szContents = "Please select Testing Collet!";
	szTitle = "Auto Clean Collet";
	szList.AddTail("Arm 1 Collet Auto Clean Test");
	szList.AddTail("Arm 2 Collet Auto Clean Test");
	LONG lSelection = HmiSelection(szContents, szTitle, szList, 0);
	BOOL bOldValue = m_bBHZ2TowardsPick;
	
	// IN AUTO MODE, THE STATE NOT REVERSE YET, BUT DIE BONDED,
	switch (lSelection)
	{
		case 0:
			m_bBHZ2TowardsPick = TRUE;
			break;
		case 1:
			m_bBHZ2TowardsPick = FALSE;
			break;
		default:
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
	}

	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	{
		if (pBinTable && pBinTable->MoveToNextLastDiePosn() == FALSE)
		{
			HmiMessage("BT Table Move Error");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	OpMS100AutoCleanCollet(IsBHZ1ToPick());
	m_bBHZ2TowardsPick = bOldValue;	//	IPC command test ACC

	T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
	HmiMessage("Auto Clean Collet test complete");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ACCMoveToPosnT(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	Z_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
	Sleep(50);
	//v4.53A14
	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lAutoCleanCollet_T);
	T_Profile(NORMAL_PROF);
	//T_MoveTo(m_lAutoCleanCollet_T, SFM_WAIT);
	Sleep(50);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_UpdateACCData(IPC_CServiceMessage &svMsg)
{
	m_lBPAccPosT = m_lAutoCleanCollet_T;
	m_lBPAccLvlZ = m_lAutoCleanCollet_Z;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondHead::BH_BlockButtonCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

#ifdef NUM_MOTION
	HmiMessage(" Dual Arm use bond position to clean collet.\n no need to teach it!");
	bReturn = FALSE;
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_SetSuckingHead(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetSuckingHead(bOn);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//---------------------------- 
//--- Diagnostic Functions ---
//---------------------------- 
/*** Power On ***/
LONG CBondHead::Diag_PowerOn_T(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	T_PowerOn(bOn);
	CloseLoadingAlert();

	if (IsMS60() && T_IsPowerOn())
	{
		InitMS60ThermalCtrl();
	}
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_Z(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	Z_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_Z2(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	Z2_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_Ej(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	Ej_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_EjCap(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	EjCap_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_EjT(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	EjT_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_EjX(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	EjX_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_EjY(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	EjY_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_EjElevator(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	EjElevator_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_BinEj(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	BinEj_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_BinEjCap(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	BinEjCap_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_BinEjT(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	BinEjT_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_BinEjElevator(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	BinEjElevator_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::Diag_PowerOn_ChgColletT(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	MS50ChgColletT_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_ChgColletZ(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	MS50ChgColletZ_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_PowerOn_All(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	T_PowerOn(bOn);
	Z_PowerOn(bOn);
	Z2_PowerOn(bOn);
	Ej_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

/*** Commutate ***/
LONG CBondHead::Diag_Comm_T(IPC_CServiceMessage &svMsg)
{
	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	BOOL bReturn = TRUE;

	//Check ExArm position to avoid crashing with Bond Arm
	BOOL bBinLoaderEnabled = IsBLEnable();

	StartLoadingAlert();
	T_Comm();
	CloseLoadingAlert();
	
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Comm_Z(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	Z_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Comm_Z2(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	Z2_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Comm_Ej(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	Ej_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Comm_EjT(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	EjT_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Comm_EjCap(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	EjCap_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Comm_BinEj(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	BinEj_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Comm_BinEjT(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	BinEjT_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Comm_BinEjCap(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	BinEjCap_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


/*** Home ***/
LONG CBondHead::Diag_Home_T(IPC_CServiceMessage &svMsg)
{
	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	BOOL bReturn = TRUE;

	//Check ExArm position to avoid crashing with Bond Arm
	BOOL bBinLoaderEnabled = IsBLEnable();

	StartLoadingAlert();

	//v4.34T9
	if (Z_IsPowerOn())
	{
		Z_MoveTo(0);

		//v4.46T20	//Cree
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
		//{
			if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
			{
				Sleep(100);
				Z_Move(pApp->GetBHZ1HomeOffset());
			}
		//}
	}
	else
	{
		Z_Home();
	}
	if (m_bIsArm2Exist)
	{
		if (Z2_IsPowerOn())
		{
			Z2_MoveTo(0);

			//v4.46T20	//Cree
			CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
			//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
			//{
				if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
				{
					Sleep(100);
					Z2_Move(pApp->GetBHZ2HomeOffset());
				}
			//}
		}
		else
		{
			Z2_Home();
		}
	}
	if (Ej_IsPowerOn())
	{
		Ej_MoveTo(0);
	}
	else
	{
		Ej_Home();
	}

/*
	//v4.48A10
	if (IsWLExpanderOpen())
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//v4.54A5		//Check BT frame level
	//BOOL bBinLoader	= IsBLEnable();
	if (bBinLoaderEnabled && !IsBT1FrameLevel())
	{
		SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
		SetErrorMessage("BT frame level not DOWN");
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!CheckWExpanderSafeToMove())
	{
		SetAlert_Red_Yellow(IDS_MS_EXPANDER_OPEN);
	}
	else
	{
		T_Home();
	}
*/
	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		CloseLoadingAlert();

		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	T_Home();

	CloseLoadingAlert();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_Z(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	//v4.51A17
	if (Ej_IsPowerOn())
	{
		Ej_MoveTo(0);
	}
	else
	{
		Ej_Home();
	}	
	Z_Home();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_Z2(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	//v4.51A17
	if (Ej_IsPowerOn())
	{
		Ej_MoveTo(0);
	}
	else
	{
		Ej_Home();
	}	
	Z2_Home();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_Ej(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	Ej_Home();
	CloseLoadingAlert();
	
	//if (!IsMS60EjtHome())	//v4.55A6
	//{
	//	HmiMessage_Red_Back("EJ Z motor is not HOME properly.");
	//}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_EjT(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	EjT_Home();
	CloseLoadingAlert();
	
	//if (!IsMS60EjtHome())	//v4.55A6
	//{
	//	HmiMessage_Red_Back("EJ Z motor is not HOME properly.");
	//}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_EjCap(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	EjCap_Home();
	CloseLoadingAlert();
	
	EjCap_MoveTo(1000);	//andrewng //2020-0831	//2020-0903

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::Diag_Home_EjX(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	EjX_Home();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_EjY(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	EjY_Home();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_EjElevator(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	EjElevator_Home();
	CloseLoadingAlert();
	
	// Move To non hit WT position
	//EjElevator_MoveTo(-33500);
	if (EjElevator_IsPowerOn())
	{
		if (m_dEjElevatorRes == 1.00)
		{
			//EjElevator_MoveTo(-10000);
		}
		else
		{
			//EjElevator_MoveTo(-20000);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::Diag_Home_BinEj(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	BinEj_Home();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_BinEjT(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	BinEjT_Home();
	CloseLoadingAlert();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_BinEjCap(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	BinEjCap_Home();
	CloseLoadingAlert();

	BinEjCap_MoveTo(1000);	//andrewng //2020-0831	//2020-0903

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::Diag_Home_BinEjElevator(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	BinEjElevator_Home();
	CloseLoadingAlert();
	
	// Move To non hit BT position
	//BinEjElevator_MoveTo(-38500);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_MS50ChgColletT(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	MS50ChgColletT_Home();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_MS50ChgColletZ(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	MS50ChgColletZ_Home();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

/*** Move ***/
LONG CBondHead::Diag_Move_T(IPC_CServiceMessage &svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

//	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	BOOL bReturn = TRUE;

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();
	T_Move(lPos);
	CloseLoadingAlert();
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_Z(IPC_CServiceMessage &svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	StartLoadingAlert();
	Z_Move(lPos);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_Z2(IPC_CServiceMessage &svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	StartLoadingAlert();
	Z2_Move(lPos);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_Ej(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (!m_bEnableEjectorTheta)		//If old NVC Test Jig
	{
		lPos = -1 * lPos;
	}

	LONG lMaxDist = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
	LONG lMinDist = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);
	GetEncoderValue();

	CString szLog;
	szLog.Format("EJ Diag MOVE = %ld, Enc = %ld, (%ld, %ld)",  lPos, m_lEnc_Ej, lMinDist, lMaxDist);
	//HmiMessage(szLog);

	if ( (lMinDist <= m_lEnc_Ej + lPos) && (m_lEnc_Ej + lPos <= lMaxDist) )
	{
		if (m_lMotionCycle > 0)
		{
			LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, TRUE, 5, m_stDataLog);

			m_lMotionOrigEncPosn	= m_lEnc_Ej;
			m_lMotionTarEncPosn		= m_lEnc_Ej + lPos;
			m_lMotionCycleCount		= 0;
			m_bMoveDirection		= TRUE;
			m_bMotionCycleStop		= FALSE;
			m_ucMotionAxis = 1;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
		}
		else
		{
			Ej_Move(lPos);
		}
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_NegMove_Ej(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	lPos = -1 * lPos;

	LONG lMaxDist = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
	LONG lMinDist = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);
	GetEncoderValue();

	CString szLog;
	szLog.Format("EJ Diag NegMOVE = %ld, Enc = %ld, (%ld, %ld)",  lPos, m_lEnc_Ej, lMinDist, lMaxDist);
	//HmiMessage(szLog);

	if ( (lMinDist <= m_lEnc_Ej + lPos) && (m_lEnc_Ej + lPos <= lMaxDist) )
	{
		if (m_lMotionCycle > 0)
		{
			LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, TRUE, 5, m_stDataLog);

			m_lMotionOrigEncPosn	= m_lEnc_Ej;
			m_lMotionTarEncPosn		= m_lEnc_Ej + lPos;
			m_lMotionCycleCount		= 0;
			m_bMoveDirection		= TRUE;
			m_bMotionCycleStop		= FALSE;
			m_ucMotionAxis = 1;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
		}
		else
		{
			Ej_Move(lPos);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_EjT(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (!m_bEnableEjectorTheta)		//If old NVC Test Jig
	{
		lPos = -1 * lPos;
	}

	GetEncoderValue();

	CString szLog;
	szLog.Format("EjT Diag MOVE T = %ld, Enc = %ld, MotionCycle = %ld, MotionAxis = %d", 
					lPos, m_lEnc_EjT, m_lMotionCycle, m_ucMotionAxis);

	if (m_lMotionCycle > 0)
	{
		HmiMessage(szLog);
		LogAxisPerformance(BH_AXIS_EJ_T, &m_stBHAxis_EjT, TRUE, 5, m_stDataLog);

		m_lMotionOrigEncPosn	= m_lEnc_EjT;
		m_lMotionTarEncPosn		= m_lEnc_EjT + lPos;
		m_lMotionCycleCount		= 0;
		m_bMoveDirection		= TRUE;
		m_bMotionCycleStop		= FALSE;
		m_ucMotionAxis = 2;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
	}
	else
	{
		EjT_MoveTo(lPos + m_lEnc_EjT);
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_NegMove_EjT(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	lPos = -1 * lPos;
	GetEncoderValue();

	CString szLog;
	szLog.Format("EjT Diag NegMOVE T = %ld, Enc = %ld, MotionCycle = %ld, MotionAxis = %d", 
		lPos, m_lEnc_EjT, m_lMotionCycle, m_ucMotionAxis);

	if (m_lMotionCycle > 0)
	{
		HmiMessage(szLog);
		LogAxisPerformance(BH_AXIS_EJ_T, &m_stBHAxis_EjT, TRUE, 5, m_stDataLog);

		m_lMotionOrigEncPosn	= m_lEnc_EjT;
		m_lMotionTarEncPosn		= m_lEnc_EjT + lPos;
		m_lMotionCycleCount		= 0;
		m_bMoveDirection		= TRUE;
		m_bMotionCycleStop		= FALSE;
		m_ucMotionAxis = 2;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
	}
	else
	{
		EjT_MoveTo(lPos + m_lEnc_EjT);
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_EjCap(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (!m_bEnableEjectorTheta)		//If old NVC Test Jig
	{
		lPos = -1 * lPos;
	}

	GetEncoderValue();

	CString szLog;
	szLog.Format("EjCap Diag MOVE = %ld, Enc = %ld, MotionCycle = %ld, MotionAxis = %d", 
		lPos, m_lEnc_EjCap, m_lMotionCycle, m_ucMotionAxis);

	if (m_lMotionCycle > 0)
	{
		LogAxisPerformance(BH_AXIS_EJCAP, &m_stBHAxis_EjCap, TRUE, 5, m_stDataLog);

		m_lMotionOrigEncPosn	= m_lEnc_EjCap;
		m_lMotionTarEncPosn		= m_lEnc_EjCap + lPos;
		m_lMotionCycleCount		= 0;
		m_bMoveDirection		= TRUE;
		m_bMotionCycleStop		= FALSE;
		m_ucMotionAxis = 3;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
	}
	else
	{
		EjCap_MoveTo(lPos + m_lEnc_EjCap);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_NegMove_EjCap(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	lPos = -1 * lPos;

	GetEncoderValue();

	CString szLog;
	szLog.Format("EjCap Diag NegMOVE = %ld, Enc = %ld, MotionCycle = %ld, MotionAxis = %d", 
		lPos, m_lEnc_EjCap, m_lMotionCycle, m_ucMotionAxis);

	if (m_lMotionCycle > 0)
	{
		LogAxisPerformance(BH_AXIS_EJCAP, &m_stBHAxis_EjCap, TRUE, 5, m_stDataLog);

		m_lMotionOrigEncPosn	= m_lEnc_EjCap;
		m_lMotionTarEncPosn		= m_lEnc_EjCap + lPos;
		m_lMotionCycleCount		= 0;
		m_bMoveDirection		= TRUE;
		m_bMotionCycleStop		= FALSE;
		m_ucMotionAxis = 3;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
	}
	else
	{
		EjCap_MoveTo(lPos + m_lEnc_EjCap);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_EjX(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_EjY(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_EjElevator(IPC_CServiceMessage &svMsg)
{
	CString szLog;
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (!m_bEnableEjectorTheta)		//If old NVC Test Jig
	{
		lPos = -1 * lPos;
	}

	if (m_bMS60EjElevator && EjElevator_IsPowerOn())
	{
		GetEncoderValue();
		LONG lEncoder = m_lEnc_EjElevatorZ;

		LONG lMaxDist = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
		LONG lMinDist = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);

		szLog.Format("Ej Elevator Diag MOVE = %ld + %ld, (%ld, %ld)", lEncoder, lPos, lMinDist, lMaxDist);
		//HmiMessage(szLog);

		if ( (lMinDist <= lEncoder + lPos) && (lEncoder + lPos <= lMaxDist) )
		{
			if (m_lMotionCycle > 0)
			{
				LogAxisPerformance(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator, TRUE, 5, m_stDataLog);

				m_lMotionOrigEncPosn	= lEncoder;
				m_lMotionTarEncPosn		= lEncoder + lPos;
				m_lMotionCycleCount		= 0;
				m_bMoveDirection		= TRUE;
				m_bMotionCycleStop		= FALSE;
				m_ucMotionAxis = 4;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
			
				CString szMsg;
				szMsg.Format("Cycle +MOVE start ... : AXIS=EjElev, ENC=%ld, Pos1=%ld, Pos2=%ld, MOVE-DIST=%ld",
						lEncoder, m_lMotionOrigEncPosn, m_lMotionTarEncPosn, lPos);
HmiMessage(szMsg);			
			}
			else
			{
				CString szMsg;
				szMsg.Format("Single +MOVETO: AXIS=EjElev, ENC=%ld, TarPos=%ld, MOVE-DIST=%ld",
						lEncoder, lEncoder + lPos, lPos);
HmiMessage(szMsg);

				EjElevator_MoveTo(lEncoder + lPos);
			}
		}
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_NegMove_EjElevator(IPC_CServiceMessage &svMsg)
{
	CString szLog;
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	//lPos = -1 * lPos;

	if (m_bMS60EjElevator && EjElevator_IsPowerOn())
	{
		GetEncoderValue();
		LONG lEncoder = m_lEnc_EjElevatorZ;

		LONG lMaxDist = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
		LONG lMinDist = GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);

		szLog.Format("Ej Elevator Diag NegMOVE = %ld + %ld, (%ld, %ld)", lEncoder, lPos, lMinDist, lMaxDist);
		//HmiMessage(szLog);

		if ( (lMinDist <= (lEncoder - lPos)) && ((lEncoder - lPos) <= lMaxDist) )
		{
			if (m_lMotionCycle > 0)
			{
				LogAxisPerformance(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator, TRUE, 5, m_stDataLog);
				
				m_lMotionOrigEncPosn	= lEncoder;
				m_lMotionTarEncPosn		= lEncoder - lPos;
				m_lMotionCycleCount		= 0;
				m_bMoveDirection		= TRUE;
				m_bMotionCycleStop		= FALSE;
				m_ucMotionAxis = 4;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
			
				CString szMsg;
				szMsg.Format("Cycle -MOVE start ... : AXIS=EjElev, ENC=%ld, Pos1=%ld, Pos2=%ld, MOVE-DIST=%ld",
						lEncoder, m_lMotionOrigEncPosn, m_lMotionTarEncPosn, lPos);
HmiMessage(szMsg);
			}
			else
			{
				CString szMsg;
				szMsg.Format("Single -MOVETO: AXIS=EjElev, ENC=%ld, TarPos=%ld, MOVE-DIST=%ld",
						lEncoder, lEncoder - lPos, lPos);
HmiMessage(szMsg);

				EjElevator_MoveTo(lEncoder - lPos);
			}
		}
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_BinEj(IPC_CServiceMessage &svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (!m_bEnableEjectorTheta)		//If old NVC Test Jig
	{
		lPos = -1 * lPos;
	}

	LONG lMaxDist = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
	LONG lMinDist = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);
	GetEncoderValue();

	CString szLog;
	szLog.Format("Bin EJ Diag MOVE = %ld, Enc = %ld (%ld, %ld)",  lPos, m_lEnc_BinEj, lMinDist, lMaxDist);
	//HmiMessage(szLog);

	if ( (lMinDist <= m_lEnc_BinEj + lPos) && (m_lEnc_BinEj + lPos <= lMaxDist) )
	{
		if (m_lMotionCycle > 0)
		{
			LogAxisPerformance(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj, TRUE, 5, m_stDataLog);
			
			m_lMotionOrigEncPosn	= m_lEnc_BinEj;
			m_lMotionTarEncPosn		= m_lEnc_BinEj + lPos;
			m_lMotionCycleCount		= 0;
			m_bMoveDirection		= TRUE;
			m_bMotionCycleStop		= FALSE;
			m_ucMotionAxis = 5;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
		}
		else
		{
			BinEj_Move(lPos);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_NegMove_BinEj(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	lPos = -1 * lPos;

	LONG lMaxDist = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								
	LONG lMinDist = GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR, MS896A_CFG_CH_MIN_DISTANCE);
	GetEncoderValue();

	CString szLog;
	szLog.Format("Bin EJ Diag NegMOVE = %ld, Enc = %ld (%ld, %ld)",  lPos, m_lEnc_BinEj, lMinDist, lMaxDist);
	//HmiMessage(szLog);

	if ( (lMinDist <= m_lEnc_BinEj + lPos) && (m_lEnc_BinEj + lPos <= lMaxDist) )
	{
		if (m_lMotionCycle > 0)
		{
			HmiMessage(szLog);
			LogAxisPerformance(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj, TRUE, 5, m_stDataLog);
			
			m_lMotionOrigEncPosn	= m_lEnc_BinEj;
			m_lMotionTarEncPosn		= m_lEnc_BinEj + lPos;
			m_lMotionCycleCount		= 0;
			m_bMoveDirection		= TRUE;
			m_bMotionCycleStop		= FALSE;
			m_ucMotionAxis = 5;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
		}
		else
		{
			BinEj_Move(lPos);
		}
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_BinEjT(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (!m_bEnableEjectorTheta)		//If old NVC Test Jig
	{
		lPos = -1 * lPos;
	}

	GetEncoderValue();

	CString szLog;
	szLog.Format("EjT Diag MOVE T = %ld, Enc = %ld, MotionCycle = %ld, MotionAxis = %d", 
		lPos, m_lEnc_BinEjT, m_lMotionCycle, m_ucMotionAxis);

	if (m_lMotionCycle > 0)
	{
		HmiMessage(szLog);
		LogAxisPerformance(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT, TRUE, 5, m_stDataLog);

		m_lMotionOrigEncPosn	= m_lEnc_BinEjT;
		m_lMotionTarEncPosn		= m_lEnc_BinEjT + lPos;
		m_lMotionCycleCount		= 0;
		m_bMoveDirection		= TRUE;
		m_bMotionCycleStop		= FALSE;
		m_ucMotionAxis = 6;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
	}
	else
	{
		BinEjT_MoveTo(lPos + m_lEnc_BinEjT);
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_NegMove_BinEjT(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	lPos = -1 * lPos;

	GetEncoderValue();

	CString szLog;
	szLog.Format("EjT Diag NegMOVE T = %ld, Enc = %ld, MotionCycle = %ld, MotionAxis = %d", 
		lPos, m_lEnc_BinEjT, m_lMotionCycle, m_ucMotionAxis);

	if (m_lMotionCycle > 0)
	{
		HmiMessage(szLog);
		LogAxisPerformance(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT, TRUE, 5, m_stDataLog);

		m_lMotionOrigEncPosn	= m_lEnc_BinEjT;
		m_lMotionTarEncPosn		= m_lEnc_BinEjT + lPos;
		m_lMotionCycleCount		= 0;
		m_bMoveDirection		= TRUE;
		m_bMotionCycleStop		= FALSE;
		m_ucMotionAxis = 6;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
	}
	else
	{
		BinEjT_MoveTo(lPos + m_lEnc_BinEjT);
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_BinEjCap(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (!m_bEnableEjectorTheta)		//If old NVC Test Jig
	{
		lPos = -1 * lPos;
	}

	GetEncoderValue();

	if (m_lMotionCycle > 0)
	{
		LogAxisPerformance(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap, TRUE, 5, m_stDataLog);
		
		m_lMotionOrigEncPosn	= m_lEnc_BinEjCap;
		m_lMotionTarEncPosn		= m_lEnc_BinEjCap + lPos;
		m_lMotionCycleCount		= 0;
		m_bMoveDirection		= TRUE;
		m_bMotionCycleStop		= FALSE;
		m_ucMotionAxis = 7;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
	}
	else
	{
		BinEjCap_MoveTo(lPos + m_lEnc_BinEjCap);
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_NegMove_BinEjCap(IPC_CServiceMessage &svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	lPos = -1 * lPos;

	GetEncoderValue();

	if (m_lMotionCycle > 0)
	{
		LogAxisPerformance(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap, TRUE, 5, m_stDataLog);
		
		m_lMotionOrigEncPosn	= m_lEnc_BinEjCap;
		m_lMotionTarEncPosn		= m_lEnc_BinEjCap + lPos;
		m_lMotionCycleCount		= 0;
		m_bMoveDirection		= TRUE;
		m_bMotionCycleStop		= FALSE;
		m_ucMotionAxis = 7;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev
	}
	else
	{
		BinEjCap_MoveTo(lPos + m_lEnc_BinEjCap);
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_BinEjElevator(IPC_CServiceMessage &svMsg)
{
	CString szLog;
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (!m_bEnableEjectorTheta)		//If old NVC Test Jig
	{
		lPos = -1 * lPos;
	}

	if (m_bMS60EjElevator && BinEjElevator_IsPowerOn())
	{
		GetEncoderValue();
		LONG lEncoder = m_lEnc_BinEjElevatorZ;

		LONG lMaxDist = GetChannelInformation(MS896A_CFG_CH_BIN_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
		LONG lMinDist = GetChannelInformation(MS896A_CFG_CH_BIN_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);

		szLog.Format("BinEj Elevator Diag MOVE = %ld + %ld, (%ld, %ld)", lEncoder, lPos, lMinDist, lMaxDist);
		//HmiMessage(szLog);

		if ( (lMinDist <= lEncoder + lPos) && (lEncoder + lPos <= lMaxDist) )
		{
			if (m_lMotionCycle > 0)
			{
				LogAxisPerformance(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator, TRUE, 5, m_stDataLog);
		
				m_lMotionOrigEncPosn	= lEncoder;
				m_lMotionTarEncPosn		= lEncoder + lPos;
				m_lMotionCycleCount		= 0;
				m_bMoveDirection		= TRUE;
				m_bMotionCycleStop		= FALSE;
				m_ucMotionAxis = 8;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev

				CString szMsg;
				szMsg.Format("Cycle +MOVE start ... : AXIS=BinEjElev, ENC=%ld, Pos1=%ld, Pos2=%ld, MOVE-DIST=%ld",
						lEncoder, m_lMotionOrigEncPosn, m_lMotionTarEncPosn, lPos);
HmiMessage(szMsg);

			}
			else
			{
				CString szMsg;
				szMsg.Format("Single +MOVETO: AXIS=BinEjElev, ENC=%ld, TarPos=%ld, MOVE-DIST=%ld",
						lEncoder, lEncoder + lPos, lPos);
HmiMessage(szMsg);

				BinEjElevator_MoveTo(lEncoder + lPos);
			}
		}
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_NegMove_BinEjElevator(IPC_CServiceMessage &svMsg)
{
	CString szLog;
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	//lPos = -1 * lPos;

	if (m_bMS60EjElevator && BinEjElevator_IsPowerOn())
	{
		GetEncoderValue();
		LONG lEncoder = m_lEnc_BinEjElevatorZ;

		LONG lMaxDist = GetChannelInformation(MS896A_CFG_CH_BIN_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								
		LONG lMinDist = GetChannelInformation(MS896A_CFG_CH_BIN_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);

		szLog.Format("BinEj Elevator Diag NegMOVE = %ld + %ld, (%ld, %ld)", lEncoder, lPos, lMinDist, lMaxDist);
		//HmiMessage(szLog);

		if ( (lMinDist <= lEncoder - lPos) && (lEncoder - lPos <= lMaxDist) )
		{
			if (m_lMotionCycle > 0)
			{
				LogAxisPerformance(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator, TRUE, 5, m_stDataLog);
		
				m_lMotionOrigEncPosn	= lEncoder;
				m_lMotionTarEncPosn		= lEncoder - lPos;
				m_lMotionCycleCount		= 0;
				m_bMoveDirection		= TRUE;
				m_bMotionCycleStop		= FALSE;
				m_ucMotionAxis = 8;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev

				CString szMsg;
				szMsg.Format("Cycle -MOVE start ... : AXIS=BinEjElev, ENC=%ld, Pos1=%ld, Pos2=%ld, MOVE-DIST=%ld",
						lEncoder, m_lMotionOrigEncPosn, m_lMotionTarEncPosn, lPos);
HmiMessage(szMsg);

			}
			else
			{
				CString szMsg;
				szMsg.Format("Single -MOVETO: AXIS=BinEjElev, ENC=%ld, TarPos=%ld, MOVE-DIST=%ld",
						lEncoder, lEncoder - lPos, lPos);
HmiMessage(szMsg);

				BinEjElevator_MoveTo(lEncoder - lPos);
			}
		}
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_MS50ChgColletT(IPC_CServiceMessage &svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

#ifdef NU_MOTION
	lPos = -1 * lPos;
#endif

	MS50ChgColletT_Move(lPos);
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_MS50ChgColletZ(IPC_CServiceMessage &svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

#ifdef NU_MOTION
	lPos = -1 * lPos;
#endif

	MS50ChgColletZ_Move(lPos);
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::UpdateOutput(IPC_CServiceMessage &svMsg)
{
	BOOL bUpdate = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bUpdate);

	m_bUpdateOutput = bUpdate;

	bUpdate = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bUpdate);
	return 1;
}

LONG CBondHead::CheckPreBondChecking(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->m_bOperator == TRUE)
	{
		m_bCheckCoverSensor = TRUE;
		(*m_psmfSRam)["BondHead"]["EnableCoverSensor"] = TRUE;
		SaveBhData();
	}

	//MS60/MS90 using digital air-flow sensor
	if (m_bMS100DigitalAirFlowSnr && IsBurnIn()==FALSE)
	{
		RuntimeUpdateMissingDieThresholdValue(TRUE);
		RuntimeUpdateMissingDieThresholdValue(FALSE);
		if (!CheckDigitalAirFlowThresholdLimit(FALSE))
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		if (!CheckDigitalAirFlowThresholdLimit(TRUE))
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		//Check un-clogged values before running AUTOBOND
		SetPickVacuum(TRUE);
		SetPickVacuumZ2(TRUE);
		Sleep(500);

		if (!IsMissingDie() || !IsMissingDieZ2())
		{
			Sleep(1000);
			if (!IsMissingDie() || !IsMissingDieZ2())
			{
				Sleep(2000);
				if (!IsMissingDie() || !IsMissingDieZ2())
				{
					CString szMsg, szLog, szTemp;

					if (!IsMissingDie())
					{
						szLog.Format("BHZ1 collet is clogged (%lu, Thres = %lu);",
							m_ulBHZ1DAirFlowValue, m_ulBHZ1DAirFlowThreshold);
						szMsg = szLog;
					}
					if (!IsMissingDieZ2())
					{
						szTemp.Format("BHZ2 collet is clogged (%lu, Thres = %lu);",
							m_ulBHZ2DAirFlowValue, m_ulBHZ2DAirFlowThreshold);
						szMsg = szMsg + "\n" + szTemp;
						szLog = szLog + szTemp;
					}
					szTemp = "Please clean or exchange collet.";
					szMsg = szMsg + "\n" + szTemp;
					szLog = szLog + szTemp;

					SetPickVacuum(FALSE);
					SetPickVacuumZ2(FALSE);
					Sleep(500);
					SetErrorMessage(szLog);

					BOOL bColletCleanPocket = pApp->GetFeatureStatus(MS896A_FUNC_COLLET_CLEAN_POCKET);
					if (m_bAutoCleanCollet == TRUE && bColletCleanPocket)
					{
						m_dLastPickTime				= 0;
					}
					else
					{
						HmiMessage_Red_Yellow(szMsg);

						bReturn = FALSE;
						svMsg.InitMessage(sizeof(BOOL), &bReturn);
						return 1;
					}
				}
			}
		}
		SetPickVacuum(FALSE);
		SetPickVacuumZ2(FALSE);
	}


	//v4.50A9	//MS60/MS90
	if (CMS896AStn::m_bDBHThermostat)	//v4.50A12
	{
		/*	//v4.59A10
		if (!m_bMS60ThermalCtrl || !m_bEnableMS60ThermalCheck)
		{
			CString szErr;
			if (m_ulBHTThermostatReading == 0)
			{
				szErr = "Please make sure that the BH thermo couple is installed properly";
				SetErrorMessage(szErr);
				HmiMessage_Red_Yellow(szErr);
				
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
			else	//v4.59A5
			{
				szErr = "WARNING: BondHead thermal function is not enabled ...";
				SetErrorMessage(szErr);
				HmiMessage_Red_Yellow(szErr);
			}
		}*/

		if (IsMS60TempertureOverHeated(MS60_BH_MAX_TEMPERATURE_AUTOBOND - 5))	//v4.59A18
		{
			CString szErr;
			szErr.Format("BondHead Temperature is overheat (%ld) at PRESTART; operation is aborted", 
				m_ulBHTThermostatReading);
			SetErrorMessage(szErr);
			HmiMessage_Red_Yellow("BondHead Temperature is overheat!  Machine is aborted!");
			
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}


	//v4.51A4	//AutoChangeCollet Fcn
	if (IsEnaAutoChangeCollet() && !m_bDisableBH && !IsBurnIn())
	{
		CString szErr;

		if (m_bAutoChgCollet1Fail || m_bAutoChgCollet2Fail)
		{
			if (m_bAutoChgCollet1Fail)
				szErr = "BH: Auto-Change-Collet #1 fails; please Manual-SearchCollet-1 and try again";
			else
				szErr = "BH: Auto-Change-Collet #2 fails; please Manual-SearchCollet-2 and try again";
			SetErrorMessage(szErr);
			HmiMessage_Red_Yellow(szErr);
			
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}
	else if (IsEnaAutoChangeCollet() && IsBurnIn())
	{
		m_bAutoChgCollet1Fail = FALSE;
		m_bAutoChgCollet2Fail = FALSE;
	}

	if (pApp->GetCustomerName() == CTM_SANAN)
	{
		CString szPackageName = (*m_psmfSRam)["MS896A"]["PKG Filename"];//M68
		if ( szPackageName.Find("08F") != -1)
		{
			m_bCheckK1K2 = TRUE;
		}

		if ( m_bCheckK1K2 == TRUE)//M68
		{
			if ((m_lEjectorKOffset == 0 || m_lEjectorKOffsetForBHZ1MD == 0 || m_lEjectorKOffsetForBHZ2MD == 0)
				&&(m_lEjectorKOffset == 0 ||m_lEjSubRegionSKOffset == 0 || m_lEjSubRegionSKCount == 0))
			{
				HmiMessage_Red_Yellow("Please enable Ejector K offset function");
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
	
}

LONG CBondHead::CheckPreBondStatus(IPC_CServiceMessage &svMsg)
{
	BOOL bUpdate = TRUE;

	//No BHT in Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bUpdate);
	return TRUE;

	if (State() != IDLE_Q) 
	{
		bUpdate = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bUpdate);
		return 1;
	}

	if (m_fHardware == FALSE)
	{
		bUpdate = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bUpdate);
		return 1;
	}

	if (m_bDisableBH)		//v3.60
	{
		bUpdate = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bUpdate);
		return 1;
	}
		
	if (IsLowPressure())
	{
		SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);
		SetErrorMessage("Machine pressure is low");
		bUpdate = FALSE;
	}

	if (IsLowVacuumFlow())
	{
		SetAlert_Red_Yellow(IDS_BH_LOW_VACUUM_FLOW);
		SetErrorMessage("Machine low vacuum flow at prebond");
		bUpdate = FALSE;
	}

	if (IsCoverOpen())		//v2.72a5
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		SetErrorMessage("Machine cover open");
		bUpdate = FALSE;
	}
	
	//v4.011T3	//Tyntek to handle motion error sequence hangup
	if (!Ej_IsPowerOn())
	{
		SetErrorMessage("Ejector not power on in CheckPreBondStatus");
		CString szErr = "\nEjector is not power on!";
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		bUpdate = FALSE;
	}

	if (m_bSel_Ej)
	{
		if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_EJ, &m_stBHAxis_Ej) != 0)
		{
			SetErrorMessage("Ejector motion error in CheckPreBondStatus");
			CString szErr = "\nEjector motion error!  Please HOME motor and try again.";
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
			bUpdate = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bUpdate);
	return 1;
}

LONG CBondHead::StartAirFlowSensorTest(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;

	BOOL bReturn = TRUE;

	szTitle.LoadString(HMB_BH_AIR_FLOW_SENSOR_RESPONSE_TEST);
	szContent.LoadString(HMB_BH_AIR_FLOW_SENSOR_RESPONSE_TEST_START);
	
	HmiMessage(szContent, szTitle);

	DOUBLE dStartTime = GetTime();
	DOUBLE dStopTime = dStartTime;
	DOUBLE dResponseTime;
	// Open Pick Vacuum
	SetPickVacuum(TRUE);
	dStartTime = GetTime();
	dStopTime = dStartTime;

	BOOL bToggle = FALSE;

	while (1)
	{	

		if (IsMissingDie() == TRUE)
		{
			bToggle = TRUE;
		}

		if ((IsMissingDie() == FALSE) && (bToggle == TRUE))
		{
			dStopTime = GetTime();
			dResponseTime = dStopTime - dStartTime;
			CString szResponseTime;
			szResponseTime.Format("Missing Die Sensor Response Time:%f ms", dResponseTime);
			HmiMessage(szResponseTime, szTitle);
			break;
		}

		dStopTime = GetTime();
		dResponseTime = dStopTime - dStartTime;
		
		if (dResponseTime >= 1000)
		{
			szContent.LoadString(HMB_BH_AIR_FLOW_SENSOR_RESPONSE_TIME_OUT);
			HmiMessage(szContent, szTitle);
			break;
		}
	}

	SetPickVacuum(FALSE);
	
	svMsg.InitMessage(sizeof(BOOL), & bReturn);
	
	return 1;
}



LONG CBondHead::StartTDacTest(IPC_CServiceMessage &svMsg)
{
	
	if (State() != IDLE_Q)
	{
		return 1;
	}

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		return TRUE;
	}

	Z_MoveTo(0);

	//v4.46T20	//Cree
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
	//{
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
	//}

	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lPickPos_T);
	T_Profile(NORMAL_PROF);
	m_bBhTDacForwardMove = TRUE;
	m_szSqOfDacFromPickToBond = "0";
	m_szSqOfDacFromBondToPick = "0";
	m_dSqOfDacFromPickToBond = 0;
	m_dSqOfDacFromBondToPick = 0;
	m_szSumOfDac = "0";
	Sleep(500);
	m_bStartDacTTest = TRUE;

	return 1;

}


LONG CBondHead::StopTDacTest(IPC_CServiceMessage &svMsg)
{
	m_bStartDacTTest = FALSE;
	
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		return TRUE;
	}

	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lPrePickPos_T);
	T_Profile(NORMAL_PROF);

	return 1;
}

LONG CBondHead::UpdateDieCounterCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	// Update Die Counter and Log Wafer Information
	LogWaferInformation(FALSE);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::CheckStartThermalTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bEnableThermalTest == FALSE)
	{
		m_bThermalTestFirstCycle = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_bThermalTestFirstCycle = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::StaticMotionLog(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bStart = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bStart);

	//Motion Log
	if (m_bEnableMotionLogZ)
	{
		LogAxisPerformance(BH_AXIS_Z, &m_stBHAxis_Z, bStart, 10, m_stDataLog);
	}

	if (m_bEnableMotionLogT)
	{
		LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, bStart, 10, m_stDataLog);
	}

	if (m_bEnableMotionLogEJ)
	{
		LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, bStart, 10, m_stDataLog);
	}
	
	if (m_bEnableMotionLogZ2)
	{
		LogAxisPerformance(BH_AXIS_Z2, &m_stBHAxis_Z2, bStart, 10, m_stDataLog);
	}

	if (bStart)
	{
		HmiMessage("Static Motion Log Start!", "Static Motion Log", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0);
	}
	else
	{
		HmiMessage("Static Motion Log Stop!", "Static Motion Log", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::CheckExArmInSafePos(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	INT nConvID;
	IPC_CServiceMessage rReqMsg;

	//Check ExArm position to avoid crashing with Bond Arm
	BOOL bBinLoaderEnabled = IsBLEnable();

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

	
LONG CBondHead::GenerateParameterList(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CString szConfigFile = _T("c:\\MapSorter\\UserData\\Parameters.csv");

	bReturn = GenerateParameterList();
	//if (bReturn)
	//{
	//	HmiMessage("Parameter List generated"/* + szConfigFile*/);
	//}
	//else
	//{
	//	HmiMessage("Parameter List failure at"/* + szConfigFile*/);
	//}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::GenerateConfigData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	bReturn = GenerateConfigData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::GeneratePkgDataFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szLine;

	CString szLocalPkgDataFileName = (*m_psmfSRam)["MS896A"]["PKG Local Data File Name"];

	CStdioFile oFile;
	if (oFile.Open(szLocalPkgDataFileName, 
				   CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareExclusive | CFile::typeText) != TRUE)
	{
		HmiMessage("ERROR: unable to open package data file - " + szLocalPkgDataFileName);
		return FALSE;
	}

	oFile.SeekToEnd();

	szLine.Format("Collet chg.Limit:,%ld\n",		m_ulMaxColletCount);
	oFile.WriteString(szLine);
	szLine.Format("Collet clean Limit:,%ld\n",		m_ulMaxCleanCount);
	oFile.WriteString(szLine);
	szLine.Format("Collet2 chg.Limit:,%ld\n",		m_ulMaxCollet2Count);
	oFile.WriteString(szLine);
	szLine.Format("Collet2 clean Limit:,%ld\n",		m_ulMaxCleanCount);
	oFile.WriteString(szLine);
	szLine.Format("Ejector chg.Limit:,%ld\n",		m_ulMaxEjectorCount);
	oFile.WriteString(szLine);

	oFile.WriteString("BondArm\n");
	szLine.Format(",Prepick position:,%ld\n",		m_lPrePickPos_T);
	oFile.WriteString(szLine);
	szLine.Format(",Pick position:,%ld\n",			m_lPickPos_T);
	oFile.WriteString(szLine);
	szLine.Format(",Bond position:,%ld\n",			m_lBondPos_T);
	oFile.WriteString(szLine);
	szLine.Format(",Clean collet/Unload:,%ld\n",	m_lCleanColletPos_T);
	oFile.WriteString(szLine);
	
	oFile.WriteString("BondHead\n");
	szLine.Format(",Pick Level:,%ld\n",				m_lPickLevel_Z);
	oFile.WriteString(szLine);
	szLine.Format(",Bond Level:,%ld\n",				m_lBondLevel_Z);
	oFile.WriteString(szLine);
	szLine.Format(",Swing Level Offset:,%ld\n",		m_lSwingOffset_Z);
	oFile.WriteString(szLine);

	oFile.WriteString("Ejector\n");
	szLine.Format(",Up Level:,%ld\n",				m_lEjectLevel_Ej);
	oFile.WriteString(szLine);
	szLine.Format(",Up Level(With BH Down):,%ld\n", m_lEjectLevel_Ej);
	oFile.WriteString(szLine);
	szLine.Format(",Standby Level:,%ld\n",			m_lStandbyLevel_Ej);
	oFile.WriteString(szLine);

	oFile.WriteString("Delay:\n");
	oFile.WriteString("BondArm&BondHead:\n");
	szLine.Format(",Pick Delay:,%ld\n",				m_lPickDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Bond Delay:,%ld\n",				m_lBondDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Arm Pick Delay:,%ld\n",			m_lArmPickDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Arm Bond Delay:,%ld\n",			m_lArmBondDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Head Pick Delay:,%ld\n",		m_lHeadPickDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Head Bond Delay:,%ld\n",		m_lHeadBondDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Head PrePick Delay:,%ld\n",		m_lHeadPrePickDelay);
	oFile.WriteString(szLine);

	oFile.WriteString("Ejector\\PR\\BinTable\\WaferTable:\n");
	szLine.Format(",PR Delay:,%ld\n",					m_lPRDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Wafer Table Theta Delay:,%ld\n",	m_lWTTDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Wafer Table Settling Delay:,%ld\n",	m_lWTSettlingDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Ejector Up Delay:,%ld\n",			m_lEjectorUpDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Ejector Down Delay:,%ld\n",			m_lEjectorDownDelay);
	oFile.WriteString(szLine);
	szLine.Format(",Ejector Vac-Off Delay:,%ld\n",		m_lEjVacOffDelay);
	oFile.WriteString(szLine);
	oFile.WriteString("Collet Blow:\n");
	szLine.Format(",High Blow Delay:,%ld\n",			m_lHighBlowTime);
	oFile.WriteString(szLine);
	szLine.Format(",Weak Blow Delay:,%ld\n",			m_lWeakBlowOffDelay);
	oFile.WriteString(szLine);


	oFile.Close();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::OpenDACCalibrationCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szContent;
	CString szTitle;
	CString szMsg;
	IPC_CServiceMessage stMsg;
	DOUBLE dPrevOpenDACCalibRatio = 0.0;

	typedef struct 
	{
		BOOL bStart;
		LONG lArmSelection;
	} BHDACCALIB;

	BHDACCALIB stInfo;

	svMsg.GetMsg(sizeof(BHDACCALIB), &stInfo);

	if (stInfo.lArmSelection == 0)
	{
		dPrevOpenDACCalibRatio = m_dBHZ1_OpenDACCalibRatio;
		m_lOpenDACCurrBHZ = BH_MS100_BHZ1;
	}
	else if (stInfo.lArmSelection == 1)
	{
		dPrevOpenDACCalibRatio = m_dBHZ2_OpenDACCalibRatio;
		m_lOpenDACCurrBHZ = BH_MS100_BHZ2;
	}

	if (stInfo.bStart == TRUE)
	{
		szTitle.LoadString(HMB_BH_OPEN_DAC);
		szContent.LoadString(HMB_BH_START_OPEN_DAC_CALIB);
	
		if (HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		BH_MoveToBlow(stMsg);

		m_lStartOpenDACCalibValue = AutoLearnDAC(TRUE, m_lOpenDACCurrBHZ, TRUE);
		m_lCurrOpenDACCalibValue = m_lStartOpenDACCalibValue;
		m_bIsOpenDACCalibStart = TRUE;

		szTitle.LoadString(HMB_BH_OPEN_DAC);
		szContent.LoadString(HMB_BH_OPEN_DAC_CALIB_WITH_GRAM_GAUGE);    //ask operator to lift BHZ with Gram Gauge to 100g               
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL);
	}
	else
	{
		m_bIsOpenDACCalibStart = FALSE;
		m_lCurrOpenDACCalibValue = m_lCurrOpenDACCalibValue - m_lStartOpenDACCalibValue;

		if (stInfo.lArmSelection == 0)
		{
			m_dBHZ1_OpenDACCalibRatio = (DOUBLE)(m_lCurrOpenDACCalibValue / 100.0);	//100g
			
			if (m_dBHZ1_OpenDACCalibRatio == 0.0)
			{
				m_dBHZ1_OpenDACCalibRatio = dPrevOpenDACCalibRatio;

				szMsg.Format("BHZ1 DAC/Force Calibration fails (%.1f)", m_dBHZ1_OpenDACCalibRatio);
				SetErrorMessage(szMsg);
				SetAlert(IDS_BH_OPEN_DAC_CALIBRATION_FAILS);
			}
			else
			{
				szMsg.Format("BHZ1 DAC/Force Ratio is updated (%.1f)", m_dBHZ1_OpenDACCalibRatio);
			}
		}
		else if (stInfo.lArmSelection == 1)
		{
			m_dBHZ2_OpenDACCalibRatio = (DOUBLE)(m_lCurrOpenDACCalibValue / 100.0);

			if (m_dBHZ2_OpenDACCalibRatio == 0.0)
			{
				m_dBHZ2_OpenDACCalibRatio = dPrevOpenDACCalibRatio;
				
				szMsg.Format("BHZ2 DAC/Force Calibration fails (%.1f)", m_dBHZ2_OpenDACCalibRatio);
				SetErrorMessage(szMsg);
				SetAlert(IDS_BH_OPEN_DAC_CALIBRATION_FAILS);
			}
			else
			{
				szMsg.Format("BHZ2 DAC/Force Ratio is updated (%.1f)", m_dBHZ2_OpenDACCalibRatio);
			}
		}
		
		SetStatusMessage(szMsg);

		szTitle.LoadString(HMB_BH_OPEN_DAC);
		szContent.LoadString(HMB_BH_STOP_OPEN_DAC_CALIB);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL);

		BH_MoveToPrePick(stMsg);
	}

	SaveBhData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::OpenDACForceCheckCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szContent;
	CString szTitle;
	IPC_CServiceMessage stMsg;
	typedef struct 
	{
		BOOL bStart;
		LONG lArmSelection;
	} BHDACCALIB;

	BHDACCALIB stInfo;

	svMsg.GetMsg(sizeof(BHDACCALIB), &stInfo);

	if (stInfo.lArmSelection == 0)
	{
		m_lOpenDACCurrBHZ = BH_MS100_BHZ1;
	}
	else if (stInfo.lArmSelection == 1)
	{
		m_lOpenDACCurrBHZ = BH_MS100_BHZ2;
	}

	if (stInfo.bStart == TRUE)
	{
		szTitle.LoadString(HMB_BH_OPEN_DAC);
		szContent.LoadString(HMB_BH_START_OPEN_DAC_CALIB_RATIO_CHECK);

		if (HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		BH_MoveToBlow(stMsg);

		m_lStartOpenDACCalibValue = AutoLearnDAC(TRUE, m_lOpenDACCurrBHZ, TRUE);
		m_bIsOpenDACForceCheckStart = TRUE;
	}
	else
	{
		m_bIsOpenDACForceCheckStart = FALSE;

		szTitle.LoadString(HMB_BH_OPEN_DAC);
		szContent.LoadString(HMB_BH_STOP_OPEN_DAC_CALIB_RATIO_CHECK);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL);

		BH_MoveToPrePick(stMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ResetOpenDACCalibRatio(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lArmSelection;
	CString szContent;
	CString szTitle;
	CString szMsg = "";

	svMsg.GetMsg(sizeof(LONG), &lArmSelection);

	szTitle.LoadString(HMB_BH_OPEN_DAC);
	szContent.LoadString(HMB_GENERAL_AREUSURE);
	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (lArmSelection == 0)
	{
		m_dBHZ1_OpenDACCalibRatio = BH_OPENDAC_CALIBRATION_RATIO;
		szMsg.Format("BHZ1 DAC/Force Ratio is reset (%.1f)", m_dBHZ1_OpenDACCalibRatio);
	}
	else if (lArmSelection == 1)
	{
		m_dBHZ2_OpenDACCalibRatio = BH_OPENDAC_CALIBRATION_RATIO;
		szMsg.Format("BHZ2 DAC/Force Ratio is reset (%.1f)", m_dBHZ2_OpenDACCalibRatio);	
	}	

	SetStatusMessage(szMsg);

	SaveBhData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::DACDistanceCalibrationCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szContent;
	CString szTitle;
	CString szMsg;
	IPC_CServiceMessage stMsg;
	INT		nStatus;
	LONG	lArmSelection;
	LONG	lCurrZLevel = 0;
	LONG	lStepSize = 20;
	LONG	lMinPos = 0;
	LONG	lInitialDAC, lFinalDAC;

	//Check min limit value
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);

	svMsg.GetMsg(sizeof(LONG), &lArmSelection);

	szTitle.LoadString(HMB_BH_DAC_DIST_CALIB);
	szContent.LoadString(HMB_BH_DAC_DIST_CALIB_START);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	else
	{	
		lCurrZLevel = 0;
		Z_MoveTo(lCurrZLevel);
		Z2_MoveTo(lCurrZLevel);

		if (lArmSelection == 0)
		{
			SetPickVacuum(TRUE);	//Test missing die sensor reply
			Sleep(1000);
			if (IsColletJam() == TRUE)
			{
				OpDisplayAlarmPage(TRUE);	//v4.53A13
				SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_1);		//v4.50A1
				SetPickVacuum(FALSE);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			T_MoveTo(m_lBondPos_T);

			while (1)	//Start Learning Missing Die Sensor
			{
				Sleep(10);
				if (IsColletJam() == TRUE)	//Use air-flow sensor
				{
					Sleep(10);
					if (IsColletJam() == TRUE)
					{
						break;
					}
				}

				nStatus = Z_MoveTo((lCurrZLevel -= lStepSize));
				if ((lCurrZLevel <= lMinPos) || (nStatus != gnOK))
				{
					SetPickVacuum(FALSE);
					SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}
			}

			SetPickVacuum(FALSE);			//Off Collet vacuum 

			Z_Sync();
			Sleep(500);
			lInitialDAC = AutoLearnDAC(FALSE, BH_MS100_BHZ1, TRUE);

			Z_MoveTo(lCurrZLevel - 500);
			Z_Sync();
			Sleep(500);
			lFinalDAC = AutoLearnDAC(FALSE, BH_MS100_BHZ1, TRUE);

			if ((lFinalDAC - lInitialDAC) != 0)
			{
				m_dBHZ1_DACDistanceCalibRatio = (lFinalDAC - lInitialDAC) / 500.0;
				szMsg.Format("BHZ1 DAC/Distance Ratio is updated (%.1f)", m_dBHZ1_DACDistanceCalibRatio);
			}
			else
			{
				szMsg.Format("BHZ1 DAC/Distance Ratio fails (%.1f)", m_dBHZ1_DACDistanceCalibRatio);
				SetErrorMessage(szMsg);
				SetAlert(IDS_BH_DAC_DIST_CALIB_FAILS);
			}
		}
		else if (lArmSelection == 1)
		{
			SetPickVacuumZ2(TRUE);	//Test missing die sensor reply
			Sleep(1000);
			if (IsColletJamZ2() == TRUE)
			{
				OpDisplayAlarmPage(TRUE);	//v4.53A13
				SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_1);	//v4.50A1
				SetPickVacuumZ2(FALSE);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		
			T_MoveTo(m_lPickPos_T);

			while (1)	//Start Learning Missing Die Sensor
			{
				Sleep(10);
				if (IsColletJamZ2() == TRUE)	//Use air-flow sensor

				{
					Sleep(10);
					if (IsColletJamZ2() == TRUE)
					{
						break;
					}
				}

				nStatus = Z2_MoveTo((lCurrZLevel -= lStepSize));
				if ((lCurrZLevel <= lMinPos) || (nStatus != gnOK))
				{
					SetPickVacuumZ2(FALSE);
					SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}
			}

			SetPickVacuumZ2(FALSE);			//Off Collet vacuum 

			Z2_Sync();
			Sleep(500);
			lInitialDAC = AutoLearnDAC(FALSE, BH_MS100_BHZ2, TRUE);

			Z2_MoveTo(lCurrZLevel - 500);
			Z2_Sync();
			Sleep(500);
			lFinalDAC = AutoLearnDAC(FALSE, BH_MS100_BHZ2, TRUE);

			if ((lFinalDAC - lInitialDAC) != 0)
			{
				m_dBHZ2_DACDistanceCalibRatio = (lFinalDAC - lInitialDAC) / 500.0;
				szMsg.Format("BHZ2 DAC/Distance Ratio is updated (%.1f)", m_dBHZ2_DACDistanceCalibRatio);
			}
			else
			{
				szMsg.Format("BHZ2 DAC/Distance Ratio fails (%.1f)", m_dBHZ2_DACDistanceCalibRatio);
				SetErrorMessage(szMsg);
				SetAlert(IDS_BH_DAC_DIST_CALIB_FAILS);
			}
		}
	}
	
	SetStatusMessage(szMsg);

	szTitle.LoadString(HMB_BH_DAC_DIST_CALIB);
	szContent.LoadString(HMB_BH_DAC_DIST_CALIB_STOP);            
	HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL);
		
	BH_MoveToPrePick(stMsg);

	SaveBhData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ResetDACDistanceCalibRatio(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lArmSelection;
	CString szContent;
	CString szTitle;
	CString szMsg = "";

	svMsg.GetMsg(sizeof(LONG), &lArmSelection);

	szTitle.LoadString(HMB_BH_DAC_DIST_CALIB);
	szContent.LoadString(HMB_GENERAL_AREUSURE);
	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (lArmSelection == 0)
	{
		m_dBHZ1_DACDistanceCalibRatio = BH_DACDISTANCE_CALIBRATION_RATIO;
		szMsg.Format("BHZ1 DAC/Distance Ratio is reset (%.1f)", m_dBHZ1_DACDistanceCalibRatio);
	}
	else if (lArmSelection == 1)
	{
		m_dBHZ2_DACDistanceCalibRatio = BH_DACDISTANCE_CALIBRATION_RATIO;
		szMsg.Format("BHZ2 DAC/Distance Ratio is reset (%.1f)", m_dBHZ2_DACDistanceCalibRatio);	
	}	

	SetStatusMessage(szMsg);

	SaveBhData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::CheckMDResponseTime(IPC_CServiceMessage &svMsg)
{
	BOOL bBHZ1 = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bBHZ1);

//HmiMessage("Start test ...");

	INT i = 0;
	CString szMsg;
	BOOL bDieDetected	= FALSE;
	BOOL bResult		= TRUE;
	CString szTitle		= _T("BH Missing-Die Response Test");
	//INT nStage = 1;

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}


	//M75	//v4.52A14
	LONG lSelection = 0;
	CStringList szSelection;
	szSelection.AddTail("MANUAL");
	szSelection.AddTail("AUTO");

	LONG lResult = HmiSelection("Please select a mode to measure MD sensor.", 
						"BL Mgzn configuration", szSelection, 0);
	if (lResult == -1)
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	BOOL bManual = FALSE;
	if (lResult == 0)
	{
		bManual = TRUE;
	}


	if (bManual)
	{
		while (1)
		{
			bDieDetected = FALSE;

			if (bBHZ1)
			{
				szMsg = "BHZ1 MD response time out";
				SetPickVacuum(TRUE);
				LONG lCurrentTime = GetCurTime();
				Sleep(10);

				for (i = 0; i < 2000; i++)
				{
					Sleep(1);
					if (IsMissingDie() == FALSE)
					{
						lCurrentTime = GetCurTime() - lCurrentTime;
						szMsg.Format("BHZ1 MD response time = %d ms", lCurrentTime);
						bDieDetected = TRUE;
						break;
					}
				}
			}
			else
			{
				szMsg = "BHZ2 MD response time out";
				SetPickVacuumZ2(TRUE);
				LONG lCurrentTime = GetCurTime();
				Sleep(10);

				for (i = 0; i < 2000; i++)
				{
					Sleep(1);
					if (IsMissingDieZ2() == FALSE)
					{
						lCurrentTime = GetCurTime() - lCurrentTime;
						szMsg.Format("BHZ2 MD response time = %d ms", lCurrentTime);
						bDieDetected = TRUE;
						break;
					}
				}
			}

			if (HmiMessage(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			{
				SetPickVacuum(FALSE);	
				SetPickVacuumZ2(FALSE);	
				break;
			}

			if (bBHZ1)
			{
				SetPickVacuum(FALSE);	
			}
			else
			{
				SetPickVacuumZ2(FALSE);	
			}

			Sleep(500);
		}
	}
	else
	{

		//checkNoDie
		IPC_CServiceMessage stMsg;
		BOOL bFindDie = TRUE;
		stMsg.InitMessage(sizeof(BOOL),&bFindDie);
		INT nConvID = m_comClient.SendRequest(WAFER_PR_STN,"UserSearchDie_CheckNoDie",stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bFindDie);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bFindDie == FALSE)
		{
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		
		SetPickVacuum(FALSE);	
		SetPickVacuumZ2(FALSE);	
		Z_MoveToHome();
		if (m_bIsArm2Exist == TRUE)
		{
			Z2_MoveToHome();
		}

		//v4.52A11
		if (m_bMS100EjtXY && m_bEnableMS100EjtXY)
		{
			if (EjX_IsPowerOn() && EjY_IsPowerOn())
			{
				LONG lEjtX = m_lCDiePos_EjX;
				LONG lEjtY = m_lCDiePos_EjY;

				LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
				LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
				LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
				LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

				if (!bBHZ1)
				{
					lEjtX = lEjtX + lCollet2OffsetX;
					lEjtY = lEjtY + lCollet2OffsetY;
				}
				else
				{
					lEjtX = lEjtX + lCollet1OffsetX;
					lEjtY = lEjtY + lCollet1OffsetY;
				}

				EjX_MoveTo(lEjtX, SFM_NOWAIT);
				EjY_MoveTo(lEjtY, SFM_WAIT);
				EjX_Sync();
			}
		}

		if (bBHZ1)
		{
			T_MoveTo(m_lPickPos_T);
		}
		else
		{
			T_MoveTo(m_lBondPos_T);
		}

		while (1)
		{
			bDieDetected = FALSE;

			if (bBHZ1)
			{
				szMsg = "BHZ1 MD response time out";
				Z_MoveTo(m_lPickLevel_Z);

				SetPickVacuum(TRUE);
				LONG lCurrentTime = GetCurTime();
				Sleep(10);

				for (i = 0; i < 2000; i++)
				{
					Sleep(1);
					if (IsMissingDie() == FALSE)
					{
						lCurrentTime = GetCurTime() - lCurrentTime;
						szMsg.Format("BHZ1 MD response time = %d ms", lCurrentTime);
						bDieDetected = TRUE;
						break;
					}
				}
			}
			else
			{
				szMsg = "BHZ2 MD response time out";
				Z2_MoveTo(m_lPickLevel_Z2);

				SetPickVacuumZ2(TRUE);
				LONG lCurrentTime = GetCurTime();
				Sleep(10);

				for (i = 0; i < 2000; i++)
				{
					Sleep(1);
					if (IsMissingDieZ2() == FALSE)
					{
						lCurrentTime = GetCurTime() - lCurrentTime;
						szMsg.Format("BHZ2 MD response time = %d ms", lCurrentTime);
						bDieDetected = TRUE;
						break;
					}
				}
			}

			if (HmiMessage(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			{
				break;
			}

			if (bBHZ1)
			{
				SetPickVacuum(FALSE);	
				Z_MoveToHome();
			}
			else
			{
				SetPickVacuumZ2(FALSE);	
				Z2_MoveToHome();
			}
			Sleep(500);
		}
			
		SetPickVacuum(FALSE);	
		SetPickVacuumZ2(FALSE);	
		Z_MoveToHome();
		if (m_bIsArm2Exist == TRUE)
		{
			Z2_MoveToHome();
		}
		T_MoveTo(m_lPrePickPos_T);
	}


	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::StartBHZ1MotionTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	/*
	typedef struct 
	{
		BOOL	bIsStart;
		LONG	lDistance;
		LONG	lDelay;
	} MOTION_TEST;

	MOTION_TEST stData;

	svMsg.GetMsg(sizeof(MOTION_TEST), &stData);

	if (stData.bIsStart == TRUE)
	{
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(BH_AXIS_Z, &m_stBHAxis_Z, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(BH_AXIS_Z, &m_stBHAxis_Z, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestBondHeadZ1 = stData.bIsStart;
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::StartBHZ2MotionTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	/*
	typedef struct 
	{
		BOOL	bIsStart;
		LONG	lDistance;
		LONG	lDelay;
	} MOTION_TEST;

	MOTION_TEST stData;

	svMsg.GetMsg(sizeof(MOTION_TEST), &stData);

	if (stData.bIsStart == TRUE)
	{
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(BH_AXIS_Z2, &m_stBHAxis_Z2, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(BH_AXIS_Z2, &m_stBHAxis_Z2, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestBondHeadZ2 = stData.bIsStart;
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}



LONG CBondHead::StartBArmMotionTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		BOOL	bIsStart;
		LONG	lMode;
		LONG	lDistance;
		LONG	lDelay;
	} MOTION_TEST;

	MOTION_TEST stData;

	svMsg.GetMsg(sizeof(MOTION_TEST), &stData);

/*
	//v4.48A2	//Protect against BH collision with expander
	if (IsEnableWL() && (IsWLExpanderOpen() == TRUE))
	{
		SetErrorMessage("Expander not closed in ILC_AutoLearnCmd");
		//bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//v4.48A2	//Check BT frame level
	BOOL bBinLoader	= IsBLEnable();
	if (bBinLoader)	 //&& !bFrameLevel)
	{
		if (!IsBT1FrameLevel())
		{
			SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
			SetErrorMessage("ILC_AutoLearnCmd: BT frame level not DOWN");
			//bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		//bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	if (stData.lMode == 0)
	{
		if (stData.bIsStart == TRUE)
		{
			// start from positive direction first
			m_bMoveDirection = TRUE;
			LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, TRUE, 10, m_stDataLog);
		}
		else
		{
			if (CheckCoverOpenInManual("Start BArm Motion Test") == FALSE)
			{
				m_bIsMotionTestBondArm = FALSE;
				bReturn = TRUE;
				LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;		
			}

			T_MoveTo(m_lPrePickPos_T);
			LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
		}

		m_lMotionTestDelay = stData.lDelay;
		m_lMotionTestDist = stData.lDistance;
		m_bIsMotionTestBondArm = stData.bIsStart;
	}
	else
	{
		if (IsEnableILC() == FALSE)
		{
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (stData.bIsStart == TRUE)
		{
			Z_MoveTo(m_lSwingLevel_Z);
			Z2_MoveTo(m_lSwingLevel_Z2);		//v4.44A1	//Semitek

			if (ILC_IsInitSuccess() == FALSE)
			{
				CString szContent, szTitle;
				bReturn = TRUE;
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_BH_ILC_INIT_FAIL);
				HmiMessage(szContent, szTitle);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (ILC_IsAutoLearnComplete() == FALSE)
			{
				CString szContent, szTitle;
				bReturn = TRUE;
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_BH_ILC_MOTION_TEST_INIT_FAIL);
				HmiMessage(szContent, szTitle);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (ILC_UpdateContourMoveProfile() == FALSE)
			{
				CString szContent, szTitle;
				bReturn = TRUE;
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_BH_ILC_UPDATE_PROFILE_FAIL);
				HmiMessage(szContent, szTitle);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (CheckCoverOpenInManual("Start BArm Motion Test") == FALSE)
			{
				m_bILCContourMoveTest = FALSE;
				bReturn = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}		

			if (stData.lDistance == BH_T_PICK_TO_BOND)
			{
				T_MoveTo(m_lPickPos_T);

			}
			else if (stData.lDistance == BH_T_BOND_TO_PICK)
			{
				T_MoveTo(m_lBondPos_T);
			}
			else if (stData.lDistance == BH_T_PREPICK_TO_PICK)
			{
				T_MoveTo(m_lPrePickPos_T);
			}
			else
			{
				T_MoveTo(m_lPrePickPos_T);
			}

			m_bMoveDirection = TRUE;
			LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, TRUE, 10, m_stDataLog);
		}
		else
		{
			if (CheckCoverOpenInManual("Start BArm Motion Test") == FALSE)
			{
				m_bILCContourMoveTest = FALSE;
				bReturn = TRUE;
				LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;		
			}

			T_MoveTo(m_lPrePickPos_T);
			LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
		}

		m_lMotionTestDelay = stData.lDelay;
		m_lMotionTestDist = stData.lDistance;
		m_bILCContourMoveTest = stData.bIsStart;
	}
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::StartEjMotionTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	/*
	typedef struct 
	{
		BOOL	bIsStart;
		LONG	lDistance;
		LONG	lDelay;
	} MOTION_TEST;

	MOTION_TEST stData;

	svMsg.GetMsg(sizeof(MOTION_TEST), &stData);

	if (stData.bIsStart == TRUE)
	{
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, TRUE, 10, m_stDataLog);
	}
	else
	{
		Ej_MoveTo(0);
		LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestEjector = stData.bIsStart;
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::StartPBTZMotionTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	/*
	typedef struct 
	{
		BOOL	bIsStart;
		LONG	lDistance;
		LONG	lDelay;
	} MOTION_TEST;

	MOTION_TEST stData;

	svMsg.GetMsg(sizeof(MOTION_TEST), &stData);

	if (stData.bIsStart == TRUE)
	{
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestPBTZ = stData.bIsStart;
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::GetCommandPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CString szTemp;
	szTemp.Format("Get Command Pos:%d", CMS896AStn::MotionGetCommandPosition(BH_AXIS_T, &m_stBHAxis_T));

	AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;

}

LONG CBondHead::BH_AutoMoveToPrePick(IPC_CServiceMessage &svMsg)
{
	BOOL bToSafe = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bToSafe);

	T_Sync();
	Z_Sync();
	if (m_bIsArm2Exist)
	{
		Z2_Sync();
	}
	Ej_Sync();
	Sleep(50);

	if (bToSafe)
	{
		GetEncoderValue();
		m_lAtPickPosnT	= m_lCurPos_T;
		m_lAtPickPosnZ	= m_lEnc_Z;
		if (m_bIsArm2Exist)
		{
			m_lAtPickPosnZ2		= m_lEnc_Z2;
		}
		m_lAtPickPosnEj = m_lEnc_Ej;

		Ej_MoveTo(0);
		Z_MoveTo(m_lSwingLevel_Z);
		if (m_bIsArm2Exist)
		{
			Z2_MoveTo(m_lSwingLevel_Z2);
		}
		Z_Sync();
		if (m_bIsArm2Exist)
		{
			Z2_Sync();
		}
		Ej_Sync();
		Sleep(50);

		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
		T_Profile(NORMAL_PROF);
		T_Sync();
		Sleep(50);
		CString szMsg;
		szMsg.Format("BH - To safe, Sync all, old T %d, Z %d Z2 %d, EJ %d; moveto home and prepickT",
			m_lEnc_T, m_lEnc_Z, m_lEnc_Z2, m_lEnc_Ej);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
		DisplaySequence(szMsg);
	}
	else
	{
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lAtPickPosnT);
		T_Profile(NORMAL_PROF);
		T_Sync();
		Sleep(50);

		Ej_MoveTo(m_lStandbyLevel_Ej);	//	m_lAtPickPosnEj
		Sleep(100);
		Z_MoveTo(m_lAtPickPosnZ);
		if (m_bIsArm2Exist)
		{
			Z2_MoveTo(m_lAtPickPosnZ2);
		}
		Z_Sync();
		if (m_bIsArm2Exist)
		{
			Z2_Sync();
		}
		Ej_Sync();
		Sleep(50);
		CString szMsg;
		szMsg.Format("BH - To sort, Sync all, to T %d, Z %d Z2 %d, EJ %d (old %ld); moveto home and prepickT", 
			m_lAtPickPosnT, m_lAtPickPosnZ, m_lAtPickPosnZ2, m_lStandbyLevel_Ej, m_lAtPickPosnEj);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
		DisplaySequence(szMsg);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondHead::BH_EnableThermalControlCmd(IPC_CServiceMessage &svMsg)	//	control vai IPC command for auto cycle only
{
	BOOL bEnable = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	if (IsMS60() && m_bMS60ThermalCtrl)
	{
		BH_EnableThermalControl(bEnable);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondHead::BH_UpdateThermalTemperature(IPC_CServiceMessage &svMsg)
{
	if (IsMS60() && m_bMS60ThermalCtrl)
	{
		OpUpdateBHTThermalRMSValues(30, TRUE);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}	//	update thermal temperature

LONG CBondHead::ILC_AutoLearnCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		BOOL	bIsStart;
		LONG	lMode;
		LONG	lDelay;
	} ILC_LEARN;

	ILC_LEARN stData;

	svMsg.GetMsg(sizeof(ILC_LEARN), &stData);

	if (IsEnableILC() == FALSE)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!IsBondArmAbleToMove())
	{
		//bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//v4.43T6	
	//LONG lSwingLevel_Z	= m_lPickLevel_Z + m_lSwingOffset_Z;
	//LONG lSwingLevel_Z2	= m_lPickLevel_Z2 + m_lSwingOffset_Z2;
	//v4.47T12
	LONG lSwingLevel_Z	= max(m_lPickLevel_Z, m_lBondLevel_Z) + m_lSwingOffset_Z;
	LONG lSwingLevel_Z2	= max(m_lPickLevel_Z2, m_lBondLevel_Z2) + m_lSwingOffset_Z2;

	if (stData.bIsStart == TRUE)
	{
		CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoLearnCmd - ILC_AutoLearnInitFunc pick to bond");

		if (ILC_AutoLearnInitFunc(BH_T_PICK_TO_BOND) == FALSE)
		{
			CString szTitle, szContent;
			szTitle.LoadString(HMB_BH_ILC);
			szContent.LoadString(HMB_BH_ILC_AUTO_LEARN_INIT_FAIL);
			HmiMessage(szContent, szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (ILC_AutoLearnInitFunc(BH_T_BOND_TO_PICK) == FALSE)
		{
			CString szTitle, szContent;
			szTitle.LoadString(HMB_BH_ILC);
			szContent.LoadString(HMB_BH_ILC_AUTO_LEARN_INIT_FAIL);
			HmiMessage(szContent, szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		
		if (CheckCoverOpenInManual("Auto Learn Cmd") == FALSE)
		{
			m_bILCAutoLearn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		
		Z_MoveTo(lSwingLevel_Z);	//v4.43T6
		Z2_MoveTo(lSwingLevel_Z2);	//v4.43T6
		T_MoveTo(m_lPickPos_T);
		m_bMoveDirection = TRUE;
		
		Sleep(100);
	}
	else
	{
		//Move back to prepick position
		if (CheckCoverOpenInManual("Auto Learn Cmd") == FALSE)
		{
			m_bILCAutoLearn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		Z_MoveTo(0);	//v4.43T6
		Z2_MoveTo(0);	//v4.43T6
		Sleep(200);

		//v4.46T20	//Cree
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
		//{
			if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
			{
				Sleep(100);
				Z_Move(pApp->GetBHZ1HomeOffset());
			}
#ifdef NU_MOTION
			if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
			{
				Sleep(100);
				Z2_Move(pApp->GetBHZ2HomeOffset());
			}
#endif
		//}

		T_MoveTo(m_lPrePickPos_T);
	}

	if (IsMS60())
	{
		//turn off thermal fcn before performing ILC
		//m_bEnableMS60ThermalCheck = FALSE;		//v4.48A27	
		EnableBHTThermalControl(FALSE);				//v4.50A9	
	}

	m_lILCAutoLearnDelay = stData.lDelay;
	m_bILCAutoLearn = stData.bIsStart;
	m_lILCAutoLearnState = stData.lMode;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if(stData.bIsStart == TRUE)
	{
		pApp->EnableScreenButton(FALSE);
	}
	else
	{
		pApp->EnableScreenButton(TRUE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}



// used for burn in
LONG CBondHead::ILC_RestartUpdateCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (IsEnableILC() == FALSE)
	{
		bReturn = TRUE;
		CMS896AApp::m_bBondHeadILCFirstCycle = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMS896AApp::m_bBondHeadILCFirstCycle = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ILC_BondArmPreStartCheck(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (IsEnableILC() == FALSE)
	{	
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;	
	}

	if (ILC_IsInitSuccess() == FALSE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (ILC_IsAutoLearnComplete() == FALSE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ReadDigitalAirFlowSensor(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lValue = 0;

	if (!m_bMS100DigitalAirFlowSnr)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szSelection1 = "Arm1";
	CString szSelection2 = "Arm2";
	CString szMsg;
	INT nArmSelection = HmiMessage("Please choose which arm to be learnt.", "Learn Collet Hole", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);
	if (nArmSelection == 1)
	{
		SetPickVacuum(FALSE);
		szMsg = "Please press CLOSE button to take the Set-Zero Offset for BHZ1";
	}
	else if (nArmSelection == 2)
	{
		SetPickVacuumZ2(FALSE);
		szMsg = "Please press CLOSE button to take the Set-Zero Offset for BHZ2";
	}
	else
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//v4.44T1	//Added SetZero Offset
	LONG lReturn = HmiMessageEx(szMsg, "Learn Digital AirFlow Sensor", 
		glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (nArmSelection == 1)
	{
		m_ulBHZ1DAirFlowSetZeroOffset = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
		szMsg = "Please press CLOSE button to take the clogged reading for BHZ1";
		Sleep(100);
		SetPickVacuum(TRUE);
	}
	else
	{
		m_ulBHZ2DAirFlowSetZeroOffset = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
		szMsg = "Please press CLOSE button to take the clogged reading for BHZ2";
		Sleep(100);
		SetPickVacuumZ2(TRUE);
	}


	lReturn = HmiMessageEx(szMsg, "Learn Digital AirFlow Sensor", 
		glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (nArmSelection == 1)
	{
		lValue = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
		lValue = lValue - m_ulBHZ1DAirFlowSetZeroOffset;
		if (lValue < 0)
			lValue = 0;
		m_ulBHZ1DAirFlowBlockValue	= (ULONG) lValue;
		//m_ulBHZ1DAirFlowBlockValue = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
		szMsg = "Please press CLOSE button to take the unclogged reading for BHZ1";
	}
	else
	{
		lValue = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
		lValue = lValue - m_ulBHZ2DAirFlowSetZeroOffset;
		if (lValue < 0)
			lValue = 0;
		m_ulBHZ2DAirFlowBlockValue	= (ULONG) lValue;
		//m_ulBHZ2DAirFlowBlockValue = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
		szMsg = "Please press CLOSE button to take the unclogged reading for BHZ2";
	}

	ULONG ulThresholdValue = 0;

	lReturn = HmiMessageEx(szMsg, "Learn Digital AirFlow Sensor", 
		glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (nArmSelection == 1)
	{
		ReadUpdateBHZ1ColletThresholdValue(FALSE);
		SetPickVacuum(FALSE);
		CheckDigitalAirFlowThresholdLimit(FALSE);	//v4.50A3
	}
	else
	{
		ReadUpdateBHZ2ColletThresholdValue(FALSE);
		SetPickVacuumZ2(FALSE);
		CheckDigitalAirFlowThresholdLimit(TRUE);	//v4.50A3
	}

	SaveBhData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::AutoLearnBHZHomeOffset(IPC_CServiceMessage &svMsg)		//v4.46T26	//Cree HuiZhou PkgSort
{
	BOOL bReturn = TRUE;
	BOOL bBHZ2 = FALSE;
	CString szMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	svMsg.GetMsg(sizeof(BOOL), &bBHZ2);

	if (!IsBondArmAbleToMove())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lPrePickPos_T);
	T_Profile(NORMAL_PROF);

	if (bBHZ2)
	{
		Z2_PowerOn(FALSE);

		LONG lHmiStatus = HmiMessage("Please move BHZ2 to topmost position and press CONTINUE, or press STOP to reset", 
										"BHZ2 Home Offset", glHMI_MBX_CONTINUESTOP | 0x80000000);
		if (lHmiStatus != glHMI_CONTINUE)
		{
			m_lBHZ2HomeOffset = 0;
			pApp->SetBHZ2HomeOffset(m_lBHZ2HomeOffset);
			szMsg.Format("BHZ2 Home offset is reset to %ld", m_lBHZ2HomeOffset);
			HmiMessage(szMsg);

			Z2_Home();
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		GetEncoderValue();
		m_lBHZ2HomeOffset = m_lEnc_Z2 - 500;

		INT nCount = 0;
		do 
		{
			HmiMessage("Please move away your hands; BHZ2 will HOME ....");
			Z2_Home();
			nCount++;
			if (nCount > 3)
			{
				SetErrorMessage("BHZ2 not power on in AutoLearnBHZHomeOffset");
				CString szErr = "\nBHZ 2 is not power on!";
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

		} while (!Z2_IsPowerOn());

		pApp->SetBHZ2HomeOffset(m_lBHZ2HomeOffset);
		szMsg.Format("BHZ2 Home offset is updated to %ld", m_lBHZ2HomeOffset);
	}
	else
	{
		Z_PowerOn(FALSE);

		LONG lHmiStatus = HmiMessage("Please move BHZ1 to topmost position and press CONTINUE, or press STOP to reset", 
										"BHZ1 Home Offset", glHMI_MBX_CONTINUESTOP | 0x80000000);
		if (lHmiStatus != glHMI_CONTINUE)
		{
			m_lBHZ1HomeOffset = 0;
			pApp->SetBHZ1HomeOffset(m_lBHZ1HomeOffset);
			szMsg.Format("BHZ1 Home offset is reset to %ld", m_lBHZ1HomeOffset);
			HmiMessage(szMsg);

			Z_Home();
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		GetEncoderValue();
		m_lBHZ1HomeOffset = m_lEnc_Z - 500;

		INT nCount = 0;
		do 
		{
			HmiMessage("Please move away your hands; BHZ will HOME ....");
			Z_Home();
			nCount++;
			if (nCount > 3)
			{
				SetErrorMessage("BHZ not power on in AutoLearnBHZHomeOffset");
				CString szErr = "\nBHZ 1 is not power on!";
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

		} while (!Z_IsPowerOn());

		pApp->SetBHZ1HomeOffset(m_lBHZ1HomeOffset);
		szMsg.Format("BHZ1 Home offset is updated to %ld", m_lBHZ1HomeOffset);
	}

	HmiMessage(szMsg);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Test(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = FALSE;
	BOOL bResult = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	//CString szMsg;
	//if (bOn)		//If BHZ2 towards bond	
	//{
	//	LONG lBondZ2 = GetZ2BondLevel(FALSE, TRUE);
	//	AC_Z2B_MoveToOrSearch(lBondZ2, SFM_WAIT);
	//	szMsg.Format("BH - BHZ2 to bond %d PICK_Z_DOWN_Q", lBondZ2);
	//	Z2_MoveTo(0);
	//}
	//else
	//{
	//	LONG lBondZ1 = GetZ1BondLevel(FALSE, TRUE);
	//	AC_Z1B_MoveToOrSearch(lBondZ1, SFM_WAIT);
	//	szMsg.Format("BH - BHZ1 to bond %d in PICK_Z_DOWN_Q", lBondZ1);
	//	Z_MoveTo(0);
	//}								
	//DisplaySequence(szMsg);

	if (bOn)
	{
		HmiMessage("TEST: ON ....");
		TurnOnBlowAndCatchModule(TRUE);
	}
	else
	{
		HmiMessage("TEST: OFF ....");
		TurnOnBlowAndCatchModule(FALSE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CBondHead::CheckTheEjectorLifeTime(IPC_CServiceMessage& svMsg)
{
	BOOL bResult = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	// if m_ulMaxEjectorCount <= m_ulEjectorCount + MapDieCount , then return false

	ULONG lMapDieCount = m_lPrescanSortingTotal;
	if ( m_ulMaxEjectorCount > 0 )
	{
		if ( m_ulMaxEjectorCount <= m_ulEjectorCount + lMapDieCount )
		{
			bResult = FALSE;
		}
		else 
		{
			bResult = TRUE;
		}

		//4.53D26 Check EjCount without Map die count fuc
		if (pApp->GetCustomerName() == "Electech3E(DL)")		//v4.53	Klocwork
		{
			if ( m_ulMaxEjectorCount <= m_ulEjectorCount )
			{
				bResult = FALSE;
			}
			else 
			{
				bResult = TRUE;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CBondHead::SetBondHeadFanCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	BOOL bOn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	CString szTemp;
	szTemp.Format("SetBondHeadFan %d", bOn);
	HmiMessage(szTemp);

	if (IsMS60())
	{
		if (bOn)
		{
//m_bCoolingFanWillTurnOff = FALSE;
//m_lBondHeadCoolingSecond = 0;
			SetBondHeadFan(TRUE);

			m_bCoolingFanWillTurnOff = FALSE;
			m_lBondHeadCoolingSecond = 0;	

			Sleep(1000);

			if (!OpCheckMS60BHFanOK())
			{
				HmiMessage("BH Fan sensor is not ON; please check sensor board & connection!");
			}
		}
		else
		{
			SetBondHeadFan(FALSE);
			m_bCoolingFanWillTurnOff = TRUE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CBondHead::CheckCoverAlarm(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = IsCoverOpen();
	//AfxMessageBox("1");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::EjSoftTouch(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	EjSoftTouch_CMD();
	SaveBhData();

	//v4.54A6
	CString szMsg;
	if (IsMS60() && m_bMS60EjElevator)
	{
		szMsg.Format("EJ (%s) and EjCAP (%s) speed is changed.", 
						(LPCTSTR) m_szEjSoftTouch, (LPCTSTR) m_szEjCapSoftTouch);
	}
	else
	{
		szMsg.Format("EJ (%s) speed is changed.", (LPCTSTR) m_szEjSoftTouch);
	}
	HmiMessage(szMsg);

	//return TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::HC_UpdateISValue(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	DOUBLE dValue = 0;
/*	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szValue;
	szValue.Format("%.2f", m_dIS);
	pApp->WriteProfileString(gszPROFILE_HW_CONFIG, _T("MS60 Thermal IS"), szValue);
*/
	CString szMsg;
	szMsg.Format("IS value updated and saved (%.2f)", m_dIS);
	HmiMessage(szMsg);

	//return TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::HC_UpdatePIDValue(IPC_CServiceMessage &svMsg)	//v4.54A1
{
	BOOL bReturn = TRUE;
	DOUBLE dValue = 0;

	//svMsg.GetMsg(sizeof(DOUBLE), &dValue);
	//m_dIS = dValue;
/*	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szValue;
	szValue.Format("%.2f", m_dMS60Thermal_P);
	pApp->WriteProfileString(gszPROFILE_HW_CONFIG, _T("MS60 Thermal P"), szValue);
	szValue.Format("%.2f", m_dMS60Thermal_I);
	pApp->WriteProfileString(gszPROFILE_HW_CONFIG, _T("MS60 Thermal I"), szValue);
	szValue.Format("%.2f", m_dMS60Thermal_D);
	pApp->WriteProfileString(gszPROFILE_HW_CONFIG, _T("MS60 Thermal D"), szValue);
*/

	CString szMsg;
	szMsg.Format("PID value updated and saved (%.2f, %.2f, %.2f)", 
						m_dMS60Thermal_P, m_dMS60Thermal_I, m_dMS60Thermal_D);
	HmiMessage(szMsg);

	//return TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_AutoLearnLevels(IPC_CServiceMessage &svMsg)
{
	BOOL bPassIn = TRUE;

	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	ULONG lRTUpdateMissingDie = pAppMod->GetFeatureValue(MS896A_FUNC_GENERAL_RT_UPDTAE_MISSING_DIE_THRESHOLD);

	if (lRTUpdateMissingDie == 2)
	{
		ReadUpdateBHZ1ColletThresholdValue(TRUE);
		ReadUpdateBHZ2ColletThresholdValue(TRUE);
	}

	OpAutoLearnPickBondLevels(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_UpdateRegistry(IPC_CServiceMessage &svMsg)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("Auto Learn Level Idle Time"), m_lAutoLearnLevelsIdleTime);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::ManualPickDieToUpLookPR(IPC_CServiceMessage &svMsg)	//andrewng6
{
	BOOL bReturn = TRUE;
	CStringList szSelection;
	szSelection.AddTail("BH 1");
	szSelection.AddTail("BH 2");
	LONG lSelection = 0;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	//No BHT in Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	BOOL bBH2 = FALSE;
	LONG lResult = HmiSelection("Please select BH to PICK level", "BH Selection", szSelection, lSelection);
	
	switch (lResult)
	{
	case 1:		//BH2
		m_lWithBHDown = 1;
		bBH2 = TRUE;
		break;
	case 0:		//BH1
		m_lWithBHDown = 0;
		bBH2 = FALSE;
		break;
	case -1:	//CANCEL
	default:
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	Z_MoveToHome();
	Z2_MoveToHome();

	if (!UserSearchDieToPrCenter(bBH2))
	{
		HmiMessage("No die is found !!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bMS100EjtXY && m_bEnableMS100EjtXY)
	{
		if (EjX_IsPowerOn() && EjY_IsPowerOn())
		{
			LONG lEjtX = m_lCDiePos_EjX;
			LONG lEjtY = m_lCDiePos_EjY;

			LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
			LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
			LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
			LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

			if (bBH2)
			{
				lEjtX = lEjtX + lCollet2OffsetX;
				lEjtY = lEjtY + lCollet2OffsetY;
			}
			else
			{
				lEjtX = lEjtX + lCollet1OffsetX;
				lEjtY = lEjtY + lCollet1OffsetY;
			}

			EjX_MoveTo(lEjtX, SFM_NOWAIT);
			EjY_MoveTo(lEjtY, SFM_WAIT);
			EjX_Sync();
		}
	}

	if (!bBH2)
	{
		T_MoveTo(m_lPickPos_T);

		SetPickVacuum(TRUE);
		SetEjectorVacuum(TRUE);
		Z_MoveTo(m_lPickLevel_Z);


		Ej_MoveTo(m_lEjectLevel_Ej);
		Sleep(1000);

		Z_MoveTo(m_lSwingLevel_Z);
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}

		Ej_MoveTo(m_lStandbyLevel_Ej);
		SetEjectorVacuum(FALSE);

		T_MoveTo(m_lPrePickPos_T);
	}
	else
	{
		T_MoveTo(m_lBondPos_T);
		
		SetPickVacuumZ2(TRUE);
		SetEjectorVacuum(TRUE);
		Z2_MoveTo(m_lPickLevel_Z2);


		Ej_MoveTo(m_lEjectLevel_Ej);
		Sleep(1000);

		Z2_MoveTo(m_lSwingLevel_Z2);
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}

		Ej_MoveTo(m_lStandbyLevel_Ej);
		SetEjectorVacuum(FALSE);

		T_MoveTo(m_lPrePickPos_T);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::BH_BondLeftDie(IPC_CServiceMessage &svMsg)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bReturn = TRUE;
	BOOL bBH2 = FALSE;
	DisplaySequence("BH - begin bond left die");
	if( m_bBHZ2HasDie )
	{
		if( m_bPickVacuumOnZ2==FALSE )
		{
			DisplaySequence("BH - BHZ2 vacuum off already");
			m_bBHZ2HasDie = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		SetPickVacuumZ2(TRUE);
		Sleep(500);
		if( IsMissingDieZ2() )
		{
			DisplaySequence("BH - BHZ2 die lost");
			m_bBHZ2HasDie = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		bBH2 = TRUE;
	}
	else if( m_bBHZ1HasDie )
	{
		if( m_bPickVacuumOn==FALSE )
		{
			DisplaySequence("BH - BHZ1 vacuum off already");
			m_bBHZ1HasDie = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		SetPickVacuum(TRUE);
		Sleep(500);
		if( IsMissingDie() )
		{
			DisplaySequence("BH - BHZ1 die lost");
			m_bBHZ1HasDie = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}
	else
	{
		m_bBHZ2HasDie = FALSE;
		m_bBHZ1HasDie = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( m_ucAtPickDieGrade==0 )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = !bBH2;
	m_bBHZ2TowardsPick	= !bBH2;	//BH1 at PICK = FALSE

	IPC_CServiceMessage stMsg;
	CString szMsg;
	LONG lBlkID = 0;
	INT nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "GetCurrBlockID", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(LONG), &lBlkID);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if( IsBinFull() && lBlkID!=0 )
	{
		szMsg.Format("Is the full frame unloaded?");
		if( HmiMessage_Red_Back(szMsg, "Auto Sort", glHMI_MBX_YESNO)==glHMI_NO )
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	BOOL bFrameExist = FALSE;
	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "CheckFrameExistOnBinTable", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bFrameExist);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	LONG lLeftBlkID = 0;
	stMsg.InitMessage(sizeof(UCHAR), &m_ucAtPickDieGrade);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "BT_GetGradeBinBlock", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(LONG), &lLeftBlkID);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if( bFrameExist==FALSE )
	{
		szMsg.Format("Please manual load bin frame %d firstly.", lLeftBlkID);
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (lBlkID != lLeftBlkID)
	{
		szMsg.Format("Please unload %d and load bin frame %d.", lBlkID, lLeftBlkID);
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//	if( IsExpanderSafeToMove(
	//	BH at prepick and do UpLook firstly
	if (!OpIsBinTableLevelAtDnLevel())				//v4.22T8	//Walsin China
	{
		SetErrorMessage("BH: BT platform at UP level in MOVE_T_PREPICK_TO_PICK_Q !!");
		SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (Z_MoveTo(m_lSwingLevel_Z) != gnOK)
	{
		szMsg.Format("Bond Head Z1 to swing level fail.");
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

#ifdef NU_MOTION
	if (Z2_MoveTo(m_lSwingLevel_Z2) != gnOK)
	{
		szMsg.Format("Bond Head Z2 to swing level fail.");
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
#endif

	if (Ej_MoveTo(m_lStandbyLevel_Ej) != gnOK)
	{
		szMsg.Format("Ejector move to standby level fail.");
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (T_MoveTo(m_lPrePickPos_T) != gnOK)
	{
		szMsg.Format("Bond Arm to prepick position fail.");
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsLowPressure() == TRUE)
	{
		SetStatusMessage("Machine low pressure");
		SetErrorMessage("Machine low pressure");
		szMsg.Format("Machine low pressure.");
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsCoverOpen() == TRUE)
	{
		szMsg.Format("Machine cover open.");
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BOOL bEnableBHUplookPr	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
	BOOL b2DStatus = TRUE;
	if (bEnableBHUplookPr)
	{
		Sleep(100);
		b2DStatus = UplookPrSearchDie_Rpy1(bBH2);
		if(b2DStatus)
		{
			b2DStatus = UplookPrSearchDie_Rpy2(bBH2);
		}
	}


	if (b2DStatus==FALSE )
	{
		szMsg.Format("BHZ %d PR up look fail.", bBH2+1);
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

// BT
	(*m_psmfSRam)["WaferMap"]["Y"]	= m_ulAtPickDieRow;
	(*m_psmfSRam)["WaferMap"]["X"]	= m_ulAtPickDieCol;
	(*m_psmfSRam)["WaferMap"]["Grade"] = m_ucAtPickDieGrade + m_WaferMapWrapper.GetGradeOffset();
	(*m_psmfSRam)["BinTable"]["BlkInUse"] = lBlkID;
	(*m_psmfSRam)["BinTable"]["BinChanged"] = FALSE;

	(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = bBH2; //!bBH2;
	(*m_psmfSRam)["BondPr"]["BinTable At BH1"] = !bBH2;
	LONG lStage = 1;
	stMsg.InitMessage(sizeof(LONG), &lStage);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "BT_BondLeftDie", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if( bReturn==FALSE )
	{
		szMsg.Format("Bin Table index and move error.");
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

//	BH
	Sleep(50);
	CheckCoverOpenInAuto("at BH PrePick state");
	if (IsBHZ1ToPick())			//If BHZ1 towards PICK	
	{
		BA_MoveTo(m_lPickPos_T);
	}
	else
	{
		BA_MoveTo(m_lBondPos_T);
	}
	(*m_psmfSRam)["BondHead"]["AtPrePick"] = 0;

	if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
	{
		LONG lBondZ2 = GetZ2BondLevel(TRUE, TRUE);
		AC_Z2B_MoveToOrSearch(lBondZ2, SFM_WAIT);

		SetPickVacuumZ2(FALSE);				//Turn off BHZ2 vac on BOND side
		if ( (m_bNeutralVacState && !m_bFirstCycle && (m_lBondDelay > 0)) || m_bNGPick )
		{
			SetStrongBlowZ2(TRUE);    //NEUTRAL state
		}		
	}
	else
	{
		LONG lBondZ1 = GetZ1BondLevel(TRUE, TRUE);
		AC_Z1B_MoveToOrSearch(lBondZ1, SFM_WAIT);

		SetPickVacuum(FALSE);				//Else Turn off BHZ1 Vac on BOND side
		if ( (m_bNeutralVacState && (m_lBondDelay > 0)) || m_bNGPick )
		{
			SetStrongBlow(TRUE);    //NEUTRAL state
		}		
	}

	//	V450X16	at bond, update bond grade and check BT block
	m_lAtBondDieGrade	= m_ucAtPickDieGrade;
	m_lAtBondDieRow		= m_ulAtPickDieRow;
	m_lAtBondDieCol		= m_ulAtPickDieCol;
	LONG lBTBlkInUse	= (*m_psmfSRam)["BinTable"]["BlkInUse"];	//	["BinTable"]["LastBlkInUse"]
	LONG lBTGrade		= (*m_psmfSRam)["BinTable"]["BondingGrade"];	//	wrong sort bug fix
	szMsg.Format("BH - map (%ld,%ld)%ld BHZ%d At BOND, Bin grade %ld, Blk %ld", m_lAtBondDieRow, m_lAtBondDieCol, m_lAtBondDieGrade,
			IsBHZ1ToPick()+1, lBTGrade, lBTBlkInUse);

	if ( m_lPickDelay > 0 )
	{
		Sleep(m_lPickDelay);
	}

	if (m_bNeutralVacState && !m_bFirstCycle && (m_lBondDelay > 0))
	{
		//Use PLLM REBEL WB sequence
		if (IsBHZ1ToPick())			//If BHZ1 at PICK	
		{
			SetStrongBlowZ2(FALSE);			//BHZ2 to WEAKBLOW
		}
		else
		{
			SetStrongBlow(FALSE);			//BHZ1 to WEAKBLOW
		}
		Sleep(m_lBondDelay);
	}

	OpUpdateMachineCounters();
	OpUpdateBondedUnit(0);		//Update bonded unit

	AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
	AC_Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
	
	T_MoveTo(m_lPrePickPos_T);
	(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
	CMS896AStn::m_lBondHeadAtSafePos = 1;

//	BT after bonded
	lStage = 2;
	stMsg.InitMessage(sizeof(LONG), &lStage);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "BT_BondLeftDie", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if( bReturn==FALSE )
	{
		szMsg.Format("Bin Table index and move error.");
		HmiMessage_Red_Back(szMsg, "Auto Sort");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

//	BPR	bonded, arm away, do post bond
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BPR_BondLeftDie", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	m_bBHZ2HasDie = FALSE;
	m_bBHZ1HasDie = FALSE;

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}



LONG CBondHead::LoopTestBondArm(IPC_CServiceMessage &svMsg)
{

	extern VOID SaveBHArm1Offset(CString szLogMsg, BOOL bBackUp = TRUE);
	extern VOID SaveBHArm2Offset(CString szLogMsg, BOOL bBackUp = TRUE);
	LONG	lLoopTestCount = 0;

	svMsg.GetMsg(sizeof(LONG), &lLoopTestCount);

	BOOL bReturn = TRUE;
	CStringList szSelection;
	szSelection.AddTail("BH Arm 1(+45deg ClockWise)");
	szSelection.AddTail("BH Arm 1(-45deg AntiClockWise)");
	szSelection.AddTail("BH Arm 2(+45deg ClockWise)");
	szSelection.AddTail("BH Arm 2(-45deg AntiClockWise)");
	LONG lSelection = 0;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	
	//No BHT in Mega Da
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

	if (m_bDisableBH)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	BOOL bBH2 = FALSE;
	LONG lResult = HmiSelection("Please select which BH Arm to Test", "BH Arm Selection", szSelection, lSelection);
	
	switch (lResult)
	{
	case 3:		//BH2
	case 2:
		bBH2 = TRUE;
		break;
	case 1:
	case 0:		//BH1
		bBH2 = FALSE;
		break;
	case -1:	//CANCEL
	default:
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG nErr = 0;

	Z_MoveToHome();
	Z2_MoveToHome();

	if (!bBH2)
	{
		nErr = Z_MoveTo(m_lSwingLevel_Z);
	}
	else
	{
		nErr = Z2_MoveTo(m_lSwingLevel_Z2);
	}

	if (nErr)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if (m_bMS100EjtXY && m_bEnableMS100EjtXY)
	{
		if (EjX_IsPowerOn() && EjY_IsPowerOn())
		{
			LONG lEjtX = m_lCDiePos_EjX;
			LONG lEjtY = m_lCDiePos_EjY;

			LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
			LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
			LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
			LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

			if (bBH2)
			{
				lEjtX = lEjtX + lCollet2OffsetX;
				lEjtY = lEjtY + lCollet2OffsetY;
			}
			else
			{
				lEjtX = lEjtX + lCollet1OffsetX;
				lEjtY = lEjtY + lCollet1OffsetY;
			}

			EjX_MoveTo(lEjtX, SFM_NOWAIT);
			EjY_MoveTo(lEjtY, SFM_WAIT);
			EjX_Sync();
		}
	}

	LONG lOffsetT = _round(625000/4);
	if ((lResult == 1) || (lResult == 3))
	{
		lOffsetT = -lOffsetT;
	}

	for (LONG i = 0; i < lLoopTestCount; i++)
	{
		if (!bBH2)
		{
			T_MoveTo(m_lPickPos_T);

			SetPickVacuum(TRUE);
			SetEjectorVacuum(TRUE);
			Z_MoveTo(m_lPickLevel_Z);
		}
		else
		{
			T_MoveTo(m_lBondPos_T);
		
			SetPickVacuumZ2(TRUE);
			SetEjectorVacuum(TRUE);
			Z2_MoveTo(m_lPickLevel_Z2);
		}

		Sleep(200);
		typedef struct {
			BOOL	bStatus;
			double	dOffsetX;
			double 	dOffsetY;
			DOUBLE	dAngle;
		} SRCH_RESULT;
		SRCH_RESULT	stResult;

		IPC_CServiceMessage svMsg;
		//Search the reference die which user pick the die
		LONG nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchDie_NoMove", svMsg);
		while (1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, svMsg);
				svMsg.GetMsg(sizeof(SRCH_RESULT), &stResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}


		if ((i > 0) && stResult.bStatus)
		{
			CString szLogMsg;
			if ((lResult == 1) || (lResult == 3))
			{
				szLogMsg.Format("AntiClockWise, offsetX = %.1f, offsetY = %.1f, Angle = %.1f",
								 stResult.dOffsetX, stResult.dOffsetY, stResult.dAngle);
			}
			else
			{
				szLogMsg.Format("ClockWise, offsetX = %.1f, offsetY = %.1f, Angle = %.1f",
								 stResult.dOffsetX, stResult.dOffsetY, stResult.dAngle);
			}
			if (!bBH2)
			{
				SaveBHArm1Offset(szLogMsg);
			}
			else
			{
				SaveBHArm2Offset(szLogMsg);
			}
		}


		if (!bBH2)
		{
			nErr = Z_MoveTo(m_lSwingLevel_Z);
			T_MoveTo(m_lPickPos_T + lOffsetT);
			Sleep(200);
		}
		else
		{
			nErr = Z2_MoveTo(m_lSwingLevel_Z2);
			T_MoveTo(m_lBondPos_T + lOffsetT);
			Sleep(200);
		}
	}

	Z_MoveToHome();
	Z2_MoveToHome();
	T_MoveTo(m_lPrePickPos_T);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::SearchColletHoleTest(IPC_CServiceMessage &svMsg)
{
	//CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	SetBhTReadyForWPR(FALSE);
	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	if (HmiMessage_Red_Back("BH1?", "Prompt", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER))		//If BHZ1 at Bond	
	{
			//SetEjectorVacuum(FALSE);
			if (m_bComplete_T == FALSE)
			{
				T_Sync();
				m_bComplete_T = TRUE;
			}
			if (m_bComplete_Z == FALSE)
			{	
				Z_Sync();
				m_bComplete_Z = TRUE;
			}
			if (m_bComplete_Z2 == FALSE)
			{	
				Z2_Sync();
				m_bComplete_Z2 = TRUE;
			}
			//pWaferTable->SwitchColletOffset(TRUE);
			//OpSwitchEjectorTableXY(TRUE,SFM_WAIT);
			T_MoveTo(m_lPickPos_T);
			Z_MoveTo(m_lPickLevel_Z);
			TakeTime(LC7);
			SetSearchMarkReady(FALSE);
			SetSearchMarkDone(FALSE);
			m_lBH1MarkCount = 0;
			m_bDoColletSearch = TRUE;
			Sleep(m_lCheckBHMarkDelay);
			GetEncoderValue();
			m_lColletHoleEnc_T = m_lEnc_T;
			m_lColletHoleEnc_Z = m_lEnc_Z;
			//AfxMessageBox("Ready to capture an image after delay1");
			pWaferPr->OpAutoSearchColletHoleWithEpoxyPattern();
			SetBhTReadyForWPR(TRUE);
			(*m_psmfSRam)["BondHead"]["ColletTest"]["BH Mark 2"] = FALSE;
			DisplaySequence("BH - BH1 Collet");
	}
	else
	{

			//SetEjectorVacuum(FALSE);
			if (m_bComplete_T == FALSE)
			{
				T_Sync();
				m_bComplete_T = TRUE;
			}
			if (m_bComplete_Z == FALSE)
			{	
				Z_Sync();
				m_bComplete_Z = TRUE;
			}
			if (m_bComplete_Z2 == FALSE)
			{	
				Z2_Sync();
				m_bComplete_Z2 = TRUE;
			}
			//pWaferTable->SwitchColletOffset(FALSE);
			//OpSwitchEjectorTableXY(FALSE,SFM_WAIT);
			T_MoveTo(m_lBondPos_T);
			Z2_MoveTo(m_lPickLevel_Z2);
			SetSearchMarkReady(FALSE);
			SetSearchMarkDone(FALSE);
			m_lBH2MarkCount = 0;
			m_bDoColletSearch = TRUE;
			Sleep(m_lCheckBHMarkDelay);
			//AfxMessageBox("Ready to capture an image after delay2");
			GetEncoderValue();
			m_lColletHoleEnc_T = m_lEnc_T;
			m_lColletHoleEnc_Z = m_lEnc_Z2;
			pWaferPr->OpAutoSearchColletHoleWithEpoxyPattern();
			SetBhTReadyForWPR(TRUE);
			(*m_psmfSRam)["BondHead"]["ColletTest"]["BH Mark 2"] = TRUE;
			DisplaySequence("BH - BH2 Collet");
	}

	while(1)
	{
		if(WaitSearchMarkReady())
		{
			break;
		}
	}
	//Sleep(100);
	m_bDoColletSearch = FALSE;
	TakeTime(LC10);
	//CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	if (!IsBHZ1ToPick())		//If BHZ1 at Bond	
	{
		Z_MoveTo(m_lSwingLevel_Z);
		//pWaferTable->SwitchColletOffset(FALSE);
		T_MoveTo(m_lBondPos_T);
	}
	else
	{
		Z2_MoveTo(m_lSwingLevel_Z2);
		//pWaferTable->SwitchColletOffset(TRUE);
		T_MoveTo(m_lPickPos_T);
	}
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::SearchBondColletEpoxyCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	if( LoopCheckCoverOpen("Auto Learn Bond Epoxy")==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	CStringList szList;
    szList.AddTail("BH1");
    szList.AddTail("BH2");
  
    CString szContent = "Please select BH No";
    CString szTitle =  "Auto Learn Bond Collet Epoxy";
	LONG lSet = 0;
 
    LONG lSelection = HmiSelection(szContent, szTitle, szList, lSet);
	LONG lBH1 = !lSelection;
	OpPrestartBPRColletHoleCheck(lBH1);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

BOOL CBondHead::CleanColletCmd(IPC_CServiceMessage &svMsg)
{
	CString szMsg, szTitle;
	LONG lTempBHPosT;
	LONG lTempWTPosX, lTempWTPosY, lTempWTPosT;
	
	Z_MoveTo(0);
	Z2_MoveTo(0);
	GetEncoderValue();
	lTempBHPosT = m_lEnc_T;

	GetWTEncoderValue(&lTempWTPosX, &lTempWTPosY, &lTempWTPosT);

	MoveWaferTableNoCheck(0, -80000);
	Sleep(50);

	BH_SubMoveBHToPick(TRUE, 180000);
	Sleep(50);

	SetStrongBlowZ2(TRUE);
	Sleep(50);
	SetStrongBlow(TRUE);
	Sleep(50);

	szMsg = "Press OK to finish!";
	szTitle = "Clean Collet";
	HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	
	T_Profile(SETUP_PROF);
	T_SMoveTo(lTempBHPosT);
	T_Profile(NORMAL_PROF);

	Sleep(50);

	MoveWaferTableNoCheck(lTempWTPosX, lTempWTPosY);
	Sleep(50);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::SetDefaultKValue(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lDieType = 0;
	svMsg.GetMsg(sizeof(LONG), &lDieType);

	m_lEjectorKCount = 30000;
	m_lEjectorKOffsetResetCount = 200;
	m_lEjSubRegionKCount = 10000;
	m_lEjSubRegionSKCount = 1000;
	m_lEjSubRegionSKOffset = 0;

	if (lDieType == 0) //None Die
	{
		m_lEjectorKOffsetLimit = 120;
		m_lEjectorKOffset = 0;
		m_lEjSubRegionKOffset = 0;
		m_lEjectorKOffsetForBHZ1MD = 0;
		m_lEjectorKOffsetForBHZ2MD = 0;
	}
	else if (lDieType == 1) //Small Die
	{
		m_lEjectorKOffsetLimit = 100;
		m_lEjectorKOffset = 1;
		m_lEjSubRegionKOffset = 1;
		m_lEjectorKOffsetForBHZ1MD = 3;
		m_lEjectorKOffsetForBHZ2MD = 3;
	}
	else if (lDieType == 2) //Large Die
	{
		m_lEjectorKOffsetLimit = 150;
		m_lEjectorKOffset = 5;
		m_lEjSubRegionKOffset = 10;
		m_lEjectorKOffsetForBHZ1MD = 5;
		m_lEjectorKOffsetForBHZ2MD = 5;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::MoveContourProfileLearnCollet(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bResult = FALSE;
	BOOL bTakeLog = FALSE;
	BOOL bTakeImage = FALSE;
	LONG lCycleCounter = 0;
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szBH1Result, szBH2Result, szText, szTime;
	CStdioFile oLogFile;

	if (HmiMessage("Start to Collet Hole Shift Test?", "Collet Hole Shift Test", glHMI_MBX_YESNO) == glHMI_NO)
	{
		T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	GetEncoderValue();
	LONG lOldBHZ = m_lEnc_Z;
	LONG lOldBHZ2 = m_lEnc_Z2;

	CMS896AApp::m_bStopAlign = FALSE;
	pBinTable->MoveTableToEmptyPosition();
	Z_MoveTo(m_lBondLevel_Z + 5000, SFM_WAIT);
	Sleep(20);
	Z2_MoveTo(m_lBondLevel_Z2 + 5000, SFM_WAIT);
	Sleep(20);
	T_MoveTo(m_lPickPos_T, SFM_WAIT);
	Sleep(20);

	while(1)
	{
		if (lCycleCounter > 0 && lCycleCounter % 10 == 0)
		{
			bTakeImage = TRUE;
		}
		//Bond Head 1
		BA_MoveTo(m_lBondPos_T, SFM_WAIT);
		Sleep(5);
		if (bTakeImage)
		{
			bResult = pBondPr->OpAutoSearchBPRColletHoleWithEpoxyPattern(FALSE);
		}
		Sleep(15);
		if (bTakeImage)
		{
			GetEncoderValue();
			szBH1Result.Format("%d,%d,%d,%d,%d", m_lEnc_T, pBondPr->GetBondPosOffsetXPixel(), pBondPr->GetBondPosOffsetYPixel(), pBinTable->GetBHZ1BondPosOffsetX(), pBinTable->GetBHZ1BondPosOffsetY());
		}

		//Bond Head 2
		BA_MoveTo(m_lPickPos_T, SFM_WAIT);
		Sleep(5);
		if (bTakeImage)
		{
			bResult = pBondPr->OpAutoSearchBPRColletHoleWithEpoxyPattern(TRUE);
		}
		Sleep(15);
		if (bTakeImage)
		{
			GetEncoderValue();
			szBH2Result.Format("%d,%d,%d,%d,%d", m_lEnc_T, pBondPr->GetBondPosOffsetXPixel(), pBondPr->GetBondPosOffsetYPixel(), pBinTable->GetBHZ2BondPosOffsetX(), pBinTable->GetBHZ2BondPosOffsetY());
		}

		if (bTakeImage)
		{
			CTime theTime = CTime::GetCurrentTime();
			szTime = theTime.Format("%Y-%m-%d (%H:%M:%S),");
			szText = szText + szTime + szBH1Result + "," + szBH2Result + "\n";
			if (!bResult)
			{
				CMS896AApp::m_bStopAlign = TRUE;
			}
			else
			{
				bTakeImage = FALSE;
			}
		}

		lCycleCounter++;
		if (lCycleCounter % 100 == 0 || pApp->IsStopAlign())
		{
			bTakeLog = TRUE;
		}

		if (bTakeLog)
		{
			CString szPath = "c:\\Mapsorter\\UserData\\BondPrILCLearnColletOffset.txt";
			BOOL bAddHeader = FALSE;
			CString szLine;

			if (_access(szPath, 0) == -1)
			{
				bAddHeader = TRUE;
			}

			if (oLogFile.Open(szPath, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText))
			{
				if (bAddHeader)
				{
					szLine = "Time,BH1_T_ENC,BH1_COOR_X,BH1_COOR_Y,BH1_OFFSET_X,BH1_OFFSET_Y,BH2_T_ENC,BH2_COOR_X,BH2_COOR_Y,BH2_OFFSET_X,BH2_OFFSET_Y\n";
					oLogFile.WriteString(szLine);						
				}
				oLogFile.SeekToEnd();
				oLogFile.WriteString(szText);
				oLogFile.Close();
			}
			bTakeLog = FALSE;
			szText.Empty();

			if (pApp->IsStopAlign())
			{
				break;
			}
		}
	}
	
	Z_MoveTo(lOldBHZ, SFM_WAIT);
	Sleep(20);
	Z2_MoveTo(lOldBHZ, SFM_WAIT);
	Sleep(20);
	T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
	Sleep(20);

	HmiMessage("Collet Hole Shift Test Finish!!!", "Collet Hole Shift Test");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::MHCTesting(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szText;
	LONG lMHCValue;
	m_szMHCresult.Empty();

	for(int i = 0; i < 20; i++)
	{
		lMHCValue = AgmpMotionHealthCheck(&m_stBHAxis_T, m_szMHCresult, m_bIsMHCTrained);
		Sleep(10);
		T_MoveTo(0);
		Sleep(20);
		T_MoveTo(312500);
		Sleep(20);
		T_MoveTo(0);
		Sleep(20);
		T_MoveTo(-312500);
		Sleep(20);
		T_MoveTo(0);
		Sleep(20);
		if (!m_bIsMHCTrained && lMHCValue == 1)
		{
			m_bIsMHCTrained = TRUE;
			SaveBhData();
		}
	}

	if (m_szMHCresult.IsEmpty())
	{
		szText.Format("MHC Return Error Value = %d", lMHCValue);
		HmiMessage(szText, "MHC Test");
	}
	else
	{
		HmiMessage(m_szMHCresult, "MHC Test Finished");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::EjElevatorZGoToDownPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	EjCap_MoveTo(m_lStandby_EJ_Cap);
	EjElevator_MoveTo(m_lStandby_EJ_Elevator);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BinEjElevatorZGoToDownPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	BinEjCap_MoveTo(m_lStandby_Bin_EJ_Cap);

	LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJ_ELEVATOR, MS896A_CFG_CH_MIN_DISTANCE);								
	LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_BIN_EJ_ELEVATOR, MS896A_CFG_CH_MAX_DISTANCE);								

	if (m_dEjElevatorRes == 1.00)
	//if ((m_lStandby_Bin_EJ_Elevator > lMinPos) && (m_lStandby_Bin_EJ_Elevator < lMaxPos))
	{
		BinEjElevator_MoveTo(m_lStandby_Bin_EJ_Elevator);
	}
	else
	{
		DOUBLE dTargetHomeZ = -35000.0 / m_dEjElevatorRes;
		LONG lTargetZ = (LONG) dTargetHomeZ;
		BinEjElevator_MoveTo(lTargetZ);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::EjElevatorZGoToUpPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	EjCap_MoveTo(m_lUpLevel_EJ_Cap);
	EjElevator_MoveTo(m_lUpLevel_EJ_Elevator);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::BinEjElevatorZGoToUpPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	BinEjCap_MoveTo(m_lUpLevel_Bin_EJ_Cap);
	BinEjElevator_MoveTo(m_lUpLevel_Bin_EJ_Elevator);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::NVCSingleBondDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	DOUBLE dbStartEjTMove, dbStartBinEjTMove, dbStartEjUp;

	EjT_MoveTo(m_lUpLevel_EJ_Theta, SFM_NOWAIT);
	dbStartEjTMove		= GetTime();
	BinEjT_MoveTo(m_lUpLevel_Bin_EJ_Theta, SFM_NOWAIT);
	dbStartBinEjTMove	= GetTime();

	m_bComplete_Ej		= FALSE;
	m_bComplete_BinEj	= FALSE;

	if (m_lHeadBondDelay > m_lHeadPickDelay)
	{
		if (m_lHeadPickDelay > 0)
		{	
			LONG lWaitTime = m_lUpTime_Ej_Theta + m_lHeadPickDelay - m_lTime_Ej;
			WaitForDelay(lWaitTime, dbStartEjTMove);
		}
		Ej_MoveTo(m_lEjectLevel_Ej, SFM_NOWAIT);
		m_dEjDnStartTime = GetTime();

		if (m_lHeadBondDelay > 0)
		{
			LONG lWaitTime = m_lUpTime_BinEj_Theta + m_lHeadBondDelay - m_lTime_BinEj;
			WaitForDelay(lWaitTime, dbStartBinEjTMove);
		}
		BinEj_MoveTo(m_lUpLevel_Bin_EJ_Pin, SFM_NOWAIT);
		m_dBinEjDnStartTime = GetTime();
	}
	else
	{
		if (m_lHeadBondDelay > 0)
		{
			LONG lWaitTime = m_lUpTime_BinEj_Theta + m_lHeadBondDelay - m_lTime_BinEj;
			WaitForDelay(lWaitTime, dbStartBinEjTMove);
		}
		BinEj_MoveTo(m_lUpLevel_Bin_EJ_Pin, SFM_NOWAIT);
		m_dBinEjDnStartTime = GetTime();

		if (m_lHeadPickDelay > 0)
		{	
			LONG lWaitTime = m_lUpTime_Ej_Theta + m_lHeadPickDelay - m_lTime_Ej;
			WaitForDelay(lWaitTime, dbStartEjTMove);
		}
		Ej_MoveTo(m_lEjectLevel_Ej, SFM_NOWAIT);
		m_dEjDnStartTime = GetTime();
	}

	//Ej and EjT arrive to UP Level
	if (m_bComplete_Ej == FALSE || m_bComplete_BinEj == FALSE)
	{
		if (m_lHeadPickDelay <= m_lUpTime_Ej_Theta)
		{
			WaitForDelay(m_lUpTime_Ej_Theta, dbStartEjTMove);
		}
		if (m_lHeadBondDelay <= m_lUpTime_BinEj_Theta)
		{
			WaitForDelay(m_lUpTime_BinEj_Theta, dbStartBinEjTMove);
		}

		//Ejector Pin Arrive
		WaitForDelay(m_lTime_Ej, m_dEjDnStartTime);
		WaitForDelay(m_lTime_BinEj, m_dBinEjDnStartTime);

		m_bComplete_Ej = TRUE;
		m_bComplete_BinEj = TRUE;
	}

	//Wait for PICK/BOND delay
	dbStartEjUp = GetTime();
	if (m_lPickDelay < m_lBondDelay)
	{
		WaitForDelay(m_lPickDelay, dbStartEjUp);
	}
	else
	{
		WaitForDelay(m_lBondDelay, dbStartEjUp);
	}

	Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
	m_dEjDnStartTime = GetTime();
	BinEj_MoveTo(m_lStandby_Bin_EJ_Pin, SFM_NOWAIT);
	m_dBinEjDnStartTime = GetTime();

	if (m_lArmBondDelay > m_lArmPickDelay)
	{
		if (m_lArmPickDelay > 0)
		{
			WaitForDelay(m_lArmPickDelay, m_dEjDnStartTime);
		}
		EjT_MoveTo(m_lStandby_EJ_Theta, SFM_NOWAIT);
		dbStartEjTMove = GetTime();

		if (m_lArmBondDelay > 0)
		{
			WaitForDelay(m_lArmBondDelay, m_dBinEjDnStartTime);
		}
		BinEjT_MoveTo(m_lStandby_Bin_EJ_Theta, SFM_NOWAIT);
		dbStartBinEjTMove = GetTime();
	}
	else
	{
		if (m_lArmBondDelay > 0)
		{
			WaitForDelay(m_lArmBondDelay, m_dBinEjDnStartTime);
		}
		BinEjT_MoveTo(m_lStandby_Bin_EJ_Theta, SFM_NOWAIT);
		dbStartBinEjTMove = GetTime();
		
		if (m_lArmPickDelay > 0)
		{
			WaitForDelay(m_lArmPickDelay, m_dEjDnStartTime);
		}
		EjT_MoveTo(m_lStandby_EJ_Theta, SFM_NOWAIT);
		dbStartEjTMove = GetTime();
	}

	m_bComplete_Ej		= FALSE;
	m_bComplete_BinEj	= FALSE;

	//Ej and EjT arrive to Standby Level
	Ej_Sync();
	BinEj_Sync();
	EjT_Sync();
	BinEjT_Sync();

	m_bComplete_Ej = TRUE;
	m_bComplete_BinEj = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::EjectorThetaShakeVisionTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szTitle, szLoop, szDelay;
	LONG lLoop = 0, lSwingInDelay = 0, lSwingOutDelay = 0;

	CString szSelection1 = "Wafer Ejector";
	CString szSelection2 = "Bin Ejector";
	CString szSelection3 = "Cancel";
	INT nArmSelection = HmiMessage("Please choose which Ejector to be tested.", "Ejector Theta Shake", glHMI_MBX_TRIPLEBUTTON, 
		glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2, &szSelection3);

	if (nArmSelection == 1)
	{
		szTitle = "Please input loop count";
		HmiStrInputKeyboard(szTitle, szLoop);
		lLoop = atoi(LPCTSTR(szLoop));

		szTitle = "Please input swing in wait time";
		HmiStrInputKeyboard(szTitle, szDelay);
		lSwingInDelay = atoi(LPCTSTR(szDelay));

		szTitle = "Please input swing out wait time";
		HmiStrInputKeyboard(szTitle, szDelay);
		lSwingOutDelay = atoi(LPCTSTR(szDelay));

		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		DOUBLE dbStartEjTMove;
		for(int i = 0; i < lLoop; i++)
		{
			EjT_MoveTo(m_lUpLevel_EJ_Theta, SFM_NOWAIT);
			dbStartEjTMove = GetTime();
			if (m_lHeadPickDelay > 0)
			{	
				LONG lWaitTime = m_lUpTime_Ej_Theta + m_lHeadPickDelay - m_lTime_Ej;
				WaitForDelay(lWaitTime, dbStartEjTMove);
			}
			Ej_MoveTo(m_lEjectLevel_Ej);

			Sleep(lSwingInDelay);

			GetEncoderValue();
			GetCommanderValue();
			if (pBondPr->BprSerachWaferEjectorPin(m_lEnc_EjT, m_lCmd_EjT) == FALSE)
			{
				HmiMessage("Error: BPR Search Wafer Ej Pin fail");
				break;
			}

			EjT_MoveTo(m_lStandby_EJ_Theta);
			Ej_MoveTo(m_lStandbyLevel_Ej);
			Sleep(lSwingOutDelay);
		}
	}
	else if (nArmSelection == 5)
	{
		szTitle = "Please input loop count";
		HmiStrInputKeyboard(szTitle, szLoop);
		lLoop = atoi(LPCTSTR(szLoop));

		szTitle = "Please input delay time";
		HmiStrInputKeyboard(szTitle, szDelay);
		lSwingInDelay = atoi(LPCTSTR(szDelay));

		szTitle = "Please input swing out wait time";
		HmiStrInputKeyboard(szTitle, szDelay);
		lSwingOutDelay = atoi(LPCTSTR(szDelay));

		CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
		DOUBLE dbStartBinEjTMove;

		for (int i = 0; i < lLoop; i++)
		{
			BinEjT_MoveTo(m_lUpLevel_Bin_EJ_Theta);
			dbStartBinEjTMove = GetTime();
			if (m_lHeadPickDelay > 0)
			{	
				LONG lWaitTime = m_lUpTime_BinEj_Theta + m_lHeadBondDelay - m_lTime_BinEj;
				WaitForDelay(lWaitTime, dbStartBinEjTMove);
			}
			BinEj_MoveTo(m_lUpLevel_Bin_EJ_Pin);

			Sleep(lSwingInDelay);

			GetEncoderValue();
			GetCommanderValue();
			if (pWaferPr->WprSerachBinEjectorPin(m_lEnc_BinEjT, m_lCmd_BinEjT) == FALSE)
			{
				HmiMessage("Error: WPR Search Bin Ej Pin fail");
				break;
			}

			BinEjT_MoveTo(m_lStandby_Bin_EJ_Theta);
			BinEj_MoveTo(m_lStandby_Bin_EJ_Pin);
			Sleep(lSwingOutDelay);
		}
	}

	EjT_MoveTo(m_lStandby_EJ_Theta);
	Ej_MoveTo(m_lStandbyLevel_Ej);
	BinEjT_MoveTo(m_lStandby_Bin_EJ_Theta);
	BinEj_MoveTo(m_lStandby_Bin_EJ_Pin);

	HmiMessage("Test is done.");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}