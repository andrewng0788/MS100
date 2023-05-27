/////////////////////////////////////////////////////////////////
// WPR_Common.cpp : Common functions of the CWaferPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tuesday, November 9, 2004
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
#include "MS896A.h"
#include "WaferPr.h"
#include "FileUtil.h"
#include "GallerySearchController.h"
#include "BondPr.h"
#include "BondHead.h"
#include "WaferTable.h"
#include "BinLoader.h"
#include "NGGrade.h"
#include "PRFailureCaseLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CWaferPr::InitVariable(VOID)
{
	m_pDPR		= NULL;
	m_bPRInit	= FALSE;
	m_bInitPreScanWaferCamFailed = FALSE;

	m_dEjtXYRes = GetChannelResolution(MS896A_CFG_CH_EJECTOR_X) * 1000.0;// M/Count ==> um/count
	m_dWTXYRes = GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * 1000.0;// M/Count ==> um/count
	m_dBTResolution_UM_CNT	= GetChannelResolution(MS896A_CFG_CH_BINTABLE_X) * 1000; // M/Count ==> um/count

	m_stIMEffView.coCorner1.x = PR_MAX_WIN_ULC_X;
	m_stIMEffView.coCorner1.y = PR_MAX_WIN_ULC_Y;
	m_stIMEffView.coCorner2.x = PR_MAX_WIN_LRC_X;
	m_stIMEffView.coCorner2.y = PR_MAX_WIN_LRC_Y;
	m_stWprRoiEffView = m_stIMEffView;	//	init variable

	m_bAutoBondMode = FALSE;
	m_bUseHWTrigger = FALSE;

	m_pIMFunc	= new CIMEvent;		//v2.83T2
	m_pIMFunc->SetWaferPr(this);

	m_lLetBPRGrabFirstImage		= FALSE;

	m_pAdapterWaferFunc = new CAdaptWaferWrapper();
	m_pAdapterWaferFunc->SetWaferPr(this);

	//Init data need to save in file
	m_lPrCenterX				= PR_DEF_CENTRE_X;
	m_lPrCenterY				= PR_DEF_CENTRE_Y;
	m_lPrCenterXInPixel			= 0;
	m_lPrCenterYInPixel			= 0;
	m_lWaferToBondPrCenterOffsetX	= 0;	//v4.08
	m_lWaferToBondPrCenterOffsetY	= 0;	//v4.08

	m_bSrchEnableLookForward_Backup = FALSE;
	m_ulMaxNoDieSkipCount_Backup = 0;

	m_bEnableWTIndexTest		= FALSE;
	m_stWTIndexTestLFEnc.lX		= 0;
	m_stWTIndexTestLFEnc.lY		= 0;

	m_bF1SearchDieWithRoatation = FALSE;
	m_bLearnLookupCamera		= FALSE; //4.52D17
	m_bDieIsLearnt				= FALSE;
	m_bDieCalibrated			= FALSE;
	m_bPitchLearnt				= FALSE;
	m_bGetBadCutReply			= FALSE;
	m_bSrchEnableLookForward	= TRUE;
	m_bSrchEnableBackupAlign	= TRUE;
	m_bThetaCorrection			= FALSE;
	m_bBinTableThetaCorrection	= FALSE;
	m_bConfirmSearch			= FALSE;
	m_bRefDieCheck				= TRUE;
	m_bAllRefDieCheck			= FALSE;
	m_bReverseDieStatus			= FALSE;
	m_bPickAlignableDie			= FALSE;
	m_lRefDieFaceValueNo		= -2;		//v4.48A26	//Avago
	m_dMaxDieAngle				= 6.0;	
	m_dMinDieAngle				= 3.0;
	m_dAcceptDieAngle			= 3.0;
	m_dCalibX					= 0.0;	
	m_dCalibY					= 0.0;		
	m_dCalibXY					= 0.0;
	m_dCalibYX					= 0.0;
	m_dFFCalibX					= 0.0;	
	m_dFFCalibY					= 0.0;		
	m_dFFCalibXY				= 0.0;
	m_dFFCalibYX				= 0.0;
	m_dEjtCalibX				= 0.0;	
	m_dEjtCalibY				= 0.0;		
	m_dEjtCalibXY				= 0.0;
	m_dEjtCalibYX				= 0.0;
	m_dEjtFFCalibX				= 0.0;	
	m_dEjtFFCalibY				= 0.0;		
	m_dEjtFFCalibXY				= 0.0;
	m_dEjtFFCalibYX				= 0.0;
	m_siDiePitchXX				= 0;		
	m_siDiePitchXY				= 0;				
	m_siDiePitchYX				= 0;				
	m_siDiePitchYY				= 0;				
	m_lEjtCollet1OffsetX		= 0;
	m_lEjtCollet1OffsetY		= 0;
	m_lEjtCollet2OffsetX		= 0;
	m_lEjtCollet2OffsetY		= 0;
	//v4.42T15
	m_lEjtCollet1OffsetX_Pixel	= 0;
	m_lEjtCollet1OffsetY_Pixel	= 0;
	m_lEjtCollet2OffsetX_Pixel	= 0;
	m_lEjtCollet2OffsetY_Pixel	= 0;
	m_lEjtCollet1CoorX			= 0;
	m_lEjtCollet1CoorY			= 0;
	m_lEjtCollet2CoorX			= 0;
	m_lEjtCollet2CoorY			= 0;
	m_dMaxEpoxySizeFactor		= 2.0;	//v4.57A1
	m_dWTXinUm					= 0.0;
	m_dWTYinUm					= 0.0;
	m_dLFSize					= 0.0;			
	m_lSrchRefDieNo				= 1;
	m_lNoLastStateFile			= 0;
	m_lLrnTotalRefDie			= 0;
	m_lAutoScreenFG				= 1;	
	m_lAutoScreenMode			= 2;	//Default is 1/16	
	m_ulMaxSkipCount			= 0;
	m_ulMaxNoDieSkipCount		= 0;
	m_ulMaxPRSkipCount			= 0;
	m_ulMaxBadCutSkipCount		= 0;
	m_ulSkipCount				= 0;
	m_ulNoDieSkipCount			= 0;
	m_ulPRSkipCount				= 0;
	m_ulDefectSkipCount			= 0;
	m_ulMaxDefectSkipCount		= 0;
	m_ulStartSkipCount			= 0;
	m_ulMaxStartSkipLimit		= 0;
	m_dWaferGoodDieRatio		= 0.0;
	m_ulWaferDefectDieCount		= 0;
	m_ulWaferMaxDefectCount		= 0;
	m_ulBadCutSkipCount			= 0;
	m_bSetAutoBondScreen		= FALSE;
	m_bCheckAllNmlDie			= FALSE;
	m_bCheckAllRefDie			= FALSE;
	m_lCurrSrchDieType			= 0;
	m_lCurrSrchDieId			= 1;
	m_lMaxSortNmlDieID			= 1;
	m_lCurrSrchRefDieId			= 1;
	m_ucGenDieShape				= WPR_RECTANGLE_DIE;
	m_ucDieShape				= WPR_RECTANGLE_DIE;
	//m_bEnableRhombusDie			= FALSE;
	m_lPrCal1stStepSize			= 30;		//v2.89T1
	m_lLearnPitchSpanX			= 1;
	m_lLearnPitchSpanY			= 1;
	m_bIMAutoLearnWafer			= FALSE;	//v3.06
	m_bEnable2Lighting			= FALSE;	//v3.44T1
	m_bRefCheckRst				= TRUE;
	m_iMapDig1					= 0;
	m_iMapDig2					= 0;
	m_iWafDig1					= 0;
	m_iWafDig2					= 0;
	m_bCompareHomeReferDieScore	= FALSE;

	// CP Die Thick
	m_dDieThicknessInput = 0.000;

	// For character Die
	m_bLrnDigit0 = TRUE;
	m_bLrnDigit1 = TRUE;
	m_bLrnDigit2 = TRUE;
	m_bLrnDigit3 = TRUE;
	m_bLrnDigit4 = TRUE;
	m_bLrnDigit5 = TRUE;
	m_bLrnDigit6 = TRUE;
	m_bLrnDigit7 = TRUE;
	m_bLrnDigit8 = TRUE;
	m_bLrnDigit9 = TRUE;
	m_bEnableOCR = FALSE;

	m_bAoiSortInspction			= TRUE;
	m_bAOINgPickPartDie			= FALSE;
	m_lNgPickDieAreaTolMin		= 0;
	m_lNgPickDieAreaTolMax		= 0;
	m_szPkgNameCurrent.Empty();
	m_szPkgNamePrevious.Empty();
	for(int i=0; i<WPR_MAX_DIE; i++)
	{
		m_saPkgRecordCurrent[i] = 0;
		m_saPkgRecordPrevious[i] = 0;
	}
	//BH Marl
	m_lBHMarkRef1X = 0;
	m_lBHMarkRef1Y = 0;
	m_lBHMarkRef2X = 0;
	m_lBHMarkRef2Y = 0;

	m_lBHColletHoleRef1X = 0;
	m_lBHColletHoleRef1Y = 0;
	m_lBHColletHoleRef2X = 0;
	m_lBHColletHoleRef2Y = 0;

	m_ulPreviousUpdateDieTypeCounterRow = 0;
	m_ulPreviousUpdateDieTypeCounterCol = 0;

	memset(&m_stLFSearchArea, PR_DEF_CENTRE_X, sizeof(m_stLFSearchArea));
	memset(&m_stSearchArea.coCorner1, PR_MAX_WIN_ULC_X, sizeof(m_stSearchArea.coCorner1));
	memset(&m_stSearchArea.coCorner2, PR_MAX_WIN_LRC_X, sizeof(m_stSearchArea.coCorner2));
	memset(&m_stLearnDieCornerPos, 0, sizeof(m_stLearnDieCornerPos));

	memset(&m_ssGenPRSrchID, 0, sizeof(m_ssGenPRSrchID));
	memset(&m_stGenDieSize, 0, sizeof(m_stGenDieSize));
	memset(&m_bGenDieLearnt, FALSE, sizeof(m_bGenDieLearnt));
	memset(&m_lGenLrnAlignAlgo, 0, sizeof(m_lGenLrnAlignAlgo));
	memset(&m_lGenLrnBackgroud, 0, sizeof(m_lGenLrnBackgroud));
	memset(&m_lGenLrnFineSrch, PR_RESOLUTION_HIGH, sizeof(m_lGenLrnFineSrch));
	memset(&m_lGenLrnInspMethod, 0, sizeof(m_lGenLrnInspMethod));
	memset(&m_lGenLrnInspRes, 0, sizeof(m_lGenLrnInspRes));
	memset(&m_lGenSrchAlignRes, 0, sizeof(m_lGenSrchAlignRes));
	memset(&m_lGenSrchDefectThres, 12, sizeof(m_lGenSrchDefectThres));
	memset(&m_lGenSrchDieAreaX, 0, sizeof(m_lGenSrchDieAreaX));
	memset(&m_lGenSrchDieAreaY, 0, sizeof(m_lGenSrchDieAreaY));
	memset(&m_lGenSrchDieScore, 0, sizeof(m_lGenSrchDieScore));
	memset(&m_lGenSrchGreyLevelDefect, 0, sizeof(m_lGenSrchGreyLevelDefect));
	memset(&m_dGenSrchSingleDefectArea, 0, sizeof(m_dGenSrchSingleDefectArea));
	memset(&m_dGenSrchTotalDefectArea, 0, sizeof(m_dGenSrchTotalDefectArea));
	memset(&m_dGenSrchChipArea, 0, sizeof(m_dGenSrchChipArea));
	memset(&m_bGenSrchEnableDefectCheck, FALSE, sizeof(m_bGenSrchEnableDefectCheck));
	memset(&m_bGenSrchEnableChipCheck, FALSE, sizeof(m_bGenSrchEnableChipCheck));

	memset(&m_stInspDetectWin, 0, sizeof(m_stInspDetectWin));
	memset(&m_stInspIgnoreWin, 0, sizeof(m_stInspIgnoreWin));
	memset(&m_stAlnDetectWin, 0, sizeof(m_stAlnDetectWin));
	memset(&m_stAlnIgnoreWin, 0, sizeof(m_stAlnIgnoreWin));

	//Init data no need to save in file
	m_stDieCenter.x			= PR_DEF_CENTRE_X;
	m_stDieCenter.y			= PR_DEF_CENTRE_Y;
	m_fDieAngle				= 0.0;
	m_bSelectDieType		= WPR_NORMAL_DIE;	
	m_bSrchDiePara			= FALSE;
	m_bCurrentCamera		= FALSE;
	m_bKeepShowBondInAuto	= FALSE;
	m_ulAutoSwitchPrCount	= 0;
	m_bPostSealCamera		= FALSE;		//v3.70T2
	m_unCurrPostSealID		= 0;
	m_bSrchEnableDefectCheck = FALSE;
	m_bSrchEnableChipCheck	= FALSE;
	m_bBadCutDetection		= FALSE;
	m_bPrCircleDetection	= FALSE;		//v2.77
	m_bAutoLearnRefDie		= FALSE;		//v2.78T2
	m_ulOcrDieOrientation	= 0;
	m_bBadcutBiDetect		= FALSE;
	m_lBadcutBiThreshold	= 0;
	m_bNormalRoughLedDie	= FALSE;

	m_lLrnAlignAlgo			= 0;
	m_lLrnBackground		= 0;
	m_bEnableLrnAlignAlgo	= TRUE;
	m_lLrnFineSrch			= 2;	//Default value by Matthew 20190410
	//m_lLrnFineSrch		= 1;			//v4.08			// Set MEDIUM resolution as default
	//m_lLrnFineSrch		= (LONG)PR_RESOLUTION_HIGH;	// Set high resolution as default
	m_lLrnInspMethod		= 0;
	m_lLrnInspRes			= 0;
	m_lSrchAlignRes			= 0;
	m_lSrchDefectThres		= 12;	
	m_lSrchDieAreaX			= 4;
	m_lSrchDieAreaY			= 4;
	m_dSrchDieAreaX			= GetSrchDieAreaX() * WPR_SEARCHAREA_SHIFT_FACTOR;
	m_dSrchDieAreaY			= GetSrchDieAreaY() * WPR_SEARCHAREA_SHIFT_FACTOR;
	m_lSrchDieScore			= 70;
	m_lSrchGreyLevelDefect	= 0;
	m_dSrchSingleDefectArea	= 0.0;
	m_dSrchTotalDefectArea	= 0.0;
	m_dSrchChipArea			= 0.0;

	m_bEnableLineDefect		= FALSE;
	m_dMinLineDefectLength	= 0;
	m_lMinNumOfLineDefects	= 1;

	m_bAutoEnlargeSrchWnd	= FALSE;

	m_lCurRefDieNo			= 1;
	m_lCurNormDieID			= 0;
	m_lCurNormDieSizeX		= 0;
	m_lCurNormDieSizeY		= 0;
	m_dCurNormDieSizeX		= 0;
	m_dCurNormDieSizeY		= 0;
	m_lCurBadCutSizeX		= 0;
	m_lCurBadCutSizeY		= 0;
	m_lCurPitchSizeX		= 0;
	m_lCurPitchSizeY		= 0;
	m_dCurPitchSizeX		= 0;
	m_dCurPitchSizeY		= 0;
	m_lInspDetectWinNo		= 0;
	m_lInspIgnoreWinNo		= 0;
	m_lAlnDetectWinNo		= 0;
	m_lAlnIgnoreWinNo		= 0;
	m_dCurSrchDieSizeX		= 0;
	m_dCurSrchDieSizeY		= 0;
	m_nCurrLFPosn			= 0;

	m_lJoystickLevel_BT		= 1;
	m_lJoystickLevel_WT		= 1;
	m_bUseMouse				= FALSE;
	m_ulLogPRCaseInHQ		= 0;

	m_szSrhDieResult		= "Die Type: ";
	m_szLrnDieType			= WPR_HMI_NORM_DIE;
	m_sz2DCodePrTitle		= "2D Code";
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA) )
		m_sz2DCodePrTitle	= "WL1 Cam";
	m_bDisplayDieInUM	= FALSE;
	
	m_dThetaRes				= GetChannelResolution(MS896A_CFG_CH_WAFTABLE_T);	
	m_lThetaMotorDirection	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MOTOR_DIRECTION);

	m_ulWaferCameraColor	= WPR_SELECTED_COLOR;
	m_ulBondCameraColor		= WPR_UNSELECTED_COLOR;
	m_ul2DCameraColor		= WPR_UNSELECTED_COLOR;

	m_ulBH1UplookCameraColor = WPR_UNSELECTED_COLOR;//4.52D17
	m_ulBH2UplookCameraColor = WPR_UNSELECTED_COLOR;
	m_ulPrWindowHeight		= 480;
	m_ulPrWindowWidth		= 512;
	m_bCPItemMapToTop		= FALSE;

	m_szSoftVersion			= "";
	m_szSWReleaseNo			= "";

	//m_szShowImagePath		= "";
	for (int i = 0; i < WPR_MAX_DIE; i++)
	{
		m_szShowImagePath[i] = "";
	}

	m_szShowImageType		= WPR_HMI_NORM_DIE;
	m_lShowImageNo			= 1;

	//Init Motorized Variable
	m_bAutoFocusPower		= FALSE;
	m_bAutoFocusHomeSensor	= FALSE;
	m_bAutoFocusLimitSensor	= FALSE;
	m_lEnc_AF	= 0;
	m_bAutoFocusOK		= FALSE;
	m_lAutoFocusPosition	= 0;
	m_lAutoFocusPosition2	= 0;
	m_bWaferAutoFocus	= FALSE;
	m_lAFPrDelay		= 0;
	m_lAFUpLimit		= 1000;
	m_lAFLowLimit		= 1000;
	m_lAFErrorTol		= 500;
	m_lAFMinDist		= 50;
	m_lAFStandbyOffset	= 0;
	m_lAFZPosLimit		= (30 * 100 * 96);
	m_lAFZNegLimit		= (-100 * 96);
	m_dAFrValueLimit	= 0.0;
	m_bAutoFocusDown	= FALSE;
	m_bAFDynamicAdjust		= FALSE;	//	427TX	3
	m_bAFRescanBadFrame		= FALSE;
	m_lAFDynamicAdjustStep	= 10;
	m_dAFDynamicAdjustLimit	= 0;
	m_dAFRescanBadFrameLimit = 0;

	m_bWprWithAF_Z	= FALSE;
	m_bComm_Z	= FALSE;
	m_bBackLightComm_Z	= FALSE;
	//m_pStepper_Focus_Z = NULL;
	//m_pStepper_Zoom_Z = NULL;
	m_lEnc_Focus = 0;
	m_lEnc_Zoom = 0;
	m_bFocusHome_Z = FALSE;
	m_bZoomHome_Z = FALSE;
	m_bZoomPower = FALSE;
	m_bFocusPower = FALSE;
	m_bZoomHomeSensor = FALSE;
	m_bFocusHomeSensor = FALSE;
	m_bZoomLimitSensor = FALSE;
	m_bFocusLimitSensor = FALSE;

	m_bMoveDirection = FALSE;
	m_lMotionTestDelay = 0;
	m_lMotionTestDist = 0;
	m_lBackLightElevatorMoveTestCount = 0;
	m_bBackLightElevatorMoveTest = FALSE;


	for (int i = 0; i < WPR_BADCUT_POINT; i++)
	{
		m_stBadCutPoint[i].x = PR_DEF_CENTRE_X;
		m_stBadCutPoint[i].y = PR_DEF_CENTRE_Y;
	}

	m_lWPRGenCoaxLightLevel_Backup = 0;
	m_lWPRGenSideLightLevel_Backup = 0;
	m_lWPRGenRingLightLevel_Backup = 0;
	m_lWPRGenBackLightLevel_Backup = 0;

	for (int i = 0; i < WPR_MAX_LIGHT_GROUP; i++)
	{
		m_lWPRGenCoaxLightLevel[i] = 0;
		m_lWPRGenRingLightLevel[i] = 0;
		m_lWPRGenSideLightLevel[i] = 0;
		m_lWPRGenBackLightLevel[i] = 0;

		m_lWPRLrnCoaxLightLevel[i] = 0;
		m_lWPRLrnRingLightLevel[i] = 0;
		m_lWPRLrnSideLightLevel[i] = 0;
		m_lWPRLrnBackLightLevel[i] = 0;
	}

	m_lWPRGeneral_TMP = 0;
	m_lWPRGeneral_1 = 0; // for up level
	m_lWPRGeneral_2 = 0; // for standby level
	m_lBackLightElevatorUpLevel = 0;
	m_lBackLightElevatorStandByLevel = 0;
	m_lBLZAutoFocusDelay	= 0;

	m_bWprUseBackLight = IsAOIOnlyMachine();
	m_lWPRLrnCoaxLightHmi = m_lWPRLrnCoaxLightLevel[0];
	m_lWPRLrnRingLightHmi = m_lWPRLrnRingLightLevel[0];
	m_lWPRLrnSideLightHmi = m_lWPRLrnSideLightLevel[0];


	m_lWPRLrnExposureTimeLevel	= 0;
	m_lWPRGenExposureTimeLevel	= 0;
	m_lWprLrnExposureTimeHmi	= 0;
	m_lSCMLrnCoaxLightLevel		= 0;
	m_lSCMLrnRingLightLevel		= 0;
	m_lSCMLrnSideLightLevel		= 0;
	m_lSCMLrnExposureTimeLevel	= 0;
	m_lWPRGenExposureTimeHmi	= 0;
	m_lSCMGenExposureTimeLevel	= 0;

	m_lPPRGenCoaxLightLevel	= 0;
	m_lPPRGenRingLightLevel	= 0;
	m_lPPRGenSideLightLevel	= 0;

	m_stBadCutTolerenceX	= 10;
	m_stBadCutTolerenceY	= 10;
	m_stBadCutScanWidthX	= 1;
	m_stBadCutScanWidthY	= 1;

	m_dAlignPassScore		= 0;
	m_dSingleDefectScore	= 0;
	m_dTotalDefectScore		= 0;
	m_dChipAreaScore		= 0;
	m_dCurAlignPassScore	= 0;
	m_dCurSingleDefectScore	= 0;
	m_dCurTotalDefectScore	= 0;
	m_dCurChipAreaScore		= 0;
	m_szCurPrStatus			= "";
	m_dSpSingleDefectScore	= 0;
	m_dSpTotalDefectScore	= 0;
	m_dSpChipAreaScore		= 0;

	m_lChangeColletTempCenter1X = 0;
	m_lChangeColletTempCenter1Y = 0;
	m_lChangeColletTempCenter2X = 0;
	m_lChangeColletTempCenter2Y = 0;

	m_nPolygonSize = 0;

	for (int i = 0; i < WPR_MAX_DIE_CORNER; i++)
	{
		m_stNewBadCutPoint[i].x = 0 ;
		m_stNewBadCutPoint[i].y = 0 ;
		m_stDieCoordinate[i].x = 0 ;
		m_stDieCoordinate[i].y = 0 ;
	}

	m_dSetRegionByDieSizeXCopy = 0;
	m_dSetRegionByDieSizeYCopy = 0;
	m_dSetRegionByDieSizeX	= 0;
	m_dSetRegionByDieSizeY	= 0; 

	m_bWarningNoInspection	= TRUE;		//default=TRUE to trigger the checking after starting up machine
	if (pApp->m_bMSAutoLineMode == 1)
	{
		m_bWarningNoInspection	= FALSE;	//v4.57A15	//AUTOLINE
	}

	m_bDebugVideoTest		= FALSE;

	for(int i=0; i<3; i++)
	{
		m_dRMasterDieSizeX[i]	= 0;
		m_dRMasterDieSizeY[i]	= 0;
	}
	m_dWAlignMasterPitchInMilX	= 0;	//v2.78T2
	m_dWAlignMasterPitchInMilY	= 0;	//v2.78T2
	m_lNDieOffsetInMotorStepX	= 0;
	m_lNDieOffsetInMotorStepY	= 0;
// prescan relative code
	InitPrescanVariables();

	m_szPKGNormalDieRecordPath	= "";
	m_szPKGRefDieRecordPath		= "";
	m_szSaveImagePath			= WPR_PR_DISPLAY_IMAGE_LOG;

	m_nArmSelection = 0;

	m_lWPRSpecial1_RingLightLevel = 0;
	m_lWPRSpecial2_RingLightLevel = 0;

	m_bEnableMouseJoystick = FALSE;
	m_bNeedTurnOffLighting = FALSE;

	m_bEnableBHUplookPr		= FALSE;

	InitZoomView();
}


BOOL CWaferPr::SaveRecordID(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	if (pUtl->LoadWPRConfig() == FALSE)
	{
		return FALSE;
	}
	psmf = pUtl->GetWPRConfigFile();
	if (psmf == NULL)
	{
		return FALSE;
	}

	CString szLog = _T("WPR: SaveRecordID - ");
	CString szValue;

	for (INT i = 0; i < WPR_MAX_DIE; i++)
	{
		(*psmf)[WPR_DIE_PRID][i + 1]								= (LONG) m_ssGenPRSrchID[i];
		(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_LEARNDIE][i + 1]	= m_bGenDieLearnt[i];

		szValue.Format("%ld(ID=%ld) ", i, m_ssGenPRSrchID[i]);
		szLog = szLog + szValue;
	}

	pUtl->UpdateWPRConfig();
	pUtl->CloseWPRConfig();

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return TRUE;
}


BOOL CWaferPr::SavePrData(BOOL bGetHMIVariable)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	PR_COORD		stPixel;
	short			i = 0;
	CString			szLog;


	if (pUtl->LoadWPRConfig() == FALSE)
	{
		return FALSE;
	}

	psmf = pUtl->GetWPRConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	/*if (labs(GetPrCenterX() - PR_DEF_CENTRE_X) > 1000)
	{
		m_lPrCenterX = PR_DEF_CENTRE_X;
	}
	if (labs(GetPrCenterY() - PR_DEF_CENTRE_Y) > 1000)
	{
		m_lPrCenterY = PR_DEF_CENTRE_Y;
	}*/

	m_ucDieShape			= CheckDieShape(m_ucGenDieShape);	// save msd
	//General die data
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_CURSOR_CENTER_X]			= GetPrCenterX();				//v3.80
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_CURSOR_CENTER_Y]			= GetPrCenterY();				//v3.80
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_WAFBOND_CENTER_OFFSET_X]	= m_lWaferToBondPrCenterOffsetX;	//v4.08
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_WAFBOND_CENTER_OFFSET_Y]	= m_lWaferToBondPrCenterOffsetY;	//v4.08

	//4.52D17learn die  final Save
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_UPLOOK_BH1_LEARN_CENTER_X] = m_lPrUplookBH1LearnCenterX;		
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_UPLOOK_BH1_LEARN_CENTER_Y] = m_lPrUplookBH1LearnCenterY;		
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_UPLOOK_BH2_LEARN_CENTER_X] = m_lPrUplookBH2LearnCenterX;		
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_UPLOOK_BH2_LEARN_CENTER_Y] = m_lPrUplookBH2LearnCenterY;	

	(*psmf)[WPR_GEN_NDIE_DATA]["WPR Use Auto Bond Mode"]	= m_bUseAutoBondMode;
	(*psmf)[WPR_GEN_NDIE_DATA]["WPR Use Hardware Trigger"]	= m_bUseHWTrigger;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_SHOW_BPR_AUTO]		= m_bKeepShowBondInAuto;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_LEARNPITCH]		= m_bPitchLearnt;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_LEARNCALIB]		= m_bDieCalibrated;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_CALIBX]			= GetCalibX();
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_CALIBY]			= GetCalibY();
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_CALIBXY]		= GetCalibXY();
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_CALIBYX]		= GetCalibYX();
	(*psmf)[WPR_GEN_NDIE_DATA]["FF Calib X"]				= m_dFFCalibX;
	(*psmf)[WPR_GEN_NDIE_DATA]["FF Calib Y"]				= m_dFFCalibY;
	(*psmf)[WPR_GEN_NDIE_DATA]["FF Calib XY"]				= m_dFFCalibXY;
	(*psmf)[WPR_GEN_NDIE_DATA]["FF Calib YX"]				= m_dFFCalibYX;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_PITCHXX]		= (LONG)m_siDiePitchXX;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_PITCHXY]		= (LONG)m_siDiePitchXY;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_PITCHYX]		= (LONG)m_siDiePitchYX;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_PITCHYY]		= (LONG)m_siDiePitchYY;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_AUTOSCN]		= m_lAutoScreenMode;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_SHAPE]			= m_ucGenDieShape;
	(*psmf)[WPR_GEN_NDIE_DATA][W_ALIGN_MASTER_PITCH_X]		= m_dWAlignMasterPitchInMilX;	//v2.78T2
	(*psmf)[WPR_GEN_NDIE_DATA][W_ALIGN_MASTER_PITCH_Y]		= m_dWAlignMasterPitchInMilY;	//v2.78T2
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_OFFSETX]		= m_lNDieOffsetInMotorStepX;	//v3.77
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_OFFSETY]		= m_lNDieOffsetInMotorStepY;	//v3.77
	for(i=0; i<3; i++)
	{
		(*psmf)[WPR_GEN_NDIE_DATA][W_R_MASTER_SIZE_X][i]	= m_dRMasterDieSizeX[i];
		(*psmf)[WPR_GEN_NDIE_DATA][W_R_MASTER_SIZE_Y][i]	= m_dRMasterDieSizeY[i];
	}
	(*psmf)[WPR_GEN_NDIE_DATA][W_LEARN_PITCH_SPAN_X]	= m_lLearnPitchSpanX;
	(*psmf)[WPR_GEN_NDIE_DATA][W_LEARN_PITCH_SPAN_Y]	= m_lLearnPitchSpanY;
	//v4.42T3
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_CALIBX]				= m_dEjtCalibX;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_CALIBY]				= m_dEjtCalibY;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_CALIBXY]				= m_dEjtCalibXY;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_CALIBYX]				= m_dEjtCalibYX;

	(*psmf)[WPR_GEN_NDIE_DATA]["Ejt FF Calib X"]				= m_dEjtFFCalibX;
	(*psmf)[WPR_GEN_NDIE_DATA]["Ejt FF Calib Y"]				= m_dEjtFFCalibY;
	(*psmf)[WPR_GEN_NDIE_DATA]["Ejt FF Calib XY"]				= m_dEjtFFCalibXY;
	(*psmf)[WPR_GEN_NDIE_DATA]["Ejt FF Calib YX"]				= m_dEjtFFCalibYX;

	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_OFFSET_X]	= m_lEjtCollet1OffsetX;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_OFFSET_Y]	= m_lEjtCollet1OffsetY;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_OFFSET_X]	= m_lEjtCollet2OffsetX;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_OFFSET_Y]	= m_lEjtCollet2OffsetY;
	// added the coordinate , offset in pixel		//v4.42T15
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_OFFSET_X_PIXEL]		= m_lEjtCollet1OffsetX_Pixel;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_OFFSET_Y_PIXEL]		= m_lEjtCollet1OffsetY_Pixel;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_OFFSET_X_PIXEL]		= m_lEjtCollet2OffsetX_Pixel;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_OFFSET_Y_PIXEL]		= m_lEjtCollet2OffsetY_Pixel;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_COOR_X]				= m_lEjtCollet1CoorX ;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_COOR_Y]				= m_lEjtCollet1CoorY ;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_COOR_X]				= m_lEjtCollet2CoorX ;
	(*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_COOR_Y]				= m_lEjtCollet2CoorY ;

	(*psmf)[WPR_GEN_NDIE_DATA][WPR_F1_SRCH_DIE_WITH_ROTATION]	= m_bF1SearchDieWithRoatation;

	// Save Number of Ref Die to LastState File instead
	//Search die 
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_SRCHULCX]		= (LONG)m_stSearchArea.coCorner1.x;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_SRCHULCY]		= (LONG)m_stSearchArea.coCorner1.y;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_SRCHLRCX]		= (LONG)m_stSearchArea.coCorner2.x;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_SRCHLRCY]		= (LONG)m_stSearchArea.coCorner2.y;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_BACKUPALIGN]	= m_bSrchEnableBackupAlign;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_LOOKFORWARD]	= m_bSrchEnableLookForward;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_THETACORRECT]	= m_bThetaCorrection;
	(*psmf)[WPR_SRH_NDIE_DATA]["Bin Table Theta Correction"] = m_bBinTableThetaCorrection;
	if (m_bBinTableThetaCorrection)
	{
		//No New LF in Bin Theta Correction
		m_bWaferPrMS60LFSequence = FALSE;
	}
	else
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		m_bWaferPrMS60LFSequence	= pApp->GetFeatureStatus(MS896A_FUNC_MS60_LF_SEQ);
	}

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_WTINDEXTEST]	= m_bEnableWTIndexTest;
	
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_UPLOOK_PR]		= m_bEnableBHUplookPr;	//v4.57A8

	if (!m_bThetaCorrection && !m_bBinTableThetaCorrection)			//No Confirm-Search if theta correction is disabled
	{
		m_bConfirmSearch = FALSE;    //v2.96T3
	}		
	else
	{
		if (CMSPrescanUtility::Instance()->GetPrescanAreaPickMode() == FALSE)
		{
			m_bConfirmSearch = TRUE;
		}
	}
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_CONFIRMSRCH]	= m_bConfirmSearch;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_CHECK]	= m_bBadCutDetection;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MINANGLE]		= m_dMinDieAngle;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MAXANGLE]		= m_dMaxDieAngle;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_ACEPTANGLE]		= m_dAcceptDieAngle;

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_FORWARDSIZE]	= m_dLFSize;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_FORWARDSIZE_X]	= m_dLFSizeX;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_FORWARDSIZE_Y]	= m_dLFSizeY;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BC_BI_DETECT]	= m_bBadcutBiDetect;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BC_BI_THRESHOLD] = m_lBadcutBiThreshold;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_LRN_NORMAL_ROUGH_LED_DIE] = m_bNormalRoughLedDie;

	//v4.57A1	//v4.58A6
	//if (m_dMaxEpoxySizeFactor < 1.0)
	//	m_dMaxEpoxySizeFactor = 2.0;
	//else 
	if (m_dMaxEpoxySizeFactor > 5.0)
		m_dMaxEpoxySizeFactor = 5.0;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_MAX_EPOXY_SIZE_FACTOR] = m_dMaxEpoxySizeFactor;

	//v3.70T4
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
	//if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && (nPLLM == PLLM_REBEL))	//xyz
	{
		m_bRefDieCheck = TRUE;
	}

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_USE_NO]			= m_lSrchRefDieNo;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_REFDIE_CHECK]	= m_bRefDieCheck;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_CHK_ALL_REFDIE]	= m_bAllRefDieCheck;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_AUTO_LEARN_REFDIE]	= m_bAutoLearnRefDie;	//v2.78T2
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_NDIE_SORT_INSPECTION]	= m_bAoiSortInspction;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_NDIE_NG_PICK_PARTIAL]	= m_bAOINgPickPartDie;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_NDIE_NG_PICK_AREA_TOL_MIN]	= m_lNgPickDieAreaTolMin;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_NDIE_NG_PICK_AREA_TOL_MAX]	= m_lNgPickDieAreaTolMax;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_OCR_DIE_ORIENTATION]			= m_ulOcrDieOrientation;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_REFDIE_FACE_VALUE_NO]= m_lRefDieFaceValueNo;	//v4.48A26

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_REV_DIE_STATUS]	= m_bReverseDieStatus;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_PICK_ALIGN_DIE]	= m_bPickAlignableDie;
	if (pApp->GetCustomerName() != "Cree" && pApp->GetCustomerName() != _T("SanAn") )
	{
		m_bCheckAllNmlDie = FALSE;
	}
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_CHK_ALL_DIE]	= m_bCheckAllNmlDie;

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_SKIP]			= (ULONG)m_ulMaxSkipCount;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_NODIE_SKIP]		= (ULONG)m_ulMaxNoDieSkipCount;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_PR_SKIP]		= (ULONG)m_ulMaxPRSkipCount;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_BADCUT_SKIP]	= (ULONG)m_ulMaxBadCutSkipCount;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_DEFECT_SKIP]	= (ULONG)m_ulMaxDefectSkipCount;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_GOODDIE_RATIO]		= (DOUBLE)m_dWaferGoodDieRatio;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_WAFER_DEFECT_LIMIT]	= (ULONG)m_ulWaferMaxDefectCount;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_WAFER_DEFECT_COUNT]	= (ULONG)m_ulWaferDefectDieCount;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_START_SKIP]		= (ULONG)m_ulMaxStartSkipLimit;

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_LINE_DEFECT]			= m_bEnableLineDefect;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MIN_LINE_DEFECT_LENGTH]	= m_dMinLineDefectLength;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MIN_NUM_OF_LINE_DEFECT]	= m_lMinNumOfLineDefects;

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_AUTO_ENLARGE_SRCHWND]	= m_bAutoEnlargeSrchWnd;

	for (i = 0; i < WPR_BADCUT_POINT; i++)
	{
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_POS][i + 1]["X"] = (LONG)m_stBadCutPoint[i].x;
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_POS][i + 1]["Y"] = (LONG)m_stBadCutPoint[i].y;
	}

	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_TOL_X]	= m_stBadCutTolerenceX;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_TOL_Y]	= m_stBadCutTolerenceY;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SET_REGION_BY_DIE_SIZE_X]	= m_dSetRegionByDieSizeX;
	(*psmf)[WPR_SRH_NDIE_DATA][WPR_SET_REGION_BY_DIE_SIZE_Y]	= m_dSetRegionByDieSizeY;
	
	//BH Mark
	(*psmf)[WPR_SRH_NDIE_DATA]["BH Mark Ref1X"]	= m_lBHMarkRef1X;
	(*psmf)[WPR_SRH_NDIE_DATA]["BH Mark Ref1Y"]	= m_lBHMarkRef1Y;
	(*psmf)[WPR_SRH_NDIE_DATA]["BH Mark Ref2X"]	= m_lBHMarkRef2X;
	(*psmf)[WPR_SRH_NDIE_DATA]["BH Mark Ref2Y"]	= m_lBHMarkRef2Y;

	(*psmf)[WPR_SRH_NDIE_DATA]["BH Collet Hole Ref1X"]	= m_lBHColletHoleRef1X;
	(*psmf)[WPR_SRH_NDIE_DATA]["BH Collet Hole Ref1Y"]	= m_lBHColletHoleRef1Y;
	(*psmf)[WPR_SRH_NDIE_DATA]["BH Collet Hole Ref2X"]	= m_lBHColletHoleRef2X;
	(*psmf)[WPR_SRH_NDIE_DATA]["BH Collet Hole Ref2Y"]	= m_lBHColletHoleRef2Y;

	for (i = 0; i < WPR_MAX_LIGHT_GROUP; i++)
	{
		(*psmf)[WPR_LIGHT_SETTING][i + 1][WPR_LRN_COAX_LIGHT_LEVEL] = m_lWPRLrnCoaxLightLevel[i];
		(*psmf)[WPR_LIGHT_SETTING][i + 1][WPR_LRN_RING_LIGHT_LEVEL] = m_lWPRLrnRingLightLevel[i];
		(*psmf)[WPR_LIGHT_SETTING][i + 1][WPR_LRN_SIDE_LIGHT_LEVEL] = m_lWPRLrnSideLightLevel[i];
		(*psmf)[WPR_LIGHT_SETTING][i + 1][WPR_LRN_BACK_LIGHT_LEVEL] = m_lWPRLrnBackLightLevel[i];
	}

	(*psmf)[WPR_LIGHT_SETTING][WPR_LRN_EXPOSURE_TIME_LEVEL]		= m_lWPRLrnExposureTimeLevel;
	(*psmf)[WPR_LIGHT_SETTING][WPR_SCM_LRN_COAX_LIGHT_LEVEL]	= m_lSCMLrnCoaxLightLevel;
	(*psmf)[WPR_LIGHT_SETTING][WPR_SCM_LRN_RING_LIGHT_LEVEL]	= m_lSCMLrnRingLightLevel;
	(*psmf)[WPR_LIGHT_SETTING][WPR_SCM_LRN_SIDE_LIGHT_LEVEL]	= m_lSCMLrnSideLightLevel;
	(*psmf)[WPR_LIGHT_SETTING][WPR_SCM_LRN_EXPOSURE_TIME]		= m_lSCMLrnExposureTimeLevel;

	//v3.71T5	//PostSeal lighting if enabled
	(*psmf)[PPR_LIGHT_SETTING][PPR_GEN_COAX_LIGHT_LEVEL]	= m_lPPRGenCoaxLightLevel;
	(*psmf)[PPR_LIGHT_SETTING][PPR_GEN_RING_LIGHT_LEVEL]	= m_lPPRGenRingLightLevel;
	(*psmf)[PPR_LIGHT_SETTING][PPR_GEN_SIDE_LIGHT_LEVEL]	= m_lPPRGenSideLightLevel;


	if (bGetHMIVariable == TRUE)
	{
		GetHmiVariable();
	}

