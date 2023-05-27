/////////////////////////////////////////////////////////////////
// BT_Common.cpp : Common functions of the CBinTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, November 25, 2004
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
#include "BinTable.h"
#include "LblPtr.h"
#include "PtrFactory.h"
#include "Printer.h"
#include "FlushMessageThread.h"
#include "BinLoader.h"
#include "WaferPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CBinTable::InitVariable(VOID)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_szEventPrefix				= "BT";

	m_ulBondedBlk				= 0;	
	m_bMachineFirstStart		= TRUE;

	m_bHomeSnr_X				= FALSE;
	m_bHomeSnr_Y				= FALSE;
	m_bPosLimitSnr_X			= FALSE;
	m_bNegLimitSnr_X			= FALSE;
	m_bPosLimitSnr_Y			= FALSE;
	m_bNegLimitSnr_Y			= FALSE;

	//m_pServo_X					= NULL;
	//m_pServo_Y					= NULL;
	m_nBTInUse					= 0;		//0=DEFAULT, 1=BT2

	m_bSel_X					= TRUE;
	m_bSel_Y					= TRUE;
	m_bSel_T					= FALSE;
	m_bSel_X2					= TRUE;
	m_bSel_Y2					= TRUE;

	m_lEnc_X					= 0;
	m_lEnc_Y					= 0;
	m_lEnc_T					= 0;
	m_lEnc_X2					= 0;
	m_lEnc_Y2					= 0;
	m_lCmd_X					= 0;
	m_lCmd_Y					= 0;
	m_lCurPos_T					= 0;		//v4.59A33
	m_lLastX					= 0;
	m_lLastY					= 0;

	m_lBlocksDisplayResol	    = 800; //for wafer map control
      
	m_bIsPowerOn_X				= TRUE;
	m_bIsPowerOn_Y				= TRUE;
	m_bIsPowerOn_T				= TRUE;
	m_bIsPowerOn_X2				= TRUE;
	m_bIsPowerOn_Y2				= TRUE;

	m_bUpdateOutput				= FALSE;
	m_bHome_X					= FALSE;
	m_bHome_Y					= FALSE;
	m_bHome_T					= FALSE;
	m_bComm_X					= FALSE;
	m_bComm_Y					= FALSE;
	m_bComm_T					= FALSE;
	m_bHome_X2					= FALSE;
	m_bHome_Y2					= FALSE;
	m_bComm_X2					= FALSE;
	m_bComm_Y2					= FALSE;
	
	m_lX_ProfileType			= 0;
	m_lY_ProfileType			= 0;

	//v4.49A9	//Bin Table T Calibration
	m_lBinCalibX				= 0;
	m_lBinCalibY				= 0;
	m_lNGPickPocketX			= 0;
	m_lNGPickPocketY			= 0;
	m_bStartCalibrate			= FALSE;
	m_bIsRotate180Deg			= FALSE;
	m_bIsRotate180DegHMI		= FALSE;
	m_bMS90RotatedBT			= FALSE;

	m_bCreated					= FALSE;	//for physical blocks map
	m_bBinBlkCreated			= FALSE;	//for bin blocks map

	m_bUseLargeBinArea			= FALSE;
	m_bUseDualTablesOption		= FALSE;
	m_bEnable_T					= FALSE;
	m_bUseTEncoder				= FALSE;

	m_bUseFrameCrossAlignment	= FALSE;	//v4.59A1

	m_dXResolution_UM_CNT	= GetChannelResolution(MS896A_CFG_CH_BINTABLE_X) * 1000;	
	m_dYResolution_UM_CNT	= GetChannelResolution(MS896A_CFG_CH_BINTABLE_Y) * 1000;

	m_dXResolution	= 1.0 / m_dXResolution_UM_CNT;	
	m_dYResolution	= 1.0 / m_dYResolution_UM_CNT;

	m_dThetaRes				= GetChannelResolution(MS896A_CFG_CH_BINTABLE_T);
	m_lThetaMotorDirection	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_T, MS896A_CFG_CH_MOTOR_DIRECTION);

	//m_BT_T_RESOLUTION = IsBinTableTHighResolution() ? BT_T_HIGHT_RESOLUTION : BT_T_RESOLUTION;
	m_BT_T_RESOLUTION = BT_T_RESOLUTION;

	m_lMinTravel_X = (LONG)( (0.5 / GetChannelResolution(MS896A_CFG_CH_BINTABLE_X)) * 1.05 );
	m_lMinTravel_Y = (LONG)( (0.5 / GetChannelResolution(MS896A_CFG_CH_BINTABLE_Y)) * 1.05 );

	m_lXNegLimit = 0; 
	m_lXPosLimit = 0; 
	m_lYNegLimit = 0;
	m_lYPosLimit = 0; 

	m_lMotorTestPt1 = 0;
	m_lMotorTestPt2 = 0;
	m_ulMotorTestRepeatDelay = 0;
	m_ulMotorTestRepeatCycles = 0;
	m_ulTestRunDirection = 0;
	m_ulTestRunRepeatDelay = 0;
	m_ulTestRunRepeatCycles = 0;

	m_ulTotalSortedFromAllGrades = 0;

	/******************************/
	/*     HMI Reg Variables      */
	/******************************/
	m_bReply = FALSE;

	//Enability of Hmi Controls
	m_bIfEnableTBEditBlkSettings = FALSE;
	m_bIfEnableTBClrBlkSettings = FALSE;

	m_bIfEnableCBClrBlkSettingsSelection = FALSE;
	m_bIfEnablePIBlkToClrSettings = FALSE;
	m_bIfEnableTBSubmitClrBlkSettings = FALSE;

	m_bIfEnableCBSetupMode = FALSE;
	m_bIfEnablePINoOfBlk = FALSE;  
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
	m_bIfEnablePIGrade = FALSE;
	m_bIfEnablePIDiePitchX = FALSE;
	m_bIfEnablePIDiePitchY = FALSE;
	m_bIfEnableCBWalkPath = FALSE;
	m_bIfEnableChkBAutoAssignGrade = FALSE;
	m_bIfEnable2DBarcodeOutput = FALSE;			//v3.33T3
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
	m_bIfEnableWafflePadPageBack = FALSE;

	m_bIfUseBlockCornerAsFirstDiePos = FALSE;
	m_bIfEnableTBSubmit = FALSE;
	m_bIfEnableTBCancel = FALSE;

	//BinTable Limits & unload position
	m_lTableXNegLimit	= 0;
	m_lTableXPosLimit	= 0;
	m_lTableYNegLimit	= 0;
	m_lTableYPosLimit	= 0;

	m_lTableUnloadX2Pos	= 0;
	m_lTableUnloadY2Pos	= 0;
	m_lTableX2NegLimit	= 0;
	m_lTableX2PosLimit	= 0;
	m_lTableY2NegLimit	= 0;
	m_lTableY2PosLimit	= 0;

	//Clear Block Settings
	m_szClrBlkSettingsSelection = "Single Block";

	//Clear Physical Block Settings
	m_ulPhyBlkToClrSettings = 1;

	//Clear Bin Block Settings
	m_ulBinBlkToClrSettings = 1;

	//Block Setup
	m_szSetupMode = "Single Block";
	m_bFirstTimeSetUL = TRUE;
	m_bFirstTimeSetLR = TRUE;

	//Physical Block Setup
	m_ulNoOfPhyBlk = 0;
	m_ulPhyBlkToSetup = 0;
	m_lPhyBlkPitchX = 0;
	m_lPhyBlkPitchY = 0;
	m_lPhyUpperLeftX = 0;
	m_lPhyUpperLeftY = 0;
	m_lPhyLowerRightX = 0;
	m_lPhyLowerRightY = 0;
	
	//Bin Block Setup

	//m_bSumbitAndSaveAsTemplate = FALSE;
	m_szBinBlkTemplate = "";
	m_szBinBlkTemplateSel = "";
	m_ulTemplateSetupInputCount = 0;
	m_ulNoOfBinBlk = 0;
	m_ulBinBlkToSetup = 0;
	m_lBinBlkPitchX = 0;
	m_lBinBlkPitchY = 0;
	m_lBinUpperLeftX = 0;
	m_lBinUpperLeftY = 0;
	m_lBinLowerRightX = 0;
	m_lBinLowerRightY = 0;
	m_ucGrade		= 1;
	m_dDiePitchX	= 500;
	m_dDiePitchY	= 500;
	m_bUsePt5UmInDiePitchX	= FALSE;
	m_bUsePt5UmInDiePitchY	= FALSE;
	m_dBondAreaOffsetXInUm	= 0;
	m_dBondAreaOffsetYInUm	= 0;
	m_dThermalDeltaPitchXInUm = 0;		//v4.59A22	//David Ma
	m_dThermalDeltaPitchYInUm = 0;
	m_szWalkPath = "";
	m_ulWalkPath = 0;
	m_ulDiePerBlk = 0;
	m_ulDiePerRow = 0;
	m_ulDiePerCol = 0;
	m_ulSkipUnit = 0;	
	m_ulMaxUnit = 0;
	m_bEnableFirstRowColSkipPattern = FALSE;
	m_ulFirstRowColSkipUnit = 0;
	m_bNoReturnTravel	= FALSE;
	m_bByPassBinMap		= FALSE;
	// Temp to disable it
	m_bTeachWithPhysicalBlk = FALSE;
	m_bCentralizedBondArea = FALSE;
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		m_bEnableFirstRowColSkipPattern = FALSE;
		m_ulFirstRowColSkipUnit			= 0;
		m_bCentralizedBondArea			= TRUE;
		//m_bTeachWithPhysicalBlk			= TRUE;
		//m_bNoReturnTravel				= FALSE;
	}
	m_lCentralizedOffsetX = 0;
	m_lCentralizedOffsetY = 0;
	m_lCentralizedUpperLeftX = 0;
	m_lCentralizedUpperLeftY = 0;
	m_bEnableWafflePad = FALSE;	
	m_lWafflePadDistX = 0;
	m_lWafflePadDistY = 0;
	m_lWafflePadSizeX = 0;
	m_lWafflePadSizeY = 0;
	m_dSpeedModeOffsetX = 0;
	m_dSpeedModeOffsetY = 0;
	m_lBHZ2BondPosOffsetX	= 0;
	m_lBHZ2BondPosOffsetY	= 0;
	m_lBHZ1BondPosOffsetX	= 0;
	m_lBHZ1BondPosOffsetY	= 0;
	m_lBinBarcodeMinDigit	= 0;
	m_lBHZ1PrePickToBondOffsetX	= 0;
	m_lBHZ1PrePickToBondOffsetY	= 0;
	m_lBHZ2PrePickToBondOffsetX	= 0;
	m_lBHZ2PrePickToBondOffsetY	= 0;

	m_dBond180DegOffsetX_um = 0;
	m_dBond180DegOffsetY_um = 0;
	//v3.71T1	//PLLM REBEL 
	m_bEnableCircularBondArea	= FALSE;
	m_lCirAreaCenterX			= 0;
	m_lCirAreaCenterY			= 0;
	m_dCirAreaRadius			= 0;
	m_bEnableBinMapBondArea		= FALSE;	//v4.03
	m_bEnableBinMapCDieOffset	= FALSE;	//v4.42T1
	m_dBinMapCircleRadiusInMm	= 0.00;		//v4.36
	m_lBinMapXOffset			= 0;
	m_lBinMapYOffset			= 0;
	m_lBinMapROffset			= 0;
	m_szBinMapFilePath			= _T("");
	m_ucSpGrade1ToByPassBinMap	= 0;
	m_ucBinMixTypeAGrade		= 0;
	m_ucBinMixTypeBGrade		= 0;

	//v4.47T13	
	m_bEnableOsramBinMixMap		= FALSE;
	m_lBinMixSetQty				= 0;
	m_ulBinMixPatternType		= 0;
	m_ucBinMixTypeAGrade		= 0;
	m_ucBinMixTypeBGrade		= 0;
	m_bBinMixOrder				= 0;
	m_lBinMixCount				= 0;
	m_lBinMixLessCommonType     = 0;
	m_lBinMixCountTemp			= 0;
	m_lBinMixTypeAQty           = 0;
	m_lBinMixPatternQty			= 0;
	m_lBinMixTypeBQty           = 0;
	m_lBinMixTypeAStopPoint     = 0;
	m_lBinMixTypeBStopPoint     = 0;
	m_bUseBlockCornerAsFirstDiePos = FALSE;

	m_lBTNoDiePosX				= 0;
	m_lBTNoDiePosY				= 0;

	m_bAutoAssignGrade = TRUE;
	m_bAssignSameGradeForAllBin = FALSE;

	m_ucStartingGrade = 1;
	m_ulSourcePhyBlk = 1;
	m_ulBinSerialNoFormat = 1;
	m_ulBinSerialNoMaxLimit = 9999;
	m_ulBinClearFormat = 0;
	m_ulBinClearMaxLimit = 99;
	m_ulBinClearInitCount = 1;
	m_bSaveBinBondedCount = FALSE;	
	m_bOptimizeBinGrade = FALSE;
	m_bLSBondPattern = FALSE;

	m_szPKGFilenameDisplay = "";
	m_ulPKGDiePitchXDisplay = 0;
	m_ulPKGDiePitchYDisplay = 0;
	m_ulPKGInputCountDisplay = 0;
	m_ulPKGDiePerRowDisplay = 0;
	m_ulPKGDiePerColDisplay = 0;
	m_ulPKGBlkCapacityDisplay = 0;
	m_ulPKGNoOfBinBlkDisplay = 0;

	//Bin Grade Data
	m_ulGradeCapacity = 0;
	m_ucAliasGrade = 0;

	//Clear Bin Counter (by Physical Block)
	m_szClrBinCntMode = "Single Block";
	m_bIfIsClrAllBinCtr = FALSE;

	m_ulPhyBlkToClrCount = 1;
	m_bIfGenOutputFile = TRUE; 
	m_bGenOFileIfClrByGrade = TRUE;
	m_bUseUniqueSerialNum	= FALSE;
	m_bUseMultipleSerialCounter = FALSE;
	m_szFileNo = "";
	m_szBlueTapeNo = "";
	m_szSpecVersion = "";

	m_szBinOutputFileFormat = "";
	m_szOutputFilename = "";
	m_szLabelFilePath = gszROOT_DIRECTORY + _T("\\OutputFiles");

	//Clear Bin Counter (by Grade)
	m_ucGradeToClrCount = 1;
	m_szClrBinByGradeMode = "Single Grade";

	m_bIsClrAllGrades = FALSE;

	//Step Move
	m_ulTableToStepMove = 0;	//0=BT1, 1=BT2
	m_ulBinBlkToStepMove = 1;
	m_ulCurrBondIndex = 0;
	m_ulNoOfBondedDie = 0;
	m_lTempIndex = 0;
	m_ulHowManySteps = 0;
	m_szStepMoveMsg = "Testing";

	//Grade Information
	m_ulNoOfSortedDie = 0;
	m_ulBinBlkInUse = 1;
	m_ulSortedDieIndex = 0;
	m_ulGradeCapacity = 0;
	m_ulInputCount = 0;

	m_ulFullDieCountInput = 0;
	m_ulMinDieCountInput = 0;
	m_bEnableDieCountInput = FALSE;
	m_ucAliasGrade = 1;
	m_bEnableAliasGrade = FALSE;

	//SDS map information
	m_bSPCStatus = FALSE;		
	m_dSPCAverageAngle = 0.0;
	m_lSPCTotalDieCount = 0;
	m_lSPCGoodDieCount = 0;

	//The Reference Cross (Citizen)
	m_lEncRefCross_X = 0;
	m_lEncRefCross_Y = 0;
	m_lEncRefCross_T = 0;
	m_dBinTwoCrossXDistance = -1.0;
	m_dBinTwoCrossYDistance = -1.0;
	m_dThetaOffsetByCross	= 0.0;

	//Collet Offset Information;
	m_lColletOffsetX		= 0;
	m_lColletOffsetY		= 0;

	m_lBTDelay				= 20;			// BinTable Delay for long distance
#ifdef NU_MOTION
	m_lBTDelay_Short		= 0;			// DBH BinTable Delay for short distance
#else
	m_lBTDelay_Short		= 20;			// BinTable Delay for short distance
#endif
	m_lTime_BT				= 0;			// Bin Table motion time
	m_lX					= 0;
	m_lY					= 0;
	m_bFirstDie				= TRUE;
	m_nMultiSeachCounter	= 0;		//4.51D20
	m_nMultiUnfondCounter	= 1;		//4.51D20b
	m_lDoubleCheckFirstDie =  0;
	m_lTempMultiSearchX		= 0;
	m_lTempMultiSearchY		= 0;

	m_nProfile_X			= NORMAL_PROF;
	m_nProfile_Y			= NORMAL_PROF;
	m_nProfile_X2			= NORMAL_PROF;
	m_nProfile_Y2			= NORMAL_PROF;

	//v4.21	//BT2 parameters	//v4.37
	//m_lBT2OffsetX			= 0;
	//m_lBT2OffsetY			= 0;

	m_lJoystickLevel		= 1;		// Medium speed
	m_bJoystickOn			= TRUE;		// Joystick ON

	m_bIsAllBinCntCleared	= TRUE;
	m_bStopBinLoader		= FALSE;
	m_bIsSetupBinBlk		= FALSE;

	m_ulBinBlkFull			= 0;
	m_bIsUnload				= FALSE;
	m_bAutoClearBin			= FALSE;
	m_bIfPrintLabel			= FALSE;
	m_bUseBarcodeAsFileName	= FALSE;

	m_bUseSameView			= FALSE;
	m_bAlignBinInAlive		= TRUE;

	m_bFilenameAsWaferID	= FALSE;

	m_lAlignBinDelay = 600;
	m_dAlignLastDieOffsetX		= 3;		//v2.63
	m_dAlignLastDieOffsetY		= 3;		//v2.63
	m_dAlignLastDieRelOffsetX	= 0;
	m_dAlignLastDieRelOffsetY	= 0;	
	
	m_bPreLoadFullFrame		= FALSE;		//v2.67
	m_bPreLoadChangeFrame	= FALSE;		//v2.67
	m_bDisableClearAllPrintLabel= FALSE;	//v2.70
	m_bCheckAlignBinResult	= TRUE;
	m_bManualConfirmLastDieResult1	= FALSE;
	m_bManualConfirmLastDieResult2	= FALSE;
	m_lManualConfirmXOffset = 0;
	m_lManualConfirmYOffset = 0;
	m_bPreLoadNextMapGradeFrame = FALSE;

	m_bShowBinBlkTemplateSetup = FALSE;		//v4.42T8

	for (INT i=0; i<BT_EXTRA_BIN_INFO_ITEM_NO; i++)
	{
		m_szaExtraBinInfo[i] = "N/A";
		m_szaExtraBinInfoField[i].Format("Extra Info %d", i+1);
		m_bEnableExtraBinInfo[i] = FALSE;
	}


	m_bWaferEndUploadMapMpd		= FALSE;
	m_bEnableBinOutputFile		= FALSE;
	m_bFinForBinOutputFile		= FALSE;
	m_bEnableBinMapFile			= FALSE;
	m_bFin2ForBinMapFile		= FALSE;
	m_bEnable2DBarcodeOutput	= FALSE;	//v3.33T3
	m_ulHoleDieNum				= 3;		//v3.33T3
	m_lRealignBinFrameOption	= 0;		//0 = DEfault 3-pt alignment, 1= 1-pt alignment, 2=2-pt alignment	//v3.70T3		
	m_bPrPreBondAlignment		= FALSE;	//v3.79
	m_b1ptRealignComp			= FALSE;	//v3.86		//Available for 1pt realignment only
	//CSP
	m_lPreBondTableOffsetX		= 0;
	m_lPreBondTableOffsetY		= 0;
	m_lCollet1PadOffsetX		= 0;
	m_lCollet1PadOffsetY		= 0;
	m_lCollet2PadOffsetX		= 0;
	m_lCollet2PadOffsetY		= 0;

	m_bEnableAutoLoadRankFile = FALSE;
	m_bLoadRankIDFromMap	  = FALSE;
	m_bAutoBinBlkSetupWithTempl = FALSE;
	m_szRankIDFilename = "";
	m_szRankIDFilePath = "";

	//v4.47T5
	m_bIsMotionTestBinTableX	= FALSE;
	m_bIsMotionTestBinTableX2	= FALSE;
	m_bIsMotionTestBinTableY	= FALSE;
	m_bIsMotionTestBinTableY2	= FALSE;

	m_lGenSummaryPeriodNum = 0;
	m_szLastGenSummaryTime = "";

	m_szBinLotSummaryFilename = "";
	m_szBinLotSummaryPath = "";
	m_szBinLotSummaryPath2= "";	
	
	m_szInputCountSetupFilename = "";
	m_szInputCountSetupFilePath= "";


	for (INT i=0; i<BT_SUMMARY_TIME_NO; i++)
	{
		m_szBinSummaryGenTime[i] = "";
	}

	m_ucOptimizeBinCountMode = '0';
	m_ulEnableOptimizeBinCount = FALSE;
	m_bEnableBinSumWaferLotCheck = FALSE;
	m_ucOpimizeBinSetupGrade = 1;
	m_ucOpimizeBinSetupGradeLimit = 1;
	m_ulMinFrameDieCount = 0;
	m_ulMaxFrameDieCount = 0;
	m_ulMinLotCount		 = 0;
	
	m_bTestRealign	= FALSE;		//v3.70T4

	m_lBpX = 0;
	m_lBpY = 0;

	//Offline Postbond Test		//v4.11T1	//Lumileds
	m_lStartBondIndex	= 1;	
	m_lCurrBondIndex	= 1;
	m_lNoOfPostBondDices= 0;
	m_lCurrBlkID		= 0;
	m_bOfflinePostBondTest = FALSE;
	m_bStopOfflinePostBondTest = FALSE;

	m_bNotGenerateWaferEndFile = TRUE;
	m_dWaferEndSortingYield =0.0;

	// Auto Clean Collet
	m_lACCLiquidX	= 0;
	m_lACCLiquidY	= 0;
	m_lACCBrushX	= 0;
	m_lACCBrushY	= 0;
	m_lACCRangeX	= 1000;
	m_lACCRangeY	= 50;

	m_lACCCycleCount= 5;

	m_lACCMatrixRow	= 1;
	m_lACCMatrixCol	= 1;
	m_lACCAreaSizeX	= 1000;
	m_lACCAreaSizeY	= 50;
	m_lACCAreaLimit	= 50;
	m_lACCAreaCount	= 0;
	m_lACCAreaIndex	= 1;
	m_lACCReplaceLimit = 50;
	m_lACCReplaceCount	= (m_lACCAreaIndex-1)*m_lACCAreaLimit + m_lACCAreaCount;

	m_lCleanDirtMatrixRow = 1;
	m_lCleanDirtMatrixCol = 1;
	m_lCleanDirtUpleftPosX = 0;
	m_lCleanDirtUpleftPosY = 0;
	m_lCleanDirtLowerRightPosX = 0;
	m_lCleanDirtLowerRightPosY = 0;
	m_lCleanDirtRowIndex = 0;
	m_lCleanDirtColIndex = 0;

	m_bACCToggleBHVacuum	= FALSE;
	m_lMS90MCCGarbageBinX	= 0;
	m_lMS90MCCGarbageBinY	= 0;
	//shiraishi02
	m_bCheckFrameOrientation= FALSE;
	m_lFrameOrientCheckX	= 0;
	m_lFrameOrientCheckY	= 0;

	m_bEnableBTMotionLog = FALSE;
	for (int i=1; i<=200; i++)
	{
		m_bBinStatus[i] = FALSE;
	}
	m_bValidOutputFilePath2 = FALSE;
	m_szOutputFilePath2		= "";
	m_szOutputFilePath3		= "";				//v4.44T2
	m_szOutputFilePath4		= "";				//v4.44T2
	m_bIsAllBinsEmptyStatus	= FALSE;			//v4.14T1	//Ubiliux
	m_bIfEnableResortDie = FALSE;
	m_bIfNeedUpdateBinFrameAngle	= FALSE;	//v4.44T2	//Semitek

	// Process Debug
	m_lBinTableDebugCounter = 0;
	//Nichia//v4.43T7
	m_bEnableNichiaOutputFile		= TRUE;
	m_bEnableNichiaOTriggerFile		= TRUE;
	m_bEnableNichiaRPTFile			= TRUE;

	//v4.43T8	//Jenoptic
	m_lMagSummaryYear		= 0;
	m_lMagSummaryMonth		= 0;
	m_lMagSummaryDay		= 0;

	m_bClearBinInCopyTempFileMode = FALSE;

	m_lCreeLastGenReportHour		= 0;
	m_lCreeStatisticsReportCounter	= 0;

	//v4.50A8	//AGC Fcns
	m_lChgColletClampPosX	= 0;
	m_lChgColletClampPosY	= 0;

	m_lChgColletHolderInstallPosX	= 0;
	m_lChgColletHolderInstallPosY	= 0;

	m_lChgColletHolderUploadPosX	= 0;
	m_lChgColletHolderUploadPosY	= 0;

	m_lChgColletPusher3PosX	= 0;
	m_lChgColletPusher3PosY	= 0;

	m_lChgColletUpLookPosX	= 0;
	m_lChgColletUpLookPosY	= 0;

	m_lChgColletUPLUploadPosX = 0;
	m_lChgColletUPLUploadPosY = 0;

	m_dFrameAlignAngleForTesting = 0;
	m_bNVRamUpdatedInIfBondOK = FALSE;		//Used in AUTOBOND cycle only

	m_bRealignBinFrameReq	= FALSE;
	m_ulRealignBinBlkID		= 0;
	m_bRealignEmptyFrame	= FALSE;
	m_bRealignUseBT2		= FALSE;

	m_qNextSubOperation = -1;

	//andrewng //2020-0630
	m_lTempFileBinCol	= 0;
	m_lTempFileBinRow	= 0;
	m_lTempFileWTEncX	= 0;
	m_lTempFileWTEncY	= 0;
	m_lTempFileBTEncX	= 0;
	m_lTempFileBTEncY	= 0;

	m_lBackupDummyPosX	= 0;
	m_lBackupDummyPosY	= 0;

	m_lUnloadPhyPosX	= 0;
	m_lUnloadPhyPosY	= 0;
	m_lUnloadPhyPosT	= 0;

	m_bDoOfflinePostbondTest = FALSE;
	m_bDoOfflinePostbondTest2 = FALSE;
	m_lIndexToMoveTo	= 0;
	m_lOffPBTestLFStepX = 0;
	m_lOffPBTestLFStepY = 0;
	m_bOffPBTestLFDieResult = FALSE;
	//andrewng //2020-0806
	m_ulErrMapNoOfRow	= 0;
	m_ulErrMapNoOfCol	= 0;
	m_dErrMapPitchX		= 0;
	m_dErrMapPitchY		= 0;
	m_ulErrMapDelay		= 0;
	m_ulErrMapCount		= 0;
	m_dErrMapSamplingTolX = 0;
	m_dErrMapSamplingTolY = 0;
	m_ulErrMapSamplingTolLimit = 0;
	m_dErrMapSlope		= 0;
	m_lErrMapRefX1		= 0;
	m_lErrMapRefY1	    = 0;
	m_lErrMapRefX1InUm	= 0;
	m_lErrMapRefY1InUm	= 0;
	m_lErrMapLimitX1	= 0;
	m_lErrMapLimitY1	= 0;
	m_lErrMapLimitX2	= 0;
	m_lErrMapLimitY2	= 0;
	m_lErrMapOffsetLimitX = 0;
	m_lErrMapOffsetLimitY = 0;
	m_ulMarksPrAccLimit	= 0;
	m_ulMarksPrConLimit	= 0;
}

BOOL CBinTable::UpdatedPKGDisplayRecord()
{
	UCHAR ucGrade = '0';
	CString szPitchXDisplay = "", szPitchYDisplay ="";

	m_szPKGFilenameDisplay	= (*m_psmfSRam)["MS896A"]["PKG Filename Display"];
	m_ulPKGDiePitchXDisplay = _round(m_oBinBlkMain.GrabDDiePitchX(1));
	m_ulPKGDiePitchYDisplay = _round(m_oBinBlkMain.GrabDDiePitchY(1));
	m_ulPKGInputCountDisplay = m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(1));
	m_ulPKGDiePerRowDisplay = m_oBinBlkMain.GrabNoOfDiePerCol(1);
	m_ulPKGDiePerColDisplay = m_oBinBlkMain.GrabNoOfDiePerRow(1);
	m_ulPKGBlkCapacityDisplay = m_oBinBlkMain.GrabNoOfDiePerBlk(1);
	m_ulPKGNoOfBinBlkDisplay = m_oBinBlkMain.GetNoOfBlk();

	SaveBinTableData();

	return TRUE;
}

BOOL CBinTable::PortableBinBlockSetup()
{
	if (IsLoadingPortablePKGFile() == TRUE && IsLoadingPKGFile() == TRUE)
	{		
		for (ULONG i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE-1);	//v4.51A20	//Klocwork

			UpdateBinBlkSetupParameters(i);
			
			if (SetupBinBlk("Single Block", FALSE, FALSE) == FALSE)
			{
				m_oBinBlkMain.ClrAllBlksSettings();
				m_oBinBlkMain.ClrAllGradeInfo();
				return FALSE;
			}

			if (m_oBinBlkMain.CheckAllBlockSize(i) == FALSE)
			{
				m_oBinBlkMain.ClrAllBlksSettings();
				m_oBinBlkMain.ClrAllGradeInfo();
				SetAlert_Red_Yellow(IDS_BT_FAIL_TO_FIT_INTO_PHY_BLK);
				return FALSE;
			}
		}

		
	}

	return TRUE;
}

BOOL CBinTable::BT_GetFrameLevel()
{
	BOOL bIsUp;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "GetFrameLevel" , stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bIsUp);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bIsUp;
}

VOID CBinTable::BackupTempFile(ULONG ulBlkId)
{
	if (CMS896AStn::m_bBackupTempFile == TRUE)
	{
		m_oBinBlkMain.BackupTempFile(ulBlkId);
	}
}

VOID CBinTable::BackupAllTempFile()
{
	if (CMS896AStn::m_bBackupTempFile == TRUE)
	{
		m_oBinBlkMain.BackupAllTempFiles();
	}
}

LONG CBinTable::CheckOutTableLimit(LONG lX, LONG lY)
{
	if (!m_bSel_X && !m_bSel_Y)			//v4.37T10
	{
		return 0;
	}

	if ( (lX > m_lTableXPosLimit) || (lX < m_lTableXNegLimit) )
	{
		return Err_BinTableOutXLimit;
	}

	if ( (lY > m_lTableYPosLimit) || (lY < m_lTableYNegLimit) )
	{
		return Err_BinTableOutYLimit;
	}

	return 0;
}

LONG CBinTable::CheckOutTable2Limit(LONG lX, LONG lY)
{
	if (!m_bSel_X2 && !m_bSel_Y2)		//v4.37T10
	{
		return 0;
	}

	if ( (lX > m_lTableX2PosLimit) || (lX < m_lTableX2NegLimit) )
	{
		return Err_BinTableOutXLimit;
	}

	if ( (lY > m_lTableY2PosLimit) || (lY < m_lTableY2NegLimit) )
	{
		return Err_BinTableOutYLimit;
	}

	return 0;
}


