///////////////////////////////////////////////////////////////////////
// WL_Command.cpp : HMI Registered Command of the CWaferLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			Barry Chu	
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
///////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "HmiDataManager.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "MachineStat.h"
#include "CTmpChange.h"
#include "WaferTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Registered HMI Command 
VOID CWaferLoader::RegisterVariables()
{
	try
	{
		// Register Variables
		RegVariable(_T("WL_bUseBLAsLoaderZ"),			&m_bUseBLAsLoaderZ);		//v4.31T11
		RegVariable(_T("WL_bBLAsLoaderZDisablePreload"), &m_bUseBLAsLoaderZDisablePreload);	//v4.39T9

		RegVariable(_T("WL_lEnc_Z"),				&m_lEnc_Z);
		RegVariable(_T("WL_lEnc_X"),				&m_lEnc_X);
		RegVariable(_T("WL_lEnc_Z2"),				&m_lEnc_Z2);
		RegVariable(_T("WL_lEnc_X2"),				&m_lEnc_X2);
		RegVariable(_T("WL_lEnc_ExpZ"),				&m_lEnc_ExpZ);

		RegVariable(_T("WL_bHomeSnr_X"),			&m_bHomeSnr_X);
		RegVariable(_T("WL_bULimitSnr_X"),			&m_bULimitSnr_X);
		RegVariable(_T("WL_bLLimitSnr_X"),			&m_bLLimitSnr_X);

		RegVariable(_T("WL_bHomeSnr_Z"),			&m_bHomeSnr_Z);
		RegVariable(_T("WL_bULimitSnr_Z"),			&m_bULimitSnr_Z);
		RegVariable(_T("WL_bLLimitSnr_Z"),			&m_bLLimitSnr_Z);

		RegVariable(_T("WL_bHomeSnr_X2"),			&m_bHomeSnr_X2);
		RegVariable(_T("WL_bULimitSnr_X2"),			&m_bULimitSnr_X2);
		RegVariable(_T("WL_bLLimitSnr_X2"),			&m_bLLimitSnr_X2);

		RegVariable(_T("WL_bHomeSnr_Z2"),			&m_bHomeSnr_Z2);
		RegVariable(_T("WL_bULimitSnr_Z2"),			&m_bULimitSnr_Z2);
		RegVariable(_T("WL_bLLimitSnr_Z2"),			&m_bLLimitSnr_Z2);

		RegVariable(_T("WL_bUseMagazine3AsX"),		&m_bUseMagazine3AsX);
		RegVariable(_T("WL_bUseAMagazineAsG"),		&m_bUseAMagazineAsG);
		RegVariable(_T("WL_lMagazineG1CurrentSlotNo"),	&m_lMagazineG1CurrentSlotNo);
		RegVariable(_T("WL_lMagazineG2CurrentSlotNo"),	&m_lMagazineG2CurrentSlotNo);

		RegVariable(_T("WL_bIsEnabled"),			&m_bIsEnabled);
		RegVariable(_T("WL_bIsGearType"),			&m_bIsGearType);
		RegVariable(_T("WL_lExpanderType"),			&m_lExpanderType);

		//Common
		RegVariable(_T("WL_bMagazineExist"),		&m_bMagazineExist);	
		RegVariable(_T("WL_bFrameDetect"),			&m_bFrameDetect);	
		RegVariable(_T("WL_bFrameJam"),				&m_bFrameJam);
		RegVariable(_T("WL_bWaferFrameDetect"),		&m_bWaferFrameDetect);	
		RegVariable(_T("WL_bScopeLevel"),			&m_bScopeLevel);	
		RegVariable(_T("WL_lStepOnLoadPos"),		&m_lStepOnLoadPos);
		RegVariable(_T("WL_lStepOnUnloadPos"),		&m_lStepOnUnloadPos);
		RegVariable(_T("WL_szBarcodeName"),			&m_szBarcodeName);
		RegVariable(_T("WL_szBarcodeName2"),		&m_szBarcodeName2);
		RegVariable(_T("WL_bNoSensorCheck"),		&m_bNoSensorCheck);

		RegVariable(_T("WL_lStepOnLoadPos2"),		&m_lStepOnLoadPos2);
		RegVariable(_T("WL_lStepOnUnloadPos2"),		&m_lStepOnUnloadPos2);

		//Gear version		
		RegVariable(_T("WL_bExpanderClose"),		&m_bExpanderClose);	
		RegVariable(_T("WL_bExpanderOpen"),			&m_bExpanderOpen);	
		RegVariable(_T("WL_bExpanderLock"),			&m_bExpanderLock);	
		RegVariable(_T("WL_bFrameExist"),			&m_bFrameExist);	
		RegVariable(_T("WL_lExpDCMotorDacValue"),	&m_lExpDCMotorDacValue);

		RegVariable(_T("WL_bFrontGateSensor"),		&m_bFrontGateSensor);
		RegVariable(_T("WL_bBackGateSensor"),		&m_bBackGateSensor);

		RegVariable(_T("WL_bMagazineExist2"),		&m_bMagazineExist2);
		RegVariable(_T("WL_bFrameDetect2"),			&m_bFrameDetect2);
		RegVariable(_T("WL_bWaferFrameDetect2"),	&m_bWaferFrameDetect2);
		RegVariable(_T("WL_bExpander2Close"),		&m_bExpander2Close);
		RegVariable(_T("WL_bExpander2Open"),		&m_bExpander2Open);
		RegVariable(_T("WL_bExpander2Lock"),		&m_bExpander2Lock);
		RegVariable(_T("WL_bFrameExist2"),			&m_bFrameExist2);
		RegVariable(_T("WL_bFrameJam2"),			&m_bFrameJam2);
		RegVariable(_T("WL_bLeftLoaderCoverSensor"),		&m_bLeftLoaderCoverSensor);
		RegVariable(_T("WL_bRightLoaderCoverSensor"),		&m_bRightLoaderCoverSensor);
		RegVariable(_T("WL_bDisableWT2InAutoBondMode"),		&m_bDisableWT2InAutoBondMode);
		//Vacuum version	
		RegVariable(_T("WL_bFramePosition"),		&m_bFramePosition);	
		RegVariable(_T("WL_bFrameProtection"),		&m_bFrameProtection);	
		RegVariable(_T("WL_bExpanderVacuum"),		&m_bExpanderVacuum);	
		RegVariable(_T("WL_bExpanderLevel"),		&m_bExpanderLevel);	
		RegVariable(_T("WL_lVacuumBefore"),			&m_lVacuumBefore);
		RegVariable(_T("WL_lVacuumAfter"),			&m_lVacuumAfter);
		RegVariable(_T("WL_lWTAlignFrameCount"),	&m_lWTAlignFrameCount);
		//YEaly MS100 buffer Table sensors
		RegVariable(_T("WL_bFrameExistInUpperSlot"),	&m_bFrameExistInUpperSlot);
		RegVariable(_T("WL_bFrameExistInLowerSlot"),	&m_bFrameExistInLowerSlot);
		RegVariable(_T("WL_bFrameExistOnBufferTable"),	&m_bFrameExistOnBufferTable);
		RegVariable(_T("WL_bExpanderStatus"),			&m_bExpanderStatus);

		RegVariable(_T("WL_lTotalSlotNo"),			&m_lTotalSlotNo);	
		RegVariable(_T("WL_dSlotPitch"),			&m_dSlotPitch);

		RegVariable(_T("WL_lSkipSlotNo"),			&m_lSkipSlotNo);	
		RegVariable(_T("WL_lCurrentSlotNo"),		&m_lCurrentSlotNo);
		RegVariable(_T("WL_lCurrentMagNo"),			&m_lCurrentMagNo);
		RegVariable(_T("WL_lUnloadOffset"),			&m_lUnloadOffset);

		RegVariable(_T("WL_lSkipSlotNo2"),			&m_lSkipSlotNo2);	
		RegVariable(_T("WL_lCurrentSlotNo2"),		&m_lCurrentSlotNo2);
		RegVariable(_T("WL_lCurrentMagNo2"),		&m_lCurrentMagNo2);
		RegVariable(_T("WL_lUnloadOffset2"),		&m_lUnloadOffset2);
		RegVariable(_T("WL_lCurrentMagazineX1SlotNo"),	&m_lCurrentMagazineX1SlotNo);
		RegVariable(_T("WL_lCurrentMagazineX2SlotNo"),	&m_lCurrentMagazineX2SlotNo);
	
		RegVariable(_T("WL_lHotBlowOpenTime"),		&m_lHotBlowOpenTime);	
		RegVariable(_T("WL_lHotBlowCloseTime"),		&m_lHotBlowCloseTime);	
		RegVariable(_T("WL_lMylarCoolTime"),		&m_lMylarCoolTime);
		RegVariable(_T("WL_lOpenDriveInTime"),		&m_lOpenDriveInTime);
		RegVariable(_T("WL_lCloseDriveInTime"),		&m_lCloseDriveInTime);
		RegVariable(_T("WL_lUnloadOpenExtraTime"),	&m_lUnloadOpenExtraTime);		//v4.59A17
		//v4.38T6	//Knowles MS109 Penang
		RegVariable(_T("WL_bExpDCMotorEncoder"),	&m_bExpDCMotorUseEncoder);
		RegVariable(_T("WL_lExpEncoderOpenOffsetZ"), &m_lExpEncoderOpenOffsetZ);		
		RegVariable(_T("WL_lExpEncoderCloseOffsetZ"), &m_lExpEncoderCloseOffsetZ);

		RegVariable(_T("WL_lHotBlowOpenTime2"),		&m_lHotBlowOpenTime2);	
		RegVariable(_T("WL_lHotBlowCloseTime2"),	&m_lHotBlowCloseTime2);	
		RegVariable(_T("WL_lMylarCoolTime2"),		&m_lMylarCoolTime2);
		RegVariable(_T("WL_lOpenDriveInTime2"),		&m_lOpenDriveInTime2);
		RegVariable(_T("WL_lCloseDriveInTime2"),	&m_lCloseDriveInTime2);
		
		RegVariable(_T("WL_lUnloadPhyPosX"),		&m_lUnloadPhyPosX);	
		RegVariable(_T("WL_lUnloadPhyPosY"),		&m_lUnloadPhyPosY);	

		RegVariable(_T("WL_lHomeDiePhyPosX"),		&m_lHomeDiePhyPosX);	
		RegVariable(_T("WL_lHomeDiePhyPosY"),		&m_lHomeDiePhyPosY);	
		RegVariable(_T("WL_lWT2HomeDiePhyPosX"),	&m_lWT2HomeDiePhyPosX);	
		RegVariable(_T("WL_lWT2HomeDiePhyPosY"),	&m_lWT2HomeDiePhyPosY);	

		RegVariable(_T("WL_lUnloadPhyPosX2"),		&m_lUnloadPhyPosX2);	
		RegVariable(_T("WL_lUnloadPhyPosY2"),		&m_lUnloadPhyPosY2);	

		RegVariable(_T("WL_lAngleLHSCount"),		&m_lAngleLHSCount);	
		RegVariable(_T("WL_lAngleRHSCount"),		&m_lAngleRHSCount);	
		RegVariable(_T("WL_lSprialSize"),			&m_lSprialSize);	
		RegVariable(_T("WL_lHomeDiePattern"),		&m_lPRSrchID);	
		RegVariable(_T("WL_lAlignWaferAngleMode"),	&m_lAlignWaferAngleMode);	//v3.02T7

		RegVariable(_T("WL_bEnableContourGT"),		&m_bEnableContourGT);
		RegVariable(_T("WL_bContourPreloadMap"),	&m_bContourPreloadMap);
		RegVariable(_T("WL_dContourExtraMarginX"),	&m_dContourExtraMarginX);
		RegVariable(_T("WL_dContourExtraMarginY"),	&m_dContourExtraMarginY);
		RegVariable(_T("WL_bUseContour"),			&m_bUseContour);
		RegVariable(_T("WL_bUseBarcode"),			&m_bUseBarcode);	
		RegVariable(_T("WL_lScanRange"),			&m_lScanRange);
		RegVariable(_T("WL_lBarcodeCheckLength"),	&m_lBarcodeCheckLength);
		RegVariable(_T("WL_lBarcodeCheckLengthUpperBound"),	&m_lBarcodeCheckLengthUpperBound);


		RegVariable(_T("WL_bRotateBackAfterScan"),	&m_bRotateBackAfterScan);
		RegVariable(_T("WL_lWftYScanRange"),		&m_lWftYScanRange);
		RegVariable(_T("WL_dWftTScanRange"),		&m_dWftTScanRange);
		RegVariable(_T("WL_bBarcodeCheck"),			&m_bBarcodeCheck);
		RegVariable(_T("WL_ucBarcodeModel"),		&m_ucBarcodeModel);
		RegVariable(_T("WL_bCompareBarcode"),		&m_bCompareBarcode);
		RegVariable(_T("WL_bEnable2DBarcodeCheck"),	&m_bEnable2DBarcodeCheck);

		RegVariable(_T("WL_szLotInfoLotNo"),		&m_szLotInfoLotNo);
		RegVariable(_T("WL_szLotInfoFilePath"),		&m_szLotInfoFilePath);	
		RegVariable(_T("WL_szLotInfoFileName"),		&m_szLotInfoFileName);	
		RegVariable(_T("WL_szLotInfoFileExt"),		&m_szLotInfoFileExt);	

		RegVariable(_T("WL_ulSequenceMode"),		&m_ulSequenceMode);

		RegVariable(_T("WL_lWLGeneral_1"),			&m_lWLGeneral_1);
		RegVariable(_T("WL_lWLGeneral_2"),			&m_lWLGeneral_2);
		RegVariable(_T("WL_lWLGeneral_3"),			&m_lWLGeneral_3);
		RegVariable(_T("WL_lWLGeneral_4"),			&m_lWLGeneral_4);
		RegVariable(_T("WL_lWLGeneral_5"),			&m_lWLGeneral_5);
		RegVariable(_T("WL_lWLGeneral_6"),			&m_lWLGeneral_6);
		RegVariable(_T("WL_lWLGeneral_7"),			&m_lWLGeneral_7);
		RegVariable(_T("WL_lWLGeneral_8"),			&m_lWLGeneral_8);
		RegVariable(_T("WL_lWLGeneral_9"),			&m_lWLGeneral_9);
		RegVariable(_T("WL_dWLGeneral_10"),			&m_dWLGeneral_10);
		RegVariable(_T("WL_lWLGeneral_11"),			&m_lWLGeneral_11);
		RegVariable(_T("WL_lWLGeneral_12"),			&m_lWLGeneral_12);
		RegVariable(_T("WL_lWLGeneral_13"),			&m_lWLGeneral_13);
		RegVariable(_T("WL_lWLGeneral_14"),			&m_lWLGeneral_14);
		RegVariable(_T("WL_lWLGeneral_15"),			&m_lWLGeneral_15);
		RegVariable(_T("WL_lWLGeneral_TmpA"),		&m_lWLGeneral_TmpA);

		RegVariable(_T("WL_l2DBarcodePos_X"),		&m_l2DBarcodePos_X);		//v3.70T3
		RegVariable(_T("WL_bHomeGripper"),			&m_bHomeGripper);
		RegVariable(_T("WL_bHomeGripper2"),			&m_bHomeGripper2);
		
		RegVariable(_T("WL_bReadBarCodeOnTable"),		&m_bReadBarCodeOnTable);
		RegVariable(_T("WL_bOpenGripperInUnload"),		&m_bOpenGripperInUnload);
		RegVariable(_T("WL_bOpenGripper2InUnload"),		&m_bOpenGripper2InUnload);
		
		RegVariable(_T("WL_lEjrCapCleanLimit"),			&m_lEjrCapCleanLimit);		//v3.28T2
		RegVariable(_T("WL_lEjrCapCleanCount"),			&m_lEjrCapCleanCount);		//v3.28T2

		RegVariable(_T("WL_bAutoAdpatWafer"),			&m_bAutoAdpatWafer);		//v3.28T2
				
		RegVariable(_T("WL_bUseBCInCassetteFile"),		&m_bUseBCInCassetteFile);
		RegVariable(_T("WL_szBCInCassetteFilename"),	&m_szBCInCassetteFilename);

		RegVariable(_T("WL_bWaferAlignComplete"),		&m_bWaferAlignComplete);

		//RegVariable(_T("WL_bStartWaferBarcodeTest"),	&m_bStartWaferBarcodeTest);
		RegVariable(_T("WL_lWaferBarcodeReverseCount"),	&m_lWaferBarcodeReverseCount);
		RegVariable(_T("WL_lWaferBarcodeTestStep"),		&m_lBarcodeTestMoveStep);

		//Yealy MS100Plus SIngle loader option 
		RegVariable(_T("WL_bFrameInUpperSlot"),			&m_bFrameInUpperSlot);
		RegVariable(_T("WL_bFrameInLowerSlot"),			&m_bFrameInLowerSlot);
		RegVariable(_T("WL_szUpperSlotBarcode"),		&m_szUpperSlotBarcode);
		RegVariable(_T("WL_szLowerSlotBarcode"),		&m_szLowerSlotBarcode);

		RegVariable(_T("WL_ucPLLMWaferAlignOption"),	&m_ucPLLMWaferAlignOption);

		RegVariable(_T("WL_lLoopTestCounter"),			&m_lLoopTestCounter);

		DisplayMessage("WaferLoaderStn variables Registered...");


		// Register Commands
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),		&CWaferLoader::UpdateAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogItems"),			&CWaferLoader::LogItems);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateExpanderType"), &CWaferLoader::UpdateExpanderType);
		//Common
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HotAir"),			&CWaferLoader::HotAir);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GripperState"),		&CWaferLoader::GripperState);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GripperLevel"),		&CWaferLoader::GripperLevel);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HotAir2"),			&CWaferLoader::HotAir2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Gripper2State"),	&CWaferLoader::Gripper2State);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Gripper2Level"),	&CWaferLoader::Gripper2Level);

		//m_comServer.IPC_REG_SERVICE_COMMAND(_T("DCMPower"),			DCMPower);
		//m_comServer.IPC_REG_SERVICE_COMMAND(_T("DCMDirection"),		DCMDirection);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Expander2Lock"),	&CWaferLoader::Expander2Lock);
		
		//Gear
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DCMPower"),					&CWaferLoader::DCMPower);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DCMDirection"),				&CWaferLoader::DCMDirection);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExpanderLock"),				&CWaferLoader::ExpanderLock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckExpanderLock"),		&CWaferLoader::CheckExpanderLock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckExpander2Lock"),		&CWaferLoader::CheckExpander2Lock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WLCheckPrestartStatus"),	&CWaferLoader::WLCheckPrestartStatus);		
		//Vacuum
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrameVacuum"),		&CWaferLoader::FrameVacuum);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrameLevel"),		&CWaferLoader::FrameLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrameAlign"),		&CWaferLoader::FrameAlign);
		//MS60 AutoLine
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ALFrontGate"),		&CWaferLoader::ALFrontGate);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ALBackGate"),		&CWaferLoader::ALBackGate);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ALMgznClamp"),		&CWaferLoader::ALMgznClamp);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeZ"),			&CWaferLoader::HomeZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeX"),			&CWaferLoader::HomeX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerZ"),			&CWaferLoader::PowerZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerX"),			&CWaferLoader::PowerX);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerX2"),			&CWaferLoader::PowerX2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerZ2"),			&CWaferLoader::PowerZ2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeX2"),			&CWaferLoader::HomeX2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeZ2"),			&CWaferLoader::HomeZ2);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangePosition"),	&CWaferLoader::ChangePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("KeyInPosition"),	&CWaferLoader::KeyInPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovePosPosition"),	&CWaferLoader::MovePosPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveNegPosition"),	&CWaferLoader::MoveNegPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSetup"),		&CWaferLoader::ConfirmSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelSetup"),		&CWaferLoader::CancelSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMagazine"),	&CWaferLoader::SelectMagazine);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveSetupData"),	&CWaferLoader::SaveSetupData);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangePosition2"),	&CWaferLoader::ChangePosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("KeyInPosition2"),	&CWaferLoader::KeyInPosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovePosPosition2"),	&CWaferLoader::MovePosPosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveNegPosition2"),	&CWaferLoader::MoveNegPosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSetup2"),	&CWaferLoader::ConfirmSetup2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelSetup2"),		&CWaferLoader::CancelSetup2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMagazine2"),	&CWaferLoader::SelectMagazine2);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenerateConfigData"),	&CWaferLoader::GenerateConfigData);		//v3.86
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToSlot"),		&CWaferLoader::MoveToSlot);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToSlot2"),		&CWaferLoader::MoveToSlot2);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToPosition"),			&CWaferLoader::MoveToPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetUnloadPosition"),		&CWaferLoader::SetUnloadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmUnloadPosition"),	&CWaferLoader::ConfirmUnloadPosition);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToPosition2"),			&CWaferLoader::MoveToPosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetUnloadPosition2"),		&CWaferLoader::SetUnloadPosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmUnloadPosition2"),	&CWaferLoader::ConfirmUnloadPosition2);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetMS902ndHomePosn"),		&CWaferLoader::SetMS902ndHomePosn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmMS902ndHomePosn"),	&CWaferLoader::ConfirmMS902ndHomePosn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetHomeDiePosition"),		&CWaferLoader::SetHomeDiePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmHomeDiePosition"),	&CWaferLoader::ConfirmHomeDiePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetHomeDieRecordID"),		&CWaferLoader::GetHomeDieRecordID);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualChangeFilmFrame"),	&CWaferLoader::ManualChangeFilmFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualChangeFilmFrame_WT2"), &CWaferLoader::ManualChangeFilmFrame_WT2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("App_AutoChangeFilmFrame"),	&CWaferLoader::App_AutoChangeFilmFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("App_OpenCloseExpander"),	&CWaferLoader::App_OpenCloseExpander);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetSingleWaferSlotInfoCmd"),	&CWaferLoader::ResetSingleWaferSlotInfoCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetWaferSlotInfoCmd"),	&CWaferLoader::ResetWaferSlotInfoCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayWaferSlotInfo"),		&CWaferLoader::DisplayWaferSlotInfo);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoChangeFilmFrame"),		&CWaferLoader::AutoChangeFilmFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoRegionAlignWafer"),		&CWaferLoader::AutoRegionAlignWafer);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoopTestCounter"),			&CWaferLoader::LoopTestCounter);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLoadUnloadTest"),		&CWaferLoader::AutoLoadUnloadTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartContLoadUnloadTest"),	&CWaferLoader::StartContLoadUnloadTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLoadUnloadTest2"),		&CWaferLoader::AutoLoadUnloadTest2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualSearchHomeDie"),		&CWaferLoader::ManualSearchHomeDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualReadBarCodeOnTable"),	&CWaferLoader::ManualReadBarCodeOnTable);	//v4.42T6
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestBarcode"),				&CWaferLoader::TestBarcode);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestBarcode2"),				&CWaferLoader::TestBarcode2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLoadWaferFrame"),		&CWaferLoader::AutoLoadWaferFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WLBurnInSetup"),			&CWaferLoader::WLBurnInSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualOpenCloseExpander"),	&CWaferLoader::ManualOpenCloseExpander);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualOpenCloseExpander2"),	&CWaferLoader::ManualOpenCloseExpander2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoAlignFrameWafer"),		&CWaferLoader::AutoAlignFrameWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WL1InputBarcodeLoadMap"),	&CWaferLoader::WL1InputBarcodeLoadMap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WL2InputBarcodeLoadMap"),	&CWaferLoader::WL2InputBarcodeLoadMap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoReadBarcodeOnTable"),	&CWaferLoader::AutoReadBarcodeOnTable);	//v4.44A6

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowHistory"),				&CWaferLoader::ShowHistory);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReadWaferLotInfoFile"),		&CWaferLoader::ReadWaferLotInfoFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetWaferLotInfoFilePath"),	&CWaferLoader::GetWaferLotInfoFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsExpanderSafeToMove"),		&CWaferLoader::IsExpanderSafeToMove);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsExpander2SafeToMove"),	&CWaferLoader::IsExpander2SafeToMove);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckExpanderSafeToMove"),	&CWaferLoader::CheckExpanderSafeToMove);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateOutput"),				&CWaferLoader::UpdateOutput);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetEjectorCapCount"),		&CWaferLoader::ResetEjectorCapCount);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadBarcodeInCassetteFile"), &CWaferLoader::LoadBarcodeInCassetteFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBarcodeInCassette"),	&CWaferLoader::ResetBarcodeInCassette);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FindWaferAngle"),			&CWaferLoader::FindWaferAngle);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FindWaferAngleFirstButton"),&CWaferLoader::FindWaferAngleFirstButton);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WaferBarcodeTest"),			&CWaferLoader::WaferBarcodeTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetFullMagazine"),		&CWaferLoader::ResetFullMagazine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetXMagazine"),			&CWaferLoader::ResetXMagazine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetGMagazine"),			&CWaferLoader::ResetGMagazine);
		//Yealy MS100Plus with single loader & buffer Table config	//v4.31T11
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadFromMgznToBuffer"),		&CWaferLoader::ManualLoadFromMgznToBuffer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadFromBufferToMgzn"),		&CWaferLoader::ManualUnloadFromBufferToMgzn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadFromBufferToTable"),		&CWaferLoader::ManualLoadFromBufferToTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadFromTableToBuffer"),	&CWaferLoader::ManualUnloadFromTableToBuffer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetCurrentMgznSlotNo"),				&CWaferLoader::GetCurrentMgznSlotNo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetCurrentMgznSlotNo"),				&CWaferLoader::SetCurrentMgznSlotNo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoPreloadFrameFromMgznToBuffer"),	&CWaferLoader::AutoPreloadFrameFromMgznToBuffer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoUnloadFrameFromBufferToMgzn"),	&CWaferLoader::AutoUnloadFrameFromBufferToMgzn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBufferTable"),					&CWaferLoader::ResetBufferTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Test"),								&CWaferLoader::Test);
		//ANDREW_SC
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WL_MapCassette"),			&CWaferLoader::WL_MapCassette);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WL_LoadUnloadCassette"),	&CWaferLoader::WL_LoadUnloadCassette);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WL_GetCassInfo"),			&CWaferLoader::WL_GetCassInfo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateHMIData"),			&CWaferLoader::UpdateHMIData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateHMIData2"),			&CWaferLoader::UpdateHMIData2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchWTContourCmd"),		&CWaferLoader::SearchWTContourCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchWT1ContourCmd"),		&CWaferLoader::SearchWT1ContourCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchWT2ContourCmd"),		&CWaferLoader::SearchWT2ContourCmd);
		//v4.55A11	//Autoline
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SECS_LoadFrameCmd"),		&CWaferLoader::SECS_LoadFrameCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SECS_LoadFrameDoneCmd"),	&CWaferLoader::SECS_LoadFrameDoneCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SECS_UnloadFrameCmd"),		&CWaferLoader::SECS_UnloadFrameCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SECS_UnloadFrameDoneCmd"),	&CWaferLoader::SECS_UnloadFrameDoneCmd);

		DisplayMessage("WaferLoaderStn Service Registered...");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}