// prescan relative code
	SavePrescanDataPr(psmf);

	(*psmf)["SaveFromLastState"]	= 1;
	(*psmf)[WPR_RDIE_LEARNT]	= m_lLrnTotalRefDie;
	
	szLog = _T("WPR: SavePrData - ");
	CString szValue;

	for (i = 0; i < WPR_MAX_DIE; i++)
	{
		(*psmf)[WPR_DIE_PRID][i + 1]	= (LONG)m_ssGenPRSrchID[i];
		//v4.49A7	//Osram Penang
		szValue.Format("%ld(ID=%ld) ", i, m_ssGenPRSrchID[i]);
		szLog = szLog + szValue;

		// Save PR ID to LastState File instead
		(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_LEARNDIE][i + 1]		= m_bGenDieLearnt[i];

		(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_ALNALGO][i + 1]			= m_lGenLrnAlignAlgo[i];
		(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_BKGROUND][i + 1]		= m_lGenLrnBackgroud[i];
		m_lGenLrnFineSrch[i] = min(m_lGenLrnFineSrch[i], 2);
		m_lGenLrnFineSrch[i] = max(m_lGenLrnFineSrch[i], 0);
		//(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_FINESCH][i + 1]		= m_lGenLrnFineSrch[i];	//Default value by Matthew 20190410
		(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_INSPRES][i + 1]			= m_lGenLrnInspRes[i];
		//(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_INSPMETHOD][i + 1]	= m_lGenLrnInspMethod[i];	//Default value by Matthew 20190410
		(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_DIESIZEX][i + 1]		= (LONG)m_stGenDieSize[i].x;
		(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_DIESIZEY][i + 1]		= (LONG)m_stGenDieSize[i].y;
		(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_ZOOM_MODE][i + 1]		= m_szGenDieZoomMode[i];
		(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_CALIBRATION][i + 1]		= m_bGenDieCalibration[i];

		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_ALGNRES][i + 1]		= m_lGenSrchAlignRes[i];

		if (m_lGenSrchDefectThres[i] <= 0)		//v3.06
		{
			m_lGenSrchDefectThres[i] = 12;		//Normal
		}		
		if (m_lGenSrchDefectThres[i] >= 255)	//v4.42T15
		{
			m_lGenSrchDefectThres[i] = 12;		//Normal
		}		
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_DEFECTTHRE][i + 1]	= m_lGenSrchDefectThres[i];

		if (m_lGenSrchDieScore[i] <= 0)			//v4.42T15
		{
			m_lGenSrchDieScore[i] = 70;			//Normal
		}		
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_PASSSCORE][i + 1]		= m_lGenSrchDieScore[i];

		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_GREYLEVEL][i + 1]		= m_lGenSrchGreyLevelDefect[i];
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_SIGNLEDEFECT][i + 1]	= m_dGenSrchSingleDefectArea[i];
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_TOTALDEFECT][i + 1]	= m_dGenSrchTotalDefectArea[i];
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MINCHIPAREA][i + 1]	= m_dGenSrchChipArea[i];
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_DEFECT_CHECK][i + 1]	= m_bGenSrchEnableDefectCheck[i];
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_CHIP_CHECK][i + 1]	= m_bGenSrchEnableChipCheck[i];

		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_SCRHAREA_X][i + 1]	= GetGenSrchAreaX(i);
		(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_SCRHAREA_Y][i + 1]	= GetGenSrchAreaY(i);
	}

	(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_EXTRA_GRAB_DELAY]		= (*m_psmfSRam)["WaferPr"]["ExtraGrabDelay"];

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.49A7

	(*m_psmfSRam)["WaferPr"]["RefDie"]["Count"] = m_lLrnTotalRefDie;
	
	(*psmf)[WPR_LRN_NDIE_DATA][WPR_DIE_CIRCLE_DETECTION]		= m_bPrCircleDetection;		//v2.77
	(*psmf)[WPR_LRN_NDIE_DATA][WPR_DIE_CAL_1ST_STEP_SIZE]		= m_lPrCal1stStepSize;		//v2.89T1
	(*psmf)[WPR_LRN_NDIE_DATA][WPR_IM_AUTO_LEARN_WAFER]			= m_bIMAutoLearnWafer;		//v3.06
	(*psmf)[WPR_LRN_NDIE_DATA][WPR_ENABLE_2_LIGHTING]			= m_bEnable2Lighting;		//v3.44T1
	(*psmf)[WPR_LRN_NDIE_DATA][WPR_COMPARE_DIESCORE_HOMEREFER]	= m_bCompareHomeReferDieScore;

	(*psmf)[WPR_CAMERA_DATA][WPR_MOTORIZED_ZOOM_LEVEL]		=	m_lEnc_Zoom;
	(*psmf)[WPR_CAMERA_DATA][WPR_MOTORIZED_FOCUS_LEVEL]		=	m_lEnc_Focus;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_RESULT]			=	m_bAutoFocusOK;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_POSITION]		=	m_lAutoFocusPosition;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_POSITION2]		=	m_lAutoFocusPosition2;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_WAFER]			=	m_bWaferAutoFocus;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_PR_DELAY]		=	m_lAFPrDelay;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_UP_LIMIT]		=	m_lAFUpLimit;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_LOW_LIMIT]		=	m_lAFLowLimit;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_ERROR_TOL]		=	m_lAFErrorTol;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_MIN_DIST]		=	m_lAFMinDist;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_STANDBY_OFFSET]	=	m_lAFStandbyOffset;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_RVALUE_LIMIT]	= (DOUBLE)m_dAFrValueLimit;

	if (m_lAFDynamicAdjustStep == 0)
	{
		m_lAFDynamicAdjustStep = 10;
	}
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_DYNAMIC_ADJUST_STEP]	= m_lAFDynamicAdjustStep;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_DYNAMIC_ADJUST]		= m_bAFDynamicAdjust;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_DYNAMIC_FOCUS_SCORE]	= (DOUBLE)m_dAFDynamicAdjustLimit;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_RESCAN_BAD_FRAME]	= m_bAFRescanBadFrame;
	(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_RESCAN_FOCUS_SCORE]	= (DOUBLE)m_dAFRescanBadFrameLimit;

	// Back Light data
	(*psmf)[WPR_CAMERA_DATA][WPR_BACKLIGHT_AUTO_FOCUS_DELAY]		= m_lBLZAutoFocusDelay;
	(*psmf)[WPR_CAMERA_DATA][WPR_BACKLIGHT_ELEVATOR_UP_LEVEL]		= m_lBackLightElevatorUpLevel;
	(*psmf)[WPR_CAMERA_DATA][WPR_BACKLIGHT_ELEVATOR_STANDBY_LEVEL]	= m_lBackLightElevatorStandByLevel;

	(*psmf)[WPR_PKG_FILE_DISPLAY][WPR_PKG_FILE_NOR_DIE_IMG_PATH] = m_szPKGNormalDieRecordPath;
	(*psmf)[WPR_PKG_FILE_DISPLAY][WPR_PKG_FILE_REF_DIE_IMG_PATH] = m_szPKGRefDieRecordPath;
	(*psmf)[WPR_GRAB_SAVE_IMAGE][WPR_GRAB_SAVE_IMAGE_PATH]	= m_szSaveImagePath;

	// zoom sensor data
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][BPR_GRAB_DIGITAL_ZOOM]	= m_lGrabDigitalZoom;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_NORMAL_DIGITAL_ZOOM]	= m_lNmlDigitalZoom;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_NORMAL_ZOOM_FACTOR]	= m_lNmlZoomFactor;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_ZOOM_FACTOR]	= m_lScnZoomFactor;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_CONTINUOUS_ZOOM_MODE]		= m_bContinuousZoom;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_CON_ZOOM_SUB_FACTOR]		= m_dNmlZoomSubFactor;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_FOV_SHRINK_MODE]			= m_ucFovShrinkMode;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_CON_ZOOM_MODE_DP5M]		= m_bContinuousZoomDP5M;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_CON_ZOOM_SUB_FACTOR_DP5M]	= m_dConZoomSubFactorDP5M;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_ROI_ZOOM_SHRINK_DP5M]		= m_dWprZoomRoiShrinkDP5M;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_ROI_ZOOM_SHRINK]			= m_dWprZoomRoiShrink;

	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_CALIB_XX]		= m_stZoomView.m_dScanCalibXX;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_CALIB_XY]		= m_stZoomView.m_dScanCalibXY;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_CALIB_YY]		= m_stZoomView.m_dScanCalibYY;
	(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_CALIB_YX]		= m_stZoomView.m_dScanCalibYX;


	//Check Load/Save Data

	int lDieSizeX, lDieSizeY;
	stPixel.x = GetNmlSizePixelX();
	stPixel.y = GetNmlSizePixelY();
	ConvertPixelToMotorStep(stPixel, &lDieSizeX, &lDieSizeY);
	m_nDieSizeX = labs(lDieSizeX);
	m_nDieSizeY = labs(lDieSizeY);


	// close config file
	pUtl->UpdateWPRConfig();
	pUtl->CloseWPRConfig();


	if (m_bAOINgPickPartDie)
	{
		m_bThetaCorrection = FALSE;
		m_bConfirmSearch = FALSE;
		m_bSrchEnableLookForward = FALSE;
	}
	//Update Theta Correction Status
	(*m_psmfSRam)["WaferPr"]["EnableThetaCorrection"]	= m_bThetaCorrection;
	(*m_psmfSRam)["WaferPr"]["EnableBinTableThetaCorrection"] = m_bBinTableThetaCorrection;
	(*m_psmfSRam)["WaferPr"]["Theta Accept Angle"]		= m_dAcceptDieAngle;
	(*m_psmfSRam)["WaferPr"]["Theta Max Die Angle"]		= m_dMaxDieAngle;
	(*m_psmfSRam)["WaferPr"]["ThetaMiniumRotation"]		= (LONG)(m_dMinDieAngle / m_dThetaRes);
	(*m_psmfSRam)["WaferPr"]["ScanAlignFromFile"]		= m_bScanAlignFromFile;
	(*m_psmfSRam)["WaferPr"]["ScanAlignAutoFromMap"]	= m_bScanAlignAutoFromMap;
	(*m_psmfSRam)["WaferPr"]["EnableBHUplookPr"]		= m_bEnableBHUplookPr;
	(*m_psmfSRam)["WaferPr"]["EnableWTIndexTest"]		= m_bEnableWTIndexTest;
	(*m_psmfSRam)["WaferPr"]["Use Hardware Trigger"]	= m_bUseHWTrigger;

	//Update HIM variable
	m_bDieIsLearnt = m_bGenDieLearnt[0];		
	m_lCurNormDieID = (LONG)m_ssGenPRSrchID[0];
	m_dAlignPassScore		= m_lGenSrchDieScore[WPR_NORMAL_DIE];			//andrew
	m_dSingleDefectScore	= m_dGenSrchSingleDefectArea[WPR_NORMAL_DIE];	//andrew
	m_dTotalDefectScore		= m_dGenSrchTotalDefectArea[WPR_NORMAL_DIE];	//andrew
	m_dChipAreaScore		= m_dGenSrchChipArea[WPR_NORMAL_DIE];			//andrew

	UpdateHmiCurNormalDieSize();
	if (GetDieShape() == WPR_RECTANGLE_DIE)
	{
		stPixel.x = abs(m_stBadCutPoint[0].x - m_stBadCutPoint[1].x);
		stPixel.y = abs(m_stBadCutPoint[0].y - m_stBadCutPoint[1].y);
	}
	else if( GetDieShape() == WPR_RHOMBUS_DIE )
	{
		stPixel.x = abs(m_stBadCutPoint[2].x - m_stBadCutPoint[1].x);
		stPixel.y = abs(m_stBadCutPoint[2].y - m_stBadCutPoint[0].y);
	}
	else
	{
		stPixel.x = abs(m_stBadCutPoint[4].x - m_stBadCutPoint[1].x);
		stPixel.y = abs(m_stBadCutPoint[2].y - m_stBadCutPoint[0].y);
	}

	ConvertPixelToUnit(stPixel, &m_lCurBadCutSizeX, &m_lCurBadCutSizeY, m_bDisplayDieInUM);
	UpdateHmiCurPitchSize();

	//CalculateExtraExposureGrabTime();	//v4.51A14

	(*m_psmfSRam)["WaferPr"]["Calibration"]["X"]	= GetCalibX();
	(*m_psmfSRam)["WaferPr"]["Calibration"]["Y"]	= GetCalibY();
	(*m_psmfSRam)["WaferPr"]["Calibration"]["XY"]	= GetCalibXY();
	(*m_psmfSRam)["WaferPr"]["Calibration"]["YX"]	= GetCalibYX();

	(*m_psmfSRam)["WaferPr"]["CursorCenter"]["X"]	= GetPrCenterX();			//v3.80
	(*m_psmfSRam)["WaferPr"]["CursorCenter"]["Y"]	= GetPrCenterY();			//v3.80
	(*m_psmfSRam)["WaferPr"]["WaferToBondCenterOffset"]["X"] = (LONG)(m_lWaferToBondPrCenterOffsetX);	//v4.08
	(*m_psmfSRam)["WaferPr"]["WaferToBondCenterOffset"]["Y"] = (LONG)(m_lWaferToBondPrCenterOffsetY);	//v4.08
	(*m_psmfSRam)["WaferPr"]["CheckAllReferDie"]	= m_bAllRefDieCheck;

	SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);
	SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);

	(*m_psmfSRam)["MS896A"]["F1 Search With Rotate"]	= m_bF1SearchDieWithRoatation;
	//4.52D17learn save
	(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre X"] = m_lPrUplookBH1LearnCenterX ;
	(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre Y"] = m_lPrUplookBH1LearnCenterY ;
	(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre X"] = m_lPrUplookBH2LearnCenterX ;
	(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre Y"] = m_lPrUplookBH2LearnCenterY ;
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("ES101AutoLearnWaferNotDone"), m_bIsAutoLearnWaferNOTDone);

	CalculateDieInView(TRUE);	//	save PR data

	return TRUE;
}

BOOL CWaferPr::LoadWaferPrOption()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	short			i = 0;

	if (pUtl->LoadWPRConfig() == FALSE)	// load option data
	{
		return FALSE;
	}

	psmf = pUtl->GetWPRConfigFile();
	
	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	m_bKeepShowBondInAuto		= (BOOL)((LONG)(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_SHOW_BPR_AUTO]);
	m_lAutoScreenMode			= (*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_AUTOSCN];
	m_dWAlignMasterPitchInMilX	= (*psmf)[WPR_GEN_NDIE_DATA][W_ALIGN_MASTER_PITCH_X];		//v2.78T2
	m_dWAlignMasterPitchInMilY	= (*psmf)[WPR_GEN_NDIE_DATA][W_ALIGN_MASTER_PITCH_Y];		//v2.78T2
	for(i=0; i<3; i++)
	{
		m_dRMasterDieSizeX[i]	= (*psmf)[WPR_GEN_NDIE_DATA][W_R_MASTER_SIZE_X][i];
		m_dRMasterDieSizeY[i]	= (*psmf)[WPR_GEN_NDIE_DATA][W_R_MASTER_SIZE_Y][i];
	}

	m_bSrchEnableBackupAlign	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_BACKUPALIGN]);
	m_bSrchEnableLookForward	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_LOOKFORWARD]);
	m_bThetaCorrection			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_THETACORRECT]);
	m_bBinTableThetaCorrection	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA]["Bin Table Theta Correction"]);
	if (m_bBinTableThetaCorrection)
	{
		m_bWaferPrMS60LFSequence = FALSE;
	}
	else
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		m_bWaferPrMS60LFSequence	= pApp->GetFeatureStatus(MS896A_FUNC_MS60_LF_SEQ);
	}
	m_bConfirmSearch			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_CONFIRMSRCH]);
	m_bBadCutDetection			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_CHECK]);
	m_dMinDieAngle				= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MINANGLE];
	m_dMaxDieAngle				= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MAXANGLE];
	m_dAcceptDieAngle			= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_ACEPTANGLE];
	m_bBadcutBiDetect			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BC_BI_DETECT]);
	m_lBadcutBiThreshold		= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BC_BI_THRESHOLD];
	m_bNormalRoughLedDie		= (BOOL)(LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_LRN_NORMAL_ROUGH_LED_DIE];
	m_dMaxEpoxySizeFactor		= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_MAX_EPOXY_SIZE_FACTOR];
	m_bEnableWTIndexTest		= (BOOL)(LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_WTINDEXTEST];

	if (m_dWprZoomRoiShrink < 65)
	{
		m_dWprZoomRoiShrink = 65;
	}
	m_dWprZoomRoiShrink			= (DOUBLE)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_ROI_ZOOM_SHRINK];

	//if (m_dMaxEpoxySizeFactor < 1.0)
	//	m_dMaxEpoxySizeFactor = 2.0;
	//else 
	if (m_dMaxEpoxySizeFactor > 5.0)
		m_dMaxEpoxySizeFactor = 5.0;

	m_bEnableBHUplookPr			= (BOOL)(LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_UPLOOK_PR];	//v4.57A8

	m_bAllRefDieCheck			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_CHK_ALL_REFDIE]);
	m_bReverseDieStatus			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_REV_DIE_STATUS]);
	m_bPickAlignableDie			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_PICK_ALIGN_DIE]);
	m_bCheckAllNmlDie			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_CHK_ALL_DIE]);
	m_bAutoLearnRefDie			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_AUTO_LEARN_REFDIE]);		//v2.78T2
	m_bAoiSortInspction			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_NDIE_SORT_INSPECTION]);
	m_bAOINgPickPartDie			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_AOI_NDIE_NG_PICK_PARTIAL]);
	m_lNgPickDieAreaTolMin		= ((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_NDIE_NG_PICK_AREA_TOL_MIN]);
	m_lNgPickDieAreaTolMax		= ((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_NDIE_NG_PICK_AREA_TOL_MAX]);
	m_ulOcrDieOrientation		= ((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_OCR_DIE_ORIENTATION]);

	m_bEnableLineDefect			= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_LINE_DEFECT]);
	m_dMinLineDefectLength		= (DOUBLE)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MIN_LINE_DEFECT_LENGTH];
	m_lMinNumOfLineDefects		= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MIN_NUM_OF_LINE_DEFECT];

	m_bAutoEnlargeSrchWnd		= (BOOL)(LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_AUTO_ENLARGE_SRCHWND];

	m_ulMaxSkipCount			= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_SKIP];
	m_ulMaxNoDieSkipCount		= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_NODIE_SKIP];
	m_ulMaxPRSkipCount			= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_PR_SKIP];
	m_ulMaxBadCutSkipCount		= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_BADCUT_SKIP];
	m_ulMaxDefectSkipCount		= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_DEFECT_SKIP];
	m_dWaferGoodDieRatio		= (DOUBLE)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_GOODDIE_RATIO];
	m_ulWaferMaxDefectCount		= (ULONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_WAFER_DEFECT_LIMIT];
	m_ulWaferDefectDieCount		= (ULONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_WAFER_DEFECT_COUNT];
	m_ulMaxStartSkipLimit		= (ULONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_MAX_START_SKIP];

	m_lSrchRefDieNo				= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_USE_NO];

	m_szPKGNormalDieRecordPath	= (*psmf)[WPR_PKG_FILE_DISPLAY][WPR_PKG_FILE_NOR_DIE_IMG_PATH];
	m_szPKGRefDieRecordPath		= (*psmf)[WPR_PKG_FILE_DISPLAY][WPR_PKG_FILE_REF_DIE_IMG_PATH];
	m_szSaveImagePath = WPR_PR_DISPLAY_IMAGE_LOG;
	if ((!((*psmf)[WPR_GRAB_SAVE_IMAGE][WPR_GRAB_SAVE_IMAGE_PATH])) == FALSE)
	{
		m_szSaveImagePath = (*psmf)[WPR_GRAB_SAVE_IMAGE][WPR_GRAB_SAVE_IMAGE_PATH];
	}
	
	m_bRefDieCheck				= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_REFDIE_CHECK]);
	
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && (nPLLM == PLLM_REBEL))	//xyz
	{
		m_bRefDieCheck = TRUE;
	}
	if (pApp->GetCustomerName() != "Cree" && pApp->GetCustomerName() != _T("SanAn") )
	{
		m_bCheckAllNmlDie = FALSE;
	}

	m_lRefDieFaceValueNo	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_RDIE_REFDIE_FACE_VALUE_NO];					//v3.35T7
	m_bPrCircleDetection	= (BOOL)((LONG)(*psmf)[WPR_LRN_NDIE_DATA][WPR_DIE_CIRCLE_DETECTION]);	//v2.77
	m_lPrCal1stStepSize		= (*psmf)[WPR_LRN_NDIE_DATA][WPR_DIE_CAL_1ST_STEP_SIZE];				//v2.89T1
	m_lLearnPitchSpanX		= (*psmf)[WPR_GEN_NDIE_DATA][W_LEARN_PITCH_SPAN_X];
	m_lLearnPitchSpanY		= (*psmf)[WPR_GEN_NDIE_DATA][W_LEARN_PITCH_SPAN_Y];
	if( m_lLearnPitchSpanX<=0 )
		m_lLearnPitchSpanX			= 1;
	if( m_lLearnPitchSpanY<=0 )
		m_lLearnPitchSpanY			= 1;
	
	if (m_lPrCal1stStepSize < 30)
	{
		m_lPrCal1stStepSize = 30;
	}

	m_bIMAutoLearnWafer		= (BOOL)(LONG)(*psmf)[WPR_LRN_NDIE_DATA][WPR_IM_AUTO_LEARN_WAFER];		//v3.06
	m_bEnable2Lighting		= (BOOL)(LONG)(*psmf)[WPR_LRN_NDIE_DATA][WPR_ENABLE_2_LIGHTING];		//v3.44T1

	m_stBadCutTolerenceX		= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_TOL_X];
	m_stBadCutTolerenceY		= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_TOL_Y];

	m_bCompareHomeReferDieScore	= (BOOL)(LONG)(*psmf)[WPR_LRN_NDIE_DATA][WPR_COMPARE_DIESCORE_HOMEREFER];
	
	for (i = 0; i < WPR_MAX_DIE; i++)
	{
		m_lGenSrchAlignRes[i]	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_ALGNRES][i + 1];

		m_lGenSrchDefectThres[i]= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_DEFECTTHRE][i + 1];
		if (m_lGenSrchDefectThres[i] <= 0)	//v3.06
		{
			m_lGenSrchDefectThres[i] = 12;
		}
		if (m_lGenSrchDefectThres[i] >= 255)	//v4.42T15
		{
			m_lGenSrchDefectThres[i] = 12;
		}

		m_lGenSrchDieScore[i]	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_PASSSCORE][i + 1];
		if (m_lGenSrchDieScore[i] <= 0)		//v4.42T15
		{
			m_lGenSrchDieScore[i] = 70;
		}

		m_lGenSrchGreyLevelDefect[i]	= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_GREYLEVEL][i + 1];
		m_dGenSrchSingleDefectArea[i]	= (DOUBLE)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_SIGNLEDEFECT][i + 1];
		m_dGenSrchTotalDefectArea[i]	= (DOUBLE)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_TOTALDEFECT][i + 1];
		m_dGenSrchChipArea[i]			= (DOUBLE)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_MINCHIPAREA][i + 1];

		m_bGenSrchEnableDefectCheck[i]	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_DEFECT_CHECK][i + 1]);
		m_bGenSrchEnableChipCheck[i]	= (BOOL)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_CHIP_CHECK][i + 1]);

		m_lGenSrchDieAreaX[i]			= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_SCRHAREA_X][i + 1];
		m_lGenSrchDieAreaY[i]			= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_SCRHAREA_Y][i + 1];
		// Make sure the search area multiplier is at least 2
	}

	// Make sure Search ref die no should be at least 1
	if (m_lSrchRefDieNo == 0)
	{
		m_lSrchRefDieNo = 1;
	}

	LoadPrescanSetupData(psmf);

	pUtl->CloseWPRConfig();

	m_dAlignPassScore	= m_lGenSrchDieScore[WPR_NORMAL_DIE];		
	m_dSingleDefectScore = m_dGenSrchSingleDefectArea[WPR_NORMAL_DIE];
	m_dTotalDefectScore	= m_dGenSrchTotalDefectArea[WPR_NORMAL_DIE];
	m_dChipAreaScore	= m_dGenSrchChipArea[WPR_NORMAL_DIE];
	(*m_psmfSRam)["WaferPr"]["CheckAllReferDie"]	= m_bAllRefDieCheck;
	(*m_psmfSRam)["WaferPr"]["ScanAlignFromFile"]	= m_bScanAlignFromFile;		//v4.40T10
	(*m_psmfSRam)["WaferPr"]["ScanAlignAutoFromMap"]	= m_bScanAlignAutoFromMap;

	//v4.28T6
	if (m_bEnableLineDefect && !pApp->GetFeatureStatus(MS896A_FUNC_VISION_LINE_INSPECTION))
	{
		m_bEnableLineDefect = FALSE;
	}

	if (m_bAOINgPickPartDie)
	{
		m_bThetaCorrection = FALSE;
		m_bConfirmSearch = FALSE;
		m_bSrchEnableLookForward = FALSE;
	}

	//CalculateExtraExposureGrabTime();	//v4.49A3

	UINT unItem = pApp->GetProfileInt(gszPROFILE_SETTING, _T("ES101AutoLearnWaferNotDone"), 1);
	m_bIsAutoLearnWaferNOTDone = (BOOL)unItem;
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("ES101AutoLearnWaferNotDone"), m_bIsAutoLearnWaferNOTDone);


	return TRUE;
}

