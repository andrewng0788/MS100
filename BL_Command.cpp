///////////////////////////////////////////////////////////////////////
// BL_Command.cpp : HMI Registered Command of the CBinrLoader class
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
#include "BinLoader.h"
#include "HmiDataManager.h"
#include "BT_CmdName.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "MachineStat.h"
#include "LogFileUtil.h"
#include "SC_Constant.h"
#include "CTmpChange.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Registered HMI Command 
VOID CBinLoader::RegisterVariables()
{
	try
	{
        CString str;
        
		// Register Variables
		RegVariable(_T("BL_bIsEnabled"),			&m_bIsEnabled);
		RegVariable(_T("BL_bIsExChgArmExist"),		&m_bIsExChgArmExist);
		RegVariable(_T("BL_bIsDualBufferExist"),    &m_bIsDualBufferExist);
		
		RegVariable(_T("BL_lEnc_X"),				&m_lEnc_X);
		RegVariable(_T("BL_lEnc_X2"),				&m_lEnc_X2);
		RegVariable(_T("BL_lEnc_Y"),				&m_lEnc_Y);
		RegVariable(_T("BL_lEnc_Z"),				&m_lEnc_Z);
		RegVariable(_T("BL_lEnc_Arm"),				&m_lEnc_Arm);
		RegVariable(_T("BL_lEnc_T"),				&m_lEnc_T);

		RegVariable(_T("BL_bHomeSnr_X"),			&m_bHomeSnr_X);
		RegVariable(_T("BL_bHomeSnr_X2"),			&m_bHomeSnr_X2);
		RegVariable(_T("BL_bULimitSnr_X"),			&m_bULimitSnr_X);
		RegVariable(_T("BL_bLLimitSnr_X"),			&m_bLLimitSnr_X);

		RegVariable(_T("BL_bHomeSnr_Y"),			&m_bHomeSnr_Y);
		RegVariable(_T("BL_bULimitSnr_Y"),			&m_bULimitSnr_Y);
		RegVariable(_T("BL_bLLimitSnr_Y"),			&m_bLLimitSnr_Y);

		RegVariable(_T("BL_bHomeSnr_Z"),			&m_bHomeSnr_Z);
		RegVariable(_T("BL_bULimitSnr_Z"),			&m_bULimitSnr_Z);
		RegVariable(_T("BL_bLLimitSnr_Z"),			&m_bLLimitSnr_Z);

		RegVariable(_T("BL_bHomeSnr_Arm"),			&m_bHomeSnr_Arm);

		//=====================================================================
		//2018.1.3 add new HMI variable
//		RegVariable(_T("BL_bHomeSnr_Upper"),		&m_bHomeSnr_Upper);
//		RegVariable(_T("BL_bHomeSnr_Lower"),		&m_bHomeSnr_Lower);
		RegVariable(_T("BL_bHomeSnr_Theta"),		&m_bHomeSnr_Theta);
		//=====================================================================

		RegVariable(_T("BL_bFrameDetect"),			&m_bFrameInClampDetect);	
		RegVariable(_T("BL_bFrameJam"),				&m_bFrameJam);
		RegVariable(_T("BL_bFramePosition"),		&m_bFrameOnBTPosition);	
//		RegVariable(_T("BL_bFrameProtection"),		&m_bFrameProtection);	
		RegVariable(_T("BL_bFrameInMagazine"),		&m_bFrameInMagazine);	
		//RegVariable(_T("BL_bBinFrameDetect"),		&m_bFrameOnBTDetect);

		RegVariable(_T("BL_bBufferSafetySensor"),			&m_bBufferSafetySensor_HMI);
		RegVariable(_T("BL_bElevatorDoorCover"),			&m_bElevatorDoorCover_HMI);
		RegVariable(_T("BL_bFrontRightElevatorDoorCover"),	&m_bFrontRightElevatorDoorCover_HMI);
 
		RegVariable(_T("BL_bFrameOutOfMgz"),		&m_bFrameOutOfMgz);
		RegVariable(_T("BL_bFrameLevel"),			&m_bFrameLevel);

		RegVariable(_T("BL_bFrameVacuum"),			&m_bFrameVacuum);
		RegVariable(_T("BL_bPressureSensor"),		&m_bPressureSensor);
		//MS100 9Inch DualTable attributes
		RegVariable(_T("BL_bFrameDetect2"),			&m_bFrameInClampDetect2);	
//		RegVariable(_T("BL_bFramePosition2"),		&m_bFrameOnBTPosition2);	
//		RegVariable(_T("BL_bFrameProtection2"),		&m_bFrameProtection2);	
		RegVariable(_T("BL_bFrameJam2"),			&m_bFrameJam2);	
//		RegVariable(_T("BL_bFrameLevel2"),			&m_bFrameLevel2);
//		RegVariable(_T("BL_bFrameVacuum2"),			&m_bFrameVacuum2);
//		RegVariable(_T("BL_bMagazine2Exist"),		&m_bMagazine2Exist);
		RegVariable(_T("BL_bMS90PusherZ"),			&m_bMS90PusherZ);
		RegVariable(_T("BL_bMS90PusherX"),			&m_bMS90PusherX);
		RegVariable(_T("BL_bMS90PusherLimitX"),		&m_bMS90PusherLimitX);

		//Multi-magazine sensors
		for (INT i=0; i<MS_BL_MGZN_NUM; i++)
		{
			str.Format("BL_bMagazineExist%d", i);
			RegVariable(_T(str), &m_bMagazineExist[i]);
		}
/*
		//MS50 support 8 magazines
		RegVariable(_T("BL_bMagazineExist0"),	&m_bMagazineExist[0]);
		RegVariable(_T("BL_bMagazineExist1"),	&m_bMagazineExist[1]);
		RegVariable(_T("BL_bMagazineExist2"),	&m_bMagazineExist[2]);
		RegVariable(_T("BL_bMagazineExist3"),	&m_bMagazineExist[3]);
		RegVariable(_T("BL_bMagazineExist4"),	&m_bMagazineExist[4]);
		RegVariable(_T("BL_bMagazineExist5"),	&m_bMagazineExist[5]);
		RegVariable(_T("BL_bMagazineExist6"),	&m_bMagazineExist[6]);
		RegVariable(_T("BL_bMagazineExist7"),	&m_bMagazineExist[7]);
*/
		RegVariable(_T("BL_bFrontArmFrameExist"),		&m_bFrontArmFrameExist);
		RegVariable(_T("BL_bRearArmFrameExist"),		&m_bRearArmFrameExist);
		RegVariable(_T("BL_bFrontArmReady"),			&m_bFrontArmReady);
		RegVariable(_T("BL_bRearArmReady"),				&m_bRearArmReady);
		RegVariable(_T("BL_bBufferFrameExist"),			&m_bBufferFrameExist);
		
		//Dual Buffer Table Sensors
		RegVariable(_T("BL_bDualBufferUpperExist"),		&m_bDualBufferUpperExist);
		RegVariable(_T("BL_bDualBufferLowerExist"),		&m_bDualBufferLowerExist);
		RegVariable(_T("BL_bDualBufferRightProtect"),	&m_bDualBufferRightProtect);
		RegVariable(_T("BL_bDualBufferLeftProtect"),	&m_bDualBufferLeftProtect);
		RegVariable(_T("BL_bDualBufferLevel"),			&m_bDualBufferLevel);

		RegVariable(_T("BL_bSemitekBLMode"),			&m_bSemitekBLMode);		//v4.53A25

        RegVariable(_T("BL_bUseBarcode"),				&m_bUseBarcode);
        RegVariable(_T("BL_bUseExtension"),				&m_bUseExtension);
		RegVariable(_T("BL_bCheckBarcode"),				&m_bCheckBarcode);
		RegVariable(_T("BL_bCheckBarcodeSkipEmpty"),	&m_bCheckBCSkipEmpty);
		RegVariable(_T("BL_bCompareBarcode"),			&m_bCompareBarcode);
        RegVariable(_T("BL_bStopChgGradeScan"),			&m_bStopChgGradeScan);
		RegVariable(_T("BL_lScanRange"),				&m_lScanRange);
        RegVariable(_T("BL_lRetryCounter"),				&m_lTryLimits);
        RegVariable(_T("BL_szExtensionName"),			&m_szExtName);
		RegVariable(_T("BL_ucBarcodeModel"),			&m_ucBarcodeModel);
		RegVariable(_T("BL_bWaferToBinResort"),			&m_bWaferToBinResort);
		RegVariable(_T("BL_bEnableMS90TablePusher"),	&m_bEnableMS90TablePusher);		//v4.59A40

		RegVariable(_T("BH_lTemperature"),				&m_lTemperature);

		RegVariable(_T("BL_szBinBarcodePrefix"),		&m_szBinBarcodePrefix);
		RegVariable(_T("BL_lBinBarcodeLength"),			&m_lBinBarcodeLength);

		RegVariable(_T("BL_lBLGeneral_1"),				&m_lBLGeneral_1);
        RegVariable(_T("BL_lBLGeneral_2"),				&m_lBLGeneral_2);
        RegVariable(_T("BL_lBLGeneral_3"),				&m_lBLGeneral_3);
        RegVariable(_T("BL_lBLGeneral_4"),				&m_lBLGeneral_4);
        RegVariable(_T("BL_lBLGeneral_5"),				&m_lBLGeneral_5);
        RegVariable(_T("BL_lBLGeneral_6"),				&m_lBLGeneral_6);
        RegVariable(_T("BL_lBLGeneral_7"),				&m_lBLGeneral_7);
		RegVariable(_T("BL_lBLGeneral_8"),				&m_lBLGeneral_8);
		RegVariable(_T("BL_lBLGeneral_9"),				&m_lBLGeneral_9);		//BT Barcode Pos X
		RegVariable(_T("BL_lBLGeneral_10"),				&m_lBLGeneral_10);		//BT Barcode Pos Y
		RegVariable(_T("BL_lBLGeneral_11"),				&m_lBLGeneral_11);
		RegVariable(_T("BL_lBLGeneral_12"),				&m_lBLGeneral_12);
		RegVariable(_T("BL_lBLGeneral_13"),				&m_lBLGeneral_13);
		RegVariable(_T("BL_lBLGeneral_14"),				&m_lBLGeneral_14);
		RegVariable(_T("BL_lBLGeneral_15"),				&m_lBLGeneral_15);
        RegVariable(_T("BL_lBLGeneral_TmpA"),			&m_lBLGeneral_TmpA);
        RegVariable(_T("BL_lSelMagazineID"),			&m_lSelMagazineID);
        RegVariable(_T("BL_dSlotPitch"),				&m_dSlotPitch);

		RegVariable(_T("BL_lAutoLineUnloadZ"),			&m_lAutoLineUnloadZ);	//v4.56A1

        // operation mode setup
        for(INT i=0; i<MS_BL_MGZN_NUM; i++)
        {
            str.Format("BL_OM_szMgznUse%d", i+1);
            RegVariable(str, &m_szBLMgznUse[i]);
        }

		RegVariable(_T("BL_bOut8MagConfig"),			&m_bBLOut8MagConfig);		//v3.82		//MS100 8mag config option
		RegVariable(_T("BL_OM_szOMSP"),					&m_szOMSP);					//v3.82
		RegVariable(_T("BL_OM_szSlotUsage"),			&m_szBLSlotUsage);
        RegVariable(_T("BL_OM_lMgznSelect"),			&m_ulMgznSelected);
        RegVariable(_T("BL_OM_lSlotSelect"),			&m_ulSlotSelected);
        RegVariable(_T("BL_OM_lSlotBlock"),				&m_ulSlotPhyBlock);
        RegVariable(_T("BL_lRTOperationMode"),			&m_lOMRT);
		RegVariable(_T("BL_lRTCurrMgzn"),				&m_lCurrMgzn);
		RegVariable(_T("BL_lRTCurrMgzn1"),				&m_lCurrHmiMgzn);
        //RegVariable(_T("BL_lRTCurrSlot"),				&m_lCurrSlot);
		RegVariable(_T("BL_lRTCurrSlot"),				&m_lCurrHmiSlot);
        RegVariable(_T("BL_lRTCurrBlock"),				&m_lBTCurrentBlock);
		RegVariable(_T("BL_szCurrMgznDisplayName"),		&m_szCurrMgznDisplayName);
		//MS100 9Inch dual-table config
		RegVariable(_T("BL_lRTCurrMgzn2"),				&m_lCurrMgzn2);
		RegVariable(_T("BL_lRTCurrSlot2"),				&m_lCurrHmiSlot2);
        RegVariable(_T("BL_lRTCurrBlock2"),				&m_lBTCurrentBlock2);
		RegVariable(_T("BL_szCurrMgznDisplayName2"),	&m_szCurrMgznDisplayName2);

        RegVariable(_T("BL_bMotionFail"),				&m_bMotionFail);
        RegVariable(_T("BL_bUseBinTableVacuum"),		&m_bUseBinTableVacuum);
		RegVariable(_T("BL_bRealignBinFrame"),			&m_bRealignBinFrame);
		
		RegVariable(_T("BL_bBurnInTestRun"),			&m_bBurnInTestRun);
        RegVariable(_T("BL_bNoSensorCheck"),			&m_bNoSensorCheck);
        RegVariable(_T("BL_bCheckElevatorCover"),		&m_bCheckElevatorCover);
        RegVariable(_T("BL_bCheckFrameIsAligned"),		&m_bCheckFrameIsAligned);
        RegVariable(_T("BL_bHomeGripper"),				&m_bHomeGripper);

		RegVariable(_T("BL_bSetSlotBlockEnable"),		&m_bSetSlotBlockEnable);

		//MS50_01
        RegVariable(_T("BL_lUnloadOffset"),				&m_lUnloadOffset);
        RegVariable(_T("BL_lUpperToLowerBufferOffsetZ"),	&m_lUpperToLowerBufferOffsetZ);
		RegVariable(_T("BL_lUseLBufferGripperForSetup"),	&m_lUseLBufferGripperForSetup);

        RegVariable(_T("BL_lUnloadOffsetX"),			&m_lUnloadOffsetX);					
		RegVariable(_T("BL_lUnloadOffsetY"),			&m_lUnloadOffsetY);
        RegVariable(_T("BL_lUnloadPusherOffsetX"),		&m_lUnloadPusherOffsetX);			//v2.93T2
		RegVariable(_T("BL_bGripperPusher"),			&m_bGripperPusher);					//v2.93T2
		RegVariable(_T("BL_lLoadSearchInClampOffsetX"),	&m_lLoadSearchInClampOffsetX);		//v3.59
		//2018.4.18
		RegVariable(_T("BL_lSIS_Top1LoadClampOffsetX"),	&m_lSIS_Top1MagClampOffsetX);
		RegVariable(_T("BL_lTop2LoadClampOffsetX"),		&m_lTop2MagClampOffsetX);

		RegVariable(_T("BL_lSIS_Top1LoadClampOffsetX2"),	&m_lSIS_Top1MagClampOffsetX2);
		RegVariable(_T("BL_lTop2LoadClampOffsetX2"),		&m_lTop2MagClampOffsetX2);

		RegVariable(_T("BL_lBTExArmOffsetX"),			&m_lBTExArmOffsetX);				//v3.74zT44
		RegVariable(_T("BL_lBTExArmOffsetY"),			&m_lBTExArmOffsetY);				//v3.74zT44

		RegVariable(_T("BL_bEnableMagzFullFile"),		&m_bEnableMagzFullFile);
		RegVariable(_T("BL_bExChgFullMgznOutputFile"),	&m_bExChgFullMgznOutputFile);		//v4.15T9
		RegVariable(_T("BL_szMagzFullFilePath"),		&m_szMagzFullFilePath);
		RegVariable(_T("BL_szCreeBinBcSummaryPath"),	&m_szCreeBinBcSummaryPath);			//v3.75

		RegVariable(_T("BL_ulMinClearAllCount"),		&m_ulMinClearAllCount);
		RegVariable(_T("BL_lClearAllFrameMode"),		&m_lClearAllFrameMode);

		RegVariable(_T("BL_bExArmBarcodeOnGripper"),&m_bExArmBarcodeOnGripper);
		RegVariable(_T("BL_lExArmPickDelay"),		&m_lExArmPickDelay);
		RegVariable(_T("BL_lExArmVacuumDelay"),		&m_lExArmVacuumDelay);
        RegVariable(_T("BL_lExArmBufferBlock"),		&m_lExArmBufferBlock);
		RegVariable(_T("BL_lExArmUpDelay"),			&m_lExArmUpDelay);

		RegVariable(_T("BL_lBTAlignFrameCount"),	&m_lBTAlignFrameCount);
		RegVariable(_T("BL_lBTAlignFrameDelay"),	&m_lBTAlignFrameDelay);
		RegVariable(_T("BL_lBTLoadOffset_Y"),		&m_lBTLoadOffset_Y);					//v3.79

		RegVariable(_T("BL_lBUTAlignFrameCount"),	&m_lBUTAlignFrameCount);
		RegVariable(_T("BL_lBUTAlignFrameDelay"),	&m_lBUTAlignFrameDelay);

		RegVariable(_T("BL_lBTVacuumDelay"),		&m_lBTVacuumDelay);
		RegVariable(_T("BL_lBT_FrameUpBlowTime"),	&m_lBT_FrameUpBlowTime);
		RegVariable(_T("BL_lVacSettleDelay"),		&m_lVacSettleDelay);		//v3.67T1	//Semitek

		RegVariable(_T("BL_bNewZConfig"),			&m_bNewZConfig);
		RegVariable(_T("BL_bFastBcScanMethod"),		&m_bFastBcScanMethod);

		RegVariable(_T("BL_bGenBinFullFile"),		&m_bGenBinFullFile);
		//RegVariable(_T("BL_bDebugCheckSensor"),		&m_bDebugCheckSensor);		//v3.35	//Cree

		RegVariable(_T("BL_bDisplaySingleMgznSnr"),	&m_bDisplaySingleMgznSnr);	//v3.30T2	//Display single magazine sensor in HMI
        RegVariable(_T("BL_bDisplayMultiMgznSnrs"),	&m_bDisplayMultiMgznSnrs);	//v3.30T2	//Display single magazine sensor in HMI

		RegVariable(_T("BL_szBinFrameBarcode"),		&m_szBinFrameBarcode);		//v3.30T2	//Display single magazine sensor in HMI
		RegVariable(_T("BL_szBinFrame2Barcode"),	&m_szBinFrame2Barcode);		//v4.34		//Display BT2 frame BC
		RegVariable(_T("BL_szBufferFrameBarcode"),	&m_szBufferFrameBarcode);	//v3.30T2	//Display single magazine sensor in HMI

		//Dual Buffer
		RegVariable(_T("BL_lDualBufferUpperBlock"),	&m_clUpperGripperBuffer.m_lBufferBlock);
		RegVariable(_T("BL_lDualBufferLowerBlock"),	&m_clLowerGripperBuffer.m_lBufferBlock);
		RegVariable(_T("BL_lTestBufferBlock"),		&m_lTestBufferBlock);
		RegVariable(_T("BL_lTestTableBlock"),		&m_lTestTableBlock);
		RegVariable(_T("BL_szUBufferBarcode"),		&m_clUpperGripperBuffer.m_szBufferBarcode);
		RegVariable(_T("BL_szLBufferBarcode"),		&m_clLowerGripperBuffer.m_szBufferBarcode);
		RegVariable(_T("BL_lDualBufferTestMgzn"),	&m_lDualBufferTestMgzn);
		
		//Dual Tables
		RegVariable(_T("BL_bDualDLPreloadEmpty"),	&m_bDualDLPreloadEmpty);	//v4.48A15	//Freescale

		RegVariable(_T("BL_lFrameLevelTestCount"),	&m_lFrameLevelTestCount);	//v3.70T4
		RegVariable(_T("BL_lTestBlkID"),			&m_lTestBlkID);				//v3.71T1

		RegVariable(_T("BL_bEnableBLMotionLog"),	&m_bEnableBLMotionLog);
		RegVariable(_T("BL_lUnloadToFullTimeSpan"),	&m_lUnloadToFullTimeSpan);	//v4.60A2

		RegVariable(_T("BL_bFrontGateSensor"),		&m_bFrontGateSensor);
		RegVariable(_T("BL_bBackGateSensor"),		&m_bBackGateSensor);

		//For Generate Empty Frame Text File, GenerateEmptyFrameTextFile()
		RegVariable(_T("BL_szEmptyFramePath"),		&m_szEmptyFramePath);

		//Clear Bin Selected
		CString szRegMsg;
		for(int i=1; i<=200; i++)
		{
			szRegMsg.Format("BL_bClearBin%03d", i);
			RegVariable(_T(szRegMsg), &m_bClearBinHmi[i]);
		}

		RegVariable(_T("BL_lLoopTestCounter"),			&m_lLoopTestCounter);
		DisplayMessage("BinLoaderStn variables Registered...");


		// Register Commands
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OsramBinRealignCheck"),	&CBinLoader::OsramBinRealignCheck);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckBinLoaderSetup"),	&CBinLoader::CheckBinLoaderSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PreBondChecking"),		&CBinLoader::PreBondChecking);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),			&CBinLoader::UpdateAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogItems"),				&CBinLoader::LogItems);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GripperState"),			&CBinLoader::GripperState);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GripperLevel"),			&CBinLoader::GripperLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GripperPusher"),		&CBinLoader::GripperPusher);		//v2.93T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrameVacuum"),			&CBinLoader::FrameVacuum);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrameLevel"),			&CBinLoader::FrameLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetFrameLevel"),		&CBinLoader::GetFrameLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrameAlign"),			&CBinLoader::FrameAlign);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BufferAlign"),			&CBinLoader::BufferAlign);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FontFrameLevel"),		&CBinLoader::FontFrameLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FontFrameVacuum"),		&CBinLoader::FontFrameVacuum);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RearFrameLevel"),		&CBinLoader::RearFrameLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RearFrameVacuum"),		&CBinLoader::RearFrameVacuum);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DualBufferLevel"),		&CBinLoader::DualBufferLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ALFrontGate"),			&CBinLoader::ALFrontGate);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ALBackGate"),			&CBinLoader::ALBackGate);
//		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ALMgznClamp"),			&CBinLoader::ALMgznClamp);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetMS90PusherZ"),		&CBinLoader::SetMS90PusherZ);	//v4.59A40
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetMS90PusherX"),		&CBinLoader::SetMS90PusherX);	//v4.59A40

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Gripper2State"),		&CBinLoader::Gripper2State);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Gripper2Level"),		&CBinLoader::Gripper2Level);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrameLevel2"),			&CBinLoader::FrameLevel2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrameAlign2"),			&CBinLoader::FrameAlign2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FrameVacuum2"),			&CBinLoader::FrameVacuum2);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeX"),				&CBinLoader::HomeX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeX2"),				&CBinLoader::HomeX2);
//		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeY"),				&CBinLoader::HomeY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeZ"),				&CBinLoader::HomeZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeTheta"),			&CBinLoader::HomeTheta);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeToReady"),			&CBinLoader::HomeToReady);		//pllm
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DownElevatorZToReady"), &CBinLoader::DownElevatorZToReady);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DownElevatorThetaZToHome"), &CBinLoader::DownElevatorThetaZToHome);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerX"),				&CBinLoader::PowerX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerX2"),				&CBinLoader::PowerX2);
//		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerY"),				&CBinLoader::PowerY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerZ"),				&CBinLoader::PowerZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerArm"),				&CBinLoader::PowerArm);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerTheta"),			&CBinLoader::PowerTheta);

        // for gripper and bin table setup
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateHMIData"),		&CBinLoader::UpdateHMIData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangePosition"),		&CBinLoader::ChangePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("KeyInPosition"),		&CBinLoader::KeyInPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovePosPosition"),		&CBinLoader::MovePosPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveNegPosition"),		&CBinLoader::MoveNegPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSetup"),			&CBinLoader::ConfirmSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelSetup"),			&CBinLoader::CancelSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveSetupData"),		&CBinLoader::SaveSetupData);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("BarcodeTest"),			&CBinLoader::BarcodeTest);
 		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckSemitekPasswordBC"),		&CBinLoader::CheckSemitekPasswordBC);
       m_comServer.IPC_REG_SERVICE_COMMAND(_T("Barcode2Test"),			&CBinLoader::Barcode2Test);		//dual-table config fcn
		//MS100 9Inch fcns for gripper 2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangePosition2"),		&CBinLoader::ChangePosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateHMIData2"),		&CBinLoader::UpdateHMIData2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("KeyInPosition2"),		&CBinLoader::KeyInPosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovePosPosition2"),		&CBinLoader::MovePosPosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveNegPosition2"),		&CBinLoader::MoveNegPosition2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSetup2"),		&CBinLoader::ConfirmSetup2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelSetup2"),			&CBinLoader::CancelSetup2);

        // for magazine setup
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMagazine"),       &CBinLoader::SelectMagazine);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeMagazineIndex"),  &CBinLoader::ChangeMagazineIndex);

        m_comServer.IPC_REG_SERVICE_COMMAND(_T("OSRAMMagazineUnload"),  &CBinLoader::OSRAMMagazineUnload);

        m_comServer.IPC_REG_SERVICE_COMMAND(_T("KeyInValue"),           &CBinLoader::KeyInValue);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("IncreaseValue"),        &CBinLoader::IncreaseValue);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("DecreaseValue"),        &CBinLoader::DecreaseValue);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmMagazineSetup"), &CBinLoader::ConfirmMagazineSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelMagazineSetup"),  &CBinLoader::CancelMagazineSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateSlotPitch"),		&CBinLoader::UpdateSlotPitch);
		//MS100 9Inch Dual-Table config
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMagazine2"),		&CBinLoader::SelectMagazine2);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeMagazineIndex2"),	&CBinLoader::ChangeMagazineIndex2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelMagazineSetup2"), &CBinLoader::CancelMagazineSetup2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmMagazineSetup2"),&CBinLoader::ConfirmMagazineSetup2);

		// for Exchange Arm setup
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeArmPosition"),	&CBinLoader::ChangeArmPosition);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("KeyInArmPosition"),		&CBinLoader::KeyInArmPosition);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovePosArmPosition"),	&CBinLoader::MovePosArmPosition);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveNegArmPosition"),	&CBinLoader::MoveNegArmPosition);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmArmSetup"),		&CBinLoader::ConfirmArmSetup);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelArmSetup"),		&CBinLoader::CancelArmSetup);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveArmToChangePos"),	&CBinLoader::MoveArmToChangePos);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExchangeFrameTest"),	&CBinLoader::ExchangeFrameTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadBufferFrame"),	&CBinLoader::ManualLoadBufferFrame);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadBufferFrame"),	&CBinLoader::ManualUnloadBufferFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExArmPreloadFrameForNextWafer"), &CBinLoader::ExArmPreloadFrameForNextWafer);	//v2.68
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExArmUnloadLastFrame"),	&CBinLoader::ExArmUnloadLastFrame);

		DisplayMessage("BinLoaderStn Service Registered...");


        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetOperationMode"),			&CBinLoader::SetOperationMode);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetMgznUsage"),				&CBinLoader::SetMgznUsage);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetSlotUsage"),				&CBinLoader::SetSlotUsage);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetSlotBlock"),				&CBinLoader::SetSlotBlock);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMgznSetup"),			&CBinLoader::SelectOMSetupMgzn);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectSlotSetup"),			&CBinLoader::SelectOMSetupSlot);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadOperationMode"),		&CBinLoader::SetMagazineOMSetupDefault);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadCustomOpMode"),			&CBinLoader::LoadMagazineOMSetup);			//v3.82
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOperationMode"),		&CBinLoader::SaveMagazineOMSetup);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMagazineDisplay"),	&CBinLoader::SelectMagazineRTDisplay);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateCurrMgznDisplayName"),&CBinLoader::UpdateCurrMgznDisplayName);		//v4.17T1
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableBurnInTestRun"),      &CBinLoader::EnableBurnInTestRun);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUpdateFrameBarcode"),	&CBinLoader::ManualUpdateFrameBarcode);		//v3.98T5
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadFilmFrame"),      &CBinLoader::ManualLoadFilmFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadFilmFrame"),    &CBinLoader::ManualUnloadFilmFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoUnloadFilmFrameToFull"),		&CBinLoader::AutoUnloadFilmFrameToFull);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoUnloadFilmFrameNotToFull"),    &CBinLoader::AutoUnloadFilmFrameNotToFull);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoUnloadFilmFrameOsram"),    &CBinLoader::AutoUnloadFilmFrameOsram);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoUnloadFilmFrame"),		&CBinLoader::AutoUnloadFilmFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleStepMode"),           &CBinLoader::ToggleStepMode);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoChangeFilmFrame"),      &CBinLoader::AutoChangeFilmFrame);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoopTestCounter"),			&CBinLoader::LoopTestCounter);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLoadUnloadTest"),       &CBinLoader::AutoLoadUnloadTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetSingleSlotInfo"),		&CBinLoader::ResetSingleSlotInfo);
	    m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetMagazineCmd"),         &CBinLoader::ResetMagazineCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetSlotBarcode"),         &CBinLoader::ResetSlotBarcode);				//v3.68T1
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectRTOperationMode"),    &CBinLoader::SelectRTOperationMode);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadUnloadBinTableCmd"),	&CBinLoader::LoadUnloadBinTableCmd);			//v4.02T1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RealignBinFrameCmd"),       &CBinLoader::RealignBinFrameCmd);
 		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RealignBinFrameCmd1"),      &CBinLoader::RealignBinFrameCmd1);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("RealignBinFrameCmd_F10"),	&CBinLoader::RealignBinFrameCmd_F10);		//v4.26T1
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckBinFrameIsAligned"),	&CBinLoader::CheckBinFrameIsAligned);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckDualBufferFrameExist"),	&CBinLoader::CheckDualBufferFrameExist);
        //m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableRealign"),			EnableRealign);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UnloadFilmFrame"),			&CBinLoader::UnloadFilmFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartLotResetAllMgznCmd"),	&CBinLoader::StartLotResetAllMgznCmd);		//v2.70
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RestoreMagazineRunTimeStatus"),	&CBinLoader::RestoreMagazineRunTimeStatus);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsSlotAssignedWithBlock"),	&CBinLoader::IsSlotAssignedWithBlock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckFrameExistOnBinTable"), &CBinLoader::CheckFrameExistOnBinTable);	//v3.70T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowHistory"),				&CBinLoader::ShowHistory);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BinFrameSummary"),			&CBinLoader::BinFrameSummary);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetMagzFullFilePath"),			&CBinLoader::GetMagzFullFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetCreeBinBcSummaryFilePath"),	&CBinLoader::GetCreeBinBcSummaryFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualOutputSummaryFile"),		&CBinLoader::ManualOutputSummaryFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearAllMagazineFile"),			&CBinLoader::ClearAllMagazineFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearAllFrame"),				&CBinLoader::ClearAllFrame);
//		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualClearFrameForDBuffer"),	&CBinLoader::ManualClearFrameForDBuffer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUploadBinSummary"),		&CBinLoader::ManualUploadBinSummary);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToSlot"),				&CBinLoader::MoveToSlot);
//		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToSlot2"),				&CBinLoader::MoveToSlot2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateOutput"),				&CBinLoader::UpdateOutput);
		//m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBufferAlignParameters"), SetBufferAlignParameters);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MagazineSensorDisplay"),	&CBinLoader::MagazineSensorDisplay);	//v3.30T2

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckBinLoaderInSafeStatusCmd"), &CBinLoader::CheckBinLoaderInSafeStatusCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckExArmInSafePos"),			&CBinLoader::CheckExArmInSafePos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenerateConfigData"),			&CBinLoader::GenerateConfigData);			//v3.85
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestFcn"),						&CBinLoader::TestFcn);						//v3.57T3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestFrameLevel"),				&CBinLoader::TestFrameLevel);				//v3.70T4
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestBufferLevel"),				&CBinLoader::TestBufferLevel);				//v3.70T4
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestBothLevel"),				&CBinLoader::TestBothLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadUnloadRealignTest"),	&CBinLoader::ManualLoadUnloadRealignTest);	//v3.71T1

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DB_ManualLoadFromMgzToBuffer"),		&CBinLoader::DB_ManualLoadFromMgzToBuffer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DB_ManualUnloadFromBufferToMgz"),	&CBinLoader::DB_ManualUnloadFromBufferToMgz);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DB_ManualLoadFromBufferToTable"),	&CBinLoader::DB_ManualLoadFromBufferToTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DB_ManualUnloadFromTableToBuffer"), &CBinLoader::DB_ManualUnloadFromTableToBuffer);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetDualBuffer"),		&CBinLoader::ResetDualBuffer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearAllBinAdvanced"),	&CBinLoader::ClearAllBinAdvanced);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearAllBinUpdated"),	&CBinLoader::ClearAllBinUpdated);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearAllBinSelected"),	&CBinLoader::ClearAllBinSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearBinResetGradeSlot"),&CBinLoader::ClearBinResetGradeSlot);	//v4.50A12
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectClearBins"),		&CBinLoader::SelectClearBins);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeselectClearBins"),	&CBinLoader::DeselectClearBins);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBarcodePrefix"),			&CBinLoader::SetBarcodePrefix);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetCurrBlockID"),			&CBinLoader::GetCurrBlockID);		//v4.11T3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableCoverCheckPassword"),	&CBinLoader::EnableCoverCheckPassword);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetWLMagazine"),		&CBinLoader::GetWLMagazine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWLMagazine"),		&CBinLoader::SetWLMagazine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveWLoaderZ"),			&CBinLoader::MoveWLoaderZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveWLoaderY"),			&CBinLoader::MoveWLoaderY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovetoWLSlot"),			&CBinLoader::MovetoWLSlot);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovetoWLSlot_Sync"),	&CBinLoader::MovetoWLSlot_Sync);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UnloadTableFrameAtWaferEnd"),	&CBinLoader::UnloadTableFrameAtWaferEnd);

		//ChangeLight request 1:	//v4.60A2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UnLoadBinFrameToFullMagazine"),	&CBinLoader::UnLoadBinFrameToFullMagazine);

		//v4.55A11	//Autoline
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SECS_LoadFrameCmd"),		&CBinLoader::SECS_LoadFrameCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SECS_LoadFrameDoneCmd"),	&CBinLoader::SECS_LoadFrameDoneCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SECS_UnloadFrameCmd"),		&CBinLoader::SECS_UnloadFrameCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SECS_UnloadFrameDoneCmd"),	&CBinLoader::SECS_UnloadFrameDoneCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SIS_TransferInlineToStandalone"),	&CBinLoader::SIS_TransferInlineToStandalone);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SIS_TransferStandaloneToInline"),	&CBinLoader::SIS_TransferStandaloneToInline);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectLowerBufferGripperForSetup"),	&CBinLoader::SelectLowerBufferGripperForSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetUpperToLowerBufferOffsetZ"),	&CBinLoader::SetUpperToLowerBufferOffsetZ);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BurnInBLZUpperLimit"),	&CBinLoader::BurnInBLZUpperLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FindBLZEncoderFromZeroToHomeSensor"),	&CBinLoader::FindBLZEncoderFromZeroToHomeSensor);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetEmptyFramePath"),	&CBinLoader::SetEmptyFramePath);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

LONG CBinLoader::SelectLowerBufferGripperForSetup(IPC_CServiceMessage& svMsg)
{
    svMsg.GetMsg(sizeof(LONG), &m_lUseLBufferGripperForSetup);
	return TRUE;
}

LONG CBinLoader::SetUpperToLowerBufferOffsetZ(IPC_CServiceMessage& svMsg)
{
	LONG lTemp = 0;

	svMsg.GetMsg(sizeof(LONG),	&lTemp);
	if ((lTemp < 0) || (lTemp > 50000))
	{
		SetAlert_WarningMsg(IDS_MS_WARNING_GENERAL, "Exceed the limit, Input value must be 0 and 50000");
		SetAlarmLamp_Yellow(FALSE, TRUE);
		return TRUE;
	}
	m_lUpperToLowerBufferOffsetZ = lTemp;
	CBinGripperBuffer *pGripperBuffer = &m_clLowerGripperBuffer;
	DownElevatorToReady(pGripperBuffer);
	return TRUE;
}