BOOL CBinTable::LearnColletOffset(VOID)
{
	BOOL bManualMode = TRUE;	
	ULONG ulBlock = 0;
	ULONG ulIndex = 0;
	ULONG i = 1;
	LONG lX =0 , lY = 0;
	DOUBLE dX=0, dY=0;
	CString szTitle, szContent;


	for (i=1; i<=MS_MAX_BIN; i++)
	{
		ulIndex = m_oBinBlkMain.GrabNVCurrBondIndex(i);
		if (ulIndex >= 1)
		{
			ulBlock = i;
			break;
		}
	}

	if (ulBlock == 0)
	{
		return FALSE;
	}

	szTitle.LoadString(HMB_BT_COLLET_OFFSET);

	szContent.LoadString(HMB_BT_AUTO_ADJ_OFFSET);

	if ( HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES )
	{
		//Move to this binblock & index
		SetJoystickOn(FALSE);

		GetDieLogicalEncoderValue(ulBlock, ulIndex, lX, lY);

		MoveXYTo(lX, lY);
	}
	else
	{
		SetJoystickOn(TRUE);

		szContent.LoadString(HMB_BT_ADJ_OFFSET_STEP1);

		//HmiMessage(szContent ,szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
		HmiMessageEx(szContent ,szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
		SetJoystickOn(FALSE);
	}

	if (CheckBPRStatus() == 1)
	{
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
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchDie", stMsg);
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

		if (stInfo.bResult == TRUE)
		{
			GetEncoderValue();
			MoveXYTo(m_lEnc_X + stInfo.siStepX, 
					 m_lEnc_Y + stInfo.siStepY);
		}

		bManualMode = !stInfo.bResult;
	}


	if (bManualMode == TRUE)
	{
		SetJoystickOn(TRUE);

		szContent.LoadString(HMB_BT_ADJ_OFFSET_STEP2);

		if (HmiMessage(szContent ,szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_CONTINUE)
		{
			SetJoystickOn(FALSE);
		}
		else
		{
			return FALSE;
		}
	}


	//Calcualte offset value
	GetEncoderValue();
	m_lColletOffsetX = m_lEnc_X - lX;
	m_lColletOffsetY = m_lEnc_Y - lY;
	//SaveBinTableData();
	SaveBinTableSetupData();
	SetJoystickOn(TRUE);

	return TRUE;
}


VOID CBinTable::FlushMessage()
{
	if (m_comServer.ScanRequest(10))
		m_comServer.ReadRequest();
}

VOID CBinTable::UpdateStationData()
{
	//AfxMessageBox("CBinTable", MB_SYSTEMMODAL);
	ULONG ulBlockId = 1;
	m_oBinTableDataBlock.m_szBinBlockDieQuantities.Format("%d", m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(ulBlockId)));
	
	if (m_oBinBlkMain.GrabWalkPath(ulBlockId) == 0)
	{
		m_oBinTableDataBlock.m_szBinBlockWalkPath = "TL-Horz";
	}
	else if (m_oBinBlkMain.GrabWalkPath(ulBlockId) == 1)
	{
		m_oBinTableDataBlock.m_szBinBlockWalkPath = "TL-Vert";
	}
	else if (m_oBinBlkMain.GrabWalkPath(ulBlockId) == 2)
	{
		m_oBinTableDataBlock.m_szBinBlockWalkPath = "TR-Horz";
	}
	else
	{
		m_oBinTableDataBlock.m_szBinBlockWalkPath = "TR-Vert";
	}

	m_oBinTableDataBlock.m_szFrameReAlignLastDieOffsetX.Format("%.2f", m_dAlignLastDieOffsetX);
	m_oBinTableDataBlock.m_szFrmaeRealignLastDieOffsetY.Format("%.2f", m_dAlignLastDieOffsetY);
	m_oBinTableDataBlock.m_szMaxDieQuantities.Format("%d", m_oBinBlkMain.GrabNoOfDiePerBlk(ulBlockId));
	m_oBinTableDataBlock.m_szNoOfBinBlocks.Format("%d", m_oBinBlkMain.GetNoOfBlk());
	
	if (m_lRealignBinFrameOption == 0)
	{
		m_oBinTableDataBlock.m_szReAlignMethod = "Default";
	}
	else
	{
		m_oBinTableDataBlock.m_szReAlignMethod = "1-Pt";
	}

	m_oBinTableDataBlock.m_szChangeGradeLimit.Format("%d", m_ulGradeChangeLimit);
	m_oBinTableDataBlock.m_szBHZ2BondPosOffsetX.Format("%d", m_lBHZ2BondPosOffsetX);
	m_oBinTableDataBlock.m_szBHZ2BondPosOffsetY.Format("%d", m_lBHZ2BondPosOffsetX);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SetBinTableDataBlock(m_oBinTableDataBlock);
}

BOOL CBinTable::ReprintTSCLabel(CString szFileName, CString& szRetCode)
{
	/*
	CStdioFile cfLabelFile;
	INT nCol;
	CTSCPrinter oTSCPrinter;
	CString szContent, szFormat;
	CString szLabelFeedDirection, szLabelWidth, szLabelHeight, szPrinterSpeed,
		szPrinterDarkness, szSensorType, szGapMarkSize, szGapMarkOffset;
	
	if (cfLabelFile.Open(szFileName, CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone|CFile::typeText) == FALSE)
	{
		return FALSE;
	}
		
	cfLabelFile.SeekToBegin();
	//Read Header Info
	cfLabelFile.ReadString(szContent);

	//Read Printer Model
	cfLabelFile.ReadString(szContent);

	// Read Label Feed Direction
	cfLabelFile.ReadString(szContent);
	szLabelFeedDirection = szContent;

	// Read Label Width
	cfLabelFile.ReadString(szContent);
	szLabelWidth = szContent;

	// Read Label Height
	cfLabelFile.ReadString(szContent);
	szLabelHeight = szContent;

	// Read Label Speed
	cfLabelFile.ReadString(szContent);
	szPrinterSpeed = szContent;

	// Read Label Printer Darkness
	cfLabelFile.ReadString(szContent);
	szPrinterDarkness = szContent;

	// Read Label Printer Sensor Type
	cfLabelFile.ReadString(szContent);
	szSensorType = szContent;

	// Read Label Printer Gap Mark Size
	cfLabelFile.ReadString(szContent);
	szGapMarkSize = szContent;

	// Read Label Printer Gap Mark Offset
	cfLabelFile.ReadString(szContent);
	szGapMarkOffset = szContent;

	//AfxMessageBox(" szLabelWidth:" + szLabelWidth + " szLabelHeight:" + szLabelHeight + " szSensorType:" + 
	//	szSensorType + " szGapMarkSize:" + szGapMarkSize + " szGapMarkOffset:" + szGapMarkOffset, MB_SYSTEMMODAL);
	oTSCPrinter.OpenPrinterPort(m_szLabelPrinterSel);

	oTSCPrinter.SetupPrinter(atoi(szLabelWidth), atoi(szLabelHeight) , atoi(szSensorType), atoi(szGapMarkSize), atoi(szGapMarkOffset));
	oTSCPrinter.SendCommnad(szLabelFeedDirection);
	oTSCPrinter.ClearBuffer();

	//Read contents
	while ( cfLabelFile.ReadString(szContent) !=  NULL )
	{
		//Read Format
		nCol = szContent.Find(":");
		szFormat = szContent.Left(nCol);
		szContent = szContent.Right(szContent.GetLength() - nCol -1);

		if (szFormat == BT_LABEL_TEXT_FORMAT)
		{
			CString szX, szY, szFontHeight, szFontStyle, szFontUnderLine, szRotation, 
				szFont, szOutputContent;
			
			nCol = szContent.Find(",");
			szX = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szY = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szFontHeight = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szFontStyle = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szFontUnderLine = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szRotation = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szFont = szContent.Left(nCol);
			szOutputContent = szContent.Right(szContent.GetLength() - nCol -1);

			//AfxMessageBox("szX:" + szX + " szY:" + szY + " szFontHeight:" + szFontHeight + " szOutputContent:" + szOutputContent
				//+ " szFontStyle:" + szFontStyle + " szFontUnderLine:" + szFontUnderLine + " szRotation:" + szRotation, MB_SYSTEMMODAL);

			oTSCPrinter.Windowsfont(atoi(szX), atoi(szY), atoi(szFontHeight),szOutputContent,atoi(szFontStyle), atoi(szFontUnderLine), 
				atoi(szRotation));

			
		}
		else if (szFormat == BT_LABEL_BARCODE_FORMAT)
		{

			CString szX, szY, szBarcodeType, szBarcodeHeight, szVisibleText, szRotation, szNarrowBarSize,
				szWideBarSize, szOutputContent;

			nCol = szContent.Find(",");
			szX = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szY = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szBarcodeType = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szBarcodeHeight = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szVisibleText = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szRotation = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szNarrowBarSize = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol -1);

			nCol = szContent.Find(",");
			szWideBarSize = szContent.Left(nCol);
			szOutputContent = szContent.Right(szContent.GetLength() - nCol -1);

			
			//AfxMessageBox("szX:" + szX + " szY:" + szY + " szBarcodeType:" + szBarcodeType + " szBarcodeHeight:" + szBarcodeHeight
			//	+ " szVisibleText:" + szVisibleText + " szRotation:" + szRotation + " szNarrowBarSize:" + szNarrowBarSize
			//	+ " szWideBarSize:" + szWideBarSize + " OutputContent:" + szOutputContent, MB_SYSTEMMODAL);

			oTSCPrinter.Barcode(atoi(szX), atoi(szY), atoi(szBarcodeHeight), szOutputContent, atoi(szNarrowBarSize), 
				atoi(szWideBarSize), atoi(szVisibleText) , atoi(szRotation));
		}
	}

	oTSCPrinter.PrintLabel(1,1);
	oTSCPrinter.ClosePrinterPort();

	cfLabelFile.Close();
	*/
	return TRUE;
}

BOOL CBinTable::ReprintDymoLabel(CString szFileName, CString& szRetCode)
{
	CStdioFile cfLabelFile;
	CString szPrinterModel;
	CString szLabelName;
	CString szContent, szContent1;
	CString szTempValue;
	CString szFormat;
	int nCol = 0;
	int nTextTop = 0;
	int nTextLeft = 0;
	int nTextBottom = 0;
	int nTextRight = 0;  
	int nRtnCode = 0;
	RECT TextRect;
	RECT LineRect;
	CPtrFactory PtrFactory;
	CLblPtr* pPtrObj;


	//v2.72a5
	if (!AfxOleInit())
	{
		szRetCode = "OLE initialization fails";
		return FALSE;
	}

	//Create printer obj
	pPtrObj = PtrFactory.Create(0);
	if ( pPtrObj == NULL )
	{
		szRetCode = "No printer object defined";
		return FALSE;
	}

	//Read file 
	if ( cfLabelFile.Open(szFileName, CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone|CFile::typeText) == TRUE )
	{
		cfLabelFile.SeekToBegin();

		//Read Header Info
		cfLabelFile.ReadString(szContent);
		if ( szContent != BT_LABEL_FILE_HEADER )
		{
			cfLabelFile.Close();
			delete pPtrObj;
			szRetCode = "No label file header!";
			return FALSE;
		}

		//Read Printer name
		cfLabelFile.ReadString(szPrinterModel);
		szPrinterModel = m_szLabelPrinterSel;

		//Read & Set Label name
		cfLabelFile.ReadString(szLabelName);
		pPtrObj->SelectLabel(szLabelName);

		//Read & set label angle
		cfLabelFile.ReadString(szContent);
		pPtrObj->SetRotation(atoi((LPCTSTR)szContent));

		//Read & set Text font & size
		cfLabelFile.ReadString(szContent);
		cfLabelFile.ReadString(szContent1);
		pPtrObj->SetFont(szContent, atoi((LPCTSTR)szContent1), TRUE, FALSE, FALSE, FALSE);

		//Read & Set barcode type & size
		cfLabelFile.ReadString(szContent);
		cfLabelFile.ReadString(szContent1);
		pPtrObj->SetBCType(atoi((LPCTSTR)szContent));
		pPtrObj->SetBCSize(atoi((LPCTSTR)szContent1));

		//Set Text & barcode
		pPtrObj->SetHAlign(0);
		pPtrObj->SetTextVAlign(1);
		pPtrObj->SetBCTextPos(0);


		//Read contents
		while ( cfLabelFile.ReadString(szContent) !=  NULL )
		{
			//Read Format
			nCol = szContent.Find(":");
			szFormat = szContent.Left(nCol);

			if (szFormat == BT_LABEL_SELECT_FONT)
			{
				CString szFont, szFontSize;
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);
				
				nCol = szContent.Find(",");
				szFont = szContent.Left(nCol);
				szFontSize = szContent.Right(szContent.GetLength() - nCol - 1);

				pPtrObj->SetFont(szFont, atoi((LPCTSTR)szFontSize), TRUE, FALSE, FALSE, FALSE);

			}
			else if (szFormat == BT_LABEL_SELECT_FONT2)
			{
				//AfxMessageBox("Font: 2", MB_SYSTEMMODAL);

				CString szFont, szFontSize, szBold, szItalic, szUnderline, szStrikeOut;

				szContent = szContent.Right(szContent.GetLength() - nCol - 1);
				
				nCol = szContent.Find(",");
				szFont = szContent.Left(nCol);
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				nCol = szContent.Find(",");
				szFontSize = szContent.Left(nCol);
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				nCol = szContent.Find(",");
				szBold = szContent.Left(nCol);
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				nCol = szContent.Find(",");
				szItalic = szContent.Left(nCol);
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				nCol = szContent.Find(",");
				szUnderline = szContent.Left(nCol);
				szStrikeOut = szContent.Right(szContent.GetLength() - nCol - 1);

				//AfxMessageBox("Font: " + szFont + "," + szBold + "," + szItalic + "," + szUnderline + "," + szStrikeOut, MB_SYSTEMMODAL);

				pPtrObj->SetFont(szFont, atoi(szFontSize), atoi(szBold), atoi(szItalic), atoi(szUnderline), atoi(szStrikeOut));

			}
			else if (szFormat == BT_LABEL_DRAW_LINE)
			{
				CString szOrientation, szThickness, szTop, szLeft, szBottom, szRight;
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);
				
				nCol = szContent.Find(",");
				szOrientation = szContent.Left(nCol);
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				nCol = szContent.Find(",");
				szThickness = szContent.Left(nCol);
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);
				
				//Read Top
				nCol = szContent.Find(",");
				szTop = szContent.Left(nCol);
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				//Read Left
				nCol = szContent.Find(",");
				szLeft = szContent.Left(nCol);
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				//Read bottom
				nCol = szContent.Find(",");
				szBottom = szContent.Left(nCol);
				szRight = szContent.Right(szContent.GetLength() - nCol - 1);

				LineRect.top = atoi(szTop);
				LineRect.left = atoi(szLeft);
				LineRect.bottom = atoi(szBottom);
				LineRect.right = atoi(szRight);

				//AfxMessageBox("szOrientation: " + szOrientation + "," + szThickness + "," +
					//szTop + "," + szLeft + "," + szBottom + "," + szRight, MB_SYSTEMMODAL);
			
				pPtrObj->AddLine(atoi(szOrientation), atoi(szThickness), LineRect);
			}
			else
			{

				szContent = szContent.Right(szContent.GetLength() - nCol - 1);
				
				//Read Top
				nCol = szContent.Find(",");
				TextRect.top = atoi((LPCTSTR)szContent.Left(nCol));
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				//Read Left
				nCol = szContent.Find(",");
				TextRect.left = atoi((LPCTSTR)szContent.Left(nCol));
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				//Read bottom
				nCol = szContent.Find(",");
				TextRect.bottom = atoi((LPCTSTR)szContent.Left(nCol));
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				//Read right
				nCol = szContent.Find(",");
				TextRect.right = atoi((LPCTSTR)szContent.Left(nCol));
				szContent = szContent.Right(szContent.GetLength() - nCol - 1);

				//Add information for printer use
				if ( szFormat == BT_LABEL_TEXT_FORMAT )
				{
					pPtrObj->AddText(szContent, TextRect);
				}
				else if (szFormat == BT_LABEL_BARCODE_FORMAT)
				{
					pPtrObj->AddBarcode(szContent, TextRect);
				}
				else if (szFormat == BT_LABEL_GRAPHIC_FORMAT)
				{
					// szContent is path of bmp
					pPtrObj->AddGraphic(szContent, TextRect);
				}
				
			}
		}

		cfLabelFile.Close();

		//Print label
		pPtrObj->PrintLabel(szPrinterModel);
#if 0	// debug, not wait queue
		do
		{
			nRtnCode = pPtrObj->GetPrinterStatus(szPrinterModel);

			if (nRtnCode == 0X00000010)	//JOB_STATUS_PRINTING
			{
				Sleep(100);
			}
			else
			{
				if (nRtnCode != 0)
				{
					if ((nRtnCode & 0X00000001) != 0)	//JOB_STATUS_PAUSED
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_PAUSED";
						return FALSE;
					}

					if ((nRtnCode & 0X00000002) != 0)	//JOB_STATUS_ERROR
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_ERROR";
						return FALSE;
					}

					if ((nRtnCode & 0X00000004) != 0)	//JOB_STATUS_DELETING
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_DELETING";
						return FALSE;
					}

					if ((nRtnCode & 0X00000008) != 0)	//JOB_STATUS_SPOOLING
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_SPOOLING";
						return FALSE;
					}

					if ((nRtnCode & 0X00000020) != 0)	//JOB_STATUS_OFFLINE
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_OFFLINE";
						return FALSE;
					}

					if ((nRtnCode & 0X00000040) != 0)	//JOB_STATUS_PAPEROUT
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_PAPEROUT";
						return FALSE;
					}

					if ((nRtnCode & 0X00000080) != 0)	//JOB_STATUS_PRINTED
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_PRINTED";
						return FALSE;
					}

					if ((nRtnCode & 0X00000100) != 0)	//JOB_STATUS_DELETED
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_DELETED";
						return FALSE;
					}

					if ((nRtnCode & 0X00000200) != 0)	//JOB_STATUS_BLOCKED_DEVQ
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_BLOCKED_DEVQ";
						return FALSE;
					}

					if ((nRtnCode & 0X00000400) != 0)	//JOB_STATUS_USER_INTERVENTION
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_USER_INTERVENTION";
						return FALSE;
					}

					if ((nRtnCode & 0X00000800) != 0)	//JOB_STATUS_RESTART
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						szRetCode = "JOB_STATUS_RESTART";
						return FALSE;
					}
				}
				else //Success
				{
					break;
				}
			}
		} while (TRUE);
#endif
	}


	delete pPtrObj;
	szRetCode = "Printing OK";
	return TRUE;
}


BOOL CBinTable::ReprintLabel(CString szFileName, CString& szRetCode)
{
	CStdioFile cfLabelFile;
	CString szContent;
	BOOL bReturn;
	CString szLabelPrinterModel = m_szLabelPrinterSel;

	if (szLabelPrinterModel.Find(BT_DYMO_LABEL_PRINTER_U) != -1)
	{
		bReturn = ReprintDymoLabel(szFileName, szRetCode);	
	}
	else
	{
		bReturn = ReprintTSCLabel(szFileName, szRetCode);
	}
	
	return bReturn;
}

//4.53D24 func
BOOL CBinTable::InputBinBarcodeLabelToPrint()
{
	CString szLog;
	CString szBarcode = "";
	CString szTitle = "Please scan the bin Frame Barcode";
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


	if(HmiStrInputKeyboard(szTitle, szBarcode) == FALSE)
	{	
		szLog = "Error: input key board fail!";
		HmiMessage(szLog);
		SetErrorMessage(szLog);
		return FALSE;
	}

	if(szBarcode.IsEmpty() == TRUE)
	{
		szLog = "Error: input barcode name is empty! \nPlease check it!";
		HmiMessage(szLog);
		SetErrorMessage(szLog);
		return FALSE;
	}

	CFileFind cfReprint;
	CString szLabelFile = m_szOutputFilePath +  "\\" + szBarcode + "\\*.lbex";
	BOOL bFind = cfReprint.FindFile(szLabelFile);

	if(!bFind)
	{
		szLog.Format("Error: Find Bin Label file path fail! \nPath:%s",szLabelFile);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	while (bFind)
	{
		bFind = cfReprint.FindNextFile();
		CString szLabelFileName	=cfReprint.GetFileName();
		CString szLabelFilePath =cfReprint.GetFilePath();
		CString szNewLabelPath  =m_szLabelFilePath + "\\PrintLabel\\" + szLabelFileName;
		CString szRetCode = "OK";

		szLog.Format("Label file path:%s",szLabelFilePath);
		//HmiMessage(szLog);  //4.53D102
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		

		if(szLabelFileName.Find("MultiLabel_") != -1)
		{
			if (ReprintLabelWithSecondXMLLayout(szLabelFilePath, szRetCode) == TRUE)
			{
				CreateDirectory(m_szLabelFilePath + "\\PrintLabel",NULL);
				CopyFile(szLabelFilePath,szNewLabelPath,false);
				remove(szLabelFilePath);
			}
			
			return TRUE;
		}

		
		if (ReprintLabelWithXMLLayout(szLabelFilePath, szRetCode) == TRUE)
		{
			CreateDirectory(m_szLabelFilePath + "\\PrintLabel",NULL);
			CopyFile(szLabelFilePath,szNewLabelPath,false);
			remove(szLabelFilePath);
		}
	}

	return TRUE;
}



BOOL CBinTable::ReprintLabelWithSecondXMLLayout(CString szFileName, CString& szRetCode)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

#ifndef MS_DEBUG	//v4.47T5

	CLabelFactory* pLabelFactory;
	CPrinter* pPrinterFile;
	CLabelDatabase* pDataBase;
	CString szPrinterFile ;
	CString szLayoutFilePath ;
	szPrinterFile		= "c:\\MapSorter\\Exe\\BinOutputFiles\\"+ pApp->GetCustomerName()  + "MultiLabelPrinter.xml";
	szLayoutFilePath	= "c:\\MapSorter\\Exe\\BinOutputFiles\\" + pApp->GetCustomerName() + "MultiLabelLayout.xml";
	CString szBpLabelFileName = szFileName;
	CString szLog;

	szLog.Format("Label 2nd layout path:\n %s \n %s",szPrinterFile, szLayoutFilePath);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	pPrinterFile = CreateAndInitPrinterFromXml((LPCTSTR) szPrinterFile);
	if( pPrinterFile == NULL)
	{
		return FALSE;
	}

//AfxMessageBox(szLayoutFilePath, MB_SYSTEMMODAL);

	pLabelFactory = CreateLabelFactory();
	if (pLabelFactory == NULL)
	{
		return FALSE;
	}

	pLabelFactory->LoadXml(szLayoutFilePath);
//TSC 128Barcode issue
	pPrinterFile->SetOrientationFitLayout(0, 0);
//AfxMessageBox(szBpLabelFileName, MB_SYSTEMMODAL);

	pDataBase = CreateDatabase(szBpLabelFileName);
	if (pDataBase == NULL)
	{
		return FALSE;
	}

	pLabelFactory->Print(*pPrinterFile, *pDataBase);

	//CopyFile(m_szBpLabelFileName, g_szBackupFilePath + "\\" + m_szBarcodeNo + ".lbex", FALSE);

	if (pPrinterFile != NULL)
	{
		DeletePrinter(pPrinterFile);
	}

	if (pLabelFactory != NULL)
	{
		DeleteLabelFactory(pLabelFactory);
	}

	if (pDataBase != NULL)
	{
		delete pDataBase;
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
#endif 

	return TRUE;

} // End of ReprintLabelWithSecondXMLLayout


BOOL CBinTable::ReprintLabelWithXMLLayout(CString szFileName, CString& szRetCode)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

#ifndef MS_DEBUG	//v4.47T5

	CLabelFactory* pLabelFactory;
	CPrinter* pPrinterFile;
	CLabelDatabase* pDataBase;
	CString szPrinterFile ;
	CString szLayoutFilePath ;
	szPrinterFile		= "c:\\MapSorter\\Exe\\BinOutputFiles\\"+ pApp->GetCustomerName()+ "Printer.xml";
	szLayoutFilePath	= "c:\\MapSorter\\Exe\\BinOutputFiles\\" + pApp->GetCustomerName() + "Layout.xml";
	CString szBpLabelFileName = szFileName;
//AfxMessageBox(szPrinterFile, MB_SYSTEMMODAL);

	pPrinterFile = CreateAndInitPrinterFromXml((LPCTSTR) szPrinterFile);
	if( pPrinterFile == NULL)
	{
		HmiMessage("Load Printer File Fails," + szPrinterFile);
		return FALSE;
	}

//AfxMessageBox(szLayoutFilePath, MB_SYSTEMMODAL);

	pLabelFactory = CreateLabelFactory();
	if (pLabelFactory == NULL)
	{
		HmiMessage("Creat Label Factory Fails.");
		return FALSE;
	}

	pLabelFactory->LoadXml(szLayoutFilePath);
//TSC 128Barcode issue
	pPrinterFile->SetOrientationFitLayout(0, 0);
//AfxMessageBox(szBpLabelFileName, MB_SYSTEMMODAL);

	pDataBase = CreateDatabase(szBpLabelFileName);
	if (pDataBase == NULL)
	{
		HmiMessage("Create Database Fails," + szBpLabelFileName + "," + szLayoutFilePath);
		return FALSE;
	}

	pLabelFactory->Print(*pPrinterFile, *pDataBase);

	//CopyFile(m_szBpLabelFileName, g_szBackupFilePath + "\\" + m_szBarcodeNo + ".lbex", FALSE);

	if (pPrinterFile != NULL)
	{
		DeletePrinter(pPrinterFile);
	}

	if (pLabelFactory != NULL)
	{
		DeleteLabelFactory(pLabelFactory);
	}

	if (pDataBase != NULL)
	{
		delete pDataBase;
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
#endif 

	return TRUE;
	//BOOL bReturn = TRUE;
	//CLabelFactory* pLabelFactory;
	//CPrinter* pPrinter;
	//CLabelDatabase* pDataBase;
	//CString szLabelPrinterModel, szPrinterPort;
	//CString szStr;
	//INT nCol = 0;
	//CStdioFile cfLabelFile;
	//CString szLayoutFile, szLayoutFilePath;
	//CString szLabelWidth, szLabelHeight, szLabelGap;
	//CString szTemp;
	//CString szDarkness = "";
	//
	//szLabelPrinterModel = m_szLabelPrinterSel;
	//szPrinterPort		= m_szLabelPrinterSel;
	//
	//if (cfLabelFile.Open(_T(szFileName), 
	//	CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone|CFile::typeText) == FALSE)
	//{
	//	return FALSE;
	//}

	//cfLabelFile.SeekToBegin();

	//while(cfLabelFile.ReadString(szStr))
	//{
	//	if (szStr == BT_LAYOUT_FILE_TAG)
	//	{
	//		cfLabelFile.ReadString(szStr);

	//		szLayoutFile = szStr;
	//		szLayoutFilePath = gszROOT_DIRECTORY + "\\Exe\\BinOutputFiles\\";
	//		szLayoutFile = szLayoutFilePath + szLayoutFile;
	//		//AfxMessageBox(szLayoutFile);
	//		if ( _access(szLayoutFile, 0) == -1 )
	//		{
	//			HmiMessage("The LayoutFile:" + szLayoutFile + " missing!!");
	//			return FALSE;
	//		}
	//	}
	//	else if (szStr == BT_LABEL_WIDTH_TAG)
	//	{
	//		cfLabelFile.ReadString(szLabelWidth);
	//	}
	//	else if (szStr == BT_LABEL_HEIGHT_TAG)
	//	{
	//		cfLabelFile.ReadString(szLabelHeight);
	//	}
	//	else if (szStr == BT_LABEL_GAP_TAG)
	//	{
	//		cfLabelFile.ReadString(szLabelGap);
	//	}
	//	else if (szStr == BT_LABEL_DARKNESS)
	//	{
	//		cfLabelFile.ReadString(szDarkness);
	//	}
	//	else if (szStr == BT_LABEL_FILE_CONTENT)
	//	{
	//		break;
	//	}
	//}

	//cfLabelFile.Close();

	//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	//if ( pApp->GetFeatureStatus(MS896A_FUNC_LABELPRINTOUT_XML) )
	//{
	//	//AfxMessageBox("CreatePrinterFromXml", MB_SYSTEMMODAL);
	//	CString szPrinterFile = gszROOT_DIRECTORY + "\\Exe\\BinOutputFiles\\" + pApp->GetCustomerName() + "PrinterXMLFile.xml";
	//	pPrinter = CreateAndInitPrinterFromXml ( (LPCTSTR) szPrinterFile );
	//	//AfxMessageBox(szPrinterFile, MB_SYSTEMMODAL);
	//	if (pPrinter == NULL)
	//	{
	//		return FALSE;
	//	}
	//}
	//else
	//{

	//	if (szLabelPrinterModel.Find(BT_DYMO_LABEL_PRINTER_U) != -1)
	//	{
	//		szLabelPrinterModel = BT_DYMO_LABEL_PRINTER_U;
	//	}
	//	else if (szLabelPrinterModel.Find(BT_TSC_LABEL_PRINTER) != -1)
	//	{
	//		szLabelPrinterModel = BT_TSC_LABEL_PRINTER;
	//	}
	//	else
	//	{
	//		szLabelPrinterModel = BT_ZEBRA_LABEL_PRINTER;
	//	}

	//	pPrinter = CreateAndInitPrinter(szLabelPrinterModel, szPrinterPort, "1", atoi(szLabelWidth), atoi(szLabelHeight), atoi(szLabelGap));
	//	
	//	if (pPrinter == NULL)
	//	{
	//		return FALSE;
	//	}

	//	if (szDarkness == "")
	//	{
	//		szDarkness = "5";
	//	}

	//	pPrinter->SetUp("1", atoi(szLabelWidth), atoi(szLabelHeight), atoi(szLabelGap), 3.0, atoi(szDarkness));

	//	//if (pPrinter == NULL)
	//	//{
	//	//	return FALSE;
	//	//}
	//}

	//pLabelFactory = CreateLabelFactory();
	//if (pLabelFactory == NULL)
	//{
	//	return FALSE;
	//}

	//pLabelFactory->LoadXml(szLayoutFile);

	//pDataBase = CreateDatabase(szFileName);
	//if (pDataBase == NULL)
	//{
	//	return FALSE;
	//}

	//pLabelFactory->Print(*pPrinter, *pDataBase);

	//if (pPrinter != NULL)
	//{
	//	delete pPrinter;
	//}

	//if (pLabelFactory != NULL)
	//{
	//	delete pLabelFactory;
	//}

	//if (pDataBase != NULL)
	//{
	//	delete pDataBase;
	//}

	//return bReturn;
}


BOOL CBinTable::OptimizeBinGrade(VOID)
{
	if ( m_oBinBlkMain.GrabAutoAssignGrade() == FALSE )
	{
		return TRUE;
	}

	CUIntArray aulAvailableGradeList;
	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\OptimizeGrade.log";
	CString szBinBlks = "", szBlkNo = "";
	ULONG ulLeft = 0, ulPick = 0, ulTotal = 0;
	UCHAR ucBlkCurGrade;
	ULONG ulMapGrade;
	ULONG i,k;
	INT j;
	UCHAR *ucBlkStatus;
	UCHAR *ucGradeStatus;
	ULONG *ulGradeRemainCount;
	BOOL bUpdateStatus;
	UCHAR ucGrade;
	BOOL bFirst;
	BOOL bFirstStart;
	BOOL bHaveAssign;
	CString szLogText;


	ucBlkStatus = new UCHAR[m_oBinBlkMain.GetNoOfBlk() + 1];
	for (i=0; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		ucBlkStatus[i] = BT_OZ_BLK_NOT_ASSIGN;	//Index 0 is no used
	}

	//Get available wafermap grade & its remain count
	m_WaferMapWrapper.GetAvailableGradeList(aulAvailableGradeList);

	ucGradeStatus = new UCHAR[aulAvailableGradeList.GetSize()];
	ulGradeRemainCount = new ULONG[aulAvailableGradeList.GetSize()];

	for (j=0; j<aulAvailableGradeList.GetSize(); j++)
	{
		ucGradeStatus[j] = BT_OZ_GRADE_NOT_ASSIGN;

		m_WaferMapWrapper.GetStatistics((UCHAR)aulAvailableGradeList[j], ulLeft, ulPick, ulTotal);
		ulGradeRemainCount[j] = ulLeft;
	}


	//Stage 1: Check which binblk is fulled & ask user to clear this binblk	
	for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		//CString szBlkNo = "";
		if (m_oBinBlkMain.GrabNVIsFull(i) == TRUE)
		{
			BOOL bAllBlkInSameGradeFull = TRUE;
			szBinBlks = "";

			for (k=1; k<=m_oBinBlkMain.GetNoOfBlk(); k++)
			{
				if (m_oBinBlkMain.GrabGrade(i) == m_oBinBlkMain.GrabGrade(k))
				{
					if (m_oBinBlkMain.GrabNVIsFull(k) == FALSE)
					{
						bAllBlkInSameGradeFull = FALSE;
						k = m_oBinBlkMain.GetNoOfBlk();
					}
					else
					{
						szBlkNo.Format("%d,", k);
						szBinBlks += szBlkNo;
					}
				}
			}

			if (bAllBlkInSameGradeFull == TRUE)
			{
				szBinBlks = szBinBlks.Left(szBinBlks.GetLength()-1);
				AfxMessageBox("BinBlk " + szBinBlks + " is fulled!\nPlease clear before load map!", MB_ICONSTOP|MB_SYSTEMMODAL);
				//Clear Wafermap
				m_WaferMapWrapper.InitMap();

				//Klocwork
				delete [] ucBlkStatus;
				delete [] ucGradeStatus;
				delete [] ulGradeRemainCount;

				return FALSE;
			}
		}
	}

	if (CMSLogFileUtility::Instance()->GetEnableMachineLog() == TRUE)
	{
		CString szTemp;
		szLogText = "";
		bFirst = TRUE;
		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog("",TRUE);
		
		szTemp.Format("[Begin]\n");
		szLogText = szLogText + szTemp;
		
		szTemp.Format("Map File\t-> %s\n", GetMapFileName());
		szLogText = szLogText + szTemp;
		
		szTemp.Format("Map Grade\t-> ");
		szLogText = szLogText + szTemp;

		for (j=0; j<aulAvailableGradeList.GetSize(); j++)
		{
			if ( bFirst == TRUE )
			{
				bFirst = FALSE; 
				//fprintf(fp, "%d(%d)", (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset(), ulGradeRemainCount[j]);
				szTemp.Format("%d(%d)", (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset(), ulGradeRemainCount[j]);
				szLogText = szLogText + szTemp;
			}
			else
			{
				//fprintf(fp, ", %d(%d)", (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset(), ulGradeRemainCount[j]);
				szTemp.Format(", %d(%d)", (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset(), ulGradeRemainCount[j]);
				szLogText = szLogText + szTemp;
			}
		}
		
		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
	}


	//Stage 1: Default unbonded binblk to original grade
	for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);		//v4.51A20	//Klocwork

		if ( m_oBinBlkMain.GrabNVNoOfBondedDie(i) == 0 )
		{
			bUpdateStatus = m_oBinBlkMain.ReassignBlkGrade(i, m_oBinBlkMain.GrabOriginalGrade(i), TRUE, TRUE);
		}
	}


	//Stage 2: Check (same grade bonded) binblock remain count can be occupied by available wafermap grade's die count
	for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		ucBlkCurGrade = m_oBinBlkMain.GrabGrade(i);
		ulMapGrade = (ULONG)ucBlkCurGrade + m_WaferMapWrapper.GetGradeOffset();

		if ( (m_oBinBlkMain.GrabIsAssigned(ucBlkCurGrade) == TRUE) && (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0) )
		{
			for (j = 0; j < aulAvailableGradeList.GetSize(); j++)
			{
				if ( ulMapGrade == aulAvailableGradeList[j] )
				{
					m_WaferMapWrapper.GetStatistics((UCHAR)aulAvailableGradeList[j], ulLeft, ulPick, ulTotal);
					if ( ulLeft > (m_oBinBlkMain.GrabNoOfDiePerBlk(i) - m_oBinBlkMain.GrabNVNoOfBondedDie(i)) )
					{
						ucBlkStatus[i] = BT_OZ_BLK_IS_FULL;
						ucGradeStatus[j] = BT_OZ_GRADE_MORE_BLK;
						ulGradeRemainCount[j] -= (m_oBinBlkMain.GrabNoOfDiePerBlk(i) - m_oBinBlkMain.GrabNVNoOfBondedDie(i));  
					}
					else
					{
						ucBlkStatus[i] = BT_OZ_BLK_NOT_FULL;
						ucGradeStatus[j] = BT_OZ_GRADE_LESS_BLK;
						ulGradeRemainCount[j] = 0;
						break;
					}
				}
			}
		}
	}

	if ( CMSLogFileUtility::Instance()->GetEnableMachineLog() == TRUE)
	{
		CString szTemp;
		szLogText = "";
		//Outpur Grade assignment
		bFirstStart = TRUE;
	
		szTemp.Format("[Stage 2]\n");
		szLogText = szLogText + szTemp;

		szTemp.Format("Grade Assign\t-> ");
		szLogText = szLogText + szTemp;

		for (j=0; j<aulAvailableGradeList.GetSize(); j++)
		{
			ucGrade = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
			if ( bFirstStart == TRUE )
			{
				szTemp.Format("%d(", ucGrade);
				szLogText = szLogText + szTemp;
				bFirstStart = FALSE;
			}
			else
			{
				szTemp.Format(", %d(", ucGrade);
				szLogText = szLogText + szTemp;
			}
			
			bFirst = TRUE;
			bHaveAssign = FALSE;
			for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
			{
				if ( m_oBinBlkMain.GrabGrade(i) == ucGrade )
				{
					if ( bFirst == TRUE )
					{
						szTemp.Format("%d", i);
						szLogText = szLogText + szTemp;
						bFirst = FALSE;
					}
					else
					{
						szTemp.Format(",%d", i);
						szLogText = szLogText + szTemp;
					}
					bHaveAssign = TRUE;
				}
			}
			
			if ( bHaveAssign == FALSE )
			{
				szTemp.Format("0");
				szLogText = szLogText + szTemp;
			}

			szTemp.Format(")");
			szLogText = szLogText + szTemp;
		}
		szTemp.Format("\n");
		szLogText = szLogText + szTemp;

		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
		//empty the string first
		szLogText = "";

		//Outpur Grade remain count
		bFirstStart = TRUE;
		szTemp.Format("Grade Left/Use\t-> ");
		szLogText = szLogText + szTemp;

		for (j=0; j<aulAvailableGradeList.GetSize(); j++)
		{
			ucGrade = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
			if ( bFirstStart == TRUE )
			{
				szTemp.Format("%d(%d/%d)", ucGrade, ulGradeRemainCount[j], m_oBinBlkMain.GrabNVBlkInUse(ucGrade));
				szLogText = szLogText + szTemp;
				bFirstStart = FALSE;
			}
			else
			{
				szTemp.Format(", %d(%d/%d)", ucGrade, ulGradeRemainCount[j], m_oBinBlkMain.GrabNVBlkInUse(ucGrade));
				szLogText = szLogText + szTemp;
			}
		
		}

		szTemp.Format("\n");
		szLogText = szLogText + szTemp;

		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
		//empty the string first
		szLogText = "";

		//Output Grade status
		bFirst = TRUE;
		szTemp.Format("Grade Status\t-> ");
		szLogText = szLogText + szTemp;

		for (j=0; j<aulAvailableGradeList.GetSize(); j++)
		{
			ucGrade = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
			if ( bFirst == TRUE )
			{
				szTemp.Format("%d(%d)", ucGrade, ucGradeStatus[j]);
				szLogText = szLogText + szTemp;
				bFirst = FALSE;
			}
			else
			{
				szTemp.Format(", %d(%d)", ucGrade, ucGradeStatus[j]);
				szLogText = szLogText + szTemp;
			}
		}
		szTemp.Format("\n");
		szLogText = szLogText + szTemp;

		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
		//empty the string first
		szLogText = "";

		//Output Blk status
		bFirst = TRUE;
		szTemp.Format("Blk Status\t-> ");
		szLogText = szLogText + szTemp;

		for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			if ( bFirst == TRUE )
			{
				szTemp.Format("%d(%d)", i, ucBlkStatus[i]);
				szLogText = szLogText + szTemp;
				bFirst = FALSE;
			}
			else
			{
				szTemp.Format(", %d(%d)", i, ucBlkStatus[i]);
				szLogText = szLogText + szTemp;
			}
		}
		szTemp.Format("\n");
		szLogText = szLogText + szTemp;

		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
	
	}

	//Stage 3: Check (same grade empty) binblock remain count can be occupied by available wafermap grade's die count
	for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		ucBlkCurGrade = m_oBinBlkMain.GrabGrade(i);
		ulMapGrade = (ULONG)ucBlkCurGrade + m_WaferMapWrapper.GetGradeOffset();

		if ( (m_oBinBlkMain.GrabIsAssigned(ucBlkCurGrade) == TRUE) && (m_oBinBlkMain.GrabNVNoOfBondedDie(i) == 0) )
		{
			ucBlkStatus[i] = BT_OZ_BLK_IS_ASSIGN;

			for (j = 0; j < aulAvailableGradeList.GetSize(); j++)
			{
				if ( ulMapGrade == aulAvailableGradeList[j] )
				{
					if ( ulGradeRemainCount[j] > m_oBinBlkMain.GrabNoOfDiePerBlk(i) )
					{
						ucBlkStatus[i] = BT_OZ_BLK_IS_FULL;
						ucGradeStatus[j] = BT_OZ_GRADE_MORE_BLK;
						ulGradeRemainCount[j] -= m_oBinBlkMain.GrabNoOfDiePerBlk(i);
					}
					else
					{
						ucBlkStatus[i] = BT_OZ_BLK_NOT_FULL;
						ucGradeStatus[j] = BT_OZ_GRADE_LESS_BLK;
						ulGradeRemainCount[j] = 0;
						break;
					}
				}
			}
		}
	}

	if ( CMSLogFileUtility::Instance()->GetEnableMachineLog() == TRUE)
	{
		CString szTemp;
		szLogText = "";

		//Outpur Grade remain count
		bFirstStart = TRUE;
		szTemp.Format("[Stage 3]\n");
		szLogText = szLogText + szTemp;

		szTemp.Format("Grade Left/Use\t-> ");
		szLogText = szLogText + szTemp;

		for (j=0; j<aulAvailableGradeList.GetSize(); j++)
		{
			ucGrade = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
			if ( bFirstStart == TRUE )
			{
				szTemp.Format("%d(%d/%d)", ucGrade, ulGradeRemainCount[j], m_oBinBlkMain.GrabNVBlkInUse(ucGrade));
				szLogText = szLogText + szTemp;
				bFirstStart = FALSE;
			}
			else
			{
				szTemp.Format(", %d(%d/%d)", ucGrade, ulGradeRemainCount[j], m_oBinBlkMain.GrabNVBlkInUse(ucGrade));
				szLogText = szLogText + szTemp;
			}
		
		}

		szTemp.Format("\n");
		szLogText = szLogText + szTemp;
		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
		szLogText = "";

		//Output Grade status
		bFirst = TRUE;
		szTemp.Format("Grade Status\t-> ");
		szLogText = szLogText + szTemp;

		for (j=0; j<aulAvailableGradeList.GetSize(); j++)
		{
			ucGrade = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
			if ( bFirst == TRUE )
			{
				szTemp.Format("%d(%d)", ucGrade, ucGradeStatus[j]);
				szLogText = szLogText + szTemp;
				bFirst = FALSE;
			}
			else
			{
				szTemp.Format(", %d(%d)", ucGrade, ucGradeStatus[j]);
				szLogText = szLogText + szTemp;
			}
		}
		szTemp.Format("\n");
		szLogText = szLogText + szTemp;
		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
		szLogText = "";

		//Output Blk status
		bFirst = TRUE;
		szTemp.Format("Blk Status\t-> ");
		for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			if ( bFirst == TRUE )
			{
				szTemp.Format("%d(%d)", i, ucBlkStatus[i]);
				szLogText = szLogText + szTemp;
				bFirst = FALSE;
			}
			else
			{
				szTemp.Format(", %d(%d)", i, ucBlkStatus[i]);
				szLogText = szLogText + szTemp;
			}
		}
		szTemp.Format("\n");
		szLogText = szLogText + szTemp;
		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
	}


	//Stage 4: Check all map grades can be occupied into binblocks
	for (j=0; j<aulAvailableGradeList.GetSize(); j++)
	{
		ucGrade = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();

		//Skip check Grade 0 since this grade cannot be supported
		if ( (ucGradeStatus[j] == BT_OZ_GRADE_NOT_ASSIGN) && (ucGrade > 0) )
		{
			AfxMessageBox("Not enough binblk for current map!\nPlease clear all binblk before load map!", MB_ICONSTOP|MB_SYSTEMMODAL);
			//Clear Wafermap
			m_WaferMapWrapper.InitMap();

			//Klocwork
			delete [] ucBlkStatus;
			delete [] ucGradeStatus;
			delete [] ulGradeRemainCount;

			return FALSE;
		}
	}


	//Stage 5: Check any binblks can occupy remain wafermap grade if this grade status is BT_OZ_GRADE_MORE_BLK
	for (j=0; j<aulAvailableGradeList.GetSize(); j++)
	{
		if ( ucGradeStatus[j] == BT_OZ_GRADE_MORE_BLK )
		{
			for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
			{
				//If this blk is assigned but no grade to use this time
				if ( ucBlkStatus[i] == BT_OZ_BLK_IS_ASSIGN )
				{
					i = min(i, BT_MAX_BINBLK_SIZE - 1);	//v4.53	Klocwork

					//update this binblk grade to current wafermap grade
					ulMapGrade = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
					bUpdateStatus = m_oBinBlkMain.ReassignBlkGrade(i, (UCHAR)ulMapGrade, FALSE, TRUE);

					if ( ulGradeRemainCount[j] > (m_oBinBlkMain.GrabNoOfDiePerBlk(i) - m_oBinBlkMain.GrabNVNoOfBondedDie(i)) )
					{
						ulGradeRemainCount[j] -= (m_oBinBlkMain.GrabNoOfDiePerBlk(i) - m_oBinBlkMain.GrabNVNoOfBondedDie(i));  
						ucBlkStatus[i] = BT_OZ_BLK_IS_FULL;
					}
					else
					{
						//No need to find any blk for this map grade	
						ulGradeRemainCount[j] = 0;
						ucBlkStatus[i] = BT_OZ_BLK_NOT_FULL;
						break;
					}
				}
			}
		}
	}

	if ( CMSLogFileUtility::Instance()->GetEnableMachineLog() == TRUE)
	{
		CString szTemp;
		szLogText = "";

		//Outpur Grade assignment
		bFirstStart = TRUE;
		szTemp.Format("[Stage 5]\n");
		szLogText = szLogText + szTemp;

		szTemp.Format("Grade Assign\t-> ");
		szLogText = szLogText + szTemp;

		for (j=0; j<aulAvailableGradeList.GetSize(); j++)
		{
			ucGrade = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
			if ( bFirstStart == TRUE )
			{
				szTemp.Format("%d(", ucGrade);
				szLogText = szLogText + szTemp;
				bFirstStart = FALSE;
			}
			else
			{
				szTemp.Format(", %d(", ucGrade);
				szLogText = szLogText + szTemp;
			}
			
			bFirst = TRUE;
			bHaveAssign = FALSE;
			for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
			{
				if ( m_oBinBlkMain.GrabGrade(i) == ucGrade )
				{
					if ( bFirst == TRUE )
					{
						szTemp.Format("%d", i);
						szLogText = szLogText + szTemp;
						bFirst = FALSE;
					}
					else
					{
						szTemp.Format(",%d", i);
						szLogText = szLogText + szTemp;
					}

					bHaveAssign = TRUE;
				}
			}
			
			if ( bHaveAssign == FALSE )
			{
				szTemp.Format("0");
				szLogText = szLogText + szTemp;
			}

			szTemp.Format(")");
			szLogText = szLogText + szTemp;
		}

		szTemp.Format("\n");
		szLogText = szLogText + szTemp;
		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
		szLogText ="";

		//Outpur Grade remain count
		bFirstStart = TRUE;
		szTemp.Format("Grade Left/Use\t-> ");
		szLogText = szLogText + szTemp;

		for (j=0; j<aulAvailableGradeList.GetSize(); j++)
		{
			ucGrade = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
			if ( bFirstStart == TRUE )
			{
				szTemp.Format("%d(%d/%d)", ucGrade, ulGradeRemainCount[j], m_oBinBlkMain.GrabNVBlkInUse(ucGrade));
				szLogText = szLogText + szTemp;
				bFirstStart = FALSE;
			}
			else
			{
				szTemp.Format(", %d(%d/%d)", ucGrade, ulGradeRemainCount[j], m_oBinBlkMain.GrabNVBlkInUse(ucGrade));
				szLogText = szLogText + szTemp;
			}
		
		}

		szTemp.Format("\n");
		szLogText = szLogText + szTemp;
		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
		szLogText ="";

		//Output Blk status
		bFirst = TRUE;
		szTemp.Format("Blk Status\t-> ");
		szLogText = szLogText + szTemp;

		for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			if ( bFirst == TRUE )
			{
				szTemp.Format("%d(%d)", i, ucBlkStatus[i]);
				szLogText = szLogText + szTemp;
				bFirst = FALSE;
			}
			else
			{
				szTemp.Format(", %d(%d)", i, ucBlkStatus[i]);
				szLogText = szLogText + szTemp;
			}
		}
		szTemp.Format("\n\n");
		szLogText = szLogText + szTemp;
		CMSLogFileUtility::Instance()->BT_OptimizeGradeLog(szLogText);
		
	}


	//Stage 5: Store Bin blk status
	for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE-1);	//v4.53	Klocwork
		m_oBinBlkMain.SetStatus(i, ucBlkStatus[i]);
	}


	delete [] ucBlkStatus;
	delete [] ucGradeStatus;
	delete [] ulGradeRemainCount;

	return TRUE;
}