BOOL CWaferPr::LoadPrData(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	PR_COORD		stPixel;
	short			i = 0;
	CString			szLog;
	
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bIsManualLoadPkgFile = pApp->IsManualLoadPkgFile();

	if (pUtl->LoadWPRConfig() == FALSE)	// load raw data
	{
		return FALSE;
	}

	psmf = pUtl->GetWPRConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	//General die data
	m_lPrCenterX				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_CURSOR_CENTER_X];				//v3.80
	m_lPrCenterY				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_CURSOR_CENTER_Y];				//v3.80
	//m_lPrCenterX				= 4096; //hardcode 2019.03.05
	//m_lPrCenterY				= 4096;

	m_lWaferToBondPrCenterOffsetX	= (*psmf)[WPR_GEN_NDIE_DATA][WPR_WAFBOND_CENTER_OFFSET_X];	//v4.08
	m_lWaferToBondPrCenterOffsetY	= (*psmf)[WPR_GEN_NDIE_DATA][WPR_WAFBOND_CENTER_OFFSET_Y];	//v4.08
	//m_lWaferToBondPrCenterOffsetX	= 0; //hardcode 2019.03.05
	//m_lWaferToBondPrCenterOffsetY	= 0;

	//4.52D17learn die  load centre
	m_lPrUplookBH1LearnCenterX				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_UPLOOK_BH1_LEARN_CENTER_X];		
	m_lPrUplookBH1LearnCenterY				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_UPLOOK_BH1_LEARN_CENTER_Y];		
	m_lPrUplookBH2LearnCenterX				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_UPLOOK_BH2_LEARN_CENTER_X];		
	m_lPrUplookBH2LearnCenterY				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_UPLOOK_BH2_LEARN_CENTER_Y];	
	/*if (labs(GetPrCenterX() - PR_DEF_CENTRE_X) > 1000)
	{
		m_lPrCenterX = PR_DEF_CENTRE_X;
	}
	if (labs(GetPrCenterY() - PR_DEF_CENTRE_Y) > 1000)
	{
		m_lPrCenterY = PR_DEF_CENTRE_Y;
	}*/

	//v4.08
	m_lPrCenterXInPixel	= (GetPrCenterX() - PR_DEF_CENTRE_X) / GetPrScaleFactor();
	m_lPrCenterYInPixel	= (GetPrCenterY() - PR_DEF_CENTRE_Y) / GetPrScaleFactor();

	m_bUseAutoBondMode			= (BOOL)(LONG)(*psmf)[WPR_GEN_NDIE_DATA]["WPR Use Auto Bond Mode"];
	m_bUseHWTrigger				= (BOOL)(LONG)(*psmf)[WPR_GEN_NDIE_DATA]["WPR Use Hardware Trigger"];
	m_bPitchLearnt				= (BOOL)((LONG)(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_LEARNPITCH]);
	m_bDieCalibrated			= (BOOL)((LONG)(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_LEARNCALIB]);
	m_dCalibX					= (*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_CALIBX];
	m_dCalibY					= (*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_CALIBY];
	m_dCalibXY					= (*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_CALIBXY];
	m_dCalibYX					= (*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_CALIBYX];
	m_dFFCalibX					= (*psmf)[WPR_GEN_NDIE_DATA]["FF Calib X"];
	m_dFFCalibY					= (*psmf)[WPR_GEN_NDIE_DATA]["FF Calib Y"];
	m_dFFCalibXY				= (*psmf)[WPR_GEN_NDIE_DATA]["FF Calib XY"];
	m_dFFCalibYX				= (*psmf)[WPR_GEN_NDIE_DATA]["FF Calib YX"];
	m_siDiePitchXX				= (int)((LONG)(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_PITCHXX]);
	m_siDiePitchXY				= (int)((LONG)(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_PITCHXY]);
	m_siDiePitchYX				= (int)((LONG)(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_PITCHYX]);
	m_siDiePitchYY				= (int)((LONG)(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_PITCHYY]);

	// cotour depedant
	m_bWL1Calibrated		= (BOOL)(LONG)((*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBRATED]);
	m_dWL1CalibXX			= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBXX];
	m_dWL1CalibXY			= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBXY];
	m_dWL1CalibYY			= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBYY];
	m_dWL1CalibYX			= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1CALIBYX];

	m_bWL2Calibrated		= (BOOL)(LONG)((*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBRATED]);
	m_dWL2CalibXX			= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBXX];
	m_dWL2CalibXY			= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBXY];
	m_dWL2CalibYY			= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBYY];
	m_dWL2CalibYX			= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2CALIBYX];

	m_lWL1WaferOffsetX		= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1WAFEROFFSETX];
	m_lWL1WaferOffsetY		= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL1WAFEROFFSETY];

	m_lWL2WaferOffsetX		= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2WAFEROFFSETX];
	m_lWL2WaferOffsetY		= (*psmf)[WPR_ES_CONTOUR_OPTIONS][WPR_WL2WAFEROFFSETY];

	if( m_lWL1WaferOffsetX==0 )
		m_lWL1WaferOffsetX		= 0 - 407*2000;
	if( m_lWL2WaferOffsetX==0 )
		m_lWL2WaferOffsetX		= 487*2000;
	// cotour depedant

	// Back Light data
	m_lBackLightElevatorUpLevel = (*psmf)[WPR_CAMERA_DATA][WPR_BACKLIGHT_ELEVATOR_UP_LEVEL];
	m_lBackLightElevatorStandByLevel = (*psmf)[WPR_CAMERA_DATA][WPR_BACKLIGHT_ELEVATOR_STANDBY_LEVEL];

	//v4.42T3
	m_dEjtCalibX				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_CALIBX];
	m_dEjtCalibY				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_CALIBY];
	m_dEjtCalibXY				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_CALIBXY];
	m_dEjtCalibYX				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_CALIBYX];

	m_dEjtFFCalibX				= (*psmf)[WPR_GEN_NDIE_DATA]["Ejt FF Calib X"];
	m_dEjtFFCalibY				= (*psmf)[WPR_GEN_NDIE_DATA]["Ejt FF Calib Y"];
	m_dEjtFFCalibXY				= (*psmf)[WPR_GEN_NDIE_DATA]["Ejt FF Calib XY"];
	m_dEjtFFCalibYX				= (*psmf)[WPR_GEN_NDIE_DATA]["Ejt FF Calib YX"];

	m_lEjtCollet1OffsetX		= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_OFFSET_X];
	m_lEjtCollet1OffsetY		= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_OFFSET_Y];
	m_lEjtCollet2OffsetX		= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_OFFSET_X];
	m_lEjtCollet2OffsetY		= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_OFFSET_Y];
	//v4.42T15
	m_lEjtCollet1OffsetX_Pixel	= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_OFFSET_X_PIXEL];
	m_lEjtCollet1OffsetY_Pixel	= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_OFFSET_Y_PIXEL];
	m_lEjtCollet2OffsetX_Pixel	= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_OFFSET_X_PIXEL];
	m_lEjtCollet2OffsetY_Pixel	= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_OFFSET_Y_PIXEL];
	m_lEjtCollet1CoorX			= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_COOR_X];
	m_lEjtCollet1CoorY			= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET1_COOR_Y];
	m_lEjtCollet2CoorX			= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_COOR_X];
	m_lEjtCollet2CoorY			= (*psmf)[WPR_GEN_NDIE_DATA][WPR_EJT_COLLET2_COOR_Y];

	m_ucGenDieShape				= (*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_SHAPE];
	m_ucDieShape				= CheckDieShape(m_ucGenDieShape);	// load msd

	m_lNDieOffsetInMotorStepX	= (*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_OFFSETX];			//v3.77
	m_lNDieOffsetInMotorStepY	= (*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_OFFSETY];			//v3.77
	m_bF1SearchDieWithRoatation	= (BOOL)(LONG)(*psmf)[WPR_GEN_NDIE_DATA][WPR_F1_SRCH_DIE_WITH_ROTATION];

	// Load Number of Ref Dies from LastState file instead

	//Search die 
	m_stSearchArea.coCorner1.x	= (PR_WORD)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_SRCHULCX]);
	m_stSearchArea.coCorner1.y	= (PR_WORD)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_SRCHULCY]);
	m_stSearchArea.coCorner2.x	= (PR_WORD)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_SRCHLRCX]);
	m_stSearchArea.coCorner2.y	= (PR_WORD)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_SRCHLRCY]);
	
	for (i = 0; i < WPR_BADCUT_POINT; i++)
	{
		m_stBadCutPoint[i].x = (PR_WORD)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_POS][i + 1]["X"]);
		m_stBadCutPoint[i].y = (PR_WORD)((LONG)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SRN_NDIE_BADCUT_POS][i + 1]["Y"]);
	}

	m_dSetRegionByDieSizeX		= (DOUBLE)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SET_REGION_BY_DIE_SIZE_X];
	m_dSetRegionByDieSizeY		= (DOUBLE)(*psmf)[WPR_SRH_NDIE_DATA][WPR_SET_REGION_BY_DIE_SIZE_Y];
	
	m_lBHMarkRef1X				= (*psmf)[WPR_SRH_NDIE_DATA]["BH Mark Ref1X"];
	m_lBHMarkRef1Y				= (*psmf)[WPR_SRH_NDIE_DATA]["BH Mark Ref1Y"];
	m_lBHMarkRef2X				= (*psmf)[WPR_SRH_NDIE_DATA]["BH Mark Ref2X"];
	m_lBHMarkRef2Y				= (*psmf)[WPR_SRH_NDIE_DATA]["BH Mark Ref2Y"];

	m_lBHColletHoleRef1X		= (*psmf)[WPR_SRH_NDIE_DATA]["BH Collet Hole Ref1X"];
	m_lBHColletHoleRef1Y		= (*psmf)[WPR_SRH_NDIE_DATA]["BH Collet Hole Ref1Y"];
	m_lBHColletHoleRef2X		= (*psmf)[WPR_SRH_NDIE_DATA]["BH Collet Hole Ref2X"];
	m_lBHColletHoleRef2Y		= (*psmf)[WPR_SRH_NDIE_DATA]["BH Collet Hole Ref2Y"];

	// prescan relative code
	//LoadPrescanDataPr(psmf);
	LoadPrescanDataPr(psmf);

	m_dLFSize					= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_FORWARDSIZE];
	m_dLFSizeX					= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_FORWARDSIZE_X];
	m_dLFSizeY					= (*psmf)[WPR_SRH_NDIE_DATA][WPR_SRH_NDIE_FORWARDSIZE_Y];
	
	for (i = 0; i < WPR_MAX_LIGHT_GROUP; i++)
	{
		m_lWPRLrnCoaxLightLevel[i] = (*psmf)[WPR_LIGHT_SETTING][i + 1][WPR_LRN_COAX_LIGHT_LEVEL];
		m_lWPRLrnRingLightLevel[i] = (*psmf)[WPR_LIGHT_SETTING][i + 1][WPR_LRN_RING_LIGHT_LEVEL];
		m_lWPRLrnSideLightLevel[i] = (*psmf)[WPR_LIGHT_SETTING][i + 1][WPR_LRN_SIDE_LIGHT_LEVEL];
		m_lWPRLrnBackLightLevel[i] = (*psmf)[WPR_LIGHT_SETTING][i + 1][WPR_LRN_BACK_LIGHT_LEVEL];
	}
	m_lWPRLrnCoaxLightHmi = m_lWPRLrnCoaxLightLevel[0];
	m_lWPRLrnRingLightHmi = m_lWPRLrnRingLightLevel[0];
	m_lWPRLrnSideLightHmi = m_lWPRLrnSideLightLevel[0];

	m_lWPRLrnExposureTimeLevel	= (*psmf)[WPR_LIGHT_SETTING][WPR_LRN_EXPOSURE_TIME_LEVEL];
	m_lSCMLrnCoaxLightLevel	= (*psmf)[WPR_LIGHT_SETTING][WPR_SCM_LRN_COAX_LIGHT_LEVEL];
	m_lSCMLrnRingLightLevel	= (*psmf)[WPR_LIGHT_SETTING][WPR_SCM_LRN_RING_LIGHT_LEVEL];
	m_lSCMLrnSideLightLevel	= (*psmf)[WPR_LIGHT_SETTING][WPR_SCM_LRN_SIDE_LIGHT_LEVEL];
	m_lSCMLrnExposureTimeLevel	= (*psmf)[WPR_LIGHT_SETTING][WPR_SCM_LRN_EXPOSURE_TIME];

	//v3.71T5	//PostSeal lighting if enabled
	m_lPPRGenCoaxLightLevel		= (*psmf)[PPR_LIGHT_SETTING][PPR_GEN_COAX_LIGHT_LEVEL];
	m_lPPRGenRingLightLevel		= (*psmf)[PPR_LIGHT_SETTING][PPR_GEN_RING_LIGHT_LEVEL];
	m_lPPRGenSideLightLevel		= (*psmf)[PPR_LIGHT_SETTING][PPR_GEN_SIDE_LIGHT_LEVEL];


	m_lNoLastStateFile	= (*psmf)["SaveFromLastState"];
	m_lLrnTotalRefDie	= (*psmf)[WPR_RDIE_LEARNT];

	szLog = _T("WPR: LoadPrData - ");
	CString szValue;

	for (i = 0; i < WPR_MAX_DIE; i++)
	{
		// Load Record ID from LastState file instead
		if (!bIsManualLoadPkgFile)	//v4.53A22
		{
			//andrew: do not overwrite record IDs as updated by auto-assigned by 
			//	PR_DownloadRecordProcessCmd();
			m_ssGenPRSrchID[i]	= (PR_WORD)((LONG)(*psmf)[WPR_DIE_PRID][i + 1]);
		}

		szValue.Format("%ld(ID=%ld) ", i, m_ssGenPRSrchID[i]);
		szLog = szLog + szValue;

		m_bGenDieLearnt[i]				= (BOOL)((LONG)(*psmf)[WPR_GEN_NDIE_DATA][WPR_GEN_NDIE_LEARNDIE][i + 1]);
		m_lGenLrnAlignAlgo[i]			= (*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_ALNALGO][i + 1];
		m_lGenLrnBackgroud[i]			= (*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_BKGROUND][i + 1];
		//m_lGenLrnFineSrch[i]			= (*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_FINESCH][i + 1];	//Default value by Matthew 20190410
		m_lGenLrnFineSrch[i] = min(m_lGenLrnFineSrch[i], 2);
		m_lGenLrnFineSrch[i] = max(m_lGenLrnFineSrch[i], 0);
		m_lGenLrnInspRes[i]				= (*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_INSPRES][i + 1];
		//m_lGenLrnInspMethod[i]		= (*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_INSPMETHOD][i + 1];	//Default value by Matthew 20190410
		m_stGenDieSize[i].x				= (PR_WORD)((LONG)(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_DIESIZEX][i + 1]);
		m_stGenDieSize[i].y				= (PR_WORD)((LONG)(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_DIESIZEY][i + 1]);
		m_szGenDieZoomMode[i]			= (*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_ZOOM_MODE][i + 1];
		m_bGenDieCalibration[i]			= (BOOL)((LONG)(*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_CALIBRATION][i + 1]);
	}

	(*m_psmfSRam)["WaferPr"]["ExtraGrabDelay"] = (*psmf)[WPR_LRN_NDIE_DATA][WPR_LRN_NDIE_EXTRA_GRAB_DELAY];

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.49A7

	// Load Zoom & Focus Motor Position
	m_lEnc_Zoom			= (*psmf)[WPR_CAMERA_DATA][WPR_MOTORIZED_ZOOM_LEVEL];
	m_lEnc_Focus		= (*psmf)[WPR_CAMERA_DATA][WPR_MOTORIZED_FOCUS_LEVEL];
	m_bAutoFocusOK		= (BOOL)(LONG)(*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_RESULT];
	m_lAutoFocusPosition	= (*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_POSITION];
	m_lAutoFocusPosition2	= (*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_POSITION2];
	if (m_lAutoFocusPosition2 == 0)
	{
		m_lAutoFocusPosition2 = m_lAutoFocusPosition;
	}

	m_lAFStandbyOffset	= (*psmf)[WPR_CAMERA_DATA][WPR_AUTO_FOCUS_STANDBY_OFFSET];

	m_bAFDynamicAdjust		= (BOOL)(LONG)(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_DYNAMIC_ADJUST];
	m_dAFDynamicAdjustLimit	= (DOUBLE)(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_DYNAMIC_FOCUS_SCORE];
	m_bAFRescanBadFrame		= (BOOL)(LONG)(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_RESCAN_BAD_FRAME];
	m_dAFRescanBadFrameLimit = (DOUBLE)(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_RESCAN_FOCUS_SCORE];
	m_lAFDynamicAdjustStep	= (LONG)(*psmf)[WPR_CAMERA_DATA][WPR_AUTOFOCUS_DYNAMIC_ADJUST_STEP];
	if (m_lAFDynamicAdjustStep == 0)
	{
		m_lAFDynamicAdjustStep = 10;
	}

	// zoom sensor data
	m_lGrabDigitalZoom	= (*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][BPR_GRAB_DIGITAL_ZOOM];
	m_lNmlDigitalZoom	= (LONG)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_NORMAL_DIGITAL_ZOOM];
	m_lNmlZoomFactor	= (LONG)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_NORMAL_ZOOM_FACTOR];
	m_lScnZoomFactor	= (LONG)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_ZOOM_FACTOR];
	m_bContinuousZoom	= (BOOL)(LONG)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_CONTINUOUS_ZOOM_MODE];
	m_dNmlZoomSubFactor	= (DOUBLE)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_CON_ZOOM_SUB_FACTOR];
	m_ucFovShrinkMode				= (UCHAR)(LONG)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_FOV_SHRINK_MODE];
	if( m_dWprZoomRoiShrink<50 || m_dWprZoomRoiShrink>100 )
	{
		m_dWprZoomRoiShrink = 100.0;
	}

	m_bContinuousZoomDP5M	= (BOOL)(LONG)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_CON_ZOOM_MODE_DP5M];
	m_dConZoomSubFactorDP5M	= (DOUBLE)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_CON_ZOOM_SUB_FACTOR_DP5M];
	m_dWprZoomRoiShrinkDP5M	= (DOUBLE)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_ROI_ZOOM_SHRINK_DP5M];
	if( m_dWprZoomRoiShrinkDP5M<50 || m_dWprZoomRoiShrinkDP5M>100 )
	{
		m_dWprZoomRoiShrinkDP5M = 100.0;
	}

	m_stZoomView.m_dScanCalibXX = (double)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_CALIB_XX];
	m_stZoomView.m_dScanCalibXY = (double)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_CALIB_XY];
	m_stZoomView.m_dScanCalibYY = (double)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_CALIB_YY];
	m_stZoomView.m_dScanCalibYX = (double)(*psmf)[WPR_CAMERA_DATA][WPR_ZOOM_SENSOR][WPR_PRESCAN_CALIB_YX];
	pUtl->CloseWPRConfig();

	if (m_lNoLastStateFile <= 0)
	{
		//v4.53A22
		//For compatability only; very old sw uses LastState.MSD to store
		//PR records; current sw uses WaferPr.msd instead
		LoadRecordID();	
	}

	(*m_psmfSRam)["WaferPr"]["RefDie"]["Count"] = m_lLrnTotalRefDie;

	//Update Lookforward search area
	CalculateLFSearchArea();

	//Update die pitch in Sram string map file
	(*m_psmfSRam)["WaferPr"]["CursorCenter"]["X"]	= GetPrCenterX();				//v3.80
	(*m_psmfSRam)["WaferPr"]["CursorCenter"]["Y"]	= GetPrCenterY();				//v3.80
	(*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"]		= (LONG)(m_siDiePitchXX);
	(*m_psmfSRam)["WaferPr"]["DiePitchX"]["Y"]		= (LONG)(m_siDiePitchXY);
	(*m_psmfSRam)["WaferPr"]["DiePitchY"]["X"]		= (LONG)(m_siDiePitchYX);
	(*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"]		= (LONG)(m_siDiePitchYY);
	(*m_psmfSRam)["WaferPr"]["FOV"]["X"]			= m_dLFSizeX;				//v3.15T1
	(*m_psmfSRam)["WaferPr"]["FOV"]["Y"]			= m_dLFSizeY;				//v3.15T1
	(*m_psmfSRam)["WaferPr"]["WaferToBondCenterOffset"]["X"] = (LONG)(m_lWaferToBondPrCenterOffsetX);	//v4.08
	(*m_psmfSRam)["WaferPr"]["WaferToBondCenterOffset"]["Y"] = (LONG)(m_lWaferToBondPrCenterOffsetY);	//v4.08
	//Update Theta Correction Status
	(*m_psmfSRam)["WaferPr"]["EnableThetaCorrection"]	= m_bThetaCorrection;
	(*m_psmfSRam)["WaferPr"]["EnableBinTableThetaCorrection"] = m_bBinTableThetaCorrection;
	(*m_psmfSRam)["WaferPr"]["Theta Accept Angle"]		= m_dAcceptDieAngle;
	(*m_psmfSRam)["WaferPr"]["Theta Max Die Angle"]		= m_dMaxDieAngle;
	(*m_psmfSRam)["WaferPr"]["ThetaMiniumRotation"] 	= (LONG)(m_dMinDieAngle / m_dThetaRes);
	(*m_psmfSRam)["WaferPr"]["EnableBHUplookPr"]		= m_bEnableBHUplookPr;	//v4.57A8
	(*m_psmfSRam)["WaferPr"]["EnableWTIndexTest"]		= m_bEnableWTIndexTest;
	(*m_psmfSRam)["WaferPr"]["Use Hardware Trigger"]	= m_bUseHWTrigger;

	//v4.42T3
	SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);
	SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		m_bEnableMouseJoystick = TRUE;
	}

	//Update HIM variable
	m_lLrnAlignAlgo		= m_lGenLrnAlignAlgo[0];
	m_lLrnBackground	= m_lGenLrnBackgroud[0];
	m_lLrnInspMethod	= m_lGenLrnInspMethod[0];
	m_bDieIsLearnt		= m_bGenDieLearnt[0];		
	m_lCurNormDieID		= (LONG)m_ssGenPRSrchID[0];
	m_lSrchDieAreaX		= GetGenSrchAreaX(0);
	m_lSrchDieAreaY		= GetGenSrchAreaY(0);		
	m_dSrchDieAreaX		= GetSrchDieAreaX() * WPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60
	m_dSrchDieAreaY		= GetSrchDieAreaY() * WPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60

	UpdateHmiCurNormalDieSize();
	if (GetDieShape() == WPR_RECTANGLE_DIE)
	{
		stPixel.x = abs(m_stBadCutPoint[0].x - m_stBadCutPoint[1].x);
		stPixel.y = abs(m_stBadCutPoint[0].y - m_stBadCutPoint[1].y);
	}
	else if (GetDieShape() == WPR_RHOMBUS_DIE)
	{
		stPixel.x = abs(m_stBadCutPoint[2].x - m_stBadCutPoint[1].x);
		stPixel.y = abs(m_stBadCutPoint[2].y - m_stBadCutPoint[0].y);
	}
	else
	{
		stPixel.x = abs(m_stBadCutPoint[4].x - m_stBadCutPoint[1].x);
		stPixel.y = abs(m_stBadCutPoint[2].y - m_stBadCutPoint[0].y);
	}

	ConvertPixelToUnit(stPixel, &m_lCurBadCutSizeX, &m_lCurBadCutSizeY, m_bDisplayDieInUM);
	UpdateHmiCurPitchSize();

	(*m_psmfSRam)["WaferPr"]["Calibration"]["X"]	= GetCalibX();
	(*m_psmfSRam)["WaferPr"]["Calibration"]["Y"]	= GetCalibY();
	(*m_psmfSRam)["WaferPr"]["Calibration"]["XY"]	= GetCalibXY();
	(*m_psmfSRam)["WaferPr"]["Calibration"]["YX"]	= GetCalibYX();

	(*m_psmfSRam)["MS896A"]["F1 Search With Rotate"] = m_bF1SearchDieWithRoatation;
	//4.52D17learn load
	(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre X"] = m_lPrUplookBH1LearnCenterX ;
	(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre Y"] = m_lPrUplookBH1LearnCenterY ;
	(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre X"] = m_lPrUplookBH2LearnCenterX ;
	(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre Y"] = m_lPrUplookBH2LearnCenterY ;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();
	PR_RSIZE		stFov;
	PR_OPTIC		stOptic;
	PR_COMMON_RPY	stComRpy;
	stFov.x = (PR_REAL)(GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * GetCalibX() * PR_MAX_COORD);
	stFov.y = (PR_REAL)(GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * GetCalibY() * PR_MAX_COORD);
	// Set FOV to Wafer PR
	PR_GetOptic(ubPpsG, ubSID, ubRID, &stOptic, &stComRpy);
	PR_SetFov(&stFov, ubSID, ubRID, &stOptic, &stComRpy);

// prescan relative code	B
	int lDieSizeX, lDieSizeY;
	stPixel.x = GetNmlSizePixelX();
	stPixel.y = GetNmlSizePixelY();
	ConvertPixelToMotorStep(stPixel, &lDieSizeX, &lDieSizeY);

	m_nDieSizeX = labs(lDieSizeX);
	m_nDieSizeY = labs(lDieSizeY);

	// Back Light data
	m_lWPRGeneral_1 = m_lBackLightElevatorUpLevel;
	m_lWPRGeneral_2 = m_lBackLightElevatorStandByLevel;

	CalculateDieInView(TRUE);	//	load PR data
	//CalculateExtraExposureGrabTime();	//v4.51A14

	m_pPrescanPrCtrl->RemoveAllShareImages();
// prescan relative code	E

	m_bWarningNoInspection	= TRUE;	
	
	//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->m_bMSAutoLineMode == 1)
	{
		m_bWarningNoInspection	= FALSE;	//v4.57A15	//AUTOLINE
	}

	return TRUE;
}

BOOL CWaferPr::LoadRecordID(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	short			i = 0;
	CString			szLog;

	//v4.53A9
	if (pUtl->LoadLastState("WPR: LoadRecordID") == FALSE) 	// open LastState file
	{
		return FALSE;
	}

	psmf = pUtl->GetLastStateFile("WPR: LoadRecordID");		// get file pointer
	if (psmf != NULL)
	{
		m_lLrnTotalRefDie	= (*psmf)[WPR_RDIE_LEARNT];

		//v4.49A7
		CString szLog = _T("WPR: LoadRecordID - ");
		CString szValue;

		for (i = 0; i < WPR_MAX_DIE; i++)
		{
			m_ssGenPRSrchID[i]	= (PR_WORD)((LONG)(*psmf)[WPR_DIE_PRID][i + 1]);
			szValue.Format("%ld(ID=%ld) ", i, m_ssGenPRSrchID[i]);
			szLog = szLog + szValue;
		}
	
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}

	//v4.53A9
	// close config file
	pUtl->CloseLastState("WPR: LoadRecordID");

	return TRUE;
}

LONG CWaferPr::GetWprDieNo()
{
	LONG	lDieNo = 0;

	if (m_bSelectDieType == WPR_NORMAL_DIE)
	{
		lDieNo = WPR_GEN_NDIE_OFFSET + m_lCurRefDieNo;
		if( m_bEnable2ndPrSearch && m_lCurRefDieNo==GetPrescan2ndPrID() )
		{
		}
		else
		if (lDieNo > WPR_GEN_RDIE_OFFSET)
		{
			lDieNo = 0;
		}
	}
	else
	{
		lDieNo = WPR_GEN_RDIE_OFFSET + m_lCurRefDieNo;
		if( IsOcrAOIMode() && IsCharDieInUse() && m_lCurRefDieNo>=WPR_GEN_OCR_DIE_START)
		{
			lDieNo = m_lCurRefDieNo;
		}
	}

	if (lDieNo < 0 || lDieNo >= WPR_MAX_DIE)
	{
		lDieNo = 0;
	}

	return lDieNo;
}

VOID CWaferPr::UpdateHmiVariable(VOID)
{
	LONG	lDieNo = 0;

	lDieNo = GetWprDieNo();

	m_ucDieShape				= CheckDieShape(m_ucGenDieShape);	// update hmi
	m_lLrnAlignAlgo				= m_lGenLrnAlignAlgo[lDieNo];
	m_lLrnBackground			= m_lGenLrnBackgroud[lDieNo];
	m_lLrnFineSrch				= m_lGenLrnFineSrch[lDieNo];
	m_lLrnInspMethod			= m_lGenLrnInspMethod[lDieNo];
	m_lLrnInspRes				= m_lGenLrnInspRes[lDieNo];
	m_lSrchAlignRes				= m_lGenSrchAlignRes[lDieNo];
	m_lSrchDefectThres			= m_lGenSrchDefectThres[lDieNo];
	m_lSrchDieAreaX				= GetGenSrchAreaX(lDieNo);
	m_lSrchDieAreaY				= GetGenSrchAreaY(lDieNo);
	m_lSrchDieScore				= m_lGenSrchDieScore[lDieNo];

	if (CNGGrade::Instance()->m_bHaveNGGrade)
	{
		m_lGenSrchDieScore[BPR_GEN_NG_GRADE_INDEX_WITH_OFFSET]	= m_lGenSrchDieScore[WPR_NORMAL_DIE];
	}

	m_lSrchGreyLevelDefect		= m_lGenSrchGreyLevelDefect[lDieNo];
	m_dSrchSingleDefectArea		= m_dGenSrchSingleDefectArea[lDieNo];
	m_dSrchTotalDefectArea		= m_dGenSrchTotalDefectArea[lDieNo];
	m_dSrchChipArea				= m_dGenSrchChipArea[lDieNo];
	m_bSrchEnableDefectCheck	= m_bGenSrchEnableDefectCheck[lDieNo];
	m_bSrchEnableChipCheck		= m_bGenSrchEnableChipCheck[lDieNo];

	if( IsDP_ScanCam() )
	{
		m_lWPRLrnCoaxLightHmi = m_lSCMLrnCoaxLightLevel;
		m_lWPRLrnRingLightHmi = m_lSCMLrnRingLightLevel;
		m_lWPRLrnSideLightHmi = m_lSCMLrnSideLightLevel;
	}
	else
	{
		m_lWPRLrnCoaxLightHmi = m_lWPRLrnCoaxLightLevel[m_bSelectDieType];
		m_lWPRLrnRingLightHmi = m_lWPRLrnRingLightLevel[m_bSelectDieType];
		m_lWPRLrnSideLightHmi = m_lWPRLrnSideLightLevel[m_bSelectDieType];
	}

	m_dSrchDieAreaX				= GetSrchDieAreaX() * WPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60
	m_dSrchDieAreaY				= GetSrchDieAreaY() * WPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60
}


VOID CWaferPr::GetHmiVariable(VOID)
{
	LONG	lDieNo = 0;

	lDieNo = GetWprDieNo();

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Lumileds" && (lDieNo == 1 || lDieNo == 2))
	{
		m_lLrnAlignAlgo = 2;
	}

	if( IsDP_ScanCam() )
	{
		m_lSCMLrnCoaxLightLevel	= m_lWPRLrnCoaxLightHmi;
		m_lSCMLrnRingLightLevel	= m_lWPRLrnRingLightHmi;
		m_lSCMLrnSideLightLevel	= m_lWPRLrnSideLightHmi;
	}
	else
	{
		m_lWPRLrnCoaxLightLevel[m_bSelectDieType] = m_lWPRLrnCoaxLightHmi;
		m_lWPRLrnRingLightLevel[m_bSelectDieType] = m_lWPRLrnRingLightHmi;
		m_lWPRLrnSideLightLevel[m_bSelectDieType] = m_lWPRLrnSideLightHmi;
	}

	m_ucGenDieShape						= CheckDieShape(m_ucDieShape);	// get from hmi
	m_lGenLrnAlignAlgo[lDieNo]			= m_lLrnAlignAlgo; 
	m_lGenLrnBackgroud[lDieNo]			= m_lLrnBackground;
	m_lGenLrnFineSrch[lDieNo]			= m_lLrnFineSrch;
	m_lGenLrnInspMethod[lDieNo]			= m_lLrnInspMethod;
	m_lGenLrnInspRes[lDieNo]			= m_lLrnInspRes;
	m_lGenSrchAlignRes[lDieNo]			= m_lSrchAlignRes;
	m_lGenSrchDefectThres[lDieNo]		= m_lSrchDefectThres;
	m_lGenSrchDieAreaX[lDieNo]			= GetSrchDieAreaX();
	m_lGenSrchDieAreaY[lDieNo]			= GetSrchDieAreaY();

	if (m_lSrchDieScore <= 0)		//v2.83T58	//v4.42T15
	{
		m_lSrchDieScore = 70;
	}

	m_lGenSrchDieScore[lDieNo]			= m_lSrchDieScore;
	m_lGenSrchGreyLevelDefect[lDieNo]	= m_lSrchGreyLevelDefect;
}


VOID CWaferPr::OutText(char *pMessage, PR_COORD stTxtCoord, PR_COLOR eTxtColor)
{
	// Display a text
	PR_DISPLAY_CHAR_CMD	stDispCmd;
	PR_DISPLAY_CHAR_RPY	stDispRpy;

	stDispCmd.tcoPosition.x = stTxtCoord.x;
	stDispCmd.tcoPosition.y = stTxtCoord.y;
	stDispCmd.emColor = eTxtColor;
	stDispCmd.uwNChar = (PR_UWORD)strlen(pMessage);
	stDispCmd.pubChar = (PR_UBYTE*)pMessage;
	PR_DisplayCharCmd(&stDispCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stDispRpy);
}

VOID CWaferPr::ChangeCamera(BOOL bCameraNo, BOOL bChangeJoystick, UINT unPostSealID)
{
	if (PR_NotInit())
	{
		return;
	}

	(*m_psmfSRam)["MS896A"]["Current Camera"] = (LONG)bCameraNo;
	
	//v4.57A3
	//CString szLog;
	//szLog.Format("Change Camera --- CameraNo:%d, Joy:%d, PostSealID:%d", bCameraNo,bChangeJoystick, unPostSealID );
	//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	m_lJsTableMode = bCameraNo;
	
	m_bCurrentCamera = bCameraNo;
	if ((State() == AUTO_Q) || (State() == DEMO_Q))
	{		
		AutoBondScreen(TRUE);
		//WT_SpecialLog("WPR change camera auto to grab mode");		//v4.31T8
		return;
	}
	else if ((State() == IDLE_Q) && m_bAutoBondMode)	//v4.12T1	//Cree US
	{
		AutoBondScreen(FALSE);		//Reset AUTOBOND mode to LiveVideo mode
	}

	m_bAutoBondMode = FALSE;
	PR_COORD stCrossHair;
	PR_SELECT_VIDEO_SOURCE_CMD		stCmd;
	PR_SELECT_VIDEO_SOURCE_RPY		stRpy;
	PR_COORD stCollet1BondCorner1, stCollet1BondCorner2;
	PR_COORD stCollet2BondCorner1, stCollet2BondCorner2;
	stCollet1BondCorner1.x = 0;
	stCollet1BondCorner1.y = 0;
	stCollet1BondCorner2.x = 0;
	stCollet1BondCorner2.y = 0;
	stCollet2BondCorner1.x = 0;
	stCollet2BondCorner1.y = 0;
	stCollet2BondCorner2.x = 0;
	stCollet2BondCorner2.y = 0;

	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	PR_InitSelectVideoSourceCmd(&stCmd);


	m_ulPrWindowHeight		= 480;
	m_ulPrWindowWidth		= 512;
	m_bCPItemMapToTop		= FALSE;
	m_unCurrPostSealID		= 0;
	m_bPostSealCamera		= FALSE;
	PR_CAMERA	ubCameraID	= GetRunCamID();
	PR_PURPOSE	ubPrPurPose = GetRunPurposeG();
	switch (bCameraNo)
	{
	case 1:		//Bond PR	

		ubCameraID				= MS899_BOND_CAM_ID;
		ubPrPurPose				= MS899_BOND_GEN_PURPOSE;
		m_ulWaferCameraColor	= WPR_UNSELECTED_COLOR;
		m_ulBondCameraColor		= WPR_SELECTED_COLOR;
		m_ul2DCameraColor		= WPR_UNSELECTED_COLOR;		//pllm
		m_ulBH1UplookCameraColor = WPR_UNSELECTED_COLOR;
		m_ulBH2UplookCameraColor = WPR_UNSELECTED_COLOR;

		//v4.59A8	//Use WPR_CAM_WAFER to access BH Uplook PostSeal cameras
		/*
		if(unPostSealID == 6) 
		{
			// BH1 uplook
			ubPrPurPose = MS899_POSTSEAL_BH1_GEN_PURPOSE; // PR_PURPOSE_CAM_A3
			ubCameraID	= MS899_POSTSEAL_2D_BH1_CAM_ID;	// PR_CAMERA_4
			
			m_ulWaferCameraColor	= WPR_UNSELECTED_COLOR;
			m_ulBondCameraColor		= WPR_UNSELECTED_COLOR;
			m_ul2DCameraColor		= WPR_UNSELECTED_COLOR;		//pllm
			m_ulBH1UplookCameraColor = WPR_SELECTED_COLOR;
			m_ulBH2UplookCameraColor = WPR_UNSELECTED_COLOR;

			m_bPostSealCamera		= TRUE;
			m_unCurrPostSealID		= unPostSealID;
			//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Ref PostSealID: case 6");
		}
		else if(unPostSealID == 7)
		{
			// BH2 uplook
			ubPrPurPose = MS899_POSTSEAL_BH2_GEN_PURPOSE; // PR_PURPOSE_CAM_A4
			ubCameraID	= MS899_POSTSEAL_2D_BH2_CAM_ID; // PR_CAMERA_5

			m_ulWaferCameraColor	= WPR_UNSELECTED_COLOR;
			m_ulBondCameraColor		= WPR_UNSELECTED_COLOR;
			m_ul2DCameraColor		= WPR_UNSELECTED_COLOR;		//pllm
			m_ulBH1UplookCameraColor = WPR_UNSELECTED_COLOR;
			m_ulBH2UplookCameraColor = WPR_SELECTED_COLOR;

			m_bPostSealCamera		= TRUE;
			m_unCurrPostSealID		= unPostSealID;
			//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Ref PostSealID: case 7");
		}*/
		break;

	default:	//Wafer PR

		if (unPostSealID > 0)		//PostSeal PR	//v3.70T2
		{
			ubPrPurPose = MS899_POSTSEAL_PR_PURPOSE;
			ubCameraID	= MS899_POSTSEAL_CAM_ID;

			m_ulWaferCameraColor		= WPR_UNSELECTED_COLOR;
			m_ulBondCameraColor			= WPR_UNSELECTED_COLOR;
			m_ul2DCameraColor			= WPR_SELECTED_COLOR;
			m_ulBH1UplookCameraColor	= WPR_UNSELECTED_COLOR;
			m_ulBH2UplookCameraColor	= WPR_UNSELECTED_COLOR;
			m_bPostSealCamera			= TRUE;

			m_unCurrPostSealID			= unPostSealID;

			switch ( unPostSealID )
			{
			case 2:	// BH1 2D code	//PLLM uplook for 2D code checking
				ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
				ubCameraID	= MS899_POSTSEAL_2D_BH1_CAM_ID;
				break;
			case 3:	// BH2 2D code	//PLLM uplook for 2D code checking
				ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
				ubCameraID	= MS899_POSTSEAL_2D_BH2_CAM_ID;
				break;
			case 4:	// WL1
				ubPrPurPose = ES_CONTOUR_WL1_PR_PURPOSE;
				ubCameraID	= ES_CONTOUR_WL1_CAM_ID;
				break;
			case 5:	// WL2
				ubPrPurPose = ES_CONTOUR_WL2_PR_PURPOSE;
				ubCameraID	= ES_CONTOUR_WL2_CAM_ID;
				break;
			case 6:	//BH1 Uplook PR		//MS90
				ubPrPurPose = MS899_POSTSEAL_BH1_GEN_PURPOSE;
				ubCameraID	= MS899_POSTSEAL_2D_BH1_CAM_ID;	
				m_ul2DCameraColor			= WPR_UNSELECTED_COLOR;
				m_ulBH1UplookCameraColor	= WPR_SELECTED_COLOR;
				break;
			case 7:	//BH2 Uplook PR		//MS90
				ubPrPurPose = MS899_POSTSEAL_BH2_GEN_PURPOSE;
				ubCameraID	= MS899_POSTSEAL_2D_BH2_CAM_ID;
				m_ul2DCameraColor			= WPR_UNSELECTED_COLOR;
				m_ulBH2UplookCameraColor	= WPR_SELECTED_COLOR;
				break;

			default:
				break;
			}
		}
		else	//Wafer PR
		{
			m_ulWaferCameraColor		= WPR_SELECTED_COLOR;
			m_ulBondCameraColor			= WPR_UNSELECTED_COLOR;
			m_ul2DCameraColor			= WPR_UNSELECTED_COLOR;		//pllm
			m_ulBH1UplookCameraColor	= WPR_UNSELECTED_COLOR;
			m_ulBH2UplookCameraColor	= WPR_UNSELECTED_COLOR;
		}
		break;

	}
	

	stCmd.emCameraNo		= ubCameraID;
	stCmd.emPurpose			= ubPrPurPose;
	PR_SelectVideoSourceCmd(&stCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stRpy);


	//Draw Home cursor
	switch (bCameraNo)
	{
	case 1:		//Bond PR		
		//stHomeCmd.coPosition.x = (PR_WORD)(LONG)(*m_psmfSRam)["BondPr"]["CursorCenter"]["X"];	
		//stHomeCmd.coPosition.y = (PR_WORD)(LONG)(*m_psmfSRam)["BondPr"]["CursorCenter"]["Y"];
		stCrossHair.x = (PR_WORD)(LONG)(*m_psmfSRam)["BondPr"]["CursorCenter"]["X"];
		stCrossHair.y = (PR_WORD)(LONG)(*m_psmfSRam)["BondPr"]["CursorCenter"]["Y"];

		if (IsMS60())	//v4.47T1
		{
			DOUBLE dBprCalibX	= (DOUBLE)(*m_psmfSRam)["BondPr"]["Calibration"]["X"];
			DOUBLE dBprCalibY	= (DOUBLE)(*m_psmfSRam)["BondPr"]["Calibration"]["Y"];
			CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
			if (pBondPr != NULL)
			{
				dBprCalibX	= pBondPr->GetCalibX();
				dBprCalibY	= pBondPr->GetCalibX();
			}

			LONG lULX = (*m_psmfSRam)["BondPr"]["SearchArea"]["ULX"];
			LONG lULY = (*m_psmfSRam)["BondPr"]["SearchArea"]["ULY"];
			LONG lLRX = (*m_psmfSRam)["BondPr"]["SearchArea"]["LRX"];
			LONG lLRY = (*m_psmfSRam)["BondPr"]["SearchArea"]["LRY"];
			if (pBondPr && pBondPr->IsSensorZoomFFMode())
			{
				lULX = (*m_psmfSRam)["BondPr"]["FFModeSearchArea"]["ULX"];
				lULY = (*m_psmfSRam)["BondPr"]["FFModeSearchArea"]["ULY"];
				lLRX = (*m_psmfSRam)["BondPr"]["FFModeSearchArea"]["LRX"];
				lLRY = (*m_psmfSRam)["BondPr"]["FFModeSearchArea"]["LRY"];
			}

			if (m_bEnableMS100EjtXY)
			{
				if ( (dBprCalibX != 0) && (dBprCalibY != 0) )
				{
					LONG lBHZ1BondPosOffsetXCount = (*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX"];
					LONG lBHZ1BondPosOffsetYCount = (*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY"];

					stCollet1BondCorner1.x = (PR_WORD)_round(lULX + (lBHZ1BondPosOffsetXCount / dBprCalibX));
					stCollet1BondCorner1.y = (PR_WORD)_round(lULY + (lBHZ1BondPosOffsetYCount / dBprCalibY));
					stCollet1BondCorner2.x = (PR_WORD)_round(lLRX + (lBHZ1BondPosOffsetXCount / dBprCalibX));
					stCollet1BondCorner2.y = (PR_WORD)_round(lLRY + (lBHZ1BondPosOffsetYCount / dBprCalibY));

					LONG lBHZ2BondPosOffsetXCount = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"];
					LONG lBHZ2BondPosOffsetYCount = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"];

					stCollet2BondCorner1.x = (PR_WORD)_round(lULX + (lBHZ2BondPosOffsetXCount / dBprCalibX));
					stCollet2BondCorner1.y = (PR_WORD)_round(lULY + (lBHZ2BondPosOffsetYCount / dBprCalibY));
					stCollet2BondCorner2.x = (PR_WORD)_round(lLRX + (lBHZ2BondPosOffsetXCount / dBprCalibX));
					stCollet2BondCorner2.y = (PR_WORD)_round(lLRY + (lBHZ2BondPosOffsetYCount / dBprCalibY));
				}
			}
			else
			{
				stCollet1BondCorner1.x = (PR_WORD)lULX;
				stCollet1BondCorner1.y = (PR_WORD)lULY;
				stCollet1BondCorner2.x = (PR_WORD)lLRX;
				stCollet1BondCorner2.y = (PR_WORD)lLRY;
				
				stCollet2BondCorner1.x = (PR_WORD)lULX;
				stCollet2BondCorner1.y = (PR_WORD)lULY;
				stCollet2BondCorner2.x = (PR_WORD)lLRX;
				stCollet2BondCorner2.y = (PR_WORD)lLRY;
			}
		}

		break;
	default:	//Wafer PR
		if (unPostSealID > 0)		//PostSeal PR
		{
			stCrossHair.x = PR_DEF_CENTRE_X;
			stCrossHair.y = PR_DEF_CENTRE_Y;
		}
		else
		{
			stCrossHair.x = (PR_WORD)GetPrCenterX();	
			stCrossHair.y = (PR_WORD)GetPrCenterY();
		}
		break;
	}

	DrawHomeCursor(stCrossHair);
	
	if (IsMS60() && bCameraNo /*&& m_bBTIsMoveCollet*/)		//v4.47T1	//v4.47A1	//BOND camera, display EjtXY SrchWnds
	{
		DrawRectangleBox(stCollet1BondCorner1, stCollet1BondCorner2, PR_COLOR_YELLOW);
		DrawRectangleBox(stCollet2BondCorner1, stCollet2BondCorner2, PR_COLOR_RED);
	}

	if (bChangeJoystick == TRUE)
	{
		//Switch Joystick
		switch (bCameraNo)
		{
		case 1:		
			UseWaferTableJoystick(FALSE);
			UseBinTableJoystick(TRUE);
			if (!CMS896AApp::m_bMS100Plus9InchOption)	//v4.35T4	
			{
				m_lJsBinTableInUse = 0;    //Always switch to BT1 by default; only for MS100 9Inch		//v4.17T1
			}					
			break;

		default:
			if (unPostSealID > 0)
			{
				if (CMS896AStn::m_bAutoChangeCollet)	// && (unPostSealID == 1))		//v4.51A23
				{
					m_lJsTableMode = 1;								//v4.51A25
					(*m_psmfSRam)["MS896A"]["Current Camera"] = 1;	//v4.51A25
					UseWaferTableJoystick(FALSE);
					UseBinTableJoystick(TRUE);
				}
				else
				{
					UseWaferTableJoystick(FALSE);
					UseBinTableJoystick(FALSE);
				}
			}
			else
			{
				UseBinTableJoystick(FALSE);
				UseWaferTableJoystick(TRUE);
			}
			break;
		}
	}
}


BOOL CWaferPr::UpdateAutoBondScreen(BOOL bOn)
{
	PR_SCREEN					stBackGroundCmd;
	PR_SCREEN					stForeGroundCmd;
	PR_COMMON_RPY				stBackGroundRpy;
	PR_COMMON_RPY				stForeGroundRpy;
	PR_PURPOSE					uiBackGroundScn;
	PR_PURPOSE					uiForeGroundScn;
	PR_AUTO_BOND_SCREEN_MODE	uiMode;
	PR_COORD					stTxtCoord;
	PR_COORD					stCrossHair;

	stTxtCoord.x = 2;
	stTxtCoord.y = 18;

	BOOL bBprUsePostBond = (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["UsePostBond"];		//v4.35T1

	//Check current Screen mode
	switch (m_lAutoScreenMode)
	{
	case 1:	
		uiMode = PR_AUTO_BOND_SCREEN_MODE_4;
		break;

	case 2:	
		uiMode = PR_AUTO_BOND_SCREEN_MODE_16;
		break;

	default:	
		uiMode = PR_AUTO_BOND_SCREEN_MODE_1;
		break;
	}

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	PR_PURPOSE ubPpsI = GetRunPurposeI();
	PR_PURPOSE ubPpsG = GetRunPurposeG();
	//Select background & foreground
	switch (m_bCurrentCamera)
	{
	case WPR_CAM_BOND:
		//if (m_bUsePostBond == FALSE)
		if (bBprUsePostBond == FALSE)		//v4.35T1
		{
			uiBackGroundScn = PR_PURPOSE_CAM_A1;
		}
		else
		{
			uiBackGroundScn = MS899_BOND_PB_PURPOSE;
		}
		uiForeGroundScn = ubPpsI;
		if (m_bWprInAutoSortingMode == FALSE)
		{
			uiForeGroundScn = ubPpsG;
		}
		break;

	default:
		if (bBprUsePostBond == FALSE)		//v4.35T1
		{
			uiForeGroundScn = PR_PURPOSE_CAM_A1;
		}
		else
		{
			uiForeGroundScn = MS899_BOND_PB_PURPOSE;
		}
		uiBackGroundScn = ubPpsI;
		if (m_bWprInAutoSortingMode == FALSE)
		{
			uiBackGroundScn = ubPpsG;
		}
		break;
	}

	//Draw Home Cursor
	stCrossHair.x = (PR_WORD)GetPrCenterX();
	stCrossHair.y = (PR_WORD)GetPrCenterY();
	DrawHomeCursor(stCrossHair);

	//Set background
	PR_InitPRScreen(&stBackGroundCmd);
	stBackGroundCmd.aemPurpose[0] = uiBackGroundScn;
	stBackGroundCmd.wNumOfPurpose = 1;

	// if , comment the whole #ifdef block
	PR_SetAutoBondBackgroundCmd(uiMode, &stBackGroundCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stBackGroundRpy);
	if (stBackGroundRpy.uwCommunStatus != PR_COMM_NOERR || stBackGroundRpy.uwPRStatus != PR_ERR_NOERR)
	{
		return FALSE;
	}

	//Set foreground
	if (uiMode != PR_AUTO_BOND_SCREEN_MODE_1)
	{
		PR_InitPRScreen(&stForeGroundCmd);
		stForeGroundCmd.aemPurpose[0] = uiForeGroundScn;
		stForeGroundCmd.wNumOfPurpose = 1;

		// if , comment the whole #ifdef block
		PR_SetAutoBondForegroundCmd(uiMode, (PR_WORD)(m_lAutoScreenFG), &stForeGroundCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stForeGroundRpy);
		if (stForeGroundRpy.uwCommunStatus != PR_COMM_NOERR || stForeGroundRpy.uwPRStatus != PR_ERR_NOERR)
		{
			return FALSE;
		}
	}

	return TRUE;
}


VOID CWaferPr::DrawRectangleBox(PR_COORD stCorner1, PR_COORD stCorner2, PR_COLOR ssColor)
{
	PR_DRAW_RECT_CMD		stRectCmd;
	PR_DRAW_RECT_RPY		stRectRpy;

	stRectCmd.emColor = (PR_COLOR)(ssColor);
	stRectCmd.stWin.coCorner1.x = (PR_WORD)(stCorner1.x);
	stRectCmd.stWin.coCorner1.y = (PR_WORD)(stCorner1.y);
	stRectCmd.stWin.coCorner2.x = (PR_WORD)(stCorner2.x);
	stRectCmd.stWin.coCorner2.y = (PR_WORD)(stCorner2.y);

	PR_DrawRectCmd(&stRectCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stRectRpy);
}


VOID CWaferPr::DrawRectangleDieSize(PR_COLOR ssColor)
{
	PR_COORD stCorner1; 
	PR_COORD stCorner2;

	stCorner1.x = (PR_WORD)GetPrCenterX() - (GetNmlSizePixelX() / 2);
	stCorner1.y = (PR_WORD)GetPrCenterY() - (GetNmlSizePixelY() / 2);
	stCorner2.x = (PR_WORD)GetPrCenterX() + (GetNmlSizePixelX() / 2);
	stCorner2.y = (PR_WORD)GetPrCenterY() + (GetNmlSizePixelY() / 2);

	DrawRectangleBox(stCorner1, stCorner2, ssColor);
}

VOID CWaferPr::DrawSrchDieResultBox(BOOL bDieType, LONG lRefNo, PR_COLOR ssColor)
{
	PR_COORD stCorner1; 
	PR_COORD stCorner2;
	LONG	lDieNo;

	if (bDieType == WPR_NORMAL_DIE)
	{
		lDieNo = WPR_GEN_NDIE_OFFSET + lRefNo;
	}
	else
	{
		lDieNo = WPR_GEN_RDIE_OFFSET + lRefNo;
	}

	stCorner1.x = (PR_WORD)GetPrCenterX() - (GetDieSizePixelX(lDieNo) / 2);
	stCorner1.y = (PR_WORD)GetPrCenterY() - (GetDieSizePixelY(lDieNo) / 2);
	stCorner2.x = (PR_WORD)GetPrCenterX() + (GetDieSizePixelX(lDieNo) / 2);
	stCorner2.y = (PR_WORD)GetPrCenterY() + (GetDieSizePixelY(lDieNo) / 2);

	DrawRectangleBox(stCorner1, stCorner2, ssColor);
}


VOID CWaferPr::DrawAndEraseCursor(PR_COORD stStartPos, unsigned char ucCorner, BOOL bDraw)
{
	PR_COORD				stLineLength;
	PR_COORD				coPoint1, coPoint2;

	stLineLength.x = (PR_WORD)_round(50 * GetPrScaleFactor());
	stLineLength.y = (PR_WORD)_round(50 * GetPrScaleFactor());

	if (ucCorner == PR_LOWER_RIGHT) 	
	{
		stLineLength.x = -stLineLength.x;
		stLineLength.y = -stLineLength.y; 
	}

	coPoint1 = stStartPos;
	coPoint2 = stStartPos; 

	for (short i = 0; i < 2; i++)
	{
		coPoint2 = stStartPos; 
		if (i == 0)	//Draw Horz Line
		{
			coPoint2.x += stLineLength.x;
		}	
		else		//Draw Vert Line
		{
			coPoint2.y += stLineLength.y;
		}

		m_pPrGeneral->PRDrawLine(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, coPoint1, coPoint2, (bDraw == 1) ? PR_COLOR_GREEN : PR_COLOR_TRANSPARENT);
	}
}


VOID CWaferPr::DrawAndEraseLine(PR_COORD stStartPos, PR_COORD stEndPos, BOOL bDraw)
{
	m_pPrGeneral->PRDrawLine(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, stStartPos, stEndPos, (bDraw == 1) ? PR_COLOR_GREEN : PR_COLOR_TRANSPARENT);
}

VOID CWaferPr::DrawHomeCursor(PR_COORD stCenter, BOOL bClearScreen)
{
	if (bClearScreen)
	{
		m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	}
	
	CString szErrMsg;
	m_pPrGeneral->DrawHomeCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, stCenter.x, stCenter.y, szErrMsg);
}

BOOL CWaferPr::EnableMouseClickCallbackCmd(CONST BOOL bEnable)
{
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		PR_SET_ON_MOUSE_CLICK_CMD stCmd;
		PR_SET_ON_MOUSE_CLICK_RPY stRpy;

		PR_InitSetOnMouseClickCmd(&stCmd);

		if (bEnable)
		{
			stCmd.emEnable = PR_TRUE;

			m_bJoystickOn		= TRUE;
			m_bXJoystickOn		= TRUE;
			m_bYJoystickOn		= TRUE;
			
		}
		else
		{
			stCmd.emEnable = PR_FALSE;
		
			m_bJoystickOn		= FALSE;
			m_bXJoystickOn		= FALSE;
			m_bYJoystickOn		= FALSE;
		}
		
		//stCmd.emGraphicInfo = PR_NO_DISPLAY;	//PR_DISPLAY_CORNERS, PR_DISPLAY_CENTRE, PR_DISPLAY_EDGE_PT
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->m_bUseTouchScreen)
			stCmd.emButton = PR_CLICK_BUTTON_LEFT;	//v4.50A3	//MS60
		else
			stCmd.emButton = PR_CLICK_BUTTON_RIGHT; //suggest to use right click to align with wirebond and other machine

		PR_SetOnMouseClickCmd(&stCmd, MS_MOUSE_CLICK_SEND_ID, MS_MOUSE_CLICK_RECV_ID, &stRpy);
		if ((stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR) || (stRpy.stStatus.uwPRStatus != PR_ERR_NOERR))
		{
			CString szErr;
			szErr.Format("PR_SetOnMouseClickCmd ERROR: COMM=0x%x, PR=0x%x", stRpy.stStatus.uwCommunStatus, stRpy.stStatus.uwPRStatus);
			#ifdef OFFLINE
				DisplayMessage(szErr);
			#else
				AfxMessageBox(szErr, MB_SYSTEMMODAL);
			#endif
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CWaferPr::IsMouseClickModeAvailable()
{
	if ((State() == AUTO_Q) || (State() == DEMO_Q))
	{
		return FALSE;
	}
	//if (m_bCurrentCamera != WPR_CAM_WAFER)
	//	return FALSE;
	BOOL bEnable = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["MouseClick"]["Enable"];

	return TRUE;
}


VOID CWaferPr::ConvertPixelToUM(const LONG lPixelX, const LONG lPixelY, double &dXum, double &dYum)
{
	PR_COORD stPixel;

	stPixel.x = (PR_WORD)lPixelX;
	stPixel.y = (PR_WORD)lPixelY;
	ConvertPixelToUM(stPixel, dXum, dYum);
}

VOID CWaferPr::ConvertPixelToUM(PR_COORD stPixel, double &dXum, double &dYum)
{
	int siStepX, siStepY;
	ConvertPixelToMotorStep(stPixel, &siStepX, &siStepY);

	dXum = m_dWTXYRes * siStepX;
	dYum = m_dWTXYRes * siStepY;
}

VOID CWaferPr::ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY)
{
	*siStepX = (int)((DOUBLE)stPixel.x * GetCalibX() + (DOUBLE)stPixel.y * GetCalibXY());
	*siStepY = (int)((DOUBLE)stPixel.y * GetCalibY() + (DOUBLE)stPixel.x * GetCalibYX());
}

VOID CWaferPr::ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY, 
									   double dCalibX, double dCalibY, double dCalibXY, double dCalibYX)
{
	*siStepX = (int)((DOUBLE)stPixel.x * dCalibX + (DOUBLE)stPixel.y * dCalibXY);
	*siStepY = (int)((DOUBLE)stPixel.y * dCalibY + (DOUBLE)stPixel.x * dCalibYX);
}

VOID CWaferPr::ConvertMotorStepToPixel(int siStepX, int siStepY, PR_COORD &stPixel)
{
	DOUBLE dStepX = siStepX;
	DOUBLE dStepY = siStepY;
	if (GetCalibX() != 0)
	{
		stPixel.x = (PR_WORD)(dStepX / GetCalibX());
	}
	if (GetCalibY() != 0)
	{
		stPixel.y = (PR_WORD)(dStepY / GetCalibY());
	}
}

VOID CWaferPr::ConvertMotorStepToFFPixel(int siStepX, int siStepY, PR_COORD &stPixel)
{
	DOUBLE dStepX = siStepX;
	DOUBLE dStepY = siStepY;
	if (GetCalibX() != 0)
	{
		stPixel.x = (PR_WORD)(dStepX / 4*GetCalibX());
	}
	if (GetCalibY() != 0)
	{
		stPixel.y = (PR_WORD)(dStepY / 4*GetCalibY());
	}
}

VOID CWaferPr::ConvertPixelToUnit(PR_COORD stPixel, LONG *lUnitX, LONG *lUnitY, BOOL bUseUm)
{
	int		siStepX;
	int		siStepY;

	//1st Convert into motor step 
	ConvertPixelToMotorStep(stPixel, &siStepX, &siStepY);

	if (bUseUm)		//shiraishi03
	{
		DOUBLE dStepX = siStepX;
		DOUBLE dStepY = siStepY;
		//Convert motor into um	
		*lUnitX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * dStepX * 1000);
		*lUnitY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * dStepY * 1000);
	}
	else
	{
		//Convert motor into mil	
		*lUnitX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)siStepX * 1000 / 25.4);
		*lUnitY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)siStepY * 1000 / 25.4);
	}
}