LONG CBinLoader::CheckBinLoaderSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	
/*
	if (m_bWarningDisabledBarcodeScanner == TRUE)
	{
		if (m_bUseBarcode == FALSE)
		{
			LONG lRtn;
			bReturn = TRUE;
			CString szTitle, szContent;	
			szTitle.LoadString(HMB_BL_SETUP_CHECK);
			szContent.LoadString(HMB_BL_BC_READER_IS_DISABLED);
	 
			lRtn = HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			
			if (lRtn == glHMI_STOP)
			{
				bReturn = FALSE;		
			}
			else
			{
				m_bWarningDisabledBarcodeScanner = FALSE;
			}
			
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}


	//v4.40T15	//Cree HuiZhou
	if (m_fHardware && !m_bDisableBL)
	{
		CString szText;
		BOOL bAbort = FALSE;
		if (!m_bHome_Upper || !Upper_IsPowerOn())
		{
			szText = "\nBin Upper Gripper";
			SetStatusMessage("Bin Upper Gripper NOT home");
			SetErrorMessage("Bin Upper Gripper NOT home");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			bAbort = TRUE;
		}

		if (!m_bHome_Lower || !Lower_IsPowerOn())
		{
			szText = "\nBin Lower Gripper";
			SetStatusMessage("Bin Lower Gripper NOT home");
			SetErrorMessage("Bin Lower Gripper NOT home");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			bAbort = TRUE;
		}

		if ( !m_bHome_Theta || !Theta_IsPowerOn() )
		{
			szText = "\nBinLoaderTheta";
			SetStatusMessage("BinLoaderTheta NOT home");
			SetErrorMessage("BinLoaderTheta NOT home");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			bAbort = TRUE;
		}

		if ( !m_bHome_Z || !Z_IsPowerOn() )
		{
			szText = "\nBinLoaderZ";
			SetStatusMessage("BinLoaderZ NOT home");
			SetErrorMessage("BinLoaderZ NOT home");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			bAbort = TRUE;
		}

		if (bAbort)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	//v4.42T12
	if (m_bRealignBinFrame		&& 		// && !m_bNoSensorCheck)
		!IsBurnIn()				&& 
		(GetCurrBinOnBT() != 0) )
	{
		if (IsMS90())		//v4.58A1
		{
		}
	}


	//v4.40T6
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetCustomerName() == CTM_NICHIA) && (pApp->GetProductLine() == "") &&	//v4.59A34
		 (GetCurrBinOnBT() != 0) && 
		 !m_clUpperGripperBuffer.IsBufferBlockExist())
	{
		BOOL bGradeFound = FALSE;
		if ( m_WaferMapWrapper.IsMapValid() && (CMS896AStn::m_oNichiaSubSystem.m_ulMaxNoOfWafer > 0) )
		{
			CUIntArray aulSelectedGradeList;
			UCHAR ucGrade = 0;
			m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);

			for (INT k=0; k<aulSelectedGradeList.GetSize(); k++)
			{
				ucGrade = aulSelectedGradeList.GetAt(k) - m_WaferMapWrapper.GetGradeOffset();
				if (ucGrade == GetCurrBinOnBT())
				{
					bGradeFound = TRUE;
					break;
				}
			}

			aulSelectedGradeList.RemoveAll();

			if (bGradeFound)
			{
				AddNichiaWafIDList(GetCurrBinOnBT(), m_szMapFileName);
				if (CheckIfNichiaWafIDListExceedLimit(GetCurrBinOnBT(), FALSE) == TRUE)
				{
					if (m_fHardware && IsMagazineSafeToMove())
					{
//AfxMessageBox("Andrew: Grade found BL Unload frame to FULL magazine ...", MB_SYSTEMMODAL);
						UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, TRUE, TRUE);
					}
				}
				else
				{
					UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, FALSE, TRUE);
				}
			}
			else if (CMS896AStn::m_oNichiaSubSystem.m_ucProcessMode == 1)	//Single Mode
			{
				UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, TRUE, TRUE);
			}
			else
			{
				if (CheckIfNichiaWafIDListExceedLimit(GetCurrBinOnBT(), TRUE) == TRUE)
				{
					if (m_fHardware && IsMagazineSafeToMove())
					{
//AfxMessageBox("Andrew: Grade NOT found but BL Unload frame to FULL magazine ...", MB_SYSTEMMODAL);
						UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, TRUE, TRUE);
					}
				}
			}
		}
	}
*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::PreBondChecking(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
/*
	if (IsClearAllState() == TRUE)
	{
		//v4.48A25	//YiMing
		CString szTitle, szContent;
		szTitle.LoadString(HMB_BL_CLEAR_ALL_FRAME);
		szContent.LoadString(HMB_BL_CLEAR_ALL_FRAME_NOT_COMPLETE);	
		HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;	
	}

	//v4.39T7
	if (IsFrameLevel() == FALSE)
	{
		SetFrameLevel(FALSE);	// down bin table first if necessary
		Sleep(500);
		if (IsFrameLevel() == FALSE)
		{
			CString szMsg = "Bin Table 1";
			SetErrorMessage("BL PreBondChecking: BT1 platform not at DOWN posn");
			SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);	//Matt: change alarm
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;	
		}
	}

	//v4.59A40	//Finisar MS90 TX
	if (!CheckMS90BugPusherAtSafePos())
	{
		CString szMsg = "Bin Table";
		SetErrorMessage("BL PreBondChecking: BT Pusher XZ not at SAFE position");
		SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;	
	}
*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::UpdateAction(IPC_CServiceMessage& svMsg)
{
	Sleep(500);
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::OsramBinRealignCheck(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bDisable = TRUE;

	svMsg.GetMsg(sizeof(BOOL),	&bDisable);
	if( pApp->GetCustomerName()=="OSRAM" && bDisable==FALSE )
	{
		CString szPassword;
		CHAR acPar[200], *pTemp;
		ULONG ulSize;
		IPC_CServiceMessage stMsg;

		szPassword = PRESCAN_PASSWORD_SUPER_USER;
		strcpy_s(acPar, sizeof(acPar), "Please Enter YOUR Password");
		ulSize = (ULONG)strlen(acPar) + 1;
		pTemp = acPar + ulSize;
		strcpy_s(pTemp, sizeof(acPar) - ulSize, szPassword);
		ulSize += szPassword.GetLength() + 1;

		stMsg.InitMessage(ulSize, acPar);
		INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiPassword", stMsg);

		// Get the reply
		while ( m_comClient.ScanReplyForConvID(nConvID, 500) == 0);

		m_comClient.ReadReplyForConvID(nConvID, stMsg);
		stMsg.GetMsg(sizeof(BOOL), &bReturn);
		if( bReturn==FALSE )
		{
			m_bRealignBinFrame = TRUE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}	// osram check password when disable bin frame realignment


LONG CBinLoader::LogItems(IPC_CServiceMessage& svMsg)
{
	LONG lEventNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lEventNo);

	LogItems(lEventNo);
	
	return 1;
}

LONG CBinLoader::GripperState(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetGripperState(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::Gripper2State(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetGripper2State(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::GripperLevel(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetGripperLevel(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::Gripper2Level(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetGripper2Level(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::GripperPusher(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetGripperPusher(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::FrameVacuum(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetFrameVacuum(bOn);
	CMS896AStn::m_bRealignFrameDone = FALSE;	//v2.63

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::FrameVacuum2(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetFrameVacuum2(bOn);
	CMS896AStn::m_bRealignFrame2Done = FALSE;

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::FrameLevel(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	// Turn off the joystick first 
	if (bOn == TRUE)
		SetBinTableJoystick(FALSE);
	SetFrameLevel(bOn);

	if (bOn && m_bRealignBinFrame && m_lBTCurrentBlock > 0)		//v4.59A31
	{
		int nConvID = 0;
		ULONG ulBlkID = (ULONG) m_lBTCurrentBlock;
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(ULONG), &ulBlkID);
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetFrameAlign", stMsg);
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::FrameLevel2(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	// Turn off the joystick first 
	if (bOn == TRUE)
		SetBinTableJoystick(FALSE);
	SetFrameLevel2(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::GetFrameLevel(IPC_CServiceMessage& svMsg)
{
	BOOL bIsUp;
	bIsUp = !IsFrameLevel();

	svMsg.InitMessage(sizeof(BOOL), &bIsUp);

	//BOOL bReturn=TRUE;
	//svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::FrameAlign(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetFrameAlign(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::FrameAlign2(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetFrameAlign2(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::BufferAlign(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetBufferAlign(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::FontFrameLevel(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if ( (bOn == TRUE) && (IsBondArmSafe() == FALSE) )
	{
		return TRUE;
	}

	if (IsExArmInPickPlacePos() == FALSE)
	{
		return TRUE;
	}

	SetFontArmLevel(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::FontFrameVacuum(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetFontArmVacuum(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::RearFrameLevel(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if ( (bOn == TRUE) && (IsBondArmSafe() == FALSE) )
	{
		return TRUE;
	}

	if (IsExArmInPickPlacePos() == FALSE)
	{
		return TRUE;
	}

	SetRearArmLevel(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::RearFrameVacuum(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetRearArmVacuum(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::DualBufferLevel(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetBufferLevel(bOn);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::ALFrontGate(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetALFrontGate(bOn);

	if (bOn)	//autoline1
	{
		HmiMessage("BL FRONT gate is OPEN");
	}
	else
	{
		HmiMessage("BL FRONT gate is CLOSED");
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::ALBackGate(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetALBackGate(bOn);

	if (bOn)	//autoline1
	{
		HmiMessage("BL BACK gate is OPEN");
	}
	else
	{
		HmiMessage("BL BACK gate is CLOSED");
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

/*
LONG CBinLoader::ALMgznClamp(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetALMgznClamp(bOn);

	if (bOn)	//autoline1
	{
		HmiMessage("BL MGZN holder is LOCKED");
	}
	else
	{
		HmiMessage("BL MGZN holder is UNLOCKED");
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
*/