//4.53D90fnc Reset after 1st time
BOOL CBinTable::OptimizeBinCountPerWft()
{

	CString szMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckResetBinCountPerWft = FALSE;
	bCheckResetBinCountPerWft = pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT_PER_WAFER);
	
	szMsg.Format("Check Reset Bin Count per Wafer, enable status: %d", bCheckResetBinCountPerWft);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	CUIntArray aulAvailableGradeList;
	ULONG ulLeft = 0, ulPick = 0, ulTotal = 0;

	INT j;
	
	ULONG *ulGradeResetCount = NULL;
	ULONG ulFullDieCountInput = 0, ulMinDieCountInput = 0, ulOrgInputCount;
	ULONG ulCurTotalDie, ulGradeMapCount, ulGradeDieRemainCount;
	ULONG ulResetFullDieCount = 0;
	UCHAR ucGrade;

	//Get available wafermap grade & its remain count
	m_WaferMapWrapper.GetAvailableGradeList(aulAvailableGradeList);

	//ucGradeStatus = new UCHAR[aulAvailableGradeList.GetSize()];
	//ulGradeRemainCount = new ULONG[aulAvailableGradeList.GetSize()];
	//ulGradeResetCount = new ULONG[aulAvailableGradeList.GetSize()];

	ulFullDieCountInput	  =  m_ulFullDieCountInput;
	ulMinDieCountInput	  = m_ulMinDieCountInput;


	//Stage 1: Check the condition of Die Count
	if(!bCheckResetBinCountPerWft)
	{
		return TRUE;
	}

	if(ulFullDieCountInput == 0 && ulMinDieCountInput == 0)
	{
		return TRUE;
	}



	//Stage 2: Run the function
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	for (j=0; j<aulAvailableGradeList.GetSize(); j++)
	{

		m_WaferMapWrapper.GetStatistics((UCHAR)aulAvailableGradeList[j], ulLeft, ulPick, ulTotal);

		ucGrade				  = (UCHAR)aulAvailableGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
		ulGradeDieRemainCount = m_oBinBlkMain.GrabNVNoOfSortedDie(ucGrade);
		ulGradeMapCount		  = ulLeft; 
		ulCurTotalDie		  = ulGradeDieRemainCount + ulGradeMapCount;
		
		////Stage2.0 comment by Zou Feng Sheng 20161206 //4.55T06
		//if(ulGradeMapCount < ulMinDieCountInput )
		//{
		//	szMsg.Format("Special case: Grade:%d, Prescan Die Count%ld < Min Die Count %d", ucGrade, ulGradeMapCount,ulMinDieCountInput);
		//	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//	continue;
		//}

		//Stage 2.1: Check Org. input count  //4.53D91 //4.55T11 //4.55T15
		ulOrgInputCount = m_oBinBlkMain.GrabInputCount(ucGrade);
		if(  ulFullDieCountInput > ulOrgInputCount )
		{
			szMsg.Format("Reset back input count of Grade%d to full %ld ( > Input Count %d) before checking die count", ucGrade, ulFullDieCountInput,ulOrgInputCount);
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

			//re-store input count
			m_oBinBlkMain.SaveGradeInfo(m_bEnableAliasGrade, ucGrade, ulFullDieCountInput, m_ucAliasGrade, pBTfile);
			//ulFullDieCountInput = ulOrgInputCount;
		}

		szMsg.Format("Check Die Count with Grade %d, Die(Rm:%d,Map:%d,Tol:%d),Input(Full:%d,Min:%d)",ucGrade, ulGradeDieRemainCount, ulGradeMapCount,ulCurTotalDie,ulFullDieCountInput,ulMinDieCountInput );
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		//Stage 2.2: Decide the last second frame 
		if(CheckLastSecondFrameDieFull(ulFullDieCountInput,ulMinDieCountInput, ulCurTotalDie, ulResetFullDieCount,ulGradeDieRemainCount) == FALSE)
		{
			return FALSE;
		}
		
		//Stage 2.3: Reset Input Count
		if(ulResetFullDieCount >0 )
		{

			szMsg.Format("Save input count, Grade:%d, ResetFullDieCount:%d",ucGrade, ulResetFullDieCount);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

			m_oBinBlkMain.SaveGradeInfo(m_bEnableAliasGrade, ucGrade, ulResetFullDieCount, m_ucAliasGrade, pBTfile);
		}
		else
		{
			szMsg.Format("NO Save input count on Grade:%d",ucGrade);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}

	}
	CMSFileUtility::Instance()->SaveBTConfig();


	if (ulGradeResetCount != NULL)
	{
		delete [] ulGradeResetCount;
	}

	return TRUE;
}

//4.53D90 fnc check last second frame die full
BOOL CBinTable::CheckLastSecondFrameDieFull(ULONG ulFullDieCountInput,ULONG ulMinDieCountInput, ULONG ulCurTotalDie,ULONG& ulResetFullDieCount,ULONG ulGradeDieRemainCount)
{

	CString szMsg;
	BOOL bResetFullDieCount = FALSE;
	ULONG ulMaxCount;

	if(ulFullDieCountInput <= ulMinDieCountInput )
	{
		szMsg.Format("Error: Min Die Count %ld over Full Die Count %d, Please check",  ulMinDieCountInput, ulFullDieCountInput);
		SetErrorMessage(szMsg);
		HmiMessage_Red_Back(szMsg,"Optimize Bin Count Checking");
		return FALSE;
	}

	//ulMaxCount = ulFullDieCountInput + ulMinDieCountInput*2;
	ulMaxCount = ulFullDieCountInput + ulMinDieCountInput;

	if( ulCurTotalDie < ulMaxCount )
	{
		// using this conditional logic
		if( ulCurTotalDie < ulMinDieCountInput*2) 
		{
			bResetFullDieCount = FALSE;
			szMsg.Format("CurTotalDie %d < Min count 2*%d, still unchanging full die count", ulCurTotalDie,ulMinDieCountInput);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			//HmiMessage(szMsg);
		}
		else
		{
			bResetFullDieCount = TRUE;
			szMsg.Format("Full %d + Min %d = %d > CurTotalDie %d > Min count 2*%d = %d, changing full die count", ulFullDieCountInput,ulMinDieCountInput, ulMaxCount, ulCurTotalDie,ulMinDieCountInput,ulMinDieCountInput*2);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			//HmiMessage(szMsg);
		}

	}
	else
	{
		bResetFullDieCount = FALSE;
		szMsg.Format("CurTotalDie %d > (Full %d + Min %d) = %d, still unchanging full die count",ulCurTotalDie,ulFullDieCountInput,ulMinDieCountInput,ulMaxCount);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//HmiMessage(szMsg);
	}


	if(bResetFullDieCount)
	{
		ulResetFullDieCount = ulCurTotalDie - ulMinDieCountInput;


		if( ulResetFullDieCount < ulGradeDieRemainCount) //4.55T06 20161209 debugfixed: bin table should clear bin first
		{
			ulResetFullDieCount = 0;

			szMsg.Format("ResetFullDieCount %d = (CurTotalDie %d - Min %d) < BinFrameDieRemainCount %d,So unchange full die count!",ulResetFullDieCount,ulCurTotalDie,ulMinDieCountInput,ulGradeDieRemainCount );
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}
		else
		{

		     szMsg.Format("ResetFullDieCount %d = CurTotalDie %d - Min %d",ulResetFullDieCount,ulCurTotalDie,ulMinDieCountInput );
		     CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}
		//HmiMessage(szMsg);
	}
	else
	{
		ulResetFullDieCount = 0;
	}

	return TRUE;
}
VOID CBinTable::DisplayOptimizeInfo(VOID)
{
	if ( m_oBinBlkMain.GrabAutoAssignGrade() == FALSE )
	{
		return;
	}

	BinBlksDrawing();
	WriteGradeLegend();
	LoadGradeRankID();
	LoadWaferStatistics();
}

//4.54T15
VOID CBinTable::ResetWaferEndFileData(VOID)
{
	(*m_psmfSRam)["BondHead"]["MissingDieBHZ1"]  = 0; 
	(*m_psmfSRam)["BondHead"]["MissingDieBHZ2"]  = 0; 
	(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"]	 = 0;
	(*m_psmfSRam)["BondHead"]["ColletJamBHZ2"]	 = 0;

	return;
}

// Wafer End File Generating Checking
VOID CBinTable::SetNotGenerateWaferEndFile(BOOL bNotGenFile)
{
	if (CMS896AStn::m_bWaferEndFileGenProtection == FALSE)
		return;

	BOOL bOpDisableLoadMap = (BOOL)(LONG)(*m_psmfSRam)["MSOperator"]["DisableLoadMap"];
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="WH" )	//	not allow to load map manually
	{
		bOpDisableLoadMap = TRUE;
	}

	m_bNotGenerateWaferEndFile = bNotGenFile;

	if (m_bNotGenerateWaferEndFile == TRUE)
	{
		m_WaferMapWrapper.EnableOpenButton(FALSE);
	}
	else
	{
		if (bOpDisableLoadMap == FALSE)
		{
			m_WaferMapWrapper.EnableOpenButton(TRUE);
		}
	}

	SaveLastState();
}

BOOL CBinTable::WaferEndFileGeneratingChecking()
{
	BOOL bOpDisableLoadMap = (BOOL)(LONG)(*m_psmfSRam)["MSOperator"]["DisableLoadMap"];
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_SANAN && pApp->GetProductLine()=="WH" )	//	not allow to load map manually
	{
		bOpDisableLoadMap = TRUE;
	}

	if (CMS896AStn::m_bWaferEndFileGenProtection == FALSE)
		return TRUE;

	// No map is loaded
	if (m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		if (bOpDisableLoadMap == FALSE)
		{
			m_WaferMapWrapper.EnableOpenButton(TRUE);
		}

		return TRUE;
	}

	if (m_bIfGenWaferEndFile == FALSE)
	{
		m_bNotGenerateWaferEndFile = TRUE;
		
		if (bOpDisableLoadMap == FALSE)
		{
			m_WaferMapWrapper.EnableOpenButton(TRUE);
		}

		return TRUE;
	}

	if (m_bNotGenerateWaferEndFile == TRUE)
	{
		if( GetNewPickCount()==0 && pApp->GetCustomerName()==CTM_SEMITEK && pApp->GetProductLine()=="ZJG" )
		{
			SetNotGenerateWaferEndFile(FALSE);
			return TRUE;
		}
		if ((CMS896AStn::m_bAutoGenWaferEndFile == TRUE)/* || m_bIfGenWaferEndFile*/)
		{
			INT nCol = -1;
			CString szErrMsg, szMapFilename;
			SetAlert_Red_Yellow(IDS_BT_WAFER_END_FILE_AUTO_GENERATE);
			WaferEndFileGenerating("Generate Waferend checking");
			
			szMapFilename = GetMapFileName();
			nCol = szMapFilename.ReverseFind('\\');
			if (nCol != -1)
			{
				szMapFilename = szMapFilename.Right(szMapFilename.GetLength() - nCol -1);
			}

			szErrMsg = "Wafer end file not yet generated for map filename(auto generate wafer end): " + szMapFilename;
			SetErrorMessage(szErrMsg);

			//if (m_bEjNeedReplacement == TRUE)
			//{
			//	m_bEjNeedReplacement = FALSE;
			//	//SetAlert_Red_Yellow(IDS_BH_REPLACE_EJECTOR);
			//	SetAlert_Yellow_Green(IDS_BH_REPLACE_EJECTOR); //4.51D8 BH0003
			//	SetErrorMessage("WL: autobond cycle aborted due to EJ pin lifetime & replacement alarm");
			//}
			//return TRUE;
		}
		else
		{
			INT nCol = -1;
			CString szErrMsg, szMapFilename;
			SetAlert_Red_Yellow(IDS_BT_WAFER_END_FILE_NOT_YET_GENERATED);
			
			szMapFilename = GetMapFileName();
			nCol = szMapFilename.ReverseFind('\\');
			
			if (nCol != -1)
			{
				szMapFilename = szMapFilename.Right(szMapFilename.GetLength() - nCol -1);
			}

			szErrMsg = "Wafer end file not yet generated for map filename: " + szMapFilename;
			SetErrorMessage(szErrMsg);
		}

		return FALSE;
	}

	return TRUE;
}

BOOL CBinTable::AutoWaferEndFileGenerating()	//v4.40T14	//3E
{
	BOOL bReturn = TRUE;
	BOOL bWaferEnd = TRUE;
	BOOL bGenFileOk = FALSE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->GetFeatureStatus(MS896A_FUNC_AUTO_GEN_WAFEREND_PRESCAN))
	{
		if( pApp->GetCustomerName()=="SiLan" && IsMS90HalfSortMode() )
		{
		}
		else
			return TRUE;
	}

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if ( (pUtl->GetPrescanDummyMap() || IsEnablePNP()) )	// wafer end
	{
		if( !IsMS90HalfSortMode() )
			return TRUE;
	}

	//Get Wafer End Date and Time
	GenerateWaferEndTime();

	//Generate wafer end file
	if (m_bIfGenWaferEndFile && !IsBurnIn())
	{
		if ( (bGenFileOk = WaferEndFileGenerating("Auto Generate Waferend")) == TRUE )
		{
			UpdateWaferLotLoadStatus();
		}

		WaferEndYieldCheck();
	}

	//Send wafer end info to Host
	if (!IsBurnIn())
	{
		ScanSummaryGenerating();
		SendWaferEndInfoToHost();
	}

	return TRUE;
}


BOOL CBinTable::LoadRankIDFromMap()
{
	CString szRankIDFilename, szRankIDFilePath;
	CString szFileExt;
	CString szError;

	if (CMS896AStn::m_bEnableLoadRankIDFromMap == FALSE)
	{
		return TRUE;
	}

	if (CMS896AStn::m_szRankIDFileNameInMap.IsEmpty() == TRUE)
	{
		return TRUE;
	}

	if (m_bLoadRankIDFromMap == FALSE)
	{
		return TRUE;
	}

	if (m_WaferMapWrapper.IsMapValid() == FALSE || (m_WaferMapWrapper.GetReader() == NULL))
	{
		SetErrorMessage("Invalid map file in LoadRankIDFromMap");
		SetAlert_Red_Yellow(IDS_WL_MAP_NOT_VALID);
		return FALSE;
	}

	m_WaferMapWrapper.GetHeaderInfo()->Lookup(CMS896AStn::m_szRankIDFileNameInMap, szRankIDFilename);

	if (szRankIDFilename.IsEmpty() == TRUE)
	{
		SetErrorMessage("LoadRankIDFromMap empty string");
		return FALSE;
	}

	szRankIDFilename = szRankIDFilename.Trim(' ');

	if (IfAllBinCountsAreCleared() == FALSE)
	{
		//szError = "old: " + m_szRankIDFilename + "new" + szRankIDFilename;
		//	AfxMessageBox(szError, MB_SYSTEMMODAL);
		if (szRankIDFilename == m_szRankIDFilename)
		{		
			return TRUE;
		}
		else
		{
			if (m_szRankIDFilename.IsEmpty() == TRUE)
			{
				return TRUE;
			}

			SetAlert_Red_Yellow(IDS_BT_FAIL_TO_COMPARE_BIN_FILE);
			m_WaferMapWrapper.InitMap();
			return FALSE;
		}
	}

	if (CMS896AStn::m_szRankIDFileExt.IsEmpty() == FALSE)
	{
		szFileExt = "." + CMS896AStn::m_szRankIDFileExt;
	}
	
	szRankIDFilePath = m_szRankIDFilePath + "\\" + szRankIDFilename + szFileExt;

	if (ReadRankIDFile(szRankIDFilePath) == FALSE)
	{
		CString szError;
		szError = "Load Rank ID File From Map:" + szRankIDFilePath;
		SetErrorMessage(szError);
		SetAlert_Red_Yellow(IDS_BT_FAIL_TO_LOAD_RANK_FILE);
		m_oBinBlkMain.ResetRankID();
		m_szRankIDFilename = "";
		UpdateGradeRankID();
		m_WaferMapWrapper.InitMap();
		return FALSE;
	}

	m_szRankIDFilename = szRankIDFilename;
	
	SaveLastState();
	return TRUE;
}

BOOL CBinTable::CheckValidRankIDFromCurrentMap(BOOL bClearMap)			//v4.21T4	//TongFang
{
	if (!m_bLoadRankIDFile)
	{
		return TRUE;
	}

	if (!m_WaferMapWrapper.IsMapValid() || (m_WaferMapWrapper.GetReader() == NULL))
	{
		SetErrorMessage("Invalid map file is detected in CheckValidRankIDFromCurrentMap");
		HmiMessage_Red_Back("Invalid map file for RankID checking; please reload map.", "RankID Checking");
		return FALSE;
	}

	CUIntArray aulSelectedGradeList;
	UCHAR ucSelectedGrade = 0;

	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);

	for (INT i=0; i< aulSelectedGradeList.GetSize(); i++)
	{
		ucSelectedGrade = (UCHAR)(aulSelectedGradeList.GetAt(i) - m_WaferMapWrapper.GetGradeOffset());

		if ( !m_oBinBlkMain.CheckValidRankID(ucSelectedGrade) )
		{
			CString szErr;
			szErr.Format("RankID Check: Grade #%d is not available in RankID database!", ucSelectedGrade);
			SetErrorMessage(szErr);
			HmiMessage_Red_Back(szErr, "Check Rank ID");

			if (bClearMap)
				m_WaferMapWrapper.InitMap();	//Clear current map
			return FALSE;
		}
	}

	return TRUE;
}

/*
VOID CBinTable::LOG_ReAlignBinFrame(CONST CString szMsg, CONST CString szMode)
{
	if ( CMS896AApp::m_bEnableMachineLog == TRUE )
	{
		FILE *fp;
		if ( (fp = fopen(gszUSER_DIRECTORY + "\\History\\Realign.log", szMode)) != NULL)
		{
			CTime theTime = CTime::GetCurrentTime(); 
			CString szLine;
			szLine.Format("%d-%2d-%2d (%2d:%2d:%2d) - ", 
								theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), 
								theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
			
			szLine = szLine + szMsg + "\n";
			fprintf(fp, szLine);
			fclose(fp);
		}
	}
}
*/

/*
VOID CBinTable::LOG_LoadUnloadTime(CONST CString szMsg, CONST CString szMode)
{
	if ( CMS896AApp::m_bEnableMachineLog == TRUE )
	{
		FILE *fp;
		if ( (fp = fopen(gszUSER_DIRECTORY + "\\History\\DLAStatus.log", szMode)) != NULL)
		{
			CTime theTime = CTime::GetCurrentTime(); 
			CString szLine;
			szLine.Format("%d-%2d-%2d (%2d:%2d:%2d) - ", 
								theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), 
								theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
			
			szLine = szLine + szMsg + "\n";
			fprintf(fp, szLine);
			fclose(fp);
		}
	}
}
*/


ULONG CBinTable::GetGradeLeftOnWaferMap()
{
	UCHAR ucMapGrade = m_oBinBlkMain.GrabGrade(GetBTBlock()) + m_WaferMapWrapper.GetGradeOffset();

	ULONG ulGradeLeft = 0;
	ULONG ulGradePick = 0;
	ULONG ulGradeTotal = 0;
		
	m_WaferMapWrapper.GetStatistics(ucMapGrade, ulGradeLeft, ulGradePick, ulGradeTotal);
	return ulGradeLeft;
}

BOOL CBinTable::OPENCLOSE_BT_LOG(BOOL bOpen)
{
	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\PB_OFFSET.log";

	if (bOpen)
	{
		m_fBtLog = NULL;
		errno_t nErr = fopen_s(&m_fBtLog, szLogFileName, "w+");
		if ((nErr != 0) || (m_fBtLog == NULL))
		{
			return FALSE;
		}
	}
	else
	{
		fclose(m_fBtLog);
		m_fBtLog = NULL;
	}

	return TRUE;
}


BOOL CBinTable::LOG_BT(CONST CString szMsg)
{
	if (m_fBtLog != NULL)
	{
		CString szLine;
		//szLine.Format("%d-%2d-%2d (%2d:%2d:%2d) - ", 
		//				theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		szLine = szMsg + "\n";
		fprintf(m_fBtLog, szLine);
		return TRUE;
	}
	return FALSE;
}


LONG CBinTable::CheckWtPickAndPlaceOn()
{
	IPC_CServiceMessage rReqMsg;
	//BOOL bIsPickAndPlaceOn = FALSE;
	LONG lPickAndPlaceMode = MS_PICK_N_PLACE_DISABLED;

	//Check use Pick & Place or not
	INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "IsPickAndPlaceOn", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &lPickAndPlaceMode);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return lPickAndPlaceMode;
}

BOOL CBinTable::ClearSPCData(ULONG ulBlkId, BOOL bWaitReply, BOOL bClearAll)
{
	IPC_CServiceMessage stMsg;
	CString szCommand;
	int nConvID;
	BOOL bReply;
	LONG lSPCBin = (LONG) ulBlkId;
	
	szCommand = "DeleteRuntimeData";
	
	if (bClearAll == TRUE)
		szCommand = "DeleteRuntimeAllData";

	stMsg.InitMessage(sizeof(LONG), &lSPCBin);
	
	nConvID = m_comClient.SendRequest(BOND_PR_STN, szCommand, stMsg);
	
	if (bWaitReply == FALSE)
	{
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
	else
	{
		m_comClient.ScanReplyForConvID(nConvID, 5000);
		m_comClient.ReadReplyForConvID(nConvID, stMsg);
		bReply = TRUE;
	}

	return bReply;
}

BOOL CBinTable::UpdateWaferLotLoadStatus()
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, _T("UpdateWaferLotLoadStatusCmd"), stMsg);
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

	BOOL bResult = TRUE;
	stMsg.GetMsg(sizeof(BOOL), &bResult);
	return bResult;
}


LONG CBinTable::ClrAllPhyBinCtr(CString szBinOutputFileFormat)
{
	INT nConvID = 0;
	ULONG ulClearCount = 0;
	IPC_CServiceMessage	stMsg;
	//BOOL bFcnRtn;
	CString szContent,szTitle;
	BOOL bReply = TRUE;
	BOOL bReturn = TRUE;
	BOOL bNeedToClear = FALSE;


	szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);

	//v2.70
	//Offline print label only available in Manual Clear-All-Bins fcn for DLA
	if (m_bDisableClearAllPrintLabel)
		CMS896AStn::m_bOfflinePrintLabel = TRUE;

	for (INT i = 1; i <= (INT) m_oPhyBlkMain.GetNoOfBlk(); i++)
	{
		if (ClrBinCntCmdSubTasks(i, "All Blocks", szBinOutputFileFormat, TRUE, FALSE, bNeedToClear) == TRUE)
		{
			ulClearCount++;
		}
	}

	CMS896AStn::m_bOfflinePrintLabel = FALSE;	//v2.70

	if (ulClearCount == 0)
	{
		szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);

		HmiMessage(szContent, szTitle);
		return FALSE;
	}
	else if (ulClearCount == m_oPhyBlkMain.GetNoOfBlk())
	{
		szContent.LoadString(HMB_BT_ALL_BINS_COUNTER_CLEARED);

		HmiMessage(szContent, szTitle);
		SetStatusMessage("All Bin Counters are cleared");
	}
	else
	{
		szContent.LoadString(HMB_BT_SOME_BIN_COUNTER_NOT_CLR);

		HmiMessage(szContent, szTitle);
		return FALSE;
	}

	
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

	// Reset ExArm PreLoad flags after CLEAR-ALL	//v2.67
	m_bPreLoadFullFrame = FALSE;	
	m_bPreLoadChangeFrame = FALSE;

	return TRUE;
}

/*
BOOL CBinTable::ReadRankIDFile(CString szFilename)
{
	INT nCol;
	UCHAR ucGrade;
	LONG lFormatType;
	CString szContent, szESD, szLifeTime, szName, szTapeId, szBlockSize, szTemp, szBinTableFilename;
	CString szIF;
	CStdioFile cfSelectionFile;
	BOOL bReadHeader = TRUE;
	BOOL bFoundHeaderInfo = FALSE;

	cfSelectionFile.Open(szFilename, 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone|CFile::typeText);

	cfSelectionFile.SeekToBegin();

	//Read File Header to decide which format
	cfSelectionFile.ReadString(szContent);	

	if (szContent.Find("Type ID:") != -1)
	{
		lFormatType = BT_EXTRA_RANK_FILE;
	}
	else if (szContent.Find("Sorter Grade Table:") != -1)
	{
		nCol = szContent.Find(',');
		
		if (nCol != -1)
		{
			if (szContent.GetLength() > nCol + 1)
			{
				szBinTableFilename = szContent.Mid(nCol + 1);
				m_oBinBlkMain.SetBinTableFilename(szBinTableFilename);
			}
		}

		lFormatType = BT_EXTRA_GRADE_FILE;
	}
	else if (szContent.Find("TypeFileName:") != -1)
	{
		lFormatType = BT_EXTRA_TYPE_FILE;
	}
	else if (szContent.Find("BIN\tRANKCODE") != -1)	//Sharp
	{
		lFormatType = BT_EXTRA_RANK_FILE_2;
	}
	else
	{
		lFormatType = BT_EXTRA_UNKNOWN_FILE;	
		cfSelectionFile.Close();
		
		return FALSE;
	}

	
	//Read header information
	if ( lFormatType == BT_EXTRA_RANK_FILE )
	{
		//Read Bin:,Spec ID: (This is no use)
		cfSelectionFile.ReadString(szContent);
	}
	else if ( lFormatType == BT_EXTRA_TYPE_FILE )
	{
		//Read "Time" (This is no use)
		cfSelectionFile.ReadString(szContent);

		//Read "Tester Model" (This is no use)
		cfSelectionFile.ReadString(szContent);

		//Read "Machine" (This is no use)
		cfSelectionFile.ReadString(szContent);

		//Read Blank line (This is no use)
		cfSelectionFile.ReadString(szContent);

		//Read "Color, Grade No, Type 1, Type 2" (This is no use)
		cfSelectionFile.ReadString(szContent);
	}
	else if ( lFormatType == BT_EXTRA_RANK_FILE_2 )
	{
		//no need to remove header
	}
	else
	{
		
		while(cfSelectionFile.ReadString(szContent))
		{
			if (szContent.Find("Life time:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szLifeTime = szContent.Mid(nCol+1);
					m_oBinBlkMain.SetLifeTime(szLifeTime);
				}

				bFoundHeaderInfo = TRUE;
			}
			else if (szContent.Find("Tape Id:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szTapeId = szContent.Mid(nCol+1);
					//m_oBinBlkMain.SetLifeTime(szLifeTime);
					m_oBinBlkMain.SetTapeID(szTapeId);
				}

				bFoundHeaderInfo = TRUE;
			}
			else if (szContent.Find("Life time:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szLifeTime = szContent.Mid(nCol+1);
					m_oBinBlkMain.SetLifeTime(szLifeTime);
				}

				bFoundHeaderInfo = TRUE;
			}
			else if (szContent.Find("ESD:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szESD = szContent.Mid(nCol+1);
					m_oBinBlkMain.SetESD(szESD);
				}

				bFoundHeaderInfo = TRUE;
			} 
			else if (szContent.Find("IF:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szIF = szContent.Mid(nCol+1);
					m_oBinBlkMain.SetIF(szIF);
					//AfxMessageBox(szIF, MB_SYSTEMMODAL);
				}

				bFoundHeaderInfo = TRUE;
				
			}
			else if (szContent.Find("Grade no:,Bin Grade:") != -1)
			{
				if (bFoundHeaderInfo == FALSE)
				{
					lFormatType = BT_EXTRA_UNKNOWN_FILE;
				}

				break;
			}
		}

		if (lFormatType == BT_EXTRA_UNKNOWN_FILE)
		{
			cfSelectionFile.Close();
		}
	}


	//Read File content
	if ( lFormatType == BT_EXTRA_TYPE_FILE )
	{
		while(cfSelectionFile.ReadString(szContent))
		{
			//This is End
			nCol = szContent.Find("END");
			if (nCol != -1)
			{
				break;
			}

			nCol = szContent.Find(",");
			if (nCol != -1)
			{
				szName = szContent.Left(nCol) + ";";
				szContent = szContent.Mid(nCol+1);

				nCol = szContent.Find(",");
				if ( nCol != -1 )
				{
					ucGrade	= (UCHAR)atoi((LPCTSTR)szContent.Left(nCol));
					szContent = szContent.Mid(nCol+1);
					szContent.Replace("," , ";");

					szName	+= szContent;
					m_oBinBlkMain.SetRankID(ucGrade, szName);
				}
			}
		}
	}
	else if ( lFormatType == BT_EXTRA_RANK_FILE_2 )
	{
		while(cfSelectionFile.ReadString(szContent))
		{
			szBlockSize = "";
			szName = "";

			nCol = szContent.Find("\t");
			if (nCol != -1)
			{
				ucGrade	= (UCHAR)atoi((LPCTSTR)szContent.Left(nCol));
				szName	= szContent.Mid(nCol+1);
				szContent = szName;
				
				nCol = szContent.Find(",");
				
				if (nCol != -1)
				{
					szName = szContent.Left(nCol);
					szTemp = szContent.Mid(nCol + 1);
					szContent = szTemp;

					nCol = szContent.Find(",");

					if (nCol != -1)
					{
						szTemp = szContent.Left(nCol);
						szBlockSize = szContent.Mid(nCol + 1);
					}
				}
				
				m_oBinBlkMain.SetRankID(ucGrade, szName);
				m_oBinBlkMain.SetBlockSize(ucGrade, szBlockSize);
			}
		}
	}
	else
	{
		while(cfSelectionFile.ReadString(szContent))
		{
			szBlockSize = "";
			szName = "";

			nCol = szContent.Find(",");
			if (nCol != -1)
			{
				ucGrade	= (UCHAR)atoi((LPCTSTR)szContent.Left(nCol));
				szName	= szContent.Mid(nCol+1);
				szContent = szName;
				
				nCol = szContent.Find(",");
				
				if (nCol != -1)
				{
					szName = szContent.Left(nCol);
					szTemp = szContent.Mid(nCol + 1);
					szContent = szTemp;

					nCol = szContent.Find(",");

					if (nCol != -1)
					{
						szTemp = szContent.Left(nCol);
						szBlockSize = szContent.Mid(nCol + 1);
					}
				}
				
				m_oBinBlkMain.SetRankID(ucGrade, szName);
				m_oBinBlkMain.SetBlockSize(ucGrade, szBlockSize);
			}
		}
	}

	cfSelectionFile.Close();
	return TRUE;
}
*/

