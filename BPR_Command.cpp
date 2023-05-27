/////////////////////////////////////////////////////////////////
// BPR_Command.cpp : HMI Registered Command of the CBondPr class
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
#include "BondPr.h"
#include "BPR_Constant.h"
#include "HmiDataManager.h"
#include "MS_SecCommConstant.h"
#include "CTmpChange.h"
#include "BinLoader.h"
#include "WaferPr.h"
#include "NGGrade.h"
#include "BondHead.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Registered HMI Command 
VOID CBondPr::RegisterVariables()
{
	int i;
	CString szText;

	try
	{
		// Register Commands
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateOutput"),			&CBondPr::UpdateOutput);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetGeneralLighting"),	&CBondPr::SetGeneralLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetGeneralLighting"),	&CBondPr::GetGeneralLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RestoreGeneralLighting"), &CBondPr::RestoreGeneralLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetExposureTime"),		&CBondPr::SetExposureTime);		//ching
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetGExposureTime"),		&CBondPr::SetGExposureTime);		//ching
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateHMIData"),		&CBondPr::UpdateHMIData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),			&CBondPr::UpdateAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogItems"),				&CBondPr::LogItems);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelLearnDie"),		&CBondPr::CancelLearnDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmLearnDie"),		&CBondPr::ConfirmLearnDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSearchDiePara"), &CBondPr::ConfirmSearchDiePara);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSearchDieArea"), &CBondPr::ConfirmSearchDieArea);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchDie"),		&CBondPr::UserSearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchDie_NoMove"),	&CBondPr::UserSearchDie_NoMove);//Matt: 
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_MultiSearchDie"),	&CBondPr::BT_MultiSearchDie); //4.51D20
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_MultiSearchInit"),	&CBondPr::BT_MultiSearchInit); //4.51D20
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_MultiSearchFirstDie"),&CBondPr::BT_MultiSearchFirstDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_MultiSearchFirstDie1"),&CBondPr::BT_MultiSearchFirstDie1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchDie"),			&CBondPr::BT_SearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchRefDie2"),		&CBondPr::BT_SearchRefDie2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_LargeAreaSearchDie"),		&CBondPr::BT_LargeAreaSearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT2_SearchDie"),		&CBondPr::BT2_SearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchDieWithAngle"),&CBondPr::BT_SearchDieWithAngle);	//v4.11T3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchFirstDie"),	&CBondPr::BT_SearchFirstDie);
 		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT2_SearchFirstDie"),	&CBondPr::BT2_SearchFirstDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_GetPRStatus"),		&CBondPr::BT_GetPRStatus);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_GetFOVSize"),		&CBondPr::BT_GetFOVSize);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchDieInFOV"),	&CBondPr::BT_SearchDieInFOV);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchDieXYInFOV"),	&CBondPr::BT_SearchDieXYInFOV);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT2_SearchDieXYInFOV"),	&CBondPr::BT2_SearchDieXYInFOV);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchDieInMaxFOV"),	&CBondPr::BT_SearchDieInMaxFOV);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_GetDieSize"),		&CBondPr::BT_GetDieSize);			//v2.67
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HotKeySearchRefDie"),	&CBondPr::HotKeySearchRefDie);	//v3.80
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchPreBondPattern"),	&CBondPr::SearchPreBondPattern);	//v3.80
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LookForwardPreBondPattern"), &CBondPr::LookForwardPreBondPattern);	//CSP
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetPrCenterXY"),		&CBondPr::ResetPrCenterXY);		//v3.80

		//m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetLearnDieLighting"), GetLearnDieLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetLearnDieLighting"),	&CBondPr::SetLearnDieLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisableLightingSetting"),&CBondPr::DisableLightingSetting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovePRCursor"),			&CBondPr::MovePRCursor);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLearnDie"),			&CBondPr::AutoLearnDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoDetectDie"),		&CBondPr::AutoDetectDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BprAutoColorAdjust"),	&CBondPr::BprAutoColorAdjust);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetInspDetectRegion"),	&CBondPr::SetInspDetectRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EndInspDetectRegion"),	&CBondPr::EndInspDetectRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetInspIgnoreRegion"),	&CBondPr::SetInspIgnoreRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EndInspIgnoreRegion"),	&CBondPr::EndInspIgnoreRegion);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LearnDieCalibration"),	&CBondPr::LearnDieCalibration);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LearnDieCalibration2"),	&CBondPr::LearnDieCalibration2);	//BT2 for MS109
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectRunTimeBinBlk"),	&CBondPr::SelectRunTimeBinBlk);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectOffLineBinBlk"),	&CBondPr::SelectOffLineBinBlk);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartOfflinePostbond"), &CBondPr::StartOfflinePostbond);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualCompensate"),		&CBondPr::ManualCompensate);		//v3.50T1

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeleteRuntimeData"),	&CBondPr::DeleteRuntimeData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeleteOfflineData"),	&CBondPr::DeleteOfflineData);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeleteRuntimeAllData"), &CBondPr::DeleteRuntimeAllData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeleteOfflineAllData"), &CBondPr::DeleteOfflineAllData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplaySPCMessage"),	&CBondPr::DisplaySPCMessage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CopyAllSPCValue"),		&CBondPr::CopyAllSPCValue);


		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UsePRMouse"),			&CBondPr::UsePRMouse);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DrawComplete"),			&CBondPr::DrawComplete);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelMouseDraw"),		&CBondPr::CancelMouseDraw);

		// Upload and download PR Record
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UploadPrRecords"),		&CBondPr::UploadPrRecords);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DownloadPrRecords"),	&CBondPr::DownloadPrRecords);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeletePrRecords"),		&CBondPr::DeletePrRecords);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeletePRRecord"),		&CBondPr::DeletePRRecord);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayPRRecord"),		&CBondPr::DisplayPRRecord);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayPRIdListRecord"),&CBondPr::DisplayPRIdListRecord);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PkgKeyParametersTask"),	&CBondPr::PkgKeyParametersTask);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckPostBondSetup"),	&CBondPr::CheckPostBondSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckRecordID"),		&CBondPr::CheckRecordID);

		// Generate SPC data
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveSPCData"),			&CBondPr::SaveSPCData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveSPCDisplayData"),	&CBondPr::SaveSPCDisplayData);	//v4.11

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DrawSearchWindow"),		&CBondPr::DrawSearchWindow);		//v2.56
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DrawDieSizeWindow"),	&CBondPr::DrawDieSizeWindow);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnSelectShowPrImage"),		&CBondPr::OnSelectShowPrImage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PreOnSelectShowPrImage"),	&CBondPr::PreOnSelectShowPrImage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayPRRecordInSummary"),	&CBondPr::DisplayPRRecordInSummary);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachBHZ1BondPos"),			&CBondPr::TeachBHZ1BondPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBHZ1BondPos"),		&CBondPr::ConfirmBHZ1BondPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelBHZ1BondPos"),		&CBondPr::CancelBHZ1BondPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBHZ1BondPos"),			&CBondPr::ResetBHZ1BondPos);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachBHZ2BondPos"),			&CBondPr::TeachBHZ2BondPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBHZ2BondPos"),		&CBondPr::ConfirmBHZ2BondPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelBHZ2BondPos"),		&CBondPr::CancelBHZ2BondPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBHZ2BondPos"),			&CBondPr::ResetBHZ2BondPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AdjustCrossHair"),			&CBondPr::AdjustCrossHair);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AdjustCrossHairStepByStep"), &CBondPr::AdjustCrossHairStepByStep);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableBondPrInspction"),	&CBondPr::EnableBondPrInspction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TransformBHRTCompXY"),		&CBondPr::TransformBHRTCompXY);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelBHZ1OrBHZ2BondPos"),	&CBondPr::CancelBHZ1OrBHZ2BondPos);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SavePrDataCmd"),			&CBondPr::SavePrDataCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchRefDieInFOV"),		&CBondPr::BT_SearchRefDieInFOV);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchRefDie2InFOV"),	&CBondPr::BT_SearchRefDie2InFOV);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SearchRefDieErrMap"),	&CBondPr::BT_SearchRefDieErrMap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BPR_CheckReferenceDie"),	&CBondPr::CheckReferenceDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchColletHole"),			&CBondPr::SearchColletHole);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SearchUploadColletPR"),	&CBondPr::AGC_SearchUploadColletPR);		//v4.50A13
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SearchColletHole"),		&CBondPr::AGC_SearchColletHole);		//v4.50A13
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SearchColletInTray"),	&CBondPr::AGC_SearchColletInTray);	//v4.53A12		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_UsePRMouse"),			&CBondPr::AGC_UsePRMouse);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_DrawRectComplete"),		&CBondPr::AGC_DrawRectComplete);		//v4.50A29
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_ResetColletCentre"),	&CBondPr::AGC_ResetColletCentre);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BPR_BondLeftDie"),			&CBondPr::BPR_BondLeftDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetCleanProbeLighting"),	&CBondPr::SetCleanProbeLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveTheCleanProbeLighting"),&CBondPr::SaveTheCleanProbeLighting);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenerateConfigData"),       &CBondPr::GenerateConfigData);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PRIDDetailInfo"),			&CBondPr::PRIDDetailInfo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSearchNGDiePara"),	&CBondPr::ConfirmSearchNGDiePara);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowSearchDiePara"),		&CBondPr::ShowSearchDiePara);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearAccEmptyCounter"),		&CBondPr::ClearAccEmptyCounter);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleLearnBondPosnZoomScreenCmd"),		&CBondPr::ToggleLearnBondPosnZoomScreenCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExtractObjectCmd"),			&CBondPr::ExtractObjectCmd);

		DisplayMessage("BondPRStn Service Registered...");

		// Register Variables
		//Learn die variable
		RegVariable(_T("BPR_szLrnDieType"),				&m_szLrnDieType);
		RegVariable(_T("BPR_bSelectDieType"),			&m_bSelectDieType);
		RegVariable(_T("BPR_lCurRefDieNo"),				&m_lCurRefDieNo);	
		RegVariable(_T("BPR_lLrnAlignAlgo"),			&m_lLrnAlignAlgo);	
		RegVariable(_T("BPR_lLrnBackground"),			&m_lLrnBackground);
		RegVariable(_T("BPR_lLrnFineSrch"),				&m_lLrnFineSrch);	
		RegVariable(_T("BPR_lLrnInspMethod"),			&m_lLrnInspMethod);	
		RegVariable(_T("BPR_lLrnInspRes"),				&m_lLrnInspRes);	
		RegVariable(_T("BPR_lExposureTimeLevel"),		&m_lBPRExposureTimeLevel);	//ching
		RegVariable(_T("BPR_lGExposureTimeLevel"),		&m_lBPRGExposureTimeLevel);	//ching
		RegVariable(_T("BPR_bLrnAdvanceOption"),		&m_bLrnAdvanceOption);

		//Die shape
		RegVariable(_T("BPR_ucDieShape"),				&m_ucDieShape);
		RegVariable(_T("BPR_dFovSize"),					&m_dFovSize);

		//Search Die variable
		RegVariable(_T("BPR_lSrchDieScore"),			&m_lSrchDieScore);	
		RegVariable(_T("BPR_lSrchDefectThres"),			&m_lSrchDefectThres);	
		RegVariable(_T("BPR_lSrchDieAreaX"),			&m_lSrchDieAreaX);	
		RegVariable(_T("BPR_lSrchDieAreaY"),			&m_lSrchDieAreaY);	
		RegVariable(_T("BPR_lSrchGreyLevelDefect"),		&m_lSrchGreyLevelDefect);	
		RegVariable(_T("BPR_dSrchSingleDefectArea"),	&m_dSrchSingleDefectArea);
		RegVariable(_T("BPR_dSrchTotalDefectArea"),		&m_dSrchTotalDefectArea);
		RegVariable(_T("BPR_dSrchChipArea"),			&m_dSrchChipArea);
		RegVariable(_T("BPR_bSrchEnableDefectCheck"),	&m_bSrchEnableDefectCheck);	
		RegVariable(_T("BPR_bSrchEnableChipCheck"),		&m_bSrchEnableChipCheck);		
		RegVariable(_T("BPR_dSrchDieAreaX"),			&m_dSrchDieAreaX);		//v2.60
		RegVariable(_T("BPR_dSrchDieAreaY"),			&m_dSrchDieAreaY);		//v2.60
		RegVariable(_T("BPR_lPrCal1stStepSize"),		&m_lPrCal1stStepSize);	//v4.57A2

		RegVariable(_T("BPR_bMoveEjrZAndPostBond"),		&m_bMoveEjrZAndPostBond);

		//v3.25T2
		RegVariable(_T("BPR_bEnableProberBondPR"),		&m_bEnableProberBondPR);
		RegVariable(_T("BPR_bEnableTotalDefect"),		&m_bEnableTotalDefect);
		RegVariable(_T("BPR_bEnableLineDefect"),		&m_bEnableLineDefect);
		RegVariable(_T("BPR_dMinLineDefectLength"),		&m_dMinLineDefectLength);
		RegVariable(_T("BPR_lMinNumOfLineDefects"),		&m_lMinNumOfLineDefects);
		RegVariable(_T("BPR_lMaxNumOfDefects"),			&m_lMaxNumOfDefects);
		RegVariable(_T("BPR_bDisableLightingSetting"),	&m_bDisableLightingSetting);
		//Show information into HMI
		RegVariable(_T("BPR_bUseHWTrigger"),			&m_bUseHWTrigger);				//andrewng //2020-0615
		RegVariable(_T("BPR_lPrCenterX"),				&m_lPrCenterX);
		RegVariable(_T("BPR_lPrCenterY"),				&m_lPrCenterY);
		RegVariable(_T("BPR_lPrCenterOffsetXInPixel"),	&m_lPrCenterOffsetXInPixel);
		RegVariable(_T("BPR_lPrCenterOffsetYInPixel"),	&m_lPrCenterOffsetYInPixel);
		RegVariable(_T("BPR_bDieLearnt"),				&m_bDieLearnt);	
		RegVariable(_T("BPR_bDieCalibrated"),			&m_bDieCalibrated);	
		RegVariable(_T("BPR_bDieCalibrated2"),			&m_bDieCalibrated2);	
		RegVariable(_T("BPR_lCurNormDieID"),			&m_lCurNormDieID);						
		RegVariable(_T("BPR_lCurNormDieSizeX"),			&m_lCurNormDieSizeX);			
		RegVariable(_T("BPR_lCurNormDieSizeY"),			&m_lCurNormDieSizeY);				
		RegVariable(_T("BPR_dCurNormDieSizeX"),			&m_dCurNormDieSizeX);			
		RegVariable(_T("BPR_dCurNormDieSizeY"),			&m_dCurNormDieSizeY);				

		RegVariable(_T("BPR_bHMIReferenceDie"),			&m_bHMIReferenceDie);

		//Other variable
		for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
		{
			szText.Format("BPR_lGenCoaxLightLevel%d", i);
			RegVariable(_T(szText), &m_lBPRGenCoaxLightLevel[i]);	

			szText.Format("BPR_lGenRingLightLevel%d", i);
			RegVariable(_T(szText), &m_lBPRGenRingLightLevel[i]);	

			szText.Format("BPR_lGenSideLightLevel%d", i);
			RegVariable(_T(szText), &m_lBPRGenSideLightLevel[i]);	
		}

		//v4.50A15
		RegVariable(_T("BPR_lLrnCoaxLightLevel0"),	&m_lBPRLrnCoaxLightHmi);
		RegVariable(_T("BPR_lLrnRingLightLevel0"),	&m_lBPRLrnRingLightHmi);
		RegVariable(_T("BPR_lLrnSideLightLevel0"),	&m_lBPRLrnSideLightHmi);
		//v4.50A15
		for (i=1; i<BPR_MAX_LIGHT_GROUP; i++)	//Change i from 0 to 1; o uses Hmi vars above
		{
			szText.Format("BPR_lLrnCoaxLightLevel%d", i);
			RegVariable(_T(szText), &m_lBPRLrnCoaxLightLevel[i]);	

			szText.Format("BPR_lLrnRingLightLevel%d", i);
			RegVariable(_T(szText), &m_lBPRLrnRingLightLevel[i]);	

			szText.Format("BPR_lLrnSideLightLevel%d", i);
			RegVariable(_T(szText), &m_lBPRLrnSideLightLevel[i]);	
		}

		//For NG Grade 
		CNGGrade *pNGGrade = CNGGrade::Instance();
		RegVariable(_T("BPR_bHaveNGGrade"),			&pNGGrade->m_bHaveNGGrade);	
		RegVariable(_T("BPR_bNGGrade148"),			&pNGGrade->m_bNGGrade148);	
		RegVariable(_T("BPR_bNGGrade149"),			&pNGGrade->m_bNGGrade149);	
		RegVariable(_T("BPR_bNGGrade150"),			&pNGGrade->m_bNGGrade150);	
		RegVariable(_T("BPR_bNGGradeOther"),		&pNGGrade->m_bNGGradeOther);	
		RegVariable(_T("BPR_szNGGradeOtherList"),	&pNGGrade->m_szNGGradeOtherList);	
		RegVariable(_T("BPR_lSrchNGDieScore"),		&pNGGrade->m_lSrchNGDieScore);
		
		//Postbond variable
		RegVariable(_T("BPR_bAverageAngleCheck"),	&m_bAverageAngleCheck);	
		RegVariable(_T("BPR_bBinTableAngleCheck"),	&m_bBinTableAngleCheck);	
		RegVariable(_T("BPR_bPlacementCheck"),		&m_bPlacementCheck);	
		RegVariable(_T("BPR_bRelOffsetCheck"),		&m_bRelOffsetCheck);		//v4.26T1	//Semitek
		RegVariable(_T("BPR_bDefectCheck"),			&m_bDefectCheck);	
		RegVariable(_T("BPR_bEmptyCheck"),			&m_bEmptyCheck);			//v3.02T1	
		RegVariable(_T("BPR_dAverageAngle"),		&m_dAverageAngle);	
		RegVariable(_T("BPR_dMaxAngle"),			&m_dMaxAngle);	
		RegVariable(_T("BPR_dDieShiftX"),			&m_dDieShiftX);	
		RegVariable(_T("BPR_dDieShiftY"),			&m_dDieShiftY);	
		RegVariable(_T("BPR_lMaxAngleAllow"),		&m_lMaxAngleAllow);	
		RegVariable(_T("BPR_lMaxAccAngleAllow"),	&m_lMaxAccAngleAllow);		//v4.58A5
		RegVariable(_T("BPR_lMaxDefectAllow"),		&m_lMaxDefectAllow);	
		RegVariable(_T("BPR_lMaxEmptyAllow"),		&m_lMaxEmptyAllow);			//v3.02T1
		RegVariable(_T("BPR_lMaxAccEmptyAllow"),	&m_lMaxAccEmptyAllow);		//v4.40T9
		RegVariable(_T("BPR_lMaxShiftAllow"),		&m_lMaxShiftAllow);	
		RegVariable(_T("BPR_lMaxRelShiftAllow"),	&m_lMaxRelShiftAllow);		//v4.26T1	//Semitek
		//v4.51A17	//SanAn TJ
		RegVariable(_T("BPR_dMinCpXAllow"),			&m_dMinCpXAllow);
		RegVariable(_T("BPR_dMinCpYAllow"),			&m_dMinCpYAllow);
		RegVariable(_T("BPR_dMinCpTAllow"),			&m_dMinCpTAllow);
		RegVariable(_T("BPR_dSpcCpX"),				&m_dSpcCpX);
		RegVariable(_T("BPR_dSpcCpY"),				&m_dSpcCpY);
		RegVariable(_T("BPR_dSpcCpT"),				&m_dSpcCpT);
		RegVariable(_T("BPR_dSpcCpkX"),				&m_dSpcCpkX);
		RegVariable(_T("BPR_dSpcCpkY"),				&m_dSpcCpkY);
		RegVariable(_T("BPR_dSpcCpkT"),				&m_dSpcCpkT);
		RegVariable(_T("BPR_bUsePostBond"),			&m_bUsePostBond);	
		RegVariable(_T("BPR_bPostBondAtBond"),		&m_bPostBondAtBond);		//v2.60
		RegVariable(_T("BPR_szPostbondDataX"),		&m_szPostbondDataX);	
		RegVariable(_T("BPR_szPostbondDataY"),		&m_szPostbondDataY);	
		RegVariable(_T("BPR_szPostbondDataTheta"),	&m_szPostbondDataTheta);	
		RegVariable(_T("BPR_szPostbondDispX"),		&m_szPostbondDispX);	
		RegVariable(_T("BPR_szPostbondDispY"),		&m_szPostbondDispY);	
		RegVariable(_T("BPR_szPostbondDispTheta"),	&m_szPostbondDispTheta);	
		RegVariable(_T("BPR_bRTCompXY"),			&m_bEnableRTCompXY);		//v3.86

		/*** Arm1 ***/
		RegVariable(_T("BPR_szPostbondDataX_Arm1"),		&m_szPostbondDataX_Arm1);	
		RegVariable(_T("BPR_szPostbondDataY_Arm1"),		&m_szPostbondDataY_Arm1);	
		RegVariable(_T("BPR_szPostbondDataTheta_Arm1"),	&m_szPostbondDataTheta_Arm1);	
		RegVariable(_T("BPR_szPostbondDispX_Arm1"),		&m_szPostbondDispX_Arm1);	
		RegVariable(_T("BPR_szPostbondDispY_Arm1"),		&m_szPostbondDispY_Arm1);	
		RegVariable(_T("BPR_szPostbondDispTheta_Arm1"),	&m_szPostbondDispTheta_Arm1);

		/*** Arm2 ***/
		RegVariable(_T("BPR_szPostbondDataX_Arm2"),		&m_szPostbondDataX_Arm2);	
		RegVariable(_T("BPR_szPostbondDataY_Arm2"),		&m_szPostbondDataY_Arm2);	
		RegVariable(_T("BPR_szPostbondDataTheta_Arm2"),	&m_szPostbondDataTheta_Arm2);	
		RegVariable(_T("BPR_szPostbondDispX_Arm2"),		&m_szPostbondDispX_Arm2);	
		RegVariable(_T("BPR_szPostbondDispY_Arm2"),		&m_szPostbondDispY_Arm2);	
		RegVariable(_T("BPR_szPostbondDispTheta_Arm2"),	&m_szPostbondDispTheta_Arm2);

		RegVariable(_T("BPR_szDieTypeInfo"),			&m_szDieTypeInfo);

		RegVariable(_T("BPR_szSPCCmdDspDieX"),			&m_szSPCCmdDspDieX);		//v4.40T7
		RegVariable(_T("BPR_szSPCCmdDspDieY"),			&m_szSPCCmdDspDieY);		//v4.40T7
		RegVariable(_T("BPR_szSPCCmdDspDieT"),			&m_szSPCCmdDspDieT);		//v4.51A17
		RegVariable(_T("BPR_szSPCCmdDspDieX_Arm1"),		&m_szSPCCmdDspDieX_Arm1);	//v4.40T7
		RegVariable(_T("BPR_szSPCCmdDspDieY_Arm1"),		&m_szSPCCmdDspDieY_Arm1);	//v4.40T7
		RegVariable(_T("BPR_szSPCCmdDspDieX_Arm2"),		&m_szSPCCmdDspDieX_Arm2);	//v4.40T7
		RegVariable(_T("BPR_szSPCCmdDspDieY_Arm2"),		&m_szSPCCmdDspDieY_Arm2);	//v4.40T7
		//v4.50A30
		RegVariable(_T("BPR_lAGCC1CenterXInPixel"),		&m_lAGCC1CenterXInPixel);
		RegVariable(_T("BPR_lAGCC1CenterYInPixel"),		&m_lAGCC1CenterYInPixel);
		RegVariable(_T("BPR_lAGCC2CenterXInPixel"),		&m_lAGCC2CenterXInPixel);
		RegVariable(_T("BPR_lAGCC2CenterYInPixel"),		&m_lAGCC2CenterYInPixel);
		//v4.59A16	//Renesas MS90
		RegVariable(_T("BPR_szSpcYDisplayTitle"),		&m_szSpcYDisplayTitle);
		// for general use
		RegVariable(_T("BPR_szGenShowImagePath"),		&m_szGenShowImagePath);
		// for Huga summary
		RegVariable(_T("BPR_szShowImagePath"),			&m_szShowImagePath);
		RegVariable(_T("BPR_szShowImageType"),			&m_szShowImageType);
		RegVariable(_T("BPR_lShowImageNo"),				&m_lShowImageNo);

		RegVariable(_T("BPR_lCleanProbeLightLevel"),	&m_lCleanProbeLightLevel);
		//v4.59A1
		RegVariable(_T("BPR_bPreBondAlignUseEpoxySearch"),	&m_bPreBondAlignUseEpoxySearch);
		RegVariable(_T("BPR_bCheckFrameAngleByHwMark"),	&m_bCheckFrameAngleByHwMark);

		//Calibration
		RegVariable(_T("BPR_dCalibX"),	&m_dCalibX);
		RegVariable(_T("BPR_dCalibXY"), &m_dCalibXY);
		RegVariable(_T("BPR_dCalibY"),	&m_dCalibY);
		RegVariable(_T("BPR_dCalibYX"), &m_dCalibYX);

		RegVariable(_T("BPR_lCurrentBinBlock"),		&m_lCurrentBinBlock);

		RegVariable(_T("BPR_szBprZoomFactor"),		&m_szBprZoomFactor);
		RegVariable(_T("BPR_lBprZoomDigital"),		&m_lBprZoomDigital);
		RegVariable(_T("BPR_bBprContinuousZoom"),	&m_bBprContinuousZoom);
		RegVariable(_T("BPR_lBprContZoomFactor"),	&m_lBprContZoomFactor);
		RegVariable(_T("BPR_lBprZoomRoiShrink"),	&m_lBprZoomRoiShrink);

		DisplayMessage("BondPRStn variables Registered...");
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

LONG CBondPr::UpdateOutput(IPC_CServiceMessage& svMsg)
{
	BOOL bUpdate = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUpdate);

	m_bUpdateOutput = bUpdate;

	bUpdate = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bUpdate);
	return 1;
}

LONG CBondPr::GetGeneralLighting(IPC_CServiceMessage& svMsg)
{
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	if (lInitPR != 1)
	{
		return -1;
	}

	if (m_bDisableBT)
	{
		return -1;
	}


	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_UWORD		usBPRCoaxLightLevel;
	PR_UWORD		usBPRRingLightLevel;
	PR_UWORD		usBPRSideLightLevel;
	PR_SOURCE		emCoaxID, emRingID, emSideID;
	int				i;


	if ( CMS896AStn::m_bEnablePRDualColor == TRUE )
	{
		PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
		{
			GetLightingConfig(i, emCoaxID, emRingID, emSideID);
			
			PR_GetLighting(&stOptic, emCoaxID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usBPRCoaxLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emRingID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usBPRRingLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emSideID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usBPRSideLightLevel, &stRpy);

			m_lBPRTmpCoaxLightLevel[i] = m_lBPRGenCoaxLightLevel[i] = usBPRCoaxLightLevel;
			m_lBPRTmpRingLightLevel[i] = m_lBPRGenRingLightLevel[i] = usBPRRingLightLevel;
			m_lBPRTmpSideLightLevel[i] = m_lBPRGenSideLightLevel[i] = usBPRSideLightLevel;
		}
	}
	else
	{
		PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

		GetLightingConfig(0, emCoaxID, emRingID, emSideID);
		PR_GetLighting(&stOptic, emCoaxID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usBPRCoaxLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emRingID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usBPRRingLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emSideID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usBPRSideLightLevel, &stRpy);
//4.53D19 log
//CString szMsg;
//szMsg.Format("Get General Light:(%ld,%ld,%ld)" , usBPRCoaxLightLevel,usBPRRingLightLevel,usBPRSideLightLevel);
//AfxMessageBox(szMsg ,MB_SYSTEMMODAL);

		for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
		{
			m_lBPRTmpCoaxLightLevel[i] = m_lBPRGenCoaxLightLevel[i] = usBPRCoaxLightLevel;
			m_lBPRTmpRingLightLevel[i] = m_lBPRGenRingLightLevel[i] = usBPRRingLightLevel;
			m_lBPRTmpSideLightLevel[i] = m_lBPRGenSideLightLevel[i] = usBPRSideLightLevel;
		}
	}

	return 1;
}