LONG CBinLoader::SetMS90PusherZ(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	BOOL bReturn=TRUE;
	if (!m_bEnableMS90TablePusher)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetMS90BufPusherZ(bOn);

	if (bOn)
	{
		HmiMessage("BL PusherZ is UP");
	}
	else
	{
		HmiMessage("BL PusherZ is DOWN");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::SetMS90PusherX(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	BOOL bReturn=TRUE;
	if (!m_bEnableMS90TablePusher)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetMS90BufPusherX(bOn);

	if (bOn)
	{
		HmiMessage("BL PusherX is ON");
	}
	else
	{
		HmiMessage("BL PusherX is OFF");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::PowerX(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetLoaderXPower(bOn);
	if ( (bOn == TRUE) && (m_bHomeGripper == TRUE) )
	{
		//v4.59A41	//Finisar MS90 TX
		if (!CheckMS90BugPusherAtSafePos())
		{
			CString szMsg = "Bin Loader Module";
			SetErrorMessage("BL PreBondChecking: BT Pusher XZ not at SAFE position");
			SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);	
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;	
		}

		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
		{
			if ( IsDualBufferUpperExist() )
			{
				SetErrorMessage("BL Frame exists in Upper Buffer");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
			}
			else if ( IsDualBufferLowerExist() )
			{
				SetBufferLevel(FALSE);
				Sleep(300);

				if ( IsDualBufferLevelDown() == FALSE )
				{
					SetErrorMessage("BL Frame exists in Lower Buffer");
					SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
				}
				else
				{
					X_Home();
				}
			}
			else
			{
				X_Home();
			}
		}
		else
		{
			X_Home();
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::PowerX2(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetLoaderX2Power(bOn);

	if ( (bOn == TRUE) && (m_bHomeGripper == TRUE) )
	{
		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
		{
			if ( IsDualBufferUpperExist() )
			{
				SetErrorMessage("BL Frame exists in Upper Buffer");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
			}
			else if ( IsDualBufferLowerExist() )
			{
				SetBufferLevel(FALSE);
				Sleep(300);

				if ( IsDualBufferLevelDown() == FALSE )
				{
					SetErrorMessage("BL Frame exists in Lower Buffer");
					SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
				}
				else
				{
					X2_Home();
				}
			}
			else
			{
				X2_Home();
			}
		}
		else
		{
			X2_Home();
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::PowerZ(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetLoaderZPower(bOn);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::PowerArm(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::PowerTheta(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetLoaderTPower(bOn);

//	if (bOn == TRUE)
//	{
//		Theta_Home();
//	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::HomeX(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	//v4.59A41	//Finisar MS90 TX
	if (!CheckMS90BugPusherAtSafePos())
	{
		CString szMsg = "BL PreBondChecking: BT Pusher XZ not at SAFE position";
		SetErrorMessage(szMsg);
		SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;	
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		if ( IsDualBufferUpperExist() )
		{
			SetErrorMessage("BL Frame exists in Upper Buffer");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
		}
		else if ( IsDualBufferLowerExist() )
		{
			SetBufferLevel(FALSE);
			Sleep(300);

			if ( IsDualBufferLevelDown() == FALSE )
			{
				SetErrorMessage("BL Frame exists in Lower Buffer");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
			}
			else
			{
				X_Home();
			}
		}
		else
		{
            X_Home();
		}
	}
	else
	{
		X_Home();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::HomeX2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		if ( IsDualBufferUpperExist() )
		{
			SetErrorMessage("BL Frame exists in Upper Buffer");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
		}
		else if ( IsDualBufferLowerExist() )
		{
			SetBufferLevel(FALSE);
			Sleep(300);

			if ( IsDualBufferLevelDown() == FALSE )
			{
				SetErrorMessage("BL Frame exists in Lower Buffer");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
			}
			else
			{
				X2_Home();
			}
		}
		else
		{
            X2_Home();
		}
	}
	else
	{
		X2_Home();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::HomeZ(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if ( IsMagazineSafeToMove() == FALSE )
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsFrameOutOfMgz())
	{
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		SetErrorMessage("BL Frame is out of magazine");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	
	X_Home();
	X2_Home();
	Z_Home();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::HomeTheta(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if ( IsMagazineSafeToMove() == FALSE )
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsFrameOutOfMgz())
	{
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		SetErrorMessage("BL Frame is out of magazine");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	X_Home();
	X2_Home();

	Theta_Home();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::HomeToReady(IPC_CServiceMessage& svMsg)	//pllm
{
	BOOL bReturn = TRUE;

	if (!LoadUnloadLockMutex())
	{
		CString szLog;

		szLog.Format("LoadUnloadLockMutex  UnloadBinFrame  = %ld", m_lLoadUnloadSECSCmdObject);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (IsExchangeArmReady())	//v3.01T3
	{
	//	Arm_MoveTo(m_lExArmReadyPos);	//v2.66
	}

	if (!CheckSafeToMove(TRUE))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		LoadUnloadUnlock();
		return TRUE;
	}

	if (Z_IsPowerOn())
	{
		if (Z_MoveTo(m_lReadyLvlZ) != gnOK)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			LoadUnloadUnlock();
			return TRUE;
		}
		Z_Sync();
	}

	if (m_bEnableBinElevatorCoverLock == TRUE)
	{
		SetBinElevatorCoverLock(FALSE);
	}
	if (m_bEnableSideCoverLock == TRUE)
	{
		SetSideCoverLock(FALSE);
	}

	LoadUnloadUnlock();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::DownElevatorZToReady(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = DownElevatorThetaZToPosn(m_lReadyLvlZ);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::DownElevatorThetaZToHome(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = DownElevatorThetaZToPosn(0);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::DownElevatorThetaZToPosn(const LONG lZPosn)
{
	BOOL bReturn = TRUE;

	if (!LoadUnloadLockMutex())
	{
		CString szLog;

		szLog.Format("LoadUnloadLockMutex  UnloadBinFrame  = %ld", m_lLoadUnloadSECSCmdObject);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	if (!CheckSafeToMove(TRUE))
	{
		LoadUnloadUnlock();
		return FALSE;
	}


	if (GripperMoveTo(&m_clUpperGripperBuffer, GetReadyPosn(&m_clUpperGripperBuffer)) != TRUE)
	{
		CString szMess;
		szMess = _T("Upper Gripper can not move to home position");
		HmiMessage_Red_Yellow(szMess);
		SetErrorMessage(szMess);
		LoadUnloadUnlock();
		return FALSE;
	}
	
	if (GripperMoveTo(&m_clLowerGripperBuffer, GetReadyPosn(&m_clLowerGripperBuffer)) != TRUE)
	{
		CString szMess;
		szMess = _T("Lower Gripper can not move to home position");
		HmiMessage_Red_Yellow(szMess);
		SetErrorMessage(szMess);
		LoadUnloadUnlock();
		return FALSE;
	}

	if (Theta_MoveTo(m_lReadyPos_T, SFM_WAIT) != gnOK)
	{
		CString szMess;
		szMess = _T("Theta can not move to ready position");
		HmiMessage_Red_Yellow(szMess);
		SetErrorMessage(szMess);
		LoadUnloadUnlock();
		return FALSE;
	}

	if (Z_MoveTo(lZPosn) != gnOK)
	{
		LoadUnloadUnlock();
		return FALSE;
	}

	Z_Sync();

	LoadUnloadUnlock();
	return TRUE;
}


LONG CBinLoader::UpdateHMIData(IPC_CServiceMessage& svMsg)
{
	//Restore HMI variable
    m_lBLGeneral_1	= m_lPreUnloadPos_X;
	m_lBLGeneral_2	= m_lUnloadPos_X;		
	m_lBLGeneral_3	= m_lBarcodePos_X;	
	m_lBLGeneral_4	= m_lLoadMagPos_X;
	m_lBLGeneral_15	= m_lUnloadMagPos_X;
	m_lBLGeneral_5	= m_lReadyPos_X;
	m_lBLGeneral_6	= m_lBTUnloadPos_X;
	m_lBLGeneral_7	= m_lBTUnloadPos_Y;
	m_lBLGeneral_8	= m_lBufferUnloadPos_X;
	m_lBLGeneral_9	= m_lBTBarcodePos_X;
	m_lBLGeneral_10	= m_lBTBarcodePos_Y;
	m_lBLGeneral_11 = m_lAutoLineUnloadZ;	//v4.56A1

	m_lBLGeneral_12	= m_lLoadPos1_T;
	m_lBLGeneral_13	= m_lLoadPos2_T;
	m_lBLGeneral_14	= m_lReadyPos_T;
	//Used to update Current Mgzn & Slot number display in HMI BinLoader setup page
	UpdateExArmMgzSlotNum(GetCurrBinOnBT());	//v4.34T1	//Yealy Mode-H

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::UpdateHMIData2(IPC_CServiceMessage& svMsg)
{
	//Restore HMI variable
    m_lBLGeneral_1	= m_lPreUnloadPos_X2;
	m_lBLGeneral_2	= m_lUnloadPos_X2;		
	m_lBLGeneral_3	= m_lBarcodePos_X2;	
	m_lBLGeneral_4	= m_lLoadMagPos_X2;
	m_lBLGeneral_15	= m_lUnloadMagPos_X2;
	m_lBLGeneral_5	= m_lReadyPos_X2;
	m_lBLGeneral_6	= m_lBTUnloadPos_X2;
	m_lBLGeneral_7	= m_lBTUnloadPos_Y2;
	m_lBLGeneral_8	= m_lBufferUnloadPos_X2;

	//v4.59A43
	m_lBLGeneral_12	= m_lLoadPos1_T;
	m_lBLGeneral_13	= m_lLoadPos2_T;
	m_lBLGeneral_14	= m_lReadyPos_T;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBinLoader::ChangePosition(IPC_CServiceMessage& svMsg)
{
	LONG lPosition;
	LONG lMinPos, lMaxPos, lDist=0;
	LONG lMinPos3, lMaxPos3;


	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	//Let frame level is down & gripper level is up before move bintable
	if ( lPosition >= 5 )
	{
		SetGripperLevel(FALSE);
		SetFrameLevel(FALSE);
		Sleep(250);
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A43
	{
		lMinPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_MAX_DISTANCE);	
		lMinPos3 = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos3 = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MAX_DISTANCE);	
	}
	else
	{
		//Check limit range
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_MAX_DISTANCE);	
	}

	switch(lPosition)
	{
		case 0:	lDist = m_lPreUnloadPos_X;	break;	// Gripper preunload
		case 1:	lDist = m_lUnloadPos_X;		break;	// Gripper unload
		case 2:	lDist = m_lBarcodePos_X;	break;	// Gripper barcode scan
		case 3:	lDist = m_lLoadMagPos_X;	break;	// Gripper load magazine
		case 4:	lDist = m_lReadyPos_X;		break;	// Gripper ready
		case 7:	lDist = m_lBufferUnloadPos_X;	break;	// Gripper ready

		case 12: lDist = m_lLoadPos1_T;		break;	// Load1 T
		case 13: lDist = m_lLoadPos2_T;		break;	// Load2 T
		case 14: lDist = m_lReadyPos_T;		break;	// Gripper ready
		case 15: lDist = m_lUnloadMagPos_X; break;  // Gripper unload magazine
	}


	if ((lPosition >= 12) && (lPosition <= 14))	//THETA axis
	{
		if (lDist < lMinPos3) 
		{
			lDist = lMinPos3;
		}
		if (lDist > lMaxPos3)
		{
			lDist = lMaxPos3;
		}
	}
	else
	{
		if (lDist < lMinPos) 
		{
			lDist = lMinPos;
		}

		if (lDist > lMaxPos)
		{
			lDist = lMaxPos;
		}
	}

	if (lPosition == 4)
	{
		if (lDist > BL_GRIPPER_MAX_POS_LIMIT)
		{
			lDist = BL_GRIPPER_MAX_POS_LIMIT;
		}
	}

	switch(lPosition)
	{
	case 0: 
	case 1:		
	case 2:		
	case 3:		
	case 4:
	case 7:
	case 15:
		if ( IsFrameLevel() == FALSE )
		{
			SetErrorMessage("Frame Platform is not down");
			LONG lReturn = HmiMessage_Red_Yellow("Frame Platform is not down; continue?", "BL Gripper Setup", glHMI_MBX_CONTINUESTOP|0x80000000);	//v3.98T5
			if (lReturn == glHMI_STOP)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}
		Upper_MoveTo(lDist);
		m_lBLGeneral_TmpA = lDist;
		break;

	case 5:	// BT Unload x
    	SetBinTableJoystick(FALSE);
		MoveWaferTableToSafePosn(TRUE);		//v3.61
		MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y);
		SetBinTableJoystick(TRUE);
		m_lBLGeneral_TmpA = m_lBTUnloadPos_X;
		break;

	case 6:	// BT Unload x
    	SetBinTableJoystick(FALSE);
		MoveWaferTableToSafePosn(TRUE);		//v3.61
		MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y);
		SetBinTableJoystick(TRUE);
		m_lBLGeneral_TmpA = m_lBTUnloadPos_Y;
		break;

	case 9:		// BT Barcode x
    	SetBinTableJoystick(FALSE);
		MoveWaferTableToSafePosn(TRUE);	
		MoveBinTable(m_lBTBarcodePos_X, m_lBTBarcodePos_Y);
		SetBinTableJoystick(TRUE);
		m_lBLGeneral_TmpA = m_lBTBarcodePos_X;
		break;

	case 10:	// BT Barcode Y
    	SetBinTableJoystick(FALSE);
		MoveWaferTableToSafePosn(TRUE);	
		MoveBinTable(m_lBTBarcodePos_X, m_lBTBarcodePos_Y);
		SetBinTableJoystick(TRUE);
		m_lBLGeneral_TmpA = m_lBTBarcodePos_Y;
		break;

	case 12:	//TOP1 T
	case 13:	//TOP2 T
	case 14:	//READY T (or UNLOAD T)
		Theta_SlowMoveTo(lDist);
		m_lBLGeneral_TmpA = lDist;
		break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CBinLoader::ChangePosition2(IPC_CServiceMessage& svMsg)
{
	LONG lPosition;
	LONG lMinPos, lMaxPos, lDist=0;
	LONG lMinPos3, lMaxPos3;
	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	
	//Let frame level is down & gripper level is up before move bintable
	if ( lPosition >= 5 )
	{
		SetGripper2Level(FALSE);
		SetFrameLevel2(FALSE);
		Sleep(250);
	}


	//Check limit range
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A43
	{
		lMinPos		= GetChannelInformation(MS896A_CFG_CH_BINLOADER_LOWER, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos		= GetChannelInformation(MS896A_CFG_CH_BINLOADER_LOWER, MS896A_CFG_CH_MAX_DISTANCE);	
		lMinPos3	= GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos3	= GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MAX_DISTANCE);	
	}
	else
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER2, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER2, MS896A_CFG_CH_MAX_DISTANCE);	
	}

	switch(lPosition)
	{
		case 0:	lDist = m_lPreUnloadPos_X2;		break;	// Gripper preunload
		case 1:	lDist = m_lUnloadPos_X2;		break;	// Gripper unload
		case 2:	lDist = m_lBarcodePos_X2;		break;	// Gripper barcode scan
		case 3:	lDist = m_lLoadMagPos_X2;		break;	// Gripper load magazine
		case 15:lDist = m_lUnloadMagPos_X2;		break;	// Gripper unload magazine
		case 4:	lDist = m_lReadyPos_X2;			break;	// Gripper ready
		case 7:	lDist = m_lBufferUnloadPos_X2;	break;	// Gripper ready

		case 12: lDist = m_lLoadPos1_T;			break;	// Load1 T
		case 13: lDist = m_lLoadPos2_T;			break;	// Load2 T
		case 14: lDist = m_lReadyPos_T;			break;	// Gripper ready
	}

	if ( (lPosition >= 12) && (lPosition <= 14) )	//THETA axis
	{
		if (lDist < lMinPos3) 
		{
			lDist = lMinPos3;
		}
		if (lDist > lMaxPos3)
		{
			lDist = lMaxPos3;
		}
	}
	else
	{
		if (lDist < lMinPos) 
		{
			lDist = lMinPos;
		}

		if (lDist > lMaxPos)
		{
			lDist = lMaxPos;
		}
	}

	if ( lPosition == 4 )
	{
		if (lDist > BL_GRIPPER_MAX_POS_LIMIT)
		{
			lDist = BL_GRIPPER_MAX_POS_LIMIT;
		}
	}

	switch(lPosition)
	{
	case 0: 
	case 1:		
	case 2:		
	case 3:		
	case 4:
	case 7:
	case 15:
		if ( IsFrameLevel2() == FALSE )
		{
			SetErrorMessage("Frame Platform 2 is not down");
			LONG lReturn = HmiMessage_Red_Yellow("Frame Platform 2 is not down; continue?", "BL Gripper Setup", glHMI_MBX_CONTINUESTOP|0x80000000);	//v3.98T5
			if (lReturn == glHMI_STOP)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}
		X2_MoveTo(lDist);
		m_lBLGeneral_TmpA = lDist;
		break;

	case 5:	// BT Unload x2
    	SetBinTableJoystick(FALSE);
		MoveBinTable2(m_lBTUnloadPos_X2, m_lBTUnloadPos_Y2);
		SetBinTableJoystick(TRUE, TRUE);
		m_lBLGeneral_TmpA = m_lBTUnloadPos_X2;
		break;

	case 6:	// BT Unload y2
    	SetBinTableJoystick(FALSE);
		MoveBinTable2(m_lBTUnloadPos_X2, m_lBTUnloadPos_Y2);
		SetBinTableJoystick(TRUE, TRUE);
		m_lBLGeneral_TmpA = m_lBTUnloadPos_Y2;
		break;

	case 12:	//TOP1 T
	case 13:	//TOP2 T
	case 14:	//READY T (or UNLOAD T)
		Theta_SlowMoveTo(lDist);
		m_lBLGeneral_TmpA = lDist;
		break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CBinLoader::KeyInPosition(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lMaxPos3 = 0;
	LONG lMinPos3 = 0;
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;
	BLSETUP stInfo;


	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A43
	{
		lMinPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_MAX_DISTANCE);	
		lMinPos3 = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos3 = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MAX_DISTANCE);	
	}
	else
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_MAX_DISTANCE);	
	}

	if ( stInfo.lPosition == 4 )
	{
		lMaxPos = BL_GRIPPER_MAX_POS_LIMIT;
	}

	if ( (stInfo.lPosition >= 12) && (stInfo.lPosition <= 14) )	//THETA axis
	{
		if (stInfo.lStep < lMinPos3) 
		{
			stInfo.lStep = lMinPos3;
		}
		if (stInfo.lStep > lMaxPos3)
		{
			stInfo.lStep = lMaxPos3;
		}
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

	if ( IsFrameLevel() == FALSE )
	{
		SetErrorMessage("Frame Platform is not down");
		LONG lReturn = HmiMessage_Red_Yellow("Frame Platform is not down; continue?", "BL Gripper Setup", glHMI_MBX_CONTINUESTOP|0x80000000);	//v3.98T5
		if (lReturn == glHMI_STOP)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	switch(stInfo.lPosition)
	{
		case 0: // gripper preunload
			m_lBLGeneral_1 = stInfo.lStep;
			Upper_MoveTo(m_lBLGeneral_1);
			break;

		case 1:	//Gripper Unload
			m_lBLGeneral_2 = stInfo.lStep;
			Upper_MoveTo(m_lBLGeneral_2);
			break;

		case 2:		//Gripper barcode
			m_lBLGeneral_3 = stInfo.lStep;
			Upper_MoveTo(m_lBLGeneral_3);
			break;

		case 3:		//Gripper load Magazine
			m_lBLGeneral_4 = stInfo.lStep;
			Upper_MoveTo(m_lBLGeneral_4);
			break;

		case 4:		//Gripper Ready
			m_lBLGeneral_5 = stInfo.lStep;
			Upper_MoveTo(m_lBLGeneral_5);
			break;
		case 7:		//Gripper Ready
			m_lBLGeneral_8 = stInfo.lStep;
			Upper_MoveTo(m_lBLGeneral_8);
			break;

		case 12:	//TOP1 T
			m_lBLGeneral_12 = stInfo.lStep;
			Theta_SlowMoveTo(m_lBLGeneral_12);
			break;
		case 13:	//TOP2 T
			m_lBLGeneral_13 = stInfo.lStep;
			Theta_SlowMoveTo(m_lBLGeneral_13);
			break;
		case 14:	//READY T (or UNLOAD T)
			m_lBLGeneral_14 = stInfo.lStep;
			Theta_SlowMoveTo(m_lBLGeneral_14);
			break;

		case 15:		//Gripper unload Magazine
			m_lBLGeneral_15 = stInfo.lStep;
			Upper_MoveTo(m_lBLGeneral_15);
			break;

	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::KeyInPosition2(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lMaxPos3 = 0;
	LONG lMinPos3 = 0;
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;
	BLSETUP stInfo;


	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A43
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_LOWER, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_LOWER, MS896A_CFG_CH_MAX_DISTANCE);	
		lMinPos3 = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos3 = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MAX_DISTANCE);	
	}
	else
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER2, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER2, MS896A_CFG_CH_MAX_DISTANCE);	
	}

	if ( stInfo.lPosition == 4 )
	{
		lMaxPos = BL_GRIPPER_MAX_POS_LIMIT;
	}

	if ( (stInfo.lPosition >= 12) && (stInfo.lPosition <= 14) )	//THETA axis
	{
		if (stInfo.lStep < lMinPos3) 
		{
			stInfo.lStep = lMinPos3;
		}
		if (stInfo.lStep > lMaxPos3)
		{
			stInfo.lStep = lMaxPos3;
		}
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

	if ( IsFrameLevel2() == FALSE )
	{
		SetErrorMessage("Frame Platform 2 is not down");
		LONG lReturn = HmiMessage_Red_Yellow("Frame Platform 2 is not down; continue?", "BL Gripper Setup", glHMI_MBX_CONTINUESTOP|0x80000000);	//v3.98T5
		if (lReturn == glHMI_STOP)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	switch(stInfo.lPosition)
	{
		case 0: // gripper preunload
			m_lBLGeneral_1 = stInfo.lStep;
			X2_MoveTo(m_lBLGeneral_1);
			break;

		case 1:	//Gripper Unload
			m_lBLGeneral_2 = stInfo.lStep;
			X2_MoveTo(m_lBLGeneral_2);
			break;

		case 2:		//Gripper barcode
			m_lBLGeneral_3 = stInfo.lStep;
			X2_MoveTo(m_lBLGeneral_3);
			break;

		case 3:		//Gripper load magazine
			m_lBLGeneral_4 = stInfo.lStep;
			X2_MoveTo(m_lBLGeneral_4);
			break;

		case 4:		//Gripper Ready
			m_lBLGeneral_5 = stInfo.lStep;
			X2_MoveTo(m_lBLGeneral_5);
			break;
		case 7:		//Gripper Ready
			m_lBLGeneral_8 = stInfo.lStep;
			X2_MoveTo(m_lBLGeneral_8);
			break;

		case 12:	//TOP1 T
			m_lBLGeneral_12 = stInfo.lStep;
			Theta_SlowMoveTo(m_lBLGeneral_12);
			break;
		case 13:	//TOP2 T
			m_lBLGeneral_13 = stInfo.lStep;
			Theta_SlowMoveTo(m_lBLGeneral_13);
			break;
		case 14:	//READY T (or UNLOAD T)
			m_lBLGeneral_14 = stInfo.lStep;
			Theta_SlowMoveTo(m_lBLGeneral_14);
			break;

		case 15:		//Gripper unload magazine
			m_lBLGeneral_15 = stInfo.lStep;
			X2_MoveTo(m_lBLGeneral_15);
			break;

	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::MovePosPosition(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;

	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	switch(stInfo.lPosition)
	{
		case 0:	lCurrentPos = m_lBLGeneral_1 + stInfo.lStep;        break; // gripper preunload
		case 1:	lCurrentPos = m_lBLGeneral_2 + stInfo.lStep;        break;	//Gripper Unload
		case 2:	lCurrentPos = m_lBLGeneral_3 + stInfo.lStep;        break;	//Gripper barcode
		case 3:	lCurrentPos = m_lBLGeneral_4 + stInfo.lStep;        break;	//Gripper load magazine
		case 4:	lCurrentPos = m_lBLGeneral_5 + stInfo.lStep;        break;	//Gripper Ready
		case 7:	lCurrentPos = m_lBLGeneral_8 + stInfo.lStep;        break;	//Gripper Buffer Unload
		case 12:lCurrentPos = m_lBLGeneral_12 + stInfo.lStep;		break;	//Top1 T
		case 13:lCurrentPos = m_lBLGeneral_13 + stInfo.lStep;		break;	//Top2 T
		case 14:lCurrentPos = m_lBLGeneral_14 + stInfo.lStep;		break;	//Ready T
		case 15:lCurrentPos = m_lBLGeneral_15 + stInfo.lStep;       break;	//Gripper unload magazine
		default: return TRUE;
	}

	//Check limit range
	if ( (stInfo.lPosition >= 12) && (stInfo.lPosition <= 14) )
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MAX_DISTANCE);	

		if ( (lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos) )
		{
			Theta_SlowMoveTo(lCurrentPos);

			switch(stInfo.lPosition)
			{
				case 12:	m_lBLGeneral_12 = lCurrentPos;   break;
				case 13:	m_lBLGeneral_13 = lCurrentPos;   break;
				case 14:	m_lBLGeneral_14 = lCurrentPos;   break;
			}
		}

	}
	else if ( (stInfo.lPosition < 5) || (stInfo.lPosition == 7) || (stInfo.lPosition == 15))
	{
		if ( IsFrameLevel() == FALSE )
		{
			SetErrorMessage("Frame Platform is not down");
			LONG lReturn = HmiMessage_Red_Yellow("Frame Platform is not down; continue?", "BL Gripper Setup", glHMI_MBX_CONTINUESTOP|0x80000000);
			if (lReturn == glHMI_STOP)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A43
		{
			lMinPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_MAX_DISTANCE);	
		}
		else
		{
			lMinPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_MAX_DISTANCE);
		}

		if ( stInfo.lPosition == 4 )
		{
			lMaxPos = BL_GRIPPER_MAX_POS_LIMIT;
		}

		if ( (lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos) )
		{
			Upper_MoveTo(lCurrentPos);

			switch(stInfo.lPosition)
			{
				case 0:	m_lBLGeneral_1 = lCurrentPos;   break;
				case 1:	m_lBLGeneral_2 = lCurrentPos;	break;
				case 2:	m_lBLGeneral_3 = lCurrentPos;	break;
				case 3:	m_lBLGeneral_4 = lCurrentPos;	break;
				case 4:	m_lBLGeneral_5 = lCurrentPos;	break;
				case 7:	m_lBLGeneral_8 = lCurrentPos;	break;
				case 15:m_lBLGeneral_15 = lCurrentPos;	break;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::MovePosPosition2(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;

	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	switch(stInfo.lPosition)
	{
		case 0:	lCurrentPos = m_lBLGeneral_1 + stInfo.lStep;        break; // gripper preunload
		case 1:	lCurrentPos = m_lBLGeneral_2 + stInfo.lStep;        break;	//Gripper Unload
		case 2:	lCurrentPos = m_lBLGeneral_3 + stInfo.lStep;        break;	//Gripper barcode
		case 3:	lCurrentPos = m_lBLGeneral_4 + stInfo.lStep;        break;	//Gripper load magazine
		case 4:	lCurrentPos = m_lBLGeneral_5 + stInfo.lStep;        break;	//Gripper Ready
		case 7:	lCurrentPos = m_lBLGeneral_8 + stInfo.lStep;        break;	//Gripper Buffer Unload
		case 12:lCurrentPos = m_lBLGeneral_12 + stInfo.lStep;		break;	//Top1 T
		case 13:lCurrentPos = m_lBLGeneral_13 + stInfo.lStep;		break;	//Top2 T
		case 14:lCurrentPos = m_lBLGeneral_14 + stInfo.lStep;		break;	//Ready T
		case 15:lCurrentPos = m_lBLGeneral_15 + stInfo.lStep;        break;	//Gripper unload magazine
		default: return TRUE;
	}

	//Check limit range
	if ( (stInfo.lPosition >= 12) && (stInfo.lPosition <= 14) )
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MAX_DISTANCE);	

		if ( (lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos) )
		{
			Theta_SlowMoveTo(lCurrentPos);

			switch(stInfo.lPosition)
			{
				case 12:	m_lBLGeneral_12 = lCurrentPos;   break;
				case 13:	m_lBLGeneral_13 = lCurrentPos;   break;
				case 14:	m_lBLGeneral_14 = lCurrentPos;   break;
			}
		}

	}
	else if ((stInfo.lPosition < 5) || (stInfo.lPosition == 7) || (stInfo.lPosition == 15))
	{
		if ( IsFrameLevel2() == FALSE )
		{
			SetErrorMessage("Frame Platform 2 is not down");
			LONG lReturn = HmiMessage_Red_Yellow("Frame Platform 2 is not down; continue?", "BL Gripper Setup", glHMI_MBX_CONTINUESTOP|0x80000000);
			if (lReturn == glHMI_STOP)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A43
		{
			lMinPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_LOWER, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_LOWER, MS896A_CFG_CH_MAX_DISTANCE);	
		}
		else
		{
			lMinPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER2, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER2, MS896A_CFG_CH_MAX_DISTANCE);	
		}

		if ( stInfo.lPosition == 4 )
		{
			lMaxPos = BL_GRIPPER_MAX_POS_LIMIT;
		}

		if ( (lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos) )
		{
			X2_MoveTo(lCurrentPos);

			switch(stInfo.lPosition)
			{
				case 0:	m_lBLGeneral_1 = lCurrentPos;   break;
				case 1:	m_lBLGeneral_2 = lCurrentPos;	break;
				case 2:	m_lBLGeneral_3 = lCurrentPos;	break;
				case 3:	m_lBLGeneral_4 = lCurrentPos;	break;
				case 4:	m_lBLGeneral_5 = lCurrentPos;	break;
				case 7:	m_lBLGeneral_8 = lCurrentPos;	break;
				case 15:m_lBLGeneral_15 = lCurrentPos;	break;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::MoveNegPosition(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;

	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);


	switch(stInfo.lPosition)
	{
		case 0:	lCurrentPos = m_lBLGeneral_1 - stInfo.lStep;    break;  // gripper preunload
		case 1:	lCurrentPos = m_lBLGeneral_2 - stInfo.lStep;	break;	// Gripper Unload
		case 2:	lCurrentPos = m_lBLGeneral_3 - stInfo.lStep;	break;	// Gripper barcode
		case 3:	lCurrentPos = m_lBLGeneral_4 - stInfo.lStep;	break;	// Gripper load magazine
		case 4:	lCurrentPos = m_lBLGeneral_5 - stInfo.lStep;	break;	// Gripper Ready
		case 7:	lCurrentPos = m_lBLGeneral_8 - stInfo.lStep;	break;	// Gripper Buffer Unload
		case 12:	lCurrentPos = m_lBLGeneral_12 - stInfo.lStep;	break;	// LOAD1 T
		case 13:	lCurrentPos = m_lBLGeneral_13 - stInfo.lStep;	break;	// LOAD2 T
		case 14:	lCurrentPos = m_lBLGeneral_14 - stInfo.lStep;	break;	// READY T
		case 15:	lCurrentPos = m_lBLGeneral_15 - stInfo.lStep;	break;	// Gripper unload magazine
		default: return TRUE;
	}

	//Check limit range
	if ( (stInfo.lPosition >= 12) && (stInfo.lPosition <= 14) )	//v4.59A43	//v4.59A45 bugfix
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MAX_DISTANCE);	

		if ( (lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos) )
		{
			Theta_SlowMoveTo(lCurrentPos);

			switch(stInfo.lPosition)
			{
				case 12:	m_lBLGeneral_12 = lCurrentPos;   break;
				case 13:	m_lBLGeneral_13 = lCurrentPos;   break;
				case 14:	m_lBLGeneral_14 = lCurrentPos;   break;
			}
		}

	}
	else if ( (stInfo.lPosition < 5)  || (stInfo.lPosition == 7) || (stInfo.lPosition == 15))
	{
		if ( IsFrameLevel() == FALSE )
		{
			SetErrorMessage("Frame Platform is not down");
			LONG lReturn = HmiMessage_Red_Yellow("Frame Platform is not down; continue?", "BL Gripper Setup", glHMI_MBX_CONTINUESTOP|0x80000000);
			if (lReturn == glHMI_STOP)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A43
		{
			lMinPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_MAX_DISTANCE);	
		}
		else
		{
			lMinPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_MAX_DISTANCE);
		}
   
		if ( stInfo.lPosition == 4 )
		{
			lMaxPos = BL_GRIPPER_MAX_POS_LIMIT;
		}

		if ( (lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos) )
		{
			Upper_MoveTo(lCurrentPos);

			switch(stInfo.lPosition)
			{
				case 0:	m_lBLGeneral_1 = lCurrentPos;   break;
				case 1:	m_lBLGeneral_2 = lCurrentPos;	break;
				case 2:	m_lBLGeneral_3 = lCurrentPos;	break;
				case 3:	m_lBLGeneral_4 = lCurrentPos;	break;
				case 4:	m_lBLGeneral_5 = lCurrentPos;	break;
				case 7:	m_lBLGeneral_8 = lCurrentPos;	break;
				case 15:m_lBLGeneral_15 = lCurrentPos;	break;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::MoveNegPosition2(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;

	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);


	switch(stInfo.lPosition)
	{
		case 0:	lCurrentPos = m_lBLGeneral_1 - stInfo.lStep;    break;  // gripper preunload
		case 1:	lCurrentPos = m_lBLGeneral_2 - stInfo.lStep;	break;	// Gripper Unload
		case 2:	lCurrentPos = m_lBLGeneral_3 - stInfo.lStep;	break;	// Gripper barcode
		case 3:	lCurrentPos = m_lBLGeneral_4 - stInfo.lStep;	break;	// Gripper load magazine
		case 4:	lCurrentPos = m_lBLGeneral_5 - stInfo.lStep;	break;	// Gripper Ready
		case 7:	lCurrentPos = m_lBLGeneral_8 - stInfo.lStep;	break;	// Gripper Buffer Unload
		case 12:	lCurrentPos = m_lBLGeneral_12 - stInfo.lStep;	break;	// LOAD1 T
		case 13:	lCurrentPos = m_lBLGeneral_13 - stInfo.lStep;	break;	// LOAD2 T
		case 14:	lCurrentPos = m_lBLGeneral_14 - stInfo.lStep;	break;	// READY T
		case 15:	lCurrentPos = m_lBLGeneral_15 - stInfo.lStep;	break;	// Gripper unload magazine
		default: return TRUE;
	}

	//Check limit range
	if ( (stInfo.lPosition >= 12) && (stInfo.lPosition <= 14) )	//v4.59A43
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA, MS896A_CFG_CH_MAX_DISTANCE);	

		if ( (lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos) )
		{
			Theta_SlowMoveTo(lCurrentPos);

			switch(stInfo.lPosition)
			{
				case 12:	m_lBLGeneral_12 = lCurrentPos;   break;
				case 13:	m_lBLGeneral_13 = lCurrentPos;   break;
				case 14:	m_lBLGeneral_14 = lCurrentPos;   break;
			}
		}

	}
	else if ((stInfo.lPosition < 5)  || (stInfo.lPosition == 7) || (stInfo.lPosition == 15))
	{
		if ( IsFrameLevel2() == FALSE )
		{
			SetErrorMessage("Frame Platform 2 is not down");
			LONG lReturn = HmiMessage_Red_Yellow("Frame Platform 2 is not down; continue?", "BL Gripper Setup", glHMI_MBX_CONTINUESTOP|0x80000000);
			if (lReturn == glHMI_STOP)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A43
		{
			lMinPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_LOWER, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos  = GetChannelInformation(MS896A_CFG_CH_BINLOADER_LOWER, MS896A_CFG_CH_MAX_DISTANCE);	
		}
		else
		{
			lMinPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER2, MS896A_CFG_CH_MIN_DISTANCE);								
			lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINGRIPPER2, MS896A_CFG_CH_MAX_DISTANCE);
		}
     
		if ( stInfo.lPosition == 4 )
		{
			lMaxPos = BL_GRIPPER_MAX_POS_LIMIT;
		}

		if ( (lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos) )
		{
			X2_MoveTo(lCurrentPos);

			switch(stInfo.lPosition)
			{
				case 0:	m_lBLGeneral_1 = lCurrentPos;   break;
				case 1:	m_lBLGeneral_2 = lCurrentPos;	break;
				case 2:	m_lBLGeneral_3 = lCurrentPos;	break;
				case 3:	m_lBLGeneral_4 = lCurrentPos;	break;
				case 4:	m_lBLGeneral_5 = lCurrentPos;	break;
				case 7:	m_lBLGeneral_8 = lCurrentPos;	break;
				case 15:m_lBLGeneral_15 = lCurrentPos;	break;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::CancelSetup(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	//Restore HMI variable
    m_lBLGeneral_1      = m_lPreUnloadPos_X;
	m_lBLGeneral_2		= m_lUnloadPos_X;		
	m_lBLGeneral_3		= m_lBarcodePos_X;	
	m_lBLGeneral_4		= m_lLoadMagPos_X;		
	m_lBLGeneral_5		= m_lReadyPos_X;
	m_lBLGeneral_6      = m_lBTUnloadPos_X;
	m_lBLGeneral_7      = m_lBTUnloadPos_Y;
	m_lBLGeneral_8		= m_lBufferUnloadPos_X;
	m_lBLGeneral_9		= m_lBTBarcodePos_X;
	m_lBLGeneral_10		= m_lBTBarcodePos_Y;
	m_lBLGeneral_11		= m_lAutoLineUnloadZ;	//v4.56A1
/*
	//v4.59A43
	m_lBLGeneral_12		= m_lLoadPos1_T;
	m_lBLGeneral_13		= m_lLoadPos2_T;
	m_lBLGeneral_14		= m_lReadyPos_T;
*/
	m_lBLGeneral_15		= m_lUnloadMagPos_X;	
	//Move Gripper back to Home pos
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		if ( IsDualBufferUpperExist() )
		{
			SetErrorMessage("BL Frame exists in Upper Buffer");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
		}
		else if ( IsDualBufferLowerExist() )
		{
			SetBufferLevel(FALSE);
			Sleep(300);

			if ( IsDualBufferLevelDown() == FALSE )
			{
				SetErrorMessage("BL Frame exists in Lower Buffer");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
			}
			else
			{
				Upper_MoveTo(0);
			}
		}
		else
		{
            Upper_MoveTo(0);
		}
	}
	else
	{
		Upper_MoveTo(0);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::CancelSetup2(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	//Restore HMI variable
    m_lBLGeneral_1      = m_lPreUnloadPos_X2;
	m_lBLGeneral_2		= m_lUnloadPos_X2;		
	m_lBLGeneral_3		= m_lBarcodePos_X2;	
	m_lBLGeneral_4		= m_lLoadMagPos_X2;		
	m_lBLGeneral_5		= m_lReadyPos_X2;
	m_lBLGeneral_6      = m_lBTUnloadPos_X2;
	m_lBLGeneral_7      = m_lBTUnloadPos_Y2;
	m_lBLGeneral_8		= m_lBufferUnloadPos_X2;

	//v4.59A43
	m_lBLGeneral_12		= m_lLoadPos1_T;
	m_lBLGeneral_13		= m_lLoadPos2_T;
	m_lBLGeneral_14		= m_lReadyPos_T;

	m_lBLGeneral_15		= m_lUnloadMagPos_X2;	
	//Move Gripper back to Home pos
	X2_MoveTo(0);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::ConfirmSetup(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	if (m_lPreUnloadPos_X != m_lBLGeneral_1)
	{
		m_lPreUnloadPos_X = m_lBLGeneral_1;
		LogItems(BIN_GRIPPER_PREUNLOAD_POS);
	}
	else if (m_lUnloadPos_X != m_lBLGeneral_2)
	{
		m_lUnloadPos_X = m_lBLGeneral_2;
		LogItems(BIN_GRIPPER_UNLOAD_POS);
	}
	else if (m_lBarcodePos_X != m_lBLGeneral_3)
	{
		m_lBarcodePos_X = m_lBLGeneral_3;
		LogItems(BIN_GRIPPER_BC_POS);
	}
	else if (m_lLoadMagPos_X != m_lBLGeneral_4)
	{
		m_lLoadMagPos_X = m_lBLGeneral_4;
		LogItems(BIN_GRIPPER_LOAD_MAG_POS);
	}
	else if (m_lUnloadMagPos_X != m_lBLGeneral_15)
	{
		m_lUnloadMagPos_X = m_lBLGeneral_15;
		LogItems(BIN_GRIPPER_UNLOAD_MAG_POS);
	}
	else if (m_lReadyPos_X != m_lBLGeneral_5)
	{
		m_lReadyPos_X = m_lBLGeneral_5;
		LogItems(BIN_GRIPPER_READY_POS);
	}
/*
	//MS50
	else if (m_lLoadPos1_T != m_lBLGeneral_12)
	{
		m_lLoadPos1_T = m_lBLGeneral_12;
	}
	else if (m_lLoadPos2_T != m_lBLGeneral_13)
	{
		m_lLoadPos2_T = m_lBLGeneral_13;
	}
	else if (m_lReadyPos_T != m_lBLGeneral_14)
	{
		m_lReadyPos_T = m_lBLGeneral_14;
	}
*/	

	//Update variable
    m_lPreUnloadPos_X   = m_lBLGeneral_1;
	m_lUnloadPos_X		= m_lBLGeneral_2;		
	m_lBarcodePos_X		= m_lBLGeneral_3;	
	m_lLoadMagPos_X		= m_lBLGeneral_4;
	m_lUnloadMagPos_X	= m_lBLGeneral_15;
	m_lReadyPos_X		= m_lBLGeneral_5;
	m_lBufferUnloadPos_X = m_lBLGeneral_8;
/*
	//MS50 config
	m_lLoadPos1_T		= m_lBLGeneral_12;
	m_lLoadPos2_T		= m_lBLGeneral_13;
	m_lReadyPos_T		= m_lBLGeneral_14;
*/
    if ( (lPosition >= 12) && (lPosition <= 14) )		//v4.59A43
	{
	}
    else if ( lPosition==5 || lPosition==6 )
    {
        GetBinTableEncoder(&m_lBLGeneral_6, &m_lBLGeneral_7);

		if (m_lBTUnloadPos_X != m_lBLGeneral_6)
		{
			m_lBTUnloadPos_X = m_lBLGeneral_6;
			LogItems(BT_UNLOAD_POS_X);
		}

		if (m_lBTUnloadPos_Y != m_lBLGeneral_7)
		{
			m_lBTUnloadPos_Y = m_lBLGeneral_7;
			LogItems(BT_UNLOAD_POS_Y);
		}

        m_lBTUnloadPos_X    = m_lBLGeneral_6;
        m_lBTUnloadPos_Y    = m_lBLGeneral_7;
    }
    else if( lPosition==9 || lPosition==10 )	//v4.39T7	//Nichia MS100+
    {
        GetBinTableEncoder(&m_lBLGeneral_9, &m_lBLGeneral_10);

		if (m_lBTBarcodePos_X != m_lBLGeneral_9)
		{
			m_lBTBarcodePos_X = m_lBLGeneral_9;
			//LogItems(BT_UNLOAD_POS_X);
		}

		if (m_lBTBarcodePos_Y != m_lBLGeneral_10)
		{
			m_lBTBarcodePos_Y = m_lBLGeneral_10;
			//LogItems(BT_UNLOAD_POS_Y);
		}

        m_lBTBarcodePos_X    = m_lBLGeneral_9;
        m_lBTBarcodePos_Y    = m_lBLGeneral_10;
    }
	else
	{
		//Move Gripper back to Home pos
//		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
		{
			if ( IsDualBufferUpperExist() )
			{
				SetErrorMessage("BL Frame exists in Upper Buffer");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
			}
			else if ( IsDualBufferLowerExist() )
			{
				SetBufferLevel(FALSE);
				Sleep(300);

				if ( IsDualBufferLevelDown() == FALSE )
				{
					SetErrorMessage("BL Frame exists in Lower Buffer");
					SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
				}
				else
				{
					Upper_MoveTo(0);
				}
			}
			else
			{
				Upper_MoveTo(0);
			}
		}
	}

	SaveData();

	SendGripperEvent_7050(m_lPreUnloadPos_X, m_lUnloadPos_X, m_lLoadMagPos_X, m_lBarcodePos_X,
						  m_lBarcodePos_X, m_lBTUnloadPos_X, m_lBTUnloadPos_Y);
	svMsg.InitMessage(sizeof(BOOL),		&bReturn);
	return 1;
}

LONG CBinLoader::ConfirmSetup2(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	if (m_lPreUnloadPos_X2 != m_lBLGeneral_1)
	{
		m_lPreUnloadPos_X2 = m_lBLGeneral_1;
		LogItems(BIN_GRIPPER_PREUNLOAD_POS);
	}
	else if (m_lUnloadPos_X2 != m_lBLGeneral_2)
	{
		m_lUnloadPos_X2 = m_lBLGeneral_2;
		LogItems(BIN_GRIPPER_UNLOAD_POS);
	}
	else if (m_lBarcodePos_X2 != m_lBLGeneral_3)
	{
		m_lBarcodePos_X2 = m_lBLGeneral_3;
		LogItems(BIN_GRIPPER_BC_POS);
	}
	else if (m_lLoadMagPos_X2 != m_lBLGeneral_4)
	{
		m_lLoadMagPos_X2 = m_lBLGeneral_4;
		LogItems(BIN_GRIPPER_LOAD_MAG_POS);
	}
	else if (m_lUnloadMagPos_X2 != m_lBLGeneral_15)
	{
		m_lUnloadMagPos_X2 = m_lBLGeneral_15;
		LogItems(BIN_GRIPPER_LOAD_MAG_POS);
	}
	else if (m_lReadyPos_X2 != m_lBLGeneral_5)
	{
		m_lReadyPos_X2 = m_lBLGeneral_5;
		LogItems(BIN_GRIPPER_READY_POS);
	}
	else if (m_lLoadPos1_T != m_lBLGeneral_12)
	{
		m_lLoadPos1_T = m_lBLGeneral_12;
	}
	else if (m_lLoadPos2_T != m_lBLGeneral_13)
	{
		m_lLoadPos2_T = m_lBLGeneral_13;
	}
	else if (m_lReadyPos_T != m_lBLGeneral_14)
	{
		m_lReadyPos_T = m_lBLGeneral_14;
	}
	
	//Update variable
    m_lPreUnloadPos_X2		= m_lBLGeneral_1;
	m_lUnloadPos_X2			= m_lBLGeneral_2;		
	m_lBarcodePos_X2		= m_lBLGeneral_3;	
	m_lLoadMagPos_X2		= m_lBLGeneral_4;
	m_lUnloadMagPos_X2		= m_lBLGeneral_15;	
	m_lReadyPos_X2			= m_lBLGeneral_5;
	m_lBufferUnloadPos_X2	= m_lBLGeneral_8;

	//MS50 config
	m_lLoadPos1_T		= m_lBLGeneral_12;
	m_lLoadPos2_T		= m_lBLGeneral_13;
	m_lReadyPos_T		= m_lBLGeneral_14;


    if ( (lPosition >= 12) && (lPosition <= 14) )		//v4.59A43
	{
	}
    else if( lPosition==5 || lPosition==6 )
    {
        GetBinTable2Encoder(&m_lBLGeneral_6, &m_lBLGeneral_7);

		if (m_lBTUnloadPos_X2 != m_lBLGeneral_6)
		{
			m_lBTUnloadPos_X2 = m_lBLGeneral_6;
			LogItems(BT_UNLOAD_POS_X);
		}

		if (m_lBTUnloadPos_Y2 != m_lBLGeneral_7)
		{
			m_lBTUnloadPos_Y2 = m_lBLGeneral_7;
			LogItems(BT_UNLOAD_POS_Y);
		}

        m_lBTUnloadPos_X2    = m_lBLGeneral_6;
        m_lBTUnloadPos_Y2    = m_lBLGeneral_7;
    }
	else
	{
		X2_MoveTo(0);
	}

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// commands for magazine setup
LONG CBinLoader::SelectMagazine(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	if ((lPosition < 0) || (lPosition >= MS_BL_MGZN_NUM))
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_lSelMagazineID = lPosition;

    m_lBLGeneral_1  = m_stMgznRT[lPosition].m_lTopLevel;
    m_lBLGeneral_2  = m_stMgznRT[lPosition].m_lMidPosnY;
    m_lBLGeneral_3  = m_stMgznRT[lPosition].m_lSlotPitch;
    m_lBLGeneral_4  = m_stMgznRT[lPosition].m_lNoOfSlots;
    m_lBLGeneral_5  = m_lReadyPosY;
    m_lBLGeneral_6  = m_lReadyLvlZ;

	//Show Slot Pitch in mm
	m_dSlotPitch	= (DOUBLE)(m_lBLGeneral_3) * m_dZRes;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CBinLoader::SelectMagazine2(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	//v3.82
	if ((lPosition < 0) || (lPosition >= MS_BL_MGZN_NUM))
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

    //m_lBLGeneral_1  = m_stMgznRT[lPosition].m_lTopLevel2;
    //m_lBLGeneral_2  = m_stMgznRT[lPosition].m_lMidPosnY2;
    m_lBLGeneral_3  = m_stMgznRT[lPosition].m_lSlotPitch;
    m_lBLGeneral_4  = m_stMgznRT[lPosition].m_lNoOfSlots;
    m_lBLGeneral_5  = m_lReadyPosY;
    m_lBLGeneral_6  = m_lReadyLvlZ;

	//Show Slot Pitch in mm
	m_dSlotPitch	= (DOUBLE)(m_lBLGeneral_3) * m_dZRes;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CBinLoader::OSRAMMagazineUnload(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "OSRAM"|| pApp->GetProductLine() != "Germany" || pApp->GetCustomerName() != "BSE")
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);	//v4.50A18
		return TRUE;
	}

	if (!CheckSafeToMove())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);	//v4.50A18
		return TRUE;
	}

//	Y_MoveTo(-1600000, SFM_NOWAIT);
	m_lBLGeneral_TmpA = -1600000;

//	Y_Sync(TRUE);	//v4.50A23

	//v4.50A17
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinLoader::ChangeMagazineIndex(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	LONG lPosition = 0;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	if (!CheckSafeToMove())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);	//v4.50A18
		return TRUE;
	}

	//v3.82
	if (!m_bBLOut8MagConfig)
	{
		if (lPosition >= 6)		//8Mag BTM 1/2 not supported!
		{
			SetErrorMessage("8Mag BTM 1&2 not supported in 6Mag mode!");
			SetAlert_Red_Yellow(IDS_BL_8MAG_CONFIG_NOT_SUPPORT);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	switch (lPosition)
	{
		case 0: // magazine top level
			if (Z_MoveTo(m_lBLGeneral_1, SFM_NOWAIT) != gnOK)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			m_lBLGeneral_TmpA = m_lBLGeneral_1;
			break;

		case 1:	// magazine center position
//			Y_MoveTo(m_lBLGeneral_2, SFM_NOWAIT);
			m_lBLGeneral_TmpA = m_lBLGeneral_2;
			break;

		case 2:	// slot pitch
			m_lBLGeneral_TmpA = m_lBLGeneral_3;
			break;

		case 3:	// total slots
			m_lBLGeneral_TmpA = m_lBLGeneral_4;
			break;

		case 4:	// ready y
//			Y_MoveTo(m_lBLGeneral_5, SFM_NOWAIT);
			m_lBLGeneral_TmpA = m_lBLGeneral_5;
			break;

		case 5:	// ready z
			if (Z_MoveTo(m_lBLGeneral_6, SFM_NOWAIT) != gnOK)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			m_lBLGeneral_TmpA = m_lBLGeneral_6;
			break;

		case 6:	//autoline Unload Z		//v4.57A3
			if (Z_MoveTo(m_lBLGeneral_11, SFM_NOWAIT) != gnOK)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			m_lBLGeneral_TmpA = m_lBLGeneral_11;
			break;
	}

	//v4.50A23	//Osram Germany CEMark machine
	switch (lPosition)
	{
	case 0:
	case 5:
	case 6:		//v4.57A3
		if (Z_Sync(TRUE) != gnOK)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
		break;

	case 1:
	case 4:
//		Y_Sync(TRUE);	
		break;
		break;
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CBinLoader::ChangeMagazineIndex2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	LONG lPosition;

	svMsg.GetMsg(sizeof(LONG), &lPosition);


	if ( (IsFrameOutOfMgz() == TRUE) && (m_bNoSensorCheck == FALSE) )
	{
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		SetErrorMessage("BL Frame is out of magazine 2");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if ( IsMagazine2SafeToMove() == FALSE )
	{
		SetErrorMessage("Gripper2 is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if ( IsElevatorCoverOpen() == TRUE )
	{
		if ( CheckElevatorCover() == FALSE )
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}
/*
	if (IsCoverOpen() == TRUE)
	{
		if (CheckCover() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}
*/
	//v3.82
	if (!m_bBLOut8MagConfig)
	{
		if (lPosition >= 6)		//8Mag BTM 1/2 not supported!
		{
			SetErrorMessage("8Mag BTM 1&2 not supported in 6Mag mode!");
			SetAlert_Red_Yellow(IDS_BL_8MAG_CONFIG_NOT_SUPPORT);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	switch(lPosition)
	{
	case 0: // magazine top level
		if (Z_MoveTo(m_lBLGeneral_1, SFM_NOWAIT) != gnOK)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
		m_lBLGeneral_TmpA = m_lBLGeneral_1;
		break;

	case 1:	// magazine center position
//		Y_MoveTo(m_lBLGeneral_2, SFM_NOWAIT);
		m_lBLGeneral_TmpA = m_lBLGeneral_2;
		break;

	case 2:	// slot pitch
		m_lBLGeneral_TmpA = m_lBLGeneral_3;
		break;

	case 3:	// total slots
		m_lBLGeneral_TmpA = m_lBLGeneral_4;
		break;

	case 4:	// ready y
//		Y_MoveTo(m_lBLGeneral_5, SFM_NOWAIT);
		m_lBLGeneral_TmpA = m_lBLGeneral_5;
		break;

	case 5:	// ready z
		if (Z_MoveTo(m_lBLGeneral_6, SFM_NOWAIT) != gnOK)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
		m_lBLGeneral_TmpA = m_lBLGeneral_6;
		break;
	}

	//v4.50A23	//Osram Germany CEMark machine
	switch (lPosition)
	{
	case 0:
	case 5:
		if (Z_Sync(TRUE)  != gnOK)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
		break;

	case 1:
	case 4:
//		Y_Sync(TRUE);
		break;
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CBinLoader::KeyInValue(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;
	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	BOOL bReturn = FALSE;

	if (!CheckSafeToMove())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//Check KeyIn Positon Limit
    // get magazine z limit
	if (stInfo.lPosition==0 || stInfo.lPosition==5 ) // z limit
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);
	}
	else if( stInfo.lPosition==1 || stInfo.lPosition==4 ) // y limit
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_MIN_DISTANCE);
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_MAX_DISTANCE);
	}
    else if( stInfo.lPosition==3 )
    {
		lMinPos = 0;

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetCustomerName() == "Cree") && (m_lOMRT == BL_MODE_D) )
		{
			lMaxPos = MS_BL_MAX_MGZN_SLOT;		//v4.19	//Cree HuiZhou
		}
		else
		{
			lMaxPos = MS_BL_MGZN_SLOT;
		}
    }
    else if( stInfo.lPosition==2 )
    {
		lMinPos = MS_BL_MGZ_MIN_PITCH;
		lMaxPos = MS_BL_MGZ_MAX_PITCH;
    }

	if (stInfo.lStep < lMinPos) 
	{
		stInfo.lStep = lMinPos;
	}

	if (stInfo.lStep > lMaxPos)
	{
		stInfo.lStep = lMaxPos;
	}

	switch(stInfo.lPosition)
	{
		case 0: // z top level
			m_lBLGeneral_1 = stInfo.lStep;
			if (Z_MoveTo(m_lBLGeneral_1, SFM_NOWAIT) != gnOK)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			break;

		case 1:	// y position
			m_lBLGeneral_2 = stInfo.lStep;
//			Y_MoveTo(m_lBLGeneral_2, SFM_NOWAIT);
			break;

		case 2:	// pitch
			m_lBLGeneral_3 = stInfo.lStep;
			break;

		case 3:	// total slots
			m_lBLGeneral_4 = stInfo.lStep;
			break;

		case 4:	// y Ready
			m_lBLGeneral_5 = stInfo.lStep;
//			Y_MoveTo(m_lBLGeneral_5, SFM_NOWAIT);
			break;

		case 5:	// z ready
			m_lBLGeneral_6 = stInfo.lStep;
			if (Z_MoveTo(m_lBLGeneral_6, SFM_NOWAIT) != gnOK)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			break;

		case 6:	//AL Unload Z	//autoline1
			m_lBLGeneral_11 = stInfo.lStep;
			if (Z_MoveTo(m_lBLGeneral_11, SFM_NOWAIT) != gnOK)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			break;
	}

	m_dSlotPitch = (DOUBLE)(m_lBLGeneral_3) * m_dZRes;

	//v4.50A23	//Osram Germany CEMark machine
	switch (stInfo.lPosition)
	{
	case 0:
	case 5:
	case 6:
		if (Z_Sync(TRUE) != gnOK)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
		break;

	case 1:
	case 4:
//		Y_Sync(TRUE);
		break;
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::IncreaseValue(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;

	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	BOOL bReturn = FALSE;
	switch(stInfo.lPosition)
	{
		case 0:	lCurrentPos = m_lBLGeneral_1 + stInfo.lStep; break;		// z top level		
		case 1:	lCurrentPos = m_lBLGeneral_2 + stInfo.lStep; break;		// y position
		case 2:	lCurrentPos = m_lBLGeneral_3 + stInfo.lStep; break;		// pitch
		case 3:	lCurrentPos = m_lBLGeneral_4 + stInfo.lStep; break;		// total slots
		case 4:	lCurrentPos = m_lBLGeneral_5 + stInfo.lStep; break;		// y Ready
		case 5:	lCurrentPos = m_lBLGeneral_6 + stInfo.lStep; break;		// z ready
		case 6:	lCurrentPos = m_lBLGeneral_11 + stInfo.lStep; break;	// AL Unload Z
	}

	//Check limit range
	if (stInfo.lPosition==0 || stInfo.lPosition==5 || stInfo.lPosition==6) // z limit
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);
	}
	else if( stInfo.lPosition==1 || stInfo.lPosition==4 ) // y limit
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_MIN_DISTANCE);
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_MAX_DISTANCE);
	}
    else if( stInfo.lPosition==3 )
    {
		lMinPos = 0;

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetCustomerName() == "Cree") && (m_lOMRT == BL_MODE_D) )
		{
			lMaxPos = MS_BL_MAX_MGZN_SLOT;		//v4.19	//Cree HuiZhou
		}
		else
		{
			lMaxPos = MS_BL_MGZN_SLOT;
		}
    }
    else if( stInfo.lPosition==2 )
    {
		lMinPos = MS_BL_MGZ_MIN_PITCH;
		lMaxPos = MS_BL_MGZ_MAX_PITCH;
    }

	if (!CheckSafeToMove())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( stInfo.lPosition==0 || stInfo.lPosition==5 || stInfo.lPosition==6)
	{
		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			if (Z_MoveTo(lCurrentPos) != gnOK)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			switch(stInfo.lPosition)
			{
				case 0:     m_lBLGeneral_1 = lCurrentPos;   break;
				case 5:	    m_lBLGeneral_6 = lCurrentPos;	break;
				case 6:	    m_lBLGeneral_11 = lCurrentPos;	break;	//autoline1
			}
		}
	}
	else
	{
        if( stInfo.lPosition==1 || stInfo.lPosition==4 )
        {
		    if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		    {
//			    Y_MoveTo(lCurrentPos);
                switch( stInfo.lPosition )
                {
					case 1:		m_lBLGeneral_2 = lCurrentPos;	break;
					case 4:		m_lBLGeneral_5 = lCurrentPos;	break;
                }
		    }
        }
        else
        {
		    if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		    {
                switch( stInfo.lPosition )
                {
					case 2:		m_lBLGeneral_3 = lCurrentPos;	break;
					case 3:		m_lBLGeneral_4 = lCurrentPos;	break;
                }
		    }
        }
	}

	m_dSlotPitch = (DOUBLE)(m_lBLGeneral_3) * m_dZRes;

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::DecreaseValue(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;

	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	BOOL bReturn = FALSE;

	switch(stInfo.lPosition)
	{
		case 0:     lCurrentPos = m_lBLGeneral_1 - stInfo.lStep;    break;  // gripper preunload
		case 1:	    lCurrentPos = m_lBLGeneral_2 - stInfo.lStep;	break;	// Gripper Unload
		case 2:		lCurrentPos = m_lBLGeneral_3 - stInfo.lStep;	break;	// Gripper barcode
		case 3:		lCurrentPos = m_lBLGeneral_4 - stInfo.lStep;	break;	// Gripper load
		case 4:		lCurrentPos = m_lBLGeneral_5 - stInfo.lStep;	break;	// Gripper Ready
		case 5:		lCurrentPos = m_lBLGeneral_6 - stInfo.lStep;	break;	// bin table x
		case 6:		lCurrentPos = m_lBLGeneral_11 - stInfo.lStep;	break;	// AL Unload Z
	}

	//Check limit range
	if (stInfo.lPosition==0 || stInfo.lPosition==5 || stInfo.lPosition==6) // z limit
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);
	}
	else if( stInfo.lPosition==1 || stInfo.lPosition==4 ) // y limit
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_MIN_DISTANCE);
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_MAX_DISTANCE);
	}
    else if( stInfo.lPosition==3 )
    {
		lMinPos = 0;
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetCustomerName() == "Cree") && (m_lOMRT == BL_MODE_D) )
		{
			lMaxPos = MS_BL_MAX_MGZN_SLOT;		//v4.19	//Cree HuiZhou
		}
		else
		{
			lMaxPos = MS_BL_MGZN_SLOT;
		}
    }
    else if( stInfo.lPosition==2 )
    {
		lMinPos = MS_BL_MGZ_MIN_PITCH;
		lMaxPos = MS_BL_MGZ_MAX_PITCH;
    }

	if (!CheckSafeToMove())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( stInfo.lPosition==0 || stInfo.lPosition==5 || stInfo.lPosition==6)
	{
		if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		{
			if (Z_MoveTo(lCurrentPos, SFM_NOWAIT) != gnOK)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			switch(stInfo.lPosition)
			{
				case 0:     m_lBLGeneral_1 = lCurrentPos;   break;
				case 5:	    m_lBLGeneral_6 = lCurrentPos;	break;
				case 6:	    m_lBLGeneral_11 = lCurrentPos;	break;		//autoline1
			}
		}
	}
	else
	{
        if( stInfo.lPosition==1 || stInfo.lPosition==4 )
        {
		    if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		    {
//			    Y_MoveTo(lCurrentPos, SFM_NOWAIT);
                switch( stInfo.lPosition )
                {
					case 1:		m_lBLGeneral_2 = lCurrentPos;	break;
					case 4:		m_lBLGeneral_5 = lCurrentPos;	break;
                }
		    }
        }
        else
        {
		    if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
		    {
                switch( stInfo.lPosition )
                {
					case 2:		m_lBLGeneral_3 = lCurrentPos;	break;
					case 3:		m_lBLGeneral_4 = lCurrentPos;	break;
                }
		    }
        }
	}

	m_dSlotPitch = (DOUBLE)(m_lBLGeneral_3) * m_dZRes;

	//v4.50A23
//	Y_Sync(TRUE);
	if (Z_Sync(TRUE) == gnOK)
	{
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::CancelMagazineSetup(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	//Restore HMI variable
    m_lBLGeneral_1  = m_stMgznRT[lPosition].m_lTopLevel;
    m_lBLGeneral_2  = m_stMgznRT[lPosition].m_lMidPosnY;
    m_lBLGeneral_3  = m_stMgznRT[lPosition].m_lSlotPitch;
    m_lBLGeneral_4  = m_stMgznRT[lPosition].m_lNoOfSlots;
    m_lBLGeneral_5  = m_lReadyPosY;
    m_lBLGeneral_6  = m_lReadyLvlZ;
    m_lBLGeneral_11 = m_lAutoLineUnloadZ;	//autoline1

	//Show Slot Pitch in mm
	m_dSlotPitch	= (DOUBLE)(m_lBLGeneral_3) * m_dZRes;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::CancelMagazineSetup2(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	//Restore HMI variable
    //m_lBLGeneral_1  = m_stMgznRT[lPosition].m_lTopLevel2;
    //m_lBLGeneral_2  = m_stMgznRT[lPosition].m_lMidPosnY2;
    m_lBLGeneral_3  = m_stMgznRT[lPosition].m_lSlotPitch;
    m_lBLGeneral_4  = m_stMgznRT[lPosition].m_lNoOfSlots;
    m_lBLGeneral_5  = m_lReadyPosY;
    m_lBLGeneral_6  = m_lReadyLvlZ;

	//Show Slot Pitch in mm
	m_dSlotPitch	= (DOUBLE)(m_lBLGeneral_3) * m_dZRes;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::ConfirmMagazineSetup(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	//v3.82
	if (!m_bBLOut8MagConfig)
	{
		if (lPosition >= 6)		//8Mag BTM 1/2 not supported!
		{
			SetErrorMessage("8Mag BTM 1&2 not supported when CONFIRM BL-Mag Setup!");
			SetAlert_Red_Yellow(IDS_BL_8MAG_CONFIG_NOT_SUPPORT);
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	m_lSetupMagPos						= lPosition;

	if (m_stMgznRT[lPosition].m_lTopLevel != m_lBLGeneral_1)
	{
		m_stMgznRT[lPosition].m_lTopLevel = m_lBLGeneral_1;
		LogItems(BL_MAGAZINE_TOP_SLOT_POS);	
	}
	else if (m_stMgznRT[lPosition].m_lMidPosnY  != m_lBLGeneral_2)
	{
		m_stMgznRT[lPosition].m_lMidPosnY  = m_lBLGeneral_2;
		LogItems(BL_MAGAZINE_CENTER_Y_POS);
	}
	else if (m_stMgznRT[lPosition].m_lSlotPitch != m_lBLGeneral_3)
	{
		m_stMgznRT[lPosition].m_lSlotPitch = m_lBLGeneral_3;
		LogItems(BL_MAGAZINE_SLOT_PITCH);
	}
	else if (m_lReadyPosY != m_lBLGeneral_5)
	{
		m_lReadyPosY = m_lBLGeneral_5;
		LogItems(ELEVATOR_Y_POS);
	}
	else if (m_lReadyLvlZ != m_lBLGeneral_6)
	{
		m_lReadyLvlZ = m_lBLGeneral_6;
		LogItems(ELEVATOR_Z_POS);
	}
	else if (m_lAutoLineUnloadZ != m_lBLGeneral_11)		//autoline1
	{
		m_lAutoLineUnloadZ = m_lBLGeneral_11;
		LogItems(ELEVATOR_Z_POS);
	}

	//Update variable
	m_stMgznRT[lPosition].m_lTopLevel   = m_lBLGeneral_1;
    m_stMgznRT[lPosition].m_lMidPosnY   = m_lBLGeneral_2;		
    m_stMgznRT[lPosition].m_lSlotPitch  = m_lBLGeneral_3;	
    m_stMgznRT[lPosition].m_lNoOfSlots  = m_lBLGeneral_4;		
	m_lReadyPosY		                = m_lBLGeneral_5;		
    m_lReadyLvlZ                        = m_lBLGeneral_6;

	SaveData();

    // 7051
   	SendMagazineEvent_7051(lPosition, m_lReadyPosY, m_lReadyLvlZ);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::ConfirmMagazineSetup2(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	//v3.82
	if (!m_bBLOut8MagConfig)
	{
		if (lPosition >= 6)		//8Mag BTM 1/2 not supported!
		{
			SetErrorMessage("8Mag BTM 1&2 not supported when CONFIRM BL-Mag Setup!");
			SetAlert_Red_Yellow(IDS_BL_8MAG_CONFIG_NOT_SUPPORT);
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}


	m_lSetupMagPos = lPosition;

	/*if (m_stMgznRT[lPosition].m_lTopLevel2 != m_lBLGeneral_1)
	{
		m_stMgznRT[lPosition].m_lTopLevel2 = m_lBLGeneral_1;
		LogItems(BL_MAGAZINE_TOP_SLOT_POS);	
	}
	else if (m_stMgznRT[lPosition].m_lMidPosnY2  != m_lBLGeneral_2)
	{
		m_stMgznRT[lPosition].m_lMidPosnY2  = m_lBLGeneral_2;
		LogItems(BL_MAGAZINE_CENTER_Y_POS);
	}*/
	if (m_stMgznRT[lPosition].m_lSlotPitch != m_lBLGeneral_3)
	{
		m_stMgznRT[lPosition].m_lSlotPitch = m_lBLGeneral_3;
		LogItems(BL_MAGAZINE_SLOT_PITCH);
	}
	else if (m_lReadyPosY != m_lBLGeneral_5)
	{
		m_lReadyPosY = m_lBLGeneral_5;
		LogItems(ELEVATOR_Y_POS);
	}
	else if (m_lReadyLvlZ != m_lBLGeneral_6)
	{
		m_lReadyLvlZ = m_lBLGeneral_6;
		LogItems(ELEVATOR_Z_POS);
	}


	//Update variable
	//m_stMgznRT[lPosition].m_lTopLevel2		= m_lBLGeneral_1;
    //m_stMgznRT[lPosition].m_lMidPosnY2		= m_lBLGeneral_2;		
    //m_stMgznRT[lPosition].m_lSlotPitch	= m_lBLGeneral_3;	
    //m_stMgznRT[lPosition].m_lNoOfSlots	= m_lBLGeneral_4;		
	//m_lReadyPosY							= m_lBLGeneral_5;		
    //m_lReadyLvlZ							= m_lBLGeneral_6;

	SaveData();
	// 7051
	SendMagazineEvent_7051(lPosition, m_lReadyPosY, m_lReadyLvlZ);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::UpdateSlotPitch(IPC_CServiceMessage& svMsg)
{
	m_lBLGeneral_3 = (LONG)((m_dSlotPitch / m_dZRes) + 0.5);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

/*
LONG CBinLoader::SetUnloadPosition(IPC_CServiceMessage& svMsg) // not used now
{
	SetBinTableJoystick(FALSE);

	MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y);

	SetBinTableJoystick(TRUE);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}
*/

LONG CBinLoader::CheckSemitekPasswordBC(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	m_bUseBarcode = !m_bUseBarcode;
	if( pApp->GetCustomerName()=="Semitek" )
	{
		int nPass = (int) pApp->GetProfileInt(gszPROFILE_SETTING, _T("SemitekPassword"), 201277);
		CString szPassword;
		szPassword.Format("%d", nPass);
		pApp->UpdateHmiVariableData("szLoginID",		"");
		pApp->UpdateHmiVariableData("szLoginPassword",	"");

		CString szTitle			= _T("User Logon");
		CString szInputUserID	= _T("");

		szTitle = _T("Please Enter YOUR Password");
		bReturn = pApp->HmiStrInputKeyboard(szTitle, "szLoginID", TRUE, szInputUserID);

		if (szInputUserID != szPassword)
			bReturn = FALSE;

		if( bReturn==FALSE )
		{
			m_bUseBarcode = !m_bUseBarcode;
		}
	}

	if( bReturn )
	{
		SaveData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::SaveSetupData(IPC_CServiceMessage& svMsg)
{
	SaveData();
	
	// 7052
	SendBarcodeEvent_7052(m_bUseBarcode, m_lTryLimits, m_lScanRange, m_bUseExtension, m_szExtName);		

 	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


// IPC COMMAND SENT TO BIN TABLE
VOID CBinLoader::SetBinTableJoystick(BOOL bState, BOOL bUseBT2)
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(BOOL), &bState);

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetMouseControlCmd", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bState);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		m_lJsBinTableInUse = 0;	//BT1
	}
	else
	{
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetJoystickCmdWithoutSrCheck", stMsg);
	
		// Get the reply
		m_comClient.ScanReplyForConvID(nConvID, 5000);
		m_comClient.ReadReplyForConvID(nConvID,stMsg);
	}
}


BOOL CBinLoader::MoveBinTableThetaToZero()
{
	IPC_CServiceMessage stMsg;

	LONG nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("T_MoveToZeroCmd_ForBL"), stMsg);
	BL_DEBUGBOX("MoveBinTableThetaToZero");

	// Get the reply
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 60000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	BOOL bReturn=FALSE;
	stMsg.GetMsg(sizeof(BOOL), &bReturn);
	return bReturn;
}

BOOL CBinLoader::MoveBinTable(LONG lXAxis, LONG lYAxis, INT nToUnload, BOOL bWait)
{
	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lWait;
		LONG lUnload;
	} RELPOS;

 	RELPOS stPos;
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stPos.lX = lXAxis;
	stPos.lY = lYAxis;
	
	stPos.lWait = bWait ? 1 : 0;

	stPos.lUnload = (nToUnload == 0) ? 0 : 1;

	stMsg.InitMessage(sizeof(RELPOS), &stPos);

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("XY_MoveToCmd_ForBL"), stMsg);
	BL_DEBUGBOX("XY_MoveToCmd_ForBL");		//v4.22T8

	// Get the reply
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 60000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	BOOL bReturn=FALSE;
	stMsg.GetMsg(sizeof(BOOL), &bReturn);
	return bReturn;
}


BOOL CBinLoader::MoveBinTable2(LONG lXAxis, LONG lYAxis, INT nToUnload, BOOL bWait)
{
	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lWait;
		LONG lUnload;
	} RELPOS;

 	RELPOS stPos;
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stPos.lX = lXAxis;
	stPos.lY = lYAxis;
	if (!bWait)
		stPos.lWait		= 0;
	else
		stPos.lWait		= 1;
	if (nToUnload == 0)				//v4.22T7
		stPos.lUnload	= 0;
	else
		stPos.lUnload	= 1;


	//v4.51A12	//Cree HZ
	//v4.51A8	//Nichia
/*	if (IsFrameLevel2() == FALSE)
	{
		Sleep(300);
		if (IsFrameLevel2() == FALSE)
		{
			CString szMsg = "Bin Table 2";
			SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);		
			return FALSE;
		}
	}
*/

	stMsg.InitMessage(sizeof(RELPOS), &stPos);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("XY2_MoveToCmd_ForBL"), stMsg);
	BL_DEBUGBOX("XY2_MoveToCmd_ForBL");		//v4.22T8

	// Get the reply
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,60000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	BOOL bReturn=FALSE;
	stMsg.GetMsg(sizeof(BOOL), &bReturn);
	return bReturn;
}


VOID CBinLoader::GetBinTableEncoder(LONG *lXAxis, LONG *lYAxis)
{
	typedef struct
	{
		LONG lX;
		LONG lY;
	} ENCVAL;

 	ENCVAL stEnc;
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GetEncoderCmd", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	stMsg.GetMsg(sizeof(ENCVAL), &stEnc);
	
	*lXAxis = stEnc.lX;
	*lYAxis = stEnc.lY;
}

VOID CBinLoader::GetBinTable2Encoder(LONG *lXAxis, LONG *lYAxis)
{
	typedef struct
	{
		LONG lX;
		LONG lY;
	} ENCVAL;

 	ENCVAL stEnc;
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GetEncoderCmd2", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	stMsg.GetMsg(sizeof(ENCVAL), &stEnc);
	
	*lXAxis = stEnc.lX;
	*lYAxis = stEnc.lY;
}

BOOL CBinLoader::IsToStopTest(VOID)
{
    BOOL bState;
    int nConvID = 0;
	IPC_CServiceMessage stMsg;

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GetBinLoadTestState", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	stMsg.GetMsg(sizeof(BOOL), &bState);

    return !bState;
}

LONG CBinLoader::IsSlotAssignedWithBlock(IPC_CServiceMessage &svMsg)
{
    ULONG lBlock;
    BOOL bExit;
    LONG i, j;

	typedef struct
	{
		BOOL bAssigned[MS_MAX_BIN+1];		//v3.83
	} BIN_ASS;

 	BIN_ASS stPos;

	for (lBlock=1; lBlock<=MS_MAX_BIN; lBlock++)
	{
		bExit = FALSE;
		stPos.bAssigned[lBlock] = FALSE;

		switch( m_lOMRT )
		{
			case BL_MODE_A:		// GET LOAD
			case BL_MODE_E:		// GET LOAD		//v3.45
			case BL_MODE_F:		//MS100 8mag 150bins config		//v3.82
			case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			case BL_MODE_H:		//MS100 4mag 100bins config		//v4.31T10
				for(i=0; i<MS_BL_MGZN_NUM; i++)
				{
					if( m_stMgznOM[i].m_lMgznUsage!=BL_MGZN_USAGE_ACTIVE )
						continue;
					for(j=0; j<MS_BL_MGZN_SLOT; j++)
					{
						if( m_stMgznOM[i].m_lSlotUsage[j]==BL_SLOT_USAGE_UNUSE )
							continue;
						if( m_stMgznOM[i].m_lSlotBlock[j]==lBlock )
						{
							bExit = TRUE;
							stPos.bAssigned[lBlock] = TRUE;
							break;
						}
					}
					if( bExit==TRUE )
						break;
				}
				break;

			case BL_MODE_B:
				bExit = TRUE;
				break;

			case BL_MODE_C: // GET LOAD
				for(i=0; i<MS_BL_MGZN_NUM; i++)
				{
					if( m_stMgznOM[i].m_lMgznUsage!=BL_MGZN_USAGE_MIXED )
						continue;
					for(j=0; j<MS_BL_MGZN_SLOT; j++)
					{
						if( m_stMgznOM[i].m_lSlotUsage[j]==BL_SLOT_USAGE_UNUSE )
							continue;
						if( m_stMgznOM[i].m_lSlotBlock[j]==lBlock )
						{
							if( m_stMgznOM[i].m_lSlotUsage[j]!=BL_SLOT_USAGE_ACTIVE )
							{
								continue;
							}
							else
							{
								bExit = TRUE;
								stPos.bAssigned[lBlock] = TRUE;
								break;
							}
						}
					}
					if( bExit==TRUE )
						break;
				}
				break;

			case BL_MODE_D: // GET LOAD
				for(i=0; i<MS_BL_MGZN_NUM; i++)
				{
					if( m_stMgznOM[i].m_lMgznUsage!=BL_MGZN_USAGE_MIXED )
						continue;
					for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
					{
						if( m_stMgznOM[i].m_lSlotUsage[j]!=BL_SLOT_USAGE_ACTIVE )
							continue;
						if( m_stMgznOM[i].m_lSlotBlock[j]==lBlock )
						{
							bExit = TRUE;
							stPos.bAssigned[lBlock] = TRUE;
							break;
						}
					}
					if( bExit==TRUE )
						break;
				}
				break;

			default:
				break;
		}
	}

    svMsg.InitMessage(sizeof(BIN_ASS), &stPos);
    return TRUE;
}


LONG CBinLoader::CheckFrameExistOnBinTable(IPC_CServiceMessage &svMsg)
{
	BOOL bFrameExist = FALSE;

	if (CheckFrameOnBinTable() != BL_FRAME_NOT_EXIST)
	{
//AfxMessageBox("2. sw checksfFrame EXIST on table!", MB_SYSTEMMODAL);
		bFrameExist = TRUE;
		if (!m_fHardware || m_bNoSensorCheck)
			bFrameExist = FALSE;
	}

	//v2.83T71
	if (GetCurrBinOnBT() != 0)
	{
		bFrameExist = TRUE;
		if (!m_fHardware || m_bNoSensorCheck)
			bFrameExist = FALSE;
	}

//Temporarily disabled because not yet tested by Andrew
/*	
	//MS100 or MS109 only	//v4.48A11
	if ( (m_lBinLoaderConfig == BL_CONFIG_DUAL_DL) || (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER) )
	{
		IPC_CServiceMessage stMsg;
		BOOL bReturn = TRUE;

		if (!m_bNoSensorCheck && CheckFrameOnBinTable() && (GetCurrBinOnBT() != 0))
		{
			ManualUnloadFilmFrame(stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			if (bReturn)
				bFrameExist = FALSE;
		}

		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
		{
			if (!m_bNoSensorCheck && CheckFrameOnBinTable2() && (m_lBTCurrentBlock2 != 0))
			{
				ManualUnloadFilmFrame2(stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				if (bReturn)
					bFrameExist = FALSE;
			}
		}
	}
*/
    //svMsg.GetMsg(sizeof(BOOL), &bFrameExist);
    svMsg.InitMessage(sizeof(BOOL), &bFrameExist);
    return 1;
}


// ======================================================================================================================
// test run
// ======================================================================================================================
LONG CBinLoader::EnableBurnInTestRun(IPC_CServiceMessage &svMsg)
{
    svMsg.GetMsg(sizeof(BOOL), &m_bBurnInTestRun);

    return TRUE;
}


BOOL CBinLoader::CheckBTBLModtorStatus()
{
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable && !pBinTable->IsAllMotorsEnable())
	{
		SetErrorMessage("Motor is OFF");
		SetAlert_Red_Yellow(IDS_BT_MOTOR_OFF);
        return FALSE;
	}

	if (pBinTable && !pBinTable->IsAllMotorsHomed())
	{
		SetErrorMessage("Motor is OFF");
		SetAlert_Red_Yellow(IDS_BT_MOTOR_NOT_HOMED);
        return FALSE;
	}

	if ( IsAllMotorsEnable() == FALSE )
	{
		SetErrorMessage("Motor is OFF");
		SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);
        return FALSE;
	}

	return TRUE;
}


BOOL CBinLoader::CheckLoadUnloadSafety()
{
	if (!CheckBTBLModtorStatus())
	{
        return FALSE;
	}

	if (m_fHardware == TRUE)
	{
		if (IsPressureSensorAlarmOn() == TRUE)
		{
			SetErrorMessage("Machine pressure is low");
			SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);

			return FALSE;
		}
	}

	if ( IsMagazineSafeToMove() == FALSE )
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);

        return FALSE;
	}
	return TRUE;
}


BOOL CBinLoader::ManualLoadUnloadFilmFrame(BOOL bLoad)
{
	BOOL bStatus = TRUE;

	if (!m_bStartManualLoadUnloadTest)
	{
		return FALSE;
	}

	if (!CheckLoadUnloadSafety())
	{
		return FALSE;
	}

	SetBinTableJoystick(FALSE);

	if (bLoad)
	{
		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
		{
		}
		else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//DL with Buffer Table
		{
			INT	nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, &m_clUpperGripperBuffer, m_lTestBlkID, TRUE);

			if (nLoadFrameStatus == BL_FRAME_WT_NO_BAR_CODE)
			{
				SetErrorMessage("Manual push back bin frame due to unread barcode");
				SetAlert_Red_Yellow(IDS_BL_PUSH_BACK_FRAME_TO_SLOT);
				return FALSE;
			}

			if (nLoadFrameStatus == FALSE)
				return FALSE;
		}
		else
		{
			SetBLStartTime();
			SetBLCurTime(0,0);
			
			//Continous to Load the empty Frame if cannot read barcode on the frame
			INT nLoadFrameStatus = 0;
			do{
				nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, &m_clUpperGripperBuffer, m_lTestBlkID, TRUE);

				if (nLoadFrameStatus == BL_FRAME_WT_NO_BAR_CODE)
				{
					SetErrorMessage("Manual push back bin frame due to unread barcode");
					SetAlert_Red_Yellow(IDS_BL_PUSH_BACK_FRAME_TO_SLOT);
				}

			} while (nLoadFrameStatus == BL_FRAME_WT_NO_BAR_CODE);

			SetBLCurTime(13,0);
			SaveBLTime();
			if (nLoadFrameStatus == FALSE)
				return FALSE;

		}

		if (m_bRealignBinFrame)
		{	
			IPC_CServiceMessage svMsg;
			BOOL bStatus = TRUE;
			ULONG ulID = m_lTestBlkID;
			svMsg.InitMessage(sizeof(ULONG), &ulID);

			INT nCovID = m_comClient.SendRequest(BIN_TABLE_STN, _T("RealignPhysicalBlockCmd"), svMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nCovID, 1000) == TRUE )	// 1-sec	//v2.93T2
				{
					m_comClient.ReadReplyForConvID(nCovID, svMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
	
			svMsg.GetMsg(sizeof(BOOL), &bStatus);
			if (!bStatus)
			{
				SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);
				return FALSE;
			}
		}
		
	}
	else
	{
		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
		{

		}
/*
		else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//DL with Buffer Table
		{
			//if upper buffer has frame, unload it first
			if (m_clUpperGripperBuffer.IsBufferBlockExist())
			{
				if ( DB_UnloadFromBufferToMgz(m_bBurnInTestRun, FALSE, FALSE, FALSE, FALSE, BL_BUFFER_UPPER) == FALSE )
				{
					return FALSE;
				}
			}

			//if lower buffer has frame, unload it first		
			if (m_clLowerGripperBuffer.IsBufferBlockExist())
			{
				if ( DB_UnloadFromBufferToMgz(m_bBurnInTestRun, FALSE, FALSE, FALSE, FALSE, BL_BUFFER_LOWER) == FALSE )
				{
					return FALSE;
				}
			}

			UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, FALSE, TRUE);
		}
*/
		else
		{
			SetBLStartTime();
			SetBLCurTime(0,0);
			LONG lStatus = UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, FALSE, TRUE);
			SetBLCurTime(13,0);
			SaveBLTime();
			if (lStatus == FALSE)
				return FALSE;
		}
	}

	return bStatus;
}


LONG CBinLoader::ManualUpdateFrameBarcode(IPC_CServiceMessage& svMsg)
{
    LONG lSlot = 0;
	LONG lStatus = TRUE;
	BOOL bReturn=TRUE;

	svMsg.GetMsg(sizeof(LONG), &lSlot);
    if( lSlot < 1 || lSlot > MS_BL_MGZN_SLOT )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	LONG lMgzn = m_lSelMagazineID;
    if( lMgzn < 0 || lMgzn >= MS_BL_MGZN_NUM )		//v4.02T5
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}


	lMgzn = min(lMgzn, MS_BL_MGZN_NUM - 1);
	lSlot = min(lSlot, MS_BL_MGZN_SLOT);
	lSlot--;

    if (m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] != BL_SLOT_USAGE_ACTIVE )
	{
		CString szErr;
		szErr.Format("Bin frame status not ACTIVE for Mgzn #%d Slot #%d; barcode checking is aborted.", 1, lSlot-1);
		HmiMessage(szErr);
		SetErrorMessage(szErr);

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	LONG lBlock = m_stMgznRT[lMgzn].m_lSlotBlock[lSlot];

	if (!CheckLoadUnloadSafety())
	{
		bReturn = TRUE;			//v2.56 prevent HMI page lock-up
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (m_bUseBarcode != TRUE)
	{
		HmiMessage("ERROR: BARCODE option is not enabled");

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	//Turn off COMPARE-BARCODE option if currently enabled
	CTmpChange<BOOL> bCompareBarcode(&m_bCompareBarcode, FALSE);

	SetBinTableJoystick(FALSE);
	BL_DEBUGBOX("Rescan Barcode start");	//v4.01T1
	// StartLoadingAlert();

	if (m_lBinLoaderConfig == BL_CONFIG_DL) //DL
	{
		INT	nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, &m_clUpperGripperBuffer, lBlock, TRUE, FALSE, TRUE);
	}
	else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//DL with Buffer Table
	{
		LONG lStatus = 0;

		if (!UnloadFrameOnUpperGripper(FALSE, TRUE, FALSE))
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (!UnloadFrameOnLowerGripper(FALSE, FALSE, FALSE))
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		//INT	nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, &m_clUpperGripperBuffer, lBlock, TRUE, FALSE, TRUE);
		LONG nLoadFrameStatus = UDB_MgznSlotToMgznSlot(&m_clUpperGripperBuffer, lMgzn, lSlot, lMgzn, lSlot, TRUE);
	}

	SetBinTableJoystick(TRUE);
	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Load Film Frame");
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::ManualLoadFilmFrame(IPC_CServiceMessage& svMsg)
{
    CString szMsgLog;
    CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

    szMsgLog ="Manual Load Film Frame";
    CMSLogFileUtility::Instance()->MS_LogOperation(szMsgLog);
	BL_DEBUGBOX(szMsgLog);
	
    LONG lBlock;
	LONG lStatus = TRUE;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lBlock);

//CreateWIPTempFile(m_lCurrMgzn, lBlock - 1);
//CreateWIPOutputFileWithBarcode(m_lCurrMgzn, lBlock - 1);

    if( lBlock<1 || lBlock>MS_MAX_BIN )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (!CheckLoadUnloadSafety())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	BOOL bSetupModeWithoutAutoLine = FALSE;
	if (IsMSAutoLineMode())
	{
		bSetupModeWithoutAutoLine = TRUE;
		CStringList List;
		List.AddTail("SIS setup");
		if (!pApp->CheckSanAnProductionMode())
		{
			//Not Operation Mode
			List.AddTail("Machine Setup");
		}
		LONG a = HmiSelection("Select Setup-Mode", "Select", List, 0);
		if (a == -1)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
		else if (a == 0)
		{
			bSetupModeWithoutAutoLine = 0;
		}
	}
	CTmpChange<BOOL> bAutoLoadUnloadTest(&m_bAutoLoadUnloadTest, bSetupModeWithoutAutoLine);

	SetBinTableJoystick(FALSE);
	(*m_psmfSRam)["BinTable"]["PhyBlkDieCount"] = 0;	//v4.59A25

	bReturn = FALSE;
	/*********/ StartLoadingAlert(); /*********/
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)			//UDBuffer
	{
		BOOL bBufferLevel	= BL_BUFFER_UPPER;
		if (m_lUseLBufferGripperForSetup)
		{
			bBufferLevel = BL_BUFFER_LOWER;
		}

		CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;

		LONG nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, pGripperBuffer, lBlock, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE);
		if (nLoadFrameStatus == BL_FRAME_WT_NO_BAR_CODE)
		{
			SetErrorMessage("Manual push back bin frame due to unread barcode");
			SetAlert_Red_Yellow(IDS_BL_PUSH_BACK_FRAME_TO_SLOT);
		}
		if (nLoadFrameStatus == TRUE)
		{
			bReturn = TRUE;
		}
	}
/*
	else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//DL with Buffer Table
	{
		INT	nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, &m_clUpperGripperBuffer, lBlock, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE);

		if (nLoadFrameStatus == BL_FRAME_WT_NO_BAR_CODE)
		{
			SetErrorMessage("Manual push back bin frame due to unread barcode");
			SetAlert_Red_Yellow(IDS_BL_PUSH_BACK_FRAME_TO_SLOT);
		}
	}
*/
	else		//DL or Dual-DL Tables 
	{
		SetBLStartTime();
		SetBLCurTime(0,0);
		
		//Continous to Load the empty Frame if cannot read barcode on the frame

		INT nLoadFrameStatus = 0;
		//pllm
		nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, &m_clUpperGripperBuffer, lBlock, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE);
		while (nLoadFrameStatus == BL_FRAME_WT_NO_BAR_CODE)
		{
			if (nLoadFrameStatus == BL_FRAME_WT_NO_BAR_CODE)
			{
				SetErrorMessage("Manual push back bin frame due to unread barcode");
				SetAlert_Red_Yellow(IDS_BL_PUSH_BACK_FRAME_TO_SLOT);
			}
			nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, &m_clUpperGripperBuffer, lBlock, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE);
		}

		if (nLoadFrameStatus == Err_BLLoadEmptyBarcodeFail)	
		{
		}

		SetBLCurTime(13,0);
		SaveBLTime();
		if (nLoadFrameStatus == TRUE)
		{
			bReturn = TRUE;
		}
	}
	/*********/ CloseLoadingAlert(); /*********/

	SetBinTableJoystick(TRUE);

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Load Film Frame");
	}

	if (bReturn)
	{
		//DO Realign
		RealignBinFrameCmd(svMsg);
	}

	bReturn = TRUE; //need do
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::ManualUnloadFilmFrame(IPC_CServiceMessage& svMsg)
{
	BOOL bFull;
	BOOL bReturn = TRUE;
	LONG lStatus = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (!CheckLoadUnloadSafety())
	{
 		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	LONG lFrameOnBT = CheckFrameOnBinTable();
	if ( !m_bNoSensorCheck && (lFrameOnBT != BL_FRAME_ON_CENTER) )
    {
		SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
		SetErrorMessage("BL: No frame existed on bintable 1 in ManualUnloadFilmFrame");

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
    }

	SetBinTableJoystick(FALSE);


	BOOL bAutoLoadUnloadMachineSetup = FALSE;
	BOOL bOption = (BOOL)(LONG)(*m_psmfSRam)["BinOutputFile"]["Enable Osram Bin Mix Map"];
	if(	bOption)
	{
		bFull = TRUE;
	}
	else
	{
		if (IsMSAutoLineMode())
		{
			bAutoLoadUnloadMachineSetup = TRUE;
			CStringList List;
			List.AddTail("SIS setup");
			if (!pApp->CheckSanAnProductionMode())
			{
				//Not Operation Mode
				List.AddTail("Machine Setup");
			}
			LONG a = HmiSelection("Select Setup-Mode", "Select", List, 0);
			if (a == -1)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			else if (a == 0)
			{
				bAutoLoadUnloadMachineSetup = FALSE;
				CStringList List;
				List.AddTail("WIP Slots");
				List.AddTail("Output Slots");
				CString szSelection;
				LONG a = HmiSelection("Select Slots", "Select", List, 0);
				if (a == -1)
				{
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return TRUE;
				}
				else
				{
					bFull = (a == 1)? TRUE : FALSE;
				}
			}
			else
			{
				bFull = BL_YES_NO(HMB_BL_UNLOAD_TO_FULL_MAGZ, IDS_BL_UNLOAD_FRAME);
			}
		}
		else
		{
			bFull = BL_YES_NO(HMB_BL_UNLOAD_TO_FULL_MAGZ, IDS_BL_UNLOAD_FRAME);
		}
	}

	CTmpChange<BOOL> bAutoLoadUnloadTest(&m_bAutoLoadUnloadTest, bAutoLoadUnloadMachineSetup);

	/*********/ StartLoadingAlert(); /*********/
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//DL with UpDown Buffer Table
	{
		//andrewng
		LONG lStatus		= TRUE;
		BOOL bBufferLevel	= BL_BUFFER_UPPER;
		if (m_lUseLBufferGripperForSetup)
		{
			bBufferLevel = BL_BUFFER_LOWER;
		}

		CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
		UnloadBinFrame(m_bBurnInTestRun, pGripperBuffer, bFull, TRUE);		
	}
/*
	else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//DL with Buffer Table
	{
		if (!UnloadFrameOnUpperGripper(bFull, TRUE, TRUE)
		{
			CloseLoadingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (!UnloadFrameOnUpperGripper(FALSE, FALSE, TRUE)
		{
			CloseLoadingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);

	}
*/
	else
	{
		SetBLStartTime();
		SetBLCurTime(0,0);
		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);
		SetBLCurTime(13,0);
		SaveBLTime();
	}
	/*********/ CloseLoadingAlert(); /*********/

	SetBinTableJoystick(TRUE);

	m_bBinFrameCheckBCFail	= FALSE;

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Unload Film Frame");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::UnloadFilmFrame(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lStatus = TRUE;


	if (GetCurrBinOnBT()==0)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (!CheckLoadUnloadSafety())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	SetBinTableJoystick(FALSE);

	SetBLStartTime();
	SetBLCurTime(0,0);
	UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, FALSE, TRUE);
	SetBLCurTime(13,0);
	SaveBLTime();

	BL_DEBUGBOX("Unload frame set cur block to 0");
	m_lBTCurrentBlock	= 0;
	m_lExArmBufferBlock = 0;
	
	m_szBinFrameBarcode = "";
	m_szBufferFrameBarcode = "";
	m_clLowerGripperBuffer.SetBufferBarcode("");

	m_lCurrMgzn			= 0;
	m_lCurrSlot			= 0;
	m_lCurrHmiMgzn		= 0;
	m_lCurrHmiSlot		= 0;

	for(int i = 0; i < MS_BL_MGZN_NUM; i++)
	{
		ResetMagazine(i);
	}

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Unload Film Frame");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::ToggleStepMode(IPC_CServiceMessage& svMsg)
{
	svMsg.GetMsg(sizeof(BOOL), &m_bStepMode);

    return TRUE;
}

LONG CBinLoader::AutoChangeFilmFrame(IPC_CServiceMessage& svMsg)
{
    LONG i, LOOPCOUNTER;

    SetBinTableJoystick(FALSE);
	
    switch( m_lOMRT )
    {
    case BL_MODE_A:
        LOOPCOUNTER = 100;
        break;
    case BL_MODE_B:
        LOOPCOUNTER = BT_MAX_BINBLK_NO;
        break;
    case BL_MODE_C:
        LOOPCOUNTER = BT_MAX_BINBLK_NO;
        break;
    case BL_MODE_D:
        LOOPCOUNTER = BT_MAX_BINBLK_NO;
        break;
    case BL_MODE_E:		//v3.45
        LOOPCOUNTER = BT_MAX_BINBLK_NO;
        break;
    case BL_MODE_F:		//v3.82		//MS100 8mag 150 config
        LOOPCOUNTER = BT_MAX_BINBLK_NO;
        break;
    case BL_MODE_G:		//v3.82		//MS100 8mag 175 config
        LOOPCOUNTER = BT_MAX_BINBLK_NO;
        break;
    case BL_MODE_H:		//v4.31T10	//MS100 4mag 100 config		//Yearly MS100Plus
        LOOPCOUNTER = 100;
        break;
    default:
        break;
    }

    for(i=0; i<LOOPCOUNTER; i++ )
    {
        if( IsToStopTest() )
            break;

		if ( LoadBinFrame(m_bBurnInTestRun, &m_clUpperGripperBuffer, i+1) != TRUE )
		{
			break;
		}

        if( IsToStopTest() )
            break;

		if ( UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, TRUE) != TRUE )
		{
			break;
		}
    }

	SetBinTableJoystick(TRUE);
	
	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Auto Change Film Frame");
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CBinLoader::LoopTestCounter(IPC_CServiceMessage &svMsg)
{
	svMsg.GetMsg(sizeof(LONG), &m_lLoopTestCounter);

	return TRUE;
}


LONG CBinLoader::AutoLoadUnloadTest(IPC_CServiceMessage &svMsg)
{
	LONG	lResult = 1;
	LONG	lCurrSlot = m_lCurrSlot;
	LONG	lCurrMgzn = m_lCurrMgzn;

	CStringList MgznList;
	CStringList List;
	BOOL IsSingleMagazine = FALSE;
	MgznList.AddTail("Single Magazine");
	MgznList.AddTail("Multiple Magazine");

	//CString szSelection;
	LONG a1 = HmiSelection("Select Test Magazine Type", "Select", MgznList, 0);

	
	if (a1 < 0)
	{
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}
	else if (a1 == 0)
	{		
		List.AddTail("TOP 1");
		List.AddTail("MID 1");
		List.AddTail("BTM 1");
		List.AddTail("TOP 2");
		List.AddTail("MID 2");
		List.AddTail("BTM 2");
		List.AddTail("8-BTM 1");
		List.AddTail("8-BTM 2");
		IsSingleMagazine = TRUE;
	}
	else if (a1 == 1)
	{
		List.AddTail("MID1-MID2");
		List.AddTail("TOP1-TOP2");
		List.AddTail("BTM1-BTM2");
		List.AddTail("8-BTM1-8-BTM2");
		List.AddTail("All");
		IsSingleMagazine = FALSE;
	}

	LONG a = HmiSelection("Select Magazine", "Select", List, 0);
	if (a < 0)
	{
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	BOOL bMID1MID2Mgzn = FALSE;
	BOOL bTOP1TOP2Mgzn = FALSE;
	BOOL bBTM1BTM2Mgzn = FALSE;
	BOOL b8BTM18BTM2Mgzn = FALSE;
	BOOL bAllMgzn = FALSE;

	if (IsSingleMagazine)
	{
		m_lCurrMgzn = a;
	}
	else 
	{
		switch(a)
		{
			case 0:
				m_lCurrMgzn = BL_MGZ_MID_1;
				bMID1MID2Mgzn = TRUE;
				break;
			case 1:
				m_lCurrMgzn = BL_MGZ_TOP_1;
				bTOP1TOP2Mgzn = TRUE;
				break;
			case 2:
				m_lCurrMgzn = BL_MGZ_BTM_1;
				bBTM1BTM2Mgzn = TRUE;
				break;
			case 3:
				m_lCurrMgzn = BL_MGZ8_BTM_1;
				b8BTM18BTM2Mgzn = TRUE;
				break;
			case 4:
				m_lCurrMgzn = BL_MGZ_TOP_1;
				bAllMgzn = TRUE;
				break;
			default:
				m_lCurrMgzn = BL_MGZ_MID_1;
				break;
		}
	}

	if (!CheckBTBLModtorStatus())
	{
		CMSLogFileUtility::Instance()->BL_LogStatus("CheckBTBLModtorStatus Failure");

		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	if (!CheckSafeToMove())
	{
		svMsg.InitMessage(sizeof(LONG), &lResult);
		return TRUE;
	}

	LONG lTotalSlots = m_stMgznRT[m_lCurrMgzn].m_lNoOfSlots;

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
			bAutoLoadUnloadQCTest = 2;
			lTotalSlots = MS_BL_AUTO_LINE_INPUT_END_MGZN_SLOT;
		}
		else
		{
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return TRUE;
		}
	}

	CTmpChange<BOOL> bAutoLoadUnloadTest(&m_bAutoLoadUnloadTest, bAutoLoadUnloadQCTest);

//	for (LONG i = 0; i < lTotalSlots; i++)
	LONG lCount = 0;
	LONG i = 0;
	theApp.ClearStopLoadUnloadLoopTest();
	while (TRUE)
	{
		if (theApp.IsStopLoadUnloadLoopTest())
		{
			HouseKeeping(m_lReadyPos_X);
			CString szMess;
			szMess.Format("Stop LoadUnload Loop Test (cycle count = %d, Mag = %s, Slot = %d) Failure", lCount, (const char*)GetMagazineName(m_lCurrMgzn),  m_lCurrSlot);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMess);
			m_lCurrSlot = lCurrSlot;
			m_lCurrMgzn = lCurrMgzn;
			HmiMessage(szMess);
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return TRUE;
		}

		m_lCurrSlot = i;

		CBinGripperBuffer *pGripperBuffer = &m_clUpperGripperBuffer;
		if (MoveElevatorToLoad(m_lCurrMgzn, m_lCurrSlot, pGripperBuffer) == FALSE)
		{
			HouseKeeping(m_lReadyPos_X);

			CString szMess;
			szMess.Format("LoadUnload Loop Test (cycle count = %d, Mag = %s, Slot = %d) Failure", lCount, (const char*)GetMagazineName(m_lCurrMgzn),  m_lCurrSlot);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMess);
			m_lCurrSlot = lCurrSlot;
			m_lCurrMgzn = lCurrMgzn;
			HmiMessage(szMess);
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return TRUE;
		}

//		nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, lBlock, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE);
		LONG lBlock = m_stMgznRT[m_lCurrMgzn].m_lSlotBlock[m_lCurrSlot];
		LONG nLoadBinFrameStatus = LoadBinFrame(FALSE, &m_clUpperGripperBuffer, lBlock, TRUE);
		if (nLoadBinFrameStatus != TRUE)
		{
			HouseKeeping(m_lReadyPos_X);

			CString szMess;
			szMess.Format("LoadUnload Loop Test (cycle count = %d, Mag = %s, Slot = %d) Failure", lCount, (const char*)GetMagazineName(m_lCurrMgzn),  m_lCurrSlot);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMess);
			m_lCurrSlot = lCurrSlot;
			m_lCurrMgzn = lCurrMgzn;
			HmiMessage(szMess);
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return TRUE;
		}

		SetStatusMessage("BL Filmframe is loaded");
		Sleep(500);
		X_Home();
		Sleep(500);

		if (UnloadBinFrame(FALSE, &m_clLowerGripperBuffer, FALSE) != TRUE )
		{
			HouseKeeping(m_lReadyPos_X);

			CString szMess;
			szMess.Format("LoadUnload Loop Test (cycle count = %d, Mag = %s, Slot = %d) Failure", lCount, (const char*)GetMagazineName(m_lCurrMgzn),  m_lCurrSlot);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMess);
			m_lCurrSlot = lCurrSlot;
			m_lCurrMgzn = lCurrMgzn;
			HmiMessage(szMess);
			svMsg.InitMessage(sizeof(LONG), &lResult);
			return TRUE;
		}
		SetStatusMessage("BL Filmframe is unloaded");
		Sleep(1000);
		
		if (++i >= lTotalSlots)
		{
			if (bAllMgzn)
			{
				m_lCurrMgzn++;
				if (m_lCurrMgzn > BL_MGZ8_BTM_2)
				{
					m_lCurrMgzn = BL_MGZ_TOP_1;
				}
			}
			else
			{
				if (m_lCurrMgzn == BL_MGZ_MID_1)
				{
					m_lCurrMgzn = BL_MGZ_MID_2;
				}
				else if (m_lCurrMgzn == BL_MGZ_TOP_1)
				{
					m_lCurrMgzn = BL_MGZ_TOP_2;
				}
				else if (m_lCurrMgzn == BL_MGZ_BTM_1)
				{
					m_lCurrMgzn = BL_MGZ_BTM_2;
				}
				else if (m_lCurrMgzn == BL_MGZ8_BTM_1)
				{
					m_lCurrMgzn = BL_MGZ8_BTM_2;
				}
				else
				{
					m_lCurrMgzn = BL_MGZ_MID_1;
				}
			}
			i = 0;
		}
	
		if (bMID1MID2Mgzn || bTOP1TOP2Mgzn || bBTM1BTM2Mgzn || b8BTM18BTM2Mgzn)
		{
			if ((i > 1) && (i < lTotalSlots - 2))
			{
				i = lTotalSlots - 2;
			}
			if (++lCount >= 14)
			{
				break;
			}
		}

		if (++lCount >= m_lLoopTestCounter)
		{
			break;
		}
	}

	HouseKeeping(m_lReadyPos_X);
	CString szMess;
	szMess.Format("LoadUnload Loop Test (cycle count = %d, Mag = %s, Slot = %d) Done", lCount, (const char*)GetMagazineName(m_lCurrMgzn),  m_lCurrSlot);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMess);
	m_lCurrSlot = lCurrSlot;
	m_lCurrMgzn = lCurrMgzn;
	HmiMessage(szMess);
	svMsg.InitMessage(sizeof(LONG), &lResult);
	return TRUE;
}