BOOL CBinTable::ReadRankIDFile(CString szFilename)
{
	INT nCol;
	INT nGrade;
	UCHAR ucGrade;
	LONG lFormatType;
	CString szContent, szESD, szLifeTime, szName, szTapeId, szBlockSize, szTemp, szBinTableFilename;
	CString szIF;
	CStdioFile cfSelectionFile;
	BOOL bReadHeader = TRUE;
	BOOL bFoundHeaderInfo = FALSE;
	CString szSerialNo;

	if (cfSelectionFile.Open(szFilename, 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone|CFile::typeText) == FALSE)
	{
		return FALSE;
	}


	cfSelectionFile.SeekToBegin();

	// ------------ Read File Header to decide the format -------------//
	cfSelectionFile.ReadString(szContent);	

	if (szContent.Find("Type ID:") != -1)
	{
		lFormatType = BT_EXTRA_RANK_FILE;
	}
	else if (szContent.Find("Sorter Grade Table:") != -1)
	{
		nCol = szContent.Find(',');
		
		if (nCol != -1)
		{
			if (szContent.GetLength() > nCol + 1)
			{
				szBinTableFilename = szContent.Mid(nCol + 1);
				m_oBinBlkMain.SetBinTableFilename(szBinTableFilename);
			}
		}

		lFormatType = BT_EXTRA_GRADE_FILE;
	}
	else if (szContent.Find("TypeFileName:") != -1)
	{
		lFormatType = BT_EXTRA_TYPE_FILE;
	}
	else if (szContent.Find("BIN\tRANKCODE") != -1)	//Sharp
	{
		lFormatType = BT_EXTRA_RANK_FILE_2;
	}
	else
	{
		lFormatType = BT_EXTRA_UNKNOWN_FILE;	
		cfSelectionFile.Close();
		
		return FALSE;
	}

	
	// ------------ Read File Content Portion -------------//
	if ( lFormatType == BT_EXTRA_RANK_FILE )
	{
		//Read Bin:,Spec ID: (This is no use)
		cfSelectionFile.ReadString(szContent);
	}
	else if ( lFormatType == BT_EXTRA_TYPE_FILE )
	{
		//Read "Time" (This is no use)
		cfSelectionFile.ReadString(szContent);

		//Read "Tester Model" (This is no use)
		cfSelectionFile.ReadString(szContent);

		//Read "Machine" (This is no use)
		cfSelectionFile.ReadString(szContent);

		//Read Blank line (This is no use)
		cfSelectionFile.ReadString(szContent);

		//Read "Color, Grade No, Type 1, Type 2" (This is no use)
		cfSelectionFile.ReadString(szContent);
	}
	else if ( lFormatType == BT_EXTRA_RANK_FILE_2 )
	{
		//no need to remove header
	}
	else
	{
		while(cfSelectionFile.ReadString(szContent))
		{
			//AfxMessageBox(szContent, MB_SYSTEMMODAL);

			if (szContent.Find("Life time:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szLifeTime = szContent.Mid(nCol+1);
					m_oBinBlkMain.SetLifeTime(szLifeTime);
				}

				bFoundHeaderInfo = TRUE;
			}
			else if (szContent.Find("Tape Id:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szTapeId = szContent.Mid(nCol+1);
					//m_oBinBlkMain.SetLifeTime(szLifeTime);
					m_oBinBlkMain.SetTapeID(szTapeId);
				}

				bFoundHeaderInfo = TRUE;
			}
			else if (szContent.Find("Life time:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szLifeTime = szContent.Mid(nCol+1);
					m_oBinBlkMain.SetLifeTime(szLifeTime);
				}

				bFoundHeaderInfo = TRUE;
			}
			else if (szContent.Find("ESD:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szESD = szContent.Mid(nCol+1);
					m_oBinBlkMain.SetESD(szESD);
				}

				bFoundHeaderInfo = TRUE;
			} 
			else if (szContent.Find("IF:") != -1)
			{
				nCol = szContent.Find(",");
				if (nCol != -1)
				{
					szIF = szContent.Mid(nCol+1);
					m_oBinBlkMain.SetIF(szIF);
					//AfxMessageBox(szIF, MB_SYSTEMMODAL);
				}

				bFoundHeaderInfo = TRUE;
				
			}
			else if (szContent.Find("BIN,Series No") != -1)
			{
				lFormatType = BT_EXTRA_GRADE_FILE_2;
			}
			else if (szContent.Find("Grade no:,Bin Grade:") != -1)
			{
				if (bFoundHeaderInfo == FALSE)
				{
					lFormatType = BT_EXTRA_UNKNOWN_FILE;
				}
			}
		}

		if (lFormatType == BT_EXTRA_UNKNOWN_FILE)
		{
			cfSelectionFile.Close();
		}
	}

	// ------------ Read File Content Portion -------------//

	if ( lFormatType == BT_EXTRA_TYPE_FILE )
	{
		while(cfSelectionFile.ReadString(szContent))
		{
			//This is End
			nCol = szContent.Find("END");
			if (nCol != -1)
			{
				break;
			}

			nCol = szContent.Find(",");
			if (nCol != -1)
			{
				szName = szContent.Left(nCol) + ";";
				szContent = szContent.Mid(nCol+1);

				nCol = szContent.Find(",");
				if ( nCol != -1 )
				{
					ucGrade	= (UCHAR)atoi((LPCTSTR)szContent.Left(nCol));
					szContent = szContent.Mid(nCol+1);
					szContent.Replace("," , ";");

					szName	+= szContent;
					m_oBinBlkMain.SetRankID(ucGrade, szName);
				}
			}
		}
	}
	else if ( lFormatType == BT_EXTRA_RANK_FILE_2 )
	{
		while(cfSelectionFile.ReadString(szContent))
		{
			szBlockSize = "";
			szName = "";

			nCol = szContent.Find("\t");
			if (nCol != -1)
			{
				ucGrade	= (UCHAR)atoi((LPCTSTR)szContent.Left(nCol));
				szName	= szContent.Mid(nCol+1);
				szContent = szName;
				
				nCol = szContent.Find(",");
				
				if (nCol != -1)
				{
					szName = szContent.Left(nCol);
					szTemp = szContent.Mid(nCol + 1);
					szContent = szTemp;

					nCol = szContent.Find(",");

					if (nCol != -1)
					{
						szTemp = szContent.Left(nCol);
						szBlockSize = szContent.Mid(nCol + 1);
					}
				}
				
				m_oBinBlkMain.SetRankID(ucGrade, szName);
				m_oBinBlkMain.SetBlockSize(ucGrade, szBlockSize);
			}
		}
	}
	else if ( lFormatType == BT_EXTRA_GRADE_FILE_2 )
	{
		cfSelectionFile.SeekToBegin();

		while(cfSelectionFile.ReadString(szContent))
		{
			if (szContent.Find("BIN,Series No") != -1)
			{
				break;
			}
		}

		nGrade = 0;
		ucGrade = 0;

		while(cfSelectionFile.ReadString(szContent))
		{
			if (szContent == "BIN_Fail")
			{
				break;
			}

			ucGrade = ucGrade + 1;
			
			nCol = szContent.Find(",");
			
			if (nCol != -1)
			{
				szContent = szContent.Right(szContent.GetLength() - nCol -1);

				nCol = szContent.Find(",");

				if (nCol != -1)
				{
					szSerialNo = szContent.Left(nCol);
					m_oBinBlkMain.SetRankID(ucGrade, szSerialNo);
				}
			}
		}
	}
	else
	{
		cfSelectionFile.SeekToBegin();

		while(cfSelectionFile.ReadString(szContent))
		{
			if (szContent.Find("Grade no:,Bin Grade:") != -1)
			{
				break;
			}
		}

		while(cfSelectionFile.ReadString(szContent))
		{
			szBlockSize = "";
			szName = "";

			nCol = szContent.Find(",");
			if (nCol != -1)
			{
				ucGrade	= (UCHAR)atoi((LPCTSTR)szContent.Left(nCol));
				szName	= szContent.Mid(nCol+1);
				szContent = szName;
				
				nCol = szContent.Find(",");
				
				if (nCol != -1)
				{
					szName = szContent.Left(nCol);
					szTemp = szContent.Mid(nCol + 1);
					szContent = szTemp;

					nCol = szContent.Find(",");

					if (nCol != -1)
					{
						szTemp = szContent.Left(nCol);
						szBlockSize = szContent.Mid(nCol + 1);
					}
				}
				
				m_oBinBlkMain.SetRankID(ucGrade, szName);
				m_oBinBlkMain.SetBlockSize(ucGrade, szBlockSize);
			}
		}
	}

	cfSelectionFile.Close();
	return TRUE;
}

BOOL CBinTable::AutoSetupBinBlkUsingGradeRankFile()
{
	CString szTitle, szContent;
	//BOOL bSetupOk;
	CString szTemplateName;
	BOOL bIfBondingInProcess = FALSE;

	ULONG ulTotalNoOfBlk = m_oBinBlkMain.GetNoOfBlk();

	for (INT i = 1; i <= (INT)ulTotalNoOfBlk; i++)
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
		{
			bIfBondingInProcess = TRUE;
			break;
		}
	}

	if (bIfBondingInProcess)
	{
		//Block settings can't be edited because it contains bin blocks.
		SetAlert(IDS_BT_DENYBLKSETTINGSEDIT1);
		return FALSE;
	}
	
	// Load Grade Ranke File First
	if (LoadGradeRank() != TRUE)
	{
		return FALSE;
	}

	for (INT i = 1; i <= (INT) ulTotalNoOfBlk; i++)
	{
		
		UCHAR ucGrade = m_oBinBlkMain.GrabGrade(i);
		szTemplateName = m_oBinBlkMain.GrabBlockSize(ucGrade);

		if (SetupSingleBinBlkUsingTemplate(szTemplateName, ulTotalNoOfBlk, i, i, ucGrade) == FALSE)
		{
			
			szTitle.LoadString(HMB_BT_LOAD_TEMPLATE);
			szContent.Format("Fail to apply template to bin block:%d", i);
			HmiMessage(szContent , szTitle , glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

			return FALSE;

		}
	
		if (UpdateGradeInputCountUsingTemplate(szTemplateName, ucGrade) == FALSE)
		{
			szTitle.LoadString(HMB_BT_LOAD_TEMPLATE);
			szContent.Format("Fail to apply template to bin block:%d", i);
			
			HmiMessage(szContent , szTitle , glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

			return FALSE;
		}
	}


	szTitle.LoadString(HMB_BT_LOAD_TEMPLATE);
	szContent.LoadString(HMB_BT_LOAD_TEMPLATE_COMPLETE);

	HmiMessage(szContent , szTitle , glHMI_MBX_OK, 
						glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	return TRUE;
}

BOOL CBinTable::SetupSingleBinBlkUsingTemplate(CString szTemplateName,ULONG ulNoOfBinBlk,
												ULONG ulBinBlkToSetup, ULONG ulSourcePhyBlk, 
												UCHAR ucGrade)
{
	BOOL bLoadTemplate = FALSE, bSetupRtnValue = FALSE;
	LONG lSourcePhyBlkULY, lSourcePhyBlkLRY; 
	LONG lIsValidLR, lIsValidUL;
	LONG lBinBlkULX, lBinBlkULY;
	LONG lBinBlkLRX, lBinBlkLRY;
	CString szWalkPath;
	ULONG ulDiePerRow, ulDiePerCol;
	DOUBLE dDiePitchX = 0, dDiePitchY = 0;
	ULONG ulSkipUnit, ulMaxUnit;
	ULONG ulWalkPath;
	ULONG ulInputCount;
	BOOL bEnableFirstRowColSkipPattern;
	ULONG ulFirstRowColSkipUnit;
	BOOL bCentralizedBondArea;
	BOOL bTeachWithPhysicalBlk;
	BOOL bUseBlockCornerAsFirstDiePos;
	BOOL bEnableWafflePad;
	LONG lWafflePadDistX;
	LONG lWafflePadDistY;
	LONG lWafflePadSizeX;
	LONG lWafflePadSizeY;
	
	bLoadTemplate = m_oBinBlkMain.LoadBinBlkTemplate(szTemplateName, lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY,
						szWalkPath,	ulDiePerRow, ulDiePerCol, 
						dDiePitchX, dDiePitchY ,ulSkipUnit, ulMaxUnit,
						ulInputCount, bEnableFirstRowColSkipPattern, ulFirstRowColSkipUnit, 
						bCentralizedBondArea, bTeachWithPhysicalBlk, bUseBlockCornerAsFirstDiePos, bEnableWafflePad,
						lWafflePadSizeX, lWafflePadSizeY, lWafflePadDistX, lWafflePadDistY);

	
	if (bLoadTemplate == FALSE)
		return FALSE;

	//Update WalkPath
	ulWalkPath = GetWalkPathWithHMIName(szWalkPath);

	if (! m_oPhyBlkMain.GrabIsSetup(ulSourcePhyBlk))
	{
		SetAlert(IDS_BT_SOURCE_PHYBLK_NOT_SETUP);
		return FALSE;
	}

	//Upper Left & Lower Right validation
	lIsValidUL = m_oBinBlkMain.ValidateSingleSetupUL_BL(&m_oPhyBlkMain, 
		ulBinBlkToSetup, lBinBlkULX, lBinBlkULY, ulSourcePhyBlk);

	if (lIsValidUL == -1)
	{
		//HmiMessage("Error: Inputted upper left is not on the indicated source physical block!");
		SetAlert(IDS_BT_ULCHECK4);
		return FALSE;
	}
	else if (lIsValidUL == -2)
	{
		//HmiMessage("Error: Inputted upper left overlaps with existing bin block!");
		SetAlert(IDS_BT_ULCHECK5);
		return FALSE;
	}
	else if (lIsValidUL == 1)
	{
		lIsValidLR = m_oBinBlkMain.ValidateSingleSetupLR_BL(&m_oPhyBlkMain,
			ulBinBlkToSetup, lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY,
			ulSourcePhyBlk);

		if (lIsValidLR == -1)
		{	
			//HmiMessage("Error: Inputted lower right is not on the indicated source physical block!");
			SetAlert(IDS_BT_LRCHECK5);
			return FALSE;
		}
		else if (lIsValidLR == -2)
		{
			//HmiMessage("Error: Inputted lower right overlaps with existing bin block!");
			SetAlert(IDS_BT_LRCHECK6);
			return FALSE;
		}
		else if (lIsValidLR == -3)
		{
			//HmiMessage("Error: Inputted upper right overlaps with existing bin block!");
			SetAlert(IDS_BT_URCHECK2);
			return FALSE;
		}
		else if (lIsValidLR == -4)
		{
			//HmiMessage("Error: Inputted lower left is within existing bin block!");
			SetAlert(IDS_BT_LLCHECK1);
			return FALSE;
		}
		else if (lIsValidLR == -5)
		{
			//HmiMessage("Error: Left boundary of existing bin block is within blk to teach!");
			SetAlert(IDS_BT_LEFTBOUNDARYCHK);
			return FALSE;
		}
		else if (lIsValidLR == -6)
		{
			//HmiMessage("Error: Right boundary of existing bin block is within blk to teach!");
			SetAlert(IDS_BT_RIGHTBOUNDARYCHK);
			return FALSE;
		}
		else if (lIsValidLR == -7)
		{
			//HmiMessage("Error: Upper boundary of existing bin block is within blk to teach!");
			SetAlert(IDS_BT_UPPERBOUNDARYCHK);
			return FALSE;
		}
		else if (lIsValidLR == -8)
		{
			//HmiMessage("Error: Lower boundary of existing bin block is within blk to teach!");
			SetAlert(IDS_BT_LOWERBOUNDARYCHK);
			return FALSE;
		}
		else if ((lIsValidLR == -9) || (lIsValidLR == -10))
		{
			//HmiMessage("Error: Block to teach overlaps within existing block!");
			SetAlert(IDS_BT_OVERLAPPINGCHK1);
			return FALSE;
		}
		else if (lIsValidLR == -11)
		{
			//HmiMessage("Error: Lower right is not at lower-right corner!");
			SetAlert(IDS_BT_LRCHECK2);
			return FALSE;
		}
		else if (lIsValidLR == 1)
		{
			lSourcePhyBlkULY = m_oPhyBlkMain.GrabBlkUpperLeftY(ulSourcePhyBlk);
			lSourcePhyBlkLRY = m_oPhyBlkMain.GrabBlkLowerRightY(ulSourcePhyBlk);

			m_oBinBlkMain.SetUserRowCol(ulBinBlkToSetup, ulDiePerRow, ulDiePerCol);

			
			bSetupRtnValue = m_oBinBlkMain.SetupSingleBlk(ulNoOfBinBlk, 
				ulBinBlkToSetup, lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY, 
				ucGrade, ulWalkPath, dDiePitchX, dDiePitchY, ulDiePerRow,
				ulSourcePhyBlk, lSourcePhyBlkULY, lSourcePhyBlkLRY, ulSkipUnit,
				ulMaxUnit, FALSE, bEnableFirstRowColSkipPattern, ulFirstRowColSkipUnit, 
				bCentralizedBondArea, bTeachWithPhysicalBlk, TRUE, bUseBlockCornerAsFirstDiePos,
				bEnableWafflePad,  lWafflePadDistX, lWafflePadDistY, lWafflePadSizeX, lWafflePadSizeY,
				m_bNoReturnTravel, m_bUsePt5UmInDiePitchX, m_bUsePt5UmInDiePitchY,
				m_dBondAreaOffsetXInUm, m_dBondAreaOffsetYInUm);

			if (bSetupRtnValue == FALSE)
				return FALSE;

			/*
			//Update screen display
			m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBinBlkToSetup); 
			m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(ulBinBlkToSetup);
			m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(ulBinBlkToSetup);

			BinBlksDrawing_BL(m_ulBinBlkToSetup);
			*/

			WriteGradeLegend();
			
			LoadGradeRankID();
			LoadWaferStatistics();
		}
	}

	GetWafflePadSettings();

	return TRUE;
}

BOOL CBinTable::UpdateGradeInputCountUsingTemplate(CString szTemplateName, UCHAR ucGrade)
{
	BOOL bLoadTemplate = FALSE;
	LONG lBinBlkULX, lBinBlkULY;
	LONG lBinBlkLRX, lBinBlkLRY;
	CString szWalkPath;
	ULONG ulDiePerRow, ulDiePerCol;
	DOUBLE dDiePitchX = 0 , dDiePitchY = 0;
	ULONG ulSkipUnit, ulMaxUnit;
	//ULONG ulWalkPath;
	ULONG ulInputCount;
	BOOL bEnableFirstRowColSkipPattern;
	ULONG ulFirstRowColSkipUnit;
	BOOL bCentralizedBondArea;
	BOOL bTeachWithPhysicalBlk;
	BOOL bUseBlockCornerAsFirstDiePos;
	BOOL bEnableWafflePad;
	LONG lWafflePadDistX;
	LONG lWafflePadDistY;
	LONG lWafflePadSizeX;
	LONG lWafflePadSizeY;

	bLoadTemplate = m_oBinBlkMain.LoadBinBlkTemplate(szTemplateName, lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY,
						szWalkPath,	ulDiePerRow, ulDiePerCol, 
						dDiePitchX, dDiePitchY ,ulSkipUnit, ulMaxUnit,
						ulInputCount, bEnableFirstRowColSkipPattern, 
						ulFirstRowColSkipUnit, bCentralizedBondArea,
						bTeachWithPhysicalBlk, bUseBlockCornerAsFirstDiePos, bEnableWafflePad,
						lWafflePadSizeX, lWafflePadSizeY, lWafflePadDistX, lWafflePadDistY);

	if (bLoadTemplate == FALSE)
		return FALSE;

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	
	if ( ucGrade > 0)
	{
		if (ulInputCount > m_oBinBlkMain.GrabGradeCapacity(ucGrade))
		{
			ulInputCount = m_oBinBlkMain.GrabGradeCapacity(ucGrade);
		}
		
		// No AliasGrade is allowed, hard code alias grade to 0
		m_oBinBlkMain.SaveGradeInfo(FALSE, ucGrade, ulInputCount, 0, pBTfile);	//v4.52A8
	}
	else
	{
		for (INT i=1; i<=128; i++)
		{
			if (ulInputCount > m_oBinBlkMain.GrabGradeCapacity(i))
			{
				ulInputCount = m_oBinBlkMain.GrabGradeCapacity(i);
			}

			// No AliasGrade is allowed, hard code alias grade to 0
			m_oBinBlkMain.SaveGradeInfo(FALSE, i, m_ulInputCount, 0, pBTfile);	//v4.52A8
		}
	}
	
	/*
	// update the max frame die count as input count of the template
	if (m_ulEnableOptimizeBinCount >= TRUE)
	{
		m_oBinBlkMain.SetMaxFrameDieCount(ucGrade,ulInputCount);
	}
	*/

	CMSFileUtility::Instance()->SaveBTConfig();
	return TRUE;
}

INT CBinTable::LoadGradeRank()
{
	LONG lFormatType = BT_EXTRA_RANK_FILE;

	CString szColumnFilePath, szColumnFilename, szContent;
	CString szName, szLifeTime, szESD;


	CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
	((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

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
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , 
						szFilters, pMainWnd, 0);

	dlgFile.m_ofn.lpstrInitialDir = szColumnFilePath;
	//dlgFile.m_ofn.lpstrDefExt = "csv";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	if (nReturn == IDCANCEL)
	{
		pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);
		return FALSE;
	}
	else if ( nReturn == IDOK )
	{
		szColumnFilename = dlgFile.GetPathName();

		SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

		m_oBinBlkMain.ResetRankID();

		if (ReadRankIDFile(szColumnFilename) == FALSE)
		{
			//HmiMessage("Invalid file format!\nPlease check","Load NameFile Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);
			SetAlert(IDS_BT_INVALID_FILE_FORMAT);
			pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);
			return FALSE;
		}

		CMSLogFileUtility::Instance()->MS_LogOperation("Load RANKID file done - " + szColumnFilename);		//v4.21T4
		UpdateGradeRankID();
	}

	pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);
	return TRUE;
}


BOOL CBinTable::ShowBinBlkTemplateData()
{
	CStringArray szaHeader, szaData, szaDataSum;
	CString szTemplateNameFromFile, szTemplateIndex;
	LONG lTotalNoOfTemplate;

	m_BinBlkTemplateWrapper.Initialize("BinBlkTemplate");
	m_BinBlkTemplateWrapper.SetScrollBarSize(15);
	m_BinBlkTemplateWrapper.RemoveAllData();

	//Statistics Header
	szaHeader.Add("Name");
	m_BinBlkTemplateWrapper.SetHeader(8404992,10987412,"Template No", 15, szaHeader);

	CMSFileUtility::Instance()->LoadBinBlkTemplateConfig();
	CStringMapFile* pBinBlkTemplate = CMSFileUtility::Instance()->GetBinBlkTemplateConfigFile();

	if ( pBinBlkTemplate == NULL )
		return TRUE;
	
	lTotalNoOfTemplate = (*pBinBlkTemplate)["TemplateInfo"]["Total No Of Template"];
	
	for (INT i=1; i<= (INT) lTotalNoOfTemplate; i++)
	{
		szTemplateIndex.Format("%d", i);
		szTemplateNameFromFile = (*pBinBlkTemplate)["TemplateInfo"]["TemplateName"][szTemplateIndex];
		szaData.Add(szTemplateNameFromFile);
		m_BinBlkTemplateWrapper.AddData(8404992, 10987412, (UCHAR)i, 15, szaData);
		szaData.RemoveAll();
	}

	///szTempValue.Format("%d", lTotalWafer);
	//szaDataSum.Add(szTempValue);

	szaDataSum.Add(" ");

	m_BinBlkTemplateWrapper.SetSum(8404992, 10987412, "", 15, szaDataSum);
	m_BinBlkTemplateWrapper.UpdateTotal();

	CMSFileUtility::Instance()->CloseBinBlkTemplateConfig();

	return TRUE;
}

BOOL CBinTable::AutoGenerateBinBlkSummary()
{
	static int nLastMin = 0;
	
	CTime theTime = CTime::GetCurrentTime();

	if ( theTime.GetMinute() != nLastMin )
	{
		CString szMachineNo;
		szMachineNo	= GetMachineNo();

		if (m_oBinBlkMain.AutoGenBinSummaryFile(m_szBinSummaryOutputPath, szMachineNo) == TRUE)
		{
			m_oBinBlkMain.UpdateLastGenSummaryTime();
			m_szLastGenSummaryTime = m_oBinBlkMain.GetLastGenSummaryTime();
			SaveLastState();
		}
	}

	nLastMin = theTime.GetMinute();

	return TRUE;
}


BOOL CBinTable::UpdateGenerateBinBlkSummaryTime()
{
	INT nCol = -1;
	INT nHour = 0;
	INT nMin = 0;

	m_oBinBlkMain.SetBinBlkSummaryPeriodNum(m_lGenSummaryPeriodNum);

	// check the input time
	for (INT i=0; i<m_lGenSummaryPeriodNum; i++)
	{
		nCol = m_szBinSummaryGenTime[i].Find(':');

		if (nCol == -1 || m_szBinSummaryGenTime[i].GetLength() < nCol)
		{
			//m_szBinSummaryGenTime[i] = "";
			for (INT i = 0; i<BT_SUMMARY_TIME_NO; i++)
			{
				m_szBinSummaryGenTime[i] = "";
			}

			m_lGenSummaryPeriodNum = 0;
			m_oBinBlkMain.SetBinBlkSummaryPeriodNum(m_lGenSummaryPeriodNum);

			return FALSE;
		}

		nHour = atoi(m_szBinSummaryGenTime[i].Left(nCol));
		nMin = atoi(m_szBinSummaryGenTime[i].Mid(nCol + 1));

		if (nHour > 24 || nHour < 0 || nMin > 60 || nMin < 0)
		{
			for (INT i = 0; i<BT_SUMMARY_TIME_NO; i++)
			{
				m_szBinSummaryGenTime[i] = "";
			}

			m_lGenSummaryPeriodNum = 0;
			m_oBinBlkMain.SetBinBlkSummaryPeriodNum(m_lGenSummaryPeriodNum);

			return FALSE;
		}
	}

	// Clear unused time
	if (m_lGenSummaryPeriodNum < BT_SUMMARY_TIME_NO)
	{
		for (INT i=m_lGenSummaryPeriodNum; i<BT_SUMMARY_TIME_NO; i++)
		{
			m_szBinSummaryGenTime[i] = "";
		}
	}
	
	// Update the time to bin blk main

	m_oBinBlkMain.SetBinSummaryGenTime(m_szBinSummaryGenTime);

	// update save time as last gen summary file time
	m_oBinBlkMain.UpdateLastGenSummaryTime();
	m_szLastGenSummaryTime = m_oBinBlkMain.GetLastGenSummaryTime();

	return TRUE;
}

VOID CBinTable::UpdateAmiFileChecking()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG lAmiCount				= (*m_psmfSRam)["MS896A"]["AmiFile"]["MaxCount"];
	BOOL bChangeMaxCountWithAmi = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["AmiFile"]["ChangeMaxCountWithAmi"];

	if( pApp->GetCustomerName() == "Genesis" && lAmiCount != 0 &&	bChangeMaxCountWithAmi)
	{
		(*m_psmfSRam)["MS896A"]["AmiFile"]["DiePitchX"] = m_oBinBlkMain.GrabDDiePitchX(1); 
		(*m_psmfSRam)["MS896A"]["AmiFile"]["DiePitchY"] = m_oBinBlkMain.GrabDDiePitchY(1);
		
		m_ulInputCount =m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(1));
		m_ulInputCount = (ULONG)min(lAmiCount, (LONG)m_ulInputCount);

		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		for (int i = 1; i<= MS_MAX_BIN; i++)
		{
			ULONG ulInputCount = m_ulInputCount;
			if (ulInputCount > m_oBinBlkMain.GrabGradeCapacity(i))
			{
				ulInputCount = m_oBinBlkMain.GrabGradeCapacity(i);
				if (i == 1)
				{
					m_ulInputCount = ulInputCount;
				}
			}
			m_oBinBlkMain.SaveGradeInfo(m_bEnableAliasGrade, i, ulInputCount, m_ucAliasGrade, pBTfile);		//v4.52A8
		}
		CMSFileUtility::Instance()->SaveBTConfig();

		(*m_psmfSRam)["MS896A"]["AmiFile"]["MaxCount"] = m_ulInputCount;
		(*m_psmfSRam)["MS896A"]["AmiFile"]["TotalBinNo"] = m_oBinBlkMain.GetNoOfBlk();
	}
	else
	{
		(*m_psmfSRam)["MS896A"]["AmiFile"]["DiePitchX"] = m_oBinBlkMain.GrabDDiePitchX(1); 
		(*m_psmfSRam)["MS896A"]["AmiFile"]["DiePitchY"] = m_oBinBlkMain.GrabDDiePitchY(1);
		(*m_psmfSRam)["MS896A"]["AmiFile"]["MaxCount"] = m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(1));
		(*m_psmfSRam)["MS896A"]["AmiFile"]["TotalBinNo"] = m_oBinBlkMain.GetNoOfBlk();
	}
}

BOOL CBinTable::SaveGradeMapTableData(UCHAR ucGrade, USHORT usOriginalGrade)
{
	CStringMapFile pFile;
	if ( pFile.Open(MSD_MAP_TABLE_FILE, FALSE, TRUE) != 1 )
	{
		return FALSE;
	}

	(pFile)["Map"][ucGrade] = (LONG) usOriginalGrade;

	pFile.Update();
	pFile.Close();
	return TRUE;
}

BOOL CBinTable::DynamicGradeAssign(CArray<BIN_FILE_INFO, BIN_FILE_INFO>& aGradeInfo)
{
	BIN_FILE_INFO stInfo;
	UCHAR ucGrade = '0';
	BOOL bEnableGradeAssign = FALSE;

	bEnableGradeAssign = m_BinSummaryFile.GetEnableGradeAssign();
	
	if (CMS896AStn::m_bUseOptBinCountDynAssignGrade == TRUE)
	{
		if (bEnableGradeAssign == FALSE)
		{
			return TRUE;	
		}

		CArray<BIN_FILE_INFO, BIN_FILE_INFO> aNewGradeInfo;
		aNewGradeInfo.RemoveAll();

		// reset and remove grade map before performing any grade map
		DeleteFile(MSD_MAP_TABLE_FILE);

		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		for (ULONG i=0; i< m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			m_oBinBlkMain.SetGradeIsAssigned(m_oBinBlkMain.GrabGrade(i+1), FALSE, pBTfile);		//v4.52A8
		}

		m_WaferMapWrapper.SetGradeToGradeMap(0 + m_WaferMapWrapper.GetGradeOffset(), 
												0 +  m_WaferMapWrapper.GetGradeOffset(),
												"");		//v4.35T1	//Upgrade to v1.65.01 lib
		SaveGradeMapTableData(0, 0 +  m_WaferMapWrapper.GetGradeOffset());

		for (INT i = 0; i<aGradeInfo.GetSize(); i++)
		{
			stInfo = aGradeInfo.GetAt(i);
			ucGrade = m_oBinBlkMain.GrabGrade(i+1);
			
			// update mapped grade
			stInfo.ucGrade = ucGrade;
			stInfo.usOriginalGrade = stInfo.usOriginalGrade;
			stInfo.ulDieCount	= stInfo.ulDieCount;

			//m_WaferMapWrapper.SetGradeToGradeMap(stInfo.usOriginalGrade +  m_WaferMapWrapper.GetGradeOffset(), ucGrade + m_WaferMapWrapper.GetGradeOffset());
			m_WaferMapWrapper.SetGradeToGradeMap(ucGrade + m_WaferMapWrapper.GetGradeOffset(), 
													stInfo.usOriginalGrade +  m_WaferMapWrapper.GetGradeOffset(),
													"");		//v4.35T1	//Upgrade to v1.65.01 lib
			SaveGradeMapTableData(ucGrade, stInfo.usOriginalGrade +  m_WaferMapWrapper.GetGradeOffset());

			m_oBinBlkMain.SetGradeIsAssigned(ucGrade, TRUE, pBTfile);

			aNewGradeInfo.Add(stInfo);
			
		}
		
		CMSFileUtility::Instance()->SaveBTConfig();

		// For Genesis, sort the bin by number of total bin count
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( pApp->GetCustomerName() == "Genesis" )
		{
			//ArraySort
			int arraysize = (int)aNewGradeInfo.GetSize();

			CArray<BIN_FILE_INFO, BIN_FILE_INFO> aTempArray;	//Make a new array to save the sorted array
			aTempArray.SetSize(0,1);

			
			ULONG ulLocalLargestNo = -1;

			for ( int i = 0 ; i < arraysize ; i++ )
			{
				BIN_FILE_INFO TempInfo;
				TempInfo.ucGrade = i+1;
				TempInfo.ulDieCount = 0;
				for ( int j = 0 ; j < arraysize ; j++ )
				{
					BIN_FILE_INFO info = aNewGradeInfo.GetAt(j);
					ULONG ulTemp = info.ulDieCount;
//CString k;
//k.Format( "## %d %d %d %d", j, info.ucGrade, info.ulDieCount, info.usOriginalGrade );
//OutputDebugString(k);
					if ( ulTemp < ulLocalLargestNo || ulLocalLargestNo == -1)
					{
						if ( TempInfo.ulDieCount < ulTemp)
						{
							TempInfo.ulDieCount = ulTemp;
							TempInfo.usOriginalGrade = info.usOriginalGrade;
						}
					}
				}

				ulLocalLargestNo = TempInfo.ulDieCount;
				aTempArray.Add(TempInfo);

			}

			aGradeInfo.Copy(aTempArray);
		}
		else
		{
			aGradeInfo.Copy(aNewGradeInfo);
		}
	}

	return TRUE;
}

BOOL CBinTable::UpdateLotDieCount(CArray<BIN_FILE_INFO, BIN_FILE_INFO>& aGradeInfo)
{
	BIN_FILE_INFO stInfo;
	
	/*
	if (CMS896AStn::m_bUseOptBinCountDynAssignGrade == TRUE)
	{	
		for (INT i=1; i <= (INT) m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			for (INT j = 0; j<aGradeInfo.GetSize(); j++)
			{
				stInfo = aGradeInfo.GetAt(j);
				unsigned short	usOriginalGrade = m_WaferMapWrapper.GetOriginalGrade(m_oBinBlkMain.GrabGrade(i) + m_WaferMapWrapper.GetGradeOffset());

				if (((stInfo.usOriginalGrade + m_WaferMapWrapper.GetGradeOffset())  == usOriginalGrade) && (usOriginalGrade != 0))
				{
					m_oBinBlkMain.SetLotRemainCount(m_oBinBlkMain.GrabGrade(i) ,stInfo.ulDieCount);
					m_oBinBlkMain.SetGradeIsAssigned(m_oBinBlkMain.GrabGrade(i), TRUE);
				}
			}
		}
	}
	else
	*/

	for (INT i=1; i <= (INT) m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		for (INT j = 0; j<aGradeInfo.GetSize(); j++)
		{
			stInfo = aGradeInfo.GetAt(j);
			
			if (m_oBinBlkMain.GrabGrade(i) == stInfo.ucGrade)
			{
				m_oBinBlkMain.SetLotRemainCount(stInfo.ucGrade, stInfo.ulDieCount);
			}
		}
	}

	return TRUE;
}


BOOL CBinTable::SelectGradeToBond(CArray<BIN_FILE_INFO, BIN_FILE_INFO>& aGradeInfo)
{
	// Select grade bases on bin block assignment
	if (m_BinSummaryFile.GetSelectGradeBaseOnBinBlockGrade() == TRUE)
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		for (INT i=1; i <= (INT) m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			m_oBinBlkMain.SetGradeIsAssigned(m_oBinBlkMain.GrabGrade(i), TRUE, pBTfile);	//v4.52A8
		}
		CMSFileUtility::Instance()->SaveBTConfig();
	}
	else
	{
		// function is disabled if ==0
		if (m_ulMinLotCount == 0)
		{
			return TRUE;
		}

		m_oBinBlkMain.SetAllGradeIsAssigned(FALSE);

		BIN_FILE_INFO stInfo;
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

		for (INT i=1; i <= (INT) m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			for (INT j = 0; j<aGradeInfo.GetSize(); j++)
			{
				stInfo = aGradeInfo.GetAt(j);
				
				if (m_oBinBlkMain.GrabGrade(i) == stInfo.ucGrade)
				{	
					// set grade that will set as selected grade in wafer map wrapper
					m_oBinBlkMain.SetGradeIsAssigned(m_oBinBlkMain.GrabGrade(i), TRUE, pBTfile);
				}
			}
		}

		CMSFileUtility::Instance()->SaveBTConfig();
	}

	return TRUE;	
}