LONG CBondPr::RestoreGeneralLighting(IPC_CServiceMessage& svMsg)
{
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	if (lInitPR != 1)
	{
		return -1;
	}

	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_SOURCE		emCoaxID, emRingID, emSideID;
	int				i;


	//v2.96T5
	//for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
	//{
	//	m_lBPRGenCoaxLightLevel[i] = m_lBPRTmpCoaxLightLevel[i];
	//	m_lBPRGenRingLightLevel[i] = m_lBPRTmpRingLightLevel[i];
	//	m_lBPRGenSideLightLevel[i] = m_lBPRTmpSideLightLevel[i];
	//}


	if ( CMS896AStn::m_bEnablePRDualColor == TRUE )
	{
		PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
		{
			GetLightingConfig(i, emCoaxID, emRingID, emSideID);

			PR_SetLighting(emCoaxID, (PR_UWORD)m_lBPRGenCoaxLightLevel[i], MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
			PR_SetLighting(emRingID, (PR_UWORD)m_lBPRGenRingLightLevel[i], MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
			PR_SetLighting(emSideID, (PR_UWORD)m_lBPRGenSideLightLevel[i], MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		}
	}
	else
	{
		PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		GetLightingConfig(0, emCoaxID, emRingID, emSideID);
		PR_SetLighting(emCoaxID, (PR_UWORD)m_lBPRGenCoaxLightLevel[0], MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(emRingID, (PR_UWORD)m_lBPRGenRingLightLevel[0], MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(emSideID, (PR_UWORD)m_lBPRGenSideLightLevel[0], MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
	}

	return 1;
}

LONG CBondPr::SaveTheCleanProbeLighting(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	m_lCleanProbeLightLevel = m_lBPRGenRingLightLevel[m_lBPRGExposureTimeLevel];

	if ( m_lCleanProbeLightLevel < 0 )
	{
		m_lCleanProbeLightLevel = 0;
	}
	if ( m_lCleanProbeLightLevel > 100 )
	{
		m_lCleanProbeLightLevel = 100;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SetCleanProbeLighting(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	LONG lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_SOURCE		emCoaxID, emRingID, emSideID;

	LONG lGroupID = 0;
	svMsg.GetMsg(sizeof(LONG), &lGroupID);

	if ( lGroupID >= BPR_MAX_LIGHT_GROUP )
	{
		lGroupID = 0;
	}

	GetLightingConfig(lGroupID, emCoaxID, emRingID, emSideID);

	m_lBPRGenRingLightLevel[lGroupID] = m_lCleanProbeLightLevel;

	if ( lInitPR != 1 )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
	PR_SetLighting(emRingID, (PR_UWORD)m_lCleanProbeLightLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SetGeneralLighting(IPC_CServiceMessage& svMsg)
{
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	BOOL	bReturn = TRUE;


	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_SOURCE		emCoaxID, emRingID, emSideID;

	if (m_bFreezeCamera_HMI)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	typedef struct {
		LONG lGroupID;
		LONG lCoaxLevel;
		LONG lRingLevel;
		LONG lSideLevel;
	} SETLIGHT;

	SETLIGHT stInfo;
	svMsg.GetMsg(sizeof(SETLIGHT), &stInfo);

	// Saturate the levels
	if ( stInfo.lCoaxLevel > 100 )
		stInfo.lCoaxLevel = 100;
	if ( stInfo.lCoaxLevel < 0 )
		stInfo.lCoaxLevel = 0;
	if ( stInfo.lRingLevel > 100 )
		stInfo.lRingLevel = 100;
	if ( stInfo.lRingLevel < 0 )
		stInfo.lRingLevel = 0;
	if ( stInfo.lSideLevel > 100 )
		stInfo.lSideLevel = 100;
	if ( stInfo.lSideLevel < 0 )
		stInfo.lSideLevel = 0;

	if ( stInfo.lGroupID >= BPR_MAX_LIGHT_GROUP )
	{
		stInfo.lGroupID = 0;
	}

	GetLightingConfig(stInfo.lGroupID, emCoaxID, emRingID, emSideID);

	m_lBPRGenCoaxLightLevel[stInfo.lGroupID] = stInfo.lCoaxLevel;	
	m_lBPRGenRingLightLevel[stInfo.lGroupID] = stInfo.lRingLevel;	
	m_lBPRGenSideLightLevel[stInfo.lGroupID] = stInfo.lSideLevel;	

	if ( lInitPR != 1 )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
	PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
	PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
	PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::SetExposureTime(IPC_CServiceMessage& svMsg)	//ching
{
	PR_COMMON_RPY		stRpy;
	PR_OPTIC			stOptic;
	
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	BOOL	bReturn	= TRUE;

	if (lInitPR == FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	PR_EXPOSURE_TIME	stTime;
	LONG				ExposureTimeLevel;

	svMsg.GetMsg(sizeof(LONG), &ExposureTimeLevel);

	// Saturate the levels
	if (ExposureTimeLevel  > 10 )
		ExposureTimeLevel = 10;

	if (ExposureTimeLevel  < 0 )
		ExposureTimeLevel = 0;

	m_lBPRExposureTimeLevel = ExposureTimeLevel;

	switch(ExposureTimeLevel)
	{
		case 1:
			stTime=PR_EXPOSURE_TIME_1;
			break;
		case 2:
			stTime=PR_EXPOSURE_TIME_2;
			break;
		case 3:
			stTime=PR_EXPOSURE_TIME_3;
			break;
		case 4:
			stTime=PR_EXPOSURE_TIME_4;
			break;
		case 5:
			stTime=PR_EXPOSURE_TIME_5;
			break;
		case 6:
			stTime=PR_EXPOSURE_TIME_6;
			break;
		case 7:
			stTime=PR_EXPOSURE_TIME_7;
			break;
		case 8:
			stTime=PR_EXPOSURE_TIME_8;
			break;
		case 9:
			stTime=PR_EXPOSURE_TIME_9;
			break;
		case 10:
			stTime=PR_EXPOSURE_TIME_10;
			break;
		default:
			stTime=PR_EXPOSURE_TIME_DEFAULT;
			break;
	}

	CString szErrMsg;
	//v4.50A15
	BOOL bLearnPSPRColletHole = FALSE;
	if (CMS896AStn::m_bAutoChangeCollet && 
		(m_bSelectDieType == BPR_REFERENCE_DIE) && 
		(m_lCurRefDieNo == 4))
	{
		bLearnPSPRColletHole = TRUE;
	}


	if (bLearnPSPRColletHole)	//v4.50A15
	{
		PR_GetOptic(MS899_POSTSEAL_GEN_PURPOSE, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_SetExposureTime(stTime, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_GetOptic(MS899_POSTSEAL_PR_PURPOSE, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_SetExposureTime(stTime, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
	}
	else
	{
		PR_GetOptic(MS899_BOND_PB_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		if ((stRpy.uwCommunStatus == PR_COMM_NOERR) && (stRpy.uwPRStatus == PR_ERR_NOERR))
		{
			PR_SetExposureTime(stTime, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

			if (stRpy.uwCommunStatus != PR_COMM_NOERR)
			{
				HmiMessage("PR_SetExposureTime COMM error!");
			}
			else if (stRpy.uwPRStatus != PR_ERR_NOERR)
			{
				HmiMessage("PR_SetExposureTime PR error!");
			}
		}
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::SetGExposureTime(IPC_CServiceMessage& svMsg)
{
	
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	BOOL	bReturn	= TRUE;

	if (m_bFreezeCamera_HMI || lInitPR == FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	PR_COMMON_RPY		stRpy;
	PR_OPTIC			stOptic;
	PR_EXPOSURE_TIME	stTime;
	LONG				ExposureTimeLevel;

	svMsg.GetMsg(sizeof(LONG), &ExposureTimeLevel);

	// Saturate the levels
	if (ExposureTimeLevel  > 10 )
		ExposureTimeLevel = 10;

	if (ExposureTimeLevel  < 0 )
		ExposureTimeLevel = 0;

	m_lBPRGExposureTimeLevel = ExposureTimeLevel;

	switch(ExposureTimeLevel)
	{
		case 1:
			stTime=PR_EXPOSURE_TIME_1;
			break;
		case 2:
			stTime=PR_EXPOSURE_TIME_2;
			break;
		case 3:
			stTime=PR_EXPOSURE_TIME_3;
			break;
		case 4:
			stTime=PR_EXPOSURE_TIME_4;
			break;
		case 5:
			stTime=PR_EXPOSURE_TIME_5;
			break;
		case 6:
			stTime=PR_EXPOSURE_TIME_6;
			break;
		case 7:
			stTime=PR_EXPOSURE_TIME_7;
			break;
		case 8:
			stTime=PR_EXPOSURE_TIME_8;
			break;
		case 9:
			stTime=PR_EXPOSURE_TIME_9;
			break;
		case 10:
			stTime=PR_EXPOSURE_TIME_10;
			break;
		default:
			stTime=PR_EXPOSURE_TIME_DEFAULT;
	}

	PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

	if ((stRpy.uwCommunStatus == PR_COMM_NOERR) && (stRpy.uwPRStatus == PR_ERR_NOERR))
	{
		PR_SetExposureTime(stTime, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

		if (stRpy.uwCommunStatus != PR_COMM_NOERR)
		{
			HmiMessage("PR_SetExposureTime COMM error!");
		}
		else if (stRpy.uwPRStatus != PR_ERR_NOERR)
		{
			HmiMessage("PR_SetExposureTime PR error!");
		}
	}
	else
	{
		HmiMessage("BPR: PR_GetOptic COMM error in PR_SetExposureTime!");
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::GetGExposureTime(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;

	//if ( m_bPRInit == FALSE )
	//{
	//	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	//	return 1;
	//}

	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_EXPOSURE_TIME stTime = (PR_EXPOSURE_TIME)0;
	LONG lExposureTimeLevel = 0;

	PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

	if ((stRpy.uwCommunStatus == PR_COMM_NOERR) && (stRpy.uwPRStatus == PR_ERR_NOERR))
	{
		PR_GetExposureTime(&stOptic, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stTime, &stRpy);

		if (stRpy.uwCommunStatus != PR_COMM_NOERR)
		{
			HmiMessage("PR_SetExposureTime COMM error!");
		}
		else if (stRpy.uwPRStatus != PR_ERR_NOERR)
		{
			HmiMessage("PR_SetExposureTime PR error!");
		}
	}
	else
	{
		AfxMessageBox("PR_GetOptic PR error in GetGeneralExposureTime!");
	}
	
	switch(stTime)
	{
		case PR_EXPOSURE_TIME_1:
			lExposureTimeLevel = 1;
			break;
		case PR_EXPOSURE_TIME_2:
			lExposureTimeLevel = 2;
			break;
		case PR_EXPOSURE_TIME_3:
			lExposureTimeLevel = 3;
			break;
		case PR_EXPOSURE_TIME_4:
			lExposureTimeLevel = 4;
			break;
		case PR_EXPOSURE_TIME_5:
			lExposureTimeLevel = 5;
			break;
		case PR_EXPOSURE_TIME_6:
			lExposureTimeLevel = 6;
			break;
		case PR_EXPOSURE_TIME_7:
			lExposureTimeLevel = 7;
			break;
		case PR_EXPOSURE_TIME_8:
			lExposureTimeLevel = 8;
			break;
		case PR_EXPOSURE_TIME_9:
			lExposureTimeLevel = 9;
			break;
		case PR_EXPOSURE_TIME_10:
			lExposureTimeLevel = 10;
			break;
		default:
			lExposureTimeLevel = 0;
			break;
	}

	m_lBPRGExposureTimeLevel = lExposureTimeLevel;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::ConfirmLearnDie(IPC_CServiceMessage& svMsg)
{
	//SavePrData();
	short i = 0;
	PR_WORD siTotal = 0;
/*
	if( IsMS90() && IsMS90Sorting2ndPart() )
	{
		HmiMessage_Red_Back("Please learn die in original orientation.", "Bond PR");
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
*/
	//convert to 4X mode
	LONG lOldZoom = BPR_GetRunZoom();
	if ((m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == BPR_REFERENCE_PR_DIE_INDEX6))
	{
		BPR_LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_4X);
	}

	if (IsSensorZoomFFMode())
	{
		for (LONG i = 0; i < BPR_MAX_DIE_CORNER; i++)
		{
			m_stLearnDieCornerPos[i] = m_stLearnFFModeDieCornerPos[i];
		}
	}
	else
	{
		for (LONG i = 0; i < BPR_MAX_DIE_CORNER; i++)
		{
			m_stLearnDieCornerPos[i] = m_stLearnNormalDieCornerPos[i];
		}
	}


	SelectBondCamera();

	// learn die page, press the start learn button.
	m_ucGenDieShape = CheckDieShape(m_ucDieShape);	//Assign HMI var (ucDieShape) into ucGenDieShape attribute

	for (i=0; i<BPR_MAX_DIE_CORNER; i++ )
	{
		siTotal += (m_stLearnDieCornerPos[i].x + m_stLearnDieCornerPos[i].y);
	}

	if( ( siTotal == 0 ) && ( GetDieShape() == BPR_RECTANGLE_DIE ) )
	{
		SetErrorMessage("BPR die size press Start Learn button, default is 0, set with die size.");	//xxxxxx
		LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
		m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= (PR_WORD)(m_lPrCenterX - m_stGenDieSize[lDieNo].x/2);
		m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= (PR_WORD)(m_lPrCenterY - m_stGenDieSize[lDieNo].y/2);
		m_stLearnDieCornerPos[PR_LOWER_RIGHT].x	= (PR_WORD)(m_lPrCenterX + m_stGenDieSize[lDieNo].x/2);
		m_stLearnDieCornerPos[PR_LOWER_RIGHT].y	= (PR_WORD)(m_lPrCenterY + m_stGenDieSize[lDieNo].y/2);
	}

	siTotal = 0;
	for (i=0; i<BPR_MAX_DIE_CORNER; i++ )
	{
		siTotal += (m_stLearnDieCornerPos[i].x + m_stLearnDieCornerPos[i].y);
	}

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
		SetErrorMessage("BPR die size press Start Learn button, default is 0.");	//xxxxxx
		if ( GetDieShape() == BPR_RECTANGLE_DIE )
		{
			m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= (PR_WORD)m_lPrCenterX - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= (PR_WORD)m_lPrCenterY - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x	= (PR_WORD)m_lPrCenterX + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].y	= (PR_WORD)m_lPrCenterY + BPR_DETECT_DIE_CORNER;
		}
		else if ( GetDieShape() == BPR_TRIANGULAR_DIE )	//v4.06
		{
			m_stLearnDieCornerPos[BPR_DIE_CORNER_0].x	= (PR_WORD)m_lPrCenterX;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_0].y	= (PR_WORD)m_lPrCenterY - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_1].x	= (PR_WORD)m_lPrCenterX - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_1].y	= (PR_WORD)m_lPrCenterY + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_2].x	= (PR_WORD)m_lPrCenterX + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_2].y	= (PR_WORD)m_lPrCenterY + BPR_DETECT_DIE_CORNER;
		}
		else if ( GetDieShape() == BPR_RHOMBUS_DIE )
		{
			m_stLearnDieCornerPos[BPR_DIE_CORNER_0].x	= (PR_WORD)m_lPrCenterX - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_0].y	= (PR_WORD)m_lPrCenterY - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_1].x	= (PR_WORD)m_lPrCenterX - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_1].y	= (PR_WORD)m_lPrCenterY + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_2].x	= (PR_WORD)m_lPrCenterX + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_2].y	= (PR_WORD)m_lPrCenterY + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_3].x	= (PR_WORD)m_lPrCenterX + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_3].y	= (PR_WORD)m_lPrCenterY - BPR_DETECT_DIE_CORNER;
		}
		else
		{
			m_stLearnDieCornerPos[BPR_DIE_CORNER_0].x	= (PR_WORD)m_lPrCenterX - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_0].y	= (PR_WORD)m_lPrCenterY - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_1].x	= (PR_WORD)m_lPrCenterX - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_1].y	= (PR_WORD)m_lPrCenterY;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_2].x	= (PR_WORD)m_lPrCenterX - BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_2].y	= (PR_WORD)m_lPrCenterY + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_3].x	= (PR_WORD)m_lPrCenterX + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_3].y	= (PR_WORD)m_lPrCenterY + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_4].x	= (PR_WORD)m_lPrCenterX + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_4].y	= (PR_WORD)m_lPrCenterY;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_5].x	= (PR_WORD)m_lPrCenterX + BPR_DETECT_DIE_CORNER;
			m_stLearnDieCornerPos[BPR_DIE_CORNER_5].y	= (PR_WORD)m_lPrCenterY - BPR_DETECT_DIE_CORNER;
		}
	}

	m_lInspDetectWinNo = 0;
	m_lInspIgnoreWinNo = 0;
	for (i=0; i<BPR_MAX_DETECT_WIN; i++)
	{
		m_stInspDetectWin[i].coCorner1.x = (PR_WORD)m_lPrCenterX - BPR_DETECT_DIE_CORNER;
		m_stInspDetectWin[i].coCorner1.y = (PR_WORD)m_lPrCenterY - BPR_DETECT_DIE_CORNER;
		m_stInspDetectWin[i].coCorner2.x = (PR_WORD)m_lPrCenterX + BPR_DETECT_DIE_CORNER;
		m_stInspDetectWin[i].coCorner2.y = (PR_WORD)m_lPrCenterY + BPR_DETECT_DIE_CORNER;

		m_stInspIgnoreWin[i].coCorner1.x = (PR_WORD)m_lPrCenterX - BPR_DETECT_DIE_CORNER;
		m_stInspIgnoreWin[i].coCorner1.y = (PR_WORD)m_lPrCenterY - BPR_DETECT_DIE_CORNER;
		m_stInspIgnoreWin[i].coCorner2.x = (PR_WORD)m_lPrCenterX + BPR_DETECT_DIE_CORNER;
		m_stInspIgnoreWin[i].coCorner2.y = (PR_WORD)m_lPrCenterY + BPR_DETECT_DIE_CORNER;
	}

	//Learn circle	//v4.47A6
	BOOL bLearnColletHole = FALSE;
	BOOL bAGCUpLookCamera = FALSE;
	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (m_bEnableMS100EjtXY && (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 3))
	{
		bLearnColletHole = TRUE;
	}	
	else if ( (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == BPR_REFERENCE_PR_DIE_INDEX6))
	{
		CString szSelection1 = "Arm1";
		CString szSelection2 = "Arm2";
		m_nArmSelection = HmiMessage("Please choose which arm to be learnt.", "Learn Collet Hole", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);

		CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
		pBinTable->MoveTableToEmptyPosition();
		Sleep(100);
		if (m_nArmSelection == 1)
		{
			MoveBhToBond(TRUE);
		}
		else
		{
			MoveBhToBond_Z2(TRUE);
		}
	}
	//v4.50A13
	else if (CMS896AStn::m_bAutoChangeCollet && (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4))
	{
HmiMessage("AGC: Learn collet hole using UPLOOK camera ...");
		SelectPostSealCamera();					//v4.50A14
		bLearnColletHole = TRUE;
		bAGCUpLookCamera = TRUE;
/*
		PR_COMMON_RPY	stRpy;
		PR_OPTIC		stOptic;
		PR_UWORD		usLpCoaxLightLevel;
		PR_UWORD		usLpRingLightLevel;
		PR_GetOptic(MS899_POSTSEAL_GEN_PURPOSE, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_GetLighting(&stOptic, PR_COAXIAL_LIGHT,	PSPR_SENDER_ID, PSPR_RECV_ID, &usLpCoaxLightLevel, &stRpy);
		PR_GetLighting(&stOptic, PR_RING_LIGHT,		PSPR_SENDER_ID, PSPR_RECV_ID, &usLpRingLightLevel, &stRpy);
*/
	}	

	
	// Move BH to BON for learning collet hole pattern
	if (m_bPostBondAtBond || bLearnColletHole)	//v4.47A6
	{
#ifndef NU_MOTION
		MoveBhToBond(TRUE);
#else

		CString szSelection1 = "Arm PR";
		CString szSelection2 = "Upload PR";

		m_lLearnUPLPRIndex = HmiMessage("Please choose which PR to be learnt", "Learn Collet Hole", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);
		//m_lLearnUPLPRIndex 0 -- for Bond arm uplook, 1--for upload uplook

		if (m_lLearnUPLPRIndex == 1)
		{
			szSelection1 = "Arm1";
			szSelection2 = "Arm2";
			m_nArmSelection = HmiMessage("Please choose which arm to be learnt.", "Learn Collet Hole", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);

			if ( m_nArmSelection == 1 )
			{
				//v4.50A14
				if (CMS896AStn::m_bAutoChangeCollet && (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4))
				{
					BTMoveToUpLookPos(FALSE);
				}
				MoveBhToBond(TRUE);
			}
			else
			{
				//v4.50A14
				if (CMS896AStn::m_bAutoChangeCollet && (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4))
				{
					BTMoveToUpLookPos(FALSE);
				}
				MoveBhToBond_Z2(TRUE);
			}
		}
		else
		{
			//Move to Upload PR position
			BTMoveToAGCUPLUploadPos();
		}
#endif
	}

	if (!bAGCUpLookCamera)		//v4.50A14
	{
		PR_COMMON_RPY stRpy;
		PR_EnableHwTrigLighting(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stRpy);
		if (stRpy.uwCommunStatus != PR_COMM_NOERR || stRpy.uwPRStatus != PR_ERR_NOERR)
		{
			CString csMsg;
			csMsg.Format("BPR: Enable STROKE lighting error! = 0x%x, 0x%x", stRpy.uwCommunStatus, stRpy.uwPRStatus);
			SetErrorMessage(csMsg);
			HmiMessage(csMsg);
			//return FALSE;
		}
	}

	m_bFreezeCamera_HMI = TRUE;
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::CancelLearnDie(IPC_CServiceMessage& svMsg)
{
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	BOOL	bReturn=TRUE;

	m_bFreezeCamera_HMI = FALSE;

	if (lInitPR != 1)
	{
		UpdateHMIVariable();
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}


	PR_COMMON_RPY		stRpy;
	PR_OPTIC			stOptic;
	PR_UWORD			usCoaxLightLevel;
	PR_UWORD			usRingLightLevel;
	PR_UWORD			usSideLightLevel;
	PR_SOURCE			emCoaxID, emRingID, emSideID;
	int					i;


	if ( CMS896AStn::m_bEnablePRDualColor == TRUE )
	{
		PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

		for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
		{
			//Get General Lighting
			GetLightingConfig(i, emCoaxID, emRingID, emSideID);
			PR_GetLighting(&stOptic, emCoaxID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usCoaxLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emRingID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usRingLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emSideID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usSideLightLevel, &stRpy);

			//Restore General Ligthting
			PR_SetLighting(emCoaxID, usCoaxLightLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
			PR_SetLighting(emRingID, usRingLightLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
			PR_SetLighting(emSideID, usSideLightLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		}
	}
	else
	{
		//Get General Lighting
		PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

		GetLightingConfig(0, emCoaxID, emRingID, emSideID);
		PR_GetLighting(&stOptic, emCoaxID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usCoaxLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emRingID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usRingLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emSideID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usSideLightLevel, &stRpy);

		//Restore General Ligthting
		PR_SetLighting(emCoaxID, usCoaxLightLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(emRingID, usRingLightLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(emSideID, usSideLightLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
	}

	//v3.31
	//Use stroke lighting for MS810
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
//	if (CMS896AApp::m_lHardwareConfigNo == MS896A_DDBS_WITH_RING_MASTER)
	{
		PR_COMMON_RPY stRpy;
		PR_DisableHwTrigLighting(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stRpy);
		
		if (stRpy.uwCommunStatus != PR_COMM_NOERR || stRpy.uwPRStatus != PR_ERR_NOERR)
		{
			CString csMsg;
			csMsg.Format("BPR: Disable STROKE lighting error! = 0x%x, 0x%x", stRpy.uwCommunStatus, stRpy.uwPRStatus);
			SetErrorMessage(csMsg);
			HmiMessage(csMsg);
			//return FALSE;
		}
	}

	SelectBondCamera();

	//Learn circle	//v4.47A6
	BOOL bLearnColletHole = FALSE;
	if (m_bEnableMS100EjtXY && (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 3))
	{
		bLearnColletHole = TRUE;
	}	
	//v4.50A13
	else if (CMS896AStn::m_bAutoChangeCollet && (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4))
	{
		bLearnColletHole = TRUE;
	}	

	if (m_bPostBondAtBond || bLearnColletHole || 
		(m_bSelectDieType == BPR_REFERENCE_DIE && m_lCurRefDieNo == BPR_REFERENCE_PR_DIE_INDEX6))
	{
#ifndef NU_MOTION
		MoveBhToBond(FALSE);
#else
		if ( m_nArmSelection == 1 )
		{
			MoveBhToBond(FALSE);
		}
		else
		{
			MoveBhToBond_Z2(FALSE);
		}
#endif
	}

	UpdateHMIVariable();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::GetLearnDieLighting(IPC_CServiceMessage& svMsg)
{
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	BOOL	bReturn=TRUE;

	if (lInitPR != 1)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);	
		return -1;
	}


	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_UWORD		usCoaxLightLevel;
	PR_UWORD		usRingLightLevel;
	PR_UWORD		usSideLightLevel;
	PR_SOURCE		emCoaxID, emRingID, emSideID;
	int				i;


	if ( CMS896AStn::m_bEnablePRDualColor == TRUE )
	{
		PR_GetOptic(MS899_BOND_PB_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

		for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
		{
			GetLightingConfig(i, emCoaxID, emRingID, emSideID);
			PR_GetLighting(&stOptic, emCoaxID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usCoaxLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emRingID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usRingLightLevel, &stRpy);
			PR_GetLighting(&stOptic, emSideID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usSideLightLevel, &stRpy);
		}
	}
	else
	{
		PR_GetOptic(MS899_BOND_PB_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

		GetLightingConfig(0, emCoaxID, emRingID, emSideID);
		PR_GetLighting(&stOptic, emCoaxID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usCoaxLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emRingID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usRingLightLevel, &stRpy);
		PR_GetLighting(&stOptic, emSideID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &usSideLightLevel, &stRpy);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondPr::DisableLightingSetting(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	if (m_bDisableLightingSetting && (pAppMod->m_bOperator == TRUE))
	{
		bReturn = FALSE;
		HmiMessage("Lighting Setting is disable!");
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return -1;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondPr::SetLearnDieLighting(IPC_CServiceMessage& svMsg)
{
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	BOOL	bReturn=TRUE;

	if (lInitPR != 1)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}


	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;
	PR_SOURCE		emCoaxID, emRingID, emSideID;

	typedef struct {
		LONG lGroupID;
		LONG lCoaxLevel;
		LONG lRingLevel;
		LONG lSideLevel;
	} SETLIGHT;

	SETLIGHT stInfo;
	svMsg.GetMsg(sizeof(SETLIGHT), &stInfo);

	if ( stInfo.lGroupID >= BPR_MAX_LIGHT_GROUP )
	{
		stInfo.lGroupID = 0;
	}

	//v4.50A14
	BOOL bAGCUpLookCamera = FALSE;
	if (CMS896AStn::m_bAutoChangeCollet && 
		(m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4))
	{
		bAGCUpLookCamera = TRUE;
	}	

	GetLightingConfig(stInfo.lGroupID, emCoaxID, emRingID, emSideID);
	PR_GetOptic(MS899_BOND_PB_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
	PR_SetLighting(emCoaxID, (PR_UWORD)stInfo.lCoaxLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
	PR_SetLighting(emRingID, (PR_UWORD)stInfo.lRingLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);
	PR_SetLighting(emSideID, (PR_UWORD)stInfo.lSideLevel, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stRpy);

	if (bAGCUpLookCamera)	//v4.50A14
	{
		PR_GetOptic(MS899_POSTSEAL_GEN_PURPOSE, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(PR_COAXIAL_LIGHT,(PR_UWORD)stInfo.lCoaxLevel, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(PR_RING_LIGHT,	(PR_UWORD)stInfo.lRingLevel, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(PR_SIDE_LIGHT,	(PR_UWORD)stInfo.lSideLevel, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		
		PR_GetOptic(MS899_POSTSEAL_PR_PURPOSE, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(PR_COAXIAL_LIGHT,(PR_UWORD)stInfo.lCoaxLevel, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(PR_RING_LIGHT,	(PR_UWORD)stInfo.lRingLevel, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
		PR_SetLighting(PR_SIDE_LIGHT,	(PR_UWORD)stInfo.lSideLevel, PSPR_SENDER_ID, PSPR_RECV_ID, &stOptic, &stRpy);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::MovePRCursor(IPC_CServiceMessage& svMsg)
{
	PR_COORD		sttmpStartPos;
	PR_WORD			sstmpPixelStep;
	LONG			lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	unsigned char	uctmpCornerNo;

	typedef struct {
		unsigned char ucDirection;
		unsigned char ucPixelStep;
		unsigned char ucDieCorner;
	} MOVECURSOR;


	MOVECURSOR	stInfo;
	svMsg.GetMsg(sizeof(MOVECURSOR), &stInfo);

	if ( GetDieShape() != BPR_RECTANGLE_DIE )
	{
		return 1;
	}

	//Get Pixel step move
	switch(stInfo.ucPixelStep)
	{
		case 1:		//10 pixel
			sstmpPixelStep = 10*PR_SCALE_FACTOR;
			break;

		case 2:		//30 pixel
			sstmpPixelStep = 30*PR_SCALE_FACTOR;
			break;
		
		default:	//1 pixel
			sstmpPixelStep = 1*PR_SCALE_FACTOR;
			break;
	}

	//Check current learn corner pos
	switch(stInfo.ucDieCorner)
	{
		case PR_LOWER_RIGHT:	//Lower Right		
			uctmpCornerNo = PR_LOWER_RIGHT;
			break;
		
		default:				//Upper Left
			uctmpCornerNo = PR_UPPER_LEFT;
			break;
	}
	sttmpStartPos = m_stLearnDieCornerPos[uctmpCornerNo];


	if (lInitPR != 1)
	{
		CString csMsg;
		csMsg.Format("CBondPr:: Cannot MoveCursor (%d,%d)", stInfo.ucDirection,stInfo.ucPixelStep);
		DisplayMessage(csMsg);
		return -1;
	}

	//Remove Current Cursor
	DrawAndEraseCursor(sttmpStartPos,uctmpCornerNo,0);

	switch(stInfo.ucDirection)
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
	DrawAndEraseCursor(sttmpStartPos,uctmpCornerNo,1);
	CString szTemp;
	szTemp.Format(" (%ld, %ld, %ld, %ld)",
		m_stLearnDieCornerPos[PR_UPPER_LEFT].x,		m_stLearnDieCornerPos[PR_UPPER_LEFT].y,
		m_stLearnDieCornerPos[PR_LOWER_RIGHT].x,	m_stLearnDieCornerPos[PR_LOWER_RIGHT].y);
	SetErrorMessage("BPR die size Move PR Cursor, change die corner." + szTemp);	//xxxxxx

	if (!IsSensorZoomFFMode())
	{
		if (m_stLearnDieCornerPos[PR_UPPER_LEFT].x > ((PR_WORD)m_lPrCenterX - BPR_MIN_DIE_CORNER))
		{
			m_stLearnDieCornerPos[PR_UPPER_LEFT].x = (PR_WORD)m_lPrCenterX - BPR_MIN_DIE_CORNER;
		}

		if (m_stLearnDieCornerPos[PR_UPPER_LEFT].y > ((PR_WORD)m_lPrCenterY - BPR_MIN_DIE_CORNER))
		{
			m_stLearnDieCornerPos[PR_UPPER_LEFT].y = (PR_WORD)m_lPrCenterY - BPR_MIN_DIE_CORNER;
		}

		if (m_stLearnDieCornerPos[PR_LOWER_RIGHT].x < ((PR_WORD)m_lPrCenterX + BPR_MIN_DIE_CORNER))
		{
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x = (PR_WORD)m_lPrCenterX + BPR_MIN_DIE_CORNER;
		}

		if (m_stLearnDieCornerPos[PR_LOWER_RIGHT].y < ((PR_WORD)m_lPrCenterY + BPR_MIN_DIE_CORNER))
		{
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].y = (PR_WORD)m_lPrCenterY + BPR_MIN_DIE_CORNER;
		}
	}
	return 1;
}


LONG CBondPr::AutoLearnDie(IPC_CServiceMessage& svMsg)
{
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	BOOL	bReturn=TRUE;
	BOOL	bAskLearnFFModeCalibration =FALSE;
	PR_UWORD lLearnStatus = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHUplookPr	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);	//v4.58A4
	BOOL bPreBond = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];	//v4.58A4
	CString szTitle, szContent;

	m_bFreezeCamera_HMI = FALSE;
	if (lInitPR != 1)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}

	if (m_bSelectDieType == BPR_REFERENCE_DIE && (m_lCurRefDieNo == 1) && pApp->GetFeatureStatus(MS896A_FUNC_BIN_FRAME_CROSS))	
	{
		CString strTemp = "Please put the cross on the top-right reference cross";
		CString szTitle;
		szTitle.Format("Learn Ref. Die %d", m_lCurRefDieNo);
		LONG lTmp = HmiMessage(strTemp, szTitle, glHMI_MBX_OKCANCEL, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300, NULL, NULL, NULL, NULL);
		if (lTmp == glHMI_CANCEL)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return -1;			
		}
	}

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetBinTableJoystick(FALSE);
	}

	//Get General Lighting
	GetGeneralLighting(svMsg);
	GetHMIVisionVariable();

	//Erase user set position
	if ( GetDieShape() == BPR_RECTANGLE_DIE )
	{
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT],PR_UPPER_LEFT,0);
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT],PR_LOWER_RIGHT,0);
	}
	else if ( GetDieShape() == BPR_TRIANGULAR_DIE )	//v4.06
	{
		DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 0);
		DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[0], 0);
	}
	else if ( GetDieShape() == BPR_RHOMBUS_DIE )
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

	if (bEnableBHUplookPr && (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 1) &&
		m_bPreBondAlignUseEpoxySearch)		//v4.58A4
	{
		HmiMessage("Learning PREBOND EPOXY pattern ....");

		AutoLearnPreBondEpoxy(m_stLearnDieCornerPos[PR_UPPER_LEFT], m_stLearnDieCornerPos[PR_LOWER_RIGHT]);
	}
	else if (m_bSelectDieType == BPR_REFERENCE_DIE && m_lCurRefDieNo == BPR_REFERENCE_PR_DIE_INDEX6)
	{
		HmiMessage("Learning Bond Collet Epoxy Record");
		AutoLearnBondEpoxy(m_stLearnDieCornerPos[PR_UPPER_LEFT], m_stLearnDieCornerPos[PR_LOWER_RIGHT]);
	}
	else if (m_bEnableMS100EjtXY && (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 3))
	{
		bLearnColletHole = TRUE;
		AutoLearnCircle(m_stLearnDieCornerPos[PR_UPPER_LEFT], m_stLearnDieCornerPos[PR_LOWER_RIGHT]) ;
	}
	//v4.50A13
	else if (CMS896AStn::m_bAutoChangeCollet && (m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 4)) 
	{
		//m_lLearnUPLPRIndex 0 -- for Bond arm uplook, 1--for upload uplook
		bLearnColletHole = TRUE;
		m_lLearnUPLPRIndex = m_lLearnUPLPRIndex - 1;
		if ((m_lLearnUPLPRIndex < 0) || (m_lLearnUPLPRIndex > 1))
		{
			m_lLearnUPLPRIndex = 0;
		}
		//AGC_AutoLearnCircle(m_stLearnDieCornerPos[PR_UPPER_LEFT], m_stLearnDieCornerPos[PR_LOWER_RIGHT]) ;
		AGC_AutoShowEpoxy(m_stLearnDieCornerPos[PR_UPPER_LEFT], m_stLearnDieCornerPos[PR_LOWER_RIGHT], m_lLearnUPLPRIndex);
	}
	else
	{
		//Start Learn Die and display result
		if (IsSensorZoomFFMode() && ((m_bSelectDieType != BPR_NORMAL_DIE) || (m_lCurRefDieNo != 3)))
		{
			HmiMessage_Red_Back("FF Mode only support to learn (Normal, 3) setting Die");
			bReturn = FALSE;
			
			UpdateBPRInfoList();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;			
		}

		//lLearnStatus = AutoLearnDie(m_bSelectDieType, m_lCurRefDieNo);
		lLearnStatus = AutoManualLearnDie(m_bSelectDieType, m_lCurRefDieNo);
		DisplayLearnDieResult(lLearnStatus);

		if (pApp->GetCustomerName() == "AOT" || pApp->GetCustomerName() == "ABC")	//ABC is used in AOT
		{
			if (lLearnStatus == PR_ERR_NOERR && m_dFovSize < BPR_LF_SIZE)	//FOV updated in SavePrData()
			{
				CString szMsg;
				szMsg.Format("BPR FOV size %.2f less than %.2f, bin frame realignment affected!", m_dFovSize, BPR_LF_SIZE);
				HmiMessage(szMsg, "Bond PR");
			}
		}

		if (m_bSelectDieType == BPR_NORMAL_DIE && m_lCurRefDieNo == 1)// Matthew 20181220 by Harry Ho
		{
			m_lGenLrnAlignAlgo[11] = m_lLrnAlignAlgo;
			m_lGenLrnBackgroud[11] = m_lLrnBackground;
			m_lGenLrnInspMethod[11] = m_lLrnInspMethod;
			m_lGenLrnFineSrch[11] = m_lLrnFineSrch;
			m_lGenSrchDieAreaX[11] = m_lGenSrchDieAreaX[0];
			m_lGenSrchDieAreaY[11] = m_lGenSrchDieAreaY[0];
			if (AutoManualLearnDie(BPR_REFERENCE_DIE, BPR_REFERENCE_PR_DIE_INDEX9) != PR_ERR_NOERR)
			{
				szContent = "NG Grade learn die fail. Please learn again!";
				szTitle = "NG Grade learn die fail";
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			}

			szContent = "Start to learn FF mode die?";
			szTitle = "Learn FF mode Die";
			if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
			{
				if (AutoLearnBondFFModeDie() == PR_ERR_NOERR)
				{
					bAskLearnFFModeCalibration = TRUE;
					m_szGenDieZoomMode[BPR_NORMAL_PR_FF_MODE_INDEX3 + BPR_GEN_NDIE_OFFSET] = "FF";
				}
			}
		}
	}

	//Update Search Die Area
	//Force Search Die Area into 1.8x of die size
	UpdateSearchDieArea(GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE);

	//v3.31
	//Use stroke lighting for MS810
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
//	if (CMS896AApp::m_lHardwareConfigNo == MS896A_DDBS_WITH_RING_MASTER)
	{
		PR_COMMON_RPY stRpy;
		PR_DisableHwTrigLighting(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stRpy);
		
		if (stRpy.uwCommunStatus != PR_COMM_NOERR || stRpy.uwPRStatus != PR_ERR_NOERR)
		{
			CString csMsg;
			csMsg.Format("WPR: Disable STROKE lighting error! = 0x%x, 0x%x", stRpy.uwCommunStatus, stRpy.uwPRStatus);
			SetErrorMessage(csMsg);
			HmiMessage(csMsg);
			//return FALSE;
		}
	}

	//Clear Camera & Restore General Ligthting
	SelectBondCamera();
	RestoreGeneralLighting(svMsg);
	SavePrData();

	//BOOL bPreBond = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];	

	//Ask user to do Calibration if no learn die error
	//if	( (bPreBond || !m_bSelectDieType)	&&		// if ND	//v3.82	//Cree US buyoff
	if	( (!m_bSelectDieType)					&&		//v4.59A3	//WolfSpeed
		  !m_bPostBondAtBond					&&		//v4.47A7	//Collet Hole test on MS60
		  ((lLearnStatus == PR_WARN_TOO_DARK)	|| 
		   (lLearnStatus == PR_WARN_TOO_BRIGHT)	|| 
		   (lLearnStatus == PR_WARN_ROTATION_INVARIANT) || 
		   (lLearnStatus == PR_ERR_NOERR))
		)
	{
		SetszGenDieZoomMode(FALSE);

		SetStatusMessage("BPR learn G-die complete");

		szTitle.LoadString(HMB_BPR_DIE_CALIBRATION);
		szContent.LoadString(HMB_BPR_START_CALIBRATION);

		BOOL bLearnDieCalibration = FALSE;
		if (m_bSelectDieType == BPR_NORMAL_DIE && (m_lCurRefDieNo == 1 || m_lCurRefDieNo == 3))
		{
			bLearnDieCalibration = TRUE;
		}

		if (bLearnDieCalibration && HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
		{
			LearnDieCalibration(svMsg);
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
				LONG lOldZoom = BPR_GetRunZoom();
				BPR_LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_FF);
				LearnDieCalibration(svMsg);
				BPR_LiveViewZoom(lOldZoom);
				m_bGenDieCalibration[3 + BPR_GEN_NDIE_OFFSET] = TRUE;
			}
			else
			{
				m_bGenDieCalibration[3 + BPR_GEN_NDIE_OFFSET] = FALSE;
			}
		}

		//v4.53A21
		if ((CMS896AApp::m_bMS100Plus9InchOption) && !m_bDieCalibrated2)
		{
			HmiMessage("Please also perform die calibration on Bin Table 2.");
		}

		if (m_bUsePostBond == FALSE)
		{
			szTitle.LoadString(HMB_BPR_POSTBOND);
			szContent.LoadString(HMB_BPR_ENABLE_POST_BOND_INSPECTION);

			if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
			{
				m_bUsePostBond = TRUE;
			}
		}
/*
		if (!IsSensorZoomFFMode() && (m_bUsePostBond == TRUE))
		{
			if (pApp->GetCustomerName() != CTM_SEMITEK)	//v4.50A9	//Requested by LeoLam
			{
				szContent.LoadString(HMB_BPR_COPY_ALL_POST_BOND_CRITERIA);
					
				// Ask for copy all post bond criteria
				if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
				{
					IPC_CServiceMessage stMsg;
					LONG lBinBlkNo = 1;
					m_lCurrentBinBlock = lBinBlkNo;
					UpdateHMIVariable();
					svMsg.InitMessage(sizeof(LONG), &lBinBlkNo);
					CopyAllSPCValue(svMsg);
				}
			}
		}
*/
		SavePrData();
	}
	else
	{
		SetbGenDieCalibration(FALSE);
		SetszGenDieZoomMode(TRUE);
	}
	
	//v4.59A3
	if ((m_bSelectDieType == BPR_REFERENCE_DIE) && !bLearnColletHole)		//v4.47A6
	{
		SetStatusMessage("BPR learn Ref-die complete");
		//SavePrData();

		if ((m_lCurRefDieNo == 2) &&		//v4.59A3	//CrossMark Check fcn now uses Ref #2; Ref #1 is reserved for PreBond alignment
			pApp->GetFeatureStatus(MS896A_FUNC_BIN_FRAME_CROSS))
		{
			HmiMessage("BOND PR: saving 2-Cross encoder position ....", "Bond PR Ref Pattern");		//v4.59A3	

			IPC_CServiceMessage stMsg;
			int nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SaveReferenceCrossEncoderValue", stMsg);
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
		}
	}

	//shiraishi02
	BOOL bCheckFrameOrient = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["CheckFrameOrientation"];
	if ((m_bSelectDieType == BPR_REFERENCE_DIE) && (m_lCurRefDieNo == 1) && bCheckFrameOrient)
	{
		HmiMessage("Learning BinTable XY position for frame-orient-check fcn ...");

		IPC_CServiceMessage stMsg;
		BOOL bReturn = TRUE;
		INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetBinFrameOrientXY", stMsg);
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
	}


	//v2.60
	if (m_bPostBondAtBond || bLearnColletHole ||
		(m_bSelectDieType == BPR_REFERENCE_DIE && m_lCurRefDieNo == BPR_REFERENCE_PR_DIE_INDEX6))	//v4.47A6
	{
#ifndef NU_MOTION
		MoveBhToBond(FALSE);
#else
		if ( m_nArmSelection == 1 )
		{
			MoveBhToBond(FALSE);
		}
		else
		{
			MoveBhToBond_Z2(FALSE);
		}

#endif
	}

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetBinTableJoystick(TRUE);
	}

    // 3341
    SetGemValue("BPR_bDieLearnt",		m_bDieLearnt);
    SetGemValue("BPR_bDieCalibrated",	m_bDieCalibrated);
    SetGemValue("BPR_lDieSearchID",		m_lCurNormDieID);
    SetGemValue("BPR_DieAlignMethod",	m_lLrnAlignAlgo);
    SetGemValue("BPR_DieInspectMethod", m_lLrnInspMethod);
    // 3342
    SetGemValue("BPR_lDieSizeX",		m_lCurNormDieSizeX);
    SetGemValue("BPR_lDieSizeY",		m_lCurNormDieSizeY);
    // 3343
    SetGemValue("BPR_DieCoaxialLight",	m_lBPRLrnCoaxLightLevel[0]);
    SetGemValue("BPR_DieRingLight",		m_lBPRLrnRingLightLevel[0]);
    // 7150
    SendEvent(SG_CEID_BP_LEARNDIE,		FALSE);

	UpdateBPRInfoList();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::BprAutoColorAdjust(IPC_CServiceMessage& svMsg)
{
	PR_MODIFY_COLOR_COEFF_CMD    pstCmd;

	PR_InitModifyColorCoeffCmd( &pstCmd );

	pstCmd.emPurpose = MS899_BOND_PB_PURPOSE;
	PR_MODIFY_COLOR_COEFF_RPY     pstRpy;
	PR_ModifyColorCoeffCmd(&pstCmd,  MS899_BOND_CAM_SEND_ID,  MS899_BOND_CAM_RECV_ID, &pstRpy);

	CString szErrMsg;
	m_pPrGeneral->DrawHomeCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, m_lPrCenterX, m_lPrCenterY, szErrMsg);
	return 1;
}


LONG CBondPr::AutoDetectDie(IPC_CServiceMessage& svMsg)
{
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	if (lInitPR != 1)
	{
		return -1;
	}

	if ( GetDieShape() == BPR_RECTANGLE_DIE )
	{
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT],  PR_UPPER_LEFT,  1);
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 1);
	}
	else if ( GetDieShape() == BPR_TRIANGULAR_DIE )		//v4.06
	{
		DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[0], 1);
	}
	else if ( GetDieShape() == BPR_RHOMBUS_DIE )
	{
		DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 1);
		DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[0], 1);
	}	//	rhombus die
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


LONG CBondPr::ConfirmSearchDiePara(IPC_CServiceMessage& svMsg)
{
	SavePrData();

    // 3346
    SetGemValue("BPR_SrhDiePassScore", m_lSrchDieScore);
    SetGemValue("BPR_SrhDieCheckChip", m_bSrchEnableChipCheck);
    SetGemValue("BPR_SrhDieChipRate", m_dSrchChipArea);
    SetGemValue("BPR_SrhDieCheckDefect", m_bSrchEnableDefectCheck);
    SetGemValue("BPR_SrhDieSDefectRate", m_dSrchSingleDefectArea);
    SetGemValue("BPR_SrhDieTDefectRate", m_dSrchTotalDefectArea);
    // 3347
    SetGemValue("BPR_SrhDieAreaSize", m_lSrchDieAreaX);
    SetGemValue("BPR_SrhDieAreaSize", m_lSrchDieAreaY);
    // 3348
    SetGemValue("BPR_SrhDieGreyLevel", m_lSrchGreyLevelDefect);
    // 3349
    SetGemValue("BPR_SrhDefectThershold", m_lSrchDefectThres);
    // 7151
    SendEvent(SG_CEID_BP_SEARCHPARAM, FALSE);

    // 3351
    SetGemValue("BPR_bEnableInspect", m_bUsePostBond);
    SetGemValue("BPR_lPBBinBlockNo", m_lCurrentBinBlock);
    // 3352
    SetGemValue("BPR_bPBCheckAvgAngle", m_bAverageAngleCheck);
    SetGemValue("BPR_fPBAvgAngle", m_dAverageAngle);
    SetGemValue("BPR_bPBCheckMaxAngle", m_bBinTableAngleCheck);
    SetGemValue("BPR_fPBMaxAngle", m_dMaxAngle);
    SetGemValue("BPR_lPBMaxAngleDies", m_lMaxAngleAllow);
    // 3353
    SetGemValue("BPR_bPBCheckShift", m_bPlacementCheck);
    SetGemValue("BPR_fPBMaxShiftX", m_dDieShiftX);
    SetGemValue("BPR_fPBMaxShiftY", m_dDieShiftY);
    SetGemValue("BPR_lPBMaxShiftDies", m_lMaxShiftAllow);
    // 3354
    SetGemValue("BPR_bPBCheckDefect", m_bDefectCheck);
    SetGemValue("BPR_bPBDefectDies", m_lMaxDefectAllow);
    // 7152
    SendEvent(SG_CEID_BP_POSTBOND, FALSE);

//	SelectBondCamera();
	//v3.83
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::ConfirmSearchDieArea(IPC_CServiceMessage& svMsg)
{
	typedef struct {
		LONG lSrchAreaX;
		LONG lSrchAreaY;
	} SRCH_AREA;

	SRCH_AREA	stInfo;

	svMsg.GetMsg(sizeof(SRCH_AREA), &stInfo);


	m_lSrchDieAreaX = stInfo.lSrchAreaX;
	m_lSrchDieAreaY = stInfo.lSrchAreaY;
	m_dSrchDieAreaX = m_lSrchDieAreaX * BPR_SEARCHAREA_SHIFT_FACTOR;
	m_dSrchDieAreaY = m_lSrchDieAreaY * BPR_SEARCHAREA_SHIFT_FACTOR;

	if (m_bSelectDieType == BPR_REFERENCE_DIE)		//v3.82
		UpdateSearchDieArea(GetSrchDieAreaX(), GetSrchDieAreaY(), FALSE);
	else
		UpdateSearchDieArea(GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE);

	SavePrData();
	return 1;
}

LONG CBondPr::SavePrDataCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	SavePrData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBondPr::IsPRLearnt(LONG lBondPRDieNo)
{
	if (IsThisDieLearnt((UCHAR)lBondPRDieNo - 1) != TRUE )
	{
		CString szMsg;
		if (IsSensorZoomFFMode())
		{
			szMsg.Format("FF Mode die PR %d not learnt yet!", 3);
		}
		else
		{
			szMsg.Format("Normal die PR %d not learnt yet!", 1);
		}
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
		//HmiMessage_Red_Back(szMsg);
		SetAlert_Msg(IDS_BPR_DIENOTLEARNT, szMsg);
		return FALSE;
	}

	return TRUE;
}


LONG CBondPr::BT_MultiSearchInit(IPC_CServiceMessage &svMsg) //4.51D20
{
	CString szMsg;
	szMsg = "Start Test Init";
	CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);

	BOOL bReturn=TRUE;

	if (!IsPRLearnt(GetBondPRDieNo()))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	if (OpPrescanInit((UCHAR)GetBondPRDieNo()) == gnNOTOK)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondPr::BT_MultiSearchDie(IPC_CServiceMessage& svMsg) //4.51D20
{
	//AfxMessageBox("Start Test BT_MultiSearchDie");
	//MULTI_SRCH_RESULT	stMsch;
	BPR_MULTI_SRCH_RESULT stMsch;

	BOOL bDrawDie = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bDrawDie);
	//Assume bDrawDie =FALSE for first testing
	//bDrawDie = FALSE;

	if( bDrawDie==TRUE )
	{
		OpenWaitingAlert();
		m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	}
	int i;
	CDWordArray dwList;
	for(i=0; i<5; i++)
	{
		dwList.SetSize(i+1);
		dwList.SetAt(i, 0);
		stMsch.bDieState[i] = 0;
	}

	int lX = 0, lY=0, lT = 0; // there are not lT
	GetBinTableEncoder(&lX, &lY);
	(*m_psmfSRam)["BinTable"]["Current"]["X"] = lX;
	(*m_psmfSRam)["BinTable"]["Current"]["Y"] = lY;
//	ClearGoodInfo();  //if add this function ,need to #include "PrescanInfo.h" 

	PrescanAutoMultiGrabDone(REALIGN_LOWER_RIGHT_CORNER, dwList, bDrawDie);

	for(i=0; i<5; i++)
	{
		stMsch.bDieState[i] = dwList.GetAt(i);
	}

	if( bDrawDie==TRUE )
	{
		CloseWaitingAlert();
	}

	svMsg.InitMessage(sizeof(BPR_MULTI_SRCH_RESULT), &stMsch);

	return 1;
}

LONG CBondPr::BT_MultiSearchFirstDie(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		UCHAR ucCorner;
	} BPR_MULTI_INPUT;

	BPR_MULTI_INPUT stInfo;
	stInfo.ucCorner = 5;
	svMsg.InitMessage(sizeof(stInfo), &stInfo);
	return BT_MultiSearchFirstDie1(svMsg);
}

LONG CBondPr::BT_MultiSearchFirstDie1(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		UCHAR ucCorner;
	} BPR_MULTI_INPUT;

	BPR_MULTI_INPUT stInfo;
	svMsg.GetMsg(sizeof(BPR_MULTI_INPUT), &stInfo);

	INT nDieStateRight, nDieStateLeft, nDieStateUp, nDieStateDown;
	CString szMsg;
	szMsg = "Search(Step1) -- Start testing BT Multi Search First Die ";
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

	BPR_MULTI_SRCH_DIE_POSITION stMultiDieInfo;

	LONG nErr = RectBondCornerCheck(nDieStateRight, nDieStateLeft, nDieStateUp, nDieStateDown, stInfo.ucCorner);
	if (nErr == IDS_BPR_DIENOTLEARNT)
	{
		stMultiDieInfo.m_bResult = IDS_BPR_DIENOTLEARNT;
		szMsg = "Search -- PRID not Learnt";
	}
	else if (nErr ==FALSE )
	{
		stMultiDieInfo.m_bResult = FALSE;
		szMsg = "Search -- Not only corner check in LR";
	}
	else
	{
		stMultiDieInfo.m_bResult = TRUE;
		szMsg ="Search -- selected corner exists dies only"; //20170824 Leo
	}
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

	stMultiDieInfo.m_nDieEncX		= (LONG)(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"];
	stMultiDieInfo.m_nDieEncY		= (LONG)(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"];
	stMultiDieInfo.m_nDieStateDown	= nDieStateDown;
	stMultiDieInfo.m_nDieStateLeft  = nDieStateLeft;
	stMultiDieInfo.m_nDieStateRight = nDieStateRight;
	stMultiDieInfo.m_nDieStateUp	= nDieStateUp;
	stMultiDieInfo.m_dDieRowFovSize	= m_dRowFovSize; //4.53D01
	stMultiDieInfo.m_dDieColFovSize	= m_dColFovSize; 
	
	szMsg.Format("Search --- FovSize(%f,%f),Step(%d,%d),LeftUpRightDown(%d,%d,%d,%d)", m_dRowFovSize, m_dColFovSize,stMultiDieInfo.m_nDieEncX,stMultiDieInfo.m_nDieEncY,nDieStateLeft,nDieStateUp,nDieStateRight,nDieStateDown);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

	
	svMsg.InitMessage(sizeof(BPR_MULTI_SRCH_DIE_POSITION), &stMultiDieInfo);
	return 1;
}

LONG CBondPr::UserSearchDie(IPC_CServiceMessage& svMsg)
{
	PR_UWORD		usDieType;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	LONG			ulBlkInUse = GetBTCurrentBlock();

	if (m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE)
	{
		SetAlert(IDS_BPR_DIENOTLEARNT);
		SetStatusMessage("F1: Bond PR no die record");
		SetErrorMessage("BPR no die record in UserSearchDie");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}
	if (m_bDieCalibrated == FALSE)	//v4.48A8
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not calibrated");
		SetErrorMessage("Bond PR calibration factor missing in UserSearchDie");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}

	LONG lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	if (lInitPR != 1)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not initialized");
		SetErrorMessage("Bond PR is not initialized in UserSearchDie");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}


	//Clear PR screen & Off joystick
	SelectBondCamera();
	SetBinTableJoystick(FALSE);

	DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);

	LONG lDieNo = BPR_NORMAL_DIE;
	CString szSelection1 = "NORMAL Mode";
	CString szSelection2 = "HW Trigger Mode";

	if (m_bUseHWTrigger)	//andrewng //2020-0615
	{
		m_nArmSelection = HmiMessage("Please choose which BOND PR mode for Die searching", "F1 Search Die", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);
	}
	else
	{
		m_nArmSelection = 1;
	}

	if (m_nArmSelection == 2)
	{
		HmiMessage("Using HW Trigger Mode ....");
		ManualSearchDie_HWTri(BPR_NORMAL_DIE, GetBondPRDieNo(lDieNo), PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
						m_stSearchArea.coCorner1, m_stSearchArea.coCorner2);
	}
	else
	{
		ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(lDieNo), PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
						m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 2);
	}

	if (DieIsAlignable(usDieType) == TRUE)
	{
		ManualDieCompenate(stDieOffset, fDieRotate);
		DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
	}

	//Display Message on HMI
	//DisplaySearchDieResult(usDieType, m_bSelectDieType , m_lCurRefDieNo ,fDieRotate, stDieOffset, fDieScore);
	DisplaySearchDieResult(usDieType, BPR_NORMAL_DIE , BPR_NORMAL_DIE ,fDieRotate, stDieOffset, fDieScore);

	//On Joystick & Clear PR screen
	SetBinTableJoystick(TRUE);
	SelectBondCamera();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::UserSearchDie_NoMove(IPC_CServiceMessage& svMsg)
{
	PR_UWORD		usDieType;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;

	if (m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE)
	{
		SetAlert(IDS_BPR_DIENOTLEARNT);
		SetStatusMessage("F1: Bond PR no die record");
		SetErrorMessage("BPR no die record in UserSearchDie");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}
	if (m_bDieCalibrated == FALSE)	//v4.48A8
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not calibrated");
		SetErrorMessage("Bond PR calibration factor missing in UserSearchDie");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}

	LONG lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	if (lInitPR != 1)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not initialized");
		SetErrorMessage("Bond PR is not initialized in UserSearchDie");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}


	//Clear PR screen & Off joystick
	SelectBondCamera();
	SetBinTableJoystick(FALSE);

	DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);

	LONG lDieNo = BPR_NORMAL_DIE;


	ManualSearchDie(BPR_NORMAL_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
						m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 3);	//v3.85

	typedef struct 
	{
		BOOL	bStatus;
		double	dOffsetX;
		double	dOffsetY;
		DOUBLE	dAngle;
	} SRCH_RESULT;

	SRCH_RESULT stResult;

	stResult.bStatus = FALSE;
	stResult.dOffsetX = 0;
	stResult.dOffsetY = 0;
	stResult.dAngle = fDieRotate;

	if (DieIsAlignable(usDieType) == TRUE)
	{
		stResult.bStatus = TRUE;

		int siStepX = 0, siStepY = 0;

		CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		stResult.dOffsetX = siStepX;
		stResult.dOffsetY = siStepY;

		DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
	}

	SetBinTableJoystick(TRUE);
	SelectBondCamera();

	svMsg.InitMessage(sizeof(SRCH_RESULT), &stResult);
	return 1;
}