LONG CWaferLoader::UpdateAction(IPC_CServiceMessage &svMsg)
{
	Sleep(500);
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferLoader::LogItems(IPC_CServiceMessage &svMsg)
{
	LONG lEventNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lEventNo);

	LogItems(lEventNo);

	return 1;
}


//Common
LONG CWaferLoader::HotAir(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetHotAir(bOn);

	return TRUE;
}


LONG CWaferLoader::GripperState(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetGripperState(bOn);

	return TRUE;
}


LONG CWaferLoader::GripperLevel(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetGripperLevel(bOn);

	return TRUE;
}
//end common


//Gear version
LONG CWaferLoader::DCMPower(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if ((GetExpType() == WL_EXP_CYLINDER) || (GetExpType() == WL_EXP_NONE))	//v4.39T10	//Knowles MS109
	{
		return TRUE;
	}

	SetDCMPower(bOn);

	return TRUE;
}


LONG CWaferLoader::DCMDirection(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if ((GetExpType() == WL_EXP_CYLINDER) || (GetExpType() == WL_EXP_NONE))	//v4.39T10	//Knowles MS109
	{
		return TRUE;
	}

	SetDCMDirecction(bOn);

	return TRUE;
}


LONG CWaferLoader::ExpanderLock(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if ((GetExpType() == WL_EXP_CYLINDER) || (GetExpType() == WL_EXP_NONE))	//v4.39T10	//Knowles MS109
	{
		return TRUE;
	}

	SetExpanderLock(bOn);

	return TRUE;
}


LONG CWaferLoader::Gripper2State(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetGripper2State(bOn);

	return TRUE;
}


LONG CWaferLoader::Gripper2Level(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetGripper2Level(bOn);

	return TRUE;
}
//end common

LONG CWaferLoader::Expander2Lock(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if ((GetExpType() == WL_EXP_CYLINDER) || (GetExpType() == WL_EXP_NONE))	//v4.39T10	//Knowles MS109
	{
		return TRUE;
	}

	SetExpander2Lock(bOn);

	return TRUE;
}

LONG CWaferLoader::HotAir2(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetHotAir2(bOn);

	return TRUE;
}


LONG CWaferLoader::WLCheckPrestartStatus(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableWL)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsESMachine() && m_bUseMagazine3AsX )	// X_Magazine unload 2
	{
		if (m_lCurrentMagazineX1SlotNo > m_stWaferMagazine[m_lMagazineX_No-1].m_lNoOfSlots)
		{
			bReturn = FALSE;
			HmiMessage_Red_Back("Low yield magazine 1 full.\nPlease reset it", "AOI Scan");
		}
	}

	if( IsESMachine() && m_bUseAMagazineAsG )	// G_Magazine unload 2
	{
		if (m_lMagazineG1CurrentSlotNo > m_stWaferMagazine[m_lMagazineG_No-1].m_lNoOfSlots)
		{
			bReturn = FALSE;
			HmiMessage_Red_Back("Good yield magazine 1 full.\nPlease reset it", "AOI Scan");
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);	//v3.49T1
	return 1;
}


LONG CWaferLoader::CheckExpanderLock(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if ((GetExpType() == WL_EXP_VACUUM)		|| 
			(GetExpType() == WL_EXP_CYLINDER)	|| 
			(GetExpType() == WL_EXP_NONE))		//v4.39T10	//Knowles MS109
	{
		svMsg.GetMsg(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bDisableWL)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!IsAllMotorsEnable())		//v3.64
	{
		bReturn = FALSE;

		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
	}

	if (IsExpanderLock())
	{
		bReturn = FALSE;

		SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);	//v3.49T1
	return 1;
}

LONG CWaferLoader::CheckExpander2Lock(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if ((GetExpType() == WL_EXP_VACUUM)		|| 
			(GetExpType() == WL_EXP_CYLINDER)	||
			(GetExpType() == WL_EXP_NONE))			//v4.39T10	//Knowles MS109
	{
		svMsg.GetMsg(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bDisableWL)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!IsAllMotorsEnable2())		//v3.64
	{
		bReturn = FALSE;

		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
	}

	if (IsExpander2Lock())
	{
		bReturn = FALSE;

		SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);	//v3.49T1
	return 1;
}


//Vacuum version
LONG CWaferLoader::FrameVacuum(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetFrameVacuum(bOn);

	return TRUE;
}


LONG CWaferLoader::FrameLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	UpDownFrameLevel(bOn);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::UpDownFrameLevel(BOOL bOn)
{
	if (bOn && WPR_MoveFocusToSafe() == FALSE)
	{
		return FALSE;
	}

	if (bOn && (BondArmMoveToBlow() == FALSE))  		//v2.56
	{
		return FALSE;
	}

	if (IsMS60() && bOn)	//v4.48A11
	{
		// Check-home gripper if OPEN expander
		if ((X_Home() != gnOK))
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper HOME fails");
			SetErrorMessage("Gripper HOME fails");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
			return FALSE;
		}

		if (!HomeWaferTheta())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL: HomeWaferTheta fail in Frame Level");
			SetErrorMessage("WL: HomeWaferTheta fail in Frame Level");
			return FALSE;
		}

		SetWaferTableJoystick(FALSE);
		if (!MoveWaferTableNoCheck(m_lUnloadPhyPosX, m_lUnloadPhyPosY, TRUE, TRUE))
		{
			SetErrorMessage("Manual Open Expander: table move fail");
			SetAlert_Red_Yellow(IDS_WT_CANNOT_MOVE);

			return FALSE;
		}
	}

	SetFrameLevel(bOn);

	if (IsMS60())
	{
		Sleep(500);
		if (bOn)		//v4.48A10
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("VAC Expander is opened");
			ManualClearCurrentMap();	//Knowles MS109
		}
		else
		{
			BOOL bWaferRotateFail = FALSE;
			//if (!AutoRotateWafer(FALSE))	//PLLM MS109
			//{
			//	bWaferRotateFail = TRUE;
			//}

			//v4.40T14	//TJ Sanan
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			BOOL bReadBC = pApp->GetFeatureStatus(MS896A_FUNC_MANUAL_CLOSE_EXP_BC);
			if (bReadBC && m_bUseBarcode /*&& !bWaferRotateFail*/)
			{
				Sleep(200);
				BOOL bNeedMoveHome = FALSE;
				HmiMessage("3");
				if (!CMS896AApp::m_bMS100Plus9InchOption)			//v4.47T17		//Testar
				{
					MoveWaferTable(m_lUnloadPhyPosX, m_lUnloadPhyPosY);
					bNeedMoveHome = TRUE;
				}
				else
				{
					MoveWaferTable(0, 0);
				}
				INT nBarcodeStatus = ScanningBarcodeOnTable(TRUE, FALSE);
				if ( (nBarcodeStatus == TRUE) && (m_szBarcodeName.GetLength() > 0) )
				{
					INT nID = LoadMapFileWithoutSyn(m_szBarcodeName);
					CMSLogFileUtility::Instance()->WL_LogStatus("Load map, to syn file");
					SyncLoadMapFile(nID);
					CMSLogFileUtility::Instance()->WL_LogStatus("Load map, syn file done");
				}
				else
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("scan barcode on table fail, alarm");
					SetAlert_Red_Yellow(IDS_WL_NO_BARCODE);
				}

				if (bNeedMoveHome)		//v4.47T17	//Testar
				{
					MoveWaferTable(0, 0);
				}
			}
			else if (1/*!bWaferRotateFail*/)
			{
				if (HmiMessage("Keep Table at UNLOAD? (NO = HOME)", "Setup", glHMI_MBX_YESNO) == glHMI_NO)
				{
					MoveWaferTable(0, 0);
					Sleep(500);
					SetWaferTableJoystick(TRUE);
				}
			}
		}	//ELSE
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("Frame Level IPC command done");

	return TRUE;
}


LONG CWaferLoader::FrameAlign(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetFrameAlign(bOn);

	return TRUE;
}
//end vacuum