LONG CBinLoader::SelectRTOperationMode(IPC_CServiceMessage& svMsg)
{
	CString szTitle, szContent;

    svMsg.GetMsg(sizeof(LONG), &m_lOMRT);
    LoadMgznRTData();

	szTitle.LoadString(HMB_BL_OPERATION_MODE);

    switch( m_lOMRT )
    {
		case BL_MODE_A:
			szContent.LoadString(HMB_BL_SELECT_MODE_A);
			break;

		case BL_MODE_B:
			szContent.LoadString(HMB_BL_SELECT_MODE_B);
			break;

		case BL_MODE_C:
			szContent.LoadString(HMB_BL_SELECT_MODE_C);
			break;

		case BL_MODE_D:
			szContent.LoadString(HMB_BL_SELECT_MODE_D);
			break;

		case BL_MODE_E:		//v3.45
			szContent.LoadString(HMB_BL_SELECT_MODE_E);
			break;

		case BL_MODE_F:		//MS100 8mag 150 config		//v3.82
			//szContent.LoadString(HMB_BL_SELECT_MODE_A);
			szContent = "Mode F is selected";
			break;

		case BL_MODE_G:		//MS100 8mag 175 config		//v3.82
			//szContent.LoadString(HMB_BL_SELECT_MODE_E);
			szContent = "Mode G is selected";
			break;

		case BL_MODE_H:		//MS100 4mag 100 config		//v4.31T10	//Yearly MS100Plus
			//szContent.LoadString(HMB_BL_SELECT_MODE_E);
			szContent = "Mode H is selected";
			break;
	}

	HmiMessage(szContent, szTitle);
    
	SaveData();

	SendOMRuntimeEvent_7053(m_lOMRT);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CBinLoader::StartLotResetAllMgznCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	BL_DEBUGBOX("start lot reset all mgzns set cur block to 0");
	m_lBTCurrentBlock	= 0;
	m_lExArmBufferBlock = 0;
	m_szBinFrameBarcode = "";
	m_szBufferFrameBarcode = "";
	m_clUpperGripperBuffer.SetBufferBarcode("");
	m_clLowerGripperBuffer.SetBufferBarcode("");
	m_lCurrMgzn			= 0;
	m_lCurrSlot			= 0;
	m_lCurrHmiMgzn		= 0;
	m_lCurrHmiSlot		= 0;

	for(int i=0; i<MS_BL_MGZN_NUM; i++)
		ResetMagazine(i);

	(*m_psmfSRam)["BinTable"]["LastBlkInUse"] = 0;
	(*m_psmfSRam)["BinTable"]["BlkInUse"] = 0;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::ManualLoadUnloadRealignTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	BOOL bStart = FALSE;
    svMsg.GetMsg(sizeof(BOOL), &bStart);


	if (bStart && m_bStartManualLoadUnloadTest)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (!bStart)
	{
		m_bStartManualLoadUnloadTest = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}


HmiMessage("Start Test ...");


    if( m_lTestBlkID<1 || m_lTestBlkID>MS_MAX_BIN )
	{
		CString szTemp;
		szTemp.Format("Invalid block ID : %d", m_lTestBlkID);
		HmiMessage(szTemp);

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (!m_fHardware || !CheckLoadUnloadSafety())
	{
		bReturn = FALSE;	
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	SetBinTableJoystick(FALSE);

	m_lTestCycle = 0;
	m_bStartManualLoadUnloadTest = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


// Reset all magazine status but restore the barcode
LONG CBinLoader::RestoreMagazineRunTimeStatus(IPC_CServiceMessage& svMsg)
{
	BL_MAGAZINE stMagzineTemp[MS_BL_MGZN_NUM];

	BOOL bReturn = TRUE;

	BL_DEBUGBOX("restore mgzn rt data set cur block to 0");
	m_lBTCurrentBlock	= 0;
	m_lExArmBufferBlock = 0;
	m_szBinFrameBarcode = "";
	m_szBufferFrameBarcode = "";
	m_clUpperGripperBuffer.SetBufferBarcode("");
	m_clLowerGripperBuffer.SetBufferBarcode("");
	m_lCurrMgzn			= 0;
	m_lCurrSlot			= 0;
	m_lCurrHmiMgzn		= 0;
	m_lCurrHmiSlot		= 0;

	(*m_psmfSRam)["BinTable"]["LastBlkInUse"] = 0;
	(*m_psmfSRam)["BinTable"]["BlkInUse"] = 0;

	// Get a copy of magazine struct
	for (INT i=0; i<MS_BL_MGZN_NUM; i++)
	{
		stMagzineTemp[i] = m_stMgznRT[i];
	}

	// Reset All Magazine status
	for (INT i=0; i<MS_BL_MGZN_NUM; i++)
	{
		ResetMagazine(i);
	}

	for (INT i=0; i<MS_BL_MGZN_NUM; i++)
	{	
		// Reset all full and empty magazine
		if ((m_stMgznRT[i].m_lMgznUsage ==  BL_MGZN_USAGE_FULL) || 
			(m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY))
		{
			continue;
		}

		
		// Copy back the barcode to the struct
		for(INT j=0; j<MS_BL_MAX_MGZN_SLOT; j++)		//v4.19		//Cree HuiZhou
		{
			m_stMgznRT[i].m_SlotBCName[j] = stMagzineTemp[i].m_SlotBCName[j];
			//m_stMgznRT[i].m_lSlotUsage[j] = s

		}
		

		//m_stMgznRT[i] = stMagzineTemp[i];
	}

	try
	{
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Reset Magazine");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}

LONG CBinLoader::ResetSingleSlotInfo(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsgLog;
    CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG lHmiStatus = 0;
	CString szTitle, szContent;
	CStringList szList;
	LONG lSelection = 0;

	LONG lCurrSlot = 1;
	svMsg.GetMsg(sizeof(LONG), &lCurrSlot);

    szMsgLog ="Reset single slot info.";
    CMSLogFileUtility::Instance()->MS_LogOperation(szMsgLog);
	BL_DEBUGBOX(szMsgLog);

	if (lCurrSlot < 1 || lCurrSlot > 25)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	LONG lCurrMgzn = m_lSelMagazineID;

	if (IsMSAutoLineMode())
	{
		szList.AddTail("UNUSE");
		szList.AddTail("FULL");
		szList.AddTail("EMPTY");
		szList.AddTail("ACTIVE");
	}
	else
	{
		szList.AddTail("UNUSE");
		szList.AddTail("FULL");
		szList.AddTail("EMPTY");
		szList.AddTail("ACTIVE");
		szList.AddTail("ACT2F");
	}

	szTitle = "Change the Usage";
	szContent = "Please choose the Usage. \n";
	lSelection = HmiSelection(szContent, szTitle, szList, 0);
	if(lSelection >= 0)
	{
		m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot - 1] = lSelection;
	}

	if (IsMSAutoLineMode())
	{
		if ((m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot - 1] == BL_SLOT_USAGE_UNUSE) ||
			(m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot - 1] == BL_SLOT_USAGE_EMPTY))
		{
			m_stMgznRT[lCurrMgzn].m_lSlotWIPCounter[lCurrSlot - 1] = 0;
			m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot - 1].Empty();

			if (lCurrMgzn == BL_MGZ_TOP_1)
			{
				m_stMgznRT[lCurrMgzn].m_lSlotBlock[lCurrSlot - 1] = 0;
			}
		}
	}
	else
	{
		m_stMgznRT[lCurrMgzn].m_lSlotWIPCounter[lCurrSlot - 1] = 0;
		m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot - 1].Empty();
		if ((m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot - 1] == BL_SLOT_USAGE_FULL) ||
			(m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot - 1] == BL_SLOT_USAGE_EMPTY))
		{
			m_stMgznRT[lCurrMgzn].m_lSlotBlock[lCurrSlot - 1] = 0;
		}
	}

	SECS_UpdateCassetteSlotInfo(lCurrMgzn, lCurrSlot);

	SaveData();
	SaveMgznRTData();

	SendResetSlotEvent_7018(lCurrMgzn, lCurrSlot - 1);
	HmiMessage("Reset Single Bin Slot Info Finished");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::ResetMagazineCmd(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;
	CString szMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	szTitle.LoadString(HMB_BL_RESET_MAGAZINE);
	szContent.LoadString(HMB_GENERAL_AREUSURE);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
	{
		BOOL bReturn= FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CStringList szList;
	CString szLog;
	LONG lMgzn=0, i;

	BOOL bOperatorMode = FALSE;
	if ( ((pApp->GetCustomerName() == "Semitek") || (pApp->CheckSanAnProductionMode())) &&	//v4.51A9
		 (pApp->GetCustomerName() != "Cree") )			//v4.51A21
	{
		bOperatorMode = TRUE;
	}

	if (bOperatorMode)
	{
		if (IsMSAutoLineMode())
		{
			szList.AddTail(GetMagazineName(BL_MGZ_TOP_2).MakeUpper());
		}
		else if (IsMSAutoLineStandloneMode())
		{
			szList.AddTail(GetMagazineName(BL_MGZ_TOP_2).MakeUpper());
			szList.AddTail(GetMagazineName(BL_MGZ_MID_2).MakeUpper());
		}
		else
		{
			szList.AddTail("All");
			szList.AddTail(GetMagazineName(BL_MGZ_TOP_1).MakeUpper());
			szList.AddTail(GetMagazineName(BL_MGZ_TOP_2).MakeUpper());
			szList.AddTail(GetMagazineName(BL_MGZ_MID_2).MakeUpper());
		}
	}
	else
	{
		szList.AddTail("All");
		for(i=0; i<MS_BL_MGZN_NUM; i++)		//default 8-mag config for MS100
		{
			szList.AddTail(GetMagazineName(i).MakeUpper());
		}
	}

	szList.AddTail("Output Tables only");	
	szContent.LoadString(HMB_BL_SELECT_RESET_MAGZ);
	LONG lSet = 0;
	
	if( pApp->GetCustomerName()=="Cree" )
		lSet = 100;
	lMgzn = HmiSelection(szContent, szTitle, szList, lSet);
	szContent.LoadString(HMB_GENERAL_AREUSURE);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szContent);	


	//All, Top1, Top2
	if (pApp ->GetCustomerName() == "ChangeLight(XM)")
	{
		if (bOperatorMode)
		{
			if (IsMSAutoLineMode() || IsMSAutoLineStandloneMode())
			{
				if (lMgzn == 0)
				{
					//Reset TOP#2
					OpOutoutMgznSummary(TRUE);
				}
			}
			else
			{
				if (lMgzn == 0 || lMgzn == 1 || lMgzn == 2)
				{
					OpOutoutMgznSummary((lMgzn == 2) ? TRUE : FALSE);
				}
			}
		}
		else
		{
			if (lMgzn == 0 || lMgzn == 1 || lMgzn == 4)
			{
				OpOutoutMgznSummary((lMgzn == 4) ? TRUE : FALSE);
			}
		}
	}

	if (lMgzn < 0)
	{
		BOOL bReturn= FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (bOperatorMode)			//v4.51A21
	{
		CString szMsg;
		szMsg.Format("ResetMgznCmd selection is %d ", lMgzn);	//v4.53A6
		if (IsMSAutoLineMode())
		{
			if (lMgzn == 0)
			{
				lMgzn = BL_MGZ_TOP_2 + 1;
				szMsg += GetMagazineName(lMgzn - 1).MakeUpper();
			}
			else
			{
				lMgzn = 9;
				szMsg += "Output Tables only";
			}
		}
		else if (IsMSAutoLineStandloneMode())
		{
			if (lMgzn == 0)
			{
				lMgzn = BL_MGZ_TOP_2 + 1;
				szMsg += GetMagazineName(lMgzn - 1).MakeUpper();
			}
			else if (lMgzn == 1)
			{
				lMgzn = BL_MGZ_MID_2 + 1;
				szMsg += GetMagazineName(lMgzn - 1).MakeUpper();
			}
			else
			{
				lMgzn = 9;
				szMsg += "Output Tables only";
			}
		}
		else
		{
			if (lMgzn == 0)
			{
				szMsg += "All";
			}
			else if (lMgzn == 4)
			{
				lMgzn = 9;
				szMsg += "Output Tables only";
			}
			else
			{
				if (lMgzn == 3)
				{
					lMgzn = 5;
				}
				else if (lMgzn == 2)
				{
					lMgzn = 4;
				}
				else
				{
					lMgzn = 1;
				}

				szMsg += GetMagazineName(lMgzn - 1).MakeUpper();
			}
		}
		SetErrorMessage(szMsg);
	}	// semitek special, remove useless options


	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		if ( lMgzn == 0 )
		{
			// check if all bin cleared, if not pop up message for operator to select
			if (IsMagazineCanReset(0, TRUE) == FALSE)
			{
				szContent.LoadString(HMB_BL_RESET_MAG_BIN_NOT_CLEARED);
				HmiMessage(szContent, szTitle);
				BOOL bReturn= FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			//Add table sensor checking to make sure frame is removed manually before RESET
			if (!m_bNoSensorCheck)
			{
				BOOL bReturn=FALSE;
				if (CheckFrameOnBinTable() != BL_FRAME_NOT_EXIST)
				{
					SetErrorMessage("BL ResetAllMgzn: Frame exists on bin table 1");
					//HmiMessage(ERROR: frame exist on table 1; please remove it manually.");
					SetAlert_Red_Yellow(IDS_BL_BT1_FRAME_EXIST);
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}

				if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
				{
					if (IsDualBufferUpperExist())
					{
						SetErrorMessage("BL ResetAllMgzn: Frame exists in Upper Buffer");
						SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
						svMsg.InitMessage(sizeof(BOOL), &bReturn);
						return 1;
					}
					if (IsDualBufferLowerExist())
					{
						SetErrorMessage("BL ResetAllMgzn: Frame exists in Lower Buffer");
						SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
						svMsg.InitMessage(sizeof(BOOL), &bReturn);
						return 1;
					}
				}
			}

			CMSLogFileUtility::Instance()->MS_LogOperation("Manual Reset All Magazine");	
			m_bChangeOMRecordFileName = TRUE;
			
			if (m_lOMRT == BL_MODE_F)
			{
				SetOperationMode_F(); // Reset
				SaveMgznOMData();
			}
			
			for(int i = 0 ; i < 8 ; i++)
			{
				if (IsMagazineCanReset(i, FALSE) == FALSE)
				{
					szContent.LoadString(HMB_BL_RESET_MAG_BIN_NOT_CLEARED);
					HmiMessage(szContent, szTitle);
					BOOL bReturn= FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}
				ResetMagazine(i);	
			}

			//ResetAllMagazine();
			m_bChangeOMRecordFileName = FALSE;
		}
		else if (lMgzn == 9 )
		{
			//Add table sensor checking to make sure frame is removed manually before RESET
			if (!m_bNoSensorCheck)
			{
				BOOL bReturn=FALSE;
				if (CheckFrameOnBinTable() != BL_FRAME_NOT_EXIST)
				{
					SetErrorMessage("BL ResetTable: Frame exists on bin table 1");
					//HmiMessage(ERROR: frame exist on table 1; please remove it manually.");
					SetAlert_Red_Yellow(IDS_BL_BT1_FRAME_EXIST);
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}

				if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
				{
					if (IsDualBufferUpperExist())
					{
						SetErrorMessage("BL ResetTable: Frame exists in Upper Buffer");
						SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
						svMsg.InitMessage(sizeof(BOOL), &bReturn);
						return 1;
					}
					if (IsDualBufferLowerExist())
					{
						SetErrorMessage("BL ResetTable: Frame exists in Lower Buffer");
						SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
						svMsg.InitMessage(sizeof(BOOL), &bReturn);
						return 1;
					}
				}
			}

			BL_DEBUGBOX("Reset mgzn 7 or 9, set cur block to 0");
			LONG lTempBlk		= GetCurrBinOnBT();
			LONG lTempBufBlk	= m_lExArmBufferBlock;
			m_lBTCurrentBlock	= 0;
			m_lExArmBufferBlock = 0;
			m_szBinFrameBarcode = "";
			m_szBufferFrameBarcode = "";
			m_clUpperGripperBuffer.SetBufferBarcode("");
			m_clLowerGripperBuffer.SetBufferBarcode("");
			m_clUpperGripperBuffer.SetBufferBlock(0);
			m_clLowerGripperBuffer.SetBufferBlock(0);

			//v4.02T6	//WH Semitek special LoadUnloadBtCmd fcn
			m_bLoadUnloadBtToggle	= FALSE;
			m_lBTBackupBlock		= 0;

			m_lCurrMgzn			= 0;
			m_lCurrSlot			= 0;
			m_lCurrHmiMgzn		= 0;	
			m_lCurrHmiSlot		= 0;

			for (INT i=0; i<MS_BL_MGZN_NUM; i++)
			{
				if ( m_stMgznRT[i].m_lMgznUsage!=BL_MGZN_USAGE_ACTIVE )
					continue;

				for (INT j=0; j<MS_BL_MGZN_SLOT; j++)
				{
					if ( (lTempBlk > 0) && (m_stMgznRT[i].m_lSlotBlock[j] == lTempBlk) )
					{
						m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
					}
					if ( (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER) &&
						 (lTempBufBlk > 0) && 
						 (m_stMgznRT[i].m_lSlotBlock[j] == lTempBufBlk) )
					{
						m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
					}
				}
			}

			szLog = "Manual Reset BL Output Table indexes";
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v3.74
		}
		else if ( lMgzn > 0 )	//Other Grade Magazines
		{
			BOOL bIsGradeMgzn = FALSE;
			if ((m_stMgznRT[lMgzn-1].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE))	// ||
				//(m_stMgznRT[lMgzn-1].m_lMgznUsage == BL_MGZN_USAGE_MIXED) )
			{
				bIsGradeMgzn = TRUE;
			}

			//v4.51A20	//Cree HZ
			if ((pApp->GetCustomerName() == "Cree") && bIsGradeMgzn)
			{
				CString szMsg;
				szMsg.Format("Clear bin counters for Mgzn (%s) ?", GetMagazineName(lMgzn-1));
				if (HmiMessage(szMsg, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
				{
					szLog = "Manual Reset and Clear (CREE) BL magazine " + GetMagazineName(lMgzn-1);
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
					ResetMagazine(lMgzn-1, FALSE, TRUE);	//v4.51A22
				}
			}
			else
			{
				// cannot reset bin as bin frame not clear die count
				if (IsMagazineCanReset(lMgzn-1, FALSE) == FALSE)
				{
					szContent.LoadString(HMB_BL_RESET_MAG_BIN_NOT_CLEARED);
					HmiMessage(szContent, szTitle);
					BOOL bReturn= FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}

				szLog = "Manual Reset BL magazine " + GetMagazineName(lMgzn-1);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				m_bChangeOMRecordFileName = TRUE;

				if (m_lOMRT == BL_MODE_F)
				{
					SetOperationMode_F(); // Reset
					SaveMgznOMData();
				}

				ResetMagazine(lMgzn-1);	
				m_bChangeOMRecordFileName = FALSE;
			}
		}		
	}

	TRY {
		SaveData();
		SendResetSlotEvent_7018(0, 0, TRUE);
	} CATCH (CFileException, e)
	{
	}
	END_CATCH

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

    return 1;
}

LONG CBinLoader::ResetSlotBarcode(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	ULONG ulSlotPhyBlock = 0;
    svMsg.GetMsg(sizeof(ULONG), &ulSlotPhyBlock);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bMagResetEmpty = pApp->GetFeatureStatus(MS896A_FUNC_BL_RESET_GRADEMAG_EMPTY);

	INT i=0, j=0;
	for(i=0; i<MS_BL_MGZN_NUM; i++)
    {
		if ( (pApp->GetCustomerName() == "Cree") && (m_lOMRT == BL_MODE_D) )	//v4.19	//Cree HuiZhou
		{
			for (j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
			{
				if ( m_stMgznRT[i].m_lSlotBlock[j] == ulSlotPhyBlock ) 
				{
					LONG lCurrMgzn=0, lCurrSlot=0;
					GetMgznSlot(ulSlotPhyBlock, lCurrMgzn, lCurrSlot);

					CString szLog;
					szLog.Format("Bin #%d: mgzn %d(%d), slot %d(%d) manual reset barcode by manual-ClearBinCounter: ", 
						ulSlotPhyBlock, i, lCurrMgzn, j, lCurrSlot);
					BL_DEBUGBOX(szLog + m_stMgznRT[i].m_SlotBCName[j]);

					m_stMgznRT[i].m_SlotBCName[j] = _T("");
					SaveBarcodeData(ulSlotPhyBlock, _T(""), lCurrMgzn, lCurrSlot);

					//v4.39T10
					if ( (bMagResetEmpty) )	// && (m_stMgznRT[lMgzn].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE) )
					{
						m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;

						szLog.Format("Bin #%d: mgzn %d(%d), slot %d(%d) manual reset state to ACTIVE2FULL", 
							ulSlotPhyBlock, i, lCurrMgzn, j, lCurrSlot);
						BL_DEBUGBOX(szLog);
					}

					bReturn = TRUE;
					break;
				}
			}
		}
		else
		{
			for (j=0; j<MS_BL_MGZN_SLOT; j++)
			{
				if ( m_stMgznRT[i].m_lSlotBlock[j] == ulSlotPhyBlock ) 
				{
					LONG lCurrMgzn=0, lCurrSlot=0;
					GetMgznSlot(ulSlotPhyBlock, lCurrMgzn, lCurrSlot);

					CString szLog;
					szLog.Format("Bin #%d: mgzn %d(%d), slot %d(%d) manual reset barcode by manual-ClearBinCounter: ", 
						ulSlotPhyBlock, i, lCurrMgzn, j, lCurrSlot);
					BL_DEBUGBOX(szLog + m_stMgznRT[i].m_SlotBCName[j]);

					m_stMgznRT[i].m_SlotBCName[j] = _T("");
					SaveBarcodeData(ulSlotPhyBlock, _T(""), lCurrMgzn, lCurrSlot);

					//v4.39T10
					if ( (bMagResetEmpty) )	// && (m_stMgznRT[lMgzn].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE) )
					{
						m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;

						szLog.Format("Bin #%d: mgzn %d(%d), slot %d(%d) manual reset state to ACTIVE2FULL", 
							ulSlotPhyBlock, i, lCurrMgzn, j, lCurrSlot);
						BL_DEBUGBOX(szLog);
					}

					bReturn = TRUE;
					break;
				}
			}
		}

		if (bReturn == TRUE)
			break;
    }

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CBinLoader::LoadUnloadBinTableCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bLoadUnloadBtToggle)
	{
		SetBinTableJoystick(FALSE);
		if ( MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y) == FALSE )
		{
			HmiMessage("Fail to move bintable to UNLOAD position!");
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		SetFrameAlign(FALSE);
		SetFrameVacuum(FALSE);
		
		if (m_bRealignBinFrame)
		{
			CMS896AStn::m_bRealignFrameDone  = FALSE;
			CMS896AStn::m_bRealignFrame2Done = FALSE;	//FOr MS100 9Inch dual-table config only
		}

		bReturn = FALSE;
		m_bLoadUnloadBtToggle = TRUE;
	}
	else
	{
		SetBinTableJoystick(FALSE);
		MoveBinTable(0, 0);

		AlignBinFrame();

		if (!m_bNoSensorCheck && (CheckFrameOnBinTable() == BL_FRAME_NOT_EXIST))
		{
			SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
			SetErrorMessage("BL: No frame existed on bintable in LoadUnloadBtCmd");
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		bReturn = TRUE;

		m_bLoadUnloadBtToggle = FALSE;
	}

	try
	{
		SaveData();
		//SaveMgznRTData();
	}
	catch(CFileException e) {
		//BL_DEBUGBOX("BL File Exception in Manual Load Buffer Frame");
	}

    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

BOOL CBinLoader::LoadUnloadBT_Semitek()
{
	BOOL bReturn = TRUE;

	if (!m_bLoadUnloadBtToggle)
	{
		SetBinTableJoystick(FALSE);
		if ( MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y) == FALSE )
		{
			HmiMessage("Fail to move bintable to UNLOAD position!");
			return FALSE;
		}

		SetFrameAlign(FALSE);
		SetFrameVacuum(FALSE);
		
		if (m_bRealignBinFrame)
		{
			CMS896AStn::m_bRealignFrameDone  = FALSE;
			CMS896AStn::m_bRealignFrame2Done = FALSE;	//FOr MS100 9Inch dual-table config only
		}

		bReturn = FALSE;
		m_bLoadUnloadBtToggle = TRUE;
	}
	else
	{
		SetBinTableJoystick(FALSE);

		AlignBinFrame();

		//v4.51A1
		if (IsMS60() && m_bUseBinTableVacuum && (m_lBTVacuumDelay > 0))	
		{
			Sleep(m_lBTVacuumDelay);	//Used AFTER Vac is applied, before BT motion
		}

		MoveBinTable(0, 0);

		if (!m_bNoSensorCheck && (CheckFrameOnBinTable() == BL_FRAME_NOT_EXIST))
		{
			SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
			SetErrorMessage("BL: No frame existed on bintable in LoadUnloadBtCmd");
			return FALSE;
		}

		bReturn = TRUE;

		m_bLoadUnloadBtToggle = FALSE;
	}

    return bReturn;
}


LONG CBinLoader::RealignBinFrameCmd(IPC_CServiceMessage &svMsg)
{
	int nConvID = 0;
    BOOL bReturn=TRUE;
	CString szContent;

//	BOOL bBtToggle = FALSE;
//	svMsg.GetMsg(sizeof(BOOL), &bBtToggle);	//v4.02T3

	if (m_bDisableBT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	//v4.59A42	//Finisar MS90 TX
	if (!CheckMS90BugPusherAtSafePos())
	{
		CString szMsg = "BL RealignBinFrameCmd: BT Pusher XZ not at SAFE position";
		SetErrorMessage(szMsg);
		SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);	
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;	
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("BL: Manual Realign Bin Frame start");

	IPC_CServiceMessage stMsg;
	BOOL bTemp = TRUE;
	
	stMsg.InitMessage(sizeof(BOOL), &bTemp);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetAlignBinMode", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);

	(*m_psmfSRam)["BinLoader"]["DB"]["BinFull"] = FALSE;	//Nichia//v4.43T7

    if ( m_bRealignBinFrame == FALSE )
	{
		//szContent.LoadString(HMB_BL_REALIGN_DISABLED);
		//HmiMessage(szContent);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//m_lBTCurrentBlock = 1;
//================================
//2018.1.15 harcode for debug
//	m_lBTCurrentBlock = 1;
//m_ucWTGrade = GetBinBlkGrade(ulBinBlk);	
//================================
    if ( GetCurrBinOnBT()<=0 || GetCurrBinOnBT()>MS_MAX_BIN )
	{
        //HmiMessage("Current block is invalid");
		SetAlert(IDS_BL_INVALID_LOGICAL_FRAME);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( RealignBinFrame(GetCurrBinOnBT())==TRUE )
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

		if (pApp->GetCustomerName() == "Cree")		//v4.53A6 //Now for Cree HZ only
		{
			//v4.51A19	//Silan MS90
			SetErrorMessage("BL: Manual Realign Bin Frame succeed 1");
			szContent.LoadString(HMB_BL_REALIGN_OK);
			HmiMessage(szContent);
		}
	}
    else
	{
		//bReturn = FALSE;
		SetErrorMessage("BL: Manual Realign Bin Frame fail");
		SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);		//v4.46T9	//v4.51A21
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("BL: Manual Realign Bin Frame done");
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CBinLoader::RealignBinFrameCmd1(IPC_CServiceMessage &svMsg)	//shiraishi02
{
	int nConvID = 0;
    BOOL bReturn=TRUE;
	CString szContent;

	BOOL bBtToggle = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBtToggle);	//v4.02T3

	if( m_bDisableBT )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	//v4.59A42	//Finisar MS90 TX
	if (!CheckMS90BugPusherAtSafePos())
	{
		CString szMsg = "BL RealignBinFrameCmd: BT Pusher XZ not at SAFE position";
		SetErrorMessage(szMsg);
		SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;	
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("BL: Manual Realign Bin Frame start 1");	//v4.59A36

	IPC_CServiceMessage stMsg;
	BOOL bTemp = TRUE;
	
	stMsg.InitMessage(sizeof(BOOL), &bTemp);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetAlignBinMode", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);

	(*m_psmfSRam)["BinLoader"]["DB"]["BinFull"] = FALSE;	//Nichia//v4.43T7

    if ( m_bRealignBinFrame == FALSE )
	{
		//szContent.LoadString(HMB_BL_REALIGN_DISABLED);
		//HmiMessage(szContent);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


    if ( GetCurrBinOnBT()<=0 || GetCurrBinOnBT()>MS_MAX_BIN )
	{
        //HmiMessage("Current block is invalid");
		//SetAlert(IDS_BL_INVALID_LOGICAL_FRAME);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( RealignBinFrame(GetCurrBinOnBT())==TRUE )
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

		if (pApp->GetCustomerName() == "Cree")		//v4.53A6 //Now for Cree HZ only
		{
			//v4.51A19	//Silan MS90
			SetErrorMessage("BL: Manual Realign Bin Frame succeed 1");
			szContent.LoadString(HMB_BL_REALIGN_OK);
			HmiMessage(szContent);
		}
	}
    else
	{
		//bReturn = FALSE;
		SetErrorMessage("BL: Manual Realign Bin Frame fail");
		SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);		//v4.46T9	//v4.51A21

		bReturn = FALSE;	//shiraishi02
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("BL: Manual Realign Bin Frame done");
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CBinLoader::RealignBinFrameCmd_F10(IPC_CServiceMessage &svMsg)
{
	int nConvID = 0;
    BOOL bReturn=TRUE;
	CString szContent;

	BOOL bBtToggle = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBtToggle);	//v4.02T3

	if( m_bDisableBT )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	CMSLogFileUtility::Instance()->MS_LogOperation("BL: Manual Realign Bin Frame start F10");	//v4.59A36

	//v4.02T1	//WH Semitek request
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( bBtToggle && (pApp->GetCustomerName() == "Semitek") )
	{
		Upper_MoveTo(0);

		BOOL bStatus = LoadUnloadBT_Semitek();

		if (!bStatus)
		{
			bReturn = TRUE;		//Still return TRUE to unlock HMI menu!
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}


 	CMSLogFileUtility::Instance()->MS_LogOperation("BL: Manual Realign Bin Frame F10 Set AlignBin Mode");	//v4.15T9

	IPC_CServiceMessage stMsg;
	BOOL bTemp = TRUE;
	
	/*
	if( pApp->GetCustomerName()=="Semitek" )
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("semitek Realign bin frame cmd no manual locate", "a+");
		bTemp = FALSE;
	}
	*/

	stMsg.InitMessage(sizeof(BOOL), &bTemp);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetAlignBinMode", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);

	(*m_psmfSRam)["BinLoader"]["DB"]["BinFull"] = FALSE;	//Nichia//v4.43T7

    if ( m_bRealignBinFrame == FALSE )
	{
		szContent.LoadString(HMB_BL_REALIGN_DISABLED);
		HmiMessage(szContent);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//m_lBTCurrentBlock = 1;
	if ( GetCurrBinOnBT()<=0 || GetCurrBinOnBT()>MS_MAX_BIN )
	{
		//HmiMessage("Current block is invalid");
		SetAlert(IDS_BL_INVALID_LOGICAL_FRAME);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	BOOL bRealignStatus = TRUE;
	if (CMS896AApp::m_bEnableSubBin == TRUE)	//Only for finisar SubBin function
	{
		DecodeSubBinSetting();
		CStringList szList;CString szTempList;LONG lSet = 0;
		for (int k = 1; k <= m_nNoOfSubBlk ; k ++)
		{
			if(GetCurrBinOnBT() == m_nSubBlk[k])
			{
				for (int j =1; j <= m_nNoOfSubGrade; j++)
				{
					szTempList.Format("%d", m_nSubGrade[k][j]);
					szList.AddTail(szTempList);
					CString sz;
					sz.Format("j,%d,m_nSubBlk[k],%d,m_nNoOfSubGrade,%d,m_nSubGrade[k][j],%d",j,m_nSubBlk[k],m_nNoOfSubGrade,m_nSubGrade[k][j]);
					CMSLogFileUtility::Instance()->MS_LogOperation(sz);
				}
			}
		}
		CString szContent = "Please choose Align Blk.";
		CString szTitle =  "";
		LONG lBlk = HmiSelection(szContent, szTitle, szList, lSet) + 1;
		bRealignStatus = RealignBinFrame(m_nSubGrade[GetCurrBinOnBT()][lBlk]);
	}
	else
	{
		bRealignStatus = RealignBinFrame(GetCurrBinOnBT());
	}

	if( bRealignStatus == TRUE )
	{
		SetErrorMessage("BL: Manual Realign Bin Frame succeed 2");
		szContent.LoadString(HMB_BL_REALIGN_OK);
		HmiMessage(szContent);
	}
    else
	{
		SetErrorMessage("BL: Manual Realign Bin Frame fail");
		//HmiMessage("Realign bin frame fail");
		SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);
	}

	m_lJsBinTableInUse = 0;		//BT1

	CMSLogFileUtility::Instance()->MS_LogOperation("BL: Manual Realign Bin Frame F10 done");
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CBinLoader::CheckBinFrameIsAligned(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

/*
	//v3.65
    if ( (m_bRealignBinFrame == TRUE) && (CMS896AStn::m_bRealignFrameDone == FALSE) )
	{
		if ( (CheckFrameOnBinTable() == BL_FRAME_ON_CENTER) && (GetCurrBinOnBT() != 0) )
		{
			bReturn = FALSE;
			SetErrorMessage("AUTOBOND: Bin frame not yet realigned");
			SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_NOTRDY);
		}
	}

	//v4.01	//Check BT frame level at PreStartCycle
	if (!IsBTFramePlatformDown())
	{
		bReturn = FALSE;
		SetErrorMessage("AUTOBOND: BT platform not DOWN");
		SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
	}
*/

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CBinLoader::CheckDualBufferFrameExist(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if ( ! m_bBurnInTestRun && m_bNoSensorCheck != TRUE )
	{
		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
		{
			if (DB_PreloadLevel() == BL_BUFFER_UPPER)
			{
				if (m_clLowerGripperBuffer.IsBufferBlockExist())
				{
					SetErrorMessage("Current Logical Block on Lower Buffer is not Zero");
					HmiMessage("Current Logical Block on Lower Buffer is not Zero");
					bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}

				if (IsDualBufferLowerExist() == TRUE)
				{
					SetErrorMessage("Frame Exists in Lower Buffer! Please unload it!");
					HmiMessage("Frame Exists in Lower Buffer! Please unload it!");
					bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}
			}
			else
			{
				if (m_clUpperGripperBuffer.IsBufferBlockExist())
				{
					SetErrorMessage("Current Logical Block on Upper Buffer is not Zero! Please check!");
					HmiMessage_Red_Yellow("Current Logical Block on Upper Buffer is not Zero! Please check!");
					bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}

				if (IsDualBufferUpperExist() == TRUE)
				{
					SetErrorMessage("Frame Exists in Upper Buffer! Please unload it!");
					HmiMessage_Red_Yellow("Frame Exists in Upper Buffer! Please unload it!");
					bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

/*
LONG CBinLoader::EnableRealign(IPC_CServiceMessage& svMsg)
{
    svMsg.GetMsg(sizeof(BOOL), &m_bRealignBinFrame);
    SaveData();

    return TRUE;
}
*/

// ====================================================================================================================
// ================================  run time magazine information display ============================================
// ====================================================================================================================
LONG CBinLoader::SelectMagazineRTDisplay(IPC_CServiceMessage &svMsg)
{
    LONG lMgzn;
    svMsg.GetMsg(sizeof(LONG), &lMgzn);

//	if (IsMSAutoLineMode())
//	{
//		m_lSelMagazineID = BL_MGZ_MID_1;
//		lMgzn = BL_MGZ_MID_1;
//	}
	m_lSelMagazineID = lMgzn;

	GenerateOMRTTableFile(lMgzn);
    return TRUE;
}

LONG CBinLoader::UpdateCurrMgznDisplayName(IPC_CServiceMessage& svMsg)
{
	m_szCurrMgznDisplayName = GetMagazineName(m_lCurrMgzn).MakeUpper();

	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


// ====================================================================================================================
// ===================================== setup operation mode for magazine ============================================
// ====================================================================================================================
LONG CBinLoader::SelectMgznUsage_UNUSE_TARGET(LONG lUsageOld, LONG lUsageTarget)
{
    CStringList szList;
    LONG lUsageNew, lIndex;

    lUsageNew = lUsageOld;
    szList.AddTail(GetMgznUsage(BL_MGZN_USAGE_UNUSE));
    szList.AddTail(GetMgznUsage(lUsageTarget));
    if( lUsageOld==BL_MGZN_USAGE_UNUSE )
        lIndex = 0;
    else
        lIndex = 1;
    lIndex = HmiSelection("please select magazine usage", "magazine setup", szList, lIndex);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("please select magazine usage");		//anichia001
    if( lIndex==0 )
        lUsageNew = BL_MGZN_USAGE_UNUSE;
    else if( lIndex==1 )
        lUsageNew = lUsageTarget;
    else
        lUsageNew = lUsageOld;

    return lUsageNew;
}

LONG CBinLoader::SelectMgznUsage_UNUSE_TARGET2(LONG lUsageOld, LONG lUsageTarget1, LONG lUsageTarget2)
{
    CStringList szList;
    LONG lUsageNew, lIndex;

    lUsageNew = lUsageOld;
    szList.AddTail(GetMgznUsage(BL_MGZN_USAGE_UNUSE));
    szList.AddTail(GetMgznUsage(lUsageTarget1));
    szList.AddTail(GetMgznUsage(lUsageTarget2));

    if( lUsageOld == BL_MGZN_USAGE_UNUSE )
        lIndex = 0;
    else
        lIndex = 1;

    lIndex = HmiSelection("please select magazine usage", "magazine setup", szList, lIndex);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("please select magazine usage");		//anichia001
    
	if (lIndex == 0)
        lUsageNew = BL_MGZN_USAGE_UNUSE;
    else if( lIndex == 1 )
        lUsageNew = lUsageTarget1;
    else if( lIndex == 2 )
        lUsageNew = lUsageTarget2;
    else
        lUsageNew = lUsageOld;

    return lUsageNew;
}

VOID CBinLoader::ChangeOMMgznSlotUsage(LONG lMgznIndex, LONG lUsageNew)
{
	if ( (lMgznIndex == BL_MGZN_TOP1) || (lMgznIndex == BL_MGZN_TOP2) )
	{
		for (INT i=0; i<MS_BL_MGZN_SLOT; i++)
		{
			if (lUsageNew == BL_MGZN_USAGE_EMPTY)
				m_stMgznOM[lMgznIndex].m_lSlotUsage[i] = BL_SLOT_USAGE_EMPTY;
			else if (lUsageNew == BL_MGZN_USAGE_FULL)
				m_stMgznOM[lMgznIndex].m_lSlotUsage[i] = BL_SLOT_USAGE_FULL;
			else
				m_stMgznOM[lMgznIndex].m_lSlotUsage[i] = BL_SLOT_USAGE_UNUSE;
		}
	}
}

LONG CBinLoader::SelectSlotUsage_UNUSE_TARGET(LONG lUsageOld, LONG lUsageTarget)
{
    CStringList szList;
    LONG lUsageNew, lIndex;

    lUsageNew = lUsageOld;
    szList.AddTail(GetSlotUsage(BL_SLOT_USAGE_UNUSE));
    szList.AddTail(GetSlotUsage(lUsageTarget));
    if( lUsageOld==BL_SLOT_USAGE_UNUSE )
        lIndex = 0;
    else
        lIndex = 1;
    lIndex = HmiSelection("please select slot usage", "magazine setup", szList, lIndex);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("please select slot usage 2");		//anichia001
    if( lIndex==0 )
        lUsageNew = BL_SLOT_USAGE_UNUSE;
    else if( lIndex==1 )
        lUsageNew = lUsageTarget;
    else
        lUsageNew = lUsageOld;

    return lUsageNew;
}

LONG CBinLoader::SelectSlotUsage_All(LONG lUsageOld)
{
    CStringList szList;
    LONG i, lUsageNew;

    for(i=0; i<4; i++)
        szList.AddTail(GetSlotUsage(i));
    lUsageNew = HmiSelection("please select slot usage", "slot setup", szList, lUsageOld);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("please select slot usage 3");		//anichia001
    if( lUsageNew<0 )
    {
        lUsageNew = lUsageOld;
    }
    return lUsageNew;
}

LONG CBinLoader::SetOperationMode(IPC_CServiceMessage& svMsg)
{
    int i;
    svMsg.GetMsg(sizeof(LONG), &m_lOMSP);


	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (m_bBLOut8MagConfig && (m_lOMSP<5))	//v3.82		//8Mag only support MODE F & G
	{
		m_lOMSP		= 5;
		m_szOMSP	= _T("F");
	}

    LoadMgznOMData();	//Load from custom MSD file

    for(i=0; i<MS_BL_MGZN_NUM; i++)
    {
        m_szBLMgznUse[i] = GetMgznUsage(m_stMgznOM[i].m_lMgznUsage);
    }
    m_ulMgznSelected = 0;
    m_ulSlotSelected = 1;
    m_szBLSlotUsage = GetOMSlotUsage(m_ulMgznSelected, 0);
    m_ulSlotPhyBlock= m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[0]; 

	//4.55T09 Set
	BOOL bCheckEmptyFrameBC	= pApp->GetFeatureStatus(MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK);

	if(bCheckEmptyFrameBC)
	{
		if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]==BL_SLOT_USAGE_ACTIVE ||m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]== BL_SLOT_USAGE_EMPTY)
			m_bSetSlotBlockEnable = TRUE;
		else
			m_bSetSlotBlockEnable = FALSE;
	}
	else
	{
		if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]==BL_SLOT_USAGE_ACTIVE)
			m_bSetSlotBlockEnable = TRUE;
		else
			m_bSetSlotBlockEnable = FALSE;
	}

    GenerateOMSPTableFile(m_ulMgznSelected);

	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}

LONG CBinLoader::SetMgznUsage(IPC_CServiceMessage &svMsg)
{
    LONG lUsageOld=0, lUsageNew=0, lMgznIndex=0;

    svMsg.GetMsg(sizeof(LONG), &lMgznIndex);
    lUsageOld = m_stMgznOM[lMgznIndex].m_lMgznUsage;

    switch( m_lOMSP )
    {
    case BL_MODE_A:
        switch( lMgznIndex )
        {
        case BL_MGZN_TOP1:		// unuse or empty
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_EMPTY);
            break;
        case BL_MGZN_TOP2:		// unuse or full
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_FULL);
            break;
        case BL_MGZN_MID1:		// unuse or active
        case BL_MGZN_BTM1:		// unuse or active
        case BL_MGZN_MID2:		// unuse or active
        case BL_MGZN_BTM2:		// unuse or active
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_ACTIVE);
            break;
        case BL_MGZN8_BTM1:		// unuse or active		//v3.82
        case BL_MGZN8_BTM2:		// unuse or active		//v3.82
			return 1;
       }
        break;

    case BL_MODE_B:
        switch( lMgznIndex )
        {
        case BL_MGZN_TOP1: // unuse or empty
        case BL_MGZN_MID1: // unuse or empty
        case BL_MGZN_BTM1: // unuse or empty
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_EMPTY);
            break;
        case BL_MGZN_TOP2: // unuse or full
        case BL_MGZN_MID2: // unuse or full
        case BL_MGZN_BTM2: // unuse or full
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_FULL);
            break;
        case BL_MGZN8_BTM1:		// unuse or active		//v3.82
        case BL_MGZN8_BTM2:		// unuse or active		//v3.82
			return 1;
        }
        break;

    case BL_MODE_C:
        switch( lMgznIndex )
        {
        case BL_MGZN_TOP1: // unuse or mixed
        case BL_MGZN_MID1: // unuse or mixed
        case BL_MGZN_BTM1: // unuse or mixed
        case BL_MGZN_TOP2: // unuse or mixed
        case BL_MGZN_MID2: // unuse or mixed
        case BL_MGZN_BTM2: // unuse or mixed
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_MIXED);
            break;
        case BL_MGZN8_BTM1:		// unuse or active		//v3.82
        case BL_MGZN8_BTM2:		// unuse or active		//v3.82
			return 1;
        }
        break;

    case BL_MODE_D:
        switch( lMgznIndex )
        {
        case BL_MGZN_TOP1: // unuse or mixed
        case BL_MGZN_MID1: // unuse or mixed
        case BL_MGZN_BTM1: // unuse or mixed
        case BL_MGZN_TOP2: // unuse or mixed
        case BL_MGZN_MID2: // unuse or mixed
        case BL_MGZN_BTM2: // unuse or mixed
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_MIXED);
            break;
        case BL_MGZN8_BTM1:		// unuse or active		//v3.82
        case BL_MGZN8_BTM2:		// unuse or active		//v3.82
			return 1;
       }
       break;

    case BL_MODE_E:			//v3.45
        switch( lMgznIndex )
        {
        case BL_MGZN_TOP1: // empty or full (mixed)
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_MIXED);
            break;
        case BL_MGZN_MID1: // unuse or active
        case BL_MGZN_BTM1: // unuse or active
        case BL_MGZN_TOP2: // unuse or active
        case BL_MGZN_MID2: // unuse or active
        case BL_MGZN_BTM2: // unuse or active
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_ACTIVE);
            break;
        case BL_MGZN8_BTM1:		// unuse or active		//v3.82
        case BL_MGZN8_BTM2:		// unuse or active		//v3.82
			return 1;
        }
        break;

     case BL_MODE_F:			//MS100 8mag config		//v3.82
        switch( lMgznIndex )
        {
        case BL_MGZN_TOP1:		// unuse or empty
            //lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_FULL);
			lUsageNew = SelectMgznUsage_UNUSE_TARGET2(lUsageOld, IsMSAutoLineMode() ? BL_MGZN_USAGE_ACTIVE : BL_MGZN_USAGE_FULL, BL_MGZN_USAGE_EMPTY);
            ChangeOMMgznSlotUsage(lMgznIndex, lUsageNew);
			GenerateOMSPTableFile(lMgznIndex);
			break;
        case BL_MGZN_MID1:		// unuse or active
        case BL_MGZN_BTM1:		// unuse or active
        case BL_MGZN8_BTM1:		// unuse or active
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_ACTIVE);
            break;

        case BL_MGZN_TOP2:		// unuse or full
            //lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_EMPTY);
			lUsageNew = SelectMgznUsage_UNUSE_TARGET2(lUsageOld, IsMSAutoLineMode() ? BL_MGZN_USAGE_ACTIVE : BL_MGZN_USAGE_EMPTY, BL_MGZN_USAGE_FULL);
            ChangeOMMgznSlotUsage(lMgznIndex, lUsageNew);
			GenerateOMSPTableFile(lMgznIndex);
            break;
        case BL_MGZN_MID2:		// unuse or active
        case BL_MGZN_BTM2:		// unuse or active
        case BL_MGZN8_BTM2:		// unuse or active
			lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_ACTIVE); 
			break;
        }
        break;

    case BL_MODE_G:				//MS100 8mag 175 config		//v3.82
        switch( lMgznIndex )
        {
        case BL_MGZN_TOP2:		// empty or full (mixed)
			lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_MIXED);
            break;
        case BL_MGZN_TOP1:		// unuse or active
        case BL_MGZN_MID1:		// unuse or active
        case BL_MGZN_BTM1:		// unuse or active
        case BL_MGZN_MID2:		// unuse or active
        case BL_MGZN_BTM2:		// unuse or active
        case BL_MGZN8_BTM1:		// unuse or active
        case BL_MGZN8_BTM2:		// unuse or active
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_ACTIVE);
            break;
        }
        break;

    case BL_MODE_H:				//MS100 4mag 100 config		//v4.31T10
        switch( lMgznIndex )
        {
        case BL_MGZN_TOP1:		// unuse or active
        case BL_MGZN_MID1:		// unuse or active
        case BL_MGZN_BTM1:		// unuse or active
        case BL_MGZN8_BTM1:		// unuse or active
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_ACTIVE);
            break;

        case BL_MGZN_TOP2:		// unuse
		case BL_MGZN_MID2:		// unuse
        case BL_MGZN_BTM2:		// unuse
        case BL_MGZN8_BTM2:		// unuse
            lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_UNUSE);
            break;
        }
        break;
	}

    m_stMgznOM[lMgznIndex].m_lMgznUsage = lUsageNew;
    m_szBLMgznUse[lMgznIndex] = GetMgznUsage(m_stMgznOM[lMgznIndex].m_lMgznUsage);
    return TRUE;
}