LONG CBondPr::BT_GetPRStatus(IPC_CServiceMessage &svMsg)
{
	LONG	lInitPR;
    BOOL    bUsePr;

    lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

    if( m_bGenDieLearnt[BPR_NORMAL_DIE] ==TRUE && m_bDieCalibrated==TRUE && lInitPR==1 )
    {
        bUsePr = TRUE;
    }
    else
    {
        //HmiMessage("Bond PR not init, or die not learnt or die calibration undone");
		SetAlert(IDS_BPR_DIE_NOT_READY);
        bUsePr = FALSE;
    }

    svMsg.InitMessage(sizeof(BOOL), &bUsePr);

    return TRUE;
}


LONG CBondPr::BT_GetFOVSize(IPC_CServiceMessage &svMsg)
{
	LONG lInitPR;
    DOUBLE dSize = -1;

	typedef struct 
	{
	    LONG lPitchX;
	    LONG lPitchY;
	} BT_PITCH;
	BT_PITCH stInfo;

	svMsg.GetMsg(sizeof(BT_PITCH), &stInfo);

    lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

    if( m_bGenDieLearnt[BPR_NORMAL_DIE] ==TRUE && m_bDieCalibrated==TRUE && lInitPR==1 )
    {
		//Calculate this blk FOV
		PR_COORD stTmpDiePixel;
		int	siMarginPixel = 20;
		int	siDieInField = 0;
		int	siXPos;
		int	siYPos;

		DOUBLE	dRowDieCount = 0;
		DOUBLE	dColDieCount = 0;
		DOUBLE	dDieInField = 0;

		//Convert Pixel into Motor step
		LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
		stTmpDiePixel.x = PR_MAX_WIN_LRC_X - PR_MAX_WIN_ULC_X - (m_stGenDieSize[lDieNo].x / 2) - siMarginPixel;
		stTmpDiePixel.y = PR_MAX_WIN_LRC_Y - PR_MAX_WIN_ULC_Y - (m_stGenDieSize[lDieNo].y / 2) - siMarginPixel;
		ConvertPixelToMotorStep(stTmpDiePixel, &siXPos, &siYPos);

		m_lDiePitchX	= stInfo.lPitchX;
		m_lDiePitchY	= stInfo.lPitchY;

		dRowDieCount	= ((DOUBLE)abs(siXPos) / (DOUBLE)abs((int)m_lDiePitchX)) + 1;
		dColDieCount	= ((DOUBLE)abs(siYPos) / (DOUBLE)abs((int)m_lDiePitchY)) + 1;
		dDieInField		= min(dRowDieCount, dColDieCount);
		dSize			= dDieInField;
	}
    else
    {
        //HmiMessage("Bond PR not init, or die not learnt or die calibration undone");
		SetAlert(IDS_BPR_DIE_NOT_READY);
        dSize = -1;
    }

    svMsg.InitMessage(sizeof(DOUBLE), &dSize);

    return TRUE;
}