LONG CWaferLoader::ALFrontGate(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetALFrontGate(bOn);

	if (bOn)
	{
		HmiMessage("WL FRONT gate is CLOSE");
	}
	else
	{
		HmiMessage("WL FRONT gate is OPEN");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::ALBackGate(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetALBackGate(bOn);

	if (bOn)
	{
		HmiMessage("WL BACK gate is CLOSE");
	}
	else
	{
		HmiMessage("WL BACK gate is OPEN");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::ALMgznClamp(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetALMgznClamp(bOn);

	if (bOn)
	{
		HmiMessage("WL MGZN holder is LOCK");
	}
	else
	{
		HmiMessage("WL MGZN holder is UNLOCK");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::PowerX(IPC_CServiceMessage &svMsg)
{
	BOOL	bReturn = TRUE;
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (WPR_MoveFocusToSafe() == FALSE)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetGripperPower(bOn);
	if ((bOn == TRUE) && (m_bHomeGripper == TRUE))
	{
		//Check expander before homing gripper
		if (GetExpType() == WL_EXP_VACUUM)
		{
			CheckVacuumExpander();
		}
		else if (GetExpType() == WL_EXP_NONE)		//v4.39T10	//Knowles MS109
		{
		}
		else
		{
			if (IsFrameDetect() == TRUE && m_bNoSensorCheck == FALSE)
			{
				SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
				SetErrorMessage("Frame detected on gripper");
				bReturn = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			INT nStatus = CheckExpander();
			if (nStatus == Err_ExpanderLockFailed)				// if cannot engage
			{
				nStatus = ManualCheckExpander();				// try manual-engage with T motor off
				if (nStatus == Err_ExpanderLockFailed)  		// if still cannot engage
				{
					SetErrorMessage("Gripper HOME check expander fails!");
					bReturn = TRUE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return TRUE;
				}
			}
		}

		X_Home();
	}
	else
	{
		SetGripperPower(bOn);	//v4.02T6
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CWaferLoader::PowerZ(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetLoaderZPower(bOn);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CWaferLoader::HomeX(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (WPR_MoveFocusToSafe() == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//Check expander before homing gripper
	if (GetExpType() == WL_EXP_VACUUM)
	{
		CheckVacuumExpander();
	}
	else if (GetExpType() == WL_EXP_NONE)	//v4.39T10	//Knowles MS109
	{
	}
	else
	{
		if (IsFrameDetect() == TRUE && m_bNoSensorCheck == FALSE)
		{
			SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
			SetErrorMessage("Frame detected on gripper");
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		INT nStatus = CheckExpander();
		if (nStatus == Err_ExpanderLockFailed)				// if cannot engage
		{
			nStatus = ManualCheckExpander();				// try manual-engage with T motor off
			if (nStatus == Err_ExpanderLockFailed)  		// if still cannot engage
			{
				SetErrorMessage("Gripper HOME check expander fails!");
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}
	}

	X_Home();
	if (m_bHome_X == FALSE)
	{
		HmiMessage("Wafer loader Home motor X error!", "Wafer Loader");
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::HomeZ(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsWaferFrameDetect() == TRUE)
	{
		SetErrorMessage("Frame is detected on track");
		SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONTRACK);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsMagazineSafeToMove() == FALSE)
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	Z_Home();
	if (m_bHome_Z == FALSE)
	{
		HmiMessage("Wafer loader Home motor Z error!", "Wafer Loader");
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::PowerX2(IPC_CServiceMessage &svMsg)
{
	BOOL	bReturn = TRUE;
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (WPR_MoveFocusToSafe() == FALSE)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetGripper2Power(bOn);

	if ((bOn == TRUE) && (m_bHomeGripper == TRUE))
	{
		
		if (IsFrameDetect2() == TRUE && m_bNoSensorCheck == FALSE)
		{
			SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
			SetErrorMessage("Frame detected on gripper");
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		INT nStatus = CheckExpander2();
		if (nStatus == Err_ExpanderLockFailed)				// if cannot engage
		{
			nStatus = ManualCheckExpander2();				// try manual-engage with T motor off
			if (nStatus == Err_ExpanderLockFailed)  		// if still cannot engage
			{
				SetErrorMessage("Gripper HOME check expander fails!");
				bReturn = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		X2_Home();
	}
	else
	{
		SetGripper2Power(bOn);	//v4.02T6
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CWaferLoader::PowerZ2(IPC_CServiceMessage &svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetLoaderZ2Power(bOn);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferLoader::HomeX2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (WPR_MoveFocusToSafe() == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsFrameDetect2() == TRUE && m_bNoSensorCheck == FALSE)
	{
		SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
		SetErrorMessage("Frame detected on gripper");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	INT nStatus = CheckExpander2();
	if (nStatus == Err_ExpanderLockFailed)				// if cannot engage
	{
		nStatus = ManualCheckExpander2();				// try manual-engage with T motor off
		if (nStatus == Err_ExpanderLockFailed)  		// if still cannot engage
		{
			SetErrorMessage("Gripper HOME check expander fails!");
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	X2_Home();
	if (m_bHome_X2 == FALSE)
	{
		HmiMessage("Wafer loader Home motor X2 error!", "Wafer Loader");
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::HomeZ2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsWaferFrameDetect2() == TRUE)
	{
		SetErrorMessage("Frame is detected on track");
		SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONTRACK);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//Klocwork	//v4.24T11
	//if ( IsMagazineSafeToMove2() == FALSE )
	//{
	//	SetErrorMessage("Gripper is not in safe position");
	//	SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
	//	bReturn = FALSE;
	//	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	//	return TRUE;
	//}

	Z2_Home();
	if (m_bHome_Z2 == FALSE)
	{
		HmiMessage("Wafer loader Home motor Z2 error!", "Wafer Loader");
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferLoader::ChangePosition(IPC_CServiceMessage &svMsg)
{
	LONG lPosition = 0;
	LONG lMagazineNo = 0;

	typedef struct 
	{
		LONG 	lPosition;
		LONG	lMagazineNo;
	} WL_SETUP;

	WL_SETUP stData;

	svMsg.GetMsg(sizeof(WL_SETUP), &stData);

	lPosition = stData.lPosition;
	lMagazineNo = stData.lMagazineNo;

	if (m_bDisableWLWithExp)
	{
		return TRUE;
	}

	if (lPosition == 4)	// If Loader Z Top slot level
	{
		if (IsWaferFrameDetect() == TRUE)		//v4.02T6
		{
			SetErrorMessage("Frame is detected on track");
			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONTRACK);
			return TRUE;
		}

		if (IsMagazineSafeToMove() == FALSE)
		{
			SetErrorMessage("Gripper is not in safe position");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
			return TRUE;
		}
	}

	if (WPR_MoveFocusToSafe() == FALSE)
	{
		return TRUE;
	}

	switch (lPosition)
	{
		case 1:		//Gripper load
			X_MoveTo(m_lLoadPos_X);
			m_lWLGeneral_TmpA = m_lLoadPos_X;
			break;

		case 2:		//Gripper ready
			X_MoveTo(m_lReadyPos_X);
			m_lWLGeneral_TmpA = m_lReadyPos_X;
			break;

		case 3:		//Gripper Barcode
			X_MoveTo(m_lBarcodePos_X);
			m_lWLGeneral_TmpA = m_lBarcodePos_X;
			break;

		case 4:		//Loader Z Top slot level
			if (IsESMachine())
			{
				if (lMagazineNo >= 0 && lMagazineNo < GetWL1MaxMgznNo())
				{
					Z_MoveTo(m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z);	
					m_lWLGeneral_TmpA = m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z;
				}
			}
			else if (m_bUseBLAsLoaderZ)		//v4.31T11
			{
				if (lMagazineNo >= 0 && lMagazineNo < GetWL1MaxMgznNo())
				{
					MoveToBLMagazineSlot(lMagazineNo + 1, 1);
					MoveToBLMagazineSlot_Sync();
					m_lWLGeneral_TmpA = m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z;
				}
			}
			else
			{
				Z_MoveTo(m_lTopSlotLevel_Z);
				m_lWLGeneral_TmpA = m_lTopSlotLevel_Z;
			}

			break;

		case 6:		//2D Barcode position	//v3.70T3
			X_MoveTo(m_l2DBarcodePos_X);
			m_lWLGeneral_TmpA = m_l2DBarcodePos_X;
			break;
		
		case 7:		// check barcode position
			X_MoveTo(m_lBarcodeCheckPos_X);
			m_lWLGeneral_TmpA = m_lBarcodeCheckPos_X;
			break;
		
		case 8:
			if (m_bUseBLAsLoaderZ)			//v4.31T11
			{
				MoveBinLoaderY(m_lBinLoader_Y);
				m_lWLGeneral_TmpA = m_lBinLoader_Y;
			}
			break;
		
		case 9:
			break;

		case 10:	//Buffer Table UP Slot Z
			if (m_bUseBLAsLoaderZ)			//v4.31T11
			{
				Z_MoveTo(m_lBufTableUSlot_Z);
				m_lWLGeneral_TmpA = m_lBufTableUSlot_Z;
			}
			break;
		
		case 11:	//Buffer Table DN Slot Z
			if (m_bUseBLAsLoaderZ)			//v4.31T11
			{
				Z_MoveTo(m_lBufTableLSlot_Z);
				m_lWLGeneral_TmpA = m_lBufTableLSlot_Z;
			}
			break;

		case 12:	//Gripper Buffer LOAD
			X_MoveTo(m_lBufferLoadPos_X);
			m_lWLGeneral_TmpA = m_lBufferLoadPos_X;
			break;

		case 15:	//AL Unload Z	//autoline1
			Z_MoveTo(m_lALUnload_Z);
			m_lWLGeneral_TmpA = m_lALUnload_Z;
			break;

		case 0:		//Gripper Unload
		default:	//Gripper Unload
			X_MoveTo(m_lUnloadPos_X);
			m_lWLGeneral_TmpA = m_lUnloadPos_X;
			break;
	}

	return TRUE;
}


LONG CWaferLoader::KeyInPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} WLSETUP;
	WLSETUP stInfo;


	svMsg.GetMsg(sizeof(WLSETUP), &stInfo);


	//Check KeyIn Positon Limit
	if ((stInfo.lPosition < 4) || (stInfo.lPosition == 6) || (stInfo.lPosition == 7) || (stInfo.lPosition == 12))
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER, MS896A_CFG_CH_MAX_DISTANCE);								
	}
	else
	{
		//v4.02T6
		if (((stInfo.lPosition == 4) || (stInfo.lPosition == 5)) && 
				(IsWaferFrameDetect() == TRUE))		
		{
			SetErrorMessage("Frame is detected on track");
			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONTRACK);
			return TRUE;
		}

		if (IsMagazineSafeToMove() == FALSE)
		{
			SetErrorMessage("Gripper is not in safe position");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
			return TRUE;
		}

		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);								
	}

	if ((stInfo.lPosition < 4) || (stInfo.lPosition == 6) || (stInfo.lPosition == 7) || (stInfo.lPosition == 12))
	{
		//All gripper pos must check limit !!
		if (stInfo.lStep < lMinPos) 
		{
			stInfo.lStep = lMinPos;
		}

		if (stInfo.lStep > lMaxPos)
		{
			stInfo.lStep = lMaxPos;
		}
	}
	else
	{
		if (m_bUseBLAsLoaderZ)		//v4.31T11
		{
			//No need to check motor limits here for BL YZ
		}
		else
		{
			if (stInfo.lStep < lMinPos) 
			{
				stInfo.lStep = lMinPos;
			}

			if (stInfo.lStep > lMaxPos)
			{
				stInfo.lStep = lMaxPos;
			}
		}
	}


	if (WPR_MoveFocusToSafe() == FALSE)
	{
		return TRUE;
	}

	switch (stInfo.lPosition)
	{
		case 1:		//Gripper load
			m_lWLGeneral_2 = stInfo.lStep;
			X_MoveTo(m_lWLGeneral_2);
			break;

		case 2:		//Gripper Ready
			m_lWLGeneral_3 = stInfo.lStep;
			X_MoveTo(m_lWLGeneral_3);
			break;

		case 3:		//Gripper barcode
			m_lWLGeneral_4 = stInfo.lStep;
			X_MoveTo(m_lWLGeneral_4);
			break;

		case 4:		//Loader Z Top slot level
			m_lWLGeneral_5 = stInfo.lStep;
			if (m_bUseBLAsLoaderZ)			//v4.31T11
			{
				MoveBinLoaderZ(m_lWLGeneral_5);
			}
			else
			{
				Z_MoveTo(m_lWLGeneral_5);
			}
			break;

		case 5:		//Loader Z Btn slot level
			m_lWLGeneral_6 = stInfo.lStep;
			Z_MoveTo(m_lWLGeneral_6);
			break;

		case 6:		//2D Barcode posn X
			m_lWLGeneral_7 = stInfo.lStep;
			X_MoveTo(m_lWLGeneral_7);
			break;
		
		case 7:		// Gripper check barcode
			m_lWLGeneral_8	= stInfo.lStep;
			X_MoveTo(m_lWLGeneral_8);
			break;

		case 8:		//Bin-Loader Y		//v4.31T11
			if (m_bUseBLAsLoaderZ)
			{
				m_lWLGeneral_11 = stInfo.lStep;
				MoveBinLoaderY(m_lWLGeneral_11);
			}
			break;

		case 9:
			break;

		case 10:	//Buffer Table UP Slot Z
			if (m_bUseBLAsLoaderZ)			//v4.31T11
			{
				m_lWLGeneral_12 = stInfo.lStep;
				Z_MoveTo(m_lWLGeneral_12);
			}
			break;
		
		case 11:	//Buffer Table DN Slot Z
			if (m_bUseBLAsLoaderZ)			//v4.31T11
			{
				m_lWLGeneral_13 = stInfo.lStep;
				Z_MoveTo(m_lWLGeneral_13);
			}
			break;

		case 12:	//Gripper Buffer LOAD X
			if (m_bUseBLAsLoaderZ)			//v4.31T11
			{
				m_lWLGeneral_14 = stInfo.lStep;
				X_MoveTo(m_lWLGeneral_14);
			}
			break;

		case 15:	//AL Unload Z
			if (IsMSAutoLineMode())	
			{
				m_lWLGeneral_15 = stInfo.lStep;
				Z_MoveTo(m_lWLGeneral_15);
			}
			break;

		default:	//Gripper Unload
			m_lWLGeneral_1 = stInfo.lStep;
			X_MoveTo(m_lWLGeneral_1);
			break;
	}


	return TRUE;
}


LONG CWaferLoader::MovePosPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} WLSETUP;

	WLSETUP stInfo;

	svMsg.GetMsg(sizeof(WLSETUP), &stInfo);


	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lWLGeneral_2 + stInfo.lStep;	
			break;	//Gripper load
		case 2:		
			lCurrentPos = m_lWLGeneral_3 + stInfo.lStep;	
			break;	//Gripper Ready
		case 3:		
			lCurrentPos = m_lWLGeneral_4 + stInfo.lStep;	
			break;	//Gripper barcode
		case 4:		
			lCurrentPos = m_lWLGeneral_5 + stInfo.lStep;	
			break;	//Loader Z Top slot
		case 5:		
			lCurrentPos = m_lWLGeneral_6 + stInfo.lStep;	
			break;	//Loader Z Btn slot
		case 6:		
			lCurrentPos = m_lWLGeneral_7 + stInfo.lStep;	
			break;	//2D Barcode	//v3.70T3
		case 7:		
			lCurrentPos = m_lWLGeneral_8 + stInfo.lStep;	
			break;	//Gripper check barcode
		case 8:		
			lCurrentPos = m_lWLGeneral_11 + stInfo.lStep;	
			break;	//Bin Loader Y	//** Single loader option **//
		case 9:		
			break;
		case 10:	
			lCurrentPos = m_lWLGeneral_12 + stInfo.lStep;	
			break;	//Buf Table USlot Z		//** Single loader option **//
		case 11:	
			lCurrentPos = m_lWLGeneral_13 + stInfo.lStep;	
			break;	//Buf Table LSlot Z		//** Single loader option **//
		case 12:	
			lCurrentPos = m_lWLGeneral_14 + stInfo.lStep;	
			break;	//Gripper Buffer Load X
		case 15:	
			lCurrentPos = m_lWLGeneral_15 + stInfo.lStep;	
			break;	//AL Unload Z	//autoline1
		default:	
			lCurrentPos = m_lWLGeneral_1 + stInfo.lStep;	
			break;	//Gripper Unload
	}


	//Check limit range
	if ((stInfo.lPosition < 4) || (stInfo.lPosition == 6) || (stInfo.lPosition == 7) || (stInfo.lPosition == 12))		//Gripper posns
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER, MS896A_CFG_CH_MAX_DISTANCE);								

		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			X_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 1:		
					m_lWLGeneral_2 = lCurrentPos;	
					break;
				case 2:		
					m_lWLGeneral_3 = lCurrentPos;	
					break;
				case 3:		
					m_lWLGeneral_4 = lCurrentPos;	
					break;
				case 6:		
					m_lWLGeneral_7 = lCurrentPos;	
					break;		//v3.70T3
				case 7:		
					m_lWLGeneral_8 = lCurrentPos;	
					break;
				case 12:	
					m_lWLGeneral_14 = lCurrentPos;	
					break;
				default:	
					m_lWLGeneral_1 = lCurrentPos;	
					break;
			}
		}
	}
	else
	{
		if (IsMagazineSafeToMove() == FALSE)
		{
			SetErrorMessage("Gripper is not in safe position");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
			return TRUE;
		}

		if (m_bUseBLAsLoaderZ)		//v4.31T11
		{
			if (stInfo.lPosition == 4)						//Bin Loader Top Slot Z
			{
				if (MoveBinLoaderZ(lCurrentPos) == TRUE)
				{
					m_lWLGeneral_5 = lCurrentPos;
				}
			}
			else if (stInfo.lPosition == 8)					//Bin Loader Y
			{
				if (MoveBinLoaderY(lCurrentPos) == TRUE)
				{
					m_lWLGeneral_11 = lCurrentPos;
				}
			}
			else
			{
				lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);								
				lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);								

				if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
				{
					Z_MoveTo(lCurrentPos);
					switch (stInfo.lPosition)
					{
						case 5:		
							m_lWLGeneral_6	= lCurrentPos;	
							break;
						case 8:		
							break;
						case 9:		
							break;
						case 10:	
							m_lWLGeneral_12 = lCurrentPos;	
							break;		//Buf Table USlot Z
						case 11:	
							m_lWLGeneral_13 = lCurrentPos;	
							break;		//Buf Table LSlot Z
					}
				}
			}
		}
		else
		{
			lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);								

			if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
			{
				Z_MoveTo(lCurrentPos);
				switch (stInfo.lPosition)
				{
					case 4:		
						m_lWLGeneral_5 = lCurrentPos;	
						break;
					case 5:		
						m_lWLGeneral_6 = lCurrentPos;	
						break;
					case 8:		
						break;
					case 9:		
						break;
					case 15:
						m_lWLGeneral_15 = lCurrentPos;	
						break;		//AL Unload Z	//autoline1
					default:	
						m_lWLGeneral_5 = lCurrentPos;	
						break;
				}
			}
		}
	}

	return TRUE;
}


LONG CWaferLoader::MoveNegPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} WLSETUP;

	WLSETUP stInfo;

	svMsg.GetMsg(sizeof(WLSETUP), &stInfo);


	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lWLGeneral_2 - stInfo.lStep;	
			break;	//Gripper load
		case 2:		
			lCurrentPos = m_lWLGeneral_3 - stInfo.lStep;	
			break;	//Gripper Ready
		case 3:		
			lCurrentPos = m_lWLGeneral_4 - stInfo.lStep;	
			break;	//Gripper barcode
		case 4:		
			lCurrentPos = m_lWLGeneral_5 - stInfo.lStep;	
			break;	//Loader Z Top slot
		case 5:		
			lCurrentPos = m_lWLGeneral_6 - stInfo.lStep;	
			break;	//Loader Z Btn slot
		case 6:		
			lCurrentPos = m_lWLGeneral_7 - stInfo.lStep;	
			break;	//2D barcode
		case 7:		
			lCurrentPos = m_lWLGeneral_8 - stInfo.lStep;	
			break;	//Gripper check barcode
		case 8:		
			lCurrentPos = m_lWLGeneral_11 - stInfo.lStep;	
			break;	//Bin Loader Y	//** Single loader option **//
		case 9:		
			break;
		case 10:	
			lCurrentPos = m_lWLGeneral_12 - stInfo.lStep;	
			break;	//Buf Table USlot Z		//** Single loader option **//
		case 11:	
			lCurrentPos = m_lWLGeneral_13 - stInfo.lStep;	
			break;	//Buf Table LSlot Z		//** Single loader option **//
		case 12:	
			lCurrentPos = m_lWLGeneral_14 - stInfo.lStep;	
			break;	//Gripper Buffer Load X
		case 15:	
			lCurrentPos = m_lWLGeneral_15 - stInfo.lStep;	
			break;	//AL Unload Z	//autoline1	
		default:	
			lCurrentPos = m_lWLGeneral_1 - stInfo.lStep;	
			break;	//Gripper Unload
	}


	//Check limit range
	
	if ((stInfo.lPosition < 4) || (stInfo.lPosition == 6) || (stInfo.lPosition == 7) || (stInfo.lPosition == 12))
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER, MS896A_CFG_CH_MAX_DISTANCE);								

		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			X_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 1:		
					m_lWLGeneral_2 = lCurrentPos;	
					break;
				case 2:		
					m_lWLGeneral_3 = lCurrentPos;	
					break;
				case 3:		
					m_lWLGeneral_4 = lCurrentPos;	
					break;
				case 6:		
					m_lWLGeneral_7 = lCurrentPos;	
					break;		//v3.70T3
				case 7:		
					m_lWLGeneral_8 = lCurrentPos;	
					break;
				case 12:	
					m_lWLGeneral_14 = lCurrentPos;	
					break;
				default:	
					m_lWLGeneral_1 = lCurrentPos;	
					break;
			}
		}
	}
	else
	{
		if (IsMagazineSafeToMove() == FALSE)
		{
			SetErrorMessage("Gripper is not in safe position");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
			return TRUE;
		}

		if (m_bUseBLAsLoaderZ)					//v4.31T11
		{
			if (stInfo.lPosition == 8)			//Bin Loader Y		//v4.31T11
			{
				if (MoveBinLoaderY(lCurrentPos) == TRUE)
				{
					m_lWLGeneral_11 = lCurrentPos;
				}
			}
			else if (stInfo.lPosition == 4)		//Bin Loader Top Slot Z		//v4.31T11
			{
				if (MoveBinLoaderZ(lCurrentPos) == TRUE)
				{
					m_lWLGeneral_5 = lCurrentPos;
				}
			}
			else
			{
				lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);								
				lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);								
				if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
				{
					Z_MoveTo(lCurrentPos);

					switch (stInfo.lPosition)
					{
						case 5:		
							m_lWLGeneral_6 = lCurrentPos;	
							break;
						case 8:		
							break;
						case 9:		
							break;
						case 10:	
							m_lWLGeneral_12 = lCurrentPos;	
							break;		//Buf Table USlot Z
						case 11:	
							m_lWLGeneral_13 = lCurrentPos;	
							break;		//Buf Table LSlot Z
					}
				}
			}
		}
		else
		{
			lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);								

			if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
			{
				Z_MoveTo(lCurrentPos);

				switch (stInfo.lPosition)
				{
					case 4:		
						m_lWLGeneral_5 = lCurrentPos;	
						break;
					case 5:		
						m_lWLGeneral_6 = lCurrentPos;	
						break;
					case 8:		
						break;
					case 9:		
						break;
					case 15:		
						m_lWLGeneral_15 = lCurrentPos;	//autoline1
						break;
					default:	
						m_lWLGeneral_5 = lCurrentPos;	
						break;
				}
			}
		}
	}

	return TRUE;
}


LONG CWaferLoader::ConfirmSetup(IPC_CServiceMessage &svMsg)
{
	LONG lMagazineNo;
	svMsg.GetMsg(sizeof(LONG), &lMagazineNo);

	if (m_bDisableWLWithExp)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_lLoadPos_X != m_lWLGeneral_2)
	{
		m_lLoadPos_X = m_lWLGeneral_2;
		LogItems(GRIPPER_LOAD_POS);
	}
	else if (m_lUnloadPos_X != m_lWLGeneral_1)
	{
		m_lUnloadPos_X = m_lWLGeneral_1;
		LogItems(GRIPPER_UNLOAD_POS);
	}
	else if (m_lReadyPos_X != m_lWLGeneral_3)
	{
		m_lReadyPos_X = m_lWLGeneral_3;
		LogItems(GRIPPER_READY_POS);
	}
	else if (m_lBarcodePos_X != m_lWLGeneral_4)
	{
		m_lBarcodePos_X = m_lWLGeneral_4;
		LogItems(GRIPPER_BC_POS);
	}
	
	//Update variable
	m_lUnloadPos_X		= m_lWLGeneral_1;		
	m_lLoadPos_X		= m_lWLGeneral_2;		
	m_lReadyPos_X		= m_lWLGeneral_3;		
	m_lBarcodePos_X		= m_lWLGeneral_4;

	if (IsESMachine() || m_bUseBLAsLoaderZ)		//v4.31T11
	{
		if (lMagazineNo >= 0 && lMagazineNo < GetWL1MaxMgznNo())
		{
			if (m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z != m_lWLGeneral_5)
			{
				m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z = m_lWLGeneral_5;
				LogItems(MAGAZINE_TOP_SLOT_POS);
			}

			if( m_lWLGeneral_9>WL_MAX_MAG_SLOT || m_lWLGeneral_9<1 )
				m_lWLGeneral_9 = WL_MAX_MAG_SLOT;
			m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z	= m_lWLGeneral_5;
			m_stWaferMagazine[lMagazineNo].m_lNoOfSlots		= m_lWLGeneral_9;
			m_stWaferMagazine[lMagazineNo].m_dSlotPitch		= m_dWLGeneral_10;

			//v4.31T11
			m_lBinLoader_Y		= m_lWLGeneral_11;
			m_lBufTableUSlot_Z	= m_lWLGeneral_12;	
			m_lBufTableLSlot_Z	= m_lWLGeneral_13;	
			SetWLMagazineToBL(lMagazineNo + 1);	
		}
	}
	else
	{
		if (m_lTopSlotLevel_Z != m_lWLGeneral_5)
		{
			m_lTopSlotLevel_Z = m_lWLGeneral_5;
			LogItems(MAGAZINE_TOP_SLOT_POS);
		}

		m_lTopSlotLevel_Z	= m_lWLGeneral_5;
		m_dSlotPitch		= m_dWLGeneral_10;
		m_lTotalSlotNo		= m_lWLGeneral_9;
	}

	m_l2DBarcodePos_X		= m_lWLGeneral_7;
	m_lBarcodeCheckPos_X	= m_lWLGeneral_8;
	m_lBufferLoadPos_X		= m_lWLGeneral_14;

	m_lALUnload_Z	= m_lWLGeneral_15;		//autoline1

	SaveData();

	//Check expander before homing gripper
	if (GetExpType() == WL_EXP_VACUUM)
	{
		if (X_IsPowerOn())
		{
			X_MoveTo(0);
		}
		else
		{
			X_Home();
		}
		SetGripperLevel(WL_OFF);

		CheckVacuumExpander();
		
		//v4.48A1	//MS100P3
		Sleep(500);
	
	}
	else if (GetExpType() == WL_EXP_NONE)	//v4.39T10	//Knowles MS109
	{
	}
	else
	{
		INT nStatus = CheckExpander();
		if (nStatus == Err_ExpanderLockFailed)				// if cannot engage
		{
			nStatus = ManualCheckExpander();				// try manual-engage with T motor off
			if (nStatus == Err_ExpanderLockFailed)  		// if still cannot engage
			{
				SetErrorMessage("Gripper HOME check expander fails!");
			}
			else
			{
				Sleep(500);
				if (X_IsPowerOn())
				{
					X_MoveTo(0);
				}
				else
				{
					X_Home();
				}
				SetGripperLevel(WL_OFF);
			}
		}
		else
		{
			Sleep(500);
			if (X_IsPowerOn())
			{
				X_MoveTo(0);
			}
			else
			{
				X_Home();
			}
			SetGripperLevel(WL_OFF);
		}
	}
	
	SetStatusMessage("WL Gripper / Elevator position is updated");

	//CEID#7100
	SendGripperEvent_7100(m_lUnloadPos_X, m_lStepOnUnloadPos, m_lLoadPos_X, m_lStepOnLoadPos,
						  m_lReadyPos_X, m_lBarcodePos_X, m_lTopSlotLevel_Z, m_dSlotPitch,
						  m_lTotalSlotNo, m_lCurrentSlotNo, m_lSkipSlotNo);

	//v3.27T2
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferLoader::CancelSetup(IPC_CServiceMessage &svMsg)
{
	LONG lMagazineNo;
	svMsg.GetMsg(sizeof(LONG), &lMagazineNo);

	if (m_bDisableWLWithExp)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//Restore HMI variable
	m_lWLGeneral_1		= m_lUnloadPos_X;		
	m_lWLGeneral_2		= m_lLoadPos_X;		
	m_lWLGeneral_3		= m_lReadyPos_X;		
	m_lWLGeneral_4		= m_lBarcodePos_X;
	
	if (m_bUseDualTablesOption || m_bUseBLAsLoaderZ)		//v4.31T11
	{
		if (lMagazineNo >= 0 && lMagazineNo < GetWL1MaxMgznNo())
		{
			m_lWLGeneral_5 = m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z;
			m_lWLGeneral_9 = m_stWaferMagazine[lMagazineNo].m_lNoOfSlots;
			m_dWLGeneral_10 = m_stWaferMagazine[lMagazineNo].m_dSlotPitch;
		}

		//v4.31T11
		m_lWLGeneral_11		= m_lBinLoader_Y;	
		m_lWLGeneral_12		= m_lBufTableUSlot_Z;	
		m_lWLGeneral_13		= m_lBufTableLSlot_Z;	
	}
	else
	{
		m_lWLGeneral_5		= m_lTopSlotLevel_Z;
		m_lWLGeneral_9		= m_lTotalSlotNo;
		m_dWLGeneral_10		= m_dSlotPitch;
	}

	m_lWLGeneral_7		= m_l2DBarcodePos_X;
	m_lWLGeneral_8		= m_lBarcodeCheckPos_X;
	m_lWLGeneral_14		= m_lBufferLoadPos_X;

	m_lWLGeneral_15		= m_lALUnload_Z;

	//Check expander before homing gripper
	if (GetExpType() == WL_EXP_VACUUM)
	{
		CheckVacuumExpander();
		
		//v4.48A1	//MS100P3
		Sleep(500);
		if (X_IsPowerOn())
		{
			X_MoveTo(0);
		}
		else
		{
			X_Home();
		}
		SetGripperLevel(WL_OFF);
	}
	else if (GetExpType() == WL_EXP_NONE)	//v4.39T10	//Knowles MS109
	{
	}
	else
	{
		INT nStatus = CheckExpander();
		if (nStatus == Err_ExpanderLockFailed)				// if cannot engage
		{
			nStatus = ManualCheckExpander();				// try manual-engage with T motor off
			if (nStatus == Err_ExpanderLockFailed)  		// if still cannot engage
			{
				SetErrorMessage("Gripper HOME check expander fails!");
			}
			else
			{
				Sleep(500);
				if (X_IsPowerOn())
				{
					X_MoveTo(0);
				}
				else
				{
					X_Home();
				}
				SetGripperLevel(WL_OFF);
			}
		}
		else
		{
			Sleep(500);
			if (X_IsPowerOn())
			{
				X_MoveTo(0);
			}
			else
			{
				X_Home();
			}
			SetGripperLevel(WL_OFF);
		}
	}

	//if (!IsWaferFrameDetect() && IsMagazineSafeToMove())	//v4.02T6	//Mag & gripper protection //SanAn
	//{
	//	Sleep(500);
	//	Z_MoveTo(m_lTopSlotLevel_Z);
	//}

	//v3.27T2
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::SelectMagazine(IPC_CServiceMessage &svMsg)
{
	LONG lMagazineNo = 0;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lMagazineNo);

	if (lMagazineNo < 0 || lMagazineNo >= GetWL1MaxMgznNo())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bUseDualTablesOption || m_bUseBLAsLoaderZ)	//v4.31T11
	{
		m_lWLGeneral_5 = m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z;
		m_lWLGeneral_9 = m_stWaferMagazine[lMagazineNo].m_lNoOfSlots;
		m_dWLGeneral_10 = m_stWaferMagazine[lMagazineNo].m_dSlotPitch;
	}
	else
	{
		m_lWLGeneral_5		= m_lTopSlotLevel_Z;
		m_lWLGeneral_9		= m_lTotalSlotNo;
		m_dWLGeneral_10		= m_dSlotPitch;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferLoader::UpdateHMIData(IPC_CServiceMessage &svMsg)
{
	LONG lMagazineNo = 0;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lMagazineNo);

	m_lWLGeneral_1		= m_lUnloadPos_X;		
	m_lWLGeneral_2		= m_lLoadPos_X;		
	m_lWLGeneral_3		= m_lReadyPos_X;		
	m_lWLGeneral_4		= m_lBarcodePos_X;	
	
	//m_lWLGeneral_5		= m_lTopSlotLevel_Z;

	m_lWLGeneral_7		= m_l2DBarcodePos_X;
	m_lWLGeneral_8		= m_lBarcodeCheckPos_X;
	m_lWLGeneral_14		= m_lBufferLoadPos_X;

	if (IsESMachine() || m_bUseBLAsLoaderZ)		//v4.31T11
	{
		if (lMagazineNo >= 0 && lMagazineNo < GetWL1MaxMgznNo())
		{
			m_lWLGeneral_5 = m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z;
			m_lWLGeneral_9 = m_stWaferMagazine[lMagazineNo].m_lNoOfSlots;
			m_dWLGeneral_10 = m_stWaferMagazine[lMagazineNo].m_dSlotPitch;
		}

		//v4.31T11
		m_lWLGeneral_11		= m_lBinLoader_Y;	
		m_lWLGeneral_12		= m_lBufTableUSlot_Z;	
		m_lWLGeneral_13		= m_lBufTableLSlot_Z;	
	}
	else
	{
		m_lWLGeneral_5		= m_lTopSlotLevel_Z;
		m_lWLGeneral_9		= m_lTotalSlotNo;
		m_dWLGeneral_10		= m_dSlotPitch;
		m_lWLGeneral_15		= m_lALUnload_Z;	//autoline1
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferLoader::ChangePosition2(IPC_CServiceMessage &svMsg)
{
	LONG lPosition = 0;
	LONG lMagazineNo = 0;

	typedef struct 
	{
		LONG 	lPosition;
		LONG	lMagazineNo;
	} WL_SETUP;

	WL_SETUP stData;

	svMsg.GetMsg(sizeof(WL_SETUP), &stData);

	lPosition = stData.lPosition;
	lMagazineNo = stData.lMagazineNo;
	
	if (m_bDisableWLWithExp)
	{
		return TRUE;
	}

	if (lPosition == 4)	// If Loader Z Top slot level
	{
		if (IsWaferFrameDetect2() == TRUE)		//v4.02T6
		{
			SetErrorMessage("Frame is detected on track");
			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONTRACK);
			return TRUE;
		}

		//Klocwork	//v4.24T11
		//if ( IsMagazineSafeToMove2() == FALSE )
		//{
		//	SetErrorMessage("Gripper is not in safe position");
		//	SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
		//	return TRUE;
		//}
	}


	if (WPR_MoveFocusToSafe() == FALSE)
	{
		return TRUE;
	}

	switch (lPosition)
	{
		case 1:		//Gripper load
			X2_MoveTo(m_lLoadPos_X2);
			//AfxMessageBox("m_lLoadPos_X2", MB_SYSTEMMODAL);
			m_lWLGeneral_TmpA = m_lLoadPos_X2;
			break;

		case 2:		//Gripper ready
			X2_MoveTo(m_lReadyPos_X2);
			m_lWLGeneral_TmpA = m_lReadyPos_X2;
			break;

		case 3:		//Gripper Barcode
			X2_MoveTo(m_lBarcodePos_X2);
			m_lWLGeneral_TmpA = m_lBarcodePos_X2;
			break;

		case 4:		//Loader Z Top slot level
			if (lMagazineNo >= 0 && lMagazineNo < GetWL2MaxMgznNo())
			{
				Z2_MoveTo(m_stWaferMagazine2[lMagazineNo].m_lTopLevel_Z);	
				m_lWLGeneral_TmpA = m_stWaferMagazine2[lMagazineNo].m_lTopLevel_Z;
			}
			break;

		case 6:		//2D Barcode position	//v3.70T3
			X2_MoveTo(m_l2DBarcodePos_X2);
			m_lWLGeneral_TmpA = m_l2DBarcodePos_X2;
			break;
		
		case 7:		// check barcode position
			X2_MoveTo(m_lBarcodeCheckPos_X2);
			m_lWLGeneral_TmpA = m_lBarcodeCheckPos_X2;
			break;
		
		case 8:
			break;
		
		case 9:
			break;
		default:	//Gripper Unload
			X2_MoveTo(m_lUnloadPos_X2);
			m_lWLGeneral_TmpA = m_lUnloadPos_X2;
			break;
	}


	return TRUE;
}

LONG CWaferLoader::KeyInPosition2(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} WLSETUP;
	WLSETUP stInfo;


	svMsg.GetMsg(sizeof(WLSETUP), &stInfo);


	//Check KeyIn Positon Limit
	if (stInfo.lPosition < 4)
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER2, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER2, MS896A_CFG_CH_MAX_DISTANCE);								
	}
	else
	{
		//v4.02T6
		if (((stInfo.lPosition == 4) || (stInfo.lPosition == 5)) && 
				(IsWaferFrameDetect2() == TRUE))		
		{
			SetErrorMessage("Frame is detected on track");
			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONTRACK);
			return TRUE;
		}

		//Klocwork	//v4.24T11
		//if ( IsMagazineSafeToMove2() == FALSE )
		//{
		//	SetErrorMessage("Gripper is not in safe position");
		//	SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
		//	return TRUE;
		//}

		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER2_Z, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER2_Z, MS896A_CFG_CH_MAX_DISTANCE);								
	}

	if (stInfo.lStep < lMinPos) 
	{
		stInfo.lStep = lMinPos;
	}

	if (stInfo.lStep > lMaxPos)
	{
		stInfo.lStep = lMaxPos;
	}

	if (WPR_MoveFocusToSafe() == FALSE)
	{
		return TRUE;
	}


	switch (stInfo.lPosition)
	{
		case 1:		//Gripper load
			m_lWLGeneral_2 = stInfo.lStep;
			X2_MoveTo(m_lWLGeneral_2);
			break;

		case 2:		//Gripper Ready
			m_lWLGeneral_3 = stInfo.lStep;
			X2_MoveTo(m_lWLGeneral_3);
			break;

		case 3:		//Gripper barcode
			m_lWLGeneral_4 = stInfo.lStep;
			X2_MoveTo(m_lWLGeneral_4);
			break;

		case 4:		//Loader Z Top slot level
			m_lWLGeneral_5 = stInfo.lStep;
			Z2_MoveTo(m_lWLGeneral_5);
			break;

		case 5:		//Loader Z Btn slot level
			m_lWLGeneral_6 = stInfo.lStep;
			Z2_MoveTo(m_lWLGeneral_6);
			break;

		case 6:		//2D Barcode posn X
			m_lWLGeneral_7 = stInfo.lStep;
			X2_MoveTo(m_lWLGeneral_7);
			break;
		
		case 7:		// Gripper check barcode
			m_lWLGeneral_8	= stInfo.lStep;
			X2_MoveTo(m_lWLGeneral_8);
			break;

		case 8:
			break;
		case 9:
			break;

		default:	//Gripper Unload
			m_lWLGeneral_1 = stInfo.lStep;
			X2_MoveTo(m_lWLGeneral_1);
			break;
	}

	return TRUE;
}


