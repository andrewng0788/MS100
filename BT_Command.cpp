#include <stdio.h>
#include <math.h>
#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "HmiDataManager.h"
#include "BinTable.h"
#include "FileUtil.h"
#include "BT_CmdName.h"
#include "GenerateDatabase.h"
#include "TakeTime.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "MachineStat.h"
#include "Bondhead.h"
#include "BondPr.h"
#include "NGGrade.h"

#pragma once

/////////////////////////////////////////////////////////////////
//Register Variables and Functions
/////////////////////////////////////////////////////////////////
VOID CBinTable::RegisterVariables()
{
	try
	{
		CString szTemp;
		/***************************/
		/*    Reg Hmi Variables    */
		/***************************/
		RegVariable(_T("BT_bDisableBT"),		&m_bDisableBT);
		RegVariable(_T("BT_bHomeSnr_X"),		&m_bHomeSnr_X);
		RegVariable(_T("BT_bHomeSnr_Y"),		&m_bHomeSnr_Y);

		RegVariable(_T("BT_bPosLimitSnr_X"),	&m_bPosLimitSnr_X);
		RegVariable(_T("BT_bNegLimitSnr_X"),	&m_bNegLimitSnr_X);
		RegVariable(_T("BT_bPosLimitSnr_Y"),	&m_bPosLimitSnr_Y);
		RegVariable(_T("BT_bNegLimitSnr_Y"),	&m_bNegLimitSnr_Y);
		RegVariable(_T("BT_bPosLimitSnr_X2"),	&m_bPosLimitSnr_X2);
		RegVariable(_T("BT_bNegLimitSnr_X2"),	&m_bNegLimitSnr_X2);
		RegVariable(_T("BT_bPosLimitSnr_Y2"),	&m_bPosLimitSnr_Y2);
		RegVariable(_T("BT_bNegLimitSnr_Y2"),	&m_bNegLimitSnr_Y2);

		RegVariable(_T("BT_lEnc_X"),			&m_lEnc_X);
		RegVariable(_T("BT_lEnc_Y"),			&m_lEnc_Y);
		RegVariable(_T("BT_lEnc_T"),			&m_lEnc_T);
		RegVariable(_T("BT_lEnc_X2"),			&m_lEnc_X2);
		RegVariable(_T("BT_lEnc_Y2"),			&m_lEnc_Y2);

		RegVariable(_T("BT_bIsPowerOn_X"),		&m_bIsPowerOn_X);
		RegVariable(_T("BT_bIsPowerOn_Y"),		&m_bIsPowerOn_Y);
		RegVariable(_T("BT_bIsPowerOn_T"),		&m_bIsPowerOn_T);
 		RegVariable(_T("BT_bIsPowerOn_X2"),		&m_bIsPowerOn_X2);
		RegVariable(_T("BT_bIsPowerOn_Y2"),		&m_bIsPowerOn_Y2);

		RegVariable(_T("BT_bSel_X"),			&m_bSel_X);
		RegVariable(_T("BT_bSel_Y"),			&m_bSel_Y);
		RegVariable(_T("BT_bSel_T"),			&m_bSel_T);
		RegVariable(_T("BT_bSel_X2"),			&m_bSel_X2);
		RegVariable(_T("BT_bSel_Y2"),			&m_bSel_Y2);

		RegVariable(_T("BT_bEnableT"),			&m_bEnable_T);				//v4.39T7	//Nichia
		RegVariable(_T("BT_lJsBinTableInUse"),	&m_lJsBinTableInUse);		//MS100 9Inch dual-table config		//v4.17T1	
		RegVariable(_T("BT_lBinCalibX"),		&m_lBinCalibX);
		RegVariable(_T("BT_lBinCalibY"),		&m_lBinCalibY);
		//v4.51A19	//Silan MS90 NGPick
		RegVariable(_T("BT_lNGPickPocketX"),	&m_lNGPickPocketX);
		RegVariable(_T("BT_lNGPickPocketY"),	&m_lNGPickPocketY);

		/* REMARK: Only delay for long distance (normal profile) is user programmable. 
					Delay for short distance (fast profile) is hard-coded to prevent
					too many user-defined delay */
//		RegVariable(_T("BT_lBTDelay"), &m_lBTDelay);

		//Global
		RegVariable(_T("BT_bReply"), &m_bReply);

		//Enability of Hmi Controls
		RegVariable(_T("BT_bIfEnableTBEditBlkSettings"),			&m_bIfEnableTBEditBlkSettings);
		RegVariable(_T("BT_bIfEnableTBClrBlkSettings"),				&m_bIfEnableTBClrBlkSettings);

		RegVariable(_T("BT_bIfEnableCBClrBlkSettingsSelection"),	&m_bIfEnableCBClrBlkSettingsSelection);
		RegVariable(_T("BT_bIfEnablePIBlkToClrSettings"),			&m_bIfEnablePIBlkToClrSettings);
		RegVariable(_T("BT_bIfEnableTBSubmitClrBlkSettings"),		&m_bIfEnableTBSubmitClrBlkSettings);

		RegVariable(_T("BT_bIfEnableResortDie"),					&m_bIfEnableResortDie);
		RegVariable(_T("BT_bIfEnableCBSetupMode"),					&m_bIfEnableCBSetupMode);
		RegVariable(_T("BT_bIfEnablePINoOfBlk"),					&m_bIfEnablePINoOfBlk);
		RegVariable(_T("BT_bIfEnablePIBlkToSetup"),					&m_bIfEnablePIBlkToSetup);
		RegVariable(_T("BT_bIfEnableTBSetUL"),						&m_bIfEnableTBSetUL);
		RegVariable(_T("BT_bIfEnableTBSetLR"),						&m_bIfEnableTBSetLR);
		RegVariable(_T("BT_bIfEnableTBConfirmUL"),					&m_bIfEnableTBConfirmUL);
		RegVariable(_T("BT_bIfEnableTBConfirmLR"),					&m_bIfEnableTBConfirmLR);
		RegVariable(_T("BT_bIfEnableDDUpperLeftX"),					&m_bIfEnableDDUpperLeftX);
		RegVariable(_T("BT_bIfEnableDDUpperLeftY"),					&m_bIfEnableDDUpperLeftY);
		RegVariable(_T("BT_bIfEnableDDLowerRightX"),				&m_bIfEnableDDLowerRightX);
		RegVariable(_T("BT_bIfEnableDDLowerRightY"),				&m_bIfEnableDDLowerRightY);
		RegVariable(_T("BT_bIfEnablePIBlkPitchX"),					&m_bIfEnablePIBlkPitchX);
		RegVariable(_T("BT_bIfEnablePIBlkPitchY"),					&m_bIfEnablePIBlkPitchY);
		RegVariable(_T("BT_bIfEnablePIGrade"),						&m_bIfEnablePIGrade);
		RegVariable(_T("BT_bIfEnablePIDiePitchX"),					&m_bIfEnablePIDiePitchX);
		RegVariable(_T("BT_bIfEnablePIDiePitchY"),					&m_bIfEnablePIDiePitchY);
		RegVariable(_T("BT_bIfEnableCBWalkPath"),					&m_bIfEnableCBWalkPath);
		RegVariable(_T("BT_bIfEnableVDNoOfDiePerBlk"),				&m_bIfEnableVDNoOfDiePerBlk);
		RegVariable(_T("BT_bIfEnableVDNoOfDiePerRow"),				&m_bIfEnableVDNoOfDiePerRow);
		RegVariable(_T("BT_bIfEnablePIDiePerRow"),					&m_bIfEnablePIDiePerRow);
		RegVariable(_T("BT_bIfEnablePIDiePerCol"),					&m_bIfEnablePIDiePerCol);
		RegVariable(_T("BT_bIfEnablePIDiePerBlk"),					&m_bIfEnablePIDiePerBlk);
		RegVariable(_T("BT_bIfEnableChkBAutoAssignGrade"),			&m_bIfEnableChkBAutoAssignGrade);
		
		RegVariable(_T("BT_bIfEnableFirstRowColSkipPattern"),		&m_bIfEnableFirstRowColSkipPattern);	//v3.33T3
		RegVariable(_T("BT_bIfEnableTeachWithPhysicalBlk"),			&m_bIfEnableTeachWithPhysicalBlk);	//v3.33T3
		
		RegVariable(_T("BT_bIfEnable2DBarcodeOutput"),				&m_bIfEnable2DBarcodeOutput);	//v3.33T3
		RegVariable(_T("BT_bIfEnableTemplateSetup"),				&m_bIfEnableTemplateSetup);	//v3.33T3
		RegVariable(_T("BT_bIfEnableCentralizedBondArea"),			&m_bIfEnableCentralizedBondArea);
		
		RegVariable(_T("BT_bIfEnableWafflePad"),					&m_bIfEnableWafflePad);
		RegVariable(_T("BT_bIfEnableWafflePadCheckBox"),			&m_bIfEnableWafflePadCheckBox);
		RegVariable(_T("BT_bIfEnablePadPitchSetX"),					&m_bIfEnablePadPitchSetX);
		RegVariable(_T("BT_bIfEnablePadPitchSetY"),					&m_bIfEnablePadPitchSetY);
		RegVariable(_T("BT_bIfEnablePadPitchResetX"),				&m_bIfEnablePadPitchResetX);
		RegVariable(_T("BT_bIfEnablePadPitchResetY"),				&m_bIfEnablePadPitchResetY);
		RegVariable(_T("BT_bIfEnablePadDimX"),						&m_bIfEnablePadDimX);
		RegVariable(_T("BT_bIfEnablePadDimY"),						&m_bIfEnablePadDimY);
		RegVariable(_T("BT_bIfEnableWafflePadPageBack"),			&m_bIfEnableWafflePadPageBack);

		RegVariable(_T("BT_bIfUseBlockCornerAsFirstDiePos"),		&m_bIfUseBlockCornerAsFirstDiePos);
		
		RegVariable(_T("BT_bIfEnableTBSubmit"),			&m_bIfEnableTBSubmit);
		RegVariable(_T("BT_bIfEnableTBCancel"),			&m_bIfEnableTBCancel);

		//Bintable Limits & unload position
		RegVariable(_T("BT_lTableXNegLimit"),			&m_lTableXNegLimit);
		RegVariable(_T("BT_lTableXPosLimit"),			&m_lTableXPosLimit);
		RegVariable(_T("BT_lTableYNegLimit"),			&m_lTableYNegLimit);
		RegVariable(_T("BT_lTableYPosLimit"),			&m_lTableYPosLimit);
		RegVariable(_T("BT_lBinUnloadPosX"),			&m_lBTUnloadPos_X);
		RegVariable(_T("BT_lBinUnloadPosY"),			&m_lBTUnloadPos_Y);
		RegVariable(_T("BT_lTableX2NegLimit"),			&m_lTableX2NegLimit);
		RegVariable(_T("BT_lTableX2PosLimit"),			&m_lTableX2PosLimit);
		RegVariable(_T("BT_lTableY2NegLimit"),			&m_lTableY2NegLimit);
		RegVariable(_T("BT_lTableY2PosLimit"),			&m_lTableY2PosLimit);
		RegVariable(_T("BT_lBinUnloadPosX2"),			&m_lTableUnloadX2Pos);
		RegVariable(_T("BT_lBinUnloadPosY2"),			&m_lTableUnloadY2Pos);

		//Collet Offset 
		RegVariable(_T("BT_lColletOffsetX"),			&m_lColletOffsetX);
		RegVariable(_T("BT_lColletOffsetY"),			&m_lColletOffsetY);

		//Clear Block Settings
		RegVariable(_T("BT_szClrBlkSettingsSelection"), &m_szClrBlkSettingsSelection);

		//Clear Physical Block Settings 
		RegVariable(_T("BT_ulPhyBlkToClrSettings"),		&m_ulPhyBlkToClrSettings);

		//Clear Bin Block Settings
		RegVariable(_T("BT_ulBinBlkToClrSettings"),		&m_ulBinBlkToClrSettings);

		//The 2 cross
		RegVariable(_T("BT_dBinTwoCrossXDistance"),		&m_dBinTwoCrossXDistance);
		RegVariable(_T("BT_dBinTwoCrossYDistance"),		&m_dBinTwoCrossYDistance);
		//RegVariable(_T("BT_dThetaOffsetByCross"),		&m_dThetaOffsetByCross);

		//Block Setup
		RegVariable(_T("BT_szSetupMode"),				&m_szSetupMode);
		RegVariable(_T("BT_bFirstTimeSetUL"),			&m_bFirstTimeSetUL);
		RegVariable(_T("BT_bFirstTimeSetLR"),			&m_bFirstTimeSetLR);

		//Physical Block Setup
		RegVariable(_T("BT_ulNoOfPhyBlk"),				&m_ulNoOfPhyBlk);
		RegVariable(_T("BT_lPhyBlkPitchX"),				&m_lPhyBlkPitchX);
		RegVariable(_T("BT_lPhyBlkPitchY"),				&m_lPhyBlkPitchY);
		RegVariable(_T("BT_lPhyUpperLeftX"),			&m_lPhyUpperLeftX);
		RegVariable(_T("BT_lPhyUpperLeftY"),			&m_lPhyUpperLeftY);
		RegVariable(_T("BT_lPhyLowerRightX"),			&m_lPhyLowerRightX);
		RegVariable(_T("BT_lPhyLowerRightY"),			&m_lPhyLowerRightY);
		RegVariable(_T("BT_ulPhyBlkToSetup"),			&m_ulPhyBlkToSetup);

		//Bin Block Setup
		RegVariable(_T("BT_szBinBlkTemplate"),			&m_szBinBlkTemplate);
		RegVariable(_T("BT_szBinBlkTemplateSel"),		&m_szBinBlkTemplateSel);
		RegVariable(_T("BT_ulTemplateSetupInputCount"),	&m_ulTemplateSetupInputCount);
		
		RegVariable(_T("BT_ulNoOfBinBlk"),				&m_ulNoOfBinBlk);
		RegVariable(_T("BT_lBinBlkPitchX"),				&m_lBinBlkPitchX);
		RegVariable(_T("BT_lBinBlkPitchY"),				&m_lBinBlkPitchY);
		RegVariable(_T("BT_lBinUpperLeftX"),			&m_lBinUpperLeftX);
		RegVariable(_T("BT_lBinUpperLeftY"),			&m_lBinUpperLeftY);
		RegVariable(_T("BT_lBinLowerRightX"),			&m_lBinLowerRightX);
		RegVariable(_T("BT_lBinLowerRightY"),			&m_lBinLowerRightY);
		RegVariable(_T("BT_ucGrade"),					&m_ucGrade);
//		RegVariable(_T("BT_lDiePitchX"),				&m_lDiePitchX);
//		RegVariable(_T("BT_lDiePitchY"),				&m_lDiePitchY);
		RegVariable(_T("BT_dDiePitchX"),				&m_dDiePitchX);			//v4.59A19	//MS90 with 0.1um encoder
		RegVariable(_T("BT_dDiePitchY"),				&m_dDiePitchY);			//v4.59A19	//MS90 with 0.1um encoder
		RegVariable(_T("BT_bUsePt5UmInDiePitchX"),		&m_bUsePt5UmInDiePitchX);
		RegVariable(_T("BT_bUsePt5UmInDiePitchY"),		&m_bUsePt5UmInDiePitchY);
		RegVariable(_T("BT_dBondAreaOffsetXInUm"),		&m_dBondAreaOffsetXInUm);
		RegVariable(_T("BT_dBondAreaOffsetYInUm"),		&m_dBondAreaOffsetYInUm);
		RegVariable(_T("BT_dThermalDeltaPitchXInUm"),	&m_dThermalDeltaPitchXInUm);		//v4.59A22	//David Ma
		RegVariable(_T("BT_dThermalDeltaPitchYInUm"),	&m_dThermalDeltaPitchYInUm);		//v4.59A22	//David Ma

		RegVariable(_T("BT_szWalkPath"),				&m_szWalkPath);
		RegVariable(_T("BT_ulWalkPath"),				&m_ulWalkPath);
		RegVariable(_T("BT_ulDiePerBlk"),				&m_ulDiePerBlk);
		RegVariable(_T("BT_ulDiePerRow"),				&m_ulDiePerRow);
		RegVariable(_T("BT_ulDiePerCol"),				&m_ulDiePerCol);
		RegVariable(_T("BT_ulDiePerUserRow"),			&m_ulDiePerUserRow);
		RegVariable(_T("BT_ulDiePerUserCol"),			&m_ulDiePerUserCol);
		RegVariable(_T("BT_ulSkipUnit"),				&m_ulSkipUnit);
		RegVariable(_T("BT_ulMaxUnit"),					&m_ulMaxUnit);
		RegVariable(_T("BT_bEnableFirstRowColSkipPattern"), &m_bEnableFirstRowColSkipPattern);
		RegVariable(_T("BT_ulFirstRowColSkipUnit"),		&m_ulFirstRowColSkipUnit);
		RegVariable(_T("BT_ulBinBlkToSetup"),			&m_ulBinBlkToSetup);
		RegVariable(_T("BT_bTeachWithPhysicalBlk"),		&m_bTeachWithPhysicalBlk);
		RegVariable(_T("BT_bCentralizedBondArea"),		&m_bCentralizedBondArea);
		RegVariable(_T("BT_lCentralizedUpperLeftX"),	&m_lCentralizedUpperLeftX);
		RegVariable(_T("BT_lCentralizedUpperLeftY"),	&m_lCentralizedUpperLeftY);
		RegVariable(_T("BT_bByPassBinMap"),				&m_bByPassBinMap);		//v4.48A11	//Semitek, 3E DL
		
		RegVariable(_T("BT_bEnableWafflePad"),			&m_bEnableWafflePad);
		RegVariable(_T("BT_lWafflePadDistX"),			&m_lWafflePadDistX);
		RegVariable(_T("BT_lWafflePadDistY"),			&m_lWafflePadDistY);
		RegVariable(_T("BT_lWafflePadSizeX"),			&m_lWafflePadSizeX);
		RegVariable(_T("BT_lWafflePadSizeY"),			&m_lWafflePadSizeY);
	
		RegVariable(_T("BT_dSpeedModeOffsetX"),			&m_dSpeedModeOffsetX);
		RegVariable(_T("BT_dSpeedModeOffsetY"),			&m_dSpeedModeOffsetY);

		RegVariable(_T("BT_bAutoAssignGrade"),			&m_bAutoAssignGrade);
		RegVariable(_T("BT_bAssignSameGradeForAllBin"),	&m_bAssignSameGradeForAllBin);

		RegVariable(_T("BT_ucStartingGrade"),			&m_ucStartingGrade);
		RegVariable(_T("BT_ulSourcePhyBlk"),			&m_ulSourcePhyBlk);
		RegVariable(_T("BT_ulBinSerialNoFormat"),		&m_ulBinSerialNoFormat);
		RegVariable(_T("BT_ulBinSerialNoMaxLimit"),		&m_ulBinSerialNoMaxLimit);
		RegVariable(_T("BT_ulBinClearFormat"),			&m_ulBinClearFormat);
		RegVariable(_T("BT_ulBinClearMaxLimit"),		&m_ulBinClearMaxLimit);
		RegVariable(_T("BT_ulBinClearInitCount"),		&m_ulBinClearInitCount);
		RegVariable(_T("BT_bSaveBinBondedCount"),		&m_bSaveBinBondedCount);
		RegVariable(_T("BT_bOptimizeBinGrade"),			&m_bOptimizeBinGrade);
		RegVariable(_T("BT_bLSBondPattern"),			&m_bLSBondPattern);

		RegVariable(_T("BT_bEnableCircularBondArea"),	&m_bEnableCircularBondArea);
		RegVariable(_T("BT_lCirAreaCenterX"),			&m_lCirAreaCenterX);
		RegVariable(_T("BT_lCirAreaCenterY"),			&m_lCirAreaCenterY);
		RegVariable(_T("BT_dCirAreaRadius"),			&m_dCirAreaRadius);
		RegVariable(_T("BT_bEnableBinMapBondArea"),		&m_bEnableBinMapBondArea);		//v4.03		//PLLM REBEL
		RegVariable(_T("BT_bEnableBinMapCDieOffset"),	&m_bEnableBinMapCDieOffset);	//v4.42T1
		RegVariable(_T("BT_dBinMapCircleRadiusInMm"),	&m_dBinMapCircleRadiusInMm);	//v4.36
		RegVariable(_T("BT_lBinMapEdgeSize"),			&m_lBinMapEdgeSize);			//v4.36T9
		RegVariable(_T("BT_lBinMapBEdgeSize"),			&m_lBinMapBEdgeSize);			//v4.36T10
		RegVariable(_T("BT_lBinMapLEdgeSize"),			&m_lBinMapLEdgeSize);			//v4.36T10
		RegVariable(_T("BT_lBinMapREdgeSize"),			&m_lBinMapREdgeSize);			//v4.36T10
		RegVariable(_T("BT_lBinMapCDieRowOffset"),		&m_lBinMapCDieRowOffset);		//v4.42T1
		RegVariable(_T("BT_lBinMapCDieColOffset"),		&m_lBinMapCDieColOffset);		//v4.42T1
		RegVariable(_T("BT_lBinMapXOffset"),			&m_lBinMapXOffset);
		RegVariable(_T("BT_lBinMapYOffset"),			&m_lBinMapYOffset);
		RegVariable(_T("BT_szBinMapFilePath"),			&m_szBinMapFilePath);			//v4.27T11
		RegVariable(_T("BT_ucSpGrade1ToByPassBinMap"),	&m_ucSpGrade1ToByPassBinMap);	//v4.48A2

		RegVariable(_T("BT_bPkgLoadBinMap"),			&m_bPkgLoadBinMap);

		//v4.47T13
		RegVariable(_T("BT_bEnableOsramBinMixMap"),		&m_bEnableOsramBinMixMap);
		RegVariable(_T("BT_lBinMixPatternQty"),			&m_lBinMixPatternQty);
		RegVariable(_T("BT_ulBinMixPatternType"),		&m_ulBinMixPatternType);
		RegVariable(_T("BT_ucBinMixTypeAGrade"),		&m_ucBinMixTypeAGrade);
		RegVariable(_T("BT_ucBinMixTypeBGrade"),		&m_ucBinMixTypeBGrade);
		RegVariable(_T("BT_lBinMixTypeAQty"),			&m_lBinMixTypeAQty);
		RegVariable(_T("BT_lBinMixTypeBQty"),			&m_lBinMixTypeBQty);
		RegVariable(_T("BT_lBinMixTypeAStopPoint"),			&m_lBinMixTypeAStopPoint);
		RegVariable(_T("BT_lBinMixTypeBStopPoint"),			&m_lBinMixTypeBStopPoint);

		RegVariable(_T("BT_bUseBlockCornerAsFirstDiePos"),	&m_bUseBlockCornerAsFirstDiePos);
		RegVariable(_T("BT_bNoReturnTravel"),			&m_bNoReturnTravel);
		RegVariable(_T("BT_szPKGFilenameDisplay"),		&m_szPKGFilenameDisplay);
		RegVariable(_T("BT_ulPKGDiePitchXDisplay"),		&m_ulPKGDiePitchXDisplay);
		RegVariable(_T("BT_ulPKGDiePitchYDisplay"),		&m_ulPKGDiePitchYDisplay);
		RegVariable(_T("BT_ulPKGInputCountDisplay"),	&m_ulPKGInputCountDisplay);
		RegVariable(_T("BT_ulPKGDiePerRowDisplay"),		&m_ulPKGDiePerRowDisplay);
		RegVariable(_T("BT_ulPKGDiePerColDisplay"),		&m_ulPKGDiePerColDisplay);
		RegVariable(_T("BT_ulPKGBlkCapacityDisplay"),	&m_ulPKGBlkCapacityDisplay);
		RegVariable(_T("BT_ulPKGNoOfBinBlkDisplay"),	&m_ulPKGNoOfBinBlkDisplay);

		//Clear Bin Counter (by Physical Block)
		RegVariable(_T("BT_szClrBinCntMode"), &m_szClrBinCntMode);
		
		// Clear Bin Counter (by Grade)
		RegVariable(_T("BT_szClrBinByGradeMode"),		&m_szClrBinByGradeMode);
		RegVariable(_T("BT_bIsClrAllGrades"),			&m_bIsClrAllGrades);
		RegVariable(_T("BT_ucGradeToClrCount"),			&m_ucGradeToClrCount);
		RegVariable(_T("BT_bIfGenOFileIfClrByGrade"),	&m_bGenOFileIfClrByGrade);
		RegVariable(_T("BT_bUseUniqueSerialNum"),		&m_bUseUniqueSerialNum);		//v3.23T1
		RegVariable(_T("BT_bUseMultipleSerialCounter"),	&m_bUseMultipleSerialCounter);	//v3.23T1
		
		RegVariable(_T("BT_ulPhyBlkToClrCount"),		&m_ulPhyBlkToClrCount);
		RegVariable(_T("BT_bIfIsClrAllBinCtr"),			&m_bIfIsClrAllBinCtr);
		RegVariable(_T("BT_bIfGenOutputFile"),			&m_bIfGenOutputFile);
		RegVariable(_T("BT_bClearBinInCopyTempFileMode"),	&m_bClearBinInCopyTempFileMode);
		//RegVariable(_T("BT_bEnableClearBinCopyTempFile"),	&m_bEnableClearBinCopyTempFile);
		RegVariable(_T("BT_bIfGenLabel"),				&m_bIfGenLabel);
		RegVariable(_T("BT_bIfPrintLabel"),				&m_bIfPrintLabel);
		RegVariable(_T("BT_bUseBarcodeAsFileName"),		&m_bUseBarcodeAsFileName);
		//RegVariable(_T("BT_szFileNo"),				&m_szFileNo);
		//RegVariable(_T("BT_szBlueTapeNo"),			&m_szBlueTapeNo);
		RegVariable(_T("BT_szSpecVersion"),				&m_szSpecVersion);
		//RegVariable(_T("BT_szOutputFilename"),		&m_szOutputFilename);
		//RegVariable(_T("BT_szLabelOrientation"),		&m_szLabelOrientation);
		RegVariable(_T("BT_szLabelFilePath"),			&m_szLabelFilePath);
		RegVariable(_T("BT_dAlignLastDieOffsetX"),		&m_dAlignLastDieOffsetX);		//v2.63
		RegVariable(_T("BT_dAlignLastDieOffsetY"),		&m_dAlignLastDieOffsetY);		//v2.63
		RegVariable(_T("BT_dAlignLastDieRelOffsetX"),	&m_dAlignLastDieRelOffsetX);	//v4.26T1
		RegVariable(_T("BT_dAlignLastDieRelOffsetY"),	&m_dAlignLastDieRelOffsetY);	//v4.26T1	
		RegVariable(_T("BT_szOutputFileSummaryPath"),	&m_szOutputFileSummaryPath);
		RegVariable(_T("BT_szWaferEndSummaryPath"),		&m_szWaferEndSummaryPath);
		RegVariable(_T("BT_bUseFrameCrossAlignment"),	&m_bUseFrameCrossAlignment);	//v4.59A1

		//Step Move
		RegVariable(_T("BT_ulTableToStepMove"),			&m_ulTableToStepMove);
		RegVariable(_T("BT_ulBinBlkToStepMove"),		&m_ulBinBlkToStepMove);
		RegVariable(_T("BT_ulCurrBondIndex"),			&m_ulCurrBondIndex);
		RegVariable(_T("BT_ulNoOfBondedDie"),			&m_ulNoOfBondedDie);
		RegVariable(_T("BT_lTempIndex"),				&m_lTempIndex);
		RegVariable(_T("BT_ulHowManySteps"),			&m_ulHowManySteps);
		RegVariable(_T("BT_szStepMoveMsg"),				&m_szStepMoveMsg);

		//Grade Information
		RegVariable(_T("BT_ulNoOfSortedDie"),			&m_ulNoOfSortedDie);
		RegVariable(_T("BT_ulBinBlkInUse"),				&m_ulBinBlkInUse);
		RegVariable(_T("BT_ulSortedDieIndex"),			&m_ulSortedDieIndex);
		RegVariable(_T("BT_ulInputCount"),				&m_ulInputCount);	
		RegVariable(_T("BT_ulFullDieCountInput"),		&m_ulFullDieCountInput);
		RegVariable(_T("BT_ulMinDieCountInput"),		&m_ulMinDieCountInput);
		RegVariable(_T("BT_bEnableDieCountInput"),		&m_bEnableDieCountInput);
		RegVariable(_T("BT_ulGradeCapacity"),			&m_ulGradeCapacity);
		RegVariable(_T("BT_ucAliasGrade"),				&m_ucAliasGrade);
		RegVariable(_T("BT_bEnableAliasGrade"),			&m_bEnableAliasGrade);

		// For Step Move
		RegVariable(_T("BT_lStepMove"),					&m_lStepMove);

		// Joystick
		RegVariable(_T("BT_lJoystickLevel"),			&m_lJoystickLevel);	
		RegVariable(_T("BT_bJoystickOn"),				&m_bJoystickOn);

		//Output File
		RegVariable(_T("BT_bOutputFilePath2"),			&m_bEnableBinOutputFilePath2);
		RegVariable(_T("BT_szOutputFilePath2"),			&m_szOutputFilePath2);
		RegVariable(_T("BT_szOutputFilePath3"),			&m_szOutputFilePath3);
		RegVariable(_T("BT_szOutputFilePath4"),			&m_szOutputFilePath4);
		RegVariable(_T("BT_szOutputFilePath"),			&m_szOutputFilePath);
		RegVariable(_T("BT_szBinOutputFileFormat"),		&m_szBinOutputFileFormat);
		RegVariable(_T("BT_szLabelPrinterSel"),			&m_szLabelPrinterSel);
		RegVariable(_T("BT_bAutoClearBin"),				&m_bAutoClearBin);
		RegVariable(_T("BT_bCheckOutputFileRepeat"),	&m_bCheckOutputFileRepeat); // SanAn 4.51D6
		RegVariable(_T("BT_szOutputFileBlueTapeNo"),	&m_szOutputFileBlueTapeNo); // v4.51D10 Dicon 

		//Wafe end Output File
		RegVariable(_T("BT_bIfGenWaferEndFile"),		&m_bIfGenWaferEndFile);
		RegVariable(_T("BT_szWaferEndPath"),			&m_szWaferEndPath);
		RegVariable(_T("BT_szWaferEndFileFormat"),		&m_szWaferEndFileFormat);

		RegVariable(_T("BT_dWaferEndSortingYield"),		&m_dWaferEndSortingYield);

		RegVariable(_T("BT_bStopBinLoader"),			&m_bStopBinLoader);

		//Bin Summary Output File
		RegVariable(_T("BT_szBinSummaryOutputPath"),	&m_szBinSummaryOutputPath);	

		//Use Filename as WaferID
		RegVariable(_T("BT_bFilenameAsWaferID"),		&m_bFilenameAsWaferID);

		RegVariable(_T("BT_bUseSameView"),				&m_bUseSameView);
		RegVariable(_T("BT_lAlignBinDelay"),			&m_lAlignBinDelay);
		RegVariable(_T("BT_bCheckAlignBinResult"),		&m_bCheckAlignBinResult);

		RegVariable(_T("BT_ulGradeChangeLimit"),		&m_ulGradeChangeLimit);
		RegVariable(_T("BT_bShowBinBlkImage"),			&m_bShowBinBlkImage);

		RegVariable(_T("BT_bOfflinePrintLabel"),		&m_bDisableClearAllPrintLabel);		//v2.70

		RegVariable(_T("BT_bTestRealign"),				&m_bTestRealign);			//v3.70T4

		for (INT i=0; i<BT_EXTRA_BIN_INFO_ITEM_NO ;i++)
		{
			szTemp.Format("BT_szExtraBinInfo%d",i+1);
			RegVariable(_T(szTemp),	&m_szaExtraBinInfo[i]);
			szTemp.Format("BT_szExtraBinInfoField%d",i+1);
			RegVariable(_T(szTemp),	&m_szaExtraBinInfoField[i]);
			szTemp.Format("BT_bEnableExtraBinInfo%d",i+1);
			RegVariable(_T(szTemp),	&m_bEnableExtraBinInfo[i]);
		}

		RegVariable(_T("BT_bWaferEndUploadMapMpd"),		&m_bWaferEndUploadMapMpd);
		RegVariable(_T("BT_bEnableBinOutputFile"),		&m_bEnableBinOutputFile);
		RegVariable(_T("BT_bFinForBinOutputFile"),		&m_bFinForBinOutputFile);
		RegVariable(_T("BT_bEnableBinMapFile"),			&m_bEnableBinMapFile);
		RegVariable(_T("BT_bFin2ForBinMapFile"),		&m_bFin2ForBinMapFile);
		RegVariable(_T("BT_bEnable2DBarcodeOutput"),	&m_bEnable2DBarcodeOutput);	//v3.33T3
		RegVariable(_T("BT_ulHoleDieNum"),				&m_ulHoleDieNum);
		RegVariable(_T("BT_lRealignBinFrameOption"),	&m_lRealignBinFrameOption);	//v3.70T3
		RegVariable(_T("BT_bPrPreBondAlignment"),		&m_bPrPreBondAlignment);	//v3.79
		RegVariable(_T("BT_b1ptRealignComp"),			&m_b1ptRealignComp);		//v3.86
		RegVariable(_T("BT_lCollet1PadOffsetX"),		&m_lCollet1PadOffsetX);		//CSP
		RegVariable(_T("BT_lCollet1PadOffsetY"),		&m_lCollet1PadOffsetY);		//CSP
		RegVariable(_T("BT_lCollet2PadOffsetX"),		&m_lCollet2PadOffsetX);		//CSP
		RegVariable(_T("BT_lCollet2PadOffsetY"),		&m_lCollet2PadOffsetY);		//CSP

		// RankID variable
		RegVariable(_T("BT_bEnableAutoLoadRankFile"),	&m_bEnableAutoLoadRankFile);
		RegVariable(_T("BT_bLoadRankIDFromMap"),		&m_bLoadRankIDFromMap);
		RegVariable(_T("BT_bAutoBinBlkSetupWithTempl"),	&m_bAutoBinBlkSetupWithTempl);
		RegVariable(_T("BT_szRankIDFilePath"),			&m_szRankIDFilePath);	 //v3.33T3

		RegVariable(_T("BT_bShowBinBlkTemplateSetup"),	&m_bShowBinBlkTemplateSetup);
		
		RegVariable(_T("BT_lGenSummaryPeriodNum"),		&m_lGenSummaryPeriodNum);

		RegVariable(_T("BT_szBinLotSummaryFilename"),	&m_szBinLotSummaryFilename);
		RegVariable(_T("BT_szBinLotSummaryPath"),		&m_szBinLotSummaryPath);
		RegVariable(_T("BT_szBinLotSummaryPath2"),		&m_szBinLotSummaryPath2);

		RegVariable(_T("BT_szInputCountSetupFilePath"),	&m_szInputCountSetupFilePath);
		RegVariable(_T("BT_szInputCountSetupFilename"),	&m_szInputCountSetupFilename);

		for (INT i=0; i<BT_SUMMARY_TIME_NO; i++)
		{
			szTemp.Format("BT_szBinSumGenTime%d", i + 1);
			RegVariable(_T(szTemp), &m_szBinSummaryGenTime[i]);
		}

		RegVariable(_T("BT_ucOptimizeBinCountMode"),		&m_ucOptimizeBinCountMode);
		RegVariable(_T("BT_ulOptimizeBinCount"),			&m_ulEnableOptimizeBinCount);
		RegVariable(_T("BT_bEnableBinSumWaferLotCheck"),	&m_bEnableBinSumWaferLotCheck);
		RegVariable(_T("BT_ucOpimizeBinSetupGrade"),		&m_ucOpimizeBinSetupGrade);
		RegVariable(_T("BT_ucOpimizeBinSetupGradeLimit"),	&m_ucOpimizeBinSetupGradeLimit);

		RegVariable(_T("BT_ulMinFrameDieCount"),			&m_ulMinFrameDieCount);
		RegVariable(_T("BT_ulMaxFrameDieCount"),			&m_ulMaxFrameDieCount);
		RegVariable(_T("BT_ulMinLotCount"),					&m_ulMinLotCount);
		RegVariable(_T("BT_lBarcodeMinDigit"),				&m_lBinBarcodeMinDigit);
		RegVariable(_T("BT_lBHZ2BondPosOffsetX"),			&m_lBHZ2BondPosOffsetX);
		RegVariable(_T("BT_lBHZ2BondPosOffsetY"),			&m_lBHZ2BondPosOffsetY);
		RegVariable(_T("BT_lBHZ1BondPosOffsetX"),			&m_lBHZ1BondPosOffsetX);
		RegVariable(_T("BT_lBHZ1BondPosOffsetY"),			&m_lBHZ1BondPosOffsetY);
		RegVariable(_T("BT_lBT2OffsetX"),					&m_lBT2OffsetX);
		RegVariable(_T("BT_lBT2OffsetY"),					&m_lBT2OffsetY);
		RegVariable(_T("BT_lBHZ1PrePickToBondOffsetX"),		&m_lBHZ1PrePickToBondOffsetX);
		RegVariable(_T("BT_lBHZ1PrePickToBondOffsetY"),		&m_lBHZ1PrePickToBondOffsetY);
		RegVariable(_T("BT_lBHZ2PrePickToBondOffsetX"),		&m_lBHZ2PrePickToBondOffsetX);
		RegVariable(_T("BT_lBHZ2PrePickToBondOffsetY"),		&m_lBHZ2PrePickToBondOffsetY);

		RegVariable(_T("BT_dBond180DegOffsetX_um"),			&m_dBond180DegOffsetX_um);
		RegVariable(_T("BT_dBond180DegOffsetY_um"),			&m_dBond180DegOffsetY_um);

		// Auto Clean Collet
		RegVariable(_T("BT_lACCLiquidX"),			&m_lACCLiquidX);
		RegVariable(_T("BT_lACCLiquidY"),			&m_lACCLiquidY);
		RegVariable(_T("BT_lACCBrushX"),			&m_lACCBrushX);
		RegVariable(_T("BT_lACCBrushY"),			&m_lACCBrushY);
		RegVariable(_T("BT_lACCRangeX"),			&m_lACCRangeX);
		RegVariable(_T("BT_lACCRangeY"),			&m_lACCRangeY);
		RegVariable(_T("BT_lACCCycleCount"),		&m_lACCCycleCount);

		RegVariable(_T("BT_lACCMatrixRow"),			&m_lACCMatrixRow);
		RegVariable(_T("BT_lACCMatrixCol"),			&m_lACCMatrixCol);
		RegVariable(_T("BT_lACCAreaSizeX"),			&m_lACCAreaSizeX);
		RegVariable(_T("BT_lACCAreaSizeY"),			&m_lACCAreaSizeY);
		RegVariable(_T("BT_lACCAreaLimit"),			&m_lACCAreaLimit);
		RegVariable(_T("BT_lACCAreaCount"),			&m_lACCAreaCount);
		RegVariable(_T("BT_lACCAreaIndex"),			&m_lACCAreaIndex);
		RegVariable(_T("BT_lACCReplaceLimit"),		&m_lACCReplaceLimit);
		RegVariable(_T("BT_lACCReplaceCount"),		&m_lACCReplaceCount);

		RegVariable(_T("BT_lCleanDirtMatrixRow"),		&m_lCleanDirtMatrixRow);
		RegVariable(_T("BT_lCleanDirtMatrixCol"),		&m_lCleanDirtMatrixCol);
		RegVariable(_T("BT_lCleanDirtUpLeftPosX"),		&m_lCleanDirtUpleftPosX);
		RegVariable(_T("BT_lCleanDirtUpLeftPosY"),		&m_lCleanDirtUpleftPosY);
		RegVariable(_T("BT_lCleanDirtLowerRightPosX"),	&m_lCleanDirtLowerRightPosX);
		RegVariable(_T("BT_lCleanDirtLowerRightPosY"),	&m_lCleanDirtLowerRightPosY);


		RegVariable(_T("BT_bACCToggleBHVacuum"),	&m_bACCToggleBHVacuum);
		RegVariable(_T("BT_lMS90MCCGarbageBinX"),	&m_lMS90MCCGarbageBinX);	//v4.59A7
		RegVariable(_T("BT_lMS90MCCGarbageBinY"),	&m_lMS90MCCGarbageBinY);	//v4.59A7
		RegVariable(_T("BT_bCheckFrameOrientation"),&m_bCheckFrameOrientation);	//shiraishi02
		//v4.50A5
//		RegVariable(_T("BT_bEnablePusher2Pos"),				&m_bEnablePusher2Pos);
		RegVariable(_T("BT_lChgColletClampPosX"),			&m_lChgColletClampPosX);
		RegVariable(_T("BT_lChgColletClampPosY"),			&m_lChgColletClampPosY);
		RegVariable(_T("BT_lChgColletHolderInstallPosX"),	&m_lChgColletHolderInstallPosX);
		RegVariable(_T("BT_lChgColletHolderInstallPosY"),	&m_lChgColletHolderInstallPosY);
		RegVariable(_T("BT_lChgColletHolderUploadPosX"),	&m_lChgColletHolderUploadPosX);
		RegVariable(_T("BT_lChgColletHolderUploadPosY"),	&m_lChgColletHolderUploadPosY);
		RegVariable(_T("BT_lChgColletPusher3PosX"),			&m_lChgColletPusher3PosX);
		RegVariable(_T("BT_lChgColletPusher3PosY"),			&m_lChgColletPusher3PosY);
		RegVariable(_T("BT_lChgColletUpLookPosX"),			&m_lChgColletUpLookPosX);
		RegVariable(_T("BT_lChgColletUpLookPosY"),			&m_lChgColletUpLookPosY);
		//new
		RegVariable(_T("BT_lChgColletUPLUploadPosX"),	&m_lChgColletUPLUploadPosX);
		RegVariable(_T("BT_lChgColletUPLUploadPosY"),	&m_lChgColletUPLUploadPosY);
		//

		//Offline PostBond Test (Lumileds)		//v4.11T1
		RegVariable(_T("BT_lOfflinePBStartBondIndex"),	&m_lStartBondIndex);
		RegVariable(_T("BT_lOfflinePBDieCount"),		&m_lNoOfPostBondDices);

		RegVariable(_T("BT_bEnableBTMotionLog"),	&m_bEnableBTMotionLog);
		RegVariable(_T("BT_bIsAllBinsEmptyStatus"),	&m_bIsAllBinsEmptyStatus);		//v4.19	//CMLT
		//Nichia//v4.43T7
		RegVariable(_T("BT_bEnableNichiaOutputFile"),	&m_bEnableNichiaOutputFile);
		RegVariable(_T("BT_bEnableNichiaOTriggerFile"),	&m_bEnableNichiaOTriggerFile);
		RegVariable(_T("BT_bEnableNichiaRPTFile"),		&m_bEnableNichiaRPTFile);

		RegVariable(_T("BT_lBTNoDiePosX"),				&m_lBTNoDiePosX);
		RegVariable(_T("BT_lBTNoDiePosY"),				&m_lBTNoDiePosY);

		// Bin Table offset
//		RegVariable(_T("BT_bBTIsMoveCollet"),			&m_bBTIsMoveCollet);

		// File Name Barcode
		RegVariable(_T("BT_szXMLFile"),					&m_szXMLFile);

		RegVariable(_T("BT_bIsRotate180Deg"),				&m_bIsRotate180DegHMI);			//v4.49A9
		RegVariable(_T("BT_lFrameXOffsetForTesting"),		&m_lFrameXOffsetForTesting);	//v4.59A21
		RegVariable(_T("BT_lFrameYOffsetForTesting"),		&m_lFrameYOffsetForTesting);	//v4.59A21
		RegVariable(_T("BT_dFrameAlignAngleForTesting"),	&m_dFrameAlignAngleForTesting);	//v4.57A9

		CString szRegMsg;
		for(int i=1; i<=200; i++)
		{
			szRegMsg.Format("BL_bBinStatus%03d", i);
			RegVariable(_T(szRegMsg), &m_bBinStatus[i]);
		}

		//Sub Bin Row Col for each zone
		RegVariable(_T("BT_ulSubBinSRowA"),				&m_ulSubBinSRowA);
		RegVariable(_T("BT_ulSubBinSRowB"),				&m_ulSubBinSRowB);	
		RegVariable(_T("BT_ulSubBinSRowC"),				&m_ulSubBinSRowC);	
		RegVariable(_T("BT_ulSubBinSRowD"),				&m_ulSubBinSRowD);

		RegVariable(_T("BT_ulSubBinERowA"),				&m_ulSubBinERowA);
		RegVariable(_T("BT_ulSubBinERowB"),				&m_ulSubBinERowB);	
		RegVariable(_T("BT_ulSubBinERowC"),				&m_ulSubBinERowC);	
		RegVariable(_T("BT_ulSubBinERowD"),				&m_ulSubBinERowD);

		RegVariable(_T("BT_ulSubBinSColA"),				&m_ulSubBinSColA);
		RegVariable(_T("BT_ulSubBinSColB"),				&m_ulSubBinSColB);	
		RegVariable(_T("BT_ulSubBinSColC"),				&m_ulSubBinSColC);	
		RegVariable(_T("BT_ulSubBinSColD"),				&m_ulSubBinSColD);

		RegVariable(_T("BT_ulSubBinEColA"),				&m_ulSubBinEColA);
		RegVariable(_T("BT_ulSubBinEColB"),				&m_ulSubBinEColB);	
		RegVariable(_T("BT_ulSubBinEColC"),				&m_ulSubBinEColC);	
		RegVariable(_T("BT_ulSubBinEColD"),				&m_ulSubBinEColD);
		//andrewng //2020-0630
		RegVariable(_T("BT_lTempFileBinCol"),			&m_lTempFileBinCol);
		RegVariable(_T("BT_lTempFileBinRow"),			&m_lTempFileBinRow);
		RegVariable(_T("BT_lTempFileWTEncX"),			&m_lTempFileWTEncX);
		RegVariable(_T("BT_lTempFileWTEncY"),			&m_lTempFileWTEncY);
		RegVariable(_T("BT_lTempFileBTEncX"),			&m_lTempFileBTEncX);
		RegVariable(_T("BT_lTempFileBTEncY"),			&m_lTempFileBTEncY);
		//andrewng //2020-0902
		RegVariable(_T("BT_lUnloadPhyPosX"),			&m_lUnloadPhyPosX);		
		RegVariable(_T("BT_lUnloadPhyPosY"),			&m_lUnloadPhyPosY);
		RegVariable(_T("BT_lUnloadPhyPosT"),			&m_lUnloadPhyPosT);


		DisplayMessage("BinTableStn variables registered ...");


		/******************************/
		/*    Reg Service Commands    */
		/******************************/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateOutput"),					&CBinTable::UpdateOutput);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),					&CBinTable::UpdateAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogItems"),						&CBinTable::LogItems);

		//Auto-select grade before bond start
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckGradeAssignment"),			&CBinTable::CheckGradeAssignment);

		//Update the grade of bin block
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateOneBinGrade"),			&CBinTable::UpdateOneBinGrade);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateBlkByPassBinMap"),		&CBinTable::UpdateBlkByPassBinMap);

		//For Warm Start after package file restore
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearAllBinCounters"),			&CBinTable::ClearAllBinCounters);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIsAllBinCleared"),			&CBinTable::CheckIsAllBinCleared);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIsBinCleared"),			&CBinTable::CheckIsBinCleared);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIsAllBinCleared_PKG"),		&CBinTable::CheckIsAllBinCleared_PKG);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIsAllBinClearedNoMsg"),	&CBinTable::CheckIsAllBinClearedNoMsg);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIsAllBinClearedWithDieCountConstraint"), &CBinTable::CheckIsAllBinClearedWithDieCountConstraint);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIsAllBinNotFull"),			&CBinTable::CheckIsAllBinNotFull);		//v2.78T1

		//For Pick and Place use
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IfBinCntIsClrB4PickAndPlace"),	&CBinTable::IfBinCntIsClrB4PickAndPlace);
		//Wafer Statistics
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WaferStatisticsPreTask"),		&CBinTable::WaferStatisticsPreTask);
		//Bin Summary Output
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BinSummaryOutput"),				&CBinTable::BinSummaryOutput);	
		//Machine No
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMachineNo"),				&CBinTable::SaveMachineNo);
		//Clear Block Settings
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IfIsClearAllSettings"),			&CBinTable::IfIsClearAllSettings);
		//Clear Physical Block Settings
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubmitClrPhyBlkSettings"),		&CBinTable::SubmitClrPhyBlkSettings);
		//Clear Bin Block Settings
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubmitClrBinBlkSettings"),		&CBinTable::SubmitClrBinBlkSettings);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AlignEmptyFrameWithPRCmd"),		&CBinTable::AlignEmptyFrameWithPRCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExArmRealignEmptyFrameWithPRCmd"), &CBinTable::ExArmRealignEmptyFrameWithPRCmd);
		
		//Bintable Setup
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBinTableLimit"),			&CBinTable::ConfirmBinTableLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelBinTableLimit"),			&CBinTable::CancelBinTableLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachLoadPosition"),			&CBinTable::TeachLoadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmLoadPosition"),			&CBinTable::ConfirmLoadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelLoadPosition"),			&CBinTable::CancelLoadPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachColletOffset"),			&CBinTable::TeachColletOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmColletOffset"),			&CBinTable::ConfirmColletOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToUnloadLoad"),				&CBinTable::MoveToUnloadLoad);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachBinTable2OffsetXY"),		&CBinTable::TeachBinTable2OffsetXY);	//v4.17T3	

		//Physical Block Setup
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PhyBlkSetupPreRoutine"),		&CBinTable::PhyBlkSetupPreRoutine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnChangePhyBlkSetupModeSelection"), &CBinTable::OnChangePhyBlkSetupModeSelection);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickNoOfPhyBlkInput"),		&CBinTable::OnClickNoOfPhyBlkInput);	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickPhyBlkToSetupInput"),	&CBinTable::OnClickPhyBlkToSetupInput);		

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfNoOfPhyBlksIsValid"),	&CBinTable::CheckIfNoOfPhyBlksIsValid);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfPhyBlkToSetupIsValid"),	&CBinTable::CheckIfPhyBlkToSetupIsValid);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfContainsBinBlks"),		&CBinTable::CheckIfContainsBinBlks);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetPhyBlkSetupUL"),				&CBinTable::SetPhyBlkSetupUL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmPhyBlkSetupUL"),			&CBinTable::ConfirmPhyBlkSetupUL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetPhyBlkSetupLR"),				&CBinTable::SetPhyBlkSetupLR);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmPhyBlkSetupLR"),			&CBinTable::ConfirmPhyBlkSetupLR);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubmitPhyBlkSetup"),			&CBinTable::SubmitPhyBlkSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelPhyBlkSetup"),			&CBinTable::CancelPhyBlkSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayPhyBlkData"),			&CBinTable::DisplayPhyBlkData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToPhyBlk"),					&CBinTable::MoveToPhyBlk);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfClrBlkSettingsPossible"), &CBinTable::CheckIfClrBlkSettingsPossible);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubmitSubBinPhyBlkSetup"),		&CBinTable::SubmitSubBinPhyBlkSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubBinReset"),					&CBinTable::SubBinReset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubBinCheckAZone"),				&CBinTable::SubBinCheckAZone);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubBinCheckBZone"),				&CBinTable::SubBinCheckBZone);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubBinCheckCZone"),				&CBinTable::SubBinCheckCZone);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubBinCheckDZone"),				&CBinTable::SubBinCheckDZone);
		/********** Bin Block Setup **********/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BinBlkSetupPreRoutine"),		&CBinTable::BinBlkSetupPreRoutine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IfProceedWithCommandsBelow"),	&CBinTable::IfProceedWithCommandsBelow);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfNoOfBinBlksIsValid"),	&CBinTable::CheckIfNoOfBinBlksIsValid);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfBinBlkToSetupIsValid"),	&CBinTable::CheckIfBinBlkToSetupIsValid);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfBondingInProcess"),		&CBinTable::CheckIfBondingInProcess);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinBlkSetupUL"),				&CBinTable::SetBinBlkSetupUL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBinBlkSetupUL"),			&CBinTable::ConfirmBinBlkSetupUL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinBlkSetupLR"),				&CBinTable::SetBinBlkSetupLR);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBinBlkSetupLR"),			&CBinTable::ConfirmBinBlkSetupLR);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfBlockHasDie"),			&CBinTable::CheckIfBlockHasDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWafflePadDestX"),			&CBinTable::SetWafflePadDestX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWafflePadDestY"),			&CBinTable::SetWafflePadDestY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetWafflePadDestX"),			&CBinTable::ResetWafflePadDestX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetWafflePadDestY"),			&CBinTable::ResetWafflePadDestY);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AllBlocksSetup"),				&CBinTable::AllBlocksSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubmitBinBlkSetup"),			&CBinTable::SubmitBinBlkSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelBinBlkSetup"),			&CBinTable::CancelBinBlkSetup);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowBinBlkTemplateData"),		&CBinTable::ShowBinBlkTemplateData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadBinBlkTemplate"),			&CBinTable::LoadBinBlkTemplate);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveBinBlkTemplate"),			&CBinTable::SaveBinBlkTemplate);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RemoveBinBlkTemplate"),			&CBinTable::RemoveBinBlkTemplate);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayBinBlkData"),			&CBinTable::DisplayBinBlkData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToBinBlk"),					&CBinTable::MoveToBinBlk);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToBinBlk2"),				&CBinTable::MoveToBinBlk2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToBinBlk1stDie"),			&CBinTable::MoveToBinBlk1stDie);	//v2.78T1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnChangeBinBlkSetupModeSelection"), &CBinTable::OnChangeBinBlkSetupModeSelection);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickNoOfBinBlkInput"),		&CBinTable::OnClickNoOfBinBlkInput);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickBinBlkToSetupInput"),	&CBinTable::OnClickBinBlkToSetupInput);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachCircularAreaCenterPos"),	&CBinTable::TeachCircularAreaCenterPos);	//Pllm
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateBondAreaOffset"),			&CBinTable::UpdateBondAreaOffset);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveReferenceCrossEncoderValue"),	&CBinTable::SaveReferenceCrossEncoderValue);

		//All Bin Blocks Setup
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IfAllBinBlksSetupIsAllowed"),	&CBinTable::IfAllBinBlksSetupIsAllowed);

		//Single Bin Block Setup
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IfBondingInProcess"),			&CBinTable::IfBondingInProcess);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IfDisabledFromSameGradeMerge"), &CBinTable::IfDisabledFromSameGradeMerge);


		/********** Clear Bin Counter **********/
		//By Physical Block
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClrPhyBinCtrPreRoutine"),		&CBinTable::ClrPhyBinCtrPreRoutine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClrPhyBinCtr"),					&CBinTable::ClrPhyBinCtr);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnChangeClrBinCntModeSelection"), &CBinTable::OnChangeClrBinCntModeSelection);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOutputFilePath"),			&CBinTable::SaveOutputFilePath);	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOutputFileFormat"),			&CBinTable::SaveOutputFileFormat);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOutputFileBlueTapeNoFormat"), &CBinTable::SaveOutputFileBlueTapeNoFormat); // v4.51D10 Dicon
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubmitClrPhyBinCtr"),			&CBinTable::SubmitClrPhyBinCtr);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubmitGenerateSummaryFile"),	&CBinTable::SubmitGenerateSummaryFile);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InputOutputFilePath"),		&CBinTable::InputOutputFilePath);				
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InputOutputFileFormat"),	&CBinTable::InputOutputFileFormat);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetOutputFilePath"),		&CBinTable::GetOutputFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetOutputFileSummaryPath"), &CBinTable::GetOutputFileSummaryPath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InputOutputFilePath2"),		&CBinTable::InputOutputFilePath2);				
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetOutputFilePath2"),		&CBinTable::GetOutputFilePath2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetOutputFilePath3"),		&CBinTable::GetOutputFilePath3);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetOutputFilePath4"),		&CBinTable::GetOutputFilePath4);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOutputFilePath2"),		&CBinTable::SaveOutputFilePath2);	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckPhyBlkID"),			&CBinTable::CheckPhyBlkID);	

		//By Grade
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClrBinCtrByGrade"),			&CBinTable::ClrBinCtrByGrade);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubmitClrBinCtrByGrade"),	&CBinTable::SubmitClrBinCtrByGrade);
		
		//By both Physical Block and Grade
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClrBinCtrPreRoutine"),		&CBinTable::ClrBinCtrPreRoutine);

		//Clear all bin counter (BurnIn use)
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BurnInClearCounter"),		&CBinTable::BurnInClearCounter);

		//Clear Bin Count (for DL machine)
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoClrBinCnt"),			&CBinTable::AutoClrBinCnt);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoClrBinCnt_SECSGEM"),	&CBinTable::AutoClrBinCnt_SECSGEM);	//v4.59A19
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBinNumber"),				&CBinTable::GetBinNumber);	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBinGrade"),				&CBinTable::GetBinGrade);	

		/************* Step Move ***************/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StepMovePreRoutine"),		&CBinTable::StepMovePreRoutine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayBinBlkInfoForStepMove"), &CBinTable::DisplayBinBlkInfoForStepMove);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToNewIndex"),			&CBinTable::MoveToNewIndex);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateBlkIndex"),			&CBinTable::UpdateBlkIndex);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetEnableMS90Rotation180"), &CBinTable::SetEnableMS90Rotation180);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnterBinStepMovePage"),		&CBinTable::EnterBinStepMovePage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExitBinStepMovePage"),		&CBinTable::ExitBinStepMovePage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MultiRealignFirstDie"),		&CBinTable::MultiRealignFirstDie);  //4.51D20
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MS90RotateBT180"),			&CBinTable::MS90RotateBT180);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectBinRowColFromTempFile"),	&CBinTable::SelectBinRowColFromTempFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveFromTempFile"),		&CBinTable::BTMoveFromTempFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveTableToDummyPos"),		&CBinTable::MoveTableToDummyPos);

		/************* Grade Information *************/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GradeInfoPreRoutine"),		&CBinTable::GradeInfoPreRoutine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayGradeInfo"),			&CBinTable::DisplayGradeInfo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SubmitGradeInfo"),			&CBinTable::SubmitGradeInfo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetOptimizeBinCountPerWft"), &CBinTable::ResetOptimizeBinCountPerWft);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableOptimizeBinCountPerWft"), &CBinTable::EnableOptimizeBinCountPerWft);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CopyAllGradeInfo"),			&CBinTable::CopyAllGradeInfo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CopyPartialGradeInfo"),		&CBinTable::CopyPartialGradeInfo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadGradeRankFile"),		&CBinTable::LoadGradeRankFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetGradeRankID"),			&CBinTable::ResetGradeRankID);

		/********************* Bin Count Checking ************************/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfAllBinCntCleared"),			&CBinTable::CheckIfAllBinCntCleared);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateIfAllBinCountsAreCleared"),	&CBinTable::UpdateIfAllBinCountsAreCleared);		//v4.08
		
		/********** Lot Number & Lot Directory Change Checking **********/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PreLotNoChangeChecking"),			&CBinTable::PreLotNoChangeChecking);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PreLotDirectoryChangeChecking"),	&CBinTable::PreLotDirectoryChangeChecking);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PreAssociateFileChangeChecking"),	&CBinTable::PreAssociateFileChangeChecking);

		/***************** Set / Reset Serial No ****************/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinSerialNoFormat"),		&CBinTable::SetBinSerialNoFormat); 
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinSerialNoMaxLimit"),	&CBinTable::SetBinSerialNoMaxLimit); 
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBinSerialNo"),			&CBinTable::ResetBinSerialNo);

		/***************** Set / Reset Bin clear Format ****************/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinClearFormat"),		&CBinTable::SetBinClearFormat);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinClearMaxLimit"),		&CBinTable::SetBinClearMaxLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinClearInitCount"),		&CBinTable::SetBinClearInitCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBinClearedCount"),		&CBinTable::ResetBinClearedCount);

		/***************** Set / Reset Store Bin accumulated bonded count ****************/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableBinAccBondedCount"),	&CBinTable::EnableBinAccBondedCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBinAccBondedCount"),	&CBinTable::ResetBinAccBondedCount);

		/***************** Select Optimize Bin Grade ***************/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableOptimizeBinGrade"),	&CBinTable::EnableOptimizeBinGrade);

		/***************** Select LS Bond Pattern ***************/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableLSBondPattern"),		&CBinTable::EnableLSBondPattern);


		/********** Wafer End file **********/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InputWaferEndFilePath"),		&CBinTable::InputWaferEndFilePath);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InputWaferEndFileFormat"),		&CBinTable::InputWaferEndFileFormat);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveWaferEndFilePath"),			&CBinTable::SaveWaferEndFilePath);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveWaferEndFileFormat"),		&CBinTable::SaveWaferEndFileFormat);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetWaferEndFilePath"),			&CBinTable::GetWaferEndFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetWaferEndSummaryFilePath"),	&CBinTable::GetWaferEndSummaryFilePath);

		/********** Bin Summary Output File **********/	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("InputBinSummaryOutputPath"),	&CBinTable::InputBinSummaryOutputPath);	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveBinSummaryOutputPath"),		&CBinTable::SaveBinSummaryOutputPath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBinSummaryOutputPath"),		&CBinTable::GetBinSummaryOutputPath);

		/********** Supplementary For Bin Loader module **********/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfPhyBlkIsSetup"),			&CBinTable::CheckIfPhyBlkIsSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBinBlkBondedCount"),			&CBinTable::GetBinBlkBondedCount);

		/********** Output / Show LotSummary File **********/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OutputLotSummary"),				&CBinTable::OutputLotSummary);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OutputLotSummary_Avago"),		&CBinTable::OutputLotSummary_Avago);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowLotSummary"),				&CBinTable::ShowLotSummary);

		/********** Restore NVRam Runtime data **********/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RestoreNVRunTimeData"),			&CBinTable::RestoreNVRunTimeData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RestoreNVRunTimeDataInHmi"),	&CBinTable::RestoreNVRunTimeDataInHmi);		//v4.22T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearNVRunTimeData"),			&CBinTable::ClearNVRunTimeData);

		/********** Setup BinBlock pre & post function **********/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetupBinBlock"),				&CBinTable::SetupBinBlock);

		/********** Display bonding binblk function **********/
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayBondingBinBlk"),			&CBinTable::DisplayBondingBinBlk);


		// Move command
		m_comServer.IPC_REG_SERVICE_COMMAND("BT_GetCurrPosnInZero",				&CBinTable::BT_GetCurrPosnInZero);
		m_comServer.IPC_REG_SERVICE_COMMAND("BT_GetPhyBinBlockCorner",			&CBinTable::BT_GetPhyBinBlockCorner);
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_X_MoveToCmd,						&CBinTable::X_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_X_MoveCmd,						&CBinTable::X_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_Y_MoveToCmd,						&CBinTable::Y_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_Y_MoveCmd,						&CBinTable::Y_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_XY_MoveToCmd,					&CBinTable::XY_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY2_MoveToCmd"),				&CBinTable::XY2_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_XY_MoveCmd,						&CBinTable::XY_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_X_HomeCmd,						&CBinTable::X_HomeCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_Y_HomeCmd,						&CBinTable::Y_HomeCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_XY_HomeCmd,						&CBinTable::XY_HomeCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetXLimitCmd"),				&CBinTable::ResetXLimitCmd);	//v4.31T10	//MS109
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchXLimitCmd"),				&CBinTable::SearchXLimitCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchYLimitCmd"),				&CBinTable::SearchYLimitCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchX2LimitCmd"),				&CBinTable::SearchX2LimitCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchY2LimitCmd"),				&CBinTable::SearchY2LimitCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("T_MoveToZeroCmd_ForBL"),		&CBinTable::T_MoveToZeroCmd_ForBL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_MoveToCmd_ForBL"),			&CBinTable::XY_MoveToCmd_ForBL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_MoveToCmd_ForBL_XThenY"),	&CBinTable::XY_MoveToCmd_ForBL_XThenY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_MoveToCmd_ForBL_YThenX"),	&CBinTable::XY_MoveToCmd_ForBL_YThenX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY2_MoveToCmd_ForBL"),			&CBinTable::XY2_MoveToCmd_ForBL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY2_MoveToCmd_ForBL_XThenY"),	&CBinTable::XY2_MoveToCmd_ForBL_XThenY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY2_MoveToCmd_ForBL_YThenX"),	&CBinTable::XY2_MoveToCmd_ForBL_YThenX);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_Sync_ForBL"),				&CBinTable::XY_Sync_ForBL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY2_Sync_ForBL"),				&CBinTable::XY2_Sync_ForBL);
		// Get encoder values
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetEncoderCmd"),				&CBinTable::GetEncoderCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetEncoderCmd2"),				&CBinTable::GetEncoderCmd2);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBinLoadTestState"),			&CBinTable::GetBinLoadTestState);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinLoadTestState"),			&CBinTable::SetBinLoadTestState);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetFrameAlign"),				&CBinTable::ResetFrameAlign);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("RealignBinBlockCmd"),			&CBinTable::RealignBinBlockCmd);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("RealignPhysicalBlockCmd"),		&CBinTable::RealignPhysicalBlockCmd);
 //       m_comServer.IPC_REG_SERVICE_COMMAND(_T("RealignPhysicalBlock2Cmd"),		&CBinTable::RealignPhysicalBlock2Cmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RealignEmptyFrameCmd"),			&CBinTable::RealignEmptyFrameCmd);			//v2.93T2
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetFrameIsAligned"),			&CBinTable::ResetFrameIsAligned);			//v4.42T10
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeTableForRealign_BL"),		&CBinTable::HomeTableForRealign_BL);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsBT2UnderCamera"),				&CBinTable::IsBT2UnderCamera);				//v4.35T4	//MS109

		// On or off joystick
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_SetJoystickCmd,					&CBinTable::SetJoystickCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND("SwitchTableForPRJoyStick",			&CBinTable::SwitchTableForPRJoyStick);		//v4.17T5
		m_comServer.IPC_REG_SERVICE_COMMAND("SetJoystickCmdWithoutSrCheck",		&CBinTable::SetJoystickCmdWithoutSrCheck);
		
		// On or off joystick with checking the current state
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UseJoystickCmd"),				&CBinTable::UseJoystickCmd);
		// Set joystick speed
		m_comServer.IPC_REG_SERVICE_COMMAND(BT_SetJoystickSpeedCmd,				&CBinTable::SetJoystickSpeedCmd);
		// Save bintable data
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StoreBinTableData"),			&CBinTable::StoreBinTableData);
		// -- Manual gen all temp file 
		//m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualGenAllTempFiles"), ManualGenAllTempFiles);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetAlignBinMode"),				&CBinTable::SetAlignBinMode);
		// -- Manual gen WaferEnd file
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualWaferEnd"),				&CBinTable::ManualWaferEnd);

		// -- Get print label path & re-print label
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetLabelFilePath"),				&CBinTable::GetLabelFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectLabelToPrint"),			&CBinTable::SelectLabelToPrint);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BinBarcodeSelectLabelToPrint"),	&CBinTable::BinBarcodeSelectLabelToPrint);

		//--- Diagnostic Functions ---
		/* PowerOn Functions 
				Parameter: BOOL bOn */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_PowerOn_X"),	&CBinTable::Diag_PowerOn_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_PowerOn_Y"),	&CBinTable::Diag_PowerOn_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_PowerOn_T"),	&CBinTable::Diag_PowerOn_T);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_PowerOn_X2"),	&CBinTable::Diag_PowerOn_X2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_PowerOn_Y2"),	&CBinTable::Diag_PowerOn_Y2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_PowerOn_All"),	&CBinTable::Diag_PowerOn_All);
		/* Commutate Functions 
				Parameter: None */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Comm_X"),		&CBinTable::Diag_Comm_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Comm_Y"),		&CBinTable::Diag_Comm_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Comm_X2"),		&CBinTable::Diag_Comm_X2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Comm_Y2"),		&CBinTable::Diag_Comm_Y2);
		/* Home Functions 
				Parameter: None */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Home_X"),		&CBinTable::Diag_Home_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Home_Y"),		&CBinTable::Diag_Home_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Home_T"),		&CBinTable::Diag_Home_T);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Home_X2"),		&CBinTable::Diag_Home_X2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Home_Y2"),		&CBinTable::Diag_Home_Y2);
		/* Move Functions (Relative)
				Parameter: LONG lPos (Motor Counts) */
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Move_X"),		&CBinTable::Diag_Move_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Move_Y"),		&CBinTable::Diag_Move_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Move_T"),		&CBinTable::Diag_Move_T);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Move_X2"),		&CBinTable::Diag_Move_X2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_Diag_Move_Y2"),		&CBinTable::Diag_Move_Y2);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBinDiePitch"),		&CBinTable::GetBinDiePitch);		//v2.58
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetNewMapBlockInUse"),	&CBinTable::GetNewMapBlockInUse);	//v2.68
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetNextGradeBlk"),		&CBinTable::GetNextGradeBlk);		//v2.82T1

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExecutPrintLabelUtility"),	&CBinTable::ExecutPrintLabelUtility);		//v2.82T1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveBinTableDataCmd"),	&CBinTable::SaveBinTableDataCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearAllBin"),			&CBinTable::ClearAllBin);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetRankIDFilePath"),	&CBinTable::GetRankIDFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetRankIDFile"),		&CBinTable::ResetRankIDFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLoadRankIDFile"),	&CBinTable::AutoLoadRankIDFile);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UploadBinBlkRunTimeData"),		&CBinTable::UploadBinBlkRunTimeData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DownloadBinBlkRunTimeData"),	&CBinTable::DownloadBinBlkRunTimeData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenBinBlkRunTimeSummary"),		&CBinTable::GenBinBlkRunTimeSummary);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RemoveAllTempFile"),			&CBinTable::RemoveAllTempFile);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveGenSummaryFileTime"),		&CBinTable::SaveGenSummaryFileTime);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBinLotSummaryFilePath2"),	&CBinTable::GetBinLotSummaryFilePath2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetBinLotSummaryFilePath"),		&CBinTable::GetBinLotSummaryFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadBinLotSummaryFile"),		&CBinTable::LoadBinLotSummaryFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadBinLotSummaryFile2"),		&CBinTable::LoadBinLotSummaryFile2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLoadBinLotSummaryFile"),	&CBinTable::AutoLoadBinLotSummaryFile);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateOptimizeBinGradeScreen"),			&CBinTable::UpdateOptimizeBinGradeScreen);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickChangeOptimizeBinGrade"),		&CBinTable::OnClickChangeOptimizeBinGrade);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickUpdateOptimzeMinCountByRange"),	&CBinTable::OnClickUpdateOptimzeMinCountByRange);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickUpdateAllOptimizeMinCount"),		&CBinTable::OnClickUpdateAllOptimizeMinCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickUpdateOptimizeMaxCountByRange"), &CBinTable::OnClickUpdateOptimizeMaxCountByRange);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OnClickUpdateAllOptimizeMaxCount"),		&CBinTable::OnClickUpdateAllOptimizeMaxCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetOptimizeBinCountStatus"),			&CBinTable::ResetOptimizeBinCountStatus);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WaferEndFileGeneratingCheckingCmd"),	&CBinTable::WaferEndFileGeneratingCheckingCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBHZ1BondPosOffset"),				&CBinTable::ResetBHZ1BondPosOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLearnBHZ1BondPosOffset"),			&CBinTable::AutoLearnBHZ1BondPosOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachBHZ1BondPosOffsetByCollet"),		&CBinTable::TeachBHZ1BondPosOffsetByCollet);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBHZ1BondPosOffsetByCollet"),		&CBinTable::ConfirmBHZ1BondPosOffsetByCollet);

//		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLearnBHZ2BondPosOffset"),			&CBinTable::ManualLearnBHZ2BondPosOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBHZ2BondPosOffset"),				&CBinTable::ResetBHZ2BondPosOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLearnBHZ2BondPosOffset"),			&CBinTable::AutoLearnBHZ2BondPosOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachBHZ2BondPosOffsetByCollet"),		&CBinTable::TeachBHZ2BondPosOffsetByCollet);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBHZ2BondPosOffsetByCollet"),		&CBinTable::ConfirmBHZ2BondPosOffsetByCollet);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBHZ1OrBHZ2BondPosOffsetByCollet"),		&CBinTable::ConfirmBHZ1OrBHZ2BondPosOffsetByCollet);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveBinTableToSafePosn"),			&CBinTable::MoveBinTableToSafePosn);	//v3.70T1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveBinTableToMS90GBinPosn"),		&CBinTable::MoveBinTableToMS90GBinPosn);	//v4.59A7
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveBinTableToBondPosn"),			&CBinTable::MoveBinTableToBondPosn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToBinArea4Corners"),			&CBinTable::MoveToBinArea4Corners);		//v3.94T5
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToBinArea4Corners2"),			&CBinTable::MoveToBinArea4Corners2);	//v4.21
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveBTForAutoCColletAutoLearnZ"),	&CBinTable::MoveBTForAutoCColletAutoLearnZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGCMoveBinTableToBondPosn"),		&CBinTable::AGCMoveBinTableToBondPosn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsNeedCheckBatchIDFile"),			&CBinTable::IsNeedCheckBatchIDFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckBatchIDInAutoMode"),			&CBinTable::CheckBatchIDInAutoMode);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearBatchIDInformation"),			&CBinTable::ClearBatchIDInformation);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartBTXMotionTest"),		&CBinTable::StartBTXMotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartBT2XMotionTest"),		&CBinTable::StartBT2XMotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartBTYMotionTest"),		&CBinTable::StartBTYMotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartBT2YMotionTest"),		&CBinTable::StartBT2YMotionTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartOfflinePostbondTest"),	&CBinTable::StartOfflinePostbondTest);	//v4.11T1

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTBackupMapCommand"),		&CBinTable::BTBackupMapCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTGenAllTempFiles"),		&CBinTable::BTGenAllTempFiles);

		// Auto Clean Collet command
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToACCLiquid"),	&CBinTable::BTMoveToACCLiquid);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToACCBrush"),		&CBinTable::BTMoveToACCBrush);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTCycleACCOperation"),	&CBinTable::BTCycleACCOperation);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTCycleACCOperation2"),	&CBinTable::BTCycleACCOperation2);

		//Clean Dirt alochle
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToRemoveDirt"),			&CBinTable::BTMoveToRemoveDirt);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTCycleRemoveDirtOperation"),	&CBinTable::BTCycleRemoveDirtOperation);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTChangePosition"),		&CBinTable::BTChangePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTCancelSetup"),		&CBinTable::BTCancelSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTConfirmSetup"),		&CBinTable::BTConfirmSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToACCClean"),		&CBinTable::BTMoveToACCClean);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTAccConfirmSetup"),	&CBinTable::BTAccConfirmSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTAccReset"),			&CBinTable::BTAccReset);
		// AutoChangeCollet Cmds	//v4.50A5
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTChangeAGCPosition"),			&CBinTable::BTChangeAGCPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTCancelAGCSetup"),				&CBinTable::BTCancelAGCSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTConfirmAGCSetup"),			&CBinTable::BTConfirmAGCSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToAGCClampPos"),			&CBinTable::BTMoveToAGCClampPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToAGCUPLUploadPos"),		&CBinTable::BTMoveToAGCUPLUploadPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToAGCHolderUploadPos"),	&CBinTable::BTMoveToAGCHolderUploadPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToAGCHolderInstallPos"),	&CBinTable::BTMoveToAGCHolderInstallPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToAGCPusher3Pos"),		&CBinTable::BTMoveToAGCPusher3Pos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTMoveToAGCUpLookPos"),			&CBinTable::BTMoveToAGCUpLookPos);
//		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_IncrementIndex"),	&CBinTable::AGC_IncrementIndex);

		//v3.74T45		//v4.03		//PLLM/PLSG bin map fcn
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CreateBinMapFcn2"),		&CBinTable::CreateBinMapFcn2);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CreateBinMapFcn"),		&CBinTable::CreateBinMapFcn);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadBinMapFcn"),		&CBinTable::LoadBinMapFcn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadNGBinMap"),			&CBinTable::LoadNGBinMap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveBinMapFcn"),		&CBinTable::SaveBinMapFcn);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveBinMapFcn_pkg"),	&CBinTable::SaveBinMapFcn_pkg);		//v4.53A29
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadBinMapFcn_pkg"),	&CBinTable::LoadBinMapFcn_pkg);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableBinMapFcn"),		&CBinTable::EnableBinMapFcn);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetSpGradeToByPassBinMap"),	&CBinTable::SetSpGradeToByPassBinMap);	//v4.48A4
		//v4.47T13	//Osram Germany BinMix fcn
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableOsramBinMixMap"),	&CBinTable::EnableOsramBinMixMap);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CreateOsramBinMixMap"),	&CBinTable::CreateOsramBinMixMap);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOsramBinMixMap"),	&CBinTable::SaveOsramBinMixMap);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReLoadBinMapFcn"),		&CBinTable::ReLoadBinMapFcn);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTChangeNoDiePosition"),		&CBinTable::BTChangeNoDiePosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BTConfirmNoDiePositionSetup"),	&CBinTable::BTConfirmNoDiePositionSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("NGBlockBinBlkSetup"),		&CBinTable::NGBlockBinBlkSetup);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GoAndSearchAllBinBlockDie"),	&CBinTable::GoAndSearchAllBinBlockDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GoAndSearchAllBinBlockDie2"),	&CBinTable::GoAndSearchAllBinBlockDie2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StopOfflinePostbondTest"),		&CBinTable::StopOfflinePostbondTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RotateBinAngularDie"),			&CBinTable::RotateBinAngularDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FindCenterOfRotation"),			&CBinTable::FindCenterOfRotation);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Test"),					&CBinTable::Test);		//v3.67T5
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GenerateConfigData"),	&CBinTable::GenerateConfigData);		//v3.86
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GeneratePkgDataFile"),	&CBinTable::GeneratePkgDataFile);		//v4.21T7	//Walsin China
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PkgKeyParametersTask"),	&CBinTable::PkgKeyParametersTask);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBinStatus"),		&CBinTable::ResetBinStatus);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateBinStatus"),		&CBinTable::UpdateBinStatus);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("QueryBinStatus"),		&CBinTable::QueryBinStatus);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckBinEmptyStatus"),			&CBinTable::CheckBinEmptyStatus);			//v4.14T1	//Ubiliux
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("FATC_CheckBinEmptyStatus"),		&CBinTable::FATC_CheckBinEmptyStatus);		//v4.14T1	//Ubiliux
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualCleanCollet_PLLM_MS109"),	&CBinTable::ManualCleanCollet_PLLM_MS109);	//v4.36T1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadInputCountSetupFile"),		&CBinTable::LoadInputCountSetupFile);		//v4.36T1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetInputCountSetupFilePath"),	&CBinTable::GetInputCountSetupFilePath);	//v4.36T1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ValidateNichiaArrCodeInMap"),	&CBinTable::ValidateNichiaArrCodeInMap);	//v4.42T12
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CalculateBinCapacityForArrCode"),	&CBinTable::CalculateBinCapacityForArrCode);	//v4.42T12
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DrawBinMap"),					&CBinTable::DrawBinMap);	//Nichia//v4.43T7
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OsramCalculateFunction"),			&CBinTable::OsramCalculateFunction);//osram bin mix map
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TEST_AlignGlobalThetaAngleByBinFrame"),	&CBinTable::TEST_AlignGlobalThetaAngleByBinFrame);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToCORCmd"),			&CBinTable::MoveToCORCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachCalibration"),		&CBinTable::TeachCalibration);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StopCalibration"),		&CBinTable::StopCalibration);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateCOR"),			&CBinTable::UpdateCOR);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SemitekGenerateOutputFileWhenReadWaferBarcode"),	&CBinTable::SemitekGenerateOutputFileWhenReadWaferBarcode);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetFrameAlignOffset"),	&CBinTable::SetFrameAlignOffset);	//v4.59A21	//DavidMa
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetFrameAlignAngle"),	&CBinTable::SetFrameAlignAngle);	//v4.57A9	//DavidMa
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetThermalDeltaPitch"),	&CBinTable::SetThermalDeltaPitch);	//v4.59A22	//DavidMa
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RotateFrameAngle"),		&CBinTable::RotateFrameAngle);		//v4.59A1
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_BondLeftDie"),		&CBinTable::BT_BondLeftDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_GetGradeBinBlock"),	&CBinTable::BT_GetGradeBinBlock);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BT_SetCountByGrade"),	&CBinTable::BT_SetCountByGrade);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinFrameOrientXY"),	&CBinTable::SetBinFrameOrientXY);	//shiraishi02
		//andrewng //2020-0902
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToUnload"),			&CBinTable::MoveToUnload);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetUnloadPosition"),	&CBinTable::SetUnloadPosition);

		DisplayMessage("BinTableStn Service Registered...");

		RegisterBTErrorMapVariables();		//andrewng //2020-0806
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
} //end RegisterVariables


/***********************************/
/*     HMI reg service commands    */
/***********************************/

LONG CBinTable::UpdateAction(IPC_CServiceMessage& svMsg)
{
	Sleep(500);
	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

LONG CBinTable::LogItems(IPC_CServiceMessage& svMsg)
{
	LONG lEventNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lEventNo);

	LogItems(lEventNo);
	
	return 1;
}

LONG CBinTable::CheckGradeAssignment(IPC_CServiceMessage& svMsg)
{
	CUIntArray aulSelectedGradeList, aulUnassignedGradeList, aulFilteredGradeList;
	INT i;
	UCHAR ucSelectedGrade;
	unsigned char aaGrades[256];
	CString szUnassignedGrade, szPopMsg;
	BOOL bReply;

	//Check if selected grade has bin block assigned
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="Cree" )
	{
		//Get current selected grade no & total
		CUIntArray aulSelctdList;
		m_WaferMapWrapper.GetSelectedGradeList(aulSelctdList);
		UCHAR	*pNewList;
		pNewList = new UCHAR[aulSelctdList.GetSize()];

		BOOL bInkDieIn = FALSE;
		INT		nNewListSize = 0;
		for (INT i=0; i<aulSelctdList.GetSize(); i++)
		{
			UCHAR ucTempGrade = aulSelctdList.GetAt(i);
			USHORT usOrgGrade = m_WaferMapWrapper.GetOriginalGrade(ucTempGrade)- m_WaferMapWrapper.GetGradeOffset();
			if( usOrgGrade==(65533) )	//	die grade -3
			{
				bInkDieIn = TRUE;
				continue;
			}
			pNewList[nNewListSize] = aulSelctdList.GetAt(i);
			nNewListSize++;
		}

		if( bInkDieIn )
		{
			m_WaferMapWrapper.SelectGrade(pNewList, nNewListSize);
		}
		delete[] pNewList;
	}


	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);

	for (i = 0; i < aulSelectedGradeList.GetSize(); i++)
	{
		ucSelectedGrade = (UCHAR)(aulSelectedGradeList.GetAt(i) - m_WaferMapWrapper.GetGradeOffset());

		if (!GetIfGradeIsAssigned(ucSelectedGrade))
		{
			aulUnassignedGradeList.Add(aulSelectedGradeList.GetAt(i) - m_WaferMapWrapper.GetGradeOffset());
		}
		else
		{
			aulFilteredGradeList.Add(aulSelectedGradeList.GetAt(i));
		}
	}

	if (aulUnassignedGradeList.GetSize() > 0)
	{
		bReply = FALSE;

		szPopMsg = "Grade ";

		for (i = 0; i < aulUnassignedGradeList.GetSize(); i++)
		{
			szUnassignedGrade.Format("%d", aulUnassignedGradeList.GetAt(i));

			if (i == aulUnassignedGradeList.GetSize() - 1)
				szPopMsg = szPopMsg + szUnassignedGrade + " ";
			else
				szPopMsg = szPopMsg + szUnassignedGrade + ", ";
		}
		//szPopMsg = szPopMsg + "have no bin blocks assigned yet!";

		SetAlert_Msg(IDS_BT_GRADES_NOT_ASSIGN, szPopMsg);
		SetErrorMessage("Grades are not assigned");

		for (i = 0; i < aulFilteredGradeList.GetSize(); i++)
		{
			aaGrades[i] = (unsigned char)aulFilteredGradeList.GetAt(i);
		}

		m_WaferMapWrapper.SelectGrade(aaGrades, (unsigned long)aulFilteredGradeList.GetSize());
	}
	else
	{
		bReply = TRUE;
	}

	//v4.59A44
	if (!IsMotionHardwareReady())
	{
		bReply = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;

} //end CheckGradeAssignment


LONG CBinTable::ClearAllBinCounters(IPC_CServiceMessage& svMsg)
{
	ULONG i;

	StartLoadingAlert();

	//Clear Grade & Bin Counter
	m_oBinBlkMain.ClearAllGradeAndBinCounter();

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		m_oBinBlkMain.ClearBinCounterByPhyBlk(i, pBTfile);		//v4.52A8
		//m_oBinBlkMain.UpdateSerialNoGivenPhyBlkId(i);
	}
	CMSFileUtility::Instance()->SaveBTConfig();

	CloseLoadingAlert();

	//m_oBinBlkMain.CheckIfGradesAreStillValid();
	BackupToNVRAM();

	BOOL bRtn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bRtn);

	return 1;
} //end ClearAllBinCounters


LONG CBinTable::IfBinCntIsClrB4PickAndPlace(IPC_CServiceMessage& svMsg)
{
	BOOL bRtn = TRUE;

	if ( IsEnablePNP()==FALSE )
	{
		if (! IfAllBinCountsAreCleared())
		{
			bRtn = FALSE;

			SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
			SetErrorMessage("Bin counter is not cleared");
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bRtn);

	return 1;
} //end IfBinCntIsClrB4PickAndPlace


LONG CBinTable::CheckIsAllBinCleared(IPC_CServiceMessage& svMsg)
{
	BOOL bRtn = TRUE;

	if ( IfAllBinCountsAreCleared() == FALSE )
	{
		bRtn = FALSE;

		SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
		SetErrorMessage("Bin counter is not cleared");
	}

	svMsg.InitMessage(sizeof(BOOL), &bRtn);

	return 1;
}

LONG CBinTable::CheckIsBinCleared(IPC_CServiceMessage& svMsg)
{
	BOOL bRtn = TRUE;

	ULONG ulBinID;
	svMsg.GetMsg(sizeof(ULONG), &ulBinID);

	if ( (ulBinID > 0) && (ulBinID <= m_oBinBlkMain.GetNoOfBlk()) )
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(ulBinID) > 0)
		{
			bRtn = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bRtn);
	return 1;
}

LONG CBinTable::CheckIsAllBinCleared_PKG(IPC_CServiceMessage& svMsg)
{
	BOOL bRtn = TRUE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
//	if ( pApp->GetCustomerName() != "SeoulSemi" ) 
	if ( !pApp->m_bEnableSaveTempFileWithPKG )
	{
		if ( IfAllBinCountsAreCleared() == FALSE )
		{
			bRtn = FALSE;

			SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
			SetErrorMessage("Bin counter is not cleared  2*");
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bRtn);
	return 1;
}

LONG CBinTable::CheckIsAllBinClearedNoMsg(IPC_CServiceMessage& svMsg)
{
	BOOL bRtn = TRUE;

	if ( IfAllBinCountsAreCleared() == FALSE )
	{
		bRtn = FALSE;
		//HmiMessage("Error: Please clear bin count before proceeding!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bRtn);

	return 1;
}

LONG CBinTable::CheckIsAllBinClearedWithDieCountConstraint(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	ULONG lDieCount;
	svMsg.GetMsg(sizeof(ULONG), &lDieCount);

	for (INT i = 1; i <= (INT)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > lDieCount)
		{
			bReturn = FALSE;
		}
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::CheckIsAllBinNotFull(IPC_CServiceMessage& svMsg)
{
	BOOL bRtn = TRUE;
	UCHAR ucBin = 0;

	if ( (ucBin = IfAllBinCountsAreNotFull()) > 0 )
	{
		bRtn = FALSE;

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);

		CString szTemp;

		//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if ((pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_NEWLOT_EMPTYROW)) ||
			(pApp->GetCustomerName() == "CyOptics")		|| 
			(pApp->GetCustomerName() == "FiberOptics")	||		// = Dicon
			(pApp->GetCustomerName() == "Inari")	)	
		{
			//v4.53A6
			//Do not pop up error dialog for Dicon
			szTemp.Format("Bin #%d is full in grade slot; please clear bin frame and try again!", ucBin);
		}
		else
		{
			if (bPLLMSpecialFcn)	//pllm
				szTemp.Format("Bin frame #%d is full; please manually unload frame to FULL magazine.", ucBin);
			else
				szTemp.Format("Bin #%d is full in grade slot; please clear bin frame and try again!", ucBin);
			HmiMessage_Red_Yellow(szTemp);
		}

		SetErrorMessage(szTemp);
		svMsg.InitMessage(sizeof(UCHAR), &ucBin);
		return 1;
	}

	//svMsg.InitMessage(sizeof(BOOL), &bRtn);
	ucBin = 0;
	svMsg.InitMessage(sizeof(UCHAR), &ucBin);
	return 1;
}


/////////////////////////////////////////////
//           Wafer Statistics              //
/////////////////////////////////////////////
LONG CBinTable::WaferStatisticsPreTask(IPC_CServiceMessage& svMsg)
{
	if( IsAOIOnlyMachine() )
	{
		// after map loaded, call once, after prescan finish, call once, enter this page, call once.
		AOILoadMapGradesLegend(TRUE);
		BOOL bRtn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bRtn);

		return 1;
	}

	CStringArray szaData, szaDataSum;
	CString szNoOfSortedDie, szGrade, szBinBlk, szBinBlkList;
	CString szTotalSortedFromAllGrades, szCapacity, szInputCount, szBlkInUse;
	CString szOrgGradeString;
	ULONG ulNoOfSortedDie, i;
	ULONG j;
	ULONG ulCapacity, ulInputCount, ulBlkInUse, ulRowIndex;
	UCHAR ucGrade;
	int nFontSize = 17;

	m_ulTotalSortedFromAllGrades = 0;

	CMSLogFileUtility::Instance()->MS_WaferStatisticsPreTaskLog("WaferStatisticsPreTask Start");

	//Get wafermap selected grade
	int k;
	BOOL bGradeMatch;
	CString szOrgGrade;
	CUIntArray aulSelectedGradeList;
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
	CString szCategory = "";
	if (m_WaferMapWrapper.IsMapValid())
	{
		szCategory = m_WaferMapWrapper.GetReader()->GetHeader().GetCategory();
	}
	//Statistics Data
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabIsSetup(i))
		{
			bGradeMatch = FALSE;

			BOOL bHaveSameGrade = FALSE;
			for (j = 1; j <i; j++)
			{
				if (m_oBinBlkMain.GrabGrade(j) == m_oBinBlkMain.GrabGrade(i))
				{
					bHaveSameGrade = TRUE;
					break;
				}
			}

			if (i == 1)
			{
				CMSLogFileUtility::Instance()->MS_WaferStatisticsPreTaskLog("WaferStatisticsPreTask 1");
			}
			
			if ( bHaveSameGrade == FALSE )
			{
				ucGrade = m_oBinBlkMain.GrabGrade(i);
			}
			else
			{
				ucGrade = m_oBinBlkMain.GrabOriginalGrade(i);
			}

			//ucGrade = m_oBinBlkMain.GrabGrade(i);
			szGrade.Format("%d", ucGrade);

			ulRowIndex = m_oBinBlkMain.GrabStatRowIndex(ucGrade);

			//Capacity
			ulCapacity = m_oBinBlkMain.GrabGradeCapacity(ucGrade);
			szCapacity.Format("%d", ulCapacity);
			szaData.Add(szCapacity);

			//Input Count
			ulInputCount = m_oBinBlkMain.GrabInputCount(ucGrade);
			szInputCount.Format("%d", ulInputCount);
			szaData.Add(szInputCount);

			//Total No of Sorted Die
			//ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfSortedDie(ucGrade);
			//szNoOfSortedDie.Format("%d", ulNoOfSortedDie);
			//m_ulTotalSortedFromAllGrades += ulNoOfSortedDie;
			ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfSortedDie(ucGrade);
			szNoOfSortedDie.Format("%d", ulNoOfSortedDie);
			m_ulTotalSortedFromAllGrades += m_oBinBlkMain.GrabNVNoOfBondedDie(i);

			if (i == 1)
			{
				CMSLogFileUtility::Instance()->MS_WaferStatisticsPreTaskLog("WaferStatisticsPreTask 2");
			}

			szaData.Add(szNoOfSortedDie);

			//Bin Block(s)
			szBinBlkList.Format("%d", i);
			for (j = i + 1; j <= m_oBinBlkMain.GetNoOfBlk(); j++)
			{
				if (m_oBinBlkMain.GrabIsSetup(j))
				{
					if (m_oBinBlkMain.GrabGrade(j) == ucGrade)
					{
						szBinBlk.Format("%d", j);
						szBinBlkList = szBinBlkList + ";" + szBinBlk;
					}
				}
			}
			if ( ulCapacity == 0 )
			{
				szBinBlkList = "0";
			}
			szaData.Add(szBinBlkList);

			if (i == 1)
			{
				CMSLogFileUtility::Instance()->MS_WaferStatisticsPreTaskLog("WaferStatisticsPreTask 3");
			}

			//Block In Use
			//ulBlkInUse = m_oBinBlkMain.GrabBlkInUse(ucGrade);
			ulBlkInUse = m_oBinBlkMain.GrabNVBlkInUse(ucGrade);
			szBlkInUse.Format("%d", ulBlkInUse);
			if ( ulCapacity == 0 )
			{
				szBlkInUse = "0";
			}
			szaData.Add(szBlkInUse);

			//Change row color for grade is selected on wafermap or not
			for (k = 0; k < aulSelectedGradeList.GetSize(); k++)
			{
				if ( (ucGrade == (UCHAR)(aulSelectedGradeList.GetAt(k) - m_WaferMapWrapper.GetGradeOffset())) ) 
				{
					bGradeMatch = TRUE;
				}
			}

			szOrgGrade.Format("%d", m_WaferMapWrapper.GetOriginalGrade(ucGrade + m_WaferMapWrapper.GetGradeOffset()) - m_WaferMapWrapper.GetGradeOffset());

			/*
			if (m_WaferMapWrapper.GetReader() != NULL)
			{
				szOrgGradeString = m_WaferMapWrapper.GetReader()->GetOriginalGradeString(ucGrade + m_WaferMapWrapper.GetGradeOffset());

				if (szOrgGradeString != "")
				{
					szOrgGrade = szOrgGradeString;
				}
			}
			*/
			szOrgGradeString = m_WaferMapWrapper.GetOriginalGradeString(ucGrade + m_WaferMapWrapper.GetGradeOffset());

			if (szOrgGradeString != "" && szCategory != "")
			{
				szOrgGrade = szOrgGradeString;
			}

			szaData.Add(szOrgGrade);

			if ( bGradeMatch == TRUE )
			{
				m_BinTotalWrapper.ModifyData(ulRowIndex, 13158600, 8404992, ucGrade, nFontSize, szaData);
			}
			else
			{
				m_BinTotalWrapper.ModifyData(ulRowIndex, 8404992, 10987412, ucGrade, nFontSize, szaData);
			}

			if (i == 1)
			{
				CMSLogFileUtility::Instance()->MS_WaferStatisticsPreTaskLog("WaferStatisticsPreTask 4");
			}

			szaData.RemoveAll();
		}
	}

	szaDataSum.Add(" ");
	szaDataSum.Add(" ");
	szTotalSortedFromAllGrades.Format("%d", m_ulTotalSortedFromAllGrades);
	szaDataSum.Add(szTotalSortedFromAllGrades);
	m_ulTotalSorted = m_ulTotalSortedFromAllGrades;
	m_BinTotalWrapper.SetSum(8404992, 10987412, "Total", nFontSize, szaDataSum);

	m_BinTotalWrapper.UpdateTotal();

	CMSLogFileUtility::Instance()->MS_WaferStatisticsPreTaskLog("WaferStatisticsPreTask Complete");

	BOOL bRtn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bRtn);

	return 1;
} //end WaferStatisticsPreTask


///////////////////////////////////////////////
//           Bin Summary Output              //
///////////////////////////////////////////////
LONG CBinTable::BinSummaryOutput(IPC_CServiceMessage& svMsg)	
{
	CString szMachineNo;
	CString szTitle, szContent;
	
	szMachineNo	= GetMachineNo();
	
	if (m_oBinBlkMain.GenBinSummaryOutputFile(m_szBinSummaryOutputPath, szMachineNo) == TRUE)
	{
		szTitle = "";
		szContent.LoadString(HMB_GENERAL_FILE_GEN);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
	}
    else
	{
		szTitle.LoadString(HMB_GENERAL_ERROR );
		szContent.LoadString(HMB_GENERAL_INVALID_PATH);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
	}
	
	return 1;
} //end BinSummaryOutput


/////////////////////////////////////////////////
//           Clear Block Settings              //
/////////////////////////////////////////////////
LONG CBinTable::IfIsClearAllSettings(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szSelection;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szSelection = &pBuffer[0];

	delete [] pBuffer;

	if (szSelection == "Single Block")
		m_bIfEnablePIBlkToClrSettings = TRUE;
	else if (szSelection == "All Blocks")
		m_bIfEnablePIBlkToClrSettings = FALSE;

	return 1;
} //end IfIsClearAllSettings


//////////////////////////////////////////////////////////
//           Clear Physical Block Settings              //
//////////////////////////////////////////////////////////
LONG CBinTable::SubmitClrPhyBlkSettings(IPC_CServiceMessage& svMsg)
{
	BOOL bFcnReply;
	IPC_CServiceMessage stMsg;
	int nConvID;
	LONG lSPCBin = 0;

	LONG lHmiMsgReply = 3;	//Yes	//Klocwork
	ULONG i, ulBlkId;
	CString szBinBlkId, szTempFilename, szSelection;
	CString szLogFilename;
	CString szTitle;
	CString szContent;

	char *pBuffer;
	BOOL bBinNotCleared = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSelection = &pBuffer[sizeof(ULONG)];

	delete[] pBuffer;

	szTitle.LoadString(HMB_BT_SETUP_PHY_BLK);

	if (szSelection == "Single Block")
	{
		//check bin counter > 0
		for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			if ( (m_oBinBlkMain.GrabPhyBlkId(i) == ulBlkId) && (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0) )
			{
				bBinNotCleared = TRUE;
				break;
			}
		}

		if ( bBinNotCleared == TRUE )
		{
			szContent.LoadString(HMB_BT_CONF_CLEAR_SETTING);
			if ( HmiMessage(szContent, szTitle, 103, 3) != 3 )
			{
				bFcnReply = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bFcnReply);
				return 1;
			}
		}

		szContent.LoadString(HMB_BT_CLEAR_ONE_PHY_BLK);

		lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); 

		if (lHmiMsgReply == 3) //3 = Yes
		{
			m_oBinBlkMain.ClearBinCounterByPhyBlk(ulBlkId);
			m_oPhyBlkMain.ClrSingleBlkSettings(ulBlkId);
			m_oBinBlkMain.ClrBlkSettingsGivenPhyBlkId(ulBlkId);
	
			switch (m_ulMachineType)
			{
				case BT_MACHTYPE_STD:
					PhyBlksDrawing();
				break;

				case BT_MACHTYPE_DTABLE:		//v4.16T1
				case BT_MACHTYPE_DBUFFER:		//v3.71T4
				case BT_MACHTYPE_DL_DLA:
					PhyBlksDrawing_BL(ulBlkId);
				break;
			}
			LoadWaferStatistics();

			SetStatusMessage("Physical block setting is cleared");

			szContent.LoadString(HMB_BT_PHY_BLK_CLEARED);

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

			//clear SPC data
			if (CMS896AStn::m_bUsePostBond == TRUE)
			{
				lSPCBin = (LONG)ulBlkId;
				stMsg.InitMessage(sizeof(LONG), &lSPCBin);
				nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeData", stMsg);
				m_comClient.ScanReplyForConvID(nConvID, 5000);
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
			}
		}
	}
	else if (szSelection == "All Blocks")
	{
		//check any bin counter > 0
		for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
			{
				bBinNotCleared = TRUE;
				break;
			}
		}

		if ( bBinNotCleared == TRUE )
		{
			szContent.LoadString(HMB_BT_CONF_CLEAR_SETTING);
			if ( HmiMessage(szContent, szTitle, 103, 3) != 3 )
			{
				bFcnReply = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bFcnReply);
				return 1;
			}
		}

		szContent.LoadString(HMB_BT_CLEAR_ALL_PHY_BLK);

		lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); //103 = Yes, No
											      //3 = align Center
		if (lHmiMsgReply == 3) //3 = Yes
		{
			m_oPhyBlkMain.ClrAllBlksSettings();
			m_oBinBlkMain.ClrAllBlksSettings();
			m_oBinBlkMain.ClrAllGradeInfo();

			//Remove temp files
			for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
			{
				szBinBlkId.Format("%d", i);
				
				if (i < 10)
					szBinBlkId = "0" + szBinBlkId;

				szTempFilename	= gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBinBlkId + "TempFile.csv";

				remove(szTempFilename);
//	SanAn TJ 2017
				for(int mm=0; mm<3; mm++)
				{
					if (_access(szTempFilename, 0) != -1)
					{
						SetErrorMessage("BT submit clr phy bin, re-delete " + szTempFilename);
						DeleteFile(szTempFilename);
					}
					else
					{
						break;
					}
				}
//	SanAn TJ 2017
			}

			switch (m_ulMachineType)
			{
				case BT_MACHTYPE_STD:
					PhyBlksDrawing();
				break;

				case BT_MACHTYPE_DTABLE:		//v4.16T1
				case BT_MACHTYPE_DBUFFER:		//v3.71T4
				case BT_MACHTYPE_DL_DLA:
					PhyBlksDrawing_BL(1);
			}
			LoadWaferStatistics();

			SetStatusMessage("All physical block setting are cleared");

			szContent.LoadString(HMB_BT_PHY_BLK_CLEARED);

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);


			//Clear ALL SPC data
			if (CMS896AStn::m_bUsePostBond == TRUE)
			{
				nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeAllData", stMsg);
				m_comClient.ScanReplyForConvID(nConvID, 5000);
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
			}
		}
	}

	if (lHmiMsgReply == 3) //3 = Yes
	{
		//Reset Hmi Variables on the page

		m_ulNoOfPhyBlk = m_oPhyBlkMain.GetNoOfBlk();

		if (m_ulNoOfPhyBlk > 0)
			m_bIfEnablePIBlkToSetup = TRUE;
		else
			m_bIfEnablePIBlkToSetup = FALSE;

		m_ulPhyBlkToSetup = 1;
		m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(1));
		m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(1));
		m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(1));
		m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(1));

		m_lPhyBlkPitchX = ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GetBlkPitchX());
		m_lPhyBlkPitchY = ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GetBlkPitchY());

		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;

		bFcnReply = TRUE;
	}
	else
	{
		bFcnReply = FALSE;
	}

	
	svMsg.InitMessage(sizeof(BOOL), &bFcnReply);
	return 1;
} //end SubmitClrPhyBlkSettings

/////////////////////////////////////////////////////
//           Clear Bin Block Settings              //
/////////////////////////////////////////////////////
LONG CBinTable::SubmitClrBinBlkSettings(IPC_CServiceMessage& svMsg)
{
	LONG lHmiMsgReply = 3;	//Yes	//Klocwork
	ULONG ulBlkId, i;
	CString szBinBlkId, szTempFilename, szSelection;
	CString szLogFilename;
	CString szTitle;
	CString szContent;
	char *pBuffer;
	BOOL bFcnReply;
	BOOL bBinNotCleared = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSelection = &pBuffer[sizeof(ULONG)];

	delete[] pBuffer;

	szTitle.LoadString(HMB_BT_SETUP_BIN_BLK);

	if (szSelection == "Single Block")
	{
		//check bin counter > 0
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkId) > 0)
		{
			szContent.LoadString(HMB_BT_CONF_CLEAR_SETTING);
			if ( HmiMessage(szContent, szTitle, 103, 3) != 3 )
			{
				bFcnReply = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bFcnReply);
				return 1;
			}
		}

		szContent.LoadString(HMB_BT_CLEAR_ONE_BIN_BLK);

		lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); 

		if (lHmiMsgReply == 3) //3 = Yes
		{
			m_oBinBlkMain.ClearBinCounter(ulBlkId);
			m_oBinBlkMain.CheckIfGradesAreStillValid();
			m_oBinBlkMain.ClrSingleBlkSettings(ulBlkId);

			switch (m_ulMachineType)
			{
				case BT_MACHTYPE_STD:
					BinBlksDrawing();
				break;

				case BT_MACHTYPE_DTABLE:		//v4.16T1
				case BT_MACHTYPE_DBUFFER:		//v3.71T4
				case BT_MACHTYPE_DL_DLA: 
					BinBlksDrawing_BL(ulBlkId);
			}

			WriteGradeLegend();
			LoadGradeRankID();
			LoadWaferStatistics();

			SetStatusMessage("Bin block setting is cleared");

			szContent.LoadString(HMB_BT_BIN_BLK_CLEARED);

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		}
	}
	else if (szSelection == "All Blocks")
	{
		//check any bin counter > 0
		for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
			{
				bBinNotCleared = TRUE;
				break;
			}
		}

		if ( bBinNotCleared == TRUE )
		{
			szContent.LoadString(HMB_BT_CONF_CLEAR_SETTING);
			if ( HmiMessage(szContent, szTitle, 103, 3) != 3 )
			{
				bFcnReply = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bFcnReply);
				return 1;
			}
		}

		szContent.LoadString(HMB_BT_CLEAR_ALL_BIN_BLK);

		lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); 
							//103 = Yes, No; 3 = align Center

		if (lHmiMsgReply == 3) //3 = Yes
		{
			m_oBinBlkMain.ClrAllBlksSettings();
			m_oBinBlkMain.ClrAllGradeInfo();

			//Remove temp files
			for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
			{
				szBinBlkId.Format("%d", i);
				
				if (i < 10)
					szBinBlkId = "0" + szBinBlkId;

				szTempFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBinBlkId + "TempFile.csv";

				remove(szTempFilename);
//	SanAn TJ 2017
				for(int mm=0; mm<3; mm++)
				{
					if (_access(szTempFilename, 0) != -1)
					{
						SetErrorMessage("BT submit clr bin, re-delete " + szTempFilename);
						DeleteFile(szTempFilename);
					}
					else
					{
						break;
					}
				}
//	SanAn TJ 2017
			}

			switch (m_ulMachineType)
			{
				case BT_MACHTYPE_STD:
					BinBlksDrawing();
				break;

				case BT_MACHTYPE_DTABLE:		//v4.16T1
				case BT_MACHTYPE_DBUFFER:		//v3.71T4
				case BT_MACHTYPE_DL_DLA:
					BinBlksDrawing_BL(1);
				break;
			}

			WriteGradeLegend();

			LoadWaferStatistics();

			SetStatusMessage("All Bin block setting are cleared");

			szContent.LoadString(HMB_BT_BIN_BLK_CLEARED);

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		}
	}

	if (lHmiMsgReply == 3) //3 = Yes
	{
		//Reset Hmi Variables on the page
		m_szSetupMode = "Single Block";

		m_ulNoOfBinBlk = m_oBinBlkMain.GetNoOfBlk();

		if (m_ulNoOfBinBlk > 0)
			m_ulBinBlkToSetup = 1;

		m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(1));
		m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(1));
		m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(1));
		m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(1));

		m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
		m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

		m_ucGrade = m_oBinBlkMain.GrabGrade(1);

		m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(1));		//v4.59A19
		m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(1));		//v4.59A19
		m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(1);
		m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(1);
		m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(1);
		m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(1);
		m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(1);	//v4.59A22	//David Ma
		m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(1);	//v4.59A22	//David Ma
		m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(1);

		m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(1));

		m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(1);
		m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(1);
		m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(1);
		m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(1);	
		m_ulMaxUnit	  = m_oBinBlkMain.GrabMaxUnit(1);
		m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(1);
		m_ulFirstRowColSkipUnit			= m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(1);
		m_bTeachWithPhysicalBlk		= m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(1);

		//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
		m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(ulBlkId);
		m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(ulBlkId);
		m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(ulBlkId);
		
		DOUBLE dRadius = (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(ulBlkId));		// from encoder step to um
		m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 

		m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(1);
		if (m_bCentralizedBondArea)
		{
			m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(1);
			m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(1);
			m_lCentralizedUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(1) + m_lCentralizedOffsetX);
			m_lCentralizedUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(1) + m_lCentralizedOffsetY);
		}
		else
		{
			m_lCentralizedUpperLeftX = 0;
			m_lCentralizedUpperLeftY = 0;
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;
			m_lWafflePadSizeX = 0;
			m_lWafflePadSizeY = 0;
		}

		m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(1);

		if (m_bEnableWafflePad)
		{
			m_lWafflePadDistX = ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabWafflePadDistX(1));
			m_lWafflePadDistY = ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabWafflePadDistY(1));
			m_lWafflePadSizeX = m_oBinBlkMain.GrabWafflePadSizeX(1);
			m_lWafflePadSizeY = m_oBinBlkMain.GrabWafflePadSizeY(1);
		}
		else
		{
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;
			m_lWafflePadSizeX = 0;
			m_lWafflePadSizeY = 0;
		}

		m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(1);
		m_bNoReturnTravel = m_oBinBlkMain.GrabNoReturnTravel(1);	//v4.42T1

		m_bIfEnableCBSetupMode = TRUE;
		m_bIfEnablePINoOfBlk = TRUE;

		if (m_ulNoOfBinBlk > 0)
			m_bIfEnablePIBlkToSetup = TRUE;
		else
			m_bIfEnablePIBlkToSetup = FALSE;

		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnablePIDiePitchX = FALSE;
		m_bIfEnablePIDiePitchY = FALSE;
		m_bIfEnableChkBAutoAssignGrade = FALSE;
		m_bIfEnablePIGrade = FALSE;
		m_bIfEnableCBWalkPath = FALSE;

		m_bIfEnablePIDiePerRow = FALSE;
		m_bIfEnablePIDiePerCol = FALSE;
		m_bIfEnablePIDiePerBlk = FALSE;

		m_bIfEnableFirstRowColSkipPattern = FALSE;
		m_bIfEnableTeachWithPhysicalBlk = FALSE;

		m_bIfEnable2DBarcodeOutput = FALSE;
		m_bIfEnableTemplateSetup = FALSE;
		m_bIfEnableCentralizedBondArea = FALSE;
		m_bIfEnableWafflePad = FALSE;
		m_bIfEnableWafflePadCheckBox = FALSE;
		m_bIfEnablePadPitchSetX = FALSE;
		m_bIfEnablePadPitchSetY = FALSE;
		m_bIfEnablePadPitchResetX = FALSE;
		m_bIfEnablePadPitchResetY = FALSE;
		m_bIfEnablePadDimX = FALSE;
		m_bIfEnablePadDimY = FALSE;
		m_bIfUseBlockCornerAsFirstDiePos = FALSE;

		m_bIfEnableTBSubmit = FALSE;
		m_bIfEnableTBCancel = FALSE;

		bFcnReply = TRUE;
	}
	else
	{
		bFcnReply = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bFcnReply);

	BackupToNVRAM();

	return 1;
} //end SubmitClrBinBlkSettings


//////////////////////////////////////////////////////////
//					Physical Block Setup				//
//////////////////////////////////////////////////////////
LONG CBinTable::PhyBlkSetupPreRoutine(IPC_CServiceMessage& svMsg)
{
	//Enability of Hmi Controls
	m_bIfEnableTBEditBlkSettings = TRUE;
	m_bIfEnableTBClrBlkSettings = TRUE;

	m_bIfEnableCBSetupMode = TRUE;
	m_bIfEnablePINoOfBlk = TRUE;

	if (m_oPhyBlkMain.GetNoOfBlk() > 0)
		m_bIfEnablePIBlkToSetup = TRUE;
	else
		m_bIfEnablePIBlkToSetup = FALSE;

	m_bIfEnableTBSetUL = FALSE;
	m_bIfEnableTBSetLR = FALSE;
	m_bIfEnableTBConfirmUL = FALSE;
	m_bIfEnableTBConfirmLR = FALSE;
	m_bIfEnableDDUpperLeftX = FALSE;
	m_bIfEnableDDUpperLeftY = FALSE;
	m_bIfEnableDDLowerRightX = FALSE;
	m_bIfEnableDDLowerRightY = FALSE;
	m_bIfEnablePIBlkPitchX = FALSE;
	m_bIfEnablePIBlkPitchY = FALSE;
	m_bIfEnableTBSubmit = FALSE;
	m_bIfEnableTBCancel = FALSE;

	//Grab values for Screen Display (Hmi Reg Variables)
	m_szClrBlkSettingsSelection = "Single Block";

	m_ulPhyBlkToClrSettings = 1;

	if (m_oPhyBlkMain.GetNoOfBlk() > 0)
	{
		m_bIfEnableCBClrBlkSettingsSelection = TRUE;
		m_bIfEnablePIBlkToClrSettings = TRUE;
		m_bIfEnableTBSubmitClrBlkSettings = TRUE;
	}
	else //there is no physcial block setup yet
	{
		m_bIfEnableCBClrBlkSettingsSelection = FALSE;
		m_bIfEnablePIBlkToClrSettings = FALSE;
		m_bIfEnableTBSubmitClrBlkSettings = FALSE;
	}
	
	//Initialization of variables
	m_bFirstTimeSetUL = TRUE; 
	m_bFirstTimeSetLR = TRUE;

	//Grab values for Screen Display (Hmi Reg Variables)
	m_szSetupMode = "Single Block";
	m_ulNoOfPhyBlk = m_oPhyBlkMain.GetNoOfBlk();
	m_ulPhyBlkToSetup = 0;
	m_lPhyUpperLeftX = 0;
	m_lPhyUpperLeftY = 0;
	m_lPhyLowerRightX = 0;
	m_lPhyLowerRightY = 0;
	m_lPhyBlkPitchX = ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GetBlkPitchX());
	m_lPhyBlkPitchY = ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GetBlkPitchY());

	switch (m_ulMachineType)
	{
		case BT_MACHTYPE_STD:
			PhyBlksDrawing();
		break;

		case BT_MACHTYPE_DTABLE:		//v4.16T1
		case BT_MACHTYPE_DBUFFER:		//v3.71T4
		case BT_MACHTYPE_DL_DLA:
			PhyBlksDrawing_BL(1);
		break;
	}

	return 1;
} //end PhyBlkSetupPreRoutine


LONG CBinTable::OnChangePhyBlkSetupModeSelection(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId, i;
	CString szSetupMode;
	char *pBuffer;
	BOOL bIfContainBinBlk;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	bIfContainBinBlk = FALSE;

	for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
	{
		bIfContainBinBlk = m_oBinBlkMain.IfContainBinBlk(i);

		if (bIfContainBinBlk)
		{
			m_bIfEnablePINoOfBlk = FALSE;

			if (szSetupMode == "All Blocks")
			{
				m_bIfEnablePIBlkToSetup = FALSE;
				SetAlert(IDS_BT_DENYBLKSETTINGSEDIT1);
			}

			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnablePIBlkPitchX = FALSE;
			m_bIfEnablePIBlkPitchY = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel = FALSE;
	
			break;
		}
	}

	if (szSetupMode == "All Blocks")
	{
		if (! bIfContainBinBlk)
		{
			m_bIfEnablePINoOfBlk = TRUE;
			m_bIfEnablePIBlkPitchX = TRUE;
			m_bIfEnablePIBlkPitchY = TRUE;

			if (m_ulNoOfPhyBlk > 0)
			{
				m_bIfEnablePIBlkToSetup = TRUE;
				m_bIfEnableTBSetUL = TRUE;
				m_bIfEnableTBSetLR = TRUE;
				m_bIfEnableTBSubmit = TRUE;
				m_bIfEnableTBCancel = TRUE;

				m_ulPhyBlkToSetup = 1;

				//Display Blk 1 Data
				m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(1));	
				m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(1));
				m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(1));
				m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(1));
			}
			else
			{
				m_bIfEnablePIBlkToSetup = FALSE;
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;
			}
		}
	}
	else if (szSetupMode == "Single Block")
	{
		m_bIfEnablePINoOfBlk = TRUE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;

		if (m_ulNoOfPhyBlk > 0)
		{
			m_bIfEnablePIBlkToSetup = TRUE;
		}

		bIfContainBinBlk = m_oBinBlkMain.IfContainBinBlk(ulBlkId);

		if (bIfContainBinBlk)
		{
			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel = FALSE;

			SetAlert(IDS_BT_DENYBLKSETTINGSEDIT1);
		}
		else
		{
			m_bIfEnableTBSetUL = TRUE;
			m_bIfEnableTBSetLR = TRUE;

			if (m_oPhyBlkMain.GrabIsSetup(ulBlkId))
			{
				m_bIfEnableTBSubmit = TRUE;
				m_bIfEnableTBCancel = TRUE;
			}
		}
	}

	return 1;
} //end OnChangePhyBlkSetupModeSelection


LONG CBinTable::OnClickNoOfPhyBlkInput(IPC_CServiceMessage& svMsg)
{
	ULONG ulNoOfBlk;
	CString szSetupMode;
	char *pBuffer;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulNoOfBlk, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	if (ulNoOfBlk == 0)
	{
		m_bIfEnablePIBlkToSetup = FALSE;
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnableTBSubmit = FALSE;
		m_bIfEnableTBCancel = FALSE;

		SetAlert(IDS_BT_NOOFBLKSCHK);
	}
	else //no of blks > 0
	{
		m_bIfEnablePIBlkToSetup = TRUE;
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnableTBSubmit = TRUE;
		m_bIfEnableTBCancel = TRUE;

		if (szSetupMode == "All Blocks")
		{
			m_bIfEnablePIBlkPitchX = TRUE;
			m_bIfEnablePIBlkPitchY = TRUE;
		}
		else if (szSetupMode == "Single Block")
		{
			m_bIfEnablePIBlkPitchX = FALSE;
			m_bIfEnablePIBlkPitchY = FALSE;
		}
	}
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
} //end OnClickNoOfPhyBlkInput


LONG CBinTable::OnClickPhyBlkToSetupInput(IPC_CServiceMessage& svMsg)
{
	ULONG i;
	BOOL bIfContainBinBlk;

	typedef struct 
	{
		ULONG	ulPhyBlkId;
		ULONG	ulBinBlkId;	
	} BLKDATA;
	BLKDATA stInfo;

	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

	bIfContainBinBlk = FALSE;

	m_bIfEnableTBSubmit = TRUE;
	m_bIfEnableTBCancel = TRUE;

	if (m_szSetupMode == "All Blocks")
	{
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;

		if (stInfo.ulPhyBlkId != 1)
		{
			SetAlert(IDS_BT_EDITBLK1DATAONLY);
			
			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel = FALSE;
		}
		else
		{
			for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
			{
				bIfContainBinBlk = m_oBinBlkMain.IfContainBinBlk(i);

				if (bIfContainBinBlk)
				{
					m_bIfEnablePINoOfBlk = FALSE;
					m_bIfEnablePIBlkToSetup = FALSE;
					m_bIfEnableTBSetUL = FALSE;
					m_bIfEnableTBSetLR = FALSE;
					m_bIfEnablePIBlkPitchX = FALSE;
					m_bIfEnablePIBlkPitchY = FALSE;
					m_bIfEnableTBSubmit = FALSE;
					m_bIfEnableTBCancel = FALSE;

					//Block settings can't be edited because there are bin blocks.
					SetAlert(IDS_BT_DENYBLKSETTINGSEDIT1);
			
					break;
				}
			}

			if (! bIfContainBinBlk)
			{
				m_bIfEnableTBSetUL = TRUE;
				m_bIfEnableTBSetLR = TRUE;
			}
		}
	}
	else if (m_szSetupMode == "Single Block")
	{
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;

		bIfContainBinBlk = m_oBinBlkMain.IfContainBinBlk(stInfo.ulPhyBlkId);

		if (bIfContainBinBlk)
		{
			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel = FALSE;

			//Block settings can't be edited because it contains bin blocks.
			SetAlert(IDS_BT_DENYBLKSETTINGSEDIT1);
		}
		else
		{
			m_bIfEnableTBSetUL = TRUE;
			m_bIfEnableTBSetLR = TRUE;
			m_bIfEnableTBSubmit = TRUE;
			m_bIfEnableTBCancel = TRUE;
		}
	}

	switch (m_ulMachineType)
	{
		case BT_MACHTYPE_STD:
			PhyBlksDrawing();
			break;

		case BT_MACHTYPE_DTABLE:		//v4.16T1
		case BT_MACHTYPE_DBUFFER:		//v3.71T4
		case BT_MACHTYPE_DL_DLA:
			PhyBlksDrawing_BL(stInfo.ulPhyBlkId);
			break;
	}

	//Update UL and LR values upon selecting "Block to setup"
	m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(stInfo.ulPhyBlkId));	
	m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(stInfo.ulPhyBlkId));
	m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(stInfo.ulPhyBlkId));
	m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(stInfo.ulPhyBlkId));

	SetJoystickOn(FALSE);

	if (m_bUseDualTablesOption)		//v4.17T5
		HomeTable2();

	MoveXYTo(ConvertFileUnitToXEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftX(stInfo.ulPhyBlkId)), 
			 ConvertFileUnitToYEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftY(stInfo.ulPhyBlkId)));

	return 1;
} //end OnClickPhyBlkToSetupInput


LONG CBinTable::CheckIfNoOfPhyBlksIsValid(IPC_CServiceMessage& svMsg)
{
	ULONG ulNoOfBlk;
	CString szSetupMode;
	char *pBuffer;
	BOOL bFcnReply;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulNoOfBlk, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	m_bIfEnableTBSetUL = FALSE;
	m_bIfEnableTBSetLR = FALSE;
	m_bIfEnableTBSubmit = FALSE;
	m_bIfEnableTBCancel = TRUE;

	if (ulNoOfBlk == 0)
	{
		m_bIfEnablePIBlkToSetup = FALSE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;

		SetAlert(IDS_BT_NOOFBLKSCHK);

		bFcnReply = FALSE;
	}
	else //no of blks > 0
	{
		m_bIfEnablePIBlkToSetup = TRUE;
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnableTBSubmit = TRUE;

		//m_ulPhyBlkToSetup = 1;
		m_bIfEnablePIBlkToSetup = TRUE;

		//Display Block 1 Data
		//m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(1));	
		//m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(1));
		//m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(1));
		//m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(1));


		if (szSetupMode == "All Blocks")
		{
			//m_ulPhyBlkToSetup = 1;

			m_bIfEnablePIBlkPitchX = TRUE;
			m_bIfEnablePIBlkPitchY = TRUE;
			//m_bIfEnablePIBlkToSetup = TRUE;

			//Display Block 1 Data
			//m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(1));	
			//m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(1));
			//m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(1));
			//m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(1));
		}
		else if (szSetupMode == "Single Block")
		{
			m_bIfEnablePIBlkPitchX = FALSE;
			m_bIfEnablePIBlkPitchY = FALSE;
		}

		bFcnReply = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bFcnReply);

	return 1;
} //end CheckIfNoOfPhyBlksIsValid


LONG CBinTable::CheckIfPhyBlkToSetupIsValid(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;
	BOOL bFcnReply;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	m_bIfEnableTBSetUL = FALSE;
	m_bIfEnableTBSetLR = FALSE;
	m_bIfEnableTBCancel = TRUE;

	if (ulBlkId == 0)
	{
		m_bIfEnableTBSubmit = FALSE;
		SetAlert(IDS_BT_BLKTOSETUPCHK);

		bFcnReply = FALSE;
	}
	else
	{
		m_bIfEnableTBSubmit = TRUE;
		bFcnReply = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bFcnReply);

	return 1;
} //end CheckIfPhyBlkToSetupIsValid


LONG CBinTable::CheckIfContainsBinBlks(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId, i;
	CString szSetupMode;
	char *pBuffer;
	BOOL bIfContainBinBlk;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	bIfContainBinBlk = FALSE;

	if (szSetupMode == "Single Block") 
	{
		m_bIfEnablePINoOfBlk = TRUE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;

		bIfContainBinBlk = m_oBinBlkMain.IfContainBinBlk(ulBlkId);

		if (bIfContainBinBlk)
		{
			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel = FALSE;

			SetAlert(IDS_BT_DENYBLKSETTINGSEDIT1);
		}
		else
		{
			m_bIfEnableTBSetUL = TRUE;
			m_bIfEnableTBSetLR = TRUE;

			if (m_oPhyBlkMain.GrabIsSetup(ulBlkId))
			{
				m_bIfEnableTBSubmit = TRUE;
				m_bIfEnableTBCancel = TRUE;
			}
		}
	}
	else if (szSetupMode == "All Blocks")
	{
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;

		for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
		{
			bIfContainBinBlk = m_oBinBlkMain.IfContainBinBlk(i);

			if (bIfContainBinBlk)
			{
				m_bIfEnablePINoOfBlk = FALSE;
				m_bIfEnablePIBlkToSetup = FALSE;
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;

				SetAlert(IDS_BT_DENYBLKSETTINGSEDIT1);
		
				break;
			}
		}

		if (! bIfContainBinBlk)
		{
			//m_bIfEnableTBSetUL = TRUE;
			//m_bIfEnableTBSetLR = TRUE;
			m_bIfEnablePIBlkPitchX = TRUE;
			m_bIfEnablePIBlkPitchY = TRUE;

			if (ulBlkId == 1)
			{
				m_bIfEnableTBSetUL = TRUE;
				m_bIfEnableTBSetLR = TRUE;

				//Display Block 1 Data
				m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(1));	
				m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(1));
				m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(1));
				m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(1));

			}
			else
			{
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
			}
		}
	}

	return 1;
} //end CheckIfContainsBinBlks


LONG CBinTable::SetPhyBlkSetupUL(IPC_CServiceMessage& svMsg)    
{
	ULONG ulBlkId;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	m_bIfEnableTBClrBlkSettings = FALSE;

	m_bIfEnableCBSetupMode = FALSE;
	m_bIfEnablePINoOfBlk = FALSE;
	m_bIfEnablePIBlkToSetup = FALSE;

	m_bIfEnableTBConfirmUL = TRUE;
	m_bIfEnableTBSetLR = FALSE;
	m_bIfEnableDDUpperLeftX = TRUE;
	m_bIfEnableDDUpperLeftY = TRUE;

	m_bIfEnablePIBlkPitchX = FALSE;
	m_bIfEnablePIBlkPitchY = FALSE;
	m_bIfEnableTBSubmit = FALSE;
	m_bIfEnableTBCancel = TRUE;

	LONG lXInFileUnit, lYInFileUnit;

	SetJoystickOn(FALSE);

	if (m_bUseDualTablesOption)		//v4.17T5
		HomeTable2();

	//Move to UL position 
	if (m_bFirstTimeSetUL)	//first time to set
	{
		if (m_oPhyBlkMain.GrabIsSetup(ulBlkId)) //load structure default values
		{
			MoveXYTo(ConvertFileUnitToXEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftX(ulBlkId)),
					 ConvertFileUnitToYEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftY(ulBlkId)));
		}
		else	//stay at lower right position
		{
			lXInFileUnit = ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
			lYInFileUnit = ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);

			MoveXYTo(ConvertFileUnitToXEncoderValue(0 - lXInFileUnit),
					 ConvertFileUnitToYEncoderValue(0 - lYInFileUnit));
		}
	}
	else	//load previous values for minor adjustment
	{ 
		lXInFileUnit = ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
		lYInFileUnit = ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);

		MoveXYTo(ConvertFileUnitToXEncoderValue(0 - lXInFileUnit),
				 ConvertFileUnitToYEncoderValue(0 - lYInFileUnit));
	}
	SetJoystickOn(TRUE);

	m_ulJoystickFlag = 1; //indicate now is setting Upper Left of Physical Block

	return 1;
} //end SetPhyBlkSetupUL


LONG CBinTable::ConfirmPhyBlkSetupUL(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;
	LONG lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY;
	CString szSetupMode;
	char *pBuffer;
	BOOL bIsValidDimension;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	m_bFirstTimeSetUL = FALSE;

	m_bIfEnableTBClrBlkSettings = TRUE;

	m_bIfEnableCBSetupMode = TRUE;
	m_bIfEnablePINoOfBlk = TRUE;
	m_bIfEnablePIBlkToSetup = TRUE;

	if (szSetupMode == "Single Block")
	{
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
	}
	else if (szSetupMode == "All Blocks")
	{
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;
	}

	if (((szSetupMode == "All Blocks") && (m_ulPhyBlkToSetup == 1)) 
		|| (szSetupMode == "Single Block"))

	{
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnableTBCancel = TRUE;
	}
	else
	{
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnableTBCancel = FALSE;
	}
	m_bIfEnableTBConfirmUL = FALSE;
	m_bIfEnableDDUpperLeftX = FALSE;
	m_bIfEnableDDUpperLeftY = FALSE;

	SetJoystickOn(FALSE);
	m_ulJoystickFlag = 0;

	lConvertedULX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
	lConvertedULY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);
	lConvertedLRX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
	lConvertedLRY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);

	m_bIfEnableTBSubmit = TRUE;

	//Validation (only apply to standard bin table)
	switch (m_ulMachineType)
	{
		case BT_MACHTYPE_STD:
			if ((m_oPhyBlkMain.GrabIsSetup(ulBlkId)) || (m_szSetupMode == "All Blocks"))
			{
				bIsValidDimension = m_oPhyBlkMain.CheckOverlappingProblem(ulBlkId, 
					lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY);

				if (!bIsValidDimension)
				{
					m_bIfEnableTBSubmit = FALSE;

					SetAlert(m_oPhyBlkMain.GetMsgCode());
				}
			}
			break;
	}

	return 1;
} //end ConfirmPhyBlkSetupUL


LONG CBinTable::SetPhyBlkSetupLR(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	m_bIfEnableTBClrBlkSettings = FALSE;

	m_bIfEnableCBSetupMode = FALSE;
	m_bIfEnablePINoOfBlk = FALSE;
	m_bIfEnablePIBlkToSetup = FALSE;

	m_bIfEnableTBConfirmLR = TRUE;
	m_bIfEnableTBSetUL = FALSE;
	m_bIfEnableDDLowerRightX = TRUE;
	m_bIfEnableDDLowerRightY = TRUE;

	m_bIfEnablePIBlkPitchX = FALSE;
	m_bIfEnablePIBlkPitchY = FALSE;
	m_bIfEnableTBSubmit = FALSE;
	m_bIfEnableTBCancel = TRUE;

	LONG lXInFileUnit, lYInFileUnit;

	SetJoystickOn(FALSE);

	if (m_bUseDualTablesOption)		//v4.17T5
		HomeTable2();

	//Move to LR position
	if (m_bFirstTimeSetLR) //first time to set
	{
		if (m_oPhyBlkMain.GrabIsSetup(ulBlkId)) //load structure default values
		{
			MoveXYTo(ConvertFileUnitToXEncoderValue(m_oPhyBlkMain.GrabBlkLowerRightX(ulBlkId)),
					 ConvertFileUnitToYEncoderValue(m_oPhyBlkMain.GrabBlkLowerRightY(ulBlkId)));
		}
		else //stay at upper left position
		{
			lXInFileUnit = ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
			lYInFileUnit = ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);

			MoveXYTo(ConvertFileUnitToXEncoderValue(0 - lXInFileUnit),
					 ConvertFileUnitToYEncoderValue(0 - lYInFileUnit));
		}
	}
	else	//load previous values for minor adjustment
	{
		lXInFileUnit = ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
		lYInFileUnit = ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);

		MoveXYTo(ConvertFileUnitToXEncoderValue(0 - lXInFileUnit),
				 ConvertFileUnitToYEncoderValue(0 - lYInFileUnit));
	}
	SetJoystickOn(TRUE);

	m_ulJoystickFlag = 2;	//indicate now is setting Lower Right of Physical Block

	return 1;
} //end SetPhyBlkSetupLR


LONG CBinTable::ConfirmPhyBlkSetupLR(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;
	CString szSetupMode;
	char *pBuffer;
	BOOL bIsValidLowerRight, bIsValidDimension;
	LONG lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	m_bIfEnableTBClrBlkSettings = TRUE;

	m_bIfEnableCBSetupMode = TRUE;
	m_bIfEnablePINoOfBlk = TRUE;
	m_bIfEnablePIBlkToSetup = TRUE;

	if (szSetupMode == "Single Block")
	{
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
	}
	else if (szSetupMode == "All Blocks")
	{
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;
	}

	if (((szSetupMode == "All Blocks") && (m_ulPhyBlkToSetup == 1))
									|| (szSetupMode == "Single Block"))
	{
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnableTBCancel = TRUE;
	}
	else
	{
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnableTBCancel = FALSE;
	}
	m_bIfEnableTBConfirmLR = FALSE;
	m_bIfEnableDDLowerRightX = FALSE;
	m_bIfEnableDDLowerRightY = FALSE;

	SetJoystickOn(FALSE);
	m_ulJoystickFlag = 0;

	//Values conversion
	lConvertedULX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
	lConvertedULY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);
	lConvertedLRX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
	lConvertedLRY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);

	m_bIfEnableTBSubmit = TRUE;

	//Validation (only apply to Standard Bin Table)
	switch (m_ulMachineType)
	{
		case BT_MACHTYPE_STD:
			if (szSetupMode == "Single Block")
			{
				if (m_oPhyBlkMain.GrabIsSetup(ulBlkId))
				{
					bIsValidLowerRight = m_oPhyBlkMain.ValidateLowerRight(lConvertedULX, lConvertedULY, 
										lConvertedLRX, lConvertedLRY);

					if (! bIsValidLowerRight)
					{
						m_bIfEnableTBSubmit = FALSE;

						SetAlert(m_oPhyBlkMain.GetMsgCode());
					}
				}

				if (m_oPhyBlkMain.GrabIsSetup(ulBlkId))
				{
					bIsValidDimension = m_oPhyBlkMain.CheckOverlappingProblem(ulBlkId, 
						lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY);

					if (!bIsValidDimension)
					{
						m_bIfEnableTBSubmit = FALSE;

						SetAlert(m_oPhyBlkMain.GetMsgCode());
					}
				}
			}
			else if (szSetupMode == "All Blocks")
			{
				if (! ((m_lPhyUpperLeftX == 0) && (m_lPhyUpperLeftY == 0)))
				{
					bIsValidLowerRight = m_oPhyBlkMain.ValidateLowerRight(lConvertedULX, lConvertedULY,
													lConvertedLRX, lConvertedLRY);
					if (!bIsValidLowerRight)
					{
						m_bIfEnableTBSubmit = FALSE;

						SetAlert(m_oPhyBlkMain.GetMsgCode());
					}
				}
			}
		break;
	}

	return 1;
} //end ConfirmPhyBlkSetupLR


LONG CBinTable::SubmitPhyBlkSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	ULONG ulBlkId = 0;
	CString szSetupMode;
	CString szTitle, szContent;
	char *pBuffer;
	LONG lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY;
	BOOL bIsLowerRightValid, bIsSetupValid;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	lConvertedULX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
	lConvertedULY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);
	lConvertedLRX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
	lConvertedLRY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);

	szTitle.LoadString(HMB_BT_SETUP_PHY_BLK);
	szContent.LoadString(HMB_BT_SETUP_PHY_BLK_OK);


	//andrewng //2020-0806
	//Protection to make sure physical area is covered by ErrorMap area
	//		ErrorMapArea > Physical Area >= Logical Area
	if (IsErrMapInUse())
	{
		LONG lULX = ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
		LONG lULY = ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);
		LONG lLRX = ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
		LONG lLRY = ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);

		if (m_lErrMapRefX1 != 0 && m_lErrMapRefY1 != 0)
		{
			//Convert from encoder to um
			LONG lErrMapRefX1 = ConvertXEncoderValueForDisplay(m_lErrMapRefX1);
			LONG lErrMapRefY1 = ConvertYEncoderValueForDisplay(m_lErrMapRefY1);

			if (lULX > lErrMapRefX1)
			{
				CString szErr;
				szErr.Format("Physical UL corner X (%ld) is outside Error Map area X (%ld)! Please decrease physical area size.", 
								lULX, lErrMapRefX1);
				HmiMessage_Red_Yellow(szErr, szTitle);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			if (lULY > lErrMapRefY1)
			{
				CString szErr;
				szErr.Format("Physical UL corner Y (%ld) is outside Error Map area Y (%ld)! Please decrease physical area size.", 
								lULY, lErrMapRefY1);
				HmiMessage_Red_Yellow(szErr, szTitle);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}


			LONG lErrMapRefX2 = lErrMapRefX1 - m_ulErrMapNoOfCol * m_dErrMapPitchX;
			LONG lErrMapRefY2 = lErrMapRefY1 - m_ulErrMapNoOfRow * m_dErrMapPitchY;
			
			if (lLRX < lErrMapRefX2)
			{
				CString szErr;
				szErr.Format("Physical LR corner X (%ld) is outside Error Map area X (%ld = %ld - %lu x %f)! Please decrease physical area size.", 
								lLRX, lErrMapRefX2, lErrMapRefX1, m_ulErrMapNoOfCol, m_dErrMapPitchX);
				HmiMessage_Red_Yellow(szErr, szTitle);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}

			if (lLRY < lErrMapRefY2)
			{
				CString szErr;
				szErr.Format("Physical LR corner Y (%ld) is outside Error Map area Y (%ld = %ld - %lu x %f)! Please decrease physical area size.", 
								lLRY, lErrMapRefY2, lErrMapRefY1, m_ulErrMapNoOfRow, m_dErrMapPitchY);
				HmiMessage_Red_Yellow(szErr, szTitle);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}
	}



	switch (m_ulMachineType)
	{
		case BT_MACHTYPE_STD: //Standard Bin Table
			if (szSetupMode == "Single Block")
			{
				bIsSetupValid = m_oPhyBlkMain.SetupSingleBlk(m_ulNoOfPhyBlk, ulBlkId, 
								lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY);

				if (bIsSetupValid)
				{
					PhyBlksDrawing();

					SetStatusMessage("Physical block setup is completed");
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
								glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

				}
				else
				{
					SetAlert(m_oPhyBlkMain.GetMsgCode());

					//Restore original data from structure
					m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(m_ulPhyBlkToSetup));	
					m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(m_ulPhyBlkToSetup));
					m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(m_ulPhyBlkToSetup));
					m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(m_ulPhyBlkToSetup));
				}
			}
			else if (szSetupMode == "All Blocks")
			{
				bIsSetupValid = m_oPhyBlkMain.SetupAllBlks(m_ulNoOfPhyBlk, 
								ConvertDisplayUnitToFileUnit(m_lPhyBlkPitchX), 
								ConvertDisplayUnitToFileUnit(m_lPhyBlkPitchY), 
								lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY,
								m_lXNegLimit, m_lXPosLimit, m_lYNegLimit, m_lYPosLimit);

				//Hmi variables on screen
				m_ulNoOfPhyBlk = m_oPhyBlkMain.GetNoOfBlk();
				m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(1));
				m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(1));
				m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(1));
				m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(1));

				m_lPhyBlkPitchX = ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GetBlkPitchX());
				m_lPhyBlkPitchY = ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GetBlkPitchY());

				if (!bIsSetupValid) //setup fail!
				{
					SetAlert(m_oPhyBlkMain.GetMsgCode());
				} 
				else //setup OK
				{
					PhyBlksDrawing();
					
					SetStatusMessage("Physical block setup is completed");
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
								glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				}
			}
		break;

		case BT_MACHTYPE_DTABLE:	//dual-table	//v4.16T1
		case BT_MACHTYPE_DBUFFER:	//dual-buffer	//v3.71T4
		case BT_MACHTYPE_DL_DLA:	//Bin Loader
			bIsLowerRightValid = m_oPhyBlkMain.ValidateLowerRight(lConvertedULX, lConvertedULY,
					lConvertedLRX, lConvertedLRY);

			if (szSetupMode == "Single Block")
			{
				if (bIsLowerRightValid)
				{
					m_oPhyBlkMain.SetupSingleBlk_BL(m_ulNoOfPhyBlk, ulBlkId, lConvertedULX, lConvertedULY,
						lConvertedLRX, lConvertedLRY);

					PhyBlksDrawing_BL(ulBlkId);

					SetStatusMessage("Physical block setup is completed");
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
								glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				}
				else
				{
					SetAlert(IDS_BT_LRCHECK2);	
				}
			}
			else if (szSetupMode == "All Blocks")
			{
				if (bIsLowerRightValid)
				{
					m_oPhyBlkMain.SetupAllBlks_BL(m_ulNoOfPhyBlk, lConvertedULX, lConvertedULY,
						lConvertedLRX, lConvertedLRY);

					PhyBlksDrawing_BL(1);

					SetStatusMessage("Physical block setup is completed");
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
								glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				}
				else
				{
					SetAlert(IDS_BT_LRCHECK2);					
				}
			}
		
		break;

	}

    // 3391
    SetGemValue("BT_PhyNoOfBlocks",		m_ulNoOfPhyBlk);
    SetGemValue("BT_PhyBlockToSetup",	ulBlkId);
    // 3392
    SetGemValue("BT_PhyUpLeftX",		m_lPhyUpperLeftX);
    SetGemValue("BT_PhyUpLeftY",		m_lPhyUpperLeftY);
    SetGemValue("BT_PhyLowRightX",		m_lPhyLowerRightX);
    SetGemValue("BT_PhyLowRightY",		m_lPhyLowerRightY);
    // 3393
    SetGemValue("BT_PhyBlockPitchX",	m_lPhyBlkPitchX);
    SetGemValue("BT_PhyBlockPitchY",	m_lPhyBlkPitchY);
    // 7401
    SendEvent(SG_CEID_BT_PHYSETUP,		FALSE);

	GetWafflePadSettings();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

} //end SubmitPhyBlkSetup

LONG CBinTable::SubmitSubBinPhyBlkSetup(IPC_CServiceMessage& svMsg)
{
	DecodeSubBinSetting();
	
	BOOL bReturn = TRUE;
	CString szLog;
	LONG lStartX = 0,lStartY = 0;
	LONG lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY;
	m_lBinCalibX	= -70000*2;
	m_lBinCalibY	= 50000*2;
	szLog.Format("SubBin setup phy block1 - resolution(%f,%f),Pitch(%f,%f),cor(%d,%d),RowCol(%d,%d)(%d,%d),(%d,%d)(%d,%d),(%d,%d)(%d,%d),(%d,%d)(%d,%d)",
		m_dXResolution,m_dYResolution,m_dDiePitchX,m_dDiePitchY,m_lBinCalibX,m_lBinCalibY,
						m_ulSubBinSRowA,m_ulSubBinSColA,m_ulSubBinERowA,m_ulSubBinEColA,m_ulSubBinSRowB,m_ulSubBinSColB,m_ulSubBinERowB,m_ulSubBinEColB,
						m_ulSubBinSRowC,m_ulSubBinSColC,m_ulSubBinERowC,m_ulSubBinEColC,m_ulSubBinSRowD,m_ulSubBinSColD,m_ulSubBinERowD,m_ulSubBinEColD);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	//HmiMessage(szLog);


	for (int p = 1; p <= m_nNoOfSubBlk; p ++)
	{
		for(int q = 1; q <= m_nNoOfSubGrade; q ++)
		{
			for (int k = 1; k <= BT_MAX_BINBLK_NO; k ++)
			{
				if(k == m_nSubGrade[p][q])
				{
					if (q == 1 && m_ulSubBinSRowA != 0 && m_ulSubBinERowA != 0 && m_ulSubBinSColA != 0 && m_ulSubBinEColA != 0)
					{
						m_lPhyUpperLeftX = _round(m_lBinCalibX / m_dXResolution + 31750);
						m_lPhyUpperLeftY = _round(m_lBinCalibY / m_dYResolution + 31750);

						if (!IsWithinTable1Limit(_round(m_lPhyUpperLeftX * m_dXResolution), _round(m_lPhyUpperLeftY * m_dXResolution)))
						{
							szLog.Format("Grade%d in Zone%d UL out of bin table limit",k,q);
							HmiMessage(szLog);
							break;
						}

						lStartX = m_lPhyUpperLeftX;
						lStartY = m_lPhyUpperLeftY;

						m_lPhyLowerRightX = _round(lStartX - (m_ulSubBinERowA - m_ulSubBinSRowA) * m_dDiePitchX);
						m_lPhyLowerRightY = _round(lStartY - (m_ulSubBinEColA - m_ulSubBinSColA) * m_dDiePitchY);

						if (!IsWithinTable1Limit(_round(m_lPhyLowerRightX * m_dXResolution), _round(m_lPhyLowerRightY * m_dXResolution)))
						{
							szLog.Format("Grade%d in Zone%d LR out of bin table limit",k,q);
							HmiMessage(szLog);
							break;
						}
						//m_ulDiePerUserRow = m_ulSubBinERowA - m_ulSubBinSRowA + 1;
						//m_ulDiePerUserCol = m_ulSubBinEColA - m_ulSubBinSColA + 1;//for SetupBinBlk
					}
					else if (q == 2 && m_ulSubBinSRowB != 0 && m_ulSubBinERowB != 0 && m_ulSubBinSColB != 0 && m_ulSubBinEColB != 0)
					{
						m_lPhyUpperLeftX = _round(lStartX - (m_ulSubBinSRowB - m_ulSubBinSRowA) * m_dDiePitchX);
						m_lPhyUpperLeftY = _round(lStartY - (m_ulSubBinSColB - m_ulSubBinSColA) * m_dDiePitchY);
						if (!IsWithinTable1Limit(_round(m_lPhyUpperLeftX * m_dXResolution), _round(m_lPhyUpperLeftY * m_dXResolution)))
						{
							szLog.Format("Grade%d in Zone%d UL out of bin table limit",k,q);
							HmiMessage(szLog);
							break;
						}

						m_lPhyLowerRightX = _round(lStartX - (m_ulSubBinERowB - m_ulSubBinSRowA) * m_dDiePitchX);
						m_lPhyLowerRightY = _round(lStartY - (m_ulSubBinEColB - m_ulSubBinSColA) * m_dDiePitchY);

						if (!IsWithinTable1Limit(_round(m_lPhyLowerRightX * m_dXResolution), _round(m_lPhyLowerRightY * m_dXResolution)))
						{
							szLog.Format("Grade%d in Zone%d LR out of bin table limit",k,q);
							HmiMessage(szLog);
							break;
						}
						//m_ulDiePerUserRow = m_ulSubBinERowB - m_ulSubBinSRowB + 1;
						//m_ulDiePerUserCol = m_ulSubBinEColB - m_ulSubBinSColB + 1;//for SetupBinBlk
					}
					else if (q == 3 && m_ulSubBinSRowC != 0 && m_ulSubBinERowC != 0 && m_ulSubBinSColC != 0 && m_ulSubBinEColC != 0)
					{
						m_lPhyUpperLeftX = _round(lStartX - (m_ulSubBinSRowC - m_ulSubBinSRowA) * m_dDiePitchX);
						m_lPhyUpperLeftY = _round(lStartY - (m_ulSubBinSColC - m_ulSubBinSColA) * m_dDiePitchY);
						if (!IsWithinTable1Limit(_round(m_lPhyUpperLeftX * m_dXResolution), _round(m_lPhyUpperLeftY * m_dXResolution)))
						{
							szLog.Format("Grade%d in Zone%d UL out of bin table limit",k,q);
							HmiMessage(szLog);
							break;
						}

						m_lPhyLowerRightX = _round(lStartX - (m_ulSubBinERowC - m_ulSubBinSRowA) * m_dDiePitchX);
						m_lPhyLowerRightY = _round(lStartY - (m_ulSubBinEColC - m_ulSubBinSColA) * m_dDiePitchY);

						if (!IsWithinTable1Limit(_round(m_lPhyLowerRightX * m_dXResolution), _round(m_lPhyLowerRightY * m_dXResolution)))
						{
							szLog.Format("Grade%d in Zone%d LR out of bin table limit",k,q);
							HmiMessage(szLog);
							break;
						}
						//m_ulDiePerUserRow = m_ulSubBinERowC - m_ulSubBinSRowC + 1;
						//m_ulDiePerUserCol = m_ulSubBinEColC - m_ulSubBinSColC + 1;//for SetupBinBlk
					}
					else if (q == 4 && m_ulSubBinSRowD != 0 && m_ulSubBinERowD != 0 && m_ulSubBinSColD != 0 && m_ulSubBinEColD != 0)
					{
						m_lPhyUpperLeftX = _round(lStartX - (m_ulSubBinSRowD - m_ulSubBinSRowA) * m_dDiePitchX);
						m_lPhyUpperLeftY = _round(lStartY - (m_ulSubBinSColD - m_ulSubBinSColA) * m_dDiePitchY);
						if (!IsWithinTable1Limit(_round(m_lPhyUpperLeftX * m_dXResolution), _round(m_lPhyUpperLeftY * m_dXResolution)))
						{
							szLog.Format("Grade%d in Zone%d UL out of bin table limit",k,q);
							HmiMessage(szLog);
							break;
						}

						m_lPhyLowerRightX = _round(lStartX - (m_ulSubBinERowD - m_ulSubBinSRowA) * m_dDiePitchX);
						m_lPhyLowerRightY = _round(lStartY - (m_ulSubBinEColD - m_ulSubBinSColA) * m_dDiePitchY);

						if (!IsWithinTable1Limit(_round(m_lPhyLowerRightX * m_dXResolution), _round(m_lPhyLowerRightY * m_dXResolution)))
						{
							szLog.Format("Grade%d in Zone%d LR out of bin table limit",k,q);
							HmiMessage(szLog);
							break;
						}
						//m_ulDiePerUserRow = m_ulSubBinERowD - m_ulSubBinSRowD + 1;
						//m_ulDiePerUserCol = m_ulSubBinEColD - m_ulSubBinSColD + 1;//for SetupBinBlk
					}
					else
					{
						HmiMessage("Sub Grade No exceeds 4");
						break;
					}
					lConvertedULX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
					lConvertedULY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);
					lConvertedLRX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
					lConvertedLRY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);
					m_oPhyBlkMain.SetupSingleBlk_BL(BT_MAX_BINBLK_NO, k, lConvertedULX, lConvertedULY,lConvertedLRX, lConvertedLRY);

					PhyBlksDrawing_BL(k);
					szLog.Format("SubBin setup phy block2 - Setup Block %d OK, Zone,%d(1=A,2=B,3=C,4=D),((in um)UL(%d,%d),LR(%d,%d)),((converted)UL(%d,%d),LR(%d,%d))",
						k,q,m_lPhyUpperLeftX,m_lPhyUpperLeftY,m_lPhyLowerRightX,m_lPhyLowerRightY,lConvertedULX,lConvertedULY,lConvertedLRX,lConvertedLRY);
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
					//HmiMessage(szLog);

					m_ulBinBlkToSetup	= k;
					m_ulSourcePhyBlk	= k;
					m_ucGrade			= k;
					m_bUseBlockCornerAsFirstDiePos = TRUE;
					m_bNoReturnTravel			   = TRUE;
					
					//m_bTeachWithPhysicalBlk = TRUE;
					SetupBinBlk("Single Block",FALSE,TRUE);
				}
			}
		}
	}

	SaveBinTableData();
	PhyBlksDrawing();
	HmiMessage("Subbin Setup Completed");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::SubBinReset(IPC_CServiceMessage& svMsg)
{

	m_ulSubBinERowA = 0;
	m_ulSubBinERowB = 0;
	m_ulSubBinERowC = 0;
	m_ulSubBinERowD = 0;

	m_ulSubBinSRowA = 0;
	m_ulSubBinSRowB = 0;
	m_ulSubBinSRowC = 0;
	m_ulSubBinSRowD = 0;

	m_ulSubBinSColA = 0;
	m_ulSubBinSColB = 0;
	m_ulSubBinSColC = 0;
	m_ulSubBinSColD = 0;

	m_ulSubBinEColA = 0;
	m_ulSubBinEColB = 0;
	m_ulSubBinEColC = 0;
	m_ulSubBinEColD = 0;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::SubBinCheckAZone(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!CheckIfOverlapForSubBin(m_ulSubBinSColA,m_ulSubBinSRowA,m_ulSubBinEColA,m_ulSubBinERowA,m_ulSubBinSColB,m_ulSubBinSRowB,m_ulSubBinEColB,m_ulSubBinERowB)||
		!CheckIfOverlapForSubBin(m_ulSubBinSColA,m_ulSubBinSRowA,m_ulSubBinEColA,m_ulSubBinERowA,m_ulSubBinSColC,m_ulSubBinSRowC,m_ulSubBinEColC,m_ulSubBinERowC)||
		!CheckIfOverlapForSubBin(m_ulSubBinSColA,m_ulSubBinSRowA,m_ulSubBinEColA,m_ulSubBinERowA,m_ulSubBinSColD,m_ulSubBinSRowD,m_ulSubBinEColD,m_ulSubBinERowD))
	{
		bReturn = FALSE;
	}
	if (bReturn == FALSE)
	{
		m_ulSubBinSRowA = 0;
		m_ulSubBinSColA = 0;
		m_ulSubBinERowA = 0;
		m_ulSubBinEColA = 0;

	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::SubBinCheckBZone(IPC_CServiceMessage& svMsg)
{
		BOOL bReturn = TRUE;

	if (!CheckIfOverlapForSubBin(m_ulSubBinSColB,m_ulSubBinSRowB,m_ulSubBinEColB,m_ulSubBinERowB,m_ulSubBinSColA,m_ulSubBinSRowA,m_ulSubBinEColA,m_ulSubBinERowA)||
		!CheckIfOverlapForSubBin(m_ulSubBinSColB,m_ulSubBinSRowB,m_ulSubBinEColB,m_ulSubBinERowB,m_ulSubBinSColC,m_ulSubBinSRowC,m_ulSubBinEColC,m_ulSubBinERowC)||
		!CheckIfOverlapForSubBin(m_ulSubBinSColB,m_ulSubBinSRowB,m_ulSubBinEColB,m_ulSubBinERowB,m_ulSubBinSColD,m_ulSubBinSRowD,m_ulSubBinEColD,m_ulSubBinERowD))
	{
		bReturn = FALSE;
	}
	if (bReturn == FALSE)
	{
		m_ulSubBinSRowB = 0;
		m_ulSubBinSColB = 0;
		m_ulSubBinERowB = 0;
		m_ulSubBinEColB = 0;

	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::SubBinCheckCZone(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!CheckIfOverlapForSubBin(m_ulSubBinSColC,m_ulSubBinSRowC,m_ulSubBinEColC,m_ulSubBinERowC,m_ulSubBinSColA,m_ulSubBinSRowA,m_ulSubBinEColA,m_ulSubBinERowA)||
		!CheckIfOverlapForSubBin(m_ulSubBinSColC,m_ulSubBinSRowC,m_ulSubBinEColC,m_ulSubBinERowC,m_ulSubBinSColB,m_ulSubBinSRowB,m_ulSubBinEColB,m_ulSubBinERowB)||
		!CheckIfOverlapForSubBin(m_ulSubBinSColC,m_ulSubBinSRowC,m_ulSubBinEColC,m_ulSubBinERowC,m_ulSubBinSColD,m_ulSubBinSRowD,m_ulSubBinEColD,m_ulSubBinERowD))
	{
		bReturn = FALSE;
	}
	if (bReturn == FALSE)
	{
		m_ulSubBinSRowC = 0;
		m_ulSubBinSColC = 0;
		m_ulSubBinERowC = 0;
		m_ulSubBinEColC = 0;

	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::SubBinCheckDZone(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!CheckIfOverlapForSubBin(m_ulSubBinSColD,m_ulSubBinSRowD,m_ulSubBinEColD,m_ulSubBinERowD,m_ulSubBinSColA,m_ulSubBinSRowA,m_ulSubBinEColA,m_ulSubBinERowA)||
		!CheckIfOverlapForSubBin(m_ulSubBinSColD,m_ulSubBinSRowD,m_ulSubBinEColD,m_ulSubBinERowD,m_ulSubBinSColB,m_ulSubBinSRowB,m_ulSubBinEColB,m_ulSubBinERowB)||
		!CheckIfOverlapForSubBin(m_ulSubBinSColD,m_ulSubBinSRowD,m_ulSubBinEColD,m_ulSubBinERowD,m_ulSubBinSColC,m_ulSubBinSRowC,m_ulSubBinEColC,m_ulSubBinERowC))
	{
		bReturn = FALSE;
	}
	if (bReturn == FALSE)
	{
		m_ulSubBinSRowD = 0;
		m_ulSubBinSColD = 0;
		m_ulSubBinERowD = 0;
		m_ulSubBinEColD = 0;

	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::CancelPhyBlkSetup(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	m_bFirstTimeSetUL = FALSE;
	m_bFirstTimeSetLR = FALSE;

	SetJoystickOn(FALSE);
	m_ulJoystickFlag = 0;

	//Enability of Hmi Controls
	m_bIfEnableTBClrBlkSettings = TRUE;
	m_bIfEnableCBSetupMode = TRUE;
	m_bIfEnablePINoOfBlk = TRUE;  
	m_bIfEnablePIBlkToSetup = TRUE;

	if (m_szSetupMode == "Single Block")
	{
		//m_bIfEnablePIBlkToSetup = TRUE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
	}
	else if (m_szSetupMode == "All Blocks")
	{
		//m_bIfEnablePIBlkToSetup = FALSE;
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;
	}

	if (((m_szSetupMode == "All Blocks") && (m_ulPhyBlkToSetup == 1) 
							|| (m_szSetupMode == "Single Block")))
	{
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnableTBSubmit = TRUE;
		m_bIfEnableTBCancel = TRUE;	
	}
	else
	{
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnableTBSubmit = FALSE;
		m_bIfEnableTBCancel = FALSE;	
	}


	m_bIfEnableTBConfirmUL = FALSE;
	m_bIfEnableTBConfirmLR = FALSE;
	m_bIfEnableDDUpperLeftX = FALSE;
	m_bIfEnableDDUpperLeftY = FALSE;
	m_bIfEnableDDLowerRightX = FALSE;
	m_bIfEnableDDLowerRightY = FALSE;


	//Restore original data values from structure (for screen display)

	m_ulNoOfPhyBlk = m_oPhyBlkMain.GetNoOfBlk();

	m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(ulBlkId));
	m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(ulBlkId));
	m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(ulBlkId));
	m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(ulBlkId));

	m_lPhyBlkPitchX = ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GetBlkPitchX());
	m_lPhyBlkPitchY = ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GetBlkPitchY());

	return 1;
} //end CancelPhyBlkSetup


LONG CBinTable::DisplayPhyBlkData(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		ULONG	ulPhyBlkId;
		ULONG	ulBinBlkId;	
	} BLKDATA;
	BLKDATA stInfo;

	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

	//Enabilities of controls
	if ((m_szSetupMode == "All Blocks") && (stInfo.ulPhyBlkId != 1))
	{
		SetAlert(IDS_BT_EDITBLK1DATAONLY);

		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnableTBSubmit = FALSE;
		m_bIfEnableTBCancel = FALSE;
	}
	else
	{
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
	}

	if (m_szSetupMode == "All Blocks")
	{
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;
	}
	else if (m_szSetupMode == "Single Block")
	{
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
	}

	//Update UL and LR values upon selecting "Block to setup"
	m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(stInfo.ulPhyBlkId));	
	m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(stInfo.ulPhyBlkId));
	m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(stInfo.ulPhyBlkId));
	m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(stInfo.ulPhyBlkId));

	SetJoystickOn(FALSE);

	if (m_bUseDualTablesOption)		//v4.17T5
		HomeTable2();

	MoveXYTo(ConvertFileUnitToXEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftX(stInfo.ulPhyBlkId)),
			 ConvertFileUnitToYEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftY(stInfo.ulPhyBlkId)));

	return 1;
} //end DisplayPhyBlkData


LONG CBinTable::MoveToPhyBlk(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	SetJoystickOn(FALSE);

	if (m_bUseDualTablesOption)		//v4.17T5
		HomeTable2();

	MoveXYTo(ConvertFileUnitToXEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftX(ulBlkId)), 
			 ConvertFileUnitToYEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftY(ulBlkId)));

return 1;
} //end MoveToPhyBlk


LONG CBinTable::CheckIfClrBlkSettingsPossible(IPC_CServiceMessage& svMsg)
{
	if (m_oPhyBlkMain.GetNoOfBlk() > 0)
	{
		m_bIfEnableCBClrBlkSettingsSelection = TRUE;
		
		if (m_szClrBlkSettingsSelection == "Single Block")
			m_bIfEnablePIBlkToClrSettings = TRUE;
		else
			m_bIfEnablePIBlkToClrSettings = FALSE;

		m_bIfEnableTBSubmitClrBlkSettings = TRUE;
	}

	return 1;
} //end CheckIfClrBlkSettingsPossible

////////////////////////////////////////////////////////////////////////////////////////////
//		                	  	      Bin Block Setup		     	                   	  //
////////////////////////////////////////////////////////////////////////////////////////////

LONG CBinTable::BinBlkSetupPreRoutine(IPC_CServiceMessage& svMsg)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (m_oPhyBlkMain.GetNoOfBlk() == 0) //no physical block setup yet
	{
		m_bReply = FALSE; //commands below WON'T be executed
	}
	else
	{
		//Enability of Hmi Controls
		m_bIfEnableTBEditBlkSettings = TRUE;
		m_bIfEnableTBClrBlkSettings = TRUE;

		m_bIfEnableCBSetupMode = TRUE;
		m_bIfEnablePINoOfBlk = TRUE;

		if (m_oBinBlkMain.GetNoOfBlk() > 0)
			m_bIfEnablePIBlkToSetup = TRUE;
		else
			m_bIfEnablePIBlkToSetup = FALSE;

		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnableTBConfirmUL = FALSE;
		m_bIfEnableTBConfirmLR = FALSE;
		m_bIfEnableDDUpperLeftX = FALSE;
		m_bIfEnableDDUpperLeftY = FALSE;
		m_bIfEnableDDLowerRightX = FALSE;
		m_bIfEnableDDLowerRightY = FALSE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnablePIDiePitchX = FALSE;
		m_bIfEnablePIDiePitchY = FALSE;
		m_bIfEnablePIDiePerRow = FALSE;
		m_bIfEnablePIDiePerCol = FALSE;
		m_bIfEnablePIDiePerBlk = FALSE;
		m_bIfEnableChkBAutoAssignGrade = FALSE;
		m_bIfEnablePIGrade = FALSE;
		m_bIfEnableCBWalkPath = FALSE;
		m_bIfEnableFirstRowColSkipPattern = FALSE;
		m_bIfEnableTeachWithPhysicalBlk = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;		
		m_bIfEnableTemplateSetup = FALSE;
		m_bIfEnableCentralizedBondArea = FALSE;
		m_bIfEnableWafflePad = FALSE;
		m_bIfEnableWafflePadCheckBox = FALSE;
		m_bIfEnablePadPitchSetX = FALSE;
		m_bIfEnablePadPitchSetY = FALSE;
		m_bIfEnablePadPitchResetX = FALSE;
		m_bIfEnablePadPitchResetY = FALSE;
		m_bIfEnablePadDimX = FALSE;
		m_bIfEnablePadDimY = FALSE;
		m_bIfUseBlockCornerAsFirstDiePos = FALSE;
		m_bIfEnableTBSubmit = FALSE;
		m_bIfEnableTBCancel = FALSE;	

		//Grab values for Screen Display (Hmi Reg Variables)
		m_szClrBlkSettingsSelection = "Single Block";
		m_ulBinBlkToClrSettings = 1;

		if (m_oBinBlkMain.GetNoOfBlk() > 0)
		{
			m_bIfEnableCBClrBlkSettingsSelection = TRUE;
			m_bIfEnablePIBlkToClrSettings = TRUE;
			m_bIfEnableTBSubmitClrBlkSettings = TRUE;
		}
		else //there is no bin block setup yet
		{
			m_bIfEnableCBClrBlkSettingsSelection = FALSE;
			m_bIfEnablePIBlkToClrSettings = FALSE;
			m_bIfEnableTBSubmitClrBlkSettings = FALSE;
		}

		//Initialization of variables
		m_bFirstTimeSetUL = TRUE; 
		m_bFirstTimeSetLR = TRUE;

		m_szSetupMode = "Single Block";
		m_ulNoOfBinBlk = m_oBinBlkMain.GetNoOfBlk();
		m_ulBinBlkToSetup = 0;
		m_lBinUpperLeftX = 0;
		m_lBinUpperLeftY = 0;
		m_lBinLowerRightX = 0;
		m_lBinLowerRightY = 0;
		m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
		m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());
		m_ucGrade = 1;
		m_dDiePitchX = 500;
		m_dDiePitchY = 500;
		m_bUsePt5UmInDiePitchX = FALSE;
		m_bUsePt5UmInDiePitchY = FALSE;
		m_dBondAreaOffsetXInUm	= 0;
		m_dBondAreaOffsetYInUm	= 0;
		m_dThermalDeltaPitchXInUm	= 0;	//v4.59A22	//David Ma
		m_dThermalDeltaPitchYInUm	= 0;	//v4.59A22	//David Ma
		m_ulWalkPath = 0;
		m_ulDiePerBlk = 0;
		m_ulDiePerRow = 0;
		m_ulDiePerCol = 0;
		m_ulDiePerUserRow = 0;
		m_ulDiePerUserCol = 0;
		m_ulSkipUnit = 0;	
		m_ulMaxUnit = 0;
		m_ulSourcePhyBlk = 1;
		m_bEnableFirstRowColSkipPattern = FALSE;
		m_ulFirstRowColSkipUnit	= 0;
		m_bByPassBinMap = FALSE;

		//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
		m_bEnableCircularBondArea	= FALSE;
		m_lCirAreaCenterX			= 0;
		m_lCirAreaCenterY			= 0;
		m_dCirAreaRadius			= 0;

		m_bTeachWithPhysicalBlk = FALSE;
		m_bCentralizedBondArea = FALSE;
		
		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//m_bTeachWithPhysicalBlk = TRUE;
			m_bCentralizedBondArea	= TRUE;
		}
		m_lCentralizedUpperLeftX = 0;
		m_lCentralizedUpperLeftY = 0;
		
		m_bEnableWafflePad = FALSE;
		m_lWafflePadDistX = 0;
		m_lWafflePadDistY = 0;
		m_lWafflePadSizeX = 0;
		m_lWafflePadSizeY = 0;


		m_bUseBlockCornerAsFirstDiePos = FALSE;
		m_bNoReturnTravel = FALSE;			//v4.42T1

		switch (m_ulMachineType)
		{
			case BT_MACHTYPE_STD:
				BinBlksDrawing();
				break;

			case BT_MACHTYPE_DTABLE:		//v4.16
			case BT_MACHTYPE_DBUFFER:		//v3.71T4
			case BT_MACHTYPE_DL_DLA:
				BinBlksDrawing_BL(1);
				break;
		}
		WriteGradeLegend();
		LoadGradeRankID();
		m_bReply = TRUE;
	}

	return 1;
} //end BinBlkSetupPreRoutine


LONG CBinTable::IfProceedWithCommandsBelow(IPC_CServiceMessage& svMsg)
{
	if (!m_bReply)
	{
		SetAlert(IDS_BT_ENTERBINBLKPAGECHK);
	}

	svMsg.InitMessage(sizeof(BOOL), &m_bReply);

	return 1;
} //end IfProceedWithCommandsBelow


LONG CBinTable::CheckIfNoOfBinBlksIsValid(IPC_CServiceMessage& svMsg)
{
	ULONG ulNoOfBlk;
	CString szSetupMode;
	char *pBuffer;
	BOOL bFcnReply;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulNoOfBlk, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	m_bIfEnableTBSetUL = FALSE;
	m_bIfEnableTBSetLR = FALSE;
	m_bIfEnableTBSubmit = FALSE;
	m_bIfEnableTBCancel = TRUE;

	if (ulNoOfBlk == 0)
	{
		m_bIfEnablePIBlkToSetup = FALSE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;
		m_bIfEnableTemplateSetup = FALSE;

		SetAlert(IDS_BT_NOOFBLKSCHK);

		bFcnReply = FALSE;
	}
	else //no of blks > 0
	{
		m_bIfEnablePIBlkToSetup = TRUE;
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnableTBSubmit = TRUE;

		if (m_ulBinBlkToSetup == 0)
		{
			m_ulBinBlkToSetup = 1;

			//Display Block 1 Data
			m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(1));
			m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(1));
			m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(1));
			m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(1));

			//andrew789
			//m_ulDiePerRow = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabNoOfDiePerRow(1));
			//m_ulDiePerCol = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabNoOfDiePerCol(1));
			//m_ulDiePerBlk = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabNoOfDiePerBlk(1));
			m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(1);
			m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(1);
			m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(1);
			m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(1);	
			m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(1);	

			m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(1));
			m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(1));
			m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(1);
			m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(1);
			m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(1);
			m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(1);
			m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(1);	//v4.59A22	//David Ma
			m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(1);	//v4.59A22	//David Ma
			m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(1);


			m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(1));

			m_ucGrade = m_oBinBlkMain.GrabGrade(1);
		}

		if (szSetupMode == "All Blocks")
		{
			m_ulBinBlkToSetup = 1;

			m_bIfEnablePIBlkPitchX = TRUE;
			m_bIfEnablePIBlkPitchY = TRUE;
			m_bIfEnablePIBlkToSetup = TRUE;
			m_bIfEnablePIDiePerRow = TRUE;
	        m_bIfEnablePIDiePerCol = TRUE;
			m_bIfEnablePIDiePerBlk = TRUE;
			m_bIfEnablePIDiePitchX = TRUE;
			m_bIfEnablePIDiePitchY = TRUE;
			m_bIfEnableCBWalkPath = TRUE;
			m_bIfEnablePIGrade = FALSE;
			m_bIfEnableChkBAutoAssignGrade = TRUE;
			m_bIfEnableFirstRowColSkipPattern = TRUE;
			m_bIfEnableTeachWithPhysicalBlk = TRUE;
			m_bIfEnable2DBarcodeOutput = TRUE;	
			m_bIfEnableCentralizedBondArea = TRUE;
			m_bIfEnableWafflePad = TRUE;
			m_bIfEnableWafflePadCheckBox = TRUE;
			m_bIfEnablePadPitchSetX = TRUE;
			m_bIfEnablePadPitchSetY = TRUE;
			m_bIfEnablePadPitchResetX = TRUE;
			m_bIfEnablePadPitchResetY = TRUE;
			m_bIfEnablePadDimX = TRUE;
			m_bIfEnablePadDimY = TRUE;
			m_bIfUseBlockCornerAsFirstDiePos = TRUE;
			m_bIfEnableTemplateSetup = FALSE;
			m_bIfEnableTBSubmit = TRUE;

			//Display Block 1 Data
			m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(1));
			m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(1));
			m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(1));
			m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(1));

			m_ulDiePerRow = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabNoOfDiePerRow(1));
			m_ulDiePerCol = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabNoOfDiePerCol(1));
			m_ulDiePerBlk = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabNoOfDiePerBlk(1));
			m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(1);	
			m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(1);	

			m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(1));
			m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(1));
			m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(1);
			m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(1);
			m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(1);
			m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(1);
			m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(1);	//v4.59A22	//David Ma
			m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(1);	//v4.59A22	//David Ma
			m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(1);

			m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(1));

			m_ucGrade = m_oBinBlkMain.GrabGrade(1);
		}
		else if (szSetupMode == "Single Block")
		{
			m_bIfEnablePIBlkPitchX = FALSE;
			m_bIfEnablePIBlkPitchY = FALSE;		
			m_bIfEnableChkBAutoAssignGrade = FALSE;
			m_bIfEnablePIGrade = TRUE;
			m_bIfEnable2DBarcodeOutput = FALSE;
			m_bIfEnableTemplateSetup = TRUE;
		}

		bFcnReply = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bFcnReply);

	return 1;
} //end CheckIfNoOfBinBlksIsValid


LONG CBinTable::CheckIfBinBlkToSetupIsValid(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;
	BOOL bFcnReply;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	m_bIfEnableTBSetUL = FALSE;
	m_bIfEnableTBSetLR = FALSE;
	//m_bIfEnablePIBlkPitchX = FALSE;
	//m_bIfEnablePIBlkPitchY = FALSE;
	m_bIfEnablePIDiePitchX = FALSE;
	m_bIfEnablePIDiePitchY = FALSE;
	m_bIfEnablePIDiePerRow = FALSE;
	m_bIfEnablePIDiePerCol = FALSE;
	m_bIfEnablePIDiePerBlk = FALSE;	
	m_bIfEnablePIGrade = FALSE;
	m_bIfEnableCBWalkPath = FALSE;
	m_bIfEnableChkBAutoAssignGrade = FALSE;
	m_bIfEnableFirstRowColSkipPattern = FALSE;
	m_bIfEnableTeachWithPhysicalBlk = FALSE;
	m_bIfEnable2DBarcodeOutput = FALSE;
	m_bIfEnableTemplateSetup = FALSE;
	m_bIfEnableCentralizedBondArea = FALSE;
	m_bIfEnableWafflePadCheckBox = FALSE;
	m_bIfEnablePadPitchSetX = FALSE;
	m_bIfEnablePadPitchSetY = FALSE;
	m_bIfEnablePadPitchResetX = FALSE;
	m_bIfEnablePadPitchResetY = FALSE;
	m_bIfEnablePadDimX = FALSE;
	m_bIfEnablePadDimY = FALSE;
	m_bIfEnableWafflePad = FALSE;
	m_bIfUseBlockCornerAsFirstDiePos = FALSE;
	m_bIfEnableTBSubmit = FALSE;
	m_bIfEnableTBCancel = FALSE;

	if (ulBlkId == 0)
	{
		SetAlert(IDS_BT_BLKTOSETUPCHK);

		bFcnReply = FALSE;
	}
	else
	{
		m_bIfEnableTBSubmit = TRUE;
		m_bIfEnableTBCancel = TRUE;
		bFcnReply = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bFcnReply);

	return 1;
} //end CheckIfBinBlkToSetupIsValid

LONG CBinTable::CheckIfBlockHasDie(IPC_CServiceMessage& svMsg)
{
	UCHAR ulBlkId;
	BOOL bFcnReply = FALSE;

	svMsg.GetMsg(sizeof(UCHAR), &ulBlkId);

	if( (ulBlkId>0) && (ulBlkId<MS_MAX_BIN+1) )
	{
		if( m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkId)>0 ) // bonding in process
		{
			bFcnReply = TRUE;
		}
		if( m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkId)>0 )
		{
			bFcnReply = TRUE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bFcnReply);

	return 1;
}

LONG CBinTable::CheckIfBondingInProcess(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;
	BOOL bFcnReply;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	if ((ulBlkId > 0) && (ulBlkId < MS_MAX_BIN+1))
	{
		if (m_szSetupMode == "Single Block")
		{
			m_bIfEnableChkBAutoAssignGrade = FALSE;
			m_bIfEnablePIGrade = TRUE;
		}
		else if (m_szSetupMode == "All Blocks")
		{
			m_bIfEnableChkBAutoAssignGrade = TRUE;
			m_bIfEnablePIGrade = FALSE;
		}

		if (m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkId) > 0) //bonding in process
		{
			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnablePIGrade = FALSE;
			m_bIfEnablePIDiePitchX = FALSE;
			m_bIfEnablePIDiePitchY = FALSE;
			m_bIfEnablePIDiePerRow = FALSE;
			m_bIfEnablePIDiePerCol = FALSE;
			m_bIfEnablePIDiePerBlk = FALSE;
			m_bIfEnableChkBAutoAssignGrade = FALSE;
			m_bIfEnablePIGrade = FALSE;
			m_bIfEnableCBWalkPath = FALSE;
			m_bIfEnableFirstRowColSkipPattern = FALSE;
			m_bIfEnableTeachWithPhysicalBlk = FALSE;
			m_bIfEnable2DBarcodeOutput = FALSE;
			m_bIfEnableTemplateSetup = FALSE;
			m_bIfEnableCentralizedBondArea = FALSE;
			m_bIfEnableWafflePad = FALSE;
			m_bIfEnableWafflePadCheckBox = FALSE;
			m_bIfEnablePadPitchSetX = FALSE;
			m_bIfEnablePadPitchSetY = FALSE;
			m_bIfEnablePadPitchResetX = FALSE;
			m_bIfEnablePadPitchResetY = FALSE;
			m_bIfEnablePadDimX = FALSE;
			m_bIfEnablePadDimY = FALSE;
			m_bIfUseBlockCornerAsFirstDiePos = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel = FALSE;

			SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);

			bFcnReply = FALSE;
		}
		else //bonding NOT in process
		{
			m_bIfEnableTBSetUL = TRUE;
			m_bIfEnableTBSetLR = TRUE;
			m_bIfEnablePIGrade = TRUE;
			m_bIfEnablePIDiePitchX = TRUE;
			m_bIfEnablePIDiePitchY = TRUE;
			m_bIfEnablePIDiePerRow = TRUE;
			m_bIfEnablePIDiePerCol = TRUE;
			m_bIfEnablePIDiePerBlk = TRUE;
			m_bIfEnablePIGrade = TRUE;
			m_bIfEnableCBWalkPath = TRUE;
			m_bIfEnableFirstRowColSkipPattern = TRUE;
			m_bIfEnableTeachWithPhysicalBlk = TRUE;
			m_bIfEnable2DBarcodeOutput = TRUE;
			m_bIfEnableTemplateSetup = TRUE;
			m_bIfEnableCentralizedBondArea = TRUE;
			m_bIfEnableWafflePad = TRUE;
			m_bIfEnableWafflePadCheckBox = TRUE;
			m_bIfEnablePadPitchSetX = TRUE;
			m_bIfEnablePadPitchSetY = TRUE;
			m_bIfEnablePadPitchResetX = TRUE;
			m_bIfEnablePadPitchResetY = TRUE;
			m_bIfEnablePadDimX = TRUE;
			m_bIfEnablePadDimY = TRUE;
			m_bIfUseBlockCornerAsFirstDiePos = TRUE;
			m_bIfEnableTBSubmit = TRUE;
			m_bIfEnableTBCancel = TRUE;

			bFcnReply = TRUE;
		}
		
		svMsg.InitMessage(sizeof(BOOL), &bFcnReply);
	}

	return 1;
} //end CheckIfBondingInProcess


LONG CBinTable::SetBinBlkSetupUL(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	m_bIfEnableTBClrBlkSettings = FALSE;
	m_bIfEnableCBSetupMode = FALSE;
	m_bIfEnablePINoOfBlk = FALSE;
	m_bIfEnablePIBlkToSetup = FALSE;
	m_bIfEnableTBConfirmUL = TRUE;
	m_bIfEnableTBSetLR = FALSE;
	m_bIfEnableDDUpperLeftX = TRUE;
	m_bIfEnableDDUpperLeftY = TRUE;
	m_bIfEnablePIBlkPitchX = FALSE;
	m_bIfEnablePIBlkPitchY = FALSE;
	m_bIfEnablePIDiePitchX = FALSE;
	m_bIfEnablePIDiePitchY = FALSE;
	m_bIfEnablePIDiePerRow = FALSE;
	m_bIfEnablePIDiePerCol = FALSE;
	m_bIfEnablePIDiePerBlk = FALSE;
	m_bIfEnableChkBAutoAssignGrade = FALSE;
	m_bIfEnablePIGrade = FALSE;
	m_bIfEnableCBWalkPath = FALSE;
	m_bIfEnableFirstRowColSkipPattern = FALSE;
	m_bIfEnableTeachWithPhysicalBlk = FALSE;
	m_bIfEnable2DBarcodeOutput = FALSE;
	m_bIfEnableTemplateSetup = FALSE;
	m_bIfEnableCentralizedBondArea = FALSE;
	m_bIfEnableWafflePad = FALSE;
	m_bIfEnableWafflePadCheckBox = FALSE;
	m_bIfEnablePadPitchSetX = FALSE;
	m_bIfEnablePadPitchSetY = FALSE;
	m_bIfEnablePadPitchResetX = FALSE;
	m_bIfEnablePadPitchResetY = FALSE;
	m_bIfEnablePadDimX = FALSE;
	m_bIfEnablePadDimY = FALSE;
	m_bIfUseBlockCornerAsFirstDiePos = FALSE;
	
	m_bIfEnableTBSubmit = FALSE;
	m_bIfEnableTBCancel = TRUE;

	LONG lXInFileUnit, lYInFileUnit;

	SetJoystickOn(FALSE);

	if (m_bUseDualTablesOption)		//v4.17T5
	{
		HomeTable2();		
	}

	//Move to UL position
	if (m_bFirstTimeSetUL) //first time to set UL
	{
		if (m_oBinBlkMain.GrabIsSetup(ulBlkId)) //load structure default values
		{
			MoveXYTo(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftX(ulBlkId)), 
					 ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftY(ulBlkId)));
		}
		else //stay at lower right position
		{
			lXInFileUnit = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightX);
			lYInFileUnit = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightY);

			MoveXYTo(ConvertFileUnitToXEncoderValue(lXInFileUnit),
					 ConvertFileUnitToYEncoderValue(lYInFileUnit));
		}
	}
	else	//load previous values for minor adjustment
	{
		lXInFileUnit = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftX);
		lYInFileUnit = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftY);

		MoveXYTo(ConvertFileUnitToXEncoderValue(lXInFileUnit), 
				 ConvertFileUnitToYEncoderValue(lYInFileUnit));
	}

	SetJoystickOn(TRUE);

	m_ulJoystickFlag = 3; //indicate now is setting Upper Left of Bin Block

	return 1;
} //end SetBinBlkSetupUL


LONG CBinTable::ConfirmBinBlkSetupUL(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;
	LONG lIsValidUL;
	LONG lBinBlkULX, lBinBlkULY;
	BOOL bIsValidUL;
	LONG lPhyBlk1ULX, lPhyBlk1ULY, lPhyBlk1LRX, lPhyBlk1LRY;
	CString szSetupMode;
	char *pBuffer;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	//Enability of controls
	m_bIfEnableTBClrBlkSettings = TRUE;
	m_bIfEnableCBSetupMode = TRUE;
	m_bIfEnablePINoOfBlk = TRUE;

	if (((szSetupMode == "All Blocks") && (m_ulBinBlkToSetup == 1))
				|| (szSetupMode == "Single Block"))
	{
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnablePIDiePitchX = TRUE;
		m_bIfEnablePIDiePitchY = TRUE;
		m_bIfEnablePIDiePerRow = TRUE;
		m_bIfEnablePIDiePerCol = TRUE;
		m_bIfEnablePIDiePerBlk = TRUE;
		m_bIfEnableFirstRowColSkipPattern = TRUE;
		m_bIfEnableTeachWithPhysicalBlk = TRUE;
		m_bIfEnable2DBarcodeOutput = TRUE;
		m_bIfEnableTemplateSetup = TRUE;
		m_bIfEnableCentralizedBondArea = TRUE;
		m_bIfEnableWafflePadCheckBox = TRUE;
		m_bIfEnablePadPitchSetX = TRUE;
		m_bIfEnablePadPitchSetY = TRUE;
		m_bIfEnablePadPitchResetX = TRUE;
		m_bIfEnablePadPitchResetY = TRUE;
		m_bIfEnablePadDimX = TRUE;
		m_bIfEnablePadDimY = TRUE;

		m_bIfEnableWafflePad = TRUE;
		m_bIfUseBlockCornerAsFirstDiePos = TRUE;
	}
	else
	{
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnablePIDiePitchX = FALSE;
		m_bIfEnablePIDiePitchY = FALSE;
		m_bIfEnablePIDiePerRow = FALSE;
		m_bIfEnablePIDiePerCol = FALSE;
		m_bIfEnablePIDiePerBlk = FALSE;
		m_bIfEnableFirstRowColSkipPattern = FALSE;
		m_bIfEnableTeachWithPhysicalBlk = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;	
		m_bIfEnableTemplateSetup = FALSE;
		m_bIfEnableCentralizedBondArea = FALSE;
		m_bIfEnableWafflePad = FALSE;
		m_bIfEnableWafflePadCheckBox = FALSE;
		m_bIfEnablePadPitchSetX = FALSE;
		m_bIfEnablePadPitchSetY = FALSE;
		m_bIfEnablePadPitchResetX = FALSE;
		m_bIfEnablePadPitchResetY = FALSE;
		m_bIfEnablePadDimX = FALSE;
		m_bIfEnablePadDimY = FALSE;

		m_bIfUseBlockCornerAsFirstDiePos = FALSE;
	}

	m_bIfEnableTBConfirmUL = FALSE;
	m_bIfEnableDDUpperLeftX = FALSE;
	m_bIfEnableDDUpperLeftY = FALSE;
	m_bIfEnablePIBlkToSetup = TRUE;

	if (szSetupMode == "All Blocks")
	{
		m_bIfEnableChkBAutoAssignGrade = TRUE;
		m_bIfEnablePIGrade = FALSE;
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;
		m_bIfEnable2DBarcodeOutput = TRUE;
		m_bIfEnableTemplateSetup = FALSE;
	}
	else if (szSetupMode == "Single Block")
	{
		m_bIfEnableChkBAutoAssignGrade = FALSE;
		m_bIfEnablePIGrade = TRUE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;
		m_bIfEnableTemplateSetup = TRUE;
	}

	m_bIfEnableCBWalkPath = TRUE;
	m_bIfEnableTBCancel = TRUE;

	//Validation below
	SetJoystickOn(FALSE); 
	m_ulJoystickFlag = 0;
	lBinBlkULX = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftX);
	lBinBlkULY = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftY);

	if (szSetupMode == "All Blocks")
	{
		lPhyBlk1ULX = m_oPhyBlkMain.GrabBlkUpperLeftX(1);
		lPhyBlk1ULY = m_oPhyBlkMain.GrabBlkUpperLeftY(1);
		lPhyBlk1LRX = m_oPhyBlkMain.GrabBlkLowerRightX(1);
		lPhyBlk1LRY = m_oPhyBlkMain.GrabBlkLowerRightY(1);

		bIsValidUL = m_oBinBlkMain.ValidateAllSetupUL(lPhyBlk1ULX, lPhyBlk1ULY, lPhyBlk1LRX, lPhyBlk1LRY, 
												lBinBlkULX, lBinBlkULY);

		m_bIfEnableTBSubmit = TRUE;

		if (! bIsValidUL)
		{
			m_bIfEnableTBSubmit = FALSE;

			//Error: Upper Left is not on Physical Block 1! Please re-teach.
			SetAlert(IDS_BT_ULCHECK2);
		}
	}
	else if (szSetupMode == "Single Block")
	{
		switch (m_ulMachineType)
		{
			case BT_MACHTYPE_STD:	//Standard Bin Table
				lIsValidUL = m_oBinBlkMain.ValidateSingleSetupUL(&m_oPhyBlkMain, ulBlkId,
																lBinBlkULX, lBinBlkULY);

				if (lIsValidUL > 0)
				{
					m_bIfEnableTBSubmit = TRUE;

					m_ulSourcePhyBlk = lIsValidUL;
				}
				else if (lIsValidUL == -1)
				{
					m_bIfEnableTBSubmit = FALSE;

					//Error: Upper Left is not on any physical block! Please re-teach.
					SetAlert(IDS_BT_ULCHECK3);
				}
				else if (lIsValidUL == -2)
				{
					m_bIfEnableTBSubmit = FALSE;

					//Error: Upper Left overlaps with existing bin block! Please re-teach.
					SetAlert(IDS_BT_ULCHECK1);
				}
			break;

			case BT_MACHTYPE_DTABLE:		//dual-table	//v4.16T1
			case BT_MACHTYPE_DBUFFER:		//dual-buffer	//v3.71T4
			case BT_MACHTYPE_DL_DLA:		//Bin Loader
				lIsValidUL = m_oBinBlkMain.ValidateSingleSetupUL_BL(&m_oPhyBlkMain, ulBlkId,
								lBinBlkULX, lBinBlkULY, m_ulSourcePhyBlk);

				if (lIsValidUL == 1)
				{
					m_bIfEnableTBSubmit = TRUE;
				}
				else
				{
					m_bIfEnableTBSubmit = FALSE;

					if (lIsValidUL == -1)
					{
						//HmiMessage("Error: Inputted upper left is not on the indicated source physical block!");
						SetAlert(IDS_BT_ULCHECK4);
					}
					else if (lIsValidUL == -2)
					{
						//HmiMessage("Error: Inputted upper left overlaps with existing bin block!");
						SetAlert(IDS_BT_ULCHECK5);
					}
				}

			break;
		}
	}

	return 1;
} //end ConfirmBinBlkSetupUL

LONG CBinTable::SetBinBlkSetupLR(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	m_bIfEnableTBClrBlkSettings = FALSE;
	m_bIfEnableCBSetupMode = FALSE;
	m_bIfEnablePINoOfBlk = FALSE;
	m_bIfEnablePIBlkToSetup = FALSE;
	m_bIfEnableTBConfirmLR = TRUE;
	m_bIfEnableTBSetUL = FALSE;
	m_bIfEnableDDLowerRightX = TRUE;
	m_bIfEnableDDLowerRightY = TRUE;
	m_bIfEnablePIBlkPitchX = FALSE;
	m_bIfEnablePIBlkPitchY = FALSE;
	m_bIfEnablePIDiePitchX = FALSE;
	m_bIfEnablePIDiePitchY = FALSE;
	m_bIfEnablePIDiePerRow = FALSE;
	m_bIfEnablePIDiePerCol = FALSE;
	m_bIfEnablePIDiePerBlk = FALSE;
	m_bIfEnableChkBAutoAssignGrade = FALSE;
	m_bIfEnablePIGrade = FALSE;
	m_bIfEnableCBWalkPath = FALSE;
	m_bIfEnableFirstRowColSkipPattern = FALSE;
	m_bIfEnableTeachWithPhysicalBlk = FALSE;
	m_bIfEnable2DBarcodeOutput = FALSE;
	m_bIfEnableTemplateSetup = FALSE;
	m_bIfEnableCentralizedBondArea = FALSE;
	m_bIfEnableWafflePad = FALSE;
	m_bIfEnableWafflePadCheckBox = FALSE;
	m_bIfEnablePadPitchSetX = FALSE;
	m_bIfEnablePadPitchSetY = FALSE;
	m_bIfEnablePadPitchResetX = FALSE;
	m_bIfEnablePadPitchResetY = FALSE;
	m_bIfEnablePadDimX = FALSE;
	m_bIfEnablePadDimY = FALSE;
	m_bIfUseBlockCornerAsFirstDiePos = FALSE;
	
	m_bIfEnableTBSubmit = FALSE;
	m_bIfEnableTBCancel = TRUE;

	LONG lXInFileUnit, lYInFileUnit;

	SetJoystickOn(FALSE);

	if (m_bUseDualTablesOption)		//v4.17T5
	{
		HomeTable2();		
	}

	//Move to LR position
	if (m_bFirstTimeSetLR)	//first time to set LR
	{
		if (m_oBinBlkMain.GrabIsSetup(ulBlkId)) //load structure default values
		{
			MoveXYTo(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkLowerRightX(ulBlkId)), 
					 ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkLowerRightY(ulBlkId)));
		}
		else //stay at upper left position
		{
			lXInFileUnit = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftX);
			lYInFileUnit = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftY);

			MoveXYTo(ConvertFileUnitToXEncoderValue(lXInFileUnit),
					 ConvertFileUnitToYEncoderValue(lYInFileUnit));
		}
	}
	else //stay at previous position for minor adjustment
	{
		lXInFileUnit = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightX);
		lYInFileUnit = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightY);

		MoveXYTo(ConvertFileUnitToXEncoderValue(lXInFileUnit), 
				 ConvertFileUnitToYEncoderValue(lYInFileUnit));
	}

	SetJoystickOn(TRUE);

	m_ulJoystickFlag = 4; //indicate now is setting Lower Right of Bin Block

	return 1;
} //end SetBinBlkSetupLR


LONG CBinTable::ConfirmBinBlkSetupLR(IPC_CServiceMessage& svMsg)
{
	LONG lIsValidUL, lIsValidLR;
	LONG lPhyBlk1ULX, lPhyBlk1ULY, lPhyBlk1LRX, lPhyBlk1LRY;
	LONG lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY;
	ULONG ulBlkId;
	CString szSetupMode;
	char *pBuffer;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	//Enability of controls
	m_bIfEnableTBClrBlkSettings = TRUE;
	m_bIfEnableCBSetupMode = TRUE;
	m_bIfEnablePINoOfBlk = TRUE;

	if (((szSetupMode == "All Blocks") && (m_ulBinBlkToSetup == 1))
		|| (szSetupMode == "Single Block"))
	{
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnablePIDiePitchX = TRUE;
		m_bIfEnablePIDiePitchY = TRUE;
		m_bIfEnablePIDiePerRow = TRUE;
		m_bIfEnablePIDiePerCol = TRUE;
		m_bIfEnablePIDiePerBlk = TRUE;
		m_bIfEnableFirstRowColSkipPattern = TRUE;
		m_bIfEnableTeachWithPhysicalBlk = TRUE;
		m_bIfEnable2DBarcodeOutput = TRUE;
		m_bIfEnableTemplateSetup = TRUE;
		m_bIfEnableCentralizedBondArea = TRUE;
		m_bIfEnableWafflePadCheckBox = TRUE;
		m_bIfEnablePadPitchSetX = TRUE;
		m_bIfEnablePadPitchSetY = TRUE;
		m_bIfEnablePadPitchResetX = TRUE;
		m_bIfEnablePadPitchResetY = TRUE;
		m_bIfEnablePadDimX = TRUE;
		m_bIfEnablePadDimY = TRUE;
		m_bIfEnableWafflePad = TRUE;
		m_bIfUseBlockCornerAsFirstDiePos = TRUE;
	}
	else
	{
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnablePIDiePitchX = FALSE;
		m_bIfEnablePIDiePitchY = FALSE;
		m_bIfEnablePIDiePerRow = FALSE;
		m_bIfEnablePIDiePerCol = FALSE;
		m_bIfEnablePIDiePerBlk = FALSE;
		m_bIfEnableFirstRowColSkipPattern = FALSE;
		m_bIfEnableTeachWithPhysicalBlk = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;		
		m_bIfEnableTemplateSetup = FALSE;
		m_bIfEnableCentralizedBondArea = FALSE;
		m_bIfEnableWafflePad = FALSE;
		m_bIfEnableWafflePadCheckBox = FALSE;
		m_bIfEnablePadPitchSetX = FALSE;
		m_bIfEnablePadPitchSetY = FALSE;
		m_bIfEnablePadPitchResetX = FALSE;
		m_bIfEnablePadPitchResetY = FALSE;
		m_bIfEnablePadDimX = FALSE;
		m_bIfEnablePadDimY = FALSE;
		m_bIfUseBlockCornerAsFirstDiePos = FALSE;
	}

	m_bIfEnableTBConfirmLR = FALSE;
	m_bIfEnableDDLowerRightX = FALSE;
	m_bIfEnableDDLowerRightY = FALSE;
	m_bIfEnablePIBlkToSetup = TRUE;

	if (szSetupMode == "All Blocks")
	{
		m_bIfEnableChkBAutoAssignGrade = TRUE;
		m_bIfEnablePIGrade = FALSE;
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;
		m_bIfEnable2DBarcodeOutput = TRUE;
		m_bIfEnableTemplateSetup = FALSE;

	}
	else if (szSetupMode == "Single Block")
	{
		m_bIfEnableChkBAutoAssignGrade = FALSE;
		m_bIfEnablePIGrade = TRUE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;
		m_bIfEnableTemplateSetup = TRUE;
	}

	m_bIfEnableCBWalkPath = TRUE;
	m_bIfEnableTBCancel = TRUE;

	//Validation below
	SetJoystickOn(FALSE); 
	m_ulJoystickFlag = 0;

   	lPhyBlk1ULX = m_oPhyBlkMain.GrabBlkUpperLeftX(1);
	lPhyBlk1ULY = m_oPhyBlkMain.GrabBlkUpperLeftY(1);
	lPhyBlk1LRX = m_oPhyBlkMain.GrabBlkLowerRightX(1);
	lPhyBlk1LRY = m_oPhyBlkMain.GrabBlkLowerRightY(1);

	lBinBlkULX = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftX);
	lBinBlkULY = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftY);
	lBinBlkLRX = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightX);
	lBinBlkLRY = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightY);

	m_bIfEnableTBSubmit = TRUE;

	if (szSetupMode == "All Blocks")
	{
		if (! (m_lBinUpperLeftX == 0) && (m_lBinUpperLeftY == 0))
		{
			lIsValidLR = m_oBinBlkMain.ValidateAllSetupLR(lPhyBlk1ULX, lPhyBlk1ULY, lPhyBlk1LRX, lPhyBlk1LRY, 
												lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY);

			if (lIsValidLR == 1)
			{
				m_bIfEnableTBSubmit = FALSE;

				//Error: Lower Right is not on Physical Block 1
				SetAlert(IDS_BT_LRCHECK3);
			}
			else if (lIsValidLR == 2)
			{
				m_bIfEnableTBSubmit = FALSE;

				//Error: Lower Right is not at lower right corner
				SetAlert(IDS_BT_LRCHECK2);
			}
		}
	}
	else if (szSetupMode == "Single Block")
	{
		switch(m_ulMachineType)
		{
			case BT_MACHTYPE_STD:

				if (m_oBinBlkMain.GrabIsSetup(ulBlkId))
				{
					lIsValidUL = m_oBinBlkMain.ValidateSingleSetupUL(&m_oPhyBlkMain, ulBlkId,
									lBinBlkULX, lBinBlkULY);

					if (lIsValidUL > 0)
					{
						m_ulSourcePhyBlk = lIsValidUL;

						if (m_oBinBlkMain.GrabIsSetup(ulBlkId))
						{
							lIsValidLR = m_oBinBlkMain.ValidateSingleSetupLR(&m_oPhyBlkMain, m_ulBinBlkToSetup,
								m_ulSourcePhyBlk, lBinBlkULX, lBinBlkULY,lBinBlkLRX, lBinBlkLRY);
							
							if (lIsValidLR == 1)
							{
								m_bIfEnableTBSubmit = TRUE;
							}
							else if (lIsValidLR < 0)
							{
								m_bIfEnableTBSubmit = FALSE;

								SetAlert(m_oBinBlkMain.GetMsgCode());
							}
						}
					}
					else if (lIsValidUL == -1)
					{
						m_bIfEnableTBSubmit = FALSE;

						//Error: Upper left is not on any physical block!
						SetAlert(IDS_BT_ULCHECK3);
					}
					else if (lIsValidUL == -2)
					{
						m_bIfEnableTBSubmit = FALSE;

						//Error: Upper Left overlaps with existing bin block!
						SetAlert(IDS_BT_ULCHECK1);
					}
				}

			break;

			//BinLoader
			case BT_MACHTYPE_DTABLE:		//v4.16T1
			case BT_MACHTYPE_DBUFFER:		//v3.71T4
			case BT_MACHTYPE_DL_DLA:

				if (m_oBinBlkMain.GrabIsSetup(ulBlkId))
				{
					lIsValidUL = m_oBinBlkMain.ValidateSingleSetupUL_BL(&m_oPhyBlkMain, ulBlkId,
									lBinBlkULX, lBinBlkULY, m_ulSourcePhyBlk);

					if (lIsValidUL > 0)
					{
						//m_ulSourcePhyBlk = lIsValidUL;

						if (m_oBinBlkMain.GrabIsSetup(ulBlkId))
						{
							lIsValidLR = m_oBinBlkMain.ValidateSingleSetupLR_BL(&m_oPhyBlkMain, m_ulBinBlkToSetup,
								lBinBlkULX, lBinBlkULY,lBinBlkLRX, lBinBlkLRY, m_ulSourcePhyBlk);

							if (lIsValidLR == 1)
							{
								m_bIfEnableTBSubmit = TRUE;
							}
							else if (lIsValidLR == -1)
							{
								m_bIfEnableTBSubmit = FALSE;
								SetAlert(IDS_BT_LRCHECK5);

							}
							else if (lIsValidLR == -2)
							{
								m_bIfEnableTBSubmit = FALSE;
								SetAlert(IDS_BT_LRCHECK6);
							}
						}
					}
					else if (lIsValidUL == -1)
					{
						m_bIfEnableTBSubmit = FALSE;

						//Error: Upper left is not on any physical block!
						SetAlert(IDS_BT_ULCHECK3);
					}
					else if (lIsValidUL == -2)
					{
						m_bIfEnableTBSubmit = FALSE;

						//Error: Upper Left overlaps with existing bin block!
						SetAlert(IDS_BT_ULCHECK1);
					}
				}

			break;

		}
	}

	return 1;
} //end ConfirmBinBlkSetupLR

/*
LONG CBinTable::SetWafflePadDestControlEnability(IPC_CServiceMessage& svMsg)
{
	BOOL bSet;
	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bSet);

	if (bSet)
	{
		m_bIfEnableWafflePad = FALSE;
		m_bIfEnableWafflePadCheckBox = FALSE;
		m_bIfEnablePadPitchSetX = FALSE;
		m_bIfEnablePadPitchSetY = FALSE;
		m_bIfEnablePadPitchResetX  = FALSE;
		m_bIfEnablePadPitchResetY  = FALSE;
		m_bIfEnablePadDimX  = FALSE;
		m_bIfEnablePadDimY  = FALSE;
		m_bIfEnableWafflePadPageBack  = FALSE;

	}
	else
	{
		m_bIfEnableWafflePad = TRUE;
		m_bIfEnableWafflePadCheckBox = TRUE;
		m_bIfEnablePadPitchSetX = TRUE;
		m_bIfEnablePadPitchSetY = TRUE;
		m_bIfEnablePadPitchResetX  = TRUE;
		m_bIfEnablePadPitchResetY  = TRUE;
		m_bIfEnablePadDimX  = TRUE;
		m_bIfEnablePadDimY  = TRUE;
		m_bIfEnableWafflePadPageBack  = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
*/

LONG CBinTable::SetWafflePadDestX(IPC_CServiceMessage& svMsg)
{
	CString szTitle, szContent;
	BOOL bReturn = TRUE;
	LONG lDelta;
	LONG lVerticalStreetX1, lVerticalStreetX2;

	szTitle.LoadString(HMB_BT_LEARN_PAD_PITCH);
	SetJoystickOn(TRUE);

#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif
	
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001

	szContent.LoadString(HMB_BT_MV_LEFT_SIDE_PAD_VET_GAP);
	HmiMessageEx(szContent, szTitle);
	GetEncoderValue();
	lVerticalStreetX1 = m_lEnc_X;  
	
	szContent.LoadString(HMB_BT_MV_RIGHT_SIDE_PAD_VET_GAP);
	HmiMessageEx(szContent, szTitle);
	GetEncoderValue();
	lVerticalStreetX2 = m_lEnc_X;
	
	lDelta = abs(lVerticalStreetX2 - lVerticalStreetX1);
	
	m_lWafflePadDistX = ConvertXEncoderValueForDisplay(lDelta);

	SetJoystickOn(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;	
}

LONG CBinTable::ResetWafflePadDestX(IPC_CServiceMessage& svMsg)
{
	m_lWafflePadDistX = 0;
	return 1;
}

LONG CBinTable::SetWafflePadDestY(IPC_CServiceMessage& svMsg)
{
	CString szTitle, szContent;
	LONG lDelta;
	LONG lVerticalStreetY1, lVerticalStreetY2;

	szTitle.LoadString(HMB_BT_LEARN_PAD_PITCH);
	SetJoystickOn(TRUE);

#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif
	
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001

	szContent.LoadString(HMB_BT_MV_UPPER_SIDE_PAD_HORZ_GAP);
	HmiMessageEx(szContent, szTitle);
	GetEncoderValue();
	lVerticalStreetY1 = m_lEnc_Y;  
	
	szContent.LoadString(HMB_BT_MV_LOWER_SIDE_PAD_HORZ_GAP);
	HmiMessageEx(szContent, szTitle);
	GetEncoderValue();
	lVerticalStreetY2 = m_lEnc_Y;
	
	lDelta = abs(lVerticalStreetY2 - lVerticalStreetY1);
	
	m_lWafflePadDistY = ConvertYEncoderValueForDisplay(lDelta);

	SetJoystickOn(FALSE);

	return 1;
}

LONG CBinTable::ResetWafflePadDestY(IPC_CServiceMessage& svMsg)
{
	m_lWafflePadDistY = 0;
	return 1;
}

VOID CBinTable::SetUseBinMapBondArea()
{
	m_oBinBlkMain.SetUseBinMapBondArea(m_bEnableBinMapBondArea);
}

LONG CBinTable::SubmitBinBlkSetup(IPC_CServiceMessage& svMsg)
{
	CString szSetupMode;
	char *pBuffer;
	ULONG ulBlkId;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;


	//v4.59A19
	//For non MS90, Die Pitch precision should have no decimal digit
	if (!IsMS90())
	{
		LONG lDiePitchX = (LONG) m_dDiePitchX;		//Truncate 
		LONG lDiePitchY = (LONG) m_dDiePitchY;		//Truncate 
		m_dDiePitchX = lDiePitchX;
		m_dDiePitchY = lDiePitchY;
	}


	SetupBinBlk(szSetupMode, TRUE, TRUE);
	
	//v4.03		//PLSG Bin map fcn
	if (m_bEnableBinMapBondArea)
	{
		if ( (szSetupMode == "Single Block") && (m_oBinBlkMain.GrabUseByPassBinMap(m_ulBinBlkToSetup)) )
		{
			//v4.48A14	//Semitek, 3E DL
		}
		else
		{
			CNGGrade *pNGGrade = CNGGrade::Instance();
			if (!pNGGrade->IsNGBlock(ulBlkId))
			{
				HmiMessage("BinMap fcn is disabled; please re-enable & re-teach bin map pattern");
			
				m_bEnableBinMapBondArea = FALSE;
				m_oBinBlkMain.SetUseBinMapBondArea(m_bEnableBinMapBondArea);						//v4.03		//PLLM Bin map fcn

				m_BinMapWrapper.InitMap();
				SaveBinTableData();
			}
		}
	}
	if (m_bEnableOsramBinMixMap)	//v4.47A6
	{
		HmiMessage("OSRAM BinMap fcn is disabled; please re-enable & re-teach bin map pattern");
		
		m_bEnableOsramBinMixMap = FALSE;
		m_BinMapWrapper.InitMap();
		SaveBinTableData();
	}

	//v4.42T10
	if (szSetupMode == "Single Block")
	{
		m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(ulBlkId);
		m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(ulBlkId);
		m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(ulBlkId);
		m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(ulBlkId);
	}

	LogItems(NO_OF_BIN_BLOCKS);
	LogItems(MAX_DIE_QUANTITIES);
	LogItems(BIN_BLOCKS_DIE_QUANTITIES);
	LogItems(BIN_BLOCKS_WALK_PATH);
	
	BackupToNVRAM();

    // 3396
    SetGemValue("BT_BinSetupMode", szSetupMode);
    SetGemValue("BT_BinNoOfBlocks", m_ulNoOfBinBlk);
    SetGemValue("BT_BinBlockToSetup", m_ulBinBlkToSetup);
    // 3397
    SetGemValue("BT_BinUpLeftX", m_lBinUpperLeftX);
    SetGemValue("BT_BinUpLeftY", m_lBinUpperLeftY);
    SetGemValue("BT_BinLowRightX", m_lBinLowerRightX);
    SetGemValue("BT_BinLowRightY", m_lBinLowerRightY);
    // 3398
    SetGemValue("BT_BinBlockPitchX",	m_lBinBlkPitchX);
    SetGemValue("BT_BinBlockPitchY",	m_lBinBlkPitchY);
    SetGemValue("BT_BinDiePitchX",		_round(m_dDiePitchX));
    SetGemValue("BT_BinDiePitchY",		_round(m_dDiePitchY));
    // 3399
    SetGemValue("BT_BinDieNoPerRow", m_ulDiePerRow);
    SetGemValue("BT_BinDieNoPerCol", m_ulDiePerCol);
    SetGemValue("BT_BinDieNoPerBlock", m_ulDiePerBlk);
    // 3400
    SetGemValue("BT_BinGradeAutoAssign", m_bAutoAssignGrade);
    SetGemValue("BT_BinStartGrade", m_ucStartingGrade);
    SetGemValue("BT_BinBlockGrade", m_ucGrade);
    // 7402
    SendEvent(SG_CEID_BT_BINSETUP, FALSE);

	return 1;
} //end SubmitBinBlkSetup


ULONG CBinTable::GetWalkPathWithHMIName(CString szWalkPathHMI)
{
	ULONG ulWalkPath = BT_TLH_PATH;

	//Update WalkPath
	if ( szWalkPathHMI == BT_TLH_NAME )
	{
		ulWalkPath = BT_TLH_PATH;
	}
	else if ( szWalkPathHMI == BT_TLV_NAME )
	{
		ulWalkPath = BT_TLV_PATH;
	}
	else if ( szWalkPathHMI == BT_TRH_NAME )
	{
		ulWalkPath = BT_TRH_PATH;
	}
	else if ( szWalkPathHMI == BT_TRV_NAME )
	{
		ulWalkPath = BT_TRV_PATH;
	}
	else if ( szWalkPathHMI == BT_BLH_NAME )
	{
		ulWalkPath = BT_BLH_PATH;
	}
	else if ( szWalkPathHMI == BT_BLV_NAME )
	{
		ulWalkPath = BT_BLV_PATH;
	}
	else if ( szWalkPathHMI == BT_BRH_NAME )
	{
		ulWalkPath = BT_BRH_PATH;
	}
	else if ( szWalkPathHMI == BT_BRV_NAME )
	{
		ulWalkPath = BT_BRV_PATH;
	}

	return ulWalkPath;
}


CString CBinTable::GetWalkPathHMIName(ULONG ulWalkPath)
{
	CString szWalkPath = BT_TLH_NAME;

	switch(ulWalkPath)
	{
	case BT_TLV_PATH:	
		szWalkPath = BT_TLV_NAME;	
		break;
	case BT_TRH_PATH:	
		szWalkPath = BT_TRH_NAME;
		break;
	case BT_TRV_PATH:
		szWalkPath = BT_TRV_NAME;
		break;
	case BT_BLH_PATH:
		szWalkPath = BT_BLH_NAME;
		break;
	case BT_BLV_PATH:
		szWalkPath = BT_BLV_NAME;
		break;
	case BT_BRH_PATH:
		szWalkPath = BT_BRH_NAME;
		break;
	case BT_BRV_PATH:
		szWalkPath = BT_BRV_NAME;
		break;
	default:	
		szWalkPath = BT_TLH_NAME;	
		break;
	}

	return szWalkPath;
}


LONG CBinTable::SaveBinBlkTemplate(IPC_CServiceMessage& svMsg)
{
	LONG lSaveTemplate = 0;
	ULONG ulBlkId;
	CString szSetupMode;
	CString szTitle, szContent;
	char *pBuffer;
	LONG lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY;
	LONG lSourcePhyBlkULY, lSourcePhyBlkLRY;
	LONG lIsValidUL, lIsValidLR;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	lBinBlkULX = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftX);
	lBinBlkULY = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftY);
	lBinBlkLRX = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightX);
	lBinBlkLRY = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightY);   

	DOUBLE dDiePitchX = ConvertDisplayUnitToDFileUnit(m_dDiePitchX);
	DOUBLE dDiePitchY = ConvertDisplayUnitToDFileUnit(m_dDiePitchY);

	m_bIfEnablePIBlkToSetup = TRUE;
	
	//Update WalkPath
	m_ulWalkPath = GetWalkPathWithHMIName(m_szWalkPath);

	//Protect MaxUnit cannot > Cal die per blk
	if ( m_ulMaxUnit > m_ulDiePerBlk )
	{
		m_ulMaxUnit = 0;
	}

	if (! m_oPhyBlkMain.GrabIsSetup(m_ulSourcePhyBlk))
	{
		SetAlert(IDS_BT_SOURCE_PHYBLK_NOT_SETUP);
	}


	//Upper Left & Lower Right validation
	lIsValidUL = m_oBinBlkMain.ValidateSingleSetupUL_BL(&m_oPhyBlkMain, 
		m_ulBinBlkToSetup, lBinBlkULX, lBinBlkULY, m_ulSourcePhyBlk);

	if (lIsValidUL == -1)
	{
		SetAlert(IDS_BT_ULCHECK4);
	}
	else if (lIsValidUL == -2)
	{
		SetAlert(IDS_BT_ULCHECK5);
	}
	else if (lIsValidUL == 1)
	{
		lIsValidLR = m_oBinBlkMain.ValidateSingleSetupLR_BL(&m_oPhyBlkMain,
			m_ulBinBlkToSetup, lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY,
			m_ulSourcePhyBlk);

		if (lIsValidLR == -1)
		{	
			SetAlert(IDS_BT_LRCHECK5);
		}
		else if (lIsValidLR == -2)
		{
			SetAlert(IDS_BT_LRCHECK6);
		}
		else if (lIsValidLR == -3)
		{
			SetAlert(IDS_BT_URCHECK2);
		}
		else if (lIsValidLR == -4)
		{
			SetAlert(IDS_BT_LLCHECK1);
		}
		else if (lIsValidLR == -5)
		{
			SetAlert(IDS_BT_LEFTBOUNDARYCHK);
		}
		else if (lIsValidLR == -6)
		{
			SetAlert(IDS_BT_RIGHTBOUNDARYCHK);
		}
		else if (lIsValidLR == -7)
		{
			SetAlert(IDS_BT_UPPERBOUNDARYCHK);
		}
		else if (lIsValidLR == -8)
		{
			SetAlert(IDS_BT_LOWERBOUNDARYCHK);
		}
		else if ((lIsValidLR == -9) || (lIsValidLR == -10))
		{
			SetAlert(IDS_BT_OVERLAPPINGCHK1);
		}
		else if (lIsValidLR == -11)
		{
			SetAlert(IDS_BT_LRCHECK2);
		}
		else if (lIsValidLR == 1)
		{
			lSourcePhyBlkULY = m_oPhyBlkMain.GrabBlkUpperLeftY(m_ulSourcePhyBlk);
			lSourcePhyBlkLRY = m_oPhyBlkMain.GrabBlkLowerRightY(m_ulSourcePhyBlk);

			lSaveTemplate = m_oBinBlkMain.SaveBinBlkDataAsTemplate(m_szBinBlkTemplate, lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY, 
				m_szWalkPath, m_ulDiePerUserRow, m_ulDiePerUserCol, dDiePitchX, dDiePitchY, m_ulSkipUnit, m_ulMaxUnit, 
				m_ulTemplateSetupInputCount, m_bEnableFirstRowColSkipPattern, m_ulFirstRowColSkipUnit, 
				m_bCentralizedBondArea, m_bTeachWithPhysicalBlk, m_bUseBlockCornerAsFirstDiePos, m_bEnableWafflePad,
				m_lWafflePadSizeX, m_lWafflePadSizeY, m_lWafflePadDistX, m_lWafflePadDistY);

			// success
			if (lSaveTemplate == 1)
			{
				
				szTitle.LoadString(HMB_BT_BIN_BLK_TEMPLATE_SETUP);
				szContent.LoadString(HMB_BT_SAVE_BINBLK_TEMPLATE_COMPLETE);

				SetStatusMessage(szContent);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

				ShowBinBlkTemplateData();
			}
			// hit max limit
			else if (lSaveTemplate == -1)
			{
				szTitle.LoadString(HMB_BT_BIN_BLK_TEMPLATE_SETUP);
				szContent.LoadString(HMB_BT_TEMPLATE_HIT_LIMIT);

				SetStatusMessage(szContent);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				
			}
			// cannot save into file
			else
			{
				
				szTitle.LoadString(HMB_BT_BIN_BLK_TEMPLATE_SETUP);
				szContent.LoadString(HMB_BT_FAIL_TO_SAVE_BINBLK_TEMPLATE);

				SetStatusMessage(szContent);
				HmiMessage(szContent , szTitle , glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			}
		}
	}
			
	return 1;
} //end SubmitBinBlkSetup

LONG CBinTable::ShowBinBlkTemplateData(IPC_CServiceMessage& svMsg)
{
	m_bShowBinBlkTemplateSetup = CMS896AStn::m_bEnableBinBlkTemplate;	//v4.42T8
	
	ShowBinBlkTemplateData();
	return 1;
}

LONG CBinTable::LoadBinBlkTemplate(IPC_CServiceMessage& svMsg)
{
	AutoSetupBinBlkUsingGradeRankFile();
	return 1;
}

LONG CBinTable::RemoveBinBlkTemplate(IPC_CServiceMessage& svMsg)
{
	CString szContent, szTitle;

	if (m_oBinBlkMain.RemoveBinBlkTemplate(m_szBinBlkTemplate) == FALSE)
	{
		szTitle.LoadString(HMB_BT_REMOVE_BINBLK_TEMPLATE);
		szContent.LoadString(HMB_BT_REMOVE_BINBLK_TEMPLATE_FAIL);
		
		HmiMessage(szContent , szTitle , glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		
		return 1;
	
	}

	szTitle.LoadString(HMB_BT_REMOVE_BINBLK_TEMPLATE);
	szContent.LoadString(HMB_BT_REMOVE_BINBLK_TEMPLATE_COMPLETE);

	HmiMessage(szContent , szTitle , glHMI_MBX_OK, 
				glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	
	ShowBinBlkTemplateData();


	return 1;
}


LONG CBinTable::CancelBinBlkSetup(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;
	CString szSetupMode;
	char *pBuffer;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulBlkId, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	m_bFirstTimeSetUL = FALSE;
	m_bFirstTimeSetLR = FALSE;

	SetJoystickOn(FALSE);
	m_ulJoystickFlag = 0;

	//Enability of Hmi Controls
	m_bIfEnableTBClrBlkSettings = TRUE;
	m_bIfEnableCBSetupMode = TRUE;
	m_bIfEnablePINoOfBlk = TRUE;  

	if (((szSetupMode == "All Blocks") && (m_ulBinBlkToSetup == 1))
									|| (szSetupMode == "Single Block"))
	{
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
		m_bIfEnablePIDiePitchX = TRUE;
		m_bIfEnablePIDiePitchY = TRUE;
		m_bIfEnablePIDiePerRow = TRUE;
		m_bIfEnablePIDiePerCol = TRUE;
		m_bIfEnablePIDiePerBlk = TRUE;
		m_bIfEnableFirstRowColSkipPattern = TRUE;
		m_bIfEnableTeachWithPhysicalBlk = TRUE;
		m_bIfEnable2DBarcodeOutput = TRUE;
		m_bIfEnableTemplateSetup = TRUE;
		m_bIfEnableCentralizedBondArea = TRUE;
		m_bIfEnableWafflePad = TRUE;
		m_bIfEnableWafflePadCheckBox = TRUE;
		m_bIfEnablePadPitchSetX = TRUE;
		m_bIfEnablePadPitchSetY = TRUE;
		m_bIfEnablePadPitchResetX = TRUE;
		m_bIfEnablePadPitchResetY = TRUE;
		m_bIfEnablePadDimX = TRUE;
		m_bIfEnablePadDimY = TRUE;
		m_bIfUseBlockCornerAsFirstDiePos = TRUE;
	}
	else
	{
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnablePIDiePitchX = FALSE;
		m_bIfEnablePIDiePitchY = FALSE;
		m_bIfEnablePIDiePerRow = FALSE;
		m_bIfEnablePIDiePerCol = FALSE;
		m_bIfEnablePIDiePerBlk = FALSE;
		m_bIfEnableFirstRowColSkipPattern = FALSE;
		m_bIfEnableTeachWithPhysicalBlk = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;	
		m_bIfEnableTemplateSetup = FALSE;
		m_bIfEnableCentralizedBondArea = FALSE;
		m_bIfEnableWafflePad = FALSE;
		m_bIfEnableWafflePadCheckBox = FALSE;
		m_bIfEnablePadPitchSetX = FALSE;
		m_bIfEnablePadPitchSetY = FALSE;
		m_bIfEnablePadPitchResetX = FALSE;
		m_bIfEnablePadPitchResetY = FALSE;
		m_bIfEnablePadDimX = FALSE;
		m_bIfEnablePadDimY = FALSE;
		m_bIfUseBlockCornerAsFirstDiePos = FALSE;
	}

	m_bIfEnableTBConfirmUL = FALSE;
	m_bIfEnableTBConfirmLR = FALSE;
	m_bIfEnableDDUpperLeftX = FALSE;
	m_bIfEnableDDUpperLeftY = FALSE;
	m_bIfEnableDDLowerRightX = FALSE;
	m_bIfEnableDDLowerRightY = FALSE;
	m_bIfEnablePIBlkToSetup = TRUE;

	if (szSetupMode == "All Blocks")
	{
		//m_bIfEnablePIBlkToSetup = FALSE;
		m_bIfEnableChkBAutoAssignGrade = TRUE;
		m_bIfEnablePIGrade = FALSE;
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;
		m_bIfEnable2DBarcodeOutput = TRUE;
		m_bIfEnableTemplateSetup = FALSE;
	}
	else if (szSetupMode == "Single Block")		//Klocwork
	{
		//m_bIfEnablePIBlkToSetup = TRUE;
		m_bIfEnableChkBAutoAssignGrade = FALSE;
		m_bIfEnablePIGrade = TRUE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;
		m_bIfEnableTemplateSetup = TRUE;
	}

	m_bIfEnableCBWalkPath = TRUE;
	m_bIfEnableTBSubmit = TRUE;
	m_bIfEnableTBCancel = TRUE;	

	//Restore original data values from structure (for screen display)

	m_ulNoOfBinBlk = m_oBinBlkMain.GetNoOfBlk();

	m_lBinUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(ulBlkId));
	m_lBinUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(ulBlkId));
	m_lBinLowerRightX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkLowerRightX(ulBlkId));
	m_lBinLowerRightY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkLowerRightY(ulBlkId));

	m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(ulBlkId));
	m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(ulBlkId));
	m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(ulBlkId);
	m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(ulBlkId);
	m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(ulBlkId);
	m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(ulBlkId);
	m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(ulBlkId);	//v4.59A22	//David Ma
	m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(ulBlkId);	//v4.59A22	//David Ma
	m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(ulBlkId);

	m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(ulBlkId));

	m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkId);
	m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(ulBlkId);
	m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(ulBlkId);
	m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(ulBlkId);
	m_ulMaxUnit	  = m_oBinBlkMain.GrabMaxUnit(ulBlkId);
	m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(ulBlkId);
	m_ulFirstRowColSkipUnit			= m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(ulBlkId);
	m_bTeachWithPhysicalBlk = m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(ulBlkId);

	//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
	m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(ulBlkId);
	m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(ulBlkId);
	m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(ulBlkId);
	DOUBLE dRadius = (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(ulBlkId));		// from encoder step to um
	m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 

	m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(ulBlkId);
	if (m_bCentralizedBondArea)
	{
		m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(ulBlkId);
		m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(ulBlkId);
		m_lCentralizedUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(ulBlkId) + m_lCentralizedOffsetX);
		m_lCentralizedUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(ulBlkId) + m_lCentralizedOffsetY);
	}
	else
	{
		m_lCentralizedUpperLeftX = 0;
		m_lCentralizedUpperLeftY = 0;
	}

	m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(ulBlkId);
	if (m_bEnableWafflePad)
	{
		m_lWafflePadDistX =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(ulBlkId));
		m_lWafflePadDistY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(ulBlkId));
		m_lWafflePadSizeX = m_oBinBlkMain.GrabWafflePadSizeX(ulBlkId);
		m_lWafflePadSizeY = m_oBinBlkMain.GrabWafflePadSizeY(ulBlkId);
	}
	else
	{
		m_lWafflePadDistX = 0;
		m_lWafflePadDistY = 0;
		m_lWafflePadSizeX = 0;
		m_lWafflePadSizeY = 0;
	}

	m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(ulBlkId);
	m_bNoReturnTravel = m_oBinBlkMain.GrabNoReturnTravel(ulBlkId);		//v4.42T1

	if (szSetupMode == "All Blocks")
	{
		m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
		m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

		m_bAutoAssignGrade = TRUE;
	}
	else if (szSetupMode == "Single Block")
	{
		m_ucGrade = m_oBinBlkMain.GrabGrade(ulBlkId);
	}

	return 1;
} //end CancelBinBlkSetup


LONG CBinTable::DisplayBinBlkData(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		ULONG	ulPhyBlkId;
		ULONG	ulBinBlkId;	
	} BLKDATA;
	BLKDATA stInfo;

	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

	if ((stInfo.ulBinBlkId >= 1) && (stInfo.ulBinBlkId <= MS_MAX_BIN))
	{
		//Enabilities of controls
		if ((m_szSetupMode == "All Blocks") && (stInfo.ulBinBlkId != 1))
		{
			SetAlert(IDS_BT_EDITBLK1DATAONLY);

			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnablePIDiePitchX = FALSE;
			m_bIfEnablePIDiePitchY = FALSE;
			m_bIfEnablePIGrade = FALSE;
			m_bIfEnableCBWalkPath = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel = FALSE;
		}
		else if ((m_szSetupMode == "All Blocks") && (stInfo.ulBinBlkId == 1))
		{
			m_bIfEnableTBSetUL = TRUE;
			m_bIfEnableTBSetLR = TRUE;
			m_bIfEnablePIDiePitchX = TRUE;
			m_bIfEnablePIDiePitchY = TRUE;
			m_bIfEnableChkBAutoAssignGrade = TRUE;
			m_bIfEnableCBWalkPath = TRUE;
		}

		//Update HMI variables upon selecting "Block to setup"
		m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(stInfo.ulBinBlkId));	
		m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(stInfo.ulBinBlkId));
		m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(stInfo.ulBinBlkId));
		m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(stInfo.ulBinBlkId));

		m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(stInfo.ulBinBlkId));
		m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(stInfo.ulBinBlkId));
		m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(stInfo.ulBinBlkId);
		m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(stInfo.ulBinBlkId);
		m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(stInfo.ulBinBlkId);
		m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(stInfo.ulBinBlkId);
		m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(stInfo.ulBinBlkId);	//v4.59A22	//David Ma
		m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(stInfo.ulBinBlkId);	//v4.59A22	//David Ma

		m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(stInfo.ulBinBlkId);

		m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(stInfo.ulBinBlkId);
		m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(stInfo.ulBinBlkId);
		m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(stInfo.ulBinBlkId);
		m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(stInfo.ulBinBlkId); 	 
		m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(stInfo.ulBinBlkId); 	
		
		m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(stInfo.ulBinBlkId);
		m_ulFirstRowColSkipUnit			= m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(stInfo.ulBinBlkId);
		m_bTeachWithPhysicalBlk = m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(stInfo.ulBinBlkId);

		//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
		m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(stInfo.ulBinBlkId);
		m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(stInfo.ulBinBlkId);
		m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(stInfo.ulBinBlkId);
		DOUBLE dRadius = (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(stInfo.ulBinBlkId));		// from encoder step to um
		m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 

		m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(stInfo.ulBinBlkId);
		if (m_bCentralizedBondArea)
		{
			m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(stInfo.ulBinBlkId);
			m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(stInfo.ulBinBlkId);
			m_lCentralizedUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(stInfo.ulBinBlkId) + m_lCentralizedOffsetX);
			m_lCentralizedUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(stInfo.ulBinBlkId) + m_lCentralizedOffsetY);
		}
		else
		{
			m_lCentralizedUpperLeftX = 0;
			m_lCentralizedUpperLeftY = 0;
		}

		m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(stInfo.ulBinBlkId);
		if (m_bEnableWafflePad)
		{
			m_lWafflePadDistX =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(stInfo.ulBinBlkId));
			m_lWafflePadDistY =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(stInfo.ulBinBlkId));
			m_lWafflePadSizeX = m_oBinBlkMain.GrabWafflePadSizeX(stInfo.ulBinBlkId);
			m_lWafflePadSizeY = m_oBinBlkMain.GrabWafflePadSizeY(stInfo.ulBinBlkId);
		}
		else
		{
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;
		}

		m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(stInfo.ulBinBlkId);
		m_bNoReturnTravel = m_oBinBlkMain.GrabNoReturnTravel(stInfo.ulBinBlkId);		//v4.42T1

		m_ucGrade = m_oBinBlkMain.GrabGrade(stInfo.ulBinBlkId);
		
		m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(stInfo.ulBinBlkId));

		SetJoystickOn(FALSE);

		if (m_bUseDualTablesOption)		//v4.17T5
		{
			HomeTable2();		
		}

		MoveXYTo(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftX(stInfo.ulBinBlkId)), 
				 ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftY(stInfo.ulBinBlkId)));
	}
	return 1;
} //end DisplayBinBlkData


//=============================================================================
//  Set Frame Rotation
//=============================================================================
BOOL CBinTable::BT_SetFrameRotation(ULONG ulBlkId, BOOL bMoveToOrgPosn)
{
	BOOL bRet = TRUE;

	if (IsMS90())
	{
		//Change from motor steps to um
		if (m_bIsRotate180Deg || IsMS90Sorting2ndPart())
		{
			bRet = BT_SetFrameRotation(ulBlkId, TRUE, bMoveToOrgPosn);
		}
		else
		{
			bRet = BT_SetFrameRotation(ulBlkId, FALSE, bMoveToOrgPosn);
		}
	}

	return bRet;
}


LONG CBinTable::MoveToBinBlk(IPC_CServiceMessage& svMsg)
{
	//Move to bin block Last-Die

	ULONG ulBlkId;
	LONG lTime_X=0, lTime_Y=0;
	LONG lPosX=0, lPosY=0;
	DOUBLE dPosX=0, dPosY=0;
	BOOL bReply = TRUE;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);
	SetJoystickOn(FALSE);


	//v4.40T5	//Nichia MS100+
	BOOL bReAlignFrame = (BOOL)(LONG)(*m_psmfSRam)["BinLoader"]["RealignBinFrame"];
	if (m_bEnable_T && !bReAlignFrame)	//!m_bAlignBinInAlive)		//only perform BC scanning here if RealignFrame is not enabled
	{
		if (!BT_SetFrameRotation(ulBlkId, FALSE))
		{
			CString szLog;
			szLog = _T("BT MoveToBinBlk: Rotation fails");
			SetErrorMessage(szLog);
			bReply = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}

		if (!IsMS90())	//v4.49A10
		{
			CMSLogFileUtility::Instance()->BL_LogStatus("BT: Start MoveToBinBlk scan BC ...");
			BOOL bReturn = ScanningBarcode(ulBlkId, TRUE);
			if (bReturn == FALSE)
			{
				CString szLog;
				szLog = _T("BT MoveToBinBlk: ScanningBarcode fails");
				SetErrorMessage(szLog);
				bReply = FALSE;
			}
		}
	}

	GetDieLogicalEncoderValue(ulBlkId, m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkId), lPosX, lPosY);

	//v4.44T5	//Semitek	//LeoLam
	LONG lOldX = lPosX;
	LONG lOldY = lPosY;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);


	//Get X & Y travel time
	GetEncoderValue();
	//CalculateProfileTime(X_ProfName(NORMAL_PROF), abs(m_lEnc_X - lPosX), lTime_X);
	//CalculateProfileTime(Y_ProfName(NORMAL_PROF), abs(m_lEnc_Y - lPosY), lTime_Y);
	m_lCurXPosn = m_lEnc_X;
	m_lCurYPosn = m_lEnc_Y;
	SelectXYProfile(lPosX - m_lCurXPosn, lPosY - m_lCurYPosn);
	lTime_X = CMS896AStn::MotionGetProfileTime(BT_AXIS_X, X_ProfName(m_nProfile_X), abs(m_lEnc_X - lPosX), abs(m_lEnc_X - lPosX), HIPEC_SAMPLE_RATE, &m_stBTAxis_X);
	lTime_Y = CMS896AStn::MotionGetProfileTime(BT_AXIS_Y, Y_ProfName(m_nProfile_Y), abs(m_lEnc_Y - lPosY), abs(m_lEnc_Y - lPosY), HIPEC_SAMPLE_RATE, &m_stBTAxis_Y);

#ifdef NU_MOTION

	if (CMS896AApp::m_bIsPrototypeMachine)
	{
		X_MoveTo(lPosX, SFM_WAIT);
	}
	else
	{
		X_MoveTo(lPosX, SFM_NOWAIT);
		//If X takes longer travel time, wait some time before start Y
		if ( (lTime_X - lTime_Y) > 0 )
		{
			Sleep(lTime_X - lTime_Y);
		}
	}
	Y_MoveTo(lPosY, SFM_WAIT);
	X_Sync();

#else
	X_MoveTo(lPosX, SFM_NOWAIT);
	//If X takes longer travel time, wait some time before start Y
	if ( (lTime_X - lTime_Y) > 0 )
	{
		Sleep(lTime_X - lTime_Y);
	}
	Y_MoveTo(lPosY, SFM_WAIT);
	X_Sync();
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;

} //end MoveToBinBlk


LONG CBinTable::MoveToBinBlk2(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;
	LONG lTime_X=0, lTime_Y=0;
	LONG lPosX=0, lPosY=0;
	DOUBLE dPosX=0, dPosY=0;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	SetJoystickOn(FALSE);

	GetDieLogicalEncoderValue(ulBlkId, m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkId), lPosX, lPosY);
	lPosX += m_lBT2OffsetX;
	lPosY += m_lBT2OffsetY;
/*
	if (IsTable1InBondRegion())
	{
		CString szErr = "ERROR (MoveToBinBlk2): BT1 still in BOND region; BT2 cannot move!";
		HmiMessage_Red_Back(szErr);

		BOOL bReply = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}
*/
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);

	X2_Profile(LOW_PROF1);
	Y2_Profile(LOW_PROF1);
	//if (m_bUseDualTablesOption)
	//{
	//	HomeTable1();		
	//}

	GetEncoderValue();

	if (bPLLMSpecialFcn)
	{
		lTime_X = CMS896AStn::MotionGetProfileTime(BT_AXIS_X2, X_ProfName(NORMAL_PROF), abs(m_lEnc_X2 - lPosX), abs(m_lEnc_X2 - lPosX), HIPEC_SAMPLE_RATE, &m_stBTAxis_X2);
		lTime_Y = CMS896AStn::MotionGetProfileTime(BT_AXIS_Y2, Y_ProfName(NORMAL_PROF), abs(m_lEnc_Y2 - lPosY), abs(m_lEnc_Y2 - lPosY), HIPEC_SAMPLE_RATE, &m_stBTAxis_Y2);
	}
	else
	{
		if (!CMS896AApp::m_bUseSlowBTControlProfile)	
			lTime_X = CMS896AStn::MotionGetProfileTime(BT_AXIS_X2, X_ProfName(LOW_PROF), abs(m_lEnc_X2 - lPosX), abs(m_lEnc_X2 - lPosX), HIPEC_SAMPLE_RATE, &m_stBTAxis_X2);
		else
			lTime_X = CMS896AStn::MotionGetProfileTime(BT_AXIS_X2, X_ProfName(NORMAL_PROF), abs(m_lEnc_X2 - lPosX), abs(m_lEnc_X2 - lPosX), HIPEC_SAMPLE_RATE, &m_stBTAxis_X2);
		lTime_Y = CMS896AStn::MotionGetProfileTime(BT_AXIS_Y2, Y_ProfName(LOW_PROF), abs(m_lEnc_Y2 - lPosY), abs(m_lEnc_Y2 - lPosY), HIPEC_SAMPLE_RATE, &m_stBTAxis_Y2);
	}


	if (CMS896AApp::m_bIsPrototypeMachine)
	{
		X2_MoveTo(lPosX, SFM_WAIT);
	}
	else
	{
		X2_MoveTo(lPosX, SFM_NOWAIT);
		//If X takes longer travel time, wait some time before start Y
		if ( (lTime_X - lTime_Y) > 0 )
		{
			Sleep(lTime_X - lTime_Y);
		}
	}
	Y2_MoveTo(lPosY, SFM_WAIT);
	X2_Sync();


	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;

} //end MoveToBinBlk


LONG CBinTable::MoveToBinBlk1stDie(IPC_CServiceMessage& svMsg)
{
	//Move to bin block 1st-Die

	ULONG ulBlkId;
	LONG lTime_X=0, lTime_Y=0;
	LONG lPosX=0, lPosY=0;
	DOUBLE dPosX=0, dPosY=0;
	BOOL bReply = TRUE;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	//SetJoystickOn(FALSE);		//v2.93T2

	//v4.40T5	//Nichia MS100+
	BOOL bReAlignFrame = (BOOL)(LONG)(*m_psmfSRam)["BinLoader"]["RealignBinFrame"];
	if (m_bEnable_T && !bReAlignFrame)	//!m_bAlignBinInAlive)		//only perform BC scanning here if RealignFrame is not enabled
	{
		BT_SetFrameRotation(ulBlkId, FALSE);

		if (!IsMS90())	//v4.49A10
		{
			CMSLogFileUtility::Instance()->BL_LogStatus("BT: Start MoveToBinBlk1stDie scan BC ...");
			BOOL bReturn = ScanningBarcode(ulBlkId, TRUE);
			if (bReturn == FALSE)
			{
				CString szLog;
				szLog = _T("BT MoveToBinBlk: ScanningBarcode fails");
				CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
				SetErrorMessage(szLog);
				
				bReply = FALSE;		//v4.40T7
				svMsg.InitMessage(sizeof(BOOL), &bReply);	//v4.43T10
				return 1;
			}
			else	//v4.43T10
			{
				CString szLog;
				szLog = _T("BT MoveToBinBlk: ScanningBarcode OK");
				CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
			}
		}
	}


	GetDieLogicalEncoderValue(ulBlkId, 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkId), lPosX, lPosY);

	LONG lOldX = lPosX;
	LONG lOldY = lPosY;

	//v4.44A4
	//OpAddPostBondOffset(lPosX, lPosY);		//v4.42T17	//Silan	//LeoLam
	//v4.43T7
	//CString szLog;
	//szLog.Format("MS100(BH2-OFFSET MoveToBinBlk1stDie): BLK #%lu - XEnc=%ld: New(%ld), YEnc=%ld: New(%ld)", 
	//			ulBlkId, lOldX, lPosX, lOldY, lPosY);
	//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/


	//pllm
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);

#ifdef NU_MOTION
	//Use VLow table profiles (MS100 9Inch only); in MS100(Plus) LOW_PROF1 = LOW_PROF	//v4.17T5
	X_Profile(LOW_PROF1);	
	Y_Profile(LOW_PROF1);
#else
	if (bPLLMSpecialFcn)
	{
		X_Profile(FAST_PROF);
		Y_Profile(FAST_PROF);
	}
	else
	{
		if (!CMS896AApp::m_bUseSlowBTControlProfile)		//v3.67T5
		{
			X_Profile(LOW_PROF);
		}
		Y_Profile(LOW_PROF);
	}
#endif

	//Get X & Y travel time
	GetEncoderValue();
	//CalculateProfileTime(X_ProfName(NORMAL_PROF), abs(m_lEnc_X - lPosX), lTime_X);
	//CalculateProfileTime(Y_ProfName(NORMAL_PROF), abs(m_lEnc_Y - lPosY), lTime_Y);
	m_lCurXPosn = m_lEnc_X;
	m_lCurYPosn = m_lEnc_Y;

	MoveXYTo(lPosX, lPosY);

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
} //end MoveToBinBlk


LONG CBinTable::Test(IPC_CServiceMessage& svMsg)
{
	if (CMS896AStn::m_WaferMapWrapper.IsMapValid())
	{
		UCHAR ucOrigGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalDieInformation(20, 43) - 
							CMS896AStn::m_WaferMapWrapper.GetGradeOffset();

		CString szMsg;
		szMsg.Format("Map grade at 20,43 = %d", ucOrigGrade);

		HmiMessage(szMsg);
	}

/*
	SetJoystickOn(FALSE);
AfxMessageBox("Test ...", MB_SYSTEMMODAL);

	if (!CMS896AApp::m_bUseSlowBTControlProfile)		//v3.67T5
	{
		X_Profile(LOW_PROF);
	}
	Y_Profile(LOW_PROF);

	for (INT i=0; i<1; i++)
	{
		XY_MoveTo(0, 50000, SFM_WAIT);
		Sleep(1000);
		XY_MoveTo(0, 0, SFM_WAIT);
		Sleep(1000);
	}

	//v3.65
	//Y_SelectControl(BT_DYN_CTRL);
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
*/
	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::OnChangeBinBlkSetupModeSelection(IPC_CServiceMessage& svMsg)
{
	ULONG i;
	char *pBuffer;
	CString szSetupMode;
	BOOL bIfBondingInProcess, bIfIsFirstBlkInMerge;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szSetupMode = &pBuffer[0];

	delete [] pBuffer;

	if (szSetupMode == "All Blocks")
	{
		if (m_ulNoOfBinBlk == 0)
		{
			//Disable all controls
			m_bIfEnablePIBlkToSetup = FALSE;
			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnablePIBlkPitchX = FALSE;
			m_bIfEnablePIBlkPitchY = FALSE;
			m_bIfEnablePIDiePitchX = FALSE;
			m_bIfEnablePIDiePitchY = FALSE;
			m_bIfEnablePIDiePerRow = FALSE;
			m_bIfEnablePIDiePerCol = FALSE;
			m_bIfEnablePIDiePerBlk = FALSE;
			m_bIfEnableChkBAutoAssignGrade = FALSE;
			m_bIfEnablePIGrade = FALSE;
			m_bIfEnableCBWalkPath = FALSE;
			m_bIfEnableFirstRowColSkipPattern = FALSE;
			m_bIfEnableTeachWithPhysicalBlk = FALSE;
			m_bIfEnable2DBarcodeOutput = FALSE;	
			m_bIfEnableTemplateSetup = FALSE;
			m_bIfEnableCentralizedBondArea = FALSE;
			m_bIfEnableWafflePadCheckBox = FALSE;
			m_bIfEnablePadPitchSetX = FALSE;
			m_bIfEnablePadPitchSetY = FALSE;
			m_bIfEnablePadPitchResetX = FALSE;
			m_bIfEnablePadPitchResetY = FALSE;
			m_bIfEnablePadDimX = FALSE;
			m_bIfEnablePadDimY = FALSE;
			m_bIfEnableWafflePad = FALSE;
			m_bIfUseBlockCornerAsFirstDiePos = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel = FALSE;

			m_ulBinBlkToSetup = 0;
			m_lBinUpperLeftX = 0;	
			m_lBinUpperLeftY = 0;
			m_lBinLowerRightX = 0;
			m_lBinLowerRightY = 0;

			m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
			m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

			m_dDiePitchX = 500;
			m_dDiePitchY = 500;
			m_bUsePt5UmInDiePitchX	= FALSE;
			m_bUsePt5UmInDiePitchY	= FALSE;
			m_dBondAreaOffsetXInUm	= 0;
			m_dBondAreaOffsetYInUm	= 0;
			m_dThermalDeltaPitchXInUm	= 0;	//v4.59A22	//David Ma
			m_dThermalDeltaPitchYInUm	= 0;	//v4.59A22	//David Ma
			m_ulDiePerRow = 0;
			m_ulDiePerCol = 0;
			m_ulDiePerBlk = 0;
			m_bByPassBinMap = FALSE;

			m_bEnableFirstRowColSkipPattern = 0;
			m_ulFirstRowColSkipUnit = 0;

			m_bTeachWithPhysicalBlk = FALSE;
			
			//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
			m_bEnableCircularBondArea	= FALSE;
			m_lCirAreaCenterX			= 0;
			m_lCirAreaCenterY			= 0;
			m_dCirAreaRadius			= 0;

			m_bCentralizedBondArea = FALSE;
			m_lCentralizedUpperLeftX = 0;
			m_lCentralizedUpperLeftY = 0;

			m_bEnableWafflePad = FALSE;
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;

			m_bUseBlockCornerAsFirstDiePos = FALSE;
			m_bNoReturnTravel = FALSE;		//v4.42T1

			m_ucGrade = 1;
			
			m_szWalkPath = BT_TLH_NAME;
		}
		else //no of blks > 0
		{
			m_ulBinBlkToSetup = 1;
			
			//Display Block 1 Data
			m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(1));	
			m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(1));
			m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(1));
			m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(1));

			m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
			m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

			m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(1));
			m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(1));
			m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(1);
			m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(1);
			m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(1);
			m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(1);
			m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(1);	//v4.59A22	//David Ma
			m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(1);	//v4.59A22	//David Ma
			m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(1);

			m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(1);
			m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(1);
			m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(1);
			m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(1); 	 
			m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(1); 
			m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(1);
			m_ulFirstRowColSkipUnit = m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(1);

			m_bTeachWithPhysicalBlk = m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(1);

			m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(1);
			if (m_bCentralizedBondArea)
			{
				m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(1);
				m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(1);
				m_lCentralizedUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(1) + m_lCentralizedOffsetX);
				m_lCentralizedUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(1) + m_lCentralizedOffsetY);
			}
			else
			{
				m_lCentralizedUpperLeftX = 0;
				m_lCentralizedUpperLeftY = 0;
			}

			m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(1);
			if (m_bEnableWafflePad)
			{
				m_lWafflePadDistX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(1));
				m_lWafflePadDistY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(1));
				m_lWafflePadSizeX = m_oBinBlkMain.GrabWafflePadSizeX(1);
				m_lWafflePadSizeY = m_oBinBlkMain.GrabWafflePadSizeY(1);
			}
			else
			{
				m_lWafflePadDistX = 0;
				m_lWafflePadDistY = 0;
				m_lWafflePadSizeX = 0;
				m_lWafflePadSizeY = 0;
			}

			m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(1);
			m_bNoReturnTravel = m_oBinBlkMain.GrabNoReturnTravel(1);		//v4.42T1

			//pllm
			m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(1);
			m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(1);
			m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(1);
			DOUBLE dRadius = (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(1));		// from encoder step to um
			m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 
CString szTemp;
szTemp.Format("1.  %.1f, %.1f, %d, %d", m_dCirAreaRadius, dRadius, m_oBinBlkMain.GrabCircularBinRadius(1), ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(1)));
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);		

			m_ucGrade = m_oBinBlkMain.GrabGrade(1);
			
			m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(1));

			bIfBondingInProcess = FALSE;

			for (i = 1; i <= m_ulNoOfBinBlk; i++)
			{
				if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
				{
					bIfBondingInProcess = TRUE;
					break;
				}
			}

			if (bIfBondingInProcess)
			{
				//Disable all controls
				m_bIfEnablePINoOfBlk = FALSE;
				m_bIfEnablePIBlkToSetup = FALSE;
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = FALSE;
				m_bIfEnablePIDiePitchY = FALSE;
				m_bIfEnablePIDiePerRow = FALSE;
				m_bIfEnablePIDiePerCol = FALSE;
				m_bIfEnablePIDiePerBlk = FALSE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = FALSE;
				m_bIfEnableWafflePad = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;
				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;

				SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
			}
			else //NOT during bonding
			{
				bIfIsFirstBlkInMerge = 
					(m_oBinBlkMain.GrabBlkUpperLeftX(1) !=
					m_oBinBlkMain.GrabBlkUpperLeftXFromSetup(1)) 
					||
					(m_oBinBlkMain.GrabBlkUpperLeftY(1) != 
					m_oBinBlkMain.GrabBlkUpperLeftYFromSetup(1)) 
					||
					(m_oBinBlkMain.GrabBlkLowerRightX(1) !=
					m_oBinBlkMain.GrabBlkLowerRightXFromSetup(1)) 
					||
					(m_oBinBlkMain.GrabBlkLowerRightY(1) !=
					m_oBinBlkMain.GrabBlkLowerRightYFromSetup(1));

				if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(1))
				{
					m_bIfEnableTBSetUL = FALSE;
				    m_bIfEnableTBSetLR = FALSE;

					SetAlert(IDS_BT_BLKDIMENSIONCHK1);
				}
				else if (bIfIsFirstBlkInMerge)
				{
					m_bIfEnableTBSetUL = FALSE;
				    m_bIfEnableTBSetLR = FALSE;

					SetAlert(IDS_BT_BLKDIMENSIONCHK2);
				}
				else
				{
					m_bIfEnableTBSetUL = TRUE;
					m_bIfEnableTBSetLR = TRUE;
				}			

				//Enable other controls
				m_bIfEnablePIBlkPitchX = TRUE;
				m_bIfEnablePIBlkPitchY = TRUE;
				m_bIfEnablePIDiePitchX = TRUE;
				m_bIfEnablePIDiePitchY = TRUE;
				m_bIfEnablePIDiePerRow = TRUE;
				m_bIfEnablePIDiePerCol = TRUE;
				m_bIfEnablePIDiePerBlk = TRUE;
				m_bIfEnableChkBAutoAssignGrade = TRUE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = TRUE;
				m_bIfEnableFirstRowColSkipPattern = TRUE;
				m_bIfEnableTeachWithPhysicalBlk = TRUE;
				m_bIfEnable2DBarcodeOutput = TRUE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = TRUE;
				m_bIfEnableWafflePad = TRUE;
				m_bIfEnableWafflePadCheckBox = TRUE;
				m_bIfEnablePadPitchSetX = TRUE;
				m_bIfEnablePadPitchSetY = TRUE;
				m_bIfEnablePadPitchResetX = TRUE;
				m_bIfEnablePadPitchResetY = TRUE;
				m_bIfEnablePadDimX = TRUE;
				m_bIfEnablePadDimY = TRUE;
				m_bIfUseBlockCornerAsFirstDiePos = TRUE;
				m_bIfEnableTBSubmit = TRUE;
				m_bIfEnableTBCancel = TRUE;
			}
		}
	}
	else if (szSetupMode == "Single Block")
	{
		if (m_ulNoOfBinBlk == 0)
		{
			m_bIfEnablePIBlkToSetup = FALSE;
			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnablePIBlkPitchX = FALSE;
			m_bIfEnablePIBlkPitchY = FALSE;
			m_bIfEnablePIDiePitchX = FALSE;
			m_bIfEnablePIDiePitchY = FALSE;
			m_bIfEnablePIDiePerRow = FALSE;
			m_bIfEnablePIDiePerCol = FALSE;
			m_bIfEnablePIDiePerBlk = FALSE;
			m_bIfEnableChkBAutoAssignGrade = FALSE;
			m_bIfEnablePIGrade = FALSE;
			m_bIfEnableCBWalkPath = FALSE;
			m_bIfEnableFirstRowColSkipPattern = FALSE;
			m_bIfEnableTeachWithPhysicalBlk = FALSE;
			m_bIfEnable2DBarcodeOutput = FALSE;	
			m_bIfEnableTemplateSetup = FALSE;
			m_bIfEnableCentralizedBondArea = FALSE;
			m_bIfEnableWafflePadCheckBox = FALSE;
			m_bIfEnablePadPitchSetX = FALSE;
			m_bIfEnablePadPitchSetY = FALSE;
			m_bIfEnablePadPitchResetX = FALSE;
			m_bIfEnablePadPitchResetY = FALSE;
			m_bIfEnablePadDimX = FALSE;
			m_bIfEnablePadDimY = FALSE;
			m_bIfEnableWafflePad = FALSE;
			m_bIfUseBlockCornerAsFirstDiePos = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel	= FALSE;		
		}
		else //no of blk != 0
		{
			if (m_ulBinBlkToSetup == 0)
			{
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = FALSE;
				m_bIfEnablePIDiePitchY = FALSE;
				m_bIfEnablePIDiePerRow = FALSE;
				m_bIfEnablePIDiePerCol = FALSE;
				m_bIfEnablePIDiePerBlk = FALSE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;
				m_bIfEnableWafflePad = FALSE;
				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;
			}
			else
			{
				//Display "Blk to Setup" Data
				m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(m_ulBinBlkToSetup));	
				m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(m_ulBinBlkToSetup));
				m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(m_ulBinBlkToSetup));
				m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(m_ulBinBlkToSetup));

				m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
				m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

				m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(m_ulBinBlkToSetup));
				m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(m_ulBinBlkToSetup));
				m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(m_ulBinBlkToSetup);
				m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(m_ulBinBlkToSetup);
				m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(m_ulBinBlkToSetup);
				m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(m_ulBinBlkToSetup);
				m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(m_ulBinBlkToSetup);	//v4.59A22	//David Ma
				m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(m_ulBinBlkToSetup);	//v4.59A22	//David Ma
				m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(m_ulBinBlkToSetup);

				m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(m_ulBinBlkToSetup);
				m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(m_ulBinBlkToSetup);
				m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToSetup);
				m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(m_ulBinBlkToSetup); 	 
				m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(m_ulBinBlkToSetup);
				m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(m_ulBinBlkToSetup);
				m_ulFirstRowColSkipUnit = m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(m_ulBinBlkToSetup);
				m_bTeachWithPhysicalBlk = m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(m_ulBinBlkToSetup);
			
				//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
				m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(m_ulBinBlkToSetup);
				m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(m_ulBinBlkToSetup);
				m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(m_ulBinBlkToSetup);
				DOUBLE dRadius = (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(m_ulBinBlkToSetup));		// from encoder step to um
				m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 
//pllm
CString szTemp;
szTemp.Format("2.  %.1f, %.1f, %d, %d", m_dCirAreaRadius, dRadius, m_oBinBlkMain.GrabCircularBinRadius(1), ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(m_ulBinBlkToSetup)));
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);		

				m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(m_ulBinBlkToSetup);
				if (m_bCentralizedBondArea)
				{
					m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(m_ulBinBlkToSetup);
					m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(m_ulBinBlkToSetup);
					m_lCentralizedUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(m_ulBinBlkToSetup) + m_lCentralizedOffsetX);
					m_lCentralizedUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(m_ulBinBlkToSetup) + m_lCentralizedOffsetY);
				}
				else
				{
					m_lCentralizedUpperLeftX = 0;
					m_lCentralizedUpperLeftY = 0;
				}

				m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(m_ulBinBlkToSetup);
				m_bNoReturnTravel = m_oBinBlkMain.GrabNoReturnTravel(m_ulBinBlkToSetup);		//v4.42T1

				m_ucGrade = m_oBinBlkMain.GrabGrade(m_ulBinBlkToSetup);
				
				m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(m_ulBinBlkToSetup));
				
				UCHAR uGrade = m_oBinBlkMain.GrabGrade(m_ulBinBlkToSetup);
				//m_oBinBlkMain.GrabNVNoOfSortedDie(uGrade)
				if (m_oBinBlkMain.GrabNVNoOfSortedDie(uGrade) > 0)
				{
					m_bIfEnableTBSetUL = FALSE;
					m_bIfEnableTBSetLR = FALSE;
					m_bIfEnablePIBlkPitchX = FALSE;
					m_bIfEnablePIBlkPitchY = FALSE;
					m_bIfEnablePIDiePitchX = FALSE;
					m_bIfEnablePIDiePitchY = FALSE;
					m_bIfEnablePIDiePerRow = FALSE;
					m_bIfEnablePIDiePerCol = FALSE;
					m_bIfEnablePIDiePerBlk = FALSE;
					m_bIfEnableChkBAutoAssignGrade = FALSE;
					m_bIfEnablePIGrade = FALSE;
					m_bIfEnableCBWalkPath = FALSE;
					m_bIfEnableFirstRowColSkipPattern = FALSE;
					m_bIfEnableTeachWithPhysicalBlk = FALSE;
					m_bIfEnable2DBarcodeOutput = FALSE;
					m_bIfEnableTemplateSetup = FALSE;
					m_bIfEnableCentralizedBondArea = FALSE;
					m_bIfEnableWafflePad = FALSE;
					m_bIfEnableWafflePadCheckBox = FALSE;
					m_bIfEnablePadPitchSetX = FALSE;
					m_bIfEnablePadPitchSetY = FALSE;
					m_bIfEnablePadPitchResetX = FALSE;
					m_bIfEnablePadPitchResetY = FALSE;
					m_bIfEnablePadDimX = FALSE;
					m_bIfEnablePadDimY = FALSE;
					m_bIfUseBlockCornerAsFirstDiePos = FALSE;
					m_bIfEnableTBSubmit = FALSE;
					m_bIfEnableTBCancel = FALSE;		

					SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
				}
				else
				{
					bIfIsFirstBlkInMerge = 
					(m_oBinBlkMain.GrabBlkUpperLeftX(m_ulBinBlkToSetup) !=
					m_oBinBlkMain.GrabBlkUpperLeftXFromSetup(m_ulBinBlkToSetup)) 
					||
					(m_oBinBlkMain.GrabBlkUpperLeftY(m_ulBinBlkToSetup) != 
					m_oBinBlkMain.GrabBlkUpperLeftYFromSetup(m_ulBinBlkToSetup)) 
					||
					(m_oBinBlkMain.GrabBlkLowerRightX(m_ulBinBlkToSetup) !=
					m_oBinBlkMain.GrabBlkLowerRightXFromSetup(m_ulBinBlkToSetup)) 
					||
					(m_oBinBlkMain.GrabBlkLowerRightY(m_ulBinBlkToSetup) !=
					m_oBinBlkMain.GrabBlkLowerRightYFromSetup(m_ulBinBlkToSetup));

					if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(m_ulBinBlkToSetup))
					{
						m_bIfEnableTBSetUL = FALSE;
						m_bIfEnableTBSetLR = FALSE;

						SetAlert(IDS_BT_BLKDIMENSIONCHK1);
					}
					else if (bIfIsFirstBlkInMerge)
					{
						m_bIfEnableTBSetUL = FALSE;
						m_bIfEnableTBSetLR = FALSE;

						SetAlert(IDS_BT_BLKDIMENSIONCHK2);
					}
					else
					{
						m_bIfEnableTBSetUL = TRUE;
						m_bIfEnableTBSetLR = TRUE;
					}		

					m_bIfEnablePIBlkToSetup = TRUE;
					m_bIfEnableTBSetUL = TRUE;
					m_bIfEnableTBSetLR = TRUE;
					m_bIfEnablePIBlkPitchX = FALSE;
					m_bIfEnablePIBlkPitchY = FALSE;
					m_bIfEnablePIDiePitchX = TRUE;
					m_bIfEnablePIDiePitchY = TRUE;
					m_bIfEnablePIDiePerRow = TRUE;
					m_bIfEnablePIDiePerCol = TRUE;
					m_bIfEnablePIDiePerBlk = TRUE;
					m_bIfEnableChkBAutoAssignGrade = FALSE;
					m_bIfEnablePIGrade = TRUE;
					m_bIfEnableCBWalkPath = TRUE;
					m_bIfEnableFirstRowColSkipPattern = TRUE;
					m_bIfEnableTeachWithPhysicalBlk = TRUE;
					m_bIfEnable2DBarcodeOutput = FALSE;
					m_bIfEnableTemplateSetup = TRUE;
					m_bIfEnableCentralizedBondArea = TRUE;
					m_bIfEnableWafflePad = TRUE;
					m_bIfEnableWafflePadCheckBox = TRUE;
					m_bIfEnablePadPitchSetX = TRUE;
					m_bIfEnablePadPitchSetY = TRUE;
					m_bIfEnablePadPitchResetX = TRUE;
					m_bIfEnablePadPitchResetY = TRUE;
					m_bIfEnablePadDimX = TRUE;
					m_bIfEnablePadDimY = TRUE;

					m_bIfUseBlockCornerAsFirstDiePos = TRUE;
					m_bIfEnableTBSubmit = TRUE;
					m_bIfEnableTBCancel = TRUE;
				}
			}
		}
	}

	return 1;
} //end OnChangeBinBlkSetupModeSelection


LONG CBinTable::OnClickNoOfBinBlkInput(IPC_CServiceMessage& svMsg)
{
	ULONG ulNoOfBlk;
	CString szSetupMode;
	char *pBuffer;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulNoOfBlk, pBuffer, sizeof(ULONG));
	szSetupMode = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	if (ulNoOfBlk == 0)
	{
		m_bIfEnablePIBlkToSetup = FALSE;
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnablePIDiePitchX = FALSE;
		m_bIfEnablePIDiePitchY = FALSE;
		m_bIfEnablePIDiePerRow = FALSE;
		m_bIfEnablePIDiePerCol = FALSE;
		m_bIfEnablePIDiePerBlk = FALSE;
		m_bIfEnableChkBAutoAssignGrade = FALSE;
		m_bIfEnablePIGrade = FALSE;
		m_bIfEnableCBWalkPath = FALSE;
		m_bIfEnableFirstRowColSkipPattern = FALSE;
		m_bIfEnableTeachWithPhysicalBlk = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;
		m_bIfEnableTemplateSetup = FALSE;
		m_bIfEnableCentralizedBondArea = FALSE;
		m_bIfEnableWafflePad = FALSE;
		m_bIfEnableWafflePadCheckBox = FALSE;
		m_bIfEnablePadPitchSetX = FALSE;
		m_bIfEnablePadPitchSetY = FALSE;
		m_bIfEnablePadPitchResetX = FALSE;
		m_bIfEnablePadPitchResetY = FALSE;
		m_bIfEnablePadDimX = FALSE;
		m_bIfEnablePadDimY = FALSE;
		m_bIfUseBlockCornerAsFirstDiePos = FALSE;
		m_bIfEnableTBSubmit = FALSE;
		m_bIfEnableTBCancel = FALSE;

		m_ulBinBlkToSetup = 0;

		m_lBinUpperLeftX = 0;	
		m_lBinUpperLeftY = 0;
		m_lBinLowerRightX = 0;
		m_lBinLowerRightY = 0;

		m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
		m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

		m_dDiePitchX = 500;
		m_dDiePitchY = 500;
		m_bUsePt5UmInDiePitchX	= FALSE;
		m_bUsePt5UmInDiePitchY	= FALSE;
		m_dBondAreaOffsetXInUm	= 0;
		m_dBondAreaOffsetYInUm	= 0;
		m_dThermalDeltaPitchXInUm	= 0;	//v4.59A22	//David Ma
		m_dThermalDeltaPitchYInUm	= 0;	//v4.59A22	//David Ma
		m_bByPassBinMap			= FALSE;

		m_ulDiePerRow = 0;
		m_ulDiePerCol = 0;
		m_ulDiePerBlk = 0;

		m_bEnableFirstRowColSkipPattern = FALSE;
		m_ulFirstRowColSkipUnit = 0;
		m_bTeachWithPhysicalBlk = FALSE;

		//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
		m_bEnableCircularBondArea	= FALSE;
		m_lCirAreaCenterX			= 0;
		m_lCirAreaCenterY			= 0;
		m_dCirAreaRadius			= 0;

		m_bCentralizedBondArea = FALSE;
		m_lCentralizedUpperLeftX = 0;
		m_lCentralizedUpperLeftY = 0;

		m_bEnableWafflePad = FALSE;
		m_lWafflePadDistX = 0;
		m_lWafflePadDistY = 0;
		m_lWafflePadSizeX = 0;
		m_lWafflePadSizeY = 0;

		m_bUseBlockCornerAsFirstDiePos = FALSE;
		m_bNoReturnTravel	= FALSE;

		m_ucGrade = 1;
	
		m_szWalkPath = BT_TLH_NAME;
		
		SetAlert(IDS_BT_NOOFBLKSCHK);
	}
	else //no of blks > 0
	{
		m_bIfEnablePIBlkToSetup = TRUE;

		if (szSetupMode == "All Blocks")
		{
			m_ulBinBlkToSetup = 1;

			//Enablities of controls
			m_bIfEnableTBSetUL = TRUE;
			m_bIfEnableTBSetLR = TRUE;
			m_bIfEnablePIBlkPitchX = TRUE;
			m_bIfEnablePIBlkPitchY = TRUE;
			m_bIfEnablePIDiePitchX = TRUE;
			m_bIfEnablePIDiePitchY = TRUE;
			m_bIfEnablePIDiePerRow = TRUE;
			m_bIfEnablePIDiePerCol = TRUE;
			m_bIfEnablePIDiePerBlk = TRUE;
			m_bIfEnableChkBAutoAssignGrade = TRUE;
			m_bIfEnablePIGrade = FALSE;
			m_bIfEnableCBWalkPath = TRUE;
			m_bIfEnableFirstRowColSkipPattern = TRUE;
			m_bIfEnableTeachWithPhysicalBlk = TRUE;
			m_bIfEnable2DBarcodeOutput = TRUE;
			m_bIfEnableTemplateSetup = FALSE;
			m_bIfEnableCentralizedBondArea = TRUE;
			m_bIfEnableWafflePadCheckBox = TRUE;
			m_bIfEnablePadPitchSetX = TRUE;
			m_bIfEnablePadPitchSetY = TRUE;
			m_bIfEnablePadPitchResetX = TRUE;
			m_bIfEnablePadPitchResetY = TRUE;
			m_bIfEnablePadDimX = TRUE;
			m_bIfEnablePadDimY = TRUE;

			m_bTeachWithPhysicalBlk = TRUE;
			m_bCentralizedBondArea = TRUE;

			m_bIfEnableWafflePad = TRUE;
			m_bIfUseBlockCornerAsFirstDiePos = TRUE;
			m_bIfEnableTBSubmit = TRUE;
			m_bIfEnableTBCancel = TRUE;
		}
	}

	return 1;
} //end OnClickNoOfBinBlkInput


LONG CBinTable::OnClickBinBlkToSetupInput(IPC_CServiceMessage& svMsg)
{
	ULONG i;
	BOOL bIfBondingInProcess, bIfIsFirstBlkInMerge;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	typedef struct 
	{
		ULONG	ulPhyBlkId;
		ULONG	ulBinBlkId;	
	} BLKDATA;
	BLKDATA stInfo;

	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

	if (stInfo.ulBinBlkId == 0)
	{
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnablePIDiePitchX = FALSE;
		m_bIfEnablePIDiePitchY = FALSE;
		m_bIfEnablePIDiePerRow = FALSE;
		m_bIfEnablePIDiePerCol = FALSE;
		m_bIfEnablePIDiePerBlk = FALSE;
		m_bIfEnableChkBAutoAssignGrade = FALSE;
		m_bIfEnablePIGrade = FALSE;
		m_bIfEnableCBWalkPath = FALSE;
		m_bIfEnableFirstRowColSkipPattern = FALSE;
		m_bIfEnableTeachWithPhysicalBlk = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;
		m_bIfEnableTemplateSetup = FALSE;
		m_bIfEnableCentralizedBondArea = FALSE;
		m_bIfEnableWafflePad = FALSE;
		m_bIfEnableWafflePadCheckBox = FALSE;
		m_bIfEnablePadPitchSetX = FALSE;
		m_bIfEnablePadPitchSetY = FALSE;
		m_bIfEnablePadPitchResetX = FALSE;
		m_bIfEnablePadPitchResetY = FALSE;
		m_bIfEnablePadDimX = FALSE;
		m_bIfEnablePadDimY = FALSE;
		m_bIfUseBlockCornerAsFirstDiePos = FALSE;
		m_bIfEnableTBSubmit = FALSE;
		m_bIfEnableTBCancel	= FALSE;

		m_lBinUpperLeftX = 0;	
		m_lBinUpperLeftY = 0;
		m_lBinLowerRightX = 0;
		m_lBinLowerRightY = 0;

		m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
		m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

		m_dDiePitchX = 500;
		m_dDiePitchY = 500;
		m_bUsePt5UmInDiePitchX	= FALSE;
		m_bUsePt5UmInDiePitchY	= FALSE;
		m_dBondAreaOffsetXInUm	= 0;
		m_dBondAreaOffsetYInUm	= 0;
		m_dThermalDeltaPitchXInUm	= 0;	//v4.59A22	//David Ma
		m_dThermalDeltaPitchYInUm	= 0;	//v4.59A22	//David Ma
		m_bByPassBinMap			= FALSE;

		m_ulDiePerRow = 0;
		m_ulDiePerCol = 0;
		m_ulDiePerBlk = 0;

		m_bEnableFirstRowColSkipPattern = FALSE;
		m_ulFirstRowColSkipUnit = 0;
		m_bTeachWithPhysicalBlk = FALSE;
		
		//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
		m_bEnableCircularBondArea	= FALSE;
		m_lCirAreaCenterX			= 0;
		m_lCirAreaCenterY			= 0;
		m_dCirAreaRadius			= 0;

		m_bCentralizedBondArea = FALSE;
		m_lCentralizedUpperLeftX = 0;
		m_lCentralizedUpperLeftY = 0;
		
		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			//m_bTeachWithPhysicalBlk			= TRUE;
			m_bCentralizedBondArea			= TRUE;
		}

		m_bEnableWafflePad = FALSE;
		m_lWafflePadDistX = 0;
		m_lWafflePadDistY = 0;
		m_lWafflePadSizeX = 0;
		m_lWafflePadSizeY = 0;

		m_bUseBlockCornerAsFirstDiePos = FALSE;
		m_bNoReturnTravel = FALSE;

		m_ucGrade = 1;
		m_szWalkPath = BT_TLH_NAME;
		m_ulSourcePhyBlk = 1;

		//Error: Block to Setup must be greater than 0!
		SetAlert(IDS_BT_BLKTOSETUPCHK);	
	}
	else
	{
		switch (m_ulMachineType)
		{
			case BT_MACHTYPE_STD:
				BinBlksDrawing();
			break;

			case BT_MACHTYPE_DTABLE:		//v4.16T1
			case BT_MACHTYPE_DBUFFER:		//v3.71T4
			case BT_MACHTYPE_DL_DLA:
				BinBlksDrawing_BL(stInfo.ulBinBlkId);
			break;
		}
		//Display "Blk to Setup" data
		m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(stInfo.ulBinBlkId));	
		m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(stInfo.ulBinBlkId));
		m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(stInfo.ulBinBlkId));
		m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(stInfo.ulBinBlkId));

		m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
		m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

		m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(stInfo.ulBinBlkId));
		m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(stInfo.ulBinBlkId));
		m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(stInfo.ulBinBlkId);
		m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(stInfo.ulBinBlkId);
		m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(stInfo.ulBinBlkId);
		m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(stInfo.ulBinBlkId);
		m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(stInfo.ulBinBlkId);	//v4.59A22	//David Ma
		m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(stInfo.ulBinBlkId);	//v4.59A22	//David Ma
		m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(stInfo.ulBinBlkId);

		m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(stInfo.ulBinBlkId);
		m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(stInfo.ulBinBlkId);
		m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(stInfo.ulBinBlkId);
		m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(stInfo.ulBinBlkId); 	 
		m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(stInfo.ulBinBlkId);
		m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(stInfo.ulBinBlkId);
		m_ulFirstRowColSkipUnit = m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(stInfo.ulBinBlkId);
		m_bTeachWithPhysicalBlk = m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(stInfo.ulBinBlkId); 

		//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
		m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(stInfo.ulBinBlkId);
		m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(stInfo.ulBinBlkId);
		m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(stInfo.ulBinBlkId);
		DOUBLE dRadius = (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(stInfo.ulBinBlkId));		// from encoder step to um
		m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 

		m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(stInfo.ulBinBlkId);
		if (m_bCentralizedBondArea)
		{
			m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(stInfo.ulBinBlkId);
			m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(stInfo.ulBinBlkId);
			m_lCentralizedUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(stInfo.ulBinBlkId) + m_lCentralizedOffsetX);
			m_lCentralizedUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(stInfo.ulBinBlkId) + m_lCentralizedOffsetY);
		}
		else
		{
			m_lCentralizedUpperLeftX = 0;
			m_lCentralizedUpperLeftY = 0;
		}

		m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(stInfo.ulBinBlkId);
		if (m_bEnableWafflePad)
		{
			m_lWafflePadDistX =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(stInfo.ulBinBlkId));
			m_lWafflePadDistY =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(stInfo.ulBinBlkId));
			m_lWafflePadSizeX = m_oBinBlkMain.GrabWafflePadSizeX(stInfo.ulBinBlkId);
			m_lWafflePadSizeY = m_oBinBlkMain.GrabWafflePadSizeY(stInfo.ulBinBlkId);
		}
		else
		{
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;
			m_lWafflePadSizeX = 0;
			m_lWafflePadSizeY = 0;
		}

		m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(stInfo.ulBinBlkId);
		m_bNoReturnTravel	= m_oBinBlkMain.GrabNoReturnTravel(stInfo.ulBinBlkId);

		m_ucGrade = m_oBinBlkMain.GrabGrade(stInfo.ulBinBlkId);

		if (m_oBinBlkMain.GrabIsSetup(stInfo.ulBinBlkId))
			m_ulSourcePhyBlk = m_oBinBlkMain.GrabPhyBlkId(stInfo.ulBinBlkId);
		else
			m_ulSourcePhyBlk = 1;
		
		m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(stInfo.ulBinBlkId));

		SetJoystickOn(FALSE);

		if (m_bUseDualTablesOption)
		{
			HomeTable2();		//v4.17T5
		}
		MoveXYTo(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftX(stInfo.ulBinBlkId)), 
				 ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftY(stInfo.ulBinBlkId)));

		if (m_szSetupMode == "All Blocks")
		{
			if (stInfo.ulBinBlkId == 1)
			{
				bIfBondingInProcess = FALSE;

				for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
				{
					if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
					{
						bIfBondingInProcess = TRUE;
						break;
					}
				}

				if (bIfBondingInProcess)
				{
					m_bIfEnableTBSetUL = FALSE;
					m_bIfEnableTBSetLR = FALSE;
					m_bIfEnablePIBlkPitchX = FALSE;
					m_bIfEnablePIBlkPitchY = FALSE;
					m_bIfEnablePIDiePitchX = FALSE;
					m_bIfEnablePIDiePitchY = FALSE;
					m_bIfEnablePIDiePerRow = FALSE;
					m_bIfEnablePIDiePerCol = FALSE;
					m_bIfEnablePIDiePerBlk = FALSE;
					m_bIfEnableChkBAutoAssignGrade = FALSE;
					m_bIfEnablePIGrade = FALSE;
					m_bIfEnableCBWalkPath = FALSE;
					m_bIfEnableFirstRowColSkipPattern = FALSE;
					m_bIfEnableTeachWithPhysicalBlk = FALSE;
					m_bIfEnable2DBarcodeOutput = FALSE;
					m_bIfEnableTemplateSetup = FALSE;
					m_bIfEnableCentralizedBondArea = FALSE;
					m_bIfEnableWafflePadCheckBox = FALSE;
					m_bIfEnablePadPitchSetX = FALSE;
					m_bIfEnablePadPitchSetY = FALSE;
					m_bIfEnablePadPitchResetX = FALSE;
					m_bIfEnablePadPitchResetY = FALSE;
					m_bIfEnablePadDimX = FALSE;
					m_bIfEnablePadDimY = FALSE;
					m_bIfEnableWafflePad = FALSE;
					m_bIfUseBlockCornerAsFirstDiePos = FALSE;
					m_bIfEnableTBSubmit = FALSE;
					m_bIfEnableTBCancel = FALSE;	

					//Block settings can't be edited because it contains bin blocks.
					SetAlert(IDS_BT_DENYBLKSETTINGSEDIT1);
				}
				else
				{
					bIfIsFirstBlkInMerge = 
						(m_oBinBlkMain.GrabBlkUpperLeftX(1) !=
						m_oBinBlkMain.GrabBlkUpperLeftXFromSetup(1)) 
						||
						(m_oBinBlkMain.GrabBlkUpperLeftY(1) != 
						m_oBinBlkMain.GrabBlkUpperLeftYFromSetup(1)) 
						||
						(m_oBinBlkMain.GrabBlkLowerRightX(1) !=
						m_oBinBlkMain.GrabBlkLowerRightXFromSetup(1)) 
						||
						(m_oBinBlkMain.GrabBlkLowerRightY(1) !=
						m_oBinBlkMain.GrabBlkLowerRightYFromSetup(1));

					if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(1))
					{
						m_bIfEnableTBSetUL = FALSE;
						m_bIfEnableTBSetLR = FALSE;

						//Block Dimensions can't be edited when it's disabled from same 
						//grade merge. You can only do this if grade is changed.
						SetAlert(IDS_BT_BLKDIMENSIONCHK1);
					}
					else if (bIfIsFirstBlkInMerge)
					{
						m_bIfEnableTBSetUL = FALSE;
						m_bIfEnableTBSetLR = FALSE;

						//Block Dimensions can't be edited if it's the first block in merge!
						SetAlert(IDS_BT_BLKDIMENSIONCHK2);
					}
					else
					{
						m_bIfEnableTBSetUL = TRUE;
						m_bIfEnableTBSetLR = TRUE;
					}	

					m_bIfEnablePIBlkPitchX = TRUE;
					m_bIfEnablePIBlkPitchY = TRUE;
					m_bIfEnablePIDiePitchX = TRUE;
					m_bIfEnablePIDiePitchY = TRUE;
					m_bIfEnablePIDiePerRow = TRUE;
					m_bIfEnablePIDiePerCol = TRUE;
					m_bIfEnablePIDiePerBlk = TRUE;
					m_bIfEnableChkBAutoAssignGrade = TRUE;
					m_bIfEnableFirstRowColSkipPattern = TRUE;
					m_bIfEnableTeachWithPhysicalBlk = TRUE;
					m_bIfEnablePIGrade = FALSE;
					m_bIfEnableCBWalkPath = TRUE;
					m_bIfEnable2DBarcodeOutput = TRUE;
					m_bIfEnableTemplateSetup = FALSE;
					m_bIfEnableCentralizedBondArea = TRUE;
					m_bIfEnableWafflePadCheckBox = TRUE;
					m_bIfEnablePadPitchSetX = TRUE;
					m_bIfEnablePadPitchSetY = TRUE;
					m_bIfEnablePadPitchResetX = TRUE;
					m_bIfEnablePadPitchResetY = TRUE;
					m_bIfEnablePadDimX = TRUE;
					m_bIfEnablePadDimY = TRUE;
					m_bIfEnableWafflePad = TRUE;
					m_bIfUseBlockCornerAsFirstDiePos = TRUE;
					m_bIfEnableTBSubmit = TRUE;
					m_bIfEnableTBCancel = TRUE;
				}
			}
			else //Block to Setup != 1
			{
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = FALSE;
				m_bIfEnablePIDiePitchY = FALSE;
				m_bIfEnablePIDiePerRow = FALSE;
				m_bIfEnablePIDiePerCol = FALSE;
				m_bIfEnablePIDiePerBlk = FALSE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = FALSE;
				
				m_bIfEnableWafflePad = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;

				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;

				SetAlert(IDS_BT_EDITBLK1DATAONLY);
			}
		}
		else if (m_szSetupMode == "Single Block")
		{
			UCHAR uGrade = m_oBinBlkMain.GrabGrade(stInfo.ulBinBlkId);

			//if (m_oBinBlkMain.GrabNVNoOfBondedDie(stInfo.ulBinBlkId) > 0)
			if (m_oBinBlkMain.GrabNVNoOfSortedDie(uGrade) > 0)
			{
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = FALSE;
				m_bIfEnablePIDiePitchY = FALSE;
				m_bIfEnablePIDiePerRow = FALSE;
				m_bIfEnablePIDiePerCol = FALSE;
				m_bIfEnablePIDiePerBlk = FALSE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = FALSE;
				
				m_bIfEnableWafflePad = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;

				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;	

				//Block settings can't be edited when bin count is not cleared!
				SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
			}
			else
			{
				bIfIsFirstBlkInMerge = 
				(m_oBinBlkMain.GrabBlkUpperLeftX(stInfo.ulBinBlkId) !=
				m_oBinBlkMain.GrabBlkUpperLeftXFromSetup(stInfo.ulBinBlkId)) 
				||
				(m_oBinBlkMain.GrabBlkUpperLeftY(stInfo.ulBinBlkId) != 
				m_oBinBlkMain.GrabBlkUpperLeftYFromSetup(stInfo.ulBinBlkId)) 
				||
				(m_oBinBlkMain.GrabBlkLowerRightX(stInfo.ulBinBlkId) !=
				m_oBinBlkMain.GrabBlkLowerRightXFromSetup(stInfo.ulBinBlkId)) 
				||
				(m_oBinBlkMain.GrabBlkLowerRightY(stInfo.ulBinBlkId) !=
				m_oBinBlkMain.GrabBlkLowerRightYFromSetup(stInfo.ulBinBlkId));

				if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(stInfo.ulBinBlkId))
				{
					m_bIfEnableTBSetUL = FALSE;
					m_bIfEnableTBSetLR = FALSE;

					//Block Dimensions can't be edited when it's disabled from same grade merge.
					//You can only do this if grade is changed.
					SetAlert(IDS_BT_BLKDIMENSIONCHK1);
				}
				else if (bIfIsFirstBlkInMerge)
				{
					m_bIfEnableTBSetUL = FALSE;
					m_bIfEnableTBSetLR = FALSE;

					//Block Dimensions can't be edited if it's the first block in merge!
					SetAlert(IDS_BT_BLKDIMENSIONCHK2);
				}
				else
				{
					m_bIfEnableTBSetUL = TRUE;
					m_bIfEnableTBSetLR = TRUE;
				}	

				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = TRUE;
				m_bIfEnablePIDiePitchY = TRUE;
				m_bIfEnablePIDiePerRow = TRUE;
				m_bIfEnablePIDiePerCol = TRUE;
				m_bIfEnablePIDiePerBlk = TRUE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = TRUE;
				m_bIfEnableCBWalkPath = TRUE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableFirstRowColSkipPattern = TRUE;
				m_bIfEnableTeachWithPhysicalBlk = TRUE;
				m_bIfEnableTemplateSetup = TRUE;
				m_bIfEnableCentralizedBondArea = TRUE;
				m_bIfEnableWafflePad = TRUE;
				m_bIfEnableWafflePadCheckBox = TRUE;
				m_bIfEnablePadPitchSetX = TRUE;
				m_bIfEnablePadPitchSetY = TRUE;
				m_bIfEnablePadPitchResetX = TRUE;
				m_bIfEnablePadPitchResetY = TRUE;
				m_bIfEnablePadDimX = TRUE;
				m_bIfEnablePadDimY = TRUE;
				m_bIfUseBlockCornerAsFirstDiePos = TRUE;
				m_bIfEnableTBSubmit = TRUE;
				m_bIfEnableTBCancel = TRUE;
			}
		}
	}

	return 1;
} //end OnClickBinBlkToSetupInput

//pllm
LONG CBinTable::TeachCircularAreaCenterPos(IPC_CServiceMessage& svMsg)	
{
	BOOL bReturn = TRUE;

	SetJoystickOn(FALSE);
	Sleep(100);
	LONG lX = m_oBinBlkMain.GrabCircularBinCenterX(1);
	LONG lY = m_oBinBlkMain.GrabCircularBinCenterY(1);

#ifdef NU_MOTION
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
#endif

	XY_MoveTo(lX, lY);

#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	Sleep(200);

	SetJoystickOn(TRUE);

	LONG lStatus = HmiMessage("Please locate bin area center position with joystick, then press OK to continue....", 
						"Teach Circular Bin Area", 
						glHMI_MBX_OKCANCEL);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001

	if (lStatus == glHMI_CANCEL)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);
	Sleep(500);

	GetEncoderValue();
	LONG lCenterX = m_lEnc_X;
	LONG lCenterY = m_lEnc_Y;
	LONG lRadiusInUm = (LONG) (m_dCirAreaRadius * 1000.0);
	LONG lRadius = labs(ConvertFileUnitToXEncoderValue(lRadiusInUm));		// from um to encoder step

	//** Circular Area disabled (FALSE) until SUBMIT is pressed in BinBlk setup	**//
	m_oBinBlkMain.SetUseCircularBinArea(0, FALSE, lCenterX, lCenterY, lRadius);

	m_lCirAreaCenterX	= lCenterX;
	m_lCirAreaCenterY	= lCenterY;

//CString szTemp;
//szTemp.Format("3.  %.1f, Um=%d, Enc=%d", m_dCirAreaRadius, lRadiusInUm, lRadius);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);		

	SaveBinTableData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::UpdateBondAreaOffset(IPC_CServiceMessage &svMsg)	//v4.42T9	//Citizen
{
	BOOL bReturn = TRUE;

	typedef struct {
		DOUBLE	dOffsetX;
		DOUBLE	dOffsetY;
	} BOND_AREA_OFFSET;

	BOND_AREA_OFFSET stData;
	svMsg.GetMsg(sizeof(BOND_AREA_OFFSET), &stData);

	LONG lOffsetX = (LONG)stData.dOffsetX;
	LONG lOffsetY = (LONG)stData.dOffsetY;

	DOUBLE dRemainX = stData.dOffsetX - lOffsetX;
	DOUBLE dRemainY = stData.dOffsetY - lOffsetY;

	if (dRemainX == 0)
	{
	}
	else if (dRemainX >= 0.5)
	{
		m_dBondAreaOffsetXInUm = 0.5 + lOffsetX;
	}
	else
	{
		m_dBondAreaOffsetXInUm = lOffsetX;
	}

	if (dRemainY == 0)
	{
	}
	else if (dRemainY >= 0.5)
	{
		m_dBondAreaOffsetYInUm = 0.5 + lOffsetY;
	}
	else
	{
		m_dBondAreaOffsetYInUm = lOffsetY;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


/////////////////////////////////
//    All Bin Blocks Setup     //
/////////////////////////////////

LONG CBinTable::IfAllBinBlksSetupIsAllowed(IPC_CServiceMessage& svMsg)
{
	ULONG i;
	char *pBuffer;
	CString szSetupMode;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szSetupMode = &pBuffer[0];

	delete [] pBuffer;

	if (szSetupMode == "All Blocks")
	{
		if (m_ulNoOfBinBlk > 0)
		{
			m_ulBinBlkToSetup = 1;
		}
		m_bIfEnableChkBAutoAssignGrade = TRUE;
		m_bIfEnablePIGrade = FALSE;
		m_bIfEnablePIBlkPitchX = TRUE;
		m_bIfEnablePIBlkPitchY = TRUE;

		for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
			{
				SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);

				m_bIfEnablePINoOfBlk = FALSE;
				m_bIfEnablePIBlkToSetup = FALSE;

				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = FALSE;
				m_bIfEnablePIDiePitchY = FALSE;
				m_bIfEnablePIDiePerRow = FALSE;
				m_bIfEnablePIDiePerCol = FALSE;
				m_bIfEnablePIDiePerBlk = FALSE;			
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = FALSE;
				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
				
				m_bIfEnableTBCancel = FALSE;

				m_bIfEnableWafflePad = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;
				

				break;
			}
			else if (szSetupMode == "Single Block")
			{
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = TRUE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnableTemplateSetup = TRUE;
				m_bIfEnableCentralizedBondArea = FALSE;
				m_bIfEnableWafflePad = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;
				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
			}
		}
	}

	return 1;
} //end IfAllBinBlksSetupIsAllowed


///////////////////////////////////
//    Single Bin Block Setup     //
///////////////////////////////////

LONG CBinTable::IfBondingInProcess(IPC_CServiceMessage& svMsg)
{
	if (m_oBinBlkMain.GrabNVNoOfBondedDie(m_ulBinBlkToSetup) > 0)
	{
		SetAlert(IDS_BT_CHKIFBINCNTISCLR);

		m_bReply = FALSE;
	}
	else
	{
		m_bReply = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &m_bReply);

	return 1;
} //end IfBondingInProcess


LONG CBinTable::IfDisabledFromSameGradeMerge(IPC_CServiceMessage& svMsg)
{
	ULONG ulBlkId;
	BOOL bIfIsFirstBlkInMerge;

	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);

	bIfIsFirstBlkInMerge = 
		(m_oBinBlkMain.GrabBlkUpperLeftX(ulBlkId) !=
		m_oBinBlkMain.GrabBlkUpperLeftXFromSetup(ulBlkId)) 
		||
		(m_oBinBlkMain.GrabBlkUpperLeftY(ulBlkId) != 
		m_oBinBlkMain.GrabBlkUpperLeftYFromSetup(ulBlkId)) 
		||
		(m_oBinBlkMain.GrabBlkLowerRightX(ulBlkId) !=
		m_oBinBlkMain.GrabBlkLowerRightXFromSetup(ulBlkId)) 
		||
		(m_oBinBlkMain.GrabBlkLowerRightY(ulBlkId) !=
		m_oBinBlkMain.GrabBlkLowerRightYFromSetup(ulBlkId));

	m_bIfEnableTBSetUL = FALSE;
	m_bIfEnableTBSetLR = FALSE;

	if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(ulBlkId))
	{
		SetAlert(IDS_BT_BLKDIMENSIONCHK1);
	}
	else if (bIfIsFirstBlkInMerge)
	{
		SetAlert(IDS_BT_BLKDIMENSIONCHK2);
	}
	else
	{
		m_bIfEnableTBSetUL = TRUE;
		m_bIfEnableTBSetLR = TRUE;
	}

	return 1;
} //end IfDisabledFromSameGradeMerge


//////////////////////////////////////////////////
//    Clear Bin Counter (by Physical Block)     //
//////////////////////////////////////////////////
LONG CBinTable::ClrPhyBinCtrPreRoutine(IPC_CServiceMessage& svMsg)
{
	m_ulNoOfPhyBlk = m_oPhyBlkMain.GetNoOfBlk();
	svMsg.InitMessage(sizeof(ULONG), &m_ulNoOfPhyBlk);
	return 1;
} //end ClrPhyBinCtrPreRoutine

  
LONG CBinTable::ClrPhyBinCtr(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	int nConvID;
	LONG lSPCBin = 0;

	CString szTemp;
	CString szTitle, szContent;
	ULONG ulPhyBlkToClear;
	LONG lHmiMsgReply = 3;


	szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
	szContent.LoadString(HMB_GENERAL_AREUSURE);

	lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); 

	if (lHmiMsgReply == 3) //3 = Yes
	{
		svMsg.GetMsg(sizeof(ULONG), &ulPhyBlkToClear);

		m_oBinBlkMain.ClearBinCounterByPhyBlk(ulPhyBlkToClear);
		//m_oBinBlkMain.UpdateSerialNoGivenPhyBlkId(ulPhyBlkToClear);
		m_oBinBlkMain.UpdateSerialNo(ulPhyBlkToClear, '0' , CLEAR_BIN_BY_PHY_BLK, 
			m_bUseMultipleSerialCounter);

		szTemp.Format("Physical Block %d count is cleared", ulPhyBlkToClear); 
		SetStatusMessage(szTemp);

		CMSLogFileUtility::Instance()->BT_BinSerialLog("UpdateSerialNo - ClrPhyBinCtr");

		szTemp.LoadString(HMB_BT_PHY_BLK_COUNT_CLEARED);
		HmiMessage(szContent, szTitle);

		//clear SPC data
		if (CMS896AStn::m_bUsePostBond == TRUE)
		{
			lSPCBin = (LONG)ulPhyBlkToClear;
			stMsg.InitMessage(sizeof(LONG), &lSPCBin);
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeData", stMsg);
			m_comClient.ScanReplyForConvID(nConvID, 5000);
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
		}
	}
	return 1;
} //end ClrPhyBinCtr


// 
LONG CBinTable::OnChangeClrBinCntModeSelection(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szClrBinCntMode;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szClrBinCntMode = &pBuffer[0];

	delete [] pBuffer;

	if (szClrBinCntMode == "Single Block")
	{
		m_bIfIsClrAllBinCtr = FALSE;
	}
	else if (szClrBinCntMode == "All Blocks")
	{
		m_bIfIsClrAllBinCtr = TRUE;
	}
	else if (szClrBinCntMode == "Single Grade")
	{
		m_bIsClrAllGrades = FALSE;
	}
	else
	{
		m_bIsClrAllGrades = TRUE;
	}

	return 1;
} //end OnChangeClrBinCntModeSelection




LONG CBinTable::InputOutputFilePath(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szOutputFilePath, szTempFilePath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szOutputFilePath = &pBuffer[0];
	szTempFilePath = szOutputFilePath; 	

	delete [] pBuffer;


	//szTempFilePath.Replace("\\", "\\\\");
	if (CreateDirectory(szTempFilePath, NULL) == 0 )
	{
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			m_szOutputFilePath = szOutputFilePath;
			bReply = TRUE;
		}
		else
		{
			CString szContent;

			szContent.LoadString(HMB_GENERAL_INVALID_PATH);

			if (szTempFilePath != "s:\\" && szTempFilePath != "S:\\")
			{
				HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);			
				m_szOutputFilePath = (*m_psmfSRam)["MS896A"]["OutputFilePath"];
			}
			bReply = FALSE;
		}
	}
	else
	{
		m_szOutputFilePath = szOutputFilePath;
		bReply = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 

LONG CBinTable::InputOutputFilePath2(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szOutputFilePath, szTempFilePath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szOutputFilePath = &pBuffer[0];
	szTempFilePath = szOutputFilePath; 	

	delete [] pBuffer;

	if (CreateDirectory(szTempFilePath, NULL) == 0)
	{
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			m_szOutputFilePath2 = szOutputFilePath;
			(*m_psmfSRam)["MS896A"]["OutputFilePath2"] = m_szOutputFilePath2;
			bReply = TRUE;
		}
		else
		{
			CString szContent;

			szContent.LoadString(HMB_GENERAL_INVALID_PATH);
			(*m_psmfSRam)["MS896A"]["OutputFilePath2"] = m_szOutputFilePath2;
			HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);			
			m_szOutputFilePath2 = "";
			bReply = FALSE;
		}
	}
	else
	{
		m_szOutputFilePath2 = szOutputFilePath;
		(*m_psmfSRam)["MS896A"]["OutputFilePath2"] = m_szOutputFilePath2;
		bReply = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 

LONG CBinTable::SaveOutputFilePath2(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = FALSE;

	SaveBinTableData();

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end 

LONG CBinTable::CheckPhyBlkID(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;

	//SaveBinTableData();
	if (m_ulPhyBlkToClrCount < 1)
		m_ulPhyBlkToClrCount = 1;

	//enum // operation mode
	//{
	//	BL_MODE_A=0,
	//	BL_MODE_B,
	//	BL_MODE_C,
	//	BL_MODE_D,
	//	BL_MODE_E,			//125 bin config			//v3.45
	//	BL_MODE_F,			//MS100 8mag 4x2 150bins config		//v3.82
	//	BL_MODE_G			//MS100 8mag 4x2 175bins config		//v3.82
	//};

	LONG lOM = (LONG)(*m_psmfSRam)["BinLoader"]["OM"];

#ifdef NU_MOTION
	if (lOM == 7)	//BL_MODE_H		//Yearly MS100Plus with 4mag config		//v4.31T10
	{
		if (m_ulPhyBlkToClrCount > 100)
			m_ulPhyBlkToClrCount = 100;
	}
	if (lOM == 6)	//BL_MODE_G
	{
		if (m_ulPhyBlkToClrCount > 175)
			m_ulPhyBlkToClrCount = 175;
	}
	else
	{
		if (m_ulPhyBlkToClrCount > BT_MAX_BINBLK_NO)
			m_ulPhyBlkToClrCount = BT_MAX_BINBLK_NO;
	}
#else
	if (lOM == 4)	//BL_MODE_E
	{
		if (m_ulPhyBlkToClrCount > 125)
			m_ulPhyBlkToClrCount = 125;
	}
	else
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( lOM==3 && pApp->GetCustomerName()=="Cree" )			//Cree Mode D 
		{
		}
		else if( lOM==2 && pApp->GetCustomerName()=="Cree" )	//Cree Mode C	//v4.37T11
		{
		}
		else
		{
			if (m_ulPhyBlkToClrCount > 100)
				m_ulPhyBlkToClrCount = 100;
		}
	}
#endif

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::GetOutputFilePath2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp != NULL )
	{
		pApp->GetPath(m_szOutputFilePath2);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::GetOutputFilePath3(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp != NULL )
	{
		pApp->GetPath(m_szOutputFilePath3);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::GetOutputFilePath4(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp != NULL )
	{
		pApp->GetPath(m_szOutputFilePath4);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::InputOutputFileFormat(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szOutputFileFormat;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szOutputFileFormat = &pBuffer[0];

	delete [] pBuffer;

	m_szBinOutputFileFormat = szOutputFileFormat;
	bReply = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}


LONG CBinTable::SaveOutputFilePath(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szOutputFilePath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szOutputFilePath = &pBuffer[0];

	delete [] pBuffer;

	//Update SRAM contents
	(*m_psmfSRam)["MS896A"]["OutputFilePath"] = szOutputFilePath;
	
	SaveBinTableData();

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end 


LONG CBinTable::SaveOutputFileFormat(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szOutputFileFormat;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szOutputFileFormat = &pBuffer[0];

	delete [] pBuffer;

	//Update SRAM contents
	(*m_psmfSRam)["MS896A"]["OutputFileFormat"] = szOutputFileFormat;

	SaveBinTableData();

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}


LONG CBinTable::SaveOutputFileBlueTapeNoFormat(IPC_CServiceMessage& svMsg)  // v4.51D10 Dicon
{
	char *pBuffer;
	CString szOutputFileBlueTapeNo;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szOutputFileBlueTapeNo = &pBuffer[0];

	delete [] pBuffer;

	//Update SRAM contents
	(*m_psmfSRam)["MS896A"]["OutputFileBlueTapeNo"] = szOutputFileBlueTapeNo;
	m_szOutputFileBlueTapeNo						= szOutputFileBlueTapeNo;
	SaveBinTableData();

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}


LONG CBinTable::InputWaferEndFilePath(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szWaferEndPath, szTempFilePath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szWaferEndPath = &pBuffer[0];
	szTempFilePath = szWaferEndPath; 	

	delete [] pBuffer;


	//szTempFilePath.Replace("\\", "\\\\");
	if (CreateDirectory(szTempFilePath, NULL) == 0)
	{
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			m_szWaferEndPath = szWaferEndPath;
			bReply = TRUE;
		}
		else
		{
			CString szContent;

			szContent.LoadString(HMB_GENERAL_INVALID_PATH);

			HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);			
			m_szWaferEndPath = (*m_psmfSRam)["MS896A"]["WaferEndFilePath"];
			bReply = FALSE;
		}
	}
	else
	{
		m_szWaferEndPath = szWaferEndPath;
		bReply = TRUE;
	}


	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} 


LONG CBinTable::InputWaferEndFileFormat(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szWaferEndFileFormat;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szWaferEndFileFormat = &pBuffer[0];

	delete [] pBuffer;

	m_szWaferEndFileFormat = szWaferEndFileFormat;

	bReply = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}


LONG CBinTable::SaveWaferEndFilePath(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szWaferEndFilePath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szWaferEndFilePath = &pBuffer[0];

	delete [] pBuffer;

	//Update SRAM contents
	(*m_psmfSRam)["MS896A"]["WaferEndFilePath"] = szWaferEndFilePath;

	SaveBinTableData();

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

    // 3411
    SetGemValue("AB_OutputFilePath", szWaferEndFilePath);
    // 7404
    SendEvent(SG_CEID_BT_OUTPUT, FALSE);

	return 1;
} //end 


LONG CBinTable::SaveWaferEndFileFormat(IPC_CServiceMessage& svMsg)
{
	char *pBuffer;
	CString szWaferEndFileFormat;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szWaferEndFileFormat = &pBuffer[0];

	delete [] pBuffer;

	//Update SRAM contents
	(*m_psmfSRam)["MS896A"]["WaferEndFileFormat"] = szWaferEndFileFormat;

	SaveBinTableData();

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end SaveWaferEndFileFormat

LONG CBinTable::InputBinSummaryOutputPath(IPC_CServiceMessage& svMsg)	
{
	char *pBuffer;
	CString szBinSummaryOutputPath, szTempFilePath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szBinSummaryOutputPath = &pBuffer[0];
	szTempFilePath = szBinSummaryOutputPath; 	

	delete [] pBuffer;

	if (CreateDirectory(szTempFilePath, NULL) == 0)
	{
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			m_szBinSummaryOutputPath = szBinSummaryOutputPath;
			bReply = TRUE;
		}
		else
		{
			CString szContent;

			szContent.LoadString(HMB_GENERAL_INVALID_PATH);

			HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);			
			m_szBinSummaryOutputPath = (*m_psmfSRam)["MS896A"]["BinSummaryOutputPath"];
			bReply = FALSE;
		}
	}
	else
	{
		m_szBinSummaryOutputPath = szBinSummaryOutputPath;
		bReply = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end InputBinSummaryOutputPath

LONG CBinTable::SaveBinSummaryOutputPath(IPC_CServiceMessage& svMsg)	
{
	char *pBuffer;
	CString szBinSummaryOutputPath;
	BOOL bReply = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szBinSummaryOutputPath = &pBuffer[0];

	delete [] pBuffer;

	//Update SRAM contents
	(*m_psmfSRam)["MS896A"]["BinSummaryOutputPath"] = szBinSummaryOutputPath;
	
	SaveBinTableData();

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end SaveBinSummaryOutputPath


LONG CBinTable::CheckIfPhyBlkIsSetup(IPC_CServiceMessage& svMsg)
{
	ULONG ulPhyBlkId;
	BOOL bIsSetup;

	svMsg.GetMsg(sizeof(ULONG), &ulPhyBlkId);
	bIsSetup = m_oPhyBlkMain.GrabIsSetup(ulPhyBlkId);

	svMsg.InitMessage(sizeof(BOOL), &bIsSetup);
	
	return 1;
} //end CheckIfPhyBlkIsSetup

LONG CBinTable::SubmitGenerateSummaryFile(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	//int nConvID;
	char *pBuffer;
	CString szBinOutputFileFormat;
	CString szTitle, szContent;

	LONG lSPCBin = 0;
	ULONG ulClearCount = 0;

	//LONG lHmiMsgReply, lHmiMsgReply2;
	ULONG ulPhyBlkId, i;
	//BOOL bFcnRtn;
	BOOL bReply = TRUE;
	CString szMsg;
	CString ulBlkId;
	INT nMESReply = 0;

	BOOL bNeedToClear = FALSE;		//v4.14T1

	BOOL bGenOutputFile = m_bIfGenOutputFile;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Cree")
	{
		m_bIfGenOutputFile = TRUE;
	}

	BOOL bSemitekClrBin = FALSE;
	if( pApp->GetCustomerName()=="Semitek" )
	{
		bSemitekClrBin = TRUE;
	}

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulPhyBlkId, pBuffer, sizeof(ULONG));
	szBinOutputFileFormat = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	StartLoadingAlert();

	for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
	{
		BOOL bNeedToClear = FALSE;

		if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
		{
			if ( ClrBinCntCmdSubTasks(i, "Single Block", szBinOutputFileFormat, TRUE, FALSE, bNeedToClear, FALSE, TRUE) == FALSE)
			{
				CloseLoadingAlert();
				bReply = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReply);
				return 1;
			}
		}
	}

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::SubmitClrPhyBinCtr(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	int nConvID;
	char *pBuffer;
	CString szBinOutputFileFormat;
	CString szTitle, szContent;

	LONG lSPCBin = 0;
	ULONG ulClearCount = 0;

	LONG lHmiMsgReply=0, lHmiMsgReply2=0;		//v4.46T20	//Klocwork
	ULONG ulPhyBlkId, i;
	BOOL bFcnRtn;
	BOOL bReply = TRUE;
	CString szMsg;
	INT nMESReply = 0;

	BOOL bNeedToClear = FALSE;		//v4.14T1

	BOOL bGenOutputFile = m_bIfGenOutputFile;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Cree")	
	{
		m_bIfGenOutputFile = TRUE;
	}

	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		remove("c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\GetSerialNo.txt");
	}

	if (pApp->GetCustomerName() == "NSS")
	{
		remove("C:\\MapSorter\\UserData\\OutputFile\\ClearBin\\DeviceNumber");
	}

	BOOL bSemitekClrBin = FALSE;
	if (pApp->GetCustomerName() == "Semitek")
	{
		bSemitekClrBin = TRUE;
	}

	if ( m_bClearBinInCopyTempFileMode )
	{
		m_bIfGenOutputFile = FALSE;
	}

	if( m_bEnableOsramBinMixMap)
	{
		(*m_psmfSRam)["BinOutputFile"]["Osram Bin Mix Map Need Unload"] = 0;
	}

	szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);

	szContent.LoadString(HMB_GENERAL_AREUSURE);
	lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); 

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	memcpy(&ulPhyBlkId, pBuffer, sizeof(ULONG));
	szBinOutputFileFormat = &pBuffer[sizeof(ULONG)];

	delete [] pBuffer;

	// Tester Use
	CString szBLBarcodeData;
	CString szTempFileBackupPath = "C:\\MapSorter\\UserData\\Output File Backup\\";
	CString szBackupFilename;

	if (m_bClearBinInCopyTempFileMode)
	{
		szBLBarcodeData = GetBLBarcodeData(ulPhyBlkId);
		if ( szBLBarcodeData.GetLength() == 0 )
		{
			szBLBarcodeData.Format( "%d", ulPhyBlkId);
		}
	}

	if (lHmiMsgReply == 3) //3 = Yes
	{
		//v3.55
		CString szLog;
		szLog.Format("Manual-Clear-Bin #%d: GENFILE=%d  ", ulPhyBlkId, m_bIfGenOutputFile);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog + m_szClrBinCntMode);

		if (!m_bIfGenOutputFile && !m_bClearBinInCopyTempFileMode)
		{
			szContent.LoadString(HMB_BT_CONFIRM_NO_OUTPUT_FILE);
			lHmiMsgReply2 = HmiMessage(szContent, szTitle, 103, 3);
		}
		else if (m_bClearBinInCopyTempFileMode)
		{
			lHmiMsgReply2 = 3;

			//AfxMessageBox("Come in!");
			if ( _access( szTempFileBackupPath, 0) == -1 )
			{
				CreateDirectory(szTempFileBackupPath, NULL);
			}

			if ( _access( szTempFileBackupPath + szBLBarcodeData, 0) == -1 )
			{
				CreateDirectory(szTempFileBackupPath + szBLBarcodeData, NULL);
			}

			WIN32_FIND_DATA FileData;
			CString szFilename;
			HANDLE hSearch = FindFirstFile( gszROOT_DIRECTORY + "\\Exe\\Blk*TempFile.csv", &FileData);
			if ( hSearch != INVALID_HANDLE_VALUE ) 
			{ 
				do 
				{
					szFilename = gszROOT_DIRECTORY + "\\Exe\\" + FileData.cFileName;
					szBackupFilename = szTempFileBackupPath + szBLBarcodeData + "\\" + FileData.cFileName;
					CopyFile(szFilename, szBackupFilename, FALSE);
				}
				while (FindNextFile(hSearch, &FileData) == TRUE);
			}
		}

		/*
		if (!m_bIfGenOutputFile)
		{
			szContent.LoadString(HMB_BT_CONFIRM_NO_OUTPUT_FILE);
			lHmiMsgReply2 = HmiMessage(szContent, szTitle, 103, 3);
		}
		*/

		if (m_bIfGenOutputFile || lHmiMsgReply2 == 3) //3 = Yes
		{
			m_bUseBackupPath = FALSE;

			//Test the Path is invalid or not
			if( (m_bIfGenOutputFile == TRUE) && m_bAutoUploadOutputFile==FALSE )
			{
				if (!IsPathExist(m_szOutputFilePath))
				{
					szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);
					lHmiMsgReply = HmiMessage_Red_Back(szContent, szTitle, 103, 3);
					if( bSemitekClrBin )
						lHmiMsgReply = glHMI_YES;
					
					if (lHmiMsgReply != glHMI_YES)
					{
						szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);

						HmiMessage(szContent, szTitle);
						bReply = TRUE;
						svMsg.InitMessage(sizeof(BOOL), &bReply);
						m_bIfGenOutputFile = bGenOutputFile;
						return 1;
					}

					m_bUseBackupPath = TRUE;
				}
			}

			if (m_bIfGenOutputFile == TRUE)
			{
				m_bValidOutputFilePath2 = FALSE;
				if( m_bEnableBinOutputFilePath2 )
				{
					if (IsPathExist(m_szOutputFilePath2))
					{
						m_bValidOutputFilePath2 = TRUE;
					}
					/*	//v4.48A10	//WH SanAn
					else
					{
						szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);
						szContent += "\nFor bin output file path2!";
						lHmiMsgReply = HmiMessage_Red_Back(szContent, szTitle, 103, 3);
						if( bSemitekClrBin )
							lHmiMsgReply = glHMI_YES;
						if( lHmiMsgReply!=glHMI_YES )
						{
							szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);
							HmiMessage(szContent, szTitle);
							CMSLogFileUtility::Instance()->BL_LogStatus("BT: SubmitClr PhyBinCtr path2 invalid and stop");
							bReply = TRUE;
							svMsg.InitMessage(sizeof(BOOL), &bReply);
							m_bIfGenOutputFile = bGenOutputFile;
							return 1;
						}
						CMSLogFileUtility::Instance()->BL_LogStatus("BT: SubmitClr PhyBinCtr path2 need to use BACKUP path");
					}*/
				}
			}


			if (szBinOutputFileFormat.IsEmpty() == TRUE)
			{
				szContent.LoadString(HMB_BT_NO_OUTPUT_FORMAT);

				HmiMessage(szContent, szTitle);
				bReply = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReply);
				m_bIfGenOutputFile = bGenOutputFile;
				return 1;
			}


			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			BOOL bBinLoader = IsBLEnable();
			if (bBinLoader)		//&& (m_szClrBinCntMode == "Single Block") )	//v4.48A11
			{
				//v4.50A21	//Thi fcn now triggered on HMI as IPC cmd call
				//v4.50A17	//Osram Germany
				/*
				if ( (pApp->GetCustomerName() == "OSRAM") && (m_szClrBinCntMode == "All Blocks") )
				{
					//Unload frame on BT1
					BOOL bStatus = TRUE;
					nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "ManualUnloadFilmFrame12", stMsg);
					while (1)
					{
						if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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
					if (!bStatus)
					{
					}

					//v4.50A17	//Osram Germany
					nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "OSRAMMagazineUnload", stMsg);
					while (1)
					{
						if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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
				}*/

				BOOL bFrameExist = FALSE;
				stMsg.InitMessage(sizeof(BOOL), &bFrameExist);
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

				if (bFrameExist)
				{
					HmiMessage_Red_Yellow("Frame exists on bin table; please unload frame and try again!");
					SetStatusMessage("Manual ClearSingleBin frame-exist detected and aborted!");
					SetErrorMessage("Manual ClearSingleBin frame-exist detected and aborted!");

					bReply = TRUE;
					svMsg.InitMessage(sizeof(BOOL), &bReply);
					m_bIfGenOutputFile = bGenOutputFile;
					return 1;
				}
			}

			if (m_szClrBinCntMode == "Single Block")
			{
				//SetProgressBarLimit(3);
				//UpdateProgressBar(1);
				//v3.94	//PLLM Lumiramic flat file support
				//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if ( (pApp->GetCustomerName() == "Lumileds") )	
					//&& (CMS896AStn::m_lOTraceabilityFormat > 0) )		//v4.44A4
				{
					//ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
					//if (nPLLM == PLLM_LUMIRAMIC)
					AddPLLMFlatFileEntry(ulPhyBlkId);
				}

				bFcnRtn = ClrBinCntCmdSubTasks(ulPhyBlkId, "Single Block", szBinOutputFileFormat, TRUE, FALSE, bNeedToClear);

				if (pApp->GetCustomerName() == "JianSen")
				{
					int nJSTemp = ulPhyBlkId;
					CString szJSTemp;
					szJSTemp.Format("%d",nJSTemp);
					remove( "c:\\MapSorter\\UserData\\SerialNo\\" + szJSTemp + "_SerialNo.txt");
				}

				//UpdateProgressBar(2);
				//v2.83T65	//Reset barcode	//PLLM		//v3.68T1
				BOOL bBinLoader = IsBLEnable();
				if (bFcnRtn && bBinLoader )
				{
					if( bNeedToClear && !m_bClearBinInCopyTempFileMode )		//v4.14T1
					{
						bReply = FALSE;
						ULONG ulPhyBlkID = ulPhyBlkId;
						stMsg.InitMessage(sizeof(ULONG), &ulPhyBlkID);
						nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "ResetSlotBarcode", stMsg);
						while (1)
						{
							if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
							{
								m_comClient.ReadReplyForConvID(nConvID,stMsg);
								stMsg.GetMsg(sizeof(BOOL), &bReply);
								break;
							}
							else
							{
								Sleep(10);
							}
						}

						CString szLog = "--";
						if (bReply)
							szLog.Format("Phy Blk #%lu barcode is reset in Manual ClearSingleBin", ulPhyBlkId);
						else
							szLog.Format("Phy Blk #%lu barcode reset fails in Manual ClearSingleBin", ulPhyBlkId);
						SetStatusMessage(szLog);
					}
					ResetCreeGradeMappingEntry(ulPhyBlkId);	//	CLEAR single block
				}

				//Clear Single SPC data
				if ( (CMS896AStn::m_bUsePostBond == TRUE) && (bFcnRtn == TRUE) )
				{
					lSPCBin = (LONG)ulPhyBlkId;
					stMsg.InitMessage(sizeof(LONG), &lSPCBin);
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeData", stMsg);
					while (1)
					{
						if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
						{
							m_comClient.ReadReplyForConvID(nConvID,stMsg);
							stMsg.GetMsg(sizeof(BOOL), &bReply);
							break;
						}
						else
						{
							Sleep(10);
						}
					}
				}

				if ( bFcnRtn == TRUE )
				{
					CString szBinNo, szMsg;
					szBinNo.Format("%d", ulPhyBlkId);
					SetStatusMessage("Bin " + szBinNo + " Counter is cleared");
				}

				//UpdateProgressBar(3);

				//v2.99T1
				CString szTemp;
				szTemp.Format("Manual Clear Bin #%d: Count = %d", ulPhyBlkId, m_oBinBlkMain.GrabNVNoOfBondedDie(ulPhyBlkId));
				SetErrorMessage(szTemp);
			}
			else if (m_szClrBinCntMode == "All Blocks")
			{
				//v2.70
				//Offline print label only available in Manual Clear-All-Bins fcn for DLA
				if (m_bDisableClearAllPrintLabel)
					CMS896AStn::m_bOfflinePrintLabel = TRUE;
				
				StartLoadingAlert();	//v3.86

				//v4.50A16	//Cree HZ //EndLot clears current map in map memory 
				// in order to reset grade mapping
				if ( IsMapDetectSkipMode() )	//Dynamic grademapping is enabled
				{
					if ((m_WaferMapWrapper.GetReader() != NULL) && m_WaferMapWrapper.IsMapValid())
					{
						CMSLogFileUtility::Instance()->MS_LogOperation("Clear-All-Bins (Cree): clear map before Reset-Grade-Mapping");
						m_WaferMapWrapper.InitMap();
					}
				}

				for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
				{
					BOOL bNeedToClear = FALSE;

					//v3.94	//PLLM Lumiramic flat file support
					if ( (pApp->GetCustomerName() == "Lumileds") )	
						//&& (CMS896AStn::m_lOTraceabilityFormat > 0) )		//v4.44A4
					{
						//ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
						//if (nPLLM == PLLM_LUMIRAMIC)
						AddPLLMFlatFileEntry(i);
					}

					if ( ClrBinCntCmdSubTasks(i, "All Blocks", szBinOutputFileFormat, TRUE, FALSE, bNeedToClear) == TRUE)
					{
						ulClearCount++;
						if (IsBLEnable() && bNeedToClear)	//v4.14T1
						{
							if ( !m_bClearBinInCopyTempFileMode)
							{
								bReply = FALSE;
								ULONG ulPhyBlkID = i;
								stMsg.InitMessage(sizeof(ULONG), &ulPhyBlkID);
								nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "ResetSlotBarcode", stMsg);
								while (1)
								{
									if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
									{
										m_comClient.ReadReplyForConvID(nConvID,stMsg);
										stMsg.GetMsg(sizeof(BOOL), &bReply);
										break;
									}
									else
									{
										Sleep(10);
									}
								}

								CString szLog = "--";
								if (bReply)
									szLog.Format("Phy Blk #%lu barcode is reset in Manual Clear all bins", i);		//v4.13T5
								else
									szLog.Format("Phy Blk #%lu barcode reset fails in Manual Clear all bins", i);	//v4.13T5
								SetStatusMessage(szLog);
							}
						}

						//v4.50A12	//Cree HuiZHou grademapping fcn
						ResetCreeGradeMappingEntry(i, FALSE);	//Reset grade slot automatically, clear all blocks
					}
				}
				CloseLoadingAlert();	//v3.86


				if (pApp->GetCustomerName() == "Cree")		//v4.50A22	
				{
					//Reset Grade Mapping table for Cree HZ
					m_WaferMapWrapper.InitMap();
					m_WaferMapWrapper.ResetGradeMap();
					DeleteFile(MSD_MAP_TABLE_FILE);
					CMSLogFileUtility::Instance()->MS_LogOperation("CREE: Reset GRADE-MAPPING table in Clear-All-Bins");
				}
				else if (pApp->GetCustomerName() == "JianSen")
				{
					//int nJSTemp;
					CString szJSTemp;
					for (int nJSTemp=0; nJSTemp<152; nJSTemp ++)
					{
						szJSTemp.Format("%d",nJSTemp);
						remove( "c:\\MapSorter\\UserData\\SerialNo\\" + szJSTemp + "_SerialNo.txt");
					}
				}

				CMS896AStn::m_bOfflinePrintLabel = FALSE;	//v2.70

				if (ulClearCount == 0)
				{
					szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);

					HmiMessage(szContent, szTitle);
					bFcnRtn = FALSE;
				}
				else if (ulClearCount == m_oPhyBlkMain.GetNoOfBlk())
				{
					szContent.LoadString(HMB_BT_ALL_BINS_COUNTER_CLEARED);

					HmiMessage(szContent, szTitle);
					SetStatusMessage("All Bin Counters are cleared");
					bFcnRtn = TRUE;
				}
				else
				{
					szContent.LoadString(HMB_BT_SOME_BIN_COUNTER_NOT_CLR);

					HmiMessage(szContent, szTitle);
					bFcnRtn = TRUE;
				}

				if ( bFcnRtn == TRUE )
				{
					SetIsAllBinCntCleared(TRUE);
					ResetMapDieType();

					//Clear ALL SPC data
					if ( CMS896AStn::m_bUsePostBond == TRUE )
					{
						nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeAllData", stMsg);
						while (1)
						{
							if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
							{
								m_comClient.ReadReplyForConvID(nConvID,stMsg);
								stMsg.GetMsg(sizeof(BOOL), &bReply);
								break;
							}
							else
							{
								Sleep(10);
							}
						}
					}

					CopyMagazineSummary();	//v4.43T8	//Epigap
				}

				// Reset ExArm PreLoad flags after CLEAR-ALL	//v2.67
				m_bPreLoadFullFrame = FALSE;	
				m_bPreLoadChangeFrame = FALSE;
			}

			//SetProgressBarLimit(0);
			//UpdateProgressBar(0);
			//SetErrorMessage("Manual Clear All Bins");
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Clear-Bin done");	//v3.55
			bReply = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			m_bIfGenOutputFile = bGenOutputFile;
			return 1;
		}
	}


	if (m_bClearBinInCopyTempFileMode)
	{
		WIN32_FIND_DATA FileData;
		CString szFilename;
		HANDLE hSearch = FindFirstFile( szTempFileBackupPath + szBLBarcodeData + "\\Blk*TempFile.csv", &FileData);

		CString szClrBinOutPath = m_szOutputFilePath;
		if( m_bAutoUploadOutputFile )
			szClrBinOutPath = gszOUTPUT_FILE_TEMP_PATH;
		if (!IsPathExist(szClrBinOutPath))
		{
			CreateDirectory( szClrBinOutPath, NULL);
		}

		if (!IsPathExist(szClrBinOutPath + "\\" + szBLBarcodeData))
		{
			CreateDirectory( szClrBinOutPath + "\\" + szBLBarcodeData, NULL);
		}

		if ( hSearch != INVALID_HANDLE_VALUE ) 
		{ 
			do 
			{
				szBackupFilename = szTempFileBackupPath + szBLBarcodeData + "\\" + FileData.cFileName;
				CString szTempFileOutputPath = szClrBinOutPath + "\\" + szBLBarcodeData + "\\" + FileData.cFileName;
				CopyFile(szBackupFilename, szTempFileOutputPath, FALSE);
			}
			while (FindNextFile(hSearch, &FileData) == TRUE);
		}
		else 
		{
			AfxMessageBox( "Copy file to target location ERROR!", MB_SYSTEMMODAL);
		}
	}


	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	BackupToNVRAM();
	CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Clear-Bin done");	//v3.55
	m_bIfGenOutputFile = bGenOutputFile;
	return 1;

} //end 


/////////////////////////////////////////
//    Clear Bin Counter (by Grade)     //
/////////////////////////////////////////

LONG CBinTable::SubmitClrBinCtrByGrade(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	LONG lHmiMsgReply, lHmiMsgReply2;
	CString szGrade, szBinBlkId, szHmiMsg;
	CString szTitle, szContent;
	UCHAR ucGradeToClear;
	CDWordArray dwaBinBlkIds;
	BOOL bReply = TRUE;
	BOOL bFcnRtn;
	ULONG ulClearCount = 0;
	INT nConvID;

	szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
	szContent.LoadString(HMB_GENERAL_AREUSURE);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bSemitekClrBin = FALSE;
	if( pApp->GetCustomerName()=="Semitek" )
		bSemitekClrBin = TRUE;
	lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); 

	if (lHmiMsgReply == 3) //3 = Yes
	{
		svMsg.GetMsg(sizeof(UCHAR), &ucGradeToClear);
		
		if (m_bGenOFileIfClrByGrade == FALSE)
		{
			szContent.LoadString(HMB_BT_CONFIRM_NO_OUTPUT_FILE);

			lHmiMsgReply2 = HmiMessage(szContent, szTitle, 103, 3);
		}

		if (m_bGenOFileIfClrByGrade || lHmiMsgReply2 == 3) //3 = Yes
		{
			m_bUseBackupPath = FALSE;
			m_bValidOutputFilePath2 = FALSE;

			//Test the Path is invalid or not
			if( (m_bGenOFileIfClrByGrade == TRUE) && m_bAutoUploadOutputFile==FALSE )
			{
				if (!IsPathExist(m_szOutputFilePath))
				{
					szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);

					lHmiMsgReply = HmiMessage_Red_Back(szContent, szTitle, 103, 3);
					if( bSemitekClrBin )
						lHmiMsgReply = glHMI_YES;
					if (lHmiMsgReply != glHMI_YES)
					{
						szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);

						HmiMessage(szContent, szTitle);
						bReply = TRUE;
						svMsg.InitMessage(sizeof(BOOL), &bReply);
						return 1;
					}

					m_bUseBackupPath = TRUE;
				}
			}

			if (m_bGenOFileIfClrByGrade == TRUE)
			{
				if (m_bEnableBinOutputFilePath2)
				{
					if (IsPathExist(m_szOutputFilePath2))
					{
						m_bValidOutputFilePath2 = TRUE;
					}
					/*	//v4.48A10	//WH SanAn
					else
					{
						szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);
						szContent += "\nFor bin output file path2!";
						lHmiMsgReply = HmiMessage_Red_Back(szContent, szTitle, 103, 3);
						if( bSemitekClrBin )
							lHmiMsgReply = glHMI_YES;
						if( lHmiMsgReply!=glHMI_YES )
						{
							szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);
							HmiMessage(szContent, szTitle);
							CMSLogFileUtility::Instance()->BL_LogStatus("BT: SubmitClr BinCtrByGrade path2 invalid and stop");
							bReply = TRUE;
							svMsg.InitMessage(sizeof(BOOL), &bReply);
							return 1;
						}
						CMSLogFileUtility::Instance()->BL_LogStatus("BT: SubmitClr BinCtrByGrade path2 need to use BACKUP path");
					}*/
				}
			}


			if (m_szBinOutputFileFormat.IsEmpty() == TRUE)
			{
				szContent.LoadString(HMB_BT_NO_OUTPUT_FORMAT);

				HmiMessage(szContent, szTitle);
				bReply = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReply);
				return 1;
			}

			if (m_szClrBinByGradeMode == "Single Grade")
			{

				bFcnRtn = ClrBinCntCmdSubTasksByGrade(ucGradeToClear, "Single Grade",
						m_szBinOutputFileFormat, dwaBinBlkIds, TRUE, FALSE);
				
				//Clear Single SPC data
				if ((CMS896AStn::m_bUsePostBond == TRUE) && (bFcnRtn == TRUE) )
				{	
					INT i =0;
					ULONG ulBlkId;
					BOOL bWaitReply = FALSE;

					for (i=0; i<dwaBinBlkIds.GetSize(); i++)
					{
						ulBlkId = dwaBinBlkIds.GetAt(i);
					
						if (i == dwaBinBlkIds.GetSize()-1)
							bWaitReply = TRUE;
						
							ClearSPCData(ulBlkId,bWaitReply,FALSE);
					}
					
				}
				


				if ( bFcnRtn == TRUE )
				{
					CString szGrade;
					szGrade.Format("%d", ucGradeToClear);
					SetStatusMessage("Bin " + szGrade + " Counter is cleared");
				}
			}

			else if (m_szClrBinByGradeMode == "All Grades")
			{
				//v2.70
				//Offline print label only available in Manual Clear-All-Bins fcn for DLA
				if (m_bDisableClearAllPrintLabel)
					CMS896AStn::m_bOfflinePrintLabel = TRUE;

				for (UCHAR ucGrade = 1; ucGrade <= MS_MAX_BIN; ucGrade++)
				{
					if (ClrBinCntCmdSubTasksByGrade(ucGrade, "All Grades", m_szBinOutputFileFormat, dwaBinBlkIds , TRUE, FALSE) == TRUE)
					{
						ulClearCount++;
					}
				}

				CMS896AStn::m_bOfflinePrintLabel = FALSE;	//v2.70

				if (ulClearCount == 0)
				{
					szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);

					HmiMessage(szContent, szTitle);
					bFcnRtn = FALSE;
				}
				else if (ulClearCount == MS_MAX_BIN)
				{
					szContent.LoadString(HMB_BT_ALL_BINS_COUNTER_CLEARED);

					HmiMessage(szContent, szTitle);
					SetStatusMessage("All Bin Counters are cleared");
					bFcnRtn = TRUE;
				}
				else
				{
					szContent.LoadString(HMB_BT_SOME_BIN_COUNTER_NOT_CLR);

					HmiMessage(szContent, szTitle);
					bFcnRtn = TRUE;
				}

				if ( bFcnRtn == TRUE )
				{
					SetIsAllBinCntCleared(TRUE);

					//Clear ALL SPC data
				
					if ( CMS896AStn::m_bUsePostBond == TRUE )
					{
						nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeAllData", stMsg);
						while (1)
						{
							if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
							{
								m_comClient.ReadReplyForConvID(nConvID,stMsg);
								stMsg.GetMsg(sizeof(BOOL), &bReply);
								break;
							}
							else
							{
								Sleep(10);
							}
						}
					}
					
				}

			// Temp to disable Reset ExArm PreLoad flags after CLEAR-ALL for clear by grade	//v2.67
			//m_bPreLoadFullFrame = FALSE;	
			//m_bPreLoadChangeFrame = FALSE;
			}
		}
		else
		{
			bReply = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}
	
	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	
	BackupToNVRAM();
	
	//delete pBlkPtr;

	return 1;

}


LONG CBinTable::ClrBinCtrByGrade(IPC_CServiceMessage& svMsg)
{
	CString szGrade, szBinBlkId, szTitle, szContent;
	UCHAR ucGradeToClear;
	LONG lHmiMsgReply, i;
	CDWordArray dwaBinBlkIds;
	BOOL bIsClearBin = FALSE;


	szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
	szContent.LoadString(HMB_GENERAL_AREUSURE);

	lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); 

	if (lHmiMsgReply == 3) //3 = Yes
	{
		svMsg.GetMsg(sizeof(UCHAR), &ucGradeToClear);

		 bIsClearBin = m_oBinBlkMain.ClearBinCounterByGrade(ucGradeToClear, dwaBinBlkIds);
		
		if (bIsClearBin == TRUE)
		{
			szContent.Format("Grade %d is cleared", ucGradeToClear);
			SetStatusMessage(szContent);

			szGrade.Format("%d", ucGradeToClear);
			szContent = "Grade " + szGrade + " count is cleared!\n\nBlock(s) with Grade "
						+ szGrade + ":\n";

			for (i = 0; i < dwaBinBlkIds.GetSize(); i++)
			{
				szBinBlkId.Format("%d", dwaBinBlkIds.GetAt(i));

				if (i < dwaBinBlkIds.GetSize() - 1)
					szBinBlkId = szBinBlkId + ", ";

				szContent = szContent + szBinBlkId;
			}

			HmiMessage(szContent, szTitle);
		}
		//Klocwork	//v4.04
		//else
		//{
		//	szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);
		//	HmiMessage(szContent, szTitle);
		//}
		

		svMsg.InitMessage(sizeof(BOOL), &bIsClearBin);
	}

	return 1;
} //end ClrBinCtrByGrade




////////////////////////////////////////////////////////////////////
//     Clear Bin Counter (both by Grade and Physical Block)       //
////////////////////////////////////////////////////////////////////
LONG CBinTable::ClrBinCtrPreRoutine(IPC_CServiceMessage& svMsg)
{
	m_ulNoOfPhyBlk = m_oPhyBlkMain.GetNoOfBlk();
	svMsg.InitMessage(sizeof(ULONG), &m_ulNoOfPhyBlk);

	return 1;
} // end ClrBinCtrPreRoutine


//////////////////////////////////////////////////
//    Clear All Bin Counter (For BurnIn Use)    //
//////////////////////////////////////////////////
LONG CBinTable::BurnInClearCounter(IPC_CServiceMessage& svMsg)
{
	ULONG i;
	BOOL bIfGenOutputFileSuccess = FALSE;

	//Clear Bin Counter
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		m_oBinBlkMain.ClearBinCounterByPhyBlk(i);
		//m_oBinBlkMain.UpdateSerialNoGivenPhyBlkId(i);
	}	

	return 1;
} //end BurnInClearCounter


////////////////////////////////////////////////
//     Clear Bin Counter (for DL machine)     //
////////////////////////////////////////////////
LONG CBinTable::AutoClrBinCnt(IPC_CServiceMessage &svMsg)
{
	ULONG ulPhyBlkId;
	LONG lSPCBin;
	CString szBarcode;
	CString szTitle, szContent;
	BOOL bIsBinCntClred;
	BOOL bIsClearSPCData;
	BOOL bReply = FALSE;
	BOOL bNeedToClear = FALSE;		//v4.14T1
	int nConvID;
	IPC_CServiceMessage stMsg;
	LONG lHmiMsgReply = 3;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bSemitekClrBin = FALSE;
	if (pApp->GetCustomerName() == "Semitek")
	{
		bSemitekClrBin = TRUE;
	}

	svMsg.GetMsg(sizeof(ULONG), &ulPhyBlkId);

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if ((pUtl->GetPrescanDummyMap() || IsEnablePNP()) && m_bEnablePickNPlaceOutputFile==FALSE ||
		pApp->GetCustomerName() == "Knowles")
	{
		//Enable Pick & Place, Do not generate output file
		m_bIfGenOutputFile = FALSE;
		bIsClearSPCData = FALSE;
	}
	else
	{
		if (IsBurnIn() == FALSE)
		{
			//Add generate binblock temp files
			OpGenAllTempFile();
			m_bIfGenOutputFile = TRUE;
			bIsClearSPCData = TRUE;
		}
		else
		{
			m_bIfGenOutputFile = FALSE;
			bIsClearSPCData = FALSE;
			//ulPhyBlkId = 1;
		}
	}

	szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
	m_bUseBackupPath = FALSE;
		
	// Re-enable for 3E DL	//v4.48A2
	//v4.10		//Generate output data to local temp path if un-available
	CString szClrBinOutPath = m_szOutputFilePath;
	if (m_bAutoUploadOutputFile == FALSE && (!IsPathExist(szClrBinOutPath) || (_access(szClrBinOutPath, 2) == -1)))
	{
		szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);
		if( bSemitekClrBin )
		{
			lHmiMsgReply = HmiMessage_Red_Back(szContent, szTitle, 103, 3);
			lHmiMsgReply = glHMI_YES;
		}
		else if ( pApp->GetCustomerName() == "Electech3E(DL)" )		//v4.48A2
		{
			lHmiMsgReply = HmiMessage_Red_Back("Output network path cannot be accessed; bonding cycle is aborted", 
												szTitle, glHMI_MBX_CLOSE, 3);
			lHmiMsgReply = glHMI_NO;
		}
		else
		{
			lHmiMsgReply = HmiMessage_Red_Back(szContent, szTitle, 103, 3);
		}

		if( lHmiMsgReply != glHMI_YES )
		{
			szContent.LoadString(HMB_BT_ONE_BIN_COUNTER_CLEARED);
			SetErrorMessage("Cannot access Output path in AutoClrBinCnt - " + m_szOutputFilePath);

			HmiMessage(szContent, szTitle);
			bReply = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}

		if (m_bAutoUploadOutputFile == FALSE && (!IsPathExist(szClrBinOutPath) || (_access(szClrBinOutPath, 2) == -1)))
		{
			if (m_bEnableBinOutputFilePath2)
			{
				szClrBinOutPath = m_szOutputFilePath2;
			}
		}

		CMSLogFileUtility::Instance()->BL_LogStatus("BT: AutoClrBinCnt 2 need to use BACKUP path");
		if (m_bAutoUploadOutputFile == FALSE && (!IsPathExist(szClrBinOutPath) || (_access(szClrBinOutPath, 2) == -1)))
		{
			//if m_szOutputFilePath2 is not existed, machine will save it into the gszUSER_DIRECTORY + "\\OutputFile\\ClearBin" folder
			m_bUseBackupPath = TRUE;
		}
	}


	if (!m_bAutoUploadOutputFile && !IsPathExist(szClrBinOutPath))
	{
		szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);
		lHmiMsgReply = HmiMessage_Red_Back(szContent, szTitle, 103, 3);
		if (bSemitekClrBin)
		{
			lHmiMsgReply = glHMI_YES;
		}

		if (lHmiMsgReply != glHMI_YES)
		{
			szContent.LoadString(HMB_BT_ONE_BIN_COUNTER_CLEARED);
			SetErrorMessage("Cannot access Output path in AutoClrBinCnt 2 - " + m_szOutputFilePath);

			HmiMessage(szContent, szTitle);
			bReply = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}

		CMSLogFileUtility::Instance()->BL_LogStatus("BT: AutoClrBinCnt 1 need to use BACKUP path");
		m_bUseBackupPath = TRUE;
	}

	m_bValidOutputFilePath2 = FALSE;
	if (m_bEnableBinOutputFilePath2)
	{
		if (IsPathExist(m_szOutputFilePath2) && (_access(m_szOutputFilePath2, 2) != -1))
		{
			m_bValidOutputFilePath2 = TRUE;
		}
		else
		{
			szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);
			szContent += "\nFor bin output file path2!";
			lHmiMsgReply = HmiMessage_Red_Back(szContent, szTitle, 103, 3);
			if (bSemitekClrBin)
			{
				lHmiMsgReply = glHMI_YES;
			}
			if (lHmiMsgReply != glHMI_YES)
			{
				szContent.LoadString(HMB_BT_ONE_BIN_COUNTER_CLEARED);
				CMSLogFileUtility::Instance()->BL_LogStatus("BT: AutoClr BinCnt path2 1 invalid and stop");
				HmiMessage(szContent, szTitle);
				bReply = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReply);
				return 1;
			}
			CMSLogFileUtility::Instance()->BL_LogStatus("BT: AutoClr BinCnt path2 1 use BACKUP path");
		}
	}

	//Get SPC data & saved in temp directory
	if (m_bUsePostBond && bIsClearSPCData)
	{
		stMsg.InitMessage(sizeof(ULONG), &ulPhyBlkId);
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "SaveSPCData", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReply);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}

	ULONG ulClearCount = m_oBinBlkMain.GrabNVNoOfBondedDie(ulPhyBlkId);		//v3.27T2

	//v3.94	//PLLM Lumiramic flat file support
	if ((pApp->GetCustomerName() == "Lumileds"))	
		//&& (CMS896AStn::m_lOTraceabilityFormat > 0) )		//v4.44A4
	{
		//ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
		//if (nPLLM == PLLM_LUMIRAMIC)
		AddPLLMFlatFileEntry(ulPhyBlkId);
	}

	//Clear Bin & generate output file if necessary
	bIsBinCntClred = ClrBinCntCmdSubTasks(ulPhyBlkId, "Single Block", m_szBinOutputFileFormat, FALSE, TRUE, bNeedToClear);


	//Clear Single SPC data if necessary
	if (m_bUsePostBond && bIsBinCntClred && bIsClearSPCData)
	{
		lSPCBin = (LONG)ulPhyBlkId;
		stMsg.InitMessage(sizeof(LONG), &lSPCBin);
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeData", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReply);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}

	if ( bIsBinCntClred == TRUE )
	{
		CString szBinNo;
		szBinNo.Format("%d  Counter is cleared (%d)", ulPhyBlkId, ulClearCount);	//v3.27T2	//Include clear count;
		SetStatusMessage("Bin " + szBinNo);
		CMSLogFileUtility::Instance()->BL_LogStatus("Bin " + szBinNo);

		//v4.53A12	//Cree HuiZHou grademapping fcn
		if (State() == IDLE_Q)
		{
			ResetCreeGradeMappingEntry(ulPhyBlkId, FALSE, FALSE);	// auto clear bin	//v4.53A19
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bIsBinCntClred);
	BackupToNVRAM();
	return 1;
} //end AutoClrBinCnt

LONG CBinTable::AutoClrBinCnt_SECSGEM(IPC_CServiceMessage &svMsg)	//v4.59A19
{
	ULONG ulPhyBlkId;
	LONG lSPCBin;
	CString szBarcode;
	CString szTitle, szContent;
	BOOL bIsBinCntClred;
	BOOL bIsClearSPCData;
	BOOL bReply = FALSE;
	BOOL bNeedToClear = FALSE;		
	int nConvID;
	IPC_CServiceMessage stMsg;
	LONG lHmiMsgReply = 3;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	svMsg.GetMsg(sizeof(ULONG), &ulPhyBlkId);

	CString szLog;
	szLog.Format("BT: AutoClrBinCnt_SECSGEM		BLK #%lu", ulPhyBlkId);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	//Add generate binblock temp files
	OpGenAllTempFile();
	m_bIfGenOutputFile	= TRUE;
	bIsClearSPCData		= TRUE;

	//szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
	//m_bUseBackupPath = FALSE;
	//m_bValidOutputFilePath2 = FALSE;


	//Get SPC data & saved in temp directory
	if ( m_bUsePostBond && bIsClearSPCData )
	{
		stMsg.InitMessage(sizeof(ULONG), &ulPhyBlkId);
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "SaveSPCData", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReply);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}

	ULONG ulClearCount = 0;
	ULONG ulCurrCount = 0;
	CString szBinNo;

	//Clear Bin & generate output file if necessary
	if (ulPhyBlkId == 0)
	{
		for (INT i = 1; i <= (INT) m_oPhyBlkMain.GetNoOfBlk(); i++)
		{
			ulCurrCount = m_oBinBlkMain.GrabNVNoOfBondedDie(i);	
			if (ulCurrCount == 0)
			{
				continue;
			}

			ulClearCount = ulClearCount + ulCurrCount;	//OsramTrip 8/22

			bIsBinCntClred = ClrBinCntCmdSubTasks(i, "All Blocks", m_szBinOutputFileFormat, FALSE, TRUE, bNeedToClear);
		
			if ( bIsBinCntClred == TRUE )
			{
				szBinNo.Format("#%lu Counter is cleared (%lu)", ulPhyBlkId, ulClearCount);
				CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\tBin " + szBinNo);
			}
		}

		if (m_bUsePostBond && bIsBinCntClred && bIsClearSPCData)
		{
			lSPCBin = (LONG) 0;
			stMsg.InitMessage(sizeof(LONG), &lSPCBin);
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeData", stMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bReply);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}

		//OsramTrip 8/22

		USHORT usSlotNo			= 0;
		CString szMgzn			= "";
		CString szBarCodeName	= "";

		try
		{
			SetGemValue(MS_SECS_SV_SLOT_NO,				usSlotNo);			//3004
			SetGemValue(MS_SECS_SV_CASSETTE_POS,		szMgzn);			//3005
			SetGemValue(MS_SECS_SV_BT_BIN_NUMBER,		ulPhyBlkId);		//3038
			SetGemValue(MS_SECS_SV_BL_FRAME_ID,			szBarCodeName);		//3042
			SetGemValue(MS_SECS_SV_BT_BIN_DIE_COUNT,	ulClearCount);		//3043

			if (IsMSAutoLineMode())		//OsramTrip 8/22
				SendEvent(SG_CEID_ManualClearBin_AUTOLINE, TRUE);	//CEID: 8022 (Report ID: 4021)
			else
				SendEvent(SG_CEID_ManualClearBin, TRUE);			//CEID: 8020 (Report ID: 4019)
		}
		catch(...)
		{
			DisplaySequence("SG_CEID_ManualClearBin failure");
		}

		SetStatusMessage("ALL Bin Counters are cleared (SECSGEM");
	}
	else
	{
		ulClearCount = m_oBinBlkMain.GrabNVNoOfBondedDie(ulPhyBlkId);	
		bIsBinCntClred = ClrBinCntCmdSubTasks(ulPhyBlkId, "Single Block", m_szBinOutputFileFormat, FALSE, TRUE, bNeedToClear);
	
		//Clear Single SPC data if necessary
		if (m_bUsePostBond && bIsBinCntClred && bIsClearSPCData)
		{
			lSPCBin = (LONG)ulPhyBlkId;
			stMsg.InitMessage(sizeof(LONG), &lSPCBin);
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeData", stMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bReply);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}

		if ( bIsBinCntClred == TRUE )
		{
			szBinNo.Format("#%lu Counter is cleared (%lu)", ulPhyBlkId, ulClearCount);
			SetStatusMessage("Bin " + szBinNo);
			//CMSLogFileUtility::Instance()->BL_LogStatus("Bin " + szBinNo);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\tBin " + szBinNo);

			USHORT usSlotNo			= 0;
			CString szMgzn			= "";
			CString szBarCodeName	= GetBLBarcodeData(ulPhyBlkId);			//OsramTrip 8/22

			try
			{
				SetGemValue(MS_SECS_SV_SLOT_NO,				usSlotNo);			//3004
				SetGemValue(MS_SECS_SV_CASSETTE_POS,		szMgzn);			//3005
				SetGemValue(MS_SECS_SV_BT_BIN_NUMBER,		ulPhyBlkId);		//3038
				SetGemValue(MS_SECS_SV_BL_FRAME_ID,			szBarCodeName);		//3042
				SetGemValue(MS_SECS_SV_BT_BIN_DIE_COUNT,	ulClearCount);		//3043

				if (IsMSAutoLineMode())		//OsramTrip 8/22
					SendEvent(SG_CEID_ManualClearBin_AUTOLINE, TRUE);	//CEID: 8022 (Report ID: 4021)
				else
					SendEvent(SG_CEID_ManualClearBin, TRUE);			//CEID: 8020 (Report ID: 4019)
			}
			catch(...)
			{
				DisplaySequence("SG_CEID_ManualClearBin failure");
			}
		}
	}

	BackupToNVRAM();

	//OsramTrip 8/22
	//HmiMessage_Red_Yellow("Bin counters are cleared (SECSGEM)", "Clear Bin Counter");

	svMsg.InitMessage(sizeof(BOOL), &bIsBinCntClred);
	return 1;

} //end AutoClrBinCnt_SECSGEM


LONG CBinTable::GetBinNumber(IPC_CServiceMessage &svMsg)	
{
	LONG lBinNo;
	UCHAR ucBinNo;
	ULONG ulNoOfSortedDie;
	CString szLog;

	svMsg.GetMsg(sizeof(LONG), &lBinNo);
	ucBinNo = static_cast<UCHAR>(lBinNo);
	szLog.Format("BT: GetBinNumber start - %d", lBinNo);
	CMSLogFileUtility::Instance()->BL_LogStatus(szLog);		//v3.87

	TRY
	{
		//Total No of Sorted Die
		ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfSortedDie(ucBinNo);

	} CATCH (CException, e) 
	{
		ulNoOfSortedDie = 0;
		CMSLogFileUtility::Instance()->BL_LogStatus("BT: GetBinNumber throws exception!");		//v3.87
	}
	END_CATCH

	szLog.Format("BT: GetBinNumber OK - %d", ulNoOfSortedDie);
	CMSLogFileUtility::Instance()->BL_LogStatus(szLog);		//v3.87
	svMsg.InitMessage(sizeof(ULONG), &ulNoOfSortedDie);
	return 1;
}

LONG CBinTable::GetBinGrade(IPC_CServiceMessage& svMsg)
{
	ULONG lBinNo;
	UCHAR ucGrade;
	CString szLog;
	
	svMsg.GetMsg(sizeof(ULONG), &lBinNo);
	szLog.Format("BT: GetBinGrade start - %d", lBinNo);
	CMSLogFileUtility::Instance()->BL_LogStatus(szLog);		//v3.87
	
	TRY
	{
		//Total No of Sorted Die
		ucGrade = m_oBinBlkMain.GrabGrade(lBinNo);

	} CATCH (CException, e) 
	{
		ucGrade = 0;
		CMSLogFileUtility::Instance()->BL_LogStatus("BT: GetBinGrade throws exception!");		//v3.87
	}
	END_CATCH

	szLog.Format("BT: GetBinGrade OK - %d", ucGrade);
	CMSLogFileUtility::Instance()->BL_LogStatus(szLog);		//v3.87
	svMsg.InitMessage(sizeof(UCHAR), &ucGrade);
	return 1;
}


typedef struct {
	LONG lX;
	LONG lY;
} BT_XY_STRUCT;

typedef struct {
	LONG lX;
	LONG lY;
	LONG lWait;
} BT_XY_STRUCT2;

//v4.22T7
typedef struct
{
	LONG lX;
	LONG lY;
	LONG lWait;
	LONG lUnload;
} BT_XY_STRUCT3;

LONG CBinTable::BT_GetCurrPosnInZero(IPC_CServiceMessage &svMsg)
{
	BT_XY_STRUCT stPosn;

	GetEncoderValue();		// Get the encoder value from hardware
	//	rotate BT back to zero
	LONG lNewX = m_lEnc_X, lNewY = m_lEnc_Y;
//	if( IsMS90BTRotated() )
//		RotateBinTable180(m_lEnc_X, m_lEnc_Y, lNewX, lNewY);
	stPosn.lX = lNewX;
	stPosn.lY = lNewY;

	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stPosn);
	return 1;
}

LONG CBinTable::BT_GetPhyBinBlockCorner(IPC_CServiceMessage &svMsg)
{
	BT_XY_STRUCT stPosn;
	LONG lCorner = 0;
	svMsg.GetMsg(sizeof(LONG), &lCorner);

	if( lCorner==-1 )	//	get physical block corner
	{
		GetEncoderValue();		// Get the encoder value from hardware
		stPosn.lX = m_lEnc_X;
		stPosn.lY = m_lEnc_Y;
	}
	else
	{
		BT_XY_STRUCT stPosnUL, stPosnLR;
		DOUBLE dShrinkX = 0.0, dShrinkY = 0.0l;
		//	stPosnLR.lX = ConvertFileUnitToXEncoderValue(m_oPhyBlkMain.GrabBlkLowerRightX(1));
		//	stPosnLR.lY = ConvertFileUnitToYEncoderValue(m_oPhyBlkMain.GrabBlkLowerRightY(1));
		stPosnLR.lX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkLowerRightX(1));
		stPosnLR.lY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkLowerRightY(1));
		//	stPosnUL.lX = ConvertFileUnitToXEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftX(1));
		//	stPosnUL.lY = ConvertFileUnitToYEncoderValue(m_oPhyBlkMain.GrabBlkUpperLeftY(1));
		stPosnUL.lX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftX(1));
		stPosnUL.lY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabBlkUpperLeftY(1));
		if( m_bEnableBinMapBondArea )
		{
			dShrinkX = (DOUBLE)(stPosnLR.lX - stPosnUL.lX)*(1.0-sqrt(0.5))/2.0;
			dShrinkY = (DOUBLE)(stPosnLR.lY - stPosnUL.lY)*(1.0-sqrt(0.5))/2.0;
				//	sqrt(0.5)	//	0.70710678118654752440084436210485
		}
	
		if( lCorner==1 )	//	Low Right
		{
			stPosn.lX = _round(stPosnLR.lX - dShrinkX);
			stPosn.lY = _round(stPosnLR.lY - dShrinkY);
		}
		else	//	Up Left
		{
			stPosn.lX = _round(stPosnUL.lX + dShrinkX);
			stPosn.lY = _round(stPosnUL.lY + dShrinkY);
		}

		if( lCorner>1 )
		{
			LONG lOldX = _round(stPosnUL.lX + dShrinkX);	//	LR
			LONG lOldY = _round(stPosnUL.lY + dShrinkY);	//	LR
			if( lCorner==2 )	//	rotated UL == original LR
			{
				lOldX = _round(stPosnLR.lX - dShrinkX);
				lOldY = _round(stPosnLR.lY - dShrinkY);
			}
			RotateBinTable180(lOldX, lOldY, stPosn.lX, stPosn.lY);
		}	//	rotated BT
	}

	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stPosn);
	return 1;
}


/* --- Move XY --- */
// Move XY (Absolute)
LONG CBinTable::XY_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	LONG lRet = 0;
	BT_XY_STRUCT stPos, stEnc;
	svMsg.GetMsg(sizeof(BT_XY_STRUCT), &stPos);

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	if (X_IsPowerOn() && Y_IsPowerOn())				//v4.26T1	//Cree US
	{
		XY_MoveTo(stPos.lX, stPos.lY, SFM_WAIT);	// Move table
	}
	else
	{
		lRet = 1; //Power faulure
		SetErrorMessage("XY_MoveToCmd: Bin Table XY motor power is not ON!");
		HmiMessage_Red_Yellow("Bin Table XY motor power is not ON!");
	}

	GetEncoderValue();		// Get the encoder value from hardware
	stEnc.lX = m_lEnc_X;
	stEnc.lY = m_lEnc_Y;
//	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);
	svMsg.InitMessage(sizeof(LONG), &lRet);
	return 1;
}

LONG CBinTable::XY2_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	BT_XY_STRUCT stPos, stEnc;
	svMsg.GetMsg(sizeof(BT_XY_STRUCT), &stPos);

	if (!m_bUseDualTablesOption)
	{
		stEnc.lX = 0;
		stEnc.lY = 0;
		svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);
		return 1;
	}


	if (X2_IsPowerOn() && Y2_IsPowerOn())	
	{
		XY2_MoveTo(stPos.lX, stPos.lY, SFM_WAIT);		// Move table
	}
	else
	{
		SetErrorMessage("XY2_MoveToCmd: Bin Table XY motor power is not ON!");
		HmiMessage_Red_Yellow("Bin Table2 XY motor power is not ON!");
	}

	GetEncoderValue();		// Get the encoder value from hardware
	stEnc.lX = m_lEnc_X2;
	stEnc.lY = m_lEnc_Y2;
	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);

	return 1;
}

// Move XY (Relative)
LONG CBinTable::XY_MoveCmd(IPC_CServiceMessage& svMsg)
{
	BT_XY_STRUCT stPos, stEnc;
	svMsg.GetMsg(sizeof(BT_XY_STRUCT), &stPos);

	if (X_IsPowerOn() && Y_IsPowerOn())				//v4.26T1	//Cree US
	{
		XY_Move(stPos.lX, stPos.lY, SFM_WAIT);		// Move table
	}
	else
	{
		SetErrorMessage("XY_MoveCmd: Bin Table XY motor power is not ON!");
		HmiMessage_Red_Yellow("Bin Table XY motor power is not ON!");
	}

	GetEncoderValue();		// Get the encoder value from hardware
	stEnc.lX = m_lEnc_X;
	stEnc.lY = m_lEnc_Y;
	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);

	return 1;
}

/* --- Move Single Axis --- */
// Move X (Absolute)
LONG CBinTable::X_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	BT_XY_STRUCT stEnc;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (X_IsPowerOn())								//v4.26T1	//Cree US
	{
		X_MoveTo(lPos, SFM_WAIT);					// Move table
	}
	else
	{
		SetErrorMessage("X_MoveToCmd: Bin Table X motor power is not ON!");
		HmiMessage_Red_Yellow("Bin Table X motor power is not ON!");
	}

	GetEncoderValue();		// Get the encoder value from hardware
	stEnc.lX = m_lEnc_X;
	stEnc.lY = m_lEnc_Y;
	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);

	return 1;
}

// Move X (Relative)
LONG CBinTable::X_MoveCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	BT_XY_STRUCT stEnc;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (X_IsPowerOn())								//v4.26T1	//Cree US
	{
		X_Move(lPos, SFM_WAIT);						// Move table
	}
	else
	{
		SetErrorMessage("X_MoveCmd: Bin Table X motor power is not ON!");
		HmiMessage_Red_Yellow("Bin Table X motor power is not ON!");
	}

	GetEncoderValue();		// Get the encoder value from hardware
	stEnc.lX = m_lEnc_X;
	stEnc.lY = m_lEnc_Y;
	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);

	return 1;
}

// Move Y (Absolute)
LONG CBinTable::Y_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	BT_XY_STRUCT stEnc;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (Y_IsPowerOn())								//v4.26T1	//Cree US
	{
		Y_MoveTo(lPos, SFM_WAIT);					// Move table
	}
	else
	{
		SetErrorMessage("Y_MoveToCmd: Bin Table Y motor power is not ON!");
		HmiMessage_Red_Yellow("Bin Table Y motor power is not ON!");
	}

	GetEncoderValue();		// Get the encoder value from hardware
	stEnc.lX = m_lEnc_X;
	stEnc.lY = m_lEnc_Y;
	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);

	return 1;
}

// Move Y (Relative)
LONG CBinTable::Y_MoveCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	BT_XY_STRUCT stEnc;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (Y_IsPowerOn())								//v4.26T1	//Cree US
	{	
		Y_Move(lPos, SFM_WAIT);						// Move table
	}
	else
	{
		SetErrorMessage("Y_MoveCmd: Bin Table Y motor power is not ON!");
		HmiMessage_Red_Yellow("Bin Table Y motor power is not ON!");
	}

	GetEncoderValue();		// Get the encoder value from hardware
	stEnc.lX = m_lEnc_X;
	stEnc.lY = m_lEnc_Y;
	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);

	return 1;
}

/* --- Home ---*/
LONG CBinTable::X_HomeCmd(IPC_CServiceMessage& svMsg)
{
#ifdef NU_MOTION
	//Disable limit sensor protection
	CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X);
#endif

	X_Home();

#ifdef NU_MOTION
	//Enable limit sensor protection
	CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X);
#endif

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Y_HomeCmd(IPC_CServiceMessage& svMsg)
{
#ifdef NU_MOTION
	//Disable limit sensor protection
	if (CMS896AApp::m_bIsPrototypeMachine)
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBTAxis_Y);	//prototype MS100
	else
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, 999999, -999999, &m_stBTAxis_Y);
#endif

	Y_Home();

#ifdef NU_MOTION
	//Enable limit sensor protection
	if (CMS896AApp::m_bIsPrototypeMachine)
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_Y);	//prototype MS100
	else
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, m_lTableYPosLimit, m_lTableYNegLimit-2000, &m_stBTAxis_Y);
#endif

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::XY_HomeCmd(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(FALSE);

#ifdef NU_MOTION
	//Disable limit sensor protection
	CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X);
	if (CMS896AApp::m_bIsPrototypeMachine)
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBTAxis_Y);	//prototype MS100
	else
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, 999999, -999999, &m_stBTAxis_Y);

	if (m_bUseDualTablesOption)	//v4.17T1
	{
		CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X2, 999999, -999999, &m_stBTAxis_X2);
		CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, 999999, -999999, &m_stBTAxis_Y2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y2);
	}
#else
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBTAxis_X);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBTAxis_Y);
#endif

	if (m_bUseDualTablesOption)
	{
		X_Home();
		X2_Home();
		Y12_Home();
	}
	else
	{
		Y_Home();
		X_Home();

		//v4.50A21	//Cree HZ
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetCustomerName() == "Cree") )
		{
			Sleep(500);
			SetJoystickOn(FALSE);
			SelectXYProfile(m_lBTUnloadPos_X - m_lCurXPosn, m_lBTUnloadPos_Y - m_lCurYPosn);
			XY_MoveTo(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, SFM_WAIT);
		}
	}

#ifdef NU_MOTION
#else
	CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_X);
	CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_Y);
#endif

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// GetEncoderValue
LONG CBinTable::GetEncoderCmd(IPC_CServiceMessage& svMsg)
{
	BT_XY_STRUCT stEnc;
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = m_lEnc_X;
	stEnc.lY = m_lEnc_Y;
	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);

	return 1;
}

LONG CBinTable::GetEncoderCmd2(IPC_CServiceMessage& svMsg)
{
	BT_XY_STRUCT stEnc;
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = m_lEnc_X2;
	stEnc.lY = m_lEnc_Y2;
	svMsg.InitMessage(sizeof(BT_XY_STRUCT), &stEnc);

	return 1;
}

// On or off the joystick
LONG CBinTable::SetJoystickCmd(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn, bOK=TRUE;
	BOOL bBinLoader =  IsBLEnable();

	svMsg.GetMsg(sizeof(BOOL), &bOn);
	
	if (bBinLoader == TRUE && bOn == TRUE)
	{
		CString szTitle, szContent;

		if (BT_GetFrameLevel() == TRUE)
		{
			SetAlert(IDS_BL_PLATFORM_NOT_DOWN);

			svMsg.InitMessage(sizeof(BOOL), &bOK);
			return 1;
		}		
	}
	
	SetJoystickOn(bOn);
	svMsg.InitMessage(sizeof(BOOL), &bOK);
	return 1;
}

LONG CBinTable::SwitchTableForPRJoyStick(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lBTInUse = 0;
	svMsg.GetMsg(sizeof(LONG), &lBTInUse);

	if (!m_bUseDualTablesOption)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_ulJoyStickMode != MS899_JS_MODE_PR)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (lBTInUse == 1)		//If BT2 currently in use by PR mouse JS
	{
//AfxMessageBox("Enable BT2 JS; HOME BT1", MB_SYSTEMMODAL);
		HomeTable1();
	}
	else
	{
//AfxMessageBox("Enable BT1 JS; HOME BT2", MB_SYSTEMMODAL);
		HomeTable2();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::SetJoystickCmdWithoutSrCheck(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn, bOK=TRUE;
	
	svMsg.GetMsg(sizeof(BOOL), &bOn);
	
	SetJoystickOn(bOn);
	svMsg.InitMessage(sizeof(BOOL), &bOK);
	return 1;
}

// On or off the joystick with checking the current state
LONG CBinTable::UseJoystickCmd(IPC_CServiceMessage& svMsg)
{
	BOOL	bOn, bOK=TRUE, bTemp;

	svMsg.GetMsg(sizeof(BOOL), &bOn);
	if ( bOn == TRUE )
	{
		SetJoystickOn(m_bJoystickOn);
		MoveWaferTableToSafePosn(TRUE);		//v3.70T1
	}
	else
	{
		bTemp = m_bJoystickOn;	
		SetJoystickOn(FALSE);
		m_bJoystickOn = bTemp;	// Restore the original joystick state
	}
	svMsg.InitMessage(sizeof(BOOL), &bOK);

	return 1;
}

// Set joystick speed
LONG CBinTable::SetJoystickSpeedCmd(IPC_CServiceMessage& svMsg)
{
	LONG	lLevel=0;
	LONG	lX_Min, lX_Max, lY_Min, lY_Max;
	BOOL	bOK=TRUE;
	CString szAxis;

	svMsg.GetMsg(sizeof(LONG), &lLevel);

	//if ( m_pServo_X != NULL && m_pServo_Y != NULL )
	if (m_fHardware)
	{
		try
		{
			SetJoystickOn(FALSE);

			//Set X joystick limit
			lX_Min = m_lTableXNegLimit;
			lX_Max = m_lTableXPosLimit;

			//Set Y joystick limit
			lY_Min = m_lTableYNegLimit;
			lY_Max = m_lTableYPosLimit;

			
			m_lJoystickLevel = lLevel;
			switch(lLevel)
			{
				case 0:		// Slow speed
					szAxis = "BinTableXAxis";
					//m_pServo_X->SelectIncrementParam("BT_JoySlow");
					CMS896AStn::MotionSelectIncrementParam(szAxis, "BT_JoySlow", &m_stBTAxis_X);
					szAxis = "BinTableYAxis";
					//m_pServo_Y->SelectIncrementParam("BT_JoySlow"); 
					CMS896AStn::MotionSelectIncrementParam(szAxis, "BT_JoySlow", &m_stBTAxis_Y);
					m_lJoystickLevel = 0;	//v3.76
					break;

				case 2:		// Fast speed
					szAxis = "BinTableXAxis";
					//m_pServo_X->SelectIncrementParam("BT_JoyFast");
					CMS896AStn::MotionSelectIncrementParam(szAxis, "BT_JoyFast", &m_stBTAxis_X);
					szAxis = "BinTableYAxis";
					//m_pServo_Y->SelectIncrementParam("BT_JoyFast"); 
					CMS896AStn::MotionSelectIncrementParam(szAxis, "BT_JoyFast", &m_stBTAxis_Y);
					m_lJoystickLevel = 2;	//v3.76
					break;

				case 1:		// Normal speed
				default:
					szAxis = "BinTableXAxis";
					//m_pServo_X->SelectIncrementParam("BT_JoyNormal");
					CMS896AStn::MotionSelectIncrementParam(szAxis, "BT_JoyNormal", &m_stBTAxis_X);
					szAxis = "BinTableYAxis";
					//m_pServo_Y->SelectIncrementParam("BT_JoyNormal"); 
					CMS896AStn::MotionSelectIncrementParam(szAxis, "BT_JoyNormal", &m_stBTAxis_Y);
					m_lJoystickLevel = 1;
					break;
			}

			if ( m_bIsSetupBinBlk == FALSE )
			{
				szAxis = "BinTableXAxis";
				//m_pServo_X->SetJoystickPositionLimit(lX_Min,lX_Max);
				CMS896AStn::MotionSetJoystickPositionLimit(szAxis, lX_Min, lX_Max, &m_stBTAxis_X);

				szAxis = "BinTableYAxis";
				//m_pServo_Y->SetJoystickPositionLimit(lY_Min,lY_Max);
				CMS896AStn::MotionSetJoystickPositionLimit(szAxis, lY_Min, lY_Max, &m_stBTAxis_Y);
			}

			SetJoystickOn(TRUE, FALSE);		//v3.01T4
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult(szAxis);
			if (szAxis == "BinTableXAxis")
				CMS896AStn::MotionCheckResult(szAxis, &m_stBTAxis_X);
			else
				CMS896AStn::MotionCheckResult(szAxis, &m_stBTAxis_Y);
			bOK = FALSE;
		}	
	}

	svMsg.InitMessage(sizeof(BOOL), &bOK);

	return 1;
}


LONG CBinTable::MoveToCOR(const CString szLogMess)
{
	LONG lRet = gnOK;
	if ( (m_lBinCalibX != 0) && (m_lBinCalibY != 0) && IsMS90())
	{
		if (IsWithinTable1Limit(m_lBinCalibX, m_lBinCalibY))
		{
			//XY_MoveTo(m_lBinCalibX, m_lBinCalibY);
			SelectXYProfile(0 - m_lCurXPosn, m_lBinCalibY - m_lCurYPosn);
			lRet = XY_MoveTo(0, m_lBinCalibY);    //move to X home position, it avoid to hit the wafer table
			CMSLogFileUtility::Instance()->BL_LogStatus(szLogMess);
		}
	}

	return lRet;
}


// Move T to ZERO (Absolute)
//called by Binloader
LONG CBinTable::T_MoveToZeroCmd_ForBL(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_T, &m_stBTAxis_T))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bEnable_T)
	{
		GetEncoderValue();
		if (labs(m_lEnc_T) > 500)
		{
			X_Profile(LOW_PROF);
			Y_Profile(LOW_PROF);
			MoveToCOR(_T("BT: Move to COR"));
			X_Profile(NORMAL_PROF);
			Y_Profile(NORMAL_PROF);
		}

		CMSLogFileUtility::Instance()->BL_LogStatus("BT: T to ZERO degree");
		T_Profile(NORMAL_PROF);
		LONG lRet = T_MoveTo(0);
//====================================2018.5.4===========================================
		T_Profile(LOW_PROF);
//		T_Home();
//=======================================================================================
		if (lRet != gnOK)
		{
			SetErrorMessage("BT T NuMotion error in T_MoveTo BL");
			HmiMessage_Red_Back("BT T NuMotion error!  Please HOME motor again.", "Bin Table");

			X_Profile(NORMAL_PROF);
			Y_Profile(NORMAL_PROF);
			bReturn = FALSE;
		}
	}
  
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// Move XY (Absolute) for BinLoader
LONG CBinTable::XY_MoveToCmd_ForBL(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BT_XY_STRUCT3 stPos;
	svMsg.GetMsg(sizeof(BT_XY_STRUCT3), &stPos);

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);		//v4.22T8

	if (m_bUseDualTablesOption)
	{
		if (stPos.lUnload == 0)		//v4.22T7
			HomeTable2();			//v4.21T8

		//Use the new VLOW profile for long-dist travel		//v4.17T4
		X_Profile(LOW_PROF1);
		Y_Profile(LOW_PROF1);
	}
	else
	{	
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
	}

	if (CMS896AApp::m_bIsPrototypeMachine)
	{
		if (Y_MoveTo(stPos.lY, SFM_WAIT) == gnOK)
		{
			X_MoveTo(stPos.lX, SFM_WAIT);
		}
		else
		{
			bReturn = FALSE;
		}
	}
	else
	{
		if ((stPos.lUnload == 1) ||		//UNLOAD
			(stPos.lUnload == 0))		//LOAD
		{
			if (m_bEnable_T)
			{
				GetEncoderValue();
				if (labs(m_lEnc_T) > 500)	//shiraishi02
				{
					MoveToCOR(_T("BT: Move to COR1"));
				}

				CMSLogFileUtility::Instance()->BL_LogStatus((stPos.lUnload == 1) ? "BT: T to ZERO degree **Unload**" : "BT: T to ZERO degree **Load**");
				T_Profile(NORMAL_PROF);
				LONG lRet = T_MoveTo(0);
//====================================2018.5.4===========================================
				T_Profile(LOW_PROF);
//				T_Home();
//=======================================================================================
				if (lRet != gnOK)
				{
					SetErrorMessage("BT T NuMotion error in T_MoveTo BL");
					HmiMessage_Red_Back("BT T NuMotion error!  Please HOME motor again.", "Bin Table");

					X_Profile(NORMAL_PROF);
					Y_Profile(NORMAL_PROF);
					bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}
			}
		}
		CMSLogFileUtility::Instance()->BL_LogStatus((stPos.lUnload == 1) ? "BT: XY to UNLOAD" : "BT: XY to LOAD");

		XY_MoveTo(stPos.lX, stPos.lY, SFM_WAIT);
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
        
	m_lLastX = stPos.lX;
	m_lLastY = stPos.lY;

	//v3.70T1	//Check BT power to make sure motors are not dead!
	if (stPos.lWait != 0)
	{
		if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) ||
			!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
		{
			bReturn = FALSE;
		}

		if (m_bEnable_T && !T_IsPowerOn())	//v4.39T7	//Nichia MS100+
		{
			bReturn = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::XY2_MoveToCmd_ForBL(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BT_XY_STRUCT3 stPos;
	svMsg.GetMsg(sizeof(BT_XY_STRUCT3), &stPos);

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);		//v4.22T8

	//if (IsTable1InBondRegion())
	//{
	if (stPos.lUnload == 0)		//v4.22T7
	{
		HomeTable1();			//v4.21T8
	}
	//}

	//Use the new VLOW profile for long-dist travel	
	X2_Profile(LOW_PROF1);
	Y2_Profile(LOW_PROF1);

	X2_MoveTo(stPos.lX, SFM_NOWAIT);
	Y2_MoveTo(stPos.lY, SFM_NOWAIT);

	if (stPos.lWait != 0)
	{
		X2_Sync();
		Y2_Sync();
	}

	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);

	m_lLastX2 = stPos.lX;
	m_lLastY2 = stPos.lY;

	//v3.70T1	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::XY_MoveToCmd_ForBL_XThenY(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BT_XY_STRUCT3 stPos;
	svMsg.GetMsg(sizeof(BT_XY_STRUCT3), &stPos);

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);		//v4.22T8

#ifndef NU_MOTION	//MS899
	if (!CMS896AApp::m_bUseSlowBTControlProfile)
	{
		X_Profile(LOW_PROF);
	}

	Y_Profile(NORMAL_PROF);
	//XY_MoveTo(stPos.lX, stPos.lY, SFM_WAIT);	// Move table
	X_MoveTo(stPos.lX, SFM_WAIT);
	Y_MoveTo(stPos.lY, SFM_WAIT);
	X_Profile(NORMAL_PROF);

#else				//MS100

	if (m_bUseDualTablesOption)		//v4.17T4
	{
		//Use the new VLOW profile for long-dist travel	
		X_Profile(LOW_PROF1);
		Y_Profile(LOW_PROF1);
	}
	else
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
	}

	if (CMS896AApp::m_bIsPrototypeMachine)
	{
		if (Y_MoveTo(stPos.lY, SFM_WAIT) == gnOK)
		{
			X_MoveTo(stPos.lX, SFM_WAIT);
		}
		else
		{
			bReturn = FALSE;
		}
	}
	else
	{
		XY_MoveTo(stPos.lX, stPos.lY, SFM_WAIT);
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif
        
	m_lLastX = stPos.lX;
	m_lLastY = stPos.lY;

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::XY2_MoveToCmd_ForBL_XThenY(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BT_XY_STRUCT stPos;
	svMsg.GetMsg(sizeof(BT_XY_STRUCT), &stPos);

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);		//v4.22T8

	X2_Profile(LOW_PROF1);
	Y2_Profile(LOW_PROF1);

	X_MoveTo(stPos.lX, SFM_WAIT);
	Y_MoveTo(stPos.lY, SFM_WAIT);

	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);
        
	m_lLastX2 = stPos.lX;
	m_lLastY2 = stPos.lY;

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::XY_MoveToCmd_ForBL_YThenX(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BT_XY_STRUCT3 stPos;
	svMsg.GetMsg(sizeof(BT_XY_STRUCT3), &stPos);

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);		//v4.22T8

#ifndef NU_MOTION

	if (!CMS896AApp::m_bUseSlowBTControlProfile)		//v3.82
	{
		X_Profile(LOW_PROF);
	}

	Y_Profile(NORMAL_PROF);
	//XY_MoveTo(stPos.lX, stPos.lY, SFM_WAIT);	// Move table
	Y_MoveTo(stPos.lY, SFM_WAIT);
	X_MoveTo(stPos.lX, SFM_WAIT);
	X_Profile(NORMAL_PROF);
	//Y_Profile(NORMAL_PROF);

#else

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	if (CMS896AApp::m_bIsPrototypeMachine)
	{
		if (Y_MoveTo(stPos.lY, SFM_WAIT) == gnOK)
		{
			X_MoveTo(stPos.lX, SFM_WAIT);
		}
		else
		{
			bReturn = FALSE;
		}
	}
	else
	{
		XY_MoveTo(stPos.lX, stPos.lY, SFM_WAIT);
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

#endif
        
	m_lLastX = stPos.lX;
	m_lLastY = stPos.lY;

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::XY2_MoveToCmd_ForBL_YThenX(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BT_XY_STRUCT stPos;
	svMsg.GetMsg(sizeof(BT_XY_STRUCT), &stPos);

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);		//v4.22T8

	X2_Profile(LOW_PROF1);
	Y2_Profile(LOW_PROF1);

	Y2_MoveTo(stPos.lY, SFM_WAIT);
	X2_MoveTo(stPos.lX, SFM_WAIT);

	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);

	m_lLastX2 = stPos.lX;
	m_lLastY2 = stPos.lY;

	//Check BT power to make sure motors are not dead!
	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::XY_Sync_ForBL(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X) ||
		!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y))
	{
		bReturn = FALSE;
	}
	else
	{
		X_Sync();
		Y_Sync();
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::XY2_Sync_ForBL(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	if (m_bUseDualTablesOption)
	{
		if (!CMS896AStn::MotionIsPowerOn(BT_AXIS_X2, &m_stBTAxis_X2) ||
			!CMS896AStn::MotionIsPowerOn(BT_AXIS_Y2, &m_stBTAxis_Y2))
		{
			bReturn = FALSE;
		}
		else
		{
			X2_Sync();
			Y2_Sync();
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//////////////////////////////////
//           Step Move          //
//////////////////////////////////
LONG CBinTable::StepMovePreRoutine(IPC_CServiceMessage& svMsg)
{
	m_ulTableToStepMove		= 0;		//0=BT1, 1=BT2
	m_ulBinBlkToStepMove	= (GetBTBlock() > 1) ? GetBTBlock() : 1;
	m_ucGrade				= m_oBinBlkMain.GrabGrade(m_ulBinBlkToStepMove);
	m_ulCurrBondIndex		= m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove);
	m_ulNoOfBondedDie		= m_oBinBlkMain.GrabNVNoOfBondedDie(m_ulBinBlkToStepMove);
	m_lTempIndex			= m_ulCurrBondIndex;
	m_ulDiePerBlk			= m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToStepMove);
	m_ulHowManySteps		= 0;
	m_szStepMoveMsg			= "";

	switch (m_ulMachineType)
	{
		case BT_MACHTYPE_STD:
			BinBlksDrawing();
			break;

		case BT_MACHTYPE_DTABLE:		//v4.16T1
		case BT_MACHTYPE_DBUFFER:		//v3.71T4
		case BT_MACHTYPE_DL_DLA:
			BinBlksDrawing_BL(1);
			break;
	}

	if (m_bUseDualTablesOption == TRUE)
	{
		HomeTable1();
		HomeTable2();
	}

	WriteGradeLegend();
	LoadGradeRankID();
	return 1;

} //end StepMovePreRoutine


LONG CBinTable::DisplayBinBlkInfoForStepMove(IPC_CServiceMessage& svMsg)
{ 
	//LONG x, y;
	DOUBLE dX=0, dY=0;

	typedef struct 
	{
		ULONG	ulPhyBlkId; //a dummy variable
		ULONG	ulBinBlkId;	
	} BLKDATA;
	BLKDATA stInfo;

	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);
	if (m_oBinBlkMain.GetNoOfBlk() < stInfo.ulBinBlkId)
	{
		SetAlert(IDS_BT_BINID_GT_BINBLKNO);
		m_ulBinBlkToStepMove = 1;
	}
	
	else
	{
		//Update screen display variables
		m_ucGrade			= m_oBinBlkMain.GrabGrade(stInfo.ulBinBlkId);
		m_ulDiePerBlk		= m_oBinBlkMain.GrabNoOfDiePerBlk(stInfo.ulBinBlkId);
		m_ulNoOfBondedDie	= m_oBinBlkMain.GrabNVNoOfBondedDie(stInfo.ulBinBlkId);
		m_ulCurrBondIndex	= m_oBinBlkMain.GrabNVCurrBondIndex(stInfo.ulBinBlkId);
		m_lTempIndex		= m_oBinBlkMain.GrabNVCurrBondIndex(stInfo.ulBinBlkId);
		
		//Move to current index
		SetJoystickOn(FALSE);

		//m_oBinBlkMain.StepMove(stInfo.ulBinBlkId, m_ulCurrBondIndex, x, y);
		m_oBinBlkMain.StepDMove(stInfo.ulBinBlkId, m_ulCurrBondIndex, dX, dY);		//v4.59A19

#ifndef NU_MOTION
		
		//Position offset by "Collet Offset"
		X_MoveTo(ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX, SFM_NOWAIT);
		Y_MoveTo(ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY, SFM_WAIT);
		X_Sync();

#else	//To avoid hitting the bond head stand for MS100

		X_Profile(LOW_PROF1);
		Y_Profile(LOW_PROF1); 
		if (m_bUseDualTablesOption)			//v4.17T4
		{
			X2_Profile(LOW_PROF1);			//Use Very long-dist travel profile
			Y2_Profile(LOW_PROF1);

			if (m_ulTableToStepMove == 1)	//If BT2
			{
//AfxMessageBox("HOME table 1 ...", MB_SYSTEMMODAL);
				HomeTable1();
			}
			else
			{
//AfxMessageBox("HOME table 2 ...", MB_SYSTEMMODAL);
				HomeTable2();
			}
			Sleep(100);
		}

		if (m_ulTableToStepMove == 1)		//If BT2	
		{
			LONG lTargetX = ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX + m_lBT2OffsetX;
			LONG lTargetY = ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY + m_lBT2OffsetY;

			CString szLog;
			szLog.Format("MOVE table 2 - %ld, %ld", lTargetX, lTargetY);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

			if (IsWithinTable2Limit(lTargetX, lTargetY) && !IsTable1InBondRegion())
			{
				X2_MoveTo(lTargetX, SFM_NOWAIT);
				Y2_MoveTo(lTargetY, SFM_WAIT);
				X2_Sync();
			}
		}
		else
		{
			LONG lTargetX = ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX;
			LONG lTargetY = ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY;

			CString szLog;
			szLog.Format("MOVE table 1 - %ld, %ld", lTargetX, lTargetY);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

			if (IsWithinTable1Limit(lTargetX, lTargetY) && !IsTable2InBondRegion())
			{
				if (CMS896AApp::m_bIsPrototypeMachine)
				{
					X_MoveTo(lTargetX, SFM_WAIT);
					Y_MoveTo(lTargetY, SFM_WAIT);
				}
				else
				{
					X_MoveTo(lTargetX, SFM_NOWAIT);
					Y_MoveTo(lTargetY, SFM_WAIT);
					X_Sync();
				}
			}
		}

		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
		if (m_bUseDualTablesOption)		//v4.16T5	//MS100 9Inch
		{
			X2_Profile(NORMAL_PROF);
			Y2_Profile(NORMAL_PROF);
		}

		if (m_bIsRotate180Deg)	//IsMS90Sorting2ndPart();
		{
			INT nPos = (INT) (180.0 / m_dThetaRes);
			LONG lEnc_T = GetTEncoderValue(BT_AXIS_T);
			if (labs(nPos - labs(lEnc_T))  > (INT) (10.0 / m_dThetaRes))
			{
				T_MoveTo(nPos, SFM_WAIT);
				//T_MoveTo(nPos, SFM_WAIT, FALSE);		//180 degree//Matt: Rotation after BT move to target pos to prevent hit BL.	
			}
		}
#endif
	}
	return 1;
} //end DisplayBinBlkInfoForStepMove


LONG CBinTable::MoveToNewIndex(IPC_CServiceMessage& svMsg)
{
	ULONG ulDiePerBlk;
	LONG lIndexToMoveTo=0;
	DOUBLE dX=0, dY=0;

	svMsg.GetMsg(sizeof(LONG), &lIndexToMoveTo);

	ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToStepMove);

	//Restrict index to be within "die per blk" bound
	if (lIndexToMoveTo > LONG(ulDiePerBlk))
	{
		lIndexToMoveTo = ulDiePerBlk;
		m_lTempIndex = ulDiePerBlk;
		m_szStepMoveMsg = "Index inputted greater than no of die block can hold!\nIt now stays at the maximum available index.";
	}
	else if (lIndexToMoveTo < 0)
	{
		lIndexToMoveTo = 0;
		m_lTempIndex = 0;
		m_szStepMoveMsg = "Index inputted smaller than 0!\nIt now stays at index 0 position";
	}
	else
	{
		m_szStepMoveMsg = "";
	}
	
	//Move to temp index
	SetJoystickOn(FALSE);

	//m_oBinBlkMain.StepMove(m_ulBinBlkToStepMove, lIndexToMoveTo, x, y);
	m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, lIndexToMoveTo, dX, dY, TRUE);	//v4.59A19	//MS90 with 0.1um encoder

	
#ifndef NU_MOTION
	//Position offset by "Collet Offset"
	X_MoveTo(ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX, SFM_NOWAIT);
	Y_MoveTo(ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY, SFM_WAIT);
	X_Sync();
#else	//To avoid hitting the bond head stand for MS100

	if (m_bUseDualTablesOption)		//v4.17T4
	{
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
	}
	else
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
	}

	if (m_ulTableToStepMove == 1)		//If BT2	
	{
		LONG lTargetX = ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX + m_lBT2OffsetX;
		LONG lTargetY = ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY + m_lBT2OffsetY;
	
		CString szLog;
		szLog.Format("MOVE table 2 - %ld, %ld", lTargetX, lTargetY);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

		if (IsWithinTable2Limit(lTargetX, lTargetY) && !IsTable1InBondRegion())
		{
			X2_MoveTo(lTargetX, SFM_NOWAIT);
			Y2_MoveTo(lTargetY, SFM_WAIT);
			X2_Sync();
		}
	}
	else
	{
		LONG lTargetX = ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX;
		LONG lTargetY = ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY;
		
		CString szLog;
		szLog.Format("MOVE table 1 - %ld, %ld from %f,%f, collet offset %ld,%ld",
			lTargetX, lTargetY, dX, dY, m_lColletOffsetX, m_lColletOffsetY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 

		if (IsWithinTable1Limit(lTargetX, lTargetY)/* && !IsTable2InBondRegion()*/)
		{
			//andrewng //2020-0807
			if (IsErrMapInUse())
			{
				//GetNewXYMoveToPositionWithBTTheta(lTargetX, lTargetY);					//	step move
				//lTargetX = lTargetX + GetXDelta(m_lRefX, m_lRefY, lTargetX, lTargetY);	//	step move
				BM_GetNewXY(lTargetX, lTargetY);	//	step move block
				//m_stErrMapBTMarkComp.CalculateAfflineOffsetXY(lTargetX, lTargetY);	
			}

			X_MoveTo(lTargetX, SFM_NOWAIT);
			Y_MoveTo(lTargetY, SFM_WAIT);
			X_Sync();

			/*
			if (CMS896AApp::m_bIsPrototypeMachine)
			{
				X_MoveTo(lTargetX, SFM_WAIT);
				Y_MoveTo(lTargetY, SFM_WAIT);
			}
			else
			{
				X_MoveTo(lTargetX, SFM_NOWAIT);
				Y_MoveTo(lTargetY, SFM_WAIT);
				X_Sync();
			}*/
		}
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

#endif
	return 1;
} //end MoveToNewIndex


LONG CBinTable::UpdateBlkIndex(IPC_CServiceMessage& svMsg)
{
	//m_oBinBlkMain.UpdateCurrBondIndex(m_ulBinBlkToStepMove, m_ulTempIndex);
	m_oBinBlkMain.UpdateCurrBondIndex(m_ulBinBlkToStepMove, m_lTempIndex);
	m_oBinBlkMain.UpdateLastFileSaveIndex(m_ulBinBlkToStepMove, m_lTempIndex);

	//m_ulCurrBondIndex = m_oBinBlkMain.GrabCurrBondIndex(m_ulBinBlkToStepMove);
	m_ulCurrBondIndex = m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove);

	return 1;
} //end UpdateCurrBondIndex

LONG CBinTable::EnterBinStepMovePage(IPC_CServiceMessage& svMsg)
{
	m_ulBinBlkToStepMove	= (GetBTBlock() > 1) ? GetBTBlock() : 1;
	m_ucGrade				= m_oBinBlkMain.GrabGrade(m_ulBinBlkToStepMove);
	m_ulCurrBondIndex		= m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove);
	m_ulNoOfBondedDie		= m_oBinBlkMain.GrabNVNoOfBondedDie(m_ulBinBlkToStepMove);
	m_lTempIndex			= m_ulCurrBondIndex;
	m_ulDiePerBlk			= m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToStepMove);

	if (IsMS90())	//v4.59A36
	{
		m_bIsRotate180Deg = m_bIsRotate180DegHMI = IsMS90Sorting2ndPart();
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::ExitBinStepMovePage(IPC_CServiceMessage& svMsg)
{
	if (IsMS90())	//v4.59A36
	{
		if (m_bIsRotate180Deg != IsMS90Sorting2ndPart())
		{
			BinSetMS90Rotation(IsMS90Sorting2ndPart());
		}

		m_bIsRotate180Deg = FALSE;
		m_bIsRotate180DegHMI = FALSE;
	
		CString szLog;
		szLog.Format("ExitBinStepMovePage: MS90Sort2ndPart=%d, Rot180=%d, HMI=%d", 
						IsMS90Sorting2ndPart(), m_bIsRotate180Deg, m_bIsRotate180DegHMI);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

//v4.49A10
LONG CBinTable::BinSetMS90Rotation(BOOL bRoation180, BOOL bMsgBox)
{
	if (!IsMS90())
	{
		return 1;
	}

	DOUBLE dX=0, dY=0;
	BOOL bEnable = bRoation180;

	SetJoystickOn(FALSE);
	m_bIsRotate180Deg = bEnable;

	CString szMsg;
	if (bEnable)
	{
		BT_SetFrameRotation(0, TRUE, TRUE);
		szMsg = "BT set to 180 degree";
		CMS896AStn::m_b2Parts2ndPartStart = TRUE;	//v4.59A21

		m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, 1, dX, dY);		//v4.59A19
		m_oBinBlkMain.SetIsAligned(m_ulBinBlkToStepMove, FALSE);		//v4.59A32
	}
	else
	{
		BT_SetFrameRotation(0, FALSE, TRUE);
		szMsg = "BT set to 0 degree";
		CMS896AStn::m_b2Parts2ndPartStart = FALSE;	//v4.59A21

		m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, 1, dX, dY);		//v4.59A19
		m_oBinBlkMain.SetIsAligned(m_ulBinBlkToStepMove, FALSE);		//v4.59A32
	}

	if (bMsgBox)
	{
		HmiMessage(szMsg);
	}

	SetJoystickOn(TRUE);
	return 1;
}

LONG CBinTable::SetEnableMS90Rotation180(IPC_CServiceMessage& svMsg)
{
	if (IsMS90())
	{
		BOOL bEnable = FALSE;
		svMsg.GetMsg(sizeof(BOOL), &bEnable);
		BinSetMS90Rotation(bEnable, TRUE);
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::MultiRealignFirstDie(IPC_CServiceMessage& svMsg) //4.51D20
{
	BOOL bReturn = TRUE;

	m_nMultiSeachCounter							= 0;//Init 
	m_nMultiUnfondCounter = 1;
	m_lDoubleCheckFirstDie							= 0;
	(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]	= 0;
	(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]	= 0;

	//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"] = -1; 
	ULONG ulDieRow = 0, ulDieCol = 0;
	MultiSearchFirstGoodDie(1, ulDieRow, ulDieCol);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

//////////////////////////////////
//     Grade Information        //
//////////////////////////////////

LONG CBinTable::GradeInfoPreRoutine(IPC_CServiceMessage& svMsg)
{
	m_ucGrade = 1;
	m_ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfSortedDie(1);
	m_ulBinBlkInUse = m_oBinBlkMain.GrabNVBlkInUse(1);

	if (m_ulBinBlkInUse > 0)
	{
		m_ulSortedDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkInUse);
	}
	else
	{
		m_ulSortedDieIndex = 0;
	}
	m_ulGradeCapacity = m_oBinBlkMain.GrabGradeCapacity(1);
	m_ulInputCount = m_oBinBlkMain.GrabInputCount(1);

/* Disable AliasGrade
	if (m_oBinBlkMain.GrabAliasGrade(1) != 0)
		m_bEnableAliasGrade = TRUE;
	else
		m_bEnableAliasGrade = FALSE;
*/
	m_bEnableAliasGrade = FALSE;

	m_ucAliasGrade = m_oBinBlkMain.GrabAliasGrade(1);

    // set

	return 1;
} //end GradeInfoPreRoutine


VOID CBinTable::GetGradeCapacity(const LONG	lSelectedGradeNum, UCHAR aucGrade[BT_MAX_BINBLK_NO], ULONG aulGradeCapacity[BT_MAX_BINBLK_NO])
{
	for (long i = 0; i < BT_MAX_BINBLK_NO; i++)
	{
		aulGradeCapacity[i] = 0;
	}

	for (long i = 0; i < lSelectedGradeNum; i++)
	{
		ULONG ulGradeCapacity = m_oBinBlkMain.GrabGradeCapacity(aucGrade[i]);
		ULONG ulInputCount = m_oBinBlkMain.GrabInputCount(aucGrade[i]);
		if (ulInputCount > ulGradeCapacity)
		{
			ulInputCount = ulGradeCapacity;
		}
		aulGradeCapacity[i] = ulInputCount;
	}
}


LONG CBinTable::DisplayGradeInfo(IPC_CServiceMessage& svMsg)
{
	UCHAR ucGrade;

	svMsg.GetMsg(sizeof(UCHAR), &ucGrade);

	m_ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfSortedDie(ucGrade);
	m_ulBinBlkInUse = m_oBinBlkMain.GrabNVBlkInUse(ucGrade);
	m_ulSortedDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkInUse);
	m_ulGradeCapacity = m_oBinBlkMain.GrabGradeCapacity(ucGrade);
	m_ulInputCount = m_oBinBlkMain.GrabInputCount(ucGrade);

/* Disable AliasGrade
	if (m_oBinBlkMain.GrabAliasGrade(ucGrade) != 0)
		m_bEnableAliasGrade = TRUE;
	else 
		m_bEnableAliasGrade = FALSE;
*/
	m_bEnableAliasGrade = FALSE;

	m_ucAliasGrade = m_oBinBlkMain.GrabAliasGrade(ucGrade);

    // 3406
    SetGemValue("BT_CntGrade", ucGrade); // SG_CEID_BT_COUNTER

	return 1;
} //end DisplayGradeInfo

//4.53D90fnc
BOOL CBinTable::ResetOptimizeBinCountPerWft(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bResetBinCountPerWft = pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT_PER_WAFER);

	CString szMsg;
	szMsg.Format("Enable Optimize Bin Count Per Wft status: (%d,%d)", bResetBinCountPerWft,m_bEnableDieCountInput);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	if( bResetBinCountPerWft && m_bEnableDieCountInput)
	{
		OptimizeBinCountPerWft();
	}

	SaveBinTableData();

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

//4.53D92
BOOL CBinTable::EnableOptimizeBinCountPerWft(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;

	
	if(!m_bEnableDieCountInput)
	{
		if (IfAllBinCountsAreCleared() == FALSE)
		{
			SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
			SetErrorMessage("Please Bin counter is not cleared before close function");
		
			m_bEnableDieCountInput = TRUE;

			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}

	SaveBinTableData();

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::SubmitGradeInfo(IPC_CServiceMessage& svMsg)
{
	UCHAR i;
	BOOL bReply = TRUE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == _T("Soraa"))		//v4.40T2
	{
	}
	else
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(m_ucGrade) > 0)
		{
			SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
			SetErrorMessage("Bin counter is not cleared");
		
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}

	if ( m_ucGrade > 0)
	{
		if (m_ulInputCount > m_oBinBlkMain.GrabGradeCapacity(m_ucGrade))
		{
			m_ulInputCount = m_oBinBlkMain.GrabGradeCapacity(m_ucGrade);
		}

		//Disable AliasGrade
		m_bEnableAliasGrade = FALSE;

		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		m_oBinBlkMain.SaveGradeInfo(m_bEnableAliasGrade, m_ucGrade, m_ulInputCount, m_ucAliasGrade, pBTfile);
		CMSFileUtility::Instance()->SaveBTConfig();
		SetAlert(m_oBinBlkMain.GetMsgCode());
	}
	else
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		for (i=1; i<=128; i++)
		{
			if (m_ulInputCount > m_oBinBlkMain.GrabGradeCapacity(i))
			{
				m_ulInputCount = m_oBinBlkMain.GrabGradeCapacity(i);
			}

			//Disable AliasGrade
			m_bEnableAliasGrade = FALSE;
			m_oBinBlkMain.SaveGradeInfo(m_bEnableAliasGrade, i, m_ulInputCount, m_ucAliasGrade, pBTfile);
		}

		CMSFileUtility::Instance()->SaveBTConfig();
		SetAlert(m_oBinBlkMain.GetMsgCode());
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);

    // 3406
    SetGemValue("BT_CntSortedDieIndex", m_ulSortedDieIndex);
    SetGemValue("BT_CntBlockInUse", m_ulBinBlkInUse);
    SetGemValue("BT_CntNoOfSortedDie", m_ulNoOfSortedDie);
    SetGemValue("BT_CntGradeCapacity", m_ulGradeCapacity);
    SetGemValue("BT_CntGradeCount", m_ulInputCount);
    // 3407
    SetGemValue("BT_CntEnableAliasGrade", m_bEnableAliasGrade);
    SetGemValue("BT_CntAliasGrade", m_ucAliasGrade);
    // 7403
    SendEvent(SG_CEID_BT_COUNTER, FALSE);

	return 1;
} //end SubmitGradeInfo


LONG CBinTable::CopyAllGradeInfo(IPC_CServiceMessage& svMsg)
{
	UCHAR i;
	ULONG ulInputCount = 0;
	BOOL bReply = TRUE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == _T("Soraa"))		//v4.40T2
	{
	}
	else
	{
		if ( IfAllBinCountsAreCleared() == FALSE )
		{
			SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
			SetErrorMessage("Bin counter is not cleared");
		
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}

	if (m_ucGrade != 1)
	{
		SetAlert(IDS_BT_ONLY_COPY_GRADE1);
	}
	else
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		for (i = 1; i<= MS_MAX_BIN; i++)
		{
			ulInputCount = m_ulInputCount;
			if (ulInputCount > m_oBinBlkMain.GrabGradeCapacity(i))
			{
				ulInputCount = m_oBinBlkMain.GrabGradeCapacity(i);
				if (i == 1)
				{
					m_ulInputCount = ulInputCount;
				}
			}

			m_oBinBlkMain.SaveGradeInfo(m_bEnableAliasGrade, i, ulInputCount, m_ucAliasGrade, pBTfile);
		}

		CMSFileUtility::Instance()->SaveBTConfig();
		SetAlert(m_oBinBlkMain.GetMsgCode());
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}


LONG CBinTable::CopyPartialGradeInfo(IPC_CServiceMessage& svMsg)
{
	UCHAR i;
	ULONG ulInputCount = 0;
	BOOL bReply = TRUE;

	typedef struct 
	{
		UCHAR	ucStartGrade; 
		UCHAR	ucEndGrade;	
	} GRADEDATA;
	GRADEDATA stInfo;

	svMsg.GetMsg(sizeof(GRADEDATA), &stInfo);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == _T("Soraa"))		//v4.40T2
	{
	}
	else
	{
		if ( IfAllBinCountsAreCleared() == FALSE )
		{
			SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
			SetErrorMessage("Bin counter is not cleared");
		
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}

	if (stInfo.ucEndGrade < stInfo.ucStartGrade)
	{
		SetAlert(IDS_BT_WRONG_COPY_RANGE);
	}
	else
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		for (i = stInfo.ucStartGrade; i<= stInfo.ucEndGrade; i++)
		{
			ulInputCount = m_ulInputCount;
			if (ulInputCount > m_oBinBlkMain.GrabGradeCapacity(i))
			{
				ulInputCount = m_oBinBlkMain.GrabGradeCapacity(i);
				if (i == 1)
				{
					m_ulInputCount = ulInputCount;
				}
			}

			m_oBinBlkMain.SaveGradeInfo(m_bEnableAliasGrade, i, ulInputCount, m_ucAliasGrade, pBTfile);
		}

		CMSFileUtility::Instance()->SaveBTConfig();
		SetAlert(m_oBinBlkMain.GetMsgCode());
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}


LONG CBinTable::CheckIfAllBinCntCleared(IPC_CServiceMessage& svMsg)
{
	BOOL bRtn;

	if (m_bIsAllBinCntCleared)
	{
		bRtn = TRUE;
	}
	else
	{
		SetAlert(IDS_BT_MAP_FORMAT_CHANGED);
		bRtn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bRtn);

	return 1;
} //end CheckIfAllBinCntCleared


LONG CBinTable::UpdateIfAllBinCountsAreCleared(IPC_CServiceMessage& svMsg)		//v4.08		//Osram ClearBin menu page
{
	//Osram: used to reset  if Clear-All is triggered in Osram Clear-Bin menu
	if (IfAllBinCountsAreCleared())
		SetIsAllBinCntCleared(TRUE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


///////////////////////////
//   Lot Number Change   //
///////////////////////////
LONG CBinTable::PreLotNoChangeChecking(IPC_CServiceMessage& svMsg)
{
	ULONG i;
	LONG lHmiMsgReply;
	BOOL bIfAllBinCntsCleared, bReply;

	//Check if all bin counts are cleared before lot number change

	bIfAllBinCntsCleared = TRUE;
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
		{
			bIfAllBinCntsCleared = FALSE;
			break;
		}
	}

	if ((CMS896AStn::m_bForceClearBinBeforeNewLot == TRUE) && (bIfAllBinCntsCleared == FALSE))
	{
		int nConvID = 0;
		IPC_CServiceMessage stMsg;

		nConvID = m_comClient.SendRequest("MapSorter", "ResetLotNumber", stMsg);

		// Get the reply
		m_comClient.ScanReplyForConvID(nConvID, 5000);
		m_comClient.ReadReplyForConvID(nConvID,stMsg);
		
		bReply = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		HmiMessage("Please Clear Bin to change Lot Number!");
		return 1;
	}

	if (bIfAllBinCntsCleared)
	{
		bReply = TRUE;
	}
	else
	{
		CString szTitle, szContent;

		szTitle.LoadString(HMB_BT_CHANGE_LOT_NO);
		szContent.LoadString(HMB_BT_CONF_CHANGE_LOT_NO);

		lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); 
		HmiMessage("Lot number reset!");
		if (lHmiMsgReply == 3) //3 = Yes
		{
			bReply = TRUE;
		}
		else
		{
			int nConvID = 0;
			IPC_CServiceMessage stMsg;

			nConvID = m_comClient.SendRequest("MapSorter", "ResetLotNumber", stMsg);

			// Get the reply
			m_comClient.ScanReplyForConvID(nConvID, 5000);
			m_comClient.ReadReplyForConvID(nConvID,stMsg);

			bReply = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
} //end PreLotNoChangeChecking


//////////////////////////////
//   Lot Directory Change   //
//////////////////////////////
LONG CBinTable::PreLotDirectoryChangeChecking(IPC_CServiceMessage& svMsg)
{
	ULONG i;
	LONG lHmiMsgReply;
	BOOL bIfAllBinCntsCleared, bReply;

	//Check if all bin counts are cleared before lot number change

	bIfAllBinCntsCleared = TRUE;
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
		{
			bIfAllBinCntsCleared = FALSE;
			break;
		}
	}

	if (bIfAllBinCntsCleared)
	{
		bReply = TRUE;
	}
	else
	{
		CString szTitle, szContent;

		szTitle.LoadString(HMB_BT_CHANGE_LOT_DIRECTORY);
		szContent.LoadString(HMB_BT_CONF_CHANGE_LOT_DR);

		lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3); 

		if (lHmiMsgReply == 3) //3 = Yes
		{
			bReply = TRUE;
		}
		else
		{
			int nConvID = 0;
			IPC_CServiceMessage stMsg;

			nConvID = m_comClient.SendRequest("MapSorter", "ResetBinLotDirectory", stMsg);

			// Get the reply
			m_comClient.ScanReplyForConvID(nConvID, 5000);
			m_comClient.ReadReplyForConvID(nConvID,stMsg);

			bReply = FALSE;
		}
	}

	//Reset BinBlk SNR no
	if ( m_oBinBlkMain.GrabSerialNoFormat() == BT_SNR_RESET_CHANGE_LOTDIR )
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE-1);
			m_oBinBlkMain.ResetSerialNo(i, pBTfile);	//v4.52A8
		}
		CMSFileUtility::Instance()->SaveBTConfig();

		CMSLogFileUtility::Instance()->BT_BinSerialLog("Pre Lot Directory Change Checking");
		SetStatusMessage("Bins file no are reset");
		SetErrorMessage("Reset All Bin Serial (Lot Directory Change)");
	}

	// for tyntekepistar format
	DeleteFile(gszROOT_DIRECTORY + "\\Exe\\OutputFileSerial.msd");

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
} //end PreLotDirectoryChangeChecking


//////////////////////////////
//  Pre Associate File Change //
//////////////////////////////
LONG CBinTable::PreAssociateFileChangeChecking(IPC_CServiceMessage& svMsg)
{
	ULONG i;
	BOOL bIfAllBinCntsCleared, bReply;

	//Check if all bin counts are cleared before lot number change

	bIfAllBinCntsCleared = TRUE;
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
		{
			bIfAllBinCntsCleared = FALSE;
			break;
		}
	}

	if (bIfAllBinCntsCleared)
	{
		bReply = TRUE;
	}
	else
	{
		CString szTitle, szContent;

		szTitle.LoadString(HMB_BT_CHANGE_ASSOC_FILE);
		szContent.LoadString(HMB_BT_CONF_CHANGE_ASSOC_FILE);

		HmiMessage(szContent, szTitle); 

		int nConvID = 0;
		IPC_CServiceMessage stMsg;

		nConvID = m_comClient.SendRequest("MapSorter", "ResetAssociateFile", stMsg);

		// Get the reply
		m_comClient.ScanReplyForConvID(nConvID, 5000);
		m_comClient.ReadReplyForConvID(nConvID,stMsg);

		bReply = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
} //end PreLotNoChangeChecking


//Select Bin Serial No Method
LONG CBinTable::SetBinSerialNoFormat(IPC_CServiceMessage& svMsg)
{
	BOOL bSet = TRUE;
	CString szBinSerialFormat = "";

	if ( m_oBinBlkMain.IsDisableBinSNRFormat() == TRUE )
	{
		m_ulBinSerialNoFormat = m_oBinBlkMain.GetDefaultBinSNRFormat();
		m_oBinBlkMain.SetSerialNoFormat(m_ulBinSerialNoFormat);
		szBinSerialFormat.Format("Set bin serial format to mode: %d", m_ulBinSerialNoFormat);
		SetStatusMessage(szBinSerialFormat);
		
		svMsg.InitMessage(sizeof(BOOL), &bSet);
		return 1;
	}

	m_oBinBlkMain.SetSerialNoFormat(m_ulBinSerialNoFormat);

	szBinSerialFormat.Format("Set bin serial format to mode: %d", m_ulBinSerialNoFormat);
	SetStatusMessage(szBinSerialFormat);
	
	svMsg.InitMessage(sizeof(BOOL), &bSet);
	return 1;
}


//Reset Bin Serial No
LONG CBinTable::ResetBinSerialNo(IPC_CServiceMessage& svMsg)
{
	BOOL bReset = FALSE;
	BOOL bShowAlertBox = FALSE;
	ULONG i;

	svMsg.GetMsg(sizeof(BOOL), &bShowAlertBox);
	
	if (bShowAlertBox == TRUE)
	{
		CString szTitle, szContent;

		szTitle.LoadString(HMB_BT_BIN_SNR_NO);
		szContent.LoadString(HMB_BT_RESET_SNR_NO);

		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
		{
			bReset = TRUE;
			// Log Reset Bin Serial
			SetErrorMessage("Reset All Bin Serial (manually)");
			CMSLogFileUtility::Instance()->BT_BinSerialLog("Reset All Bin Serial (manually)");
		}
	}
	else
	{
		bReset = TRUE;
	}

	if (bReset == TRUE)
	{
		if (m_oBinBlkMain.IsBondingInProgress() == TRUE)
		{
			bReset = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReset);
			return 1;
		}

		if (m_bUseMultipleSerialCounter == TRUE)
		{
			m_oBinBlkMain.ResetMultipleBinSerialNo(CMS896AStn::m_lOTraceabilityFormat-1);
		}
		else
		{
			CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
			for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
			{
				i = min(i, BT_MAX_BINBLK_SIZE-1);	
				m_oBinBlkMain.ResetSerialNo(i, pBTfile);	//v4.52A8
			}
			CMSFileUtility::Instance()->SaveBTConfig();
		}
		
		if( IsAOIOnlyMachine()==FALSE )
		{
			CMSLogFileUtility::Instance()->BT_BinSerialLog("ResetBinSerialNo");
			SetStatusMessage("Bins file no are reset");
		}

		//delete the msd file which stores the serial number for 
		TRY
		{
			DeleteFile(gszROOT_DIRECTORY + "\\Exe\\FATC_serial.msd");
			DeleteFile(gszROOT_DIRECTORY + "\\Exe\\FATC_serialbkf.msd");
			SetErrorMessage("Reset FATC serial - done");
		} CATCH (CFileException, e)
		{
		}
		END_CATCH
	}

	svMsg.InitMessage(sizeof(BOOL), &bReset);
	return 1;
}

//Set Bin Max Serial No
LONG CBinTable::SetBinSerialNoMaxLimit(IPC_CServiceMessage& svMsg)
{
	BOOL bSet = TRUE;

	m_oBinBlkMain.SetSerialNoMaxLimit(m_ulBinSerialNoMaxLimit);

	svMsg.InitMessage(sizeof(BOOL), &bSet);
	return 1;
}


//Set Bin Clear format (by date or hit limit)
LONG CBinTable::SetBinClearFormat(IPC_CServiceMessage& svMsg)
{
	CString szBinClearFormat = "";
	BOOL bSet = TRUE;

	if (m_oBinBlkMain.IsDisableClearCountFormat() == TRUE)
	{
		//m_ulBinClearFormat = 
		m_ulBinClearFormat = m_oBinBlkMain.GetDefaultClearCountFormat();
		m_oBinBlkMain.SetBinClearFormat(m_ulBinClearFormat);
		szBinClearFormat.Format("Set bin clear format to mode:%d", m_ulBinClearFormat);
		SetStatusMessage(szBinClearFormat);
		svMsg.InitMessage(sizeof(BOOL), &bSet);
		return 1;
	}

	m_oBinBlkMain.SetBinClearFormat(m_ulBinClearFormat);
	szBinClearFormat.Format("Set bin clear format to mode:%d", m_ulBinClearFormat);
	SetStatusMessage(szBinClearFormat);

	svMsg.InitMessage(sizeof(BOOL), &bSet);
	return 1;
}


//Set Bin Clear max limit
LONG CBinTable::SetBinClearMaxLimit(IPC_CServiceMessage& svMsg)
{
	BOOL bSet = TRUE;

	m_oBinBlkMain.SetBinClearMaxLimit(m_ulBinClearMaxLimit);

	svMsg.InitMessage(sizeof(BOOL), &bSet);
	return 1;
}


//Set Bin Clear init count
LONG CBinTable::SetBinClearInitCount(IPC_CServiceMessage& svMsg)
{
	BOOL bSet = TRUE;

	m_oBinBlkMain.SetBinClearInitCount(m_ulBinClearInitCount);

	svMsg.InitMessage(sizeof(BOOL), &bSet);
	return 1;
}


//Reset Bin Clear count to init value
LONG CBinTable::ResetBinClearedCount(IPC_CServiceMessage& svMsg)
{
	BOOL bReset = FALSE;
	BOOL bShowAlertBox = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bShowAlertBox);
	
	if (bShowAlertBox == TRUE)
	{
		CString szTitle, szContent;

		szTitle.LoadString(HMB_BT_BIN_CLEARED_COUNT);
		szContent.LoadString(HMB_BT_RESET_BINCLR_COUNT);

		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
		{
			bReset = TRUE;
		}
	}
	else
	{
		bReset = TRUE;
	}

	if (bReset == TRUE)
	{
		if (m_oBinBlkMain.IsBondingInProgress() == TRUE)
		{
			bReset = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReset);
			return 1;
		}

		m_oBinBlkMain.ResetBinClearedCount();

		SetStatusMessage("Bin Cleared counter is reset");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReset);
	return 1;
}

//Select Record Bin Accumulated bonded count
LONG CBinTable::EnableBinAccBondedCount(IPC_CServiceMessage& svMsg)
{
	BOOL bSet = TRUE;
	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);		//v2.56

	m_bSaveBinBondedCount = bEnable;			//v2.56
	m_oBinBlkMain.SetAccCountOption(m_bSaveBinBondedCount);

	svMsg.InitMessage(sizeof(BOOL), &bSet);
	return 1;
}

//Reset Acc. Binblk bonded count
LONG CBinTable::ResetBinAccBondedCount(IPC_CServiceMessage& svMsg)
{
	BOOL bSet = TRUE;
	ULONG i;		

	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE-1);	//Klocwork	//v4.02T5
		m_oBinBlkMain.SetBinAccBondedCount(i,0);
	}

	SetStatusMessage("Accmulated bonded counter are reset");

	svMsg.InitMessage(sizeof(BOOL), &bSet);
	return 1;
}


//Select Optimize Bin Grade
LONG CBinTable::EnableOptimizeBinGrade(IPC_CServiceMessage& svMsg)
{
	BOOL bSet = TRUE;
	BOOL bUpdateStatus;
	ULONG i;

	//Check all bins should be cleared before change this option
	for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if ( (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0) && (m_oBinBlkMain.GrabStatus(i) != BT_BLK_NORMAL) )
		{
			SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
			SetErrorMessage("Bin counter is not cleared");

			bUpdateStatus = m_bOptimizeBinGrade;
			m_bOptimizeBinGrade = !bUpdateStatus;

			bSet = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bSet);
			return 1;
		}
	}

	m_oBinBlkMain.SetAutoAssignGrade(m_bOptimizeBinGrade);
	
	//Re-assign to original grade
	if ( m_bOptimizeBinGrade == FALSE )
	{
		for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE-1);	//Klocwork	//v4.02T5
			bUpdateStatus = m_oBinBlkMain.ReassignBlkGrade(i, m_oBinBlkMain.GrabOriginalGrade(i), TRUE, TRUE);
		}

		BinBlksDrawing();
		WriteGradeLegend();
		LoadGradeRankID();
		LoadWaferStatistics();

		//Clear Wafermap
		m_WaferMapWrapper.InitMap();
	}


	svMsg.InitMessage(sizeof(BOOL), &bSet);
	return 1;
}


//Select LS bond pattern 
LONG CBinTable::EnableLSBondPattern(IPC_CServiceMessage& svMsg)
{
	BOOL bSet = TRUE;
	BOOL bUpdateStatus;
	ULONG i;

	//Check all bins should be cleared before change this option
	for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if ( m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0 )
		{
			SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
			SetErrorMessage("Bin counter is not cleared");

			bUpdateStatus = m_bLSBondPattern;
			m_bLSBondPattern = !bUpdateStatus;

			bSet = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bSet);
			return 1;
		}
	}

	m_oBinBlkMain.SetLSBondPattern(m_bLSBondPattern);


	svMsg.InitMessage(sizeof(BOOL), &bSet);
	return 1;
}

//////////////////////////
//   Bin Table Limits   //
//////////////////////////

LONG CBinTable::ResetXLimitCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bBT2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBT2);

	if (bBT2 && m_bUseDualTablesOption)
	{
		m_lTableX2PosLimit = 0;
		m_lTableX2NegLimit = 0;
	}
	else
	{
		m_lTableXPosLimit = 0;
		m_lTableXNegLimit = 0;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}

// Search Bintable Both X Limit
LONG CBinTable::SearchXLimitCmd(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	BOOL bFrameAlign;
	BOOL bBinLoader;
	BOOL bReturn = TRUE;
	int nConvID = 0;
	INT nResult = gnOK;


	if (!m_fHardware)
	{	
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();

	bBinLoader = IsBLEnable();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHUplookPr = pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);

	try
	{
		//Turn on frame align before start search limit to prevent crashes with mech. part
		if ( bBinLoader == TRUE )
		{
			bFrameAlign = TRUE;
			stMsg.InitMessage(sizeof(BOOL), &bFrameAlign);
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "FrameAlign", stMsg);
		}

		LONG lMotorDirectionX = GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_MOTOR_DIRECTION);				
		SetJoystickOn(FALSE);


		// Move table back to Home & Disbable Limit sensor protection
		//X_MoveTo(0, SFM_WAIT);
		if (!m_bUseDualTablesOption)
		{
			X_Home();
		}
		else
		{
			Y_Profile(LOW_PROF1);
			Y_MoveTo(0);
			//Y_Profile(LOW_PROF1);
		}

		//m_pServo_X->DisableProtection(HP_LIMIT_SENSOR);
#ifdef NU_MOTION
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X);
		//if (m_bUseDualTablesOption)
			//CMS896AStn::MotionEnableProtection(BT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y);
		//For Nu Motion, enlarge the software limit before searching limit
		CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, 9999999, -9999999, &m_stBTAxis_X);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, GMP_PROT_TYPE_SW_LMT, FALSE, FALSE, &m_stBTAxis_X);
		if (m_bUseDualTablesOption)
			X_Profile(LOW_PROF1);
		else
			X_Profile(LOW_PROF);
#else
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBTAxis_X);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBTAxis_Y);
#endif

		/*
		if (m_bUseDualTablesOption)		//v4.21T8
		{
			Sleep(500);
			X_MoveTo(0, SFM_WAIT);	
			Sleep(500);
		}
		*/

		// Start Search Limit
		if ((nResult = X_SearchPosLimit()) == gnOK)
		{
			Sleep(300);
			GetEncoderValue();		

			//Leave 1 mm space
			if ( lMotorDirectionX == -1 )
			{
				m_lTableXNegLimit = m_lEnc_X + (LONG)(1000 * m_dXResolution);
			}
			else
			{
				//if (m_bUseDualTablesOption)
				//	m_lTableXPosLimit = m_lEnc_X - 1000;
				//else
				m_lTableXPosLimit = m_lEnc_X - (LONG)(1000 * m_dXResolution);

				if (IsMS90() && (GetChannelResolution(MS896A_CFG_CH_BINTABLE_X)  == 0.0001))
				{
					//v4.57A7
					LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_MAX_DISTANCE);	
					//m_lTableXPosLimit = m_lTableXPosLimit + 5000;
					m_lTableXPosLimit = lMaxPos;		//v4.57A6
				}
			}

			//X_Profile(LOW_PROF);	//v4.01
			Sleep(500);
			X_MoveTo(0, SFM_WAIT);	
			Sleep(500);
		}


		if ( (nResult == gnOK) && IsMS60() )		//v4.46T28
		{
			m_lTableXNegLimit = m_lTableXPosLimit - 510000;

			if (bEnableBHUplookPr)	//andrewng123
			{
				m_lTableXNegLimit = m_lTableXPosLimit - 2570000;
			}

			if (IsMS90() && (GetChannelResolution(MS896A_CFG_CH_BINTABLE_X)  == 0.0001))
			{
				//v4.57A7
				LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_MIN_DISTANCE);								
				m_lTableXNegLimit = lMinPos;
			}

#ifdef NU_MOTION
			X_Profile(NORMAL_PROF);
			Y_Profile(NORMAL_PROF);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, m_lTableXPosLimit, m_lTableXNegLimit, &m_stBTAxis_X);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, m_lTableYPosLimit, m_lTableYNegLimit-2000, &m_stBTAxis_Y);
#endif
		}
		else if ( (nResult == gnOK) && (X_SearchNegLimit() == gnOK) )
		{
			Sleep(300);
			GetEncoderValue();		

			//Leave 1 mm space
			if ( lMotorDirectionX == -1 )
			{
				//if (m_bUseDualTablesOption)
				//	m_lTableXPosLimit = m_lEnc_X - 500;
				//else
				m_lTableXPosLimit = m_lEnc_X - (LONG)(1000 * m_dXResolution);	
			}
			else
			{
				m_lTableXNegLimit = m_lEnc_X + (LONG)(1000 * m_dXResolution);
			}

			//X_Profile(LOW_PROF);	//v4.01
			Sleep(500);
			X_MoveTo(0, SFM_WAIT);
			Sleep(500);

#ifdef NU_MOTION
			X_Profile(NORMAL_PROF);
			Y_Profile(NORMAL_PROF);

			//For Nu Motion, set the software limit
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, m_lTableXPosLimit, m_lTableXNegLimit, &m_stBTAxis_X);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, m_lTableYPosLimit, m_lTableYNegLimit-2000, &m_stBTAxis_Y);
#endif
		}

		//m_pServo_X->EnableProtection(HP_LIMIT_SENSOR, TRUE);
		//m_pServo_X->SetJoystickPositionLimit(m_lTableXNegLimit, m_lTableXPosLimit);
		//CMS896AStn::MotionEnableProtection("BinTableXAxis", HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_X);
#ifdef NU_MOTION
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y);
		//if (m_bUseDualTablesOption)
		//	CMS896AStn::MotionEnableProtection(BT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, GMP_PROT_TYPE_SW_LMT, TRUE, FALSE, &m_stBTAxis_X);
#else
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_X);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_Y);
#endif
		CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_X, m_lTableXNegLimit, m_lTableXPosLimit, &m_stBTAxis_X);

		SetJoystickOn(TRUE);	
		SetStatusMessage("Bintable search X limit is completed");

		CString szLog;
		szLog.Format("BT: X Table limit is learnt: (%ld, %ld)", m_lTableXPosLimit, m_lTableXNegLimit);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


		//Turn off frame align after search limit
		if ( bBinLoader == TRUE )
		{
			bFrameAlign = FALSE;
			stMsg.InitMessage(sizeof(BOOL), &bFrameAlign);
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "FrameAlign", stMsg);
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//CheckHiPECResult("BinTableXAxis");
		CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
		bReturn = FALSE;
	}	
	
	(*m_psmfSRam)["BinTable"]["Limit"]["X Pos"]			= m_lTableXPosLimit;
	(*m_psmfSRam)["BinTable"]["Limit"]["X Neg"]			= m_lTableXNegLimit;
	(*m_psmfSRam)["BinTable"]["Limit"]["Y Pos"]			= m_lTableYPosLimit;
	(*m_psmfSRam)["BinTable"]["Limit"]["Y Neg"]			= m_lTableYNegLimit;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	CloseLoadingAlert();
	return 1;
}


LONG CBinTable::SearchX2LimitCmd(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	BOOL bBinLoader = IsBLEnable();
	BOOL bReturn = TRUE;
	int nConvID = 0;
	CString szTemp;


	if (!m_fHardware)
	{	
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (!m_bUseDualTablesOption)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();

	try
	{
		//Turn on frame align before start search limit to prevent crashes with mech. part
		//if ( bBinLoader == TRUE )
		//{
		//	bFrameAlign = TRUE;
		//	stMsg.InitMessage(sizeof(BOOL), &bFrameAlign);
		//	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "FrameAlign", stMsg);
		//}
		LONG lMotorDirectionX = GetChannelInformation(MS896A_CFG_CH_BINTABLE2_X, MS896A_CFG_CH_MOTOR_DIRECTION);				
		
		//For Nu Motion, enlarge the software limit before searching limit
		CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X2, 9999999, -9999999, &m_stBTAxis_X2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X,  HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X2);
		
		SetJoystickOn(FALSE);
		Sleep(500);
		//X12_Home();

		Y2_Profile(LOW_PROF1);
		Y2_MoveTo(0);

		X2_Profile(LOW_PROF1);

//AfxMessageBox("Before X2_MoveTo(0)", MB_SYSTEMMODAL);
		//X2_MoveTo(0, SFM_WAIT);	
		//Sleep(500);

		// Start Search Limit
		if (X2_SearchPosLimit() == gnOK)
		{
			Sleep(300);

			GetEncoderValue();		

			//Leave 1 mm space
			if ( lMotorDirectionX == -1 )
			{
				//if (m_bUseDualTablesOption)
				//	m_lTableX2NegLimit = m_lEnc_X2 + 500;
				//else
				m_lTableX2NegLimit = m_lEnc_X2 + (LONG)(1000 * m_dXResolution);
			}
			else
			{
				m_lTableX2PosLimit = m_lEnc_X2 - (LONG)(1000 * m_dXResolution);	

//szTemp.Format("X2_SearchPosLimit: %d -> m_lTableX2PosLimit = %d", lMotorDirectionX, m_lTableX2PosLimit);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
			}

			//X2_Profile(LOW_PROF1);
			Sleep(500);
			X2_MoveTo(0, SFM_WAIT);	
			Sleep(500);
		}

		if (X2_SearchNegLimit() == gnOK)
		{
			Sleep(300);

			GetEncoderValue();		

			//Leave 1 mm space
			if ( lMotorDirectionX == -1 )
			{
				m_lTableX2PosLimit = m_lEnc_X2 - (LONG)(1000 * m_dXResolution);	
			}
			else
			{
				//if (m_bUseDualTablesOption)
				//	m_lTableX2NegLimit = m_lEnc_X2 + 1000;
				//else
				m_lTableX2NegLimit = m_lEnc_X2 + (LONG)(1000 * m_dXResolution);

//szTemp.Format("X2_SearchNegLimit: %d -> m_lTableX2NegLimit = %d", lMotorDirectionX, m_lTableX2NegLimit);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
			}

			//X2_Profile(LOW_PROF1);
			Sleep(500);
			X2_MoveTo(0, SFM_WAIT);
			Sleep(500);
		}

		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);

		//X2_Home();
		//Sleep(500);

		//For Nu Motion, set the software limit
		CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X2, m_lTableX2PosLimit, m_lTableX2NegLimit,		&m_stBTAxis_X2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X,  HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X);
		CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_X2, m_lTableX2NegLimit, m_lTableX2PosLimit, &m_stBTAxis_X2);
		
		SetJoystickOn(TRUE);	
		SetStatusMessage("Bintable search X2 limit is completed");
		
		CString szLog;
		szLog.Format("BT: X2 Table limit is learnt: (%ld, %ld)", m_lTableX2PosLimit, m_lTableX2NegLimit);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		//Turn off frame align after search limit
		//if ( bBinLoader == TRUE )
		//{
		//	bFrameAlign = FALSE;
		//	stMsg.InitMessage(sizeof(BOOL), &bFrameAlign);
		//	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "FrameAlign", stMsg);
		//}

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//CheckHiPECResult("BinTableXAxis");
		CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
		bReturn = FALSE;
	}	
	
	(*m_psmfSRam)["BinTable2"]["Limit"]["X Pos"]	= m_lTableX2PosLimit;
	(*m_psmfSRam)["BinTable2"]["Limit"]["X Neg"]	= m_lTableX2NegLimit;
	(*m_psmfSRam)["BinTable2"]["Limit"]["Y Pos"]	= m_lTableY2PosLimit;
	(*m_psmfSRam)["BinTable2"]["Limit"]["Y Neg"]	= m_lTableY2NegLimit;

	CloseLoadingAlert();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


// Search Bintable Both Y Limit
LONG CBinTable::SearchYLimitCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	INT nResult = gnOK;

	if (!m_fHardware)
	{	
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHUplookPr = pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);

	try
	{
		SetJoystickOn(FALSE);	

		// Move table back to Home & Disbable Limit sensor protection
		if (m_bUseDualTablesOption)
			HomeTable2();
		else
			Y_Home();
		
#ifdef NU_MOTION
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X);
		
		if (m_bUseDualTablesOption)
			CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X2);

		//For Nu Motion, enlarge the software limit before searching limit
		//CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, 999999, -999999, &m_stBTAxis_X);
		CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, 999999, -999999, &m_stBTAxis_Y);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, GMP_PROT_TYPE_SW_LMT, FALSE, FALSE, &m_stBTAxis_Y);

		//X_Profile(LOW_PROF);	//v3.94
		if (m_bUseDualTablesOption)
		{
			Y_Profile(LOW_PROF1);
		}
		else
		{
			Y_Profile(LOW_PROF);
		}
#else
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBTAxis_Y);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBTAxis_X);
#endif

		// Start Search Limit
		if (IsMS60())		//v4.46T28
		{
			//MS60 has NO Y pos-limit sensor, so just bypass here
			nResult = gnOK;
		}
		else if ((nResult = Y_SearchPosLimit()) == gnOK)
		{
			Sleep(300);
			GetEncoderValue();		

			//Leave 1 mm space
			if (m_bUseDualTablesOption)
				m_lTableYPosLimit = m_lEnc_Y - (LONG)(500 * m_dYResolution);	//v4.35T1	//PLLM MS1109
			else
				m_lTableYPosLimit = m_lEnc_Y - (LONG)(1000 * m_dYResolution);

			//Y_Profile(LOW_PROF);	//v4.01
			Sleep(500);
			Y_MoveTo(0, SFM_WAIT);
			Sleep(500);
		}


		//v3.60	//Cree MS899HD-DLA-180
		LONG lNegOffset = GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_NEGLIMIT_OFFSET);
		if ((lNegOffset < 0) || (lNegOffset > 5000))		//in um
			lNegOffset = 0;


		if ( (nResult == gnOK) && (Y_SearchNegLimit() == gnOK) )
		{
			Sleep(300);

			GetEncoderValue();		
//AfxMessageBox("4", MB_SYSTEMMODAL);
			//Leave 1 mm space
			m_lTableYNegLimit = m_lEnc_Y + (LONG)((1000 + lNegOffset) * m_dYResolution);	//v3.60	//Cree MS899HD-DLA-180

			//Y_Profile(LOW_PROF);	//v4.01
			Sleep(500);
			Y_MoveTo(0, SFM_WAIT);
			Sleep(500);

			//v4.46T28
			if (IsMS60() || IsMS90())	//v4.46T28
			{
				m_lTableYPosLimit = m_lTableYNegLimit + 303000;		//293000;	//v4.57A2

				if (bEnableBHUplookPr)	//andrewng123
				{
					m_lTableYPosLimit = m_lTableYNegLimit + 1350000;
				}

				if (IsMS90() && (GetChannelResolution(MS896A_CFG_CH_BINTABLE_X)  == 0.0001))
				{
					//v4.57A7
					LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_MAX_DISTANCE);	
					LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_MIN_DISTANCE);	

					//m_lTableYPosLimit = m_lTableYNegLimit + 1550000;	//1510000;	//v4.57A2
					m_lTableYNegLimit = lMinPos;		//v4.57A6
					m_lTableYPosLimit = lMaxPos;		//v4.57A6
				}
			}

#ifdef NU_MOTION
			X_Profile(NORMAL_PROF);
			Y_Profile(NORMAL_PROF);

			//For Nu Motion, set the software limit
			//CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, m_lTableXPosLimit, m_lTableXNegLimit, &m_stBTAxis_X);
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, m_lTableYPosLimit, m_lTableYNegLimit-2000, &m_stBTAxis_Y);
#endif
		}


		//m_pServo_Y->EnableProtection(HP_LIMIT_SENSOR, TRUE);
		//m_pServo_Y->SetJoystickPositionLimit(m_lTableYNegLimit, m_lTableYPosLimit);
#ifdef NU_MOTION
		if (CMS896AApp::m_bIsPrototypeMachine)
		{
			CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_Y);	//prototype MS100
			CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_X);	//prototype MS100
		}
		else
		{
			CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y);
			CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X);
		}
		
		if (m_bUseDualTablesOption)
			CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, GMP_PROT_TYPE_SW_LMT, TRUE, FALSE, &m_stBTAxis_Y);
#else
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_Y);
		CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_X);
#endif

		CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_Y, m_lTableYNegLimit, m_lTableYPosLimit, &m_stBTAxis_Y);
		SetJoystickOn(TRUE);
		SetStatusMessage("Bintable search Y limit is completed");
		
		CString szLog;
		szLog.Format("BT: Y Table limit is learnt: (%ld, %ld)", m_lTableYPosLimit, m_lTableYNegLimit);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//CheckHiPECResult("BinTableYAxis");
		CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
		bReturn = FALSE;
	}

	(*m_psmfSRam)["BinTable"]["Limit"]["X Pos"]			= m_lTableXPosLimit;
	(*m_psmfSRam)["BinTable"]["Limit"]["X Neg"]			= m_lTableXNegLimit;
	(*m_psmfSRam)["BinTable"]["Limit"]["Y Pos"]			= m_lTableYPosLimit;
	(*m_psmfSRam)["BinTable"]["Limit"]["Y Neg"]			= m_lTableYNegLimit;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	CloseLoadingAlert();
	return 1;
}


LONG CBinTable::SearchY2LimitCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_fHardware)
	{	
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (!m_bUseDualTablesOption)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();

	try
	{
		SetJoystickOn(FALSE);	

		//Sleep(100);
		//Y2_Home();
		//X2_Home();
		if (m_bUseDualTablesOption)
			HomeTable1();


		//For Nu Motion, enlarge the software limit before searching limit
		//CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, 999999, -999999, &m_stBTAxis_Y2);
		CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, 99999999, -99999999, &m_stBTAxis_Y2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y2);
		Y2_Profile(LOW_PROF1);

		// Start Search Limit
		if (Y2_SearchPosLimit() == gnOK)
		{
			Sleep(300);

			GetEncoderValue();		
			//Leave 1 mm space
			m_lTableY2PosLimit = m_lEnc_Y2 - (LONG)(1000 * m_dYResolution);

			//Y2_Profile(LOW_PROF1);	//v4.01
			Sleep(500);
			Y2_MoveTo(0, SFM_WAIT);
			Sleep(500);
		}

		//v3.60	//Cree MS899HD-DLA-180
		LONG lNegOffset = GetChannelInformation(MS896A_CFG_CH_BINTABLE2_Y, MS896A_CFG_CH_NEGLIMIT_OFFSET);
		if ((lNegOffset < 0) || (lNegOffset > 5000))		//in um
			lNegOffset = 0;

		if (Y2_SearchNegLimit() == gnOK)
		{
			Sleep(300);

			GetEncoderValue();		
			//Leave 1 mm space
			if (m_bUseDualTablesOption)
				m_lTableY2NegLimit = m_lEnc_Y2 + (LONG)(500 * m_dYResolution);		//v4.35T1	//PLLM MS109 Rebel
			else
				m_lTableY2NegLimit = m_lEnc_Y2 + (LONG)((1000 + lNegOffset) * m_dYResolution);	//v3.60	//Cree MS899HD-DLA-180

			//Y2_Profile(LOW_PROF1);	//v4.01
			Sleep(500);
			Y2_MoveTo(0, SFM_WAIT);
			Sleep(500);
		}

		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);

		//Y2_Home();
		//Sleep(500);

		//For Nu Motion, set the software limit
		CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, m_lTableY2PosLimit, m_lTableY2NegLimit, &m_stBTAxis_Y2);
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y2);
		CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_Y2, m_lTableY2NegLimit, m_lTableY2PosLimit, &m_stBTAxis_Y2);
			
		SetJoystickOn(TRUE);
		SetStatusMessage("Bintable search Y2 limit is completed");

		CString szLog;
		szLog.Format("BT: Y2 Table limit is learnt: (%ld, %ld)", m_lTableY2PosLimit, m_lTableY2NegLimit);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
		bReturn = FALSE;
	}

	(*m_psmfSRam)["BinTable2"]["Limit"]["X Pos"]			= m_lTableX2PosLimit;
	(*m_psmfSRam)["BinTable2"]["Limit"]["X Neg"]			= m_lTableX2NegLimit;
	(*m_psmfSRam)["BinTable2"]["Limit"]["Y Pos"]			= m_lTableY2PosLimit;
	(*m_psmfSRam)["BinTable2"]["Limit"]["Y Neg"]			= m_lTableY2NegLimit;

	CloseLoadingAlert();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::ConfirmBinTableLimit(IPC_CServiceMessage& svMsg)
{
	if (m_bUseDualTablesOption)	//v4.21T8
	{
		HomeTable1();
		HomeTable2();
	}

	//SaveBinTableData();
	SaveBinTableSetupData();
    SendCE_Setup();
	return 1;
}

LONG CBinTable::CancelBinTableLimit(IPC_CServiceMessage& svMsg)
{
	if (m_bUseDualTablesOption)	//v4.21T8
	{
		HomeTable1();
		HomeTable2();
	}

	InitBinTableRunTimeData();
	InitBinTableSetupData();
	InitBinTableData();
	return 1;
}

//Teach BinTable unload position
LONG CBinTable::TeachLoadPosition(IPC_CServiceMessage& svMsg)
{
	if (!m_fHardware)
	{	
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( IsBLEnable() )
	{
		HmiMessage("Please teach this position under Setup -> Bin Loader -> Gripper", "Bin Table");
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	// Move table back to last saved position

	SetJoystickOn(FALSE);	
	SelectXYProfile(m_lBTUnloadPos_X - m_lCurXPosn, m_lBTUnloadPos_Y - m_lCurYPosn);
	XY_MoveTo(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, SFM_WAIT);
		
	SetJoystickOn(TRUE);	

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::ConfirmLoadPosition(IPC_CServiceMessage& svMsg)
{
	if (!m_fHardware)
	{	
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( IsBLEnable() )
	{
		HmiMessage("Please teach this position under Setup -> Bin Loader -> Gripper", "Bin Table");
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	GetEncoderValue();		

	m_lBTUnloadPos_X = m_lEnc_X;
	m_lBTUnloadPos_Y = m_lEnc_Y;

	SetStatusMessage("Unload binblock position is set");
	//SaveBinTableData();
	SaveBinTableSetupData();

    SendCE_Setup();

	return 1;
}


LONG CBinTable::CancelLoadPosition(IPC_CServiceMessage& svMsg)
{
	if( IsBLEnable() )
	{
		return TRUE;
	}

	InitBinTableRunTimeData();
	InitBinTableData();
	InitBinTableSetupData();
	return 1;
}


LONG CBinTable::GetOutputFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szOutputFilePath);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::GetWaferEndFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szWaferEndPath);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CBinTable::GetOutputFileSummaryPath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szOutputFileSummaryPath);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBinTable::GetWaferEndSummaryFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szWaferEndSummaryPath);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBinTable::GetBinSummaryOutputPath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szBinSummaryOutputPath);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBinTable::MoveToUnloadLoad(IPC_CServiceMessage& svMsg)
{
	if( m_bDisableBT )
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	StartLoadingAlert();	// Show a alert box

#ifdef NU_MOTION
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
#endif

	if (m_bIsUnload == FALSE)
	{
		// temp to disable this function as it will cause software hang up after pressed F5
		XY_MoveTo(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, SFM_WAIT);
		m_bIsUnload = TRUE;
	}
	else
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("Bin XY_MoveTo start 2");
		XY_MoveTo(0, 0, SFM_WAIT);
		//CMSLogFileUtility::Instance()->MS_LogOperation("Bin XY_MoveTo end 2");
		// temp to disable this function as it will cause software hang up after pressed F5
		m_bIsUnload = FALSE;
	}

#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	CloseLoadingAlert();	// Show a alert box

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBinTable::ResetFrameAlign(IPC_CServiceMessage& svMsg)		//v4.59A31
{
	CString szLog;
    ULONG ulPhysicalBlock, i;
    svMsg.GetMsg(sizeof(ULONG), &ulPhysicalBlock);

    for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
    {
        if( m_oBinBlkMain.GrabPhyBlkId(i) == ulPhysicalBlock )
        {
			i = min(i, BT_MAX_BINBLK_SIZE-1);

			szLog.Format("RealignPhysicalBlockCmd Bin #%d (%d) is RESET",  ulPhysicalBlock, i);
			//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);	//v4.59A36

			m_oBinBlkMain.SetIsAligned(i, FALSE);
		}
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//Realign binblock
LONG CBinTable::RealignBinBlockCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;	

	typedef struct
	{
		BOOL bManualMode;
		ULONG ulBlkInUse;
	} BIN_ALIGN;
 	BIN_ALIGN stInfo;

	svMsg.GetMsg(sizeof(BIN_ALIGN), &stInfo);

	//v4.24T6	//ZhongKe
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp == NULL)
	{
		SetErrorMessage("BT: RealignPhysicalBlockCmd NULL pApp pointer !!!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//If LS Bond pattern is enabled, not allow realign binblk at this time	
	if ( m_oBinBlkMain.GrabLSBondPattern() == TRUE )
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsBLBCRepeatCheckFail() )
	{
		HmiMessage_Red_Back("Bin Frame barcode repeated already, please unload it manually!");
		SetErrorMessage("Bin Frame barcode repeated already, please unload it manually!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//v4.50A3
	CString szLog;
	CString szTitle = _T("Bin Realignment");
	if (m_bEnableBinMapBondArea && (m_oBinBlkMain.CalclateBinMapCapacity(stInfo.ulBlkInUse) <= 0))
	{
		szLog.Format("Bin #%d realignment fails because BINMAP capacity = %ld", stInfo.ulBlkInUse,
						m_oBinBlkMain.CalclateBinMapCapacity(stInfo.ulBlkInUse));
		//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog, szTitle);
		
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_oBinBlkMain.GrabNoOfDiePerBlk(stInfo.ulBlkInUse) <= 0)
	{
		szLog.Format("Bin #%d realignment fails because capacity = %ld", stInfo.ulBlkInUse,
						m_oBinBlkMain.CalclateBinMapCapacity(stInfo.ulBlkInUse));
		//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog, szTitle);

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//v3.70T3	//PLLM dual-buffer machine
	LONG lResult = 0;
	switch (m_lRealignBinFrameOption)
	{
	case 1:
		lResult = Realign1PtBinBlock(stInfo.ulBlkInUse, FALSE, stInfo.bManualMode);
		break;
	
	case 2:
		lResult = Realign2PtBinBlock(stInfo.ulBlkInUse, FALSE, stInfo.bManualMode);		
		break;

	case 0:
	default:
		lResult = RealignBinBlock(stInfo.ulBlkInUse, FALSE, stInfo.bManualMode, FALSE, FALSE);		//v4.31T10
		if ( (lResult != TRUE) && (pApp->GetCustomerName() == "ZhongKe") )		//v4.24T6	//ZhongKe
		{
			//v4.24T6	//ZhongKe
			CString szLog;
			szLog = _T("RealignBinBlock failure triggers Realign1PtBinBlock in MANUAL mode (ZhongKe)");
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
			SetErrorMessage(szLog);

			CString szContent = "3-pt re-alignment fails; continue with 1-pt re-alignment?";
			CString szTitle = "Re-Align Bin Frame";
			//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001
			if (HmiMessageEx(szContent, szTitle, glHMI_MBX_CONTINUESTOP) == glHMI_CONTINUE)
			{
				lResult = Realign1PtBinBlock(stInfo.ulBlkInUse, FALSE, stInfo.bManualMode);
			}
		}
		break;
	}

	if (lResult == 1)
	{
		bReturn = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CBinTable::RealignPhysicalBlockCmd(IPC_CServiceMessage& svMsg)
{
	ULONG ulPhysicalBlock = 0;
    svMsg.GetMsg(sizeof(ULONG), &ulPhysicalBlock);

	LONG lRet = SubRealignPhysicalBlockCmd(ulPhysicalBlock);

	BOOL bReturn = (lRet == TRUE) ? TRUE : FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::SubRealignPhysicalBlockCmd(ULONG ulPhysicalBlock)
{
    ULONG i;
	BOOL bReturn = TRUE, bResult;
	LONG lResult;
	CString szLog;

	if( IsBLBCRepeatCheckFail() )
	{
		HmiMessage_Red_Back("Bin Frame barcode repeated already, please unload it manually!");
		SetErrorMessage("Bin Frame barcode repeated already, please unload it manually!");
		return FALSE;
	}

	//v4.24T6	//ZhongKe
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp == NULL)
	{
		SetErrorMessage("BT: RealignPhysicalBlockCmd NULL pApp pointer !!!");
		return FALSE;
	}

	if (pApp->m_bCycleStarted == TRUE && CMS896AApp::m_bEnableSubBin)
	{
		ulPhysicalBlock = (*m_psmfSRam)["BinTable"]["BlkInUse"];
		DisplaySequence("Auto cycle realign (subbin)");
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Auto cycle realign (subbin)", "a+");	
	}
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("RealignPhysicalBlockCmd ...", "a+");

    for(i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
    {
        if( m_oBinBlkMain.GrabPhyBlkId(i)==ulPhysicalBlock )
        {
			i = min(i, BT_MAX_BINBLK_SIZE-1);	//v4.51A20	//Klocwork

			szLog.Format("RealignPhysicalBlockCmd Bin #%d (%d)",  ulPhysicalBlock, i);
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			//v4.50A3
			CString szLog;
			CString szTitle = _T("Bin Realignment");
			if (m_oBinBlkMain.IsUseBinMapBondArea(i) && (m_oBinBlkMain.CalclateBinMapCapacity(i) <= 0))
			{
				szLog.Format("Bin #%d realignment fails because BINMAP capacity = %ld\n", i,
								m_oBinBlkMain.CalclateBinMapCapacity(i));
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog + "\n", "a+");
				SetErrorMessage(szLog);
				HmiMessage_Red_Yellow(szLog, szTitle);
				
				return FALSE;
			}

			if (m_oBinBlkMain.GrabNoOfDiePerBlk(i) <= 0)
			{
			szLog.Format("RealignPhysicalBlockCmd Bin #%d stop because Die capacity = %d\n",  
							ulPhysicalBlock, m_oBinBlkMain.GrabNoOfDiePerBlk(i));
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

				szLog.Format("Bin #%d realignment fails because capacity = %ld", i,
								m_oBinBlkMain.CalclateBinMapCapacity(i));
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog + "\n", "a+");
				SetErrorMessage(szLog);
				HmiMessage_Red_Yellow(szLog, szTitle);

				return FALSE;
			}

			//v4.39T7	//Nichia MS100+
			if (m_bEnable_T)
			{
				ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(i);

				szLog.Format("RealignPhysicalBlockCmd Bin #%d with T, die count = %d ...",  
							ulPhysicalBlock, ulNoOfSortedDie);
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

				//if (ulNoOfSortedDie == 0 && pApp->GetFeatureStatus(MS896A_FUNC_BIN_FRAME_CROSS))
				if (pApp->GetFeatureStatus(MS896A_FUNC_BIN_FRAME_CROSS) && 
					m_bUseFrameCrossAlignment)	//v4.59A1
				{
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("AlignGlobalThetaAngleByBinFrame ...", "a+");

					if (!BT_SetFrameRotation(i, TRUE))
					{
						CString szLog;
						szLog = _T("BT RealignPhysicalBlockCmd: Frame Rotate Failure");
						SetErrorMessage(szLog);
						return FALSE;
					}

					bResult = AlignGlobalThetaAngleByBinFrame(i);
					if (bResult == FALSE)
					{
						CString szLog;
						szLog = _T("BT RealignPhysicalBlockCmd: global T alignment with cross fails");
						SetErrorMessage(szLog);
						return FALSE;
					}
				}
				else 
				{
					if (IsMS90())
					{
						//shiraishi01
						if (!AlignFrameOrientation(i))
						{
							CString szLog;
							szLog = _T("BT RealignPhysicalBlockCmd: AlignFrameOrientation 1");
							SetErrorMessage(szLog);
							return FALSE;
						}

						//v4.59A31
						CString szLog;
						BOOL bSoftRealign = (BOOL)(LONG)(*m_psmfSRam)["BinLoader"]["SoftRealign"];
						//v4.59A36
						INT nSortPart		= (LONG)(*m_psmfSRam)["WaferTable"]["MS9oWaferSortPart"];	//0=FAIL, 1=North, 2=South

						if (m_bIsRotate180Deg || IsMS90Sorting2ndPart())
						{
							if (bSoftRealign)	//v4.59A36
							{
								//v4.59A41	//Finisar TX
								BOOL bSoftRealignTimeOut = FALSE;
								CTimeSpan stTimeDiff = CTime::GetCurrentTime() - m_stMachineStopTime;
								if (stTimeDiff.GetTotalMinutes() > 30)
								{
									bSoftRealignTimeOut = TRUE;
									bSoftRealign = FALSE;
								}

								szLog.Format("BT SoftRealign 180: Rot180=%d, Sort2ndPart=%d, FRotate=%d, IsAligned=%d, ALIGNDONE=%d, WTSortPart=%d, TimeOut=%d", 
												m_bIsRotate180Deg, IsMS90Sorting2ndPart(),		
												m_oBinBlkMain.GetFrameRotation(i),
												m_oBinBlkMain.GrabNVIsAligned(i),
												CMS896AStn::m_bRealignFrameDone,
												nSortPart,
												bSoftRealignTimeOut);
								CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
							}

							//v4.59A31
							if ( (m_oBinBlkMain.GetFrameRotation(i) == TRUE) &&	//already set as 180 degree
								 (m_oBinBlkMain.GrabNVIsAligned(i) == TRUE) &&
								 bSoftRealign )
							{
								if ( (CMS896AStn::m_bRealignFrameDone == TRUE) && (nSortPart == 2) )	//v4.59A35
								{
									szLog = "RealignPhysicalBlockCmd ALIGN 180 degree stop because already ALIGNED";
									CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	
									return TRUE;
								}
							}

							BOOL bSetStatus = BT_SetFrameRotation(i, TRUE, FALSE);
							if (!bSetStatus)	//v4.59A45
							{
								szLog = "RealignPhysicalBlockCmd ALIGN 180 degree stop because BT_SetFrameRotation fail";
								CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

								return TRUE;
							}
						}
						else
						{
							if (bSoftRealign)	//v4.59A36
							{
								//v4.59A41	//Finisar TX
								BOOL bSoftRealignTimeOut = FALSE;
								CTimeSpan stTimeDiff = CTime::GetCurrentTime() - m_stMachineStopTime;
								if (stTimeDiff.GetTotalMinutes() > 30)
								{
									bSoftRealignTimeOut = TRUE;
									bSoftRealign = FALSE;
								}

								szLog.Format("BT SoftRealign 0: Rot180=%d, Sort2ndPart=%d, FRotate=%d, IsAligned=%d, ALIGNDONE=%d, WTSortPart=%d, TimeOut=%d", 
												m_bIsRotate180Deg, IsMS90Sorting2ndPart(),		
												m_oBinBlkMain.GetFrameRotation(i),
												m_oBinBlkMain.GrabNVIsAligned(i),
												CMS896AStn::m_bRealignFrameDone,
												nSortPart,
												bSoftRealignTimeOut);
								CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
							}

							//v4.59A31
							if ( (m_oBinBlkMain.GetFrameRotation(i) == FALSE) &&	//already set as 180 degree
								 (m_oBinBlkMain.GrabNVIsAligned(i) == TRUE) &&
								 bSoftRealign )
							{
								if ( (CMS896AStn::m_bRealignFrameDone == TRUE) && (nSortPart == 1) )	//v4.59A35
								{
									szLog = "RealignPhysicalBlockCmd ALIGN 0 degree stop because already ALIGNED";
									CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	
									return TRUE;
								}
							}

							if (!BT_SetFrameRotation(i, FALSE, FALSE)) //0 degree
							{
								szLog = "RealignPhysicalBlockCmd ALIGN 0 degree stop because T_MoveTo fail";
								CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
								return TRUE;
							}
						}
						if (IsMS90())
						{
							CMSLogFileUtility::Instance()->BT_TableIndexLog("update index");
							OpIndex_MS90();//Finisar//Update the move position and row col after rotation
						}
					}
/*
					bResult = AlignGlobalThetaAngle(i);
					if (bResult == FALSE)
					{
						CString szLog;
						szLog = _T("BT RealignPhysicalBlockCmd: global T alignment fails 1");
						SetErrorMessage(szLog);
						HmiMessage_Red_Yellow(szLog, "Bin Frame Realignment");
						bReturn = FALSE;
						svMsg.InitMessage(sizeof(BOOL), &bReturn);
						return 1;
					}

					bResult = AlignGlobalThetaAngle(i);
					if (bResult == FALSE)
					{
						CString szLog;
						szLog = _T("BT RealignPhysicalBlockCmd: global T alignment fails 2");
						SetErrorMessage(szLog);
						HmiMessage_Red_Yellow(szLog, "Bin Frame Realignment");
						bReturn = FALSE;
						svMsg.InitMessage(sizeof(BOOL), &bReturn);
						return 1;
					}
*/
					//if (!m_bAlignBinInAlive)	//only perform BC scanning for BT T in AUTOBOND mode
					//{
					if (!IsMS90())		//v4.49A10	//MS90 reads BC on buffer table instead
					{
						bResult = ScanningBarcode(i);
						if (bResult == FALSE)
						{
							CString szLog;
							szLog = _T("BT RealignPhysicalBlockCmd: ScanningBarcode fails");
							SetErrorMessage(szLog);
							return FALSE;
						}
					}
				}
			}
			else if (pApp->GetFeatureStatus(MS896A_FUNC_BIN_FRAME_CROSS) && 
						m_bUseFrameCrossAlignment)	//v4.59A1
			{
				ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(i);
				if (ulNoOfSortedDie == 0)
				{
					bResult = AlignGlobalThetaAngleByBinFrame(i);
					if (bResult == FALSE)
					{
						CString szLog;
						szLog = _T("BT RealignPhysicalBlockCmd: global T alignment with cross fails");
						SetErrorMessage(szLog);
						return FALSE;
					}
				}
			}

			//v3.70T3	//PLLM dual-buffer machine
			switch (m_lRealignBinFrameOption)
			{
			case 1:
				bResult = Realign1PtBinBlock(i, TRUE, TRUE);
				break;
			
			case 2:
				bResult = Realign2PtBinBlock(i, TRUE, TRUE);		
				break;

			case 0:
			default:
				if (pApp->GetCustomerName() == "ZhongKe")
				{
					bResult = RealignBinBlock(i, TRUE, TRUE, TRUE);		//Disable-Alarm option is ENABLED
					if (IsBurnIn())
					{
						bReturn = TRUE;
					}
					if (bResult != TRUE)
					{
						//v4.24T6	//ZhongKe
						CString szLog;
						szLog = _T("3-pt re-alignment failure triggers 1-pt re-alignment (ZhongKe)");
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
						SetErrorMessage(szLog);

						bResult = Realign1PtBinBlock(i, TRUE, TRUE);
					}
				}
				else
				{
					bResult = RealignBinBlock(i, TRUE, TRUE, FALSE, FALSE);		//v4.31T10

					if ((bResult != HMB_BT_REALIGN_STEP1) && (bResult != HMB_BT_REALIGN_STEP2) && (bResult != IDS_BPR_DIENOTLEARNT) && (bResult != TRUE) && IsMS90())	//v4.59A31
					{
						lResult = RealignBinBlock(i, TRUE, TRUE, FALSE, FALSE, 1, TRUE);	
					}
				}
				break;
			}

	        if (bResult != TRUE)
	        {
		        bReturn = bResult;
				if (IsBurnIn())
				{
					bReturn = TRUE;
				}
	        }
			else
			{
				SetBTReady(TRUE);

				m_stMachineStopTime	= CTime::GetCurrentTime();
			}
			break;
        }
    }
	if (lResult == IDS_BT_REALIGN_NO_LAST_DIE)
	{
		return IDS_BT_REALIGN_NO_LAST_DIE;//Prevent duplicate alarm.
	}

	return bReturn;
}

/*
LONG CBinTable::RealignPhysicalBlock2Cmd(IPC_CServiceMessage& svMsg)
{
	ULONG ulPhysicalBlock;
	svMsg.GetMsg(sizeof(ULONG), &ulPhysicalBlock);

	BOOL bReturn = SubRealignPhysicalBlock2Cmd(ulPhysicalBlock);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBinTable::SubRealignPhysicalBlock2Cmd(const ULONG ulPhysicalBlock)
{
    ULONG i;
	BOOL bReturn = TRUE, bResult;

	if (!m_bUseDualTablesOption)
	{
		return FALSE;
	}

	if( IsBLBCRepeatCheckFail() )
	{
		HmiMessage_Red_Back("Bin Frame barcode repeated already, please unload it manually!");
		SetErrorMessage("Bin Frame barcode repeated already, please unload it manually!");
		return FALSE;
	}

     for(i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
    {
        if( m_oBinBlkMain.GrabPhyBlkId(i) == ulPhysicalBlock )
        {
			//v3.70T3	//PLLM dual-buffer machine
			switch (m_lRealignBinFrameOption)
			{
			case 1:
				bResult = Realign1PtBinBlock2(i, TRUE, TRUE);
				break;
			
			case 2:
				//bResult = Realign2PtBinBlock(i, TRUE, TRUE);		
				//break;

			case 0:
			default:
				bResult = RealignBinBlock2(i, TRUE, TRUE);
				break;
			}

	        if(bResult != TRUE)
	        {
		        bReturn = FALSE;
				if (IsBurnIn())	//v3.84
					bReturn = TRUE;
	        }
			else
			{
				SetBTReady(TRUE);
			}
			break;
        }
    }

	return bReturn;
}
*/

LONG CBinTable::RealignEmptyFrameCmd(IPC_CServiceMessage& svMsg)
{
	ULONG ulPhysicalBlock = 0;
	svMsg.GetMsg(sizeof(ULONG), &ulPhysicalBlock);

	LONG lRet = SubRealignEmptyFrameCmd(ulPhysicalBlock);

	BOOL bReturn = (lRet == TRUE) ? TRUE : FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::SubRealignEmptyFrameCmd(const ULONG ulPhysicalBlock)
{
	BOOL bReturn = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//v4.39T7	//Nichia MS100+
	if (m_bEnable_T && !m_bAlignBinInAlive)	//only perform BC scanning for BT T in AUTOBOND mode
	{
		CMSLogFileUtility::Instance()->BL_LogStatus("BT: Start RealignEmptyFrameCmd ...");

		if (IsMS90())
		{
			//shiraishi02
			if (!AlignFrameOrientation(ulPhysicalBlock))
			{
				CString szLog;
				szLog = _T("BT RealignPhysicalBlockCmd: AlignFrameOrientation 1");
				SetErrorMessage(szLog);
				return FALSE;
			}

			BT_SetFrameRotation(ulPhysicalBlock, TRUE);
		}
		
		if (!IsMS90())	//v4.49A10
		{
			bReturn = ScanningBarcode(ulPhysicalBlock, TRUE);
			if (bReturn == FALSE)
			{
				CString szLog;
				szLog = _T("BT RealignEmptyFrameCmd: ScanningBarcode fails");
				SetErrorMessage(szLog);
				bReturn = FALSE;	//v4.40T7
			}
			else if (pApp->GetFeatureStatus(MS896A_FUNC_BIN_FRAME_CROSS) && m_bUseFrameCrossAlignment)	//v4.59A1
			{
				bReturn = AlignGlobalThetaAngleByBinFrame(0,TRUE);
				if ( bReturn == FALSE )
				{
					SetErrorMessage("BT realign with cross: Rotate Empty Frame Fail");
				}
			}
		}
	}

	//v4.50A3	//WH SanAn
	CString szLog;
	CString szTitle = _T("Bin Realignment");
	ULONG i = ulPhysicalBlock;
	if (m_oBinBlkMain.IsUseBinMapBondArea(i) && (m_oBinBlkMain.CalclateBinMapCapacity(i) <= 0))
	{
		szLog.Format("Bin #%d realignment fails because BINMAP capacity = %ld", i,
						m_oBinBlkMain.CalclateBinMapCapacity(i));
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog + "\n", "a+");
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog, szTitle);
		
		return FALSE;
	}

	if (m_oBinBlkMain.GrabNoOfDiePerBlk(i) <= 0)
	{
		szLog.Format("Bin #%d realignment fails because capacity = %ld", i,
						m_oBinBlkMain.CalclateBinMapCapacity(i));
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog + "\n", "a+");
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog, szTitle);

		return FALSE;
	}

	if (IsMS90())
	{
		OpIndex_MS90();//Finisar//Update the move position and row col after rotation
	}

	SetBTReady(TRUE);
	(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= TRUE;	
	return bReturn;
}


LONG CBinTable::ResetFrameIsAligned(IPC_CServiceMessage& svMsg)	//v4.42T10
{
	BOOL bReturn = TRUE;
	ULONG ulPhysicalBlock = 0;
	svMsg.GetMsg(sizeof(ULONG), &ulPhysicalBlock);

	for (ULONG i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if( m_oBinBlkMain.GrabPhyBlkId(i) == ulPhysicalBlock )
		{
			m_oBinBlkMain.SetIsAligned(ulPhysicalBlock, FALSE);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::HomeTableForRealign_BL(IPC_CServiceMessage& svMsg)	//v4.17T6
{
	BOOL bReturn = TRUE;

	ULONG ulTable = 0;
	svMsg.GetMsg(sizeof(ULONG), &ulTable);

	SetJoystickOn(FALSE);
	if (ulTable == 1)
	{
		if (m_bUseDualTablesOption)
			HomeTable2();
	}
	else
	{
		HomeTable1();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::IsBT2UnderCamera(IPC_CServiceMessage& svMsg)
{
	BOOL bBT2UnderCamera = FALSE;

	if (!m_bUseDualTablesOption)
	{
		svMsg.InitMessage(sizeof(BOOL), &bBT2UnderCamera);
		return 1;
	}
	if ( (m_lTableY2PosLimit == 0) || (m_lTableY2NegLimit == 0) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bBT2UnderCamera);
		return 1;
	}

	GetEncoderValue();
	LONG lBT2EncY = m_lEnc_Y2;

	if ( lBT2EncY < (m_lTableY2PosLimit - (m_lTableY2PosLimit - m_lTableY2NegLimit) / 2) )
		bBT2UnderCamera = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bBT2UnderCamera);
	return 1;
}

LONG CBinTable::SaveReferenceCrossEncoderValue(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	GetEncoderValue();

	m_lEncRefCross_X = m_lEnc_X;
	m_lEncRefCross_Y = m_lEnc_Y;

	CString A;
	//A.Format("1 %f %f %d %d", m_dBinTwoCrossXDistance, m_dBinTwoCrossYDistance, m_lEncRefCross_X, m_lEncRefCross_Y);
	//HmiMessage(A);

	if ( m_bEnable_T ) 
	{
		m_lEncRefCross_T = m_lEnc_T;
	}

	if ( m_dBinTwoCrossXDistance != -1.0 && m_dBinTwoCrossYDistance != -1.0 )
	{
		// (LONG)m_dXResolution*m_dBinTwoCrossXDistance
		// (LONG)m_dYResolution*m_dBinTwoCrossYDistance
		SaveReferenceCross(m_dBinTwoCrossXDistance, m_dBinTwoCrossYDistance, m_lEncRefCross_X, m_lEncRefCross_Y);
	}
	else 
	{
		HmiMessage("Please input the Length and Width of the Bin Cross!");
	}
	//A.Format("1 %f %f %d %d", m_dBinTwoCrossXDistance, m_dBinTwoCrossYDistance, m_lEncRefCross_X, m_lEncRefCross_Y);
	//HmiMessage(A);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CBinTable::TEST_AlignGlobalThetaAngleByBinFrame(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	AlignGlobalThetaAngleByBinFrame(1);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}
/*
//BOOL CBinTable::AlignGlobalThetaAngleByBinFrame(ULONG ulBlkID)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CMSLogFileUtility::Instance()->BL_LogStatus("*AlignThetaAngleByCross: Start");

CString k;

	if (!pApp->GetFeatureStatus(MS896A_FUNC_BIN_FRAME_CROSS))
		return FALSE;

    ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkID);
	if (ulNoOfSortedDie != 0)
	{
		CMSLogFileUtility::Instance()->BL_LogStatus("*AlignThetaAngleByCross: Have Die in it");

		AlignGlobalThetaAngle(ulBlkID);
		return TRUE;
	}

	if ( m_dBinTwoCrossXDistance <= 0 || m_dBinTwoCrossYDistance <= 0 )
	{
		CMSLogFileUtility::Instance()->BL_LogStatus("*AlignThetaAngleByCross: Cross X or Y zero");
		return FALSE;
	}

	SetJoystickOn(FALSE);

	DOUBLE dTheta = 0.0; // target

k.Format( "GO1 %d %d", m_lEncRefCross_X, m_lEncRefCross_Y);
CMSLogFileUtility::Instance()->BL_LogStatus(k);
//HmiMessage(k);

	// Move to cross 1 and calculate slope, offset1
	BT_MoveTo(m_lEncRefCross_X, m_lEncRefCross_Y);
	Sleep(m_lAlignBinDelay*6);


	LONG lNewCross1X, lNewCross1Y;
	LONG lNewCross2X, lNewCross2Y;

	LONG lEncRefCross2X, lEncRefCross2Y;

	DOUBLE dSlopeM1, dSlopeM2;
	dSlopeM1 = m_dBinTwoCrossYDistance/m_dBinTwoCrossXDistance;

CString szTetraTemp;
szTetraTemp.Format("m_lEncRefCross_X:%d , m_lEncRefCross_Y:%d, dSlopeM1:%f", m_lEncRefCross_X, m_lEncRefCross_Y, dSlopeM1);
CMSLogFileUtility::Instance()->BL_LogStatus("*AlignThetaAngleByCross: Search Die");
CMSLogFileUtility::Instance()->BL_LogStatus(szTetraTemp);

	{ // Find ref die
		typedef struct 
		{
			int		siStepX;
			int		siStepY;
			BOOL    bResult;
		} BPR_DIEOFFSET;
		BPR_DIEOFFSET stInfo;

		int nConvID = 0;
		IPC_CServiceMessage stMsg;
		stInfo.bResult = FALSE;

		// Get the reply
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDieInFOV", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (stInfo.bResult == FALSE)
		{
			HmiMessage_Red_Back( "Search first reference cross failed." );
			CMSLogFileUtility::Instance()->BL_LogStatus("*AlignThetaAngleByCross: Search first reference cross failed.");
			return FALSE;
		}
		else 
		{
			lNewCross1X = stInfo.siStepX;
			lNewCross1Y = stInfo.siStepY;
		}
	}

k.Format( "CROSS1 OFFSET: %d %d , RESULT COOR: %d %d", lNewCross1X, lNewCross1Y, lNewCross1X + m_lEncRefCross_X, lNewCross1Y + m_lEncRefCross_Y);
CMSLogFileUtility::Instance()->BL_LogStatus(k);
//HmiMessage(k);

	//ConvertPixelToMotorStep( stDieOffset, &lNewCross1X, &lNewCross1Y);
	lNewCross1X = lNewCross1X + m_lEncRefCross_X;
	lNewCross1Y = lNewCross1Y + m_lEncRefCross_Y;

	lEncRefCross2X = m_lEncRefCross_X - (LONG)m_dXResolution*10*1000*m_dBinTwoCrossXDistance;
	lEncRefCross2Y = m_lEncRefCross_Y - (LONG)m_dYResolution*10*1000*m_dBinTwoCrossYDistance;
	// Then move to cross 2 by calculation

k.Format( "GO2(cross2 est. point): %d %d", lEncRefCross2X, lEncRefCross2Y);
CMSLogFileUtility::Instance()->BL_LogStatus(k);
//HmiMessage(k);

	BT_MoveTo( lEncRefCross2X, lEncRefCross2Y );
	Sleep(m_lAlignBinDelay*6);

	// find offset

	{ // Find ref die
		typedef struct 
		{
			int		siStepX;
			int		siStepY;
			BOOL    bResult;
		} BPR_DIEOFFSET;
		BPR_DIEOFFSET stInfo;

		int nConvID = 0;
		IPC_CServiceMessage stMsg;
		stInfo.bResult = FALSE;

		// Get the reply
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDieInFOV", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (stInfo.bResult == FALSE)
		{
			HmiMessage_Red_Back( "Search diagonal refence cross failed." );
			CMSLogFileUtility::Instance()->BL_LogStatus("*AlignThetaAngleByCross: Search diagonal refence cross failed.");
			return FALSE;
		}
		else 
		{
			lNewCross2X = stInfo.siStepX;
			lNewCross2Y = stInfo.siStepY;
		}
	}

k.Format( "CROSS2 OFFSET: %d %d , RESULT COOR: %d %d", lNewCross2X, lNewCross2Y, lNewCross2X + lEncRefCross2X, lNewCross2Y + lEncRefCross2Y);
CMSLogFileUtility::Instance()->BL_LogStatus(k);


	lNewCross2X = lNewCross2X + lEncRefCross2X;
	lNewCross2Y = lNewCross2Y + lEncRefCross2Y;

	// slope = y1 - y2 / x1 - x2
	double tempA = (DOUBLE)(lNewCross1Y - lNewCross2Y);
	double tempB = (DOUBLE)(lNewCross1X - lNewCross2X);
	dSlopeM2 = tempA/tempB;

	// tan(theta) = abs( (m1-m2)/(1+m1m2) )  //accue angle only
	if ( dSlopeM2 * dSlopeM1 == -1.0 )
	{
		dTheta	= PI/2;
	}
	else 
	{
		dTheta	= atan( fabs((dSlopeM2 - dSlopeM1) / (1.0 + dSlopeM2 * dSlopeM1)) );
		//dTheta  = dTheta*180.0/PI;
	}

	if ( dTheta*180.0/PI > 0.5 )
	{
		CString szAngleLarge;
		szAngleLarge.Format( "Angle too large: %f", dTheta*180.0/PI);
		HmiMessage_Red_Back( szAngleLarge );
		return FALSE;
	}

	m_dThetaOffsetByCross = dTheta;
	if ( dSlopeM2 < dSlopeM1 )
	{
		m_dThetaOffsetByCross = m_dThetaOffsetByCross*(-1.0) ;
	}
	SaveReferenceCross(m_dBinTwoCrossXDistance, m_dBinTwoCrossYDistance, m_lEncRefCross_X, m_lEncRefCross_Y);


k.Format( "FINISH %f %f %f %f" , dTheta ,m_dThetaOffsetByCross,dSlopeM1,dSlopeM2);
CMSLogFileUtility::Instance()->BL_LogStatus(k);


	CMSLogFileUtility::Instance()->BL_LogStatus("BT: Start align global T(BY FRAME CROSS) ...");
	
	if (m_bEnable_T && T_IsPowerOn() && m_bSel_T)
	{
		T_MoveByDegree(dTheta);
	}
	else 
	{
		m_oBinBlkMain.SetRotateAngleX(ulBlkID, m_dThetaOffsetByCross);
	}

	return TRUE;
}
*/
BOOL CBinTable::AlignGlobalThetaAngleByBinFrame(ULONG ulBlkID, BOOL bIsEmpty)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString k;
/*
	if (!m_bEnable_T)
		return FALSE;
	if (!m_bSel_T)
		return TRUE;
	if (!T_IsPowerOn())
		return FALSE;
*/

	if (!pApp->GetFeatureStatus(MS896A_FUNC_BIN_FRAME_CROSS))
		return FALSE;
	if (!m_bUseFrameCrossAlignment)		//v4.59A1
		return FALSE;

    ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkID);
	if (ulNoOfSortedDie != 0 && bIsEmpty == FALSE)
	{
		return AlignGlobalThetaAngle(ulBlkID);
	}

	SetJoystickOn(FALSE);

	LONG lCurrX=0, lCurrY=0;
	DOUBLE dSlopeM1, dSlopeM2;
	DOUBLE dTheta = 0.0;		//target
	DOUBLE dThetaInRadian = 0.0;

	// Move to cross 1 and calculate slope, offset1
	//v4.57A8
	k.Format("Substrate Align #1: move to Ref #1 position at (%ld, %ld) ...", m_lEncRefCross_X, m_lEncRefCross_Y);
	//HmiMessage(k);

	BT_MoveTo(m_lEncRefCross_X, m_lEncRefCross_Y);
	Sleep(m_lAlignBinDelay*6);
	LONG lNewCross1X, lNewCross1Y;
	LONG lNewCross2X, lNewCross2Y;
	LONG lEncRefCross2X, lEncRefCross2Y;

	if ( ( m_dBinTwoCrossXDistance == -1.0 || m_dBinTwoCrossYDistance == -1.0 ) ||
		 ( m_dBinTwoCrossXDistance == 0.0 && m_dBinTwoCrossYDistance == 0.0 ) )
	{
		HmiMessage_Red_Back("Please input the Length and Width of the Bin Cross!");
		return FALSE;
	}
	//Use single Frame Cross in this case
	else if ( (m_dBinTwoCrossXDistance == 0.0) && (m_dBinTwoCrossYDistance == 0.0) )	//v4.59A4
	{
		typedef struct 
		{
			int		siStepX;
			int		siStepY;
			DOUBLE	dAngle;		//v4.59A4
			BOOL    bResult;
		} BPR_DIEOFFSET;
		BPR_DIEOFFSET stInfo;

		int nConvID = 0;
		IPC_CServiceMessage stMsg;
		stInfo.bResult = FALSE;

		// Get the reply
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDie2InFOV", stMsg);	//v4.58A5
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (stInfo.bResult == FALSE)
		{
			HmiMessage_Red_Back( "Search single reference cross failed." );
			return FALSE;
		}
		else 
		{
			lNewCross1X = stInfo.siStepX;
			lNewCross1Y = stInfo.siStepY;
		}

		lNewCross1X = lNewCross1X + m_lEncRefCross_X;
		lNewCross1Y = lNewCross1Y + m_lEncRefCross_Y;
		BT_MoveTo(lNewCross1X, lNewCross1Y);
		Sleep(m_lAlignBinDelay);

		if (fabs(stInfo.dAngle) > 3.0)	//Allow 3 degree rotation in CC & CCW direction
		{
			CString szAngleLarge;
			szAngleLarge.Format( "Frame cross angle is too large - %.6f degree (max. 3 degree)", stInfo.dAngle);
			HmiMessage_Red_Back( szAngleLarge );
			return FALSE;
		}

		CMSLogFileUtility::Instance()->BL_LogStatus("BT: END align global T(BY SINGLE FRAME CROSS)");
		
		if (m_bEnable_T && T_IsPowerOn() && m_bSel_T)
		{
			T_MoveByDegree(stInfo.dAngle);
		}
		else 
		{
			m_oBinBlkMain.SetRotateAngleX(ulBlkID, stInfo.dAngle);
		}

		return TRUE;

	}
	//Use 2 Frame cross but NOT at diagonal positions
	else if ( m_dBinTwoCrossXDistance == 0.0 || m_dBinTwoCrossYDistance == 0.0 )
	{
		DOUBLE dSlopeM = 0.0;
		{ // Find ref die
			typedef struct 
			{
				int		siStepX;
				int		siStepY;
				DOUBLE	dAngle;		//v4.59A4
				BOOL    bResult;
			} BPR_DIEOFFSET;
			BPR_DIEOFFSET stInfo;

			int nConvID = 0;
			IPC_CServiceMessage stMsg;
			stInfo.bResult = FALSE;

			// Get the reply
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDie2InFOV", stMsg);	//v4.58A5
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (stInfo.bResult == FALSE)
			{
				HmiMessage_Red_Back( "Search first reference cross failed." );
				return FALSE;
			}
			else 
			{
				lNewCross1X = stInfo.siStepX;
				lNewCross1Y = stInfo.siStepY;
			}
		}
		lNewCross1X = lNewCross1X + m_lEncRefCross_X;
		lNewCross1Y = lNewCross1Y + m_lEncRefCross_Y;

		lEncRefCross2X = m_lEncRefCross_X - (LONG)(m_dXResolution*10.0*1000.0*m_dBinTwoCrossXDistance);
		lEncRefCross2Y = m_lEncRefCross_Y - (LONG)(m_dYResolution*10.0*1000.0*m_dBinTwoCrossYDistance);

		BT_MoveTo( lEncRefCross2X, lEncRefCross2Y );
		Sleep(m_lAlignBinDelay*6);

		// find offset

		{ // Find ref die
			typedef struct 
			{
				int		siStepX;
				int		siStepY;
				DOUBLE	dAngle;		//v4.59A4
				BOOL    bResult;
			} BPR_DIEOFFSET;
			BPR_DIEOFFSET stInfo;

			int nConvID = 0;
			IPC_CServiceMessage stMsg;
			stInfo.bResult = FALSE;

			// Get the reply
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDie2InFOV", stMsg);	//v4.58A5
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (stInfo.bResult == FALSE)
			{
				HmiMessage_Red_Back( "Search diagonal reference cross failed." );
				return FALSE;
			}
			else 
			{
				lNewCross2X = stInfo.siStepX;
				lNewCross2Y = stInfo.siStepY;
			}
		}
		lNewCross2X = lNewCross2X + lEncRefCross2X;
		lNewCross2Y = lNewCross2Y + lEncRefCross2Y;	

		// slope = y1 - y2 / x1 - x2
		double tempA = (DOUBLE)(lNewCross1Y - lNewCross2Y);
		double tempB = (DOUBLE)(lNewCross1X - lNewCross2X);
		//dSlopeM = tempA/tempB;

		if ( m_dBinTwoCrossXDistance == 0.0 )
		{
			if ( tempB == 0.0 )
			{
				dTheta = 0.0;
			}
			else 
			{
				dTheta = atan( fabs(tempB/tempA) );
				if ( tempB < 0 )
				{
					dTheta = dTheta*(-1);
				}
			}
		}
		else if ( m_dBinTwoCrossYDistance == 0.0 )
		{
			dTheta = atan( fabs(tempA/tempB) );
			if ( tempA/tempB < 0.0 )
			{
				dTheta = dTheta*(-1);
			}
		}
	}
	//Use 2 Frame cross but at diagonal positions
	else
	{
		dSlopeM1 = m_dBinTwoCrossYDistance/m_dBinTwoCrossXDistance;

		{ // Find ref die
			typedef struct 
			{
				int		siStepX;
				int		siStepY;
				DOUBLE	dAngle;		//v4.59A4
				BOOL    bResult;
			} BPR_DIEOFFSET;
			BPR_DIEOFFSET stInfo;

			int nConvID = 0;
			IPC_CServiceMessage stMsg;
			stInfo.bResult = FALSE;

			// Get the reply
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDie2InFOV", stMsg);		//v4.58A5
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (stInfo.bResult == FALSE)
			{
				HmiMessage_Red_Back( "Search first reference cross failed." );
				return FALSE;
			}
			else 
			{
				lNewCross1X = stInfo.siStepX;
				lNewCross1Y = stInfo.siStepY;

				//v4.57A8
				GetXYEncoderValue(lCurrX, lCurrY);
				BT_MoveTo(lCurrX + stInfo.siStepX, lCurrY + stInfo.siStepY);

				//k.Format("Substrate Align #2 result: Ref #1 final encoder = (%ld, %ld), original = (%d, %ld)", 
				//lCurrX + stInfo.siStepX, lCurrY + stInfo.siStepY, lCurrX, lCurrY);
//HmiMessage(k);
			}
		}

		//ConvertPixelToMotorStep( stDieOffset, &lNewCross1X, &lNewCross1Y);
		lNewCross1X = lNewCross1X + m_lEncRefCross_X;
		lNewCross1Y = lNewCross1Y + m_lEncRefCross_Y;

		lEncRefCross2X = m_lEncRefCross_X - (LONG)(m_dXResolution*10*1000*m_dBinTwoCrossXDistance);
		lEncRefCross2Y = m_lEncRefCross_Y - (LONG)(m_dYResolution*10*1000*m_dBinTwoCrossYDistance);
		// Then move to cross 2 by calculation

		//v4.57A8
		k.Format("Substrate Align #3: move to Ref #2 position at (%ld, %ld) ...", lEncRefCross2X, lEncRefCross2Y);
		//HmiMessage(k);

		BT_MoveTo( lEncRefCross2X, lEncRefCross2Y );
		Sleep(m_lAlignBinDelay*6);

		// find offset

		{ // Find ref die
			typedef struct 
			{
				int		siStepX;
				int		siStepY;
				DOUBLE	dAngle;		//v4.59A4
				BOOL    bResult;
			} BPR_DIEOFFSET;
			BPR_DIEOFFSET stInfo;

			int nConvID = 0;
			IPC_CServiceMessage stMsg;
			stInfo.bResult = FALSE;

			// Get the reply
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDie2InFOV", stMsg);	//v4.58A5
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (stInfo.bResult == FALSE)
			{
				HmiMessage_Red_Back( "Search diagonal reference cross failed." );
				return FALSE;
			}
			else 
			{
				lNewCross2X = stInfo.siStepX;
				lNewCross2Y = stInfo.siStepY;

				//v4.57A8
				GetXYEncoderValue(lCurrX, lCurrY);
				BT_MoveTo(lCurrX + stInfo.siStepX, lCurrY + stInfo.siStepY);

				k.Format("Substrate Align #4 result: Ref #2 final encoder = (%ld, %ld), original = (%ld, %ld)", 
								lCurrX + stInfo.siStepX, lCurrY + stInfo.siStepY, lCurrX, lCurrY);
				//HmiMessage(k);
			}
		}


		lNewCross2X = lNewCross2X + lEncRefCross2X;
		lNewCross2Y = lNewCross2Y + lEncRefCross2Y;	

		// slope = y1 - y2 / x1 - x2
		double tempA = (DOUBLE)(lNewCross1Y - lNewCross2Y);
		double tempB = (DOUBLE)(lNewCross1X - lNewCross2X);
		dSlopeM2 = tempA/tempB;

k.Format("dSlopeM1 = %.6f = %.6f / %.6f (both input in Bond PR menu)", 
		 dSlopeM1, m_dBinTwoCrossYDistance, m_dBinTwoCrossXDistance);
//HmiMessage(k);

k.Format("dSlopeM2 = %.6f", dSlopeM2);
//HmiMessage(k);

		// tan(theta) = abs( (m1-m2)/(1+m1m2) )  //accue angle only
		if ( dSlopeM2 * dSlopeM1 == -1.0 )
		{
			dTheta	= PI/2;
			dThetaInRadian = dTheta;
			dTheta  = dTheta * 180.0 / PI;		//convert from radian to degree
		}
		else 
		{
			dTheta	= atan( fabs((dSlopeM2 - dSlopeM1) / (1.0 + dSlopeM2 * dSlopeM1)) );
			dThetaInRadian = dTheta;
			dTheta  = dTheta * 180.0 / PI;		//convert from radian to degree
		}

		m_dThetaOffsetByCross = dTheta;
		if ( dSlopeM2 < dSlopeM1 )
		{
			m_dThetaOffsetByCross = m_dThetaOffsetByCross*(-1.0) ;
		}
	}

	//v4.57A8
k.Format("Substrate Align Result: angle found = %.6f degree (%.6f radian)", dTheta, dThetaInRadian);
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(k, "a+");
//HmiMessage(k);

	if (fabs(dTheta) > 3.0)	//0.5 )		//Allow 3 degree rotation in CC & CCW direction
	{
		CString szAngleLarge;
		szAngleLarge.Format( "Frame angle is too large - %.6f degree (max. 3 degree)", dTheta);
		HmiMessage_Red_Back( szAngleLarge );
		return FALSE;
	}

	SaveReferenceCross(m_dBinTwoCrossXDistance, m_dBinTwoCrossYDistance, m_lEncRefCross_X, m_lEncRefCross_Y);

	//k.Format( "FINISH %f %f %f %f" , dTheta ,m_dThetaOffsetByCross,dSlopeM1,dSlopeM2);
	//OutputDebugString(k);
	CMSLogFileUtility::Instance()->BL_LogStatus("BT: END align global T(BY FRAME CROSS)");
	
	if (m_bEnable_T && T_IsPowerOn() && m_bSel_T)
	{
		if (IsMS90())
		{
			T_MoveByDegree(-1 * dTheta);	//v4.59A7
		}
		else
		{
			T_MoveByDegree(dTheta);
		}
	}
	else 
	{
		m_oBinBlkMain.SetRotateAngleX(ulBlkID, m_dThetaOffsetByCross);
	}

	return TRUE;
}


BOOL CBinTable::AlignGlobalThetaAngle(ULONG ulBlkID)
{
	if (!m_bEnable_T)
	{
		return FALSE;
	}

	if (!m_bSel_T)
	{
		return TRUE;
	}

	if (!T_IsPowerOn())
	{
		return FALSE;
	}

    ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkID);
	if (ulNoOfSortedDie == 0)
		return TRUE;

	SetJoystickOn(FALSE);

    ULONG ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkID);
	if ( m_oBinBlkMain.GrabLSBondPattern() )
	{
		//** e.g. LS Bond sequence with TLH path, DiePerRow = 10 **//
		//
		//     1           
		// 11 10  9  8  7  6  5  4  3  2
		//    12 13 14 15 16 17 18 19 20
		//    29 28 27 26 25 24 23 22 21
		//    31 32 33 .......
		//
		ulFirstDieIndex = 2;
	}
	else if (m_bEnableBinMapBondArea)
	{
	}

	CString szError;
	szError.Format("BT: AlignGlobalThetaAngle (DieCount = %lu) ..... ", ulNoOfSortedDie);	//v4.59A21
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szError, "a+"); //v4.57A10

	LONG DieALogical_x = 0, DieALogical_y = 0;
	GetDieLogicalEncoderValue(ulBlkID, ulFirstDieIndex, DieALogical_x, DieALogical_y, TRUE);
 
	szError.Format("BT: AFTER (%ld, %ld)", DieALogical_x, DieALogical_y);
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szError, "a+");	//v4.57A10

	//v4.57A5
    if (!BT_MoveTo(DieALogical_x, DieALogical_y))
	{
		CString szError;
		szError.Format("BT MOVE is out of table limit (%ld, %ld)", DieALogical_x, DieALogical_y); 
		//szError = "BT MOVE is out of table limit";
		SetErrorMessage("BT_MoveTo Error in AlignGlobalThetaAngle - " + szError);
		HmiMessage_Red_Back(szError, "Bin Table Error");
		return FALSE;
	}

	Sleep(m_lAlignBinDelay);					

//AfxMessageBox("AlignGlobalThetaAngle -> SearchFirstGoodDie TRUE ....", MB_SYSTEMMODAL);
	CMSLogFileUtility::Instance()->BL_LogStatus("BT: Start align global T ...");

	//v4.59A21
	BOOL bUseWafflePad	= m_oBinBlkMain.GrabIsEnableWafflePad(ulBlkID);
	BOOL bDoGlobalT = TRUE;
	if ((ulNoOfSortedDie == 1) && !bUseWafflePad)
		bDoGlobalT = FALSE;

	LONG lDieFound = SearchFirstGoodDie(ulBlkID, bDoGlobalT);

	if (lDieFound == FALSE)
		return FALSE;
	return TRUE;
}

//shiraishi01
BOOL CBinTable::AlignFrameOrientation(ULONG ulBlkID)	//shiraishi01
{
	if (!m_bCheckFrameOrientation)
	{
		return TRUE;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_RENESAS)
	{
		return TRUE;
	}

	ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkID);

	CString szErr;
	CString szLog;
	szLog.Format("AlignFrameOrientation (Bin #%lu), COUNT = %lu", ulBlkID, ulNoOfSortedDie);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (ulNoOfSortedDie > 0)
	{
//CMSLogFileUtility::Instance()->MS_LogOperation("AlignFrameOrientation 3");
		return TRUE;
	}

	SetJoystickOn(FALSE);
		
	GetEncoderValue();
	if (labs(m_lEnc_T) > 500)
	{
		MoveToCOR("BT: Move to COR5");

		T_MoveTo(0);
		Sleep(500);
	}

	if (!IsWithinTable1Limit(m_lFrameOrientCheckX, m_lFrameOrientCheckY))
	{
		CString szErr;
		szErr = "ERROR (AlignFrameOrientation): BT Check-Orient position is out of table limit!";
		SetErrorMessage(szErr);
		HmiMessage_Red_Back(szErr, "Bin Frame Realignment");
		return FALSE;
	}
	if ((m_lFrameOrientCheckX == 0) && (m_lFrameOrientCheckY == 0))
	{
		CString szErr;
		szErr = "ERROR (AlignFrameOrientation): BT Check-Orient position is not set up properly!";
		SetErrorMessage(szErr);
		HmiMessage_Red_Back(szErr, "Bin Frame Realignment");
		return FALSE;
	}

	XY_MoveTo(m_lFrameOrientCheckX, m_lFrameOrientCheckY);
	Sleep(1000);

//HmiMessage("BT: align frame orientation ....");

	typedef struct 
	{
		int		siStepX;
		int		siStepY;
		BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;
	stInfo.bResult = TRUE;

	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDie2", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	if (!stInfo.bResult)
	{
		szErr = "ERROR (AlignFrameOrientation): BT Check-Orientataion 1 fails!";
		SetErrorMessage(szErr);

		Sleep(500);
		//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDieInFOV", stMsg);
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDie2", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
				break;
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if (!stInfo.bResult)
	{
		szErr = "Bin frame orientation check fails!  Please check current frame orientation.";
		SetErrorMessage(szErr);
		//HmiMessage_Red_Back(szErr, "Bin Frame Realignment");
		SetAlert_Red_Yellow(IDS_BT_FRAME_ORIENT_CHK_FAIL);
		return FALSE;
	}

	return TRUE;
}

LONG CBinTable::ExArmRealignEmptyFrameWithPRCmd(IPC_CServiceMessage& svMsg)
{
    ULONG ulPhysicalBlock = 0;
    svMsg.GetMsg(sizeof(ULONG), &ulPhysicalBlock);

	LONG lRet = SubExArmRealignEmptyFrameWithPRCmd(ulPhysicalBlock);
	BOOL bReturn = (lRet == TRUE) ? TRUE : FALSE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CBinTable::SubExArmRealignEmptyFrameWithPRCmd(const ULONG ulPhysicalBlock)
{
	BOOL bReturn = FALSE;

	for(INT i=1; i<=(INT)m_oBinBlkMain.GetNoOfBlk(); i++)
    {
        if( m_oBinBlkMain.GrabPhyBlkId(i) == ulPhysicalBlock )
        {
			bReturn = AlignEmptyFrame(ulPhysicalBlock);
		}
	}

	if (bReturn == TRUE)
	{
		SetBTReady(TRUE);
		(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= TRUE;	
	}

	return bReturn;
}

// huga
LONG CBinTable::AlignEmptyFrameWithPRCmd(IPC_CServiceMessage& svMsg)
{
	ULONG ulPhysicalBlock;

	typedef struct  
	{
		ULONG	ulBlkID;
		ULONG	ulBTInUse;
	} BL_ALIGNEMPTY;
	BL_ALIGNEMPTY stInfo;

    //svMsg.GetMsg(sizeof(ULONG), &ulPhysicalBlock);
    svMsg.GetMsg(sizeof(stInfo), &stInfo);		//v4.21T1

	ulPhysicalBlock = stInfo.ulBlkID;
	ULONG ulBTInUse = stInfo.ulBTInUse;

	LONG lRet = 0;
	for(INT i=1; i<=(INT)m_oBinBlkMain.GetNoOfBlk(); i++)
    {
		if( m_oBinBlkMain.GrabPhyBlkId(i) == ulPhysicalBlock )
        {
//			if ( m_bUseDualTablesOption && (ulBTInUse == 1) )		//BT2
//				lRet = AlignEmptyFrame2(ulPhysicalBlock);
//			else
				lRet = AlignEmptyFrame(ulPhysicalBlock);
		}
	}

	BOOL bReturn = (lRet == TRUE) ? TRUE : FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CBinTable::TeachColletOffset(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	LearnColletOffset();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

    return 1;
}


LONG CBinTable::ConfirmColletOffset(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	//SaveBinTableData();
	SaveBinTableSetupData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

    return 1;
}


LONG CBinTable::TeachBinTable2OffsetXY(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	//LONG lBT1X=0, lBT1Y=0;
	CString szLog;


	if (!m_bUseDualTablesOption)	
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BOOL bStep1 = TRUE;
	//svMsg.GetMsg(sizeof(BOOL), &bStep1);

	SetJoystickOn(FALSE);
	
	CString szTitle		= "BT2";


	if (bStep1)		//v4.42T7
	{
		CString szContent	= "Teach BT2 Offset XY; continue?";
		LONG lHmiMsgReply	= HmiMessage(szContent, szTitle, glHMI_MBX_YESNO); 
		if (lHmiMsgReply == glHMI_NO)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	
	
		szLog.Format("Teach BT2 Offset XY start - OLD (%ld, %ld)", m_lBT2OffsetX, m_lBT2OffsetY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		m_lBT1X = 0;
		m_lBT1Y = 0;

		HomeTable1();
		HomeTable2();

		Sleep(500);
		Y_Profile(LOW_PROF1);
		Y_MoveTo(0);
		Y_Profile(NORMAL_PROF);
		Sleep(100);

		//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001

		SetJoystickOn(TRUE);
		szContent = "Please move to BT1 reference position.";
		if (HmiMessageEx(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0) != glHMI_CONTINUE)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Teach BT2 Offset XY is aborted - 1");
			SetErrorMessage("Teach BT2 Offset XY is aborted - 1");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}


		// 2. Record BT1 posn
		SetJoystickOn(FALSE);
		Sleep(500);
		GetEncoderValue();
		m_lBT1X = m_lEnc_X;
		m_lBT1Y = m_lEnc_Y;
		HomeTable1();
	}
	
	bStep1 = FALSE;
	if (!bStep1)	//v4.42T16
	{
		LONG lBT2X=0, lBT2Y=0;

		// 3. Get BT2 posn
		Y2_Profile(LOW_PROF);
		X2_Profile(LOW_PROF);
		X2_MoveTo(0);
		Y2_MoveTo(m_lBT1Y);
		Sleep(200);
		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);
		Sleep(100);
		SetJoystickOn(TRUE, TRUE, TRUE);
	
		CString szContent = "Please move to BT2 reference position.";
		if (HmiMessageEx(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0) != glHMI_CONTINUE)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Teach BT2 Offset XY is aborted - 2");
			SetErrorMessage("Teach BT2 Offset XY is aborted - 2");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		// 4. Record BT2 posn
		SetJoystickOn(FALSE, TRUE, TRUE);
		Sleep(500);
		GetEncoderValue();
		lBT2X = m_lEnc_X2;
		lBT2Y = m_lEnc_Y2;


		// 5. Calculate BT2 XY Offset
		LONG lOffsetX = lBT2X - m_lBT1X;
		LONG lOffsetY = lBT2Y - m_lBT1Y;
		szContent.Format("BT2 Offset X = %ld, Y = %ld; Update?", lOffsetX, lOffsetY);
		LONG lHmiMsgReply	= HmiMessage(szContent, szTitle, glHMI_MBX_YESNO); 
		if (lHmiMsgReply == glHMI_NO)
		{
			//X2_Home();
			//Y12_Home();
			HomeTable2();

			SetJoystickOn(TRUE);
			CMSLogFileUtility::Instance()->MS_LogOperation("Teach BT2 Offset XY is aborted - 3");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		HomeTable2();

		m_lBT2OffsetX = lOffsetX;
		m_lBT2OffsetY = lOffsetY;
		m_lBT1X = 0;
		m_lBT1Y = 0;
		SaveBinTableData();

		szLog.Format("Teach BT2 Offset XY done - NEW (%ld, %ld)", m_lBT2OffsetX, m_lBT2OffsetY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CBinTable::LoadGradeRankFile(IPC_CServiceMessage& svMsg)
{
	INT nReturn;
	BOOL bReturn;
	CString szTitle, szContent;

	nReturn = LoadGradeRank();

	if (nReturn == TRUE)
	{
		m_bLoadRankIDFile = TRUE;
		SaveBinTableData();

		szTitle.LoadString(HMB_BT_NAME_FILE);
		szContent.LoadString(HMB_BT_NAME_FILE_LOADED);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);
	}
	//else if (nReturn == FALSE)
	//{
	//	SetAlert(IDS_BT_INVALID_FILE_FORMAT);
	//}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CBinTable::ResetGradeRankID(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szTitle, szContent;

	m_oBinBlkMain.ResetRankID();
	UpdateGradeRankID();

	m_bLoadRankIDFile = FALSE;
	SaveBinTableData();
	CMSLogFileUtility::Instance()->MS_LogOperation("RANKID file is reset manually");

	szTitle.LoadString(HMB_BT_NAME_FILE);
	szContent.LoadString(HMB_BT_NAME_FILE_RESET);

	HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

    return 1;
}


LONG CBinTable::SaveMachineNo(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	char *pBuffer;
	CString szMachineNo;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szMachineNo = &pBuffer[0];
	delete [] pBuffer;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::StoreBinTableData(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	//Catch any exception in SaveData	//v3.06
	TRY {
		SaveBinTableData();
	} CATCH (CFileException, e)
	{
		HmiMessage("Exception in SaveBinTableData !!!");
	}
	END_CATCH

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

/*
LONG CBinTable::ManualGenAllTempFiles(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	OpGenAllTempFile();
	BackupToNVRAM();

	HmiMessage("All temp files are manually generated");
	SetStatusMessage("Temp files are manually generated"); 

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}
*/

VOID CBinTable::GenerateWaferEndTime()
{
	int nYear, nMonth, nDay, nHour, nMinute, nSecond; 
	CTime theTime;

	//Get Wafer End Date and Time
	theTime = CTime::GetCurrentTime();
	
	nYear = theTime.GetYear();
	nMonth = theTime.GetMonth();
	nDay = theTime.GetDay();
	
	nHour = theTime.GetHour();
	nMinute = theTime.GetMinute();
	nSecond = theTime.GetSecond();
		

	//Write to SRam
	(*m_psmfSRam)["WaferTable"]["Wafer End Year"] = nYear;
	(*m_psmfSRam)["WaferTable"]["Wafer End Month"] = nMonth;
	(*m_psmfSRam)["WaferTable"]["Wafer End Day"] = nDay;
	(*m_psmfSRam)["WaferTable"]["Wafer End Hour"] = nHour;
	(*m_psmfSRam)["WaferTable"]["Wafer End Minute"] = nMinute;
	(*m_psmfSRam)["WaferTable"]["Wafer End Second"] = nSecond;
}



LONG CBinTable::ManualWaferEnd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn;
	BOOL bWaferEnd = TRUE;
	BOOL bGenFileOk = FALSE;

	////Save Stop map time
	//CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	//if ( pAppMod != NULL )
	//{
	//	pAppMod->MapIsStopped();
	//}

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( (pUtl->GetPrescanDummyMap() || IsEnablePNP()) && (IsProber() == FALSE) )	// wafer end
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Get Wafer End Date and Time
	GenerateWaferEndTime();

	//Generate wafer end file
	if (m_bIfGenWaferEndFile && !IsBurnIn())
	{
		if ( (bGenFileOk = WaferEndFileGenerating("Manual Generate Wafer End")) == TRUE )
		{
			CString szTitle, szContent;

			szTitle.LoadString(HMB_BT_GEN_WAFEREND_FILE);
			if (IsProber() == FALSE || m_lMapPrescanAlignWafer == SCAN_ALIGN_WAFER_DISABLE) //m_bMapDummyPrescanAlign == FALSE  )
			{
				szContent.LoadString(HMB_GENERAL_FILE_GEN);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
			}

			UpdateWaferLotLoadStatus();
		}

		WaferEndYieldCheck();
		IM_WaferEndChecking();
	}

	//Send wafer end info to Host
	if ((IsBurnIn() == FALSE) && (IsProber() == FALSE))
	{
		ScanSummaryGenerating();
		SendWaferEndInfoToHost();
	}

	if ((CMS896AStn::m_bForceClearMapAfterWaferEnd == TRUE) && (bGenFileOk == TRUE))
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Manual wafer end, Force clear map\n");
		SetErrorMessage("Manual wafer end, reset grade mapping");
		LogWaferEndInformation();
		m_WaferMapWrapper.InitMap();
		m_WaferMapWrapper.ResetGradeMap();
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetCustomerName() == "Testar") && (bGenFileOk == TRUE))	//v4.48A20
	{
		m_WaferMapWrapper.InitMap();
		(*m_psmfSRam)["WaferTable"]["WaferEnd"] = TRUE;
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::GetLabelFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szLabelFilePath);
		bReturn = TRUE;
		SaveBinTableData();
	}

	pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CBinTable::SelectLabelToPrint(IPC_CServiceMessage& svMsg)
{

	BOOL bScanBinFrameBarCode = FALSE;
	BOOL bReply = TRUE;


	CString szSelection1 = "All";
	CString szSelection2 = "Single";

	CString szMode1 = "Scan Barcode";
	CString szMode2 = "Reprint label";

	int nSelection = 0;
	int nSelectMode = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if(pApp->GetCustomerName()== "BSE")
	{
		nSelectMode = HmiMessage("Please choose which mode to print", "PRINT", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szMode1, &szMode2);

		if( nSelectMode == 1)
		{
			InputBinBarcodeLabelToPrint();

			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}

	//4.53D5 reprinter setting
	if( pApp->GetCustomerName()	=="KeWei" || pApp->GetCustomerName()== "HuaMao" ||pApp->GetCustomerName()== "HongYi" || pApp->GetCustomerName()== "BSE" ||
		pApp->GetCustomerName() == "ZhongWei")
	{
		nSelection = HmiMessage("Please choose which mode to print", "PRINT", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);
	}
	else
	{
		nSelection = 2;
	}

	if ( nSelection == 1)
	{
		CFileFind cfReprint;
		CString szLabelFile = m_szLabelFilePath + "\\*.lbex";
		BOOL bFind = cfReprint.FindFile(szLabelFile);
		/*AfxMessageBox(szLabelFile);*/
		while (bFind)
		{
			bFind = cfReprint.FindNextFile();
			CString szLabelFileName	=cfReprint.GetFileName();
			CString szLabelFilePath =cfReprint.GetFilePath();
			CString szNewLabelPath  =m_szLabelFilePath + "\\PrintLabel\\" + szLabelFileName;
CString szLog;
szLog.Format("Label file path:%s",szLabelFilePath);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
//AfxMessageBox(szLabelFilePath,MB_SYSTEMMODAL);
			CString szRetCode = "OK";
			if (ReprintLabelWithXMLLayout(szLabelFilePath, szRetCode) == TRUE)
			{
				CreateDirectory(m_szLabelFilePath + "\\PrintLabel",NULL);
				CopyFile(szLabelFilePath,szNewLabelPath,false);
				remove(szLabelFilePath);
			}
		}

	}
	else
	{
		CString szFormatFilename;
		CString szInitPath, szContents;
		CString szTitle, szContent;
		BOOL bFileOK = FALSE;

		// Get the Main Windows
		CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
		CWnd *pMainWnd;

		static char szFilters[]=
			"Lbex File (*.lbex)|*.lbex|All Files (*.*)|*.*||";

		if ( pAppMod != NULL )
			pMainWnd = pAppMod->m_pMainWnd;
		else
			pMainWnd = NULL;

		// Create an Open dialog; the default file name extension is ".txt".
		CFileDialog dlgFile(TRUE, "lbex", "*.lbex", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , 
							szFilters, pMainWnd, 0);


		szInitPath = m_szLabelFilePath;
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


		if ( nReturn == IDOK )
		{
			szFormatFilename = dlgFile.GetFileName();
			CString szRetCode = "OK";
			if (ReprintLabelWithXMLLayout(szFormatFilename, szRetCode) == TRUE)
			{
				HmiMessage("Reprint Label OK");
			}
			else
			{
				HmiMessage("Reprint Label Failed");
			}
			
		}
	}

	
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
	//BOOL bReturn = FALSE;

	//// Get the Main Windows
	//CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	//CWnd *pMainWnd;

	//static char szFilters[] = "Label File (*.lbe)|*.lbe||";

	//if ( pAppMod != NULL )
	//	pMainWnd = pAppMod->m_pMainWnd;
	//else
	//	pMainWnd = NULL;

	//INT nReturn;

	//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	//CString szHMITitle = _T("Please Input Filename");
	//BOOL bReply = HmiStrInputKeyboard(szHMITitle, m_szXMLFile);

	//if ( !bReply )
	//{
	//	return bReply;
	//}

	//CString szXMLFileName = m_szLabelFilePath + "\\" + m_szXMLFile + ".lbex";


	//if ( _access(szXMLFileName, 0) != -1 )
	//{

	//	CString szRetCode = "OK";
	//	HmiMessage(szRetCode + ", now print barcode in " + szXMLFileName, "reprint");
	//	bReturn = ReprintLabelWithXMLLayout(szXMLFileName, szRetCode);

	//	if ( bReturn == FALSE )
	//	{
	//		CString szTitle, szTemp;

	//		szTitle.LoadString(HMB_BT_REPRINT_LABEL);
	//		szTemp.LoadString(HMB_BT_REPRINT_FAILED);

	//		HmiMessage(szTemp ,szTitle);
	//	}

	//	HmiMessage("Print Success");

	//}
	//else 
	//{
	//	CString szTitle, szTemp;

	//	szTemp.Format( "Load file barcode '%s' failed.", m_szXMLFile ); 
	//	szTitle.LoadString(HMB_BT_REPRINT_LABEL);
	//	HmiMessage(szTemp ,szTitle);

	//	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	//	return FALSE;
	//}

	//svMsg.InitMessage(sizeof(BOOL), &bReturn);

	//return TRUE;
}

//4.53D24 offline search bin frame barcode to print out the label (No open yet in Hmi)
LONG CBinTable::BinBarcodeSelectLabelToPrint(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;

	if(InputBinBarcodeLabelToPrint() == FALSE)
	{
		bReply = FALSE;
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}
LONG CBinTable::GetBinBlkBondedCount(IPC_CServiceMessage& svMsg)
{
	ULONG ulBinBlkId = 0;
	ULONG ulBondedCount = 0;

	svMsg.GetMsg(sizeof(ULONG), &ulBinBlkId);

	if ( m_oBinBlkMain.GrabIsSetup(ulBinBlkId) == TRUE )
	{
		ulBondedCount = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBinBlkId);
	}

	svMsg.InitMessage(sizeof(ULONG), &ulBondedCount);
	return 1;
}


LONG CBinTable::OutputLotSummary(IPC_CServiceMessage& svMsg)
{
	CStringMapFile pSmfFile;
	CStdioFile cfLotFile;
	CString szFileName, szBpFileName;
	CString szText;
	CString szRankID;
	CString szLotNo;
	CString szWoNo;
	CString szGradeString;
	
	ULONG i,j;
	ULONG ulTotalWaferNo = 0;
	ULONG ulTotalPick = 0;
	ULONG ulTotalBond = 0;
	ULONG ulTotalBadCut = 0;
	ULONG ulTotalDefect = 0;
	ULONG ulTotalEmpty = 0;
	ULONG ulTotalRotate = 0;
	ULONG ulTotalChip = 0;
	ULONG ulTotalInk = 0;
	ULONG ulTotalMissing = 0;
	ULONG ulTempData = 0;
	ULONG ulDieCountOnFrame = 0;	//v2.56
	unsigned short usOrgGrade = 0;
	DOUBLE dYield = 0;
	DOUBLE dAvgTime = 0;	

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
	CString szPLine = pApp->GetProductLine();

	BOOL bReturn = TRUE;
	BOOL bFileOpen = FALSE;
	BOOL bExArm		= (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["ExChgArmExist"];		//v2.78T2
	BOOL bDBuffer	= (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["DualBufferExist"] ;	//v3.71T5


	//Open LastState msd to get Average cycle time
	dAvgTime = (*m_psmfSRam)[BH_MACHINE_STATISITC][BH_AVERAGE_CYCLE_TIME];

	//return if WaferRecord file is not exist
	if ((_access(MSD_WAFER_RECORD_FILE, 0 )) == -1)
	{
		CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferRecord msd");
		SetErrorMessage("Output Summary: Cannot Open File: WaferRecord msd");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//Read WO# & Lot#
	szWoNo	= (*m_psmfSRam)["MS896A"]["Bin WorkNo"];
	szLotNo	= (*m_psmfSRam)["MS896A"]["LotNumber"];

	//v2.83T44		//v3.71T4
	CString szBpOutputFilePath = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin";
	CString szTraceabilityPath = "c:\\output\\" + szWoNo;

	//DieFab LW format		//v4.46T20
	if ( (szPLine == "DieFab") && (CMS896AStn::m_lOTraceabilityFormat == 1) )		
	{
		szTraceabilityPath = m_szOutputFileSummaryPath + "\\" + szWoNo;
	}

	//v3.24T5
	//Modify traceability output path for Rev. 4
	if (CMS896AStn::m_lOTraceabilityFormat > 0)
	{
//SetErrorMessage("Output Summary: Use traceability lot path ...");

		//Create zWoNo directory is this is not exist
		CreateDirectory(szTraceabilityPath, NULL);
		CreateDirectory(szBpOutputFilePath, NULL);

		//Dest location = User Path + "\" + WONo(As directory) + "\" + WONo + LOTNO + "LotSummary.txt"
		if (pApp->GetProductLine() == "DieFab")	
		{
			szFileName		= m_szOutputFileSummaryPath + "\\" + szWoNo + "\\" + szWoNo + ".csv";		
			szBpFileName	= szBpOutputFilePath + "\\" /*+ szWoNo + "\\"*/ + szWoNo + ".csv";
		}
		else
		{
			szFileName		= m_szOutputFilePath + "\\" + szWoNo + "\\" + szWoNo + ".txt";		//v2.83T41		//v3.71T4
			szBpFileName	= szBpOutputFilePath + "\\" + szWoNo + "\\" + szWoNo + ".txt";			//v2.83T44
		}
		//if (nPLLM == PLLM_LUMIRAMIC)
		if ( (szPLine == "Lumiramic") && (CMS896AStn::m_lOTraceabilityFormat == 1) )		//Lumiramic LW format	//v4.35T1	//v4.35T1 MS109
			UploadPLLMFlatFileToServer(m_szOutputFileSummaryPath,	szBpOutputFilePath);	//v4.01		//Modified for Traceability project in PLLM Lumiramic
		else if ( (szPLine == "DieFab") && (CMS896AStn::m_lOTraceabilityFormat == 1) )		//DieFab LW format		//v4.46T20
			UploadPLLMFlatFileToServer(m_szOutputFileSummaryPath,	szBpOutputFilePath);
		else
			UploadPLLMFlatFileToServer(szTraceabilityPath,			szBpOutputFilePath);	//v3.94		//PLLM Flat file request for Lumiramic
	}
	else
	{
//SetErrorMessage("Output Summary: Use REBEL lot path ...");
		CString szClrBinOutPath = m_szOutputFilePath;
		if( m_bAutoUploadOutputFile )
			szClrBinOutPath = gszOUTPUT_FILE_TEMP_PATH;

		//Create zWoNo directory is this is not exist
		CreateDirectory(szClrBinOutPath + "\\" + szWoNo, NULL);
		CreateDirectory(szBpOutputFilePath + "\\" + szWoNo, NULL);

		//Dest location = User Path + "\" + WONo(As directory) + "\" + WONo + LOTNO + "LotSummary.txt"
		//szFileName = szClrBinOutPath + "\\" + szWoNo + "\\" + szWoNo + "-" + szLotNo + "-LotSummary.txt";
		if (pApp->GetProductLine() == "DieFab")	
		{
			szFileName		= m_szOutputFileSummaryPath + "\\" + szWoNo + "\\" + szWoNo + ".csv";		
			szBpFileName	= szBpOutputFilePath + "\\" /*+ szWoNo + "\\"*/ + szWoNo + ".csv";
		}
		else
		{
			szFileName		= szClrBinOutPath + "\\" + szWoNo + "\\" + szWoNo + ".txt";		//v2.83T41		//v3.71T4
			szBpFileName	= szBpOutputFilePath + "\\" + szWoNo + "\\" + szWoNo + ".txt";			//v2.83T44
		}

		//UploadPLLMFlatFileToServer(szClrBinOutPath,		szBpOutputFilePath);	//v3.95T1	//PLLM Flat file request for Lumiramic
		UploadPLLMFlatFileToServer(m_szOutputFileSummaryPath,	szBpOutputFilePath);	//v3.96		//Changed to use OutputSummary Path
	}


	//Open WaferRecord File to get existing information
	if (pSmfFile.Open(MSD_WAFER_RECORD_FILE, FALSE, TRUE) == 1)
	{
		ulTotalWaferNo	= (pSmfFile)["Total Wafer"];
	
		remove(szFileName);
		bFileOpen = cfLotFile.Open(szBpFileName, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText);

		if ( bFileOpen == FALSE )
		{
			SetErrorMessage("Output Summary: Cannot Open LOT file: " + szBpFileName);

			pSmfFile.Close();
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		cfLotFile.SeekToBegin();

		//Write WO No
		cfLotFile.WriteString("WO No: " + szWoNo + "\n");

		//Write Lot No
		cfLotFile.WriteString("Lot No: " + szLotNo + "\n");

		//Write Machine No
		szText = GetMachineNo();
		cfLotFile.WriteString("Machine No: " + szText + "\n");

		//Write Operator ID
		szText = (*m_psmfSRam)["MS896A"]["Operator Id"];
		cfLotFile.WriteString("Operator ID: " + szText + "\n");

		//Write Start Lot Time
		szText = (*m_psmfSRam)["MS896A"]["LotStartTime"];
		szText.Remove('-');
		szText.Remove(':');
		szText.Replace(';', ' ');
		cfLotFile.WriteString("Start: " + szText + "\n");

		//Write End Lot Time
		szText = (*m_psmfSRam)["MS896A"]["LotEndTime"];
		szText.Remove('-');
		szText.Remove(':');
		szText.Replace(';', ' ');
		cfLotFile.WriteString("End: " + szText + "\n\n");

		//Write map file loaded
		cfLotFile.WriteString("Map files loaded\n");
		for (i=1; i<=ulTotalWaferNo; i++)
		{
			szText = (pSmfFile)[i]["WaferID"];
			BOOL bUse2DBarcode = (BOOL)(LONG)(pSmfFile)[i]["2DBarcode"];

			szText.Replace(".txt","");

			if (bUse2DBarcode)
				cfLotFile.WriteString(szText + " (2D)\n");		//v3.70T4	//PLLM REBEL only
			else
				cfLotFile.WriteString(szText + "\n");

			ulTotalBadCut	+= (ULONG)(pSmfFile)[i]["BadCut"];
			ulTotalDefect	+= (ULONG)(pSmfFile)[i]["Defect"];
			ulTotalEmpty	+= (ULONG)(pSmfFile)[i]["Empty"];
			ulTotalRotate	+= (ULONG)(pSmfFile)[i]["Rotate"];
			ulTotalChip		+= (ULONG)(pSmfFile)[i]["Chip"];
			ulTotalInk		+= (ULONG)(pSmfFile)[i]["Ink"];
			ulTotalMissing	+= (ULONG)(pSmfFile)[i]["MissingDie"];
			ulTotalPick		+= (ULONG)(pSmfFile)[i]["Bond"];
		}
		ulTotalPick = ulTotalPick + ulTotalMissing;
		pSmfFile.Close();


		cfLotFile.WriteString("\n");

		//Write Total Map In:
		szText.Format("%d", ulTotalWaferNo);
		cfLotFile.WriteString("Total Map In: " + szText + "\n");

		//v2.71
		//Calculate total PICK count from bin instead of wafermap!!
		ULONG ulValue = 0, ulTotal = 0;
		for (i=1; i<=100; i++)
		{
			ulValue = m_oBinBlkMain.GrabBinAccBondedCount(i) + m_oBinBlkMain.GrabNVNoOfBondedDie(i);
			ulTotal += ulValue;
		}

		//Write Total Pick:
		//szText.Format("%d", ulTotalPick);		//v2.71
			CString szDieCount = "c:\\Mapsorter\\Exe\\DieCount.csv";
			CString szSummaryFile1 = "c://Mapsorter//Exe//Summary1.csv";
			CString szSummaryFile2 = "c://Mapsorter//Exe//Summary2.csv";
		if (pApp->GetProductLine() == "DieFab")	
		{
			CStdioFile cfDieCount;

			CString szTempDieCount;
			BOOL bRead;
			LONG lDieCount;
			LONG lTotalDieCount = 0;
			if (_access(szDieCount,0)!=-1)
			{
				if(cfDieCount.Open(szDieCount,CFile::modeRead)==TRUE)
				{
					do
					{
						bRead = cfDieCount.ReadString(szTempDieCount);
						lDieCount = atoi((LPCTSTR)szTempDieCount);
						lTotalDieCount = lTotalDieCount + lDieCount;
					}while(bRead);
					cfDieCount.Close();
				}
			}
			szText.Format("%d   (MD = %d)", lTotalDieCount/*ulTotal + ulTotalMissing*/, ulTotalMissing);		//v2.74
			cfLotFile.WriteString("Total Pick: " + szText + "\n\n");

		}
		else
		{
			szText.Format("%d   (MD = %d)", ulTotal + ulTotalMissing, ulTotalMissing);		//v2.74
			cfLotFile.WriteString("Total Pick: " + szText + "\n\n");
		}

		

		//Write Sort Summary
		cfLotFile.WriteString("Sort Summary\n");
		if (pApp->GetProductLine() == "DieFab")	
		{
			for (i=1; i<=50; i++)
			{
				for (j=0; j<2; j++)
				{
					szRankID = " ";
					ulTempData = 0;
					if ( m_oBinBlkMain.GrabIsSetup(i+(50*j)) == TRUE )
					{
						szRankID = m_oBinBlkMain.GrabRankID(m_oBinBlkMain.GrabGrade(i+(50*j)));

						usOrgGrade =  m_WaferMapWrapper.GetOriginalGrade(m_oBinBlkMain.GrabGrade(i+(50*j)) 
									+ m_WaferMapWrapper.GetGradeOffset())
									- m_WaferMapWrapper.GetGradeOffset();

						szGradeString = m_WaferMapWrapper.GetOriginalGradeString(m_oBinBlkMain.GrabGrade(i+(50*j)) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset());

						ulTempData =  m_oBinBlkMain.GrabBinAccBondedCount(i+(50*j))
									+ m_oBinBlkMain.GrabNVNoOfBondedDie(i+(50*j));

						ulDieCountOnFrame = m_oBinBlkMain.GrabNVNoOfBondedDie(i+(50*j));		//v2.56

						m_oBinBlkMain.SetBinAccBondedCount(i+(50*j), 0);
					}
					ulTotalBond += ulTempData;
				}
			}
			///////////////

			CString szWaferIDLoaded,szMapQty,szPrescanQty,szPickQty;
			//int nBadCut=0,nDefect=0,nEmpty=0,nRotate=0,nChip=0,nInk=0,nMissingDie=0,nBond=0;
			//ULONG ulWaferQty=0;
			cfLotFile.WriteString("Wafer IDs Loaded,Map Oty,Prescan Qty,Pick Qty,Bond Qty,Bin,BinTapeID,BinTapeDieQty");
			cfLotFile.WriteString("\n");


			CStdioFile csRead1;
			BOOL bRead;
			CString szTransfer;

			if (_access(szSummaryFile2,0)!=-1)
			{
				if(csRead1.Open(szSummaryFile2,CFile::modeRead)==TRUE)
				{
					do
					{
						bRead = csRead1.ReadString(szTransfer);
						cfLotFile.WriteString(szTransfer+ "\n");
						//cfLotFile.WriteString("\n");
					}while(bRead);
					csRead1.Close();
				}
			}

			if (_access(szSummaryFile1,0)!=-1)
			{
				if(csRead1.Open(szSummaryFile1,CFile::modeRead)==TRUE)
				{
					do
					{
						bRead = csRead1.ReadString(szTransfer);
						cfLotFile.WriteString(szTransfer + "\n");
						//cfLotFile.WriteString("\n");
					}while(bRead);
					csRead1.Close();
				}
			}



		}
		else
		{
			cfLotFile.WriteString("Block\tOrig Grade\tGrade Name\tQty\tPartial Qty\tBlock\tOrig Grade\tGrade Name\tQty\tPartial Qty\n");

			//Write BinBlock (Block No, RankID , Bonded Qty)
			for (i=1; i<=50; i++)
			{
				for (j=0; j<2; j++)
				{
					szRankID = " ";
					ulTempData = 0;
					if ( m_oBinBlkMain.GrabIsSetup(i+(50*j)) == TRUE )
					{
						szRankID = m_oBinBlkMain.GrabRankID(m_oBinBlkMain.GrabGrade(i+(50*j)));

						usOrgGrade =  m_WaferMapWrapper.GetOriginalGrade(m_oBinBlkMain.GrabGrade(i+(50*j)) 
									+ m_WaferMapWrapper.GetGradeOffset())
									- m_WaferMapWrapper.GetGradeOffset();

						//szGradeString = m_WaferMapWrapper.GetReader()->GetOriginalGradeString(m_oBinBlkMain.GrabGrade(i+(50*j)) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset());	//v4.31T2
						szGradeString = m_WaferMapWrapper.GetOriginalGradeString(m_oBinBlkMain.GrabGrade(i+(50*j)) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset());

						ulTempData =  m_oBinBlkMain.GrabBinAccBondedCount(i+(50*j))
									+ m_oBinBlkMain.GrabNVNoOfBondedDie(i+(50*j));

						ulDieCountOnFrame = m_oBinBlkMain.GrabNVNoOfBondedDie(i+(50*j));		//v2.56

						//Reset Accmulated bonded count
						m_oBinBlkMain.SetBinAccBondedCount(i+(50*j), 0);
					}

					if ( szRankID.GetLength() < 10 )
					{
						szRankID += "\t";
					}

					//v2.78T1
					CString szCatCode = "";
					if (bExArm || bDBuffer)		//v3.71T5
					{
						szCatCode = m_oBinBlkMain.GetBinBlkCatcode(i+(50*j));
						if (szCatCode.GetLength() < 10)
							szCatCode += "\t";

						if (szGradeString.GetLength() > 0)		//v4.31T2	//PLSG new map format
							szText.Format("%d\t%s\t\t%s\t%d\t%d", i+(50*j), szGradeString, szCatCode, ulTempData, ulDieCountOnFrame);
						else
							szText.Format("%d\t%d\t\t%s\t%d\t%d", i+(50*j), usOrgGrade, szCatCode, ulTempData, ulDieCountOnFrame);
					}
					else
					{
						if (szGradeString.GetLength() > 0)		//v4.31T2	//PLSG new map format
							szText.Format("%d\t%s\t\t%s\t%d\t%d", i+(50*j), szGradeString, szRankID, ulTempData, ulDieCountOnFrame);
						else
							szText.Format("%d\t%d\t\t%s\t%d\t%d", i+(50*j), usOrgGrade, szRankID, ulTempData, ulDieCountOnFrame);
					}
					
					cfLotFile.WriteString(szText + "\t\t");
					ulTotalBond += ulTempData;
				}
				cfLotFile.WriteString("\n");
			}
		}
		cfLotFile.WriteString("\n");

		
		//remove DieFab File.
		remove(szDieCount);
		remove(szSummaryFile1);
		remove(szSummaryFile2);
		remove(MSD_WAFER_RECORD_FILE);
		//Write PR Reject
		cfLotFile.WriteString("PR Reject\n");
		
		//Write Bad Cut Dice:
		szText.Format("%d", ulTotalBadCut);
		cfLotFile.WriteString("Bad Cut Dice: " + szText + "\n");

		//Write Defect Dice:
		szText.Format("%d", ulTotalDefect);
		cfLotFile.WriteString("Defect Dice: " + szText + "\n");

		//Write Empty Dice:
		szText.Format("%d", ulTotalEmpty);
		cfLotFile.WriteString("Empty Dice: " + szText + "\n");

		//Write Rotate Dice:
		szText.Format("%d", ulTotalRotate);
		cfLotFile.WriteString("Rotate Dice: " + szText + "\n");

		//Write Chip Dice:
		szText.Format("%d", ulTotalChip);
		cfLotFile.WriteString("Chip Dice: " + szText + "\n");

		//Write Ink Dice:
		szText.Format("%d", ulTotalInk);
		cfLotFile.WriteString("Ink Dice: " + szText + "\n\n");

		//Write Total bonded
		szText.Format("%d", ulTotalBond);
		cfLotFile.WriteString("Total bonded: " + szText + "\n\n");

		//Write Yield
		if ( (ulTotalBond == 0) || (ulTotal == 0) )			//(ulTotalPick == 0) )			//v2.78T2
		{
			dYield = 0.0;
		}
		else
		{
			//dYield = ((DOUBLE)ulTotalBond / (DOUBLE)ulTotalPick) * 100;
			dYield = ((DOUBLE) ulTotalBond / (DOUBLE) (ulTotal + ulTotalMissing)) * 100;		//v2.78T2
		}
		szText.Format("%3.2f", dYield);
		cfLotFile.WriteString("%Yield: " + szText + "\n\n");

		//Write Average cycle time
		szText.Format("%d", (LONG)dAvgTime);
		cfLotFile.WriteString("Average Cycle Time: " + szText + "\n\n");

		cfLotFile.Close();

		//if (bExArm)
		m_oBinBlkMain.ResetBinBlkCatcode();	//v2.78T2

		//v2.83T41
		if (!CopyFile(szBpFileName, szFileName, FALSE))
		{
			SetErrorMessage("Output Summary: Cannot copy to network: " + szFileName);
		}

		SetErrorMessage("Output Summary: file copied successfully to : " + szFileName);
	}
	else
	{
		CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferRecord msd");
		SetErrorMessage("Output Summary: Cannot Open File: WaferRecord msd");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//Generate End Lot Summary File, for Avago
LONG CBinTable::OutputLotSummary_Avago(IPC_CServiceMessage& svMsg)
{
	CStringMapFile pSmfFile;
	CStdioFile cfLotFile;
	CString szFileName;
	CString szText;
	CString szLotNo;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	ULONG i,j;
	ULONG ulTotalWaferNo	= 0;
	ULONG ulTotalDie		= 0;
	ULONG ulTotalPick		= 0;
	ULONG ulTotalBond		= 0;
	ULONG ulTotalBadCut		= 0;
	ULONG ulTotalDefect		= 0;
	ULONG ulTotalEmpty		= 0;
	ULONG ulTotalRotate		= 0;
	ULONG ulTotalChip		= 0;
	ULONG ulTotalInk		= 0;
	ULONG ulTotalMissing	= 0;

	ULONG ulTotalPRRejected = 0; 
	ULONG ulPickUpFailed	= 0; 
	ULONG ulProcessed		= 0;

	DOUBLE dProcessRate		= 0.0;
	DOUBLE dTotalPassYield	= 0.0;
	DOUBLE dPRRejectYield	= 0.0;
	DOUBLE dPickUpYield		= 0.0;
	DOUBLE dGradeYield		= 0.0;

	ULONG ulGradeTotalDie	= 0;
	ULONG ulDieCountOnFrame = 0;

	unsigned short usOrgGrade;

	DOUBLE dAvgTime = 0;	

	BOOL bReturn	= TRUE;
	BOOL bFileOpen	= FALSE;

	//Get Lot No
	szLotNo	= (*m_psmfSRam)["MS896A"]["LotNumber"];

	//Output Summary Path and Name
	CTime theTime = CTime::GetCurrentTime();
	INT nYear, nMonth, nDay, nHour, nMinute;
	CString szYear, szMonth, szDay, szHour, szMinute, szEndLotTime;

	nYear	= theTime.GetYear();
	nMonth	= theTime.GetMonth();
	nDay	= theTime.GetDay();
	nHour	= theTime.GetHour();
	nMinute	= theTime.GetMinute();
	
	szYear.Format("%d", nYear);

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

	CString szLotFolder = m_szOutputFilePath + "\\" + szLotNo;
	if (_access(szLotFolder, 0) == -1)
	{
		CreateDirectory(szLotFolder, NULL);
	}

	szEndLotTime = szYear + szMonth + szDay + "_" + szHour + szMinute;
	szFileName = m_szOutputFilePath + "\\" + szLotNo + "\\" + "Summary_" + szLotNo + "_" + szEndLotTime + ".spc";	
	CMSLogFileUtility::Instance()->MS_LogOperation("Create Output Summary (Avago) - " + szFileName);		//v4.48A21

	//v4.53A9
	if (pUtl->LoadLastState("BT: OutputLotSummary_Avago"))
	{
		psmf = pUtl->GetLastStateFile("BT: OutputLotSummary_Avago");
		if (psmf != NULL)
		{
			dAvgTime = (*psmf)[BH_MACHINE_STATISITC][BH_AVERAGE_CYCLE_TIME];
		}

		pUtl->CloseLastState("BT: OutputLotSummary_Avago");
	}

	if ((_access(MSD_WAFER_RECORD_FILE, 0 )) == -1)
	{
		SetErrorMessage("Cannot Open File: WaferRecord msd");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//Open WaferRecord File to get existing information
	if (pSmfFile.Open(MSD_WAFER_RECORD_FILE, FALSE, TRUE) == 1)
	{
		ulTotalWaferNo	= (pSmfFile)["Total Wafer"];
	
		remove(szFileName);
		
		for (i=1; i<=ulTotalWaferNo; i++)
		{
			ulTotalDie		+= (ULONG)(pSmfFile)[i]["TotalDie"];
			ulTotalBadCut	+= (ULONG)(pSmfFile)[i]["BadCut"];
			ulTotalDefect	+= (ULONG)(pSmfFile)[i]["Defect"];
			ulTotalEmpty	+= (ULONG)(pSmfFile)[i]["Empty"];
			ulTotalRotate	+= (ULONG)(pSmfFile)[i]["Rotate"];
			ulTotalChip		+= (ULONG)(pSmfFile)[i]["Chip"];
			ulTotalInk		+= (ULONG)(pSmfFile)[i]["Ink"];
			ulTotalPick		+= (ULONG)(pSmfFile)[i]["Bond"];
			ulTotalMissing	+= (ULONG)(pSmfFile)[i]["MissingDie"];
		}
		ulTotalPick = ulTotalPick + ulTotalMissing;

		ulTotalPRRejected	= ulTotalBadCut + ulTotalDefect + ulTotalEmpty + ulTotalRotate + ulTotalChip + ulTotalInk;
		ulProcessed			= ulTotalPRRejected + ulTotalPick + ulTotalMissing;

		dProcessRate	= 100.00 * ((double)ulProcessed / (double)ulTotalDie);
		dTotalPassYield = 100.00 * ((double)ulTotalPick / (double)ulTotalDie);
		if (ulProcessed == 0)
		{
			dPRRejectYield = 0.00;
			dPickUpYield = 0.00;
		}
		else
		{
			dPRRejectYield	= 100.00 * ((double)ulTotalPRRejected / (double)ulProcessed);
			dPickUpYield	= 100.00 * ((double)(ulProcessed - ulTotalMissing) / (double)ulProcessed);
		}

		//Write information to file
		bFileOpen = cfLotFile.Open(szFileName, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText);

		if ( bFileOpen == FALSE )
		{
			pSmfFile.Close();
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		cfLotFile.SeekToBegin();

		cfLotFile.WriteString("\nFormat\n");
		cfLotFile.WriteString("----------------------------------------------------------------\n");


		//Write Lot No
		cfLotFile.WriteString("LotNo:," + szLotNo + "\n");

		//Write Wafer ID
		cfLotFile.WriteString("WaferID:");
		for (i=1; i<=ulTotalWaferNo; i++)
		{

			szText = (pSmfFile)[i]["WaferID"];
			cfLotFile.WriteString("," + szText);
		}
		cfLotFile.WriteString("\n");

		//Write Mapping File Name
		cfLotFile.WriteString("Mapping File Name:");
		for (i=1; i<=ulTotalWaferNo; i++)
		{
			szText = (pSmfFile)[i]["MapName"];
			cfLotFile.WriteString("," + szText);
		}
		cfLotFile.WriteString("\n");

		//Write Start Lot Time
		szText = (*m_psmfSRam)["MS896A"]["LotStartTime"];
		cfLotFile.WriteString("TimeSortStart:," + szText + "\n");

		//Write End Lot Time
		szText = (*m_psmfSRam)["MS896A"]["LotEndTime"];
		cfLotFile.WriteString("TimeSortFinish:," + szText + "\n");
		
		//Write Operator ID
		szText = (*m_psmfSRam)["MS896A"]["Operator Id"];
		cfLotFile.WriteString("Operator ID:," + szText + "\n");
		
		//Write Machine ID
		szText = (*m_psmfSRam)["MS896A"]["MachineNo"];
		cfLotFile.WriteString("Machine ID:," + szText + "\n\n");


		//Ejector Pin life count when the lot completed
		szText = (*m_psmfSRam)["BinTable"]["EjectorUsageCount"];
		cfLotFile.WriteString("CntEjector:," + szText + "\n");

		//P&P collect life count when the lot completed
		szText = (*m_psmfSRam)["BinTable"]["ColletUsageCount"];
		cfLotFile.WriteString("CntCollet:," + szText + "\n");

		//Average Cycle Time
		szText.Format("%.2f", dAvgTime);
		cfLotFile.WriteString("Average Cycle Time:," + szText + "\n\n");	

		
		//Count of total good chips in mapping file
		szText.Format("%d", ulTotalDie);
		cfLotFile.WriteString("CntChipInput:," + szText + "\n");

		//Chips Processed
		szText.Format("%d", ulProcessed);
		cfLotFile.WriteString("CntChipProc:," + szText + ",,");
		
		//Process Rate
		szText.Format("%.2f", dProcessRate);
		cfLotFile.WriteString("Process Rate:," + szText + "%\n");

		//Chip Binned/Bonded
		szText.Format("%d", ulTotalPick);
		cfLotFile.WriteString("CntChipOK:," + szText + ",,");
		
//		Total pass yield
		szText.Format("%.2f", dTotalPassYield);
		cfLotFile.WriteString("Total pass yield:," + szText + "%\n");

		//Chips PR Reject
		szText.Format("%d", ulTotalPRRejected);
		cfLotFile.WriteString("CntChipNg:," + szText + ",,");
		
		//PR Reject yield
		szText.Format("%.2f", dPRRejectYield);
		cfLotFile.WriteString("PR Reject yield:," + szText + "%\n");

		//Chips pickup failed
		szText.Format("%d", ulTotalMissing);
		cfLotFile.WriteString("CntChipNotPick:," + szText + ",,");
	
		//PickUp yield
		szText.Format("%.2f", dPickUpYield);
		cfLotFile.WriteString("PickUp yield:," + szText + "%\n\n");

		//*************************//
		//*** PR Reject Summary ***//
		//*************************//
		//Write Bad Cut Dice:
		szText.Format("%d", ulTotalBadCut);
		cfLotFile.WriteString("Bad Cut Dice:," + szText + "\n");

		//Write Defect Dice:
		szText.Format("%d", ulTotalDefect);
		cfLotFile.WriteString("Defect Dice:," + szText + "\n");

		//Write Empty Dice:
		szText.Format("%d", ulTotalEmpty);
		cfLotFile.WriteString("Empty Dice:," + szText + "\n");

		//Write Rotate Dice:
		szText.Format("%d", ulTotalRotate);
		cfLotFile.WriteString("Rotate Dice:," + szText + "\n");

		//Write Chip Dice:
		szText.Format("%d", ulTotalChip);
		cfLotFile.WriteString("Chip Dice:," + szText + "\n");

		//Write Ink Dice:
		szText.Format("%d", ulTotalInk);
		cfLotFile.WriteString("Ink Dice:," + szText + "\n\n");


		//**************************//
		//*** Write Sort Summary ***//
		//**************************//
		cfLotFile.WriteString("Block,Rank/Grade,CntChipIn,CntChipOut,Yield (CntChipOut/CntChipIn)\n");
		CString szCategory = m_WaferMapWrapper.GetReader()->GetHeader().GetCategory();
		CString szOrgGrade, szOrgGradeString;
		//Write BinBlock (Block No, RankID , Bonded Qty)
		for (i=1; i<=100; i++)
		{
			ulGradeTotalDie = 0;
			ulDieCountOnFrame = 0;
			//ulLeft = 0;
			
			if ( m_oBinBlkMain.GrabIsSetup(i) == TRUE )
			{

				usOrgGrade =  m_WaferMapWrapper.GetOriginalGrade(m_oBinBlkMain.GrabGrade(i) 
								+ m_WaferMapWrapper.GetGradeOffset())
								- m_WaferMapWrapper.GetGradeOffset();

				szOrgGrade.Format("%d", usOrgGrade);

				szOrgGradeString = m_WaferMapWrapper.GetOriginalGradeString(m_oBinBlkMain.GrabGrade(i)  + m_WaferMapWrapper.GetGradeOffset());

				if (szOrgGradeString != "" && szCategory != "")
				{
					szOrgGrade = szOrgGradeString;
				}

				for (j=1; j<=ulTotalWaferNo; j++)
				{
					ulGradeTotalDie += (ULONG)(pSmfFile)[j][i]["TotalDieByGrade"];
					ulDieCountOnFrame += (ULONG)(pSmfFile)[j][i]["PickedDieByGrade"];
				}

			//	ulDieCountOnFrame = m_oBinBlkMain.GrabNVNoOfBondedDie(i);
			}

			if (ulDieCountOnFrame == 0)
			{
				dGradeYield = 0.00;
			}
			else
			{
				dGradeYield = 100.0 * ((double)ulDieCountOnFrame / (double)ulGradeTotalDie);
			}

			//Ranks of 0 count will be omitted
			if (ulDieCountOnFrame != 0)
			{
				szText.Format("%s, %d, %d, %d, %.2f", i, szOrgGrade, ulGradeTotalDie, ulDieCountOnFrame, dGradeYield);
				cfLotFile.WriteString(szText + "%\n");
			}
		}

		cfLotFile.WriteString("----------------------------------------------------------------\n");

		cfLotFile.Close();
		pSmfFile.Close();
	}
	else
	{
		SetErrorMessage("Cannot Open File: WaferRecord msd");
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("Create Output Summary (Avago) Done");		//v4.48A21
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::ShowLotSummary(IPC_CServiceMessage& svMsg)
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
	CString szWoNo, szLotNo, szFileName;

	CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	//Dest location = User Path + "\" + WONo(As directory) + "\" + WONo + LOTNO + "LotSummary.txt"
	szWoNo		= (*m_psmfSRam)["MS896A"]["Bin WorkNo"];
	szLotNo		= (*m_psmfSRam)["MS896A"]["LotNumber"];
	//szFileName	= m_szOutputFilePath + "\\" + szWoNo + "\\" + szWoNo + "-" + szLotNo + "-LotSummary.txt";
	szFileName	= m_szOutputFilePath + "\\" + szWoNo + "\\" + szWoNo + ".txt";		//v2.83T41		//v3.71T5

	dlg.szFileName	= szFileName;
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


LONG CBinTable::RestoreNVRunTimeData(IPC_CServiceMessage& svMsg)
{	
	BOOL bReturn = TRUE;

	CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("RESTORE NVRAM (warm-start) start");
	bReturn = m_oBinBlkMain.RestoreNVRunTimeData();
	//if (bReturn)		//v4.22T2
	BackupToNVRAM();
	if (bReturn)
		CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("RESTORE NVRAM (warm-start) done");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::RestoreNVRunTimeDataInHmi(IPC_CServiceMessage& svMsg)
{	
	BOOL bReturn = TRUE;

	//if (CMS896AStn::m_bWaferMapWarmStartFail == FALSE)
	if (CMS896AStn::m_bRestoreNVRamInBondPage != TRUE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("RESTORE NVRAM (HMI) pre-start");
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001

	LONG lSelection = HmiMessage_Red_Yellow("NVRAM data needs to be recovered due to abnormal program shutdown; Restore?", "NVRAM Restoration", glHMI_MBX_OKCANCEL);
	if (lSelection != glHMI_OK)
	{
		SetErrorMessage("NVRAM RESTORE : Restore-NVRAM fcn aborted by user.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("RESTORE NVRAM (HMI) start");


	//Use CURRENT MAP for RESTORE purpose
	CString szFileName = MS_LCL_CURRENT_MAP_FILE;
	if ( (szFileName.GetLength() == 0) || (szFileName == "") )
	{
		HmiMessage("NVRAM RESTORE: Invalid CURRENT MAP!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!CopyFile(szFileName, gszUSER_DIRECTORY + "\\RestoredMap.txt", FALSE))
	{
		HmiMessage("NVRAM RESTORE: Current map cannot be found!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//Load last map to memory for NVRAM data restore
	INT nCount=0;
	//	425TX 2 copy definition file to restore if have
	CString szDefFileName = gszUSER_DIRECTORY + "\\MapFile\\CurrentMap.def";
	CString szRestoreMapDef = gszUSER_DIRECTORY + "\\RestoredMap.def";
	BOOL bDefFileExist = FALSE;
	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szDefFileName, "r");
	if ((nErr == 0) && (fp != NULL))
	{
		bDefFileExist = TRUE;
		fclose(fp);
	}
	if( bDefFileExist )
	{
		if( !CopyFile(szDefFileName, szRestoreMapDef, FALSE) )
		{
			HmiMessage("NVRAM RESTORE: Current map def file cannot be found!");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}
	CString szLoadFile = gszUSER_DIRECTORY + "\\RestoredMap.txt";
	if( bDefFileExist )
		szLoadFile = szLoadFile + ";" + "RestoredMap.def";
	if (!LoadWaferMap(szLoadFile, ""))
	{
		SetErrorMessage("NVRAM RESTORE : Load RESTORE map failure!");
		HmiMessage("NVRAM RESTORE : Load RESTORE map failure!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	CMS896AApp::m_bMapLoadingFinish = FALSE;

	//v4.40T8	//Klocwork
	// waiting excute to complete
	nCount = 0;
	while(CMS896AApp::m_bMapLoadingFinish != TRUE)
	{
		Sleep(100);
		nCount++;
		if (nCount > 500) 
		{
			SetErrorMessage("NVRAM RESTORE : Load RESTORE map time-out!");
			HmiMessage("NVRAM RESTORE : Load RESTORE map time-out!");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	if (m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		SetErrorMessage("NVRAM RESTORE : map is not valid");
		HmiMessage("NVRAM RESTORE :map is not valid");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	nCount = 0;
	while (!m_WaferMapWrapper.IsMapValid())
	{
		Sleep(10);
		nCount++;
		if (nCount > 1000)
		{
			SetErrorMessage("NVRAM RESTORE : Load RESTORE map time-out!");
			HmiMessage("NVRAM RESTORE : Load RESTORE map time-out!");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	bReturn = m_oBinBlkMain.RestoreNVRunTimeData();
	BackupToNVRAM();
	if (bReturn)
	{
		CMS896AStn::m_bWaferMapWarmStartFail = FALSE;
		CMS896AStn::m_bRestoreNVRamInBondPage = FALSE;

		CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("RESTORE NVRAM (HMI) done\n");
		HmiMessage("Restore-NVRAM done.");
		m_WaferMapWrapper.InitMap();

		DeleteFile(gszUSER_DIRECTORY + "\\RestoredMap.txt");
		if( bDefFileExist )//	425TX 2 delete definition file if success
		{
			DeleteFile(szRestoreMapDef);
		}
	}
	else
	{
		//bReturn = FALSE;
		SetErrorMessage("RESTORE NVRAM (HMI) fails.\n");
		CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("RESTORE NVRAM (HMI) fails");
		HmiMessage("Restore-NVRAM fails.");
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::ClearNVRunTimeData(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	m_oBinBlkMain.ResetNVRunTimeData();
	// Backup to NVRam immediately to prevent mis-match cases occur
	BackupToNVRAM();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::SetupBinBlock(IPC_CServiceMessage& svMsg)
{	
	BOOL bReturn = TRUE;
	BOOL bSetup = FALSE;	
	LONG lMaxPos, lMinPos;

	svMsg.GetMsg(sizeof(BOOL), &bSetup);

	m_bIsSetupBinBlk = bSetup;
	m_bShowBinBlkTemplateSetup = FALSE;

	if (!m_fHardware)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);

	if ( m_bIsSetupBinBlk == TRUE )
	{
		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_MAX_DISTANCE);		
		if ( (lMinPos != 0) && (lMaxPos != 0) )
		{
			//m_pServo_X->SetJoystickPositionLimit(lMinPos, lMaxPos);
			CMS896AStn::MotionSetJoystickPositionLimit("BinTableXAxis", lMinPos, lMaxPos, &m_stBTAxis_X);
		}

		lMinPos = GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_MIN_DISTANCE);								
		lMaxPos = GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_MAX_DISTANCE);		
		if ( (lMinPos != 0) && (lMaxPos != 0) )
		{
			//m_pServo_Y->SetJoystickPositionLimit(lMinPos, lMaxPos);
			CMS896AStn::MotionSetJoystickPositionLimit("BinTableYAxis", lMinPos, lMaxPos, &m_stBTAxis_Y);
		}
	}
	else
	{
		if ( (m_lTableXNegLimit != 0) && (m_lTableXPosLimit != 0) )
		{
			//m_pServo_X->SetJoystickPositionLimit(m_lTableXNegLimit, m_lTableXPosLimit);
			CMS896AStn::MotionSetJoystickPositionLimit("BinTableXAxis", m_lTableXNegLimit, m_lTableXPosLimit, &m_stBTAxis_X);
		}

		if ( (m_lTableYNegLimit != 0) && (m_lTableYPosLimit != 0) )
		{
			//m_pServo_Y->SetJoystickPositionLimit(m_lTableYNegLimit, m_lTableYPosLimit);
			CMS896AStn::MotionSetJoystickPositionLimit("BinTableYAxis", m_lTableYNegLimit, m_lTableYPosLimit, &m_stBTAxis_Y);
		}
	}

	SetJoystickOn(TRUE, FALSE);	//v3.01T4

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::DisplayBondingBinBlk(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	UpdateBinDisplay(1, 1);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//---------------------------- 
//--- Diagnostic Functions ---
//---------------------------- 
/*** Power On ***/
LONG CBinTable::Diag_PowerOn_X(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (!bOn)	//pllm
	{
		SetJoystickOn(FALSE);
		Sleep(100);
	}

	//StartLoadingAlert();
	X_PowerOn(bOn);
	//CloseLoadingAlert();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_PowerOn_Y(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (!bOn)	//pllm
	{
		SetJoystickOn(FALSE);
		Sleep(100);
	}

	//StartLoadingAlert();
	Y_PowerOn(bOn);
	//CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_PowerOn_T(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (!m_bEnable_T)
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//if (!bOn)	//pllm
	//{
	//	SetJoystickOn(FALSE);
	//	Sleep(100);
	//}

	//StartLoadingAlert();
	T_PowerOn(bOn);
	//CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_PowerOn_X2(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (!bOn)	//pllm
	{
		SetJoystickOn(FALSE);
		Sleep(100);
	}

	//StartLoadingAlert();
	X2_PowerOn(bOn);
	//CloseLoadingAlert();

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_PowerOn_Y2(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (!bOn)	//pllm
	{
		SetJoystickOn(FALSE);
		Sleep(100);
	}

	CString szTemp;
	szTemp.Format("Diag_PowerOn_Y2  %d", bOn);
	//StartLoadingAlert();
	Y2_PowerOn(bOn);
	//CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_PowerOn_All(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (!bOn)	//pllm
	{
		SetJoystickOn(FALSE);
		Sleep(100);
	}

	//StartLoadingAlert();
	X_PowerOn(bOn);
	Y_PowerOn(bOn);

	if (m_bUseDualTablesOption)	//v4.16T3	//MS100 9Inch
	{
		X2_PowerOn(bOn);
		Y2_PowerOn(bOn);
	}
	//CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

/*** Commutate ***/
LONG CBinTable::Diag_Comm_X(IPC_CServiceMessage& svMsg)
{
	StartLoadingAlert();
	SetJoystickOn(FALSE);
	X_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Comm_Y(IPC_CServiceMessage& svMsg)
{
	StartLoadingAlert();
	SetJoystickOn(FALSE);
	Y_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Comm_X2(IPC_CServiceMessage& svMsg)
{
	StartLoadingAlert();
	SetJoystickOn(FALSE);
	X2_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Comm_Y2(IPC_CServiceMessage& svMsg)
{
	StartLoadingAlert();
	SetJoystickOn(FALSE);
	Y2_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


/*** Home ***/
LONG CBinTable::Diag_Home_X(IPC_CServiceMessage& svMsg)
{
#ifdef NU_MOTION
	//Disable limit sensor protection
	CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, 999999, -999999, &m_stBTAxis_X);	//v3.76
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, TRUE, 10, m_stDataLog);	//v4.03
	}
#endif

	StartLoadingAlert();
	SetJoystickOn(FALSE);

	X_Home();

	if (m_bUseDualTablesOption == TRUE)
	{
		if (X_IsPowerOn())
		{
			X_Profile(LOW_PROF1);
			if (m_lTableXNegLimit != 0)
			{
				X_MoveTo(m_lTableXNegLimit + BT_DUAL_TABLE_PARKING_OFFSET_X);
			}
			else
			{
				X_MoveTo(BT_DUAL_TABLE_PARKING_DEFAULT_POSX, FALSE);
			}
			X_Profile(NORMAL_PROF);
		}
	}

	CloseLoadingAlert();

#ifdef NU_MOTION
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, FALSE, 10, m_stDataLog);	//v4.03
	}
	//Enable limit sensor protection
	CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, m_lTableXPosLimit, m_lTableXNegLimit, &m_stBTAxis_X);	//v3.76
#endif

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Home_Y(IPC_CServiceMessage& svMsg)
{
#ifdef NU_MOTION
	//Disable limit sensor protection
	if (CMS896AApp::m_bIsPrototypeMachine)
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBTAxis_Y);	//prototype MS100
	else
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, 999999, -999999, &m_stBTAxis_Y);	//v3.76
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_Y, &m_stBTAxis_Y, TRUE, 10, m_stDataLog);	//v4.03
	}
#endif

	StartLoadingAlert();
	SetJoystickOn(FALSE);

	if (m_bUseDualTablesOption)
	{
		Y12_Home();
	}
	else
	{
		Y_Home();
	}

	CloseLoadingAlert();
	
#ifdef NU_MOTION
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_Y, &m_stBTAxis_Y, FALSE, 10, m_stDataLog);	//v4.03
	}
	//Enable limit sensor protection
	if (CMS896AApp::m_bIsPrototypeMachine)
		CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stBTAxis_Y);	//prototype MS100
	else
        CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, m_lTableYPosLimit, m_lTableYNegLimit-2000, &m_stBTAxis_Y);	//v3.76
#endif

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Home_T(IPC_CServiceMessage& svMsg)
{
	if (!m_bEnable_T)
	{
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	StartLoadingAlert();

	/*SetJoystickOn(FALSE);
	if (IsMotorXYReady())
	{
		int nRet = MoveToCOR("BT: Move to COR1 in Diag_Home_T");
		Sleep(100);
		SetJoystickOn(TRUE);
		if ( nRet == gnOK )
		{
			T_Home();
		}
		CloseLoadingAlert();
	}*/
	
	T_Home();
	CloseLoadingAlert();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Home_X2(IPC_CServiceMessage& svMsg)
{
	//Disable limit sensor protection
	CMS896AStn::MotionEnableProtection(BT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_X2);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X2, 999999, -999999, &m_stBTAxis_X2);
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_X2, &m_stBTAxis_X2, TRUE, 10, m_stDataLog);
	}

	StartLoadingAlert();
	SetJoystickOn(FALSE);
	//X12_Home();
	X2_Home();

	if (m_bUseDualTablesOption == TRUE)
	{
		if (X2_IsPowerOn())
		{
			X2_Profile(LOW_PROF1);
			if (m_lTableX2NegLimit != 0)
			{
				X2_MoveTo(m_lTableX2NegLimit + BT_DUAL_TABLE_PARKING_OFFSET_X);		//v4.24T8	//Avoid collision with DBH
			}
			else
			{
				X2_MoveTo(BT_DUAL_TABLE2_PARKING_DEFAULT_POSX, FALSE);					//v4.24T8	//DEFAULT parking pos to avoid collision with DBH
			}
			X2_Profile(NORMAL_PROF);
		}
	}

	CloseLoadingAlert();

	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_X2, &m_stBTAxis_X2, FALSE, 10, m_stDataLog);
	}
	//Enable limit sensor protection
	CMS896AStn::MotionEnableProtection(BT_AXIS_X2, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X2);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X2, m_lTableX2PosLimit, m_lTableX2NegLimit, &m_stBTAxis_X2);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Home_Y2(IPC_CServiceMessage& svMsg)
{
	//Disable limit sensor protection
	CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBTAxis_Y2);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, 999999, -999999, &m_stBTAxis_Y2);
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_Y2, &m_stBTAxis_Y2, TRUE, 10, m_stDataLog);
	}

	StartLoadingAlert();
	SetJoystickOn(FALSE);
	//Y2_Home();
	
	if (m_bUseDualTablesOption)
	{
		Y12_Home();
	}

	CloseLoadingAlert();
	
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_Y2, &m_stBTAxis_Y2, FALSE, 10, m_stDataLog);	
	}
	//Enable limit sensor protection
    CMS896AStn::MotionEnableProtection(BT_AXIS_Y2, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y2);
	CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, m_lTableY2PosLimit, m_lTableY2NegLimit, &m_stBTAxis_Y2);	//v3.76

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


/*** Move ***/
LONG CBinTable::Diag_Move_X(IPC_CServiceMessage& svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	SetJoystickOn(FALSE);

	CString szLog;
	szLog.Format("BT-X Diag MOVE = %ld ...", lPos);
	//HmiMessage(szLog);

	X_Move(lPos);
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Move_Y(IPC_CServiceMessage& svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	StartLoadingAlert();
	SetJoystickOn(FALSE);

	CString szLog;
	szLog.Format("BT-Y Diag MOVE = %ld ...", lPos);
	//HmiMessage(szLog);

	Y_Move(lPos);
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Move_T(IPC_CServiceMessage& svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	if (!m_bEnable_T)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	T_Move(lPos);
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Move_X2(IPC_CServiceMessage& svMsg)
{
	LONG lPos = 0;
	svMsg.GetMsg(sizeof(LONG), &lPos);
	/*
	StartLoadingAlert();
	SetJoystickOn(FALSE);
	
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_X2, &m_stBTAxis_X2, TRUE, 10, m_stDataLog);
	}

	X2_Move(lPos);

	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_X2, &m_stBTAxis_X2, FALSE, 10, m_stDataLog);
	}
	CloseLoadingAlert();
	*/
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::Diag_Move_Y2(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);
	/*
	StartLoadingAlert();
	SetJoystickOn(FALSE);
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_Y2, &m_stBTAxis_Y2, TRUE, 10, m_stDataLog);
	}

	Y2_Move(lPos);

	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance(BT_AXIS_Y2, &m_stBTAxis_Y2, FALSE, 10, m_stDataLog);
	}

	CloseLoadingAlert();
	*/
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::UpdateOutput(IPC_CServiceMessage& svMsg)
{
	BOOL bUpdate = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bUpdate);

	m_bUpdateOutput = bUpdate;

	bUpdate = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bUpdate);
	return 1;
}

LONG CBinTable::SetAlignBinMode(IPC_CServiceMessage& svMsg)
{
	BOOL bTemp = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bTemp);

	m_bAlignBinInAlive = bTemp;

	bTemp = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bTemp);
	return 1;
}


VOID CBinTable::SendCE_Setup(VOID)
{
    // 3386
    SetGemValue("BT_XPosLimit", m_lTableXPosLimit);
    SetGemValue("BT_XNegLimit", m_lTableXNegLimit);
    SetGemValue("BT_YPosLimit", m_lTableYPosLimit);
    SetGemValue("BT_YNegLimit", m_lTableYNegLimit);
    // 3387
    SetGemValue("BT_UnloadPosnX", m_lBTUnloadPos_X);
    SetGemValue("BT_UnloadPosnY", m_lBTUnloadPos_Y);
    // 7400
    SendEvent(SG_CEID_BT_SETUP, FALSE);
}


LONG CBinTable::GetBinDiePitch(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
	    LONG lPitchX;
	    LONG lPitchY;
	} BT_PITCH;
	BT_PITCH stInfo;

    stInfo.lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(1));
    stInfo.lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(1));

	svMsg.InitMessage(sizeof(BT_PITCH), &stInfo);
	return 1;

}


LONG CBinTable::GetNewMapBlockInUse(IPC_CServiceMessage& svMsg)
{
	LONG lBlockNo = -1;
	INT nCount = 0;


	//Return to waferloader's LoadFilmFrame if not using Ex-Arm config for DLA
	BOOL bExArm		= (BOOL)(LONG)((*m_psmfSRam)["BinLoaderStn"]["ExChgArmExist"]);
	BOOL bDBuffer	= (BOOL)(LONG)((*m_psmfSRam)["BinLoaderStn"]["DualBufferExist"]);	//pllm
	if (!bExArm && !bDBuffer) 
	{
		svMsg.InitMessage(sizeof(LONG), &lBlockNo);
		return TRUE;
	}

	//Wait until map file is completely loaded & grade list generated
	while (!CMS896AApp::m_bMapLoadingFinish)
	{
		if (nCount > 50) {
			svMsg.InitMessage(sizeof(LONG), &lBlockNo);
			return TRUE;
		}
		Sleep(100);
		nCount++;
	}

	CUIntArray aulSelectedGradeList;
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);

	if (aulSelectedGradeList.GetSize() > 0)
	{
		UCHAR ucGrade = aulSelectedGradeList.GetAt(0) - m_WaferMapWrapper.GetGradeOffset();
		lBlockNo = m_oBinBlkMain.GrabNVBlkInUse(ucGrade);
	}

	svMsg.InitMessage(sizeof(LONG), &lBlockNo);
	return TRUE;
}


LONG CBinTable::GetNextGradeBlk(IPC_CServiceMessage& svMsg)
{
	ULONG ulCurrBlk = 0;
	ULONG ulNextBlk = 0;
	ULONG ulBlk = 0;

	svMsg.GetMsg(sizeof(ULONG), &ulCurrBlk);
	
	//v4.10
	if (!m_WaferMapWrapper.IsMapValid())
	{
		ulNextBlk = 0;
		svMsg.InitMessage(sizeof(ULONG), &ulNextBlk);
		return 1;
	}
	if (m_WaferMapWrapper.GetReader() == NULL)
	{
		ulNextBlk = 0;
		svMsg.InitMessage(sizeof(ULONG), &ulNextBlk);
		return 1;
	}

	if (m_WaferMapWrapper.GetPickMode() == WAF_CDieSelectionAlgorithm::IN_GRADE_ORDER)		//v3.94
	{
		if ((ulBlk = OpGetNextNextGradeBlk(ulCurrBlk)) > 0)
			ulNextBlk = ulBlk;
	}
	else
	{
		if ((ulBlk = OpGetNextNextGradeBlkInMixOrder(ulCurrBlk)) > 0)
			ulNextBlk = ulBlk;
//CString szTemp;
//szTemp.Format("Andrew: next bin (MIX-ORDER) = %d", ulNextBlk);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	}

	svMsg.InitMessage(sizeof(ULONG), &ulNextBlk);
	return 1;
}


/*Comment for generate burn in map file, please do not remove! Thanks
VOID CBinTable::GenerateBurnInMap()
{
	FILE *fp;
	if ( (fp = fopen("BurnInDL(100Bin).txt", "w")) != NULL )
	{
		int nRow=100, nCol=2, nGrade=100, nNextCol=0;
		int i,j,k;
		
		fprintf(fp, "DataFileName,,BurnInDL(100Bin).txt\n");
		fprintf(fp, "LotNumber,,\n");
		fprintf(fp, "DeviceNumber,,\n");
		fprintf(fp, "TestTime,,1/9/2025 14:39\n");
		fprintf(fp, "MapFileName,,BurnInDL(100Bin).txt\n");
		fprintf(fp, "TransferTime,,1/9/2026 9:33\n");
		fprintf(fp, "\nmap data,,\n");
		fprintf(fp, "0,0\n");		

		for (k=1; k<=nGrade; k++)
		{
			for (j=0; j<nCol; j++)
			{
				for (i=0; i<nRow; i++)
				{
					fprintf(fp,"%d,%d,%d\n", i,nNextCol,k);
				}
				nNextCol++;
			}
		}

		fclose(fp);
	}
}
*/

//--- For Print Label Utility ---//
LONG CBinTable::ExecutPrintLabelUtility(IPC_CServiceMessage& svMsg)
{

	STARTUPINFO			startupInfo;
	PROCESS_INFORMATION	processInfo;

	/*
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
		if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	*/
	memset(&startupInfo, 0, sizeof(STARTUPINFO)); //set memory to 0
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = startupInfo.dwFlags|STARTF_USESHOWWINDOW;		//v2.83T2

	Sleep(1000);
	CreateProcess(NULL, "c:\\MapSorter\\Exe\\MS899 Label Utility.exe", NULL, NULL, FALSE, 
		NORMAL_PRIORITY_CLASS, NULL, "c:\\MapSorter\\Exe",
				&startupInfo, &processInfo);

	CloseHandle( processInfo.hThread );		//Klocwork	//v4.27

	/*
	RECT	stWinRect;

	CWnd *pHMI = CWnd::FindWindow(NULL, _T("ASM Human Machine Interface"));	// Get the HMI handle
	if ( pHMI != NULL )
	{
		pHMI->GetWindowRect(&stWinRect);
		pHMI->SetWindowPos(&CWnd::wndTopMost, 
								(stWinRect.right + stWinRect.left) / 2, 
								(stWinRect.bottom + stWinRect.top) / 2,
								0, 0,
								SWP_HIDEWINDOW);
	
	}
*/
	/*
	CWnd *pLabelUtility = NULL;
	int i=0;
	do 
	{
		Sleep(500);
		pLabelUtility = CWnd::FindWindow(NULL, _T("MS899 Label Utility V1.01"));	// Get the HMI handle
		i++;
	} while((pLabelUtility==NULL) && (i<10));

	if ( pLabelUtility != NULL )
	{
		pLabelUtility->SetForegroundWindow();
		pLabelUtility->BringWindowToTop();
		//pLabelUtility->GetWindowRect(&stWinRect);
		//	pLabelUtility->SetWindowPos(&CWnd::wndBottom, stWinRect.left, stWinRect.top, 
						////(stWinRect.right - stWinRect.left), (stWinRect.bottom - stWinRect.top),
						//SWP_SHOWWINDOW);

	}
	else
	{
		AfxMessageBox("Cannot find handle");
	}
	*/

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::SaveBinTableDataCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	SaveBinTableData();
	SaveBinTableSetupData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::UpdateOneBinGrade(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	/*
	if( pApp->GetCustomerName()!="SanAn" )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	else 
	*/
	{
		//v4.46T10	//ZhangJiaGang Semitek
		BOOL bStatus = TRUE;
		BOOL bCheckAll = TRUE;
		if (pApp->GetCustomerName() == "Semitek")
			bCheckAll = FALSE;

		bStatus = SaveBinGradeWithIndex(bCheckAll);
		if (!bStatus) 
		{
			BOOL bReturn = FALSE;
			HmiMessage("Change Bin Grade Failed.");
		}
		else 
		{
			szMsg.Format("%d Bin Block is set to Grade %d!", m_ulBinBlkToSetup , m_ucGrade );
			HmiMessage(szMsg);
		}
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
}

LONG CBinTable::UpdateBlkByPassBinMap(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	BOOL bStatus = SaveByPassBinMap(bEnable);
	if (!bStatus) 
	{
		BOOL bReturn = FALSE;
		HmiMessage("Change ByPass BinMap Failed.");
	}
	else 
	{
		szMsg.Format("Bin #%d ByPass BinMap is set to %d!", m_ulBinBlkToSetup, bEnable);
		HmiMessage(szMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::ClearAllBin(IPC_CServiceMessage& svMsg)
{
	CString szContent, szTitle;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	BOOL bReturn = TRUE, bReply = TRUE;
	BOOL bBinLoader = IsBLEnable();
	LONG lHmiMsgReply = 0;

	if (bBinLoader == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	szContent.LoadString(HMB_BT_CLR_ALL_BIN_CONFIRM);
	szTitle.LoadString(HMB_BT_CLR_ALL_BIN);

	lHmiMsgReply = HmiMessage(szContent, szTitle, glHMI_MBX_YESNO); 

	if (lHmiMsgReply == glHMI_NO)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	// Check whether frame is on the bin table and unload it back to magazine
	
	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "UnloadFilmFrame", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReply);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	
	// Clear All Bin
	if (ClrAllPhyBinCtr(m_szBinOutputFileFormat) == FALSE)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	ResetMapDieType();
	
	// Output Summary File
	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "ManualOutputSummaryFile", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReply);
			break;
		}
		else
			Sleep(1);
	}

	// Reset All Magazine
	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "StartLotResetAllMgznCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReply);
			break;
		}
		else
			Sleep(1);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::GetRankIDFilePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szRankIDFilePath);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::ResetRankIDFile(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	m_szRankIDFilename.Empty();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="SiLan" )
	{
		CMSFileUtility  *pUtl = CMSFileUtility::Instance();
		CStringMapFile  *psmf;
		if (pUtl->LoadLastState() != FALSE)
		{
			psmf = pUtl->GetLastStateFile();		

			if (psmf != NULL)
			{
				(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTBINFNAME] = "";
			}
			pUtl->UpdateLastState("BT: Sort Bin Name");
			pUtl->CloseLastState("BT: Sort Bin Name");
		}
	}

	SaveLastState();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::AutoLoadRankIDFile(IPC_CServiceMessage& svMsg)
{
	INT nCol;
	BOOL bReturn = TRUE;
	CString szFullPath = "";
	IPC_CServiceMessage stMsg;

	if (m_bEnableAutoLoadRankFile == FALSE)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szMapDate, szRankIDFilenameInMap, szFilename;
	CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_DATE_IN_MAP_FILE, szMapDate);
			
	nCol = szMapDate.Find(',');
	if (nCol != -1)
	{	
		szMapDate = szMapDate.Mid(nCol+1);
		nCol = szMapDate.Find(',');
		
		if (nCol != -1)
			szMapDate = szMapDate.Mid(nCol+1);
		
		szMapDate.Remove(',');

		szRankIDFilenameInMap = szMapDate;
	}
	
	if (szRankIDFilenameInMap.IsEmpty())
	{
		bReturn = FALSE;
		
		SetAlert_Red_Yellow(IDS_BT_FAIL_TO_LOAD_RANK_FILE);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_szRankIDFilename.IsEmpty() == FALSE)
	{
		if (m_szRankIDFilename != szRankIDFilenameInMap)
		{
			bReturn = FALSE;
			SetAlert_Red_Yellow(IDS_BT_FAIL_TO_COMPARE_BIN_FILE);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else
		{
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	szFilename = m_szRankIDFilePath + "\\" + szRankIDFilenameInMap + ".csv";
	
	if (ReadRankIDFile(szFilename) == FALSE)
	{
		bReturn = FALSE;
		SetAlert_Red_Yellow(IDS_BT_FAIL_TO_LOAD_RANK_FILE);
		m_oBinBlkMain.ResetRankID();
		UpdateGradeRankID();
		m_szRankIDFilename.Empty();
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	UpdateGradeRankID();
	SetStatusMessage("loaded rank id file");
	
	if (m_bAutoBinBlkSetupWithTempl == TRUE)
	{
	// Auto Bin Blk Setup Part
		for (INT i = 1; i <= (INT)m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
			{
				bReturn = FALSE;
				SetAlert_Red_Yellow(IDS_BT_DENYBLKSETTINGSEDIT1);
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}

		for (INT i = 1; i <= (INT)m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			CString szContent, szTitle;
			
			UCHAR ucGrade = m_oBinBlkMain.GrabGrade(i);
			 
			CString szTemplateName = m_oBinBlkMain.GrabBlockSize(ucGrade);

			if (SetupSingleBinBlkUsingTemplate(szTemplateName, m_oBinBlkMain.GetNoOfBlk(), i, i, ucGrade) == FALSE)
			{
				bReturn = FALSE;
				szTitle.LoadString(HMB_BT_LOAD_TEMPLATE);
				szContent.Format("Fail to apply template to bin block:%d", i);
				HmiMessage_Red_Yellow(szContent , szTitle , glHMI_MBX_OK, 
							glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				m_oBinBlkMain.ResetRankID();
				UpdateGradeRankID();
				m_szRankIDFilename.Empty();
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		
			if (UpdateGradeInputCountUsingTemplate(szTemplateName, ucGrade) == FALSE)
			{
				bReturn = FALSE;
				szTitle.LoadString(HMB_BT_LOAD_TEMPLATE);
				szContent.Format("Fail to apply template to bin block:%d", i);
				HmiMessage_Red_Yellow(szContent , szTitle , glHMI_MBX_OK, 
							glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				m_oBinBlkMain.ResetRankID();
				UpdateGradeRankID();
				m_szRankIDFilename.Empty();
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}
	}

	if (m_ulEnableOptimizeBinCount >= TRUE)
	{
		szFullPath = GetBinLostSummaryPath() + "\\" + szRankIDFilenameInMap + ".csv";

		if (ApplyOptimizeBinCount(szFullPath, m_bEnableBinSumWaferLotCheck, m_ulMinLotCount) == FALSE)
		{	
			bReturn = FALSE;
			m_oBinBlkMain.ResetRankID();
			UpdateGradeRankID();
			m_szRankIDFilename.Empty();
			SetStatusMessage("Fail to setup optimize bin count");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		
		SetStatusMessage("Setup optimize bin count complete");
	}

	// Update the rankID file if it is success
	m_szRankIDFilename = szRankIDFilenameInMap;
	SaveLastState();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::UploadBinBlkRunTimeData(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	DeleteFile("BinBlkRunTime.msd");

	if (m_oBinBlkMain.CreateBackupTempFileList() == FALSE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_oBinBlkMain.SaveBinBlkRunTimeData() == FALSE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_oBinBlkMain.SaveBinGradeRunTimeData() == FALSE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::DownloadBinBlkRunTimeData(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (m_oBinBlkMain.LoadBinBlkRunTimeData() == FALSE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_oBinBlkMain.LoadBinGradeRunTimeData() == FALSE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::GenBinBlkRunTimeSummary(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szMachineNo, szBinBlkRunTimeSummaryPath, szSummaryFilename;

	szMachineNo = GetMachineNo();
	szBinBlkRunTimeSummaryPath = (*m_psmfSRam)["MS896A"]["BinBlkSummaryPath"];

	szSummaryFilename = szBinBlkRunTimeSummaryPath + "\\" + "UNLOADED_ALL_BINS.txt";

	if (m_oBinBlkMain.GenerateBinRunTimeSummary(szSummaryFilename, szMachineNo) == FALSE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::RemoveAllTempFile(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	for (INT i=1 ;i<=(INT)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		m_oBinBlkMain.RemoveTempFileAndResetIndex(i, '0' , CLEAR_BIN_BY_PHY_BLK);		
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::SaveGenSummaryFileTime(IPC_CServiceMessage& svMsg)
{
	CString szTitle, szContent;
	BOOL bReturn = TRUE;

	szTitle.LoadString(HMB_BT_AUTO_GEN_SUM_SETUP);
	
	if (UpdateGenerateBinBlkSummaryTime() == FALSE)
	{
		szContent.LoadString(HMB_BT_AUTO_GEN_SUM_SETUP_FAIL);
		HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	szContent.LoadString(HMB_BT_AUTO_GEN_SUM_SETUP_COMPLETE);
	HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);

	SaveLastState();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::GetBinLotSummaryFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szBinLotSummaryPath);
		bReturn = TRUE;
		SaveBinTableData();
		SaveBinTableSetupData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}

LONG CBinTable::GetBinLotSummaryFilePath2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}

LONG CBinTable::LoadBinLotSummaryFile(IPC_CServiceMessage& svMsg)
{
	CString szContent = "", szTitle = "";
	BOOL bReturn = TRUE;
	CString szFilename = "";
	LONG lReturn = 0;
	INT nCol = 0;
	CString szFilePath = "";
	CString szFullPath = "";

	if (m_oBinBlkMain.IsBondingInProgress() == TRUE)
	{	
		SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	szTitle.LoadString(HMB_BT_LOAD_BIN_LOT_SUM_FILE);

	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]=
      "CSV File (*.csv)|*.csv|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".csv".
	CFileDialog dlgFile(TRUE, "csv", "*.csv", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , 
						szFilters, pMainWnd, 0);

	dlgFile.m_ofn.lpstrInitialDir = GetBinLostSummaryPath();
	dlgFile.m_ofn.lpstrDefExt = "csv";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	StartLoadingAlert();

	if ( nReturn == IDOK )
	{
		szFilePath = dlgFile.GetPathName();

		nCol = szFilePath.ReverseFind('\\');
		if (nCol != -1)
		{
			szFilePath = szFilePath.Left(nCol);
		}

		szFilename = dlgFile.GetFileName();

		nCol = szFilename.ReverseFind('.');
		if (nCol != -1)
		{
			szFilename = szFilename.Left(nCol);
		}

		SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

		szFullPath = szFilePath + "\\" + szFilename + ".csv";

		if (ApplyOptimizeBinCount(szFullPath, m_bEnableBinSumWaferLotCheck, m_ulMinLotCount) == FALSE)
		{
			SaveBinTableData();
			CloseLoadingAlert();

			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		CloseLoadingAlert();
		
		szContent.LoadString(HMB_BT_LOAD_BIN_LOT_SUM_FILE_COMPLETE);
		HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		SetStatusMessage("Load bin summary file complete");
	}
	else
	{
		CloseLoadingAlert();
	}

	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::LoadBinLotSummaryFile2(IPC_CServiceMessage& svMsg)
{
/*
	Chris: Return the code of 4.43 in electech3E version
*/
	BOOL bReturn = TRUE;	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetCustomerName() == "Electech3E") ||
		(pApp->GetCustomerName() == "Electech3E(DL)") )	
	{
		CString szContent = "", szTitle = "";

		CString szFilename = "";
		LONG lReturn = 0;
		INT nCol = 0;
		CString szFilePath = "";
		CString szFullPath = "";

		if (m_oBinBlkMain.IsBondingInProgress() == TRUE)
		{	
			SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		char *pBuffer;
		pBuffer = new char[svMsg.GetMsgLen()];
		svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);
		CString szFileName = &pBuffer[0];
		//szFileName = m_szInputCountSetupFilePath + "\\" + szFileName;
		delete [] pBuffer;
	//HmiMessage("LoadBinLotSummaryFile2: " + szFileName);
			
		//CString szFullPath = szFilePath + "\\" + szFilename + ".csv";
		//StartLoadingAlert();
		OpenWaitingAlert();

		if (ApplyOptimizeBinCount(szFileName, m_bEnableBinSumWaferLotCheck, m_ulMinLotCount) == FALSE)
		{
			SaveBinTableData();
			//CloseLoadingAlert();
			CloseWaitingAlert();
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		//CloseLoadingAlert();
		CloseWaitingAlert();

		szTitle.LoadString(HMB_BT_LOAD_BIN_LOT_SUM_FILE);
		szContent.LoadString(HMB_BT_LOAD_BIN_LOT_SUM_FILE_COMPLETE);
		HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		SetStatusMessage("Load bin summary file complete 2");
	}
	else
	{
		CString szContent = "", szTitle = "";
		//BOOL bReturn = TRUE;
		CString szFileName = "";
		LONG lReturn = 0;
		INT nCol = 0;
		CString szFilePath = "";
		CString szFullPath = "";

		if (m_oBinBlkMain.IsBondingInProgress() == TRUE)
		{	
			SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		//if (!pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_OPBC_LOADPATH))
		//{
		//	svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//	return 1;
		//}

		szFilePath = (*m_psmfSRam)["MS896A"]["MapFilePath"];

		WIN32_FIND_DATA FileData;
		HANDLE hSearch;
		BOOL	bFindOk = FALSE;
		hSearch = FindFirstFile(szFilePath + "\\" + "*", &FileData);
		if ( hSearch == INVALID_HANDLE_VALUE ) 
		{ 
			HmiMessage_Red_Yellow("Fail to open File Handle");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		do 
		{
			if (FileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				szFileName = FileData.cFileName;

				if (szFileName.Find(".csv") != -1)
				{
					bFindOk = TRUE;
					break;
				}
			}
		} while (FindNextFile(hSearch, &FileData));
	 
		// Close the search handle. 
		FindClose(hSearch);
		if (!bFindOk)
		{
			HmiMessage_Red_Yellow("No CSV file is found in: " + szFilePath);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		szFileName = szFilePath + "\\" + szFileName;
	HmiMessage("Loading Optimize-Bin File at: " + szFileName);
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return 1;

			
		//CString szFullPath = szFilePath + "\\" + szFilename + ".csv";
		//StartLoadingAlert();
		OpenWaitingAlert();

		if (ApplyOptimizeBinCount(szFileName, m_bEnableBinSumWaferLotCheck, m_ulMinLotCount) == FALSE)
		{
			SaveBinTableData();
			//CloseLoadingAlert();
			CloseWaitingAlert();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		//CloseLoadingAlert();
		CloseWaitingAlert();

		szTitle.LoadString(HMB_BT_LOAD_BIN_LOT_SUM_FILE);
		szContent.LoadString(HMB_BT_LOAD_BIN_LOT_SUM_FILE_COMPLETE);
		HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		SetStatusMessage("Auto Load bin summary file complete 3");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::AutoLoadBinLotSummaryFile(IPC_CServiceMessage& svMsg)
{
	CString szContent, szTitle;
	CString szBinLotFilename;
	BOOL bReturn = TRUE;
	LONG lReturn = 0;
	CString szFullPath = "";
	CString szLotFilename = "";

	char *pFilename;

	pFilename = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pFilename);

	szLotFilename = &pFilename[0];
	delete [] pFilename;

	//StartLoadingAlert();

	szFullPath =  GetBinLostSummaryPath() + "\\" + szLotFilename + ".csv";

	if (ApplyOptimizeBinCount(szFullPath, m_bEnableBinSumWaferLotCheck, m_ulMinLotCount) == FALSE)
	{
		SetErrorMessage("Fail to apply optimize bin count");
		szContent.LoadString(HMB_FAIL_TO_LOAD_BIN_SUMMARY_FILE);
		HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		//CloseLoadingAlert();
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	//CloseLoadingAlert();

	szContent.LoadString(HMB_BT_LOAD_BIN_LOT_SUM_FILE_COMPLETE);
	HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);

	SetStatusMessage("Load bin summary file complete");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::UpdateOptimizeBinGradeScreen(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	m_ucOpimizeBinSetupGrade = 1;
	m_ucOpimizeBinSetupGradeLimit = 1;

	m_ulMinFrameDieCount = m_oBinBlkMain.GetMinFrameDieCount(m_ucOpimizeBinSetupGrade);
	m_ulMaxFrameDieCount = m_oBinBlkMain.GetMaxFrameDieCount(m_ucOpimizeBinSetupGrade);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// Update the screen
LONG CBinTable::OnClickChangeOptimizeBinGrade(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	m_ulMinFrameDieCount = m_oBinBlkMain.GetMinFrameDieCount(m_ucOpimizeBinSetupGrade);
	m_ulMaxFrameDieCount = m_oBinBlkMain.GetMaxFrameDieCount(m_ucOpimizeBinSetupGrade);
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::OnClickUpdateOptimzeMinCountByRange(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	UCHAR ucSetupGrade = 1;
	CString szTitle = "", szContent = "";

	szTitle.LoadString(HMB_BT_OPT_DATA_UPDATE);

	for (INT i=m_ucOpimizeBinSetupGrade; i<=m_ucOpimizeBinSetupGradeLimit; i++)
	{
		if (m_oBinBlkMain.SetMinFrameDieCount(i, m_ulMinFrameDieCount) == FALSE)
		{
			szContent.LoadString(HMB_BT_EXCEED_BIN_BLK_CAPACITY);
			HmiMessage(szContent);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	szContent.LoadString(HMB_BT_OPT_DATA_UPDATE_COMPLETED);
	HmiMessage(szContent, szTitle);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::OnClickUpdateAllOptimizeMinCount(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	UCHAR ucSetupGrade = 1;
	CString szTitle = "", szContent = "";

	szTitle.LoadString(HMB_BT_OPT_DATA_UPDATE);

	if (m_oBinBlkMain.UpdateAllOptimizeMinCount(m_ulMinFrameDieCount) == FALSE)
	{
		szContent.LoadString(HMB_BT_EXCEED_BIN_BLK_CAPACITY);
		HmiMessage(szContent);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;

	}

	szContent.LoadString(HMB_BT_OPT_DATA_UPDATE_COMPLETED);
	HmiMessage(szContent, szTitle);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::OnClickUpdateOptimizeMaxCountByRange(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	UCHAR ucSetupGrade = 1;
	CString szTitle = "", szContent = "";
	szTitle.LoadString(HMB_BT_OPT_DATA_UPDATE);

	for (INT i=m_ucOpimizeBinSetupGrade; i<=m_ucOpimizeBinSetupGradeLimit; i++)
	{
		if (m_oBinBlkMain.SetMaxFrameDieCount(i, m_ulMaxFrameDieCount) == FALSE)
		{
			szContent.LoadString(HMB_BT_EXCEED_BIN_BLK_CAPACITY);
			HmiMessage(szContent);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	szContent.LoadString(HMB_BT_OPT_DATA_UPDATE_COMPLETED);
	HmiMessage(szContent, szTitle);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::OnClickUpdateAllOptimizeMaxCount(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	UCHAR ucSetupGrade = 1;
	CString szTitle = "", szContent = "";

	szTitle.LoadString(HMB_BT_OPT_DATA_UPDATE);

	if (m_oBinBlkMain.UpdateAllOptimizeMaxCount(m_ulMaxFrameDieCount) == FALSE)
	{
		szContent.LoadString(HMB_BT_EXCEED_BIN_BLK_CAPACITY);
		HmiMessage(szContent, szTitle);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;

	}

	szContent.LoadString(HMB_BT_OPT_DATA_UPDATE_COMPLETED);

	HmiMessage(szContent, szTitle);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

/*
LONG CBinTable::OnClickMinFrameDieCount(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	UCHAR ucSetupGrade = 1;
		
	if (m_ucOpimizeBinSetupGrade != 0)
	{
		m_oBinBlkMain.SetMinFrameDieCount(m_ucOpimizeBinSetupGrade, m_ulMinFrameDieCount);
	}
	else
	{
		CString szTitle = "", szContent = "";

		szTitle.LoadString(HMB_BT_OPT_DATA_UPDATE);
		szContent.LoadString(HMB_BT_OPT_DATA_UPDATE_COMPLETED);

		if (m_ucOpimizeBinSetupGrade == 0)
		{
			m_oBinBlkMain.UpdateAllOptimizeMinCount(m_ulMinFrameDieCount);
		}
		
		HmiMessage(szContent, szTitle);

	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::OnClickMaxFrameDieCount(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	UCHAR ucSetupGrade = 1;
	CString szTitle = "", szContent = "";

	szTitle.LoadString(HMB_BT_OPT_DATA_UPDATE);

	//m_oBinBlkMain.SetMaxFrameDieCount(m_ucOpimizeBinSetupGrade, m_ulMaxFrameDieCount);
	
	if (m_ucOpimizeBinSetupGrade != 0)
	{
		if (m_oBinBlkMain.SetMaxFrameDieCount(m_ucOpimizeBinSetupGrade, m_ulMaxFrameDieCount) == FALSE)
		{
			szContent.LoadString(HMB_BT_EXCEED_BIN_BLK_CAPACITY);
			szContent.Format("%s %d", szContent, m_ucOpimizeBinSetupGrade);
			HmiMessage(szContent, szTitle);
			OnClickChangeOptimizeBinGrade(svMsg);
		}
	}
	else
	{
		UCHAR ucGrade = 1;

		if (m_oBinBlkMain.UpdateAllOptimizeMaxCount(m_ulMaxFrameDieCount, ucGrade) == FALSE)
		{
			szContent.LoadString(HMB_BT_EXCEED_BIN_BLK_CAPACITY);
			szContent.Format("%s %d", szContent, ucGrade);
			HmiMessage(szContent, szTitle);
			OnClickChangeOptimizeBinGrade(svMsg);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		
		szTitle.LoadString(HMB_BT_OPT_DATA_UPDATE);
		szContent.LoadString(HMB_BT_OPT_DATA_UPDATE_COMPLETED);

		HmiMessage(szContent, szTitle);
	}
		
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
*/

LONG CBinTable::ResetOptimizeBinCountStatus(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szTitle = "", szContent = "";
	
	if (CMS896AStn::m_bEnableOptimizeBinCountFunc == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	szTitle.LoadString(HMB_BL_CLEAR_ALL_FRAME);
	szContent.LoadString(HMB_BT_RESET_BIN_SUMMARY_FILE);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO) == glHMI_NO)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	ResetOptimizeBinCountStatus();
	
	SaveBinTableData();
	
	szContent.LoadString(HMB_GENERAL_COMPLETED);
	HmiMessage(szContent, szTitle);
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

/*
LONG CBinTable::OnClickUpdateAllOptimizeMinCount(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	ULONG ulMinFrameDieCount = 0;
	CString szTitle = "", szContent = "";


	szTitle.LoadString(HMB_BT_OPT_DATA_UPDATE);
	szContent.LoadString(HMB_BT_OPT_DATA_UPDATE_COMPLETED);

	if (m_ucOpimizeBinSetupGrade == 0)
	{
		m_oBinBlkMain.UpdateAllOptimizeMinCount(m_ulMinFrameDieCount);
	}
	
	HmiMessage(szContent, szTitle);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::OnClickUpdateAllOptimizeMaxCount(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	CString szTitle = "", szContent = "";
	UCHAR ucGrade = 1;

	

	
	m_oBinBlkMain.UpdateAllOptimizeMaxCount(m_ulMaxFrameDieCount, ucGrade);
	
	if (m_ucOpimizeBinSetupGrade == 0)
	{
		if (m_oBinBlkMain.UpdateAllOptimizeMaxCount(m_ulMaxFrameDieCount, ucGrade) == FALSE)
		{
			szContent.LoadString(HMB_BT_EXCEED_BIN_BLK_CAPACITY);
			szContent.Format("%s %d", szContent, ucGrade);
			HmiMessage(szContent, szTitle);
			OnClickChangeOptimizeBinGrade(svMsg);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	
	/*
	if (m_oBinBlkMain.UpdateAllOptimizeMaxCount(m_ulMaxFrameDieCount, ucGrade) == FALSE)
	{
		//szContent.LoadString(HMB_BT_EXCEED_BIN_BLK_CAPACITY);
		//szContent.Format("%s %d", szContent, ucGrade);
		//HmiMessage(szContent, szTitle);
		//OnClickChangeOptimizeBinGrade(svMsg);
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return 1;
	}

	
		HmiMessage(szContent, szTitle);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
*/


LONG CBinTable::WaferEndFileGeneratingCheckingCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bLoadMap = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bLoadMap);

	BOOL bReturn = TRUE;

	//Get Wafer End Date and Time
	GenerateWaferEndTime();
	if (bLoadMap)
	{
		if ((CMS896AStn::m_bAutoGenWaferEndFile == TRUE) || m_bIfGenWaferEndFile)
		{
			bReturn = WaferEndFileGenerating("Load Map Generate Waferend");
		}
	}
	else
	{
		bReturn = WaferEndFileGeneratingChecking();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//=====================================================Teach BHZ1&BHZ2 Bond position================================================= 
VOID CBinTable::SaveBTAdjCollet1Offset_um(const DOUBLE dBHZ1AdjBondPosOffsetX_um, const DOUBLE dBHZ1AdjBondPosOffsetY_um)
{
	m_lBHZ1BondPosOffsetX += _round(dBHZ1AdjBondPosOffsetX_um / m_dXResolution_UM_CNT);
	m_lBHZ1BondPosOffsetY += _round(dBHZ1AdjBondPosOffsetY_um / m_dXResolution_UM_CNT);
	SaveBTCollet1Offset(m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY, m_dXResolution_UM_CNT);

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		pBondPr->SaveAdjBondPosOffsetPixel(FALSE, m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY);
	}
}


VOID CBinTable::SaveBTAdjCollet2Offset_um(const DOUBLE dBHZ2AdjBondPosOffsetX_um, const DOUBLE dBHZ2AdjBondPosOffsetY_um)
{
	m_lBHZ2BondPosOffsetX += _round(dBHZ2AdjBondPosOffsetX_um / m_dXResolution_UM_CNT);
	m_lBHZ2BondPosOffsetY += _round(dBHZ2AdjBondPosOffsetY_um / m_dXResolution_UM_CNT);
	SaveBTCollet2Offset(m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY, m_dXResolution_UM_CNT);

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		pBondPr->SaveAdjBondPosOffsetPixel(TRUE, m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY);
	}
}


BOOL CBinTable::SendRequestReply(CString szStnName, CString szFuncName, IPC_CServiceMessage &stMsg)
{
	// Get the reply
	int nConvID = m_comClient.SendRequest(szStnName,  szFuncName, stMsg);
	while (1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			return TRUE;
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return FALSE;
}

BOOL CBinTable::BT_SearchDieInMaxFOV(int &siStepX, int &siStepY)
{
	typedef struct 
	{
		int		siStepX;
		int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;
	
	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;

	if (SendRequestReply(BOND_PR_STN, "BT_SearchDieInMaxFOV", stMsg))
	{
		stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
	}

	siStepX = stInfo.siStepX;
	siStepY = stInfo.siStepY;
	return stInfo.bResult;
}

BOOL CBinTable::AutoLearnBHZBondPosOffset(LONG &lBHZBondPosOffsetX, LONG &lBHZBondPosOffsetY)
{
	BOOL bReturn = TRUE;
	BOOL bManualMode = TRUE;	
	CString szTitle, szContent;

	SetJoystickOn(FALSE);

	if (CheckBPRStatus() == 1)
	{
		int siStepX = 0, siStepY = 0;

		BOOL bResult = BT_SearchDieInMaxFOV(siStepX, siStepY);

		if (bResult == TRUE)
		{
			GetEncoderValue();
			X_MoveTo(m_lEnc_X + siStepX, SFM_WAIT);
			Y_MoveTo(m_lEnc_Y + siStepY, SFM_WAIT);

			lBHZBondPosOffsetX = -siStepX;
			lBHZBondPosOffsetY = -siStepY;
		}

		bManualMode = !bResult;
	}


	if (bManualMode == TRUE)
	{
		SetJoystickOn(TRUE);

		AfxMessageBox("Fail to Search Die!", MB_SYSTEMMODAL);

		return FALSE;
	}


CString szTemp;
szTemp.Format("Collet Offset(by Die) (x, y): (%d, %d)", lBHZBondPosOffsetX, lBHZBondPosOffsetY);
SetStatusMessage(szTemp);
AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	return bReturn;
}

BOOL CBinTable::ConfirmBHZBondPosOffsetByCollet(const CString szFuncName, LONG &lBHZBondPosOffsetX, LONG &lBHZBondPosOffsetY)
{
	BOOL bReturn = TRUE;
	CString szMsg;

	typedef struct 
	{
		int		siStepX;
		int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;

	if (SendRequestReply(BOND_PR_STN, szFuncName, stMsg))
	{
		stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
	}

	if (stInfo.bResult == TRUE)
	{
		lBHZBondPosOffsetX = -stInfo.siStepX;
		lBHZBondPosOffsetY = -stInfo.siStepY;
	}

	SaveBinTableData();

	szMsg.Format("Collet Offset(by Collet) (x, y): (%d, %d)", lBHZBondPosOffsetX, lBHZBondPosOffsetY);
	SetStatusMessage(szMsg);

	return stInfo.bResult;
}


//=====================================================Teach BHZ1 Bond position================================================= 
BOOL CBinTable::ResetBHZ1BondPosOffset(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;

	if (HmiMessage("Are you sure to reset BHZ1 Bond Pos Offset?", "BHZ1 Bond Pos Offset", glHMI_MBX_YESNO) == glHMI_YES)
	{
		m_lBHZ1BondPosOffsetX = 0;
		m_lBHZ1BondPosOffsetY = 0;

		szMsg.Format("Collet Offset is reset (x, y): (%d, %d)", m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY);
		SetStatusMessage(szMsg);

		SaveBinTableData();
	}
	else
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBinTable::AutoLearnBHZ1BondPosOffset(IPC_CServiceMessage& svMsg)
{
	if (AutoLearnBHZBondPosOffset( m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY))
	{
		SaveBinTableData();
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CBinTable::TeachBHZ1BondPosOffsetByCollet(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	IPC_CServiceMessage stMsg;
	if (SendRequestReply(BOND_PR_STN, "TeachBHZ1BondPos", stMsg))
	{
		stMsg.GetMsg(sizeof(BOOL), &bReturn);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CBinTable::ConfirmBHZ1BondPosOffsetByCollet(IPC_CServiceMessage& svMsg)
{
	ConfirmBHZBondPosOffsetByCollet("ConfirmBHZ1BondPos", m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

////Use Same Confirm Button for BH1 and BH2. TRUE for 1, FALSE for 2////
BOOL CBinTable::ConfirmBHZ1OrBHZ2BondPosOffsetByCollet(IPC_CServiceMessage& svMsg) 
{
	BOOL bBHZ1 = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bBHZ1);

	if(bBHZ1 == TRUE)
	{
		ConfirmBHZBondPosOffsetByCollet("ConfirmBHZ1BondPos", m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY);
	}
	else
	{
		ConfirmBHZBondPosOffsetByCollet("ConfirmBHZ2BondPos", m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//=====================================================Teach BHZ2 Bond position================================================= 
/*
BOOL CBinTable::ManualLearnBHZ2BondPosOffset(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lArm1_X, lArm1_Y, lArm2_X, lArm2_Y;

	SwitchToBPR();

	CMS896AStn::MotionSelectIncrementParam(BT_AXIS_X, "BT_JoySlow", &m_stBTAxis_X);
	CMS896AStn::MotionSelectIncrementParam(BT_AXIS_Y, "BT_JoySlow", &m_stBTAxis_Y);
	SetJoystickOn(TRUE);
	//HmiMessage("Please move to ARM 1 collet mark.", "Learn Dual Arm Collet Offset", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0);
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001
	HmiMessageEx("Please move to ARM 1 collet mark.", "Learn Dual Arm Collet Offset", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0);
	SetJoystickOn(FALSE);
	
	X_Sync();
	Y_Sync();
	GetEncoderValue();
	lArm1_X = m_lEnc_X;
	lArm1_Y = m_lEnc_Y;

	SetJoystickOn(TRUE);
	//HmiMessage("Please move to ARM 2 collet mark.", "Learn Dual Arm Collet Offset", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0);
	HmiMessageEx("Please move to ARM 2 collet mark.", "Learn Dual Arm Collet Offset", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0);
	SetJoystickOn(FALSE);
	
	X_Sync();
	Y_Sync();
	GetEncoderValue();
	lArm2_X = m_lEnc_X;
	lArm2_Y = m_lEnc_Y;

	m_lBHZ2BondPosOffsetX = lArm1_X - lArm2_X;
	m_lBHZ2BondPosOffsetY = lArm1_Y - lArm2_Y;

	SaveBinTableData();

CString szTemp;
szTemp.Format("Collet Offset(Manual) (x, y): (%d, %d)", m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY);
SetStatusMessage(szTemp);
AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
*/

BOOL CBinTable::ResetBHZ2BondPosOffset(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;

	if (HmiMessage("Are you sure to reset BHZ2 Bond Pos Offset?", "BHZ2 Bond Pos Offset", glHMI_MBX_YESNO) == glHMI_YES)
	{
		m_lBHZ2BondPosOffsetX = 0;
		m_lBHZ2BondPosOffsetY = 0;

		szMsg.Format("Collet Offset is reset (x, y): (%d, %d)", m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY);
		SetStatusMessage(szMsg);

		SaveBinTableData();
	}
	else
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBinTable::AutoLearnBHZ2BondPosOffset(IPC_CServiceMessage& svMsg)
{
	if (AutoLearnBHZBondPosOffset( m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY))
	{
		SaveBinTableData();
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CBinTable::TeachBHZ2BondPosOffsetByCollet(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	IPC_CServiceMessage stMsg;
	if (SendRequestReply(BOND_PR_STN, "TeachBHZ2BondPos", stMsg))
	{
		stMsg.GetMsg(sizeof(BOOL), &bReturn);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBinTable::ConfirmBHZ2BondPosOffsetByCollet(IPC_CServiceMessage& svMsg)
{
	ConfirmBHZBondPosOffsetByCollet("ConfirmBHZ2BondPos", m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
//====================================================================================================================


LONG CBinTable::MoveBinTableToSafePosn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSafe = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bSafe);
	
	if (m_bDisableBT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//BOOL m_bUseLargeBinArea = (BOOL)GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_LARGE_TRAVEL);
	if (!m_bUseLargeBinArea)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetJoystickOn(FALSE);

	if (!CMS896AApp::m_bUseSlowBTControlProfile)
	{
		X_Profile(LOW_PROF);
	}

	if (State() == IDLE_Q)
	{
		if (bSafe)
		{
			GetEncoderValue();
			m_lBpX = m_lEnc_X;
			m_lBpY = m_lEnc_Y;

			if (m_lBpY > (m_lBTUnloadPos_Y - 2000))
			{
				Sleep(100);
			}
		}
		else
		{
			if (CheckOutTableLimit(m_lBpX, m_lBpY) == 0)
			{
				Sleep(200);
			}
		}
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::MoveBinTableToMS90GBinPosn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bToGarbageBin = TRUE;
	INT nStatus = gnOK;

	svMsg.GetMsg(sizeof(BOOL), &bToGarbageBin);
	
	if (m_bDisableBT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (!IsMS90())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetJoystickOn(FALSE);

	if ( (m_lMS90MCCGarbageBinX == 0) && (m_lMS90MCCGarbageBinY == 0) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	if (bToGarbageBin)
	{
		GetEncoderValue();
		m_lBpX = m_lEnc_X;
		m_lBpY = m_lEnc_Y;

		if (CheckOutTableLimit(m_lMS90MCCGarbageBinX, m_lMS90MCCGarbageBinY) == 0)
		{
			nStatus = XY_MoveTo(m_lMS90MCCGarbageBinX, m_lMS90MCCGarbageBinY);
		}
	}
	else
	{
		if (CheckOutTableLimit(m_lBpX, m_lBpY) == 0)
		{
			nStatus = XY_MoveTo(m_lBpX, m_lBpY);
		}
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if (nStatus != gnOK)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::BTBackupMapCommand(IPC_CServiceMessage& svMsg)
{
	OpBackupMap();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CBinTable::BTGenAllTempFiles(IPC_CServiceMessage& svMsg)
{
	OpGenAllTempFile();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CBinTable::SetBTMoveLowProfile()
{
	if (m_bDisableBT)
	{
		return FALSE;
	}

	if (m_bUseDualTablesOption)		//v4.24T8
	{
		HomeTable2();
	}

	if (!CMS896AApp::m_bUseSlowBTControlProfile)
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
	}

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	return TRUE;
}


// Auto Clean Collet
LONG CBinTable::BTMoveToACCLiquid(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!SetBTMoveLowProfile())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//v4.42T7
	if (m_bUseDualTablesOption)
	{
		SetBT1FrameLevel(FALSE);
		Sleep(500);
		if (IsBT1FrameLevel() == FALSE)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}
//AfxMessageBox("Andrew: BT1 level at DOWN position?", MB_SYSTEMMODAL);

	LONG lX, lY;
	ACCGetTableMovePosition(lX, lY, 0);
	X_Sync();
	Y_Sync();

	XY_MoveTo(lX, lY);
	X_Sync();
	Y_Sync();

	CString szMsg;
	szMsg.Format("ACC: BIT move to drop liquid %d, %d", lX, lY);
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTMoveToACCClean(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lX, lY;

	if (!SetBTMoveLowProfile())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	ACCGetTableMovePosition(lX, lY, 1);
	lX = lX + m_lACCAreaSizeX/2;
	lY = lY + m_lACCAreaSizeY/2;
	X_Sync();
	Y_Sync();
	XY_MoveTo(lX, lY);
	X_Sync();
	Y_Sync();
	Sleep(10);
	CString szMsg;
	szMsg.Format("ACC: BIT move to clean up-left %d, %d", lX, lY);
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinTable::BTMoveToACCBrush(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!SetBTMoveLowProfile())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetJoystickOn(FALSE);
	X_Sync();
	Y_Sync();
	Sleep(50);
	SelectXYProfile(m_lACCBrushX - m_lCurXPosn, m_lACCBrushY - m_lCurYPosn);
	XY_MoveTo(m_lACCBrushX, m_lACCBrushY);
	X_Sync();
	Y_Sync();
	Sleep(50);

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


VOID CBinTable::SetBHStrongBlow(const BOOL bBHZ2, const BOOL bSet)
{
	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (pBondHead)
	{
		pBondHead->SetBHStrongBlow(bBHZ2, bSet);
	}
}


VOID CBinTable::SetBHPickVacuum(const BOOL bBHZ2, const BOOL bSet)
{
	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (pBondHead)
	{
		pBondHead->SetBHPickVacuum(bBHZ2, bSet);
	}
}

VOID CBinTable::BTColletCleanMove(const BOOL bBHZ2, const LONG lX1, const LONG lY1, const LONG lX2, const LONG lY2)
{
	if( m_bACCToggleBHVacuum )
	{
		SetBHStrongBlow(bBHZ2, TRUE);
		Sleep(500);

		SetBHStrongBlow(bBHZ2, FALSE);
		Sleep(500);

		//SetBHPickVacuum(bBHZ2, TRUE); @Matthew by Harry Ho
	}

	XY_MoveTo(lX1, lY1);
	Sleep(500);
	/*if( m_bACCToggleBHVacuum )
	{
		SetBHPickVacuum(bBHZ2, FALSE);
	}*/

	XY_MoveTo(lX2, lY2);
	Sleep(500);
}


LONG CBinTable::BTCycleACCOperation(IPC_CServiceMessage& svMsg)
{
	BOOL bBHZ2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBHZ2);

	BOOL bReturn = TRUE;

	if (!SetBTMoveLowProfile())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG lUL_X, lUL_Y, lLR_X, lLR_Y, lCtrX, lCtrY;
	ACCGetTableMovePosition(lCtrX, lCtrY, 1);
	lUL_X = lCtrX + m_lACCAreaSizeX/2;
	lUL_Y = lCtrY + m_lACCAreaSizeY/2;
	lLR_X = lCtrX - m_lACCAreaSizeX/2;
	lLR_Y = lCtrY - m_lACCAreaSizeY/2;
	CString szMsg;
	szMsg.Format("ACC: BIT move clean area %d, %d, %d, %d", lUL_X, lUL_Y, lLR_X, lLR_Y);
	//SetErrorMessage(szMsg);	//v4.50A12
	for(int i = 0; i < m_lACCCycleCount; i++)
	{
		//1. ===========================================
		//	move right&down
		BTColletCleanMove(bBHZ2, lLR_X, lUL_Y, lLR_X, lLR_Y);
		//2. ===========================================
		//	move Left&Up
		BTColletCleanMove(bBHZ2, lUL_X, lLR_Y, lUL_X, lUL_Y);
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if( m_lACCAreaLimit>0 )
	{
		m_lACCAreaCount++;
		if( m_lACCAreaCount>=m_lACCAreaLimit )
		{
			m_lACCAreaIndex++;
			m_lACCAreaCount = 0;
		}
	}

	if( m_lACCAreaIndex>(m_lACCMatrixRow*m_lACCMatrixCol) )
	{
		bReturn = FALSE;
		CString szMsg;
		szMsg = "BT auto clean collet cotton life time expired!";
		SetErrorMessage(szMsg);
		HmiMessage_Red_Yellow(szMsg);
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( pApp->GetCustomerName()!="SanAn" )
		{
			m_lACCAreaIndex = 1;
			m_lACCAreaCount = 0;
		}
	}
	m_lACCReplaceCount	= (m_lACCAreaIndex-1)*m_lACCAreaLimit + m_lACCAreaCount;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::BTCycleACCOperation2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!SetBTMoveLowProfile())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	LONG lUL_X, lUL_Y, lLR_X, lLR_Y, lCtrX, lCtrY;
	ACCGetTableMovePosition(lCtrX, lCtrY, 1);
	lUL_X = lCtrX + m_lACCAreaSizeX/2;
	lUL_Y = lCtrY + m_lACCAreaSizeY/2;
	lLR_X = lCtrX - m_lACCAreaSizeX/2;
	lLR_Y = lCtrY - m_lACCAreaSizeY/2;
	CString szMsg;
	szMsg.Format("ACC: BIT move clean area %d, %d, %d, %d", lUL_X, lUL_Y, lLR_X, lLR_Y);
	//SetErrorMessage(szMsg);	//v4.50A12

	LONG lACCCycleCount = 1;
	int i;
	for(i=0; i<lACCCycleCount; i++)
	{
		XY_MoveTo(lLR_X, lUL_Y);	Sleep(150);	//	move right
		XY_MoveTo(lLR_X, lLR_Y);	Sleep(150);	//	move down
		XY_MoveTo(lUL_X, lLR_Y);	Sleep(150);	//	move left
		XY_MoveTo(lUL_X, lUL_Y);	Sleep(150);	//	move up
	}

	//if (!CMS896AApp::m_bUseSlowBTControlProfile)
	//{
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	//}

	if( m_lACCAreaLimit>0 )
	{
		m_lACCAreaCount++;
		if( m_lACCAreaCount>=m_lACCAreaLimit )
		{
			m_lACCAreaIndex++;
			m_lACCAreaCount = 0;
		}
	}

	if( m_lACCAreaIndex>(m_lACCMatrixRow*m_lACCMatrixCol) )
	{
		bReturn = FALSE;
		CString szMsg;
		szMsg = "BT auto clean collet cotton life time expired!";
		SetErrorMessage(szMsg);
		HmiMessage_Red_Yellow(szMsg);
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( pApp->GetCustomerName()!="SanAn" )
		{
			m_lACCAreaIndex = 1;
			m_lACCAreaCount = 0;
		}
	}
	m_lACCReplaceCount	= (m_lACCAreaIndex-1)*m_lACCAreaLimit + m_lACCAreaCount;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//=============================================Remove Dirt Alcohol Position=============================================
LONG CBinTable::BTMoveToRemoveDirt(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!SetBTMoveLowProfile())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_lCleanDirtMatrixCol == 0)
	{
		m_lCleanDirtMatrixCol = 1;
	}
	if (m_lCleanDirtMatrixRow == 0)
	{
		m_lCleanDirtMatrixRow = 1;
	}
	double dPitchX = (m_lCleanDirtLowerRightPosX - m_lCleanDirtUpleftPosX) / m_lCleanDirtMatrixCol;
	double dPitchY = (m_lCleanDirtLowerRightPosY - m_lCleanDirtUpleftPosY) / m_lCleanDirtMatrixRow;
	LONG lX = m_lCleanDirtUpleftPosX + _round(dPitchX * (m_lCleanDirtColIndex + 0.5));
	LONG lY = m_lCleanDirtUpleftPosY + _round(dPitchY * (m_lCleanDirtRowIndex + 0.5));

	X_Sync();
	Y_Sync();
	XY_MoveTo(lX, lY);
	X_Sync();
	Y_Sync();
	Sleep(10);
	CString szMsg;
	szMsg.Format("ACC: BIT move to clean up-left %d, %d", lX, lY);
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTCycleRemoveDirtOperation(IPC_CServiceMessage& svMsg)
{
	BOOL bBHZ2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBHZ2);

	BOOL bReturn = TRUE;
/*
	if (!SetBTMoveLowProfile())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	double dPitchX = (m_lCleanDirtLowerRightPosX - m_lCleanDirtUpleftPosX) / m_lCleanDirtMatrixCol;
	double dPitchY = (m_lCleanDirtLowerRightPosY - m_lCleanDirtUpleftPosY) / m_lCleanDirtMatrixRow;

	LONG lUL_X = m_lCleanDirtUpleftPosX + _round(dPitchX * (m_lCleanDirtColIndex + 0.2));
	LONG lUL_Y = m_lCleanDirtUpleftPosY + _round(dPitchY * (m_lCleanDirtRowIndex + 0.2));
	LONG lLR_X = m_lCleanDirtUpleftPosX + _round(dPitchX * (m_lCleanDirtColIndex + 0.8));
	LONG lLR_Y = m_lCleanDirtUpleftPosY + _round(dPitchY * (m_lCleanDirtRowIndex + 0.8));

	for(int i = 0; i < 2; i++)
	{
		//1. ===========================================
		//	move right&down
		BTColletCleanMove(bBHZ2, lLR_X, lUL_Y, lLR_X, lLR_Y);
		//2. ===========================================
		//	move Left&Up
		BTColletCleanMove(bBHZ2, lUL_X, lLR_Y, lUL_X, lUL_Y);
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
*/
	m_lCleanDirtColIndex++;
	if (m_lCleanDirtColIndex > m_lCleanDirtMatrixCol)
	{
		m_lCleanDirtRowIndex++;
		m_lCleanDirtColIndex = 0;
		if (m_lCleanDirtRowIndex > m_lCleanDirtMatrixRow)
		{
//			bReturn = FALSE;
//			CString szMsg;
//			szMsg = "Please clean BT remove dirt area!";
//			SetErrorMessage(szMsg);
//			HmiMessage_Red_Yellow(szMsg);
			m_lCleanDirtColIndex = 0;
			m_lCleanDirtRowIndex = 0;
		}
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


LONG CBinTable::BTChangePosition(IPC_CServiceMessage& svMsg)
{
	LONG lOption = 0;
	LONG lMoveX, lMoveY;

	svMsg.GetMsg(sizeof(LONG), &lOption);
	switch(lOption)
	{
	case 0:
		lMoveX = m_lACCLiquidX;
		lMoveY = m_lACCLiquidY;
		break;
	case 1:
		lMoveX = m_lACCBrushX;
		lMoveY = m_lACCBrushY;
		break;
	case 2:		//v4.51A19	//Silan MS90 NGPIck
		lMoveX = m_lNGPickPocketX;
		lMoveY = m_lNGPickPocketY;
		break;
	default:
		return 1;
		break;
	}

	if (m_bUseDualTablesOption)		//v4.24T8
	{
		HomeTable2();
	}

	SetJoystickOn(FALSE);
	SelectXYProfile(lMoveX - m_lCurXPosn, lMoveY - m_lCurYPosn);
	XY_MoveTo(lMoveX, lMoveY);
	SetJoystickOn(TRUE);

	return 1;
}

LONG CBinTable::BTCancelSetup(IPC_CServiceMessage& svMsg)
{
	INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveToPrePick", svMsg);
	m_comClient.ScanReplyForConvID(nConvID, 3600000);
	m_comClient.ReadReplyForConvID(nConvID,svMsg);

	return 1;
}

LONG CBinTable::BTConfirmSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lOption = 0;
	LONG lOldX, lOldY;

	svMsg.GetMsg(sizeof(LONG), &lOption);
	GetEncoderValue();

	if (lOption == 0)
	{
		lOldX = m_lACCLiquidX;
		lOldY = m_lACCLiquidY;
		m_lACCLiquidX = m_lEnc_X;
		m_lACCLiquidY = m_lEnc_Y;
	}
	else if ( lOption==1 )
	{
		lOldX = m_lACCBrushX;
		lOldY = m_lACCBrushY;
		m_lACCBrushX = m_lEnc_X;
		m_lACCBrushY = m_lEnc_Y;
	}
	else if (lOption == 2)		//v4.51A19	//Silan MS90 NGPIck
	{
		lOldX = m_lNGPickPocketX;
		lOldY = m_lNGPickPocketY;
		m_lNGPickPocketX = m_lEnc_X;
		m_lNGPickPocketY = m_lEnc_Y;
	}
	else 
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (lOption < 2)	//v4.51A19
	{
		IPC_CServiceMessage stMsg;
		if (BTAccConfirmSetup(stMsg) == 0)
		{
			if( lOption==0 )
			{
				m_lACCLiquidX = lOldX;
				m_lACCLiquidY = lOldY;
			}
			else
			{
				m_lACCBrushX = lOldX;
				m_lACCBrushY = lOldY;
			}
			return 1;
		}
	}

	SaveBinTableData();

	INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveToPrePick", svMsg);
	m_comClient.ScanReplyForConvID(nConvID, 3600000);
	m_comClient.ReadReplyForConvID(nConvID,svMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::BTAccConfirmSetup(IPC_CServiceMessage& svMsg)
{
	LONG lUL_X, lUL_Y, lLR_X, lLR_Y;

	lUL_X = m_lACCBrushX + m_lACCRangeX/2;
	lLR_X = m_lACCBrushX - m_lACCRangeX/2;
	lUL_Y = m_lACCBrushY + m_lACCRangeY/2;
	lLR_Y = m_lACCBrushY - m_lACCRangeY/2;

	BOOL bReturn = FALSE;
	if( CheckTableOutLimitX(lUL_X) || CheckTableOutLimitX(lLR_X) )
	{
		HmiMessage("ACC CLEAN Work Range X out of table limit!\nPlease reteach CLEAN position\nOr change Work Range Y");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 0;
	}

	if( CheckTableOutLimitY(lUL_Y) || CheckTableOutLimitY(lLR_Y) )
	{
		HmiMessage("ACC CLEAN Work Range Y out of table limit!\nPlease reteach CLEAN position\nOr change Work Range Y");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 0;
	}

	lUL_X = m_lACCLiquidX + m_lACCRangeX/2;
	lLR_X = m_lACCLiquidX - m_lACCRangeX/2;
	lUL_Y = m_lACCLiquidY + m_lACCRangeY/2;
	lLR_Y = m_lACCLiquidY - m_lACCRangeY/2;

	if( CheckTableOutLimitX(lUL_X) || CheckTableOutLimitX(lLR_X) )
	{
		HmiMessage("ACC DROP position X out of table limit!\nPlease reteach DROP position\nOr change Work Range X");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 0;
	}

	if( CheckTableOutLimitY(lUL_Y) || CheckTableOutLimitY(lLR_Y) )
	{
		HmiMessage("ACC DROP position Y out of table limit!\nPlease reteach DROP position\nOr change Work Range Y");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 0;
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::BTAccReset(IPC_CServiceMessage& svMsg)
{
	LONG lDCtr_X, lDCtr_Y, lDLft_X, lDTop_Y, lDRgt_X, lDBtm_Y;
	LONG lBCtr_X, lBCtr_Y, lBLft_X, lBTop_Y, lBRgt_X, lBBtm_Y;
	LONG lCtrX, lCtrY;

#define DISPLAY_DIR		1
#define DISPLAY_DIRX	-1
	FILE *fpShow = NULL;
	errno_t nErr = fopen_s(&fpShow, "c:\\mapsorter\\userdata\\history\\AccSetup.txt", "w");
	if ((nErr != 0) || (fpShow == NULL))		//Klocwork
	{
		return 1;
	}

	lDLft_X = m_lACCLiquidX + m_lACCRangeX/2;
	lDTop_Y = m_lACCLiquidY + m_lACCRangeY/2;
	lDRgt_X = m_lACCLiquidX - m_lACCRangeX/2;
	lDBtm_Y = m_lACCLiquidY - m_lACCRangeY/2;

	lBLft_X = m_lACCBrushX  + m_lACCRangeX/2;
	lBTop_Y = m_lACCBrushY  + m_lACCRangeY/2;
	lBRgt_X = m_lACCBrushX  - m_lACCRangeX/2;
	lBBtm_Y = m_lACCBrushY  - m_lACCRangeY/2;

	fprintf(fpShow, "%ld,%ld,%ld,%ld\n", lDLft_X*DISPLAY_DIRX, lDTop_Y*DISPLAY_DIR,	lBLft_X*DISPLAY_DIRX, lBTop_Y*DISPLAY_DIR);
	fprintf(fpShow, "%ld,%ld,%ld,%ld\n", lDRgt_X*DISPLAY_DIRX, lDTop_Y*DISPLAY_DIR,	lBRgt_X*DISPLAY_DIRX, lBTop_Y*DISPLAY_DIR);
	fprintf(fpShow, "%ld,%ld,%ld,%ld\n", lDLft_X*DISPLAY_DIRX, lDBtm_Y*DISPLAY_DIR,	lBLft_X*DISPLAY_DIRX, lBBtm_Y*DISPLAY_DIR);
	fprintf(fpShow, "%ld,%ld,%ld,%ld\n", lDRgt_X*DISPLAY_DIRX, lDBtm_Y*DISPLAY_DIR,	lBRgt_X*DISPLAY_DIRX, lBBtm_Y*DISPLAY_DIR);

	CString szMsg;
	szMsg.Format("ACC: drop range  %ld, %ld, %ld, %ld", lDLft_X, lDTop_Y, lDRgt_X , lDBtm_Y);
	SetErrorMessage(szMsg);
	szMsg.Format("ACC: clean range %ld, %ld, %ld, %ld", lBLft_X, lBTop_Y, lBRgt_X, lBBtm_Y);
	SetErrorMessage(szMsg);

	for(m_lACCAreaIndex=1; m_lACCAreaIndex<=(m_lACCMatrixRow*m_lACCMatrixCol); m_lACCAreaIndex++)
	{
		ACCGetTableMovePosition(lCtrX, lCtrY, 0);
		lDCtr_X = lCtrX;
		lDCtr_Y = lCtrY;
		ACCGetTableMovePosition(lCtrX, lCtrY, 1);
		lBLft_X = lCtrX + m_lACCAreaSizeX/2;
		lBTop_Y = lCtrY + m_lACCAreaSizeY/2;
		lBRgt_X = lCtrX - m_lACCAreaSizeX/2;
		lBBtm_Y = lCtrY - m_lACCAreaSizeY/2;
		lBCtr_X = lCtrX;
		lBCtr_Y = lCtrY;

		fprintf(fpShow, "%ld,%ld,%ld,%ld\n", lDCtr_X*DISPLAY_DIRX, lDCtr_Y*DISPLAY_DIR,	lBLft_X*DISPLAY_DIRX, lBTop_Y*DISPLAY_DIR);
		fprintf(fpShow, "%ld,%ld,%ld,%ld\n", lDCtr_X*DISPLAY_DIRX, lDCtr_Y*DISPLAY_DIR,	lBRgt_X*DISPLAY_DIRX, lBTop_Y*DISPLAY_DIR);
		fprintf(fpShow, "%ld,%ld,%ld,%ld\n", lDCtr_X*DISPLAY_DIRX, lDCtr_Y*DISPLAY_DIR,	lBLft_X*DISPLAY_DIRX, lBBtm_Y*DISPLAY_DIR);
		fprintf(fpShow, "%ld,%ld,%ld,%ld\n", lDCtr_X*DISPLAY_DIRX, lDCtr_Y*DISPLAY_DIR,	lBRgt_X*DISPLAY_DIRX, lBBtm_Y*DISPLAY_DIR);
		fprintf(fpShow, "%ld,%ld,%ld,%ld\n", lDCtr_X*DISPLAY_DIRX, lDCtr_Y*DISPLAY_DIR, lBCtr_X*DISPLAY_DIRX, lBCtr_Y*DISPLAY_DIR);

		szMsg.Format("ACC: drop %ld point %ld, %ld", m_lACCAreaIndex, lDCtr_X, lDCtr_Y);
		SetErrorMessage(szMsg);
		szMsg.Format("ACC: clean %ld area %ld, %ld, %ld, %ld", m_lACCAreaIndex, lBLft_X, lBTop_Y, lBRgt_X, lBBtm_Y);
		SetErrorMessage(szMsg);
	}
	fclose(fpShow);

	m_lACCAreaCount = 0;
	m_lACCAreaIndex = 1;
	m_lACCReplaceCount	= (m_lACCAreaIndex-1)*m_lACCAreaLimit + m_lACCAreaCount;

	return 1;
}

BOOL CBinTable::CheckTableOutLimitX(LONG lX)
{
	if( (lX>m_lTableXPosLimit) || (lX<m_lTableXNegLimit) )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBinTable::CheckTableOutLimitY(LONG lY)
{
	if( (lY>m_lTableYPosLimit) || (lY<m_lTableYNegLimit) )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBinTable::ACCGetTableMovePosition(LONG &lX, LONG &lY, LONG lOption)
{
	LONG lBaseX, lBaseY;
	if( lOption==0 )
	{
		lBaseX = m_lACCLiquidX;
		lBaseY = m_lACCLiquidY;
	}
	else
	{
		lBaseX = m_lACCBrushX;
		lBaseY = m_lACCBrushY;
	}
	lX = lBaseX;
	lY = lBaseY;


	LONG lPitchX = 0;
	LONG lPitchY = 0;
	if( m_lACCMatrixCol>1 )
	{
		lBaseX = lBaseX + (m_lACCRangeX - m_lACCAreaSizeX)/2;
		lPitchX = (m_lACCRangeX-m_lACCAreaSizeX)/(m_lACCMatrixCol-1);
	}
	if( m_lACCMatrixRow>1 )
	{
		lBaseY = lBaseY + (m_lACCRangeY - m_lACCAreaSizeY)/2;
		lPitchY = (m_lACCRangeY-m_lACCAreaSizeY)/(m_lACCMatrixRow-1);
	}

	if( m_lACCMatrixCol>0 )
	{
		lX = lBaseX - lPitchX*((m_lACCAreaIndex-1)%m_lACCMatrixCol);
		lY = lBaseY - lPitchY*((m_lACCAreaIndex-1)/m_lACCMatrixCol);
	}

	return TRUE;
}

LONG CBinTable::BTChangeAGCPosition(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	LONG lOption = 0;
	LONG lMoveX=0, lMoveY=0;

	svMsg.GetMsg(sizeof(LONG), &lOption);
	switch(lOption)
	{
	case 1:     //BT move to PR uploading position to search the collet PR with uplook
		lMoveX = m_lChgColletUPLUploadPosX;
		lMoveY = m_lChgColletUPLUploadPosY;
		break;
	case 2:		//Holder(BT) Move to Upload position to get a new collet from upload module
		lMoveX = m_lChgColletHolderUploadPosX;  //Try2 --- UploadCollet(holder)
		lMoveY = m_lChgColletHolderUploadPosY;
		break;
	case 3:		//Holder(BT) move to the install position to attach collet into BH after get a new collect from upload.
		lMoveX = m_lChgColletHolderInstallPosX;  //Tray1 -- install collet(holder)
		lMoveY = m_lChgColletHolderInstallPosY;
		break;
	case 4:		//Pusher position on BT to tighten collet			//v4.52A14
		lMoveX = m_lChgColletPusher3PosX;   //Pusher
		lMoveY = m_lChgColletPusher3PosY;
		break;
	case 5:		//Uplook camera PR position to Search Collet		//v4.50A11	
		lMoveX = m_lChgColletUpLookPosX;
		lMoveY = m_lChgColletUpLookPosY;
		break;
	case 6:		//Clean Dirt Area for Collet Clean	
		lMoveX = m_lCleanDirtUpleftPosX;
		lMoveY = m_lCleanDirtUpleftPosY;
		break;
	case 7:		//Clean Dirt Area for Collet Clean	
		lMoveX = m_lCleanDirtLowerRightPosX;
		lMoveY = m_lCleanDirtLowerRightPosY;
		break;
	
	case 0:		//Collet clamp position to detach collet
	default:
		lMoveX = m_lChgColletClampPosX;
		lMoveY = m_lChgColletClampPosY;
		break;
	}

	SetJoystickOn(FALSE);
	if (m_bUseDualTablesOption)	
	{
		HomeTable2();
	}
	SelectXYProfile(lMoveX - m_lCurXPosn, lMoveY - m_lCurYPosn);
	XY_MoveTo(lMoveX, lMoveY);
	SetJoystickOn(TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTConfirmAGCSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	CString szMsg;
	LONG lOption = 0;
	LONG lOldX=0, lOldY=0;
	svMsg.GetMsg(sizeof(LONG), &lOption);
	
	GetEncoderValue();
	if (lOption == 0)
	{
		lOldX = m_lChgColletClampPosX;
		lOldY = m_lChgColletClampPosY;
		m_lChgColletClampPosX = m_lEnc_X;
		m_lChgColletClampPosY = m_lEnc_Y;

		szMsg.Format("Change-Collet CLAMP position is updated to (%ld, %ld)", 
			m_lChgColletClampPosX, m_lChgColletClampPosY);
		HmiMessage(szMsg);

		if( m_lChgColletHolderInstallPosX == 0 && m_lChgColletHolderInstallPosY == 0 && 
			m_lChgColletHolderUploadPosX == 0 && m_lChgColletHolderUploadPosX == 0 &&
			m_lChgColletUpLookPosX == 0 && m_lChgColletUpLookPosY == 0 )
		{
			LONG lHmiMsgReply = 3;	//Yes
			CString szTitle, szContent;
			szTitle = "Set the other Position value";
			szContent = "Do set other positions the same as Collet CLAMP position?";
			lHmiMsgReply = HmiMessage(szContent, szTitle, 103, 3);
			if(lHmiMsgReply == 3)
			{
				m_lChgColletUPLUploadPosX		= m_lEnc_X;
				m_lChgColletUPLUploadPosY		= m_lEnc_Y;
				m_lChgColletHolderUploadPosX	= m_lEnc_X;
				m_lChgColletHolderUploadPosY	= m_lEnc_Y;
				m_lChgColletHolderInstallPosX	= m_lEnc_X;
				m_lChgColletHolderInstallPosY	= m_lEnc_Y;
				m_lChgColletPusher3PosX			= m_lEnc_X;
				m_lChgColletPusher3PosY			= m_lEnc_Y;
				m_lChgColletUpLookPosX			= m_lEnc_X;
				m_lChgColletUpLookPosY			= m_lEnc_Y;

				szMsg.Format("Change-Collet All positions are updated to (%ld, %ld)", 
				m_lEnc_X, m_lEnc_Y);
				HmiMessage(szMsg);
		
			}

		}
	}
	else if (lOption == 1)	//BT UPLUpload position to seach a collet on the upload module		//v4.52A14
	{
		lOldX = m_lChgColletUPLUploadPosX;
		lOldY = m_lChgColletUPLUploadPosY;
		m_lChgColletUPLUploadPosX = m_lEnc_X;
		m_lChgColletUPLUploadPosY = m_lEnc_Y;
		
		szMsg.Format("Change-Collet Uplook Upload position is updated to (%ld, %ld)", 
			m_lChgColletUPLUploadPosX, m_lChgColletUPLUploadPosY);
		HmiMessage(szMsg);
	}
	else if (lOption == 2)	//BT Holder Upload position
	{
		lOldX = m_lChgColletHolderUploadPosX;
		lOldY = m_lChgColletHolderUploadPosY;
		m_lChgColletHolderUploadPosX = m_lEnc_X;
		m_lChgColletHolderUploadPosY = m_lEnc_Y;
		
		szMsg.Format("Change-Collet Holder Upload position is updated to (%ld, %ld)", 
			m_lChgColletHolderUploadPosX, m_lChgColletHolderUploadPosY);
		HmiMessage(szMsg);
	}
	else if (lOption == 3)	//BT Holder Install position
	{
		lOldX = m_lChgColletHolderInstallPosX;
		lOldY = m_lChgColletHolderInstallPosY;
		m_lChgColletHolderInstallPosX = m_lEnc_X;
		m_lChgColletHolderInstallPosY = m_lEnc_Y;
		
		szMsg.Format("Change-Collet Holder install position is updated to (%ld, %ld)", 
			m_lChgColletHolderInstallPosX, m_lChgColletHolderInstallPosY);
		HmiMessage(szMsg);
	}
	else if (lOption == 4)	//BT Pusher position to install collet		//v4.52A14
	{
		lOldX = m_lChgColletPusher3PosX;
		lOldY = m_lChgColletPusher3PosY;
		m_lChgColletPusher3PosX = m_lEnc_X;
		m_lChgColletPusher3PosY = m_lEnc_Y;
		
		szMsg.Format("Change-Collet PUSHER 3 position is updated to (%ld, %ld)", 
			m_lChgColletPusher3PosX, m_lChgColletPusher3PosY);
		HmiMessage(szMsg);
	}
	else if (lOption == 5)	//Uplook PR for BondArm Collet
	{
		lOldX = m_lChgColletUpLookPosX;
		lOldY = m_lChgColletUpLookPosY;
		m_lChgColletUpLookPosX = m_lEnc_X;
		m_lChgColletUpLookPosY = m_lEnc_Y;
		
		szMsg.Format("Change-Collet Uplook position is updated to (%ld, %ld)", 
			m_lChgColletUpLookPosX, m_lChgColletUpLookPosY);
		HmiMessage(szMsg);
	}
	else if (lOption == 6)	//Clean Dirt Area for Collet Clean	
	{
		lOldX = m_lCleanDirtUpleftPosX;
		lOldY = m_lCleanDirtUpleftPosY;
		m_lCleanDirtUpleftPosX = m_lEnc_X;
		m_lCleanDirtUpleftPosY = m_lEnc_Y;
		
		szMsg.Format("Clean Dirt Upper-Left position is updated to (%ld, %ld)", 
			m_lCleanDirtUpleftPosX, m_lCleanDirtUpleftPosY);
		HmiMessage(szMsg);
	}
	else if (lOption == 7)	//Clean Dirt Area for Collet Clean	
	{
		lOldX = m_lCleanDirtLowerRightPosX;
		lOldY = m_lCleanDirtLowerRightPosY;
		m_lCleanDirtLowerRightPosX = m_lEnc_X;
		m_lCleanDirtLowerRightPosY = m_lEnc_Y;
		
		szMsg.Format("Clean Dirt Upper-Left position is updated to (%ld, %ld)", 
			m_lCleanDirtLowerRightPosX, m_lCleanDirtLowerRightPosY);
		HmiMessage(szMsg);
	}

	SaveBinTableData();

	INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveToPrePick", svMsg);
	m_comClient.ScanReplyForConvID(nConvID, 3600000);
	m_comClient.ReadReplyForConvID(nConvID, svMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTCancelAGCSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveToPrePick", svMsg);
	m_comClient.ScanReplyForConvID(nConvID, 3600000);
	m_comClient.ReadReplyForConvID(nConvID, svMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


VOID CBinTable::GetBHXYOffset(const CString szTitle, const BOOL bBHZ2, const BOOL bCalcEJColletOffset, LONG &lX, LONG &lY)
{
	CString szLog			= "";
	CString szBHZ2Offset	= "";
	CString szEjtXYOffset	= "";
	LONG lOrgX = lX, lOrgY = lY;

	LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
	LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
	GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dXResolution_UM_CNT, FALSE);

	if (bBHZ2)
	{
		//BHZ2 Offset
//		lX += m_lBHZ2BondPosOffsetX;
//		lY += m_lBHZ2BondPosOffsetY;
		szBHZ2Offset.Format("; BHZ2 Offset +(%ld, %ld)", m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY);

		//Ejt XY BHZ2 Offset
		if (CMS896AStn::m_bMS100EjtXY && CMS896AStn::m_bEnableMS100EjtXY)
		{
			if (/*m_bBTIsMoveCollet == TRUE &&*/ m_bEnableMS100EjtXY == TRUE)
			{
				if (bCalcEJColletOffset)
				{
					lX = _round(lX - lCollet2OffsetX);
					lY = _round(lY - lCollet2OffsetY);
				}
				szEjtXYOffset.Format("; EJTXY Offset Z2 -(%ld, %ld)", lCollet2OffsetX, lCollet2OffsetY);
			}
		}

		szLog.Format("BT %s for BHZ2 = (%ld, %ld), Orig(%ld, %ld)",  
					 (const char*)szTitle, lX, lY, lOrgX, lOrgY);
	}
	else
	{
//BHZ1 Offset
//		lX += m_lBHZ1BondPosOffsetX;
//		lY += m_lBHZ1BondPosOffsetY;
		szBHZ2Offset.Format("; BHZ1 Offset +(%ld, %ld)", m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY);

		if (CMS896AStn::m_bMS100EjtXY && CMS896AStn::m_bEnableMS100EjtXY)
		{
			if (/*m_bBTIsMoveCollet == TRUE &&*/ m_bEnableMS100EjtXY == TRUE)
			{
				if (bCalcEJColletOffset)
				{
					lX = _round(lX - lCollet1OffsetX);
					lY = _round(lY - lCollet1OffsetY);
				}
				szEjtXYOffset.Format("; EJTXY Offset Z1 -(%ld, %ld)", lCollet1OffsetX, lCollet1OffsetY);
			}
		}

		szLog.Format("BT %s for BHZ1 = (%ld, %ld), Orig(%ld, %ld)",  
					 (const char*)szTitle, lX, lY, lOrgX, lOrgY);
	}

	szLog = szLog + szBHZ2Offset + szEjtXYOffset;
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
}

BOOL CBinTable::IsAutoChangeColletReady()
{
	if (m_bDisableBT)
	{
		return FALSE;
	}
	if (!CMS896AStn::m_bAutoChangeCollet)
	{
		return FALSE;
	}

	if (m_bUseDualTablesOption)		//v4.24T8
	{
		HomeTable2();
	}

	return TRUE;
}

LONG CBinTable::BTMoveToAGCClampPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bBHZ2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBHZ2);

	if (!IsAutoChangeColletReady())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	SetJoystickOn(FALSE);
	Sleep(50);
	
	LONG lX = m_lChgColletClampPosX;
	LONG lY = m_lChgColletClampPosY;
	GetBHXYOffset("CLAMP Pos", bBHZ2, TRUE, lX, lY);
	
	LONG nErr = XY_MoveTo(lX, lY);
	Sleep(200);

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if (nErr)
	{
		bReturn = FALSE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTMoveToAGCUPLUploadPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!IsAutoChangeColletReady())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	SetJoystickOn(FALSE);
	Sleep(50);
	
	LONG lX = m_lChgColletUPLUploadPosX;
	LONG lY = m_lChgColletUPLUploadPosY;
	
	LONG nErr = XY_MoveTo(lX, lY);
	Sleep(200);

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if (nErr)
	{
		bReturn = FALSE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTMoveToAGCHolderUploadPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bBHZ2 = FALSE;

	if (!IsAutoChangeColletReady())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	SetJoystickOn(FALSE);
	Sleep(50);
	
	LONG lX = m_lChgColletHolderUploadPosX;
	LONG lY = m_lChgColletHolderUploadPosY;
	
	LONG nErr = XY_MoveTo(lX, lY);
	Sleep(200);

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if (nErr)
	{
		bReturn = FALSE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTMoveToAGCHolderInstallPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bBHZ2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBHZ2);

	if (!IsAutoChangeColletReady())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	SetJoystickOn(FALSE);
	Sleep(50);
	
	LONG lX = m_lChgColletHolderInstallPosX;
	LONG lY = m_lChgColletHolderInstallPosY;
	
	GetBHXYOffset("Holder Install Pos", bBHZ2, TRUE, lX, lY);
	
	LONG nErr = XY_MoveTo(lX, lY);
	Sleep(200);

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if (nErr)
	{
		bReturn = FALSE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTMoveToAGCPusher3Pos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bBHZ2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBHZ2);

	if (!IsAutoChangeColletReady())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	SetJoystickOn(FALSE);
	Sleep(50);
	
	LONG lX = m_lChgColletPusher3PosX;
	LONG lY = m_lChgColletPusher3PosY;
	GetBHXYOffset("PUSHER 3 Pos", bBHZ2, TRUE, lX, lY);
	
	LONG nErr = XY_MoveTo(lX, lY);
	Sleep(200);

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if (nErr)
	{
		bReturn = FALSE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTMoveToAGCUpLookPos(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bBHZ2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBHZ2);

	if (!IsAutoChangeColletReady())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	SetJoystickOn(FALSE);
	Sleep(50);
	
	LONG lX = m_lChgColletUpLookPosX;
	LONG lY = m_lChgColletUpLookPosY;
	GetBHXYOffset("UpLook Pos", bBHZ2, FALSE, lX, lY);
	
	LONG nErr = XY_MoveTo(lX, lY);
	Sleep(200);

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if (nErr)
	{
		bReturn = FALSE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinTable::GenerateConfigData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;

	szMsg = _T("BT: Generate Configuration Data");
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	bReturn = GenerateConfigData();

	szMsg.Format("BT: Generate Configuration Data done - %d", bReturn);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::GeneratePkgDataFile(IPC_CServiceMessage &svMsg)
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

	szLine.Format("input count:,%ld\n", m_oBinBlkMain.GrabInputCount(1));	//v4.22T1
	oFile.WriteString(szLine);
	oFile.WriteString("Die pitch(um):\n");
	szLine.Format(",X:,%ld\n",			_round(m_dDiePitchX));
	oFile.WriteString(szLine);
	szLine.Format(",Y:,%ld\n",			_round(m_dDiePitchY));
	oFile.WriteString(szLine);

	oFile.WriteString("Die Quantities:\n");
	szLine.Format(",Row:,%lu\n",		m_oBinBlkMain.GrabNoOfDiePerRow(1));
	oFile.WriteString(szLine);
	szLine.Format(",Col:,%lu\n",		m_oBinBlkMain.GrabNoOfDiePerCol(1));
	oFile.WriteString(szLine);

	oFile.Close();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::PkgKeyParametersTask(IPC_CServiceMessage& svMsg)
{
	CStdioFile kFile;
	BOOL bReturn = TRUE;
	LONG lAction = 0;
	svMsg.GetMsg(sizeof(LONG), &lAction);

	m_ulGradeCapacity	= m_oBinBlkMain.GrabGradeCapacity(1);
	m_ulInputCount		= m_oBinBlkMain.GrabInputCount(1);
	m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(1));
	m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(1));
	DOUBLE	dDiePitchX		= (LONG) m_dDiePitchX;
	DOUBLE	dDiePitchY		= (LONG) m_dDiePitchY;
	DOUBLE	dBinMapRadius	= m_dBinMapCircleRadiusInMm;
	BOOL	bEnableBinMap	= m_bEnableBinMapBondArea;
	ULONG	ulGradeCapacity	= m_ulGradeCapacity;
	ULONG	ulInputCount	= m_ulInputCount;
	CString szExtraBinInfo	= m_szaExtraBinInfo[0];
	CString	szOutputFormat	= m_szBinOutputFileFormat;

	if( lAction==0 || lAction==1 )
	{
		if( lAction==1 )
		{
			CStringMapFile psmf;
			CString szNewMsdName = "C:\\MapSorter\\UserData\\BinTable.msd";
			if ( psmf.Open(szNewMsdName, FALSE, FALSE) )
			{
				dDiePitchX		= (LONG) (psmf)["BinBlock"][1]["DiePitchX"];
				dDiePitchY		= (LONG) (psmf)["BinBlock"][1]["DiePitchY"];
				dBinMapRadius	= (psmf)[BT_TABLE_OPTION][BT_BINMAP_CIRCLE_RADIUS];
				bEnableBinMap	= (BOOL)(LONG)(psmf)[BT_TABLE_OPTION][BT_BINMAP];
				ulGradeCapacity	= (psmf)["BinGradeData"][1]["GradeCapacity"];
				ulInputCount	= (psmf)["BinGradeData"][1]["InputCount"];
				CString szTemp;
				szTemp.Format("%s %d",BT_EXTRA_BIN_INFO_ITEM,1);
				szExtraBinInfo	= (psmf)[BT_OUTPUTFILE_OPTION][BT_EXTRA_BIN_INFO][szTemp];
				szOutputFormat	= (psmf)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FORMAT_NAME];
			}
			psmf.Close();
			DeleteFile(szNewMsdName);
		}
		if( kFile.Open(gszLocalPkgCheckListFile, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText) )
		{
			kFile.SeekToEnd();
			CString szText;
			szText.Format("%s",		"BIT list begin");		kFile.WriteString(szText + "\n");
			szText.Format("%f",		dDiePitchX);			kFile.WriteString(szText + "\n");
			szText.Format("%f",		dDiePitchY);			kFile.WriteString(szText + "\n");
			szText.Format("%f",		dBinMapRadius);			kFile.WriteString(szText + "\n");
			szText.Format("%d",		bEnableBinMap);			kFile.WriteString(szText + "\n");
			szText.Format("%lu",	ulGradeCapacity);		kFile.WriteString(szText + "\n");
			szText.Format("%lu",	ulInputCount);			kFile.WriteString(szText + "\n");
			szText.Format("%s",		szExtraBinInfo);		kFile.WriteString(szText + "\n");
			szText.Format("%s",		szOutputFormat);		kFile.WriteString(szText + "\n");
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
				if( szText.Find("BIT list begin")!=-1 )
				{
					break;
				}
			}

			if( kFile.ReadString(szText) )
			{
				dDiePitchX = (DOUBLE) atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				dDiePitchY = (DOUBLE) atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				dBinMapRadius = atof(szText);
			}
			if( kFile.ReadString(szText) )
			{
				bEnableBinMap = atoi(szText);
			}
			if( kFile.ReadString(szText) )
			{
				ulGradeCapacity = atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				ulInputCount = atol(szText);
			}
			if( kFile.ReadString(szText) )
			{
				szExtraBinInfo = szText;
			}
			if( kFile.ReadString(szText) )
			{
				szOutputFormat = szText;
			}
			kFile.Close();

			if( dDiePitchX != m_dDiePitchX )
			{
				szText.Format("Die Pitch X changed to %f (%f)", dDiePitchX, m_dDiePitchX);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( dDiePitchY != m_dDiePitchY )
			{
				szText.Format("Die Pitch Y changed to %f (%f)", dDiePitchY, m_dDiePitchY);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( dBinMapRadius != m_dBinMapCircleRadiusInMm )
			{
				szText.Format("Map Radius changed to %f(%f)", dBinMapRadius, m_dBinMapCircleRadiusInMm);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( bEnableBinMap != m_bEnableBinMapBondArea )
			{
				szText.Format("Bin Map enable changed to %d(%d)", bEnableBinMap, m_bEnableBinMapBondArea);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( ulGradeCapacity	!= m_ulGradeCapacity )
			{
				szText.Format("Grade Capacity changed to %lu(%lu)", ulGradeCapacity, m_ulGradeCapacity);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( ulInputCount != m_ulInputCount )
			{
				szText.Format("Input Count changed to %lu(%lu)", ulInputCount, m_ulInputCount);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( szExtraBinInfo != m_szaExtraBinInfo[0] )
			{
				szText.Format("Extra Bin Info changed to %s(%s)", szExtraBinInfo, m_szaExtraBinInfo[0]);
				szListMsg = szListMsg + szText + "\n";
				bReturn = FALSE;
			}
			if( szOutputFormat != m_szBinOutputFileFormat )
			{
				szText.Format("Output Format changed to %s(%s)", szOutputFormat, m_szBinOutputFileFormat);
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
			HmiMessage(szListMsg, "BT Parameter Check");
			SetAlarmLamp_Yellow(FALSE, TRUE);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::MoveBinTableToBondPosn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bToBond = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bToBond);

	if (m_bDisableBT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//	ALREADY DISABLE JOYSTICK BEFORE CALL THIS
	if( bToBond==FALSE )
	{
		X_Sync();
		Y_Sync();
		if (m_bUseDualTablesOption)			//MS100 9Inch	//v4.24T8
		{
			X2_Sync();
			Y2_Sync();
		}
		Sleep(100);
		GetEncoderValue();

		if ( m_bUseDualTablesOption && (m_nBTInUse == 1) )	//If BT2 in use
		{
			m_lBpX = m_lEnc_X2;
			m_lBpY = m_lEnc_Y2;
		}
		else
		{
			m_lBpX = m_lEnc_X;
			m_lBpY = m_lEnc_Y;
		}

		//v4.42T7
		if (m_bUseDualTablesOption)
		{
			SetBT1FrameLevel(FALSE);
			Sleep(500);
			if (IsBT1FrameLevel() == FALSE)
			{
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}
	}
	else
	{
		if ( (m_bUseDualTablesOption) && (m_nBTInUse == 1) )	//If BT2 in use		//v4.24T8
		{
			HomeTable1();

			if (!CMS896AApp::m_bUseSlowBTControlProfile)
			{
				X2_Profile(LOW_PROF);
			}

			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			//if( pApp->GetCustomerName()==_T("Electech3E") )	// move back to last bond position table2
			//v4.49A1	//Requested by Leo
			//if ( (pApp->GetCustomerName()==_T("Electech3E")) || 
			//	 (pApp->GetCustomerName()==_T("Electech3E(DL)")) )	// move back to last bond position table2
			//{
			X2_Profile(LOW_PROF);
			Y2_Profile(LOW_PROF);
			//}
			XY2_MoveTo(m_lBpX, m_lBpY);

			X2_Profile(NORMAL_PROF);
			Y2_Profile(NORMAL_PROF);
		}
		else
		{
			if (m_bUseDualTablesOption)		//v4.24T8
			{
				HomeTable2();
			}

			if (!CMS896AApp::m_bUseSlowBTControlProfile)
			{
				X_Profile(LOW_PROF);
			}

			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			//if( pApp->GetCustomerName()==_T("Electech3E") )	// move back to last bond position
			//v4.49A1	//Requested by Leo
			//if ( (pApp->GetCustomerName()==_T("Electech3E")) || 
			//	 (pApp->GetCustomerName()==_T("Electech3E(DL)")) )	// move back to last bond position table2
			//{
			X_Profile(LOW_PROF);
			Y_Profile(LOW_PROF);
			//}
			XY_MoveTo(m_lBpX, m_lBpY);

			X_Profile(NORMAL_PROF);
			Y_Profile(NORMAL_PROF);
		}

		Sleep(100);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::MoveToBinArea4Corners(IPC_CServiceMessage& svMsg)			//v3.94T5
{
	BOOL bReturn = TRUE;
	LONG lPosn = 0;
	LONG lX=0, lY=0;

	svMsg.GetMsg(sizeof(LONG), &lPosn);

	SetJoystickOn(FALSE);
	if (m_bUseDualTablesOption)		//v4.21
		HomeTable2();

	switch (lPosn)
	{
	case 0:		//Center	//v4.08
		lX = (m_oBinBlkMain.GrabBlkUpperLeftX(1) + m_oBinBlkMain.GrabBlkLowerRightX(1)) / 2;
		lY = (m_oBinBlkMain.GrabBlkUpperLeftY(1) + m_oBinBlkMain.GrabBlkLowerRightY(1)) / 2;
		break;
	
	case 1:		//Upper-Left
		lX = m_oBinBlkMain.GrabBlkUpperLeftX(1);
		lY = m_oBinBlkMain.GrabBlkUpperLeftY(1);
		break;

	case 2:		//Upper-Right
		lX = m_oBinBlkMain.GrabBlkLowerRightX(1);
		lY = m_oBinBlkMain.GrabBlkUpperLeftY(1);
		break;

	case 3:		//Lower-Right
		lX = m_oBinBlkMain.GrabBlkLowerRightX(1);
		lY = m_oBinBlkMain.GrabBlkLowerRightY(1);
		break;

	case 4:		//Lower-Left
		lX = m_oBinBlkMain.GrabBlkUpperLeftX(1);
		lY = m_oBinBlkMain.GrabBlkLowerRightY(1);
		break;
	}

	//v4.21
	if (!IsWithinTable1Limit(ConvertFileUnitToXEncoderValue(lX), 
							 ConvertFileUnitToYEncoderValue(lY)))
	{
		CString szErr;
		szErr = "ERROR (MoveToBinArea4Corners): BT1 target position is out of table limit!";
		HmiMessage(szErr);
		SetErrorMessage(szErr);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	MoveXYTo(ConvertFileUnitToXEncoderValue(lX),
			 ConvertFileUnitToYEncoderValue(lY));
	Sleep(200);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::MoveToBinArea4Corners2(IPC_CServiceMessage& svMsg)	
{
	BOOL bReturn = TRUE;
	LONG lPosn = 0;
	LONG lX=0, lY=0;

	svMsg.GetMsg(sizeof(LONG), &lPosn);

	if (!m_bUseDualTablesOption)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);
	HomeTable1();

	switch (lPosn)
	{
	case 0:		//Center
		lX = (m_oBinBlkMain.GrabBlkUpperLeftX(1) + m_oBinBlkMain.GrabBlkLowerRightX(1)) / 2;
		lY = (m_oBinBlkMain.GrabBlkUpperLeftY(1) + m_oBinBlkMain.GrabBlkLowerRightY(1)) / 2;
		break;
	
	case 1:		//Upper-Left
		lX = m_oBinBlkMain.GrabBlkUpperLeftX(1);
		lY = m_oBinBlkMain.GrabBlkUpperLeftY(1);
		break;

	case 2:		//Upper-Right
		lX = m_oBinBlkMain.GrabBlkLowerRightX(1);
		lY = m_oBinBlkMain.GrabBlkUpperLeftY(1);
		break;

	case 3:		//Lower-Right
		lX = m_oBinBlkMain.GrabBlkLowerRightX(1);
		lY = m_oBinBlkMain.GrabBlkLowerRightY(1);
		break;

	case 4:		//Lower-Left
		lX = m_oBinBlkMain.GrabBlkUpperLeftX(1);
		lY = m_oBinBlkMain.GrabBlkLowerRightY(1);
		break;
	}

	if (!IsWithinTable2Limit(ConvertFileUnitToXEncoderValue(lX) + m_lBT2OffsetX, 
							 ConvertFileUnitToYEncoderValue(lY) + m_lBT2OffsetY))
	{
		CString szErr;
		szErr = "ERROR (MoveToBinArea4Corners2): BT2 target position is out of table limit!";
		HmiMessage(szErr);
		SetErrorMessage(szErr);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	X2_Profile(LOW_PROF1);
	Y2_Profile(LOW_PROF1);

	X2_MoveTo(ConvertFileUnitToXEncoderValue(lX) + m_lBT2OffsetX, SFM_NOWAIT);
	Y2_MoveTo(ConvertFileUnitToYEncoderValue(lY) + m_lBT2OffsetY, SFM_WAIT);
	X2_Sync();
	Sleep(200);

	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::MoveBTForAutoCColletAutoLearnZ(IPC_CServiceMessage& svMsg)	//v4.49A7	//WH SanAn
{
	BOOL bReturn = TRUE;

	BOOL bShiftY = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bShiftY);

	if (!X_IsPowerOn() || !Y_IsPowerOn())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG lX			= _round(m_oBinBlkMain.GrabDDiePitchX(1) * 4);
	LONG lY			= _round(m_oBinBlkMain.GrabDDiePitchY(1) * 4);
	LONG lEncoderX	= ConvertFileUnitToYEncoderValue(lX);
	LONG lEncoderY	= ConvertFileUnitToYEncoderValue(lY);

	LONG lBTBlkInUse	= (*m_psmfSRam)["BinTable"]["BlkInUse"];
	if (bShiftY)
	{
		if ( (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_BLH_PATH) ||
			 (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_BRH_PATH) )
		{
			//Shift UP
			Y_Move(-1 * lEncoderY);
		}
		else if ( (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_TLH_PATH) ||
				  (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_TRH_PATH) )
		{
			//Shift Y Down
			Y_Move(lEncoderY);
		}
		else if ( (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_TLV_PATH) ||
				  (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_BLV_PATH) )
		{
			//Shift X Left
			X_Move(-1 * lEncoderX);
		}
		else if ( (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_TRV_PATH) ||
				  (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_BRV_PATH) )
		{
			//Shift X Right
			X_Move(lEncoderX);
		}
	}
	else
	{
		if ( (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_BLH_PATH) ||
			 (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_BRH_PATH) )
		{
			//Shift DOWN
			Y_Move(lEncoderY);
		}
		else if ( (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_TLH_PATH) ||
				  (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_TRH_PATH) )
		{
			//Shift Y Up
			Y_Move(-1 * lEncoderY);
		}
		else if ( (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_TLV_PATH) ||
				  (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_BLV_PATH) )
		{
			//Shift X Right
			X_Move(lEncoderX);
		}
		else if ( (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_TRV_PATH) ||
				  (m_oBinBlkMain.GrabWalkPath(lBTBlkInUse) == BT_BRV_PATH) )
		{
			//Shift X Left
			X_Move(-1 * lEncoderX);
		}
	}

	if (!X_IsPowerOn() || !Y_IsPowerOn())
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::IsNeedCheckBatchIDFile(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	bReturn = IsNeedCheckBatchIDFile();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::CheckBatchIDInAutoMode(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	LPTSTR lpszMapFilename = new CHAR[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), lpszMapFilename);
	
	CString szMapFilename = &lpszMapFilename[0];
	delete [] lpszMapFilename;

	bReturn = CheckBatchIdFile(szMapFilename);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBinTable::ClearBatchIDInformation(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (CMS896AStn::m_bEnableBatchIdFileCheck == FALSE)
	{
		return TRUE;
	}

	for (INT i=0 ; i<BT_EXTRA_BIN_INFO_ITEM_NO ; i++)
	{
		m_szaExtraBinInfo[i] = "";
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::StartBTXMotionTest(IPC_CServiceMessage& svMsg)
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
		SetJoystickOn(FALSE);
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestBinTableX = stData.bIsStart;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::StartBT2XMotionTest(IPC_CServiceMessage& svMsg)
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
		SetJoystickOn(FALSE);
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(BT_AXIS_X2, &m_stBTAxis_X2, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(BT_AXIS_X2, &m_stBTAxis_X2, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestBinTableX2 = stData.bIsStart;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
	
LONG CBinTable::StartBTYMotionTest(IPC_CServiceMessage& svMsg)
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
		SetJoystickOn(FALSE);
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(BT_AXIS_Y, &m_stBTAxis_Y, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(BT_AXIS_Y, &m_stBTAxis_Y, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestBinTableY = stData.bIsStart;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::StartBT2YMotionTest(IPC_CServiceMessage& svMsg)
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
		SetJoystickOn(FALSE);
		// start from positive direction first
		m_bMoveDirection = TRUE;
		LogAxisPerformance(BT_AXIS_Y2, &m_stBTAxis_Y2, TRUE, 10, m_stDataLog);
	}
	else
	{
		LogAxisPerformance(BT_AXIS_Y2, &m_stBTAxis_Y2, FALSE, 10, m_stDataLog);
	}

	m_lMotionTestDelay = stData.lDelay;
	m_lMotionTestDist = stData.lDistance;
	m_bIsMotionTestBinTableY2 = stData.bIsStart;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
// Auto Clean Collet


LONG CBinTable::StartOfflinePostbondTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bStart = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bStart);

	//m_lStartBondIndex	= 1;	
	//m_lCurrBondIndex	= 1;
	//m_lNoOfPostBondDices= 0;
	//m_bOfflinePostBondTest = FALSE;
	SetJoystickOn(FALSE);

	if (bStart)
	{
		if (m_bOfflinePostBondTest)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		IPC_CServiceMessage stMsg;
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
		if (lBlkID <= 0)
		{
			//HmiMessage("Offline Postbond test starts FAIL!");
			//bReturn = FALSE;
			//svMsg.InitMessage(sizeof(BOOL), &bReturn);
			//return 1;
lBlkID = 1;
		}


		CTime theTime = CTime::GetCurrentTime();
		CString szTime = theTime.Format("%y%m%d%H%M%S");

		CString szWorkOrder		= (*m_psmfSRam)["MS896A"]["Bin WorkNo"];
		//if (szWorkOrder.GetLength() == 0)
			szWorkOrder = "asm";
		CString szPBTopFolder	= gszUSER_DIRECTORY + "\\OutputFile\\Postbond";
		CString szPBFolder		= gszUSER_DIRECTORY + "\\OutputFile\\Postbond\\" + szWorkOrder;
		CString szPBFile		= gszUSER_DIRECTORY + "\\OutputFile\\Postbond\\" + szWorkOrder + "\\pb_" + szTime + ".txt";

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		CreateDirectory(szPBTopFolder, NULL);
		pApp->SearchAndRemoveFiles(szPBTopFolder);
		CreateDirectory(szPBFolder, NULL);


		BOOL bPBStatus = TRUE;
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "SaveSPCDisplayData", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bPBStatus);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		if (!bPBStatus)
		{
			HmiMessage("Fail to export Postbond Runtime data!");
		}

		if (m_bUseDualTablesOption)		//v4.17T5
			HomeTable2();

		if (!m_oOfflinePostBondFile.Open(szPBFile, 
					CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
		{
			HmiMessage("Cannot open offline postbond file for writing!");
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if ( m_oBinBlkMain.GrabLSBondPattern() )	//v4.13T1	//If LSBondPattern is used
			m_oOfflinePostBondFile.WriteString("DieNo,DieNo(LS),IsGoodDie,X_mil,Y_mil,T_Degree\n");
		else
			m_oOfflinePostBondFile.WriteString("DieNo,IsGoodDie,X_mil,Y_mil,T_Degree\n");

HmiMessage("Start Offline PostBond Test ....");

		m_lCurrBlkID				= lBlkID;
		m_lCurrBondIndex			= m_lStartBondIndex;
		m_bStopOfflinePostBondTest	= FALSE;		//Reset STOP flag
		m_bOfflinePostBondTest		= TRUE;			//trigger START-TEST
	}
	else
	{
		if (!m_bOfflinePostBondTest)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		m_bStopOfflinePostBondTest = TRUE;			//Turn ON STOP flag

		//v4.40T8
		CString szWorkOrder		= (*m_psmfSRam)["MS896A"]["Bin WorkNo"];
		CString szPBFile		= gszUSER_DIRECTORY + "\\OutputFile\\Postbond\\" + szWorkOrder;
		HmiMessage("Offline PB file is created at: " + szPBFile);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

VOID CBinTable::RunOfflinePostBondTest()
{
	CString szMsg;
		
	//v4.40T8
	CString szWorkOrder	= (*m_psmfSRam)["MS896A"]["Bin WorkNo"];
	CString szPBFile	= gszUSER_DIRECTORY + "\\OutputFile\\Postbond\\" + szWorkOrder;


	ULONG ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(m_lCurrBlkID);
	if (m_lCurrBondIndex > ((LONG)ulDiePerBlk) )
	{
		m_oOfflinePostBondFile.Close();
		szMsg.Format("Offline PostBond Test done2 with bin capacity hit #%ld", ulDiePerBlk - m_lStartBondIndex);
		m_bOfflinePostBondTest = FALSE;
		HmiMessage("Offline PB file is created at: " + szPBFile);
		return;
	}

	if ( m_bStopOfflinePostBondTest )
	{
		m_oOfflinePostBondFile.Close();
		szMsg.Format("Offline PostBond Test stopped by OP with die count = #%ld", m_lCurrBondIndex-m_lStartBondIndex);
		HmiMessage(szMsg);
		m_bOfflinePostBondTest = FALSE;
		return;
	}

	//If STOP flag ON or no of dices to be postbond is hit, end the test here!
	if ( m_lCurrBondIndex >= m_lStartBondIndex + m_lNoOfPostBondDices )
	{
		m_oOfflinePostBondFile.Close();
		szMsg.Format("Offline PostBond Test done with die count = #%ld", m_lCurrBondIndex-m_lStartBondIndex);
		HmiMessage(szMsg);
		m_bOfflinePostBondTest = FALSE;
		HmiMessage("Offline PB file is created at: " + szPBFile);
		return;
	}


	//Else continue the postbond test
	//LONG x=0, y=0;
	DOUBLE dX=0, dY=0;
	ULONG ulLSBondIndex = 0;
	if ( m_oBinBlkMain.GrabLSBondPattern() )	//v4.13T1	//If LSBondPattern is used
	{
		m_oBinBlkMain.GrabLSBondPatternIndex(m_lCurrBlkID, m_lCurrBondIndex, ulLSBondIndex);	//Find LSPattern new index
		//m_oBinBlkMain.StepMove(m_lCurrBlkID, ulLSBondIndex, x, y);						//Use LSBondPattern nex index as StepMove
		m_oBinBlkMain.StepDMove(m_lCurrBlkID, ulLSBondIndex, dX, dY);						//Use LSBondPattern nex index as StepMove
		m_lCurrBondIndex++;																	//Still increment original Index
	}
	else
	{
		m_oBinBlkMain.StepDMove(m_lCurrBlkID, m_lCurrBondIndex, dX, dY);
		m_lCurrBondIndex++;
	}


#ifndef NU_MOTION
	//Position offset by "Collet Offset"
	X_MoveTo(ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX, SFM_NOWAIT);
	Y_MoveTo(ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY, SFM_WAIT);
	X_Sync();
#else		//To avoid hitting the bond head stand for MS100
	MoveXYTo(ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX, 
			 ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY);
#endif
	
	Sleep(100);


	typedef struct 
	{
	    DOUBLE	dX;
		DOUBLE	dY;
		DOUBLE	dDegree;
    	BOOL    bResult;
	
	} BPR_DIEOFFSET;

	BPR_DIEOFFSET stInfo;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);
	INT nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchDieWithAngle", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	
	//post-bond results XY in mil, T in degree
	if ( m_oBinBlkMain.GrabLSBondPattern() )	//v4.13T1	//If LSBondPattern is used
	{
		szMsg.Format("%ld,%lu,%d,%.1f,%.1f,%.2f\n", m_lCurrBondIndex-1, ulLSBondIndex-1, stInfo.bResult, stInfo.dX, stInfo.dY, stInfo.dDegree);
		m_oOfflinePostBondFile.WriteString(szMsg);
	}
	else
	{
		szMsg.Format("%ld,%d,%.1f,%.1f,%.2f\n", m_lCurrBondIndex-1, stInfo.bResult, stInfo.dX, stInfo.dY, stInfo.dDegree);
		m_oOfflinePostBondFile.WriteString(szMsg);
	}
}


LONG CBinTable::CreateBinMapFcn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	//v4.03		//PLLM/PLSG bin map fcn
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (!bEnableBinMapFcn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!m_bEnableBinMapBondArea)
	{
		HmiMessage("ERROR: Please enable BINMAP function first!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (HmiMessage("CREATE will erase current binmap; CONTINUE?", "Bin-map", glHMI_MBX_CONTINUESTOP) != glHMI_CONTINUE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("BT: Start CreateBinMap");
	if (!CreateBinMap())
	{
		HmiMessage("Error: fail to Create bin map!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::CreateBinMapFcn2(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	//v4.03		//PLLM/PLSG bin map fcn
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (!bEnableBinMapFcn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!m_bEnableBinMapBondArea)
	{
		HmiMessage_Red_Yellow("ERROR: Please enable BINMAP function first!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_dBinMapCircleRadiusInMm <= 0.0)
	{
		HmiMessage_Red_Yellow("ERROR: BOnd circle radius is invalid!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);
	Sleep(200);

	if (m_bUseDualTablesOption)		//MS109
	{
		HomeTable2();
		MoveXYTo(0, 0);
	}
	else							//MS899/MS100
	{
		LONG lX = (m_oBinBlkMain.GrabBlkUpperLeftX(1) + m_oBinBlkMain.GrabBlkLowerRightX(1)) / 2;
		LONG lY = (m_oBinBlkMain.GrabBlkUpperLeftY(1) + m_oBinBlkMain.GrabBlkLowerRightY(1)) / 2;

		if ( IsWithinTable1Limit(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY)) )
		{
			MoveXYTo(ConvertFileUnitToXEncoderValue(lX), 
					 ConvertFileUnitToYEncoderValue(lY));
		}
	}

	Sleep(500);
	SetJoystickOn(TRUE);
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001

	if (HmiMessageEx("Please move BT1 to circle center, then press CONTINUE.", "Bin-map", 
			glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 3600000, glHMI_MSG_MODAL, 0, 450, 300, 0,0,0,0,
			500, 300) != glHMI_CONTINUE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	Sleep(100);
	SetJoystickOn(FALSE);

	GetEncoderValue();
	//LONG lCXinUm = ConvertXEncoderValueForDisplay(m_lEnc_X);
	//LONG lCYinUm = ConvertYEncoderValueForDisplay(m_lEnc_Y);

	CMSLogFileUtility::Instance()->MS_LogOperation("BT: Start CreateBinMap2");
	if (!CreateBinMap2(m_lEnc_X, m_lEnc_Y, m_dBinMapCircleRadiusInMm * 1000))
	{
		HmiMessage("Error: fail to Create bin map!");
	}
	else
	{
		SaveBinTableData();
		HmiMessage("Bin map is created.");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::LoadBinMapFcn(IPC_CServiceMessage& svMsg)
{
	//HmiMessage("Load Bin Map ....");
	BOOL bReturn = TRUE;

	//v4.03		//PLLM/PLSG bin map fcn
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (!bEnableBinMapFcn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!m_bEnableBinMapBondArea)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( IfAllBinCountsAreCleared() == FALSE )
	{
		//SetAlarmLamp_Red();
		SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
		//SetAlarmLamp_Yellow();

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CreateBinMap2(0, 0, 0.00, FALSE);
	CMSLogFileUtility::Instance()->MS_LogOperation("BT: LoadBinMap");
	ULONG ulBinCount = 0;
	BOOL bLoadBinMap = LoadBinMap(FALSE, ulBinCount);
	if (!bLoadBinMap)
	{
		HmiMessage("ERROR: DEFAULT bin-map not found!  Please create and then SAVE a new DEFAULT binmap.");
	}
	else
	{
		UpdateBinMapCapacity(ulBinCount);
		BackupBinMap();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::LoadNGBinMap(IPC_CServiceMessage& svMsg)
{
	//HmiMessage("Load Bin Map ....");
	BOOL bReturn = TRUE;
	CNGGrade *pNGGrade = CNGGrade::Instance();

	//v4.03		//PLLM/PLSG bin map fcn
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (!bEnableBinMapFcn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!m_bEnableBinMapBondArea)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!CMS896AApp::m_bNGWithBinMap)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IfNGBinCountsAreCleared() == FALSE)
	{
		//SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
		HmiMessage_Red_Back("NG Block Bin Count is not cleared!", "Not Clear Bin Count");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CreateBinMap2(0, 0, 0.00, FALSE);
	CMSLogFileUtility::Instance()->MS_LogOperation("BT: LoadBinMap");
	ULONG ulBinCount = 0;
	BOOL bLoadBinMap = LoadBinMap(FALSE, ulBinCount);
	if (!bLoadBinMap)
	{
		HmiMessage("ERROR: DEFAULT bin-map not found!  Please create and then SAVE a new DEFAULT binmap.");
	}
	else
	{
		if (pNGGrade->IsHaveNGGrade())
		{
			pNGGrade->m_aulNGGradeBlockList.RemoveAll();
			pNGGrade->GetNGGradeBlockList(pNGGrade->m_aulNGGradeBlockList);
			for (int i = 0; i < pNGGrade->m_aulNGGradeBlockList.GetSize(); i++)
			{
				m_oBinBlkMain.UpdateSingleGradeCapacity(pNGGrade->m_aulNGGradeBlockList[i], TRUE);
			}
		}
		UpdateBinMapCapacity(ulBinCount);
		BackupBinMap();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::SaveBinMapFcn(IPC_CServiceMessage& svMsg)	//	Bin Map, save setting
{
	BOOL bReturn = TRUE;

	//v4.53A29	//PLLM
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (!bEnableBinMapFcn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (!m_bEnableBinMapBondArea)
	{
		//bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( IfAllBinCountsAreCleared() == FALSE )
	{
		//SetAlarmLamp_Red();
		SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
		//SetAlarmLamp_Yellow();

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_szBinMapFilePath.IsEmpty())
	{
		//Default Path setting
		m_szBinMapFilePath = gszEXE_DIRECTORY +  _T("\\Bin Map");
		CMSLogFileUtility::Instance()->MS_LogOperation("Input Path change to default path " + m_szBinMapFilePath);
		CreateDirectory(m_szBinMapFilePath, NULL);
	}

	/*if (HmiMessage("Save will overwrite current DEFAULT binmap; CONTINUE?", "Bin-map", glHMI_MBX_CONTINUESTOP) != glHMI_CONTINUE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}*/
	
	CString szBinMapFileName = m_szBinMapFilePath + _T("\\Binmap_") + pApp->GetPKGFilename() + _T(".dat");
	if (_access(szBinMapFileName, 0) != -1)
	{
		CString szMsg = "The bin map (" + szBinMapFileName + ") exists. Please delete it.";
		HmiMessage(szMsg, "Bin Map Exists");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("BT: SaveBinMap");
	ULONG ulNewInputCount = m_oBinBlkMain.GrabInputCount(1);
	if (!SaveBinMap(ulNewInputCount))
	{
		HmiMessage("Error: fail to save bin map!");
	}
	else
	{
		BackupBinMap();
	}

	UpdateBinMapCapacity(ulNewInputCount);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::SaveBinMapFcn_pkg(IPC_CServiceMessage& svMsg)	//	Save Package file
{
	BOOL bReturn = TRUE;
	/*
	//v4.53A29	//PLLM
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (!bEnableBinMapFcn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (!m_bEnableBinMapBondArea)
	{
		//bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if( pApp->GetCustomerName()=="EverVision" || pApp->GetCustomerName()=="SiLan" )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}	//	evervision, no need to update whiel saving PKG or PPKG
	*/
/*
	if ( IfAllBinCountsAreCleared() == FALSE )
	{
		//SetAlarmLamp_Red();
		SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
		//SetAlarmLamp_Yellow();

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (HmiMessage("Save will overwrite current DEFAULT binmap; CONTINUE?", "Bin-map", glHMI_MBX_CONTINUESTOP) != glHMI_CONTINUE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
*/
	/*
	CMSLogFileUtility::Instance()->MS_LogOperation("BT: SaveBinMap PKG");
	ULONG ulNewInputCount = m_oBinBlkMain.GrabInputCount(1);

	if (!SaveBinMap(ulNewInputCount))
	{
		HmiMessage("Error: fail to save bin map!");
	}

	UpdateBinMapCapacity(ulNewInputCount);
	*/
	/*
	m_oBinBlkMain.SetUseBinMapOffset(1, m_bEnableBinMapCDieOffset, 
							m_lBinMapCDieRowOffset, m_lBinMapCDieColOffset);

	//UPdate Input counts
	CString szMsg;
	szMsg.Format("PKG DEFAULT binmap updated with new bin count = %d; INPUT-COUNT will also be updated.", ulNewInputCount);
	HmiMessage(szMsg);

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	for (INT i = 1; i<= BT_MAX_BINBLK_NO; i++)
	{
		if (ulNewInputCount > m_oBinBlkMain.GrabGradeCapacity(i))
		{
			ulNewInputCount = m_oBinBlkMain.GrabGradeCapacity(i);
		}

		m_oBinBlkMain.SaveGradeInfo(FALSE, i, ulNewInputCount, 1, pBTfile);
	}

	CMSFileUtility::Instance()->SaveBTConfig();
	SaveBinTableData();
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::LoadBinMapFcn_pkg(IPC_CServiceMessage& svMsg)
{
	//HmiMessage("Load Bin Map ....");
	BOOL bReturn = TRUE;
	/*
	//v4.03		//PLLM/PLSG bin map fcn
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (!bEnableBinMapFcn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!m_bEnableBinMapBondArea)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CreateBinMap2(0, 0, 0.00, FALSE);
	CMSLogFileUtility::Instance()->MS_LogOperation("BT: LoadBinMap");
	ULONG ulBinCount = 0;
	BOOL bLoadBinMap = LoadBinMap(FALSE, ulBinCount);
	if (!bLoadBinMap)
	{
		HmiMessage("ERROR: PKG bin-map not found!  Please create and then SAVE a new PKG binmap.");
	}
	else
	{
		UpdateBinMapCapacity(ulBinCount);
		BackupBinMap();
	}
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::EnableBinMapFcn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);
	m_bEnableBinMapBondArea = bEnable;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( m_bEnableBinMapBondArea && (IfAllBinCountsAreCleared() == FALSE) )
	{
		//SetAlarmLamp_Red();
		SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);
		//SetAlarmLamp_Yellow();

		m_bEnableBinMapBondArea = FALSE;

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_oBinBlkMain.SetUseBinMapBondArea(m_bEnableBinMapBondArea);						//v4.03		//PLLM Bin map fcn

	if (m_bEnableBinMapBondArea)
	{
		IPC_CServiceMessage stMsg;
		CreateBinMapFcn2(stMsg);

		//v4.06		//removed for PLSG testing on MS899DLA with 3pt alignment
		//v4.04
		//if (m_lRealignBinFrameOption != 1)		//if not 1-pr alignment
		//{
		//	m_lRealignBinFrameOption = 1;
		//	HmiMessage("Warning: Bin-Map fcn automatically switches to 1-pt bin-frame alignment method.");
		//}
	}
	else
	{
		if ((pApp->GetCustomerName() == CTM_NICHIA) &&	//v4.42T12
			(pApp->GetProductLine() == ""))				//v4.59A34
		{
		}
		else
		{
			m_BinMapWrapper.InitMap();
			HmiMessage("BinMap fcn is disabled; please re-teach bin block setup.");
		}
	}

	SaveBinTableData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::SetSpGradeToByPassBinMap(IPC_CServiceMessage& svMsg)	//v4.48A4
{
	BOOL bReturn  = TRUE;
	CString szErr;

	UCHAR ucGrade = 0;
	svMsg.GetMsg(sizeof(UCHAR),	&ucGrade);

	if (m_oBinBlkMain.GrabNVNoOfBondedDie(ucGrade) > 0)
	{
		szErr.Format("Block #%d is not cleared; please clear bin counter and try again.", ucGrade);
		HmiMessage_Red_Yellow(szErr);
		m_ucSpGrade1ToByPassBinMap = m_oBinBlkMain.GetSpGradeToByPassBinMap(ucGrade);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	szErr.Format("Grade #%d is set to Special Grade in BinMap.", ucGrade);
	HmiMessage(szErr);

	m_ucSpGrade1ToByPassBinMap = ucGrade;
	m_oBinBlkMain.SetSpGradeToByPassBinMap(1, m_ucSpGrade1ToByPassBinMap);
	SaveBinTableData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::EnableOsramBinMixMap(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	BOOL bEnable = TRUE;
	svMsg.GetMsg(sizeof(BOOL),	&bEnable);
	m_bEnableOsramBinMixMap = bEnable;

	CString szTemp;
	szTemp.Format("EnableOsramBinMixMap: Enable = %d, Pattern = %d, TypeA = %d, TypeB = %d",
					m_bEnableOsramBinMixMap, m_ulBinMixPatternType, 
					m_ucBinMixTypeAGrade, m_ucBinMixTypeBGrade);
	HmiMessage(szTemp);
	SaveBinTableData();
	CreateOsramBinMixMap(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::CreateOsramBinMixMap(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	m_lBinMixSetQty = 0;//stop point//stop in the cycle and stop after cycle//benableosrambinmixmap
	m_bBinMixOrder = 0;// grade a grade b
	m_lBinMixCount = 0;// counter
	if (!m_bEnableOsramBinMixMap)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szTemp;
	szTemp.Format("CreateOsramBinMixMap: Enable = %d, Pattern = %d, TypeA = %d, TypeB = %d",
					m_bEnableOsramBinMixMap, m_ulBinMixPatternType, 
					m_ucBinMixTypeAGrade, m_ucBinMixTypeBGrade);
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
	HmiMessage(szTemp);

	CreateOsramBinMixMap(TRUE);
	SaveBinTableData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::SaveOsramBinMixMap(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bEnableOsramBinMixMap)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( IfAllBinCountsAreCleared() == FALSE )
	{
		SetAlert_Red_Yellow(IDS_BT_BIN_NOT_CLEARED);

		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (HmiMessage("Save will overwrite current DEFAULT binmap; CONTINUE?", "Bin-map", glHMI_MBX_CONTINUESTOP) != glHMI_CONTINUE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("BT: SaveOsramBinMixMap");
	ULONG ulNewInputCount = m_oBinBlkMain.GrabInputCount(1);
	if (!SaveOsramBinMixMap(ulNewInputCount))
	{
		HmiMessage_Red_Yellow("Error: fail to save OSRAM bin map!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//UPdate Input counts
	CString szMsg;
	CString szSelection1 = "Yes";
	CString szSelection2 = "No";
	szMsg.Format("OSRAM binmap updated with new bin count = %d; INPUT-COUNT will also be updated.", ulNewInputCount);
	HmiMessage(szMsg);

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	for (INT i = 1; i<= BT_MAX_BINBLK_NO; i++)
	{
		if (ulNewInputCount > m_oBinBlkMain.GrabGradeCapacity(i))
		{
			ulNewInputCount = m_oBinBlkMain.GrabGradeCapacity(i);
		}
		m_oBinBlkMain.SaveGradeInfo(FALSE, i, ulNewInputCount, 1, pBTfile);
	}
/*
	IPC_CServiceMessage rReqMsg;
	BOOL bResult;

	int nSelection = HmiMessage("Need To Load A New Bin Frame?", "BINMIX", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);
	if (nSelection == 1)
	{
		int nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "ManualLoadFilmFrame", rReqMsg);
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
	}
*/
	CMSFileUtility::Instance()->SaveBTConfig();
	SaveBinTableData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::ResetBinStatus(IPC_CServiceMessage &svMsg)
{
	for (int i=1; i<=200; i++)
	{
		m_bBinStatus[i] = FALSE;
	}

	HmiMessage("All bins clear status reset.");

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CBinTable::QueryBinStatus(IPC_CServiceMessage &svMsg)
{
	LONG lBin = 1;
	svMsg.GetMsg(sizeof(LONG), &lBin);

	lBin = min(lBin, 200);
	BOOL bReturn = m_bBinStatus[lBin];
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBinTable::UpdateBinStatus(IPC_CServiceMessage &svMsg)
{
	ULONG i;
	for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
	{
		i = min(i, 250);		//Klocwork	//v4.08

		if ( m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0 )
		{
			m_bBinStatus[i] = TRUE;
		}
		else
		{
			m_bBinStatus[i] = FALSE;
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}	// update hmi display


LONG CBinTable::CheckBinEmptyStatus(IPC_CServiceMessage &svMsg)
{
	m_bIsAllBinsEmptyStatus = IfAllBinCountsAreCleared();

	//FALSE -> has die counts
	//TRUE  -> all EMPTY!!

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::FATC_CheckBinEmptyStatus(IPC_CServiceMessage &svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="FATC" )
		m_bIsAllBinsEmptyStatus = TRUE;
	//FALSE -> has die counts
	//TRUE  -> all EMPTY!!

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::ManualCleanCollet_PLLM_MS109(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	BOOL bEnable = TRUE;
	svMsg.GetMsg(sizeof(BOOL),	&bEnable);

	if (!m_fHardware)
	{	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if (!m_bUseDualTablesOption)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (bEnable)
	{
		//Change Collet
		HomeTable1();
		HomeTable2();
	}
	else
	{
		//ChangeCollet done!
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::GetInputCountSetupFilePath(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szInputCountSetupFilePath);
		bReturn = TRUE;
		SaveBinTableData();
		SaveBinTableSetupData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::LoadInputCountSetupFile(IPC_CServiceMessage& svMsg)
{
	CString szTitle, szContent;
	CInputCountSetupFile oCountSetupFile;
	BOOL bReturn = TRUE;
	
	static char szFilters[]=
				"All Files (*.*)|*.*||";

	// Create an Open dialog; the default file name extension is ".pkg".
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , szFilters, m_pMainWnd, 0);
	dlgFile.m_ofn.lpstrInitialDir = m_szInputCountSetupFilePath;
	dlgFile.m_ofn.lpstrDefExt = "csv";
	
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}
	
	szTitle.LoadString(HMB_BT_INPUT_COUNT_SETUP_FILE);

	if ( nReturn == IDOK )
	{
		SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
		
		m_oBinBlkMain.SetAllGradeIsAssigned(FALSE);

		if (oCountSetupFile.ReadFile(dlgFile.GetPathName()) == FALSE)
		{
			szContent.LoadString(HMB_BT_LOAD_INPUT_COUNT_SETUP_FILE_FAIL);
			HmiMessage(szContent, szTitle);
			SaveBinTableData();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		
		if (UpdateInputCountFromSetupFile(oCountSetupFile) == FALSE)
		{
			szContent.LoadString(HMB_BT_UPDATE_INPUT_COUNT_FAIL);
			HmiMessage(szContent, szTitle);
			SaveBinTableData();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		m_szInputCountSetupFilename = dlgFile.GetFileName();

		szContent.LoadString(HMB_BT_INPUT_COUNT_SETUP_FILE_LAOD_COMPLETE);
		HmiMessage(szContent, szTitle);
		SaveBinTableData();
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//v4.43T8
BOOL CBinTable::CopyMagazineSummary()	//for Epigap/Jenoptic
{
	CTime theTime = CTime::GetCurrentTime();
	INT nYear, nMonth, nDay, nHour, nMinute, nSecond;
	CString szYear, szMonth, szDay, szHour, szMinute, szSecond;
	CString szMachineNo, szTargetFilename;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetCustomerName() != "Jenoptic")	&& 
		 (pApp->GetCustomerName() != "Epigap")		&& 
		 (pApp->GetCustomerName() != "Minicircuit") )
	{
		return TRUE;
	}

	if (_access(BT_MAG_SUMMARY_TEMP_PATH, 0) == -1)
	{
		return FALSE;
	}

	nYear	= theTime.GetYear();
	nMonth	= theTime.GetMonth();
	nDay	= theTime.GetDay();
	nHour	= theTime.GetHour();
	nMinute	= theTime.GetMinute();
	nSecond	= theTime.GetSecond();
	
	szYear.Format("%d", nYear);
	szYear = szYear.Right(2);

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

	szMachineNo = GetMachineNo();

	//szTargetFilename = m_szOutputFilePath + "\\" + 
	//					"M" + szMachineNo + "_" + 
	//					szDay + szMonth + szYear + "_" + 
	//					szHour + szMinute + szSecond + ".CSV";
	szTargetFilename = m_szOutputFilePath + "\\" + 
						"BL_Mag_Sum_" + szYear + szMonth + szDay + ".csv";

	//v3.91T24	//Jemoptik
	BOOL bCopyFile = TRUE;
	CTime tCurrTime = CTime::GetCurrentTime();
	if (m_lMagSummaryYear == 0)
	{
		m_lMagSummaryYear	= tCurrTime.GetYear();
		m_lMagSummaryMonth	= tCurrTime.GetMonth();
		m_lMagSummaryDay	= tCurrTime.GetDay();
		SaveBinTableData();
		bCopyFile = TRUE;
	}
	else
	{
		CTime tTime(m_lMagSummaryYear, m_lMagSummaryMonth, m_lMagSummaryDay, 0, 0, 0);
		CTimeSpan cTimeDiff = tCurrTime - tTime;

		if (cTimeDiff.GetDays() < 1)
		{
			bCopyFile = FALSE;
		}
		else	//> 24 hrs
		{
			m_lMagSummaryYear	= tCurrTime.GetYear();
			m_lMagSummaryMonth	= tCurrTime.GetMonth();
			m_lMagSummaryDay	= tCurrTime.GetDay();
			SaveBinTableData();
			bCopyFile = TRUE;
		}
	}

	if (bCopyFile)
	{
		if (_access(szTargetFilename, 0) == -1)
		{
			if (CopyFile(BT_MAG_SUMMARY_TEMP_PATH, szTargetFilename, FALSE) == TRUE)
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("Jenoptic: BL Summary is generated at: " + szTargetFilename);
				DeleteFile(BT_MAG_SUMMARY_TEMP_PATH);
			}
		}
		else
		{
			SetErrorMessage("Jenoptic: duplicated BL Summary found at: " + szTargetFilename);
		}
	}

	return TRUE;
}


LONG CBinTable::OsramCalculateFunction(IPC_CServiceMessage& svMsg)
{
	//m_lBinMixTypeAQty;
	//m_lBinMixTypeBQty;
	//m_lBinMixPatternQty;
	//m_ulBinMixPatternType;
	m_lBinMixCountTemp = 0; //reset Bin Mix Count Temp (used to calculate more common type's stop point) to 0.
	m_bBinMixOrder     = 0;
	m_lBinMixCount     = 0;
	m_lBinMixSetQty	   = 0;
	LONG lTemp1;
	LONG lTemp2;
	switch (m_ulBinMixPatternType)
	{
	case 1:		//ABAB
		lTemp1 = m_lBinMixTypeAQty;
		lTemp2 = m_lBinMixTypeBQty;
		if (lTemp1 < lTemp2)
		{
			HmiMessage("Type A is less common type");
			m_lBinMixPatternQty = lTemp1;
			m_lBinMixLessCommonType = 1;
		}
		else
		{
			HmiMessage("Type B is less common type");
			m_lBinMixPatternQty = lTemp2;
			m_lBinMixLessCommonType = 2;
		}

//HmiMessage("BinBix Pattern = ABAB");
		break;
	case 2:		//AABAAB
		lTemp1 = m_lBinMixTypeAQty / 2;
		lTemp2 = m_lBinMixTypeBQty;
		if (lTemp1 < lTemp2)
		{
			HmiMessage("Type A is less common type");
			m_lBinMixPatternQty = lTemp1;
			m_lBinMixLessCommonType = 1;
		}
		else
		{
			HmiMessage("Type B is less common type");
			m_lBinMixPatternQty = lTemp2;
			m_lBinMixLessCommonType = 2;
		}

//HmiMessage("BinBix Pattern = AABAAB");
		break;
	case 3:		//AAABAAAB
		lTemp1 = m_lBinMixTypeAQty / 3;
		lTemp2 = m_lBinMixTypeBQty;
		if (lTemp1 < lTemp2)
		{
			HmiMessage("Type A is less common type");
			m_lBinMixPatternQty = lTemp1;
			m_lBinMixLessCommonType = 1;
		}
		else
		{
			HmiMessage("Type B is less common type");
			m_lBinMixPatternQty = lTemp2;
			m_lBinMixLessCommonType = 2;
		}

//HmiMessage("BinBix Pattern = AAABAAAB");
		break;
	case 4:		//AAAABAAAAB
		lTemp1 = m_lBinMixTypeAQty / 4;
		lTemp2 = m_lBinMixTypeBQty;
		if (lTemp1 < lTemp2)
		{
			HmiMessage("Type A is less common type");
			m_lBinMixPatternQty = lTemp1;
			m_lBinMixLessCommonType = 1;
		}
		else
		{
			HmiMessage("Type B is less common type");
			m_lBinMixPatternQty = lTemp2;
			m_lBinMixLessCommonType = 2;
		}
//HmiMessage("BinBix Pattern = AAAABAAAAB");
		break;
	case 5:		//AABBAABB
		lTemp1 = m_lBinMixTypeAQty / 2;
		lTemp2 = m_lBinMixTypeBQty / 2;
		if (lTemp1 < lTemp2)
		{
			HmiMessage("Type A is less common type");
			m_lBinMixPatternQty = lTemp1;
			m_lBinMixLessCommonType = 1;
		}
		else
		{
			HmiMessage("Type B is less common type");
			m_lBinMixPatternQty = lTemp2;
			m_lBinMixLessCommonType = 2;
		}

//HmiMessage("BinBix Pattern = AABBAABB");
		break;
	case 6:		//ABBABB
		lTemp1 = m_lBinMixTypeAQty;
		lTemp2 = m_lBinMixTypeBQty / 2;
		if (lTemp1 < lTemp2)
		{
			HmiMessage("Type A is less common type");
			m_lBinMixPatternQty = lTemp1;
			m_lBinMixLessCommonType = 1;
		}
		else
		{
			HmiMessage("Type B is less common type");
			m_lBinMixPatternQty = lTemp2;
			m_lBinMixLessCommonType = 2;
		}

//HmiMessage("BinBix Pattern = ABBABB");
		break;
	case 7:		//ABBBABBB
		lTemp1 = m_lBinMixTypeAQty ;
		lTemp2 = m_lBinMixTypeBQty / 3;
		if (lTemp1 < lTemp2)
		{
			HmiMessage("Type A is less common type");
			m_lBinMixPatternQty = lTemp1;
			m_lBinMixLessCommonType = 1;
		}
		else
		{
			HmiMessage("Type B is less common type");
			m_lBinMixPatternQty = lTemp2;
			m_lBinMixLessCommonType = 2;
		}

//HmiMessage("BinBix Pattern = ABBBABBB");
		break;
	case 8:		//ABBBBABBBB
		lTemp1 = m_lBinMixTypeAQty ;
		lTemp2 = m_lBinMixTypeBQty / 4;
		if (lTemp1 < lTemp2)
		{
			HmiMessage("Type A is less common type");
			m_lBinMixPatternQty = lTemp1;
			m_lBinMixLessCommonType = 1;
		}
		else
		{
			HmiMessage("Type B is less common type");
			m_lBinMixPatternQty = lTemp2;
			m_lBinMixLessCommonType = 2;
		}
//HmiMessage("BinBix Pattern = ABBBBABBBB");
		break;
	default:
		HmiMessage("BinBix Pattern = NONE");
		//return FALSE;
	}


	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::MoveToCORCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableBT || !m_bEnable_T)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	MoveToCOR("MoveToCORCmd");

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if (!X_IsPowerOn() || !Y_IsPowerOn())
	{
		SetErrorMessage("BT: COR error");
		SetAlert_Red_Yellow(IDS_BT_POS_OUT_LIMIT);
		bReturn = FALSE;
	}

	Sleep(200);
	SetJoystickOn(TRUE);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::TeachCalibration(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bDisableBT || !m_bEnable_T)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

//HmiMessage("BT: Start calibration ....");
	StartBinCalibration();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::StopCalibration(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bOK = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOK);

	if (m_bDisableBT || !m_bEnable_T)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	StopBinCalibration((LONG) bOK);
	SetStatusMessage("Bin calibration is learnt");

	T_MoveTo(0, SFM_WAIT);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//v4.59A10
LONG CBinTable::UpdateCOR(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lX;
		LONG	lY;	
	} BLKCOR;
	BLKCOR stCOR;

	svMsg.GetMsg(sizeof(BLKCOR), &stCOR);

	m_lBinCalibX	= stCOR.lX;		//in motor steps
	m_lBinCalibY	= stCOR.lY;		//in motor steps

CString szMsg;
szMsg.Format("Bin COR updated; X = %ld, Y = %ld", m_lBinCalibX, m_lBinCalibY);
HmiMessage(szMsg);

	SaveBinTableSetupData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::StartBinCalibration(VOID)
{
	//Off joystick before move
	SetJoystickOn(FALSE);

	//Move to Wafer Center position
	T_MoveTo(0, SFM_WAIT);
	
	if (m_bUseDualTablesOption)
		HomeTable2();

	INT nMove = 0;
	LONG lX=0, lY=0;
	LONG lEncX=0, lEncY=0;

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	if ( (m_lBinCalibX != 0) && (m_lBinCalibY != 0) )
	{
		lX = m_lBinCalibX;		//in motor steps
		lY = m_lBinCalibY;		//in motor steps
		nMove = XY_MoveTo(lX, lY);
	}
	else
	{
		//in um
		lX = (m_oBinBlkMain.GrabBlkUpperLeftX(1) + m_oBinBlkMain.GrabBlkLowerRightX(1)) / 2;
		lY = (m_oBinBlkMain.GrabBlkUpperLeftY(1) + m_oBinBlkMain.GrabBlkLowerRightY(1)) / 2;
		lEncX = ConvertFileUnitToXEncoderValue(lX);
		lEncY = ConvertFileUnitToXEncoderValue(lY);

CString szMsg;
szMsg.Format("BT COR X = %ld, Y = %ld", lEncX, lEncY);
HmiMessage(szMsg);

		//Change to motor steps
		if (IsWithinTable1Limit(lEncX, lEncY))
		{
			nMove = XY_MoveTo(lEncX, lEncY);
		}
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	
	//On joystick
	//m_bCheckBinLimit = FALSE;	
	Sleep(500);
	SetJoystickOn(TRUE);
	m_bStartCalibrate = TRUE;

	return TRUE;
}

LONG CBinTable::StopBinCalibration(LONG lStop)
{
	m_bStartCalibrate = FALSE;
	T_MoveTo(0, SFM_WAIT);
	Sleep(500);

	if (lStop == 1)
	{
		GetEncoderValue();
		m_lBinCalibX = m_lEnc_X;
		m_lBinCalibY = m_lEnc_Y;

		//Change from motor steps to um
		LONG lCORX = -1 * ConvertXEncoderValueForDisplay(m_lBinCalibX);
		LONG lCORY = -1 * ConvertYEncoderValueForDisplay(m_lBinCalibY);

		//v4.59A10
CString szMsg;
szMsg.Format("Bin Calibration done; XY(steps) = (%ld, %ld), XY(um) = (%ld, %ld)", 
				m_lBinCalibX, m_lBinCalibY, lCORX, lCORY);
HmiMessage(szMsg);

		SaveBinTableSetupData();

		//m_bCheckWaferLimit = TRUE;	
		SetJoystickOn(FALSE);
		if (m_bUseDualTablesOption == TRUE)
		{
			HomeTable1();
		}
	}

	return TRUE;
}

LONG CBinTable::MoveWaferTheta(VOID)
{
	//T_Profile(LOW_PROF);	
	if (m_bDisableBT || !m_bEnable_T)
		return TRUE;
	if (!T_IsPowerOn())
	{
		m_bStartCalibrate = FALSE;
		return FALSE;
	}

	T_Profile(LOW_PROF);
	T_MoveTo(-_round(180.0/m_dThetaRes), SFM_WAIT);
	Sleep(500);
	T_MoveTo(_round(180.0/m_dThetaRes),	SFM_WAIT);
	Sleep(500);

	return TRUE;
}

BOOL CBinTable::BT_SetFrameRotation(ULONG ulBlkId, BOOL bEnable180Rotation, BOOL bMoveToOrgPosn)
{
	if (IsMS90())
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("BT_SetFrameRotation 1", "a+");

		//Change from motor steps to um
		LONG lCORX = -1 * ConvertXEncoderValueForDisplay(m_lBinCalibX);
		LONG lCORY = -1 * ConvertYEncoderValueForDisplay(m_lBinCalibY);
		LONG lX,lY;
		BOOL bIsMove = FALSE;
		GetXYEncoderValue(lX, lY);

		LONG lEnc_T = GetTEncoderValue(BT_AXIS_T);
		INT nPos = 0;
		CString szTemp;
		if (bEnable180Rotation)
		{
			//T_MoveTo(23198);	//180 degree	//v4.49A11
			//v4.59A31
			nPos = (INT) (180.0 / m_dThetaRes);
			szTemp.Format("BT_SetFrameRotation 180 degree - %d",  nPos);
		}
		else
		{
			szTemp.Format("BT_SetFrameRotation 0 degree - %d",  nPos);
		}
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szTemp, "a+");

		if (labs(lEnc_T - nPos) > 50)
		{
			if ((m_lBinCalibX != 0) && (m_lBinCalibY != 0) && IsMS90())
			{
				if (IsWithinTable1Limit(m_lBinCalibX, m_lBinCalibY))
				{
					MoveToCOR("BT: Move to COR5");
					bIsMove = TRUE;
				}
			}

			T_Profile(NORMAL_PROF);
			LONG lRet = T_MoveTo(nPos, SFM_WAIT); 		//180 degree
//===============================2018.5.4=======================
			T_Profile(LOW_PROF);
//			T_Home(bEnable180Rotation);
//===============================================================

			if (lRet != gnOK)
			{
				return FALSE;
			}
		}
		
		if (bIsMove && bMoveToOrgPosn)
		{
				XY_MoveTo(lX,lY);
		}

		CString szMsg;
		szMsg.Format("BT - rotate bin frame %d, Wafer at second part %d",
			bEnable180Rotation, IsMS90Sorting2ndPart());
		DisplaySequence(szMsg);
		m_bMS90RotatedBT = bEnable180Rotation;
		m_oBinBlkMain.SetFrameRotation(ulBlkId, bEnable180Rotation, lCORX, lCORY);

		(*m_psmfSRam)["BinTable"]["MS90"]["Rotate180"] = bEnable180Rotation;	//v4.58A1
	}

	return TRUE;
}

BOOL CBinTable::RotateBinTo180(LONG lOldX, LONG lOldY, LONG &lNewX, LONG &lNewY)
{
	lNewX = lOldX;
	lNewY = lOldY;

	if( IsMS90() )
	{
		BOOL bIs180Rotation = IsMS90BTRotated();
		if ( (m_lBinCalibX != 0) && (m_lBinCalibY != 0) && bIs180Rotation )
		{
			lNewX = 2 * m_lBinCalibX - lOldX;
			lNewY = 2 * m_lBinCalibY - lOldY;
		}

		CString szMsg;
		szMsg.Format("BT Convert Curr (%ld, %ld) on BT Org (%ld, %ld)", lOldX, lOldY, lNewX, lNewY);
		DisplaySequence(szMsg);
	}

	return TRUE;
}

BOOL CBinTable::RotateBinToOrg(LONG lOldX, LONG lOldY, LONG &lNewX, LONG &lNewY)
{
	lNewX = lOldX;
	lNewY = lOldY;

	if ( IsMS90() )
	{
		BOOL bIs180Rotation = IsMS90BTRotated();
		if ( (m_lBinCalibX != 0) && (m_lBinCalibY != 0) && bIs180Rotation )
		{
			lNewX = 2 * m_lBinCalibX - lOldX;
			lNewY = 2 * m_lBinCalibY - lOldY;
		}

		CString szMsg;
		szMsg.Format("BT Convert Curr (%ld, %ld) on BT Org (%ld, %ld)", lOldX, lOldY, lNewX, lNewY);
		DisplaySequence(szMsg);
	}

	return TRUE;
}

VOID CBinTable::OnBinBlkSetupNo(ULONG ulBinBlkId)
{
	ULONG i;
	BOOL bIfBondingInProcess, bIfIsFirstBlkInMerge;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Genesis")
	{
		LONG lNoOfBlock = (*m_psmfSRam)["MS896A"]["AmiFile"]["NoOfBlock"];
		BOOL bChangeMaxCountWithAmi = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["AmiFile"]["ChangeMaxCountWithAmi"];
		if (m_ulNoOfBinBlk != lNoOfBlock && bChangeMaxCountWithAmi && lNoOfBlock != 0)
			m_ulNoOfBinBlk = lNoOfBlock;
		CString szMsg;
		szMsg.Format("OnBinBlkSetupMode:m_ulNoOfBinBlk:%d",m_ulNoOfBinBlk);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg); 
	}
	if (ulBinBlkId == 0)
	{
		m_bIfEnableTBSetUL = FALSE;
		m_bIfEnableTBSetLR = FALSE;
		m_bIfEnablePIBlkPitchX = FALSE;
		m_bIfEnablePIBlkPitchY = FALSE;
		m_bIfEnablePIDiePitchX = FALSE;
		m_bIfEnablePIDiePitchY = FALSE;
		m_bIfEnablePIDiePerRow = FALSE;
		m_bIfEnablePIDiePerCol = FALSE;
		m_bIfEnablePIDiePerBlk = FALSE;
		m_bIfEnableChkBAutoAssignGrade = FALSE;
		m_bIfEnablePIGrade		= FALSE;
		m_bIfEnableCBWalkPath	= FALSE;
		m_bIfEnableFirstRowColSkipPattern = FALSE;
		m_bIfEnableTeachWithPhysicalBlk = FALSE;
		m_bIfEnable2DBarcodeOutput = FALSE;
		m_bIfEnableTemplateSetup = FALSE;
		m_bIfEnableCentralizedBondArea = FALSE;
		m_bIfEnableWafflePad = FALSE;
		m_bIfEnableWafflePadCheckBox = FALSE;
		m_bIfEnablePadPitchSetX = FALSE;
		m_bIfEnablePadPitchSetY = FALSE;
		m_bIfEnablePadPitchResetX = FALSE;
		m_bIfEnablePadPitchResetY = FALSE;
		m_bIfEnablePadDimX = FALSE;
		m_bIfEnablePadDimY = FALSE;
		m_bIfUseBlockCornerAsFirstDiePos = FALSE;
		m_bIfEnableTBSubmit = FALSE;
		m_bIfEnableTBCancel	= FALSE;

		m_lBinUpperLeftX = 0;	
		m_lBinUpperLeftY = 0;
		m_lBinLowerRightX = 0;
		m_lBinLowerRightY = 0;

		m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
		m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

		m_dDiePitchX = 500;
		m_dDiePitchY = 500;
		m_bUsePt5UmInDiePitchX	= FALSE;
		m_bUsePt5UmInDiePitchY	= FALSE;
		m_dBondAreaOffsetXInUm	= 0;
		m_dBondAreaOffsetYInUm	= 0;
		m_bByPassBinMap			= FALSE;

		m_ulDiePerRow = 0;
		m_ulDiePerCol = 0;
		m_ulDiePerBlk = 0;

		m_bEnableFirstRowColSkipPattern = FALSE;
		m_ulFirstRowColSkipUnit = 0;
		m_bTeachWithPhysicalBlk = FALSE;

		//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
		m_bEnableCircularBondArea	= FALSE;
		m_lCirAreaCenterX			= 0;
		m_lCirAreaCenterY			= 0;
		m_dCirAreaRadius			= 0;

		m_bCentralizedBondArea = FALSE;
		m_lCentralizedUpperLeftX = 0;
		m_lCentralizedUpperLeftY = 0;

		m_bEnableWafflePad = FALSE;
		m_lWafflePadDistX = 0;
		m_lWafflePadDistY = 0;
		m_lWafflePadSizeX = 0;
		m_lWafflePadSizeY = 0;

		m_bUseBlockCornerAsFirstDiePos = FALSE;
		m_bNoReturnTravel = FALSE;

		m_ucGrade = 1;
		m_szWalkPath = BT_TLH_NAME;
		m_ulSourcePhyBlk = 1;

		//Error: Block to Setup must be greater than 0!
		SetAlert(IDS_BT_BLKTOSETUPCHK);	
	}
	else
	{
		//Display "Blk to Setup" data
		m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(ulBinBlkId));	
		m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(ulBinBlkId));
		m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(ulBinBlkId));
		m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(ulBinBlkId));

		m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
		m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

		m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(ulBinBlkId));
		m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(ulBinBlkId));
		m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(ulBinBlkId);
		m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(ulBinBlkId);
		m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(ulBinBlkId);
		m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(ulBinBlkId);
		m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(ulBinBlkId);	//v4.59A22	//David Ma
		m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(ulBinBlkId);	//v4.59A22	//David Ma
		m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(ulBinBlkId);

		m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBinBlkId);
		m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(ulBinBlkId);
		m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(ulBinBlkId);
		m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(ulBinBlkId); 	 
		m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(ulBinBlkId);
		m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(ulBinBlkId);
		m_ulFirstRowColSkipUnit = m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(ulBinBlkId);
		m_bTeachWithPhysicalBlk = m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(ulBinBlkId); 

		//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
		m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(ulBinBlkId);
		m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(ulBinBlkId);
		m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(ulBinBlkId);
		DOUBLE dRadius	= (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(ulBinBlkId));		// from encoder step to um
		m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 

		m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(ulBinBlkId);
		if (m_bCentralizedBondArea)
		{
			m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(ulBinBlkId);
			m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(ulBinBlkId);
			m_lCentralizedUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(ulBinBlkId) + m_lCentralizedOffsetX);
			m_lCentralizedUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(ulBinBlkId) + m_lCentralizedOffsetY);
		}
		else
		{
			m_lCentralizedUpperLeftX = 0;
			m_lCentralizedUpperLeftY = 0;
		}

		m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(ulBinBlkId);
		if (m_bEnableWafflePad)
		{
			m_lWafflePadDistX =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(ulBinBlkId));
			m_lWafflePadDistY =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(ulBinBlkId));
			m_lWafflePadSizeX = m_oBinBlkMain.GrabWafflePadSizeX(ulBinBlkId);
			m_lWafflePadSizeY = m_oBinBlkMain.GrabWafflePadSizeY(ulBinBlkId);
		}
		else
		{
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;
			m_lWafflePadSizeX = 0;
			m_lWafflePadSizeY = 0;
		}

		m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(ulBinBlkId);
		m_bNoReturnTravel	= m_oBinBlkMain.GrabNoReturnTravel(ulBinBlkId);

		m_ucGrade = m_oBinBlkMain.GrabGrade(ulBinBlkId);

		if (m_oBinBlkMain.GrabIsSetup(ulBinBlkId))
			m_ulSourcePhyBlk = m_oBinBlkMain.GrabPhyBlkId(ulBinBlkId);
		else
			m_ulSourcePhyBlk = 1;

		m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(ulBinBlkId));

		if (m_szSetupMode == "All Blocks")
		{
			if (ulBinBlkId == 1)
			{
				bIfBondingInProcess = FALSE;

				for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
				{
					if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
					{
						bIfBondingInProcess = TRUE;
						break;
					}
				}

				if (bIfBondingInProcess)
				{
					m_bIfEnableTBSetUL = FALSE;
					m_bIfEnableTBSetLR = FALSE;
					m_bIfEnablePIBlkPitchX = FALSE;
					m_bIfEnablePIBlkPitchY = FALSE;
					m_bIfEnablePIDiePitchX = FALSE;
					m_bIfEnablePIDiePitchY = FALSE;
					m_bIfEnablePIDiePerRow = FALSE;
					m_bIfEnablePIDiePerCol = FALSE;
					m_bIfEnablePIDiePerBlk = FALSE;
					m_bIfEnableChkBAutoAssignGrade = FALSE;
					m_bIfEnablePIGrade = FALSE;
					m_bIfEnableCBWalkPath = FALSE;
					m_bIfEnableFirstRowColSkipPattern = FALSE;
					m_bIfEnableTeachWithPhysicalBlk = FALSE;
					m_bIfEnable2DBarcodeOutput = FALSE;
					m_bIfEnableTemplateSetup = FALSE;
					m_bIfEnableCentralizedBondArea = FALSE;
					m_bIfEnableWafflePadCheckBox = FALSE;
					m_bIfEnablePadPitchSetX = FALSE;
					m_bIfEnablePadPitchSetY = FALSE;
					m_bIfEnablePadPitchResetX = FALSE;
					m_bIfEnablePadPitchResetY = FALSE;
					m_bIfEnablePadDimX = FALSE;
					m_bIfEnablePadDimY = FALSE;
					m_bIfEnableWafflePad = FALSE;
					m_bIfUseBlockCornerAsFirstDiePos = FALSE;
					m_bIfEnableTBSubmit = FALSE;
					m_bIfEnableTBCancel = FALSE;	

					//Block settings can't be edited because it contains bin blocks.
					SetAlert(IDS_BT_DENYBLKSETTINGSEDIT1);
				}
				else
				{
					bIfIsFirstBlkInMerge =	(m_oBinBlkMain.GrabBlkUpperLeftX(1) != m_oBinBlkMain.GrabBlkUpperLeftXFromSetup(1)) ||
											(m_oBinBlkMain.GrabBlkUpperLeftY(1) != m_oBinBlkMain.GrabBlkUpperLeftYFromSetup(1)) ||
											(m_oBinBlkMain.GrabBlkLowerRightX(1)!= m_oBinBlkMain.GrabBlkLowerRightXFromSetup(1)) ||
											(m_oBinBlkMain.GrabBlkLowerRightY(1)!= m_oBinBlkMain.GrabBlkLowerRightYFromSetup(1));

					if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(1))
					{
						m_bIfEnableTBSetUL = FALSE;
						m_bIfEnableTBSetLR = FALSE;

						//Block Dimensions can't be edited when it's disabled from same 
						//grade merge. You can only do this if grade is changed.
						SetAlert(IDS_BT_BLKDIMENSIONCHK1);
					}
					else if (bIfIsFirstBlkInMerge)
					{
						m_bIfEnableTBSetUL = FALSE;
						m_bIfEnableTBSetLR = FALSE;

						//Block Dimensions can't be edited if it's the first block in merge!
						SetAlert(IDS_BT_BLKDIMENSIONCHK2);
					}
					else
					{
						m_bIfEnableTBSetUL = TRUE;
						m_bIfEnableTBSetLR = TRUE;
					}	

					m_bIfEnablePIBlkPitchX = TRUE;
					m_bIfEnablePIBlkPitchY = TRUE;
					m_bIfEnablePIDiePitchX = TRUE;
					m_bIfEnablePIDiePitchY = TRUE;
					m_bIfEnablePIDiePerRow = TRUE;
					m_bIfEnablePIDiePerCol = TRUE;
					m_bIfEnablePIDiePerBlk = TRUE;
					m_bIfEnableChkBAutoAssignGrade = TRUE;
					m_bIfEnableFirstRowColSkipPattern = TRUE;
					m_bIfEnableTeachWithPhysicalBlk = TRUE;
					m_bIfEnablePIGrade = FALSE;
					m_bIfEnableCBWalkPath = TRUE;
					m_bIfEnable2DBarcodeOutput = TRUE;
					m_bIfEnableTemplateSetup = FALSE;
					m_bIfEnableCentralizedBondArea = TRUE;
					m_bIfEnableWafflePadCheckBox = TRUE;
					m_bIfEnablePadPitchSetX = TRUE;
					m_bIfEnablePadPitchSetY = TRUE;
					m_bIfEnablePadPitchResetX = TRUE;
					m_bIfEnablePadPitchResetY = TRUE;
					m_bIfEnablePadDimX = TRUE;
					m_bIfEnablePadDimY = TRUE;
					m_bIfEnableWafflePad = TRUE;
					m_bIfUseBlockCornerAsFirstDiePos = TRUE;
					m_bIfEnableTBSubmit = TRUE;
					m_bIfEnableTBCancel = TRUE;
				}
			}
			else //Block to Setup != 1
			{
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = FALSE;
				m_bIfEnablePIDiePitchY = FALSE;
				m_bIfEnablePIDiePerRow = FALSE;
				m_bIfEnablePIDiePerCol = FALSE;
				m_bIfEnablePIDiePerBlk = FALSE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = FALSE;
				
				m_bIfEnableWafflePad = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;

				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;

				SetAlert(IDS_BT_EDITBLK1DATAONLY);
			}
		}
		else if (m_szSetupMode == "Single Block")
		{
			UCHAR uGrade = m_oBinBlkMain.GrabGrade(ulBinBlkId);

			if (m_oBinBlkMain.GrabNVNoOfSortedDie(uGrade) > 0)
			{
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = FALSE;
				m_bIfEnablePIDiePitchY = FALSE;
				m_bIfEnablePIDiePerRow = FALSE;
				m_bIfEnablePIDiePerCol = FALSE;
				m_bIfEnablePIDiePerBlk = FALSE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = FALSE;
				
				m_bIfEnableWafflePad = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;

				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;	

				//Block settings can't be edited when bin count is not cleared!
				SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
			}
			else
			{
				bIfIsFirstBlkInMerge =	(m_oBinBlkMain.GrabBlkUpperLeftX(ulBinBlkId) !=	m_oBinBlkMain.GrabBlkUpperLeftXFromSetup(ulBinBlkId)) ||
										(m_oBinBlkMain.GrabBlkUpperLeftY(ulBinBlkId) != m_oBinBlkMain.GrabBlkUpperLeftYFromSetup(ulBinBlkId)) ||
										(m_oBinBlkMain.GrabBlkLowerRightX(ulBinBlkId)!= m_oBinBlkMain.GrabBlkLowerRightXFromSetup(ulBinBlkId))||
										(m_oBinBlkMain.GrabBlkLowerRightY(ulBinBlkId)!= m_oBinBlkMain.GrabBlkLowerRightYFromSetup(ulBinBlkId));

				if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(ulBinBlkId))
				{
					m_bIfEnableTBSetUL = FALSE;
					m_bIfEnableTBSetLR = FALSE;

					//Block Dimensions can't be edited when it's disabled from same grade merge.
					//You can only do this if grade is changed.
					SetAlert(IDS_BT_BLKDIMENSIONCHK1);
				}
				else if (bIfIsFirstBlkInMerge)
				{
					m_bIfEnableTBSetUL = FALSE;
					m_bIfEnableTBSetLR = FALSE;

					//Block Dimensions can't be edited if it's the first block in merge!
					SetAlert(IDS_BT_BLKDIMENSIONCHK2);
				}
				else
				{
					m_bIfEnableTBSetUL = TRUE;
					m_bIfEnableTBSetLR = TRUE;
				}	

				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = TRUE;
				m_bIfEnablePIDiePitchY = TRUE;
				m_bIfEnablePIDiePerRow = TRUE;
				m_bIfEnablePIDiePerCol = TRUE;
				m_bIfEnablePIDiePerBlk = TRUE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = TRUE;
				m_bIfEnableCBWalkPath = TRUE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableFirstRowColSkipPattern = TRUE;
				m_bIfEnableTeachWithPhysicalBlk = TRUE;
				m_bIfEnableTemplateSetup = TRUE;
				m_bIfEnableCentralizedBondArea = TRUE;
				m_bIfEnableWafflePad = TRUE;
				m_bIfEnableWafflePadCheckBox = TRUE;
				m_bIfEnablePadPitchSetX = TRUE;
				m_bIfEnablePadPitchSetY = TRUE;
				m_bIfEnablePadPitchResetX = TRUE;
				m_bIfEnablePadPitchResetY = TRUE;
				m_bIfEnablePadDimX = TRUE;
				m_bIfEnablePadDimY = TRUE;
				m_bIfUseBlockCornerAsFirstDiePos = TRUE;
				m_bIfEnableTBSubmit = TRUE;
				m_bIfEnableTBCancel = TRUE;
			}
		}
	}
}

VOID CBinTable::OnBinBlkSetupMode(CString szSetupMode)
{
	ULONG i;
	BOOL bIfBondingInProcess, bIfIsFirstBlkInMerge;

	if (szSetupMode == "All Blocks")
	{
		if (m_ulNoOfBinBlk == 0)
		{
			//Disable all controls
			m_bIfEnablePIBlkToSetup = FALSE;
			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnablePIBlkPitchX = FALSE;
			m_bIfEnablePIBlkPitchY = FALSE;
			m_bIfEnablePIDiePitchX = FALSE;
			m_bIfEnablePIDiePitchY = FALSE;
			m_bIfEnablePIDiePerRow = FALSE;
			m_bIfEnablePIDiePerCol = FALSE;
			m_bIfEnablePIDiePerBlk = FALSE;
			m_bIfEnableChkBAutoAssignGrade = FALSE;
			m_bIfEnablePIGrade = FALSE;
			m_bIfEnableCBWalkPath = FALSE;
			m_bIfEnableFirstRowColSkipPattern = FALSE;
			m_bIfEnableTeachWithPhysicalBlk = FALSE;
			m_bIfEnable2DBarcodeOutput = FALSE;	
			m_bIfEnableTemplateSetup = FALSE;
			m_bIfEnableCentralizedBondArea = FALSE;
			m_bIfEnableWafflePadCheckBox = FALSE;
			m_bIfEnablePadPitchSetX = FALSE;
			m_bIfEnablePadPitchSetY = FALSE;
			m_bIfEnablePadPitchResetX = FALSE;
			m_bIfEnablePadPitchResetY = FALSE;
			m_bIfEnablePadDimX = FALSE;
			m_bIfEnablePadDimY = FALSE;
			m_bIfEnableWafflePad = FALSE;
			m_bIfUseBlockCornerAsFirstDiePos = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel = FALSE;

			m_ulBinBlkToSetup = 0;
			m_lBinUpperLeftX = 0;	
			m_lBinUpperLeftY = 0;
			m_lBinLowerRightX = 0;
			m_lBinLowerRightY = 0;

			m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
			m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

			m_dDiePitchX = 500;
			m_dDiePitchY = 500;
			m_bUsePt5UmInDiePitchX	= FALSE;
			m_bUsePt5UmInDiePitchY	= FALSE;
			m_dBondAreaOffsetXInUm	= 0;
			m_dBondAreaOffsetYInUm	= 0;
			m_dThermalDeltaPitchXInUm	= 0;	//v4.59A22	//David Ma
			m_dThermalDeltaPitchYInUm	= 0;	//v4.59A22	//David Ma
			m_ulDiePerRow = 0;
			m_ulDiePerCol = 0;
			m_ulDiePerBlk = 0;
			m_bByPassBinMap = FALSE;

			m_bEnableFirstRowColSkipPattern = 0;
			m_ulFirstRowColSkipUnit = 0;

			m_bTeachWithPhysicalBlk = FALSE;

			//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
			m_bEnableCircularBondArea	= FALSE;
			m_lCirAreaCenterX			= 0;
			m_lCirAreaCenterY			= 0;
			m_dCirAreaRadius			= 0;

			m_bCentralizedBondArea = FALSE;
			m_lCentralizedUpperLeftX = 0;
			m_lCentralizedUpperLeftY = 0;

			m_bEnableWafflePad = FALSE;
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;
			m_lWafflePadDistX = 0;
			m_lWafflePadDistY = 0;

			m_bUseBlockCornerAsFirstDiePos = FALSE;
			m_bNoReturnTravel = FALSE;		//v4.42T1

			m_ucGrade = 1;
			
			m_szWalkPath = BT_TLH_NAME;
		}
		else //no of blks > 0
		{
			m_ulBinBlkToSetup = 1;
			
			//Display Block 1 Data
			m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(1));	
			m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(1));
			m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(1));
			m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(1));

			m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
			m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

			m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(1));
			m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(1));
			m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(1);
			m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(1);
			m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(1);
			m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(1);
			m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(1);	//v4.59A22	//David Ma
			m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(1);	//v4.59A22	//David Ma
			m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(1);

			m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(1);
			m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(1);
			m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(1);
			m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(1); 	 
			m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(1); 
			m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(1);
			m_ulFirstRowColSkipUnit = m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(1);

			m_bTeachWithPhysicalBlk = m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(1);

			m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(1);
			if (m_bCentralizedBondArea)
			{
				m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(1);
				m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(1);
				m_lCentralizedUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(1) + m_lCentralizedOffsetX);
				m_lCentralizedUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(1) + m_lCentralizedOffsetY);
			}
			else
			{
				m_lCentralizedUpperLeftX = 0;
				m_lCentralizedUpperLeftY = 0;
			}

			m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(1);
			if (m_bEnableWafflePad)
			{
				m_lWafflePadDistX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(1));
				m_lWafflePadDistY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(1));
				m_lWafflePadSizeX = m_oBinBlkMain.GrabWafflePadSizeX(1);
				m_lWafflePadSizeY = m_oBinBlkMain.GrabWafflePadSizeY(1);
			}
			else
			{
				m_lWafflePadDistX = 0;
				m_lWafflePadDistY = 0;
				m_lWafflePadSizeX = 0;
				m_lWafflePadSizeY = 0;
			}

			m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(1);
			m_bNoReturnTravel = m_oBinBlkMain.GrabNoReturnTravel(1);		//v4.42T1

			//pllm
			m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(1);
			m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(1);
			m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(1);
			DOUBLE dRadius = (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(1));		// from encoder step to um
			m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 

			m_ucGrade = m_oBinBlkMain.GrabGrade(1);
			
			m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(1));

			bIfBondingInProcess = FALSE;

			for (i = 1; i <= m_ulNoOfBinBlk; i++)
			{
				if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
				{
					bIfBondingInProcess = TRUE;
					break;
				}
			}

			if (bIfBondingInProcess)
			{
				//Disable all controls
				m_bIfEnablePINoOfBlk = FALSE;
				m_bIfEnablePIBlkToSetup = FALSE;
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = FALSE;
				m_bIfEnablePIDiePitchY = FALSE;
				m_bIfEnablePIDiePerRow = FALSE;
				m_bIfEnablePIDiePerCol = FALSE;
				m_bIfEnablePIDiePerBlk = FALSE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = FALSE;
				m_bIfEnableWafflePad = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;
				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;

				SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
			}
			else //NOT during bonding
			{
				bIfIsFirstBlkInMerge =	(m_oBinBlkMain.GrabBlkUpperLeftX(1) != m_oBinBlkMain.GrabBlkUpperLeftXFromSetup(1)) ||
										(m_oBinBlkMain.GrabBlkUpperLeftY(1) != m_oBinBlkMain.GrabBlkUpperLeftYFromSetup(1)) ||
										(m_oBinBlkMain.GrabBlkLowerRightX(1)!= m_oBinBlkMain.GrabBlkLowerRightXFromSetup(1)) ||
										(m_oBinBlkMain.GrabBlkLowerRightY(1)!= m_oBinBlkMain.GrabBlkLowerRightYFromSetup(1));

				if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(1))
				{
					m_bIfEnableTBSetUL = FALSE;
				    m_bIfEnableTBSetLR = FALSE;

					SetAlert(IDS_BT_BLKDIMENSIONCHK1);
				}
				else if (bIfIsFirstBlkInMerge)
				{
					m_bIfEnableTBSetUL = FALSE;
				    m_bIfEnableTBSetLR = FALSE;

					SetAlert(IDS_BT_BLKDIMENSIONCHK2);
				}
				else
				{
					m_bIfEnableTBSetUL = TRUE;
					m_bIfEnableTBSetLR = TRUE;
				}			

				//Enable other controls
				m_bIfEnablePIBlkPitchX = TRUE;
				m_bIfEnablePIBlkPitchY = TRUE;
				m_bIfEnablePIDiePitchX = TRUE;
				m_bIfEnablePIDiePitchY = TRUE;
				m_bIfEnablePIDiePerRow = TRUE;
				m_bIfEnablePIDiePerCol = TRUE;
				m_bIfEnablePIDiePerBlk = TRUE;
				m_bIfEnableChkBAutoAssignGrade = TRUE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = TRUE;
				m_bIfEnableFirstRowColSkipPattern = TRUE;
				m_bIfEnableTeachWithPhysicalBlk = TRUE;
				m_bIfEnable2DBarcodeOutput = TRUE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = TRUE;
				m_bIfEnableWafflePad = TRUE;
				m_bIfEnableWafflePadCheckBox = TRUE;
				m_bIfEnablePadPitchSetX = TRUE;
				m_bIfEnablePadPitchSetY = TRUE;
				m_bIfEnablePadPitchResetX = TRUE;
				m_bIfEnablePadPitchResetY = TRUE;
				m_bIfEnablePadDimX = TRUE;
				m_bIfEnablePadDimY = TRUE;
				m_bIfUseBlockCornerAsFirstDiePos = TRUE;
				m_bIfEnableTBSubmit = TRUE;
				m_bIfEnableTBCancel = TRUE;
			}
		}
	}
	else if (szSetupMode == "Single Block")
	{
		if (m_ulNoOfBinBlk == 0)
		{
			m_bIfEnablePIBlkToSetup = FALSE;
			m_bIfEnableTBSetUL = FALSE;
			m_bIfEnableTBSetLR = FALSE;
			m_bIfEnablePIBlkPitchX = FALSE;
			m_bIfEnablePIBlkPitchY = FALSE;
			m_bIfEnablePIDiePitchX = FALSE;
			m_bIfEnablePIDiePitchY = FALSE;
			m_bIfEnablePIDiePerRow = FALSE;
			m_bIfEnablePIDiePerCol = FALSE;
			m_bIfEnablePIDiePerBlk = FALSE;
			m_bIfEnableChkBAutoAssignGrade = FALSE;
			m_bIfEnablePIGrade = FALSE;
			m_bIfEnableCBWalkPath = FALSE;
			m_bIfEnableFirstRowColSkipPattern = FALSE;
			m_bIfEnableTeachWithPhysicalBlk = FALSE;
			m_bIfEnable2DBarcodeOutput = FALSE;	
			m_bIfEnableTemplateSetup = FALSE;
			m_bIfEnableCentralizedBondArea = FALSE;
			m_bIfEnableWafflePadCheckBox = FALSE;
			m_bIfEnablePadPitchSetX = FALSE;
			m_bIfEnablePadPitchSetY = FALSE;
			m_bIfEnablePadPitchResetX = FALSE;
			m_bIfEnablePadPitchResetY = FALSE;
			m_bIfEnablePadDimX = FALSE;
			m_bIfEnablePadDimY = FALSE;
			m_bIfEnableWafflePad = FALSE;
			m_bIfUseBlockCornerAsFirstDiePos = FALSE;
			m_bIfEnableTBSubmit = FALSE;
			m_bIfEnableTBCancel	= FALSE;		
		}
		else //no of blk != 0
		{
			if (m_ulBinBlkToSetup == 0)
			{
				m_bIfEnableTBSetUL = FALSE;
				m_bIfEnableTBSetLR = FALSE;
				m_bIfEnablePIBlkPitchX = FALSE;
				m_bIfEnablePIBlkPitchY = FALSE;
				m_bIfEnablePIDiePitchX = FALSE;
				m_bIfEnablePIDiePitchY = FALSE;
				m_bIfEnablePIDiePerRow = FALSE;
				m_bIfEnablePIDiePerCol = FALSE;
				m_bIfEnablePIDiePerBlk = FALSE;
				m_bIfEnableChkBAutoAssignGrade = FALSE;
				m_bIfEnablePIGrade = FALSE;
				m_bIfEnableCBWalkPath = FALSE;
				m_bIfEnableFirstRowColSkipPattern = FALSE;
				m_bIfEnableTeachWithPhysicalBlk = FALSE;
				m_bIfEnable2DBarcodeOutput = FALSE;
				m_bIfEnableTemplateSetup = FALSE;
				m_bIfEnableCentralizedBondArea = FALSE;
				m_bIfEnableWafflePadCheckBox = FALSE;
				m_bIfEnablePadPitchSetX = FALSE;
				m_bIfEnablePadPitchSetY = FALSE;
				m_bIfEnablePadPitchResetX = FALSE;
				m_bIfEnablePadPitchResetY = FALSE;
				m_bIfEnablePadDimX = FALSE;
				m_bIfEnablePadDimY = FALSE;
				m_bIfEnableWafflePad = FALSE;
				m_bIfUseBlockCornerAsFirstDiePos = FALSE;
				m_bIfEnableTBSubmit = FALSE;
				m_bIfEnableTBCancel = FALSE;
			}
			else
			{
				//Display "Blk to Setup" Data
				m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(m_ulBinBlkToSetup));	
				m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(m_ulBinBlkToSetup));
				m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(m_ulBinBlkToSetup));
				m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(m_ulBinBlkToSetup));

				m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
				m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

				m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(m_ulBinBlkToSetup));
				m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(m_ulBinBlkToSetup));
				m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(m_ulBinBlkToSetup);
				m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(m_ulBinBlkToSetup);
				m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(m_ulBinBlkToSetup);
				m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(m_ulBinBlkToSetup);
				m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(m_ulBinBlkToSetup);

				m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(m_ulBinBlkToSetup);
				m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(m_ulBinBlkToSetup);
				m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToSetup);
				m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(m_ulBinBlkToSetup); 	 
				m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(m_ulBinBlkToSetup);
				m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(m_ulBinBlkToSetup);
				m_ulFirstRowColSkipUnit = m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(m_ulBinBlkToSetup);
				m_bTeachWithPhysicalBlk = m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(m_ulBinBlkToSetup);
			
				//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
				m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(m_ulBinBlkToSetup);
				m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(m_ulBinBlkToSetup);
				m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(m_ulBinBlkToSetup);
				DOUBLE dRadius = (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(m_ulBinBlkToSetup));		// from encoder step to um
				m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 

				m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(m_ulBinBlkToSetup);
				if (m_bCentralizedBondArea)
				{
					m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(m_ulBinBlkToSetup);
					m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(m_ulBinBlkToSetup);
					m_lCentralizedUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(m_ulBinBlkToSetup) + m_lCentralizedOffsetX);
					m_lCentralizedUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(m_ulBinBlkToSetup) + m_lCentralizedOffsetY);
				}
				else
				{
					m_lCentralizedUpperLeftX = 0;
					m_lCentralizedUpperLeftY = 0;
				}

				m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(m_ulBinBlkToSetup);
				m_bNoReturnTravel = m_oBinBlkMain.GrabNoReturnTravel(m_ulBinBlkToSetup);		//v4.42T1

				m_ucGrade = m_oBinBlkMain.GrabGrade(m_ulBinBlkToSetup);
				
				m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(m_ulBinBlkToSetup));
				
				UCHAR uGrade = m_oBinBlkMain.GrabGrade(m_ulBinBlkToSetup);
				if (m_oBinBlkMain.GrabNVNoOfSortedDie(uGrade) > 0)
				{
					m_bIfEnableTBSetUL = FALSE;
					m_bIfEnableTBSetLR = FALSE;
					m_bIfEnablePIBlkPitchX = FALSE;
					m_bIfEnablePIBlkPitchY = FALSE;
					m_bIfEnablePIDiePitchX = FALSE;
					m_bIfEnablePIDiePitchY = FALSE;
					m_bIfEnablePIDiePerRow = FALSE;
					m_bIfEnablePIDiePerCol = FALSE;
					m_bIfEnablePIDiePerBlk = FALSE;
					m_bIfEnableChkBAutoAssignGrade = FALSE;
					m_bIfEnablePIGrade = FALSE;
					m_bIfEnableCBWalkPath = FALSE;
					m_bIfEnableFirstRowColSkipPattern = FALSE;
					m_bIfEnableTeachWithPhysicalBlk = FALSE;
					m_bIfEnable2DBarcodeOutput = FALSE;
					m_bIfEnableTemplateSetup = FALSE;
					m_bIfEnableCentralizedBondArea = FALSE;
					m_bIfEnableWafflePad = FALSE;
					m_bIfEnableWafflePadCheckBox = FALSE;
					m_bIfEnablePadPitchSetX = FALSE;
					m_bIfEnablePadPitchSetY = FALSE;
					m_bIfEnablePadPitchResetX = FALSE;
					m_bIfEnablePadPitchResetY = FALSE;
					m_bIfEnablePadDimX = FALSE;
					m_bIfEnablePadDimY = FALSE;
					m_bIfUseBlockCornerAsFirstDiePos = FALSE;
					m_bIfEnableTBSubmit = FALSE;
					m_bIfEnableTBCancel = FALSE;		

					SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
				}
				else
				{
					bIfIsFirstBlkInMerge =	(m_oBinBlkMain.GrabBlkUpperLeftX(m_ulBinBlkToSetup) != m_oBinBlkMain.GrabBlkUpperLeftXFromSetup(m_ulBinBlkToSetup)) ||
											(m_oBinBlkMain.GrabBlkUpperLeftY(m_ulBinBlkToSetup) != m_oBinBlkMain.GrabBlkUpperLeftYFromSetup(m_ulBinBlkToSetup)) ||
											(m_oBinBlkMain.GrabBlkLowerRightX(m_ulBinBlkToSetup)!= m_oBinBlkMain.GrabBlkLowerRightXFromSetup(m_ulBinBlkToSetup)) ||
											(m_oBinBlkMain.GrabBlkLowerRightY(m_ulBinBlkToSetup)!= m_oBinBlkMain.GrabBlkLowerRightYFromSetup(m_ulBinBlkToSetup));

					if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(m_ulBinBlkToSetup))
					{
						m_bIfEnableTBSetUL = FALSE;
						m_bIfEnableTBSetLR = FALSE;

						SetAlert(IDS_BT_BLKDIMENSIONCHK1);
					}
					else if (bIfIsFirstBlkInMerge)
					{
						m_bIfEnableTBSetUL = FALSE;
						m_bIfEnableTBSetLR = FALSE;

						SetAlert(IDS_BT_BLKDIMENSIONCHK2);
					}
					else
					{
						m_bIfEnableTBSetUL = TRUE;
						m_bIfEnableTBSetLR = TRUE;
					}		

					m_bIfEnablePIBlkToSetup = TRUE;
					m_bIfEnableTBSetUL = TRUE;
					m_bIfEnableTBSetLR = TRUE;
					m_bIfEnablePIBlkPitchX = FALSE;
					m_bIfEnablePIBlkPitchY = FALSE;
					m_bIfEnablePIDiePitchX = TRUE;
					m_bIfEnablePIDiePitchY = TRUE;
					m_bIfEnablePIDiePerRow = TRUE;
					m_bIfEnablePIDiePerCol = TRUE;
					m_bIfEnablePIDiePerBlk = TRUE;
					m_bIfEnableChkBAutoAssignGrade = FALSE;
					m_bIfEnablePIGrade = TRUE;
					m_bIfEnableCBWalkPath = TRUE;
					m_bIfEnableFirstRowColSkipPattern = TRUE;
					m_bIfEnableTeachWithPhysicalBlk = TRUE;
					m_bIfEnable2DBarcodeOutput = FALSE;
					m_bIfEnableTemplateSetup = TRUE;
					m_bIfEnableCentralizedBondArea = TRUE;
					m_bIfEnableWafflePad = TRUE;
					m_bIfEnableWafflePadCheckBox = TRUE;
					m_bIfEnablePadPitchSetX = TRUE;
					m_bIfEnablePadPitchSetY = TRUE;
					m_bIfEnablePadPitchResetX = TRUE;
					m_bIfEnablePadPitchResetY = TRUE;
					m_bIfEnablePadDimX = TRUE;
					m_bIfEnablePadDimY = TRUE;

					m_bIfUseBlockCornerAsFirstDiePos = TRUE;
					m_bIfEnableTBSubmit = TRUE;
					m_bIfEnableTBCancel = TRUE;
				}
			}
		}
	}
}

LONG CBinTable::MS90RotateBT180(IPC_CServiceMessage& svMsg)
{
	if( IsMS90() )
	{
		BOOL bEnable = FALSE;
		svMsg.GetMsg(sizeof(BOOL), &bEnable);

		SetJoystickOn(FALSE);
/*
		MoveToCOR("BT: Move to COR6");

		T_Profile(NORMAL_PROF);
		if (bEnable)
		{
			//T_MoveTo(23198);	//180 degree
			//v4.59A31
			INT nPos = (INT) (180.0 / m_dThetaRes);
			T_MoveTo(nPos, SFM_WAIT);		//180 degree
		}
		else
		{
			T_MoveTo(0, SFM_WAIT);
		}
//===============================2018.5.4=======================
		T_Profile(LOW_PROF);
//		T_Home(bEnable);
//==============================================================
*/
		BT_SetFrameRotation(0, bEnable, TRUE);
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}	//	rotate BT 0 or 180 degree

LONG CBinTable::AllBlocksSetup(IPC_CServiceMessage& svMsg)
{
	CString szStr;

	m_ulSourcePhyBlk	= 1;
	m_ulBinBlkToSetup	= 1;
	m_szSetupMode		= "All Blocks";

	szStr.Format("Genesis Load AMI do All Blocks Setup on Mode");
	CMSLogFileUtility::Instance()->MS_LogOperation(szStr); 
	OnBinBlkSetupMode(m_szSetupMode);

	szStr.Format("Genesis Load AMI do All Blocks Setup on Blk 1");
	CMSLogFileUtility::Instance()->MS_LogOperation(szStr); 
	OnBinBlkSetupNo(m_ulBinBlkToSetup);

	m_dDiePitchX	= (DOUBLE)(LONG)(*m_psmfSRam)["MS896A"]["AmiFile"]["DiePitchX"];	// get from AMI to setup block
	m_dDiePitchY	= (DOUBLE)(LONG)(*m_psmfSRam)["MS896A"]["AmiFile"]["DiePitchY"];	// get from AMI to setup block

	szStr.Format("Genesis Load AMI do All Blocks Setup on All blocks");
	CMSLogFileUtility::Instance()->MS_LogOperation(szStr); 
	SetupBinBlk(m_szSetupMode, TRUE, TRUE);	// submit and to confirm settings.

	if (m_bEnableBinMapBondArea)
	{
		HmiMessage("BinMap fcn is disabled; please re-enable & re-teach bin map pattern");
		
		m_bEnableBinMapBondArea = FALSE;
		m_oBinBlkMain.SetUseBinMapBondArea(m_bEnableBinMapBondArea);						//v4.03		//PLLM Bin map fcn

		m_BinMapWrapper.InitMap();
		SaveBinTableData();
	}

	if (m_bEnableOsramBinMixMap)	//v4.47A6
	{
		HmiMessage("OSRAM BinMap fcn is disabled; please re-enable & re-teach bin map pattern");
		
		m_bEnableOsramBinMixMap = FALSE;
		m_BinMapWrapper.InitMap();
		SaveBinTableData();
	}

	szStr.Format("Genesis Load AMI do All Blocks Setup Backup NVRam");
	CMSLogFileUtility::Instance()->MS_LogOperation(szStr); 
	BackupToNVRAM();

	szStr.Format("Genesis Load AMI do All Blocks Setup on Update file checking");
	CMSLogFileUtility::Instance()->MS_LogOperation(szStr); 
	UpdateAmiFileChecking();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::SemitekGenerateOutputFileWhenReadWaferBarcode(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bNeedToClear = FALSE;
	ULONG ulClearCount = 0;
	CString szBinOutputFileFormat = "Semitek2";
	CString szContent, szTitle;
	CMSLogFileUtility::Instance()->WL_LogStatus("SemitekGenerateOutputFileWhenReadWaferBarcode");
	//AfxMessageBox("2");
	for (INT i = 1; i <= (INT) m_oPhyBlkMain.GetNoOfBlk(); i++)
	{
		if (ClrBinCntCmdSubTasks(i, "All Blocks", szBinOutputFileFormat, TRUE, FALSE, bNeedToClear,FALSE) == TRUE)
		{
			ulClearCount++;
		}
	}

	if (ulClearCount == 0)
	{
		//AfxMessageBox("3");
		szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);
		bReturn = FALSE;
		HmiMessage(szContent, szTitle);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	else if (ulClearCount == m_oPhyBlkMain.GetNoOfBlk())
	{
		SetStatusMessage("All Bin Counters are cleared");
	}
	else
	{
		//AfxMessageBox("4");
		szContent.LoadString(HMB_BT_SOME_BIN_COUNTER_NOT_CLR);
		bReturn = FALSE;
		HmiMessage(szContent, szTitle);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::SetFrameAlignAngle(IPC_CServiceMessage& svMsg)		//v4.57A9	//DavidMa
{
	BOOL bReturn = TRUE;

	DOUBLE dAngle = 0;
	svMsg.GetMsg(sizeof(DOUBLE), &dAngle);

	m_dFrameAlignAngleForTesting = dAngle;
	
	m_oBinBlkMain.SetRotateAngleX(1, m_dFrameAlignAngleForTesting);
	CMS896AStn::m_bRealignFrameDone = TRUE;

	CString szLog;
	szLog.Format("Frame angle is updated to %.6f for BIn frame #1 only",  m_dFrameAlignAngleForTesting);
	HmiMessage(szLog);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::SetFrameAlignOffset(IPC_CServiceMessage& svMsg)		//v4.59A21	//DavidMa
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG	lXOffset;
		LONG	lYOffset;	
	} BLKDATA;
	BLKDATA stInfo;

	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

	m_lFrameXOffsetForTesting = stInfo.lXOffset;
	m_lFrameYOffsetForTesting = stInfo.lYOffset;

	//m_oBinBlkMain.SetRotateAngleX(1, m_dFrameAlignAngleForTesting);
	m_oBinBlkMain.SetXOffset(1, m_lFrameXOffsetForTesting);
	m_oBinBlkMain.SetYOffset(1, m_lFrameYOffsetForTesting);
	CMS896AStn::m_bRealignFrameDone = TRUE;

	CString szLog;
	szLog.Format("Frame Offset is updated to %d, %d for Bin frame #1 only",  
		m_lFrameXOffsetForTesting, m_lFrameYOffsetForTesting);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	HmiMessage(szLog);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::SetThermalDeltaPitch(IPC_CServiceMessage& svMsg)		//v4.59A22	//DavidMa
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		DOUBLE	dXOffset;
		DOUBLE	dYOffset;	
	} BLKDATA;
	BLKDATA stInfo;

	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

	m_dThermalDeltaPitchXInUm = stInfo.dXOffset;
	m_dThermalDeltaPitchYInUm = stInfo.dYOffset;

	m_oBinBlkMain.SetThermalDeltaPitch(m_ulBinBlkToSetup, m_dThermalDeltaPitchXInUm, m_dThermalDeltaPitchYInUm);

	CString szMsg;
	szMsg.Format("Thermal Delta Pitch is updated for Bin #%lu: (%f, %f)", m_ulBinBlkToSetup, m_dThermalDeltaPitchXInUm, m_dThermalDeltaPitchYInUm);
	//HmiMessage(szMsg);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::RotateFrameAngle(IPC_CServiceMessage& svMsg)	//v4.59A1
{
	BOOL bReturn = TRUE;

	DOUBLE dAngle = 0;
	svMsg.GetMsg(sizeof(DOUBLE), &dAngle);

	if (m_bEnable_T && T_IsPowerOn() && m_bSel_T)
	{
		CString szLog;
		szLog.Format("BT: RotateFrameAngle = %f degree", dAngle);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		if ((dAngle > -15) && (dAngle <= 15))
		{
			T_MoveByDegree(dAngle);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinTable::BT_BondLeftDie(IPC_CServiceMessage &svMsg)
{
	LONG lStage = 0;
	BOOL bReturn = FALSE;
	svMsg.GetMsg(sizeof(LONG), &lStage);

	if( lStage==1 )	//	befor bonded on
	{
		LoadWaferStatistics();

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		BOOL bEnableBHUplookPr = pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);	//v4.52A16
		if (OpIndex() == 1)
		{
			if (OpMoveTable() == gnOK)
			{
				WaitMoveComplete();
				Sleep(100);
				if (m_bPrPreBondAlignment)		//v3.79
				{
					(*m_psmfSRam)["BinTable"]["LFBondPad"]["IsFound"] = FALSE;
					if (!OpPrPreBondAlignPad())
					{
					}
				}
				ULONG ulBlkInUse	= (*m_psmfSRam)["BinTable"]["BlkInUse"];
				(*m_psmfSRam)["BinTable"]["SPC"]["Block"]		= ulBlkInUse;
				(*m_psmfSRam)["BinTable"]["SPC"]["Index"]		= m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse);
				(*m_psmfSRam)["BinTable"]["SPC"]["Bonded"]		= FALSE;
				(*m_psmfSRam)["BinTable"]["SPC"]["DiePerRow"]	= m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse);
				(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"]	= m_oBinBlkMain.GrabWalkPath(ulBlkInUse);
				if (bEnableBHUplookPr )
				{
					OpPerformUplookBTCompensation();	
				}
				WaitMoveComplete();
				Sleep(100);
				//OpUpdateMS100BinRealignAngle(m_lX, m_lY);		//v4.59A31
				bReturn = TRUE;
			}
		}
	}
	else if( lStage==2 )//	after die bonded
	{
		if( OpNextDie()==TRUE )
		{
			OpGenAllTempFile();
		}
		bReturn = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::BT_GetGradeBinBlock(IPC_CServiceMessage& svMsg)
{
	UCHAR ucGrade = 0;
	svMsg.GetMsg(sizeof(UCHAR), &ucGrade);
	LONG ulBlkInUse = m_oBinBlkMain.GrabNVBlkInUse(ucGrade);

	svMsg.InitMessage(sizeof(LONG), &ulBlkInUse);
	return 1;
}

//Called by load frame
LONG CBinTable::BT_SetCountByGrade(IPC_CServiceMessage& svMsg)
{
	if (IsMSAutoLineMode())
	{
		typedef struct 
		{
			ULONG	ulBlk[5];
			ULONG	ulUnloadDieGradeCount[5];		
		} BLKDATA;

		BLKDATA stInfo;
		svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

		CString szLog;
		for (int i = 0; i < 5; i++)
		{
			if (stInfo.ulBlk[i] > 0)
			{
				UCHAR ucGrade = m_oBinBlkMain.GrabGrade(stInfo.ulBlk[i]);
				ULONG ulInputCount = m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(stInfo.ulBlk[i]));
		//		stInfo.ulUnloadDieGradeCount = ulInputCount - stInfo.ulUnloadDieGradeCount;

				m_oBinBlkMain.SetCountByGrade((UCHAR)stInfo.ulBlk[i], stInfo.ulUnloadDieGradeCount[i]);
				
				ULONG ulBinBlkInUse = m_oBinBlkMain.GrabNVBlkInUse((UCHAR)stInfo.ulBlk[i]);
				if (ulBinBlkInUse > 0)
				{
					m_oBinBlkMain.UpdateCurrBondIndex(ulBinBlkInUse, stInfo.ulUnloadDieGradeCount[i]);
				}

				if (i == 0)
				{
					szLog.Format("BT: BT_SetCountByGrade(i, block, grade, InputCount, count) = %d, %d, %d, %d, %d", i, stInfo.ulBlk[i], ucGrade, ulInputCount, stInfo.ulUnloadDieGradeCount[i]);
				}
				else
				{
					szLog.AppendFormat("BT: BT_SetCountByGrade(i, block, grade, InputCount, count) = %d, %d, %d, %d, %d", i, stInfo.ulBlk[i], ucGrade, ulInputCount, stInfo.ulUnloadDieGradeCount[i]);
				}
			}
		}
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}

	BOOL bRet =  TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bRet);
	return 1;
}


VOID CBinTable::BT_ClearBinCounter(const ULONG ulBlkId)
{
	m_oBinBlkMain.ClearBinCounter(ulBlkId, FALSE);
}

//shiraishi02
LONG CBinTable::SetBinFrameOrientXY(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bCheckFrameOrientation)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	GetEncoderValue();
	m_lFrameOrientCheckX = m_lEnc_X;
	m_lFrameOrientCheckY = m_lEnc_Y;

	SaveBinTableData();

	CString szLog;
	szLog.Format("BT Frame Orient-Check XY is updated: (%ld, %ld)", 
		m_lFrameOrientCheckX, m_lFrameOrientCheckY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	szLog.Format("BT Frame Orientation Check XY position \nis updated: (%ld, %ld)", 
		m_lFrameOrientCheckX, m_lFrameOrientCheckY);
	HmiMessage(szLog, "Bin Frame Orientation Check");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::AGCMoveBinTableToBondPosn(IPC_CServiceMessage& svMsg)
{
	X_Sync();
	Y_Sync();
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
	Y_MoveTo(m_lAGCY);
	X_MoveTo(m_lAGCX);
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::BTChangeNoDiePosition(IPC_CServiceMessage& svMsg)
{	
	BOOL bReturn = TRUE;

	SetJoystickOn(FALSE);

	CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_X, m_lTableXNegLimit, m_lTableXPosLimit, &m_stBTAxis_X);		
	CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_Y, m_lTableXNegLimit, m_lTableXPosLimit, &m_stBTAxis_Y);

#ifdef NU_MOTION
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
#endif

	XY_MoveTo(m_lBTNoDiePosX, m_lBTNoDiePosY, SFM_WAIT);

#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	SetJoystickOn(TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::BTConfirmNoDiePositionSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
	CString szMsg;

	if (!m_fHardware || m_bDisableWT)	//v3.61
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	GetEncoderValue();

	m_lBTNoDiePosX = m_lEnc_X;
	m_lBTNoDiePosY = m_lEnc_Y;

	SaveBinTableData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

BOOL CBinTable::IsBinFrameRotated180()
{
	GetEncoderValue();
	if (labs(m_lEnc_T) > 2500)
	{
		return TRUE;
	}

	return FALSE;
}


LONG CBinTable::ReLoadBinMapFcn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	//v4.03		//PLLM/PLSG bin map fcn
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (!bEnableBinMapFcn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!m_bEnableBinMapBondArea)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	unsigned char aucAvailableGrades[1] = {'1'};
	unsigned char aaTempGrades[1];

	aucAvailableGrades[0] = 48 + 1;
	aaTempGrades[0] = 48 + 1;

	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;


	LONG lNoOfRows = m_oBinBlkMain.GrabNoOfDiePerCol(1);
	LONG lNoOfCols = m_oBinBlkMain.GrabNoOfDiePerRow(1);

	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows * lNoOfCols];
	aaGrades = new unsigned char*[lNoOfRows];
	for (int i = 0; i < lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i]; //new unsigned char[lNoOfCols];
	}

	// Set the grades
	for (int i = 0; i < lNoOfRows; i++) 
	{
		for (int j = 0; j < lNoOfCols; j++) 
		{
			if ( m_oBinBlkMain.m_nBinMap[i][j] == 1 )
			{
				aaGrades[i][j] = aaTempGrades[0];
			}
			else
			{
				aaGrades[i][j] = WAF_CMapConfiguration::DEFAULT_NULL_BIN;
			}
		}
	}

	//Clear Wafermap & reset grade map
	m_BinMapWrapper.InitMap();
	CString szDummyWaferMapName = "BinMap";	
	m_BinMapWrapper.CreateMap(szDummyWaferMapName, "Raw", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);

	m_oBinBlkMain.SetBinMapCircleRadius(0, m_dBinMapCircleRadiusInMm * 1000);
	m_BinMapWrapper.SuspendAlgorithmPreparation();
	m_BinMapWrapper.Redraw();

	delete [] aaGrades;
	delete [] pGradeBuffer;

	CMSLogFileUtility::Instance()->MS_LogOperation("BT: ReLoadBinMap");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::NGBlockBinBlkSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CUIntArray aulNGGradeBlockList;
	CNGGrade *pNGGrade = CNGGrade::Instance();

	pNGGrade->GetNGGradeBlockList(aulNGGradeBlockList);
	if (pNGGrade->IsHaveNGGrade() && CMS896AApp::m_bNGWithBinMap)
	{
		for (LONG i = 0; i < aulNGGradeBlockList.GetSize(); i++)
		{
			m_oBinBlkMain.SetDDiePitchX((LONG)aulNGGradeBlockList[i], m_oBinBlkMain.GrabDDiePitchX(1));
			m_oBinBlkMain.SetDDiePitchY((LONG)aulNGGradeBlockList[i], m_oBinBlkMain.GrabDDiePitchY(1));
			m_oBinBlkMain.SetNoOfDiePerBlk((LONG)aulNGGradeBlockList[i], m_oBinBlkMain.GrabNoOfDiePerBlk(1));
			m_oBinBlkMain.SetNoOfDiePerRow((LONG)aulNGGradeBlockList[i], m_oBinBlkMain.GrabNoOfDiePerRow(1));
			m_oBinBlkMain.SetNoOfDiePerCol((LONG)aulNGGradeBlockList[i], m_oBinBlkMain.GrabNoOfDiePerCol(1));		
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}



LONG CBinTable::GoAndSearchAllBinBlockDie(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	//ULONG ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToStepMove);
	//LONG lIndexToMoveTo = 0, lTargetX, lTargetY;
	//DOUBLE dX = 0, dY = 0;

	//LONG lBTDelay = (*m_psmfSRam)["BinTable"]["BTDelay"];

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr == NULL)
	{
		HmiMessage("pBondPr != NULL fail","Bond Die fail");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	HmiMessage("Starting Offline PostBOND Test ....");

	m_lIndexToMoveTo			= 1;
	m_bDoOfflinePostbondTest2	= FALSE;

	m_bDoOfflinePostbondTest	= TRUE;

	/*
	while (1)
	{
		if (lIndexToMoveTo > LONG(ulDiePerBlk))
		{
			break;
		}
		if (lIndexToMoveTo >  m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove))
		{
			break;
		}
		m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, lIndexToMoveTo, dX, dY, TRUE);
		lTargetX = ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX;
		lTargetY = ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY;
		
		if (IsWithinTable1Limit(lTargetX, lTargetY))
		{
			XY_MoveTo(lTargetX, lTargetY);
			
			if ( (lBTDelay > 0) && (lBTDelay <= 1000) )
			{
				Sleep(lBTDelay);	//andrewng //2020-0626
			}
			else
			{
				Sleep(50);
			}

			pBondPr->SearchBPRDie(FALSE, TRUE);
		}

		lIndexToMoveTo++;
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::StopOfflinePostbondTest(IPC_CServiceMessage &svMsg)		//andrewng //2020-0804
{
	BOOL bReturn = TRUE;

	m_bDoOfflinePostbondTest = FALSE;
	m_bDoOfflinePostbondTest2 = FALSE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

BOOL CBinTable::OfflinePostBondTest1()
{
	LONG lTargetX = 0, lTargetY = 0;
	DOUBLE dX = 0, dY = 0;


	ULONG ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToStepMove);
	LONG lBTDelay = (*m_psmfSRam)["BinTable"]["BTDelay"];

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr == NULL)
	{
		HmiMessage("pBondPr != NULL fail","Bond Die fail");
		return FALSE;
	}

	if (m_lIndexToMoveTo > LONG(ulDiePerBlk))
	{
		m_bDoOfflinePostbondTest = FALSE;
		return FALSE;
	}
	if (m_lIndexToMoveTo >  m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove))
	{
		m_bDoOfflinePostbondTest = FALSE;
		return FALSE;
	}

	m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, m_lIndexToMoveTo, dX, dY, TRUE);
	lTargetX = ConvertFileUnitToXEncoderValue(dX) - m_lColletOffsetX;
	lTargetY = ConvertFileUnitToYEncoderValue(dY) - m_lColletOffsetY;
		

	if (IsWithinTable1Limit(lTargetX, lTargetY))
	{
		XY_MoveTo(lTargetX, lTargetY);
			
		if ( (lBTDelay > 0) && (lBTDelay <= 1000) )
		{
			Sleep(lBTDelay);	//andrewng //2020-0626
		}
		else
		{
			Sleep(50);
		}

		pBondPr->SearchBPRDie(FALSE, TRUE);
	}

	m_lIndexToMoveTo++;

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	return TRUE;
}

BOOL CBinTable::OfflinePostBondTest2()
{
	BOOL bReturn = TRUE;
	CString szLog, szLog2;

	LONG lTargetX, lTargetY;
	DOUBLE dX = 0, dY = 0;

	BOOL bIsLFMove		= FALSE;
	BOOL bDieResult		= FALSE;
	ULONG ulDiePerBlk	= m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToStepMove);
	LONG lBTDelay		= (*m_psmfSRam)["BinTable"]["BTDelay"];
    LONG lPitchX		= ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(m_ulBinBlkToStepMove));
    LONG lPitchY		= ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(m_ulBinBlkToStepMove));


	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr == NULL)
	{
		m_bDoOfflinePostbondTest2 = FALSE;
		HmiMessage("pBondPr != NULL fail","Bond Die fail");
		return FALSE;
	}

	if (m_lIndexToMoveTo > LONG(ulDiePerBlk))
	{
		m_bDoOfflinePostbondTest2 = FALSE;
		return FALSE;
	}
	if (m_lIndexToMoveTo > m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove))
	{
		m_bDoOfflinePostbondTest2 = FALSE;
		return FALSE;
	}

	BOOL bLastLFDieResult = m_bOffPBTestLFDieResult;
	m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, m_lIndexToMoveTo, dX, dY, TRUE);

	DOUBLE dX2 = 0, dY2 = 0;
	LONG lTargetX2 = 0, lTargetY2 = 0;
	if ( (m_lIndexToMoveTo < LONG(ulDiePerBlk)) && 
		 (m_lIndexToMoveTo < m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove)) )
	{
		m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, m_lIndexToMoveTo + 1, dX2, dY2, FALSE);
		lTargetX2 = ConvertFileUnitToXEncoderValue(dX2);
		lTargetY2 = ConvertFileUnitToYEncoderValue(dY2);
	}


	if ( (m_lIndexToMoveTo <= 1) || !bLastLFDieResult)
	{
		bIsLFMove = FALSE;		//This cycle not use LF pos
		lTargetX = ConvertFileUnitToXEncoderValue(dX);
		lTargetY = ConvertFileUnitToYEncoderValue(dY);
	}
	else	//Use LF Result from last cycle
	{
		bIsLFMove = TRUE;		//This cycle use LF pos
		lTargetX = m_lOffPBTestLFStepX;
		lTargetY = m_lOffPBTestLFStepY;
	}


	if (IsWithinTable1Limit(lTargetX, lTargetY)/* && !IsTable2InBondRegion()*/)
	{
		XY_MoveTo(lTargetX, lTargetY);
			
		if ( (lBTDelay > 0) && (lBTDelay <= 1000) )
		{
			Sleep(lBTDelay);	//andrewng //2020-0626
		}
		else
		{
			Sleep(50);
		}

		if (!bIsLFMove)
		{
			szLog.Format("PBTest: Move To #%d - Step Move (%ld, %ld)", m_lIndexToMoveTo, lTargetX, lTargetY);
		}
		else
		{
			szLog.Format("PBTest: Move To #%d LF - ENC (%ld, %ld)", m_lIndexToMoveTo, lTargetX, lTargetY);
		}
//HmiMessage(szLog);


		LONG lEncX = 0, lEncY = 0;
		GetXYEncoderValue(lEncX, lEncY);

		LONG lX = lEncX, lY = lEncY;
		CString szDieResult = "";
		CString szInfo		= "";
		szDieResult.Format("BtEnc,%ld,%ld,", lEncX, lEncY);
		bDieResult = pBondPr->BprSearchDie(lX, lY, szDieResult);

		if (!bIsLFMove)
		{
			//Do BT Compensation
			BT_MoveTo(lEncX + lX, lEncY + lY);
				
			if ( (lBTDelay > 0) && (lBTDelay <= 1000) )
			{
				Sleep(lBTDelay);	//andrewng //2020-0626
			}
			else
			{
				Sleep(50);
			}

			GetXYEncoderValue(lEncX, lEncY);
			szInfo.Format("IsLFMove,0,BtEncLF,%ld,%ld,", lEncX, lEncY);
			szDieResult = szDieResult + szInfo;
		}
		else
		{
			szInfo.Format("IsLFMove,1,BtEncLF,%ld,%ld,", lEncX, lEncY);
			szDieResult = szDieResult + szInfo;
		}


		if ( (m_lIndexToMoveTo < LONG(ulDiePerBlk)) && 
			 (m_lIndexToMoveTo < m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove)) )
		{
			//Calculate NEXT die encoder XY
			//DOUBLE dX2 = 0, dY2 = 0;
			//m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, m_lIndexToMoveTo + 1, dX2, dY2, FALSE);

			//#define		BPR_CT_DIE			0	//CENTER
			//#define		BPR_LT_DIE			1	//LHS
			//#define		BPR_RT_DIE			5	//RHS	
			//#define		BPR_UP_DIE			3	//UP
			//#define		BPR_DN_DIE			7	//DOWN
			//#define		BPR_UL_DIE			2	//UP+LHS
			//#define		BPR_DL_DIE			8	//DOWN+LHS	
			//#define		BPR_UR_DIE			4	//UP+RHS
			//#define		BPR_DR_DIE			6	//DOWN+RHS
			
			LONG lDir = 0;
			DOUBLE dDiffX = (DOUBLE) labs(lTargetX2 - lTargetX);
			DOUBLE dDiffY = (DOUBLE) labs(lTargetY2 - lTargetY);
			DOUBLE dPitchTolX = 0.5 * labs(lPitchX);
			DOUBLE dPitchTolY = 0.5 * labs(lPitchY);
				
			szLog.Format("PBTest2: LF-Dir calculation: Pitch-Diff (%f, %f). Pitch-Tol (%f, %f)", 
										dDiffX, dDiffY, dPitchTolX, dPitchTolY);
//HmiMessage(szLog);

			if (dDiffX > dPitchTolX)		//Next Die is moving LEFT or RIGHT
			{
				if (lTargetX2 > lTargetX)	//Next Die on LEFT
				{
					lDir = 1;	//BPR_LT_DIE
					szLog2 = "LEFT";
				}
				else						//Next Die on RIGHT
				{
					lDir = 5;	//BPR_RT_DIE
					szLog2 = "RIGHT";
				}
			}
			else						//Next Die is moving UP or DOWN
			{
				if (lTargetY2 > lTargetY)	//Next Die on UP
				{
					lDir = 3;	//BPR_UP_DIE
					szLog2 = "UP";
				}
				else						//Next Die on DOWN
				{
					lDir = 7;	//BPR_DN_DIE
					szLog2 = "DOWN";
				}
			}


			lX = lEncX;
			lY = lEncY;
			m_bOffPBTestLFDieResult = pBondPr->BprSearchLFDie(TRUE, lDir, !bIsLFMove, szDieResult, lX, lY);
				
			szLog.Format("PBTest2: LF Dir = %s, Result=%d, DieOFFSET-ENC (%ld, %ld)", 
							szLog2, m_bOffPBTestLFDieResult, lX, lY);
//HmiMessage(szLog);

			if (m_bOffPBTestLFDieResult)
			{
				m_lOffPBTestLFStepX = lEncX + lX;
				m_lOffPBTestLFStepY = lEncY + lY;
			}
			else
			{
				m_lOffPBTestLFStepX = 0;
				m_lOffPBTestLFStepY = 0;
			}
		}
		else	
		{
			//If no LF needed, just log current Die result
			CMSLogFileUtility::Instance()->BT_PostBondLog(szDieResult);
			m_bOffPBTestLFDieResult = FALSE;
			m_lOffPBTestLFStepX = 0;
			m_lOffPBTestLFStepY = 0;
		}
	}
	else		//ELSE if !IsWithinTable1Limit()
	{
		szLog.Format("PBTest2: ERR - XY Pos is out of table limit ENC(%ld, %ld, LF = %d", lTargetX, lTargetY, bIsLFMove);
		HmiMessage(szLog);
		m_bDoOfflinePostbondTest2 = FALSE;
		return FALSE;
	}


	m_lIndexToMoveTo++;

	//X_Profile(NORMAL_PROF);
	//Y_Profile(NORMAL_PROF);
	return TRUE;
}

//andrewng //2020-0803
LONG CBinTable::GoAndSearchAllBinBlockDie2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szLog, szLog2;

	GetBPRFOVSize(m_ulBinBlkToStepMove);	//Get BT Die Pitch from BT station

	HmiMessage("Starting Offline PostBOND Test 2 ...");

	m_lIndexToMoveTo = 1;
	m_lOffPBTestLFStepX = 0;
	m_lOffPBTestLFStepY = 0;
	m_bOffPBTestLFDieResult		= FALSE;

	m_bDoOfflinePostbondTest	= FALSE;
	m_bDoOfflinePostbondTest2	= TRUE;
	/*
	LONG lIndexToMoveTo = 0, lTargetX, lTargetY;
	DOUBLE dX = 0, dY = 0;
	LONG lLFStepX = 0, lLFStepY = 0;

	BOOL bIsLFMove			= FALSE;
	BOOL bDieResult			= FALSE;
	BOOL bLFDieResult		= FALSE;
	BOOL bLastLFDieResult	= FALSE;


	ULONG ulDiePerBlk	= m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToStepMove);
	LONG lBTDelay		= (*m_psmfSRam)["BinTable"]["BTDelay"];
    LONG lPitchX		= ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(m_ulBinBlkToStepMove));
    LONG lPitchY		= ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(m_ulBinBlkToStepMove));

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr == NULL)
	{
		HmiMessage("pBondPr != NULL fail","Bond Die fail");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	while (1)
	{
		if (lIndexToMoveTo > LONG(ulDiePerBlk))
		{
			break;
		}
		if (lIndexToMoveTo > m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove))
		{
			break;
		}

		bLastLFDieResult = bLFDieResult;
		m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, lIndexToMoveTo, dX, dY, TRUE);

		if ( (lIndexToMoveTo <= 1) || !bLastLFDieResult)
		{
			bIsLFMove = FALSE;
			lTargetX = ConvertFileUnitToXEncoderValue(dX);
			lTargetY = ConvertFileUnitToYEncoderValue(dY);
		}
		else	//Use LF Result from last cycle
		{
			bIsLFMove = TRUE;
			lTargetX = lLFStepX;
			lTargetY = lLFStepY;
		}


		if (IsWithinTable1Limit(lTargetX, lTargetY))
		{
			XY_MoveTo(lTargetX, lTargetY);
			
			if ( (lBTDelay > 0) && (lBTDelay <= 1000) )
			{
				Sleep(lBTDelay);	//andrewng //2020-0626
			}
			else
			{
				Sleep(50);
			}

			if (!bIsLFMove)
			{
				szLog.Format("PBTest: Move To #%d - Step Move (%ld, %ld)", lIndexToMoveTo, lTargetX, lTargetY);
			}
			else
			{
				szLog.Format("PBTest: Move To #%d LF - ENC (%ld, %ld)", lIndexToMoveTo, lTargetX, lTargetY);
			}
HmiMessage(szLog);


			LONG lEncX = 0, lEncY = 0;
			GetXYEncoderValue(lEncX, lEncY);

			LONG lX = lEncX, lY = lEncY;
			CString szDieResult;
			bDieResult = pBondPr->BprSearchDie(lX, lY, szDieResult);

			if (!bIsLFMove)
			{
				//Do BT Compensation
				BT_MoveTo(lEncX + lX, lEncY + lY);
				
				if ( (lBTDelay > 0) && (lBTDelay <= 1000) )
				{
					Sleep(lBTDelay);	//andrewng //2020-0626
				}
				else
				{
					Sleep(50);
				}

				GetXYEncoderValue(lEncX, lEncY);
				szDieResult = szDieResult + "IsLFMove,0,";
			}
			else
			{
				szDieResult = szDieResult + "IsLFMove,1,";
			}


			if ( (lIndexToMoveTo < LONG(ulDiePerBlk)) && 
				 (lIndexToMoveTo < m_oBinBlkMain.GrabNVCurrBondIndex(m_ulBinBlkToStepMove)) )
			{
				//Calculate NEXT die encoder XY
				DOUBLE dX2 = 0, dY2 = 0;
				m_oBinBlkMain.StepDMove(m_ulBinBlkToStepMove, lIndexToMoveTo + 1, dX2, dY2, FALSE);
				LONG lTargetX2 = ConvertFileUnitToXEncoderValue(dX2);
				LONG lTargetY2 = ConvertFileUnitToYEncoderValue(dY2);
				
				LONG lDir = 0;
				DOUBLE dDiff = (DOUBLE) labs(lTargetX2 - lTargetX);
				DOUBLE dPitchTolX = 0.5 * lPitchX;
				
				if (dDiff > dPitchTolX)		//Next Die is moving LEFT or RIGHT
				{
					if (lTargetX2 > lTargetX)	//Next Die on LEFT
					{
						lDir = 1;	//BPR_LT_DIE
						szLog2 = "LEFT";
					}
					else						//Next Die on RIGHT
					{
						lDir = 5;	//BPR_RT_DIE
						szLog2 = "RIGHT";
					}
				}
				else						//Next Die is moving UP or DOWN
				{
					if (lTargetY2 > lTargetY)	//Next Die on UP
					{
						lDir = 3;	//BPR_UP_DIE
						szLog2 = "UP";
					}
					else						//Next Die on DOWN
					{
						lDir = 7;	//BPR_DN_DIE
						szLog2 = "DOWN";
					}
				}

				lX = lEncX;
				lY = lEncY;
				bLFDieResult = pBondPr->BprSearchLFDie(TRUE, lDir, !bIsLFMove, szDieResult, lX, lY);
				
				szLog.Format("PBTest: LF Dir = %s, Result=%d", szLog2, bLFDieResult);
HmiMessage(szLog);

				if (bLFDieResult)
				{
					lLFStepX = lEncX + lX;
					lLFStepY = lEncY + lY;
				}
				else
				{
					lLFStepX = 0;
					lLFStepY = 0;
				}
			}
			else
			{
				//If no LF needed, just log current Die result
				CMSLogFileUtility::Instance()->BT_PostBondLog(szDieResult);
				bLFDieResult = FALSE;
				lLFStepX = 0;
				lLFStepY = 0;
			}
		}

		lIndexToMoveTo++;
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBinTable::RotateBinAngularDie(IPC_CServiceMessage &svMsg)
{
	typedef struct {
		LONG	lX;
		LONG	lY;
		LONG	lT;
		DOUBLE	dAngle;
	} ABSPOS;

	ABSPOS	stPos;
	LONG	lX, lY, lTheta;
	DOUBLE	dDegree;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(ABSPOS), &stPos);

	lX		= stPos.lX;
	lY		= stPos.lY;
	lTheta	= stPos.lT;
	dDegree = stPos.dAngle;

	CalculateRotateBinTheta(&lX, &lY, &lTheta, dDegree, m_lBinCalibX, m_lBinCalibY);
	//lTheta = -1 * lTheta;

	//X_Profile(LOW_PROF);
	//Y_Profile(LOW_PROF);

	CString szMsg;
	szMsg.Format("BT MV_T %d,%d by %f to %d,%d,T%d", stPos.lX, stPos.lY, dDegree, lX, lY, lTheta);
	CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);

	XY_MoveTo(lX, lY, SFM_NOWAIT);
	if (lTheta != 0)
	{
		T_MoveTo(lTheta, SFM_NOWAIT);
		//T_MoveByDegree(dDegree, SFM_NOWAIT);
	}

	X_Sync();
	Y_Sync();
	if (lTheta != 0)
	{
		T_Sync();
	}

	//X_Profile(NORMAL_PROF);
	//Y_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::FindCenterOfRotation(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szTitle, szDegree, szCycle;
	DOUBLE dDegree = 0;
	LONG lCycle = 0;

	if (!m_bEnable_T || !T_IsPowerOn() || !m_bSel_T)
	{
		HmiMessage("Test is aborted because T is not enabled");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	XY_MoveTo(m_lBinCalibX, m_lBinCalibY);
	T_Home();

	szTitle = "Please input Angle (Degree)";
	if (!HmiStrInputKeyboard(szTitle, szDegree))
	{
		HmiMessage("Test is aborted");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	dDegree = atof(LPCTSTR(szDegree));

	szTitle = "Please input number of rotation cycle";
	if (!HmiStrInputKeyboard(szTitle, szCycle))
	{
		HmiMessage("Test is aborted");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	lCycle = atoi(LPCTSTR(szCycle));
	
	if (dDegree > 360 || dDegree < -360)
	{
		dDegree = 360;
	}

	for (int j = 0; j < lCycle; j++)
	{
		LONG lSearchFailCounter = 0;
		for (int i = 0; i <= labs(360/dDegree); i++)
		{
			Sleep(100);
			if (i > 0)
			{
				T_MoveByDegree(dDegree);
			}
			Sleep(100);

			GetEncoderValue();
			CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
			if (!pBondPr->BprSerachDieOnCenterOfRotation(m_lEnc_T, (dDegree * i)))
			{
				lSearchFailCounter++;
			}
			else
			{
				lSearchFailCounter = 0;
			}

			if (lSearchFailCounter > 5)
			{
				HmiMessage("Search Die Fail 5 times!", "Search Fail");
				goto BreakPoint;
			}
		}

		T_Home();
		Sleep(100);
	}

	BreakPoint:

	Sleep(1000);
	T_Home();
	HmiMessage("Test is done.");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::SelectBinRowColFromTempFile(IPC_CServiceMessage &svMsg)		//andrewng //2020-0630
{
	BOOL bReturn = TRUE;
	CString szMsg;

	//szMsg.Format("SelectBinRowColFromTempFile - (%ld, %ld)", m_lTempFileBinCol, m_lTempFileBinRow);
	//HmiMessage(szMsg);


	if (m_oBinBlkMain.GrabNVNoOfBondedDie(1) <= 0)
	{
		HmiMessage_Red_Yellow("ERROR: Bin #1 has NO bin count; please check again.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CStdioFile oFile;
	CString szTempFileName = gszEXE_DIRECTORY + "\\Blk01TempFile.csv";

	if (_access(szTempFileName, 0) == -1)
	{
		HmiMessage_Red_Yellow("ERROR: Bin #1 TEMP file (Blk01TempFile.csv) is not found in EXE folder; please check again.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BOOL bStatus = oFile.Open(szTempFileName,
							CFile::modeRead | CFile::shareExclusive | CFile::typeText);
	if (!bStatus)
	{
		HmiMessage_Red_Yellow("ERROR: Bin #1 TEMP file cannot be open!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	CString szLine, szBinCol, szBinRow;
	CString szMapEncX, szMapEncY, szBTEncX, szBTEncY;
	LONG lBinCol = 0, lBinRow = 0;
	LONG lMapEncX = 0, lMapEncY = 0, lBTEncX = 0, lBTEncY = 0;
	INT nLine;
	INT nCount = 0;


	while (oFile.ReadString(szLine))
	{
		nCount++;

		nLine = szLine.Find(",");
		if (nLine == -1)
		{
			continue;
		}

		//1. Remove MapName from LINE
		szLine	= szLine.Mid(nLine + 1);
		
		nLine	= szLine.Find(",");
		if (nLine == -1)
		{
			continue;
		}

		//2. Remove MapCol from LINE
		szLine	= szLine.Mid(nLine + 1);
		nLine	= szLine.Find(",");
		if (nLine == -1)
		{
			continue;
		}

		//3. Remove MapRow from LINE
		szLine	= szLine.Mid(nLine + 1);
		nLine	= szLine.Find(",");
		if (nLine == -1)
		{
			continue;
		}

		//4. Acquire Bin COL
		szBinCol	= szLine.Left(nLine);
		szLine		= szLine.Mid(nLine + 1);

		nLine	= szLine.Find(",");
		if (nLine == -1)
		{
			continue;
		}

		//5. Acquire Bin ROW
		szBinRow	= szLine.Left(nLine);
		szLine		= szLine.Mid(nLine + 1);

		lBinCol = atol((LPCTSTR) szBinCol);
		lBinRow = atol((LPCTSTR) szBinRow);

		if ( (lBinCol != m_lTempFileBinCol) || (lBinRow != m_lTempFileBinRow) )
		{
			continue;
		}


		szMsg.Format("Bin COL-ROW found = (%ld, %ld); line = %s", lBinCol, lBinRow, szLine);
		HmiMessage(szMsg);

		nLine	= szLine.Find(",");
		if (nLine != -1)
		{
			//6. Remove Bin Grade 
			szLine = szLine.Mid(nLine + 1);

			//szMsg.Format("Line = %s", szLine);
			//HmiMessage(szMsg);

			//7. Acquire Map Enc X 
			nLine = szLine.Find(",");
			szMapEncX = szLine.Left(nLine);
			lMapEncX = atol((LPCTSTR) szMapEncX);
			
			szLine	= szLine.Mid(nLine + 1);

			//7. Acquire Map Enc Y 
			nLine = szLine.Find(",");
			if (nLine == -1)
			{
				lMapEncY = atol((LPCTSTR) szLine);

				m_lTempFileBTEncX = 0;
				m_lTempFileBTEncY = 0;
			}
			else
			{
				szMapEncY	= szLine.Left(nLine);
				lMapEncY	= atol((LPCTSTR) szMapEncY);

				szLine	= szLine.Mid(nLine + 1);

				//szMsg.Format("Line for BT = %s", szLine);
				//HmiMessage(szMsg);

				nLine = szLine.Find(",");
				szBTEncX = szLine.Left(nLine);
				lBTEncX = atol((LPCTSTR) szBTEncX);

				szLine	= szLine.Mid(nLine + 1);
				lBTEncY = atol((LPCTSTR) szLine);

				m_lTempFileBTEncX = lBTEncX;
				m_lTempFileBTEncY = lBTEncY;
			}

			m_lTempFileWTEncX = lMapEncX;
			m_lTempFileWTEncY = lMapEncY;

			szMsg.Format("Bin COL-ROW (%ld, %ld) found with WT ENC (%ld, %ld), BT ENC (%ld, %ld)", 
				lBinCol, lBinRow, m_lTempFileWTEncX, m_lTempFileWTEncY, m_lTempFileBTEncX, m_lTempFileBTEncY);
			HmiMessage(szMsg);
		}

		break;
	}

	oFile.Close();

	szMsg.Format("Bin COL-ROW (%ld, %ld) is selected; Line COUNT = %d", 
						m_lTempFileBinCol, m_lTempFileBinRow, nCount);
	HmiMessage(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBinTable::BTMoveFromTempFile(IPC_CServiceMessage &svMsg)		//andrewng //2020-0630
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

	if (IsWithinTable1Limit(stInfo.lEncX, stInfo.lEncY))
	{
		szMsg.Format("BTMoveFromTempFile: BT Move To (%ld, %ld) (%ld, %ld)", 
							stInfo.lEncX, stInfo.lEncY, m_lTempFileBTEncX, m_lTempFileBTEncY);
		HmiMessage(szMsg);

		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);

		if (X_IsPowerOn() && Y_IsPowerOn())		
		{
			XY_MoveTo(m_lTempFileBTEncX, m_lTempFileBTEncY, SFM_WAIT);	
		}

		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
	}
	else
	{
		szMsg.Format("ERROR: BTMoveFromTempFile: BT Move To (%ld, %ld) fail !!", 
						m_lTempFileBTEncX, m_lTempFileBTEncY);
		HmiMessage(szMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//andrewng //2020-0727
LONG CBinTable::MoveTableToDummyPos(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;

	BOOL bToDummy = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bToDummy);

	LONG lX = -95630;
	LONG lY = -53210;

	if (bToDummy)
	{
		GetEncoderValue();
		m_lBackupDummyPosX = m_lEnc_X;
		m_lBackupDummyPosY = m_lEnc_Y;

		szMsg.Format("BinTable: BT Move To DUMMY Pos (%ld, %ld)", lX, lY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//HmiMessage(szMsg);

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
		szMsg.Format("BinTable: BT DUMMY Move To backup Pos (%ld, %ld)", m_lBackupDummyPosX, m_lBackupDummyPosY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);

		if (IsWithinTable1Limit(m_lBackupDummyPosX, m_lBackupDummyPosY))
		{
			if (X_IsPowerOn() && Y_IsPowerOn())		
			{
				XY_MoveTo(m_lBackupDummyPosX, m_lBackupDummyPosY, SFM_WAIT);	
			}
		}
		else
		{
			szMsg.Format("BinTable: BT DUMMY Move To backup Pos (%ld, %ld) FAIL", m_lBackupDummyPosX, m_lBackupDummyPosY);
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

LONG CBinTable::MoveToUnload(IPC_CServiceMessage& svMsg)
{
	BOOL bUnload	= TRUE;
	BOOL bReturn	= TRUE;

	BOOL bOpen = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOpen);

	if (!m_fHardware || m_bDisableBT)
	{		
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//svMsg.GetMsg(sizeof(BOOL), &bUnload);
	if (!X_IsPowerOn() || !Y_IsPowerOn() || !T_IsPowerOn())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (bOpen)
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);

		//if (IsWithinWaferLimit(m_lUnloadPhyPosX, m_lUnloadPhyPosY) == TRUE)
		//{
		//XY_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
		T_MoveTo(m_lUnloadPhyPosT, SFM_NOWAIT);
		X_MoveTo(m_lUnloadPhyPosX);
		Y_MoveTo(m_lUnloadPhyPosY);
		//}

		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
	}
	else
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
		
		//When UNLOAD, Y should move DOWN first to avoid collision with Loader
		Y_MoveTo(0, SFM_NOWAIT);
		Sleep(50);
		X_MoveTo(0, SFM_NOWAIT);
		Sleep(10);
		//XY_MoveTo(0, 0, SFM_WAIT);
		T_MoveTo(0, SFM_NOWAIT);
		Y_Sync();
		X_Sync();
		
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
		//Y_Home();
		//X_Home();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBinTable::SetUnloadPosition(IPC_CServiceMessage& svMsg)
{
	if (!m_fHardware || m_bDisableBT)	
	{	
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	//SetJoystickOn(FALSE);
	GetEncoderValue();

	m_lUnloadPhyPosX	= m_lEnc_X;
	m_lUnloadPhyPosY	= m_lEnc_Y;
	m_lUnloadPhyPosT	= m_lEnc_T;

	SaveBinTableData();
	
	HmiMessage("BT UNLOAD XY is updated; table will move back to HOME.");

	Y_Home();
	X_Home();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}