VOID CBondPr::BT_ChangeSearchRange(PR_WIN &stSearchArea, const DOUBLE dRatio)
{
	DOUBLE dX = stSearchArea.coCorner2.x - stSearchArea.coCorner1.x;
	DOUBLE dY = stSearchArea.coCorner2.y - stSearchArea.coCorner1.y;

	dX *= dRatio;
	dY *= dRatio;

	LONG lCenterX = _round((double)(stSearchArea.coCorner2.x + stSearchArea.coCorner1.x) /2);
	LONG lCenterY = _round((double)(stSearchArea.coCorner2.y + stSearchArea.coCorner1.y) /2);

	stSearchArea.coCorner1.x = (PR_WORD)(lCenterX - _round(dX / 2));
	if (stSearchArea.coCorner1.x < 256)
	{
		stSearchArea.coCorner1.x = 256;
	}
	stSearchArea.coCorner2.x = (PR_WORD)(lCenterX + _round(dX / 2));
	if (stSearchArea.coCorner2.x > 8192 - 256)
	{
		stSearchArea.coCorner2.x = 8192 - 256;
	}

	stSearchArea.coCorner1.y = (PR_WORD)(lCenterY - _round(dY / 2));
	if (stSearchArea.coCorner1.y < 256)
	{
		stSearchArea.coCorner1.y = 256;
	}
	stSearchArea.coCorner2.y = (PR_WORD)(lCenterY + _round(dY / 2));
	if (stSearchArea.coCorner2.y > 8192 - 256)
	{
		stSearchArea.coCorner2.y = 8192 - 256;
	}
}


LONG CBondPr::BT_SearchDie(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

	//PR_COORD stOrgSrchCorner1 = m_stSearchArea.coCorner1;
	//PR_COORD stOrgSrchCorner2 = m_stSearchArea.coCorner2;

    stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;

	//Use 1.5x search area to search die
/*
	LONG lSearchCodeX = 4;
	LONG lSearchCodeY = 4;
	m_stSearchArea.coCorner1.x = PR_DEF_CENTRE_X - (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stDieSize.x );
	m_stSearchArea.coCorner1.y = PR_DEF_CENTRE_Y - (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stDieSize.y );
	m_stSearchArea.coCorner2.x = PR_DEF_CENTRE_X + (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stDieSize.x );
	m_stSearchArea.coCorner2.y = PR_DEF_CENTRE_Y + (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stDieSize.y );
	VerifyPRRegion(&m_stSearchArea);
*/
	//DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_GREEN);


	//Disable Die inspection on here
	if (ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 4) == -1 )		//v3.85
	{
        stInfo.bResult = FALSE;
    }

	m_dPrDieCenterX = 0;
	m_dPrDieCenterY = 0;
	if (DieIsAlignable(usDieType) == TRUE)
	{
		m_dPrDieCenterX = stDieOffset.x;
		m_dPrDieCenterY = stDieOffset.y;
	    CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
		//DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
        stInfo.bResult = TRUE;
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}

LONG CBondPr::BT_LargeAreaSearchDie(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

	//PR_COORD stOrgSrchCorner1 = m_stSearchArea.coCorner1;
	//PR_COORD stOrgSrchCorner2 = m_stSearchArea.coCorner2;

    stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;

	//Use 1.5x search area to search die
/*
	LONG lSearchCodeX = 4;
	LONG lSearchCodeY = 4;
	m_stSearchArea.coCorner1.x = PR_DEF_CENTRE_X - (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stDieSize.x );
	m_stSearchArea.coCorner1.y = PR_DEF_CENTRE_Y - (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stDieSize.y );
	m_stSearchArea.coCorner2.x = PR_DEF_CENTRE_X + (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stDieSize.x );
	m_stSearchArea.coCorner2.y = PR_DEF_CENTRE_Y + (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stDieSize.y );
	VerifyPRRegion(&m_stSearchArea);
*/
	//DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_GREEN);

	LONG lSearchCodeX = 2;
	LONG lSearchCodeY = 2;
	PR_WIN stWIN = m_stSearchArea;
	LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
	stWIN.coCorner1.x = m_stSearchArea.coCorner1.x  - (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].x );
	stWIN.coCorner1.y = m_stSearchArea.coCorner1.y  - (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].y );
	stWIN.coCorner2.x = m_stSearchArea.coCorner2.x  + (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].x );
	stWIN.coCorner2.y = m_stSearchArea.coCorner2.y  + (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].y );
	CString szLog;
	szLog.Format("LargeAreaSearchDie,%d,%d,%d,%d,Die Size X:%ld, Die Size Y:%ld",stWIN.coCorner1.x,stWIN.coCorner1.y,stWIN.coCorner2.x,stWIN.coCorner2.y,m_stGenDieSize[BPR_NORMAL_DIE].x,m_stGenDieSize[BPR_NORMAL_DIE].y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	//Disable Die inspection on here
	if ( ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stWIN.coCorner1, stWIN.coCorner2, 0, 5) == -1 )		//v3.85
	{
        stInfo.bResult = FALSE;
    }

	m_dPrDieCenterX = 0;
	m_dPrDieCenterY = 0;
	if (DieIsAlignable(usDieType) == TRUE)
	{
		m_dPrDieCenterX = stDieOffset.x;
		m_dPrDieCenterY = stDieOffset.y;
	    CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
		//DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
        stInfo.bResult = TRUE;
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}


//v4.58A5	//shiraishi02
LONG CBondPr::BT_SearchRefDie2(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

    stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;

	PR_WIN stSearchArea = m_stSearchArea;
	GetSearchDieArea(&stSearchArea, BPR_REFERENCE_DIE + BPR_GEN_RDIE_OFFSET);
	
	PR_COORD	stSrchCorner1 = {PR_MAX_WIN_ULC_X, PR_MAX_WIN_ULC_Y};
	PR_COORD	stSrchCorner2 = {PR_MAX_WIN_LRC_X, PR_MAX_WIN_LRC_Y};

	VerifyPRRegion(&stSearchArea);

	//Disable Die inspection on here
	if ( ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchCorner1, stSrchCorner2, 0, 6) == -1 )	
	{
		SetErrorMessage("BT_SearchRefDie2 RPY1 fail");
		stInfo.bResult = FALSE;

    }
	else
	{	
		if (DieIsAlignable(usDieType) == TRUE)
		{
			CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
			stInfo.bResult = TRUE;
		}
		else
		{
			CString szLog;
			szLog.Format("BT_SearchRefDie2 result fail; DieType = %d", usDieType);
			SetErrorMessage(szLog);
			stInfo.bResult = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}

LONG CBondPr::BT2_SearchDie(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

	//PR_COORD stOrgSrchCorner1 = m_stSearchArea.coCorner1;
	//PR_COORD stOrgSrchCorner2 = m_stSearchArea.coCorner2;

    stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;

	if (!m_bDieCalibrated2)
	{
		CString szContent;
		szContent.LoadString(HMB_BPR_BOND_PR_NOT_SETUP_PROPERLY);
		HmiMessage(szContent, "BT2_SearchDie");
		svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
		return 1;
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}


LONG CBondPr::BT_SearchDieWithAngle(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    DOUBLE	dX;
		DOUBLE	dY;
		DOUBLE	dDegree;
    	BOOL    bResult;
	
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	int			siStepX = 0;
	int			siStepY = 0;

    stInfo.bResult = FALSE;
    stInfo.dX		= 0.00;
    stInfo.dY		= 0.00;
	stInfo.dDegree	= 0.00;


	//Disable Die inspection on here
	if ( ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 7) != -1 )	
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
			ConvertUnit(siStepX, siStepY, &stInfo.dX, &stInfo.dY);	//Covert from motor steps to MIL

			DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
			stInfo.bResult = TRUE;
			stInfo.dDegree = (DOUBLE) fDieRotate;
		}
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}


LONG CBondPr::BT_SearchDieInFOV(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		LONG	lDirection;
		LONG	lLookAheadDieNum;
		BOOL    bResult;
		int		iPrX;
		int		iPrY;
	} BPR_SrchDie;
	
	BPR_SrchDie stInfo;
	BPR_SrchDie stResult;

	BOOL bIsGoodDie = FALSE;
	PR_BOOLEAN bLatch = PR_TRUE;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_COORD stSrchCenter; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;

	CTmpChange<PR_WIN> stOrgSrch(&m_stSearchArea, m_stSearchArea);

	svMsg.GetMsg(sizeof(BPR_SrchDie), &stInfo);

	//Calcualte search area base on direction	
	CalculateLFSearchArea(stInfo.lDirection, stInfo.lLookAheadDieNum, &m_stSearchArea);
	if (!VerifyPRRegion(&m_stSearchArea))
	{
		stResult.bResult	= FALSE;
		stResult.iPrX		= 0;
		stResult.iPrY		= 0;
		svMsg.InitMessage(sizeof(BPR_SrchDie), &stResult);
		return 1;
	}

	//Use 1.5x search area to search die
	LONG lSearchCodeX = 4;
	LONG lSearchCodeY = 4;

	if ( stInfo.lDirection == 0 )
	{
		bLatch = PR_TRUE;
		stSrchCenter.x = (PR_WORD)m_lPrCenterX;
		stSrchCenter.y = (PR_WORD)m_lPrCenterY;
	}
	else
	{
		bLatch = PR_FALSE;
		stSrchCenter.x = (m_stSearchArea.coCorner2.x + m_stSearchArea.coCorner1.x) / 2 + stInfo.iPrX;
		stSrchCenter.y = (m_stSearchArea.coCorner2.y + m_stSearchArea.coCorner1.y) / 2 + stInfo.iPrY;
	}

	//v4.35T1	//PLLM MS109
/*
	//Update search area
	LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
	m_stSearchArea.coCorner1.x = stSrchCenter.x - (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].x );
	m_stSearchArea.coCorner1.y = stSrchCenter.y - (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].y );
	m_stSearchArea.coCorner2.x = stSrchCenter.x + (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].x );
	m_stSearchArea.coCorner2.y = stSrchCenter.y + (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].y );
	VerifyPRRegion(&m_stSearchArea);
*/

	//Search Die -- Disable Die inspection on here
	//Klocwork
	ULONG ulSearchDie = 0;
	ulSearchDie = (ULONG) ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), bLatch, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
												m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 8);
	if ( ulSearchDie == -1 )	//v3.85
    {
        bIsGoodDie = FALSE;
    }

	if (DieIsAlignable(usDieType) == TRUE)
	{
        bIsGoodDie = TRUE;
	}

	//Restore current search area
//	m_stSearchArea.coCorner1 = stOrgSrchCorner1;
//	m_stSearchArea.coCorner2 = stOrgSrchCorner2;

	stResult.lDirection	= stInfo.lDirection;
	stResult.bResult	= bIsGoodDie;
	stResult.iPrX		= stDieOffset.x;
	stResult.iPrY		= stDieOffset.y;

	//svMsg.InitMessage(sizeof(BOOL), &bIsGoodDie);
	svMsg.InitMessage(sizeof(BPR_SrchDie), &stResult);		//v4.26T1	//Semitek
	return 1;
}


LONG CBondPr::BT_SearchDieXYInFOV(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		LONG	lDirection;
		LONG	lLookAheadDieNum;
    	BOOL    bResult;
		int		iPrX;
		int		iPrY;
		int		lSearchRangeRatio;
		BOOL	bLookAheadDieOffset;
	} BPR_SrchDie;
	BPR_SrchDie stInfo;
	BPR_SrchDie stResult;

	BOOL bIsGoodDie = FALSE;
	PR_BOOLEAN bLatch = PR_TRUE;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_COORD stSrchCenter; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;
	int siStepX = 0, siStepY = 0;

	CTmpChange<PR_WIN> stOrgSrch(&m_stSearchArea, m_stSearchArea);
//	PR_COORD stOrgSrchCorner1 = m_stSearchArea.coCorner1;                                                
//	PR_COORD stOrgSrchCorner2 = m_stSearchArea.coCorner2;

	svMsg.GetMsg(sizeof(BPR_SrchDie), &stInfo);

	//Calcualte search area base on direction	
	CalculateLFSearchArea(stInfo.lDirection, stInfo.lLookAheadDieNum, &m_stSearchArea);
	if (!VerifyPRRegion(&m_stSearchArea))
	{
		stResult.bResult	= FALSE;
		stResult.iPrX		= 0;
		stResult.iPrY		= 0;
		svMsg.InitMessage(sizeof(BPR_SrchDie), &stResult);
		return 1;
	}

	//Use 1.5x search area to search die
	LONG lSearchCodeX = 4;
	LONG lSearchCodeY = 4;

	if (stInfo.lSearchRangeRatio > 1)
	{
		lSearchCodeX *= stInfo.lSearchRangeRatio;
		lSearchCodeY *= stInfo.lSearchRangeRatio;
	}

	if ( stInfo.lDirection == 0 )
	{
		bLatch = PR_TRUE;
		stSrchCenter.x = (PR_WORD)m_lPrCenterX;
		stSrchCenter.y = (PR_WORD)m_lPrCenterY;
	}
	else
	{
		bLatch = PR_FALSE;
		stSrchCenter.x = (m_stSearchArea.coCorner2.x + m_stSearchArea.coCorner1.x) / 2 + stInfo.iPrX;
		stSrchCenter.y = (m_stSearchArea.coCorner2.y + m_stSearchArea.coCorner1.y) / 2 + stInfo.iPrY;
	}

	//Update search area
	LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
	m_stSearchArea.coCorner1.x = stSrchCenter.x - (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].x );
	m_stSearchArea.coCorner1.y = stSrchCenter.y - (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].y );
	m_stSearchArea.coCorner2.x = stSrchCenter.x + (PR_WORD)((lSearchCodeX*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].x );
	m_stSearchArea.coCorner2.y = stSrchCenter.y + (PR_WORD)((lSearchCodeY*1.0/4 + 0.5) * m_stGenDieSize[lDieNo].y );
	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "AOT" || pApp->GetCustomerName() == "ABC") //ABC is used in AOT
	{
		PR_WIN stOldSearchArea;
		stOldSearchArea.coCorner1.x = m_stSearchArea.coCorner1.x;
		stOldSearchArea.coCorner1.y = m_stSearchArea.coCorner1.y;
		stOldSearchArea.coCorner2.x = m_stSearchArea.coCorner2.x;
		stOldSearchArea.coCorner2.y = m_stSearchArea.coCorner2.y;

		if (!VerifyPRRegion(&m_stSearchArea))
		{
			CString szLog;
			szLog.Format("BT_SearchDieXYInFOV-Set Search Die Area from (%d,%d) and (%d,%d) to (%d,%d) and (%d,%d)",
				(LONG)stOldSearchArea.coCorner1.x, (LONG)stOldSearchArea.coCorner1.y,
				(LONG)stOldSearchArea.coCorner2.x, (LONG)stOldSearchArea.coCorner2.y,
				(LONG)m_stSearchArea.coCorner1.x, (LONG)m_stSearchArea.coCorner1.y,
				(LONG)m_stSearchArea.coCorner2.x, (LONG)m_stSearchArea.coCorner2.y);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}
	}
	else
	{
		if (!VerifyPRRegion(&m_stSearchArea))
		{
			stResult.bResult	= FALSE;
			stResult.iPrX		= 0;
			stResult.iPrY		= 0;
			svMsg.InitMessage(sizeof(BPR_SrchDie), &stResult);
			return 1;
		}
	}

	//Search Die -- Disable Die inspection on here
	//Klocwork
	ULONG ulSearchDie = 0;
	ulSearchDie = ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), bLatch, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore,
								m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 9);
	if ( ulSearchDie == -1 )
    {
        bIsGoodDie = FALSE;
    }

	if (DieIsAlignable(usDieType) == TRUE)
	{
		LONG lDiePosnOffsetX = 0;
		LONG lDiePosnOffsetY = 0;
		if (stInfo.bLookAheadDieOffset)
		{
			CalculateLFDiePosn(stInfo.lDirection, stInfo.lLookAheadDieNum, lDiePosnOffsetX, lDiePosnOffsetY);
			stDieOffset.x -= (PR_WORD)lDiePosnOffsetX;
			stDieOffset.y -= (PR_WORD)lDiePosnOffsetY;
		}

	    CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
        bIsGoodDie = TRUE;
	}

	//Restore current search area
//	m_stSearchArea.coCorner1 = stOrgSrchCorner1;
//	m_stSearchArea.coCorner2 = stOrgSrchCorner2;

	stResult.bResult	= bIsGoodDie;
	stResult.iPrX		= siStepX;
	stResult.iPrY		= siStepY;

	svMsg.InitMessage(sizeof(BPR_SrchDie), &stResult);
	return 1;
}

LONG CBondPr::BT2_SearchDieXYInFOV(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		LONG	lDirection;
		LONG	lLookAheadDieNum;
    	BOOL    bResult;
		int		iPrX;
		int		iPrY;
	} BPR_SrchDie;
	BPR_SrchDie stInfo;
	BPR_SrchDie stResult;

	BOOL bIsGoodDie = FALSE;
	PR_BOOLEAN bLatch = PR_TRUE;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_COORD stSrchCenter; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;
	int siStepX=0, siStepY=0;

	svMsg.GetMsg(sizeof(BPR_SrchDie), &stInfo);

	svMsg.InitMessage(sizeof(BPR_SrchDie), &stResult);
	return 1;
}


LONG CBondPr::BT_SearchDieInMaxFOV(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;

	PR_COORD stOrgSrchCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stOrgSrchCorner2 = m_stSearchArea.coCorner2;
	PR_COORD	stSrchCorner1 = {PR_MAX_WIN_ULC_X, PR_MAX_WIN_ULC_Y};
	PR_COORD	stSrchCorner2 = {PR_MAX_WIN_LRC_X, PR_MAX_WIN_LRC_Y};
	m_stSearchArea.coCorner1 = stSrchCorner1;
	m_stSearchArea.coCorner2 = stSrchCorner2;
    
	stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;

	DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);

	//Disable Die inspection on here
	if ( ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 10) == -1 )		//v3.85
	{
        stInfo.bResult = FALSE;
    }

	if (DieIsAlignable(usDieType) == TRUE)
	{
	    CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
		DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
        stInfo.bResult = TRUE;
	}

	DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);

	//Restore current search area
	m_stSearchArea.coCorner1 = stOrgSrchCorner1;
	m_stSearchArea.coCorner2 = stOrgSrchCorner2;

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}

LONG CBondPr::BT_SearchRefDieInFOV(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;

	PR_COORD	stSrchCorner1 = {PR_MAX_WIN_ULC_X, PR_MAX_WIN_ULC_Y};
	PR_COORD	stSrchCorner2 = {PR_MAX_WIN_LRC_X, PR_MAX_WIN_LRC_Y};

	stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;

	//Disable Die inspection on here
	if ( ManualSearchDie(BPR_REFERENCE_DIE, 1, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchCorner1, stSrchCorner2, 0, 11) == -1 )		//v3.85
	{
        stInfo.bResult = FALSE;
    }
	else if (DieIsAlignable(usDieType) == TRUE)
	{
	    CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
        stInfo.bResult = TRUE;
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}

LONG CBondPr::BT_SearchRefDie2InFOV(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
		DOUBLE	dAngle;		//v4.59A4
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;

	PR_COORD	stSrchCorner1 = {PR_MAX_WIN_ULC_X, PR_MAX_WIN_ULC_Y};
	PR_COORD	stSrchCorner2 = {PR_MAX_WIN_LRC_X, PR_MAX_WIN_LRC_Y};

	stInfo.bResult = FALSE;
    stInfo.siStepX	= 0;
    stInfo.siStepY	= 0;
	stInfo.dAngle	= 0;

	//Disable Die inspection on here
	if ( ManualSearchDie(BPR_REFERENCE_DIE, 2, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchCorner1, stSrchCorner2, 0, 12) == -1 )
	{
        stInfo.bResult = FALSE;
    }
	else if (DieIsAlignable(usDieType) == TRUE)
	{
	    CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
        stInfo.bResult	= TRUE;
		stInfo.dAngle	= fDieRotate;		//v4.59A4
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}

LONG CBondPr::BT_SearchRefDieErrMap(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int	siStepX;
	    int	siStepY;
		int siPixelX;
	    int	siPixelY;
		DOUBLE	dAngle;	
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD stDieOffset; 
	PR_REAL		fDieRotate = 0; 
	PR_REAL		fDieScore = 0;

	PR_COORD	stSrchCorner1 = {PR_MAX_WIN_ULC_X, PR_MAX_WIN_ULC_Y};
	PR_COORD	stSrchCorner2 = {PR_MAX_WIN_LRC_X, PR_MAX_WIN_LRC_Y};

	stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;
	stInfo.dAngle	= 0;

	if (!IsPRLearnt(BPR_REF_SRCH_ERR_MAP))
	{
		HmiMessage("BPR: BT_SearchRefDieErrMap fail - pls learn BPR REF Record 8");
        stInfo.bResult = FALSE;
		svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
		return 1;
	}

	//Disable Die inspection on here
	if ( ManualSearchDie(BPR_REFERENCE_DIE, BPR_REF_SRCH_ERR_MAP, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchCorner1, stSrchCorner2) == -1 )
	{
		HmiMessage("BPR: BT_SearchRefDieErrMap fail - SearchFails");
        stInfo.bResult = FALSE;
    }
	else if (DieIsAlignable(usDieType) == TRUE)
	{
		//HmiMessage("AlignFails");
	    CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
		
		//stInfo.siPixelX = m_lPrCenterX - stDieOffset.x;
		//stInfo.siPixelY = m_lPrCenterY - stDieOffset.y;
		stInfo.siPixelX	= stDieOffset.x;
		stInfo.siPixelY	= stDieOffset.y;

        stInfo.bResult	= TRUE;
		stInfo.dAngle	= fDieRotate;
	}

	CString szMsg;
	szMsg.Format("BPR: Search ErrMap PR Result,%d,%d", stInfo.siStepX, stInfo.siStepY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}

LONG CBondPr::BT_SearchFrameAngleInFOV(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		DOUBLE	dAngle;
		BOOL    bResult;

	} BPR_FRAMEANGLE;
	BPR_FRAMEANGLE stInfo;

	DOUBLE dAngle	= 0;
	BOOL bReturn	= SearchFrameEdgeAngle(dAngle);
	stInfo.bResult	= bReturn;
	stInfo.dAngle	= dAngle;

	svMsg.InitMessage(sizeof(BPR_FRAMEANGLE), &stInfo);
	return 1;
}

LONG CBondPr::DrawSearchWindow(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bDisplay = FALSE;
	INT nDirection = 1;
	BOOL bLF = FALSE;
	//svMsg.GetMsg(sizeof(BOOL), &bDisplay);

	typedef struct 
	{
	    BOOL bDisplay;
	    BOOL bLF;
		LONG lDirection;
	} BPR_DISPLAY_SA;
	BPR_DISPLAY_SA stInfo;
	svMsg.GetMsg(sizeof(BPR_DISPLAY_SA), &stInfo);

	bDisplay	= stInfo.bDisplay;
	bLF			= stInfo.bLF;
	nDirection	= stInfo.lDirection;

	m_pPrGeneral->PRClearScreenNoCursor(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID);

	PR_COORD stPrMsg;
	stPrMsg.x = 1;
	stPrMsg.y = 1;


	// Display main search window
	if (bDisplay)
		DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_GREEN);
	else
		DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);

	// Display LF search window?
	if (bLF)
	{
		switch (nDirection)
		{
			default:
			case BPR_CT_DIE:
				break;

			case BPR_UL_DIE: 
				DisplayLFSearchArea(BPR_UL_DIE, PR_COLOR_YELLOW);
				DisplayLFSearchArea(BPR_LT_DIE, PR_COLOR_YELLOW);
				DisplayLFSearchArea(BPR_UP_DIE, PR_COLOR_YELLOW);
				break;

			case BPR_UR_DIE:
				DisplayLFSearchArea(BPR_UR_DIE, PR_COLOR_YELLOW);
				DisplayLFSearchArea(BPR_RT_DIE, PR_COLOR_YELLOW);
				DisplayLFSearchArea(BPR_UP_DIE, PR_COLOR_YELLOW);
				break;

			case BPR_DL_DIE:
				DisplayLFSearchArea(BPR_DL_DIE, PR_COLOR_YELLOW);
				DisplayLFSearchArea(BPR_LT_DIE, PR_COLOR_YELLOW);
				DisplayLFSearchArea(BPR_DN_DIE, PR_COLOR_YELLOW);
				break;

			case BPR_DR_DIE:
				DisplayLFSearchArea(BPR_DR_DIE, PR_COLOR_YELLOW);
				DisplayLFSearchArea(BPR_RT_DIE, PR_COLOR_YELLOW);
				DisplayLFSearchArea(BPR_DN_DIE, PR_COLOR_YELLOW);
				break;
		}
	}
	else
	{	
		if ( nDirection != BPR_CT_DIE )
		{
			DisplayLFSearchArea(nDirection, PR_COLOR_YELLOW);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::DrawDieSizeWindow(IPC_CServiceMessage& svMsg)
{
	BOOL bDisplayHomeCursor = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bDisplayHomeCursor);

	m_pPrGeneral->PRClearScreenNoCursor(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID);

	if (bDisplayHomeCursor)
	{
		//Draw Home cursor
		CString szErrMsg;
		m_pPrGeneral->DrawHomeCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, m_lPrCenterX, m_lPrCenterY, szErrMsg);
	}

	DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);

	svMsg.InitMessage(sizeof(BOOL), &bDisplayHomeCursor);
	return 1;
}



