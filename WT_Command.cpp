/////////////////////////////////////////////////////////////////
// WT_Command.cpp : HMI Registered Command of the CWaferTable class
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
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "HmiDataManager.h"
#include "WT_CmdName.h"
#include "MS_SecCommConstant.h"
#include "StrInputDlg.h"
// prescan relative code
#include "PrescanUtility.h"
#include "WaferPr.h"
//#include "StrHdrInputDlg2.h"
#include "WL_Constant.h"
#include "PrescanInfo.h"
#include "Utility.H"
#include "MarkDieRegionManager.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#ifndef MS_DEBUG		//v4.47T5
//	#include "mxml.h"	
//#endif

#include "WAF_CWaferZoomWindowView.h"
#include "gdiplus.h"

#define	AUTO_LOAD_MAP_RETRY		10

VOID CWaferTable::RegisterVariables()
{
	int i;
	CString szText;

	try
	{
		RegVariable(_T("WM_szSamplingRescanIgnoreGradeList"), &m_pWaferMapManager->m_szSamplingRescanIgnoreGradeList); // 4.50D10 
		RegVariable(_T("WT_szGradeDisplayMode"), &m_szGradeDisplayMode); // 4.50D10 
		RegVariable(_T("WT_lGradeDisplayMode"),  &m_lGradeDisplayMode); 

		RegVariable(_T("WT_bHomeSnr_X"),	&m_bHomeSnr_X);
		RegVariable(_T("WT_bHomeSnr_Y"),	&m_bHomeSnr_Y);
		RegVariable(_T("WT_bHomeSnr_T"),	&m_bHomeSnr_T);
		RegVariable(_T("WT_bLimitSnr_X"),	&m_bLimitSnr_X);
		RegVariable(_T("WT_bLimitSnr_Y"),	&m_bLimitSnr_Y);

		RegVariable(_T("WT_bHomeSnr_X2"),	&m_bHomeSnr_X2);
		RegVariable(_T("WT_bHomeSnr_Y2"),	&m_bHomeSnr_Y2);
		RegVariable(_T("WT_bHomeSnr_T2"),	&m_bHomeSnr_T2);
		RegVariable(_T("WT_bLimitSnr_X2"),	&m_bLimitSnr_X2);
		RegVariable(_T("WT_bLimitSnr_Y2"),	&m_bLimitSnr_Y2);
		RegVariable(_T("WT_bEJTLLimit"),	&m_bEJTLLimit);		//v4.38T6
		RegVariable(_T("WT_bEJTULimit"),	&m_bEJTULimit);		//v4.38T6

		RegVariable(_T("WT_lPLLMWaferRow"),	&m_lPLLMWaferRow);
		RegVariable(_T("WT_lPLLMWaferCol"),	&m_lPLLMWaferCol);//input wafer size for rebel.

		RegVariable(_T("WT_lEnc_X"),		&m_lEnc_X1);
		RegVariable(_T("WT_lEnc_Y"),		&m_lEnc_Y1);
		RegVariable(_T("WT_lEnc_T"),		&m_lEnc_T1);

		RegVariable(_T("WT_lEnc_X2"),		&m_lEnc_X2);
		RegVariable(_T("WT_lEnc_Y2"),		&m_lEnc_Y2);
		RegVariable(_T("WT_lEnc_T2"),		&m_lEnc_T2);

		RegVariable(_T("WT_bSel_X"),		&m_bSel_X);
		RegVariable(_T("WT_bSel_Y"),		&m_bSel_Y);
		RegVariable(_T("WT_bSel_T"),		&m_bSel_T);

		RegVariable(_T("WT_bSel_X2"),		&m_bSel_X2);
		RegVariable(_T("WT_bSel_Y2"),		&m_bSel_Y2);
		RegVariable(_T("WT_bSel_T2"),		&m_bSel_T2);

		RegVariable(_T("WT_lStart_X"),		&m_lStart_X);
		RegVariable(_T("WT_lStart_Y"),		&m_lStart_Y);
		RegVariable(_T("WT_lStart_T"),		&m_lStart_T);

		RegVariable(_T("WT_bAlignWafer"),			&m_bAlignedWafer);
		RegVariable(_T("WT_bAutoAlignWafer"),		&m_bAutoAlignWafer);
		RegVariable(_T("WT_bEnableBarcodeLoadMap"), &m_bEnableBarcodeLoadMap);
		RegVariable(_T("WT_bBarCodeSearchMap"),		&m_bBarCodeSearchMap);		//v3.54T1
		RegVariable(_T("WT_bBarCodeSearchFolder"),	&m_bBarCodeSearchFolder);
		RegVariable(_T("WT_bBarCodeSearchPartMap"),	&m_bBarCodeSearchPartMap);
		RegVariable(_T("WT_szSearchFolderName"),	&m_szSearchFolderName);
		RegVariable(_T("WT_bEnableSuffix"),			&m_bEnableSuffix);
		RegVariable(_T("WT_szSuffixName"),			&m_szSuffixName);
		RegVariable(_T("WT_bEnableLocalLoadMap"),	&m_bEnableLocalLoadMap);
		RegVariable(_T("WT_bLocalLoadMapManualMode"),		&m_bUserModeLocalLoadMap);
		RegVariable(_T("WT_bEnableAutoLoadBackupMap"),		&m_bEnableAutoLoadBackupMap);
		RegVariable(_T("WT_bMultiGradeSortingtoSingleBin"),	&m_bMultiGradeSortingtoSingleBin);	//v4.15T8	//Osram Germany

		// Power On/Off
		RegVariable(_T("WT_bIsPowerOn_X"),			&m_bIsPowerOn_X);
		RegVariable(_T("WT_bIsPowerOn_Y"),			&m_bIsPowerOn_Y);
		RegVariable(_T("WT_bIsPowerOn_T"),			&m_bIsPowerOn_T);

		RegVariable(_T("WT_bIsPowerOn_X2"),			&m_bIsPowerOn_X2);
		RegVariable(_T("WT_bIsPowerOn_Y2"),			&m_bIsPowerOn_Y2);
		RegVariable(_T("WT_bIsPowerOn_T2"),			&m_bIsPowerOn_T2);

		// For wafer Process setup
		RegVariable(_T("WT_lWaferUnloadX"),			&m_lWaferUnloadX);
		RegVariable(_T("WT_lWaferUnloadY"),			&m_lWaferUnloadY);
		RegVariable(_T("WT_lWaferCenterX"),			&m_lWaferCenterX);
		RegVariable(_T("WT_lWaferCenterY"),			&m_lWaferCenterY);
		RegVariable(_T("WT_lWaferWorkingAngle"),	&m_lWaferWorkingAngle);
		
		RegVariable(_T("WT_lWaferCalibX"),			&m_lWaferCalibX);
		RegVariable(_T("WT_lWaferCalibY"),			&m_lWaferCalibY);
		
		RegVariable(_T("WT_lWaferCalibX2"),			&m_lWaferCalibX2);
		RegVariable(_T("WT_lWaferCalibY2"),			&m_lWaferCalibY2);

		RegVariable(_T("WT_lWaferEmptyLimit"),		&m_lWaferEmptyLimit);
		RegVariable(_T("WT_lWaferStreetLimit"),		&m_lWaferStreetLimit);
		RegVariable(_T("WT_lWaferIndexPath"),		&m_lWaferIndexPath);
		RegVariable(_T("WT_dWaferDiameter"),		&m_dWaferDiameter);

		RegVariable(_T("WT_bIsCircleLimit"),		&m_bIsCircleLimit);
		RegVariable(_T("WT_ucWaferLimitType"),		&m_ucWaferLimitType);
		RegVariable(_T("WT_lWaferPolyLimitNo"),		&m_lWaferPolyLimitNo);
		RegVariable(_T("WT_ucWaferLimitTemplateNoSel"),	&m_ucWaferLimitTemplateNoSel);
		RegVariable(_T("WT_LastLotNumber"),			&m_szLastLotNumber);

		// For Step Move
		RegVariable(_T("WT_lStepMove"),				&m_lStepMove);

		// For Repeat Cycle when burn-in
		//RegVariable(_T("WT_bRepeatCycle"),		&m_bRepeatCycle);

		// For Pick & Place Algorithm (pick without map)
		RegVariable(_T("WT_bEnablePickAndPlace"),	&m_bEnablePickAndPlace);
		RegVariable(_T("WT_lPickAndPlaceWalkPath"), &m_lPickAndPlaceWalkPath);
		RegVariable(_T("WT_bReply"),				&m_bReply);

		// For Burn In
		RegVariable(_T("WT_szBurnInMapFile"),		&m_szBurnInMapFile);
		RegVariable(_T("WT_bBurnInAutoMap"),		&m_bBurnInAutoMap);
		RegVariable(_T("WT_bBurnInAutoGrade"),		&m_bBurnInAutoGrade);
		RegVariable(_T("WT_lBurnInMapDieCount"),	&m_lBurnInMapDieCount);

		//For output file generating
		
		RegVariable(_T("WT_bEnableHeaderBtn"),		&m_bEnableHeaderBtn);
		RegVariable(_T("WT_lCurDefineHeaderPageNo"),&m_lCurDefineHeaderPageNo);
		RegVariable(_T("WT_lNoOfWaferMapCols"),		&m_lNoOfWaferMapCols);
		RegVariable(_T("WT_szMapHeaderFileName"),	&m_szMapHeaderFileName);

		for (i=1; i<WT_MAX_MAP_COL; i++)
		{
			szText.Format("WT_szWaferMapCol%d", i); 
			RegVariable(_T(szText), &m_szWaferMapCol[i]);

			szText.Format("WT_bIfEnableWaferMapCol%dCtrl", i); 
			RegVariable(_T(szText), &m_bIfEnableWaferMapColCtrl[i]);

			szText.Format("WT_lWMColIndex%d", i);
			RegVariable(_T(szText), &m_lWaferMapColCtrlIndex[i]);
		}

		RegVariable(_T("WT_bEnableOutputBtn"), &m_bEnableOutputBtn);
		RegVariable(_T("WT_lNoOfOutFileCols"), &m_lNoOfOutFileCols);
		RegVariable(_T("WT_lCurOutputFilePageNo"), &m_lCurOutputFilePageNo);
		
		RegVariable(_T("WT_szOutputFormatFileName"), &m_szOutputFormatFileName);
		for (i=1; i<WT_MAX_OUTPUT_COL; i++)
		{
			szText.Format("WT_szOutFileCol%d", i); 
			RegVariable(_T(szText), &m_szOutFileCol[i]);

			szText.Format("WT_bOutFileCol%dMin", i); 
			RegVariable(_T(szText), &m_bOutFileColMin[i]);

			szText.Format("WT_bOutFileCol%dMax", i); 
			RegVariable(_T(szText), &m_bOutFileColMax[i]);

			szText.Format("WT_bOutFileCol%dAvg", i); 
			RegVariable(_T(szText), &m_bOutFileColAvg[i]);

			szText.Format("WT_bOutFileCol%dStd", i); 
			RegVariable(_T(szText), &m_bOutFileColStd[i]);

			szText.Format("WT_bIfEnableOutCol%dCtrl", i); 
			RegVariable(_T(szText), &m_bIfEnableOutColCtrl[i]);

			szText.Format("WT_lOFCtrlIndex%d", i);
			RegVariable(_T(szText), &m_lOutColCtrlIndex[i]);

		}

		RegVariable(_T("WT_szMapFilePath"),			&m_szMapFilePath);
		RegVariable(_T("WT_szMapFileExtension"),	&m_szMapFileExtension);
		RegVariable(_T("WT_szMapFilename"),			&m_szBCMapFilename);
		
		RegVariable(_T("WT_szMapDisplayFilename"),	&m_szMapDisplayFilename);

		RegVariable(_T("WT_szBackupMapPath"),		&m_szBackupMapPath);
		RegVariable(_T("WT_szBackupMapExtension"),	&m_szBackupMapExtension);

		RegVariable(_T("WT_szUploadBarcodeIDFilePath"),		&m_szUploadBarcodeIDFilePath);

		RegVariable(_T("WT_ucMapRotation"),			&m_ucMapRotation);
		RegVariable(_T("WT_ucAutoWaferRotation"),	&m_ucAutoWaferRotation);
		RegVariable(_T("WT_ucAutoBCRotation"),		&m_ucAutoBCRotation);		//v4.46T20	//PLSG
		
		RegVariable(_T("WT_ucMapEdgeSize"),			&m_ucMapEdgeSize);
		RegVariable(_T("WT_bMapHoriFlip"),			&m_bMapHoriFlip);
		RegVariable(_T("WT_bMapVertFlip"),			&m_bMapVertFlip);
		RegVariable(_T("WT_bShowFirstDie"),			&m_bShowFirstDie);
		RegVariable(_T("WT_lSortingMode"),			&m_lSortingMode);
		RegVariable(_T("WT_ulMinGradeCount"),		&m_ulMinGradeCount);
		RegVariable(_T("WT_ulMinGradeBinNo"),		&m_ulMinGradeBinNo);		//v4.38T7
		RegVariable(_T("WT_bMultiWaferSupport"),	&m_bMultiWaferSupport);		//v2.91T1
		RegVariable(_T("WT_ucSpiralPickDirection"), &m_ucSpiralPickDirection);	//v2.95T1
		RegVariable(_T("WT_bNewEjrSequence"),		&m_bNewEjectorSequence);	//v2.96T4
		RegVariable(_T("WT_lSpecialRefDieGrade"),	&m_lSpecialRefDieGrade);	//v3.24T1	//CMLT specail REF-DIE grade support
		RegVariable(_T("WT_bOsramResortMode"),		&m_bOsramResortMode);		//v4.21T3
		RegVariable(_T("WT_bAOsramDeny"),			&m_bOsramDenyHMI);
		RegVariable(_T("WT_bEnableOSRAM"),			&m_bEnableOSRAM);
		RegVariable(_T("WT_lWaitEjectorUp"),		&m_lWaitEjectorUp);
		RegVariable(_T("WT_lRotateCount"),			&m_lRotateCount);
		RegVariable(_T("WT_lLookForwardCounter"),	&m_lLookForwardCounter);	//v3.55
		RegVariable(_T("WT_lNoLookForwardCounter"),	&m_lNoLookForwardCounter);	//v3.55

		// Joystick
		RegVariable(_T("WT_lJoystickLevel"),		&m_lJoystickLevel);
		RegVariable(_T("WT_bJoystickOn"),			&m_bJoystickOn);
		RegVariable(_T("WT_bXJoystickOn"),			&m_bXJoystickOn);
		RegVariable(_T("WT_bYJoystickOn"),			&m_bYJoystickOn);

		RegVariable(_T("WT_bStartPoint"),			&m_bStartPoint);

		RegVariable(_T("WT_bScnLoaded"),			&m_bScnLoaded);				// Flag to indicate whether SCN file is loaded
		RegVariable(_T("WT_bEnableGlobalTheta"),	&m_bEnableGlobalTheta);
		RegVariable(_T("WT_bAutoSrchHome"),			&m_bAutoSrchHome);
		RegVariable(_T("WT_bResetMapStatus"),		&m_bResetMapStatus);
		RegVariable(_T("WT_ucAlignStepSize"),		&m_ucAlignStepSize);
		RegVariable(_T("WT_dMoveBackAngle"),		&m_dMoveBackAngle);			//v2.82
		RegVariable(_T("WT_bCheckCOR"),				&m_bCheckCOR);				//Block2
		RegVariable(_T("WT_bUseMultiCorOffset"),	&m_bUseMultiCorOffset);		//v3.44

		RegVariable(_T("WT_lSCNIndex_X"),			&m_lSCNIndex_X);
		RegVariable(_T("WT_lSCNIndex_Y"),			&m_lSCNIndex_Y);

		RegVariable(_T("WT_lCorBackPitchTolX"),		&m_lCorBackPitchTolX);		
		RegVariable(_T("WT_lCorBackPitchTolY"),		&m_lCorBackPitchTolY);		
		RegVariable(_T("WT_lCorMovePitchTolX"),		&m_lCorMovePitchTolX);		
		RegVariable(_T("WT_lCorMovePitchTolY"),		&m_lCorMovePitchTolY);		
		RegVariable(_T("WT_lPredScanPitchTolX"),	&m_lPredScanPitchTolX);		
		RegVariable(_T("WT_lPredScanPitchTolY"),	&m_lPredScanPitchTolY);		
		RegVariable(_T("WT_lPitchX_Tolerance"),		&m_lPitchX_Tolerance);		
		RegVariable(_T("WT_lPitchY_Tolerance"),		&m_lPitchY_Tolerance);	

		RegVariable(_T("WT_bDiePitchCheck"),		&m_bDiePitchCheck);		
		RegVariable(_T("WT_bLimitCheck"),			&m_bLimitCheck);		
		RegVariable(_T("WT_bAbsDiePitchCheck"),		&m_bAbsDiePitchCheck);			//v2.78T2

		// For Block Function
		RegVariable(_T("WT_lMnSrchRefGrid"),	&m_lMnSrchRefGrid);
		RegVariable(_T("WT_lMnSrchHomeGrid"),	&m_lMnSrchHomeGrid);
		RegVariable(_T("WT_lMnMaxJumpCtr"),		&m_lMnMaxJumpCtr);
		RegVariable(_T("WT_lMnMaxJumpEdge"),	&m_lMnMaxJumpEdge);
		RegVariable(_T("WT_lMnNoDieGrade"),		&m_lMnNoDieGrade);
		RegVariable(_T("WT_lMnAlignReset"),		&m_lMnAlignReset);
		RegVariable(_T("WT_lMnMinDieForPick"),	&m_lMnMinDieForPick);
		RegVariable(_T("WT_bMnEdgeAlign"),		&m_bMnEdgeAlign);			//Blcok2
		RegVariable(_T("WT_lMnPassPercent"),	&m_lMnPassPercent);			//Block2
		RegVariable(_T("WT_bMnDigitF"),			&m_bOCRDigitalF);				//Block2
		RegVariable(_T("WT_lMnAlignCheckRow"),	&m_lMnAlignCheckRow);
		RegVariable(_T("WT_lMnAlignCheckCol"),	&m_lMnAlignCheckCol);
		RegVariable(_T("WT_lRealignDieNum"),	&m_lRealignDieNum);			//Block2
		RegVariable(_T("WT_bMnPositionSwitch"),	&m_bMnPositionSwitch);
		RegVariable(_T("WT_lNumOfRetry"),		&m_lNumOfRetry);
		RegVariable(_T("WT_lMapRetryDelay"),	&m_lMapRetryDelay);
		RegVariable(_T("WT_lCurrBlkNum"),		&m_lCurrBlockNum);					//v3.30T3
		RegVariable(_T("WT_bAutoUpdateRefDieOffset"), &m_bAutoUpdateRefDieOffset);	//Block2

		RegVariable(_T("WT_lLongJumpDelay"),		&m_lLongJumpDelay);			//MS100Plus		//v4.10T5
		RegVariable(_T("WT_lLongJumpMotTime"),		&m_lLongJumpMotTime);		//MS100Plus		//v4.10T5

		RegVariable(_T("WT_bEnableXCheck"),			&m_bEnableXCheck);		
		RegVariable(_T("WT_bPickCrossCheckDie"),	&m_bPickCrossCheckDie);		//v3.57	
		RegVariable(_T("WT_lXCheckToleranceX"),		&m_lXCheckToleranceX);		
		RegVariable(_T("WT_lXCheckToleranceY"),		&m_lXCheckToleranceY);		

		RegVariable(_T("WT_lRefCrossRow"),			&m_lRefCrossRow);		
		RegVariable(_T("WT_lRefCrossCol"),			&m_lRefCrossCol);		
		RegVariable(_T("WT_lRefCrossWidth"),		&m_lRefCrossWidth);		

		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		RegVariable(_T("WT_lSubRows"),				&pSRInfo->m_ulSubRows);
		RegVariable(_T("WT_lSubCols"),				&pSRInfo->m_ulSubCols);
		RegVariable(_T("WT_lSubSortMode"),			&m_lSubSortMode);
		RegVariable(_T("WT_bSortOuterHmi"),			&m_bSortOuterHmi);

		RegVariable(_T("WT_bIgnoreRegion"),			&m_bIgnoreRegion);
		RegVariable(_T("WT_lCurrentSelectRegion"),	&m_lCurrentSelectRegion);
		RegVariable(_T("WT_ulSelectRegion1ULX"),	&m_ulSelectRegionULX[0]);		//v3.49T5
		RegVariable(_T("WT_ulSelectRegion1ULY"),	&m_ulSelectRegionULY[0]);
		RegVariable(_T("WT_ulSelectRegion1LRX"),	&m_ulSelectRegionLRX[0]);
		RegVariable(_T("WT_ulSelectRegion1LRY"),	&m_ulSelectRegionLRY[0]);
		RegVariable(_T("WT_ucSelectChangeGrade"),	&m_ucSelectChangeGrade);

		RegVariable(_T("WT_bEnableSCNCheck"),	&m_bEnableSCNCheck);
		RegVariable(_T("WT_lScnHmiCheckRow"),	&m_lScnHmiRow);
		RegVariable(_T("WT_lScnHmiCheckCol"),	&m_lScnHmiCol);
		RegVariable(_T("WT_bKeepSCNAlignDie"),	&m_bKeepSCNAlignDie);

		RegVariable(_T("WT_bEnableScnCheckAdjust"), &m_bEnableScnCheckAdjust);
		RegVariable(_T("WT_lRefHomeDieRowOffset"), &m_lRefHomeDieRowOffset);
		RegVariable(_T("WT_lRefHomeDieColOffset"), &m_lRefHomeDieColOffset);
		RegVariable(_T("WT_lRefHomeDieRow"), &m_lRefHomeDieRow);
		RegVariable(_T("WT_lRefHomeDieCol"), &m_lRefHomeDieCol);

		RegVariable(_T("WT_bCheckSCNLoaded"),	&m_bCheckSCNLoaded);
		RegVariable(_T("WT_bScnCheckByPR"),		&m_bScnCheckByPR);
		RegVariable(_T("WT_bScnCheckIsRefDie"), &m_bScnCheckIsRefDie);
		RegVariable(_T("WT_lScnCheckRefDieNo"),	&m_lScnCheckRefDieNo);
		RegVariable(_T("WT_lScnCheckXTol"),		&m_lScnCheckXTol);
		RegVariable(_T("WT_lScnCheckYTol"),		&m_lScnCheckYTol);

		RegVariable(_T("WT_ucChangeStartGrade"), &m_ucChangeStartGrade);
		RegVariable(_T("WT_ucChangeEndGrade"), &m_ucChangeEndGrade);
		RegVariable(_T("WT_ucChangeTargetGrade"), &m_ucChangeTargetGrade);
		//4.53D16 
		RegVariable(_T("WT_bEnableMatchMapHeader"), &m_bEnableMatchMapHeader);

		RegVariable(_T("WT_lGradeCount"), &m_lGradeCount);

		RegVariable(_T("WT_bEnableFixMapName"), &m_bEnableFixMapName);
		RegVariable(_T("WT_szFixMapName"), &m_szFixMapName);

		RegVariable(_T("WT_bEnablePrefix"),			&m_bEnablePrefix);
		RegVariable(_T("WT_bEnablePrefixCheck"),	&m_bEnablePrefixCheck);
		RegVariable(_T("WT_bEnableSuffixCheck"),	&m_bEnableSuffixCheck);
		RegVariable(_T("WT_szPrefixName"),			&m_szPrefixName);
		RegVariable(_T("WT_bMatchFileName"),		&m_bMatchFileName);

		RegVariable(_T("WT_bWaferControlPageNo"),	&m_bWaferControlPageNo);
		RegVariable(_T("WT_bCheckSepGrade"),		&m_bCheckSepGrade);
		RegVariable(_T("WT_ucDieShape"),			&m_ucDieShape);

		RegVariable(_T("WT_szNextPath"), &m_szNextPath);
		
		RegVariable(_T("WT_bCheckRepeatMap"),	&m_bCheckRepeatMap);
		RegVariable(_T("WT_szCurrentMapPath"),	&m_szCurrentMapPath);
		RegVariable(_T("WT_bDisableRefDie"),	&m_bDisableRefDie);				//v2.78T1
		RegVariable(_T("WT_bCheckGDOnRefDiePos"),	&m_bCheckGDOnRefDiePos);	//v2.91T1

		RegVariable(_T("WT_ulAlignOption"),		&m_ulAlignOption);	
		RegVariable(_T("WT_lHomeDieMapRow"),	 &m_lHomeDieMapRow);	
		RegVariable(_T("WT_lHomeDieMapCol"), &m_lHomeDieMapCol);	
		RegVariable(_T("WT_lOrgMapRowOffset"), &m_lOrgMapRowOffset);	
		RegVariable(_T("WT_lOrgMapColOffset"), &m_lOrgMapColOffset);	
		RegVariable(_T("WT_lScnMapRowOffset"), &m_lScnMapRowOffset);	
		RegVariable(_T("WT_lScnMapColOffset"), &m_lScnMapColOffset);	

		RegVariable(_T("WT_bFindHomeDieFromMap"),	&m_bFindHomeDieFromMap);	
		RegVariable(_T("WT_ulHomePatternRow"),		&m_ulHomePatternRow);	
		RegVariable(_T("WT_ulHomePatternCol"),		&m_ulHomePatternCol);	
		RegVariable(_T("WT_ulHomePitchRow"),		&m_ulHomePitchRow);	
		RegVariable(_T("WT_ulHomePitchCol"),		&m_ulHomePitchCol);	
		RegVariable(_T("WT_ulHomeKeyDieRow"),		&m_ulHomeKeyDieRow);	
		RegVariable(_T("WT_ulHomeKeyDieCol"),		&m_ulHomeKeyDieCol);	

		RegVariable(_T("WT_lHomeDiePhyPosX"),		&m_lHomeDieWftPosnX);	
		RegVariable(_T("WT_lHomeDiePhyPosY"),		&m_lHomeDieWftPosnY);	
		RegVariable(_T("WL_bSearchHomeOption"),		&m_bSearchHomeOption);	
		RegVariable(_T("WL_ulCornerSearchOption"),	&m_ulCornerSearchOption);	
//		RegVariable(_T("WT_lAngleLHSCount"),		&m_lLHSIndexCount);	
//		RegVariable(_T("WT_lAngleRHSCount"),		&m_lRHSIndexCount);	
		RegVariable(_T("WT_lAlignWaferAngleMode"),	&m_lAlignWaferAngleMode_Std);	
		RegVariable(_T("WT_bNewGTPosition"),		&m_bNewGTPosition);	
		RegVariable(_T("WT_lNewGTPositionX"),		&m_lNewGTPositionX);	
		RegVariable(_T("WT_lNewGTPositionY"),		&m_lNewGTPositionY);	
		RegVariable(_T("WT_lNewGTPositionX2"),		&m_lNewGTPositionX2);	
		RegVariable(_T("WT_lNewGTPositionY2"),		&m_lNewGTPositionY2);
		RegVariable(_T("WT_bNewGTUse2"),				&m_bNewGTUse2);	

		RegVariable(_T("WT_bStopBinLoader"), &m_bStopBinLoader);

		//The CP100Item2
		RegVariable(_T("WT_bIsCP100Item2Enable"),	&m_bIsCP100Item2Enable);	
		RegVariable(_T("WT_bIsCP100Item4Enable"),	&m_bIsCP100Item4Enable);

		//Total SCN check die
		RegVariable(_T("WT_lTotalSCNCount"), &m_lTotalSCNCount);

		RegVariable(_T("WT_bCheckNullBinInMap"), &m_bCheckNullBinInMap);

		//BlockPick(with regular ref die) - ref die count display	//Block2
		RegVariable(_T("WT_lTotalRefDieCount"),		&m_lTotalRefDieCount);	
		RegVariable(_T("WT_lCurrRefDieCount"),		&m_lCurrRefDieCount);

		RegVariable(_T("WT_bSelOfUpdateMapPos"), &m_bSelOfUpdateMapPos);

		// For Wafer Lot File
		RegVariable(_T("WT_bCheckLotWaferName"), &m_bCheckLotWaferName);
		RegVariable(_T("WT_szLotInfoFilePath"), &m_szLotInfoFilePath);	
		RegVariable(_T("WT_szLotInfoFileName"), &m_szLotInfoFileName);	
		RegVariable(_T("WT_szLotInfoFileExt"), &m_szLotInfoFileExt);
		RegVariable(_T("WT_szLotInfoOpID"),		&m_szLotInfoOpID);
		RegVariable(_T("WT_szOutWaferInfoFilePath"), &m_szOutWaferInfoFilePath);	

		//Continue to pick the previous picked grade if new wafer is loaded
		RegVariable(_T("WT_bKeepLastUnloadGrade"), &m_bKeepLastUnloadGrade);

		// Enable Smart Walk in Pick & Place
		RegVariable(_T("WT_bEnableSmartWalkInPicknPlace"),	&m_bEnableSmartWalkInPicknPlace);
		RegVariable(_T("WT_bEnableSWalk1stDieFinder"),		&m_bEnableSWalk1stDieFinder);

		RegVariable(_T("WT_szDummyWaferMapName"),	&m_szDummyWaferMapName);

		// Enable Sorting Check Points
		// Software will wait for operator to confirm when pick die at specific row & col
		RegVariable(_T("WT_bEnableSortingCheckPts"), &m_bEnableSortingCheckPts);
		
		RegVariable(_T("WT_szWaferId"),					&m_szWaferId);
		RegVariable(_T("WT_szUserDefineDieType"),		&m_szUserDefineDieType);
		RegVariable(_T("WT_bEnableMapDieTypeCheck"),	&m_bEnableMapDieTypeCheck);
		RegVariable(_T("WT_bEnablePackageFileCheck"),	&m_bEnablePackageFileCheck);
		RegVariable(_T("WT_bEnableMapHeaderCheck"),		&m_bEnableMapHeaderCheck);
		RegVariable(_T("WT_szMapHeaderCheckWaferId"),	&m_szMapHeaderCheckWaferId);
		
		//Lattice Power Check Lot No.
		RegVariable(_T("WT_bEnableCheckLotNo"),	&m_bEnableCheckLotNo);

		//AutoMapDieTypeCheck (for Ubilux)	//v3.31T1
		RegVariable(_T("WT_bEnableAutoMapDieTypeCheck"), &m_bEnableAutoMapDieTypeCheck);
		RegVariable(_T("WT_bSelectAutoCheckLotID"), &m_bSelectAutoCheckLotID);
		RegVariable(_T("WT_bSelectAutoCheckSerialNum"), &m_bSelectAutoCheckSerialNum);
		RegVariable(_T("WT_bSelectAutoCheckProductNum"), &m_bSelectAutoCheckProductNum);
		RegVariable(_T("WT_bSelectAutoCheckMode"), &m_bSelectAutoCheckMode);

		// MES Connection
		RegVariable(_T("WT_bEnableMESConnection"), &m_bEnableMESConnection);
		RegVariable(_T("WT_szMESIncomingPath"), &m_szMESIncomingPath);
		RegVariable(_T("WT_szMESOutgoingPath"), &m_szMESOutgoingPath);
		RegVariable(_T("WT_lMESTimeout"), &m_lMESTimeout);

		RegVariable(_T("WT_bEnableTwoDimensionsBarcode"),	&m_bEnableTwoDimensionsBarcode);	//v3.33T1
		RegVariable(_T("WT_b2DBarCodeCheckGrade99"),		&m_b2DBarCodeCheckGrade99);			//v3.59
		RegVariable(_T("WT_bOTraceabilityDisableLotID"),	&m_bOTraceabilityDisableLotID);		//v3.68T3

		RegVariable(_T("WT_bBlkFuncPage"),	&m_bBlkFuncPage);	//Block2
		RegVariable(_T("WT_bBlkFuncPage2"), &m_bBlkFuncPage2);	//Block2

		RegVariable(_T("WT_bMapSynMove"),			&m_bMapSyncMove);

		for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
		{
			szText.Format("WT_szaMapDieTypeCheckCurValue%d", i+1);
			RegVariable(_T(szText),	&m_szaMapDieTypeCheckCurValue[i]);

			szText.Format("WT_baIsSelectMapDieTypeCheck%d", i+1);
			RegVariable(_T(szText),	&m_baIsSelectMapDieTypeCheck[i]);
		}

		RegVariable(_T("WT_lSlowProfileTrigger"),	&m_lSlowProfileTrigger);

		RegVariable(_T("WT_lWTNoDiePosX"), &m_lWTNoDiePosX);
		RegVariable(_T("WT_lWTNoDiePosY"), &m_lWTNoDiePosY);

		RegVariable(_T("WT_bUseLF2ndAlignment"),		&m_bUseLF2ndAlignment);
		RegVariable(_T("WT_bUseLF2ndAlignNoMotion"),	&m_bUseLF2ndAlignNoMotion);	//andrewng //2020-05-19
		//andrewng //2020-0902
		RegVariable(_T("WT_lUnloadPhyPosX"),			&m_lUnloadPhyPosX);	
		RegVariable(_T("WT_lUnloadPhyPosY"),			&m_lUnloadPhyPosY);	

		// motiton log
		RegVariable(_T("WT_bEnableWTMotionLog"),	&m_bEnableWTMotionLog);
		RegVariable(_T("WT_bDisableWaferMapGradeSelect"),&m_bDisableWaferMapGradeSelect);
		RegVariable(_T("WT_bUseWaferMapCategory"),	&m_bUseWaferMapCategory);
		RegVariable(_T("WT_lJsWftInUse"),			&m_lJsWftInUse);		//MS100 9Inch dual-table config		//v4.17T1	
		RegVariable(_T("WT_bSamplingScanLog"),		&WM_CMarkDieRegionManager::Instance()->m_bScanRegionMarkDieCaselogEnable);



		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RemoveAllSCNPSN"),			&CWaferTable::RemoveAllSCNPSN);
		// Move command
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("X_MoveToCmd"),				&CWaferTable::X_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("X_MoveCmd"),				&CWaferTable::X_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Y_MoveToCmd"),				&CWaferTable::Y_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Y_MoveCmd"),				&CWaferTable::Y_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("T_MoveToCmd"),				&CWaferTable::T_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("T_MoveCmd"),				&CWaferTable::T_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_MoveToCmd"),				&CWaferTable::XY_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_MoveToUnloadCmd"),		&CWaferTable::XY_MoveToUnloadCmd);		//v4.16T5	//MS100 9Inch
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY2_MoveToUnloadCmd"),		&CWaferTable::XY2_MoveToUnloadCmd);		//v4.24T9	//ES101
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY1_MoveToHomeLoadCmd"),	&CWaferTable::XY1_MoveToHomeLoadCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY2_MoveToHomeLoadCmd"),	&CWaferTable::XY2_MoveToHomeLoadCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_SafeMoveToCmd"),			&CWaferTable::XY_SafeMoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_SafeMoveToUnloadCmd"),	&CWaferTable::XY_SafeMoveToUnloadCmd);	//v4.16T5	//MS100 9Inch
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_MoveCmd"),				&CWaferTable::XY_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XYT_MoveToCmd"),			&CWaferTable::XYT_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XYT_MoveToCmd2"),			&CWaferTable::XYT_MoveToCmd2);			//v2.83T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XYT_MoveAdaptWaferCallBack"),	&CWaferTable::XYT_MoveAdaptWaferCallBack);			//v2.83T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XYT_MoveCmd"),				&CWaferTable::XYT_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("X_HomeCmd"),				&CWaferTable::X_HomeCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Y_HomeCmd"),				&CWaferTable::Y_HomeCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("T_HomeCmd"),				&CWaferTable::T_HomeCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("T2_HomeCmd"),				&CWaferTable::T2_HomeCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_HomeCmd"),				&CWaferTable::XY_HomeCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XYT_HomeCmd"),				&CWaferTable::XYT_HomeCmd);
		// Get encoder values
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AdaptWaferGetEncoderValue"),&CWaferTable::AdaptWaferGetEncoderValue);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetEncoderCmd"),			&CWaferTable::GetEncoderCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetEncoderCmd2"),			&CWaferTable::GetEncoderCmd2);			//v2.83T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetCommanderCmd"),			&CWaferTable::GetCommanderCmd);
		// On or off joystick
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetJoystickCmd"),			&CWaferTable::SetJoystickCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetLockJoystickCmd"),		&CWaferTable::SetLockJoystickCmd);
		// On or off joystick with checking with current state
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UseJoystickCmd"),			&CWaferTable::UseJoystickCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetJoystickSpeedCmd"),		&CWaferTable::SetJoystickSpeedCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetJoystickLimitCmd"),		&CWaferTable::SetJoystickLimitCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToCORCmd"),				&CWaferTable::MoveToCORCmd);
		// Enable Ejector Vacuum
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableEjectorVacuum"),		&CWaferTable::EnableEjectorVacuum);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnSelectWaferLimitCmd"),	&CWaferTable::OnSelectWaferLimitCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetEJTSlideUpCmd"),			&CWaferTable::SetEJTSlideUpCmd);			//v4.16T3
		// Use in process setup		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogItems"),					&CWaferTable::LogItems);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachWaferLimit"),			&CWaferTable::TeachWaferLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowWaferLimit"),			&CWaferTable::ShowWaferLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelWaferLimit"),			&CWaferTable::CancelWaferLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachCalibration"),			&CWaferTable::TeachCalibration);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StopCalibration"),			&CWaferTable::StopCalibration);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachCalibration2"),		&CWaferTable::TeachCalibration2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StopCalibration2"),			&CWaferTable::StopCalibration2);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RotateAngularDie"),			&CWaferTable::RotateAngularDie);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdatePickAndPlacePath"),	&CWaferTable::UpdatePickAndPlacePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdatePickAndPlaceGrade"),	&CWaferTable::UpdatePickAndPlaceGrade);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckPickAndPlaceGrade"),	&CWaferTable::CheckPickAndPlaceGrade);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnablePickAndPlace"),		&CWaferTable::EnablePickAndPlace);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsPickAndPlaceOn"),			&CWaferTable::IsPickAndPlaceOn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdatePickAndPlacePos"),	&CWaferTable::UpdatePickAndPlacePos);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartRotationTest"),		&CWaferTable::StartRotationTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StopRotationTest"),			&CWaferTable::StopRotationTest);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToUnload"),					&CWaferTable::MoveToUnload);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetUnloadPosition"),			&CWaferTable::SetUnloadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmUnloadPosition"),		&CWaferTable::ConfirmUnloadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToUnloadLoad"),				&CWaferTable::MoveToUnloadLoad);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PreStartCheckGlobalTheta"),		&CWaferTable::PreStartCheckGlobalTheta);	//v3.49T1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveWaferTableToSafePosn"),		&CWaferTable::MoveWaferTableToSafePosn);	//v3.61
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveWaferTableToSafePosn2"),	&CWaferTable::MoveWaferTableToSafePosn2);	//v3.61
		
		//Align wafer
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoAlignStandardWafer"),		&CWaferTable::AutoAlignStandardWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoAlignWafer"),				&CWaferTable::AutoAlignWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualAlignWafer"),				&CWaferTable::ManualAlignWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateGThetaMode"),				&CWaferTable::UpdateGThetaMode);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateHomeDiePhyPosition"),		&CWaferTable::UpdateHomeDiePhyPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateSearchHomeOption"),		&CWaferTable::UpdateSearchHomeOption);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateCornerSearchOption"),		&CWaferTable::UpdateCornerSearchOption);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetSoraaModeOption"),			&CWaferTable::SetSoraaModeOption);		//v4.28T5

		//Align wafer (called from other stations)
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AlignInputWafer"),			&CWaferTable::AlignInputWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckRepeatMap"),			&CWaferTable::CheckRepeatMap);			//PLLM v3.74T33

		//For Block Function
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBlkFuncPara"),			&CWaferTable::ResetBlkFuncPara);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckBlkAlignResult"),		&CWaferTable::CheckBlkAlignResult);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RealignFDCDie"),			&CWaferTable::RealignFDCDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GoToBlk1stAutoAlignPos"),	&CWaferTable::GoToBlk1stAutoAlignPos);	//v3.31

		//Generating output file
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WaferMapHeadersPreTask"),	&CWaferTable::WaferMapHeadersPreTask);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadWaferMapColumnsFile"),	&CWaferTable::LoadWaferMapColumnsFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveWaferMapColumnsFile"),	&CWaferTable::SaveWaferMapColumnsFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickNoOfMapColsInput"),	&CWaferTable::OnClickNoOfMapColsInput);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickChangeHeaderPage"),	&CWaferTable::OnClickChangeHeaderPage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OutputFileFormatPreTask"),	&CWaferTable::OutputFileFormatPreTask);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickNoOfOutputColsInput"), &CWaferTable::OnClickNoOfOutputColsInput);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickChangeOutputFilePage"), &CWaferTable::OnClickChangeOutputFilePage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadOutputFileFormatFile"), &CWaferTable::LoadOutputFileFormatFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOutputFileFormatFile"), &CWaferTable::SaveOutputFileFormatFile);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetDummyMapForNichia"),		&CWaferTable::SetDummyMapForNichia);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadMapFileForBurnIn"),			&CWaferTable::LoadMapFileForBurnIn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetTableStartPointForBurnIn"),	&CWaferTable::SetTableStartPointForBurnIn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetMapStartPointForBurnIn"),	&CWaferTable::SetMapStartPointForBurnIn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BurnInAutoAssignGrade"),		&CWaferTable::BurnInAutoAssignGrade);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartPickAndPlace"),			&CWaferTable::StartPickAndPlace);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartBurnIn"),					&CWaferTable::StartBurnIn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WTBurnInSetup"),				&CWaferTable::WTBurnInSetup);
		
		//Search exact file path with a KeyWord
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchFilePath"),				&CWaferTable::SearchFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BrowseFilePath"),				&CWaferTable::BrowseLoadMapFile);

		//Load map with Bar Code Reader & rotate map
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadMapWithBarCode"),			&CWaferTable::LoadMapWithBarCode);

		//Load fix map
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadFixMap"),					&CWaferTable::LoadFixMap);

		//Map file
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMapFilePath"),				&CWaferTable::SaveMapFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMapFileExt"),				&CWaferTable::SaveMapFileExt);

		// Check whether the wafer is aligned
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsWaferAligned"),				&CWaferTable::IsWaferAligned);

		// Get map path via pop-up dialog
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetMapPath"),					&CWaferTable::GetMapPath);

		// Get backup map path via pop-up dialog
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBackupMapPath"),				&CWaferTable::GetBackupMapPath);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetUploadBarcodeIDPath"),		&CWaferTable::GetUploadBarcodeIDPath);

		// get prescan backup map path via pop-op dialog
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetPrescanBackupMapPath"),		&CWaferTable::GetPrescanBackupMapPath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetPrescanSummaryPath"),		&CWaferTable::GetPrescanSummaryPath);

		// Set Map file name from other station
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetMapFileName"),		&CWaferTable::SetMapFileName);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearWaferMap"),		&CWaferTable::ClearWaferMap);

		// Update the Pitch Tolerance
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdatePitchTolerance"), &CWaferTable::UpdatePitchTolerance);

		// Update the data to MSD file
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateData"),			&CWaferTable::UpdateData);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),			&CWaferTable::UpdateAction);

		// Update SCN Check Info
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateCheckSCNInfo"),	&CWaferTable::UpdateCheckSCNInfo);

		// Update Reference cross parameter
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateRefCrossInfo"),	&CWaferTable::UpdateRefCrossInfo);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateGradeDisplayMode"), &CWaferTable::UpdateGradeDisplayMode);

		// Set Wafer table Global angle from other stations
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWaferGlobalAnlge"),	&CWaferTable::SetWaferGlobalAnlge);

		// Update Sub Region Info
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateSubRegionInfo"),	&CWaferTable::UpdateSubRegionInfo);

		// Update Ignore Region Info
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateIgnoreRegionInfo"),	&CWaferTable::UpdateIgnoreRegionInfo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateSelectRegion"),		&CWaferTable::UpdateSelectRegion);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("VerifySelectRegion"),		&CWaferTable::VerifySelectRegion);

		// Enable Map sync move 
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableMapSyncMove"),		&CWaferTable::EnableMapSyncMove);

		//Burn-In Map File Path
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckBurnInMapFilePath"),	&CWaferTable::CheckBurnInMapFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBurnInMapFilePath"),		&CWaferTable::GetBurnInMapFilePath);

		//Map grade color option 
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateMapGradeColor"),		&CWaferTable::UpdateMapGradeColor);

		//Grade mapping funciton
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetGradeMapping"),		&CWaferTable::ResetGradeMapping);
		
		//Set CEID 
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetStartBondEvent"),		&CWaferTable::SetStartBondEvent);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetStopBondEvent"),			&CWaferTable::SetStopBondEvent);

		//Move to current Row/Col @ SCN function
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GoToSCNDiePos"),			&CWaferTable::GoToSCNDiePos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GoToMapPos"),				&CWaferTable::GoToMapPos);				//v3.01T5

		//Set wafertable working angle
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWorkingAngle"),			&CWaferTable::SetWorkingAngle);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmWorkingAngle"),		&CWaferTable::ConfirmWorkingAngle);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualRotateWaferTheta"),	&CWaferTable::ManualRotateWaferTheta);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualRotateWaferTheta2"),	&CWaferTable::ManualRotateWaferTheta2);
		
		//--- Diagnostic Functions ---
		/* PowerOn Functions 
				Parameter: BOOL bOn */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_PowerOn_X"),	&CWaferTable::Diag_PowerOn_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_PowerOn_Y"),	&CWaferTable::Diag_PowerOn_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_PowerOn_T"),	&CWaferTable::Diag_PowerOn_T);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_PowerOn_All"),	&CWaferTable::Diag_PowerOn_All);
		//v4.24T9
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_PowerOn_X2"),	&CWaferTable::Diag_PowerOn_X2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_PowerOn_Y2"),	&CWaferTable::Diag_PowerOn_Y2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_PowerOn_T2"),	&CWaferTable::Diag_PowerOn_T2);
		/* Commutate Functions 
				Parameter: None */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Comm_X"),		&CWaferTable::Diag_Comm_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Comm_Y"),		&CWaferTable::Diag_Comm_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Comm_T"),		&CWaferTable::Diag_Comm_T);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Comm_X2"),		&CWaferTable::Diag_Comm_X2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Comm_Y2"),		&CWaferTable::Diag_Comm_Y2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Comm_T2"),		&CWaferTable::Diag_Comm_T2);

		/* Home Functions 
				Parameter: None */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Home_X"),		&CWaferTable::Diag_Home_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Home_Y"),		&CWaferTable::Diag_Home_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Home_T"),		&CWaferTable::Diag_Home_T);
		//v4.24T9
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Home_X2"),		&CWaferTable::Diag_Home_X2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Home_Y2"),		&CWaferTable::Diag_Home_Y2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Home_T2"),		&CWaferTable::Diag_Home_T2);
		/* Move Functions (Relative)
				Parameter: LONG lPos (Motor Counts) */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Move_X"),		&CWaferTable::Diag_Move_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Move_Y"),		&CWaferTable::Diag_Move_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Move_T"),		&CWaferTable::Diag_Move_T);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Move_X2"),		&CWaferTable::Diag_Move_X2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Move_Y2"),		&CWaferTable::Diag_Move_Y2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_Diag_Move_T2"),		&CWaferTable::Diag_Move_T2);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_MoveWToffset"),		&CWaferTable::MoveWToffset);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestXMLFunction"),		&CWaferTable::TestXMLFunction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenerateWaferDiagram"), &CWaferTable::GenerateWaferDiagram);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeBoundaryDieColour"), &CWaferTable::ChangeBoundaryDieColour);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeNGDieColour"),	&CWaferTable::ChangeNGDieColour);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeGridLineColour"), &CWaferTable::ChangeGridLineColour);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateOutput"),			&CWaferTable::UpdateOutput);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EndLotClearMap"),		&CWaferTable::EndLotClearMap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetDisableRefDie"),		&CWaferTable::SetDisableRefDie);		//v2.78T1

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateWaferMapPath"),	&CWaferTable::UpdateWaferMapPath);	
		//------- Command For Wafer Lot File --------//
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetOutWaferInfoFilePath"), &CWaferTable::GetOutWaferInfoFilePath);
		// Get the Wafer Lot File Path
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetWaferLotInfoFilePath"), &CWaferTable::GetWaferLotInfoFilePath);
		// Read the Wafer Lot File and store it into msd file
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReadWaferLotInfoFile"),			&CWaferTable::ReadWaferLotInfoFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReadWaferLotInfoFile_SECSGEM"), &CWaferTable::ReadWaferLotInfoFile_SECSGEM);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableCheckWaferName"),			&CWaferTable::EnableCheckWaferName);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowWaferLotDataCmd"),			&CWaferTable::ShowWaferLotDataCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearWaferLotDataCmd"),			&CWaferTable::ClearWaferLotDataCmd);
		// Checking for the manual mode
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckWaferLotDataInManualMode"), &CWaferTable::CheckWaferLotDataInManualMode);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckAllWaferLoadedCmd"),		&CWaferTable::CheckAllWaferLoadedCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateWaferLotLoadStatusCmd"),	&CWaferTable::UpdateWaferLotLoadStatusCmd);
		
		
		// Checking for the auto mode
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckWaferLotDataInAutoMode"),	&CWaferTable::CheckWaferLotDataInAutoMode);

		//------- Command For Moving Wafer Table For Searching the Barcode ------//
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReadBarCodeOnTable"),			&CWaferTable::ReadBarCodeOnTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReadBarCodeOnTable2"),			&CWaferTable::ReadBarCodeOnTable2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("X_SyncCmd"),					&CWaferTable::X_SyncCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Y_SyncCmd"),					&CWaferTable::Y_SyncCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("T_SyncCmd"),					&CWaferTable::T_SyncCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_SyncCmd"),					&CWaferTable::XY_SyncCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DirectCornerSearchDie"),		&CWaferTable::DirectCornerSearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateDirectCornerSearchPos"),	&CWaferTable::UpdateDirectCornerSearchPos);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsUpperLeftCornerComplete"),	&CWaferTable::IsUpperLeftCornerComplete);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearMapDisplayFilename"),		&CWaferTable::ClearMapDisplayFilename);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateMapFileName"),			&CWaferTable::UpdateMapFileName);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateNewEjrSeqOption"),		&CWaferTable::UpdateNewEjrSeqOption);	//v2.96T4
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestFcn"),						&CWaferTable::TestFcn);								//v3.04
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateSortingCheckPointsOption"), &CWaferTable::UpdateSortingCheckPointsOption);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadWaferMapHeader"),			&CWaferTable::LoadWaferMapHeader);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadOutputFileFormat"),			&CWaferTable::LoadOutputFileFormat);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WaferMapHeaderCheckingCmd"),	&CWaferTable::WaferMapHeaderCheckingCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckLoadCurrentMapCmd"),		&CWaferTable::CheckLoadCurrentMapCmd);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadWaferMapErrorCheckingCmd"),	&CWaferTable::LoadWaferMapErrorCheckingCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND("EnableMESConnection",				&CWaferTable::EnableMESConnection);
		m_comServer.IPC_REG_SERVICE_COMMAND("UpdateMapDieTypeCheck",			&CWaferTable::UpdateMapDieTypeCheck);
		m_comServer.IPC_REG_SERVICE_COMMAND("LoadDieTypeFromFile",				&CWaferTable::LoadDieTypeFromFile);
		m_comServer.IPC_REG_SERVICE_COMMAND("ResetDieType",						&CWaferTable::ResetDieType);
		m_comServer.IPC_REG_SERVICE_COMMAND("CheckDieType",						&CWaferTable::CheckDieType);
		m_comServer.IPC_REG_SERVICE_COMMAND("CheckInputWaferIDHeader",			&CWaferTable::CheckInputWaferIDHeader);
		m_comServer.IPC_REG_SERVICE_COMMAND("AutoCheckCOR",						&CWaferTable::AutoCheckCOR);

		m_comServer.IPC_REG_SERVICE_COMMAND("UpdateAutoMapDieTypeCheck",		&CWaferTable::UpdateAutoMapDieTypeCheck);	//v3.31T1

		// MES Connection Command
		m_comServer.IPC_REG_SERVICE_COMMAND("GetMESOutgoingPath",			&CWaferTable::GetMESOutgoingPath);
		m_comServer.IPC_REG_SERVICE_COMMAND("GetMESIncomingPath",			&CWaferTable::GetMESIncomingPath);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetMESTimeout",				&CWaferTable::SetMESTimeout);

		m_comServer.IPC_REG_SERVICE_COMMAND("AutoRotateWafer",				&CWaferTable::AutoRotateWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND("AutoRotateWaferForBC",			&CWaferTable::AutoRotateWaferForBC);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartWTXMotionTest"),		&CWaferTable::StartWTXMotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartWTYMotionTest"),		&CWaferTable::StartWTYMotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartWT2XMotionTest"),		&CWaferTable::StartWT2XMotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartWT2YMotionTest"),		&CWaferTable::StartWT2YMotionTest);

// prescan relative code
		RegPrescanVarFunc();
// Temperature Controller
		//TC_RegisterVariables();

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrAdaptWafer"),				&CWaferTable::PrAdaptWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Test"),						&CWaferTable::Test);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SwitchBlockFuncPage"),		&CWaferTable::SwitchBlockFuncPage);	//Block2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CalculateCOROffset"),		&CWaferTable::CalculateCOROffset);				//v3.44
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RemoveLotInfoFile"),		&CWaferTable::RemoveLotInfoFile);				//v3.44
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetAlignStatus"),			&CWaferTable::ResetAlignStatus);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoAlignStdWafer"),		&CWaferTable::AutoAlignStdWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FindWaferAngle"),			&CWaferTable::FindWaferAngle);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetHomeDiePosition"),		&CWaferTable::SetWftHomeDiePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmHomeDiePosition"),	&CWaferTable::ConfirmWftHomeDiePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsAutoAlignElCornerWafer"), &CWaferTable::IsAutoAlignElCornerWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoRegionAlignStdWafer"),	&CWaferTable::AutoRegionAlignStdWafer);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoRegionAlignBlock"),		&CWaferTable::AutoRegionAlignBlock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoAlign2PhasesSortSecondPart"),	&CWaferTable::AutoAlign2PhasesSortSecondPart);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MS90SortTo2ndHalf"),		&CWaferTable::MS90SortTo2ndHalf);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MS90Set2ndHomePosition"),	&CWaferTable::MS90Set2ndHomePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_SetNewGTPosition"),		&CWaferTable::WT_SetNewGTPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_ConfirmNewGTPosition"),	&CWaferTable::WT_ConfirmNewGTPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_GetNewGTPosition"),		&CWaferTable::WT_GetNewGTPosition);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PkgKeyParametersTask"),		&CWaferTable::PkgKeyParametersTask);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExportRefCheckFailFile"),	&CWaferTable::ExportRefCheckFailFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GeneratePkgDataFile"),		&CWaferTable::GeneratePkgDataFile);		//v4.22T1	//Walsin China
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenerateConfigData"),		&CWaferTable::GenerateConfigData);		//v4.24T5

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWaferClampOnOff"),		&CWaferTable::SetWaferClampOnOff);		//v4.26T1	//Walsin China MS810EL-90
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsExpanderClosed"),			&CWaferTable::IsExpanderClosed);		//v4.26T1	//Walsin China MS810EL-90
		//v4.48A26	//Avago
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FaceValueExistsInMap"),			&CWaferTable::FaceValueExistsInMap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeMapColorToOneColor"),		&CWaferTable::ChangeMapColorToOneColor);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckClearBinResetSuffixName"),	&CWaferTable::CheckClearBinResetSuffixName);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ScanCheckDieOffset"),		&CWaferTable::ScanCheckDieOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FOVCheckDieOffset"),		&CWaferTable::FOVCheckDieOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WftLoadBinFile"),			&CWaferTable::WftLoadBinFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_WTMoveDiff"),			&CWaferTable::AGC_WTMoveDiff);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WTChangeNoDiePosition"),	&CWaferTable::WTChangeNoDiePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WTConfirmNoDiePositionSetup"),	&CWaferTable::WTConfirmNoDiePositionSetup);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveSCNData"),			&CWaferTable::SaveSCNData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckSamplingRescanIgnoreGradeList"),	&CWaferTable::CheckSamplingRescanIgnoreGradeList);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ToggleScanRegionMarkDieCaseLog"),	&CWaferTable::ToggleScanRegionMarkDieCaseLog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WTMoveFromTempFile"),	&CWaferTable::WTMoveFromTempFile);		//andrewng //2020-0630
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveTableToDummyPos"),	&CWaferTable::MoveTableToDummyPos);		//andrewng //2020-0727

		RegisterVariables2();

		RegisterVariables_EjPinClean();

		DisplayMessage("WaferTableStn commands registered ...");
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

typedef struct {
	LONG lX;
	LONG lY;
	LONG lT;
} WT_XYT_STRUCT;

typedef struct {
	LONG lX;
	LONG lY;
	DOUBLE dT;
} WT_XYT_STRUCT2;

typedef struct {
	LONG lX;
	LONG lY;
} WT_XY_STRUCT;

typedef struct {
	LONG lX;
	LONG lY;
	LONG lUnload;
} WT_XY_UNLOAD_STRUCT;

/* --- Move XYT --- */
// Move XYT (Absolute)
LONG CWaferTable::XYT_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	WT_XYT_STRUCT stPos, stEnc;
	svMsg.GetMsg(sizeof(WT_XYT_STRUCT), &stPos);

	if( IsWithinWaferLimit(stPos.lX, stPos.lY) )
	{
		XYT_MoveTo(stPos.lX, stPos.lY, stPos.lT, SFM_WAIT);	// Move table
	}

	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX1();
	stEnc.lY = GetCurrY1();
	stEnc.lT = GetCurrT1();
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}


LONG CWaferTable::XYT_MoveToCmd2(IPC_CServiceMessage& svMsg)
{
	WT_XYT_STRUCT2 stPos, stEnc;
	svMsg.GetMsg(sizeof(WT_XYT_STRUCT2), &stPos);

	SetJoystickOn(FALSE);

	LONG lX			= stPos.lX;
	LONG lY			= stPos.lY;
	DOUBLE dDegree	= stPos.dT;		//relative angle
	LONG lTheta		= 0;

	if (dDegree != 0)
	{
		RotateWaferTheta(&lX, &lY, &lTheta, dDegree);
	}
	
	//Move Table & Theta
	XY_MoveTo(lX, lY, SFM_WAIT);
	if (dDegree != 0)
	{
		LONG lOldX=0,lOldY=0,lOldT=0;
		GetEncoder(&lOldX, &lOldY, &lOldT);
		T_MoveTo(lOldT + lTheta, SFM_WAIT);
	}
	Sleep(10);
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX();
	stEnc.lY = GetCurrY();
	stEnc.dT = GetCurrT();

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT2), &stEnc);
	return 1;
}

LONG CWaferTable::XYT_MoveAdaptWaferCallBack(IPC_CServiceMessage& svMsg)
{
	WT_XYT_STRUCT2 stPos, stEnc;
	svMsg.GetMsg(sizeof(WT_XYT_STRUCT2), &stPos);

	SetJoystickOn(FALSE);
	
	LONG lX			= stPos.lX;
	LONG lY			= stPos.lY;
	DOUBLE dDegree	= stPos.dT;		//relative angle
	LONG lTheta		= 0;

	if (m_bUseDualTablesOption == TRUE)
	{
		if (IsWT1UnderCamera())
		{
			if (dDegree != 0)
			{
				RotateWaferThetaAngle(&lX, &lY, &lTheta, dDegree, FALSE);
			}
			
			//Move Table & Theta
			if( IsWithinWT1WaferLimit(lX, lY) )
				XY1_MoveTo(lX, lY, SFM_WAIT);
			
			if (dDegree != 0)
			{
				LONG lOldT=0;
				GetEncoderValue();
				lOldT = GetCurrT1();
				T1_MoveTo(lOldT + lTheta, SFM_WAIT);
			}

			Sleep(10);
			GetEncoderValue();		// Get the encoder value from hardware

			stEnc.lX = GetCurrX1();
			stEnc.lY = GetCurrY1();
			stEnc.dT = GetCurrT1();
		}
		else if (IsWT2UnderCamera() )
		{
			if (dDegree != 0)
			{
				RotateWaferThetaAngle(&lX, &lY, &lTheta, dDegree, TRUE);
			}
			
			//Move Table & Theta
			if( IsWithinWT2WaferLimit(lX, lY) )
				XY2_MoveTo(lX, lY, SFM_WAIT);
			
			if (dDegree != 0)
			{
				LONG lOldT=0;
				GetEncoderValue();
				lOldT = GetCurrT2();
				T2_MoveTo(lOldT + lTheta, SFM_WAIT);
			}

			Sleep(10);
			GetEncoderValue();		// Get the encoder value from hardware

			stEnc.lX = GetCurrX2();
			stEnc.lY = GetCurrY2();
			stEnc.dT = GetCurrT2();
		}
	}
	else
	{
		if (dDegree != 0)
		{
			RotateWaferTheta(&lX, &lY, &lTheta, dDegree);
		}
		
		//Move Table & Theta
		XY_MoveTo(lX, lY, SFM_WAIT);
		
		if (dDegree != 0)
		{
			LONG lOldT=0;
			GetEncoderValue();
			lOldT = GetCurrT();
			T_MoveTo(lOldT + lTheta, SFM_WAIT);
		}

		Sleep(10);
		GetEncoderValue();		// Get the encoder value from hardware

		stEnc.lX = GetCurrX();
		stEnc.lY = GetCurrY();
		stEnc.dT = GetCurrT();
	}

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT2), &stEnc);
	return 1;

}


// Move XYT (Relative)
LONG CWaferTable::XYT_MoveCmd(IPC_CServiceMessage& svMsg)
{
	WT_XYT_STRUCT stPos, stEnc;
	svMsg.GetMsg(sizeof(WT_XYT_STRUCT), &stPos);

	X1_Sync();
	Y1_Sync();
	GetEncoderValue();

	if( IsWithinWaferLimit(GetCurrX1()+stPos.lX, GetCurrY1()+stPos.lY) )
	{
		XYT_Move(stPos.lX, stPos.lY, stPos.lT, SFM_WAIT);	// Move table
	}

	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX1();
	stEnc.lY = GetCurrY1();
	stEnc.lT = GetCurrT1();
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

/* --- Move XY --- */
// Move XY (Absolute)	// for normal table only
LONG CWaferTable::XY_MoveToCmd(IPC_CServiceMessage& svMsg)	// no wafer limit check
{
	WT_XY_STRUCT stPos;
	BOOL bResult = TRUE;

	svMsg.GetMsg(sizeof(WT_XY_STRUCT), &stPos);
	
	//v3.71T1
	if (XY_IsPowerOff())
	{
		HmiMessage("Wafer Table XY not power ON!");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}


	INT nResult = 0;
#ifdef NU_MOTION
	//v3.70T1
	nResult = X1_Profile(LOW_PROF);
	nResult = Y1_Profile(LOW_PROF);
	if (nResult != gnOK)
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
#else
	if (CMS896AApp::m_lCycleSpeedMode > 0)	//v3.71T1	//For MS899/810 HD version only
	{
		nResult = X1_Profile(LOW_PROF);
		nResult = Y1_Profile(LOW_PROF);
		if (nResult != gnOK)
		{
			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}
	}
#endif

	if (!m_bSel_X || !m_bSel_Y)			//v4.30T2
		bResult = FALSE;		
	else if (XY1_MoveTo(stPos.lX, stPos.lY, SFM_WAIT) == gnOK)	// Move table
		bResult = TRUE;											// Get the encoder value from hardware
	else
		bResult = FALSE;				//v3.60

	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);

	//bResult = TRUE;					//v4.30T2		//expander may crash with gripper if XY1_MoveTo() fails
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}


LONG CWaferTable::XY_MoveToUnloadCmd(IPC_CServiceMessage& svMsg)	// no wafer limit check for MS100 9Inch
{
	WT_XY_UNLOAD_STRUCT stPos;
	BOOL bResult = TRUE;
	CString szLog;

	svMsg.GetMsg(sizeof(WT_XY_UNLOAD_STRUCT), &stPos);
	
	if (XY_IsPowerOff())
	{
		HmiMessage("Wafer Table XY not power ON!");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	SetJoystickOn(FALSE);				//v4.23T2

	INT nResult = 0;
#ifdef NU_MOTION
	nResult = X1_Profile(LOW_PROF1);		//v4.20
	nResult = Y1_Profile(LOW_PROF1);		//v4.20
	if (nResult != gnOK)
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
#else
	if (CMS896AApp::m_lCycleSpeedMode > 0)		//For MS899/810 HD version only
	{
		nResult = X1_Profile(LOW_PROF);
		nResult = Y1_Profile(LOW_PROF);
		if (nResult != gnOK)
		{
			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}
	}
#endif


	if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.16T3	//MS100 9Inch
	{
		if (stPos.lUnload > 0)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT: XY MoveToUnloadCmd to 0,0");

			if (XY1_MoveTo(0, 0, SFM_WAIT) == gnOK)	
			{
				SetEJTSlideUpDn(FALSE);
				Sleep(500);

				//v4.42T6
				BOOL bAtUnload = FALSE;
				for(int i=0; i<30; i++)
				{
                    if (!IsEJTAtUnloadPosn())
					{
						Sleep(1000);
					}
					else
					{
						bAtUnload = TRUE;
						break;
					}
				}
				if (bAtUnload==FALSE)
				{
					//SetAlert_Red_Yellow(IDS_WT_BL_NOT_IN_SAFE_POS);
					HmiMessage_Red_Back("Ejector isnt in a safe position");
					SetErrorMessage("SetEJTSlideUpCmd: EJT not at UNLOAD position (MS109)");
					X1_Profile(NORMAL_PROF);
					Y1_Profile(NORMAL_PROF);	
					bResult = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bResult);
					return 1;
				}

				szLog.Format("WT: XY MoveToUnloadCmd to UNLOAD at %ld, %ld", stPos.lX, stPos.lY);
				CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

				if (XY1_MoveTo(stPos.lX, stPos.lY, SFM_WAIT) == gnOK)
					bResult = TRUE;					
				else
					bResult = FALSE;
			}
			else
			{
				bResult = FALSE;	//v4.24
			}
		}
		else
		{
			if (!CheckCEMarkCoverSensors())		//v4.50A24
			{
				if (XY1_MoveTo(stPos.lX, stPos.lY, SFM_WAIT) == gnOK)
				{
					SetEJTSlideUpDn(TRUE);
					Sleep(100);
				}
				else
				{
					bResult = FALSE;	//v4.24
				}
			}
			else
			{
				bResult = FALSE;	//v4.24
			}
		}
	}
	else if (m_bUseDualTablesOption)	//v4.24T9	//ES101
	{
		LONG lX = stPos.lX;
		LONG lY = stPos.lY;
		if ( (lX < m_lXNegLimit) || (lX > m_lXPosLimit) )
		{
			CString szErr;
			szErr.Format("WT: X exceeds WT1 limit in XY1 MoveTo UnloadCmd: %ld (%ld, %ld)", lX, m_lXNegLimit, m_lXPosLimit);
			SetErrorMessage(szErr);
			AfxMessageBox(szErr, MB_SYSTEMMODAL);
		}

		if ( (lY < m_lYNegLimit) || (lY > m_lYPosLimit) )
		{
			CString szErr;
			szErr.Format("Y exceeds WT1 limit in XY1 MoveTo UnloadCmd: %ld (%ld, %ld)", lY, m_lYNegLimit, m_lYPosLimit);
			SetErrorMessage(szErr);
			AfxMessageBox(szErr, MB_SYSTEMMODAL);
		}

		if (stPos.lUnload > 0)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT: XY MoveToUnloadCmd to UNLOAD (ES101)");
			if (XY1_MoveTo(stPos.lX, stPos.lY, SFM_WAIT) == gnOK)
				bResult = TRUE;					
			else
				bResult = FALSE;
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT: XY MoveToUnloadCmd to LOAD (ES101)");
			HomeTable2(FALSE);
			if (XY1_MoveTo(stPos.lX, stPos.lY, SFM_WAIT) == gnOK)
			{
				bResult = TRUE;					
				Sleep(100);
			}
			else
			{
				bResult = FALSE;
			}
		}
	}
	else
	{
		if (XY1_MoveTo(stPos.lX, stPos.lY, SFM_WAIT) == gnOK)	// Move table
			bResult = TRUE;									// Get the encoder value from hardware
		else
			bResult = FALSE;
	}


	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);

	szLog.Format("WT: XY MoveToUnloadCmd done; code=%d", bResult);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

	//bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

// 4.24TX
LONG CWaferTable::XY2_MoveToUnloadCmd(IPC_CServiceMessage& svMsg)	// no wafer limit check for ES101
{
	WT_XY_UNLOAD_STRUCT stPos;
	BOOL bResult = TRUE;
	INT nResult = 0;
	CString szLog;

	svMsg.GetMsg(sizeof(WT_XY_UNLOAD_STRUCT), &stPos);
	

	if( IsESDualWT()==FALSE )
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (XY_IsPowerOff())
	{
		HmiMessage("Wafer Table XY not power ON!");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!X2_IsPowerOn() || !Y2_IsPowerOn())
	{
		HmiMessage("Wafer Table XY 2 not power ON!");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	LONG lX = stPos.lX;
	LONG lY = stPos.lY;
	if ( (lX < m_lX2NegLimit) || (lX > m_lX2PosLimit) )
	{
CString szErr;
szErr.Format("WT: X2 exceeds WT2 limit in XY2 MoveTo UnloadCmd: %ld (%ld, %ld)", lX, m_lX2NegLimit, m_lX2PosLimit);
SetErrorMessage(szErr);
AfxMessageBox(szErr, MB_SYSTEMMODAL);
	}

	if ( (lY < m_lY2NegLimit) || (lY > m_lY2PosLimit) )
	{
CString szErr;
szErr.Format("Y2 exceeds WT2 limit in XY2 MoveTo UnloadCmd: %ld (%ld, %ld)", lY, m_lY2NegLimit, m_lY2PosLimit);
SetErrorMessage(szErr);
AfxMessageBox(szErr, MB_SYSTEMMODAL);
	}


	SetJoystickOn(FALSE);	
	nResult = X2_Profile(LOW_PROF1);
	nResult = Y2_Profile(LOW_PROF1);


	if (stPos.lUnload > 0)
	{
		//No need to HOME-Table 1
		CMSLogFileUtility::Instance()->WL_LogStatus("WT: XY2 MoveToUnloadCmd to UNLOAD (ES101)");
		if (XY2_MoveTo(stPos.lX, stPos.lY, SFM_WAIT) == gnOK)
			bResult = TRUE;					
		else
			bResult = FALSE;
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WT: XY2 MoveToUnloadCmd to LOAD (ES101)");

		HomeTable1(FALSE);
		if (XY2_MoveTo(stPos.lX, stPos.lY, SFM_WAIT) == gnOK)
		{
			bResult = TRUE;					
			Sleep(100);
		}
		else
		{
			bResult = FALSE;
		}
	}

	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);

	szLog.Format("WT: XY2 MoveToUnloadCmd done; code=%d", bResult);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

	if (XY_IsPowerOff())
	{
		HmiMessage("Wafer Table XY not power ON!");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}


// Move XY (Relative)
LONG CWaferTable::XY_MoveCmd(IPC_CServiceMessage& svMsg)
{
	WT_XY_STRUCT stPos;
	WT_XYT_STRUCT stEnc;
	svMsg.GetMsg(sizeof(WT_XY_STRUCT), &stPos);

	X1_Sync();
	Y1_Sync();
	GetEncoderValue();		// Get the encoder value from hardware

	if( IsWithinWaferLimit(GetCurrX1()+stPos.lX, GetCurrY1()+stPos.lY) )
	{
		XY_Move(stPos.lX, stPos.lY, SFM_WAIT, FALSE);	// Move table
	}
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX1();
	stEnc.lY = GetCurrY1();
	stEnc.lT = GetCurrT1();
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

/* --- Move Single Axis --- */
// Move X (Absolute)
LONG CWaferTable::X_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	WT_XYT_STRUCT stEnc;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	X1_MoveTo(lPos, SFM_WAIT);	// Move table
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX1();
	stEnc.lY = GetCurrY1();
	stEnc.lT = GetCurrT1();
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

// Move X (Relative)
LONG CWaferTable::X_MoveCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	WT_XYT_STRUCT stEnc;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	X1_Move(lPos, SFM_WAIT);	// Move table
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX1();
	stEnc.lY = GetCurrY1();
	stEnc.lT = GetCurrT1();
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

// Move Y (Absolute)
LONG CWaferTable::Y_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	WT_XYT_STRUCT stEnc;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	Y1_MoveTo(lPos, SFM_WAIT);	// Move table
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX1();
	stEnc.lY = GetCurrY1();
	stEnc.lT = GetCurrT1();
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

// Move Y (Relative)
LONG CWaferTable::Y_MoveCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	WT_XYT_STRUCT stEnc;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	Y1_Move(lPos, SFM_WAIT);	// Move table
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX1();
	stEnc.lY = GetCurrY1();
	stEnc.lT = GetCurrT1();
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

// Move T (Absolute)
LONG CWaferTable::T_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	INT nResult = T1_MoveTo(lPos, SFM_WAIT);	// Move table

	svMsg.InitMessage(sizeof(INT), &nResult);

	return 1;
}

// Move T (Relative)
LONG CWaferTable::T_MoveCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	WT_XYT_STRUCT stEnc;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	T1_Move(lPos, SFM_WAIT);	// Move table
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX1();
	stEnc.lY = GetCurrY1();
	stEnc.lT = GetCurrT1();
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

/* --- Home ---*/
LONG CWaferTable::X_HomeCmd(IPC_CServiceMessage& svMsg)
{
	if (MoveBackLightToSafeLevel() == FALSE)
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	X_Home();
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Y_HomeCmd(IPC_CServiceMessage& svMsg)
{
	if (MoveBackLightToSafeLevel() == FALSE)
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	Y_Home();
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::T_HomeCmd(IPC_CServiceMessage& svMsg)
{
	T_Home();
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::T2_HomeCmd(IPC_CServiceMessage& svMsg)
{
	T2_Home();
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::XY_HomeCmd(IPC_CServiceMessage& svMsg)
{
	if( IsBLInUse() )
	{
		if (MoveBackLightToSafeLevel() == FALSE)
		{
			BOOL bReturn=TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	if( IsEjtElvtInUse() )
	{
		if (MoveEjectorElevatorToSafeLevel() == FALSE)
		{
			BOOL bReturn=TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	XY_Home();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::XYT_HomeCmd(IPC_CServiceMessage& svMsg)
{
	if (MoveBackLightToSafeLevel() == FALSE)
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	XYT_Home();
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::AdaptWaferGetEncoderValue(IPC_CServiceMessage& svMsg)
{
	typedef struct {
		LONG lX;
		LONG lY;
		LONG lT;
		DOUBLE dT;
	} WT_XYT_STRUCT2;

	WT_XYT_STRUCT2 stEnc;

	GetEncoderValue();		// Get the encoder value from hardware

	if (m_bUseDualTablesOption == TRUE)
	{
		if (IsWT1UnderCamera())
		{
			stEnc.lX = GetCurrX1();
			stEnc.lY = GetCurrY1();
			stEnc.lT = GetCurrT1();
			
			DOUBLE dEncT = GetCurrT1();
			stEnc.dT = dEncT * m_dThetaRes / -m_lThetaMotorDirection;
		}
		else if (IsWT2UnderCamera() )
		{
			stEnc.lX = GetCurrX2();
			stEnc.lY = GetCurrY2();
			stEnc.lT = GetCurrT2();
			
			DOUBLE dEncT = GetCurrT2();
			stEnc.dT = dEncT * m_dThetaRes / -m_lThetaMotorDirection;
		}
	}
	else
	{
		stEnc.lX = GetCurrX();
		stEnc.lY = GetCurrY();
		stEnc.lT = GetCurrT();
		DOUBLE dEncT = GetCurrT();
		stEnc.dT = dEncT * m_dThetaRes / -m_lThetaMotorDirection;
	}

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT2), &stEnc);
	return 1;
}

LONG CWaferTable::GetEncoderCmd(IPC_CServiceMessage& svMsg)
{
	WT_XYT_STRUCT stEnc;
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX();
	stEnc.lY = GetCurrY();
	stEnc.lT = GetCurrT();

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}


LONG CWaferTable::GetCommanderCmd(IPC_CServiceMessage& svMsg)
{
	WT_XYT_STRUCT stCmd;
	GetCmdValue();		// Get the encoder value from hardware

	stCmd.lX = m_lCmd_X;
	stCmd.lY = m_lCmd_Y;
	stCmd.lT = 0;

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stCmd);

	return 1;
}


LONG CWaferTable::GetEncoderCmd2(IPC_CServiceMessage& svMsg)
{
	typedef struct {
		LONG lX;
		LONG lY;
		LONG lT;
		DOUBLE dT;
	} WT_XYT_STRUCT2;

	WT_XYT_STRUCT2 stEnc;
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = GetCurrX2();
	stEnc.lY = GetCurrY2();
	stEnc.lT = GetCurrT2();
	
	DOUBLE dEncT = GetCurrT2();
	stEnc.dT = dEncT * m_dThetaRes / -m_lThetaMotorDirection;

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT2), &stEnc);
	return 1;
}

LONG CWaferTable::SetLockJoystickCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bLock = FALSE;
	BOOL bOK = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bLock);
	
	SetLockJoystick(bLock);

	svMsg.InitMessage(sizeof(BOOL), &bOK);
	return 1;
}


// On or off the joystick
LONG CWaferTable::SetJoystickCmd(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn, bOK=TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);
	SetJoystickOn(bOn);	// ES101_XU NEED PASS IN SOMETIMES
	svMsg.InitMessage(sizeof(BOOL), &bOK);

	return 1;
}

// On or off the joystick with checking with current state
LONG CWaferTable::UseJoystickCmd(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn, bOK=TRUE, bTemp;
	svMsg.GetMsg(sizeof(BOOL), &bOn);
	if ( bOn == TRUE )
	{
		SetJoystickOn(m_bJoystickOn);
		MoveBinTableToSafePosn(TRUE);	//v3.70T1
	}
	else
	{
		bTemp = m_bJoystickOn;
		SetJoystickOn(FALSE);
		m_bJoystickOn = bTemp;		// Restore the original joystick state
	}

	svMsg.InitMessage(sizeof(BOOL), &bOK);

	return 1;
}

// Set joystick speed
LONG CWaferTable::SetJoystickSpeedCmd(IPC_CServiceMessage& svMsg)
{
	LONG	lLevel=0;
	LONG	lX_Min, lX_Max, lY_Min, lY_Max;
	BOOL	bOK=TRUE;

	svMsg.GetMsg(sizeof(LONG), &lLevel);

	//if ( m_pServo_X != NULL && m_pServo_Y != NULL )
	if (m_fHardware && !m_bDisableWT)	//v3.61
	{
		try 
		{
			SetJoystickOn(FALSE);

			lX_Min = GetCircleWaferMinX();
			lX_Max = GetCircleWaferMaxX();

			lY_Min = GetCircleWaferMinY();
			lY_Max = GetCircleWaferMaxY();

			if (GetWaferDiameter() <= 0)
			{
				lX_Min = WT_JOY_MAXLIMIT_NEG_X;
				lX_Max = WT_JOY_MAXLIMIT_POS_X;

				lY_Min = WT_JOY_MAXLIMIT_NEG_Y;
				lY_Max = WT_JOY_MAXLIMIT_POS_Y;
			}

			m_lJoystickLevel = lLevel;
			if( IsWT2InUse() )
			{
				CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X2, lX_Min, lX_Max, &m_stWTAxis_X2);
				CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y2, lY_Min, lY_Max, &m_stWTAxis_Y2);
			}
			else
			{
				CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, lX_Min, lX_Max, &m_stWTAxis_X);
				CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, lY_Min, lY_Max, &m_stWTAxis_Y);
			}

			CString szSpeed = "WT_JoyNormal";
			switch(lLevel)
			{
				case 0:		// Slow speed
					m_lJoystickLevel = 0;	//v3.76
					szSpeed = "WT_JoySlow";
					break;

				case 2:		// Fast speed
					m_lJoystickLevel = 2;	//v3.76
					szSpeed = "WT_JoyFast";
					break;

				case 1:		// Normal speed
				default:
					m_lJoystickLevel = 1;	//v3.76
					szSpeed = "WT_JoyNormal";
					break;
			}
			if( IsWT2InUse() )
			{
				CMS896AStn::MotionSelectIncrementParam(WT_AXIS_X2, szSpeed, &m_stWTAxis_X2);
				CMS896AStn::MotionSelectIncrementParam(WT_AXIS_Y2, szSpeed, &m_stWTAxis_Y2); 
			}
			else
			{
				CMS896AStn::MotionSelectIncrementParam(WT_AXIS_X, szSpeed, &m_stWTAxis_X);
				CMS896AStn::MotionSelectIncrementParam(WT_AXIS_Y, szSpeed, &m_stWTAxis_Y); 
			}
			if (m_bXJoystickOn)
				X_SetJoystickOn(TRUE);
			else
				X_SetJoystickOn(FALSE);

			if (m_bYJoystickOn)
				Y_SetJoystickOn(TRUE);
			else
				Y_SetJoystickOn(FALSE);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			if( IsWT2InUse() )
			{
				CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
				CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			}
			else
			{
				CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
				CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			}
			bOK = FALSE;
		}	
	}
	svMsg.InitMessage(sizeof(BOOL), &bOK);

	return 1;
}


// Set max joystick limit
LONG CWaferTable::SetJoystickLimitCmd(IPC_CServiceMessage &svMsg)
{
	BOOL	bMax, bOK=TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bMax);

	try
	{
		if (bMax == TRUE)
		{
			//Enlarge joystick limit when teach wafer limit
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, WT_JOY_MAXLIMIT_NEG_X, WT_JOY_MAXLIMIT_POS_X, &m_stWTAxis_X);		
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, WT_JOY_MAXLIMIT_NEG_Y, WT_JOY_MAXLIMIT_POS_Y, &m_stWTAxis_Y);
		}
		else
		{
			//Restore original limit 
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);		
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
		bOK = FALSE;
	}	

	svMsg.InitMessage(sizeof(BOOL), &bOK);

	return 1;
}

LONG CWaferTable::MoveToCORCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bMove = TRUE;

	bMove = XY_SafeMoveTo(m_lWaferCalibX, m_lWaferCalibY);

	if( bMove==FALSE )
	{
		CString szContent;
		szContent.LoadString(HMB_WT_OUT_WAFER_LIMIT);
		HmiMessage(szContent);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// Enable Ejector Vacuum
LONG CWaferTable::EnableEjectorVacuum(IPC_CServiceMessage &svMsg)
{
	BOOL bSet;
	svMsg.GetMsg(sizeof(BOOL), &bSet);
	SetEjectorVacuum(bSet);
	return 1;
}

LONG CWaferTable::OnSelectWaferLimitCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	SetJoystickOn(FALSE);

	if (CMS896AStn::m_bEnableWaferSizeSelect == TRUE)
	{
		CString szLogMsg;
		szLogMsg.Format("Do you want to change wafer limit to size%d?", m_ucWaferLimitTemplateNoSel+1);
		if( HmiMessage(szLogMsg, "Wafer Table", glHMI_MBX_YESNO)==glHMI_YES )
		{
			OnSelectWaferLimit(m_ucWaferLimitTemplateNoSel);
			SaveData();
			szLogMsg.Format("Select Wafer Limit size%d, now center (%ld,%ld), diameter %.2f", 
				m_ucWaferLimitTemplateNoSel+1, m_lWaferCenterX, m_lWaferCenterY, m_dWaferDiameter);
			SetErrorMessage(szLogMsg);	//	425TX 1
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::TeachWaferLimit(IPC_CServiceMessage &svMsg)
{
	LONG lTeachStep;
	BOOL bReturn = TRUE;

	try
	{
		svMsg.GetMsg(sizeof(LONG), &lTeachStep);

		//v4.52A6
		if (IsMS90())
		{
			HmiMessage_Red_Yellow("Please input diameter value directly for MS90.", 
				"Teach Wafer Limit");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (MoveBackLightToSafeLevel()== FALSE)
		{
			return 1;	
		}

		if (MoveEjectorElevatorToSafeLevel() == FALSE)
		{
			return 1;
		}

		if (lTeachStep == 0)
		{
			HomeTable2();
		}

		if ( m_ucWaferLimitType == WT_CIRCLE_LIMIT )
		{
			SetWaferLimit(lTeachStep);
		}
		else
		{
			if ( lTeachStep == 0 )
			{
				SetPolygonWaferLimit();
			}
		}

		if (lTeachStep >= 4)
		{
			SetStatusMessage("Wafer limit is set");

			// 3421
			SetGemValue("WT_WaferCenterX", GetWft1CenterX());
			SetGemValue("WT_WaferCenterY", GetWft1CenterY());
			// 3422
			SetGemValue("WT_WaferSize", GetWaferDiameter());
			// 7500
			SendEvent(SG_CEID_WT_WAFERSIZE, FALSE);
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
	}	

	return 1;
}


LONG CWaferTable::CancelWaferLimit(IPC_CServiceMessage &svMsg)
{
	//Restore wafer limit is user cancel the learning	
	try
	{
		if (m_fHardware && !m_bDisableWT)	//v3.61
		{
			SetJoystickOn(FALSE);
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);		
			if ( (m_ulJoyStickMode == MS899_JS_MODE_PR) )	//v3.76
				m_bCheckWaferLimit = TRUE;	
			SetJoystickOn(TRUE);
			m_bXJoystickOn = TRUE;
			m_bYJoystickOn = TRUE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
	}	
	return 1;
}


LONG CWaferTable::ShowWaferLimit(IPC_CServiceMessage &svMsg)
{
//	4.24TX maybe need to fix to wafer table 1
#ifdef NU_MOTION
	X1_Profile(LOW_PROF);
	Y1_Profile(LOW_PROF);
#endif
	
	SetJoystickOn(FALSE);

	HomeTable2();

	// Move table to center pos
	XY1_MoveTo(GetWft1CenterX(), GetWft1CenterY(), SFM_WAIT);

	if ( m_ucWaferLimitType == WT_CIRCLE_LIMIT )
	{
		MoveWaferLimit();
	}
	else
	{
		MovePolygonWaferLimit();
	}
	
	SetJoystickOn(TRUE);

#ifdef NU_MOTION
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
#endif

	m_bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &m_bReply);

	return 1;
}


LONG CWaferTable::LearnCOR()
{
	long	Cur_ThetaPosn = 0, nErr = 0;
	PR_RCOORD wpr_offset = {0, 0};
	double wpr_rot = 0;
	short	res = 0;

	m_bStartCalibrate = FALSE;
	SetJoystickOn(FALSE);

	if (nErr = XY_MoveTo(m_lWaferCalibX, m_lWaferCalibY, SFM_WAIT)) 
	{
		m_lWaferCalibX = 0;
		m_lWaferCalibY = 0;
		return nErr;
	}
/*
	if (DispQuestion("Learn COR with PRS? (Y/N)") == glHMI_YES)
	{
		SetJoystickOn(TRUE);
		DispMsg("Move to Die Center 1, Press [Close] to Confirm");
		SetJoystickOn(FALSE);
		WaferCam();
		res = SearchForDie(&wpr_offset, &wpr_rot, TRUE, FALSE);
		while (res != PR_ERR_GOOD_DIE)
		{
			WaferCam();
			DispMsgbox("Not Good Die");
			SetJoystickOn(TRUE);
			if (DispQuestion("Move to Die Center 1 Again, Press [Yes] to Confirm") != glHMI_YES)
			{
				SetJoystickOn(FALSE);
				return 1;
			}
			SetJoystickOn(FALSE);
			res = SearchForDie(&wpr_offset, &wpr_rot, TRUE, FALSE);
			Sleep(1);
		}
		AdjustTableOffset(wpr_offset, wpr_rot);
		WaferCam();
		wt_get_posn(&pos1, &Cur_ThetaPosn);
		pos2.x_pos = 2 * m_lWaferCalibX - pos1.x_pos;
		pos2.y_pos = 2 * m_lWaferCalibY - pos1.y_pos;
		//Cur_ThetaPosn += _round(180.0 / WT_T_RES);
		Cur_ThetaPosn += _round(180.0 / m_dThetaRes);
		if (nErr = wt_move_xyt(pos2.x_pos, pos2.y_pos, Cur_ThetaPosn)) 
		{
			return nErr;
		}
		SetJoystickOn(TRUE);
		DispMsg("Move to Die Center 2, Press [Close] to Confirm");
		SetJoystickOn(FALSE);
		res = SearchForDie(&wpr_offset, &wpr_rot, TRUE, FALSE);
		while (res != PR_ERR_GOOD_DIE)
		{
			WaferCam();
			DispMsg("Not Good Die");
			SetJoystickOn(ON);
			if (DispQuestion("Move to Die Center 2 Again, Press [Yes] to Confirm") != glHMI_YES)
			{
				SetJoystickOn(FALSE);
				//Restore
				wt_get_posn(&pos1, &Cur_ThetaPosn);
				//Cur_ThetaPosn -= _round(180.0 / WT_T_RES);
				Cur_ThetaPosn -= _round(180.0 / m_dThetaRes);
				if (nErr = wt_move_xyt(WT_NOT_MOVE, WT_NOT_MOVE, Cur_ThetaPosn)) 
				{
					return nErr;
				}
				return 1;
			}
			res = SearchForDie(&wpr_offset, &wpr_rot, TRUE, FALSE);
			Sleep(1);
		}
		AdjustTableOffset(wpr_offset, 0);
		WaferCam();
		wt_get_posn(&pos2, &Cur_ThetaPosn);
		m_lWaferCalibX = (pos1.x_pos + pos2.x_pos) / 2;
		m_lWaferCalibY = (pos1.y_pos + pos2.y_pos) / 2;
		DispMsDispMsggbox("COR Position Learned");
	}
	else
*/
	{
		//On joystick
		SetJoystickOn(TRUE);
//		EnableMouseClickCallbackCmd(TRUE);
		DispMsg("Move to Point 1, Press [Close] to Confirm");
		SetJoystickOn(FALSE);
		//WaferCam();
		LONG lPos1_X = 0, lPos1_Y = 0;
		LONG lPos2_X = 0, lPos2_Y = 0;
		GetEncoderValue();
		lPos1_X = GetCurrX();
		lPos1_Y = GetCurrY();

		lPos2_X = 2 * m_lWaferCalibX - lPos1_X;
		lPos2_Y = 2 * m_lWaferCalibY - lPos1_Y;
		SetJoystickOn(FALSE);
		//Cur_ThetaPosn += _round(180.0 / WT_T_RES);
		Cur_ThetaPosn += _round(180.0 / m_dThetaRes);
//		if (Cur_ThetaPosn > _round(360.0 / WT_T_RES))
//			Cur_ThetaPosn -= _round(360.0 / WT_T_RES);
		T1_MoveTo(Cur_ThetaPosn, SFM_WAIT);
		if (nErr = XY_MoveTo(lPos2_X, lPos2_Y, SFM_WAIT)) 
		{
			return nErr;
		}

		SetJoystickOn(TRUE);
		DispMsg("Move to Point 2, Press [Close] to Confirm");
		SetJoystickOn(FALSE);
		GetEncoderValue();
		lPos2_X = GetCurrX();
		lPos2_Y = GetCurrY();
		m_lWaferCalibX = (lPos1_X + lPos2_X) / 2;
		m_lWaferCalibY = (lPos1_Y + lPos2_Y) / 2;
		DispMsg("COR Position Learned");
	}
	if (IsMS90())	//v4.50A7
	{
		CMS896AStn::m_lWaferCenterX	= m_lWaferCalibX;
		CMS896AStn::m_lWaferCenterY = m_lWaferCalibY;
	}

	//Off joystick
	SetJoystickOn(FALSE);
	SaveWaferTblData();
	GetEncoderValue();
	Cur_ThetaPosn = GetCurrT();

	Cur_ThetaPosn -= _round(180.0 / m_dThetaRes);
	if (nErr = T_MoveTo(Cur_ThetaPosn, SFM_WAIT)) 
	{
		return nErr;
	}
	return 0;
}


LONG CWaferTable::TeachCalibration(IPC_CServiceMessage &svMsg)
{
	if (MoveBackLightToSafeLevel() == FALSE)
	{
		return 1;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		return 1;
	}

	if (!CheckWExpanderLock())		//v3.48
	{
		return 0;
	}

	if (0)
	{
		LearnCOR();
	}
	else
	{
		m_lWaferBeforeCalibT = 0;
		GetEncoderValue();
		Sleep(100);
		m_lWaferBeforeCalibT = GetCurrT1();

		m_lRotateCount = (LONG)(180 / m_dThetaRes);

		StartWaferCalibration();
	}
	return 1;
}


LONG CWaferTable::StopCalibration(IPC_CServiceMessage &svMsg)
{
	LONG	lTeachStep;

	svMsg.GetMsg(sizeof(LONG), &lTeachStep);

	StopWaferCalibration(lTeachStep);

	SetStatusMessage("Wafer calibration is learnt");

	T1_MoveTo(m_lWaferBeforeCalibT, SFM_WAIT);

    // 3426
    SetGemValue("WT_WaferTCaliX", m_lWaferCalibX);
    SetGemValue("WT_WaferTCaliY", m_lWaferCalibY);
    // 7502
    SendEvent(SG_CEID_WT_CALIBRATE, FALSE);

	return 1;
}

LONG CWaferTable::TeachCalibration2(IPC_CServiceMessage &svMsg)
{
	if (MoveBackLightToSafeLevel() == FALSE)
	{
		return 1;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		return 1;
	}

	m_lWaferBeforeCalibT2 = 0;
	GetEncoderValue();
	Sleep(100);
	m_lWaferBeforeCalibT2 = GetCurrT2();

	m_lRotateCount = (LONG)(8 / m_dThetaRes);

	if (!CheckWExpander2Lock())		//v3.48
	{
		return 0;
	}

	StartWaferCalibration2();
	return 1;
}


LONG CWaferTable::StopCalibration2(IPC_CServiceMessage &svMsg)
{
	LONG	lTeachStep;

	svMsg.GetMsg(sizeof(LONG), &lTeachStep);

	StopWaferCalibration2(lTeachStep);

	SetStatusMessage("Wafer table 2 calibration is learnt");

	T2_MoveTo(m_lWaferBeforeCalibT2, SFM_WAIT);

	try
	{
		// 3426
		SetGemValue("WT_WaferTCaliX2", m_lWaferCalibX2);
		SetGemValue("WT_WaferTCaliY2", m_lWaferCalibY2);
		// 7502
		SendEvent(SG_CEID_WT_CALIBRATE, FALSE);
	}
	catch(...)
	{
		DisplaySequence("SG_CEID_WT_CALIBRATE failure");
	}

	return 1;
}


LONG CWaferTable::StartRotationTest(IPC_CServiceMessage &svMsg)
{
	SetJoystickOn(TRUE);		//On joystick
	T1_MoveTo(0,SFM_WAIT);

	m_bStartCalibrate = TRUE;

	return 1;
}

LONG CWaferTable::StopRotationTest(IPC_CServiceMessage &svMsg)
{
	LONG	lTeachStep;

	svMsg.GetMsg(sizeof(LONG), &lTeachStep);
	StopWaferCalibration(lTeachStep);

	return 1;
}

LONG CWaferTable::RotateAngularDie(IPC_CServiceMessage &svMsg)
{
	typedef struct {
		LONG	lX;
		LONG	lY;
		LONG	lT;
		DOUBLE	dAngle;
	} ABSPOS;

	ABSPOS	stPos;
	LONG	lX;
	LONG	lY; 
	LONG	lTheta;
	DOUBLE	dDegree;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(ABSPOS), &stPos);

	lX		= stPos.lX;
	lY		= stPos.lY;
	lTheta	= stPos.lT;
	dDegree	= stPos.dAngle;

	RotateWaferTheta(&lX, &lY, &lTheta, dDegree);

	//***************************//
	//v3.44
	LONG lDX=0, lDY=0;
	if (m_bUseMultiCorOffset)
	{
		DOUBLE dDX, dDY;
		CString szTemp;
		//Move Table & Theta
		GetEncoderValue();

		//v3.44
		LONG lCurrX = GetCurrX();
		LONG lCurrY = GetCurrY();
		LONG iSrcT  = GetCurrT();


		if (lTheta > 0)
		{
			dDX = m_dCorOffsetA1 * (lCurrY - m_lWaferCalibY) + m_dCorOffsetB1;
			dDY = m_dCorOffsetA2 * (lCurrX - m_lWaferCalibX) + m_dCorOffsetB2;
			lDX = (LONG) dDX;
			lDY = (LONG) dDY;
		}
		else
		{
			dDX = m_dACorOffsetA1 * (lCurrY - m_lWaferCalibY) + m_dACorOffsetB1;
			dDY = m_dACorOffsetA2 * (lCurrX - m_lWaferCalibX) + m_dACorOffsetB2;
			lDX = (LONG) dDX;
			lDY = (LONG) dDY;
		}
	}
	lX = lX+lDX;
	lY = lY+lDY;
	LONG lT = 0;
	if (lTheta != 0)
	{
		LONG lOldX=0,lOldY=0,lOldT=0;
		GetEncoder(&lOldX, &lOldY, &lOldT);
		lT = lOldT + lTheta;
	}
	//***************************//

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	CString szMsg;
	szMsg.Format("WFT MV_T %d,%d by %f to %d,%d,T%d", stPos.lX, stPos.lY, dDegree, lX, lY, lTheta);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);

	//v2.97T3
	XY_MoveTo(lX, lY, SFM_NOWAIT);
	if (lTheta != 0)
	{
		T_MoveTo(lT, SFM_NOWAIT);
	}
	X_Sync();
	Y_Sync();
	if (lTheta != 0)
	{
		T_Sync();

		//v3.61		//Apply T dealy to ConfirmSearch die also	//andrew
		LONG lTDelay = (*m_psmfSRam)["WaferTable"]["TDelay"];
		if ((lTDelay > 0) && (lTDelay < 10000))
			Sleep(lTDelay);

		//Tell BH task that curr cycle with theta correction CONFIRM-SEARCH action, so extra delay is needed		
		(*m_psmfSRam)["WaferTable"]["Rotate"] = TRUE;		//v3.62		
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	//v3.48
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::UpdatePickAndPlacePath(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG lWaferEmptyLimit;
		LONG lWaferStreetLimit;
		LONG lWalkPath;
	} LIMIT;

	LIMIT stPos;

	svMsg.GetMsg(sizeof(LIMIT), &stPos);

	m_lPickAndPlaceWalkPath = stPos.lWalkPath;
	m_lWaferEmptyLimit		= stPos.lWaferEmptyLimit;
	m_lWaferStreetLimit		= stPos.lWaferStreetLimit;

	SaveData();		//these attributes now put to LastState file	//v4.11T1

	SetWaferIndexingPathParameters();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
} 

BOOL CWaferTable::CheckDummyBinGradeHasNoDie(UCHAR ucGrade)
{
	IPC_CServiceMessage stMsg;
	int		nConvID = 0;
	BOOL	bReturn = FALSE;
	stMsg.InitMessage(sizeof(UCHAR), &ucGrade);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIfBlockHasDie", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
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

	return bReturn;
}

LONG CWaferTable::UpdatePickAndPlaceGrade(IPC_CServiceMessage &svMsg)
{
	UCHAR ucGrade;
	CString szMsg;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(UCHAR), &ucGrade);

	m_ucDummyPrescanPNPGrade = ucGrade;

	bReturn = CheckDummyBinGradeHasNoDie(m_ucDummyPrescanPNPGrade);
	if( bReturn )
	{
		szMsg.Format("Pick and Place grade %d has die on bin frame!", m_ucDummyPrescanPNPGrade);
		HmiMessage(szMsg);
		m_bEnablePickAndPlace = FALSE;	// original enable, grade has die
		LoadWaferMapAlgorithmSettings();
	}
	else
	{
		szMsg.Format("Pick and Place grade %d !", m_ucDummyPrescanPNPGrade);
	}
	SetErrorMessage(szMsg);

	SetWaferIndexingPathParameters();

	SaveData();
	bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
} 

LONG CWaferTable::CheckPickAndPlaceGrade(IPC_CServiceMessage &svMsg)
{
	UCHAR ucGrade;
	CString szMsg;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(UCHAR), &ucGrade);

	if( CheckDummyBinGradeHasNoDie(ucGrade) && IsEnablePNP() )
	{
		szMsg.Format("Pick and Place grade %d has die on bin frame, please clear it before change!", ucGrade);
		HmiMessage(szMsg);
		bReturn=FALSE;
		SetErrorMessage(szMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
} 	//	425TX1


LONG CWaferTable::EnablePickAndPlace(IPC_CServiceMessage &svMsg)
{
	BOOL bEnable;
	BOOL bEnableOptimizeBinCount = FALSE;
	CString szTitle = "", szContent = "";
	BOOL bReturn=TRUE;
	CString szMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	svMsg.GetMsg(sizeof(BOOL), &bEnable);


	//v4.41T6	//SanAn
	BOOL bSanAn = (pApp->GetCustomerName() == CTM_SANAN);
	if (bSanAn)
	{
		if (bEnable)
		{
			m_bDiePitchCheck = FALSE;
		}
		else
		{
			m_bDiePitchCheck = TRUE;
		}
		//SaveData();
	}


	if (!bEnable)
	{
		m_bEnablePickAndPlace = FALSE;	// previous is enable, now to disable it.
		bReturn = CheckDummyBinGradeHasNoDie(m_ucDummyPrescanPNPGrade);
		if( bReturn )
		{
			szMsg.Format("Pick and Place grade %d has die on bin frame!\nDo you want to disable pick and place?", m_ucDummyPrescanPNPGrade);
			if( HmiMessage(szMsg, "Pick and Place", glHMI_MBX_YESNO)!=glHMI_YES )
			{
				m_bEnablePickAndPlace = TRUE;	// previous is enable, now to disable it.
			}
		}
		LoadWaferMapAlgorithmSettings();
		SetAlignmentStatus(FALSE);
		m_WaferMapWrapper.InitMap();
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	bEnableOptimizeBinCount = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["EnableOptimizeBinCount"]; 
	if( bEnableOptimizeBinCount )
	{
		m_bEnablePickAndPlace = FALSE;	// optimize bin counter, can not be enable
		szTitle.LoadString(HMB_WT_PICK_N_PLACE);
		szContent.LoadString(HMB_WT_EN_PNP_FAILED_OPT_EN);
		HmiMessage(szContent, szTitle);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsPrescanEnable() )
	{
		if( pApp->GetCustomerName()=="Cree" )
		{
			HmiMessage("Please disable prescan firstly!", "Pick and Place");
			m_bEnablePickAndPlace = FALSE;	// cree, previous disable, prescan on, auto disable
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else
		{
			HmiMessage("Prescan auto become disable", "Pick and Place");
		}
	}

	bReturn = CheckDummyBinGradeHasNoDie(m_ucDummyPrescanPNPGrade);
	if( bReturn )
	{
		m_bEnablePickAndPlace = FALSE;	//	425TX 1
		szMsg.Format("Pick and Place grade %d has die on bin frame!", m_ucDummyPrescanPNPGrade);
		HmiMessage(szMsg);
	}
	else
	{
		m_bEnablePickAndPlace = TRUE;	//	425TX 1
		szMsg.Format("Pick and Place grade %d !", m_ucDummyPrescanPNPGrade);
	}
	SetErrorMessage(szMsg);

	SetWaferIndexingPathParameters();
	//Update wafermap format = Pick & Place
	SetWaferMapFormat("PickAndPlace");

	//v4.20
	//Need to chagne back to any valid mapping algorithm for mapsort mode
	bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
} 

LONG CWaferTable::UpdatePickAndPlacePos(IPC_CServiceMessage &svMsg)
{
	m_stStopTime = CTime::GetCurrentTime();	// pick and place set start position

	ClearPrescanRawData();	// when press Set Start Position (Pick and Place mode), clear data again.
	//Set parameters
	SetWaferIndexingPathParameters();

	SetAlignmentStatus(TRUE);
	Sleep(50);
	GetEncoderValue();
	SetGlobalTheta();				//v3.66

	m_bScnLoaded	= FALSE;
	m_lStart_X		= GetCurrX();
	m_lStart_Y		= GetCurrY();
	m_lStart_T		= GetCurrT();		//v3.66

	m_stLastDie.lX = 0;
	m_stLastDie.lY = 0;
	m_stLastDie.ucGrade = 48 + m_ucDummyPrescanPNPGrade;
	m_stLastDie.lX_Enc = GetCurrX();
	m_stLastDie.lY_Enc = GetCurrY();
	m_stLastDie.lT_Enc = GetCurrT();
	m_stCurrentDie = m_stLastDie;

	if (m_bEnableSmartWalkInPicknPlace == FALSE)
	{
		m_WaferMapWrapper.SetAlgorithmParameter("StartX", m_lStart_X);
		m_WaferMapWrapper.SetAlgorithmParameter("StartY", m_lStart_Y);
		SetMapPhyPosn(0, 0, m_lStart_X, m_lStart_Y);
		m_WaferMapWrapper.SetStartPosition(0, 0);

		SetEnableSmartWalk(FALSE);		//v4.23T3	//SmartWalk bug
	}
	else
	{
		LONG lWaferUpperLeftX, lWaferUpperLeftY, lStartRow, lStartCol, lHoriDiePitchX, lVertDiePitchY;
		LONG lHoriDiePitchY, lVertDiePitchX;

		lWaferUpperLeftX = GetCircleWaferMaxX();
		lWaferUpperLeftY = GetCircleWaferMaxY();

		lHoriDiePitchX	= GetDiePitchX_X();
		lHoriDiePitchY	= GetDiePitchX_Y();

		lVertDiePitchX	= GetDiePitchY_X();
		lVertDiePitchY	= GetDiePitchY_Y();
	
		lStartCol = labs(m_lStart_X - lWaferUpperLeftX)/(lHoriDiePitchX + lVertDiePitchX);
		lStartRow = labs(m_lStart_Y - lWaferUpperLeftY)/(lVertDiePitchY + lHoriDiePitchY);

		m_WaferMapWrapper.SetAlgorithmParameter("StartX", m_lStart_X);
		m_WaferMapWrapper.SetAlgorithmParameter("StartY", m_lStart_Y);
		SetMapPhyPosn(lStartRow, lStartCol, m_lStart_X, m_lStart_Y);
		m_WaferMapWrapper.SetStartPosition(lStartRow, lStartCol);

		//v4.23T3	//SmartWalk bug
		SetEnableSmartWalk(TRUE);
		//SmartWalk can only support MIXED mode
		if (m_WaferMapWrapper.GetPickMode() != WAF_CDieSelectionAlgorithm::MIXED_ORDER)		//If not MIX mode
		{
			m_WaferMapWrapper.SetPickMode(WAF_CDieSelectionAlgorithm::MIXED_ORDER);
			SaveWaferMapAlgorithmSettings();
		}
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 


LONG CWaferTable::IsPickAndPlaceOn(IPC_CServiceMessage &svMsg)
{
	LONG lReturn = MS_PICK_N_PLACE_DISABLED;

	// manual mode
	if ((IsEnablePNP() == TRUE) )
	{
		if( IsWLManualMode() )
			lReturn = MS_PICK_N_PLACE_MANUAL_MODE;
		else if( IsWLAutoMode() )
			lReturn =	MS_PICK_N_PLACE_AUTO_MODE;
	}
	
	svMsg.InitMessage(sizeof(BOOL), &lReturn);
	return 1;
}

LONG CWaferTable::ManualAlignWafer(IPC_CServiceMessage &svMsg)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();

	if ( CMS896AApp::m_bMapLoadingFinish == FALSE && pUtl->GetPrescanDummyMap() == FALSE )
	{
		BOOL bReturn = FALSE;
		HmiMessage_Red_Back("Map loading not complete, please wait!", "Manual Align Wafer");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	CString szAlgorithm;
	CString szPathFinder;
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
	if( szAlgorithm.Find("Sorting Path 1") != -1 && IsPrescanEnable() && IsAlignedWafer() )
	{
		m_lBuildShortPathInScanning = 0;
		m_pWaferMapManager->SuspendAlgorithmPreparation();
	}

	//WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	BOOL bReturn = TRUE;
	bReturn = ManualAlignWaferMap(FALSE);

	ScnAlignLog("ManualAlign Wafer: finish");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
/*
====================================================================================================================================
m_bAlignedWafer  -- if wafer are not aligned, set m_bAlignedWafer to FALSE.
m_bRegionAligned -- if the current region of wafer is not aligned, set m_bRegionAligned to FALSE.
m_bManualAlignRegion -- if user press "Manual Align" button, set m_bManualAlignRegion to TRUE
after picked the currrent region, and check whether the next region has reference die,
if there has not reference die in the next region and set m_bManualAlignRegion to FALSE,
and set m_bAlignedWafer to FASLE, ask user to press "Manual Align" button
m_bManualRegionScanSort -- if user press "Manual Align" button and picked all die in the currrent region, it will
auto set both m_bManualAlignRegion and m_bAlignedWafer to FALSE, ask user to press "Manual Align" button
====================================================================================================================================
*/
/// <summary>
/// 
/// </summary>
/// <param name="bSecondTime">		</param>
/// <returns>
/// 
/// </returns>
BOOL CWaferTable::ManualAlignWaferMap(BOOL bSecondTime)
{
	CString szTitle, szContent;
	INT nCol = -1;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	CMSLogFileUtility::Instance()->MS_LogOperation("ManualAlign Wafer: Start"); /******/	//v4.08
	ScnAlignLog("ManualAlign Wafer: Start");
	m_dStartWaferAlignTime = GetTime();	// manual align

	szTitle.LoadString(HMB_WT_ALIGN_WAFER);

	if ( pUtl->GetPrescanDummyMap() == FALSE )	// create dummy map for pick and place
	{
		if (m_WaferMapWrapper.IsMapValid() != TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT ERR: Map is not valid");
			SetErrorMessage("Map is not valid");
			SetAlert_Red_Yellow(IDS_WL_MAP_NOT_VALID);
			return FALSE;
		}
	}

	//Check SCN file is loaded or not
	if (CheckSCNFileLoaded() == FALSE)
	{
		return FALSE;
	}

	// prescan relative code
	if (CheckLoadCurrentMapStatus() != TRUE)
	{
		return FALSE;
	}

	SetAlignmentStatus(FALSE);


	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	/*if (pWaferPr != NULL)
	{
		if (pWaferPr->IsWaferRotated180() && !IsMS90Sorting2ndPart()||
			!pWaferPr->IsWaferRotated180() && IsMS90Sorting2ndPart())
		{
			SetErrorMessage("Orientation not match when manual align!");
			HmiMessage("Orientation not match!!");
			return FALSE;
		}
	}*/

	//v3.30T2	
	//Manual-Align fcn not available for BLock-pick mode
	CString szAlgorithm;
	CString szPathFinder;
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
	
	CString szLog;
	szLog.Format("ManualAlign Wafer: ALGOR=%s, PATH=%s BLKPICK=%d", szAlgorithm, szPathFinder, IsBlkFuncEnable());
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


	typedef struct {
		BOOL	bStatus;
		double	dOffsetX;
		double 	dOffsetY;
		DOUBLE	dAngle;
	} SRCH_RESULT;
	INT			nConvID=0;
	LONG		lCurrX=0, lCurrY=0;
	LONG		lTheta = 0;
	BOOL		bOriginal=FALSE;
	ULONG		ulAlignRow=0, ulAlignCol=0;
	CString		szMsg;
	SRCH_RESULT	stResult;

	m_WaferMapWrapper.GetSelectedPosition(ulAlignRow, ulAlignCol);
	m_WaferMapWrapper.SetCurrentPosition(ulAlignRow, ulAlignCol);

	if (m_bManualAlignReferDie && IsScanAlignWafer() == FALSE)
	{
		if( m_WaferMapWrapper.IsReferenceDie(ulAlignRow, ulAlignCol) == FALSE )
		{
			szTitle.LoadString(HMB_WT_ALIGN_DIE_FAILED);
			LONG lHmiRow = 0, lHmiCol = 0;
			ConvertAsmToHmiUser(ulAlignRow, ulAlignCol, lHmiRow, lHmiCol);
			szMsg.Format("You select die (%d,%d) on map is NOT a REFER die", lHmiRow, lHmiCol);
			HmiMessage(szMsg, szTitle);
			return FALSE;
		}
	}


	stResult.dOffsetX	= 0;
	stResult.dOffsetY	= 0;
	stResult.dAngle		= 0;
	stResult.bStatus	= TRUE;		//Klocwork	//v4.46	

	//Search the reference die which user pick the die
	if (!IsBurnIn())
	{
		IPC_CServiceMessage svMsg;

		if (IsScanAlignWafer())
		{
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchDie_NoMove", svMsg);
		}
		else if (m_bManualAlignReferDie)
		{
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchRefDie_NoMove", svMsg);
		}
		else
		{
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchDie_NoMove", svMsg);
		}

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
	}

	if (stResult.bStatus == FALSE)
	{
		szTitle.LoadString(HMB_WT_ALIGN_DIE_FAILED);
		HmiMessage(szTitle, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 300, 150, NULL, NULL, NULL, NULL);
		SetErrorMessage("Manual Align Wafer PR fails.");
		return FALSE;
	}


	GetEncoderValue();
	lCurrX = GetCurrX() + _round(stResult.dOffsetX);	
	lCurrY = GetCurrY() + _round(stResult.dOffsetY);

	if (m_bJoystickOn == TRUE) 
	{
		SetJoystickOn(FALSE);
		bOriginal = TRUE;
	}

	// Move the table for die compensation
	XY_SafeMoveTo(lCurrX, lCurrY);
	Sleep(50);
	
	//Check Die Orientation
	if (pWaferPr != NULL)
	{
		LONG nErr = pWaferPr->SearchAOTDie();
		if (nErr > 0)
		{
			SetErrorMessage("WT: ManualAlignWafer SearchAOTDie fails.");
			return FALSE;
		}
	}

	if (IsScanAlignWafer())
	{
		BOOL bReferOnNormal = TRUE;
		IPC_CServiceMessage stMsg;
		int	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "CounterCheckReferOnNormalDie", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);		
			}
		}

		stMsg.GetMsg(sizeof(BOOL), &bReferOnNormal);
		if (bReferOnNormal == FALSE)
		{
			szContent = "Normal die can be recognized as refer die.\nPlease re-learn refer die again!";
			szTitle.LoadString(HMB_WT_ALIGN_DIE_FAILED);
			HmiMessage(szContent, szTitle);
			SetErrorMessage("Manual Align Wafer fails because normal die recognized as refer.");

			return FALSE;
		}
	}
	
	GetEncoderValue();
	m_lStart_X		= GetCurrX();
	m_lStart_Y		= GetCurrY();
	m_lStart_T		= GetCurrT();
	LONG lAlignWftX = m_lStart_X;
	LONG lAlignWftY = m_lStart_Y;
	LONG lAlignWftT = m_lStart_T;


	if( CreateDummyMapForPrescanPicknPlace() == FALSE )	// manual align
	{
		SetErrorMessage("ManualAlignWafer: CreateDummyMapForPrescanPicknPlace fail");
		return FALSE;
	}

	if( GetPSCMode()!=PSC_NONE && GetPSCMode()!=PSC_REGN )	// get the home die, it is first die;
	{
		m_WaferMapWrapper.GetFirstDiePosition(ulAlignRow, ulAlignCol);
		m_WaferMapWrapper.SetSelectedPosition(ulAlignRow, ulAlignCol);
		szMsg.Format("Prober Region sample check first die (%d, %d)", ulAlignRow, ulAlignCol);
		//SetErrorMessage(szMsg);
		Sleep(500);
	}

	// search die and get the OCR value
	m_WaferMapWrapper.GetSelectedPosition(ulAlignRow, ulAlignCol);
	m_WaferMapWrapper.SetCurrentPosition(ulAlignRow, ulAlignCol);

	//if (m_bFullRefBlock == FALSE || pUtl->GetPrescanRegionMode() )	//Block2
	//{
	SetupSubRegionMode();		// manual align button
	//}

	// Clear All Physical Position if Realign
	// prescan relative code	//  manual align wafer

	m_ulAlignRow = ulAlignRow;
	m_ulAlignCol = ulAlignCol;

	SetStatusMessage("Manual align wafer is completed");

	pSRInfo->SetManualAlignRegion(TRUE);
	m_b2PartsSortAutoAlignWay = FALSE;

	if (pUtl->GetPrescanRegionMode() && IsPrescanEnable())
	{
		ULONG ulRegionNo = pSRInfo->GetWithinRegionNo(ulAlignRow, ulAlignCol);

		if (pSRInfo->IsInvalidRegion(ulRegionNo))
		{
			//v0.08
			szLog.Format("ManualAlignWafer: PrescanRegion fail - ALIGN (%ld, %ld) in invalid Region #%lu", 
				ulAlignRow, ulAlignCol, ulRegionNo);
			SetErrorMessage(szLog);

			szContent = "Select region is invalid.";
			HmiMessage_Red_Back(szContent, szTitle);
			return FALSE;
		}

		if (pSRInfo->GetRegionState(ulRegionNo) == WT_SUBREGION_STATE_BONDED)
		{
			//v0.08
			szLog.Format("ManualAlignWafer: PrescanRegion fail - ALIGN (%ld, %ld) in sorted Region #%lu", 
				ulAlignRow, ulAlignCol, ulRegionNo);
			SetErrorMessage(szLog);

			szContent = "Select region is sorted.";
			HmiMessage_Red_Back(szContent, szTitle);
			return FALSE;
		}

		szContent = "Only sorting one region under wafer MANUAL alignment!";

		if (pSRInfo->GetInitState() != WT_SUBREGION_INIT_ERROR)
		{
			pSRInfo->SetCurrentRegionState_HMI("M");
			HmiMessage(szContent, szTitle);
		}
		else
		{
			pSRInfo->SetManualAlignRegion(FALSE);
			szContent = "picking region is invalid, please setup reference die position again";
			HmiMessage(szContent, szTitle);
			return FALSE;
		}

		szLog.Format("Manual align (%d,%d) set target region %d", ulAlignRow, ulAlignCol, ulRegionNo);
		pUtl->RegionOrderLog(szLog);
		pSRInfo->SetTargetRegion(ulRegionNo);
		SaveRegionStateAndIndex();
	}


	GetEncoderValue();
	SetGlobalTheta();
	GetPrescanDiePitch();			// manual align
	Select1stLFSortGrade();			//v4.59A15	//SanAn & Semitek

	SetAlignmentStatus(TRUE);
	ResetMapStauts();

	SetCtmRefDieOptions();			//v3.03

	m_WaferMapWrapper.SetStartPosition(ulAlignRow, ulAlignCol);
	DisplayNextDirection(ulAlignRow, ulAlignCol);

	SaveScanTimeEvent("    WFT: To set scan auto align map");
	// prescan relative code
	SetPrescanAutoAlignMap();

	//Refresh wafermap
	SaveScanTimeEvent("    WFT: To draw map");
	m_WaferMapWrapper.Redraw();

	if (PrescanUpdateWaferAlignment(m_lStart_X, m_lStart_Y, m_ulAlignRow, m_ulAlignCol, 0))	// manual align
	{
		//================================update again ===========================
		//2018.4.24
		ULONG ulRegionNo = pSRInfo->GetWithinRegionNo(ulAlignRow, ulAlignCol);
		pSRInfo->SetTargetRegion(ulRegionNo);
		SaveRegionStateAndIndex();
		//========================================================================

		pSRInfo->SetManualAlignRegion(TRUE);
		pSRInfo->SetCurrentRegionState_HMI("M");
	}

	//Covert to user ROW & COL for display purpose only
	m_WaferMapWrapper.SetCurrentPosition(ulAlignRow, ulAlignCol);
	XY_SafeMoveTo(lAlignWftX, lAlignWftY);
	Sleep(50);


	LONG lOrgRow=0, lOrgCol=0, lHmiRow = 0, lHmiCol = 0;
	ConvertAsmToHmiUser(m_ulAlignRow, m_ulAlignCol, lHmiRow, lHmiCol);
	ConvertAsmToOrgUser(m_ulAlignRow, m_ulAlignCol, lOrgRow, lOrgCol); 
	szMsg.Format(" Die Map %ld, %ld (%ld, %ld)\n Die Pos (X,Y,T): (%ld, %ld, %ld)",
		lHmiRow, lHmiCol, lOrgRow, lOrgCol, lAlignWftX, lAlignWftY, lAlignWftT);
	if( IsBurnIn() == FALSE && bSecondTime==FALSE )
	{
		HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180);
	}

	szMsg.Format("Manual manual-Align wafer fcn done at Map %ld, %ld (%ld, %ld), WFT (X,Y,T): (%ld, %ld, %ld)",
		lHmiRow, lHmiCol, lOrgRow, lOrgCol, lAlignWftX, lAlignWftY, lAlignWftT);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	(*m_psmfSRam)["MS896A"]["WaferMapRow"] = m_ulAlignRow;
	(*m_psmfSRam)["MS896A"]["WaferMapCol"] = m_ulAlignCol;
	(*m_psmfSRam)["MS896A"]["WaferTableX"] = m_lStart_X;
	(*m_psmfSRam)["MS896A"]["WaferTableY"] = m_lStart_Y;
	
	
	//BOOL bOrgOption = m_bEnableAlignWaferImageLog;
	//GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_MAN);

	if ( bOriginal == TRUE )
	{
		SetJoystickOn(TRUE);
	}

	return TRUE;
}


LONG CWaferTable::AutoAlignWafer(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = FALSE;
	//	426TX	2
	LONG lReturn;
	CString szTitle, szContent;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();


	m_dStartWaferAlignTime = GetTime();	// auto align std wft
	
	SetAlignmentStatus(FALSE);

	CreateDummyMapForPrescanPicknPlace();	// auto align button

	if (m_WaferMapWrapper.IsMapValid() != TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Map is not valid");
		SetErrorMessage("Map is not valid");
		SetAlert_Red_Yellow(IDS_WL_MAP_NOT_VALID);
		BOOL bReply = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}


	if (!m_fHardware || m_bDisableWT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	SetJoystickOn(FALSE);
	if ( CheckSCNFileLoaded() == FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	//	427TX	5
	if (!m_WaferMapWrapper.IsMapValid())
	{
		HmiMessage("Map is invalid!\nPlease check", "Align Wafer Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);
		bReply = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	//Check SCN file is loaded or not
	if ( pUtl->GetPrescanDummyMap() == FALSE )
	{
		SetupSubRegionMode();		// auto align button
	}


	//Do not pop message when Auto search home is enable
	if (CheckLoadCurrentMapStatus() != TRUE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}


	szTitle.LoadString(HMB_WT_ALIGN_WAFER);
	szContent.LoadString(HMB_WT_START_ALIGN_WAFER);
	lReturn = HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	if ( lReturn != glHMI_YES )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	SetJoystickOn(TRUE);

	szContent.LoadString(HMB_WT_SET_ALIGN_POS);
	lReturn = HmiMessageEx(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	if ( lReturn != glHMI_CONTINUE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	((CMS896AApp*)AfxGetApp())->m_bOnBondMode = TRUE;	//ANDREW_SC
	
	CString szLog;
	szLog.Format("AutoAlign Wafer: BLKPICK=%d", IsBlkFuncEnable());
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	GetEncoderValue();
	SetJoystickOn(FALSE);

	m_lAlignPosX		= GetCurrX();
	m_lAlignPosY		= GetCurrY();

	//v3.02T6
	//Set default g-theta step size for STD config
	m_bAutoAlignWafer = TRUE;

	BOOL bAlignResult = AligningWafer();

	SetAlignmentStatus(bAlignResult);

	if (bAlignResult)
	{
		PrescanUpdateWaferAlignment(m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol, 1);	// half auto align by HMI
		if ( IsAlignedWafer() == FALSE )
		{
			bAlignResult = FALSE;
		}
	}

	if (!bAlignResult)
	{
		SetErrorMessage("Manual Auto-Align wafer fail - " + GetMapFileName());
	
		CMSLogFileUtility::Instance()->MS_LogOperation("AutoAlign Wafer stop");	

		bReply = bAlignResult;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		((CMS896AApp*)AfxGetApp())->m_bOnBondMode = FALSE;	//ANDREW_SC
		return 1;
	}


	ResetMapStauts();
	m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
	DisplayNextDirection(m_ulAlignRow, m_ulAlignCol);	

	SetCtmRefDieOptions();	

	Select1stLFSortGrade();	

	//Refresh wafermap
	m_WaferMapWrapper.Redraw();
	m_WaferMapWrapper.ResetGrade();	
	SetJoystickOn(TRUE);
	
		
	//CString szTitle, szContent;
	SetStatusMessage("Auto align wafer is finished - " + GetMapFileName());
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN Done & OK\n");		//v4.13T5
	CMSLogFileUtility::Instance()->MS_LogOperation("Manual Auto-Align wafer fcn done and OK - " + GetMapFileName());	


	szTitle.LoadString(HMB_WT_ALIGN_WAFER);
	szContent.LoadString(HMB_WT_AUTO_ALIGN_OK);
	HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	SaveData();

	bReply = bAlignResult;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	((CMS896AApp*)AfxGetApp())->m_bOnBondMode = FALSE;
	return 1;
}

/*
LONG CWaferTable::UpdateGThetaCounts(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;

	typedef struct {
		LONG	lLHSCount;
		LONG	lRHSCount;
	} GTheta_Counts;

	GTheta_Counts	stCount;
	svMsg.GetMsg(sizeof(GTheta_Counts), &stCount);

	m_lLHSIndexCount	= stCount.lLHSCount;
	m_lRHSIndexCount	= stCount.lRHSCount;

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}
*/

LONG CWaferTable::UpdateGThetaMode(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;

	LONG lValue;
	svMsg.GetMsg(sizeof(lValue), &lValue);
	m_lAlignWaferAngleMode_Std = lValue;

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferTable::UpdateHomeDiePhyPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;

	typedef struct {
		LONG	lLHSCount;
		LONG	lRHSCount;
	} GTheta_Counts;

	GTheta_Counts	stCount;
	svMsg.GetMsg(sizeof(GTheta_Counts), &stCount);

	m_lHomeDieWftPosnX	= stCount.lLHSCount;
	m_lHomeDieWftPosnY	= stCount.lRHSCount;

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferTable::UpdateSearchHomeOption(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;

	typedef struct {
		BOOL	lLHSCount;
		LONG	lRHSCount;
	} GTheta_Counts;

	GTheta_Counts	stCount;
	svMsg.GetMsg(sizeof(GTheta_Counts), &stCount);

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferTable::UpdateCornerSearchOption(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;

	ULONG bValue;
	svMsg.GetMsg(sizeof(bValue), &bValue);

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}




LONG CWaferTable::WaferMapHeadersPreTask(IPC_CServiceMessage &svMsg)
{
	CStdioFile cfWaferMapColFile;
	CString szNoOfWaferMapCols, szColHeader;
	BOOL bIfFileExists;
	LONG i;
	
	m_bEnableHeaderBtn = TRUE;

	m_lCurDefineHeaderPageNo = 1;
	m_lPrevDefineHeaderPageNo = 1;

	for (i=1; i<WT_MAX_MAP_COL; i++)
	{
		m_bIfEnableWaferMapColCtrl[i] = FALSE;
		m_lWaferMapColCtrlIndex[i] = i ;
	}

	//Display "WaferMapColumns.txt" contents on screen
	bIfFileExists = cfWaferMapColFile.Open("WaferMapColumns.txt",
		CFile::modeRead|CFile::shareExclusive|CFile::typeText);

	if (bIfFileExists)
	{
		cfWaferMapColFile.SeekToBegin();

		if (cfWaferMapColFile.ReadString(szNoOfWaferMapCols))
		{
			m_lNoOfWaferMapCols = atoi((LPCTSTR(szNoOfWaferMapCols)));
		}

		m_lNoOfWaferMapCols = min(m_lNoOfWaferMapCols, 9999);	//Klocwork	//v4.02T5

		for ( i=1; i<=m_lNoOfWaferMapCols; i++ )
		{
			i = min(i, WT_MAX_MAP_COL_ALL-1);		//Klocwork	//v4.02T5

			cfWaferMapColFile.ReadString(m_szAllWaferMapCol[i]);
			
			if (i <= (WT_MAX_MAP_COL - 1))
			{
				m_szWaferMapCol[i] = m_szAllWaferMapCol[i];
				m_bIfEnableWaferMapColCtrl[i] = TRUE;
			}
		}

		cfWaferMapColFile.Close();
	}
	else
	{
		IPC_CServiceMessage svMsg;
		SaveWaferMapColumnsFile(svMsg);
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end WaferMapHeadersPreTask


LONG CWaferTable::LoadWaferMapColumnsFile(IPC_CServiceMessage &svMsg)
{
	CString szColumnFilename, szFile, szUserFile;
	CString szInitPath;
	CString szTitle, szContent;

	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]=
      "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.txt", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , 
						szFilters, pMainWnd, 0);

	szInitPath = gszUSER_DIRECTORY + "\\OutputFile";
	dlgFile.m_ofn.lpstrInitialDir = szInitPath;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	szTitle.LoadString(HMB_WT_MAP_HEADER_FILE);

	if ( nReturn == IDOK )
	{
		szColumnFilename = dlgFile.GetFileName();

		if (LoadnCheckWaferMapColumnsFile(szColumnFilename) == TRUE)
		{
			szContent.LoadString(HMB_WT_LOAD_HEADER_OK);
			HmiMessage(szContent, szTitle);
			SetStatusMessage("Wafermap header file is loaded");
		}
		else
		{
			szContent.LoadString(HMB_WT_LOAD_HEADER_FAILED);
			HmiMessage(szContent, szTitle);
		}
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;

} //end LoadWaferMapColumnsFile


LONG CWaferTable::OnClickNoOfMapColsInput(IPC_CServiceMessage &svMsg)
{
	LONG lNoOfPages = 0;
	LONG lNoOfMapCols;

	svMsg.GetMsg(sizeof(LONG), &lNoOfMapCols);

	UpdateWaferMapHeaderCtrl(lNoOfMapCols);

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end OnClickNoOfMapColsInput


LONG CWaferTable::SaveWaferMapColumnsFile(IPC_CServiceMessage &svMsg)
{
	CStdioFile cfWaferMapColFile, cfSelectionFile;
	CString szNoOfCols;
	CString szAsmMapColFile;
	CString szAsmOPFileColFile;
	CString szUserMapColFile;
	LONG i;

	//DeleteFile("WaferMapColumns.txt");
	//DeleteFile("OutputFileColumnsSelection.txt");
	szAsmMapColFile		= gszROOT_DIRECTORY + "\\Exe\\WaferMapColumns.txt";
	szAsmOPFileColFile	= gszROOT_DIRECTORY + "\\Exe\\OutputFileColumnsSelection.txt";
	szUserMapColFile	= gszUSER_DIRECTORY + "\\OutputFile\\" + m_szMapHeaderFileName;

	DeleteFile(szAsmMapColFile);
	DeleteFile(szAsmOPFileColFile);
	
	UpdateWaferMapHeaderCtrl(m_lNoOfWaferMapCols);

	//Do NOT append
	if (!cfWaferMapColFile.Open(szAsmMapColFile, 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		HmiMessage("ERROR: fail to open c:\\MapSorter\\Exe\\WaferMapColumns.txt");
		BOOL bReply = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);

		return 1;
	}

	if (!cfSelectionFile.Open(gszROOT_DIRECTORY + "\\Exe\\OutputFileColumnsSelection.txt", 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		HmiMessage("ERROR: fail to open c:\\MapSorter\\Exe\\OutputFileColumnsSelection.txt");

		BOOL bReply = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);

		return 1;
	}


	szNoOfCols.Format("%d", m_lNoOfWaferMapCols);

	cfWaferMapColFile.WriteString(szNoOfCols + "\n");

	cfSelectionFile.WriteString("None,WaferID,Bin Row,Bin Col");

	for (i=1; i<=m_lNoOfWaferMapCols; i++ )
	{
		/*
		cfWaferMapColFile.WriteString(m_szWaferMapCol[i] + "\n");

		if (m_szWaferMapCol[i] != "")
			cfSelectionFile.WriteString("," + m_szWaferMapCol[i]);

		*/
		cfWaferMapColFile.WriteString(m_szAllWaferMapCol[i] + "\n");

		if (m_szAllWaferMapCol[i] != "")
			cfSelectionFile.WriteString("," + m_szAllWaferMapCol[i]);

		//m_bIfEnableOutColCtrl[i] = FALSE;
	}

	/*
	//Refresh variables on screen
	for (i=m_lNoOfWaferMapCols+1; i<WT_MAX_MAP_COL; i++ )
	{
		m_szWaferMapCol[i] = "";
		m_bIfEnableOutColCtrl[i] = FALSE;
	}
	*/

	cfWaferMapColFile.Close();

	DeleteFile(gszROOT_DIRECTORY + "\\Exe\\OutputFileFormat.txt");

	//Duplicate 1 set of file; file name is user input
	if (m_szMapHeaderFileName.IsEmpty() == FALSE)
	{
		CString szTitle, szContent;

		CopyFile(szAsmMapColFile, szUserMapColFile, FALSE);

		szTitle.LoadString(HMB_WT_MAP_HEADER_FILE);
		szContent.LoadString(HMB_WT_SAVE_HEADER_OK);

		HmiMessage(szContent, szTitle, 
				glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 600, 300, NULL, NULL, NULL, NULL);

		SetStatusMessage("New wafermap header file is created");
		SaveData();
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end SaveWaferMapColumnsFile

LONG CWaferTable::OnClickChangeHeaderPage(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	if (m_lCurDefineHeaderPageNo > 6)
	{
		svMsg.InitMessage(sizeof(BOOL),&bReturn);
		return 1;
	}

	
	UpdateWaferMapHeaderCtrl(m_lNoOfWaferMapCols);

	m_lPrevDefineHeaderPageNo = m_lCurDefineHeaderPageNo;

	svMsg.InitMessage(sizeof(BOOL),&bReturn);

	return 1;
}


LONG CWaferTable::OutputFileFormatPreTask(IPC_CServiceMessage &svMsg)
{
	CStdioFile cfFormatFile;
	CString szLineData, szTemp;
	BOOL bIfFileExists;
	int nIndex;
	LONG i;
	CString szFieldname, szMin, szMax, szAvg, szStd;


	//Initialize all variables on screen
	m_bEnableOutputBtn = TRUE;
	m_lNoOfOutFileCols = 1;
	m_lCurOutputFilePageNo = 1;
	m_lPrevOutputFilePageNo = 1;

	for (i=1; i<WT_MAX_OUTPUT_COL; i++)
	{
		m_bIfEnableOutColCtrl[i] = FALSE;
		m_lOutColCtrlIndex[i] = i;
		m_szOutFileCol[i] = "None";
		m_bOutFileColMin[i] = FALSE;
		m_bOutFileColMax[i] = FALSE;
		m_bOutFileColAvg[i] = FALSE;
		m_bOutFileColStd[i] = FALSE;
	}

	//Display "OutputFileFormat.txt" contents on screen
	bIfFileExists = cfFormatFile.Open("OutputFileFormat.txt",
		CFile::modeRead|CFile::shareExclusive|CFile::typeText);

	if (bIfFileExists)
	{
		cfFormatFile.SeekToBegin();

		cfFormatFile.ReadString(szLineData); //skip first line (columns description)

		m_lNoOfOutFileCols = 0;

		while(cfFormatFile.ReadString(szLineData) != NULL)
		{
			if ( szLineData.IsEmpty() == TRUE )
			{
				break;		
			}

			m_lNoOfOutFileCols++;

			nIndex = szLineData.Find(",");
			szFieldname = szLineData.Left(nIndex);

			szLineData = szLineData.Mid(nIndex + 1);
			nIndex = szLineData.Find(",");
			szMin = szLineData.Left(nIndex);

			szLineData = szLineData.Mid(nIndex + 1);
			nIndex = szLineData.Find(",");
			szMax = szLineData.Left(nIndex);

			szLineData = szLineData.Mid(nIndex + 1);
			nIndex = szLineData.Find(",");
			szAvg = szLineData.Left(nIndex);


			szStd = szLineData.Mid(nIndex + 1);


			if (m_lNoOfOutFileCols < WT_MAX_OUTPUT_COL)
			{
				m_bIfEnableOutColCtrl[m_lNoOfOutFileCols] = TRUE;

				if (m_szOutFileCol[m_lNoOfOutFileCols] == " ")
					m_szOutFileCol[m_lNoOfOutFileCols] = "None";
				else
					m_szOutFileCol[m_lNoOfOutFileCols] = szFieldname;

				if (szMin == "1")
					m_bOutFileColMin[m_lNoOfOutFileCols] = TRUE;
				else
					m_bOutFileColMin[m_lNoOfOutFileCols] = FALSE;

				if (szMax == "1")
					m_bOutFileColMax[m_lNoOfOutFileCols] = TRUE;
				else
					m_bOutFileColMax[m_lNoOfOutFileCols] = FALSE;

				if (szAvg == "1")
					m_bOutFileColAvg[m_lNoOfOutFileCols] = TRUE;
				else
					m_bOutFileColAvg[m_lNoOfOutFileCols] = FALSE;

				if (szStd == "1")
					m_bOutFileColStd[m_lNoOfOutFileCols] = TRUE;
				else
					m_bOutFileColStd[m_lNoOfOutFileCols] = FALSE;	
			}

			
			if (m_szAllOutFileCol[m_lNoOfOutFileCols] == " ")
				m_szAllOutFileCol[m_lNoOfOutFileCols] = "None";
			else
				m_szAllOutFileCol[m_lNoOfOutFileCols] = szFieldname;

			if (szMin == "1")
				m_bAllOutFileColMin[m_lNoOfOutFileCols] = TRUE;
			else
				m_bAllOutFileColMin[m_lNoOfOutFileCols] = FALSE;

			if (szMax == "1")
				m_bAllOutFileColMax[m_lNoOfOutFileCols] = TRUE;
			else
				m_bAllOutFileColMax[m_lNoOfOutFileCols] = FALSE;

			if (szAvg == "1")
				m_bAllOutFileColAvg[m_lNoOfOutFileCols] = TRUE;
			else
				m_bAllOutFileColAvg[m_lNoOfOutFileCols] = FALSE;

			if (szStd == "1")
				m_bAllOutFileColStd[m_lNoOfOutFileCols] = TRUE;
			else
				m_bAllOutFileColStd[m_lNoOfOutFileCols] = FALSE;	

		}

		cfFormatFile.Close();
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(bReply), &bReply); //Send reply

	return 1;
} //end OutputFileFormatPreTask

LONG CWaferTable::OnClickNoOfOutputColsInput(IPC_CServiceMessage &svMsg)
{
	LONG lNoOfOutputCols;

	svMsg.GetMsg(sizeof(LONG), &lNoOfOutputCols);

	UpdateOutputFileCtrl(lNoOfOutputCols);
	/*

	for (i=1; i<WT_MAX_OUTPUT_COL; i++)
	{
		m_bIfEnableOutColCtrl[i] = FALSE;
	}
	
	for (i=1; i<=lNoOfOutputCols; i++)
	{
		m_bIfEnableOutColCtrl[i] = TRUE;
	}
	*/

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end OnClickNoOfOutputColsInput


LONG CWaferTable::OnClickChangeOutputFilePage(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	if (m_lCurOutputFilePageNo > 6)
	{
		svMsg.InitMessage(sizeof(BOOL),&bReturn);
		return 1;
	}

	
	UpdateOutputFileCtrl(m_lNoOfOutFileCols);

	m_lPrevOutputFilePageNo = m_lCurOutputFilePageNo;

	svMsg.InitMessage(sizeof(BOOL),&bReturn);

	return 1;
}

LONG CWaferTable::LoadOutputFileFormatFile(IPC_CServiceMessage& svMsg)
{
	CString szFormatFilename;
	CString szInitPath, szContents;
	CString szTitle, szContent;
	BOOL bFileOK = FALSE;

	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]=
      "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.txt", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , 
						szFilters, pMainWnd, 0);

	szInitPath = gszUSER_DIRECTORY + "\\OutputFile";
	dlgFile.m_ofn.lpstrInitialDir = szInitPath;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}


	szTitle.LoadString(HMB_WT_OTF_FORMAT_FILE);

	if ( nReturn == IDOK )
	{
		szFormatFilename = dlgFile.GetFileName();

		if (LoadnCheckOutputFileFormatFile(szFormatFilename) == TRUE)
		{
			szContent.LoadString(HMB_WT_LOAD_OTF_OK);
			HmiMessage(szContent, szTitle);
			SetStatusMessage("Output format file is loaded");
		}
		else
		{
			szContent.LoadString(HMB_WT_LOAD_OTF_FAILED);
			HmiMessage(szContent, szTitle);

		}
		
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end LoadOutputFileFormatFile


LONG CWaferTable::SaveOutputFileFormatFile(IPC_CServiceMessage &svMsg)
{
	CStdioFile cfFormatFile;
	LONG lNoOfOutFileCols;
	LONG i;

	CString szAsmFormatFile;
	CString szUserFormatFile;

	svMsg.GetMsg(sizeof(LONG), &lNoOfOutFileCols);

	UpdateOutputFileCtrl(lNoOfOutFileCols);

	szAsmFormatFile		= gszROOT_DIRECTORY + "\\Exe\\OutputFileFormat.txt";
	szUserFormatFile	= gszUSER_DIRECTORY + "\\OutputFile\\" + m_szOutputFormatFileName;

	DeleteFile(szAsmFormatFile);

	//Do NOT append
	if (!cfFormatFile.Open("OutputFileFormat.txt", 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		HmiMessage("Error: cannot open file! (OutputFileFormat.txt)");
		BOOL bReply = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);

		return 1;
	}

	cfFormatFile.WriteString("<Column,Min,Max,Avg,Std>\n");

	for (i=1; i<=lNoOfOutFileCols; i++)
	{
		if (m_szAllOutFileCol[i] == "None")
		{
			m_szAllOutFileCol[i] = " ";
		}

		cfFormatFile.WriteString(m_szAllOutFileCol[i]);

		if (m_bAllOutFileColMin[i])
			cfFormatFile.WriteString(",1");
		else
			cfFormatFile.WriteString(",0");

		if (m_bAllOutFileColMax[i])
			cfFormatFile.WriteString(",1");
		else
			cfFormatFile.WriteString(",0");

		if (m_bAllOutFileColAvg[i])
			cfFormatFile.WriteString(",1");
		else
			cfFormatFile.WriteString(",0");

		if (m_bAllOutFileColStd[i])
			cfFormatFile.WriteString(",1\n");
		else
			cfFormatFile.WriteString(",0\n");
	}

	/*
	//refresh variables on screen
	for (i=lNoOfOutFileCols+1; i<WT_MAX_OUTPUT_COL; i++)
	{
		m_szOutFileCol[i] = "None";
		m_bOutFileColMin[i] = FALSE;
		m_bOutFileColMax[i] = FALSE;
		m_bOutFileColAvg[i] = FALSE;
		m_bOutFileColStd[i] = FALSE;
	}
	*/

	cfFormatFile.Close();

	//Duplicate 1 set of file; file name is user input
	if (m_szOutputFormatFileName.IsEmpty() == FALSE)
	{
		CString szTitle, szContent;

		CopyFile(szAsmFormatFile, szUserFormatFile, FALSE);

		szTitle.LoadString(HMB_WT_OTF_FORMAT_FILE);

		szContent.LoadString(HMB_WT_SAVE_OTF_OK);

		HmiMessage(szContent, szTitle, 
				glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 600, 300, NULL, NULL, NULL, NULL);

		SetStatusMessage("New output format file is created");
		SaveData();
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end SaveOutputFileFormatFile


LONG CWaferTable::LoadMapFileForBurnIn(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;
	CString szTempPath;
	CString szTemp;

//#ifdef NU_MOTION
//	if (CMS896AApp::m_ucNuSimCardType != GMODE_NORMAL_SUITE)
//	{
//		szTempPath = m_szBurnInMapFile;
//		CString szDrive, szDirectory, szFileName, szExtension;
//
//		_splitpath(szTempPath.GetBuffer(MAX_PATH),
//			szDrive.GetBuffer(_MAX_DRIVE), szDirectory.GetBuffer(_MAX_DIR),
//			szFileName.GetBuffer(_MAX_FNAME), szExtension.GetBuffer(_MAX_EXT) );
//
//		szTempPath.ReleaseBuffer();
//		szDrive.ReleaseBuffer();
//		szDirectory.ReleaseBuffer();
//		szFileName.ReleaseBuffer();
//		szExtension.ReleaseBuffer();
//
//		szTemp = gzDRIVE;
//		szTemp = szTemp.MakeUpper();
//		
//		if (szDrive != gzDRIVE && szDrive != szTemp)
//		{
//			m_WaferMapWrapper.InitMap();
//
//			CString szTitle, szContent;
//			szTitle.LoadString(HMB_WT_BURNIN_LOAD_MAP);
//			szContent.LoadString(HMB_WT_LOAD_MAP_FROM_C_DRIVE);
//			HmiMessage(szContent, szTitle);
//			
//			svMsg.InitMessage(sizeof(BOOL), &bReply);
//			return 1;
//		}
//	}
//#endif

	LoadWaferMap(m_szBurnInMapFile, "");
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

LONG CWaferTable::BurnInAutoAssignGrade(IPC_CServiceMessage &svMsg)
{
	ULONG ulMaxGrade = 100;

	svMsg.GetMsg(sizeof(ULONG), &ulMaxGrade);

	if( ulMaxGrade<=0 )
		ulMaxGrade = 100;

	m_lBurnInMaxGrade = (LONG)ulMaxGrade;

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferTable::SetTableStartPointForBurnIn(IPC_CServiceMessage &svMsg)
{
	Sleep(50);
	GetEncoderValue();
	SetGlobalTheta();			//v4.16T5

	m_lStart_X	= GetCurrX();
	m_lStart_Y	= GetCurrY();
	m_lStart_T	= GetCurrT();		//v4.16T5

	m_lBurningInGrade = 1;
	if (m_bPostBondAtBond)		//v4.47A7
	{
		//Remove old PB collet-hole log files
		DeleteFile(_T("c:\\Mapsorter\\UserData\\BondPr.txt")); 
		DeleteFile(_T("c:\\Mapsorter\\UserData\\BondPr2.txt")); 
	}
	if (m_bPreBondAtPick)		//v4.47A7
	{
		//Remove old PB collet-hole log files
		DeleteFile(_T("c:\\Mapsorter\\UserData\\WaferPr.txt")); 
		DeleteFile(_T("c:\\Mapsorter\\UserData\\WaferPr2.txt")); 
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}


LONG CWaferTable::SetMapStartPointForBurnIn(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;

	ULONG ulAlnRow = 0, ulAlnCol = 0;
	if( m_bBurnInAutoMap )
	{
		m_szBurnInMapFile			= gszROOT_DIRECTORY + "\\Exe\\AutoDummyMap.txt";
		CreateDummyMapForBurnIn(m_lStart_X, m_lStart_Y, m_szBurnInMapFile);
		ConvertOrgUserToAsm(0, 0, ulAlnRow, ulAlnCol);
		m_WaferMapWrapper.SetSelectedPosition(ulAlnRow, ulAlnCol);
		m_WaferMapWrapper.SetCurrentPosition(ulAlnRow, ulAlnCol);
	}

	if( m_bBurnInAutoGrade )
	{
		m_WaferMapWrapper.GetSelectedPosition(ulAlnRow, ulAlnCol);
	}

	if ( m_WaferMapWrapper.IsMapValid() == TRUE )
	{
		if( m_bBurnInAutoMap==FALSE && m_bBurnInAutoGrade)
		{
			UCHAR ucMapGrade = (UCHAR)m_lBurningInGrade + m_WaferMapWrapper.GetGradeOffset();
			for(LONG ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
			{
				for(LONG ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
				{
					if (m_pWaferMapManager->IsMapHaveBin(ulRow, ulCol) &&
						m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol)==FALSE)
					{
						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucMapGrade);
					}
				}
			}

			unsigned char aaTempGrades[1];
			aaTempGrades[0] = ucMapGrade;

			m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aaTempGrades[0]);

			m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);
		}

		SetAlignmentStatus(TRUE);
		m_bScnLoaded	= FALSE;
		bReply			= TRUE;

		if( m_bBurnInAutoMap || m_bBurnInAutoGrade)
		{
			m_lBurningInGrade++;
			if( m_lBurningInGrade>m_lBurnInMaxGrade )
				m_lBurningInGrade = 1;
			CString szMsg;
			szMsg.Format("grade %d, max %d", m_lBurningInGrade, m_lBurnInMaxGrade);
			SetErrorMessage(szMsg);
			m_WaferMapWrapper.EnableAutoAlign(TRUE);
		}

		SetMapPhyPosn(ulAlnRow, ulAlnCol, m_lStart_X, m_lStart_Y);
		m_WaferMapWrapper.SetStartPosition(ulAlnRow, ulAlnCol);
	}
	else
	{
		bReply = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}


LONG CWaferTable::StartPickAndPlace(IPC_CServiceMessage &svMsg)
{
	BOOL	bPar;
	BOOL	bReturn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bPar);

	if (IsEnablePNP() == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;

	}

	//Disbale Block Function
	SetBlkFuncEnable(FALSE);

	m_WaferMapWrapper.DeleteAllBoundary();
	
	if (m_bEnableSmartWalkInPicknPlace == FALSE)
	{
		m_WaferMapWrapper.EnableCommunication(!bPar);
	}

	// check in manual mode only
	if ( IsWLManualMode() )
	{
		if ( bPar == TRUE)
		{
			if ( IsAlignedWafer() == FALSE )
			{
				SetAlert(IDS_WT_MAP_NOT_ALIGN);
				SetErrorMessage("Wafer is not aligned");
				bReturn = FALSE;
			}

			if ( m_ucWaferLimitType != WT_CIRCLE_LIMIT )
			{
				SetAlert(IDS_WT_INVALID_LIMIT_TYPE);
				SetErrorMessage("Invalid wafer limit type");
				bReturn = FALSE;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
} //end StartPickAndPlace


LONG CWaferTable::StartBurnIn(IPC_CServiceMessage &svMsg)
{
	BOOL	bPar = FALSE;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bPar);

	//Disbale Block Function
	SetBlkFuncEnable(FALSE);

	m_ulNewPickCounter = 0;
	(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 0;
	m_lReadyToSampleOnWft = 0;
	m_lReadyToCleanEjPinOnWft = 0;
	m_bPitchAlarmOnce	= FALSE;
	m_bReSampleAsError	= FALSE;

	SetAlignmentStatus(bPar);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
} //end StartPickAndPlace


LONG CWaferTable::WTBurnInSetup(IPC_CServiceMessage &svMsg)
{
	BOOL	bPar = FALSE;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bPar);

	//Disbale Block Function
	SetBlkFuncEnable(FALSE);

	SetAlignmentStatus(bPar);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
} //end WT burn in setup

LONG CWaferTable::SearchFilePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;
	
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp->GetCustomerName() == "BlueLight" ) 
	{
		CString KeyWord = m_szMapDisplayFilename;
		CString KeyExtension = m_szMapFileExtension;
		CString KeyFilePath	= m_szMapFilePath;
		
		CStringList szSelection;

		WIN32_FIND_DATA FileData;
		HANDLE hSearch;
		hSearch = FindFirstFile(KeyFilePath + "\\" + "*" + KeyWord + "*." + KeyExtension, &FileData);
		if ( hSearch == INVALID_HANDLE_VALUE ) 
		{ 
			bReply = FALSE;
			HmiMessage("No File Found");
		}
		else 
		{
			CString szTitle = "File Found";
			CString szContent = "Please Select File:";
			LONG lselection = 0;
			if ( FileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
			{
				szSelection.AddHead(/*KeyFilePath + "\\" +*/ FileData.cFileName);			
			}
			while (FindNextFile(hSearch, &FileData) == TRUE)
			{
				if ( FileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
				{
					szSelection.AddTail(/*KeyFilePath + "\\" +*/ FileData.cFileName);
				}
			}
			LONG lResult = 0;
			lResult = HmiSelection(szContent, szTitle, szSelection, lselection);
			CString szTest;
			szTest.Format( "%d" , lResult );
			//AfxMessageBox("lResult: " + szTest, MB_SYSTEMMODAL);

			if ( lResult < 0 ) 
			{
				bReply = FALSE;
			}
			else 
			{
				bReply = TRUE;
				POSITION pos;
				if ((pos = szSelection.FindIndex(lResult)) != NULL)
				{
					KeyWord = szSelection.GetAt(pos);
					//int a = KeyWord.Find(".");
					int a = KeyWord.ReverseFind('.');
					if ( a > -1 ) 
					{
						KeyWord = KeyWord.Mid( 0, a );
					}
					m_szMapDisplayFilename = KeyWord; 
					//AfxMessageBox("Path:\n" + KeyWord, MB_SYSTEMMODAL);
				}
				else 
				{
					AfxMessageBox("Index Error!", MB_SYSTEMMODAL);
					bReply = FALSE;
				}
			}
		}
	}
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferTable::LoadMapWithBarCode(IPC_CServiceMessage &svMsg)
{
	BOOL bLoadSuccess = TRUE;
	CString szNewName;
	BOOL	bReturn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	szNewName = m_szBCMapFilename;

	//v4.45T3	//Electech 3E DL
	if( m_bBarCodeSearchFolder )
	{
		CString szMsg;
		int nPosn = szNewName.Find("-");
		if( nPosn!=-1 )
		{
			m_szSearchFolderName = szNewName.Left(nPosn);
		}
		else
		{
			szMsg.Format("find wafer lot fail barcode %s", szNewName);
			SetErrorMessage(szMsg);
			bLoadSuccess = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bLoadSuccess);
			return 1;
		}
	}
	if( LoadMapWithBarCodeName(szNewName) )
	{
		//v4.45T3	//3E DL
		if (pApp->GetCustomerName() == "Electech3E(DL)")
		{
			m_bIsWaferEndedFor3EDL = FALSE;		//reset status
		}
		if (pApp->GetCustomerName() == "Semitek")
		{
			CString szPath = "c:\\mapsorter\\exe\\binoutputfiles\\Semitek2.dll";
			if (_access(szPath,0) != -1)
			{
				(*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK1] = m_szBCMapFilename;
				CMSLogFileUtility::Instance()->WL_LogStatus("Semitek Load Map Start");
				IPC_CServiceMessage stMsg;
				int nConvID;
				nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SemitekGenerateOutputFileWhenReadWaferBarcode", stMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 60000) == TRUE )
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
		}
		////4.52D10 // wrong calling fnc without 
		//if (pApp->GetCustomerName() == "Lumileds" && pApp->GetProductLine() == "Rebel" && pApp->m_bWaferLabelFile)
		//{
		//	pApp->PrintRebealLabel(); 		
		//}
		svMsg.InitMessage(sizeof(BOOL), &bLoadSuccess);
		return 1;
	}
	CString szName1 = "", szName2 = "";
	if( m_bLoadMapAccessFail )
	{
		szName1 = m_szAccessFailFileName;
	}
	if( m_bBarCodeSearchPartMap )
	{
		CString szFolderName;
		szFolderName = "-";
		CString szMsg;
		szMsg.Format("research %s, folder %s, barcode %s", szFolderName, m_szSearchFolderName, szNewName);
		SetErrorMessage(szMsg);
		int nPosn = szNewName.Find(szFolderName);
		if( nPosn!=-1 )
		{
			CString szNewOne;
			szNewOne = szNewName.Mid(nPosn+1);
			szMsg.Format("to search %s", szNewOne);
			SetErrorMessage(szMsg);
			if( LoadMapWithBarCodeName(szNewOne) )
			{
				svMsg.InitMessage(sizeof(BOOL), &bLoadSuccess);
				return 1;
			}
			else
			{
				if( m_bLoadMapAccessFail )
				{
					szName2 = m_szAccessFailFileName;
				}
			}
		}
	}

	if( m_bLoadMapAccessFail )
	{
		CMS896AApp::m_bMapLoadingFinish = TRUE;
		CMS896AApp::m_bMapLoadingAbort = TRUE;	//v4.40T10
		CString szTitle, szContent, szDisplay;
		szTitle.LoadString(HMB_WT_LOAD_MAP_FAILED);
		szContent.LoadString(HMB_WT_MAPFILE_NOT_EXIST);
		szDisplay = szContent + " : " + szName1;
		if( szName2!="" )
			szDisplay = szDisplay + " : " + szName2;

		SetAlarm(HMB_WT_MAPFILE_NOT_EXIST, TRUE);		//v4.59A22	//Osram
		HmiMessage_Red_Yellow(szDisplay, szTitle);
		ClearAlarm(HMB_WT_MAPFILE_NOT_EXIST, TRUE);		//v4.59A22
		SetErrorMessage(szDisplay);
		CMSLogFileUtility::Instance()->WL_LogStatus(szDisplay);
	}

	bLoadSuccess = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bLoadSuccess);

	return 1;
} //end 
LONG CWaferTable::LoadMapWithBarCodeName(CString szMapBCFileName)
{
	CString szTitle, szContent;
	LONG i;
	CString strTemp;

	m_bLoadMapAccessFail		= FALSE;
	m_szAccessFailFileName		= "";

	CString szLotNum		= (*m_psmfSRam)["MS896A"]["LotNumber"];

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( pApp->GetCustomerName() == "DeLi")
	{
		int t = szMapBCFileName.Find(" ");
		if (t > 0)
			szMapBCFileName = szMapBCFileName.Mid(t);
		szMapBCFileName = szLotNum + " " + szMapBCFileName;
		CMSLogFileUtility::Instance()->MS_LogOperation("DeLi New Map File Name:" + szMapBCFileName);
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("LoadMapWithBarCodeName:" + szMapBCFileName + "," + m_szPrefixName);
	if ( m_bEnablePrefix && m_bEnablePrefixCheck && (m_szPrefixName.IsEmpty() == FALSE) )		//Check prefix existence	//v3.61	//NeoNeon
	{
		if (pApp->GetCustomerName() == "Epitop")
		{
			if (CheckPrefixNameForEpitop(szMapBCFileName,m_szPrefixName) == FALSE)
			{
				SetErrorMessage("WT: barcode Load map check prefix fail - BC= " + szMapBCFileName + " - " + m_szPrefixName);
				HmiMessage_Red_Yellow("WT: barcode auto Load map check prefix fail(Epitop)");		//v4.50
				return FALSE;
			}
		}
		else
		{
			if (szMapBCFileName.Find(m_szPrefixName) == -1)
			{
				SetErrorMessage("WT: barcode Load map check prefix fail - BC= " + szMapBCFileName + " - " + m_szPrefixName);
				HmiMessage_Red_Yellow("WT: barcode auto Load map check prefix fail");		//v3.65
				return FALSE;
			}
		}
	}


	if ( m_bEnableSuffix && m_bEnableSuffixCheck && (m_szSuffixName.GetLength() > 0) )	//v4.36		//SanAn
	{
		if (szMapBCFileName.Find(m_szSuffixName) == -1)
		{
			SetErrorMessage("WT: barcode auto Load map check suffix fail - BC=" + szMapBCFileName + " - " + m_szSuffixName);
			HmiMessage_Red_Yellow("WT: barcode auto Load map check suffix fail");		//v3.65
			return FALSE;
		}
	}

	if( pUtl->GetPrescanDummyMap() )
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("LoadMapWithBarCodeName: dummy map");
		m_szDummyWaferMapName = szMapBCFileName;	// loader load map for dummy scan
		if( IsMS90HalfSortDummyScanAlign() )
		{
			CreateDummyMapForPrescanPicknPlace(FALSE, TRUE);	// MS90 half sort mode, load map by WL and not burn in and not contour
		}
		return TRUE;
	}

	CString szLclPath	= LOCALHD_MAP_PATH;
	CString szMapPath	= GetMapFilePath();
	CString szMapNameWoPath = szMapBCFileName;

	if( m_bEnablePrefix && m_bEnablePrefixCheck==FALSE && (m_szPrefixName.IsEmpty() == FALSE) )		//Add filename prefix if necessary
	{
		szMapNameWoPath	= m_szPrefixName + szMapBCFileName;	//v3.74
		//PLLM Traceability project only
		if( (m_lOTraceabilityFormat == 2) || ((m_lOTraceabilityFormat == 1) && (!m_bOTraceabilityDisableLotID)) )			//PLLM SUBMOUNT only
		{
			szMapNameWoPath	= m_szPrefixName + szLotNum + "_" + szMapBCFileName;	//v3.74
		}
		else if (m_lOTraceabilityFormat == 1)		//PLLM LED only
		{
			CString szProductPartID	= (*m_psmfSRam)["MS896A"]["Operator Id"];
			INT nCol = szProductPartID.Find("-");
			if (nCol != -1)
			{
				CString szLabel = szProductPartID.Mid(nCol+1);
				if ( (szLabel.Find("L") != -1) || (szLabel.Find("l") != -1) )		//v3.23T1
				{
					ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
					if (nPLLM != PLSG_DIEFAB)	//v3.93	//Quah
					{
						szMapPath = szMapPath + "\\Ingan2_5";
						szLclPath = szLclPath + "\\Ingan2_5";
					}
				}
			}
			//Modified for PLLS (singapore) Die-Fab line
		}

		if ( m_bMatchFileName == TRUE )
		{
			if ( MatchMapFile(szMapPath) == TRUE )
			{
				szMapNameWoPath = szMapBCFileName;	//v3.74
			}
			else
			{
				SetErrorMessage("WT: auto barcode Load map match file name fail");
				return FALSE;
			}
		}
	}

	if (m_bEnableSuffix && !m_bEnableSuffixCheck && (m_szSuffixName.GetLength() > 0) )			//v3.65		//Semitek
	{
		szMapNameWoPath = szMapNameWoPath + m_szSuffixName;		//v3.74
	}

	CString szSrvFullName	= szMapPath + "\\" + szMapNameWoPath;
	CString szLclFullName	= szLclPath + "\\" + szMapNameWoPath;

	CString szRemoteSCNFile = szSrvFullName + ".SCN";
	CString szLocalSCNFile	= szLclFullName + ".SCN";
	CString szRemotePSMFile = szSrvFullName + ".PSM";
	CString szLocalPSMFile	= szLclFullName + ".PSM";

	//Add extension if necessary
	if ( m_szMapFileExtension.IsEmpty() == FALSE)
	{
		szSrvFullName	= szSrvFullName		+ "." + m_szMapFileExtension;
		szLclFullName	= szLclFullName		+ "." + m_szMapFileExtension;
	}

	BOOL bIsBackupMapLoaded = FALSE;
	CString szNewBackupMap ;

	CString szTempMsg;
	szTempMsg.Format("WaferLabel -- Auto Load Bk Map, enable status: %d", m_bEnableAutoLoadBackupMap);
	CMSLogFileUtility::Instance()->WL_LogStatus(szTempMsg);	

	if (m_bEnableAutoLoadBackupMap)
	{
		CString szBackupMap		= m_szBackupMapPath + "\\" + szMapNameWoPath;		//v3.74
		szNewBackupMap	= szBackupMap + ".aaa";			//temp map file for backup map
		
		if( m_szBackupMapExtension.IsEmpty() == FALSE)
		{
			szBackupMap = szBackupMap + "." + m_szBackupMapExtension;
		}

		CString szMsg;
		szMsg = "Try to find Auto-Load backup map at: " + szBackupMap;
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);		//v3.74

		for(int i = 0; i < AUTO_LOAD_MAP_RETRY; i++)
		{
			if (_access(szBackupMap, 0) != -1)						//If backup map already exist
			{
				TRY 
				{
					DeleteFile(szNewBackupMap);						//Delete last temp map if present
				}
				CATCH (CFileException, e)
				{
				}
				END_CATCH

				TRY 
				{
					if ( CopyFileWithRetry(szBackupMap, szNewBackupMap, FALSE) )	//Copy backup map to temp map
					{
						szSrvFullName = szNewBackupMap;					//Set temp map as load-map
						bIsBackupMapLoaded = TRUE;
						szMsg = "Will load Auto-Load backup map OK at: " + szNewBackupMap;		//PLLM v3.74T34
						CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);		//v3.74
						break;
					}
				}
				CATCH (CFileException, e)
				{
					CMSLogFileUtility::Instance()->MS_LogOperation("WT: auto barcode Load backup map fail");
				}
				END_CATCH
			}
			Sleep(1000);
		}	//	try 3 times to get the target backup map file
	}

	if (m_bBarCodeSearchMap && !bIsBackupMapLoaded)
	{
		CString szSearchName;
		BOOL bFindFile = FALSE;

		if( m_bBarCodeSearchPartMap )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT search part map");
			CString szInName = szMapBCFileName;
			if (m_bEnableSuffix && m_szSuffixName.GetLength() > 0)
			{
				szInName = szMapBCFileName + m_szSuffixName;
			}
			for(int i=0; i<AUTO_LOAD_MAP_RETRY; i++)
			{
				bFindFile = SearchFileAndFolder(szSearchName, szMapPath, szInName);
				if( bFindFile )
				{
					break;
				}
				Sleep(1000);
			}
		}
		else if( m_szSuffixName.GetLength()==0 )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT search in folder:" + szMapBCFileName + " path:" + szMapPath);
			for(int i=0; i<AUTO_LOAD_MAP_RETRY; i++)
			{
				bFindFile = SearchMapFileInFolder(szSearchName, szMapPath, szMapBCFileName);
				if( bFindFile )
				{
					break;
				}
				Sleep(1000);
			}
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT suffix name is not zero:" + m_szSuffixName);
		}

		if (bFindFile)
		{
			szSrvFullName	= szMapPath + "\\" + szSearchName;		//Dis-regard all prefix, extension, etc.
			szLclFullName	= szLclPath + "\\" + szSearchName;	
			if( pApp->GetCustomerName()=="HuaLei" )
			{
				szSrvFullName = szSrvFullName.MakeUpper();
				szLclFullName = szLclFullName.MakeUpper();
				CMSLogFileUtility::Instance()->WL_LogStatus(szSrvFullName + "HuaLei to upper case");
				CMSLogFileUtility::Instance()->WL_LogStatus(szLclFullName + "HuaLei to upper case");
			}
			CMSLogFileUtility::Instance()->WL_LogStatus(szSrvFullName);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLclFullName);
			// for psm and scn
			CString szTempMapFullName, szTempLocalMap;
			int iCol;
			iCol = szSrvFullName.ReverseFind('.');
			if( iCol == -1 )
			{
				szTempMapFullName = szSrvFullName;	
			}
			else
			{
				szTempMapFullName = szSrvFullName.Left(iCol);
			}
			iCol = szLclFullName.ReverseFind('.');
			if( iCol==-1 )
			{
				szTempLocalMap = szLclFullName;
			}
			else
			{
				szTempLocalMap = szLclFullName.Left(iCol);
			}

			szRemoteSCNFile = szTempMapFullName + ".SCN";
			szLocalSCNFile	= szTempLocalMap + ".SCN";
			szRemotePSMFile = szTempMapFullName + ".PSM";
			szLocalPSMFile	= szTempLocalMap + ".PSM";
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT sarch folder fail");
		}	// no finding
	}

	if (PsmFileExistCheck(szRemotePSMFile) == FALSE)
	{
		return FALSE;
	}

	m_szMapServerFullPath = szSrvFullName;
	BOOL bIfLoadSuccess = FALSE;
	//Only available in Auto-Load-Map fcn
	if (m_bEnableLocalLoadMap)		//v3.15T5
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WT load map with bar code in local HD");
		// Use local map loading instead of network load to avoid hang-up due to networking problem
		//Try to create local MAP folder if not exist
		if ( CreateDirectory(LOCALHD_MAP_PATH, NULL) == 0 )
		{
			if ( GetLastError() != ERROR_ALREADY_EXISTS )
			{
				HmiMessage_Red_Yellow("Fail to create CurrMap sub-folder!");
				return FALSE;
			}
		}
		
		DeleteLastLocalMapFile();	

		TRY 
		{
			CString szMsg;
			BOOL bFail = TRUE;
			for(int i=0; i<AUTO_LOAD_MAP_RETRY; i++)
			{
				if (_access(szSrvFullName, 0) != -1)
				{
					bFail = FALSE;
					break;
				}
				Sleep(500);
			}
			if( bFail )
			{
				m_bLoadMapAccessFail = TRUE;
				m_szAccessFailFileName		= szSrvFullName;
				szMsg = "Load with BC Fail to access network folder! - " + szSrvFullName;
				HmiMessage_Red_Yellow(szMsg);
				SetErrorMessage(szMsg);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				return FALSE;
			}

			bFail = TRUE;
			for(int i=0; i<AUTO_LOAD_MAP_RETRY; i++)
			{
				if ( CopyFileWithRetry(szSrvFullName, szLclFullName, FALSE))
				{
					bFail = FALSE;
					break;
				}
				Sleep(500);
			}
			if( bFail )
			{
				szMsg.Format("Fail to copy map to local sub-folder!  System ErrorCode = %d", GetLastError());
				HmiMessage_Red_Yellow(szMsg);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				szMsg = "Source path = " + szSrvFullName;
				HmiMessage_Red_Yellow(szMsg);
				SetErrorMessage(szMsg);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				szMsg = "Target path = " + szLclFullName;
				HmiMessage_Red_Yellow(szMsg);
				SetErrorMessage(szMsg);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

				return FALSE;
			}

			//	szSrvDfntName should be the same location with server map file.
			CString szAssociateFile  = (*m_psmfSRam)["MS896A"]["Associate File"];
			if( szAssociateFile.IsEmpty()==FALSE )
			{
				SetAlarmLog("Associate File Name " + szAssociateFile);
				SetAlarmLog("Server map file " + szSrvFullName);
				CString szSrvDfntName, szLclDfntName;
				if( m_szAssocFileExt.IsEmpty()==FALSE )
					szAssociateFile = szAssociateFile +  "." + m_szAssocFileExt;
				int nCol = szSrvFullName.ReverseFind('\\');
				if( nCol!=-1 )
				{
					szSrvDfntName = szSrvFullName.Left(nCol+1) + szAssociateFile;
					szLclDfntName = szLclPath + "\\" + szAssociateFile;
					SetAlarmLog("Server DEF file " + szSrvDfntName);
					SetAlarmLog("Local DEF file " +szLclDfntName );
					bFail = TRUE;
					for(int i=0; i<AUTO_LOAD_MAP_RETRY; i++)
					{
						if ( _access(szSrvDfntName, 0) != -1)
						{
							if (CopyFileWithRetry(szSrvDfntName, szLclDfntName, FALSE))
							{
								bFail = FALSE;
								break;
							}
						}
						Sleep(500);
					}
					if( bFail )
					{
						szMsg = "Fail to copy DEFINE file " + szSrvDfntName + " to local HD folder!";
						HmiMessage_Red_Yellow(szMsg);
						SetErrorMessage(szMsg);
						return FALSE;
					}
				}
			}

			if (_access(szRemoteSCNFile, 0) != -1)
			{
				bFail = TRUE;
				for(int i=0; i<AUTO_LOAD_MAP_RETRY; i++)
				{
					if (CopyFileWithRetry(szRemoteSCNFile, szLocalSCNFile, FALSE))
					{
						bFail = FALSE;
						break;
					}
					Sleep(500);
				}
				if( bFail )
				{
					szMsg = "Fail to copy SCN to local sub-folder!  " + szLocalSCNFile;
					HmiMessage_Red_Yellow(szMsg);
					SetErrorMessage(szMsg);

					return FALSE;
				}
			}

			if (_access(szRemotePSMFile, 0) != -1)
			{
				bFail = TRUE;
				for(int i=0; i<AUTO_LOAD_MAP_RETRY; i++)
				{
					if (CopyFileWithRetry(szRemotePSMFile, szLocalPSMFile, FALSE))
					{
						bFail = FALSE;
						break;
					}
					Sleep(500);
				}
				if( bFail )
				{
					szMsg = "Fail to copy PSM to local sub-folder!  " + szLocalPSMFile;
					HmiMessage_Red_Yellow(szMsg);
					SetErrorMessage(szMsg);

					return FALSE;
				}
			}

			bFail = TRUE;
			for(int i = 0; i < AUTO_LOAD_MAP_RETRY; i++)
			{
				if (_access(szLclFullName, 0) != -1)
				{
					bFail = FALSE;
					break;
				}
				Sleep(500);
			}
			if( bFail )
			{
				m_bLoadMapAccessFail = TRUE;
				m_szAccessFailFileName		= szLclFullName;
				szMsg = szLclFullName + " is not accessible";
				SetErrorMessage(szMsg);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				return FALSE;
			}

			//Start to load map file
			if( pApp->GetCustomerName()=="OSRAM" )	//	426TX	1
			{
				szLclFullName = szLclFullName + ";" + m_szOsramSDF;
			}
			else if( szAssociateFile.IsEmpty()==FALSE )
			{
				szLclFullName = szLclFullName + ";" + szAssociateFile;
				SetAlarmLog("Local load map full name " + szLclFullName);
			}
			bFail = TRUE;
			for(int i=0; i<AUTO_LOAD_MAP_RETRY; i++)
			{
				bIfLoadSuccess = AutoLoadMap(szLclFullName, "");
				if( bIfLoadSuccess )
				{
					bFail = FALSE;
					break;
				}
				Sleep(500);
			}
			if( bFail )
			{
				szMsg = "Auto load map file fail " + szLclFullName;
				HmiMessage_Red_Back(szMsg);
				SetErrorMessage(szMsg);
			}
		} 
		CATCH (CFileException, e)
		{
			HmiMessage_Red_Yellow("Exception in Load Map!!!");
		}
		END_CATCH
		CMSLogFileUtility::Instance()->WL_LogStatus("WT load map with bar code in local HD called");
	}
	else
	{
		if( (m_bEnableAutoLoadBackupMap) && bIsBackupMapLoaded )
			szSrvFullName = szNewBackupMap;					//Set temp map as load-map
		//Start to load map file
		if (_access(szSrvFullName, 0) == -1)
		{
			m_bLoadMapAccessFail = TRUE;
			m_szAccessFailFileName		= szSrvFullName;
			SetErrorMessage(szSrvFullName + " is not accessible");
			return FALSE;
		}

		if( pApp->GetCustomerName()=="OSRAM" )	//	426TX	1
		{
			szSrvFullName = szSrvFullName + ";" + m_szOsramSDF;
		}

CMSLogFileUtility::Instance()->WL_LogStatus("2.4 " + szSrvFullName);	

		bIfLoadSuccess = AutoLoadMap(szSrvFullName, "");
CMSLogFileUtility::Instance()->WL_LogStatus("2.4 DONE");	
		if ( bIfLoadSuccess )
		{
			return TRUE;
		}
	}

CMSLogFileUtility::Instance()->WL_LogStatus("3");	

	//Load Map Retry through network only
	i = 1;
	if (!bIfLoadSuccess && !m_bEnableLocalLoadMap)		//v3.15T5	//v3.23T1
	{
		//Disable re-try loadmap if offline using barcode to load map
		LONG lTimes = m_lNumOfRetry;
		LONG lDelay = m_lMapRetryDelay;

		//anichia005
		if (pApp->GetCustomerName() == "Nichia")
		{
			lTimes = CMS896AStn::m_ucCommRetry;
			lDelay = CMS896AStn::m_ulCommInterval;
		}

		while (i<=lTimes)
		{
			strTemp.Format("Loading map is failure in Retry #%d\n\n\nNext Retry will be in %d Seconds.", i, lDelay);
			HmiMessage(strTemp, "Auto-Load Map", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 1000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			Sleep(lDelay*1000);

			bIfLoadSuccess = AutoLoadMap(szSrvFullName, "");
			if ( bIfLoadSuccess ) 
				break;
			i++;
		}
	}

CMSLogFileUtility::Instance()->WL_LogStatus("4");	

	if (!bIfLoadSuccess)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Unable to load\n" + szSrvFullName + "\nPlease verify input data!");
		HmiMessage("Unable to load\n" + szSrvFullName + "\nPlease verify input data!", "Barcode load map error");
	}
		
	return bIfLoadSuccess;
}


LONG CWaferTable::LoadFixMap(IPC_CServiceMessage &svMsg)
{
	CString szMapFullName;
	BOOL bReply = TRUE;

	if ( m_bEnableFixMapName == TRUE )
	{
		szMapFullName = m_szMapFilePath + "\\" + m_szFixMapName;
		
		if (m_szMapFileExtension.IsEmpty() == FALSE)
		{
			szMapFullName = szMapFullName + "." + m_szMapFileExtension;
		}

		if (_access(szMapFullName, 0) == -1)
		{
			szMapFullName = "Unable to load\n" + m_szFixMapName + "." + m_szMapFileExtension;
			HmiMessage(szMapFullName +  "\nPlease verify input data!", "Fix name load map error");
			bReply = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}

		bReply = LoadWaferMap(szMapFullName, "");	//	fxied map

		if ( bReply == FALSE )
		{
			szMapFullName = "Unable to load\n" + m_szFixMapName + "." + m_szMapFileExtension;
			HmiMessage(szMapFullName +  "\nPlease verify input data!", "Fix name load map error");
		}

		//Always set to FALSE as stop HMI command list
		bReply = FALSE;
	}
	else
	{
		bReply = TRUE;
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="WH" )	//	not allow to load map manually
		{
			bReply = FALSE;
		}
	}
		
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

LONG CWaferTable::SaveMapFilePath(IPC_CServiceMessage &svMsg)
{
	//char *pBuffer;
	CString szMapFilePath, szInitMapPath;
	BOOL bReply = FALSE;

	/*
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szMapFilePath = &pBuffer[0];

	delete [] pBuffer;

	(*m_psmfSRam)["MS896A"]["MapFilePath"] = szMapFilePath;
	*/
	szMapFilePath = m_szMapFilePath;

	//Update wafermap init path
	szInitMapPath = szMapFilePath;
	m_WaferMapWrapper.SetInitialPath(szInitMapPath);

	SaveData();

	//v4.44A4
/*
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_OPBC_LOADPATH))
	{
		BOOL bStatus = TRUE;
		IPC_CServiceMessage stMsg;
		int	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "LoadBinLotSummaryFile2", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bStatus);
				break;
			}
			else
			{
				Sleep(10);		
			}
		}
	}
*/

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
} //end 



LONG CWaferTable::SaveMapFileExt(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szMapFileExt;
	BOOL bReply = FALSE;

	if( svMsg.GetMsgLen()>0 )	//	426TX	1
	{
		pBuffer = new char[svMsg.GetMsgLen()];
		svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

		szMapFileExt = &pBuffer[0];

		delete [] pBuffer;
	}

	(*m_psmfSRam)["MS896A"]["MapFileExt"] = szMapFileExt;

	m_szMapFileExtension = szMapFileExt;
	//Update wafermap init extension
	m_WaferMapWrapper.SetInitialExt(szMapFileExt);

	SaveData();

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end SaveMapFileExt


LONG CWaferTable::IsWaferAligned(IPC_CServiceMessage &svMsg)	//	App prestart cycle, all other stations idle.
{
	BOOL bReturn = IsAlignedWafer();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szMsg;

	SaveScanTimeEvent("    WFT: start auto cycle");

	if( IsWLAutoMode()==FALSE && m_lPresortWftStatus>=8 )//4.53D45
	{
		szMsg = "Wafer end already, please change wafer";
		HmiMessage_Red_Back(szMsg,"Wafer Table");
		SetErrorMessage(szMsg);
		bReturn = FALSE;
	}

	if( CheckEjectorPinLifeB4Start(FALSE)==FALSE )
	{
		bReturn = FALSE;
	}

	if ( IsAlignedWafer() == FALSE )
	{
		if( m_lPresortWftStatus>0 )
		{
			LONG lResult = m_lPresortWftStatus;

			if( lResult>=4 )
			{
				szMsg += "ES101 prescan finish, down back light error.\n";
				lResult -= 4;
			}
			if( lResult>=2 )
			{
				szMsg += "Scan MA1 compare check fail.\n";
				lResult -= 2;
			}
			if( lResult>=1 )
			{
				szMsg += "Find die in map empty position.\n";
				lResult -= 1;
			}
		}
		if( m_lPrescanVerifyResult>0 )
		{
			LONG lResult = m_lPrescanVerifyResult;
			if( lResult>=65536 )
			{
				lResult -= 65536;
				szMsg += "Scan edge good die pass score error.\n";
			}
			if( lResult>=32768 )
			{
				lResult -= 32768;
			}
			if( lResult>=16384 )
			{
				szMsg += "Map die position overlap.\n";
				lResult -= 16384;
			}
			if( lResult>=8192 )
			{
				szMsg += "Cree NG grade quantity error.\n";
				lResult -= 8192;
			}
			if( lResult>=4096 )
			{
				szMsg += "Fast home map index, can not find home refer die.\n";
				lResult -= 4096;
			}
			if( lResult>=2048 )
			{
				szMsg += "Check Scan wafer dimension with map failure.\n";
				lResult -= 2048;
			}
			if( lResult>=1024 )
			{
				szMsg += "Prescan neighbour die pitch over 0.5 tolerance.\n";
				lResult -= 1024;
			}
			if( lResult>=512 )
			{
				szMsg += "Grade total die increase after prescan.\n";
				lResult -= 512;
			}
			if( lResult>=GetScanErrorCode(SCAN_ERR_REALIGN_FAIL) )
			{
				szMsg += "Realignment after prescan fail.\n";
				lResult -= GetScanErrorCode(SCAN_ERR_REALIGN_FAIL);
			}
			if( lResult>=GetScanErrorCode(SCAN_ERR_NML_REFER) )
			{
				szMsg += "Refer die check fail.\n";
				lResult -= GetScanErrorCode(SCAN_ERR_NML_REFER);
			}
			if( lResult>=GetScanErrorCode(SCAN_ERR_SP_REFER) )
			{
				szMsg += "Special Refer die check fail.\n";
				lResult -= GetScanErrorCode(SCAN_ERR_SP_REFER);
			}
			if( lResult>=GetScanErrorCode(SCAN_ERR_BLOCK_REFER_CHECK) )
			{
				szMsg += "Reverify block refer die fail.\n";
				lResult -= GetScanErrorCode(SCAN_ERR_BLOCK_REFER_CHECK);
			}
			if( lResult>=GetScanErrorCode(SCAN_ERR_KEY_DIE) )
			{
				szMsg += "Set predication KEY DIE fail.\n";
				lResult -= GetScanErrorCode(SCAN_ERR_KEY_DIE);
			}
			if( lResult>=GetScanErrorCode(SCAN_ERR_PASSSCORE) )
			{
				szMsg += "Good die pass score lower than spec.\n";
				lResult -= GetScanErrorCode(SCAN_ERR_PASSSCORE);
			}
			if( lResult>=GetScanErrorCode(SCAN_ERR_RECT_CORNER) )
			{
				szMsg += "Rectangle wafer corner verify fail.\n";
				lResult -= GetScanErrorCode(SCAN_ERR_RECT_CORNER);
			}
			if( lResult>=GetScanErrorCode(SCAN_ERR_EMPTY_DIE_CHECK) )
			{
				szMsg += "Empty die check fail.\n";
				lResult -= GetScanErrorCode(SCAN_ERR_EMPTY_DIE_CHECK);
			}
			if( lResult>=GetScanErrorCode(SCAN_ERR_FPC) )
			{
				szMsg += "Five point check fail.\n";
				lResult -= GetScanErrorCode(SCAN_ERR_FPC);
			}
			HmiMessage_Red_Back(szMsg, "Prescan Check");
			SetErrorMessage(szMsg);
		}
		SetAlert_Red_Yellow(IDS_WT_MAP_NOT_ALIGN);
		SetErrorMessage("Wafer is not aligned");
	}
	else
	{
		if( IsPrescanning() && pUtl->GetPrescanBarWafer() && m_bContourEdge==FALSE && WSGetPrescanTotalDie()==0 &&
			pApp->GetCustomerName()=="emcore" && m_WaferMapWrapper.IsMapValid()  )
		{
			if( DefineAreaAndWalkPath_BarWafer()==FALSE )
				bReturn = FALSE;
		}

		if( pApp->GetCustomerName()=="OSRAM" && (IsEnablePNP()==FALSE) )
		{
			CTimeSpan stTimeDiff = CTime::GetCurrentTime() - m_stStopTime;	// press start button
			if( stTimeDiff.GetTotalMinutes()>60 )
			{
				szMsg.Format("Machine stop time (%d min) over 60 minutes!", stTimeDiff.GetTotalMinutes());
				HmiMessage_Red_Yellow(szMsg);
				SetErrorMessage(szMsg);
				bReturn = FALSE;
				SetAlignmentStatus(FALSE);
			}
		}

		if( pUtl->GetPrescanAreaPickMode() && m_lAreaPickStage==0 )
		{
			bReturn = FALSE;
			szMsg.Format("Area pick wafer finished, please load a new wafer!");
			HmiMessage_Red_Yellow(szMsg, "Area Picking");
			SetErrorMessage(szMsg);
		}

		LONG lCtrCol = BAR_MAP_CTR_COL-5;
		if( IsMS90Sorting2ndPart() )
		{
			lCtrCol = BAR_MAP_CTR_COL + 5;
		}
	}

	if( pUtl->GetPrescanRegionMode() || GetRegionSortOuter() )
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		if( pSRInfo->IsAllRegionsBonded(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(), GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol()))
		{
			szMsg = "All regions sorting complete";
			HmiMessage_Red_Back(szMsg, "Prescan Check");
			bReturn = FALSE;
			RegionPickLog(szMsg);
		}
	}

	if( IsBurnIn()==FALSE && TC_CheckAlarmOn() )
	{
		bReturn = FALSE;
		szMsg = "TC heater alarm signal triggered when start!";
		HmiMessage_Red_Yellow(szMsg);
		SetErrorMessage(szMsg);
	}

	//v4.47A5	//PLLM MS109
	if ( (pApp->GetCustomerName() == "Lumileds") && CMS896AApp::m_bMS100Plus9InchOption )
	{
		GetEncoderValue();
		LONG lEncT = m_lEnc_T;
	
		BOOL bStatus = AutoCheckWaferRotation(lEncT);	//v4.35T2

		CString szLog;
		szLog.Format("PreStart: Auto Check WT T rotation (PLLM): EncT = %ld, Status = %d", 
			m_lEnc_T, bStatus);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		
		if (!bStatus)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	//	old->new
	if( pApp->GetCustomerName()=="OSRAM" && m_bEnableSyncMove )
	{
		if( (m_dSyncSpeed==0) || (m_lSyncDistance==0) )
		{
			if( HmiMessage_Red_Back("Sync Move Settings are abnormal!\nDo you want to continue?", "Sync Move", glHMI_MBX_YESNO)==glHMI_NO )
			{
				SetErrorMessage("Sync Move settings abnormal, user stop");
				bReturn = FALSE;
			}
			else
			{
				SetErrorMessage("Sync move settins abnormal, user continue");
			}
		}
	}

	if( IsPrescanEnded() && m_ulPitchAlarmGoCheckLimit>0 && m_ulPitchAlarmGoCheckCount<m_ulPitchAlarmGoCheckLimit )
	{
		BOOL bAutoGoPass = FALSE;
		if( m_bAutoGoAroundPE )
		{
			bAutoGoPass = AutoGoPitchErrorAround();
		}
		
		if( bAutoGoPass==FALSE )
		{
			CString szMsg;
			szMsg.Format("Pitch GO check only %d points.\nSetting limit is %d!", m_ulPitchAlarmGoCheckCount, m_ulPitchAlarmGoCheckLimit);
			HmiMessage_Red_Back(szMsg, "AutSort");
			bReturn = FALSE;
		}
	}

	//	old->new
	if( IsPrescanEnable() && IsPrescanEnded() && m_ulPrescanGoCheckLimit>0 )
	{
		if( m_ulPrescanGoCheckCount<m_ulPrescanGoCheckLimit )
		{
			CString szMsg;
			szMsg.Format("Prescan GO check only %d points.\nSetting limit is %d!", m_ulPrescanGoCheckCount, m_ulPrescanGoCheckLimit);
			HmiMessage_Red_Back(szMsg, "AutSort");
			bReturn = FALSE;
		}
	}

	if( IsPrescanEnable() /*&& IsPrescanEnded()*/ && m_bChangeRegionGrade && m_ulGrade151Total != -1 )
	{
		CString szMsg;
		szMsg.Format("Region grade not changed to 151!");
		HmiMessage_Red_Back(szMsg, "AutSort");
		bReturn = FALSE;
	}

	if( IsEnableFPC() && pApp->GetCustomerName()=="TongFang" )
	{
		BOOL bNoRepeat = TRUE;
		CString szMsg, szTemp;
		szMsg = "Five Point Check Repeated\n";
		for(INT i=1; i<=m_lTotalSCNCount; i++)
		{
			for(INT j=i+1; j<=m_lTotalSCNCount; j++)
			{
				if( (m_lScnCheckRow[i]==m_lScnCheckRow[j]) &&
					(m_lScnCheckCol[i]==m_lScnCheckCol[j]) )
				{
					szTemp.Format("Point %d is same to %d (%d,%d)\n", i, j, m_lScnCheckRow[i], m_lScnCheckCol[i]);
					szMsg += szTemp;
					bNoRepeat = FALSE;
				}
			}
		}

		if( bNoRepeat==FALSE )
		{
			HmiMessage_Red_Back(szMsg, "Five Point Check");
			bReturn = FALSE;
		}
	}

	//v4.46T51	//Yealy
	//(must be yealy and ES101)
	if ( pApp->GetCustomerName() == "Yealy" && IsES101() )
	{
		//if boolean is true
		if ( m_bIsAutoLearnWaferNOTDone == TRUE )
		{
			CString szMsg = "START FAIL: Please Learn Wafer Again";
			HmiMessage_Red_Back(szMsg, "Auto Learn Wafer not Done");
			//then bReturn = flase, alarm message
			bReturn = FALSE;
		}
	}

	// here to check need to re-scan or not for prescan or rescan.
	if( IsAutoRescanEnable() && IsPrescanning() && pUtl->GetPrAbnormal() && GetNewPickCount()==0 )	//	prescan fail, pre start again, redo it
	{
		SaveScanTimeEvent("WFT: prescan fail, re-scan it again");
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

		LONG ulAlignRow, ulAlignCol = 0, lAlignX = 0, lAlignY = 0;
		pUtl->GetAlignPosition(ulAlignRow, ulAlignCol, lAlignX, lAlignY);
		m_WaferMapWrapper.SetCurrentPosition(ulAlignRow, ulAlignCol);

		// Move the table for die compensation
		XY_SafeMoveTo(lAlignX, lAlignY);
		Sleep(50);

		m_lStart_X		= lAlignX;
		m_lStart_Y		= lAlignY;
		m_lStart_T		= GetCurrT();


		ClearPrescanRawData();	// when do wafer alignment, clear data again.

		m_ulAlignRow = ulAlignRow;
		m_ulAlignCol = ulAlignCol;

		PrescanUpdateWaferAlignment(m_lStart_X, m_lStart_Y, m_ulAlignRow, m_ulAlignCol, 0);	// manual align

		(*m_psmfSRam)["MS896A"]["WaferMapRow"] = m_ulAlignRow;
		(*m_psmfSRam)["MS896A"]["WaferMapCol"] = m_ulAlignCol;
		(*m_psmfSRam)["MS896A"]["WaferTableX"] = m_lStart_X;
		(*m_psmfSRam)["MS896A"]["WaferTableY"] = m_lStart_Y;

		//if (m_bMS100EjtXY && m_bEnableMS100EjtXY)
		if (m_bEnableMS100EjtXY)	//v4.52A14
		{
			SetWTAtColletPos(0);
		}
	}	//	to represcan wafer

	m_lReadyToSampleOnWft	= 0;	//	pre start reset
	m_lReadyToCleanEjPinOnWft = 0;
	if( IsAutoRescanEnable() && (GetNewPickCount()>0) && IsAutoSampleSetupOK() )
	{
		BOOL bRescanByIdleTime = FALSE;
		CTime stStartTime = CTime::GetCurrentTime();
		UINT unLowTimeLimit = m_lMC[WTMC_DAR_IdleLowLmt];
		CTimeSpan stKeyDieSpan = stStartTime - m_stLastPickTime;
		LONGLONG lIdleTime = stKeyDieSpan.GetTotalMinutes();
		if( unLowTimeLimit>0 && lIdleTime>unLowTimeLimit )
		{
		//	szMsg.Format("WFT: rescan at start because idle time %ld over limit %d", lIdleTime, unLowTimeLimit);
		//	SaveScanTimeEvent(szMsg);
		//	bRescanByIdleTime = TRUE;
		}	// idle time over limit, do sampling. need to verify later.
		if( m_bReSampleAsError || bRescanByIdleTime )
		{
			SaveScanTimeEvent("WFT: rescan fail, re-scan it again");
			m_lReadyToSampleOnWft	= 1;	// pre start, pitch error. auto rescan wafer
			RescanSortingWafer();	//	start again after the previous rescan fail, retry.
		}
	}	// rescan wafer

	if ((m_lReadyToSampleOnWft==0) && IsAutoSampleSort() && (GetNewPickCount()>0) && IsAutoSampleSetupOK() )
	{
		if (IsAutoRescanAndSampling() || IsAutoRescanAndDEB())
		{
			if( ((GetNewPickCount()+5)>=GetNextRscnLimit()) )
			{
				m_ulNextRescanCount = GetNewPickCount()+5;
			}
		}
		else
		{
			if( ((GetNewPickCount()+5)>=GetNextXXXCounter()) )
			{
				m_ulNextAdvSampleCount = GetNewPickCount()+5;
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetMapPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szMapFilePath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetBackupMapPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szBackupMapPath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetUploadBarcodeIDPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szUploadBarcodeIDFilePath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetPrescanBackupMapPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szPrescanBackupMapPath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetPrescanSummaryPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szOutputScanSummaryPath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::ClearWaferMap(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	m_WaferMapWrapper.InitMap();
	SetAlignmentStatus(FALSE);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SetMapFileName(IPC_CServiceMessage &svMsg)
{
	LPTSTR lpsz = new TCHAR[255];

	svMsg.GetMsg(sizeof(TCHAR)*255, lpsz);
	m_szBCMapFilename = lpsz;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="HuaLei" )
	{
		m_szBCMapFilename = m_szBCMapFilename.MakeUpper();
	}

	if( pApp->GetCustomerName() == "Electech3E(DL)" )
	{
		CString szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];

		CString szFileName = m_szOutWaferInfoFilePath + "\\" + szMachineNo + "_" + m_szLotInfoFileName + ".csv";
		int nFileExist = _access(szFileName, 0);
		FILE *fp = NULL;
#if	1
		errno_t nErr = fopen_s(&fp, szFileName, "a");
		if ((nErr == 0) && (fp != NULL))
		{
			if( nFileExist==-1 )
			{
				fprintf(fp, "Lot ID,Wafer ID,Machine NO,Date,OPID\n");
			}
			CTime stTime = CTime::GetCurrentTime();
			CString szTime;	//	 = stTime.Format("%Y/%m/%d %H:%M");	//		2015/7/3 9:20
			szTime.Format("%d/%d/%d %d:%d", stTime.GetYear(), stTime.GetMonth(), stTime.GetDay(),
				stTime.GetHour(), stTime.GetMinute());
			fprintf(fp, "%s,%s,%s,%s,%s\n", (LPCTSTR) m_szLotInfoFileName, (LPCTSTR) m_szBCMapFilename,
								(LPCTSTR) szMachineNo, (LPCTSTR) szTime, (LPCTSTR) m_szLotInfoOpID);
			fclose(fp);
		}
#else
		if( nFileExist==-1 )
		{
			fp = fopen(szFileName, "w");
			if( fp!=NULL )
			{
				fprintf(fp, "Lot ID,Machine NO,Date,OPID\n");
				CTime stTime = CTime::GetCurrentTime();
				CString szTime;
				szTime.Format("%d/%d/%d %d:%d", stTime.GetYear(), stTime.GetMonth(), stTime.GetDay(),
					stTime.GetHour(), stTime.GetMinute());
				fprintf(fp, "%s,%s,%s,%s\n", m_szLotInfoFileName, szMachineNo, szTime, m_szLotInfoOpID);
				fclose(fp);
			}
		}
#endif
	}
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	delete[] lpsz;

	return 1;
}


//Teach wafer unload position
LONG CWaferTable::MoveToUnload(IPC_CServiceMessage& svMsg)
{
	BOOL bUnload	= TRUE;
	BOOL bReturn	= TRUE;

	BOOL bOpen = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOpen);

	if (!m_fHardware || m_bDisableWT)
	{		
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//svMsg.GetMsg(sizeof(BOOL), &bUnload);
	if (!X_IsPowerOn() || !Y_IsPowerOn())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (bOpen)
	{
	//if (IsWithinWaferLimit(m_lUnloadPhyPosX, m_lUnloadPhyPosY) == TRUE)
	//{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
		
		//XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
		X_MoveTo(m_lUnloadPhyPosX);
		Y_MoveTo(m_lUnloadPhyPosY);
		
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
	//}
	}
	else
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);

		//XY1_MoveTo(0, 0, SFM_WAIT);
		Y_MoveTo(0, SFM_NOWAIT);
		Sleep(50);
		X_MoveTo(0, SFM_NOWAIT);
		Y_Sync();
		X_Sync();

		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
		//Y_Home();
		//X_Home();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

	/*
	if (IsWLExpanderOpen())
	{
		SetErrorMessage("Expander is opened in MoveToUnload");
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	if (bUnload == TRUE)
	{
		SetJoystickOn(FALSE);
		GetEncoderValue();
		m_lCurrent_X = GetCurrX1();
		m_lCurrent_Y = GetCurrY1();
		if( IsEnableWL() )
		{
			if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.35T3	//MS109
			{
				X1_Profile(LOW_PROF1);
				Y1_Profile(LOW_PROF1);

				if (XY1_MoveTo(0, 0, SFM_WAIT) == gnOK)	
				{
					SetEJTSlideUpDn(FALSE);
					Sleep(500);

					//szLog.Format("WT: XY MoveToUnloadCmd to UNLOAD at %ld, %ld", stPos.lX, stPos.lY);
					//CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
					XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
				}

				X1_Profile(NORMAL_PROF);
				Y1_Profile(NORMAL_PROF);
			}
			else
			{
				XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
			}
		}
		else
		{
			XY1_MoveTo(m_lWaferUnloadX, m_lWaferUnloadY, SFM_WAIT);
		}
	}
	else
	{
		SetJoystickOn(FALSE);
		if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.35T3	//MS109
		{
			X1_Profile(LOW_PROF1);
			Y1_Profile(LOW_PROF1);

			if (XY1_MoveTo(0, 0, SFM_WAIT) == gnOK)	
			{
				SetEJTSlideUpDn(TRUE);
				Sleep(500);
				
				//v4.43T13
				INT nCount = 0;
				while (IsEJTAtUnloadPosn())
				{
					Sleep(300);
					nCount++;
					if (nCount > 10)
					{
						//HmiMessage_Red_Yellow("EJT Table detected at UNLOAD position; operation si aborted.");
						SetAlert_Red_Yellow(IDS_WT_EJT_NOT_SAFE);		//v4.59A22	//40035

						X1_Profile(NORMAL_PROF);
						Y1_Profile(NORMAL_PROF);
						svMsg.InitMessage(sizeof(BOOL), &bReturn);
						return TRUE;
					}
				}

				XY1_MoveTo(m_lCurrent_X, m_lCurrent_Y, SFM_WAIT);
			}

			X1_Profile(NORMAL_PROF);
			Y1_Profile(NORMAL_PROF);
		}
		else
		{
			XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);
		}
		SetJoystickOn(TRUE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
	*/
}


LONG CWaferTable::SetUnloadPosition(IPC_CServiceMessage& svMsg)
{
	if (!m_fHardware || m_bDisableWT)	//v3.61
	{	
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//SetJoystickOn(FALSE);
	GetEncoderValue();

	m_lUnloadPhyPosX	= m_lEnc_X;
	m_lUnloadPhyPosY	= m_lEnc_Y;

	SaveData();
	
	HmiMessage("WT UNLOAD XY is updated; table will move back to HOME.");

	Y_Home();
	X_Home();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;

	/*
	LONG lUnloadPosX, lUnloadPosY;
	BOOL bStartFromHome = FALSE;

	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, WT_JOY_MAXLIMIT_NEG_X, WT_JOY_MAXLIMIT_POS_X, &m_stWTAxis_X);		
	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, WT_JOY_MAXLIMIT_NEG_Y, WT_JOY_MAXLIMIT_POS_Y, &m_stWTAxis_Y);

	INT nReturn = gnOK;
	
#ifdef NU_MOTION
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
#endif
	
	if (bStartFromHome == TRUE)
	{
		lUnloadPosX = 0;
		lUnloadPosY = 0;
	}
	else
	{
		if( IsEnableWL() )
		{
			lUnloadPosX = m_lUnloadPhyPosX;
			lUnloadPosY = m_lUnloadPhyPosY;
		}
		else
		{
			lUnloadPosX = m_lWaferUnloadX;
			lUnloadPosY = m_lWaferUnloadY;
		}
	}

	nReturn = XY1_MoveTo(lUnloadPosX, lUnloadPosY, SFM_WAIT);

#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	SetJoystickOn(TRUE);

	BOOL bReturn=TRUE;
	if (nReturn != gnOK)
		bReturn = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
	*/
}


LONG CWaferTable::ConfirmUnloadPosition(IPC_CServiceMessage& svMsg)
{
	LONG lThetaPosition = 0;
	BOOL bReturn=TRUE;

	if (!m_fHardware || m_bDisableWT)	//v3.61
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	try
	{
		GetEncoderValue();

		m_lWaferUnloadX = GetCurrX1();
		m_lWaferUnloadY = GetCurrY1();

		SaveWaferTblData();


		//Restore original limit 
		SetJoystickOn(FALSE);
		
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);	

		XY_SafeMoveTo(GetWft1CenterX(), GetWft1CenterY());

		SetJoystickOn(TRUE);

		SetStatusMessage("Wafer unload position is set");

        // 3431
        SetGemValue("WT_WaferUnloadX", m_lWaferUnloadX);
        SetGemValue("WT_WaferUnloadY", m_lWaferUnloadY);
        // 7502
        SendEvent(SG_CEID_WT_UNLOADPOSN, FALSE);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
		bReturn = FALSE;
	}	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferTable::MoveToUnloadLoad(IPC_CServiceMessage& svMsg)
{
	//v2.78T1
	if (IsEnableWL())
	{
		if ( IsWL1ExpanderSafeToMove() == FALSE )
		{
			BOOL bReturn=TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		if ( IsWL2ExpanderSafeToMove() == FALSE )
		{
			BOOL bReturn=TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	StartLoadingAlert();	// Show a alert box

#ifdef NU_MOTION
	X1_Profile(LOW_PROF);
	Y1_Profile(LOW_PROF);
#endif


	CMSLogFileUtility::Instance()->WL_LogStatus("WFT Move to unload load Home T");
	T_Home(FALSE, FALSE);
	INT nReturn = gnOK;
	
	//v4.26T1	//Walsin China
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableWaferClamp = pApp->GetFeatureStatus(MS896A_FUNC_SO_WAFER_CLAMP);

	if (m_bIsUnload == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WFT Move to unload position");
		if( IsEnableWL() )
		{
			if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.34
			{
				X1_Profile(LOW_PROF1);
				Y1_Profile(LOW_PROF1);

				if (XY1_MoveTo(0, 0, SFM_WAIT) == gnOK)	
				{
					SetEJTSlideUpDn(FALSE);
					Sleep(500);

					//szLog.Format("WT: XY MoveToUnloadCmd to UNLOAD at %ld, %ld", stPos.lX, stPos.lY);
					//CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
					nReturn = XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
				}
				else
					nReturn = FALSE;

				X1_Profile(NORMAL_PROF);
				Y1_Profile(NORMAL_PROF);
			}
			else
			{
				nReturn = XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
			}
		}
		else
		{
			nReturn = XY1_MoveTo(m_lWaferUnloadX, m_lWaferUnloadY, SFM_WAIT);
		}

		//v4.26T1
		if (bEnableWaferClamp && m_bDisableWL)	//!bWaferLoader)		//Only available for Walsin MS810EL-90
		{
			Sleep(500);
			SetWaferClamp(FALSE);
		}

		m_bIsUnload = TRUE;
	}
	else
	{
		//v4.26T1
		if (bEnableWaferClamp && m_bDisableWL)	// !bWaferLoader)		//Only available for Walsin MS810EL-90
		{
			SetWaferClamp(TRUE);
			Sleep(500);
		}

		CMSLogFileUtility::Instance()->WL_LogStatus("WFT Move to home position");

		if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.34
		{
			X1_Profile(LOW_PROF1);
			Y1_Profile(LOW_PROF1);
			XY1_MoveTo(0, 0, SFM_WAIT);
			X1_Profile(NORMAL_PROF);
			Y1_Profile(NORMAL_PROF);
			SetEJTSlideUpDn(TRUE);
		}
		else
		{
			XY_SafeMoveTo(0, 0);
		}

		m_bIsUnload = FALSE;
	}

#ifdef NU_MOTION
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
#endif

	CloseLoadingAlert();	// Show a alert box

	BOOL bReturn = TRUE;
	if (nReturn != gnOK)	//Klocwork	//v4.24T11
		bReturn = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	CMSLogFileUtility::Instance()->WL_LogStatus("WFT Move to loadunload complete");

	return TRUE;
}


LONG CWaferTable::PreStartCheckGlobalTheta(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( (pApp->GetCustomerName()==CTM_SANAN   && pApp ->GetProductLine() == "XA") ||
		(pApp->GetCustomerName()=="Semitek" && pApp ->GetProductLine() == "ZJG") )
	{
		GetEncoderValue();
		LONG lCmdT = CMS896AStn::MotionGetCommandPosition(WT_AXIS_T, &m_stWTAxis_T);
		if( IsESDualWT()==FALSE && labs(lCmdT-GetCurrT())>5 )
		{
			CString szErr;
			szErr.Format("AUTOBOND: WT T encoder error:  %d (%d), Please home T", lCmdT, GetCurrT());
			SetErrorMessage(szErr);	
			HmiMessage_Red_Yellow(szErr);
			bReturn = FALSE;
		}
	}	//	SanAn special with tight check

	T_MoveTo(GetGlobalT(), SFM_WAIT);
	Sleep(100);
	GetEncoderValue();

	//v4.34T10
	if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.16T3	//v4.20
	{
		if (abs(GetCurrT() - GetGlobalT()) > 200)
		{
			CString szErr;
			szErr.Format("AUTOBOND: WT T encoder error:  %d (%d)", GetGlobalT(), GetCurrT());
			SetErrorMessage(szErr);	
			HmiMessage_Red_Yellow(szErr);
			bReturn = FALSE;
		}
	}
	else if( pApp->GetCustomerName()==CTM_SANAN && pApp ->GetProductLine() == "XA" )
	{
		if (abs(GetCurrT() - GetGlobalT()) > 5)
		{
			CString szErr;
			szErr.Format("AUTOBOND: WT T encoder error:  %d (%d)", GetGlobalT(), GetCurrT());
			SetErrorMessage(szErr);	
			HmiMessage_Red_Yellow(szErr);
			bReturn = FALSE;
		}
	}	//	SanAn special with tight check
	else
	{
		if (abs(GetCurrT() - GetGlobalT()) > 100 && IsBurnIn()==FALSE )
		{
			CString szErr;
			szErr.Format("AUTOBOND: WT T encoder error:  %d (%d)", GetGlobalT(), GetCurrT());
			SetErrorMessage(szErr);	
			HmiMessage_Red_Yellow(szErr);
			bReturn = FALSE;
		}
	}

	//v4.59A36
	if (IsMS90())
	{
		GetWaferSortPart();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferTable::MoveWaferTableToSafePosn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSafe = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bSafe);
	
	if (m_bDisableWT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	// no need to move wafer table if it less than 6"
	if( Is180Arm6InchWT()==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( bSafe )
        WftToSafeLog("6 inch wft move to safe position");
	else
		WftToSafeLog("6 inch wft move to original position");

	//v3.70T3
	if (XY_IsPowerOff())
	{
		HmiMessage_Red_Yellow("ERROR: wafer table motor not power on!");
		SetErrorMessage("Expander not closed 31");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//v3.70T3	//v4.47T1	//Cree HuiZhou MS899
	if (IsEnableWL() && IsWLExpanderOpen())
	{
		SetErrorMessage("Expander not closed 32");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);
	MoveWftToSafePosn(bSafe);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::MoveWftToSafePosn(BOOL bSafe)
{
	X_Sync();
	Y_Sync();
	T_Sync();

	if (bSafe)
	{
		GetEncoder(&m_lBpX, &m_lBpY, &m_lBpT);
		CString szMsg;
		szMsg.Format("6 inch wft to safe posn, current %d,%d", m_lBpX, m_lBpY);
		WftToSafeLog(szMsg);
		if (m_lBpY < -150000)	//v3.70T3	-170000
		{
			XY_SafeMoveTo(0, 0);
			Sleep(100);
		}
	}
	else
	{
		CString szMsg;
		szMsg.Format("6 inch wft to safe posn, move back, original %d,%d", m_lBpX, m_lBpY);
		WftToSafeLog(szMsg);
		if (XY_SafeMoveTo(m_lBpX, m_lBpY))
		{
			Sleep(200);
		}
	}

	return 1;
}

LONG CWaferTable::UpdatePitchTolerance(IPC_CServiceMessage& svMsg)
{
	if ( m_lPredScanPitchTolX < 0 )
		m_lPredScanPitchTolX = 0;

	if ( m_lPredScanPitchTolY < 0 )
		m_lPredScanPitchTolY = 0;

	if ( m_lPitchX_Tolerance < 0 )
		m_lPitchX_Tolerance = 0;
	if ( m_lPitchX_Tolerance > 100 )
		m_lPitchX_Tolerance = 100;

	if ( m_lPitchY_Tolerance < 0 )
		m_lPitchY_Tolerance = 0;
	if ( m_lPitchY_Tolerance > 100 )
		m_lPitchY_Tolerance = 100;

	m_lRunPredScanPitchTolX		= m_lPredScanPitchTolX;
	m_lRunPredScanPitchTolY		= m_lPredScanPitchTolY;
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::UpdateAction(IPC_CServiceMessage& svMsg)
{
	Sleep(500);
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::LogItems(IPC_CServiceMessage& svMsg)
{
	LONG lEventNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lEventNo);

	LogItems(lEventNo);

	return 1;
}

LONG CWaferTable::UpdateData(IPC_CServiceMessage& svMsg)
{
	SaveData();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	try
	{
		// 3446
		SetGemValue("WL_WaferOrientation", m_ucMapRotation);
		// 3447
		SetGemValue("AB_bCrossCheck", 0);
		SetGemValue("AB_CrossCheckPosErrorX", 0);
		SetGemValue("AB_CrossCheckPosErrorY", 0);
		// 3448
		SetGemValue("AB_bDiePitchCheck", m_bDiePitchCheck);
		SetGemValue("WPR_lPitchCheckX", m_lPitchX_Tolerance);
		SetGemValue("WPR_lPitchCheckY", m_lPitchY_Tolerance);
		// 3449
		SetGemValue("AB_RefCrossWidth", m_lRefCrossWidth);
		SetGemValue("AB_RefCrossRow", m_lRefCrossRow);
		SetGemValue("AB_RefCrossCol", m_lRefCrossCol);
		// 3450
		SetGemValue("AB_bIgnoreRegion", m_bIgnoreRegion);
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		SetGemValue("AB_SubRegionRow", pSRInfo->m_ulSubRows);
		SetGemValue("AB_SubRegionCol", pSRInfo->m_ulSubCols);
		// 7505
		SendEvent(SG_CEID_WT_AUTOBOND, FALSE);

		SendCE_ScanSetting();

		// 3436
		SetGemValue("AB_BackUpMapPath", m_szBackupMapPath);
		SetGemValue("AB_BackUpMapExt", m_szBackupMapExtension);
		// 7503
		SendEvent(SG_CEID_WT_BACKUPMAP, FALSE);
	}
	catch(...)
	{
		DisplaySequence("CWaferTable::UpdateData failure");
	}

	return 1;
}

LONG CWaferTable::UpdateRefCrossInfo(IPC_CServiceMessage& svMsg)
{
	m_WaferMapWrapper.ResetGrade();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//4.54T21
LONG CWaferTable::UpdateGradeDisplayMode(IPC_CServiceMessage& svMsg)
{
	LONG lGradeDisplayMode = 3;
	svMsg.GetMsg(sizeof(LONG), &lGradeDisplayMode);	
	
	switch(lGradeDisplayMode)
	{
		case 0:		
			m_szGradeDisplayMode = "Sequential";
			break;

		case 1:		
			m_szGradeDisplayMode = "ASCII";
			break;

		case 2:		
			m_szGradeDisplayMode = "Character";
			break;


		case 3:		
			m_szGradeDisplayMode = "Original";
			break;


		case 4:		
			m_szGradeDisplayMode = "Char+ASCII";
			break;


		case 5:		
			m_szGradeDisplayMode = "HEX";
			break;

		case 6:		
			m_szGradeDisplayMode = "Original Only";
			break;

		default:
			m_szGradeDisplayMode = "Original";
			break;
	}

	SaveData();
	
	//m_lGradeDisplayMode
	//m_WaferMapWrapper.ResetGrade();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SetWaferGlobalAnlge(IPC_CServiceMessage& svMsg)
{
	Sleep(50);
	GetEncoderValue();
	SetGlobalTheta();
	GetPrescanDiePitch();	// set gt by ipc command

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::UpdateSubRegionInfo(IPC_CServiceMessage& svMsg)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	SetupSubRegionMode();	// setu sub region row/col
	SaveData();

	if( pUtl->GetPrescanRegionMode() || GetRegionSortOuter() )
	{
		HmiMessage("You need to RELOAD MAP file after this change!");
		m_WaferMapWrapper.InitMap();
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::UpdateIgnoreRegionInfo(IPC_CServiceMessage& svMsg)
{
	// TRUE = selected region will no pick;
	m_WaferMapWrapper.SetDieSelectMode(m_bIgnoreRegion);
	SaveData();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::UpdateSelectRegion(IPC_CServiceMessage &svMsg)
{
	LONG lCurrRegion = 1;
	svMsg.GetMsg(sizeof(LONG), &lCurrRegion);
	m_lCurrentSelectRegion = lCurrRegion;

	if (m_lCurrentSelectRegion < 1)
		m_lCurrentSelectRegion = 1;
	if (m_lCurrentSelectRegion > (WT_MAX_SELECT_REGION-1))
		m_lCurrentSelectRegion = WT_MAX_SELECT_REGION - 1;

	m_ulSelectRegionULX[0] = m_ulSelectRegionULX[m_lCurrentSelectRegion];
	m_ulSelectRegionULY[0] = m_ulSelectRegionULY[m_lCurrentSelectRegion];
	m_ulSelectRegionLRX[0] = m_ulSelectRegionLRX[m_lCurrentSelectRegion];
	m_ulSelectRegionLRY[0] = m_ulSelectRegionLRY[m_lCurrentSelectRegion];

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::VerifySelectRegion(IPC_CServiceMessage &svMsg)		//3.49T5
{
	CString szMsg;

	ULONG ulNumOfRows = 0, ulNumOfCols = 0;
	m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols);

	LONG ulUserRow1 = 0, ulUserCol1 = 0;
	LONG ulUserRow2 = 0, ulUserCol2 = 0;
	m_WaferMapWrapper.ConvertInternalToUser(0, 0, ulUserRow1, ulUserCol1);
	m_WaferMapWrapper.ConvertInternalToUser(ulNumOfRows-1, ulNumOfCols-1, ulUserRow2, ulUserCol2);
	
	if (m_ulSelectRegionULX[0] < ulUserCol1)
		m_ulSelectRegionULX[0] = ulUserCol1;
	if (m_ulSelectRegionULY[0] < ulUserRow1)
		m_ulSelectRegionULY[0] = ulUserRow1;
	
	if (m_ulSelectRegionLRX[0] > ulUserCol2)
		m_ulSelectRegionLRX[0] = ulUserCol2;
	if (m_ulSelectRegionLRY[0] > ulUserRow2)
		m_ulSelectRegionLRY[0] = ulUserRow2;

	m_ulSelectRegionULX[m_lCurrentSelectRegion] = m_ulSelectRegionULX[0];
	m_ulSelectRegionULY[m_lCurrentSelectRegion] = m_ulSelectRegionULY[0];
	m_ulSelectRegionLRX[m_lCurrentSelectRegion] = m_ulSelectRegionLRX[0];
	m_ulSelectRegionLRY[m_lCurrentSelectRegion] = m_ulSelectRegionLRY[0];

	SaveData();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::AlignInputWafer(IPC_CServiceMessage& svMsg) // for wafer loader in auto mode
{
	BOOL	bResult;
	ULONG	ulRow, ulCol;
	LONG	lX, lY, lT;
	BOOL	bFullAuto;

	svMsg.GetMsg(sizeof(BOOL), &bFullAuto);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsMS90HalfSortDummyScanAlign() )
	{
	}
	else
	{
		CreateDummyMapForPrescanPicknPlace();	//	none AOI when align input wafer
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("to align input wafer");
	SetJoystickOn(FALSE);

	CreeAutoSearchMapLimit();	// wafer loader in auto mode before block pick alignment

	SetAlignmentStatus(FALSE);
	ResetMapStauts();	//	426TX	2
	if (IsEnablePNP() == TRUE)
	{
		UpdatePickAndPlacePos(svMsg);
		bResult = TRUE;
		SetAlignmentStatus(TRUE);
		CMSLogFileUtility::Instance()->MS_LogOperation("Pick And Place Fully-AutoAlign Wafer completed");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	if (CMS896AApp::m_bEnableGripperLoadUnloadTest == TRUE)
	{
		GetEncoder(&lX, &lY, &lT);
		m_lStart_X	= lX;
		m_lStart_Y	= lY;
		
		SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow, m_ulAlignCol);
		m_WaferMapWrapper.SetStartPosition(m_ulAlignRow, m_ulAlignCol);
		m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
		SetAlignmentStatus(TRUE);

		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//v3.55
	CString szLog;
	szLog.Format("Fully-AutoAlign Wafer: BLKPICK=%d", IsBlkFuncEnable());
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

	if( IsPrescanBlkPick()==FALSE && pUtl->GetPrescanRegionMode()==FALSE && m_bFindAllRefer4Scan && m_bFindHomeDieFromMap )
	{
		if( FindCrossReferDieAlignWafer()==FALSE )
		{
			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return TRUE;
		}
	}

	GetMapAlignHomeDie(ulRow, ulCol);
	BOOL bSAFromMap = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["ScanAlignAutoFromMap"];

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
/*
	if( IsScanAlignWafer() && !pSRInfo->IsManualAlignRegion() && bSAFromMap && m_bSingleHomeRegionScan==FALSE )
	{
		if( pUtl->GetPrescanRegionMode()==FALSE )
		{
			ulRow = (GetMapValidMinRow() + GetMapValidMaxRow())/2;
			ulCol = (GetMapValidMinCol() + GetMapValidMaxCol())/2;
		}
	}
*/
	if( (pApp->GetCustomerName()=="OSRAM") && IsBlkFunc2Enable() && IsCharDieInUse() && bFullAuto==FALSE )	// Manual align
	{
		m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);
	}

	if( IsOcrAOIMode() && IsCharDieInUse() )
	{
		LONG lOffsetX = 0, lOffsetY = 0;
		CString szOcrPrValue = "", szMsg = "";
		// after find, do the confirmation again, make sure it is a valid OCR die
		if( AOISearchOcrDie(lOffsetX, lOffsetY, szOcrPrValue, szMsg)==FALSE )
		{
			HmiMessage_Red_Back(szMsg, "AOI OCR Die");
			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}

		LONG lUserRow = (szOcrPrValue.GetAt(2) - 'A')*10 + szOcrPrValue.GetAt(3) - '0';
		LONG lUserCol = (szOcrPrValue.GetAt(4) - 'A')*10 + szOcrPrValue.GetAt(5) - '0';
		ConvertOrgUserToAsm(lUserRow, lUserCol, ulRow, ulCol);
	//	m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
	//	m_WaferMapWrapper.SetCurrentPosition(ulRow,  ulCol);
	}

	m_ulAlignRow = ulRow;
	m_ulAlignCol = ulCol;

	X_Sync();
	Y_Sync();
	T_Sync();
	GetEncoder(&lX, &lY, &lT);

	if( pApp->GetCustomerName()=="Lumileds" && (m_ucPLLMWaferAlignOption == 2) )	//v4.53A22
	{
		if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol)==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL home die invalid");
			HmiMessage_Red_Back("Home die is invalid, please manually align wafer!", "Auto Align Wafer");
			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return TRUE;
		}
	}

	if ( IsBlkFunc2Enable() || IsPrescanBlkPick() )	//Block2
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL blk func2");
		LONG lAlarmStatus = GetAlarmLamp_Status();

		bResult = FALSE;
	
		//Set SECSGEM Value
		LONG lSlotNo = 1;
		try
		{
			SetGemValue(MS_SECS_SV_WAFER_ID, m_szWaferId);		//3003
			SetGemValue(MS_SECS_SV_SLOT_NO, lSlotNo);			//3004
			SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");		//3005
			SendEvent(SG_CEID_StartAlignment, TRUE);
		}
		catch(...)
		{
			DisplaySequence("SG_CEID_StartAlignment failure");
		}

		m_WaferMapWrapper.EnableSequenceCheck(FALSE);
		SaveBlkData();
		bResult = Blk2FindAllReferDiePosn();
		if( bResult==FALSE )
		{
			m_WaferMapWrapper.DeleteAllBoundary();
		}

		SetAlarmLamp_Back(lAlarmStatus, FALSE, TRUE);
		SetAlarmLog("Blk2 auto align complete");

		SetAlignmentStatus(bResult);

		if( bResult==TRUE )
		{
			SetPrescanAutoAlignMap();

			if( bFullAuto )
				m_pBlkFunc2->Blk2GetStartDie(&m_lBlkHomeRow, &m_lBlkHomeCol, &m_lBlkHomeX, &m_lBlkHomeY);
			else
				m_pBlkFunc2->Blk2GetStartDie(&m_lBlkHomeRow, &m_lBlkHomeCol, &m_lBlkHomeX, &m_lBlkHomeY);
			//Return to Align position
			SetAlignStartPoint(FALSE, m_lBlkHomeX, m_lBlkHomeY, m_lBlkHomeRow, m_lBlkHomeCol);

			//Update Wafertable & wafer map start position
			m_lStart_X	= m_lBlkHomeX;
			m_lStart_Y	= m_lBlkHomeY;
		}
	}
	else
	{
		//Check SCN file is loaded or not
		if ( CheckSCNFileLoaded() == FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL check SCN loaded false");
			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return TRUE;
		}

		bResult = FALSE;
		if (IsBlkFunc1Enable())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL blk func enable");
			m_WaferMapWrapper.EnableSequenceCheck(FALSE);
			SaveBlkData();
			Blk1SetAlignParameter();
			if ( IsCharDieInUse() )
			{
				bResult = m_pBlkFunc->FoundAllCharDiePos();
			}
			else
			{
				bResult = m_pBlkFunc->FoundAllRefDiePos();
			}
		}
		else if (IsScnLoaded())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL scn loaded");
			//Index to Offset Pos if need
			m_lCurrent_X = lX;
			m_lCurrent_Y = lY;
			IndexToSCNOffsetPos(&m_ulAlignRow, &m_ulAlignCol);
			GetEncoder(&lX, &lY, &lT);

			m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);	
			SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow, m_ulAlignCol);
			m_WaferMapWrapper.EnableAutoAlign(!m_bStartPoint);

			m_ulCurrentRow = m_ulAlignRow;
			m_ulCurrentCol = m_ulAlignCol;
			m_lCurrent_X = lX;
			m_lCurrent_Y = lY;

			bResult = TRUE;

			//Return to Align position	
			SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow, m_ulAlignCol);

			//Update Wafertable & wafer map start position
			m_lStart_X	= lX;
			m_lStart_Y	= lY;
			m_WaferMapWrapper.SetStartPosition(m_ulAlignRow, m_ulAlignCol); 
		}
		else
		{	
			ULONG ulCurrAlignRow = m_ulAlignRow;
			ULONG ulCurrAlignCol = m_ulAlignCol;

			//v4.08		// Triangular die wafermap support
			if( IsSorraSortMode() )
			{
				CString strAlgorithm, strPathFinder;
				m_WaferMapWrapper.GetAlgorithm(strAlgorithm, strPathFinder);
				m_WaferMapWrapper.SelectAlgorithm("TLH-Tri", strPathFinder);
				m_WaferMapWrapper.SetDieStep(2);
				m_WaferMapWrapper.SetAlgorithmParameter("Reverse Shape Dir", FALSE);		

				m_b2PartsSortAutoAlignWay	= TRUE;		//FUlly-Auto mode must set this to TRUE

				BOOL bIsFindMapDnDie = TRUE;
				MoveSoraaMapByOneDieUpDnFromHomeDie(bIsFindMapDnDie);
				GetEncoder(&lX, &lY, &lT);

				//Use the Good-die below Ref(0,0) (i.e.GD at (-1,0)) to align "0-degree" map
				if (bIsFindMapDnDie)
				{
					//Mark DN-die from HOME die (0,0) as MAP ALIGN point
					m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow+1, m_ulAlignCol);
					SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow+1, m_ulAlignCol);
				}
				else
				{
					//mark UP-die as MAP ALIGN point
					m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow-1, m_ulAlignCol);
					SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow-1, m_ulAlignCol);
				}
			}
			else if( IsMS90HalfSortMode()  )	// fully auto align input wafer
			{
				if( IsBlkPickAlign() )
				{
					if( BlockPickAlignHalfWafer()==FALSE )
					{
						bResult = FALSE;
						svMsg.InitMessage(sizeof(BOOL), &bResult);
						return 1;
					}
					X_Sync();
					Y_Sync();
					T_Sync();
					GetEncoder(&lX, &lY, &lT);
				}
				m_b2PartsSortAutoAlignWay	= TRUE;		//FUlly-Auto mode must set this to TRUE
				m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
				SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow, m_ulAlignCol);
			}
			else
			{
				m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
				SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow, m_ulAlignCol);
			}

			SetPrescanAutoAlignMap();

			m_lStart_X	= lX;
			m_lStart_Y	= lY;

			//v4.46T25
			if (pApp->GetCustomerName() == "Lumileds"	&& 
				pApp->GetProductLine()	== "Rebel"		&& 
				(m_ucPLLMWaferAlignOption > 0))			
			{
				//if (m_ucPLLMWaferAlignOption == 2)			//REBEL U2U wafer
				//{
				//	bResult = AlignU2UReferenceDie();
				//}
				//else if (m_ucPLLMWaferAlignOption == 1)		//REBEL Tile Wafer
				//{
				LONG lStatus = AlignRectWafer(m_ulCornerSearchOption);
				if ( lStatus == WT_ALN_OK )
				{
					bResult = TRUE;
				}
				//}
			}
			else if ( m_bSearchHomeOption == WT_SPIRAL_SEARCH )
			{
				if (pApp->GetCustomerName() == "Lumileds" && pApp->GetProductLine() == "Rebel")
				{
					bResult = AlignU2UReferenceDie();
				}
				else if (pApp->GetCustomerName() == "IsoFoton")
				{
					bResult = AlignAllRefDiePositions();
				}
				else if ((pApp->GetCustomerName() == "Cree") && m_bEnableTwoDimensionsBarcode)	//v4.08
				{
					//v4.10T3
					LONG lStatus = AlignRectWafer();
					if ( lStatus == WT_ALN_OK )
					{
						bResult = TRUE;
					}
					//if (TwoDimensionsBarcodeChecking())
					//{
					//	bResult = TRUE;
					//}
				}
				else if (CheckCreeHorz4RefDice())				//v3.82		//Cree US Waffle Pack Buyoff
				{
					LONG lEncX = lX;
					LONG lEncY = lY;
					if (FindLeftmostCreeHorz4RefDice(lEncX, lEncY))		//v3.92
					{
						m_lStart_X	= lX = lEncX;
						m_lStart_Y	= lY = lEncY;
					}

					bResult = AlignAllRefDiePositions();
				}
				else
				{
					bResult = TRUE;
				}
			}
			else
			{
				// hard code for test
				LONG lStatus = AlignRectWafer(m_ulCornerSearchOption);

				if ( lStatus == WT_ALN_OK )
				{
					bResult = TRUE;
				}
			}

			SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow, m_ulAlignCol);
			if( m_lBuildShortPathInScanning==0 )
				m_WaferMapWrapper.SetStartPosition(m_ulAlignRow, m_ulAlignCol);
			m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);

			if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
			{
				m_WaferMapWrapper.EnableAutoAlign(TRUE);		//pllm
			}
		}
		if( bResult )
		{
			bResult = CheckTekCoreAutoAlignResult(m_ulAlignRow, m_ulAlignCol);
		}

		SetAlignmentStatus(bResult);
	}

	if( bResult )
	{
		SetCtmRefDieOptions();	
		if (m_bFullRefBlock == FALSE || pUtl->GetPrescanRegionMode() )	//Block2
		{
			if( pUtl->GetPrescanDummyMap()==FALSE )
			{
				SetupSubRegionMode();		// manual align button
			}
		}

		if (PrescanUpdateWaferAlignment(lX, lY, m_ulAlignRow, m_ulAlignCol, 2))	// fully auto
		{
			//================================update again ===========================
			//2018.7.27
			pSRInfo->FindFirstPickingRegion(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(), GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol());
			//ULONG ulRegionNo = pSRInfo->GetWithinRegionNo(m_ulAlignRow, m_ulAlignCol);
			//pSRInfo->SetTargetRegion(ulRegionNo);
			SaveRegionStateAndIndex();
			//========================================================================
		}

		if( IsAlignedWafer()==FALSE )
			bResult = FALSE;
		if( m_bEnableAlignWaferImageLog )
		{
			(*m_psmfSRam)["MS896A"]["WaferMapRow"] = m_ulAlignRow;
			(*m_psmfSRam)["MS896A"]["WaferMapCol"] = m_ulAlignCol;
			(*m_psmfSRam)["MS896A"]["WaferTableX"] = lX;
			(*m_psmfSRam)["MS896A"]["WaferTableY"] = lY;
			IPC_CServiceMessage stMsg;
			int	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchAndSaveReferDieImages", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
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

	if ( IsBlkFunc2Enable() )	//Block2
	{
		m_WaferMapWrapper.EnableSequenceCheck(FALSE);	// for block pick align, it is must
		m_pBlkFunc2->VirtualAlignBlockWafer();
		Sleep(500);
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("WL Fully-AutoAlign Wafer completed");
	CMSLogFileUtility::Instance()->MS_LogOperation("WL Fully-AutoAlign Wafer completed");

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}


LONG CWaferTable::CheckRepeatMap(IPC_CServiceMessage& svMsg)				//v3.74T33
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (!pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))		//If not PLLM REBEL/FLASH, no need to check
	{
		//SetErrorMessage("WT: check Repeat Map OK (not PLLM)");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (!m_WaferMapWrapper.IsMapValid())		//If map not yet loaded, no need to check
	{
		SetErrorMessage("WT: check Repeat Map OK (no map)");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CStrInputDlg dlg;
	if (dlg.DoModal() != IDOK)
	{
		SetErrorMessage("WT: check Repeat Map fail - dialog abort");
		m_WaferMapWrapper.InitMap();
		SetAlignmentStatus(FALSE);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szMapFileName = GetMapFileName();
	if ((dlg.m_szData.GetLength() < 3) || (dlg.m_szData == ""))
	{
		SetErrorMessage("WT: check Repeat Map fail - empty barcode");
		m_WaferMapWrapper.InitMap();		
		SetAlignmentStatus(FALSE);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (szMapFileName.Find(dlg.m_szData) == -1)		//If not FOUND
	{
		HmiMessage("Error: Auto & manual barcode do not match!  Please check machine barcode scanner!");
		SetErrorMessage("WT: check Repeat Map fail - barcode not match!");
		m_WaferMapWrapper.InitMap();		
		SetAlignmentStatus(FALSE);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szLog = "WT: check Repeat Map OK - to continue = " + dlg.m_szData;
	SetErrorMessage(szLog);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::ResetBlkFuncPara(IPC_CServiceMessage &svMsg)
{
	if (m_bFullRefBlock == TRUE)	//Block2
	{
		Blk2SetAlignParameter();
	}
	else
	{
		Blk1SetAlignParameter();
	}
	SaveBlkData();
	return TRUE;
}


LONG CWaferTable::CheckBlkAlignResult(IPC_CServiceMessage &svMsg)
{
	ULONG ulRow = 0, ulCol = 0;
	LONG lUserRow = 0, lUserCol = 0;

	m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);
	ConvertAsmToOrgUser(ulRow,ulCol,lUserRow,lUserCol);

	m_lMnAlignCheckRow = lUserRow;
	m_lMnAlignCheckCol = lUserCol;

	SetJoystickOn(FALSE);

	if ( IsBlkFunc2Enable() )	//Block2
	{
		LONG lX=0, lY=0, lT=0;		//Klocwork
		if( m_pBlkFunc2->Blk2GetNextDiePhyPos(ulRow, ulCol, 2000, 2000, lX, lY) )
		{
			lT = GetGlobalT();
			T_MoveTo(lT);
			XY_SafeMoveTo(lX, lY);
		}
	//	m_pBlkFunc2->Blk2GotoTargetDie();
	}
	else
	{
		//andrew3
		m_bStopGoToBlkDie = FALSE;
		m_bStartGoToAlignDie = TRUE;	//in Operation()
	}
	SetJoystickOn(TRUE);
	return TRUE;
}


//andrew3
BOOL CWaferTable::GoToAlignDie()
{
	if ( m_bFullRefBlock )
		m_pBlkFunc->GotoTargetDie(m_lMnAlignCheckRow, m_lMnAlignCheckCol);
	else
	{
		if ((m_pBlkFunc->GotoReferenceKey(m_lStart_X,m_lStart_Y,m_lMnAlignCheckRow, m_lMnAlignCheckCol)) == FALSE)
		{
			CString szMsg;
			szMsg.Format("Cannot reach Key-Die at (%d, %d)!", m_lMnAlignCheckRow, m_lMnAlignCheckCol);
			HmiMessage(szMsg, "Check Reference");
		}
		else
		{
			SetJoystickOn(FALSE);

			CString szTemp;
			szTemp = _T("Set current position as HOME die?");
			if (IsBlkFunc1Enable() && (HmiMessage(szTemp, "WaferMap", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES))
			{
				GetEncoderValue();
				m_lAlignPosX = GetCurrX();
				m_lAlignPosY = GetCurrY();

				m_WaferMapWrapper.GetCurrentPosition(m_ulAlignRow, m_ulAlignCol);	
				SetAlignStartPoint(TRUE, m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol);

				LONG lAlignResult = AlignBlockWafer();

				if ( lAlignResult == WT_ALN_BLK_OK )
				{
					//	426TX	2
					SetAlignStartPoint(FALSE, m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol);
					m_WaferMapWrapper.SetStartPosition(m_ulAlignRow, m_ulAlignCol); 
					m_pBlkFunc->SetWaferAlignment(1, (LONG)m_ulAlignRow, (LONG)m_ulAlignCol, m_lAlignPosX, m_lAlignPosY);
				}
				
				SetAlignmentStatus(TRUE);
				SetStatusMessage("Auto GOTO align wafer is finished");
				SaveData();

				m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
				DisplayNextDirection(m_ulAlignRow, m_ulAlignCol);				//v3.01T1
			}
				
			SetJoystickOn(TRUE);
		}
	}
	
	m_bStopGoToBlkDie		= FALSE;
	m_bStartGoToAlignDie	= FALSE;

	return TRUE;
}


BOOL CWaferTable::RealignFDCDie(IPC_CServiceMessage &svMsg)
{
    CMS896AApp::m_bStopAlign = FALSE;

	if( IsAlignedWafer()==FALSE )
		return FALSE;

	SetJoystickOn(FALSE);

// prescan relative code	B
	if( IsPrescanBlkPick() && IsPrescanEnded() )
	{
		m_bScnModeWaferAlignTwice	= FALSE;
		if( DEB_IsEnable() )
			return FALSE;
		if( RealignBlkFunc2RefDiePos()==FALSE )
			return FALSE;
		UpdatePrescanPosnAfterBlockRealign();
	}
// prescan relative code	E	
	else
	{
		if ( IsBlkFunc2Enable() )	//Block2
		{
			BOOL	bRtn;
			LONG	lTmp;

			Blk2SetAlignParameter();
			SaveBlkData();

			lTmp = HmiMessage("Start wafer realignment ...", "Re-align", glHMI_MBX_CONTINUESTOP);
			if (lTmp != glHMI_STOP)
			{
				m_lRealignDieNum = 0;
				bRtn = m_pBlkFunc2->Blk2RealignRefDiePos();
				if ( bRtn )
				{
					HmiMessage("Wafer realign finish!", "Re-align", glHMI_MBX_OK);
				}
			}
		}
		else
		{
			m_pBlkFunc->RealignRefDiePos(TRUE, TRUE);
		}
// prescan relative code	B
		if( IsPrescanEnable() )
		{
			CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
			LONG lMapRow, lMapCol, lTableX, lTableY, lRefRow, lRefCol, lRefX, lRefY;
			ULONG i, j;
			for(j=0; j<pUtl->GetNumOfReferPoints(); j++)
			{
				pUtl->GetReferPosition(j, lRefRow, lRefCol, lRefX, lRefY);
				for(i=1; i<=m_pBlkFunc2->GetTotalRefPoints(); i++)
				{
					if( m_pBlkFunc2->Blk2GetRefDiePosn(i, lMapRow, lMapCol, lTableX, lTableY) )
					{
						if( lMapRow==lRefRow && lMapCol==lRefCol )
						{
							pUtl->UpdateReRefPosition(j, lMapRow, lMapCol, lTableX, lTableY);
						}
					}
				}
			}
		}
// prescan relative code	E
	}

	return TRUE;
}

//---------------------------- 
//--- Diagnostic Functions ---
//---------------------------- 
/*** Power On ***/
LONG CWaferTable::Diag_PowerOn_X(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	X_PowerOn(bOn);
	CloseLoadingAlert();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_PowerOn_Y(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	Y_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_PowerOn_T(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	T_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//v4.24T9
LONG CWaferTable::Diag_PowerOn_X2(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (IsESDualWT()==FALSE)
	{
		BOOL bReturn=FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	StartLoadingAlert();
	X2_PowerOn(bOn);
	CloseLoadingAlert();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//v4.24T9
LONG CWaferTable::Diag_PowerOn_Y2(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (IsESDualWT()==FALSE)
	{
		BOOL bReturn=FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	StartLoadingAlert();
	Y2_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//v4.24T9
LONG CWaferTable::Diag_PowerOn_T2(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (IsESDualWT()==FALSE)
	{
		BOOL bReturn=FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	StartLoadingAlert();
	T2_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_PowerOn_All(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	X_PowerOn(bOn);
	Y_PowerOn(bOn);
	T_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

/*** Commutate ***/
LONG CWaferTable::Diag_Comm_X(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(FALSE);
	X_Comm();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Comm_X2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Comm_Y(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(FALSE);
	Y_Comm();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Comm_Y2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Comm_T(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(FALSE);
	T_Comm();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Comm_T2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

/*** Home ***/
LONG CWaferTable::Diag_Home_X(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(FALSE);
	if (X_Home() == gnAMS_OK)
		m_bIsPowerOn_X = TRUE;	
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Home_Y(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(FALSE);
	if (Y_Home() == gnAMS_OK)
		m_bIsPowerOn_Y = TRUE;
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Home_T(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(FALSE);
	if (T_Home() == gnAMS_OK)
	{
		m_bIsPowerOn_T = TRUE;	
	}

	LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);

	//v4.21T3		//MS100Plus 9Inch machine	
	if (CMS896AApp::m_bMS100Plus9InchOption && lEnableEngageOffset != 0)
	{
		LONG lSelection = HmiMessage("Move to UNLOAD T?", "HOME T", glHMI_MBX_YESNO);

		if (lSelection == glHMI_YES)
		{
			Sleep(500);

			LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
			LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			int nPosnT = (int) pApp->GetProfileInt(gszPROFILE_SETTING, _T("WTEngagePositionT"), 0);
			
			//CString szTemp;
			//szTemp.Format("WT Expander T ENGAGE T retrieved = %d (%ld, %ld)", nPosnT, lMinPos, lMaxPos);
			//DisplayMessage(szTemp);

			if (nPosnT > lMaxPos)
				nPosnT = 0;
			if (nPosnT < lMinPos)
				nPosnT = 0;

			T1_MoveTo(nPosnT);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Home_X2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Home_Y2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Home_T2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


/*** Move ***/
LONG CWaferTable::Diag_Move_X(IPC_CServiceMessage& svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	//StartLoadingAlert();
	SetJoystickOn(FALSE);

	//if (labs(lPos) > GetSlowProfileDist())
	//{
	//	X1_Profile(LOW_PROF);
	//}

	CString szLog;
	szLog.Format("WT-X Diag MOVE = %ld ...", lPos);
	//HmiMessage(szLog);

	X1_Profile(NORMAL_PROF);
	X1_Move(lPos);
	//CloseLoadingAlert();
	
	//Sleep(100);
	//X1_Profile(NORMAL_PROF);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Move_X2(IPC_CServiceMessage& svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Move_Y(IPC_CServiceMessage& svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	SetJoystickOn(FALSE);

	Y1_Profile(NORMAL_PROF);
	//if (labs(lPos) > GetSlowProfileDist())
	//{
	//	Y1_Profile(LOW_PROF);
	//}

	CString szLog;
	szLog.Format("WT-Y Diag MOVE = %ld ...", lPos);
	//HmiMessage(szLog);

	Y1_Move(lPos);
	
	Sleep(100);
	Y1_Profile(NORMAL_PROF);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Move_Y2(IPC_CServiceMessage& svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Move_T(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	//StartLoadingAlert();
	SetJoystickOn(FALSE);
	
	CString szLog;
	szLog.Format("WT-T Diag MOVE = %ld ...", lPos);
	HmiMessage(szLog);
		
	T1_Move(lPos);
	//CloseLoadingAlert();
	

	//andrew
	if (!CMS896AStn::MotionIsServo(WT_AXIS_T, &m_stWTAxis_T))
	{
		LONG lEnc_T = 0;
		LONG lCmd_T = 0;
		
		Sleep(1000);

		GetEncoderValue();
		try {
			lCmd_T = CMS896AStn::MotionGetPosition(WT_AXIS_T, &m_stWTAxis_T);
			lEnc_T = GetCurrT();
		} catch (CAsmException e) {
			DisplayException(e);
		}

		long lTPosnDiff = 10;	// default position error = 10 steps
		long lAbsPos = abs(lPos);
		if (lTPosnDiff > lAbsPos)
			lTPosnDiff = lAbsPos / 2;

		// Check if CMD & ENC difference is too large based on tis travel dist
		if (abs(lCmd_T - lEnc_T) > lTPosnDiff)
		{
			CString szErr = "\nWaferTAxis: position error!\nPlease check the T motor.";
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::Diag_Move_T2(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	StartLoadingAlert();
	SetJoystickOn(FALSE);
	T2_Move(lPos);
	CloseLoadingAlert();
	
	//andrew
	if (! CMS896AStn::MotionIsServo(WT_AXIS_T2, &m_stWTAxis_T2))
	{
		LONG lEnc_T = 0;
		LONG lCmd_T = 0;
		
		Sleep(1000);

		GetEncoderValue();
		try {
			lCmd_T = CMS896AStn::MotionGetPosition(WT_AXIS_T2, &m_stWTAxis_T2);
			lEnc_T = GetCurrT2();
		} catch (CAsmException e) {
			DisplayException(e);
		}

		long lTPosnDiff = 10;	// default position error = 10 steps
		long lAbsPos = abs(lPos);
		if (lTPosnDiff > lAbsPos)
			lTPosnDiff = lAbsPos / 2;

		// Check if CMD & ENC difference is too large based on tis travel dist
		if (abs(lCmd_T - lEnc_T) > lTPosnDiff)
		{
			CString szErr = "\nWaferTAxis: position error!\nPlease check the T motor.";
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
		}
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GoToSCNDiePos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;

	//check if wafer aligned?
	if ( IsAlignedWafer() == TRUE )
	{
// prescan relative code	B
		if( IsPrescanEnable() || IsScnLoaded() )
		{
			bReturn = GoToPrescanDieCheckPosition();
		}
// prescan relative code	E
	}
	else
	{
		SetAlert(IDS_WT_MAP_NOT_ALIGN);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::GoToMapPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	ULONG ulRow=0, ulCol=0;
	m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);

	CString szTemp;
	szTemp.Format("Go to (%d, %d)?", ulRow, ulCol);

	if (HmiMessage(szTemp, "WaferMap", glHMI_MBX_YESNO) == glHMI_YES)
	{
		SetJoystickOn(FALSE);	//v3.02T1

		LONG lx=0, ly=0;
		if ( GetMapPhyPosn(ulRow, ulCol, lx, ly) )
		{
			if (XY_SafeMoveTo(lx, ly))
			{
			}
			else
			{
				AfxMessageBox("Current position out of wafer limit", MB_SYSTEMMODAL);
			}
		}
		else
		{
			//AfxMessageBox("No physical position found at current position", MB_SYSTEMMODAL);
			SetAlert(IDS_WT_INVALID_MAP_PHY_POS);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::UpdateOutput(IPC_CServiceMessage& svMsg)
{
	BOOL bUpdate = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bUpdate);

	m_bUpdateOutput = bUpdate;

	bUpdate = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bUpdate);
	return 1;
}


LONG CWaferTable::CheckBurnInMapFilePath(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szBurnInMapFile, szTemp;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szBurnInMapFile = &pBuffer[0];

	delete [] pBuffer;

	if (CreateDirectory(szBurnInMapFile, NULL) == 0)
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			CString szContent;

			szContent.LoadString(HMB_GENERAL_INVALID_PATH);

			HmiMessage(szContent ,"", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);			
		}
	}

	return 1;
}


LONG CWaferTable::GetBurnInMapFilePath(IPC_CServiceMessage &svMsg)
{
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	BOOL bReturn = TRUE;

	CWnd *pMainWnd;

	static char szFilters[] = "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;


	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.txt", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , 
						szFilters, pMainWnd, 0);

	dlgFile.m_ofn.lpstrInitialDir = m_szBurnInMapFile;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	INT nReturn = (INT)dlgFile.DoModal();		// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();			// Show the HMI on top
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	if ( nReturn == IDOK )
	{
		m_lBurningInGrade	= 1;
		m_szBurnInMapFile = dlgFile.GetPathName();
		m_WaferMapWrapper.SelectAlgorithm("TLH", "Direct");
		m_WaferMapWrapper.SetDieStep(1000);

		bReturn = TRUE;
	}
	else
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CWaferTable::EnableMapSyncMove(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSyncMove = FALSE;
	BOOL bPrevSyncMove = m_bMapSyncMove;
	CString szBtnText1 = "Yes";
	CString szBtnText2 = "No";

	svMsg.GetMsg(sizeof(BOOL), &bSyncMove);

	m_bMapSyncMove = bSyncMove;

	m_bJoystickOn = !m_bMapSyncMove; 
	SetJoystickOn(!m_bMapSyncMove);

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetJoystickOn(TRUE);

		if (m_bMapSyncMove)
		{
			m_bJoystickOn	= FALSE;
			m_bXJoystickOn	= FALSE;
			m_bYJoystickOn	= FALSE;
		}
	}

	if (bSyncMove == TRUE)
	{
		SetStatusMessage("Enable Sync Move");		
	}
	else
	{
		SetStatusMessage("Disable Sync Move");
	}


	//If syncmove is turn off, ask user to update map to current pos or not
	if ( (bPrevSyncMove == TRUE) && (bSyncMove == FALSE) )
	{
		CString szTitle, szContent;

		szTitle.LoadString(HMB_WT_MAP_SYNC_MOVE);
		szContent.LoadString(HMB_WT_MAP_UPDATE_POS);
 
		m_bSelOfUpdateMapPos = TRUE;	//v4.38T5	//PLLM Lumiramic

		if (m_bSelOfUpdateMapPos)
		{
			ULONG ulCurrRow = 0, ulCurrCol = 0;
			if ( HmiMessage(szContent, szTitle, glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szBtnText1, &szBtnText2) != 2 )	//	not glHMI_CANCEL	//v2.89T1
			{
				CString szText;

				//Get current & set selected position
				m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);
				m_WaferMapWrapper.SetSelectedPosition(ulCurrRow, ulCurrCol);

				//Get encoder start position
				Sleep(50);
				GetEncoderValue();
				m_lStart_X		= GetCurrX();
				m_lStart_Y		= GetCurrY();
				m_lStart_T		= GetCurrT();

				SetGlobalTheta();

				SetAlignmentStatus(TRUE);

				ResetMapStauts();

				//Re-calculate map 
				m_WaferMapWrapper.ResetGrade();
				m_WaferMapWrapper.GetSelectedPosition(ulCurrRow, ulCurrCol);
				m_WaferMapWrapper.SetStartPosition(ulCurrRow, ulCurrCol);
				m_WaferMapWrapper.EnableAutoAlign(FALSE);

				SetStatusMessage("Map start position is updated");

				//Refresh wafermap
				m_WaferMapWrapper.Redraw();
			}
			else
			{
				//v4.38T5
				//Still need update current map cursor to prevent accidentially pressed MANUAL-ALIGN key
				m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);
				m_WaferMapWrapper.SetSelectedPosition(ulCurrRow, ulCurrCol);
			}
		}
	}
	else if (bSyncMove && !bPrevSyncMove)		//v2.97T4
	{
		ULONG ulCurrRow=0, ulCurrCol=0;
		LONG lPosX=0, lPosY=0;
		CString szTemp;

		m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);

		if ( GetMapPhyPosn(ulCurrRow, ulCurrCol, lPosX, lPosY) )
		{
			szTemp.Format("Syn Move ROW=%d, COL=%d,  pos-x = %d, pos-y = %d", ulCurrRow, ulCurrCol, lPosX, lPosY);
			//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
			SetStatusMessage(szTemp);

			if (IsEnableWL() && IsWLExpanderOpen())
			{
				SetErrorMessage("Expander not closed");
			}
			else
			{
				XY_SafeMoveTo(lPosX, lPosY);
			}
		}
		else
		{
			szTemp.Format("Syn Move No phy posn found at ROW=%d, COL=%d", ulCurrRow, ulCurrCol);
			//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
			SetStatusMessage(szTemp);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::UpdateMapGradeColor(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bResetColor = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bResetColor);

	bReturn = UpdateWaferGradeColor(bResetColor);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::ResetGradeMapping(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp!=NULL && pApp->CheckIsAllBinCleared()==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetErrorMessage("IPC command clear wafer map, reset grade mapping");
	//Clear Wafermap & reset grade map
	m_WaferMapWrapper.InitMap();
	m_WaferMapWrapper.ResetGradeMap();
	DeleteGradeMapTable();		//v4.08

	//v4.59A16	//Renesas MS90 Character die grade description on Map display
	m_WaferMapWrapper.ResetGradeDescription();		

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SetStartBondEvent(IPC_CServiceMessage &svMsg)
{
	BOOL bResult;

	//Set SECSGEM Value
	//SetCtmLogMessage("SG_CEID_StartBond");
	try
	{
		SetGemValue(MS_SECS_SV_WAFER_ID, m_szBCMapFilename);		//3003
		SetGemValue(MS_SECS_SV_SLOT_NO, 0);							//3004
		SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");				//3005
		SetGemValue(MS_SECS_SV_TOTAL_BIN_TABLE_DIE_ON_WAFER, 0);	//3040
		SendEvent(SG_CEID_StartBond, TRUE);			//8016
	}
	catch(...)
	{
		DisplaySequence("SG_CEID_StartBond failure");
	}

	bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

LONG CWaferTable::SetStopBondEvent(IPC_CServiceMessage &svMsg)
{
	BOOL bResult;

	//Set SECSGEM Value
	//SetCtmLogMessage("SG_CEID_StopBond");
	try
	{
		SetGemValue(MS_SECS_SV_WAFER_ID, "WaferId");
		SetGemValue(MS_SECS_SV_SLOT_NO, 0);							//3004
		SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");				//3005
		SetGemValue(MS_SECS_SV_TOTAL_BIN_TABLE_DIE_ON_WAFER, 0);	//3040
		SendEvent(SG_CEID_StopBond, TRUE);				//8019
	}
	catch(...)
	{
		DisplaySequence("SG_CEID_StopBond failure");
	}

	bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

LONG CWaferTable::SetWorkingAngle(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bHome = FALSE;
	
	svMsg.GetMsg(sizeof(BOOL), &bHome);

	if ( bHome == TRUE )
	{
		m_lWaferWorkingAngle = 0;
		T_Home();
	}
	else
	{
		T1_MoveTo((INT)((DOUBLE)m_lWaferWorkingAngle / m_dThetaRes),SFM_WAIT);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::ConfirmWorkingAngle(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bConfirm = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bConfirm);

	if ( bConfirm == TRUE )
	{
		SaveWaferTblData();
	}
	else
	{
		m_lWaferWorkingAngle = m_lWaferCurrentWorkingAngle;
	}

	T_Home();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


VOID CWaferTable::SendCE_ScanSetting(VOID)
{
	try
	{
		// 3441
		SetGemValue("AB_bScnResultCheck", m_bEnableSCNCheck);
		SetGemValue("AB_lScnResultRow", m_lScnHmiRow);
		SetGemValue("AB_lScnResultCol", m_lScnHmiCol);
		// 3442
		SetGemValue("AB_bScnKeepAlignDie", m_bKeepSCNAlignDie);
		SetGemValue("AB_lScnHorizontal", m_lSCNIndex_X);
		SetGemValue("AB_lScnVertical", m_lSCNIndex_Y);
		// 7504
		SendEvent(SG_CEID_WT_SCNRESULT, FALSE);
	}
	catch(...)
	{
		DisplaySequence("SendCE_ScanSetting failure");
	}
}


LONG CWaferTable::EndLotClearMap(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	SetErrorMessage("End Lot and clear wafer map, reset grade mapping");
	//Clear Wafermap & reset grade map
	CMS896AStn::m_WaferMapWrapper.InitMap();
	CMS896AStn::m_WaferMapWrapper.ResetGradeMap();
	DeleteGradeMapTable();		//v2.83T32
	SetPsmEnable(FALSE);

	m_szCurrentMapPath = GetMapFileName();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::ManualRotateWaferTheta(IPC_CServiceMessage &svMsg)
{
	typedef struct {
		LONG lDirection;
		LONG lRelPos;
		BOOL bUsingAngle;
	} ROT_THETA;


	BOOL bReturn = TRUE;
	ROT_THETA stPos;
	LONG lRelPos = 0;

	svMsg.GetMsg(sizeof(ROT_THETA), &stPos);
	BOOL bBinLoader = IsBLEnable();	//	426TX	1	// DL/DLA or STD config?	//v3.00T1

	//v3.48	//CHeck Wexpander lock
	if (!CheckWExpanderLock())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		SetErrorMessage("Manual Rotate Wafer T fail because Expander not lock");
		return 1;
	}


	if (stPos.bUsingAngle == TRUE)
		lRelPos = (LONG) (stPos.lRelPos / m_dThetaRes);
	else
		lRelPos = (LONG) stPos.lRelPos;

	if ( stPos.lDirection == 1 )
	{
		if ( bBinLoader || IsEnableWL() )
		{
			T1_Move(lRelPos, SFM_WAIT);	// Move POS direction (clock-wise in DL)
		}
		else
			T1_Move(-lRelPos, SFM_WAIT);	// Move NEG direction (anti clock-wise in DL)
	}
	else
	{
		if ( bBinLoader || IsEnableWL() )
		{
			T1_Move(-lRelPos, SFM_WAIT);	// Move NEG direction (anti clock-wise in DL)
		}
		else
			T1_Move(lRelPos, SFM_WAIT);	// Move POS direction (clock-wise in DL)
	}
	
	GetEncoderValue();		// Get the encoder value from hardware
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::SetDisableRefDie(IPC_CServiceMessage &svMsg)
{
	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	m_bDisableRefDie = bEnable;

	if (m_bDisableRefDie)
	{
		m_WaferMapWrapper.SetReferenceDie(0, 0, FALSE);
		m_WaferMapWrapper.ResetGrade();
		HmiMessage("Ref die disabled; please reload current map file manually.");

	}
	else
	{
		HmiMessage("Ref die enabled; please reload current map file manually.");
	}

	SaveData();
	return 1;
}

// Show the Select  Folder Dialog for the user to set the path
LONG CWaferTable::GetWaferLotInfoFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	//CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	//((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szLotInfoFilePath);
		SaveData();		
		bReturn = TRUE;
	}

	//pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);
	(*m_psmfSRam)["WaferTable"]["WaferLotInfoFilePath"] = m_szLotInfoFilePath;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::EnableCheckWaferName(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bCheckLotWaferName)
	{
		SetStatusMessage("WaferName check enable");	
	}
	else
	{
		SetStatusMessage("WaferName check disable");	
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsRunTimeLoadPKGFile() )
	{
		BOOL bFalse = FALSE;
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(BOOL), &bFalse);
		INT nConvID = m_comClient.SendRequest("MapSorter", "SetLoadPkgForNewWaferFrame", stMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


// Read the wafer lot file and store it to msd file
LONG CWaferTable::ReadWaferLotInfoFile(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( (CMS896AStn::m_bEnableWaferLotFileProtection == TRUE) || pApp->GetCustomerName() == "OSRAM" )  // 4.51D1 Secs
	{
		// check clear all bin before loading lot info file
		if (pApp->CheckIsAllBinCleared() == FALSE)
		{
			m_szLotInfoFileName = (*m_psmfSRam)["MS896A"]["WaferLot Filename"];  
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	if( pApp->GetCustomerName() == "Electech3E(DL)" )
	{
		SetAlarmLog("here to input op id for DL 3E"); 
		for(int i=0; i<5; i++)
		{
			SetAlarmLog("try to input DL 3E op Id");
			CString szTempID = "";
			if( HmiStrInputKeyboard("Please input your OP ID:", szTempID) )
			{
				SetAlarmLog("input op id is " + szTempID);
				if( szTempID.GetLength()==5 )
				{
					m_szLotInfoOpID = szTempID;
					break;
				}
			}
		}
		SetAlarmLog("here finish inputing the OP ID for DL 3E");
	}

	if (SaveWaferLotData())
	{
		HmiMessage("Wafer Lot Info saved successfully.");
	}


	//v4.42T16
	if ((pApp->GetCustomerName() == "Electech3E") ||
		(pApp->GetCustomerName() == "Electech3E(DL)") )		//v4.48C1
	{
		BOOL	bStatus = FALSE;
		IPC_CServiceMessage stMsg;
		CString szName =   m_szLotInfoFilePath + "\\" + m_szLotInfoFileName + "." + m_szLotInfoFileExt;

		INT nMsgLength = (szName.GetLength() + 1) * sizeof(CHAR);
		char* pFilename;
		pFilename = new char[nMsgLength];
		strcpy_s(pFilename, nMsgLength, szName);
	
		stMsg.InitMessage(nMsgLength, pFilename);
		//stMsg.InitMessage(szName.GetLength(), &szName);
		INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "LoadBinLotSummaryFile2", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bStatus);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		delete [] pFilename;
		//4.53D26 check comparing function
		if( bStatus == FALSE )
		{
			//for protecting Start bonding
			m_WaferMapWrapper.InitMap();	

			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::ReadWaferLotInfoFile_SECSGEM(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (CMS896AStn::m_bEnableWaferLotFileProtection == TRUE)
	{
		// check clear all bin before loading lot info file
		if (pApp->CheckIsAllBinCleared() == FALSE)
		{
			m_szLotInfoFileName = (*m_psmfSRam)["MS896A"]["WaferLot Filename"];  
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	m_szLotInfoFileName = (*m_psmfSRam)["MS"]["SECSGEM"]["WaferLotName"];
	CString szLog = "WT::ReadWaferLotInfoFile start - " + m_szLotInfoFileName;
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	if (!SaveWaferLotData())
	{
		HmiMessage_Red_Yellow("Fail to save Wafer Lot Info - " + m_szLotInfoFileName);
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::ClearWaferLotDataCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::ShowWaferLotDataCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	ShowWaferLotData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::UpdateWaferLotLoadStatusCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	UpdateWaferLotLoadStatus();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CheckAllWaferLoadedCmd(IPC_CServiceMessage& svMsg)
{
	CString szContent, szTitle;
	BOOL bReturn = TRUE;
	CStringArray szaWaferIds;
	IPC_CServiceMessage stMsg;
	CString szMsg, szTemp, szTemp2, szStr;

	szTitle.LoadString(HMB_WAFER_LOT);
	szStr.LoadString(HMB_WT_WAFER_LOT_CHECK_FAIL);

	szMsg = "";

	if (CheckAllWaferLoaded(szaWaferIds) == FALSE)
	{
		for (INT i=0; i<szaWaferIds.GetSize(); i++)
		{
			if (i == 0)
			{
				szMsg = szaWaferIds.GetAt(i);
			}
			else
			{
				szMsg = szMsg + ", " + szaWaferIds.GetAt(i);
			}
			//szTemp.Format("%d. WaferId: ", i+1);
			//szTemp = szTemp + szaWaferIds.GetAt(i);
			//szMsg = szMsg + szTemp  + "\n";
		}
		
		HmiMessage_Red_Yellow(szStr + "\n" +  szMsg, szTitle);
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// Lot File Name Comparision for manual mode
LONG CWaferTable::CheckWaferLotDataInManualMode(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bCheckLotWaferName)
	{
		if (m_bEnableBarcodeLoadMap)
		{
			bReturn = CompareWaferLotDatabase(m_szBCMapFilename);
		}
		else
		{
			bReturn = CompareWaferLotDatabase(GetMapNameOnly());
		}

		CString szArrTemp[4];
		szArrTemp[0] = (*m_psmfSRam)["WaferTable"]["YealyFilePath_1Layer"];  // Top == First
		szArrTemp[1] = (*m_psmfSRam)["WaferTable"]["YealyFilePath_2Layer"];
		szArrTemp[2] = (*m_psmfSRam)["WaferTable"]["YealyFilePath_3Layer"];
		szArrTemp[3] = (*m_psmfSRam)["WaferTable"]["YealyFilePath_LowestLayer"];
		if( IsWT2InUse() )
		{
			(*m_psmfSRam)["WaferTable"]["WT2FilePath_1Layer"]		= szArrTemp[0];  // Top == First
			(*m_psmfSRam)["WaferTable"]["WT2FilePath_2Layer"]		= szArrTemp[1];
			(*m_psmfSRam)["WaferTable"]["WT2FilePath_3Layer"]		= szArrTemp[2];
			(*m_psmfSRam)["WaferTable"]["WT2FilePath_LowestLayer"]	= szArrTemp[3];
		}
		else
		{
			(*m_psmfSRam)["WaferTable"]["WT1FilePath_1Layer"]		= szArrTemp[0];  // Top == First
			(*m_psmfSRam)["WaferTable"]["WT1FilePath_2Layer"]		= szArrTemp[1];
			(*m_psmfSRam)["WaferTable"]["WT1FilePath_3Layer"]		= szArrTemp[2];
			(*m_psmfSRam)["WaferTable"]["WT1FilePath_LowestLayer"]	= szArrTemp[3];
		}

		if (bReturn == FALSE)
		{
			SetAlert(IDS_WT_NO_WAFER_IN_LOT);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// Lot File Name Comparision for Auto mode
LONG CWaferTable::CheckWaferLotDataInAutoMode(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	LPTSTR lpszMapFilename = new CHAR[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), lpszMapFilename);
	
	CString szMapFilename = &lpszMapFilename[0];
	delete [] lpszMapFilename;

	if (m_bCheckLotWaferName)
	{	
		bReturn = CompareWaferLotDatabase(szMapFilename);
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// Searching Barcode for
LONG CWaferTable::ReadBarCodeOnTable(IPC_CServiceMessage& svMsg)
{
	INT  nReturn = gnOK ;
	BOOL bReturn = TRUE;

	SetJoystickOn(FALSE);

	//nXYDirection = 0 for moving x direction
	//nXYDirection = 1 for moving y direction

	typedef struct
	{
		INT	nXYDirection;
		INT siDirection;
		LONG lSearchDistance;
		BOOL bWaitComplete;
			
	}TABLE_MOVE ;
	
	TABLE_MOVE stInfo;
	
	svMsg.GetMsg(sizeof(TABLE_MOVE), &stInfo);

	if (stInfo.nXYDirection == 0)
	{
		nReturn = SearchBarcodeMoveX(stInfo.siDirection, stInfo.lSearchDistance, stInfo.bWaitComplete);
	}
	else if (stInfo.nXYDirection == 1)
	{
		nReturn = SearchBarcodeMoveY(stInfo.siDirection, stInfo.lSearchDistance, stInfo.bWaitComplete);
	}
	else if (stInfo.nXYDirection == 2)
	{
		nReturn = SearchBarcodeMoveT(stInfo.siDirection, stInfo.lSearchDistance, stInfo.bWaitComplete);
	}

	if (nReturn != gnOK)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	
	return 1;
}

//	ES101_XU	need add another function as it is used when load frame out
LONG CWaferTable::ReadBarCodeOnTable2(IPC_CServiceMessage& svMsg)
{
	INT  nReturn = gnOK ;
	BOOL bReturn = TRUE;

	SetJoystickOn(FALSE);

	typedef struct
	{
		INT	nXYDirection;
		INT siDirection;
		LONG lSearchDistance;
		BOOL bWaitComplete;
			
	}TABLE_MOVE ;
	
	TABLE_MOVE stInfo;
	
	svMsg.GetMsg(sizeof(TABLE_MOVE), &stInfo);

	if (stInfo.nXYDirection == 0)
	{
		nReturn = SearchBarcodeMoveX2(stInfo.siDirection, stInfo.lSearchDistance, stInfo.bWaitComplete);
	}
	else if (stInfo.nXYDirection == 1)
	{
		nReturn = SearchBarcodeMoveY2(stInfo.siDirection, stInfo.lSearchDistance, stInfo.bWaitComplete);
	}
	else if (stInfo.nXYDirection == 2)
	{
		nReturn = SearchBarcodeMoveT2(stInfo.siDirection, stInfo.lSearchDistance, stInfo.bWaitComplete);
	}

	if (nReturn != gnOK)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	
	return 1;
}

// Command for Sync Motor X
LONG CWaferTable::X_SyncCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	try
	{
		if( X1_Sync()!=gnOK )
			bReturn = FALSE;
	}
	catch (CAsmException e)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

// Command for Sync Motor Y
LONG CWaferTable::Y_SyncCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	try
	{
		Y1_Sync();
	}
	catch (CAsmException e)
	{
		bReturn = FALSE;
	}
	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

// Command for Sync Motor T
LONG CWaferTable::T_SyncCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	try
	{
		T_Sync();
	}
	catch (CAsmException e)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

//V2.83T4 Sync X & Sync Y
LONG CWaferTable::XY_SyncCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	try
	{
		X1_Sync();
		Y1_Sync();
	}
	catch (CAsmException e)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

LONG CWaferTable::UpdateDirectCornerSearchPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	typedef	struct {
		LONG	lX;
		LONG	lY;
		BOOL	bDirectCornerSearchDie; 
	} SEARCH_INFO;

	SEARCH_INFO stInfo;

	svMsg.GetMsg(sizeof(SEARCH_INFO),&stInfo);
	
	m_nCornerHomeDiePosX = stInfo.lX;
	m_nCornerHomeDiePosY = stInfo.lY;
	SaveData();		//v2.93T2

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::DirectCornerSearchDie(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;

	svMsg.InitMessage(sizeof(BOOL),&bReturn);

	return 1;
}

LONG CWaferTable::IsUpperLeftCornerComplete(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (IsMapNullBin(0, 0))
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

LONG CWaferTable::ClearMapDisplayFilename(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	m_szMapDisplayFilename = "";
	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

LONG CWaferTable::UpdateMapFileName(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="HuaLei" )
	{
		m_szMapDisplayFilename = m_szMapDisplayFilename.MakeUpper();
	}
	m_szBCMapFilename = m_szMapDisplayFilename;

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}
	

LONG CWaferTable::UpdateNewEjrSeqOption(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	m_bNewEjectorSequence = bEnable;

	SaveWaferTblData();

	if (bEnable)
		HmiMessage("New ejector sequence option enabled");
	else
		HmiMessage("New ejector sequence option disabled");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::PrAdaptWafer(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::TestFcn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	//	426TX	1	move to auto align button
	HmiMessage("Move to auto align button, Done.");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::UpdateSortingCheckPointsOption(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable = FALSE, bReturn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	// If function is enabled, load the check points to the CStringArray m_szaSortingCheckPts
	if (bEnable == TRUE)
	{
		if (ReadSortingCheckPoints(WT_SORTING_CHECK_POINT_FILENAME) == FALSE)
		{
			CString szTitle, szContent;
			szTitle.LoadString(HMB_WT_SORTING_CHECK_PTS);
			szContent.LoadString(HMB_WT_INVALID_SORTING_CHECK_PTS_FILE);
			m_bEnableSortingCheckPts = FALSE;
			HmiMessage(szContent, szTitle);
		}
	}
	else
	{
		m_szaSortingCheckPts.RemoveAll();
	}

	SaveData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::LoadWaferMapHeader(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn;
	CString szTitle, szContent;
	LPTSTR lpsz = new TCHAR[255];
	svMsg.GetMsg(sizeof(TCHAR)*255, lpsz);

	bReturn = LoadnCheckWaferMapColumnsFile(lpsz);

	if (bReturn == FALSE)
	{
		szTitle.LoadString(HMB_WT_MAP_HEADER_FILE);
		szContent.LoadString(HMB_WT_LOAD_HEADER_FAILED);
		HmiMessage(szContent, szTitle);
	}
	
	//BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	delete[] lpsz;

	return 1;
}

LONG CWaferTable::LoadOutputFileFormat(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn;
	CString szTitle, szContent;
	LPTSTR lpsz = new TCHAR[255];
	svMsg.GetMsg(sizeof(TCHAR)*255, lpsz);
	
	bReturn = LoadnCheckOutputFileFormatFile(lpsz);

	if (bReturn == FALSE)
	{
		szTitle.LoadString(HMB_WT_OTF_FORMAT_FILE);
		szContent.LoadString(HMB_WT_LOAD_OTF_FAILED);
		
		HmiMessage(szContent, szTitle);
	}

	//BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	delete[] lpsz;

	return 1;

}

LONG CWaferTable::UpdateMapDieTypeCheck(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bEnableMapDieTypeCheck == FALSE)
	{
		m_bIsMapDieTypeCheckOk = TRUE;
	}

	SaveData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::LoadDieTypeFromFile(IPC_CServiceMessage& svMsg)
{
	CString szFullFileName;
	BOOL bFileOK = FALSE;
	CStdioFile cfTypeFile;
	CString szSelectionItem;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( (pApp->GetCustomerName()!="Tekcore") )
	{
		BOOL bReply = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	if( m_szUserDefineDieType!="" )
	{
		HmiMessage("please clear all bins firstly");
		BOOL bReply = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]= "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.txt", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , szFilters, pMainWnd, 0);

	dlgFile.m_ofn.lpstrInitialDir = m_szMapFilePath;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	if ( nReturn == IDOK )
	{
		szFullFileName	= dlgFile.GetPathName();	// full path and file name

		//Check file format is valid is correct or not
		bFileOK = cfTypeFile.Open(szFullFileName, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText);

		if (bFileOK == TRUE)
		{
			cfTypeFile.ReadString(szSelectionItem);
			cfTypeFile.Close();
			m_szUserDefineDieType = szSelectionItem;

			SaveData();
		}
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferTable::ResetDieType(IPC_CServiceMessage& svMsg)
{
	if (CMS896AStn::m_bClearDieTypeFieldDuringClearAllBin == TRUE)
	{
		m_szUserDefineDieType = "";
		SaveData();
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

LONG CWaferTable::CheckDieType(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;

	if (CMS896AStn::m_bClearDieTypeFieldDuringClearAllBin == TRUE)
	{
		if( m_szUserDefineDieType!="" )
		{
			bReply = FALSE;
			HmiMessage("please clear all bins firstly");
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferTable::CheckInputWaferIDHeader(IPC_CServiceMessage& svMsg)
{
	CString szCheckWaferID;
	int nIndex;

	szCheckWaferID = m_szMapHeaderCheckWaferId;

	// filter to remove duplicated ** or *? or ?*
	while( 1 )
	{
		if( szCheckWaferID.Find("*?")==-1 &&
			szCheckWaferID.Find("?*")==-1 &&
			szCheckWaferID.Find("**")==-1 )
		{
			break;
		}

		szCheckWaferID.Replace("*?", "*");
		szCheckWaferID.Replace("?*", "*");
		szCheckWaferID.Replace("**", "*");
	}

	// to remove the * at begin and end position
	nIndex = szCheckWaferID.GetLength()-1;
	if( szCheckWaferID.GetAt(nIndex)=='*' )
		szCheckWaferID = szCheckWaferID.Left(nIndex-1);
	if( szCheckWaferID.GetAt(0)=='*' )
		szCheckWaferID = szCheckWaferID.Mid(1);
	// remove the ? at begin
	while( 1 )
	{
		if( szCheckWaferID.GetAt(0)=='?' )
			szCheckWaferID = szCheckWaferID.Mid(1);
		else
			break;
	}
	// remove the ? at end
	while( 1 )
	{
		nIndex = szCheckWaferID.GetLength()-1;
		if( szCheckWaferID.GetAt(nIndex)=='?' )
			szCheckWaferID = szCheckWaferID.Left(nIndex-1);
		else
			break;
	}

	SetErrorMessage("Input Check ID:" + szCheckWaferID);
	m_szMapHeaderCheckWaferId = szCheckWaferID;

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}


BOOL CWaferTable::CheckCORTolerance(LONG lOffsetX, LONG lOffsetY)
{
	if( m_lCorBackPitchTolX>0 && labs(lOffsetX) > labs(GetDiePitchX_X()*m_lCorBackPitchTolX/100) )
	{
		return FALSE;
	}

	if( m_lCorBackPitchTolY && labs(lOffsetY) > labs(GetDiePitchY_Y()*m_lCorBackPitchTolY/100) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckCOROffset(LONG lOffsetX, LONG lOffsetY)
{
	if( m_lCorMovePitchTolX>0 && labs(lOffsetX) > labs(GetDiePitchX_X()*m_lCorMovePitchTolX/100) )
	{
		return FALSE;
	}

	if( m_lCorMovePitchTolY && labs(lOffsetY) > labs(GetDiePitchY_Y()*m_lCorMovePitchTolY/100) )
	{
		return FALSE;
	}

	return TRUE;
}

LONG CWaferTable::AutoCheckCOR(IPC_CServiceMessage& svMsg)
{
	BOOL bNormalDie;
	LONG lOldX=0, lOldY=0, lOldT=0;
	LONG lDiffUmX, lDiffUmY;
	LONG lX,lY,lT;
	LONG lX0,lY0;
	BOOL bResult;
	CString	strTemp;

	svMsg.GetMsg(sizeof(BOOL), &bNormalDie);
	bNormalDie = !bNormalDie;

	SetJoystickOn(TRUE);
	HmiMessageEx("Start wafer COR checking...");
	SetJoystickOn(FALSE);
	Sleep(500);

	GetEncoder(&lOldX, &lOldY, &lOldT);
	if( IsWithinWaferLimit(lOldX, lOldY) == FALSE )
	{
		strTemp.Format("CHECK COR position out of table XY(%d,%d) limit!", lOldX, lOldY);
		HmiMessage(strTemp);
		SetErrorMessage(strTemp);
		return 1;
	}

	GetEncoder(&lX,&lY,&lT);
	if( bNormalDie )
		bResult = WftMoveSearchDie( lX, lY, TRUE);
	else
		bResult = WftMoveSearchReferDie( lX, lY, TRUE);
	Sleep(1000);
	if( bResult==FALSE )
	{
		strTemp.Format("CHECK COR failure 0, no die found!");
		SetErrorMessage(strTemp);
		HmiMessage(strTemp);
		return 1;
	}

	GetEncoder(&lOldX, &lOldY, &lOldT);

	CheckCOR(5.0);
	Sleep(500);
	GetEncoder(&lX,&lY,&lT);
	lX0=lX; lY0=lY;
	if( bNormalDie )
		bResult = WftMoveSearchDie( lX, lY, FALSE);
	else
		bResult = WftMoveSearchReferDie( lX, lY, FALSE);
	Sleep(1000);
	lDiffUmX = (LONG)(ConvertMotorStepToUnit(lX-lX0));
	lDiffUmY = (LONG)(ConvertMotorStepToUnit(lY-lY0));
	if ( bResult == FALSE )
	{
		strTemp.Format("CHECK COR failure 1, no die found!");
		SetErrorMessage(strTemp);
		HmiMessage(strTemp);
	}
	bResult = CheckCOROffset((lX-lX0), (lY-lY0));
	if ( bResult == FALSE )
	{
		strTemp.Format("CHECK COR failure 2, XY_Error(%d,%d), over half pitch or 5 mil!", lDiffUmX, lDiffUmY );
		SetErrorMessage(strTemp);
		HmiMessage(strTemp);
	}
	XY_SafeMoveTo(lOldX, lOldY);
	T_MoveTo(lOldT, SFM_WAIT);
	Sleep(500);
	if( bResult == FALSE )
	{
		return 1;
	}

	strTemp.Format("CHECK COR first step ok, XY_Error(%d,%d)!", lDiffUmX, lDiffUmY );
	SetErrorMessage(strTemp);

	//Rotate back and check
	GetEncoder(&lX,&lY,&lT);
	lX0=lX; lY0=lY;
	if( bNormalDie )
		bResult = WftMoveSearchDie( lX, lY, TRUE);
	else
		bResult = WftMoveSearchReferDie( lX, lY, TRUE);
	Sleep(1000);
	lDiffUmX = (LONG)(ConvertMotorStepToUnit(lX-lX0));
	lDiffUmY = (LONG)(ConvertMotorStepToUnit(lY-lY0));
	if (bResult==FALSE)
	{
		strTemp.Format("CHECK COR failure 3, no die found!");
		SetErrorMessage(strTemp);
		HmiMessage(strTemp);
		return 1;
	}
	if ( CheckCORTolerance((lX-lX0), (lY-lY0))==FALSE )
	{
		strTemp.Format("CHECK COR failure 4, XY_Error(%d,%d) over 2 mil!", lDiffUmX, lDiffUmY );
		SetErrorMessage(strTemp);
		HmiMessage(strTemp);
		return 1;
	}

	strTemp.Format("CHECK COR second step ok, XY_Error(%d,%d)", lDiffUmX, lDiffUmY );
	SetErrorMessage(strTemp);


	CheckCOR(-5.0);
	Sleep(500);
	GetEncoder(&lX,&lY,&lT);
	lX0=lX; lY0=lY;
	if( bNormalDie )
		bResult = WftMoveSearchDie( lX, lY, FALSE);
	else
		bResult = WftMoveSearchReferDie( lX, lY, FALSE);
	Sleep(1000);
	lDiffUmX = (LONG)(ConvertMotorStepToUnit(lX-lX0));
	lDiffUmY = (LONG)(ConvertMotorStepToUnit(lY-lY0));

	if ( bResult == FALSE )
	{
		strTemp.Format("CHECK COR failure 5, no die found!");
		HmiMessage(strTemp);
		SetErrorMessage(strTemp);
	}
	bResult = CheckCOROffset((lX-lX0), (lY-lY0));
	if( bResult==FALSE )
	{
		strTemp.Format("CHECK COR failure 6, XY_Error(%d,%d), over half pitch or 5 mil!", lDiffUmX, lDiffUmY );
		HmiMessage(strTemp);
		SetErrorMessage(strTemp);
		return 1;
	}

	XY_SafeMoveTo(lOldX, lOldY);
	T_MoveTo(lOldT, SFM_WAIT);
	Sleep(500);

	strTemp.Format("CHECK COR thrid step ok, XY_Error(%d,%d)!", lDiffUmX, lDiffUmY );
	SetErrorMessage(strTemp);

	//Rotate back and check
	GetEncoder(&lX,&lY,&lT);
	lX0=lX; lY0=lY;
	if( bNormalDie )
		bResult = WftMoveSearchDie( lX, lY, TRUE);
	else
		bResult = WftMoveSearchReferDie( lX, lY, TRUE);
	Sleep(1000);
	lDiffUmX = (LONG)(ConvertMotorStepToUnit(lX-lX0));
	lDiffUmY = (LONG)(ConvertMotorStepToUnit(lY-lY0));

	if (bResult == FALSE)
	{
		strTemp.Format("CHECK COR failure 7, no die found!");
		HmiMessage(strTemp);
		SetErrorMessage(strTemp);
		return 1;
	}

	if ( CheckCORTolerance((lX-lX0), (lY-lY0))==FALSE )
	{
		strTemp.Format("CHECK COR failure 8, XY_Error(%d,%d) over 2 mil!", lDiffUmX, lDiffUmY);
		HmiMessage(strTemp);
		SetErrorMessage(strTemp);
		return 1;
	}

	strTemp.Format("CHECK COR result is ok, XY_Error(%d,%d)!", lDiffUmX, lDiffUmY);
	HmiMessage(strTemp);
	SetErrorMessage(strTemp);

	return 1;
}

LONG CWaferTable::UpdateAutoMapDieTypeCheck(IPC_CServiceMessage& svMsg)	//Auto Map Die Type Check (for Ubilux)	//v3.31T1
{
	BOOL bReturn = TRUE;

	if (m_bEnableAutoMapDieTypeCheck == FALSE)
	{
		for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
		{
			m_baIsMapDieTypeCheckOk[i] = TRUE;
		}

	}

	SaveData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::GoToBlk1stAutoAlignPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;

	if (!IsBlkFuncEnable())	//Only available for block-pick
	{
		HmiMessage("Error: BLK fcn disabed!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_bFullRefBlock)	//Not for Osram block-pick mode
	{
		HmiMessage("Error: Full BLK fcn disabed!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if ((m_lBlkAutoAlignPosX==0) && (m_lBlkAutoAlignPosY==0) && (m_lBlkAutoAlignPosT==0))
	{
		HmiMessage("Error: positions are zero!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if ( IsAlignedWafer()==FALSE )
	{
		HmiMessage("Error: map not yet aligned!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	if (!IsWithinWaferLimit(m_lBlkAutoAlignPosX, m_lBlkAutoAlignPosY))
	{
		HmiMessage("Error: Align posn not within wafer limit!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);
	T_MoveTo(m_lBlkAutoAlignPosT, SFM_WAIT);
	XY_MoveTo(m_lBlkAutoAlignPosX, m_lBlkAutoAlignPosY, SFM_WAIT);
	Sleep(500);
	SetJoystickOn(TRUE);
		
	m_WaferMapWrapper.SetCurrentPosition(m_ulBlkAutoAlignRow, m_ulBlkAutoAlignCol);

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::LoadWaferMapErrorCheckingCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (LoadWaferMapErrorChecking() != TRUE)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::WaferMapHeaderCheckingCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if ( IsMapLoadingOk() != TRUE )
			bReturn = FALSE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckLoadCurrentMapCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	if (CheckLoadCurrentMapStatus() != TRUE)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}



LONG CWaferTable::EnableMESConnection(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMESConnector::Instance()->EnableMESConnector(m_bEnableMESConnection);
	CMESConnector::Instance()->SetMESIncomingPath(m_szMESIncomingPath);
	CMESConnector::Instance()->SetMESOutgoingPath(m_szMESOutgoingPath);
	CMESConnector::Instance()->SetMESTimeout(m_lMESTimeout);

	if (m_bEnableMESConnection == FALSE)
	{
		m_bMESWaferIDCheckOk = TRUE;
		m_szMESWaferIDCheckMsg = "";
	}

	SaveData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetMESIncomingPath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;

	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{

		pAppMod->GetPath(m_szMESIncomingPath);
		CMESConnector::Instance()->SetMESIncomingPath(m_szMESIncomingPath);
		SaveData();
		bReturn = TRUE;
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::GetMESOutgoingPath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;

	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szMESOutgoingPath);
		CMESConnector::Instance()->SetMESOutgoingPath(m_szMESOutgoingPath);
		SaveData();
		bReturn = TRUE;
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SetMESTimeout(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMESConnector::Instance()->SetMESTimeout(m_lMESTimeout);
	SaveData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


// For Rotating the wafer
LONG CWaferTable::AutoRotateWaferT1(BOOL bKeepBarcodeAngle)
{
	INT nBarcodePos = 0;
	LONG lEncT = 0;

	GetEncoderValue();	
	if (bKeepBarcodeAngle == TRUE)
	{
		nBarcodePos = GetCurrT1();
	}

	LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);

	INT nPos = GetAutoWaferT();
	if (CMS896AApp::m_bMS100Plus9InchOption && lEnableEngageOffset != 0)	//MS100 9Inch	//v4.20		//LOAD/UNLOAD TY pos is not at ENC=0
	{
		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		int nPosnT = (int) pApp->GetProfileInt(gszPROFILE_SETTING, _T("WTEngagePositionT"), 0);

		if (nPosnT > lMaxPos)
			nPosnT = 0;
		if (nPosnT < lMinPos)
			nPosnT = 0;

		lEncT = nPosnT + nPos + nBarcodePos;
	}
	else if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.45T1
	{
		lEncT = nPos + nBarcodePos;
	}
	else
	{
		lEncT = nPos + nBarcodePos;
	}

	if( IsMS90() )
	{
		XY_SafeMoveTo(0, 0);
	}
	LONG lDist = lEncT - GetCurrT1();
	LONG lRotateTime = T1_ProfileTime(m_nProfile_T, lDist, lDist);
	T1_MoveTo(lEncT, SFM_NOWAIT);	// Move table 4.24TX

	AutoCheckWaferRotation(lEncT);	//v4.35T2

	return lRotateTime;
}

LONG CWaferTable::AutoRotateWaferT1ForBC(BOOL bKeepBarcodeAngle)
{
	INT nBarcodePos = 0;
	LONG lEncT = 0;

	GetEncoderValue();	
	if (bKeepBarcodeAngle == TRUE)
	{
		nBarcodePos = GetCurrT1();
	}

	LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);

	INT nPos = (INT)((90 * m_ucAutoBCRotation) / m_dThetaRes);
	if (CMS896AApp::m_bMS100Plus9InchOption && lEnableEngageOffset != 0)	//MS100 9Inch	//v4.20		//LOAD/UNLOAD TY pos is not at ENC=0
	{
		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		int nPosnT = (int) pApp->GetProfileInt(gszPROFILE_SETTING, _T("WTEngagePositionT"), 0);

		if (nPosnT > lMaxPos)
			nPosnT = 0;
		if (nPosnT < lMinPos)
			nPosnT = 0;

		if (m_ucAutoBCRotation == 3)				//270 degree = -90 degree		//v4.43T9
		{
			nPos = (INT)((90 * -1) / m_dThetaRes);
		}

		lEncT = nPosnT + nPos + nBarcodePos;
	}
	else if (CMS896AApp::m_bMS100Plus9InchOption)
	{
		if (m_ucAutoBCRotation == 3)				//270 degree = -90 degree
		{
			nPos = (INT)((90 * -1) / m_dThetaRes);
		}
		lEncT = nPos + nBarcodePos;
	}
	else
	{
		lEncT = nPos + nBarcodePos;
	}

	LONG lDist = lEncT - GetCurrT1();
	LONG lRotateTime = T1_ProfileTime(m_nProfile_T, lDist, lDist);
	T1_MoveTo(lEncT, SFM_NOWAIT);	// Move table 4.24TX

	AutoCheckWaferRotationForBC(lEncT);
	return lRotateTime;
}

LONG CWaferTable::AutoRotateWafer(IPC_CServiceMessage& svMsg)
{
	BOOL bKeepBarcodeAngle = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bKeepBarcodeAngle);

	LONG lRotateTime = AutoRotateWaferT1(bKeepBarcodeAngle);
	svMsg.InitMessage(sizeof(LONG), &lRotateTime);
	return 1;
}

LONG CWaferTable::AutoRotateWaferForBC(IPC_CServiceMessage& svMsg)
{
	BOOL bKeepBarcodeAngle = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bKeepBarcodeAngle);

	LONG lRotateTime = AutoRotateWaferT1ForBC(bKeepBarcodeAngle);
	svMsg.InitMessage(sizeof(LONG), &lRotateTime);
	return 1;
}

BOOL CWaferTable::AutoCheckWaferRotation(LONG lEncT)
{
	if (!m_fHardware)
		return TRUE;
	if (m_bDisableWT)
		return TRUE;
	if (!m_bSel_T)
		return TRUE;

	BOOL bIsWithinLimitT = TRUE;

	LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
	LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG	lMaxOffsetT = (LONG) (46.0 / m_dThetaRes);		//Normal wafer angle max is 15 degree
	LONG lWaferRotationT = 0;
	LONG lMaxT=0, lMinT=0;
	CString szErr;

	LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);	//v4.37T1
	LONG lEngagePosnT = 0;
	if ( CMS896AApp::m_bMS100Plus9InchOption && (lEnableEngageOffset != 0) )	
		lEngagePosnT = (LONG) pApp->GetProfileInt(gszPROFILE_SETTING, _T("WTEngagePositionT"), 0);


	lWaferRotationT = GetAutoWaferT();
	lMaxT = lEngagePosnT + lWaferRotationT + lMaxOffsetT;
	lMinT = lEngagePosnT + lWaferRotationT - lMaxOffsetT;


	if( (lEncT<lMinPos && lMinPos<0) || (lEncT>lMaxPos && lMaxPos>0) )
	{
		bIsWithinLimitT = FALSE;
		szErr.Format("WT: auto check wafer T hits sw limits: curr EncT = %ld (%ld %ld)", lEncT, lMinPos, lMaxPos);
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr, "Check Wafer Angle");
	}

	if ( (lEncT < lMinT) || (lEncT > lMaxT) )
	{
		bIsWithinLimitT = FALSE;
		szErr.Format("WT: auto check wafer T fails: curr EncT = %ld (%ld %ld %ld); Rot=%d", lEncT, lMinT, lEngagePosnT,  lMaxT, m_ucAutoWaferRotation);
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr, "Check Wafer Angle");
	}

	if (bIsWithinLimitT)
	{
		szErr.Format("WT: auto check wafer T OK: curr EncT = %ld (%ld %ld); Rot=%d", lEncT, lMinT, lMaxT, m_ucAutoWaferRotation);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
	}

	return bIsWithinLimitT;
}

BOOL CWaferTable::AutoCheckWaferRotationForBC(LONG lEncT)
{
	if (!m_fHardware)
		return TRUE;
	if (m_bDisableWT)
		return TRUE;
	if (!m_bSel_T)
		return TRUE;

	BOOL bIsWithinLimitT = TRUE;

	LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
	LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG	lMaxOffsetT = (LONG) (46.0 / m_dThetaRes);		//Normal wafer angle max is 15 degree
	LONG lWaferRotationT = 0;
	LONG lMaxT=0, lMinT=0;
	CString szErr;

	LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);	//v4.37T1
	LONG lEngagePosnT = 0;
	if ( CMS896AApp::m_bMS100Plus9InchOption && (lEnableEngageOffset != 0) )	
		lEngagePosnT = (LONG) pApp->GetProfileInt(gszPROFILE_SETTING, _T("WTEngagePositionT"), 0);


	switch (m_ucAutoBCRotation)
	{
	case 1:		//90 degree
		lWaferRotationT = (LONG) (90.0 / m_dThetaRes);
		break;

	case 2:		//180 degree
		lWaferRotationT = (LONG) (180.0 / m_dThetaRes);
		break;

	case 3:		//270 degree
		lWaferRotationT = (LONG) (270.0 / m_dThetaRes);
		if ( CMS896AApp::m_bMS100Plus9InchOption  && (m_ucAutoBCRotation == 3) )	//270 degree = -90 degree		//v4.43T9
		{
			lWaferRotationT = (LONG) (90.0 * -1.0 / m_dThetaRes);
		}
		break;
	
	case 0:		//0 degree
	default:
		lWaferRotationT = 0;
		break;
	}
	lMaxT = lEngagePosnT + lWaferRotationT + lMaxOffsetT;
	lMinT = lEngagePosnT + lWaferRotationT - lMaxOffsetT;


	if( (lEncT<lMinPos && lMinPos<0) || (lEncT>lMaxPos && lMaxPos>0) )
	{
		bIsWithinLimitT = FALSE;
		szErr.Format("WT: auto check wafer T (BC) hits sw limits: curr EncT = %ld (%ld %ld)", lEncT, lMinPos, lMaxPos);
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr, "Check Wafer Angle");
	}

	if ( (lEncT < lMinT) || (lEncT > lMaxT) )
	{
		bIsWithinLimitT = FALSE;
		szErr.Format("WT: auto check wafer T (BC) fails: curr EncT = %ld (%ld %ld %ld); Rot=%d", lEncT, lMinT, lEngagePosnT,  lMaxT, m_ucAutoWaferRotation);
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr, "Check Wafer Angle");
	}

	if (bIsWithinLimitT)
	{
		szErr.Format("WT: auto check wafer T OK: curr EncT = %ld (%ld %ld); Rot=%d", lEncT, lMinT, lMaxT, m_ucAutoWaferRotation);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
	}

	return bIsWithinLimitT;
}

//================================================================
// Test()
//   Created-By  : Andrew Ng
//   Date        : 5/8/2009 11:51:39 AM
//   Description : 
//   Remarks     : 
//================================================================
LONG CWaferTable::Test(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;

	HmiMessage("Create DUMMY wafer map for BURNIN ...");

	CreateBurnInMapWith2Grades(104, 40, 1, 2);

	HmiMessage("Done");

	/*
	CString szMsg;
	CString szNewName = "D55735P-09";
	szMsg.Format("WT: unable to Load map from SECS: map = %s", szNewName);
SetAlert_Msg_Red_Yellow(IDS_SEC_WL_UPLOADMAP_ERR, szMsg, "CLOSE");		//v4.59A16
*/
	/*
	//v4.40T9
	if (m_WaferMapWrapper.IsMapValid())
	{
		ULONG ulRow, ulCol;
		LONG lUserRow, lUserCol;

		m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);
		ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
		ULONG ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
		UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol) - m_WaferMapWrapper.GetGradeOffset();
		BOOL bRef = m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol);

		BOOL bDefective = m_WaferMapWrapper.GetReader()->IsDefectiveDie(ulRow, ulCol);

		CString szMsg;
		szMsg.Format("Die at U(%ld, %ld) I(%lu, %lu); State=%lu, Grade=%d, Ref=%d, Def=%d", 
			lUserRow, lUserCol, ulRow, ulCol, ulDieState, ucGrade, bRef, bDefective);
		HmiMessage(szMsg);
	}*/

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SwitchBlockFuncPage(IPC_CServiceMessage& svMsg)	//Block2
{
	BOOL bReturn=TRUE;

	m_bBlkFuncPage	= TRUE;
	m_bBlkFuncPage2 = FALSE;

	if (m_bFullRefBlock == TRUE)
	{
		m_bBlkFuncPage	= FALSE;
		m_bBlkFuncPage2 = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//v3.44		//ShowaDenko
LONG CWaferTable::CalculateCOROffset(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	DOUBLE dA1=0, dA2=0, dB1=0, dB2=0;
	DOUBLE daA1=0, daA2=0, daB1=0, daB2=0;


	if (!m_bUseMultiCorOffset)
	{
		HmiMessage("Error: mutlti-COR Offset fcn not enabled!");

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//1. Open COR file
	CStdioFile oFile;
	if (!oFile.Open(_T("c:\\Mapsorter\\cor.txt"), 
		CFile::modeNoTruncate|CFile::modeRead|CFile::typeText))
	{
		HmiMessage("Error: cannot open COR file for reading! (c:\\Mapsorter\\cor.txt)");

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	CString szLine;
	CString szItem1, szItem2;
	DOUBLE dDx1=0, dDx2=0;
	DOUBLE dy1=0, dy2=0;
	DOUBLE dDy1=0, dDy2=0;
	DOUBLE dx1=0, dx2=0;

	DOUBLE daDx1=0, daDx2=0;
	DOUBLE day1=0, day2=0;
	DOUBLE daDy1=0, daDy2=0;
	DOUBLE dax1=0, dax2=0;


	oFile.SeekToBegin();
	
	oFile.ReadString(szLine);
	szItem1 = szLine.Left(szLine.Find(","));
	szLine	= szLine.Mid(szLine.Find(",") + 1);
	szItem2 = szLine.Left(szLine.Find(","));
	dDx1	= atof(szItem1);
	dy1		= atof(szItem2);
	
	oFile.ReadString(szLine);
	szItem1 = szLine.Left(szLine.Find(","));
	szLine	= szLine.Mid(szLine.Find(",") + 1);
	szItem2 = szLine.Left(szLine.Find(","));
	dDx2	= atof(szItem1);
	dy2		= atof(szItem2);


	oFile.ReadString(szLine);
	szItem1 = szLine.Left(szLine.Find(","));
	szLine	= szLine.Mid(szLine.Find(",") + 1);
	szItem2 = szLine.Left(szLine.Find(","));
	daDx1	= atof(szItem1);
	day1	= atof(szItem2);

	oFile.ReadString(szLine);
	szItem1 = szLine.Left(szLine.Find(","));
	szLine	= szLine.Mid(szLine.Find(",") + 1);
	szItem2 = szLine.Left(szLine.Find(","));
	daDx2	= atof(szItem1);
	day2		= atof(szItem2);


	oFile.ReadString(szLine);
	szItem1 = szLine.Left(szLine.Find(","));
	szLine	= szLine.Mid(szLine.Find(",") + 1);
	szItem2 = szLine.Left(szLine.Find(","));
	dDy1	= atof(szItem1);
	dx1		= atof(szItem2);

	oFile.ReadString(szLine);
	szItem1 = szLine.Left(szLine.Find(","));
	szLine	= szLine.Mid(szLine.Find(",") + 1);
	szItem2 = szLine.Left(szLine.Find(","));
	dDy2	= atof(szItem1);
	dx2		= atof(szItem2);


	oFile.ReadString(szLine);
	szItem1 = szLine.Left(szLine.Find(","));
	szLine	= szLine.Mid(szLine.Find(",") + 1);
	szItem2 = szLine.Left(szLine.Find(","));
	daDy1	= atof(szItem1);
	dax1		= atof(szItem2);

	oFile.ReadString(szLine);
	szItem1 = szLine.Left(szLine.Find(","));
	szLine	= szLine.Mid(szLine.Find(",") + 1);
	szItem2 = szLine.Left(szLine.Find(","));
	daDy2	= atof(szItem1);
	dax2	= atof(szItem2);


	oFile.Close();


CString szTemp;
szTemp.Format("1 =  (%.1f, %.1f)   (%.1f, %.1f)", dDx1, dy1, dDx2, dy2);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
szTemp.Format("1A =  (%.1f, %.1f)   (%.1f, %.1f)", daDx1, day1, daDx2, day2);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

szTemp.Format("2 =  (%.1f, %.1f)   (%.1f, %.1f)", dDy1, dx1, dDy2, dx2);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
szTemp.Format("2A =  (%.1f, %.1f)   (%.1f, %.1f)", daDy1, dax1, daDy2, dax2);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);


	//2. Calculate A1 & B1 for dX
	if ((dy1 - dy2) == 0)
		dA1 = 0;
	else
		dA1 = (dDx1 - dDx2) / (dy1 - dy2);

	if ((dy2 - dy1) == 0)
		dB1 = 0;
	else
		dB1 = (dy2*dDx1 - dy1*dDx2) / (dy2 - dy1);


	//3. Calculate aA1 & aB1 for dX
	if ((day1 - day2) == 0)
		daA1 = 0;
	else
		daA1 = (daDx1 - daDx2) / (day1 - day2);

	if ((day2 - day1) == 0)
		daB1 = 0;
	else
		daB1 = (day2*daDx1 - day1*daDx2) / (day2 - day1);


	//4. Calculate A2 & B2 for dY
	if ((dx1 - dx2) == 0)
		dA2 = 0;
	else
		dA2 = (dDy1 - dDy2) / (dx1 - dx2);

	if ((dx2 - dx1) == 0)
		dB2 = 0;
	else
		dB2 = (dx2*dDy1 - dx1*dDy2) / (dx2 - dx1);


	//5. Calculate aA2 & aB2 for dY
	if ((dax1 - dax2) == 0)
		daA2 = 0;
	else
		daA2 = (daDy1 - daDy2) / (dax1 - dax2);

	if ((dax2 - dax1) == 0)
		daB2 = 0;
	else
		daB2 = (dax2*daDy1 - dax1*daDy2) / (dax2 - dax1);


	m_dCorOffsetA1			= dA1;
	m_dCorOffsetA2			= dA2;
	m_dCorOffsetB1			= dB1;
	m_dCorOffsetB2			= dB2;

	m_dACorOffsetA1			= daA1;
	m_dACorOffsetA2			= daA2;
	m_dACorOffsetB1			= daB1;
	m_dACorOffsetB2			= daB2;

	SaveWaferTblData();

szTemp.Format("Result =  (%.5f, %.5f)   (%.1f, %.1f)", dA1, dB1, dA2, dB2);
AfxMessageBox(szTemp, MB_SYSTEMMODAL);
szTemp.Format("Result A =  (%.5f, %.5f)   (%.1f, %.1f)", daA1, daB1, daA2, daB2);
AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	HmiMessage("COR Offset done.");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::RemoveLotInfoFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	DeleteFile(MSD_LOT_INFO_FILE);
	DeleteFile(MSD_LOT_INFO_BKF);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferTable::ResetAlignStatus(IPC_CServiceMessage &svMsg)
{
	SetAlignmentStatus(FALSE);
	m_WaferMapWrapper.DeleteAllBoundary();

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}


//Generate Log file when missing reference is detected during auto-bond		//block2
BOOL CWaferTable::ExportRefCheckFailFile(IPC_CServiceMessage &svMsg)	
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( IsBlkFunc2Enable() && pApp->GetCustomerName()=="Cree" )
	{
		LONG lX = (*m_psmfSRam)["DieInfo"]["RefDie"]["ulX"];
		LONG lY = (*m_psmfSRam)["DieInfo"]["RefDie"]["ulY"];
		m_pBlkFunc2->Blk2ExportRefCheckFailLog(lX, lY);
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

BOOL CWaferTable::ManualDisplaySemitekMapName(BOOL bFullyAuto)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp->GetCustomerName() != _T("Semitek") || pApp->m_bCycleStarted )		//v4.02T4
	{
		return TRUE;
	}

	if( pApp->GetProductLine()=="ZJG" )
	{
		return TRUE;
	}

	CString strTmp2;

	strTmp2 = GetMapNameOnly();
	strTmp2 = strTmp2.MakeUpper();

	if (bFullyAuto)
	{
		HmiMessage(strTmp2, "Map File Name"); 
		return TRUE;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szBarcode = _T("");
	CString szTitle = "Please input barcode (" + strTmp2 + ")";
	if( pUtl->GetPrescanDummyMap() )
	{
		szTitle = "Please input barcode on wafer.";
	}

	if( HmiStrInputKeyboard(szTitle, szBarcode)==FALSE )
	{
		return FALSE;
	}

	CString szWaferID = szBarcode;
	if ( m_bEnablePrefix && !m_bEnablePrefixCheck && (m_szPrefixName.IsEmpty() == FALSE) )		//Add filename prefix if necessary
	{
		szWaferID = m_szPrefixName + szWaferID;
	}
	if ( m_bEnableSuffix && !m_bEnableSuffixCheck && (m_szSuffixName.GetLength() > 0) )
	{
		szWaferID = szWaferID + m_szSuffixName;
	}

	if( pUtl->GetPrescanDummyMap() )
	{
		m_szDummyWaferMapName = szWaferID;
		m_szMapFileName = szWaferID;
	}
	else
	{
		if( szWaferID.CompareNoCase(strTmp2)!=0 )
		{
			return FALSE;
		}
	}

	return TRUE;
}

// only for auto sorting mode
LONG CWaferTable::MoveWaferTableToSafePosn2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSafe = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bSafe);

	X_Sync();
	Y_Sync();
	T_Sync();

	if (m_bDisableWT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (XY_IsPowerOff())
	{
		HmiMessage_Red_Yellow("ERROR: wafer table motor not power on!");
		SetErrorMessage("ERROR: wafer table motor not power on!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsEnableWL() && IsWLExpanderOpen())
	{
		SetErrorMessage("Expander not closed");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (bSafe)
	{
		GetEncoder(&m_lBpX, &m_lBpY, &m_lBpT);
		if (m_lBpY < -150000)
		{
			XY_SafeMoveTo(0, 0);
		}
	}
	else
	{
		if (XY_SafeMoveTo(m_lBpX, m_lBpY))
		{
		}
	}
	Sleep(100);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::StartWTXMotionTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct {
		BOOL	bIsStart;
		LONG	lDistance;
		LONG	lDelay;
	} MOTION_TEST;

	MOTION_TEST stData;

	svMsg.GetMsg(sizeof(MOTION_TEST), &stData);

	if (stData.bIsStart == TRUE)
	{
		if (MoveBackLightToSafeLevel() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (MoveEjectorElevatorToSafeLevel() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		SetJoystickOn(FALSE);
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(WT_AXIS_X, &m_stWTAxis_X, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(WT_AXIS_X, &m_stWTAxis_X, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestWaferTableX = stData.bIsStart;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::StartWTYMotionTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct {
		BOOL	bIsStart;
		LONG	lDistance;
		LONG	lDelay;
	} MOTION_TEST;

	MOTION_TEST stData;

	svMsg.GetMsg(sizeof(MOTION_TEST), &stData);

	if (stData.bIsStart == TRUE)
	{
		if (MoveBackLightToSafeLevel() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (MoveEjectorElevatorToSafeLevel() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		SetJoystickOn(FALSE);
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(WT_AXIS_Y, &m_stWTAxis_Y, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(WT_AXIS_Y, &m_stWTAxis_Y, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestWaferTableY = stData.bIsStart;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::StartWT2XMotionTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct {
		BOOL	bIsStart;
		LONG	lDistance;
		LONG	lDelay;
	} MOTION_TEST;

	MOTION_TEST stData;

	svMsg.GetMsg(sizeof(MOTION_TEST), &stData);

	if (stData.bIsStart == TRUE)
	{
		if (MoveBackLightToSafeLevel() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (MoveEjectorElevatorToSafeLevel() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		SetJoystickOn(FALSE);
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(WT_AXIS_X2, &m_stWTAxis_X2, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(WT_AXIS_X2, &m_stWTAxis_X2, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestWaferTable2X = stData.bIsStart;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::StartWT2YMotionTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct {
		BOOL	bIsStart;
		LONG	lDistance;
		LONG	lDelay;
	} MOTION_TEST;

	MOTION_TEST stData;

	svMsg.GetMsg(sizeof(MOTION_TEST), &stData);

	if (stData.bIsStart == TRUE)
	{
		if (MoveBackLightToSafeLevel() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (MoveEjectorElevatorToSafeLevel() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		SetJoystickOn(FALSE);
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(WT_AXIS_Y2, &m_stWTAxis_Y2, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(WT_AXIS_Y2, &m_stWTAxis_Y2, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestWaferTable2Y = stData.bIsStart;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::XY_SafeMoveToCmd(IPC_CServiceMessage& svMsg)
{
	WT_XY_STRUCT stPos;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(WT_XY_STRUCT), &stPos);

	if (XY_IsPowerOff())
	{
		HmiMessage("Wafer Table XY not power ON!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( XY_SafeMoveTo(stPos.lX, stPos.lY) )
	{
		bReturn = TRUE;
	}
	else
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

//v4.16T5
LONG CWaferTable::XY_SafeMoveToUnloadCmd(IPC_CServiceMessage& svMsg)
{
	WT_XY_UNLOAD_STRUCT stPos;
	BOOL bReturn = TRUE;
	INT nReturn = gnOK;

	svMsg.GetMsg(sizeof(WT_XY_UNLOAD_STRUCT), &stPos);
	
	if (XY_IsPowerOff())
	{
		HmiMessage("Wafer Table XY not power ON!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.16T3	//MS100 9Inch
	{
		if (stPos.lUnload > 0)		//to UNLOAD
		{
			X1_Profile(LOW_PROF1);
			Y1_Profile(LOW_PROF1);

			bReturn = XY1_MoveTo(0, 0);
			SetEJTSlideUpDn(FALSE);							//Need EJT table to slide down
			Sleep(500);
			bReturn = XY1_MoveTo(stPos.lX, stPos.lY);		//Then move Y to UNLOADY
			
			X1_Profile(NORMAL_PROF);
			Y1_Profile(NORMAL_PROF);
		}
		else						//to HOME
		{
			X1_Profile(LOW_PROF1);
			Y1_Profile(LOW_PROF1);
			bReturn = XY1_MoveTo(0, 0);
			X1_Profile(NORMAL_PROF);
			Y1_Profile(NORMAL_PROF);

			SetEJTSlideUpDn(TRUE);							//Need EJT table to slide up to HOME
			Sleep(100);
		}
		bReturn = TRUE;
	}
	else if ( m_bUseDualTablesOption )	//v4.24T9	//ES101
	{
		if (stPos.lUnload <= 0)		//to LOAD
		{
			HomeTable2();
		}

		LONG lX = stPos.lX;
		LONG lY = stPos.lY;

		if ( (lX < m_lXNegLimit) || (lX > m_lXPosLimit) )
		{
			CString szErr;
			szErr.Format("X exceeds limit: %ld (%ld, %ld)", lX, m_lXNegLimit, m_lXPosLimit);
			AfxMessageBox(szErr, MB_SYSTEMMODAL);
		}

		if ( (lY < m_lYNegLimit) || (lY > m_lYPosLimit) )
		{
			CString szErr;
			szErr.Format("Y exceeds limit: %ld (%ld, %ld)", lY, m_lYNegLimit, m_lYPosLimit);
			AfxMessageBox(szErr, MB_SYSTEMMODAL);
		}

		X1_Profile(LOW_PROF1);
		Y1_Profile(LOW_PROF1);
		nReturn = XY1_MoveTo(stPos.lX, stPos.lY);
		X1_Profile(NORMAL_PROF);
		Y1_Profile(NORMAL_PROF);

		if (nReturn != gnOK)
		{
			bReturn = FALSE;
			CString szErr;
			szErr.Format("WT: XY1_MoveTo returns FALSE !!!  %d", nReturn);
			SetErrorMessage(szErr);
		}
		else
		{
			bReturn = TRUE;
		}
	}
	else
	{
		bReturn = XY_SafeMoveTo(stPos.lX, stPos.lY);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SetEJTSlideUpCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	CString szMsg;
	INT nReturn = 0;

	if (!IsEnableWL())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (!CMS896AApp::m_bMS100Plus9InchOption)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//v4.42T4
	if (bOn == FALSE)	//DOWN
		szMsg = "Move expander to UNLOAD?";
	else
		szMsg = "Move expander to HOME?";

	if( HmiMessage(szMsg, "EJT", glHMI_MBX_YESNO) != glHMI_YES )
	{
		//TRUE=UP; FALSE=DOWN(Unload)
		SetEJTSlideUpDn(bOn);
	}
	else
	{
		if (bOn == FALSE)	//DOWN
		{
			X1_Profile(LOW_PROF1);
			Y1_Profile(LOW_PROF1);

			if (XY1_MoveTo(0, 0, SFM_WAIT) == gnOK)	
			{
				SetEJTSlideUpDn(FALSE);
				Sleep(500);

				//v4.42T6
				BOOL bAtUnload = FALSE;
				for(int i=0; i<30; i++)
				{
                    if (!IsEJTAtUnloadPosn())
					{
						Sleep(1000);
					}
					else
					{
						bAtUnload = TRUE;
						break;
					}
				}
				if( bAtUnload==FALSE )
				{
					//SetAlert_Red_Yellow(IDS_WT_BL_NOT_IN_SAFE_POS);
					HmiMessage_Red_Back("Ejector isnt in a safe position");
					SetErrorMessage("SetEJTSlideUpCmd: EJT not at UNLOAD position (MS109)");
					X1_Profile(NORMAL_PROF);
					Y1_Profile(NORMAL_PROF);	
					bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}

				//szLog.Format("WT: XY MoveToUnloadCmd to UNLOAD at %ld, %ld", stPos.lX, stPos.lY);
				//CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
				nReturn = XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
			}
			else
				nReturn = FALSE;

			X1_Profile(NORMAL_PROF);
			Y1_Profile(NORMAL_PROF);	
			m_bIsUnload = TRUE;
		}
		else
		{
			X1_Profile(LOW_PROF1);
			Y1_Profile(LOW_PROF1);
			XY1_MoveTo(0, 0, SFM_WAIT);
			X1_Profile(NORMAL_PROF);
			Y1_Profile(NORMAL_PROF);
			SetEJTSlideUpDn(TRUE);
			m_bIsUnload = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::PkgKeyParametersTask(IPC_CServiceMessage &svMsg)	
{
	CStdioFile kFile;
	BOOL bReturn = TRUE;
	LONG lAction = 0;
	svMsg.GetMsg(sizeof(LONG), &lAction);

	BOOL	bEnableSuffix			= m_bEnableSuffix;
	BOOL	bCheckDieOrientation	= m_bCheckDieOrientation;
	ULONG	ulMinGradeCount			= m_ulMinGradeCount;
	CString	szSuffixName			= m_szSuffixName;
	CString	szOutputFormatName		= m_szOutputFormatFileName;
	CString	szMapHeaderFileName		= m_szMapHeaderFileName;

	if( lAction==0 || lAction==1 )
	{
		if( lAction==1 )
		{
			CStringMapFile psmf;
			CString szNewMsdName = "C:\\MapSorter\\UserData\\LastState.msd";
			if ( psmf.Open(szNewMsdName, FALSE, FALSE) )
			{
				bEnableSuffix			= (BOOL)(LONG)(psmf)[WT_OPTION][WT_MAP_ENABLE_SUFFIX];
				bCheckDieOrientation	= (BOOL)(LONG)(psmf)[WT_OPTION][WT_CHECK_DIE_ORIENTATION];
				ulMinGradeCount			= (ULONG)(psmf)[WT_OPTION][WT_MAP_MIN_GRADE_COUNT];
				szSuffixName			= (psmf)[WT_OPTION][WT_MAP_SUFFIX_NAME];
				szOutputFormatName		= (psmf)[WT_OUTPUTFILE_OPTION][WT_OUTPUTFORMAT_FILE_NAME];
				szMapHeaderFileName		= (psmf)[WT_OUTPUTFILE_OPTION][WT_MAPHEADER_FILE_NAME];
			}
			psmf.Close();
			DeleteFile(szNewMsdName);
		}

		if( kFile.Open(gszLocalPkgCheckListFile, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText) )
		{
			kFile.SeekToEnd();
			CString szText;
			szText.Format("%s",		"WFT list begin");			kFile.WriteString(szText + "\n");
			szText.Format("%d",		bEnableSuffix);				kFile.WriteString(szText + "\n");
			szText.Format("%d",		bCheckDieOrientation);		kFile.WriteString(szText + "\n");
			szText.Format("%lu",	ulMinGradeCount);			kFile.WriteString(szText + "\n");
			szText.Format("%s",		szSuffixName);				kFile.WriteString(szText + "\n");
			szText.Format("%s",		szOutputFormatName);		kFile.WriteString(szText + "\n");
			szText.Format("%s",		szMapHeaderFileName);		kFile.WriteString(szText + "\n");
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
				if( szText.Find("WFT list begin")!=-1 )
				{
					break;
				}
			}

			if( kFile.ReadString(szText) )
			{
				bEnableSuffix = atoi(szText);
			}
			if( kFile.ReadString(szText) )
			{
				bCheckDieOrientation = atoi(szText);
			}
			if( kFile.ReadString(szText) )
			{
				ulMinGradeCount = atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				szSuffixName = szText;
			}
			if( kFile.ReadString(szText) )
			{
				szOutputFormatName = szText;
			}
			if( kFile.ReadString(szText) )
			{
				szMapHeaderFileName = szText;
			}
			kFile.Close();

			if( bEnableSuffix != m_bEnableSuffix )
			{
				szText.Format("Enable Suffix changed to %d(%d)", bEnableSuffix, m_bEnableSuffix);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( bCheckDieOrientation != m_bCheckDieOrientation )
			{
				szText.Format("Check Die Orientation changed to %d(%d)", bCheckDieOrientation, m_bCheckDieOrientation);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( ulMinGradeCount != m_ulMinGradeCount )
			{
				szText.Format("Min Grade Count changed to %lu(%lu)", ulMinGradeCount, m_ulMinGradeCount);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( szSuffixName != m_szSuffixName )
			{
				szText.Format("Suffix Name changed to %s(%s)", szSuffixName, m_szSuffixName);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( szOutputFormatName != m_szOutputFormatFileName )
			{
				szText.Format("Output Format Name changed to %s(%s)", szOutputFormatName, m_szOutputFormatFileName);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( szMapHeaderFileName != m_szMapHeaderFileName )
			{
				szText.Format("Map Header File Name changed to %s(%s)", szMapHeaderFileName, m_szMapHeaderFileName);
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
			HmiMessage(szListMsg, "WT Parameter Check");
			SetAlarmLamp_Yellow(FALSE, TRUE);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GeneratePkgDataFile(IPC_CServiceMessage &svMsg)	
{
	BOOL bReturn = TRUE;
	CString szLine;

	CString szLocalPkgDataFileName = (*m_psmfSRam)["MS896A"]["PKG Local Data File Name"];

	CStdioFile oFile;
	if (oFile.Open(szLocalPkgDataFileName, 
					CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText) != TRUE)
	{
		HmiMessage("ERROR: unable to open package data file - " + szLocalPkgDataFileName);
		return FALSE;
	}

	oFile.SeekToEnd();

	szLine = "HomeDie WaferMap Position Setup:\n";
	oFile.WriteString(szLine);

	szLine.Format(",Row No:,%ld\n",					m_lHomeDieMapRow);
	oFile.WriteString(szLine);
	szLine.Format(",Col No:,%ld\n",					m_lHomeDieMapCol);
	oFile.WriteString(szLine);

	szLine.Format("Map Orientation (Deg):,%ld\n",	m_ucMapRotation * 90);
	oFile.WriteString(szLine);

	oFile.Close();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::GenerateConfigData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMSLogFileUtility::Instance()->MS_LogOperation("WT: Generate Configuration Data");
	bReturn = GenerateConfigData();
	CMSLogFileUtility::Instance()->MS_LogOperation("WT: Generate Configuration Data done");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}



LONG CWaferTable::SetWaferClampOnOff(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "Walsin")
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	CString szLog;
	szLog.Format("WT: manual SetWaferClampOnOff - %d", bOn);
HmiMessage(szLog);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	
	SetWaferClamp(bOn);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::IsExpanderClosed(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if ( IsWLExpanderOpen() == TRUE )
	{
		bReturn = FALSE;
		SetErrorMessage("Expander not closed");
	}

	//v4.43T13
	if (CMS896AApp::m_bMS100Plus9InchOption)	//MS109
	{
		if (IsEJTAtUnloadPosn())
		{
			bReturn = FALSE;
			SetErrorMessage("MS109 EJT detected at UNLOAD pos");
			//HmiMessage_Red_Yellow("EJT Table detected at UNLOAD position; operation is aborted.");
			SetAlert_Red_Yellow(IDS_WT_EJT_NOT_SAFE);		//v4.59A22	//40035
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// ES101 WFT1 slow move with wait motion complete option
LONG CWaferTable::XY1_MoveToHomeLoadCmd(IPC_CServiceMessage& svMsg)
{
	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG nMode;
	} WT_XY_HOME_LOAD_STRUCT;
	WT_XY_HOME_LOAD_STRUCT stEsPos;

	svMsg.GetMsg(sizeof(WT_XY_HOME_LOAD_STRUCT), &stEsPos);
	LONG lX = stEsPos.lX;
	LONG lY = stEsPos.lY;
	INT nMode = stEsPos.nMode;
	
	BOOL bResult = FALSE;
	if (XY_IsPowerOff())
	{
		HmiMessage("Wafer Table XY not power ON!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	SetJoystickOn(FALSE);

	if ( (lX < m_lXNegLimit) || (lX > m_lXPosLimit) )
	{
		CString szErr;
		szErr.Format("WT: X exceeds WT1 limit in XY1 MoveToHomeLoadCmd: %ld (%ld, %ld)", lX, m_lXNegLimit, m_lXPosLimit);
		SetErrorMessage(szErr);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
	}

	if ( (lY < m_lYNegLimit) || (lY > m_lYPosLimit) )
	{
		CString szErr;
		szErr.Format("WT: Y exceeds WT1 limit in XY1 MoveToHomeLoadCmd: %ld (%ld, %ld)", lY, m_lYNegLimit, m_lYPosLimit);
		SetErrorMessage(szErr);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
	}

	X1_Profile(LOW_PROF1);
	Y1_Profile(LOW_PROF1);
	bResult = (XY1_MoveTo(lX, lY, nMode) == gnOK);					
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

// ES101 WFT2 slow move with wait motion complete option
LONG CWaferTable::XY2_MoveToHomeLoadCmd(IPC_CServiceMessage& svMsg)
{
	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG nMode;
	} WT_XY_HOME_LOAD_STRUCT;
	WT_XY_HOME_LOAD_STRUCT stEsPos;

	svMsg.GetMsg(sizeof(WT_XY_HOME_LOAD_STRUCT), &stEsPos);
	LONG lX = stEsPos.lX;
	LONG lY = stEsPos.lY;
	INT nMode = stEsPos.nMode;

	BOOL bResult = FALSE;
	if( IsESDualWT()==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	SetJoystickOn(FALSE);

	if (XY_IsPowerOff())
	{
		HmiMessage_Red_Back("Wafer Table XY not power ON!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if ( (lX < m_lX2NegLimit) || (lX > m_lX2PosLimit) )
	{
		CString szErr;
		szErr.Format("WT: X2 exceeds WT2 limit in XY2 MoveToHomeLoadCmd: %ld (%ld, %ld)", lX, m_lX2NegLimit, m_lX2PosLimit);
		SetErrorMessage(szErr);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
	}
	if ( (lY < m_lY2NegLimit) || (lY > m_lY2PosLimit) )
	{
		CString szErr;
		szErr.Format("WT: Y2 exceeds WT2 limit in XY2 MoveToHomeLoadCmd: %ld (%ld, %ld)", lY, m_lY2NegLimit, m_lY2PosLimit);
		SetErrorMessage(szErr);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
	}

	X2_Profile(LOW_PROF1);
	Y2_Profile(LOW_PROF1);
	bResult = (XY2_MoveTo(lX, lY, nMode) == gnOK);					
	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferTable::MoveWToffset(IPC_CServiceMessage& svMsg)
{
	BOOL bResult = TRUE;

	if ( !m_bMS100EjtXY )
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	GetEncoderValue();

	typedef struct
	{
		BOOL bIsArmOne;
	} WT_XY_HOME_LOAD_STRUCT;
	WT_XY_HOME_LOAD_STRUCT stEsPos;

	svMsg.GetMsg(sizeof(WT_XY_HOME_LOAD_STRUCT), &stEsPos);

	BOOL bArmSelect = stEsPos.bIsArmOne;


	//CString szSelection1 = "Arm1";
	//CString szSelection2 = "Arm2";
	//INT nArmSelection = HmiMessage("Please choose which arm offset to move.", "Collet Offset", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);

	LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
	LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
	GetColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, (m_dXYRes * 1000), FALSE);

	if ( bArmSelect ) 
	{  // load arm 1 coor
		XY_Move(lCollet1OffsetX, lCollet1OffsetY);
	}
	else 
	{  // load arm 2 coor
		XY_Move(lCollet2OffsetX, lCollet2OffsetY);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

BOOL CWaferTable::AutoLoadMap(CString szMapFullName, CString szFormatName, const BOOL bMenuLoad)
{
	BOOL bReturn = FALSE;
	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//AfxMessageBox(pApp ->GetCustomerName() +"," +pApp ->GetProductLine()+"," + m_szUserDefineDieType );
	if (pApp ->GetCustomerName() == CTM_SANAN && pApp ->GetProductLine() == "XA" && m_szUserDefineDieType != "" &&m_szUserDefineDieType != " " && m_ulTotalSorted != 0)
	{	
		CMSLogFileUtility::Instance()->MS_LogOperation("Read SanAn LoadNumber Before Auto Load Map:" + szMapFullName);
		if (_access(szMapFullName,0) != -1)
		{
			CStdioFile cfReadMap;
			if(cfReadMap.Open(szMapFullName, CFile::modeRead|CFile::shareDenyNone))
			{
				CString szTemp;
				CString szTemp1;
				cfReadMap.ReadString(szTemp);
				cfReadMap.ReadString(szTemp);
				CMSLogFileUtility::Instance()->MS_LogOperation("Read SanAn LoadNumber Before Auto Load Map:" + szTemp + "," + m_szUserDefineDieType);
				szTemp.MakeUpper();
				szTemp1 = m_szUserDefineDieType;
				szTemp1.MakeUpper();
				if (szTemp.Find(szTemp1) == -1)
				{
					CMS896AApp::m_bMapLoadingFinish = TRUE;
					CMS896AApp::m_bMapLoadingAbort	= TRUE;
					SetErrorMessage("Check Lot Number(SANAN): fail -  " + szTemp + " - " + m_szUserDefineDieType);
					HmiMessage_Red_Yellow("Check Lot Number(SANAN): fail -  " + szTemp + " - " + m_szUserDefineDieType);
					cfReadMap.Close();
					return FALSE;
				}
				cfReadMap.Close();
			}
		}
	}

	if( pApp->GetCustomerName()==CTM_NICHIA && m_bEnableMapDMFile )
	{
		CString szDMFile = szMapFullName;
		int nFolder = szMapFullName.ReverseFind('\\');
		int nExtDot = szMapFullName.ReverseFind('.');
		if( nFolder<nExtDot )
		{
			szDMFile = szMapFullName.Left(nExtDot);
		}
		szDMFile = szDMFile + ".DM";
		DeleteFile(szDMFile);
		FILE *fp = NULL;
		errno_t nErr = fopen_s(&fp, szDMFile, "wt");
		if ((nErr == 0) && (fp != NULL))
		{
			fclose(fp);
		}
		for(INT i=0; i<300; i++)
		{
			if (_access(szMapFullName, 0) != -1)
			{
				Sleep(2000);
				break;
			}
			Sleep(1000);
		}
	}

	bReturn	= LoadWaferMap(szMapFullName, szFormatName, bMenuLoad);	// Auto load map by WL
	CString szMsg;
	if (m_bEnableLocalLoadMap)
		szMsg.Format("Auto Load lcl Map ok=%d at: ", bReturn);
	else
		szMsg.Format("Auto Load srv Map ok=%d at: ", bReturn);
	szMsg += szMapFullName;
	SetErrorMessage(szMsg);
	SetStatusMessage(szMsg);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	return bReturn;
}

//	for ES101 and ES201 to rotate theta under camera in case crash with loader gripper
LONG CWaferTable::RotateWFTTUnderCam(CONST BOOL bWT2)
{
	return TRUE;
}

//	for ES101 and ES201 to rotate theta under camera in case crash with loader gripper
LONG CWaferTable::WFTTGoHomeUnderCam(CONST BOOL bWT2)
{
	return TRUE;
}

BOOL CWaferTable::WTCheckEjtLife()
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bResult);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "CheckTheEjectorLifeTime", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);	
		}
	}

	return bResult;
}

VOID CWaferTable::GenerateTabletPicture()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "Yealy" )
		return ;

	SaveScanTimeEvent("    WFT: to generate PNG picture");
	CPreBondEventBin::m_bMapLoaded = FALSE;
	m_BinMapWrapper.InitMap();

	LONG lNoOfRows = GetMapValidMaxRow() - GetMapValidMinRow() + 1;
	LONG lNoOfCols = GetMapValidMaxCol() - GetMapValidMinCol() + 1;

	unsigned char aucAvailableGrades[1] = {'1'};
	unsigned char aaTempGrades[1];
	aucAvailableGrades[0] = 48 + 1;
	aaTempGrades[0] = 48 + 1;

	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;
	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows*lNoOfCols];
	aaGrades = new unsigned char*[lNoOfRows];

	int i, j;
	for (i=0; i<lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i];
	}

	// Set the grades
	for (i=0; i<lNoOfRows; i++) 
	{
		for (j=0; j<lNoOfCols; j++) 
		{
			aaGrades[i][j] = WAF_CMapConfiguration::DEFAULT_NULL_BIN;
		}
	}

	//Clear Wafermap & reset grade map
	CString szPngMapFile = PRESCAN_RESULT_FULL_PATH + "Png_" + GetMapNameOnly() + ".map";
	m_BinMapWrapper.CreateMap(szPngMapFile, "General", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);

//CString szLog;
//szLog.Format("Tablet map dimension %d,%d", lNoOfRows, lNoOfCols);
//SetAlarmLog(szLog);
	// pass original map data to new created bin map wrapper.
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	for (ULONG i=0; i< (ULONG)lNoOfRows; i++)
	{
		for (ULONG j=0; j< (ULONG)lNoOfCols; j++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(i+GetMapValidMinRow(), j+GetMapValidMinCol());
			if( ucGrade!=ucNullBin )
			{
				m_BinMapWrapper.AddDie(i, j, ucGrade);
			}
		}
	}

	delete [] aaGrades;
	delete [] pGradeBuffer;

	WAF_CWaferZoomWindowView *pView = new WAF_CWaferZoomWindowView;
	pView->SetReader(m_BinMapWrapper.GetReader());
	pView->Refresh();

	INT nTabletPixelX	= m_lTabletPixelX;
	INT nTabletPixelY	= m_lTabletPixelY;
	INT nImagePixelX	= m_lPngPixelX;
	INT nImagePixelY	= m_lPngPixelY;
	pView->ResetAllDrawState();

	// to get scanned map up left corner die position in theory
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	LONG lRow, lCol, lAlnX, lAlnY;
	pUtl->GetAlignPosition(lRow, lCol, lAlnX, lAlnY);
	LONG lMaxX = lAlnX, lMinX = lAlnX, lMaxY = lAlnY, lMinY = lAlnY;
	LONG lPhyX = 0, lPhyY = 0;
	for(LONG lRow=GetMapValidMinRow(); lRow<=GetMapValidMaxRow(); lRow++)
	{
		for(LONG lCol=GetMapValidMinCol(); lCol<=GetMapValidMaxCol(); lCol++)
		{
			if( GetMapPhyPosn(lRow, lCol, lPhyX, lPhyY) )
			{
				if( lMaxX<lPhyX )
					lMaxX = lPhyX;
				if( lMinX>lPhyX )
					lMinX = lPhyX;
				if( lMaxY<lPhyY )
					lMaxY = lPhyY;
				if( lMinY>lPhyY )
					lMinY = lPhyY;
			}
		}
	}

	lMaxX = lMaxX + GetDiePitchX_X()/2;
	lMinX = lMinX - GetDiePitchX_X()/2;
	lMaxY = lMaxY + GetDiePitchY_Y()/2;
	lMinY = lMinY - GetDiePitchY_Y()/2;

	for(LONG lRow=GetMapValidMinRow(); lRow<=GetMapValidMaxRow(); lRow++)
	{
		for(LONG lCol=GetMapValidMinCol(); lCol<=GetMapValidMaxCol(); lCol++)
		{
			if( GetMapPhyPosn(lRow, lCol, lPhyX, lPhyY) )
			{
				LONG lDieUmX = (LONG) ConvertMotorStepToUnit(lMaxX - lPhyX);
				LONG lDieUmY = (LONG) ConvertMotorStepToUnit(lMaxY - lPhyY);
				m_BinMapWrapper.SetPhysicalPosition(lRow-GetMapValidMinRow(), lCol-GetMapValidMinCol(), lDieUmX, lDieUmY);
			}
		}
	}

	LONG lExtraEdge = m_lPngDieExtraEdgeY;
	if ( m_szCurrentGrade == "C" )
	{
		lExtraEdge = m_lPngDieExtraEdgeN;		// NG die (X)
	}
	else if ( m_szCurrentGrade == "N" )
	{
		lExtraEdge = m_lPngDieExtraEdgeX;		// NG die (N)
	}
	//	m_lPngDieExtraEdgeE

	LONG lWaferUmX	= (LONG)ConvertMotorStepToUnit(GetDiePitchX_X()*lNoOfCols);	//	(LONG)ConvertMotorStepToUnit(lMaxX-lMinX);
	LONG lWaferUmY	= (LONG)ConvertMotorStepToUnit(GetDiePitchY_Y()*lNoOfRows);	//	(LONG)ConvertMotorStepToUnit(lMaxY-lMinY);
	LONG lSizeUmX	= (LONG)ConvertMotorStepToUnit(GetDieSizeX()*(100+lExtraEdge)/100);
	LONG lSizeUmY	= (LONG)ConvertMotorStepToUnit(GetDieSizeY()*(100+lExtraEdge)/100);
	LONG lPitchUmX	= (LONG)ConvertMotorStepToUnit(GetDiePitchX_X());
	LONG lPitchUmY	= (LONG)ConvertMotorStepToUnit(GetDiePitchY_Y());
	m_BinMapWrapper.SetPhysicalDimension(lWaferUmX, lWaferUmY, lSizeUmX, lSizeUmY, lPitchUmX, lPitchUmY);

	LONG lMaxUmX	= (LONG)ConvertMotorStepToUnit(lMaxX);
	LONG lMaxUmY	= (LONG)ConvertMotorStepToUnit(lMaxY);
	LONG lMinUmX	= (LONG)ConvertMotorStepToUnit(lMinX);
	LONG lMinUmY	= (LONG)ConvertMotorStepToUnit(lMinY);
//szLog.Format("XY order dimension (%d,%d) (%d,%d)  die(um) size (%d,%d) pitch (%d,%d)",
//			 lMaxUmX, lMaxUmY, lMinUmX, lMinUmY, lSizeUmX, lSizeUmY, lPitchUmX, lPitchUmY);
//SetAlarmLog(szLog);

	int nWaferPixelX	= (int)(lWaferUmX * m_dTabletXPPI / 25400); // Convert wafer width to number of pixels
	int nWaferPixelY	= (int)(lWaferUmY * m_dTabletYPPI / 25400); // Convert wafer height to number of pixels

//szLog.Format("wafer dimension (um) (%d,%d) pixel(%d,%d)", lWaferUmX, lWaferUmY, nWaferPixelX, nWaferPixelY);
//SetAlarmLog(szLog);

	CDC dcMemory;
	HDC hDC = GetDC(GetDesktopWindow());

	CDC ScreenDC;
	ScreenDC.Attach(hDC);

	CBitmap bmpScreen;
	bmpScreen.CreateCompatibleBitmap(&ScreenDC, nImagePixelX, nImagePixelY);

	dcMemory.CreateCompatibleDC(&ScreenDC);
	CBitmap *pOldBitmap;
	pOldBitmap = dcMemory.SelectObject(&bmpScreen);

	dcMemory.FillSolidRect(CRect(0, 0, nImagePixelX, nImagePixelY), RGB(0, 0, 0));

	m_BinMapWrapper.GetReader()->GetProcessData().SetPhysicalValid(TRUE);

	CUIntArray aColor;
	aColor.Add(m_ulBoundaryDieColour);	// Boundary die
	if ( m_szCurrentGrade == "Y" )
	{
		aColor.Add(m_ulNGDieColourY);		// NG die
	}
	else if ( m_szCurrentGrade == "N" )
	{
		aColor.Add(m_ulNGDieColourX);		// NG die (X)
	}
	else
	{
		aColor.Add(m_ulNGDieColourN);		// NG die (N)
	}

	if( m_bScanDetectFakeEmpty )
		aColor.Add(m_ulNGDieColourE);

	// For the boundary, do the following: 
	ULONG ulRowCount, ulColCount;
	m_BinMapWrapper.GetReader()->SelectEdge(1); // Select the boundary
	m_BinMapWrapper.GetMapDimension(ulRowCount, ulColCount);

	for (ULONG i = 0; i < ulRowCount; i++)
	{
		for (ULONG j = 0; j < ulColCount; j++)
		{
			if ( m_BinMapWrapper.GetReader()->IsSelected(i, j))
			{
				pView->SetDieDrawState(i, j, DRAW_OUTLINE, 0); // use color 0 to draw outline
			}

			// For all NG dice, 
			ULONG ulState = m_WaferMapWrapper.GetDieState(i+GetMapValidMinRow(), j+GetMapValidMinCol());
			if( ulState==WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT || 
				ulState==WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT )
			{
				pView->SetDieDrawState(i, j, DRAW_SOLID, 1);  // use color 1 to draw the NG die
			}

			if( ulState==WT_MAP_DS_SCAN_BACKUP_ALIGN )
			{
				pView->SetDieDrawState(i, j, DRAW_SOLID, 2);  // use color 2 to draw the NG die
			}
		}
	}
	m_BinMapWrapper.GetReader()->SelectEdge(0); // deselect the boundary

	// Put the picture in center
	int nULPixelX = (nImagePixelX - nWaferPixelX) / 2;
	int nULPixelY = (nImagePixelY - nWaferPixelY) / 2; 
	int nLRPixelX = nULPixelX + nWaferPixelX;
	int nLRPixelY = nULPixelY + nWaferPixelY;

//szLog.Format("wafer corner position (%d,%d) (%d,%d)", nULPixelX, nULPixelY, nLRPixelX, nLRPixelY);
//SetAlarmLog(szLog);
//	pView->DrawPictureA(&dcMemory, CRect(nULPixelX, nULPixelY, nLRPixelX, nLRPixelY), aColor, 1);
//	DrawPictureB(DC, Rect, ColorArray, 1, GridColorArray, GridRowCount, GridColCount, GridLineThick)
	if ( m_bPngEnableGrid == TRUE )
	{
		CUIntArray aulColourArray;
		// If one of line is black or 5 ticks not choose, make to 5 colour
		if ( ReformTheGridColourArray(aulColourArray) == FALSE )
		{
			//HmiMessage("A1");
			pView->DrawPictureA(&dcMemory, CRect(nULPixelX, nULPixelY, nLRPixelX, nLRPixelY), aColor, 1);
		}
		else
		{
			//HmiMessage("B");
			pView->DrawPictureB(&dcMemory, CRect(nULPixelX, nULPixelY, nLRPixelX, nLRPixelY), aColor, 1,
				aulColourArray, m_lPngGridRow, m_lPngGridCol, m_lPngGridThick);
		}
	}
	else
	{
		//HmiMessage("A2");
		pView->DrawPictureA(&dcMemory, CRect(nULPixelX, nULPixelY, nLRPixelX, nLRPixelY), aColor, 1);
	}

	int nTabletMaxPixel = max(nTabletPixelX, nTabletPixelY);
	int nPointSize = 3 * nTabletMaxPixel / 20;
	int nTextX = (nImagePixelX - nTabletPixelX) / 2 + nTabletMaxPixel / 50;
	int nTextY = (nImagePixelY - nTabletPixelY) / 2 + nTabletMaxPixel / 100;

	CFont Font;
	Font.CreatePointFont(nPointSize, "Arial");

	CFont *pFont = dcMemory.SelectObject(&Font);

	CString szPassPercentage;
	szPassPercentage.Format("%f",m_dCurrentScore);
	dcMemory.SetTextColor(RGB(255,255,255));
	dcMemory.TextOut(nTextX, nTextY, GetMapNameOnly());
	dcMemory.TextOut(nTextX, nTextY+50, szPassPercentage);
	dcMemory.SelectObject(pFont);
//szLog.Format("wafer text out (%d,%d)", nTextX, nTextY);
//SetAlarmLog(szLog); 

	Gdiplus::Bitmap *pImg = NULL;
	if (pImg = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)bmpScreen.GetSafeHandle(), 0))
	{
		string szPath = GetMapNameOnly() + ".png";
		CLSID pngClsid;
		wstring widestr;

		widestr = wstring(szPath.begin(), szPath.end());
		GetEncoderClsid(L"image/png", &pngClsid);
		pImg->Save(widestr.c_str(), &pngClsid, NULL);
		delete pImg;
	}

	dcMemory.SelectObject(pOldBitmap);
	bmpScreen.DeleteObject();
	dcMemory.DeleteDC();

	ReleaseDC(GetDesktopWindow(), hDC);

	// copy files to server and back up to local with fixed name.
	CString szPath;
	szPath.Format("C:\\MapSorter\\Exe\\%s.png", GetMapNameOnly());
	CString szBkPath;
	szBkPath = "C:\\MapSorter\\Exe\\ThePictureFile_bk.png";

	CString szArrTemp[4];
	if( IsWT2InUse() )
	{
		szArrTemp[0] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_1Layer"];  // Top == First
		szArrTemp[1] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_2Layer"];
		szArrTemp[2] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_3Layer"];
		szArrTemp[3] = (*m_psmfSRam)["WaferTable"]["WT2FilePath_LowestLayer"];
	}
	else
	{
		szArrTemp[0] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_1Layer"];  // Top == First
		szArrTemp[1] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_2Layer"];
		szArrTemp[2] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_3Layer"];
		szArrTemp[3] = (*m_psmfSRam)["WaferTable"]["WT1FilePath_LowestLayer"];
	}

	CString szSaveMapImagePath = m_szSaveMapImagePath;
	for ( int i = 0 ; i < 4 ; i++ )
	{
		if( szArrTemp[i] != "" )
		{
			szSaveMapImagePath = szSaveMapImagePath + "\\" + szArrTemp[i];
			if ( _access(szSaveMapImagePath, 0) == -1 )
			{
				CreateDirectory(szSaveMapImagePath, NULL);
			}
		}
	}
	szSaveMapImagePath = szSaveMapImagePath + "\\Pad";
	if ( _access(szSaveMapImagePath, 0) == -1 )
	{
		CreateDirectory(szSaveMapImagePath, NULL);
	}

	CString szTempPath;
	szTempPath = szSaveMapImagePath + "\\" + GetMapNameOnly() + ".png";
	CopyFileWithQueue(szPath, szTempPath, FALSE);

	RenameFile(szPath, szBkPath);

	m_BinMapWrapper.InitMap();

	delete pView;
	SaveScanTimeEvent("    WFT: generate PNG picture done");
}

BOOL CWaferTable::ReformTheGridColourArray(CUIntArray &aulColourArray)
{
	int nCounter = 0;

	if ( m_bGridColourOn1 == TRUE && m_ulGridColourArray1 != 0 )
	{
		aulColourArray.Add(m_ulGridColourArray1);
		nCounter++;
	}	
	if ( m_bGridColourOn2 == TRUE && m_ulGridColourArray2 != 0 )
	{
		aulColourArray.Add(m_ulGridColourArray2);
		nCounter++;
	}	
	if ( m_bGridColourOn3 == TRUE && m_ulGridColourArray3 != 0 )
	{
		aulColourArray.Add(m_ulGridColourArray3);
		nCounter++;
	}	
	if ( m_bGridColourOn4 == TRUE && m_ulGridColourArray4 != 0 )
	{
		aulColourArray.Add(m_ulGridColourArray4);
		nCounter++;
	}
	if ( m_bGridColourOn5 == TRUE && m_ulGridColourArray5 != 0 )
	{
		aulColourArray.Add(m_ulGridColourArray5);
		nCounter++;
	}

	if ( nCounter == 0 )
	{
		return FALSE;
	}

	return TRUE;
}

LONG CWaferTable::GenerateWaferDiagram(IPC_CServiceMessage& svMsg)
{
	ObtainMapValidRange();	// test gen png
	GenerateTabletPicture();
	
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferTable::TestXMLFunction(IPC_CServiceMessage& svMsg)
{
	BOOL bResult = TRUE;

	m_bPngEnableGrid = TRUE;
	m_lPngGridRow = 2;
	m_lPngGridCol = 3;
	m_lPngGridThick = 2;

	m_lPngDieExtraEdgeE = 0;
	m_lPngDieExtraEdgeY	= 3;
	m_lPngDieExtraEdgeN	= 4;		// NG die (X)
	m_lPngDieExtraEdgeX	= 5;		// NG die (N)

	m_ulNGDieColourY	= RGB(173,137,000);		// NG die
	m_ulNGDieColourX	= RGB(000,222,111);		// NG die (X)
	m_ulNGDieColourN	= RGB(173,173,173);		// NG die (N)
	m_ulNGDieColourE	= RGB(0,0,0);		// NG die E

	m_bGridColourOn1 = TRUE;
	m_bGridColourOn2 = TRUE;
	m_bGridColourOn3 = TRUE;
	m_bGridColourOn4 = TRUE;
	m_bGridColourOn5 = TRUE;
	m_ulGridColourArray1 = RGB(255,137,000);
	m_ulGridColourArray2 = RGB(000,255,000);
	m_ulGridColourArray3 = RGB(000,000,000);
	m_ulGridColourArray4 = RGB(000,000,255);
	m_ulGridColourArray5 = RGB(255,255,255);

	ObtainMapValidRange();	// test gen png
	GenerateTabletPicture();

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

// You need this function

int CWaferTable::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
	UINT num = 0;
	UINT size = 0;

	Gdiplus::ImageCodecInfo *pImageCodecInfo = NULL;
	Gdiplus::GetImageEncodersSize(&num, &size);

	if (size == 0)
	{
		return -1;
	}

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
	{
		return -1;
	}

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

LONG CWaferTable::RemoveAllSCNPSN(IPC_CServiceMessage& svMsg)
{
	BOOL bResult = FALSE;

	if (HmiMessage("Are you sure remove all .scn and .psn file in " + m_szMapFilePath + " ?", "Remove Files", glHMI_MBX_YESNO) == glHMI_YES)
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->SearchAndRemoveFiles(m_szMapFilePath, 0, TRUE, "scn") && pApp->SearchAndRemoveFiles(m_szMapFilePath, 0, TRUE, "psn"))
		{
			HmiMessage("Removed");
			bResult = TRUE;
		}
		else 
		{
			HmiMessage("Remove failed");
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferTable::ChangeBoundaryDieColour(IPC_CServiceMessage& svMsg)
{
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}


LONG CWaferTable::ChangeNGDieColour(IPC_CServiceMessage& svMsg)
{
	LONG lIndex;
	svMsg.GetMsg(sizeof(LONG), &lIndex);
	BOOL bResult = FALSE;

	if ( lIndex == 0 )
	{
		bResult = GetColorByColourDialog(m_ulBoundaryDieColour);
	}
	else if ( lIndex == 1 )
	{
		bResult = GetColorByColourDialog(m_ulNGDieColourY);
	}
	else if ( lIndex == 2 )
	{
		bResult = GetColorByColourDialog(m_ulNGDieColourN);
	}
	else if ( lIndex == 3 )
	{
		bResult = GetColorByColourDialog(m_ulNGDieColourX);
	}
	else if ( lIndex == 4 )
	{
		bResult = GetColorByColourDialog(m_ulNGDieColourE);
	}
	else
	{
		CString szTitle;
		szTitle.Format("ERROR(%d)",lIndex);
		HmiMessage("Set up NG Die Colour ERROR!",szTitle);
	}

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferTable::ChangeGridLineColour(IPC_CServiceMessage& svMsg)
{
	LONG lIndex;
	svMsg.GetMsg(sizeof(LONG), &lIndex);
	BOOL bResult = FALSE;

	if ( lIndex <= 0 || lIndex > 5 )
	{
		CString szTitle;
		szTitle.Format("ERROR(%d)",lIndex);
		HmiMessage("Set up Grid Line Colour ERROR!",szTitle);
	}
	else if ( lIndex == 1 && m_bGridColourOn1 )
	{
		bResult = GetColorByColourDialog(m_ulGridColourArray1);
	}
	else if ( lIndex == 2 && m_bGridColourOn2 )
	{
		bResult = GetColorByColourDialog(m_ulGridColourArray2);
	}
	else if ( lIndex == 3 && m_bGridColourOn3 )
	{
		bResult = GetColorByColourDialog(m_ulGridColourArray3);
	}
	else if ( lIndex == 4 && m_bGridColourOn4 )
	{
		bResult = GetColorByColourDialog(m_ulGridColourArray4);
	}
	else if ( lIndex == 5 && m_bGridColourOn5 )
	{
		bResult = GetColorByColourDialog(m_ulGridColourArray5);
	}

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

BOOL CWaferTable::GetColorByColourDialog(COLORREF &ulReturnColour)
{
	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	// Set the application to TopMost, otherwise the dialog box will only show at background
	BOOL bReturn = TRUE;
	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);	
		bReturn = pAppMod->GetColorWithDialog(ulReturnColour);
		// After input, restore the application to background
		pAppMod->ShowHmi();				
	}

	return bReturn;
}

BOOL CWaferTable::CreateDummyMapForBurnIn(LONG lAlnWfX, LONG lAlnWfY, CString szDummyScanMapPath)
{
	UCHAR ucMapGrade = (UCHAR) m_lBurningInGrade;

	LONG lDummyMapDiameter	= GetWaferDiameter()*9/10;
	LONG lDummyMapCenterX	= GetWaferCenterX();
	LONG lDummyMapCenterY	= GetWaferCenterY();
	LONG lLftX = lDummyMapCenterX + lDummyMapDiameter/2;
	LONG lRgtX = lDummyMapCenterX - lDummyMapDiameter/2;
	LONG lTopY = lDummyMapCenterY + lDummyMapDiameter/2;
	LONG lBtmY = lDummyMapCenterY - lDummyMapDiameter/2;

	LONG lDiePitchX_X	= labs(GetDiePitchX_X());
	LONG lDiePitchY_Y	= labs(GetDiePitchY_Y());
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();

	LONG ulNoOfRowsTop = 0, ulNoOfRowsBtm = 0, ulNoOfColsLft = 0, ulNoOfColsRgt = 0;
	if (lDiePitchY_Y != 0)
	{
		ulNoOfRowsTop = labs((lTopY-lAlnWfY) / lDiePitchY_Y) + 5;
		ulNoOfRowsBtm = labs((lAlnWfY-lBtmY) / lDiePitchY_Y) + 5;
	}

	if (lDiePitchX_X != 0)
	{
		ulNoOfColsLft = labs((lLftX-lAlnWfX) / lDiePitchX_X) + 5;
		ulNoOfColsRgt = labs((lAlnWfX-lRgtX) / lDiePitchX_X) + 5;
	}

	LONG lLoopOfRows = max(ulNoOfRowsTop, ulNoOfRowsBtm);
	LONG lLoopOfCols = max(ulNoOfColsLft, ulNoOfColsRgt);
	LONG lLoopMax = max(lLoopOfRows, lLoopOfCols);
	LONG lAlnRow = 0, lAlnCol = 0;

	//Clear Wafermap & reset grade map
	m_WaferMapWrapper.InitMap();
	SetPsmEnable(FALSE);

	FILE *fpMap = NULL;
	errno_t nErr = fopen_s(&fpMap, szDummyScanMapPath, "w");
	if ((nErr == 0) && (fpMap != NULL))
	{
		fprintf(fpMap, "DataFileName,,%s\n", (LPCTSTR) szDummyScanMapPath);
		fprintf(fpMap, "LotNumber,,\n");
		fprintf(fpMap, "DeviceNumber,,\n");
		fprintf(fpMap, "wafer id=\n");
		fprintf(fpMap, "TestTime,20140930,052215\n");
		fprintf(fpMap, "MapFileName,,%s\n", (LPCTSTR) szDummyScanMapPath);
		fprintf(fpMap, "TransferTime,20140930,052215\n");
		fprintf(fpMap, "\n");
		fprintf(fpMap, "map data\n");
		fprintf(fpMap, "%ld,%ld,\n", lAlnCol, lAlnRow);
		fprintf(fpMap, "%ld,%ld,%d\n", lAlnCol, lAlnRow, ucMapGrade+1);

		LONG lSpanRow, lSpanCol, lEncX, lEncY;
		LONG lCheckDiameter = lDummyMapDiameter;
		LONG lMapRow = lAlnRow;
		LONG lMapCol = lAlnCol;
		LONG lDieCount = 0;
		BOOL bMapFull = FALSE;

		for(LONG lLoop = 1; lLoop <= lLoopMax; lLoop++)
		{
			//Move to RIGHT
			lMapCol += 1;
			lMapRow += 0;

			lSpanRow = lMapRow - lAlnRow;
			lSpanCol = lMapCol - lAlnCol;
			lEncX = lAlnWfX - lSpanCol * lDiePitchX_X - lSpanRow * lDiePitchY_X;
			lEncY = lAlnWfY - lSpanRow * lDiePitchY_Y - lSpanCol * lDiePitchX_Y;
			if( IsWithinLimit(lEncX, lEncY, lDummyMapCenterX, lDummyMapCenterY, lCheckDiameter, TRUE) )
			{
				fprintf(fpMap, "%ld,%ld,%d\n", lMapCol, lMapRow, ucMapGrade);
				lDieCount++;
				if( m_lBurnInMapDieCount>0 && lDieCount>=m_lBurnInMapDieCount )
				{
					bMapFull = TRUE;
					break;
				}
			}

			//Move to UP
			for(LONG lIndex=0; lIndex<(lLoop * 2  - 1); lIndex++)
			{
				lMapRow -= 1;
				lMapCol += 0;

				lSpanRow = lMapRow - lAlnRow;
				lSpanCol = lMapCol - lAlnCol;
				lEncX = lAlnWfX - lSpanCol * lDiePitchX_X - lSpanRow * lDiePitchY_X;
				lEncY = lAlnWfY - lSpanRow * lDiePitchY_Y - lSpanCol * lDiePitchX_Y;
				if( IsWithinLimit(lEncX, lEncY, lDummyMapCenterX, lDummyMapCenterY, lCheckDiameter, TRUE) )
				{
					fprintf(fpMap, "%ld,%ld,%d\n", lMapCol, lMapRow, ucMapGrade);
					lDieCount++;
					if( m_lBurnInMapDieCount>0 && lDieCount>=m_lBurnInMapDieCount )
					{
						bMapFull = TRUE;
						break;
					}
				}
			}
			if( bMapFull )
			{
				break;
			}

			//Move to LEFT
			for(LONG lIndex=0; lIndex<(lLoop * 2); lIndex++)
			{
				lMapCol -= 1;
				lMapRow += 0;

				lSpanRow = lMapRow - lAlnRow;
				lSpanCol = lMapCol - lAlnCol;
				lEncX = lAlnWfX - lSpanCol * lDiePitchX_X - lSpanRow * lDiePitchY_X;
				lEncY = lAlnWfY - lSpanRow * lDiePitchY_Y - lSpanCol * lDiePitchX_Y;
				if( IsWithinLimit(lEncX, lEncY, lDummyMapCenterX, lDummyMapCenterY, lCheckDiameter, TRUE) )
				{
					fprintf(fpMap, "%ld,%ld,%d\n", lMapCol, lMapRow, ucMapGrade);
					lDieCount++;
					if( m_lBurnInMapDieCount>0 && lDieCount>=m_lBurnInMapDieCount )
					{
						bMapFull = TRUE;
						break;
					}
				}
			}
			if( bMapFull )
			{
				break;
			}

			//Move to DOWN
			for(LONG lIndex=0; lIndex<(lLoop * 2); lIndex++)
			{
				lMapRow += 1;
				lMapCol += 0;

				lSpanRow = lMapRow - lAlnRow;
				lSpanCol = lMapCol - lAlnCol;
				lEncX = lAlnWfX - lSpanCol * lDiePitchX_X - lSpanRow * lDiePitchY_X;
				lEncY = lAlnWfY - lSpanRow * lDiePitchY_Y - lSpanCol * lDiePitchX_Y;
				if( IsWithinLimit(lEncX, lEncY, lDummyMapCenterX, lDummyMapCenterY, lCheckDiameter, TRUE) )
				{
					fprintf(fpMap, "%ld,%ld,%d\n", lMapCol, lMapRow, ucMapGrade);
					lDieCount++;
					if( m_lBurnInMapDieCount>0 && lDieCount>=m_lBurnInMapDieCount )
					{
						bMapFull = TRUE;
						break;
					}
				}
			}
			if( bMapFull )
			{
				break;
			}

			//Move to Right
			for(LONG lIndex=0; lIndex<(lLoop * 2); lIndex++)
			{
				lMapRow += 0;
				lMapCol += 1;

				lSpanRow = lMapRow - lAlnRow;
				lSpanCol = lMapCol - lAlnCol;
				lEncX = lAlnWfX - lSpanCol * lDiePitchX_X - lSpanRow * lDiePitchY_X;
				lEncY = lAlnWfY - lSpanRow * lDiePitchY_Y - lSpanCol * lDiePitchX_Y;
				if( IsWithinLimit(lEncX, lEncY, lDummyMapCenterX, lDummyMapCenterY, lCheckDiameter, TRUE) )
				{
					fprintf(fpMap, "%ld,%ld,%d\n", lMapCol, lMapRow, ucMapGrade);
					lDieCount++;
					if( m_lBurnInMapDieCount>0 && lDieCount>=m_lBurnInMapDieCount )
					{
						bMapFull = TRUE;
						break;
					}
				}
			}
			if( bMapFull )
			{
				break;
			}
		}

		fclose(fpMap);
	}

	CMS896AApp::m_bMapLoadingFinish	= FALSE;
	if( LoadWaferMap(szDummyScanMapPath, "")==FALSE )	// dummy map for burn in
	{
		return FALSE;
	}

	LONG lLoopTimer = 0;
	while( CMS896AApp::m_bMapLoadingFinish==FALSE )
	{
		Sleep(100);
		lLoopTimer++;
		if( lLoopTimer>=1200 )
		{
			return FALSE;
		}
	}

	lLoopTimer = 0;
	while (!m_WaferMapWrapper.IsMapValid())
	{
		Sleep(100);
		lLoopTimer++;
		if( lLoopTimer>=1200 )
		{
			return FALSE;
		}
	}

	unsigned char aaTempGrades[1];
	aaTempGrades[0] = m_WaferMapWrapper.GetGradeOffset() + ucMapGrade;

	m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aaTempGrades[0]);
	m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);

	return TRUE;
}	// create dummy map for burn in of MS machine.

BOOL CWaferTable::SetDummyMapForNichia(IPC_CServiceMessage &svMsg)
{
	CreateDummyMapForNichia(10,10,10,10,10,10);
	return TRUE;
}

LONG CWaferTable::FaceValueExistsInMap(IPC_CServiceMessage& svMsg)	//Avago
{
	BOOL	bReturn = FALSE;
	LONG	i, lFaceValueFromMap;
	SHORT	sRefNo;
	ULONG	ulNumRefDie;
	WAF_CMapDieInformation *arrRef;

	svMsg.GetMsg(sizeof(LONG), &lFaceValueFromMap);

	if (!m_WaferMapWrapper.IsMapValid())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	ulNumRefDie = m_WaferMapWrapper.GetNumberOfReferenceDice();
	arrRef		= new WAF_CMapDieInformation [ulNumRefDie];
	m_WaferMapWrapper.GetReferenceDieList(arrRef, ulNumRefDie);

	for (i=0; i<(LONG)ulNumRefDie; i++)
	{
		if (m_WaferMapWrapper.HasReferenceFaceValue(arrRef[i].GetRow(), arrRef[i].GetColumn()) == TRUE)
		{
			sRefNo = (SHORT) m_WaferMapWrapper.GetReferenceDieFaceValue(arrRef[i].GetRow(), arrRef[i].GetColumn());
			if (sRefNo == lFaceValueFromMap)
			{
				bReturn = TRUE;
				break;
			}
		}
	}
	
	delete[] arrRef;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::BrowseLoadMapFile(IPC_CServiceMessage &svMsg)
{
	// Get the Main Windows
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]= "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.*", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pMainWnd, 0);

	dlgFile.m_ofn.lpstrInitialDir = m_szMapFilePath;
	//dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	if ( nReturn != IDOK )
	{
		BOOL	bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szMapPathName = dlgFile.GetPathName();
	CString szMapPath = szMapPathName;
	int nCol = szMapPath.ReverseFind('\\');
	if( nCol!=-1 )
		szMapPath = szMapPath.Left(nCol);

	CString szLclPath	= LOCALHD_MAP_PATH;
	CString szLclFullName	= szLclPath + "\\" + dlgFile.GetFileName();

	CString szMapName = dlgFile.GetFileName();
	nCol = szMapName.ReverseFind('.');
	if( nCol!=-1 )
	{
		szMapName = szMapName.Left(nCol);
	}

	if( pApp->GetCustomerName()=="HuaLei" )
	{
		szMapName = szMapName.MakeUpper();
	}
	m_szBCMapFilename = m_szMapDisplayFilename = szMapName;
	m_bLoadMapAccessFail		= FALSE;
	m_szAccessFailFileName		= "";

	CString szRemoteSCNFile = szMapPath + "\\" + szMapName + ".SCN";
	CString szRemotePSMFile = szMapPath + "\\" + szMapName + ".PSM";
	CString szLocalSCNFile	= szLclPath + "\\" + szMapName + ".SCN";
	CString szLocalPSMFile	= szLclPath + "\\" + szMapName + ".PSM";

	if (PsmFileExistCheck(szRemotePSMFile) == FALSE)
	{
		BOOL	bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Try to create local MAP folder if not exist		//v2.83T34
	if ( CreateDirectory(LOCALHD_MAP_PATH, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			HmiMessage_Red_Yellow("Fail to create CurrMap sub-folder!");
			BOOL	bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	DeleteLastLocalMapFile();	

	m_szMapServerFullPath = szMapPathName;
	TRY 
	{
		if (_access(szMapPathName, 0) == -1)
		{
			m_bLoadMapAccessFail = TRUE;
			m_szAccessFailFileName		= szMapPathName;
			HmiMessage_Red_Yellow("Fail to access network folder! - " + szMapPathName);		//v3.65
			SetErrorMessage("Fail to access network folder - " + szMapPathName);	//v3.65
			BOOL	bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (!CopyFileWithRetry(szMapPathName, szLclFullName, FALSE))
		{
			CString szMsg;
			szMsg.Format("Fail to copy map to local sub-folder!  System ErrorCode = %d", GetLastError());
			HmiMessage_Red_Yellow(szMsg);
			HmiMessage_Red_Yellow("Source path = " + szMapPathName);
			HmiMessage_Red_Yellow("Target path = " + szLclFullName);

			BOOL	bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (_access(szLclFullName, 0) == -1)
		{
			m_bLoadMapAccessFail = TRUE;
			m_szAccessFailFileName		= szLclFullName;
			SetErrorMessage(szLclFullName + " is not accessible");
			BOOL	bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if (_access(szRemoteSCNFile, 0) != -1)
		{
			if (!CopyFileWithRetry(szRemoteSCNFile, szLocalSCNFile, FALSE))
			{
				HmiMessage_Red_Yellow("Fail to copy SCN to local sub-folder!  " + szLocalSCNFile);
				SetErrorMessage("Fail to copy SCN to local sub-folder!  " + szLocalSCNFile);
				BOOL	bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}

		if (_access(szRemotePSMFile, 0) != -1)
		{
			if (!CopyFileWithRetry(szRemotePSMFile, szLocalPSMFile, FALSE))
			{
				HmiMessage_Red_Yellow("Fail to copy PSM to local sub-folder!  " + szLocalPSMFile);
				SetErrorMessage("Fail to copy PSM to local sub-folder!  " + szLocalPSMFile);
				BOOL	bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}

		//	szSrvDfntName should be the same location with server map file.
		CString szAssociateFile  = (*m_psmfSRam)["MS896A"]["Associate File"];
		if( szAssociateFile.IsEmpty()==FALSE )
		{
			if( m_szAssocFileExt.IsEmpty()==FALSE )
				szAssociateFile = szAssociateFile +  "." + m_szAssocFileExt;
			CString szSrvDfntName = szMapPath + szAssociateFile;
			CString	szLclDfntName = szLclPath + "\\" + szAssociateFile;
			if ( _access(szSrvDfntName, 0) != -1)
			{
				if (!CopyFileWithRetry(szSrvDfntName, szLclDfntName, FALSE))
				{
					HmiMessage_Red_Yellow("Fail to copy DEFINE file to local HD folder!");
					SetErrorMessage("Fail to copy DEFINE file " + szSrvDfntName + " to local HD folder!");
					BOOL	bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}
			}
		}

		//Start to load map file
		if( pApp->GetCustomerName()=="OSRAM" )	//	426TX	1
		{
			szLclFullName = szLclFullName + ";" + m_szOsramSDF;
		}
		else if( szAssociateFile.IsEmpty()==FALSE )
		{
			szLclFullName = szLclFullName + ";" + szAssociateFile;
		}

		if( AutoLoadMap(szLclFullName, "", TRUE) == FALSE )
		{
			HmiMessage("Unable to load " + szMapPathName + "\nAt local " + szLclFullName + "!", "User Mode Error");
		}
	} 
	CATCH (CFileException, e)
	{
		HmiMessage_Red_Yellow("Exception in User Mode Load Map!!!");
	}
	END_CATCH

	BOOL	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetOutWaferInfoFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szOutWaferInfoFilePath);
		SaveData();		
		bReturn = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::CheckClearBinResetSuffixName(IPC_CServiceMessage& svMsg)
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg;
	BOOL bResult = FALSE;

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIsAllBinCleared", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if ( bResult == FALSE)
	{
		m_szSuffixName = (*m_psmfSRam)["WaferTable"]["SuffixName"];
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

#define	WT_MAP_GRADE_COLOR_CHANGE_MAX	199
LONG CWaferTable::ChangeMapColorToOneColor(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	if ( m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
  	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()!=CTM_SANAN )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_WaferMapWrapper.UpdateGradeColor();
	UCHAR ucGradeOffset = m_WaferMapWrapper.GetGradeOffset();

	int bOneColor = _access(gszGreenColorFile, 0);

	if (bOneColor == -1)
	{
		CopyFile(gszAsmHmiColorFile, gszGreenColorFile, FALSE);
		UCHAR ucGrade = 0;
		ULONG ulColor = 65280;
		while (ucGrade < WT_MAP_GRADE_COLOR_CHANGE_MAX)
		{
			m_WaferMapWrapper.SetGradeColor(ucGrade + ucGradeOffset, (COLORREF)ulColor);
			ucGrade++;
		}
		m_WaferMapWrapper.UpdateGradeColor();
	}
	else
	{
		CStdioFile cfColorFile;
		//Get current wafer color & update other grades
		BOOL bIfFileExists = cfColorFile.Open(gszGreenColorFile, CFile::modeRead|CFile::shareExclusive|CFile::typeText);

		if (bIfFileExists)
		{
			//calculate line in green color file
			int nNum = 0;
			CString szContent;
			while (cfColorFile.ReadString(szContent) != NULL)
			{
				nNum ++;
			}
			//szContent.Format("%d",nNum);
			//AfxMessageBox(szContent);

			//Set selected grade color
			szContent = "";
			UCHAR ucGrade = 0;
			cfColorFile.SeekToBegin();
			int nCount = 0;
			while (cfColorFile.ReadString(szContent) != NULL)
			{
				nCount ++;
				if (nCount > nNum - 6)
				{
					cfColorFile.SeekToBegin();
					cfColorFile.ReadString(szContent);
				}
				ULONG ulTempColor = atoi((LPCTSTR)szContent);
				m_WaferMapWrapper.SetGradeColor(ucGrade + ucGradeOffset, (COLORREF)ulTempColor);
				ucGrade++;
				if( ucGrade>=WT_MAP_GRADE_COLOR_CHANGE_MAX )
					break;
			}
			cfColorFile.Close();
			m_WaferMapWrapper.UpdateGradeColor();
			remove(gszGreenColorFile);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::UpdateWaferMapPath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	m_szCurrentMapPath = GetMapFileName();//SECSGEM 
	SaveData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CWaferTable::WFT_SearchPrDie(LONG &lOffsetX, LONG &lOffsetY)
{
	lOffsetX = 0;
	lOffsetY = 0;

	IPC_CServiceMessage stMsg;
	{
		REF_TYPE	stInfo;
		SRCH_TYPE	stSrchInfo;

		//Init Message
		stSrchInfo.bNormalDie	= TRUE;
		stSrchInfo.lRefDieNo	= 1;
		stSrchInfo.bShowPRStatus = TRUE;
		stSrchInfo.bDisableBackupAlign = FALSE;

		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		int nConvID = 0;
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if( (stInfo.bStatus == FALSE) )	// PR error
		{
			return FALSE;
		}

		if( (stInfo.bFullDie == FALSE) )	// empty die
		{
			return FALSE;
		}

		lOffsetX = stInfo.lX;	
		lOffsetY = stInfo.lY;
		return TRUE;
	}
}

LONG CWaferTable::ScanCheckDieOffset(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	if( (IsPrescanEnable() && IsPrescanEnded()==FALSE) ||
		(IsScnLoaded() && IsAlignedWafer()==FALSE) )
	{
		HmiMessage("Prescan doesn't complete yet!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SelectWaferCamera();

	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
  	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMS896AApp::m_bStopAlign = FALSE;
	ULONG ulJUMP_STEP = pApp->GetProfileInt(gszPROFILE_SETTING, _T("CP Scan Check Step"), 3);
	if( ulJUMP_STEP<=0 )
	{
		ulJUMP_STEP = 3;
	}
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("CP Scan Check Step"), ulJUMP_STEP);

	ULONG ulMaxRow = 0, ulMaxCol = 0;
	m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
	ulMaxCol = (ulMaxCol/ulJUMP_STEP)*ulJUMP_STEP;

	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\";
	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	CString szTemp;
	szTemp.Format("ScanSearch_DC%dPO%d_", pCPInfo->GetDCState(), pCPInfo->GetPOState());
	szTemp.Format("ScanSearch_DC%lu_", GetNewPickCount());
	szLogFileName = szLogFileName + szTemp + ctDateTime.Format("%Y%m%d%H%M%S") + ".txt";
	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFileName, "w");
	LONG lGoX = 0, lGoY = 0, lMaxOffsetX = 0, lMaxOffsetY = 0, lMinOffsetX = 100, lMinOffsetY = 100;
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	DOUBLE dTime = GetTime();
	LONG lPrDelay = m_lCPClbPrDelay;
	lPrDelay = m_lPrescanPrDelay;
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "row,col,posnx,posny,offx,offy\n");
	}

	UCHAR ucDummyGrade = m_ucDummyPrescanPNPGrade + ucOffset;
	for(ULONG ulRow=0; ulRow<ulMaxRow; ulRow++)
	{
		for (LONG ulCol = 0; ulCol < (LONG)ulMaxCol; ulCol++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucGrade!=ucNullBin && ucGrade!=ucDummyGrade )
			{
				m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucDummyGrade);
			}
		}
	}

	for(ULONG ulRow=0; ulRow<ulMaxRow; ulRow=ulRow+ulJUMP_STEP)
	{
		LONG ulCol = 0;
		if( ulRow%(ulJUMP_STEP*2)!=0 )
			ulCol = ulMaxCol;
		while( 1 )
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucGrade==ucNullBin )
			{
				if( ulRow%(ulJUMP_STEP*2)!=0 )
				{
					ulCol = ulCol - ulJUMP_STEP;
					if( ulCol<0 )
						break;
				}
				else
				{
					ulCol = ulCol + ulJUMP_STEP;
					if( ulCol>(LONG)ulMaxCol )
						break;
				}
				continue;
			}
			LONG lScanX, lScanY;
			if( GetPrescanWftPosn(ulRow, ulCol, lScanX, lScanY)==FALSE )
			{
				if( ulRow%(ulJUMP_STEP*2)!=0 )
				{
					ulCol = ulCol - ulJUMP_STEP;
					if( ulCol<0 )
						break;
				}
				else
				{
					ulCol = ulCol + ulJUMP_STEP;
					if( ulCol>(LONG)ulMaxCol )
						break;
				}
				continue;
			}
			lGoX = lScanX;
			lGoY = lScanY;

			XY_SafeMoveTo(lGoX, lGoY, SFM_WAIT);
			Sleep(lPrDelay);
			GetEncoderValue();
			LONG lGoToX = GetCurrX();
			LONG lGoToY = GetCurrY();
			LONG lOffsetX = 0, lOffsetY = 0;
			BOOL bPrResult = WFT_SearchPrDie(lOffsetX, lOffsetY);
			LONG lOffsetX2 = 0, lOffsetY2 = 0;
			if( bPrResult )
			{
				if( WFT_SearchPrDie(lOffsetX2, lOffsetY2) )
				{
					if( labs(lOffsetX)>labs(lOffsetX2) )
						lOffsetX = lOffsetX2;
					if( labs(lOffsetY)>labs(lOffsetY2) )
						lOffsetY = lOffsetY2;
				}
			}
			UCHAR ucNewGrade = 100 + ucOffset;
			if( bPrResult==FALSE )
				ucNewGrade = 200 + ucOffset;
			{
				if( labs(lOffsetX)>125 || labs(lOffsetY)>125 )
				{
					if (labs(lOffsetX) > 125)
					{
						ucNewGrade += (UCHAR)((abs(lOffsetX) + 25) / 50);
					}
					if (labs(lOffsetY) > 125)
					{
						ucNewGrade += (UCHAR)(((abs(lOffsetY) + 25) / 50) * 10);
					}
				}

				if (fp != NULL)
				{
					LONG lHmiRow, lHmiCol;
					ConvertAsmToHmiUser(ulRow, ulCol, lHmiRow, lHmiCol);
					fprintf(fp, "%4ld,%4ld,%8ld,%8ld,%4ld,%4ld,%4ld,%4ld\n",	//Klocwork //v4.40T8
						lHmiRow, lHmiCol, lGoX, lGoY, lOffsetX, lOffsetY, lOffsetX2, lOffsetY2);
				}
			}

			m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNewGrade);
			m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
			if( (lMaxOffsetX)<(lOffsetX) )
				lMaxOffsetX = lOffsetX;
			if( (lMaxOffsetY)<(lOffsetY) )
				lMaxOffsetY = lOffsetY;
			if( (lMinOffsetX)>(lOffsetX) )
				lMinOffsetX = lOffsetX;
			if( (lMinOffsetY)>(lOffsetY) )
				lMinOffsetY = lOffsetY;
			if( pApp->IsStopAlign() )
			{
				break;
			}
			if( ulRow%(ulJUMP_STEP*2)!=0 )
			{
				ulCol = ulCol - ulJUMP_STEP;
				if( ulCol<0 )
					break;
			}
			else
			{
				ulCol = ulCol + ulJUMP_STEP;
				if( ulCol>(LONG)ulMaxCol )
					break;
			}
		}
		if( pApp->IsStopAlign() )
		{
			break;
		}
	}

	if( fp!=NULL )
	{
		fprintf(fp, "min %ld,%ld max %ld,%ld, used time %.4f\n",	//Klocwork	//v4.40T8
			lMinOffsetX, lMinOffsetY, lMaxOffsetX, lMaxOffsetY, (GetTime()-dTime)/1000.0);
		fclose(fp);
	}

	HmiMessage_Red_Back("Scan check whole wafer complete");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::FOVCheckDieOffset(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	LONG lLoopCounter = 1;
	while( 1 )
	{
		CString szOpId	= _T("1");
		CString szTitle		= "Please input loop test counter: ";
		BOOL bReturn = HmiStrInputKeyboard(szTitle, szOpId);
		if( bReturn && szOpId.IsEmpty()!=TRUE )
		{
			lLoopCounter = atoi(szOpId);
			break;
		}
	}

	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMS896AApp::m_bStopAlign = FALSE;

	MultiSrchInitNmlDie1(FALSE);	// hmi multi search test button

	GetEncoderValue();
	LONG lGrabX = GetCurrX();
	LONG lGrabY = GetCurrY();

	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lGrabX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lGrabY;

	for(int i=0; i<lLoopCounter; i++)
	{
		BOOL bDrawDie = FALSE;
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(BOOL), &bDrawDie);
		int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MultiSearchNmlDie1", stMsg);
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

		SelectWaferCamera();

		CString szLogFileName = gszUSER_DIRECTORY + "\\History\\";
		CTime ctDateTime;
		ctDateTime = CTime::GetCurrentTime();
		CString szTemp;

		szTemp.Format("FovScan_DC%dPO%d_", pCPInfo->GetDCState(), pCPInfo->GetPOState());
		szLogFileName = szLogFileName + szTemp + ctDateTime.Format("%Y%m%d%H%M%S") + ".txt";

		FILE *fp = NULL;
		errno_t nErr = fopen_s(&fp, szLogFileName, "w");
		LONG lMaxX = 0, lMaxY = 0, lMinX = 100, lMinY = 100;
		LONG lPrDelay = m_lCPClbPrDelay;
		if ((nErr == 0) && (fp != NULL))
		{
			fprintf(fp, "grab die %ld,%ld\n", lGrabX, lGrabY);
			fprintf(fp, "col,row,grd,posnx,posny,movex,movey,offx,offy,realx,realy");
			fprintf(fp, "\n");
		}

		for (ULONG i = 1; i <= GetGoodTotalDie(); i++)
		{
			LONG lPosX = 0, lPosY = 0, lOffsetX = 0, lOffsetY = 0;
			DOUBLE dAngle = 0;
			CString szDieBin;
			LONG lRow = 0, lCol = 0;
			GetGoodPosnAll(i, lPosX, lPosY, dAngle, szDieBin, lRow, lCol);
			LONG lScanX = lGrabX + lPosX;
			LONG lScanY = lGrabY + lPosY;
			LONG lMoveX = 0, lMoveY = 0;
			lMoveX = lScanX;
			lMoveY = lScanY;
			XY_MoveTo(lMoveX, lMoveY, SFM_WAIT);
			Sleep(lPrDelay);
			BOOL bPrResult = WFT_SearchPrDie(lOffsetX, lOffsetY);
			LONG lOffsetX2 = 0, lOffsetY2 = 0;
			if( bPrResult )
			{
				LONG lOffsetX2, lOffsetY2;
				if( WFT_SearchPrDie(lOffsetX2, lOffsetY2) )
				{
					if( labs(lOffsetX)>labs(lOffsetX2) )
						lOffsetX = lOffsetX2;
					if( labs(lOffsetY)>labs(lOffsetY2) )
						lOffsetY = lOffsetY2;
				}
			}
			if( (lMaxY)<(lOffsetY) )
				lMaxY = lOffsetY;
			if( (lMinX)>(lOffsetX) )
				lMinX = lOffsetX;
			if( (lMinY)>(lOffsetY) )
				lMinY = lOffsetY;
			if( fp!=NULL )
			{
				LONG ucGrade = 100;
				if( labs(lOffsetX)>=20 )
					ucGrade += (labs(lOffsetX)+5)/10*10;
				if( labs(lOffsetY)>=20 )
					ucGrade += (labs(lOffsetY)+5)/10;
				fprintf(fp, "%3ld,%3ld,%3ld,%8ld,%8ld,%8ld,%8ld,%4ld,%4ld,%8ld,%8ld",
					lCol, lRow, ucGrade, lScanX, lScanY, lMoveX, lMoveY, lOffsetX, lOffsetY, lMoveX+lOffsetX, lMoveY+lOffsetY);
				fprintf(fp, "\n");
			}
			if( pApp->IsStopAlign() )
			{
				break;
			}
		}

		if( fp!=NULL )
		{
			fprintf(fp, "min %ld,%ld max %ld,%ld\n", lMinX, lMinY, lMaxX, lMaxY);	//Klocwork	//v4.40T8
			fclose(fp);
		}

		XY_MoveTo(lGrabX, lGrabY, SFM_WAIT);

		if( pApp->IsStopAlign() )
		{
			break;
		}
		Sleep(2000);
	}

	HmiMessage_Red_Back("FOV check offset complete!");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::WftLoadBinFile(IPC_CServiceMessage &svMsg)
{
	if (CMS896AStn::m_bWaferLotWithBinSummaryFormat==FALSE)
	{
		return FALSE;
	}

	CString szMachineNo, szPKGFilename;
	CString szBinSummaryFilename;
	szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];

	CString szFileName = m_szLotInfoFilePath + "\\" + szMachineNo + "_" + m_szLotInfoFileName + "." + m_szLotInfoFileExt;

	// load it to update the wafer lot summary
	if (ReadLextarLotFile(szFileName, szBinSummaryFilename, szPKGFilename) == FALSE)
	{
		SetErrorMessage("Fail to load lot file");
		SetAlert(IDS_WL_LOTFILE_NOT_FOUND);
		DeleteFile(MSD_WAFER_LOT_INFO_FILE);
		DeleteFile(MSD_WAFER_LOT_INFO_BKF);
		ShowWaferLotData();
		return FALSE;
	}

	if (LoadBinSummaryFile(szBinSummaryFilename) == FALSE)
	{
		SetErrorMessage("Fail to auto load bin summary file");
		DeleteFile(MSD_WAFER_LOT_INFO_FILE);
		DeleteFile(MSD_WAFER_LOT_INFO_BKF);
		return FALSE;
	}
	
	ShowWaferLotData();

	return TRUE;
}

LONG CWaferTable::AGC_WTMoveDiff(IPC_CServiceMessage &svMsg)////TestarBuyoff
{
	typedef struct 
	{
		BOOL bBHCollet2;
		LONG lDiffX;
		LONG lDiffY;
	} AGC_OFFSET;

	AGC_OFFSET stInfo;
	svMsg.GetMsg(sizeof(AGC_OFFSET), &stInfo);
	
	X_Move(stInfo.lDiffX);
	Y_Move(stInfo.lDiffY);

	CString szTemp;
	szTemp.Format("AGC_WTMoveDiff,Diff,(%d,%d)",stInfo.lDiffX,stInfo.lDiffY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::WTChangeNoDiePosition(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	SetJoystickOn(FALSE);

	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, WT_JOY_MAXLIMIT_NEG_X, WT_JOY_MAXLIMIT_POS_X, &m_stWTAxis_X);		
	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, WT_JOY_MAXLIMIT_NEG_Y, WT_JOY_MAXLIMIT_POS_Y, &m_stWTAxis_Y);

#ifdef NU_MOTION
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
#endif

	XY1_MoveTo(m_lWTNoDiePosX, m_lWTNoDiePosY, SFM_WAIT);

#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	SetJoystickOn(TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::WTConfirmNoDiePositionSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	CString szMsg;

	if (!m_fHardware || m_bDisableWT)	//v3.61
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	GetEncoderValue();

	m_lWTNoDiePosX = GetCurrX1();
	m_lWTNoDiePosY = GetCurrY1();

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::SaveSCNData(IPC_CServiceMessage& svMsg)	//Matthew 20190109
{
	BOOL bReturn = TRUE;
	int nYear, nMonth, nDay, nHour, nMinute, nSecond;
	CString szYear, szMonth, szDay, szHour, szMinute, szSecond;
	CStdioFile Log;
	CString szPath, szLine;

	CTime theTime = CTime::GetCurrentTime();

	nYear	= theTime.GetYear();
	nMonth	= theTime.GetMonth();
	nDay	= theTime.GetDay();
	nHour	= theTime.GetHour();
	nMinute	= theTime.GetMinute();
	nSecond	= theTime.GetSecond();
	
	szYear.Format("%d", nYear);
	szYear = szYear.Right(4);

	szMonth.Format("%d", nMonth);
	if ( nMonth < 10 )
		szMonth = "0" + szMonth;

	szDay.Format("%d", nDay);
	if ( nDay < 10 )
		szDay = "0" + szDay;

	szHour.Format("%d", nHour);
	if ( nHour < 10 )
		szHour = "0" + szHour;

	szMinute.Format("%d", nMinute);
	if ( nMinute < 10 )
		szMinute = "0" + szMinute;

	szSecond.Format("%d", nSecond);
	if ( nSecond < 10 )
		szSecond = "0" + szSecond;

	szPath = "c:\\MapSorter\\UserData\\OutputFile\\SCNData\\SCNParameter_" + szYear + szMonth + szDay + szHour + szMinute + szSecond + ".txt";
	
	if(Log.Open(szPath, CFile::modeReadWrite))
	{	
		//Open Passed
		Log.Close(); //close the file
		Log.Open(szPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite); //open and create file
		Log.SeekToEnd(); //overwrite?
	}
	else
	{	
		//Open Failed
		_mkdir("c:\\MapSorter\\UserData\\OutputFile\\SCNData");
		Log.Open(szPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite); //open and create file
		Log.SeekToEnd(); //overwrite?
	}
	
	szLine.Format("Horizontal:%d\n", m_lSCNIndex_X);
	Log.WriteString(szLine);//

	szLine.Format("Vertical:%d\n", m_lSCNIndex_Y);
	Log.WriteString(szLine);//

	szLine.Format("Total:%d\n", m_lScnAlignTotalPoints);
	Log.WriteString(szLine);//

	szLine.Format("Row:%d\n", m_lScnHmiRow);
	Log.WriteString(szLine);

	szLine.Format("Col:%d\n", m_lScnHmiCol);
	Log.WriteString(szLine);

	szLine.Format("Keep Align Die:%d\n", m_bKeepSCNAlignDie);
	Log.WriteString(szLine);//

	szLine.Format("SCN File Check:%d\n", m_bCheckSCNLoaded);
	Log.WriteString(szLine);//

	szLine.Format("Prescan if Wafer Bonded:%d\n", m_bScnPrescanIfBonded);
	Log.WriteString(szLine);//

	szLine.Format("User Refer Die:%d\n", m_bScnAlignUseRefer);
	Log.WriteString(szLine);//

	szLine.Format("RefDie No:%d\n", m_lScnAlignReferNo);
	Log.WriteString(szLine);//

	szLine.Format("SCN Align:%d\n", m_lScnAlignMethod);
	Log.WriteString(szLine);//

	szLine.Format("Go Type:%d\n", m_ulPrescanGoToType);
	Log.WriteString(szLine);

	szLine.Format("Scn Error To Go:%d\n", m_bErrorChooseGoFPC);
	Log.WriteString(szLine);//

	szLine.Format("Enable Check Null Bin In Map:%d\n", m_bCheckNullBinInMap);
	Log.WriteString(szLine);//

	szLine.Format("Enable Checking:%d\n", m_bEnableSCNCheck);
	Log.WriteString(szLine);//

	szLine.Format("Use RefDie:%d\n", m_bScnCheckIsRefDie);
	Log.WriteString(szLine);//

	szLine.Format("RefDie No:%d\n", m_lScnCheckRefDieNo);
	Log.WriteString(szLine);//

	szLine.Format("Set As Ref. Die:%d\n", m_bScnCheckDieAsRefDie);
	Log.WriteString(szLine);//

	szLine.Format("Check Normal:%d\n", m_bScnCheckNormalOnRefer);
	Log.WriteString(szLine);//

	szLine.Format("Alarm Limit:%d\n", m_lScnCheckAlarmLimit);
	Log.WriteString(szLine);//

	szLine.Format("Enable Ref Check Adjustment:%d\n", m_bEnableScnCheckAdjust);
	Log.WriteString(szLine);//

	szLine.Format("Ref Home Row:%d\n", m_lRefHomeDieRow);
	Log.WriteString(szLine);//

	szLine.Format("Ref Home Col:%d\n", m_lRefHomeDieCol);
	Log.WriteString(szLine);//

	szLine.Format("Ref Row Offset:%d\n", m_lRefHomeDieRowOffset);
	Log.WriteString(szLine);

	szLine.Format("Ref Col Offset:%d\n", m_lRefHomeDieColOffset);
	Log.WriteString(szLine);

	szLine.Format("Total Check Die:%d\n", m_lTotalSCNCount);
	Log.WriteString(szLine);

	szLine.Format("Scn Die X Offset:%d\n", m_lScnCheckXTol);
	Log.WriteString(szLine);//

	szLine.Format("Scn Die Y Offset:%d\n", m_lScnCheckYTol);
	Log.WriteString(szLine);//

	Log.Close(); //close the file
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::CheckSamplingRescanIgnoreGradeList(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CUIntArray aulIgnoreGradeList;
	if (!m_pWaferMapManager->GetSamplingRescanIgnoreGradeList(aulIgnoreGradeList))
	{
		HmiMessage("Input Value error" ,"", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
		m_pWaferMapManager->m_szSamplingRescanIgnoreGradeList.Empty();
	}
//	WM_CMarkDieRegionManager *pMarkDieRegionManager = WM_CMarkDieRegionManager::Instance();
//	pMarkDieRegionManager->AutoUpdateMarkDieCoord();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//andrewng //2020-0630
LONG CWaferTable::WTMoveFromTempFile(IPC_CServiceMessage &svMsg)		//andrewng //2020-0630
{
	BOOL bReturn = TRUE;
	CString szMsg;

	typedef struct 
	{
		LONG 	lEncX;
		LONG	lEncY;	
	} BLKDATA;
	BLKDATA stInfo;

	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

	if (IsWithinWaferLimit(stInfo.lEncX, stInfo.lEncY))
	{
		szMsg.Format("WTMoveFromTempFile: WT Move To (%ld, %ld)", stInfo.lEncX, stInfo.lEncY);
		HmiMessage(szMsg);

		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);

		if (X_IsPowerOn() && Y_IsPowerOn())		
		{
			XY_MoveTo(stInfo.lEncX, stInfo.lEncY, SFM_WAIT);	
		}

		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
	}
	else
	{
		szMsg.Format("ERROR: WTMoveFromTempFile: WT Move To (%ld, %ld) fail !!", 
						stInfo.lEncX, stInfo.lEncY);
		HmiMessage(szMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::MoveTableToDummyPos(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;

	BOOL bToDummy = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bToDummy);

	LONG lX = -64880;
	LONG lY = -85640;

	if (bToDummy)
	{
		GetEncoderValue();
		m_lBackupDummyPosX = m_lEnc_X;
		m_lBackupDummyPosY = m_lEnc_Y;

		szMsg.Format("WaferTable: WT Move To DUMMY Pos (%ld, %ld)", lX, lY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage(szMsg);

		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);

		if (X_IsPowerOn() && Y_IsPowerOn())		
		{
			XY_MoveTo(lX, lY, SFM_WAIT);	
		}

		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
	}
	else
	{
		szMsg.Format("WaferTable: WT DUMMY Move To backup Pos (%ld, %ld)", m_lBackupDummyPosX, m_lBackupDummyPosY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);

		if (IsWithinWaferLimit(m_lBackupDummyPosX, m_lBackupDummyPosY))
		{
			if (X_IsPowerOn() && Y_IsPowerOn())		
			{
				XY_MoveTo(m_lBackupDummyPosX, m_lBackupDummyPosY, SFM_WAIT);	
			}
		}
		else
		{
			szMsg.Format("WaferTable: WT DUMMY Move To backup Pos (%ld, %ld) FAIL", m_lBackupDummyPosX, m_lBackupDummyPosY);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			bReturn = FALSE;
		}

		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);

		m_lBackupDummyPosX = 0;
		m_lBackupDummyPosY = 0;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