LONG CBinTable::OptimizeBinCount(CString szFilename, BOOL bEnableBinSumWaferLotCheck, ULONG ulMinLotCount)
{
	UCHAR ucGrade = '1';
	CString szTitle = "", szContent = "";
	ULONG ulTotalBinCount = 0;
	ULONG ulGradeInputCount = 0;
	CString szMsg;
	
	CArray<BIN_FILE_INFO, BIN_FILE_INFO> aGradeInfo;

	if (m_ulEnableOptimizeBinCount == FALSE)
	{
		szContent.LoadString(HMB_OPT_BIN_GRADE_OPTION_DIABLED);
		SetErrorMessage("Optmize bin count function not enabled");
		HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		return FALSE;
	}

	m_BinSummaryFile.SetEnableBinSumWaferLot(bEnableBinSumWaferLotCheck);
	m_BinSummaryFile.SetMinLotCount(ulMinLotCount);

	if (m_BinSummaryFile.ReadBinSummaryFile(szFilename, CMS896AStn::m_bUseOptBinCountDynAssignGrade) == FALSE )
	{
		szContent.LoadString(HMB_BT_LOAD_BIN_LOT_SUM_FILE_FAILED);
		szContent += "\n" + szFilename;
		SetErrorMessage("Fail to load bin lot summary file: " + szFilename);
		SetErrorMessage("Optimize Bin Error: " + m_BinSummaryFile.GetLastError());	//v4.44T4
		HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		return FALSE;
	}

	m_BinSummaryFile.GetGadeInfo(aGradeInfo);

//4.53D26 Comparing pkg name
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if(pApp->GetCustomerName() == "Electech3E(DL)")
	{
		BOOL bOpenMatchFuc = CMS896AStn::m_bOpenMatchPkgNameCheck;
		szMsg.Format("OptimzieBinCount -- Comparing function status: %d", bOpenMatchFuc);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		
		if(bOpenMatchFuc)
		{
		if	(CompareWithPkgName( m_BinSummaryFile.GetPkgName()) == FALSE )
		{
			return FALSE;		
		}
	}
	}

	if ((ULONG) aGradeInfo.GetSize() > m_oBinBlkMain.GetNoOfBlk())
	{
		CString szErr;
		szErr.Format("No of grades (%d) exceeds bin blk no (%d)", aGradeInfo.GetSize(), m_oBinBlkMain.GetNoOfBlk());
		SetErrorMessage(szErr);
		szContent.LoadString(HMB_BT_NO_OF_GRADES_EXCEED_BLK_NO);
		//HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		HmiMessage(szErr, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		return FALSE;
	}
	
	//v4.04		//Klocwork
	DynamicGradeAssign(aGradeInfo);
	UpdateLotDieCount(aGradeInfo);
	//SelectGradeToBond(aGradeInfo);

	for (INT i=1; i<= (INT) m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		ucGrade = m_oBinBlkMain.GrabGrade(i);
		ulGradeInputCount = m_oBinBlkMain.GetMaxFrameDieCount(ucGrade);
		m_oBinBlkMain.UpdateGradeInputCount(ucGrade, ulGradeInputCount);

		CString szLog;
		szLog.Format("OptimizeBinCount: Bin %d, Count = %lu", ucGrade, ulGradeInputCount);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		m_oBinBlkMain.OptimizeBinFrameCountByGrade(ucGrade, m_ucOptimizeBinCountMode, m_ulEnableOptimizeBinCount);
	}

	SelectGradeToBond(aGradeInfo);
	
	return TRUE;
}


BOOL CBinTable::ApplyOptimizeBinCount(CString szFullPath, BOOL bEnableBinSumWaferLotCheck, ULONG ulMinLotCount)
{
	INT nCol = 0;
	LONG lReturn = 0;
	
	ResetOptimizeBinCountStatus();

	if ((lReturn = OptimizeBinCount(szFullPath, bEnableBinSumWaferLotCheck, ulMinLotCount)) != TRUE)
	{
		SaveBinTableData();
		return FALSE;
	}

	nCol = szFullPath.ReverseFind('\\');
	if (nCol != -1)
	{
		m_szBinLotSummaryFilename = szFullPath.Right(szFullPath.GetLength() - nCol -1);	
	}

	WriteGradeLegend();
	LoadWaferStatistics();
	SaveBinTableData();

	if (m_oBinBlkMain.GrabSerialNoFormat() == BT_SNR_RESET_LOAD_BIN_SUMMARY)
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		for (INT i = 1; i <= (INT)m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE-1);
			m_oBinBlkMain.ResetSerialNo(i, pBTfile);	//v4.52A8
		}
		CMSFileUtility::Instance()->SaveBTConfig();

		SetStatusMessage("Reset bin serial no (Load bin summary file)");
	}

	if (m_oBinBlkMain.GrabBinClearFormat() == BT_CLEAR_BIN_COUNT_LOAD_BIN_SUMMARY)
	{
		m_oBinBlkMain.ResetBinClearedCount();
		SetStatusMessage("Reset bin cleared count (Load bin summary file)");
	}

	return TRUE;
}

BOOL CBinTable::ResetOptimizeBinCountStatus()
{
	// reset the filename and remaining counts to 0
	m_szBinLotSummaryFilename = "";
	
	for (INT i=1; i <= (INT) m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		m_oBinBlkMain.SetLotRemainCount(m_oBinBlkMain.GrabGrade(i), 0);
	}

	SetErrorMessage("Reset optimized bin counters, reset grade mapping");
	m_BinSummaryFile.ClearData();
	m_WaferMapWrapper.ResetGradeMap();
	//m_oBinBlkMain.SetAllGradeIsAssigned(TRUE);
	
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	for (INT i=1; i<=(INT)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		m_oBinBlkMain.SetGradeIsAssigned(m_oBinBlkMain.GrabGrade(i), TRUE, pBTfile);
	}

	CMSFileUtility::Instance()->SaveBTConfig();
	SaveBinTableData();
	return TRUE;
}


BOOL CBinTable::MoveWaferTableToSafePosn(CONST BOOL bSafe)
{
	if( !m_bUseLargeBinArea && !Is180Arm6InchWT() )
		return TRUE;

	IPC_CServiceMessage stMsg;
	BOOL bToSafePosn = bSafe;
	stMsg.InitMessage(sizeof(BOOL), &bToSafePosn);

	if( bToSafePosn )
        WftToSafeLog("BT 6 inch wft move to safe position");
	else
		WftToSafeLog("BT 6 inch wft move to original position");

	INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, _T("MoveWaferTableToSafePosn"), stMsg);

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

	BOOL bReturn = TRUE;
	stMsg.GetMsg(sizeof(BOOL), &bReturn);
	return bReturn;
}

BOOL CBinTable::UpdateBinBlkSetupParameters(ULONG ulBlkId)
{
	m_ulBinBlkToSetup = ulBlkId;
	m_ulNoOfBinBlk = m_oBinBlkMain.GetNoOfBlk();

	m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(ulBlkId));	
	m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(ulBlkId));
	m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(ulBlkId));
	m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(ulBlkId));

	m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchX());
	m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GetBlkPitchY());

	m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(ulBlkId));	//v4.59A19
	m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(ulBlkId));	//v4.59A19
	m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(ulBlkId);
	m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(ulBlkId);
	m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(ulBlkId);
	m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(ulBlkId);
	m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(ulBlkId);			//v4.59A22	//David Ma
	m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(ulBlkId);			//v4.59A22	//David Ma
	m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(ulBlkId);

	//m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkId);
	//m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(ulBlkId);
	m_ulDiePerUserRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkId);
	m_ulDiePerUserCol = m_oBinBlkMain.GrabNoOfDiePerCol(ulBlkId);
	m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(ulBlkId);

	m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(ulBlkId); 	 
	m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(ulBlkId);
	m_bEnableFirstRowColSkipPattern = m_oBinBlkMain.GetFirstRowColSkipPattern(ulBlkId);
	m_ulFirstRowColSkipUnit = m_oBinBlkMain.GetFirstRowColSkipPatternSkipUnit(ulBlkId);

	//v3.71T1	//CIrcular BOnd Area Support for PLLM REBEL
	m_bEnableCircularBondArea	= m_oBinBlkMain.GrabUseCircularArea(ulBlkId);
	m_lCirAreaCenterX			= m_oBinBlkMain.GrabCircularBinCenterX(ulBlkId);
	m_lCirAreaCenterY			= m_oBinBlkMain.GrabCircularBinCenterY(ulBlkId);
	DOUBLE dRadius = (DOUBLE) ConvertXEncoderValueForDisplay(m_oBinBlkMain.GrabCircularBinRadius(ulBlkId));		// from encoder step to um
	m_dCirAreaRadius			= dRadius / 1000.0;		//from um to mm 

	// must enable teach with physical block and centralized bond area
	m_bTeachWithPhysicalBlk = m_oBinBlkMain.GrabIsTeachWithPhysicalBlk(ulBlkId); 
	//m_bTeachWithPhysicalBlk = TRUE;

	m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(ulBlkId);
	//m_bCentralizedBondArea = TRUE;

	m_lCentralizedUpperLeftX = 0;
	m_lCentralizedUpperLeftY = 0;

	/*
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
	*/

	m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(ulBlkId);
	if (m_bEnableWafflePad)
	{
		m_lWafflePadDistX =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(ulBlkId));
		m_lWafflePadDistY =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(ulBlkId));
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
	m_bNoReturnTravel	= m_oBinBlkMain.GrabNoReturnTravel(ulBlkId);	//v4.42T1

	m_ucGrade = m_oBinBlkMain.GrabGrade(ulBlkId);
	m_ulInputCount	= m_oBinBlkMain.GrabInputCount(m_ucGrade);

	if (m_oBinBlkMain.GrabIsSetup(ulBlkId))
		m_ulSourcePhyBlk = m_oBinBlkMain.GrabPhyBlkId(ulBlkId);
	else
		m_ulSourcePhyBlk = 1;
	
	m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(ulBlkId));

	return TRUE;
}

BOOL CBinTable::SetupBinBlk(CString szSetupMode,  BOOL bIsMannualMode, BOOL bUpdateGradeData)
{ 
	ULONG ulTotalNoOfBinBlksCanHold;
	CString szTitle, szContent;
	LONG lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY;
	LONG lPhyBlk1ULX, lPhyBlk1ULY, lPhyBlk1LRX, lPhyBlk1LRY;
	LONG lBinBlkPitchX, lBinBlkPitchY;
	LONG lSourcePhyBlkULY, lSourcePhyBlkLRY;
	LONG lWafflePadDistX, lWafflePadDistY;
	LONG lIsValidUL, lIsValidLR;
	BOOL bIfIsFirstBlkInMerge, bIfEditBlkInMergeOK, bSetupRtnValue, bIsValidUL;
	BOOL bIsSetupComplete;
	LONG lResetBinSerial = 0;
	BOOL bResetBinSerial = FALSE;
	DOUBLE dDiePitchX = 0, dDiePitchY = 0;

	lBinBlkULX = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftX);
	lBinBlkULY = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftY);
	lBinBlkLRX = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightX);
	lBinBlkLRY = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightY);   

	lBinBlkPitchX = ConvertDisplayUnitToFileUnit(m_lBinBlkPitchX);
	lBinBlkPitchY = ConvertDisplayUnitToFileUnit(m_lBinBlkPitchY);

	dDiePitchX = ConvertDisplayUnitToDFileUnit(m_dDiePitchX);
	dDiePitchY = ConvertDisplayUnitToDFileUnit(m_dDiePitchY);

	lWafflePadDistX = ConvertDisplayUnitToFileUnit(m_lWafflePadDistX);
	lWafflePadDistY = ConvertDisplayUnitToFileUnit(m_lWafflePadDistY);

	m_bIfEnablePIBlkToSetup = TRUE;
	
	szTitle.LoadString(HMB_BT_SETUP_BIN_BLK);
	szContent.LoadString(HMB_BT_SETUP_BIN_BLK_OK);

	//Update WalkPath
	m_ulWalkPath = GetWalkPathWithHMIName(m_szWalkPath);

	//Protect MaxUnit cannot > Cal die per blk
	if ( m_ulMaxUnit > m_ulDiePerBlk )
	{
		m_ulMaxUnit = 0;
	}

	//Setup
	if (szSetupMode == "All Blocks") //All blocks setup common for both standard bin table and 
									 //bin loader
	{
		lPhyBlk1ULX = m_oPhyBlkMain.GrabBlkUpperLeftX(1);
		lPhyBlk1ULY = m_oPhyBlkMain.GrabBlkUpperLeftY(1);
		lPhyBlk1LRX = m_oPhyBlkMain.GrabBlkLowerRightX(1);
		lPhyBlk1LRY = m_oPhyBlkMain.GrabBlkLowerRightY(1);

		if (m_bTeachWithPhysicalBlk == TRUE)
		{
			lBinBlkULX = lPhyBlk1ULX;
			lBinBlkULY = lPhyBlk1ULY;
			lBinBlkLRX = lPhyBlk1LRX;
			lBinBlkLRY = lPhyBlk1LRY;  
		}

		bIsValidUL =  m_oBinBlkMain.ValidateAllSetupUL(lPhyBlk1ULX, lPhyBlk1ULY, 
						lPhyBlk1LRX, lPhyBlk1LRY, lBinBlkULX, lBinBlkULY);

		if (!bIsValidUL)
		{
			//Error: Upper Left is not on Physical Block 1! Please re-teach.
			SetAlert(IDS_BT_ULCHECK2);
			return FALSE;
		}
		else //valid UL
		{
			lIsValidLR = m_oBinBlkMain.ValidateAllSetupLR(lPhyBlk1ULX, lPhyBlk1ULY, 
				lPhyBlk1LRX, lPhyBlk1LRY, lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY);

			//check if LR is valid
			if (lIsValidLR == 1)
			{
				//Error: Lower Right is not on Physical Block 1! Please re-teach.
				SetAlert(IDS_BT_LRCHECK3);
				return FALSE;
			}
			else if (lIsValidLR == 2)
			{
				//Error: Lower Right is not at lower right corner! Please re-teach.
				SetAlert(IDS_BT_LRCHECK2);
				return FALSE;
			}
		}	

		if (bIsValidUL && lIsValidLR == 3) //both UL and LR are valid
		{
			ulTotalNoOfBinBlksCanHold = m_oPhyBlkMain.CalculateTotalNoOfBinBlksCanHold(lBinBlkULX,
				lBinBlkULY, lBinBlkLRX, lBinBlkLRY, lBinBlkPitchX, lBinBlkPitchY);

			if (m_ulNoOfBinBlk > ulTotalNoOfBinBlksCanHold)
			{
				//restore original structure values
				m_ulNoOfBinBlk = m_oBinBlkMain.GetNoOfBlk();

				m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftX(1));
				m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkUpperLeftY(1));
				m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightX(1));
				m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabBlkLowerRightY(1));
				
				m_lBinBlkPitchX = ConvertDisplayUnitToFileUnit(m_lBinBlkPitchX);
				m_lBinBlkPitchY = ConvertDisplayUnitToFileUnit(m_lBinBlkPitchY);

				m_ucGrade = m_oBinBlkMain.GrabGrade(1);
				
				m_dDiePitchX = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchX(1));	//v4.59A19
				m_dDiePitchY = ConvertDFileUnitToDisplayUnit(m_oBinBlkMain.GrabDDiePitchY(1));	//v4.59A19
				m_bUsePt5UmInDiePitchX	= m_oBinBlkMain.GrabUsePt5UmInDiePitchX(1);
				m_bUsePt5UmInDiePitchY	= m_oBinBlkMain.GrabUsePt5UmInDiePitchY(1);
				m_dBondAreaOffsetXInUm	= m_oBinBlkMain.GrabBondAreaOffsetX(1);
				m_dBondAreaOffsetYInUm	= m_oBinBlkMain.GrabBondAreaOffsetY(1);
				m_dThermalDeltaPitchXInUm	= m_oBinBlkMain.GrabThermalDeltaPitchX(1);	//v4.59A22	//David Ma
				m_dThermalDeltaPitchYInUm	= m_oBinBlkMain.GrabThermalDeltaPitchY(1);	//v4.59A22	//David Ma
				m_bByPassBinMap			= m_oBinBlkMain.GrabUseByPassBinMap(1);

				m_szWalkPath = GetWalkPathHMIName(m_oBinBlkMain.GrabOriginalWalkPath(1));

				m_ulDiePerRow = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabNoOfDiePerRow(1));
				m_ulDiePerCol = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabNoOfDiePerCol(1));
				m_ulDiePerBlk = ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabNoOfDiePerBlk(1));
				m_ulSkipUnit  = m_oBinBlkMain.GrabSkipUnit(1);	
				m_ulMaxUnit   = m_oBinBlkMain.GrabMaxUnit(1);


				if (ulTotalNoOfBinBlksCanHold == 0)
				{
					//Setup Error: Please re-teach Upper Left and Lower Right!
					SetAlert(IDS_BT_ULLRCHECK1);
					return FALSE;
				}
				else
				{
					//Setup Error: Bin table cannot hold the number of bin blocks with the specified dimensions!
					SetAlert(IDS_BT_NOTENOUGHROOM);
					return FALSE;
				}
			}
			else
			{
				m_oBinBlkMain.SetUserRowCol(0, m_ulDiePerUserRow, m_ulDiePerUserCol);

				//pllm
				LONG lRadiusInUm = (LONG) (m_dCirAreaRadius * 1000.0);
				LONG lRadius = labs(ConvertFileUnitToXEncoderValue(lRadiusInUm));		// from um to encoder step

				bIsSetupComplete = m_oBinBlkMain.SetupAllBlks(&m_oPhyBlkMain, m_ulNoOfBinBlk, 
									lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY, 
									lBinBlkPitchX, lBinBlkPitchY, dDiePitchX, dDiePitchY, 
									m_ulWalkPath, m_bAutoAssignGrade,m_bAssignSameGradeForAllBin, m_ucStartingGrade, m_ulSkipUnit,
									m_ulMaxUnit, m_bEnableFirstRowColSkipPattern, m_ulFirstRowColSkipUnit, 
									m_bCentralizedBondArea, m_bTeachWithPhysicalBlk, bUpdateGradeData, m_bEnableWafflePad,
									lWafflePadDistX, lWafflePadDistY, m_lWafflePadSizeX, m_lWafflePadSizeY,
									m_bEnableCircularBondArea, lRadius,	m_bUseBlockCornerAsFirstDiePos,
									m_bNoReturnTravel, m_bUsePt5UmInDiePitchX, m_bUsePt5UmInDiePitchY,
									m_dBondAreaOffsetXInUm, m_dBondAreaOffsetYInUm);

				if (! bIsSetupComplete)
				{
					SetAlert(m_oBinBlkMain.GetMsgCode());
					return FALSE;
				}
				else
				{
					//Update screen display
					m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(1);
					m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(1); 
					m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(1); 

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
						m_lWafflePadDistX =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(1));
						m_lWafflePadDistY =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(1));
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

					if (bIsMannualMode == TRUE)
					{
						WriteGradeLegend();
						LoadGradeRankID();
						LoadWaferStatistics();

						SetStatusMessage("Bin block setup is completed");
						HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
									glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					}
				}
			}
		}
	}
	
	if (szSetupMode == "Single Block")
	{
		switch (m_ulMachineType)
		{
			case BT_MACHTYPE_STD:	//Standard Bin Table
			
				if (m_oBinBlkMain.GrabNVNoOfSortedDie(m_ucGrade) > 0)
				{
					SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
					return FALSE;
				}

				
				if (m_bTeachWithPhysicalBlk == TRUE)
				{
					lBinBlkULX = m_oPhyBlkMain.GrabBlkUpperLeftX(m_ulBinBlkToSetup);
					lBinBlkULY = m_oPhyBlkMain.GrabBlkUpperLeftY(m_ulBinBlkToSetup);
					lBinBlkLRX = m_oPhyBlkMain.GrabBlkLowerRightX(m_ulBinBlkToSetup);
					lBinBlkLRY = m_oPhyBlkMain.GrabBlkLowerRightY(m_ulBinBlkToSetup);  
				}

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

				if ((! m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(m_ulBinBlkToSetup)) &&
					(! bIfIsFirstBlkInMerge))
				{
					//Upper Left & Lower Right validation
					lIsValidUL = m_oBinBlkMain.ValidateSingleSetupUL(&m_oPhyBlkMain, m_ulBinBlkToSetup,
									lBinBlkULX, lBinBlkULY);

					if (lIsValidUL > 0)
					{
						m_ulSourcePhyBlk = lIsValidUL;
						lIsValidLR = m_oBinBlkMain.ValidateSingleSetupLR(&m_oPhyBlkMain, m_ulBinBlkToSetup,
							m_ulSourcePhyBlk, lBinBlkULX, lBinBlkULY,lBinBlkLRX, lBinBlkLRY);

						if (lIsValidLR < 0) //lower right invalid
						{
							SetAlert(m_oBinBlkMain.GetMsgCode());
							return FALSE;
						}
					}
					else if (lIsValidUL == -1)
					{
						//Error: Upper Left is not on any physical block! Please re-teach.
						SetAlert(IDS_BT_ULCHECK3);
						return FALSE;
					}
					else if (lIsValidUL == -2)
					{
						//Error: Upper Left overlaps with existing bin block! Please re-teach.
						SetAlert(IDS_BT_ULCHECK1);
						return FALSE;
					}

					//setup 
					if ((lIsValidUL > 0) && (lIsValidLR > 0)) //setup only if both upper left and 
															//lower right are valid
					{
						lSourcePhyBlkULY = m_oPhyBlkMain.GrabBlkUpperLeftY(m_ulSourcePhyBlk);
						lSourcePhyBlkLRY = m_oPhyBlkMain.GrabBlkLowerRightY(m_ulSourcePhyBlk);

						m_oBinBlkMain.SetUserRowCol(m_ulBinBlkToSetup, m_ulDiePerUserRow, m_ulDiePerUserCol);

						bSetupRtnValue = m_oBinBlkMain.SetupSingleBlk(m_ulNoOfBinBlk, m_ulBinBlkToSetup, lBinBlkULX, lBinBlkULY,
							lBinBlkLRX, lBinBlkLRY, m_ucGrade, m_ulWalkPath, dDiePitchX, dDiePitchY,
							m_ulDiePerRow,m_ulSourcePhyBlk, lSourcePhyBlkULY, lSourcePhyBlkLRY, m_ulSkipUnit,
							m_ulMaxUnit, bResetBinSerial, m_bEnableFirstRowColSkipPattern, m_ulFirstRowColSkipUnit, 
							m_bCentralizedBondArea, m_bTeachWithPhysicalBlk, bUpdateGradeData, 
							m_bUseBlockCornerAsFirstDiePos, m_bEnableWafflePad,
							lWafflePadDistX, lWafflePadDistY, m_lWafflePadSizeX, m_lWafflePadSizeY,
							m_bNoReturnTravel, m_bUsePt5UmInDiePitchX, m_bUsePt5UmInDiePitchY,
							m_dBondAreaOffsetXInUm, m_dBondAreaOffsetYInUm);

						if (! bSetupRtnValue)
						{
							SetAlert(m_oBinBlkMain.GetMsgCode());
							return FALSE;
						}

						//Update screen display
						m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(m_ulBinBlkToSetup); 
						m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToSetup);
						m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(m_ulBinBlkToSetup);

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

						m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(m_ulBinBlkToSetup);
						if (m_bEnableWafflePad)
						{
							m_lWafflePadDistX =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(m_ulBinBlkToSetup));
							m_lWafflePadDistY =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(m_ulBinBlkToSetup));
							m_lWafflePadSizeX = m_oBinBlkMain.GrabWafflePadSizeX(m_ulBinBlkToSetup);
							m_lWafflePadSizeY = m_oBinBlkMain.GrabWafflePadSizeY(m_ulBinBlkToSetup);
						}
						else
						{
							m_lWafflePadDistX = 0;
							m_lWafflePadDistY = 0;
							m_lWafflePadSizeX = 0;
							m_lWafflePadSizeY = 0;
						}

						m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(m_ulBinBlkToSetup);

						if (bIsMannualMode == TRUE)
						{
							BinBlksDrawing();

							WriteGradeLegend();
							LoadGradeRankID();
							LoadWaferStatistics();

							SetStatusMessage("Bin block setup is completed");
							HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
										glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
							
						}
					}
				}
				else if (! m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(m_ulBinBlkToSetup) && 
						bIfIsFirstBlkInMerge)
				{
					//v4.59A19
					LONG lPitchX = (LONG) ConvertDisplayUnitToDFileUnit(m_dDiePitchX);
					LONG lPitchY = (LONG) ConvertDisplayUnitToDFileUnit(m_dDiePitchY);

					m_oBinBlkMain.Edit1stBlkInMerge(m_ulBinBlkToSetup, m_ucGrade, 
						lPitchX, lPitchY, m_ulWalkPath);

					if (bIsMannualMode == TRUE)
					{
						BinBlksDrawing();

						WriteGradeLegend();
						LoadGradeRankID();
						LoadWaferStatistics();

						SetStatusMessage("Bin block setup is completed");
						HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
									glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					}
				}
				else if (m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(m_ulBinBlkToSetup))
							//if block is currently disabled from same grade merge
				{
					//v4.59A19
					LONG lPitchX = (LONG) m_dDiePitchX;
					LONG lPitchY = (LONG) m_dDiePitchY;

					bIfEditBlkInMergeOK = m_oBinBlkMain.EditBlkInMerge(m_ulBinBlkToSetup, m_ucGrade, 
										lPitchX, lPitchY, m_ulWalkPath, m_ulSkipUnit, m_ulMaxUnit);

					if (! bIfEditBlkInMergeOK)
					{
						SetAlert(m_oBinBlkMain.GetMsgCode());
						return FALSE;
					}
					else
					{
						
						if (bIsMannualMode == TRUE)
						{
							BinBlksDrawing();

							WriteGradeLegend();
							LoadGradeRankID();
							LoadWaferStatistics();

							SetStatusMessage("Bin block setup is completed");
							HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
										glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

						}
					}
				}
			break;

			case BT_MACHTYPE_DTABLE:		//dual-buffer	//v4.16
			case BT_MACHTYPE_DBUFFER:		//dual-buffer	//v3.71T4
			case BT_MACHTYPE_DL_DLA:		//Bin Loader
				if (! m_oPhyBlkMain.GrabIsSetup(m_ulSourcePhyBlk))
				{
					SetAlert(IDS_BT_SOURCE_PHYBLK_NOT_SETUP);
					return FALSE;
				}

				//v4.42T8	//Citizen
				m_oBinBlkMain.SetUsePt5UmInDiePitchX(m_ulBinBlkToSetup, m_bUsePt5UmInDiePitchX);
				m_oBinBlkMain.SetUsePt5UmInDiePitchY(m_ulBinBlkToSetup, m_bUsePt5UmInDiePitchY);

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

				if ((! m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(m_ulBinBlkToSetup)) &&
					(! bIfIsFirstBlkInMerge))
				{
					if (m_bTeachWithPhysicalBlk == TRUE)
					{
						lBinBlkULX = m_oPhyBlkMain.GrabBlkUpperLeftX(m_ulBinBlkToSetup);
						lBinBlkULY = m_oPhyBlkMain.GrabBlkUpperLeftY(m_ulBinBlkToSetup);
						lBinBlkLRX = m_oPhyBlkMain.GrabBlkLowerRightX(m_ulBinBlkToSetup);
						lBinBlkLRY = m_oPhyBlkMain.GrabBlkLowerRightY(m_ulBinBlkToSetup);  
					}

					//Upper Left & Lower Right validation
					lIsValidUL = m_oBinBlkMain.ValidateSingleSetupUL_BL(&m_oPhyBlkMain, 
						m_ulBinBlkToSetup, lBinBlkULX, lBinBlkULY, m_ulSourcePhyBlk);

					if (lIsValidUL == -1)
					{
						SetAlert(IDS_BT_ULCHECK4);
						return FALSE;
					}
					else if (lIsValidUL == -2)
					{
						SetAlert(IDS_BT_ULCHECK5);
						return FALSE;
					}
					else if (lIsValidUL == 1)
					{
						lIsValidLR = m_oBinBlkMain.ValidateSingleSetupLR_BL(&m_oPhyBlkMain,
							m_ulBinBlkToSetup, lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY,
							m_ulSourcePhyBlk);

						if (lIsValidLR == -1)
						{	
							SetAlert(IDS_BT_LRCHECK5);
							return FALSE;
						}
						else if (lIsValidLR == -2)
						{
							SetAlert(IDS_BT_LRCHECK6);
							return FALSE;
						}
						else if (lIsValidLR == -3)
						{
							SetAlert(IDS_BT_URCHECK2);
							return FALSE;
						}
						else if (lIsValidLR == -4)
						{
							SetAlert(IDS_BT_LLCHECK1);
							return FALSE;
						}
						else if (lIsValidLR == -5)
						{
							SetAlert(IDS_BT_LEFTBOUNDARYCHK);
							return FALSE;
						}
						else if (lIsValidLR == -6)
						{
							SetAlert(IDS_BT_RIGHTBOUNDARYCHK);
							return FALSE;
						}
						else if (lIsValidLR == -7)
						{
							SetAlert(IDS_BT_UPPERBOUNDARYCHK);
							return FALSE;
						}
						else if (lIsValidLR == -8)
						{
							SetAlert(IDS_BT_LOWERBOUNDARYCHK);
							return FALSE;
						}
						else if ((lIsValidLR == -9) || (lIsValidLR == -10))
						{
							SetAlert(IDS_BT_OVERLAPPINGCHK1);
							return FALSE;
						}
						else if (lIsValidLR == -11)
						{
							SetAlert(IDS_BT_LRCHECK2);
							return FALSE;
						}
						else if (lIsValidLR == 1)
						{
							lSourcePhyBlkULY = m_oPhyBlkMain.GrabBlkUpperLeftY(m_ulSourcePhyBlk);
							lSourcePhyBlkLRY = m_oPhyBlkMain.GrabBlkLowerRightY(m_ulSourcePhyBlk);

							m_oBinBlkMain.SetUserRowCol(m_ulBinBlkToSetup, m_ulDiePerUserRow, m_ulDiePerUserCol);

							bSetupRtnValue = m_oBinBlkMain.SetupSingleBlk(m_ulNoOfBinBlk, 
								m_ulBinBlkToSetup, lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY, 
								m_ucGrade, m_ulWalkPath, dDiePitchX, dDiePitchY, m_ulDiePerRow,
								m_ulSourcePhyBlk, lSourcePhyBlkULY, lSourcePhyBlkLRY, m_ulSkipUnit,
								m_ulMaxUnit, bResetBinSerial, m_bEnableFirstRowColSkipPattern, 
								m_ulFirstRowColSkipUnit, m_bCentralizedBondArea, m_bTeachWithPhysicalBlk,
								bUpdateGradeData, m_bUseBlockCornerAsFirstDiePos, m_bEnableWafflePad,
								lWafflePadDistX, lWafflePadDistY, m_lWafflePadSizeX, m_lWafflePadSizeY,
								m_bNoReturnTravel, m_bUsePt5UmInDiePitchX, m_bUsePt5UmInDiePitchY, 
								m_dBondAreaOffsetXInUm, m_dBondAreaOffsetYInUm);

							if (bSetupRtnValue == FALSE)
							{
								SetAlert(m_oBinBlkMain.GetMsgCode());
								return FALSE;
							}
							else
							{

								//Update screen display
								m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(m_ulBinBlkToSetup); 
								m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToSetup);
								m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(m_ulBinBlkToSetup);
					
								m_bCentralizedBondArea = m_oBinBlkMain.GrabIsCentralizedBondArea(m_ulBinBlkToSetup);
								if (m_bCentralizedBondArea)
								{
									m_lCentralizedOffsetX = m_oBinBlkMain.GrabCentralizedOffsetX(m_ulBinBlkToSetup);
									m_lCentralizedOffsetY = m_oBinBlkMain.GrabCentralizedOffsetY(m_ulBinBlkToSetup);
									m_lCentralizedUpperLeftX = ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftX(m_ulBinBlkToSetup) + m_lCentralizedOffsetX);
									m_lCentralizedUpperLeftY = ConvertDisplayUnitToFileUnit(m_oBinBlkMain.GrabBlkUpperLeftY(m_ulBinBlkToSetup) + m_lCentralizedOffsetY);
								}
								else
								{
									m_lCentralizedUpperLeftX = 0;
									m_lCentralizedUpperLeftY = 0;
								}

								m_bEnableWafflePad = m_oBinBlkMain.GrabIsEnableWafflePad(m_ulBinBlkToSetup);
								if (m_bEnableWafflePad)
								{
									m_lWafflePadDistX =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistX(m_ulBinBlkToSetup));
									m_lWafflePadDistY =	ConvertFileUnitToDisplayUnit(m_oBinBlkMain.GrabWafflePadDistY(m_ulBinBlkToSetup));
									m_lWafflePadSizeX = m_oBinBlkMain.GrabWafflePadSizeX(m_ulBinBlkToSetup);
									m_lWafflePadSizeY = m_oBinBlkMain.GrabWafflePadSizeY(m_ulBinBlkToSetup);
								}
								else
								{
									m_lWafflePadDistX = 0;
									m_lWafflePadDistY = 0;
									m_lWafflePadSizeX = 0;
									m_lWafflePadSizeY = 0;
								}

								m_bUseBlockCornerAsFirstDiePos = m_oBinBlkMain.GrabIsUseBlockCornerAsFirstDiePos(m_ulBinBlkToSetup);

								if (bIsMannualMode == TRUE)
								{
									BinBlksDrawing_BL(m_ulBinBlkToSetup);

									WriteGradeLegend();
									LoadGradeRankID();
									LoadWaferStatistics();

									SetStatusMessage("Bin block setup is completed");
									HmiMessage(szContent, szTitle, glHMI_MBX_OK, 
												glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
								}
							}
						}
					}
				}
		}
	}

	GetWafflePadSettings();

	return TRUE;
}

//v4.42T11	//Nichia
//Nichia//v4.43T7
BOOL CBinTable::SetupBinBlkForArrCode(ULONG ulBinBlkID, UCHAR ucType,
								DOUBLE dAreaXInUm, DOUBLE dAreaYInUm,
								UCHAR ucStartPos, ULONG ulMaxLoad, LONG lBinPitchX, LONG lBinPitchY,
								BOOL bReturnTravel)
{
	BOOL bStatus = TRUE;
	CString szLog;

	if (ucType == 1)	//Circular shape
	{
		LONG lULX = m_oPhyBlkMain.GrabBlkUpperLeftX(ulBinBlkID);
		LONG lULY = m_oPhyBlkMain.GrabBlkUpperLeftY(ulBinBlkID);
		LONG lLRX = m_oPhyBlkMain.GrabBlkLowerRightX(ulBinBlkID);
		LONG lLRY = m_oPhyBlkMain.GrabBlkLowerRightY(ulBinBlkID);  

		bStatus = m_oBinBlkMain.SetupCircularBlkByArrCode(ulBinBlkID, lULX, lULY, lLRX, lLRY,
													dAreaXInUm, dAreaYInUm, 
													ucStartPos, ulMaxLoad,  
													lBinPitchX, lBinPitchY, bReturnTravel);
		
		szLog.Format("ARRCODE applied on Circular Bin #%lu - Status=%d, Type=%d, dX=%.1f, dY=%.1f, Start=%d, Max=%ld, PitchX=%ld, PitchY=%ld, Return=%d",
					ulBinBlkID, bStatus, ucType, dAreaXInUm, dAreaYInUm, ucStartPos, ulMaxLoad, lBinPitchX, lBinPitchY, bReturnTravel);
	}
	else
	{
		LONG lULX = 0 - m_oBinBlkMain.GrabBlkUpperLeftX(ulBinBlkID);
		LONG lULY = 0 - m_oBinBlkMain.GrabBlkUpperLeftY(ulBinBlkID);
		LONG lLRX = 0 - (lULX - (LONG) dAreaXInUm);
		LONG lLRY = 0 - (lULY - (LONG) dAreaYInUm);

		if ( (lLRX < m_oPhyBlkMain.GrabBlkUpperLeftX(1))  ||
			(lLRX > m_oPhyBlkMain.GrabBlkLowerRightX(1)) ||
			(lLRY < m_oPhyBlkMain.GrabBlkUpperLeftY(1))  ||
			(lLRY > m_oPhyBlkMain.GrabBlkLowerRightY(1)) )
		{
			szLog.Format("ERROR: SetupBinBlkForArrCode fail - %ld %ld %ld %ld %ld %ld", lLRX, lLRY,
							m_oPhyBlkMain.GrabBlkUpperLeftX(1), m_oPhyBlkMain.GrabBlkLowerRightX(1),
							m_oPhyBlkMain.GrabBlkUpperLeftY(1), m_oPhyBlkMain.GrabBlkLowerRightY(1));
			CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
			SetErrorMessage(szLog);
			return FALSE;
		}

		//a54321234
		LONG lPhyULX = m_oPhyBlkMain.GrabBlkUpperLeftX(ulBinBlkID);
		LONG lPhyULY = m_oPhyBlkMain.GrabBlkUpperLeftY(ulBinBlkID);
		LONG lPhyLRX = m_oPhyBlkMain.GrabBlkLowerRightX(ulBinBlkID);
		LONG lPhyLRY = m_oPhyBlkMain.GrabBlkLowerRightY(ulBinBlkID);  

		bStatus = m_oBinBlkMain.SetupSingleBlkByArrCode(ulBinBlkID, 
													lPhyULX, lPhyULY, lPhyLRX, lPhyLRY,
													ucType,
													dAreaXInUm, dAreaYInUm, 
													ucStartPos, ulMaxLoad,  
													lBinPitchX, lBinPitchY, bReturnTravel);
		
		szLog.Format("ARRCODE applied on Bin #%lu - Status=%d, Type=%d, dX=%.1f, dY=%.1f, Start=%d, Max=%ld, PitchX=%ld, PitchY=%ld, Return=%d",
					ulBinBlkID, bStatus, ucType, dAreaXInUm, dAreaYInUm, ucStartPos, ulMaxLoad, lBinPitchX, lBinPitchY, bReturnTravel);
	}

	CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
	return bStatus;
}