LONG CBinLoader::SelectOMSetupMgzn(IPC_CServiceMessage &svMsg)
{
    svMsg.GetMsg(sizeof(LONG), &m_ulMgznSelected);
    m_ulSlotSelected = 1;
    m_szBLSlotUsage = GetOMSlotUsage(m_ulMgznSelected, m_ulSlotSelected-1);
    m_ulSlotPhyBlock= m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected-1];

	//4.55T09 Select
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckEmptyFrameBC	= pApp->GetFeatureStatus(MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK);

	if(bCheckEmptyFrameBC)
	{
		if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]==BL_SLOT_USAGE_ACTIVE  || m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]== BL_SLOT_USAGE_EMPTY)
			m_bSetSlotBlockEnable = TRUE;
		else
			m_bSetSlotBlockEnable = FALSE;
	}
	else
	{
		if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]==BL_SLOT_USAGE_ACTIVE)
			m_bSetSlotBlockEnable = TRUE;
		else
			m_bSetSlotBlockEnable = FALSE;
	}

    GenerateOMSPTableFile(m_ulMgznSelected);

    return TRUE;
}

LONG CBinLoader::SelectOMSetupSlot(IPC_CServiceMessage &svMsg)
{
    svMsg.GetMsg(sizeof(LONG), &m_ulSlotSelected);

	//v4.19
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bUse29Slots = pApp->GetFeatureStatus(MS896A_FUNC_BL_USE_29_MAGSLOTS);
	if (!bUse29Slots && (m_ulSlotSelected > MS_BL_MGZN_SLOT))
	{
		m_ulSlotSelected = MS_BL_MGZN_SLOT;
	}

    m_szBLSlotUsage = GetOMSlotUsage(m_ulMgznSelected, m_ulSlotSelected-1);
    m_ulSlotPhyBlock= m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected-1];

	//4.55T09 Select
	BOOL bCheckEmptyFrameBC	= pApp->GetFeatureStatus(MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK);
	
	if(bCheckEmptyFrameBC)
	{
		if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected-1]==BL_SLOT_USAGE_ACTIVE || m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected-1] == BL_SLOT_USAGE_EMPTY )
			m_bSetSlotBlockEnable = TRUE;
		else
			m_bSetSlotBlockEnable = FALSE;
	}
	else
	{
    if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected-1]==BL_SLOT_USAGE_ACTIVE )
        m_bSetSlotBlockEnable = TRUE;
    else
        m_bSetSlotBlockEnable = FALSE;
	}

    return TRUE;
}