LONG CWaferLoader::MovePosPosition2(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} WLSETUP;

	WLSETUP stInfo;

	svMsg.GetMsg(sizeof(WLSETUP), &stInfo);


	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lWLGeneral_2 + stInfo.lStep;	
			break;	//Gripper load
		case 2:		
			lCurrentPos = m_lWLGeneral_3 + stInfo.lStep;	
			break;	//Gripper Ready
		case 3:		
			lCurrentPos = m_lWLGeneral_4 + stInfo.lStep;	
			break;	//Gripper barcode
		case 4:		
			lCurrentPos = m_lWLGeneral_5 + stInfo.lStep;	
			break;	//Loader Z Top slot
		case 5:		
			lCurrentPos = m_lWLGeneral_6 + stInfo.lStep;	
			break;	//Loader Z Btn slot
		case 6:		
			lCurrentPos = m_lWLGeneral_7 + stInfo.lStep;	
			break;	//2D Barcode	//v3.70T3
		case 7:		
			lCurrentPos = m_lWLGeneral_8 + stInfo.lStep;	
			break;	//Gripper check barcode
		case 8:		
			break;
		case 9:		
			break;
		default:	
			lCurrentPos = m_lWLGeneral_1 + stInfo.lStep;	
			break;	//Gripper Unload
	}

	//Check limit range
	if ((stInfo.lPosition < 4) || (stInfo.lPosition >= 6))
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER2, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER2, MS896A_CFG_CH_MAX_DISTANCE);								

		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			X2_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 1:		
					m_lWLGeneral_2 = lCurrentPos;	
					break;
				case 2:		
					m_lWLGeneral_3 = lCurrentPos;	
					break;
				case 3:		
					m_lWLGeneral_4 = lCurrentPos;	
					break;
				case 6:		
					m_lWLGeneral_7 = lCurrentPos;	
					break;		//v3.70T3
				case 7:		
					m_lWLGeneral_8 = lCurrentPos;	
					break;
				case 8:		
					break;
				case 9:		
					break;
				default:	
					m_lWLGeneral_1 = lCurrentPos;	
					break;
			}
		}
	}
	else
	{
		//Klocwork	//v4.24T11
		//if ( IsMagazineSafeToMove2() == FALSE )
		//{
		//	SetErrorMessage("Gripper is not in safe position");
		//	SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
		//	return TRUE;
		//}

		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER2_Z, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER2_Z, MS896A_CFG_CH_MAX_DISTANCE);								

		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			Z2_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 4:		
					m_lWLGeneral_5 = lCurrentPos;	
					break;
				case 5:		
					m_lWLGeneral_6 = lCurrentPos;	
					break;
				case 8:		
					break;
				case 9:		
					break;
				default:	
					m_lWLGeneral_5 = lCurrentPos;	
					break;
			}
		}
	}

	return TRUE;
}


LONG CWaferLoader::MoveNegPosition2(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} WLSETUP;

	WLSETUP stInfo;

	svMsg.GetMsg(sizeof(WLSETUP), &stInfo);


	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lWLGeneral_2 - stInfo.lStep;	
			break;	//Gripper load
		case 2:		
			lCurrentPos = m_lWLGeneral_3 - stInfo.lStep;	
			break;	//Gripper Ready
		case 3:		
			lCurrentPos = m_lWLGeneral_4 - stInfo.lStep;	
			break;	//Gripper barcode
		case 4:		
			lCurrentPos = m_lWLGeneral_5 - stInfo.lStep;	
			break;	//Loader Z Top slot
		case 5:		
			lCurrentPos = m_lWLGeneral_6 - stInfo.lStep;	
			break;	//Loader Z Btn slot
		case 6:		
			lCurrentPos = m_lWLGeneral_7 - stInfo.lStep;	
			break;	//2D barcode
		case 7:		
			lCurrentPos = m_lWLGeneral_8 - stInfo.lStep;	
			break;	//Gripper check barcode
		case 8:		
			break;
		case 9:		
			break;
		default:	
			lCurrentPos = m_lWLGeneral_1 - stInfo.lStep;	
			break;	//Gripper Unload
	}

	//Check limit range
	if ((stInfo.lPosition < 4) || (stInfo.lPosition >= 6))
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER2, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER2, MS896A_CFG_CH_MAX_DISTANCE);								

		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			X2_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 1:		
					m_lWLGeneral_2 = lCurrentPos;	
					break;
				case 2:		
					m_lWLGeneral_3 = lCurrentPos;	
					break;
				case 3:		
					m_lWLGeneral_4 = lCurrentPos;	
					break;
				case 6:		
					m_lWLGeneral_7 = lCurrentPos;	
					break;		//v3.70T3
				case 7:		
					m_lWLGeneral_8 = lCurrentPos;	
					break;
				case 8:		
					break;
				case 9:		
					break;
				default:	
					m_lWLGeneral_1 = lCurrentPos;	
					break;
			}
		}
	}
	else
	{
		//Klocwork	//v4.24T11
		//if ( IsMagazineSafeToMove2() == FALSE )
		//{
		//	SetErrorMessage("Gripper is not in safe position");
		//	SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
		//	return TRUE;
		//}

		lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER2_Z, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFLOADER2_Z, MS896A_CFG_CH_MAX_DISTANCE);								

		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			Z2_MoveTo(lCurrentPos);

			switch (stInfo.lPosition)
			{
				case 4:		
					m_lWLGeneral_5 = lCurrentPos;	
					break;
				case 5:		
					m_lWLGeneral_6 = lCurrentPos;	
					break;
				case 8:		
					break;
				case 9:		
					break;
				default:	
					m_lWLGeneral_5 = lCurrentPos;	
					break;
			}
		}
	}

	return TRUE;
}