BOOL CBinTable::IM_WaferEndChecking()
{
	BOOL bResult = TRUE;
	return bResult;
}

BOOL CBinTable::ResetMapDieType()
{
	IPC_CServiceMessage rReqMsg;
	// reset die type settings
	INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ResetDieType", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	
	return TRUE;
}

BOOL CBinTable::IsNeedCheckBatchIDFile()
{
	if (CMS896AStn::m_bEnableBatchIdFileCheck == FALSE)
	{
		return FALSE;
	}
	
	for (INT i=0; i<BT_EXTRA_BIN_INFO_ITEM_NO; i++)
	{
		if (m_szaExtraBinInfo[i].IsEmpty() == FALSE)
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CBinTable::ClearBatchIDInformation()
{
	if (CMS896AStn::m_bEnableBatchIdFileCheck == FALSE)
	{
		return TRUE;
	}

	for (INT i=0 ; i<BT_EXTRA_BIN_INFO_ITEM_NO ; i++)
	{
		m_szaExtraBinInfo[i] = "";
	}

	return TRUE;
}

BOOL CBinTable::CheckBatchIdFile(CString szBarcodeNumber)
{
	if (CMS896AStn::m_bEnableBatchIdFileCheck == FALSE)
	{
		return TRUE;
	}

	CString szExtraInfoFilename;
	CString szBatchNo;
	CString szBatchID;
	CString szProductName;

	if (GetBatchIdInfoFilename(szExtraInfoFilename) == FALSE)
	{
		SetAlert_Red_Yellow(IDS_BT_BIN_NO_BATCH_ID);
		return FALSE;
	}

	if (ReadBatchIdInfoFile(szExtraInfoFilename, szBarcodeNumber, szBatchNo, szBatchID, szProductName) == FALSE)
	{
		SetAlert_Red_Yellow(IDS_BT_BIN_NO_BATCH_ID);

		return FALSE;
	}

	for (INT i=0; i<BT_EXTRA_BIN_INFO_ITEM_NO; i++)
	{
		m_szaExtraBinInfo[i] = " ";
	}

	m_szaExtraBinInfo[0] = szBatchID;
	m_szaExtraBinInfo[1] = szBatchNo;
	m_szaExtraBinInfo[2] = szProductName;

	SaveBinTableData();
	return TRUE;
}


BOOL CBinTable::GetBatchIdInfoFilename(CString& szExtraInfoFilename)
{
	CStdioFile cfFile;
	INT nCol = -1;
	CString szStr = "";
	CString szExtraInfoPath = "";
	CString szFilename = BT_BATCHID_INFO_FILE;
	CString szFormatFilename = "";
	CString szOutputFileFormat = "";

	if (cfFile.Open(szFilename, CFile::modeRead) == FALSE)
	{
		//DebugLog("Cannot open GetExtraInfoFilename");
		return FALSE;
	}
	
	cfFile.ReadString(szStr);
	nCol = szStr.Find("[Path]");

	if (nCol == -1)
	{
		//DebugLog("Cannot extract the path");
		cfFile.Close();
		return FALSE;
	}

	nCol = szStr.Find("=");

	if (nCol == -1)
	{
		//DebugLog("Cannot find =");
		cfFile.Close();
		return FALSE;
	}
	
	szExtraInfoPath = szStr.Right(szStr.GetLength() - nCol -1);

	if (szExtraInfoPath.IsEmpty() == TRUE)
	{
		//DebugLog("GetExtraInfoFilename path is empty");
		cfFile.Close();
		return FALSE;
	}
	
	szOutputFileFormat = m_szBinOutputFileFormat;
	
	if (szOutputFileFormat == "uSiliconCMLT")
	{
		szFormatFilename = "New_001.csv";
	}
	else if (szOutputFileFormat == "uSiliconForEpi")
	{
		szFormatFilename = "New_003.csv";
	}
	else if (szOutputFileFormat == "uSiliconAOC")
	{
		szFormatFilename = "New_005.csv";
	}
	else if (szOutputFileFormat == "uSiliconHPO")
	{
		szFormatFilename = "New_008.csv";
	}
	else if (szOutputFileFormat == "uSiliconCLO")
	{
		szFormatFilename = "New_011.csv";
	}
	else if (szOutputFileFormat == "uSiliconGPI")
	{
		szFormatFilename = "New_013.csv";
	}
	else if (szOutputFileFormat == "uSiliconWalsin")
	{
		szFormatFilename = "New_014.csv";
	}
	else if (szOutputFileFormat == "uSiliconAlder")
	{
		szFormatFilename = "New_015.csv";
	}
	
	szExtraInfoFilename = szExtraInfoPath + "\\" + szFormatFilename;

	cfFile.Close();
	return TRUE;
}

BOOL CBinTable::ReadBatchIdInfoFile(CString szFilename, CString szBarcodeNumber, CString& szBatchNo, CString& szBatchID, 
									CString& szProductName)
{
	CStdioFile cfFile;
	INT nCol = -1;
	CString szStr;
	CStringArray szaWaferIds;
	CString szBatchNoTemp = "", szBatchIDTemp = "", szBarcodeNoTemp = "", szProductNameTemp = "", szStationTemp = "";

	if (cfFile.Open(szFilename, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}
	
	nCol = szBarcodeNumber.Find(".");
	if (nCol != -1)
	{
		szBarcodeNumber = szBarcodeNumber.Left(nCol);
	}

	while(cfFile.ReadString(szStr))
	{
		szBatchNoTemp = "";
		szBatchIDTemp = "";
		szBarcodeNoTemp = "";
		szProductNameTemp = "";
		szStationTemp = "";
		nCol = -1;

		DecodeBatchIdInfoStr(szStr, szBatchNoTemp, szBatchIDTemp, szBarcodeNoTemp, 
			szProductNameTemp, szStationTemp);

		nCol = szStationTemp.Find("Sorter");

		if (szBarcodeNumber == szBarcodeNoTemp && nCol != -1)
		{
			szBatchNo = szBatchNoTemp;
			szBatchID = szBatchIDTemp;
			szProductName = szProductNameTemp;
			cfFile.Close();
			
			if (szBatchNo.IsEmpty() || szBatchID.IsEmpty() || szProductName.IsEmpty())
			{
				return FALSE;
			}

			return TRUE;
		}
	}

	//DebugLog("cannot found the match barcode");

	cfFile.Close();
	return FALSE;
}

VOID CBinTable::DecodeBatchIdInfoStr(CString szStr, CString& szBatchNo, CString& szBatchID, CString& szBarcodeNo, 
		CString& szProductName, CString& szStation)
{
	CString szFirstPart, szSecPart;

	ParseString(szStr, szFirstPart, szSecPart, ",");
	ParseString(szSecPart, szBatchNo, szSecPart, ",");
	ParseString(szSecPart, szFirstPart, szSecPart, ",");
	ParseString(szSecPart, szBatchID, szSecPart, ",");
	ParseString(szSecPart, szFirstPart, szSecPart, ",");
	ParseString(szSecPart, szBarcodeNo, szSecPart, ",");
	ParseString(szSecPart, szProductName, szSecPart, ",");
	ParseString(szSecPart, szFirstPart, szSecPart, ",");
	ParseString(szSecPart, szStation, szSecPart, ",");
	//ParseString(szSecPart, szStation, szSecPart, ",");
}

VOID CBinTable::ParseString(CString szStr, CString& szFirstPart, CString& szSecPart, CString szToken)
{
	szFirstPart.Empty();
	szSecPart.Empty();

	INT nCol = -1;
	nCol = szStr.Find(szToken);

	if (nCol == -1)
	{
		return;
	}
	
	szFirstPart = szStr.Left(nCol);
	szSecPart = szStr.Right(szStr.GetLength() - nCol -1);
}

LONG CBinTable::LogItems(LONG lEventNo)
{
	if (m_bEnableItemLog == FALSE)
	{
		return 1;
	}

	CString szMsg;
	
	GetLogItemsString(lEventNo, szMsg);
	SetLogItems(szMsg);

	return 1;
}

LONG CBinTable::GetLogItemsString(LONG lEventNo, CString& szMsg)
{
	CString szBinBlkId;
	CString szTemp;

	if (m_szSetupMode == "Single Block")
	{
		szBinBlkId.Format("(Bin Blk %d)", m_ulBinBlkToSetup);
	}
	else
	{
		szBinBlkId = "(All Bin Blocks)";
	}
	
	switch(lEventNo)
	{
	case NO_OF_BIN_BLOCKS:
		szTemp.Format("%d", m_oBinBlkMain.GetNoOfBlk());
		szMsg = "No of Bin Blocks," + szTemp;
		break;
	
	case MAX_DIE_QUANTITIES:
		
		if (m_szSetupMode == "Single Block")
		{
			szBinBlkId.Format("(Bin Blk %d)", m_ulBinBlkToSetup);
			szTemp.Format("%d", m_oBinBlkMain.GrabNoOfDiePerBlk(m_ulBinBlkToSetup));
			szMsg = "Max Die Quantities " + szBinBlkId + "," + szTemp;
		}
		else
		{
			szBinBlkId = "(All Bin Blks)";
			szTemp.Format("%d", m_oBinBlkMain.GrabNoOfDiePerBlk(1));
			szMsg = "Max Die Quantities " + szBinBlkId + "," + szTemp;
		}

		break;

	case BIN_BLOCKS_DIE_QUANTITIES:

		if (m_szSetupMode == "Single Block")
		{
			szBinBlkId.Format("(Bin Blk %d)", m_ulBinBlkToSetup);
			szTemp.Format("%d", m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(m_ulBinBlkToSetup)));
			szMsg = "Bin Block Die Quantities " + szBinBlkId + "," + szTemp;
		}
		else
		{
			szBinBlkId = "(All Bin Blks)";
			szTemp.Format("%d", m_oBinBlkMain.GrabInputCount(1));
			szMsg = "Bin Block Die Quantities " + szBinBlkId + "," + szTemp;
		}

		break;

	case BIN_BLOCKS_WALK_PATH:
		
		if (m_szSetupMode == "Single Block")
		{
			szBinBlkId.Format("(Bin Blk %d)", m_ulBinBlkToSetup);
			
			if (m_oBinBlkMain.GrabWalkPath(m_ulBinBlkToSetup) == 0)
			{
				szTemp = "TL-Horz";
			}
			else if (m_oBinBlkMain.GrabWalkPath(m_ulBinBlkToSetup) == 1)
			{
				szTemp = "TL-Vert";
			}
			else if (m_oBinBlkMain.GrabWalkPath(m_ulBinBlkToSetup) == 2)
			{
				szTemp = "TR-Horz";
			}
			else
			{
				szTemp = "TR-Vert";
			}

			szMsg = "Bin Block Walk Path " + szBinBlkId + "," + szTemp;
		}
		else
		{
			szBinBlkId = "(All Bin Blks)";
			
			if (m_oBinBlkMain.GrabWalkPath(1) == 0)
			{
				szTemp = "TL-Horz";
			}
			else if (m_oBinBlkMain.GrabWalkPath(1) == 1)
			{
				szTemp = "TL-Vert";
			}
			else if (m_oBinBlkMain.GrabWalkPath(1) == 2)
			{
				szTemp = "TR-Horz";
			}
			else
			{
				szTemp = "TR-Vert";
			}

			szMsg = "Bin Block Walk Path " + szBinBlkId + "," + szTemp;
		}

		break;

	case FARME_REALIGN_LAST_DIE_OFFSET_X:
		szMsg.Format("Frame Realign Last Die Offset X,%.2f", m_dAlignLastDieOffsetX);
		break;

	case FARME_REALIGN_LAST_DIE_OFFSET_Y:
		szMsg.Format("Frame Realign Last Die Offset Y,%.2f", m_dAlignLastDieOffsetY);
		break;

	case FRAME_REALIGN_METHOD:
		if (m_lRealignBinFrameOption == 0)
		{
			szMsg = "Frame Realign Method,Default";
		}
		else
		{
			szMsg = "Frame Realign Method,1-Pt";
		}
		break;
	
	case BT_CHANGE_GRADE_LIMIT:
		szMsg.Format("Buffer Min Die Count for Preload,%d", m_ulGradeChangeLimit);
		break;
	}

	return 1;
}


BOOL CBinTable::AddPLLMFlatFileEntry(CONST ULONG ulBlkId)
{
	//v4.02T3	//v4.14T1	//Put this checking at top of fcn
	if (m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkId) <= 0)
	{
		//No need to add entry if bin count is zero!!
		return FALSE;
	}

	//CString szProductPart		= (*m_psmfSRam)["MS896A"]["Operator Id"];
	CString szProductPart		= (*m_psmfSRam)["MS896A"]["Product Id"];	//v4.42T4
	szProductPart				= szProductPart + _T(",");
	CString szSmartLotNumber1	= (*m_psmfSRam)["MS896A"]["LotNumber"];
	CString szSmartLotNumber	= szSmartLotNumber1 + _T(",");
	
	CString szOutputID			= ",";
	//if ( (m_ulMachineType == 2) || (m_ulMachineType == 3) )		//if DL/DLA (2) or DBuffer (3)
	if ( m_ulMachineType >= BT_MACHTYPE_DL_DLA )	//v4.16T1
	{
		//v4.51A17
		//szOutputID = "B" + m_oBinBlkMain.GetBinBlkBarcode(ulBlkId) + ",";
		szOutputID = "B" + GetBLBarcodeData(ulBlkId) + ",";
	}


	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *pBinSerial = NULL;
	LONG lSerialNo = 0;
	if (pUtl->LoadMultipleBinSerialFile())
	{
		// get file pointer
		pBinSerial = pUtl->GetMultipleBinSerialFile();
		if ( pBinSerial )
		{
			lSerialNo = (*pBinSerial)["Bin Serial"][0]["SerialNo"];
			lSerialNo = lSerialNo + 1;
		}
		pUtl->CloseMultipleBinSerialFile();
	}

	CTime theTime			= CTime::GetCurrentTime();
	CString szDateGenerated	= theTime.Format("%y%m%d");
	CString szMachineNumber	= GetMachineNo();
	szMachineNumber			= szMachineNumber;
	CString szSerialNo;
	szSerialNo.Format("%d", lSerialNo);
	CString szBinTapeID		= _T("DS1_") + szMachineNumber + _T("_") + szDateGenerated + _T("-") + szSerialNo + _T(",");
	
	unsigned short usOrgGrade = 0;
	if (m_WaferMapWrapper.GetReader() != NULL)	//If MAP is cleared		//v4.14T1
	{
		usOrgGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade(m_oBinBlkMain.GrabGrade(ulBlkId) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset())
								- CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
	}

	CString szBin1			= _T("B");
	szBin1.Format("B%d", usOrgGrade);
	CString szBin = szBin1 + _T(",");

	CString szQty			= _T("");
	szQty.Format("%d,", m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkId));

	CString szMarketGrade	= _T(",");	//EMPTY
	CString szBinLot		= szSmartLotNumber1 + _T("-") + szBin1 + _T(",");		//v3.96T2

	CString szEngBin		= _T(",");	//EMPTY
	CString szVF1			= _T(",");	//EMPTY
	CString szVF2			= _T(",");	//EMPTY
	CString szLOP1			= _T(",");	//EMPTY
	CString szLOP2			= _T(",");	//EMPTY
	CString szLambda1		= _T(",");	//EMPTY
	CString szLambda2		= _T(",");	//EMPTY		//v3.96T1

	CString szLine = szProductPart + szSmartLotNumber + szOutputID + szBinTapeID + szBin + szQty + szMarketGrade + szBinLot +
						szEngBin + szVF1 + szVF2 + szLOP1 + szLOP2 + szLambda1 + szLambda2;

	CStdioFile oFile;
	CString szTmpFile = gszUSER_DIRECTORY + _T("\\OutputFile\\FlatFile.txt");

	if (oFile.Open(szTmpFile, 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) != TRUE)
	{
		SetErrorMessage("BT: fail to add entry into PLLM Flat file! - " + szLine);
		return FALSE;
	}

	oFile.SeekToEnd();
	oFile.WriteString(szLine + "\n");
	oFile.Close();
	CMSLogFileUtility::Instance()->MS_LogOperation("FLAT File record: " + szLine);	//v4.44A4
	return TRUE;
}


BOOL CBinTable::UploadPLLMFlatFileToServer(CONST CString szServerPath, CONST CString szBackupPath)
{
	CString szInterfaceDoc		= _T("I201");
	CString szSmartLotNumber	= (*m_psmfSRam)["MS896A"]["LotNumber"];
	CString szWorkOrder			= (*m_psmfSRam)["MS896A"]["Bin WorkNo"];
	CString szMachineNumber		= GetMachineNo();
	
	CTime theTime				= CTime::GetCurrentTime();
	CString szTimeGenerated		= theTime.Format("%M:%S");		//theTime.Format("%H:%M:%S");
	CString szDateGenerated		= theTime.Format("%m-%d-%y");

	CString szLine1		= szInterfaceDoc;
	CString szLine2		= szSmartLotNumber;
	CString szLine3		= szWorkOrder;
	CString szLine4		= szMachineNumber;
	CString szLine5		= szTimeGenerated;
	CString szLine6		= szDateGenerated;
	CString szLine7		= _T("\n");


	CStdioFile oTmpFile, oFlatFile;
	CString szFlatFile			= gszUSER_DIRECTORY + _T("\\OutputFile\\") + szWorkOrder;
	CString szLocalTmpFile		= gszUSER_DIRECTORY + _T("\\OutputFile\\FlatFile.txt");
	CString szServerFlatFile	= szServerPath + _T("\\") + szWorkOrder + ".txt";		//v4.14T8
	CString szBackupFlatFile	= szBackupPath + _T("\\") + szWorkOrder + ".txt";		//v4.14T8

	CMSLogFileUtility::Instance()->MS_LogOperation("Upload PLLM FLAT File: " + szServerFlatFile);

	if (oFlatFile.Open(szFlatFile, 
		CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) != TRUE)
	{
		SetErrorMessage("BT: fail to open local FLAT file for writing");
		return FALSE;
	}

	if (oTmpFile.Open(szLocalTmpFile, 
		CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText) != TRUE)
	{
		SetErrorMessage("BT: fail to open local FLAT file for reading");
		oFlatFile.Close();
		return FALSE;
	}
	oTmpFile.SeekToBegin();

	//Create FLAT file header
	oFlatFile.WriteString(szLine1 + "\n");
	oFlatFile.WriteString(szLine2 + "\n");
	oFlatFile.WriteString(szLine3 + "\n");
	oFlatFile.WriteString(szLine4 + "\n");
	oFlatFile.WriteString(szLine5 + "\n");
	oFlatFile.WriteString(szLine6 + "\n");
	oFlatFile.WriteString(szLine7);

	//Copy FLAT file content to local copy before copying to server
	CString szLine;
	while (oTmpFile.ReadString(szLine))
	{
		oFlatFile.WriteString(szLine + "\n");
	}

	oFlatFile.Close();
	oTmpFile.Close();

	TRY {
		CopyFile(szFlatFile, szBackupFlatFile, FALSE);
	}
	CATCH (CFileException, e)
	{
		SetErrorMessage("BT: fail to upload FLAT file to backup path.");
		//return FALSE;
	}
	END_CATCH

	TRY {
		CopyFile(szFlatFile, szServerFlatFile, FALSE);
	}
	CATCH (CFileException, e)
	{
		SetErrorMessage("BT: fail to upload FLAT file to server.");
		return FALSE;
	}
	END_CATCH

	TRY {
		DeleteFile(szFlatFile);
	}
	CATCH (CFileException, e)
	{
		SetErrorMessage("BT: fail to delete local FLAT file");
	}
	END_CATCH

	TRY {
		DeleteFile(szLocalTmpFile);
	}
	CATCH (CFileException, e)
	{
		SetErrorMessage("BT: fail to delete local FLAT entry file");
	}
	END_CATCH

	return TRUE;
}

//v4.50A10	//Cree HuiZhou GradeMapping fcn 
BOOL CBinTable::ResetCreeGradeMappingEntry(CONST ULONG ulBlkID, BOOL bPrompt, BOOL bResetSlotStatus)
{
	BOOL bStatus = TRUE;
	if (IsMapDetectSkipMode())
	{
		CString szLog;
		USHORT usOrigGrade = m_WaferMapWrapper.GetOriginalGrade((UCHAR)(ulBlkID + m_WaferMapWrapper.GetGradeOffset()));
		bStatus = m_WaferMapWrapper.RecycleGradeMap((UCHAR)(ulBlkID + m_WaferMapWrapper.GetGradeOffset()));
		if (bStatus)
		{
			szLog.Format("BT ClearBin (CREE): BLK #%d (OrigGrade = %d) grade-mapping is reset", 
							ulBlkID, usOrigGrade);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
		else
		{
			szLog.Format("Fail to reset Bin #%d (%d) grade-mapping", ulBlkID, usOrigGrade);
			SetErrorMessage(szLog);
			HmiMessage_Red_Yellow(szLog, "Clear Bin Counter");
		}

		//v4.50A12	//Auto-reset grade slot to ACT2FULL when ClearBin
		if (bResetSlotStatus)	//v4.51A22
		{
			IPC_CServiceMessage stMsg;
			int nConvID = 0;
			BOOL bResult = TRUE;
			ULONG ulBlk = ulBlkID;
			stMsg.InitMessage(sizeof(ULONG), &ulBlk);
			
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "ClearBinResetGradeSlot" , stMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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
		}
	}

	return bStatus;
}


BOOL CBinTable::MotionTest(VOID)
{
	if (m_bIsMotionTestBinTableX == FALSE && m_bIsMotionTestBinTableY == FALSE)
	{
		return TRUE;
	}

	LONG lDiff_X, lDiff_Y;
	INT nProf_X, nProf_Y;
	BOOL bLogAxis = FALSE;
	CString szTemp;
	INT nTestDistance = (INT)m_lMotionTestDist;

	/*
	if ((m_bMoveDirection == TRUE && m_lMotionTestDist >= 0) || (m_bMoveDirection == FALSE && m_lMotionTestDist < 0))
	{
		bLogAxis = TRUE;
	}
	*/

	lDiff_X = nTestDistance;
	lDiff_Y = nTestDistance;

	if (lDiff_X <= m_lMinTravel_X)
	{
		nProf_X = FAST_PROF;
	}
	else
	{
		nProf_X = NORMAL_PROF;
	}

	if (lDiff_Y <= m_lMinTravel_Y)
	{
		nProf_Y = FAST_PROF;
	}
	else
	{
		nProf_Y = NORMAL_PROF;
	}
	
	
	if (m_bIsMotionTestBinTableX == TRUE)
	{
		X_Profile(nProf_X);

		if (bLogAxis)
		{
			//LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, TRUE, 10, m_stDataLog);
		}

		if (m_bMoveDirection)
		{
			LaserTestOuputBitTrigger(FALSE);

			if (X_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBinTableX = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);

		}
		else
		{
			nTestDistance = -1* nTestDistance;
			
			LaserTestOuputBitTrigger(FALSE);

			if (X_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBinTableX = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}

		X_Profile(NORMAL_PROF);

		if (bLogAxis)
		{
			//LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, FALSE, 10, m_stDataLog);
		}
	}
	else if (m_bIsMotionTestBinTableX2 == TRUE)
	{
		X2_Profile(nProf_X);

		if (bLogAxis)
		{
			//LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, TRUE, 10, m_stDataLog);
		}

		if (m_bMoveDirection)
		{
			LaserTestOuputBitTrigger(FALSE);

			if (X2_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBinTableX2 = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);

		}
		else
		{
			nTestDistance = -1* nTestDistance;
			
			LaserTestOuputBitTrigger(FALSE);

			if (X2_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBinTableX2 = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}

		X2_Profile(NORMAL_PROF);

		if (bLogAxis)
		{
			//LogAxisPerformance(BT_AXIS_X, &m_stBTAxis_X, FALSE, 10, m_stDataLog);
		}
	}
	else if (m_bIsMotionTestBinTableY == TRUE)
	{
		Y_Profile(nProf_Y);

		if (bLogAxis)
		{
			//LogAxisPerformance(BT_AXIS_Y, &m_stBTAxis_Y, TRUE, 10, m_stDataLog);
		}

		if (m_bMoveDirection)
		{
			LaserTestOuputBitTrigger(FALSE);

			if (Y_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBinTableY = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}
		else
		{
			nTestDistance = -1* nTestDistance;

			LaserTestOuputBitTrigger(FALSE);

			if (Y_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBinTableY = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}

		Y_Profile(NORMAL_PROF);
		
		if (bLogAxis)
		{
			//LogAxisPerformance(BT_AXIS_Y, &m_stBTAxis_Y, FALSE, 10, m_stDataLog);
		}
	}
	else if (m_bIsMotionTestBinTableY2)
	{
		Y2_Profile(nProf_Y);

		if (bLogAxis)
		{
			//LogAxisPerformance(BT_AXIS_Y, &m_stBTAxis_Y, TRUE, 10, m_stDataLog);
		}

		if (m_bMoveDirection)
		{
			LaserTestOuputBitTrigger(FALSE);

			if (Y2_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBinTableY2 = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}
		else
		{
			nTestDistance = -1* nTestDistance;

			LaserTestOuputBitTrigger(FALSE);

			if (Y2_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBinTableY2 = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}

		Y2_Profile(NORMAL_PROF);
		
		if (bLogAxis)
		{
			//LogAxisPerformance(BT_AXIS_Y, &m_stBTAxis_Y, FALSE, 10, m_stDataLog);
		}
	}

	//reverse the direction
	m_bMoveDirection = !m_bMoveDirection;

	if (m_lMotionTestDelay > 0)
	{
		Sleep(m_lMotionTestDelay);
	}

	Sleep(500);

	// for motion settling and reverse direction
	
	return TRUE;
}


BOOL CBinTable::CreateBinMap(CONST BOOL bCreate)
{
	CString szAlgorithm;
	CString szDummyWaferMapName;

	unsigned char aucAvailableGrades[1] = {'1'};
	unsigned char aaTempGrades[1];

	aucAvailableGrades[0] = 48 + 1;
	aaTempGrades[0] = 48 + 1;

	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;
	int i, j;

	//LONG lNoOfRows = m_oBinBlkMain.GrabNoOfDiePerRow(1);
	//LONG lNoOfCols = m_oBinBlkMain.GrabNoOfDiePerCol(1);
	LONG lNoOfRows = m_oBinBlkMain.GrabNoOfDiePerCol(1);
	LONG lNoOfCols = m_oBinBlkMain.GrabNoOfDiePerRow(1);

	//v4.36
	DOUBLE dRadius = 0.0;
	DOUBLE dCX = 1.00 * lNoOfCols / 2.0 + 0.5 + m_lBinMapXOffset;
	DOUBLE dCY = 1.00 * lNoOfRows / 2.0 + m_lBinMapYOffset;
	if (lNoOfRows > lNoOfCols)
	{
		dRadius = 1.00 * lNoOfCols / 2.0 + m_lBinMapROffset;
	}
	else
	{
		dRadius = 1.00 * lNoOfRows / 2.0 + m_lBinMapROffset;
	}


	if ((lNoOfRows > 300) || (lNoOfCols > 300))
		return FALSE;

	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows*lNoOfCols];
	aaGrades = new unsigned char*[lNoOfRows];

	for (i=0; i<lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i]; //new unsigned char[lNoOfCols];
	}

	for (INT m=0; m<MAX_BINMAP_SIZE_X; m++)
	{
		for (INT n=0; n<MAX_BINMAP_SIZE_Y; n++)
		{
			m_oBinBlkMain.m_nBinMap[m][n]	= 0;
			CBinBlk::m_nBinMap[m][n]		= 0;
			CBinBlk::m_cBondedDieMap[m][n] = 0;
		}
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
	m_BinMapWrapper.InitMap();
	//m_BinMapWrapper.ResetGradeMap();
	szDummyWaferMapName = "BinMap";	
	m_BinMapWrapper.CreateMap(szDummyWaferMapName, "Raw", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);

	if (bCreate)
	{
		for (ULONG i=0; i< (ULONG)lNoOfRows; i++)
		{
			for (ULONG j=0; j< (ULONG)lNoOfCols; j++)
			{
				if ( IsWithinCircularBinMapLimit(dRadius, dCX, dCY, (DOUBLE)j, (DOUBLE)i) )
				{
					m_BinMapWrapper.AddDie(i, j, aaTempGrades[0]);
					m_oBinBlkMain.m_nBinMap[i][j]	= 1;
					CBinBlk::m_nBinMap[i][j]		= 1;
				}
			}
		}
	}

	m_BinMapWrapper.SuspendAlgorithmPreparation();

	delete [] aaGrades;
	delete [] pGradeBuffer;

	m_BinMapWrapper.SelectGrade(aaTempGrades, 1);
	m_BinMapWrapper.ResumeAlgorithmPreparation(TRUE);
	
	return TRUE;
}

BOOL CBinTable::InitBinBlkBinMap()		//Nichia//v4.43T7
{
	for (ULONG i=0; i< (ULONG)MAX_BINMAP_SIZE_X; i++)
	{
		for (ULONG j=0; j< (ULONG)MAX_BINMAP_SIZE_Y; j++)
		{
				CBinBlk::m_nBinMap[i][j] = 1;
		}
	}

	return TRUE;
}

BOOL CBinTable::IsWithinCircularBinMapLimit(DOUBLE dRadius, DOUBLE dCX, DOUBLE dCY, DOUBLE dMapX, DOUBLE dMapY)
{
	BOOL bStatus = TRUE;

	DOUBLE dLengthX = pow(dMapX + 1.0 - dCX, 2.0);
	DOUBLE dLengthY = pow(dMapY + 1.0 - dCY, 2.0);

	DOUBLE dLength = sqrt(dLengthX + dLengthY);

	if ( dLength > (dRadius * 1.00) )
		bStatus = FALSE;

	return bStatus;
}


BOOL CBinTable::IsWithinOvalBinMapLimit(DOUBLE dCX, DOUBLE dCY, DOUBLE dMapX, DOUBLE dMapY, DOUBLE dRadiusX, DOUBLE dRadiusY)
{
	BOOL bStatus = TRUE;

	// Is within Oval limit:
	//
	//	sq(x)/sq(a) + sq(y)/sq(b) <= 1
	//

	DOUBLE dLengthX = pow(dMapX + 1.0 - dCX, 2.0);
	DOUBLE dLengthY = pow(dMapY + 1.0 - dCY, 2.0);
	DOUBLE dA		= pow(dRadiusX, 2.0);
	DOUBLE dB		= pow(dRadiusY, 2.0);

	DOUBLE dResult = dLengthX / dA + dLengthY / dB;

	if (dResult > 1)
		bStatus = FALSE;
	return bStatus;
}


BOOL CBinTable::CreateOsramBinMixMap(BOOL bCreate)
{
	CString szAlgorithm;
	CString szDummyWaferMapName;

	unsigned char aucAvailableGrades[2];
	unsigned char aaTempGrades[2];

	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;
	int i, j;


	LONG lNoOfRows = m_oBinBlkMain.GrabNoOfDiePerCol(1);
	LONG lNoOfCols = m_oBinBlkMain.GrabNoOfDiePerRow(1);
	if ( (lNoOfRows == 0) || (lNoOfCols == 0) )
	{
		SetErrorMessage("CreateOsramBinMixMap: Invalid die row & col");
		HmiMessage("ERROR: CreateOsramBinMixMap: Invalid die row & col!");
		return FALSE;
	}

	if ((lNoOfRows > MAX_BINMAP_SIZE_Y - 20) || (lNoOfCols > MAX_BINMAP_SIZE_X - 20))
	{
		SetErrorMessage("CreateOsramBinMixMap: map row-col exceeds limit of 1000");
		HmiMessage("ERROR: CreateOsramBinMixMap: map row-col exceeds limit of 1000");
		return FALSE;
	}


	ULONG ulPitchX = _round(m_oBinBlkMain.GrabDDiePitchX(1));	//in um
	ULONG ulPitchY = _round(m_oBinBlkMain.GrabDDiePitchY(1));	//in um
	ULONG ulPitchXInEnc = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(1)));	//in ENC
	ULONG ulPitchYInEnc = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchY(1)));	//in ENC
	if ( (ulPitchX == 0) || (ulPitchY == 0) )
	{
		//AfxMessageBox("Invalid die pitch!", MB_SYSTEMMODAL);
		SetErrorMessage("CreateOsramBinMixMap: Invalid die pitch");
		return FALSE;
	}

	UINT nNoOfDicesPerPattern = 0;
	CUIntArray oArrPattern;
	UCHAR ucTypeAGrade = m_ucBinMixTypeAGrade + m_BinMapWrapper.GetGradeOffset();
	UCHAR ucTypeBGrade = m_ucBinMixTypeBGrade + m_BinMapWrapper.GetGradeOffset();

	aucAvailableGrades[0]	= 48 + m_ucBinMixTypeAGrade;
	aucAvailableGrades[1]	= 48 + m_ucBinMixTypeBGrade;
	aaTempGrades[0]			= 48 + m_ucBinMixTypeAGrade;
	aaTempGrades[1]			= 48 + m_ucBinMixTypeBGrade;


//HmiMessage("CreateOsramBinMixMap .....");

	switch (m_ulBinMixPatternType)
	{
	case 1:		//ABAB
		oArrPattern.SetSize(2);
		oArrPattern.SetAt(0, ucTypeAGrade);
		oArrPattern.SetAt(1, ucTypeBGrade);
		nNoOfDicesPerPattern = 2;
//HmiMessage("BinBix Pattern = ABAB");
		break;
	case 2:		//AABAAB
		oArrPattern.SetSize(3);
		oArrPattern.SetAt(0, ucTypeAGrade);
		oArrPattern.SetAt(1, ucTypeAGrade);
		oArrPattern.SetAt(2, ucTypeBGrade);
		nNoOfDicesPerPattern = 3;
//HmiMessage("BinBix Pattern = AABAAB");
		break;
	case 3:		//AAABAAAB
		oArrPattern.SetSize(4);
		oArrPattern.SetAt(0, ucTypeAGrade);
		oArrPattern.SetAt(1, ucTypeAGrade);
		oArrPattern.SetAt(2, ucTypeAGrade);
		oArrPattern.SetAt(3, ucTypeBGrade);
		nNoOfDicesPerPattern = 4;
//HmiMessage("BinBix Pattern = AAABAAAB");
		break;
	case 4:		//AAAABAAAAB
		oArrPattern.SetSize(5);
		oArrPattern.SetAt(0, ucTypeAGrade);
		oArrPattern.SetAt(1, ucTypeAGrade);
		oArrPattern.SetAt(2, ucTypeAGrade);
		oArrPattern.SetAt(3, ucTypeAGrade);
		oArrPattern.SetAt(4, ucTypeBGrade);
		nNoOfDicesPerPattern = 5;
//HmiMessage("BinBix Pattern = AAAABAAAAB");
		break;
	case 5:		//AABBAABB
		oArrPattern.SetSize(4);
		oArrPattern.SetAt(0, ucTypeAGrade);
		oArrPattern.SetAt(1, ucTypeAGrade);
		oArrPattern.SetAt(2, ucTypeBGrade);
		oArrPattern.SetAt(3, ucTypeBGrade);
		nNoOfDicesPerPattern = 4;
//HmiMessage("BinBix Pattern = AABBAABB");
		break;
	case 6:		//ABBABB
		oArrPattern.SetSize(3);
		oArrPattern.SetAt(0, ucTypeAGrade);
		oArrPattern.SetAt(1, ucTypeBGrade);
		oArrPattern.SetAt(2, ucTypeBGrade);
		nNoOfDicesPerPattern = 3;
//HmiMessage("BinBix Pattern = ABBABB");
		break;
	case 7:		//ABBBABBB
		oArrPattern.SetSize(4);
		oArrPattern.SetAt(0, ucTypeAGrade);
		oArrPattern.SetAt(1, ucTypeBGrade);
		oArrPattern.SetAt(2, ucTypeBGrade);
		oArrPattern.SetAt(3, ucTypeBGrade);
		nNoOfDicesPerPattern = 4;
//HmiMessage("BinBix Pattern = ABBBABBB");
		break;
	case 8:		//ABBBBABBBB
		oArrPattern.SetSize(5);
		oArrPattern.SetAt(0, ucTypeAGrade);
		oArrPattern.SetAt(1, ucTypeBGrade);
		oArrPattern.SetAt(2, ucTypeBGrade);
		oArrPattern.SetAt(3, ucTypeBGrade);
		oArrPattern.SetAt(4, ucTypeBGrade);
		nNoOfDicesPerPattern = 5;
//HmiMessage("BinBix Pattern = ABBBBABBBB");
		break;
	default:
//HmiMessage("BinBix Pattern = NONE");
		return FALSE;
	}

	//v4.47A6
	INT nReminder = lNoOfCols % nNoOfDicesPerPattern;
	if (nReminder > 0)
	{
		lNoOfCols = lNoOfCols - nReminder;
	}


	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows*lNoOfCols];
	aaGrades = new unsigned char*[lNoOfRows];
	for (i=0; i<lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i]; //new unsigned char[lNoOfCols];
	}

	for (INT m=0; m<MAX_BINMAP_SIZE_X; m++)
	{
		for (INT n=0; n<MAX_BINMAP_SIZE_Y; n++)
		{
			m_oBinBlkMain.m_nBinMap[m][n]	= 0;
			CBinBlk::m_nBinMap[m][n]		= 0;
		}
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
	m_BinMapWrapper.InitMap();
	szDummyWaferMapName = "BinMap";	
	m_BinMapWrapper.CreateMap(szDummyWaferMapName, "Raw", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);

	if (bCreate)
	{
		for (i=0; i<lNoOfRows; i++)
		{
			INT nCurrColIndex = 0;
			INT nMaxIndex = (INT)oArrPattern.GetSize();

			for (j=0; j<lNoOfCols; j++)
			{
				m_BinMapWrapper.AddDie(i, j, oArrPattern.GetAt(nCurrColIndex));
				//m_oBinBlkMain.m_nBinMap[lMapRow][lMapCol]	= 1;
				//CBinBlk::m_nBinMap[lMapRow][lMapCol]		= 1;
				nCurrColIndex = nCurrColIndex + 1;
				if (nCurrColIndex >= nMaxIndex)
					nCurrColIndex = 0;
			}
		}
	}

	m_BinMapWrapper.SuspendAlgorithmPreparation();

	delete [] aaGrades;
	delete [] pGradeBuffer;

	m_BinMapWrapper.SelectGrade(aaTempGrades, 1);
	m_BinMapWrapper.ResumeAlgorithmPreparation(TRUE);	
	return TRUE;
}


BOOL CBinTable::CreateBinMap2(CONST LONG lCX, CONST LONG lCY, CONST DOUBLE dBondRadius, CONST BOOL bCreate)
{
	CString szAlgorithm;
	CString szDummyWaferMapName;

	unsigned char aucAvailableGrades[1] = {'1'};
	unsigned char aaTempGrades[1];

	aucAvailableGrades[0] = 48 + 1;
	aaTempGrades[0] = 48 + 1;

	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;
	int i, j;


	LONG lNoOfRows = m_oBinBlkMain.GrabNoOfDiePerCol(1);
	LONG lNoOfCols = m_oBinBlkMain.GrabNoOfDiePerRow(1);
	if ( (lNoOfRows == 0) || (lNoOfCols == 0) )
	{
		SetErrorMessage("CreateBinMap2: Invalid die row & col");
		HmiMessage("ERROR: CreateBinMap2: Invalid die row & col!");
		return FALSE;
	}

	if ((lNoOfRows > MAX_BINMAP_SIZE_Y - 20) || (lNoOfCols > MAX_BINMAP_SIZE_X - 20))
	{
		SetErrorMessage("CreateBinMap2: map row-col exceeds limit of 1000");
		HmiMessage("ERROR: CreateBinMap2: map row-col exceeds limit of 1000");
		return FALSE;
	}


	ULONG ulPitchX = _round(m_oBinBlkMain.GrabDDiePitchX(1));	//in um
	ULONG ulPitchY = _round(m_oBinBlkMain.GrabDDiePitchY(1));	//in um
	ULONG ulPitchXInEnc = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(1)));	//in ENC
	ULONG ulPitchYInEnc = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchY(1)));	//in ENC
	if ( (ulPitchX == 0) || (ulPitchY == 0) )
	{
		//AfxMessageBox("Invalid die pitch!", MB_SYSTEMMODAL);
		SetErrorMessage("CreateBinMap2: Invalid die pitch");
		return FALSE;
	}

	DOUBLE dRadiusInUm	= dBondRadius;						//User defined radius in mm
	LONG lRadius		= (LONG) (dRadiusInUm / ulPitchY);	//Radius in die steps
	LONG lRadiusX		= (LONG) (dRadiusInUm / ulPitchX);	//Radius in X die steps
	LONG lRadiusY		= (LONG) (dRadiusInUm / ulPitchY);	//Radius in Y die steps

	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows*lNoOfCols];
	aaGrades = new unsigned char*[lNoOfRows];
	for (i=0; i<lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i]; //new unsigned char[lNoOfCols];
	}

	for (INT m=0; m<MAX_BINMAP_SIZE_X; m++)
	{
		for (INT n=0; n<MAX_BINMAP_SIZE_Y; n++)
		{
			m_oBinBlkMain.m_nBinMap[m][n]	= 0;
			CBinBlk::m_nBinMap[m][n]		= 0;
		}
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
	m_BinMapWrapper.InitMap();
	//m_BinMapWrapper.ResetGradeMap();
	szDummyWaferMapName = "BinMap";	
	m_BinMapWrapper.CreateMap(szDummyWaferMapName, "Raw", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);


	if (bCreate)
	{
		//LONG lX=0, lY=0;
		DOUBLE dPosX=0, dPosY=0;		//v4.57A7
		LONG lIndex=0, lCurrIndex=0;
		ULONG ulQuotient, ulRemainder;
		LONG lMapCol=0, lMapRow=0;
		LONG lCapacity = lNoOfRows * lNoOfCols;
		LONG lEncX=0, lEncY=0;
		
		LONG lCMapCol=0, lCMapRow=0;
		//LONG lCDieX=0, lCDieY=0;
		DOUBLE dCDieX=0, dCDieY=0;		//v4.57A7
		BOOL bCDieFound = FALSE;
		
		LONG lULXinFileUnit = m_oBinBlkMain.GrabBlkUpperLeftX(1);
		LONG lULYinFileUnit = m_oBinBlkMain.GrabBlkUpperLeftY(1);
		LONG lLRXinFileUnit = m_oBinBlkMain.GrabBlkLowerRightX(1);
		LONG lLRYinFileUnit = m_oBinBlkMain.GrabBlkLowerRightY(1);
		LONG lCXinFileUnit	= ConvertXEncoderValueForDisplay(lCX);
		LONG lCYinFileUnit	= ConvertXEncoderValueForDisplay(lCY);
		CString szLog;

szLog.Format("CreateBinMap2: bin area dimension (um): UL=(%ld, %ld); LR=(%ld, %ld)",
				lULXinFileUnit, lULYinFileUnit, lLRXinFileUnit, lLRYinFileUnit);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

szLog.Format("CreateBinMap2: DiePitch um=(%ld, %ld); Enc=(%ld, %ld)",
				ulPitchX, ulPitchY, ulPitchXInEnc, ulPitchYInEnc);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

szLog.Format("CreateBinMap2: JS posn um=(%ld, %ld); Enc=(%ld, %ld)", 
				lCXinFileUnit, lCYinFileUnit, lCX, lCY);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		//v4.59A41
		LONG lDividend = lNoOfCols;
		BOOL bUseHorizPath = TRUE;
		ULONG ulWalkPath = m_oBinBlkMain.GrabWalkPath(1);
		if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH) ||
			 (ulWalkPath == BT_BLV_PATH) || (ulWalkPath == BT_BRV_PATH) )
		{
			lDividend = lNoOfRows;
			bUseHorizPath = FALSE;
		}

		//Find closet die posn near lCX & lCY enc
		for (lIndex=1; lIndex<=lCapacity; lIndex++)
		{
			lCurrIndex = lIndex;
			m_oBinBlkMain.GetIndexMovePosnXY(1, lIndex, dPosX, dPosY);

			lEncX = ConvertFileUnitToXEncoderValue(dPosX);
			lEncY = ConvertFileUnitToXEncoderValue(dPosY);

			//if (lIndex == 1)
			//{
			//	szLog.Format("INDEX = %ld, Pos(%.2f, %.2f), Enc(%ld, %ld)", lIndex, dPosX, dPosY, lEncX, lEncY);
			//	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			//}

			if ((labs(lEncX - lCX) * 2 <= (LONG)ulPitchXInEnc) && (labs(lEncY - lCY) * 2 <= (LONG)ulPitchYInEnc))
			{
				bCDieFound	= TRUE;
				dCDieX		= dPosX;
				dCDieY		= dPosY;

				ulQuotient	= lCurrIndex / lDividend;
				ulRemainder = lCurrIndex % lDividend;

				if (bUseHorizPath)	//v4.59A41
				{
					if (ulRemainder == 0)	//change row die - either the first or last in row
					{
						lCMapRow = ulQuotient - 1;

						if (ulQuotient % 2 == 1)	//last in row
						{
							lCMapCol = lNoOfCols - 1;
						}
						else						//first in row
						{
							lCMapCol = 0;
						}
					}
					else	//die other than the change row one
					{
						lCMapRow = ulQuotient;

						if (ulQuotient % 2 == 0)	//on odd row
						{
							lCMapCol = ulRemainder - 1;
						}
						else						//on even row
						{
							lCMapCol = lNoOfCols - ulRemainder - 1;
						}
					}
				}
				else	//vertical path
				{
					if (ulRemainder == 0)	//change row die - either the first or last in row
					{
						lCMapCol = ulQuotient - 1;

						if (ulQuotient % 2 == 1)	//last in row
						{
							lCMapRow = lNoOfRows - 1;
						}
						else						//first in row
						{
							lCMapRow = 0;
						}
					}
					else	//die other than the change row one
					{
						lCMapCol = ulQuotient;

						if (ulQuotient % 2 == 0)	//on odd row
						{
							lCMapRow = ulRemainder - 1;
						}
						else						//on even row
						{
							lCMapRow = lNoOfRows - ulRemainder - 1;
						}
					}
				}
				break;
			}
		}

		if (!bCDieFound)
		{
			//AfxMessageBox("Center Die not found!", MB_SYSTEMMODAL);
			SetErrorMessage("CreateBinMap2: Center die ROW/COL not found");

			//Klocwork	//v4.36T1
			delete [] aaGrades;
			delete [] pGradeBuffer;
			return FALSE;
		}

		//v4.42T1	//Nichia & TW
		m_lBinMapCDieRowOffset	= lCMapRow;
		m_lBinMapCDieColOffset	= lCMapCol;

		CString szMsg;
		szMsg.Format("Center Die at ROW = %ld, COL = %ld", lCMapRow, lCMapCol);
		//AfxMessageBox(szMsg, MB_SYSTEMMODAL);