LONG CBinLoader::SetSlotUsage(IPC_CServiceMessage &svMsg)
{
    LONG lUsageOld=0, lUsageNew=0;

    lUsageOld = m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected-1];

    switch( m_lOMSP )
    {
    case BL_MODE_A:
        switch(m_ulMgznSelected)
        {
        case BL_MGZN_TOP1: // unuse or empty
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_EMPTY);
            break;
        case BL_MGZN_MID1: // unuse or active
        case BL_MGZN_BTM1: // unuse or active
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
        case BL_MGZN_TOP2: // unuse or full
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_FULL);
            break;
        case BL_MGZN_MID2: // unuse or active
        case BL_MGZN_BTM2: // unuse or active
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
		//v3.82
        case BL_MGZN8_BTM1:		
        case BL_MGZN8_BTM2:	
			return TRUE;
        }
        break;

    case BL_MODE_B:
        switch(m_ulMgznSelected)
        {
        case BL_MGZN_TOP1: // unuse or empty
        case BL_MGZN_MID1: // unuse or empty
        case BL_MGZN_BTM1: // unuse or empty
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_EMPTY);
            break;
        case BL_MGZN_TOP2: // unuse or full
        case BL_MGZN_MID2: // unuse or full
        case BL_MGZN_BTM2: // unuse or full
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_FULL);
            break;
        }
        break;

    case BL_MODE_C: // any selection
        lUsageNew = SelectSlotUsage_All(lUsageOld);
        break;

    case BL_MODE_D: // any selection
        lUsageNew = SelectSlotUsage_All(lUsageOld);
        break;

    case BL_MODE_E:		//v3.82
        switch(m_ulMgznSelected)
        {
        case BL_MGZN_TOP1: // unuse or empty
			if (m_ulSlotSelected == 1)
				lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_FULL);
			else
				lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_EMPTY);
            break;
        case BL_MGZN_MID1: // unuse or active
        case BL_MGZN_BTM1: // unuse or active
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
        case BL_MGZN_TOP2: // unuse or full
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
        case BL_MGZN_MID2: // unuse or active
        case BL_MGZN_BTM2: // unuse or active
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
		//v3.82
        case BL_MGZN8_BTM1:		
        case BL_MGZN8_BTM2:	
			return TRUE;
        }
        break;

    case BL_MODE_F:		//v3.82
        switch(m_ulMgznSelected)
        {
        case BL_MGZN_TOP1: // unuse or empty
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_FULL);
            break;
        case BL_MGZN_MID1: // unuse or active
        case BL_MGZN_BTM1: // unuse or active
        case BL_MGZN8_BTM1: // unuse or active
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
        case BL_MGZN_TOP2: // unuse or full
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_EMPTY);
            break;
        case BL_MGZN_MID2: // unuse or active
        case BL_MGZN_BTM2: // unuse or active
        case BL_MGZN8_BTM2: // unuse or active
           lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
        }
        break;

    case BL_MODE_G:		//v3.82
        switch(m_ulMgznSelected)
        {
        case BL_MGZN_TOP1: // unuse or empty
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
        case BL_MGZN_MID1: // unuse or active
        case BL_MGZN_BTM1: // unuse or active
        case BL_MGZN8_BTM1: // unuse or active
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
        case BL_MGZN_TOP2: // unuse or full
			if (m_ulSlotSelected == 1)
				lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_FULL);
			else
				lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_EMPTY);
            break;
        case BL_MGZN_MID2: // unuse or active
        case BL_MGZN_BTM2: // unuse or active
        case BL_MGZN8_BTM2: // unuse or active
           lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;
        }
        break;

    case BL_MODE_H:			//v4.31T10	//Yearly MS100Plus
        switch(m_ulMgznSelected)
        {
        case BL_MGZN_TOP1:	// unuse or active
        case BL_MGZN_MID1:	// unuse or active
        case BL_MGZN_BTM1:	// unuse or active
        case BL_MGZN8_BTM1: // unuse or active
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_ACTIVE);
            break;

        case BL_MGZN_TOP2:	// unuse
        case BL_MGZN_MID2:	// unuse
        case BL_MGZN_BTM2:	// unuse
        case BL_MGZN8_BTM2: // unuse
           lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, BL_SLOT_USAGE_UNUSE);
            break;
        }
        break;
    }

    m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected-1] = lUsageNew;
	if ( (lUsageNew == BL_SLOT_USAGE_FULL) || (lUsageNew == BL_SLOT_USAGE_EMPTY) )
	{
		m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected-1] = 0;			//v4.26T1
		m_ulSlotPhyBlock = 0;
	}
    m_szBLSlotUsage = GetSlotUsage(lUsageNew);

	//4.55T09 Set
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckEmptyFrameBC	= pApp->GetFeatureStatus(MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK);

	if(bCheckEmptyFrameBC)
	{
		if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected-1]==BL_SLOT_USAGE_ACTIVE || m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected-1]== BL_SLOT_USAGE_EMPTY)
			m_bSetSlotBlockEnable = TRUE;
		else
			m_bSetSlotBlockEnable = FALSE;
	}
	else
	{
		if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected-1]==BL_SLOT_USAGE_ACTIVE)
			m_bSetSlotBlockEnable = TRUE;
		else
			m_bSetSlotBlockEnable = FALSE;
	}


	//v4.52A17	//Cree HZ
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//DO not assign BlockNo for those UNUSE slots
	if (m_lOMSP == BL_MODE_A)
	{
		ReAssignSlotBlock_ModeA();
	}
	else if (m_lOMSP == BL_MODE_D)
	{
		ReAssignSlotBlock_ModeD();
	}

    GenerateOMSPTableFile(m_ulMgznSelected);
    return TRUE;
}

LONG CBinLoader::SetSlotBlock(IPC_CServiceMessage& svMsg)
{
    ULONG i, j;

    svMsg.GetMsg(sizeof(ULONG), &m_ulSlotPhyBlock);
    if( CheckPhysicalBlockValid(m_ulSlotPhyBlock)!=TRUE )
    {
		CString szMsg;
		szMsg.Format("\n%d",m_ulSlotPhyBlock);
		SetAlert_Msg_Red_Yellow(IDS_BL_NOTHIS_PHYBLK, szMsg);		
        m_ulSlotPhyBlock = m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected-1];
		SetErrorMessage("BL Invalid physical block");
        return TRUE;
    }

	//4.55T09 Set multi-use blks
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckEmptyFrameBC	= pApp->GetFeatureStatus(MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK);
	
	if( bCheckEmptyFrameBC )
	{
		BL_DEBUGBOX("SlotBlock can use multi PhyBlock for Checking Empty frame Barcode");
	}
	else
	{
        for(i=0; i<MS_BL_MGZN_NUM; i++)
        {
            for(j=0; j<MS_BL_MGZN_SLOT; j++)
            {
                if( m_stMgznOM[i].m_lSlotBlock[j]==m_ulSlotPhyBlock ) // && i!=m_ulMgznSelected && j!=(m_ulSlotSelected-1) 
                {
                    m_stMgznOM[i].m_lSlotBlock[j] = 0;
                }
            }
        }
	}

    m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected-1] = m_ulSlotPhyBlock;
    GenerateOMSPTableFile(m_ulMgznSelected);
    return TRUE;
}

LONG CBinLoader::SetMagazineOMSetupDefault(IPC_CServiceMessage &svMsg)
{
    LONG i;
	BOOL bReply = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//v3.82
	LONG lSelection = 0;
	CStringList szSelection;
	if (m_bBLOut8MagConfig)
	{
		//if (CMS896AApp::m_bMS100SingleLoaderOption == TRUE)
		if (m_bUseBLAsLoaderZ == TRUE)			//v4.44T1
		{
			szSelection.AddTail("Mode G");		//v4.36T2	New ModeG for Yealy MS100Plus
			szSelection.AddTail("Mode H");
		}
		else
		{
			szSelection.AddTail("Mode F");
			szSelection.AddTail("Mode G");
		}
	}
	else
	{
		szSelection.AddTail("Mode A");
		szSelection.AddTail("Mode B");
		szSelection.AddTail("Mode C");
		szSelection.AddTail("Mode D");
		szSelection.AddTail("Mode E");
	}

	LONG lResult = HmiSelection("Please select a DEFAULT mode", "BL Mgzn configuration", szSelection, lSelection);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("Please select a DEFAULT mode 4");		//anichia001

	if (lResult == -1)
	{
		bReply = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

CString szTemp;
szTemp.Format("MODE selection = %d", lResult);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	if (m_bBLOut8MagConfig)
	{
		//if (CMS896AApp::m_bMS100SingleLoaderOption == TRUE)		//v4.31T10
		if (m_bUseBLAsLoaderZ == TRUE)		//v4.44T1
		{
			if (lResult == 0)
				m_lOMSP = BL_MODE_G;		//v4.36T2
			else
				m_lOMSP = BL_MODE_H;
		}
		else
		{
			m_lOMSP = lResult + 5;
		}
	}
	else
		m_lOMSP = lResult;


    switch( m_lOMSP )
    {
    case BL_MODE_A:
        SetOperationMode_A();
        break;
    case BL_MODE_B:
        SetOperationMode_B();
        break;
    case BL_MODE_C:
        SetOperationMode_C();
        break;
    case BL_MODE_D:
        SetOperationMode_D();
        break;
    case BL_MODE_E:
        SetOperationMode_E();
        break;
    case BL_MODE_F:			//MS100 8Mag 150bins config
		SetOperationMode_F();
        break;
    case BL_MODE_G:			//MS100 8Mag 175bins config
        SetOperationMode_G();
        break;
    case BL_MODE_H:			//MS100 4Mag 100bins config		//YEarly MS100Plus
        SetOperationMode_H();
        break;
   default:
		SetAlert_Red_Yellow(IDS_BL_OPMODE_WRONG);
		SetErrorMessage("BL Incorrect operation mode");
		return TRUE;
    }

    for(i=0; i<MS_BL_MGZN_NUM; i++)
    {
        m_szBLMgznUse[i] = GetMgznUsage(m_stMgznOM[i].m_lMgznUsage);
    }
    m_ulMgznSelected = 0;
    m_ulSlotSelected = 1;
    m_szBLSlotUsage = GetOMSlotUsage(m_ulMgznSelected, m_ulSlotSelected-1);
    m_ulSlotPhyBlock = m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected-1];

    GenerateOMSPTableFile(m_ulMgznSelected);

	//v3.82
	switch (m_lOMSP)
	{
	case 1:		//MODE B
		m_szOMSP = _T("B");
		break;
	case 2:		//MODE C
		m_szOMSP = _T("C");
		break;
	case 3:		//MODE D
		m_szOMSP = _T("D");
		break;
	case 4:		//MODE E
		m_szOMSP = _T("E");
		break;
	case 5:		//MODE F
		m_szOMSP = _T("F");
		break;
	case 6:		//MODE G
		m_szOMSP = _T("G");
		break;
	case 7:		//MODE G
		m_szOMSP = _T("H");
		break;
	case 0:
	default:
		m_szOMSP = _T("A");
		break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
    return 1;
}

LONG CBinLoader::LoadMagazineOMSetup(IPC_CServiceMessage& svMsg)
{
    int i;
    //svMsg.GetMsg(sizeof(LONG), &m_lOMSP);

	//v3.82
	LONG lSelection = 0;
	CStringList szSelection;
	if (m_bBLOut8MagConfig)
	{
		//if (CMS896AApp::m_bMS100SingleLoaderOption == TRUE)		//v4.31T10
		if (m_bUseBLAsLoaderZ == TRUE)		//v4.44T1
		{
			szSelection.AddTail("Mode G");
			szSelection.AddTail("Mode H");
		}
		else
		{
			szSelection.AddTail("Mode F");
			szSelection.AddTail("Mode G");
		}
	}
	else
	{
		szSelection.AddTail("Mode A");
		szSelection.AddTail("Mode B");
		szSelection.AddTail("Mode C");
		szSelection.AddTail("Mode D");
		szSelection.AddTail("Mode E");
	}

	LONG lResult = HmiSelection("Please select a DEFAULT mode", "BL Mgzn configuration", szSelection, lSelection);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("Please select a DEFAULT mode 4");		//anichia001
	if (lResult == -1)
	{
		BOOL bReply = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

CString szTemp;
szTemp.Format("MODE selection = %d", lResult);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	if (m_bBLOut8MagConfig)
	{
		if (m_bUseBLAsLoaderZ == TRUE)	//v4.36T2
		{
			if (lResult == 0)
				m_lOMSP = BL_MODE_G;
			else
				m_lOMSP = BL_MODE_H;
		}
		else
			m_lOMSP = lResult + 5;
	}
	else
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetCustomerName() != "Cree") )
		{
			if ( (lResult == 1) || (lResult == 2) || (lResult == 3) )
			{
				HmiMessage("ERROR: selected mode is not supported!");
				BOOL bReply = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReply);
				return 1;
			}
		}

		m_lOMSP = lResult;
	}

    LoadMgznOMData();
    
	for(i=0; i<MS_BL_MGZN_NUM; i++)
    {
        m_szBLMgznUse[i] = GetMgznUsage(m_stMgznOM[i].m_lMgznUsage);
    }
    m_ulMgznSelected = 0;
    m_ulSlotSelected = 1;
    m_szBLSlotUsage = GetOMSlotUsage(m_ulMgznSelected, 0);
    m_ulSlotPhyBlock= m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[0]; 

	//4.55T09 Load
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckEmptyFrameBC	= pApp->GetFeatureStatus(MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK);

	if(bCheckEmptyFrameBC)
	{
		if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]==BL_SLOT_USAGE_ACTIVE ||m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]== BL_SLOT_USAGE_EMPTY)
			m_bSetSlotBlockEnable = TRUE;
		else
			m_bSetSlotBlockEnable = FALSE;
	}
	else
	{
		if( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]==BL_SLOT_USAGE_ACTIVE ||m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0]== BL_SLOT_USAGE_EMPTY)
			m_bSetSlotBlockEnable = TRUE;
		else
			m_bSetSlotBlockEnable = FALSE;
	}


    GenerateOMSPTableFile(m_ulMgznSelected);

	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;

}

LONG CBinLoader::SaveMagazineOMSetup(IPC_CServiceMessage &svMsg)
{
	if (SaveMgznOMData())
	{
		CString szMsg;
		szMsg = "NOTE: New customized MODE-" + m_szOMSP + " will only take effect when \nyou reset ALL magainzes next time.";
		HmiMessage(szMsg);
	}

	SaveData();

	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::UpdateOutput(IPC_CServiceMessage& svMsg)
{
	BOOL bUpdate = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bUpdate);

	m_bUpdateOutput = bUpdate;

	bUpdate = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bUpdate);
	return TRUE;
}