VOID CWaferPr::ConvertPixelToDUnit(PR_COORD stPixel, DOUBLE &dUnitX, DOUBLE &dUnitY, BOOL bUseUm)
{
	int	siStepX = 0;
	int	siStepY = 0;

	//1st Convert into motor step 
	ConvertPixelToMotorStep(stPixel, &siStepX, &siStepY);
	
	//Convert motor into mil	
	if (bUseUm)		//shiraishi03
	{
		DOUBLE dStepX = siStepX;
		DOUBLE dStepY = siStepY;
		//dUnitX = GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * 1000.0 * dStepX;
		//dUnitY = GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * 1000.0 * dStepY;
		dUnitX = m_dWTXYRes * dStepX;
		dUnitY = m_dWTXYRes * dStepY;
	}
	else
	{
		//dUnitX = (0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)siStepX * 1000.0 / 25.4);
		//dUnitY = (0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)siStepY * 1000.0 / 25.4);
		dUnitX = (0.5 + m_dWTXYRes * (DOUBLE)siStepX / 25.4);
		dUnitY = (0.5 + m_dWTXYRes * (DOUBLE)siStepY / 25.4);
	}
}

DOUBLE CWaferPr::ConvertDUnitToPixel(CONST DOUBLE dUnit)
{
	DOUBLE dMotorStep	= 0.0;
	if (m_dWTXinUm != 0)
	{
		dMotorStep = dUnit / m_dWTXinUm / 1000.0 * 25.4;    //v4.28T5	//Used in bLineDefect in AUTOBOND whihc may slower down CT!!
	}		
	
	if (GetCalibX() != 0.0)	// divide by zero
	{
		return  dMotorStep / GetCalibX();
	}
	if (GetCalibY() != 0.0)	// divide by zero
	{
		return  dMotorStep / GetCalibY();
	}

	return 0;
}


VOID CWaferPr::ConvertMotorStepToUnit(LONG lStepX, LONG lStepY, LONG *lUnitX, LONG *lUnitY)
{
	/*if (bUseUm)	//v4.59A16	//Renesas MS90
	{
		//Convert motor into um	
		*lUnitX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE) lStepX);
		*lUnitY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE) lStepY);
	}
	else*/
	//{
	//Convert motor into mil	
	*lUnitX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)lStepX * 1000 / 25.4);
	*lUnitY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)lStepY * 1000 / 25.4);
	//}
}

VOID CWaferPr::ConvertMotorStepToDUnit(LONG lStepX, LONG lStepY, DOUBLE& dUnitX, DOUBLE& dUnitY)	//v4.59A18
{
	DOUBLE dStepX = lStepX;
	DOUBLE dStepY = lStepY;
	//dUnitX = GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * 1000.0 * dStepX;
	//dUnitY = GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * 1000.0 * dStepY;
	dUnitX = m_dWTXYRes * dStepX;
	dUnitY = m_dWTXYRes * dStepY;
}

BOOL CWaferPr::VerifyPRRegion(PR_WIN *stInputArea)
{
	return m_pPrGeneral->VerifyPRRegion(stInputArea, GetPRWinULX(), GetPRWinULY(), GetPRWinLRX(), GetPRWinLRY());
}

BOOL CWaferPr::VerifyPRRegion_MxN(PR_WIN *stInputArea)
{
	return m_pPrGeneral->VerifyPRRegion(stInputArea, GetPRWinULX(), GetPRWinULY(), GetPRWinLRX(), GetPRWinLRY());
}

BOOL CWaferPr::VerifyPRRegion_MxN(PR_COORD &stCorner1, PR_COORD &stCorner2)
{
	PR_WIN stInputArea;

	stInputArea.coCorner1 = stCorner1;
	stInputArea.coCorner2 = stCorner2;
	BOOL bOK = m_pPrGeneral->VerifyPRRegion(&stInputArea, GetPRWinULX(), GetPRWinULY(), GetPRWinLRX(), GetPRWinLRY());
	stCorner1 = stInputArea.coCorner1;
	stCorner2 = stInputArea.coCorner2;

	return bOK;
}

BOOL CWaferPr::VerifyPRPoint(PR_COORD *stPoint)
{
	return m_pPrGeneral->VerifyPRPoint(stPoint, GetPRWinULX(), GetPRWinULY(), GetPRWinLRX(), GetPRWinLRY());
}


VOID CWaferPr::CalculateDieCompenate(PR_COORD stDieOffset, int *siStepX, int *siStepY)
{
	PR_COORD	stRelMove;

	stRelMove.x = (PR_WORD)GetPrCenterX() - stDieOffset.x;
	stRelMove.y = (PR_WORD)GetPrCenterY() - stDieOffset.y;

	ConvertPixelToMotorStep(stRelMove, siStepX, siStepY);
}

VOID CWaferPr::CalculateNDieOffsetXY(INT &siStepX, INT &siStepY)	//3.77 Cree
{
	if ((m_lNDieOffsetInMotorStepX != 0))
	{
		siStepX = siStepX + m_lNDieOffsetInMotorStepX;			//all in motor steps
	}
	if ((m_lNDieOffsetInMotorStepY != 0))
	{
		siStepY = siStepY + m_lNDieOffsetInMotorStepY;			//all in motor steps
	}
}

BOOL CWaferPr::MoveWaferTable(int siXAxis, int siYAxis)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} RELPOS;

	RELPOS stPos;

	int nConvID = 0;

	stPos.lX = siXAxis;
	stPos.lY = siYAxis;

	stMsg.InitMessage(sizeof(RELPOS), &stPos);
	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	if (pWaferTable != NULL)
	{
		pWaferTable->XY_SafeMoveToCmd(stMsg);
	}

	BOOL bReturn = TRUE;
	stMsg.GetMsg(sizeof(BOOL), &bReturn);

	if (!IsAOIOnlyMachine())
	{
		Sleep(20);
	}

	return bReturn;
}


VOID CWaferPr::GetWaferTableEncoder(int *siXAxis, int *siYAxis, int *siTAxis)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lT;
	} ENCVAL;

	ENCVAL stEnc;

	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	if (pWaferTable != NULL)
	{
		pWaferTable->GetEncoderCmd(stMsg);
		stMsg.GetMsg(sizeof(ENCVAL), &stEnc);
			
		*siXAxis = stEnc.lX;
		*siYAxis = stEnc.lY;
		*siTAxis = stEnc.lT;
	}
}

VOID CWaferPr::GetWaferTableCommander(int *siXAxis, int *siYAxis, int *siTAxis)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lT;
	} ENCVAL;

	ENCVAL stCmd;

	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	if (pWaferTable != NULL)
	{
		pWaferTable->GetCommanderCmd(stMsg);
		stMsg.GetMsg(sizeof(ENCVAL), &stCmd);
			
		*siXAxis = stCmd.lX;
		*siYAxis = stCmd.lY;
		*siTAxis = stCmd.lT;
	}
}

VOID CWaferPr::SetWaferTableJoystick(BOOL bState, BOOL bWT2)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	//v3.71T9
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		if( IsESDualWT() && bWT2 )
		{
			m_lJsWftInUse = 1;	//WT2	
		}
		else
		{
			m_lJsWftInUse = 0;	//WT1
		}
		if (GetLockPrMouseJoystick() == TRUE)
		{
			bState = FALSE;
		}

		m_bJoystickOn	= bState;
		m_bXJoystickOn	= bState;
		m_bYJoystickOn	= bState;
		m_lJsTableMode = (*m_psmfSRam)["MS896A"]["Current Camera"];

		EnableMouseClickCallbackCmd(bState);

		return;
	}

	stMsg.InitMessage(sizeof(BOOL), &bState);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetJoystickCmd", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
}

VOID CWaferPr::SetBinTableJoystick(BOOL bState)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	//v3.71T9
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		m_bJoystickOn	= bState;
		m_bXJoystickOn	= bState;
		m_bYJoystickOn	= bState;
		return;
	}
	
	stMsg.InitMessage(sizeof(BOOL), &bState);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetJoystickCmd", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
}

VOID CWaferPr::UseWaferTableJoystick(BOOL bState)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	//v3.71T9
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetWaferTableJoystick(bState);
	//	EnableMouseClickCallbackCmd(bState);
		return;
	}

	stMsg.InitMessage(sizeof(BOOL), &bState);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "UseJoystickCmd", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
}

VOID CWaferPr::UseBinTableJoystick(BOOL bState)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	//v3.71T9
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		EnableMouseClickCallbackCmd(bState);
		return;
	}

	stMsg.InitMessage(sizeof(BOOL), &bState);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "UseJoystickCmd", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
}

VOID CWaferPr::SetWaferTableJoystickSpeed(LONG lLevel)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	//v3.71T9
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		return;
	}

	stMsg.InitMessage(sizeof(LONG), &lLevel);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetJoystickSpeedCmd", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
}


VOID CWaferPr::SetBinTableJoystickSpeed(LONG lLevel)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	//v3.71T9
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		return;
	}

	stMsg.InitMessage(sizeof(LONG), &lLevel);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetJoystickSpeedCmd", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
}


VOID CWaferPr::ManualDieCompenate(PR_COORD stDieOffset, PR_REAL fDieRotate)
{
	int			siStepX;
	int			siStepY;
	int			siOrigX;
	int			siOrigY;
	int			siOrigT;

	CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	CalculateNDieOffsetXY(siStepX, siStepY);		//v3.77
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

	siOrigX += siStepX;
	siOrigY += siStepY;

	MoveWaferTable(siOrigX, siOrigY);
	return;
}


VOID CWaferPr::ManualDieCompenate_Rotate(PR_COORD stDieOffset, PR_REAL fDieRotate)
{
	int			siStepX;
	int			siStepY;
	int			siOrigX;
	int			siOrigY;
	int			siOrigT;

	CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	CalculateNDieOffsetXY(siStepX, siStepY);		//v3.77
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

	siOrigX += siStepX;
	siOrigY += siStepY;

	WprCompensateRotateDie(siOrigX, siOrigY, (DOUBLE)fDieRotate);
}

BOOL CWaferPr::WprCompensateRotateDie(LONG siOrigX, LONG siOrigY, DOUBLE fDieRotate)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG	lX;
		LONG	lY;
		LONG	lT;
		DOUBLE	dDegree;
	} ENCVAL;

	ENCVAL stEnc;

	stEnc.lX = siOrigX;
	stEnc.lY = siOrigY;
	stEnc.lT = 0;
	stEnc.dDegree = (DOUBLE)(fDieRotate);

	stMsg.InitMessage(sizeof(ENCVAL), &stEnc);

	// Get the reply for the encoder value
	int nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "RotateAngularDie", stMsg);
	BOOL bResult = TRUE;

	//v3.48		//andrew	//Wait for reply before porceeding
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


LONG CWaferPr::FindDieCalibration(VOID)
{
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();

	if( IsDP() )
	{
		if( IsDP_ScanCam() && GetPrescanPrID()==3 )	//	Dual Path
		{
			return FindScanCalibration();
		}	// dual path die calibration
	}
	else
	{
		if( GetRunZoom()==GetScnZoom() && GetScnZoom()!=GetNmlZoom() && GetPrescanPrID()==3 )
		{
//			if( HmiMessage("Do you want to find scan die calibration?", "Die Calibration", glHMI_MBX_YESNO)==glHMI_YES )
			{
				return FindScanCalibration();
			}
		}
	}

	PR_UWORD		usDieType;
	PR_COORD		stDiePosition[2]; 
	PR_COORD		stDieShiftX, stDieShiftY; 
	PR_COORD		stXPixelMove; 
	PR_COORD		stYPixelMove; 
	PR_WIN			stOrgSearchArea = m_stSearchArea;
	PR_WIN			stDieSizeArea;
	PR_REAL			fDieRotate; 
	
	int				siSrchMargin = (32 * GetPrScaleFactor());
	int				siTempData1 = 0;
	int				siTempData2 = 0;
	int				siThetaPos;
	short			ssCycle;			
	float			fTemp = 0.0;
	BOOL			bTempCorrection = m_bThetaCorrection;
	PR_WORD			uwResult;
	CString szMsg;

	typedef struct 
	{
		int XPos;
		int YPos;
	} MOTORDATA;

	MOTORDATA	stDestX, stDestY;
	MOTORDATA	stLastPos, stTempPos;
	MOTORDATA	stTablePos[2];
	MOTORDATA	stXMotorMove, stYMotorMove;


	PR_WORD wPixelPitchX = (PR_WORD)(1.3 * GetNmlSizePixelX());
	PR_WORD wPixelPitchY = (PR_WORD)(1.3 * GetNmlSizePixelY());

	//Update Search area
	m_stSearchArea.coCorner1.x = (PR_WORD)GetPrCenterX() - wPixelPitchX;
	m_stSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - wPixelPitchY;
	m_stSearchArea.coCorner2.x = (PR_WORD)GetPrCenterX() + wPixelPitchX;
	m_stSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + wPixelPitchY;

	VerifyPRRegion(&m_stSearchArea);

	stDieSizeArea = m_stSearchArea;
	DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_GREEN);

	//Temp to dsiable  Theta Correction
	m_bThetaCorrection = FALSE;

	//Store Current position	
	GetWaferTableEncoder(&stLastPos.XPos, &stLastPos.YPos, &siThetaPos);

	LONG lReturn = 0;
	//Start learning
	for (ssCycle	= 0; ssCycle <= 2; ssCycle++)
	{
		szMsg.Format("Die Calibration cycle %d", ssCycle);
		SetAlarmLog(szMsg);
		//Search Die on current position
		m_stSearchArea = stDieSizeArea;

		uwResult = ManualSearchNormalDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_1ST_DIE;
			break;
		}

		if (ssCycle == 0)
		{
			stDestX.XPos = m_lPrCal1stStepSize;			//v2.89T1
			stDestX.YPos = 0;     
			stDestY.YPos = m_lPrCal1stStepSize;			//v2.89T1
			stDestY.XPos = 0;     
		}
		else
		{
			//Only update search area & calculate motor step on cycle 1 & 2 
			if (IsEnableZoom())
			{
				siTempData1 = (int)(stDiePosition[0].x - wPixelPitchX / 2 - GetPRWinULX() - siSrchMargin);
				siTempData2 = (int)(GetPRWinLRX() - (stDiePosition[0].x + wPixelPitchX / 2) - siSrchMargin);	
			}
			else
			{
				siTempData1 = (int)(stDiePosition[0].x - wPixelPitchX / 2 - GetPRWinULX() - siSrchMargin);
				siTempData2 = (int)(GetPRWinLRX() - (stDiePosition[0].x + wPixelPitchX / 2) - siSrchMargin);
			}

			if (ssCycle != 3)	// divide by zero
			{
				stDieShiftX.x = abs((min(siTempData1, siTempData2)) / (3 - ssCycle));
			}
			stDieShiftX.y = 0;
			ConvertPixelToMotorStep(stDieShiftX, &stDestX.XPos, &stDestX.YPos);

			siTempData1 = (int)(stDiePosition[0].y - wPixelPitchY / 2 - GetPRWinULY() - siSrchMargin);
			siTempData2 = (int)(GetPRWinLRY() - (stDiePosition[0].y + wPixelPitchY / 2) - siSrchMargin);

			stDieShiftY.x = 0;
			if (ssCycle != 3)	// divide by zero
			{
				stDieShiftY.y = abs((min(siTempData1, siTempData2)) / (3 - ssCycle));
			}
			ConvertPixelToMotorStep(stDieShiftY, &stDestY.XPos, &stDestY.YPos);
		}

		//Get current motor pos to calculate next postion
		GetWaferTableEncoder(&stTempPos.XPos, &stTempPos.YPos, &siThetaPos);

		// Move Table X + direction & search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_TRANSPARENT);
			DrawSearchBox( PR_COLOR_TRANSPARENT);

			//Update Search die area
			m_stSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - wPixelPitchY;
			m_stSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + wPixelPitchY;
			m_stSearchArea.coCorner1.x = (PR_WORD)GetPrCenterX() - wPixelPitchX + stDieShiftX.x;
			m_stSearchArea.coCorner2.x = (PR_WORD)GetPrCenterX() + wPixelPitchX + stDieShiftX.x;
			VerifyPRRegion(&m_stSearchArea);
			DrawSearchBox(PR_COLOR_GREEN);
		}

		// Move table to + direction
		BOOL bWftMove = MoveWaferTable(stTempPos.XPos + stDestX.XPos, stTempPos.YPos + stDestX.YPos);
		Sleep(1000);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		GetWaferTableEncoder(&stTablePos[0].XPos, &stTablePos[0].YPos, &siThetaPos);
		//Search PR		
		uwResult = ManualSearchNormalDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}

		// Move Table X - direction & search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_TRANSPARENT);
			DrawSearchBox( PR_COLOR_TRANSPARENT);

			//Update Search die area
			m_stSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - wPixelPitchY;
			m_stSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + wPixelPitchY;
			m_stSearchArea.coCorner1.x = (-stDieShiftX.x) + (PR_WORD)GetPrCenterX() - wPixelPitchX;
			m_stSearchArea.coCorner2.x = (-stDieShiftX.x) + (PR_WORD)GetPrCenterX() + wPixelPitchX;
			VerifyPRRegion(&m_stSearchArea);
			DrawSearchBox(PR_COLOR_GREEN);
		}
		//Move table to - direction
		bWftMove = MoveWaferTable(stTempPos.XPos - stDestX.XPos, stTempPos.YPos - stDestX.YPos);
		Sleep(1000);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		GetWaferTableEncoder(&stTablePos[1].XPos, &stTablePos[1].YPos, &siThetaPos);
		//Search PR		
		uwResult = ManualSearchNormalDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[1], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_RT_DIE;
			break;
		}

		//Get Both X Value
		stXMotorMove.XPos	= stTablePos[1].XPos - stTablePos[0].XPos;
		stXMotorMove.YPos	= stTablePos[1].YPos - stTablePos[0].YPos;
		stXPixelMove.x		= stDiePosition[1].x - stDiePosition[0].x;
		stXPixelMove.y		= stDiePosition[1].y - stDiePosition[0].y;
		szMsg.Format("WFT X    X %d -- %d, Y %d -- %d;		WPR X    X %d -- %d, Y %d -- %d", 
					 stTablePos[1].XPos, stTablePos[0].XPos, stTablePos[1].YPos, stTablePos[0].YPos,
					 stDiePosition[1].x, stDiePosition[0].x, stDiePosition[1].y, stDiePosition[0].y);
		SetAlarmLog(szMsg);
		szMsg.Format("Table X Diff: %d,%d; WPR Y Diff: %d,%d", 
					 stXMotorMove.XPos, stXMotorMove.YPos, stXPixelMove.x, stXPixelMove.y);
		SetAlarmLog(szMsg);

		//Move table back to start poistion
		DrawSearchBox( PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_GREEN);


		//Move Table Y + direction search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_TRANSPARENT);
			DrawSearchBox( PR_COLOR_TRANSPARENT);

			//Update Search die area
			m_stSearchArea.coCorner1.x	= (PR_WORD)GetPrCenterX() - wPixelPitchX;
			m_stSearchArea.coCorner2.x	= (PR_WORD)GetPrCenterX() + wPixelPitchX;
			m_stSearchArea.coCorner1.y	= (stDieShiftY.y) + (PR_WORD)GetPrCenterY() - wPixelPitchY;
			m_stSearchArea.coCorner2.y	= (stDieShiftY.y) + (PR_WORD)GetPrCenterY() + wPixelPitchY;
			VerifyPRRegion(&m_stSearchArea);
			DrawSearchBox(PR_COLOR_GREEN);
		}
		//Move table to + direction
		bWftMove = MoveWaferTable(stTempPos.XPos + stDestY.XPos, stTempPos.YPos + stDestY.YPos);
		Sleep(1000);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		GetWaferTableEncoder(&stTablePos[0].XPos, &stTablePos[0].YPos, &siThetaPos);
		//Search PR		
		uwResult = ManualSearchNormalDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_UP_DIE;
			break;
		}

		//Move Table Y - direction & search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_TRANSPARENT);
			DrawSearchBox( PR_COLOR_TRANSPARENT);

			//Update Search die area
			m_stSearchArea.coCorner1.x	= (PR_WORD)GetPrCenterX() - wPixelPitchX;
			m_stSearchArea.coCorner2.x	= (PR_WORD)GetPrCenterX() + wPixelPitchX;
			m_stSearchArea.coCorner1.y	= (-stDieShiftY.y) + (PR_WORD)GetPrCenterY() - wPixelPitchY;
			m_stSearchArea.coCorner2.y	= (-stDieShiftY.y) + (PR_WORD)GetPrCenterY() + wPixelPitchY;
			VerifyPRRegion(&m_stSearchArea);
			DrawSearchBox(PR_COLOR_GREEN);
		}
		//Move table to - direction
		bWftMove = MoveWaferTable(stTempPos.XPos - stDestY.XPos, stTempPos.YPos - stDestY.YPos);
		Sleep(1000);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		GetWaferTableEncoder(&stTablePos[1].XPos, &stTablePos[1].YPos, &siThetaPos);
		//Search PR		
		uwResult = ManualSearchNormalDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[1], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_DN_DIE;
			break;
		}

		//Get Both Y Value
		stYMotorMove.XPos	= stTablePos[1].XPos - stTablePos[0].XPos;
		stYMotorMove.YPos	= stTablePos[1].YPos - stTablePos[0].YPos;
		stYPixelMove.x		= stDiePosition[1].x - stDiePosition[0].x;
		stYPixelMove.y		= stDiePosition[1].y - stDiePosition[0].y;
		szMsg.Format("WFT Y    X %d -- %d, Y %d -- %d;		WPR Y    X %d -- %d, Y %d -- %d", 
					 stTablePos[1].XPos, stTablePos[0].XPos, stTablePos[1].YPos, stTablePos[0].YPos, 
					 stDiePosition[1].x, stDiePosition[0].x, stDiePosition[1].y, stDiePosition[0].y);
		SetAlarmLog(szMsg);
		szMsg.Format("Table Y Diff: %d,%d; WPR Y Diff: %d,%d", 
					 stYMotorMove.XPos, stYMotorMove.YPos, stYPixelMove.x, stYPixelMove.y);
		SetAlarmLog(szMsg);


		// Check for zero error!
		fTemp = (float)(stXPixelMove.x * stYPixelMove.y - stYPixelMove.x * stXPixelMove.y);
		if (fabs(fTemp) < 0.000001)
		{
			lReturn = WPR_ERR_CALIB_ZERO_VALUE;
			break;
		}

		DOUBLE  dCalibX = 0, dCalibXY = 0, dCalibY = 0, dCalibYX = 0;
		dCalibX	= (DOUBLE)(stXMotorMove.XPos * stYPixelMove.y - stYMotorMove.XPos * stXPixelMove.y) / fTemp;
		if (stYPixelMove.y != 0)
		{
			dCalibXY = ((DOUBLE)stYMotorMove.XPos - dCalibX * (DOUBLE)stYPixelMove.x) / (DOUBLE)stYPixelMove.y;
		}
		else
		{
			dCalibXY = 0.0;
		}
		dCalibY	= (DOUBLE)(stYMotorMove.YPos * stXPixelMove.x - stXMotorMove.YPos * stYPixelMove.x) / fTemp;
		if (stXPixelMove.x != 0)
		{
			dCalibYX = ((DOUBLE)stXMotorMove.YPos - dCalibY * (DOUBLE)stXPixelMove.y) / (DOUBLE)stXPixelMove.x;
		}
		else
		{
			dCalibYX = 0.0;
		}

		if( !IsEnableZoom() )
		{
			m_dCalibX	= dCalibX;	
			m_dCalibY	= dCalibY;		
			m_dCalibXY	= dCalibXY;
			m_dCalibYX	= dCalibYX;
		}
		else 
		{
			if( IsDP() )
			{
				if( IsDP_ScanCam() )
				{
					SetZoomViewCalibration(dCalibX, dCalibY, dCalibXY, dCalibYX);
					if (GetRunZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF)
					{
						SetFFCalibration(dCalibX, dCalibY, dCalibXY, dCalibYX);
					}
				}
				else
				{
					m_dCalibX	= dCalibX;	
					m_dCalibY	= dCalibY;		
					m_dCalibXY	= dCalibXY;
					m_dCalibYX	= dCalibYX;
				}
			}
			else
			{
				if( GetRunZoom()==GetScnZoom() )
				{
					SetZoomViewCalibration(dCalibX, dCalibY, dCalibXY, dCalibYX);
					if (GetRunZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF)
					{
						SetFFCalibration(dCalibX, dCalibY, dCalibXY, dCalibYX);
					}
				}
				if( GetRunZoom()==GetNmlZoom() )
				{
					m_dCalibX	= dCalibX;	
					m_dCalibY	= dCalibY;		
					m_dCalibXY	= dCalibXY;
					m_dCalibYX	= dCalibYX;
				}
			}
		}

		szMsg.Format("Calibration X %.4f,%.4f; Y %.4f,%.4f; factor %.4f,ZoomFactor,%d", dCalibX, dCalibXY, dCalibY, dCalibYX, fTemp,GetRunZoom());
		SetAlarmLog(szMsg);

		PR_RSIZE		stFov;
		PR_OPTIC		stOptic;
		PR_COMMON_RPY	stComRpy;
		stFov.x = (PR_REAL)(GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * GetCalibX() * PR_MAX_COORD);
		stFov.y = (PR_REAL)(GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * GetCalibY() * PR_MAX_COORD);
		// Set FOV to Wafer PR
		PR_GetOptic(ubPpsG, ubSID, ubRID, &stOptic, &stComRpy);
		PR_SetFov(&stFov, ubSID, ubRID, &stOptic, &stComRpy);

		//Move table back to start poistion & do PR & update start position
		DrawSearchBox( PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_GREEN);

		bWftMove = MoveWaferTable(stLastPos.XPos, stLastPos.YPos);
		Sleep(1000);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		m_stSearchArea = stDieSizeArea;
		ManualSearchNormalDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], m_stSearchArea);
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate(stDiePosition[0], fDieRotate);
			Sleep(1000);
			GetWaferTableEncoder(&stLastPos.XPos, &stLastPos.YPos, &siThetaPos);
		}
	}

	if (lReturn == 0)
	{
		m_bDieCalibrated = TRUE;
	}

	m_stSearchArea = stOrgSearchArea;
	m_bThetaCorrection = bTempCorrection;


	return lReturn;
}