szLog.Format("CreateBinMap2: NoOfCols = %ld; NoOfRows=%ld; Dividend=%ld", lNoOfCols, lNoOfRows, lDividend);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
szLog.Format("CreateBinMap2: CenterDie found at ROW=%ld; COL=%ld", lCMapRow, lCMapCol);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
lEncX = ConvertFileUnitToXEncoderValue(dCDieX);
lEncY = ConvertFileUnitToXEncoderValue(dCDieY);
szLog.Format("CreateBinMap2: CenterDie um=(%f, %f); Enc=(%ld, %ld)\n", 
				dCDieX, dCDieX, lEncX, lEncY);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


		//** Then start to look at every die posn and check if each of them is within this circle limit **//
		for (lIndex=1; lIndex<=lCapacity; lIndex++)
		{
			lCurrIndex = lIndex;
			m_oBinBlkMain.GetIndexMovePosnXY(1, lIndex, dPosX, dPosY);

			ulQuotient	= lCurrIndex / lNoOfCols;
			ulRemainder = lCurrIndex % lNoOfCols;

			if (ulRemainder == 0)	//change row die - either the first or last in row
			{
				lMapRow = ulQuotient - 1;

				if (ulQuotient % 2 == 1)	//last in row
				{
					lMapCol = lNoOfCols - 1;
				}
				else						//first in row
				{
					lMapCol = 0;
				}
			}
			else	//die other than the change row one
			{
				lMapRow = ulQuotient;

				if (ulQuotient % 2 == 0)	//on odd row
				{
					lMapCol = ulRemainder - 1;
				}
				else						//on even row
				{
					//lMapCol = lNoOfCols - ulRemainder - 1;
					lMapCol = lNoOfCols - ulRemainder;		//v4.53A29
				}
			}

			//if ( IsWithinCircularBinMapLimit2(dRadiusInUm, (DOUBLE)lCDieX, (DOUBLE)lCDieY, (DOUBLE)lX, (DOUBLE)lY) )
			//if ( IsWithinCircularBinMapLimit((DOUBLE)lRadius, (DOUBLE)lCMapCol, (DOUBLE)lCMapRow, (DOUBLE)lMapCol, (DOUBLE)lMapRow) )
			if ( IsWithinOvalBinMapLimit( (DOUBLE)lCMapCol, (DOUBLE)lCMapRow, 
										  (DOUBLE)lMapCol,  (DOUBLE)lMapRow,
										  (DOUBLE)lRadiusX, (DOUBLE)lRadiusY ) )
			{
				if ((lMapRow >= 0) && (lMapCol >= 0)	&& 
					(lMapRow < MAX_BINMAP_SIZE_X)		&& 
					(lMapCol < MAX_BINMAP_SIZE_Y) )
				{
					m_BinMapWrapper.AddDie(lMapRow, lMapCol, aaTempGrades[0]);
					m_oBinBlkMain.m_nBinMap[lMapRow][lMapCol]	= 1;
					CBinBlk::m_nBinMap[lMapRow][lMapCol]		= 1;
				}
			}
		}
	}

	m_oBinBlkMain.SetBinMapCircleRadius(0, m_dBinMapCircleRadiusInMm * 1000);
	m_oBinBlkMain.SetBinMapEdgeSize(0, m_lBinMapEdgeSize, m_lBinMapBEdgeSize, 
										m_lBinMapLEdgeSize, m_lBinMapREdgeSize);
	SetBinMapEdgeDices();
	m_BinMapWrapper.SuspendAlgorithmPreparation();

	delete [] aaGrades;
	delete [] pGradeBuffer;

	m_BinMapWrapper.SelectGrade(aaTempGrades, 1);
	m_BinMapWrapper.ResumeAlgorithmPreparation(TRUE);	
	return TRUE;
}


BOOL CBinTable::IsWithinCircularBinMapLimit2(DOUBLE dRadius, DOUBLE dCX, DOUBLE dCY, DOUBLE dDieX, DOUBLE dDieY)
{
	BOOL bStatus = TRUE;

	DOUBLE dLengthX = pow(dDieX - dCX, 2.0);
	DOUBLE dLengthY = pow(dDieY - dCY, 2.0);
	DOUBLE dLength  = sqrt(dLengthX + dLengthY);	//in Um
	
	if ( dLength > (dRadius * 1.05) )
		return FALSE;
	return bStatus;
}


BOOL CBinTable::SetBinMapEdgeDices()
{
	//if (m_lBinMapEdgeSize <= 0)
	//	return TRUE;

	BOOL b1stRowFound = FALSE;
	BOOL bLastRowFound = FALSE;
	BOOL b1stColFound = FALSE;
	BOOL bLastColFound = FALSE;
	LONG l1stRow = 0;
	LONG lLastRow = 0;
	LONG l1stCol = 0;
	LONG lLastCol = 0;

	INT nRow=0, nCol=0;
	
	//Find first row
	for (nRow=0; nRow<MAX_BINMAP_SIZE_Y; nRow++)
	{
		for (nCol=0; nCol<MAX_BINMAP_SIZE_X; nCol++)
		{
			if (m_oBinBlkMain.m_nBinMap[nRow][nCol] == 1)
			{
				l1stRow = nRow;
				b1stRowFound = TRUE;
				break;
			}
		}
		if (b1stRowFound)
			break;
	}

	if (b1stRowFound)
	{
		for (nRow=l1stRow; nRow<l1stRow+m_lBinMapEdgeSize; nRow++)
		{
			for (nCol=0; nCol<MAX_BINMAP_SIZE_X; nCol++)
			{
				if (m_oBinBlkMain.m_nBinMap[nRow][nCol] == 1)
				{
					m_BinMapWrapper.ChangeGrade(nRow, nCol, m_BinMapWrapper.GetNullBin());
					m_oBinBlkMain.m_nBinMap[nRow][nCol]	= 0;
					CBinBlk::m_nBinMap[nRow][nCol]		= 0;
				}
			}
		}
	}


	//Find last row
	for (nRow=MAX_BINMAP_SIZE_Y-1; nRow>=0; nRow--)
	{
		for (nCol=0; nCol<MAX_BINMAP_SIZE_X; nCol++)
		{
			if (m_oBinBlkMain.m_nBinMap[nRow][nCol] == 1)
			{
				lLastRow = nRow;
				bLastRowFound = TRUE;
				break;
			}
		}
		if (bLastRowFound)
			break;
	}

	if (bLastRowFound)
	{
		for (nRow=lLastRow; nRow>lLastRow-m_lBinMapBEdgeSize; nRow--)
		{
			for (nCol=0; nCol<MAX_BINMAP_SIZE_X; nCol++)
			{
				if (m_oBinBlkMain.m_nBinMap[nRow][nCol] == 1)
				{
					m_BinMapWrapper.ChangeGrade(nRow, nCol, m_BinMapWrapper.GetNullBin());
					m_oBinBlkMain.m_nBinMap[nRow][nCol]	= 0;
					CBinBlk::m_nBinMap[nRow][nCol]		= 0;
				}
			}
		}
	}


	//Find first column from left
	for (nCol=0; nCol<MAX_BINMAP_SIZE_X; nCol++)
	{
		for (nRow=0; nRow<MAX_BINMAP_SIZE_Y; nRow++)
		{
			if (m_oBinBlkMain.m_nBinMap[nRow][nCol] == 1)
			{
				l1stCol = nCol;
				b1stColFound = TRUE;
				break;
			}
		}
		if (b1stColFound)
			break;
	}

	if (b1stColFound)
	{
		for (nCol=l1stCol; nCol<l1stCol+m_lBinMapLEdgeSize; nCol++)
		{
			for (nRow=0; nRow<MAX_BINMAP_SIZE_Y; nRow++)
			{
				if (m_oBinBlkMain.m_nBinMap[nRow][nCol] == 1)
				{
					m_BinMapWrapper.ChangeGrade(nRow, nCol, m_BinMapWrapper.GetNullBin());
					m_oBinBlkMain.m_nBinMap[nRow][nCol]	= 0;
					CBinBlk::m_nBinMap[nRow][nCol]		= 0;
				}
			}
		}
	}


	//Find last column from left
	for (nCol=MAX_BINMAP_SIZE_X-1; nCol>=0; nCol--)
	{
		for (nRow=0; nRow<MAX_BINMAP_SIZE_Y; nRow++)
		{
			if (m_oBinBlkMain.m_nBinMap[nRow][nCol] == 1)
			{
				lLastCol = nCol;
				bLastColFound = TRUE;
				break;
			}
		}
		if (bLastColFound)
			break;
	}

	if (bLastColFound)
	{
		for (nCol=lLastCol; nCol>lLastCol-m_lBinMapREdgeSize; nCol--)
		{
			for (nRow=0; nRow<MAX_BINMAP_SIZE_Y; nRow++)
			{
				if (m_oBinBlkMain.m_nBinMap[nRow][nCol] == 1)
				{
					m_BinMapWrapper.ChangeGrade(nRow, nCol, m_BinMapWrapper.GetNullBin());
					m_oBinBlkMain.m_nBinMap[nRow][nCol]	= 0;
					CBinBlk::m_nBinMap[nRow][nCol]		= 0;
				}
			}
		}
	}

	return TRUE;
}


BOOL CBinTable::LoadOsramBinMixMap()
{
	ULONG ulNumOfRows=0, ulNumOfCols=0;
	m_BinMapWrapper.GetMapDimension(ulNumOfRows, ulNumOfCols);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
		return FALSE;

	int i=0, j=0;
	INT nDie = 0;
	unsigned char aaTempGrades[1];
	aaTempGrades[0] = 48 + 1;

	CStdioFile oFile;
	CString szLine;
	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];
	
	//v4.38T2 Tyntek
	INT nPos = szPKGFilename.Find(".");
	if (nPos != -1)
		szPKGFilename = szPKGFilename.Left(nPos);

	CString szBinMapFileName = _T("c:\\mapsorter\\Exe\\Bmap\\Binmap_") + szPKGFilename + _T(".dat");

	//if (m_szBinMapFilePath.GetLength() > 0)
	//{
	//	szBinMapFileName = m_szBinMapFilePath + _T("\\Binmap_") + szPKGFilename + _T(".dat");	//v4.37T12
	//}

	if (!oFile.Open(szBinMapFileName, 
		CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		SetErrorMessage("Load Bin Map: fails to load binmap at: " + szBinMapFileName);
		return FALSE;
	}

	for (INT m=0; m<MAX_BINMAP_SIZE_X; m++)
	{
		for (INT n=0; n<MAX_BINMAP_SIZE_Y; n++)
		{
			m_oBinBlkMain.m_nBinMap[m][n]	= 0;
			CBinBlk::m_nBinMap[m][n]		= 0;	//v4.04
		}
	}

	CString szRow, szCol, szGrade;
	LONG lRow, lCol;
	UCHAR ucGrade;

	while (oFile.ReadString(szLine))
	{
		szRow	= szLine.Left(szLine.Find(","));
		szLine	= szLine.Mid(szLine.Find(",") + 1);
		szCol	= szLine.Left(szLine.Find(","));
		szGrade = szLine.Mid(szLine.Find(",") + 1);

		lRow	= atol(szRow);
		lCol	= atol(szCol);
		ucGrade = atoi(szGrade);
		aaTempGrades[0] = 48 + ucGrade;

		if ((lRow >= 0) && (lRow < MAX_BINMAP_SIZE_X))
		{
			if ((lCol >= 0) && (lCol < MAX_BINMAP_SIZE_Y))
			{
				m_BinMapWrapper.AddDie(lRow, lCol, aaTempGrades[0]);
				m_oBinBlkMain.m_nBinMap[lRow][lCol] = ucGrade;
				CBinBlk::m_nBinMap[lRow][lCol]		= ucGrade;
			}
		}
	}

	oFile.Close();
	return TRUE;
}


BOOL CBinTable::LoadBinMap(BOOL bInInitData, ULONG &ulBinCount)
{
	ULONG ulNumOfRows=0, ulNumOfCols=0;
	m_BinMapWrapper.GetMapDimension(ulNumOfRows, ulNumOfCols);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
		return FALSE;
	//if (!m_bEnableOsramBinMixMap)		//v4.47A8 bug-fix
	//	return FALSE;

	int i=0, j=0;
	INT nDie = 0;
	unsigned char aaTempGrades[1];
	aaTempGrades[0] = 48 + 1;

	CStdioFile oFile;
	CString szLine, szMsg, szBinMapInputPath, szCurBinMapPath;
	//BOOL bUseLocalBinMap = FALSE;
	BOOL bCopyFileToCurBinMap = FALSE;
	CString szBinMapFileName;

	//Get PKG File Name
	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];
	INT nPos = szPKGFilename.Find(".");
	if (nPos != -1)
		szPKGFilename = szPKGFilename.Left(nPos);

	// Input Path Bin Map
	BOOL bIsBinMapInputPathEmpty = m_szBinMapFilePath.IsEmpty();
	if (!bIsBinMapInputPathEmpty)
	{
		szBinMapInputPath = m_szBinMapFilePath + _T("\\Binmap_") + szPKGFilename + _T(".dat");
	}

	//Local Current Bin Map
	szCurBinMapPath = gszEXE_DIRECTORY + "\\CurBinMap.dat";

	if (bInInitData)
	{
		//Load CurBinMap.dat for Warm Start
		szBinMapFileName = szCurBinMapPath;
	}
	else
	{
		//Load Input path bin map for Load button in HMI
		if (bIsBinMapInputPathEmpty)
		{
			szMsg = "Load Bin Map: Bin Map input path is empty!";
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			HmiMessage(szMsg);
			return FALSE;
		}
		szBinMapFileName = szBinMapInputPath;
	}

	if (!oFile.Open(szBinMapFileName, CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		szMsg = "Load Bin Map: fail to load binmap at: " + szBinMapFileName;
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage(szMsg);
		return FALSE;
	}

	//if (m_szBinMapFilePath.GetLength() > 0)
	//{
	//	if ( (pApp->GetCustomerName() == CTM_NICHIA)	&&	//v4.42T13
	//		 (pApp->GetProductLine() == _T("")) )			//v4.59A34
	//	{
	//		//54321
	//		//szBinMapFileName = m_szBinMapFilePath + _T("\\Binmap_nichia.dat");
	//	}

	//	if (!oFile.Open(szBinMapFileName, 
	//		CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	//	{
	//		SetErrorMessage("Load Bin Map: fail to load binmap at: " + szBinMapFileName);
	//		CMSLogFileUtility::Instance()->MS_LogOperation("Load Bin Map: fail to load binmap at: " + szBinMapFileName);
	//		//v4.48A13	//try to load local binmap if network copy is NOT available
	//		/*if (!oFile.Open(szLocalBinMapFile, 
	//			CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	//		{
	//			SetErrorMessage("Load Bin Map: fails to load local binmap at: " + szLocalBinMapFile);
	//			CMSLogFileUtility::Instance()->MS_LogOperation("Load Bin Map: fails to load local binmap at: " + szLocalBinMapFile);
	//			return FALSE;
	//		}
	//		else
	//		{
	//			CMSLogFileUtility::Instance()->MS_LogOperation("Load Bin Map: Success to load local binmap at: " + szLocalBinMapFile);
	//			bUseLocalBinMap = TRUE;
	//		}*/
	//		if (bInInitData) //For fail to load CurBinMap.dat only
	//		{
	//			if (!oFile.Open(szBinMapInputPath, CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	//			{
	//				SetErrorMessage("Load Bin Map: fail to load input path binmap at: " + szBinMapInputPath);
	//				CMSLogFileUtility::Instance()->MS_LogOperation("Load Bin Map: fail to load input path binmap at: " + szBinMapInputPath);
	//				return FALSE;
	//			}
	//			else
	//			{
	//				CMSLogFileUtility::Instance()->MS_LogOperation("Load Bin Map: Success to load input path binmap at: " + szBinMapInputPath);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		if (!bInInitData)
	//		{
	//			bCopyFileToCurBinMap = TRUE;
	//		}
	//	}
	//}
	//else
	//{
	//	/*if (!oFile.Open(szBinMapFileName, 
	//		CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	//	{
	//		SetErrorMessage("Load Bin Map: fails to load binmap at: " + szBinMapFileName);
	//		CMSLogFileUtility::Instance()->MS_LogOperation("Load Bin Map: fails to load binmap at: " + szBinMapFileName);
	//		return FALSE;
	//	}*/
	//	if (!bInInitData)
	//	{
	//		bCopyFileToCurBinMap = TRUE;
	//	}
	//}

	for (INT m=0; m<MAX_BINMAP_SIZE_X; m++)
	{
		for (INT n=0; n<MAX_BINMAP_SIZE_Y; n++)
		{
			m_oBinBlkMain.m_nBinMap[m][n]	= 0;
			CBinBlk::m_nBinMap[m][n]		= 0;	//v4.04
		}
	}

	ulBinCount = 0;
	CString szRow, szCol;
	LONG lRow, lCol;
	while (oFile.ReadString(szLine))
	{
		szRow	= szLine.Left(szLine.Find(","));
		szLine	= szLine.Mid(szLine.Find(",") + 1);
		szCol	= szLine.Left(szLine.Find(","));

		lRow = atol(szRow);
		lCol = atol(szCol);

		if ((lRow >= 0) && (lRow < MAX_BINMAP_SIZE_X))
		{
			if ((lCol >= 0) && (lCol < MAX_BINMAP_SIZE_Y))
			{
				m_BinMapWrapper.AddDie(lRow, lCol, aaTempGrades[0]);
				m_oBinBlkMain.m_nBinMap[lRow][lCol] = 1;
				CBinBlk::m_nBinMap[lRow][lCol]		= 1;	//v4.04
				ulBinCount++;
			}
		}
	}

	oFile.Close();
	/*if (bCopyFileToCurBinMap == TRUE)
	{
		BackupBinMap(bCopyFileToCurBinMap);
	}*/
	//if (bCopyFileToLocalBmapFolder == TRUE)
	//{
	//	CString szMsg;
	//	if (CopyFile(szBinMapFileName, szLocalBinMapFile, FALSE) != 0)
	//	{
	//		szMsg.Format("BINMAP: Copy BINMAP from (%s) to local (%s)", szBinMapFileName, szLocalBinMapFile);	
	//	}
	//	else
	//	{
	//		szMsg.Format("BINMAP: Fail to Copy BINMAP from (%s) to local (%s)", szBinMapFileName, szLocalBinMapFile);
	//	}
	//	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	//}
	//if (bUseLocalBinMap)
	//	CMSLogFileUtility::Instance()->MS_LogOperation("BT: local Binmap is loaded OK - " + szLocalBinMapFile);	//v4.49A13
	//else
	//	CMSLogFileUtility::Instance()->MS_LogOperation("BT: Binmap is loaded OK - " + szBinMapFileName);		//v4.48A21
	return TRUE;
}


BOOL CBinTable::SaveBinMap(ULONG &ulNewInputCount)
{
	ULONG ulNumOfRows=0, ulNumOfCols=0;
	m_BinMapWrapper.GetMapDimension(ulNumOfRows, ulNumOfCols);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
		return FALSE;

	ULONG i=0, j=0;

	CStdioFile oFile;
	CString szLine;
	ULONG ulCount = 0;
	CString szBinMapFileName;
	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];

	//v4.38T2 Tyntek
	INT nPos = szPKGFilename.Find(".");
	if (nPos != -1)
		szPKGFilename = szPKGFilename.Left(nPos);
	
	if ( (pApp->GetCustomerName() == CTM_NICHIA)	&&	//v4.42T13
		 (pApp->GetProductLine() == _T("")) )			//v4.59A34
	{
		szBinMapFileName = _T("c:\\mapsorter\\Exe\\Bmap\\Binmap_nichia.dat");
	}

	if (m_szBinMapFilePath.GetLength() > 0)
	{
		szBinMapFileName = m_szBinMapFilePath + _T("\\Binmap_") + szPKGFilename + _T(".dat");	//v4.37T12
		if ( (pApp->GetCustomerName() == CTM_NICHIA)	&&	//v4.42T13
			 (pApp->GetProductLine() == _T("")) )			//v4.59A34
		{
			//Nichia//v4.43T7
			//szBinMapFileName = m_szBinMapFilePath + _T("\\Binmap_nichia.dat");
		}
	}

	if (!oFile.Open(szBinMapFileName, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		ulNewInputCount = 0;
		return FALSE;
	}

	for (INT m=0; m<MAX_BINMAP_SIZE_X; m++)
	{
		for (INT n=0; n<MAX_BINMAP_SIZE_Y; n++)
		{
			m_oBinBlkMain.m_nBinMap[m][n]	= 0;
			CBinBlk::m_nBinMap[m][n]		= 0;	//v4.04
		}
	}

	for (i=0;i<ulNumOfRows; i++)
	{
		for (j=0; j<ulNumOfCols; j++)
		{
			if (m_BinMapWrapper.GetGrade(i, j) == 49)
			{
				ulCount++;
				m_oBinBlkMain.m_nBinMap[i][j]	= 1;
				CBinBlk::m_nBinMap[i][j]		= 1;	//v4.04
				szLine.Format("%d,%d,%d\n", i,j,1);
				oFile.WriteString(szLine);
			}
			else
			{
				m_oBinBlkMain.m_nBinMap[i][j]	= 0;
				CBinBlk::m_nBinMap[i][j]		= 0;	//v4.04
			}
		}
	}
	
	oFile.Close();
	ulNewInputCount = ulCount;

	CString szLog;
	szLog.Format("BT: Binmap (%s) is saved; InputCount = %lu", szBinMapFileName, ulNewInputCount);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);	//v4.48A21

	return TRUE;
}

VOID CBinTable::UpdateBinMapCapacity(const ULONG ulNewInputCount)
{
	m_oBinBlkMain.SetUseBinMapOffset(1, m_bEnableBinMapCDieOffset, 
							m_lBinMapCDieRowOffset, m_lBinMapCDieColOffset);

	//UPdate Input counts
	CString szMsg;
	szMsg.Format("DEFAULT binmap updated with new bin count = %d; INPUT-COUNT will also be updated.", ulNewInputCount);
	HmiMessage(szMsg);

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	for (INT i = 1; i<= BT_MAX_BINBLK_NO; i++)
	{
		LONG ulInputCount = ulNewInputCount;
		if (ulInputCount > m_oBinBlkMain.GrabGradeCapacity(i))
		{
			ulInputCount = m_oBinBlkMain.GrabGradeCapacity(i);
		}

		m_oBinBlkMain.SaveGradeInfo(FALSE, i, ulInputCount, 1, pBTfile);
	}

	CMSFileUtility::Instance()->SaveBTConfig();
	SaveBinTableData();
}

VOID CBinTable::BackupBinMap(BOOL bFromInputPathToCur)
{
	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];
	INT nPos = szPKGFilename.Find(".");
	if (nPos != -1)
	{
		szPKGFilename = szPKGFilename.Left(nPos);
	}
	CString szTgtPath, szSrcPath;
	CString szLogMsg;

	if (bFromInputPathToCur)
	{
		szSrcPath = m_szBinMapFilePath + _T("\\Binmap_") + szPKGFilename + _T(".dat");
		szTgtPath = gszEXE_DIRECTORY + "\\CurBinMap.dat";		
	}
	else
	{
		szSrcPath = gszEXE_DIRECTORY + "\\CurBinMap.dat";
		szTgtPath = m_szBinMapFilePath + _T("\\Binmap_") + szPKGFilename + _T(".dat");
	}

	if (CopyFile(szSrcPath, szTgtPath, FALSE) != 0)
	{
		szLogMsg.Format("Backup Bin Map from %s to %s", szSrcPath, szTgtPath);
	}
	else
	{
		szLogMsg.Format("Fail to Backup Bin Map from %s to %s", szSrcPath, szTgtPath);
		HmiMessage_Red_Back(szLogMsg, "Backup Bin Map");
	}
	CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg);
}


ULONG CBinTable::GetBinMapCount(ULONG ulBlkID)	//v4.49A1
{
	ULONG ulCount = 0;
	for (INT i=0;i<MAX_BINMAP_SIZE_X; i++)
	{
		for (INT j=0; j<MAX_BINMAP_SIZE_Y; j++)
		{
			if (m_oBinBlkMain.m_nBinMap[i][j] == 1)
			{
				ulCount++;
			}
			//if (CBinBlk::m_nBinMap[i][j] == 1)
			//{
			//	ulCount++;
			//}
		}
	}
	return ulCount;
}

BOOL CBinTable::SaveOsramBinMixMap(ULONG &ulNewInputCount)
{
	ULONG ulNumOfRows=0, ulNumOfCols=0;
	m_BinMapWrapper.GetMapDimension(ulNumOfRows, ulNumOfCols);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
		return FALSE;
	if (!m_bEnableOsramBinMixMap)
		return FALSE;

	ULONG i=0, j=0;

	CStdioFile oFile;
	CString szLine;
	ULONG ulCount = 0;

	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];

	//v4.38T2 Tyntek
	INT nPos = szPKGFilename.Find(".");
	if (nPos != -1)
		szPKGFilename = szPKGFilename.Left(nPos);

	CString szBinMapFileName = _T("c:\\mapsorter\\Exe\\Bmap\\Binmap_") + szPKGFilename + _T(".dat");

	CreateDirectory("c:\\Mapsorter\\Exe\\Bmap", NULL);

	//if (m_szBinMapFilePath.GetLength() > 0)
	//{
	//	CreateDirectory(m_szBinMapFilePath, NULL);
	//	szBinMapFileName = m_szBinMapFilePath + _T("\\Binmap_") + szPKGFilename + _T(".dat");
	//}

	if (!oFile.Open(szBinMapFileName, 
		CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		ulNewInputCount = 0;
		return FALSE;
	}

	for (INT m=0; m<MAX_BINMAP_SIZE_X; m++)
	{
		for (INT n=0; n<MAX_BINMAP_SIZE_Y; n++)
		{
			m_oBinBlkMain.m_nBinMap[m][n]	= 0;
			CBinBlk::m_nBinMap[m][n]		= 0;
		}
	}

	UCHAR ucTypeAGrade = m_ucBinMixTypeAGrade + m_BinMapWrapper.GetGradeOffset();
	UCHAR ucTypeBGrade = m_ucBinMixTypeBGrade + m_BinMapWrapper.GetGradeOffset();

	for (i=0;i<ulNumOfRows; i++)
	{
		for (j=0; j<ulNumOfCols; j++)
		{
			if (m_BinMapWrapper.GetGrade(i, j) == ucTypeAGrade)
			{
				ulCount++;
				m_oBinBlkMain.m_nBinMap[i][j]	= m_ucBinMixTypeAGrade;
				CBinBlk::m_nBinMap[i][j]		= m_ucBinMixTypeAGrade;
				szLine.Format("%d,%d,%d\n", i, j, m_ucBinMixTypeAGrade);
				oFile.WriteString(szLine);
			}
			else if (m_BinMapWrapper.GetGrade(i, j) == ucTypeBGrade)
			{
				ulCount++;
				m_oBinBlkMain.m_nBinMap[i][j]	= m_ucBinMixTypeBGrade;
				CBinBlk::m_nBinMap[i][j]		= m_ucBinMixTypeBGrade;
				szLine.Format("%d,%d,%d\n", i, j, m_ucBinMixTypeBGrade);
				oFile.WriteString(szLine);
			}
			else
			{
				m_oBinBlkMain.m_nBinMap[i][j]	= 0;
				CBinBlk::m_nBinMap[i][j]		= 0;	//v4.04
			}
		}
	}

	oFile.Close();
	ulNewInputCount = ulCount;
	return TRUE;
}