LONG CBinLoader::ShowHistory(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		LONG	lLeft;
		LONG	lTop;	
	} SETUP;
	SETUP stInfo;

	svMsg.GetMsg(sizeof(SETUP), &stInfo);


	CMachineStat dlg;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	dlg.szFileName	= BL_HISTORY_NAME;
	dlg.nStartLeft	= (int)stInfo.lLeft;	//45;
	dlg.nStartTop	= (int)stInfo.lTop;		//220;

	dlg.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinLoader::BinFrameSummary(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		LONG	lLeft;
		LONG	lTop;	
	} SETUP;
	SETUP stInfo;

	svMsg.GetMsg(sizeof(SETUP), &stInfo);

	CMachineStat dlg;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	//CString szSummaryFileName = CMSLogFileUtility::Instance()->BL_GetBinFrameStatusSummaryFilename();

	dlg.szFileName	= BL_BINFRAME_STATUS_SUMMARY;
	dlg.nStartLeft	= (int)stInfo.lLeft;	//45;
	dlg.nStartTop	= (int)stInfo.lTop;		//220;
	
	dlg.ShowClearAllButton(FALSE);

	dlg.DoModal();	// Show the file dialog box


	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::GetMagzFullFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szMagzFullFilePath);
		SaveData();		
		bReturn = TRUE;
	}

	pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CBinLoader::GetCreeBinBcSummaryFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	//CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	//((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szCreeBinBcSummaryPath);
		SaveData();		
		bReturn = TRUE;
	}

	//pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::ManualOutputSummaryFile(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	CString szMode;
	CString szTitle, szContent;


	//szMode.Format("Mode %c - Output File?", m_lOMRT + 'A');
	//if (HmiMessage(szMode, "Manual Output Summary File Message", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
	szMode.Format("Mode %c", m_lOMRT + 'A');
	if ( SetAlert_Msg(IDS_BL_OUTPUT_SUMMARY_FILE, szMode, "Yes", "No") == 1 )
	{
		switch( m_lOMRT )
		{
			case BL_MODE_B:
				OutputMagzFullSummaryFile(BL_MGZ_TOP_2, TRUE);	
				OutputMagzFullSummaryFile(BL_MGZ_MID_2, TRUE);	
				OutputMagzFullSummaryFile(BL_MGZ_BTM_2, TRUE);	
				break;

			case BL_MODE_C:
			case BL_MODE_D:
				OutputMagzFullSummaryFile(BL_MGZ_TOP_1, TRUE);	
				OutputMagzFullSummaryFile(BL_MGZ_MID_1, TRUE);	
				OutputMagzFullSummaryFile(BL_MGZ_BTM_1, TRUE);	
				OutputMagzFullSummaryFile(BL_MGZ_TOP_2, TRUE);	
				OutputMagzFullSummaryFile(BL_MGZ_MID_2, TRUE);	
				OutputMagzFullSummaryFile(BL_MGZ_BTM_2, TRUE);	
				break;

			case BL_MODE_E:		//v3.45
				OutputMagzFullSummaryFile(BL_MGZ_TOP_1, TRUE);
				break;

			case BL_MODE_F:		//MS100 8mag 150 config		//v3.82
				OutputMagzFullSummaryFile(BL_MGZ_TOP_1, TRUE);
				break;

			case BL_MODE_G:		//MS100 8mag 175 config		//v3.82
				OutputMagzFullSummaryFile(BL_MGZ_TOP_2, TRUE);
				break;

			case BL_MODE_H:		//MS100 4mag 100 config		//v4.31T10	//Yearly MS100Plus
				break;

			default:	//Mode A
				OutputMagzFullSummaryFile(BL_MGZ_TOP_2, TRUE);
				break;
		}

		szTitle.LoadString(HMB_BL_OUTPUT_SUMMARY_FILE);
		
		szContent.LoadString(HMB_GENERAL_COMPLETED);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CBinLoader::ClearAllMagazineFile(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szFileName;
	CString szTitle, szContent;
	LONG lMagNo;


	szTitle.LoadString(HMB_BL_OUTPUT_SUMMARY_FILE);
	szContent.LoadString(HMB_BL_CLEAR_SUMMARY_FILE);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		for (lMagNo = 0; lMagNo<MS_BL_MGZN_NUM; lMagNo++)
		{
			szFileName.Format("%d.txt", lMagNo);
			szFileName = BL_TEMP_MAG_NAME + szFileName;
			DeleteFile(szFileName);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CBinLoader::ClearAllFrame(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (CheckAllWaferLoaded() == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//v4.42T1
	//manual unload current frame first
	if (GetCurrBinOnBT() > 0)
	{
		IPC_CServiceMessage stMsg;
		ManualUnloadFilmFrame(stMsg);
		stMsg.GetMsg(sizeof(BOOL), &bReturn);
		if (!bReturn)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	if (ClearAllBinFrame() == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	UploadBackupOutputBinSummary();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

/*
LONG CBinLoader::ManualClearFrameForDBuffer(IPC_CServiceMessage& svMsg)	//v4.40T10	//Nichia
{
	BOOL bReturn = TRUE;

    LONG lBlock;
	LONG lStatus = TRUE;
	LONG lCurrMgzn=0, lCurrSlot=0;
	CString szCurrBarCode;
	CString szMsg;
	BOOL bPreLoadEmpty = FALSE;


	svMsg.GetMsg(sizeof(LONG), &lBlock);
    if( lBlock<1 || lBlock>MS_MAX_BIN )
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (!CheckLoadUnloadSafety())
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	//Only support DBuffer config (MS100 & MS100+)
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_BUFFER)	
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (m_clUpperGripperBuffer.IsBufferBlockExist() || m_clLowerGripperBuffer.IsBufferBlockExist())
	{
		szMsg.Format("ERROR: buffer table contains frame Upper=%d Lower=%d; please unload it first!",
						m_clUpperGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBlock());
		SetErrorMessage(szMsg);
		HmiMessage_Red_Yellow(szMsg, "Manual Clear Frame");

		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (lBlock == GetCurrBinOnBT())	//load empty frame
	{
		szMsg.Format("ERROR: Grade #%ld frame is on table; please unload using UNLOAD fcn", lBlock);
		SetErrorMessage(szMsg);
		HmiMessage_Red_Yellow(szMsg, "Manual Clear Frame");

		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	BOOL bDBufferLevel = BL_BUFFER_UPPER;		//v4.19T1	//CMLT

	SetBinTableJoystick(FALSE);
	
	StartLoadingAlert(); 

	LONG lLoadStatus = DB_LoadFromMgzToBuffer(m_bBurnInTestRun, lBlock, FALSE, bDBufferLevel);	//DEFAULT	
	if (lLoadStatus == TRUE)
	{
		lLoadStatus = DB_UnloadFromBufferToMgz(m_bBurnInTestRun, TRUE, TRUE, FALSE, FALSE, 
						bDBufferLevel, FALSE, FALSE, FALSE, TRUE);	
	}

	CloseLoadingAlert();

	SetBinTableJoystick(TRUE);

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Load Buffer Frame");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}
*/


LONG CBinLoader::ManualUploadBinSummary(IPC_CServiceMessage& svMsg)
{
	CString szTitle, szContent;
	BOOL bReturn = TRUE;

	if (UploadBackupOutputBinSummary() == FALSE)
	{
		szTitle.LoadString(HMB_BL_UPLOAD_SUMMARY);
		szContent.LoadString(HMB_BL_UPLOAD_SUMMARY_FAIL);
		HmiMessage(szContent, szTitle);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	szTitle.LoadString(HMB_BL_UPLOAD_SUMMARY);
	szContent.LoadString(HMB_BL_UPLOAD_SUMMARY_COMPLETE);
	HmiMessage(szContent, szTitle);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::MoveToSlot(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	LONG lMgzn = 0;
	ULONG ulCurrentSlot;

	typedef struct 
	{
		LONG	lMgzn;
		ULONG	ulSlot;
	} BLSETUP;
	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	lMgzn				= stInfo.lMgzn;
	m_ulSlotSelected	= stInfo.ulSlot;


	//v3.83
	if (!m_bBLOut8MagConfig)
	{
		if (lMgzn >= 6)		//8Mag BTM 1/2 not supported!
		{
			SetErrorMessage("8Mag BTM 1&2 not supported in 6Mag mode!");
			SetAlert_Red_Yellow(IDS_BL_8MAG_CONFIG_NOT_SUPPORT);
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	ulCurrentSlot = m_ulSlotSelected - 1;

	if ( ulCurrentSlot >= (ULONG)(m_stMgznRT[lMgzn].m_lNoOfSlots-1) )
	{
		ulCurrentSlot = m_stMgznRT[lMgzn].m_lNoOfSlots-1;
	}

	if ( m_fHardware == TRUE )
	{
		if (!CheckSafeToMove())
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		INT nPosT = GetThetaLoadPosn(lMgzn);
		INT nPosZ = m_stMgznRT[lMgzn].m_lTopLevel - m_stMgznRT[lMgzn].m_lSlotPitch * ulCurrentSlot;

		if (m_lUseLBufferGripperForSetup > 0)
		{
			nPosZ = m_stMgznRT[lMgzn].m_lTopLevel - m_stMgznRT[lMgzn].m_lSlotPitch * ulCurrentSlot + m_lUpperToLowerBufferOffsetZ;
		}

		INT nResultT = Theta_MoveTo(nPosT, SFM_NOWAIT);
		INT nResultZ = Z_SMoveTo(nPosZ, SFM_WAIT);

		while(1)
		{
			if ( (nResultT != gnOK) || (nResultZ != gnOK) )
			{
				bReturn = FALSE;
				break;
			}

			if ((CMS896AStn::MotionIsComplete(BL_AXIS_Z, &m_stBLAxis_Z) == TRUE) &&
				(CMS896AStn::MotionIsComplete(BL_AXIS_THETA, &m_stBLAxis_Theta) == TRUE))
			{
				Theta_Sync();
				Z_Sync();
				break;
			}
/*
			if (!CheckCoverOpen())
			{
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
*/
			Sleep(1);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}
/*
LONG CBinLoader::MoveToSlot2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	LONG lMgzn;
	ULONG ulCurrentSlot;

	svMsg.GetMsg(sizeof(LONG), &lMgzn);

	if (!m_bBLOut8MagConfig)
	{
		if (lMgzn >= 6)		//8Mag BTM 1/2 not supported!
		{
			SetErrorMessage("8Mag BTM 1&2 not supported in 6Mag mode!");
			SetAlert_Red_Yellow(IDS_BL_8MAG_CONFIG_NOT_SUPPORT);
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	ulCurrentSlot = m_ulSlotSelected - 1;

	if ( ulCurrentSlot >= (ULONG)(m_stMgznRT[lMgzn].m_lNoOfSlots-1) )
	{
		ulCurrentSlot = m_stMgznRT[lMgzn].m_lNoOfSlots-1;
	}

	if ( m_fHardware == TRUE )
	{
		if ( IsFrameOutOfMgz() == TRUE )
		{
			SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
			SetErrorMessage("BL Frame is out of magazine");
			
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if ( IsMagazine2SafeToMove() == FALSE )
		{
			SetErrorMessage("Gripper is not in safe position");
			SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);

			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}


		INT nResultY, nResultZ;
		INT nPosY, nPosZ;

		nPosY = m_stMgznRT[lMgzn].m_lMidPosnY2;
		nResultY = Y_MoveTo(nPosY, SFM_NOWAIT);

		if (m_bNewZConfig)
		{
			if ((lMgzn == 6) || (lMgzn < 3))	//v3.83
			{
				nPosZ = m_stMgznRT[lMgzn].m_lTopLevel2 + m_stMgznRT[lMgzn].m_lSlotPitch*ulCurrentSlot;
			}
			else
			{
				nPosZ = m_stMgznRT[lMgzn].m_lTopLevel2 - m_stMgznRT[lMgzn].m_lSlotPitch*ulCurrentSlot;
			}
		}
		else
		{
			nPosZ = m_stMgznRT[lMgzn].m_lTopLevel2 - m_stMgznRT[lMgzn].m_lSlotPitch*ulCurrentSlot;
		}
		nResultZ = Z_MoveTo(nPosZ, SFM_NOWAIT);

		while(1)
		{
			if ( (nResultY != gnOK) || (nResultZ != gnOK) )
			{
				bReturn = FALSE;
				break;
			}

			//if ( (m_pServo_Z->IsComplete() == TRUE) && (m_pServo_Y->IsComplete() == TRUE) )
			if (m_bDisableLoaderY)	//v4.57A7
			{
				if (CMS896AStn::MotionIsComplete(BL_AXIS_Z, &m_stBLAxis_Z) == TRUE)
				{
					Z_Sync();
					break;
				}
			}
			else
			{
				if ( (CMS896AStn::MotionIsComplete(BL_AXIS_Z, &m_stBLAxis_Z) == TRUE) && (CMS896AStn::MotionIsComplete(BL_AXIS_Y, &m_stBLAxis_Y) == TRUE) )
				{
					Z_Sync();
					Y_Sync();
					break;
				}
			}

			if ( IsElevatorCoverOpen() == TRUE )
			{
				//m_pServo_Z->Stop();
				CMS896AStn::MotionStop("BinLoaderZAxis", &m_stBLAxis_Z);
				Z_Sync();
				//m_pServo_Y->Stop();
				CMS896AStn::MotionStop("BinLoaderYAxis", &m_stBLAxis_Y);
				Y_Sync();

				if ( CheckElevatorCover() == FALSE )
				{
					if (IsFrameOutOfMgz() == FALSE)				
					{
						Sleep(500);
						Y_Home();
						Z_Home();
					}

					bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return TRUE;
				}
				else
				{
					nResultY = Y_MoveTo(nPosY, SFM_NOWAIT);
					nResultZ = Z_MoveTo(nPosZ, SFM_NOWAIT);
				}
			}
			
			if (IsCoverOpen() == TRUE)
			{
				//m_pServo_Z->Stop();
				CMS896AStn::MotionStop("BinLoaderZAxis", &m_stBLAxis_Z);
				Z_Sync();
				//m_pServo_Y->Stop();
				CMS896AStn::MotionStop("BinLoaderYAxis", &m_stBLAxis_Y);
				Y_Sync();

				if ( CheckCover() == FALSE )
				{
					if (IsFrameOutOfMgz() == FALSE)				
					{
						Sleep(500);
						Y_Home();
						Z_Home();
					}

					bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return TRUE;
				}
				else
				{
					nResultY = Y_MoveTo(nPosY, SFM_NOWAIT);
					nResultZ = Z_MoveTo(nPosZ, SFM_NOWAIT);
				}
			}

			Sleep(1);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}
*/

LONG CBinLoader::ChangeArmPosition(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;

	svMsg.GetMsg(sizeof(LONG), &lPosition);

	switch(lPosition)
	{
		default:	// Exchange Arm Pick position
			Arm_MoveTo(m_lExArmPickPos);
			m_lBLGeneral_TmpA = m_lExArmPickPos;
			break;

		case 1:		// Exchange Arm Ready position
			Arm_MoveTo(m_lExArmReadyPos);
			m_lBLGeneral_TmpA = m_lExArmReadyPos;
			break;

		case 2:		// Exchange Arm Place position
			Arm_MoveTo(m_lExArmPlacePos);
			m_lBLGeneral_TmpA = m_lExArmPlacePos;
			break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

    return TRUE;
}



LONG CBinLoader::KeyInArmPosition(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;
	BLSETUP stInfo;


	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);


	//Check KeyIn Positon Limit
    // get exchange arm position limit
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BINEXCHGARMR, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINEXCHGARMR, MS896A_CFG_CH_MAX_DISTANCE);								

	if (stInfo.lStep < lMinPos) 
	{
		stInfo.lStep = lMinPos;
	}

	if (stInfo.lStep > lMaxPos)
	{
		stInfo.lStep = lMaxPos;
	}

	switch(stInfo.lPosition)
	{
		default:	// Exchange Arm Pick position
			m_lBLGeneral_1 = stInfo.lStep;
			Arm_MoveTo(m_lBLGeneral_1);
			break;

		case 1:		// Exchange Arm Ready position
			m_lBLGeneral_2 = stInfo.lStep;
			Arm_MoveTo(m_lBLGeneral_2);
			break;

		case 2:		// Exchange Arm Place position
			m_lBLGeneral_3 = stInfo.lStep;
			Arm_MoveTo(m_lBLGeneral_3);
			break;
	
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CBinLoader::MovePosArmPosition(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;

	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	switch(stInfo.lPosition)
	{
		default:	lCurrentPos = m_lBLGeneral_1 + stInfo.lStep;	break;	// Exchange Arm Pick position	 
		case 1:		lCurrentPos = m_lBLGeneral_2 + stInfo.lStep;	break;	// Exchange Arm Ready position		
		case 2:		lCurrentPos = m_lBLGeneral_3 + stInfo.lStep;	break;	// Exchange Arm Place position		
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BINEXCHGARMR, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINEXCHGARMR, MS896A_CFG_CH_MAX_DISTANCE);								

	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		Arm_MoveTo(lCurrentPos);

		switch(stInfo.lPosition)
		{
			default:	m_lBLGeneral_1 = lCurrentPos;	break;
			case 1:		m_lBLGeneral_2 = lCurrentPos;	break;
			case 2:		m_lBLGeneral_3 = lCurrentPos;	break;
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CBinLoader::MoveNegArmPosition(IPC_CServiceMessage& svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;

	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	switch(stInfo.lPosition)
	{
		default:	lCurrentPos = m_lBLGeneral_1 - stInfo.lStep;	break;	// Exchange Arm Pick position	 
		case 1:		lCurrentPos = m_lBLGeneral_2 - stInfo.lStep;	break;	// Exchange Arm Ready position		
		case 2:		lCurrentPos = m_lBLGeneral_3 - stInfo.lStep;	break;	// Exchange Arm Ready position		
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BINEXCHGARMR, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINEXCHGARMR, MS896A_CFG_CH_MAX_DISTANCE);								

	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		Arm_MoveTo(lCurrentPos);

		switch(stInfo.lPosition)
		{
			default:	m_lBLGeneral_1 = lCurrentPos;	break;
			case 1:		m_lBLGeneral_2 = lCurrentPos;	break;
			case 2:		m_lBLGeneral_3 = lCurrentPos;	break;
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CBinLoader::CancelArmSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bHome = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bHome);

	//Restore HMI variable
    m_lBLGeneral_1 = m_lExArmPickPos;
	m_lBLGeneral_2 = m_lExArmReadyPos;		
	m_lBLGeneral_3 = m_lExArmPlacePos;

	//Move Arm back to Home pos
	if ( bHome == TRUE )
	{
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CBinLoader::ConfirmArmSetup(IPC_CServiceMessage& svMsg)
{

	if (m_lExArmPickPos != m_lBLGeneral_1)
	{
		m_lExArmPickPos = m_lBLGeneral_1;
		LogItems(EXCHANGE_ARM_PICK_POS);
	}
	else if (m_lExArmPlacePos != m_lBLGeneral_3)
	{
		m_lExArmPlacePos = m_lBLGeneral_3;
		LogItems(EXCHANGE_ARM_PLACE_POS);
	}

	//Update variable
	m_lExArmPickPos  = m_lBLGeneral_1;
	m_lExArmReadyPos = m_lBLGeneral_2;		
	m_lExArmPlacePos = m_lBLGeneral_3;

	SaveData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}



LONG CBinLoader::MoveArmToChangePos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bMoveToPick = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bMoveToPick);


	//Check Arm are in ready position
	if ( IsExchangeArmReady() == FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if ( bMoveToPick == TRUE )
	{
		Arm_MoveTo(m_lExArmPickPos);
	}
	else
	{
		Arm_MoveTo(m_lExArmPlacePos);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}


LONG CBinLoader::ExchangeFrameTest(IPC_CServiceMessage& svMsg)
{
	CString szTempBarcode = "";
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	BOOL bExchangeTest = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bExchangeTest);

	SetBinTableJoystick(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	SetBinTableJoystick(TRUE);
	return TRUE;
}


BOOL CBinLoader::MoveWaferTableToSafePosn(CONST BOOL bSafe, CONST BOOL bOffline, CONST BOOL bWaitWT)
{
	if( !m_bUseLargeBinArea && !Is180Arm6InchWT() )
		return TRUE;

	if( m_bDisableWT )
		return TRUE;

	BOOL bResult = TRUE;
	BOOL bToSafePosn = bSafe;

	//v4.53A21
	BOOL bWaitWTtoSafePos = bWaitWT;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "Lumileds")
		bWaitWTtoSafePos = TRUE;

	if( bOffline || m_bStop )
	{
		IPC_CServiceMessage stMsg;
		int nConvID = 0;

		if( bOffline )
		{
			WftToSafeLog("BL offline 6 inch wft move to safe position", m_bUseLargeBinArea);
		}

		if( m_bStop )
		{
			WftToSafeLog("bl stopped by operator");
			if( bToSafePosn )
			{
				WftToSafeLog("bl IPC let wft to safe", m_bUseLargeBinArea);
			}
			else
			{
				WftToSafeLog("bl IPC let wft to original", m_bUseLargeBinArea);
			}
		}
		stMsg.InitMessage(sizeof(BOOL), &bToSafePosn);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, _T("MoveWaferTableToSafePosn"), stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		stMsg.GetMsg(sizeof(BOOL), &bResult);
	}
	else
	{
		LONG lMoveWftState = 0;
		LONG lWaitCounter=0;
		if( bToSafePosn )
		{
			WftToSafeLog("bl auto let wft to safe", m_bUseLargeBinArea);
			(*m_psmfSRam)["BinLoader"]["MoveWftToSafeState"] = 1;
		}
		else
		{
			WftToSafeLog("bl auto let wft to original", m_bUseLargeBinArea);
			(*m_psmfSRam)["BinLoader"]["MoveWftToSafeState"] = 3;
		}

		if (bWaitWTtoSafePos)
		{
			while( 1 )
			{
				Sleep(10);
				lWaitCounter++;
				lMoveWftState = (*m_psmfSRam)["BinLoader"]["MoveWftToSafeState"];
				if( lMoveWftState==2 && bToSafePosn )
				{
					WftToSafeLog("bl auto wft to safe complete", m_bUseLargeBinArea);
					break;
				}
				if( (lMoveWftState==4 && bToSafePosn==FALSE) )
				{
					WftToSafeLog("bl auto wft to original complete", m_bUseLargeBinArea);
					break;
				}
				if( lWaitCounter>1000 )
				{
					WftToSafeLog("bl break by time out 10 seconds", m_bUseLargeBinArea);
					break;
				}
			}
		}
		else
		{
			WftToSafeLog("bl auto wft to original bypass to complete", m_bUseLargeBinArea);
			Sleep(500);
		}

		if( bToSafePosn==FALSE || m_bStop )
		{
			//v4.05	//Klocwork
			//if( m_bStop )
			//{
			//	WftToSafeLog("bl stopped by operator");
			//}
			WftToSafeLog("bl set to 0, run into normal cycle", m_bUseLargeBinArea);
			(*m_psmfSRam)["BinLoader"]["MoveWftToSafeState"] = 0;
		}
	}

	return bResult;
}


LONG CBinLoader::ManualLoadBufferFrame(IPC_CServiceMessage& svMsg)
{
    LONG lBlock;
	LONG lStatus = TRUE;
	BOOL bReturn=TRUE;

	svMsg.GetMsg(sizeof(LONG), &lBlock);

    if( lBlock<1 || lBlock>MS_MAX_BIN )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (!CheckLoadUnloadSafety())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::ManualUnloadBufferFrame(IPC_CServiceMessage& svMsg)
{
	//BOOL bFull;
	BOOL bReturn = TRUE;
	LONG lStatus = TRUE;

	if (!CheckLoadUnloadSafety())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	SetBinTableJoystick(FALSE);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::ExArmPreloadFrameForNextWafer(IPC_CServiceMessage& svMsg)
{
	BOOL bBinFullNoPreLoad = FALSE;
	BOOL bReturn = TRUE;
	LONG lBlock = 0;
	svMsg.GetMsg(sizeof(LONG), &lBlock);


	//if (!m_bIsExChgArmExist)
	if (!m_bIsDualBufferExist)		//pllm
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	bBinFullNoPreLoad = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinFull"];

	// Make Sure Full Frame unload to full magazine first
	if (bBinFullNoPreLoad == TRUE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	//Next map grade not received from BT
	if ((lBlock <= 0) || (lBlock > 100))	//v2.78T1
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	//if next grade is same as current grade frame, then no need to change
	if (lBlock == GetCurrBinOnBT())	
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (!CheckLoadUnloadSafety())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	SetBinTableJoystick(FALSE);

	LONG lStatus = TRUE;
/*
	if (m_bIsDualBufferExist)	//DL with Buffer Table
	{
		//if upper buffer has frame, unload it first
		if (m_clUpperGripperBuffer.IsBufferBlockExist())
		{
			if ( DB_UnloadFromBufferToMgz(m_bBurnInTestRun, FALSE, FALSE, FALSE, FALSE, BL_BUFFER_UPPER) == FALSE )
			{
				SetErrorMessage("BL: auto Preload failure - UnloadUpperBufferFrame");
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		//if lower buffer has frame, unload it first		
		if (m_clLowerGripperBuffer.IsBufferBlockExist())
		{
			if ( DB_UnloadFromBufferToMgz(m_bBurnInTestRun, FALSE, FALSE, FALSE, FALSE, BL_BUFFER_LOWER) == FALSE )
			{
				SetErrorMessage("BL: auto Preload failure - UnloadLowerBufferFrame");
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}

		if (GetCurrBinOnBT() != 0)
		{
			UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, FALSE, FALSE);		//andrew7890
		}

		INT	nLoadFrameStatus = LoadBinFrame(m_bBurnInTestRun, &m_clUpperGripperBuffer, lBlock);
		if (nLoadFrameStatus == BL_FRAME_WT_NO_BAR_CODE)
		{
			SetErrorMessage("Manual push back bin frame due to unread barcode");
			SetAlert_Red_Yellow(IDS_BL_PUSH_BACK_FRAME_TO_SLOT);
		}

		//andrew789
		if (nLoadFrameStatus == Err_BLLoadEmptyBarcodeFail)	
		{
			BOOL bBufferLevel = BL_BUFFER_UPPER;
			BOOL bBcUpdate = FALSE;
			if (m_bUseBarcode)
			{
				m_bUseBarcode = FALSE;
				bBcUpdate = TRUE;
			}

			DB_UnloadFromBufferToMgz(m_bBurnInEnable, TRUE, FALSE, FALSE, FALSE, bBufferLevel, FALSE, FALSE);
			
			if (bBcUpdate)
				m_bUseBarcode = TRUE;

			LONG lCurrMgzn=0, lCurrSlot=0;
			GetMgznSlot(lBlock, lCurrMgzn, lCurrSlot);
			m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
			m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot].Empty();
		}
	}
*/

	bReturn = (BOOL) lStatus;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::ExArmUnloadLastFrame(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


//Control the Display of Magazine Sensors in the HMI
LONG CBinLoader::MagazineSensorDisplay(IPC_CServiceMessage& svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	if ((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) || 
		CMS896AStn::m_bUseBinMultiMgznSnrs)
	{
		m_bDisplaySingleMgznSnr	= FALSE;
		m_bDisplayMultiMgznSnrs	= TRUE;
	}
	else
	{	
		m_bDisplaySingleMgznSnr	= TRUE;
		m_bDisplayMultiMgznSnrs	= FALSE;
	}

	return TRUE;
}

/*
// Temp to make Buffer Align parameters same as BT Align parameters
LONG CBinLoader::SetBufferAlignParameters(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct
	{
		LONG lBTAlignFrameCount;
		LONG lBTAlignFrameDelay;	
	}ALIGN_PARAM;

	ALIGN_PARAM stInfo;

	svMsg.GetMsg(sizeof(ALIGN_PARAM), &stInfo);
	
	// Buffer Table
	m_lBUTAlignFrameCount = stInfo.lBTAlignFrameCount;
	m_lBUTAlignFrameDelay = stInfo.lBTAlignFrameDelay;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	
	return TRUE;
}
*/

/*
LONG CBinLoader::UpdateArmProfile(IPC_CServiceMessage& svMsg)
{
	typedef struct
	{
		LONG lJerk;
		LONG lAcc;
		LONG lDec;
		LONG lVmax;
	} PROFILE;

	SFM_CProfile stProfile;
	PROFILE stInfo;


	svMsg.GetMsg(sizeof(PROFILE), &stInfo);

	stProfile.SetJerk(stInfo.lJerk);
	stProfile.SetAcceleration(stInfo.lAcc);
	stProfile.SetDeceleration(stInfo.lDec);
	stProfile.SetVelocity(stInfo.lVmax);

	m_pStepper_Arm->SetProfile("mpfBinExchgArm", stProfile);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}
*/

LONG CBinLoader::CheckBinLoaderInSafeStatusCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bNoSensorCheck && IsFrameInClamp())		//v3.84
	{
		SetAlert_Red_Yellow(IDS_BL_FRMAE_DETECTED_IN_BL_ERR_HANDLE);
		SetErrorMessage("Frame is detected on gripper");
		bReturn = FALSE;
	}

	if (!m_bNoSensorCheck && IsFrameJam())
	{
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);
		SetErrorMessage("Bin Gripper Jam");
		bReturn = FALSE;
	}

	if (IsFrameOutOfMgz() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		SetErrorMessage("BL Frame is out of magazine");
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinLoader::CheckExArmInSafePos(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::GenerateConfigData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMSLogFileUtility::Instance()->MS_LogOperation("BL: Generate Configuration Data");
	bReturn = GenerateConfigData();
	CMSLogFileUtility::Instance()->MS_LogOperation("BL: Generate Configuration Data done");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::TestFcn(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableBL)	
	{
		HmiMessage("Warning: BL module is disabled!");
		//return TRUE;
	}
	if ( m_bNoSensorCheck == TRUE )
	{
		HmiMessage("Warning: no-sensor-check option is enabled!");
		//return TRUE;	//OK -> assume frame exist
	}
	if (!m_bUseBinTableVacuum)
	{
		HmiMessage("Warning: Bin-Vac option is not enabled!");
		//return TRUE;	//OK -> assume frame exist
	}


    CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckVacMeter = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_BT_VAC_METER_CHECKING);

	if (!bCheckVacMeter)
	{
		HmiMessage("Warning: table-vac-check feature is not enabled for this customer; please consult MS product group");
		//return TRUE;	//OK -> assume frame exist
	}

	if (CheckFrameExistByVacuum())
	{
		HmiMessage("Test: Frame exist");
	}
	else
	{
		HmiMessage("Test: Frame not exist!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::TestFrameLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bToggle = FALSE;
	DOUBLE dStartTime=0, dEndTime = 0;

	BOOL bReturn = TRUE;

	CString szMsg;
	szMsg.Format("Frame level test start %d ...", m_lFrameLevelTestCount);
	HmiMessage(szMsg);

	for (INT i=0; i<m_lFrameLevelTestCount; i++)
	{
		dStartTime = GetTime();
		INT nCount = 0;
		
		if (!bToggle)
		{
			SetFrameLevel(TRUE);
			Sleep(500);
		}
		else
		{
			SetFrameLevel(FALSE);
			Sleep(500);
		}


		if (bToggle)
		{
			while (!IsFrameLevel())
			{
				Sleep(1);
				nCount++;
				if (nCount > 5000)
					break;
			}

			if (nCount >= 5000)
			{
				HmiMessage("Time out");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			if (!IsFrameLevel())
			{
				HmiMessage("Frame level still UP!");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			dEndTime = GetTime() - dStartTime;
			CString szTemp;
			szTemp.Format("Frame Level time = %.1f", dEndTime);
			SetErrorMessage(szTemp);

			Sleep(500);
		}

		if (bToggle)
			bToggle = FALSE;
		else
			bToggle = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::TestBufferLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bToggle = FALSE;
	DOUBLE dStartTime=0, dEndTime = 0;

	BOOL bReturn = TRUE;

	CString szMsg;
	szMsg.Format("Buffer level test start %d ...", m_lFrameLevelTestCount);
	HmiMessage(szMsg);

	for (INT i=0; i<m_lFrameLevelTestCount; i++)
	{
		dStartTime = GetTime();
		INT nCount = 0;
		
		if (!bToggle)
		{
			SetBufferLevel(TRUE);
			Sleep(1000);
		}
		else
		{
			SetBufferLevel(FALSE);
		}


		if (bToggle)
		{
			while (!IsDualBufferLevelDown())
			{
				Sleep(1);
				nCount++;
				if (nCount > 5000)
					break;
			}

			if (nCount >= 5000)
			{
				HmiMessage("Time out");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			if (!IsDualBufferLevelDown())
			{
				HmiMessage("Buffer level still UP!");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			dEndTime = GetTime() - dStartTime;
			CString szTemp;
			szTemp.Format("Buffer Level time = %.1f", dEndTime);
			SetErrorMessage(szTemp);

			Sleep(500);
		}

		if (bToggle)
			bToggle = FALSE;
		else
			bToggle = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::TestBothLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bToggle = FALSE;
	DOUBLE dStartTime=0, dEndTime = 0;

	BOOL bReturn = TRUE;

	CString szMsg;
	szMsg.Format("Both level test start %d ...", m_lFrameLevelTestCount);
	HmiMessage(szMsg);

	for (INT i=0; i<m_lFrameLevelTestCount; i++)
	{
		
		dStartTime = GetTime();
		INT nCount = 0;
		
		if (!bToggle)
		{
			SetFrameLevel(TRUE);
			Sleep(1000);
		}
		else
		{
			SetFrameLevel(FALSE);
		}

		if (bToggle)
		{
			while (!IsFrameLevel())
			{
				Sleep(1);
				nCount++;
				if (nCount > 5000)
					break;
			}

			if (nCount >= 5000)
			{
				SetErrorMessage("Frame level Time out");
				HmiMessage_Red_Yellow("Frame level Time out");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			if (!IsFrameLevel())
			{
				HmiMessage("Frame level still UP!");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			dEndTime = GetTime() - dStartTime;
			CString szTemp;
			szTemp.Format("%d Frame Level time = %.1f", i+1, dEndTime);
			SetErrorMessage(szTemp);

			Sleep(100);
		}

		dStartTime = GetTime();
		
		if (!bToggle)
		{
			SetBufferLevel(TRUE);
			Sleep(1000);
		}
		else
		{
			SetBufferLevel(FALSE);
		}

		if (bToggle)
		{
			while (!IsDualBufferLevelDown())
			{
				Sleep(1);
				nCount++;
				if (nCount > 5000)
					break;
			}

			if (nCount >= 5000)
			{
				SetErrorMessage("Buffer level Time out");
				HmiMessage_Red_Yellow("Buffer level Time out");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
			if (!IsDualBufferLevelDown())
			{
				HmiMessage("Buffer level still UP!");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			dEndTime = GetTime() - dStartTime;
			CString szTemp;
			szTemp.Format("%d Buffer Level time = %.1f", i+1, dEndTime);
			SetErrorMessage(szTemp);

			Sleep(500);
		}

		if (bToggle)
			bToggle = FALSE;
		else
			bToggle = TRUE;
	}

	SetErrorMessage("Test Finish");
	HmiMessage("Test Finish");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::DB_ManualLoadFromMgzToBuffer(IPC_CServiceMessage& svMsg)
{
    LONG lBlock = 0;
	LONG lStatus = TRUE;
	BOOL bReturn = TRUE;
	LONG lCurrMgzn = 0, lCurrSlot = 0;
	CString szCurrBarCode;
	BOOL bPreLoadEmpty = FALSE;
	LONG lLoadStatus = 0;

	BOOL bBufferLevel = BL_BUFFER_UPPER;
	if (m_lUseLBufferGripperForSetup)
	{
		bBufferLevel = BL_BUFFER_LOWER;
	}
	CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
	LONG lBufferBlock = pGripperBuffer->GetBufferBlock();

	svMsg.GetMsg(sizeof(LONG), &lBlock);


    if (lBlock < 1 || lBlock > MS_MAX_BIN)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (!CheckLoadUnloadSafety())
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	SetBinTableJoystick(FALSE);
	
	//unload a frame from buffer to magazine firstly
	if (lBufferBlock > 0)
	{
		if (lBufferBlock != lBlock)
		{
//			if (m_lBinLoaderConfig	== BL_CONFIG_DL_WITH_UPDN_BUFFER)	//MS50_01
			{
				lStatus = UDB_UnloadFromBufferToMgzWithLock(m_bBurnInTestRun, FALSE, pGripperBuffer);
			}
//			else
//			{
//				lStatus = DB_UnloadFromBufferToMgz(m_bBurnInTestRun, FALSE, FALSE, FALSE, FALSE, bBufferLevel);
//			}
		}
	}

	//Load a frame from magazine to buffer
	if (lStatus != FALSE)
	{
		if (lBlock == GetCurrBinOnBT())	//load empty frame
		{
			HmiMessage("Frame to be ejected is currently on table!  Please use UNLOAD fcn to unload to FULL magazine!");
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		lBufferBlock = m_clUpperGripperBuffer.GetBufferBlock();
//		if (m_lBinLoaderConfig	== BL_CONFIG_DL_WITH_UPDN_BUFFER)		//MS50_01
//		{
			lStatus = UDB_SubLoadFromMgzToBufferWithLock(m_bBurnInTestRun, lBlock, pGripperBuffer);
			Sleep(200);
			if (lStatus != FALSE)
			{
				if (!DownElevatorToReady(pGripperBuffer))
				{
					return FALSE;
				}
			}
//		}
//		else
//		{
//			lStatus = DB_LoadFromMgzToBuffer(m_bBurnInTestRun, lBlock, FALSE, bBufferLevel);
//		}
	}

	SetBinTableJoystick(TRUE);

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Load Buffer Frame");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::DB_ManualUnloadFromBufferToMgz(IPC_CServiceMessage& svMsg)
{
	BOOL bFull;
	BOOL bReturn = TRUE;
	LONG lStatus = TRUE;

	if (!CheckLoadUnloadSafety())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	BOOL bBufferLevel = BL_BUFFER_UPPER;
	if (m_lUseLBufferGripperForSetup)
	{
		bBufferLevel = BL_BUFFER_LOWER;
	}

	CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;

	SetBinTableJoystick(FALSE);

	bFull = BL_YES_NO(HMB_BL_UNLOAD_TO_FULL_MAGZ, IDS_BL_UNLOAD_FRAME);

	/*********/ StartLoadingAlert(); /*********/

	if (pGripperBuffer->IsBufferBlockExist())
	{
		lStatus = UDB_UnloadFromBufferToMgzWithLock(m_bBurnInTestRun, bFull, pGripperBuffer);
	}

	/*********/ CloseLoadingAlert(); /*********/

	SetBinTableJoystick(TRUE);

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Unload Buffer Frame");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::DB_ManualLoadFromBufferToTable(IPC_CServiceMessage& svMsg)
{
	BOOL bFull = FALSE;
	BOOL bReturn = TRUE;
	LONG lStatus = TRUE;

	BOOL bBufferLevel = BL_BUFFER_UPPER;
	if (m_lUseLBufferGripperForSetup)
	{
		bBufferLevel = BL_BUFFER_LOWER;
	}
	CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;

	if (!CheckBTBLModtorStatus())
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (m_fHardware == TRUE)
	{
		if (IsPressureSensorAlarmOn() == TRUE)
		{
			SetErrorMessage("Machine pressure is low");
			SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);

			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	//If frame already on BT
	if ( CheckFrameOnBinTable() == BL_FRAME_ON_CENTER )
	{
		if (m_bNoSensorCheck)
		{
		}
		else 
		{
			CString szErr;
			if (m_lBTCurrentBlock > 0)
				szErr.Format("Frame #%ld already existed on Table; please unload it first.", m_lBTCurrentBlock);
			else
				szErr = "Unknown Frame is detected on Table by sensor; please unload it first.";
			SetErrorMessage(szErr);
			HmiMessage_Red_Yellow(szErr, "Manual Load Frame");

			bReturn = TRUE;			//prevent HMI page lock-up
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}


	SetBinTableJoystick(FALSE);

	UDB_LoadFromBufferToTable(m_bBurnInTestRun, pGripperBuffer, TRUE);

	SetBinTableJoystick(TRUE);

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Load From Buffer to Table");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}

LONG CBinLoader::DB_ManualUnloadFromTableToBuffer(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lStatus = TRUE;

	BOOL bBufferLevel = BL_BUFFER_UPPER;
	if (m_lUseLBufferGripperForSetup)
	{
		bBufferLevel = BL_BUFFER_LOWER;
	}
	CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;

	if (!CheckBTBLModtorStatus())
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	if (m_fHardware == TRUE)
	{
		if (IsPressureSensorAlarmOn() == TRUE)
		{
			SetErrorMessage("Machine pressure is low");
			SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);

			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

	SetBinTableJoystick(FALSE);

	BOOL bFull = FALSE;
	if (m_bUseBarcode)
	{
		bFull = BL_YES_NO(HMB_BL_UNLOAD_TO_FULL_MAGZ, IDS_BL_UNLOAD_FRAME);
	}
	//Set Magzine&Slot Number
	m_lCurrMgzn = (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
	m_lCurrSlot = (*m_psmfSRam)["BinLoader"]["CurrSlot"];
	pGripperBuffer->SetMgznSlotNo(m_lCurrMgzn, m_lCurrSlot);
	pGripperBuffer->SetBinFull(bFull);
	UDB_UnloadFromTableToBuffer(m_bBurnInTestRun, bFull, pGripperBuffer, TRUE);

	SetBinTableJoystick(TRUE);
		
	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Unload from Table to Buffer");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

    return TRUE;
}

LONG CBinLoader::ResetDualBuffer(IPC_CServiceMessage& svMsg)
{
	CString szTitle, szContent;

	CString szMsg;

	szTitle.LoadString(HMB_BL_RESET_BUFFER);
	szContent.LoadString(HMB_GENERAL_AREUSURE);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
	{
		BOOL bReturn= FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CStringList szList;
	LONG lBuffer=0;

	szList.AddTail("All");
	szList.AddTail("Upper");
	szList.AddTail("Lower");

	szContent.LoadString(HMB_BL_SELECT_RESET_BUFFER);

	lBuffer = HmiSelection(szContent, szTitle, szList, 0);
	szContent.LoadString(HMB_GENERAL_AREUSURE);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szContent);		//v4.43T10

	if (lBuffer < 0)
	{
		BOOL bReturn= FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		if( lBuffer == 0 )
		{
			m_clUpperGripperBuffer.InitBufferInfo();
			m_clLowerGripperBuffer.InitBufferInfo();
		}
		else if ( lBuffer == 1 )
		{
			m_clUpperGripperBuffer.InitBufferInfo();
		}
		else if ( lBuffer == 2 )
		{
			m_clLowerGripperBuffer.InitBufferInfo();
		}	
	}

	TRY {
		SaveData();		//v3.45
	} CATCH (CFileException, e)
	{
	}
	END_CATCH

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

    return 1;
}

LONG CBinLoader::ClearAllBinAdvanced(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="OSRAM" )
	{
		bReturn = TRUE;
		for(int i=1; i<=200; i++)
		{
			//IPC_CServiceMessage stMsg;
			//stMsg.InitMessage(sizeof(LONG), &i);
			//// Get the reply for the encoder value
			//int nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "QueryBinStatus", stMsg);
			//while (1)
			//{
			//	if( m_comClient.ScanReplyForConvID(nConvID, 500) == TRUE )
			//	{
			//		m_comClient.ReadReplyForConvID(nConvID,stMsg);
			//		break;
			//	}
			//	else
			//	{
			//		Sleep(1);
			//	}
			//}
			//BOOL bSet = TRUE;
			//stMsg.GetMsg(sizeof(BOOL), &bSet);
			m_bClearBinHmi[i]	= FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBinLoader::ClearAllBinSelected(IPC_CServiceMessage& svMsg)
{
	CString szBarcodeName;
	CString szText;

	LONG lFrameOnBT = 0;
	LONG lUseReply = 0;
	LONG lCurrentBlock = 0;
	LONG lMagzNo = 0;
	LONG lSlotNo = 0;

	BOOL bClearAll = TRUE;


	//First, try to unload any frame on bintable
	if ( GetCurrBinOnBT()!=0 ) // logically
	{
		StartLoadingAlert();
		UnloadFilmFrame(svMsg);
		CloseLoadingAlert();
	}

	//Second, clear selected Bin(s) on menu
	for (lMagzNo=0; lMagzNo<MS_BL_MGZN_NUM; lMagzNo++)
	{
		for (lSlotNo=0; lSlotNo<MS_BL_MGZN_SLOT; lSlotNo++)
		{
			lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];
			if( m_bClearBin[lCurrentBlock] == FALSE ) 
			{
				bClearAll = FALSE;
				continue;
			}

			if ( (m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE) && 
					(m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0) )
			{
				szBarcodeName = m_stMgznRT[lMagzNo].m_SlotBCName[lSlotNo];

				if ( ClearBinFrameCounter(lCurrentBlock, szBarcodeName) == FALSE )
				{
					CString strTemp;
					strTemp.Format("Clear Bin failed in bin block %d.",lCurrentBlock);
					HmiMessage(strTemp,"Clear Bin Block");
					BOOL bReturn=TRUE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return FALSE;
				}
				// here to reset bcode
				CString szLog;
				szLog.Format("Bin #%d barcod resete by Adv-ClearBin: ", lCurrentBlock);
				BL_DEBUGBOX(szLog + m_stMgznRT[lMagzNo].m_SlotBCName[lSlotNo]);

				LONG lCurrMgzn=0, lCurrSlot=0;
				GetMgznSlot(lCurrentBlock, lCurrMgzn, lCurrSlot);
                m_stMgznRT[lMagzNo].m_SlotBCName[lSlotNo] = _T("");
				SaveBarcodeData(lCurrentBlock, _T(""), lCurrMgzn, lCurrSlot);
			}
		}
	}


	//v4.08		//Update m_bIsAllBinCntCleared in BinTableStn accordingly
	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("UpdateIfAllBinCountsAreCleared"), stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 6000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	BOOL bStatus = TRUE;
	stMsg.GetMsg(sizeof(BOOL), &bStatus);


	//Third, reset ClearAll flag if all bin counts cleared
	if (bClearAll)
	{
		ResetAllMagazine();
		HmiMessage("Clear-Bin finished; all magazine status is reset.","Clear Bin Block");
	}
	else
	{
		HmiMessage("Clear-Bin finished.","Clear Bin Block");
	}
	
	//Fourth, reset menu status
	for(int i=1; i<=200; i++)
	{
		m_bClearBin[i] = m_bClearBinHmi[i] = FALSE;
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::ClearAllBinUpdated(IPC_CServiceMessage& svMsg)
{
	for(int i=1; i<=200; i++)
	{
		m_bClearBin[i]		= m_bClearBinHmi[i];
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

//v4.50A12
LONG CBinLoader::ClearBinResetGradeSlot(IPC_CServiceMessage& svMsg)	
{
	BOOL bReturn = TRUE;
	CString szLog;

	ULONG ulSlotPhyBlock = 0;
    svMsg.GetMsg(sizeof(ULONG), &ulSlotPhyBlock);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bMagResetEmpty = pApp->GetFeatureStatus(MS896A_FUNC_BL_RESET_GRADEMAG_EMPTY);
	if (!bMagResetEmpty)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	LONG lMgzn=0, lSlot=0;
	LONG lStatus = GetMgznSlot(ulSlotPhyBlock, lMgzn, lSlot);

	szLog.Format("BL ClearBin Reset GradeSlot: Mgzn=%d, Slot=%d, MgznUsage=%d, Status=%d",
					lMgzn, lSlot, m_stMgznRT[lMgzn].m_lSlotUsage[lSlot], lStatus);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (lStatus != FALSE)
	{
		if (m_stMgznRT[lMgzn].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
		{
			m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinLoader::SelectClearBins(IPC_CServiceMessage& svMsg)
{
	LONG lMgznIndex = 0;

	svMsg.GetMsg(sizeof(LONG), &lMgznIndex);
	for(int i=1; i<=25; i++)
	{
		LONG lIndex = i + lMgznIndex*25;
		lIndex = min(200, lIndex);

		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(LONG), &lIndex);
		// Get the reply for the encoder value
		int nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "QueryBinStatus", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		BOOL bSet = TRUE;
		stMsg.GetMsg(sizeof(BOOL), &bSet);
		m_bClearBinHmi[lIndex]	= bSet;
	}

	return TRUE;
}

LONG CBinLoader::DeselectClearBins(IPC_CServiceMessage& svMsg)
{
	LONG lMgznIndex = 0;

	svMsg.GetMsg(sizeof(LONG), &lMgznIndex);
	for(int i=1; i<=25; i++)
	{
		LONG lIndex = i + lMgznIndex*25;
		lIndex = min(200, lIndex);
		m_bClearBinHmi[lIndex]	= FALSE;
	}

	return TRUE;
}

LONG CBinLoader::SetBarcodePrefix(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szBarcodePrefix;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szBarcodePrefix = &pBuffer[0];

	delete [] pBuffer;

	m_szBinBarcodePrefix = szBarcodePrefix;

	SaveData();
	
	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

LONG CBinLoader::GetCurrBlockID(IPC_CServiceMessage& svMsg)
{
	LONG lBlkID = 0;

	lBlkID = GetCurrBinOnBT();

	if ( (GetCurrBinOnBT() > 0) && (CheckFrameOnBinTable() != BL_FRAME_ON_CENTER) )
	{
		lBlkID = 0;
	}

	svMsg.InitMessage(sizeof(LONG), &lBlkID);
	return 1;
}


LONG CBinLoader::EnableCoverCheckPassword(IPC_CServiceMessage& svMsg)
{
	CString szPassword = "engineer";

	BOOL bEnable = FALSE;
	CHAR acPar[200], *pTemp;
	ULONG ulSize;
	IPC_CServiceMessage stMsg;

	strcpy_s(acPar, sizeof(acPar), "Please Enter YOUR Password");
	ulSize = (ULONG)strlen(acPar) + 1;
	pTemp = acPar + ulSize;
	strcpy_s(pTemp, sizeof(acPar) - ulSize, szPassword);
	ulSize += (ULONG)strlen(pTemp) + 1;

	stMsg.InitMessage(ulSize, acPar);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("BL: HmiUserService");		//anichia001
	INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiPassword", stMsg);

	// Get the reply
	while ( m_comClient.ScanReplyForConvID(nConvID, 500) == 0);

	m_comClient.ReadReplyForConvID(nConvID, stMsg);
	stMsg.GetMsg(sizeof(BOOL), &bEnable);

	BOOL	bReturn = TRUE;
	if( bEnable )
	{
		CString szMsg;
		szMsg.Format("BL Elevator cover state change %d", m_bCheckElevatorCover);
		SetErrorMessage(szMsg);
	}
	else
	{
		m_bCheckElevatorCover = !m_bCheckElevatorCover;
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::GetWLMagazine(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	LONG lMgznNo = 1;
	svMsg.GetMsg(sizeof(LONG), &lMgznNo);

	if ( (lMgznNo < 1) || (lMgznNo > MS_BL_WL_MGZN_NUM) )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//lMgzn = max(lMgzn, 1);
	//lMgzn = min(lMgzn, MS_BL_WL_MGZN_NUM);
	BL_WL_MAGAZINE stData;
	stData.m_lTopLevel_Z	= m_stWaferMgzn[lMgznNo-1].m_lTopLevel_Z;
	stData.m_lMidPosnY		= m_stWaferMgzn[lMgznNo-1].m_lMidPosnY;
	stData.m_lNoOfSlots		= m_stWaferMgzn[lMgznNo-1].m_lNoOfSlots;
	stData.m_dSlotPitch		= m_stWaferMgzn[lMgznNo-1].m_dSlotPitch;	

	svMsg.InitMessage(sizeof(BL_WL_MAGAZINE), &stData);
	return 1;
}

LONG CBinLoader::SetWLMagazine(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	BL_WL_MAGAZINE stData;
	svMsg.GetMsg(sizeof(BL_WL_MAGAZINE), &stData);
	LONG lMgzn = stData.m_lMgznNo;

CString szLog;
szLog.Format("BL: SetWLMagazine - %ld, %ld, %ld, %ld, %.2f", lMgzn, stData.m_lTopLevel_Z, stData.m_lMidPosnY, stData.m_lNoOfSlots, stData.m_dSlotPitch);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


	if ( (lMgzn < 1) || (lMgzn > MS_BL_WL_MGZN_NUM) )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//lMgzn = max(lMgzn, 1);
	//lMgzn = min(lMgzn, MS_BL_WL_MGZN_NUM);
	m_stWaferMgzn[lMgzn-1].m_lTopLevel_Z	= stData.m_lTopLevel_Z;
	m_stWaferMgzn[lMgzn-1].m_lMidPosnY		= stData.m_lMidPosnY;
	m_stWaferMgzn[lMgzn-1].m_lNoOfSlots		= stData.m_lNoOfSlots;
	m_stWaferMgzn[lMgzn-1].m_dSlotPitch		= stData.m_dSlotPitch;		//Slot pitch stored in mm

	for (INT i=0; i<MS_BL_WL_MGZN_NUM; i++)
	{
		m_stWaferMgzn[i].m_lMidPosnY		= stData.m_lMidPosnY;		//all mgzn Y uses same posn
	}

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::MoveWLoaderZ(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lZ = 0;
	svMsg.GetMsg(sizeof(LONG), &lZ);

	LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_MIN_DISTANCE);
	LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_MAX_DISTANCE);

CString szLog;
szLog.Format("BL: MoveWLoaderZ - %ld  (%ld, %ld)", lZ, lMinPos, lMaxPos);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if ( (lZ < lMinPos) || (lZ > lMaxPos) )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (Z_MoveTo(lZ) != gnOK)
	{
		bReturn = FALSE;
	}
	Sleep(100);

	if (!Z_IsPowerOn())
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::MoveWLoaderY(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lY = 0;
	svMsg.GetMsg(sizeof(LONG), &lY);
/*
	LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_MIN_DISTANCE);
	LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_MAX_DISTANCE);
	
CString szLog;
szLog.Format("BL: MoveWLoaderY - %ld  (%ld, %ld)", lY, lMinPos, lMaxPos);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if ( (lY < lMinPos) || (lY > lMaxPos) )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	Y_MoveTo(lY);
	Sleep(100);

	if (!Y_IsPowerOn())
		bReturn = FALSE;

*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

typedef struct {
	LONG lMgzn;
	LONG lSlot;
	LONG lUseUnloadOffset;
} BL_WL_MGZN_STRUCT;


LONG CBinLoader::MovetoWLSlot(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	BL_WL_MGZN_STRUCT stData;
	svMsg.GetMsg(sizeof(BL_WL_MGZN_STRUCT), &stData);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::MovetoWLSlot_Sync(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;	//MoveWElevator_Sync();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::AutoUnloadFilmFrame(IPC_CServiceMessage& svMsg)
{
	BOOL bFull = TRUE;
	BOOL bReturn = FALSE;

	CString szMsg;
	szMsg.Format("BL Auto Unload Film Frame at WAFEREND %d %d", m_bWaferToBinResort, m_bFrameToBeUnloadAtIdleState);
	LogCycleStopState(szMsg);
	if ( !m_bWaferToBinResort && !m_bFrameToBeUnloadAtIdleState )	//v4.42T18
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	//v4.47T1
	if (Is180Arm6InchWT())
	{
		m_bFrameToBeUnloadAtIdleState = FALSE;
		BL_DEBUGBOX("Auto unload bin frame disabled due to WT 180 arm config\n");

		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	//v4.42T18
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (!m_bWaferToBinResort && pApp->GetCustomerName()!="NanoJoin" )
	{
		BL_DEBUGBOX("Start auto unload bin frame after wafer end");
		OpUnloadFrameOnTable();
		m_bFrameToBeUnloadAtIdleState = FALSE;
		
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}
	else
	{
		BL_DEBUGBOX("Start auto unload bin frame to full after wafer end");
	}

	if (!CheckLoadUnloadSafety())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	SetBinTableJoystick(FALSE);


	/*********/ StartLoadingAlert(); /*********/
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{

	}
/*
	else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//DL with Buffer Table
	{
		if (!UnloadFrameOnUpperGripper(bFull, TRUE, TRUE))
		{
			CloseLoadingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (!UnloadFrameOnLowerGripper(FALSE, FASLE, TRUE))
		{
			CloseLoadingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);
	}
*/
	else
	{
		SetBLStartTime();
		SetBLCurTime(0,0);
		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);
		SetBLCurTime(13,0);
		SaveBLTime();
	}
	/*********/ CloseLoadingAlert(); /*********/

	SetBinTableJoystick(TRUE);

	m_bBinFrameCheckBCFail	= FALSE;

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Unload Film Frame");
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

    return TRUE;
}


LONG CBinLoader::UnloadTableFrameAtWaferEnd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bFrameToBeUnloadAtIdleState == TRUE)
	{
		//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		//if (pApp->State() == IDLE_Q)
		//{
		OpUnloadFrameOnTable();
		m_bFrameToBeUnloadAtIdleState = FALSE;
		//}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}

LONG CBinLoader::AutoUnloadFilmFrameToFull(IPC_CServiceMessage& svMsg)
{
	BOOL bFull;
	BOOL bReturn = TRUE;
	LONG lStatus = TRUE;

	if (!CheckLoadUnloadSafety())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}
	CMSLogFileUtility::Instance()->MS_LogOperation("AutoUnloadFilmFrameToFull:1");

	//v4.38T5
	LONG lFrameOnBT = CheckFrameOnBinTable();
	if ( !m_bNoSensorCheck && (lFrameOnBT != BL_FRAME_ON_CENTER) )
    {
		SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
		SetErrorMessage("BL: No frame existed on bintable 1 in ManualUnloadFilmFrame");
		bReturn = TRUE;			//v2.56 prevent HMI page lock-up
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
    }

	SetBinTableJoystick(FALSE);

	CMSLogFileUtility::Instance()->MS_LogOperation("AutoUnloadFilmFrameToFull:4");
	//bFull = BL_YES_NO(HMB_BL_UNLOAD_TO_FULL_MAGZ, IDS_BL_UNLOAD_FRAME);
	bFull = TRUE;


	/*********/ StartLoadingAlert(); /*********/
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
	}
/*
	else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//DL with Buffer Table
	{
	CMSLogFileUtility::Instance()->MS_LogOperation("AutoUnloadFilmFrameToFull:5");
		if (!UnloadFrameOnUpperGripper(bFull, TRUE, TRUE))
		{
			CloseLoadingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (!UnloadFrameOnLowerGripper(FALSE, FALSE, TRUE))
		{
			CloseLoadingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);

	}
*/
	else
	{
		SetBLStartTime();
		SetBLCurTime(0,0);
		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);
		SetBLCurTime(13,0);
		SaveBLTime();
	}
	/*********/ CloseLoadingAlert(); /*********/

	SetBinTableJoystick(TRUE);

	m_bBinFrameCheckBCFail	= FALSE;
	CMSLogFileUtility::Instance()->MS_LogOperation("AutoUnloadFilmFrameToFull6");
	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Unload Film Frame");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CBinLoader::AutoUnloadFilmFrameNotToFull(IPC_CServiceMessage& svMsg)
{
	BOOL bFull;
	BOOL bReturn = TRUE;
	LONG lStatus = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (!CheckLoadUnloadSafety())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	//v4.38T5
	LONG lFrameOnBT = CheckFrameOnBinTable();
	if ( !m_bNoSensorCheck && (lFrameOnBT != BL_FRAME_ON_CENTER) )
    {
		SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
		SetErrorMessage("BL: No frame existed on bintable 1 in ManualUnloadFilmFrame");

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
    }

	SetBinTableJoystick(FALSE);

	//bFull = BL_YES_NO(HMB_BL_UNLOAD_TO_FULL_MAGZ, IDS_BL_UNLOAD_FRAME);
	bFull = FALSE;


	/*********/ StartLoadingAlert(); /*********/
/*
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//DL with Buffer Table
	{
		if (!UnloadFrameOnUpperGripper(bFull, TRUE, TRUE))
		{
			CloseLoadingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (!UnloadFrameOnLowerGripper(FALSE, FALSE, TRUE))
		{
			CloseLoadingAlert(); 
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);

	}
	else
*/
	{
		SetBLStartTime();
		SetBLCurTime(0,0);
		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);
		SetBLCurTime(13,0);
		SaveBLTime();
	}
	/*********/ CloseLoadingAlert(); /*********/

	SetBinTableJoystick(TRUE);

	m_bBinFrameCheckBCFail	= FALSE;

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Unload Film Frame");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}

LONG CBinLoader::AutoUnloadFilmFrameOsram(IPC_CServiceMessage& svMsg)
{
	BOOL bFull;
	BOOL bReturn = TRUE;
	LONG lStatus = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp->GetCustomerName() != "OSRAM")
	{
		return TRUE;
	}

	if (!CheckLoadUnloadSafety())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	LONG lFrameOnBT = CheckFrameOnBinTable();
	if ( !m_bNoSensorCheck && (lFrameOnBT != BL_FRAME_ON_CENTER) )
    {
		SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
		SetErrorMessage("BL: No frame existed on bintable 1 in ManualUnloadFilmFrame");
		bReturn = TRUE;			//v2.56 prevent HMI page lock-up
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
    }

	SetBinTableJoystick(FALSE);

	//bFull = BL_YES_NO(HMB_BL_UNLOAD_TO_FULL_MAGZ, IDS_BL_UNLOAD_FRAME);
	bFull = FALSE;


	/*********/ StartLoadingAlert(); /*********/
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
	}
/*
	else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//DL with Buffer Table
	{
		if (!UnloadFrameOnUpperGripper(bFull, TRUE, TRUE))
		{
			CloseLoadingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		if (!UnloadFrameOnLowerGripper(FALSE, FALSE, TRUE)
		{
			CloseLoadingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);
	}
*/
	else
	{
		SetBLStartTime();
		SetBLCurTime(0,0);
		UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, bFull, TRUE);
		SetBLCurTime(13,0);
		SaveBLTime();
	}
	/*********/ CloseLoadingAlert(); /*********/

	SetBinTableJoystick(TRUE);

	m_bBinFrameCheckBCFail	= FALSE;

	try
	{
		SaveData();
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Unload Film Frame");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}

//=====================================================================================================================
//			Add the Sanan auto-unload the expired frame
//=====================================================================================================================
//ChangeLight request 1:
LONG CBinLoader::UnLoadBinFrameToFullMagazine(IPC_CServiceMessage& svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bReturn = TRUE;
	
	if ((pApp->GetCustomerName() != CTM_SANAN) || (m_lUnloadToFullTimeSpan == 0))
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
	    return TRUE;
	}

	CString szLog;
	szLog.Format("BL: UnLoadBinFrameToFullMagazine START: TimeSpan = %ld days", m_lUnloadToFullTimeSpan);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if ((m_lUnloadToFullTimeSpan <= 0) || (m_lUnloadToFullTimeSpan > 90))
	{
		szLog.Format("BL: UnLoadBinFrameToFullMagazine STOP: TimeSpan (%ld) must between 0 to 90", m_lUnloadToFullTimeSpan);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
	    return TRUE;
	}

	if (!CheckLoadUnloadSafety())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
	    return TRUE;
	}

	while (TRUE)
	{
		CString szFullFileName = "";
		LONG lBlockID = CheckFileTimeSpan(szFullFileName);

		//bAvaliableSlot = GetFullMgznSlot(lMgz,lSlot);
		// if lResult value is 0: no need to unload bin frame to Full 
		// if lResult value is not 0: need to unload bin frame to Full Magazine
		szLog.Format("Song Test BLKID: %ld (%s)", lBlockID, szFullFileName);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		if (lBlockID <= 0)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BL: UnLoadBinFrameToFullMagazine: No Action");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
	        return TRUE;
		}
		else
		{
			szLog.Format("Check whether exist frame on bin table or buffer.");
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			if (m_clUpperGripperBuffer.IsBufferBlockExist())
			{
				szLog.Format("Unload from upper buffer to mgz.");
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				LONG lStatus = UDB_UnloadFromBufferToMgzWithLock(FALSE, FALSE, &m_clUpperGripperBuffer);
				if (lStatus != TRUE)
				{	
					SetAlert_Red_Yellow(IDS_BL_EX_UNLOAD_BUFFER_FRAME_FAIL);
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return TRUE;
				}
			}

			if (m_clLowerGripperBuffer.IsBufferBlockExist())
			{
				szLog.Format("Unload from lower buffer to mgz.");
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				LONG lStatus = UDB_UnloadFromBufferToMgzWithLock(FALSE, FALSE, &m_clLowerGripperBuffer);
				if (lStatus != TRUE)
				{	
					SetAlert_Red_Yellow(IDS_BL_EX_UNLOAD_BUFFER_FRAME_FAIL);
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return TRUE;
				}
			}

			if (!m_szBinFrameBarcode.IsEmpty())
			{
				szLog.Format("Frame on table, manual unload film frame.");
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

				LONG lStatus = UnloadBinFrame(FALSE, &m_clLowerGripperBuffer, FALSE, TRUE);
				if (lStatus != TRUE)
				{
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return TRUE;
				}
			}
			
			szLog.Format("Song Test Unload grade: %ld", lBlockID);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			bReturn = AutoLoadUnloadFilmFrameToFull(lBlockID);

			if (bReturn != TRUE)
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("UnLoadBinFrameToFullMagazine: ABORT 1");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			if (szFullFileName.GetLength() > 0)
			{
				DeleteFile(szFullFileName);
			}

			//CopyCleanBinFile();
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinLoader::CheckFileTimeSpan(CString& szFullFileName)
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString szFilePath = gszROOT_DIRECTORY + "\\Exe\\NewBinTime";
	CString szFileFullName = "";
	CString szLogFileName = "";
	CString szLog;
	
	CreateDirectory(szFilePath, NULL);

	hSearch = FindFirstFile(szFilePath + "\\" + "*", &FileData);

	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		CMSLogFileUtility::Instance()->MS_LogOperation("CheckFileTimeSpan: INVALID_HANDLE_VALUE");
		return 0;
	}

	do 
	{
		// if it is a folder recurive call to remove file
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;
        if ( (strcmp(FileData.cFileName, ".")==0) || (strcmp(FileData.cFileName, "..")==0) )
            continue;
		
		CTime cfileLastWriteTime;
		//cfileLastWriteTime = FileData.ftLastWriteTime;
		cfileLastWriteTime = CTime(FileData.ftLastWriteTime);
		
		CTime cCurrentTime = CTime::GetCurrentTime();	
		CTimeSpan cTimeSpan;
		cTimeSpan = cCurrentTime - cfileLastWriteTime;

		CString szLastWriteTime = cfileLastWriteTime.Format("%Y%m%d-%H:%M:%S");
		CString szCurrTime		= cCurrentTime.Format("%Y%m%d-%H:%M:%S");

		DOUBLE dDays = (DOUBLE)cTimeSpan.GetDays();
		szLogFileName = FileData.cFileName;

		szLog.Format("CheckFileTimeSpan: file found - %s (DaySpan=%.1f, limit=%ld); LastWriteTime = %s; CurrTime = %s", 
						(LPCTSTR) szLogFileName, dDays, m_lUnloadToFullTimeSpan, (LPCTSTR) szLastWriteTime, (LPCTSTR) szCurrTime);
		//CMSLogFileUtility::Instance()->MS_LogOperation("CheckFileTimeSpan: file found - " + szLogFileName);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


		if (cTimeSpan.GetDays() >= m_lUnloadToFullTimeSpan )
		{
			szFileFullName = szFilePath + "\\" + FileData.cFileName;
			CString szFileName = FileData.cFileName;
			szFullFileName = szFileFullName;
	
			szLog.Format("CheckFileTimeSpan: File (%s) timestamp is longer than setting !", szFileName);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			INT nIndex;
			nIndex = szFileName.Find(".");
			CString szBlkId = szFileName.Left(nIndex);
			//DeleteFile(szFileFullName);
			return atol((LPCTSTR) szBlkId);
		}
				
	} while (FindNextFile(hSearch, &FileData) == TRUE);
 
	// Close the search handle. 
	if ( FindClose(hSearch) == FALSE) 
	{ 
		CMSLogFileUtility::Instance()->MS_LogOperation("CheckFileTimeSpan: FindClose");
		return 0;
	} 
	
	return 0;
}

LONG CBinLoader::CopyCleanBinFile()
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString szFilePath = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin";
	CString szExceedPeriodPath = "c:\\MapSorter\\UserData\\OutputFile\\ExceedPeriod";
	CString szFileFullName = "";
	
	hSearch = FindFirstFile(szFilePath + "\\" + "*", &FileData);

	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return 0;
	}

	do 
	{
		// if it is a folder recurive call to remove file
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;
        if ( (strcmp(FileData.cFileName, ".")==0) || (strcmp(FileData.cFileName, "..")==0) )
            continue;

		CTime cfileLastWriteTime;
		cfileLastWriteTime = FileData.ftCreationTime;
		CTime cCurrentTime = CTime::GetCurrentTime();
		CTimeSpan cTimeSpan;
		cTimeSpan = cCurrentTime - cfileLastWriteTime;
		if (cTimeSpan.GetMinutes() < 1)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Song Test Rename Back up clean bin file.");
			szFileFullName = szFilePath + "\\" + FileData.cFileName;
			CString szExceedPeriodFullName;
			szExceedPeriodFullName = szExceedPeriodPath + "\\" + FileData.cFileName;


			CopyFile(szFileFullName, szExceedPeriodFullName, FALSE);
			//CString szFileName = FileData.cFileName;
			//INT nIndex;
			//nIndex = szFileName.Find(".");
			//CString szNewName = szFileName.Left(nIndex);
			//CString szNewFileFullName =  szFilePath + "\\" + szNewName + "_TimeOut.csv";

			//RenameFile(szFileFullName,szNewFileFullName);
			//return 1;
		}
				
	} while (FindNextFile(hSearch, &FileData) == TRUE);
 
	// Close the search handle. 
	if ( FindClose(hSearch) == FALSE) 
	{ 
		return 0;
	} 
	
	return 0;
}



LONG CBinLoader::AutoLoadUnloadFilmFrameToFull(const LONG lBlockID)
{
	LONG lRet = FALSE;

	if (!CheckLoadUnloadSafety())
	{
//		SetErrorMessage("Motor is OFF");
//		SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);

        return FALSE;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("AutoLoadUnloadFilmFrameToFull : 1");
	SetBinTableJoystick(FALSE);

	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		SetErrorMessage("ERROR: AutoLoadUnloadFilmFrameToFull disabled for non-DBuffer model");
        return FALSE;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("AutoLoadUnloadFilmFrameToFull : 4");
	if (m_clUpperGripperBuffer.IsBufferBlockExist())
	{
		CString szErr;
		szErr.Format("AutoLoadUnloadFilmFrameToFull: upper buffer slot has frame #%d", m_clUpperGripperBuffer.GetBufferBlock());
		SetErrorMessage(szErr);
		return FALSE;
	}

	if (m_clLowerGripperBuffer.IsBufferBlockExist())
	{
		CString szErr;
		szErr.Format("AutoLoadUnloadFilmFrameToFull: lower buffer slot has frame #%d", m_clLowerGripperBuffer.GetBufferBlock());
		SetErrorMessage(szErr);
		return FALSE;
	}

	CTmpChange<BOOL> bDisableBinFrameStatusSummaryFile(&m_bDisableBinFrameStatusSummaryFile, TRUE);

	BOOL bFull = TRUE;
	if ((GetCurrBinOnBT() != 0) && (lBlockID == GetCurrBinOnBT()) )
	{
		//only unload a frame to the magazine from bin table
		CMSLogFileUtility::Instance()->MS_LogOperation("UnloadBinFrame : Direct UNLOAD to FULL ...");
		
		lRet = UnloadBinFrame(FALSE, &m_clLowerGripperBuffer, bFull, TRUE);
	}
	else
	{
		if (!IsNeedResetFullMgzn())
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("AutoLoadUnloadFilmFrameToFull : Check FULL mgzn FAIL !");
			return FALSE;
		}

		CMSLogFileUtility::Instance()->MS_LogOperation("AutoLoadUnloadFilmFrameToFull : UDB_TransferFrame ...");
		lRet = UDB_TransferFrame(FALSE, lBlockID, &m_clUpperGripperBuffer, bFull);
	}
	
	try
	{
		//SaveData();
		//SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Manual Unload Film Frame");
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("AutoLoadUnloadFilmFrameToFull : DONE");
    return lRet;
}


BOOL CBinLoader::IsNeedResetFullMgzn()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	for (INT i = 0; i < MS_BL_MGZN_NUM; i++)
	{
		if (m_stMgznRT[i].m_lMgznState == BL_MGZN_STATE_FULL)
		{
			if (m_bBurnInEnable)		//v4.31T1	//Auto-reset mgzn for BURNIN mode only
			{
				OpOutoutMgznSummary(IsMSAutoLineMode());
				ResetMagazine(i);
				m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
			}
			else
			{
				if (IsElevatorCoverOpen() == TRUE)
				{
					HmiMessage_Red_Back("Please close elevator cover to proceed!");
				}

				//Move elevator to ready position for replace magazine
				if (DownElevatorThetaZToPosn(0) == FALSE)
				{
					SetErrorMessage("BL: IsNeedResetFullMgzn DownElevatorThetaZToPosn to ABORT");
					return FALSE;
				}
					
				LONG lStatus = 0;

				
				if (pApp->GetCustomerName() == "ChangeLight(XM)")
				{
					lStatus = BL_YES_NO(IsMSAutoLineStandloneMode() ? HMB_BL_AUTO_LINE_CLEAR_FRAME_MAGZ_IS_FULL_RESET_CHANGELIGHTXM : HMB_BL_MAGZ_IS_FULL_RESET_CHANGELIGHTXM, IDS_BL_OPERATION);
				}
				else
				{
					lStatus = BL_YES_NO(IsMSAutoLineStandloneMode() ? HMB_BL_AUTO_LINE_CLEAR_FRAME_MAGZ_IS_FULL_RESET : HMB_BL_MAGZ_IS_FULL_RESET, IDS_BL_OPERATION);
				}

				if (lStatus == TRUE)	//Change EMPTY to FULL msg
				{
					OpOutoutMgznSummary(IsMSAutoLineMode());	
					ResetMagazine(i);
					m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;

					//Also check other FULL mgzns
					for (INT k = 0; k < MS_BL_MGZN_NUM; k++)
					{
						if (k == i)
						{
							continue;
						}
						if ((m_stMgznRT[k].m_lMgznUsage == BL_MGZN_USAGE_FULL) &&
							(m_stMgznRT[k].m_lMgznState == BL_MGZN_STATE_FULL))
						{
							ResetMagazine(k);
							m_stMgznRT[k].m_lMgznState = BL_MGZN_STATE_OK;
						}
					}
				}
				else
				{
					SetErrorMessage("BL: IsNeedResetFullMgzn selects NO to ABORT");
					return FALSE;
				}
			}
		}
	}

	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData in CycleState !!!");
	}
	END_CATCH

	TRY {
		SaveMgznRTData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH

	return TRUE;
}

LONG CBinLoader::SetEmptyFramePath(IPC_CServiceMessage &svMsg)	//Matthew 20181107
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szEmptyFramePath);
		bReturn = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::BurnInBLZUpperLimit(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	BOOL bTakeLog = FALSE;
	LONG lCycleCounter = 0, lStandardEnc_Z = 0, lResult = 0;
	CString szResult, szTemp, szText, szTime, szStatus, szStantardEncZ;
	CStdioFile oLogFile;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CTime theTime;

	CMS896AApp::m_bStopAlign = FALSE;

	for(int i = 0; i < 5; i++)
	{
		Z_MoveTo(50000);
		Z_Home();
		Z_MoveTo(300000);
		lResult = Z_SerachUpperLimitSensor();
		if (lResult != 0)
		{
			HmiMessage("Search Upper Sensor Error!", "Search Error");
			CMS896AApp::m_bStopAlign = TRUE;
		}
		GetEncoderValue();
		lStandardEnc_Z = lStandardEnc_Z + m_lEnc_Z;
		if (pApp->IsStopAlign())
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	lStandardEnc_Z = lStandardEnc_Z / 5;
	szStantardEncZ.Format("(%d)", lStandardEnc_Z);

	while(1)
	{
		//Z_MoveTo(0);
		//Z_MoveTo(300000);
		//Z_SerachUpperLimitSensor();

		for(int i = 0; i < 10; i++)
		{
			Z_MoveTo(0);
			Z_MoveTo(370000);
			if (pApp->IsStopAlign())
			{
				Z_Home();
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}

		Z_MoveTo(0);
		Z_MoveTo(300000);
		
		lResult = Z_SerachUpperLimitSensor();

		if (lResult != 0)
		{
			HmiMessage("Search Upper Sensor Error!", "Search Error");
			CMS896AApp::m_bStopAlign = TRUE;
		}

		theTime = CTime::GetCurrentTime();
		szTime.Format("%d-%02d-%02d %02d:%02d:%02d,", theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		GetEncoderValue();
		szTemp.Format("%d,", m_lEnc_Z);
		if (m_lEnc_Z - lStandardEnc_Z > 100 || m_lEnc_Z - lStandardEnc_Z < -100)
		{
			szStatus = "FAIL";
		}
		else
		{
			szStatus = "PASS";
		}
		szStatus = szStatus + szStantardEncZ + "\n";

		szResult = szResult + szTime + szTemp + szStatus;

		lCycleCounter++;
		if (lCycleCounter % 1 == 0 || pApp->IsStopAlign())
		{
			bTakeLog = TRUE;
		}

		if (bTakeLog)
		{
			CString szPath = "c:\\Mapsorter\\UserData\\BurnInBLZUpperLimitResult.txt";
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
					szLine = "Time,Encoder,Status\n";
					oLogFile.WriteString(szLine);						
				}
				oLogFile.SeekToEnd();
				oLogFile.WriteString(szResult);
				oLogFile.Close();
			}
			bTakeLog = FALSE;
			szResult.Empty();

			if (pApp->IsStopAlign())
			{
				break;
			}
		}

	}

	Z_Home();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinLoader::FindBLZEncoderFromZeroToHomeSensor(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	Z_MoveTo(0);
	Sleep(30);

	LONG lResult = Z_SerachHomeLimitSensor();
	Sleep(30);

	if (lResult != 0)
	{
		HmiMessage("Search Home Sensor Error!", "Search Error");
	}
	else
	{
		GetEncoderValue();
		LONG lEncBLZHomeSensor = m_lEnc_Z;

		CString szEncBLZHomeSensor;
		szEncBLZHomeSensor.Format("%d", lEncBLZHomeSensor);
		HmiMessage(szEncBLZHomeSensor, "BLZ Home Sensor Encoder");

		Z_Home();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}