LONG CWaferPr::FindDiePitch(BOOL bAutoLearn)
{
	PR_UWORD		usDieType;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	int				siThetaPos;
	short			ssTryCount = 0;
	PR_WIN			stOrgSearchArea = m_stSearchArea;
	BOOL			bTempCorrection = m_bThetaCorrection;
	CString			szText, szTitle;
	BOOL			bManaulLearnOK = FALSE;
	int				nFlipX = 1;
	int				nFlipY = 1;
	CTime			ctCurTime(CTime::GetCurrentTime());
	PR_WORD			wResult;

	typedef struct 
	{
		int XPos;
		int YPos;
	} MOTORDATA;

	MOTORDATA	stTablePos[7];
	MOTORDATA	stCurPos;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//update search die area
	if( (GetDieShape() == WPR_RECTANGLE_DIE) || (GetDieShape() == WPR_RHOMBUS_DIE) )
	{
		m_stSearchArea.coCorner1.x = (PR_WORD)GetPrCenterX() - (PR_WORD)(1.3 * GetNmlSizePixelX());
		m_stSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - (PR_WORD)(1.3 * GetNmlSizePixelY());
		m_stSearchArea.coCorner2.x = (PR_WORD)GetPrCenterX() + (PR_WORD)(1.3 * GetNmlSizePixelX());
		m_stSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + (PR_WORD)(1.3 * GetNmlSizePixelY());
	}
	else
	{
		m_stSearchArea.coCorner1.x = (PR_WORD)GetPrCenterX() - (PR_WORD)(1.0 * GetNmlSizePixelX());
		m_stSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - (PR_WORD)(1.0 * GetNmlSizePixelY());
		m_stSearchArea.coCorner2.x = (PR_WORD)GetPrCenterX() + (PR_WORD)(1.0 * GetNmlSizePixelX());
		m_stSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + (PR_WORD)(1.0 * GetNmlSizePixelY());
	}

	VerifyPRRegion(&m_stSearchArea);
	DrawSearchBox(PR_COLOR_GREEN);

	CMSLogFileUtility::Instance()->WPR_LearnPitchLog("***Log File For Learn Die Pitch***");
	CMSLogFileUtility::Instance()->WPR_LearnPitchLog(ctCurTime.Format("Data Created Time:%d/%m/%Y %H:%M:%S"));
	CMSLogFileUtility::Instance()->WPR_LearnPitchLog("\nDie Position:");

	//temp to disable Theta Correction
	m_bThetaCorrection = FALSE;

	szTitle.LoadString(HMB_WPR_MANUAL_DIE_PITCH);

	if (bAutoLearn == FALSE)
	{
		for (ssTryCount = 0; ssTryCount < 3; ssTryCount++)
		{
			SetWaferTableJoystick(TRUE);
			bManaulLearnOK = TRUE;
	
			switch (ssTryCount)
			{
			default:
				szText.LoadString(HMB_WPR_MLRN_PITCH_STEP_1);
				break;

			case 1:
				szText.LoadString(HMB_WPR_MLRN_PITCH_STEP_2);
				break;

			case 2:
				szText.LoadString(HMB_WPR_MLRN_PITCH_STEP_3);
				break;
			}


			if (HmiMessageEx(szText, szTitle, glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_CANCEL)
			{
				bManaulLearnOK = FALSE;
			}
			else
			{
				SetWaferTableJoystick(FALSE);
				bManaulLearnOK = TRUE;
			}

			if (bManaulLearnOK == FALSE)
			{
				m_stSearchArea = stOrgSearchArea;
				m_bThetaCorrection = bTempCorrection;
				switch (ssTryCount)
				{
				default:	
					return WPR_ERR_PITCH_NO_1ST_DIE;
				case 1: 	
					return WPR_ERR_PITCH_NO_X_DIE;
				case 2: 	
					return WPR_ERR_PITCH_NO_Y_DIE;
				}
			}

			GetWaferTableEncoder(&stTablePos[ssTryCount].XPos, &stTablePos[ssTryCount].YPos, &siThetaPos);
		}
	}
	else
	{
		INT lDieSizeX_X = GetDieSizeX();
		INT lDieSizeX_Y = 0;
		INT lDieSizeY_Y = GetDieSizeY();
		INT lDieSizeY_X = 0;
		//Try to locate 1st die
		wResult =    ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2);
		if (wResult == -1 || (DieIsAlignable(usDieType) == FALSE) )
		{
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			m_stSearchArea = stOrgSearchArea;
			m_bThetaCorrection = bTempCorrection;
			return WPR_ERR_PITCH_NO_1ST_DIE;
		}

		ManualDieCompenate(stDieOffset, fDieRotate);
		GetWaferTableEncoder(&stTablePos[0].XPos, &stTablePos[0].YPos, &siThetaPos);

		if( GetDieShape() == WPR_RHOMBUS_DIE )
		{
			BOOL bCornerOK = FALSE;
			if( AutoDetectDieCorner(WPR_NORMAL_DIE, 1)==PR_ERR_NOERR )
			{
				bCornerOK = TRUE;
				CMSLogFileUtility::Instance()->WPR_LearnPitchLog("Auto detect corner ok");
			}
			else
			{
				ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, TRUE);
				if (wResult != -1 && DieIsAlignable(usDieType) )
				{
					CMSLogFileUtility::Instance()->WPR_LearnPitchLog("auto search die corner ok");
					bCornerOK = TRUE;
				}
			}

			if( bCornerOK )
			{
				// get the corner and draw.
				INT nUR_X	= m_stLearnDieCornerPos[PR_UPPER_RIGHT].x;
				INT nUR_Y	= m_stLearnDieCornerPos[PR_UPPER_RIGHT].y;
				INT nUL_X	= m_stLearnDieCornerPos[PR_UPPER_LEFT].x;
				INT nUL_Y	= m_stLearnDieCornerPos[PR_UPPER_LEFT].y;
				INT nLL_X	= m_stLearnDieCornerPos[PR_LOWER_LEFT].x;
				INT nLL_Y	= m_stLearnDieCornerPos[PR_LOWER_LEFT].y;
				INT nLR_X	= m_stLearnDieCornerPos[PR_LOWER_RIGHT].x;
				INT nLR_Y	= m_stLearnDieCornerPos[PR_LOWER_RIGHT].y;
				CString szMsg;
				szMsg.Format("CORNER UR %d,%d UL %d,%d LL %d,%d LR %d,%d", nUR_X, nUR_Y, nUL_X, nUL_Y, nLL_X, nLL_Y, nLR_X, nLR_Y);
				CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szMsg);

				int nMaxX = max(nUR_X, nLR_X);
				nMaxX = max(nMaxX, nUL_X);
				nMaxX = max(nMaxX, nLL_X);
				int nMinX = min(nUR_X, nLR_X);
				nMinX = min(nMinX, nUL_X);
				nMinX = min(nMinX, nLL_X);
				// update the new search area based on the die size dimension
				INT nSizeX	= nMaxX - nMinX;
				INT nSizeY	= (nLR_Y + nLL_Y - nUR_Y - nUL_Y)/2;
				m_stSearchArea.coCorner1.x = (PR_WORD)GetPrCenterX() - (PR_WORD)(1.3 * nSizeX);
				m_stSearchArea.coCorner2.x = (PR_WORD)GetPrCenterX() + (PR_WORD)(1.3 * nSizeX);
				m_stSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - (PR_WORD)(1.3 * nSizeY);
				m_stSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + (PR_WORD)(1.3 * nSizeY);
				VerifyPRRegion(&m_stSearchArea);
				DrawSearchBox( PR_COLOR_YELLOW);

				// calculate index steps in both main and delta
				INT nDeltaX	= (nUR_X - nUL_X + nLR_X - nLL_X)/2*12/10;
				INT nDeltaY = 0;
				PR_COORD	stTmpPixel;
				stTmpPixel.x = nDeltaX;
				stTmpPixel.y = nDeltaY;
				ConvertPixelToMotorStep(stTmpPixel, &lDieSizeX_X, &lDieSizeX_Y);
				CString szTemp;
				szTemp.Format("move x %d,%d", lDieSizeX_X, lDieSizeX_Y);
				CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szTemp);
				nDeltaX = (nLL_X - nUL_X + nLR_X - nUR_X)/2*12/10;
				nDeltaY	= (nLL_Y - nUL_Y + nLR_Y - nUR_Y)/2*12/10;
				stTmpPixel.x = nDeltaX;
				stTmpPixel.y = nDeltaY;
				ConvertPixelToMotorStep(stTmpPixel, &lDieSizeY_X, &lDieSizeY_Y);
				szTemp.Format("move Y %d,%d", lDieSizeY_Y, lDieSizeY_X);
				CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szTemp);
			}
			else
			{
				HmiMessage("auto get die corner fail, please manual learn pitch.");
			}
		}

		short sTryLimitX = 3, sTryLimitY = 3;
		bool  bTryMoreX = false, bTryMoreY = false;
		//Do learn X pitch
		stCurPos = stTablePos[0];

		ssTryCount = 0;
		while (1)
		{
			ssTryCount++;

			stCurPos.XPos += lDieSizeX_X;
			stCurPos.YPos += lDieSizeX_Y;
			Sleep(1000);	//v4.15T8
			MoveWaferTable(stCurPos.XPos, stCurPos.YPos);
			Sleep(1000);	//v4.15T8

			ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2);
			if (DieIsAlignable(usDieType) == TRUE)
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
				GetWaferTableEncoder(&stTablePos[1].XPos, &stTablePos[1].YPos, &siThetaPos);
				ssTryCount = 0;
				break;
			}

			if (ssTryCount >= sTryLimitX)
			{
				DrawSearchBox( PR_COLOR_TRANSPARENT);
				m_stSearchArea = stOrgSearchArea;
				m_bThetaCorrection = bTempCorrection;
				return WPR_ERR_PITCH_NO_X_DIE;
			}
		}

		//Do learn Y pitch
		stCurPos = stTablePos[1];

		ssTryCount = 0;
		while (1)
		{
			ssTryCount++;

			stCurPos.XPos += lDieSizeY_X;
			stCurPos.YPos += lDieSizeY_Y;
			Sleep(1000);	//v4.15T8
			MoveWaferTable(stCurPos.XPos, stCurPos.YPos);
			Sleep(1000);	//v4.15T8

			ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2);
			
			if (DieIsAlignable(usDieType) == TRUE)
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
				GetWaferTableEncoder(&stTablePos[2].XPos, &stTablePos[2].YPos, &siThetaPos);
				ssTryCount = 0;
				break;
			}

			if (ssTryCount >= sTryLimitY)
			{
				DrawSearchBox( PR_COLOR_TRANSPARENT);
				m_stSearchArea = stOrgSearchArea;
				m_bThetaCorrection = bTempCorrection;
				return WPR_ERR_PITCH_NO_Y_DIE;
			}
		}
	}

	//Move back to Oringinal Position
	MoveWaferTable(stTablePos[0].XPos, stTablePos[0].YPos);
	Sleep(1000);

	if ( GetDieShape() != WPR_RHOMBUS_DIE )
	{
		if (abs(stTablePos[1].XPos - stTablePos[0].XPos) < GetDieSizeX())
		{
			m_stSearchArea = stOrgSearchArea;
			m_bThetaCorrection = bTempCorrection;
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			return WPR_ERR_PITCH_NO_X_DIE;
		}

		if (abs(stTablePos[2].YPos - stTablePos[1].YPos) < GetDieSizeY())
		{
			m_stSearchArea = stOrgSearchArea;
			m_bThetaCorrection = bTempCorrection;
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			return WPR_ERR_PITCH_NO_Y_DIE;
		}
	}	//	no check for rhombus die


	//xyz
	//Check with Master pitch before final update die pitch
	//Calculate die pitch in motor step
	INT nDiePitchXX = nFlipX * (stTablePos[1].XPos - stTablePos[0].XPos);				
	INT nDiePitchXY = nFlipX * (stTablePos[1].YPos - stTablePos[0].YPos);				
	INT nDiePitchYX = nFlipY * (stTablePos[2].XPos - stTablePos[1].XPos);				
	INT nDiePitchYY = nFlipY * (stTablePos[2].YPos - stTablePos[1].YPos);

	if( m_lLearnPitchSpanX>1 || m_lLearnPitchSpanY>1 )
	{
		LONG lOldX_X = GetDiePitchX_X();
		LONG lOldX_Y = GetDiePitchX_Y();
		LONG lOldY_Y = GetDiePitchY_Y();
		LONG lOldY_X = GetDiePitchY_X();

		(*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"] = (LONG)(nDiePitchXX);
		(*m_psmfSRam)["WaferPr"]["DiePitchX"]["Y"] = (LONG)(nDiePitchXY);
		(*m_psmfSRam)["WaferPr"]["DiePitchY"]["X"] = (LONG)(nDiePitchYX);
		(*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"] = (LONG)(nDiePitchYY);

		PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
		PR_COORD	stDieOffset; 
		PR_REAL		fDieRotate; 
		PR_REAL		fDieScore;

		int		siTempX = 0, siTempY = 0, siOrigX = 0, siOrigY = 0, siOrigT = 0;
		int		siLeftDieX = 0, siLeftDieY = 0, siTopDieX = 0,  siTopDieY = 0;
		LONG	i;

		PR_WORD wResult = 0;

		//Save original position
		GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
		siTempX = siOrigX;
		siTempY = siOrigY;

		LONG lHoriFind = 0;
		for (i = 0; i < m_lLearnPitchSpanX; i++)
		{
			MoveWaferTable(siTempX + GetDiePitchX_X(), siTempY + GetDiePitchX_Y());

			//Search die at curr position
			wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, 
										&usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
										m_stSearchArea.coCorner1, m_stSearchArea.coCorner2);

			if (wResult == -1)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("PR error when find left hand wafer angle");
				break;
			}

			if (DieIsAlignable(usDieType) == TRUE)
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
				Sleep(20);
				GetWaferTableEncoder(&siTempX, &siTempY, &siOrigT);
				siLeftDieX = siTempX;
				siLeftDieY = siTempY;
				lHoriFind++;
			}
			else
			{
				break;
			}
			Sleep(50);
		}

		//Move to original position
		MoveWaferTable(siOrigX, siOrigY);
		siTempX = siOrigX;
		siTempY = siOrigY;

		LONG lVertFind = 0;
		for (i = 0; i < m_lLearnPitchSpanY; i++)
		{
			MoveWaferTable(siTempX + GetDiePitchY_X(), siTempY + GetDiePitchY_Y());
			Sleep(1000);

			wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2);

			if (wResult == -1)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("PR error find right hand die wafer angle");
				break;
			}

			if (DieIsAlignable(usDieType) == TRUE)
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
				Sleep(20);
				GetWaferTableEncoder(&siTempX, &siTempY, &siOrigT);
				siTopDieX = siTempX;
				siTopDieY = siTempY;
			}
			else
			{
				break;
			}
			Sleep(50);
		}

		MoveWaferTable(siOrigX, siOrigY);

		//Check with Master pitch before final update die pitch
		//Calculate die pitch in motor step
		if( lHoriFind>1 )
		{
			nDiePitchXX = (siLeftDieX - siOrigX)/lHoriFind;
			nDiePitchXY = (siLeftDieY - siOrigY)/lHoriFind;
		}

		(*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"] = lOldX_X;
		(*m_psmfSRam)["WaferPr"]["DiePitchX"]["Y"] = lOldX_Y;
		(*m_psmfSRam)["WaferPr"]["DiePitchY"]["X"] = lOldY_X;
		(*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"] = lOldY_Y;
	}

	if ( GetDieShape() != WPR_RHOMBUS_DIE )
	{
		if (abs(nDiePitchXX) < GetDieSizeX())
		{
			m_stSearchArea = stOrgSearchArea;
			m_bThetaCorrection = bTempCorrection;
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			return WPR_ERR_PITCH_NO_X_DIE;
		}

		if (abs(nDiePitchYY) < GetDieSizeY())
		{
			m_stSearchArea = stOrgSearchArea;
			m_bThetaCorrection = bTempCorrection;
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			return WPR_ERR_PITCH_NO_Y_DIE;
		}
	}	//	no check for rhombus die

	// Also check against master pitch if enabled
	double dCurrPitchinMilX = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)nDiePitchXX * 1000 / 25.4);
	double dCurrPitchinMilY = (LONG)(0.5 + GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)nDiePitchYY * 1000 / 25.4);
	double dDx = fabs(m_dWAlignMasterPitchInMilX - dCurrPitchinMilX);
	double dDy = fabs(m_dWAlignMasterPitchInMilY - dCurrPitchinMilY);
	if ((m_dWAlignMasterPitchInMilX > 0) && (dDx > (m_dWAlignMasterPitchInMilX * WPR_DIE_PITCH_TOLERANCE)))
	{
		CString szTemp;
		szTemp.Format("Warning: Current x-pitch is different from master-pitch-x by more than 30/%");
		HmiMessage_Red_Yellow(szTemp);
		SetErrorMessage(szTemp);
		m_stSearchArea = stOrgSearchArea;
		m_bThetaCorrection = bTempCorrection;
		return WPR_ERR_INVALID_PITCH_X;
	}
	if ((m_dWAlignMasterPitchInMilY > 0) && (dDy > (m_dWAlignMasterPitchInMilY * WPR_DIE_PITCH_TOLERANCE)))
	{
		CString szTemp;
		szTemp.Format("Warning: Current y-pitch is different from master-pitch-y by more than 30/%");
		HmiMessage_Red_Yellow(szTemp);
		SetErrorMessage(szTemp);
		m_stSearchArea = stOrgSearchArea;
		m_bThetaCorrection = bTempCorrection;
		return WPR_ERR_INVALID_PITCH_Y;
	}

	//Calculate die pitch in motor step
	m_siDiePitchXX = (nDiePitchXX);				
	m_siDiePitchXY = (nDiePitchXY);				
	m_siDiePitchYX = (nDiePitchYX);				
	m_siDiePitchYY = (nDiePitchYY);

	//Log Pitch info into Log File
	if (CMSLogFileUtility::Instance()->GetEnableMachineLog() == TRUE)
	{
		CMSLogFileUtility::Instance()->WPR_LearnPitchLog("\nStatistic for Die Pitch:");
		CString szData;
		LONG lUnitX, lUnitY;
		
		ConvertMotorStepToUnit(nDiePitchXX, nDiePitchXY, &lUnitX, &lUnitY);
		szData.Format("Pitch XX: %d  %d in mil ", m_siDiePitchXX, lUnitX);
		CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szData);

		szData.Format("Pitch XY: %d  %d in mil ", m_siDiePitchXY, lUnitY);
		CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szData);

		ConvertMotorStepToUnit(nDiePitchYX, nDiePitchYY, &lUnitX, &lUnitY);
		szData.Format("Pitch YX: %d  %d in mil ", m_siDiePitchYX, lUnitX);
		CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szData);

		szData.Format("Pitch YY: %d  %d in mil \n", m_siDiePitchYY, lUnitY);
		CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szData);
	}

	if( (GetDieShape() == WPR_HEXAGON_DIE) || 
		(GetDieShape() == WPR_TRIANGULAR_DIE && IsSorraSortMode()))
	{
		m_siDiePitchXX = m_siDiePitchXX / 2;				
		m_siDiePitchYY = m_siDiePitchYY / 2;				
	}


	DrawSearchBox( PR_COLOR_TRANSPARENT);

	m_bPitchLearnt = TRUE;
	m_stSearchArea = stOrgSearchArea;
	m_bThetaCorrection = bTempCorrection;

	(*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"] = (LONG)(m_siDiePitchXX);
	(*m_psmfSRam)["WaferPr"]["DiePitchX"]["Y"] = (LONG)(m_siDiePitchXY);
	(*m_psmfSRam)["WaferPr"]["DiePitchY"]["X"] = (LONG)(m_siDiePitchYX);
	(*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"] = (LONG)(m_siDiePitchYY);

	//Check Die count in field ifof view		
	CalculateDieInView();

	return 0;
}



VOID CWaferPr::CalculateDieInView(CONST BOOL bPrMsd)
{
	PR_COORD	stTmpDiePixel;
	int			siMarginPixel = 20;
	int			siDieInField = 0;
	int			siXPos;
	int			siYPos;

	if( PR_NotInit() )
		return ;

	if( GetDiePitchXX()==0 || GetDiePitchYY()==0 )
	{
		return ;
	}

	DOUBLE	dRowDieCount = 0;
	DOUBLE	dColDieCount = 0;
	DOUBLE	dDieInField = 0;

	stTmpDiePixel.x = GetPRWinLRX() - GetPRWinULX() - siMarginPixel;
	stTmpDiePixel.y = GetPRWinLRY() - GetPRWinULY() - siMarginPixel;
	ConvertPixelToMotorStep(stTmpDiePixel, &siXPos, &siYPos);

	dRowDieCount	= ((DOUBLE)abs(siXPos) / (DOUBLE)abs(GetDiePitchXX()));
	dColDieCount	= ((DOUBLE)abs(siYPos) / (DOUBLE)abs(GetDiePitchYY()));

	dDieInField		= min(dRowDieCount, dColDieCount);

	m_dLFSize		= dDieInField;
	m_dLFSizeX		= dRowDieCount;
	m_dLFSizeY		= dColDieCount;	

	//Update SRAM value 
	(*m_psmfSRam)["WaferPr"]["FOV"]["X"]		= m_dLFSizeX;		//v3.15T1
	(*m_psmfSRam)["WaferPr"]["FOV"]["Y"]		= m_dLFSizeY;		//v3.15T1

	CalculatePrescanFov(bPrMsd);

	DOUBLE dRowTol = 0, dColTol = 0;
	GetMapIndexTolerance(dRowTol, dColTol);
	if( IsAutoRescanEnable() )
	{
		if( m_lPrescanIndexRowReduce < 1 )
			m_lPrescanIndexRowReduce = 1;
		if( m_lPrescanIndexColReduce < 1 )
			m_lPrescanIndexColReduce = 1;
		DOUBLE dScanFov = min(m_dPrescanLFSizeY, m_dPrescanLFSizeX);
		if( dScanFov > 30)
		{
			if( m_lPrescanIndexRowReduce < 4 )
				m_lPrescanIndexRowReduce = 4;
			if( m_lPrescanIndexColReduce < 4 )
				m_lPrescanIndexColReduce = 4;
		}
		else
		{
			m_lPrescanIndexRowReduce = 2;
			m_lPrescanIndexColReduce = 2;
		}

		if (m_dPrescanLFSizeY > 50)
		{
			if( m_lPrescanIndexRowReduce < 8 )
				m_lPrescanIndexRowReduce = 8;
		}
		else if (m_dPrescanLFSizeY > 40)
		{
			if( m_lPrescanIndexRowReduce < 6 )
				m_lPrescanIndexRowReduce = 6;
		}

		if (m_dPrescanLFSizeX > 50)
		{
			if( m_lPrescanIndexColReduce < 8 )
				m_lPrescanIndexColReduce = 8;
		}
		else if (m_dPrescanLFSizeX > 40)
		{
			if( m_lPrescanIndexColReduce < 6 )
				m_lPrescanIndexColReduce = 6;
		}
	}

	if (m_lPrescanIndexRowReduce > 0)
	{
		m_nPrescanIndexStepRow = (LONG)(m_dPrescanLFSizeY - m_lPrescanIndexRowReduce);
	}
	else
	{
		if( bPrMsd==FALSE )
		{
		//	m_nPrescanIndexStepRow = (LONG)(m_dPrescanLFSizeY - dRowTol);
		}
	}

	if (m_lPrescanIndexColReduce > 0)
	{
		m_nPrescanIndexStepCol = (LONG)(m_dPrescanLFSizeX - m_lPrescanIndexColReduce);
	}
	else
	{
		if( bPrMsd==FALSE )
		{
		//	m_nPrescanIndexStepCol = (LONG)(m_dPrescanLFSizeX - dColTol);
		}
	}

	if (m_bSrchEnableLookForward && bPrMsd==FALSE && !IsLFSizeOK())
	{
		CString szMsg;
		szMsg.Format("FOV size %f less than %f, cycle time would be affected!", m_dLFSize, WPR_LF_SIZE);
		HmiMessage(szMsg, "Die Pitch");
	}
}


VOID CWaferPr::CalculateLFSearchArea(VOID)
{
	LONG lDiffRow = 0, lDiffCol = 0;
	PR_WIN stWnd;

	lDiffRow = 0;	lDiffCol = -1;
	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);
	m_stLFSearchArea[WPR_LT_DIE] = stWnd;

	lDiffRow = 0;	lDiffCol = 1;
	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);
	m_stLFSearchArea[WPR_RT_DIE] = stWnd;

	lDiffRow = -1;	lDiffCol = 0;
	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);
	m_stLFSearchArea[WPR_UP_DIE] = stWnd;

	lDiffRow = 1;	lDiffCol = 0;
	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);
	m_stLFSearchArea[WPR_DN_DIE] = stWnd;

	lDiffRow = -1;	lDiffCol = -1;
	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);
	m_stLFSearchArea[WPR_UL_DIE] = stWnd;

	lDiffRow = 1;	lDiffCol = -1;
	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);
	m_stLFSearchArea[WPR_DL_DIE] = stWnd;

	lDiffRow = -1;	lDiffCol = 1;
	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);
	m_stLFSearchArea[WPR_UR_DIE] = stWnd;

	lDiffRow = 1;	lDiffCol = 1;
	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);
	m_stLFSearchArea[WPR_DR_DIE] = stWnd;
}

BOOL CWaferPr::GetLFSearchArea_3x3(PR_WIN &stWnd, CONST INT nLFRegion)
{
	if ((nLFRegion <= 0) || (nLFRegion > 9))
	{
		return FALSE;
	}

	LONG lDiffRow = 0, lDiffCol = 0;
	//Determine ROW number
	if (nLFRegion <= 3)			//Row #1
	{
		lDiffRow = -1;
	}
	else if (nLFRegion <= 6)	//Row #2
	{
		lDiffRow = 0;
	}
	else						//Row #3
	{
		lDiffRow = 1;
	}

	//Determine COL number
	if ((nLFRegion % 3) == 1)			//Col #1
	{
		lDiffCol = -1;
	}
	else if ((nLFRegion % 3) == 2)	//Col #2
	{
		lDiffCol = 0;
	}
	else if ((nLFRegion % 3) == 0)	//Col #3
	{
		lDiffCol = 1;
	}

	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);

	return TRUE;
}


BOOL CWaferPr::GetLFSearchArea_5x3(PR_WIN &stWnd, CONST INT nLFRegion)
{
	if ((nLFRegion <= 0) || (nLFRegion > 15))
	{
		return FALSE;
	}

	LONG lDiffRow = 0, lDiffCol = 0;
	//Determine ROW number
	if (nLFRegion <= 5)				//ROW #1
	{
		lDiffRow = -1;
	}
	else if (nLFRegion <= 10)		//Row #2
	{
		lDiffRow = 0;
	}
	else //if (nLFRegion <= 15)		//Row #3
	{
		lDiffRow = 1;
	}

	//Determine COL number
	if ((nLFRegion % 5) == 1)			//Col #1
	{
		lDiffCol = -2;
	}
	else if ((nLFRegion % 5) == 2)	//Col #2
	{
		lDiffCol = -1;
	}
	else if ((nLFRegion % 5) == 3)	//COl #3
	{
		lDiffCol = 0;
	}
	else if ((nLFRegion % 5) == 4)	//COl #4
	{
		lDiffCol = 1;
	}
	else //if ((nLFRegion%5) == 0)	//Col #5
	{
		lDiffCol = 2;
	}

	GetLFSearchWindow(lDiffRow, lDiffCol, FALSE, stWnd);

	return TRUE;
}

BOOL CWaferPr::GetLFSearchArea_5x5(PR_WIN &stWnd, CONST INT nLFRegion)
{
	if ((nLFRegion <= 0) || (nLFRegion > 25))
	{
		return FALSE;
	}

	//////////////////////
	//  1  2   3   4  5
	//  6  7   8   9  10
	//  11 12 [13] 14 15
	//  16 17  18  19 20
	//  21 22  23  24 25
	//

	LONG lDiffRow = 0, lDiffCol = 0;
	//Determine ROW number
	if (nLFRegion <= 5)				//ROW #1
	{
		lDiffRow = -2;
	}
	else if (nLFRegion <= 10)		//Row #2
	{
		lDiffRow = -1;
	}
	else if (nLFRegion <= 15)		//Row #3
	{
		lDiffRow = 0;
	}
	else if (nLFRegion <= 20)		//Row #4
	{
		lDiffRow = 1;
	}
	else							//Row #5
	{
		lDiffRow = 2;
	}

	//Determine COL number
	if ((nLFRegion % 5) == 1)			//Col #1
	{
		lDiffCol = -2;
	}
	else if ((nLFRegion % 5) == 2)	//Col #2
	{
		lDiffCol = -1;
	}
	else if ((nLFRegion % 5) == 3)	//Col #3
	{
		lDiffCol = 0;
	}
	else if ((nLFRegion % 5) == 4)	//Col #4
	{
		lDiffCol = 1;
	}
	else //if ((nLFRegion%5) == 0)	//Col #5
	{
		lDiffCol = 2;
	}

	GetLFSearchWindow(lDiffRow, lDiffCol, TRUE, stWnd);

	//v4.43T3
	if ( (nLFRegion == 11) || (nLFRegion == 15))
	{
		CString szTemp;
		szTemp.Format("WPR 5x5 LF %d Wnd: (%d %d)  (%d %d)", nLFRegion,
			stWnd.coCorner1.x, stWnd.coCorner1.y, 
			stWnd.coCorner2.x, stWnd.coCorner2.y);
		//SetErrorMessage(szTemp);
	}

	return TRUE;
}

BOOL CWaferPr::GetLFSearchArea_7x7(PR_WIN &stWnd, CONST INT nLFRegion)
{
	if ((nLFRegion <= 0) || (nLFRegion > 49))
	{
		return FALSE;
	}

	//////////////////////////
	//	1  2  3   4   5  6  7
	//  8  9  10  11  12 13 14
	//  15 16 17  18  19 20 21
	//  22 23 24 [25] 26 27 28
	//  29 30 31  32  33 34 35
	//  36 37 38  39  40 41 42
	//  43 44 45  46  47 48 49
	//

	LONG lDiffRow = 0, lDiffCol = 0;
	//Determine ROW number
	if (nLFRegion <= 7)				//ROW #1
	{
		lDiffRow = -3;
	}
	else if (nLFRegion <= 14)		//ROW #2
	{
		lDiffRow = -2;
	}
	else if (nLFRegion <= 21)		//Row #3
	{
		lDiffRow = -1;
	}
	else if (nLFRegion <= 28)		//Row #4
	{
		lDiffRow = 0;
	}
	else if (nLFRegion <= 35)		//Row #5
	{
		lDiffRow = 1;
	}
	else if (nLFRegion <= 42)		//Row #6
	{
		lDiffRow = 2;
	}
	else							//Row #7
	{
		lDiffRow = 3;
	}

	//Determine COL number
	if ((nLFRegion % 7) == 1)			//Col #1
	{
		lDiffCol = -3;
	}
	else if ((nLFRegion % 7) == 2)		//Col #2
	{
		lDiffCol = -2;
	}
	else if ((nLFRegion % 7) == 3)		//Col #3
	{
		lDiffCol = -1;
	}
	else if ((nLFRegion % 7) == 4)		//Col #4
	{
		lDiffCol = 0;
	}
	else if ((nLFRegion % 7) == 5)		//Col #5
	{
		lDiffCol = 1;
	}
	else if ((nLFRegion % 7) == 6)		//Col #6
	{
		lDiffCol = 2;
	}
	else //if ((nLFRegion%7) == 0)		//Col #7
	{
		lDiffCol = 3;
	}

	GetLFSearchWindow(lDiffRow, lDiffCol, TRUE, stWnd);

	CString szMsg;
	szMsg.Format("GetLFSearchArea_7x7 #Diff(%d, %d, %d), win(LeftTop(%d, %d), RightBottom(%d, %d)", 
		nLFRegion, lDiffRow, lDiffCol, stWnd.coCorner1.x, stWnd.coCorner1.y, stWnd.coCorner2.x, stWnd.coCorner2.y);
	CMSLogFileUtility::Instance()->MS60_Log(szMsg);

	return TRUE;
}

BOOL CWaferPr::GetLFSearchArea_9x9(PR_WIN &stWnd, CONST INT nLFRegion)
{
	if ((nLFRegion <= 0) || (nLFRegion > 81))
	{
		return FALSE;
	}

	//Use 9x9 LF Window
	//////////////////////////
	//	1   2   3   4   5   6   7   8   9
	//  10  11  12  13  14  15  16  17  18
	//  19  20  21  22  23  24  25  26  27
	//  28  29  30  31  32  33  34  35  36
	//  37  38  39  40 [41] 42  43  44  45
	//  46  47  48  49  50  51  52  53  54
	//  55  56  57  58  59  60  61  62  63
	//  64  65  66  67  68  69  70  71  72
	//  73  74  75  76  77  78  79  80  81

	//

	LONG lDiffRow = 0, lDiffCol = 0;
	//Determine ROW number
	if (nLFRegion <= 9)				//ROW #1
	{
		lDiffRow = -4;
	}
	else if (nLFRegion <= 18)		//ROW #2
	{
		lDiffRow = -3;
	}
	else if (nLFRegion <= 27)		//Row #3
	{
		lDiffRow = -2;
	}
	else if (nLFRegion <= 36)		//Row #4
	{
		lDiffRow = -1;
	}
	else if (nLFRegion <= 45)		//Row #5
	{
		lDiffRow = 0;
	}
	else if (nLFRegion <= 54)		//Row #6
	{
		lDiffRow = 1;
	}
	else if (nLFRegion <= 63)		//Row #7
	{
		lDiffRow = 2;
	}
	else if (nLFRegion <= 72)		//Row #8
	{
		lDiffRow = 3;
	}
	else							//Row #9
	{
		lDiffRow = 4;
	}

	//Determine COL number
	if ((nLFRegion % 9) == 1)			//Col #1
	{
		lDiffCol = -4;
	}
	else if ((nLFRegion % 9) == 2)		//Col #2
	{
		lDiffCol = -3;
	}
	else if ((nLFRegion % 9) == 3)		//Col #3
	{
		lDiffCol = -2;
	}
	else if ((nLFRegion % 9) == 4)		//Col #4
	{
		lDiffCol = -1;
	}
	else if ((nLFRegion % 9) == 5)		//Col #5
	{
		lDiffCol = 0;
	}
	else if ((nLFRegion % 9) == 6)		//Col #6
	{
		lDiffCol = 1;
	}
	else if ((nLFRegion % 9) == 7)		//Col #7
	{
		lDiffCol = 2;
	}
	else if ((nLFRegion % 9) == 8)		//Col #8
	{
		lDiffCol = 3;
	}
	else //if ((nLFRegion%7) == 0)		//Col #9
	{
		lDiffCol = 4;
	}

	BOOL bOK = GetLFSearchWindow(lDiffRow, lDiffCol, TRUE, stWnd);

	CString szMsg;
	szMsg.Format("GetLFSearchArea_9x9 OK%d #Diff(%d, %d, %d), win(LeftTop(%d, %d), RightBottom(%d, %d)", 
		bOK, nLFRegion, lDiffRow, lDiffCol, stWnd.coCorner1.x, stWnd.coCorner1.y, stWnd.coCorner2.x, stWnd.coCorner2.y);
	CMSLogFileUtility::Instance()->MS60_Log(szMsg);

	return TRUE;
}

BOOL CWaferPr::GetLFSearchArea_11x11(PR_WIN &stWnd, CONST INT nLFRegion)
{
	if ((nLFRegion <= 0) || (nLFRegion > 121))
	{
		return FALSE;
	}

	//Use 11x11 LF Window
	//////////////////////////
	//	1   2   3   4   5   6   7   8   9   10  11
	//  12  13  14  15  16  17  18  19  20  21  22  
	//	23  24  25  26  27  28  29  30  31  32  33
	//  34  35  36  37  38  39  40  41  42  43  44
	//  45  46  47  48  49  50  51  52  53  54  55
	//  56  57  58  59  60 [61] 62  63  64  65  66
	//	67  68  69  70  71  72  73  74  75  76  77
	//	78  79  80  81  82  83  84  85  86  87  88
	//	89  90  91  92  93  94  95  96  97  98  99
	//	100 101 102 103 104 105 106 107 108 109 110
	//  111 112 113 114 115 116 117 118 119 120 121

	LONG lDiffRow = 0, lDiffCol = 0;
	//Determine ROW number
	if (nLFRegion <= 11)				//ROW #1
	{
		lDiffRow = -5;
	}
	else if (nLFRegion <= 22)		//ROW #2
	{
		lDiffRow = -4;
	}
	else if (nLFRegion <= 33)		//Row #3
	{
		lDiffRow = -3;
	}
	else if (nLFRegion <= 44)		//Row #4
	{
		lDiffRow = -2;
	}
	else if (nLFRegion <= 55)		//Row #5
	{
		lDiffRow = -1;
	}
	else if (nLFRegion <= 66)		//Row #6
	{
		lDiffRow = 0;
	}
	else if (nLFRegion <= 77)		//Row #7
	{
		lDiffRow = 1;
	}
	else if (nLFRegion <= 88)		//Row #8
	{
		lDiffRow = 2;
	}
	else if (nLFRegion <= 99)		//Row #9
	{
		lDiffRow = 3;
	}
	else if (nLFRegion <= 110)		//Row #10
	{
		lDiffRow = 4;
	}
	else							//Row #11
	{
		lDiffRow = 5;
	}

	//Determine COL number
	if ((nLFRegion % 11) == 1)			//Col #1
	{
		lDiffCol = -5;
	}
	else if ((nLFRegion % 11) == 2)		//Col #2
	{
		lDiffCol = -4;
	}
	else if ((nLFRegion % 11) == 3)		//Col #3
	{
		lDiffCol = -3;
	}
	else if ((nLFRegion % 11) == 4)		//Col #4
	{
		lDiffCol = -2;
	}
	else if ((nLFRegion % 11) == 5)		//Col #5
	{
		lDiffCol = -1;
	}
	else if ((nLFRegion % 11) == 6)		//Col #6
	{
		lDiffCol = 0;
	}
	else if ((nLFRegion % 11) == 7)		//Col #7
	{
		lDiffCol = 1;
	}
	else if ((nLFRegion % 11) == 8)		//Col #8
	{
		lDiffCol = 2;
	}
	else if ((nLFRegion % 11) == 9)		//Col #9
	{
		lDiffCol = 3;
	}
	else if ((nLFRegion % 11) == 10)		//Col #10
	{
		lDiffCol = 4;
	}
	else //if ((nLFRegion%7) == 0)		//Col #11
	{
		lDiffCol = 5;
	}

	BOOL bOK = GetLFSearchWindow(lDiffRow, lDiffCol, TRUE, stWnd);

	CString szMsg;
	szMsg.Format("GetLFSearchArea_11x11 OK%d #Diff(%d, %d, %d), win(LeftTop(%d, %d), RightBottom(%d, %d)", 
		bOK, nLFRegion, lDiffRow, lDiffCol, stWnd.coCorner1.x, stWnd.coCorner1.y, stWnd.coCorner2.x, stWnd.coCorner2.y);
	CMSLogFileUtility::Instance()->MS60_Log(szMsg);

	return TRUE;
}

BOOL CWaferPr::GetLFSearchArea_13x13(PR_WIN &stWnd, CONST INT nLFRegion)
{
	if ((nLFRegion <= 0) || (nLFRegion > 169))
	{
		return FALSE;
	}

	//Use 13x13 LF Window
	//////////////////////////
	//	1   2   3   4   5   6   7   8   9   10  11  12  13
	//	14  15  16  17  18  19  20  21  22  23  24  25  26
	//	27  28  29  30  31  32  33  34  35  36  37  38  39
	//  40  41  42  43  44  45  46  47  48  49  50  51  52
	//  53  54  55  56  57  58  59  60  61  62  63  64  65
	//  66  67  68  69  70  71  72  73  74  75  76  77  78
	//  79  80  81  82  83  84 [85] 86  87  88  89  90  91
	//	92  93  94  95  96  97  98  99  100 101 102 103 104
	//  105 106 107 108 109 110 111 112 113 114 115 116 117
	//  118 119 120 121 122 123 124 125 126 127 128 129 130
	//  131 132 133 134 135 136 137 138 139 140 141 142 143
	//  144 145 146 147 148 149 150 151 152 153 154 155 156
	//  157 158 159 160 161 162 163 164 165 166 167 168 169

	LONG lDiffRow = 0, lDiffCol = 0;
	//Determine ROW number
	if (nLFRegion <= 13)			//ROW #1
	{
		lDiffRow = -6;
	}
	else if (nLFRegion <= 26)		//ROW #2
	{
		lDiffRow = -5;
	}
	else if (nLFRegion <= 39)		//Row #3
	{
		lDiffRow = -4;
	}
	else if (nLFRegion <= 52)		//Row #4
	{
		lDiffRow = -3;
	}
	else if (nLFRegion <= 65)		//Row #5
	{
		lDiffRow = -2;
	}
	else if (nLFRegion <= 78)		//Row #6
	{
		lDiffRow = -1;
	}
	else if (nLFRegion <= 91)		//Row #7
	{
		lDiffRow = 0;
	}
	else if (nLFRegion <= 104)		//Row #8
	{
		lDiffRow = 1;
	}
	else if (nLFRegion <= 117)		//Row #9
	{
		lDiffRow = 2;
	}
	else if (nLFRegion <= 130)		//Row #10
	{
		lDiffRow = 3;
	}
	else if (nLFRegion <= 143)		//Row #11
	{
		lDiffRow = 4;
	}
	else if (nLFRegion <= 156)		//Row #12
	{
		lDiffRow = 5;
	}
	else							//Row #13
	{
		lDiffRow = 6;
	}

	//Determine COL number
	if ((nLFRegion % 13) == 1)			//Col #1
	{
		lDiffCol = -6;
	}
	else if ((nLFRegion % 13) == 2)		//Col #2
	{
		lDiffCol = -5;
	}
	else if ((nLFRegion % 13) == 3)		//Col #3
	{
		lDiffCol = -4;
	}
	else if ((nLFRegion % 13) == 4)		//Col #4
	{
		lDiffCol = -3;
	}
	else if ((nLFRegion % 13) == 5)		//Col #5
	{
		lDiffCol = -2;
	}
	else if ((nLFRegion % 13) == 6)		//Col #6
	{
		lDiffCol = -1;
	}
	else if ((nLFRegion % 13) == 7)		//Col #7
	{
		lDiffCol = 0;
	}
	else if ((nLFRegion % 13) == 8)		//Col #8
	{
		lDiffCol = 1;
	}
	else if ((nLFRegion % 13) == 9)		//Col #9
	{
		lDiffCol = 2;
	}
	else if ((nLFRegion % 13) == 10)		//Col #10
	{
		lDiffCol = 3;
	}
	else if ((nLFRegion % 13) == 11)		//Col #11
	{
		lDiffCol = 4;
	}
	else if ((nLFRegion % 13) == 12)		//Col #12
	{
		lDiffCol = 3;
	}
	else									//Col #13
	{
		lDiffCol = 6;
	}

	BOOL bOK = GetLFSearchWindow(lDiffRow, lDiffCol, TRUE, stWnd);

	CString szMsg;
	szMsg.Format("GetLFSearchArea_13x13 OK%d #Diff(%d, %d, %d), win(LeftTop(%d, %d), RightBottom(%d, %d)", 
		bOK, nLFRegion, lDiffRow, lDiffCol, stWnd.coCorner1.x, stWnd.coCorner1.y, stWnd.coCorner2.x, stWnd.coCorner2.y);
	CMSLogFileUtility::Instance()->MS60_Log(szMsg);

	return TRUE;
}