LONG CWaferLoader::ConfirmSetup2(IPC_CServiceMessage &svMsg)
{
	LONG lMagazineNo;
	svMsg.GetMsg(sizeof(LONG), &lMagazineNo);

	if (m_bDisableWLWithExp)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_lLoadPos_X2 != m_lWLGeneral_2)
	{
		m_lLoadPos_X2 = m_lWLGeneral_2;
		LogItems(GRIPPER_LOAD_POS2);
	}
	else if (m_lUnloadPos_X != m_lWLGeneral_1)
	{
		m_lUnloadPos_X2 = m_lWLGeneral_1;
		LogItems(GRIPPER_UNLOAD_POS2);
	}
	else if (m_lReadyPos_X2 != m_lWLGeneral_3)
	{
		m_lReadyPos_X2 = m_lWLGeneral_3;
		LogItems(GRIPPER_READY_POS2);
	}
	else if (m_lBarcodePos_X2 != m_lWLGeneral_4)
	{
		m_lBarcodePos_X2 = m_lWLGeneral_4;
		LogItems(GRIPPER_BC_POS2);
	}
	
	//Update variable
	m_lUnloadPos_X2		= m_lWLGeneral_1;		
	m_lLoadPos_X2		= m_lWLGeneral_2;		
	m_lReadyPos_X2		= m_lWLGeneral_3;		
	m_lBarcodePos_X2	= m_lWLGeneral_4;

	if (lMagazineNo >= 0 && lMagazineNo < GetWL2MaxMgznNo())
	{
		if (m_stWaferMagazine2[lMagazineNo].m_lTopLevel_Z != m_lWLGeneral_5)
		{
			m_stWaferMagazine2[lMagazineNo].m_lTopLevel_Z = m_lWLGeneral_5;
			LogItems(MAGAZINE_TOP_SLOT_POS2);
		}

		if( m_lWLGeneral_9>WL_MAX_MAG_SLOT || m_lWLGeneral_9<1 )
			m_lWLGeneral_9 = WL_MAX_MAG_SLOT;
		m_stWaferMagazine2[lMagazineNo].m_lNoOfSlots = m_lWLGeneral_9;
		m_stWaferMagazine2[lMagazineNo].m_dSlotPitch = m_dWLGeneral_10;
	}

	m_l2DBarcodePos_X	= m_lWLGeneral_7;		//v3.70T3
	m_lBarcodeCheckPos_X	= m_lWLGeneral_8;

	SaveData();

	INT nStatus = CheckExpander2();
	if (nStatus == Err_ExpanderLockFailed)				// if cannot engage
	{
		nStatus = ManualCheckExpander2();				// try manual-engage with T motor off
		if (nStatus == Err_ExpanderLockFailed)  		// if still cannot engage
		{
			SetErrorMessage("Gripper HOME check expander fails!");
		}
		else
		{
			X2_MoveTo(0);
		}
	}
	else
	{
		X2_MoveTo(0);
	}

	//if (!IsWaferFrameDetect() && IsMagazineSafeToMove())	//v4.02T6	//Mag & gripper protection //SanAn
	//{
	//	Sleep(500);
	//	Z_MoveTo(m_lTopSlotLevel_Z);
	//}
	
	X2_MoveTo(0);
	SetStatusMessage("WL Gripper / Elevator position is updated");

	//CEID#7100
	SendGripper2Event_7100(m_lUnloadPos_X2, m_lStepOnUnloadPos2, m_lLoadPos_X2, m_lStepOnLoadPos2,
						   m_lReadyPos_X2, m_lBarcodePos_X2, lMagazineNo, m_lCurrentSlotNo2, m_lSkipSlotNo2);

	//v3.27T2
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferLoader::CancelSetup2(IPC_CServiceMessage &svMsg)
{
	LONG lMagazineNo;
	svMsg.GetMsg(sizeof(LONG), &lMagazineNo);

	if (m_bDisableWLWithExp)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//Restore HMI variable
	m_lWLGeneral_1		= m_lUnloadPos_X2;		
	m_lWLGeneral_2		= m_lLoadPos_X2;		
	m_lWLGeneral_3		= m_lReadyPos_X2;		
	m_lWLGeneral_4		= m_lBarcodePos_X2;			
	m_lWLGeneral_7		= m_l2DBarcodePos_X2;
	m_lWLGeneral_8		= m_lBarcodeCheckPos_X2;

	if (lMagazineNo >= 0 && lMagazineNo < GetWL2MaxMgznNo())
	{
		m_lWLGeneral_5 = m_stWaferMagazine2[lMagazineNo].m_lTopLevel_Z;
		m_lWLGeneral_9 = m_stWaferMagazine2[lMagazineNo].m_lNoOfSlots;
		m_dWLGeneral_10 = m_stWaferMagazine2[lMagazineNo].m_dSlotPitch;
	}

	INT nStatus = CheckExpander2();
	if (nStatus == Err_ExpanderLockFailed)				// if cannot engage
	{
		nStatus = ManualCheckExpander2();				// try manual-engage with T motor off
		if (nStatus == Err_ExpanderLockFailed)  		// if still cannot engage
		{
			SetErrorMessage("Gripper HOME check expander fails!");
		}
		else
		{
			X2_MoveTo(0);
		}
	}
	else
	{
		X2_MoveTo(0);
	}
	
	X2_MoveTo(0);

	//if (!IsWaferFrameDetect() && IsMagazineSafeToMove())	//v4.02T6	//Mag & gripper protection //SanAn
	//{
	//	Sleep(500);
	//	Z_MoveTo(m_lTopSlotLevel_Z);
	//}

	//v3.27T2
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::SelectMagazine2(IPC_CServiceMessage &svMsg)
{
	LONG lMagazineNo = 0;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lMagazineNo);

	if (lMagazineNo < 0 || lMagazineNo >= GetWL2MaxMgznNo())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	m_lWLGeneral_5 = m_stWaferMagazine2[lMagazineNo].m_lTopLevel_Z;
	m_lWLGeneral_9 = m_stWaferMagazine2[lMagazineNo].m_lNoOfSlots;
	m_dWLGeneral_10 = m_stWaferMagazine2[lMagazineNo].m_dSlotPitch;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferLoader::UpdateHMIData2(IPC_CServiceMessage &svMsg)
{
	LONG lMagazineNo = 0;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lMagazineNo);

	m_lWLGeneral_1		= m_lUnloadPos_X2;		
	m_lWLGeneral_2		= m_lLoadPos_X2;		
	m_lWLGeneral_3		= m_lReadyPos_X2;		
	m_lWLGeneral_4		= m_lBarcodePos_X2;	
	
	//m_lWLGeneral_5		= m_lTopSlotLevel_Z;
	m_lWLGeneral_7		= m_l2DBarcodePos_X2;
	m_lWLGeneral_8		= m_lBarcodeCheckPos_X2;

	if (lMagazineNo>=0 && lMagazineNo < GetWL2MaxMgznNo())
	{
		m_lWLGeneral_5 = m_stWaferMagazine2[lMagazineNo].m_lTopLevel_Z;
		m_lWLGeneral_9 = m_stWaferMagazine2[lMagazineNo].m_lNoOfSlots;
		m_dWLGeneral_10 = m_stWaferMagazine2[lMagazineNo].m_dSlotPitch;
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferLoader::SaveSetupData(IPC_CServiceMessage &svMsg)
{
	SaveData();

	//CEID#7101
	SendSetupEvent_7101();

	//v3.27T2
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

		
LONG CWaferLoader::GenerateConfigData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	bReturn = GenerateConfigData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferLoader::SetUnloadPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bTeachFromHome = FALSE;


	svMsg.GetMsg(sizeof(BOOL), &bTeachFromHome);

	if (!m_fHardware)
	{	
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (BondArmMoveToBlow() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);
	X_Home();
	Sleep(200);

	if (bTeachFromHome == TRUE)
	{
		MoveWaferTableLoadUnload(0, 0, FALSE);								//v4.16T5	//MS100 9Inch
		m_bCheckWaferLimit = FALSE;	
	}
	else
	{
		BOOL bStatus = MoveWaferTableLoadUnload(m_lUnloadPhyPosX, m_lUnloadPhyPosY, TRUE);	//v4.16T5	//MS100 9Inch
		m_bCheckWaferLimit = FALSE;	

		if (WPR_MoveFocusToSafe() == FALSE)
		{
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (bStatus)	//v4.22T8
		{
			INT nStatus = TRUE;
			if ((GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||		//v4.01	
					(GetExpType() == WL_EXP_GEAR_DIRECTRING))				//v4.28T4
			{
				nStatus = ExpanderDCMotorPlatform(WL_UP, FALSE, TRUE, FALSE, m_lExpDCMotorDacValue);
			}
			else if (GetExpType() == WL_EXP_CYLINDER)
			{
				nStatus = ExpanderCylinderMotorPlatform(WL_UP, FALSE, TRUE, FALSE);
			}
			else
			{
				nStatus = ExpanderGearPlatform(WL_UP, FALSE, TRUE, FALSE);
			}
		}
		else
		{
			CString szErr = _T("UNLOAD position hits current wafer limit!");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
			SetErrorMessage("WL: Set UNLOAD pos hits wafer limit");
		}
	}

	SetJoystickLimit(TRUE);
	SetWaferTableJoystick(TRUE);
	SetGripperPower(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferLoader::ConfirmUnloadPosition(IPC_CServiceMessage &svMsg)
{
	LONG lThetaPosition = 0;

	BOOL bConfirm = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bConfirm);

	if (!m_fHardware)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);
	if (bConfirm)	//v4.21
	{
		GetES101WTEncoder(&m_lUnloadPhyPosX, &m_lUnloadPhyPosY, &lThetaPosition, FALSE);
	}

	SetGripperLevel(WL_OFF);
	X_Home();
	Sleep(200);

	if (CMS896AApp::m_bMS100Plus9InchOption)		//v4.21
	{
		if ((GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
				(GetExpType() == WL_EXP_GEAR_DIRECTRING))				//v4.28T4
		{
			ExpanderDCMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE, m_lExpDCMotorDacValue);
		}
		else if (GetExpType() == WL_EXP_CYLINDER)
		{
			ExpanderCylinderMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE);
		}
		else
		{
			ExpanderGearPlatform(WL_DOWN, FALSE, FALSE, FALSE);
		}

		Sleep(200);
		MoveWaferTableNoCheck(0, 0, FALSE);
	}

	m_bCheckWaferLimit = TRUE;		//v4.16T5
	SetJoystickLimit(FALSE);
	SetWaferTableJoystick(TRUE);

	SaveData();

	BOOL bReturn = TRUE;
	//svMsg.InitMessage(sizeof(BOOL), &bReturn);

	SetStatusMessage("WL Unload position is set");

	//CEID#7102
	SendWTUnloadPosnEvent_7102(m_lUnloadPhyPosX, m_lUnloadPhyPosY);

	//v3.27T2
	//BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferLoader::MoveToPosition(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG lPosX;
		LONG lPosY;
	} MOVETO;

	MOVETO stInfo;
	svMsg.GetMsg(sizeof(MOVETO), &stInfo);

	if (!m_fHardware)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveBackLightToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);

	if (CMS896AApp::m_bMS100Plus9InchOption)		//v4.16T3	//MS100 9Inch
	{
		MoveWaferTableNoCheck(stInfo.lPosX, stInfo.lPosY);
	}
	else if (m_bUseDualTablesOption)				//v4.24T9
	{
		MoveWaferTableNoCheck(stInfo.lPosX, stInfo.lPosY);
	}
	else
	{
		MoveWaferTable(stInfo.lPosX, stInfo.lPosY);
	}

	SetWaferTableJoystick(TRUE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferLoader::SetUnloadPosition2(IPC_CServiceMessage &svMsg)
{
	BOOL bTeachFromHome = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bTeachFromHome);

	if (!m_fHardware)
	{	
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (IsESDualWT() == FALSE)
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	if (BondArmMoveToBlow() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE, TRUE);
	X2_Home();
	Sleep(200);

	if (bTeachFromHome == TRUE)
	{
		MoveWaferTableNoCheck(0, 0, FALSE, SFM_WAIT, TRUE);		//v4.16T5	//MS100 9Inch
		m_bCheckWaferLimit = FALSE;	
	}
	else
	{
		BOOL bStatus = MoveWaferTableNoCheck(m_lUnloadPhyPosX2, m_lUnloadPhyPosY2, FALSE, SFM_WAIT, TRUE);	//v4.16T5	//MS100 9Inch
		m_bCheckWaferLimit = FALSE;	

		if (bStatus)	//v4.22T8
		{
			ExpanderDCMotor2Platform(WL_UP, FALSE, TRUE, FALSE, m_lExpDCMotorDacValue);
		}
		else
		{
			CString szErr = _T("UNLOAD position hits current wafer limit!");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
			SetErrorMessage("WL: Set UNLOAD pos hits wafer limit");
		}
	}

	SetJoystickLimit(TRUE);
	SetWaferTableJoystick(TRUE, TRUE);
	SetGripper2Power(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferLoader::ConfirmUnloadPosition2(IPC_CServiceMessage &svMsg)
{
	LONG lThetaPosition = 0;

	BOOL bConfirm = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bConfirm);

	if (!m_fHardware)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (IsESDualWT() == FALSE)
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);
	if (bConfirm)	//v4.21
	{
		GetWaferTable2Encoder(&m_lUnloadPhyPosX2, &m_lUnloadPhyPosY2, &lThetaPosition);
	}

	SetGripper2Level(WL_OFF);
	X2_Home();
	Sleep(200);

	ExpanderDCMotor2Platform(WL_DOWN, FALSE, FALSE, FALSE, m_lExpDCMotorDacValue);
	Sleep(200);
	
	m_bCheckWaferLimit = TRUE;		//v4.16T5
	SetJoystickLimit(FALSE);
	SetWaferTableJoystick(TRUE);

	SaveData();

	BOOL bReturn = TRUE;
	//svMsg.InitMessage(sizeof(BOOL), &bReturn);
	SetStatusMessage("WL Unload position is set");

	//CEID#7102
	SendWTUnloadPosn2Event_7102(m_lUnloadPhyPosX2, m_lUnloadPhyPosY2);

	SetGemValue("WL_WTUnloadX2", m_lUnloadPhyPosX2);	// 3331
	SetGemValue("WL_WTUnloadY2", m_lUnloadPhyPosY2); 
	SendEvent(SG_CEID_WL_WTUNLOADPOSN, FALSE);			// 7102

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::MoveToPosition2(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG lPosX;
		LONG lPosY;
	} MOVETO;

	MOVETO stInfo;
	svMsg.GetMsg(sizeof(MOVETO), &stInfo);

	if (!m_fHardware)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (IsESDualWT() == FALSE)
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveBackLightToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);
	MoveWaferTableNoCheck(stInfo.lPosX, stInfo.lPosY, TRUE, SFM_WAIT, TRUE);

	SetWaferTableJoystick(TRUE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CWaferLoader::SetHomeDiePosition(IPC_CServiceMessage &svMsg)
{
	BOOL bWT2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bWT2);

	if (!m_fHardware)
	{	
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bDisableWL)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveBackLightToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsWL1ExpanderSafeToMove() == FALSE || IsWL2ExpanderSafeToMove() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE, bWT2);

	LONG lX = 0, lY = 0;
	if( bWT2 )
	{
		GetWT2HomeDiePhyPosn(lX, lY);
	}
	else
	{
		lX = m_lHomeDiePhyPosX;
		lY = m_lHomeDiePhyPosY;
	}
	if (m_bUseDualTablesOption)			//v4.24T9
	{
		m_bCheckWaferLimit = FALSE;

		MoveWaferTableNoCheck(lX, lY, FALSE, SFM_WAIT, bWT2);	

		if (IsWT1UnderCamera() || IsWT2UnderCamera())
		{
			m_lBackLightZStatus = 0;	//	4.24TX 4
			MoveES101BackLightZUpDn(TRUE);	// in prestart, if align ok, check current is safe, UP BL Z
			MoveFocusToWafer(FALSE);
		}
	}
	else
	{
		MoveWaferTable(lX, lY);
	}

	SetJoystickLimit(TRUE, bWT2);
	SetWaferTableJoystick(TRUE, bWT2);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CWaferLoader::ConfirmHomeDiePosition(IPC_CServiceMessage &svMsg)
{
	LONG lThetaPosition = 0;

	if (!m_fHardware)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	
	if (m_bDisableWT)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//andrewng //2020-0528
	/*
	if (MoveBackLightToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (WPR_MoveFocusToSafe() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	*/

	//if (IsWT1UnderCamera())
	//{
	GetES101WTEncoder(&m_lHomeDiePhyPosX, &m_lHomeDiePhyPosY, &lThetaPosition, FALSE);
	//}
	/*else if (IsWT2UnderCamera())
	{
		GetES101WTEncoder(&m_lWT2HomeDiePhyPosX, &m_lWT2HomeDiePhyPosY, &lThetaPosition, TRUE);
	}
	else
	{
		HmiMessage("Main table home position out of wafer limit!", "WL home position");
	}

	if (m_bUseDualTablesOption)
	{
		m_bCheckWaferLimit = TRUE;
	}*/

	SetWaferTableJoystick(FALSE);

	SetJoystickLimit(FALSE);
	SetWaferTableJoystick(TRUE);

	SaveData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	SetStatusMessage("WL HomeDie position is set");

	LogItems(WAFER_HOME_DIE_POS_X);
	LogItems(WAFER_HOME_DIE_POS_Y);
	
	//CEID#7103
	SendHomeDieTablePosnEvent_7103(m_lHomeDiePhyPosX, m_lHomeDiePhyPosY);

	return TRUE;
}


LONG CWaferLoader::MoveToSlot(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_fHardware)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWLWithExp)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	typedef struct 
	{
		LONG 	lMagNo;
		LONG	lSlotNo;
	} WL_SETUP;

	if( IsES101()==FALSE )
	{
		WL_SETUP stData;
		svMsg.GetMsg(sizeof(WL_SETUP), &stData);

		//v4.55A10
		m_lCurrentMagNo		= stData.lMagNo;
		m_lCurrentSlotNo	= stData.lSlotNo;
	}

	if (m_bUseDualTablesOption || m_bUseBLAsLoaderZ)	//v4.31T11
	{
		if (m_lCurrentMagNo > GetWL1MaxMgznNo())			//Max. 4 mgzns for Yealy MS100Plus with single loader
		{
			m_lCurrentMagNo = GetWL1MaxMgznNo();
		}

		if (m_lCurrentSlotNo > m_stWaferMagazine[m_lCurrentMagNo - 1].m_lNoOfSlots)
		{
			m_lCurrentSlotNo = m_stWaferMagazine[m_lCurrentMagNo - 1].m_lNoOfSlots;
		}

		if (m_lCurrentSlotNo == 0)
		{
			m_lCurrentSlotNo = 1;
		}
	}
	else
	{
		m_lCurrentMagNo = 1;	//v4.31T11

		if (m_lCurrentSlotNo == 0)
		{
			m_lCurrentSlotNo = 1;
		}
		
		if (m_lCurrentSlotNo >= m_lTotalSlotNo)
		{
			m_lCurrentSlotNo = m_lTotalSlotNo;
		}
	}

	if (m_bUseBLAsLoaderZ)
	{
		bReturn = MoveToBLMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
		MoveToBLMagazineSlot_Sync();
	}
	else
	{
		bReturn = MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
	}

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::MoveToSlot2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_fHardware)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWLWithExp)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	LONG lMaxMagNo = GetWL2MaxMgznNo();

	if (m_lCurrentMagNo2 > lMaxMagNo)		//Max. 3 mgzns only for ES101
	{
		m_lCurrentMagNo2 = lMaxMagNo;
	}
	else if (m_lCurrentMagNo2 < 1)
	{
		m_lCurrentMagNo2 = 1;
	}
	else
	{
		if (m_lCurrentSlotNo2 > m_stWaferMagazine2[m_lCurrentMagNo2 - 1].m_lNoOfSlots)
		{
			m_lCurrentMagNo2	= m_lCurrentMagNo2 + 1;
			
			if (m_lCurrentMagNo2 > lMaxMagNo)
			{
				m_lCurrentMagNo2 = lMaxMagNo;
				m_lCurrentSlotNo2 = m_stWaferMagazine2[m_lCurrentMagNo2 - 1].m_lNoOfSlots;
			}
			else
			{
				m_lCurrentSlotNo2 = 1;
			}
		}
		else if (m_lCurrentSlotNo2 < 1)
		{
			m_lCurrentMagNo2 = m_lCurrentMagNo2 - 1;
			
			if (m_lCurrentMagNo2 < 1)
			{
				m_lCurrentMagNo2 = 1;
				m_lCurrentSlotNo2 = 1;
			}
			else
			{
				m_lCurrentSlotNo2 = m_stWaferMagazine2[m_lCurrentMagNo2 - 1].m_lNoOfSlots;
			}
		}
	}
	
	if (MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2) != TRUE)
	{
		bReturn = FALSE;
	}

	SaveData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::ManualChangeFilmFrame(IPC_CServiceMessage &svMsg)
{
	BOOL	bLoad;
	BOOL	bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CMSLogFileUtility::Instance()->WL_LogStatus("ManualChangeFilmFrame");
	svMsg.GetMsg(sizeof(BOOL), &bLoad);
	m_lBinMixCount = 0;
	(*m_psmfSRam)["BinOutputFile"]["Osram Wafer Unload"]		= FALSE;
	(*m_psmfSRam)["WaferTable"]["OsramWaferEnd"]				= FALSE;
	(*m_psmfSRam)["WaferTable"]["OsramWaferEndOK"]				= TRUE;	
	(*m_psmfSRam)["WaferTable"]["OsramLoadReset"]				= TRUE;
	if (m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	m_bFrameOperationInAutoMode = FALSE;

	SetErrorMessage("Start Manual Change Film Frame");

	if (IsAllMotorsEnable() == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Motor is OFF");
		SetErrorMessage("Motor is OFF");

		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveBackLightToSafeLevel() == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("MoveBackLightToSafeLevel failure");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("MoveEjectorElevatorToSafeLevel failure");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bUseDualTablesOption == TRUE && m_bIsMagazineFull == TRUE && bLoad == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("m_bIsMagazineFull");
		SetAlert_Red_Yellow(IDS_WL_MAG1_FULL);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (GetExpType() == WL_EXP_NONE)		//v4.39T10	//Knowles MS109
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("GetExpType failure");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	//v3.97	//Lextar
	if ((GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER && GetExpType() != WL_EXP_NONE) 
			&& IsExpanderLock())	//v4.16T6
	{
		SetExpanderLock(WL_OFF);
		Sleep(500);

		if (IsExpanderLock())
		{
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			SetErrorMessage("Expander unlock fails in ManualChange FilmFrame");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	if (CMS896AStn::MotionReadInputBit("iPressureSensor") == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Machine pressure is low");
		SetErrorMessage("Machine pressure is low");

		SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo) != TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("MoveToMagazineSlot failure");

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	BOOL bGetWIPMgzSlot_AutoLine = FALSE;
	BOOL bSetupModeWithoutAutoLine = TRUE;
	if (IsMSAutoLineMode())
	{
		bSetupModeWithoutAutoLine = FALSE;
		CStringList List;
		List.AddTail("WIP Slots");
		if (bLoad)
		{
			List.AddTail("Input Slots");
		}
		else
		{
			List.AddTail("Output Slots");
		}
		if (!pApp->CheckSanAnProductionMode())
		{
			//Not Operation Mode
			List.AddTail("Machine Setup");
		}
		CString szSelection;
		LONG a = HmiSelection("Select Slots", "Select", List, 0);
		if (a == 0)
		{
			bGetWIPMgzSlot_AutoLine = TRUE;
		}
		else if (a == 2)
		{
			bSetupModeWithoutAutoLine = TRUE;
		}
		else if (a == -1)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	CTmpChange<BOOL> bAutoLoadUnloadTest(&m_bAutoLoadUnloadTest, bSetupModeWithoutAutoLine);

	SetWaferTableJoystick(FALSE);

	if( IsMS90() )
	{
		MoveWaferTable(0, 0);
	}
	///*********/ StartLoadingAlert(); /*********/
	if (bLoad == TRUE)
	{
		if (m_bEjNeedReplacement)
		{
			SetAlert_Yellow_Green(IDS_BH_REPLACE_EJECTOR); 
			SetErrorMessage("WL: Load Frame Rejected Due To Hit Ej Life Time Limit");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
		else if (IsOpNeedReplaceEjrPinOrCollet())
		{
			SetErrorMessage("WL: Load Frame Rejected Due To Hit Ej Life Time Limit");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		//pllm
		if (m_bFrameExistOnExp)
		{
			CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("Frame index existed on table");		//v4.43T10
			if (HmiMessage("Frame index existed on table; continue to load film frame?", "Manual Load Frame", glHMI_MBX_OKCANCEL | 0x80000000) == glHMI_CANCEL)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		//v4.42T6
		IPC_CServiceMessage stMsg;
		BOOL bReply = TRUE;
		INT nConvID = m_comClient.SendRequest("MapSorter", _T("CheckIfAllTaskInIdleState"), stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{	
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReply);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		if (!bReply)	//If not all task in IDLE state, do not load frame with barcode! (SanAn)		
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		m_bManualLoadFrame = TRUE;
		INT nLoadFrameStatus = FALSE;

		if (m_bReadBarCodeOnTable == FALSE)
		{
			SetErrorMessage("Start Read Barcode on Gripper Sequnce (Manual)");
			nLoadFrameStatus = LoadFilmFrame(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, bGetWIPMgzSlot_AutoLine, TRUE);
		}
		else
		{
			SetErrorMessage("Start Read Barcode on Wafer Table Sequence (Manual)");

			nLoadFrameStatus = LoadFilmFrame(TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, bGetWIPMgzSlot_AutoLine, TRUE);		//v4.39T10
		}

		if (nLoadFrameStatus == TRUE)
		{
			SetStatusMessage("WL Filmframe is loaded");
		}
//		SendGemEvent_WL_Load(TRUE);
	}
	else
	{
		//pllm
		if (!m_bFrameExistOnExp)
		{
			CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("No frame index available on table");		//v4.43T10
			if (HmiMessage("No frame index available on table; continue to unload film frame?", "Manual Unload Frame", glHMI_MBX_OKCANCEL | 0x80000000) == glHMI_CANCEL)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		BOOL bIsWaferEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferEnd"];
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( bIsWaferEnd || IsPrescanEnded()==FALSE || pApp->GetCustomerName()!=CTM_OSRAM )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Manual unload frame, clear map file.");
			ManualClearCurrentMap();		//v4.35T3	//Knowles MS109
		}

		BOOL bStatus = SECS_UploadbackupMapinUnload();		//v4.59A13	//Renesas MS90

		if (bStatus)
		{
			if (UnloadFilmFrame(TRUE, FALSE, bGetWIPMgzSlot_AutoLine, TRUE) == TRUE)
			{
				SetStatusMessage("WL Filmframe is unloaded");
			}
			SendGemEvent_WL_Load(FALSE); 
		}
	}


	if (IsES101() && IsWT2InUse() == FALSE)
	{
		//Reset Wafer Align Status
		IPC_CServiceMessage rReqMsg;
		INT nConvID = 0;
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ResetAlignStatus", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}	// reset align status

	///*********/ CloseLoadingAlert(); /*********/
	SetWaferTableJoystick(TRUE);
	m_bManualLoadFrame = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::ManualChangeFilmFrame_WT2(IPC_CServiceMessage &svMsg)
{
	BOOL	bLoad;
	BOOL	bReturn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bLoad);

	if (IsESDualWT() == FALSE)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (m_bDisableWLWithExp)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	m_bFrameOperationInAutoMode = FALSE;

	SetErrorMessage("Start Manual Change WT2 Film Frame");
	
	if (IsAllMotorsEnable2() == FALSE)
	{
		//LogStatusInfo("Motor is OFF", TRUE);

		CMSLogFileUtility::Instance()->WL_LogStatus("Motor is OFF");
		SetErrorMessage("Motor is OFF");

		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);

		bReturn = TRUE;		//v2.56
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveBackLightToSafeLevel() == FALSE)
	{
		bReturn = TRUE;		//v2.56
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bUseDualTablesOption == TRUE && m_bIsMagazine2Full == TRUE && bLoad == TRUE)
	{
		BOOL bReturn = TRUE;
		SetAlert_Red_Yellow(IDS_WL_MAG2_FULL);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (GetExpType() == WL_EXP_NONE)		//v4.39T10	//Knowles MS109
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	//v3.97	//Lextar
	if ((GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER && GetExpType() != WL_EXP_NONE) && 
			IsExpander2Lock())	//v4.16T6
	{
		SetExpander2Lock(WL_OFF);
		Sleep(500);

		if (IsExpander2Lock())
		{
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			SetErrorMessage("Expander unlock fails in ManualChange FilmFrame WT2");
			bReturn = TRUE;		//v2.56
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	if (CMS896AStn::MotionReadInputBit("iPressureSensor") == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Machine pressure is low");
		SetErrorMessage("Machine pressure is low");

		SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);

		bReturn = TRUE;		//v2.56
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2) != TRUE)
	{
		bReturn = TRUE;		//v2.56
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);

	///*********/ StartLoadingAlert(); /*********/
	if (bLoad == TRUE)
	{
		//pllm
		if (m_bFrameExistOnExp2)
		{
			CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("Frame index existed on table 2");		//anichia001
			if (HmiMessage("Frame index existed on table; continue to load film frame?", "Manual Load Frame", glHMI_MBX_OKCANCEL | 0x80000000) == glHMI_CANCEL)
			{
				bReturn = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		m_bManualLoadFrame = TRUE;
		INT nLoadFrameStatus = FALSE;

		SetErrorMessage("Start Read Barcode on Wafer Table Sequence (Manual)");

		nLoadFrameStatus = LoadFilmFrame_WT2(TRUE, FALSE, FALSE, FALSE);

		if (nLoadFrameStatus == TRUE)
		{
			SetStatusMessage("WL Filmframe is loaded");
		}

		SendGemEvent_WL_Load(TRUE);
	}
	else
	{
		//pllm
		if (!m_bFrameExistOnExp2)
		{
			CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("No frame index available on table");		//anichia001
			if (HmiMessage("No frame index available on table; continue to unload film frame?", "Manual Unload Frame", glHMI_MBX_OKCANCEL | 0x80000000) == glHMI_CANCEL)
			{
				bReturn = TRUE;	
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		if (UnloadFilmFrame_WT2(TRUE, FALSE) == TRUE)
		{
			SetStatusMessage("WL Filmframe is unloaded");
		}

		SendGemEvent_WL_Load(FALSE);
	}

	if (IsES101() && IsWT2InUse())
	{
		//Reset Wafer Align Status
		IPC_CServiceMessage rReqMsg;
		INT nConvID = 0;
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ResetAlignStatus", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}

	///*********/ CloseLoadingAlert(); /*********/
	SetWaferTableJoystick(TRUE);
	m_bManualLoadFrame = FALSE;
	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferLoader::AutoChangeFilmFrame(IPC_CServiceMessage &svMsg)
{
	BOOL	bBurnIn;
	LONG	lResult;
	svMsg.GetMsg(sizeof(BOOL), &bBurnIn);

	CMSLogFileUtility::Instance()->WL_LogStatus("AutoChangeFilmFrame");
	if (IsWLManualMode())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL not fully auto mode");
		lResult = -1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return Err_No_Error;
	}

	SaveScanTimeEvent("  WFL: Start Auto Change WL FilmFrame");
	CMSLogFileUtility::Instance()->WL_LogStatus("Start Auto Change WL FilmFrame");

	if (GetExpType() == WL_EXP_NONE)		//v4.39T10	//Knowles MS109
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL with NONE expander type");
		lResult = -1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return Err_No_Error;
	}


	BOOL bNeedUnload = m_bFrameExistOnExp;		//v3.28T2
	
	//Re-verify All Ref Dice found (for Cree)
	if( !bBurnIn && (IsBlkFunc2Enable() || IsPrescanBlkPick() || m_bNonBlkPkEndVerifyRefDie) )
	{
		if ( m_bUseReVerifyRefDie )
		{
			if ((BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["VerifyRefDieFail"] == TRUE)
			{
				(*m_psmfSRam)["WaferTable"]["VerifyRefDieFail"] = FALSE;

				UnloadFilmFrame(TRUE, FALSE);

				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return Err_No_Error;
			}
		}
	}


	BOOL bUseUSlotForUnload = TRUE;
	if (m_bUseBLAsLoaderZ && m_bFrameInLowerSlot)		//v4.31T12	//MS100 Single loade & buffer table config
	{
		//If next frame was PRELOAD, first unload current frame to Buffer table USlot
		LONG lStatus = WL_UnloadFromTableToBuffer(bBurnIn, bUseUSlotForUnload, TRUE);
		if (lStatus != TRUE)
		{
			SetErrorMessage("Auto Change WL UNLOAD frame Table->Buffer fails");
			CMSLogFileUtility::Instance()->WL_LogStatus("WL unload frame Table->Buffer fail");
			lResult = 0;
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return Err_UnloadFrameFail;
		}

		CMS896AStn::m_bWLReadyToUnloadBufferFrame	= FALSE;
		CMS896AStn::m_bWLFrameToBeUnloadOnBuffer	= TRUE;
		if (!m_bUseBLAsLoaderZDisablePreload)	//v4.39T9	//Yealy requested feature
		{
			CMS896AStn::m_bWLFrameNeedPreloadToBuffer	= TRUE;
		}
	}
	else
	{
		SECS_UploadbackupMapinUnload();		//v4.59A13	//Renesas MS90

		INT nStatus = TRUE;

		if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) && (m_lUnloadOpenExtraTime > 0) )
		{
			//v4.59A25	//Renesas MS90
			nStatus	= UnloadFilmFrame(TRUE, bBurnIn);
		}
		else
		{
			nStatus	= UnloadFilmFrame(FALSE, bBurnIn);
		}

		if (nStatus != TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL unload frame fail");
			lResult = 0;
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return Err_UnloadFrameFail;
		}
		
		CMS896AStn::m_bWLFrameToBeUnloadOnBuffer	= FALSE;
		CMS896AStn::m_bWLReadyToUnloadBufferFrame	= FALSE;
		if (m_bUseBLAsLoaderZ && !m_bUseBLAsLoaderZDisablePreload)	//v4.39T9	//Yealy requested feature
		{
			CMS896AStn::m_bWLFrameNeedPreloadToBuffer	= TRUE;
		}
		else
		{
			CMS896AStn::m_bWLFrameNeedPreloadToBuffer	= FALSE;
		}
	}

	SetStatusMessage("WL Filmframe is unloaded");
	CMSLogFileUtility::Instance()->WL_LogStatus("WL Filmframe is unloaded");

	SendGemEvent_WL_Load(FALSE, TRUE, TRUE);	//v4.59A11

	//v3.28T2
	//Ejector Cap Clean count for AOI machines
	if (bNeedUnload)
	{
		if ((m_lEjrCapCleanLimit > 0) && (m_lEjrCapCleanLimit <= (m_lEjrCapCleanCount + 1)))
		{
			IPC_CServiceMessage stMsg;
			BOOL bPowerOn = FALSE;
			stMsg.InitMessage(sizeof(BOOL), &bPowerOn);
			int nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_Diag_PowerOn_Ej", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			SetErrorMessage("Ejector Cap Clean limit is hit");

			SetEjectorVacuum(TRUE);

			//Change Wafer Camera
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SelectWaferCamera", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//Set Lighting
			typedef struct 
			{
				LONG lGroupID;
				LONG lCoaxLevel;
				LONG lRingLevel;
				LONG lSideLevel;
				LONG lBackLevel;
			} SETLIGHT;
			SETLIGHT stInfo;

			stInfo.lGroupID = 0;
			stInfo.lCoaxLevel = 60;
			stInfo.lSideLevel = 20;
			stInfo.lRingLevel = 0;
			stInfo.lBackLevel = 0;

			stMsg.InitMessage(sizeof(SETLIGHT), &stInfo);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetGeneralLighting2", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			
			//Set Exposure Time
			LONG lExposureTimeLevel = 7;
			stMsg.InitMessage(sizeof(LONG), &lExposureTimeLevel);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetGeneralExposureTime", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			SetAlert_Red_Back(IDS_WL_CLEAN_EJECOTR_CAP_LIMIT);
			SetEjectorVacuum(FALSE);

			bPowerOn = TRUE;
			stMsg.InitMessage(sizeof(BOOL), &bPowerOn);
			nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_Diag_PowerOn_Ej", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_Diag_Home_Ej", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			m_lEjrCapCleanCount = 0;		//v3.59T1	//Reset automatically for SanAn
			CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
			if (pApp->GetFeatureStatus(MS896A_FUNC_CLEAN_EJECTOR_CAP_STOP_CYCLE))
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("WL unload frame clean ejector cap stop");
				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return Err_No_Error;
			}
		}
		else if (m_lEjrCapCleanLimit > 0)
		{
			m_lEjrCapCleanCount++;
		}
		else
		{
			m_lEjrCapCleanCount = 0;
		}

		//v4.39T10	//Silan (WanYiMing)
		if (m_bEjNeedReplacement == TRUE)
		{
			//m_bEjNeedReplacement = FALSE;
			//SetAlert_Red_Yellow(IDS_BH_REPLACE_EJECTOR);
			SetAlert_Yellow_Green(IDS_BH_REPLACE_EJECTOR); //4.51D8 BH0003

			SetErrorMessage("WL: autobond cycle aborted due to EJ pin lifetime & replacement alarm, please reset it");
			lResult = 0;
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return Err_EjNeedReplacement;
		}

		if (IsOpNeedReplaceEjrPinOrCollet()) // Check need to replace Ejr pin before loading wafer
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Ej Pin Need Replacement");
			SetErrorMessage("Ej Pin Need Replacement");
			OpenCloseExpander(FALSE);
			lResult = 0;
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return Err_EjNeedReplacement;
		}
	}

	if( IsAOIOnlyMachine()==FALSE )
	{
		m_bIsGetCurrentSlot = FALSE; // Wafer end, in auto cycle to change frame, after unload, should load next.
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (m_bUseBLAsLoaderZ && m_bFrameInLowerSlot)	//v4.31T12	//MS100 Single loade & buffer table config
	{
		//Then LOad NExt frame onto table, and let BL sequence to UNLOAD current 
		// frame when time allowed afterwards;
		LONG lStatus = WL_LoadFromBufferToTable(bBurnIn, TRUE, !bUseUSlotForUnload, TRUE);
		if (lStatus != TRUE)
		{
			SetErrorMessage("Auto Change WL LOAD frame buffer->Table fails");
			CMSLogFileUtility::Instance()->WL_LogStatus("Auto load WL frame Buffer->Table fail");
			lResult = 0;
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return Err_LoadFrameFail;
		}
		SendGemEvent_WL_Load(TRUE);
	}
	else
	{
		CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE, TRUE);	//anichia002

		if (m_bReadBarCodeOnTable == FALSE)
		{
			//SetErrorMessage("Start Read Barcode On Gripper Sequence ");

			INT nStatus = TRUE;
			if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) && (m_lUnloadOpenExtraTime > 0) )
			{
				//v4.59A25	//Renesas MS90
				nStatus	= LoadFilmFrame(TRUE, bBurnIn, TRUE, TRUE);
			}
			else
			{
				nStatus = LoadFilmFrame(FALSE, bBurnIn, TRUE, TRUE);
			}

			if (nStatus != TRUE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("load Read Barcode On Gripper Sequence fail");
				
				//v4.46T11	//Knowles by Hou Jian Ming
				if (CMS896AApp::m_bMS100Plus9InchOption && !m_bExpanderStatus)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Load failure trigger 9Inch table to HOME");
					if (!MoveWaferTableNoCheck(0, 0, FALSE, TRUE))
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("9Inch table to HOME fail");
						SetErrorMessage("Manual Open Expander: table move to LOAD fail");
						SetAlert_Red_Yellow(IDS_WT_CANNOT_MOVE);
					}
					else
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("9Inch table to HOME done");
					}
				}
				
				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return Err_LoadFrameFail;
			}
		}
		else
		{
			int nResult = LoadFilmFrame(FALSE, bBurnIn, TRUE, TRUE, TRUE);
			if ( nResult != Err_No_Error)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("load Read Barcode On Wafer Table Sequence fail");
				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return nResult;
			}
		}
	}

	if (m_bUseBLAsLoaderZ)
	{
		X_MoveTo(m_lReadyPos_X, FALSE);	//Prepare for next PRELOAD to buffer table
	}

	lResult = 1;
	if (IsWLSemiAutoMode())
	{
		lResult = 0;	//Force to abort current LOAD wafer operation
	}

	SetStatusMessage("WL Filmframe is loaded");
	CMSLogFileUtility::Instance()->WL_LogStatus("WL Filmframe is loaded");
	
	svMsg.InitMessage(sizeof(LONG), &lResult);
	return Err_No_Error;
}

LONG CWaferLoader::LoopTestCounter(IPC_CServiceMessage &svMsg)
{
	svMsg.GetMsg(sizeof(LONG), &m_lLoopTestCounter);

	return TRUE;
}

LONG CWaferLoader::AutoLoadUnloadTest(IPC_CServiceMessage &svMsg)
{
	BOOL	bBurnIn;
	svMsg.GetMsg(sizeof(BOOL), &bBurnIn);
	LONG	lResult = 1;
	INT		iRtn;
	LONG	lCurrSlot = m_lCurrentSlotNo;

	if (IsWLManualMode())
	{
		CString szContent, szTitle;

		szTitle.LoadString(HMB_WL_AUTO_UNLOAD_TEST);
		szContent.LoadString(HMB_WL_NOT_FULLYAUTO);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);

		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (IsAllMotorsEnable() == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Motor is OFF");
		SetErrorMessage("Motor is OFF");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);

		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (GetExpType() == WL_EXP_NONE)		//v4.39T10	//Knowles MS109
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL NONE expander mode does not support this operation");
		SetErrorMessage("WL NONE expander mode does not support this operation");
		HmiMessage_Red_Yellow("WL NONE expander mode does not support this operation");

		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);

	LONG lTotalSlots = m_lTotalSlotNo;
	if (m_bUseDualTablesOption || m_bUseBLAsLoaderZ)		//v4.31T11
	{
		if ((m_lCurrentMagNo > 0) && (m_lCurrentMagNo <= GetWL1MaxMgznNo()))
		{
			lTotalSlots = m_stWaferMagazine[m_lCurrentMagNo - 1].m_lNoOfSlots;
		}
	}


	CString szMsg;
	szMsg.Format("Auto LoadUnload Test Start - %ld", lTotalSlots);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	BOOL bAutoLoadUnloadQCTest = TRUE;
	if (IsMSAutoLineMode())
	{
		CStringList List;
		List.AddTail("QC Test(Normal)");
		List.AddTail("Test With SIS");
		CString szSelection;
		LONG a = HmiSelection("Select Slots", "Select", List, 0);
		if (a == 0)
		{
			bAutoLoadUnloadQCTest = TRUE;
		}
		else if (a == 1)
		{
			bAutoLoadUnloadQCTest = FALSE;
			lTotalSlots = MS_WL_AUTO_LINE_INPUT_END_MGZN_SLOT;
		}
		else
		{
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return TRUE;
		}
	}
	CTmpChange<BOOL> bAutoLoadUnloadTest(&m_bAutoLoadUnloadTest, bAutoLoadUnloadQCTest);
//	for (LONG i = 1; i <= lTotalSlots; i++)
	LONG i = 1;
	LONG lCount = 0;
	CString szLog;
	while (TRUE)
	{
		m_lCurrentSlotNo = i;

		if (MoveToMagazineSlot(m_lCurrentMagNo, i) != TRUE)
		{
			szLog.Format("Auto LoadUnload Test fails; Mgzn=%ld, Slot=%ld, Count=%d", m_lCurrentMagNo, i, lCount);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			HmiMessage(szLog);

			svMsg.InitMessage(sizeof(LONG), &lResult);
			m_lCurrentSlotNo = lCurrSlot;
			return TRUE;
		}

		iRtn = LoadFilmFrame(TRUE, FALSE, FALSE, FALSE);
		if (iRtn != TRUE)
		{
			if (iRtn == Err_NoFrameDetected)
			{
				if ((GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
						(GetExpType() == WL_EXP_GEAR_DIRECTRING))				//v4.28T4
				{
					ExpanderDCMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE, m_lExpDCMotorDacValue);
				}
				else if (GetExpType() == WL_EXP_CYLINDER)
				{
					ExpanderCylinderMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE);
				}
				else
				{
					ExpanderGearPlatform(WL_DOWN, FALSE, FALSE, FALSE);
				}

				//Reset All Motors
				HouseKeeping(FALSE, FALSE, FALSE, TRUE);
			}
			szLog.Format("Auto LoadUnload Test fails2;Count=%d",lCount);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			svMsg.InitMessage(sizeof(LONG), &lResult);
			m_lCurrentSlotNo = lCurrSlot;
			return TRUE;
		}

		SetStatusMessage("WL Filmframe is loaded");
		Sleep(500);
		X_Home();		//v3.93
		Sleep(500);

		if (UnloadFilmFrame(TRUE, FALSE) != TRUE)
		{
			svMsg.InitMessage(sizeof(LONG), &lResult);
			m_lCurrentSlotNo = lCurrSlot;
			szLog.Format("Auto LoadUnload Test fails3;Count=%d",lCount);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			return TRUE;
		}
		SetStatusMessage("WL Filmframe is unloaded");
		Sleep(1000);

		i = i + m_lSkipSlotNo;
		if (++i > lTotalSlots)
		{
			Z_Home();//Matt:Prevent Hardware Accumulated Error
			i = 1;
		}
		if (++lCount >= m_lLoopTestCounter)
		{
			break;
		}
	}

	SetWaferTableJoystick(TRUE);

	svMsg.InitMessage(sizeof(LONG), &lResult);
	m_lCurrentSlotNo = lCurrSlot;
	CMSLogFileUtility::Instance()->MS_LogOperation("LoadUnload Loop Test Done");
	HmiMessage("LoadUnload Loop Test Done");
	return TRUE;
}

LONG CWaferLoader::StartContLoadUnloadTest(IPC_CServiceMessage &svMsg)
{
	BOOL	bStart = FALSE;
	LONG	lResult;
	svMsg.GetMsg(sizeof(BOOL), &bStart);
	LONG	lCurrSlot = m_lCurrentSlotNo;

	if (IsWLManualMode())
	{
		CString szContent, szTitle;

		szTitle.LoadString(HMB_WL_AUTO_UNLOAD_TEST);
		szContent.LoadString(HMB_WL_NOT_FULLYAUTO);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);

		lResult = 1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (IsAllMotorsEnable() == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Motor is OFF");
		SetErrorMessage("Motor is OFF");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);

		lResult = 1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (GetExpType() == WL_EXP_NONE)		//v4.39T10	//Knowles MS109
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL NONE expander mode does not support this operation");
		SetErrorMessage("WL NONE expander mode does not support this operation");
		HmiMessage_Red_Yellow("WL NONE expander mode does not support this operation");

		lResult = 1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);

	if (bStart)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("WL ContLoadUnload Test Start");
HmiMessage("Start");
		m_bContLoadUnloedStart = TRUE;
	}
	else
	{
		m_bContLoadUnloedStart = FALSE;
		CMSLogFileUtility::Instance()->MS_LogOperation("WL ContLoadUnload Test DOne");
HmiMessage("Stop");
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return TRUE;
}


BOOL CWaferLoader::ContLoadUnloadTest()
{
	INT iRtn = 0;
	LONG lTotalSlots	= m_lTotalSlotNo;
	LONG lCurrSlot		= m_lCurrentSlotNo;
	CString szLog;

	if ((m_lCurrentMagNo > 0) && (m_lCurrentMagNo <= GetWL1MaxMgznNo()))
	{
		lTotalSlots = m_stWaferMagazine[m_lCurrentMagNo - 1].m_lNoOfSlots;
	}

	for (INT i = 1; i <= lTotalSlots; i++)
	{
		if (!m_bContLoadUnloedStart)
			break;

		m_lCurrentSlotNo = i;

		if (IsAllMotorsEnable() == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Motor is OFF");
			SetErrorMessage("Motor is OFF in ContLoadUnloadTest");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			m_bContLoadUnloedStart = FALSE;
			return FALSE;
		}

		if (MoveToMagazineSlot(m_lCurrentMagNo, i) != TRUE)
		{
			CString szLog;
			szLog.Format("Auto LoadUnload Test fails; Mgzn=%ld, Slot=%ld", m_lCurrentMagNo, i);
			HmiMessage(szLog);
			m_lCurrentSlotNo = lCurrSlot;
			m_bContLoadUnloedStart = FALSE;
			return FALSE;
		}

		iRtn = LoadFilmFrame(TRUE, FALSE, FALSE, FALSE);
		if (iRtn != TRUE)
		{
			if (iRtn == Err_NoFrameDetected)
			{
				if ((GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
						(GetExpType() == WL_EXP_GEAR_DIRECTRING))				//v4.28T4
				{
					ExpanderDCMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE, m_lExpDCMotorDacValue);
				}
				else if (GetExpType() == WL_EXP_CYLINDER)
				{
					ExpanderCylinderMotorPlatform(WL_DOWN, FALSE, FALSE, FALSE);
				}
				else
				{
					ExpanderGearPlatform(WL_DOWN, FALSE, FALSE, FALSE);
				}

				//Reset All Motors
				HouseKeeping(FALSE, FALSE, FALSE, TRUE);
			}

			m_lCurrentSlotNo = lCurrSlot;
			m_bContLoadUnloedStart = FALSE;
			return FALSE;
		}

		szLog.Format("WL Filmframe is loaded (Cont) Slot=%ld", m_lCurrentSlotNo+1);
		//SetStatusMessage("WL Filmframe is unloaded");
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		//SetStatusMessage("WL Filmframe is loaded");
		
		Sleep(500);
		X_Home();
		Sleep(500);

		if (UnloadFilmFrame(TRUE, FALSE) != TRUE)
		{
			m_lCurrentSlotNo = lCurrSlot;
			m_bContLoadUnloedStart = FALSE;
			return FALSE;
		}

		szLog.Format("WL Filmframe is unloaded (Cont) Slot=%ld", m_lCurrentSlotNo+1);
		//SetStatusMessage("WL Filmframe is unloaded");
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		Sleep(1000);

		i = i + m_lSkipSlotNo;
	}

	return TRUE;
}


LONG CWaferLoader::AutoLoadUnloadTest2(IPC_CServiceMessage& svMsg)
{
	BOOL	bBurnIn;
	LONG	lResult;
	svMsg.GetMsg(sizeof(BOOL), &bBurnIn);
	int	i;
	INT		iRtn;
	LONG	lCurrSlot = m_lCurrentSlotNo2;

	if (IsWLManualMode())
	{
		CString szContent, szTitle;

		szTitle.LoadString(HMB_WL_AUTO_UNLOAD_TEST);
		szContent.LoadString(HMB_WL_NOT_FULLYAUTO);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);

		lResult = 1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (IsAllMotorsEnable() == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Motor is OFF");
		SetErrorMessage("Motor is OFF");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);

		lResult = 1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (GetExpType() == WL_EXP_NONE)		//v4.39T10	//Knowles MS109
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL2 NONE expander mode does not support this operation");
		SetErrorMessage("WL2 NONE expander mode does not support this operation");
		HmiMessage_Red_Yellow("WL2 NONE expander mode does not support this operation");

		lResult = 1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE, TRUE);

	LONG lTotalSlots = m_lTotalSlotNo;	//XX
	if (m_bUseDualTablesOption)		//v4.31T11
	{
		if ((m_lCurrentMagNo2 > 0) && (m_lCurrentMagNo2 <= GetWL2MaxMgznNo()))
		{
			lTotalSlots = m_stWaferMagazine2[m_lCurrentMagNo2 - 1].m_lNoOfSlots;
		}
	}

	CString szMsg;
	szMsg.Format("Auto LoadUnload Test2 Start - %ld", lTotalSlots);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	for (i = 1; i <= lTotalSlots; i++)
	{
		m_lCurrentSlotNo2 = i;

		if (MoveToMagazineSlot2(m_lCurrentMagNo2, i) != TRUE)
		{
			CString szLog;
			szLog.Format("Auto LoadUnload Test2 fails; Mgzn=%ld, Slot=%ld", m_lCurrentMagNo2, i);
			HmiMessage(szLog);

			lResult = 1;	//v2.60
			svMsg.InitMessage(sizeof(LONG), &lResult);
			m_lCurrentSlotNo2 = lCurrSlot;
			return TRUE;
		}

		iRtn = LoadFilmFrame_WT2(TRUE, FALSE, FALSE, FALSE);
		if (iRtn != TRUE)
		{
			if (iRtn == Err_NoFrameDetected)
			{
				if ((GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
						(GetExpType() == WL_EXP_GEAR_DIRECTRING))				//v4.28T4
				{
					ExpanderDCMotor2Platform(WL_DOWN, FALSE, FALSE, FALSE, m_lExpDCMotorDacValue);
				}
				else if (GetExpType() == WL_EXP_CYLINDER)
				{
					ExpanderCylinderMotorPlatform2(WL_DOWN, FALSE, FALSE, FALSE);
				}
				else
				{
					ExpanderGearPlatform(WL_DOWN, FALSE, FALSE, FALSE);	//XX
				}

				//Reset All Motors
				HouseKeeping_WT2(FALSE, FALSE, FALSE, TRUE);
			}
			lResult = 1;	//v2.60
			svMsg.InitMessage(sizeof(LONG), &lResult);
			m_lCurrentSlotNo2 = lCurrSlot;
			return TRUE;
		}

		SetStatusMessage("WL2 Filmframe is loaded");
		Sleep(500);
		X2_Home();		//v3.93
		Sleep(500);

		if (UnloadFilmFrame_WT2(TRUE, FALSE) != TRUE)
		{
			lResult = 1;	//v2.60
			svMsg.InitMessage(sizeof(LONG), &lResult);
			m_lCurrentSlotNo2 = lCurrSlot;
			return TRUE;
		}
		SetStatusMessage("WL2 Filmframe is unloaded");
		Sleep(1000);

		i = i + m_lSkipSlotNo2;
	}

	SetWaferTableJoystick(TRUE, TRUE);

	lResult = 1;
	svMsg.InitMessage(sizeof(LONG), &lResult);
	m_lCurrentSlotNo2 = lCurrSlot;
	CMSLogFileUtility::Instance()->MS_LogOperation("Auto LoadUnload Test2 Done");

	return TRUE;
}

LONG CWaferLoader::ManualSearchHomeDie(IPC_CServiceMessage &svMsg)
{
	//	426TX	1	move to auto align button

	BOOL bReturn = TRUE;		//v2.60
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferLoader::GetHomeDieRecordID(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}



LONG CWaferLoader::WLBurnInSetup(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	BOOL bStart = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bStart);

	if (IsESMachine())
	{
		m_bIsMagazineFull = FALSE;
		m_lCurrentMagNo	= 1;
		m_lCurrentSlotNo = 1;

		m_bFrameExistOnExp  = FALSE;
		m_bFrameExistOnExp2 = FALSE;
		if( IsESDualWL() )
		{
			m_lCurrentMagNo2	= 1;
			m_lCurrentSlotNo2	= 1;
			m_bIsMagazine2Full	= FALSE;
		}
			
		SetWT2InUse(FALSE);
		m_bUseBarcode			= FALSE;
		m_bIsMagazine2Full		= FALSE;
		m_bIsMagazineFull		= FALSE;
		m_lFrameToBePreloaded	= 0;
		m_lFrameToBeUnloaded	= 0;
		m_bFrameExistOnExp2		= TRUE;	
		SaveData();
		SaveScanTimeEvent("Burn in start/stop reset wafer loader");

		if ( IsWL1ExpanderSafeToMove() == FALSE )
		{
			bReturn = FALSE;
		}
		else
		{
			MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
			Z_Home();
			MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
		}

		if( IsESDualWL() )
		{
			if ( IsWL2ExpanderSafeToMove() == FALSE )
			{
				bReturn = FALSE;
			}
			else
			{
				MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2);
				Z2_Home();
				MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2);
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferLoader::AutoLoadWaferFrame(IPC_CServiceMessage &svMsg)
{
	BOOL	bBurnIn;
	BOOL	bFrameInside = FALSE;
	LONG	lResult;
	svMsg.GetMsg(sizeof(BOOL), &bBurnIn);

	CMSLogFileUtility::Instance()->WL_LogStatus("AutoLoadWaferFrame");

	m_bFrameOperationInAutoMode = FALSE;
	SetES101PreloadFrameDone(FALSE);	//	4.24TX 4	//	auto load wafer frame in manual mode
	if (IsWLManualMode())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL manual mode");
		lResult = -1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (m_fHardware && !X_IsPowerOn())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper not power ON");
		HmiMessage_Red_Yellow("wafer gripper not power ON!");
		lResult = -1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (GetExpType() == WL_EXP_NONE)		//v4.39T10	//Knowles MS109
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL NONE expander type does not support AutoLoad WaferFrame");
		HmiMessage_Red_Yellow("WL NONE expander type does not support AutoLoad WaferFrame");
		lResult = -1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}


	if (GetExpType() == WL_EXP_VACUUM)
	{
		if (CMS896AApp::m_bMS60)		//v4.46T28
		{
			bFrameInside = m_bFrameExistOnExp;
		}
		else
			bFrameInside = !IsFramePosition() && !IsFrameProtection();
	}
	else if (GetExpType() == WL_EXP_NONE)	//v4.39T10	//Knowles MS109
	{
		//nStatus = TRUE;
	}
	else
	{
		if (IsWLExpanderOpen() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander not closed");
			SetErrorMessage("Expander not closed");

			lResult = -1;
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return TRUE;
		}

		bFrameInside = m_bFrameExistOnExp;	//IsFrameExist();
	}

	SaveData();

	if (bFrameInside == TRUE)
	{
		lResult = 1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);

	//Continue to search filmframe & get current magazine slot
	m_bIsGetCurrentSlot = TRUE;		//Get NEXT frame slot

	if (IsOpNeedReplaceEjrPinOrCollet()) // Check need to replace Ejr pin before loading wafer
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Ej Pin Need Replacement");
		SetErrorMessage("Ej Pin Need Replacement");
		lResult = 0;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (m_bUseBLAsLoaderZ)		//v4.31T11	//MS100Plus single loader with buffer table config
	{
		BOOL bUseUSlotforUnload = TRUE;

		if (!m_bFrameInLowerSlot)		//If no frame was PRELOAD
		{
			if (m_bFrameInUpperSlot)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Frame exists in upper buffer slot");
				HmiMessage_Red_Yellow("AUTOBOND: Frame exists in upper buffer slot!  Please reset buffer table.");
				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return 1;
			}

			//Use direct-LOAD from mgzn; same as normal MS899/MS100 sequence
			if (LoadFilmFrame(TRUE, bBurnIn, TRUE, TRUE) != TRUE)
			{
				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return TRUE;
			}
		}
		else
		{
			LONG lStatus = 0;

			//Move BL magazine to current slot
			lStatus = MoveToBLMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, FALSE, TRUE, !bUseUSlotforUnload);
			if (lStatus != TRUE)
			{
				CString szErr;
				szErr.Format("WL::AutoLoad WaferFrame: MoveToBLMagazineSlot fails - Mag=%ld, Slot=%ld", m_lCurrentMagNo, m_lCurrentSlotNo);
				SetErrorMessage(szErr);
				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return TRUE;
			}
			MoveToBLMagazineSlot_Sync();

			//Load PRELOAD frame from buffer to table
			lStatus = WL_LoadFromBufferToTable(bBurnIn, TRUE, !bUseUSlotforUnload);
			if (lStatus != TRUE)
			{
				SetErrorMessage("Auto LOAD WL frame buffer->Table fails");
				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return TRUE;
			}
			SendGemEvent_WL_Load(TRUE);
		}

		X_MoveTo(m_lReadyPos_X, FALSE);						//Prepare for next PRELOAD to buffer table
		if (!m_bUseBLAsLoaderZDisablePreload)				//v4.39T9	//Yealy requested feature
		{
			CMS896AStn::m_bWLFrameNeedPreloadToBuffer = TRUE;    //Trigger PRELOAD of next frame in BL task
		}	
	}
	else
	{
		CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE, TRUE);	//anichia001
		if (m_bReadBarCodeOnTable == FALSE)
		{
			//SetErrorMessage("Start Read Barcode On Gripper Sequence ");
			if (LoadFilmFrame(TRUE, bBurnIn, TRUE, TRUE) != TRUE)
			{
				CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);	//anichia001
				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return TRUE;
			}
		}
		else
		{
			//SetErrorMessage("Start Read Barcode on Wafer Table Sequence (Manual)");
			if (LoadFilmFrame(TRUE, bBurnIn, TRUE, TRUE, TRUE) != TRUE)	//v4.39T10
			{
				CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);	//anichia001
				lResult = 0;
				svMsg.InitMessage(sizeof(LONG), &lResult);
				return TRUE;
			}
		}

		CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);			//anichia001
	}

	//v3.42T1
	lResult = 1;
	if (IsWLSemiAutoMode())		//New SEMI-AUTO mode
	{
		lResult = 0;	//Force to abort current loading
	}

	SetStatusMessage("WL Filmframe is loaded");
	svMsg.InitMessage(sizeof(LONG), &lResult);
	return TRUE;
}