//================================================================
// BT_GetDieSize()
//   Created-By  : Andrew Ng
//   Date        : 5/18/2007 10:03:54 AM
//   Description : 
//   Remarks     : 
//================================================================
LONG CBondPr::BT_GetDieSize(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    LONG lX;
	    LONG lY;
	} BPR_DIESIZE;
	BPR_DIESIZE stInfo;

	//int nStepX=0, nStepY=0;
	//ConvertPixelToMotorStep(m_stDieSize, &nStepX, &nStepY);

	PR_COORD stPixel;
	stPixel.x = m_stGenDieSize[BPR_NORMAL_DIE].x;
	stPixel.y = m_stGenDieSize[BPR_NORMAL_DIE].y;
	LONG lX=0, lY=0;		//in mil
	ConvertPixelToUnit(stPixel, &lX, &lY);

	stInfo.lX = lX;
	stInfo.lY = lY;

	svMsg.InitMessage(sizeof(BPR_DIESIZE), &stInfo);
	return 1;
}


LONG CBondPr::HotKeySearchRefDie(IPC_CServiceMessage& svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	
	BOOL bPreBond	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];
	BOOL bCheckFrameMarkFcn = pApp->GetFeatureStatus(MS896A_FUNC_BIN_FRAME_CROSS);
	BOOL bCheckFrameOrientation = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["CheckFrameOrientation"];


	//if (!bPreBond && !bCheckFrameMarkFcn)
	if (!bPreBond && !bCheckFrameMarkFcn && !bCheckFrameOrientation)	//shiraishi02
	{
		HmiMessage_Red_Yellow("BOND Reference Die pattern not enabled.", "Bond PR");
		BOOL bTemp = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bTemp);
		return 1;
	}


	if (bPreBond && m_bPreBondAlignUseEpoxySearch)
	{
		if (m_ssPSPRSrchID[0] == 0)
		{
			SetAlert(IDS_BPR_DIENOTLEARNT);
			SetStatusMessage("F9: Bond PR no EPOXY die record");
			SetErrorMessage("BPR no EPOXY record in UserSearchRefDie");
			BOOL bTemp = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bTemp);
			return 1;
		}
	}
	else if (bCheckFrameMarkFcn && m_bCheckFrameAngleByHwMark)	//v4.59A1
	{
HmiMessage("Searching frame edge ......");

		DOUBLE dAngle = 0;
		BOOL bReturn	= SearchFrameEdgeAngle(dAngle);
		if (bReturn)
		{
			CString szMsg;
			szMsg.Format("Frame angle = %.2f degree", dAngle);
			HmiMessage(szMsg, "Bond PR");

			IPC_CServiceMessage stMsg;
			stMsg.InitMessage(sizeof(DOUBLE), &dAngle);

			BOOL bResult = TRUE;
			/*INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "RotateFrameAngle", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}*/

			if (bResult)
			{

			}
		}
	}
	else
	{
		if (m_bGenDieLearnt[BPR_REFERENCE_DIE + BPR_GEN_RDIE_OFFSET] == FALSE)
		{
			SetAlert(IDS_BPR_DIENOTLEARNT);
			SetStatusMessage("F9: Bond PR no die record");
			SetErrorMessage("BPR no die record in UserSearchRefDie");
			BOOL bTemp = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bTemp);
			return 1;
		}

		if (m_bDieCalibrated == FALSE)
		{
			SetAlert(IDS_BPR_DIE_NOT_READY);
			SetStatusMessage("F9: Bond PR is not calibrated");
			SetErrorMessage("Bond PR calibration factor missing in UserSearchRefDie");
			BOOL bTemp = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bTemp);
			return 1;
		}
	}

	//Clear PR screen & Off joystick
	SelectBondCamera();
	SetBinTableJoystick(FALSE);


	CString szMsg;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;

	PR_WIN stSearchArea;
	GetSearchDieArea(&stSearchArea, BPR_REFERENCE_DIE + BPR_GEN_RDIE_OFFSET);		//v3.85
	VerifyPRRegion(&stSearchArea);		//v3.85
	DrawRectangleBox(stSearchArea.coCorner1, stSearchArea.coCorner2, PR_COLOR_RED);

	//v4.58A4
	if (bPreBond && m_bPreBondAlignUseEpoxySearch)
	{
		UINT nPosX = 0, nPosY = 0;
		ULONG ulEpoxySize = 0;

		BOOL bReturn = AutoSearchPreBondEpoxy(FALSE, TRUE, 0, 0, nPosX, nPosY, ulEpoxySize); 
		if (!bReturn)
			szMsg = "EPOXY is not found";
		else
			szMsg.Format("EPOXY is found at (%d, %d), size = %lu", nPosX, nPosY, ulEpoxySize);
		HmiMessage(szMsg);
	}
	else if (bCheckFrameMarkFcn && m_bCheckFrameAngleByHwMark)
	{
	}
	else
	{
		ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore,
							stSearchArea.coCorner1, stSearchArea.coCorner2, 0, 13);	//v3.85
		
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate(stDieOffset);
		}

		DisplaySearchDieResult(usDieType, BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, fDieRotate, stDieOffset, fDieScore);
	}


	//On Joystick & Clear PR screen
	SetBinTableJoystick(TRUE);
	SelectBondCamera();


	BOOL bTemp = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bTemp);
	return 1;
}


LONG CBondPr::SearchPreBondPattern(IPC_CServiceMessage& svMsg)
{
	//CSP
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
		BOOL	bIsBHArm2;	// CSP003c
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	
	BPR_DIEOFFSET stInfo;
    stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;
    stInfo.iPrX = 0;
    stInfo.iPrY = 0;
	
	BOOL bBTAtBH1Pos	= (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["BinTable At BH1"];		//andrewng
	//stInfo.bIsBHArm2 = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
	if (bBTAtBH1Pos)
	{
		stInfo.bIsBHArm2 = FALSE;
	}
	else
	{
		stInfo.bIsBHArm2 = TRUE;
	}

	CString szMsg;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;
	PR_WIN stSearchArea;

	GetSearchDieArea(&stSearchArea, BPR_REFERENCE_DIE + BPR_GEN_RDIE_OFFSET);
	VerifyPRRegion(&stSearchArea);

	PR_COORD stCollet2BondCorner1 = stSearchArea.coCorner1;
	PR_COORD stCollet2BondCorner2 = stSearchArea.coCorner2;
	PR_COORD stCollet1BondCorner1 = stSearchArea.coCorner1;
	PR_COORD stCollet1BondCorner2 = stSearchArea.coCorner2;

	LONG lCollet1OffsetX	= 0, lCollet1OffsetY	= 0;
	LONG lCollet2OffsetX	= 0, lCollet2OffsetY	= 0;
	GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, FALSE);

	DOUBLE dCalibX = GetCalibX();
	DOUBLE dCalibY = GetCalibY();
	if (m_bEnableMS100EjtXY /*&& m_bBTIsMoveCollet*/)
	{
		if ((fabs(dCalibX) > 0.0000001) && (fabs(dCalibY) > 0.0000001))		//v4.48A30
		{
			stCollet2BondCorner1.x = stCollet2BondCorner1.x - (PR_WORD)_round(lCollet2OffsetX / dCalibX);
			stCollet2BondCorner1.y = stCollet2BondCorner1.y - (PR_WORD)_round(lCollet2OffsetY / dCalibY);
			stCollet2BondCorner2.x = stCollet2BondCorner2.x - (PR_WORD)_round(lCollet2OffsetX / dCalibX);
			stCollet2BondCorner2.y = stCollet2BondCorner2.y - (PR_WORD)_round(lCollet2OffsetY / dCalibY);

			stCollet1BondCorner1.x = stCollet1BondCorner1.x - (PR_WORD)_round(lCollet1OffsetX / dCalibX);
			stCollet1BondCorner1.y = stCollet1BondCorner1.y - (PR_WORD)_round(lCollet1OffsetY / dCalibY);
			stCollet1BondCorner2.x = stCollet1BondCorner2.x - (PR_WORD)_round(lCollet1OffsetX / dCalibX);
			stCollet1BondCorner2.y = stCollet1BondCorner2.y - (PR_WORD)_round(lCollet1OffsetY / dCalibY);
		}
	}

	//No inspection on here!
	PR_WORD nResult = 0;
	INT nCount = 1;

//TakeTime(LC2);

	if (m_bPreBondAlignUseEpoxySearch)	//v4.58A4
	{
		UINT nPosX = 0, nPosY = 0;
		ULONG ulEpoxySize = 0;
		BOOL bReturn = AutoSearchPreBondEpoxy(TRUE, TRUE, 0, 0, nPosX, nPosY, ulEpoxySize); 

		if (!bReturn)
		{
			stInfo.bResult = FALSE;
			CMSLogFileUtility::Instance()->MS_LogOperation("BH PREBOND fail (Epoxy)");
		}
		else
		{
			stDieOffset.x = nPosX;
			stDieOffset.y = nPosY;

			CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);

			stInfo.bResult	= TRUE;
			stInfo.iPrX		= stDieOffset.x - m_lPrCenterX;
			stInfo.iPrY		= stDieOffset.y - m_lPrCenterY;

			int nOrigX = stInfo.siStepX;
			int nOrigY = stInfo.siStepY;

			if (m_bEnableMS100EjtXY)	// CSP003c
			{
				LONG lCollet1OffsetX	= 0, lCollet1OffsetY	= 0;
				LONG lCollet2OffsetX	= 0, lCollet2OffsetY	= 0;
				GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, FALSE);

				if(stInfo.bIsBHArm2)
				{
					//andrewng1234
					//BT XY encoder resolution chagned from 0.5 to 0.1 um
					lCollet2OffsetX = lCollet2OffsetX * 5;
					lCollet2OffsetY = lCollet2OffsetY * 5;

					stInfo.siStepX = stInfo.siStepX - lCollet2OffsetX; //CSP006b3	//andrewng12345
					stInfo.siStepY = stInfo.siStepY - lCollet2OffsetY;				//andrewng12345

					szMsg.Format("BH2 PREBOND EPOXY Offset - PadCenter(InPixel)(%d, %d), Orig(%d, %d), C2-Offset(%d, %d), Final-Offset(%d, %d)",	
									stDieOffset.x, stDieOffset.y, 
									nOrigX, nOrigY, lCollet2OffsetX, lCollet2OffsetY, 
									stInfo.siStepX, stInfo.siStepY);
					//AfxMessageBox(szMsg, MB_SYSTEMMODAL);
					//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);	
				}
				else
				{
					//andrewng1234
					//BT XY encoder resolution chagned from 0.5 to 0.1 um
					lCollet1OffsetX = lCollet1OffsetX * 5;
					lCollet1OffsetY = lCollet1OffsetY * 5;

					stInfo.siStepX = stInfo.siStepX - lCollet1OffsetX;	//andrewng12345
					stInfo.siStepY = stInfo.siStepY - lCollet1OffsetY;	//andrewng12345

					szMsg.Format("BH1 PREBOND EPOXY Offset - PadCenter(InPixel)(%d, %d), Orig(%d, %d), C1-Offset(%d, %d), Final-Offset(%d, %d)", 
									stDieOffset.x, stDieOffset.y, 
									nOrigX, nOrigY, 
									lCollet1OffsetX, lCollet1OffsetY, 
									stInfo.siStepX, stInfo.siStepY);
					//AfxMessageBox(szMsg, MB_SYSTEMMODAL);
					//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);	

				}
			}
		}
	}
	else
	{
		do	
		{
			nCount--;
	
			nResult = ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, 
										&usDieType, &fDieRotate, &stDieOffset, &fDieScore,
										stSearchArea.coCorner1, stSearchArea.coCorner2, 0, 14);
			//}
	
			if ( nResult == -1 )		//v3.85
			{
				stInfo.bResult = FALSE;
				CMSLogFileUtility::Instance()->MS_LogOperation("BH PREBOND fail");
			}
			else
			{
				if (!DieIsAlignable(usDieType))
				{
					stInfo.bResult = FALSE;
	
					if (stInfo.bIsBHArm2)
						CMSLogFileUtility::Instance()->MS_LogOperation("BH2 PREBOND fail");
					else
						CMSLogFileUtility::Instance()->MS_LogOperation("BH1 PREBOND fail");
				}
				else
				{
					CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
					//DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
					stInfo.bResult	= TRUE;
					stInfo.iPrX		= stDieOffset.x - m_lPrCenterX;
					stInfo.iPrY		= stDieOffset.y - m_lPrCenterY;
	
					int nOrigX = stInfo.siStepX;
					int nOrigY = stInfo.siStepY;
	
					if (m_bEnableMS100EjtXY)	// CSP003c
					{
						LONG lCollet1OffsetX	= 0, lCollet1OffsetY	= 0;
						LONG lCollet2OffsetX	= 0, lCollet2OffsetY	= 0;
						GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, FALSE);

						if(stInfo.bIsBHArm2)
						{
							//andrewng1234
							//BT XY encoder resolution chagned from 0.5 to 0.1 um
							lCollet2OffsetX = lCollet2OffsetX * 5;
							lCollet2OffsetY = lCollet2OffsetY * 5;
	
							stInfo.siStepX = stInfo.siStepX - lCollet2OffsetX; //CSP006b3	//andrewng12345
							stInfo.siStepY = stInfo.siStepY - lCollet2OffsetY;				//andrewng12345
	
							szMsg.Format("BH2 PREBOND Offset - PadCenter(InPixel)(%d, %d), Orig(%d, %d), C2-Offset(%d, %d), Final-Offset(%d, %d)",	
											stDieOffset.x, stDieOffset.y, 
											nOrigX, nOrigY, lCollet2OffsetX, lCollet2OffsetY, 
											stInfo.siStepX, stInfo.siStepY);
							//AfxMessageBox(szMsg, MB_SYSTEMMODAL);
							//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);	
						}
						else
						{
							//andrewng1234
							//BT XY encoder resolution chagned from 0.5 to 0.1 um
							lCollet1OffsetX = lCollet1OffsetX * 5;
							lCollet1OffsetY = lCollet1OffsetY * 5;
	
							stInfo.siStepX = stInfo.siStepX - lCollet1OffsetX;	//andrewng12345
							stInfo.siStepY = stInfo.siStepY - lCollet1OffsetY;	//andrewng12345
	
							szMsg.Format("BH1 PREBOND Offset - PadCenter(InPixel)(%d, %d), Orig(%d, %d), C1-Offset(%d, %d), Final-Offset(%d, %d)", 
											stDieOffset.x, stDieOffset.y, 
											nOrigX, nOrigY, 
											lCollet1OffsetX, lCollet1OffsetY, 
											stInfo.siStepX, stInfo.siStepY);
							//AfxMessageBox(szMsg, MB_SYSTEMMODAL);
							//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);	
	
						}
					}
				}
			}
	
		} while (nCount > 0);
	}


	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;

/*
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;

	PR_WIN stSearchArea;
	//Use Max search area to locate 1st die
	//stSearchArea.coCorner1.x	= PR_MAX_WIN_ULC_X + 100;
	//stSearchArea.coCorner1.y	= PR_MAX_WIN_ULC_Y + 100;
	//stSearchArea.coCorner2.x	= PR_MAX_WIN_LRC_X - 100;
	//stSearchArea.coCorner2.y	= PR_MAX_WIN_LRC_Y - 100;
	GetSearchDieArea(&stSearchArea, BPR_REFERENCE_DIE + BPR_GEN_RDIE_OFFSET);		//v3.85
	VerifyPRRegion(&stSearchArea);		//v3.85

    stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;
    stInfo.iPrX = 0;
    stInfo.iPrY = 0;

	//No inspection on here!
	if( ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore,
							stSearchArea.coCorner1, stSearchArea.coCorner2) == -1 )		//v3.85
    {
        stInfo.bResult = FALSE;
    }

	if (DieIsAlignable(usDieType) == TRUE)
	{
	    CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
		//DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
        stInfo.bResult = TRUE;

        stInfo.iPrX = stDieOffset.x - m_lPrCenterX;
		stInfo.iPrY = stDieOffset.y - m_lPrCenterY;
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
*/
}

LONG CBondPr::LookForwardPreBondPattern(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
		BOOL	bIsBHArm2;	// CSP003c
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	
	BPR_DIEOFFSET stInfo;
    stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;
    stInfo.iPrX = 0;
    stInfo.iPrY = 0;
	
	BOOL bBTAtBH1Pos	= (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["BinTable At BH1"];		//andrewng
	//stInfo.bIsBHArm2 = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
	if (bBTAtBH1Pos)
	{
		stInfo.bIsBHArm2 = FALSE;
	}
	else
	{
		stInfo.bIsBHArm2 = TRUE;
	}

	CString szMsg;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;
	PR_WIN stSearchArea;
/*
	//calculate PR Centre offset
	LONG lBHZ2BondPosOffsetX = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"];
	LONG lBHZ2BondPosOffsetY = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"];
	LONG lBHZ1BondPosOffsetX = (*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX"];
	LONG lBHZ1BondPosOffsetY = (*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY"];
*/
	DOUBLE dCentreOffsetX = 0;
	DOUBLE dCentreOffsetY = 0;

	//1. Get REF DIE Search window size (at cursor center)
	GetSearchDieArea(&stSearchArea, BPR_REFERENCE_DIE + BPR_GEN_RDIE_OFFSET);
	VerifyPRRegion(&stSearchArea);

	//2. Calculate & update Srch Window by Collet 1/2 offset
//	PR_COORD stCorner1, stCorner2;
	PR_COORD stCollet2BondCorner1 = stSearchArea.coCorner1;
	PR_COORD stCollet2BondCorner2 = stSearchArea.coCorner2;
	PR_COORD stCollet1BondCorner1 = stSearchArea.coCorner1;
	PR_COORD stCollet1BondCorner2 = stSearchArea.coCorner2;

	LONG lCollet1OffsetX	= 0, lCollet1OffsetY	= 0;
	LONG lCollet2OffsetX	= 0, lCollet2OffsetY	= 0;
	GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, FALSE);

	//andrewng1234
	//BT XY encoder resolution chagned from 0.5 to 0.1 um
	lCollet1OffsetX = lCollet1OffsetX * 5;
	lCollet1OffsetY = lCollet1OffsetY * 5;
	lCollet2OffsetX = lCollet2OffsetX * 5;
	lCollet2OffsetY = lCollet2OffsetY * 5;

	DOUBLE dCalibX = GetCalibX();
	DOUBLE dCalibY = GetCalibY();

	if (m_bEnableMS100EjtXY /*&& m_bBTIsMoveCollet*/)
	{
		if ((fabs(dCalibX) > 0.0000001) && (fabs(dCalibY) > 0.0000001))		//v4.48A30
		{
			stCollet2BondCorner1.x = stCollet2BondCorner1.x - (PR_WORD)_round(lCollet2OffsetX / dCalibX);
			stCollet2BondCorner1.y = stCollet2BondCorner1.y - (PR_WORD)_round(lCollet2OffsetY / dCalibY);
			stCollet2BondCorner2.x = stCollet2BondCorner2.x - (PR_WORD)_round(lCollet2OffsetX / dCalibX);
			stCollet2BondCorner2.y = stCollet2BondCorner2.y - (PR_WORD)_round(lCollet2OffsetY / dCalibY);

			stCollet1BondCorner1.x = stCollet1BondCorner1.x - (PR_WORD)_round(lCollet1OffsetX / dCalibX);
			stCollet1BondCorner1.y = stCollet1BondCorner1.y - (PR_WORD)_round(lCollet1OffsetY / dCalibY);
			stCollet1BondCorner2.x = stCollet1BondCorner2.x - (PR_WORD)_round(lCollet1OffsetX / dCalibX);
			stCollet1BondCorner2.y = stCollet1BondCorner2.y - (PR_WORD)_round(lCollet1OffsetY / dCalibY);
		}
	}

	if (stInfo.bIsBHArm2)		//Arm 2
	{
		stSearchArea.coCorner1 = stCollet2BondCorner1;
		stSearchArea.coCorner2 = stCollet2BondCorner2;
	}
	else						//Arm1
	{
		stSearchArea.coCorner1 = stCollet1BondCorner1;
		stSearchArea.coCorner2 = stCollet1BondCorner2;
	}

	//3. Calculate corresponding LF window from Srch Window above
	PR_WIN stOldSrchArea = stSearchArea;
	PR_WIN stSrchArea2 = stSearchArea;
	PR_WIN stSrchArea3 = stSearchArea;
	PR_WIN stSrchArea4 = stSearchArea;

	CalculateLFSearchArea(BPR_LT_DIE, 1, &stSrchArea2);	//LF LEFT die
	VerifyPRRegion(&stSrchArea2);
	CalculateLFSearchArea(BPR_RT_DIE, 1, &stSrchArea3);	//LF RIGHT die
	VerifyPRRegion(&stSrchArea3);
	CalculateLFSearchArea(BPR_DN_DIE, 1, &stSrchArea4);	//LF DOWN die
	VerifyPRRegion(&stSrchArea4);

	LONG lSortDir = (*m_psmfSRam)["BinTable"]["SortDir"];	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN

	if (lSortDir == 1)		//LEFT
	{
		stSearchArea = stSrchArea2;
	}
	else if (lSortDir == 2)	//RIGHT
	{
		stSearchArea = stSrchArea3;
	}
	else if (lSortDir == 4)	//DOWN
	{
		stSearchArea = stSrchArea4;
	}
	else
	{
		stInfo.bResult	= FALSE;
		stInfo.siStepX	= 0;
		stInfo.siStepY	= 0;
		
		szMsg.Format("BPR LF PAD fail: SortDir=%d", lSortDir);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);	
		
		svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
		return 1;
	}

//TakeTime(LC2);	//v4.59A2

	//No inspection on here!
	PR_WORD nResult = 0;
	nResult = ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_FALSE, PR_TRUE, PR_FALSE, 
								&usDieType, &fDieRotate, &stDieOffset, &fDieScore,
								stSearchArea.coCorner1, stSearchArea.coCorner2, 0, 15);

	if ( nResult == -1 )
    {
        stInfo.bResult = FALSE;
		stInfo.siStepX	= 0;
		stInfo.siStepY	= 0;

		CMSLogFileUtility::Instance()->MS_LogOperation("BPR PREBOND LF fail");
    }
	else
	{
		if (!DieIsAlignable(usDieType))
		{
			stInfo.bResult = FALSE;
			stInfo.siStepX	= 0;
			stInfo.siStepY	= 0;

			szMsg.Format("BPR PREBOND LF fail: IsBH2=%d, PR-Result=%d, DieType=%d",
					stInfo.bIsBHArm2, nResult, usDieType);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);	

			//andrewng12
			CString szLog;
			szLog.Format("0,0,0,%d,%d,%d,%d,", 
				stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
				stSearchArea.coCorner2.x, stSearchArea.coCorner2.y);
			//CMSLogFileUtility::Instance()->BT_TableIndex2Log(szLog);	
		}
		else
		{
			CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);

			stInfo.bResult	= TRUE;
			stInfo.iPrX		= stDieOffset.x - m_lPrCenterX;
			stInfo.iPrY		= stDieOffset.y - m_lPrCenterY;

			int nOrigX = stInfo.siStepX;
			int nOrigY = stInfo.siStepY;

			if (m_bEnableMS100EjtXY)	// CSP003c
			{
				LONG lCollet1OffsetX	= 0, lCollet1OffsetY	= 0;
				LONG lCollet2OffsetX	= 0, lCollet2OffsetY	= 0;

				GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, FALSE);
				if(stInfo.bIsBHArm2)	//If current BT at BH2 position, next pos should be BH1
				{
					stInfo.siStepX = stInfo.siStepX - lCollet1OffsetX;
					stInfo.siStepY = stInfo.siStepY - lCollet1OffsetY;

					szMsg.Format("BPR PREBOND LF result (at BH2 in motor steps) - Dir = %d (%ld, %ld, %.3f, %.3f), Orig(%d, %d); C1-Offset(%d, %d); Final(%d, %d); SrchWnd(%ld, %ld, %ld, %ld); oldWnd(%ld, %ld, %ld, %ld)",	
									lSortDir, m_lDiePitchX, m_lDiePitchY, dCalibX, dCalibY,
									nOrigX, nOrigY, 
									lCollet1OffsetX, lCollet1OffsetY, 
									stInfo.siStepX, stInfo.siStepY,
									stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
									stSearchArea.coCorner2.x, stSearchArea.coCorner2.y,
									stOldSrchArea.coCorner1.x, stOldSrchArea.coCorner1.y,
									stOldSrchArea.coCorner2.x, stOldSrchArea.coCorner2.y);
					
					//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);	

					CString szLog;
					szLog.Format("%d,%d,%d,%d,%.2f,%d,%d,%d,%d,", 
						(int)stDieOffset.x, (int)stDieOffset.y, nOrigX, nOrigY, fDieScore,
						stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
						stSearchArea.coCorner2.x, stSearchArea.coCorner2.y);
					//CMSLogFileUtility::Instance()->BT_TableIndex2Log(szLog);	
				}
				else
				{
					//andrewng1234
					//BT XY encoder resolution chagned from 0.5 to 0.1 um
					lCollet2OffsetX = lCollet2OffsetX * 5;
					lCollet2OffsetY = lCollet2OffsetY * 5;

					stInfo.siStepX = stInfo.siStepX - lCollet2OffsetX;
					stInfo.siStepY = stInfo.siStepY - lCollet2OffsetY;

					szMsg.Format("BPR PREBOND LF result (at BH1 in motor steps) - Dir = %d (%ld, %ld, %.3f, %.3f), Orig(%d, %d); C2-Offset(%d, %d); Final(%d, %d); SrchWnd(%ld, %ld, %ld, %ld); oldWnd(%ld, %ld, %ld, %ld)",	
									lSortDir, m_lDiePitchX, m_lDiePitchY, dCalibX, dCalibY, nOrigX, nOrigY, 
									lCollet2OffsetX, lCollet2OffsetY, 
									stInfo.siStepX, stInfo.siStepY,
									stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
									stSearchArea.coCorner2.x, stSearchArea.coCorner2.y,
									stOldSrchArea.coCorner1.x, stOldSrchArea.coCorner1.y,
									stOldSrchArea.coCorner2.x, stOldSrchArea.coCorner2.y);

					//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);	
					
					CString szLog;
					szLog.Format("%d,%d,%d,%d,%.2f,%d,%d,%d,%d,", 
						(int)stDieOffset.x, (int)stDieOffset.y, nOrigX, nOrigY, fDieScore,
						stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
						stSearchArea.coCorner2.x, stSearchArea.coCorner2.y);
					//CMSLogFileUtility::Instance()->BT_TableIndex2Log(szLog);	
				}
			}
		}
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}


LONG CBondPr::ResetPrCenterXY(IPC_CServiceMessage& svMsg)
{
	m_lPrCenterX = PR_DEF_CENTRE_X;
	m_lPrCenterY = PR_DEF_CENTRE_Y;
	m_lPrCenterOffsetXInPixel	= 0;
	m_lPrCenterOffsetYInPixel	= 0;

	SavePrData();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::BT_SearchFirstDie(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;

	PR_COORD stOrgSrchCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stOrgSrchCorner2 = m_stSearchArea.coCorner2;

    stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;
    stInfo.iPrX = 0;
    stInfo.iPrY = 0;


	//Use Max search area to locate 1st die
	m_stSearchArea.coCorner1.x = PR_MAX_WIN_ULC_X;
	m_stSearchArea.coCorner1.y = PR_MAX_WIN_ULC_Y;
	m_stSearchArea.coCorner2.x = PR_MAX_WIN_LRC_X-1;
	m_stSearchArea.coCorner2.y = PR_MAX_WIN_LRC_Y-1;

	if (!IsPRLearnt(GetBondPRDieNo()))
	{
		stInfo.bResult = IDS_BPR_DIENOTLEARNT; //PR not learnt
		svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
		return 1;
	}

	//No inspection on here!
	if( ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore,
							m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 16) == -1 )
    {
        stInfo.bResult = FALSE;
    }

	if (DieIsAlignable(usDieType) == TRUE)
	{
	    CalculateDieCompenate(stDieOffset, &stInfo.siStepX, &stInfo.siStepY);
		//DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
        stInfo.bResult = TRUE;

        stInfo.iPrX = stDieOffset.x - m_lPrCenterX;
		stInfo.iPrY = stDieOffset.y - m_lPrCenterY;
	}

	//Restore current search area
	m_stSearchArea.coCorner1 = stOrgSrchCorner1;
	m_stSearchArea.coCorner2 = stOrgSrchCorner2;

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}


LONG CBondPr::BT2_SearchFirstDie(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;

	PR_COORD stOrgSrchCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stOrgSrchCorner2 = m_stSearchArea.coCorner2;

    stInfo.bResult = FALSE;
    stInfo.siStepX = 0;
    stInfo.siStepY = 0;
    stInfo.iPrX = 0;
    stInfo.iPrY = 0;

	if (!m_bDieCalibrated2)
	{
		CString szContent;
		szContent.LoadString(HMB_BPR_BOND_PR_NOT_SETUP_PROPERLY);
		//HmiMessage(szContent, "BT2_SearchFirstDie");
			
		SetAlert(IDS_BPR_CALIBRATEFACTORERROR);		//v4.53A21

		svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
		return 1;
	}

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}