INT CWaferPr::GetLFRegion_5x5(CONST ULONG ulCurRow, CONST ULONG ulCurCol, 
								CONST ULONG ulTargetRow, CONST ULONG ulTargetCol)		//v4.43T2
{
	INT nRegion = 0;
	
	LONG lCurRow = (LONG) ulCurRow;
	LONG lCurCol = (LONG) ulCurCol;
	LONG lTarRow = (LONG) ulTargetRow;
	LONG lTarCol = (LONG) ulTargetCol;

	LONG lRowDiff = lTarRow - lCurRow;
	LONG lColDiff = lTarCol - lCurCol;

	//////////////////////
	//  1  2   3   4  5
	//  6  7   8   9  10
	//  11 12 [13] 14 15
	//  16 17  18  19 20
	//  21 22  23  24 25
	//

	switch (lRowDiff)
	{
	case -2:
		nRegion = 0;
		break;
	case -1:
		nRegion = 5;
		break;
	case 1:
		nRegion = 15;
		break;
	case 2:
		nRegion = 20;
		break;
	case 0:
	default:
		nRegion = 15;
		break;
	}

	switch (lColDiff)
	{
	case -2:
		nRegion = nRegion + 1;
		break;
	case -1:
		nRegion = nRegion + 2;
		break;
	case 1:
		nRegion = nRegion + 4;
		break;
	case 2:
		nRegion = nRegion + 5;
		break;
	case 0:
	default:
		nRegion = nRegion + 3;
		break;
	}

	return nRegion;
}


INT CWaferPr::GetLFRegion_7x7(CONST ULONG ulCurRow, CONST ULONG ulCurCol, 
								CONST ULONG ulTargetRow, CONST ULONG ulTargetCol)		//v4.43T2
{
	INT nRegion = 0;
	
	LONG lCurRow = (LONG) ulCurRow;
	LONG lCurCol = (LONG) ulCurCol;
	LONG lTarRow = (LONG) ulTargetRow;
	LONG lTarCol = (LONG) ulTargetCol;

	LONG lRowDiff = lTarRow - lCurRow;
	LONG lColDiff = lTarCol - lCurCol;

	//////////////////////////
	//	1  2  3   4   5  6  7
	//  8  9  10  11  12 13 14
	//  15 16 17  18  19 20 21
	//  22 23 24 [25] 26 27 28
	//  29 30 31  32  33 34 35
	//  36 37 38  39  40 41 42
	//  43 44 45  46  47 48 49
	//

	switch (lRowDiff)
	{
	case -3:
		nRegion = 0;
		break;
	case -2:
		nRegion = 7;
		break;
	case -1:
		nRegion = 14;
		break;
	case 1:
		nRegion = 28;
		break;
	case 2:
		nRegion = 35;
		break;
	case 3:
		nRegion = 42;
		break;
	case 0:
	default:
		nRegion = 21;
		break;
	}

	switch (lColDiff)
	{
	case -3:
		nRegion = nRegion + 1;
		break;
	case -2:
		nRegion = nRegion + 2;
		break;
	case -1:
		nRegion = nRegion + 3;
		break;
	case 1:
		nRegion = nRegion + 5;
		break;
	case 2:
		nRegion = nRegion + 6;
		break;
	case 3:
		nRegion = nRegion + 7;
		break;
	case 0:
	default:
		nRegion = nRegion + 4;
		break;
	}

	return nRegion;
}


VOID CWaferPr::DisplayRectArea(PR_COORD stCorner1, PR_COORD stCorner2, PR_COLOR ssColor)
{
	DrawRectangleBox(stCorner1, stCorner2,  ssColor);
}

VOID CWaferPr::DisplayLFSearchArea(PR_COLOR ssColor)
{
	short i = 0;
	BOOL bUseLookForward;

	bUseLookForward = IsUseLF();

	if ((m_bSrchEnableLookForward == TRUE) && (bUseLookForward == TRUE))
	{
		if (m_dLFSize != 0)
		{
			if ((m_dLFSize > WPR_LF_SIZE_5X5) && m_bWaferPrUseMxNLFWnd)		//v4.43T9
			{
				PR_WIN stWnd;

				GetLFSearchWindow(0, -2, FALSE, stWnd);
				DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);

				GetLFSearchWindow(0, 2, FALSE, stWnd);
				DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);

				GetLFSearchWindow(-2, 0, FALSE, stWnd);
				DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);

				GetLFSearchWindow(2, 0, FALSE, stWnd);
				DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);

				stWnd.coCorner1.x = m_stSearchArea.coCorner1.x;
				stWnd.coCorner2.x = m_stSearchArea.coCorner2.x;
				stWnd.coCorner1.y = m_stSearchArea.coCorner1.y;
				stWnd.coCorner2.y = m_stSearchArea.coCorner2.y;
			}
			else
			{
				for (i = 0; i < WPR_CUR_FORWARD; i++)
				{
					DrawRectangleBox(m_stLFSearchArea[i].coCorner1, m_stLFSearchArea[i].coCorner2, ssColor);
				}
			}
		}
	}
}


VOID CWaferPr::DisplayLFSearchArea_mxn(PR_COLOR ssColor, INT nRegionSize, INT nCurrLFPosn)
{
	short i = 0;
	BOOL bUseLookForward;

	bUseLookForward = IsUseLF();

	if (m_dLFSize > 0)
	{
		PR_WIN stWnd;
		
		if (nRegionSize == REGION_SIZE_5x5)
		{
			for(INT i=1; i<=25; i++)
			{
				GetLFSearchArea_5x5(stWnd, i);
				if( i%2==1 )
					ssColor = PR_COLOR_YELLOW;
				else
					ssColor = PR_COLOR_GREEN;
				DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);
				Sleep(3000);
			}

			if (GetLFSearchArea_5x5(stWnd, 13))
			{
				DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, PR_COLOR_RED);
			}
		}
		else if (nRegionSize == REGION_SIZE_5x3)
		{
			if (m_nCurrLFPosn != 0)
			{
				if (GetLFSearchArea_5x3(stWnd, m_nCurrLFPosn))
				{
					DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, PR_COLOR_TRANSPARENT);
				}
				m_nCurrLFPosn = 0;
			}

			GetLFSearchArea_5x3(stWnd, 1);
			DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);
			GetLFSearchArea_5x3(stWnd, 3);
			DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);
			GetLFSearchArea_5x3(stWnd, 5);
			DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);
			GetLFSearchArea_5x3(stWnd, 7);
			DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);
			GetLFSearchArea_5x3(stWnd, 9);
			DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);
			GetLFSearchArea_5x3(stWnd, 11);
			DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);
			GetLFSearchArea_5x3(stWnd, 13);
			DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);
			GetLFSearchArea_5x3(stWnd, 15);
			DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, ssColor);

			if ((nCurrLFPosn != 0) &&					// not defined
					(nCurrLFPosn != 8) &&					// not Center region
					(ssColor != PR_COLOR_TRANSPARENT))		// not display
			{
				m_nCurrLFPosn = nCurrLFPosn;
				GetLFSearchArea_5x3(stWnd, nCurrLFPosn);
				DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, PR_COLOR_RED);
			}
		}
		else if (nRegionSize == REGION_SIZE_3x3)
		{
			for (i = 0; i < WPR_CUR_FORWARD; i++)
			{
				DrawRectangleBox(m_stLFSearchArea[i].coCorner1, m_stLFSearchArea[i].coCorner2, ssColor);
			}
		}
	}
}


VOID CWaferPr::FreeAllPrRecords(BOOL bResetState)
{
	INT i;
	CString csMsg;
	CString szLog;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	m_pPrGeneral->FreeAllPRRecord(ubSID, ubRID);

	if (bResetState)
	{
		//v4.49A7
		CString szLog = _T("WPR: FreeAll PrRecords - ");
		CString szValue;

		for (i = 0; i < WPR_MAX_DIE ; i++)
		{
			m_szGenDieZoomMode[i] = "";
			m_bGenDieCalibration[i] = FALSE;
			m_ssGenPRSrchID[i] = 0;
			m_bGenDieLearnt[i] = FALSE;

			//v4.49A7	//Osram Penang
			szValue.Format("%ld(ID=%ld) ", i, m_ssGenPRSrchID[i]);
			szLog = szLog + szValue;
		}
		
		m_lLrnTotalRefDie = 0;
		m_bDieCalibrated	= FALSE;	
		m_bDieIsLearnt		= FALSE;
		m_bPitchLearnt		= FALSE;
		m_lCurNormDieID		= 0;

		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);	//v4.49A7
	}

	CONST CString szExePath = gszROOT_DIRECTORY + "\\EXE\\";
	CString szTgtFile = szExePath + "WaferPr_" + m_szPkgNameCurrent + ".msd";
	DeleteFile(szTgtFile);
	szTgtFile = szExePath + "WaferTable_" + m_szPkgNameCurrent + ".msd";
	DeleteFile(szTgtFile);
	szTgtFile = szExePath + "WaferPr_" + m_szPkgNamePrevious + ".msd";
	DeleteFile(szTgtFile);
	szTgtFile = szExePath + "WaferTable_" + m_szPkgNamePrevious + ".msd";
	DeleteFile(szTgtFile);
	m_szPkgNameCurrent.Empty();
	m_szPkgNamePrevious.Empty();
	for(int i=0; i<WPR_MAX_DIE; i++)
	{
		m_saPkgRecordCurrent[i] = 0;
		m_saPkgRecordPrevious[i] = 0;
	}
	SaveRuntimeTwoRecords();
}


VOID CWaferPr::FlushMessage()
{
	if (m_comServer.ScanRequest(10))
	{
		m_comServer.ReadRequest();
	}
}

VOID CWaferPr::UpdateStationData()
{
	//AfxMessageBox("CWaferPr", MB_SYSTEMMODAL);

	m_oWaferPrDataBlk.m_szPrLrnCoaxLightLevel.Format("%d", m_lWPRLrnCoaxLightHmi);
	m_oWaferPrDataBlk.m_szPrLrnRingLightLevel.Format("%d", m_lWPRLrnRingLightHmi);
	m_oWaferPrDataBlk.m_szPrLrnSideLightLevel.Format("%d", m_lWPRLrnSideLightHmi);
	
	m_oWaferPrDataBlk.m_szPrDiePassScore.Format("%d", m_lGenSrchDieScore[WPR_NORMAL_DIE]);
	m_oWaferPrDataBlk.m_szRefDiePRPassScore.Format("%d", m_lGenSrchDieScore[WPR_GEN_RDIE_OFFSET + WPR_REFERENCE_DIE]);
	m_oWaferPrDataBlk.m_szDefectCheckChipPercent.Format("%.2f", m_dGenSrchChipArea[WPR_NORMAL_DIE]);
	m_oWaferPrDataBlk.m_szSingleDefectCheckPercent.Format("%.2f", m_dGenSrchSingleDefectArea[WPR_NORMAL_DIE]);
	m_oWaferPrDataBlk.m_szTotalDefectCheckPercent.Format("%.2f", m_dGenSrchTotalDefectArea[WPR_NORMAL_DIE]);

	m_oWaferPrDataBlk.m_szNormalDieSearchAreaX.Format("%.1f", GetGenSrchAreaX(WPR_NORMAL_DIE) * WPR_SEARCHAREA_SHIFT_FACTOR);
	m_oWaferPrDataBlk.m_szNormalDieSearchAreaY.Format("%.1f", GetGenSrchAreaY(WPR_NORMAL_DIE) * WPR_SEARCHAREA_SHIFT_FACTOR);
	m_oWaferPrDataBlk.m_szReferenceDieSearchAreaX.Format("%.1f", GetGenSrchAreaX(WPR_GEN_RDIE_OFFSET + WPR_REFERENCE_DIE) * WPR_SEARCHAREA_SHIFT_FACTOR);
	m_oWaferPrDataBlk.m_szReferenceDieSearchAreaY.Format("%.1f", GetGenSrchAreaY(WPR_GEN_RDIE_OFFSET + WPR_REFERENCE_DIE) * WPR_SEARCHAREA_SHIFT_FACTOR);
	
	if (m_lGenSrchGreyLevelDefect[WPR_NORMAL_DIE] == 0)
	{
		m_oWaferPrDataBlk.m_szGreyLevelDefectType = "Both";
	}
	else if (m_lGenSrchGreyLevelDefect[WPR_NORMAL_DIE] == 1)
	{
		m_oWaferPrDataBlk.m_szGreyLevelDefectType = "Bright";
	}
	else
	{
		m_oWaferPrDataBlk.m_szGreyLevelDefectType = "Dark";
	}
	
	m_oWaferPrDataBlk.m_szEnableCheckChip = m_oWaferPrDataBlk.ConvertBoolToOnOff(m_bGenSrchEnableChipCheck[WPR_NORMAL_DIE]);
	m_oWaferPrDataBlk.m_szEnableCheckDefect = m_oWaferPrDataBlk.ConvertBoolToOnOff(m_bGenSrchEnableDefectCheck[WPR_NORMAL_DIE]);

	m_oWaferPrDataBlk.m_szDieCheckDefectThreshold.Format("%d", m_lGenSrchDefectThres[WPR_NORMAL_DIE]);
	m_oWaferPrDataBlk.m_szEnableLookForward = m_oWaferPrDataBlk.ConvertBoolToOnOff(m_bSrchEnableLookForward);
	m_oWaferPrDataBlk.m_szBadCutDieDetectionXTolerance.Format("%d", m_stBadCutTolerenceX);
	m_oWaferPrDataBlk.m_szBadCutDieDetectionYTolerance.Format("%d", m_stBadCutTolerenceY);
	
	m_oWaferPrDataBlk.m_szNoDieMaxCount.Format("%d", m_ulMaxNoDieSkipCount);
	m_oWaferPrDataBlk.m_szPRDieMaxSkipCount.Format("%d", m_ulMaxPRSkipCount);
	m_oWaferPrDataBlk.m_szEnableThetaCorrection = m_oWaferPrDataBlk.ConvertBoolToOnOff(m_bThetaCorrection);
	m_oWaferPrDataBlk.m_szPreScanSearchDieScore.Format("%d", m_lPrescanSrchDieScore);

	m_oWaferPrDataBlk.m_szBadCutDetection.Format("%d", m_bBadCutDetection);

	LONG lEjrCapCleanLimit = (*m_psmfSRam)["WaferTable"]["EjectorCapCleanLimit"];
	m_oWaferPrDataBlk.m_szEjrCapCleanLimit.Format("%d", lEjrCapCleanLimit);
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	pApp->SetWaferPrDataBlock(m_oWaferPrDataBlk);
}

VOID CWaferPr::GetLightingConfig(int nGroupID, PR_SOURCE &emCoaxID, PR_SOURCE &emRingID, PR_SOURCE &emSideID)
{
	if (CMS896AStn::m_bEnablePRDualColor == FALSE)
	{
		emCoaxID = PR_COAXIAL_LIGHT;
		emRingID = PR_RING_LIGHT;
		emSideID = PR_SIDE_LIGHT;
	}
	else
	{
		switch (nGroupID)
		{
			default:
				emCoaxID = PR_COAXIAL_LIGHT;
				emRingID = PR_RING_LIGHT;
				emSideID = PR_SIDE_LIGHT_1;
				break;
				
			case 1:
				emCoaxID = PR_COAXIAL_LIGHT_1;
				emRingID = PR_RING_LIGHT;
				emSideID = PR_SIDE_LIGHT_2;
				break;
		}
	}
}




VOID CWaferPr::CloneCoordinate(PR_COORD *stSource , PR_COORD *stDest , INT size)
{
	for (int i = 0; i < size; i++)
	{
		stDest[i].x = stSource[i].x;
		stDest[i].y = stSource[i].y;
	}
}


VOID CWaferPr::DrawAndErasePolygon(PR_COORD *stCorners, INT size, PR_COLOR color)
{
	int j = 1;

	for (int i = 0; i < size; i++)
	{
		if (i == size - 1)
		{
			j = 0;
		}
		else
		{
			j = i + 1;
		}
			
		DrawAndEraseLineForPolygon(stCorners[i], stCorners[j], color);
	}
}


VOID CWaferPr::DrawAndEraseLineForPolygon(PR_COORD stStartPos, PR_COORD stEndPos, PR_COLOR color)
{
	PR_DRAW_LINE_CMD		stLineCmd;
	PR_DRAW_LINE_RPY		stLineRpy;

	stLineCmd.emColor = color;
	
	stLineCmd.stLine.coPoint1 = stStartPos;
	stLineCmd.stLine.coPoint2 = stEndPos; 
	PR_DrawLineCmd(&stLineCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stLineRpy);
}

DOUBLE CWaferPr::FindAngle(PR_COORD stPos1, PR_COORD stPos2)
{
	DOUBLE dAngleInRadian = 0;

	if (stPos1.x == stPos2.x)
	{
		if ((stPos2.y - stPos1.y) > 0)
		{
			dAngleInRadian = PI / 2;
		}
		else if ((stPos2.y - stPos1.y) < 0)
		{
			dAngleInRadian = -PI / 2;
		}

		else
		{
			dAngleInRadian = 0;
		}
	}

	else
	{
		dAngleInRadian = atan2((DOUBLE)(stPos1.y - stPos2.y), (DOUBLE)(stPos1.x - stPos2.x));
	}

	return dAngleInRadian;
}

DOUBLE CWaferPr::FindLength(PR_COORD stPos1, PR_COORD stPos2)
{
	return sqrt(pow((stPos1.x - stPos2.x), 2.0) + pow((stPos1.y - stPos2.y), 2.0));
}

// Function to switch on/off the zoom motor
INT CWaferPr::SetZoomZPower(CONST BOOL bSet)
{
	if (!m_fHardware)
	{
		return TRUE;
	}

	try
	{
		if (bSet == TRUE)
		{
			//m_pStepper_Zoom_Z->PowerOn();	
			CMS896AStn::MotionPowerOn("WaferPrZoomZAxis", NULL);	
		}
		else
		{
			//m_pStepper_Zoom_Z->PowerOff();	
			CMS896AStn::MotionPowerOff("WaferPrZoomZAxis", NULL);	
		}	
	}
	catch (CAsmException e) 
	{
		DisplayException(e);
		return FALSE;
	}

	return TRUE;
}

// Function to switch on/off the focus motor
INT CWaferPr::SetFocusZPower(CONST BOOL bSet)
{
	if (!m_fHardware)
	{
		return TRUE;
	}

	try
	{
		if (bSet == TRUE)
		{
			//m_pStepper_Focus_Z->PowerOn();
			CMS896AStn::MotionPowerOn("WaferPrFocusZAxis", NULL);
		}
		else
		{
			//m_pStepper_Focus_Z->PowerOff();
			CMS896AStn::MotionPowerOff("WaferPrFocusZAxis", NULL);
		}
	}
	catch (CAsmException e) 
	{
		DisplayException(e);
		return FALSE;
	}

	return TRUE;
}

// Function to Move the motor to home position
INT CWaferPr::Z_HomeZoom()
{
	INT nResult	= gnOK;

	//if (m_fHardware && m_pStepper_Zoom_Z)
	if (m_fHardware)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus("WaferPrZoomZAxis", NULL) != 0)
			{
				CMS896AStn::MotionClearError("WaferPrZoomZAxis", NULL);
			}

			if ((nResult = CMS896AStn::MotionMoveHome("WaferPrZoomZAxis", 1, 0, NULL)) == gnAMS_OK)
			{	
				Sleep(100);
				CMS896AStn::MotionSetPosition("WaferPrZoomZAxis", 0, NULL);
				m_bZoomHome_Z	= TRUE;
				m_lEnc_Zoom = CMS896AStn::MotionGetPosition("WaferPrZoomZAxis", NULL);
			}
			
			if (nResult == gnNOTOK)
			{
				nResult = Err_ZoomHome; 
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult("WaferPrZoomZAxis");
			//ClearStepperError("stpWaferPrZoomZ");
			CMS896AStn::MotionCheckResult("WaferPrZoomZAxis", NULL);
			CMS896AStn::MotionClearError("WaferPrZoomZAxis", NULL);
			nResult = Err_ZoomHome; 
			m_bZoomHome_Z	= FALSE;
		}
	}
	else
	{
		m_bZoomHome_Z = TRUE;
	}

	return nResult;
}


INT CWaferPr::Z_HomeFocus()
{
	INT nResult	= gnOK;

	//if (m_fHardware && m_pStepper_Focus_Z)
	if (m_fHardware)
	{
		try
		{
			/*
			if (m_pStepper_Focus_Z->ReportErrorStatus() != 0)
			{
				ClearStepperError("stpWaferPrFocusZ");
			}
			   
			//m_pStepper_Zoom_Z->DisableProtection(HP_LIMIT_SENSOR);
						
			if ((nResult = MoveHome("WaferPrFocusZAxis"))== gnAMS_OK)
			{
				Sleep(100);
				m_pStepper_Focus_Z->SetPosition(0);
				m_bFocusHome_Z	= TRUE;
				m_lEnc_Focus = m_pStepper_Focus_Z->GetPosition();
				//	m_pStepper_Focus_Z->EnableProtection(HP_LIMIT_SENSOR, TRUE);
			}
			if (nResult == gnNOTOK)
			{
				nResult = Err_FocusHome; 
			}
			*/
			if (CMS896AStn::MotionReportErrorStatus("WaferPrFocusZAxis", NULL) != 0)
			{
				CMS896AStn::MotionClearError("WaferPrFocusZAxis", NULL);
			}

			if ((nResult = CMS896AStn::MotionMoveHome("WaferPrFocusZAxis", 1, 0, NULL)) == gnAMS_OK)
			{
				Sleep(100);
				CMS896AStn::MotionSetPosition("WaferPrFocusZAxis", 0, NULL);
				m_bFocusHome_Z	= TRUE;
				m_lEnc_Focus = CMS896AStn::MotionGetPosition("WaferPrFocusZAxis", NULL);
			}
			if (nResult == gnNOTOK)
			{
				nResult = Err_FocusHome; 
			}
			
			/*
			if ((m_pStepper_Focus_Z->IsHomeSensorHigh()) && (m_pStepper_Focus_Z->IsPosLimitSensorHigh()))
			{
				if ((nResult = MoveHome("WaferPrFocusZAxis")) == gnAMS_OK)
				{
					Sleep(100);
					m_pStepper_Focus_Z->SetPosition(0);
					m_bFocusHome_Z	= TRUE;
				}
			}
			else
			{
				MoveHome("WaferPrFocusZAxis");
				SelectSearchProfile("WaferPrFocusZAxis", "spfWaferPrFocusLimit");
				m_pStepper_Focus_Z->Search(1,SFM_WAIT);
				nResult = MoveHome("WaferPrFocusZAxis");
				Sleep(100);
				m_pStepper_Focus_Z->SetPosition(0);
				m_bFocusHome_Z	= TRUE;
			}
			*/

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult("WaferPrFocusZAxis");
			//ClearStepperError("stpWaferPrFocusZ");
			CMS896AStn::MotionCheckResult("WaferPrFocusZAxis", NULL);
			CMS896AStn::MotionClearError("WaferPrFocusZAxis", NULL);
			nResult = Err_FocusHome; 
			m_bFocusHome_Z	= FALSE;
		}
	}
	else
	{
		m_bFocusHome_Z = TRUE;
	}

	return nResult;
}