LONG CWaferLoader::ManualOpenCloseExpander(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bOpen = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOpen);

	OpenCloseExpander(bOpen);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferLoader::OpenCloseExpander(BOOL bOpen)
{
	if (IsFrameDetect() == TRUE && m_bNoSensorCheck == FALSE)
	{
		SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
		SetErrorMessage("Frame detected on gripper");
		return FALSE;
	}

	// Lock the Wafer Table When the expander is Opening / Closing
	SetWaferTableJoystick(FALSE);

	if (bOpen && WPR_MoveFocusToSafe() == FALSE)
	{
		return FALSE;
	}

	if (MoveBackLightToSafeLevel() == FALSE)
	{
		return FALSE;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		return FALSE;
	}

	if (BondArmMoveToBlow() == FALSE)
	{
		return FALSE;
	}

	//v3.97	//Lextar
	if ((GetExpType() != WL_EXP_VACUUM)		&& 
			(GetExpType() != WL_EXP_CYLINDER)	&& 
			(GetExpType() != WL_EXP_NONE)		&&		//v4.39T10	//Knowles MS109
			IsExpanderLock())		//v4.16T6
	{
		SetExpanderLock(WL_OFF);
		Sleep(500);

		if (IsExpanderLock())
		{
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			SetErrorMessage("Expander unlock fails in Manual OpenClose Expander");
			return FALSE;
		}
	}
	else if (GetExpType() == WL_EXP_CYLINDER)
	{
		if ((IsExpanderOpen() == TRUE) && (bOpen == TRUE))
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander already open");
			SetErrorMessage("Expander already open");
			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			return FALSE;
		}
	}

	// Check-home gripper if OPEN expander
	if (bOpen && (X_Home() != gnOK))	//v2.58
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper HOME fails");
		SetErrorMessage("Gripper HOME fails");

		SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);

		return FALSE;
	}

	if (!HomeWaferTheta())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL: HomeWaferTheta fail in ManualOpenCloseExpander");
		SetErrorMessage("WL: HomeWaferTheta fail in ManualOpenCloseExpander");
		return FALSE;
	}
		

	if (bOpen)		//v3.60		//For production usage
	{
		if (!MoveWaferTableNoCheck(m_lUnloadPhyPosX, m_lUnloadPhyPosY, TRUE, TRUE))		//v4.16T5	//MS100 9Inch
		{
			SetErrorMessage("Manual Open Expander: table move fail");
			SetAlert_Red_Yellow(IDS_WT_CANNOT_MOVE);

			return FALSE;
		}
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Semitek" && pApp->GetProductLine() != "ZJG")
	{
		m_WaferMapWrapper.InitMap();	// clear wafer map if disable
	}

	INT nStatus = 0;
	BOOL bWaferRotateFail = FALSE;		//v4.46T13

	if ((GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
			(GetExpType() == WL_EXP_GEAR_DIRECTRING))				//v4.28T4
	{
		nStatus = ExpanderDCMotorPlatform(bOpen, FALSE, TRUE, FALSE, m_lExpDCMotorDacValue);
	}
	else if (GetExpType() == WL_EXP_CYLINDER)
	{
		nStatus = ExpanderCylinderMotorPlatform(bOpen, FALSE, TRUE, FALSE);
	}
	else
	{
		nStatus = ExpanderGearPlatform(bOpen, FALSE, TRUE, FALSE);
	}

	if (nStatus == Err_ExpanderAlreadyOpen)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander already open");
		SetErrorMessage("Expander already open");
		SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);

		ManualClearCurrentMap();			//v4.35T3	//Knowles MS109
	}
	else if (nStatus == Err_ExpanderAlreadyClose)
	{
		if (CMS896AApp::m_bMS100Plus9InchOption && !bOpen)
		{
			if (!MoveWaferTableNoCheck(0, 0, FALSE, TRUE))
			{
				SetErrorMessage("Manual Open Expander: table move to LOAD fail");
				SetAlert_Red_Yellow(IDS_WT_CANNOT_MOVE);
				return FALSE;
			}
			
			if (!AutoRotateWafer(FALSE))			//PLLM MS109	//v4.35T2
			{
				bWaferRotateFail = TRUE;
			}

			CheckRepeatMap();				//PLLM v3.74T33
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander already close");
			SetErrorMessage("Expander already close");
			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_CLOSE);
		}

		//Update Expander Status	//v4.41T1
		(*m_psmfSRam)["MS896A"]["Expander Status"] = bOpen;
		m_bExpanderStatus = bOpen;
		pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_STATUS, (int)m_bExpanderStatus);
	}
	else if (nStatus != TRUE)
	{
		if (bOpen == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot open");
			SetErrorMessage("Expander cannot open");
			SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot close");
			SetErrorMessage("Expander cannot close");
			SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);
		}
	}
	else
	{
		if (bOpen == TRUE)
		{
			//LogStatusInfo("Expander is opened", TRUE);
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander is opened");
			//SetErrorMessage("Expander is opened");
			CMSLogFileUtility::Instance()->MS_LogOperation("Expander is opened");	//v4.37T13

			ManualClearCurrentMap();		//v4.35T3	//Knowles MS109
		}
		else
		{
			//LogStatusInfo("Expander is closed", TRUE);
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander is closed");
			//SetErrorMessage("Expander is closed");
			CMSLogFileUtility::Instance()->MS_LogOperation("Expander is closed");	//v4.37T13

			if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.20
			{
				if (!MoveWaferTableNoCheck(0, 0, FALSE, TRUE))
				{
					SetErrorMessage("Manual Open Expander: table move to LOAD fail");
					SetAlert_Red_Yellow(IDS_WT_CANNOT_MOVE);
					return FALSE;
				}
			}
		}
	}

	//BH cannot go back to pre-pick
	HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
	if ((nStatus == TRUE) && (bOpen != TRUE))
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander is closed");
		SetErrorMessage("Expander is closed");

		//v4.39T10
		if ((GetExpType() != WL_EXP_VACUUM)			&& 
				(GetExpType() != WL_EXP_CYLINDER)	&& 
				(GetExpType() != WL_EXP_NONE))
		{
			SetExpanderLock(WL_OFF);	//v4.46T13
			
			LONG lLoopCounter = 0;
			while (IsExpanderLock())
			{
				lLoopCounter++;
				Sleep(1000);
				if (lLoopCounter > 5)
				{
					break;
				}
			}
		}

		Sleep(1000);
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		INT nRotateUnderCam = pApp->GetProfileInt(gszPROFILE_SETTING, gszES_ROTATE_T_UNDER_CAM, 0);
		if( (IsES101()==FALSE && IsES201()==FALSE) || (nRotateUnderCam==0) )
		{
			if (!AutoRotateWafer(FALSE))			//PLLM MS109	//v4.35T2
			{
				bWaferRotateFail = TRUE;
			}
		}
		CheckRepeatMap();				//PLLM v3.74T33
	}


	//v4.40T14	//TJ Sanan
	CString szLog;
	BOOL bReadBC = pApp->GetFeatureStatus(MS896A_FUNC_MANUAL_CLOSE_EXP_BC);
	if (!bOpen && bReadBC && m_bUseBarcode && !bWaferRotateFail)
	{
		Sleep(200);
		BOOL bNeedMoveHome = FALSE;
		if (CMS896AApp::m_bMS100Plus9InchOption)					//v4.47T17		//Testar
		{
			if (pApp->GetCustomerName() == _T("Testar"))			//v4.48A21
			{
				szLog.Format("Manual-Close-Exp Scan BC: Move WT (MS109) to (%ld, %ld)", m_lUnloadPhyPosX, m_lUnloadPhyPosY);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				MoveWaferTable(m_lUnloadPhyPosX, m_lUnloadPhyPosY);
				bNeedMoveHome = TRUE;
			}
			else
			{
				szLog = "Manual-Close-Exp Scan BC: Move WT to (0, 0)";
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				MoveWaferTable(0, 0);
			}
		}
		else
		{
			szLog.Format("Manual-Close-Exp Scan BC: Move WT to (%ld, %ld)", m_lUnloadPhyPosX, m_lUnloadPhyPosY);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			MoveWaferTable(m_lUnloadPhyPosX, m_lUnloadPhyPosY);
			bNeedMoveHome = TRUE;
		}

		INT nBarcodeStatus = ScanningBarcodeOnTable(TRUE, FALSE);	

		CString szOldBC = "";
		if (m_WaferMapWrapper.IsMapValid())
			szOldBC = GetMapFileName();
		szLog.Format("Manual-Close-Exp Scan BC: status = %d, BC = %s (Old = %s)",
							nBarcodeStatus, m_szBarcodeName, szOldBC);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

					
		if (pApp->GetCustomerName() == _T("Testar"))			//v4.48A21
		{
			if (!ManualCloseExpLoadMap())
			{
				bNeedMoveHome = FALSE;
			}
		}
		else
		{
			if ( (nBarcodeStatus == TRUE) && (m_szBarcodeName.GetLength() > 0) )
			{
				if (!ManualCloseExpLoadMap())
				{
					bNeedMoveHome = FALSE;
				}
			}
			else
			{
				SetAlert_Red_Yellow(IDS_WL_NO_BARCODE);
				bNeedMoveHome = FALSE;	//v4.48A21
			}
		}

		if (bNeedMoveHome)		//v4.47T17	//Testar
		{
			if (pApp->GetCustomerName() == _T("Testar"))			
			{
				szLog = "Manual-Close-Exp Scan BC: Move WT T to 0";
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				MoveWaferThetaTo(0);		//v4.48A21
			}

			MoveWaferTable(0, 0);
		}
	}

	return TRUE;
}