LONG CBondPr::LearnDieCalibration(IPC_CServiceMessage& svMsg)
{
	LONG	lResult;	
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	BOOL	bReturn=TRUE;


	if (lInitPR != 1)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}
	if (m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE)
	{
		SetAlert(IDS_BPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}

	BOOL bNormalDie = TRUE;
	BOOL bPreBond = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];	
	if	(bPreBond && m_bSelectDieType)
		bNormalDie = FALSE;

	//Force to switch camera
	SelectBondCamera();
	SetBinTableJoystick(FALSE);

	lResult = FindDieCalibration(bNormalDie);	//v3.82
	switch(lResult)
	{
		case BPR_ERR_CALIB_NO_1ST_DIE:
			SetAlert(IDS_BPR_NODIEONCENTER);
			break;
		case BPR_ERR_CALIB_NO_LT_DIE:
			SetAlert(IDS_BPR_NODIEONLEFTSIDE);
			break;
		case BPR_ERR_CALIB_NO_RT_DIE:
			SetAlert(IDS_BPR_NODIEONRIGHTSIDE);
			break;
		case BPR_ERR_CALIB_NO_UP_DIE:
			SetAlert(IDS_BPR_NODIEONUPSIDE);
			break;
		case BPR_ERR_CALIB_NO_DN_DIE:
			SetAlert(IDS_BPR_NODIEONDOWNSIDE);
			break;
		case BPR_ERR_CALIB_ZERO_VALUE:
			SetAlert(IDS_BPR_CALIBRATEFACTORERROR);
			break;
		default:
			SetStatusMessage("Binblock die calibration is completed");
			CString szTitle, szContent;
			szTitle.LoadString(HMB_BPR_DIE_CALIBRATION);
			szContent.LoadString(HMB_BPR_CALIBRATION_OK);
			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			//SetAlert(IDS_BPR_DIECALIBRATIONDONE);
			SetbGenDieCalibration(TRUE);
			break;
	}

	if ( lResult != 0)
	{
		SetErrorMessage("Binblock die calibration is failed");
		m_bDieCalibrated = FALSE;	//v4.53A21
		SetbGenDieCalibration(FALSE);
	}

	UpdateBPRInfoList();

	//Force to switch camera
	SelectBondCamera();
	SetBinTableJoystick(TRUE);
	SavePrData();


	if( m_dFovSize<BPR_LF_SIZE )	//FOV updated in SavePrData()
	{
		CString szMsg;
		szMsg.Format("BPR FOV size %.2f less than %.2f, bin frame realignment affected!", m_dFovSize, BPR_LF_SIZE);
		HmiMessage(szMsg, "Bond PR");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::LearnDieCalibration2(IPC_CServiceMessage& svMsg)
{
	LONG	lResult;	
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	BOOL	bReturn=TRUE;


	if (!CMS896AApp::m_bMS100Plus9InchOption)	
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (lInitPR != 1)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}
	if (m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE)
	{
		SetAlert(IDS_BPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return -1;
	}

	LONG lBTInUse = (*m_psmfSRam)["BinTable"]["BTInUse"];
	if (lBTInUse != 1)
	{
		//HmiMessage_Red_Yellow("Error: BT2 not in use", "Learn BT2 Calibration");
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return -1;
	}

	BOOL bNormalDie = TRUE;
	BOOL bPreBond = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];	
	if	(bPreBond && m_bSelectDieType)
		bNormalDie = FALSE;


	//Force to switch camera
	SelectBondCamera();
	SetBinTableJoystick(FALSE);

	lResult = FindDieCalibration2(bNormalDie);	//v3.82
	switch(lResult)
	{
		case BPR_ERR_CALIB_NO_1ST_DIE:
			SetAlert(IDS_BPR_NODIEONCENTER);
			break;
		case BPR_ERR_CALIB_NO_LT_DIE:
			SetAlert(IDS_BPR_NODIEONLEFTSIDE);
			break;
		case BPR_ERR_CALIB_NO_RT_DIE:
			SetAlert(IDS_BPR_NODIEONRIGHTSIDE);
			break;
		case BPR_ERR_CALIB_NO_UP_DIE:
			SetAlert(IDS_BPR_NODIEONUPSIDE);
			break;
		case BPR_ERR_CALIB_NO_DN_DIE:
			SetAlert(IDS_BPR_NODIEONDOWNSIDE);
			break;
		case BPR_ERR_CALIB_ZERO_VALUE:
			SetAlert(IDS_BPR_CALIBRATEFACTORERROR);
			break;
		default:
			SetStatusMessage("Binblock die calibration 2 is completed");
			CString szTitle, szContent;
			szTitle.LoadString(HMB_BPR_DIE_CALIBRATION);
			szContent.LoadString(HMB_BPR_CALIBRATION_OK);
			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			//SetAlert(IDS_BPR_DIECALIBRATIONDONE);
			break;
	}

	if ( lResult != 0)
	{
		SetErrorMessage("Binblock die calibration 2 is failed");
		m_bDieCalibrated2 = FALSE;	//v4.53A21
	}


	//Force to switch camera
	SelectBondCamera();
	SetBinTableJoystick(TRUE);
	SavePrData();

	//if( m_dFovSize<BPR_LF_SIZE )	//FOV updated in SavePrData()
	//{
	//	CString szMsg;
	//	szMsg.Format("BPR FOV size %.2f less than %.2f, bin frame realignment affected!", m_dFovSize, BPR_LF_SIZE);
	//	HmiMessage(szMsg, "Bond PR");
	//}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::SelectRunTimeBinBlk(IPC_CServiceMessage& svMsg)
{
	LONG	lBinBlkNo;
	svMsg.GetMsg(sizeof(LONG), &lBinBlkNo);

	m_szPostbondDataX.Format("MapSorter RunTime>Bin%d+X Placement",lBinBlkNo);
	m_szPostbondDataY.Format("MapSorter RunTime>Bin%d+Y Placement",lBinBlkNo);
	m_szPostbondDataTheta.Format("MapSorter RunTime>Bin%d+Die Rotation",lBinBlkNo);

#ifdef NU_MOTION
	m_szPostbondDataX_Arm1.Format("MapSorter RunTime>Bin%d+X Placement Arm1",lBinBlkNo);
	m_szPostbondDataY_Arm1.Format("MapSorter RunTime>Bin%d+Y Placement Arm1",lBinBlkNo);
	m_szPostbondDataTheta_Arm1.Format("MapSorter RunTime>Bin%d+Die Rotation Arm1",lBinBlkNo);

	m_szPostbondDataX_Arm2.Format("MapSorter RunTime>Bin%d+X Placement Arm2",lBinBlkNo);
	m_szPostbondDataY_Arm2.Format("MapSorter RunTime>Bin%d+Y Placement Arm2",lBinBlkNo);
	m_szPostbondDataTheta_Arm2.Format("MapSorter RunTime>Bin%d+Die Rotation Arm2",lBinBlkNo);
#endif

	m_lCurrentBinBlock = lBinBlkNo;

	//Update HMI variable
	UpdateHMIVariable();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::SelectOffLineBinBlk(IPC_CServiceMessage& svMsg)
{
	LONG	ulBinBlkNo;

	svMsg.GetMsg(sizeof(ULONG), &ulBinBlkNo);

	m_szPostbondDataX.Format("MapSorter Offline>Bin%d+X Placement",ulBinBlkNo);
	m_szPostbondDataY.Format("MapSorter Offline>Bin%d+Y Placement",ulBinBlkNo);
	m_szPostbondDataTheta.Format("MapSorter Offline>Bin%d+Die Rotation",ulBinBlkNo);

	return 1;
}


LONG CBondPr::ManualCompensate(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
    	LONG lPixelX;
		LONG lPixelY;
	} BPR_MCLICK;

	BPR_MCLICK stInfo;
	svMsg.GetMsg(sizeof(stInfo), &stInfo);
	
	PR_COORD stDieOffset;
	stDieOffset.x = (PR_WORD)stInfo.lPixelX;
	stDieOffset.y = (PR_WORD)stInfo.lPixelY;

	ManualDieCompenate(stDieOffset);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::DeleteRuntimeData(IPC_CServiceMessage& svMsg)
{
	LONG	lBinBlkNo;
	BOOL	bReturn=TRUE;
	CString szText;

	svMsg.GetMsg(sizeof(LONG), &lBinBlkNo);

	//v4.59A19
	if ((lBinBlkNo <= 0) || (lBinBlkNo > 175))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

//	m_lPBIShiftCounter = 0;
//	m_lPBIAngleCounter = 0;


	if (lBinBlkNo == 0)		//v4.59A19	//For AutoClrBinCnt_SECSGEM()
	{
		for (INT i=1; i<=BPR_MAX_BINBLOCK; i++)
		{
			m_pBin[i-1]->Delete(100);
#ifdef NU_MOTION
			m_pBin_Arm1[i-1]->Delete(100);
			m_pBin_Arm2[i-1]->Delete(100);
#endif
			m_lOutAngleCounter[i-1]		= 0;	
			m_lOutAngleCounter2[i-1]	= 0;	
			m_lAccOutAngleCounter[i-1]	= 0;	
			m_lAccOutAngleCounter2[i-1]	= 0;	
			m_lOutDefectCounter[i-1]	= 0;	
			m_lOutEmptyCounter[i-1]		= 0;
			m_lOutEmptyCounter2[i-1]	= 0;	
			m_lOutAccEmptyCounter[i-1]	= 0;	
			m_lOutAccEmptyCounter2[i-1]	= 0;
			m_lOutAccEmptyCounterTotal[i-1]	= 0;
			m_lOutShiftCounter[i-1]		= 0;	
			m_lAccOutShiftCounter[i-1]	= 0;	
			m_lAccOutShiftCounter2[i-1]	= 0;	
			m_lOutRelShiftCounter[i-1]	= 0;	
			m_lTotalDieBonded[i-1]		= 0;	
			m_dOutAverageAngle[i-1]		= 0.0;	
			m_dTotalAngle[i-1]			= 0.0;	

			szText.Format("Binblock #%d SPC data is deleted", i);
			//SetStatusMessage(szText);

			//Delete Temp file in temp directory
			szText.Format(m_szPostbondDataSource + "\\Temp\\" + "%d", i);
			DeleteFile(szText);

#ifdef NU_MOTION
			szText.Format(m_szPostbondDataSource_Arm1 + "\\Temp\\" + "%d", i);
			DeleteFile(szText);
			szText.Format(m_szPostbondDataSource_Arm2 + "\\Temp\\" + "%d", i);
			DeleteFile(szText);
#endif
		}
	}
	else
	{
		m_pBin[lBinBlkNo-1]->Delete(100);
#ifdef NU_MOTION
		m_pBin_Arm1[lBinBlkNo-1]->Delete(100);
		m_pBin_Arm2[lBinBlkNo-1]->Delete(100);
#endif
	
		m_lOutAngleCounter[lBinBlkNo-1]		= 0;	
		m_lOutAngleCounter2[lBinBlkNo-1]	= 0;	
		m_lAccOutAngleCounter[lBinBlkNo-1]	= 0;	
		m_lAccOutAngleCounter2[lBinBlkNo-1]	= 0;	
		m_lOutDefectCounter[lBinBlkNo-1]	= 0;	
		m_lOutEmptyCounter[lBinBlkNo-1]		= 0;
		//m_lOutCamBlockEmptyCounter[lBinBlkNo-1]	= 0;
		m_lOutEmptyCounter2[lBinBlkNo-1]	= 0;	
		m_lOutAccEmptyCounter[lBinBlkNo-1]	= 0;	
		m_lOutAccEmptyCounter2[lBinBlkNo-1]	= 0;
		m_lOutAccEmptyCounterTotal[lBinBlkNo-1]	= 0;
		m_lOutShiftCounter[lBinBlkNo-1]		= 0;	
		m_lAccOutShiftCounter[lBinBlkNo-1]	= 0;	
		m_lAccOutShiftCounter2[lBinBlkNo-1]	= 0;	
		m_lOutRelShiftCounter[lBinBlkNo-1]	= 0;	
		m_lTotalDieBonded[lBinBlkNo-1]		= 0;	
		m_dOutAverageAngle[lBinBlkNo-1]		= 0.0;	
		m_dTotalAngle[lBinBlkNo-1]			= 0.0;	


		szText.Format("Binblock #%d SPC data is deleted", lBinBlkNo);
		SetStatusMessage(szText);
	
		//Delete Temp file in temp directory
		szText.Format(m_szPostbondDataSource + "\\Temp\\" + "%d", lBinBlkNo);
		DeleteFile(szText);
	
#ifdef NU_MOTION
		szText.Format(m_szPostbondDataSource_Arm1 + "\\Temp\\" + "%d", lBinBlkNo);
		DeleteFile(szText);
		szText.Format(m_szPostbondDataSource_Arm2 + "\\Temp\\" + "%d", lBinBlkNo);
		DeleteFile(szText);
#endif
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::DeleteRuntimeAllData(IPC_CServiceMessage& svMsg)
{
	short	ssBin = 0;
	BOOL	bReturn=TRUE;
	CString szText;
	
//	m_lPBIShiftCounter = 0;
//	m_lPBIAngleCounter = 0;

	if (IsBurnIn() == FALSE)
	{
		m_lOutCamBlockEmptyCounter = 0;

		for (ssBin=0; ssBin < BPR_MAX_BINBLOCK; ssBin++)
		{
			m_pBin[ssBin]->Delete(100);
#ifdef NU_MOTION
			m_pBin_Arm1[ssBin]->Delete(100);
			m_pBin_Arm2[ssBin]->Delete(100);
#endif

			m_lOutAngleCounter[ssBin]		= 0;	
			m_lOutAngleCounter2[ssBin]		= 0;	
			m_lAccOutAngleCounter[ssBin]	= 0;
			m_lAccOutAngleCounter2[ssBin]	= 0;
			m_lOutDefectCounter[ssBin]		= 0;	
			m_lOutEmptyCounter[ssBin]		= 0;	
			m_lOutEmptyCounter2[ssBin]		= 0;	
			m_lOutAccEmptyCounter[ssBin]	= 0;	
			m_lOutAccEmptyCounter2[ssBin]	= 0;	
			m_lOutAccEmptyCounterTotal[ssBin]	= 0;
			//m_lOutCamBlockEmptyCounter[ssBin]= 0;
			m_lOutShiftCounter[ssBin]		= 0;	
			m_lAccOutShiftCounter[ssBin]	= 0;	
			m_lAccOutShiftCounter2[ssBin]	= 0;	
			m_lOutRelShiftCounter[ssBin]	= 0;	
			m_lTotalDieBonded[ssBin]		= 0;	
			m_dOutAverageAngle[ssBin]		= 0.0;	
			m_dTotalAngle[ssBin]			= 0.0;	

			//Delete Temp file in temp directory
			szText.Format(m_szPostbondDataSource + "\\Temp\\" + "%d", ssBin+1);
			DeleteFile(szText);
			
#ifdef NU_MOTION
			szText.Format(m_szPostbondDataSource_Arm1 + "\\Temp\\" + "%d", ssBin+1);
			DeleteFile(szText);
			szText.Format(m_szPostbondDataSource_Arm2 + "\\Temp\\" + "%d", ssBin+1);
			DeleteFile(szText);
#endif
		}

		SetStatusMessage("All binblock SPC data are deleted");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::DisplaySPCMessage(IPC_CServiceMessage& svMsg)
{
	CString szText;

	typedef struct 
	{
    	BOOL bAllFile;
		LONG lBinBlkNo;
	} BPR_SPCMESS;

	BPR_SPCMESS stInfo;

	svMsg.GetMsg(sizeof(BPR_SPCMESS), &stInfo);
	
	if (stInfo.bAllFile == TRUE)
	{
		szText.Format("All binblock SPC data are deleted");
	}
	else
	{
		szText.Format("Binblock #%d SPC data is deleted", stInfo.lBinBlkNo);
	}

	HmiMessage(szText, "PostBond Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::CopyAllSPCValue(IPC_CServiceMessage& svMsg)
{
	LONG i = 0;	
	LONG lBinBlkNo;
	BOOL bReturn=TRUE;
	CString szText;
	CString szTitle, szContent;
	LONG lCurrentBlkTemp;

	svMsg.GetMsg(sizeof(LONG), &lBinBlkNo);

	szTitle.LoadString(HMB_BPR_POSTBOND);

	if (lBinBlkNo == 1)
	{
		for (i=1; i<BPR_MAX_BINBLOCK; i++)
		{
			if (!IsNGBlock((UCHAR)(i + 1))) //Do not copy all to NG Block
			{
				m_lGenMaxAngleAllow[i]	= m_lGenMaxAngleAllow[0];
				m_lGenMaxAccAngleAllow[i]	= m_lGenMaxAccAngleAllow[0];	//v4.58A5
				m_dGenMaxAngle[i]		= m_dGenMaxAngle[0];
				m_dGenAverageAngle[i]	= m_dGenAverageAngle[0];
				m_lGenMaxShiftAllow[i]	= m_lGenMaxShiftAllow[0];
				m_lGenRelDieShiftAllow[i]	= m_lGenRelDieShiftAllow[0];
				m_dGenDieShiftX[i]		= m_dGenDieShiftX[0];
				m_dGenDieShiftY[i]		= m_dGenDieShiftY[0];
				m_lGenMaxDefectAllow[i]	= m_lGenMaxDefectAllow[0];
				m_lGenMaxEmptyAllow[i]	= m_lGenMaxEmptyAllow[0];			//v3.02T1
				m_lGenMaxAccEmptyAllow[i]	= m_lGenMaxAccEmptyAllow[0];	//v4.40T9
			}
		}

		lCurrentBlkTemp = m_lCurrentBinBlock;
		m_lCurrentBinBlock = 0;
		LogItems(BPR_PLACEMENT_CHECK);
		LogItems(BPR_PLACEMENT_MAX_SHIFT_X);
		LogItems(BPR_PLACEMENT_MAX_SHIFT_Y);
		LogItems(BPR_PLACEMENT_MAX_ALLOWED);
		LogItems(BPR_DIE_ROTATION_AVG_ANGLE);
		LogItems(BPR_DIE_ROTATION_AVG_ANGLE_ALLOW);
		LogItems(BPR_DIE_ROTATION_CHECK_MAX_ANGLE);
		LogItems(BPR_DIE_ROTATION_MAX_ANGLE_DIE_ALLOW);
		LogItems(BPR_DIE_EMPTY_DIE_CHECK);
		LogItems(BPR_DIE_EMPTY_DIE_CHECK_MAX_ALLOWED);
		LogItems(BPR_DIE_ROTATION_MAX_ANGLE);

		m_lCurrentBinBlock = lCurrentBlkTemp;

		szContent.LoadString(HMB_BPR_COPY_OK);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		SavePrData();
	}
	else
	{
		szContent.LoadString(HMB_BPR_ONLY_COPY_BLK1);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::DeleteOfflineData(IPC_CServiceMessage& svMsg)
{
	//LONG	lBinBlkNo;

	//svMsg.GetMsg(sizeof(LONG), &lBinBlkNo);

	//SPC_DeletePart(m_nOffBin[lBinBlkNo-1], 100);

	return 1;
}


LONG CBondPr::DeleteOfflineAllData(IPC_CServiceMessage& svMsg)
{
	//short	ssBin = 0;

	//for (ssBin=0; ssBin < BPR_MAX_BINBLOCK; ssBin++)
	//{
	//	SPC_DeletePart(m_nOffBin[ssBin], 100);
	//}

	return 1;
}


LONG CBondPr::StartOfflinePostbond(IPC_CServiceMessage& svMsg)
{
	//typedef struct {
	//	ULONG	ulBinBlkNo;
	//	LONG	lTotalCount;	
	//} TYPE_INFO;

	//TYPE_INFO stInfo;
	//svMsg.GetMsg(sizeof(TYPE_INFO), &stInfo);


	//m_lCurrentBinBlock	= (LONG)(stInfo.ulBinBlkNo);
	//m_lOfflineCount		= stInfo.lTotalCount;
	//m_bStartOffline		= TRUE;

	////return reply
	//BOOL	bStatus = TRUE;
	//svMsg.InitMessage(sizeof(BOOL), &bStatus);

	return 1;
}


// --- Use Mouse ---
LONG CBondPr::UsePRMouse(IPC_CServiceMessage& svMsg)
{
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetBinTableJoystick(FALSE);
	}

	PR_SET_MOUSE_CTRL_OBJ_CMD		stSetCmd;
   	PR_SET_MOUSE_CTRL_OBJ_RPY		stSetRpy;

	if ( m_bUseMouse == FALSE )
	{
		// Erase the original region
		if ( GetDieShape() == BPR_RECTANGLE_DIE )
		{
			DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT],  PR_UPPER_LEFT,  0);
			DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 0);
		}
		else if ( GetDieShape() == BPR_TRIANGULAR_DIE )	//v4.06
		{
			DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 0);
			DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[0], 0);
		}
		else if ( GetDieShape() == BPR_RHOMBUS_DIE )
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
		m_pPrGeneral->MouseDefineRegion();

		// Set the mouse size first
		PR_InitSetMouseCtrlObjCmd(&stSetCmd);
		stSetCmd.ulObjectId = 1;

		if ( GetDieShape() == BPR_RECTANGLE_DIE )
		{
			stSetCmd.emShape = PR_SHAPE_TYPE_RECTANGLE;
			stSetCmd.uwNumOfCorners = 2;
			stSetCmd.acoObjCorner[0] = m_stLearnDieCornerPos[PR_UPPER_LEFT];
			stSetCmd.acoObjCorner[1] = m_stLearnDieCornerPos[PR_LOWER_RIGHT];
		}
		else if ( GetDieShape() == BPR_TRIANGULAR_DIE )	//v4.06
		{
			stSetCmd.emShape = PR_SHAPE_TYPE_POLYGON;
			stSetCmd.uwNumOfCorners = BPR_TRIANGLE_CORNERS;
			stSetCmd.acoObjCorner[0] = m_stLearnDieCornerPos[BPR_DIE_CORNER_0];
			stSetCmd.acoObjCorner[1] = m_stLearnDieCornerPos[BPR_DIE_CORNER_1];
			stSetCmd.acoObjCorner[2] = m_stLearnDieCornerPos[BPR_DIE_CORNER_2];
		}
		else if ( GetDieShape() == BPR_RHOMBUS_DIE )
		{
			stSetCmd.emShape = PR_SHAPE_TYPE_POLYGON;
			stSetCmd.uwNumOfCorners = BPR_RHOMBUS_CORNERS;
			stSetCmd.acoObjCorner[0] = m_stLearnDieCornerPos[BPR_DIE_CORNER_0];
			stSetCmd.acoObjCorner[1] = m_stLearnDieCornerPos[BPR_DIE_CORNER_1];
			stSetCmd.acoObjCorner[2] = m_stLearnDieCornerPos[BPR_DIE_CORNER_2];
			stSetCmd.acoObjCorner[3] = m_stLearnDieCornerPos[BPR_DIE_CORNER_3];
		}
		else
		{
			stSetCmd.emShape = PR_SHAPE_TYPE_POLYGON;
			stSetCmd.uwNumOfCorners = BPR_HEXAGON_CORNERS;
			stSetCmd.acoObjCorner[0] = m_stLearnDieCornerPos[BPR_DIE_CORNER_0];
			stSetCmd.acoObjCorner[1] = m_stLearnDieCornerPos[BPR_DIE_CORNER_1];
			stSetCmd.acoObjCorner[2] = m_stLearnDieCornerPos[BPR_DIE_CORNER_2];
			stSetCmd.acoObjCorner[3] = m_stLearnDieCornerPos[BPR_DIE_CORNER_3];
			stSetCmd.acoObjCorner[4] = m_stLearnDieCornerPos[BPR_DIE_CORNER_4];
			stSetCmd.acoObjCorner[5] = m_stLearnDieCornerPos[BPR_DIE_CORNER_5];
		}

		stSetCmd.emColor = PR_COLOR_RED;
		PR_SetMouseCtrlObjCmd(&stSetCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stSetRpy);

		m_bUseMouse = TRUE;
	}
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::DrawComplete(IPC_CServiceMessage& svMsg)
{
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetBinTableJoystick(TRUE);
	}

	if ( m_bUseMouse == TRUE )
	{
		PR_COORD acoObjCorner[PR_MAX_NO_OF_SHAPE_CORNER];
		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetAllCorners(acoObjCorner);
		if( uwNumOfCorners == 2 )
		{
			m_stLearnDieCornerPos[PR_UPPER_LEFT]	= acoObjCorner[0];
			m_stLearnDieCornerPos[PR_LOWER_RIGHT]	= acoObjCorner[1];
			CString szTemp;
			szTemp.Format(" (%ld, %ld, %ld, %ld)",
				m_stLearnDieCornerPos[PR_UPPER_LEFT].x,		m_stLearnDieCornerPos[PR_UPPER_LEFT].y,
				m_stLearnDieCornerPos[PR_LOWER_RIGHT].x,	m_stLearnDieCornerPos[PR_LOWER_RIGHT].y);
			SetErrorMessage("BPR die size Confirm Mouse Draw die corner." + szTemp);	//xxxxxx
		}
		else if (uwNumOfCorners == BPR_TRIANGLE_CORNERS)	//v4.06		//Triangular die
		{
			m_stLearnDieCornerPos[BPR_DIE_CORNER_0]	= acoObjCorner[0];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_1] = acoObjCorner[1];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_2] = acoObjCorner[2];
		}
		else if (uwNumOfCorners == BPR_RHOMBUS_CORNERS )
		{
			m_stLearnDieCornerPos[BPR_DIE_CORNER_0]	= acoObjCorner[0];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_1] = acoObjCorner[1];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_2] = acoObjCorner[2];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_3] = acoObjCorner[3];
		}
		else if (uwNumOfCorners == BPR_HEXAGON_CORNERS )
		{
			m_stLearnDieCornerPos[BPR_DIE_CORNER_0]	= acoObjCorner[0];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_1] = acoObjCorner[1];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_2] = acoObjCorner[2];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_3] = acoObjCorner[3];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_4] = acoObjCorner[4];
			m_stLearnDieCornerPos[BPR_DIE_CORNER_5] = acoObjCorner[5];
		}

		// Draw the defined region
		if (uwNumOfCorners == 2)
		{
			DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT], PR_UPPER_LEFT, 1);
			DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 1);
		}
		else if (uwNumOfCorners == BPR_TRIANGLE_CORNERS)	//v4.06		//Triangular die
		{
			DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[0], 1);
		}
		else if (uwNumOfCorners == BPR_RHOMBUS_CORNERS)
		{
			DrawAndEraseLine(m_stLearnDieCornerPos[0], m_stLearnDieCornerPos[1], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[1], m_stLearnDieCornerPos[2], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[2], m_stLearnDieCornerPos[3], 1);
			DrawAndEraseLine(m_stLearnDieCornerPos[3], m_stLearnDieCornerPos[0], 1);
		}
		else if (uwNumOfCorners == BPR_HEXAGON_CORNERS)
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
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::CancelMouseDraw(IPC_CServiceMessage& svMsg)
{
	if ( m_bUseMouse == TRUE )
	{
		// Erase the mouse object
		m_pPrGeneral->EraseMouseCtrlObj();

		m_bUseMouse = FALSE;
	}
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


// --- Upload and Download WPR Records ---
LONG CBondPr::UploadPrRecords(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	CString szPath;
	CString szLog;
	CHAR acFilename[PR_MAX_FILE_PATH_LENGTH];

	if (m_fHardware == TRUE)
	{
		szPath = (*m_psmfSRam)["PR"]["RecordPath"];
		for (INT i=0; i < BPR_MAX_DIE; i++)
		{
			// Upload the normal die 
			if (m_ssGenPRSrchID[i] == 0)
				continue;

			sprintf_s((char*)acFilename, sizeof(acFilename), "%s\\BPR%d.rec", (LPCTSTR) szPath, i);		//Klocwork
			CString szErrMsg;
			PR_UWORD uwReturn = m_pPrGeneral->UploadPRRecord(m_ssGenPRSrchID[i], acFilename, GetPostBondPRSenderID(), GetPostBondPRReceiverID(), szErrMsg);

			if (uwReturn == PR_ERR_NO_PIDREC )		// Given record ID not exist
			{
				m_ssGenPRSrchID[i] = 0;	// Clear the invalid record ID
				(*m_psmfSRam)["BPR"]["RecordExist"] = FALSE;
				bReturn = TRUE;
			} 
			else
			{
				if (uwReturn != PR_ERR_NOERR)
				{
					DisplayMessage(szErrMsg);	
					bReturn = FALSE;
				}
				(*m_psmfSRam)["BPR"]["RecordExist"] = TRUE;
			}

			//v4.53A22
			szLog.Format("BPR: Upload Record #%d, Channel=0x%x, RecordID=%d, Path=%s", 
						i, GetPostBondPRSenderID(), m_ssGenPRSrchID[i], acFilename);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}

		//Upload Epoxy Inspection ID
		if (m_ssPSPRSrchID[0] > 0)
		{
			sprintf_s((char*)acFilename, sizeof(acFilename), "%s\\PEI0.rec", (LPCTSTR) szPath);		//Klocwork
			CString szErrMsg;
			PR_UWORD uwReturn = m_pPrGeneral->UploadPRRecord(m_ssPSPRSrchID[0], acFilename, GetPostBondPRSenderID(), GetPostBondPRReceiverID(), szErrMsg);

			if (uwReturn == PR_ERR_NO_PIDREC )		// Given record ID not exist
			{
				m_ssPSPRSrchID[0] = 0;	// Clear the invalid record ID
			} 
			else
			{
				if (uwReturn != PR_ERR_NOERR)
				{
					DisplayMessage(szErrMsg);	
				}
				else
				{
					(*m_psmfSRam)["BPR"]["RecordExist"] = TRUE;
				}
			}
		}
	}
	else
	{
		(*m_psmfSRam)["BPR"]["RecordExist"] = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::DownloadPrRecords(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	LONG i=0;
	CHAR acTemp[PR_MAX_FILE_NAME_LENGTH];
	CFile cfFile;
	CString szErrMsg, szPath;

	if( IsAOIOnlyMachine() )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szLog = _T("");
	CString szIDLog = _T("BPR: DownloadPrRecords - ");
	CString szIDValue;

	BOOL bHmiDL = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bHmiDL);

	if ( m_fHardware )
	{
		szPath = (*m_psmfSRam)["PR"]["RecordPath"];
		//FreeAllPrRecords();	

		for (i=0; i<BPR_MAX_DIE; i++)	//v3.06
		{
			sprintf_s(acTemp, sizeof(acTemp), "%s\\BPR%ld.rec", (LPCTSTR) szPath, i);	//Klocwork

			// Check whether the file is existing
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
				
			PR_ULWORD ulRetRecordID = m_pPrGeneral->DownloadPRRecord(0, acTemp, GetPostBondPRSenderID(), GetPostBondPRReceiverID(), szErrMsg);

			if (ulRetRecordID == 0)
			{
				HmiMessage(szErrMsg);
				bReturn = FALSE;
				break;
			}
		
			m_ssGenPRSrchID[i] = (PR_WORD)ulRetRecordID;
			m_bGenDieLearnt[i] = TRUE;
			if (i == 0 && m_bGenDieLearnt[i] == TRUE)
			{
				m_bDieLearnt = TRUE;
			}

			//v4.53A22
			szLog.Format("BPR: Download Record #%d, Channel=0x%x, RetRecordID=%d", 
						i, GetPostBondPRSenderID(), ulRetRecordID);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			//v4.53A22
			szIDValue.Format("%ld(ID=%d) ", i, m_ssGenPRSrchID[i]);
			szIDLog = szIDLog + szIDValue;
		}

		CMSLogFileUtility::Instance()->MS_LogOperation(szIDLog);		//v4.53A22
		//Prebond Epoxy Inspection
		sprintf_s(acTemp, sizeof(acTemp), "%s\\PEI0.rec", (LPCTSTR) szPath);

		// Check whether the file is existing
		BOOL bFileExist = FALSE;
		if (cfFile.Open(acTemp, CFile::modeRead) == TRUE)
		{
			if (cfFile.GetLength() > 0)
			{
				bFileExist = TRUE;
			}
			cfFile.Close();
		}		
				
		if (bFileExist)
		{
			PR_ULWORD ulRetRecordID = m_pPrGeneral->DownloadPRRecord(0, acTemp, GetPostBondPRSenderID(), GetPostBondPRReceiverID(), szErrMsg);

			if (ulRetRecordID > 0)
			{
				m_ssPSPRSrchID[0] = (PR_WORD)ulRetRecordID;
			}
			else
			{
				HmiMessage(szErrMsg);
				bReturn = FALSE;
			}
		}
	}

	//	not load and not save, just down load PR records, as the status is in PKG file msd
	if( bHmiDL )
	{
		SavePrData(FALSE);
	}
	UpdateBPRInfoList();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);		//v4.52A10
	return 1;
}

LONG CBondPr::DeletePrRecords(IPC_CServiceMessage& svMsg)
{
	CString szTemp, szPath;

	szPath = (*m_psmfSRam)["PR"]["RecordPath"];

	
	for (INT i=0; i <= BPR_MAX_DIE; i++ )
	{
		// Delete the uploaded records
		szTemp.Format("%s\\BPR%d.rec", szPath,i);
		DeleteFile(szTemp);
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::DeletePRRecord(IPC_CServiceMessage& svMsg)
{
	BOOL		bReturn = TRUE;
	LONG		lPrID;
	int			i;
	CString		csMsgA;
	CString		szTitle;

	svMsg.GetMsg(sizeof(LONG), &lPrID);

	szTitle.LoadString(HMB_BPR_REMOVE_PR_RECORD);
	//szTitle = "BPR Remove Record";

	csMsgA.LoadString(HMB_GENERAL_AREUSURE);

	if (lPrID == 0)
	{
		if (HmiMessage(csMsgA, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		m_pPrGeneral->FreeAllPRRecord(GetPostBondPRSenderID(), GetPostBondPRReceiverID());
		csMsgA.LoadString(HMB_BPR_ALL_PRID_REMOVED);
		//clear variables
		
		for (i=0; i<BPR_MAX_DIE; i++)
		{
			m_bGenDieLearnt[i] = FALSE;
			m_ssGenPRSrchID[i] = 0;
			m_szGenDieZoomMode[i] = "";
			m_bGenDieCalibration[i] = FALSE;
		}
		m_ssPSPRSrchID[0]	= 0;
		m_ssPSPRSrchID[1]	= 0;
		SavePrData();

		//SetAlert(IDS_WPR_ALLPRIDCLR);
		HmiMessage(csMsgA, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}
	else
	{
		for (i=0; i<BPR_MAX_DIE; i++)
		{
			if (m_ssGenPRSrchID[i] == lPrID)
			{
				if (HmiMessage(csMsgA, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
				{
					bReturn = FALSE;
					UpdateBPRInfoList();
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}

				if ( lPrID < BPR_MAX_DIE )
				{
					m_pPrGeneral->FreePRRecord(lPrID, GetPostBondPRSenderID(), GetPostBondPRReceiverID());
					csMsgA.LoadString(HMB_BPR_ONE_PRID_REMOVED);
					//csMsgA = "BPR Record Removed";

					//clear variables
					m_szGenDieZoomMode[i] = "";
					m_bGenDieCalibration[i] = FALSE;
					m_bGenDieLearnt[i] = FALSE;
					m_ssGenPRSrchID[i] = 0;
					SavePrData();

					HmiMessage(csMsgA, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
					break;
				}
			}
		}
	}
	
	UpdateBPRInfoList();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CBondPr::DisplayPRRecord(IPC_CServiceMessage &svMsg)
{
	PR_COORD				stCorner1;
	PR_COORD				stCorner2;
//	PR_COORD				stTxtCoord;
	LONG					lPrID;

	LONG lDieNo = GetDieNo(m_bSelectDieType, m_lCurRefDieNo);
	lPrID = m_ssGenPRSrchID[lDieNo];

	PR_UWORD uwPRStatus = m_pPrGeneral->DisplayLearnInfo(lPrID, GetPrCenterX(), GetPrCenterY(), GetPostBondPRSenderID(), GetPostBondPRReceiverID());
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
	SelectBondCamera();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CBondPr::PkgKeyParametersTask(IPC_CServiceMessage& svMsg)
{
	CStdioFile kFile;
	BOOL bReturn = TRUE;
	LONG lAction = 0;
	svMsg.GetMsg(sizeof(LONG), &lAction);

	BOOL	bBinTableAngleCheck	=	m_bBinTableAngleCheck;
	BOOL	bPlacementCheck		=	m_bPlacementCheck;
	BOOL	bEmptyCheck			=	m_bEmptyCheck;
	DOUBLE	dMaxAngle			=	m_dMaxAngle;
	DOUBLE	dDieShiftX			=	m_dDieShiftX;
	DOUBLE	dDieShiftY			=	m_dDieShiftY;
	LONG	lMaxAngleAllow		=	m_lMaxAngleAllow;
	LONG	lMaxShiftAllow		=	m_lMaxShiftAllow;
	LONG	lMaxEmptyAllow		=	m_lMaxEmptyAllow;
	LONG	lMaxAccEmptyAllow	=	m_lMaxAccEmptyAllow;

	if( lAction==0 || lAction==1 )
	{
		if( lAction==1 )
		{
			CStringMapFile psmf;
			CString szNewMsdName = "C:\\MapSorter\\UserData\\BondPr.msd";
			if ( psmf.Open(szNewMsdName, FALSE, FALSE) )
			{
				bBinTableAngleCheck	=	(BOOL)((LONG)(psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ANGLE_CHECK]);
				bPlacementCheck		=	(BOOL)((LONG)(psmf)[BPR_POSTBOND_DATA][BPR_PB_PLACEMENT_CHECK]);
				bEmptyCheck			=	(BOOL)((LONG)(psmf)[BPR_POSTBOND_DATA][BPR_PB_EMPTY_CHECK]);
				dMaxAngle			=	(psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ANGLE][1];
				dDieShiftX			=	(psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_X_SHIFT][1];
				dDieShiftY			=	(psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_Y_SHIFT][1];
				lMaxAngleAllow		=	(psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ANGLE_ALLOW][1];
				lMaxShiftAllow		=	(psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_SHIFT_ALLOW][1];
				lMaxEmptyAllow		=	(psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_EMPTY_ALLOW][1];
				lMaxAccEmptyAllow	=	(psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ACC_EMPTY_ALLOW][1];
			}
			psmf.Close();
			DeleteFile(szNewMsdName);
		}

		if( kFile.Open(gszLocalPkgCheckListFile, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText) )
		{
			kFile.SeekToEnd();

			CString szText;
			szText.Format("%s",		"BPR list begin");		kFile.WriteString(szText + "\n");
			szText.Format("%d",		bBinTableAngleCheck);	kFile.WriteString(szText + "\n");
			szText.Format("%d",		bPlacementCheck);		kFile.WriteString(szText + "\n");
			szText.Format("%d",		bEmptyCheck);			kFile.WriteString(szText + "\n");
			szText.Format("%f",		dMaxAngle);				kFile.WriteString(szText + "\n");
			szText.Format("%f",		dDieShiftX);			kFile.WriteString(szText + "\n");
			szText.Format("%f",		dDieShiftY);			kFile.WriteString(szText + "\n");
			szText.Format("%ld",	lMaxAngleAllow);		kFile.WriteString(szText + "\n");
			szText.Format("%ld",	lMaxShiftAllow);		kFile.WriteString(szText + "\n");
			szText.Format("%ld",	lMaxEmptyAllow);		kFile.WriteString(szText + "\n");
			szText.Format("%ld",	lMaxAccEmptyAllow);		kFile.WriteString(szText + "\n");
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
				if( szText.Find("BPR list begin")!=-1 )
				{
					break;
				}
			}

			if( kFile.ReadString(szText) )
			{
				bBinTableAngleCheck = atoi(szText);
			}
			if( kFile.ReadString(szText) )
			{
				bPlacementCheck = atoi(szText);
			}
			if( kFile.ReadString(szText) )
			{
				bEmptyCheck = atoi(szText);
			}
			if( kFile.ReadString(szText) )
			{
				dMaxAngle = atof(szText);
			}
			if( kFile.ReadString(szText) )
			{
				dDieShiftX = atof(szText);
			}
			if( kFile.ReadString(szText) )
			{
				dDieShiftY = atof(szText);
			}
			if( kFile.ReadString(szText) )
			{
				lMaxAngleAllow = atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				lMaxShiftAllow = atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				lMaxEmptyAllow = atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				lMaxAccEmptyAllow = atol(szText);
			}
			kFile.Close();

			if( bBinTableAngleCheck != m_bBinTableAngleCheck )
			{
				szText.Format("Angle Check changed to %d(%d)", bBinTableAngleCheck, m_bBinTableAngleCheck);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( bPlacementCheck != m_bPlacementCheck )
			{
				szText.Format("Placement Check changed to %d(%d)", bPlacementCheck, m_bPlacementCheck);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( bEmptyCheck != m_bEmptyCheck )
			{
				szText.Format("Empty Check changed to %d(%d)", bEmptyCheck, m_bEmptyCheck);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( dMaxAngle != m_dMaxAngle )
			{
				szText.Format("Max Angle changed to %f(%f)", dMaxAngle, m_dMaxAngle);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( dDieShiftX != m_dDieShiftX )
			{
				szText.Format("Shift X changed to %f(%f)", dDieShiftX, m_dDieShiftX);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( dDieShiftY != m_dDieShiftY )
			{
				szText.Format("Shift Y changed to %f(%f)", dDieShiftY, m_dDieShiftY);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( lMaxAngleAllow != m_lMaxAngleAllow )
			{
				szText.Format("Max Angle Allow changed to %ld(%ld)", lMaxAngleAllow, m_lMaxAngleAllow);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( lMaxShiftAllow != m_lMaxShiftAllow )
			{
				szText.Format("Max Shift Allow changed to %ld(%ld)", lMaxShiftAllow, m_lMaxShiftAllow);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( lMaxEmptyAllow != m_lMaxEmptyAllow )
			{
				szText.Format("Max Empty Allow changed to %ld(%ld)", lMaxEmptyAllow, m_lMaxEmptyAllow);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( lMaxAccEmptyAllow != m_lMaxAccEmptyAllow )
			{
				szText.Format("Max Acc Empty allow changed to %ld(%ld)", lMaxAccEmptyAllow, m_lMaxAccEmptyAllow);
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
			HmiMessage(szListMsg, "BPR Parameter Check");
			SetAlarmLamp_Yellow(FALSE, TRUE);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::CheckPostBondSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szTitle, szContent;

	//SetErrorMessage("CheckPostBondSetup Start");		//v4.30T4

	if ( CMS896AStn::m_bUsePostBond == FALSE || IsAOIOnlyMachine() )	//v4.37T11
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bUsePostBond == TRUE)
	{
		SetErrorMessage("CheckPostBondSetup Use PostBond Check");

		if ((m_ssGenPRSrchID[BPR_NORMAL_DIE] == 0) || (m_bDieCalibrated == FALSE) || 
			(m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE))
		{
			SetErrorMessage("CheckPostBondSetup fails #1: no PR record");
			szTitle.LoadString(HMB_BPR_CK_POST_BOND_SETUP);
			szContent.LoadString(HMB_BPR_BOND_PR_NOT_SETUP_PROPERLY);
			HmiMessage(szContent, szTitle);
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else
		{
			//v4.44A6	//Semitek
			if ( (m_stGenDieSize[BPR_NORMAL_DIE].x == 0) || 
				 (m_stGenDieSize[BPR_NORMAL_DIE].y == 0) )
			{
				SetErrorMessage("CheckPostBondSetup fails #2: invalid die size");
				szTitle.LoadString(HMB_BPR_CK_POST_BOND_SETUP);
				szContent.LoadString(HMB_BPR_BOND_PR_NOT_SETUP_PROPERLY);
				HmiMessage(szContent, szTitle);
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}

/*
		//v4.35T4
		if ( (CMS896AApp::m_bMS100Plus9InchOption == TRUE) && (m_bDieCalibrated2 == FALSE) )
		{
			SetErrorMessage("CheckPostBondSetup fails BT2 calibration");
			szTitle.LoadString(HMB_BPR_CK_POST_BOND_SETUP);
			szContent.LoadString(HMB_BPR_BOND_PR_NOT_SETUP_PROPERLY);
			HmiMessage(szContent, szTitle);
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
*/
		//v4.51A2
		if (CheckElectech3EDLPostBondChecking() != TRUE)
		{
			//m_bShowAlarmPage = TRUE;
			SetErrorMessage("BPR (PreStart)(3EDL): Postbond hit XY Limit Count");
			SetAlert_Red_Yellow(IDS_BH_SPC_XY_LIMIT);		// Set Alarm

			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

	}
	else if (CMS896AStn::m_bEnableEmptyBinFrameCheck == TRUE)	// bin frame check, must have bond pr record
	{
		if ((m_ssGenPRSrchID[BPR_NORMAL_DIE] == 0) || (m_bDieCalibrated == FALSE) || 
			(m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE) || (m_bUsePostBond == FALSE))
		{
			SetErrorMessage("CheckPostBondSetup fails #2");		//v4.15T9
			szTitle.LoadString(HMB_BPR_CK_POST_BOND_SETUP);
			szContent.LoadString(HMB_BPR_BOND_PR_NOT_SETUP_PROPERLY);
			HmiMessage(szContent, szTitle);
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	//SetErrorMessage("CheckPostBondSetup Complete");

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::UpdateAction(IPC_CServiceMessage& svMsg)
{
	Sleep(500);
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::LogItems(IPC_CServiceMessage& svMsg)
{
	LONG lEventNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lEventNo);

	LogItems(lEventNo);
	
	return 1;
}

LONG CBondPr::UpdateHMIData(IPC_CServiceMessage& svMsg)
{
	BOOL	bStatus = TRUE;

	typedef struct {
		BOOL bDieType;
		LONG lRefDieNo;
	} REF_TYPE;

	REF_TYPE stInfo;
	svMsg.GetMsg(sizeof(REF_TYPE), &stInfo);

	m_bSelectDieType	= stInfo.bDieType;
	m_lCurRefDieNo		= stInfo.lRefDieNo;
	UpdateHMIVisionVariable();

	BOOL bPreBond = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];	//v4.5920
	if (m_bSelectDieType)		//if REF die
	{
		//v4.53A21
		if ( (CMS896AStn::m_bAutoChangeCollet) )	//&& (m_lCurRefDieNo == 5) )
		{
			//Do not hardcode ALIGN method becuase collet-search usually uses Edge-Matching
		}
		else if (bPreBond)
		{
			//PreBOnd alignment may use EDGE matching to align pad	//v4.59A20
		}
		else
		{
			m_lLrnAlignAlgo	= 1;	//pattern matching
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bStatus);
	return 1;
}

LONG CBondPr::CheckRecordID(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	LONG lPrID;

	svMsg.GetMsg(sizeof(LONG), &lPrID);

	if ( (lPrID == 0) || (lPrID == (LONG)m_ssGenPRSrchID[0]) )
	{
		m_ssGenPRSrchID[0]		= 0;
		m_stGenDieSize[0].x		= 0;
		m_stGenDieSize[0].y		= 0;
		m_bUsePostBond		= FALSE;
		m_bGenDieLearnt[0]	= FALSE;
		m_bDieCalibrated	= FALSE;	
		m_lCurNormDieID		= 0;
		m_lCurNormDieSizeX	= 0; 
		m_lCurNormDieSizeY	= 0;
		m_dCurNormDieSizeX	= 0; 
		m_dCurNormDieSizeY	= 0;

		//if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.35T4	//MS109	
		//{
		m_bDieCalibrated2	= FALSE;	
		//}

		SavePrData();
	}


	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::SaveSPCData(IPC_CServiceMessage& svMsg)		
{
	ULONG	ulBinIndex = 0;	
	BOOL	bReturn = FALSE;


	svMsg.GetMsg(sizeof(ULONG), &ulBinIndex);

	if ( (ulBinIndex < 1) || (ulBinIndex > BPR_MAX_BINBLOCK) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Temp directory for temp SPC data
	CreateDirectory(m_szPostbondDataSource + "\\Temp", NULL);
	CreateDirectory(m_szPostbondDataSource_Arm1 + "\\Temp", NULL);
	CreateDirectory(m_szPostbondDataSource_Arm2 + "\\Temp", NULL);

#ifdef NU_MOTION
	if (ExportPostBondData_Arm1(ulBinIndex-1) == 1)
	{
		if (ExportPostBondData_Arm2(ulBinIndex-1) == 1)
		{
			bReturn = TRUE;
		}
	}
#else
	if (ExportPostBondData(ulBinIndex-1) == 1)
	{
		bReturn = TRUE;
	}
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SaveSPCDisplayData(IPC_CServiceMessage& svMsg)		
{
	BOOL	bReturn = TRUE;

	bReturn = ExportPostBondDisplayData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::PreOnSelectShowPrImage(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSelectDieType = 0;
	LONG lDieNo;
	CString szPath;

	m_szShowImageType	= BPR_HMI_NORM_DIE;
	m_lShowImageNo		= 1;

	if (m_szShowImageType == BPR_HMI_NORM_DIE)
	{
		bSelectDieType = BPR_NORMAL_DIE;
	}
	else
	{
		bSelectDieType = BPR_REFERENCE_DIE;
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

LONG CBondPr::OnSelectShowPrImage(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSelectDieType = 0;
	LONG lDieNo;
	CString szPath;

	if (m_szShowImageType == BPR_HMI_NORM_DIE)
	{
		bSelectDieType = BPR_NORMAL_DIE;
	}
	else
	{
		bSelectDieType = BPR_REFERENCE_DIE;
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

LONG CBondPr::DisplayPRRecordInSummary(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szPath;
	LONG lDieNo;
	
	// Display Normal Die Record
	if (ShowPrImage(szPath, lDieNo, BPR_NORMAL_DIE, 1) == FALSE)
	{
		m_szShowImagePath = "";
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_szShowImagePath = szPath;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//============================================================Teach BHZ1&BHZ2 bond position=========================================
VOID CBondPr::SaveAdjBondPosOffsetPixel(const BOOL bBHZ2, const LONG lBHZAdjBondPosOffsetX_count, const LONG lBHZAdjBondPosOffsetY_count)
{
	PR_COORD stPixel;
	ConvertMotorStepToPixel(lBHZAdjBondPosOffsetX_count, lBHZAdjBondPosOffsetY_count, stPixel);

	if (bBHZ2)
	{
		m_lBondPosOffsetXPixel2 = m_lPrCenterX + stPixel.x;
		m_lBondPosOffsetYPixel2 = m_lPrCenterY + stPixel.y;
	}
	else
	{
		m_lBondPosOffsetXPixel1 = m_lPrCenterX + stPixel.x;
		m_lBondPosOffsetYPixel1 = m_lPrCenterY + stPixel.y;
	}
}


VOID CBondPr::TeachBHZBondPos(const BOOL bBHZ2)
{
	SelectBondCamera();

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetBinTableJoystick(FALSE);
	}

	if (!bBHZ2)
	{
		HmiMessage("Arm 1 will move to bond position!");
		MoveBhToBond(TRUE);
	}
	else
	{
		HmiMessage("Arm 2 will move to bond position!");
		MoveBhToBond_Z2(TRUE);
	}

	PR_DRAW_HOME_CURSOR_CMD 	stHomeCmd;
//	PR_DRAW_CROSS_CMD			stCrossCmd;
	PR_COMMON_RPY				stGenRpy;
	PR_COORD stCenter;

	stCenter.x = (PR_WORD)m_lPrCenterX;
	stCenter.y = (PR_WORD)m_lPrCenterX;

	//Erase Home Cursor
	stHomeCmd.emTmplSize = PR_TMPL_SIZE_0;
	stHomeCmd.coPosition.x = stCenter.x;
	stHomeCmd.coPosition.y = stCenter.y;
	PR_EraseHomeCursorCmd(&stHomeCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stGenRpy);

	//for restore when cancel
	if (bBHZ2)
	{
		m_lTmpBondPosOffsetXPixel = m_lBondPosOffsetXPixel2;
		m_lTmpBondPosOffsetYPixel = m_lBondPosOffsetYPixel2;
	}
	else
	{
		m_lTmpBondPosOffsetXPixel = m_lBondPosOffsetXPixel1;
		m_lTmpBondPosOffsetYPixel = m_lBondPosOffsetYPixel1;
	}

	m_lBondPosOffsetXPixel = m_lTmpBondPosOffsetXPixel;
	m_lBondPosOffsetYPixel = m_lTmpBondPosOffsetYPixel;

	m_pPrGeneral->PRS_DrawCross((short)m_lBondPosOffsetXPixel, (short)m_lBondPosOffsetYPixel, PR_COLOR_GREEN, 500);
}


VOID CBondPr::CalcBHZBondPosPixelToCount(const BOOL bBHZ2, int &siStepX, int &siStepY)
{
	PR_COORD stBondPos; 

	if (bBHZ2)
	{
		m_lBondPosOffsetXPixel2 = m_lBondPosOffsetXPixel;
		m_lBondPosOffsetYPixel2 = m_lBondPosOffsetYPixel;
	}
	else
	{
		m_lBondPosOffsetXPixel1 = m_lBondPosOffsetXPixel;
		m_lBondPosOffsetYPixel1 = m_lBondPosOffsetYPixel;
	}

	stBondPos.x = (PR_WORD)m_lBondPosOffsetXPixel;
	stBondPos.y = (PR_WORD)m_lBondPosOffsetYPixel;

	CalculateDieCompenate(stBondPos, &siStepX, &siStepY);
	DOUBLE dRatio = 1;
	if (m_szGenDieZoomMode[BPR_GEN_NDIE_OFFSET + 1] == "1X")
	{
		dRatio = 0.25;
	}
	else if (m_szGenDieZoomMode[BPR_GEN_NDIE_OFFSET + 1] == "2X")
	{
		dRatio = 0.5;
	}

	siStepX = _round(siStepX * dRatio);
	siStepY = _round(siStepY * dRatio);
}


BOOL CBondPr::ConfirmBHZBondPos(const BOOL bBHZ2, BOOL &bResult, int &siStepX, int &siStepY)
{
	bResult = TRUE;

	CalcBHZBondPosPixelToCount(bBHZ2, siStepX, siStepY);

	HmiMessage("Bond Arm will move to clean collet position!");
	if (!bBHZ2)
	{
		MoveBhToBond(FALSE);
	}
	else
	{
		MoveBhToBond_Z2(FALSE);
	}

	m_pPrGeneral->PRS_EraseCross(m_lBondPosOffsetXPixel, m_lTmpBondPosOffsetYPixel, 0, 500);
	//Draw Home cursor
	CString szErrMsg;
	m_pPrGeneral->DrawHomeCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, m_lPrCenterX, m_lPrCenterY, szErrMsg);

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetBinTableJoystick(TRUE);
	}

	return TRUE;
}

LONG CBondPr::CancelBHZBondPos(const BOOL bBHZ2)
{
	//Erase the current cross
	m_pPrGeneral->PRS_EraseCross((short)m_lBondPosOffsetXPixel, (short)m_lBondPosOffsetYPixel, 0, 500);

	//restore the previous bond pos offset and draw the previous cross
	if (bBHZ2)
	{
		m_lBondPosOffsetXPixel2 = m_lTmpBondPosOffsetXPixel;
		m_lBondPosOffsetYPixel2 = m_lTmpBondPosOffsetYPixel;

	}
	else
	{
		m_lBondPosOffsetXPixel1 = m_lTmpBondPosOffsetXPixel;
		m_lBondPosOffsetYPixel1 = m_lTmpBondPosOffsetYPixel;
	}

	m_pPrGeneral->PRS_DrawCross((short)m_lTmpBondPosOffsetXPixel, (short)m_lTmpBondPosOffsetYPixel, PR_COLOR_GREEN, 500);

	HmiMessage("Bond Arm will move to clean collet position!");
	if (!bBHZ2)
	{
		MoveBhToBond(FALSE);
	}
	else
	{
		MoveBhToBond_Z2(FALSE);
	}

	m_pPrGeneral->PRS_EraseCross((short)m_lTmpBondPosOffsetXPixel, (short)m_lTmpBondPosOffsetYPixel, 0, 500);

	//Draw Home cursor
	CString szErrMsg;
	m_pPrGeneral->DrawHomeCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, m_lPrCenterX, m_lPrCenterY, szErrMsg);

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetBinTableJoystick(TRUE);
	}
	
	return 1;
}

BOOL CBondPr::PrestartConfirmBHZBondPos(const BOOL bBHZ2, BOOL &bResult, int &siStepX, int &siStepY)
{
	CalcBHZBondPosPixelToCount(bBHZ2, siStepX, siStepY);

	siStepX = siStepX * (-1);
	siStepY = siStepY * (-1);
	CString szMsg;
	szMsg.Format("Prestart update Bond Pos Offset, Pixel(%d,%d), step(%d,%d), BH2,%d",m_lBondPosOffsetXPixel,m_lBondPosOffsetYPixel,
		siStepX,siStepY,bBHZ2);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	return TRUE;
}
//=====================================================================================================================


//============================================================Teach BHZ1 bond position=========================================
LONG CBondPr::TeachBHZ1BondPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	TeachBHZBondPos(FALSE); //Bond Arm#1

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::ConfirmBHZ1BondPos(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	ConfirmBHZBondPos(FALSE, stInfo.bResult, stInfo.siStepX, stInfo.siStepY);

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}

LONG CBondPr::CancelBHZ1BondPos(IPC_CServiceMessage& svMsg)
{
	CancelBHZBondPos(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::ResetBHZ1BondPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	m_lBondPosOffsetXPixel1 = m_lPrCenterX;
	m_lBondPosOffsetYPixel1 = m_lPrCenterY;

	ResetRtBHCompXYArrays(FALSE);

	SavePrData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//=============================================================================================================================


//============================================================Teach BHZ2 bond position=========================================
LONG CBondPr::TeachBHZ2BondPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	TeachBHZBondPos(TRUE); //Bond Arm#2

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::ConfirmBHZ2BondPos(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	ConfirmBHZBondPos(TRUE, stInfo.bResult, stInfo.siStepX, stInfo.siStepY);

	svMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	return 1;
}

LONG CBondPr::CancelBHZ2BondPos(IPC_CServiceMessage& svMsg)
{
	CancelBHZBondPos(TRUE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::ResetBHZ2BondPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	m_lBondPosOffsetXPixel2 = m_lPrCenterX;
	m_lBondPosOffsetYPixel2 = m_lPrCenterY;

	ResetRtBHCompXYArrays(TRUE);

	SavePrData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//=============================================================================================================================
//Use Same Cancel Button for BH1 and BH2. TRUE for 1, FALSE for 2
LONG CBondPr::CancelBHZ1OrBHZ2BondPos(IPC_CServiceMessage& svMsg)
{
	BOOL bBHZ1 = TRUE;
	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bBHZ1);

	if(bBHZ1 == TRUE)
	{
		CancelBHZBondPos(FALSE);
	}
	else
	{
		CancelBHZBondPos(TRUE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
//=============================================================================================================================


LONG CBondPr::AdjustCrossHair(IPC_CServiceMessage& svMsg)
{
	PR_COORD			sttmpStartPos;
	PR_WORD				sstmpPixelStep;
	
	typedef struct {
		unsigned char ucDirection;
		unsigned char ucPixelStep;
	} MOVECURSOR;

	MOVECURSOR	stInfo;
	svMsg.GetMsg(sizeof(MOVECURSOR), &stInfo);

	//Get Pixel step move
	switch(stInfo.ucPixelStep)
	{
		case 1:		//10 pixel
			sstmpPixelStep = 10*PR_SCALE_FACTOR;
			break;

		case 2:		//30 pixel
			sstmpPixelStep = 30*PR_SCALE_FACTOR;
			break;
		
		default:	//1 pixel
			sstmpPixelStep = 1*PR_SCALE_FACTOR;
			break;
	}

	sttmpStartPos.x = (PR_WORD)m_lBondPosOffsetXPixel;
	sttmpStartPos.y = (PR_WORD)m_lBondPosOffsetYPixel;

	m_pPrGeneral->PRS_EraseCross((short)m_lBondPosOffsetXPixel, (short)m_lBondPosOffsetYPixel, 0, 500);

	switch(stInfo.ucDirection)
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
	LONG lHalfSrchWndX	= _round((m_lSrchDieAreaX * 1.0 / 4 + 0.5) * m_stGenDieSize[BPR_NORMAL_DIE].x * BPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lHalfSrchWndY	= _round((m_lSrchDieAreaY * 1.0 / 4 + 0.5) * m_stGenDieSize[BPR_NORMAL_DIE].y * BPR_SEARCHAREA_SHIFT_FACTOR);
	INT siDiePixelX		= m_stGenDieSize[BPR_NORMAL_DIE].x / 2;
	INT siDiePixelY		= m_stGenDieSize[BPR_NORMAL_DIE].y / 2;
	LONG lSizeX			= lHalfSrchWndX - siDiePixelX;
	LONG lSizeY			= lHalfSrchWndY - siDiePixelY;
	
	if ( (siDiePixelX == 0) || (siDiePixelY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	

	CString szMsg;
	szMsg.Format("BPR Adjust CrossHair Wnd: X width = %ld, Y Width = %ld; Pitch(%ld %ld); 1/2SrchWnd(%ld %ld)", lSizeX, lSizeY,
					siDiePixelX, siDiePixelY, lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);


	m_lBondPosOffsetXPixel = sttmpStartPos.x;
	m_lBondPosOffsetYPixel = sttmpStartPos.y;

	m_pPrGeneral->PRS_DrawCross((short)m_lBondPosOffsetXPixel, (short)m_lBondPosOffsetYPixel, PR_COLOR_GREEN, 500);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::AdjustCrossHairStepByStep(IPC_CServiceMessage& svMsg)
{
	PR_COORD			sttmpStartPos;
	PR_WORD				sstmpPixelStep;
	
	typedef struct {
		unsigned char ucDirection;
		unsigned char ucPixelStep;
	} MOVECURSOR;

	MOVECURSOR	stInfo;
	svMsg.GetMsg(sizeof(MOVECURSOR), &stInfo);
	LONG lPixelStep = 1;

	//Get Pixel step move
	switch(stInfo.ucPixelStep)
	{
		case 1:		//10 pixel
			sstmpPixelStep = 10*PR_SCALE_FACTOR;
			lPixelStep = 10;
			break;

		case 2:		//30 pixel
			sstmpPixelStep = 30*PR_SCALE_FACTOR;
			lPixelStep = 30;
			break;
		
		default:	//1 pixel
			sstmpPixelStep = 1*PR_SCALE_FACTOR;
			lPixelStep = 1;
			break;
	}

	m_pPrGeneral->PRClearScreenNoCursor(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID);

	sttmpStartPos.x = (PR_WORD)m_lPrCenterX;
	sttmpStartPos.y = (PR_WORD)m_lPrCenterY;
	//stCrossCmd.coCrossCentre = sttmpStartPos;
	//PR_EraseCrossCmd(&stCrossCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stGenRpy);

	switch(stInfo.ucDirection)
	{
		case 1:
			sttmpStartPos.x -= sstmpPixelStep;
			m_lPrCenterOffsetXInPixel -= lPixelStep;	//v4.44A3
			break;

		case 2:
			sttmpStartPos.y += sstmpPixelStep;
			m_lPrCenterOffsetYInPixel += lPixelStep;	//v4.44A3
			break;

		case 3:
			sttmpStartPos.x += sstmpPixelStep;
			m_lPrCenterOffsetXInPixel += lPixelStep;	//v4.44A3
			break;

		default:
			sttmpStartPos.y -= sstmpPixelStep;
			m_lPrCenterOffsetYInPixel -= lPixelStep;	//v4.44A3
			break;
	}
	
	//v4.43T9
	LONG lHalfSrchWndX	= _round((m_lSrchDieAreaX * 1.0 / 4 + 0.5) * m_stGenDieSize[BPR_NORMAL_DIE].x * BPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lHalfSrchWndY	= _round((m_lSrchDieAreaY * 1.0 / 4 + 0.5) * m_stGenDieSize[BPR_NORMAL_DIE].y * BPR_SEARCHAREA_SHIFT_FACTOR);
	INT siDiePixelX		= m_stGenDieSize[BPR_NORMAL_DIE].x / 2;
	INT siDiePixelY		= m_stGenDieSize[BPR_NORMAL_DIE].y / 2;
	LONG lSizeX			= lHalfSrchWndX - siDiePixelX;
	LONG lSizeY			= lHalfSrchWndY - siDiePixelY;

	if ( (siDiePixelX == 0) || (siDiePixelY == 0) )
	{
		lSizeX = 1500;
		lSizeY = 1500;
	}
	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1500;
		lSizeY = 1500;
	}

	CString szMsg;
	szMsg.Format("BPR CrossHair Wnd: X width = %ld, Y Width = %ld; DieSize(%ld, %ld); 1/2SrchWnd(%ld, %ld)", 
					lSizeX, lSizeY,
					siDiePixelX, siDiePixelY, 
					lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	PR_COORD coULCorner;
	PR_COORD coLRCorner;
	coULCorner.x = PR_DEF_CENTRE_X - (PR_WORD)lSizeX;
	coULCorner.y = PR_DEF_CENTRE_Y - (PR_WORD)lSizeY;
	coLRCorner.x = PR_DEF_CENTRE_X + (PR_WORD)lSizeX;
	coLRCorner.y = PR_DEF_CENTRE_Y + (PR_WORD)lSizeY;
	if (coULCorner.x < 500)
		coULCorner.x = 500;
	if (coULCorner.y < 500)
		coULCorner.y = 500;
	if (coLRCorner.x > PR_MAX_COORD - 500)
		coLRCorner.x = PR_MAX_COORD - 500;
	if (coLRCorner.y > PR_MAX_COORD - 500)
		coLRCorner.y = PR_MAX_COORD - 500;
	//DrawRectangleBox(coULCorner, coLRCorner, PR_COLOR_RED);

	//v4.46T27
	szMsg.Format("BPR CrossHair (temp) X=%ld Y=%ld (%ld, %ld)", sttmpStartPos.x, sttmpStartPos.y, lSizeX, lSizeY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	////v4.43T6
	//if (sttmpStartPos.x > coLRCorner.x)
	//	sttmpStartPos.x = coLRCorner.x;
	//if (sttmpStartPos.x < coULCorner.x)
	//	sttmpStartPos.x = coULCorner.x;
	//if (sttmpStartPos.y > coLRCorner.y)
	//	sttmpStartPos.y = coLRCorner.y;
	//if (sttmpStartPos.y < coULCorner.y)
	//	sttmpStartPos.y = coULCorner.y;

	m_lPrCenterX = sttmpStartPos.x;
	m_lPrCenterY = sttmpStartPos.y;

	m_lPrCenterOffsetXInPixel = (m_lPrCenterX - PR_DEF_CENTRE_X) / PR_SCALE_FACTOR;
	m_lPrCenterOffsetYInPixel = (m_lPrCenterY - PR_DEF_CENTRE_Y) / PR_SCALE_FACTOR;

	//stCrossCmd.coCrossCentre = sttmpStartPos;
	//stCrossCmd.emColor = PR_COLOR_GREEN;
	//stCrossCmd.szCrossSize.x = 500;
	//stCrossCmd.szCrossSize.y = 500;
	//PR_DrawCrossCmd(&stCrossCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stGenRpy);

//v4.46T27
szMsg.Format("BPR CrossHair X=%ld Y=%ld", m_lPrCenterX, m_lPrCenterY);
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	CString szErrMsg;
	m_pPrGeneral->DrawHomeCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, m_lPrCenterX, m_lPrCenterY, szErrMsg);
	
	DrawRectangleBox(coULCorner, coLRCorner, PR_COLOR_RED);

	SavePrData();
	return 1;
}

LONG CBondPr::EnableBondPrInspction(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable = FALSE;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bEnable);
	m_bUsePostBond = bEnable;
	SavePrData();
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::TransformBHRTCompXY(IPC_CServiceMessage& svMsg)		//v4.50A9
{
	BOOL bReturn = TRUE;

	UCHAR ucRotation = 0;
	svMsg.GetMsg(sizeof(UCHAR), &ucRotation);

	if (m_bEnableRTCompXY)
	{
		TransformBHCompXYArrays(ucRotation);
		SavePrData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//Set Inspection Detect / Ignore region
LONG CBondPr::SetInspDetectRegion(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lRegionNo = 0;

	//Get Region No from HMI
	svMsg.GetMsg(sizeof(LONG), &lRegionNo);
	if (lRegionNo == 0)
	{
		HmiMessage("window no is 0");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	if (lInitPR != 1)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( m_bUseMouse == FALSE )
	{
		// Erase the original region
		DrawAndEraseCursor(m_stInspDetectWin[lRegionNo-1].coCorner1,PR_UPPER_LEFT,0);
		DrawAndEraseCursor(m_stInspDetectWin[lRegionNo-1].coCorner2,PR_LOWER_RIGHT,0);

		// Define by using mouse
		m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_YELLOW);

		// Set the mouse size first
		m_pPrGeneral->MouseSet2PointRegion(m_stInspDetectWin[lRegionNo-1].coCorner1, m_stInspDetectWin[lRegionNo-1].coCorner2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_YELLOW);

		m_bUseMouse = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::EndInspDetectRegion(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct {
		LONG	lMaxRegion;
		LONG	lRegionNo;
	} REG_TYPE;

	REG_TYPE	stInfo;

	//Get Region No from HMI
	svMsg.GetMsg(sizeof(REG_TYPE), &stInfo);
	if (stInfo.lRegionNo == 0)
	{
		HmiMessage("window no is 0");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	if (lInitPR != 1)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( m_bUseMouse == TRUE )
	{
		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stInspDetectWin[stInfo.lRegionNo-1].coCorner1,
														   m_stInspDetectWin[stInfo.lRegionNo-1].coCorner2);
		if (uwNumOfCorners == 2)
		{
			// Draw the defined region
			DrawRectangleBox(m_stInspDetectWin[stInfo.lRegionNo-1].coCorner1, m_stInspDetectWin[stInfo.lRegionNo-1].coCorner2, PR_COLOR_YELLOW);
		}

		m_bUseMouse = FALSE;
	}

	m_lInspDetectWinNo = stInfo.lMaxRegion;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SetInspIgnoreRegion(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lRegionNo = 0;

	//Get Region No from HMI
	svMsg.GetMsg(sizeof(LONG), &lRegionNo);
	if (lRegionNo == 0)
	{
		HmiMessage("window no is 0");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	if (lInitPR != 1)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( m_bUseMouse == FALSE )
	{
		// Erase the original region
		DrawAndEraseCursor(m_stInspIgnoreWin[lRegionNo-1].coCorner1,PR_UPPER_LEFT,0);
		DrawAndEraseCursor(m_stInspIgnoreWin[lRegionNo-1].coCorner2,PR_LOWER_RIGHT,0);

		// Define by using mouse
		m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_CYAN);

		// Set the mouse size first
		m_pPrGeneral->MouseSet2PointRegion(m_stInspIgnoreWin[lRegionNo-1].coCorner1, m_stInspIgnoreWin[lRegionNo-1].coCorner2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_CYAN);

		m_bUseMouse = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::EndInspIgnoreRegion(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct {
		LONG	lMaxRegion;
		LONG	lRegionNo;
	} REG_TYPE;

	REG_TYPE	stInfo;

	//Get Region No from HMI
	svMsg.GetMsg(sizeof(REG_TYPE), &stInfo);
	if (stInfo.lRegionNo == 0)
	{
		HmiMessage("window no is 0");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	if (lInitPR != 1)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if ( m_bUseMouse == TRUE )
	{
		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stInspIgnoreWin[stInfo.lRegionNo - 1].coCorner1,
														   m_stInspIgnoreWin[stInfo.lRegionNo - 1].coCorner2);

		if (uwNumOfCorners == 2)
		{
			// Draw the defined region
			DrawRectangleBox(m_stInspIgnoreWin[stInfo.lRegionNo-1].coCorner1, m_stInspIgnoreWin[stInfo.lRegionNo-1].coCorner2, PR_COLOR_CYAN);
		}
		m_bUseMouse = FALSE;
	}

	m_lInspIgnoreWinNo = stInfo.lMaxRegion;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
// end inspection widnow

LONG CBondPr::CheckReferenceDie(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
//AfxMessageBox(m_szLrnDieType, MB_SYSTEMMODAL);
//AfxMessageBox(m_lCurRefDieNo, MB_SYSTEMMODAL);
	if (m_szLrnDieType == BPR_HMI_REF_DIE && m_lCurRefDieNo == 1)
	{
		m_bHMIReferenceDie = TRUE;
	}
	else 
	{
		m_bHMIReferenceDie = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SearchColletHole(IPC_CServiceMessage &svMsg)		//v4.47A5
{
	BOOL bReturn = TRUE;
//	PR_WORD		rtnPR;
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	PR_COORD	stDieOffset; 
	INT stDieOffsetx = 0;
	INT stDieOffsety = 0;

	BOOL bBH2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBH2);		//v4.48A8

	typedef struct {
		BOOL	bStatus;
		INT		nOffsetX;
		INT		nOffsetY;
	} SRCH_CHOLE_RESULT;

	SRCH_CHOLE_RESULT stResult;
	stResult.bStatus = FALSE;
	stResult.nOffsetX = 0;
	stResult.nOffsetY = 0;

	if ( (m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE) || (m_bDieCalibrated == FALSE) )
	{
		HmiMessage_Red_Yellow("Bond PR: no record ID found in SearchColletHole");
		svMsg.InitMessage(sizeof(SRCH_CHOLE_RESULT), &stResult);
		return 1;
	}


	PR_WIN stSrchArea;
	stSrchArea.coCorner1.x = PR_MAX_WIN_ULC_X;
	stSrchArea.coCorner1.y = PR_MAX_WIN_ULC_Y;
	stSrchArea.coCorner2.x = PR_MAX_WIN_LRC_X;
	stSrchArea.coCorner2.y = PR_MAX_WIN_LRC_Y;

//	PR_DETECT_SHAPE_RPY2 pstRpy;


	//rtnPR = AutoSearchCircle(m_lPrCenterX, m_lPrCenterY, m_stGenDieSize[BPR_GEN_RDIE_OFFSET+3].x, m_stGenDieSize[BPR_GEN_RDIE_OFFSET+3].y, stDieOffsetx, stDieOffsety);
	ULONG ulResult = 0;
	if (bBH2)
	{
		//Use Normal Die Record #2
		ulResult = ManualSearchDie(BPR_NORMAL_DIE, 2, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchArea.coCorner1, stSrchArea.coCorner2, 0, 17);
	}
	else
	{
		//Use Normal Die Record #1
		ulResult = ManualSearchDie(BPR_NORMAL_DIE, 1, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchArea.coCorner1, stSrchArea.coCorner2, 0, 18);
	}

	if (DieIsAlignable(usDieType) == TRUE)
	{
		//CalculateDieCompenate(stDieOffset, &stDieOffsetx, &stDieOffsety);
		//stRelMove.x = (PR_WORD)m_lPrCenterX - stDieOffset.x;
		//stRelMove.y = (PR_WORD)m_lPrCenterY - stDieOffset.y;
		stResult.bStatus	= TRUE;
		stResult.nOffsetX	= m_lPrCenterX - stDieOffset.x;
		stResult.nOffsetY	= m_lPrCenterY - stDieOffset.y;
	}
	else
	{
		stResult.bStatus	= FALSE;
	}

	svMsg.InitMessage(sizeof(SRCH_CHOLE_RESULT), &stResult);
	return 1;
}

LONG CBondPr::GenerateConfigData(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	CString szConfigFile = (*m_psmfSRam)["MS896A"]["ParameterListPath"];
		//_T("c:\\MapSorter\\UserData\\Parameters.csv");
	CStdioFile oFile;
	CString szLine;
	CString szMsg;

	szMsg = _T("BPR: Generate Configuration Data");
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	if (oFile.Open(szConfigFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		szMsg.Format("BPR: Generate Configuration Data - open fail");
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		return FALSE;
	}

	oFile.SeekToEnd();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp ->GetCustomerName() == "EverVision")
	{
		oFile.WriteString("[Bond Inspection Setup]\n");

		szMsg.Format("%d",m_lBPRLrnCoaxLightLevel[0]);
		oFile.WriteString("Bond Coaxial light,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lBPRLrnRingLightLevel[0]);
		oFile.WriteString("Bond Ring light,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lBPRLrnSideLightLevel[0]);
		oFile.WriteString("Bond Side Light,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lSrchDieScore);
		oFile.WriteString("Die Pass Score,"			+ szMsg + "\n");

		szMsg.Format("%d",m_bSrchEnableChipCheck);
		oFile.WriteString("Check Chip ON/OFF,"		+ szMsg + "\n");


		szMsg.Format("%.2f",m_dSrchChipArea);
		oFile.WriteString("Check Chip %,"		+ szMsg + "\n");

		szMsg.Format("%d",m_bSrchEnableDefectCheck);
		oFile.WriteString("Check Defect ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%.2f",m_dSrchSingleDefectArea);
		oFile.WriteString("Single Detect %,"					+ szMsg + "\n");

		szMsg.Format("%.2f",m_dSrchTotalDefectArea);
		oFile.WriteString("Total Defect %,"		+ szMsg + "\n");

		szMsg.Format("%.2f",m_dSrchDieAreaX);
		oFile.WriteString("Search Area Size X,"			+ szMsg + "\n");

		szMsg.Format("%.2f",m_dSrchDieAreaY);
		oFile.WriteString("Search Area Size Y,"			+ szMsg + "\n");

		szMsg.Format("%d",m_bUsePostBond);
		oFile.WriteString("Enable Runtime Inspection ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%d",m_bBinTableAngleCheck);
		oFile.WriteString("Check Maximum Angle ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%.f",m_dMaxAngle);
		oFile.WriteString("Max Angle,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lMaxAngleAllow);
		oFile.WriteString("Max Allowed,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lMaxAccAngleAllow);
		oFile.WriteString("Max ACC Allowed,"	+ szMsg + "\n");

		szMsg.Format("%d",m_bPlacementCheck);
		oFile.WriteString("Check XY Shift ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%.f",m_dDieShiftX);
		oFile.WriteString("Max X,"		+ szMsg + "\n");

		szMsg.Format("%.f",m_dDieShiftY);
		oFile.WriteString("Max Y,"		+ szMsg  + "\n");

		szMsg.Format("%d",m_lMaxShiftAllow);
		oFile.WriteString("Max Allowed,"		+ szMsg + "\n");

		szMsg.Format("%d",m_bDefectCheck);
		oFile.WriteString("Check Defect ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lMaxDefectAllow);
		oFile.WriteString("Max Die Allowed,"		+ szMsg + "\n");

		szMsg.Format("%d",m_bEmptyCheck);
		oFile.WriteString("Check Empty ON/OFF,"		+ szMsg + "\n");

		szMsg.Format("%d",m_lMaxEmptyAllow);
		oFile.WriteString("Count,"		+ szMsg + "\n");
	}
	else if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		oFile.WriteString("[Bond Inspection Setup]\n");
		szMsg.Format("%d,%d",m_lPrCenterX, m_lPrCenterY);
		oFile.WriteString("PR Corsshair Offset XY,"		+ szMsg + "\n");
	}
	oFile.Close();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::BPR_BondLeftDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if( AutoSearchDie() )
	{
		if( AutoGrabDieDone() )
		{
			LONG lSPCResult = AutoDieResult();
			if (lSPCResult == TRUE)
			{
				lSPCResult = OpCheckRelOffsetinFOV();	
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	start to bond left die


VOID CBondPr::GetAxisInformation()
{
	return;
}

LONG CBondPr::PRIDDetailInfo(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szTemp;
	if(m_szLrnDieType == BPR_HMI_NORM_DIE)
	{
		if(m_lCurRefDieNo == 1)
		{
			m_szDieTypeInfo = "NORMAL_PR_ARM1_INDEX";
		}
		else if(m_lCurRefDieNo == 2)
		{
			m_szDieTypeInfo = "NORMAL_PR_ARM2_INDEX";
		}
		else if(m_lCurRefDieNo == 3)
		{
			m_szDieTypeInfo = "realignment FF mode";
		}
		else
		{
			m_szDieTypeInfo = "No die type";
		}
	}
	else if(m_szLrnDieType == BPR_HMI_REF_DIE)
	{
		if(m_lCurRefDieNo == 1)
		{
			m_szDieTypeInfo = "Prebond";
		}
		else if(m_lCurRefDieNo == 2)
		{
			m_szDieTypeInfo = "no use";
		}
		else if(m_lCurRefDieNo == 3)
		{
			m_szDieTypeInfo = "no use";
		}
		else if(m_lCurRefDieNo == 4)
		{
			m_szDieTypeInfo = "BT uplook collet hole";
		}
		else if(m_lCurRefDieNo == 5)
		{
			m_szDieTypeInfo = "Ejector Pin";
		}
		else if(m_lCurRefDieNo == 6)
		{
			m_szDieTypeInfo = "no use";
		}
		else if(m_lCurRefDieNo == 7)
		{
			m_szDieTypeInfo = "no use";
		}
		else if(m_lCurRefDieNo == 8)
		{
			m_szDieTypeInfo = "no use";
		}
		else if(m_lCurRefDieNo == 9)
		{
			m_szDieTypeInfo = "NG grade PR";
		}
		else
		{
			m_szDieTypeInfo = "No die type";
		}
	}

	LONG lDieNo = GetDieNo(m_bSelectDieType, m_lCurRefDieNo);
	LONG lPrID = m_ssGenPRSrchID[lDieNo];
	szTemp.Format("  PRID=%d",lPrID);
	m_szDieTypeInfo += szTemp;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::ConfirmSearchNGDiePara(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CUIntArray aulNGGradeBlockList;

	CNGGrade *pNGGrade = CNGGrade::Instance();
	pNGGrade->GetNGGradeBlockList(aulNGGradeBlockList);
	if (pNGGrade->IsHaveNGGrade())
	{
		m_lGenSrchDieScore[BPR_GEN_NG_GRADE_INDEX_WITH_OFFSET]	= m_lGenSrchDieScore[BPR_NORMAL_DIE];//pNGGrade->m_lSrchNGDieScore; //Reference 9
		pNGGrade->m_lSrchNGDieScore								= m_lGenSrchDieScore[BPR_NORMAL_DIE];
		CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
		pWaferPr->SetNGGradeScore(pNGGrade->m_lSrchNGDieScore);

		for (LONG i = 0; i < aulNGGradeBlockList.GetSize(); i++)
		{
			m_lCurrentBinBlock = (LONG)aulNGGradeBlockList[i];
			GetHMIVariable();
		}
	}

	LONG lNormalGradeBlock = 1;
	for (lNormalGradeBlock = 1; lNormalGradeBlock <= BPR_MAX_BINBLOCK; lNormalGradeBlock++)
	{
		if (!pNGGrade->IsInNGGradeBlockList(lNormalGradeBlock, aulNGGradeBlockList))
		{
			break;
		}
	}

	for (LONG i = 0; i < pNGGrade->m_aulNGGradeBlockList.GetSize(); i++)
	{
		LONG lOldBinBlock = (LONG)pNGGrade->m_aulNGGradeBlockList[i];
		if (!pNGGrade->IsInNGGradeBlockList(lOldBinBlock, aulNGGradeBlockList))
		{
			CopyBinBlockVariable(lNormalGradeBlock, lOldBinBlock);
		}
	}
	pNGGrade->m_aulNGGradeBlockList.RemoveAll();
	pNGGrade->GetNGGradeBlockList(pNGGrade->m_aulNGGradeBlockList);

	SetUseBinMapBondArea();
	SavePrData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::ShowSearchDiePara(IPC_CServiceMessage& svMsg)
{
	BOOL bNGGrade = TRUE;
	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bNGGrade);		//v4.48A8

	CNGGrade *pNGGrade = CNGGrade::Instance();
	pNGGrade->m_aulNGGradeBlockList.RemoveAll();
	pNGGrade->GetNGGradeBlockList(pNGGrade->m_aulNGGradeBlockList);

	m_lCurrentBinBlock = 1;
	if (bNGGrade == TRUE)
	{
		m_lCurrentBinBlock = pNGGrade->GetCurrentNGGradeBlock();
	}
	//Update HMI variable
	UpdateHMIVariable();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::ClearAccEmptyCounter(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	ULONG ulBinBlkFull = 0;
	svMsg.GetMsg(sizeof(ULONG), &ulBinBlkFull);

	m_lOutAccEmptyCounter[ulBinBlkFull] = 0;
	m_lOutAccEmptyCounter2[ulBinBlkFull] = 0;
	m_lOutAccEmptyCounterTotal[ulBinBlkFull] = 0;
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::DisplayPRIdListRecord(IPC_CServiceMessage &svMsg)
{
	PR_COORD				stCorner1;
	PR_COORD				stCorner2;
	LONG					lDieNo, lPrZoomMode;
	svMsg.GetMsg(sizeof(LONG), &lDieNo);
	LONG lPrID = m_ssGenPRSrchID[lDieNo];

	//Change the zoom mode before showing the record
	LONG lOldZoom = BPR_GetRunZoom();
	if (m_szGenDieZoomMode[lPrID] == PR_ZOOM_VALUE_1X)
	{
		lPrZoomMode = CPrZoomSensorMode::PR_ZOOM_MODE_1X;
	}
	else if (m_szGenDieZoomMode[lPrID] == PR_ZOOM_VALUE_2X)
	{
		lPrZoomMode = CPrZoomSensorMode::PR_ZOOM_MODE_2X;
	}
	else if (m_szGenDieZoomMode[lPrID] == PR_ZOOM_VALUE_4X)
	{
		lPrZoomMode = CPrZoomSensorMode::PR_ZOOM_MODE_4X;
	}
	else if (m_szGenDieZoomMode[lPrID] == PR_ZOOM_VALUE_FF)
	{
		lPrZoomMode = CPrZoomSensorMode::PR_ZOOM_MODE_FF;
	}
	BPR_LiveViewZoom(lPrZoomMode);

	PR_UWORD uwPRStatus = m_pPrGeneral->DisplayLearnInfo(lPrID, GetPrCenterX(), GetPrCenterY(), GetPostBondPRSenderID(), GetPostBondPRReceiverID());
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
	SelectBondCamera();
	BPR_LiveViewZoom(lOldZoom);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondPr::ExtractObjectCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	typedef struct 
	{
		BOOL	bStatus;
		double	dOffsetX;
		double	dOffsetY;
		DOUBLE	dAngle;
	} SRCH_RESULT;

	SRCH_RESULT		stResult;
	double dX = 0,dY = 0,dAngle = 0;
	ExtractObjectCmd(FALSE,dX,dY,dAngle);
	stResult.dOffsetX = dX;
	stResult.dOffsetY = dY;
	stResult.dAngle	  = dAngle;

	svMsg.InitMessage(sizeof(SRCH_RESULT), &stResult);
	return 1;
}