//MS100 AOI Zoom Z motor
INT CWaferPr::Z_Home()
{
	INT nResult			= gnOK;

	if (m_fHardware && IsWprWithAF())
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(WAFERPR_AXIS_Z, &m_stZoomAxis_Z) != 0)
			{
				m_bComm_Z	= FALSE;
				CMS896AStn::MotionClearError(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
			}

			if (!m_bComm_Z)
			{
				m_bComm_Z	= FALSE;

				nResult = CMS896AStn::MotionCommutateServo(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);				
				if (nResult == gnOK)
				{
					m_bComm_Z = TRUE;
				}
				else
				{
					nResult	= gnNOTOK;
					CMS896AStn::MotionClearError(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
				}
			}

			if (nResult == gnOK)
			{
				nResult = CMS896AStn::MotionMoveHome(WAFERPR_AXIS_Z, 1, 0, &m_stZoomAxis_Z);

				if ((CMS896AStn::MotionReportErrorStatus(WAFERPR_AXIS_Z, &m_stZoomAxis_Z) == 0) && 
						CMS896AStn::MotionIsPowerOn(WAFERPR_AXIS_Z, &m_stZoomAxis_Z))
				{
					Sleep(50);
					CMS896AStn::MotionSetPosition(WAFERPR_AXIS_Z, 0, &m_stZoomAxis_Z);
					m_bZoomHome_Z = TRUE;
				}
				else if (nResult == gnNOTOK)
				{
					m_bZoomHome_Z = FALSE;
					nResult = gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
			CMS896AStn::MotionClearError(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);

			m_bComm_Z	= FALSE;
			m_bZoomHome_Z	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (IsWprWithAF())
		{
			m_bComm_Z	= TRUE;
			m_bZoomHome_Z	= TRUE;
		}
	}

	return nResult;
}

INT CWaferPr::BLZ_Home()
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bSelBL_Z)
	{
		try
		{
		//	SaveScanTimeEvent("BLZ Home");
			if (CMS896AStn::MotionReportErrorStatus(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z) != 0)
			{
				CMS896AStn::MotionClearError(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
			}

			//v4.39T7	//ES101 v1.2
			if (!m_bBackLightComm_Z)
			{
				m_bBackLightComm_Z	= FALSE;

				nResult = CMS896AStn::MotionCommutateServo(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);				
				if (nResult == gnOK)
				{
					m_bBackLightComm_Z = TRUE;
				}
				else
				{
					nResult	= gnNOTOK;
					CMS896AStn::MotionClearError(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
				}
			}

			if (nResult == gnOK)
			{
				if ((nResult = CMS896AStn::MotionMoveHome(WAFERPR_BACKLIGHT_Z, 1, 0, &m_stBackLight_Z)) == gnAMS_OK)
				{
					Sleep(200);

					CMS896AStn::MotionSelectSearchProfile(WAFERPR_BACKLIGHT_Z, BL_SP_INDEXER_Z, &m_stBackLight_Z);
					CMS896AStn::MotionSearch(WAFERPR_BACKLIGHT_Z, 1, SFM_WAIT, &m_stBackLight_Z, BL_SP_INDEXER_Z);

					if ((CMS896AStn::MotionReportErrorStatus(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z) == 0) && 
							CMS896AStn::MotionIsPowerOn(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z))
					{
						CMS896AStn::MotionSetPosition(WAFERPR_BACKLIGHT_Z, 0, &m_stBackLight_Z);
						m_bBackLightHome_Z	= TRUE;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
			CMS896AStn::MotionClearError(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
			m_bBackLightComm_Z	= FALSE;
			m_bBackLightHome_Z	= FALSE;
			nResult = gnNOTOK; 
		}
	}
	else
	{
		if (m_bSelBL_Z)
		{
			Sleep(100);
			m_bBackLightComm_Z	= TRUE;
			m_bBackLightHome_Z	= TRUE;
		}
	}

	return nResult;
}


INT CWaferPr::Z_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_fHardware && IsWprWithAF())
	{
		try 
		{
			if (bOn == TRUE)
			{
				if (CMS896AStn::MotionIsPowerOn(WAFERPR_AXIS_Z, &m_stZoomAxis_Z) == FALSE)
				{
					CMS896AStn::MotionPowerOn(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(WAFERPR_AXIS_Z, &m_stZoomAxis_Z) == TRUE)
				{
					CMS896AStn::MotionPowerOff(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}

INT CWaferPr::BLZ_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;

	if (m_fHardware && IsBLInUse())
	{
		try 
		{
			if (bOn == TRUE)
			{
				if (CMS896AStn::MotionIsPowerOn(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z) == FALSE)
				{
					CMS896AStn::MotionPowerOn(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
				}
			}
			else
			{
				if (CMS896AStn::MotionIsPowerOn(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z) == TRUE)
				{
					CMS896AStn::MotionPowerOff(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
			m_bBackLightComm_Z	= FALSE;
			m_bBackLightHome_Z	= FALSE;
			nResult = gnNOTOK;
		}	
	}
	return nResult;
}


INT CWaferPr::Z_Move(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && IsWprWithAF())
	{
		try
		{
			if (!m_bZoomHome_Z)
			{
				nResult = gnNOTOK;
			}

			Z_Sync();
			WPR_GetEncoderValue();
			LONG lTarget = m_lEnc_AF + nPos;
			if (lTarget >= m_lAFZPosLimit || lTarget <= m_lAFZNegLimit)
			{
				CString szMsg;
				szMsg.Format("Position target is %d, distance is %d wrong!", lTarget, nPos);
				HmiMessage_Red_Back(szMsg);
				nResult = gnNOTOK;
			}

			if (IsWLExpanderOpen() == TRUE)
			{
				SetErrorMessage("Expander not closed, can not move focus z motor");
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMove(WAFERPR_AXIS_Z, nPos, SFM_NOWAIT, &m_stZoomAxis_Z);

				if (nMode == SFM_WAIT)
				{
					//Klocwork
					nResult = CMS896AStn::MotionSync(WAFERPR_AXIS_Z, 10000, &m_stZoomAxis_Z);
					if (nResult != gnOK)
					{
						nResult = gnNOTOK;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);

			m_bZoomHome_Z	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (IsWprWithAF())
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	return nResult;
}

INT CWaferPr::BLZ_Move(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && IsBLInUse())
	{
		try
		{
			if (!m_bBackLightHome_Z)
			{
				nResult = gnNOTOK;
			}

			BLZ_Sync();

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMove(WAFERPR_BACKLIGHT_Z, nPos, SFM_NOWAIT, &m_stBackLight_Z);

				if (nMode == SFM_WAIT)
				{
					//Klocwork
					nResult = CMS896AStn::MotionSync(WAFERPR_BACKLIGHT_Z, 10000, &m_stBackLight_Z);
					if (nResult != gnOK)
					{
						nResult = gnNOTOK;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);

			m_bBackLightComm_Z	= FALSE;
			m_bBackLightHome_Z	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (IsBLInUse())
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	return nResult;
}


INT CWaferPr::Z_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && IsWprWithAF())
	{
		try
		{
			if (!m_bZoomHome_Z)
			{
				nResult	= gnNOTOK;
			}

			Z_Sync();
			if ((nPos) >= m_lAFZPosLimit || (nPos) <= m_lAFZNegLimit)
			{
				CString szMsg;
				szMsg.Format("Position target is %d wrong!", nPos);
				HmiMessage_Red_Back(szMsg);
				nResult = gnNOTOK;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMoveTo(WAFERPR_AXIS_Z, nPos, SFM_NOWAIT, &m_stZoomAxis_Z);

				if (nMode == SFM_WAIT)
				{
					//Klocwork
					nResult = CMS896AStn::MotionSync(WAFERPR_AXIS_Z, 10000, &m_stZoomAxis_Z);
					if (nResult != gnOK)
					{
						nResult = gnNOTOK;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
			CMS896AStn::MotionClearError(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);

			m_bZoomHome_Z	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (IsWprWithAF())
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	return nResult;
}

INT CWaferPr::BLZ_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && IsBLInUse())
	{
		try
		{
			if (!m_bBackLightHome_Z)
			{
				nResult	= gnNOTOK;
			}

			BLZ_Sync();
			//if( (nPos)>=m_lAFZPosLimit || (nPos)<=m_lAFZNegLimit )
			//{
			//	CString szMsg;
			//	szMsg.Format("Position target is %d wrong!", nPos);
			//	HmiMessage_Red_Back(szMsg);
			//	nResult = gnNOTOK;
			//}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionMoveTo(WAFERPR_BACKLIGHT_Z, nPos, SFM_NOWAIT, &m_stBackLight_Z);

				if (nMode == SFM_WAIT)
				{
					//Klocwork
					nResult = CMS896AStn::MotionSync(WAFERPR_BACKLIGHT_Z, 10000, &m_stBackLight_Z);
					if (nResult != gnOK)
					{
						nResult = gnNOTOK;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
			CMS896AStn::MotionClearError(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);

			m_bBackLightComm_Z	= FALSE;
			m_bBackLightHome_Z	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
		if (IsBLInUse())
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(50);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	return nResult;
}


INT CWaferPr::Z_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && IsWprWithAF())
	{
		if (IsMotionCE() == TRUE)
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(WAFERPR_AXIS_Z, 10000, &m_stZoomAxis_Z);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
			nResult = gnNOTOK;
		}	
	}

	if (nResult != gnOK)
	{
		SetMotionCE(TRUE, "WPR Z Motor sync timeout(Z_Sync)");
	}
	return nResult;
}

INT CWaferPr::BLZ_Sync()
{
	INT nResult = gnOK;
	if (m_fHardware && IsBLInUse())
	{
		if (IsMotionCE() == TRUE)
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			CMS896AStn::MotionSync(WAFERPR_BACKLIGHT_Z, 10000, &m_stBackLight_Z);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
			nResult = gnNOTOK;
		}	
	}

	if (nResult != gnOK)
	{
		SetMotionCE(TRUE, "WPR BLZ_Sync Motor sync timeout(Z_Sync)");
	}
	return nResult;
}


// Function to Move the motor for specific step count
INT CWaferPr::Z_Zoom_Move(INT nPos, INT nMode, BOOL bCheckHome)
{
	INT nResult	= gnOK;

	if (!m_fHardware)
	{
		return nResult;
	}

	//if (m_pStepper_Zoom_Z)
	//{
	if (bCheckHome && (m_bZoomHome_Z == FALSE))
	{
		return gnNOTOK;
	}
		
	try
	{
		//LONG lCurPos = m_pStepper_Zoom_Z->GetPosition();
		LONG lCurPos = CMS896AStn::MotionGetPosition("WaferPrZoomZAxis", NULL);

		// Check for Software Bound for Zoom
		if ((nPos + lCurPos > WPR_ZOOM_POSITIVE_LIMIT) || (nPos + lCurPos < WPR_ZOOM_NEGATIVE_LIMIT))
		{
			CString szContent, szTitle;
			szContent.LoadString(HMB_WPR_CAMERA_LEVEL_ERR);
			szTitle.LoadString(HMB_GENERAL_ERROR);
			HmiMessage(szContent , szTitle);
			return gnNOTOK;
		}
	
		//m_pStepper_Zoom_Z->Move(nPos, SFM_NOWAIT);
		CMS896AStn::MotionMove("WaferPrZoomZAxis", nPos, SFM_NOWAIT, NULL);
			
		if (nMode == SFM_WAIT)
		{
			//if ((nResult = SyncStepper(m_pStepper_Zoom_Z, 10000)) != gnOK)
			if ((nResult = CMS896AStn::MotionSync("WaferPrZoomZAxis", 10000, NULL)) != gnOK)
			{
				nResult = Err_ZoomMove;
			}
		}
			
		//update the position for showing in hmi
		//m_lEnc_Zoom = m_pStepper_Zoom_Z ->GetPosition();
		m_lEnc_Zoom = CMS896AStn::MotionGetPosition("WaferPrZoomZAxis", NULL);

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//CheckHiPECResult("WaferPrZoomZAxis");
		//ClearStepperError("stpWaferPrZoomZ");
		CMS896AStn::MotionCheckResult("WaferPrZoomZAxis", NULL);
		CMS896AStn::MotionClearError("WaferPrZoomZAxis", NULL);

		m_bZoomHome_Z	= FALSE;
		nResult = Err_ZoomMove;
	}
	//}

	return nResult;
}

// Function to Move the motor for specific step count
INT CWaferPr::Z_Focus_Move(INT nPos, INT nMode, BOOL bCheckHome)
{
	INT nResult	= gnOK;

	if (!m_fHardware)
	{
		return nResult;
	}

	//if (m_pStepper_Focus_Z)
	//{
	if (bCheckHome && (m_bFocusHome_Z == FALSE))
	{
		return gnNOTOK;
	}

	try
	{
		//LONG lCurPos = m_pStepper_Focus_Z->GetPosition();
		LONG lCurPos = CMS896AStn::MotionGetPosition("WaferPrFocusZAxis", NULL);

		// Check for Software Bound for Focus
		if ((nPos + lCurPos > WPR_FOCUS_POSITIVE_LIMIT) || (nPos + lCurPos < WPR_FOCUS_NEGATIVE_LIMIT))
		{
			CString szContent, szTitle;
			szContent.LoadString(HMB_WPR_CAMERA_LEVEL_ERR);
			szTitle.LoadString(HMB_GENERAL_ERROR);
			HmiMessage(szContent , szTitle);
			return gnNOTOK;
		}

		//m_pStepper_Focus_Z->Move(nPos, SFM_NOWAIT);
		CMS896AStn::MotionMove("WaferPrFocusZAxis", nPos, SFM_NOWAIT, NULL);

		if (nMode == SFM_WAIT)
		{
			//if ((nResult = SyncStepper(m_pStepper_Focus_Z, 10000)) != gnOK)
			if ((nResult = CMS896AStn::MotionSync("WaferPrFocusZAxis", 10000, NULL)) != gnOK)
			{
				nResult = Err_FocusMove;
			}
		}

		//update the position for showing in hmi
		//m_lEnc_Focus = m_pStepper_Focus_Z ->GetPosition();
		CMS896AStn::MotionGetPosition("WaferPrFocusZAxis", NULL);

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//CheckHiPECResult("WaferPrFocusZAxis");
		//ClearStepperError("stpWaferPrFocusZ");
		CMS896AStn::MotionCheckResult("WaferPrFocusZAxis", NULL);
		CMS896AStn::MotionClearError("WaferPrFocusZAxis", NULL);

		m_bFocusHome_Z	= FALSE;
		nResult = Err_FocusMove;
	}
	//}

	return nResult;
}


/*
BOOL CWaferPr::Log_AlignWafer(CONST CString szMsg)
{
	if (CMS896AApp::m_bEnableMachineLog != TRUE)
		return TRUE;

	CStdioFile fLogFile;
	if (fLogFile.Open(_T("c:\\mapsorter\\UserData\\History\\WT_ALIGN_WAFER.log"), 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		fLogFile.SeekToEnd();

		CString szDate;
		CString szYear, szMonth;
		CTime CurTime = CTime::GetCurrentTime();

		szYear.Format("%d", CurTime.GetYear()); 
		szYear = szYear.Right(2);
		szMonth.Format("%d", CurTime.GetMonth());
		if ( CurTime.GetMonth() < 10 )
			szMonth = "0" + szMonth;
		szDate = CurTime.Format("[%H:%M:%S %d/%m/%y] ");

		fLogFile.WriteString(szDate + szMsg + "\n");
		fLogFile.Close();
		return TRUE;
	}
	return FALSE;
}
*/

INT CWaferPr::UpdateDirectCornerSearchPos(INT lX, INT lY)
{
	IPC_CServiceMessage stMsg;
	INT nConvID = 0;
	BOOL bReturn = TRUE;

	typedef	struct 
	{
		LONG	lX;
		LONG	lY;
		BOOL	bDirectCornerSearchDie; 
	} SEARCH_INFO;

	SEARCH_INFO stInfo;

	stInfo.lX = lX;
	stInfo.lY = lY;
	stInfo.bDirectCornerSearchDie = FALSE;

	
	stMsg.InitMessage(sizeof(SEARCH_INFO), &stInfo);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "UpdateDirectCornerSearchPos", stMsg);

	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
	}
	
	return 1;
}


VOID CWaferPr::MoveBhToPick(CONST BOOL bPick, CONST LONG lOffset)
{
	// Move BH to BON for learning collet hole pattern
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;

	typedef struct 
	{
		BOOL		bPick;
		LONG		lOffset;
	} PICK_TYPE;

	PICK_TYPE stInfo;
	stInfo.bPick = bPick;
	stInfo.lOffset = lOffset;
	stMsg.InitMessage(sizeof(PICK_TYPE), &stInfo);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveBHToPick1", stMsg);
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
}

//dual arm learn collet
VOID CWaferPr::MoveBhToPick_Z2(CONST BOOL bPick)
{
	// Move BH to BON for learning collet hole pattern
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;
	BOOL bBhToPick = bPick;

	stMsg.InitMessage(sizeof(BOOL), &bBhToPick);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveBHToPick_Z2", stMsg);
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
}

BOOL CWaferPr::WPR_GetFrameLevel()
{
	BOOL bIsUp;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "GetFrameLevel" , stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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


BOOL CWaferPr::UploadPrPkgPreviewImage(CString szImageDestPath, BOOL bDieType, LONG lRefDieNo)
{

	BOOL bFoundImage = FALSE;
	LONG lDieNo = 0;
	
	//CTime curTime = CTime::GetCurrentTime();
	//CString szDateTime = curTime.Format("%y%m%d%H%M%S");

	if (bDieType == WPR_NORMAL_DIE)
	{
		lDieNo = WPR_GEN_NDIE_OFFSET + lRefDieNo;
	}
	else
	{
		lDieNo = WPR_GEN_RDIE_OFFSET + lRefDieNo;
	}

	if (lDieNo > WPR_MAX_DIE)
	{
		return FALSE;
	}

	lDieNo = min(lDieNo, WPR_MAX_DIE - 1);	//Klocwork	//v4.02T5

	PR_UWORD ssID = m_ssGenPRSrchID[lDieNo];

	if (ssID == 0)
	{
		return FALSE;
	}

	CString szIndex = "";
	CString szDestPath = "";
	INT nIndex = 0;

	LONG lID = (LONG) ssID;
	CString szID = "";

	szID.Format("%d", lID);
	if (lID < 10)
	{
		szID = "00" + szID;
	}
	else if (lID < 100)
	{
		szID = "0" + szID;
	}

	//Source path
	CString szSourcePath = WPR_PR_RECORD_PATH;
	szSourcePath = szSourcePath + szID;

	if ((_access(szSourcePath + "\\log\\buffer00.bmp", 0)) != -1)
	{	
		szSourcePath += "\\log\\buffer00.bmp";
		bFoundImage = TRUE;
	}
	else if ((_access(szSourcePath + "\\log\\buffer01.bmp", 0)) != -1)
	{
		szSourcePath += "\\log\\buffer01.bmp";
		bFoundImage = TRUE;
	}

	// remove no use file in the folder first
	//RemoveFilesInFolder(WPR_SHOW_IMAGE_PATH);

	if (bFoundImage == FALSE)
	{
		return FALSE;
	}

	CopyFileWithRetry(szSourcePath, szImageDestPath, FALSE);

	return TRUE;
}

BOOL CWaferPr::ShowPrImage(CString &szImagePath, LONG &lDieNo, BOOL bDieType, LONG lRefDieNo)
{
	BOOL bFoundImage = FALSE;
	//LONG lDieNo = 0;
	
	CTime curTime = CTime::GetCurrentTime();
	CString szDateTime = curTime.Format("%y%m%d%H%M%S");

	if (bDieType == WPR_NORMAL_DIE)
	{
		lDieNo = WPR_GEN_NDIE_OFFSET + lRefDieNo;
	}
	else
	{
		lDieNo = WPR_GEN_RDIE_OFFSET + lRefDieNo;
	}

	if (lDieNo > WPR_MAX_DIE)
	{
		//RemoveFilesInFolder(WPR_SHOW_IMAGE_PATH);
		//m_szShowImagePath[lDieNo] = "";
		//m_szGenShowImagePath		= "";
		szImagePath = "";
		return FALSE;
	}

	lDieNo = min(lDieNo, WPR_MAX_DIE - 1);	//Klocwork	//v4.02T5

	PR_UWORD ssID = m_ssGenPRSrchID[lDieNo];

	if (ssID == 0)
	{
		//RemoveFilesInFolder(WPR_SHOW_IMAGE_PATH);
		//m_szShowImagePath[lDieNo]	= "";
		//m_szGenShowImagePath		= "";
		szImagePath = "";
		return TRUE;
	}

	CString szIndex = "";
	CString szDestPath = "";
	INT nIndex = 0;

	LONG lID = (LONG) ssID;
	CString szID = "";

	szID.Format("%d", lID);
	if (lID < 10)
	{
		szID = "00" + szID;
	}
	else if (lID < 100)
	{
		szID = "0" + szID;
	}

	//Source path
	CString szSourcePath = WPR_PR_RECORD_PATH;
	szSourcePath = szSourcePath + szID;

	if ((_access(szSourcePath + "\\log\\buffer00.bmp", 0)) != -1)
	{	
		szSourcePath += "\\log\\buffer00.bmp";
		bFoundImage = TRUE;
	}
	else if ((_access(szSourcePath + "\\log\\buffer01.bmp", 0)) != -1)
	{
		szSourcePath += "\\log\\buffer01.bmp";
		bFoundImage = TRUE;
	}
	
	szDestPath = WPR_SHOW_IMAGE_PATH;
	CreateDirectory(WPR_SHOW_IMAGE_PATH, NULL);

	// remove no use file in the folder first
	//RemoveFilesInFolder(WPR_SHOW_IMAGE_PATH);

	if (bFoundImage == FALSE)
	{
		//m_szShowImagePath[lDieNo] = "";
		szImagePath = "";
		return TRUE;
	}

	szDestPath = szDestPath + "\\WPR" + szDateTime + szID + ".bmp";
	szImagePath = szDestPath;

	//m_szGenShowImagePath	  = szDestPath;		
	//m_szShowImagePath[lDieNo] = szDestPath;	// Update HMI display path in 

	// Copy PR record bitmap to Mapsorter sub-folder for display
	CopyFileWithRetry(szSourcePath, szDestPath, FALSE);

	return TRUE;
}

BOOL CWaferPr::UpdatePKGFileRecordImage(PR_WORD ssID, BOOL bIsNormalDie)
{
	CTime curTime = CTime::GetCurrentTime();
	CString szDateTime = curTime.Format("%y%m%d%H%M%S");

	if (ssID == 0)
	{
		return TRUE;
	}

	CString szIndex = "";
	CString szDestPath = "";
	INT nIndex = 0;

	LONG lID = (LONG) ssID;
	CString szID = "";

	szID.Format("%d", lID);
	if (lID < 10)
	{
		szID = "00" + szID;
	}
	else if (lID < 100)
	{
		szID = "0" + szID;
	}

	//Source path
	CString szSourcePath = WPR_PR_RECORD_PATH;
	szSourcePath = szSourcePath + szID;

	if ((_access(szSourcePath + "\\log\\buffer00.bmp", 0)) != -1)
	{	
		szSourcePath += "\\log\\buffer00.bmp";
	}
	else if ((_access(szSourcePath + "\\log\\buffer01.bmp", 0)) != -1)
	{
		szSourcePath += "\\log\\buffer01.bmp";
	}
	
	szDestPath = WPR_PKG_FILE_IMAGE_PATH;
	CreateDirectory(WPR_PKG_FILE_IMAGE_PATH, NULL);

	// remove no use file in the folder first
	RemoveFilesInFolder(WPR_PKG_FILE_IMAGE_PATH);

	if (bIsNormalDie == TRUE)
	{
		
		szDestPath = szDestPath + "\\Normal" + szDateTime + ".bmp";
			
		m_szPKGNormalDieRecordPath = szDestPath;	// Update HMI display path in 

		// Copy PR record bitmap to Mapsorter sub-folder for display
		CopyFileWithRetry(szSourcePath, szDestPath, FALSE);
	}
	else
	{
		szDestPath = szDestPath + "\\Reference" + szDateTime + ".bmp";
		m_szPKGRefDieRecordPath = szDestPath;
		CopyFileWithRetry(szSourcePath, szDestPath, FALSE);
	}
	
	return TRUE;
}


BOOL CWaferPr::Search2DBarCodePatternFromWafer(CString &szCode, CONST BOOL bRetry, BOOL bMirrorMode)
{
	PR_READ_2DCODE_CMD	stRead2DCmd;
	PR_READ_2DCODE_RPY1	stRead2DRpy1;
	PR_READ_2DCODE_RPY2	stRead2DRpy2; 

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();

	PR_InitRead2DCodeCmd(&stRead2DCmd);

	stRead2DCmd.emPurpose				= ubPpsG;
	stRead2DCmd.stSrchWin.coCorner1.x	= GetPRWinULX() + WPR_2D_BARCODE_SEARCH_MARG;
	stRead2DCmd.stSrchWin.coCorner1.y	= GetPRWinULY() + WPR_2D_BARCODE_SEARCH_MARG;
	stRead2DCmd.stSrchWin.coCorner2.x	= GetPRWinLRX() - WPR_2D_BARCODE_SEARCH_MARG;
	stRead2DCmd.stSrchWin.coCorner2.y	= GetPRWinLRY() - WPR_2D_BARCODE_SEARCH_MARG;

	stRead2DCmd.emGraphicInfo			= PR_DISPLAY_CENTRE;
	stRead2DCmd.emSameView				= PR_FALSE;
	stRead2DCmd.emType					= PR_2D_CODE_TYPE_0;	//PR_2D_CODE_TYPE_12;	//v4.50A2 
	stRead2DCmd.emOpMode				= PR_2D_CODE_OPMODE_NORMAL;
	if (bMirrorMode == TRUE)	
		stRead2DCmd.emMirrorImage		= PR_TRUE;		//PLLM REBEL
	else
		stRead2DCmd.emMirrorImage		= PR_FALSE;		//v4.43T8

	if (bRetry)
	{
		stRead2DCmd.emOpMode			= PR_2D_CODE_OPMODE_LIGHTING_RETRY;
	}

	PR_Read2DCodeCmd(&stRead2DCmd, ubSID, ubRID, &stRead2DRpy1);

	if ((stRead2DRpy1.uwCommunStatus != PR_COMM_NOERR) || 
			(stRead2DRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		CString csMsg;
		csMsg.Format("WPR PR_Read2DCodeCmd Get reply1 error = 0x%x, 0x%x", stRead2DRpy1.uwCommunStatus, stRead2DRpy1.uwPRStatus);
		AfxMessageBox(csMsg, MB_SYSTEMMODAL);
		return FALSE;
	}

	PR_Read2DCodeRpy(ubSID, ubRID, &stRead2DRpy2);

	if ((stRead2DRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) ||
			(stRead2DRpy2.stStatus.uwPRStatus != PR_ERR_NOERR))
	{
		CString csMsg;
		csMsg.Format("WPR PR_Read2DCodeRpy error = 0x%x, 0x%x", stRead2DRpy2.stStatus.uwCommunStatus, stRead2DRpy2.stStatus.uwPRStatus);
		SetErrorMessage(csMsg);
		return FALSE;
	}

	szCode = stRead2DRpy2.aubCodeStr;
	return TRUE;
}

// 2D barcode fcn
BOOL CWaferPr::Search2DBarCodePattern(CString &szCode, UINT unID, CONST BOOL bRetry, BOOL bMirrorMode)
{
	PR_READ_2DCODE_CMD	stRead2DCmd;
	PR_READ_2DCODE_RPY1	stRead2DRpy1;
	PR_READ_2DCODE_RPY2	stRead2DRpy2; 

	PR_InitRead2DCodeCmd(&stRead2DCmd);

	PR_UBYTE	ubSendID	= PSPR_SENDER_ID;
	PR_UBYTE	ubRecvID	= PSPR_RECV_ID;
	PR_PURPOSE	ubPrPurPose = MS899_POSTSEAL_PR_PURPOSE;
	switch( m_unCurrPostSealID )
	{
	case 2:
		ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
		break;
	case 3:
		ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
		break;
	default:
		break;
	}

	stRead2DCmd.emPurpose				= ubPrPurPose;
	stRead2DCmd.stSrchWin.coCorner1.x	= GetPRWinULX() + WPR_2D_BARCODE_SEARCH_MARG;
	stRead2DCmd.stSrchWin.coCorner1.y	= GetPRWinULY() + WPR_2D_BARCODE_SEARCH_MARG;
	stRead2DCmd.stSrchWin.coCorner2.x	= GetPRWinLRX() - WPR_2D_BARCODE_SEARCH_MARG;
	stRead2DCmd.stSrchWin.coCorner2.y	= GetPRWinLRY() - WPR_2D_BARCODE_SEARCH_MARG;

	stRead2DCmd.emGraphicInfo			= PR_DISPLAY_CENTRE;
	stRead2DCmd.emSameView				= PR_FALSE;
	stRead2DCmd.emType					= PR_2D_CODE_TYPE_0; 
	stRead2DCmd.emOpMode				= PR_2D_CODE_OPMODE_NORMAL;
	if (bMirrorMode == TRUE)	
		stRead2DCmd.emMirrorImage		= PR_TRUE;		//PLLM REBEL
	else
		stRead2DCmd.emMirrorImage		= PR_FALSE;		//v4.43T8

	if (bRetry)
	{
		stRead2DCmd.emOpMode			= PR_2D_CODE_OPMODE_LIGHTING_RETRY;
	}

	PR_Read2DCodeCmd(&stRead2DCmd, ubSendID, ubRecvID, &stRead2DRpy1);

	if ((stRead2DRpy1.uwCommunStatus != PR_COMM_NOERR) || 
			(stRead2DRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		CString csMsg;
		csMsg.Format("WPR PR_Read2DCodeCmd Get reply1 error = 0x%x, 0x%x", stRead2DRpy1.uwCommunStatus, stRead2DRpy1.uwPRStatus);
		AfxMessageBox(csMsg, MB_SYSTEMMODAL);
		return FALSE;
	}

	PR_Read2DCodeRpy(ubSendID, ubRecvID, &stRead2DRpy2);

	if ((stRead2DRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) ||
			(stRead2DRpy2.stStatus.uwPRStatus != PR_ERR_NOERR))
	{
		CString csMsg;
		csMsg.Format("WPR PR_Read2DCodeRpy error = 0x%x, 0x%x", stRead2DRpy2.stStatus.uwCommunStatus, stRead2DRpy2.stStatus.uwPRStatus);
		SetErrorMessage(csMsg);
		return FALSE;
	}

	szCode = stRead2DRpy2.aubCodeStr;
	return TRUE;
}

BOOL CWaferPr::GetPostSealLighting(UINT unID)
{
	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->GetFeatureStatus(MS896A_FUNC_VISION_POSTSEAL_OPTICS))
	{
		return FALSE;
	}

	//Only get PR lighting here because GEN lighting may be ON/OFF
	PR_UBYTE	ubSendID	= PSPR_SENDER_ID;
	PR_UBYTE	ubRecvID	= PSPR_RECV_ID;
	PR_PURPOSE	ubPrPurPose = MS899_POSTSEAL_PR_PURPOSE;
	switch( unID )
	{
	case 2:
		ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
		break;
	case 3:
		ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
		break;
	case 6:
		// 4.52D17ID  Get Purpose ID
		ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;	//MS899_POSTSEAL_BH1_GEN_PURPOSE;	//andrewng6
		//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Get Lighting ID 6");
		break;
	case 7:
		ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;	//MS899_POSTSEAL_BH2_GEN_PURPOSE;	//andrewng6
		//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Get Lighting ID 7");
		break;
	default:
		break;
	}

	//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- PR: Preparing Get Optic");
	PR_GetOptic(ubPrPurPose, ubSendID, ubRecvID, &stOptic, &stRpy);

	if ((stRpy.uwCommunStatus != PR_COMM_NOERR) || (stRpy.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog; 
		szLog.Format("WPR: get post seal Optics reply failure - Comm= 0x%x, PR = 0x%x",stRpy.uwCommunStatus, stRpy.uwPRStatus);
		HmiMessage(szLog);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		return FALSE;
	}

	PR_UWORD		usWPRCoaxLightLevel;
	PR_UWORD		usWPRRingLightLevel;
	PR_UWORD		usWPRSideLightLevel;
	PR_SOURCE		emCoaxID, emRingID, emSideID;

	emCoaxID = PR_COAXIAL_LIGHT;
	emRingID = PR_RING_LIGHT;
	emSideID = PR_SIDE_LIGHT;

	PR_GetLighting(&stOptic, emCoaxID, ubSendID, ubRecvID, &usWPRCoaxLightLevel, &stRpy);
	PR_GetLighting(&stOptic, emRingID, ubSendID, ubRecvID, &usWPRRingLightLevel, &stRpy);
	PR_GetLighting(&stOptic, emSideID, ubSendID, ubRecvID, &usWPRSideLightLevel, &stRpy);

	//Update HMI PR Indirect View parameters
	//4.52D17light Save value  (Get Post Seal Lighting)
	m_lWPRGenCoaxLightLevel[0]		= usWPRCoaxLightLevel;
	m_lWPRGenRingLightLevel[0]		= usWPRRingLightLevel;
	m_lWPRGenSideLightLevel[0]		= usWPRSideLightLevel;

	CString szMsg;
	//szMsg.Format("Uplook --- get light Coax %d Ring %d Side %d", usWPRCoaxLightLevel, usWPRRingLightLevel, usWPRSideLightLevel);
	//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	SetPostSealLighting(TRUE, unID);		//Set PR lighting to GEN lighting for Live Video

	return TRUE;
}


BOOL CWaferPr::SetPostSealLighting(CONST BOOL bOn, UINT unID)
{
	LONG lCoaxLightLevel, lRingLightLevel, lSideLightLevel;
	PR_SOURCE emCoaxID, emRingID, emSideID;
	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stGnOptic;
	PR_OPTIC		stPrOptic;

	emCoaxID = PR_COAXIAL_LIGHT;
	emRingID = PR_RING_LIGHT;
	emSideID = PR_SIDE_LIGHT;

	if (bOn)
	{
		lCoaxLightLevel = m_lWPRGenCoaxLightLevel[0];	//m_lPPRGenCoaxLightLevel;
		lRingLightLevel = m_lWPRGenRingLightLevel[0];	//m_lPPRGenRingLightLevel;
		lSideLightLevel = m_lWPRGenSideLightLevel[0];	//m_lPPRGenSideLightLevel;
	}
	else
	{
		lCoaxLightLevel = 0;
		lRingLightLevel = 0;
		lSideLightLevel = 0;
	}

	PR_UBYTE	ubSendID	= PSPR_SENDER_ID;
	PR_UBYTE	ubRecvID	= PSPR_RECV_ID;
	PR_PURPOSE	ubGnPurpose	= MS899_POSTSEAL_GEN_PURPOSE;
	PR_PURPOSE	ubPrPurPose = MS899_POSTSEAL_PR_PURPOSE;
	switch( unID )
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
		//4.52D17ID Set Gn Pr Purpose ID
		ubGnPurpose	= MS899_POSTSEAL_BH1_GEN_PURPOSE; 
		ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
		//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Set Lighting ID 6");
		break;
	case 7:
		ubGnPurpose	= MS899_POSTSEAL_BH2_GEN_PURPOSE;
		ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
		//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Set Lighting ID 7");
		break;
	default:
		break;
	}

	CString szLog;
	szLog.Format("Uplook --- Set Lighting state -- bOn:%d, unID:%d",bOn, unID);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog); 
	//4.52D17light Set PR_value
	PR_GetOptic(ubGnPurpose, ubSendID, ubRecvID, &stGnOptic, &stRpy);	
	PR_GetOptic(ubPrPurPose, ubSendID, ubRecvID, &stPrOptic, &stRpy);

	PR_SetLighting(emCoaxID, (PR_UWORD)lCoaxLightLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
	PR_SetLighting(emRingID, (PR_UWORD)lRingLightLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
	PR_SetLighting(emSideID, (PR_UWORD)lSideLightLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);//Finisar CK confirm
	
	if (bOn)	//Only update PR lighting if bOn = TRUE
	{
		PR_COMMON_RPY	stRpy;

		PR_EnableHwTrigLighting(ubSendID, ubRecvID, &stRpy);

		PR_SetLighting(emCoaxID, (PR_UWORD)lCoaxLightLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		PR_SetLighting(emRingID, (PR_UWORD)lRingLightLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		PR_SetLighting(emSideID, (PR_UWORD)lSideLightLevel, ubSendID, ubRecvID, &stPrOptic, &stRpy);
	
		PR_EXPOSURE_TIME stTime = PR_EXPOSURE_TIME_3; //(PR_EXPOSURE_TIME) 0;
		PR_SetExposureTime(stTime, ubSendID, ubRecvID, &stPrOptic, &stRpy);
	}

	return TRUE;
}

//andrewng6
BOOL CWaferPr::SetPostSealGenLighting(CONST BOOL bOn, UINT unID)
{
	LONG lCoaxLightLevel, lRingLightLevel, lSideLightLevel;
	PR_SOURCE emCoaxID, emRingID, emSideID;
	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stGnOptic;
//	PR_OPTIC		stPrOptic;

	emCoaxID = PR_COAXIAL_LIGHT;
	emRingID = PR_RING_LIGHT;
	emSideID = PR_SIDE_LIGHT;

	if (bOn)
	{
		lCoaxLightLevel = m_lWPRGenCoaxLightLevel[0];	//m_lPPRGenCoaxLightLevel;
		lRingLightLevel = m_lWPRGenRingLightLevel[0];	//m_lPPRGenRingLightLevel;
		lSideLightLevel = m_lWPRGenSideLightLevel[0];	//m_lPPRGenSideLightLevel;
	}
	else
	{
		lCoaxLightLevel = 0;
		lRingLightLevel = 0;
		lSideLightLevel = 0;
	}

	PR_UBYTE	ubSendID	= PSPR_SENDER_ID;
	PR_UBYTE	ubRecvID	= PSPR_RECV_ID;
	PR_PURPOSE	ubGnPurpose	= MS899_POSTSEAL_GEN_PURPOSE;
	PR_PURPOSE	ubPrPurPose = MS899_POSTSEAL_PR_PURPOSE;
	switch( unID )
	{
	case 2:
		ubGnPurpose	= MS899_POSTSEAL_BH1_GEN_PURPOSE;
		//ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
		break;
	case 3:
		ubGnPurpose	= MS899_POSTSEAL_BH2_GEN_PURPOSE;
		//ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
		break;
	case 6: 
		//4.52D17ID Set Gn Pr Purpose ID
		ubGnPurpose	= MS899_POSTSEAL_BH1_GEN_PURPOSE; 
		ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
		//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Set Lighting ID 6");
		break;
	case 7:
		ubGnPurpose	= MS899_POSTSEAL_BH2_GEN_PURPOSE;
		ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
		//CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Set Lighting ID 7");
		break;
	default:
		break;
	}

	CString szLog;
	//szLog.Format("Uplook --- Set GEN Lighting state -- bOn:%d, unID:%d",bOn, unID);
	//CMSLogFileUtility::Instance()->MS_LogOperation(szLog); 
	
	//4.52D17light Set PR_value
	PR_GetOptic(ubGnPurpose, ubSendID, ubRecvID, &stGnOptic, &stRpy);	
	PR_SetLighting(emCoaxID, (PR_UWORD)lCoaxLightLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
	PR_SetLighting(emRingID, (PR_UWORD)lRingLightLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
	PR_SetLighting(emSideID, (PR_UWORD)lSideLightLevel, ubSendID, ubRecvID, &stGnOptic, &stRpy);
	
	if (!bOn)
	{
		PR_COMMON_RPY	stRpy;

		PR_DisableHwTrigLighting(ubSendID, ubRecvID, &stRpy);
	}

	return TRUE;
}

VOID CWaferPr::SetEjectorVacuum(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
	if (m_bDisableWT)	//v3.61
	{
		return;
	}

	//CMS896AStn::MotionSetOutputBit("oEjectorVacuum", bSet);
	return;
}

LONG CWaferPr::LogItems(LONG lEventNo)
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

LONG CWaferPr::GetLogItemsString(LONG lEventNo, CString &szMsg)
{
	CString szTemp;
	CString szDieType;
	CString szDieNo;

	if (m_bSelectDieType == WPR_NORMAL_DIE)
	{
		szDieType = "Normal Die";
	}
	else
	{
		szDieType = "Reference Die";
	}

	szDieNo.Format("%d", m_lCurRefDieNo);
	
	switch (lEventNo)
	{
		case WPR_LOOK_FORWARD:
			if (m_bSrchEnableLookForward)
			{
				szMsg = "Look Forward Function,On";
			}
			else
			{
				szMsg = "Look Forward Function,Off";
			}
			break;

		case WPR_WAFER_THETA_CORRECTION:
			if (m_bThetaCorrection)
			{
				szMsg = "Theta Correction Function,On";
			}
			else 
			{
				szMsg = "Theta Correction Function,Off";
			}
			break;

		case WPR_NO_DIE_MAX_SKIP_COUNT:
			szMsg.Format("Wafer Pr No. Max Skip Count,%d", m_ulMaxNoDieSkipCount);
			break;

		case WPR_PR_LEARN_DIE_LIGHT_COX:
			szTemp.Format("%d", m_lWPRLrnCoaxLightHmi);
			szMsg = szDieType + " Wafer Pr Coaxial Light (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_PR_LEARN_DIE_LIGHT_RING:
			szTemp.Format("%d", m_lWPRLrnRingLightHmi);
			szMsg = szDieType + " Wafer Pr Ring Light (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_PR_LEARN_DIE_LIGHT_SIDE:
			szTemp.Format("%d", m_lWPRLrnSideLightHmi);
			szMsg = szDieType + " Wafer Pr Side Light (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_DIE_PASS_SCORE:
			szTemp.Format("%d", m_lSrchDieScore);
			szMsg = szDieType + " Pass Score (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_INSP_CHECK_DEFECTIVE:
			if (m_bSrchEnableDefectCheck)
			{
				szTemp = "On";
			}
			else
			{
				szTemp = "Off";
			}
			szMsg = szDieType + " Check Defect (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_INSP_CHECK_DEFECTIVE_CHIP:
			if (m_bSrchEnableChipCheck)
			{
				szTemp = "On";
			}
			else
			{
				szTemp = "Off";
			}
			szMsg = szDieType + " Check Chip (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_INSP_CHECK_DEFECTIVE_CHIP_AREA:
			szTemp.Format("%.2f", m_dSrchChipArea);
			szMsg = szDieType + " Check Chip Area (Die No: " + szDieNo + ")," + szTemp;
			break;
		

		case WPR_INSP_CHECK_DEFECTIVE_SINGLE_DEFECT:
			szTemp.Format("%.2f", m_dSrchSingleDefectArea);
			szMsg = szDieType + " Check Single Defect (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_INSP_CHECK_DEFECTIVE_TOTAL_DEFECT:
			szTemp.Format("%.2f", m_dSrchTotalDefectArea);
			szMsg = szDieType + " Check Total Defect (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_GREY_LEVEL_TYPE:
		
			if (m_lSrchGreyLevelDefect == 0)
			{
				szTemp = "Both";
			}
			else if (m_lSrchGreyLevelDefect == 1)
			{
				szTemp = "Bright";
			}
			else
			{
				szTemp = "Dark";
			}
			szMsg = szDieType + " Grey Level Defect (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_DIE_CHECK_DEFECTIVE_THRESHOLD:
			szTemp.Format("%d", m_lSrchDefectThres);
			szMsg = szDieType + " Check Defect Threshold (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_BAD_CUT_TOLERANCE_X:
			szMsg.Format("Wafer Pr Bad Cut Tolerance X,%d", m_stBadCutTolerenceX);
			break;

		case WPR_BAD_CUT_TOLERANCE_Y:
			szMsg.Format("Wafer Pr Bad Cut Tolerance Y,%d", m_stBadCutTolerenceY);
			break;

		case WPR_DIE_MAX_SKIP_COUNT:
			szMsg.Format("Wafer Pr Die Max Skip Count,%d", m_ulMaxPRSkipCount);
			break;

		case WPR_PRESCAN_SEARCH_DIE_SCORE:
			szTemp.Format("%d", m_lPrescanSrchDieScore);
			szMsg = szDieType + " Prescan Search Die Score (Die No: " + szDieNo + ")," + szTemp;
			break;
	
		case WPR_SEARCH_AREA_X:
			szTemp.Format("%.1f", m_dSrchDieAreaX);
			szMsg = szDieType + " Wafer Pr Search Area X (Die No: " + szDieNo + ")," + szTemp;
			break;

		case WPR_SEARCH_AREA_Y:
			szTemp.Format("%.1f", m_dSrchDieAreaY);
			szMsg = szDieType + " Wafer Pr Search Area Y (Die No: " + szDieNo + ")," + szTemp;
			break;
	}
	return 1;
}

BOOL CWaferPr::SearchFirstDieInWafer()
{
	BOOL bDieFound = FALSE;
	LONG lResult = 0;
	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	
	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX() - 1, GetPRWinLRY() - 1};

	if (IsLFSizeOK() == TRUE)
	{
		int siOrigX = 0, siOrigY = 0, siOrigT = 0;
		int nX, nY;
		GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

		//for (int i=0; i<=WPR_SRH_FIRST_DIE_RETRY_WITH_LF; i++)
		for (int i = 0; i <= 7; i++)
		{
			switch (i)
			{
				case 0:		//LL corner
					nX = siOrigX - GetDiePitchXX();
					nY = siOrigY - GetDiePitchYY();
					break;
				case 1:		//LR Corner
					nX = siOrigX + GetDiePitchXX();
					nY = siOrigY - GetDiePitchYY();
					break;
				case 2:		//UL Corner
					nX = siOrigX - GetDiePitchXX();
					nY = siOrigY + GetDiePitchYY();
					break;
				case 3:		//UR Corner
					nX = siOrigX + GetDiePitchXX();
					nY = siOrigY + GetDiePitchYY();
					break;

					//v4.15T1
				case 4:		//LL corner x2
					nX = siOrigX - 2 * GetDiePitchXX();
					nY = siOrigY - 2 * GetDiePitchYY();
					break;
				case 5:		//LR Corner x2
					nX = siOrigX + 2 * GetDiePitchXX();
					nY = siOrigY - 2 * GetDiePitchYY();
					break;
				case 6:		//UL Corner x2
					nX = siOrigX - 2 * GetDiePitchXX();
					nY = siOrigY + 2 * GetDiePitchYY();
					break;
				case 7:		//UR Corner x2
					nX = siOrigX + 2 * GetDiePitchXX();
					nY = siOrigY + 2 * GetDiePitchYY();
					break;
			}

			MoveWaferTable(nX, nY);
			Sleep(500);

			lResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);

			if (lResult != -1)
			{
				if (DieIsAlignable(usDieType) == TRUE)
				{
					bDieFound = TRUE;
					break;
				}
			}
		}

		if (bDieFound != TRUE)
		{
			MoveWaferTable(siOrigX, siOrigY);
		}

		return bDieFound;
	}
	else
	{
		int nStepX = 0, nStepY = 0;
		int siOrigX = 0, siOrigY = 0, siOrigT = 0;
		int nX, nY;
		int nMoveFactor;
		GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

		for (int i = 0; i < WPR_SHR_FIRST_DIE_RETERY_LOOP; i++)
		{
			nMoveFactor = 2 * (WPR_SHR_FIRST_DIE_RETERY_LOOP - i);

			//Klocwork	//v4.05
			//if (nMoveFactor == 0)
			//{
			//	nMoveFactor = 1;
			//}

			nStepX = GetDiePitchXX() / nMoveFactor;
			nStepY = GetDiePitchYY() / nMoveFactor;
		
			for (int j = 0; j <= WPR_SRH_FIRST_DIE_RETRY_NO_LF; j++)
			{
				switch (j)
				{
					case 0: //Left
						nX = siOrigX - nStepX;
						nY = siOrigY;
						break;
					case 1:	// Right
						nX = siOrigX + nStepX;
						nY = siOrigY;
						break;
					case 2:	// Upper
						nX = siOrigX;
						nY = siOrigY - nStepY;
						break;
					case 3:	// Lower
						nX = siOrigX;
						nY = siOrigY + nStepY;
						break;
					case 4:// UL
						nX = siOrigX - nStepX;
						nY = siOrigY - nStepY;
						break;
					case 5://LL
						nX = siOrigX - nStepX;
						nY = siOrigY + nStepY;
						break;
					case 6://UR
						nX = siOrigX + nStepX;
						nY = siOrigY - nStepY;
						break;
					case 7://LR
						nX = siOrigX + nStepX;
						nY = siOrigY + nStepY;
						break;
				}
			
				MoveWaferTable(nX, nY);
				Sleep(500);

				lResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);

				if (lResult != -1)
				{
					if (DieIsAlignable(usDieType) == TRUE)
					{
						return TRUE;
					}
				}
			}
		}

		if (bDieFound != TRUE)
		{
			MoveWaferTable(siOrigX, siOrigY);
		}

		return FALSE;
	}
}

BOOL CWaferPr::HasReferDieCheck()
{
	return (CMS896AStn::m_bUseRefDieCheck == TRUE) && (m_lLrnTotalRefDie >= 1) && (m_bRefDieCheck == TRUE);
}

BOOL CWaferPr::HasSpRefDieCheck()
{
	return ((CMS896AStn::m_bUseRefDieCheck == TRUE) && (m_lLrnTotalRefDie > 1));
}

BOOL CWaferPr::IsNormalDieLearnt()
{
	return m_bGenDieLearnt[WPR_NORMAL_DIE];
}

BOOL CWaferPr::IsMS60NGPickEmptyHoleLearnt()	//v4.54A5
{
	return m_bGenDieLearnt[WPR_NORMAL_DIE+1];	//Use normal die pattern #2
}

DOUBLE CWaferPr::GetSrchDieAreaX()
{
	DOUBLE lSrchAreaX = m_lSrchDieAreaX;

	if (lSrchAreaX == 0)
	{
		lSrchAreaX = 4;
	}
	if (lSrchAreaX < 6 || lSrchAreaX > 8)		//v4.44A1	//SEmitek
	{
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == "Semitek")
		{
			lSrchAreaX = 7;
		}
	}

	return lSrchAreaX;
}

DOUBLE CWaferPr::GetSrchDieAreaY()
{
	DOUBLE lSrchAreaY = m_lSrchDieAreaY;
	if (lSrchAreaY == 0)
	{
		lSrchAreaY = 4;
	}
	if (lSrchAreaY < 6 || lSrchAreaY > 8)		//v4.44A1	//Semitek
	{
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == "Semitek")
		{
			lSrchAreaY = 7;
		}
	}

	return lSrchAreaY;
}

DOUBLE CWaferPr::GetGenSrchAreaX(INT nDieNo)
{
	nDieNo = min(nDieNo, WPR_MAX_DIE - 1);
	DOUBLE lSrchAreaX = m_lGenSrchDieAreaX[nDieNo];
	if (lSrchAreaX == 0)
	{
		lSrchAreaX = 4;
	}
	if (nDieNo >= 0 && nDieNo <= WPR_GEN_RDIE_OFFSET && (lSrchAreaX < 6 || lSrchAreaX > 8))		//v4.46T1
	{
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == "Semitek")
		{
			lSrchAreaX = 7;
		}
	}

	return lSrchAreaX;
}

DOUBLE CWaferPr::GetGenSrchAreaY(INT nDieNo)
{
	nDieNo = min(nDieNo, WPR_MAX_DIE - 1);
	DOUBLE lSrchAreaY = m_lGenSrchDieAreaY[nDieNo];
	if (lSrchAreaY == 0)
	{
		lSrchAreaY = 4;
	}
	if (nDieNo >= 0 && nDieNo <= WPR_GEN_RDIE_OFFSET && (lSrchAreaY < 6 || lSrchAreaY > 8))	//v4.46T1
	{
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == "Semitek")
		{
			lSrchAreaY = 7;
		}
	}

	return lSrchAreaY;
}

VOID CWaferPr::SetLockPrMouseJoystick(BOOL bLock)
{
	m_bLockPRMouseJoystick = bLock;
}

BOOL CWaferPr::GetLockPrMouseJoystick()
{
	return m_bLockPRMouseJoystick;
}

INT	 CWaferPr::GetDiePitchXX()	// Die Pitch X (X) Enc Pos
{
	return GetDiePitchX_X();
}

INT	 CWaferPr::GetDiePitchXY()	// Die Pitch X (Y) Enc Pos
{
	return GetDiePitchX_Y();
}

INT	 CWaferPr::GetDiePitchYY()	// Die Pitch Y (X) Enc Pos
{
	return GetDiePitchY_Y();
}

INT	 CWaferPr::GetDiePitchYX()	// Die Pitch Y (X) Enc Pos
{
	return GetDiePitchY_X();
}

LONG CWaferPr::GetPrCenterX()
{
	return m_lPrCenterX;
}

LONG CWaferPr::GetPrCenterY()
{
	return m_lPrCenterY;
}

VOID CWaferPr::SetPrCenterX(LONG lPrCenterX)
{
	m_lPrCenterX = lPrCenterX;
}

VOID CWaferPr::SetPrCenterY(LONG lPrCenterY)
{
	m_lPrCenterY = lPrCenterY;
}

VOID CWaferPr::GetES101WTEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis, BOOL bWT2)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lT;
	} ENCVAL;

	ENCVAL stEnc;

	int nConvID = 0;

	// Get the reply for the encoder value
	if (IsESDualWT())
	{
		if (bWT2)
		{
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetWT2EncoderCmd", stMsg);
		}
		else
		{
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetWT1EncoderCmd", stMsg);
		}
	}
	else
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetEncoderCmd", stMsg);
	}
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(ENCVAL), &stEnc);
	
			*lXAxis = stEnc.lX;
			*lYAxis = stEnc.lY;
			*lTAxis = stEnc.lT;
			break;
		}
		else
		{
			Sleep(10);
		}	
	}
}

BOOL CWaferPr::IsWT1UnderCamera()
{
	LONG lX, lY, lT;

	GetES101WTEncoder(&lX, &lY, &lT, FALSE);

	return IsWithinWT1WaferLimit(lX, lY);
}

BOOL CWaferPr::IsWT2UnderCamera()
{
	if (IsESDualWT() == FALSE)
	{
		return FALSE;
	}

	LONG lX, lY, lT;

	GetES101WTEncoder(&lX, &lY, &lT, TRUE);

	return IsWithinWT2WaferLimit(lX, lY);
}

BOOL CWaferPr::MotionTest()
{
	if (m_bBackLightElevatorMoveTest == FALSE)
	{
		return TRUE;
	}

	if (m_bBackLightElevatorMoveTest)
	{
		m_lBackLightElevatorMoveTestCount = m_lBackLightElevatorMoveTestCount + 1;

		if (m_bMoveDirection)
		{
			DOUBLE dTotalTime;
			DOUBLE dStartTime = GetTime();

			BLZ_MoveTo(m_lBackLightElevatorUpLevel);
			dTotalTime = GetTime() - dStartTime;
			Sleep(100);
			WPR_GetEncoderValue();

			CString szTemp;
			szTemp.Format("Index, %d, Enc, %d, Diff, %d, Time, %f", 
						  m_lBackLightElevatorMoveTestCount, m_lEnc_BLZ, m_lEnc_BLZ - m_lBackLightElevatorUpLevel, dTotalTime);

			SetErrorMessage(szTemp);

			if (abs(m_lEnc_BLZ - m_lBackLightElevatorUpLevel) >= 100)
			{
				HmiMessage_Red_Yellow("Backlight elevator missing steps");
				m_bBackLightElevatorMoveTest = FALSE;
				BLZ_PowerOn(FALSE);
				//BLZ_Home();
			}
		}
		else
		{
			DOUBLE dTotalTime;
			DOUBLE dStartTime = GetTime();
			BLZ_MoveTo(m_lBackLightElevatorStandByLevel);
			dTotalTime = GetTime() - dStartTime;
			Sleep(100);
			WPR_GetEncoderValue();
			
			CString szTemp;
			szTemp.Format("Index, %d, Enc, %d, Diff, %d, Time, %f", 
						  m_lBackLightElevatorMoveTestCount, m_lEnc_BLZ, m_lEnc_BLZ - m_lBackLightElevatorStandByLevel, dTotalTime);

			SetErrorMessage(szTemp);

			if (abs(m_lEnc_BLZ - m_lBackLightElevatorStandByLevel) >= 100)
			{
				HmiMessage_Red_Yellow("Backlight elevator missing steps");
				m_bBackLightElevatorMoveTest = FALSE;
				BLZ_PowerOn(FALSE);
				//BLZ_Home();
			}
		}
	}

	//reverse the direction
	m_bMoveDirection = !m_bMoveDirection;

	if (m_lMotionTestDelay > 0)
	{
		Sleep(m_lMotionTestDelay);
	}

	return TRUE;
}

BOOL CWaferPr::IM_InitPrHighLevelInfo(BOOL bWaferAligned)
{
	return TRUE;
}

PR_WORD CWaferPr::ManualSearchNormalDie(PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_WIN stWin)
{
	PR_REAL			fDieScore;
	PR_WORD			uwResult;

	LONG lDieNo =  WPR_NORMAL_DIE + 1;
	uwResult = ManualSearchDie(WPR_NORMAL_DIE, lDieNo, PR_TRUE, PR_TRUE, bInspect, usDieType, fDieRotate, stDieOffset, &fDieScore, stWin.coCorner1, stWin.coCorner2);
	return uwResult;
}


PR_WORD CWaferPr::ManualSearchScanDie(PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_WIN stWin)
{
	PR_REAL			fDieScore;
	PR_WORD			uwResult;

	LONG lDieNo = GetPrescanPrID();
	uwResult = ManualSearchDie(WPR_NORMAL_DIE, lDieNo, PR_TRUE, PR_TRUE, bInspect, usDieType, fDieRotate, stDieOffset, &fDieScore, stWin.coCorner1, stWin.coCorner2);
	return uwResult;
}

VOID CWaferPr::UpdateHmiCurNormalDieSize()
{
	PR_COORD stPixel;
	stPixel.x = GetNmlSizePixelX();
	stPixel.y = GetNmlSizePixelY();

	ConvertPixelToUnit(stPixel, &m_lCurNormDieSizeX, &m_lCurNormDieSizeY);
	
	ConvertPixelToDUnit(stPixel, m_dCurNormDieSizeX, m_dCurNormDieSizeY, m_bDisplayDieInUM);
}

VOID CWaferPr::UpdateHmiCurPitchSize()
{
	PR_COORD stPixel;
	stPixel.x = (PR_WORD)GetPitchPixelXX();
	stPixel.y = (PR_WORD)GetPitchPixelYY();
	
	ConvertPixelToUnit(stPixel, &m_lCurPitchSizeX, &m_lCurPitchSizeY);
	
	ConvertPixelToDUnit(stPixel, m_dCurPitchSizeX, m_dCurPitchSizeY, m_bDisplayDieInUM);
}

LONG CWaferPr::GetPrScaleFactor()
{
	return PR_SCALE_FACTOR;
}

BOOL CWaferPr::TurnOnOffPrLighting(BOOL bOn)
{
	PR_OPTIC stOptic;
	PR_COMMON_RPY stRpy;

	//v4.39T10	//Klocwork
	PR_SOURCE emCoaxID = PR_COAXIAL_LIGHT;
	PR_SOURCE emRingID = PR_RING_LIGHT;
	PR_SOURCE emSideID = PR_SIDE_LIGHT;
	PR_SOURCE emBackID = PR_BACK_LIGHT;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();

	PR_GetOptic(ubPpsG, ubSID, ubRID, &stOptic, &stRpy);
	if (bOn)
	{
		SetAlarmLog("WPR turn on lighting");
		GetLightingConfig(0, emCoaxID, emRingID, emSideID);
		PR_SetLighting(emCoaxID, (PR_UWORD)m_lWPRGenCoaxLightLevel[0], ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emRingID, (PR_UWORD)m_lWPRGenRingLightLevel[0], ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emSideID, (PR_UWORD)m_lWPRGenSideLightLevel[0], ubSID, ubRID, &stOptic, &stRpy);
		PR_SetLighting(emBackID, (PR_UWORD)m_lWPRGenBackLightLevel[0], ubSID, ubRID, &stOptic, &stRpy);
		if (CMS896AStn::m_bEnablePRDualColor == TRUE)
		{
			for (INT i = 1; i < WPR_MAX_LIGHT_GROUP; i++)
			{
				GetLightingConfig(i, emCoaxID, emRingID, emSideID);

				PR_SetLighting(emCoaxID, (PR_UWORD)m_lWPRGenCoaxLightLevel[i], ubSID, ubRID, &stOptic, &stRpy);
				PR_SetLighting(emRingID, (PR_UWORD)m_lWPRGenRingLightLevel[i], ubSID, ubRID, &stOptic, &stRpy);
				PR_SetLighting(emSideID, (PR_UWORD)m_lWPRGenSideLightLevel[i], ubSID, ubRID, &stOptic, &stRpy);
				PR_SetLighting(emBackID, (PR_UWORD)m_lWPRGenBackLightLevel[i], ubSID, ubRID, &stOptic, &stRpy);
			}
		}
	}
	else
	{
		SetAlarmLog("WPR turn off lighting");
		if (CMS896AStn::m_bEnablePRDualColor == TRUE)
		{
			for (INT i = 0; i < WPR_MAX_LIGHT_GROUP; i++)
			{
				GetLightingConfig(i, emCoaxID, emRingID, emSideID);

				PR_SetLighting(emCoaxID, (PR_UWORD)0, ubSID, ubRID, &stOptic, &stRpy);
				PR_SetLighting(emRingID, (PR_UWORD)0, ubSID, ubRID, &stOptic, &stRpy);
				PR_SetLighting(emSideID, (PR_UWORD)0, ubSID, ubRID, &stOptic, &stRpy);
				PR_SetLighting(emBackID, (PR_UWORD)0, ubSID, ubRID, &stOptic, &stRpy);
			}
		}
		else
		{
			GetLightingConfig(0, emCoaxID, emRingID, emSideID);
			PR_SetLighting(emCoaxID, (PR_UWORD)0, ubSID, ubRID, &stOptic, &stRpy);
			PR_SetLighting(emRingID, (PR_UWORD)0, ubSID, ubRID, &stOptic, &stRpy);
			PR_SetLighting(emSideID, (PR_UWORD)0, ubSID, ubRID, &stOptic, &stRpy);
			PR_SetLighting(emBackID, (PR_UWORD)0, ubSID, ubRID, &stOptic, &stRpy);
		}
	}

	return TRUE;
}

PR_WORD CWaferPr::GetDieSizePixelX(LONG lDieNo)
{
	UCHAR ucIndex = (UCHAR)lDieNo;
	ucIndex = min(ucIndex, WPR_MAX_DIE-1);
	return m_stGenDieSize[ucIndex].x;
}

PR_WORD CWaferPr::GetDieSizePixelY(LONG lDieNo)
{
	UCHAR ucIndex = (UCHAR)lDieNo;
	ucIndex = min(ucIndex, WPR_MAX_DIE-1);
	return m_stGenDieSize[ucIndex].y;
}

BOOL CWaferPr::DumpAppLog(PR_WORD wRecordID)
{
#ifdef	VS_5MCAM

	PR_DUMP_APP_LOG_CMD	stCmd;
	PR_DUMP_APP_LOG_RPY stRpy;

	CString szFilePath = _T("c:\\MapSorter\\UserData\\PrDumpLog");

	if (_access(szFilePath, 0) == -1)
	{
		if (CreateDirectory(szFilePath, NULL) == 0)
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				CString szErr;
				szErr.Format("WPR: DumpAppLog Create Folder failure - GetLastError = %d", GetLastError());
				SetErrorMessage(szErr);
				return FALSE;
			}
		}
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_InitDumpAppLogCmd(&stCmd);
	//stCmd.aubDesLogPath = "";				//Destination Log Path.
	strcpy_s((char*)stCmd.aubDesLogPath, sizeof(stCmd.aubDesLogPath), (LPCTSTR) szFilePath);

	stCmd.wIsSaveRecord = wRecordID;		// Save record along with the dump case.
	stCmd.uwStationID =	(PR_UWORD)ubRID;	

	PR_DumpAppLogCmd(&stCmd, ubSID, ubRID, &stRpy);

    if (stRpy.uwCommunStatus != PR_COMM_NOERR)
	{
		CString szErr;
		szErr.Format("WPR: DumpAppLog COM reply failure - 0x%x", stRpy.uwCommunStatus);
		SetErrorMessage(szErr);
		return FALSE;
    }
    if (stRpy.uwPRStatus != PR_ERR_NOERR)
	{
		CString szErr;
		szErr.Format("WPR: DumpAppLog PR reply failure - 0x%x", stRpy.uwPRStatus);
		SetErrorMessage(szErr);
		return FALSE;
    }
#endif
	return TRUE;
}


VOID CWaferPr::CalculateExtraExposureGrabTime()
{
	BOOL bReturn = TRUE;

	if (PR_NotInit())
	{
		return;
	}

	PR_EXPOSURE_TIME stTime = (PR_EXPOSURE_TIME) 0;
	LONG lExposureTimeLevel = 0;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();

	CString szErrMsg;
	if (!m_pPrGeneral->GetExposureTime(ubPpsI, ubSID, ubRID, stTime, szErrMsg))
	{
		CString szErr;
		szErr = _T("WPR PR ") + szErrMsg;
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
	}
	LONG lExtraPrGrabDelay = 0;
			
	if (stTime == PR_EXPOSURE_TIME_10)
		lExtraPrGrabDelay = 30 - 3 + 1;
	else if (stTime == PR_EXPOSURE_TIME_9)
		lExtraPrGrabDelay = 15 - 3 + 1;
	else if (stTime == PR_EXPOSURE_TIME_8)
		lExtraPrGrabDelay = 8 - 3 + 1;
	else if (stTime == PR_EXPOSURE_TIME_7)
		lExtraPrGrabDelay = 4 - 3 + 1;
	else
		lExtraPrGrabDelay = 0;

	if (lExtraPrGrabDelay > 0)	//v4.51A14
	{
		CString szLog;
		szLog.Format("WPR PR Exposure Time = %d; extra PR GRAB delay = %ld ms", (UINT) stTime, lExtraPrGrabDelay);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}

	(*m_psmfSRam)["WaferPr"]["ExtraGrabDelay"] = lExtraPrGrabDelay;
}


BOOL CWaferPr::LogLearnDieRecord(LONG lDieNo, INT nCamNo, INT nPurpose, 
								 UCHAR ucDieShape, USHORT usPRLrnStatus,
								 INT nSendID, INT nRecvID)				//v4.49A10
{
	if ((lDieNo < 0) || (lDieNo >= WPR_MAX_DIE))	//v4.49 Klocwork
	{
		return FALSE;
	}
	if (m_bGenDieLearnt[lDieNo] != TRUE)
	{
		return FALSE;
	}

	CString szFilePath = _T("c:\\MapSorter\\UserData\\PrDumpLog");
	if (_access(szFilePath, 0) == -1)
	{
		if (CreateDirectory(szFilePath, NULL) == 0)
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				CString szErr;
				szErr.Format("WPR: DumpAppLog Create Folder failure 1 - GetLastError = %d", GetLastError());
				SetErrorMessage(szErr);
				return FALSE;
			}
		}
	}

	CTime curTime = CTime::GetCurrentTime();
	CString szDateTime  = curTime.Format("%y-%m-%d - %H:%M:%S");
	CString szDateTime2 = curTime.Format("%y%m%d_%H%M%S");

	CString szFileName;
	if ( CMSLogFileUtility::Instance()->GetEnableMachineLog() == TRUE)
	{
		szFileName.Format("WPR_LEARN_DIE_%ld_", lDieNo);
		szFileName = szFilePath + "\\" + szFileName + szDateTime2 + ".csv";
	}
	else
	{
		szFileName.Format("WPR_LEARN_DIE_%ld.csv", lDieNo);
		szFileName = szFilePath + "\\" + szFileName;
		DeleteFile(szFileName);
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	pApp->SearchAndRemoveFiles(szFilePath, 7);


	CStdioFile oLogFile;
	if (oLogFile.Open(szFileName, CFile::modeCreate|CFile::modeReadWrite|CFile::typeText))
	{
		CString szLine;

		szLine.Format("Date,%s\n\n", szDateTime);
		oLogFile.WriteString(szLine);	


		if (lDieNo >= 3)
			szLine.Format("Die No (Ref),%ld,(Ref #%ld)\n", lDieNo, lDieNo-2);
		else
			szLine.Format("Die No,%ld\n",	lDieNo);
		oLogFile.WriteString(szLine);	
		szLine.Format("Record ID,%d\n",		m_ssGenPRSrchID[lDieNo]);
		oLogFile.WriteString(szLine);	
		szLine.Format("Camera ID,%d\n",		nCamNo);
		oLogFile.WriteString(szLine);	
		szLine.Format("Purpose,%d,%s\n",	nPurpose, GetPurposeInString(nPurpose));
		oLogFile.WriteString(szLine);	
		szLine.Format("SEND ID,0x%x\n",		nSendID);
		oLogFile.WriteString(szLine);	
		szLine.Format("RECV ID,0x%x\n",		nRecvID);
		oLogFile.WriteString(szLine);	


		szLine.Format("\nDie Shape,%d\n",	ucDieShape);
		oLogFile.WriteString(szLine);	

		switch (m_lGenLrnFineSrch[lDieNo])
		{
		case 0:
			szLine = "ALIGN Resolution,LOW\n";
			break;
		case 2:
			szLine = "ALIGN Resolution,HIGH\n";
			break;
		case 1:
		default:
			szLine = "ALIGN Resolution,MEDIUM\n";
			break;
		}
		oLogFile.WriteString(szLine);	

		szLine.Format("ALIGN Algorithm,%d,%s\n",	GetLrnAlignAlgo(lDieNo), GetLrnAlignAlgoInString(lDieNo));
		oLogFile.WriteString(szLine);	
		szLine.Format("INSP Algorithm,%d,%s\n",		GetLrnInspMethod(lDieNo), GetLrnAlignAlgoInString(lDieNo));
		oLogFile.WriteString(szLine);	


		szLine.Format("\nPR LrnDie Status,0x%x\n",	usPRLrnStatus);
		oLogFile.WriteString(szLine);	

		oLogFile.Close();
	}

	return TRUE;
}

//v4.49A10
BOOL CWaferPr::LogUserSearchDieResult(LONG lDieNo, USHORT usPrResult, 
									  INT nX, INT nY, FLOAT fDieRotation, FLOAT fDieScore,
									  PR_COORD stCorner1, PR_COORD stCorner2,
									  INT nSendID, INT nRecvID)			
{
	if ((lDieNo < 0) || (lDieNo >= WPR_MAX_DIE))	//v4.49 Klocwork
	{
		return FALSE;
	}
	if (m_bGenDieLearnt[lDieNo] != TRUE)
	{
		return FALSE;
	}


	CString szFilePath = _T("c:\\MapSorter\\UserData\\PrDumpLog");
	if (_access(szFilePath, 0) == -1)
	{
		if (CreateDirectory(szFilePath, NULL) == 0)
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				CString szErr;
				szErr.Format("WPR: DumpAppLog Create Folder failure 1 - GetLastError = %d", GetLastError());
				SetErrorMessage(szErr);
				return FALSE;
			}
		}
	}

	CTime curTime = CTime::GetCurrentTime();
	CString szDateTime  = curTime.Format("%y-%m-%d - %H:%M:%S");
	CString szDateTime2 = curTime.Format("%y%m%d_%H%M%S");

	CString szFileName;
	if ( CMSLogFileUtility::Instance()->GetEnableMachineLog() == TRUE)
	{
		szFileName.Format("WPR_SRCH_DIE_%ld_", lDieNo);
		szFileName = szFilePath + "\\" + szFileName + szDateTime2 + ".csv";
	}
	else
	{
		szFileName.Format("WPR_SRCH_DIE_%ld.csv", lDieNo);
		szFileName = szFilePath + "\\" + szFileName;
		DeleteFile(szFileName);
	}


	CStdioFile oLogFile;
	if (oLogFile.Open(szFileName, CFile::modeCreate|CFile::modeReadWrite|CFile::typeText))
	{
		CString szLine;
		szLine.Format("Date,%s\n\n", szDateTime);
		oLogFile.WriteString(szLine);	


		if (lDieNo >= 3)
			szLine.Format("Die No (Ref),%ld,(Ref #%ld)\n", lDieNo, lDieNo-2);
		else
			szLine.Format("Die No,%ld\n", lDieNo);
		oLogFile.WriteString(szLine);	
		szLine.Format("Record ID,%d\n",			m_ssGenPRSrchID[lDieNo]);
		oLogFile.WriteString(szLine);	
		szLine.Format("SEND ID,0x%x\n",			nSendID);
		oLogFile.WriteString(szLine);	
		szLine.Format("RECV ID,0x%x\n",			nRecvID);
		oLogFile.WriteString(szLine);	


		szLine.Format("\nALIGN Algorithm,%d,%s\n",	GetLrnAlignAlgo(lDieNo), GetLrnAlignAlgoInString(lDieNo));
		oLogFile.WriteString(szLine);	
		szLine.Format("INSP Algorithm,%d,%s\n",	GetLrnInspMethod(lDieNo), GetLrnInspMethodInString(lDieNo));
		oLogFile.WriteString(szLine);	
		szLine.Format("DEFECT Threshold,%d\n",	m_lGenSrchDefectThres[lDieNo]);
		oLogFile.WriteString(szLine);	
		szLine.Format("Srch Wnd,%d,%d,%d,%d\n",	stCorner1.x, stCorner1.y, stCorner2.x, stCorner2.y);
		oLogFile.WriteString(szLine);	


		szLine.Format("\nPR Result,0x%x\n",		usPrResult);
		oLogFile.WriteString(szLine);	
		szLine.Format("X,%d\n",					nX);
		oLogFile.WriteString(szLine);	
		szLine.Format("Y,%d\n",					nY);
		oLogFile.WriteString(szLine);	
		szLine.Format("Rotate,%.2f\n",			fDieRotation);
		oLogFile.WriteString(szLine);	
		szLine.Format("Die Score,%.2f\n",		fDieScore);
		oLogFile.WriteString(szLine);	


		oLogFile.Close();
	}

	return TRUE;
}

CString CWaferPr::GetPurposeInString(INT nPurpose)		//v4.49A10
{
	CString szPurpose = "none";
	switch (nPurpose)
	{
	case MS899_WAF_GEN_PURPOSE:					
		szPurpose = "PR_PURPOSE_CAMERA_1";
		break;
	case MS_DP_WAF_5M_GN_PURPOSE:					
		szPurpose = "PR_PURPOSE_5M_GEN_1";
		break;
	/*case MS899_BOND_GEN_PURPOSE:				
		szPurpose = "PR_PURPOSE_CAMERA_2";
		break;*/
	case MS899_WAF_INSP_PURPOSE:				
		szPurpose = "PR_WAF_INSP";
		break;
	case MS_DP_WAF_5M_PR_PURPOSE:
		szPurpose = "PR_WAF5M_INSP";
		break;
	/*case MS899_BOND_PB_PURPOSE:				
		szPurpose = "PR_POST_DIE_INSP";
		break;*/
	case MS899_POSTSEAL_GEN_PURPOSE:				
		szPurpose = "PR_PURPOSE_CAMERA_3";
		break;
	case MS899_POSTSEAL_PR_PURPOSE:				
		szPurpose = "PR_PURPOSE_2D_CODE_1";
		break;
	case MS899_POSTSEAL_BH1_GEN_PURPOSE:				
		szPurpose = "PR_PURPOSE_CAM_A3";
		break;
	case MS899_POSTSEAL_BH1_PR_PURPOSE:				
		szPurpose = "PR_PURPOSE_2D_CODE_2";
		break;
	case MS899_POSTSEAL_BH2_GEN_PURPOSE:				
		szPurpose = "PR_PURPOSE_CAM_A4";
		break;
	case MS899_POSTSEAL_BH2_PR_PURPOSE:				
		szPurpose = "PR_PURPOSE_2D_CODE_3";
		break;
	}

	return szPurpose;
}

UCHAR	CWaferPr::GetDieShape()
{
	return m_ucGenDieShape;
}

UCHAR CWaferPr::CheckDieShape(UCHAR &ucDieShape)
{
	//Force die shape to rectangle if die shape option is disable
	if (CMS896AStn::m_bEnablePolyonDie == FALSE)
	{
		ucDieShape = WPR_RECTANGLE_DIE;
	}
	if( ucDieShape==WPR_RHOMBUS_DIE && m_bEnableRhombusDie==FALSE )
	{
		ucDieShape = WPR_RECTANGLE_DIE;
	}

	return ucDieShape;
}

BOOL CWaferPr::BH_TnZToPick1()
{
	IPC_CServiceMessage stMsg;
	BOOL bGet;
	int nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_TnZToPick1", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bGet);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	return TRUE;
}

BOOL CWaferPr::BH_TnZToPick2()
{
	IPC_CServiceMessage stMsg;
	BOOL bGet;
	int nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_TnZToPick2", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bGet);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	return TRUE;
}

BOOL CWaferPr::BH_CancelToPrePick1()
{
	IPC_CServiceMessage stMsg;
	BOOL bGet;
	int nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_CancelBondHeadSetup", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bGet);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	return TRUE;
}

BOOL CWaferPr::BH_CancelToPrePick2()
{
	IPC_CServiceMessage stMsg;
	BOOL bGet;
	int nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_CancelBondHeadSetupZ2", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bGet);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	return TRUE;
}


BOOL CWaferPr::IsEnableBHMark()
{
	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));

	if (pBondHead != NULL)
	{
		return pBondHead->IsEnableBHMark();
	}

	return FALSE;
}

LONG CWaferPr::SetNGGradeScore(LONG lSrchNGDieScore)
{
	m_lGenSrchDieScore[BPR_GEN_NG_GRADE_INDEX_WITH_OFFSET] = lSrchNGDieScore;
	return TRUE;
}

BOOL CWaferPr::IsNGBlock(const LONG lRow, const LONG lCol)
{
	if (this == NULL)
	{
		return FALSE;
	}
	UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lRow, lCol) - m_WaferMapWrapper.GetGradeOffset();
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		return pBondPr->IsNGBlock(ucGrade);
	}

	return FALSE;
}

BOOL CWaferPr::UpdateWPRInfoList() //Matthew 20190128
{
	CStdioFile fTemp;
	CString szText, szRight, szLeft;
	LONG lLeftCount = 0;

	CString szPath = _T("c:\\mapsorter\\exe\\WPR_Info.csv");
	if (!fTemp.Open(szPath, CFile::modeCreate| CFile::modeReadWrite))
	{
		return FALSE;
	}

	for(int i = 0; i < 13; i++)
	{
		//PR Record Type
		if (i < 3)
		{
			szText = "Normal,";
		}
		else
		{
			szText = "Reference,";
		}
		fTemp.WriteString(szText);

		//PRID
		if (i < 3)
		{
			szText.Format("%d,", i + 1);
		}
		else
		{
			szText.Format("%d,", i - 2);
		}
		fTemp.WriteString(szText);

		//PR Record Description
		if (i == 0)	
		{
			szText = "Normal #1";
		}
		else if (i == 1)
		{
			szText = "Second PR";
		}
		else if (i == 2)
		{
			szText = "FF Mode (Prescan)";
		}
		else if (i == 3) //Reference 1
		{
			szText = "Wafer Map Reference #1";
		}
		else if (i == 4) 
		{
			szText = "Wafer Map Reference #2";
		}
		else if (i == 5)
		{
			szText = "no use";
		}
		else if (i == 6)
		{
			szText = "Collet Hole (Expoxy)";
		}
		else if (i == 7)
		{
			szText = "BH1 Mark";
		}
		else if (i == 8)
		{
			szText = "BH2 Mark";
		}
		else if (i == 9)
		{
			szText = "BH1 Collet Hole With Circle";
		}
		else if (i == 10)
		{
			szText = "BH2 Collet Hole With Circle";
		}
		else if (i == 11)
		{
			szText = "NG grade PR";
		}
		else if (i == 12)
		{
			szText = "Ej cap hole";
		}
		szText = szText + ",";
		fTemp.WriteString(szText);

		//Algorithm Type
		switch (m_lGenLrnAlignAlgo[i])
		{
		case 0:
			szText = "PGE";
			break;
		case 1:
			szText = "PM";
			break;
		case 2:
			szText = "EM";
			break;
		case 3:
			szText = "P+E";
			break;
		case 4:
			szText = "ESM";
			break;
		case 5:
			szText = "CGLF";
			break;
		}
		szText = szText + ",";
		fTemp.WriteString(szText);

		//Learn Status
		if(m_bGenDieLearnt[i] == TRUE)
		{
			szText = "OK,";
		}
		else
		{
			szText = "No,";
		}
		fTemp.WriteString(szText);
		
		//Zoom Mode
		szText = m_szGenDieZoomMode[i];
		szText = szText + ",";
		fTemp.WriteString(szText);

		//Calibration Status
		if (m_bGenDieCalibration[i])
		{
			szText = "Yes,";
		}
		else
		{
			szText = "No,";
		}
		fTemp.WriteString(szText);

		//Learn PR Record ID
		LONG lPrID = m_ssGenPRSrchID[i];
		if (lPrID != 0)
		{
			szText.Format("%d,", lPrID);
		}
		else
		{
			szText = "N/A,";
		}
		fTemp.WriteString(szText);
	}

	fTemp.Close();
	return TRUE;
}

BOOL CWaferPr::SetbGenDieCalibration(BOOL bCalibration) //Matthew 20190128
{
	if (bCalibration == FALSE)
	{
		if (m_bSelectDieType == WPR_REFERENCE_DIE)
		{
			m_bGenDieCalibration[m_lCurRefDieNo + WPR_GEN_RDIE_OFFSET] = FALSE;
		}
		else
		{
			m_bGenDieCalibration[m_lCurRefDieNo + WPR_GEN_NDIE_OFFSET] = FALSE;
		}
	}
	else
	{
		if (m_bSelectDieType == WPR_REFERENCE_DIE)
		{
			m_bGenDieCalibration[m_lCurRefDieNo + WPR_GEN_RDIE_OFFSET] = TRUE;
		}
		else
		{
			m_bGenDieCalibration[m_lCurRefDieNo + WPR_GEN_NDIE_OFFSET] = TRUE;
		}
	}
	
	return TRUE;
}

BOOL CWaferPr::SetszGenDieZoomMode(BOOL bEmpty)	//Matthew 20190128
{
	if (bEmpty == FALSE)
	{	
		if (m_bSelectDieType == WPR_REFERENCE_DIE)
		{
			m_szGenDieZoomMode[m_lCurRefDieNo + WPR_GEN_RDIE_OFFSET] = m_stZoomView.m_szZoomFactor;
		}
		else
		{
			m_szGenDieZoomMode[m_lCurRefDieNo + WPR_GEN_NDIE_OFFSET] = m_stZoomView.m_szZoomFactor;
		}
	}
	else
	{
		if (m_bSelectDieType == WPR_REFERENCE_DIE)
		{
			m_szGenDieZoomMode[m_lCurRefDieNo + WPR_GEN_RDIE_OFFSET] = "";
		}
		else
		{
			m_szGenDieZoomMode[m_lCurRefDieNo + WPR_GEN_NDIE_OFFSET] = "";
		}
	}

	return TRUE;
}


BOOL CWaferPr::LoadPackageMsdWPRData(VOID)
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

	//	WPR Presecan Setting
	m_lExtraPrescanOverlapX = (*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_OVERLAP_X];
	m_lExtraPrescanOverlapY	= (*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_OVERLAP_Y];
	m_dPrescanPitchTolX		= (DOUBLE)((LONG)(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_PITCH_X]);
	m_dPrescanPitchTolY		= (DOUBLE)((LONG)(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_PITCH_Y]);
	m_dAcceptDieAngle		= (DOUBLE)((LONG)(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING]["Theta Accept Angle"]);
	m_dScanAcceptAngle		= (DOUBLE)((LONG)(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_SCAN_ACCEPT_ANGLE_1SEARCH]);
	
	//	WPR Alarm Setting
	m_ulMaxNoDieSkipCount	= (*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_WAFER_NO_DIE_LIMIT];

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to load WPR data from PackageData.msd");

	// close config file
    pUtl->ClosePackageDataConfig();

	return TRUE;
}

BOOL CWaferPr::SavePackageMsdWPRData(VOID)
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

	//	WPR Presecan Setting
	(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_OVERLAP_X]		 = m_lExtraPrescanOverlapX;
	(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_OVERLAP_Y]		 = m_lExtraPrescanOverlapY;
	(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_PITCH_X]			 = m_dPrescanPitchTolX;
	(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_PITCH_Y]			 = m_dPrescanPitchTolY;
	(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING]["Theta Accept Angle"] = m_dAcceptDieAngle;	
	(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_SCAN_ACCEPT_ANGLE_1SEARCH]	 = m_dScanAcceptAngle;
	
	//	WPR Alarm Setting
	(*psmf)[WPR_DATA][WPR_PRESCAN_SETTING][WPR_WAFER_NO_DIE_LIMIT] = m_ulMaxNoDieSkipCount;

	pUtl->UpdatePackageDataConfig();	
	// close config file
    pUtl->ClosePackageDataConfig();

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to save WPR data from PackageData.msd");

	return TRUE;
}