BOOL CWaferLoader::ManualCloseExpLoadMap()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szLog;
	CString szOldBC = "";

	BOOL bWaferEnd = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferEnd"];	//v4.44A19

	if (m_WaferMapWrapper.IsMapValid())
		szOldBC = GetMapFileName();

	//v4.48A15	//Testar
	if (pApp->GetCustomerName() == _T("Testar")) 
	{
		szLog.Format("Manual-Close-Exp Scan BC (Testar): MAP Name = %s, WaferEnd=%d, MapValid=%d", 
						szOldBC, bWaferEnd, m_WaferMapWrapper.IsMapValid());
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		if (bWaferEnd)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Close-Exp: no action due to WAFEREND");
		}
		else if (m_WaferMapWrapper.IsMapValid() == TRUE)
		{
			//Check BC with current map file loaded in menu
			if ( (m_szBarcodeName.GetLength() == 0) || 
				 (GetMapFileName().Find(m_szBarcodeName) == -1) )
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Close-Exp: BC Check fail; manual-input");
				SetAlert_Red_Yellow(IDS_WL_MAP_NOT_VALID);

				CString szNewBarcode	= _T("");
				CString szTitle		= "Please input barcode: ";
				CString szContent	= "Please input barcode manually";
				
				BOOL bInputStatus = TRUE;
				INT nCount = 0;
				do
				{
					//v4.51A20	//Klocwork
					nCount++;
					if (nCount > 10)
					{
						CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Close-Exp: manual-input aborted (nCount)");
						return FALSE;
					}

					bInputStatus = HmiStrInputKeyboard(szTitle, szNewBarcode);
			
					szLog.Format("Manual-Close-Exp Manual-Input: %s (%s)", szNewBarcode, GetMapFileName());
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


					if (!bInputStatus || szNewBarcode.IsEmpty())
					{
						CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Close-Exp: manual-input aborted");
						continue;
					}
					else if (szNewBarcode.GetLength()<=3)	//v4.48A21
					{
						CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Close-Exp: manual-input invalid BC");
						continue;
					}

					if (GetMapFileName().Find(szNewBarcode) != -1)
					{
						szLog.Format("Manual-Close-Exp Manual-Input OK - %s", szNewBarcode);	//v4.48A21
						CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
						//return TRUE;	//v4.48A21
						break;	//v4.51A20	//Klocwork
					}

				} while (1);
			}
			else
			{
				szLog.Format("Manual-Close-Exp Scan BC: BC (%s) matched and OK", m_szBarcodeName);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Close-Exp: BC check not enabled due to INVALID map");
		}

		return TRUE;
	}
	else
	{
		//auto load MAP file
		CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Close-Exp: Load Map - " + m_szBarcodeName);
		INT nID = LoadMapFileWithoutSyn(m_szBarcodeName);
		return SyncLoadMapFile(nID);
	}
}


