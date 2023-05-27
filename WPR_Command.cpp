/////////////////////////////////////////////////////////////////
// WPR_Command.cpp : HMI Registered Command of the CWaferPr class
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
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "HmiDataManager.h"
#include "MS_SecCommConstant.h"
#include "GallerySearchController.h"
#include "WaferTable.h"
#include "WL_Constant.h"
#include "BondPr.h"
#include "BondHead.h"
#include "BinTable.h"
#include "PRFailureCaseLog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Registered HMI Command 
VOID CWaferPr::RegisterVariables()
{
	int i;
	CString szText;

	try
	{
		// Register Service Commands
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MeasureObject"),		&CWaferPr::MeasureObject);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectCamera"),			&CWaferPr::SelectCamera);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectWaferCamera"),	&CWaferPr::SelectWaferCamera);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectBondCamera"),		&CWaferPr::SelectBondCamera);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectPostSealCamera"), &CWaferPr::SelectPostSealCamera);		//v3.70T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SwitchToTarget"),		&CWaferPr::SwitchToTarget);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SwitchToTarget_dbh"),	&CWaferPr::SwitchToTarget_dbh);			//v3.93
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TurnOffPSLighting"),	&CWaferPr::TurnOffPSLighting);	

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetGeneralLighting"),		&CWaferPr::SetGeneralLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetGeneralLighting2"),		&CWaferPr::SetGeneralLighting2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetGeneralLighting"),		&CWaferPr::GetGeneralLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RestoreGeneralLighting"),	&CWaferPr::RestoreGeneralLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RecoverBackupLighting"),	&CWaferPr::RecoverBackupLighting);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetExposureTime"),			&CWaferPr::SetExposureTime);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetGeneralExposureTime"),	&CWaferPr::SetGeneralExposureTime);	//v3.30T1

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),			&CWaferPr::UpdateAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogItems"),				&CWaferPr::LogItems);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateHMIData"),		&CWaferPr::UpdateHMIData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckLearnAlignAlgorithm"),		&CWaferPr::CheckLearnAlignAlgorithm);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelLearnDie"),		&CWaferPr::CancelLearnDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmLearnDie"),		&CWaferPr::ConfirmLearnDie);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableLookForwardCmd"), &CWaferPr::EnableLookForwardCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSearchDiePara"),		&CWaferPr::ConfirmSearchDiePara);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSearchDieArea"),		&CWaferPr::ConfirmSearchDieArea);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSearchDieAreaNew"),	&CWaferPr::ConfirmSearchDieAreaNew);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchLookForwardDie"),	&CWaferPr::UserSearchLookForwardDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualFailureCaseLog"),		&CWaferPr::ManualFailureCaseLog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchDie"),			&CWaferPr::UserSearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchDie_CheckNoDie"),	&CWaferPr::UserSearchDie_CheckNoDie);	//v3.98T3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchDie_Rotate"),		&CWaferPr::UserSearchDie_Rotate);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchDie_NoMove"),		&CWaferPr::UserSearchDie_NoMove);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchNormalDie_NoMove"),&CWaferPr::UserSearchNormalDie_NoMove);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchMark1"),			 &CWaferPr::UserSearchMark1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchMark2"),			 &CWaferPr::UserSearchMark2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchColletHole1"),	 &CWaferPr::UserSearchColletHole1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchColletHole2"),	 &CWaferPr::UserSearchColletHole2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchRefDie_NoMove"),	&CWaferPr::UserSearchRefDie_NoMove);		//v3.70T3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchRefDie"),			&CWaferPr::UserSearchRefDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchAllRefDie"),		&CWaferPr::UserSearchAllRefDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HotKeySearchRefDie"),		&CWaferPr::HotKeySearchRefDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchAllDie"),			&CWaferPr::UserSearchAllDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchUpLookDieCmd"),	&CWaferPr::UserSearchUpLookDieCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PRIDDetailInfo"),			&CWaferPr::PRIDDetailInfo);

		//v4.42T3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateDefectFromHmi"),				&CWaferPr::UpdateDefectFromHmi);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchRefDie_NoMove_GivenID"),	&CWaferPr::UserSearchRefDie_NoMove_GivenID);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchCurrentDieInFOV_Spiral"),		&CWaferPr::SearchCurrentDieInFOV_Spiral);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WprAutoColorAdjust"),		&CWaferPr::WprAutoColorAdjust);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetLearnDieLighting"),		&CWaferPr::SetLearnDieLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetLearnDieLighting2"),		&CWaferPr::SetLearnDieLighting2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovePRCursor"),				&CWaferPr::MovePRCursor);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLearnDie"),				&CWaferPr::AutoLearnDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoDetectDie"),			&CWaferPr::AutoDetectDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLearnRefDie"),			&CWaferPr::AutoLearnRefDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckRebel_R2N_Cmd"),		&CWaferPr::CheckRebel_R2N_Cmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckRebel_N2R_Cmd"),		&CWaferPr::CheckRebel_N2R_Cmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetReferDieSize"),			&CWaferPr::GetReferDieSize);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoUpLookSearchDie_Rpy1"), &CWaferPr::AutoUpLookSearchDie_Rpy1);//4.52D17

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LearnDiePitch"),			&CWaferPr::LearnDiePitch);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LearnDieCalibration"),		&CWaferPr::LearnDieCalibration);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeletePRRecord"),			&CWaferPr::DeletePRRecord);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayPRRecord"),			&CWaferPr::DisplayPRRecord);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableDebugFlag"),			&CWaferPr::EnableDebugFlag);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableDebugLog"),			&CWaferPr::EnableDebugLog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayLFSearchRegion"),	&CWaferPr::DisplayLFSearchRegion);	//v3.13T3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateSoraaDiePitch"),		&CWaferPr::UpdateSoraaDiePitch);	//v4.28T5
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetPrCenterXY"),			&CWaferPr::ResetPrCenterXY);		//v3.80
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdatePrCenterXY"),			&CWaferPr::UpdatePrCenterXY);		//v4.08
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayPRIdListRecord"),	&CWaferPr::DisplayPRIdListRecord);

		//Debug only
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OfflineSearchDie"),			&CWaferPr::OfflineSearchDie);

		//Command for others Station; search GOOD/REF die without any table motion cmd
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchCurrentDie"),			&CWaferPr::SearchCurrentDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchCurrentDie_PLLM_REBEL"),	&CWaferPr::SearchCurrentDie_PLLM_REBEL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchDieInSearchWindow"),	&CWaferPr::SearchDieInSearchWindow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchCurrentDieInFOV"),	&CWaferPr::SearchCurrentDieInFOV);

		// Toggle the joystick speed
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleJoystickSpeed"),		&CWaferPr::ToggleJoystickSpeed);

		// Mouse
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UsePRMouse"),				&CWaferPr::UsePRMouse);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DrawComplete"),				&CWaferPr::DrawComplete);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelMouseDraw"),			&CWaferPr::CancelMouseDraw);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetInspDetectRegion"),		&CWaferPr::SetInspDetectRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EndInspDetectRegion"),		&CWaferPr::EndInspDetectRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetInspIgnoreRegion"),		&CWaferPr::SetInspIgnoreRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EndInspIgnoreRegion"),		&CWaferPr::EndInspIgnoreRegion);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetAlnDetectRegion"),		&CWaferPr::SetAlnDetectRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EndAlnDetectRegion"),		&CWaferPr::EndAlnDetectRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetAlnIgnoreRegion"),		&CWaferPr::SetAlnIgnoreRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EndAlnIgnoreRegion"),		&CWaferPr::EndAlnIgnoreRegion);


		// Upload and download PR Record
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UploadPrRecords"),			&CWaferPr::UploadPrRecords);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DownloadPrRecords"),		&CWaferPr::DownloadPrRecords);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeletePrRecords"),			&CWaferPr::DeletePrRecords);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RunTimeDownloadPrRecords"), &CWaferPr::RunTimeDownloadPrRecords);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PkgKeyParametersTask"),		&CWaferPr::PkgKeyParametersTask);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UploadPrPkgPreviewImage"),	&CWaferPr::UploadPrPkgPreviewImage);
		
		// Check PR ID
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckRecordID"),			&CWaferPr::CheckRecordID);

		// Check Die is learnt
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsDieLearnt"),				&CWaferPr::IsDieLearnt);
		// down load multi search command
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DownloadMultiSearchRecord"), &CWaferPr::DownloadMultiSearchRecord);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartBackLightElevatorMotionTest"), &CWaferPr::StartBackLightElevatorMotionTest);

		// Zoom Sensor Command
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LiveViewRoiZoomCmd"),	&CWaferPr::LiveViewRoiZoomCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LiveViewConZoomCmd"),	&CWaferPr::LiveViewConZoomCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LiveViewZoomCmd"),		&CWaferPr::LiveViewZoomCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ZoomScreenCmd"),		&CWaferPr::ZoomScreenCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LockLiveViewZoom"),		&CWaferPr::LockLiveViewZoom);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleZoomViewCmd"),	&CWaferPr::ToggleZoomViewCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleLearnColletZoomScreenCmd"),	&CWaferPr::ToggleLearnColletZoomScreenCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleZoomScreenCmd"),	&CWaferPr::ToggleZoomScreenCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleAutoZoomViewCmd"),	&CWaferPr::ToggleAutoZoomViewCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleNml4XCmd"),		&CWaferPr::ToggleNml4XCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SwitchWaferDualPath"),	&CWaferPr::SwitchWaferDualPath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmDigitalZoom"),	&CWaferPr::ConfirmDigitalZoom);

		// Check Die inspection set
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsInspectionSet"),		&CWaferPr::IsInspectionSet);
		// Setup badcut region
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BadCutSetupStart"),		&CWaferPr::BadCutSetupStart);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BadCutSetupStop"),		&CWaferPr::BadCutSetupStop);

		// Learn more die
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AdaptiveLearnDie"),		&CWaferPr::AdaptiveLearnDie);

		// For wafer loader supporting
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FindFirstDieInWafer"),		&CWaferPr::FindFirstDieInWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FindFirstDieAngleInWafer"), &CWaferPr::FindFirstDieAngleInWafer);	//xyz
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AlignWaferAngle"),			&CWaferPr::AlignWaferAngle);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SpiralSearchHomeDie"),		&CWaferPr::SpiralSearchHomeDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SpiralSearchRefDie"),		&CWaferPr::SpiralSearchRefDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CornerSearchHomeDie"),		&CWaferPr::CornerSearchHomeDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestHomeDiePattern"),		&CWaferPr::TestHomeDiePattern);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LookAheadURightRectWaferCorner"),	&CWaferPr::LookAheadURightRectWaferCorner);	//v4.21T3	//Osram RESORT rect wafer
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LookAheadLRightRectWaferCorner"),	&CWaferPr::LookAheadLRightRectWaferCorner);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LookAheadLRightRectWaferCornerWithIncompleteLastRow"), &CWaferPr::LookAheadLRightRectWaferCornerWithIncompleteLastRow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LookAheadLLeftRectWaferCorner"),	&CWaferPr::LookAheadLLeftRectWaferCorner);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LookAheadLLeftRectWaferWithInCompleteCorner"), &CWaferPr::LookAheadLLeftRectWaferWithInCompleteCorner);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LookAroundBaseOnMap"),		&CWaferPr::LookAroundBaseOnMap);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LookAroundNormalDie"),		&CWaferPr::LookAroundNormalDie);	//v3.13T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetFOVSize"),				&CWaferPr::GetFOVSize);				//v3.13T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchAlignPattern"),		&CWaferPr::SearchAlignPattern);		//v3.42
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TurnOnPostSealLighting"),	&CWaferPr::TurnOnPostSealLighting);	//v3.71T5

		// For Block Function
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchReferDieInWafer"),	&CWaferPr::SearchReferDieInWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchNormalDieInWafer"),	&CWaferPr::SearchNormalDieInWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchDieInWafer"),			&CWaferPr::SearchDieInWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchDigitalInWafer"),		&CWaferPr::SearchDigitalInWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchNmlDieAround"),		&CWaferPr::SearchNmlDieAround);

		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetupPrCrossHair"),			&CWaferPr::SetupPrCrossHair);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AdjustCrossHairStepByStep"),&CWaferPr::AdjustCrossHairStepByStep);

		// For Character die function
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectLearnCharDie"),		&CWaferPr::SelectLearnCharDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectCharDieNo"),			&CWaferPr::SelectCharDieNo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchCharDie"),		&CWaferPr::UserSearchCharDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchCharDieInFOV"),		&CWaferPr::SearchCharDieInFOV);

		// PR Dialog Box
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowPrDiagnosticDialog"),	&CWaferPr::ShowPrDiagnosticDialog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowPrServiceDialog"),		&CWaferPr::ShowPrServiceDialog);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetPRSysBondMode"),			&CWaferPr::SetPRSysBondMode);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetPRSysLiveMode"),			&CWaferPr::SetPRSysLiveMode);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BadCutSetupUsingDieSize"),	&CWaferPr::BadCutSetupUsingDieSize);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BadCutBinaryThreshold"),	&CWaferPr::BadCutBinaryThreshold);

		//MS100 AOI Zoom Z motor	//v4.04
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeAOIZoomZ"),				&CWaferPr::HomeAOIZoomZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerOnAOIZoomZ"),			&CWaferPr::PowerOnAOIZoomZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveAOIZoomZ"),				&CWaferPr::MoveAOIZoomZ);

		// For Motorized Zoom & Focus
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerOnZoom"),		&CWaferPr::PowerOnZoom);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerOnFocus"),		&CWaferPr::PowerOnFocus);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveZoom"),			&CWaferPr::MoveZoom);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveFocus"),		&CWaferPr::MoveFocus);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveZoomToHome"),	&CWaferPr::MoveZoomToHome);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveFocusToHome"),	&CWaferPr::MoveFocusToHome);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoFocus"),		&CWaferPr::AutoFocus);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmFocusLevel"),&CWaferPr::ConfirmFocusLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GotoFocusLevel"),	&CWaferPr::GotoFocusLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GotoHomeLevel"),	&CWaferPr::GotoHomeLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GotoTableFocus"),	&CWaferPr::GotoTableFocus);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FocusZSync"),		&CWaferPr::FocusZSync);

		// For Back Light Setup
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeBackLightElevator"),	&CWaferPr::HomeBackLightElevator);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerOnBackLightElevator"), &CWaferPr::PowerOnBackLightElevator);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveBackLightElevator"),	&CWaferPr::MoveBackLightElevator);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeBackLightElevatorPos"),	&CWaferPr::ChangeBackLightElevatorPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBackLightElevatorPos"),	&CWaferPr::ConfirmBackLightElevatorPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelBackLightPos"),			&CWaferPr::CancelBackLightPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("KeyInElevatorPos"),				&CWaferPr::KeyInElevatorPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveElevatorPositivePos"),		&CWaferPr::MoveElevatorPositivePos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveElevatorNegativePos"),		&CWaferPr::MoveElevatorNegativePos);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveBackLightToUpStandby"),		&CWaferPr::MoveBackLightToUpStandby);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveBackLightToSafeLevel"),		&CWaferPr::MoveBackLightToSafeLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveBackLightToUpStandbyRoutine"),	&CWaferPr::MoveBackLightToUpStandbyRoutine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BondMoveBackLightToStandby"),	&CWaferPr::BondMoveBackLightToStandby);
		

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearDisplay"),				&CWaferPr::ClearDisplay);

		//IM Support Cmds	//av2.83T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InitIMPopupPage"),			&CWaferPr::InitIMPopupPage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InitIMCriteriaPage"),		&CWaferPr::InitIMCriteriaPage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IMUserSearchDie"),			&CWaferPr::IMUserSearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InitHighLevelVisionInfo"),	&CWaferPr::InitHighLevelVisionInfo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IM_InitPrHighLevelInfoCmd"), &CWaferPr::IM_InitPrHighLevelInfoCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IMAutoLearnWafer"),			&CWaferPr::IMAutoLearnWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IMManualAutoLearnWafer"),	&CWaferPr::IMManualAutoLearnWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IMManualAutoLearnWaferSeperated"),	&CWaferPr::IMManualAutoLearnWaferSeperated);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IMChangeWafer"),			&CWaferPr::IMChangeWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IMWaferEndChecking"),		&CWaferPr::IMWaferEndChecking);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IM_StartWafer"),			&CWaferPr::IM_StartWafer);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SpiralSearchDie"),			&CWaferPr::SpiralSearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearBadCutPoints"),		&CWaferPr::ClearBadCutPoints);
		
		// Digital Zoom
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AdaptWaferInitHighLevelVisionInfo"), &CWaferPr::AdaptWaferInitHighLevelVisionInfo);

		//Other Fcns
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetMouseControlCmd"),			&CWaferPr::SetMouseControlCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CopyRefDieSettings"),			&CWaferPr::CopyRefDieSettings);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DrawCORCirclePattern"),			&CWaferPr::DrawCORCirclePattern);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Search2DBarCode"),				&CWaferPr::Search2DBarCode);	//v2.83T61
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Search2DBarCodeFromWafer"),		&CWaferPr::Search2DBarCodeFromWafer);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RemoveShowImages"),				&CWaferPr::RemoveShowImages);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnSelectShowPrImage"),			&CWaferPr::OnSelectShowPrImage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PreOnSelectShowPrImage"),		&CWaferPr::PreOnSelectShowPrImage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayPRRecordInSummary"),		&CWaferPr::DisplayPRRecordInSummary);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchAndSaveReferDieImages"),		&CWaferPr::SearchAndSaveReferDieImages);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchAndSaveNormalDieImages"),		&CWaferPr::SearchAndSaveNormalDieImages);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLookAroundDie_Cmd"),			&CWaferPr::AutoLookAroundDie_Cmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLookAroundNearDie"),			&CWaferPr::AutoLookAroundNearDie);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IMGetPRDeviceIdCmd"),				&CWaferPr::IMGetPRDeviceIdCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetLockPrMouseJoystickCmd"),		&CWaferPr::SetLockPrMouseJoystickCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InputSaveImagePath"),				&CWaferPr::InputSaveImagePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetSaveImagePath"),					&CWaferPr::GetSaveImagePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TurnOnOffPrLightingCmd"),			&CWaferPr::TurnOnOffPrLightingCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnablePickAndPlace"),				&CWaferPr::EnablePickAndPlace);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RefreshAllRingLighting"),			&CWaferPr::RefreshAllRingLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsFaceValueExists"),				&CWaferPr::IsFaceValueExists);	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenerateConfigData"),				&CWaferPr::GenerateConfigData);	

		DisplayMessage("WaferPRStn Service Registered...");


		//Register variables
		RegVariable(_T("SYS_bPRInit"),	&m_bPRInit);
		RegVariable(_T("WPR_bWprWithAF_Z"),	&m_bWprWithAF_Z);
		RegVariable(_T("WPR_bColorCamera"),	&m_bColorCamera);

		RegVariable(_T("WPR_bUseHWTrigger"),&m_bUseHWTrigger);

		RegVariable(_T("bFreezeCamera"),	&m_bFreezeCamera_HMI);
		//Learn Die variable
		RegVariable(_T("WPR_lLrnAlignAlgo"),		&m_lLrnAlignAlgo);
		RegVariable(_T("WPR_lLrnBackground"),		&m_lLrnBackground);
		RegVariable(_T("WPR_lLrnFineSrch"),			&m_lLrnFineSrch);
		RegVariable(_T("WPR_lLrnInspMethod"),		&m_lLrnInspMethod);
		RegVariable(_T("WPR_lLrnInspRes"),			&m_lLrnInspRes);
		RegVariable(_T("WPR_lCurRefDieNo"),			&m_lCurRefDieNo);
		RegVariable(_T("WPR_szLrnDieType"),			&m_szLrnDieType);
		RegVariable(_T("WPR_sz2DCodePrTitle"),		&m_sz2DCodePrTitle);
		RegVariable(_T("WPR_bEnableLrnAlignAlgo"),	&m_bEnableLrnAlignAlgo);		//andrew
		RegVariable(_T("WPR_lExposureTimeLevel"),	&m_lWprLrnExposureTimeHmi);
		RegVariable(_T("WPR_lGExposureTimeLevel"),	&m_lWPRGenExposureTimeHmi);
		RegVariable(_T("WPR_szDieTypeInfo"),		&m_szDieTypeInfo);
		RegVariable(_T("WPR_lGenCoaxLightLevel0"),	&m_lWPRGenCoaxLightLevel[0]);	
		RegVariable(_T("WPR_lGenRingLightLevel0"),	&m_lWPRGenRingLightLevel[0]);	
		RegVariable(_T("WPR_lGenSideLightLevel0"),	&m_lWPRGenSideLightLevel[0]);	

		for (i = 1; i < WPR_MAX_LIGHT_GROUP; i++)
		{
			szText.Format("WPR_lGenCoaxLightLevel%d", i);
			RegVariable(_T(szText), &m_lWPRGenCoaxLightLevel[i]);	

			szText.Format("WPR_lGenRingLightLevel%d", i);
			RegVariable(_T(szText), &m_lWPRGenRingLightLevel[i]);	

			szText.Format("WPR_lGenSideLightLevel%d", i);
			RegVariable(_T(szText), &m_lWPRGenSideLightLevel[i]);	
		}

		RegVariable(_T("WPR_lLrnCoaxLightLevel0"), &m_lWPRLrnCoaxLightHmi);	
		RegVariable(_T("WPR_lLrnRingLightLevel0"), &m_lWPRLrnRingLightHmi);	
		RegVariable(_T("WPR_lLrnSideLightLevel0"), &m_lWPRLrnSideLightHmi);	

		szText.Format("WPR_lLrnCoaxLightLevel%d", 1);
		RegVariable(_T(szText), &m_lWPRLrnCoaxLightLevel[1]);	

		szText.Format("WPR_lLrnRingLightLevel%d", 1);
		RegVariable(_T(szText), &m_lWPRLrnRingLightLevel[1]);	

		szText.Format("WPR_lLrnSideLightLevel%d", 1);
		RegVariable(_T(szText), &m_lWPRLrnSideLightLevel[1]);	

		RegVariable(_T("WPR_lGenBackLightLevel0"),	&m_lWPRGenBackLightLevel[0]);
		RegVariable(_T("WPR_lGenBackLightLevel1"),	&m_lWPRGenBackLightLevel[1]);
		RegVariable(_T("WPR_lLrnBackLightLevel0"),	&m_lWPRLrnBackLightLevel[0]);
		RegVariable(_T("WPR_lLrnBackLightLevel1"),	&m_lWPRLrnBackLightLevel[1]);
		RegVariable(_T("WPR_bUseBackLight"),		&m_bWprUseBackLight);
		RegVariable(_T("WPR_lBLZAutoFocusDelay"),	&m_lBLZAutoFocusDelay);

		//pllm
		RegVariable(_T("PPR_lGenCoaxLightLevel"), &m_lPPRGenCoaxLightLevel);
		RegVariable(_T("PPR_lGenRingLightLevel"), &m_lPPRGenRingLightLevel);
		RegVariable(_T("PPR_lGenSideLightLevel"), &m_lPPRGenSideLightLevel);

		//Learn Character Die Record		
		RegVariable(_T("WPR_bLrnDigit0"), &m_bLrnDigit0);
		RegVariable(_T("WPR_bLrnDigit1"), &m_bLrnDigit1);
		RegVariable(_T("WPR_bLrnDigit2"), &m_bLrnDigit2);
		RegVariable(_T("WPR_bLrnDigit3"), &m_bLrnDigit3);
		RegVariable(_T("WPR_bLrnDigit4"), &m_bLrnDigit4);
		RegVariable(_T("WPR_bLrnDigit5"), &m_bLrnDigit5);
		RegVariable(_T("WPR_bLrnDigit6"), &m_bLrnDigit6);
		RegVariable(_T("WPR_bLrnDigit7"), &m_bLrnDigit7);
		RegVariable(_T("WPR_bLrnDigit8"), &m_bLrnDigit8);
		RegVariable(_T("WPR_bLrnDigit9"), &m_bLrnDigit9);
		RegVariable(_T("WPR_bEnableOCR"), &m_bEnableOCR);
		RegVariable(_T("WPR_ulOcrDieOrientation"),	&m_ulOcrDieOrientation);

		//Search Die variable
		RegVariable(_T("WPR_lSrchAlignRes"),			&m_lSrchAlignRes);	
		RegVariable(_T("WPR_lSrchDefectThres"),			&m_lSrchDefectThres);	
		RegVariable(_T("WPR_lSrchDieAreaX"),			&m_lSrchDieAreaX);	
		RegVariable(_T("WPR_lSrchDieAreaY"),			&m_lSrchDieAreaY);	
		RegVariable(_T("WPR_dSrchDieAreaX"),			&m_dSrchDieAreaX);	
		RegVariable(_T("WPR_dSrchDieAreaY"),			&m_dSrchDieAreaY);	
		RegVariable(_T("WPR_lSrchDieScore"),			&m_lSrchDieScore);	
		RegVariable(_T("WPR_lSrchGreyLevelDefect"),		&m_lSrchGreyLevelDefect);	
		RegVariable(_T("WPR_bSrchEnableBackupAlign"),	&m_bSrchEnableBackupAlign);
		RegVariable(_T("WPR_bSrchEnableLookForward"),	&m_bSrchEnableLookForward);
		RegVariable(_T("WPR_bSrchEnableDefectCheck"),	&m_bSrchEnableDefectCheck);
		RegVariable(_T("WPR_bSrchEnableChipCheck"),		&m_bSrchEnableChipCheck);
		RegVariable(_T("WPR_dSrchSingleDefectArea"),	&m_dSrchSingleDefectArea);

		RegVariable(_T("WPR_dSrchTotalDefectArea"),		&m_dSrchTotalDefectArea);
		RegVariable(_T("WPR_dSrchChipArea"),			&m_dSrchChipArea);
		RegVariable(_T("WPR_bThetaCorrection"),			&m_bThetaCorrection);
		RegVariable(_T("WPR_bBinTableThetaCorrection"),	&m_bBinTableThetaCorrection);
		RegVariable(_T("WPR_bConfirmSearch"),			&m_bConfirmSearch);	
		RegVariable(_T("WPR_bBadCutDetection"),			&m_bBadCutDetection);	
		RegVariable(_T("WPR_dMaxDieAngle"),				&m_dMaxDieAngle);	
		RegVariable(_T("WPR_dMinDieAngle"),				&m_dMinDieAngle);	
		RegVariable(_T("WPR_dAcceptDieAngle"),			&m_dAcceptDieAngle);	
		RegVariable(_T("WPR_lSrchRefDieNo"),			&m_lSrchRefDieNo);
		RegVariable(_T("WPR_bRefDieCheck"),				&m_bRefDieCheck);
		RegVariable(_T("WPR_bAllRefDieCheck"),			&m_bAllRefDieCheck);
		RegVariable(_T("WPR_bReverseDieStatus"),		&m_bReverseDieStatus);
		RegVariable(_T("WPR_bPickAlignableDie"),		&m_bPickAlignableDie);
		RegVariable(_T("WPR_bCheckAllNmlDie"),			&m_bCheckAllNmlDie);
		RegVariable(_T("WPR_ulMaxSkipCount"),			&m_ulMaxSkipCount);
		RegVariable(_T("WPR_ulMaxNoDieSkipCount"),		&m_ulMaxNoDieSkipCount);
		RegVariable(_T("WPR_ulMaxPRSkipCount"),			&m_ulMaxPRSkipCount);
		RegVariable(_T("WPR_ulMaxDefectSkipCount"),		&m_ulMaxDefectSkipCount);
		RegVariable(_T("WPR_ulMaxBadCutSkipCount"),		&m_ulMaxBadCutSkipCount);
		RegVariable(_T("WPR_dGoodDieRatio"),			&m_dWaferGoodDieRatio);
		RegVariable(_T("WPR_ulMaxStartSkipCount"),		&m_ulMaxStartSkipLimit);
		RegVariable(_T("WPR_bEnableBHUplookPr"),		&m_bEnableBHUplookPr);		//v4.57A8
		RegVariable(_T("WPR_bEnableWTIndexTest"),		&m_bEnableWTIndexTest);
		
		RegVariable(_T("WPR_bAutoLearnRefDie"),			&m_bAutoLearnRefDie);		//v2.78T2
		RegVariable(_T("WPR_bPreBondAtPick"),			&m_bPreBondAtPick);			//v3.34
		RegVariable(_T("WPR_bAoiSortInsp"),				&m_bAoiSortInspction);
		RegVariable(_T("WPR_bAOINgPickPartDie"),		&m_bAOINgPickPartDie);
		RegVariable(_T("WPR_lNgPickDieAreaTolMin"),		&m_lNgPickDieAreaTolMin);
		RegVariable(_T("WPR_lNgPickDieAreaTolMax"),		&m_lNgPickDieAreaTolMax);
		RegVariable(_T("WPR_bBadcutBiDetect"),			&m_bBadcutBiDetect);
		RegVariable(_T("WPR_lBadcutBiThreshold"),		&m_lBadcutBiThreshold);
		RegVariable(_T("WPR_bNormalRoughLedDie"),		&m_bNormalRoughLedDie);
		RegVariable(_T("WPR_lRefDieFaceValueNo"),		&m_lRefDieFaceValueNo);		//v4.48A26	//Avago

		//v3.17T1
		RegVariable(_T("WPR_bEnableLineDefect"),		&m_bEnableLineDefect);
		RegVariable(_T("WPR_dMinLineDefectLength"),		&m_dMinLineDefectLength);
		RegVariable(_T("WPR_lMinNumOfLineDefects"),		&m_lMinNumOfLineDefects);

		RegVariable(_T("WPR_bAutoEnlargeSrchWnd"),		&m_bAutoEnlargeSrchWnd);

		//Bad cut
		RegVariable(_T("WPR_lBadCutTolerenceX"), &m_stBadCutTolerenceX);	
		RegVariable(_T("WPR_lBadCutTolerenceY"), &m_stBadCutTolerenceY);	
		//RegVariable(_T("WPR_lScanWidthX"), &m_stBadCutScanWidthX);	
		//RegVariable(_T("WPR_lScanWidthY"), &m_stBadCutScanWidthY);
		
		//Calibration
		RegVariable(_T("WPR_dCalibX"),	&m_dCalibX);
		RegVariable(_T("WPR_dCalibXY"), &m_dCalibXY);
		RegVariable(_T("WPR_dCalibY"),	&m_dCalibY);
		RegVariable(_T("WPR_dCalibYX"), &m_dCalibYX);

		//Die Shape
		RegVariable(_T("WPR_ucDieShape"),			&m_ucDieShape);	
		RegVariable(_T("WPR_bEnableRhombusDie"),	&m_bEnableRhombusDie);

		//Other variable
		RegVariable(_T("WPR_bDebugVideoTest"), &m_bDebugVideoTest);	
		RegVariable(_T("WPR_lAutoScreenFG"), &m_lAutoScreenFG);	
		RegVariable(_T("WPR_lAutoScreenMode"), &m_lAutoScreenMode);	
		RegVariable(_T("WPR_lDebugFlag"), &m_lDebugFlag);	
		RegVariable(_T("WPR_lDebugLog"), &m_lDebugLog);	

		//HMI variable
		RegVariable(_T("WPR_bSelectDieType"), &m_bSelectDieType);	
		RegVariable(_T("WPR_bCurrentCamera"), &m_bCurrentCamera);	
		RegVariable(_T("WPR_bKeepShowBondInAuto"),	&m_bKeepShowBondInAuto);

		//Show information into HMI
		RegVariable(_T("WPR_bDisplayDieInUM"),	&m_bDisplayDieInUM);
		RegVariable(_T("WPR_lPrCenterX"),		&m_lPrCenterX);
		RegVariable(_T("WPR_lPrCenterY"),		&m_lPrCenterY);
		RegVariable(_T("WPR_lPrCenterXInPixel"), &m_lPrCenterXInPixel);
		RegVariable(_T("WPR_lPrCenterYInPixel"), &m_lPrCenterYInPixel);
		RegVariable(_T("WPR_bDieLearnt"),		&m_bDieIsLearnt);	
		RegVariable(_T("WPR_bDieCalibrated"),	&m_bDieCalibrated);	
		RegVariable(_T("WPR_bPitchLearnt"),		&m_bPitchLearnt);	
		RegVariable(_T("WPR_lCurNormDieID"),	&m_lCurNormDieID);	
		RegVariable(_T("WPR_lCurNormDieSizeX"), &m_lCurNormDieSizeX);	
		RegVariable(_T("WPR_lCurNormDieSizeY"), &m_lCurNormDieSizeY);	
		RegVariable(_T("WPR_dCurNormDieSizeX"), &m_dCurNormDieSizeX);	
		RegVariable(_T("WPR_dCurNormDieSizeY"), &m_dCurNormDieSizeY);	
		RegVariable(_T("WPR_lCurBadCutSizeX"),	&m_lCurBadCutSizeX);	
		RegVariable(_T("WPR_lCurBadCutSizeY"),	&m_lCurBadCutSizeY);	
		RegVariable(_T("WPR_lLrnTotalRefDie"),	&m_lLrnTotalRefDie);
		RegVariable(_T("WPR_dFieldOfViewSize"), &m_dLFSize);
		RegVariable(_T("WPR_lCurPitchSizeX"),	&m_lCurPitchSizeX);	
		RegVariable(_T("WPR_lCurPitchSizeY"),	&m_lCurPitchSizeY);	
		RegVariable(_T("WPR_dCurPitchSizeX"),	&m_dCurPitchSizeX);	
		RegVariable(_T("WPR_dCurPitchSizeY"),	&m_dCurPitchSizeY);	
		RegVariable(_T("WPR_szSrhDieResult"),	&m_szSrhDieResult);
		RegVariable(_T("WPR_dMaxEpoxySizeFactor"),	&m_dMaxEpoxySizeFactor);	//v4.57A1

		RegVariable(_T("WPR_bUseAutoBondMode"),			&m_bUseAutoBondMode);
		
		// Camera Button color	CP Map Display
		RegVariable(_T("WPR_ulWaferCameraColor"),	&m_ulWaferCameraColor);	
		RegVariable(_T("WPR_ulBondCameraColor"),	&m_ulBondCameraColor);	
		RegVariable(_T("WPR_ul2DCameraColor"),		&m_ul2DCameraColor);		//pllm
		RegVariable(_T("WPR_bCPItemMapToTop"),		&m_bCPItemMapToTop);
		RegVariable(_T("WPR_ulPrWindowHeight"),		&m_ulPrWindowHeight);
		RegVariable(_T("WPR_ulPrWindowWidth"),		&m_ulPrWindowWidth);
		RegVariable(_T("WPR_ulBH1UplookCameraColor"),		&m_ulBH1UplookCameraColor); //4.52D17Color
		RegVariable(_T("WPR_ulBH2UplookCameraColor"),		&m_ulBH2UplookCameraColor); 
		// PR Software Version and Release Number
		RegVariable(_T("WPR_szSoftVersion"), &m_szSoftVersion);
		RegVariable(_T("WPR_szSWReleaseNo"), &m_szSWReleaseNo);

		RegVariable(_T("WPR_dAlignPassScore"),			&m_dAlignPassScore);
		RegVariable(_T("WPR_dSingleDefectScore"),		&m_dSingleDefectScore);
		RegVariable(_T("WPR_dTotalDefectScore"),		&m_dTotalDefectScore);
		RegVariable(_T("WPR_dChipAreaScore"),			&m_dChipAreaScore);

		RegVariable(_T("WPR_dCurAlignPassScore"),		&m_dCurAlignPassScore);
		RegVariable(_T("WPR_dCurSingleDefectScore"),	&m_dCurSingleDefectScore);
		RegVariable(_T("WPR_dCurTotalDefectScore"),		&m_dCurTotalDefectScore);
		RegVariable(_T("WPR_dCurChipAreaScore"),		&m_dCurChipAreaScore);
		RegVariable(_T("WPR_szPrStatus"),				&m_szCurPrStatus);

		RegVariable(_T("WPR_dSetRegionByDieSizeX"),		&m_dSetRegionByDieSizeX);
		RegVariable(_T("WPR_dSetRegionByDieSizeY"),		&m_dSetRegionByDieSizeY);

		RegVariable(_T("WPR_bF1SearchDieWithRoatation"), &m_bF1SearchDieWithRoatation);

		// For Motorized Zoom & Focus
		// For Displaying the Sensor Status
		RegVariable(_T("WPR_bZoomPower"),		&m_bZoomPower);
		RegVariable(_T("WPR_bFocusPower"),		&m_bFocusPower);
		RegVariable(_T("WPR_bZoomHomeSensor"), &m_bZoomHomeSensor);
		RegVariable(_T("WPR_bFocusHomeSensor"), &m_bFocusHomeSensor);
		RegVariable(_T("WPR_bZoomLimitSensor"), &m_bZoomLimitSensor);
		RegVariable(_T("WPR_bFocusLimitSensor"), &m_bFocusLimitSensor);

		RegVariable(_T("WPR_bAFPower"),			&m_bAutoFocusPower);
		RegVariable(_T("WPR_bAFHomeSensor"),	&m_bAutoFocusHomeSensor);
		RegVariable(_T("WPR_bAFLimitSensor"),	&m_bAutoFocusLimitSensor);
		RegVariable(_T("WPR_bAutoFocusWafer"),	&m_bWaferAutoFocus);
		RegVariable(_T("WPR_lEnc_AF"),				&m_lEnc_AF);
		RegVariable(_T("WPR_lAFPrDelay"),			&m_lAFPrDelay);
		RegVariable(_T("WPR_lAutoFocusUpLmt"),		&m_lAFUpLimit);
		RegVariable(_T("WPR_lAutoFocusLowLmt"),		&m_lAFLowLimit);
		RegVariable(_T("WPR_lAutoFocusErrorTol"),	&m_lAFErrorTol);
		RegVariable(_T("WPR_lAutoFocusMinDist"),	&m_lAFMinDist);
		RegVariable(_T("WPR_lAutoFocusPosition"),	&m_lAutoFocusPosition);
		RegVariable(_T("WPR_lAutoFocusPosition2"),	&m_lAutoFocusPosition2);
		RegVariable(_T("WPR_lAFZPosLimit"),			&m_lAFZPosLimit);
		RegVariable(_T("WPR_lAFZNegLimit"),			&m_lAFZNegLimit);
		RegVariable(_T("WPR_lAFStandbyOffset"),		&m_lAFStandbyOffset);
		RegVariable(_T("WPR_dAFrValueLimit"),		&m_dAFrValueLimit);
		RegVariable(_T("WPR_bAutoFocusDown"),		&m_bAutoFocusDown);
		RegVariable(_T("WPR_bAFDynamicAdjust"),			&m_bAFDynamicAdjust);
		RegVariable(_T("WPR_bAFRescanBadFrame"),		&m_bAFRescanBadFrame);
		RegVariable(_T("WPR_lAFDynamicAdjustStep"),		&m_lAFDynamicAdjustStep);
		RegVariable(_T("WPR_dAFDynamicAdjustLimit"),	&m_dAFDynamicAdjustLimit);
		RegVariable(_T("WPR_dAFRescanBadFrameLimit"),	&m_dAFRescanBadFrameLimit);

		// backlight variable
		RegVariable(_T("WPR_lEnc_BLZ"),				&m_lEnc_BLZ);
		RegVariable(_T("WPR_bBLZHome"),				&m_bBLZHome);
		RegVariable(_T("WPR_bBLZPower"),			&m_bBLZPower);
		RegVariable(_T("WPR_bSelBL_Z"),				&m_bSelBL_Z);
		
		RegVariable(_T("WPR_lWPRGeneral_TMP"),	&m_lWPRGeneral_TMP);
		RegVariable(_T("WPR_lWPRGeneral_1"),	&m_lWPRGeneral_1);
		RegVariable(_T("WPR_lWPRGeneral_2"),	&m_lWPRGeneral_2);

		// For Displaying the position of the zoom and focus motors
		RegVariable(_T("WPR_lEncFocus"), &m_lEnc_Focus);
		RegVariable(_T("WPR_lEncZoom"), &m_lEnc_Zoom);

		//Digital Zoom
		RegVariable(_T("WPR_bEnableZoomSensor"),	&m_stZoomView.m_bEnableZoomSensor);
		RegVariable(_T("WPR_szZoomFactor"),			&m_stZoomView.m_szZoomFactor);
		RegVariable(_T("WPR_bFixZoom"),				&m_stZoomView.m_bIsFixZoom);
		RegVariable(_T("WPR_lScreenZoomFactor"),	&m_lRunDigitalZoom);
		RegVariable(_T("WPR_ucFovShrinkMode"),		&m_ucFovShrinkMode);
		RegVariable(_T("WPR_bContinuousZoom"),		&m_bContinuousZoomHMI);
		RegVariable(_T("WPR_dNmlZoomSubFactor"),	&m_dConZoomSubFactorHMI);
		RegVariable(_T("WPR_dWprZoomRoiShrink"),	&m_dWprZoomRoiShrinkHMI);

		RegVariable(_T("WPR_dWAlignMasterPitchInMilX"),		&m_dWAlignMasterPitchInMilX);	//v2.78T2
		RegVariable(_T("WPR_dWAlignMasterPitchInMilY"),		&m_dWAlignMasterPitchInMilY);	//v2.78T2

		for(int i=0; i<3; i++)
		{
			CString szVarName;
			szVarName.Format("WPR_dRMasterDieSizeX%d", i+1);
			RegVariable(_T(szVarName),			&m_dRMasterDieSizeX[i]);
			szVarName.Format("WPR_dRMasterDieSizeY%d", i+1);
			RegVariable(_T(szVarName),			&m_dRMasterDieSizeY[i]);
		}
		RegVariable(_T("WPR_lLearnPitchSpanX"),				&m_lLearnPitchSpanX);
		RegVariable(_T("WPR_lLearnPitchSpanY"),				&m_lLearnPitchSpanY);

		RegVariable(_T("WPR_lNDieOffsetInMotorStepX"),		&m_lNDieOffsetInMotorStepX);	//v3.77
		RegVariable(_T("WPR_lNDieOffsetInMotorStepY"),		&m_lNDieOffsetInMotorStepY);	//v3.77
		RegVariable(_T("WPR_bPrCircleDetection"),			&m_bPrCircleDetection);			//v2.78T1
		RegVariable(_T("WPR_lPrCal1stStepSize"),			&m_lPrCal1stStepSize);			//v2.89T1
		RegVariable(_T("WPR_bIMAutoLearnWafer"),			&m_bIMAutoLearnWafer);			//v3.06
		RegVariable(_T("WPR_bEnable2Lighting"),				&m_bEnable2Lighting);			//v3.60
		RegVariable(_T("WPR_bCompareHomeReferDieScore"),	&m_bCompareHomeReferDieScore);
// prescan relative code
		RegPrescanVarFunc();

		RegVariable(_T("WPR_szPKGNormalDieRecordPath"),	&m_szPKGNormalDieRecordPath);
		RegVariable(_T("WPR_szPKGRefDieRecordPath"),	&m_szPKGRefDieRecordPath);
		RegVariable(_T("WPR_szSaveImagePath"),			&m_szSaveImagePath);
		RegVariable(_T("WPR_bEnableAlignWaferImageLog"), &m_bEnableAlignWaferImageLog);

		//RegVariable(_T("WPR_szShowImagePath"),			&m_szShowImagePath);
		for (INT i = 0; i < WPR_MAX_DIE; i++)
		{
			szText.Format("WPR_szShowImagePath%d", i + 1);
			RegVariable(_T(szText),	&m_szShowImagePath[i]);
		}

		RegVariable(_T("WPR_szGenShowImagePath"),		&m_szGenShowImagePath);
		RegVariable(_T("WPR_szShowImageType"),			&m_szShowImageType);
		RegVariable(_T("WPR_lShowImageNo"),				&m_lShowImageNo);

		RegVariable(_T("WPR_lMouseJoystickSpeed"),	&m_lMouseJoystickSpeed);
		RegVariable(_T("WPR_bEnableMouseJoystick"),	&m_bEnableMouseJoystick);

		RegVariable(_T("WPR_lWPRSpecial1_RingLightLevel"), &m_lWPRSpecial1_RingLightLevel);
		RegVariable(_T("WPR_lWPRSpecial2_RingLightLevel"), &m_lWPRSpecial2_RingLightLevel);

		RegisterEjtVariables();		//v4.44T2

		DisplayMessage("WaferPRStn variables Registered...");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}



/////////////////////////////////////////////////////////////////
//Register HIM Command
/////////////////////////////////////////////////////////////////
LONG CWaferPr::MeasureObject(IPC_CServiceMessage &svMsg)
{
	PR_GET_MOUSE_CTRL_OBJ_CMD		stGetCmd; 
	PR_GET_MOUSE_CTRL_OBJ_RPY		stGetRpy;
	PR_ERASE_MOUSE_CTRL_OBJ_CMD		stEraseCmd;
	PR_ERASE_MOUSE_CTRL_OBJ_RPY		stEraseRpy;
	PR_COORD pt1, pt2;
	DOUBLE dXum = 0, dYum = 0;
	LONG lDistX = 0, lDistY = 0;

	if (HmiMessage("Measure Object?", "System Message", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL, 400, 300) == glHMI_YES)
	{	
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		CString szMsg, szTemp;
		m_pPrGeneral->MouseDefineRegion();
		CloseAlarm();
		szMsg = "Please use mouse to locate";
		szMsg += "\nObject Region";
		szMsg += "\nClick <Close> when completed";
		DispMsg(szMsg);

		PR_InitGetMouseCtrlObjCmd(&stGetCmd);
		stGetCmd.ulObjectId = 1;
		PR_GetMouseCtrlObjCmd(&stGetCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stGetRpy);
		pt1 = stGetRpy.acoObjCorner[0];
		pt2 = stGetRpy.acoObjCorner[1];
		lDistX = (pt2.x - pt1.x);
		lDistY = (pt2.y - pt1.y);
		switch (m_bCurrentCamera)
		{
		case 0: //Wafer Cam
			ConvertPixelToUM(lDistX, lDistY, dXum, dYum);
			break;
		case 1: //Bond Clam
			if (pBondPr != NULL)
			{
				pBondPr->ConvertPixelToUM(lDistX, lDistY, dXum, dYum);
			}
			break;
		default:
			ConvertPixelToUM(lDistX, lDistY, dXum, dYum);
			break;
		}

		szMsg = "Measured Object Size\n";
		szTemp.Format("X=%.1f, Y=%.1f um\n", dXum, dYum);
		szMsg += szTemp;
		szTemp.Format("X=%f, Y=%f pixels", (DOUBLE)_round(abs(pt1.x - pt2.x) / 16.0), (DOUBLE)_round(abs(pt1.y - pt2.y) / 16.0));
		szMsg += szTemp;
		DispMsg(szMsg);
		// Erase the mouse object
		CloseAlarm();
		PR_InitEraseMouseCtrlObjCmd(&stEraseCmd);
		stEraseCmd.ulObjectId = 1;
		PR_EraseMouseCtrlObjCmd(&stEraseCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stEraseRpy);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::SelectCamera(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (IsAOIOnlyMachine())
	{
		m_bCurrentCamera = WPR_CAM_WAFER;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_bCurrentCamera++;
	if (m_bCurrentCamera > WPR_CAM_BOND)
	{
		m_bCurrentCamera = WPR_CAM_WAFER;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

//v4.05		//MS100 AOI
#ifdef NU_MOTION
	if ((State() == AUTO_Q) || (State() == DEMO_Q))
	{
		m_bPrAoiSwitchCamera = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
#endif

	ChangeCamera(m_bCurrentCamera);

	//v4.39T11
	if (m_bCurrentCamera == WPR_CAM_WAFER)
	{
		//Update HMI lighting attributes for WPR
		WPR_GetGeneralLighting();
	}


	//v3.99T1
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SelectWaferCamera(IPC_CServiceMessage &svMsg)
{
	if (PR_NotInit())
	{
		return 1;
	}

	m_bCurrentCamera = WPR_CAM_WAFER;

//v3.93		//MS100 AOI
#ifdef NU_MOTION
	if ((State() == AUTO_Q) || (State() == DEMO_Q))
	{
		m_ulPrWindowHeight		= 480;
		m_ulPrWindowWidth		= 512;
		m_bCPItemMapToTop		= FALSE;
		m_bPrAoiSwitchCamera = TRUE;
		return 1;
	}
#endif

	ChangeCamera(m_bCurrentCamera);

	WPR_GetGeneralLighting();

	TurnOffPSLightings();

	return 1;
}


LONG CWaferPr::SelectBondCamera(IPC_CServiceMessage &svMsg)
{
	if (IsAOIOnlyMachine())
	{
		return 1;
	}
	if (PR_NotInit())
	{
		return 1;
	}

	m_bCurrentCamera = WPR_CAM_BOND;//WPR_CAM_WAFER;//WPR_CAM_BOND;

//v3.93		//MS100 AOI
#ifdef NU_MOTION
	if ((State() == AUTO_Q) || (State() == DEMO_Q))
	{
		m_ulPrWindowHeight		= 480;
		m_ulPrWindowWidth		= 512;
		m_bCPItemMapToTop		= FALSE;
		m_bPrAoiSwitchCamera = TRUE;
		return 1;
	}
#endif

	ChangeCamera(m_bCurrentCamera);

	TurnOffPSLightings();

	return 1;
}


LONG CWaferPr::SelectPostSealCamera(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (PR_NotInit())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->GetFeatureStatus(MS896A_FUNC_VISION_POSTSEAL_OPTICS))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	UCHAR ucPostSealID = 1;		//1=WL Lookup; 2=BH1 lookup; 3=BH2 Lookup
	svMsg.GetMsg(sizeof(UCHAR), &ucPostSealID);

	m_bCurrentCamera = WPR_CAM_WAFER;

	//v4.40T1
	if (!pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS) &&
		!pApp->GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA))
	{
		ucPostSealID = 1;	
	}

	if( pApp->GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA) )
	{
		if( ucPostSealID!=4 && ucPostSealID!=5 )
		{
			if( m_unCurrPostSealID!=4 )
			{
				ucPostSealID = 4;
			}
			else
			{
				ucPostSealID = 5;
			}
		}
		BOOL bWT2 = FALSE;
		if( ucPostSealID==4 )
		{
			m_sz2DCodePrTitle	= "WL1 Cam";
		}
		else
		{
			bWT2 = TRUE;
			m_sz2DCodePrTitle	= "WL2 Cam";
		}
		m_bCurrentCamera = WPR_CAM_WAFER;
		m_unCurrPostSealID = ucPostSealID;		//	CONTOUR
		ChangeCamera(m_bCurrentCamera, TRUE, m_unCurrPostSealID);

		m_lWPRGenCoaxLightLevel[0]	= m_lWL1CoaxLightLevel;
		m_lWPRGenRingLightLevel[0]	= m_lWL1RingLightLevel;
		m_lWPRGenSideLightLevel[0]	= m_lWL1SideLightLevel;
		m_lWPRGenBackLightLevel[0]	= m_lWL1BackLightLevel;
		if( bWT2 )
		{
			m_lWPRGenCoaxLightLevel[0]	= m_lWL2CoaxLightLevel;
			m_lWPRGenRingLightLevel[0]	= m_lWL2RingLightLevel;
			m_lWPRGenSideLightLevel[0]	= m_lWL2SideLightLevel;
			m_lWPRGenBackLightLevel[0]	= m_lWL2BackLightLevel;
		}
		CString szMsg;
		szMsg.Format("contour select cam  on, WT %d", bWT2);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		SetContourLighting(TRUE, bWT2);			//Get PR lighting from Vision
	}
	else
	{
		m_unCurrPostSealID = ucPostSealID;
		//4.52D17light  *BH1 & *BH2 (Select PS camera)
		if( ucPostSealID == 6 || ucPostSealID == 7)  
		{
			//v4.59A8	//Use WPR_CAM_WAFER to access BH Uplook lighting
			//m_bCurrentCamera = WPR_CAM_BOND;	//WPR_REFERENCE_DIE;	
		}
		
		ChangeCamera(m_bCurrentCamera, TRUE, m_unCurrPostSealID);
		GetPostSealLighting(m_unCurrPostSealID);			//Get PR lighting from Vision
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::TurnOffPSLighting(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_unCurrPostSealID == 6)
	{
		SetPostSealGenLighting(FALSE, 6);
	}
	else if (m_unCurrPostSealID == 7)
	{
		SetPostSealGenLighting(FALSE, 7);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}


LONG CWaferPr::SwitchToTarget(IPC_CServiceMessage &svMsg)
{
	BOOL bTarget;
	svMsg.GetMsg(sizeof(BOOL), &bTarget);

	if (bTarget != WPR_CAM_BOND)
	{
		bTarget = WPR_CAM_WAFER;
	}

	if (bTarget == m_bCurrentCamera)
	{
		return 1;
	}

	//bTarget = WPR_CAM_WAFER;
	m_bCurrentCamera = bTarget;

#ifdef NU_MOTION
	if ((State() == AUTO_Q) || (State() == DEMO_Q))
	{
		m_bPrAoiSwitchCamera = TRUE;
		return 1;
	}
#endif

	ChangeCamera(m_bCurrentCamera, FALSE);
	return 1;
}


LONG CWaferPr::SwitchToTarget_dbh(IPC_CServiceMessage &svMsg)
{
	//if (State() != IDLE_Q)
	//if ((State() == AUTO_Q) || (State() == DEMO_Q) || m_bAutoBondMode)	//v4.12T1	//Cree US
	//{
	//	m_bCurrentCamera = WPR_CAM_WAFER;
	//}
	ChangeCamera(m_bCurrentCamera, FALSE);
	m_bPrAoiSwitchCamera = FALSE;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::GetGeneralLighting(IPC_CServiceMessage &svMsg)
{
	return WPR_GetGeneralLighting();
}


LONG CWaferPr::SetGeneralExposureTime(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bFreezeCamera_HMI || PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_EXPOSURE_TIME stTime;
	LONG			ExposureTimeLevel;

	svMsg.GetMsg(sizeof(LONG), &ExposureTimeLevel);

	// Saturate the levels
	if (ExposureTimeLevel > 10)
	{
		ExposureTimeLevel = 10;
	}

	if (ExposureTimeLevel < 0)
	{
		ExposureTimeLevel = 0;
	}

	if( IsDP_ScanCam() )
		m_lSCMGenExposureTimeLevel	= ExposureTimeLevel;
	else
		m_lWPRGenExposureTimeLevel	= ExposureTimeLevel;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();
	stTime = (PR_EXPOSURE_TIME)ExposureTimeLevel;

	CString szErrMsg;
	if (!m_pPrGeneral->SetExposureTime(ubPpsG, stTime, ubSID, ubRID, szErrMsg))
	{
		HmiMessage(szErrMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CWaferPr::GetGeneralExposureTime()
{
	if (PR_NotInit())
	{
		return TRUE;
	}

	PR_EXPOSURE_TIME stTime = (PR_EXPOSURE_TIME)0;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();

	CString szErrMsg;
	if (!m_pPrGeneral->GetExposureTime(ubPpsG, ubSID, ubRID, stTime, szErrMsg))
	{
		HmiMessage(szErrMsg);
	}

	m_lWPRGenExposureTimeHmi = m_lWPRGenExposureTimeLevel = (LONG) stTime;

	if( IsDP() )
	{
		ubSID = GetScnSenID();	ubRID = GetScnRecID();
		ubPpsG = GetScnPurpose();

		if (!m_pPrGeneral->GetExposureTime(ubPpsG, ubSID, ubRID, stTime, szErrMsg))
		{
			HmiMessage(szErrMsg);
		}
		m_lSCMGenExposureTimeLevel	= (LONG) stTime;
	}

	return TRUE;
}


LONG CWaferPr::RestoreGeneralLighting(IPC_CServiceMessage &svMsg)
{
	if (PR_NotInit())
	{
		return 1;
	}

	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_SOURCE		emCoaxID, emRingID, emSideID;
	int				i;


	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();
	PR_SOURCE emBackID = PR_BACK_LIGHT;
	PR_GetOptic(ubPpsG, ubSID, ubRID, &stOptic, &stRpy);
	GetLightingConfig(0, emCoaxID, emRingID, emSideID);
	PR_SetLighting(emCoaxID, (PR_UWORD)m_lWPRGenCoaxLightLevel[0], ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emRingID, (PR_UWORD)m_lWPRGenRingLightLevel[0], ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emSideID, (PR_UWORD)m_lWPRGenSideLightLevel[0], ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emBackID, (PR_UWORD)m_lWPRGenBackLightLevel[0], ubSID, ubRID, &stOptic, &stRpy);
	if (CMS896AStn::m_bEnablePRDualColor == TRUE)
	{
		for (i = 1; i < WPR_MAX_LIGHT_GROUP; i++)
		{
			GetLightingConfig(i, emCoaxID, emRingID, emSideID);

			PR_SetLighting(emCoaxID, (PR_UWORD)m_lWPRGenCoaxLightLevel[i], ubSID, ubRID, &stOptic, &stRpy);
			PR_SetLighting(emRingID, (PR_UWORD)m_lWPRGenRingLightLevel[i], ubSID, ubRID, &stOptic, &stRpy);
			PR_SetLighting(emSideID, (PR_UWORD)m_lWPRGenSideLightLevel[i], ubSID, ubRID, &stOptic, &stRpy);
			PR_SetLighting(emBackID, (PR_UWORD)m_lWPRGenBackLightLevel[i], ubSID, ubRID, &stOptic, &stRpy);
		}
	}

	return 1;
}



LONG CWaferPr::SetGeneralLighting(IPC_CServiceMessage &svMsg)
{
	BOOL			bReturn = TRUE;
	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stGnOptic;
	PR_OPTIC		stPrOptic;
	PR_SOURCE		emCoaxID, emRingID, emSideID;

	if (m_bFreezeCamera_HMI)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_lWPRGenCoaxLightLevel_Backup = m_lWPRGenCoaxLightLevel[0];
	m_lWPRGenSideLightLevel_Backup = m_lWPRGenSideLightLevel[0];
	m_lWPRGenRingLightLevel_Backup = m_lWPRGenRingLightLevel[0];
	m_lWPRGenBackLightLevel_Backup = m_lWPRGenBackLightLevel[0];

	typedef struct 
	{
		LONG lGroupID;
		LONG lCoaxLevel;
		LONG lRingLevel;
		LONG lSideLevel;
	} SETLIGHT;

	SETLIGHT stInfo;
	svMsg.GetMsg(sizeof(SETLIGHT), &stInfo);

	// Saturate the levels
	if (stInfo.lCoaxLevel > 100)
	{
		stInfo.lCoaxLevel = 100;
	}
	if (stInfo.lCoaxLevel < 0)
	{
		stInfo.lCoaxLevel = 0;
	}
	if (stInfo.lRingLevel > 100)
	{
		stInfo.lRingLevel = 100;
	}
	if (stInfo.lRingLevel < 0)
	{
		stInfo.lRingLevel = 0;
	}
	if (stInfo.lSideLevel > 100)
	{
		stInfo.lSideLevel = 100;
	}
	if (stInfo.lSideLevel < 0)
	{
		stInfo.lSideLevel = 0;
	}

	if (stInfo.lGroupID >= WPR_MAX_LIGHT_GROUP)
	{
		stInfo.lGroupID = 0;
	}

	GetLightingConfig(stInfo.lGroupID, emCoaxID, emRingID, emSideID);

	PR_SOURCE emBackID = PR_BACK_LIGHT;
	LONG	lGeneralBackLight = m_lWPRGenBackLightLevel[0];	// by HMI xuxuxu
	if (lGeneralBackLight > 100)
	{
		lGeneralBackLight = 100;
	}
	if (lGeneralBackLight < 0)
	{
		lGeneralBackLight = 0;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();
	if (m_bPostSealCamera)		//v3.70T2
	{
		m_lWPRGenCoaxLightLevel[0]	= stInfo.lCoaxLevel;
		m_lWPRGenRingLightLevel[0]	= stInfo.lRingLevel;
		m_lWPRGenSideLightLevel[0]	= stInfo.lSideLevel;

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA))
		{
			BOOL bWT2 = (m_unCurrPostSealID==5);
			if( bWT2==FALSE )
			{
				m_lWL1CoaxLightLevel	= m_lWPRGenCoaxLightLevel[0];
				m_lWL1RingLightLevel	= m_lWPRGenRingLightLevel[0];
				m_lWL1SideLightLevel	= m_lWPRGenSideLightLevel[0];
				m_lWL1BackLightLevel	= m_lWPRGenBackLightLevel[0];
			}
			else
			{
				m_lWL2CoaxLightLevel	= m_lWPRGenCoaxLightLevel[0];
				m_lWL2RingLightLevel	= m_lWPRGenRingLightLevel[0];
				m_lWL2SideLightLevel	= m_lWPRGenSideLightLevel[0];
				m_lWL2BackLightLevel	= m_lWPRGenBackLightLevel[0];
			}
			CString szMsg;
			szMsg.Format("contour set gen lt on, WT %d", bWT2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			SetContourLighting(TRUE, bWT2);
			SavePrData(FALSE);
		}
		else
		{
			PR_UBYTE	ubSendID	= PSPR_SENDER_ID;
			PR_UBYTE	ubRecvID	= PSPR_RECV_ID;
			PR_PURPOSE	ubGnPurpose	= MS899_POSTSEAL_GEN_PURPOSE;
			PR_PURPOSE	ubPrPurPose = MS899_POSTSEAL_PR_PURPOSE;
			switch( m_unCurrPostSealID )
			{
			case 2:
				ubGnPurpose	= MS899_POSTSEAL_BH1_GEN_PURPOSE;
				ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
				break;
			case 3:
				ubGnPurpose	= MS899_POSTSEAL_BH2_GEN_PURPOSE;
				ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
				break;
			case 6:
				//4.52D17light  SetGenLig2 (Hmi) RightUpConer
				ubGnPurpose	= MS899_POSTSEAL_BH1_GEN_PURPOSE;  
				ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
				break;
			case 7:
				ubGnPurpose	= MS899_POSTSEAL_BH2_GEN_PURPOSE;
				ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
				break;
			default:
				break;
			}

			PR_GetOptic(ubGnPurpose, ubSendID, ubRecvID, &stGnOptic, &stRpy);
			PR_GetOptic(ubPrPurPose, ubSendID, ubRecvID, &stPrOptic, &stRpy);

			PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
			PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
			PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
		
			PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
			PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
			PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		}
	}
	else
	{
		m_lWPRGenCoaxLightLevel[stInfo.lGroupID] = stInfo.lCoaxLevel;	
		m_lWPRGenRingLightLevel[stInfo.lGroupID] = stInfo.lRingLevel;	
		m_lWPRGenSideLightLevel[stInfo.lGroupID] = stInfo.lSideLevel;	
		m_lWPRGenBackLightLevel[stInfo.lGroupID] = lGeneralBackLight;	
		PR_GetOptic(ubPpsG, ubSID, ubRID, &stGnOptic, &stRpy);
		PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSID, ubRID, &stGnOptic, &stRpy);
		PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSID, ubRID, &stGnOptic, &stRpy);
		PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSID, ubRID, &stGnOptic, &stRpy);
		PR_SetLighting(emBackID, (PR_UWORD)lGeneralBackLight, ubSID, ubRID, &stGnOptic, &stRpy);
		CString szMsg;
		szMsg.Format("WPR: DP set light Coax %d Ring %d Side %d", stInfo.lCoaxLevel, stInfo.lRingLevel, stInfo.lSideLevel);
		//SetAlarmLog(szMsg);
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SetGeneralLighting2(IPC_CServiceMessage &svMsg)
{
	BOOL			bReturn = TRUE;
	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stGnOptic;
	PR_OPTIC		stPrOptic;
	PR_SOURCE		emCoaxID, emRingID, emSideID;

	if (m_bFreezeCamera_HMI)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_lWPRGenCoaxLightLevel_Backup = m_lWPRGenCoaxLightLevel[0];
	m_lWPRGenSideLightLevel_Backup = m_lWPRGenSideLightLevel[0];
	m_lWPRGenRingLightLevel_Backup = m_lWPRGenRingLightLevel[0];
	m_lWPRGenBackLightLevel_Backup = m_lWPRGenBackLightLevel[0];

	typedef struct 
	{
		LONG lGroupID;
		LONG lCoaxLevel;
		LONG lRingLevel;
		LONG lSideLevel;
		LONG lBackLevel;
	} SETLIGHT;

	SETLIGHT stInfo;
	svMsg.GetMsg(sizeof(SETLIGHT), &stInfo);

	// Saturate the levels
	if (stInfo.lCoaxLevel > 100)
	{
		stInfo.lCoaxLevel = 100;
	}
	if (stInfo.lCoaxLevel < 0)
	{
		stInfo.lCoaxLevel = 0;
	}
	if (stInfo.lRingLevel > 100)
	{
		stInfo.lRingLevel = 100;
	}
	if (stInfo.lRingLevel < 0)
	{
		stInfo.lRingLevel = 0;
	}
	if (stInfo.lSideLevel > 100)
	{
		stInfo.lSideLevel = 100;
	}
	if (stInfo.lSideLevel < 0)
	{
		stInfo.lSideLevel = 0;
	}
	if (stInfo.lBackLevel > 100)
	{
		stInfo.lBackLevel = 100;
	}
	if (stInfo.lBackLevel < 0)
	{
		stInfo.lBackLevel = 0;
	}

	if (stInfo.lGroupID >= WPR_MAX_LIGHT_GROUP)
	{
		stInfo.lGroupID = 0;
	}

	GetLightingConfig(stInfo.lGroupID, emCoaxID, emRingID, emSideID);
	PR_SOURCE emBackID = PR_BACK_LIGHT;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();
	if (m_bPostSealCamera)		//v3.70T2
	{
		m_lWPRGenCoaxLightLevel[0]	= stInfo.lCoaxLevel;
		m_lWPRGenRingLightLevel[0]	= stInfo.lRingLevel;
		m_lWPRGenSideLightLevel[0]	= stInfo.lSideLevel;
		m_lWPRGenBackLightLevel[0]	= stInfo.lBackLevel;

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA))
		{
			BOOL bWT2 = (m_unCurrPostSealID==5);
			if( bWT2==FALSE )
			{
				m_lWL1CoaxLightLevel	= m_lWPRGenCoaxLightLevel[0];
				m_lWL1RingLightLevel	= m_lWPRGenRingLightLevel[0];
				m_lWL1SideLightLevel	= m_lWPRGenSideLightLevel[0];
				m_lWL1BackLightLevel	= m_lWPRGenBackLightLevel[0];
			}
			else
			{
				m_lWL2CoaxLightLevel	= m_lWPRGenCoaxLightLevel[0];
				m_lWL2RingLightLevel	= m_lWPRGenRingLightLevel[0];
				m_lWL2SideLightLevel	= m_lWPRGenSideLightLevel[0];
				m_lWL2BackLightLevel	= m_lWPRGenBackLightLevel[0];
			}
			CString szMsg;
			szMsg.Format("contour set gen lt2 on, WT %d", bWT2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			SetContourLighting(TRUE, bWT2);
			SavePrData(FALSE);
		}
		else
		{
			PR_UBYTE	ubSendID	= PSPR_SENDER_ID;
			PR_UBYTE	ubRecvID	= PSPR_RECV_ID;
			PR_PURPOSE	ubGnPurpose	= MS899_POSTSEAL_GEN_PURPOSE;
			PR_PURPOSE	ubPrPurPose = MS899_POSTSEAL_PR_PURPOSE;
			switch( m_unCurrPostSealID )
			{
			case 2:
				ubGnPurpose	= MS899_POSTSEAL_BH1_GEN_PURPOSE;
				ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
				break;
			case 3:
				ubGnPurpose	= MS899_POSTSEAL_BH2_GEN_PURPOSE;
				ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
				break;
			case 6:
				//4.52D17light  SetGenLig2 (Hmi) RightUpConer
				ubGnPurpose	= MS899_POSTSEAL_BH1_GEN_PURPOSE;  
				ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
				break;
			case 7:
				ubGnPurpose	= MS899_POSTSEAL_BH2_GEN_PURPOSE;
				ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
				break;
			default:
				break;
			}

			PR_GetOptic(ubGnPurpose, ubSendID, ubRecvID, &stGnOptic, &stRpy);
			PR_GetOptic(ubPrPurPose, ubSendID, ubRecvID, &stPrOptic, &stRpy);

			PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
			PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
			PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
		
			PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
			PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
			PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		}
	}
	else
	{
		m_lWPRGenCoaxLightLevel[stInfo.lGroupID] = stInfo.lCoaxLevel;	
		m_lWPRGenRingLightLevel[stInfo.lGroupID] = stInfo.lRingLevel;	
		m_lWPRGenSideLightLevel[stInfo.lGroupID] = stInfo.lSideLevel;	
		m_lWPRGenBackLightLevel[stInfo.lGroupID] = stInfo.lBackLevel;	

		PR_GetOptic(ubPpsG, ubSID, ubRID, &stGnOptic, &stRpy);
		PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSID, ubRID, &stGnOptic, &stRpy);
		PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSID, ubRID, &stGnOptic, &stRpy);
		PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSID, ubRID, &stGnOptic, &stRpy);
		PR_SetLighting(emBackID, (PR_UWORD)stInfo.lBackLevel, ubSID, ubRID, &stGnOptic, &stRpy);
	}
	//CString szMsg;
	//szMsg.Format("WPR: DP set light2 Coax %d Ring %d Side %d", stInfo.lCoaxLevel, stInfo.lRingLevel, stInfo.lSideLevel);
	//SetAlarmLog(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SetExposureTime(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	PR_EXPOSURE_TIME stTime;
	LONG			ExposureTimeLevel;

	svMsg.GetMsg(sizeof(LONG), &ExposureTimeLevel);

	// Saturate the levels
	if (ExposureTimeLevel > 10)
	{
		ExposureTimeLevel = 10;
	}

	if (ExposureTimeLevel < 0)
	{
		ExposureTimeLevel = 0;
	}

	if( IsDP_ScanCam() )
		m_lSCMLrnExposureTimeLevel	= ExposureTimeLevel;
	else
		m_lWPRLrnExposureTimeLevel	= ExposureTimeLevel;

	stTime = (PR_EXPOSURE_TIME) (ExposureTimeLevel);

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();

	//4.52D17light Exposure Time
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHUplookPrFcn	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
	if (bEnableBHUplookPrFcn && (m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH1 || m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH2))
	{
		ubSID = PSPR_SENDER_ID;
		ubRID = PSPR_RECV_ID;
		if( m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH1 ) 
		{
			ubPpsI	= MS899_POSTSEAL_BH1_GEN_PURPOSE; 
			//ubPpsI = MS899_POSTSEAL_BH1_PR_PURPOSE;
		}
		else if (m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH2)
		{
			ubPpsI	= MS899_POSTSEAL_BH2_GEN_PURPOSE; 
			//ubPpsI = MS899_POSTSEAL_BH2_PR_PURPOSE;
		}
	}

	CString szErrMsg;
	if (!m_pPrGeneral->SetExposureTime(ubPpsI, stTime, ubSID, ubRID, szErrMsg))
	{
		HmiMessage(szErrMsg);
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::UpdateAction(IPC_CServiceMessage &svMsg)
{
	Sleep(500);
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::LogItems(IPC_CServiceMessage &svMsg)
{
	LONG lEventNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lEventNo);

	LogItems(lEventNo);
	
	//v3.99T2	//Bug fix to HMI hangup when setting WPR settings
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ResetPrCenterXY(IPC_CServiceMessage &svMsg)
{
	m_lPrCenterX		= PR_DEF_CENTRE_X;
	m_lPrCenterY		= PR_DEF_CENTRE_Y;
	m_lPrCenterXInPixel	= (LONG)((DOUBLE)(GetPrCenterX() - PR_DEF_CENTRE_X) / GetPrScaleFactor());	//v4.08
	m_lPrCenterYInPixel	= (LONG)((DOUBLE)(GetPrCenterY() - PR_DEF_CENTRE_Y) / GetPrScaleFactor());	//v4.08

	m_lWaferToBondPrCenterOffsetX	= 0;
	m_lWaferToBondPrCenterOffsetY	= 0;

	SavePrData(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::UpdatePrCenterXY(IPC_CServiceMessage &svMsg)
{
	m_lPrCenterX	= m_lPrCenterXInPixel * GetPrScaleFactor() + PR_DEF_CENTRE_X;
	m_lPrCenterY	= m_lPrCenterYInPixel * GetPrScaleFactor() + PR_DEF_CENTRE_Y;

	m_lWaferToBondPrCenterOffsetX	= GetPrCenterX() - PR_DEF_CENTRE_X;
	m_lWaferToBondPrCenterOffsetY	= GetPrCenterY() - PR_DEF_CENTRE_Y;

	SavePrData(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::UpdateHMIData(IPC_CServiceMessage &svMsg)
{
	BOOL	bStatus = TRUE;

	typedef struct 
	{
		BOOL bDieType;
		LONG lRefDieNo;
	} REF_TYPE;

	REF_TYPE stInfo;
	svMsg.GetMsg(sizeof(REF_TYPE), &stInfo);

	m_bEnableLrnAlignAlgo = TRUE;
	m_bSelectDieType	= stInfo.bDieType;
	m_lCurRefDieNo		= stInfo.lRefDieNo;
	if (m_lCurRefDieNo < 1)
	{
		m_lCurRefDieNo = 1;
	}
	//Update HMI Screen
	if (m_bSelectDieType == WPR_NORMAL_DIE)
	{
		if (m_lCurRefDieNo > (WPR_GEN_RDIE_OFFSET + 1))
		{
			if( m_bEnable2ndPrSearch )
			{
				if( (m_lCurRefDieNo!=GetPrescan2ndPrID()) )
				{
					m_lCurRefDieNo = 1;
				}
			}
			else
			{
				m_lCurRefDieNo = 1;
			}
		}
		m_szLrnDieType = WPR_HMI_NORM_DIE;
	}
	else
	{
		m_szLrnDieType = WPR_HMI_REF_DIE;
	}

	UpdateHmiVariable();

	CheckLearnAlignAlgorithm(svMsg);

	//andrew
	if (m_bSelectDieType==WPR_REFERENCE_DIE)		//if REF die
	{
		m_bEnableLrnAlignAlgo = FALSE;

		//v3.82 do not overwrite to 1 because sub-sequent GetHMIVariable() in  will be overwritten
		//Only overwrite for PLLM REBEL where REF-DIE alignment must use BOTH matching method
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))		//v3.80
		{
			m_lLrnAlignAlgo	= 3;	//pattern+EDGE matching
		}
	}
	else
	{
		//Need to update Srch WND when PR crosshair XY is updated (for normal die only)
		UpdateSearchDieArea(GetWprDieNo(), GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE, FALSE);	//v3.86
	}

	svMsg.InitMessage(sizeof(BOOL), &bStatus);
	SendCE_SearchParam();
	return 1;
}


LONG CWaferPr::ConfirmLearnDie(IPC_CServiceMessage &svMsg)
{
	short i = 0;
	PR_WORD siTotal = 0;	

	//convert to 4X mode
	LONG lOldZoom = GetRunZoom();
	if ((m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX4))
	{
		LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_4X, FALSE, FALSE, 9);
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHPostSeal	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS);
	BOOL bEnableBHUplookPrFcn	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
	
	CString szMzg;
	szMzg.Format("Uplook --- Confirm Learn Die bPostSeal:%d, bUplook:",bEnableBHPostSeal, bEnableBHUplookPrFcn);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMzg);

	if(bEnableBHUplookPrFcn&& (m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH1 || m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH2)) 
	{
		UCHAR ucPostSealID;
		
		if(m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH1)
		{
			ucPostSealID = 6;		//1=WL Lookup; 2=BH1 2Dlookup; 3=BH2 2DLookup; 6=BH1 lookup; 7=BH2 Lookup
		}
		else if(m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH2)
		{
			ucPostSealID = 7;
		}
		CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Start change camera (Comfim Learn Die)");
		//4.52D17learn comfirm learn die
		ChangeCamera(WPR_REFERENCE_DIE, TRUE ,ucPostSealID);
		//4.52D17light learn die
		GetPostSealLighting(ucPostSealID); 
		m_bLearnLookupCamera = TRUE;
	}
	else
	{
		ChangeCamera(WPR_CAM_WAFER);
		m_bLearnLookupCamera = FALSE;
	}


	for (i = 0; i < WPR_MAX_DIE_CORNER; i++)
	{
		siTotal += (m_stLearnDieCornerPos[i].x + m_stLearnDieCornerPos[i].y);
	}

	m_ucGenDieShape = CheckDieShape(m_ucDieShape);		//Assign HMI var (ucDieShape) into ucGenDieShape attribute

/*
	if (m_bCheckDieOrientation)
	{
		// edge matching
		if (m_bSelectDieType == WPR_NORMAL_DIE && m_lLrnAlignAlgo == 2)
		{
			HmiMessage("Edge matching not suitable for die orientation check!", "Wafer PR");
		}
	}
*/

	//Die Size not detect
	BOOL bLeanrDieSizeInvalid = FALSE;
	if ( GetDieShape() == BPR_RECTANGLE_DIE )
	{
		if ((abs(m_stLearnDieCornerPos[PR_LOWER_RIGHT].x - m_stLearnDieCornerPos[PR_UPPER_LEFT].x) < 200) || 
			(abs(m_stLearnDieCornerPos[PR_LOWER_RIGHT].y - m_stLearnDieCornerPos[PR_UPPER_LEFT].y) < 200))
		{
			bLeanrDieSizeInvalid = TRUE;
		}
	}

	//Only draw default learn die area if all learn die pos is zero
	if ((siTotal == 0) || bLeanrDieSizeInvalid)
	{
		if (GetDieShape() == WPR_RECTANGLE_DIE)
		{
			// Use 1st GOOD-DIE die size as default, if ID exists
			if (IsNormalDieLearnt() == TRUE)		//v2.91T1
			{
				LONG lHalfWidth		= GetNmlSizePixelX() / 2;
				LONG lHalfHeight	= GetNmlSizePixelY() / 2;

				m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= (PR_WORD)(GetPrCenterX() - lHalfWidth);
				m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= (PR_WORD)(GetPrCenterY() - lHalfHeight);
				m_stLearnDieCornerPos[PR_LOWER_RIGHT].x	= (PR_WORD)(GetPrCenterX() + lHalfWidth);
				m_stLearnDieCornerPos[PR_LOWER_RIGHT].y	= (PR_WORD)(GetPrCenterY() + lHalfHeight);
			}
			else
			{
				m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
				m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
				m_stLearnDieCornerPos[PR_LOWER_RIGHT].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
				m_stLearnDieCornerPos[PR_LOWER_RIGHT].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
			}
		}
		else if (GetDieShape() == WPR_TRIANGULAR_DIE)		//v4.06
		{
			m_stLearnDieCornerPos[WPR_DIE_CORNER_0].x	= (PR_WORD)GetPrCenterX();
			m_stLearnDieCornerPos[WPR_DIE_CORNER_0].y	= (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_1].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_1].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_2].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_2].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
		}
		else if( GetDieShape() == WPR_RHOMBUS_DIE )
		{
			m_stLearnDieCornerPos[WPR_DIE_CORNER_0].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_0].y	= (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_1].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_1].y	= (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_2].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_2].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_3].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_3].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
		}
		else
		{
			m_stLearnDieCornerPos[WPR_DIE_CORNER_0].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_0].y	= (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_1].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_1].y	= (PR_WORD)GetPrCenterY();
			m_stLearnDieCornerPos[WPR_DIE_CORNER_2].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_2].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_3].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_3].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_4].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_4].y	= (PR_WORD)GetPrCenterY();
			m_stLearnDieCornerPos[WPR_DIE_CORNER_5].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[WPR_DIE_CORNER_5].y	= (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
		}
	}

	m_lInspDetectWinNo = 0;
	m_lInspIgnoreWinNo = 0;
	m_lAlnDetectWinNo = 0;
	m_lAlnIgnoreWinNo = 0;

	for (i = 0; i < WPR_MAX_DETECT_WIN; i++)
	{
		m_stInspDetectWin[i].coCorner1.x = (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
		m_stInspDetectWin[i].coCorner1.y = (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
		m_stInspDetectWin[i].coCorner2.x = (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
		m_stInspDetectWin[i].coCorner2.y = (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;

		m_stInspIgnoreWin[i].coCorner1.x = (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
		m_stInspIgnoreWin[i].coCorner1.y = (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
		m_stInspIgnoreWin[i].coCorner2.x = (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
		m_stInspIgnoreWin[i].coCorner2.y = (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;

		m_stAlnDetectWin[i].coCorner1.x = (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
		m_stAlnDetectWin[i].coCorner1.y = (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
		m_stAlnDetectWin[i].coCorner2.x = (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
		m_stAlnDetectWin[i].coCorner2.y = (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;

		m_stAlnIgnoreWin[i].coCorner1.x = (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
		m_stAlnIgnoreWin[i].coCorner1.y = (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
		m_stAlnIgnoreWin[i].coCorner2.x = (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
		m_stAlnIgnoreWin[i].coCorner2.y = (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
	}



	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_COMMON_RPY stRpy;
	if(m_bLearnLookupCamera)
	{
		ubSID = PSPR_SENDER_ID;
		ubRID = PSPR_RECV_ID;
	}
	PR_EnableHwTrigLighting(ubSID, ubRID, &stRpy);
	
	if (stRpy.uwCommunStatus != PR_COMM_NOERR || stRpy.uwPRStatus != PR_ERR_NOERR)
	{
		CString csMsg;
		csMsg.Format("WPR: Enable STROKE lighting error! = 0x%x, 0x%x", stRpy.uwCommunStatus, stRpy.uwPRStatus);
		SetErrorMessage(csMsg);
		HmiMessage(csMsg);
		//return FALSE;
	}

	m_bFreezeCamera_HMI = TRUE;

	//v4.42T3
	BOOL m_bLearnColletHole = FALSE;
	BOOL bMS100EjtXY = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_ENABLE);
	if (bMS100EjtXY && (m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == 3))
	{
		//Hole with circle Pattern
		m_bLearnColletHole = TRUE;
	}
	else if ((m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4))
	{
		//Hole with Epoxy Pattern
		m_bLearnColletHole = TRUE;
	}
	//v3.34
	if (m_bPreBondAtPick || m_bLearnColletHole)
	{
#ifndef NU_MOTION
		MoveBhToPick(TRUE);
#else
		CString szSelection1 = "Arm1";
		CString szSelection2 = "Arm2";

		m_nArmSelection = HmiMessage("Please choose which arm to be learnt.", "Learn Collet Hole", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);

		if (m_nArmSelection == 1)
		{
			MoveBhToPick(TRUE);
		}
		else
		{
			MoveBhToPick_Z2(TRUE);
		}

#endif
	}
	if(m_bLearnLookupCamera) 
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("Start --- Move to pre pick");
		MoveBhToPick(FALSE);
	}

	if(IsEnableBHMark())
	{
		if ((m_bSelectDieType == WPR_REFERENCE_DIE) && ((m_lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX5) || (m_lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX7)))
		{
			BH_TnZToPick1();
		}
		else if ((m_bSelectDieType == WPR_REFERENCE_DIE) && ((m_lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX6) || (m_lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX8)))
		{
			BH_TnZToPick2();
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


VOID CWaferPr::BHMarkMoveToPrePick(const LONG bSelectDieType, const LONG lCurRefDieNo)
{
	if (IsEnableBHMark())
	{
		if ((bSelectDieType == WPR_REFERENCE_DIE) && ((lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX5) || (lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX7)))
		{
			BH_CancelToPrePick1();
		}
		else if ((bSelectDieType == WPR_REFERENCE_DIE) && ((lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX6) || (lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX8)))
		{
			BH_CancelToPrePick2();
		}
	}
}


LONG CWaferPr::CancelLearnDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	m_bFreezeCamera_HMI = FALSE;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	PR_COMMON_RPY		stRpy;
	PR_OPTIC			stOptic;
	PR_UWORD			usCoaxLightLevel;
	PR_UWORD			usRingLightLevel;
	PR_UWORD			usSideLightLevel;
	PR_SOURCE			emCoaxID, emRingID, emSideID;
	int					i;

	PR_UWORD		usBackLightLevel;
	PR_SOURCE emBackID = PR_BACK_LIGHT;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();
	//4.52D17Camera ID (Cancel learn)
	if (m_bLearnLookupCamera)
	{
		ubSID = PSPR_SENDER_ID;
		ubRID = PSPR_RECV_ID;
		CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Change to Uplook ID (Cancel learn)");
		
		//andrewng6	//Turn off uplook optics GENERAL lighting
		//1=WL Lookup; 2=BH1 2Dlookup; 3=BH2 2DLookup; 6=BH1 lookup; 7=BH2 Lookup		
		SetPostSealGenLighting(FALSE, 6);	
		SetPostSealGenLighting(FALSE, 7);	
	}

	PR_GetOptic(ubPpsG, ubSID, ubRID, &stOptic, &stRpy);
	if (CMS896AStn::m_bEnablePRDualColor == TRUE)
	{
		for (i = 0; i < WPR_MAX_LIGHT_GROUP; i++)
		{
			//Get General Lighting
			GetLightingConfig(i, emCoaxID, emRingID, emSideID);
			PR_GetLighting(&stOptic, emCoaxID, ubSID, ubRID, &usCoaxLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emRingID, ubSID, ubRID, &usRingLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emSideID, ubSID, ubRID, &usSideLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emBackID, ubSID, ubRID, &usBackLightLevel, &stRpy);

			//Restore General Ligthting
			PR_SetLighting(emCoaxID, usCoaxLightLevel, ubSID, ubRID, &stOptic, &stRpy);
			PR_SetLighting(emRingID, usRingLightLevel, ubSID, ubRID, &stOptic, &stRpy);
			PR_SetLighting(emSideID, usSideLightLevel, ubSID, ubRID, &stOptic, &stRpy);
			PR_SetLighting(emBackID, usBackLightLevel, ubSID, ubRID, &stOptic, &stRpy);
		}
	}
	else
	{
		//Get General Lighting
		GetLightingConfig(0, emCoaxID, emRingID, emSideID);
		PR_GetLighting(&stOptic, emCoaxID, ubSID, ubRID, &usCoaxLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emRingID, ubSID, ubRID, &usRingLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emSideID, ubSID, ubRID, &usSideLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emBackID, ubSID, ubRID, &usBackLightLevel, &stRpy);

		//Restore General Ligthting
		PR_SetLighting(emCoaxID, usCoaxLightLevel, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emRingID, usRingLightLevel, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emSideID, usSideLightLevel, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emBackID, usBackLightLevel, ubSID, ubRID, &stOptic, &stRpy);
	}

	if(m_bLearnLookupCamera == FALSE)//CSP006b4
	{
		PR_DisableHwTrigLighting(ubSID, ubRID, &stRpy);
	
		if (stRpy.uwCommunStatus != PR_COMM_NOERR || stRpy.uwPRStatus != PR_ERR_NOERR)
		{
			CString csMsg;
			csMsg.Format("WPR: Disable STROKE lighting error! = 0x%x, 0x%x", stRpy.uwCommunStatus, stRpy.uwPRStatus);
			SetErrorMessage(csMsg);
			HmiMessage(csMsg);
			//return FALSE;
		}
	}

	//v4.42T3
	BOOL m_bLearnColletHole = FALSE;
	BOOL bMS100EjtXY = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_ENABLE);
	if (bMS100EjtXY && (m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == 3))
	{
		m_bLearnColletHole = TRUE;
	}
	//Learn Hole with Epoxy Pattern
	else if ((m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4))
	{
		m_bLearnColletHole = TRUE;
	}


	if (IsEnableBHMark())
	{
		BHMarkMoveToPrePick(m_bSelectDieType, m_lCurRefDieNo);
	}

	//v3.34
	if (m_bPreBondAtPick || m_bLearnColletHole)
	{
#ifndef NU_MOTION
		MoveBhToPick(FALSE);
#else
		if (m_nArmSelection == 1)
		{
			MoveBhToPick(FALSE);
		}
		else
		{
			MoveBhToPick_Z2(FALSE);
		}
#endif
	}

	ChangeCamera(WPR_CAM_WAFER);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::GetLearnDieLighting(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_UWORD		usCoaxLightLevel;
	PR_UWORD		usRingLightLevel;
	PR_UWORD		usSideLightLevel;
	PR_SOURCE		emCoaxID, emRingID, emSideID;
	int				i;

	PR_SOURCE emBackID = PR_BACK_LIGHT;
	PR_UWORD		usBackLightLevel;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();

	PR_GetOptic(ubPpsI, ubSID, ubRID, &stOptic, &stRpy);
	if (CMS896AStn::m_bEnablePRDualColor == TRUE)
	{
		for (i = 0; i < WPR_MAX_LIGHT_GROUP; i++)
		{
			GetLightingConfig(i, emCoaxID, emRingID, emSideID);
			PR_GetLighting(&stOptic, emCoaxID, ubSID, ubRID, &usCoaxLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emRingID, ubSID, ubRID, &usRingLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emSideID, ubSID, ubRID, &usSideLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emBackID, ubSID, ubRID, &usBackLightLevel, &stRpy);
		}
	}
	else
	{
		GetLightingConfig(0, emCoaxID, emRingID, emSideID);
		PR_GetLighting(&stOptic, emCoaxID, ubSID, ubRID, &usCoaxLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emRingID, ubSID, ubRID, &usRingLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emSideID, ubSID, ubRID, &usSideLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emBackID, ubSID, ubRID, &usBackLightLevel, &stRpy);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::SetLearnDieLighting(IPC_CServiceMessage &svMsg)
{
	BOOL			bReturn = TRUE;
	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_SOURCE		emCoaxID, emRingID, emSideID;

	typedef struct 
	{
		LONG lGroupID;
		LONG lCoaxLevel;
		LONG lRingLevel;
		LONG lSideLevel;
	} SETLIGHT;

	SETLIGHT stInfo;
	svMsg.GetMsg(sizeof(SETLIGHT), &stInfo);

	if (stInfo.lGroupID >= WPR_MAX_LIGHT_GROUP)
	{
		stInfo.lGroupID = 0;
	}

	GetLightingConfig(stInfo.lGroupID, emCoaxID, emRingID, emSideID);
	PR_SOURCE emBackID = PR_BACK_LIGHT;
	LONG lLearnUIBackLight = 0;	// xuxuxu

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();


	//4.52D17
	BOOL bAGCUpLookCamera = FALSE; 
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHPostSeal	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS);
	BOOL bEnableBHUplookPrFcn	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
	if (bEnableBHUplookPrFcn && (m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH1 || m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH2))
	{
		bAGCUpLookCamera = TRUE;
	}

	if (!bAGCUpLookCamera)
	{
	PR_GetOptic(ubPpsI, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emBackID, (PR_UWORD)lLearnUIBackLight, ubSID, ubRID, &stOptic, &stRpy);
	}
	else
	{	
		//4.52D17
		ubSID = PSPR_SENDER_ID; 
		ubRID = PSPR_RECV_ID;
		CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Set Learn Uplook lighting"); 
		PR_GetOptic(MS899_POSTSEAL_BH1_GEN_PURPOSE, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emBackID, (PR_UWORD)lLearnUIBackLight, ubSID, ubRID, &stOptic, &stRpy);

		PR_GetOptic(MS899_POSTSEAL_BH1_PR_PURPOSE, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emBackID, (PR_UWORD)lLearnUIBackLight, ubSID, ubRID, &stOptic, &stRpy);
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::SetLearnDieLighting2(IPC_CServiceMessage &svMsg)	//	set Insp lighting to learn die.
{
	BOOL			bReturn = TRUE;
	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_SOURCE		emCoaxID, emRingID, emSideID;

	typedef struct 
	{
		LONG lGroupID;
		LONG lCoaxLevel;
		LONG lRingLevel;
		LONG lSideLevel;
		LONG lBackLevel;
	} SETLIGHT2;

	SETLIGHT2 stInfo;
	svMsg.GetMsg(sizeof(SETLIGHT2), &stInfo);

	if (stInfo.lGroupID >= WPR_MAX_LIGHT_GROUP)
	{
		stInfo.lGroupID = 0;
	}

	GetLightingConfig(stInfo.lGroupID, emCoaxID, emRingID, emSideID);
	PR_SOURCE emBackID = PR_BACK_LIGHT;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();

	//4.52D17
	BOOL bAGCUpLookCamera = FALSE; 
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHPostSeal	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS);
	BOOL bEnableBHUplookPrFcn	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
	if (bEnableBHUplookPrFcn && (m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH1 || m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH2))
	{
		bAGCUpLookCamera = TRUE;
	}

	if (!bAGCUpLookCamera)
	{
	PR_GetOptic(ubPpsI, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emBackID, (PR_UWORD)stInfo.lBackLevel, ubSID, ubRID, &stOptic, &stRpy);
	}
	else
	{	
		CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Set Learn Uplook lighting2 --- True");
		
		CString szMsg;

		ubSID = PSPR_SENDER_ID;
		ubRID = PSPR_RECV_ID;
		//4.52D17light save light value (set learn die lig2)
		m_lWPRGenCoaxLightLevel[0]	= stInfo.lCoaxLevel;  
		m_lWPRGenRingLightLevel[0]	= stInfo.lRingLevel;
		m_lWPRGenSideLightLevel[0]	= stInfo.lSideLevel;

		PR_OPTIC		stGnOptic;
		PR_OPTIC		stPrOptic;
		PR_UBYTE	ubSendID	= PSPR_SENDER_ID;
		PR_UBYTE	ubRecvID	= PSPR_RECV_ID;
		PR_PURPOSE	ubGnPurpose	= MS899_POSTSEAL_GEN_PURPOSE;
		PR_PURPOSE	ubPrPurPose = MS899_POSTSEAL_PR_PURPOSE;

		emCoaxID = PR_COAXIAL_LIGHT;
		emRingID = PR_RING_LIGHT;
		emSideID = PR_SIDE_LIGHT;

		if( m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH1 ) 
		{
			ubGnPurpose	= MS899_POSTSEAL_BH1_GEN_PURPOSE; 
			ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
		}
		else if (m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH2)
		{
			ubGnPurpose	= MS899_POSTSEAL_BH2_GEN_PURPOSE; 
			ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
		}
		else
		{
			szMsg = "Uplook --- Please check the CurRefDieNo that has to be 11 or 12";
			
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			SetErrorMessage(szMsg);
			AfxMessageBox(szMsg , MB_SYSTEMMODAL);
		}


//		PR_UWORD			usWPRCoaxLightLevel;
//		PR_UWORD			usWPRRingLightLevel;
//		PR_UWORD			usWPRSideLightLevel;

		PR_GetOptic(ubGnPurpose, ubSendID, ubRecvID, &stGnOptic, &stRpy);
		PR_GetOptic(ubPrPurPose, ubSendID, ubRecvID, &stPrOptic, &stRpy);

		//PR_GetLighting(&stGnOptic, emCoaxID, ubSendID, ubRecvID, &usWPRCoaxLightLevel, &stRpy);
		//PR_GetLighting(&stGnOptic, emRingID, ubSendID, ubRecvID, &usWPRRingLightLevel, &stRpy);
		//PR_GetLighting(&stGnOptic, emSideID, ubSendID, ubRecvID, &usWPRSideLightLevel, &stRpy);
		//szMsg.Format("Uplook --- WPR: BH1 PR get light Coax %d Ring %d Side %d", usWPRCoaxLightLevel, usWPRRingLightLevel, usWPRSideLightLevel);
		//AfxMessageBox(szMsg);
		//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
		PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
		PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
	
		//PR_GetLighting(&stPrOptic, emCoaxID, ubSendID, ubRecvID, &usWPRCoaxLightLevel, &stRpy);
		//PR_GetLighting(&stPrOptic, emRingID, ubSendID, ubRecvID, &usWPRRingLightLevel, &stRpy);
		//PR_GetLighting(&stPrOptic, emSideID, ubSendID, ubRecvID, &usWPRSideLightLevel, &stRpy);

		PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
	
		szMsg.Format("Uplook --- WPR: BH1 PR get light Coax %d Ring %d Side %d", (PR_UWORD)stInfo.lCoaxLevel, (PR_UWORD)stInfo.lRingLevel, (PR_UWORD)stInfo.lSideLevel);
		//AfxMessageBox(szMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::MovePRCursor(IPC_CServiceMessage &svMsg)
{
	PR_COORD		sttmpStartPos;
	PR_WORD			sstmpPixelStep;

	unsigned char	uctmpCornerNo;

	typedef struct 
	{
		unsigned char ucDirection;
		unsigned char ucPixelStep;
		unsigned char ucDieCorner;
	} MOVECURSOR;


	MOVECURSOR	stInfo;
	svMsg.GetMsg(sizeof(MOVECURSOR), &stInfo);

	if (GetDieShape() != WPR_RECTANGLE_DIE)
	{
		return 1;
	}

	//Get Pixel step move
	switch (stInfo.ucPixelStep)
	{
		case 1:		//10 pixel
			sstmpPixelStep = (PR_WORD)(10 * GetPrScaleFactor());
			break;

		case 2:		//30 pixel
			sstmpPixelStep = (PR_WORD)(30 * GetPrScaleFactor());
			break;
		
		default:	//1 pixel
			sstmpPixelStep = (PR_WORD)(1 * GetPrScaleFactor());
			break;
	}

	//Check current learn corner pos
	switch (stInfo.ucDieCorner)
	{
		case PR_LOWER_RIGHT:	//Lower Right		
			uctmpCornerNo = PR_LOWER_RIGHT;
			break;
		
		default:				//Upper Left
			uctmpCornerNo = PR_UPPER_LEFT;
			break;
	}
	sttmpStartPos = m_stLearnDieCornerPos[uctmpCornerNo];
	
	if (PR_NotInit())
	{
		//CString csMsg;
		//csMsg.Format("CWaferPr:: Cannot MoveCursor (%d,%d)", stInfo.ucDirection,stInfo.ucPixelStep);
		//DisplayMessage(csMsg);
		return 1;
	}

	//Remove Current Cursor
	DrawAndEraseCursor(sttmpStartPos, uctmpCornerNo, 0);

	switch (stInfo.ucDirection)
	{
		case 1:
			sttmpStartPos.x -= sstmpPixelStep;
			break;

		case 2:
			sttmpStartPos.y += sstmpPixelStep;
			break;

		case 3:
			sttmpStartPos.x += sstmpPixelStep;
			break;

		default:
			sttmpStartPos.y -= sstmpPixelStep;
			break;
	}
	m_stLearnDieCornerPos[uctmpCornerNo] = sttmpStartPos;
	DrawAndEraseCursor(sttmpStartPos, uctmpCornerNo, 1);

	return 1;
}


LONG CWaferPr::AutoLearnFFModeDie()
{
	PR_WIN	 stSrchWin;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL	 fDieRotate; 
	PR_REAL	 fDieScore;

	stSrchWin.coCorner1.x = PR_MIN_COORD + 256;
	stSrchWin.coCorner1.y = PR_MIN_COORD + 256;
	stSrchWin.coCorner2.x = PR_MAX_COORD - 256;
	stSrchWin.coCorner2.y = PR_MAX_COORD - 256;

	PR_UWORD lLearnStatus = 0;

	//Search reference die with user defined search region
	PR_WORD wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, 
							&usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchWin.coCorner1, stSrchWin.coCorner2);

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate(stDieOffset, fDieRotate);
			Sleep(25);

			LONG lWidth = m_stLearnDieCornerPos[PR_LOWER_RIGHT].x - m_stLearnDieCornerPos[PR_UPPER_LEFT].x;
			LONG lLength = m_stLearnDieCornerPos[PR_LOWER_RIGHT].y - m_stLearnDieCornerPos[PR_UPPER_LEFT].y;

			DOUBLE dRatio = 1;
			//if (m_szGenDieZoomMode[1 + WPR_GEN_NDIE_OFFSET] == "1X")
			if (m_stZoomView.m_szZoomFactor == PR_ZOOM_VALUE_1X)
			{
				dRatio = 1;
			}
			//else if (m_szGenDieZoomMode[1 + WPR_GEN_NDIE_OFFSET] == "2X")
			else if (m_stZoomView.m_szZoomFactor == PR_ZOOM_VALUE_2X)
			{
				dRatio = 0.5;
			}
			//else if (m_szGenDieZoomMode[1 + WPR_GEN_NDIE_OFFSET] == "4X")
			else if (m_stZoomView.m_szZoomFactor == PR_ZOOM_VALUE_4X)
			{
				dRatio = 0.25;
			}

			lWidth = _round(lWidth * dRatio);
			lLength = _round(lLength * dRatio);

			m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= (PR_WORD)(PR_DEF_CENTRE_X - lWidth /2);
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x = (PR_WORD)(PR_DEF_CENTRE_X + lWidth /2);
			m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= (PR_WORD)(PR_DEF_CENTRE_Y - lLength /2);
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].y = (PR_WORD)(PR_DEF_CENTRE_Y + lLength /2);

			//Move Die to center
			//convert to 4X mode
			LONG lOldZoom = GetRunZoom();
			LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_FF, FALSE, FALSE, 10);
			//Auto learn FF Mode die
			lLearnStatus =  AutoLearnDie(WPR_NORMAL_DIE, 3);
			LiveViewZoom(lOldZoom, FALSE, FALSE, 11);
		}
	}

	return lLearnStatus;
}


LONG CWaferPr::AutoLearnDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bIsCharDie = FALSE;
	BOOL bAskLearnFFModeCalibration = FALSE;
	PR_UWORD lLearnStatus = 0;		//Klocwork	//v4.46

	m_bFreezeCamera_HMI = FALSE;
	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsAOIOnlyMachine())
	{
		OpenWaitingAlert();
	}

	EnableMouseClickCallbackCmd(FALSE);

	svMsg.GetMsg(sizeof(BOOL), &bIsCharDie);
	
	//Get General Lighting
	WPR_GetGeneralLighting();
	//Get Hmi Variable
	GetHmiVariable();	// learn die

	//Erase user set position
	if (GetDieShape() == WPR_RECTANGLE_DIE)
	{
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT], PR_UPPER_LEFT, 0);
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 0);
	}
	else if (GetDieShape() == WPR_TRIANGULAR_DIE)		//v4.06
	{
		DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[0], 0);
	}
	else if (GetDieShape() == WPR_RHOMBUS_DIE)
	{
		DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[0], 0);
	}
	else
	{
		DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[4], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[4], m_stLearnDieCornerPos[5], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[5], m_stLearnDieCornerPos[0], 0);
	}

	BOOL bLearnColletHole = FALSE;
	//Learn circle
	if (m_bEnableMS100EjtXY && (m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == 3))
	{
		bLearnColletHole = TRUE;
		lLearnStatus = AutoLearnCircle(m_stLearnDieCornerPos[PR_UPPER_LEFT], m_stLearnDieCornerPos[PR_LOWER_RIGHT]) ;
	}
	//Learn Epoxy
	else if (m_bEnableMS100EjtXY && (m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4))
	{
		BOOL bStatus = AutoLearnEpoxy(m_stLearnDieCornerPos[PR_UPPER_LEFT], m_stLearnDieCornerPos[PR_LOWER_RIGHT]) ;
		lLearnStatus = bStatus ? PR_ERR_NOERR : PR_ERR_NO_DIE;
	}
	//Start Learn Die & display result
	else if ((bIsCharDie == TRUE) && (m_bSelectDieType == WPR_REFERENCE_DIE))
	{
		if (m_bEnableOCR == FALSE)
		{
			lLearnStatus = AutoLearnCharDie(m_lCurRefDieNo - WPR_GEN_CDIE_OFFSET);
		}
		else
		{
			lLearnStatus = AutoLearnOCRDie(m_lCurRefDieNo - WPR_GEN_CDIE_OFFSET);
		}
	}
	else if (CMS896AStn::m_bMS60NGPick &&		//v4.54A6
			 (m_bSelectDieType == WPR_NORMAL_DIE) && 
			 (m_lCurRefDieNo == 2))
	{

		BOOL bStatus = AutoLearnEpoxy(	m_stLearnDieCornerPos[PR_UPPER_LEFT], 
										m_stLearnDieCornerPos[PR_LOWER_RIGHT]);
		if (bStatus)
		{
			lLearnStatus = PR_ERR_NOERR;
		}
		else
		{
			lLearnStatus = PR_ERR_NO_DIE;
		}
	}
	else
	{
		lLearnStatus =  AutoLearnDie(m_bSelectDieType, m_lCurRefDieNo);

		/*if (m_bSelectDieType == WPR_NORMAL_DIE && m_lCurRefDieNo == 1)// Matthew 20181220 by Harry Ho
		{
			m_lGenLrnAlignAlgo[11] = m_lLrnAlignAlgo;
			m_lGenLrnBackgroud[11] = m_lLrnBackground;
			m_lGenLrnInspMethod[11] = m_lLrnInspMethod;
			m_lGenLrnFineSrch[11] = m_lLrnFineSrch;
			m_lGenSrchDieAreaX[11] = m_lGenSrchDieAreaX[0];
			m_lGenSrchDieAreaY[11] = m_lGenSrchDieAreaY[0];
			if (AutoLearnDie(WPR_REFERENCE_DIE, WPR_REFERENCE_PR_DIE_INDEX9) != PR_ERR_NOERR)
			{
				CString szText = "NG Grade learn die fail. Please learn again!";
				CString szTitle = "NG Grade learn die fail";
				HmiMessage(szText,szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			}

			//Move Die to center
			//convert to FF mode
			
			CString szContent = "Start to learn FF mode die?";
			CString szTitle = "Learn FF mode Die";
			if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
			{	
				if (AutoLearnFFModeDie() == PR_ERR_NOERR)
				{
					bAskLearnFFModeCalibration = TRUE;
					MultiSearchDieForLearnDieProcess(WPR_NORMAL_PR_DIE_INDEX3);
					m_szGenDieZoomMode[WPR_NORMAL_PR_DIE_INDEX3 + WPR_GEN_NDIE_OFFSET] = "FF";
				}
			}
		}*/

		//v2.82T1	//v2.91T1
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		BOOL bDLAConfig = (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["ExChgArmExist"];
		if (bDLAConfig && (pApp->GetCustomerName() == "Lumileds"))		// This feature only available for Lumileds DLA 
		{
			////Force Search Die Area into 1.8x of die size
			m_lSrchDieAreaX = 5;
			m_lSrchDieAreaY = 5;
		}
		else
		{
		}

		m_dSrchDieAreaX = GetSrchDieAreaX() * WPR_SEARCHAREA_SHIFT_FACTOR;
		m_dSrchDieAreaY = GetSrchDieAreaY() * WPR_SEARCHAREA_SHIFT_FACTOR;

		if (m_bSelectDieType == WPR_REFERENCE_DIE)
		{
			UpdateSearchDieArea(GetWprDieNo(), GetSrchDieAreaX(), GetSrchDieAreaY(), FALSE, FALSE);
		}
		else
		{
			UpdateSearchDieArea(GetWprDieNo(), GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE, FALSE);
		}
	}

	//Show learnt result	
	BOOL bDisplayLearnDieResult = DisplayLearnDieResult(lLearnStatus);
	if (!bLearnColletHole && bDisplayLearnDieResult == TRUE)
	{
		if (m_bSelectDieType == WPR_REFERENCE_DIE)
		{	
			if (CounterCheckReferenceDieWithNormalDie() == FALSE)
			{
				CString szTemp;
				szTemp.LoadString(HMB_WPR_GOOD_DIE_WITH_NORMAL_RECORD);
				HmiMessage(szTemp);
				SetStatusMessage("Warning! Good die found using normal die record");
			}
		}
	}

	if (bDisplayLearnDieResult == FALSE)
	{
		SetszGenDieZoomMode(TRUE);
		SetbGenDieCalibration(FALSE);
		UpdateWPRInfoList();
	}
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_COMMON_RPY stRpy;
	PR_DisableHwTrigLighting(ubSID, ubRID, &stRpy);
	
	if (stRpy.uwCommunStatus != PR_COMM_NOERR || stRpy.uwPRStatus != PR_ERR_NOERR)
	{
		CString csMsg;
		csMsg.Format("WPR: Disable STROKE lighting error! = 0x%x, 0x%x", stRpy.uwCommunStatus, stRpy.uwPRStatus);
		SetErrorMessage(csMsg);
		HmiMessage(csMsg);
		//return FALSE;
	}
	if(m_bLearnLookupCamera == FALSE)//CSP006b4
	{
		PR_DisableHwTrigLighting(ubSID, ubRID, &stRpy);

		if (stRpy.uwCommunStatus != PR_COMM_NOERR || stRpy.uwPRStatus != PR_ERR_NOERR)
		{
			CString csMsg;
			csMsg.Format("WPR: Disable STROKE lighting error! = 0x%x, 0x%x", stRpy.uwCommunStatus, stRpy.uwPRStatus);
			SetErrorMessage(csMsg);
			HmiMessage(csMsg);
			//return FALSE;
		}
	}
	//v4.42T3
	BOOL m_bLearnColletHole = FALSE;
	BOOL bMS100EjtXY = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_ENABLE);
	if (bMS100EjtXY && (m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == 3))
	{
		m_bLearnColletHole = TRUE;
	}
	//Learn Epoxy
	else if ((m_bSelectDieType == WPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4))
	{
		m_bLearnColletHole = TRUE;
	}

	//v3.34
	if (m_bPreBondAtPick || m_bLearnColletHole)
	{
#ifndef NU_MOTION
		MoveBhToPick(FALSE);
#else
		if (m_nArmSelection == 1)
		{
			MoveBhToPick(FALSE);
		}
		else
		{
			MoveBhToPick_Z2(FALSE);
		}
#endif
	}

	//Clear Camera & Restore General Ligthting
	ChangeCamera(WPR_CAM_WAFER);
	RestoreGeneralLighting(svMsg);
	//andrewng6		
	if (m_bLearnLookupCamera)
	{
		//Turn off uplook optics GENERAL lighting
		//1=WL Lookup; 2=BH1 2Dlookup; 3=BH2 2DLookup; 6=BH1 lookup; 7=BH2 Lookup		
		SetPostSealGenLighting(FALSE, 6);	
		SetPostSealGenLighting(FALSE, 7);	
		m_bLearnLookupCamera = FALSE;
	}
	if ( m_bSelectDieType == WPR_REFERENCE_DIE )
	{
		ChangeCamera(WPR_CAM_WAFER);
		SetWaferTableJoystick(FALSE);
		CheckRebel_RC2N_Location(TRUE);
		CheckRebel_RUL2N_Location(TRUE);
		CheckRebel_RR2N_Location(TRUE);
		ChangeCamera(WPR_CAM_WAFER);
		SetWaferTableJoystick(TRUE);
	}

	if (IsEnableBHMark())
	{
		BHMarkMoveToPrePick(m_bSelectDieType, m_lCurRefDieNo);
	}

	if (m_bSelectDieType == WPR_NORMAL_DIE)
	{
		CalculateExtraExposureGrabTime();		//v4.49A3
	}

	SavePrData(FALSE);

	LogItems(WPR_PR_LEARN_DIE_LIGHT_COX);
	LogItems(WPR_PR_LEARN_DIE_LIGHT_RING);
	LogItems(WPR_PR_LEARN_DIE_LIGHT_SIDE);
	

	// 3356
	SetGemValue("WPR_lDieType", m_bSelectDieType);
	SetGemValue("WPR_lDieID", m_lCurRefDieNo);
	SetGemValue("WPR_lDieAlignAlgorithm", m_lLrnAlignAlgo);
	SetGemValue("WPR_lDieInpsectionMethod", m_lLrnInspMethod);
	SetGemValue("WPR_lCoaxialLight", m_lWPRLrnCoaxLightHmi);
	SetGemValue("WPR_lRingLight", m_lWPRLrnRingLightHmi);
	// 3357
	SetGemValue("WPR_lNormDieLearnt", m_bDieIsLearnt);
	SetGemValue("WPR_lNormDieSearchID", m_lCurNormDieID);
	SetGemValue("WPR_lNormDieFOVSize", m_dLFSize);
	// 3358
	SetGemValue("WPR_lNormDieCalib", m_bDieCalibrated);
	SetGemValue("WPR_lNormDieSizeX", m_lCurNormDieSizeX);
	SetGemValue("WPR_lNormDieSizeY", m_lCurNormDieSizeY);
	// 3359
	SetGemValue("WPR_lNormDiePitch", m_bPitchLearnt);
	SetGemValue("WPR_lNormDiePitchX", m_lCurPitchSizeX);
	SetGemValue("WPR_lNormDiePitchY", m_lCurPitchSizeY);
	// 3360
	SetGemValue("WPR_lRefDieLearnt", m_lLrnTotalRefDie);
	// 7300
	SendEvent(SG_CEID_WP_LEARNDIE, FALSE);
	
	//Ask user to do Calibration if no learn die error
	if ((lLearnStatus == PR_WARN_TOO_DARK) || (lLearnStatus == PR_WARN_TOO_BRIGHT) || (lLearnStatus == PR_WARN_ROTATION_INVARIANT) || (lLearnStatus == PR_ERR_NOERR))
	{
		SetszGenDieZoomMode(FALSE);

		if (m_bSelectDieType == WPR_REFERENCE_DIE)
		{
			//No need to die calbration & die pitch for ref die
			if (bIsCharDie == TRUE)
			{
				CString szTemp;
				szTemp.Format("Wafer learn char reference die complete (No: %d) (Algo: %d)", m_lCurRefDieNo, m_lLrnAlignAlgo);
				SetStatusMessage(szTemp);
			}
			else
			{
				CString szTemp;
				szTemp.Format("Wafer learn reference die complete (No: %d) (Algo: %d)", m_lCurRefDieNo, m_lLrnAlignAlgo);
				SetStatusMessage(szTemp);
			}

			EnableMouseClickCallbackCmd(TRUE);
			if (IsAOIOnlyMachine())
			{
				CloseWaitingAlert();
			}

			UpdateWPRInfoList();
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		CString szTemp;
		szTemp.Format("Wafer learn normal die complete (No: %d) (Algo: %d)", m_lCurRefDieNo, m_lLrnAlignAlgo);
		SetStatusMessage(szTemp);

		BOOL bNoDieCalib = FALSE;
		if( IsEnableZoom() && m_bEnable2ndPrSearch && m_lCurRefDieNo==GetPrescan2ndPrID() )
		{
			bNoDieCalib = TRUE;
		}

		if (bNoDieCalib || m_bPreBondAtPick || m_bLearnColletHole)
		{
			EnableMouseClickCallbackCmd(TRUE);

			if (IsAOIOnlyMachine())
			{
				CloseWaitingAlert();
			}

			UpdateWPRInfoList();
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		CString szTitle, szContent;
		szTitle.LoadString(HMB_WPR_DIE_CALIBRATION);
		szContent.LoadString(HMB_WPR_START_CALIBRATION);

		BOOL bLearnDieCalibration = FALSE;
		if (m_bSelectDieType == WPR_NORMAL_DIE && (m_lCurRefDieNo == 1 || m_lCurRefDieNo == 3))
		{
			bLearnDieCalibration = TRUE;
		}

		if (bLearnDieCalibration && HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
		{
			WprLearnDieCalibration();
			SetbGenDieCalibration(TRUE);
		}
		else
		{
			SetbGenDieCalibration(FALSE);
		}

		if (bAskLearnFFModeCalibration)
		{
			szTitle = "FF mode Die calibration";
			szContent = "Start FF mode calibration?";
			if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
			{
				LONG lOldZoom = GetRunZoom();
				LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_FF, FALSE, FALSE, 12);
				WprLearnDieCalibration();
				LiveViewZoom(lOldZoom, FALSE, FALSE, 13);
				m_bGenDieCalibration[3 + WPR_GEN_NDIE_OFFSET] = TRUE;
			}
			else
			{
				m_bGenDieCalibration[3 + WPR_GEN_NDIE_OFFSET] = FALSE;
			}
		}

		if (m_bSelectDieType == WPR_NORMAL_DIE && m_lCurRefDieNo == 3)
		{
			MultiSearchDieForLearnDieProcess(m_lCurRefDieNo);
		}

		//Ask user to do die pitch
		szTitle.LoadString(HMB_WPR_DIE_PITCH);
		szContent.LoadString(HMB_WPR_START_LRN_PITCH);

		EnableMouseClickCallbackCmd(TRUE);

		BOOL bLearnPitch = TRUE;
		if( IsEnableZoom() )
		{
			if( IsDP() )
			{
				if( IsDP_ScanCam() )
				{
					bLearnPitch = FALSE;
				}
			}
			else if( GetRunZoom()!=GetNmlZoom() )
			{
				bLearnPitch = FALSE;
			}
		}
		
		if (m_bSelectDieType == WPR_NORMAL_DIE && m_lCurRefDieNo == 1)
		{
			bLearnPitch = TRUE;
		}
		else
		{
			bLearnPitch = FALSE;
		}

		if(bLearnPitch && (HmiMessageEx(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES) )
		{
			WprLearnDiePitch(TRUE);
		}
	}

	EnableMouseClickCallbackCmd(TRUE);

	if (IsAOIOnlyMachine())
	{
		CloseWaitingAlert();
	}

	UpdateWPRInfoList();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::WprAutoColorAdjust(IPC_CServiceMessage &svMsg)
{
	PR_MODIFY_COLOR_COEFF_CMD    pstCmd;
	PR_MODIFY_COLOR_COEFF_RPY     pstRpy;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();


	PR_InitModifyColorCoeffCmd(&pstCmd);
	pstCmd.emPurpose = ubPpsI;
	PR_ModifyColorCoeffCmd(&pstCmd, ubSID, ubRID, &pstRpy);

	PR_COORD	stCrossHair;
	stCrossHair.x = (PR_WORD)GetPrCenterX();
	stCrossHair.y = (PR_WORD)GetPrCenterY();
	DrawHomeCursor(stCrossHair);
	return 1;
}

LONG CWaferPr::AutoDetectDie(IPC_CServiceMessage &svMsg)
{
	if (PR_NotInit())
	{
		return 1;
	}

	//v4.42T11
	//Update normal die or Ref-die die window //PLLM
	LONG lDieNo = 0;
	if ( m_bSelectDieType == WPR_NORMAL_DIE )
		lDieNo = WPR_GEN_NDIE_OFFSET + m_lCurRefDieNo;
	else
		lDieNo = WPR_GEN_RDIE_OFFSET + m_lCurRefDieNo;

	LONG lHalfWidth		= GetDieSizePixelX(lDieNo) / 2;
	LONG lHalfHeight	= GetDieSizePixelY(lDieNo) / 2;

	if ( (IsThisDieLearnt((UCHAR)lDieNo)) && (lHalfWidth > 0) && (lHalfHeight > 0))
	{
		m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= (PR_WORD)(GetPrCenterX() - lHalfWidth);
		m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= (PR_WORD)(GetPrCenterY() - lHalfHeight);
		m_stLearnDieCornerPos[PR_LOWER_RIGHT].x = (PR_WORD)(GetPrCenterX() + lHalfWidth);
		m_stLearnDieCornerPos[PR_LOWER_RIGHT].y = (PR_WORD)(GetPrCenterY() + lHalfHeight);
	}
	else
	{
		if (IsNormalDieLearnt() == TRUE)
		{
			lHalfWidth	= GetNmlSizePixelX() / 2;
			lHalfHeight	= GetNmlSizePixelY() / 2;
			m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= (PR_WORD)(GetPrCenterX() - lHalfWidth);
			m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= (PR_WORD)(GetPrCenterY() - lHalfHeight);
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x = (PR_WORD)(GetPrCenterX() + lHalfWidth);
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].y = (PR_WORD)(GetPrCenterY() + lHalfHeight);
		}
		else
		{
			m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
		}
	}

	if (GetDieShape() == WPR_RECTANGLE_DIE)
	{
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT], PR_UPPER_LEFT, 1);
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 1);
	}
	else if (GetDieShape() == WPR_TRIANGULAR_DIE)	//v4.06
	{
		DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[0], 1);
	}
	else if (GetDieShape() == WPR_RHOMBUS_DIE)
	{
		m_stLearnDieCornerPos[0].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
		m_stLearnDieCornerPos[0].y	= (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
		m_stLearnDieCornerPos[1].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
		m_stLearnDieCornerPos[1].y	= (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
		m_stLearnDieCornerPos[2].x	= (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
		m_stLearnDieCornerPos[2].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
		m_stLearnDieCornerPos[3].x	= (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
		m_stLearnDieCornerPos[3].y	= (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;

		DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[0], 1);
	}
	else
	{
		DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[4], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[4], m_stLearnDieCornerPos[5], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[5], m_stLearnDieCornerPos[0], 1);
	}

	return 1;
}


LONG CWaferPr::AutoLearnRefDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	PR_UWORD lLearnStatus;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Get Hmi Variable
	GetHmiVariable();	// learn refer die

	//Get General Lighting
	WPR_GetGeneralLighting();

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();
	//Set Ref die Learn die lighting
	PR_OPTIC stOptic;
	PR_COMMON_RPY stRpy;
	PR_UWORD usCoaxLightLevel = 0, usRingLightLevel = 0, usSideLightLevel = 0;
	PR_UWORD usBackLightLevel = 0;
	PR_GetOptic(ubPpsI, ubSID, ubRID, &stOptic, &stRpy);
	PR_GetLighting(&stOptic, PR_COAXIAL_LIGHT,	ubSID, ubRID, &usCoaxLightLevel, &stRpy);
	PR_GetLighting(&stOptic, PR_RING_LIGHT,		ubSID, ubRID, &usRingLightLevel, &stRpy);
	PR_GetLighting(&stOptic, PR_SIDE_LIGHT,		ubSID, ubRID, &usSideLightLevel, &stRpy);
	PR_GetLighting(&stOptic, PR_BACK_LIGHT,		ubSID, ubRID, &usBackLightLevel, &stRpy);
	PR_SetLighting(PR_COAXIAL_LIGHT,	usCoaxLightLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(PR_RING_LIGHT,		usRingLightLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(PR_SIDE_LIGHT,		usSideLightLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(PR_BACK_LIGHT,		usBackLightLevel, ubSID, ubRID, &stOptic, &stRpy);

	if (AutoDetectDieCorner(WPR_REFERENCE_DIE, 1) != PR_ERR_NOERR)
	{
		HmiMessage("Error: ref die corners not found be AutoDetectDieCmd!!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Erase user set position
	DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT], PR_UPPER_LEFT, 0);
	DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 0);

	//Start Learn Die & display result
	lLearnStatus = AutoLearnDie(m_bSelectDieType, m_lCurRefDieNo);

	UpdateWPRInfoList();

	//Show learnt result 
	if (DisplayLearnDieResult(lLearnStatus) == TRUE)
	{
		if (m_bSelectDieType == WPR_REFERENCE_DIE)
		{	
			if (CounterCheckReferenceDieWithNormalDie() == FALSE)
			{
				CString szTemp;
				szTemp.LoadString(HMB_WPR_GOOD_DIE_WITH_NORMAL_RECORD);
				HmiMessage(szTemp);
				SetStatusMessage("Warning! Good die found using normal die record");
			}
		}
	}

	//Clear Camera & Restore General Ligthting
	ChangeCamera(WPR_CAM_WAFER);
	RestoreGeneralLighting(svMsg);

	GetHmiVariable();	// learn refer die
	SavePrData(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::GetRebel_R_DieSize(INT &lRDieSizeX, INT &lRDieSizeY, UCHAR ucReferPrID)
{
	if( ucReferPrID<1 || ucReferPrID>m_lLrnTotalRefDie )
		ucReferPrID = 1;
	lRDieSizeX = 0;
	lRDieSizeY = 0;
	PR_COORD		stPixel;

	UCHAR ucIndex = WPR_GEN_RDIE_OFFSET + ucReferPrID;
	
	if (ucIndex < WPR_MAX_DIE)		//Klocwork	//v4.46
	{
		stPixel.x = GetDieSizePixelX(ucIndex);
		stPixel.y = GetDieSizePixelY(ucIndex);
		ConvertPixelToMotorStep(stPixel, &lRDieSizeX, &lRDieSizeY);
		lRDieSizeX = labs(lRDieSizeX);
		lRDieSizeY = labs(lRDieSizeY);
	}
	return 1;
}

LONG CWaferPr::GetRebel_R_N_Pitch(LONG &lPitchX, LONG &lPitchY)	//	always in absolute value.
{
	int lRDieSizeX = 0, lRDieSizeY = 0;

	GetRebel_R_DieSize(lRDieSizeX, lRDieSizeY, 1);
	lPitchX = (GetDiePitchX_X() - GetDieSizeX()/2 + (lRDieSizeX)/2);
	lPitchY = (GetDiePitchY_Y() - GetDieSizeY()/2 + (lRDieSizeY)/2);

	CString szMsg;
	szMsg.Format("Rebel R size %d,%d, N pitch %d,%d, size %d,%d", 
		lRDieSizeX, lRDieSizeY, GetDiePitchX_X(), GetDiePitchY_Y(), GetDieSizeX(), GetDieSizeY());
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	return TRUE;
}

BOOL CWaferPr::CheckRebel_RC2N_Location(BOOL bAutoLearnDie)
{
	UCHAR ucPrID = 1;
//	if( PLLM_REBEL_EXTRA_RC2N==0 )
//	{
//		return TRUE;
//	}
	if( GetWprDieNo()!=(ucPrID+WPR_GEN_RDIE_OFFSET) )
	{
		return TRUE;
	}
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsPLLMRebel()==FALSE )
	{
		return TRUE;
	}
	if (m_bSearchHomeOption == WT_SPIRAL_SEARCH)
	{
		return TRUE;		//Not available for U2U, which uses spiral-Search	//v4.46T7
	}

	BOOL bReturn = TRUE;
	INT lRSizeX, lRSizeY;
	GetRebel_R_DieSize(lRSizeX, lRSizeY, ucPrID);
	double dMilX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)lRSizeX * 1000 / 25.4);
	double dMilY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)lRSizeY * 1000 / 25.4);
	double dDx = fabs(m_dRMasterDieSizeX[ucPrID-1] - dMilX);
	double dDy = fabs(m_dRMasterDieSizeY[ucPrID-1] - dMilY);
	CString szTemp;
	if (( m_dRMasterDieSizeX[ucPrID-1]> 0) && (dDx > (m_dRMasterDieSizeX[ucPrID-1] * WPR_RDIE_SIZE_TOLERANCE)))
	{
		szTemp.Format("Refer %d x-size is over 20% of master-size", ucPrID);
		SetErrorMessage(szTemp);
		bReturn = FALSE;
	}
	if ((m_dRMasterDieSizeY[ucPrID-1] > 0) && (dDy > (m_dRMasterDieSizeY[ucPrID-1] * WPR_RDIE_SIZE_TOLERANCE)))
	{
		szTemp.Format("Refer %d y-size is over 20% of master-size", ucPrID);
		SetErrorMessage(szTemp);
		bReturn = FALSE;
	}
	if( bAutoLearnDie && bReturn==FALSE )
	{
		HmiMessage(szTemp + ".\nPlease relearn the reference die again", "PLLM");
		m_bGenDieLearnt[WPR_GEN_RDIE_OFFSET + ucPrID] = FALSE;
		m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET + ucPrID] = 0;
		if( m_lLrnTotalRefDie>0 )
			m_lLrnTotalRefDie--;
		(*m_psmfSRam)["WaferPr"]["RefDie"]["Count"] = m_lLrnTotalRefDie;
		return bReturn;
	}

	HmiMessage("Begin check (refer C - normal) die location");

	GrabAndSaveImage(WPR_REFERENCE_DIE, ucPrID, WPR_GRAB_SAVE_IMG_RC2N);

	LONG lOffsetX = 0, lOffsetY = 0;
	bReturn = FALSE;
	if(	WprSearchDie(FALSE, ucPrID, lOffsetX, lOffsetY) )
	{
		LONG lOrigX, lOrigY;
		INT lEcdX = 0, lEcdY = 0, lEcdT = 0;
		GetWaferTableEncoder(&lEcdX, &lEcdY, &lEcdT);
		lOrigX = lEcdX;
		lOrigY = lEcdY;
		lEcdX += lOffsetX;
		lEcdY += lOffsetY;

		LONG lPitchX = 0, lPitchY = 0;
		GetRebel_R_N_Pitch(lPitchX, lPitchY);
		switch (m_ulCornerSearchOption)
		{
		case WL_CORNER_TL:
		default:	//TL
			lEcdX -= lPitchX;
			lEcdY -= lPitchY;
			break;

		case WL_CORNER_TR:		//TR
			lEcdX += lPitchX;
			lEcdY -= lPitchY;
			break;

		case WL_CORNER_BL:		//BL
			lEcdX -= lPitchX;
			lEcdY += lPitchY;
			break;

		case WL_CORNER_BR:		//BR
			lEcdX += lPitchX;
			lEcdY += lPitchY;
			break;
		}

		if( MoveWaferTable(lEcdX, lEcdY) )
		{
			BOOL bFindNormal = FALSE;
			Sleep(100);
			if( WprSearchDie(TRUE, 1, lOffsetX, lOffsetY) )
			{
				bFindNormal = TRUE;
			}
			else if( IsThisDieLearnt(1) && WprSearchDie(TRUE, 2, lOffsetX, lOffsetY) )
			{
				bFindNormal = TRUE;
			}
			Sleep(100);
			if( bFindNormal )
			{
				if( labs(lOffsetX)>labs(GetDieSizeX())/2 ||
					labs(lOffsetY)>labs(GetDieSizeY())/2 )
				{
					HmiMessage("Refer die learning is not good!\nPlease Learn it again.", "PLLM");
				}
				else
				{
					bReturn = TRUE;
				}
			}
			else
			{
				HmiMessage_Red_Back("Can not find normal die at Low Left", "PLLM");
			}
		}
		else
		{
			HmiMessage("Move to target normal die error.");
		}
		MoveWaferTable(lOrigX, lOrigY);
	}
	else
	{
		HmiMessage("Can not find refer die.");
	}

	if( bAutoLearnDie && bReturn==FALSE )
	{
		HmiMessage("Can not find normal die at Low Left of refer die.\nPlease relearn the reference die again", "PLLM");
		m_bGenDieLearnt[WPR_GEN_RDIE_OFFSET + ucPrID] = FALSE;
		m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET + ucPrID] = 0;
		if( m_lLrnTotalRefDie>0 )
			m_lLrnTotalRefDie--;
		(*m_psmfSRam)["WaferPr"]["RefDie"]["Count"] = m_lLrnTotalRefDie;
	}

	return bReturn;
}

BOOL CWaferPr::CheckRebel_N2RC_Location(BOOL bFullyAuto)
{
	UCHAR ucPrID = 1;
//	if( PLLM_REBEL_EXTRA_N2RC==0 )
//	{
//		return TRUE;
//	}
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsPLLMRebel()==FALSE )
	{
		return TRUE;
	}
	if (m_bSearchHomeOption == WT_SPIRAL_SEARCH)
	{
		return TRUE;		//Not available for U2U, which uses spiral-Search	//v4.46T7
	}

	GrabAndSaveImage(WPR_NORMAL_DIE, ucPrID, WPR_GRAB_SAVE_IMG_N2RC);
	CString szMsg;
	BOOL bReturn = TRUE;
	INT siOrigX, siOrigY, siOrigT;
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
	LONG lPitchX = 0, lPitchY = 0;
	GetRebel_R_N_Pitch(lPitchX, lPitchY);
	LONG lMoveX = siOrigX;
	LONG lMoveY = siOrigY;
	szMsg.Format("N2RC normal at wft %d,%d", lMoveX, lMoveY);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	switch (m_ulCornerSearchOption)
	{
	case WL_CORNER_TL:
	default:	//TL
		lMoveX += lPitchX;
		lMoveY += lPitchY;
		break;

	case WL_CORNER_TR:		//TR
		lMoveX -= lPitchX;
		lMoveY += lPitchY;
		break;

	case WL_CORNER_BL:		//BL
		lMoveX += lPitchX;
		lMoveY -= lPitchY;
		break;

	case WL_CORNER_BR:		//BR
		lMoveX -= lPitchX;
		lMoveY -= lPitchY;
		break;
	}
	MoveWaferTable(lMoveX, lMoveY);
	Sleep(100);
	LONG lOffsetX = 0, lOffsetY = 0;
	GrabAndSaveImage(WPR_NORMAL_DIE, ucPrID, WPR_GRAB_SAVE_IMG_N2RC);
	szMsg.Format("N2RC refer at wft %d,%d", lMoveX, lMoveY);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	if(	WprSearchDie(FALSE, ucPrID, lOffsetX, lOffsetY) )
	{
		Sleep(100);
		if( labs(lOffsetX)>labs(GetDieSizeX())/2 ||
			labs(lOffsetY)>labs(GetDieSizeY())/2 )
		{
			lMoveX = siOrigX + lOffsetX;
			lMoveY = siOrigY + lOffsetY;
			MoveWaferTable(lMoveX, lMoveY);
			szMsg.Format("N2RC refer offset large at wft %d,%d", lMoveX, lMoveY);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			if( CheckRebel_N2R4_Location(m_ulCornerSearchOption) )
			{
				siOrigX = lMoveX;
				siOrigY = lMoveY;
			}
			else
			{
				szMsg = "Refer die offset over half of die size.\nWafer alignment fail!";
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
				HmiMessage_Red_Back(szMsg, "PLLM");
				bReturn = FALSE;
			}
		}
	}
	else
	{
		Sleep(100);
		if( bFullyAuto )
		{
			szMsg = "Refer die missing.\nWafer alignment fail!";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			HmiMessage_Red_Back(szMsg, "PLLM");
			bReturn = FALSE;
		}
		else
		{
			szMsg = "Refer die missing.\nContinue align wafer?";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			if( HmiMessage_Red_Back(szMsg, "PLLM", glHMI_MBX_YESNO)==glHMI_NO )
			{
				bReturn = FALSE;
			}
		}
	}

	MoveWaferTable(siOrigX, siOrigY);
	Sleep(100);

	return bReturn;
}

BOOL CWaferPr::CheckRebel_N2R4_Location(ULONG ulCornerType)
{
//	if( PLLM_REBEL_EXTRA_N2R4==0 )
//	{
//		return TRUE;
//	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsPLLMRebel()==FALSE )
	{
		return TRUE;
	}
	if (m_bSearchHomeOption == WT_SPIRAL_SEARCH)
	{
		return TRUE;		//Not available for U2U, which uses spiral-Search	//v4.46T7
	}


	INT siOrigX = 0, siOrigY = 0, siOrigT = 0;
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
	INT lRSizeX = 0, lRSizeY = 0;
	UCHAR ucPrID = 2;
	if( ulCornerType==WL_CORNER_TR || ulCornerType==WL_CORNER_BR )
		ucPrID = 3;
	GetRebel_R_DieSize(lRSizeX, lRSizeY, ucPrID);

	GrabAndSaveImage(WPR_NORMAL_DIE, ucPrID, WPR_GRAB_SAVE_IMG_N2RX);

	LONG lPitchX = GetDiePitchX_X() + lRSizeX/2 - GetDieSizeX()/2;
	LONG lPitchY = lRSizeY/2 - GetDieSizeY()/2;
	LONG lMoveX = siOrigX;
	LONG lMoveY = siOrigY;
	CString szMsg;
	switch (ulCornerType)
	{
	case WL_CORNER_TL:
	default:	//TL
		lMoveX += lPitchX;
		lMoveY -= lPitchY;
		szMsg = "TL ";
		break;

	case WL_CORNER_TR:		//TR
		lMoveX -= lPitchX;
		lMoveY -= lPitchY;
		szMsg = "TR ";
		break;

	case WL_CORNER_BL:		//BL
		lMoveX += lPitchX;
		lMoveY += lPitchY;
		szMsg = "BL ";
		break;

	case WL_CORNER_BR:		//BR
		lMoveX -= lPitchX;
		lMoveY += lPitchY;
		szMsg = "BR ";
		break;
	}

	CString szLog;
	szLog.Format("N2R4 refer size %d,%d at wft %d,%d", lRSizeX, lRSizeY, lMoveX, lMoveY);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szLog);

	MoveWaferTable(lMoveX, lMoveY);
	Sleep(100);
	LONG lOffsetX = 0, lOffsetY = 0;
	GrabAndSaveImage(WPR_NORMAL_DIE, ucPrID, WPR_GRAB_SAVE_IMG_N2RX);
	BOOL bReturn = WprSearchDie(FALSE, ucPrID, lOffsetX, lOffsetY);
	if( bReturn==FALSE )
	{
		bReturn = WprSearchDie(FALSE, ucPrID, lOffsetX, lOffsetY);
	}

	if( bReturn )
	{
		if( labs(lOffsetX)>labs(GetDieSizeX()*2/3) ||
			labs(lOffsetY)>labs(GetDieSizeY()*2/3) )
		{
			szMsg += "Side Refer die offset over limit.\nWafer alignment fail!";
			HmiMessage_Red_Back(szMsg, "PLLM");
			bReturn = FALSE;
		}
	}
	else
	{
		szMsg += "Side Refer die missing.\nWafer alignment fail!";
		HmiMessage_Red_Back(szMsg, "PLLM");
		bReturn = FALSE;
	}
	Sleep(100);

	MoveWaferTable(siOrigX, siOrigY);
	Sleep(100);
	if( bReturn==FALSE )
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}

	return bReturn;
}

BOOL CWaferPr::CheckRebel_RUL2N_Location(BOOL bAutoLearnDie)
{
	UCHAR ucPrID = 2;
//	if( PLLM_REBEL_EXTRA_RUL2N==0 )
//	{
//		return TRUE;
//	}
	if( GetWprDieNo()!=(ucPrID+WPR_GEN_RDIE_OFFSET) )
	{
		return TRUE;
	}
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsPLLMRebel()==FALSE )
	{
		return TRUE;
	}
	if (m_bSearchHomeOption == WT_SPIRAL_SEARCH)
	{
		return TRUE;		//Not available for U2U, which uses spiral-Search	//v4.46T7
	}


	BOOL bReturn = TRUE;
	INT lRSizeX, lRSizeY;
	GetRebel_R_DieSize(lRSizeX, lRSizeY, ucPrID);
	double dMilX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)lRSizeX * 1000 / 25.4);
	double dMilY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)lRSizeY * 1000 / 25.4);
	double dDx = fabs(m_dRMasterDieSizeX[ucPrID-1] - dMilX);
	double dDy = fabs(m_dRMasterDieSizeY[ucPrID-1] - dMilY);
	CString szTemp;
	if (( m_dRMasterDieSizeX[ucPrID-1]> 0) && (dDx > (m_dRMasterDieSizeX[ucPrID-1] * WPR_RDIE_SIZE_TOLERANCE)))
	{
		szTemp.Format("Refer %d x-size is over 20% of master-size", ucPrID);
		SetErrorMessage(szTemp);
		bReturn = FALSE;
	}
	if ((m_dRMasterDieSizeY[ucPrID-1] > 0) && (dDy > (m_dRMasterDieSizeY[ucPrID-1] * WPR_RDIE_SIZE_TOLERANCE)))
	{
		szTemp.Format("Refer %d y-size is over 20% of master-size", ucPrID);
		SetErrorMessage(szTemp);
		bReturn = FALSE;
	}
	if( bAutoLearnDie && bReturn==FALSE )
	{
		HmiMessage(szTemp + ".\nPlease relearn the reference die again", "PLLM");
		m_bGenDieLearnt[WPR_GEN_RDIE_OFFSET + ucPrID] = FALSE;
		m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET + ucPrID] = 0;
		if( m_lLrnTotalRefDie>0 )
			m_lLrnTotalRefDie--;
		(*m_psmfSRam)["WaferPr"]["RefDie"]["Count"] = m_lLrnTotalRefDie;
		return bReturn;
	}

	HmiMessage("Begin check (refer Upper Left - normal) die location");

	GrabAndSaveImage(WPR_REFERENCE_DIE, ucPrID, WPR_GRAB_SAVE_IMG_RX2N);

	LONG lOffsetX = 0, lOffsetY = 0;
	bReturn = FALSE;
	if(	WprSearchDie(FALSE, ucPrID, lOffsetX, lOffsetY) )
	{
		LONG lOrigX, lOrigY;
		INT lEcdX = 0, lEcdY = 0, lEcdT = 0;
		GetWaferTableEncoder(&lEcdX, &lEcdY, &lEcdT);
		lOrigX = lEcdX;
		lOrigY = lEcdY;
		lEcdX += lOffsetX;
		lEcdY += lOffsetY;

		LONG lPitchX = 0, lPitchY = 0;
		int lRDieSizeX = 0, lRDieSizeY = 0;

		GetRebel_R_DieSize(lRDieSizeX, lRDieSizeY, ucPrID);
		lPitchX = (GetDiePitchX_X() - GetDieSizeX()/2 + (lRDieSizeX)/2);
		lPitchY = lRDieSizeY/2 - GetDieSizeY()/2;

		lEcdX -= lPitchX;
		lEcdY += lPitchY;

		if( MoveWaferTable(lEcdX, lEcdY) )
		{
			BOOL bFindNormal = FALSE;
			Sleep(100);
			if( WprSearchDie(TRUE, 1, lOffsetX, lOffsetY) )
			{
				bFindNormal = TRUE;
			}
			else if( IsThisDieLearnt(1) && WprSearchDie(TRUE, 2, lOffsetX, lOffsetY) )
			{
				bFindNormal = TRUE;
			}
			Sleep(100);
			if( bFindNormal )
			{
				if( labs(lOffsetX)>labs(GetDieSizeX())/2 ||
					labs(lOffsetY)>labs(GetDieSizeY())/2 )
				{
					HmiMessage("Refer die learning is not good!\nPlease Learn it again.", "PLLM");
				}
				else
				{
					bReturn = TRUE;
				}
			}
			else
			{
				HmiMessage_Red_Back("Can not find normal die at upper right", "PLLM");
			}
		}
		else
		{
			HmiMessage("Move to target normal die error.");
		}
		MoveWaferTable(lOrigX, lOrigY);
	}
	else
	{
		HmiMessage("Can not find refer die.");
	}

	if( bAutoLearnDie && bReturn==FALSE )
	{
		HmiMessage("Can not find normal die at upper right of refer die.\nPlease relearn the reference die again", "PLLM");
		m_bGenDieLearnt[WPR_GEN_RDIE_OFFSET + ucPrID] = FALSE;
		m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET + ucPrID] = 0;
		if( m_lLrnTotalRefDie>0 )
			m_lLrnTotalRefDie--;
		(*m_psmfSRam)["WaferPr"]["RefDie"]["Count"] = m_lLrnTotalRefDie;
	}

	return bReturn;
}

BOOL CWaferPr::CheckRebel_RR2N_Location(BOOL bAutoLearnDie)
{
	UCHAR ucPrID = 3;
//	if( PLLM_REBEL_EXTRA_RUR2N==0 )
//	{
//		return TRUE;
//	}
	if( GetWprDieNo()!=(ucPrID+WPR_GEN_RDIE_OFFSET) )
	{
		return TRUE;
	}
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsPLLMRebel()==FALSE )
	{
		return TRUE;
	}
	if (m_bSearchHomeOption == WT_SPIRAL_SEARCH)
	{
		return TRUE;		//Not available for U2U, which uses spiral-Search	//v4.46T7
	}


	BOOL bReturn = TRUE;
	INT lRSizeX, lRSizeY;
	GetRebel_R_DieSize(lRSizeX, lRSizeY, ucPrID);
	double dMilX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)lRSizeX * 1000 / 25.4);
	double dMilY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)lRSizeY * 1000 / 25.4);
	double dDx = fabs(m_dRMasterDieSizeX[ucPrID-1] - dMilX);
	double dDy = fabs(m_dRMasterDieSizeY[ucPrID-1] - dMilY);
	CString szTemp;
	if (( m_dRMasterDieSizeX[ucPrID-1]> 0) && (dDx > (m_dRMasterDieSizeX[ucPrID-1] * WPR_RDIE_SIZE_TOLERANCE)))
	{
		szTemp.Format("Refer %d x-size is over 20% of master-size", ucPrID);
		SetErrorMessage(szTemp);
		bReturn = FALSE;
	}
	if ((m_dRMasterDieSizeY[ucPrID-1] > 0) && (dDy > (m_dRMasterDieSizeY[ucPrID-1] * WPR_RDIE_SIZE_TOLERANCE)))
	{
		szTemp.Format("Refer %d y-size is over 20% of master-size", ucPrID);
		SetErrorMessage(szTemp);
		bReturn = FALSE;
	}
	if( bAutoLearnDie && bReturn==FALSE )
	{
		HmiMessage(szTemp + ".\nPlease relearn the reference die again", "PLLM");
		m_bGenDieLearnt[WPR_GEN_RDIE_OFFSET + ucPrID] = FALSE;
		m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET + ucPrID] = 0;
		if( m_lLrnTotalRefDie>0 )
			m_lLrnTotalRefDie--;
		(*m_psmfSRam)["WaferPr"]["RefDie"]["Count"] = m_lLrnTotalRefDie;
		return bReturn;
	}

	HmiMessage("Begin check (refer Upper Right - normal) die location");

	GrabAndSaveImage(WPR_REFERENCE_DIE, ucPrID, WPR_GRAB_SAVE_IMG_RX2N);

	LONG lOffsetX = 0, lOffsetY = 0;
	bReturn = FALSE;
	if(	WprSearchDie(FALSE, ucPrID, lOffsetX, lOffsetY) )
	{
		LONG lOrigX, lOrigY;
		INT lEcdX = 0, lEcdY = 0, lEcdT = 0;
		GetWaferTableEncoder(&lEcdX, &lEcdY, &lEcdT);
		lOrigX = lEcdX;
		lOrigY = lEcdY;
		lEcdX += lOffsetX;
		lEcdY += lOffsetY;

		LONG lPitchX = 0, lPitchY = 0;
		int lRDieSizeX = 0, lRDieSizeY = 0;

		GetRebel_R_DieSize(lRDieSizeX, lRDieSizeY, ucPrID);
		lPitchX = (GetDiePitchX_X() - GetDieSizeX()/2 + (lRDieSizeX)/2);
		lPitchY = lRDieSizeY/2 - GetDieSizeY()/2;

		lEcdX += lPitchX;
		lEcdY += lPitchY;

		if( MoveWaferTable(lEcdX, lEcdY) )
		{
			BOOL bFindNormal = FALSE;
			Sleep(100);
			if( WprSearchDie(TRUE, 1, lOffsetX, lOffsetY) )
			{
				bFindNormal = TRUE;
			}
			else if( IsThisDieLearnt(1) && WprSearchDie(TRUE, 2, lOffsetX, lOffsetY) )
			{
				bFindNormal = TRUE;
			}
			Sleep(100);
			if( bFindNormal )
			{
				if( labs(lOffsetX)>labs(GetDieSizeX())/2 ||
					labs(lOffsetY)>labs(GetDieSizeY())/2 )
				{
					HmiMessage("Refer die learning is not good!\nPlease Learn it again.", "PLLM");
				}
				else
				{
					bReturn = TRUE;
				}
			}
			else
			{
				HmiMessage_Red_Back("Can not find normal die at upper Left", "PLLM");
			}
		}
		else
		{
			HmiMessage("Move to target normal die error.");
		}
		MoveWaferTable(lOrigX, lOrigY);
	}
	else
	{
		HmiMessage("Can not find refer die.");
	}

	if( bAutoLearnDie && bReturn==FALSE )
	{
		HmiMessage("Can not find normal die at upper Left of refer die.\nPlease relearn the reference die again", "PLLM");
		m_bGenDieLearnt[WPR_GEN_RDIE_OFFSET + ucPrID] = FALSE;
		m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET + ucPrID] = 0;
		if( m_lLrnTotalRefDie>0 )
			m_lLrnTotalRefDie--;
		(*m_psmfSRam)["WaferPr"]["RefDie"]["Count"] = m_lLrnTotalRefDie;
	}

	return bReturn;
}


LONG CWaferPr::CheckRebel_R2N_Cmd(IPC_CServiceMessage &svMsg)	// can not be called in wafer table station
{
	if( m_bSelectDieType == WPR_REFERENCE_DIE )
	{
		LONG lSearchDieNo;
		
		svMsg.GetMsg(sizeof(LONG), &lSearchDieNo);
		ChangeCamera(WPR_CAM_WAFER);
		SetWaferTableJoystick(FALSE);

		switch( lSearchDieNo )
		{
		case 1:
			CheckRebel_RC2N_Location(TRUE);
			break;
		case 2:
			CheckRebel_RUL2N_Location(TRUE);
			break;
		case 3:
			CheckRebel_RR2N_Location(TRUE);
			break;
		}
		ChangeCamera(WPR_CAM_WAFER);
		SetWaferTableJoystick(TRUE);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);

	return 1;
}

LONG CWaferPr::CheckRebel_N2R_Cmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = CheckRebel_N2RC_Location(FALSE);

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);

	return 1;
}

LONG CWaferPr::GetReferDieSize(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG	x;
		LONG	y;
	} RDIE_SIZE;

	RDIE_SIZE stDieSize;

	UCHAR ucPrID = 1;
	svMsg.GetMsg(sizeof(UCHAR), &ucPrID);

	INT lDieSizeX = 0, lDieSizeY = 0;
	GetRebel_R_DieSize(lDieSizeX, lDieSizeY, ucPrID);
	stDieSize.x = lDieSizeX;
	stDieSize.y = lDieSizeY;

	svMsg.InitMessage(sizeof(RDIE_SIZE), &stDieSize);

	return 1;
}

LONG CWaferPr::EnableLookForwardCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bEnable;
	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	m_bSrchEnableLookForward = bEnable;
	SavePrData(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;	
}

LONG CWaferPr::ConfirmSearchDiePara(IPC_CServiceMessage &svMsg)
{
	GetHmiVariable();	// search die param.

	SavePrData(FALSE);

	SendCE_SearchParam();

	// 3381
	SetGemValue("WPR_AutoBondScrnMode", m_lAutoScreenMode);
	SetGemValue("WPR_ABForeground", m_lAutoScreenFG);
	// 3382
	SetGemValue("AB_MaxSkipTotal", m_ulMaxSkipCount);
	SetGemValue("AB_MaxSkipNoDie", m_ulMaxNoDieSkipCount);
	SetGemValue("AB_MaxSkipPRSkip", m_ulMaxPRSkipCount);
	SetGemValue("AB_MaxSkipBadCut", m_ulMaxBadCutSkipCount);
	// 7303
	SendEvent(SG_CEID_WP_AUTOBOND, FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::ConfirmSearchDieArea(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		DOUBLE lSrchAreaX;
		DOUBLE lSrchAreaY;
	} SRCH_AREA;
	SRCH_AREA	stInfo;
	svMsg.GetMsg(sizeof(SRCH_AREA), &stInfo);

	m_lSrchDieAreaX = stInfo.lSrchAreaX;
	m_lSrchDieAreaY = stInfo.lSrchAreaY;
	m_dSrchDieAreaX = GetSrchDieAreaX() * WPR_SEARCHAREA_SHIFT_FACTOR;
	m_dSrchDieAreaY = GetSrchDieAreaY() * WPR_SEARCHAREA_SHIFT_FACTOR;

	CString szLog;

	if (m_bSelectDieType == WPR_REFERENCE_DIE)
	{
		UpdateSearchDieArea(GetWprDieNo(), GetSrchDieAreaX(), GetSrchDieAreaY(), FALSE);
		
		//v4.46T7	//PLLM REBEL
		szLog.Format("REF Srch Wnd #%d updated: (%ld, %ld) ", GetWprDieNo(),
						GetSrchDieAreaX(), GetSrchDieAreaY());
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}
	else
	{
		UpdateSearchDieArea(GetWprDieNo(), GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE);
		
		//v4.46T7	//PLLM REBEL
		szLog.Format("NML Srch Wnd #%d updated: (%ld, %ld) ", GetWprDieNo(),
						GetSrchDieAreaX(), GetSrchDieAreaY());
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}


	GetHmiVariable();	// confirm search die area
	SavePrData(FALSE);

	SendCE_SearchParam();
	return 1;
}

LONG CWaferPr::ConfirmSearchDieAreaNew(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	if ( m_dSrchDieAreaX < 1 || m_dSrchDieAreaX > 6 || m_dSrchDieAreaY > 6 ||m_dSrchDieAreaX < 1)
	{
		HmiMessage("Over Limit (1-6)");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;

	}
	m_lSrchDieAreaX = m_dSrchDieAreaX * 2;
	m_lSrchDieAreaY = m_dSrchDieAreaY * 2;

	CString szLog;
	//szLog.Format("%.2f,%.2f,d:%.2f,%.2f,",m_lSrchDieAreaX,m_lSrchDieAreaY,m_dSrchDieAreaX,m_dSrchDieAreaY);
	//HmiMessage(szLog);
//
	if (m_bSelectDieType == WPR_REFERENCE_DIE)
	{
		UpdateSearchDieArea(GetWprDieNo(), GetSrchDieAreaX(), GetSrchDieAreaY(), FALSE);
		
		//v4.46T7	//PLLM REBEL
		szLog.Format("New REF Srch Wnd #%d updated: (%.2f, %.2f) ", GetWprDieNo(),
						GetSrchDieAreaX(), GetSrchDieAreaY());
		//HmiMessage(szLog);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}
	else
	{
		UpdateSearchDieArea(GetWprDieNo(), GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE);
		
		//v4.46T7	//PLLM REBEL
		szLog.Format("New NML Srch Wnd #%d updated: (%.2f, %.2f) ", GetWprDieNo(),
						GetSrchDieAreaX(), GetSrchDieAreaY());
		//HmiMessage(szLog);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}


	GetHmiVariable();	// confirm search die area
	SavePrData(FALSE);

	SendCE_SearchParam();
	return 1;
}


LONG CWaferPr::UserSearchDie(IPC_CServiceMessage &svMsg)
{
	PR_UWORD usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate = 0; 
	PR_REAL fDieScore = 0;
	BOOL bReturn = TRUE;


	if (PR_NotInit())
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsNormalDieLearnt() == FALSE)		//v4.48A8
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bDieCalibrated == FALSE)			//v4.48A8
	{
		SetAlert(IDS_WPR_DIENOTCALIBRATED);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

// prescan relative code	B
	if (IsPrescanning())
	{
		m_pPrescanPrCtrl->ResumePR();
		GetImageNumInGallery();
	}	// Exit if there are still image in gallery
// prescan relative code	E

	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	LONG lDieNo = WPR_NORMAL_DIE;

	//Change the zoom mode before searching die
	LONG lOldZoom = GetRunZoom();
	ChangePrRecordZoomMode(lDieNo);

	UpdateSearchDieArea(lDieNo, GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE, FALSE);

	//v4.47A10
/*
	if (m_bPreBondAtPick)
	{
		CString szSelection1 = "Record 1";
		CString szSelection2 = "Record 2";

		m_nArmSelection = HmiMessage("Please choose which Wafer PR record to be searched:", "Learn Collet Hole", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);

		if ( m_nArmSelection == 2 )
		{
			lDieNo = WPR_NORMAL_DIE + 2;
			HmiMessage("Use WPR Normal Die Record #2 ....");
		}
	}
*/
	//andrewng //2020-0611
	BOOL bUseHWTriggerMode = FALSE;
	if (m_bUseHWTrigger)
	{
		CString szSelection1 = "NORMAL Mode";
		CString szSelection2 = "HW-Trigger Mode";
		m_nArmSelection = HmiMessage("Please choose which mode to search a die.", "F1 Search Die (Bond PR)", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);
		if ( m_nArmSelection == 2 )
		{
			bUseHWTriggerMode = TRUE;
		}
	}

	DrawSearchBox(PR_COLOR_GREEN);
	PR_COORD stCorner1, stCorner2;
	stCorner1.x = (PR_WORD) GetPrCenterX()-GetNmlSizePixelX()/2;
	stCorner1.y = (PR_WORD) GetPrCenterY()-GetNmlSizePixelY()/2;
	stCorner2.x = (PR_WORD) GetPrCenterX()+GetNmlSizePixelX()/2;
	stCorner2.y = (PR_WORD) GetPrCenterY()+GetNmlSizePixelY()/2;
	DrawRectangleBox(stCorner1, stCorner2, PR_COLOR_GREEN);
	
	PR_WORD rtnPR = 0;

	//andrewng //2020-0611
	if (bUseHWTriggerMode)
	{
		rtnPR = ManualSearchDie_HWTri(	WPR_NORMAL_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2, TRUE);	
	}
	else
	{
		rtnPR = ManualSearchDie(		WPR_NORMAL_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2, TRUE);	
	}

	if (rtnPR != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			bReturn = TRUE;
			
			//Allow die to rotate
			ManualDieCompenate(stDieOffset, fDieRotate);
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			// If video test enabled, not show the die size rectangle
			
			if (m_bDebugVideoTest == FALSE)
			{
				DrawRectangleDieSize(PR_COLOR_GREEN);
				if (m_bBadCutDetection)
				{
					DrawRectangleBox(m_stBadCutPoint[0], m_stBadCutPoint[1], PR_COLOR_BLUE);
				}
			}
		}
		else
		{
			bReturn = TRUE;		//v3.98T3	//Need to set to TRUE again for BURNIN SetStartPt fcn; use another fcn for auto-Learn Ej fcn
			//bReturn = FALSE;	//v3.98T1
		}

		//Display Message on HMI
		DisplaySearchDieResult(usDieType, WPR_NORMAL_DIE, WPR_NORMAL_DIE, fDieRotate, stDieOffset, fDieScore);
	}
	else
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		bReturn = FALSE;
	}

	//On Joystick & Clear PR screen & Change to the original zoom mode
	SetWaferTableJoystick(TRUE);
	ChangeCamera(WPR_CAM_WAFER);
	LiveViewZoom(lOldZoom, FALSE, FALSE, 14);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::UserSearchDie_CheckNoDie(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);


	DrawSearchBox(PR_COLOR_GREEN);
	PR_WORD rtnPR;
	
	rtnPR = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
	
	if (rtnPR != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			bReturn = TRUE;
			
			//Allow die to rotate
			ManualDieCompenate(stDieOffset, fDieRotate);
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			// If video test enabled, not show the die size rectangle
			
			if (m_bDebugVideoTest == FALSE)
			{
				DrawRectangleDieSize(PR_COLOR_GREEN);
				if (m_bBadCutDetection)
				{
					DrawRectangleBox(m_stBadCutPoint[0], m_stBadCutPoint[1], PR_COLOR_BLUE);
				}
			}
		}
		else
		{
			bReturn = FALSE;	//** different from UserSearchDie() here **//
			SetAlert_Red_Back(IDS_WPR_NODIEONCENTER);	//v4.55A8
		}
	}
	else
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);

		bReturn = FALSE;
	}

	//On Joystick & Clear PR screen
	SetWaferTableJoystick(TRUE);
	ChangeCamera(WPR_CAM_WAFER);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::UserSearchDie_Rotate(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	LONG lBLLevel = m_lBackLightZStatus;
	MoveBackLightWithTableCheck(TRUE, TRUE);

	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	DrawSearchBox(PR_COLOR_GREEN);

	PR_WORD wResult = 0;
	int iTotal = 1;
for(int i=0; i<iTotal; i++)
{
	wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2, TRUE);

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			//Allow die to rotate
			ManualDieCompenate_Rotate(stDieOffset, fDieRotate);
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			DrawRectangleDieSize(PR_COLOR_GREEN);
		}

		//Display Message on HMI
		if( (i+1)==iTotal )
			DisplaySearchDieResult(usDieType, WPR_NORMAL_DIE, WPR_NORMAL_DIE, fDieRotate, stDieOffset, fDieScore);
		bReturn = TRUE;
	}
	else
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		bReturn = FALSE;
		break;
	}
}

	if (lBLLevel != 2)
	{
		MoveBackLightWithTableCheck(FALSE);
	}

	//On Joystick & Clear PR screen
	SetWaferTableJoystick(TRUE);
	ChangeCamera(WPR_CAM_WAFER);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::UserSearchDie_NoMove(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		BOOL	bStatus;
		double	dOffsetX;
		double	dOffsetY;
		DOUBLE	dAngle;
	} SRCH_RESULT;

	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	SRCH_RESULT		stResult;

	stResult.bStatus = FALSE;
	stResult.dOffsetX = 0;
	stResult.dOffsetY = 0;
	stResult.dAngle = 0;
	if (PR_NotInit())
	{
		stResult.bStatus = TRUE;
	}
	else
	{
		PR_WORD wResult = 0;
		wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
		if (wResult != -1)
		{
			if (DieIsAlignable(usDieType) == TRUE)
			{
				stResult.bStatus = TRUE;
				int nOffsetX = 0;
				int nOffsetY = 0;
				CalculateDieCompenate(stDieOffset, &nOffsetX, &nOffsetY);
				CalculateNDieOffsetXY(nOffsetX, nOffsetY);		//v3.77
				stResult.dOffsetX = nOffsetX;
				stResult.dOffsetY = nOffsetY;
				stResult.dAngle = (DOUBLE) fDieRotate;
			}
			else
			{
				stResult.bStatus = FALSE;
			}
		}
		else
		{
			stResult.bStatus = FALSE;
		}
	}
	svMsg.InitMessage(sizeof(SRCH_RESULT), &stResult);

	return 1;
}

LONG CWaferPr::UserSearchNormalDie_NoMove(IPC_CServiceMessage &svMsg)
{

	PR_COORD coCorner1,coCorner2;
	coCorner1 = GetSrchArea().coCorner1;
	coCorner2 = GetSrchArea().coCorner2;
	LONG lNorNo = 0;

	svMsg.GetMsg(sizeof(ULONG), &lNorNo);
	typedef struct 
	{
		BOOL	bStatus;
		double	dOffsetX;
		double	dOffsetY;
		DOUBLE	dAngle;
	} SRCH_RESULT;
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	SRCH_RESULT		stResult;

	stResult.bStatus = FALSE;
	stResult.dOffsetX = 0;
	stResult.dOffsetY = 0;
	stResult.dAngle = 0;
	if (PR_NotInit())
	{
		stResult.bStatus = TRUE;
	}
	else
	{
		PR_WORD wResult = 0;
		wResult = ManualSearchDie(WPR_NORMAL_DIE, lNorNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
		if (wResult != -1)
		{
			if (DieIsAlignable(usDieType) == TRUE)
			{
				stResult.bStatus = TRUE;
				int nOffsetX = 0;
				int nOffsetY = 0;
				CalculateDieCompenate(stDieOffset, &nOffsetX, &nOffsetY);
				CalculateNDieOffsetXY(nOffsetX, nOffsetY);		//v3.77
				stResult.dOffsetX = nOffsetX;
				stResult.dOffsetY = nOffsetY;
				stResult.dAngle = (DOUBLE) fDieRotate;
			}
			else
			{
				stResult.bStatus = FALSE;
			}
		}
		else
		{
			stResult.bStatus = FALSE;
		}
	}
	svMsg.InitMessage(sizeof(SRCH_RESULT), &stResult);

	return 1;
}


BOOL CWaferPr::UserSearchMark(const CString szTitle, const LONG lLoopTest, const LONG lArmNo, const LONG lDieNo, PR_COORD &stDieOffset, PR_REAL &fDieRotate, LONG &lBHMarkRefX, LONG &lBHMarkRefY)
{
	BOOL			bStatus = FALSE;
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_REAL			fDieScore;

	PR_WORD wResult = 0;
	PR_COORD coCorner1temp = {PR_MAX_WIN_ULC_X, PR_MAX_WIN_ULC_Y};
	PR_COORD coCorner2temp = {PR_MAX_WIN_LRC_X, PR_MAX_WIN_LRC_Y};

	wResult = ManualSearchDie(WPR_REFERENCE_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, coCorner1temp, coCorner2temp);
	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			bStatus = TRUE;

			CString szMsg;

			szMsg.Format("%s: ref %d, %d, %d, %f, Arm%d", (const char*)szTitle, lDieNo, stDieOffset.x,stDieOffset.y, (DOUBLE) fDieRotate, lArmNo);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			
			lBHMarkRefX = stDieOffset.x;
			lBHMarkRefY = stDieOffset.y;

			if (lLoopTest != 1)
			{
				HmiMessage(szMsg);
			}
		}
		else
		{
			bStatus = FALSE;
			HmiMessage("search Fails");
		}
	}

	return bStatus;
}

LONG CWaferPr::UserSearchMark(IPC_CServiceMessage &svMsg, 
							  const CString szTitle, const LONG lArmNo, const LONG lDieNo,
							  LONG &lBHColletHoleRefX, LONG &lBHColletHoleRefY,
							  LONG &lBHMarkRefX, LONG &lBHMarkRefY)
{
	LONG lLoopTest;
	svMsg.GetMsg(sizeof(LONG), &lLoopTest);

	LONG lOldZoom = GetRunZoom();
	LiveViewZoom(0, FALSE, FALSE, 15);
	typedef struct 
	{
		BOOL	bStatus;
		double	dOffsetX;
		double	dOffsetY;
		DOUBLE	dAngle;
	} SRCH_RESULT;

	SRCH_RESULT		stResult;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 

	CMS896AApp			*pApp = (CMS896AApp*) AfxGetApp();
	BOOL bLogColletHoleData = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_ENABLE_LOG_COLLET_HOLE_DATA);
	if (bLogColletHoleData && (lLoopTest != 1))
	{
		UserSearchMark("BHColletHole", lLoopTest, lArmNo, lDieNo + 2, stDieOffset, fDieRotate, lBHColletHoleRefX, lBHColletHoleRefY);
	}
	stResult.bStatus = UserSearchMark(szTitle, lLoopTest, lArmNo, lDieNo, stDieOffset, fDieRotate, lBHMarkRefX, lBHMarkRefY);


	if (stResult.bStatus)
	{
		stResult.dOffsetX = (DOUBLE)stDieOffset.x;
		stResult.dOffsetY = (DOUBLE)stDieOffset.y;
		stResult.dAngle = (DOUBLE) fDieRotate;

		SavePrData();
	}

	LiveViewZoom(lOldZoom, FALSE, FALSE, 16);
	BOOL bReturn = TRUE;

	if (lLoopTest != 1)
	{
//		if (HmiMessage_Red_Back("Bond Arm Move to Prepick", "System", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER) == glHMI_YES)
//		{
		BHMarkMoveToPrePick(WPR_REFERENCE_DIE, lDieNo);
//		}

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
	}
	else
	{
		svMsg.InitMessage(sizeof(SRCH_RESULT), &stResult);
	}
	return 1;
}


LONG CWaferPr::UserSearchMark1(IPC_CServiceMessage &svMsg)
{
	return UserSearchMark(svMsg, "BHMark", 1, WPR_REFERENCE_PR_DIE_INDEX5, m_lBHColletHoleRef1X, m_lBHColletHoleRef1Y, m_lBHMarkRef1X, m_lBHMarkRef1Y);
}

LONG CWaferPr::UserSearchMark2(IPC_CServiceMessage &svMsg)
{
	return UserSearchMark(svMsg, "BHMark", 2, WPR_REFERENCE_PR_DIE_INDEX6, m_lBHColletHoleRef2X, m_lBHColletHoleRef2Y, m_lBHMarkRef2X, m_lBHMarkRef2Y);
}

LONG CWaferPr::UserSearchColletHole1(IPC_CServiceMessage &svMsg)
{
	return UserSearchMark(svMsg, "BHColletHole", 1, WPR_REFERENCE_PR_DIE_INDEX7, m_lBHColletHoleRef1X, m_lBHColletHoleRef1Y, m_lBHColletHoleRef1X, m_lBHColletHoleRef1Y);
}

LONG CWaferPr::UserSearchColletHole2(IPC_CServiceMessage &svMsg)
{
	return UserSearchMark(svMsg, "BHColletHole", 2, WPR_REFERENCE_PR_DIE_INDEX8, m_lBHColletHoleRef2X, m_lBHColletHoleRef2Y, m_lBHColletHoleRef2X, m_lBHColletHoleRef2Y);
}


LONG CWaferPr::UserSearchRefDie_NoMove(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		BOOL	bStatus;
		double	dOffsetX;
		double	dOffsetY;
		DOUBLE	dAngle;
	} SRCH_RESULT;
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	SRCH_RESULT		stResult;

	stResult.dOffsetX = 0;
	stResult.dOffsetY = 0;
	stResult.dAngle = 0;

	//Get the Search Die Area Region for Reference Die from the array
	PR_WIN stSrchArea;
	//GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET );

	if (PR_NotInit())
	{
		stResult.bStatus = FALSE;
	}
	else
	{
		//v4.08		//Use all available ref-die pattern for manual wafer alignment instead
		//search all learnt reference die
		for (int i = 0; i < m_lLrnTotalRefDie; i++)
		{
			GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET + i);

			if (ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE + i, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2) != -1)
			{	
				//bFindRefDie = TRUE;
				if (DieIsAlignable(usDieType) == TRUE)
				{
					stResult.bStatus = TRUE;
					int nOffsetX = 0;
					int nOffsetY = 0;
					CalculateDieCompenate(stDieOffset, &nOffsetX, &nOffsetY);
					stResult.dOffsetX = nOffsetX;
					stResult.dOffsetX = nOffsetY;
					stResult.dAngle = (DOUBLE) fDieRotate;
					break;
				}
				else
				{
					//v4.11T3
					stResult.bStatus = FALSE;
					//if ( i == (m_lLrnTotalRefDie - 1) )		//the last ref die record
					//{
					//	stResult.bStatus = FALSE;
					//}
				}
			}
		}
/*
		if (ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2) != -1)
		{
			if (DieIsAlignable(usDieType) == TRUE)
			{
				stResult.bStatus = TRUE;
				CalculateDieCompenate(stDieOffset, &stResult.nOffsetX, &stResult.nOffsetY);
				stResult.dAngle = (DOUBLE) fDieRotate;
			}
			else
			{
				stResult.bStatus = FALSE;
			}
		}
		else
		{
			stResult.bStatus = FALSE;
		}
*/
	}

	svMsg.InitMessage(sizeof(SRCH_RESULT), &stResult);
	return 1;
}


LONG CWaferPr::UserSearchRefDie(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	//Get the Search Die Area Region for Reference Die from the array
	PR_WIN stSrchArea;
	GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET);
	DrawRectangleBox(stSrchArea.coCorner1, stSrchArea.coCorner2, PR_COLOR_GREEN);

	//v4.46T7	//PLLM REBEL
	CString szLog;
	szLog.Format("UserSearchRefDie #%d SrchWnd: UL(%ld, %ld) LR(%ld, %ld)", WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET,
					stSrchArea.coCorner1.x, stSrchArea.coCorner1.y,
					stSrchArea.coCorner2.x, stSrchArea.coCorner2.y);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	PR_BOOLEAN bInsp = PR_TRUE;
	if (m_bAOINgPickPartDie)
	{
		bInsp = PR_FALSE;
	}


	//v4.49A10
	LONG lDieNo = WPR_REFERENCE_DIE;
	if (m_bPreBondAtPick)
	{
		CString szSelection1 = "Record 1";
		CString szSelection2 = "Record 2";

		m_nArmSelection = HmiMessage("Please choose which Wafer PR record to be searched:", "Learn Collet Hole", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);
		
		stSrchArea.coCorner1.x = GetPRWinULX();
		stSrchArea.coCorner1.y = GetPRWinULY();
		stSrchArea.coCorner2.x = GetPRWinLRX();
		stSrchArea.coCorner2.y = GetPRWinLRY();

		if ( m_nArmSelection == 2 )
		{
			lDieNo = WPR_REFERENCE_DIE + 1;
			HmiMessage("Use WPR Ref Die Record #2 ....");
		}
	}


	if (ManualSearchDie(WPR_REFERENCE_DIE, lDieNo, PR_TRUE, PR_TRUE, bInsp, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2, TRUE) != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate(stDieOffset, fDieRotate);
		}

		//Display Message on HMI
		DisplaySearchDieResult(usDieType, WPR_REFERENCE_DIE, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET, fDieRotate, stDieOffset, fDieScore);
		bReturn = TRUE;
	}
	else
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		bReturn = FALSE;
	}


	//On Joystick & Clear PR screen
	SetWaferTableJoystick(TRUE);
	ChangeCamera(WPR_CAM_WAFER);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::UserSearchAllRefDie(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	PR_WIN stSrchArea;
	GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET);
	DrawRectangleBox(stSrchArea.coCorner1, stSrchArea.coCorner2, PR_COLOR_GREEN);

	BOOL bFindRefDie = FALSE;	
	//search all learnt reference die
	for (int i = 0; i < m_lLrnTotalRefDie; i++)
	{
		//v4.43T11
		if (m_bEnableMS100EjtXY && (i == 1))	// Ej Cap pattern
		{
			continue;
		}
		if (m_bEnableMS100EjtXY && (i == 2))	// Collet hole
		{
			continue;
		}

		GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET + i);

		//v4.46T7	//PLLM REBEL
		CString szLog;
		szLog.Format("UserSearchAllRefDie #%d SrchWnd: UL(%ld, %ld) LR(%ld, %ld)", WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET + i,
						stSrchArea.coCorner1.x, stSrchArea.coCorner1.y,
						stSrchArea.coCorner2.x, stSrchArea.coCorner2.y);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		if (ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE + i, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2, TRUE) != -1)
		{	
			bFindRefDie = TRUE;

			if (DieIsAlignable(usDieType) == TRUE)
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
				
				PR_WIN stFirstSrchArea;
				GetSearchDieArea(&stFirstSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET);
				DrawRectangleBox(stFirstSrchArea.coCorner1, stFirstSrchArea.coCorner2, PR_COLOR_TRANSPARENT);
				DrawRectangleBox(stSrchArea.coCorner1, stSrchArea.coCorner2, PR_COLOR_GREEN);
				//find reference die, display Message on HMI
				DisplaySearchDieResult(usDieType, WPR_REFERENCE_DIE, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET + i, fDieRotate, stDieOffset, fDieScore);
				bReturn = TRUE;
				//bFindRefDie = TRUE;
				break;
			}
			else
			{
				if (i == (m_lLrnTotalRefDie - 1))		//the last ref die record
				{
					DisplaySearchDieResult(usDieType, WPR_REFERENCE_DIE, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET + i, fDieRotate, stDieOffset, fDieScore);
					bReturn = TRUE;
				}
			}
		}
	}
	
	//cannot find reference die
	if (!bFindRefDie)
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		bReturn = FALSE;
	}

	//On Joystick & Clear PR screen
	SetWaferTableJoystick(TRUE);
	ChangeCamera(WPR_CAM_WAFER);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferPr::UserSearchAllDie(IPC_CServiceMessage &svMsg)
{
	LONG lSearchDieNo, lOldZoom;
	svMsg.GetMsg(sizeof(LONG), &lSearchDieNo);


	if (m_bSelectDieType == WPR_NORMAL_DIE)
	{
		PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
		PR_COORD		stDieOffset; 
		PR_REAL			fDieRotate; 
		PR_REAL			fDieScore;
		BOOL bReturn = TRUE;


		if (PR_NotInit())
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}


		//Clear PR screen & Off joystick
		ChangeCamera(WPR_CAM_WAFER);
		SetWaferTableJoystick(FALSE);
		DrawSearchBox(PR_COLOR_GREEN);

		//Change the zoom mode before searching die
		lOldZoom = GetRunZoom();
		ChangePrRecordZoomMode(lSearchDieNo + WPR_GEN_NDIE_OFFSET);

		PR_WORD wResult = 0;
		BOOL bSearchEproxy = FALSE;
		BOOL bEproxyStatus = TRUE;

		//v4.54A6
		if (CMS896AStn::m_bMS60NGPick && (lSearchDieNo == 2))
		{
			bSearchEproxy = TRUE;

			LONG lcx = GetPrCenterX();
			LONG lcy = GetPrCenterY();
			UINT nX = 0, nY = 0;
			ULONG ulSize = 0;
			CString szErrMsg = "";
			bEproxyStatus = AutoSearchEpoxy(FALSE, TRUE, TRUE, lcx, lcy, nX, nY, ulSize, NULL,szErrMsg);
		}
		else
		{
			wResult = ManualSearchDie(WPR_NORMAL_DIE, lSearchDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
		} 
		
		if (bSearchEproxy)
		{
		}
		else
		{
			if (wResult != -1)
			{
				if (DieIsAlignable(usDieType) == TRUE)
				{
					//Allow die to rotate
					ManualDieCompenate(stDieOffset, fDieRotate);
					DrawSearchBox( PR_COLOR_TRANSPARENT);
					// If video test enabled, not show the die size rectangle
					if (m_bDebugVideoTest == FALSE)
					{
						DrawSrchDieResultBox(m_bSelectDieType, m_lCurRefDieNo, PR_COLOR_GREEN);
					}
				}

				//Display Message on HMI
				DisplaySearchDieResult(usDieType, WPR_NORMAL_DIE, lSearchDieNo - 1, fDieRotate, stDieOffset, fDieScore);
				bReturn = TRUE;
			}
			else
			{
				SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
				bReturn = FALSE;
			}
		}

		//On Joystick & Clear PR screen & Change to the original zoom mode
		SetWaferTableJoystick(TRUE);
		ChangeCamera(WPR_CAM_WAFER);
		LiveViewZoom(lOldZoom, FALSE, FALSE, 17);

		svMsg.InitMessage(sizeof(BOOL), &bReturn);

		//return 1;
	}
//4.52D17Manual Search
	if((lSearchDieNo == MS899_UPLOOK_REF_DIENO_BH1 || lSearchDieNo == MS899_UPLOOK_REF_DIENO_BH2) && m_bSelectDieType == WPR_REFERENCE_DIE) 
	{
		BOOL bReturn = UserSearchUpLookDie(lSearchDieNo, TRUE);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
	}
	else if (m_bSelectDieType == WPR_REFERENCE_DIE)
	{
		PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
		PR_COORD		stDieOffset; 
		PR_REAL			fDieRotate; 
		PR_REAL			fDieScore;
		BOOL			bReturn = TRUE;

		if (PR_NotInit())
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		//Clear PR screen & Off joystick
		ChangeCamera(WPR_CAM_WAFER);
		SetWaferTableJoystick(FALSE);

		//Change the zoom mode before searching die
		LONG lOldZoom = GetRunZoom();
		ChangePrRecordZoomMode(lSearchDieNo + WPR_GEN_RDIE_OFFSET);

		PR_WIN stSrchArea;
		GetSearchDieArea(&stSrchArea, lSearchDieNo + WPR_GEN_RDIE_OFFSET);
		DrawRectangleBox(stSrchArea.coCorner1, stSrchArea.coCorner2, PR_COLOR_GREEN);

		if (ManualSearchDie(WPR_REFERENCE_DIE, lSearchDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2) != -1)
		{
			if (DieIsAlignable(usDieType) == TRUE)
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
			}

			//Display Message on HMI
			DisplaySearchDieResult(usDieType, WPR_REFERENCE_DIE, lSearchDieNo + WPR_GEN_RDIE_OFFSET, fDieRotate, stDieOffset, fDieScore);
			bReturn = TRUE;
		}
		else
		{
			SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
			bReturn = FALSE;
		}


		//On Joystick & Clear PR screen & Change to the original zoom mode
		SetWaferTableJoystick(TRUE);
		ChangeCamera(WPR_CAM_WAFER);
		LiveViewZoom(lOldZoom, FALSE, FALSE, 18);

		svMsg.InitMessage(sizeof(BOOL), &bReturn);

		//return 1;
	}

	return TRUE;
}

LONG CWaferPr::LearnDiePitch(IPC_CServiceMessage &svMsg)
{
	BOOL bAutoLearn = TRUE;
	//Get Auto or Manual Learn
	svMsg.GetMsg(sizeof(BOOL), &bAutoLearn);

	BOOL bLearnPitch = TRUE;
	if( IsEnableZoom() )
	{
		if( IsDP() )
		{
			if( IsDP_ScanCam() )
			{
				bLearnPitch = FALSE;
			}
		}
		else if( GetRunZoom()!=GetNmlZoom() )
		{
			bLearnPitch = FALSE;
		}
	}
	if( bLearnPitch )
	{
		MoveBackLightWithTableCheck(TRUE, TRUE);
		WprLearnDiePitch(bAutoLearn);
		MoveBackLightWithTableCheck(FALSE);
	}

	BOOL	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::WprLearnDiePitch(CONST BOOL bAutoLearn)
{
	LONG	lResult;	
	BOOL	bReturn = TRUE;
	INT nPreviousXX, nPreviousYY;
	INT siDelta_X, siDelta_Y;

	if (PR_NotInit())
	{
		return bReturn;
	}

	if (IsNormalDieLearnt() == FALSE)
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		return bReturn;
	}

	if (m_bDieCalibrated == FALSE)
	{
		SetAlert(IDS_WPR_DIENOTCALIBRATED);
		return bReturn;
	}


	//Force to switch camera
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	nPreviousXX = m_siDiePitchXX;
	nPreviousYY = m_siDiePitchYY;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bRebelLumileds = pApp->IsPLLMRebel();
	if( bRebelLumileds)
	{
		CString szTitle = "Auto learn die pitch";
		CString szContent = "Do you want to find wafer angle?";
		if (HmiMessageEx(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
		{
			if( GetDiePitchX_X()<=GetDieSizeX() )
			{
				(*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"] = GetDieSizeX()*110/100;
			}
			if( GetDiePitchY_Y()<=GetDieSizeY() )
			{
				(*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"] = GetDieSizeY()*110/100;
			}

			IPC_CServiceMessage stMsg;
			typedef struct 
			{
				LONG	lLHS;
				LONG	lRHS;	
				LONG	lAlignMode;		// 0=Horiz, 1=Vert		//v3.02T6
			} ALNSETUP;
			ALNSETUP stInfo;

			// Set Index parameter
			stInfo.lLHS = 5;	//	m_lAngleLHSCount;
			stInfo.lRHS = 5;	//	m_lAngleRHSCount;
			stInfo.lAlignMode = 0;	//	m_lAlignWaferAngleMode;		// 0=Horizontal, 1=Vertical		//v3.02T7
			stMsg.InitMessage(sizeof(ALNSETUP), &stInfo);

			// Get the reply for the encoder value
			AlignWaferAngle(stMsg);
		}
	}
	lResult = FindDiePitch(bAutoLearn);

	siDelta_X = abs(m_siDiePitchXX - nPreviousXX);
	siDelta_Y = abs(m_siDiePitchYY - nPreviousYY);

	//Give out warning if exceed die pitch tolerance
	if (siDelta_X > (nPreviousXX * WPR_DIE_PITCH_TOLERANCE))
	{
		SetAlert(IDS_WPR_LRN_PITCH_INVALID_X);
		SetErrorMessage("Wafer Learn-Die-Pitch exceeds X pitch tolerance");
	}
	
	if (siDelta_Y > (nPreviousYY * WPR_DIE_PITCH_TOLERANCE))
	{
		SetAlert(IDS_WPR_LRN_PITCH_INVALID_Y);
		SetErrorMessage("Wafer Learn-Die-Pitch exceeds Y pitch tolerance");
	}


	if (lResult == 0)		//xyz
	{
		//v2.78T2
		// Also check against master pitch if enabled
		double dCurrPitchinMilX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)m_siDiePitchXX * 1000 / 25.4);
		double dCurrPitchinMilY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)m_siDiePitchYY * 1000 / 25.4);
		double dDx = fabs(m_dWAlignMasterPitchInMilX - dCurrPitchinMilX);
		double dDy = fabs(m_dWAlignMasterPitchInMilY - dCurrPitchinMilY);
		if ((m_dWAlignMasterPitchInMilX > 0) && (dDx > (m_dWAlignMasterPitchInMilX * WPR_DIE_PITCH_TOLERANCE)))
		{
			CString szTemp;
			szTemp.Format("Warning: Current x-pitch is different from master-pitch-x by more than 30%");
			SetErrorMessage(szTemp);
			SetAlert_Red_Yellow(IDS_WPR_LRN_PITCH_INVALID_MASTER_X);
		}
		if ((m_dWAlignMasterPitchInMilY > 0) && (dDy > (m_dWAlignMasterPitchInMilY * WPR_DIE_PITCH_TOLERANCE)))
		{
			CString szTemp;
			szTemp.Format("Warning: Current y-pitch is different from master-pitch-y by more than 30%");
			SetErrorMessage(szTemp);
			SetAlert_Red_Yellow(IDS_WPR_LRN_PITCH_INVALID_MASTER_Y);
		}
	}


	switch (lResult)
	{
	case WPR_ERR_PITCH_NO_1ST_DIE:
		SetAlert(IDS_WPR_NODIEONCENTER);
		break;

	case WPR_ERR_PITCH_NO_X_DIE:
		SetAlert(IDS_WPR_NODIEONLEFTSIDE);
		break;

	case WPR_ERR_PITCH_NO_Y_DIE:
		SetAlert(IDS_WPR_NODIEONUPSIDE);
		break;

		//xyz
	case WPR_ERR_INVALID_PITCH_X:
		break;
	case WPR_ERR_INVALID_PITCH_Y:
		break;

	default:
		SetStatusMessage("Wafer learn die pitch is completed");
		
		CString szTitle, szContent;

		szTitle.LoadString(HMB_WPR_DIE_PITCH);
		szContent.LoadString(HMB_WPR_LRN_PITCH_OK);
		
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		//SetAlert(IDS_WPR_DIEPITCHLEARNT);
		break;
	}

	if (lResult != 0)
	{
		SetErrorMessage("Wafer learn die pitch is failed");
	}
	
	//Force to switch camera
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(TRUE);
	SavePrData(TRUE);

	//Update LookForward search area
	CalculateLFSearchArea();

	if( bRebelLumileds )
	{
		if( m_siDiePitchYX*128>m_siDiePitchXX ||
			m_siDiePitchXY*128>m_siDiePitchYY )
		{
			HmiMessage("Learn die global theta is not good.\nPlease sure global theta done.", "PLLM");
		}
	}

	return bReturn;
}

LONG CWaferPr::LearnDieCalibration(IPC_CServiceMessage &svMsg)
{
	BOOL	bReturn = WprLearnDieCalibration();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::WprLearnDieCalibration()
{
	LONG	lResult;
	BOOL	bReturn = 	TRUE;

	if (PR_NotInit())
	{
		return bReturn;
	}

	BOOL bNmlLrnt = IsNormalDieLearnt();
	if ( bNmlLrnt== FALSE)
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		return bReturn;
	}
	if( GetPrescanPrID()==3 )
	{
		if( IsDP() )
		{
			if( IsDP_ScanCam() )
			{
				bNmlLrnt = IsThisDieLearnt(GetPrescanPrID()-1);
			}
		}
		else
		{
			if( GetRunZoom()==GetScnZoom() && GetScnZoom()!=GetNmlZoom() && GetRunZoom()==0 )
			{
				bNmlLrnt = IsThisDieLearnt(GetPrescanPrID()-1);
			}
		}
	}

	if ( bNmlLrnt== FALSE)
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		return bReturn;
	}

	//Force to switch camera
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	lResult = FindDieCalibration();

	switch (lResult)
	{
	case WPR_ERR_CALIB_NO_1ST_DIE:
		SetAlert(IDS_WPR_NODIEONCENTER);
		break;

	case WPR_ERR_CALIB_NO_LT_DIE:
		SetAlert(IDS_WPR_NODIEONLEFTSIDE);
		break;

	case WPR_ERR_CALIB_NO_RT_DIE:
		SetAlert(IDS_WPR_NODIEONRIGHTSIDE);
		break;

	case WPR_ERR_CALIB_NO_UP_DIE:
		SetAlert(IDS_WPR_NODIEONUPSIDE);
		break;

	case WPR_ERR_CALIB_NO_DN_DIE:
		SetAlert(IDS_WPR_NODIEONDOWNSIDE);
		break;

	case WPR_ERR_CALIB_ZERO_VALUE:
		SetAlert(IDS_WPR_CALFACTORERROR);
		break;

	default:
		SetStatusMessage("Wafer learn die calibration is completed");
		
		CString szTitle, szContent;

		szTitle.LoadString(HMB_WPR_DIE_CALIBRATION);
		szContent.LoadString(HMB_WPR_DIE_CALIBRATION_OK);

		SetbGenDieCalibration(TRUE);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		break;
	}

	if (lResult != 0)
	{
		SetErrorMessage("Wafer learn die calibration is failed");
		SetbGenDieCalibration(FALSE);
	}

	UpdateWPRInfoList();
	//Force to switch camera
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(TRUE);
	SavePrData(TRUE);

	return 1;
}



//Debug PR function
LONG CWaferPr::DeletePRRecord(IPC_CServiceMessage &svMsg)
{
	BOOL		bReturn = TRUE;
	LONG		lPrID;
	int			i;
	CString		csMsgA;
	CString		szTitle;

	svMsg.GetMsg(sizeof(LONG), &lPrID);

	szTitle.LoadString(HMB_WPR_REMOVE_PR_RECORD);

	csMsgA.LoadString(HMB_GENERAL_AREUSURE);

	if (lPrID == 0)
	{
		if (HmiMessage(csMsgA, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
		{
			bReturn = FALSE;
			UpdateWPRInfoList();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		FreeAllPrRecords();	//	HMI delete all

		csMsgA.LoadString(HMB_WPR_ALL_PRID_REMOVED);
		SavePrData(TRUE);
		HmiMessage(csMsgA, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}
	else
	{
		for (i = 0; i < WPR_MAX_DIE; i++)
		{
			if (GetDiePrID(i) == lPrID)
			{
				if (HmiMessage(csMsgA, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
				{
					bReturn = FALSE;
					UpdateWPRInfoList();
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}

				if (lPrID < WPR_MAX_DIE)
				{
					m_pPrGeneral->FreePRRecord(lPrID, GetRunSenID(), GetRunRecID()); //	delete single by HMI
		
					csMsgA.LoadString(HMB_WPR_ONE_PRID_REMOVED);
		
					//clear variables
					m_szGenDieZoomMode[i] = "";
					m_bGenDieCalibration[i] = FALSE;
					m_bGenDieLearnt[i] = FALSE;
					m_ssGenPRSrchID[i] = 0;
					SavePrData(TRUE);
		
					HmiMessage(csMsgA, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
					break;
				}
			}
		}
	}

	m_uwIMMergeNewRecordID		= 0;
	for (int i = 0; i < WPR_MAX_DIE; i++)
	{
		m_uwIMMergeRecordID[i] = 0;
	}

	UpdateWPRInfoList();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CWaferPr::DisplayPRRecord(IPC_CServiceMessage &svMsg)
{
	PR_COORD				stCorner1;
	PR_COORD				stCorner2;
	LONG					lPrID;
	svMsg.GetMsg(sizeof(LONG), &lPrID);

	if (lPrID == 0)
	{
		LONG lDieNo = GetWprDieNo(); //(m_bSelectDieType, m_lCurRefDieNo);
		lPrID = m_ssGenPRSrchID[lDieNo];
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_UWORD uwPRStatus = m_pPrGeneral->DisplayLearnInfo(lPrID, GetPrCenterX(), GetPrCenterY(), ubSID, ubRID);
	if (PRS_PRNoErr(uwPRStatus))
	{
		stCorner1.x	= PR_MIN_COORD;
		stCorner1.y = PR_MIN_COORD;
		stCorner2.x = PR_MAX_COORD;
		stCorner2.y = PR_MAX_COORD;

		DrawRectangleBox(stCorner1, stCorner2, PR_COLOR_YELLOW);
		HmiMessage("Confirm Previous Learnt PR", "System Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}
	else
	{
		HmiMessage("PRS Show Info Error", "System Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}

	ChangeCamera(m_bCurrentCamera);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::EnableDebugFlag(IPC_CServiceMessage &svMsg)
{
	PR_SetDebugFlag((PR_DEBUG)m_lDebugFlag);
	SendCE_DebugPr();

	return 1;
}

LONG CWaferPr::EnableDebugLog(IPC_CServiceMessage &svMsg)
{
	PR_SetComLogFlag((PR_COM_LOG)(m_lDebugLog));
	SendCE_DebugPr();

	return 1;
}

LONG CWaferPr::DisplayLFSearchRegion(IPC_CServiceMessage &svMsg)
{
	BOOL bDisplay		= TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bDisplay);
	LONG lRegionSize	= REGION_SIZE_5x5;
	LONG lLFPosn		= 0;

	PR_COLOR sColor = PR_COLOR_TRANSPARENT;
	if (bDisplay)
	{
		sColor = PR_COLOR_YELLOW;
	}
	DisplayLFSearchArea_mxn(sColor, lRegionSize, lLFPosn);
	CString szMsg;
	szMsg.Format("WPR: Pixel Pitch XX(%ld)XY(%ld) YY(%ld)YX(%ld)", 
					GetPitchPixelXX(), GetPitchPixelXY(), GetPitchPixelYY(), GetPitchPixelYX());
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);


	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::UpdateSoraaDiePitch(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != _T("Soraa"))
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsSorraSortMode())
	{
		m_siDiePitchXX = m_siDiePitchXX / 2;				
		m_siDiePitchYY = m_siDiePitchYY / 2;				
	}
	else
	{
		m_siDiePitchXX = m_siDiePitchXX * 2;				
		m_siDiePitchYY = m_siDiePitchYY * 2;				
	}

	SavePrData(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

VOID CWaferPr::SendCE_DebugPr(VOID)
{
	// 3376
	SetGemValue("SD_PRVideoTest", m_bDebugVideoTest);
	SetGemValue("SD_PRDebugFlag", m_lDebugFlag);
	SetGemValue("SD_PRDebugLog", m_lDebugLog);
	// 7302
	SendEvent(SG_CEID_WP_DEBUG, FALSE);
}

VOID CWaferPr::SendCE_SearchParam(VOID)
{
	// 3361
	SetGemValue("WPR_SrhDieType", m_bSelectDieType);
	SetGemValue("WPR_SrhDieID", m_lCurRefDieNo);
	// 3362
	SetGemValue("WPR_SrhDiePassScore", m_lSrchDieScore);
	SetGemValue("WPR_SrhDieCheckChip", m_bSrchEnableChipCheck);
	SetGemValue("WPR_SrhDieChipRate", m_dSrchChipArea);
	SetGemValue("WPR_SrhDieCheckDefect", m_bSrchEnableDefectCheck);
	SetGemValue("WPR_SrhDieSDefectRate", m_dSrchSingleDefectArea);
	SetGemValue("WPR_SrhDieTDefectRate", m_dSrchTotalDefectArea);
	// 3363
	SetGemValue("WPR_SrhDieAreaSize", m_lSrchDieAreaX);
	SetGemValue("WPR_SrhDieAreaSize", m_lSrchDieAreaY);
	// 3364
	SetGemValue("WPR_SrhDieTCorrect", m_bThetaCorrection);
	SetGemValue("WPR_SrhDieTTolerance", m_dAcceptDieAngle);
	SetGemValue("WPR_SrhDieTMinTolerance", m_dMinDieAngle);
	SetGemValue("WPR_SrhDieTMaxTolerance", m_dMaxDieAngle);
	// 3365
	SetGemValue("WPR_SrhDieLookForward", m_bSrchEnableLookForward);
	// 3366
	SetGemValue("WPR_SrhDieBadCut", m_bBadCutDetection);
	SetGemValue("WPR_SrhDieBadCutX", m_stBadCutTolerenceX);
	SetGemValue("WPR_SrhDieBadCutY", m_stBadCutTolerenceY);
	// 3367
	SetGemValue("WPR_SrhDieBackupAlign", m_bSrchEnableBackupAlign);
	SetGemValue("WPR_SrhDieAlignResol", m_lSrchAlignRes);
	// 3368
	SetGemValue("WPR_SrhDieGreyLevel", m_lSrchGreyLevelDefect);
	// 3369
	SetGemValue("WPR_SrhDieThreshold", m_lSrchDefectThres);
	// 3370
	SetGemValue("WPR_SrhDieRefCheck", m_bRefDieCheck);
	SetGemValue("WPR_SrhDieRefCheckNo", m_lCurRefDieNo);
	// 7301
	SendEvent(SG_CEID_WP_SEARCHPARAM, FALSE);
}

//Debug only
LONG CWaferPr::OfflineSearchDie(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bStatus = TRUE;
	int				siEncX = 0; 
	int				siEncY = 0; 
	int				siEncT = 0; 



	if (PR_NotInit())
	{
		bStatus = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bStatus);
		return 1;
	}

	SetWaferTableJoystick(FALSE);

	DrawSearchBox(PR_COLOR_GREEN);
	PR_WORD wResult = 0;

	//v2.96T3
	wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);

	if (wResult != -1)
	{
		//Update Sram variable
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate(stDieOffset, fDieRotate);
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			DrawRectangleDieSize(PR_COLOR_GREEN);
		}	

		bStatus = TRUE;
	}
	else
	{
		bStatus = FALSE;
	}

	//Return Reply
	GetWaferTableEncoder(&siEncX, &siEncY, &siEncT);

	svMsg.InitMessage(sizeof(BOOL), &bStatus);

	return 1;
}

LONG CWaferPr::SearchDieInSearchWindow(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bBackupAlignTemp;
	BOOL			bStatus = TRUE;
	BOOL			bDieType = WPR_NORMAL_DIE;
	LONG			lRefDieNo = 1;
	int				siStepX = 0; 
	int				siStepY = 0; 

	PR_WIN			stSearchArea;


	typedef struct 
	{
		BOOL		bShowPRStatus;
		BOOL		bNormalDie;
		LONG		lRefDieNo;
		BOOL		bDisableBackupAlign;
	} SRCH_TYPE;

	SRCH_TYPE	stSrchInfo;

	typedef struct 
	{
		BOOL		bStatus;
		BOOL		bGoodDie;
		BOOL		bFullDie;
		LONG		lRefDieNo;
		LONG		lX;
		LONG		lY;
	} REF_TYPE;

	REF_TYPE	stInfo;


	if (PR_NotInit())
	{
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
		svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
		return 1;
	}

	//Get Information from other station
	svMsg.GetMsg(sizeof(SRCH_TYPE), &stSrchInfo);

	//Do Search die on this position
	lRefDieNo = stSrchInfo.lRefDieNo;

	if (stSrchInfo.bNormalDie == TRUE)
	{
		bDieType = WPR_NORMAL_DIE;
	}
	else
	{
		bDieType = WPR_REFERENCE_DIE;
	}

	//v2.96T3
	PR_WORD wResult = 0;

	bBackupAlignTemp = m_bSrchEnableBackupAlign;
	if (stSrchInfo.bDisableBackupAlign == TRUE)
	{
		m_bSrchEnableBackupAlign = FALSE;
	}

	if (bDieType == WPR_REFERENCE_DIE)
	{
		GetSearchDieArea(&stSearchArea, lRefDieNo + WPR_GEN_RDIE_OFFSET);
	}
	else
	{
		GetSearchDieArea(&stSearchArea, lRefDieNo);
	}
	
	CString szLog;
	szLog.Format("SearchDieInSearchWindow (PLLM) RefDie #%d: SrchWnd - (%ld, %ld) (%ld, %ld)", 
					lRefDieNo,
					stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
					stSearchArea.coCorner2.x, stSearchArea.coCorner2.y);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	wResult = ManualSearchDie(bDieType, lRefDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSearchArea.coCorner1, stSearchArea.coCorner2);

	m_bSrchEnableBackupAlign = bBackupAlignTemp;

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		}

		//Return result
		stInfo.bStatus	= TRUE;
		stInfo.bGoodDie	= DieIsGood(usDieType);
		stInfo.bFullDie	= DieIsAlignable(usDieType);
		stInfo.lX		= (LONG)(siStepX);
		stInfo.lY		= (LONG)(siStepY);

		//Display PR status if necessary
		if (stSrchInfo.bShowPRStatus == TRUE)
		{
			CString szDieResult;

			ConvertDieTypeToText(usDieType, szDieResult);
			m_szSrhDieResult = "Die Type: " + szDieResult;
		}
	}
	else
	{
		//Return result
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
	}
	
	svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);

	return 1;
}


LONG CWaferPr::SearchCurrentDie(IPC_CServiceMessage &svMsg)
{
	SRCH_TYPE	stSrchInfo;

	REF_TYPE	stInfo;
	//Get Information from other station
	svMsg.GetMsg(sizeof(SRCH_TYPE), &stSrchInfo);

	SubSearchCurrentDie(stSrchInfo, stInfo);

	svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);

	return 1;
}

VOID CWaferPr::SubSearchCurrentDie(const SRCH_TYPE stSrchInfo, REF_TYPE &stInfo)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bBackupAlignTemp;
	BOOL			bStatus = TRUE;
	BOOL			bDieType = WPR_NORMAL_DIE;
	LONG			lRefDieNo = 1;
	int				siStepX = 0; 
	int				siStepY = 0; 

	if (PR_NotInit())
	{
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
		return;
	}


	//Do Search die on this position
	lRefDieNo = stSrchInfo.lRefDieNo;

	if (stSrchInfo.bNormalDie == TRUE)
	{
		bDieType = WPR_NORMAL_DIE;
	}
	else
	{
		bDieType = WPR_REFERENCE_DIE;
	}

	//v2.96T3
	PR_WORD wResult = 0;

	bBackupAlignTemp = m_bSrchEnableBackupAlign;
	if (stSrchInfo.bDisableBackupAlign == TRUE)
	{
		m_bSrchEnableBackupAlign = FALSE;
	}

	wResult = ManualSearchDie(bDieType, lRefDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);

	m_bSrchEnableBackupAlign = bBackupAlignTemp;

	m_dPrDieCenterX = 0;
	m_dPrDieCenterY = 0;
	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			m_dPrDieCenterX = stDieOffset.x;
			m_dPrDieCenterY = stDieOffset.y;
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		}

		//Return result
		stInfo.bStatus	= TRUE;
		stInfo.bGoodDie	= DieIsGood(usDieType);
		stInfo.bFullDie	= DieIsAlignable(usDieType);
		stInfo.lX		= (LONG)(siStepX);
		stInfo.lY		= (LONG)(siStepY);

		//Display PR status if necessary
		if (stSrchInfo.bShowPRStatus == TRUE)
		{
			CString szDieResult;

			ConvertDieTypeToText(usDieType, szDieResult);
			m_szSrhDieResult = "Die Type: " + szDieResult;
		}
	}
	else
	{
		//Return result
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
	}

}


LONG CWaferPr::SearchCurrentDie_PLLM_REBEL(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bBackupAlignTemp;
	BOOL			bStatus = TRUE;
	BOOL			bDieType = WPR_NORMAL_DIE;
	LONG			lRefDieNo = 1;
	int				siStepX = 0; 
	int				siStepY = 0; 


	typedef struct 
	{
		BOOL		bShowPRStatus;
		BOOL		bNormalDie;
		LONG		lRefDieNo;
		BOOL		bDisableBackupAlign;
	} SRCH_TYPE;

	SRCH_TYPE	stSrchInfo;

	typedef struct 
	{
		BOOL		bStatus;
		BOOL		bGoodDie;
		BOOL		bFullDie;
		LONG		lRefDieNo;
		LONG		lX;
		LONG		lY;
	} REF_TYPE;

	REF_TYPE	stInfo;


	if (PR_NotInit())
	{
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
		svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
		return 1;
	}

	//Get Information from other station
	svMsg.GetMsg(sizeof(SRCH_TYPE), &stSrchInfo);

	//Do Search die on this position
	lRefDieNo = stSrchInfo.lRefDieNo;

	PR_COORD stCorner1 = GetSrchArea().coCorner1;
	PR_COORD stCorner2 = GetSrchArea().coCorner2;
	if (stSrchInfo.bNormalDie == TRUE)
	{
		bDieType = WPR_NORMAL_DIE;
	}
	else
	{
		bDieType = WPR_REFERENCE_DIE;
	//	if( PLLM_REBEL_EXTRA_RFOV )
		{
		stCorner1.x = GetPRWinULX();
		stCorner1.y = GetPRWinULY();
		stCorner2.x = GetPRWinLRX() - 1;
		stCorner2.y = GetPRWinLRY() - 1;
		}
	}

	//v2.96T3
	PR_WORD wResult = 0;

	bBackupAlignTemp = m_bSrchEnableBackupAlign;
	if (stSrchInfo.bDisableBackupAlign == TRUE)
	{
		m_bSrchEnableBackupAlign = FALSE;
	}

	wResult = ManualSearchDie(bDieType, lRefDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stCorner1, stCorner2);
	if ( (wResult == -1) || !DieIsAlignable(usDieType) )
	{
		//v4.41T1
		//Use normal die record #2 to search die for REBEL, because there may be REBEL dices
		//	without dome pattern but still need to be aligned;
		if (bDieType == WPR_NORMAL_DIE && IsThisDieLearnt((UCHAR)lRefDieNo) )
			wResult = ManualSearchDie(bDieType, lRefDieNo+1, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stCorner1, stCorner2);
	}
	
	m_bSrchEnableBackupAlign = bBackupAlignTemp;

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		}

		//Return result
		stInfo.bStatus	= TRUE;
		stInfo.bGoodDie	= DieIsGood(usDieType);
		stInfo.bFullDie	= DieIsAlignable(usDieType);
		stInfo.lX		= (LONG)(siStepX);
		stInfo.lY		= (LONG)(siStepY);

		//Display PR status if necessary
		if (stSrchInfo.bShowPRStatus == TRUE)
		{
			CString szDieResult;

			ConvertDieTypeToText(usDieType, szDieResult);
			m_szSrhDieResult = "Die Type: " + szDieResult;
		}
	}
	else
	{
		//Return result
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
	}
	
	svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
	return 1;
}


LONG CWaferPr::SearchCurrentDieInFOV(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bStatus = TRUE;
	BOOL			bDieType = WPR_NORMAL_DIE;
	LONG			lRefDieNo = 1;
	int				siStepX = 0; 
	int				siStepY = 0; 
	BOOL			bBackupAlignTemp;

	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX() - 1, GetPRWinLRY() - 1};


	typedef struct 
	{
		BOOL		bShowPRStatus;
		BOOL		bNormalDie;
		LONG		lRefDieNo;
		BOOL		bDisableBackupAlign;
	} SRCH_TYPE;

	SRCH_TYPE	stSrchInfo;

	typedef struct 
	{
		BOOL		bStatus;
		BOOL		bGoodDie;
		BOOL		bFullDie;
		LONG		lRefDieNo;
		LONG		lX;
		LONG		lY;
	} REF_TYPE;

	REF_TYPE	stInfo;


	if (PR_NotInit())
	{
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
		svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
		return 1;
	}

	//Get Information from other station
	svMsg.GetMsg(sizeof(SRCH_TYPE), &stSrchInfo);

	//Do Search die on this position
	lRefDieNo = stSrchInfo.lRefDieNo;

	if (stSrchInfo.bNormalDie == TRUE)
	{
		bDieType = WPR_NORMAL_DIE;
	}
	else
	{
		bDieType = WPR_REFERENCE_DIE;
	}

	//v2.96T3
	PR_WORD wResult = 0;

	//Disable backup-align temporarily if currently enabled
	bBackupAlignTemp = m_bSrchEnableBackupAlign;
	if (stSrchInfo.bDisableBackupAlign == TRUE)
	{
		m_bSrchEnableBackupAlign = FALSE;
	}


	if ((bDieType == WPR_REFERENCE_DIE) && m_bAllRefDieCheck)	//v3.49T4
	{
		//PR_WIN stSrchArea;

		for (int i = 0; i < m_lLrnTotalRefDie; i++)
		{
			//v4.43T11
			if (m_bEnableMS100EjtXY && (i == 1))	// Ej Cap pattern
			{
				continue;
			}
			if (m_bEnableMS100EjtXY && (i == 2))	// Collet hole
			{
				continue;
			}

			//GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET + i);
			wResult = ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE + i, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
			
			if (wResult != -1)
			{	
				if (!DieIsGood(usDieType))
				{
					continue;
				}	

				CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);

				//Return result
				stInfo.bStatus	= TRUE;
				stInfo.bGoodDie	= DieIsGood(usDieType);
				stInfo.bFullDie	= DieIsAlignable(usDieType);
				stInfo.lX		= (LONG)(siStepX);
				stInfo.lY		= (LONG)(siStepY);

				m_bSrchEnableBackupAlign = bBackupAlignTemp;
				svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
				return 1;
			}
		}

		//Return NG result if all ref-die not matched
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;

		m_bSrchEnableBackupAlign = bBackupAlignTemp;
	}
	else
	{
		wResult = ManualSearchDie(bDieType, lRefDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);

		m_bSrchEnableBackupAlign = bBackupAlignTemp;

		if (wResult != -1)
		{
			if (DieIsAlignable(usDieType) == TRUE)
			{
				CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
			}

			//Return result
			stInfo.bStatus	= TRUE;
			stInfo.bGoodDie	= DieIsGood(usDieType);
			stInfo.bFullDie	= DieIsAlignable(usDieType);
			stInfo.lX		= (LONG)(siStepX);
			stInfo.lY		= (LONG)(siStepY);

			//Display PR status if necessary
			if (stSrchInfo.bShowPRStatus == TRUE)
			{
				CString szDieResult;
				ConvertDieTypeToText(usDieType, szDieResult);
				m_szSrhDieResult = "Die Type: " + szDieResult;
			}
		}
		else
		{
			//Return result
			stInfo.bStatus	= FALSE;
			stInfo.bGoodDie	= FALSE;
			stInfo.bFullDie	= FALSE;
			stInfo.lX		= 0;
			stInfo.lY		= 0;
		}
	}
	
	svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
	return 1;
}


LONG CWaferPr::ToggleJoystickSpeed(IPC_CServiceMessage &svMsg)
{
	
	SetBinTableJoystick(FALSE);
	SetWaferTableJoystick(FALSE);

	if (m_bCurrentCamera == WPR_CAM_WAFER)
	{
		m_lJoystickLevel_WT++;
		if (m_lJoystickLevel_WT > 2) 
		{
			m_lJoystickLevel_WT = 0;
		}
		SetWaferTableJoystickSpeed(m_lJoystickLevel_WT);
		
		if (m_ulJoyStickMode == MS899_JS_MODE_PR)
		{
			SetWaferTableJoystick(TRUE);
			m_lMouseJoystickSpeed = m_lJoystickLevel_WT;
		}
	}
	else
	{
		if (WPR_GetFrameLevel() == TRUE)
		{
			SetAlert(IDS_BL_PLATFORM_NOT_DOWN);
		}
		else
		{
			m_lJoystickLevel_BT++;
			if (m_lJoystickLevel_BT > 2) 
			{
				m_lJoystickLevel_BT = 0;
			}
			SetBinTableJoystickSpeed(m_lJoystickLevel_BT);

			if (m_ulJoyStickMode == MS899_JS_MODE_PR)
			{
				SetBinTableJoystick(TRUE);
				m_lMouseJoystickSpeed = m_lJoystickLevel_BT;
			}
		}
		
	}
//	DisplayJoystickSpeed();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// --- Use Mouse ---
LONG CWaferPr::UsePRMouse(IPC_CServiceMessage &svMsg)
{
	EnableMouseClickCallbackCmd(FALSE);

	PR_SET_MOUSE_CTRL_OBJ_CMD		stSetCmd;
	PR_SET_MOUSE_CTRL_OBJ_RPY		stSetRpy;

	if (m_bUseMouse == FALSE)
	{
		// Erase the original region
		if (GetDieShape() == WPR_RECTANGLE_DIE)
		{
			DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT], PR_UPPER_LEFT, 0);
			DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 0);
		}
		else if (GetDieShape() == WPR_TRIANGULAR_DIE)	//v4.06
		{
			DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[0], 0);
		}
		else if (GetDieShape() == WPR_RHOMBUS_DIE)
		{
			DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[0], 0);
		}
		else
		{
			DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[4], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[4], m_stLearnDieCornerPos[5], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[5], m_stLearnDieCornerPos[0], 0);
		}

		// Define by using mouse
		PR_SHAPE_TYPE emShape = PR_SHAPE_TYPE_RECTANGLE;
		if (GetDieShape() == WPR_RECTANGLE_DIE)
		{
			emShape = PR_SHAPE_TYPE_RECTANGLE;
		}
		else
		{
			emShape = PR_SHAPE_TYPE_POLYGON;
		}
		m_pPrGeneral->MouseDefineRegion(emShape, PR_COLOR_RED);

		// Set the mouse size first
		PR_InitSetMouseCtrlObjCmd(&stSetCmd);
		stSetCmd.ulObjectId = 1;

		if (GetDieShape() == WPR_RECTANGLE_DIE)
		{
			stSetCmd.emShape = PR_SHAPE_TYPE_RECTANGLE;
			stSetCmd.uwNumOfCorners = 2;
			stSetCmd.acoObjCorner[0] = m_stLearnDieCornerPos[PR_UPPER_LEFT];
			stSetCmd.acoObjCorner[1] = m_stLearnDieCornerPos[PR_LOWER_RIGHT];
		}
		else if (GetDieShape() == WPR_TRIANGULAR_DIE)		//v4.06
		{
			stSetCmd.emShape = PR_SHAPE_TYPE_POLYGON;
			stSetCmd.uwNumOfCorners = WPR_TRIANGLE_CORNERS;
			stSetCmd.acoObjCorner[0] = m_stLearnDieCornerPos[WPR_DIE_CORNER_0];
			stSetCmd.acoObjCorner[1] = m_stLearnDieCornerPos[WPR_DIE_CORNER_1];
			stSetCmd.acoObjCorner[2] = m_stLearnDieCornerPos[WPR_DIE_CORNER_2];
		}
		else if (GetDieShape() == WPR_RHOMBUS_DIE)
		{
			stSetCmd.emShape = PR_SHAPE_TYPE_POLYGON;
			stSetCmd.uwNumOfCorners = WPR_RHOMBUS_CORNERS;
			stSetCmd.acoObjCorner[0] = m_stLearnDieCornerPos[WPR_DIE_CORNER_0];
			stSetCmd.acoObjCorner[1] = m_stLearnDieCornerPos[WPR_DIE_CORNER_1];
			stSetCmd.acoObjCorner[2] = m_stLearnDieCornerPos[WPR_DIE_CORNER_2];
			stSetCmd.acoObjCorner[3] = m_stLearnDieCornerPos[WPR_DIE_CORNER_3];
			CString szMsg = "Use PR Mouse ";
			CString szTemp;
			for(int i=0; i<4; i++)
			{
				szTemp.Format("%d,%d ", stSetCmd.acoObjCorner[i].x, stSetCmd.acoObjCorner[i].y);
				szMsg += szTemp;
			}
			CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szMsg);
		}
		else
		{
			stSetCmd.emShape = PR_SHAPE_TYPE_POLYGON;
			stSetCmd.uwNumOfCorners = WPR_HEXAGON_CORNERS;
			stSetCmd.acoObjCorner[0] = m_stLearnDieCornerPos[WPR_DIE_CORNER_0];
			stSetCmd.acoObjCorner[1] = m_stLearnDieCornerPos[WPR_DIE_CORNER_1];
			stSetCmd.acoObjCorner[2] = m_stLearnDieCornerPos[WPR_DIE_CORNER_2];
			stSetCmd.acoObjCorner[3] = m_stLearnDieCornerPos[WPR_DIE_CORNER_3];
			stSetCmd.acoObjCorner[4] = m_stLearnDieCornerPos[WPR_DIE_CORNER_4];
			stSetCmd.acoObjCorner[5] = m_stLearnDieCornerPos[WPR_DIE_CORNER_5];
		}
			
		stSetCmd.emColor = PR_COLOR_RED;
		PR_SetMouseCtrlObjCmd(&stSetCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stSetRpy);

		m_bUseMouse = TRUE;
	}
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::DrawComplete(IPC_CServiceMessage &svMsg)
{
	EnableMouseClickCallbackCmd(TRUE);

	if (m_bUseMouse == TRUE)
	{
		// Get the mouse defined region
		PR_COORD acoObjCorner[PR_MAX_NO_OF_SHAPE_CORNER];

		LONG uwNumOfCorners = m_pPrGeneral->MouseGetAllCorners(acoObjCorner);
		if (uwNumOfCorners == 2)
		{
			m_stLearnDieCornerPos[PR_UPPER_LEFT]	= acoObjCorner[0];
			m_stLearnDieCornerPos[PR_LOWER_RIGHT]	= acoObjCorner[1];
		}
		else if (uwNumOfCorners == WPR_TRIANGLE_CORNERS)	//v4.06
		{
			m_stLearnDieCornerPos[WPR_DIE_CORNER_0]	= acoObjCorner[0];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_1] = acoObjCorner[1];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_2] = acoObjCorner[2];
		}
		else if (uwNumOfCorners == WPR_RHOMBUS_CORNERS)
		{
			m_stLearnDieCornerPos[WPR_DIE_CORNER_0]	= acoObjCorner[0];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_1] = acoObjCorner[1];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_2] = acoObjCorner[2];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_3] = acoObjCorner[3];
			CString szMsg = "Draw Complete ";
			CString szTemp;
			for(int i=0; i<4; i++)
			{
				szTemp.Format("%d,%d ", acoObjCorner[i].x, acoObjCorner[i].y);
				szMsg += szTemp;
			}
			CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szMsg);
		}
		else if (uwNumOfCorners == WPR_HEXAGON_CORNERS)
		{
			m_stLearnDieCornerPos[WPR_DIE_CORNER_0]	= acoObjCorner[0];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_1] = acoObjCorner[1];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_2] = acoObjCorner[2];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_3] = acoObjCorner[3];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_4] = acoObjCorner[4];
			m_stLearnDieCornerPos[WPR_DIE_CORNER_5] = acoObjCorner[5];
		}

		// Draw the defined region
		if (uwNumOfCorners == 2)
		{
			DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT], PR_UPPER_LEFT, 1);
			DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 1);
		}
		else if (uwNumOfCorners == WPR_TRIANGLE_CORNERS)	//v4.05
		{
			DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[0], 1);
		}
		else if (uwNumOfCorners == WPR_RHOMBUS_CORNERS)
		{
			DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[0], 1);
		}
		else if (uwNumOfCorners == WPR_HEXAGON_CORNERS)
		{
			DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[4], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[4], m_stLearnDieCornerPos[5], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[5], m_stLearnDieCornerPos[0], 1);
		}

		m_bUseMouse = FALSE;
	}
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//Set Inspection Detect / Ignore region
LONG CWaferPr::SetInspDetectRegion(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lRegionNo = 0;


	//Get Region No from HMI
	svMsg.GetMsg(sizeof(LONG), &lRegionNo);
	if (lRegionNo == 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bUseMouse == FALSE)
	{
		// Erase the original region
		DrawAndEraseCursor(m_stInspDetectWin[lRegionNo - 1].coCorner1, PR_UPPER_LEFT, 0);
		DrawAndEraseCursor(m_stInspDetectWin[lRegionNo - 1].coCorner2, PR_LOWER_RIGHT, 0);

		// Define by using mouse
		m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_YELLOW);

		// Set the mouse size first
		m_pPrGeneral->MouseSet2PointRegion(m_stInspDetectWin[lRegionNo - 1].coCorner1, m_stInspDetectWin[lRegionNo - 1].coCorner2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_YELLOW);

		m_bUseMouse = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::EndInspDetectRegion(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lMaxRegion;
		LONG	lRegionNo;
	} REG_TYPE;

	REG_TYPE	stInfo;


	//Get Region No from HMI
	svMsg.GetMsg(sizeof(REG_TYPE), &stInfo);
	if (stInfo.lRegionNo == 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if (m_bUseMouse == TRUE)
	{
		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stInspDetectWin[stInfo.lRegionNo - 1].coCorner1,
														   m_stInspDetectWin[stInfo.lRegionNo - 1].coCorner2);
		if (uwNumOfCorners == 2)
		{
			// Draw the defined region
			DrawRectangleBox(m_stInspDetectWin[stInfo.lRegionNo - 1].coCorner1, m_stInspDetectWin[stInfo.lRegionNo - 1].coCorner2, PR_COLOR_YELLOW);
		}

		m_bUseMouse = FALSE;
	}

	m_lInspDetectWinNo = stInfo.lMaxRegion;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::SetInspIgnoreRegion(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lRegionNo = 0;


	//Get Region No from HMI
	svMsg.GetMsg(sizeof(LONG), &lRegionNo);
	if (lRegionNo == 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if (m_bUseMouse == FALSE)
	{
		// Erase the original region
		DrawAndEraseCursor(m_stInspIgnoreWin[lRegionNo - 1].coCorner1, PR_UPPER_LEFT, 0);
		DrawAndEraseCursor(m_stInspIgnoreWin[lRegionNo - 1].coCorner2, PR_LOWER_RIGHT, 0);

		// Define by using mouse
		m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_CYAN);

		// Set the mouse size first
		m_pPrGeneral->MouseSet2PointRegion(m_stInspIgnoreWin[lRegionNo - 1].coCorner1, m_stInspIgnoreWin[lRegionNo - 1].coCorner2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_CYAN);

		m_bUseMouse = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::EndInspIgnoreRegion(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lMaxRegion;
		LONG	lRegionNo;
	} REG_TYPE;

	REG_TYPE	stInfo;

	//Get Region No from HMI
	svMsg.GetMsg(sizeof(REG_TYPE), &stInfo);
	if (stInfo.lRegionNo == 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if (m_bUseMouse == TRUE)
	{
		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stInspIgnoreWin[stInfo.lRegionNo - 1].coCorner1,
														   m_stInspIgnoreWin[stInfo.lRegionNo - 1].coCorner2);
		if (uwNumOfCorners == 2)
		{
			// Draw the defined region
			DrawRectangleBox(m_stInspIgnoreWin[stInfo.lRegionNo - 1].coCorner1, m_stInspIgnoreWin[stInfo.lRegionNo - 1].coCorner2, PR_COLOR_CYAN);
		}
		m_bUseMouse = FALSE;
	}

	m_lInspIgnoreWinNo = stInfo.lMaxRegion;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//Set Alignment Detect / Ignore region
LONG CWaferPr::SetAlnDetectRegion(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lRegionNo = 0;


	//Get Region No from HMI
	svMsg.GetMsg(sizeof(LONG), &lRegionNo);
	if (lRegionNo == 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bUseMouse == FALSE)
	{
		// Erase the original region
		DrawAndEraseCursor(m_stAlnDetectWin[lRegionNo - 1].coCorner1, PR_UPPER_LEFT, 0);
		DrawAndEraseCursor(m_stAlnDetectWin[lRegionNo - 1].coCorner2, PR_LOWER_RIGHT, 0);

		// Define by using mouse
		m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_BLUE);

		// Set the mouse size first
		m_pPrGeneral->MouseSet2PointRegion(m_stAlnDetectWin[lRegionNo - 1].coCorner1, m_stAlnDetectWin[lRegionNo - 1].coCorner2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_BLUE);

		m_bUseMouse = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::EndAlnDetectRegion(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lMaxRegion;
		LONG	lRegionNo;
	} REG_TYPE;

	REG_TYPE	stInfo;


	//Get Region No from HMI
	svMsg.GetMsg(sizeof(REG_TYPE), &stInfo);
	if (stInfo.lRegionNo == 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if (m_bUseMouse == TRUE)
	{
		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stAlnDetectWin[stInfo.lRegionNo - 1].coCorner1,
														   m_stAlnDetectWin[stInfo.lRegionNo - 1].coCorner2);

		if (uwNumOfCorners == 2)
		{
			// Draw the defined region
			DrawRectangleBox(m_stAlnDetectWin[stInfo.lRegionNo - 1].coCorner1, m_stAlnDetectWin[stInfo.lRegionNo - 1].coCorner2, PR_COLOR_BLUE);
		}
		m_bUseMouse = FALSE;
	}

	m_lAlnDetectWinNo = stInfo.lMaxRegion;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::SetAlnIgnoreRegion(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lRegionNo = 0;


	//Get Region No from HMI
	svMsg.GetMsg(sizeof(LONG), &lRegionNo);
	if (lRegionNo == 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if (m_bUseMouse == FALSE)
	{
		// Erase the original region
		DrawAndEraseCursor(m_stAlnIgnoreWin[lRegionNo - 1].coCorner1, PR_UPPER_LEFT, 0);
		DrawAndEraseCursor(m_stAlnIgnoreWin[lRegionNo - 1].coCorner2, PR_LOWER_RIGHT, 0);

		// Define by using mouse
		m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_MAGENTA);

		// Set the mouse size first
		m_pPrGeneral->MouseSet2PointRegion(m_stAlnIgnoreWin[lRegionNo - 1].coCorner1, m_stAlnIgnoreWin[lRegionNo - 1].coCorner2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_MAGENTA);

		m_bUseMouse = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::EndAlnIgnoreRegion(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lMaxRegion;
		LONG	lRegionNo;
	} REG_TYPE;

	REG_TYPE	stInfo;

	//Get Region No from HMI
	svMsg.GetMsg(sizeof(REG_TYPE), &stInfo);
	if (stInfo.lRegionNo == 0)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if (m_bUseMouse == TRUE)
	{
		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stAlnIgnoreWin[stInfo.lRegionNo - 1].coCorner1,
														   m_stAlnIgnoreWin[stInfo.lRegionNo - 1].coCorner2);

		if (uwNumOfCorners == 2)
		{
			// Draw the defined region
			DrawRectangleBox(m_stAlnIgnoreWin[stInfo.lRegionNo - 1].coCorner1, m_stAlnIgnoreWin[stInfo.lRegionNo - 1].coCorner2, PR_COLOR_MAGENTA);
		}

		m_bUseMouse = FALSE;
	}

	m_lAlnIgnoreWinNo = stInfo.lMaxRegion;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::CancelMouseDraw(IPC_CServiceMessage &svMsg)
{
	if (m_bUseMouse == TRUE)
	{
		// Erase the mouse object
		m_pPrGeneral->EraseMouseCtrlObj();

		m_bUseMouse = FALSE;
	}
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// --- Upload and Download WPR Records ---
LONG CWaferPr::UploadPrRecords(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG i;
	CString szLog;
	CString szPath;
	CHAR acFilename[PR_MAX_FILE_PATH_LENGTH];
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	if (m_fHardware)
	{
		szPath = (*m_psmfSRam)["PR"]["RecordPath"];

		// Upload the normal die and reference die (assume index 0 is normal die)
		for (i = 0; i < WPR_MAX_DIE; i++)
		{
			if (GetDiePrID((UCHAR)i) == 0)
			{
				continue;
			}

			sprintf_s((char*)acFilename, sizeof(acFilename), "%s\\WPR%ld.rec", (LPCTSTR) szPath, i);		//Klocwork
			CString szErrMsg;
			PR_UWORD uwReturn = m_pPrGeneral->UploadPRRecord(GetDiePrID((UCHAR)i), acFilename, ubSID, ubRID, szErrMsg);

			if (uwReturn == PR_ERR_NO_PIDREC)		// Given record ID not exist
			{
				continue;
			}

			if (uwReturn != PR_ERR_NOERR)
			{
				HmiMessage(szErrMsg);	//v3.06
				bReturn = FALSE;
				break;
			}

			szLog.Format("WPR: Upload Record #%d, Channel=0x%x, RecordID=%d, Path=%s", 
						i, ubSID, GetDiePrID((UCHAR)i), acFilename);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);	//v4.53A22
		}
	}

	(*m_psmfSRam)["WaferPr"]["NumOfRecords"] = m_lLrnTotalRefDie;

	if( IsAOIOnlyMachine() )
	{
		CONST CString szExePath = gszROOT_DIRECTORY + "\\EXE\\";
		CString szTgtFile = szExePath + "WaferPr_" + m_szPkgNameCurrent + ".msd";
		DeleteFile(szTgtFile);
		szTgtFile = szExePath + "WaferTable_" + m_szPkgNameCurrent + ".msd";
		DeleteFile(szTgtFile);
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		CString szCurrPkgName = pApp->GetPKGFilename();
		m_szPkgNameCurrent	= szCurrPkgName;
		for (int i = 0; i < WPR_MAX_DIE ; i++)
		{
			m_saPkgRecordCurrent[i] = m_ssGenPRSrchID[i];
		}

		CString szSrcFile = szExePath + "WaferPr.msd";
		szTgtFile = szExePath + "WaferPr_" + szCurrPkgName + ".msd";
		CopyFileWithRetry(szSrcFile, szTgtFile, FALSE);
		szSrcFile = szExePath + "WaferTable.msd";
		szTgtFile = szExePath + "WaferTable_" + szCurrPkgName + ".msd";
		CopyFileWithRetry(szSrcFile, szTgtFile, FALSE);
		CString szLog;
		szLog.Format("Save PKG file, curr %s, prev %s", m_szPkgNameCurrent, m_szPkgNamePrevious);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

		SaveRuntimeTwoRecords();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::DownloadPrRecords(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG i = 0;
	CHAR acTemp[PR_MAX_FILE_NAME_LENGTH];
	//FILE *pFile;
	BOOL bDownloadFile = FALSE;
	CStdioFile cfFile;
	CString csMsg, szPath;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	BOOL bHmiDL = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bHmiDL);
	//bHmiDL = TRUE;
	//	not load and not save, just down load PR records, as the status is in PKG file msd
	if( bHmiDL )
	{
		if (IsLoadingPortablePKGFile() == TRUE && IsLoadingPKGFile() == TRUE)
		{
			LoadWaferPrOption();
		}
		else if (CMS896AStn::m_bNoPRRecordForPKGFile == TRUE && IsLoadingPKGFile() == TRUE)
		{
		}
		else
		{
			LoadWaferPrOption();
			LoadPrData();
		}
	}

	if (m_fHardware)
	{
		SetAlarmLog("WPR DL records, free all before download new");
		szPath = (*m_psmfSRam)["PR"]["RecordPath"];

		FreeAllPrRecords(FALSE);		// Free all PR records first when download
		//boolean = false
		m_bIsAutoLearnWaferNOTDone = FALSE;
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		pApp->WriteProfileInt(gszPROFILE_SETTING, _T("ES101AutoLearnWaferNotDone"), FALSE);
		// Download the normal die and reference die (assume index 0 is normal die)
		csMsg.Format("WPR DL Records, before total refer is %d", m_lLrnTotalRefDie);
		SetAlarmLog(csMsg);
		CString szLog = _T("WPR: Down Load All PrRecords - ");
		CString szValue;
		CString szTemp, szIDValue, szLog2;
		CString szIDLog = _T("WPR: DownloadPrRecords - ");


		for (i = 0; i < WPR_MAX_DIE; i++)	//v3.06
		{
			szValue = "Unknown ";
			if( i<3 )
			{
				szValue.Format("Normal %d", i+1);
			}
			else if( i>=3 && i<WPR_GEN_OCR_DIE_START )
			{
				szValue.Format("Refer  %d", 1);
			}
			else if( i>=WPR_GEN_OCR_DIE_START && i<WPR_GEN_OCR_DIE_START+10 )
			{
				szValue.Format("Number %c", '0' + i - WPR_GEN_OCR_DIE_START);
			}
			else if( i>=(WPR_GEN_OCR_DIE_START+10) )
			{
				szValue.Format("Letter %c", 'A' + i - (WPR_GEN_OCR_DIE_START+10));
			}

			if( i<10 )
				csMsg.Format("WPR%ld.rec ", i);
			else
				csMsg.Format("WPR%ld.rec", i);
			szTemp.Format("WPR DL Records, %s %s learnt %d, record id %4d",
					csMsg, szValue, m_bGenDieLearnt[i], GetDiePrID((UCHAR)i));
			m_bGenDieLearnt[i] = FALSE;
			bDownloadFile = FALSE;

			sprintf_s(acTemp, sizeof(acTemp), "%s\\WPR%ld.rec", (LPCTSTR) szPath, i);	//Klocwork

			if (cfFile.Open(acTemp, CFile::modeRead) == TRUE)
			{
				if (cfFile.GetLength() == 0)
				{
					cfFile.Close();
					continue;
				}
			}
			else
			{
				continue;
			}
			cfFile.Close();
			// Check whether the file is existing

			PR_ULWORD ulRecordID = 0;
			if (IsOcrAOIMode() && IsCharDieInUse())
			{
				ulRecordID = i + 1;
			}

			szValue.Format(" pass id %4d ", ulRecordID);
			szTemp = szTemp + szValue;

			CString szErrMsg;
			PR_ULWORD ulRetRecordID = m_pPrGeneral->DownloadPRRecord(ulRecordID, acTemp, ubSID, ubRID, szErrMsg);
			if (ulRetRecordID == 0)
			{
				HmiMessage_Red_Back(szErrMsg);
				SetAlarmLog(szTemp + szErrMsg);
				bReturn = FALSE;
				break;
			}

			m_ssGenPRSrchID[i] = (PR_WORD)ulRetRecordID;
			m_bGenDieLearnt[i] = TRUE;	
			if (i == 0 && m_bGenDieLearnt[i] == TRUE)
			{
				m_bDieIsLearnt = TRUE;
			}

			//v4.53A22
			szLog2.Format("WPR: Download Record #%d, Channel=0x%x, RecordID=%d, RetID=%d", 
						i, ubSID, ulRecordID, ulRetRecordID);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog2);
			//v4.53A22
			szIDValue.Format("%ld(ID=%d) ", i, m_ssGenPRSrchID[i]);
			szIDLog = szIDLog + szIDValue;

			szValue.Format("%2ld(ID=%4ld) ", i, GetDiePrID((UCHAR)i));
			szLog = szLog + szValue;
			szTemp = szTemp + "==>" + szValue;
			SetAlarmLog(szTemp);
		}

		if (i == 0)
		{
			HmiMessage("No record file downloaded!");
			bReturn = FALSE;
		}

		CMSLogFileUtility::Instance()->MS_LogOperation(szIDLog);		//v4.53A22
	}

	m_lLrnTotalRefDie = 0;
	for (i = WPR_GEN_RDIE_OFFSET + 1; i < WPR_MAX_DIE; i++)
	{
		if (IsThisDieLearnt((UCHAR)i) == TRUE)
		{
			m_lLrnTotalRefDie++;
		}
	}

	if( m_bIsAutoLearnWaferNOTDone )
	{
		HmiMessage("PR record come from other machine, Press \"Auto-Learn Wafer\" button!");
	}

	csMsg.Format("WPR DL Records, after total refer is %d", m_lLrnTotalRefDie);
	SetAlarmLog(csMsg);

	UpdateWPRInfoList();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	//	not load and not save, just down load PR records, as the status is in PKG file msd
	if( bHmiDL )
	{
		SavePrData(FALSE);
	}

	return 1;
}

LONG CWaferPr::DeletePrRecords(IPC_CServiceMessage &svMsg)
{
	LONG i;
	CString szTemp, szPath;

	szPath = (*m_psmfSRam)["PR"]["RecordPath"];

	// Delete the uploaded records
	for (i = 0; i <= WPR_MAX_DIE; i++)
	{
		szTemp.Format("%s\\WPR%d.rec", szPath, i);		//v3.19T1
		DeleteFile(szTemp);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::UploadPrPkgPreviewImage(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szImageDestPath;
	LPTSTR lpsz = new TCHAR[255];
	svMsg.GetMsg(sizeof(TCHAR) * 255, lpsz);

	szImageDestPath = lpsz;
	bReturn = UploadPrPkgPreviewImage(szImageDestPath , WPR_NORMAL_DIE, 1);

	delete[] lpsz;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::BadCutSetupStart(IPC_CServiceMessage &svMsg)
{
	PR_SET_MOUSE_CTRL_OBJ_CMD		stSetCmd;
	PR_SET_MOUSE_CTRL_OBJ_RPY		stSetRpy;

	m_dSetRegionByDieSizeXCopy = m_dSetRegionByDieSizeX;
	m_dSetRegionByDieSizeYCopy = m_dSetRegionByDieSizeY;

	if (m_bUseMouse == FALSE)
	{
		// Define by using mouse
		PR_SHAPE_TYPE emShape = PR_SHAPE_TYPE_RECTANGLE;
		if (GetDieShape() == WPR_RECTANGLE_DIE)
		{
			emShape = PR_SHAPE_TYPE_RECTANGLE;	
		}
		else
		{
			emShape = PR_SHAPE_TYPE_POLYGON;	
		}
		m_pPrGeneral->MouseDefineRegion(emShape, PR_COLOR_YELLOW);

		// Set the Region 
		PR_InitSetMouseCtrlObjCmd(&stSetCmd);
		stSetCmd.ulObjectId = 1;
		stSetCmd.emColor = PR_COLOR_YELLOW;
		if (GetDieShape() == WPR_RECTANGLE_DIE)
		{
			stSetCmd.uwNumOfCorners = 2;
			stSetCmd.emShape = PR_SHAPE_TYPE_RECTANGLE;	

			stSetCmd.acoObjCorner[0].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[0].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
			stSetCmd.acoObjCorner[1].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[1].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
			

			PR_COORD PrevDieSize;
			PrevDieSize.x = abs(m_stBadCutPoint[1].x - m_stBadCutPoint[0].x);
			PrevDieSize.y = abs(m_stBadCutPoint[1].y - m_stBadCutPoint[0].y);
			
			// Checking the previous badcut die size is within the limit, if true, use the limit
			if ((PrevDieSize.x > GetNmlSizePixelX() / 2)
					&& (PrevDieSize.x < GetNmlSizePixelX() * 2.5)
					&& (PrevDieSize.y > GetNmlSizePixelY() / 2)
					&& (PrevDieSize.y < GetNmlSizePixelY() * 2.5))
			{
				stSetCmd.acoObjCorner[0] = m_stBadCutPoint[0];
				stSetCmd.acoObjCorner[1] = m_stBadCutPoint[1];
			}
		}
		else if (GetDieShape() == WPR_TRIANGULAR_DIE)	//v4.06
		{
			PR_COORD Center, stPtOnNormalPolygon;
			DOUBLE dNormalPolygonLength = 0;
			BOOL bInitWithPrevRecord = TRUE;

			stSetCmd.uwNumOfCorners = WPR_TRIANGLE_CORNERS;
			stSetCmd.emShape = PR_SHAPE_TYPE_POLYGON;	

			stSetCmd.acoObjCorner[0].x = (PR_WORD)GetPrCenterX();
			stSetCmd.acoObjCorner[0].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
			stSetCmd.acoObjCorner[1].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[1].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
			stSetCmd.acoObjCorner[2].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[2].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
			
			Center.x = (PR_WORD)GetPrCenterX();
			Center.y = (PR_WORD)GetPrCenterY();

			stPtOnNormalPolygon.x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stPtOnNormalPolygon.y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);

			// Find the length from the center to one of the point of the polygon
			dNormalPolygonLength = FindLength(Center, stPtOnNormalPolygon);
			
			// check the length of the polygon from center is within the limit. if true, recover the record
			for (INT i = 0; i < WPR_TRIANGLE_CORNERS; i++)
			{
				if (FindLength(m_stBadCutPoint[i], Center) < dNormalPolygonLength / 2 
						|| FindLength(m_stBadCutPoint[i], Center) > dNormalPolygonLength * 2)
				{
					bInitWithPrevRecord = FALSE;
					break;
				}
			}

			if (bInitWithPrevRecord == TRUE)
			{
				for (INT i = 0; i < WPR_TRIANGLE_CORNERS; i++)
				{
					stSetCmd.acoObjCorner[i].x = m_stBadCutPoint[i].x;
					stSetCmd.acoObjCorner[i].y = m_stBadCutPoint[i].y;
				}
			}
		}
		else if (GetDieShape() == WPR_RHOMBUS_DIE)
		{
			PR_COORD Center, stPtOnNormalPolygon;
			DOUBLE dNormalPolygonLength = 0;
			BOOL bInitWithPrevRecord = TRUE;

			stSetCmd.uwNumOfCorners = WPR_RHOMBUS_CORNERS;
			stSetCmd.emShape = PR_SHAPE_TYPE_POLYGON;	

			stSetCmd.acoObjCorner[0].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[0].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
			stSetCmd.acoObjCorner[1].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[1].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
			stSetCmd.acoObjCorner[2].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[2].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
			stSetCmd.acoObjCorner[3].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[3].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
			
			Center.x = (PR_WORD)GetPrCenterX();
			Center.y = (PR_WORD)GetPrCenterY();

			stPtOnNormalPolygon.x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stPtOnNormalPolygon.y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);

			// Find the length from the center to one of the point of the polygon
			dNormalPolygonLength = FindLength(Center, stPtOnNormalPolygon);
			
			// check the length of the polygon from center is within the limit. if true, recover the record
			for (INT i = 0; i < WPR_RHOMBUS_CORNERS; i++)
			{
				if (FindLength(m_stBadCutPoint[i], Center) < dNormalPolygonLength / 2 
						|| FindLength(m_stBadCutPoint[i], Center) > dNormalPolygonLength * 2)
				{
					bInitWithPrevRecord = FALSE;
					break;
				}
			}

			if (bInitWithPrevRecord == TRUE)
			{
				for (INT i = 0; i < WPR_RHOMBUS_CORNERS; i++)
				{
					stSetCmd.acoObjCorner[i].x = m_stBadCutPoint[i].x;
					stSetCmd.acoObjCorner[i].y = m_stBadCutPoint[i].y;
				}
			}
		}	//	rhombus die
		else
		{
			PR_COORD Center, stPtOnNormalPolygon;
			DOUBLE dNormalPolygonLength = 0;
			BOOL bInitWithPrevRecord = TRUE;

			stSetCmd.uwNumOfCorners = WPR_HEXAGON_CORNERS;
			stSetCmd.emShape = PR_SHAPE_TYPE_POLYGON;	

			stSetCmd.acoObjCorner[0].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[0].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
			stSetCmd.acoObjCorner[1].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[1].y = (PR_WORD)GetPrCenterY();
			stSetCmd.acoObjCorner[2].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[2].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
			stSetCmd.acoObjCorner[3].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[3].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
			stSetCmd.acoObjCorner[4].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[4].y = (PR_WORD)GetPrCenterY();
			stSetCmd.acoObjCorner[5].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
			stSetCmd.acoObjCorner[5].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);

			
			Center.x = (PR_WORD)GetPrCenterX();
			Center.y = (PR_WORD)GetPrCenterY();
			stPtOnNormalPolygon.x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
			stPtOnNormalPolygon.y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);

			// Find the length from the center to one of the point of the polygon
			dNormalPolygonLength = FindLength(Center, stPtOnNormalPolygon);
			
			// check the length of the polygon from center is within the limit. if true, recover the record
			for (INT i = 0; i < 6; i++)
			{
				if (FindLength(m_stBadCutPoint[i], Center) < dNormalPolygonLength / 2 
						|| FindLength(m_stBadCutPoint[i], Center) > dNormalPolygonLength * 2)
				{
					bInitWithPrevRecord = FALSE;
					break;
				}
			}

			if (bInitWithPrevRecord == TRUE)
			{
				for (INT i = 0; i < 6; i++)
				{
					stSetCmd.acoObjCorner[i].x = m_stBadCutPoint[i].x;
					stSetCmd.acoObjCorner[i].y = m_stBadCutPoint[i].y;
				}
			}
			
		}

		PR_SetMouseCtrlObjCmd(&stSetCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stSetRpy);

		m_bUseMouse = TRUE;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::BadCutSetupStop(IPC_CServiceMessage &svMsg)
{
	if (m_bUseMouse == TRUE)
	{
		// Get the mouse defined region
		PR_COORD acoObjCorner[PR_MAX_NO_OF_SHAPE_CORNER];

		LONG uwNumOfCorners = m_pPrGeneral->MouseGetAllCorners(acoObjCorner);
		if (uwNumOfCorners == 2)
		{
			m_stBadCutPoint[0] = acoObjCorner[0];
			m_stBadCutPoint[1] = acoObjCorner[1];
		}
		else if (uwNumOfCorners == WPR_TRIANGLE_CORNERS)		//v4.06
		{
			m_stBadCutPoint[0] = acoObjCorner[0];
			m_stBadCutPoint[1] = acoObjCorner[1];
			m_stBadCutPoint[2] = acoObjCorner[2];
		}
		else if (uwNumOfCorners == WPR_RHOMBUS_CORNERS)
		{
			m_stBadCutPoint[0] = acoObjCorner[0];
			m_stBadCutPoint[1] = acoObjCorner[1];
			m_stBadCutPoint[2] = acoObjCorner[2];
			m_stBadCutPoint[3] = acoObjCorner[3];
		}
		else if (uwNumOfCorners == WPR_HEXAGON_CORNERS)
		{
			m_stBadCutPoint[0] = acoObjCorner[0];
			m_stBadCutPoint[1] = acoObjCorner[1];
			m_stBadCutPoint[2] = acoObjCorner[2];
			m_stBadCutPoint[3] = acoObjCorner[3];
			m_stBadCutPoint[4] = acoObjCorner[4];
			m_stBadCutPoint[5] = acoObjCorner[5];
		}

		m_bUseMouse = FALSE;
	}
	else
	{
		for (int i = 0; i < m_nPolygonSize; i++)
		{
			m_stBadCutPoint[i] = m_stNewBadCutPoint[i] ;
		}
	}

	SavePrData(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//Functions for doing wafer alignment in waferloader machine
LONG CWaferPr::FindFirstDieInWafer(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FindOneDieInWafer();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferPr::FindOneDieInWafer()
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	PR_COORD	stSrchCorner1;
	PR_COORD	stSrchCorner2;

	if (PR_NotInit())
	{
		return FALSE;
	}

	if( IsBurnIn() )
	{
		return TRUE;
	}

	SaveScanTimeEvent("WPR: Find first Die in wafer begin.");
	stSrchCorner1.x = GetPRWinULX();
	stSrchCorner1.y = GetPRWinULY();
	stSrchCorner2.x = GetPRWinLRX();
	stSrchCorner2.y = GetPRWinLRY();

	//Set & Display Max search die area
	DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_RED);

	//Do search
	LONG lResult;
	BOOL bDieFound;

	bDieFound = FALSE;

	
	lResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);

	if (lResult == -1)
	{
		SetAlert(IDS_WPR_ALIGNMENT_FAILED);
		DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
		return FALSE;
	}
	
	if (DieIsAlignable(usDieType) == TRUE)
	{
		bDieFound = TRUE;
	}
	else		//v3.02T6
	{
		bDieFound = SearchFirstDieInWafer();

		//V4.15T1
		if (bDieFound)
		{
			lResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);

			if (lResult == -1)
			{
				SetAlert(IDS_WPR_ALIGNMENT_FAILED);
				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				return FALSE;
			}
			
			if (DieIsAlignable(usDieType) == TRUE)
			{
				bDieFound = TRUE;
			}
		}
	}

	//v2.82T5	//disable auto-learn-pitch fcn for DLA
	if (bDieFound == TRUE)
	{
		SaveScanTimeEvent("WPR: Find first Die in wafer, begin do compensation.");
		//Allow die to rotate
		ManualDieCompenate(stDieOffset, fDieRotate);
	}
	DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);

	return bDieFound;
}


LONG CWaferPr::FindFirstDieAngleInWafer(IPC_CServiceMessage &svMsg)	
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	BOOL			bJustCheck = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bJustCheck);

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_UWORD uwResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);

	if ((uwResult == -1) || !DieIsAlignable(usDieType))
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	// for u2u map file, if > 2 degree, return FALSE;

	if (bJustCheck == TRUE)
	{
		bReturn = TRUE;

		if (fabs(fDieRotate) > 2)
		{
			bReturn = FALSE;
		}
		
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//No need to rotate if < 0.5 degree
	if (fabs(fDieRotate) <= 0.5)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	ManualDieCompenate_Rotate(stDieOffset, fDieRotate);
	Sleep(200);

	//Perform SearchDie again to double check angle
	uwResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);

	if ((uwResult == -1) || !DieIsAlignable(usDieType))
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Return failure if angle still OUT
	if (fabs(fDieRotate) > 0.5)
	{
		//HmiMessage("Wafer Alignment ERROR: global angle is out!");
		SetErrorMessage("WaferPR: global angle is out!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::AlignWaferAngle(IPC_CServiceMessage &svMsg)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

	int			siTempX = 0;
	int			siTempY = 0;
	int			siTempT = 0;
	int			siLHSDieX = 0;
	int			siLHSDieY = 0;
	int			siRHSDieX = 0;
	int			siRHSDieY = 0;
	int			siOrigX;
	int			siOrigY;
	int			siOrigT;
	LONG		i;
	LONG		lLHSIndexCount, lRHSIndexCount;
	LONG		lAlignMode;

	DOUBLE		dThetaAngle	= 0.0;
	DOUBLE		dDiffX	= 0.0;
	DOUBLE		dDiffY	= 0.0;
	
	CString		szErr;
	BOOL bReturn = FALSE;
	PR_WORD wResult = 0;

	typedef struct 
	{
		LONG	lLHS;
		LONG	lRHS;	
		LONG	lAlignMode;		// 0=Horiz, 1=Vert		//v3.02T6
	} ALNSETUP;

	ALNSETUP stInfo;
	svMsg.GetMsg(sizeof(ALNSETUP), &stInfo);

	if (PR_NotInit())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Align Wafer Angle PR not init");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsBurnIn() )
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BOOL	bResult = FALSE;	
	lLHSIndexCount	= stInfo.lLHS;
	lRHSIndexCount = stInfo.lRHS;
	lAlignMode		= stInfo.lAlignMode;

	CMS896AApp::m_bStopAlign = FALSE;
	m_bFovFoundWaferAngle = FALSE;
	//Locate any die in the wafer table
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (IsPrescanEnable() && m_bFovToFindGT)
	{
		LONG lNewAlignMode = lAlignMode;
		if ((lLHSIndexCount == 0) && (lRHSIndexCount == 0))
		{
			lNewAlignMode = -1;
		}
		//Do spiral search
		SaveScanTimeEvent("WPR Spiral Search first normal die Start");
		PR_COORD	stSrchCorner1;
		PR_COORD	stSrchCorner2;
		stSrchCorner1.x = GetPRWinULX();
		stSrchCorner1.y = GetPRWinULY();
		stSrchCorner2.x = GetPRWinLRX();
		stSrchCorner2.y = GetPRWinLRY();
		//Diplay current search region
        bResult = SpiralSearchNormalDie(lNewAlignMode);	// for FOV find GT only
		if (bResult == FALSE)
		{
			SaveScanTimeEvent("WPR: spiral find normal die on wafer fails");
		}
		if (lNewAlignMode != -1)
		{
			lAlignMode = lNewAlignMode;
		}

		if ( m_bFovFoundWaferAngle && (m_bFovFindGTMove == FALSE) && bResult)
		{
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WPR Find 1st die on wafer");
		// Get the reply for the encoder value
		bResult = FindOneDieInWafer();

		DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
		DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
		DOUBLE dFov = min(dFOVX, dFOVY);
		if (IsAOIOnlyMachine() && dFov < 2.5 && bResult == FALSE)
		{
			GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
			for (int i = 0; i < 2; i++)
			{
				if (i == 0)
				{
					siOrigX = siOrigX + GetDiePitchX_X() / 2;
				}
				else
				{
					siOrigY = siOrigY + GetDiePitchY_Y() / 2;
				}
				MoveWaferTable(siOrigX, siOrigY);
				Sleep(100);
				bResult = FindOneDieInWafer();
				if (bResult)
				{
					break;
				}
			}
		}
	}

	if (bResult == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL: Find 1st die on wafer fails");
		SetErrorMessage("WL: Find 1st die on wafer fails");
		SetAlert_Red_Yellow(IDS_WL_NODIE_IN_WAFER);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//V4.33T1	//Osram Germany requires When Serach First die also correct T angle
	if (pApp->GetCustomerName() == "OSRAM")
	{
		Sleep(100);
		wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
		if ((wResult != -1) && DieIsAlignable(usDieType))
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Correct 1st die angle (OSRAM)");
			ManualDieCompenate_Rotate(stDieOffset, fDieRotate);
			Sleep(50);
		}

	}


	//Save original position
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
	siTempT = siOrigT;
	siLHSDieX = siRHSDieX = siTempX = siOrigX;
	siLHSDieY = siRHSDieY = siTempY = siOrigY;
	CString szMsg;
	szMsg.Format("Begin align wafer angle at %ld,%ld", siOrigX, siOrigY);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	//V2.83T4 (Use One die to do the Theta correction)
	if ((lLHSIndexCount == 0) && (lRHSIndexCount == 0))
	{
		wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);

		if (wResult == -1)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("search die error");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate_Rotate(stDieOffset, fDieRotate);
		}

		Sleep(10);
		CMSLogFileUtility::Instance()->WL_LogStatus("single current die angle");
	}
	else
	{
		for (i = 0; i < lLHSIndexCount; i++)
		{
			//Index table to next pitch on LEFT/TOP
			if (lAlignMode == 1)	//Vert
			{
				MoveWaferTable(siTempX + GetDiePitchYX(), siTempY + GetDiePitchYY());
			}
			else					//Horiz
			{
				MoveWaferTable(siTempX + GetDiePitchXX(), siTempY + GetDiePitchXY());
			}

			//Search die at curr position
			wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, 
									  &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
									  GetSrchArea().coCorner1, GetSrchArea().coCorner2);

			if (wResult == -1)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("PR error when find left hand wafer angle");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (DieIsAlignable(usDieType) == TRUE)
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
				Sleep(20);
				GetWaferTableEncoder(&siTempX, &siTempY, &siTempT);
				siLHSDieX = siTempX;
				siLHSDieY = siTempY;
			}
			else
			{
				GetWaferTableEncoder(&siTempX, &siTempY, &siTempT);		//v4.08
			}
			Sleep(50);
		}

		//Move to original position
		MoveWaferTable(siOrigX, siOrigY);
		siTempX = siOrigX;
		siTempY = siOrigY;
		Sleep(200);

		for (i = 0; i < lRHSIndexCount; i++)
		{
			//Index table to next pitch on right/DOWN
			if (lAlignMode == 1)	//Vert
			{
				MoveWaferTable(siTempX - GetDiePitchYX(), siTempY - GetDiePitchYY());
			}
			else					//Horiz
			{
				MoveWaferTable(siTempX - GetDiePitchXX(), siTempY - GetDiePitchXY());
			}

			wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);

			if (wResult == -1)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("PR error find right hand die wafer angle");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (DieIsAlignable(usDieType) == TRUE)
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
				Sleep(20);
				GetWaferTableEncoder(&siTempX, &siTempY, &siTempT);
				siRHSDieX = siTempX;
				siRHSDieY = siTempY;
			}
			else
			{
				GetWaferTableEncoder(&siTempX, &siTempY, &siTempT);		//v4.08
			}
			Sleep(50);
		}

		//Move to original position
		MoveWaferTable(siOrigX, siOrigY);
		Sleep(50);

		//Calcuate wafer angle	
		dDiffY = (double)(siLHSDieY - siRHSDieY);
		dDiffX = (double)(siLHSDieX - siRHSDieX);

		if (lAlignMode == 1)	//Vert
		{
			dThetaAngle = atan2(dDiffX * -1.00, dDiffY) * (180 / PI);
		}
		else
		{
			dThetaAngle = atan2(dDiffY , dDiffX) * (180 / PI);
		}

		WprCompensateRotateDie(siOrigX, siOrigY, dThetaAngle);
	}

	//Final check center-die angle to make sure global angle is within acceptance angle
	Sleep(500);
	wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
	if (wResult == -1)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("align wafer angle fail");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!DieIsAlignable(usDieType))
	{
		szErr.Format("WPR align global theta angle fails at CENTER-die (DieType = %0x%x)", usDieType);
		SetErrorMessage(szErr);
		CMSLogFileUtility::Instance()->WL_LogStatus(szErr);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//v4.33T1	//Osram 
	if (pApp->GetCustomerName() == "OSRAM")
	{
		BOOL bFail = FALSE;
		if (m_bThetaCorrection == TRUE) 
		{
			if (fabs(fDieRotate) > m_dMinDieAngle)
			{
				bFail = TRUE;
			}
		}
		else
		{
			if (fabs(fDieRotate) > m_dAcceptDieAngle)
			{
				bFail = TRUE;
			}
		}

		if (bFail == TRUE)
		{
			szErr.Format("WPR align global theta angle (Osram) fails at CENTER-die angle = %.2f (%.2f;%.2f)", fDieRotate, m_dMinDieAngle, m_dAcceptDieAngle);
			SetErrorMessage(szErr);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szErr);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}
	else
	{
		if (DieIsOutAngle(fDieRotate))
		{
			szErr.Format("WPR align global theta angle fails at CENTER-die angle = %.2f (%.2f;%.2f)", fDieRotate, m_dMaxDieAngle, m_dAcceptDieAngle);
			SetErrorMessage(szErr);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szErr);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	ManualDieCompenate(stDieOffset, fDieRotate);

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::SpiralSearchHomeDie(IPC_CServiceMessage &svMsg)
{
	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX() - 1, GetPRWinLRY() - 1};

	int			siOrigX;
	int			siOrigY;
	int			siOrigT;
	LONG		lUpIndex = 0;
	LONG		lDnIndex = 0;
	LONG		lLtIndex = 0;
	LONG		lRtIndex = 0;
	LONG		lMaxLoopCount = 0;
	LONG		lRow, lCol;
	LONG		lCurrentLoop = 1;
	LONG		lCurrentIndex = 0;
	LONG		lTotalIndex = 0;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	svMsg.GetMsg(sizeof(LONG), &lMaxLoopCount);

	lMaxLoopCount = lMaxLoopCount + 1;					//v4.04
	CMS896AApp::m_bStopAlign = FALSE;

	if (IsPrescanMapIndex() == FALSE)
	{
		if (((IsThisDieLearnt(WPR_GEN_RDIE_OFFSET + 1) == FALSE) || (GetDiePrID(WPR_GEN_RDIE_OFFSET + 1) <= 0)))
		{
			SetAlert(IDS_WPR_REFDIENOTLEARNT);
			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	DOUBLE dTime = GetTime();

	if (IsPrescanMapIndex() && (GetMapIndexStepCol() % 2 == 0 || GetMapIndexStepRow() % 2 == 0))
	{
		//Get current position
		GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
		LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
		if (GetMapIndexStepCol() % 2 == 0)
		{
			lGrabOffsetX = GetDiePitchX_X() / 2;
		}
		if (GetMapIndexStepRow() % 2 == 0)
		{
			lGrabOffsetY = GetDiePitchY_Y() / 2;
		}
		siOrigX = siOrigX + lGrabOffsetX;
		siOrigY = siOrigY + lGrabOffsetY;
		MoveWaferTable(siOrigX, siOrigY);
	}

	//Diplay current search region
	DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_YELLOW);

	//Get current position
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

	//Search from current position	
	if (IndexAndSearchRefHomeDie(&siOrigX, &siOrigY, WPR_CT_DIE) == TRUE)
	{
		DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
		BOOL bReturn = m_bHomeDieUniqueResult;

		CString szMsg;
		szMsg.Format("Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	lTotalIndex = 0;
	for (lCurrentLoop = 1; lCurrentLoop <= lMaxLoopCount; lCurrentLoop++)
	{
		lRow = (lCurrentLoop * 2 + 1);
		lCol = (lCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lTotalIndex++;
		lCurrentIndex = 1;

		if (IndexAndSearchRefHomeDie(&siOrigX, &siOrigY, WPR_RT_DIE) == TRUE)
		{
			DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
			CString szMsg;
			szMsg.Format("Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

			BOOL bReturn = m_bHomeDieUniqueResult;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		Sleep(10);

		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;
			lTotalIndex++;
			if (pApp->IsStopAlign())
			{
				CString szMsg;
				szMsg.Format("Stop Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (IndexAndSearchRefHomeDie(&siOrigX, &siOrigY, WPR_UP_DIE) == TRUE)
			{
				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				CString szMsg;
				szMsg.Format("Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				BOOL bReturn = m_bHomeDieUniqueResult;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}	
			Sleep(10);
		}			

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;
			lTotalIndex++;
			if (pApp->IsStopAlign())
			{
				CString szMsg;
				szMsg.Format("Stop Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (IndexAndSearchRefHomeDie(&siOrigX, &siOrigY, WPR_LT_DIE) == TRUE)
			{
				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				CString szMsg;
				szMsg.Format("Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				BOOL bReturn = m_bHomeDieUniqueResult;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}	
			Sleep(10);
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;
			lTotalIndex++;
			if (pApp->IsStopAlign())
			{
				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				CString szMsg;
				szMsg.Format("Stop Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				return 1;
			}

			if (IndexAndSearchRefHomeDie(&siOrigX, &siOrigY, WPR_DN_DIE) == TRUE)
			{
				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = m_bHomeDieUniqueResult;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				CString szMsg;
				szMsg.Format("Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				return 1;
			}	
			Sleep(10);
		}


		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;
			lTotalIndex++;
			if (pApp->IsStopAlign())
			{
				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				CString szMsg;
				szMsg.Format("Stop Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				return 1;
			}

			if (IndexAndSearchRefHomeDie(&siOrigX, &siOrigY, WPR_RT_DIE) == TRUE)
			{
				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = m_bHomeDieUniqueResult;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				CString szMsg;
				szMsg.Format("Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				return 1;
			}
			Sleep(10);
		}
	}

	CString szMsg;
	szMsg.Format("Expire Spiral Search Home Die %d used time %.2f", lTotalIndex, GetTime()-dTime);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
	BOOL bReturn = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SpiralSearchRefDie(IPC_CServiceMessage &svMsg)
{
	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX() - 1, GetPRWinLRY() - 1};

	int			siOrigX;
	int			siOrigY;
	int			siOrigT;
	LONG		lUpIndex = 0;
	LONG		lDnIndex = 0;
	LONG		lLtIndex = 0;
	LONG		lRtIndex = 0;
	LONG		lMaxLoopCount = 1;
	LONG		lRow, lCol;
	LONG		lCurrentLoop = 1;
	LONG		lCurrentIndex = 0;
	LONG		lTotalIndex = 0;
	LONG		lRefDieNo = 0;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	svMsg.GetMsg(sizeof(LONG), &lRefDieNo);

	lMaxLoopCount = lMaxLoopCount + 1;					//v4.04
	CMS896AApp::m_bStopAlign = FALSE;

	//Diplay current search region
	DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_YELLOW);

	//Get current position
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

	//Search from current position	
	if (IndexAndSearchRefDie(&siOrigX, &siOrigY, WPR_CT_DIE, lRefDieNo) == TRUE)
	{
		DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
		BOOL bReturn = m_bHomeDieUniqueResult;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	lTotalIndex = 0;
	for (lCurrentLoop = 1; lCurrentLoop <= lMaxLoopCount; lCurrentLoop++)
	{
		lRow = (lCurrentLoop * 2 + 1);
		lCol = (lCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lTotalIndex++;
		lCurrentIndex = 1;

		if (IndexAndSearchRefDie(&siOrigX, &siOrigY, WPR_RT_DIE, lRefDieNo) == TRUE)
		{
			DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
			BOOL bReturn = m_bHomeDieUniqueResult;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		Sleep(10);

		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;
			lTotalIndex++;
			if (pApp->IsStopAlign())
			{
				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (IndexAndSearchRefDie(&siOrigX, &siOrigY, WPR_UP_DIE, lRefDieNo) == TRUE)
			{
				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = m_bHomeDieUniqueResult;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}	
			Sleep(10);
		}			

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;
			lTotalIndex++;
			if (pApp->IsStopAlign())
			{
				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (IndexAndSearchRefDie(&siOrigX, &siOrigY, WPR_LT_DIE, lRefDieNo) == TRUE)
			{
				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = m_bHomeDieUniqueResult;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}	
			Sleep(10);
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;
			lTotalIndex++;
			if (pApp->IsStopAlign())
			{
				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (IndexAndSearchRefDie(&siOrigX, &siOrigY, WPR_DN_DIE, lRefDieNo) == TRUE)
			{
				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = m_bHomeDieUniqueResult;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}	
			Sleep(10);
		}


		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;
			lTotalIndex++;
			if (pApp->IsStopAlign())
			{
				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			if (IndexAndSearchRefDie(&siOrigX, &siOrigY, WPR_RT_DIE, lRefDieNo) == TRUE)
			{
				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = m_bHomeDieUniqueResult;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
			Sleep(10);
		}
	}

	DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
	BOOL bReturn = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SpiralSearchDie(IPC_CServiceMessage &svMsg)
{
	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX() - 1, GetPRWinLRY() - 1};

	int			siOrigX;
	int			siOrigY;
	int			siOrigT;
	LONG		lUpIndex = 0;
	LONG		lDnIndex = 0;
	LONG		lLtIndex = 0;
	LONG		lRtIndex = 0;
	LONG		lMaxLoopCount = 0;
	LONG		lRow, lCol;
	LONG		lCurrentLoop = 1;
	LONG		lCurrentIndex = 0;
	LONG		lTotalIndex = 0;


	svMsg.GetMsg(sizeof(LONG), &lMaxLoopCount);


	lMaxLoopCount = lMaxLoopCount;

	//Diplay current search region
	DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_YELLOW);

	//Get current position
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);


	//v2.93T2
	PR_COORD stBpULCorner = GetSrchArea().coCorner1;
	PR_COORD stBpLRCorner = GetSrchArea().coCorner2;
	m_stSearchArea.coCorner1.x = GetPRWinULX();
	m_stSearchArea.coCorner1.y = GetPRWinULY();
	m_stSearchArea.coCorner2.x = GetPRWinLRX() - 1;
	m_stSearchArea.coCorner2.y = GetPRWinLRY() - 1;


	//Search from current position	
	if (IndexAndSearchNormalDie(&siOrigX, &siOrigY, WPR_CT_DIE, WPR_CT_DIE, TRUE, FALSE, PR_FALSE) == TRUE)
	{
		//v2.93T2
		m_stSearchArea.coCorner1 = stBpULCorner;
		m_stSearchArea.coCorner2 = stBpLRCorner;

		DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	lTotalIndex = 0;
	for (lCurrentLoop = 1; lCurrentLoop <= lMaxLoopCount; lCurrentLoop++)
	{
		lRow = (lCurrentLoop * 2 + 1);
		lCol = (lCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lTotalIndex++;
		lCurrentIndex = 1;

		
		if (IndexAndSearchNormalDie(&siOrigX, &siOrigY, WPR_RT_DIE, WPR_CT_DIE, TRUE, FALSE, PR_FALSE) == TRUE)		//v2.93T2
		{
			//v2.93T2
			m_stSearchArea.coCorner1 = stBpULCorner;
			m_stSearchArea.coCorner2 = stBpLRCorner;

			DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}


		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;
			lTotalIndex++;
			
			if (IndexAndSearchNormalDie(&siOrigX, &siOrigY, WPR_UP_DIE, WPR_CT_DIE, TRUE, FALSE, PR_FALSE) == TRUE)		//v2.93T2
			{
				//v2.93T2
				m_stSearchArea.coCorner1 = stBpULCorner;
				m_stSearchArea.coCorner2 = stBpLRCorner;

				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}	
		}			

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;
			lTotalIndex++;
			
			if (IndexAndSearchNormalDie(&siOrigX, &siOrigY, WPR_LT_DIE, WPR_CT_DIE, TRUE, FALSE, PR_FALSE) == TRUE)		//v2.93T2
			{
				//v2.93T2
				m_stSearchArea.coCorner1 = stBpULCorner;
				m_stSearchArea.coCorner2 = stBpLRCorner;

				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}	
		}


		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;
			lTotalIndex++;

			if (IndexAndSearchNormalDie(&siOrigX, &siOrigY, WPR_DN_DIE, WPR_CT_DIE, TRUE, FALSE, PR_FALSE) == TRUE)		//v2.93T2
			{
				//v2.93T2
				m_stSearchArea.coCorner1 = stBpULCorner;
				m_stSearchArea.coCorner2 = stBpLRCorner;

				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}	
		}


		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;
			lTotalIndex++;
			
			if (IndexAndSearchNormalDie(&siOrigX, &siOrigY, WPR_RT_DIE, WPR_CT_DIE, TRUE, FALSE, PR_FALSE) == TRUE)		//v2.93T2
			{
				//v2.93T2
				m_stSearchArea.coCorner1 = stBpULCorner;
				m_stSearchArea.coCorner2 = stBpLRCorner;

				DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);
				BOOL bReturn = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}
	}

	DrawRectangleBox(stSrchCorner1, stSrchCorner2, PR_COLOR_TRANSPARENT);

	//v2.93T2
	m_stSearchArea.coCorner1 = stBpULCorner;
	m_stSearchArea.coCorner2 = stBpLRCorner;

	BOOL bReturn = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::CornerSearchHomeDie(IPC_CServiceMessage &svMsg)
{
	int siOrigX, siTempPhyX;
	int	siOrigY, siTempPhyY;
	int	siOrigT;
	
	LONG lConfirmResult = 0;
	LONG lRetryCount = 0;
	BOOL bDieFound = FALSE;

	ULONG ulTargetPosition = 0;	//0 = TL; 1 = TR; 2 = BL; 3 = BR

	svMsg.GetMsg(sizeof(ULONG), &ulTargetPosition);

	BOOL bPLLMRebel = FALSE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetProductLine() == "Rebel")
	{
		bPLLMRebel = TRUE;
	}

	// -------- Start of Search Home Die Step by Step -------//

	//Get current position
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

	BOOL bNewCornerHome = FALSE;
	ULONG ulTargetCorner = 0;	//0 = TL; 1 = TR; 2 = BL; 3 = BR
	CString szMsg;
	if (IsOsramResortMode() && m_ucMapRotation != 0)
	{
		bNewCornerHome = TRUE;
		switch (m_ucMapRotation)
		{
				//case 0:		//Klocwork	//v4.27
				//	ulTargetCorner = 0;
				//	break;
			case 1:
				ulTargetCorner = 1;
				break;
			case 2:
				ulTargetCorner = 3;
				break;
			case 3:
				ulTargetCorner = 2;
				break;
		}
		szMsg.Format("begin corner search home die at map rotation %d", m_ucMapRotation * 90);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}
	else if (IsLFSizeOK() && bPLLMRebel)	//v4.39T9
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Begin corner search home die (PLLM Rebel)");
	}
	else if (IsLFSizeOK())
	{
		bNewCornerHome = TRUE;
		ulTargetCorner = ulTargetPosition;
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("begin corner search home die with ShareGrab");
	}
	else
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("begin corner search home die");
	}

	m_ucCornerAlignDieState = 0;	//	begine corner search
	if (bNewCornerHome)
	{
		BOOL bNeedUp = TRUE, bNeedDown = TRUE, bNeedLeft = TRUE, bNeedRight = TRUE;
		LONG lFindResult = 0;
		lConfirmResult = TRUE;
		lRetryCount = 0;
		LONG lIdxDir = WPR_UR_DIE;

		PrescanNormalInit(GetPrescanPrID(), FALSE);	// for RectWaferAroundDieCheck to speed up time

		while (lRetryCount < 5)
		{
			m_ucCornerAlignDieState = 1;	//	multi-search begin, no die
			bNeedUp = FALSE;		
			bNeedDown = FALSE;
			bNeedLeft = FALSE;		
			bNeedRight = FALSE;
			(*m_psmfSRam)["WaferTable"]["Current"]["X"] = siOrigX;
			(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = siOrigY;
			lFindResult = RectWaferAroundDieCheck();

			szMsg.Format("current die around state %ld", lFindResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

			if (lFindResult >= 10000)	// center die remove
			{
				m_ucCornerAlignDieState = 2;	//	multi-search found die at corner
				lFindResult -= 10000;
			}
			if (lFindResult >= 1000)		// bottom die remove
			{
				bNeedDown = TRUE;
				lFindResult -= 1000;
			}
			if (lFindResult >= 100)		// right die remove
			{
				bNeedRight = TRUE;
				lFindResult -= 100;
			}
			if (lFindResult >= 10)		// up die remove
			{
				bNeedUp = TRUE;
				lFindResult -= 10;
			}
			if (lFindResult >= 1)		// left die remove
			{
				bNeedLeft = TRUE;
				lFindResult -= 1;
			}

			if (ulTargetCorner == 0)
			{
				if (bNeedUp == FALSE && bNeedLeft == FALSE)	// no any die found at TL	outer
				{
					lConfirmResult = FALSE;
					break;
				}
				lIdxDir = WPR_UL_DIE;
				if (bNeedUp && bNeedLeft)
				{
					lIdxDir = WPR_UL_DIE;
				}
				else if (bNeedLeft)
				{
					lIdxDir = WPR_LT_DIE;
				}
				else if (bNeedUp)
				{
					lIdxDir = WPR_UP_DIE;
				}
			}

			if (ulTargetCorner == 1)
			{
				if (bNeedUp == FALSE && bNeedRight == FALSE)	// no any die found at TR
				{
					lConfirmResult = FALSE;
					break;
				}
				lIdxDir = WPR_UR_DIE;
				if (bNeedUp && bNeedRight)
				{
					lIdxDir = WPR_UR_DIE;
				}
				else if (bNeedRight)
				{
					lIdxDir = WPR_RT_DIE;
				}
				else if (bNeedUp)
				{
					lIdxDir = WPR_UP_DIE;
				}
			}
			if (ulTargetCorner == 3)
			{
				if (bNeedDown == FALSE && bNeedRight == FALSE)	// no die at BR
				{
					lConfirmResult = FALSE;
					break;
				}
				lIdxDir = WPR_DR_DIE;
				if (bNeedDown && bNeedRight)
				{
					lIdxDir = WPR_DR_DIE;
				}
				else if (bNeedDown)
				{
					lIdxDir = WPR_DN_DIE;
				}
				else if (bNeedRight)
				{
					lIdxDir = WPR_RT_DIE;
				}
			}
			if (ulTargetCorner == 2)
			{
				if (bNeedDown == FALSE && bNeedLeft == FALSE)	// no die at BL
				{
					lConfirmResult = FALSE;
					break;
				}
				lIdxDir = WPR_DL_DIE;
				if (bNeedDown && bNeedLeft)
				{
					lIdxDir = WPR_DL_DIE;
				}
				else if (bNeedDown)
				{
					lIdxDir = WPR_DN_DIE;
				}
				else if (bNeedLeft)
				{
					lIdxDir = WPR_LT_DIE;
				}
			}

			siTempPhyX = siOrigX;
			siTempPhyY = siOrigY;

			bDieFound = IndexAndSearchNormalDie(&siTempPhyX, &siTempPhyY, lIdxDir, WPR_CT_DIE, TRUE, TRUE, PR_TRUE);

			siOrigX = siTempPhyX;
			siOrigY = siTempPhyY;
			if (bDieFound == TRUE)
			{
				lRetryCount = 0;
			}
			else
			{
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("here to do something outer check");
				lRetryCount++;
			}
		}
	}
	else	// not bNewCornerHome
	{
		LONG lSrchPos = WPR_CT_DIE;
		LONG lIndexPos[WPR_MAX_CORNER_DIE_CHECK];
		LONG lConfirmPos;
		switch (ulTargetPosition)
		{
		default:	//TL
			lIndexPos[0] = WPR_UL_DIE;
			lIndexPos[1] = WPR_UP_DIE;
			lIndexPos[2] = WPR_LT_DIE;
			lIndexPos[3] = WPR_DL_DIE;
			lConfirmPos	= WPR_UR_DIE;
			break;

		case 1:		//TR
			lIndexPos[0] = WPR_UR_DIE;
			lIndexPos[1] = WPR_UP_DIE;
			lIndexPos[2] = WPR_RT_DIE;
			lIndexPos[3] = WPR_DR_DIE;
			lConfirmPos = WPR_UL_DIE;
			break;

		case 2:		//BL
			lIndexPos[0] = WPR_DL_DIE;
			lIndexPos[1] = WPR_DN_DIE;
			lIndexPos[2] = WPR_LT_DIE;
			lIndexPos[3] = WPR_UL_DIE;
			lConfirmPos = WPR_DR_DIE;
			break;

		case 3:		//BR
			lIndexPos[0] = WPR_DR_DIE;
			lIndexPos[1] = WPR_DN_DIE;
			lIndexPos[2] = WPR_RT_DIE;
			lIndexPos[3] = WPR_UR_DIE;
			lConfirmPos = WPR_DL_DIE;
			break;
		}

		BOOL bMoveTable = TRUE;
		BOOL bUseIndex = TRUE;
		PR_BOOLEAN bPRLatch = PR_TRUE;

		if (IsLFSizeOK())
		{
			bUseIndex	= FALSE;
			bMoveTable	= FALSE;
			bPRLatch	= PR_TRUE;
		}
		else
		{
			//Index 1 by 1
			bUseIndex	= TRUE;
			bMoveTable	= TRUE;
			bPRLatch	= PR_TRUE;
		}

		int i;
		while (lRetryCount < 2)
		{
			//Search IndexPos die (For LF, 1 cycle only no need to move table & die compensation)
			for (i = 0; i < WPR_MAX_CORNER_DIE_CHECK; i++)
			{
				if (bUseIndex == FALSE)
				{
					lSrchPos = lIndexPos[i];
				}
	
				siTempPhyX = siOrigX;
				siTempPhyY = siOrigY;
	
				bDieFound = (BOOL)IndexAndSearchNormalDie(&siTempPhyX, &siTempPhyY, lIndexPos[i], lSrchPos, bMoveTable, TRUE, bPRLatch);	//v2.78T1
	
				bMoveTable = bUseIndex;
				bPRLatch = (PR_BOOLEAN)bUseIndex;
	
				if (bDieFound == TRUE)
				{
					break;
				}
			}
	
			if (bDieFound == TRUE)
			{
				siOrigX = siTempPhyX;
				siOrigY = siTempPhyY;
	
				//If LF use, need to grab PR image again after die is found
				if (bUseIndex == FALSE)
				{
					bPRLatch = PR_TRUE;
				}
	
				lRetryCount = 0;
			}
			else
			{
				if (!IsLFSizeOK())
				{
					lSrchPos = WPR_CT_DIE;
	
					if (bUseIndex == FALSE)
					{
						lSrchPos = lConfirmPos;
					}
	
					if (lRetryCount == 0)
					{
						//Search ConfirmPos die
						siTempPhyX = siOrigX;
						siTempPhyY = siOrigY;
						lConfirmResult = IndexAndSearchNormalDie(&siTempPhyX, &siTempPhyY, lConfirmPos, lSrchPos, bMoveTable, FALSE, bPRLatch);		//v2.78T1
					}
	
					//Move 1 more IndexPos
					siTempPhyX = siOrigX;
					siTempPhyY = siOrigY;
					IndexAndSearchNormalDie(&siTempPhyX, &siTempPhyY, lIndexPos[lRetryCount], WPR_CT_DIE, TRUE, TRUE, PR_TRUE);		//v2.78T1
	
					lRetryCount++;
				}
				else
				{
					typedef struct 
					{
						ULONG	ulCornerOption;
						BOOL	bLfResult;
						BOOL	bFovSize;
					} SRCH_LR_TYPE;
					SRCH_LR_TYPE	stSrchLrCornerInfo;
					IPC_CServiceMessage stMsg;
					stMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

					INT nDiePos = 0;
					if (ulTargetPosition == 1)	//If UR		//v4.40T11
					{
						nDiePos = LookAheadURightRectWaferCorner(stMsg);
						stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
					}
					else
					{
						nDiePos = LookAheadULeftRectWaferCorner(stMsg);
						stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

						//pllm	//Retry!!
						//v4.01		//Re-implement for Cree China
						if (nDiePos == WPR_UR_DIE)
						{
							int nX = 0, nY = 0, nT = 0;
							GetWaferTableEncoder(&nX, &nY, &nT);
							IndexAndSearchNormalDie(&nX, &nY, WPR_UP_DIE, WPR_CT_DIE, TRUE, FALSE, PR_TRUE);
		
							siOrigX = nX;
							siOrigY = nY;
		
							Sleep(200);
							nDiePos = LookAheadULeftRectWaferCorner(stMsg);
							stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		
						}
						else if (nDiePos == WPR_DL_DIE)
						{
							int nX = 0, nY = 0, nT = 0;
							GetWaferTableEncoder(&nX, &nY, &nT);
							IndexAndSearchNormalDie(&nX, &nY, WPR_LT_DIE, WPR_CT_DIE, TRUE, FALSE, PR_TRUE);
		
							siOrigX = nX;
							siOrigY = nY;
		
							Sleep(200);
							nDiePos = LookAheadULeftRectWaferCorner(stMsg);
							stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
						}
						else if (nDiePos != 1)	//!=0 -> not OK!
						{
							Sleep(200);
							INT nDiePos = LookAheadULeftRectWaferCorner(stMsg);
							stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
						}
					}

					if (stSrchLrCornerInfo.bLfResult)
					{
						lConfirmResult = FALSE;
					}
					else
					{	
						lConfirmResult = TRUE;
					}
	
					break;
				}
			}
		}
	}

	MoveWaferTable(siOrigX, siOrigY);

	BOOL bReturn = FALSE;
	if (lConfirmResult == FALSE)
	{
		bReturn = TRUE;
		szMsg.Format("cofnirm false and return true at %d,%d", siOrigX, siOrigY);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}
	else
	{
		m_ucCornerAlignDieState = 0;	//	corner search fail
		szMsg.Format("cofnirm true at %d,%d", siOrigX, siOrigY);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
		//v3.70T3	//PLLM special feature
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
		if (nPLLM != PLLM_REBEL)	//xyz
		{
			//Allow user to manaul align
			LONG lReply = 0;
			CString szContent = " ";

			lReply = SetAlert_Msg_Red_Yellow(IDS_WPR_CORNER_SRH_FAILED, szContent, "Yes", "No");
			
			//if ( lReply == glHMI_YES )
			if (lReply == 1)
			{
				SetWaferTableJoystickSpeed(2);		//v2.64
				SetWaferTableJoystick(TRUE);

				CString szContent;
				szContent.LoadString(HMB_WPR_MANUAL_ALIGN_HDIE);
				HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				SetWaferTableJoystick(FALSE);
				bReturn = TRUE;
			}
		}
	}
	// -------- End of Search Home Die Step by Step -------//

	if( CheckRebel_N2RC_Location(TRUE)==FALSE )
	{
		bReturn = FALSE;
	}

	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

	UpdateDirectCornerSearchPos(siOrigX, siOrigY);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::TestHomeDiePattern(IPC_CServiceMessage &svMsg)
{
	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX(), GetPRWinLRY()};
	PR_UWORD	usDieType	= PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

	BOOL bReturn = FALSE;

	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	//Search from current position	
	//Search reference die with max search region
	if (ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2) != -1)
	{
		if (DieIsGood(usDieType) == TRUE)
		{
			CString szContent;

			ManualDieCompenate(stDieOffset, fDieRotate);

			szContent.LoadString(HMB_WPR_FIND_GOOD_REFDIE);

			HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		}
	}
	else
	{
		bReturn = FALSE;
	}



	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}



//================================================================
// LookAheadULeftRectWaferCorner()
//   Created-By  : Andrew Ng
//   Date        : 9/28/2007 11:24:08 AM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferPr::LookAheadULeftRectWaferCorner(IPC_CServiceMessage &svMsg)
{
	INT nResult = -1;
	BOOL bIsUpperLeftCornerComplete = TRUE;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;

	typedef struct 
	{
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;
	svMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

	//Return FALSE if FOV size is too small
	if (!IsLFSizeOK())
	{
		CString szTemp;
		szTemp.Format("LookAheadULeftRectWaferCorner: FOV size = %.2f reject", m_dLFSize);
		//Log_AlignWafer(szTemp);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		stSrchLrCornerInfo.bFovSize		= FALSE;
		stSrchLrCornerInfo.bLfResult	= FALSE;
		svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		return 1;
	}
	else
	{
		stSrchLrCornerInfo.bFovSize		= TRUE;
	}
	stSrchLrCornerInfo.bLfResult		= FALSE;

	int nPosnX, nPosnY;
	BOOL bCenterDie = FALSE;
	BOOL bULeftDie = TRUE, bLeftDie = TRUE, bTopDie = TRUE;
	BOOL bLRightDie = FALSE, bRightDie = FALSE, bBtmDie = FALSE, bURightDie = FALSE, bLLeftDie = FALSE;


	//Check center die
	bCenterDie = LookAroundNormalDie(nPosnX, nPosnY, WPR_CT_DIE, PR_TRUE);

	//if center die not found, confirm with nearby dices on left/up
	if (!bCenterDie)
	{
		bRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE);
		bBtmDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_DN_DIE, PR_FALSE);
			
		//if (!bULeftDie || !bLeftDie || !bTopDie) 
		if (!bRightDie || !bBtmDie) 
		{
			CString szTemp;
			szTemp.Format("LookAheadULeftRectWaferCorner FAIL: C=%d, R=%d, B=%d", bCenterDie, bRightDie, bBtmDie);
			//Log_AlignWafer(szTemp);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}

	bLLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DL_DIE, PR_FALSE);
	bLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE);
	bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);
	bULeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UL_DIE, PR_FALSE);
	bURightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UR_DIE, PR_FALSE);

	/*
	if (bIsUpperLeftCornerComplete == FALSE)
	{
		// must have a die on it, set it to false for passing the case
		bURightDie = FALSE;
	}
	*/

	// lower-right corners should have no dices
	if (!bLLeftDie && !bLeftDie && !bTopDie && !bULeftDie && !bURightDie)
	{
		stSrchLrCornerInfo.bLfResult = TRUE;		// LF result OK!!
		CString szTemp;
		szTemp.Format("LookAheadULeftRectWaferCorner OK:  LL=%d, L=%d, UL=%d, UP=%d, UR=%d", bLLeftDie, bLeftDie, bULeftDie, bTopDie, bURightDie);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	}
	else
	{
		CString szTemp;
		szTemp.Format("LookAheadULeftRectWaferCorner FAIL:  LL=%d, L=%d, UL=%d, UP=%d, UR=%d", bLLeftDie, bLeftDie, bULeftDie, bTopDie, bURightDie);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}

	svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);	//pllm

	if ((bLLeftDie == TRUE) && (bURightDie == FALSE))
	{
		return WPR_DL_DIE;
	}
	if ((bLLeftDie == FALSE) && (bURightDie == TRUE))
	{
		return WPR_UR_DIE;
	}
	return nResult;
}


LONG CWaferPr::LookAheadURightRectWaferCorner(IPC_CServiceMessage &svMsg)	//v4.21T3
{
	INT nResult = -1;
	BOOL bIsUpperLeftCornerComplete = TRUE;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;

	typedef struct 
	{
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;
	svMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

	ULONG ulOption = stSrchLrCornerInfo.ulCornerOption;

	//Return FALSE if FOV size is too small
	if (!IsLFSizeOK())
	{
		CString szTemp;
		szTemp.Format("LookAhead URightRectWaferCorner: FOV size = %.2f reject", m_dLFSize);
		//Log_AlignWafer(szTemp);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		stSrchLrCornerInfo.bFovSize		= FALSE;
		stSrchLrCornerInfo.bLfResult	= FALSE;
		svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		return 1;
	}
	else
	{
		stSrchLrCornerInfo.bFovSize		= TRUE;
	}
	stSrchLrCornerInfo.bLfResult		= FALSE;
	

	int nPosnX, nPosnY;
	BOOL bCenterDie = FALSE;
	BOOL bULeftDie = TRUE, bLeftDie = TRUE, bTopDie = TRUE;
	BOOL bLRightDie = FALSE, bRightDie = FALSE, bBtmDie = FALSE, bURightDie = FALSE, bLLeftDie = FALSE;


	//Check center die
	bCenterDie = LookAroundNormalDie(nPosnX, nPosnY, WPR_CT_DIE, PR_TRUE);

	//if center die not found, confirm with nearby dices on left/up
	if (!bCenterDie)
	{
		bLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE);
		bBtmDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_DN_DIE, PR_FALSE);
			
		//if (!bULeftDie || !bLeftDie || !bTopDie) 
		if (!bLeftDie || !bBtmDie) 
		{
			CString szTemp;
			szTemp.Format("LookAhead URightRectWaferCorner FAIL: C=%d, L=%d, B=%d", bCenterDie, bLeftDie, bBtmDie);
			//Log_AlignWafer(szTemp);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}

	bLRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DR_DIE, PR_FALSE);
	bRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE);
	bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);
	bULeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UL_DIE, PR_FALSE);
	bURightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UR_DIE, PR_FALSE);
			
	CString szTemp;

	if (ulOption > 0)
	{
		// lower-right corners should have no dices
		if (!bLRightDie && !bRightDie && !bTopDie && !bULeftDie && !bURightDie)
		{
			stSrchLrCornerInfo.bLfResult = TRUE;		// LF result OK!!
			szTemp.Format("LookAhead URightRectWaferCorner OK:  LR=%d, R=%d, UL=%d, UP=%d, UR=%d", bLRightDie, bRightDie, bULeftDie, bTopDie, bURightDie);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		}
		else
		{
			szTemp.Format("LookAhead URightRectWaferCorner FAIL:  LR=%d, R=%d, UL=%d, UP=%d, UR=%d", bLRightDie, bRightDie, bULeftDie, bTopDie, bURightDie);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		}
	}
	else
	{
		// lower-right corners should have no dices
		if (!bRightDie)
		{
			stSrchLrCornerInfo.bLfResult = TRUE;		// LF result OK!!
			szTemp.Format("LookAhead URightRectWaferCorner OK:  R=%d", bRightDie);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		}
		else
		{
			szTemp.Format("LookAhead URightRectWaferCorner FAIL:  R=%d", bRightDie);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		}
	}

	svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

	return TRUE;
}


//================================================================
// LookAheadLRightRectWaferCorner()
//   Created-By  : Andrew Ng
//   Date        : 9/27/2007 3:27:03 PM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferPr::LookAheadLRightRectWaferCorner(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;
	svMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);	//v4.41T1

	//Return FALSE if FOV size is too small
	if (!IsLFSizeOK())
	{
		CString szTemp;
		szTemp.Format("LookAheadLRightRectWaferCorner: FOV size = %.2f reject", m_dLFSize);
		//Log_AlignWafer(szTemp);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		stSrchLrCornerInfo.bFovSize		= FALSE;
		stSrchLrCornerInfo.bLfResult	= FALSE;
		svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		return 1;
	}
	else
	{
		stSrchLrCornerInfo.bFovSize		= TRUE;
	}
	stSrchLrCornerInfo.bLfResult		= FALSE;


	//v4.15T2	//Cree China
	CString szTemp;
	ULONG ulCurrMapRow = (*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapRow"]; 
	ULONG ulCurrMapCol = (*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapCol"];
	LONG lUserRow = 0, lUserCol = 0;
	ConvertAsmToOrgUser(ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);

	BOOL bCheckRefDie = FALSE;
	if (m_WaferMapWrapper.IsReferenceDie(ulCurrMapRow, ulCurrMapCol))
	{
		bCheckRefDie = TRUE;
		szTemp.Format("LookAheadLRightRectWaferCorner REF die at Map I(%lu, %lu) - U(%lu, %lu)",
					  ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	}
	else
	{
		szTemp.Format("LookAheadLRightRectWaferCorner die at Map I(%lu, %lu) - U(%ld, %ld)",
					  ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	}
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);


	int nPosnX, nPosnY;
	int nCPosnX, nCPosnY;
	int nCPosnX2, nCPosnY2;
	BOOL bCenterDie = FALSE;
	BOOL bULeftDie = TRUE, bLeftDie = TRUE, bTopDie = TRUE;
	BOOL bLRightDie = FALSE, bRightDie = FALSE, bBtmDie = FALSE, bURightDie = FALSE, bLLeftDie = FALSE;

	//Check center die
	if (bCheckRefDie)
	{
		bCenterDie = LookAroundRefDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE);		//v4.15T2
		LookAroundNormalDie(nCPosnX2, nCPosnY2, WPR_CT_DIE, PR_TRUE);
	}
	else
	{
		bCenterDie = LookAroundNormalDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE);
		if (!bCenterDie && bPLLMSpecialFcn)		//v4.41T1
			bCenterDie = LookAroundNormalDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE, 1);
	}


	//if center die not found, confirm with nearby dices on left/up
	if (!bCenterDie)
	{
		//bULeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UL_DIE, PR_FALSE);
		bLeftDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE);
		if (!bLeftDie && bPLLMSpecialFcn)
			bLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE, 1);
		bTopDie			= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);
		if (!bTopDie && bPLLMSpecialFcn)
			bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE, 1);
			
		//if (!bULeftDie || !bLeftDie || !bTopDie) 
		if (!bLeftDie || !bTopDie) 
		{
			CString szTemp;
			//szTemp.Format("LookAheadLRightRectWaferCorner FAIL: C=%d, UL=%d, L=%d, T=%d", bCenterDie, bULeftDie, bLeftDie, bTopDie);
			szTemp.Format("LookAheadLRightRectWaferCorner FAIL: C=%d, L=%d, T=%d", bCenterDie, bLeftDie, bTopDie);
			//Log_AlignWafer(szTemp);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}
	else
	{
		if (abs(nCPosnX) > 0.33 * GetDiePitchXX())
		{
			CString szTemp;
			szTemp.Format("LookAheadLRightRectWaferCorner FAIL pitch offset X: %d (%d)", nCPosnX, fabs(0.33 * GetDiePitchXX()));
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}

		if (abs(nCPosnY) > 0.33 * GetDiePitchYY())
		{
			CString szTemp;
			szTemp.Format("LookAheadLRightRectWaferCorner FAIL pitch offset Y: %d (%d)", nCPosnY, fabs(0.33 * GetDiePitchYY()));
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}


	bLRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DR_DIE, PR_FALSE);
	bRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE);
	bBtmDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_DN_DIE, PR_FALSE);
	bURightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UR_DIE, PR_FALSE);
	bLLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DL_DIE, PR_FALSE);
	bLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE);
	bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);

	// lower-right corners should have no dices
	if (!bLRightDie && !bRightDie && !bBtmDie && !bURightDie && !bLLeftDie)		// && bLeftDie && bTopDie)
	{
		stSrchLrCornerInfo.bLfResult = TRUE;		// LF result OK!!

		if (bCenterDie)
			szTemp.Format("LookAheadLRightRectWaferCorner: OK: Last-die offset dX=%d(%.3f), dY=%d(%.3f)", 
						  nCPosnX, 0.33 * GetDiePitchXX(), nCPosnY, 0.33 * GetDiePitchYY());
		else
		{
			szTemp = _T("LookAheadLRightRectWaferCorner: OK: center-die not found");
		}
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		szTemp.Format("LookAheadLRightRectWaferCorner OK:  LR=%d, R=%d, B=%d, UR=%d, LL=%d",	//, L=%d, U=%d", 
					  bLRightDie, bRightDie, bBtmDie, bURightDie, bLLeftDie);		//, bLeftDie, bTopDie);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}
	else
	{
		szTemp.Format("LookAheadLRightRectWaferCorner FAIL:  LR=%d, R=%d, B=%d, UR=%d, LL=%d",	//, L=%d, U=%d", 
					  bLRightDie, bRightDie, bBtmDie, bURightDie, bLLeftDie);		//, bLeftDie, bTopDie);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}

	svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
	return 1;
}


LONG CWaferPr::LookAheadLRightRectWaferCornerWithIncompleteLastRow(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;
	svMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);	//v4.41T1


	//Return FALSE if FOV size is too small
	if (!IsLFSizeOK())
	{
		CString szTemp;
		szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow: FOV size = %.2f reject", m_dLFSize);
		//Log_AlignWafer(szTemp);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		stSrchLrCornerInfo.bFovSize		= FALSE;
		stSrchLrCornerInfo.bLfResult	= FALSE;
		svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		return 1;
	}
	else
	{
		stSrchLrCornerInfo.bFovSize		= TRUE;
	}
	stSrchLrCornerInfo.bLfResult		= FALSE;


	CString szTemp;
	ULONG ulCurrMapRow = (*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapRow"]; 
	ULONG ulCurrMapCol = (*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapCol"];
	//v4.14T8
	LONG lUserRow = 0, lUserCol = 0;
	ConvertAsmToOrgUser(ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	BOOL bCheckRefDie = FALSE;
	if (m_WaferMapWrapper.IsReferenceDie(ulCurrMapRow, ulCurrMapCol))
	{
		szTemp.Format("LookAheadLRRectWaferCornerWithIncompleteLastRow REF die at Map I(%lu, %lu) - U(%lu, %lu)",
					  ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	}
	else
	{
		szTemp.Format("LookAheadLRRectWaferCornerWithIncompleteLastRow die at Map I(%lu, %lu) - U(%lu, %lu)",
					  ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	}
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//v4.14T8
	ULONG ulCurrRow = 0, ulCurrCol = 0;
	LONG ulCurrUserRow = 0, ulCurrUserCol = 0;
	m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);
	ConvertAsmToOrgUser(ulCurrRow, ulCurrCol, ulCurrUserRow, ulCurrUserCol);

	//v4.15T2
	bCheckRefDie = FALSE;
	if (m_WaferMapWrapper.IsReferenceDie(ulCurrMapRow, ulCurrMapCol))
	{
		bCheckRefDie = TRUE;
		szTemp.Format("LookAheadLRRectWaferCornerWithIncompleteLastRow curr-REF-pos at I(%lu, %lu) - U(%lu, %lu)",
					  ulCurrRow, ulCurrCol, ulCurrUserRow, ulCurrUserCol);
	}
	else
	{
		szTemp.Format("LookAheadLRRectWaferCornerWithIncompleteLastRow curr-die-pos at I(%lu, %lu) - U(%lu, %lu)",
					  ulCurrRow, ulCurrCol, ulCurrUserRow, ulCurrUserCol);
	}
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);


	int nPosnX, nPosnY;
	int nCPosnX, nCPosnY;
	BOOL bCenterDie = FALSE;
	BOOL bLeftDie = TRUE, bTopDie = TRUE;
	BOOL bLRightDie = FALSE, bRightDie = FALSE, bBtmDie = FALSE;
	BOOL bURightDie = FALSE, bLLeftDie = FALSE;

	//Check center die
	if (bCheckRefDie)
	{
		bCenterDie = LookAroundRefDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE);    //v4.15T2	//Cree China
	}		
	else
	{
		bCenterDie = LookAroundNormalDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE);
		if (!bCenterDie && bPLLMSpecialFcn)		//v4.41T1
			bCenterDie = LookAroundNormalDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE, 1);
	}


	//if center die not found, confirm with nearby dices on left/up
	if (!bCenterDie)
	{
		bLeftDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE);
		if (!bLeftDie && bPLLMSpecialFcn)		//v4.41T1
			bLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE, 1);
		bTopDie			= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);
		if (!bTopDie && bPLLMSpecialFcn)		//v4.41T1
			bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE, 1);
		
		//if (!bULeftDie || !bLeftDie || !bTopDie) 
		if (!bLeftDie || !bTopDie) 
		{
			CString szTemp;
			//szTemp.Format("LookAheadLRightRectWaferCorner FAIL: C=%d, UL=%d, L=%d, T=%d", bCenterDie, bULeftDie, bLeftDie, bTopDie);
			szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow FAIL: C=%d, L=%d, T=%d", bCenterDie, bLeftDie, bTopDie);
			//Log_AlignWafer(szTemp);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}
	else
	{
		if (abs(nCPosnX) > 0.33 * GetDiePitchXX())
		{
			CString szTemp;
			szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow FAIL pitch offset X: %d (%d)", nCPosnX, fabs(0.33 * GetDiePitchXX()));
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}

		if (abs(nCPosnY) > 0.33 * GetDiePitchYY())
		{
			CString szTemp;
			szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow FAIL pitch offset Y: %d (%d)", nCPosnY, fabs(0.33 * GetDiePitchYY()));
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}


	BOOL bURDieExist = FALSE;
	
	if (WM_CWaferMap::Instance()->IsMapHaveBin(ulCurrMapRow - 1, ulCurrMapCol + 1))
	{
		bURDieExist = TRUE;
	}
	BOOL bLLDieExist = FALSE;
	if (WM_CWaferMap::Instance()->IsMapHaveBin(ulCurrMapRow + 1, ulCurrMapCol - 1))
	{
		bLLDieExist = TRUE;
	}

	bLRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DR_DIE, PR_FALSE);
	bRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE);
	bBtmDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_DN_DIE, PR_FALSE);
	bURightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UR_DIE, PR_FALSE);
	bLLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DL_DIE, PR_FALSE);
	bLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE);
	bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);


	// lower-right corners should have no dices
	if (!bLRightDie && !bRightDie && !bBtmDie)	// && bLeftDie && bTopDie)
	{
		if ((bURDieExist && !bURightDie) || (!bURDieExist && bURightDie))
		{
			szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow FAIL: UR=%d but MAP=%d", bURightDie, bURDieExist);
		}
		else if ((bLLDieExist && !bLLeftDie) || (!bLLDieExist && bLLeftDie))
		{
			szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow FAIL: LL=%d but MAP=%d", bLLeftDie, bLLDieExist);
		}
		else
		{
			stSrchLrCornerInfo.bLfResult = TRUE;		// LF result OK!!

			if (bCenterDie)
				szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow: OK: Last-die offset dX=%d(%.2f), dY=%d(%.2f)", 
							  nCPosnX, 0.33 * GetDiePitchXX(), nCPosnY, 0.33 * GetDiePitchYY());
			else
			{
				szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow: center-die not found but OK (L=%d, T=%d)", bLeftDie, bTopDie);
			}
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow OK:  LR=%d, R=%d, B=%d UR=%d(%d) LL=%d(%d)", 
						  bLRightDie, bRightDie, bBtmDie, bURightDie, bURDieExist, bLLeftDie, bLLDieExist);
			//CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		}

		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}
	else
	{
		szTemp.Format("LookAheadLRightRectWaferCornerWithIncompleteLastRow FAIL:  LR=%d, R=%d, B=%d", 
					  bLRightDie, bRightDie, bBtmDie);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}

	svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
	return 1;

}


//================================================================
// LookAheadLLeftRectWaferCorner()
//   Created-By  : Andrew Ng
//   Date        : 10/4/2007 9:45:09 AM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferPr::LookAheadLLeftRectWaferCorner(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;
	svMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);	//v4.41T1


	//Return FALSE if FOV size is too small
	if (!IsLFSizeOK())
	{
		CString szTemp;
		szTemp.Format("LookAheadLLeftRectWaferCorner: FOV size = %.2f reject", m_dLFSize);
		//Log_AlignWafer(szTemp);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		stSrchLrCornerInfo.bFovSize		= FALSE;
		stSrchLrCornerInfo.bLfResult	= FALSE;
		svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		return 1;
	}
	else
	{
		stSrchLrCornerInfo.bFovSize		= TRUE;
	}
	stSrchLrCornerInfo.bLfResult		= FALSE;


	int nPosnX, nPosnY;
	int nCPosnX, nCPosnY;
	int nCPosnX2, nCPosnY2;
	DOUBLE dX = 0.00, dY = 0.00;
	BOOL bCenterDie = FALSE;
	BOOL bURightDie = TRUE, bRightDie = TRUE, bTopDie = TRUE;
	BOOL bLLeftDie = FALSE, bLeftDie = FALSE, bBtmDie = FALSE, bULeftDie = FALSE, bLRightDie = FALSE;
	dX = 0.33 * GetDiePitchXX();
	dY = 0.33 * GetDiePitchYY();


	//v4.15T2
	ULONG ulCurrMapRow = (*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapRow"]; 
	ULONG ulCurrMapCol = (*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapCol"];
	
	CString szTemp;
	LONG lUserRow = 0, lUserCol = 0;
	ConvertAsmToOrgUser(ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	
	BOOL bCenterRefDie = FALSE;
	if (m_WaferMapWrapper.IsReferenceDie(ulCurrMapRow, ulCurrMapCol))
	{
		bCenterRefDie = TRUE;
		szTemp.Format("LookAheadLLeftRectWaferCorner REF die at Map I(%lu, %lu) - U(%lu, %lu)",
					  ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	}	
	else
	{
		szTemp.Format("LookAheadLLeftRectWaferCorner die at Map I(%lu, %lu) - U(%lu, %lu)",
					  ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	}
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);


	//Check center die
	if (bCenterRefDie)
	{
		bCenterDie = LookAroundRefDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE);		//v4.15T2
		LookAroundNormalDie(nCPosnX2, nCPosnY2, WPR_CT_DIE, PR_TRUE);
	}
	else
	{
		bCenterDie = LookAroundNormalDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE);
		if (!bCenterDie && bPLLMSpecialFcn)		//v4.41T1
			bCenterDie = LookAroundNormalDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE, 1);
	}

	//if center die not found, confirm with nearby dices on left/up
	if (!bCenterDie)
	{
		bRightDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE);
		if (!bRightDie && bPLLMSpecialFcn)		//v4.41T1
			bRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE, 1);
		bTopDie			= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);
		if (!bTopDie && bPLLMSpecialFcn)		//v4.41T1
			bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE, 1);
			
		//if (!bULeftDie || !bLeftDie || !bTopDie) 
		if (!bRightDie || !bTopDie) 
		{
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("LookAheadLLeftRectWaferCorner Center-die not found.");

			CString szTemp;
			//szTemp.Format("LookAheadLRightRectWaferCorner FAIL: C=%d, UL=%d, L=%d, T=%d", bCenterDie, bULeftDie, bLeftDie, bTopDie);
			szTemp.Format("LookAheadLLeftRectWaferCorner FAIL: C=%d, R=%d, T=%d", bCenterDie, bRightDie, bTopDie);
			//Log_AlignWafer(szTemp);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}
	else
	{
		if (abs(nCPosnX) > dX)
		{
			CString szTemp;
			szTemp.Format("LookAheadLLeftRectWaferCorner FAIL pitch offset X: %d (%d)", abs(nCPosnX), dX);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}

		if (abs(nCPosnY) > dY)
		{
			CString szTemp;
			szTemp.Format("LookAheadLLeftRectWaferCorner FAIL pitch offset Y: %d (%d)", abs(nCPosnY), dY);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}

	bLLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DL_DIE, PR_FALSE);
	bLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE);
	bBtmDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_DN_DIE, PR_FALSE);
	bULeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UL_DIE, PR_FALSE);
	bLRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DR_DIE, PR_FALSE);
	bRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE);
	bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);


	// lower-right corners should have no dices
	if (!bLLeftDie && !bLeftDie && !bBtmDie && !bULeftDie && !bLRightDie)	// && bRightDie && bTopDie)
	{
		stSrchLrCornerInfo.bLfResult = TRUE;		// LF result OK!!

		if (bCenterDie)
			szTemp.Format("LookAheadLLeftRectWaferCorner: OK: Last-die offset dX=%d(%.2f), dY=%d(%.2f)", 
						  nCPosnX, dX, nCPosnY, dY);
		else
		{
			szTemp.Format("LookAheadLLeftRectWaferCorner: center-die not found but OK (R=%d, T=%d)", bRightDie, bTopDie);
		}
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		
		szTemp.Format("LookAheadLLeftRectWaferCorner OK:  LL=%d, L=%d, B=%d, UL=%d, LR=%d, C=%d",	// R=%d, UP=%d", 
					  bLLeftDie, bLeftDie, bBtmDie, bULeftDie, bLRightDie, bCenterDie);						//, bRightDie, bTopDie);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}
	else
	{
		szTemp.Format("LookAheadLLeftRectWaferCorner FAIL:  LL=%d, L=%d, B=%d, UL=%d, LR=%d, C=%d",	// R=%d, UP=%d", 
					  bLLeftDie, bLeftDie, bBtmDie, bULeftDie, bLRightDie, bCenterDie);						//, bRightDie, bTopDie);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}

	svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
	return 1;
}


//================================================================
// LookAheadLLeftRectWaferCorner()
//   Created-By  : Andrew Ng
//   Date        : 10/4/2007 9:45:09 AM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferPr::LookAheadLLeftRectWaferWithInCompleteCorner(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;
	svMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);	//v4.41T1


	//Return FALSE if FOV size is too small
	if (!IsLFSizeOK())
	{
		CString szTemp;
		szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner: FOV size = %.2f reject", m_dLFSize);
		//Log_AlignWafer(szTemp);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		stSrchLrCornerInfo.bFovSize		= FALSE;
		stSrchLrCornerInfo.bLfResult	= FALSE;
		svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		return 1;
	}
	else
	{
		stSrchLrCornerInfo.bFovSize		= TRUE;
	}
	stSrchLrCornerInfo.bLfResult		= FALSE;


	int nPosnX, nPosnY;
	int nCPosnX, nCPosnY;
	DOUBLE dX = 0.00, dY = 0.00;
	BOOL bCenterDie = FALSE;
	BOOL bURightDie = TRUE, bRightDie = TRUE, bTopDie = TRUE;
	BOOL bLLeftDie = FALSE, bLeftDie = FALSE, bBtmDie = FALSE, bULeftDie = FALSE, bLRightDie = FALSE;
	dX = 0.33 * GetDiePitchXX();
	dY = 0.33 * GetDiePitchYY();


	//Check center die
	bCenterDie = LookAroundNormalDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE);
	if (!bCenterDie && bPLLMSpecialFcn)		//v4.41T1
		bCenterDie = LookAroundNormalDie(nCPosnX, nCPosnY, WPR_CT_DIE, PR_TRUE, 1);

	//if center die not found, confirm with nearby dices on left/up
	if (!bCenterDie)
	{
		bRightDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE);
		if (!bRightDie && bPLLMSpecialFcn)		//v4.41T1
			bRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE, 1);
		bTopDie			= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);
		if (!bTopDie && bPLLMSpecialFcn)		//v4.41T1
			bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE, 1);
			
		//if (!bULeftDie || !bLeftDie || !bTopDie) 
		if (!bRightDie || !bTopDie) 
		{
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("LookAheadLLeftRectWaferWithInCompleteCorner Center-die not found.");

			CString szTemp;
			//szTemp.Format("LookAheadLRightRectWaferCorner FAIL: C=%d, UL=%d, L=%d, T=%d", bCenterDie, bULeftDie, bLeftDie, bTopDie);
			szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner FAIL: C=%d, R=%d, T=%d", bCenterDie, bRightDie, bTopDie);
			//Log_AlignWafer(szTemp);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}
	else
	{
		if (abs(nCPosnX) > dX)
		{
			CString szTemp;
			szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner FAIL pitch offset X: %d (%d)", abs(nCPosnX), dX);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}

		if (abs(nCPosnY) > dY)
		{
			CString szTemp;
			szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner FAIL pitch offset Y: %d (%d)", abs(nCPosnY), dY);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			stSrchLrCornerInfo.bLfResult	= FALSE;
			svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			return 1;
		}
	}

	ULONG ulCurrMapRow = (*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapRow"]; 
	ULONG ulCurrMapCol = (*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapCol"];
	
	//v4.14T8
	CString szTemp;
	LONG lUserRow = 0, lUserCol = 0;
	ConvertAsmToOrgUser(ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner die at Map I(%lu, %lu) - U(%lu, %lu)",
				  ulCurrMapRow, ulCurrMapCol, lUserRow, lUserCol);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	//v4.14T8
	ULONG ulCurrRow = 0, ulCurrCol = 0;
	LONG ulCurrUserRow = 0, ulCurrUserCol = 0;
	m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);
	ConvertAsmToOrgUser(ulCurrRow, ulCurrCol, ulCurrUserRow, ulCurrUserCol);
	szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner curr-die-pos at I(%lu, %lu) - U(%lu, %lu)",
				  ulCurrRow, ulCurrCol, ulCurrUserRow, ulCurrUserCol);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);


	BOOL bULDieExist = FALSE;
	if (WM_CWaferMap::Instance()->IsMapHaveBin(ulCurrMapRow - 1, ulCurrMapCol - 1))
	{
		bULDieExist = TRUE;
	}
	BOOL bLRDieExist = FALSE;
	if (WM_CWaferMap::Instance()->IsMapHaveBin(ulCurrMapRow + 1, ulCurrMapCol + 1))
	{
		bLRDieExist = TRUE;
	}


	bLLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DL_DIE, PR_FALSE);
	bLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE);
	bBtmDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_DN_DIE, PR_FALSE);
	bULeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UL_DIE, PR_FALSE);
	bLRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DR_DIE, PR_FALSE);
	bRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE);
	bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);
	//CString szTemp;

	// lower-right corners should have no dice
	if (!bLLeftDie && !bLeftDie && !bBtmDie)
	{
		if ((bULDieExist && !bULeftDie) || (!bULDieExist && bULeftDie))
		{
			szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner FAIL: UL=%d but MAP=%d", bULeftDie, bULDieExist);
		}
		else if ((bLRDieExist && !bLRightDie) || (!bLRDieExist && bLRightDie))
		{
			szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner FAIL: LR=%d but MAP=%d", bLRightDie, bLRDieExist);
		}
		else
		{
			stSrchLrCornerInfo.bLfResult = TRUE;		// LF result OK!!

			if (bCenterDie)
				szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner: OK: Last-die offset dX=%d(%.2f), dY=%d(%.2f)", 
							  nCPosnX, dX, nCPosnY, dY);
			else
			{
				szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner: center-die not found but OK (R=%d, T=%d)", bRightDie, bTopDie);
			}
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			
			szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner OK:  LL=%d, L=%d, B=%d, UL=%d(%d), LR=%d(%d)",
						  bLLeftDie, bLeftDie, bBtmDie, bULeftDie, bULDieExist, bLRightDie, bLRDieExist);	
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		}
	}
	else
	{
		szTemp.Format("LookAheadLLeftRectWaferWithInCompleteCorner FAIL:  LL=%d, L=%d, B=%d",
					  bLLeftDie, bLeftDie, bBtmDie);	
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}

	svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
	return 1;
}

LONG CWaferPr::LookAroundBaseOnMap(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		ULONG ulMapRow;
		ULONG ulMapCol;
		BOOL	bLfResult;
		BOOL	bFovSize;
		BOOL	bLatch;
	} SRCH_LR_TYPE;

	SRCH_LR_TYPE	stLookAroundInfo;

	svMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stLookAroundInfo);
	
	if (!IsLFSizeOK())
	{
		stLookAroundInfo.bFovSize	= FALSE;
		stLookAroundInfo.bLfResult	= FALSE;
		svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stLookAroundInfo);
		return 1;
	}

	ULONG ulCurrMapRow, ulCurrMapCol;
	BOOL bLLeftDie, bLeftDie, bBtmDie, bULeftDie, bLRightDie, bRightDie, bTopDie, bURightDie;
	BOOL bLLeftDieExist, bLeftDieExist, bBtmDieExist, bULeftDieExist, bLRightDieExist, bRightDieExist, bTopDieExist, bURightDieExist;
	INT nPosnX, nPosnY;

	ulCurrMapRow = stLookAroundInfo.ulMapRow;
	ulCurrMapCol = stLookAroundInfo.ulMapCol;
	
	bULeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UL_DIE, PR_FALSE);
	bTopDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, PR_FALSE);
	bLLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DL_DIE, PR_FALSE);
	bLeftDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, PR_FALSE);
	bBtmDie		= LookAroundNormalDie(nPosnX, nPosnY, WPR_DN_DIE, PR_FALSE);
	bURightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_UR_DIE, PR_FALSE);
	bLRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_DR_DIE, PR_FALSE);
	bRightDie	= LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, PR_FALSE);
	

	bULeftDieExist = TRUE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulCurrMapRow - 1, ulCurrMapCol - 1))
	{
		bULeftDieExist = FALSE;
	}

	bTopDieExist = TRUE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulCurrMapRow - 1, ulCurrMapCol))
	{
		bTopDieExist = FALSE;
	}

	bURightDieExist = TRUE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulCurrMapRow - 1, ulCurrMapCol + 1))
	{
		bURightDieExist = FALSE;
	}

	bLeftDieExist = TRUE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulCurrMapRow, ulCurrMapCol - 1))
	{
		bLeftDieExist = FALSE;
	}

	bRightDieExist = TRUE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulCurrMapRow, ulCurrMapCol + 1))
	{
		bRightDieExist = FALSE;
	}

	bLLeftDieExist = TRUE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulCurrMapRow + 1, ulCurrMapCol - 1))
	{
		bLLeftDieExist = FALSE;
	}

	bBtmDieExist = TRUE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulCurrMapRow + 1, ulCurrMapCol))
	{
		bBtmDieExist = FALSE;
	}

	bLRightDieExist = TRUE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulCurrMapRow + 1, ulCurrMapCol + 1))
	{
		bLRightDieExist = FALSE;
	}

	if ((bULeftDieExist != bULeftDie) || (bTopDieExist != bTopDie) || (bURightDieExist != bURightDie) ||
			(bLeftDieExist != bLeftDie) || (bRightDieExist != bRightDie) || (bLLeftDieExist != bLLeftDie) || 
			(bBtmDieExist != bBtmDie) || (bLRightDieExist != bLRightDie))
	{
		stLookAroundInfo.bFovSize	= TRUE;
		stLookAroundInfo.bLfResult = FALSE;
		svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stLookAroundInfo);
		return 1;
	}

	stLookAroundInfo.bFovSize	= TRUE;
	stLookAroundInfo.bLfResult = TRUE;
	svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stLookAroundInfo);
	return 1;
	
}


//================================================================
// LookAroundNormalDie()
//   Created-By  : Andrew Ng
//   Date        : 4/24/2008 12:11:05 PM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferPr::LookAroundNormalDie(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG	lLFRegionSize;
		LONG	lLFPosn;
		LONG	lDistX;
		LONG	lDistY;
		BOOL	bLfResult;
		BOOL	bFovSize;
		BOOL	bLatch;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stLookAroundInfo;
	svMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stLookAroundInfo);


//CString szTemp;
//szTemp.Format("CWaferPrStn::LookAroundNormalDie  LF = %d", stLookAroundInfo.lLFPosn);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);


	//Return FALSE if FOV size is too small
	if (!IsLFSizeOK())
	{
		stLookAroundInfo.bFovSize	= FALSE;
		stLookAroundInfo.bLfResult	= FALSE;
		svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stLookAroundInfo);
		return 1;
	}
	else
	{
		stLookAroundInfo.bFovSize = TRUE;
	}

	stLookAroundInfo.bFovSize	= TRUE;
	stLookAroundInfo.bLfResult	= FALSE;
	stLookAroundInfo.lDistX		= 0;
	stLookAroundInfo.lDistY		= 0;

	BOOL bLFResult = TRUE;
	INT nPosnX = 0, nPosnY = 0;
	LONG lLFPosn		= stLookAroundInfo.lLFPosn;
	INT nLFRegionSize	= stLookAroundInfo.lLFRegionSize;
	PR_BOOLEAN bLatch	= PR_TRUE;
	if (!stLookAroundInfo.bLatch)
	{
		bLatch = PR_FALSE;
	}


	if (nLFRegionSize == REGION_SIZE_3x3)
	{
		//Determine LF posn within 3x3 region
		//Use original, old 3x3 LF Search-die cmd here
		switch (lLFPosn)
		{
			case 1:
				bLFResult = LookAroundNormalDie(nPosnX, nPosnY, WPR_UL_DIE, bLatch);
				break;
			case 2:
				bLFResult = LookAroundNormalDie(nPosnX, nPosnY, WPR_UP_DIE, bLatch);
				break;
			case 3:
				bLFResult = LookAroundNormalDie(nPosnX, nPosnY, WPR_UR_DIE, bLatch);
				break;
			case 4:
				bLFResult = LookAroundNormalDie(nPosnX, nPosnY, WPR_LT_DIE, bLatch);
				break;
			case 5:
				bLFResult = LookAroundNormalDie(nPosnX, nPosnY, WPR_RT_DIE, bLatch);
				break;
			case 6:
				bLFResult = LookAroundNormalDie(nPosnX, nPosnY, WPR_DL_DIE, bLatch);
				break;
			case 7:
				bLFResult = LookAroundNormalDie(nPosnX, nPosnY, WPR_DN_DIE, bLatch);
				break;
			case 8:
				bLFResult = LookAroundNormalDie(nPosnX, nPosnY, WPR_DR_DIE, bLatch);
				break;
			case 0:
			default:
				bLFResult = LookAroundNormalDie(nPosnX, nPosnY, WPR_CT_DIE, bLatch);
				break;
		}
	}
	else
	{
		//Support other LF region size
		//Please check WT_1stDieFinder.h for available LF region size "nLFRegionSize"
		bLFResult = LookAroundNormalDie_mxn(nPosnX, nPosnY, nLFRegionSize, lLFPosn, bLatch);
	}


	stLookAroundInfo.bLfResult = bLFResult;
	if (bLFResult) 
	{
		stLookAroundInfo.lDistX = nPosnX;
		stLookAroundInfo.lDistY = nPosnY;
	}

	svMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stLookAroundInfo);
	return 1;
}


LONG CWaferPr::GetFOVSize(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		DOUBLE	dFOVSizeX;
		DOUBLE	dFOVSizeY;
	} FOV_TYPE;
	FOV_TYPE	stFOVInfo;

	stFOVInfo.dFOVSizeX = stFOVInfo.dFOVSizeY = m_dLFSize;

	svMsg.InitMessage(sizeof(FOV_TYPE), &stFOVInfo);
	return 1;
}


//================================================================
// SearchAlignPattern()
//   Created-By  : Andrew Ng
//   Date        : 5/7/2009 5:05:44 PM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferPr::SearchAlignPattern(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		BOOL bStatus;
		BOOL bLFStatus;
		BOOL bCGoodDie;
		LONG lCX;
		LONG lCY;
	} REF_TYPE;

	REF_TYPE	stInfo;
	stInfo.bLFStatus	= TRUE;
	stInfo.bStatus		= FALSE;


	//Get pattern ID from WT
	UCHAR ucPatternID = 0;
	svMsg.GetMsg(sizeof(UCHAR), &ucPatternID);

	switch (ucPatternID)
	{
		case 1:				//NeoNeon
			break;
		case 0:
		default:			//Not supported!!
			stInfo.bStatus = FALSE;
			svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
			return 1;
	}


	//Return FALSE if FOV size is too small
	if (!IsLFSizeOK())
	{
		stInfo.bLFStatus	= FALSE;		//LF status not passed!
		stInfo.bStatus		= FALSE;
		stInfo.bCGoodDie	= FALSE;
		svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
		return 1;
	}


	//Grab 1st image for center-die
	int nPosnX, nPosnY;
	int nCPosnX, nCPosnY;
	BOOL bDieResult[26];	//[0] not used
	bDieResult[13] = LookAroundNormalDie_mxn(nCPosnX, nCPosnY, REGION_SIZE_5x5, 13, PR_TRUE);		//Center die


	//Check other LF windows for the pattern
	for (int i = 1; i <= 25; i++)
	{
		//Determine LF posn within 3x3 region
		//Use original, old 3x3 LF Search-die cmd here
		if (i == 13)
		{
			continue;
		}
		bDieResult[i] = LookAroundNormalDie_mxn(nPosnX, nPosnY, REGION_SIZE_5x5, i, PR_FALSE);
	}


	//If center-die is found -> pattern not found!
	stInfo.bCGoodDie = bDieResult[13];
	if (stInfo.bCGoodDie) 
	{
		stInfo.bStatus = FALSE;		//pattern not found although center die is found!
		stInfo.lCX = nCPosnX;
		stInfo.lCY = nCPosnY;

		svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
		return 1;
	}


	//5x5 region
	//===============
	// 1  2  3  4  5
	// 6  7  8  9  10
	// 11 12 13 14 15
	// 16 17 18 19 20
	// 21 22 23 24 25
	//===============
	
	//5x5 region (NeoNeon)
	//===============
	// -  o  o  o  -
	// o  x  x  x  o
	// o  x  x  x  o
	// o  x  x  x  o
	// -  o  o  o  -
	//===============


	//NeoNeon pattern
	//FIrst Check for 3x3 empty pattern
	if (!bDieResult[7] && !bDieResult[8] && !bDieResult[9] &&
			!bDieResult[12] && !bDieResult[13] && !bDieResult[14] &&
			!bDieResult[17] && !bDieResult[18] && !bDieResult[19])
	{
		//Then check for existence of surrounding dices
		if (bDieResult[2] && bDieResult[3] && bDieResult[4] &&
				bDieResult[6] && bDieResult[11] && bDieResult[16] &&
				bDieResult[10] && bDieResult[15] && bDieResult[20] &&
				bDieResult[22] && bDieResult[23] && bDieResult[24])
		{
			stInfo.bStatus = TRUE;			//NeoNeon Pattern found!!
		}
	}

	svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
	return 1;
}


LONG CWaferPr::TurnOnPostSealLighting(IPC_CServiceMessage &svMsg)			//v3.71T5	//PLLM REBEL
{
	BOOL bReturn = TRUE;
	BOOL bOn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetFeatureStatus(MS896A_FUNC_VISION_POSTSEAL_OPTICS))
	{
		SetPostSealLighting(bOn, 1);
	}


	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::ShowPrDiagnosticDialog(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	PR_VISIONNT_DLG_CMD	stCmd;
	PR_VISIONNT_DLG_RPY stRpy;

	PR_InitVisionNtDlgCmd(&stCmd);
	stCmd.emDlg = PR_VISIONNT_DLG_DIAGNOSTIC;
	PR_VisionNtDlgCmd(&stCmd, ubSID, ubRID, &stRpy);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ShowPrServiceDialog(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	PR_VISIONNT_DLG_CMD	stCmd;
	PR_VISIONNT_DLG_RPY stRpy;

	PR_InitVisionNtDlgCmd(&stCmd);
	stCmd.emDlg = PR_VISIONNT_DLG_SERVICE;
	stCmd.coDlgOffset.x = 348;
	stCmd.coDlgOffset.y = 118;
	PR_VisionNtDlgCmd(&stCmd, ubSID, ubRID, &stRpy);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}




LONG CWaferPr::CheckRecordID(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lPrID;

	svMsg.GetMsg(sizeof(LONG), &lPrID);

	if ((lPrID == 0) || (lPrID == (LONG)GetDiePrID(0)))
	{
		m_ssGenPRSrchID[0]	= 0;
		m_bGenDieLearnt[0]	= FALSE;
		m_stGenDieSize[0].x = 0;
		m_stGenDieSize[0].y = 0;
		m_bDieCalibrated	= FALSE;	
		m_bDieIsLearnt		= FALSE;		
		m_lCurNormDieID		= 0;
		m_lCurNormDieSizeX	= 0; 
		m_lCurNormDieSizeY	= 0;
		m_dCurNormDieSizeX	= 0; 
		m_dCurNormDieSizeY	= 0;
		m_lCurBadCutSizeX	= 0; 
		m_lCurBadCutSizeY	= 0;

		SavePrData(TRUE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::IsDieLearnt(IPC_CServiceMessage &svMsg)
{
	BOOL bDieLearnt = TRUE;

	if( IsBurnIn() && m_bNoWprBurnIn )
	{
		svMsg.InitMessage(sizeof(BOOL),	&bDieLearnt);
		return 1;
	}

	if (IsNormalDieLearnt() == FALSE)
	{
		bDieLearnt = FALSE;
		SetAlert(IDS_WPR_DIENOTLEARNT);
	}

	//v4.54A5	//MS60 NGPick mode; make sure NormalDie #2 is learnt for EMPTY hole pattern
	if (CMS896AStn::m_bMS60NGPick)
	{
		if (IsMS60NGPickEmptyHoleLearnt() == FALSE)
		{
			bDieLearnt = FALSE;
			//SetAlert(IDS_WPR_DIENOTLEARNT);
			HmiMessage_Red_Back("Normal Die pattern #2 is not learnt for NGPick !", "Wafer PR");
		}
	}

	if (m_bDieCalibrated == FALSE)
	{
		bDieLearnt = FALSE;
		SetAlert(IDS_WPR_DIENOTCALIBRATED);
	}

	//v2.82
	//lDieNo = WPR_GEN_RDIE_OFFSET + lInputDieNo;
	if (m_bRefDieCheck && 
			((IsThisDieLearnt(WPR_GEN_RDIE_OFFSET + 1) == FALSE) || (GetDiePrID(WPR_GEN_RDIE_OFFSET + 1) <= 0)))
	{
		bDieLearnt = FALSE;
		SetAlert(IDS_WPR_REFDIENOTLEARNT);
		SetErrorMessage("WPR: PreStart Cycle fail because Ref-die record not available!");
	}

	//v2.83T53	//PLLM
	if (m_bRefDieCheck && (m_lLrnTotalRefDie <= 0))
	{
		bDieLearnt = FALSE;
		SetAlert(IDS_WPR_REFDIENOTLEARNT);
		SetErrorMessage("WPR: PreStart Cycle fail because Ref-die record not exist!");
	}

	//BlockPick (with regular ref die) doesn't need this checking
	if (m_bFullRefBlock == FALSE)	//block2
	{
		//v3.30T2	
		//Block pick algorithm must use look-forward mode
		CString szAlgorithm;
		CString szPathFinder;
		m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
		if (szAlgorithm == "Block Algorithm")
		{
			if ((!m_bSrchEnableLookForward) || (!IsLFSizeOK()))
			{
				bDieLearnt = FALSE;
				HmiMessage("ERROR: invalid wafer PR FOV to run block-pick mode!");
			}
		}
	}

	//v4.49A7	//Osram
	//CHeck each valid wafer record IDs and find if conflict with BOND pr records
	BOOL bCheckIDOK = TRUE;
	for (INT m=0; m<WPR_MAX_DIE; m++)
	{
		if (m_bGenDieLearnt[m] == TRUE)
		{
			bCheckIDOK = CheckWaferIDWithBondIDs((LONG) m_ssGenPRSrchID[m]);

			if (!bCheckIDOK)
			{
				bDieLearnt = FALSE;
				SetErrorMessage("ERROR: invalid wafer PR IDs with conflicts with Bond PR IDs");
				break;
			}
		}
	}

	//xyz
	//Check for Master pitch before AUTOBOND, if enabled
	double dCurrPitchinMilX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)GetDiePitchXX() * 1000 / 25.4);
	double dCurrPitchinMilY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)GetDiePitchYY() * 1000 / 25.4);
	double dDx = fabs(m_dWAlignMasterPitchInMilX - dCurrPitchinMilX);
	double dDy = fabs(m_dWAlignMasterPitchInMilY - dCurrPitchinMilY);
	if ((m_dWAlignMasterPitchInMilX > 0) && (dDx > (m_dWAlignMasterPitchInMilX * WPR_DIE_PITCH_TOLERANCE)))
	{
		CString szTemp;
		szTemp.Format("Warning: Current x-pitch is different from master-pitch-x by more than 30%");
		SetErrorMessage(szTemp);
		SetAlert_Red_Yellow(IDS_WPR_LRN_PITCH_INVALID_MASTER_X);
		bDieLearnt = FALSE;
	}
	if ((m_dWAlignMasterPitchInMilY > 0) && (dDy > (m_dWAlignMasterPitchInMilY * WPR_DIE_PITCH_TOLERANCE)))
	{
		CString szTemp;
		szTemp.Format("Warning: Current y-pitch is different from master-pitch-y by more than 30%");
		SetErrorMessage(szTemp);
		SetAlert_Red_Yellow(IDS_WPR_LRN_PITCH_INVALID_MASTER_Y);
		bDieLearnt = FALSE;
	}
	//}

	svMsg.InitMessage(sizeof(BOOL), &bDieLearnt);
	return 1;
}

LONG CWaferPr::IsInspectionSet(IPC_CServiceMessage &svMsg)
{
	BOOL bRtn = TRUE;

	if (IsPrescanEnable() && IsThisDieLearnt(GetPrescanPrID() - 1) != TRUE)
	{
		CString szMsg;
		szMsg.Format("Prescan Normal die PR %d not learnt yet!", GetPrescanPrID());
		HmiMessage_Red_Back(szMsg);
		bRtn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bRtn);
		return 1;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( IsAutoRescanEnable() && pUtl->GetPrAbnormal() )
	{
		GetImageNumInGallery();
		if( m_pPrescanPrCtrl->RecoverScanChannels()==false )
		{
			CString szMsg;
			szMsg.Format("Scan PR abnormal still recovering, please wait!");
			HmiMessage_Red_Back(szMsg);
			SaveScanTimeEvent(szMsg);
			bRtn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bRtn);
			return 1;
		}
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if( IsBurnIn()==FALSE && (m_bWarningNoInspection) )
	{
		if ((m_dGenSrchSingleDefectArea[0] == 0 && m_dGenSrchTotalDefectArea[0] == 0 && m_dGenSrchChipArea[0] == 0)
				|| (m_bGenSrchEnableDefectCheck[0] == FALSE && m_bGenSrchEnableChipCheck[0] == FALSE))
		{
			LONG lTmp;
			lTmp = HmiMessage("No inspection setting in Wafer PR.\n\nContinue bonding or Stop?", "Warning: No Inspection",
							  glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			if (lTmp == glHMI_STOP)
			{
				bRtn = FALSE;
			}
			else
			{
				//Once this checking is by-passed, it will not be checked again until machine starts up next time
				m_bWarningNoInspection = FALSE;
			}
		}
		if (pApp->GetCustomerName() == "Cree")	
		{
			if (!IsLFSizeOK())
			{
				HmiMessage("Warning: wafer PR FOV is smaller than 3.3!");
				SetErrorMessage("Warning: wafer PR FOV is smaller than 3.3!");
			}
		}
	}

	if (IsPrescanEnable() && m_lPrescanSrchDieScore == 0)
	{
		CString szMsg;
		szMsg.Format("Prescan pass score is 0! Please chcek");
		HmiMessage_Red_Back(szMsg);
		bRtn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bRtn);
		return 1;
	}

	if ((pApp->GetCustomerName() == "Cree") && m_bCheckAllNmlDie)
	{
		LONG lX1 = 0, lX2 = 0, lX3 = 0, lY1 = 0, lY2 = 0, lY3 = 0;
		lX1	= _round(GetGenSrchAreaX(WPR_GEN_NDIE_OFFSET + 1));
		lY1	= _round(GetGenSrchAreaY(WPR_GEN_NDIE_OFFSET + 1));
		lX2	= _round(GetGenSrchAreaX(WPR_GEN_NDIE_OFFSET + 2));
		lY2	= _round(GetGenSrchAreaY(WPR_GEN_NDIE_OFFSET + 2));
		lX3	= _round(GetGenSrchAreaX(WPR_GEN_NDIE_OFFSET + 3));
		lY3	= _round(GetGenSrchAreaY(WPR_GEN_NDIE_OFFSET + 3));
		if (lX1 != lX2 || lX1 != lX3 || lY1 != lY2 || lY1 != lY3)
		{
			LONG lTmp;
			lTmp = HmiMessage("All Normal Die search window not same.\n\nContinue bonding or Stop?", "Warning: Search Window",
							  glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			if (lTmp == glHMI_STOP)
			{
				bRtn = FALSE;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bRtn);
	return 1;
}


LONG CWaferPr::SearchDieInWafer(IPC_CServiceMessage &svMsg)
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	
	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX(), GetPRWinLRY()};

	BOOL	bPrType;
	LONG	lPrId;
	LONG	lSrchDieMsg[2];

	typedef struct
	{
		LONG lSrchMode;
		LONG lX;
		LONG lY;
		LONG lDigit1;
		LONG lDigit2;
		LONG lResult;
	} WTPos;
	WTPos stPRS;

	int		iOffsetX, iOffsetY;

	if (PR_NotInit())
	{
		stPRS.lSrchMode = 0;
		//stPRS.bRtn = FALSE;
		stPRS.lX = 0;
		stPRS.lY = 0;
		stPRS.lDigit1 = 0;
		stPRS.lDigit2 = 0;
		stPRS.lResult = 0;
		svMsg.InitMessage(sizeof(WTPos), &stPRS);
		return 1;
	}
	
	// Get Search Message
	svMsg.GetMsg(2 * sizeof(LONG), lSrchDieMsg);

	if (lSrchDieMsg[1] == 0)
	{
		bPrType = WPR_NORMAL_DIE;
		lPrId = 1;
	}
	else
	{
		bPrType = WPR_REFERENCE_DIE;
		lPrId = 1;
	}

	PR_WORD wResult = 0;

	switch (lSrchDieMsg[0])
	{
		case 0:
			wResult = ManualSearchDie(bPrType, lPrId, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
			if (wResult != -1)
			{
				if (DieIsAlignable(usDieType) == TRUE)
				{
					//Allow die to rotate
					//ManualDieCompenate(stDieOffset, fDieRotate);
					CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);

					stPRS.lSrchMode = 0;
					//stPRS.bRtn = TRUE;
					stPRS.lX = iOffsetX;
					stPRS.lY = iOffsetY;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 1;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
				else
				{
					stPRS.lSrchMode = 0;
					//stPRS.bRtn = FALSE;
					stPRS.lX = 0;
					stPRS.lY = 0;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 0;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
			}
			break;

		case 1:
			//Set & Display Max search die area
			wResult = ManualSearchDie(bPrType, lPrId, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
			if (wResult != -1)
			{
				if (DieIsAlignable(usDieType) == TRUE)
				{
					CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);
					stPRS.lSrchMode = 0;
					//stPRS.bRtn = TRUE;
					stPRS.lX = (LONG)iOffsetX;
					stPRS.lY = (LONG)iOffsetY;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 1;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
				else
				{
					stPRS.lSrchMode = 0;
					//stPRS.bRtn = FALSE;
					stPRS.lX = 0;
					stPRS.lY = 0;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 0;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
			}
			else
			{
				stPRS.lSrchMode = 0;
				//stPRS.bRtn = FALSE;
				stPRS.lX = 0;
				stPRS.lY = 0;
				stPRS.lDigit1 = 0;
				stPRS.lDigit2 = 0;
				stPRS.lResult = 0;
				svMsg.InitMessage(sizeof(WTPos), &stPRS);
			}

			if (m_bAllRefDieCheck && stPRS.lResult == 0)
			{
				LONG i;
				PR_WORD wRtn = 0;

				for (i = 2; i <= m_lLrnTotalRefDie; i++)
				{
					//v4.43T11
					if (bPrType && m_bEnableMS100EjtXY && (i == 2))	// Ej Cap pattern
					{
						continue;
					}
					if (bPrType && m_bEnableMS100EjtXY && (i == 3))	// Collet hole
					{
						continue;
					}

					wRtn = ManualSearchDie(bPrType, i, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
					if (wRtn != -1)
					{
						if (DieIsAlignable(usDieType) == TRUE)
						{
							CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);
							stPRS.lSrchMode = 0;
							//stPRS.bRtn = TRUE;
							stPRS.lX = (LONG)iOffsetX;
							stPRS.lY = (LONG)iOffsetY;
							stPRS.lDigit1 = 0;
							stPRS.lDigit2 = 0;
							stPRS.lResult = 1;
							svMsg.InitMessage(sizeof(WTPos), &stPRS);
							break;
						}
					}
				}
			}

			break;
		default:
			;
	}

	return 1;
}


LONG CWaferPr::SearchDigitalInWafer(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lDigit1;
		LONG lDigit2;
		LONG lResult;
	} WT_Die;
	WT_Die stPRS;

	//Search die
	PR_REAL		fDieRotate; 
	PR_COORD	stDieOffset; 
	int			iOffsetX = 0;
	int			iOffsetY = 0;

	LONG lRtn;
	PR_RCOORD	stDieScore;
	PR_COORD	stDieDigit;

	stDieDigit.x = -1;
	stDieDigit.y = -1;
	stDieScore.x = 0;
	stDieScore.y = 0;
	fDieRotate = 0;

	lRtn = SearchNumberDie(&stDieOffset, &fDieRotate, &stDieScore, &stDieDigit);
	if (lRtn == FALSE)
	{
		stPRS.lResult = 0;			//No Die
		stPRS.lDigit1 = -1;
		stPRS.lDigit2 = -1;
		stPRS.lX = 0;
		stPRS.lY = 0;
	}
	else
	{
		stPRS.lResult = 1;
		stPRS.lDigit1 = (LONG)stDieDigit.x;
		stPRS.lDigit2 = (LONG)stDieDigit.y;
		CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);
		stPRS.lX = (LONG)iOffsetX;
		stPRS.lY = (LONG)iOffsetY;
	}

	svMsg.InitMessage(sizeof(WT_Die), &stPRS);

	return 1;
}


LONG CWaferPr::SearchNmlDieAround(IPC_CServiceMessage &svMsg)
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX(), GetPRWinLRY()};

	LONG	lSrchDieMsg[2];

	typedef struct
	{
		LONG lSrchMode;
		LONG lX;
		LONG lY;
		LONG lResult;
	} WTPos;
	WTPos stPRS;

	int	iOffsetX, iOffsetY;

	if (PR_NotInit())
	{
		stPRS.lSrchMode = 0;
		stPRS.lX = 0;
		stPRS.lY = 0;
		stPRS.lResult = 0;
		svMsg.InitMessage(sizeof(WTPos), &stPRS);
		return 1;
	}

	LONG lResFOV, lDir;
	// Get Search Message
	svMsg.GetMsg(2 * sizeof(LONG), lSrchDieMsg);
	lResFOV = lSrchDieMsg[0];
	lDir = lSrchDieMsg[1];

	PR_UWORD uwResult = 0;

	uwResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_FALSE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, m_stLFSearchArea[lDir].coCorner1, m_stLFSearchArea[lDir].coCorner2);
	if (uwResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);

			stPRS.lSrchMode = 0;
			stPRS.lX = iOffsetX;
			stPRS.lY = iOffsetY;
			stPRS.lResult = 1;
			svMsg.InitMessage(sizeof(WTPos), &stPRS);
		}
		else
		{
			stPRS.lSrchMode = 0;
			stPRS.lX = 0;
			stPRS.lY = 0;
			stPRS.lResult = 0;
			svMsg.InitMessage(sizeof(WTPos), &stPRS);
		}
	}

	return 1;
}


//Learn more die commands
LONG CWaferPr::AdaptiveLearnDie(IPC_CServiceMessage &svMsg)
{
	PR_ADAPT_LRN_DIE_CMD	stAdaptLrnDieCmd;
	PR_ADAPT_LRN_DIE_RPY1	stAdaptLrnDieRpy1;
	PR_ADAPT_LRN_DIE_RPY2	stAdaptLrnDieRpy2;
	PR_GEN_DIE_RECORD_CMD	stGenDieCmd;
	PR_GEN_DIE_RECORD_RPY	stGenDieRpy;

	CString	csMsgA, szTitle;
	BOOL	bReply = TRUE;
	BOOL	bLearnOK = FALSE;


	//Init learn die structure
	PR_InitAdaptLrnDieCmd(&stAdaptLrnDieCmd); 
	stAdaptLrnDieCmd.emCameraNo = GetRunCamID();
	stAdaptLrnDieCmd.uwRecordID = GetDiePrID(WPR_NORMAL_DIE);
	stAdaptLrnDieCmd.emAdaptAlg = PR_DEF_ADAPT_LRN_DIE_ALG;


	szTitle.LoadString(HMB_WPR_ADAPTIVE_LRN_DIE);


	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	//Start learn more die
	PR_AdaptLrnDieCmd(&stAdaptLrnDieCmd, ubSID, ubRID, &stAdaptLrnDieRpy1);
	if (stAdaptLrnDieRpy1.uwCommunStatus != PR_COMM_NOERR)
	{
		//HmiMessage("PR Reply1 Comm Error" ,"Adaptive learn die message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
		csMsgA = "PR Reply1 Comm Error";
		SetAlert_Msg(IDS_WPR_ADAPTIVE_LRN_DIE, csMsgA);
		bReply = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	if (stAdaptLrnDieRpy1.uwPRStatus != PR_ERR_NOERR)
	{
		csMsgA.Format("PR Reply Error = %d", stAdaptLrnDieRpy1.uwPRStatus);
		//HmiMessage(csMsgA ,"Adaptive learn die message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
		SetAlert_Msg(IDS_WPR_ADAPTIVE_LRN_DIE, csMsgA);
		bReply = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	PR_AdaptLrnDieRpy(ubSID, &stAdaptLrnDieRpy2);
	if (stAdaptLrnDieRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
	{
		//HmiMessage("PR Reply1 Comm Error" ,"Adaptive learn die message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
		csMsgA = "PR Reply1 Comm Error";
		SetAlert_Msg(IDS_WPR_ADAPTIVE_LRN_DIE, csMsgA);
		bReply = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	switch (stAdaptLrnDieRpy2.stStatus.uwPRStatus)
	{
		case PR_ERR_PAR:	
			csMsgA.LoadString(HMB_WPR_LRN_DIE_FAILED_1);
			break;

		case PR_ERR_NOREF:
			csMsgA.LoadString(HMB_WPR_LRN_DIE_FAILED_2);
			break;

		case PR_ERR_PID_NOT_LD:
			csMsgA.LoadString(HMB_WPR_LRN_DIE_FAILED_3);
			break;

		case PR_ERR_ROTATION:
			csMsgA.LoadString(HMB_WPR_LRN_DIE_FAILED_4);
			break;

		case PR_ERR_NO_DIE:
			csMsgA.LoadString(HMB_WPR_LRN_DIE_FAILED_5);
			break;

		case PR_ERR_DEFECTIVE_DIE:
			csMsgA.LoadString(HMB_WPR_LRN_DIE_FAILED_6);
			break;

		case PR_ERR_ALG_NOT_LRN:
			csMsgA.LoadString(HMB_WPR_LRN_DIE_FAILED_7);
			break;

		case PR_ERR_LOCATED_DEFECTIVE_DIE:
			csMsgA.LoadString(HMB_WPR_LRN_DIE_FAILED_8);
			break;

		case PR_ERR_NON_LOCATED_DEFECTIVE_DIE:
			csMsgA.LoadString(HMB_WPR_LRN_DIE_FAILED_9);
			break;

		case PR_WARN_SMALL_SRCH_WIN:
		case PR_WARN_TOO_MANY_DIE:
		case PR_ERR_NOERR:
			bLearnOK = TRUE;
			csMsgA.LoadString(HMB_WPR_LRN_DIE_OK);
			break;

		default:
			csMsgA.Format("Other error = %d", stAdaptLrnDieRpy2.stStatus.uwPRStatus); 
			SetAlert_Msg(IDS_WPR_ADAPTIVE_LRN_DIE, csMsgA);
			bReply = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
			break;
	}


	if (bLearnOK == TRUE)
	{
		PR_InitGenDieRecordCmd(&stGenDieCmd); 
		stGenDieCmd.emCameraNo = GetRunCamID();
		UCHAR ucTempChar = (UCHAR)( m_bSelectDieType * m_lCurRefDieNo );
		stGenDieCmd.uwRecordID = GetDiePrID(ucTempChar);

		PR_GenDieRecordCmd(&stGenDieCmd, ubSID, ubRID, &stGenDieRpy);
		if (stGenDieRpy.uwCommunStatus != PR_COMM_NOERR)
		{
			//HmiMessage("PR GenDie Reply Comm Error" ,"Adaptive learn die message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			csMsgA = "PR GenDie Reply Comm Error";
			SetAlert_Msg(IDS_WPR_ADAPTIVE_LRN_DIE, csMsgA);
			bReply = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
		
		if (stGenDieRpy.uwPRStatus != PR_ERR_NOERR)
		{
			//HmiMessage(csMsgA ,"Adaptive learn die message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			csMsgA.Format("PR GenDie Reply Error = %d", stGenDieRpy.uwPRStatus);
			SetAlert_Msg(IDS_WPR_ADAPTIVE_LRN_DIE, csMsgA);
			bReply = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}

	HmiMessage(csMsgA , szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	ChangeCamera(WPR_CAM_WAFER);


	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferPr::SelectLearnCharDie(IPC_CServiceMessage &svMsg)
{
	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	if (bEnable)
	{
		m_lLrnAlignAlgo = 1;    // Force to use Pattern Matching method for char die
	}	
	else
	{
		m_lLrnAlignAlgo = 2;    // Force to use Edge Matching method for ref die
	}	

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;

}

//Select Character die no
LONG CWaferPr::SelectCharDieNo(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;
	LONG lDigitNo = 0;


	//Get digital no from HMI	
	svMsg.GetMsg(sizeof(LONG), &lDigitNo);
	
	m_lCurRefDieNo = lDigitNo + WPR_GEN_CDIE_OFFSET;


	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

//Search Character Die by Hotkey F9
LONG CWaferPr::UserSearchCharDie(IPC_CServiceMessage &svMsg)
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_RCOORD	stDieScore;
	BOOL		bReturn = TRUE;
	BOOL		bIsCharDie = FALSE;
	LONG		lDigit = 0;

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	PR_WIN stArea;
	GetSearchDieArea(&stArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET);
	PR_COORD stCorner1, stCorner2;
	stCorner1 = stArea.coCorner1;
	stCorner2 = stArea.coCorner2;

	DrawRectangleBox(stCorner1, stCorner2, PR_COLOR_RED);

	ManualSearchCharDie(&usDieType, &fDieRotate, &stDieOffset, &stDieScore);

	if (DieIsAlignable(usDieType) == TRUE)
	{
		ManualDieCompenate(stDieOffset, fDieRotate);
		DrawRectangleBox(stCorner1, stCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleDieSize(PR_COLOR_GREEN);
		bReturn = TRUE;
	}
	else
	{
		bReturn = FALSE;
	}

	//Display Message on HMI
	DisplaySearchCharDieResult(usDieType, fDieRotate, stDieOffset, stDieScore);
	//On Joystick & Clear PR screen
	SetWaferTableJoystick(TRUE);
	ChangeCamera(WPR_CAM_WAFER);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

//not use in block2
LONG CWaferPr::SearchCharDieInFOV(IPC_CServiceMessage &svMsg)
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_UWORD	usSearchResult = 0;
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore; 
	PR_COORD	stDieOffset; 
	PR_RCOORD	stCharDieScore;
	PR_COORD	stCharDieDigit;

	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX(), GetPRWinLRY()};

	int			iOffsetX = 0;
	int			iOffsetY = 0;

	typedef struct
	{
		LONG lStatus;
		LONG lX;
		LONG lY;
		LONG lDigit1;
		LONG lDigit2;
	} DIE_POS;

	DIE_POS stInfo;

	//Search any reference die in max search area
	usSearchResult = ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
	if (DieIsAlignable(usDieType) == TRUE)
	{
		//search character die
		usDieType = SearchCharDie(stDieOffset, &stCharDieScore, &stCharDieDigit);
		CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);

		if (DieIsGood(usDieType) == TRUE)
		{
			stInfo.lStatus = 2;		//Character die is found
			stInfo.lDigit1 = (LONG)stCharDieDigit.x;
			stInfo.lDigit2 = (LONG)stCharDieDigit.y;
		}
		else
		{
			stInfo.lStatus = 1;		//alignable die is found
			stInfo.lDigit1 = -1;
			stInfo.lDigit2 = -1;
		}
	}
	else
	{
		stInfo.lStatus = 0;			//No ref die is found
		stInfo.lDigit1 = -1;
		stInfo.lDigit2 = -1;
	}

	stInfo.lX = (LONG)iOffsetX;
	stInfo.lY = (LONG)iOffsetY;
	svMsg.InitMessage(sizeof(DIE_POS), &stInfo);

	return 1;
}

VOID CWaferPr::SetPRSysBondMode(IPC_CServiceMessage &svMsg)
{
	AutoBondScreen(TRUE);

	BOOL bTemp = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bTemp);

	return;
}

VOID CWaferPr::SetPRSysLiveMode(IPC_CServiceMessage &svMsg)
{
	if (m_bUseAutoBondMode)
	{
		AutoBondScreen(FALSE);
	}

	BOOL bTemp = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bTemp);

	return;
}


LONG CWaferPr::HotKeySearchRefDie(IPC_CServiceMessage &svMsg)
{
	//v4.47A5	//PLLM MS109 PR issue
	m_bCurrentCamera = WPR_CAM_WAFER;
	ChangeCamera(m_bCurrentCamera);

	if (m_bUseRefDieCheck == TRUE)
	{
		IPC_CServiceMessage stMsg;

		if (IsCharDieInUse())
		{
			CString pszText1 = "Reference";
			CString pszText2 = "Character";
			CString pszText3 = "Cancel";
			LONG lSelection = 0;

			CString szTitle, szContent;

			szTitle.LoadString(HMB_WPR_SRH_REF_DIE);
			szContent.LoadString(HMB_WPR_SELECT_SRH_TYPE);

			lSelection = HmiMessage(szContent, szTitle, glHMI_MBX_TRIPLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, &pszText1, &pszText2, &pszText3);
			
			switch (lSelection)
			{
			case 1:		//Search Normal Ref Die
				UserSearchRefDie(stMsg);
				break;

			case 5:		//Search Char. Ref Die
				UserSearchCharDie(stMsg);
				break;

			default:
				break;
			}
		}
		else
		{
			//Search Normal Ref Die
			if (m_bAllRefDieCheck)		//Search using all reference die
			{
				UserSearchAllRefDie(stMsg);
			}
			else		//Search using one reference die
			{
				UserSearchRefDie(stMsg);
			}
		}
	}

	BOOL bTemp = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bTemp);

	return 1;
}

LONG CWaferPr::BadCutBinaryThreshold(IPC_CServiceMessage &svMsg)
{
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();
	if (m_bBadcutBiDetect && m_bBadCutDetection)
	{
		PR_DISP_BIN_IMG_CMD	stDispCmd;
		PR_DISP_BIN_IMG_RPY stDispRpy;

		PR_UWORD		uwRecordID = GetDiePrID(0);
		PR_OPTIC		stOptic;
		PR_COMMON_RPY	stRpy;
		PR_LoadOptic(uwRecordID,	ubPpsI,	// general purpose or learn purpose
					 ubSID, ubRID,
					 &stOptic,				&stRpy);

		PR_InitDispBinImgCmd(&stDispCmd);
		stDispCmd.emGrab			= PR_TRUE;
		stDispCmd.emCameraNo		= GetRunCamID();
		stDispCmd.ubThresholdValue	= (PR_UBYTE) m_lBadcutBiThreshold;
		stDispCmd.emPurpose			= ubPpsI;

		PR_DispBinImgCmd(&stDispCmd, ubSID, ubRID, &stDispRpy);
	}

	BOOL bTemp = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bTemp);

	return 1;
}

LONG CWaferPr::BadCutSetupUsingDieSize(IPC_CServiceMessage &svMsg)
{
	if (m_bUseMouse == FALSE)
	{
		ChangeCamera(WPR_CAM_WAFER);

		if (GetDieShape() == WPR_RECTANGLE_DIE)
		{			
			m_nPolygonSize = 2;
			m_stNewBadCutPoint[0].x = (PR_WORD)(GetPrCenterX() - (GetNmlSizePixelX() / 2) - ((m_dSetRegionByDieSizeX / 100) * GetNmlSizePixelX()));
			m_stNewBadCutPoint[0].y = (PR_WORD)(GetPrCenterY() - (GetNmlSizePixelY() / 2) - ((m_dSetRegionByDieSizeY / 100) * GetNmlSizePixelY()));
			m_stNewBadCutPoint[1].x = (PR_WORD)(GetPrCenterX() + (GetNmlSizePixelX() / 2) + ((m_dSetRegionByDieSizeX / 100) * GetNmlSizePixelX()));
			m_stNewBadCutPoint[1].y = (PR_WORD)(GetPrCenterY() + (GetNmlSizePixelY() / 2) + ((m_dSetRegionByDieSizeY / 100) * GetNmlSizePixelY()));
			DrawRectangleBox(m_stNewBadCutPoint[0], m_stNewBadCutPoint[1], PR_COLOR_BLUE);
		}
		else if (GetDieShape() == WPR_TRIANGULAR_DIE)	//v4.06
		{
			PR_COORD Center;
			Center.x = (PR_WORD)GetPrCenterX();
			Center.y = (PR_WORD)GetPrCenterY();
			m_nPolygonSize = 3;
			DOUBLE dLength = 0;
			DOUBLE dAngle = 0;
			DOUBLE nXYRatio = 1.0 + sqrt(pow(m_dSetRegionByDieSizeX / 100.0, 2.0) + pow(m_dSetRegionByDieSizeY / 100.0, 2.0));
			
			for (int i = 0; i < m_nPolygonSize; i++)
			{
				dLength = FindLength(m_stDieCoordinate[i], Center) * nXYRatio;
				dAngle = FindAngle(m_stDieCoordinate[i], Center);

				if (dAngle <= -PI / 2)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() - abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() - abs((INT)(dLength * sin(dAngle)))) ;
				}

				else if (dAngle <= 0)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() + abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() - abs((INT)(dLength * sin(dAngle)))) ;
				}
				else if (dAngle <= PI / 2)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() + abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() + abs((INT)(dLength * sin(dAngle)))) ;
				}
				else if (dAngle <= PI)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() - abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() + abs((INT)(dLength * sin(dAngle)))) ;
				}
			}
			
			DrawAndErasePolygon(m_stNewBadCutPoint, m_nPolygonSize, PR_COLOR_BLUE);
		}
		else if (GetDieShape() == WPR_RHOMBUS_DIE)
		{
			PR_COORD Center;
			Center.x = (PR_WORD)GetPrCenterX();
			Center.y = (PR_WORD)GetPrCenterY();
			m_nPolygonSize = 4;
			DOUBLE dLength = 0;
			DOUBLE dAngle = 0;
			DOUBLE nXYRatio = 1.0 + sqrt(pow(m_dSetRegionByDieSizeX / 100.0, 2.0) + pow(m_dSetRegionByDieSizeY / 100.0, 2.0));
			
			for (int i = 0; i < m_nPolygonSize; i++)
			{
				dLength = FindLength(m_stDieCoordinate[i], Center) * nXYRatio;
				dAngle = FindAngle(m_stDieCoordinate[i], Center);

				if (dAngle <= -PI / 2)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() - abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() - abs((INT)(dLength * sin(dAngle)))) ;
				}

				else if (dAngle <= 0)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() + abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() - abs((INT)(dLength * sin(dAngle)))) ;
				}
				else if (dAngle <= PI / 2)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() + abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() + abs((INT)(dLength * sin(dAngle)))) ;
				}
				else if (dAngle <= PI)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() - abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() + abs((INT)(dLength * sin(dAngle)))) ;
				}
			}
			
			DrawAndErasePolygon(m_stNewBadCutPoint, m_nPolygonSize, PR_COLOR_BLUE);
		}	//	rhombus die
		else
		{
			PR_COORD Center;
			Center.x = (PR_WORD)GetPrCenterX();
			Center.y = (PR_WORD)GetPrCenterY();
			m_nPolygonSize = 6;
			DOUBLE dLength = 0.0;
			DOUBLE dAngle = 0.0;
			DOUBLE nXYRatio = 1.0 + sqrt(pow(m_dSetRegionByDieSizeX / 100.0, 2.0) + pow(m_dSetRegionByDieSizeY / 100.0, 2.0));
			
			for (int i = 0; i < m_nPolygonSize; i++)
			{
				dLength = FindLength(m_stDieCoordinate[i], Center) * nXYRatio;
				dAngle = FindAngle(m_stDieCoordinate[i], Center);

				if (dAngle <= -PI / 2)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() - abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() - abs((INT)(dLength * sin(dAngle)))) ;
				}

				else if (dAngle <= 0)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() + abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() - abs((INT)(dLength * sin(dAngle)))) ;
				}
				else if (dAngle <= PI / 2)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() + abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() + abs((INT)(dLength * sin(dAngle)))) ;
				}
				else if (dAngle <= PI)
				{
					m_stNewBadCutPoint[i].x = (PR_WORD)(GetPrCenterX() - abs((INT)(dLength * cos(dAngle)))) ;
					m_stNewBadCutPoint[i].y = (PR_WORD)(GetPrCenterY() + abs((INT)(dLength * sin(dAngle)))) ;
				}
			}
			
			DrawAndErasePolygon(m_stNewBadCutPoint, m_nPolygonSize, PR_COLOR_BLUE);
		}
	}
	else
	{
		m_dSetRegionByDieSizeX = m_dSetRegionByDieSizeXCopy ;
		m_dSetRegionByDieSizeY = m_dSetRegionByDieSizeYCopy;
	}
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ClearDisplay(IPC_CServiceMessage &svMsg)
{
	if (m_bUseMouse == FALSE)
	{
		ChangeCamera(WPR_CAM_WAFER);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// Function to power on/off the zoom motor
LONG CWaferPr::PowerOnZoom(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);
	SetZoomZPower(bOn);
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

// Function to power on/off the focus motor
LONG CWaferPr::PowerOnFocus(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);
	SetFocusZPower(bOn);
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

// Fucntion for Zoom motor to move back to home pos
LONG CWaferPr::MoveZoomToHome(IPC_CServiceMessage &svMsg)
{
	//Note: this cmd only for HiPEC, not NuMotion

	if( m_bUseMotorizedZoom == FALSE )
		return 1;
	SetZoomZPower(TRUE);

	// Move away from home sensor before searching home
	BOOL bResult = CMS896AStn::MotionIsHomeSensorHigh("WaferPrZoomZAxis", &m_stZoomAxis_Z);
	if (bResult == TRUE)	//Klocwork	//v4.05
	{
		Z_Zoom_Move(10000);
	}

	BOOL bReturn = Z_HomeZoom();
	SetZoomZPower(FALSE);
	return 1;
}

// Function for Focus to move back to home pos
LONG CWaferPr::MoveFocusToHome(IPC_CServiceMessage &svMsg)
{
	//Note: this cmd only for HiPEC, not NuMotion

	SetFocusZPower(TRUE);

	// Move away from home sensor before searching home
	BOOL bResult = CMS896AStn::MotionIsHomeSensorHigh("WaferPrFocusZAxis", &m_stZoomAxis_Z);
	if (bResult == TRUE)	//Klocwork	//v4.05
	{
		Z_Focus_Move(-2000);
	}

	BOOL bReturn = Z_HomeFocus();
	SetFocusZPower(FALSE);
	return 1;
}

LONG CWaferPr::HomeAOIZoomZ(IPC_CServiceMessage &svMsg)
{
	if (IsWprWithAF())
	{
		m_bAutoFocusDown = FALSE;
		if (m_bZoomHome_Z)
		{
			Z_MoveTo(10000, SFM_WAIT);
		}

		Z_Home();
		m_lFocusZState = 0;
	}

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferPr::PowerOnAOIZoomZ(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);
	Z_PowerOn(bOn);
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferPr::MoveAOIZoomZ(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	typedef struct 
	{
		BOOL bForward;
		LONG lDist;
	} ST_MOVE;
	ST_MOVE stInfo;

	svMsg.GetMsg(sizeof(ST_MOVE), &stInfo);

	if (IsWprWithAF())
	{
		LONG lDist = stInfo.lDist;
		BOOL bForward = stInfo.bForward;

		if (bForward)
		{
			Z_Move(lDist, SFM_WAIT);
		}
		else
		{
			Z_Move(-1 * lDist, SFM_WAIT);
		}
		Sleep(100);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

// Move Zoom motor for a specific steps
LONG CWaferPr::MoveZoom(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		BOOL bForward;
		LONG lDist;
	} ST_MOVE;

	ST_MOVE stInfo;
	svMsg.GetMsg(sizeof(ST_MOVE), &stInfo);
	LONG lDist = stInfo.lDist;
	BOOL bForward = stInfo.bForward;

	//Move ZOOM Z motor
	SetZoomZPower(TRUE);
	Sleep(100);
	if (bForward)
	{
		Z_Zoom_Move(lDist);
	}
	else
	{
		Z_Zoom_Move(-1 * lDist);
	}
	Sleep(100);
	SetZoomZPower(FALSE);

	SavePrData(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

// Move Focus motor for a specific steps
LONG CWaferPr::MoveFocus(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	typedef struct 
	{
		BOOL bForward;
		LONG lDist;
	} ST_MOVE;

	ST_MOVE stInfo;
	svMsg.GetMsg(sizeof(ST_MOVE), &stInfo);
	LONG lDist = stInfo.lDist;
	BOOL bForward = stInfo.bForward;

	/*
	CString szTemp;
	szTemp.Format("Move Focus Z %d   %d", bForward, lDist);
	AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	*/

	//Move FOCUS-Z motor
	SetFocusZPower(TRUE);

	Sleep(100);
	if (bForward)
	{
		Z_Focus_Move(lDist);
	}
	else
	{
		Z_Focus_Move(-1 * lDist);
	}
	Sleep(100);

	SetFocusZPower(FALSE);
	
	SavePrData(TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

// Backlight Motor function

LONG CWaferPr::HomeBackLightElevator(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	
	BLZ_Home();	

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferPr::PowerOnBackLightElevator(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);
	
	BLZ_PowerOn(bOn);

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferPr::MoveBackLightElevator(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	typedef struct 
	{
		BOOL bForward;
		LONG lDist;
	} ST_MOVE;

	ST_MOVE stInfo;
	svMsg.GetMsg(sizeof(ST_MOVE), &stInfo);
	LONG lDist = stInfo.lDist;
	BOOL bForward = stInfo.bForward;

	if (bForward)
	{
		BLZ_Move(lDist);
	}
	else
	{
		BLZ_Move(-1 * lDist);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}


LONG CWaferPr::ChangeBackLightElevatorPos(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;

	typedef struct 
	{
		LONG	lPosition;
		BOOL	bFromZero;	
	} BLSETUP;
	BLSETUP stInfo;
	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	if (stInfo.bFromZero == TRUE)
	{
		m_lWPRGeneral_TMP = 0;

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lWPRGeneral_2 = 0;	
				break;	//standy
			default:	
				m_lWPRGeneral_1 = 0;	
				break;	//up level
		}
	}

	switch (stInfo.lPosition)
	{
		case 1: // Standy
			if (stInfo.bFromZero == FALSE)
			{
				BLZ_MoveTo(m_lBackLightElevatorStandByLevel);
				m_lWPRGeneral_TMP = m_lBackLightElevatorStandByLevel;
			}
			break;
		default: // uplevel
			if (stInfo.bFromZero == FALSE)
			{
				BLZ_MoveTo(m_lBackLightElevatorUpLevel);
				m_lWPRGeneral_TMP = m_lBackLightElevatorUpLevel;
			}
			break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferPr::ConfirmBackLightElevatorPos(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();

	// home add later
	BLZ_MoveTo(0);
	m_lBackLightElevatorUpLevel = m_lWPRGeneral_1;
	m_lBackLightElevatorStandByLevel = m_lWPRGeneral_2;

	CloseLoadingAlert();

	SetStatusMessage("Back Light Elevator level is updated");

	SavePrData();
	return 1;
}

LONG CWaferPr::CancelBackLightPos(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();

	// home add later
	BLZ_MoveTo(0);
	m_lWPRGeneral_1 = m_lBackLightElevatorUpLevel;
	m_lWPRGeneral_2 = m_lBackLightElevatorStandByLevel;

	CloseLoadingAlert();
	return TRUE;
}

LONG CWaferPr::KeyInElevatorPos(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;
	BLSETUP stInfo;
	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	switch (stInfo.lPosition)
	{
		case 1:		//stand by
			m_lWPRGeneral_2 = stInfo.lStep;
			BLZ_MoveTo(m_lWPRGeneral_2);
			break;

		default:	//up level
			m_lWPRGeneral_1 = stInfo.lStep;
			BLZ_MoveTo(m_lWPRGeneral_1);
			break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferPr::MoveElevatorPositivePos(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	LONG lCurrentPos = 0;
	LONG lMinPos, lMaxPos;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;
	BLSETUP stInfo;
	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lWPRGeneral_2 + stInfo.lStep;	
			break;	//stand by
		default:	
			lCurrentPos = m_lWPRGeneral_1 + stInfo.lStep;
			break;	//up level
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BACKLIGHT_Z, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BACKLIGHT_Z, MS896A_CFG_CH_MAX_DISTANCE);								
	
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		BLZ_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lWPRGeneral_2 = lCurrentPos;	
				break;
			default:	
				m_lWPRGeneral_1 = lCurrentPos;	
				break;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferPr::MoveElevatorNegativePos(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	LONG lCurrentPos = 0;
	LONG lMinPos, lMaxPos;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BLSETUP;
	BLSETUP stInfo;
	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lWPRGeneral_2 - stInfo.lStep;	
			break;	//stand by
		default:	
			lCurrentPos = m_lWPRGeneral_1 - stInfo.lStep;
			break;	//up level
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BACKLIGHT_Z, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_BACKLIGHT_Z, MS896A_CFG_CH_MAX_DISTANCE);								
	
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		BLZ_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lWPRGeneral_2 = lCurrentPos;	
				break;
			default:	
				m_lWPRGeneral_1 = lCurrentPos;	
				break;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}


//================================================================
//   Created-By  : Andrew Ng
//   Date        : 10/23/2007 1:49:46 PM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferPr::InitIMPopupPage(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	//IM not yet support ref die
	if (m_bSelectDieType == WPR_REFERENCE_DIE)	
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//================================================================
// InitIMCriteriaPage()
//   Created-By  : Andrew Ng
//   Date        : 10/23/2007 2:04:09 PM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferPr::InitIMCriteriaPage(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	//IM not yet support ref die
	if (m_bSelectDieType == WPR_REFERENCE_DIE) 
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//================================================================
// IMUserSearchDie()
//   Created-By  : Andrew Ng
//   Date        : 10/23/2007 2:23:58 PM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferPr::IMUserSearchDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	//IM not yet support ref die
	if (m_bSelectDieType == WPR_REFERENCE_DIE) 
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//================================================================
//   Created-By  : Andrew Ng
//   Date        : 10/23/2007 5:46:12 PM
//   Description : 
//   Remarks     : 
//================================================================
// old function used by wafer table for adapt wafer
LONG CWaferPr::InitHighLevelVisionInfo(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// function for HMI
LONG CWaferPr::IM_InitPrHighLevelInfoCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::IMAutoLearnWafer(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::IMChangeWafer(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::IM_StartWafer(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ClearBadCutPoints(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (GetDieShape() == WPR_RECTANGLE_DIE)
	{
		m_stBadCutPoint[0].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[0].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
		m_stBadCutPoint[1].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[1].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
	}
	else if (GetDieShape() == WPR_TRIANGULAR_DIE)	//v4.06
	{
		m_stBadCutPoint[0].x = (PR_WORD)GetPrCenterX();
		m_stBadCutPoint[0].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
		m_stBadCutPoint[1].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[1].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
		m_stBadCutPoint[2].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[2].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
	}
	else if (GetDieShape() == WPR_RHOMBUS_DIE)
	{
		m_stBadCutPoint[0].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[0].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
		m_stBadCutPoint[1].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[1].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
		m_stBadCutPoint[2].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[2].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
		m_stBadCutPoint[3].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[3].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
	}	//	rhombus die
	else
	{
		m_stBadCutPoint[0].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[0].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
		m_stBadCutPoint[1].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[1].y = (PR_WORD)GetPrCenterY();
		m_stBadCutPoint[2].x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[2].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
		m_stBadCutPoint[3].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[3].y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);
		m_stBadCutPoint[4].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[4].y = (PR_WORD)GetPrCenterY();
		m_stBadCutPoint[5].x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
		m_stBadCutPoint[5].y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

// this function is for black and white PR
LONG CWaferPr::AdaptWaferInitHighLevelVisionInfo(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	LONG lWaferCenterX	= GetWaferCenterX();
	LONG lWaferCEnterY	= GetWaferCenterY();
	LONG lRadius	= GetWaferDiameter() / 2;

	PR_ADAPT_WAFER_CMD	stCmd;
	PR_ADAPT_WAFER_RPY	stRpy;

	PR_InitAdaptWaferCmd(&stCmd);
	stCmd.uwRecordID		= GetDiePrID(0);
	stCmd.rcoWaferCentre.x	= (IMG_REAL) lWaferCenterX;	
	stCmd.rcoWaferCentre.y	= (IMG_REAL) lWaferCEnterY;	
	stCmd.rWaferRadius		= (IMG_REAL) lRadius;
	stCmd.rGoodDiePercent	= 90;
	stCmd.emIsAutoLight		= PR_TRUE;					// enable autolighting (Need to move to first good die position )

	PR_AdaptWaferCmd(&stCmd, GetRunSenID(), GetRunRecID(), &stRpy);
	
	if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy.stStatus.uwPRStatus))
	{
		CString szErr;
		szErr.Format("PR_AdaptWaferCmd fail; comm = 0x%x, pr = 0x%x", stRpy.stStatus.uwCommunStatus, stRpy.stStatus.uwPRStatus);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SetMouseControlCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	if (GetLockPrMouseJoystick() == TRUE)
	{
		bEnable = FALSE;
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return 1; 
	}

	m_bJoystickOn	= bEnable;
	m_bXJoystickOn	= bEnable;
	m_bYJoystickOn	= bEnable;

	m_lJsTableMode = (*m_psmfSRam)["MS896A"]["Current Camera"];

	EnableMouseClickCallbackCmd(bEnable);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::CopyRefDieSettings(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	for (INT i = WPR_GEN_RDIE_OFFSET + 2; i < WPR_MAX_DIE; i++)
	{
		m_lGenSrchDefectThres[i]		= m_lGenSrchDefectThres[WPR_GEN_RDIE_OFFSET + 1];
		m_lGenSrchDieAreaX[i]			= m_lGenSrchDieAreaX[WPR_GEN_RDIE_OFFSET + 1];
		m_lGenSrchDieAreaY[i]			= m_lGenSrchDieAreaY[WPR_GEN_RDIE_OFFSET + 1];
		m_lGenSrchDieScore[i]			= m_lGenSrchDieScore[WPR_GEN_RDIE_OFFSET + 1];
		m_lGenSrchGreyLevelDefect[i]	= m_lGenSrchGreyLevelDefect[WPR_GEN_RDIE_OFFSET + 1];
		m_dGenSrchSingleDefectArea[i]	= m_dGenSrchSingleDefectArea[WPR_GEN_RDIE_OFFSET + 1];
		m_dGenSrchTotalDefectArea[i]	= m_dGenSrchTotalDefectArea[WPR_GEN_RDIE_OFFSET + 1];
		m_dGenSrchChipArea[i]			= m_dGenSrchChipArea[WPR_GEN_RDIE_OFFSET + 1];
		m_bGenSrchEnableDefectCheck[i]	= m_bGenSrchEnableDefectCheck[WPR_GEN_RDIE_OFFSET + 1];
		m_bGenSrchEnableChipCheck[i]	= m_bGenSrchEnableChipCheck[WPR_GEN_RDIE_OFFSET + 1];
	}	

	SavePrData(FALSE);

	HmiMessage("Copy Completed");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::DrawCORCirclePattern(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bDraw = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bDraw);

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	if (bDraw)
	{
		PR_DRAW_CIRCLE_CMD	stCmd1;
		PR_DRAW_CIRCLE_CMD	stCmd2;
		PR_DRAW_CIRCLE_CMD	stCmd3;
		PR_DRAW_CROSS_CMD	stCrossCmd;
		PR_DRAW_CIRCLE_RPY	stRpy;
		PR_DRAW_CROSS_RPY	stCrosssRpy;

		stCmd1.stCentre.x	= (PR_WORD)GetPrCenterX();
		stCmd1.stCentre.y	= (PR_WORD)GetPrCenterY();
		stCmd1.leRadius		= 1000;
		stCmd2.stCentre.x	= (PR_WORD)GetPrCenterX();
		stCmd2.stCentre.y	= (PR_WORD)GetPrCenterY();
		stCmd2.leRadius		= 2000;
		stCmd3.stCentre.x	= (PR_WORD)GetPrCenterX();
		stCmd3.stCentre.y	= (PR_WORD)GetPrCenterY();
		stCmd3.leRadius		= 3000;

		stCrossCmd.coCrossCentre.x = (PR_WORD)GetPrCenterX();
		stCrossCmd.coCrossCentre.y = (PR_WORD)GetPrCenterY();
		stCrossCmd.emColor = PR_COLOR_RED;
		stCrossCmd.szCrossSize.x = 100;
		stCrossCmd.szCrossSize.y = 100;

		PR_DrawCircleCmd(&stCmd2, ubSID, ubRID, &stRpy);
		PR_DrawCrossCmd(&stCrossCmd, ubSID, ubRID, &stCrosssRpy);
	}
	else
	{
		ChangeCamera(WPR_CAM_WAFER);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
/// <summary>
/// 
/// </summary>
/// <param name="svMsg">		</param>
/// <returns>
/// 
/// </returns>
LONG CWaferPr::Search2DBarCodeFromWafer(IPC_CServiceMessage &svMsg)
{
	BOOL bManual = FALSE;
	//svMsg.GetMsg(sizeof(BOOL), &bManual);
	BOOL bMirrorCode = FALSE;	//FALSE;
	CString szCode = "";

	BOOL bStatus = Search2DBarCodePatternFromWafer(szCode, FALSE, bMirrorCode);	
	if (!bStatus)	//v4.46T4
	{
		Sleep(500);
		bStatus = Search2DBarCodePatternFromWafer (szCode, TRUE, bMirrorCode);	
	}

	CString szMsg;
	szMsg.Format("WPR: Search2DBarCodeFromWafer - 2D read = %s; status = %d", szCode, bStatus);
	//HmiMessage(szMsg);
	//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);		//v4.50A3	

	(*m_psmfSRam)["WaferPr"]["2DBarCode"] = szCode;

	svMsg.InitMessage(sizeof(BOOL), &bStatus);
	return 1;
}


LONG CWaferPr::Search2DBarCode(IPC_CServiceMessage &svMsg)
{
	BOOL bManual = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bManual);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->GetFeatureStatus(MS896A_FUNC_VISION_POSTSEAL_OPTICS))
	{
		BOOL bStatus = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bStatus);
		return 1;
	}

	//First Turn on PR lighting  
	GetPostSealLighting(m_unCurrPostSealID);
	Sleep(200);

	CString szCode = _T("");
	BOOL bMirrorCode = TRUE;
	if (pApp->GetCustomerName() == "Knowles")	//v4.46T4
		bMirrorCode = FALSE;
	BOOL bStatus = Search2DBarCodePattern(szCode, m_unCurrPostSealID, FALSE, bMirrorCode);	

	if (!bStatus)	//v4.46T4
	{
		Sleep(500);
		bStatus = Search2DBarCodePattern(szCode, m_unCurrPostSealID, FALSE, bMirrorCode);	
	}

	if (bStatus)
	{
		if (bManual)
		{
			HmiMessage("2D barcode = " + szCode);
		}
		else
		{
			if (pApp->GetCustomerName() == "Lumileds")
			{
				(*m_psmfSRam)["PSPR"]["2DBarCode"] = "_" + szCode;
			}
			else
			{
				(*m_psmfSRam)["PSPR"]["2DBarCode"] = szCode;	//v4.46T11
			}
		}
	}
	else
	{
		if (bManual)
		{
			HmiMessage("2D barcode failure!");
		}
		else
		{
			(*m_psmfSRam)["PSPR"]["2DBarCode"] = _T("");
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bStatus);
	return 1;
}

LONG CWaferPr::RemoveShowImages(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	RemoveFilesInFolder(WPR_SHOW_IMAGE_PATH);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::PreOnSelectShowPrImage(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSelectDieType = 0;
	LONG lDieNo = 0;
	CString szPath = "";

	m_szShowImageType	= WPR_HMI_NORM_DIE;
	m_lShowImageNo		= 1;

	if (m_szShowImageType == WPR_HMI_NORM_DIE)
	{
		bSelectDieType = WPR_NORMAL_DIE;
	}
	else
	{
		bSelectDieType = WPR_REFERENCE_DIE;
	}

	if (ShowPrImage(szPath, lDieNo, bSelectDieType, m_lShowImageNo) == FALSE)
	{
		m_szGenShowImagePath = "";
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_szGenShowImagePath = szPath;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::OnSelectShowPrImage(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSelectDieType = 0;
	CString szPath;
	LONG lDieNo = 0;

	if (m_szShowImageType == WPR_HMI_NORM_DIE)
	{
		bSelectDieType = WPR_NORMAL_DIE;
	}
	else
	{
		bSelectDieType = WPR_REFERENCE_DIE;
	}
	
	if (ShowPrImage(szPath, lDieNo, bSelectDieType, m_lShowImageNo) == FALSE)
	{
		m_szGenShowImagePath = "";
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_szGenShowImagePath = szPath;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}

LONG CWaferPr::DisplayPRRecordInSummary(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lDieNo = 0;
	CString szPath;
	
	// Display Normal Die Record
	if (ShowPrImage(szPath, lDieNo, WPR_NORMAL_DIE, 1) == TRUE)
	{
		m_szShowImagePath[lDieNo] = szPath;
	}

	// Display Ref 1
	if (ShowPrImage(szPath, lDieNo, WPR_REFERENCE_DIE, 1) == TRUE)
	{
		m_szShowImagePath[lDieNo] = szPath;
	}

	// Display Ref 2
	if (ShowPrImage(szPath, lDieNo, WPR_REFERENCE_DIE, 2) == TRUE)
	{
		m_szShowImagePath[lDieNo] = szPath;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SearchAndSaveNormalDieImages(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	GrabAndSaveImage(WPR_NORMAL_DIE, WPR_NORMAL_DIE, WPR_GRAB_SAVE_IMG_UFO);
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SearchAndSaveReferDieImages(IPC_CServiceMessage &svMsg)
{
	CMSLogFileUtility::Instance()->WL_LogStatus("WPR grab save refer die image");
	Sleep(100);
	LONG lRefDieNo = 1;
	if(m_bAllRefDieCheck)
		lRefDieNo = 0;	// 0 is to search all refer die
	GrabAndSaveImage(WPR_REFERENCE_DIE, lRefDieNo, WPR_GRAB_SAVE_IMG_FHD);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}
LONG CWaferPr::SearchNormalDieInWafer(IPC_CServiceMessage &svMsg)
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX(), GetPRWinLRY()};

	BOOL	bPrType;
	LONG	lPrId;
	LONG	lSrchNmlMsg[2];

	typedef struct
	{
		LONG lSrchMode;
		LONG lX;
		LONG lY;
		LONG lDigit1;
		LONG lDigit2;
		LONG lResult;
	} WTPos;
	WTPos stPRS;

	int		iOffsetX, iOffsetY;

	if (PR_NotInit())
	{
		stPRS.lSrchMode = 0;
		//stPRS.bRtn = FALSE;
		stPRS.lX = 0;
		stPRS.lY = 0;
		stPRS.lDigit1 = 0;
		stPRS.lDigit2 = 0;
		stPRS.lResult = 0;
		svMsg.InitMessage(sizeof(WTPos), &stPRS);
		return 1;
	}
	
	// Get Search Message
	svMsg.GetMsg(2 * sizeof(LONG), lSrchNmlMsg);

	bPrType = WPR_NORMAL_DIE;
	lPrId = lSrchNmlMsg[1];

	PR_WORD wResult = 0;

	switch (lSrchNmlMsg[0])
	{
		case 0:
			wResult = ManualSearchDie(bPrType, lPrId, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
			if (wResult != -1)
			{
				if (DieIsAlignable(usDieType) == TRUE)
				{
					//Allow die to rotate
					//ManualDieCompenate(stDieOffset, fDieRotate);
					CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);

					stPRS.lSrchMode = 0;
					//stPRS.bRtn = TRUE;
					stPRS.lX = iOffsetX;
					stPRS.lY = iOffsetY;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 1;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
				else
				{
					stPRS.lSrchMode = 0;
					//stPRS.bRtn = FALSE;
					stPRS.lX = 0;
					stPRS.lY = 0;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 0;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
			}
			break;

		case 1:
			//Set & Display Max search die area
			wResult = ManualSearchDie(bPrType, lPrId, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
			if (wResult != -1)
			{
				if (DieIsAlignable(usDieType) == TRUE)
				{
					CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);
					stPRS.lSrchMode = 0;
					//stPRS.bRtn = TRUE;
					stPRS.lX = (LONG)iOffsetX;
					stPRS.lY = (LONG)iOffsetY;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 1;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
				else
				{
					stPRS.lSrchMode = 0;
					//stPRS.bRtn = FALSE;
					stPRS.lX = 0;
					stPRS.lY = 0;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 0;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
			}
			else
			{
				stPRS.lSrchMode = 0;
				//stPRS.bRtn = FALSE;
				stPRS.lX = 0;
				stPRS.lY = 0;
				stPRS.lDigit1 = 0;
				stPRS.lDigit2 = 0;
				stPRS.lResult = 0;
				svMsg.InitMessage(sizeof(WTPos), &stPRS);
			}

			if (m_bAllRefDieCheck && stPRS.lResult == 0)
			{
				LONG i;
				PR_WORD wRtn = 0;

				for (i = 2; i <= m_lLrnTotalRefDie; i++)
				{
					//v4.43T11
					if (m_bEnableMS100EjtXY && (i == 2))	// Ej Cap pattern
					{
						continue;
					}
					if (m_bEnableMS100EjtXY && (i == 3))	// Collet hole
					{
						continue;
					}

					wRtn = ManualSearchDie(bPrType, i, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
					if (wRtn != -1)
					{
						if (DieIsAlignable(usDieType) == TRUE)
						{
							CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);
							stPRS.lSrchMode = 0;
							//stPRS.bRtn = TRUE;
							stPRS.lX = (LONG)iOffsetX;
							stPRS.lY = (LONG)iOffsetY;
							stPRS.lDigit1 = 0;
							stPRS.lDigit2 = 0;
							stPRS.lResult = 1;
							svMsg.InitMessage(sizeof(WTPos), &stPRS);
							break;
						}
					}
				}
			}

			break;
		default:
			;
	}

	return 1;
}

LONG CWaferPr::SearchReferDieInWafer(IPC_CServiceMessage &svMsg)
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	
	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX(), GetPRWinLRY()};

	BOOL	bPrType;
	LONG	lPrId;
	LONG	lSrchRefMsg[2];

	typedef struct
	{
		LONG lSrchMode;
		LONG lX;
		LONG lY;
		LONG lDigit1;
		LONG lDigit2;
		LONG lResult;
	} WTPos;
	WTPos stPRS;

	int		iOffsetX, iOffsetY;

	if (PR_NotInit())
	{
		stPRS.lSrchMode = 0;
		//stPRS.bRtn = FALSE;
		stPRS.lX = 0;
		stPRS.lY = 0;
		stPRS.lDigit1 = 0;
		stPRS.lDigit2 = 0;
		stPRS.lResult = 0;
		svMsg.InitMessage(sizeof(WTPos), &stPRS);
		return 1;
	}
	
	// Get Search Message
	svMsg.GetMsg(2 * sizeof(LONG), lSrchRefMsg);

	bPrType = WPR_REFERENCE_DIE;
	lPrId = lSrchRefMsg[1];

	PR_WORD wResult = 0;

	switch (lSrchRefMsg[0])
	{
		case 0:
			wResult = ManualSearchDie(bPrType, lPrId, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
			if (wResult != -1)
			{
				if (DieIsAlignable(usDieType) == TRUE)
				{
					//Allow die to rotate
					//ManualDieCompenate(stDieOffset, fDieRotate);
					CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);

					stPRS.lSrchMode = 0;
					//stPRS.bRtn = TRUE;
					stPRS.lX = iOffsetX;
					stPRS.lY = iOffsetY;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 1;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
				else
				{
					stPRS.lSrchMode = 0;
					//stPRS.bRtn = FALSE;
					stPRS.lX = 0;
					stPRS.lY = 0;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 0;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
			}
			break;

		case 1:
			//Set & Display Max search die area
			wResult = ManualSearchDie(bPrType, lPrId, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
			if (wResult != -1)
			{
				if (DieIsAlignable(usDieType) == TRUE)
				{
					CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);
					stPRS.lSrchMode = 0;
					//stPRS.bRtn = TRUE;
					stPRS.lX = (LONG)iOffsetX;
					stPRS.lY = (LONG)iOffsetY;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 1;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
				else
				{
					stPRS.lSrchMode = 0;
					//stPRS.bRtn = FALSE;
					stPRS.lX = 0;
					stPRS.lY = 0;
					stPRS.lDigit1 = 0;
					stPRS.lDigit2 = 0;
					stPRS.lResult = 0;
					svMsg.InitMessage(sizeof(WTPos), &stPRS);
				}
			}
			else
			{
				stPRS.lSrchMode = 0;
				//stPRS.bRtn = FALSE;
				stPRS.lX = 0;
				stPRS.lY = 0;
				stPRS.lDigit1 = 0;
				stPRS.lDigit2 = 0;
				stPRS.lResult = 0;
				svMsg.InitMessage(sizeof(WTPos), &stPRS);
			}

			if (m_bAllRefDieCheck && stPRS.lResult == 0)
			{
				LONG i;
				PR_WORD wRtn = 0;

				for (i = 2; i <= m_lLrnTotalRefDie; i++)
				{
					//v4.43T11
					if (m_bEnableMS100EjtXY && (i == 2))	// Ej Cap pattern
					{
						continue;
					}
					if (m_bEnableMS100EjtXY && (i == 3))	// Collet hole
					{
						continue;
					}

					wRtn = ManualSearchDie(bPrType, i, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
					if (wRtn != -1)
					{
						if (DieIsAlignable(usDieType) == TRUE)
						{
							CalculateDieCompenate(stDieOffset, &iOffsetX, &iOffsetY);
							stPRS.lSrchMode = 0;
							//stPRS.bRtn = TRUE;
							stPRS.lX = (LONG)iOffsetX;
							stPRS.lY = (LONG)iOffsetY;
							stPRS.lDigit1 = 0;
							stPRS.lDigit2 = 0;
							stPRS.lResult = 1;
							svMsg.InitMessage(sizeof(WTPos), &stPRS);
							break;
						}
					}
				}
			}

			break;
		default:
			;
	}

	return 1;
}

LONG CWaferPr::SetupPrCrossHair(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	PR_DRAW_CROSS_CMD	stCrossCmd;
	PR_DRAW_CROSS_RPY	stCrosssRpy;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	m_pPrGeneral->PRClearScreenNoCursor(ubSID, ubRID);

	m_lHmiPrCenterX = PR_DEF_CENTRE_X;
	m_lHmiPrCenterY = PR_DEF_CENTRE_Y;
	
	stCrossCmd.coCrossCentre.x = (PR_WORD)m_lHmiPrCenterX;
	stCrossCmd.coCrossCentre.y = (PR_WORD)m_lHmiPrCenterY;
	stCrossCmd.emColor = PR_COLOR_RED;

	stCrossCmd.szCrossSize.x = 1000;
	stCrossCmd.szCrossSize.y = 1000;

	PR_DrawCrossCmd(&stCrossCmd, ubSID, ubRID, &stCrosssRpy);
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::AdjustCrossHairStepByStep(IPC_CServiceMessage &svMsg)
{
	PR_COORD			sttmpStartPos;
	PR_WORD				sstmpPixelStep;
	LONG				lPixel;
	
	typedef struct 
	{
		unsigned char ucDirection;
		unsigned char ucPixelStep;
	} MOVECURSOR;

	MOVECURSOR	stInfo;
	svMsg.GetMsg(sizeof(MOVECURSOR), &stInfo);

	//Get Pixel step move
	switch (stInfo.ucPixelStep)
	{
		case 1:		//10 pixel
			sstmpPixelStep = (PR_WORD)(10 * GetPrScaleFactor());
			lPixel = 10;
			break;

		case 2:		//30 pixel
			sstmpPixelStep = (PR_WORD)(30 * GetPrScaleFactor());
			lPixel = 30;
			break;
		
		default:	//1 pixel
			sstmpPixelStep = (PR_WORD)(1 * GetPrScaleFactor());
			lPixel = 1;
			break;
	}


	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);

	sttmpStartPos.x = (PR_WORD)GetPrCenterX();
	sttmpStartPos.y = (PR_WORD)GetPrCenterY();

	switch (stInfo.ucDirection)
	{
		case 1:
			sttmpStartPos.x -= sstmpPixelStep;
			break;

		case 2:
			sttmpStartPos.y += sstmpPixelStep;
			break;

		case 3:
			sttmpStartPos.x += sstmpPixelStep;
			break;

		default:
			sttmpStartPos.y -= sstmpPixelStep;
			break;
	}


	//v4.43T9
	LONG lHalfSrchWndX = (LONG)( (GetSrchDieAreaX()*1.0/4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR );
	LONG lHalfSrchWndY = (LONG)( (GetSrchDieAreaY()*1.0/4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR );
	LONG lSizeX = PR_DEF_CENTRE_X - GetPitchPixelXX() - lHalfSrchWndX;
	LONG lSizeY = PR_DEF_CENTRE_Y - GetPitchPixelYY() - lHalfSrchWndY;
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
		if (dFov > WPR_LF_SIZE_7X7)	
		{
			lSizeX = PR_DEF_CENTRE_X - 3 * GetPitchPixelXX() - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 3 * GetPitchPixelYY() - lHalfSrchWndY;
		} 
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			lSizeX = PR_DEF_CENTRE_X - 2 * GetPitchPixelXX() - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 2 * GetPitchPixelYY() - lHalfSrchWndY;
		}
	}
	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	if ( (GetPitchPixelXX() == 0) || (GetPitchPixelYY() == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}

	//v4.52A6	//Nichia
	if (lSizeX <= 0)
	{
		lSizeX = 1700;
	}
	if (lSizeY <= 0)
	{
		lSizeY = 1700;
	}


	////v4.43T9	//Srch Wnd checking
	//if (sttmpStartPos.x > PR_DEF_CENTRE_X + (PR_WORD)lSizeX)
	//	sttmpStartPos.x = PR_DEF_CENTRE_X + (PR_WORD)lSizeX;
	//if (sttmpStartPos.x < PR_DEF_CENTRE_X - (PR_WORD)lSizeX)
	//	sttmpStartPos.x = PR_DEF_CENTRE_X - (PR_WORD)lSizeX;
	//if (sttmpStartPos.y > PR_DEF_CENTRE_Y + (PR_WORD)lSizeY)
	//	sttmpStartPos.y = PR_DEF_CENTRE_Y + (PR_WORD)lSizeY;
	//if (sttmpStartPos.y < PR_DEF_CENTRE_Y - (PR_WORD)lSizeY)
	//	sttmpStartPos.y = PR_DEF_CENTRE_Y - (PR_WORD)lSizeY;

	m_lPrCenterX		= sttmpStartPos.x;
	m_lPrCenterY		= sttmpStartPos.y;
	m_lPrCenterXInPixel	= (GetPrCenterX() - PR_DEF_CENTRE_X) / GetPrScaleFactor();	//v4.08
	m_lPrCenterYInPixel	= (GetPrCenterY() - PR_DEF_CENTRE_Y) / GetPrScaleFactor();	//v4.08

	CString szMsg;
	szMsg.Format("WPR CrossHair Adjust: (%ld, %ld); X width = %ld, Y Width = %ld; Pitch(%ld(%ld) %ld(%ld)); 1/2SrchWnd(%ld %ld)", lSizeX, lSizeY,
					m_lPrCenterX, m_lPrCenterY, 
					GetPitchPixelXX(), GetPitchPixelXY(), GetPitchPixelYY(), GetPitchPixelYX(), 
					lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	CString szErrMsg;
	m_pPrGeneral->DrawHomeCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, GetPrCenterX(), GetPrCenterY(), szErrMsg);

	SavePrData(TRUE);
	return 1;
}


LONG CWaferPr::AutoLookAroundDie_Cmd(IPC_CServiceMessage &svMsg)
{
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset; 
	PR_REAL				fDieScore;
	PR_COORD			stDieSize;
	PR_BOOLEAN			bInspection;
	PR_BOOLEAN			bPrLatch;
	INT i, j;
	PR_WIN		stLFSearchArea;

	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];

	WT_LA_DIE_RESULT	stLADieState;

	INT lWidth, lHeight;

	lWidth = (LONG) dFOVX;
	lHeight = (LONG) dFOVY;

	if (lWidth / 2 == 0)
	{
		lWidth = lWidth - 1;
	}
	if (lHeight / 2 == 0)
	{
		lHeight = lHeight - 1;
	}

	lWidth = lWidth / 2 - 1;
	lHeight = lHeight / 2 - 1;

	if (lWidth < 1)
	{
		lWidth = 1;
	}
	if (lWidth > WT_LA_CTR_DIE)
	{
		lWidth = WT_LA_CTR_DIE;
	}
	if (lHeight < 1)
	{
		lHeight = 1;
	}
	if (lHeight > WT_LA_CTR_DIE)
	{
		lHeight = WT_LA_CTR_DIE;
	}

	memset(&stLADieState, 0, sizeof(WT_LA_DIE_RESULT));

	(*m_psmfSRam)["WaferPr"]["FOV"]["BaseCol"]	= lWidth;
	(*m_psmfSRam)["WaferPr"]["FOV"]["BaseRow"]	= lHeight;

	for (i = 0; i < (lHeight * 2 + 1); i++)
	{
		for (j = 0; j < (lWidth * 2 + 1); j++)
		{
			stLADieState.m_sDieState[i][j] = 1;
		}
	}

	if (CMS896AApp::m_lVisionPlatform == PR_SYSTEM_AOI)		//v4.05		//Klocwork
	{
		bInspection = PR_TRUE;
	}
	else
	{
		bInspection = PR_FALSE;
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	bPrLatch = PR_TRUE;		//Need to LATCH if current cycle is NO-COMPENSATE
	for (i = 0; i < (lHeight * 2 + 1); i++)
	{
		for (j = 0; j < (lWidth * 2 + 1); j++)
		{
			if( GetLFSearchWindow((i - lHeight), (j - lWidth), FALSE, stLFSearchArea) == FALSE )
			{
				continue;
			}
			usSearchResult = SearchDieCmd(WPR_NORMAL_DIE, WPR_NORMAL_DIE, 
										  ubSID, 
										  ubRID, 
										  stLFSearchArea.coCorner1, 
										  stLFSearchArea.coCorner2, 
										  bPrLatch, 
										  PR_TRUE, 
										  bInspection, 
										  PR_NO_DISPLAY);
			if (usSearchResult != PR_ERR_NOERR)
			{
				break;
			}

			//Get reply 1
			usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
			if (usSearchResult != PR_ERR_NOERR)
			{
				break;
			}

			//No need to LATCH for other LA dice beside die-1
			if (bPrLatch)	
			{
				bPrLatch = PR_FALSE;
			}

			//Get reply 2
			usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
			if (usSearchResult == PR_COMM_ERR)
			{
				break;
			}

			usDieType = stSrchRpy2.stStatus.uwPRStatus;
			if (usDieType == PR_ERR_NOERR)
			{
				usDieType = PR_ERR_GOOD_DIE;
			}

			if ((DieIsAlignable(usDieType)) && bInspection)
			{
				usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
				if (usSearchResult == PR_COMM_ERR)
				{
					break;
				}
			}

			ExtractDieResult(stSrchRpy2, stSrchRpy3, bInspection, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);
			if (DieIsAlignable(usDieType) == TRUE)
			{
				stLADieState.m_sDieState[i][j] = 2;
				INT nX = 0, nY = 0;
				CalculateDieCompenate(stDieOffset, &nX, &nY);
				stLADieState.m_sDiePosnX[i][j] = nX;
				stLADieState.m_sDiePosnY[i][j] = nY;
			}
			//	DieIsGood(usDieType);
		}
	}

	CString szDebug = "Die Result";
	for (i = 0; i < (lHeight * 2 + 1); i++)
	{
		szDebug.Format("Row %d die:", i);
		for (j = 0; j < (lWidth * 2 + 1); j++)
		{
			if (stLADieState.m_sDieState[i][j] == 2)
			{
				szDebug += " OK";
			}
			else
			{
				szDebug += " NO";
			}
		}
		if (IsPrescanEnded())
		{
			CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szDebug);
		}
	}

	svMsg.InitMessage(sizeof(WT_LA_DIE_RESULT), &stLADieState);
	return 1;
}



LONG CWaferPr::AutoFocus(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	CString szMsg;
	CString szDisplay;

	if (IsWprWithAF() == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	BOOL bByHmi = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bByHmi);
	if (bByHmi != TRUE && m_bWaferAutoFocus == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed, can not move focus z");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if( IsBLInUse() )
	{
		BLZ_Sync();
		if( m_lBLZAutoFocusDelay>0 )
		{
			Sleep(m_lBLZAutoFocusDelay);
		}
	}

	Z_Sync();
	WPR_GetEncoderValue();
	szMsg.Format("WPR: do auto focus begin at %d (Up %ld, Low %ld, Min %ld, Tol %ld), delay %ld, BLZ delay %ld!", 
				 m_lEnc_AF, m_lAFLowLimit, m_lAFUpLimit, m_lAFMinDist, m_lAFErrorTol, m_lAFPrDelay, m_lBLZAutoFocusDelay);
	SaveScanTimeEvent(szMsg);

	BOOL bWft = FALSE;
	if (IsESDualWT())
	{
		if (bByHmi != -1 && bByHmi != -2)
		{
			bWft = IsWT2UnderCamera();
		}
		else
		{
			if (bByHmi == -2)
			{
				bWft = TRUE;
			}
		}
	}

	if (m_bAutoFocusOK)
	{
		if (bWft)
		{
			Z_MoveTo(m_lAutoFocusPosition2, SFM_WAIT);
		}
		else
		{
			Z_MoveTo(m_lAutoFocusPosition, SFM_WAIT);
		}
		if (bWft)
		{
			m_lFocusZState	= 22;
		}
		else
		{
			m_lFocusZState = 2;
		}
	}
	m_bAutoFocusDown = TRUE;

	LONG lResult = PR_AutoFocus(szMsg);
	if (lResult != -1)
	{
		if (lResult == 1)
		{
			WPR_GetEncoderValue();
			m_bAutoFocusOK = TRUE;
			if (bByHmi != -1 && bByHmi != -2)	//	427TX	4
			{
				if (bWft)
				{
					m_lAutoFocusPosition2 = m_lEnc_AF;
				}
				else
				{
					m_lAutoFocusPosition = m_lEnc_AF;
				}
				SavePrData(FALSE);
			}
			else
			{
				(*m_psmfSRam)["WPR Auto Focus"]["Sample Level"] = m_lEnc_AF;
			}
			szDisplay = "Auto focus ok!";
		}
		else
		{
			if (bByHmi != -1 && bByHmi != -2)
			{
				bResult = FALSE;
			}
			else	//	427TX	4
			{
				(*m_psmfSRam)["WPR Auto Focus"]["Sample Level"] = WT_AF_LEVEL_FAIL;
			}
			szDisplay = "Auto focus error!";
		}
	}
	else
	{
		bResult = FALSE;
		szDisplay = "Auto focus error!";
	}

	if (bResult == FALSE)
	{
		HmiMessage_Red_Back(szMsg, szDisplay);
	}
	else if (bByHmi == TRUE)	//	427TX	4
	{
		HmiMessage(szMsg, szDisplay);
	}

	SetErrorMessage(szMsg);
	SaveScanTimeEvent(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}


LONG CWaferPr::PR_AutoFocus(CString &szMsg)
{
	PR_AUTOFOCUS_PROCESS_CMD	stAFCmd;
	PR_AUTOFOCUS_PROCESS_RPY	stAFRpy;
	PR_WIN						stWin;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	OpenWaitingAlert();

	PR_InitAutofocusProcessCmd(&stAFCmd);

	if ((IsNormalDieLearnt() == FALSE) || (m_bDieCalibrated == FALSE))
	{
		stAFCmd.emPurpose = PR_PURPOSE_CAMERA_1;
	}
	else
	{
		stAFCmd.emPurpose = PR_PRE_LF_ALIGN;    // ON THE FLY, NOT USED YET
	}	

	// Region 1
	stWin.coCorner1.x = GetPRWinULX();
	stWin.coCorner1.y = GetPRWinULY();
	stWin.coCorner2.x = GetPRWinLRX();
	stWin.coCorner2.y = GetPRWinLRY();

	stAFCmd.uwNumOfRegion	= 1;
	stAFCmd.emMeasurement	= PR_AUTOFOCUS_MEASUREMENT_RFF;
	stAFCmd.astRegion[0].rWeight	= 1.0f;
	stAFCmd.astRegion[0].stRegion.uwNumOfCorners	= 4;
	stAFCmd.astRegion[0].stRegion.acoCorners[0].x	= stWin.coCorner2.x;
	stAFCmd.astRegion[0].stRegion.acoCorners[0].y	= stWin.coCorner1.y;
	stAFCmd.astRegion[0].stRegion.acoCorners[1]		= stWin.coCorner1;
	stAFCmd.astRegion[0].stRegion.acoCorners[2].x	= stWin.coCorner1.x;
	stAFCmd.astRegion[0].stRegion.acoCorners[2].y	= stWin.coCorner2.y;
	stAFCmd.astRegion[0].stRegion.acoCorners[3]		= stWin.coCorner2;

	stAFCmd.rPositiveLrnLimit	= (float)m_lAFUpLimit;
	stAFCmd.rNegativeLrnLimit	= (float)m_lAFLowLimit;
	stAFCmd.rMinMovingDistance	= (float)m_lAFMinDist;
	stAFCmd.rErrTolerance		= (float)m_lAFErrorTol;

	PR_AutofocusProcessCmd(&stAFCmd, ubSID, ubRID, &stAFRpy);

	LONG lResult = 1;
	if (stAFRpy.stStatus.uwCommunStatus == PR_COMM_NOERR &&
			stAFRpy.stStatus.uwPRStatus == PR_ERR_NOERR)
	{
		if (stAFRpy.astROIResult[0].rValue >= m_dAFrValueLimit)
		{
			lResult = 1;
			szMsg.Format("WPR: Auto focus success at position %d with rValue %f!", m_lEnc_AF, stAFRpy.astROIResult[0].rValue);
		}
		else
		{
			lResult = 0;
			szMsg.Format("WPR: Auto focus failure becuase rValue %f less than %f!",
						 stAFRpy.astROIResult[0].rValue, m_dAFrValueLimit);
		}
	}
	else
	{
		lResult = -1;
		szMsg.Format("WPR: Auto focus failure becuase PR status %u, %u index %d !",
					 stAFRpy.stStatus.uwCommunStatus, stAFRpy.stStatus.uwPRStatus, stAFRpy.rOptFocusIndex);
	}
	CloseWaitingAlert();	// start/open by wafer pr station

	return lResult;
}




VOID CWaferPr::WPR_GetEncoderValue()
{
	if (m_fHardware && IsWprWithAF())
	{
		try
		{
			if (CMS896AStn::MotionIsServo(WAFERPR_AXIS_Z, &m_stZoomAxis_Z))
			{
				m_lEnc_AF = CMS896AStn::MotionGetEncoderPosition(WAFERPR_AXIS_Z, 1, &m_stZoomAxis_Z);
			}
			else
			{
				m_lEnc_AF = CMS896AStn::MotionGetEncoderPosition(WAFERPR_AXIS_Z, 0.8, &m_stZoomAxis_Z);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
		}

		if (IsBLInUse())
		{
			try
			{
				m_lEnc_BLZ = CMS896AStn::MotionGetEncoderPosition(WAFERPR_BACKLIGHT_Z, 0.8, &m_stBackLight_Z);
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
			}
		}
	}
}

INT CWaferPr::AF_Z_Move(INT nPos, INT nMode)
{
	INT nStatus = gnOK;
	if (IsWprWithAF())
	{
		nStatus = Z_Move(nPos, nMode);
		if( m_lAFPrDelay>0 )
		{
			Sleep(m_lAFPrDelay);
		}
	}

	return nStatus;
}


LONG CWaferPr::ConfirmFocusLevel(IPC_CServiceMessage &svMsg)
{
	if (IsWprWithAF())
	{
		if (HmiMessage("Do you want to confirm focust level?", "ES100", glHMI_MBX_YESNO) == glHMI_YES)
		{
			Z_Sync();
			WPR_GetEncoderValue();
			m_bAutoFocusOK = TRUE;
			if (IsWT2UnderCamera())
			{
				m_lAutoFocusPosition2 = m_lEnc_AF;
			}
			else
			{
				m_lAutoFocusPosition = m_lEnc_AF;
			}
			SavePrData(FALSE);
		}
	}

	return 1;
}

LONG CWaferPr::GotoFocusLevel(IPC_CServiceMessage &svMsg)
{
	if (IsWprWithAF())
	{
		Z_Sync();
		if (IsWLExpanderOpen() == TRUE)
		{
			SetErrorMessage("Expander not closed, can not move focus z");
		}
		else
		{
			if (IsWT2UnderCamera())	//	4.24TX2
			{	
				//	427TX	4
				Z_MoveTo(m_lAutoFocusPosition2, SFM_WAIT);
				(*m_psmfSRam)["WPR Auto Focus"]["Sample Level"] = m_lAutoFocusPosition2;
				m_lFocusZState = 22;
			}
			else
			{	
				//	427TX	4
				Z_MoveTo(m_lAutoFocusPosition, SFM_WAIT);
				(*m_psmfSRam)["WPR Auto Focus"]["Sample Level"] = m_lAutoFocusPosition;
				m_lFocusZState = 2;
			}
		}
		m_bAutoFocusDown = TRUE;
	}

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

LONG CWaferPr::GotoHomeLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	if (IsWprWithAF())
	{
		Z_Sync();
		WPR_GetEncoderValue();
		if (abs(m_lEnc_AF) > 1000)
		{
			if (Z_MoveTo(0 + m_lAFStandbyOffset, SFM_WAIT) != gnOK)
			{
				bResult = FALSE;
			}
			m_lFocusZState = 0;
		}
		m_bAutoFocusDown = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

LONG CWaferPr::GotoTableFocus(IPC_CServiceMessage &svMsg)
{
	if (IsWprWithAF())
	{
		BOOL bWft = FALSE;
		svMsg.GetMsg(sizeof(BOOL), &bWft);
		Z_Sync();
		if (IsWLExpanderOpen() == TRUE)
		{
			SetErrorMessage("Expander not closed, can not move focus z");
		}
		else
		{
			if (bWft)	//	4.24TX 4
			{
				Z_MoveTo(m_lAutoFocusPosition2, SFM_NOWAIT);
			}
			else
			{
				Z_MoveTo(m_lAutoFocusPosition, SFM_NOWAIT);
			}
			if (bWft)
			{
				m_lFocusZState = 11;
			}
			else
			{
				m_lFocusZState = 1;
			}
		}
		m_bAutoFocusDown = TRUE;
	}

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}	// to table focus level

LONG CWaferPr::FocusZSync(IPC_CServiceMessage &svMsg)	//	4.24TX 4
{
	BOOL bResult = TRUE;
	if (IsWprWithAF())
	{
		bResult = (Z_Sync() == gnOK);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

LONG CWaferPr::IMGetPRDeviceIdCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	IMGetPRDeviceId();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SetLockPrMouseJoystickCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bLock = FALSE;
	BOOL bOK = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bLock);
	
	SetLockPrMouseJoystick(bLock);

	svMsg.InitMessage(sizeof(BOOL), &bOK);
	return 1;
}

LONG CWaferPr::InputSaveImagePath(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szSaveImagePath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szSaveImagePath = &pBuffer[0];

	delete[] pBuffer;

	if (CreateDirectory(szSaveImagePath, NULL) == 0)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			m_szSaveImagePath = szSaveImagePath;
			bReply = TRUE;
		}
		else
		{
			CString szContent;
			szContent.LoadString(HMB_GENERAL_INVALID_PATH);
			HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);			
			m_szSaveImagePath = WPR_PR_DISPLAY_IMAGE_LOG;
			bReply = FALSE;
		}
	}
	else
	{
		m_szSaveImagePath = szSaveImagePath;
		bReply = TRUE;
	}

	SavePrData(FALSE);
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 

LONG CWaferPr::GetSaveImagePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	if (pAppMod != NULL)
	{
		pAppMod->GetPath(m_szSaveImagePath);
		bReturn = TRUE;
		SavePrData(FALSE);
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::UpdateDefectFromHmi(IPC_CServiceMessage &svMsg)	// for defect and chip area.
{
	LONG	lDieNo = 0;

	lDieNo = GetWprDieNo();

	//v3.99T2	
	if ((m_dSrchSingleDefectArea > 0) && (m_dSrchTotalDefectArea < m_dSrchSingleDefectArea))
	{
		m_dSrchTotalDefectArea = m_dSrchSingleDefectArea;
	}

	m_dGenSrchSingleDefectArea[lDieNo]	= m_dSrchSingleDefectArea;
	m_dGenSrchTotalDefectArea[lDieNo]	= m_dSrchTotalDefectArea;
	m_dGenSrchChipArea[lDieNo]			= m_dSrchChipArea;

	m_bGenSrchEnableDefectCheck[lDieNo]	= m_bSrchEnableDefectCheck;
	m_bGenSrchEnableChipCheck[lDieNo]	= m_bSrchEnableChipCheck;

	SavePrData(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::UserSearchRefDie_NoMove_GivenID(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		BOOL	bStatus;
		INT		nOffsetX;
		INT		nOffsetY;
		DOUBLE	dAngle;
	} SRCH_RESULT;
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	SRCH_RESULT		stResult;

	LONG lPrID = 0, lStartID, lEndID;
	svMsg.GetMsg(sizeof(LONG), &lPrID);
	if (lPrID == 1)
	{
		lStartID = lEndID = lPrID;
	}
	else
	{
		lStartID = 1;
		lEndID = m_lLrnTotalRefDie;
		if (lStartID > lEndID)
		{
			lStartID = lEndID;
		}
	}
	stResult.nOffsetX = 0;
	stResult.nOffsetY = 0;
	stResult.dAngle = 0;

	//Get the Search Die Area Region for Reference Die from the array
	PR_WIN stSrchArea;
	//GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET );

	if (PR_NotInit())
	{
		stResult.bStatus = FALSE;
	}
	else
	{
		//v4.08		//Use all available ref-die pattern for manual wafer alignment instead
		//search all learnt reference die
		LONG lSrchID = 0;
		for (lSrchID = lStartID; lSrchID <= lEndID; lSrchID++)
		{
			GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET + lSrchID - 1);

			if (ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE + lSrchID - 1, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2) != -1)
			{	
				//bFindRefDie = TRUE;
				if (DieIsAlignable(usDieType) == TRUE)
				{
					stResult.bStatus = TRUE;
					CalculateDieCompenate(stDieOffset, &stResult.nOffsetX, &stResult.nOffsetY);
					stResult.dAngle = (DOUBLE) fDieRotate;
					break;
				}
				else
				{
					//v4.11T3
					stResult.bStatus = FALSE;
				}
			}
		}
	}

	svMsg.InitMessage(sizeof(SRCH_RESULT), &stResult);

	return 1;
}

LONG CWaferPr::SearchCurrentDieInFOV_Spiral(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bStatus = TRUE;
	BOOL			bDieType = WPR_NORMAL_DIE;
	LONG			lRefDieNo = 1;
	int				siStepX = 0; 
	int				siStepY = 0; 
	BOOL			bBackupAlignTemp;

	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX() - 1, GetPRWinLRY() - 1};

	typedef struct 
	{
		BOOL		bShowPRStatus;
		BOOL		bNormalDie;
		LONG		lRefDieNo;
		BOOL		bDisableBackupAlign;
	} SRCH_TYPE;

	SRCH_TYPE	stSrchInfo;

	typedef struct 
	{
		BOOL		bStatus;
		BOOL		bGoodDie;
		BOOL		bFullDie;
		LONG		lRefDieNo;
		LONG		lX;
		LONG		lY;
	} REF_TYPE;

	REF_TYPE	stInfo;

	if (PR_NotInit())
	{
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
		svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
		return 1;
	}

	//Get Information from other station
	svMsg.GetMsg(sizeof(SRCH_TYPE), &stSrchInfo);

	//Do Search die on this position
	lRefDieNo = stSrchInfo.lRefDieNo;

	if (stSrchInfo.bNormalDie == TRUE)
	{
		bDieType = WPR_NORMAL_DIE;
	}
	else
	{
		bDieType = WPR_REFERENCE_DIE;
	}

	PR_WORD wResult = 0;

	//Disable backup-align temporarily if currently enabled
	bBackupAlignTemp = m_bSrchEnableBackupAlign;
	if (stSrchInfo.bDisableBackupAlign == TRUE)
	{
		m_bSrchEnableBackupAlign = FALSE;
	}

	wResult = SemiSearchDie(bDieType, lRefDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);

	m_bSrchEnableBackupAlign = bBackupAlignTemp;

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		}

		//Return result
		stInfo.bStatus	= TRUE;
		stInfo.bGoodDie	= DieIsGood(usDieType);
		stInfo.bFullDie	= DieIsAlignable(usDieType);
		stInfo.lX		= (LONG)(siStepX);
		stInfo.lY		= (LONG)(siStepY);

		//Display PR status if necessary
		if (stSrchInfo.bShowPRStatus == TRUE)
		{
			CString szDieResult;
			ConvertDieTypeToText(usDieType, szDieResult);
			m_szSrhDieResult = "Die Type: " + szDieResult;
		}
	}
	else
	{
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
	}
	
	svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
	return 1;
}

LONG CWaferPr::IMManualAutoLearnWaferSeperated(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;
	m_bIsAutoLearnWaferNOTDone = FALSE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("ES101AutoLearnWaferNotDone"), FALSE);
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferPr::IMManualAutoLearnWafer(IPC_CServiceMessage &svMsg)
{
	m_bIsAutoLearnWaferNOTDone = FALSE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("ES101AutoLearnWaferNotDone"), FALSE);
	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferPr::CheckLearnAlignAlgorithm(IPC_CServiceMessage &svMsg)
{
	if (m_bSelectDieType == WPR_NORMAL_DIE)
	{
		if (m_lLrnAlignAlgo == 5)
		{
			m_lLrnAlignAlgo = 0;
		}
	}
	else
	{
		if (m_bAOINgPickPartDie)
		{
			m_lLrnAlignAlgo = 5;
		}
		else
		{
			if (m_lLrnAlignAlgo == 5)
			{
				m_lLrnAlignAlgo = 0;
			}
		}
	}
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::MoveBackLightToSafeLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	if (IsBLInUse())
	{
	//	SaveScanTimeEvent("BLZ to safe by IPC");
		if (BLZ_MoveTo(m_lBackLightElevatorStandByLevel) != gnOK)
		{
			bResult = FALSE;
		}
		m_lBackLightZStatus = 0;	//	4.24TX 4
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

LONG CWaferPr::MoveBackLightToUpStandby(IPC_CServiceMessage &svMsg)
{
	BOOL bToUp = FALSE;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bToUp);
	BLZ_Sync();
	if (bToUp)
	{
		bReturn = (BLZ_MoveTo(m_lBackLightElevatorUpLevel, SFM_NOWAIT) == gnOK);
		m_lBackLightZStatus = 1;	//	4.24TX 4
	}
	else
	{
		bReturn = (BLZ_MoveTo(m_lBackLightElevatorStandByLevel, SFM_NOWAIT) == gnOK);
		m_lBackLightZStatus = 3;	//	4.24TX 4
	}

	if (bReturn == FALSE)
	{
		SetMotionCE(TRUE, "Back Light Z motion error, please home and power it");
		HmiMessage_Red_Back("Back Light Z motion error, please home and power it!", "Back Light Z");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::MoveBackLightToUpStandbyRoutine(IPC_CServiceMessage &svMsg)
{
	BOOL bToUp = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bToUp);

	if (IsBLInUse())
	{
	//	if (bToUp)
	//	{
	//	//	SaveScanTimeEvent("BLZ to UP by Routine");
	//	}
	//	else
	//	{
	//	//	SaveScanTimeEvent("BLZ to DN by Routine");
	//	}
		MoveBackLightWithTableCheck(bToUp);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferPr::MoveBackLightWithTableCheck(CONST BOOL bToUp, CONST BOOL bToLocateDie)
{
	if (IsBLInUse() == FALSE)
	{
		return TRUE;
	}

	if (bToUp)
	{
		if (IsWT1UnderCamera() || IsWT2UnderCamera())
		{
			BLZ_MoveTo(m_lBackLightElevatorUpLevel);
			m_lBackLightZStatus = 2;	//	4.24TX 4
			Sleep(100);
			if (bToLocateDie)
			{
				SetWaferTableJoystick(TRUE);
				//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("Please move PR center to target die!");		//v4.43T10
				HmiMessageEx("Please move PR center to target die!", "ES101",
							 glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300, 0, 0, 0, 0, 400, 400);
				SetWaferTableJoystick(FALSE);
			}
		}
	}
	else
	{
		BLZ_MoveTo(m_lBackLightElevatorStandByLevel);
		m_lBackLightZStatus = 0;	//	4.24TX 4
	}

	return TRUE;
}

LONG CWaferPr::BondMoveBackLightToStandby(IPC_CServiceMessage &svMsg)
{
	BOOL bToUp = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bToUp);

	if (IsBLInUse() == FALSE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->m_bCycleStarted == TRUE)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsPrescanEnded() && IsBLInUse())
	{
	//	SaveScanTimeEvent("BLZ to DN IPC scan ended");
		BLZ_MoveTo(m_lBackLightElevatorStandByLevel);
		m_lBackLightZStatus = 0;	//	4.24TX 4
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CWaferPr::DownloadMultiSearchRecord(IPC_CServiceMessage &svMsg)
{
	OpPrescanInit(GetPrescanPrID());	// download when press start button

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::StartBackLightElevatorMotionTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

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
		m_lBackLightElevatorMoveTestCount = 0;
		m_bMoveDirection = TRUE;
		//LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, TRUE, 10, m_stDataLog);
	}
	else
	{
		BLZ_MoveTo(m_lBackLightElevatorStandByLevel);

		CString szTemp;
		szTemp.Format("BackLight Elevator Move Test Count:%d", m_lBackLightElevatorMoveTestCount);
		AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		//LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bBackLightElevatorMoveTest = stData.bIsStart;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::AutoLookAroundNearDie(IPC_CServiceMessage &svMsg)
{
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset; 
	PR_REAL				fDieScore;
	PR_COORD			stDieSize;
	PR_BOOLEAN			bInspection;
	PR_BOOLEAN			bPrLatch;
	PR_WIN		stLFSearchArea;

	typedef struct 
	{
		BOOL		bStatus;
		BOOL		bGoodDie;
		BOOL		bFullDie;
		LONG		lRefDieNo;
		LONG		lX;
		LONG		lY;
	} REF_TYPE;

	REF_TYPE	stInfo;
	//Return result
	stInfo.bStatus	= FALSE;
	stInfo.bGoodDie	= FALSE;
	stInfo.bFullDie	= FALSE;
	stInfo.lX		= 0;
	stInfo.lY		= 0;

	if (CMS896AApp::m_lVisionPlatform == PR_SYSTEM_AOI)		//v4.05		//Klocwork
	{
		bInspection = PR_TRUE;
	}
	else
	{
		bInspection = PR_FALSE;
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	bPrLatch = PR_TRUE;		//Need to LATCH if current cycle is NO-COMPENSATE
	BOOL bFindDie = FALSE;
	for (PR_WORD x = -1; x <= 1; x++)
	{
		for (PR_WORD y = -1; y <= 1; y++)
		{
			if (x == 0 && y == 0)
			{
				continue;
			}
			stLFSearchArea.coCorner1.x = GetSrchArea().coCorner1.x + GetPitchPixelXX() * x / 2;
			stLFSearchArea.coCorner2.x = GetSrchArea().coCorner2.x + GetPitchPixelXX() * x / 2;
			stLFSearchArea.coCorner1.y = GetSrchArea().coCorner1.y + GetPitchPixelYY() * y / 2;
			stLFSearchArea.coCorner2.y = GetSrchArea().coCorner2.y + GetPitchPixelYY() * y / 2;
			if (VerifyPRRegion(&stLFSearchArea) == FALSE)
			{
				continue;
			}
			usSearchResult = SearchDieCmd(WPR_NORMAL_DIE, WPR_NORMAL_DIE, 
										  ubSID, 
										  ubRID, 
										  stLFSearchArea.coCorner1, 
										  stLFSearchArea.coCorner2, 
										  bPrLatch, 
										  PR_TRUE, 
										  bInspection, 
										  PR_NO_DISPLAY);
			if (usSearchResult != PR_ERR_NOERR)
			{
				break;
			}

			//Get reply 1
			usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
			if (usSearchResult != PR_ERR_NOERR)
			{
				break;
			}

			//No need to LATCH for other LA dice beside die-1
			if (bPrLatch)	
			{
				bPrLatch = PR_FALSE;
			}

			//Get reply 2
			usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
			if (usSearchResult == PR_COMM_ERR)
			{
				break;
			}

			usDieType = stSrchRpy2.stStatus.uwPRStatus;
			if (usDieType == PR_ERR_NOERR)
			{
				usDieType = PR_ERR_GOOD_DIE;
			}

			if ((DieIsAlignable(usDieType)) && bInspection)
			{
				usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
				if (usSearchResult == PR_COMM_ERR)
				{
					break;
				}
			}

			ExtractDieResult(stSrchRpy2, stSrchRpy3, bInspection, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);
			if (DieIsAlignable(usDieType) == TRUE)
			{
				int siStepX = 0, siStepY = 0;
				if (DieIsAlignable(usDieType) == TRUE)
				{
					CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
				}

				//Return result
				stInfo.bStatus	= TRUE;
				stInfo.bGoodDie	= DieIsGood(usDieType);
				stInfo.bFullDie	= TRUE;
				stInfo.lX		= (LONG)(siStepX);
				stInfo.lY		= (LONG)(siStepY);
				bFindDie = TRUE;
				break;
			}
		}
		if (bFindDie)
		{
			break;
		}
	}

	svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
	return 1;
}	// auto look around near die


LONG CWaferPr::TurnOnOffPrLightingCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::EnablePickAndPlace(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = TRUE;
	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == _T("SanAn"))
	{
		if (bOn)
		{
//AfxMessageBox("TRUE", MB_SYSTEMMODAL);
			m_bThetaCorrection		= TRUE;
			m_ulMaxNoDieSkipCount	= 0;
		}
		else
		{
//AfxMessageBox("FALSE", MB_SYSTEMMODAL);
			m_bThetaCorrection		= FALSE;
			m_ulMaxNoDieSkipCount	= 2;
		}
		
		SavePrData(FALSE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::RefreshAllRingLighting(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ( IsES101() != TRUE || pApp->GetCustomerName() != _T("Yealy") )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;


	if ( m_lWPRSpecial1_RingLightLevel < 0 )
	{
		m_lWPRSpecial1_RingLightLevel = 0;
	}
	else if ( m_lWPRSpecial1_RingLightLevel > 100 )
	{
		m_lWPRSpecial1_RingLightLevel = 100;
	}
	if ( m_lWPRSpecial2_RingLightLevel < 0 )
	{
		m_lWPRSpecial2_RingLightLevel = 0;
	}
	else if ( m_lWPRSpecial2_RingLightLevel > 100 )
	{
		m_lWPRSpecial2_RingLightLevel = 100;
	}


	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();
	PR_GetOptic(ubPpsG, ubSID, ubRID, &stOptic, &stRpy);

	PR_SetLighting(PR_RING_LIGHT, (PR_UWORD)m_lWPRGenRingLightLevel[0], ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(PR_RING_LIGHT_1, (PR_UWORD)m_lWPRSpecial1_RingLightLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(PR_RING_LIGHT_2, (PR_UWORD)m_lWPRSpecial2_RingLightLevel, ubSID, ubRID, &stOptic, &stRpy);
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::IsFaceValueExists(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lFaceValueFromMap = m_lRefDieFaceValueNo;

	INT nConvID;
	IPC_CServiceMessage rReqMsg;

	rReqMsg.InitMessage(sizeof(LONG), &lFaceValueFromMap);

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "FaceValueExistsInMap", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bReturn == FALSE)
	{
		HmiMessage_Red_Back("Face Value chosen does not exist in Map. Please check!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::GenerateConfigData(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	CString szConfigFile = (*m_psmfSRam)["MS896A"]["ParameterListPath"];
		//_T("c:\\MapSorter\\UserData\\Parameters.csv");
	CStdioFile oFile;
	CString szLine;
	CString szMsg;

	szMsg = _T("WL: Generate Configuration Data");
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	if (oFile.Open(szConfigFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		szMsg.Format("WL: Generate Configuration Data - open fail");
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		return FALSE;
	}

	oFile.SeekToEnd();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp ->GetCustomerName() == "EverVision")
	{
		oFile.WriteString("[Wafer Inspection Setup]\n");

		szMsg.Format("%d",m_lWPRLrnCoaxLightHmi);
		oFile.WriteString("Wafer Coaxial Light,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lWPRLrnRingLightHmi);
		oFile.WriteString("Wafer Ring Light,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lWPRLrnSideLightHmi);
		oFile.WriteString("Wafer Ring Light,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lSrchDieScore);
		oFile.WriteString("Die Pass Score,"			+ szMsg + "\n");

		szMsg.Format("%d",m_bSrchEnableChipCheck);
		oFile.WriteString("Check Chip ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%.f",m_dSrchChipArea);
		oFile.WriteString("Chip %,"					+ szMsg + "\n");

		szMsg.Format("%d",m_bSrchEnableDefectCheck);
		oFile.WriteString("Check Defect ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%.f",m_dSrchSingleDefectArea);
		oFile.WriteString("Single Defect %,"			+ szMsg + "\n");

		szMsg.Format("%.f",m_dSrchTotalDefectArea);
		oFile.WriteString("Total Defect %,"			+ szMsg + "\n");

		szMsg.Format("%.f",m_dSrchDieAreaX);
		oFile.WriteString("Search Area Size X,"		+ szMsg + "\n");

		szMsg.Format("%.f",m_dSrchDieAreaY);
		oFile.WriteString("Search Area Size Y,"		+ szMsg + "\n");

		szMsg.Format("%d",m_bThetaCorrection);
		oFile.WriteString("Enable Theta Correction ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%.f",m_dAcceptDieAngle);
		oFile.WriteString("Tolerance,"		+ szMsg + "\n");

		szMsg.Format("%d",m_bSrchEnableLookForward);
		oFile.WriteString("Enable Look Forward ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%d",m_bBadCutDetection);
		oFile.WriteString("Enable BadCut ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%.f",m_dSetRegionByDieSizeX);
		oFile.WriteString("Set Region By X %,"		+ szMsg  + "\n");

		szMsg.Format("%.f",m_dSetRegionByDieSizeY);
		oFile.WriteString("Set Region By Y %,"		+ szMsg + "\n");

		szMsg.Format("%d",m_stBadCutTolerenceX);
		oFile.WriteString("Tolerance X %,"		+ szMsg + "\n");

		szMsg.Format("%d",m_stBadCutTolerenceY);
		oFile.WriteString("Tolerance Y %,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lSrchDefectThres);
		oFile.WriteString("Defect Thresold,"		+ szMsg + "\n");
	}
	else if (pApp ->GetCustomerName() == "ChangeLight(XM)")
	{
		oFile.WriteString("[Wafer Inspection Setup]\n");
		szMsg.Format("%d",m_bBadCutDetection);
		oFile.WriteString("Enable Bad Cut,"		+ szMsg + "\n");
	}

	oFile.Close();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::RunTimeDownloadPrRecords(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szCurrPkgName = pApp->GetPKGFilename();

	BOOL bLoadWFT = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bLoadWFT);
	CString szLog;
	szLog.Format("RT download PR, curr %s, prev %s, new %s", m_szPkgNameCurrent, m_szPkgNamePrevious, szCurrPkgName);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

	if( pApp->IsRuntime2PkgInUse() )
	{
		if( m_szPkgNameCurrent.CompareNoCase(szCurrPkgName)==0 )	// new is the one currently in use.
		{
			DeletePrRecords(svMsg);	// Delete the WaferPR Records files
			CMSLogFileUtility::Instance()->WL_LogStatus("PKG is current in use.");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	UCHAR ucFree_DL = 0;	// 0 = first download; 1 = swap; 2 = thrid PKG, free previous and download
	if( m_szPkgNamePrevious.IsEmpty() )
	{
		ucFree_DL = 0;
	}
	else if( m_szPkgNamePrevious.CompareNoCase(szCurrPkgName)==0 )
	{
		ucFree_DL = 1;
	}
	else
	{
		ucFree_DL = 2;
	}

	if( pApp->IsRuntime2PkgInUse()==FALSE )
	{
		ucFree_DL = 3;
	}
	else
	{
		if( ucFree_DL==0 || ucFree_DL==2 )
		{
			CONST CString szExePath = gszROOT_DIRECTORY + "\\EXE\\";
			CString szTgtFile = szExePath + "WaferPr_" + m_szPkgNamePrevious + ".msd";
			DeleteFile(szTgtFile);
			szTgtFile = szExePath + "WaferTable_" + m_szPkgNamePrevious + ".msd";
			DeleteFile(szTgtFile);

			CMSLogFileUtility::Instance()->WL_LogStatus("get waferpr msd, wafer table msd and PR record files begin.");
			CString szFullPath = (*m_psmfSRam)["WPR"]["Package File Full Path"];
			ObtainWftWprFiles(szFullPath, szCurrPkgName, bLoadWFT);
			CMSLogFileUtility::Instance()->WL_LogStatus("get waferpr msd, wafer table msd and PR record files done.");
		}
		else
		{
			CONST CString szExePath = gszROOT_DIRECTORY + "\\EXE\\";
			CString szSrcFile = szExePath + "WaferPr_" + szCurrPkgName + ".msd";
			CString szTgtFile = szExePath + "WaferPr.msd";
			CopyFileWithRetry(szSrcFile, szTgtFile, FALSE);
			if( bLoadWFT )
			{
				szSrcFile = szExePath + "WaferTable_" + szCurrPkgName + ".msd";
				szTgtFile = szExePath + "WaferTable.msd";
				CopyFileWithRetry(szSrcFile, szTgtFile, FALSE);
			}
		}
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	//	not load and not save, just down load PR records, as the status is in PKG file msd
	CMSLogFileUtility::Instance()->WL_LogStatus("RT download PR, load msd and reset.");
	LoadWaferPrOption();
	LoadPrData();
	for (int i = 0; i < WPR_MAX_DIE ; i++)
	{
		m_ssGenPRSrchID[i] = 0;
		m_bGenDieLearnt[i] = FALSE;
	}

	if( ucFree_DL==1 )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("RT download PR, switch record");
	}
	else
	{
		// not free, reset id, download, get id, update list, save
		if( ucFree_DL==2 )	// free old before download
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("RT download PR, 2, free old");
			for(INT i=0; i<WPR_MAX_DIE; i++)
			{
				if (m_saPkgRecordPrevious[i] != 0)
				{
					m_pPrGeneral->FreePRRecord(m_saPkgRecordPrevious[i], GetRunSenID(), GetRunRecID());
				}
			}
		}

		if( ucFree_DL==3 && pApp->IsRuntime2PkgInUse()==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("RT download PR, 3, free all records");
			FreeAllPrRecords(FALSE);	// run time download, only one
		}

		CMSLogFileUtility::Instance()->WL_LogStatus("RT download PR, download files");
		m_szPkgNamePrevious		= szCurrPkgName;
		for (int i = 0; i < WPR_MAX_DIE ; i++)
		{
			m_saPkgRecordPrevious[i] = 0;
		}

		//boolean = false
		m_bIsAutoLearnWaferNOTDone = FALSE;
		pApp->WriteProfileInt(gszPROFILE_SETTING, _T("ES101AutoLearnWaferNotDone"), FALSE);

		CHAR acTemp[PR_MAX_FILE_NAME_LENGTH];
		PR_DOWNLOAD_RECORD_PROCESS_CMD	stCmd;
		PR_DOWNLOAD_RECORD_PROCESS_RPY	stRpy;

		BOOL bDLOnce = FALSE;
		// Download the normal die and reference die (assume index 0 is normal die)
		CString szPath = (*m_psmfSRam)["PR"]["RecordPath"];
		CString szTemp = "";

		for (int i = 0; i < WPR_MAX_DIE; i++)	//v3.06
		{
			PR_InitDownloadRecordProcessCmd(&stCmd);
			szTemp.Format("%s\\WPR%d.rec", (LPCTSTR) szPath, i);

			//sprintf_s(acTemp, sizeof(acTemp), "%s\\WPR%d.rec", (LPCTSTR) szPath, i);	//v4.53	Klocwork
			sprintf_s(acTemp, sizeof(acTemp), szTemp);			//v4.53	Klocwork

			// Check whether the file is existing
			if (_access(acTemp, 0) == -1)
			{
				continue;
			}

			stCmd.ulRecordID = 0;		// Auto-assign record ID
			stCmd.emIsOverwrite = PR_TRUE;
			strcpy_s((char*)stCmd.acFilename, sizeof(stCmd.acFilename), acTemp);
			PR_DownloadRecordProcessCmd(&stCmd, ubSID, ubRID, &stRpy);

			bDLOnce = TRUE;
			m_saPkgRecordPrevious[i] = m_ssGenPRSrchID[i] = (PR_WORD)stRpy.ulRetRecordID;
			m_bGenDieLearnt[i] = TRUE;	

			if( stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR ||
				stRpy.stStatus.uwPRStatus != PR_ERR_NOERR )
			{
				CString csMsg;
				csMsg.Format("Runtime Download Wafer PR record file %d.rec error! comm-err = 0x%x, pr-err = 0x%x", i,
								stRpy.stStatus.uwCommunStatus, stRpy.stStatus.uwPRStatus);
				// Not call display message since it will be very slow 
				HmiMessage_Red_Back(csMsg);
				bReturn = FALSE;
				break;
			}
		}

		if (bDLOnce == FALSE)
		{
			HmiMessage("No record file downloaded!");
			bReturn = FALSE;
		}

		if( m_bIsAutoLearnWaferNOTDone )
		{
			HmiMessage("PR record come from other machine, Press \"Auto-Learn Wafer\" button!");
		}
	}

	CString szTempName;
	PR_WORD saList[WPR_MAX_DIE];
	// back up current
	szTempName = m_szPkgNameCurrent;
	for(int i=0; i<WPR_MAX_DIE; i++)
	{
		saList[i] = (PR_WORD)m_saPkgRecordCurrent[i];
	}
	// pass previous to current 
	m_szPkgNameCurrent = m_szPkgNamePrevious;
	for(int i=0; i<WPR_MAX_DIE; i++)
	{
		m_saPkgRecordCurrent[i] = m_saPkgRecordPrevious[i];
	}
	// swap backuped current to previous
	m_szPkgNamePrevious = szTempName;
	for(int i=0; i<WPR_MAX_DIE; i++)
	{
		m_saPkgRecordPrevious[i] = saList[i];
	}

	szLog.Format("RT download PR, switch in use pkg, curr %s, prev %s, new %s", m_szPkgNameCurrent, m_szPkgNamePrevious, szCurrPkgName);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

	CMSLogFileUtility::Instance()->WL_LogStatus("RT download PR, delete record files");
	DeletePrRecords(svMsg);	// Delete the WaferPR Records	files	

	CMSLogFileUtility::Instance()->WL_LogStatus("RT download PR, save pr msd");
	//	not load and not save, just down load PR records, as the status is in PKG file msd
	SavePrData(FALSE);

	SaveRuntimeTwoRecords();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::PkgKeyParametersTask(IPC_CServiceMessage &svMsg)
{
	CStdioFile kFile;
	BOOL bReturn = TRUE;
	LONG lAction = 0;
	svMsg.GetMsg(sizeof(LONG), &lAction);

	LONG	lLrnAlignAlgo		= m_lGenLrnAlignAlgo[0];
	LONG	lSrchDieScore		= m_lGenSrchDieScore[0];
	LONG	lPScnSrchDieScore	= m_lPrescanSrchDieScore;
	ULONG	ulMaxNoDieSkipCount	= m_ulMaxNoDieSkipCount;

	if( lAction==0 || lAction==1 )
	{
		if( lAction==1 )
		{
			CStringMapFile psmf;
			CString szNewMsdName = "C:\\MapSorter\\UserData\\WaferPr.msd";
			if ( psmf.Open(szNewMsdName, FALSE, FALSE) )
			{
				lLrnAlignAlgo		= (psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_ALNALGO][1];
				lSrchDieScore		= (psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_PASSSCORE][1];
				lPScnSrchDieScore	= (psmf)[WPR_SRH_NDIE_DATA][WPR_PRESCAN_SRH_NDIE_PASSSCORE][1];
				ulMaxNoDieSkipCount	= (psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_NODIE_SKIP];
			}
			psmf.Close();
			DeleteFile(szNewMsdName);
		}

		if( kFile.Open(gszLocalPkgCheckListFile, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText) )
		{
			kFile.SeekToEnd();
			CString szText;
			szText.Format("%s",		"WPR list begin");		kFile.WriteString(szText + "\n");
			szText.Format("%ld",	lLrnAlignAlgo);			kFile.WriteString(szText + "\n");
			szText.Format("%ld",	lSrchDieScore);			kFile.WriteString(szText + "\n");
			szText.Format("%ld",	lPScnSrchDieScore);		kFile.WriteString(szText + "\n");
			szText.Format("%lu",	ulMaxNoDieSkipCount);	kFile.WriteString(szText + "\n");
			kFile.Close();
		}
	}
	else
	{
		CString szListMsg = "";
		if( kFile.Open(gszLocalPkgCheckListFile, CFile::modeRead|CFile::typeText) )
		{
			CString szText;
			while( kFile.ReadString(szText) )
			{
				if( szText.Find("WPR list begin")!=-1 )
				{
					break;
				}
			}

			if( kFile.ReadString(szText) )
			{
				lLrnAlignAlgo = atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				lSrchDieScore = atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				lPScnSrchDieScore = atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				ulMaxNoDieSkipCount = atol(szText);
			}
			kFile.Close();

			if( lLrnAlignAlgo != m_lGenLrnAlignAlgo[0] )
			{
				szText.Format("Learn Die Algorithm changed to %ld(%ld)", lLrnAlignAlgo, m_lGenLrnAlignAlgo[0]);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( lSrchDieScore != m_lGenSrchDieScore[0] )
			{
				szText.Format("Die seach score changed to %ld(%ld)", lSrchDieScore, m_lGenSrchDieScore[0]);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( lPScnSrchDieScore != m_lPrescanSrchDieScore )
			{
				szText.Format("Prescan search score changed to %ld(%ld)", lPScnSrchDieScore, m_lPrescanSrchDieScore);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( ulMaxNoDieSkipCount != m_ulMaxNoDieSkipCount )
			{
				szText.Format("No Die Skip limit changed to %lu(%lu)", ulMaxNoDieSkipCount, m_ulMaxNoDieSkipCount);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
		}
		else
		{
			bReturn = FALSE;
			szListMsg = "No compare file exist";
		}

		if( bReturn==FALSE )
		{
			SetErrorMessage(szListMsg);
			SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
			HmiMessage(szListMsg, "WPR Parameter Check");
			SetAlarmLamp_Yellow(FALSE, TRUE);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	key paramers check

BOOL CWaferPr::SaveRuntimeTwoRecords()
{
	if( IsAOIOnlyMachine()==FALSE )
	{
		return TRUE;
	}

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadCP_IndependentConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetCP_IndependentConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	CString szName;
	(*psmf)["AOI 2 Records In Use"]["Current PKG Name"]	= m_szPkgNameCurrent;
	(*psmf)["AOI 2 Records In Use"]["Previous PKG Name"]	= m_szPkgNamePrevious;
	for(int i=0; i<WPR_MAX_DIE; i++)
	{
		szName.Format("Record %02d", i);
		(*psmf)["AOI 2 Records In Use"]["Current Record"][szName]	= m_saPkgRecordCurrent[i];
		(*psmf)["AOI 2 Records In Use"]["Previous Record"][szName]	= m_saPkgRecordPrevious[i];
	}

	//Check Load/Save Data
	pUtl->SaveCP_IndependentConfig() ;
	 // close config file
	pUtl->CloseCP_IndependentConfig();

	return TRUE;
}


BOOL CWaferPr::LoadRuntimeTwoRecords()
{
	if( IsAOIOnlyMachine()==FALSE )
	{
		return TRUE;
	}

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadCP_IndependentConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetCP_IndependentConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	m_szPkgNameCurrent		= (*psmf)["AOI 2 Records In Use"]["Current PKG Name"];
	m_szPkgNamePrevious		= (*psmf)["AOI 2 Records In Use"]["Previous PKG Name"];
	CString szName;
	for(int i=0; i<WPR_MAX_DIE; i++)
	{
		szName.Format("Record %02d", i);
		m_saPkgRecordCurrent[i]		= (PR_WORD)(LONG)(*psmf)["AOI 2 Records In Use"]["Current Record"][szName];
		m_saPkgRecordPrevious[i]	= (PR_WORD)(LONG)(*psmf)["AOI 2 Records In Use"]["Previous Record"][szName];
	}

	//Check Load/Save Data
	pUtl->SaveCP_IndependentConfig() ;
	 // close config file
	pUtl->CloseCP_IndependentConfig();

	return TRUE;
}

BOOL CWaferPr::ObtainWftWprFiles(CString& szDevName, CString szCurrPkgName, BOOL bLoadWFT)
{
	CFile		objDevFile, objSrcFile; 
	LONG		lVersion;
	ULONG		i, ulNumOfFiles, ulDataLength;
	BYTE		*pbRawData;
	CHAR		acFilename[256];
	CString		szSmfName;

	if( !objDevFile.Open(szDevName, CFile::modeRead) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("device file can not open " + szDevName);
		return FALSE;
	}

	// Read version number
	objDevFile.Read(&lVersion, sizeof(LONG));
	if( lVersion==999 )
	{
		objDevFile.Close();
		CMSLogFileUtility::Instance()->WL_LogStatus("device file version is 999");
		return FALSE;
	}

	// Read the number of files saved
	objDevFile.Read(&ulNumOfFiles, sizeof(ULONG));

	for( i=0; i < ulNumOfFiles; i++ )
	{
		// Read filename
		objDevFile.Read(acFilename, 256 * sizeof(CHAR));
		szSmfName.Format("%s",acFilename);

		// Read file size
		objDevFile.Read(&ulDataLength, sizeof(ULONG));
		pbRawData = new BYTE[ulDataLength];
		objDevFile.Read(pbRawData, ulDataLength * sizeof(BYTE));

		if( szSmfName=="WaferLoader.msd" ||	szSmfName=="BinLoader.msd" || szSmfName=="BondHead.msd" ||
			szSmfName=="LastState.msd"	 || szSmfName=="BinTable.msd" )
		{
		}
		else if( (bLoadWFT==FALSE && szSmfName=="WaferTable.msd") )
		{
		}
		else
		{
			if( !objSrcFile.Open(acFilename, CFile::modeCreate | CFile::modeWrite) )
			{
				delete [] pbRawData;
				objDevFile.Close();
				CMSLogFileUtility::Instance()->WL_LogStatus("can not write file " + szSmfName);
				return FALSE;
			}

			objSrcFile.Write(pbRawData, ulDataLength * sizeof(BYTE));
			objSrcFile.Close();

			szSmfName.MakeLower();
			if( szSmfName.Find(".msd")!=-1 )
			{
				CMSFileUtility::Instance()->BackupMSDFile(szSmfName);
			}
		}

		delete [] pbRawData;
	}

	objDevFile.Close();

	CONST CString szExePath = gszROOT_DIRECTORY + "\\EXE\\";
	CString szSrcFile = szExePath + "WaferPr.msd";
	CString szTgtFile = szExePath + "WaferPr_" + szCurrPkgName + ".msd";
	CopyFileWithRetry(szSrcFile, szTgtFile, FALSE);
	if( bLoadWFT )
	{
		szSrcFile = szExePath + "WaferTable.msd";
		szTgtFile = szExePath + "WaferTable_" + szCurrPkgName + ".msd";
		CopyFileWithRetry(szSrcFile, szTgtFile, FALSE);
	}

	return TRUE;
}	// resort PR data from device file

BOOL CWaferPr::WPR_GetGeneralLighting()
{
	if (PR_NotInit())
	{
		return TRUE;
	}

	m_bWprUseBackLight = IsAOIOnlyMachine();

	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_UWORD		usWPRCoaxLightLevel;
	PR_UWORD		usWPRRingLightLevel;
	PR_UWORD		usWPRSideLightLevel;
	PR_UWORD		usWPRBackLightLevel;
	PR_SOURCE		emBackID = PR_BACK_LIGHT;
	PR_SOURCE		emCoaxID, emRingID, emSideID;
	int				i;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();

	PR_GetOptic(ubPpsG, ubSID, ubRID, &stOptic, &stRpy);
	if (CMS896AStn::m_bEnablePRDualColor == TRUE)
	{
		for (i = 0; i < WPR_MAX_LIGHT_GROUP; i++)
		{
			GetLightingConfig(i, emCoaxID, emRingID, emSideID);
			
			PR_GetLighting(&stOptic, emCoaxID, ubSID, ubRID, &usWPRCoaxLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emRingID, ubSID, ubRID, &usWPRRingLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emSideID, ubSID, ubRID, &usWPRSideLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emBackID, ubSID, ubRID, &usWPRBackLightLevel, &stRpy);

			m_lWPRGenCoaxLightLevel[i] = usWPRCoaxLightLevel;
			m_lWPRGenRingLightLevel[i] = usWPRRingLightLevel;
			m_lWPRGenSideLightLevel[i] = usWPRSideLightLevel;
			m_lWPRGenBackLightLevel[i] = usWPRBackLightLevel;
		}
	}
	else
	{
		GetLightingConfig(0, emCoaxID, emRingID, emSideID);
		PR_GetLighting(&stOptic, emCoaxID, ubSID, ubRID, &usWPRCoaxLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emRingID, ubSID, ubRID, &usWPRRingLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emSideID, ubSID, ubRID, &usWPRSideLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emBackID, ubSID, ubRID, &usWPRBackLightLevel, &stRpy);

	CString szMsg;
	szMsg.Format("WPR: DP get light Coax %d Ring %d Side %d", usWPRCoaxLightLevel, usWPRRingLightLevel, usWPRSideLightLevel);
	//SetAlarmLog(szMsg);
		for (i = 0; i < WPR_MAX_LIGHT_GROUP; i++)
		{
			m_lWPRGenCoaxLightLevel[i] = usWPRCoaxLightLevel;
			m_lWPRGenRingLightLevel[i] = usWPRRingLightLevel;
			m_lWPRGenSideLightLevel[i] = usWPRSideLightLevel;
			m_lWPRGenBackLightLevel[i] = usWPRBackLightLevel;
		}
	}

	return TRUE;
}

PR_WORD CWaferPr::SemiSearchDie(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
								  PR_COORD stCorner1, PR_COORD stCorner2)
{
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	PR_UWORD			usSearchResult;
    PR_COORD			stPRDieSize;
	BOOL				bGetRpy3 = FALSE;

    PR_COORD			stPRCoorCentre;
	stPRCoorCentre.x  = (PR_WORD)GetPrCenterX();
	stPRCoorCentre.y  = (PR_WORD)GetPrCenterY();

	PR_GRAPHIC_INFO		emInfo = PR_NO_DISPLAY;
	if ( m_bDebugVideoTest == TRUE )
		emInfo = PR_SHOW_DEFECT;

	if( IsBurnIn() && (m_bNoWprBurnIn ||IsAOIOnlyMachine()) )
	{
        *usDieType = PR_ERR_NOERR;
		*fDieRotate = 0.0;
		stDieOffset->x = 0;
		stDieOffset->y = 0;
		*fDieScore = 0;
		return 0;
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	FLOAT fOrientation = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
/*
	if( m_bCheckDieOrientation)
	{
		fOrientation = (FLOAT) (pUtl->GetSearchDieRotation());
	}
*/
	if (pApp->IsPLLMRebel() && bDieType==WPR_REFERENCE_DIE)	// auto enlarge window for refer
	{
		stCorner1.x = GetPRWinULX();
		stCorner1.y = GetPRWinULY();
		stCorner2.x = GetPRWinLRX() - 1;
		stCorner2.y = GetPRWinLRY() - 1;
	}

	usSearchResult = SearchDieCmd(bDieType, lDieNo, ubSID, ubRID, stCorner1, stCorner2, bLatch, bAlign, bInspect, emInfo, fOrientation);
//	pUtl->SetSearchDieRoation(0);

	CString csMsgA;
	if (usSearchResult != PR_ERR_NOERR)
	{
		if (usSearchResult == IDS_WPR_DIENOTLEARNT)
		{
			csMsgA.Format("WPR Not Learnt");
		}
		else
		{
			csMsgA.Format("Semi SearchDie Send Command = %x",usSearchResult);
		}
		WT_SpecialLog(csMsgA);

		return -1;
	}

	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
	if (usSearchResult != PR_ERR_NOERR)
	{
		csMsgA.Format("Semi SearchDie Get Reply 1 = %x",usSearchResult);
		WT_SpecialLog(csMsgA);
		return -1;
	}

	usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		csMsgA.Format("Semi SearchDie Get Reply 2 = %x",usSearchResult);
		WT_SpecialLog(csMsgA);
		return -1;
	}

    *usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (*usDieType == PR_ERR_NOERR)
	{
        *usDieType = PR_ERR_GOOD_DIE;
    }

	if ((DieIsAlignable(*usDieType) == TRUE) && (bInspect == TRUE))
	{
		bGetRpy3 = TRUE;
		usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			csMsgA.Format("Semi SearchDie Get Reply 3 = %x",usSearchResult);
			WT_SpecialLog(csMsgA);
			return -1;
		}
	}

	//Identify die type
	ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, usDieType, fDieRotate, stDieOffset, fDieScore, &stPRDieSize);
	
	CalculateDefectScore(*usDieType, lDieNo, &stSrchRpy3);

	return 0;
}

//4.52D17Auto Rpy1
BOOL CWaferPr::AutoUpLookSearchDie_Rpy1(IPC_CServiceMessage &svMsg)
{
	LONG lBHNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lBHNo);

	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
//	PR_COORD		stDieOffset; 
//	PR_REAL			fDieRotate; 
//	PR_REAL			fDieScore;
	BOOL			bReturn = FALSE;
	UCHAR ucPostSealID;
	LONG lSearchDieNo;
	CString csMsgA;


	if(lBHNo == 1)
	{
		lSearchDieNo = MS899_UPLOOK_REF_DIENO_BH1;  // uplook camera RefNo BH1 == 11
		ucPostSealID = 6;
	}
	else if (lBHNo == 2)
	{
		lSearchDieNo = MS899_UPLOOK_REF_DIENO_BH2;
		ucPostSealID = 7;
	}
	else
	{
		csMsgA.Format("BH = %d",lBHNo);
		SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (PR_NotInit())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_WIN stSrchArea;
	GetSearchDieArea(&stSrchArea, lSearchDieNo + WPR_GEN_RDIE_OFFSET);
	DrawRectangleBox(stSrchArea.coCorner1, stSrchArea.coCorner2, PR_COLOR_GREEN);
	
	// 4.52D17Note No prepick ,Mo Change Camera, No Lighting here, only move Swinglevel Outside
	
	//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Start Auto Search Rpy1");

	BOOL bDieType			= WPR_REFERENCE_DIE;
	LONG lDieNo				= lSearchDieNo;
	PR_BOOLEAN bLatch		= PR_TRUE; 
	PR_BOOLEAN bAlign		= PR_TRUE; 
	PR_BOOLEAN bInspect		= PR_TRUE;
/*	PR_UWORD *usDieType		= &usDieType;
	PR_REAL *fDieRotate		= &fDieRotate;
	PR_COORD *stDieOffset	= &stDieOffset;
	PR_REAL *fDieScore		= &fDieScore;	*/				
	PR_COORD stCorner1		= stSrchArea.coCorner1;
	PR_COORD stCorner2		= stSrchArea.coCorner2;
	BOOL bLog = FALSE;

	PR_SRCH_DIE_RPY1	stSrchRpy1;
//	PR_SRCH_DIE_RPY2	stSrchRpy2;
//	PR_SRCH_DIE_RPY3	stSrchRpy3;
//	CString				csMsgA;
	CString				csMsgB;
	PR_UWORD			usSearchResult;
//    PR_COORD			stPRCoorCentre;
//    PR_COORD			stPRDieSize;
	BOOL				bGetRpy3 = FALSE;
	PR_GRAPHIC_INFO		emInfo;

	//stPRCoorCentre.x  = (PR_WORD)GetPrCenterX();
	//stPRCoorCentre.y  = (PR_WORD)GetPrCenterY();

	m_dCurSrchDieSizeX	= 0;
	m_dCurSrchDieSizeY	= 0;

	if ( m_bDebugVideoTest == TRUE )
		emInfo = PR_SHOW_DEFECT;
	else
		emInfo = PR_NO_DISPLAY;

	//PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_UBYTE ubSID = PSPR_SENDER_ID;
	PR_UBYTE ubRID = PSPR_RECV_ID;

	FLOAT fOrientation = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
/*
	if( m_bCheckDieOrientation)
	{
		fOrientation = (FLOAT) (pUtl->GetSearchDieRotation());
	}
*/
	fOrientation = 9;	//disable 180 degree rotation for ALL uplook PR //v4.59A36

	//if (pApp->IsPLLMRebel() && bDieType==WPR_REFERENCE_DIE)	// auto enlarge window for refer
	//{
	//	stCorner1.x = GetPRWinULX();
	//	stCorner1.y = GetPRWinULY();
	//	stCorner2.x = GetPRWinLRX() - 1;
	//	stCorner2.y = GetPRWinLRY() - 1;
	//}
	
	usSearchResult = SearchDieCmd(bDieType, lDieNo, ubSID, ubRID, stCorner1, stCorner2, bLatch, bAlign, bInspect, emInfo, fOrientation);
//	pUtl->SetSearchDieRoation(0);
	if (usSearchResult != PR_ERR_NOERR)
	{
		if (usSearchResult == IDS_WPR_DIENOTLEARNT)
		{
			csMsgA.Format("WPR Not Learnt");
		}
		else
		{
			csMsgA.Format("WPR: Uplook PR Auto Send Command Err = %lu", usSearchResult);
		}
		//SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
		SetErrorMessage(csMsgA);
CMSLogFileUtility::Instance()->BT_TableIndexLog(csMsgA);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	usSearchResult = PR_ERR_NOERR;

	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
	//v4.57A13
	if (stSrchRpy1.uwCommunStatus != PR_COMM_NOERR)
	{
		usSearchResult = PR_COMM_ERR;
	}
	if (stSrchRpy1.uwPRStatus != PR_ERR_NOERR)
	{
		usSearchResult = stSrchRpy1.uwPRStatus;
	}

	if (usSearchResult != PR_ERR_NOERR)
	{
		csMsgA.Format("WPR: Uplook PR Auto Get Reply1 Err = %lu", usSearchResult);
		SetErrorMessage(csMsgA);
CMSLogFileUtility::Instance()->BT_TableIndexLog(csMsgA);
		//SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
		//return -1;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::UserSearchUpLookDieCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHUplookPrFcn	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
	if (bEnableBHUplookPrFcn && m_bCurrentCamera==WPR_CAM_WAFER && (m_unCurrPostSealID == 6 || m_unCurrPostSealID == 7))
	{
		m_bSelectDieType = WPR_REFERENCE_DIE;
		LONG lSearchDieNo = MS899_UPLOOK_REF_DIENO_BH1;
		if( m_unCurrPostSealID != 6 )
		{
			lSearchDieNo = MS899_UPLOOK_REF_DIENO_BH2;
		}
		UserSearchUpLookDie(lSearchDieNo, TRUE);
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferPr::UserSearchUpLookDie(LONG lSearchDieNo, BOOL bF1Srch)
{
	if (PR_NotInit())
	{
		return TRUE;
	}

	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;

	UCHAR ucPostSealID;

	PR_WIN stSrchArea;
	GetSearchDieArea(&stSrchArea, lSearchDieNo + WPR_GEN_RDIE_OFFSET);
	DrawRectangleBox(stSrchArea.coCorner1, stSrchArea.coCorner2, PR_COLOR_GREEN);

	if(lSearchDieNo == MS899_UPLOOK_REF_DIENO_BH1)
	{
		// uplook camera RefNo BH1 == 11
		ucPostSealID = 6;
	}
	else if (lSearchDieNo == MS899_UPLOOK_REF_DIENO_BH2)
	{
		ucPostSealID = 7;
	}

	ChangeCamera(WPR_CAM_WAFER, FALSE, ucPostSealID);
	//4.52D17light  manual search die
	GetPostSealLighting(ucPostSealID);
	//4.52D17Move manual search
	MoveBhToPick(FALSE); 

	CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Start Manual Search");
	//4.52D17manual start search
	if (ManualUpLookSearchDie(WPR_REFERENCE_DIE, lSearchDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2) != -1)
	{
		//Display Message on HMI
		DisplaySearchDieResult(usDieType, WPR_REFERENCE_DIE, lSearchDieNo + WPR_GEN_RDIE_OFFSET, fDieRotate, stDieOffset, fDieScore);
		bReturn = TRUE;
	}
	else
	{
		HmiMessage("Error: Manual Uplook Search Die Faile!");
		bReturn = FALSE;
	}


	if( bF1Srch==FALSE )
	{
		SetPostSealGenLighting(FALSE, 6);	
		SetPostSealGenLighting(FALSE, 7);	
	}

	return bReturn;
}

LONG CWaferPr::PRIDDetailInfo(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szTemp;
	
	if(m_szLrnDieType == WPR_HMI_NORM_DIE)
	{
		if(m_lCurRefDieNo == 1)
		{
			m_szDieTypeInfo = "Normal #1";
		}
		else if(m_lCurRefDieNo == 2)
		{
			m_szDieTypeInfo = "Second PR";
		}
		else if(m_lCurRefDieNo == 3)
		{
			m_szDieTypeInfo = "FF Mode (Prescan)";
		}
		else
		{
			m_szDieTypeInfo = "No die type";
		}
	}
	else if(m_szLrnDieType == WPR_HMI_REF_DIE)
	{
		if(m_lCurRefDieNo == 1)
		{
			m_szDieTypeInfo = "Wafer Map Reference#1";
		}
		else if(m_lCurRefDieNo == 2)
		{
			m_szDieTypeInfo = "Wafer Map Reference#2";
		}
		else if(m_lCurRefDieNo == 3)
		{
			m_szDieTypeInfo = "no use";
		}
		else if(m_lCurRefDieNo == 4)
		{
			m_szDieTypeInfo = "Collet Hole with Epoxy Pattern";
		}
		else if(m_lCurRefDieNo == 5)
		{
			m_szDieTypeInfo = "Ejector Pin";
		}
		else if(m_lCurRefDieNo == 6)
		{
			m_szDieTypeInfo = "BH2 Mark";
		}
		else if(m_lCurRefDieNo == 7)
		{
			m_szDieTypeInfo = "BH1 Collet Hole With Circle";
		}
		else if(m_lCurRefDieNo == 8)
		{
			m_szDieTypeInfo = "BH2 Collet Hole With Circle";
		}
		else if(m_lCurRefDieNo == 9)
		{
			m_szDieTypeInfo = "NG grade PR";
		}
		else if(m_lCurRefDieNo == 10)
		{
			m_szDieTypeInfo = "Ej cap hole";
		}
		else
		{
			m_szDieTypeInfo = "No die type";
		}
	}

	LONG lDieNo = GetWprDieNo();
	LONG lPrID = m_ssGenPRSrchID[lDieNo];
	szTemp.Format("  PRID=%d",lPrID);
	m_szDieTypeInfo += szTemp;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::DisplayPRIdListRecord(IPC_CServiceMessage &svMsg)
{
	PR_COORD				stCorner1;
	PR_COORD				stCorner2;
	LONG					lDieNo;
	svMsg.GetMsg(sizeof(LONG), &lDieNo);
	LONG lPrID = m_ssGenPRSrchID[lDieNo];
	
	PR_UBYTE ubSID = GetRunSenID();	
	PR_UBYTE ubRID = GetRunRecID();
	PR_UWORD uwPRStatus = m_pPrGeneral->DisplayLearnInfo(lPrID, GetPrCenterX(), GetPrCenterY(), ubSID, ubRID);
	if (PRS_PRNoErr(uwPRStatus))
	{
		stCorner1.x	= PR_MIN_COORD;
		stCorner1.y = PR_MIN_COORD;
		stCorner2.x = PR_MAX_COORD;
		stCorner2.y = PR_MAX_COORD;

		DrawRectangleBox(stCorner1, stCorner2, PR_COLOR_YELLOW);
		HmiMessage("Confirm Previous Learnt PR", "System Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL, 500, 300);
	}
	else
	{
		HmiMessage("PRS Show Info Error", "System Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}

	ChangeCamera(m_bCurrentCamera);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::RecoverBackupLighting(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG lGroupID;
		LONG lCoaxLevel;
		LONG lRingLevel;
		LONG lSideLevel;
		LONG lBackLevel;
	} SETLIGHT;
	SETLIGHT stInfo;
	IPC_CServiceMessage stMsg;

	stInfo.lGroupID = 0;
	stInfo.lCoaxLevel = m_lWPRGenCoaxLightLevel_Backup;
	stInfo.lSideLevel = m_lWPRGenSideLightLevel_Backup;
	stInfo.lRingLevel = m_lWPRGenRingLightLevel_Backup;
	stInfo.lBackLevel = m_lWPRGenBackLightLevel_Backup;

	stMsg.InitMessage(sizeof(SETLIGHT), &stInfo);
	SetGeneralLighting2(stMsg);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::UserSearchLookForwardDie(IPC_CServiceMessage &svMsg)
{
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate, fDieScore;
	PR_UWORD	usDieType	= PR_ERR_NO_OBJECT_FOUND;
	BOOL		bReturn		= TRUE;

	if (PR_NotInit())
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsNormalDieLearnt() == FALSE)
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bDieCalibrated == FALSE)
	{
		SetAlert(IDS_WPR_DIENOTCALIBRATED);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CStringList szList;
    szList.AddTail("Left Next Next Die");
    szList.AddTail("Left Next Die");
	szList.AddTail("Right Next Die");
    szList.AddTail("Right Next Next Die");
	CString szContent = "Please select Die Position";
    CString szTitle =  "Manaul Search LF Die";
	LONG lSet = 0;
	LONG lSelection = HmiSelection(szContent, szTitle, szList, lSet);

	PR_WIN stLFArea = GetSrchArea();

	if (lSelection == 0)
	{
		GetLFSearchArea_5x5(stLFArea, 11);
	}
	else if (lSelection == 1)
	{
		GetLFSearchArea_5x5(stLFArea, 12);
	}
	else if (lSelection == 2)
	{
		GetLFSearchArea_5x5(stLFArea, 14);
	}
	else if (lSelection == 3)
	{
		GetLFSearchArea_5x5(stLFArea, 15);
	}
	else
	{
		HmiMessage("No Selection!", "Error");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	LONG lDieNo = WPR_NORMAL_DIE;

	//Change the zoom mode before searching die
	LONG lOldZoom = GetRunZoom();
	ChangePrRecordZoomMode(lDieNo);

	//UpdateSearchDieArea(lDieNo, GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE, FALSE);

	DrawSearchBox(PR_COLOR_GREEN);
	DrawRectangleBox(stLFArea.coCorner1, stLFArea.coCorner2, PR_COLOR_GREEN);

	PR_WORD rtnPR = ManualSearchDie(WPR_NORMAL_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stLFArea.coCorner1, stLFArea.coCorner2, TRUE);

	if (rtnPR != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			bReturn = TRUE;
			
			//Allow die to rotate
			ManualDieCompenate(stDieOffset, fDieRotate);
			DrawSearchBox(PR_COLOR_TRANSPARENT);
		}
		else
		{
			bReturn = TRUE;
		}

		//Display Message on HMI
		DisplaySearchDieResult(usDieType, WPR_NORMAL_DIE, WPR_NORMAL_DIE, fDieRotate, stDieOffset, fDieScore);
	}
	else
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		bReturn = FALSE;
	}

	//On Joystick & Clear PR screen & Change to the original zoom mode
	SetWaferTableJoystick(TRUE);
	ChangeCamera(WPR_CAM_WAFER);
	LiveViewZoom(lOldZoom, FALSE, FALSE, 19);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::ManualFailureCaseLog(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	PR_UWORD usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD stDieOffset; 
	PR_REAL	 fDieRotate; 
	PR_REAL	 fDieScore;

	if (PR_NotInit())
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsNormalDieLearnt() == FALSE)
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bDieCalibrated == FALSE)
	{
		SetAlert(IDS_WPR_DIENOTCALIBRATED);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	LONG lDieNo = WPR_NORMAL_DIE;

	//Change the zoom mode before searching die
	LONG lOldZoom = GetRunZoom();
	ChangePrRecordZoomMode(lDieNo);

	UpdateSearchDieArea(lDieNo, GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE, FALSE);

	DrawSearchBox(PR_COLOR_GREEN);
	PR_COORD stCorner1, stCorner2;
	stCorner1.x = (PR_WORD) GetPrCenterX() - GetNmlSizePixelX()/2;
	stCorner1.y = (PR_WORD) GetPrCenterY() - GetNmlSizePixelY()/2;
	stCorner2.x = (PR_WORD) GetPrCenterX() + GetNmlSizePixelX()/2;
	stCorner2.y = (PR_WORD) GetPrCenterY() + GetNmlSizePixelY()/2;
	DrawRectangleBox(stCorner1, stCorner2, PR_COLOR_GREEN);
	PR_WORD rtnPR = ManualSearchDie(WPR_NORMAL_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stCorner1, stCorner1, TRUE);

	WaferPrLogFailureCaseImage();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}