BOOL CWaferPr::UpdateWPRPackageList(VOID)
{
	CStdioFile fTemp;
	CString szContent, szText, szTemp;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\WPRPackageList.csv"), CFile::modeCreate| CFile::modeReadWrite))
	{
		return FALSE;
	}

	//	WPR Presecan Setting
	szText = WPR_OVERLAP_X;
	szText = szText + ",";
	szTemp.Format("%d,", m_lExtraPrescanOverlapX);
	szContent = szContent + szText + szTemp;

	szText = WPR_OVERLAP_Y;
	szText = szText + ",";
	szTemp.Format("%d,", m_lExtraPrescanOverlapY);
	szContent = szContent + szText + szTemp;

	szText = WPR_PITCH_X;
	szText = szText + ",";
	szTemp.Format("%4.2f,", m_dPrescanPitchTolX);
	szContent = szContent + szText + szTemp;

	szText = WPR_PITCH_Y;
	szText = szText + ",";
	szTemp.Format("%4.2f,", m_dPrescanPitchTolY);
	szContent = szContent + szText + szTemp;

	szText = "Theta Accept Angle";
	szText = szText + ",";
	szTemp.Format("%4.2f,", m_dAcceptDieAngle);
	szContent = szContent + szText + szTemp;

	szText = WPR_SCAN_ACCEPT_ANGLE_1SEARCH;
	szText = szText + ",";
	szTemp.Format("%4.2f,", m_dScanAcceptAngle);
	szContent = szContent + szText + szTemp;
	
	//	WPR Alarm Setting
	szText = WPR_WAFER_NO_DIE_LIMIT;
	szText = szText + ",";
	szTemp.Format("%d,", m_ulMaxNoDieSkipCount);
	szContent = szContent + szText + szTemp;

	fTemp.WriteString(szContent);
	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to update WPR package data to WTPackageList.csv");

	fTemp.Close();

	return TRUE;
}


BOOL CWaferPr::WPRIndexTest()
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	BOOL		bStatus = TRUE;

	if (PR_NotInit())
	{
		bStatus = FALSE;
	}
	else
	{
		PR_WORD wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
		if (wResult != -1)
		{
			if (DieIsAlignable(usDieType) == TRUE)
			{
				int nOffsetX = 0, nOffsetY = 0;
				CString szLog;
				CalculateDieCompenate(stDieOffset, &nOffsetX, &nOffsetY);
				CalculateNDieOffsetXY(nOffsetX, nOffsetY);

				int	siEncX = 0, siEncY = 0, siEncT = 0; 
				GetWaferTableEncoder(&siEncX, &siEncY, &siEncT);

				int	siCmdX = 0, siCmdY = 0, siCmdT = 0; 
				GetWaferTableCommander(&siCmdX, &siCmdY, &siCmdT);

				szLog.Format("WT Index Test,Pixel Offset,%d,%d,ENC Offset,%d,%d,Rotate,%f,WT ENC,%d,%d,WT CMD,%d,%d,WT CMD in LF,%d,%d", 
					(int)stDieOffset.x, (int)stDieOffset.y, nOffsetX, nOffsetY, (DOUBLE)fDieRotate, siEncX, siEncY, siCmdX, siCmdY, m_stWTIndexTestLFEnc.lX, m_stWTIndexTestLFEnc.lY);

				CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szLog);

				if (nOffsetX > 20 || nOffsetY > 20)
				{
					WaferPrLogFailureCaseImage();
				}
			}
			else
			{
				bStatus = FALSE;
			}
		}
		else
		{
			bStatus = FALSE;
		}
	}

	return bStatus;
}


BOOL CWaferPr::WprSerachBinEjectorPin(LONG lEjTEnc, LONG lEjTCmd)
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	BOOL		bStatus = TRUE;
	CString		szLog;


	if (PR_NotInit())
	{
		bStatus = FALSE;
	}
	else
	{
		PR_WORD wResult = ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_PR_DIE_INDEX5, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
		if (wResult != -1)
		{
			if (DieIsAlignable(usDieType) == TRUE)
			{
				int nOffsetX = 0, nOffsetY = 0;
				DOUBLE dXum = 0, dYum = 0;
				//CString szLog;

				CalculateDieCompenate(stDieOffset, &nOffsetX, &nOffsetY);
				CalculateNDieOffsetXY(nOffsetX, nOffsetY);
				ConvertPixelToUM(stDieOffset, dXum, dYum);

				szLog.Format("Wpr Search Bin Ejector Pin,Pixel Offset,%d,%d,ENC Offset,%d,%d,um offset,%d,%d,Rotate,%f,EjT ENC,%d,EjT CMD,%d", 
					(int)stDieOffset.x, (int)stDieOffset.y, nOffsetX, nOffsetY, dXum, dYum, (DOUBLE)fDieRotate, lEjTEnc, lEjTCmd);

				CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szLog, TRUE);
			}
			else
			{
				bStatus = FALSE;

				szLog.Format("Wpr Search Bin Ejector Pin,ErrorDieType,%d", (UINT) usDieType);
				CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szLog, TRUE);
			}
		}
		else
		{
			bStatus = FALSE;

			szLog.Format("Wpr Search Bin Ejector Pin,SearchDieError,%d", (INT) wResult);
			CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szLog, TRUE);
		}
	}

	return bStatus;
}


BOOL CWaferPr::WaferPrLogFailureCaseImage()
{
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_UWORD uwRecordID		= (LONG)(*m_psmfSRam)["WaferPr"]["Srch Die Record ID"];
	PR_ULWORD ulImageID		= (*m_psmfSRam)["WaferPr"]["Latch Image ID"];
	PR_ULWORD ulStationID	= (*m_psmfSRam)["WaferPr"]["Latch Station ID"];
	
	CString szFileName, szTime;
	SYSTEMTIME sysTm;
	GetLocalTime(&sysTm);
	szTime.Format("%04d%02d%02d_%02d%02d%02d%03d",
			sysTm.wYear, sysTm.wMonth, sysTm.wDay, sysTm.wHour, sysTm.wMinute, sysTm.wSecond, sysTm.wMilliseconds);
	szFileName = "WaferPrImage_" + szTime;

	CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
	if (pPRFailureCaseLog->LogFailureCaseImage(uwRecordID,  ubSID,  ubRID, szFileName) != 0)
	{
		return FALSE;
	}

	return TRUE;
}