LONG CWaferLoader::ManualOpenCloseExpander2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bOpen = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOpen);

	if (IsFrameDetect2() == TRUE && m_bNoSensorCheck == FALSE)
	{
		SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
		SetErrorMessage("Frame detected on gripper");
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	// Lock the Wafer Table When the expander is Opening / Closing
	SetWaferTableJoystick(FALSE, TRUE);

	if (bOpen && WPR_MoveFocusToSafe() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveBackLightToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (BondArmMoveToBlow() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//v3.97	//Lextar
	if ((GetExpType() != WL_EXP_VACUUM)		&& 
			(GetExpType() != WL_EXP_CYLINDER)	&& 
			(GetExpType() != WL_EXP_NONE)		&& 
			IsExpander2Lock())		//v4.16T6
	{
		SetExpander2Lock(WL_OFF);
		Sleep(500);

		if (IsExpander2Lock())
		{
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			SetErrorMessage("Expander2 unlock fails in Manual OpenClose Expander");
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}
	else if (GetExpType() == WL_EXP_CYLINDER)
	{
		if ((IsExpander2Open() == TRUE) && (bOpen == TRUE))
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 already open");
			SetErrorMessage("Expander2 already open");
			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	// Check-home gripper if OPEN expander
	if (bOpen && (X2_Home() != gnOK))	//v2.58
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper HOME fails");
		SetErrorMessage("Gripper HOME fails");

		SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	
	if (!HomeWaferTheta(TRUE))
	{
		bReturn = FALSE;
		CMSLogFileUtility::Instance()->WL_LogStatus("WL: HomeWaferTheta2 fail in ManualOpenCloseExpander");
		SetErrorMessage("WL: HomeWaferTheta2 fail in ManualOpenCloseExpander");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	if (bOpen)		//v3.60		//For production usage
	{
		// 4.24TX		
		if (!MoveWaferTableNoCheck(m_lUnloadPhyPosX2, m_lUnloadPhyPosY2, TRUE, SFM_WAIT, TRUE))		//v4.16T5	//MS100 9Inch
		{
			SetErrorMessage("Manual Open Expander2: table move fail");
			SetAlert_Red_Yellow(IDS_WT_CANNOT_MOVE);

			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	INT nStatus = 0;
	if ((GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
			(GetExpType() == WL_EXP_GEAR_DIRECTRING))				//v4.28T4
	{
		nStatus = ExpanderDCMotor2Platform(bOpen, FALSE, TRUE, FALSE, m_lExpDCMotorDacValue);
	}
	else if (GetExpType() == WL_EXP_CYLINDER)
	{
		nStatus = ExpanderCylinderMotorPlatform2(bOpen, FALSE, TRUE, FALSE);
	}
	
	if (nStatus == Err_ExpanderAlreadyOpen)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 already open");
		SetErrorMessage("Expander2 already open");
		SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
	}
	else if (nStatus == Err_ExpanderAlreadyClose)
	{
		if (CMS896AApp::m_bMS100Plus9InchOption && !bOpen)
		{
			if (!MoveWaferTableNoCheck(0, 0, FALSE, SFM_WAIT, TRUE))
			{
				SetErrorMessage("Manual Open Expander2: table move to LOAD fail");
				SetAlert_Red_Yellow(IDS_WT_CANNOT_MOVE);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 already close");
			SetErrorMessage("Expander2 already close");
			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_CLOSE);
		}
	}
	else if (nStatus != TRUE)
	{
		if (bOpen == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 cannot open");
			SetErrorMessage("Expander2 cannot open");
			SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 cannot close");
			SetErrorMessage("Expander2 cannot close");
			SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);
		}
	}
	else
	{
		if (bOpen == TRUE)
		{
			//LogStatusInfo("Expander is opened", TRUE);
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 is opened");
			SetErrorMessage("Expander2 is opened");
		}
		else
		{
			//LogStatusInfo("Expander is closed", TRUE);
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 is closed");
			SetErrorMessage("Expander2 is closed");

			if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.20
			{
				if (!MoveWaferTableNoCheck(0, 0, FALSE, SFM_WAIT, TRUE))
				{
					SetErrorMessage("Manual Open Expander2: table move to LOAD fail");
					SetAlert_Red_Yellow(IDS_WT_CANNOT_MOVE);
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return TRUE;
				}
			}
		}
	}

	//BH cannot go back to pre-pick
	HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}



LONG CWaferLoader::UpdateOutput(IPC_CServiceMessage &svMsg)
{
	BOOL bUpdate = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bUpdate);

	m_bUpdateOutput = bUpdate;

	bUpdate = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bUpdate);
	return 1;
}


LONG CWaferLoader::UpdateExpanderType(IPC_CServiceMessage &svMsg)
{
	BOOL bUpdate = TRUE;

	if (GetExpType() == WL_EXP_VACUUM)
	{
		m_bIsGearType = FALSE;
	}
	else
	{
		m_bIsGearType = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bUpdate);
	return 1;
}


LONG CWaferLoader::ShowHistory(IPC_CServiceMessage &svMsg)
{
	CMachineStat dlg;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	CWinThread *pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	((CFlushMessageThread*)pThread)->m_pStation = dynamic_cast<CMS896AStn*>(this);

	if (pAppMod != NULL)
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	dlg.szFileName	= WL_HISTORY_NAME;
	dlg.nStartLeft	= 55;
	dlg.nStartTop	= 170;

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


LONG CWaferLoader::ReadWaferLotInfoFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	HmiMessage("Please modify s/w and call wafer table IPC command");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferLoader::GetWaferLotInfoFilePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	
	if (pAppMod != NULL)
	{
		pAppMod->GetPath(m_szLotInfoFilePath);
		SaveData();		
		bReturn = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferLoader::WL_MapCassette(IPC_CServiceMessage &svMsg)
{
	LONG	lResult, lTmp;
	INT		i, iRtn;
	CString	strTemp;
	IPC_CServiceMessage stMsg;

	((CMS896AApp*)AfxGetApp())->m_bOnBondMode = TRUE;

	lTmp = HmiMessage("Start to Map Cassette ... ", "RCMD", glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 2 * 1000);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("Start to Map Cassette ... ");		//anichia001
	if (lTmp == glHMI_CANCEL)
	{
		lResult = -1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		((CMS896AApp*)AfxGetApp())->m_bOnBondMode = FALSE;
		return TRUE;
	}

	BOOL bTemp = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bTemp);

	SetWaferTableJoystick(FALSE);

	for (i = 0; i <= WL_MAX_MAG_SLOT; i++)
	{
		m_szWfrBarcodeInCass[i] = "";
	}

	m_bMapWfrCass = TRUE;
	m_lCurrentSlotNo = 1;

	while (m_lCurrentSlotNo <= m_lTotalSlotNo)
	{
		if (MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo) == Err_FrameExistOnTrack)
		{
			lResult = 0;
			svMsg.InitMessage(sizeof(LONG), &lResult);
			m_bMapWfrCass = FALSE;
			((CMS896AApp*)AfxGetApp())->m_bOnBondMode = FALSE;
			return TRUE;
		}
		Sleep(1000);

		if (m_lCurrentSlotNo == 1)
		{
			iRtn = CheckBarcodeInMgzn(TRUE, FALSE, FALSE);
		}
		else if (m_lCurrentSlotNo >= m_lTotalSlotNo)
		{
			iRtn = CheckBarcodeInMgzn(FALSE, FALSE, TRUE);
		}
		else
		{
			iRtn = CheckBarcodeInMgzn(FALSE, FALSE, FALSE);
		}

		if (iRtn != TRUE)
		{
			lResult = 0;
			svMsg.InitMessage(sizeof(LONG), &lResult);
			m_bMapWfrCass = FALSE;
			HmiMessage("Map cassette fail!", "Map Cassette", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 3600 * 1000);
			((CMS896AApp*)AfxGetApp())->m_bOnBondMode = FALSE;
			return TRUE;
		}
		Sleep(1000);

		m_lCurrentSlotNo += (m_lSkipSlotNo + 1);

		Sleep(1000);
	}

	m_lCurrentSlotNo = 1;
	if (MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo) == Err_FrameExistOnTrack)
	{
		lResult = 0;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		m_bMapWfrCass = FALSE;
		((CMS896AApp*)AfxGetApp())->m_bOnBondMode = FALSE;
		return TRUE;
	}

	m_bMapWfrCass = FALSE;

	lResult = 1;
	svMsg.InitMessage(sizeof(LONG), &lResult);

	HmiMessage("Map cassette finished!", "Map Cassette", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 2 * 1000);

	SaveCassetteInfo();

	//CEID#8001
	SendCassetteMappedEvent_8001();

	((CMS896AApp*)AfxGetApp())->m_bOnBondMode = FALSE;
	return TRUE;
}

LONG CWaferLoader::WL_LoadUnloadCassette(IPC_CServiceMessage &svMsg)
{
	INT	i;

	//Set SECSGEM Value
	SendUnloadCassetteEvent_8003();

	//Set SECSGEM Value
	SendLoadCassetteEvent_8002();

	for (i = 0; i <= WL_MAX_MAG_SLOT; i++)
	{
		m_szWfrBarcodeInCass[i] = "";
	}
	SaveCassetteInfo();

	HmiMessage("Wafer cassette re-load!", "Unload/Load Cassette", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	return TRUE;
}

BOOL CWaferLoader::SaveCassetteInfo()
{
	CStringMapFile	WaferLotInfo;
	INT		i;

	DeleteFile(MSD_WAFER_LOT_INFO_FILE);
	DeleteFile(MSD_WAFER_LOT_INFO_BKF);

	if (WaferLotInfo.Open(MSD_WAFER_LOT_INFO_FILE, FALSE, TRUE) != 1)
	{
		HmiMessage("File not found!\nPlease check", "Load File Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	// Definition File
	(WaferLotInfo)[WL_LOT_HEADER_A_DEFFILE] = "";//"Wafer Cassette";

	// Number of wafer
	(WaferLotInfo)[WL_LOT_HEADER_A_WAFER_NO] = WL_MAX_MAG_SLOT;

	// Save wafer name
	for (i = 1; i <= WL_MAX_MAG_SLOT; i++)
	{
		(WaferLotInfo)[WL_LOT_WAFER_NO][i] = m_szWfrBarcodeInCass[i];
	}

	// close config file
	WaferLotInfo.Update();
	WaferLotInfo.Close();

	return TRUE;
}

LONG CWaferLoader::WL_GetCassCurSlot()
{
	return m_lCurrentSlotNo;
}

LONG CWaferLoader::WL_GetCassInfo(IPC_CServiceMessage &svMsg)
{
	LONG	lSlotNo;

	lSlotNo = m_lCurrentSlotNo;
	svMsg.InitMessage(sizeof(LONG), &lSlotNo);
	return TRUE;
}


LONG CWaferLoader::IsExpanderSafeToMove(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = IsWL1ExpanderSafeToMove();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferLoader::IsExpander2SafeToMove(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = IsWL2ExpanderSafeToMove();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferLoader::CheckExpanderSafeToMove(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableWL)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ((GetExpType() != WL_EXP_VACUUM) && (GetExpType() != WL_EXP_NONE))
	{
		INT nStatus = CheckExpander();
		if (nStatus == Err_ExpanderLockFailed)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander is locked");
			SetErrorMessage("Expander is locked");

			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);
			bReturn = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferLoader::ResetEjectorCapCount(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;


	if (HmiMessage("Reset ejector cap clean count?", "Ejector Clean Cap", 
				   glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		m_lEjrCapCleanCount = 0;
		SaveData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferLoader::Test(IPC_CServiceMessage &svMsg)
{
	//	426TX	1	move to auto align button

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferLoader::FindWaferAngleFirstButton(IPC_CServiceMessage &svMsg)	// for cp100
{
	BOOL bReturn = TRUE;
	BOOL	bFindGT = (BOOL)(LONG)((*m_psmfSRam)["MS896A"]["FindGlobalTheta"]);
	if( bFindGT )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Find Wafer Angle start");
		SetWaferTableJoystick(FALSE);
		StartLoadingAlert();

		LONG	lCurrentX = 0, lCurrentY = 0;
		GetHomeDiePhyPosn(lCurrentX, lCurrentY);

		MoveWaferTable(lCurrentX, lCurrentY);
		Sleep(50);

		if (CorrectWaferAngle(&lCurrentX, &lCurrentY) == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Cannot correct wafer angle");
			SetErrorMessage("Cannot correct wafer angle");

			SetAlert_Red_Yellow(IDS_WL_CORRECT_ANGLE_FAIL);
			bReturn = FALSE;
		}

		CloseLoadingAlert();
		SetWaferTableJoystick(TRUE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::FindWaferAngle(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;		//v2.60

	SetWaferTableJoystick(FALSE);
	/*********/ 
	StartLoadingAlert(); /*********/

	if (IsWL1ExpanderSafeToMove() == FALSE || IsWL2ExpanderSafeToMove() == FALSE)
	{
		bReturn = TRUE;	
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsBLInUse())	//	4.24TX 3
	{
		if (MoveBackLightToSafeLevel() == FALSE)
		{
			bReturn = TRUE;				//v2.64
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (MoveEjectorElevatorToSafeLevel() == FALSE)
		{
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}
	IPC_CServiceMessage stMsg;

	int		nConvID = 0;
	LONG	lCurrentX = 0;
	LONG	lCurrentY = 0;

	if (IsWT2InUse())
	{
		GetWT2HomeDiePhyPosn(lCurrentX, lCurrentY);
	}
	else
	{
		GetHomeDiePhyPosn(lCurrentX, lCurrentY);
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanBarWafer() || IsScnLoaded() )
	{
		LONG lT = 0;
		if( IsOcrAOIMode() )
		{
			if( IsCharDieInUse() )
			{
				GetWaferTableEncoder(&lCurrentX, &lCurrentY, &lT);
			}
		}
		else
		{
			CString szLogPath = "";
			pUtl->GetPrescanLogPath(szLogPath);
			CString szOldOcrPath	= szLogPath + _T(".OCR");	//	find GT at current position
			if (_access(szOldOcrPath, 0) != -1)
			{
				GetWaferTableEncoder(&lCurrentX, &lCurrentY, &lT);
			}
		}
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("Find Wafer Angle start");

	MoveWaferTable(lCurrentX, lCurrentY);
	Sleep(50);

	if (IsESMachine())
	{
		WPR_MoveToFocusLevel();
		BOOL bWT2 = FALSE;
		if (IsWT1UnderCamera() || IsWT2UnderCamera())
		{
			if( IsWT2UnderCamera() )
			{
				bWT2 = TRUE;
			}
			MoveBackLightToUpLevel();
		}
		RotateWFTTUnderCam( bWT2 );
	}

	if (CorrectWaferAngle(&lCurrentX, &lCurrentY) == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Cannot correct wafer angle");
		SetErrorMessage("Cannot correct wafer angle");

		SetAlert_Red_Yellow(IDS_WL_CORRECT_ANGLE_FAIL);
		bReturn = FALSE;
	}

	if (IsBLInUse())
	{
		if (MoveBackLightToSafeLevel() == FALSE)
		{
			bReturn = TRUE;				//v2.64
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	if (IsEjtElvtInUse())
	{
		if (MoveEjectorElevatorToSafeLevel() == FALSE)
		{
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}	//	DOWN BACK LIGH AND EJECTOR ELEVATOR

	/*********/ CloseLoadingAlert(); /*********/
	SetWaferTableJoystick(TRUE);

	bReturn = TRUE;				//v2.64
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferLoader::AutoRegionAlignWafer(IPC_CServiceMessage &svMsg)	// this for region prescan only after frame loaded
{
	LONG lAlnStatus = 1;
	BOOL bReturn = FALSE;
	IPC_CServiceMessage stMsg;
	int	nConvID = 0;

	m_dStartWaferAlignTime = GetTime();	// auto region align

	m_bWaferAlignComplete = FALSE;
	CMSLogFileUtility::Instance()->WL_LogStatus("Auto region align wafer by ms_common");
	//Request switch camera to wafer side
	BOOL bCamera = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bCamera);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SwitchToTarget", stMsg);

	LONG lBlkAln = -1;
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoRegionAlignBlock", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 360000000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(LONG), &lBlkAln);
			break;
		}
		else
		{
			Sleep(1);		
		}
	}

	switch (lBlkAln)
	{
		case -1:	// not block
			break;
		case 1:		// alignment ok
			lAlnStatus = 1;
			svMsg.InitMessage(sizeof(LONG), &lAlnStatus);
			m_bWaferAlignComplete = TRUE;
			return 1;
			break;
		case 0:		// alignment fail
		default:
			lAlnStatus = 0;
			svMsg.InitMessage(sizeof(LONG), &lAlnStatus);
			m_bWaferAlignComplete = TRUE;
			return 1;
			break;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	pUtl->RegionOrderLog("REGN WL to do region align begin");

	//Find HOME die on wafer
	Sleep(200);
	if (FindWaferGlobalAngle() == FALSE)
	{
		lAlnStatus = 0;
		svMsg.InitMessage(sizeof(LONG), &lAlnStatus);
		pUtl->RegionOrderLog("REGN find home die failure");
		m_bWaferAlignComplete = TRUE;
		return 1;
	}

	if (AutoLearnAdaptWafer() == FALSE)
	{
		lAlnStatus = 0;
		svMsg.InitMessage(sizeof(LONG), &lAlnStatus);
		pUtl->RegionOrderLog("REGN find home die failure");
		m_bWaferAlignComplete = TRUE;
		return 1;
	}

	if (FindHomeDie() == FALSE)	// in fully auto mode to align region sort wafer
	{
		lAlnStatus = 0;
		svMsg.InitMessage(sizeof(LONG), &lAlnStatus);
		pUtl->RegionOrderLog("REGN find home die failure");
		m_bWaferAlignComplete = TRUE;
		return 1;
	}
	pUtl->RegionOrderLog("REGN find home die success");

	bReturn = AlignInputWafer();

	if (bReturn == FALSE)
	{
		SetErrorMessage("Align wafer failed\n");
		SetAlert_Red_Yellow(IDS_WL_ALIGNWAF_FAIL);
		lAlnStatus = 0;
		pUtl->RegionOrderLog("REGN align input wafer failed");
	}
	else
	{
		pUtl->RegionOrderLog("REGN align input wafer finish");
	}

	svMsg.InitMessage(sizeof(LONG), &lAlnStatus);
	m_bWaferAlignComplete = TRUE;
	return TRUE;
}

BOOL CWaferLoader::ResetFullMagazine(IPC_CServiceMessage &svMsg)
{
	if( IsESMachine() )
	{
		BOOL bReturn = TRUE;
		LONG lSelection = 0;
		CStringList szSelection;
		szSelection.AddTail("WL 1 magazine");

		LONG lResult = HmiSelection("Please select target magazeine", "Reset Magazines", szSelection, lSelection);
		if (lResult < 0)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		OpenWaitingAlert();
		if (lResult == 0 || lResult==2 )
		{
			m_bIsMagazineFull = FALSE;
			m_lCurrentMagNo	= 1;
			m_lCurrentSlotNo = 1;
			MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
			Z_Home();
			MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
		}

		if( lResult==1 || lResult==2 )
		{
			m_lCurrentMagNo2	= 1;
			m_lCurrentSlotNo2	= 1;
			m_bIsMagazine2Full	= FALSE;
			
			MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2);
			Z2_Home();
			MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2);
		}
		
		SaveData();

		if (IsESDualWT())
		{
			SetWT2InUse(FALSE);
			IPC_CServiceMessage stMsg;
			INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "UpdateData", stMsg);
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

		CloseWaitingAlert();

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	CString szContent;
	BOOL bReturn = TRUE;

	szContent.LoadString(HMB_WL_RESET_ALL_MAGAZINE);

	if (HmiMessage(szContent, "", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	OpenWaitingAlert();

	m_bIsMagazineFull = FALSE;
	m_lCurrentMagNo	= 1;
	m_lCurrentSlotNo = 1;
	
	MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
	Z_Home();
	MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
	if (m_bUseDualTablesOption == TRUE)
	{
		if (IsES201() == FALSE)
		{
			m_lCurrentMagNo2	= 1;
			m_lCurrentSlotNo2	= 1;
			m_bIsMagazine2Full	= FALSE;
			
			MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2);
			Z2_Home();
			MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2);
		}
	}

	SaveData();

	if (IsESDualWT())
	{
		SetWT2InUse(FALSE);
		IPC_CServiceMessage stMsg;
		INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "UpdateData", stMsg);
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

	CloseWaitingAlert();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

BOOL CWaferLoader::ResetXMagazine(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if( !m_bUseMagazine3AsX )		// reset x magazine button
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	LONG lSelection = 0;
	CStringList szSelection;
	szSelection.AddTail("WL 1 X magazine");

	LONG lResult = HmiSelection("Please select target magazeine", "Reset X Magazine", szSelection, lSelection);
	if (lResult < 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (lResult==0 ||lResult==2  )
	{
		m_lCurrentMagazineX1SlotNo = 1;
	}

	if( lResult==1 || lResult==2 )
	{
		m_lCurrentMagazineX2SlotNo = 1;
	}

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

BOOL CWaferLoader::ResetGMagazine(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if( !m_bUseAMagazineAsG )		// reset x magazine button
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	LONG lSelection = 0;
	CStringList szSelection;
	szSelection.AddTail("WL 1 G magazine");

	LONG lResult = HmiSelection("Please select target magazeine", "Reset G Magazine", szSelection, lSelection);
	if (lResult < 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (lResult==0 ||lResult==2  )
	{
		m_lMagazineG1CurrentSlotNo = 1;
	}

	if( lResult==1 || lResult==2 )
	{
		m_lMagazineG2CurrentSlotNo = 1;
	}

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}	//	Reset Magazine G

BOOL CWaferLoader::IsWT1UnderCamera()
{
	LONG lX, lY, lT;

	GetES101WTEncoder(&lX, &lY, &lT, FALSE);

	return IsWithinWT1WaferLimit(lX, lY);
}

BOOL CWaferLoader::IsWT2UnderCamera()
{
	if (IsESDualWT() == FALSE)
	{
		return FALSE;
	}

	LONG lX, lY, lT;

	GetES101WTEncoder(&lX, &lY, &lT, TRUE);

	return IsWithinWT2WaferLimit(lX, lY);
}

BOOL CWaferLoader::IsWT1UnderEjectorPos()
{
	LONG lX, lY, lT;

	GetES101WTEncoder(&lX, &lY, &lT, FALSE);

	return IsWithinWT1WaferLimit(lX, lY, 1, TRUE);
}

BOOL CWaferLoader::IsWT2UnderEjectorPos()
{
	LONG lX, lY, lT;

	GetES101WTEncoder(&lX, &lY, &lT, TRUE);

	return IsWithinWT2WaferLimit(lX, lY, 1, TRUE);
}


LONG CWaferLoader::SetMS902ndHomePosn(IPC_CServiceMessage &svMsg)
{
	if( !m_fHardware || m_bDisableWL || IsWL1ExpanderSafeToMove()==FALSE )
	{	
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);

	MoveWaferTable(m_lWT2HomeDiePhyPosX, m_lWT2HomeDiePhyPosY);	// for MS90 half sort second part

	SetJoystickLimit(TRUE);
	SetWaferTableJoystick(TRUE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferLoader::ConfirmMS902ndHomePosn(IPC_CServiceMessage &svMsg)
{
	if( !m_fHardware || m_bDisableWL )
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetWaferTableJoystick(FALSE);

	LONG lT = 0;
	GetES101WTEncoder(&m_lWT2HomeDiePhyPosX, &m_lWT2HomeDiePhyPosY, &lT);	// for MS90 half sort second part

	SetJoystickLimit(FALSE);
	SetWaferTableJoystick(TRUE);

	SaveData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CWaferLoader::App_AutoChangeFilmFrame(IPC_CServiceMessage &svMsg)
{
	BOOL bBurnIn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBurnIn);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if( IsWLManualMode() && (!bBurnIn) && (GetExpType() != WL_EXP_NONE) &&
		!IsESMachine() && (pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="WH") )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL not fully auto mode, open expander");
		BOOL bOpen = TRUE;
		if( m_bIsGearType )
		{
			OpenCloseExpander(bOpen);
		}
		else
		{
			UpDownFrameLevel(bOpen);
		}

		m_bExpanderStatus = TRUE;
		CMSLogFileUtility::Instance()->WL_LogStatus("WL Filmframe is opened");

		LONG	lResult = -1;
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	return AutoChangeFilmFrame(svMsg);
}

LONG CWaferLoader::App_OpenCloseExpander(IPC_CServiceMessage &svMsg)
{
	BOOL bOpen = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOpen);

	BOOL bReturn = FALSE;
	if( m_bIsGearType )
	{
		bReturn = OpenCloseExpander(bOpen);
	}
	else
	{
		bReturn = UpDownFrameLevel(bOpen);
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="WH" )
		;
	else
		bReturn = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferLoader::ResetSingleWaferSlotInfoCmd(IPC_CServiceMessage &svMsg)
{
	LONG lSlot;
	svMsg.GetMsg(sizeof(LONG), &lSlot);

	m_stWaferMagazine[0].m_lSlotUsage[lSlot - 1] = WL_SLOT_USAGE_EMPTY;
	m_stWaferMagazine[0].m_lSlotWIPCounter[lSlot - 1] = 0;
	m_stWaferMagazine[0].m_SlotBCName[lSlot - 1] = _T("");
	SECS_UpdateCassetteSlotInfo(lSlot);

	//Song
	SaveData();
	SendResetSlotEvent_7018(lSlot - 1);
	DisplayWaferSlotInfo(svMsg);
	HmiMessage("Reset Single Wafer Slot Info Finished");
	return TRUE;
}


LONG CWaferLoader::ResetWaferSlotInfoCmd(IPC_CServiceMessage &svMsg)
{
	for (LONG j = 0; j < MS_WL_AUTO_LINE_MGZN_SLOT; j++)
	{
		m_stWaferMagazine[0].m_lSlotUsage[j] = WL_SLOT_USAGE_EMPTY;
		m_stWaferMagazine[0].m_lSlotWIPCounter[j] = 0;
		m_stWaferMagazine[0].m_SlotBCName[j] = _T("");
		SECS_UpdateCassetteSlotInfo(j + 1);
	}
	SaveData();
	SendResetSlotEvent_7018(0, TRUE);
	DisplayWaferSlotInfo(svMsg);
	HmiMessage("Reset Wafer Slot Info Finished");
	return TRUE;
}

LONG CWaferLoader::DisplayWaferSlotInfo(IPC_CServiceMessage &svMsg)
{
    GenerateOMRTTableFile(0);
    return TRUE;
}