BOOL CBinTable::LoadBinMapData(ULONG ulBlkID)
{
	if (!m_bEnableBinMapBondArea)
	{
		return FALSE;
	}
	m_bEnableBinMapCDieOffset	= m_oBinBlkMain.GetBinMapOffset(ulBlkID, 
									m_lBinMapCDieRowOffset, m_lBinMapCDieColOffset);
	return TRUE;
}

CString CBinTable::GetWaferPrDeviceID()
{
	IPC_CServiceMessage stMsg;
	BOOL bReply;
	INT nConvID = 0;
	CString szDeviceID;

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "IMGetPRDeviceIdCmd", stMsg);
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

	szDeviceID = (*m_psmfSRam)["WaferPr"]["DeviceId"];
	
	return szDeviceID;
}

VOID CBinTable::CheckNewHoopsToLeaveEmptyRow()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if(pApp ->GetCustomerName() == "FiberOptics")
	{
		m_oBinBlkMain.SetLeaveEmptyRowArray(TRUE);
		OpGenAllTempFile();		//Must update all bin frame's die index before AUTOBOND!
		return;
	}
	if ( (pApp->GetCustomerName() != "CyOptics") &&		//This fcn only available for CyOptics US
		 (pApp->GetCustomerName() != "Inari") )			//v4.51A24
	{
		m_oBinBlkMain.SetLeaveEmptyRowArray(FALSE);
		return;
	}

	//THis fcn is called in PerbondEvent during LoadMap to check if new Hoops is changed to trigger
	//NEw bin rows for CyOptics; must be called AFTER CHeckMapHeaderInfo() to retrieve Hoops header before
	//calling this fcn;
	BOOL bNewHoops = (BOOL)(LONG) (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CYOPTICS_HOOPS];
	CString szWaferNumber;

	if (bNewHoops)
	{
		CStringMapFile  *psmf;
		CMSFileUtility  *pUtl = CMSFileUtility::Instance();

		if (pUtl->LoadLastState() == TRUE)
		{
			psmf = pUtl->GetLastStateFile();	
			if (psmf != NULL)	//Klocwork	//v4.30T5
			{
				szWaferNumber = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CYOPTICS_HOOPS];
			}
			pUtl->CloseLastState();
		}

		//AfxMessageBox("New Hoop !!!!", MB_SYSTEMMODAL);
		CMSLogFileUtility::Instance()->MS_LogOperation("CyOptics: Wafer-Number is changed - " + szWaferNumber);
		m_oBinBlkMain.SetLeaveEmptyRowArray(TRUE);

		OpGenAllTempFile();		//Must update all bin frame's die index before AUTOBOND!
	}
	else
	{
		//AfxMessageBox("Old Hoop !!!!", MB_SYSTEMMODAL);
		//m_oBinBlkMain.SetLeaveEmptyRowArray(FALSE);
	}
}


BOOL CBinTable::CheckNichiaArrCodeInMap()	//v4.42T2	//Nichia
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp->GetCustomerName() != CTM_NICHIA )
		return TRUE;
	if (pApp->GetProductLine() != _T(""))			//v4.59A34
		return TRUE;
	if ( !CMS896AStn::m_oNichiaSubSystem.IsEnabled() )
		return TRUE;
	if ( !CMS896AStn::m_oNichiaSubSystem.m_bEnableArrCodeFcn )
	{
		CMSLogFileUtility::Instance()->MS_LogCtmOperation("LoadMap: ARR-Code fcn disabled - " + GetMapFileName());
		return TRUE;
	}
	if ( !CMS896AStn::m_WaferMapWrapper.IsMapValid() )
		return FALSE;


	//Retrieve Map arrangement code from header
	CString szMapArrCodeInfo, szArrCode;
	szMapArrCodeInfo.Format("%s", ucaMapArrCode);
	CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(szMapArrCodeInfo, szArrCode);
	CMS896AStn::m_oNichiaSubSystem.UpdateMapArrCode(szArrCode);

	//Get gradelist from current map file
	CUIntArray aulAvailableGradeList;
	m_WaferMapWrapper.GetAvailableGradeList(aulAvailableGradeList);
	if (aulAvailableGradeList.GetSize() <= 0)
		return FALSE;


	CString szLog;
	UCHAR ucUserGrade	= 0;
	ULONG ulBlkID		= 0;
	LONG lBinArrCode	= 0;
	LONG lMapArrCode	= 0;
	BOOL bNeedUpdate	= FALSE;
	BOOL bStatus		= TRUE;
	ULONG ulDieCount	= 0;

	//ArrCode parameters retrieval
	DOUBLE dAreaXInUm=0, dAreaYInUm=0;
	UCHAR ucStartPos=0, ucType=0;
	ULONG ulMaxLoad=0, ulBinPitchX=0, ulBinPitchY=0;
	BOOL bReturnTravel = FALSE;
	BOOL bIsUpate = FALSE;


	//Check which die-grades are available in map, then 
	//	perform arr-code checking on those grades;
	for (INT i=0; i<aulAvailableGradeList.GetSize(); i++)
	{
		ucUserGrade = aulAvailableGradeList.GetAt(i) - CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
		ulBlkID = (ULONG) ucUserGrade;
		bNeedUpdate = FALSE;

		ulDieCount = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkID);
		bStatus = CMS896AStn::m_oNichiaSubSystem.CompareMapAndBinArrCode(ulBlkID, lBinArrCode, lMapArrCode, bNeedUpdate);

		if (ulDieCount == 0)
		{
			szLog.Format("Map ARRCODE applied due to 0 die-count: Bin #%lu, Status=%d, BinCode=%ld, MapCode=%ld, Update=%d, DieCount=%lu", 
				ulBlkID, bStatus, lBinArrCode, lMapArrCode, bNeedUpdate, ulDieCount);
			CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);

			//Apply Arrangement Code on this bin frame
			CMS896AStn::m_oNichiaSubSystem.GetArrCode(ulBlkID, TRUE, ucType,
													  dAreaXInUm, dAreaYInUm, 
													  ucStartPos, ulMaxLoad,  
													  ulBinPitchX, ulBinPitchY, bReturnTravel);
			BOOL bStatus = SetupBinBlkForArrCode(ulBlkID, ucType,
													  dAreaXInUm, dAreaYInUm, 
													  ucStartPos, ulMaxLoad,  
													  ulBinPitchX, ulBinPitchY, bReturnTravel);
			if (!bStatus)
			{
				szLog.Format("ERROR: Bin #%d ARRCODE update fails - BinCode = %ld, MapCode = %ld",
					ulBlkID, lBinArrCode, lMapArrCode);
				
				CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
				SetErrorMessage(szLog);
				SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szLog, "CLOSE");
				m_WaferMapWrapper.InitMap();
				CMS896AApp::m_bMapLoadingAbort = TRUE;	//a54321234
				return FALSE;
			}

			bIsUpate = TRUE;
			CMS896AStn::m_oNichiaSubSystem.UpdateBinArrCode(ulBlkID, lMapArrCode);
		}
		else
		{
			if (bStatus)
			{
				szLog.Format("Bin #%d ARRCODE is matched & OK - BinCode = %ld; MapCode = %ld",
					ulBlkID, lBinArrCode, lMapArrCode);
				CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
			}
			else
			{
				szLog.Format("ERROR: Bin #%d ARRCODE is not matched - BinCode = %ld; MapCode = %ld",
					ulBlkID, lBinArrCode, lMapArrCode);

				CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
				SetErrorMessage(szLog);
				SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szLog, "CLOSE");
				m_WaferMapWrapper.InitMap();
				CMS896AApp::m_bMapLoadingAbort = TRUE;	//a54321234
				return FALSE;
			}
		}
	}

	if (bIsUpate)
		m_oBinBlkMain.SaveAllBinMapData();
	return TRUE;
}

LONG CBinTable::ValidateNichiaArrCodeInMap(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

//AfxMessageBox("BT: ValidateNichiaArrCodeInMap ....", MB_SYSTEMMODAL);
	bReturn = CheckNichiaArrCodeInMap();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinTable::CalculateBinCapacityForArrCode(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	ULONG lCapacity = 0;
	UCHAR ucIndex = 0;
	svMsg.GetMsg(sizeof(UCHAR), &ucIndex);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetCustomerName() != CTM_NICHIA) ||
		 !CMS896AStn::m_oNichiaSubSystem.IsEnabled() ||
		 !CMS896AStn::m_oNichiaSubSystem.m_bEnableArrCodeFcn )
	{
		svMsg.InitMessage(sizeof(ULONG), &lCapacity);
		return 1;
	}

	CString szLog;
	DOUBLE dAreaXInUm=0, dAreaYInUm=0;
	UCHAR ucStartPos=0, ucType=0;
	ULONG ulMaxLoad=0, ulBinPitchX=0, ulBinPitchY=0;
	BOOL bReturnTravel = FALSE;

	//Apply Arrangement Code on this bin frame
	CMS896AStn::m_oNichiaSubSystem.GetArrCodeByIndex(ucIndex, ucType,
												dAreaXInUm, dAreaYInUm, 
												ucStartPos, ulMaxLoad,  
												ulBinPitchX, ulBinPitchY, bReturnTravel);

	//Use bin 175 for this calculation purpose only!
	BOOL bStatus = SetupBinBlkForArrCode(101, ucType,
												dAreaXInUm, dAreaYInUm, 
												ucStartPos, ulMaxLoad,  
												ulBinPitchX, ulBinPitchY, bReturnTravel);

	if (!bStatus)
	{
		szLog = "ERROR: Bin #101 ARRCODE update fails in CalculateBinCapacityForArrCode";		
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
		SetErrorMessage(szLog);
		SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szLog, "CLOSE");
		svMsg.InitMessage(sizeof(ULONG), &lCapacity);
		return FALSE;
	}

	UCHAR ucGrade = m_oBinBlkMain.GrabGrade(101);
	lCapacity = m_oBinBlkMain.GrabInputCount(ucGrade);

	szLog.Format("ARRCODE #%d updated (Bin #101) with max capacity = %ld", ucIndex, lCapacity);
	CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);

	svMsg.InitMessage(sizeof(ULONG), &lCapacity);
	return 1;
}

LONG CBinTable::DrawBinMap(IPC_CServiceMessage& svMsg)		//Nichia//v4.43T7
{
	BOOL bReturn = TRUE;
	ULONG ulBlkId = 0;
	svMsg.GetMsg(sizeof(ULONG), &ulBlkId);
	CString szMsg;

	if (ulBlkId > 0)
	{
		szMsg.Format("Draw BINMAP for Blk #%lu ...", ulBlkId);
		HmiMessage(szMsg);
		m_oBinBlkMain.DrawBinMap(ulBlkId);
	}

	HmiMessage("Draw BINMAP is done.");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBinTable::UpdateInputCountFromSetupFile(CInputCountSetupFile& oCountSetupFile)
{
	LONG lInputCount = 0;

	if (m_oBinBlkMain.IsBondingInProgress() == TRUE)
	{	
		SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
		return FALSE;
	}

	// disable all grades
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	for (INT i=1; i<=(INT)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		m_oBinBlkMain.SetGradeIsAssigned(m_oBinBlkMain.GrabGrade(i), FALSE, pBTfile);
	}
	CMSFileUtility::Instance()->SaveBTConfig();

	for (INT i=1; i<=(INT)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (oCountSetupFile.GetInputCount(m_oBinBlkMain.GrabGrade(i), lInputCount) == TRUE)
		{
			if (lInputCount > (LONG)m_oBinBlkMain.GrabGradeCapacity((UCHAR)m_oBinBlkMain.GrabGrade(i)))
			{
				return FALSE;
			}
		}
	}

	// update the input count
	pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	for (INT i=1; i<=(INT)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (oCountSetupFile.GetInputCount(m_oBinBlkMain.GrabGrade(i), lInputCount) == TRUE)
		{
			m_oBinBlkMain.SetGradeIsAssigned(m_oBinBlkMain.GrabGrade(i), TRUE, pBTfile);	//v4.52A8

			m_oBinBlkMain.UpdateGradeInputCount(m_oBinBlkMain.GrabGrade(i), lInputCount);
		}
	}
	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}

BOOL CBinTable::UpdateWaferMapHeader()
{
	//if (CMS896AStn::m_bUpdateWaferMapHeader == FALSE)
	//{
	//	return TRUE;
	//}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return FALSE;
	}
	if (pApp->GetProductLine() != _T(""))	//v4.59A34
	{
		return FALSE;
	}


	CString szPickedDieCountList, szPicked;
	ULONG ulLeft = 0, ulPick =0, ulDieTotal = 0;
	CUIntArray aulAllGradeList;

	CString szKey;
	CString szSortStartDateTime;
	CTime curTime = CTime::GetCurrentTime();
	CString szCurDateTime = curTime.Format("%Y/%m/%d %H:%M:%S");
	CString szMachineNo = GetMachineNo();			//v4.40T5
	CString szSortStartDate = (*m_psmfSRam)["WaferTable"]["Map Start Date"];
	szSortStartDate.Replace('-', '/');
	CString szSortStartTime = (*m_psmfSRam)["WaferTable"]["Map Start Time"];
	szSortStartDateTime = szSortStartDate + " " + szSortStartTime;

	szSortStartDateTime = CMS896AStn::m_oNichiaSubSystem.GetMachineLog_Time(8);	//v4.22T6
	
	// Machine No
	szKey.Format("%s", ucaMapHeaderMachineNo);
	CMS896AStn::m_WaferMapWrapper.GetReader()->MapData(szKey, szMachineNo);

	// set the sort date
	szKey.Format("%s", ucaMapHeaderSortDate);
	CMS896AStn::m_WaferMapWrapper.GetReader()->MapData(szKey, szSortStartDate);

	// set the sort date time
	szKey.Format("%s", ucaMapHeaderSortTime);
	CMS896AStn::m_WaferMapWrapper.GetReader()->MapData(szKey, szSortStartDateTime);

	// set the sort end date time
	szKey.Format("%s", ucaMapHeaderEndTime);
	CMS896AStn::m_WaferMapWrapper.GetReader()->MapData(szKey, szCurDateTime);

	// write the picked die counts of each grade (1-100)	//v4.40T5
	szPickedDieCountList = "";
	szPicked = "";
	CMS896AStn::m_WaferMapWrapper.GetAvailableGradeList(aulAllGradeList);
/*
	for (int m=0; m<aulAllGradeList.GetSize(); m++)
	{
		CMS896AStn::m_WaferMapWrapper.GetStatistics(aulAllGradeList.GetAt(m), ulLeft, ulPick, ulDieTotal);
		szPicked.Format("%d", ulPick);
		if (m==0)
		{
			szPickedDieCountList = szPicked;
		}
		else
		{
			szPickedDieCountList = szPickedDieCountList + "," + szPicked;
		}
	}
*/
	LONG lGrade = 0;
	LONG lCurrGrade = 0;
	BOOL bFound = FALSE;

	for (int i=1; i<=100; i++)
	{
		bFound = FALSE;
		lGrade = i + m_WaferMapWrapper.GetGradeOffset();
		
		for (int m=0; m<aulAllGradeList.GetSize(); m++)
		{
			lCurrGrade = aulAllGradeList.GetAt(m);
			if (lCurrGrade == lGrade)
			{
				CMS896AStn::m_WaferMapWrapper.GetStatistics(aulAllGradeList.GetAt(m), ulLeft, ulPick, ulDieTotal);
				szPicked.Format("%d", ulPick);
				bFound = TRUE;
				break;
			}
		}

		if (!bFound)
		{
			szPicked = "0";
		}
		if (szPickedDieCountList.GetLength() > 0)
			szPickedDieCountList = szPickedDieCountList + "," + szPicked;
		else
			szPickedDieCountList = szPicked;
	}
	szKey.Format("%s", ucaMapHeaderDieCountAfterSorting);
	CMS896AStn::m_WaferMapWrapper.GetReader()->MapData(szKey, szPickedDieCountList);
	

	//v4.40T5
	//UPDate Raw Material ID list in backup RPT map
	szKey.Format("%s", ucaMapHeaderRawMaterialNo);
	CString szRawMaterialID;
	szRawMaterialID = CMS896AStn::m_oNichiaSubSystem.m_szRawMaterialList;
	CMS896AStn::m_WaferMapWrapper.GetReader()->MapData(szKey, szRawMaterialID);

	return TRUE;
}

VOID CBinTable::SetBT1FrameLevel(BOOL bSet)		//v4.42T7
{
	if (!m_fHardware)
		return;
	if (m_bDisableBL)		//v3.60
		return;
	//CMS896AStn::MotionSetOutputBit("obFrameLevel", bSet);
}

BOOL CBinTable::IsBT1FrameLevel(VOID)
{
	CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
	if (pBinLoader)
	{
		return pBinLoader->IsFrameLevel();
	}
	return FALSE;
}


BOOL CBinTable::CreateCreeStatisticReport(LONG lFrequency)	//v4.49A4
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_CREE)
	{
		return TRUE;
	}

	m_lCreeStatisticsReportCounter++;
	if (m_lCreeStatisticsReportCounter < lFrequency)
	{
		return FALSE;
	}

	m_lCreeStatisticsReportCounter = 0;

	CTime tTime;
	tTime = CTime::GetCurrentTime();
	int nHour = tTime.GetHour();
	if (nHour == m_lCreeLastGenReportHour)
	{
		return FALSE;
	}

	m_lCreeLastGenReportHour = nHour;

	CString szMsg;
	CString szTime2 = tTime.Format("%H:%M:%S");
	szMsg.Format("CREE: create Statistics Report at: %s, (Time = %s), Hr = %d, Freq = %ld", 
				m_szOutputFilePath2, szTime2, m_lCreeLastGenReportHour, lFrequency);  
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	if (m_szOutputFilePath2.GetLength() == 0)
	{
CMSLogFileUtility::Instance()->MS_LogOperation("Error = 1");
		return FALSE;
	}
	if (!IsPathExist(m_szOutputFilePath2))
	{
CMSLogFileUtility::Instance()->MS_LogOperation("Error = 2");
		return FALSE;
	}

	CString szFileName = m_szOutputFilePath2 + "\\Statistics.txt";
	if (_access(szFileName, 0) != -1)
	{
		DeleteFile(szFileName);
	}

	CStdioFile oFile;
	BOOL bOpen = oFile.Open(szFileName, CFile::modeCreate|CFile::modeReadWrite|CFile::typeText);
	if (!bOpen)
	{
CMSLogFileUtility::Instance()->MS_LogOperation("Error = 3");
		return FALSE;
	}

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	if (pUtl->LoadConfigBLBarcode() == FALSE)
	{
CMSLogFileUtility::Instance()->MS_LogOperation("Error = 4");
		oFile.Close();
		return FALSE;
	}
	
	CStringMapFile *pSmfBLBC;
	pSmfBLBC = pUtl->GetConfigFileBLBarcode();
	if (pSmfBLBC == NULL)
	{
CMSLogFileUtility::Instance()->MS_LogOperation("Error = 5");
		oFile.Close();
		return FALSE;
	}

	//Machine No
	CString szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
	oFile.WriteString("Machine No: " + szMachineNo + "\n");

	//Date:
	CString szDate = tTime.Format("%Y-%m-%d\n");
	oFile.WriteString("Date: " + szDate);

	//Time:
	CString szTime = tTime.Format("%H:%M:%S\n");
	oFile.WriteString("Time: " + szTime);

	//Package File Name: 
	//CString szPkgFile = (*m_psmfSRam)["MS896A"]["PKG Filename Display"]
	CString szPkgFile = (*m_psmfSRam)["MS896A"]["PKG Filename"];
	oFile.WriteString("Package File Name: " + szPkgFile + "\n\n");

	//Statistics Data
	CString szLine;
	CString szBarCode;
	CString szGrade;
	CString szNoOfSortedDie;
	ULONG ulTotalSortedFromAllGrades = 0, ulNoOfSortedDie = 0;
	UCHAR ucGrade = 0;
	LONG lBlkNo = 0;


	for (INT i = 1; i <= (LONG)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		ulTotalSortedFromAllGrades += m_oBinBlkMain.GrabNVNoOfBondedDie(i);
	}

	//Total Sorted:
	CString szTotal = "";
	szTotal.Format("%lu", ulTotalSortedFromAllGrades);
	oFile.WriteString("Total Sorted: " + szTotal + "\n\n");

	//Barcode	Grade	Sorted
	oFile.WriteString("Barcode\tGrade\tSorted\n");

	for (INT i = 1; i <= (LONG)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		lBlkNo = i;
		szBarCode = (*pSmfBLBC)[lBlkNo]["Barcode"];

		//Grade
		ucGrade = m_oBinBlkMain.GrabGrade(i);
		szGrade.Format("%d", ucGrade);

		//Sorted
		//ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfSortedDie(ucGrade);
		ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(i);
		szNoOfSortedDie.Format("%lu", ulNoOfSortedDie);
		
		if (szBarCode.GetLength() > 0)
			szLine = szBarCode + "\t" + szGrade + "\t" + szNoOfSortedDie;
		else
			szLine = "           \t" + szGrade + "\t" + szNoOfSortedDie;
		oFile.WriteString(szLine + "\n");
	}

	pUtl->CloseConfigBLBarcode();
	oFile.Close();
	CMSLogFileUtility::Instance()->MS_LogOperation("CREE: Statistic Report is created OK");
	return TRUE;
}



//4.53D26 Compare Pkg fuc
BOOL CBinTable::CompareWithPkgName(CString szName)
{
	if(CMS896AStn::m_bOpenMatchPkgNameCheck == FALSE)
	{
		return TRUE;
	}
	
	CString szContent;
	CString szMsg;

	CString szCurrentPkgName = (*m_psmfSRam)["MS896A"]["PKG Filename"];


	szMsg.Format("Input Name:%s \n Current Pkg Name:%s",szName, szCurrentPkgName );
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	//if(szName != szCurrentPkgName)
	if( szCurrentPkgName.Find(szName) == -1)
	{
		HmiMessage_Red_Back(szMsg, "Fail Comparing Name"); 
		SetErrorMessage("ERROR: fail to comparing name -- " + szMsg);
		SetStatusMessage("ERROR: fail to comparing name -- " + szMsg);
		return FALSE;

	}
	return TRUE;
}


//for Smart Inline system
//All machine share the same folder and save the tempfile for WIP when this WIP frame was taken out by RGV
CString CBinTable::GetSmartInlineShareFolder()
{
	return m_szOutputFilePath2;
}


CString CBinTable::GetSlotTempFileName(const LONG lBlkId)
{
	CString szBlkIdForFilename;
	CString szTempFilename;

	//File Name
	szBlkIdForFilename.Format("%d", lBlkId);
	if (lBlkId < 10)
	{
		szBlkIdForFilename = "0" + szBlkIdForFilename;
	}

	szTempFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkIdForFilename + "TempFile.csv";

	return szTempFilename;
}

LONG CBinTable::GetNoOfSortedDie(UCHAR lBlkId)
{
	LONG lNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfSortedDie((UCHAR)lBlkId);

	return lNoOfSortedDie;
}

CString CBinTable::GetCustomOutputFileName()
{
	return m_oBinBlkMain.GetCustomOutputFileName();
}

CString CBinTable::GetBinLostSummaryPath()
{
	return m_szBinLotSummaryPath;
}

BOOL CBinTable::CheckIfChangeFrameForSubBin(ULONG ulLast, ULONG ulCurrent)
{
	if (CMS896AApp::m_bEnableSubBin != TRUE)
		return TRUE;

	if (!IsMS90Sorting2ndPart() && ulLast == 0)
	{
		return TRUE;
	}

	if  ((ulLast != ulCurrent) && (ulLast != 0))
	{
		CMSLogFileUtility::Instance()->BT_TableIndexLog("Sub Bin Change Grade");
		(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"] = TRUE;
	}
	else
	{
		return FALSE;
	}
	ULONG ulLastBlk    = 0;
	ULONG ulCurrentBlk = 0;
	for (int k = 1; k <= m_nNoOfSubBlk; k++)
	{
		for (int j = 1; j <= m_nNoOfSubGrade; j++)
		{
			if (ulLast == m_nSubGrade[k][j])
				ulLastBlk = k;
		}
	}

	for (int k = 1; k <= m_nNoOfSubBlk; k++)
	{
		for (int j = 1; j <= m_nNoOfSubGrade; j++)
		{
			if (ulCurrent == m_nSubGrade[k][j])
				ulCurrentBlk = k;
		}
	}

	CString szMsg;
	szMsg.Format("CheckIfChangeFrameForSubBin,Grade(%d,%d),Blk(%d,%d)",ulLast,ulCurrent,ulLastBlk,ulCurrentBlk);
	DisplaySequence(szMsg);
	if (ulLastBlk == ulCurrentBlk)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CBinTable::CheckIfOverlapForSubBin(ULONG x1, ULONG y1, ULONG x2, ULONG y2, ULONG x3, ULONG y3, ULONG x4, ULONG y4)
{

	//x1y1 is the ul of block 1, x2y2 is the lr of block 1
	//x3y3 is the ul of block 2, x4y4 is the lr of block 2
	if (x1 == 0 || x2 == 0 || x3 == 0 || x4 == 0 ||y1 == 0 || y2 == 0 || y3 == 0 || y4 == 0)
	{
		return TRUE;
	}

	if (x3<=x2 && x2<=x4 && y3<=y2 && y2<=y4)
	{
		HmiMessage("false1");
		return FALSE;
	}

	if (x3<=x1 && x1<=x4 && y3<=y2 && y2<=y4)
	{
		HmiMessage("false2");
		return FALSE;
	}

	if (x3<=x2 && x2<=x4 && y3<=y1 && y1<=y4)
	{
		HmiMessage("false3");
		return FALSE;
	}

	if (x3<=x1 && x1<=x4 && y3<=y1 && y1<=y4)
	{
		HmiMessage("false4");
		return FALSE;
	}

	if (x1<x3 && x4<=x2 && y1<=y3 && y4<=y2)
	{
		HmiMessage("false5");
		return FALSE;
	}

	return TRUE;
}


LONG CBinTable::GetBTCurrentBlock()
{
	LONG lBinBlk = 0;
	CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
	if (pBinLoader != NULL)
	{
		lBinBlk = pBinLoader->GetBTCurrentBlock();
	}

	return lBinBlk;
}


LONG CBinTable::GetBlock(UCHAR ucGrade)
{
	return m_oBinBlkMain.GetBlock(ucGrade);
}

BOOL CBinTable::ClearBPRAccEmptyCounter(ULONG *ulBinBlk)
{
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(ULONG), &ulBinBlk);
	INT nConvID = m_comClient.SendRequest("BondPrStn", "ClearAccEmptyCounter", stMsg);
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

	return TRUE;
}


LONG CBinTable::GetBHZ1BondPosOffsetX()
{
	return m_lBHZ1BondPosOffsetX;
}


LONG CBinTable::GetBHZ1BondPosOffsetY()
{
	return m_lBHZ1BondPosOffsetY;
}


LONG CBinTable::GetBHZ2BondPosOffsetX()
{
	return m_lBHZ2BondPosOffsetX;
}


LONG CBinTable::GetBHZ2BondPosOffsetY()
{
	return m_lBHZ2BondPosOffsetY;
}


VOID CBinTable::SetAOTClearBinCounterDefaultSetting()
{
	m_szClrBinCntMode	= "Single Block";
	m_bIfIsClrAllBinCtr = FALSE;
	m_bIfGenOutputFile	= TRUE;
}


BOOL CBinTable::LoadPackageMsdBTData(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadPackageDataConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetPackageDataConfig();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	m_lACCCycleCount = (*psmf)[BT_DATA][BT_AUTO_CC_CYCLE_COUNT];

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to load BT data from PackageData.msd");

	// close config file
    pUtl->ClosePackageDataConfig();

	return TRUE;
}

BOOL CBinTable::SavePackageMsdBTData(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadPackageDataConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetPackageDataConfig();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	(*psmf)[BT_DATA][BT_AUTO_CC_CYCLE_COUNT] = m_lACCCycleCount;

	pUtl->UpdatePackageDataConfig();	

	// close config file
    pUtl->ClosePackageDataConfig();

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to save BT data from PackageData.msd");

	return TRUE;
}


BOOL CBinTable::UpdateBTPackageList(VOID)
{
	CStdioFile fTemp;
	CString szContent, szText, szTemp;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\BTPackageList.csv"), CFile::modeCreate| CFile::modeReadWrite))
	{
		return FALSE;
	}

	szText = BT_AUTO_CC_CYCLE_COUNT;
	szText = szText + ",";
	szTemp.Format("%d,", m_lACCCycleCount);
	szContent = szContent + szText + szTemp;

	fTemp.WriteString(szContent);
	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to update BT package data to BRPackageList.csv");

	fTemp.Close();

	return TRUE;
}

BOOL CBinTable::CheckIfGenerateWaferEnd(CString szWaferId, LONG lPickCount, LONG lMapTotal)
{
	CString szFileName  = gszUSER_DIRECTORY + "\\History\\WaferEndInfo\\" + szWaferId + ".txt";
	BOOL bReturn = TRUE;
	if (_access(szFileName,0) != -1)
	{
		CStdioFile cfRead;
		CString szRead, szTemp;
		LONG lCount1, lCount2;
		BOOL bCheck1 = FALSE, bCheck2 = FALSE;
		if (!cfRead.Open(_T(szFileName), CFile::modeRead))
		{
			return TRUE;
		}

		while (cfRead.ReadString(szRead))
		{
			int n = szRead.Find("#P,");
			if (n >= 0)
			{
				szTemp = szRead.Mid(n + 3);
				n = szTemp.Find(",");
				if (n > 0)
				{
					szTemp = szTemp.Left(n);
					lCount1 = atoi(LPCTSTR(szTemp));
					if (lCount1 == lPickCount)
					{
						bCheck1 = TRUE;
					}
				}
			}

			n = szRead.Find("#M,");
			if (n > 0)
			{
				szTemp = szRead.Mid(n + 3);
				n = szTemp.Find(",");
				if (n > 0)
				{
					szTemp = szTemp.Left(n);
					lCount2 = atoi(LPCTSTR(szTemp));
					if (lCount2 == lMapTotal)
					{
						bCheck2 = TRUE;
					}
				}
			}

			if (bCheck1 == TRUE && bCheck2 == TRUE)
			{
				bReturn = FALSE;
				break;
			}
		}
		cfRead.Close();
	}
	return bReturn;
}

DOUBLE CBinTable::GetWaferYield(CString szMsg)
{
	CString szWaferId = "";
	CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferId);
	szWaferId.Remove('\t');
	szWaferId.Remove(',');
	szWaferId.Remove('\"');

	CString szFileName  = gszUSER_DIRECTORY + "\\History\\WaferEndInfo\\" + szWaferId + ".txt";
	BOOL bReturn = FALSE;
	DOUBLE dYield = 0;
	if (_access(szFileName,0) != -1)
	{
		CStdioFile cfRead;
		CString szRead, szTemp;
		LONG lTotalPick = 0, lMaxMapCount = 0, lTemp = 0;
		if (!cfRead.Open(_T(szFileName), CFile::modeRead))
		{
			return TRUE;
		}

		while (cfRead.ReadString(szRead))
		{
			int n = szRead.Find("#P,");
			lTemp = 0;
			if (n >= 0)
			{
				szTemp = szRead.Mid(n + 3);
				n = szTemp.Find(",");
				if (n > 0)
				{
					szTemp = szTemp.Left(n);
					lTemp = atoi(LPCTSTR(szTemp));
				}
			}
			lTotalPick = lTotalPick + lTemp;

			n = szRead.Find("#M,");
			lTemp = 0;
			if (n > 0)
			{
				szTemp = szRead.Mid(n + 3);
				n = szTemp.Find(",");
				if (n > 0)
				{
					szTemp = szTemp.Left(n);
					lTemp = atoi(LPCTSTR(szTemp));
				}
				if (lTemp > lMaxMapCount)
				{
					lMaxMapCount = lTemp;
				}
			}
		}
		cfRead.Close();

		dYield = (DOUBLE)lTotalPick/(DOUBLE)lMaxMapCount * 100;
	}
	CString szLog;
	szLog.Format("Check Yield,%f,%s",dYield,szMsg);
	SaveWaferEndInfo(szLog, szWaferId);

	return dYield;
}


BOOL CBinTable::CalculateRotateBinTheta(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree, LONG lCalibX, LONG lCalibY)
{
	DOUBLE	dRadAngle = 0.0;
	DOUBLE	dX, dY, dRadius, dNewTheta = 0, dCurTheta = 0;	//Klocwork

	//Convert PR die angle to Radian
	dCurTheta = (dDegree * PI) / 180.0;
	
	dX = (DOUBLE)(*lX - lCalibX);
	dY = (DOUBLE)(*lY - lCalibY);

	dRadius = sqrt((dX * dX) + (dY * dY));
	
	if (dRadius == 0.0)
	{
		dX = 0.0;
	}
	else
	{
        dX = fabs(dY / dRadius); //sin
	}

	if (*lX <= lCalibX)
	{
		if (*lY <= lCalibY)
		{
			dNewTheta = asin(dX);
		}
		else
		{
			dNewTheta = 2.0*PI - asin(dX);
		}
	}
	else
	{
		if (*lY <= lCalibY)
		{
			dNewTheta = PI - asin(dX);
		}
		else
		{
			dNewTheta = PI + asin(dX);
		}
	}

    dNewTheta -= dCurTheta;

	if (dNewTheta < 0.0)
	{
		dNewTheta = (2.0 * PI) + dNewTheta;
	}
	else if(dNewTheta >= 2.0*PI)
	{
		dNewTheta -= (2.0 * PI);
	}

	if((dNewTheta >= 0.0) && (dNewTheta <= PI/2.0))
	{
		dX = dRadius * cos(dNewTheta);
		dY = dRadius * sin(dNewTheta);
	}
	else if((dNewTheta > PI/2.0) && (dNewTheta <= PI))
	{
		dX = -dRadius * cos(PI - dNewTheta);
		dY =  dRadius * sin(PI - dNewTheta);
	}
    else if((dNewTheta > PI) && (dNewTheta <= PI*3.0/2.0))
	{
		dX = -dRadius * cos(dNewTheta - PI);
		dY = -dRadius * sin(dNewTheta - PI);
	}
    else if((dNewTheta > PI*3.0/2.0) && (dNewTheta <= 2.0*PI))
	{
		dX =  dRadius * cos(2.0*PI - dNewTheta);
		dY = -dRadius * sin(2.0*PI - dNewTheta);
	}

	*lX		= -(LONG)(dX) + lCalibX;
	*lY		= -(LONG)(dY) + lCalibY;
	*lTheta	= m_lThetaMotorDirection * (LONG)(dDegree/m_dThetaRes);
	
	return 1;
}


BOOL CBinTable::BinTableThetaCompensation(DOUBLE dDegree)
{
	//Make sure BT stop
	X_Sync();
	Y_Sync();
	T_Sync();

	GetEncoderValue();
	LONG lX = m_lEnc_X, lY = m_lEnc_Y, lTheta = m_lEnc_T;
	dDegree = -1 * dDegree;

	CString szLog;
	szLog.Format("BT ThetaOpCompensation Original XYTD,%d,%d,%d,%f", lX, lY, m_lEnc_T, dDegree);
	CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szLog);

	CalculateRotateBinTheta(&lX, &lY, &lTheta, dDegree, m_lBinCalibX, m_lBinCalibY);

	XY_MoveTo(lX, lY, SFM_NOWAIT);
	if (lTheta != 0)
	{
		T_MoveTo(lTheta, SFM_NOWAIT);
	}

	X_Sync();
	Y_Sync();
	if (lTheta != 0)
	{
		T_Sync();
	}

	(*m_psmfSRam)["BinTable"]["ThetaCorrection"] = 0;

	szLog.Format("BT ThetaOpCompensation New XYT,%d,%d,%d", m_lX, m_lY, lTheta);
	CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szLog);

	return TRUE;
}