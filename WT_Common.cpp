/////////////////////////////////////////////////////////////////
// WT_Common.cpp : Common functions of the CWaferTable class
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
#include "WaferTable.h"
#include "math.h"
#include "FileUtil.h"
#include "MS896A.h"
#include "PrescanUtility.h"
#include "StrInputDlg.h"
#include "WL_Constant.h"
#include "WaferPr.h"
#include "PrescanInfo.h"
#include "PRFailureCaseLog.h"
//#ifndef MS_DEBUG
//	#include "mxml.h"	
//#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CWaferTable::InitVariable(VOID)
{
	int i;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	m_ulRescanAtBondDieCount = 0;
	m_bForceRescan			 = FALSE;
	m_szGradeDisplayMode	 = "Original";
	m_lGradeDisplayMode		 = 3;
	

	m_bUseDualTablesOption		= FALSE;
	m_bUseES101v11TableConfig	= FALSE;	//ES101 new WT config with only INDEX sensor for both XY channels
	m_nWTInUse					= 0;		//0=WT1 (1=WT2 for ES101)

	m_bHomeSnr_X				= FALSE;
	m_bHomeSnr_Y				= FALSE;
	m_bHomeSnr_T				= FALSE;
	m_bLimitSnr_X				= FALSE;
	m_bLimitSnr_Y				= FALSE;

	m_bHomeSnr_X2				= FALSE;
	m_bHomeSnr_Y2				= FALSE;
	m_bHomeSnr_T2				= FALSE;
	m_bLimitSnr_X2				= FALSE;
	m_bLimitSnr_Y2				= FALSE;

	m_lGetNextDieRow			= -1;
	m_lGetNextDieCol			= -1;

	m_bSel_X					= TRUE;
	m_bSel_Y					= TRUE;
	m_bSel_T					= TRUE;

	m_bSel_X2					= FALSE;
	m_bSel_Y2					= FALSE;
	m_bSel_T2					= FALSE;

	m_lEnc_X					= 0;
	m_lEnc_Y					= 0;
	m_lCmd_X					= 0;
	m_lCmd_Y					= 0;
	m_lEnc_T					= 0;
	m_lEnc_X1					= 0;
	m_lEnc_Y1					= 0;
	m_lCmd_Y1					= 0;
	m_lEnc_T1					= 0;
	m_lEnc_X2					= 0;
	m_lEnc_Y2					= 0;
	m_lCmd_Y2					= 0;
	m_lEnc_T2					= 0;

	m_lMotorDirectionX			= 0;
	m_lMotorDirectionY			= 0;

	//v4.24T9	//ES101
	m_lXNegLimit				= 0;
	m_lXPosLimit				= 0;
	m_lYNegLimit				= 0;
	m_lYPosLimit				= 0;
	m_lX2NegLimit				= 0;
	m_lX2PosLimit				= 0;
	m_lY2NegLimit				= 0;
	m_lY2PosLimit				= 0;
	m_bEJTLLimit				= FALSE;
	m_bEJTULimit				= FALSE;

	m_lX_ProfileType			= 0;
	m_lY_ProfileType			= 0;

	m_bUpdateOutput				= FALSE;
	m_bHome_X					= FALSE;
	m_bHome_Y					= FALSE;
	m_bHome_T					= FALSE;
	m_bComm_X					= FALSE;
	m_bComm_Y					= FALSE;
	m_bComm_T					= FALSE;

	m_bDieIsGet					= FALSE;
	m_bLoadMap					= FALSE;		// Wafer map not loaded

	m_lMinDelay					= 10; 
	m_lReadyDelay				= m_lMinDelay;

	m_lStart_X					= 0;
	m_lStart_Y					= 0;
	m_lStart_T					= 0;
	SetAlignmentStatus(FALSE);
	m_bIsUnload					= FALSE;
	m_bEnableBarcodeLoadMap		= FALSE;
	m_bBarCodeSearchMap			= FALSE;
	m_bBarCodeSearchFolder		= FALSE;
	m_bBarCodeSearchPartMap		= FALSE;
	m_bLoadMapAccessFail		= FALSE;
	m_szAccessFailFileName		= "";
	m_szSearchFolderName		= "";
	m_bEnableSuffix				= FALSE;
	m_szSuffixName				= "";
	m_szLastLotNumber			= "";
	m_bEnableLocalLoadMap		= FALSE;
	m_bUserModeLocalLoadMap		= FALSE;
	m_bEnableAutoLoadBackupMap	= FALSE;
	m_bMultiGradeSortingtoSingleBin = FALSE;

	m_bIsIgnoreGradeCountCheckOk = FALSE;

	m_szLastPkgName.Empty();

	m_bEnableCheckLotNo			= FALSE;

	m_bMapGradeMappingOK		= TRUE;

	m_bIsPowerOn_X				= TRUE;
	m_bIsPowerOn_Y				= TRUE;
	m_bIsPowerOn_T				= TRUE;

	m_lWaferUnloadX				= 0;
	m_lWaferUnloadY				= 0;
	m_lWaferCenterX				= 0;			
	m_lWaferCenterY				= 0;
	m_lWaferCalibX				= 0;			
	m_lWaferCalibY				= 0;
	m_lWaferCalibX2				= 0;
	m_lWaferCalibY2				= 0;
	m_lWaferBeforeCalibT		= 0;
	m_lWaferBeforeCalibT2		= 0;
	m_lWaferType				= 0;
	m_lWaferSize				= 0;
	m_dWaferDiameter			= 0.0;
	m_lWaferWorkingAngle		= 0;
	m_lWaferCurrentWorkingAngle	= 0;
	m_lWaferIndexPath			= 0;
	m_lWaferEmptyLimit			= 1;
	m_lWaferEdgeLimit			= 1;
	m_lWaferStreetLimit			= 1;
	m_lWaferTmpX[0]				= 0;		
	m_lWaferTmpY[0]				= 0;		
	m_lWaferTmpX[1]				= 0;		
	m_lWaferTmpY[1]				= 0;		
	m_lWaferTmpX[2]				= 0;		
	m_lWaferTmpY[2]				= 0;
	m_bStartCalibrate			= FALSE;
	m_bStartCalibrate2			= FALSE;

	m_bUseMultiCorOffset		= FALSE;
	m_dCorOffsetA1				= 0.00;
	m_dCorOffsetA2				= 0.00;
	m_dCorOffsetB1				= 0.00;
	m_dCorOffsetB2				= 0.00;
	m_dACorOffsetA1				= 0.00;
	m_dACorOffsetA2				= 0.00;
	m_dACorOffsetB1				= 0.00;
	m_dACorOffsetB2				= 0.00;

	m_ulCurrentRow				= 0;
	m_ulCurrentCol				= 0;
	m_ulAlignRow				= 0;
	m_ulAlignCol				= 0;
	m_lAlignPosX				= 0;
	m_lAlignPosY				= 0;
	m_bAutoAlignWafer			= FALSE;
	m_lSCNIndex_X				= 10;
	m_lSCNIndex_Y				= 10;
	m_lLookForwardCounter		= 0;
	m_lNoLookForwardCounter		= 0;
	m_bNewGTPosition			= FALSE;
	m_lNewGTPositionX			= 0;
	m_lNewGTPositionY			= 0;
	m_lNewGTPositionX2			= 0;
	m_lNewGTPositionY2			= 0;
	m_bNewGTUse2                  = 0;

	CString szTemp = pApp->GetProfileString(gszPROFILE_SETTING, _T("MS Aln Table Theta"), "0.0");	// get when start up.
	m_dAlnTableTheta		= atof(szTemp)/180.0;
	m_dScnTableTheta		= 0;
	m_ulScnDieA_Row			= 0;
	m_ulScnDieA_Col			= 0;
	m_ulScnDieB_Row			= 0;
	m_ulScnDieB_Col			= 0;
	m_dScnDieA_X			= 0;
	m_dScnDieA_Y			= 0;
	m_dWS896Angle_X			= 0.0;
	m_dWS896Angle_Y			= 0.0;
	m_lMsDieA_X				= 0;
	m_lMsDieA_Y				= 0;
	m_lMsDieB_X				= 0;
	m_lMsDieB_Y				= 0;
	m_dMS896Angle_X			= 0.0;
	m_dMS896Angle_Y			= 0.0;

	m_lGlobalTheta				= 0;
	m_dMoveBackAngle			= 0.00;		//v2.82

	m_bEnablePickAndPlace		= FALSE;
	m_lPickAndPlaceWalkPath		= 0;
	m_bNewEjectorSequence		= TRUE;		//v2.61T9

	m_lBurningInGrade			= 1;
	m_lBurnInMaxGrade			= 100;
	m_bBurnInAutoMap			= FALSE;
	m_bBurnInAutoGrade			= FALSE;
	m_szBurnInMapFile			= gszROOT_DIRECTORY + "\\Exe\\BurnIn(24Bin).txt";

	m_ucMapRotation				= 0;
	m_ucAutoWaferRotation		= 0;
	m_ucAutoBCRotation			= 0;
	m_bMapHoriFlip				= FALSE;
	m_bMapVertFlip				= FALSE;	
	m_bShowFirstDie				= FALSE;
	m_ucMapEdgeSize				= 0;
	m_ucMapPickMode				= 0;	//In-Grade		//v2.78T1
	m_ulMapDieStep				= 1;					//v2.78T1
	m_szMapTour					= "TLH";
	m_szMapPath					= "LocalMinDistance";
	m_bMultiWaferSupport		= FALSE;				//v2.91T1
	m_bEnableSmartWalk			= FALSE;				//v2.99T1
	m_bEnableSWalk1stDieFinder	= FALSE;				//v3.15T1
	m_ucSpiralPickDirection		= 0;	//=horiz		//v2.95T1
	m_lSpecialRefDieGrade		= 0;					//v3.24T1	//CMLT


	//For output file generating
	m_lNoOfWaferMapCols			= 1;
	m_szMapHeaderFileName		= "";

	for (i=0; i<WT_MAX_MAP_COL; i++)
	{
		m_szWaferMapCol[i] = "";
		m_bIfEnableWaferMapColCtrl[i] = FALSE;
		m_lWaferMapColCtrlIndex[i] = i;
		
	}

	for (i=0; i<WT_MAX_MAP_COL_ALL; i++)
	{
		m_szAllWaferMapCol[i] = "";
	}

	m_bEnableHeaderBtn			= TRUE;
	m_bEnableOutputBtn			= TRUE;
	m_lNoOfOutFileCols			= 1;
	m_lCurOutputFilePageNo		= 1;
	m_lPrevOutputFilePageNo		= 1;
	m_szOutputFormatFileName	= "";
	m_lCurDefineHeaderPageNo = 1;
	m_lPrevDefineHeaderPageNo = 1;

	for (i=0; i<WT_MAX_OUTPUT_COL; i++)
	{
		m_szOutFileCol[i] = "";
		m_bOutFileColMin[i]	= FALSE;
		m_bOutFileColMax[i] = FALSE;
		m_bOutFileColAvg[i] = FALSE;
		m_bOutFileColStd[i] = FALSE;
		m_bIfEnableOutColCtrl[i] = FALSE;
		m_lOutColCtrlIndex[i] = i;
	}

	for (i=0; i<WT_MAX_OUTPUT_COL_ALL; i++)
	{
		m_szAllOutFileCol[i] = "None";
		m_bAllOutFileColMin[i]	= FALSE;
		m_bAllOutFileColMax[i] = FALSE;
		m_bAllOutFileColAvg[i] = FALSE;
		m_bAllOutFileColStd[i] = FALSE;
	}

	m_szMapFilePath				= "";
	m_szMapFileExtension		= "";
	m_szBCMapFilename			= "";
	m_szMapDisplayFilename		= "";

	m_szBackupMapPath			= gszROOT_DIRECTORY + _T("\\OutputFiles\\BackupMap");
	m_szBackupMapExtension		= "rpt";

	m_lTDelay					= 15;		// Theta delay: 15 ms per degree
	m_lRotateCount				= (LONG)(8 / m_dThetaRes);
	m_lJoystickLevel			= 1;		// Medium speed
	m_bJoystickOn				= TRUE;		// Joystick ON
	m_bXJoystickOn				= TRUE;		// X-joystick ON
	m_bYJoystickOn				= TRUE;		// Y-joystick ON
	m_bConfirmSrch				= FALSE;
	m_bStartPoint				= FALSE;
	m_bMoveBack					= FALSE;

	m_bScnLoaded				= FALSE;	// SCN file is not loaded
	m_bEnableGlobalTheta		= FALSE;
	m_szWaferMapFormat			= "";
	m_bAutoSrchHome				= FALSE;
	m_bResetMapStatus			= TRUE;
	if (pApp->GetCustomerName() == CTM_NICHIA && pApp->GetProductLine()==_T(""))	
	{
		m_bResetMapStatus = FALSE;
	}
	m_ucAlignStepSize			= 1;
	m_bCheckCOR					= FALSE;	//Block2
	m_lTotalRefDieCount			= 0;		//Block2
	m_lCurrRefDieCount			= 0;		//Block2

	m_bLockJoystick				= FALSE;
	m_lLastPickRow				= 0;
	m_lLastPickCol				= 0;
	m_bLastPickSet				= FALSE;

	//v3.28T1
	//Get Block-Pick mode from feature
	if (pApp->GetFeatureValue(MS896A_FUNC_WAFERMAP_BLOCK_MODE) == 2)	//OptoTech
	{
		m_bFullRefBlock = FALSE;
	}
	else if (pApp->GetFeatureValue(MS896A_FUNC_WAFERMAP_BLOCK_MODE) == 1)	//Block2
	{
		m_bFullRefBlock = TRUE;		//Osram mode (with regular ref die)
	}

	m_lMnSrchRefGrid			= 10;
	m_lMnSrchHomeGrid			= 3;
	m_lMnMaxJumpCtr				= 15;
	m_lMnMaxJumpEdge			= 7;
	m_lMnAlignReset				= 0;
	m_lMnNoDieGrade				= 17;
	m_lMnMinDieForPick			= 0;
	m_lMnAlignCheckRow			= 500;
	m_lMnAlignCheckCol			= 500;
	m_bMnPositionSwitch			= FALSE;
	m_lNumOfRetry				= 0;	//5; Disable retry load map
	m_lMapRetryDelay			= 0;	//15;
	m_lRealignDieNum			= 0;	//Block2
	m_bMnEdgeAlign				= FALSE;
	m_lMnPassPercent			= 70;
	m_bOCRDigitalF				= FALSE;
	m_lBlkAutoAlignPosX			= 0;
	m_lBlkAutoAlignPosY			= 0;
	m_lBlkAutoAlignPosT			= 0;
	m_ulBlkAutoAlignRow			= 0;
	m_ulBlkAutoAlignCol			= 0;
	m_bAutoUpdateRefDieOffset	= FALSE;	//Block2

	//Get resolution & motor direction from config file
	m_dEjtXYRes					= GetChannelResolution(MS896A_CFG_CH_EJECTOR_X) * 1000.0; // M/Count ==> um/count
	m_dXYRes					= GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X);
	m_dThetaRes					= GetChannelResolution(MS896A_CFG_CH_WAFTABLE_T);
	m_lThetaMotorDirection		= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MOTOR_DIRECTION);

	m_lCorMovePitchTolX			= 40;
	m_lCorMovePitchTolY			= 40;
	m_lCorBackPitchTolX			= 20;
	m_lCorBackPitchTolY			= 20;
	m_lPredScanPitchTolX		= 0;
	m_lPredScanPitchTolY		= 0;
	m_lRunPredScanPitchTolX		= m_lPredScanPitchTolX;
	m_lRunPredScanPitchTolY		= m_lPredScanPitchTolY;
	m_lPitchX_Tolerance			= 0;
	m_lPitchY_Tolerance			= 0;
	m_bDiePitchCheck			= FALSE;
	m_bAbsDiePitchCheck			= FALSE;	//v2.78T2
	m_bLimitCheck				= TRUE;

	m_lXCheckToleranceX			= 30;
	m_lXCheckToleranceY			= 30;
	m_bEnableXCheck				= FALSE;
	m_bPickCrossCheckDie		= FALSE;

	m_lLongJumpDelay			= 0;
	m_lLongJumpMotTime			= 0;

	m_lRefCrossRow				= 0;
	m_lRefCrossCol				= 0;
	m_lRefCrossWidth			= 0;	//0 = 1; 1 = 3; 2 = 5;

	m_lSubSortMode				= 0;
	m_bSortOuterHmi				= FALSE;

	m_bIgnoreRegion				= FALSE;
	m_ucSelectChangeGrade		= 0;	
	m_lCurrentSelectRegion		= 1;
	for (INT i=0; i<WT_MAX_SELECT_REGION; i++)
	{
		m_ulSelectRegionLRX[i] = 0;
		m_ulSelectRegionLRY[i] = 0;
		m_ulSelectRegionULX[i] = 0;
		m_ulSelectRegionULY[i] = 0;
	}

	m_bEnableSCNCheck			= FALSE;
	m_bKeepSCNAlignDie			= FALSE;
	m_bScnCheckByPR				= FALSE;
	m_bScnCheckIsRefDie			= FALSE;
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		//m_bEnableSCNCheck		= TRUE; 
		m_bScnCheckByPR			= TRUE;
		m_bScnCheckIsRefDie		= TRUE;
	}
	m_lScnCheckRefDieNo			= 0;
	m_bCheckSCNLoaded			= FALSE;
	m_lScnCheckXTol				= 0;
	m_lScnCheckYTol				= 0;

	m_bEnableScnCheckAdjust		= FALSE;
	m_lRefHomeDieRowOffset		= 0;
	m_lRefHomeDieColOffset		= 0;
	m_lRefHomeDieRow			= 0;
	m_lRefHomeDieCol			= 0;

	m_lSortingMode				= WT_SORT_NORMAL;
	m_ulMinGradeCount			= 0;
	m_ulMinGradeBinNo			= 0;
	m_unArrMinGradeBin.SetSize(0, 1);

	m_bMapSyncMove				= FALSE;

	m_ucChangeStartGrade		= 1;
	m_ucChangeEndGrade			= 1;
	m_ucChangeTargetGrade		= 1;

	m_lGradeCount				= 0;
	//4.53D16
	m_bEnableMatchMapHeader		= FALSE;

	m_bWaferControlPageNo		= FALSE;
	m_bCheckSepGrade			= FALSE;

	m_bEnableFixMapName			= FALSE;
	m_szFixMapName				= "";
	m_bEnablePrefix				= FALSE;
	m_bEnablePrefixCheck		= FALSE;
	m_bEnableSuffixCheck		= FALSE;
	m_szPrefixName				= "";
	m_bMatchFileName			= FALSE;

	m_ulAlignOption				= WT_MANUAL_HOMEDIE;
	m_ulCrossHomeDieRow			= 0;
	m_ulCrossHomeDieCol			= 0;
	m_lHomeDieMapRow			= 0;
	m_lHomeDieMapCol			= 0;
	m_lOrgMapRowOffset			= 0;
	m_lOrgMapColOffset			= 0;
	m_lScnMapRowOffset			= 0;
	m_lScnMapColOffset			= 0;

	m_bFindHomeDieFromMap	= TRUE;
	m_ulHomePatternRow		= 3;
	m_ulHomePatternCol		= 3;
	m_ulHomePitchRow		= 30;
	m_ulHomePitchCol		= 30;
	m_ulHomeKeyDieRow		= 1;
	m_ulHomeKeyDieCol		= 1;

	m_bUseWaferMapCategory		= FALSE;

	m_lHomeDieWftPosnX			= 0;
	m_lHomeDieWftPosnY			= 0;
	m_lAlignWaferAngleMode_Std	= 0;

	m_ucDieShape				= WPR_RECTANGLE_DIE;
	m_szNextPath				= " ";

	m_bCheckRepeatMap			= FALSE;
	m_szCurrentMapPath			= "";
	m_lMapValidMinRow			= 0;
	m_lMapValidMinCol			= 0;
	m_lMapValidMaxRow			= 0;
	m_lMapValidMaxCol			= 0;


	memset(&m_lScnCheckRow, 0, sizeof(m_lScnCheckRow));
	memset(&m_lScnCheckCol, 0, sizeof(m_lScnCheckCol));

	m_lTotalSCNCount			= WT_ALN_MAXCHECK_SCN;

	m_bCheckNullBinInMap		= FALSE;
	m_bCheckMapHomeNullBin		= FALSE;

	m_bIsCircleLimit			= TRUE;
	m_ucWaferLimitType			= WT_CIRCLE_LIMIT;
	m_lWaferPolyLimitNo			= 3;
	memset(&m_lWaferPolyLimitX, 0, sizeof(m_lWaferPolyLimitX));
	memset(&m_lWaferPolyLimitY, 0, sizeof(m_lWaferPolyLimitY));

	m_bStopBinLoader			= FALSE;
	m_bIsWaferEnded				= FALSE;

	m_bSelOfUpdateMapPos		= FALSE;
	m_bDisableRefDie			= FALSE;
	m_bCheckGDOnRefDiePos		= FALSE;

	// For Wafer Lot File
	m_bCheckLotWaferName = FALSE;
	m_szLotInfoFilePath = "";
	m_szLotInfoFileName = "";
	m_szLotInfoFileExt = "";
	m_szLotInfoOpID	=	"";
	m_szOutWaferInfoFilePath = "";

	m_nCornerHomeDiePosX = 0;
	m_nCornerHomeDiePosY = 0;

	m_bKeepLastUnloadGrade	= FALSE;
	//m_ucLastUnloadGrade		= 0;

	m_bEnableSmartWalkInPicknPlace = FALSE;
	m_szDummyWaferMapName = "";

	//Sorting Check Pts
	m_bEnableSortingCheckPts = FALSE;

	m_bIs1stDieFinderStarted = FALSE;		
	m_bIs1stDieAFinderStarted = FALSE;

	//m_szBinDieType = "";
	m_szUserDefineDieType = "";
	m_bEnableMapDieTypeCheck = FALSE;
	m_bIsMapDieTypeCheckOk = FALSE;

	m_bEnableAutoMapDieTypeCheck = FALSE;
	m_bSelectAutoCheckLotID		 = FALSE;
	m_bSelectAutoCheckSerialNum  = FALSE;
	m_bSelectAutoCheckProductNum = FALSE;
	m_bSelectAutoCheckMode		 = FALSE;

	m_szAutoCheckLotID		= "";
	m_szAutoCheckSerialNum  = "";
	m_szAutoCheckProductNum = "";
	m_szAutoCheckMode		= "";

	m_bIsAutoCheckLotIDOk		 = FALSE;
	m_bIsAutoCheckSerialNumOk	 = FALSE;
	m_bIsAutoCheckProductNumOk	 = FALSE;
	m_bIsAutoCheckModeOk		 = FALSE;

	m_bMESWaferIDCheckOk = FALSE;
	m_szMESWaferIDCheckMsg = "MES WaferID Check Fail";

	m_bEnableMESConnection = FALSE;
	m_szMESIncomingPath = "";
	m_szMESOutgoingPath = "";
	m_lMESTimeout = 8;

	m_bStartGoToAlignDie = FALSE;		//v3.30T1

	m_bEnableTwoDimensionsBarcode	= FALSE;	//v3.33T1
	m_b2DBarCodeCheckGrade99		= FALSE;
	m_bOTraceabilityDisableLotID	= FALSE;

	m_bEnableIM	= FALSE;

	m_bBlkFuncPage 	= FALSE;	//Block2
	m_bBlkFuncPage2 = FALSE;

	m_lSlowProfileTrigger = 50000;
	
	m_bLoadCurrentMapStatus = FALSE;

	m_bEnableWTMotionLog = FALSE;
	m_lMotionTestDelay = 0;
	m_lMotionTestDist = 0;
	m_bIsMotionTestWaferTableX = FALSE;
	m_bIsMotionTestWaferTableY = FALSE;	
	m_bIsMotionTestWaferTable2X = FALSE;
	m_bIsMotionTestWaferTable2Y = FALSE;	

	m_ucWaferLimitTemplateNoSel = 0;

	for (INT i=0; i<WT_WAFER_LIMIT_TEMPLATE_NO; i++)
	{
		for (INT j=0; j<WT_MAX_POLYGON_PT; j++)
		{
			m_oWaferTableLimitTmp[i].lWaferPolyLimitX[j] = 0;
			m_oWaferTableLimitTmp[i].lWaferPolyLimitX[j] = 0;
		}

		m_oWaferTableLimitTmp[i].lWaferPolyLimitNo = 0;
		m_oWaferTableLimitTmp[i].ucWaferLimitType = WT_CIRCLE_LIMIT;
	}

	SetWTAtColletPos(0);	//CENTER
	m_nWTLastColletPos = 0;	//v4.47T2

	m_lWTNoDiePosX = 0;
	m_lWTNoDiePosY = 0;
	m_lLastGradeSetToEquip = 0;

	m_bUseLF2ndAlignment		= FALSE;
	m_bUseLF2ndAlignNoMotion	= FALSE;	//andrewng //2020-05-19

// prescan relative code
	InitPrescanVariables();
// Temperature Controller
	TC_InitVariables();
	m_lCPClbPrDelay = 100;

	m_lBpX = 0;
	m_lBpY = 0;
	m_lBpT = 0;

	m_nProfile_X = NORMAL_PROF;
	m_nProfile_Y = NORMAL_PROF;
	m_nProfile_X2	= NORMAL_PROF;
	m_nProfile_Y2	= NORMAL_PROF;
	m_nProfile_T	= NORMAL_PROF;
	m_nProfile_T2	= NORMAL_PROF;

	m_lChangeLightSCNContinue = 0;

	m_nChangeColletOnWT = FALSE;
	m_bStopBonding = FALSE;

	m_bEnablePrescan_Backup = FALSE;
	m_bMS90HalfSortMode_Backup = FALSE;

	m_lBackupDummyPosX	= 0;
	m_lBackupDummyPosY	= 0;
}


BOOL CWaferTable::SetWaferIndexingPathParameters()
{
	LONG lHoriDiePitchX, lHoriDiePitchY, lVertDiePitchX, lVertDiePitchY;
	LONG lNoOfRows, lNoOfCols;

	lHoriDiePitchX	= GetDiePitchX_X();
	lHoriDiePitchY	= GetDiePitchX_Y();
	lVertDiePitchX	= GetDiePitchY_X();
	lVertDiePitchY	= GetDiePitchY_Y();
	lHoriDiePitchX	= labs(lHoriDiePitchX);
	lVertDiePitchY	= labs(lVertDiePitchY);

	if (lVertDiePitchY == 0)
		lNoOfRows = 0;
	else
		lNoOfRows = GetWaferDiameter() / lVertDiePitchY + 2;

	if (lHoriDiePitchX == 0)
		lNoOfCols = 0;
	else
		lNoOfCols = GetWaferDiameter() / lHoriDiePitchX + 2;

	if (lNoOfRows > lNoOfCols)
		lNoOfCols = lNoOfRows;
	else
		lNoOfRows = lNoOfCols;

	if ((IsEnablePNP() == TRUE))
	{
		if( (m_bEnableSmartWalkInPicknPlace == FALSE) )
		{
			CreateDummyMapForPicknPlace(lNoOfRows, lNoOfCols, lHoriDiePitchX, lHoriDiePitchY, 
				lVertDiePitchX, lVertDiePitchY);
			m_lPNPWftIndexLimit		= (m_lWaferEmptyLimit+1)*(m_lWaferStreetLimit+1)*2 + 10;
		}
		else
		{
			CrateDummyMapForSmartWalkWithPicknPlace(lNoOfRows, lNoOfCols, lHoriDiePitchX, lHoriDiePitchY, 
				lVertDiePitchX, lVertDiePitchY);
			m_lPNPWftIndexLimit		= 0;
		}
	}

	return TRUE;
} //end SetWaferIndexingPathParameters

BOOL CWaferTable::CreateDummyMapForNichia(LONG lNoOfRows,LONG lNoOfCols, LONG lHoriDiePitchX,
										  LONG lHoriDiePitchY, LONG lVertDiePitchX, LONG lVertDiePitchY)
{
	unsigned char aucAvailableGrades[2] = {'0', '1'};
	aucAvailableGrades[0] = 48 + 0;
	aucAvailableGrades[1] = 48 + 1;
//	unsigned char aaTempGrades[1];
	CString szDummyWaferMapName;
	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;
	//pGradeBuffer = new unsigned char[lNoOfRows*lNoOfCols];	//v4.49 Klocwork
	int i, j;

	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows*lNoOfCols];

	aaGrades = new unsigned char*[lNoOfRows];
	for (i=0; i<lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i]; //new unsigned char[lNoOfCols];
	}

	// Set the grades
	for (i=0; i<lNoOfRows; i++) 
	{
		for (j=0; j<lNoOfCols; j++) 
		{
			aaGrades[i][j] =WAF_CMapConfiguration::DEFAULT_NULL_BIN;
		}
	}

	//Clear Wafermap & reset grade map
	m_WaferMapWrapper.InitMap();
	m_WaferMapWrapper.ResetGradeMap();
	SetPsmEnable(FALSE);

	if (m_szDummyWaferMapName .IsEmpty())
	{
		szDummyWaferMapName = "DummyWaferMap";
	}
	else
	{
		szDummyWaferMapName = m_szDummyWaferMapName;
	}


	for (i=0; i<lNoOfRows; i++) 
	{
		for (j=0; j<lNoOfCols; j++) 
		{
			aaGrades[i][j] ='1';
		}
	}
	//LONG lNoOfCols = 10;
	//LONG lNoOfRows = 10;
	m_WaferMapWrapper.CreateMap(szDummyWaferMapName, "Raw", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);
	
	delete [] aaGrades;
	delete [] pGradeBuffer;
	return TRUE;
}
		
BOOL CWaferTable::CreateDummyMapForPicknPlace(LONG lNoOfRows,LONG lNoOfCols, LONG lHoriDiePitchX, 
											  LONG lHoriDiePitchY, LONG lVertDiePitchX, LONG lVertDiePitchY)
{
	CString szAlgorithm;
	CString szDummyWaferMapName;

	unsigned char aucAvailableGrades[1] = {'1'};
	unsigned char aaTempGrades[1];

	aucAvailableGrades[0] = 48 + m_ucDummyPrescanPNPGrade;
	aaTempGrades[0] = 48 + m_ucDummyPrescanPNPGrade;

	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;
	int i, j;

	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows*lNoOfCols];

	aaGrades = new unsigned char*[lNoOfRows];
	for (i=0; i<lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i]; //new unsigned char[lNoOfCols];
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
	m_WaferMapWrapper.InitMap();
	m_WaferMapWrapper.ResetGradeMap();
	SetPsmEnable(FALSE);

	if (m_szDummyWaferMapName .IsEmpty())
	{
		szDummyWaferMapName = "DummyWaferMap";
	}
	else
	{
		szDummyWaferMapName = m_szDummyWaferMapName;
	}

	m_szCurrentMapPath = szDummyWaferMapName;
	
	m_WaferMapWrapper.CreateMap(szDummyWaferMapName, "Raw", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);
	m_lMapValidMinRow	= 0;
	m_lMapValidMinCol	= 0;
	m_lMapValidMaxRow	= lNoOfRows;
	m_lMapValidMaxCol	= lNoOfCols;

	SetErrorMessage("PNP Create dummy map " + szDummyWaferMapName);
	m_pWaferMapManager->SuspendAlgorithmPreparation();

	delete [] aaGrades;
	delete [] pGradeBuffer;

	// Delete the file generated to prevent too many file
	DeleteFile(szDummyWaferMapName);


	switch( m_lPickAndPlaceWalkPath )
	{
	case 0:
		szAlgorithm = "Pick and Place: TLH-Circle";
		break;
	case 1:
		szAlgorithm = "Pick and Place: BLH-Circle";
		break;
	case 2:
		szAlgorithm = "Pick and Place: TLV-Circle";
		break;
	case 3:
		szAlgorithm = "Pick and Place: TRV-Circle";
		break;
	}

	m_WaferMapWrapper.SelectAlgorithm(szAlgorithm, "DummyMinDistance");

	//Set parameters
	SetAlignmentStatus(FALSE);

	m_WaferMapWrapper.SetAlgorithmParameter("WaferCenterX", GetWaferCenterX());
	m_WaferMapWrapper.SetAlgorithmParameter("WaferCenterY", GetWaferCenterY());
	m_WaferMapWrapper.SetAlgorithmParameter("WaferDiameter", GetWaferDiameter());
	m_WaferMapWrapper.SetAlgorithmParameter("EmptyHoleLimit", m_lWaferEmptyLimit*(m_lWaferStreetLimit+1));
	m_WaferMapWrapper.SetAlgorithmParameter("EndStreetLimit", m_lWaferStreetLimit);
	m_WaferMapWrapper.SetAlgorithmParameter("HoriDiePitchX", lHoriDiePitchX);
	m_WaferMapWrapper.SetAlgorithmParameter("HoriDiePitchY", lHoriDiePitchY);
	m_WaferMapWrapper.SetAlgorithmParameter("VertDiePitchX", lVertDiePitchX);
	m_WaferMapWrapper.SetAlgorithmParameter("VertDiePitchY", lVertDiePitchY);
	m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aaTempGrades[0]);

	m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);
	m_WaferMapWrapper.ResumeAlgorithmPreparation(TRUE);
	
	return TRUE;
}

BOOL CWaferTable::CrateDummyMapForSmartWalkWithPicknPlace(LONG lNoOfRows,LONG lNoOfCols, LONG lHoriDiePitchX, 
														  LONG lHoriDiePitchY, LONG lVertDiePitchX, LONG lVertDiePitchY)
{
	CString szDummyWaferMapName;
	CString szAlgorithm;
	unsigned char aucAvailableGrades[1] = {'1'};
	unsigned char aaTempGrades[1];

	aucAvailableGrades[0] = 48 + m_ucDummyPrescanPNPGrade;
	aaTempGrades[0] = 48 + m_ucDummyPrescanPNPGrade;

	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;
	int i, j;

	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows*lNoOfCols];

	aaGrades = new unsigned char*[lNoOfRows];
	for (i=0; i<lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i]; //new unsigned char[lNoOfCols];
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
	m_WaferMapWrapper.InitMap();
	m_WaferMapWrapper.ResetGradeMap();
	SetPsmEnable(FALSE);

	if (m_szDummyWaferMapName .IsEmpty())
	{
		szDummyWaferMapName = "DummyWaferMap";
	}
	else
	{
		szDummyWaferMapName = m_szDummyWaferMapName;
	}

	m_szCurrentMapPath = szDummyWaferMapName;

	//v3.69T1	//andrew	//as advised by Sammy
	m_WaferMapWrapper.CreateMap(szDummyWaferMapName, "General", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);
	m_lMapValidMinRow	= 0;
	m_lMapValidMinCol	= 0;
	m_lMapValidMaxRow	= lNoOfRows;
	m_lMapValidMaxCol	= lNoOfCols;

	SetErrorMessage("SM WALK PNP Create dummy map " + szDummyWaferMapName);

	DOUBLE dLimitPct = 0.98;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="OSRAM" )
	{
		dLimitPct = 1.0;
	}
	for (ULONG i=0; i< (ULONG)lNoOfRows; i++)
	{
		for (ULONG j=0; j< (ULONG)lNoOfCols; j++)
		{
			LONG lEncX,lEncY;
			
			lEncX = GetCircleWaferMaxX() - j* (lHoriDiePitchX + lVertDiePitchX);
			lEncY = GetCircleWaferMaxY() - i* (lHoriDiePitchY + lVertDiePitchY);

			if (IsWithinWaferLimit(lEncX, lEncY, dLimitPct ) == TRUE)
			{
				m_WaferMapWrapper.AddDie(i, j, aaTempGrades[0]);
			}
		}
	}

	m_pWaferMapManager->SuspendAlgorithmPreparation();

	delete [] aaGrades;
	delete [] pGradeBuffer;

	// Delete the file generated to prevent too many file
	DeleteFile(szDummyWaferMapName);

	switch( m_lPickAndPlaceWalkPath )
	{
	case 0:
		szAlgorithm = "TLHSW";
		break;
	case 1:
		szAlgorithm = "BLHSW";
		break;
	case 2:
		szAlgorithm = "TLVSW";
		break;
	case 3:
		szAlgorithm = "TRVSW";
		break;
	}

	m_WaferMapWrapper.SelectAlgorithm(szAlgorithm, "Direct");
	m_WaferMapWrapper.SetPickMode(WAF_CDieSelectionAlgorithm::MIXED_ORDER);

	//Set parameters
	SetAlignmentStatus(FALSE);

	m_WaferMapWrapper.SetAlgorithmParameter("WaferCenterX", GetWaferCenterX());
	m_WaferMapWrapper.SetAlgorithmParameter("WaferCenterY", GetWaferCenterY());
	m_WaferMapWrapper.SetAlgorithmParameter("WaferDiameter", GetWaferDiameter());
	m_WaferMapWrapper.SetAlgorithmParameter("EmptyHoleLimit", m_lWaferEmptyLimit);
	m_WaferMapWrapper.SetAlgorithmParameter("EndStreetLimit", m_lWaferStreetLimit);
	m_WaferMapWrapper.SetAlgorithmParameter("HoriDiePitchX", lHoriDiePitchX);
	m_WaferMapWrapper.SetAlgorithmParameter("HoriDiePitchY", lHoriDiePitchY);
	m_WaferMapWrapper.SetAlgorithmParameter("VertDiePitchX", lVertDiePitchX);
	m_WaferMapWrapper.SetAlgorithmParameter("VertDiePitchY", lVertDiePitchY);
	m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aaTempGrades[0]);

	m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);
	m_WaferMapWrapper.ResumeAlgorithmPreparation(TRUE);

	return TRUE;
}


//andrewng //2020-0622
BOOL CWaferTable::CreateBurnInMapWith2Grades(LONG lNoOfRows,LONG lNoOfCols, LONG lGrade1, LONG lGrade2)	//104, 40
{
	unsigned char aucAvailableGrades[2];
	aucAvailableGrades[0] = m_WaferMapWrapper.GetGradeOffset() + lGrade1;
	aucAvailableGrades[1] = m_WaferMapWrapper.GetGradeOffset() + lGrade2;

	int i, j;
	CString szDummyWaferMapName;
	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;

	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows * lNoOfCols];
	aaGrades = new unsigned char*[lNoOfRows];

	for (i = 0; i < lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i];	//new unsigned char[lNoOfCols];
	}

	// Set the grades
	for (i = 0; i < lNoOfRows; i++) 
	{
		for (j = 0; j < lNoOfCols; j++) 
		{
			aaGrades[i][j] = WAF_CMapConfiguration::DEFAULT_NULL_BIN;
		}
	}

	//Clear Wafermap & reset grade map
	m_WaferMapWrapper.InitMap();
	m_WaferMapWrapper.ResetGradeMap();
	SetPsmEnable(FALSE);

		szDummyWaferMapName = "ASM BURNIN Map";


	for (i = 0; i < lNoOfRows; i++) 
	{
		for (j = 0; j < lNoOfCols; j++) 
		{
			if (i % 2 == 0)
			{
				if (j % 2 == 0)
				{
					aaGrades[i][j] = m_WaferMapWrapper.GetGradeOffset() + lGrade2;
				}
				else
				{
					aaGrades[i][j] = m_WaferMapWrapper.GetGradeOffset() + lGrade1;
				}
			}
			else
			{
				if (j % 2 == 0)
				{
					aaGrades[i][j] = m_WaferMapWrapper.GetGradeOffset() + lGrade1;
				}
				else
				{
					aaGrades[i][j] = m_WaferMapWrapper.GetGradeOffset() + lGrade2;
				}
			}
		}
	}

	//LONG lNoOfCols = 10;
	//LONG lNoOfRows = 10;
	m_WaferMapWrapper.CreateMap(szDummyWaferMapName, "Raw", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);
	
	delete [] aaGrades;
	delete [] pGradeBuffer;
	return TRUE;
}


BOOL CWaferTable::LoadWaferTblData(CONST BOOL bOnlyPPKG)
{
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	int i, j;
	CString szTemp, szX, szY;

    // open config file
    if (pUtl->LoadWTConfig() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
    psmf = pUtl->GetWTConfigFile();

	//Check Load/Save Data
	if ( psmf == NULL )
	{
		return FALSE;
	}

	if( bOnlyPPKG==TRUE )
	{
		BOOL bPpkgRegion = (BOOL)(LONG) (*m_psmfSRam)["MS896A"]["Include Region"];
		if( bPpkgRegion )
		{
			WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
			pSRInfo->m_ulSubRows	= (LONG)(*psmf)[WT_OPTION][WT_MAP_SUB_ROWS];
			pSRInfo->m_ulSubCols	= (LONG)(*psmf)[WT_OPTION][WT_MAP_SUB_COLS];
			m_lSubSortMode			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_SORT_BY_GRADE];
			m_bMultiWaferSupport	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_MULTI_WAFER];
		}

		LoadPrescanOptionFromWFT(psmf);
		// Temperature Controller
		TC_LoadOption(psmf);

		// close config file
		pUtl->CloseWTConfig();

		return TRUE;
	}

	m_lWaferUnloadX			= (*psmf)[WT_PROCESS_DATA][WT_UNLOAD_X];
	m_lWaferUnloadY			= (*psmf)[WT_PROCESS_DATA][WT_UNLOAD_Y];
	m_lWaferCenterX			= (*psmf)[WT_PROCESS_DATA][WT_CENTER_X];
	m_lWaferCenterY			= (*psmf)[WT_PROCESS_DATA][WT_CENTER_Y];
	m_lWaferCalibX			= (*psmf)[WT_PROCESS_DATA][WT_CALIB_X];
	m_lWaferCalibY			= (*psmf)[WT_PROCESS_DATA][WT_CALIB_Y];
	m_lWaferCalibX2			= (*psmf)[WT_PROCESS_DATA][WT_CALIB_X2];
	m_lWaferCalibY2			= (*psmf)[WT_PROCESS_DATA][WT_CALIB_Y2];
	m_lWaferType			= (*psmf)[WT_PROCESS_DATA][WT_WAFER_TYPE];
	m_lWaferSize			= (*psmf)[WT_PROCESS_DATA][WT_WAFER_SIZE];
	m_lWaferWorkingAngle	= (*psmf)[WT_PROCESS_DATA][WT_WORKING_ANGLE];
	m_lWaferIndexPath		= (*psmf)[WT_PROCESS_DATA][WT_INDEX_PATH];
	m_ucWaferLimitType		= (*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TYPE];
	
	m_lWTNoDiePosX			= (*psmf)[WT_OPTION][WT_No_Die_COOR_X];
	m_lWTNoDiePosY			= (*psmf)[WT_OPTION][WT_No_Die_COOR_Y];

	m_bUseLF2ndAlignment		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_Use_LF_2nd_Alignment];
	m_bUseLF2ndAlignNoMotion	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_Use_LF_2nd_Align_NoMotion];	//andrewng //2020-05-19

	m_lEjtOffsetX			= (*psmf)[WT_PROCESS_DATA][WT_NGPICK_OFFSET_X];		//v4.24T10
	m_lEjtOffsetY			= (*psmf)[WT_PROCESS_DATA][WT_NGPICK_OFFSET_Y];		//v4.24T10
	
	m_lWT2OffsetX			= (*psmf)[WT_PROCESS_DATA][WT2_OFFSET_X];			//v4.24T10
	m_lWT2OffsetY			= (*psmf)[WT_PROCESS_DATA][WT2_OFFSET_Y];			//v4.24T10
	if( labs(m_lWT2OffsetX-(-487-407)*1000*2)>50*1000*2 )	// V1.1 415 and 495
		m_lWT2OffsetX = (-487-407)*1000*2;
	if( labs(m_lWT2OffsetY)>30*1000*2 )
		m_lWT2OffsetY = 0;
	
	m_lWaferPolyLimitNo		= (*psmf)[WT_PROCESS_DATA][WT_WAFER_POLY_LIMIT][WT_POLYGON_LIMIT_NO];
	if ( m_lWaferPolyLimitNo < 3 )
	{
		m_lWaferPolyLimitNo = 3;
	}

	for (i=0; i<WT_MAX_POLYGON_PT; i++)
	{
		m_lWaferPolyLimitX[i] = (*psmf)[WT_PROCESS_DATA][WT_WAFER_POLY_LIMIT][i][WT_X];
		m_lWaferPolyLimitY[i] = (*psmf)[WT_PROCESS_DATA][WT_WAFER_POLY_LIMIT][i][WT_Y];
	}

	m_ulHomePatternRow	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_PATTERN_ROW];
	m_ulHomePatternCol	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_PATTERN_COL];
	m_ulHomePitchRow	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_PITCH_ROW];
	m_ulHomePitchCol	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_PITCH_COL];
	m_ulHomeKeyDieRow	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_KEY_DIE_ROW];
	m_ulHomeKeyDieCol	= (ULONG)(LONG)(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_KEY_DIE_COL];
	{
		if( m_ulHomePatternRow<3 )
			m_ulHomePatternRow = 3;
		if( m_ulHomePatternCol<3 )
			m_ulHomePatternCol = 3;
		if( m_ulHomePitchRow<10 )
			m_ulHomePitchRow = 10;
		if( m_ulHomePitchCol<10 )
			m_ulHomePitchCol = 10;
	}

	m_bIsCircleLimit = TRUE;
	if ( m_ucWaferLimitType != WT_CIRCLE_LIMIT )
	{
		m_bIsCircleLimit = FALSE;
	}

	if (CMS896AStn::m_bEnableWaferSizeSelect == TRUE)
	{
		for (i=0; i<WT_WAFER_LIMIT_TEMPLATE_NO; i++)
		{
			szTemp.Format("Node_%d", i+1);

			for (j=0; j<WT_MAX_POLYGON_PT; j++)
			{
				szX.Format("X_%d",j+1);
				szY.Format("Y_%d",j+1);

				m_oWaferTableLimitTmp[i].lWaferPolyLimitX[j] = (*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][szX];
				m_oWaferTableLimitTmp[i].lWaferPolyLimitY[j] = (*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][szY];
				
			}

			m_oWaferTableLimitTmp[i].bIsSetupComplete = (BOOL)(LONG)(*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][WT_TEMPLATE_SETUP_COMPLETE];
			m_oWaferTableLimitTmp[i].lWaferPolyLimitNo = (*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][WT_POLYGON_LIMIT_NO];
			m_oWaferTableLimitTmp[i].ucWaferLimitType = (*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][WT_WAFER_LIMIT_TYPE];
		}
	}

	LoadPrescanOptionFromWFT(psmf);
// Temperature Controller
	TC_LoadOption(psmf);

	m_bMESWaferIDCheckOk = (BOOL)(LONG)(*psmf)[WT_MES_CONNECTION][WT_MES_WAFER_ID_CHECK_STATUS];
	m_szMESWaferIDCheckMsg = (*psmf)[WT_MES_CONNECTION][WT_MES_WAFER_ID_CHECK_MSG];

    // close config file
    pUtl->CloseWTConfig();

	m_lAutoWaferCenterX = GetWaferCenterX();
	m_lAutoWaferCenterY = GetWaferCenterY();
	m_lAutoWaferDiameter = GetWaferDiameter();
	m_lAutoWaferWidth	= GetWaferDiameter();
	m_lAutoWaferHeight	= GetWaferDiameter();

	//Update Wafer Diameter on HMI (mil)
	m_dWaferDiameter = ((DOUBLE)GetWaferDiameter() * m_dXYRes) / 25.4;

	//Update current working angle;
	m_lWaferCurrentWorkingAngle = m_lWaferWorkingAngle;

	if (m_fHardware && !m_bDisableWT && GetWaferDiameter() != 0)
	{
		try
		{
			//update joystick limit
			SetJoystickOn(FALSE);
			if( IsES101()==FALSE )	// 4.24TX1
			{
				CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
				CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);
			}
			SetJoystickOn(TRUE);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
			CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
			if( IsESDualWT() )
			{
				CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
				CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
			}
		}	
	}

    return TRUE;
}


BOOL CWaferTable::SaveWaferTblData(VOID)
{
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	int i,j;
	CString szTemp, szX, szY;

    // open config file
    if (pUtl->LoadWTConfig() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
    psmf = pUtl->GetWTConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	// update data
	(*psmf)[WT_PROCESS_DATA][WT_UNLOAD_X]			= m_lWaferUnloadX;
	(*psmf)[WT_PROCESS_DATA][WT_UNLOAD_Y]			= m_lWaferUnloadY;
	(*psmf)[WT_PROCESS_DATA][WT_CENTER_X]			= m_lWaferCenterX;
	(*psmf)[WT_PROCESS_DATA][WT_CENTER_Y]			= m_lWaferCenterY;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_X]			= m_lWaferCalibX;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_Y]			= m_lWaferCalibY;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_X2]			= m_lWaferCalibX2;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_Y2]			= m_lWaferCalibY2;
	(*psmf)[WT_PROCESS_DATA][WT_WAFER_TYPE]			= m_lWaferType;
	(*psmf)[WT_PROCESS_DATA][WT_WAFER_SIZE]			= m_lWaferSize;
	(*psmf)[WT_PROCESS_DATA][WT_WORKING_ANGLE]		= m_lWaferWorkingAngle;
	(*psmf)[WT_PROCESS_DATA][WT_INDEX_PATH]			= m_lWaferIndexPath;
	(*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TYPE]	= m_ucWaferLimitType;
	
	//v3.44		//SHowaDenko COR calculation
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_COR_A1]		= m_dCorOffsetA1;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_COR_A2]		= m_dCorOffsetA2;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_COR_B1]		= m_dCorOffsetB1;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_COR_B2]		= m_dCorOffsetB2;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_ACOR_A1]		= m_dACorOffsetA1;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_ACOR_A2]		= m_dACorOffsetA2;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_ACOR_B1]		= m_dACorOffsetB1;
	(*psmf)[WT_PROCESS_DATA][WT_CALIB_ACOR_B2]		= m_dACorOffsetB2;

	(*psmf)[WT_PROCESS_DATA][WT_NGPICK_OFFSET_X]	= m_lEjtOffsetX;		//v4.24T10
	(*psmf)[WT_PROCESS_DATA][WT_NGPICK_OFFSET_Y]	= m_lEjtOffsetY;		//v4.24T10

	if( labs(m_lWT2OffsetX-(-487-407)*1000*2)>50*1000*2 )	// V1.1 415 and 495
		m_lWT2OffsetX = (-487-407)*1000*2;
	if( labs(m_lWT2OffsetY)>30*1000*2 )
		m_lWT2OffsetY = 0;

	(*psmf)[WT_PROCESS_DATA][WT2_OFFSET_X]			= m_lWT2OffsetX;
	(*psmf)[WT_PROCESS_DATA][WT2_OFFSET_Y]			= m_lWT2OffsetY;

	(*psmf)[WT_PROCESS_DATA][WT_WAFER_POLY_LIMIT][WT_POLYGON_LIMIT_NO]  = m_lWaferPolyLimitNo;
	for (i=0; i<WT_MAX_POLYGON_PT; i++)
	{
		(*psmf)[WT_PROCESS_DATA][WT_WAFER_POLY_LIMIT][i][WT_X] = m_lWaferPolyLimitX[i];
		(*psmf)[WT_PROCESS_DATA][WT_WAFER_POLY_LIMIT][i][WT_Y] = m_lWaferPolyLimitY[i];
	}

	{
		if( m_ulHomePatternRow<3 )
			m_ulHomePatternRow = 3;
		if( m_ulHomePatternCol<3 )
			m_ulHomePatternCol = 3;
		if( m_ulHomePitchRow<10 )
			m_ulHomePitchRow = 10;
		if( m_ulHomePitchCol<10 )
			m_ulHomePitchCol = 10;
	}
	(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_PATTERN_ROW]	= m_ulHomePatternRow;
	(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_PATTERN_COL]	= m_ulHomePatternCol;
	(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_PITCH_ROW]		= m_ulHomePitchRow;
	(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_PITCH_COL]		= m_ulHomePitchCol;
	(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_KEY_DIE_ROW]	= m_ulHomeKeyDieRow;
	(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_KEY_DIE_COL]	= m_ulHomeKeyDieCol;

	if (CMS896AStn::m_bEnableWaferSizeSelect == TRUE)
	{
		for (i=0; i<WT_WAFER_LIMIT_TEMPLATE_NO; i++)
		{
			szTemp.Format("Node_%d", i+1);

			for (j=0; j<WT_MAX_POLYGON_PT; j++)
			{
				szX.Format("X_%d", j+1);
				szY.Format("Y_%d", j+1);
				
				(*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][szX] = m_oWaferTableLimitTmp[i].lWaferPolyLimitX[j];
				(*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][szY] = m_oWaferTableLimitTmp[i].lWaferPolyLimitY[j];
			}

			(*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][WT_POLYGON_LIMIT_NO] = m_oWaferTableLimitTmp[i].lWaferPolyLimitNo;
			(*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][WT_WAFER_LIMIT_TYPE] = m_oWaferTableLimitTmp[i].ucWaferLimitType;
			(*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][szTemp][WT_TEMPLATE_SETUP_COMPLETE] = m_oWaferTableLimitTmp[i].bIsSetupComplete;
		}
	}

	SavePrescanOptionFromWFT(psmf);
// Temperature Controller
	TC_SaveData(psmf);

	(*psmf)[WT_MES_CONNECTION][WT_MES_WAFER_ID_CHECK_STATUS] = m_bMESWaferIDCheckOk;
	(*psmf)[WT_MES_CONNECTION][WT_MES_WAFER_ID_CHECK_MSG] =	m_szMESWaferIDCheckMsg;

	m_bIsCircleLimit = TRUE;
	if ( m_ucWaferLimitType != WT_CIRCLE_LIMIT )
	{
		m_bIsCircleLimit = FALSE;
	}

	//Check Load/Save Data
    pUtl->UpdateWTConfig() ;

	 // close config file
    pUtl->CloseWTConfig();

	//Update Wafer Diameter on HMI (mil)
	m_dWaferDiameter = ((DOUBLE)GetWaferDiameter() * m_dXYRes) / 25.4;

	//Update current working angle;
	m_lWaferCurrentWorkingAngle = m_lWaferWorkingAngle;

    return TRUE;
}

BOOL CWaferTable::CalculateRotateWaferTheta(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree, LONG lCalibX, LONG lCalibY)
{
	DOUBLE	dRadAngle = 0.0;
	DOUBLE	dX, dY, dRadius, dNewTheta=0, dCurTheta=0;	//Klocwork

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
        dX = fabs(dY / dRadius);
	}

	if((*lX <= lCalibX) )
	{
		if( (*lY <= lCalibY) )
			dNewTheta = asin(dX);
		else
			dNewTheta = 2.0*PI - asin(dX);
	}
	else
	{
		if( (*lY <= lCalibY))
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
	*lTheta	= -m_lThetaMotorDirection * (LONG)(dDegree/m_dThetaRes);
	
	return 1;
}

BOOL CWaferTable::RotateWaferTheta(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree)
{
	//Convert PR die angle to Radian
	LONG lCalibX = 0, lCalibY = 0;
	if( IsWT2InUse() )
	{
		lCalibX = m_lWaferCalibX2;
		lCalibY = m_lWaferCalibY2;
	}
	else
	{
		lCalibX = m_lWaferCalibX;
		lCalibY = m_lWaferCalibY;
	}

	CalculateRotateWaferTheta(lX, lY, lTheta, dDegree, lCalibX, lCalibY);

	/*
	dX = (DOUBLE)(*lX - lCalibX);
	dY = (DOUBLE)(*lY - lCalibY);

	dRadius = sqrt((dX * dX) + (dY * dY));
	
	if (dRadius == 0.0)
	{
		dX = 0.0;
	}
	else
	{
        dX = fabs(dY / dRadius);
	}

	if((*lX <= lCalibX) )
	{
		if( (*lY <= lCalibY) )
			dNewTheta = asin(dX);
		else
			dNewTheta = 2.0*PI - asin(dX);
	}
	else
	{
		if( (*lY <= lCalibY))
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
	*lTheta	= -m_lThetaMotorDirection * (LONG)(dDegree/m_dThetaRes);
	*/

	return 1;
}

BOOL CWaferTable::RotateWaferThetaAngle(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree, BOOL bIsWT2)
{
	
	LONG lCalibX = 0, lCalibY = 0;
	
	if( bIsWT2)
	{
		lCalibX = m_lWaferCalibX2;
		lCalibY = m_lWaferCalibY2;
	}
	else
	{
		lCalibX = m_lWaferCalibX;
		lCalibY = m_lWaferCalibY;
	}
	
	CalculateRotateWaferTheta(lX, lY, lTheta, dDegree, lCalibX, lCalibY);
	
	return 1;
}


LONG CWaferTable::SetWaferLimit(LONG lPoint)
{
	double	dRadius = 0.0;

	if (lPoint == 0)
	{
		m_lWaferTmpX[0] = 0;
		m_lWaferTmpY[0] = 0;
		m_lWaferTmpX[1] = 0;
		m_lWaferTmpY[1] = 0;
		m_lWaferTmpX[2] = 0;
		m_lWaferTmpY[2] = 0;

		m_bXJoystickOn = TRUE;
		m_bYJoystickOn = TRUE;

		//Enlarge joystick limit when teach wafer limit
		SetJoystickOn(FALSE);
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, WT_JOY_MAXLIMIT_NEG_X, WT_JOY_MAXLIMIT_POS_X, &m_stWTAxis_X);		
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, WT_JOY_MAXLIMIT_NEG_Y, WT_JOY_MAXLIMIT_POS_Y, &m_stWTAxis_Y);
		if ( (m_ulJoyStickMode == MS899_JS_MODE_PR) )	//v3.76
			m_bCheckWaferLimit = FALSE;	
		SetJoystickOn(TRUE);

		return TRUE;
	}
	else if (lPoint >= 4)
	{
		m_lWaferCenterX = (LONG)((double)(m_lWaferTmpX[1] + m_lWaferTmpX[2]) / 2.0 + 0.5);
		m_lWaferCenterY = (LONG)((double)(m_lWaferTmpY[1] + m_lWaferTmpY[0]) / 2.0 + 0.5);

		dRadius = sqrt(((double)abs(m_lWaferCenterY - m_lWaferTmpY[0]) * (double)abs(m_lWaferCenterY - m_lWaferTmpY[0]))
					+  ((double)abs(m_lWaferCenterX - m_lWaferTmpX[0]) * (double)abs(m_lWaferCenterX - m_lWaferTmpX[0])));

		m_lWaferSize = (LONG)(dRadius*2);

		//Move to new center & update joystick limit
		SetJoystickOn(FALSE);

#ifdef NU_MOTION
		X1_Profile(LOW_PROF);
		Y1_Profile(LOW_PROF);
#endif

		XY1_MoveTo(GetWft1CenterX(), GetWft1CenterY(), SFM_WAIT);	

#ifdef NU_MOTION
		X1_Profile(NORMAL_PROF);
		Y1_Profile(NORMAL_PROF);
#endif

		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);		

		if ( (m_ulJoyStickMode == MS899_JS_MODE_PR) )	//v3.76
			m_bCheckWaferLimit = TRUE;	
		SetJoystickOn(TRUE);
		m_bXJoystickOn = TRUE;
		m_bYJoystickOn = TRUE;

		if (CMS896AStn::m_bEnableWaferSizeSelect == TRUE)
		{
			if (m_ucWaferLimitTemplateNoSel< WT_WAFER_LIMIT_TEMPLATE_NO)
			{
				//update for template
				m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].ucWaferLimitType = WT_CIRCLE_LIMIT;
				m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].lWaferPolyLimitNo = MS_WAFER_CIRCULAR_LIMIT_PTS;
				m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].bIsSetupComplete = TRUE;

				for (INT i=0; i<MS_WAFER_CIRCULAR_LIMIT_PTS; i++)
				{
					m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].lWaferPolyLimitX[i] = m_lWaferTmpX[i];
					m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].lWaferPolyLimitY[i] = m_lWaferTmpY[i];	
				}
			}
		}

		SaveWaferTblData();
	}
	else
	{
		GetEncoderValue();
		m_lWaferTmpX[lPoint-1] = GetCurrX1();
		m_lWaferTmpY[lPoint-1] = GetCurrY1();

		m_bXJoystickOn = TRUE;
		m_bYJoystickOn = TRUE;

		if (lPoint == 1)
		{
			m_bXJoystickOn = FALSE;
			m_bYJoystickOn = TRUE;
			X_SetJoystickOn(FALSE);
			Y_SetJoystickOn(TRUE);
		}
		else if (lPoint == 2)
		{
			m_bXJoystickOn = TRUE;
			m_bYJoystickOn = FALSE;
			X_SetJoystickOn(TRUE);
			Y_SetJoystickOn(FALSE);
		}
	}

	return TRUE;
}


LONG CWaferTable::MoveWaferLimit(VOID)
{
	DOUBLE	dCounter = 0.0;			
	LONG	lXmove, lYmove;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	lXmove = 0;
	lYmove = 0;

#ifdef NU_MOTION
	if (m_bEnableWTMotionLog)
	{
		LogAxisPerformance3(WT_AXIS_X, WT_AXIS_Y, WT_AXIS_T, &m_stWTAxis_X, &m_stWTAxis_Y, &m_stWTAxis_T, TRUE);
	}
#endif

	X_Profile(LOW_PROF);	//v4.47T8
	Y_Profile(LOW_PROF);	//v4.47T8

	while (dCounter < 6.28)	
	{			
		lXmove = (LONG)(sin(dCounter) * (DOUBLE)GetWaferDiameter() / 2 + (DOUBLE)GetWft1CenterX());
		lYmove = (LONG)(cos(dCounter) * (DOUBLE)GetWaferDiameter() / 2 + (DOUBLE)GetWft1CenterY());


		if (IsMS90())	//v4.50A2
		{
			if ( (lXmove <= m_lXPosLimit) && (lXmove >= m_lXNegLimit) )
			{
				X1_MoveTo(lXmove, SFM_NOWAIT);
			}

			if ( (lYmove <= m_lYPosLimit) && (lYmove >= m_lYNegLimit) )
			{
				Y1_MoveTo(lYmove, SFM_NOWAIT);
			}

			X1_Sync();
			Y1_Sync();
		}
		else
		{
			XY1_MoveTo(lXmove, lYmove, SFM_WAIT);		// Move table
		}

#ifdef NU_MOTION
		Sleep(35);
#else
		Sleep(10);
		if( pApp->GetCustomerName()=="OSRAM" )
		{
			Sleep(50);
		}
#endif
		dCounter += 0.04;						// Approx 80 segments
	}

	XY1_MoveTo(GetWft1CenterX(), GetWft1CenterY(), SFM_WAIT);	

	X_Profile(NORMAL_PROF);		//v4.47T8
	Y_Profile(NORMAL_PROF);		//v4.47T8


#ifdef NU_MOTION
	if (m_bEnableWTMotionLog)
	{
		LogAxisPerformance3(WT_AXIS_X, WT_AXIS_Y, WT_AXIS_T, &m_stWTAxis_X, &m_stWTAxis_Y, &m_stWTAxis_T, FALSE);
	}
#endif

	return TRUE;
}


LONG CWaferTable::SetPolygonWaferLimit(VOID)
{
	LONG i;
	CString szText;
	LONG lMaxX,lMaxY;
	LONG lMinX,lMinY;
	DOUBLE dRadius;
	CString szTitle, szContent;


	//Enlarge joystick limit when teach wafer limit
	SetJoystickOn(FALSE);
	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, WT_JOY_MAXLIMIT_NEG_X, WT_JOY_MAXLIMIT_POS_X, &m_stWTAxis_X);		
	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, WT_JOY_MAXLIMIT_NEG_Y, WT_JOY_MAXLIMIT_POS_Y, &m_stWTAxis_Y);
	if ( (m_ulJoyStickMode == MS899_JS_MODE_PR) )	//v3.76
		m_bCheckWaferLimit = FALSE;	
	SetJoystickOn(TRUE);


	szTitle.LoadString(HMB_WT_POLYGON_LIMIT);


	//Get polygon point
	szContent.LoadString(HMB_WT_SET_POLYGON_LMT_1);

	HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	for (i=0; i<m_lWaferPolyLimitNo; i++)
	{
		szText.Format("Please move to Point %d", i+1);
		HmiMessageEx(szText, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

		GetEncoderValue();
		m_lWaferPolyLimitX[i] = GetCurrX1();
		m_lWaferPolyLimitY[i] = GetCurrY1();
	}

	//Check polygon
	if ( IsConvexPolygon() == FALSE )
	{
		szContent.LoadString(HMB_WT_SET_POLYGON_FAILED);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	//Calculate wafer center pos & diameter
	lMaxX = m_lWaferPolyLimitX[0];
	lMaxY = m_lWaferPolyLimitY[0];
	lMinX = m_lWaferPolyLimitX[0];
	lMinY = m_lWaferPolyLimitY[0];

	for (i=1; i<m_lWaferPolyLimitNo; i++)
	{
		lMaxX = max(lMaxX, m_lWaferPolyLimitX[i]);
		lMaxY = max(lMaxY, m_lWaferPolyLimitY[i]);
		lMinX = min(lMinX, m_lWaferPolyLimitX[i]);
		lMinY = min(lMinY, m_lWaferPolyLimitY[i]);
	}

	if (CMS896AStn::m_bEnableWaferSizeSelect == TRUE)
	{
		if (m_ucWaferLimitTemplateNoSel< WT_WAFER_LIMIT_TEMPLATE_NO)
		{
			//update for template
			m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].ucWaferLimitType = WT_POLYGON_LIMIT;
			m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].lWaferPolyLimitNo = m_lWaferPolyLimitNo;
			m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].bIsSetupComplete = TRUE;
			
			for (i=0; i<(INT)m_lWaferPolyLimitNo; i++)
			{
				m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].lWaferPolyLimitX[i] = m_lWaferPolyLimitX[i];
				m_oWaferTableLimitTmp[m_ucWaferLimitTemplateNoSel].lWaferPolyLimitY[i] = m_lWaferPolyLimitY[i];	
			}
		}
	}

	m_lWaferCenterX = (LONG)((double)(lMinX + lMaxX) / 2.0 + 0.5);
	m_lWaferCenterY = (LONG)((double)(lMinY + lMaxY) / 2.0 + 0.5);

	dRadius = (DOUBLE)(max(abs(m_lWaferCenterY - lMaxY), abs(m_lWaferCenterX - lMaxX)));
	m_lWaferSize = (LONG)(dRadius*2);

	SaveWaferTblData();

	//Move to new center & update joystick limit
	SetJoystickOn(FALSE);

	XY1_MoveTo(GetWft1CenterX(), GetWft1CenterY(), SFM_WAIT);	

	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);		

	if ( (m_ulJoyStickMode == MS899_JS_MODE_PR) )	//v3.76
		m_bCheckWaferLimit = TRUE;	
	SetJoystickOn(TRUE);

	return TRUE;
}

LONG CWaferTable::MovePolygonWaferLimit(VOID)
{
	LONG i;

	for (i=0; i<m_lWaferPolyLimitNo; i++)
	{
		if (IsMS90())	//v4.50A2
		{
			if ( (m_lWaferPolyLimitX[i] <= m_lXPosLimit) && (m_lWaferPolyLimitX[i] >= m_lXNegLimit) )
			{
				X1_MoveTo(m_lWaferPolyLimitX[i], SFM_NOWAIT);
			}

			if ( (m_lWaferPolyLimitY[i] <= m_lYPosLimit) && (m_lWaferPolyLimitY[i] >= m_lYNegLimit) )
			{
				Y1_MoveTo(m_lWaferPolyLimitY[i], SFM_NOWAIT);
			}

			X1_Sync();
			Y1_Sync();
		}
		else
		{
			XY1_MoveTo(m_lWaferPolyLimitX[i], m_lWaferPolyLimitY[i], SFM_WAIT);
		}
		Sleep(500);
	}

	XY1_MoveTo(m_lWaferPolyLimitX[0], m_lWaferPolyLimitY[0], SFM_WAIT);	
	Sleep(500);

	XY1_MoveTo(GetWft1CenterX(), GetWft1CenterY(), SFM_WAIT);	
	return TRUE;
}

LONG CWaferTable::OnSelectWaferLimit(UCHAR ucTemplateNo)
{
	LONG i;
	DOUBLE dRadius = 0.0;
	LONG lMaxX,lMaxY;
	LONG lMinX,lMinY;

	if (IsMS90())	//v4.49A3
	{
		return FALSE;
	}

	if (ucTemplateNo > WT_WAFER_LIMIT_TEMPLATE_NO)
	{
		return FALSE;
	}

	ucTemplateNo = min(ucTemplateNo, WT_WAFER_LIMIT_TEMPLATE_NO-1);		//v4.19	//Klocwork
	if (m_oWaferTableLimitTmp[ucTemplateNo].bIsSetupComplete == FALSE)
	{
		return FALSE;
	}

	if (m_oWaferTableLimitTmp[ucTemplateNo].ucWaferLimitType == WT_CIRCLE_LIMIT)
	{
		m_ucWaferLimitType = m_oWaferTableLimitTmp[ucTemplateNo].ucWaferLimitType;

		for (INT i=0; i<MS_WAFER_CIRCULAR_LIMIT_PTS; i++)
		{
			m_lWaferTmpX[i] = m_oWaferTableLimitTmp[ucTemplateNo].lWaferPolyLimitX[i];
			m_lWaferTmpY[i] = m_oWaferTableLimitTmp[ucTemplateNo].lWaferPolyLimitY[i];	
		}

		m_lWaferCenterX = (LONG)((double)(m_lWaferTmpX[1] + m_lWaferTmpX[2]) / 2.0 + 0.5);
		m_lWaferCenterY = (LONG)((double)(m_lWaferTmpY[1] + m_lWaferTmpY[0]) / 2.0 + 0.5);

		dRadius = sqrt(((double)abs(m_lWaferCenterY - m_lWaferTmpY[0]) * (double)abs(m_lWaferCenterY - m_lWaferTmpY[0]))
					+  ((double)abs(m_lWaferCenterX - m_lWaferTmpX[0]) * (double)abs(m_lWaferCenterX - m_lWaferTmpX[0])));

		m_lWaferSize = (LONG)(dRadius*2);

		if( IsES101()==FALSE )	// 4.24TX1
		{
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);		
		}
	}
	else
	{
		m_ucWaferLimitType = m_oWaferTableLimitTmp[ucTemplateNo].ucWaferLimitType;
		m_lWaferPolyLimitNo = m_oWaferTableLimitTmp[ucTemplateNo].lWaferPolyLimitNo;

		for (i=0; i<m_lWaferPolyLimitNo; i++)
		{
			m_lWaferPolyLimitX[i] = m_oWaferTableLimitTmp[ucTemplateNo].lWaferPolyLimitX[i];
			m_lWaferPolyLimitY[i] = m_oWaferTableLimitTmp[ucTemplateNo].lWaferPolyLimitY[i];	
		}

		//Calculate wafer center pos & diameter
		lMaxX = m_lWaferPolyLimitX[0];
		lMaxY = m_lWaferPolyLimitY[0];
		lMinX = m_lWaferPolyLimitX[0];
		lMinY = m_lWaferPolyLimitY[0];

		for (i=1; i<m_lWaferPolyLimitNo; i++)
		{
			lMaxX = max(lMaxX, m_lWaferPolyLimitX[i]);
			lMaxY = max(lMaxY, m_lWaferPolyLimitY[i]);
			lMinX = min(lMinX, m_lWaferPolyLimitX[i]);
			lMinY = min(lMinY, m_lWaferPolyLimitY[i]);
		}

		m_lWaferCenterX = (LONG)((double)(lMinX + lMaxX) / 2.0 + 0.5);
		m_lWaferCenterY = (LONG)((double)(lMinY + lMaxY) / 2.0 + 0.5);

		dRadius = (DOUBLE)(max(abs(m_lWaferCenterY - lMaxY), abs(m_lWaferCenterX - lMaxX)));
		m_lWaferSize = (LONG)(dRadius*2);

		if( IsES101()==FALSE )	// 4.24TX1
		{
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);
		}
	}

	//Update Wafer Diameter on HMI (mil)
	m_dWaferDiameter = ((DOUBLE)GetWaferDiameter() * m_dXYRes) / 25.4;

	return TRUE;
}


LONG CWaferTable::StartWaferCalibration(VOID)
{
	//Off joystick before move
	SetJoystickOn(FALSE);

	//Move to Wafer Center position
	T1_MoveTo(0,SFM_WAIT);
	
	HomeTable2();

	XY1_MoveTo(m_lWaferCalibX, m_lWaferCalibY);
	
	//On joystick
	m_bCheckWaferLimit = FALSE;	
	SetJoystickOn(TRUE);

	m_bStartCalibrate = TRUE;

	if( IsBLInUse() )	//	4.24TX 3
	{
		Sleep(100);
		if( IsWT1UnderCamera() )
		{
			m_lBackLightZStatus = 0;	//	4.24TX 4
			MoveES101BackLightZUpDn(TRUE);	// in prestart, if align ok, check current is safe, UP BL Z
			MoveFocusToWafer(FALSE);
		}
	}

	return TRUE;
}

LONG CWaferTable::StartWaferCalibration2(VOID)
{
	//Off joystick before move
	SetJoystickOn(FALSE);

	//Move to Wafer Center position
	T2_MoveTo(0,SFM_WAIT);
	
	HomeTable1();

	XY2_MoveTo(m_lWaferCalibX2, m_lWaferCalibY2);
	if( IsESDualWT() )	//	4.24TX 3
	{
		Sleep(100);
		if( IsWT2UnderCamera() )
		{
			m_lBackLightZStatus = 0;	//	4.24TX 4
			MoveES101BackLightZUpDn(TRUE);	// in prestart, if align ok, check current is safe, UP BL Z
			MoveFocusToWafer(TRUE);
		}
	}

		
	m_bCheckWaferLimit = FALSE;	
	//On joystick
	SetJoystickOn(TRUE, TRUE);

	m_bStartCalibrate2 = TRUE;

	return TRUE;
}

LONG CWaferTable::MoveWaferTheta(VOID)
{
	T1_Profile(LOW_PROF);	

	T1_MoveTo((INT)(-m_lRotateCount),SFM_WAIT);
	Sleep(500);
	T1_MoveTo((INT)(m_lRotateCount),SFM_WAIT);
	Sleep(500);

	T1_Profile(NORMAL_PROF);

	return TRUE;
}

LONG CWaferTable::MoveWafer2Theta(VOID)
{
	T2_Profile(LOW_PROF);	

	T2_MoveTo((INT)(-m_lRotateCount),SFM_WAIT);
	Sleep(500);
	T2_MoveTo((INT)(m_lRotateCount),SFM_WAIT);
	Sleep(500);

	T2_Profile(NORMAL_PROF);

	return TRUE;
}

LONG CWaferTable::StopWaferCalibration(LONG lStop)
{
	m_bStartCalibrate = FALSE;
	T1_MoveTo(0, SFM_WAIT);

	if( IsBLInUse() )	//	4.24TX 3
	{
		m_lBackLightZStatus = 2;	//	4.24TX 4
		MoveES101BackLightZUpDn(FALSE);	// in prestart, if align ok, check current is safe, UP BL Z
		Sleep(100);
	}

	if (lStop == 1)
	{
		GetEncoderValue();
		m_lWaferCalibX = GetCurrX1();
		m_lWaferCalibY = GetCurrY1();

		if (IsMS90())	//v4.50A7
		{
			CMS896AStn::m_lWaferCenterX	= m_lWaferCalibX;
			CMS896AStn::m_lWaferCenterY = m_lWaferCalibY;
		}

		SaveWaferTblData();

		m_bCheckWaferLimit = TRUE;	
		SetJoystickOn(FALSE);

		HomeTable1();
	}

	return TRUE;
}

LONG CWaferTable::StopWaferCalibration2(LONG lStop)
{
	m_bStartCalibrate2 = FALSE;
	T2_MoveTo(0,SFM_WAIT);

	if( IsESDualWT() )	//	4.24TX 3
	{
		m_lBackLightZStatus = 2;	//	4.24TX 4
		MoveES101BackLightZUpDn(FALSE);	// in prestart, if align ok, check current is safe, UP BL Z
		Sleep(100);
	}

	if (lStop == 1)
	{
		GetEncoderValue();
		m_lWaferCalibX2 = GetCurrX2();
		m_lWaferCalibY2 = GetCurrY2();
		SaveWaferTblData();

		HomeTable2();

		m_bCheckWaferLimit = TRUE;	
		SetJoystickOn(FALSE, FALSE);
	}

	return TRUE;
}

VOID CWaferTable::InitLFInfo(LF_INFO& stInfo)	// Init Look-Forward Info
{
	stInfo.lDirection	= -1;
    stInfo.bRotate		= FALSE;
    stInfo.bFullDie		= FALSE;
	stInfo.bGoodDie		= FALSE;
	stInfo.lX			= 0;
	stInfo.lY			= 0;
	stInfo.dTheta		= 0.0;
	stInfo.lWM_X		= 0;
	stInfo.lWM_Y		= 0;
}

BOOL CWaferTable::IsWithinMapDieLimit(LONG lX, LONG lY, DOUBLE dScale)
{
	if( m_lFindMapEdgeDieLimit<=0 || m_bSearchWaferLimitDone==FALSE )
	{
		return TRUE;
	}

	if (!m_bSel_X && !m_bSel_Y)		//v4.36T3
	{
		return TRUE;
	}

	if ( m_ucWaferLimitType == WT_CIRCLE_LIMIT )
	{
		DOUBLE dDeltaX = 0, dDeltaY = 0, dDist = 0;
		dDeltaX = (double)abs(lX - m_lAutoWaferCenterX);
		dDeltaY	= (double)abs(lY - m_lAutoWaferCenterY);
		dDist	= (LONG)sqrt((dDeltaX*dDeltaX) + (dDeltaY*dDeltaY));
		LONG lWaferRadius = m_lAutoWaferDiameter/2;
		if( dDist > ((DOUBLE)lWaferRadius*dScale) )
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return TRUE;
	}
}

BOOL CWaferTable::IsWithinWaferLimit(LONG lX, LONG lY, DOUBLE dScale, BOOL bUsePosOffset)
{
	if (IsES101() ||  IsES201() )		//v4.28
	{
		if (IsWT2InUse())		//WT2
			return IsWithinWT2WaferLimit(lX, lY, dScale, bUsePosOffset);
		else					//WT1
			return IsWithinWT1WaferLimit(lX, lY, dScale, bUsePosOffset);
	}
	else
	{
		if (!m_bSel_X && !m_bSel_Y)		//v4.36T3
			return TRUE;

		if (IsMS90())					//v4.49A2
		{
			if ( (lX > m_lXPosLimit) || (lX < m_lXNegLimit) )
			{
				CString szErr;
				szErr.Format("WT: IsWithinWaferLimit(MS90) - X is out of range; XEnc=%ld (%ld, %ld)",
					lX, m_lXNegLimit, m_lXPosLimit);
			//	SetErrorMessage(szErr);
				return FALSE;
			}

			if ( (lY > m_lYPosLimit) || (lY < m_lYNegLimit) )
			{
				CString szErr;
				szErr.Format("WT: IsWithinWaferLimit(MS90) - Y is out of range; YEnc=%ld (%ld, %ld)",
					lY, m_lYNegLimit, m_lYPosLimit);
			//	SetErrorMessage(szErr);
				return FALSE;
			}
		}

		return IsWithinInputWaferLimit(lX, lY, dScale);
	}

} //end IsWithinWaferLimit


LONG CWaferTable::GetCircleWaferMinX()
{
	return GetWaferCenterX() - (GetWaferDiameter() / 2);
}

LONG CWaferTable::GetCircleWaferMaxX()
{
	return GetWaferCenterX() + (GetWaferDiameter() / 2);
}

LONG CWaferTable::GetCircleWaferMinY()
{
	return GetWaferCenterY() - (GetWaferDiameter() / 2);
}

LONG CWaferTable::GetCircleWaferMaxY()
{
	return GetWaferCenterY() + (GetWaferDiameter() / 2);
}

VOID CWaferTable::ScnLoaded(BOOL bLoaded)
{
	m_bScnLoaded = bLoaded;
	if( bLoaded )
	{
		CString szLog;
		szLog.Format("Align scanner-like Wafer: SCN=%d", 1);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}
}

VOID CWaferTable::SetBlkFuncEnable(BOOL bBlkFunc)
{
// prescan relative code
	m_bPrescanBlkPickEnabled = bBlkFunc;	// if block pick algorithm enable, then this true, otherwise, false.
	m_bBlkFuncEnable = bBlkFunc;	// enable by map algorithm
}

VOID CWaferTable::BlkGoToPosition()
{
	ULONG ulRow = 0, ulCol = 0;

	if (m_bMnPositionSwitch == FALSE)
	{
		 return;
	}

	m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);
	ConvertAsmToOrgUser(ulRow, ulCol, m_lMnAlignCheckRow, m_lMnAlignCheckCol);

	SetJoystickOn(FALSE);

	if (IsBlkFunc2Enable())	//Block2
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
	else if( IsBlkFunc1Enable() )
	{
		m_pBlkFunc->GotoTargetDie(m_lMnAlignCheckRow, m_lMnAlignCheckCol);
	}
	SetJoystickOn(TRUE);
}

CString CWaferTable::GetWaferMapFormat()
{	
	return m_szWaferMapFormat;
} //end GetWaferMapFormat

VOID CWaferTable::SetWaferMapFormat(CString szWaferMapFormat)
{
	m_szWaferMapFormat = szWaferMapFormat;
	SaveData();
} //end 


VOID CWaferTable::SetEnableSmartWalk(CONST BOOL bEnable)
{
	m_bEnableSmartWalk = bEnable;
}


BOOL CWaferTable::CheckNichiaCriteraInMapLoaded()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
		return TRUE;
	if (pApp->GetProductLine() != "")	//v4.59A34
	{
		return TRUE;
	}

	CString szLog;

	//1. Check MapHeader Carrier No with PRM name
	CString szPkgFileName = (*m_psmfSRam)["MS896A"]["PKG Filename"];
	if( szPkgFileName.IsEmpty() )
	{
		szLog.Format("ERROR: current Machine PRM (%s) is not EXIST.", szPkgFileName);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
		SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szLog, "CLOSE");
		return FALSE;
	}
/*
	if (szPkgFileName.Find(".ppkg") == -1)
	{
		szLog.Format("ERROR: current Machine PRM (%s) is not with PPKG extension.", szPkgFileName);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
		SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szLog, "CLOSE");
		return FALSE;
	}
	else
	{
*/
	CString szMachinePRMName	= szPkgFileName;
	int nCol = szPkgFileName.Find('.');
	if( nCol!=-1 )
	{
		szMachinePRMName	= szPkgFileName.Left(nCol);
	}

	CString szLotNo, szLotValue;
	szLotNo.Format("%s", ucaMapHeaderCarrier);
	CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(szLotNo, szLotValue);

	szLog = "Nichia Map LotNo Check: machine PRM = " + szMachinePRMName + "; Map LotNo = " + szLotValue;
	CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);

	if (szMachinePRMName != szLotValue)	//Nichia//v4.43T7
	{
		szLog.Format("Machine PRM (%s) not matched; current Map LotNo = (%s)", 
						szMachinePRMName, szLotValue);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
	
		//SetErrorMessage(szLog);
		//HmiMessage_Red_Yellow(szLog);

		//54321
		BOOL bReply = pApp->RestoreNichiaPRM(szLotValue);
		if (!bReply)	//If not all task in IDLE state, do not load frame with barcode! (SanAn)		
		{
			szLog.Format("Nichia: fails to load PRM file (%s)\nin LoadMap: " + GetMapFileName(), 
				(LPCTSTR) szLotValue);
			SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szLog, "CLOSE");
			return FALSE;
		}
	}
	else
	{
			//Nichia//v4.43T7
		szLog.Format("Current Machine PRM (%s) is matched with LotNo = (%s)", szMachinePRMName, szLotValue);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
	}

	//v4.40T10
	//Nichia Regulatory Code
	CString szMapRegCode, szMapRegCodeInfo;
	szMapRegCodeInfo.Format("%s", ucaMapHeaderRegCode);
	CString szCurrCode = CMS896AStn::m_oNichiaSubSystem.GetRegulatoryCode();
	CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(szMapRegCodeInfo, szMapRegCode);
//AfxMessageBox("Regulatory Code = " + szMapRegCode, MB_SYSTEMMODAL);
	if (pApp->CheckIsAllBinClearedNoMsg() == TRUE)
	{
		CMS896AStn::m_oNichiaSubSystem.SetRegulatoryCode(szMapRegCode);
		szLog.Format("Nichia: map Regulatory Code updated to (%s); previous (%s)", 
			(LPCTSTR) szMapRegCode, (LPCTSTR) szCurrCode);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
	}
	else
	{
		if (szCurrCode.GetLength() == 0)
		{
			CMS896AStn::m_oNichiaSubSystem.SetRegulatoryCode(szMapRegCode);
			szLog.Format("Nichia: map Regulatory Code updated (%s)", (LPCTSTR) szMapRegCode);
			CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
		}
		else if (szMapRegCode != szCurrCode)
		{
			szLog.Format("Nichia: map Regulatory code (%s) not matched with previous one (%s)", 
				(LPCTSTR) szMapRegCode, (LPCTSTR) szCurrCode);
			SetErrorMessage(szLog);
			
			//nichia003
			//HmiMessage_Red_Yellow(szLog);
			szLog.Format("Nichia: map Regulatory code \n(%s) \nnot matched with previous one \n(%s)", 
				(LPCTSTR) szMapRegCode, (LPCTSTR) szCurrCode);
			SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szLog, "CLOSE");
			return FALSE;
		}
	}


	//v4.41T4
	//Nichia Base Point Info for wafer alignment purpose
	CString szMapBasePtInfo, szBasePoint;
	szMapBasePtInfo.Format("%s", ucaMapHeaderBasePtInfo);
	CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(szMapBasePtInfo, szBasePoint);
	CMS896AStn::m_oNichiaSubSystem.SetMapBasePt(szBasePoint);
	
	//v4.42T2	//Moved to CheckNichiaArrCodeInMap() in BT_Common.cpp
	/*//anichia005
	CString szMapArrCodeInfo, szArrCode;
	szMapArrCodeInfo.Format("%s", ucaMapArrCode);
	CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(szMapArrCodeInfo, szArrCode);
	CMS896AStn::m_oNichiaSubSystem.UpdateMapArrCode(szArrCode);
	*/

	//FInd & search TEG patterns from Nichia map file for auto wafer alignment purpose
	ULONG ulAsmRow=0, ulAsmCol=0;
	BOOL bScanAlignFromFile = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["ScanAlignFromFile"]; 
	if (bScanAlignFromFile)
	{
		//Delete WaferHole.txt file before creation below
		CString szFileName = gszROOT_DIRECTORY + "\\Exe\\WaferHole.txt";
		DeleteFile(szFileName);

		BOOL bPattern1=FALSE, bPattern2=FALSE, bPattern3=FALSE;
		UCHAR ucWaferType	= CMS896AStn::m_oNichiaSubSystem.m_ucWaferType;
		UCHAR ucTEGType		= CMS896AStn::m_oNichiaSubSystem.m_ucWaferTEGType;
		ULONG ulRow=0,  ulCol=0;
		ULONG ulRow2=0, ulCol2=0;
		ULONG ulRow3=0, ulCol3=0;
		LONG lColWidth = 0;

		CString szBasePt = CMS896AStn::m_oNichiaSubSystem.GetMapBasePt();

		if (szBasePt == "C")
		{
//AfxMessageBox("Type C: ....", MB_SYSTEMMODAL);
			bPattern1 = FindNichiaTEGPatternFromMap(ulRow,  ulCol,  1, TRUE,  TRUE);
			bPattern2 = FindNichiaTEGPatternFromMap(ulRow2, ulCol2, 2, FALSE, TRUE);
			bPattern3 = FindNichiaTEGPatternFromMap(ulRow3, ulCol3, 3, FALSE, TRUE);

			lColWidth = 0;
			if (bPattern1 && bPattern2 && bPattern3)
			{
				//if all 3 TEG pattern exists, use CENTER one
				ulAsmRow = ulRow2;
				ulAsmCol = ulCol2-1;
			}
			else
			{
				ulAsmRow = ulRow;
				ulAsmCol = ulCol-1;
			}

			if (!bPattern2)
			{
				bPattern2 = TRUE;
			}
			if (!bPattern3)
			{
				bPattern3 = TRUE;
			}
		}
		//else if ( (szBasePt == "B") || (szBasePt == "C") || (szBasePt == "D") )
		else if ( (szBasePt == "B") || (szBasePt == "D") )
		{
			bPattern1 = FindNichiaTEGPatternFromMap(ulRow, ulCol, 1, TRUE, TRUE);
			bPattern2 = TRUE;
			bPattern3 = TRUE;

			if (ucWaferType == 0)			//800x300
			{
				if (szBasePt == "B")			//Left TEG only
					lColWidth = 84;
				else if (szBasePt == "C")		//Center TEG only
					lColWidth = 0;
				else							//Right TEG only
					lColWidth = 84;
			}
			else if (ucWaferType == 1)		//800x600
			{
				if (szBasePt == "B")			//Left TEG only
					lColWidth = 42;
				else if (szBasePt == "C")		//Center TEG only
					lColWidth = 0;
				else							//Right TEG only
					lColWidth = 42;
			}
			else							//600x600
			{
				lColWidth = 0;
			}

			ulAsmRow = ulRow;
			ulAsmCol = ulCol-1;
		}
		else if ( (szBasePt == "E") ||		
				  (szBasePt == "F"))
		{
			bPattern1 = FindNichiaTEGPatternFromMap(ulRow,  ulCol,  1, TRUE);
			bPattern2 = FindNichiaTEGPatternFromMap(ulRow2, ulCol2, 2);
			bPattern3 = TRUE;

			//lColWidth = 0;
			//if (szBasePt == "E")
			//	lColWidth = -1 * labs(ulCol2 - ulCol);
			//else
			lColWidth = labs(ulCol2 - ulCol);

			if (szBasePt == "E")
			{
				ulAsmRow = ulRow2;
				ulAsmCol = ulCol2-1;
			}
			else
			{
				ulAsmRow = ulRow;
				ulAsmCol = ulCol-1;
			}
		}
		else
		{
			bPattern1 = FindNichiaTEGPatternFromMap(ulRow, ulCol, 1, TRUE);
			if (bPattern1)
			{
				bPattern2 = FindNichiaTEGPatternFromMap(ulRow2, ulCol2, 2);
				bPattern3 = FindNichiaTEGPatternFromMap(ulRow3, ulCol3, 3);
			}

			lColWidth = labs(ulCol2 - ulCol);
			ulAsmRow = ulRow2;
			ulAsmCol = ulCol2-1;
		}

		if (!bPattern1 || !bPattern2 || !bPattern3)
		{
			szLog.Format("Nichia: TEG pattern is not found for WTYPE=%d, TEG=%d, %d %d %d", 
				ucWaferType, ucTEGType, bPattern1, bPattern2, bPattern3);	//v4.40T13
			SetErrorMessage(szLog);
						
			//anichia003
			szLog.Format("Nichia: TEG pattern is not found for \nWTYPE=%d, TEG=%d, %d %d %d", 
				ucWaferType, ucTEGType, bPattern1, bPattern2, bPattern3);	//v4.40T13
			SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szLog, "CLOSE");

			return FALSE;
		}

		CString szLog;
		szLog.Format("NichiaMap Type=%d, TEG=%d, BasePt=%s; Colwidth=%ld", ucWaferType, ucTEGType, 
						(LPCTSTR) szBasePt, lColWidth);
		//AfxMessageBox(szLog, MB_SYSTEMMODAL);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
		CMS896AStn::m_oNichiaSubSystem.SetMapBasePtColWidth(lColWidth);

		LONG lUserRow=0, lUserCol=0;
		ConvertAsmToOrgUser(ulAsmRow, ulAsmCol, lUserRow, lUserCol);
		szLog.Format("FindNichiaTEGRefDieOnMap: HOME die set at (%ld, %ld)", lUserRow, lUserCol);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
		m_lHomeDieMapRow = lUserRow;
		m_lHomeDieMapCol = lUserCol;

	}
	else
	{
		BOOL bCRefDieFound = FindNichiaTEGRefDieOnMap(0, ulAsmRow, ulAsmCol);
		if (bCRefDieFound)
		{
			LONG lUserRow=0, lUserCol=0;
			ConvertAsmToOrgUser(ulAsmRow, ulAsmCol, lUserRow, lUserCol);
			CString szMsg;
			szMsg.Format("FindNichiaTEGRefDieOnMap: CRef Die found at (%ld, %ld)", lUserRow, lUserCol);
			CMSLogFileUtility::Instance()->MS_LogCtmOperation(szMsg);
//AfxMessageBox(szMsg, MB_SYSTEMMODAL);
			m_lHomeDieMapRow = lUserRow;
			m_lHomeDieMapCol = lUserCol;
		}
	}

	//anichia003
	if (IsEnableFPC() && m_bScnCheckIsRefDie)
	{
		ULONG ulNoOfRows=0, ulNoOfCols=0;
		m_WaferMapWrapper.GetMapDimension(ulNoOfRows, ulNoOfCols);

		m_lTotalSCNCount = 0;
		for (INT m = 0; m < WT_ALN_MAXCHECK_SCN; m++)
		{
			m_lScnCheckRow[m] = 0;
			m_lScnCheckCol[m] = 0;
		}

		LONG lUserRow=0, lUserCol=0;
		UCHAR ucERR10Grade = 205 + m_WaferMapWrapper.GetGradeOffset();

		for (INT i = 0; i < (INT)ulNoOfRows; i++)
		{
			for (INT j = 0; j < (INT)ulNoOfCols; j++)
			{
				if ( (m_WaferMapWrapper.GetGrade(i, j) == ucERR10Grade) ||
					 m_WaferMapWrapper.IsReferenceDie(i, j) )
				{
					ConvertAsmToOrgUser(i, j, lUserRow, lUserCol);
					m_lTotalSCNCount++;

					m_lScnCheckRow[m_lTotalSCNCount] = lUserRow;
					m_lScnCheckCol[m_lTotalSCNCount] = lUserCol;

					if ( m_lTotalSCNCount >= (WT_ALN_MAXCHECK_SCN-1) )
						break;
				}
			}

			if (m_lTotalSCNCount >= WT_ALN_MAXCHECK_SCN)
				break;
		}

		if (m_lTotalSCNCount > 0)
		{
			m_lScnHmiRow = m_lScnCheckRow[1];
			m_lScnHmiCol = m_lScnCheckCol[1];
		}
		SaveData();
	}


	//Check Collet & Needle material ID before sorting
	BOOL bCollet1 = CMS896AStn::m_oNichiaSubSystem.CheckRawMaterialExpireDate(CMS896AStn::m_oNichiaSubSystem.m_szColletType);
	if (!bCollet1)
	{
		szLog = "Collet Type has expired: ID = " + CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type;
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog);
		return FALSE;
	}

	BOOL bCollet2 = CMS896AStn::m_oNichiaSubSystem.CheckRawMaterialExpireDate(CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type);
	if (!bCollet2)
	{
		szLog = "Collet2 Type has expired: ID = " + CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type;
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog);
		return FALSE;
	}

	BOOL bNeedle  = CMS896AStn::m_oNichiaSubSystem.CheckRawMaterialExpireDate(CMS896AStn::m_oNichiaSubSystem.m_szNeedleType);
	if (!bNeedle)
	{
		szLog = "Needle Type has expired: ID = " + CMS896AStn::m_oNichiaSubSystem.m_szNeedleType;
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog);
		return FALSE;
	}


	CMS896AStn::m_oNichiaSubSystem.UpdateRawMaterialList();	
	CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(5);	//MAP_ACCESS	//v4.40T11
	//v4.42T11
	//CMS896AStn::m_oNichiaSubSystem.SaveData();					//Save Machine PRM for any para updates
	return TRUE;
}

BOOL CWaferTable::SetupRenesasMapInMapLoaded()		//v4.59A15
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_RENESAS)
	{
		return TRUE;
	}

	//v4.59A13	//Renesas MS90 with SECSGEM
	if (m_WaferMapWrapper.GetReader() != NULL) 
	{
		//Grade Offset is ZERO when map is downloaded from SECSGEM through InterpretMap()
		//So, need to set again here in PreBondEvent() -> ();
		m_WaferMapWrapper.SetGradeOffset(48);

		m_WaferMapWrapper.SetGradeDescription('a', "TEG");
		m_WaferMapWrapper.SetGradeDescription('b', "TEG");
		m_WaferMapWrapper.SetGradeDescription('c', "TEG");
		m_WaferMapWrapper.SetGradeDescription('d', "TEG");
		m_WaferMapWrapper.SetGradeDescription('e', "TEG");
		m_WaferMapWrapper.SetGradeDescription('f', "TEG");
		m_WaferMapWrapper.SetGradeDescription('g', "TEG");
		m_WaferMapWrapper.SetGradeDescription('h', "TEG");
		m_WaferMapWrapper.SetGradeDescription('i', "TEG");
		m_WaferMapWrapper.SetGradeDescription('j', "TEG");
		m_WaferMapWrapper.SetGradeDescription('k', "TEG");
		m_WaferMapWrapper.SetGradeDescription('l', "TEG");
		m_WaferMapWrapper.SetGradeDescription('m', "TEG");
		m_WaferMapWrapper.SetGradeDescription('n', "TEG");
		m_WaferMapWrapper.SetGradeDescription('o', "TEG");
		m_WaferMapWrapper.SetGradeDescription('p', "TEG");
		m_WaferMapWrapper.SetGradeDescription('q', "TEG");
		m_WaferMapWrapper.SetGradeDescription('r', "TEG");
		m_WaferMapWrapper.SetGradeDescription('s', "TEG");
		m_WaferMapWrapper.SetGradeDescription('t', "TEG");
		m_WaferMapWrapper.SetGradeDescription('u', "TEG");
		m_WaferMapWrapper.SetGradeDescription('v', "TEG");
		m_WaferMapWrapper.SetGradeDescription('w', "TEG");
		m_WaferMapWrapper.SetGradeDescription('x', "TEG");
		m_WaferMapWrapper.SetGradeDescription('y', "TEG");
		m_WaferMapWrapper.SetGradeDescription('z', "TEG");
		m_WaferMapWrapper.SetGradeDescription('Z', "TEG");
		m_WaferMapWrapper.SetGradeDescription('[', "TEG");
		m_WaferMapWrapper.SetGradeDescription(']', "TEG");
		m_WaferMapWrapper.SetGradeDescription('_', "TEG");
		m_WaferMapWrapper.SetGradeDescription('^', "TEG");

		m_WaferMapWrapper.SetGradeDescription('X', "Fail");
	}

	return TRUE;
}


BOOL CWaferTable::RealignBinFrame()
{
	IPC_CServiceMessage rReqMsg;
	int	nConvID2;
	BOOL bResult = TRUE;

	BOOL bToggle = FALSE;	//Not used
	rReqMsg.InitMessage(sizeof(BOOL), &bToggle);
	nConvID2 = m_comClient.SendRequest(BIN_LOADER_STN, "RealignBinFrameCmd1", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID2, 500000)==TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID2, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	return bResult;
}


BOOL CWaferTable::IsCheckRepeatMap()
{
	return m_bCheckRepeatMap;
}

VOID CWaferTable::MapLoaded()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bMapLoadAbort = FALSE;
	BOOL b2Parts1stPartDone = m_b2Parts1stPartDone;

	m_ulLastRealignDieCount = 0;
	CString szLogFile = "C:\\MapSorter\\UserData\\History\\TwoPhasesSort.txt";
	DeleteFile(szLogFile);
	DeleteFile(gszUnpickDieInfoPath);
	CString szAllFile = gszUSER_DIRECTORY + "\\History\\WT_AllRefer.log";
	remove(szAllFile);

	if( IsScnLoaded()==FALSE )	//	SCN loaded, not to clear
		DelPrescanRunPosn();	// map file loaded, have to clear
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	m_bAoiOcrLoaded	= FALSE;
	m_bLoadedWaferScannedOnce = FALSE;

	//Rotate Wafer to ZERO Degree
	if (m_bAutoLoadWaferMap==FALSE && IsWL1ExpanderSafeToMove() && IsMS90Sorting2ndPart())
	{
		XY_SafeMoveTo(0, 0);
		INT nPos = GetAutoWaferT();
		T_MoveTo(nPos, SFM_WAIT);	// Move table Theta
		Sleep(100);
	}

	//Trigger to rotate BT to 0 degree if BT is at 180 degree//2019.04.11 bt do realign after wt roate done
	if (IsMS90HalfSortMode() && IsMS90Sorting2ndPart())
	{
		//Init 
		MS90Set1stDone(FALSE);
		RealignBinFrame();
		MS90Set1stDone(b2Parts1stPartDone);
	}

	if( pUtl->GetPrescanBarWafer() || m_lScnAlignMethod==3 )
	{
		CString szOldOcrPath	=  GetMapFileName();
		// to find ; and remove
		INT iColumn = szOldOcrPath.ReverseFind(';');
		if( iColumn!=-1 )
		{
			szOldOcrPath = szOldOcrPath.Left(iColumn);
		}
		iColumn = szOldOcrPath.ReverseFind('.');
		if ( iColumn != -1 )
		{
			szOldOcrPath = szOldOcrPath.Left(iColumn);
		}
		szOldOcrPath = szOldOcrPath + _T(".OCR");	// copy from map location to local prescanresult folder.
		CString szTgtOcrPath	= m_szPrescanLogPath + _T(".OCR");
		if (_access(szOldOcrPath, 0) != -1)
		{
			if (szOldOcrPath.CompareNoCase(szTgtOcrPath) != 0)
			{
				CopyFileWithQueue(szOldOcrPath, szTgtOcrPath, FALSE);
			}
			m_bAoiOcrLoaded = TRUE;
		}
	}

	if( pApp->GetCustomerName()=="Lumileds" && pApp->GetProductLine()=="Rebel" && (m_ucPLLMWaferAlignOption == 2) )
	{
		SetPrescanOption(TRUE);
	}	//	Lumileds Penang, after load a new map, turn on Prescan.

	// sort mode, load map, reset status
	DWTDSortLog("load a new wafer map");
	if( m_bScanExtraDieAction )
		m_WaferMapWrapper.AddEmptySpace(10);
	MS90HalfSortMapAction(TRUE);	// map loaded

	if( pUtl->GetPrescanDummyMap() && IsMS90Sorting2ndPart() )
	{
	}
	else
	{
		m_bSortGoingTo2ndPart		= FALSE;	// map loaded
		m_b2PartsAllDone			= FALSE;	// map loaded
		m_b2PartsSortAutoAlignWay	= FALSE;	// map loaded
		MS90Set1stDone(FALSE);	// map loaded
	}
//	m_bIsRegionEnding		= FALSE;	// map loaded.
	m_bEnablePickAndPlace = FALSE;
	m_lBuildShortPathInScanning = 0;
	m_bFirstMapIsOldMap		= FALSE;
	// Clear the information for last wafer map
	m_dScnDieA_X			= 0;
	m_dScnDieA_Y			= 0;
	m_ulScnDieA_Row			= 0;
	m_ulScnDieA_Col			= 0;
	m_ulScnDieB_Row			= 0;
	m_ulScnDieB_Col			= 0;
	m_dWS896Angle_X			= 0.0;
	m_dWS896Angle_Y			= 0.0;
	m_lMsDieA_X				= 0;
	m_lMsDieA_Y				= 0;
	m_lMsDieB_X				= 0;
	m_lMsDieB_Y				= 0;
	m_dMS896Angle_X			= 0.0;
	m_dMS896Angle_Y			= 0.0;
	for(int i=0; i<SCN_ADD_REF_MAX; i++)
	{
		m_lEsScanDieA_X[i]	= 0;
		m_lEsScanDieA_Y[i]	= 0;
		m_lMsSrchDieA_X[i]	= 0;
		m_lMsSrchDieA_Y[i]	= 0;
		m_dCosThetaTran[i]	= 0;
		m_dSinThetaTran[i]	= 0;
	}
	m_lLookForwardCounter = 0;
	m_lNoLookForwardCounter = 0;
	m_bSearchWaferLimitDone	= FALSE;
	if( pUtl->GetPrescanDummyMap()==FALSE )
	{
		m_lAutoWaferCenterX		= GetWaferCenterX();
		m_lAutoWaferCenterY		= GetWaferCenterY();
		m_lAutoWaferDiameter	= GetWaferDiameter();
	}

	DeleteFile(MSD_WAFER_SORTED_BIN_BC_FILE);
	UpdateSortingMode();
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))		//xyz	//Currently only available for PLLM by default
		ReverseSortingMode();
	
	//v4.43T9	//Cree HuiZhou
	//Move last Grade to 1st of list when last-grade has only 1 die
	INT nCount = 0;
	while (UpdateLastGradeSortingMode() == TRUE)
	{
		nCount++;
		if (nCount >= 1)	//> 6)		//v4.53A6	//Fixed bug of long loadmap timeout problem
			break;
	}

	Select1stLFSortGrade();				//v4.59A15	//SanAn & Semitek
	SetupRenesasMapInMapLoaded();

	SetupReferenceCross();

	//Send Wafer map Selected Grade to EM after sorting  2017.10.27
//	SendWaferMapSelectedGradeList();
	//=======================================================

	//v2.78T1
	CString szWaferID = m_szCurrentMapPath;
	szWaferID.Replace("\\", "-");
	BackupDiePitchLog(szWaferID);

	//v4.40T5	//Nichia
	BOOL bNichiaStatus = CheckNichiaCriteraInMapLoaded();
	if (!bNichiaStatus)
	{
		m_WaferMapWrapper.InitMap();
		CMS896AApp::m_bMapLoadingAbort = TRUE;	//v4.40T10
		HmiMessage_Red_Back("Map loading fail because nichia Arr code wrong");
		return;
	}

	if( CMS896AApp::m_lPreBondEventReply > 0 )
	{
		m_WaferMapWrapper.InitMap();
		CMS896AApp::m_bMapLoadingAbort = TRUE;	//v4.40T10
		return;
	}

	if (pApp->GetCustomerName() == "Semitek" && pApp->GetProductLine() == "ZJG")//4.51
	{
		CString szMapTitlename;
		CString szMapContentname;

		szMapTitlename = CMS896AStn::m_WaferMapWrapper.GetFileName();
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("DataFileName", szMapContentname);

		szMapTitlename	= szMapTitlename.MakeUpper();
		szMapContentname = szMapContentname.MakeUpper();
		if (szMapContentname.GetLength()>3)
		{
			szMapContentname = szMapContentname.Mid(1);
			szMapContentname = szMapContentname.Left(szMapContentname.GetLength()-4);
		}
		CMSLogFileUtility::Instance()->MS_LogOperation("szMapTitlename:" + szMapTitlename + ",szMapContentname:" + szMapContentname);

		if(szMapTitlename.Find(szMapContentname) == -1 && szMapTitlename != "" && szMapContentname != "")
		{
			HmiMessage_Red_Back("Map File Data Wrong, refer to Operation.log");
			CMSLogFileUtility::Instance()->MS_LogOperation("Map File data wrong! MapTitlename:" + szMapTitlename + ",MapContentname:" + szMapContentname);
			m_WaferMapWrapper.InitMap();
			CMS896AApp::m_bMapLoadingAbort = TRUE;
			return;
		}

		CString szInMapFileName = "";
		//	new added to check another part in header.
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("MapFileName", szInMapFileName);
		szInMapFileName = szInMapFileName.MakeUpper();
		if (szInMapFileName.GetLength()>3)
		{
			szInMapFileName = szInMapFileName.Mid(1);
			szInMapFileName = szInMapFileName.Left(szInMapFileName.GetLength()-4);
		}
		CMSLogFileUtility::Instance()->MS_LogOperation("szMapTitlename:" + szMapTitlename + ",MapFileName:" + szInMapFileName);
		
		if(szMapTitlename.Find(szInMapFileName) == -1 && szMapTitlename != "" && szInMapFileName != "")
		{
			HmiMessage_Red_Back("Map File Name Wrong, refer to Operation.log");
			CMSLogFileUtility::Instance()->MS_LogOperation("Map File data wrong! MapTitlename:" + szMapTitlename + ",MapFileNamename:" + szInMapFileName);
			m_WaferMapWrapper.InitMap();
			CMS896AApp::m_bMapLoadingAbort = TRUE;
			return;
		}
	}

	if (pApp->GetCustomerName() == "Primaopto")//4.49 compare lot number.
	{
		CString szLotNumberForMap;
		CString szLotNumberForHMI = (*m_psmfSRam)["MS896A"]["LotNumber"];
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("BinTableName", szLotNumberForMap);		
		if (szLotNumberForMap.GetLength()>3)
		{
			szLotNumberForMap = szLotNumberForMap.Mid(2);
			szLotNumberForMap = szLotNumberForMap.Left(szLotNumberForMap.GetLength()-1);
		}
		//AfxMessageBox(szLotNumberForMap);
		//AfxMessageBox(szLotNumberForHMI);
		if( GetScanMethod() != SCAN_METHOD_DUMMY_SORT_ALL && GetScanMethod() != SCAN_METHOD_DUMMY_EXACT_MAP ) //4.51D12
		{
			if (szLotNumberForMap != szLotNumberForHMI)
			{
				HmiMessage_Red_Back("Lot Number Wrong!Map Lot:" + szLotNumberForMap + ",HMI Lot:" + szLotNumberForHMI, "LoadMap");
				m_WaferMapWrapper.InitMap();
				CMS896AApp::m_bMapLoadingAbort = TRUE;
				return;
			}
		}
	}

	if (pApp->GetCustomerName() == "ZhongWei")
	{
		CString szSpec = "";
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("SortBINFileName", szSpec);
		szSpec.Replace(",","");
		szSpec = szSpec.Left(5);
		(*m_psmfSRam)["MS896A"]["ZWSpec"] = szSpec;
	}

	if (pApp->GetCustomerName() == "BabyLighting")//4.49 compare lot number.
	{
		CString szLotNumberForMap;
		CString szLotNumberForHMI = (*m_psmfSRam)["MS896A"]["LotNumber"];
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("Lot #", szLotNumberForMap);
		szLotNumberForHMI.Replace(":","");
		szLotNumberForHMI.Replace(",","");
		szLotNumberForHMI.Replace(" ","");
		if (szLotNumberForHMI.GetLength()>0 && szLotNumberForMap.Find(szLotNumberForHMI) == -1)
		{
				HmiMessage_Red_Back("Lot Number Wrong!Map Lot:" + szLotNumberForMap + ",HMI Lot:" + szLotNumberForHMI, "LoadMap");
				m_WaferMapWrapper.InitMap();
				CMS896AApp::m_bMapLoadingAbort = TRUE;
				return;
		}
	}


	if (pApp->GetCustomerName() == "NSS")
	{
		CString szDeviceNumber;
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("DeviceNumber", szDeviceNumber);
		if (szDeviceNumber.GetLength()>3)
		{
			szDeviceNumber = szDeviceNumber.Mid(2);
			szDeviceNumber = szDeviceNumber.Left(szDeviceNumber.GetLength()-1);
		}

		if (szDeviceNumber == "")
		{
			HmiMessage("Wrong Map Format Without DeviceNumber");
		}
		CString szSavePath = "C:\\MapSorter\\UserData\\OutputFile\\ClearBin\\DeviceNumber";

		CStdioFile cfDevice;
		BOOL bOpen;
		if (_access(szSavePath,0) != -1)
		{
			bOpen = cfDevice.Open(szSavePath, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if( bOpen)
			{
				CString szGotDevice;
				cfDevice.ReadString(szGotDevice);
				if (szGotDevice != szDeviceNumber)
				{
					HmiMessage_Red_Back("DeviceNumber Wrong!" + szGotDevice, "LoadMap");
					m_WaferMapWrapper.InitMap();
					CMS896AApp::m_bMapLoadingAbort = TRUE;
					return;
				}
			}
		}
		else
		{
			bOpen = cfDevice.Open(szSavePath, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if( bOpen)
			{
				cfDevice.WriteString(szDeviceNumber);
			}
		}
	}

	if (pApp->GetCustomerName() == "DeLi")
	{
		CString szDeviceNumber;
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("BinGrade", szDeviceNumber);
		CMSLogFileUtility::Instance()->MS_LogOperation("DeLi:" + szDeviceNumber);

		if (szDeviceNumber == "")
		{
			HmiMessage("Wrong Map Format Without BinGrade");
		}
		CString szSavePath = "C:\\MapSorter\\UserData\\OutputFile\\ClearBin\\BinGrade";

		CStdioFile cfDevice;
		BOOL bOpen;
		if (_access(szSavePath,0) != -1)
		{
			bOpen = cfDevice.Open(szSavePath, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if( bOpen)
			{
				CString szGotDevice;
				cfDevice.ReadString(szGotDevice);
				if (szGotDevice != szDeviceNumber)
				{
					HmiMessage_Red_Back("BinGrade Wrong!" + szGotDevice, "LoadMap");
					m_WaferMapWrapper.InitMap();
					CMS896AApp::m_bMapLoadingAbort = TRUE;
					return;
				}
			}
		}
		else
		{
			bOpen = cfDevice.Open(szSavePath, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if( bOpen)
			{
				cfDevice.WriteString(szDeviceNumber);
			}
		}
	}

	//v4.40T14	
	//if (pApp ->GetCustomerName() == "SanAn" && pApp ->GetProductLine() == "XA")
	//{		
	//	CString szLotNumber;
	//	CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("LotNumber", szLotNumber);
	//	CMSLogFileUtility::Instance()->MS_LogOperation("Check Lot Number(SANAN):" + szLotNumber + "," + m_szUserDefineDieType);

	//	if (szLotNumber.Find(m_szUserDefineDieType) == -1 && m_szUserDefineDieType != "" &&m_szUserDefineDieType != " ")
	//	{
	//		SetErrorMessage("Check Lot Number(SANAN): fail -  " + szLotNumber + " - " + m_szUserDefineDieType);
	//		HmiMessage_Red_Yellow("Check Lot Number(SANAN): fail -  " + szLotNumber + " - " + m_szUserDefineDieType);		//v3.65
	//		m_WaferMapWrapper.InitMap();
	//		CMS896AApp::m_bMapLoadingAbort = TRUE;
	//		return;
	//	}
	//}
	if (pApp->GetCustomerName() == "HuaLei")
	{
		CString szLotNumber;
		CString szLotNumberForHMI = (*m_psmfSRam)["MS896A"]["LotNumber"];
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("BinName", szLotNumber);
		szLotNumber.Remove('"');
		szLotNumber.Remove(',');

		CString szLog;
		szLog.Format("Check Lot Prefix in Map Loaded (HuaLei):New = %s, HMI = %s", szLotNumber, szLotNumberForHMI);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		if (szLotNumberForHMI.GetLength()>0 && szLotNumberForHMI != szLotNumber)
		{
			szLog.Format("Error: Lot = %s is different from Recorded Lot (%s); please try again.", 
							szLotNumber, szLotNumberForHMI);
			HmiMessage_Red_Back(szLog, "LoadMap");
			m_WaferMapWrapper.InitMap();
			CMS896AApp::m_bMapLoadingAbort = TRUE;
			return;
		}

		//Check wafer id
		CString szWaferID = "";
		BOOL bFind = FALSE;
		CString szRead;
		CString szRecordPath = "\\\\192.168.30.251\\WaferEnd\\WECON\\WECON\\WTS_BarCode_CheckFile.txt";

		m_WaferMapWrapper.GetWaferID(szWaferID);
		szWaferID.Replace(".txt","");
		szWaferID.Replace(".TXT","");
		CStdioFile cfRecord;
		//CMSLogFileUtility::Instance()->MS_LogOperation("WaferID***" + szWaferID);
		if(cfRecord.Open(szRecordPath,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead) == TRUE)
		{
			BOOL bRead = FALSE;
			while(1)
			{
				szRead = "";
				bRead = cfRecord.ReadString(szRead);
				//CMSLogFileUtility::Instance()->MS_LogOperation("Check WaferID***" + szRead);
				if (!bRead)
				{
					bFind = FALSE;
					break;
				}
				if (szRead.Find(".TXT") > 0)
				{
					szRead = szRead.Left(szRead.Find(".TXT"));
				}
				if (szRead.Find(".CSV") > 0)
				{
					szRead = szRead.Left(szRead.Find(".CSV"));
				}
				szRead.Replace("\"","");
				CMSLogFileUtility::Instance()->MS_LogOperation("WaferID***" + szWaferID + "," + szRead);
				if (/*szRead.Find(szWaferID) != -1*/szRead == szWaferID)
				{
					bFind = TRUE;
					break;
				}
			}
			cfRecord.Close();
		}
		else
		{
			HmiMessage_Red_Back("Cannt find WTS_BarCode_CheckFile.txt");
			m_WaferMapWrapper.InitMap();
			CMS896AApp::m_bMapLoadingAbort = TRUE;
			return;
		}
		
		if (bFind)
		{
			szLog.Format("Error: WaferID = %s exists (%s); please try again.", 
							szWaferID, szRead);
			HmiMessage_Red_Back(szLog, "LoadMap");
			m_WaferMapWrapper.InitMap();
			CMS896AApp::m_bMapLoadingAbort = TRUE;
			return;
		}
		SaveData();
	}

	if (pApp->GetCustomerName() == "LatticePower")
	{
		CStdioFile cfRead;
		CString szLog;
		CString szLotNumberPath = "c:\\mapsorter\\lotnumberpath.csv";
		CString szReadFromFile = "";
		BOOL bFindLotNumber = FALSE;
		int nFindCount = 0;
		CString szLotNumber;
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_LOTNUMBER, szLotNumber);
		if (cfRead.Open(szLotNumberPath,CFile::modeReadWrite|CFile::modeNoTruncate)==TRUE)
		{
			while(cfRead.ReadString(szReadFromFile))
			{
				nFindCount ++;

				szLog.Format("Check Lot Prefix in Map Loaded (LatticePower): file = %s; map = %s; count,%d", szReadFromFile, szLotNumber,nFindCount);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				if(nFindCount > 10)
					break;
				if(szReadFromFile == "")
					continue;
				
				if(szLotNumber.Find(szReadFromFile) != -1)
				{
					bFindLotNumber = TRUE;
					break;
				}

			}
			cfRead.Close();
		}
		else
		{
			HmiMessage("Can not find :" + szLotNumberPath);
		}

		if (bFindLotNumber == FALSE)
		{
			szLog.Format("Error: cannot find %s in lot file",szLotNumber);
			HmiMessage_Red_Back(szLog, "LoadMap");
			m_WaferMapWrapper.InitMap();
			CMS896AApp::m_bMapLoadingAbort = TRUE;
			return;
		}
		//CString szLotNumber;
		//CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_LOTNUMBER, szLotNumber);
		//m_szLastLotNumber = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LATTICE_LOTNUMBER];
		//CString szOldLotNumber	= m_szLastLotNumber;
		//CString szOldLotPrefix	= m_szLastLotNumber;
		//CString szNewLotPrefix	= szLotNumber;
		//CString szLotNumberForHMI = (*m_psmfSRam)["MS896A"]["LotNumber"];

		//if ((szOldLotNumber.GetLength() > 0) && szOldLotNumber.Find("-") != -1)
		//	szOldLotPrefix = szOldLotNumber.Left(szOldLotNumber.Find("-"));
		//if ((szLotNumber.GetLength() > 0) && szLotNumber.Find("-") != -1)
		//	szNewLotPrefix = szLotNumber.Left(szLotNumber.Find("-"));
		//
		//szOldLotPrefix.Replace("\"", "");
		//szNewLotPrefix.Replace("\"", "");

		//CString szLog;
		//szLog.Format("Check Lot Prefix in Map Loaded (LatticePower): old = %s; New = %s, HMI = %s", szOldLotPrefix, szNewLotPrefix, szLotNumberForHMI);
		//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


		//if (szLotNumberForHMI.GetLength()>0 && szLotNumberForHMI != szNewLotPrefix)
		//{
		//	szLog.Format("Error: Lot prefix = %s is different from Recorded Lot (%s); please try again.", 
		//					szNewLotPrefix, szOldLotPrefix);
		//	HmiMessage_Red_Back(szLog, "LoadMap");
		//	m_WaferMapWrapper.InitMap();
		//	CMS896AApp::m_bMapLoadingAbort = TRUE;
		//	return;
		//}


		//m_szLastLotNumber = szLotNumber;
		//(*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LATTICE_LOTNUMBER] = m_szLastLotNumber;
		//SaveData();
	}


	//v4.40T14	
	if (pApp->GetCustomerName() == "AoYang")
	{
		CString szBinGradeName = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BIN_CODE];

		CString szLog;
		szLog.Format("Check BinGrade suffix in Map Loaded (AoYang): old = %s; New = %s", 
						m_szLastLotNumber, szBinGradeName);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		if ( (szBinGradeName.GetLength() > 0) && (m_szLastLotNumber.GetLength() > 0) )
		{
			INT nIndex = szBinGradeName.Find("-");
			if (nIndex != -1)
			{
				CString szNewSuffix	= szBinGradeName.Mid(nIndex+1);
				CString szOldSuffix	= m_szLastLotNumber.Mid(m_szLastLotNumber.Find("-")+1);

				if (szOldSuffix != szNewSuffix)
				{
					if (!pApp->CheckIsAllBinClearedNoMsg())
					{
						szLog.Format("Error: BinGrade suffix = %s is different from old suffix (%s); please clear bins and try again.", 
										szNewSuffix, szOldSuffix);
						HmiMessage_Red_Back(szLog, "LoadMap (AoYang)");
						m_WaferMapWrapper.InitMap();
						CMS896AApp::m_bMapLoadingAbort = TRUE;
						return;
					}
				}
			}
		}

		m_szLastLotNumber = szBinGradeName;
		SaveData();
	}


	//v3.60	//Cree China	//Determine if AS899 map or prober map is loaded for Cree
	if (m_bEnableTwoDimensionsBarcode)
	{
		const CMapStringToString* pStrMap = m_WaferMapWrapper.GetHeaderInfo();
		CString szSource;	//Additional "Source" header generated from AS899 outputfile DLL
		if (pStrMap->Lookup("Source", szSource) && (szSource == "AS899"))
		{
			m_b2DBarCodeCheckGrade99 = FALSE;	//Only check original empty hole 2D barcode
			SetErrorMessage("Map loaded: Check 2D barcode enabled");
		}
		else
		{
			m_b2DBarCodeCheckGrade99 = TRUE;	//Check grade98 empty hole (2) for prober map
			SetErrorMessage("Map loaded: Check 2D barcode Grade-98 enabled");
		}	
	}

	//Check repeat to load same map file
	if ( !IsBurnIn() &&					//v3.94
		 (m_bCheckRepeatMap == TRUE) && 
		 (m_szCurrentMapPath == GetMapFileName()) )
	{
		HmiMessage_Red_Yellow("Map is repeated!\nPlease check", "Wafer Load Map");
		SetErrorMessage("Map is repeated!  Please check");		//v2.78T2

		//v3.70T2	//PLLM Special Feature
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
		{
			//v2.83T43
			CStrInputDlg dlg;
			dlg.m_szTitle = "Please manual-input wafer barcode:";
			dlg.SetWindowText("Please manual-input wafer barcode:");
			dlg.SetForegroundWindow();
			dlg.BringWindowToTop();
			if (dlg.DoModal() != IDOK)
			{
				m_WaferMapWrapper.InitMap();		
				bMapLoadAbort = TRUE;		//v4.40T10
			}

			CString szMapFileName = GetMapFileName();
			if ((dlg.m_szData.GetLength() < 3) || (dlg.m_szData == ""))
			{
				HmiMessage("Error: manual barcode is empty!");
				m_WaferMapWrapper.InitMap();		
				bMapLoadAbort = TRUE;		//v4.40T10
			}
			if (szMapFileName.Find(dlg.m_szData) == -1)		//If not FOUND
			{
				HmiMessage("Error: Auto & manual barcode do not match!  Please check machine barcode scanner!");
				SetErrorMessage("Error: Auto & manual barcode do not match!  Please check machine barcode scanner!");	//andrew123
				m_WaferMapWrapper.InitMap();		
				bMapLoadAbort = TRUE;		//v4.40T10
			}

			//andrew123
			CString szLog = "Repeat Map manual-input to continue = " + dlg.m_szData;
		}
	}

	//PLLM v3.74T31
	if( m_bCheckRepeatMap && 
		(pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) || pApp->GetCustomerName()=="Lumileds") )
	{
		if (!CheckBcHistoryInCurrLot(GetMapFileName()))
		{
			//v4.33T1	//PLSG
			HmiMessage_Red_Yellow("Map is repeated (2)!\nPlease check", "Wafer Load Map");
			SetErrorMessage("Map is repeated (2)!  Please check");	

			CStrInputDlg dlg;
			dlg.m_szTitle = "Please manual-input wafer barcode:";	//this is printed out!
			dlg.SetWindowText("Please manual-input wafer barcode:");
			dlg.SetForegroundWindow();
			dlg.BringWindowToTop();
			
			if (dlg.DoModal() != IDOK)
			{
				SetErrorMessage("Repeat Map dialog abort");
				m_WaferMapWrapper.InitMap();		
				bMapLoadAbort = TRUE;		//v4.40T10
			}

			CString szMapFileName = GetMapFileName();
			if ((dlg.m_szData.GetLength() < 3) || (dlg.m_szData == ""))
			{
				HmiMessage("Error: manual barcode is empty!");
				m_WaferMapWrapper.InitMap();		
				bMapLoadAbort = TRUE;		//v4.40T10
			}
			if (szMapFileName.Find(dlg.m_szData) == -1)		//If not FOUND
			{
				HmiMessage("Error: Auto & manual barcode do not match!  Please check machine barcode scanner!");
				SetErrorMessage("Error: Auto & manual barcode do not match!  Please check machine barcode scanner!");	//aandrew123
				m_WaferMapWrapper.InitMap();		
				bMapLoadAbort = TRUE;		//v4.40T10
			}

			CString szLog = "Repeat Map manual-input to continue = " + dlg.m_szData;
			SetErrorMessage(szLog);
		}
		
		AddBcHistoryInCurrLot(GetMapFileName());
	}

	//4.52D10run UnSelect Grade from Summary File
	if (pApp->GetCustomerName() == "Lumileds" && pApp->GetProductLine() == "Rebel" && pApp->m_bWaferLabelFile)
	{		
		BOOL bOpenExtrUnSelectGrade = FALSE;
		CString szSummaryBin		= "";
		CString szCatCode			= "";
		UCHAR	ucSummaryBin;
		CString szMapSummaryBin		= "";
		CString szTempMsg;
			
		if(pApp->GetWaferSubFolderSummaryFileContent(szSummaryBin, szCatCode))
		{
			bOpenExtrUnSelectGrade = TRUE;
			ucSummaryBin = atoi(szSummaryBin) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
			szMapSummaryBin.Format("%d",ucSummaryBin);
			szTempMsg.Format("WaferLabel -- Updated Summary Bin(%s,%s)", szSummaryBin, szMapSummaryBin);
			CMSLogFileUtility::Instance()->WL_LogStatus(szTempMsg);
//AfxMessageBox(szTempMsg, MB_SYSTEMMODAL);
		}
		

		if (bOpenExtrUnSelectGrade)
		{
			CUIntArray aulReOrderGradeList, aulSelectedGradeList;
			m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);


			for (INT k=0; k<aulSelectedGradeList.GetSize(); k++)
			{
				CString szTempGrade;
				szTempGrade.Format("%d",  aulSelectedGradeList.GetAt(k));

szTempMsg.Format("WaferLabel -- Updated Grade Select (%s, %s)",szTempGrade, szMapSummaryBin);
CMSLogFileUtility::Instance()->WL_LogStatus(szTempMsg);
//AfxMessageBox(szTempMsg, MB_SYSTEMMODAL);
				if(szTempGrade == szMapSummaryBin)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Donot add the unSelected Summary Bin:" + szMapSummaryBin);
				}
				else
				{
					aulReOrderGradeList.Add(aulSelectedGradeList.GetAt(k));	
				}
			}

			UCHAR	*pGrade;
			pGrade = new UCHAR[aulSelectedGradeList.GetSize()];
			for (int j=0; j<aulReOrderGradeList.GetSize(); j++)
			{
				pGrade[j] = aulReOrderGradeList.GetAt(j);
			}

			m_WaferMapWrapper.SelectGrade(pGrade, (unsigned long)aulReOrderGradeList.GetSize());

			delete[] pGrade;
		}

	}	//If WaferLabel 

	//v4.53A23
	if (pApp->GetCustomerName() == "Lumileds")
	{
		m_WaferMapWrapper.GetReader()->MapData("NoNullBin", "TRUE");
	}

	m_ulGrade151Total	= 0;	//	map loaded
	m_szCurrentMapPath = GetMapFileName();
	CString szFullPath = m_szCurrentMapPath.MakeUpper();
	CString szMapPath = m_szMapFilePath.MakeUpper();
	if( szFullPath.Find(szMapPath)!=-1 )
	{
		m_szMapServerFullPath = m_szCurrentMapPath;
	}

	//v3.60T1	//NeoNeon
	if ( IsWLManualMode() )	//Manual Mode
	{
		CString szFileName = GetMapFileName();
		if ( m_bEnablePrefix && m_bEnablePrefixCheck && (m_szPrefixName.IsEmpty() == FALSE) )		//Check prefix existence	//v3.61	//NeoNeon
		{
			if (pApp->GetCustomerName() != "Epitop")//4.52D15
			{
    			if (szFileName.Find(m_szPrefixName) == -1)
    			{
    				m_WaferMapWrapper.InitMap();	//Clear map if prefix not match
    				SetErrorMessage("WT: barcode Load map check prefix fail: " + szFileName);
    				HmiMessage("Map prefix does not match!\nPlease check");		//, MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
    				SetStatusMessage("Manual Load map prefix check fail");
    				SetErrorMessage("Manual Load map prefix check fail");
    				m_WaferMapWrapper.InitMap();	//Clear map if prefix not match
    				bMapLoadAbort = TRUE;		//v4.40T10
    			}
		     }
		}

		if (m_bEnableSuffix && (m_szSuffixName.IsEmpty() == FALSE) )
		{
			if (m_bEnableSuffixCheck)							//v4.36		//SanAn
			{
				if (szFileName.Find(m_szSuffixName) == -1)
				{
					m_WaferMapWrapper.InitMap();	//Clear map if suffix not match
					SetErrorMessage("WT: barcode Load map check suffix fail: " + szFileName);
					HmiMessage("Map suffix does not match!\nPlease check");		//, MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
					SetStatusMessage("Manual Load map suffix check fail");
					SetErrorMessage("Manual Load map suffix check fail");
					m_WaferMapWrapper.InitMap();	//Clear map if suffix not match
					bMapLoadAbort = TRUE;		//v4.40T10
				}
			}
			else if (szFileName.Find(m_szSuffixName) == -1)		//v3.65		//Semitek
			{
				m_WaferMapWrapper.InitMap();	//Clear map if suffix not match
				SetErrorMessage("WT: barcode Load map check suffix fail: " + szFileName);
				HmiMessage("Map suffix does not match!\nPlease check");		//, MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
				SetStatusMessage("Manual Load map suffix check fail");
				SetErrorMessage("Manual Load map suffix check fail");
			}
		}
	}

	//v3.25T16	//OptoTech
	CString szLog;
	szLog = _T("Load Map (") + m_szCurrentMapPath + _T(")");
	if( IsBlkFunc1Enable() )
	{
		LOG_BLOCK_ALIGN("");
		LOG_BLOCK_ALIGN(szLog);
	}

	if( /*pApp->GetCustomerName()=="Semitek" ||*/
		pApp->GetCustomerName()=="CMLT" )
	{
		m_bEnableSCNCheck = TRUE;
	}

	//	427TX	5
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	ObtainMapValidRange();

	SaveData();

	// Tekcore to add refer die for block pick alignment
	if( m_lReferPitchRow>1 && m_lReferPitchCol>1 && m_bFindHomeDieFromMap )
	{
		ULONG ulHomeRow = 0, ulHomeCol = 0;
		LONG  lRow, lCol;
		GetMapAlignHomeDie(ulHomeRow, ulHomeCol);
		UCHAR ucReferGrade = ucNullBin;
		if( m_WaferMapWrapper.GetReader() != NULL )
		{
			ucReferGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
		}
		m_WaferMapWrapper.ChangeGrade(ulHomeRow, ulHomeCol, ucReferGrade);
		m_WaferMapWrapper.SetReferenceDie(ulHomeRow, ulHomeCol, TRUE);
		LONG lUp = GetMapValidMaxRow(), lDown = GetMapValidMinRow();
		LONG lLeft = GetMapValidMaxCol(), lRight = GetMapValidMinCol();
		// low part refer die adding
		for(lRow=ulHomeRow; lRow<=GetMapValidMaxRow(); lRow+=m_lReferPitchRow)
		{
			for(lCol=ulHomeCol-m_lReferPitchCol; lCol>=GetMapValidMinCol(); lCol-=m_lReferPitchCol)
			{
				if( HasMapValidDieAround(lRow, lCol, 2) )
				{
					m_WaferMapWrapper.ChangeGrade(lRow, lCol, ucReferGrade);
					m_WaferMapWrapper.SetReferenceDie(lRow, lCol, TRUE);
					if( lUp>lRow )
						lUp = lRow;
					if( lDown<lRow )
						lDown = lRow;
					if( lLeft>lCol )
						lLeft = lCol;
					if( lRight<lCol )
						lRight = lCol;
				}
			}
			for(lCol=ulHomeCol; lCol<=GetMapValidMaxCol(); lCol+=m_lReferPitchCol)
			{
				if( HasMapValidDieAround(lRow, lCol, 2) )
				{
					m_WaferMapWrapper.ChangeGrade(lRow, lCol, ucReferGrade);
					m_WaferMapWrapper.SetReferenceDie(lRow, lCol, TRUE);
					if( lUp>lRow )
						lUp = lRow;
					if( lDown<lRow )
						lDown = lRow;
					if( lLeft>lCol )
						lLeft = lCol;
					if( lRight<lCol )
						lRight = lCol;
				}
			}
		}
		// up part refer die adding
		for(lRow=ulHomeRow-m_lReferPitchRow; lRow>=GetMapValidMinRow(); lRow-=m_lReferPitchRow)
		{
			for(lCol=ulHomeCol-m_lReferPitchCol; lCol>=GetMapValidMinCol(); lCol-=m_lReferPitchCol)
			{
				if( HasMapValidDieAround(lRow, lCol, 2) )
				{
					m_WaferMapWrapper.ChangeGrade(lRow, lCol, ucReferGrade);
					m_WaferMapWrapper.SetReferenceDie(lRow, lCol, TRUE);
					if( lUp>lRow )
						lUp = lRow;
					if( lDown<lRow )
						lDown = lRow;
					if( lLeft>lCol )
						lLeft = lCol;
					if( lRight<lCol )
						lRight = lCol;
				}
			}
			for(lCol=ulHomeCol; lCol<=GetMapValidMaxCol(); lCol+=m_lReferPitchCol)
			{
				if( HasMapValidDieAround(lRow, lCol, 2) )
				{
					m_WaferMapWrapper.ChangeGrade(lRow, lCol, ucReferGrade);
					m_WaferMapWrapper.SetReferenceDie(lRow, lCol, TRUE);
					if( lUp>lRow )
						lUp = lRow;
					if( lDown<lRow )
						lDown = lRow;
					if( lLeft>lCol )
						lLeft = lCol;
					if( lRight<lCol )
						lRight = lCol;
				}
			}
		}

		// see the up and down direction to make sure all covered
		LONG lPt1, lPt2;
		BOOL bFindPt1 = FALSE;
		for(lRow=lUp; lRow<=lDown; lRow+=m_lReferPitchRow)
		{
			bFindPt1 = FALSE;
			for(lCol=lLeft; lCol<=lRight; lCol+=m_lReferPitchCol)
			{
				if( m_WaferMapWrapper.IsReferenceDie(lRow, lCol) )
				{
					if( bFindPt1==FALSE )
					{
						lPt1 = lCol;
						bFindPt1 = TRUE;
					}
					lPt2 = lCol;
				}
			}
			if( bFindPt1 )
			{
				for(lCol=lPt1+m_lReferPitchCol; lCol<lPt2; lCol+=m_lReferPitchCol)
				{
					if( m_WaferMapWrapper.IsReferenceDie(lRow, lCol)==FALSE )
					{
						m_WaferMapWrapper.ChangeGrade(lRow, lCol, ucReferGrade);
						m_WaferMapWrapper.SetReferenceDie(lRow, lCol, TRUE);
					}
				}
			}
		}
		for(lCol=lLeft; lCol<=lRight; lCol+=m_lReferPitchCol)
		{
			bFindPt1 = FALSE;
			for(lRow=lUp; lRow<=lDown; lRow+=m_lReferPitchRow)
			{
				if( m_WaferMapWrapper.IsReferenceDie(lRow, lCol) )
				{
					if( bFindPt1==FALSE )
					{
						lPt1 = lRow;
						bFindPt1 = TRUE;
					}
					lPt2 = lRow;
				}
			}
			if( bFindPt1 )
			{
				for(lRow=lPt1+m_lReferPitchRow; lRow<lPt2; lRow+=m_lReferPitchRow)
				{
					if( m_WaferMapWrapper.IsReferenceDie(lRow, lCol)==FALSE )
					{
						m_WaferMapWrapper.ChangeGrade(lRow, lCol, ucReferGrade);
						m_WaferMapWrapper.SetReferenceDie(lRow, lCol, TRUE);
					}
				}
			}
		}
	}

	if( m_bFindAllRefer4Scan && m_lReferMapTotal>0 && m_lReferMapTotal<WT_ALN_MAXCHECK_SCN )
	{
		ULONG ulNumRefDie = m_WaferMapWrapper.GetNumberOfReferenceDice();
		WAF_CMapDieInformation *astReferList;
		astReferList = new WAF_CMapDieInformation [ulNumRefDie];
		m_WaferMapWrapper.GetReferenceDieList(astReferList, ulNumRefDie);
		for(ULONG i=0; i<ulNumRefDie; i++)
		{
			ULONG ulTgtRow = astReferList[i].GetRow();
			ULONG ulTgtCol = astReferList[i].GetColumn();
			m_WaferMapWrapper.SetReferenceDie(ulTgtRow, ulTgtCol, FALSE);
		}
		delete[] astReferList;

		UCHAR ucReferGrade = ucNullBin;
		if( m_WaferMapWrapper.GetReader() != NULL )
		{
			ucReferGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
		}
		for(INT i=0; i<m_lReferMapTotal; i++)
		{
			ULONG ulAsmRow = 0, ulAsmCol = 0;
			LONG lUserRow = m_lReferMapRow[i];
			LONG lUserCol = m_lReferMapCol[i];
			if( ConvertOrgUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol) )
			{
				m_WaferMapWrapper.ChangeGrade(ulAsmRow, ulAsmCol, ucReferGrade);
				m_WaferMapWrapper.SetReferenceDie(ulAsmRow, ulAsmCol, TRUE);
			}
		}
	}	//	Add refer die from setting file

	DeleteFile(WT_ADV_RGN_OFFSET_GET_LOG);
	DeleteFile(WT_ADV_RGN_OFFSET_SET_LOG);
	DeleteFile(gszUSER_DIRECTORY + WT_ADV_RGN_SAMPLE_LOG);

	if( pApp->GetCustomerName()=="ZhongKe" && IsAOIOnlyMachine() && m_bPrescanSkipNgGrade )
	{
		CUIntArray aulAvailableGradeList;
		UCHAR ucBinOffset = m_WaferMapWrapper.GetGradeOffset();
		UCHAR ucMaxGrade = 0;
		m_WaferMapWrapper.GetAvailableGradeList(aulAvailableGradeList);
		for (INT i=0; i < aulAvailableGradeList.GetSize(); i++)
		{
			UCHAR ucGrade = aulAvailableGradeList.GetAt(i)- ucBinOffset;
			if (ucGrade > ucMaxGrade)
			{
				ucMaxGrade = ucGrade;
			}
		}
		m_ucScanMapStartNgGrade = m_ucScanMapEndNgGrade = ucMaxGrade;
	}	// zhong ke


	//v4.45T2
	if( pApp->GetCustomerName()=="Cree" && !m_bPrescanSkipNgGrade &&
		((m_ucScanMapStartNgGrade == 98) || (m_ucScanMapStartNgGrade == 144)) )
	{
		ULONG ulPick1=0, ulLeft1=0, ulTotal1=0;
		ULONG ulPick2=0, ulLeft2=0, ulTotal2=0;
		m_WaferMapWrapper.GetStatistics(98  + m_BinMapWrapper.GetGradeOffset(), ulLeft1, ulPick1, ulTotal1);
		m_WaferMapWrapper.GetStatistics(144 + m_BinMapWrapper.GetGradeOffset(), ulLeft2, ulPick2, ulTotal2);

		if (ulTotal2 > 0)		//Has Grade 144 dices
		{
			m_ucScanMapStartNgGrade = m_ucScanMapEndNgGrade = 144;
//HmiMessage("Cree NG Grade set to 144");
		}
		else if (ulTotal1 > 0)	//els has Grade 98 dices
		{
			m_ucScanMapStartNgGrade = m_ucScanMapEndNgGrade = 98;
//HmiMessage("Cree NG Grade set to 98");
		}
	}

	if( m_bFindHomeDieFromMap )
	{
		if( AutoFindHomeDieFromMap()==FALSE )
		{
			return ;
		}
	}

	WT_CSubRegionsInfo* pSRInfo = WT_CSubRegionsInfo::Instance();
	pSRInfo->SetCurrentRegionState_HMI("");

	if( m_WaferMapWrapper.GetReader() != NULL )
	{
		m_WaferMapWrapper.GetReader()->MapData("Keep Order", "TRUE");
	}

	//if (CMS896AStn::m_bMS100EjtXY)
	if (m_bEnableMS100EjtXY)	//v4.52A14
	{
		SetWTAtColletPos(0);
		m_nWTLastColletPos = 0;	//v4.47T2
	}

	//v4.40T10
	if (bMapLoadAbort == TRUE)
	{
		//Andrew: Set to TRUE so no need to wait for LoadMap Timeout (> 25 sec) in WL task 
		//			under AUTOBOND mode;
		//			Not yet used for other customers except Nichia
		//CMS896AApp::m_bMapLoadingAbort = TRUE;
	}

	// preform a check whether the colunm number loaded is equal to header number loaded
	// if they different, give a warning
	// check by comparing one elecdata of dye to waferheaderfile 
	CheckHeaderElectricData();

	SetupSubRegionMode(TRUE);	// map loaded

	ULONG ulRow = 0, ulCol = 0;
	GetMapAlignHomeDie(ulRow, ulCol);
	m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
	m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
	if( m_bAutoLoadWaferMap==FALSE && IsWL1ExpanderSafeToMove() )
	{
		LONG  lHomeWftX = 0, lHomeWftY = 0;
		GetHomeDiePhyPosn(lHomeWftX, lHomeWftY);
		//XY_SafeMoveTo(lHomeWftX, lHomeWftY);
		Sleep(100);
	}

	if( m_bAutoLoadWaferMap==FALSE && m_bCheckMapHomeNullBin )
	{
		if (m_pWaferMapManager->IsMapHaveBin(ulRow, ulCol))
		{
			HmiMessage_Red_Back("Home die in map is not a null bin.", "Wafer Map");
		}
	}

	CString szHolePmpFile = m_szPrescanLogPath + "_Hole.txt";	//	map loaded, to delete
	DeleteFile(szHolePmpFile);
	if( IsMS90HalfSortDummyScanAlign() )
	{
		SaveScanTimeEvent("    WFT: to create hole pattern on map");
		//	Dummy scan for SiLan MS90 half sort mode, IPC command to add dummy map hole patterns.
		IPC_CServiceMessage stMsg;
		INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "AddScanAlignPattern4MS90", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}
}

VOID CWaferTable::CutElectricData( CString &szTempString , LONG &lnumber , CHAR seperator )
{
	if ( szTempString.Find( seperator ) == 0 )
	{
		szTempString = szTempString.Right(szTempString.GetLength() - 1);
	}	
	while ( szTempString.Find( seperator ) != -1 ) 
	{
		lnumber++;
		szTempString = szTempString.Right(szTempString.GetLength() - szTempString.Find(seperator) - 1);
	}
}

BOOL CWaferTable::CheckHeaderElectricData(CHAR seperator)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="BlueLight" )
	{
		CStdioFile pReadFile;
		BOOL bReadOK = FALSE;
		CString szFile = gszROOT_DIRECTORY + "\\Exe\\WaferMapColumns.txt";
		bReadOK = pReadFile.Open(szFile, CFile::modeRead);
		LONG lLeftCounter = 0;
		LONG lRightCounter = 0;
		if (bReadOK	!= TRUE) 
		{
			HmiMessage("'WaferMapColumns.txt' is missing!!!", "Error");
			return FALSE;
		}
		else
		{
			CString szContents;
			pReadFile.SeekToBegin();
			if ( pReadFile.ReadString(szContents) == FALSE ) 
			{
				HmiMessage("File 'WaferMapColumns.txt' Empty", "Error");
				return FALSE;
			}
			else 
			{
				int n = atoi(szContents);
				while ( pReadFile.ReadString(szContents) ) 
				{
					lLeftCounter++;
				}
				if ( n != lLeftCounter ) 
				{
					HmiMessage("File 'WaferMapColumns.txt' Error!", "Error");
					return FALSE;
				}
			}
		}
		CString szElectricInfoLine = "";
		CStdioFile oFile;
		if ( oFile.Open(GetMapFileName(), CFile::modeRead|CFile::shareDenyNone) )
		{	// Get the sample electricial data
			unsigned long ulNumOfRows=0, ulNumOfCols=0;
			m_WaferMapWrapper.GetMapDimension(ulNumOfRows, ulNumOfCols);
			
			CUIntArray aulSelectedGradeList;
			m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
			BOOL bFindOne = FALSE;
			for(int i = 0; i < (INT)ulNumOfRows; i++)
			{
				for(int j = 0; j < (INT)ulNumOfCols; j++)
				{
					if (m_pWaferMapManager->IsMapHaveBin(i, j) &&
						m_WaferMapWrapper.IsReferenceDie(i, j)==FALSE )
					{
						for(int k=0; k<aulSelectedGradeList.GetSize(); k++)
						{
							if( m_WaferMapWrapper.GetGrade(i, j)==aulSelectedGradeList.GetAt(k) )
							{
								m_WaferMapWrapper.GetExtraInformation(&oFile, i, j, szElectricInfoLine);
								bFindOne = TRUE;
								break;
							}
						}
					}
					if( bFindOne )
					{
						break;
					}
				}
				if( bFindOne )
				{
					break;
				}

			}
		}
		m_szSampleElectricInfo = szElectricInfoLine;
		CString szTempString = szElectricInfoLine;
		//HmiMessage(szElectricInfoLine);
		lRightCounter = szTempString.GetLength()==0 ? 0 : 1 ;
		CutElectricData( szTempString , lRightCounter , seperator );
		//HmiMessage(szTempString);
		/*
		while ( szTempString.Find( '\t' ) != -1 ) 
		{
			lRightCounter++;
			szTempString = szTempString.Right(szTempString.GetLength() - szTempString.Find('\t') - 1);
		}
		*/
		// left counter minus 3
		if ( lRightCounter != lLeftCounter - 3) 
		{
			CString szErrorM = "Read electric info format ERROR!!!\n Please Check the format same with the HEADER file.";
			HmiMessage(szErrorM, "Error");
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CWaferTable::SaveData(VOID)
{
	SaveWaferTblData();
	CString szTemp, szPreviousUserDefineDieType;
	CMSPrescanUtility  *pPUtl = CMSPrescanUtility::Instance();

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	LONG i = 0;

	if (pUtl->LoadLastState("WT: SaveData") == FALSE) 		// open LastState file
	{
		return FALSE;	
	}

	psmf = pUtl->GetLastStateFile("WT: SaveData");	// get file pointer

	if ( psmf == NULL )
	{
		return FALSE;
	}

	m_pWaferMapManager->SaveData(&(*psmf)[WT_MAP_DATA]);
	m_pEjPinCleanRegion->SaveData(psmf);
	(*psmf)[WT_MAP_DATA][WT_MAP_FORMAT]		= m_szWaferMapFormat;
	// Alignment Data
	
//	(*psmf)[WT_ALIGN_DATA][WT_LHS_INDEX]		= m_lLHSIndexCount;
//	(*psmf)[WT_ALIGN_DATA][WT_RHS_INDEX]		= m_lRHSIndexCount;
	(*psmf)[WT_ALIGN_DATA][WT_MOVEBACK_ANGLE]	= m_dMoveBackAngle;			//v2.82

	(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION]		= m_bNewGTPosition;
	(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION_X]	= m_lNewGTPositionX;
	(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION_Y]	= m_lNewGTPositionY;
	(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION_X2]	= m_lNewGTPositionX2;
	(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION_Y2]	= m_lNewGTPositionY2;
	(*psmf)[WT_ALIGN_DATA]["NEWGTUSE2"]				= m_bNewGTUse2;
	//andrewng //2020-0902
	(*psmf)[WT_ALIGN_DATA]["Unload Position X"]		= m_lUnloadPhyPosX;
	(*psmf)[WT_ALIGN_DATA]["Unload Position Y"]		= m_lUnloadPhyPosY;


	if (((BOOL)(LONG)(*psmf)[WT_OPTION][WT_DIE_PITCH_CHECK]) && (m_bDiePitchCheck == FALSE))
	{
		SetErrorMessage("Disabled Die Pitch Check");
	}
	else if (((BOOL)(LONG)(*psmf)[WT_OPTION][WT_DIE_PITCH_CHECK] == FALSE) && (m_bDiePitchCheck == TRUE))
	{
		SetErrorMessage("Enable Die Pitch Check");
	}

	if (((BOOL)(LONG)(*psmf)[WT_OPTION][WT_ABS_DIE_PITCH_CHECK]) && (m_bAbsDiePitchCheck == FALSE))
	{
		SetErrorMessage("Disabled Abs Die Pitch Check");
	}
	else if (((BOOL)(LONG)(*psmf)[WT_OPTION][WT_ABS_DIE_PITCH_CHECK] == FALSE) && (m_bAbsDiePitchCheck == TRUE))
	{
		SetErrorMessage("Enabled Abs Die Pitch Check");
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( (pApp->GetCustomerName()=="Cree") )
	{
		m_bDiePitchCheck = TRUE;
	}

	if( pApp->GetCustomerName()=="Semitek" )
	{
		m_bDiePitchCheck = TRUE;
	}

	if( pApp->GetCustomerName()=="Huga" ||	pApp->GetCustomerName()=="FATC" )
	{
		m_bPrescanBackupMap = TRUE;
	}

	if (pApp->GetCustomerName() == "Semitek" && pApp->GetProductLine() == "ZJG")
	{
		m_ucMapRotation = 0;
		m_bMapHoriFlip = 0;
		m_bMapVertFlip = 0;
		m_bShowFirstDie = 0;
	}

	if (pApp->GetCustomerName() != "OSRAM")
	{
		m_bOsramResortMode = FALSE;
	}
	if ( m_lWaferEmptyLimit <= 0 )
	{
		m_lWaferEmptyLimit = 1;
	}
	if ( m_lWaferEdgeLimit <= 0 )
	{
		m_lWaferEdgeLimit = 1;
	}
	if ( m_lWaferStreetLimit <= 0 )
	{
		m_lWaferStreetLimit = 1;
	}

	if ( pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && 
		 (m_bSearchHomeOption != WT_SPIRAL_SEARCH))		//v4.46T9 not U2U wafer
	{
		m_bAutoSrchHome = TRUE;
	}

	//v4.24T9	//ES101
	if ( IsESDualWT()==FALSE )
		m_nWTInUse		= 0;
	(*psmf)[WT_OPTION][WT_TABLE_INUSE]			= m_nWTInUse;				//ES101	//v4.24T9
	m_lJsWftInUse = IsWT2InUse();

	(*psmf)[WT_OPTION][WT_DIE_PITCH_CHECK]		= m_bDiePitchCheck;
	(*psmf)[WT_OPTION][WT_ABS_DIE_PITCH_CHECK]	= m_bAbsDiePitchCheck;		//v2.78T2
	(*psmf)[WT_OPTION][WT_PREDSCAN_PITCHTOL_X]	= m_lPredScanPitchTolX;
	(*psmf)[WT_OPTION][WT_PREDSCAN_PITCHTOL_Y]	= m_lPredScanPitchTolY;
	(*psmf)[WT_OPTION][WT_PITCH_X_TOLERANCE]	= m_lPitchX_Tolerance;
	(*psmf)[WT_OPTION][WT_PITCH_Y_TOLERANCE]	= m_lPitchY_Tolerance;
	(*psmf)[WT_OPTION][WT_ENABLE_GLOBAL_THETA]	= m_bEnableGlobalTheta;		//v2.91T1
	(*psmf)[WT_OPTION][WT_AUTO_SEARCH_HOME]		= m_bAutoSrchHome;
	(*psmf)[WT_OPTION][WT_COR_MOVE_PITCHTOL_X]	= m_lCorMovePitchTolX;
	(*psmf)[WT_OPTION][WT_WAITEJUP]				= m_lWaitEjectorUp;
	(*psmf)[WT_OPTION][WT_COR_MOVE_PITCHTOL_Y]	= m_lCorMovePitchTolY;
	m_lCorBackPitchTolX	= m_lCorMovePitchTolX/2;
	m_lCorBackPitchTolY	= m_lCorMovePitchTolY/2;
	(*psmf)[WT_OPTION][WT_COR_BACK_PITCHTOL_X]	= m_lCorBackPitchTolX;
	(*psmf)[WT_OPTION][WT_COR_BACK_PITCHTOL_Y]	= m_lCorBackPitchTolY;

	(*psmf)[WT_OPTION][WT_PLLM_WAFER_ROW]	= m_lPLLMWaferRow;
	(*psmf)[WT_OPTION][WT_PLLM_WAFER_COL]	= m_lPLLMWaferCol;
	
	(*psmf)[WT_OPTION][WT_MAP_GRADE_DISPLAY_MODE]	= m_szGradeDisplayMode;
	(*psmf)[WT_OPTION][WT_MAP_GRADE_DISPLAY_MODE_SELECT]	= m_lGradeDisplayMode;
	
	(*psmf)[WT_OPTION][WT_MAP_PATH]				= m_szMapFilePath;
	(*psmf)[WT_OPTION][WT_MAP_EXT]				= m_szMapFileExtension;
	(*psmf)[WT_OPTION][WT_BACKUP_MAP_PATH]		= m_szBackupMapPath;
	(*psmf)[WT_OPTION][WT_BACKUP_MAP_EXT]		= m_szBackupMapExtension;
	(*psmf)[WT_OPTION][WT_UPLOAD_BARCODE_ID_PATH]	= m_szUploadBarcodeIDFilePath;

	(*psmf)[WT_OPTION][WT_MAP_ROTATION]			= m_ucMapRotation;
	(*psmf)[WT_OPTION][WT_AUTO_WAFER_ROTATION]	= m_ucAutoWaferRotation;
	(*psmf)[WT_OPTION][WT_AUTO_BC_ROTATION]		= m_ucAutoBCRotation;
	(*psmf)[WT_OPTION][WT_MAP_EDGE_SIZE]		= m_ucMapEdgeSize;
	(*psmf)[WT_OPTION][WT_MAP_HORI_FLIP]		= m_bMapHoriFlip;
	(*psmf)[WT_OPTION][WT_MAP_VERT_FLIP]		= m_bMapVertFlip;
	(*psmf)[WT_OPTION][WT_MAP_SHOW_FIRST_DIE]	= m_bShowFirstDie;
	(*psmf)[WT_OPTION][WT_MAP_SORT_MODE]		= m_lSortingMode;
	(*psmf)[WT_OPTION][WT_MAP_MIN_GRADE_COUNT]	= m_ulMinGradeCount;
	(*psmf)[WT_OPTION][WT_MAP_MIN_GRADE_BIN]	= m_ulMinGradeBinNo;
	(*psmf)[WT_OPTION][WT_MAP_PICK_MODE]		= m_ucMapPickMode;			//v2.78T1
	(*psmf)[WT_OPTION][WT_MAP_DIE_STEP]			= m_ulMapDieStep;			//v2.78T1
	(*psmf)[WT_OPTION][WT_MAP_TOUR]				= m_szMapTour;
	(*psmf)[WT_OPTION][WT_MAP_PATH_SELECTION]	= m_szMapPath;
	(*psmf)[WT_OPTION][WT_MAP_MULTI_WAFER]		= m_bMultiWaferSupport;		//v2.91T1
	(*psmf)[WT_OPTION][WT_MAP_SPIRAL_SEARCH_DIR]= m_ucSpiralPickDirection;	//v2.95T1
	(*psmf)[WT_OPTION][WT_MAP_SMART_WALK]		= m_bEnableSmartWalk;		//v2.99T1
	(*psmf)[WT_OPTION][WT_MAP_SMART_1STDIE_FINDER]	= m_bEnableSWalk1stDieFinder;		//v3.15T1
	(*psmf)[WT_OPTION][WT_MAP_START_POINT]		= m_bStartPoint;			//v3.01T1
	(*psmf)[WT_OPTION][WT_MAP_SPECIAL_REFDIE_GRADE]	= m_lSpecialRefDieGrade;	//v3.24T1
	pPUtl->SetSpcReferGrade(m_lSpecialRefDieGrade);

	(*psmf)[WT_OPTION][WT_MAP_BLK_FUNC_ENABLE]	= m_bBlkFuncEnable;			//v3.30T1
	(*psmf)[WT_OPTION][WT_MULTI_COR_OFFSET]		= m_bUseMultiCorOffset;		//v3.44		//ShowaDenko

	(*psmf)[WT_OPTION][WT_DIE_CROSS_CHECK]		= m_bEnableXCheck;
	(*psmf)[WT_OPTION][WT_PICK_CROSS_CHECK_DIE]	= m_bPickCrossCheckDie;		//v3.57T1
	(*psmf)[WT_OPTION][WT_XCHECK_TOLERANCEX]	= m_lXCheckToleranceX;
	(*psmf)[WT_OPTION][WT_XCHECK_TOLERANCEY]	= m_lXCheckToleranceY;

	(*psmf)[WT_OPTION][WT_REF_CROSS_ROW]		= m_lRefCrossRow;
	(*psmf)[WT_OPTION][WT_REF_CROSS_COL]		= m_lRefCrossCol;
	(*psmf)[WT_OPTION][WT_REF_CROSS_WIDTH]		= m_lRefCrossWidth;

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	(*psmf)[WT_OPTION][WT_MAP_SUB_ROWS]			= pSRInfo->m_ulSubRows;
	(*psmf)[WT_OPTION][WT_MAP_SUB_COLS]			= pSRInfo->m_ulSubCols;
	(*psmf)[WT_OPTION][WT_MAP_SORT_BY_GRADE]	= m_lSubSortMode;

	(*psmf)[WT_OPTION][WT_MAP_IGNORE_REGION]	= m_bIgnoreRegion;
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION1_ULX]	= m_ulSelectRegionULX[1];		//v3.49T5
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION1_ULY]	= m_ulSelectRegionULY[1];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION1_LRX]	= m_ulSelectRegionLRX[1];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION1_LRY]	= m_ulSelectRegionLRY[1];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION2_ULX]	= m_ulSelectRegionULX[2];		//v3.49T5
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION2_ULY]	= m_ulSelectRegionULY[2];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION2_LRX]	= m_ulSelectRegionLRX[2];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION2_LRY]	= m_ulSelectRegionLRY[2];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION3_ULX]	= m_ulSelectRegionULX[3];		//v3.49T5
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION3_ULY]	= m_ulSelectRegionULY[3];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION3_LRX]	= m_ulSelectRegionLRX[3];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION3_LRY]	= m_ulSelectRegionLRY[3];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION4_ULX]	= m_ulSelectRegionULX[4];		//v3.49T5
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION4_ULY]	= m_ulSelectRegionULY[4];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION4_LRX]	= m_ulSelectRegionLRX[4];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION4_LRY]	= m_ulSelectRegionLRY[4];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION5_ULX]	= m_ulSelectRegionULX[5];		//v3.49T5
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION5_ULY]	= m_ulSelectRegionULY[5];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION5_LRX]	= m_ulSelectRegionLRX[5];
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION5_LRY]	= m_ulSelectRegionLRY[5];

	(*psmf)[WT_OPTION][WT_No_Die_COOR_X]		= m_lWTNoDiePosX;
	(*psmf)[WT_OPTION][WT_No_Die_COOR_Y]		= m_lWTNoDiePosY;
	(*psmf)[WT_OPTION][WT_Use_LF_2nd_Alignment]			= m_bUseLF2ndAlignment;
	(*psmf)[WT_OPTION][WT_Use_LF_2nd_Align_NoMotion]	= m_bUseLF2ndAlignNoMotion;	//andrewng //2020-05-19

	if( m_bBarCodeSearchMap )
	{
		m_bBarCodeSearchFolder = FALSE;
	}

	(*psmf)[WT_OPTION][WT_MAP_SEARCH_PART_MAP]		= m_bBarCodeSearchPartMap;
	(*psmf)[WT_OPTION][WT_MAP_SELECT_REGION_GRADE]	= m_ucSelectChangeGrade;
	(*psmf)[WT_OPTION][WT_MAP_BARCODE_READMAP]		= m_bEnableBarcodeLoadMap;
	(*psmf)[WT_OPTION][WT_MAP_BARCODE_SEARCH_MAP]	= m_bBarCodeSearchMap;			//v3.54T1
	(*psmf)[WT_OPTION][WT_MAP_SEARCH_SUB_FOLDER]	= m_bBarCodeSearchFolder;
	(*psmf)[WT_OPTION][WT_MAP_SEARCH_FOLDER_NAME]	= m_szSearchFolderName;
	(*psmf)[WT_OPTION][WT_MAP_ENABLE_SUFFIX]		= m_bEnableSuffix;
	(*psmf)[WT_OPTION][WT_MAP_SUFFIX_NAME]			= m_szSuffixName;	
	(*psmf)[WT_OPTION][WT_MAP_LAST_LOT_NUMBER]		= m_szLastLotNumber;			//v4.40T14	//LatticePower
	(*psmf)[WT_OPTION][WT_MAP_LOCAL_LOADMAP]		= m_bEnableLocalLoadMap;		//v3.15T5
	(*psmf)[WT_OPTION][WT_MAP_USER_LOCAL_LOAD]		= m_bUserModeLocalLoadMap;

	if (CMS896AStn::m_bAlwaysLoadMapFileLocalHarddisk && (!m_bBarCodeSearchMap || !m_bLoadMapSearchDeepInFolder))
	{
		m_bEnableLocalLoadMap = TRUE;
	}

	(*psmf)[WT_OPTION][WT_MAP_AUTO_LOAD_BACKUP_MAP]	= m_bEnableAutoLoadBackupMap;

	(*psmf)[WT_OPTION][WT_MAP_SCOLOR_GRADE]			= m_ucChangeStartGrade;
	(*psmf)[WT_OPTION][WT_MAP_ECOLOR_GRADE]			= m_ucChangeEndGrade;
	(*psmf)[WT_OPTION][WT_MAP_TCOLOR_GRADE]			= m_ucChangeTargetGrade;
	//4.53D16
	(*psmf)[WT_OPTION][WT_MAP_HEADER_MATCH_CHECK]	= m_bEnableMatchMapHeader;

	(*psmf)[WT_OPTION][WT_MAP_FIXNAME_READMAP]		= m_bEnableFixMapName;
	(*psmf)[WT_OPTION][WT_MAP_FIXNAME_MAPNAME]		= m_szFixMapName;
	(*psmf)[WT_OPTION][WT_MAP_PREFIX_READMAP]		= m_bEnablePrefix;
	(*psmf)[WT_OPTION][WT_MAP_PREFIX_CHECK]			= m_bEnablePrefixCheck;		//v3.61T1	//NeoNeon
	(*psmf)[WT_OPTION][WT_MAP_SUFFIX_CHECK]			= m_bEnableSuffixCheck;		//v4.36		//SanAn
	(*psmf)[WT_OPTION][WT_MAP_PREFIX_NAME]			= m_szPrefixName;
	(*psmf)[WT_OPTION][WT_MAP_MATCH_FILE_NAME]		= m_bMatchFileName;
	(*psmf)[WT_OPTION][WT_WAFEREND_ELECTECH_3EDL]	= m_bIsWaferEndedFor3EDL;		//v4.45T3

	(*psmf)[WT_OPTION][WT_MAP_CHECK_SEP_GRADE]		= m_bCheckSepGrade;
	(*psmf)[WT_OPTION][WT_MAP_DIE_SHAPE]			= m_ucDieShape;

	(*psmf)[WT_OPTION][WT_MAP_CHECK_REPEAT]			= m_bCheckRepeatMap;
	(*psmf)[WT_OPTION][WT_MAP_CURRMAP_PATH]			= m_szCurrentMapPath;
	(*psmf)[WT_OPTION][WT_MAP_DISABLE_REFDIE]		= m_bDisableRefDie;				//v2.78T1
	(*psmf)[WT_OPTION][WT_CHECK_GD_ON_REFDIE]		= m_bCheckGDOnRefDiePos;		//v2.91T1
	(*psmf)[WT_OPTION][WT_KEEP_LAST_UNLOAD_GRADE]	= m_bKeepLastUnloadGrade;	//v2.91T1
	(*psmf)[WT_ALIGN_DATA][WT_MAP_VALID_MIN_ROW]	= m_lMapValidMinRow;
	(*psmf)[WT_ALIGN_DATA][WT_MAP_VALID_MIN_COL]	= m_lMapValidMinCol;
	(*psmf)[WT_ALIGN_DATA][WT_MAP_VALID_MAX_ROW]	= m_lMapValidMaxRow;
	(*psmf)[WT_ALIGN_DATA][WT_MAP_VALID_MAX_COL]	= m_lMapValidMaxCol;

	m_bEnableSmartWalkInPicknPlace = FALSE;
	m_bEnablePickAndPlace = FALSE;
	(*psmf)[WT_OPTION][WT_ENABLE_PICK_N_PLACE]				= m_bEnablePickAndPlace;
	(*psmf)[WT_OPTION][WT_ENABLE_SMART_WALK_PICK_N_PLLACE]	= m_bEnableSmartWalkInPicknPlace;
	(*psmf)[WT_OPTION][WT_MULTIGRADE_SORT_TO_SINGLEBIN]		= m_bMultiGradeSortingtoSingleBin;	//v4.15T8	//Osram Germany Layer-sort
	(*psmf)[WT_OPTION][WT_TRACEABILITY_DISABLE_LOTID]		= m_bOTraceabilityDisableLotID;		//v3.68T3

	(*psmf)[WT_OPTION][WT_ENABLE_CHECK_LOT_NO]			= m_bEnableCheckLotNo;

	//v2.93T2
	(*psmf)[WT_OPTION][WT_CSRCH_DIE_X]				= m_nCornerHomeDiePosX;
	(*psmf)[WT_OPTION][WT_CSRCH_DIE_Y]				= m_nCornerHomeDiePosY;

	(*psmf)[WT_OPTION][WT_LONGJUMP_DELAY]			= m_lLongJumpDelay;				//v4.10T5
	(*psmf)[WT_OPTION][WT_LONGJUMP_MOTTIME]			= m_lLongJumpMotTime;			//v4.10T5
	
	//Migrate from WaferTable to LastState; changed to under WT_OPTION				//v4.11T1
	(*psmf)[WT_OPTION][WT_EMPTY_LIMIT]				= m_lWaferEmptyLimit;
	(*psmf)[WT_OPTION][WT_EDGE_LIMIT]				= m_lWaferEdgeLimit;
	(*psmf)[WT_OPTION][WT_STREET_LIMIT]				= m_lWaferStreetLimit;
	(*psmf)[WT_OPTION][EJR_NEW_SEQ]					= m_bNewEjectorSequence;		//v2.61T9

	(*psmf)[WT_OPTION][WT_OSRAM_RESORT_MODE]		= m_bOsramResortMode;			//v4.21T3

	(*psmf)[WT_OPTION][WT_DISABLE_WAFERMAP_GRADE_SELECT] = m_bDisableWaferMapGradeSelect;
	(*psmf)[WT_OPTION][WT_USE_WAFER_MAP_CATEGORY] = m_bUseWaferMapCategory;

	// prescan relative code
	SavePrescanDataWf(psmf);

	(*psmf)[WT_ALIGN_DATA][WT_SCN_TOTAL_DIE]		= m_lTotalSCNCount;
	(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_NULL_BIN_IN_MAP]	= m_bCheckNullBinInMap;
	(*psmf)[WT_ALIGN_DATA][WT_MAP_CHECK_HOME_NULL_BIN]		= m_bCheckMapHomeNullBin;
	(*psmf)[WT_ALIGN_DATA][WT_ALIGN_DIE_STEP]		= m_ucAlignStepSize;

	(*psmf)[WT_OUTPUTFILE_OPTION][WT_MAPHEADER_FILE_NAME] = m_szMapHeaderFileName;
	(*psmf)[WT_OUTPUTFILE_OPTION][WT_OUTPUTFORMAT_FILE_NAME] = m_szOutputFormatFileName;


	(*psmf)[WT_SRCH_HOME_OPTION][WT_SRCH_OPTION]		= m_ulAlignOption;
	(*psmf)[WT_SRCH_HOME_OPTION][WT_HOME_MAP_ROW]		= m_lHomeDieMapRow;
	(*psmf)[WT_SRCH_HOME_OPTION][WT_HOME_MAP_COL]		= m_lHomeDieMapCol;
	(*psmf)[WT_SRCH_HOME_OPTION][WT_ORGMAP_ROW_OFFSET]	= m_lOrgMapRowOffset;
	(*psmf)[WT_SRCH_HOME_OPTION][WT_ORGMAP_COL_OFFSET]	= m_lOrgMapColOffset;
	(*psmf)[WT_SRCH_HOME_OPTION][WT_SCNMAP_ROW_OFFSET]	= m_lScnMapRowOffset;
	(*psmf)[WT_SRCH_HOME_OPTION][WT_SCNMAP_COL_OFFSET]	= m_lScnMapColOffset;

	(*psmf)[WT_SRCH_HOME_OPTION][WT_FIND_HOMEDIE_FROMMAP]	= m_bFindHomeDieFromMap;
	(*psmf)[WT_SRCH_HOME_OPTION][WT_ADD_REFER_PITCH_ROW]	= m_lReferPitchRow;
	(*psmf)[WT_SRCH_HOME_OPTION][WT_ADD_REFER_PITCH_COL]	= m_lReferPitchCol;

	(*psmf)[WT_ALIGN_DATA][WT_HOME_PHY_X]			= m_lHomeDieWftPosnX;
	(*psmf)[WT_ALIGN_DATA][WT_HOME_PHY_Y]			= m_lHomeDieWftPosnY;
	if( m_bDisableWL )
	{
		(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_OPTION]		= m_bSearchHomeOption;
		(*psmf)[WT_ALIGN_DATA][WT_CORNER_SRCH_OPTION]	= m_ulCornerSearchOption;
	}
	(*psmf)[WT_ALIGN_DATA][WT_ALIGN_ANGLE_MODE]		= m_lAlignWaferAngleMode_Std;

	(*psmf)[WT_OPTION][WT_UPDATE_MAP_AFTER_SYNMOVE]	= m_bSelOfUpdateMapPos;

	//Wafer Lot File Path, File Name , File Ext into LastState msd
	(*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_CHECK_NAME]	= m_bCheckLotWaferName;
	(*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_FILE_PATH]	= m_szLotInfoFilePath;
	(*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_FILE_NAME]	= m_szLotInfoFileName;
	(*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_FILE_EXT]	= m_szLotInfoFileExt;
	(*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_OPERATOR_ID]	= m_szLotInfoOpID;
	(*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_OUT_WAFER_INFO_FILE_PATH]	=	m_szOutWaferInfoFilePath;

	(*psmf)[WT_DATA][WT_WAFER_ID]			= m_szWaferId;
	(*psmf)[WT_DATA][WT_MAP_NAME_ONLY]		= m_szMapFileName;
	(*psmf)[WT_DATA][WT_MAP_EXTENSION]		= m_szOnlyMapFileFullPath;
	(*psmf)[WT_DATA][WT_MAP_NAME_FULL_PATH]	= m_szMapFileExt1;

	if (((BOOL)(LONG)(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_MAP_DIE_TYPE_CHECK_ENABLE]) && (m_bEnableMapDieTypeCheck == FALSE))
	{
		SetErrorMessage("Disabled Map Die Type Check");
	}

	szPreviousUserDefineDieType = (*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_USER_DEFINE_DIE_TYPE];

	if (szPreviousUserDefineDieType != m_szUserDefineDieType)
	{
		SetErrorMessage("User Define Die Type Change From: " + szPreviousUserDefineDieType + " To: " + m_szUserDefineDieType);
	}
	
	(*psmf)[WT_DATA][WT_PACKAGE_FILE_CHECK] = m_bEnablePackageFileCheck;
	(*psmf)[WT_DATA][WT_PACKAGE_FILE_CHECK_STATUS] = m_bIsPackageFileCheckOk;
	(*psmf)[WT_DATA][WT_MAP_HEADER_CHECK]			= m_bEnableMapHeaderCheck;
	(*psmf)[WT_DATA][WT_MAP_HEADER_CHECK_WAFER_ID]	= m_szMapHeaderCheckWaferId;

	(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_USER_DEFINE_DIE_TYPE] = m_szUserDefineDieType;
	(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_MAP_DIE_TYPE_CHECK_ENABLE] = m_bEnableMapDieTypeCheck;
	(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_MAP_DIE_TYPE_CHECK_STATUS] = m_bIsMapDieTypeCheckOk;

	(*psmf)[WT_MES_CONNECTION][WT_MES_CONNECTION_ENABLE] = m_bEnableMESConnection;
	(*psmf)[WT_MES_CONNECTION][WT_MES_INCOMING_PATH] = m_szMESIncomingPath;
	(*psmf)[WT_MES_CONNECTION][WT_MES_OUTGOING_PATH] = m_szMESOutgoingPath;
	(*psmf)[WT_MES_CONNECTION][WT_MES_TIMEOUT] = m_lMESTimeout;
	

	(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_ENABLE_AUTO_DIE_TYPE_CHECK]		= m_bEnableAutoMapDieTypeCheck;

	for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
	{
		szTemp.Format("Check Cur Value%d", i+1);
		(*psmf)[WT_MAP_DIE_TYPE_CHECK][szTemp] = m_szaMapDieTypeCheckCurValue[i];
		szTemp.Format("Check Status%d", i+1);
		(*psmf)[WT_MAP_DIE_TYPE_CHECK][szTemp] = m_baIsMapDieTypeCheckOk[i];
		szTemp.Format("Check Is Selected%d", i+1);
		(*psmf)[WT_MAP_DIE_TYPE_CHECK][szTemp] = m_baIsSelectMapDieTypeCheck[i];
	}

	(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_SORT_BIN_ITEM]				= m_szSortBinItem;
	(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_CHECK_SORT_BIN_ITEM_STATUS]	= m_bIsCheckSortBinItemOk;
	
	(*psmf)[WT_ALIGN_DATA][WT_ENABLE_2D_BARCODE]	= m_bEnableTwoDimensionsBarcode;	//v3.33T1
	(*psmf)[WT_ALIGN_DATA][WT_2D_BARCODE_CHECK_99]	= m_b2DBarCodeCheckGrade99;			//v3.59

	(*psmf)[WT_ALIGN_DATA][WT_LOAD_CURRENT_MAP_STATUS] = m_bLoadCurrentMapStatus;

	(*psmf)[WT_OPTION][WT_SLOW_PROFILE_TRIGGER]			= m_lSlowProfileTrigger;

	if (CMS896AStn::m_bEnableWaferSizeSelect == TRUE)
	{
		(*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][WT_LIMIT_TEMPLATE_SEL]	= m_ucWaferLimitTemplateNoSel;
	}

    // close LastState file
    pUtl->UpdateLastState("WT: SaveData");
    pUtl->CloseLastState("WT: SaveData");

	//Update SRAM variable

	(*m_psmfSRam)["WaferTable"]["Wafer Table No Die X Coordinate"]		= m_lWTNoDiePosX;
	(*m_psmfSRam)["WaferTable"]["Wafer Table No Die Y Coordinate"]		= m_lWTNoDiePosY;

	(*m_psmfSRam)["MS896A"]["Map Rotation"]			= m_ucMapRotation; // 4.50D10
	(*m_psmfSRam)["MS896A"]["Table Rotation"]		= m_ucAutoWaferRotation;
	(*m_psmfSRam)["MS896A"]["Flip Vertical"]		= m_bMapVertFlip;
	(*m_psmfSRam)["MS896A"]["Flip Horizontal"]		= m_bMapHoriFlip;

	(*m_psmfSRam)["MS896A"]["MapFilePath"]			= m_szMapFilePath;
	(*m_psmfSRam)["MS896A"]["FindGlobalTheta"]		= m_bEnableGlobalTheta;
	(*m_psmfSRam)["MS896A"]["UserDefineDieType"]	= m_szUserDefineDieType;
	(*m_psmfSRam)["MS896A"]["BackupMapFilePath"]	= m_szBackupMapPath;
	(*m_psmfSRam)["MS896A"]["BackupMapFileExt"]		= m_szBackupMapExtension;
	(*m_psmfSRam)["Wafer Table Options"]["SmartWalkInPicknPlace"]	= m_bEnableSmartWalkInPicknPlace;
	(*m_psmfSRam)["Wafer Table Options"]["MultiGradeSortToSingleBin"]	= m_bMultiGradeSortingtoSingleBin;	//v4.15T8
	(*m_psmfSRam)["Wafer Table Options"]["Min Grade Count"]			= m_ulMinGradeCount;

	(*m_psmfSRam)["Wafer Table Options"]["Min Grade Bin"]			= m_ulMinGradeBinNo;
	(*m_psmfSRam)["WaferTable"]["SuffixName"]					    = m_szSuffixName;

	(*m_psmfSRam)["MS896A"]["WaferLot Filename"]	= m_szLotInfoFileName;
	(*m_psmfSRam)["WaferTable"]["LongJumpDelay"]	= m_lLongJumpDelay;			//v4.16T4
	(*m_psmfSRam)["WaferTable"]["LongJumpMotion"]	= m_lLongJumpMotTime;		//v4.16T4
	(*m_psmfSRam)["Ejector"]["NewEjSequence"]		= m_bNewEjectorSequence;	//v4.49A7	//QC ParaList
	//v4.50A2
	(*m_psmfSRam)["WaferTable"]["GTPosnX"]			= m_lNewGTPositionX;
	(*m_psmfSRam)["WaferTable"]["GTPosnY"]			= m_lNewGTPositionY;
	(*m_psmfSRam)["WaferTable"]["GTPosnX2"]			= m_lNewGTPositionX2;
	(*m_psmfSRam)["WaferTable"]["GTPosnY2"]			= m_lNewGTPositionY2;
	(*m_psmfSRam)["WaferTable"]["GTPosnUse2"]			= m_bNewGTUse2;

	(*m_psmfSRam)["WaferTable"]["Fix Map Name"]		= m_szFixMapName;
	CMSPrescanUtility::Instance()->SetPickAndPlaceMode(IsEnablePNP());
	pApp->SetEqTimeMapDieType(m_szUserDefineDieType);		//v4.49A1	//SanAn

	m_bSortOuterHmi = (m_lSubSortMode==0) || m_bRegionPrescan;

	return TRUE;
}


BOOL CWaferTable::LoadData(VOID)
{
	CString szTemp;

    CMSPrescanUtility  *pPUtl = CMSPrescanUtility::Instance();
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	//CString szTemp;
	LONG i = 0;

	if (pUtl->LoadLastState("WT: LoadData") == FALSE) 
	{
		return FALSE;
	}

	psmf = pUtl->GetLastStateFile("WT: LoadData");
	if ( psmf == NULL )
	{
		return FALSE;
	}

	m_pWaferMapManager->LoadData(&(*psmf)[WT_MAP_DATA]);
	m_pEjPinCleanRegion->LoadData(psmf);
	//v4.24T9	//ES101
	m_nWTInUse			= (INT)(LONG)(*psmf)[WT_OPTION][WT_TABLE_INUSE];
	if ( IsESDualWT()==FALSE )
	{
		m_nWTInUse		= 0;
	}
	m_lJsWftInUse = IsWT2InUse();

	m_szWaferMapFormat	= (*psmf)[WT_MAP_DATA][WT_MAP_FORMAT];
	// Alignment Data

//	m_lLHSIndexCount	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_LHS_INDEX];
//	m_lRHSIndexCount	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_RHS_INDEX];
	m_dMoveBackAngle	= (DOUBLE)(*psmf)[WT_ALIGN_DATA][WT_MOVEBACK_ANGLE];		//v2.82

	m_bNewGTPosition	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION];
	m_lNewGTPositionX	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION_X];
	m_lNewGTPositionY	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION_Y];
	m_lNewGTPositionX2	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION_X2];
	m_lNewGTPositionY2	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_GT_NEW_POSITION_Y2];
	m_bNewGTUse2			= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA]["NEWGTUSE2"];

	//andrewng //2020-0902
	m_lUnloadPhyPosX	= (LONG)(*psmf)[WT_ALIGN_DATA]["Unload Position X"];
	m_lUnloadPhyPosY	= (LONG)(*psmf)[WT_ALIGN_DATA]["Unload Position Y"];

	m_lWaferEmptyLimit		= (*psmf)[WT_OPTION][WT_EMPTY_LIMIT];
	m_lWaferEdgeLimit		= (*psmf)[WT_OPTION][WT_EDGE_LIMIT];
	m_lWaferStreetLimit		= (*psmf)[WT_OPTION][WT_STREET_LIMIT];

	m_bNewEjectorSequence	= (BOOL)(LONG)(*psmf)[WT_OPTION][EJR_NEW_SEQ];		//v2.61T9

	m_bDiePitchCheck	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_DIE_PITCH_CHECK];
	m_bAbsDiePitchCheck	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ABS_DIE_PITCH_CHECK];	//v2.78T2
	m_lPredScanPitchTolX= (LONG)(*psmf)[WT_OPTION][WT_PREDSCAN_PITCHTOL_X];
	m_lPredScanPitchTolY= (LONG)(*psmf)[WT_OPTION][WT_PREDSCAN_PITCHTOL_Y];
	m_lPitchX_Tolerance	= (LONG)(*psmf)[WT_OPTION][WT_PITCH_X_TOLERANCE];
	m_lPitchY_Tolerance	= (LONG)(*psmf)[WT_OPTION][WT_PITCH_Y_TOLERANCE];
	m_lCorMovePitchTolX	= (LONG)(*psmf)[WT_OPTION][WT_COR_MOVE_PITCHTOL_X];
	m_lCorMovePitchTolY	= (LONG)(*psmf)[WT_OPTION][WT_COR_MOVE_PITCHTOL_Y];
	m_lCorBackPitchTolX	= (LONG)(*psmf)[WT_OPTION][WT_COR_BACK_PITCHTOL_X];
	m_lCorBackPitchTolY	= (LONG)(*psmf)[WT_OPTION][WT_COR_BACK_PITCHTOL_Y];
	m_lWaitEjectorUp	= (*psmf)[WT_OPTION][WT_WAITEJUP]; 
	m_lPLLMWaferRow		= (LONG)(*psmf)[WT_OPTION][WT_PLLM_WAFER_ROW];
	m_lPLLMWaferCol		= (LONG)(*psmf)[WT_OPTION][WT_PLLM_WAFER_COL];

	m_lCorBackPitchTolX	= m_lCorMovePitchTolX/2;
	m_lCorBackPitchTolY	= m_lCorMovePitchTolY/2;
	m_lRunPredScanPitchTolX		= m_lPredScanPitchTolX;
	m_lRunPredScanPitchTolY		= m_lPredScanPitchTolY;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( (pApp->GetCustomerName()=="Cree") ||
		(pApp->GetCustomerName()=="Semitek") )
	{
		m_bDiePitchCheck = TRUE;
	}

	m_bEnableXCheck			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_DIE_CROSS_CHECK];
	m_bPickCrossCheckDie	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PICK_CROSS_CHECK_DIE];	//v3.57T1
	m_lXCheckToleranceX		= (LONG)(*psmf)[WT_OPTION][WT_XCHECK_TOLERANCEX];
	m_lXCheckToleranceY		= (LONG)(*psmf)[WT_OPTION][WT_XCHECK_TOLERANCEY];

	m_lRefCrossRow		= (LONG)(*psmf)[WT_OPTION][WT_REF_CROSS_ROW];
	m_lRefCrossCol		= (LONG)(*psmf)[WT_OPTION][WT_REF_CROSS_COL];
	m_lRefCrossWidth	= (LONG)(*psmf)[WT_OPTION][WT_REF_CROSS_WIDTH];

	m_ucMapRotation		= (UCHAR)(LONG)(*psmf)[WT_OPTION][WT_MAP_ROTATION];
	m_ucAutoWaferRotation = (UCHAR)(LONG)(*psmf)[WT_OPTION][WT_AUTO_WAFER_ROTATION];
	m_ucAutoBCRotation	= (UCHAR)(LONG)(*psmf)[WT_OPTION][WT_AUTO_BC_ROTATION];
	m_ucMapEdgeSize		= (UCHAR)(LONG)(*psmf)[WT_OPTION][WT_MAP_EDGE_SIZE];
	m_bMapHoriFlip		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_HORI_FLIP];
	m_bMapVertFlip		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_VERT_FLIP];
	m_bShowFirstDie		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_SHOW_FIRST_DIE];
	m_lSortingMode		= (LONG)(*psmf)[WT_OPTION][WT_MAP_SORT_MODE];
	m_ulMinGradeCount	= (ULONG)(*psmf)[WT_OPTION][WT_MAP_MIN_GRADE_COUNT];
	m_ulMinGradeBinNo	= (ULONG)(*psmf)[WT_OPTION][WT_MAP_MIN_GRADE_BIN];			//v4.38T7
	m_ucMapPickMode		= (UCHAR)(LONG)(*psmf)[WT_OPTION][WT_MAP_PICK_MODE];		//v2.78T1
	m_ulMapDieStep		= (ULONG)(*psmf)[WT_OPTION][WT_MAP_DIE_STEP];				//v2.78T1
	m_szMapTour			= (*psmf)[WT_OPTION][WT_MAP_TOUR];
	m_szMapPath			= (*psmf)[WT_OPTION][WT_MAP_PATH_SELECTION];
	m_bMultiWaferSupport= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_MULTI_WAFER];		//v2.91T1
	m_bEnableGlobalTheta= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ENABLE_GLOBAL_THETA];	//v2.91T1
	m_bAutoSrchHome		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_AUTO_SEARCH_HOME];
	//if( pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) )
	//{
	//	m_bAutoSrchHome = TRUE;
	//}
	if (IsLoadingPortablePKGFile() == TRUE && IsLoadingPKGFile() == TRUE)
	{
	}
	else
	{
		m_lWTNoDiePosX			= (*psmf)[WT_OPTION][WT_No_Die_COOR_X];
		m_lWTNoDiePosY			= (*psmf)[WT_OPTION][WT_No_Die_COOR_Y];
	}

	m_bEnableSmartWalk	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_SMART_WALK];		//v2.99T1	
	m_bEnableSWalk1stDieFinder	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_SMART_1STDIE_FINDER];	//v3.15T1
	m_ucSpiralPickDirection		= (UCHAR)(LONG)(*psmf)[WT_OPTION][WT_MAP_SPIRAL_SEARCH_DIR];	//v2.95T1
	m_bStartPoint		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_START_POINT];		//v3.01T1
	m_bStartPoint = FALSE;
	m_lSpecialRefDieGrade = (LONG)(*psmf)[WT_OPTION][WT_MAP_SPECIAL_REFDIE_GRADE];	//v3.24T1
	pPUtl->SetSpcReferGrade(m_lSpecialRefDieGrade);

	m_bBlkFuncEnable	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_BLK_FUNC_ENABLE];	//v3.30T1
	m_bUseMultiCorOffset	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MULTI_COR_OFFSET];	//v3.44		//ShowaDenko
	m_bOTraceabilityDisableLotID = (BOOL)(LONG)(*psmf)[WT_OPTION][WT_TRACEABILITY_DISABLE_LOTID];	//v3.68T3	//PLLM

	m_lLongJumpDelay	= (*psmf)[WT_OPTION][WT_LONGJUMP_DELAY];					//v4.10T5
	m_lLongJumpMotTime	= (*psmf)[WT_OPTION][WT_LONGJUMP_MOTTIME];					//v4.10T5

	m_bEnableCheckLotNo	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ENABLE_CHECK_LOT_NO];

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	pSRInfo->m_ulSubRows	= (LONG)(*psmf)[WT_OPTION][WT_MAP_SUB_ROWS];
	pSRInfo->m_ulSubCols	= (LONG)(*psmf)[WT_OPTION][WT_MAP_SUB_COLS];
	m_lSubSortMode			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_SORT_BY_GRADE];

	m_bIgnoreRegion		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_IGNORE_REGION];
	m_ulSelectRegionULX[1] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION1_ULX];	//v3.49T5
	m_ulSelectRegionULY[1] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION1_ULY];
	m_ulSelectRegionLRX[1] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION1_LRX];
	m_ulSelectRegionLRY[1] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION1_LRY];
	m_ulSelectRegionULX[2] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION2_ULX];	//v3.49T5
	m_ulSelectRegionULY[2] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION2_ULY];
	m_ulSelectRegionLRX[2] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION2_LRX];
	m_ulSelectRegionLRY[2] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION2_LRY];
	m_ulSelectRegionULX[3] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION3_ULX];	//v3.49T5
	m_ulSelectRegionULY[3] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION3_ULY];
	m_ulSelectRegionLRX[3] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION3_LRX];
	m_ulSelectRegionLRY[3] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION3_LRY];
	m_ulSelectRegionULX[4] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION4_ULX];	//v3.49T5
	m_ulSelectRegionULY[4] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION4_ULY];
	m_ulSelectRegionLRX[4] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION4_LRX];
	m_ulSelectRegionLRY[4] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION4_LRY];
	m_ulSelectRegionULX[5] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION5_ULX];	//v3.49T5
	m_ulSelectRegionULY[5] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION5_ULY];
	m_ulSelectRegionLRX[5] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION5_LRX];
	m_ulSelectRegionLRY[5] = (LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGIONS][WT_MAP_SELECT_REGION5_LRY];

//Load CP Daily Check;
	m_ucSelectChangeGrade = (UCHAR)(LONG)(*psmf)[WT_OPTION][WT_MAP_SELECT_REGION_GRADE];

	m_bCheckSepGrade	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_CHECK_SEP_GRADE];
	m_bKeepLastUnloadGrade = (BOOL)(LONG)(*psmf)[WT_OPTION][WT_KEEP_LAST_UNLOAD_GRADE];	//v2.91T1

	m_bOsramResortMode	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_OSRAM_RESORT_MODE];			//v4.21T3
	if (pApp->GetCustomerName() != "OSRAM")
		m_bOsramResortMode = FALSE;

	m_bDisableWaferMapGradeSelect = (BOOL)(LONG)(*psmf)[WT_OPTION][WT_DISABLE_WAFERMAP_GRADE_SELECT];
	m_bUseWaferMapCategory = (BOOL)(LONG)(*psmf)[WT_OPTION][WT_USE_WAFER_MAP_CATEGORY];

	// prescan relative code
	LoadPrescanDataWf(psmf);

	m_lTotalSCNCount	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_TOTAL_DIE];
	m_bCheckNullBinInMap = (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_SCN_CHECK_NULL_BIN_IN_MAP];
	m_bCheckMapHomeNullBin = (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_MAP_CHECK_HOME_NULL_BIN];
	m_ucAlignStepSize	= (UCHAR)(*psmf)[WT_ALIGN_DATA][WT_ALIGN_DIE_STEP];

	m_bEnableBarcodeLoadMap	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_BARCODE_READMAP];
	m_bBarCodeSearchMap		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_BARCODE_SEARCH_MAP];		//v3.54T1
	m_bBarCodeSearchFolder	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_SEARCH_SUB_FOLDER];
	m_szSearchFolderName	= (*psmf)[WT_OPTION][WT_MAP_SEARCH_FOLDER_NAME];
	m_bEnableSuffix			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_ENABLE_SUFFIX];
	m_szSuffixName			= (*psmf)[WT_OPTION][WT_MAP_SUFFIX_NAME];	
	m_szLastLotNumber		= (*psmf)[WT_OPTION][WT_MAP_LAST_LOT_NUMBER];
	m_bEnableLocalLoadMap	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_LOCAL_LOADMAP];
	m_bUserModeLocalLoadMap	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_USER_LOCAL_LOAD];
	
	if (CMS896AStn::m_bAlwaysLoadMapFileLocalHarddisk && (!m_bBarCodeSearchMap || !m_bLoadMapSearchDeepInFolder))
	{
		m_bEnableLocalLoadMap = TRUE;
	}

	m_bEnableAutoLoadBackupMap	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_AUTO_LOAD_BACKUP_MAP];
	m_bBarCodeSearchPartMap		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_SEARCH_PART_MAP];
	
	if( m_bBarCodeSearchMap )
	{
		m_bBarCodeSearchFolder = FALSE;
	}

	//Get Wafermap header & ouput format file name	
	m_szMapHeaderFileName	 = (*psmf)[WT_OUTPUTFILE_OPTION][WT_MAPHEADER_FILE_NAME];
	m_szOutputFormatFileName = (*psmf)[WT_OUTPUTFILE_OPTION][WT_OUTPUTFORMAT_FILE_NAME];

	//Load wafer map grade color option 
	m_ucChangeStartGrade	= (UCHAR)(*psmf)[WT_OPTION][WT_MAP_SCOLOR_GRADE];
	m_ucChangeEndGrade		= (UCHAR)(*psmf)[WT_OPTION][WT_MAP_ECOLOR_GRADE];
	m_ucChangeTargetGrade	= (UCHAR)(*psmf)[WT_OPTION][WT_MAP_TCOLOR_GRADE];

	//4.53D16 Map Header Match
	m_bEnableMatchMapHeader = (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_HEADER_MATCH_CHECK];

	//Fix map name
	m_bEnableFixMapName		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_FIXNAME_READMAP];
	m_szFixMapName			= (*psmf)[WT_OPTION][WT_MAP_FIXNAME_MAPNAME];

	//Prefix name
	m_bEnablePrefix			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_PREFIX_READMAP];
	m_bEnablePrefixCheck	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_PREFIX_CHECK];		//v3.61T1	//NeoNeon
	m_szPrefixName			= (*psmf)[WT_OPTION][WT_MAP_PREFIX_NAME];
	m_bEnableSuffixCheck	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_SUFFIX_CHECK];		//v4.36		//SanAn

	//Match file name
	m_bMatchFileName		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_MATCH_FILE_NAME];

	m_bIsWaferEndedFor3EDL	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_WAFEREND_ELECTECH_3EDL];	//v4.45T3

	//Search Home die option
	m_ulAlignOption			= (ULONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_SRCH_OPTION];	
	m_lHomeDieMapRow		= (LONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_HOME_MAP_ROW];		
	m_lHomeDieMapCol		= (LONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_HOME_MAP_COL];		
	m_lOrgMapRowOffset		= (LONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_ORGMAP_ROW_OFFSET];
	m_lOrgMapColOffset		= (LONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_ORGMAP_COL_OFFSET];
	m_lScnMapRowOffset		= (LONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_SCNMAP_ROW_OFFSET];
	m_lScnMapColOffset		= (LONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_SCNMAP_COL_OFFSET];

	m_bFindHomeDieFromMap	= (BOOL)(LONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_FIND_HOMEDIE_FROMMAP];

	m_lReferPitchRow		= (LONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_ADD_REFER_PITCH_ROW];
	m_lReferPitchCol		= (LONG)(*psmf)[WT_SRCH_HOME_OPTION][WT_ADD_REFER_PITCH_COL];

	m_lHomeDieWftPosnX			= (LONG)(*psmf)[WT_ALIGN_DATA][WT_HOME_PHY_X];
	m_lHomeDieWftPosnY			= (LONG)(*psmf)[WT_ALIGN_DATA][WT_HOME_PHY_Y];
	if( m_bDisableWL )
	{
		m_bSearchHomeOption		= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_AUTO_HOME_OPTION];
		m_ulCornerSearchOption	= (ULONG)(*psmf)[WT_ALIGN_DATA][WT_CORNER_SRCH_OPTION];
	}
	m_lAlignWaferAngleMode_Std	= (LONG)(*psmf)[WT_ALIGN_DATA][WT_ALIGN_ANGLE_MODE];

	//Die shape
	m_ucDieShape			= (*psmf)[WT_OPTION][WT_MAP_DIE_SHAPE];

	//Repeat load map
	m_bCheckRepeatMap		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_CHECK_REPEAT];
	m_szCurrentMapPath		= (*psmf)[WT_OPTION][WT_MAP_CURRMAP_PATH];
	m_bDisableRefDie		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MAP_DISABLE_REFDIE];		//v2.78T1
	m_bCheckGDOnRefDiePos	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_CHECK_GD_ON_REFDIE];		//v2.91T1
	m_bSelOfUpdateMapPos	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_UPDATE_MAP_AFTER_SYNMOVE];

	m_lMapValidMinRow		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_MAP_VALID_MIN_ROW];
	m_lMapValidMinCol		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_MAP_VALID_MIN_COL];
	m_lMapValidMaxRow		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_MAP_VALID_MAX_ROW];
	m_lMapValidMaxCol		= (LONG)(*psmf)[WT_ALIGN_DATA][WT_MAP_VALID_MAX_COL];

	// Load Lot File Path, File Ext and File Name
	m_bCheckLotWaferName	= (BOOL)(LONG)(*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_CHECK_NAME];	
	m_szLotInfoFilePath		= (*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_FILE_PATH];
	m_szLotInfoFileName		= (*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_FILE_NAME];
	m_szLotInfoFileExt		= (*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_FILE_EXT];
	m_szLotInfoOpID			= (*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_LOT_OPERATOR_ID];
	m_szOutWaferInfoFilePath= (*psmf)[WT_DATA][WT_WAFER_LOT_INFO][WT_OUT_WAFER_INFO_FILE_PATH];

	m_szGradeDisplayMode	= (*psmf)[WT_OPTION][WT_MAP_GRADE_DISPLAY_MODE];
	m_lGradeDisplayMode		= (LONG)(*psmf)[WT_OPTION][WT_MAP_GRADE_DISPLAY_MODE_SELECT];

	//Get file path & extension
	m_szMapFilePath	= (*psmf)[WT_OPTION][WT_MAP_PATH];
	m_szMapFileExtension = (*psmf)[WT_OPTION][WT_MAP_EXT];

	m_szBackupMapPath		= (*psmf)[WT_OPTION][WT_BACKUP_MAP_PATH];
	m_szBackupMapExtension	= (*psmf)[WT_OPTION][WT_BACKUP_MAP_EXT];

	m_szUploadBarcodeIDFilePath = (*psmf)[WT_OPTION][WT_UPLOAD_BARCODE_ID_PATH];

	if ( m_szMapFilePath.IsEmpty() == FALSE )
	{
		m_WaferMapWrapper.SetInitialPath(m_szMapFilePath);
	}

	if ( m_szMapFileExtension.IsEmpty() == FALSE )
	{
		m_WaferMapWrapper.SetInitialExt(m_szMapFileExtension);
	}

	//v2.93T2
	m_nCornerHomeDiePosX	= (LONG)(*psmf)[WT_OPTION][WT_CSRCH_DIE_X];
	m_nCornerHomeDiePosY	= (LONG)(*psmf)[WT_OPTION][WT_CSRCH_DIE_Y];

	m_szWaferId				= (*psmf)[WT_DATA][WT_WAFER_ID];
	m_szMapFileName			= (*psmf)[WT_DATA][WT_MAP_NAME_ONLY];
	m_szOnlyMapFileFullPath	= (*psmf)[WT_DATA][WT_MAP_EXTENSION];
	m_szMapFileExt1			= (*psmf)[WT_DATA][WT_MAP_NAME_FULL_PATH];

	m_szUserDefineDieType	= (*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_USER_DEFINE_DIE_TYPE];
	m_bEnableMapDieTypeCheck = (BOOL)(LONG)(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_MAP_DIE_TYPE_CHECK_ENABLE];
	m_bIsMapDieTypeCheckOk	= (BOOL)(LONG)(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_MAP_DIE_TYPE_CHECK_STATUS];

	//Auto Map Die Type Check (for Ubilux)		//v3.31T1
	
	m_bEnableAutoMapDieTypeCheck	= (BOOL)(LONG)(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_ENABLE_AUTO_DIE_TYPE_CHECK];

	m_bEnablePackageFileCheck = (BOOL)(LONG)(*psmf)[WT_DATA][WT_PACKAGE_FILE_CHECK];
	m_bIsPackageFileCheckOk = (BOOL)(LONG)(*psmf)[WT_DATA][WT_PACKAGE_FILE_CHECK_STATUS];

	m_bEnableMapHeaderCheck		= (BOOL)(LONG)(*psmf)[WT_DATA][WT_MAP_HEADER_CHECK];
	m_szMapHeaderCheckWaferId	= (*psmf)[WT_DATA][WT_MAP_HEADER_CHECK_WAFER_ID];

	//alex
	for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
	{
		szTemp.Format("Check Cur Value%d", i+1);
		m_szaMapDieTypeCheckCurValue[i] = (*psmf)[WT_MAP_DIE_TYPE_CHECK][szTemp];
		szTemp.Format("Check Status%d", i+1);
		m_baIsMapDieTypeCheckOk[i]		= (BOOL)(LONG)(*psmf)[WT_MAP_DIE_TYPE_CHECK][szTemp];
		szTemp.Format("Check Is Selected%d", i+1);
		m_baIsSelectMapDieTypeCheck[i]	= (BOOL)(LONG)(*psmf)[WT_MAP_DIE_TYPE_CHECK][szTemp];
	}

	m_szSortBinItem				= (*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_SORT_BIN_ITEM];
	m_bIsCheckSortBinItemOk		= (BOOL)(LONG)(*psmf)[WT_MAP_DIE_TYPE_CHECK][WT_CHECK_SORT_BIN_ITEM_STATUS];

	m_bEnableTwoDimensionsBarcode	= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_ENABLE_2D_BARCODE];		//v3.33T1
	m_b2DBarCodeCheckGrade99		= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_2D_BARCODE_CHECK_99];	//v3.59

	m_bEnablePickAndPlace			= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ENABLE_PICK_N_PLACE];
	m_bEnableSmartWalkInPicknPlace	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_ENABLE_SMART_WALK_PICK_N_PLLACE];

	m_bMultiGradeSortingtoSingleBin	= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_MULTIGRADE_SORT_TO_SINGLEBIN];	//v4.15T8	//Osram Germany Layer-sort
	if (!pApp->GetFeatureStatus(MS896A_FUNC_MULTIGRADE_SORT_TO_SINGLEBIN))
	{
		m_bMultiGradeSortingtoSingleBin = FALSE;
	}

	m_bEnableSmartWalkInPicknPlace = FALSE;
	m_bEnablePickAndPlace = FALSE;
	m_bLoadCurrentMapStatus			= (BOOL)(LONG)(*psmf)[WT_ALIGN_DATA][WT_LOAD_CURRENT_MAP_STATUS];

	m_lSlowProfileTrigger			=	(LONG)(*psmf)[WT_OPTION][WT_SLOW_PROFILE_TRIGGER];

	// MES Connection Setting
	m_bEnableMESConnection = (BOOL)(LONG)(*psmf)[WT_MES_CONNECTION][WT_MES_CONNECTION_ENABLE];
	m_szMESIncomingPath =	(*psmf)[WT_MES_CONNECTION][WT_MES_INCOMING_PATH];
	m_szMESOutgoingPath =	(*psmf)[WT_MES_CONNECTION][WT_MES_OUTGOING_PATH];
	m_lMESTimeout		=	(LONG)(*psmf)[WT_MES_CONNECTION][WT_MES_TIMEOUT];
	m_ucWaferLimitTemplateNoSel = (*psmf)[WT_PROCESS_DATA][WT_WAFER_LIMIT_TEMPLATE][WT_LIMIT_TEMPLATE_SEL];


	pUtl->CloseLastState("WT: LoadData");		//v4.53A9


	if (!pApp->GetFeatureStatus(MS896A_FUNC_2D_BARCODE_CHECK))
	{
		m_bEnableTwoDimensionsBarcode = FALSE;
		m_b2DBarCodeCheckGrade99 = FALSE;
	}
	
	if( pApp->GetCustomerName()=="Huga" ||	pApp->GetCustomerName()=="FATC" )
	{
		m_bPrescanBackupMap = TRUE;
	}

	// set the MES parameters
	CMESConnector::Instance()->EnableMESConnector(m_bEnableMESConnection);
	CMESConnector::Instance()->SetMESIncomingPath(m_szMESIncomingPath);
	CMESConnector::Instance()->SetMESOutgoingPath(m_szMESOutgoingPath);
	CMESConnector::Instance()->SetMESTimeout(m_lMESTimeout);

	CMSPrescanUtility::Instance()->SetPickAndPlaceMode(IsEnablePNP());
	
	//Update SRAM variable
	(*m_psmfSRam)["MS896A"]["Map Rotation"]							= m_ucMapRotation; // 4.50D10
	(*m_psmfSRam)["MS896A"]["Table Rotation"]						= m_ucAutoWaferRotation;
	(*m_psmfSRam)["MS896A"]["Flip Vertical"]						= m_bMapVertFlip;
	(*m_psmfSRam)["MS896A"]["Flip Horizontal"]						= m_bMapHoriFlip;
	(*m_psmfSRam)["MS896A"]["FindGlobalTheta"]						= m_bEnableGlobalTheta;
	(*m_psmfSRam)["MS896A"]["UserDefineDieType"]					= m_szUserDefineDieType;
	(*m_psmfSRam)["Wafer Table Options"]["SmartWalkInPicknPlace"]	= m_bEnableSmartWalkInPicknPlace;
	(*m_psmfSRam)["Wafer Table Options"]["MultiGradeSortToSingleBin"]	= m_bMultiGradeSortingtoSingleBin;	//v4.15T8
	(*m_psmfSRam)["Wafer Table Options"]["Min Grade Count"]			= m_ulMinGradeCount;
	(*m_psmfSRam)["Wafer Table Options"]["Min Grade Bin"]			= m_ulMinGradeBinNo;
	(*m_psmfSRam)["MS896A"]["WaferLot Filename"]					= m_szLotInfoFileName;
	(*m_psmfSRam)["WaferTable"]["LongJumpDelay"]					= m_lLongJumpDelay;			//v4.16T4
	(*m_psmfSRam)["WaferTable"]["LongJumpMotion"]					= m_lLongJumpMotTime;		//v4.16T4
	(*m_psmfSRam)["WaferTable"]["SuffixName"]					    = m_szSuffixName;			//4.51
	(*m_psmfSRam)["WaferTable"]["GTPosnX"]							= m_lNewGTPositionX;
	(*m_psmfSRam)["WaferTable"]["GTPosnY"]							= m_lNewGTPositionY;
	(*m_psmfSRam)["WaferTable"]["GTPosnX2"]							= m_lNewGTPositionX2;
	(*m_psmfSRam)["WaferTable"]["GTPosnY2"]							= m_lNewGTPositionY2;
	(*m_psmfSRam)["WaferTable"]["GTPosnUse2"]							= m_bNewGTUse2;
	(*m_psmfSRam)["MS896A"]["MapFilePath"]							= m_szMapFilePath;
	(*m_psmfSRam)["MS896A"]["MapFileExt"]							= m_szMapFileExtension;
	(*m_psmfSRam)["MS896A"]["BackupMapFilePath"]					= m_szBackupMapPath;
	(*m_psmfSRam)["MS896A"]["BackupMapFileExt"]						= m_szBackupMapExtension;
	(*m_psmfSRam)["WaferTable"]["WaferLotInfoFilePath"]				= m_szLotInfoFilePath;
	(*m_psmfSRam)["Ejector"]["NewEjSequence"]						= m_bNewEjectorSequence;	//v4.49A7	//QC ParaList
	(*m_psmfSRam)["WaferTable"]["Fix Map Name"]						= m_szFixMapName;
	(*m_psmfSRam)["WaferTable"]["Wafer Table No Die X Coordinate"]		= m_lWTNoDiePosX;
	(*m_psmfSRam)["WaferTable"]["Wafer Table No Die Y Coordinate"]		= m_lWTNoDiePosY;

	pApp->SetEqTimeMapDieType(m_szUserDefineDieType);		//v4.49A1	//SanAn

	if ( m_ucAlignStepSize < 1 )
	{
		m_ucAlignStepSize = 1;
	}

	//v4.11T1	//Re-position to here from ()
	//check Empty, hole & edge limit
	if ( m_lWaferEmptyLimit <= 0 )
	{
		m_lWaferEmptyLimit = 1;
	}
	if ( m_lWaferEdgeLimit <= 0 )
	{
		m_lWaferEdgeLimit = 1;
	}
	if ( m_lWaferStreetLimit <= 0 )
	{
		m_lWaferStreetLimit = 1;
	}

	//Restore Output File setting
	RestoreOutputFileSetting();

	m_bSortOuterHmi = (m_lSubSortMode==0) || m_bRegionPrescan;
	return TRUE;
}


VOID CWaferTable::SetLoadMapDateAndTime(int nYear, int nMonth, int nDay, int nHour,
										   int nMinute, int nSecond)
{
	int *pnTemp;
	CString szHour, szMinute, szSecond, szLoadMapDate, szLoadMapTime;
	
	CTime theTime(CTime::GetCurrentTime());

	if ( m_pvNVRAM != NULL )
	{
		//Write to non-volatile RAM
		pnTemp = (int*)((unsigned long)m_pvNVRAM + glNVRAM_WaferTable_Start);
		*(pnTemp) = nYear;
		*(pnTemp + sizeof(int)) = nMonth;
		*(pnTemp + 2 * sizeof(int)) = nDay;
		*(pnTemp + 3 * sizeof(int)) = nHour;
		*(pnTemp + 4 * sizeof(int)) = nMinute;
		*(pnTemp + 5 * sizeof(int)) = nSecond;
	}

	szLoadMapDate.Format("%d-%d-%d", nYear, nMonth, nDay);

	nHour = theTime.GetHour();
	szHour.Format("%d", nHour);
	if (nHour < 10)
		szHour = "0" + szHour;

	nMinute = theTime.GetMinute();
	szMinute.Format("%d", nMinute);
	if (nMinute < 10)
		szMinute = "0" + szMinute;

	nSecond = theTime.GetSecond();
	szSecond.Format("%d", nSecond);
	if (nSecond < 10)
		szSecond = "0" + szSecond;

	szLoadMapTime = szHour + ":" + szMinute + ":" + szSecond;

	//Write to SRam
	(*m_psmfSRam)["WaferTable"]["Load Map Date"] = szLoadMapDate;
	(*m_psmfSRam)["WaferTable"]["Load Map Time"] = szLoadMapTime;

} //end SetLoadMapDateAndTime



//////////////////////////////////////////////////////
//	Alert & Log reference die status into a tmp file
//////////////////////////////////////////////////////

LONG CWaferTable::AlertRefDieStatus(VOID)
{
	BOOL bRefDieCheck = FALSE;
	LONG lRefDieLearnt = 0;
	LONG lReply = WT_CHK_REFDIE_OK;
	long lUserX = 0; 
	long lUserY = 0;
	unsigned long ulInternalRow;
	unsigned long ulInternalCol;
	LONG lDieType = 1;	
	CString szDieStatus = "Defect";

	FILE *fp = NULL;	//Klocwork	

	if (CMS896AStn::m_bIfGenRefDieStatusFile == TRUE)
	{
		//Convert back to user map row & col
		ulInternalRow	= (*m_psmfSRam)["DieInfo"]["RefDie"]["Y"];
		ulInternalCol	= (*m_psmfSRam)["DieInfo"]["RefDie"]["X"];
		lDieType		= (*m_psmfSRam)["DieInfo"]["RefDie"]["Result"]; 
	
		ConvertAsmToOrgUser(ulInternalRow, ulInternalCol, lUserX, lUserY);

		errno_t nErr = fopen_s(&fp, "_trefstatus", "r");
		if ((nErr != 0) || (fp == NULL))
		{
			// file is not exists, write header information
			nErr = fopen_s(&fp, "_trefstatus", "w");
			if ((nErr == 0) && (fp != NULL))		//Klocwork
			{
				fprintf(fp, "%s\n", (LPCTSTR) m_szWaferId);
				fprintf(fp, "Wafer X, Wafer Y, Result\n");
				fclose(fp);
			}
		}
		else if (fp != NULL)
		{
			//If not NULL, should close handle here 
			fclose(fp);		//Klocwork
		}

		if (lDieType == 1)
		{
			szDieStatus = "Good";
		}

		nErr = fopen_s(&fp, "_trefstatus", "a+");
		if ((nErr == 0) && (fp != NULL))
		{
			fprintf(fp, "%ld, %ld, %s\n", lUserX, lUserY, (LPCTSTR) szDieStatus);
			fclose(fp);
		}
	}


	//Give Alarm & user message
	bRefDieCheck	= (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["RefDie"]["Check"];
	lRefDieLearnt	= (*m_psmfSRam)["WaferPr"]["RefDie"]["Count"];
	LONG lRefDieCheck	= (*m_psmfSRam)["DieInfo"]["RefDie"]["Check"];		//v2.91T1

	if ( (bRefDieCheck == TRUE) && (lRefDieLearnt >= 1) )
	{
		lDieType = (*m_psmfSRam)["DieInfo"]["RefDie"]["Result"]; 
	
		if ( lDieType != 1 )
		{
			ulInternalRow	= (*m_psmfSRam)["DieInfo"]["RefDie"]["Y"];
			ulInternalCol	= (*m_psmfSRam)["DieInfo"]["RefDie"]["X"];

			ConvertAsmToOrgUser(ulInternalRow, ulInternalCol, lUserX, lUserY);
			szDieStatus.Format("Row %d, Col %d", lUserX, lUserY);
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

			CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
			BOOL bNeedAlarm = TRUE;
			if( pApp->GetCustomerName()=="Cree" && IsPrescanBlkPick() && pUtl->GetPrescanRegionMode()==FALSE )
				bNeedAlarm = FALSE;
			//v2.82
			if (pApp->GetCustomerName() == "Lumileds")		//** This feature only available for Lumileds DLA **//
			{
				SetAlert_Red_Yellow(IDS_WT_REFDIE_FAIL);
				lReply = WT_CHK_REFDIE_STOP;
			}
			else
			{
				if( bNeedAlarm==FALSE )
					lReply = 1;
				else
					lReply = SetAlert_Msg_Red_Back(IDS_WT_REFDIE_FAIL, szDieStatus, "Ignore", "Stop");		//WT0008	
			}

			if ( lReply == 1 )	//"Ignore"	or Realign for cree prescan
			{
				if( bNeedAlarm==FALSE )
				{
					SetErrorMessage("Reference die not match & realign in AUTOBOND");
					CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Reference Die Check Fail (Realign)");
					SetErrorMessage("Reference die not match & realign fail (to stop) in AUTOBOND");
					lReply = WT_CHK_REFDIE_STOP;
				}
				else
				{
					lReply = WT_CHK_REFDIE_IGNORE; 
					SetErrorMessage("Reference die not match & ignore in AUTOBOND");
					CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Reference Die Check Fail (Ignore)");
				}
			}
			else
			{
				lReply = WT_CHK_REFDIE_STOP; 				
				BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
				if (bPLLMSpecialFcn)	//v3.76
				{
					SetAlignmentStatus(FALSE);	//force to realign wafer
					SetErrorMessage("Reference die not match & stop in AUTOBOND; WAFER-ALIGN status reset!");
				}
				else
				{
					SetErrorMessage("Reference die not match & stop in AUTOBOND");
				}
				CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Reference Die Check Fail (Stop)");
			}
		}
	}
	else if (m_bCheckGDOnRefDiePos && (lRefDieCheck == 1))		//Check Srch-GOOD-DIE result on ref-die pos		//v2.91T1
	{
		lDieType	= (*m_psmfSRam)["WaferPr"]["DieResult"];
		szDieStatus = "No alignable die found on ref-die position!";

		if (lDieType == WT_MAP_DIESTATE_EMPTY)
		{
			lReply = SetAlert_Msg_Red_Yellow(IDS_WT_REFDIE_CHECK_GD_FAIL, szDieStatus, "Stop");		//v3.25T4	

			lReply = WT_CHK_REFDIE_STOP; 				
			SetErrorMessage("Reference die check GD not match & stop in AUTOBOND");
		}
	}

	return lReply;
}


LONG CWaferTable::AlertRefDieCheckOnGDStatus(VOID)
{
	LONG lReply = WT_CHK_REFDIE_OK;
	CString szDieStatus = "GOOD-die found at ref-die position!";
	BOOL bWarning		= FALSE;

	BOOL bRefDieCheck	= (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["RefDie"]["Check"];
	LONG lRefDieCheck	= (*m_psmfSRam)["DieInfo"]["RefDie"]["Check"];	


	//If good-die pattern is found at ref-die position, and with m_bCheckGDOnRefDiePos option enabled
	if (m_bCheckGDOnRefDiePos && !bRefDieCheck && lRefDieCheck)
	{
		(*m_psmfSRam)["WaferPr"]["Compensate"]["X"] = 0;
		(*m_psmfSRam)["WaferPr"]["Compensate"]["Y"] = 0;

		GetEncoderValue();		

		LONG lDiePitchX_X	= GetDiePitchX_X(); 
		LONG lDiePitchX_Y	= GetDiePitchX_Y();
		LONG lDiePitchY_X	= GetDiePitchY_X();
		LONG lDiePitchY_Y	= GetDiePitchY_Y();

		IPC_CServiceMessage stMsg;
		int nConvID = 0;

		REF_TYPE	stInfo;

		SRCH_TYPE	stSrchInfo;

		LONG lX[2];
		LONG lY[2];
		LONG lOrigEncX = GetCurrX();
		LONG lOrigEncY = GetCurrY();

		lX[0] = GetCurrX() + lDiePitchX_X / 2;		//LEFT die
		lY[0] = GetCurrY() + lDiePitchX_Y / 2;
		lX[1] = GetCurrX() - lDiePitchX_X / 2;		//RIGHT die
		lY[1] = GetCurrY() - lDiePitchX_Y / 2;

		// Go left/right 1/2 pitch to look for any no-good die (i.e. ref-die)
		for (int i=0; i<2; i++)
		{
			XY_SafeMoveTo(lX[i], lY[i]);
			Sleep(50);

			//Init Message
			stSrchInfo.bShowPRStatus	= FALSE;
			stSrchInfo.bNormalDie		= TRUE;
			stSrchInfo.lRefDieNo		= 1;
			stSrchInfo.bDisableBackupAlign = FALSE;

			stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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

			if (!stInfo.bGoodDie && stInfo.bFullDie)		// if not GOOD-DIE but alignable ->ref-die found!!
			{
				return WT_CHK_REFDIE_OK;
			}
		}

		// If still no good, return back to original position and pop up warning!!
		XY_SafeMoveTo(lOrigEncX, lOrigEncY); 
		Sleep(50);
		bWarning = TRUE;
	}


	//Display warning to operator, choose IGNORE or ABORT
	if (bWarning)
	{
		lReply = SetAlert_Msg_Red_Back(IDS_WT_REFDIE_CHECK_GD_OK, szDieStatus, "Ignore", "Stop");		//WT0019	

		if ( lReply == 1 )		//"Ignore"
		{
			lReply = WT_CHK_REFDIE_IGNORE; 
			SetErrorMessage("Reference die check GD match & ignore in AUTOBOND");
		}
		else
		{
			lReply = WT_CHK_REFDIE_STOP; 				
			SetErrorMessage("Reference die check GD match & stop in AUTOBOND");
		}
	}

	return lReply;
}

// huga
LONG CWaferTable::AlertSpecailRefDieCheckStatus(LONG lY, LONG lX)
{
	BOOL bReturn = FALSE;
	BOOL bSpRefGradeCheckResult = FALSE;

	bSpRefGradeCheckResult = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["RefGradeCheck"]["Result"];

	if (DieIsSpecialRefGrade(lY, lX) == TRUE)	
	{
		if (bSpRefGradeCheckResult == FALSE )
		{
			// Reset it
			(*m_psmfSRam)["WaferPr"]["RefGradeCheck"]["Result"] = TRUE;
			SetAlert_Red_Yellow(IDS_WT_REFDIE_FAIL);
			SetErrorMessage("Special Reference Grade Check Failed");
			CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Reference Die 2 Check Failed");
			bReturn = FALSE;
		}
		else
		{
			CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Reference Die 2 Check OK");	
			bReturn = TRUE;
		}
	}

	return bReturn;
}



////////////////////////////////////////////
//	Output reference die status file
////////////////////////////////////////////

BOOL CWaferTable::OutputRefDieStatusFile(VOID)
{
	INT nResult = gnOK;
	BOOL bReply = FALSE;	


	if (CMS896AStn::m_bIfGenRefDieStatusFile == TRUE)
	{
		//Let overwrite existing file
		if (CopyFile("_trefstatus", "RefDieStatus.txt", FALSE) != FALSE)
		{
			DeleteFile("_trefstatus");			
			bReply = TRUE;
		}
	}	

	return bReply;
}


/////////////////////////////////////////////
//  BLK Function Data
/////////////////////////////////////////////
BOOL CWaferTable::SaveBlkData(VOID)
{
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

    // open config file
    if (pUtl->LoadBlkConfig() == FALSE)
		return FALSE;

    // get file pointer
	psmf = pUtl->GetBlkConfigFile();

	if (psmf == NULL)
		return FALSE;

    // update data
	(*psmf)[WT_BLOCK][WT_SrchRefGrid]			= m_lMnSrchRefGrid;
	(*psmf)[WT_BLOCK][WT_SrchHomeGrid]			= m_lMnSrchHomeGrid;
	(*psmf)[WT_BLOCK][WT_MaxJumpCtr]			= m_lMnMaxJumpCtr;
	(*psmf)[WT_BLOCK][WT_MaxJumpEdge]			= m_lMnMaxJumpEdge;
	(*psmf)[WT_BLOCK][WT_NoDieGrade]			= m_lMnNoDieGrade;
	(*psmf)[WT_BLOCK][WT_MinDieForPick]			= m_lMnMinDieForPick;
	(*psmf)[WT_BLOCK][WT_EnableEdge]			= m_bMnEdgeAlign;
	(*psmf)[WT_BLOCK][WT_PassPercent]			= m_lMnPassPercent;
	(*psmf)[WT_BLOCK][WT_EnableF]				= m_bOCRDigitalF;
	(*psmf)[WT_BLOCK][WT_UpdateRefDieOffset]	= m_bAutoUpdateRefDieOffset;	//Block2
	(*psmf)[WT_BLOCK][WT_MAP_ALIGN_COR]			= m_bCheckCOR;

	// close config file
    pUtl->UpdateBlkConfig();
    pUtl->CloseBlkConfig();
    return TRUE;
}


BOOL CWaferTable::LoadBlkData(VOID)
{
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;


    // open config file
    if (pUtl->LoadBlkConfig() == FALSE)
		return FALSE;

    // get file pointer
    psmf = pUtl->GetBlkConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
		return FALSE;

    // retrive data
	m_lMnSrchHomeGrid		= (*psmf)[WT_BLOCK][WT_SrchHomeGrid];
	m_lMnSrchRefGrid		= (*psmf)[WT_BLOCK][WT_SrchRefGrid];
	m_lMnMaxJumpCtr			= (*psmf)[WT_BLOCK][WT_MaxJumpCtr];
	m_lMnMaxJumpEdge		= (*psmf)[WT_BLOCK][WT_MaxJumpEdge];
	m_lMnNoDieGrade			= (*psmf)[WT_BLOCK][WT_NoDieGrade];
	m_lMnMinDieForPick		= (*psmf)[WT_BLOCK][WT_MinDieForPick];
	m_bMnEdgeAlign			= (BOOL)((LONG)(*psmf)[WT_BLOCK][WT_EnableEdge]);
	m_lMnPassPercent		= (*psmf)[WT_BLOCK][WT_PassPercent];
	m_bOCRDigitalF				= (BOOL)((LONG)(*psmf)[WT_BLOCK][WT_EnableF]);
	m_bAutoUpdateRefDieOffset	= (BOOL)(LONG)(*psmf)[WT_BLOCK][WT_UpdateRefDieOffset];		//Block2
	m_bCheckCOR				= (BOOL)((LONG)(*psmf)[WT_BLOCK][WT_MAP_ALIGN_COR]);

    // close config file
    pUtl->CloseBlkConfig();

    return TRUE;
}

BOOL CWaferTable::RestoreOutputFileSetting(VOID)
{
	BOOL bGenWafermapColFile = FALSE;
	CString szFile;
	CString szPath;

	//Copy user wafermap header & output format file into ASM's use file if (user file name & file are existed)
	if (m_szMapHeaderFileName.IsEmpty() == FALSE)
	{
		szFile = gszUSER_DIRECTORY + "\\OutputFile\\" + m_szMapHeaderFileName;
		if( (_access(szFile, 0 )) != -1 )
		{
			CopyFile(szFile, gszROOT_DIRECTORY + "\\Exe\\WaferMapColumns.txt", FALSE);
			bGenWafermapColFile = TRUE;
		}
		else
		{
			m_szMapHeaderFileName = "";
			DeleteFile(gszROOT_DIRECTORY + "\\Exe\\WaferMapColumns.txt");
			DeleteFile(gszROOT_DIRECTORY + "\\Exe\\OutputFileColumnsSelection.txt");
		}
	}

	if (m_szOutputFormatFileName.IsEmpty() == FALSE)
	{
		szFile = gszUSER_DIRECTORY + "\\OutputFile\\" + m_szOutputFormatFileName;
		if ( (_access(szFile, 0 )) != -1 )
		{
			CopyFile(szFile, gszROOT_DIRECTORY + "\\Exe\\OutputFileFormat.txt", FALSE);
		}
		else
		{
			m_szOutputFormatFileName = "";
			DeleteFile(gszROOT_DIRECTORY + "\\Exe\\OutputFileFormat.txt");
		}
	}


	if (bGenWafermapColFile == TRUE)
	{
		CStdioFile cfCopyFile, cfSelectionFile;
		CString szSelectionItem, szFile;

		//Open wafermapColumns file
		szFile = gszROOT_DIRECTORY + "\\Exe\\WaferMapColumns.txt";

		cfCopyFile.Open(szFile, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone|CFile::typeText);

		cfCopyFile.SeekToBegin();


		//Write Combo Box selection file for Output File Format
		szFile = gszROOT_DIRECTORY + "\\Exe\\OutputFileColumnsSelection.txt";
		DeleteFile(szFile);

		cfSelectionFile.Open(szFile, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText);

		cfSelectionFile.SeekToBegin();

		cfSelectionFile.WriteString("None,WaferID,Bin Row,Bin Col");

		cfCopyFile.SeekToBegin();

		cfCopyFile.ReadString(szSelectionItem); //skip first line

		while (cfCopyFile.ReadString(szSelectionItem)) 
		{
			cfSelectionFile.WriteString("," + szSelectionItem);
		}

		cfSelectionFile.Close();
		cfCopyFile.Close();
	}

	return TRUE;
}


VOID CWaferTable::FlushMessage()
{
	if (m_comServer.ScanRequest(10))
		m_comServer.ReadRequest();
}

VOID CWaferTable::UpdateStationData()
{
	CString szAlgorithm, szPath;
	//AfxMessageBox("CWaferTable", MB_SYSTEMMODAL);
	m_oWaferTableDataBlk.m_szEnableDiePitchCheck = m_oWaferTableDataBlk.ConvertBoolToOnOff(m_bDiePitchCheck|m_bAbsDiePitchCheck	);
	m_oWaferTableDataBlk.m_szEnableMapDieTypeCheck = m_oWaferTableDataBlk.ConvertBoolToOnOff(m_bEnableMapDieTypeCheck);
	m_oWaferTableDataBlk.m_szMapDieTypeCheckDieType = m_szUserDefineDieType;
	
	m_oWaferTableDataBlk.m_szEnablePreScan = m_oWaferTableDataBlk.ConvertBoolToOnOff(IsPrescanEnable());
	m_oWaferTableDataBlk.m_szPreScanPrDelay.Format("%d", m_lPrescanPrDelay);
		
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPath);
	

	m_oWaferTableDataBlk.m_szWaferMapPath = szPath;
	m_oWaferTableDataBlk.m_szWaferMapTour = szAlgorithm;

	if (m_WaferMapWrapper.GetPickMode() == WAF_CDieSelectionAlgorithm::IN_GRADE_ORDER)
	{
		m_oWaferTableDataBlk.m_szWaferMapMode = "In Grade";
	}
	else
	{
		m_oWaferTableDataBlk.m_szWaferMapMode = "Mix";
	}

	if (m_lSortingMode == 0)
	{
		m_oWaferTableDataBlk.m_szSortingMode = "Default";
	}
	else if (m_lSortingMode == 1)
	{
		m_oWaferTableDataBlk.m_szSortingMode = "Ascending";
	}
	else if (m_lSortingMode == 2)
	{
		m_oWaferTableDataBlk.m_szSortingMode = "Descending";
	}
	else
	{
		m_oWaferTableDataBlk.m_szSortingMode = "User Define";
	}
	
	if (m_ulAlignOption == 0)
	{
		m_oWaferTableDataBlk.m_szAlignOption = "Manual Input";
		m_oWaferTableDataBlk.m_szHomeDieRow.Format("%d", m_lHomeDieMapRow);
		m_oWaferTableDataBlk.m_szHomeDieCol.Format("%d", m_lHomeDieMapCol) ;
	}
	else if (m_ulAlignOption == 1)
	{
		m_oWaferTableDataBlk.m_szAlignOption = "Use Map File";
		m_oWaferTableDataBlk.m_szHomeDieRow.Format("%d", m_lOrgMapRowOffset);
		m_oWaferTableDataBlk.m_szHomeDieCol.Format("%d", m_lOrgMapColOffset);		
	}	
	else
	{
		m_oWaferTableDataBlk.m_szAlignOption = "Use SCN File";
		m_oWaferTableDataBlk.m_szHomeDieRow.Format("%d", m_lScnMapRowOffset);
		m_oWaferTableDataBlk.m_szHomeDieCol.Format("%d", m_lScnMapColOffset);
	}

	CString szMsg;
	szMsg.Format("%d", m_lPredictMethod);
	m_oWaferTableDataBlk.m_szEnableDEBAdaptive	= szMsg;
	m_oWaferTableDataBlk.m_szStage1Start.Format("%d",	m_ulAdvStage1StartCount);
	m_oWaferTableDataBlk.m_szStage1Period.Format("%d",	m_ulAdvStage1SampleSpan);
	m_oWaferTableDataBlk.m_szStage2Start.Format("%d",	m_ulAdvStage2StartCount);
	m_oWaferTableDataBlk.m_szStage2Period.Format("%d",	m_ulAdvStage2SampleSpan);
	m_oWaferTableDataBlk.m_szStage3Start.Format("%d",	m_ulAdvStage3StartCount);
	m_oWaferTableDataBlk.m_szStage3Period.Format("%d",	m_ulAdvStage3SampleSpan);
	m_oWaferTableDataBlk.m_szRegionGridRow.Format("%d",	m_ulPdcRegionSizeRow);	//	GetRegionPredicationRow
	m_oWaferTableDataBlk.m_szRegionGridCol.Format("%d",	m_ulPdcRegionSizeCol);	//	GetRegionPredicationCol

	m_oWaferTableDataBlk.m_szAutoSrchHome.Format("%d",	m_bAutoSrchHome);
	m_oWaferTableDataBlk.m_szWaferID.Format("%d",		GetMapFileName());
	m_oWaferTableDataBlk.m_szPitchX_Tolerance.Format("%d",	m_lPitchX_Tolerance);
	m_oWaferTableDataBlk.m_szPitchY_Tolerance.Format("%d",	m_lPitchY_Tolerance);
	m_oWaferTableDataBlk.m_szDiePitchCheck.Format("%d",	m_bDiePitchCheck);
	m_oWaferTableDataBlk.m_szScnCheckByPR.Format("%d",	m_bScnCheckByPR);
	m_oWaferTableDataBlk.m_szEnableScnCheck.Format("%d",	m_bEnableSCNCheck);
	m_oWaferTableDataBlk.m_szScnCheckIsRefDie.Format("%d",	m_bScnCheckIsRefDie);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SetWaferTableDataBlock(m_oWaferTableDataBlk);
}

//Block Function	//Block2
BOOL CWaferTable::CheckCOR(DOUBLE dAngle)
{
	LONG lOldX=0,lOldY=0,lOldT=0;
	GetEncoder(&lOldX, &lOldY, &lOldT);

	LONG lX=lOldX, lY=lOldY, lT=0;
	RotateWaferTheta(&lX, &lY, &lT, dAngle);
	
	//Move Table & Theta
	T_MoveTo(lOldT + lT, SFM_WAIT);
	XY_SafeMoveTo(lX, lY);
	return TRUE;
}

	
LONG CWaferTable::DisplayErrorMessage(CONST CString szMsg)
{
	LONG lStatus = HmiMessage_Red_Back(szMsg, "Align Wafer Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	return lStatus;
}


LONG CWaferTable::DisplayRtMessage(CONST CString szMsg)
{
	return HmiMessage(szMsg, "Align Wafer Message", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 1000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
}


BOOL CWaferTable::GetWaferCalibXY(LONG& lCorX, LONG& lCorY)
{
	if( IsWT2InUse() )
	{
		lCorX = m_lWaferCalibX2;
		lCorY = m_lWaferCalibY2;
	}
	else
	{
		lCorX = m_lWaferCalibX;
		lCorY = m_lWaferCalibY;
	}
	return TRUE;
}


BOOL CWaferTable::IsCheckCOREnabled() CONST
{
	return m_bCheckCOR;
}


VOID CWaferTable::SetRefDieCounts(CONST LONG lTotalCount, CONST LONG lCurrCount)
{
	m_lTotalRefDieCount	= lTotalCount;
	m_lCurrRefDieCount	= lCurrCount;
}
//end Block Function

BOOL CWaferTable::IsConvexPolygon()
{
	LONG nFactor = 1000;
	LONG nM11,nM12,nM13;
	LONG nResult;
	LONG nStart,nFinih,nTestP;
	LONG nPoint = 0;
	LONG nInRegion[WT_MAX_POLYGON_PT];
	LONG nInside = 0;

	memset(&nInRegion, 0, sizeof(nInRegion));

	for (nPoint=0; nPoint<m_lWaferPolyLimitNo; nPoint++ )
	{
		nStart = nPoint;
		nFinih = nPoint + 1;
		if ( nFinih > m_lWaferPolyLimitNo-1 )
		{
			nFinih = 0;
		}

		nTestP = nFinih + 1;
		if ( nTestP > m_lWaferPolyLimitNo-1 )
		{
			nTestP = 0;
		}

		nM11 = (m_lWaferPolyLimitX[nStart]/nFactor)*((m_lWaferPolyLimitY[nFinih]/nFactor) - (m_lWaferPolyLimitY[nTestP]/nFactor));
		nM12 = (m_lWaferPolyLimitX[nFinih]/nFactor)*((m_lWaferPolyLimitY[nStart]/nFactor) - (m_lWaferPolyLimitY[nTestP]/nFactor));
		nM13 = (m_lWaferPolyLimitX[nTestP]/nFactor)*((m_lWaferPolyLimitY[nStart]/nFactor) - (m_lWaferPolyLimitY[nFinih]/nFactor));
		nResult = nM11-nM12+nM13;

		if (nResult <= 0)
		{
			nInRegion[nPoint] = 1;
		}
		else
		{
			nInRegion[nPoint] = 0;
		}
	}

	nInside = nInRegion[0];

	for (nPoint=1; nPoint<m_lWaferPolyLimitNo; nPoint++)
	{
		nInside = nInside & nInRegion[nPoint];
	}

	if ( nInside == 1 )
	{
		return TRUE;
	}

	return FALSE;
}



VOID CWaferTable::SetAlignmentStatus(BOOL bFinish)
{
	m_bAlignedWafer = bFinish;
	m_bJustAlign = bFinish;
	m_bResetStartPos = bFinish;
	m_bPitchAlarmOnce = FALSE;
	m_bReSampleAsError	= FALSE;

	if ( bFinish && CMSLogFileUtility::Instance()->GetEnableWtMachineLog() )
	{
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		if( pUtl->GetPrescanRegionMode() )
		{
			CTime stTime = CTime::GetCurrentTime();
			CString szTime;
			szTime.Format("_%2d%2d%2d%2d", stTime.GetDay(), stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
			CString szLogPath = m_szPrescanLogPath;

			CString szLocalFile = szLogPath + WT_PRESCAN_GET_IDX_LOG;
			CString szBackFile  = szLogPath + szTime + WT_PRESCAN_GET_IDX_LOG;
			RenameFile(szLocalFile, szBackFile);

			szLocalFile = szLogPath + WT_PRESCAN_GET_DIE_LOG;
			szBackFile  = szLogPath + szTime + WT_PRESCAN_GET_DIE_LOG;
			RenameFile(szLocalFile, szBackFile);

			szLocalFile = szLogPath + WT_SCAN_GET_PZN_LOG;
			szBackFile  = szLogPath + szTime + WT_SCAN_GET_PZN_LOG;
			RenameFile(szLocalFile, szBackFile);
		}
	}
}

BOOL CWaferTable::CheckGradeMappingStatus(BOOL bForceCheck)	//v4.50A11	//Cree HuiZhou GradeMapping fcn
{
	if (m_WaferMapWrapper.IsGradeMapEnabled() && (m_WaferMapWrapper.IsGradeMapFull() || bForceCheck) )
	{
		INT nSize = m_WaferMapWrapper.GetExtraGradeMapEntriesRequired();

		CString szLog;
		szLog.Format("ERROR (CheckGradeMappingStatus): Wafer Map Grade Mapping exceeded limit (%d)", nSize);
		SetErrorMessage(szLog);
		
		if (nSize > 0)	//v4.50A16
		{
			szLog.Format("Exceed Grade Mapping Limit!\nPlease clear at least %d bins to continue.", nSize);
			//AfxMessageBox(szLog, MB_ICONSTOP|MB_SYSTEMMODAL);
			HmiMessage_Red_Yellow(szLog, "Load Wafer Map");		
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CWaferTable::IsAlignedWafer()
{
	return m_bAlignedWafer;
}

BOOL CWaferTable::IsDisableRefDie()
{
	return m_bDisableRefDie;
}


VOID CWaferTable::CheckResetManualAlignRegion()
{
	//reset "m_bRegionAligned"
	//if there has not reference die in the next region and m_bManualRegionScanSort is TRUE,
	//it should reset "m_bManualAlignRegion" and "m_bWaferAlign"
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	if (!IsNextRegionAutoAlign())
	{
		pSRInfo->SetManualAlignRegion(FALSE);
		m_bAlignedWafer = FALSE;
	}
	pSRInfo->SetRegionAligned(FALSE);
}


VOID CWaferTable::SaveWaferMapAlgorithmSettings()
{
	CString szTour, szPath;
	m_WaferMapWrapper.GetAlgorithm(szTour,szPath);

	m_szMapTour = szTour;
	m_szMapPath = szPath;

	m_ucMapPickMode = m_WaferMapWrapper.GetPickMode();
	m_ulMapDieStep = m_WaferMapWrapper.GetDieStep();

	LogItems(WAFERMAP_ALGO_PATH);
	LogItems(WAFERMAP_ALGO_TOUR);
	LogItems(WAFERMAP_ALGO_MODE);

	SaveData();
}

VOID CWaferTable::LoadWaferMapAlgorithmSettings()
{
	if (m_szMapTour.Find("Shortest") != -1)	//v3.87		//Shortest Path mapping fcn for PLLM only
	{
		//It may suffer from sw crash if "Shortest Path" is set in SelectAlgorithm
	}
	else
	{
		m_WaferMapWrapper.SelectAlgorithm(m_szMapTour, m_szMapPath);
		m_WaferMapWrapper.SetPickMode((WAF_CDieSelectionAlgorithm::WAF_EPickMode) m_ucMapPickMode);
		m_WaferMapWrapper.SetDieStep(m_ulMapDieStep);
	}
}

//================================================================
// SetCustomerMapOptions()
//   Created-By  : Andrew Ng
//   Date        : 2/25/2008 10:25:59 AM
//   Description : 
//   Remarks     : 
//================================================================
BOOL CWaferTable::SetCtmRefDieOptions()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	unsigned long ulRows=0, ulCols=0;
	unsigned char ucGrade = 0;
	unsigned char ucGradeOffset = 0;
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	ucGradeOffset = m_WaferMapWrapper.GetGradeOffset();

	if (!m_pWaferMapManager->GetWaferMapDimension(ulRows, ulCols))
	{
		return FALSE;
	}

	/*if (pApp->GetCustomerName() == "IsoFoton")
	{
		unsigned char ucRefDie = 0xCC;

		for (int i=0; i<(INT)ulRows; i++)
		{
			for (int j=0; j<(INT)ulCols; j++)
			{
				ucGrade = m_WaferMapWrapper.GetReader()->GetDieInformation(i, j) -ucGradeOffset;
				if (ucGrade == ucRefDie)
				{
					m_WaferMapWrapper.SetReferenceDie(i, j, TRUE);
				}
			}
		}
	}
	else if (pApp->GetCustomerName() == "Huga")
	{
		unsigned char ucRefDie = 0xc9;

		for (int i=0; i<(INT)ulRows; i++)
		{
			for (int j=0; j<(INT)ulCols; j++)
			{
				ucGrade = m_WaferMapWrapper.GetReader()->GetDieInformation(i, j) -ucGradeOffset;
				
				if (ucGrade == ucRefDie)
				{
					m_WaferMapWrapper.SetReferenceDie(i, j, TRUE);
				}
			}
		}
	}
	else if( pApp->GetCustomerName()=="TongHui" )
	{
		for (int i=0; i<(INT)ulRows; i++)
		{
			for (int j=0; j<(INT)ulCols; j++)
			{
				if( m_WaferMapWrapper.IsReferenceDie(i, j) )
				{
					continue;
				}

				ucGrade = m_WaferMapWrapper.GetGrade(i, j);
				
				if( ucGrade==ucNullBin )
				{
					continue;
				}

				if( ucGrade>(151+ucGradeOffset) )
				{
					m_WaferMapWrapper.ChangeGrade(i, j, 151+ucGradeOffset);
				}
			}
		}
	}*/

	UCHAR ucReferBin = ucNullBin;
	if( m_WaferMapWrapper.GetReader() != NULL )
	{
		ucReferBin = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
	}

	if ( m_bScnAlignDieAsRefer )
	{
		LONG i = 0;
		ULONG ulIntRow = 0, ulIntCol = 0;
		LONG lUserRow = 0, lUserCol = 0;

		for (i = 1; i <= m_lScnAlignTotalPoints; i++)
		{
			if( GetScnFPCMapPosition(i, ulIntRow, ulIntCol, lUserRow, lUserCol) )
			{
				if ( IsMapNullBin(ulIntRow, ulIntCol) )
				{
					m_WaferMapWrapper.ChangeGrade(ulIntRow, ulIntCol, ucReferBin);
				}
				m_WaferMapWrapper.SetReferenceDie(ulIntRow, ulIntCol, TRUE);
			}
		}
	}

	/*if( IsEnableFPC() && m_bScnCheckDieAsRefDie )
	{
		LONG	i=0;
		ULONG	ulIntRow = 0, ulIntCol = 0;
		LONG	lUserRow, lUserCol;
		for (i=1; i<m_lTotalSCNCount+1; i++)
		{
			if( GetFPCMapPosition(i, ulIntRow, ulIntCol, lUserRow, lUserCol) )
			{
				if( IsMapNullBin(ulIntRow, ulIntCol) )
				{
					m_WaferMapWrapper.ChangeGrade(ulIntRow, ulIntCol, ucReferBin);
				}
				m_WaferMapWrapper.SetReferenceDie(ulIntRow, ulIntCol, TRUE);
			}
		}
	}*/

	return TRUE;
}


VOID CWaferTable::LOG_DIEPITCH_DIFF(CONST CString szMsg)
{
	if (CMS896AApp::m_bEnableMachineLog != TRUE)
		return;

	CStdioFile fLogFile;
	if (fLogFile.Open(_T("c:\\mapsorter\\UserData\\WT_DIEPITCH_CHECK.log"), 
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
	}
}
	
VOID CWaferTable::BackupDiePitchLog(CONST CString szWaferID)
{
	if (CMS896AApp::m_bEnableMachineLog != TRUE)
		return;

	CString szOldName = "c:\\mapsorter\\UserData\\WT_DIEPITCH_CHECK.log";
	CString szNewName = "c:\\mapsorter\\UserData\\WT_DIE_PITCH-" + szWaferID + ".log";

	TRY {
		if ((_access(szOldName, 0) ) == -1)
		{
			return;
		}
		RenameFile(szOldName, szNewName);
	} 
	CATCH (CFileException, e) {
	}
	END_CATCH
}

VOID CWaferTable::LOG_BLOCK_ALIGN(CONST CString szMsg)
{
	CString szFileName = _T("c:\\mapsorter\\UserData\\History\\WT_BlockAlign.log");

	CStdioFile fLogFile;
	if (fLogFile.Open(szFileName, 
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
	}
}

VOID CWaferTable::REMOVE_BLOCK_ALIGN()
{
	INT nCol =0;
	ULONGLONG nFileSize =0;
	CString szFileName = _T("c:\\mapsorter\\UserData\\History\\WT_BlockAlign.log");
	CString szBpFileName;
	CFile fLogFile;

	if  (fLogFile.Open(szFileName, CFile::modeRead) == FALSE)
	{
		return;
	}

	nFileSize = fLogFile.GetLength();
	fLogFile.Close();

	// Case of no need to backup the log file
	if (nFileSize < 1024*1024 )
	{
		return;
	}

	// Start backup log file
	szBpFileName = _T("c:\\mapsorter\\UserData\\History\\WT_BlockAlign.bak");

	try
	{
		RenameFile(szFileName, szBpFileName);
	}
	catch (CFileException e)
	{
	}
}

CString CWaferTable::GetPathByFileDialog(CString szDefaultPath, CString szDefaultFileName, CString szDefaultExt)
{
	CString szResult;

	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;
	static char szFilters[] = "All Files (*.*)|*.*||";
	CString szLoadString;
	if ( szDefaultExt != "*" && szDefaultExt != "" )
	{
		//static char szFilters[] = "XML File (*.xml)|*.xml|All Files (*.*)|*.*||";
		// XML -> " + szDefaultExt + "
		szLoadString = "" + szDefaultExt + " File (*." + szDefaultExt + ")|*." + szDefaultExt + "|All Files (*.*)|*.*||";
		strcpy(szFilters, szLoadString);
	}

	if ( pAppMod != NULL )
	{
		pMainWnd = pAppMod->m_pMainWnd;
	}
	else
	{
		pMainWnd = NULL;
	}

	CString szInitialFileName = szDefaultFileName + "." + szDefaultExt;
	// Create an Open dialog; the default file name extension is ".xml".  // v4.48 chris change to general
	CString szNewExtension = szDefaultExt=="*"||szDefaultExt==""?NULL:szDefaultExt;
	CFileDialog dlgFile(TRUE, szNewExtension, (LPCTSTR)szInitialFileName, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , szFilters, pMainWnd, 0);
	dlgFile.m_ofn.lpstrInitialDir =  szDefaultPath;
	dlgFile.m_ofn.lpstrDefExt =  szDefaultExt;

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);			// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();				// After input, restore the application to background
	}

	if ( nReturn == IDOK )
	{
		CString szFullFileName	= dlgFile.GetPathName();	// full path and file name
		szResult = szFullFileName;
		SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
	}
	else
	{
		HmiMessage("Load File Cancelled");
	}

	return szResult;
}

BOOL CWaferTable::SaveWaferLotData()
{
	CStdioFile cfDataFile;
	CString szData;
	CString szFileName;
	BOOL bDataTitleFound = FALSE;
	BOOL bCheckLotWaferName = FALSE;
	int nCol = 0;
	int nWaferNo = 1;

	CString szTempValue;
	CString szDefFileName;
	CString szTotalWafer;
	CString szFileType;
	CString szFileExt;
	CString szPKGFilename;
	CString szCheckLotWaferName = "", szLotInfoFilePath = "", szLotInfoFileName = "", szLotInfoFileExt  = "";
	CString szCurrLotInfoFileName = "";

	DeleteFile(MSD_WAFER_LOT_INFO_FILE);
	DeleteFile(MSD_WAFER_LOT_INFO_BKF);
	
	szFileExt = m_szLotInfoFileExt;
	szFileExt = szFileExt.MakeUpper();
		
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp == NULL)
	{
		return FALSE;
	}

	if (pApp->CheckIsAllBinCleared() == FALSE)  // v4.51D1 Secs
	{
		SetErrorMessage("Open lot file failed");
		m_szLotInfoFileName = (*m_psmfSRam)["MS896A"]["WaferLot Filename"];
		return FALSE;
	}

	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		// save szFileName
		// revised: yealy want a file dialog to choose file
		CString szLocalXMLPath = "C:\\MapSorter\\Exe\\Yealy.xml";	//	read via HMI button.
		if ( _access( szLocalXMLPath, 0) != -1 )
		{
			HmiMessage("When you reload the lot,Please Clear the Lot first!");
			return FALSE;
		}
		CString szChosenXMLPath = GetPathByFileDialog(m_szLotInfoFilePath,  "*", m_szLotInfoFileExt);
		LONG lSlashIndex = szChosenXMLPath.ReverseFind('\\');
		LONG lDotIndex = szChosenXMLPath.Find('.');
		if ( lSlashIndex != -1 && lDotIndex != -1 && lSlashIndex < lDotIndex )
		{
			CString szXMLName = szChosenXMLPath.Right(szChosenXMLPath.GetLength() - lSlashIndex -1);
			lDotIndex = szXMLName.Find('.');
			m_szLotInfoFileName	= szXMLName.Left(lDotIndex);
		}
		if ( _access( szChosenXMLPath, 0) != -1 )
		{
			if ( CopyFile(szChosenXMLPath, szLocalXMLPath, FALSE) )
			{
				szFileName = szChosenXMLPath;
				return MakeWaferIDListFromXML( szLocalXMLPath );
			}
			else
			{
				return FALSE;
			}
		}
		else if ( szChosenXMLPath == "" )
		{
			return FALSE;
		}
		else
		{
			HmiMessage("Open lot file failed");
			return FALSE;
		}
	}
	else 
	{
		szFileName = m_szLotInfoFilePath + "\\" + m_szLotInfoFileName + "." + m_szLotInfoFileExt;

		//Epistar Lot Number// MS899DL
		if (pApp->GetCustomerName() == "Epistar")
		{
			CStdioFile cfReadLotNo;
			BOOL bOpen = cfReadLotNo.Open(szFileName,CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			CString szLine;
			if (bOpen)
			{
				cfReadLotNo.ReadString(szLine);
			}
			for (int i = 1; i < 5; i++)
			{
				int l = szLine.Find(",");
				if( l!=-1 )
					szLine = szLine.Mid(l + 1);
			}

			//(*m_psmfSRam)["MS896A"]["WaferCommon String"]			= szLine;
			int nMsgLength;
			char* pFilename;
			nMsgLength = (szLine.GetLength()+1)*sizeof(CHAR);
			pFilename = new char[nMsgLength];
			strcpy_s(pFilename, nMsgLength, szLine);
			IPC_CServiceMessage stMsg;
			stMsg.InitMessage(nMsgLength, pFilename);
			INT nConvID = m_comClient.SendRequest("MapSorter", "SaveLotNumber", stMsg);
			BOOL bReturn = TRUE;
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
			delete [] pFilename;
		}
	}

	//if (pApp->CheckIsAllBinCleared() == FALSE)  // Comment v4.51D1 Secs
	//{
	//	SetErrorMessage("Open lot file failed");
	//	return FALSE;
	//}

	if (CMS896AStn::m_bWaferLotWithBinSummaryFormat == TRUE)
	{
		CString szMachineNo;
		CString szBinSummaryFilename;
		szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];

		szFileName = m_szLotInfoFilePath + "\\" + szMachineNo + "_" + m_szLotInfoFileName + "." + m_szLotInfoFileExt;

		// create a copy before loading the pkg file
		bCheckLotWaferName = m_bCheckLotWaferName;
		szLotInfoFilePath = m_szLotInfoFilePath;
		szLotInfoFileName = m_szLotInfoFileName;
		szLotInfoFileExt  = m_szLotInfoFileExt;

		// load it to get the pkg filename first
		if (ReadLextarLotFile(szFileName, szBinSummaryFilename, szPKGFilename) == FALSE)
		{
			SetErrorMessage("Fail to load lot file");
			SetAlert(IDS_WL_LOTFILE_NOT_FOUND);
			ShowWaferLotData();
			return FALSE;
		}

		if (pApp->LoadPKGFile(FALSE, szPKGFilename, TRUE, FALSE, TRUE) == FALSE)
		{
			SetAlert(IDS_WL_LOAD_PKG_FILE_FAIL);
			SetErrorMessage("Fail to load pkg file");
			return FALSE;
		}
	
		if (pApp->LoadPKGFile(TRUE, szPKGFilename + ".ppkg", FALSE, TRUE, TRUE) == FALSE)
		{
			SetAlert(IDS_WL_LOAD_PKKG_FILE_FAIL);
			SetErrorMessage("Fail to load ppkg file");
			return FALSE;
		}

		// restore the variable after laoded pkg file
		m_bCheckLotWaferName = bCheckLotWaferName;
		m_szLotInfoFilePath = szLotInfoFilePath;
		m_szLotInfoFileName = szLotInfoFileName;
		m_szLotInfoFileExt = szLotInfoFileExt;

		SaveData();

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
	}
	else
	{
		if (pApp->GetCustomerName() == "ZVision")
		{
			if (ReadLIFFileZVision(szFileName) == FALSE)
			{
				SetErrorMessage("Open lot file(ZVision) failed");
				ShowWaferLotData();
				return FALSE;
			}
		}
		else
		{
			if (ReadLIFFile(szFileName) == FALSE)
			{
				SetErrorMessage("Open lot file failed");
				ShowWaferLotData();
				return FALSE;
			}
		}

		SetCtmLogMessage("Open lot file completed");

		szCurrLotInfoFileName = (*m_psmfSRam)["MS896A"]["WaferLot Filename"];

		if (m_szLotInfoFileName != szCurrLotInfoFileName)
		{
			IPC_CServiceMessage stMsg;
			BOOL bShowAlert = TRUE;
			int nConvID = 0;

			//OsramTrip 8/22
			if (IsSecsGemInit())
			{
				bShowAlert = FALSE;
			}

			stMsg.InitMessage(sizeof(BOOL), &bShowAlert);
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetBinClearedCount", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
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
	
	ShowWaferLotData();

	return TRUE;
}

BOOL CWaferTable::ReadLIFFile(CString szFileName)
{
	INT nWaferNo = 1;
	CStdioFile cfDataFile;
	CStringMapFile	WaferLotInfo;
	CString szTempValue;
	CString szData;
	BOOL bCommaFormat = FALSE;

	LONG lTotalWafer;
	INT nCol = -1;

	CString szMsg;

	if (cfDataFile.Open(szFileName, 
		CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		szMsg = "Open lot file " + szFileName + " failed";
		SetErrorMessage(szMsg);
		SetAlert(IDS_WT_LOTFILE_NOT_FOUND);
		return FALSE;
	}

	if (WaferLotInfo.Open(MSD_WAFER_LOT_INFO_FILE, FALSE, TRUE) != 1)
	{
		ShowWaferLotData();
		//HmiMessage("File not found!\nPlease check","Load File Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);
		SetAlert(IDS_WT_LOTFILE_NOT_FOUND);
		SetErrorMessage("Cannot Open File: WaferLotInfo msd");
		CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferLotInfo msd");
		return FALSE;
	}

	while (cfDataFile.ReadString(szData) != NULL)
	{		
		//Search DEF file name
		nCol = szData.Find(WT_LOT_HEADER_A_DEFFILE);
		if ( nCol != -1 )
		{
			nCol = szData.Find("= ");
			if ( nCol != -1 )
			{
				szTempValue = szData.Mid(nCol+2);
				(WaferLotInfo)[WT_LOT_HEADER_A_DEFFILE] = szTempValue;
				m_szOsramSDF	= szTempValue;
			}
		}
	
		//Search Number of wafer name
		nCol = szData.Find(WT_LOT_HEADER_A_WAFER_NO);
		if ( nCol != -1 )
		{
			nCol = szData.Find("= ");
			if ( nCol != -1 )
			{
				szTempValue = szData.Mid(nCol+2);
				lTotalWafer = (LONG)atoi((LPCTSTR)szTempValue);		
				(WaferLotInfo)[WT_LOT_HEADER_A_WAFER_NO] = lTotalWafer;
			}
		}

		//Search "[WAFER]" then break
		nCol = szData.Find(WT_LOT_HEADER_B);
		
		if ( nCol != -1 )
		{
			break;
		}

		nCol = szData.Find(WT_LOT_CVS_HEADER);

		if (nCol != -1)
		{
			bCommaFormat = TRUE;
			break;
		}
	}

	if (bCommaFormat == FALSE)
	{
		while (cfDataFile.ReadString(szData) != NULL)
		{
			nCol = szData.Find(WT_LOT_WAFER_NO);
			if ( nCol != -1 )
			{
				nCol = szData.Find("= ");
				if ( nCol != -1 )
				{
					szTempValue = szData.Mid(nCol+2);
					(WaferLotInfo)[WT_LOT_WAFER_NO][nWaferNo] = szTempValue;
					(WaferLotInfo)[WT_LOT_WAFER_NO][nWaferNo][WT_LOT_WAFER_LOADED] = FALSE;
					nWaferNo++;
				}
			}
		}
	}
	else
	{
		while (cfDataFile.ReadString(szData) != NULL)
		{
			nCol = szData.Find(",");

			if ( nCol != -1 )
			{
				szData = szData.Mid(nCol+1);
				(WaferLotInfo)[WT_LOT_WAFER_NO][nWaferNo] = szData;
				(WaferLotInfo)[WT_LOT_WAFER_NO][nWaferNo][WT_LOT_WAFER_LOADED] = FALSE;
				nWaferNo++;
			}
		}

		(WaferLotInfo)[WT_LOT_HEADER_A_DEFFILE] = 0;
		(WaferLotInfo)[WT_LOT_HEADER_A_WAFER_NO] = (LONG) nWaferNo-1;
	}

	WaferLotInfo.Update();
	WaferLotInfo.Close();

	return TRUE;
}

BOOL CWaferTable::ReadLIFFileZVision(CString szFileName)
{
	INT nWaferNo = 1;
	CStdioFile cfDataFile;
	CStringMapFile	WaferLotInfo;
	CString szTempValue;
	CString szData;
	BOOL bCommaFormat = FALSE;

	LONG lTotalWafer;
	INT nCol = -1;

	CString szMsg;

	if (cfDataFile.Open(szFileName, 
		CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		szMsg = "Open lot file " + szFileName + " failed";
		SetErrorMessage(szMsg);
		SetAlert(IDS_WT_LOTFILE_NOT_FOUND);
		return FALSE;
	}

	if (WaferLotInfo.Open(MSD_WAFER_LOT_INFO_FILE, FALSE, TRUE) != 1)
	{
		ShowWaferLotData();
		//HmiMessage("File not found!\nPlease check","Load File Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);
		SetAlert(IDS_WT_LOTFILE_NOT_FOUND);
		SetErrorMessage("Cannot Open File: WaferLotInfo msd");
		CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferLotInfo msd");
		return FALSE;
	}

	while (cfDataFile.ReadString(szData) != NULL)
	{		
		//Search DEF file name
		nCol = szData.Find(WT_LOT_HEADER_A_DEFFILE);
		if ( nCol != -1 )
		{
			nCol = szData.Find("= ");
			if ( nCol != -1 )
			{
				szTempValue = szData.Mid(nCol+2);
				(WaferLotInfo)[WT_LOT_HEADER_A_DEFFILE] = szTempValue;
				m_szOsramSDF	= szTempValue;
			}
		}
	
		//Search Number of wafer name
		nCol = szData.Find("N");//NUM_OF_WAFERS
		if ( nCol != -1 )
		{
				szTempValue = szData.Mid(13);
				lTotalWafer = (LONG)atoi((LPCTSTR)szTempValue);		
				(WaferLotInfo)[WT_LOT_HEADER_A_WAFER_NO] = lTotalWafer;
				//AfxMessageBox(szTempValue);
		}

		//Search "[WAFER]" then break
		nCol = szData.Find(".");//WAFER_
			if ( nCol != -1 )
			{

					szTempValue = szData;
					szTempValue.Replace(".csv","");
					szTempValue.Replace(".CSV",",");
					szTempValue.Replace(".sor",",");
					(WaferLotInfo)[WT_LOT_WAFER_NO][nWaferNo] = szTempValue;
					(WaferLotInfo)[WT_LOT_WAFER_NO][nWaferNo][WT_LOT_WAFER_LOADED] = FALSE;
					nWaferNo++;
					//AfxMessageBox(szTempValue);
			}


		nCol = szData.Find(WT_LOT_HEADER_B);
		
		if ( nCol != -1 )
		{
			break;
		}

		nCol = szData.Find(WT_LOT_CVS_HEADER);//Map File Name

		if (nCol != -1)
		{
			bCommaFormat = TRUE;
			break;
		}
	}



	(WaferLotInfo)[WT_LOT_HEADER_A_DEFFILE] = 0;
	(WaferLotInfo)[WT_LOT_HEADER_A_WAFER_NO] = (LONG) nWaferNo-1;
	

	WaferLotInfo.Update();
	WaferLotInfo.Close();

	return TRUE;
}
	

BOOL CWaferTable::ReadLextarLotFile(CString szFileName, CString& szBinSummaryFilename, CString& szPKGFilename)
{
	LONG lWaferNo = 1;
	CStdioFile cfDataFile;
	CStringMapFile	WaferLotInfo;
	CString szTempValue;
	CString szData;
	CString szLotNo;
	CString szInfo1, szInfo2;

	INT nCol = -1;

	 if (cfDataFile.Open(szFileName, 
		CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText) == FALSE)
	 {
		 return FALSE;
	 }

	if (WaferLotInfo.Open(MSD_WAFER_LOT_INFO_FILE, FALSE, TRUE) != 1)
	{
		ShowWaferLotData();
		//HmiMessage("File not found!\nPlease check","Load File Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);
		SetAlert(IDS_WT_LOTFILE_NOT_FOUND);
		CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferLotInfo msd");
		return FALSE;
	}

	cfDataFile.ReadString(szData);

	nCol = szData.Find(',');
	
	if (nCol != -1)
	{
		szLotNo = szData.Left(nCol);
		szData = szData.Right(szData.GetLength() - nCol -1);
	}

	nCol = szData.Find(',');

	if (nCol != -1)
	{
		//szLotNo = szData.Left(nCol);
		szData = szData.Right(szData.GetLength() - nCol -1);
	}

	nCol = szData.Find(',');

	if (nCol != -1)
	{
		szPKGFilename = szData.Left(nCol);
		//szPKGFilename = szData.Right(szData.GetLength() - nCol -1);
	}

	
	(WaferLotInfo)[WT_LOT_LOTNO] = szLotNo;
	szBinSummaryFilename = szLotNo;

	while (cfDataFile.ReadString(szData) != NULL)
	{
		nCol = szData.Find(",");
		
		if (nCol != -1)
		{
			szInfo1 = szData.Left(nCol);
			szData = szData.Right(szData.GetLength() -nCol - 1);

			nCol = szData.Find(",");

			if (nCol != -1)
			{
				szInfo2 = szData.Left(nCol);
				szData = szData.Right(szData.GetLength() -nCol - 1);
			}
			else
			{
				szInfo2 = szData;
			}

			(WaferLotInfo)[WT_LOT_WAFER_NO][lWaferNo] = szInfo2;
			(WaferLotInfo)[WT_LOT_WAFER_NO][lWaferNo][WT_LOT_WAFER_LOADED] = FALSE;
			lWaferNo = lWaferNo + 1;
		}
	}

	(WaferLotInfo)[WT_LOT_HEADER_A_WAFER_NO] =  lWaferNo -1 ;
	(WaferLotInfo)[WT_LOT_HEADER_A_DEFFILE]	=  "NA";

	WaferLotInfo.Update();
	WaferLotInfo.Close();

	return TRUE;
}

// Update the Wafer Lot File Data and Display on the HMI
BOOL CWaferTable::ShowWaferLotData()
{
	CStringMapFile WaferLotInfo;
	CStringArray szaHeader, szaData, szaDataSum;
	LONG i;
	LONG lTotalWafer;
	CString szTempValue;
	CString szDefFileName;
	BOOL bFileExist = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		return MakeWaferIDListFromXML("C:\\MapSorter\\Exe\\Yealy.xml");	//	show data
	}
	//Check file exist 
	if ((_access(MSD_WAFER_LOT_INFO_FILE, 0)) == -1)
	{
		bFileExist = FALSE;
	}
	else
	{
		if ( WaferLotInfo.Open(MSD_WAFER_LOT_INFO_FILE, FALSE, FALSE) != 1 )
		{
			CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferLotInfo msd");
			bFileExist = FALSE;
		}
	}

	m_WaferLotInfoWrapper.Initialize("Lot Info", 0x1000000);
	m_WaferLotInfoWrapper.SetScrollBarSize(15);
	m_WaferLotInfoWrapper.RemoveAllData();

	szaHeader.Add("Name");
	m_WaferLotInfoWrapper.SetHeader(8404992,10987412,"Wafer", 15, szaHeader);

	if ( bFileExist == FALSE )
	{
		lTotalWafer		= 0;
		szDefFileName	= "0";
	}
	else
	{
		lTotalWafer		= (WaferLotInfo)[WT_LOT_HEADER_A_WAFER_NO];
		szDefFileName	= (WaferLotInfo)[WT_LOT_HEADER_A_DEFFILE];
	}
	
	m_szOsramSDF = szDefFileName;
	if (m_szLotInfoFileExt == "lif" || m_szLotInfoFileExt == "LIF")
	{ 
		if ( szDefFileName.IsEmpty() == FALSE  )
		{
			if( m_bEnableAssocFile )
				CMS896AStn::m_WaferMapWrapper.SetAssociateFile(szDefFileName);
			szaData.Add(szDefFileName);
			m_WaferLotInfoWrapper.AddData(8404992, 10987412, 0, 14, szaData);
			szaData.RemoveAll();
		}
	}

	for (i = 1; i <= lTotalWafer; i++)
	{
		szTempValue = (WaferLotInfo)[WT_LOT_WAFER_NO][i];
		szaData.Add(szTempValue);
	
		m_WaferLotInfoWrapper.AddData(8404992, 10987412, (UCHAR)i, 15, szaData);
		szaData.RemoveAll();
	}

	szTempValue.Format("%d", lTotalWafer);
	szaDataSum.Add(szTempValue);

	m_WaferLotInfoWrapper.SetSum(8404992, 10987412, "Total Wafer", 15, szaDataSum);
	m_WaferLotInfoWrapper.UpdateTotal();

	if ( bFileExist == TRUE )
	{
		WaferLotInfo.Close();
	}

	return bFileExist;	
}

VOID CWaferTable::SetWaferId(CString szWaferId)
{
	m_szWaferId = szWaferId;
	SaveData();
}


VOID CWaferTable::UpdateWaferLotLoadStatus()
{
	CStringMapFile	WaferLotInfo;
	LONG i;
	LONG lTotalWafer;
	CString szWaferIdInFile;
	BOOL bIsAllWaferLoaded = TRUE;
	INT nCol = -1;
	
	if (m_bCheckLotWaferName == FALSE)
	{
		return;
	}

	if (m_bEnableWaferLotLoadedMapCheck == FALSE)
	{
		return;
	}

	//Check file exist 
	if ((_access(MSD_WAFER_LOT_INFO_FILE, 0)) == -1)
	{
		return;
	}
	
	if ( WaferLotInfo.Open(MSD_WAFER_LOT_INFO_FILE, FALSE, FALSE) != 1 )
	{
		CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferLotInfo msd");
		return;
	}

	CString szMapFileName;
			
	szMapFileName = GetMapNameOnly();

	lTotalWafer	= (WaferLotInfo)[WT_LOT_HEADER_A_WAFER_NO];

	for (i = 1; i <= lTotalWafer; i++)
	{
		szWaferIdInFile = (WaferLotInfo)[WT_LOT_WAFER_NO][i];
		
		if (szWaferIdInFile == szMapFileName)
		{
			(WaferLotInfo)[WT_LOT_WAFER_NO][i][WT_LOT_WAFER_LOADED] = TRUE;
		}
	}

	WaferLotInfo.Update();
	
	WaferLotInfo.Close();	
	return;
}

BOOL CWaferTable::CheckAllWaferLoaded(CStringArray& szaWaferIds)
{
	CStringMapFile	WaferLotInfo;
	LONG i;
	LONG lTotalWafer;
	CString szWaferId;
	BOOL bWaferIsLoaded;
	BOOL bIsAllWaferLoaded = TRUE;
	
	if (m_bCheckLotWaferName == FALSE)
	{
		return TRUE;
	}

	if (m_bEnableWaferLotLoadedMapCheck == FALSE)
	{
		return TRUE;
	}

	szaWaferIds.RemoveAll();

	//Check file exist 
	if ((_access(MSD_WAFER_LOT_INFO_FILE, 0)) == -1)
	{
		return TRUE;
	}
	
	if ( WaferLotInfo.Open(MSD_WAFER_LOT_INFO_FILE, FALSE, FALSE) != 1 )
	{
		CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferLotInfo msd");
		return TRUE;
	}

	lTotalWafer	= (WaferLotInfo)[WT_LOT_HEADER_A_WAFER_NO];

	for (i = 1; i <= lTotalWafer; i++)
	{
		szWaferId = (WaferLotInfo)[WT_LOT_WAFER_NO][i];
		bWaferIsLoaded = (BOOL)(LONG)(WaferLotInfo)[WT_LOT_WAFER_NO][i][WT_LOT_WAFER_LOADED];

		if (bWaferIsLoaded == FALSE)
		{
			szaWaferIds.Add(szWaferId);
			bIsAllWaferLoaded = FALSE;
		}
	}
	
	WaferLotInfo.Close();
	
	return bIsAllWaferLoaded;
}


BOOL CWaferTable::MakeWaferIDListFromXML(CString szLocalXMLPath)
{
	m_WaferLotInfoWrapper.Initialize("Lot Info", 0x1000000);
	m_WaferLotInfoWrapper.SetScrollBarSize(15);
	m_WaferLotInfoWrapper.RemoveAllData();

	if ( _access( szLocalXMLPath, 0 ) == -1 )
	{
		//revised: just pass when no local file
		//CString szErr = "The WaferLot file does not exist!";
		//HmiMessage(szErr);	//ABORT if file is not here
		return TRUE;
	}

	return FALSE;

	/*
#ifndef MS_DEBUG	//v4.47T5

	FILE *fp;
	mxml_node_t *tree;

	fp = fopen( szLocalXMLPath, "r");
	if( fp==NULL )
	{
		CString szErr = "XML file can not be opened.";
		HmiMessage(szErr);		//ABORT if file is not here
		return FALSE;
	}
	tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	fclose(fp);

	if (!tree)
	{
		CString szErr = "Barcode Summary TREE cannot be created";
		HmiMessage(szErr);		//ABORT if tree memory cannot be created
		return FALSE;
	}

	mxml_node_t *node;
	node = NULL;

	node = mxmlFindElement(tree, tree, "WaferId", NULL, NULL, MXML_DESCEND);
	if (!node)
	{
		CString szErr = "The XML Format ERROR!";
		HmiMessage(szErr);		//ABORT if NODE not found -> no record in file!
		return FALSE;
	}

	CStringArray szaData;
	szaData.Add("Wafer ID");
	m_WaferLotInfoWrapper.SetHeader(8404992,10987412,"ID order", 15, szaData);

	// Get the WaferEndData.msd for saving the Binframe ID
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf = NULL;
	BOOL bIsFirstOpen;
	bIsFirstOpen = ( _access("WaferEndData.msd", 0 ) == -1 );
	if ( pUtl->LoadWaferEndDataStrMapFile() == FALSE )
	{
		HmiMessage( "Load 'WaferEndData.msd' failed!" );
		return FALSE;
	}

	psmf = pUtl->GetWaferEndDataStrMapFile();
	if (psmf == NULL)	//Klocwork	//v4.46
	{
		return FALSE;
	}

	ULONG ulTotalWafers = 1;

	while ( node != NULL )
	{
		mxml_node_t *tempnode;
		tempnode = node;
		CString szValue = (LPCTSTR) node->child->value.text.string;
		CString szBinFrameID = (LPCTSTR) (node->value).element.attrs[0].value;
		CString szSavedID = (*psmf)["Yealy"][szBinFrameID]["BinFrame ID Loaded"];

		if ( bIsFirstOpen == TRUE && szBinFrameID.GetLength() != 0 )
		{
			if ( szSavedID == "FALSE" || szSavedID == "TRUE")
			{
				HmiMessage("Bin Frame ID repeat!");
				DeleteFile(szLocalXMLPath);
				pUtl->ClearWaferEndDataStrMapFile();
				return FALSE;
			}
			OutputDebugString("Binframe: " + szBinFrameID);
			(*psmf)["Yealy"][szBinFrameID]["BinFrame ID Loaded"] = "FALSE";
			(*psmf)["Yealy"][szBinFrameID]["Corresponse WaferID"] = szValue;
		}

		// add to list
		szaData.RemoveAll();
		szaData.Add(szValue);
		// AfxMessageBox(szValue);
		m_WaferLotInfoWrapper.AddData(8404992, 10987412, (UCHAR)ulTotalWafers, 15, szaData);
		szaData.RemoveAll();
		ulTotalWafers++;
		// find next node
		node = mxmlFindElement(tempnode, tree, "WaferId", NULL, NULL, MXML_DESCEND);
	}

	szaData.RemoveAll();
	CString szValue;
	szValue.Format("%d", ulTotalWafers-1);
	szaData.Add(szValue);
	m_WaferLotInfoWrapper.SetSum(8404992, 10987412, "Number of Wafer", 15, szaData);

	pUtl->UpdateWaferEndDataStrMapFile();		// Save the Binframe ID data

	m_WaferLotInfoWrapper.UpdateTotal();
	//m_WaferLotInfoWrapper.UpdateAllData();
	mxmlDelete(tree);
#endif

	return TRUE;
	*/
}

BOOL CWaferTable::SearchWaferIDAndSaveDataFromXML(CString szWaferID)
{
	BOOL bResult = FALSE;
	CString szBinCodeID = szWaferID; // CTM change to check BinCodeID

	if ( m_szSaveMapImagePath.IsEmpty() || _access( m_szSaveMapImagePath, 0) == -1 )
	{
		HmiMessage("The Log Path Error!");
		return FALSE;
	}

	CString szLocalXMLPath = "C:\\MapSorter\\Exe\\Yealy.xml";	//	compare at run time
	if (  _access( szLocalXMLPath, 0 ) == -1 )
	{
		CString szErr = "The WaferLot file does not exist!";
		HmiMessage_Red_Back(szErr);		//ABORT if tree memory cannot be created
		return FALSE;
	}

	return FALSE;	//v4.59A41

	/*
#ifndef MS_DEBUG	//v4.47T5

	FILE *fp = NULL;
	mxml_node_t *tree = NULL;		//Klocwork	//v4.46

	fp = fopen( szLocalXMLPath, "r");
	if (fp != NULL)		//Klocwork	//v4.46
	{
		tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
		fclose(fp);
	}

	CString szFormatError = "The XML Format is different! Please Check!";

	if ( (tree == NULL) || !tree )
	{
		CString szErr = "Barcode Summary TREE cannot be created";
		HmiMessage(szErr);		//ABORT if tree memory cannot be created
		return FALSE;
	}
	else 
	{
		mxml_node_t *node;
		node = NULL;

		//mxml_node_t *node;
		node = mxmlFindElement(tree, tree, "WaferId", NULL, NULL, MXML_DESCEND);
		if (!node)
		{
			CString szErr = "The XML Format ERROR!";
			HmiMessage(szErr);		//ABORT if NODE not found -> no record in file!
			return FALSE;
		}

		// Get the WaferEndData.msd for loading the Binframe ID
		CMSFileUtility  *pUtl = CMSFileUtility::Instance();
		CStringMapFile  *psmf = NULL;
		if ( pUtl->LoadWaferEndDataStrMapFile() == FALSE )
		{
			HmiMessage( "Load 'WaferEndData.msd' failed!" );
			return FALSE;
		}
		psmf = pUtl->GetWaferEndDataStrMapFile();
		if (psmf == NULL)	//Klocwork	//v4.46
		{
			return FALSE;
		}

		//if ( (*psmf)["Yealy"][szBinCodeID]["BinFrame ID Loaded"] == "TRUE" )		// frame already loaded
		//{
		//	SetErrorMessage( szBinCodeID + " is already loaded.");
		//	pUtl->CloseWaferEndDataStrMapFile();
		//	return FALSE;
		//}

		while ( node != NULL )
		{
			mxml_node_t *tempnode;
			tempnode = node;
			CString szValue = (LPCTSTR) node->child->value.text.string;				// waferid
			//AfxMessageBox(szValue);
			CString szBinFrameID = (LPCTSTR) (node->value).element.attrs[0].value;	// binframeID
			CString szBinFrameStatus = (*psmf)["Yealy"][szBinFrameID]["BinFrame ID Loaded"];
			//AfxMessageBox(szBinFrameID);
			// <WaferId binFrameId="aoi00010">41455550111</WaferId>

			//if ( szValue == szWaferID )		// CTM change to check BinCodeID 
			if ( szBinCodeID == szBinFrameID && szBinFrameStatus.GetLength() > 0 )
			{
				(*m_psmfSRam)["WaferTable"]["YealyFilePath_LowestLayer"] = "";
				CString szArrTemp[3];
				for ( int i = 0 ; i < 3 ; i++ )
				{
					node = (node->parent);
					if ( node == NULL ) 
					{
						HmiMessage_Red_Back(szFormatError);
						return FALSE;
					}
					CString szAttribute = (LPCTSTR) (node->value).element.attrs[0].value;
					szArrTemp[i] = szAttribute;
					if ( szAttribute.GetLength() == 0 ) 
					{
						HmiMessage_Red_Back(szFormatError);
						return FALSE;
					}
				}

				(*m_psmfSRam)["WaferTable"]["YealyFilePath_1Layer"] = szArrTemp[2];  // Top == First (series id)
				(*m_psmfSRam)["WaferTable"]["YealyFilePath_2Layer"] = szArrTemp[1];  // (date)
				(*m_psmfSRam)["WaferTable"]["YealyFilePath_3Layer"] = szArrTemp[0];  // (lotnumber name)

				int j = 1;
				CString szVersionNumber = "_1";
				while ( _access( m_szSaveMapImagePath + "\\" + szArrTemp[2] 
								+ "\\" + szArrTemp[1] + "\\" + szArrTemp[0] 
								+ "\\" + szValue + szVersionNumber, 0) == 0 )
				{
					j++;
					szVersionNumber.Format( "_%d", j);					
				}
				(*m_psmfSRam)["WaferTable"]["YealyFilePath_LowestLayer"] = szValue + szVersionNumber; // waferid
				(*psmf)["Yealy"][szBinCodeID]["BinFrame ID Loaded"] = "TRUE";
				//HmiMessage(szValue + szVersionNumber + " Folder is produced");
				bResult = TRUE;
				break;
			}
			node = mxmlFindElement(tempnode, tree, "WaferId", NULL, NULL, MXML_DESCEND);
		}
		pUtl->UpdateWaferEndDataStrMapFile();
	}

	if ( !bResult )
	{
		//HmiMessage(szWaferID + " is not Found in XML");
	}

	mxmlDelete(tree);
#endif
	return bResult;
	*/
}

// Compare the Map File Name with the data in the wafer lot database
BOOL CWaferTable::CompareWaferLotDatabase(CString szMapFileName)
{
	CStringMapFile	WaferLotInfo;
	LONG i;
	LONG lTotalWafer;
	CString szTempValue;
	BOOL bFileExist = TRUE;
	BOOL bNameFound = FALSE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	// Use XML file check for yealy
	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		// save szFileName
		// revised: save in exe folder
		CString szWaferID;
		INT nCol = szMapFileName.ReverseFind('\\');
		if( nCol != -1 )
		{
			szWaferID = szMapFileName.Right(szMapFileName.GetLength() - nCol - 1);
		}
		else 
		{
			szWaferID = szMapFileName;
		}
		nCol = szWaferID.Find(".");
		if( nCol != -1 )
		{
			szWaferID = szWaferID.Left(nCol);
		}
		//HmiMessage(szWaferID);	
		return SearchWaferIDAndSaveDataFromXML(szWaferID);
		//return TRUE;
	}

	// Check file exist 
	if ((_access(MSD_WAFER_LOT_INFO_FILE, 0)) == -1)
	{
		bFileExist = FALSE;
	}
	else
	{
		if ( WaferLotInfo.Open(MSD_WAFER_LOT_INFO_FILE, FALSE, FALSE) != 1 )
		{
			CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferLotInfo msd");
			bFileExist = FALSE;
		}
	}

	if ( bFileExist == TRUE )
	{
		lTotalWafer	= (WaferLotInfo)[WT_LOT_HEADER_A_WAFER_NO];

		for (i = 1; i <= lTotalWafer; i++)
		{
			szTempValue = (WaferLotInfo)[WT_LOT_WAFER_NO][i];
			
			if ( szTempValue == szMapFileName )
			{
				bNameFound = TRUE;
				break;
			}
		}
		WaferLotInfo.Close();
	}

	else
	{
		bNameFound = FALSE;
	}

	return bNameFound;
}


BOOL CWaferTable::ReadSortingCheckPoints(CString szFilename)
{
	BOOL bFoundHeader = FALSE;
	CString szTitle, szContent;
	CString szRowCol, szHeader;
	CStdioFile cfCheckPtFile;

	m_szaSortingCheckPts.RemoveAll();
	
	if ( cfCheckPtFile.Open(szFilename,CFile::modeRead) == FALSE)
	{
		return FALSE;
	}
	
	try
	{	
		
		while (cfCheckPtFile.ReadString(szHeader) == TRUE)
		{
			if (szHeader == "[Start Sorting Check Points]")
			{
				bFoundHeader = TRUE;
				break;
			}
		}

		if (bFoundHeader == FALSE)
		{
			return FALSE;
		}

		while (cfCheckPtFile.ReadString(szRowCol) == TRUE)
		{
			m_szaSortingCheckPts.Add(szRowCol);	
		}
		
		szTitle.LoadString(HMB_WT_SORTING_CHECK_PTS);
		szContent.LoadString(HMB_WT_LOAD_SORTING_CHECK_PTS_FILE_OK);
		HmiMessage(szContent, szTitle);

		cfCheckPtFile.Close();
	}
	catch(CFileException e)
	{
		
		szTitle.LoadString(HMB_WT_SORTING_CHECK_PTS);
		szContent.LoadString(HMB_WT_INVALID_SORTING_CHECK_PTS_FILE);
		m_bEnableSortingCheckPts = FALSE;
		HmiMessage(szContent, szTitle);
		return FALSE;
	}

	return TRUE;
	
}


/*
// Read a line of string e.g pt#1,map row#1, map col#1, pt#2, map row#2, map col#2 .........
BOOL CWaferTable::ReadSortingCheckPoints(CString szFilename)
{
	CString szRowCol;
	CStdioFile cfCheckPtFile;

	m_szaSortingCheckPts.RemoveAll();
	
	if ( cfCheckPtFile.Open(szFilename,CFile::modeRead) == FALSE)
	{
		return FALSE;
	}
	
	try
	{
		INT nCol;
		CString szRow, szCol;
		
		cfCheckPtFile.ReadString(szRowCol);

		while (szRowCol.GetLength() != 0)
		{
			nCol = szRowCol.Find(',');
			
			if (nCol == -1)
				break;

			if ( (nCol+1) <= (szRowCol.GetLength()-1))
				szRowCol = szRowCol.Mid(nCol+1);
			else 
				break;

			nCol = szRowCol.Find(',');

			if (nCol == -1)
				break;

			szRow = szRowCol.Left(nCol);

			if ((nCol+1) <= (szRowCol.GetLength()-1))
				szRowCol = szRowCol.Mid(nCol+1);
			else
				break;

			nCol = szRowCol.Find(',');

			if (nCol == -1)
			{
				szCol = szRowCol;
			}
			else
			{
				szCol = szRowCol.Left(nCol);

				if ( (nCol+1) <= (szRowCol.GetLength()-1))
					szRowCol = szRowCol.Mid(nCol+1);
			}
			m_szaSortingCheckPts.Add(szRow + "," + szCol);
			
		}
		
		cfCheckPtFile.Close();
	}
	catch(CFileException e)
	{
		return FALSE;
	}

	for (INT i=0; i<m_szaSortingCheckPts.GetSize(); i++)
	{
		AfxMessageBox(m_szaSortingCheckPts.GetAt(i),MB_SYSTEMMODAL);
	}

	return TRUE;
}
*/
/*

BOOL CWaferTable::SaveSortingCheckPoints(CString szFilename)
{
	CStdioFile cfCheckPtFile;

	if (cfCheckPtFile.Open(szFilename, CFile::modeCreate|CFile::modeWrite) == FALSE)
	{
		return FALSE;
	}
	try
	{
		for (INT i=0; i<m_szaSortingCheckPts.GetSize(); i++)
		{
			if (i == m_szaSortingCheckPts.GetSize()-1)
				cfCheckPtFile.WriteString(m_szaSortingCheckPts.GetAt(i));
			else
				cfCheckPtFile.WriteString(m_szaSortingCheckPts.GetAt(i) + ",");
		}
	}
	catch(CFileException e)
	{
		return FALSE;
	}

	return TRUE;
}*/

BOOL CWaferTable::SaveSortingCheckPoints(CString szFilename)
{
	CStdioFile cfCheckPtFile;

	if (cfCheckPtFile.Open(szFilename, CFile::modeCreate|CFile::modeWrite) == FALSE)
	{
		return FALSE;
	}
	try
	{
		for (INT i=0; i<m_szaSortingCheckPts.GetSize(); i++)
		{
			cfCheckPtFile.WriteString(m_szaSortingCheckPts.GetAt(i) + "\n");
		}
	}
	catch(CFileException e)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::UpdateOutputFileCtrl(LONG lNoOfOutputCols)
{
	LONG lNoOfPages = 0;

	for (INT i=1; i<WT_MAX_OUTPUT_COL; i++)
	{
		m_szAllOutFileCol[i + (WT_MAX_OUTPUT_COL - 1)*(m_lPrevOutputFilePageNo-1)] = m_szOutFileCol[i];
		m_szOutFileCol[i] = m_szAllOutFileCol[i + (WT_MAX_OUTPUT_COL - 1)*(m_lCurOutputFilePageNo-1)];

		m_bAllOutFileColMin[i + (WT_MAX_OUTPUT_COL - 1)*(m_lPrevOutputFilePageNo-1)] = m_bOutFileColMin[i];
		m_bOutFileColMin[i] = m_bAllOutFileColMin[i + (WT_MAX_OUTPUT_COL - 1)*(m_lCurOutputFilePageNo-1)];

		m_bAllOutFileColMax[i + (WT_MAX_OUTPUT_COL - 1)*(m_lPrevOutputFilePageNo-1)] = m_bOutFileColMax[i];
		m_bOutFileColMax[i] = m_bAllOutFileColMax[i + (WT_MAX_OUTPUT_COL - 1)*(m_lCurOutputFilePageNo-1)];

		m_bAllOutFileColAvg[i + (WT_MAX_OUTPUT_COL - 1)*(m_lPrevOutputFilePageNo-1)] = m_bOutFileColAvg[i];
		m_bOutFileColAvg[i] = m_bAllOutFileColAvg[i + (WT_MAX_OUTPUT_COL - 1)*(m_lCurOutputFilePageNo-1)];

		m_bAllOutFileColStd[i + (WT_MAX_OUTPUT_COL - 1)*(m_lPrevOutputFilePageNo-1)] = m_bOutFileColStd[i];
		m_bOutFileColStd[i] = m_bAllOutFileColStd[i + (WT_MAX_OUTPUT_COL - 1)*(m_lCurOutputFilePageNo-1)];

	}

	for (INT i=1; i<WT_MAX_OUTPUT_COL; i++)
	{
		m_bIfEnableOutColCtrl[i] = FALSE;
		m_lOutColCtrlIndex[i] = (m_lCurOutputFilePageNo - 1) * (WT_MAX_OUTPUT_COL -1) + i;
	}

	if (lNoOfOutputCols == 0)
	{
		return TRUE;
	}

	if (m_lCurOutputFilePageNo == 1)
	{
		m_bEnableOutputBtn = TRUE;
	}
	else
	{
		m_bEnableOutputBtn = FALSE;
	}

	lNoOfPages = lNoOfOutputCols / (WT_MAX_OUTPUT_COL - 1) + 1;

	if (lNoOfOutputCols % (WT_MAX_OUTPUT_COL - 1) == 0)
	{
		lNoOfPages = lNoOfPages -1;
	}

	// last page
	if (m_lCurOutputFilePageNo < lNoOfPages)
	{
		lNoOfOutputCols = WT_MAX_OUTPUT_COL - 1;
	}
	else if (m_lCurOutputFilePageNo == lNoOfPages)
	{
		lNoOfOutputCols = lNoOfOutputCols % (WT_MAX_OUTPUT_COL - 1);

		if (lNoOfOutputCols == 0)
			lNoOfOutputCols = WT_MAX_OUTPUT_COL - 1;
	}
	else 
	{
		lNoOfOutputCols = 0;
	}

	for (LONG i = 1; i <= lNoOfOutputCols; i++)
	{
		m_bIfEnableOutColCtrl[i] = TRUE;
	}

	return TRUE;
}


BOOL CWaferTable::LoadnCheckOutputFileFormatFile(CString szFormatFilename)
{
	CString  szFile, szUserFile;
	CString	 szContents;

	BOOL bFileOK = FALSE;
	CStdioFile cfSelectionFile;

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	szFile		= gszROOT_DIRECTORY + "\\Exe\\OutputFileFormat.txt";
	szUserFile	= gszUSER_DIRECTORY + "\\OutputFile\\" + szFormatFilename;

	//Check file format is valid is correct or not
	bFileOK = cfSelectionFile.Open(szUserFile, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText);

	if (bFileOK	== TRUE)
	{
		cfSelectionFile.SeekToBegin();
		cfSelectionFile.ReadString(szContents);

		if ( (szContents.Find("<Column,Min,Max,Avg,Std>") == -1) 
			|| (szContents.GetLength() != strlen("<Column,Min,Max,Avg,Std>")) )
		{
			bFileOK = FALSE;
		}
		cfSelectionFile.Close();
	}

	if (bFileOK == TRUE)
	{
		//Copy user file to replace ASM file
		CopyFile(szUserFile, szFile, FALSE);

		m_szOutputFormatFileName = szFormatFilename;

		SaveData();
		return TRUE;
	}
	//else	//Klocwork
	//{
	//	return FALSE;
	//}

	return FALSE;
}

BOOL CWaferTable::UpdateWaferMapHeaderCtrl(LONG lNoOfMapCols)
{
	LONG lNoOfPages = 0;

	for (INT i=1; i<WT_MAX_MAP_COL; i++)
	{
		m_szAllWaferMapCol[i + (WT_MAX_MAP_COL - 1)*(m_lPrevDefineHeaderPageNo-1)] = m_szWaferMapCol[i];
		m_szWaferMapCol[i] = m_szAllWaferMapCol[i + (WT_MAX_MAP_COL - 1)*(m_lCurDefineHeaderPageNo-1)];
	}

	for (INT i=1; i<WT_MAX_MAP_COL; i++)
	{
		m_bIfEnableWaferMapColCtrl[i] = FALSE;
		m_lWaferMapColCtrlIndex[i] = (m_lCurDefineHeaderPageNo - 1) * (WT_MAX_MAP_COL -1) + i;
	}

	if (lNoOfMapCols == 0)
	{
		return TRUE;
	}

	if (m_lCurDefineHeaderPageNo == 1)
	{
		m_bEnableHeaderBtn = TRUE;
	}
	else
	{
		m_bEnableHeaderBtn = FALSE;
	}

	lNoOfPages = lNoOfMapCols / (WT_MAX_MAP_COL - 1) + 1;

	if (lNoOfMapCols % (WT_MAX_MAP_COL - 1) == 0)
	{
		lNoOfPages = lNoOfPages -1;
	}

	// last page
	if (m_lCurDefineHeaderPageNo < lNoOfPages)
	{
		lNoOfMapCols = WT_MAX_MAP_COL - 1;
	}
	else if (m_lCurDefineHeaderPageNo == lNoOfPages)
	{
		lNoOfMapCols = lNoOfMapCols % (WT_MAX_MAP_COL - 1);

		if (lNoOfMapCols == 0)
			lNoOfMapCols = WT_MAX_MAP_COL - 1;
	}
	else 
	{
		lNoOfMapCols = 0;
	}

	for (LONG i = 1; i <= lNoOfMapCols; i++)
	{
		m_bIfEnableWaferMapColCtrl[i] = TRUE;
	}

	return TRUE;
}


BOOL CWaferTable::LoadnCheckWaferMapColumnsFile(CString szColumnFilename)
{
	BOOL bFileOK = FALSE;
	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	CStdioFile cfCopyFile, cfSelectionFile;
	CString szSelectionItem, szFile, szUserFile;

	szFile		= gszROOT_DIRECTORY + "\\Exe\\WaferMapColumns.txt";
	szUserFile	= gszUSER_DIRECTORY + "\\OutputFile\\" + szColumnFilename;

	//Check file format is valid is correct or not
	bFileOK = cfSelectionFile.Open(szUserFile, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText);
			
	if (bFileOK == TRUE)
	{
		cfSelectionFile.ReadString(szSelectionItem);
		if (atoi(szSelectionItem) == 0)
		{
			bFileOK = FALSE;
		}

		cfSelectionFile.Close();
	}

	if (bFileOK == TRUE)
	{
		//Copy user file to replace ASM file
		CopyFile(szUserFile, szFile, FALSE);

		cfCopyFile.Open(szFile, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone|CFile::typeText);

		cfCopyFile.SeekToBegin();

		//Write Combo Box selection file for Output File Format
		szFile = gszROOT_DIRECTORY + "\\Exe\\OutputFileColumnsSelection.txt";
		DeleteFile(szFile);

		cfSelectionFile.Open(szFile, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText);

		cfSelectionFile.SeekToBegin();

		cfSelectionFile.WriteString("None,WaferID,Bin Row,Bin Col");

		cfCopyFile.SeekToBegin();

		cfCopyFile.ReadString(szSelectionItem); //skip first line

		while (cfCopyFile.ReadString(szSelectionItem)) 
		{
			cfSelectionFile.WriteString("," + szSelectionItem);
		}

		cfSelectionFile.Close();
		cfCopyFile.Close();

		m_szMapHeaderFileName = szColumnFilename;

		SaveData();

		return TRUE;
	}
	else
	//{		//Klocwork
		//szContent.LoadString(HMB_WT_LOAD_HEADER_FAILED);

		//HmiMessage(szContent, szTitle);
		//return FALSE;
	//}

	return FALSE;
}

INT CWaferTable::MapDieTypeCheck()
{
	CString szDieTypeFromMap = "";
	m_bIsMapDieTypeCheckOk = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDummyMap() )
	{
		return TRUE;
	}

	if (m_bEnableMapDieTypeCheck == FALSE)
	{
		SaveData();
		return TRUE;
	}

	if ( CMS896AStn::m_WaferMapWrapper.GetReader() == NULL )
	{
		return FALSE;
	}


	// Auto get from map file
	if ( CMS896AStn::m_bEnableAutoDieTypeFieldnameChek == TRUE)
	{
	
		if (pApp->GetCustomerName()== "Tekcore")
		{
			CString szWaferID, szWholeLine;
			
			m_WaferMapWrapper.GetLotID(szWholeLine);
			int nIndex = szWholeLine.Find(",");
			
			if( nIndex!=-1 )
			{
				szWaferID = szWholeLine.Left(nIndex);
			}

			nIndex = szWholeLine.ReverseFind(',');
			if( nIndex!=-1 )
			{
				szDieTypeFromMap = szWholeLine.Mid(nIndex);
			}
			szDieTypeFromMap = szDieTypeFromMap.Trim(",\"");
			
			if( m_szUserDefineDieType == "")
			{
				m_szUserDefineDieType = szDieTypeFromMap;
			}
		}
		else
		{
			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(CMS896AStn::m_szDieTypeFieldnameInMap,szDieTypeFromMap);
			szDieTypeFromMap = szDieTypeFromMap.Trim(",\"");

			if( m_szUserDefineDieType == "")
			{
				m_szUserDefineDieType = szDieTypeFromMap;
			}
		}
	}
	// manual input in the interface
	else
	{
		if (CMS896AStn::m_szDieTypeFieldnameInMap != "")
		{
			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(CMS896AStn::m_szDieTypeFieldnameInMap, szDieTypeFromMap);
			szDieTypeFromMap = szDieTypeFromMap.Trim(",\"");
			
			if (pApp->GetCustomerName() == "XinGuangLian")
			{
				int nIndex = szDieTypeFromMap.ReverseFind('.');
				if ( nIndex != -1 )
				{
					szDieTypeFromMap = szDieTypeFromMap.Left(nIndex);
				}
				SetErrorMessage(m_szUserDefineDieType+ " map:" + szDieTypeFromMap + " by:" + CMS896AStn::m_szDieTypeFieldnameInMap);
			}
			else if (pApp->GetCustomerName() == "Primaopto")	//v4.49A2
			{
				if (CMS896AStn::m_WaferMapWrapper.IsMapValid())
				{
					CString szBinTableName;
					CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_BINTABLENAME, szBinTableName);

					szBinTableName.Replace("\"", "");

					CString szMsg;
					szMsg.Format("Primaopto: BinTableName in Load MAP (%s); UserDefined (%s)", 
						szBinTableName, m_szUserDefineDieType);
					CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

					szDieTypeFromMap = szBinTableName;
				}
			}
		}
	}

	if ( (m_szUserDefineDieType.GetLength() > 0) && (m_szUserDefineDieType != szDieTypeFromMap) )
	{
		m_bIsMapDieTypeCheckOk = FALSE;
		SaveData();
		return WT_DIE_TYPE_CHECK_FAIL;
	}
	
	SaveData();
	return TRUE;
}

BOOL CWaferTable::IsMapHeaderCheckOK()
{
	if( m_bEnableMapHeaderCheck==FALSE ||
		m_szMapHeaderCheckWaferId==""	)
		return TRUE;

	BOOL bMatchOK = TRUE;

	CString szMapWaferID, szCheckWaferID, szCheckPart, szCheckOne;
	CString szErrorMsg;
	CStringList szCheckList;
	int nIndex, nIndexOne;

	m_WaferMapWrapper.GetHeaderInfo()->Lookup("WaferID", szMapWaferID);

	nIndex = szMapWaferID.ReverseFind('.');
	if( nIndex!=-1 )
		szMapWaferID = szMapWaferID.Left(nIndex);
	szCheckWaferID = m_szMapHeaderCheckWaferId;

	szErrorMsg.Format("Map wafer ID:%s\nCheck  Input:%s", szMapWaferID, szCheckWaferID);
	SetErrorMessage("Check ID:" + szCheckWaferID + " map:" + szMapWaferID);

	szMapWaferID = szMapWaferID.MakeUpper();
	szCheckWaferID = szCheckWaferID.MakeUpper();

	// break * separator part into list to simplify the finding
	while( 1 )
	{
		nIndex = szCheckWaferID.Find('*');
		if( nIndex==-1 )
		{
			szCheckList.AddTail(szCheckWaferID);
			break;
		}
		szCheckPart = szCheckWaferID.Left(nIndex);
		szCheckList.AddTail(szCheckPart);
		szCheckWaferID = szCheckWaferID.Mid(nIndex+1);
	}

	// check the key word
	POSITION pos;
	for(pos = szCheckList.GetHeadPosition(); pos != NULL; )
	{
		szCheckPart = szCheckList.GetNext(pos);

		while( 1 )
		{
			nIndexOne = szCheckPart.Find('?');
			if( nIndexOne<0 )
			{
				szCheckOne = szCheckPart;
				nIndex = szMapWaferID.Find(szCheckOne);
				if( nIndex==-1 )
					bMatchOK = FALSE;
				else
					szMapWaferID = szMapWaferID.Mid(nIndex+szCheckOne.GetLength());
				break;
			}
			else if( nIndexOne==0 )
			{
				szMapWaferID = szMapWaferID.Mid(1);
				szCheckPart = szCheckPart.Mid(1);
			}
			else
			{
				szCheckOne = szCheckPart.Left(nIndexOne);
				szCheckPart = szCheckPart.Mid(nIndexOne+1);
				nIndex = szMapWaferID.Find(szCheckOne);
				if( nIndex==-1 )
				{
					bMatchOK = FALSE;
					break;
				}
				szMapWaferID = szMapWaferID.Mid(nIndex+szCheckOne.GetLength()+1);
			}
		}

		if( bMatchOK==FALSE )
		{
			break;
		}
	}

	if( bMatchOK==FALSE )
	{
		szErrorMsg += "\nPlease check map file and load it again";
		HmiMessage_Red_Yellow(szErrorMsg, "Wafer Map", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		SetErrorMessage(szErrorMsg);
	}

	return bMatchOK;
}

BOOL CWaferTable::IsMapLoadingOk()
{
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDummyMap() )
	{
		return TRUE;
	}

	if( IsMapDieCheckOk()		!= TRUE ||
		IsMESWaferIDCheckOk()	!= TRUE || 
		IsAutoMapDieCheckOk()	!= TRUE || 
		IsMapHeaderCheckOK()	!= TRUE ||
		IsPackageFileCheckOk()	!= TRUE	)
	{
		return FALSE;
	}

	return TRUE;
}

// remove this function since it the dynamic grade assignment is done by wafermap wrapper
/*
BOOL CWaferTable::IsGradMappingOk()
{
	if( m_bUseOptBinCountDynAssignGrade )
	{
		if( m_bMapGradeMappingOK==FALSE )
		{
			HmiMessage_Red_Yellow("Dynamic Grade Mapping wrong!", "Wafer Map", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
			return FALSE;
		}
	}

	return TRUE;
}
*/

BOOL CWaferTable::IsMapDieCheckOk()
{
	CString szContent, szTitle;

	if (m_bEnableMapDieTypeCheck == FALSE)
	{
		m_bIsMapDieTypeCheckOk = TRUE;
		return TRUE;
	}

	if( (m_bIsMapDieTypeCheckOk == FALSE) || ( m_szUserDefineDieType=="" ) )
	{
		szTitle.LoadString(HMB_WT_DIE_TYPE_CHECK_FAIL);
		szContent.LoadString(HMB_WT_DIE_TYPE_FROM_MAP_MISMATCH);
		HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		return FALSE;
	}

	return TRUE;
}

INT CWaferTable::PackageFileCheck()
{
	BOOL bCheckSubString = FALSE;
	CString szPkgSubString;
	BOOL bCheckPrefixString = FALSE;
	CString szPrefixString;
	INT nCol = -1;

	if (CMS896AStn::m_bPackageFileMapHeaderCheckFunc == FALSE)
	{
		m_bIsPackageFileCheckOk = TRUE;
		return TRUE;
	}

	if (m_bEnablePackageFileCheck == FALSE)
	{
		m_bIsPackageFileCheckOk = TRUE;
		SaveData();
		return TRUE;
	}

	CString szMapPackageFilename;
	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];

	if ( CMS896AStn::m_WaferMapWrapper.GetReader() == NULL )
	{
		m_bIsPackageFileCheckOk = FALSE;
		SaveData();
		return FALSE;
	}

	if (CMS896AStn::m_szPackageFileMapHeaderCheckString.IsEmpty() == TRUE)
	{
		m_bIsPackageFileCheckOk = TRUE;
		SaveData();
		return TRUE;
	}

	CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(CMS896AStn::m_szPackageFileMapHeaderCheckString, szMapPackageFilename);
	szMapPackageFilename = szMapPackageFilename.Trim(",\"");

	nCol = szPKGFilename.Find("_");
	if (nCol != -1)
	{
		bCheckSubString = TRUE;
		szPkgSubString = szPKGFilename.Left(nCol);
	}

	// check whether it is AAA.BBB.pkg format
	szPrefixString = szPKGFilename;
	nCol = szPrefixString.ReverseFind('.');
	if (nCol != -1)
	{
		szPrefixString = szPrefixString.Left(nCol);
		nCol = szPrefixString.ReverseFind('.');
		if (nCol != -1)
		{
			bCheckPrefixString = TRUE;
			szPrefixString = szPrefixString.Left(nCol);
		}
	}

	if (bCheckSubString == TRUE)
	{
		if (szMapPackageFilename != szPkgSubString)
		{
			m_bIsPackageFileCheckOk = FALSE;
			SaveData();
			return FALSE;
		}
	}
	else if (bCheckPrefixString == TRUE)
	{
		if (szMapPackageFilename != szPrefixString)
		{
			m_bIsPackageFileCheckOk = FALSE;
			SaveData();
			return FALSE;
		}
	}
	else
	{
		if (szMapPackageFilename != szPKGFilename)
		{
			m_bIsPackageFileCheckOk = FALSE;
			SaveData();
			return FALSE;
		}
	}

	m_bIsPackageFileCheckOk = TRUE;
	SaveData();

	return TRUE;
}

BOOL CWaferTable::IsPackageFileCheckOk()
{
	CString szTitle, szContent;

	if (CMS896AStn::m_bPackageFileMapHeaderCheckFunc == FALSE)
	{
		return TRUE;
	}

	if (m_bEnablePackageFileCheck == FALSE)
	{
		return TRUE;
	}

	if (m_bIsPackageFileCheckOk == FALSE)
	{
		szTitle.LoadString(HMB_WT_MAP_HEADER_PACKAGE_CHECK);
		szContent.LoadString(HMB_WT_MAP_HEADER_PACKAGE_CHECK_FAIL);
		HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);

		return FALSE;
	}

	return TRUE;
}

//AutoMapDieTypeCheck (for Ubilux)		//v3.31T1
BOOL CWaferTable::AutoMapDieTypeCheck(BOOL bCheck)
{
	BOOL bUpdate = FALSE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->GetCustomerName() == "NeoNeon")
	{
		INT nCol = 0;
		m_bIsCheckSortBinItemOk = TRUE;

		if (m_bEnableAutoMapDieTypeCheck == FALSE)
		{
			return TRUE;
		}

		if (bCheck == FALSE)
		{
			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_SORTBINFNAME, m_szSortBinItem);
			nCol = m_szSortBinItem.Find(',');

			if (nCol != -1)
			{
				m_szSortBinItem = m_szSortBinItem.Left(nCol);
			}
		}
		else
		{
			CString szSortBinItem, szStr;

			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_SORTBINFNAME, szSortBinItem);

			nCol = szSortBinItem.Find(',');

			if (nCol != -1)
			{
				szSortBinItem = szSortBinItem.Left(nCol);
			}

			if (m_szSortBinItem != szSortBinItem)
			{
				m_bIsCheckSortBinItemOk = FALSE;
				
				szStr = "Map Die Type Check Failed! Current:" + szSortBinItem + "Previous:" + m_szSortBinItem;
				//CMSLogFileUtility::Instance()->WT_MapDieTypeCheckLog(szStr);
				SetErrorMessage(szStr);

				SaveData();
				return FALSE;
			}
		}
	}
	else
	{

		//CMS896AStn::m_szMapDieTypeCheckString[0]  = "bin table serial no,ver";
		//CMS896AStn::m_szMapDieTypeCheckString[1]  = "sorting group";	
		//CMS896AStn::m_szMapDieTypeCheckString[2]  = "transfer times";
		//CMS896AStn::m_szMapDieTypeCheckString[3]  = "device";

		for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
		{
			m_baIsMapDieTypeCheckOk[i] = TRUE;
		}
		
		if ( CMS896AStn::m_WaferMapWrapper.GetReader() != NULL )
		{
			BOOL bLog = FALSE;
			CString szStr = "New Lot: ";

			for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
			{
				if (m_szaMapDieTypeCheckCurValue[i] == "")
				{
					bUpdate = TRUE;
				}

				m_baIsMapDieTypeCheckOk[i] = TRUE;

				if (CMS896AStn::m_szMapDieTypeCheckString[i] != "N/A" && CMS896AStn::m_szMapDieTypeCheckString[i] != "" && 
						m_szaMapDieTypeCheckCurValue[i] == "")
				{
					CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(CMS896AStn::m_szMapDieTypeCheckString[i], 
						m_szaMapDieTypeCheckCurValue[i]);

					bLog = TRUE;
					szStr = szStr + CMS896AStn::m_szMapDieTypeCheckString[i] + " = " +  m_szaMapDieTypeCheckCurValue[i] + " ";
				}
			}

			if( bLog==TRUE )
			{
				CMSLogFileUtility::Instance()->WT_MapDieTypeCheckLog(szStr);
			}

			if (m_bEnableAutoMapDieTypeCheck == FALSE)
			{
				return TRUE;
			}

			if (bCheck == TRUE)
			{
				BOOL bCheckOK = TRUE;

				for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
				{
					if (m_baIsSelectMapDieTypeCheck[i] == TRUE)
					{
						if ( CMS896AStn::m_szMapDieTypeCheckString[i] != "N/A" && CMS896AStn::m_szMapDieTypeCheckString[i] != "")
						{
							CString szTemp, szCurMapValue;

							CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(CMS896AStn::m_szMapDieTypeCheckString[i], szCurMapValue);
						
							szCurMapValue = szCurMapValue.Trim(",\"");
							
							szTemp = m_szaMapDieTypeCheckCurValue[i];
							szTemp.Trim(",\"");

							if ( szCurMapValue != szTemp)
							{
								m_baIsMapDieTypeCheckOk[i] = FALSE;
								SaveData();
							}
						}
					}
				}
				
				szStr = "Map Die Type Check Failed: ";

				for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
				{
					if (m_baIsMapDieTypeCheckOk[i] == FALSE)
					{
						bCheckOK = FALSE;
					}

					szStr = szStr + CMS896AStn::m_szMapDieTypeCheckString[i] + " = " +  m_szaMapDieTypeCheckCurValue[i] + " ";
				}

				return bCheckOK;
			}
			

			for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
			{
				
				if (CMS896AStn::m_szMapDieTypeCheckString[i] != "N/A")
				{
					CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(CMS896AStn::m_szMapDieTypeCheckString[i], 
						m_szaMapDieTypeCheckCurValue[i]);
				}

			}

			SaveData();
			return TRUE;
		}
	}

	return TRUE;
}

BOOL CWaferTable::IsAutoMapDieCheckOk()		//v3.31T1
{
	CString szMisMatchField = "";
	CString szTempContent1 = "", szTempContent2 = "";
	CString szContent = "", szTitle = "";

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->GetCustomerName() == "NeoNeon")
	{
		CString szContent,szTitle;
		if (m_bEnableAutoMapDieTypeCheck == FALSE)
		{
			m_bIsCheckSortBinItemOk = TRUE;
			return TRUE;
		}

		if (m_bIsCheckSortBinItemOk == FALSE)
		{
			szTitle.LoadString(HMB_WT_DIE_TYPE_CHECK_FAIL);
			szContent.LoadString(HMB_WT_DIE_TYPE_CHECK_FAILED);
			HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
			return FALSE;
		}
	}
	else
	{
		if (m_bEnableAutoMapDieTypeCheck == FALSE)
		{
			for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
			{
				m_baIsMapDieTypeCheckOk[i] = TRUE;
			}

			return TRUE;
		}
		
		for (INT i=0; i<MS896A_DIE_TYPE_CHECK_LIMIT; i++)
		{
			if (m_baIsMapDieTypeCheckOk[i] == FALSE)
			{
				if (szMisMatchField == "")
				{
					szMisMatchField =  CMS896AStn::m_szMapDieTypeCheckString[i];
				}
				else
				{
					szMisMatchField = szMisMatchField + "," + CMS896AStn::m_szMapDieTypeCheckString[i];
				}
			}
		}

		if (szMisMatchField != "")
		{
			szTitle.LoadString(HMB_WT_DIE_TYPE_CHECK_FAIL);
			szTempContent1.LoadString(HMB_WT_AUTO_DIE_TYPE_CHECK_FAIL_1);
			szTempContent2.LoadString(HMB_WT_AUTO_DIE_TYPE_CHECK_FAIL_2);
			szContent = szTempContent1 + szMisMatchField + "\n\n" + szTempContent2;
			
			HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
			return FALSE;
		}
	}

	return TRUE;
}

INT CWaferTable::ClearMESWaferIDCheck()
{
	m_bMESWaferIDCheckOk = FALSE;
	m_szMESWaferIDCheckMsg = "Wait For MES Reply";
	return 1;
}

INT CWaferTable::MESWaferIDCheck(INT nCode, CString szMsg)
{
	if (CMESConnector::Instance()->IsMESConnectorEnable() == FALSE || nCode == TRUE)
	{
		m_bMESWaferIDCheckOk = TRUE;
		m_szMESWaferIDCheckMsg = "";
		SaveData();
		return TRUE;
	}
	
	m_bMESWaferIDCheckOk = FALSE;
	m_szMESWaferIDCheckMsg = szMsg;
	SaveData();

	return FALSE;
}

BOOL CWaferTable::IsMESWaferIDCheckOk()
{
	if (CMESConnector::Instance()->IsMESConnectorEnable() == FALSE)
	{
		m_bMESWaferIDCheckOk = TRUE;
		m_szMESWaferIDCheckMsg = "";
		return TRUE;
	}

	CString szTitle;
	szTitle.LoadString(HMB_MS_MES_MSG);
	
	if (m_bMESWaferIDCheckOk == FALSE)
	{
		HmiMessage_Red_Yellow(m_szMESWaferIDCheckMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		return FALSE;
	}
	
	return TRUE;
}

BOOL CWaferTable::AutoLoadRankIDFile()
{
	BOOL bReturn = FALSE;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "AutoLoadRankIDFile", stMsg);
	
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
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

BOOL CWaferTable::LoadWaferMapErrorChecking()
{
	CString szLoadWaferMapErr = "";

	if ( m_WaferMapWrapper.GetReader() != NULL )
	{
		m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_WAFERMAP_LOAD_MAP_ERROR_HEADER, szLoadWaferMapErr);

		if (szLoadWaferMapErr == WT_WAFERMAP_GRADE_MAP_ERROR)
		{
			SetAlert_Red_Yellow(IDS_WT_WAFER_GRADE_MAP_ERROR);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CWaferTable::CheckWExpanderLock()
{
	if (!IsEnableWL())
		return TRUE;	//OK

	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	BOOL bReturn = TRUE;

	nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "CheckExpanderLock", stMsg);
	
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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

BOOL CWaferTable::CheckWExpander2Lock()
{
	if (!IsEnableWL())
		return TRUE;	//OK

	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	BOOL bReturn = TRUE;

	nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "CheckExpander2Lock", stMsg);
	
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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

BOOL CWaferTable::SearchMapFileInFolder(CString& szMapFileName, CString &szMapPath, CString szBarCode)
{
	WIN32_FIND_DATA FileData;
	CString szFoundName;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bSearchCompleteFilename = m_bSearchCompleteFilename;

	if( pApp->GetCustomerName()=="Huga" && IsWprWithAF() )
	{
		bSearchCompleteFilename = FALSE;
	}

	HANDLE hSearch = FindFirstFile(szMapPath + "\\" + "*", &FileData);

	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return FALSE;
	}

	do 
	{
		// if it is a folder recurive call to remove file
		szFoundName = FileData.cFileName;
		if( (szFoundName.CompareNoCase(".")==0) || (szFoundName.Compare("..")==0) )
		{
			if( FindNextFile(hSearch, &FileData)==FALSE )
				break;
			continue;
		}

		if ((FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			if (m_bLoadMapSearchDeepInFolder)
			{
				CString	szFolderPath = szMapPath + "\\" + szFoundName;

				if (SearchMapFileInFolder(szMapFileName, szFolderPath, szBarCode))
				{
					FindClose(hSearch);
					szMapPath = szFolderPath;
					return TRUE;
				}
			}
		}
		else 
		{
			CString szLowName = szFoundName.MakeLower();
			CString szLowCode = szBarCode.MakeLower();
			if (bSearchCompleteFilename)
			{
				if( m_szMapFileExtension.IsEmpty()==FALSE )
					szLowCode = szLowCode + "." + m_szMapFileExtension;
				if( szLowName.CompareNoCase(szLowCode)==0 )
				{
					FindClose(hSearch);
					szMapFileName = szFoundName;
					if( pApp->GetCustomerName()=="HuaLei" )
					{
						szMapFileName = szMapFileName.MakeUpper();
					}
					return TRUE;
				}
			}
			else
			{
				if (szLowName.Find(szLowCode) != -1)
				{
					if (m_bEnableSuffix)
					{
						if (szFoundName.Find(m_szSuffixName) != -1)
						{
							FindClose(hSearch);
							szMapFileName = szFoundName;
							if( pApp->GetCustomerName()=="HuaLei" )
							{
								szMapFileName = szMapFileName.MakeUpper();
							}
							return TRUE;
						}
					}
					else
					{
						FindClose(hSearch);
						szMapFileName = szFoundName;
						if( pApp->GetCustomerName()=="HuaLei" )
						{
							szMapFileName = szMapFileName.MakeUpper();
						}
						return TRUE;
					}
				}
			}
		}
	} while (FindNextFile(hSearch, &FileData));
 
	// Close the search handle. 
	FindClose(hSearch);
	return FALSE;
}

LONG CWaferTable::LoadBinSummaryFile(CString szFilename)
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	INT nMsgLength;
	char* pFilename;

	nMsgLength = (szFilename.GetLength() + 1) * sizeof(CHAR);
	pFilename = new char[nMsgLength];
	strcpy_s(pFilename, nMsgLength, szFilename);
	
	stMsg.InitMessage(nMsgLength, pFilename);

	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "AutoLoadBinLotSummaryFile", stMsg);
	
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

	delete [] pFilename;

	return bReturn;
}

BOOL CWaferTable::LotInformation(BOOL bUseFilenameAsWaferID, CString szOperatorID)
{
	INT nIndex = -1;
	CString szWaferId = "";
	CString szLoadMapDate = "", szLoadMapTime = "";
	CTime curTime = CTime::GetCurrentTime();

	szLoadMapDate = curTime.Format("%Y/%m/%d");
	szLoadMapTime = curTime.Format("%H:%M:%S");

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	//CString szTemp;
	LONG i = 0;

	//v4.51A14	//Cree HZ //Too large of this file may cause LoadMap timeout in Cree HZ
	pUtl->CheckAndBackupLotInfoFile();	

	// open config file
    if (pUtl->LoadLotInfoFile() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
    psmf = pUtl->GetLotInfoFile();

	if ( psmf == NULL )
		return FALSE;

	//szOperatorId = (*m_psmfSRam)["MS896A"]["Operator Id"];
	if (bUseFilenameAsWaferID == FALSE)
	{
		m_WaferMapWrapper.GetWaferID(szWaferId);

		if( pApp->GetCustomerName()!="SanAn" && pApp->GetCustomerName()!="TongHui" )
		{
			nIndex = szWaferId.ReverseFind('.');	//cut extension for all waferID (for Ubilux)		//v3.33T1
			if ( nIndex != -1 )
			{
				szWaferId = szWaferId.Left(nIndex);
			}
		}
	}
	else
	{
		szWaferId = GetMapFileName();
		nIndex = szWaferId.ReverseFind('\\');			//cut path

		if (nIndex != -1)
		{
			szWaferId = szWaferId.Mid(nIndex+1);
		}
		SetWaferId(GetMapNameOnly());
	}

	if (szWaferId.IsEmpty() == FALSE)
	{
		if (psmf != NULL)
		{
			(*psmf)["WaferHistory"][szWaferId][WT_LOTINFO_LOAD_MAP_DATE] = szLoadMapDate;
			(*psmf)["WaferHistory"][szWaferId][WT_LOTINFO_LOAD_MAP_TIME] = szLoadMapTime;
			(*psmf)["WaferHistory"][szWaferId][WT_LOTINFO_LOAD_MAP_OPERATORID] = szOperatorID;
		}
	}

    //Check Load/Save Data
    pUtl->UpdateLotInfoFile();

	// close config file
    pUtl->CloseLotInfoFile();

	return TRUE;
}

VOID CWaferTable::SetIgnoreGradeCountCheckStatus(BOOL bSet)
{
	m_bIsIgnoreGradeCountCheckOk = bSet;
}

VOID CWaferTable::SetLoadCurrentMapStatus(BOOL bSet)
{
	m_bLoadCurrentMapStatus = bSet;
	SaveData();
}

BOOL CWaferTable::GetLoadCurrentMapStatus()
{
	return m_bLoadCurrentMapStatus;
}

BOOL CWaferTable::CheckLoadCurrentMapStatus()
{
	if (GetLoadCurrentMapStatus() == FALSE)
	{
		SetErrorMessage("WT: LOADMAP ERR - Cannot create CurrentMap.txt in \\UserData\\MapFile folder");
		SetAlert_Red_Yellow(IDS_WT_CREATE_CURRENT_MAP_FAILED);
		return FALSE;
	}

	return TRUE;
}


BOOL CWaferTable::MoveBinTableToSafePosn(CONST BOOL bSafe)
{
	IPC_CServiceMessage stMsg;
	BOOL bToSafePosn = bSafe;
	stMsg.InitMessage(sizeof(BOOL), &bToSafePosn);

	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("MoveBinTableToSafePosn"), stMsg);

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

BOOL CWaferTable::ReadDynWaferMapOutputFileRefFile()
{
	CStdioFile cfFormatFile;
	BOOL bIfFileExists;
	CString szFilename;
	CString szLineData, szAvg, szMin, szStd, szMax, szFieldname;
	INT nIndex;

	m_szaAllRefOutputFileCol.RemoveAll();
	m_szaAllRefOutFileColMin.RemoveAll();
	m_szaAllRefOutFileColMax.RemoveAll();
	m_szaAllRefOutFileColAvg.RemoveAll();
	m_szaAllRefOutFileColStd.RemoveAll();

	szFilename = gszUSER_DIRECTORY + "\\OutputFile\\OutputFileFormatRef.txt";

	bIfFileExists = cfFormatFile.Open(szFilename,CFile::modeRead|CFile::shareExclusive|CFile::typeText);

	if (bIfFileExists)
	{
		cfFormatFile.SeekToBegin();

		cfFormatFile.ReadString(szLineData);
		
		if (szLineData != WT_OUTPUT_FORMAT_REF_HEADER)
		{
			return FALSE;
		}

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
		if( nIndex!=-1 )
			szFieldname = szLineData.Left(nIndex);

			szLineData = szLineData.Mid(nIndex + 1);
			nIndex = szLineData.Find(",");
		if( nIndex!=-1 )
			szMin = szLineData.Left(nIndex);

			szLineData = szLineData.Mid(nIndex + 1);
			nIndex = szLineData.Find(",");
		if( nIndex!=-1 )
			szMax = szLineData.Left(nIndex);

			szLineData = szLineData.Mid(nIndex + 1);
			nIndex = szLineData.Find(",");
		if( nIndex!=-1 )
			szAvg = szLineData.Left(nIndex);

			szStd = szLineData.Mid(nIndex + 1);
			
			if (szFieldname.IsEmpty() == FALSE)
			{
				m_szaAllRefOutputFileCol.Add(szFieldname);
				m_szaAllRefOutFileColMin.Add(szMin);
				m_szaAllRefOutFileColMax.Add(szMax);
				m_szaAllRefOutFileColAvg.Add(szAvg);
				m_szaAllRefOutFileColStd.Add(szStd);
			}
		}

		cfFormatFile.Close();
	}

	return TRUE;
}

BOOL CWaferTable::DynCreateHeaderAndOutputFileFormat()
{
	if (CMS896AStn::m_bEnableDynMapHeaderFile == FALSE)
	{
		return TRUE;
	}

	CStringArray szaColHead;

	DecodeColumnHeadInMapFile(szaColHead);
	DynCreateWaferMapHeaderFile(szaColHead);
	ReadDynWaferMapOutputFileRefFile();
	DynCreateWaferMapColumnsFile(szaColHead);
	
	return TRUE;
}

BOOL CWaferTable::DynCreateWaferMapHeaderFile(CStringArray& szaColHead)
{
	CStdioFile cfWaferMapColFile, cfSelectionFile;
	CString szNoOfCols;
	CString szAsmMapColFile;
	CString szAsmOPFileColFile;
	CString szUserMapColFile;
	LONG i;

	DeleteFile("WaferMapColumns.txt");
	DeleteFile("OutputFileColumnsSelection.txt");

	szAsmMapColFile		= gszROOT_DIRECTORY + "\\Exe\\WaferMapColumns.txt";
	szAsmOPFileColFile	= gszROOT_DIRECTORY + "\\Exe\\OutputFileColumnsSelection.txt";
	
	m_szMapHeaderFileName = "WaferMapColumns.txt";

	szUserMapColFile	= gszUSER_DIRECTORY + "\\OutputFile\\" + m_szMapHeaderFileName;

	DeleteFile(szAsmMapColFile);
	DeleteFile(szAsmOPFileColFile);
	
	//Do NOT append
	cfWaferMapColFile.Open(szAsmMapColFile, 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);

	cfSelectionFile.Open(gszROOT_DIRECTORY + "\\Exe\\OutputFileColumnsSelection.txt", 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);


	szNoOfCols.Format("%d", szaColHead.GetSize());

	cfWaferMapColFile.WriteString(szNoOfCols + "\n");

	cfSelectionFile.WriteString("None,WaferID,Bin Row,Bin Col");

	for (i=0; i<szaColHead.GetSize(); i++ )
	{
		cfWaferMapColFile.WriteString(szaColHead.GetAt(i) + "\n");

		if (szaColHead.GetAt(i) != "")
		{
			cfSelectionFile.WriteString("," + szaColHead.GetAt(i));
		}
	}

	
	cfWaferMapColFile.Close();
	cfSelectionFile.Close();

	CopyFile(szAsmMapColFile, szUserMapColFile, FALSE);

	//DeleteFile(gszROOT_DIRECTORY + "\\Exe\\OutputFileFormat.txt");
	
	return TRUE;
	
}

BOOL CWaferTable::DynCreateWaferMapColumnsFile(CStringArray& szaColHead)
{
	CStdioFile cfFormatFile;
	LONG i;

	CString szAsmFormatFile;
	CString szUserFormatFile;

	szAsmFormatFile		= gszROOT_DIRECTORY + "\\Exe\\OutputFileFormat.txt";
	m_szOutputFormatFileName = "OutputFileFormat.txt";

	szUserFormatFile	= gszUSER_DIRECTORY + "\\OutputFile\\" + m_szOutputFormatFileName;

	DeleteFile(szAsmFormatFile);

	//Do NOT append
	cfFormatFile.Open("OutputFileFormat.txt", 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);

	cfFormatFile.WriteString("<Column,Min,Max,Avg,Std>\n");

	szaColHead.InsertAt(0, "Bin Col");
	szaColHead.InsertAt(0, "Bin Row");
	szaColHead.InsertAt(0, "WaferID");

	for (i=0; i<szaColHead.GetSize(); i++)
	{
		INT nIndex = -1;

		for (INT j=0; j<m_szaAllRefOutputFileCol.GetSize(); j++)
		{
			if (m_szaAllRefOutputFileCol.GetAt(j) == szaColHead.GetAt(i))
			{
				nIndex = j;
			}
		}

		if (nIndex != -1)
		{
			cfFormatFile.WriteString(m_szaAllRefOutputFileCol.GetAt(nIndex));
			cfFormatFile.WriteString("," + m_szaAllRefOutFileColMin.GetAt(nIndex));
			cfFormatFile.WriteString("," + m_szaAllRefOutFileColMax.GetAt(nIndex));
			cfFormatFile.WriteString("," + m_szaAllRefOutFileColAvg.GetAt(nIndex));
			cfFormatFile.WriteString("," + m_szaAllRefOutFileColStd.GetAt(nIndex) + "\n");
		}
		else
		{
			cfFormatFile.WriteString(szaColHead.GetAt(i) + ",0,0,0,0\n");
		}			
	}

	cfFormatFile.Close();

	CopyFile(szAsmFormatFile, szUserFormatFile, FALSE);

	return TRUE;
}
/// <summary>
/// 
/// </summary>
/// <param name="szaColHead">		</param>
/// <returns>
/// 
/// </returns>
BOOL CWaferTable::DecodeColumnHeadInMapFile(CStringArray& szaColHead)
{
	CString szColumnHead;
	CString szItem, szTemp;
	m_WaferMapWrapper.GetHeaderInfo()->Lookup("ColumnHead", szColumnHead);

	szaColHead.Add("Map Col");
	szaColHead.Add("Map Row");
	szaColHead.Add("Grade");
	
	INT nCol = szColumnHead.Find(",");
	
	while(nCol != -1)
	{
		szItem = szColumnHead.Left(nCol);
		szTemp = szItem;
		
		if ((szTemp.MakeUpper() != "POSX") && (szTemp.MakeUpper() != "POSY") && (szTemp.MakeUpper() != "BIN"))
		{
			szaColHead.Add(szItem);
		}

		szColumnHead = szColumnHead.Right(szColumnHead.GetLength() - nCol -1);
		
		nCol = szColumnHead.Find(",");
	}

	// add last item
	if (szColumnHead.IsEmpty() == FALSE)
	{
		szTemp = szColumnHead;

		if ((szTemp.MakeUpper() != "POSX") && (szTemp.MakeUpper() != "POSY") && (szTemp.MakeUpper() != "BIN"))
		{
			szaColHead.Add(szColumnHead);
		}
	}
	
	return TRUE;
}

BOOL CWaferTable::IsDisableWaferMapGradeSelect()
{
	return m_bDisableWaferMapGradeSelect;
}

BOOL CWaferTable::AddBcHistoryInCurrLot(CONST CString szBCName)
{
	CString szOBarcode = szBCName;
	CStringMapFile pSmfFile;

	if (pSmfFile.Open(MSD_WAFER_BARCODE_FILE, FALSE, TRUE) == 1) 
	{
		(pSmfFile)[szOBarcode]["IsUsed"] = TRUE;
		pSmfFile.Update();
		pSmfFile.Close();
	}

	return TRUE;
}


BOOL CWaferTable::CheckBcHistoryInCurrLot(CONST CString szBCName)	//v3.74T31
{
	CString szOBarcode = szBCName;
	CStringMapFile pSmfFile;
	BOOL bIsUsed = FALSE;

	if (pSmfFile.Open(MSD_WAFER_BARCODE_FILE, FALSE, TRUE) == 1) 
	{
		bIsUsed = (BOOL)(LONG)(pSmfFile)[szOBarcode]["IsUsed"];
		pSmfFile.Update();
		pSmfFile.Close();
	}

	if (bIsUsed)
	{
		return FALSE;	//Duplicated BC found in current lot!
	}

	return TRUE;
}


BOOL CWaferTable::PsmFileExistCheck(CString szRemotePSMFile)
{
	INT nCol = -1;
	CString szBackupPSMFilename;
	
	if (CMS896AStn::m_bEnablePsmFileExistCheck == FALSE)
	{
		return TRUE;
	}


	// Get the filename with extension
	nCol = szRemotePSMFile.ReverseFind('\\');
	if (nCol != -1)
	{
		szRemotePSMFile = szRemotePSMFile.Right(szRemotePSMFile.GetLength() - nCol -1);
	}
	
	szBackupPSMFilename = m_szBackupMapPath + "\\" + szRemotePSMFile;

	if (_access(szBackupPSMFilename, 0) != -1)
	{
		LONG lReply = 0;
		CString szText = " ";

		nCol = szRemotePSMFile.ReverseFind('.');
		if (nCol != -1)
		{
			szRemotePSMFile = szRemotePSMFile.Left(nCol);
		}

		SetErrorMessage("Psm file: " + szRemotePSMFile + " exist");
		
		szText = szRemotePSMFile;

		lReply = SetAlert_Msg_Red_Back(IDS_WT_PSM_EXIST_CHECK, szText , "Continue", "Stop");

		if ( lReply == 1 )
		{
			SetErrorMessage("Psm file exist and machine continue");
			return TRUE;
		}
		else
		{
			SetErrorMessage("Psm file exist and machine stop");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CWaferTable::SetAutoBondMode(BOOL bAutoBondMode)
{
	IPC_CServiceMessage stMsg;
	int		nConvID = 0;
	BOOL	bReturn;

	if( bAutoBondMode == FALSE )
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetPRSysLiveMode", stMsg);
	}
	else
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetPRSysBondMode", stMsg);
	}

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE )
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

BOOL CWaferTable::SearchFolderInMapPath(CString& szSubFolderPath, CString szRootMapPath, CString szSrchName)
{
	CString szFileName, szCurrentPath;
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	BOOL	bFindOk = FALSE;

	hSearch = FindFirstFile(szRootMapPath + "\\" + "*", &FileData);

	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return FALSE;
	}

	do 
	{
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			szFileName = FileData.cFileName;
			if ((szFileName != ".") && ( szFileName != "..") )
			{
				szCurrentPath = szRootMapPath + "\\" + szFileName;
				if( szFileName.CompareNoCase(szSrchName)==0 )
				{
					szSubFolderPath = szCurrentPath;
					bFindOk = TRUE;
					break;
				}
				else
				{
					if (SearchFolderInMapPath(szSubFolderPath, szCurrentPath, szSrchName))
					{
						bFindOk = TRUE;
						break;
					}
				}
			}
		}
	} while (FindNextFile(hSearch, &FileData));
 
	// Close the search handle. 
	FindClose(hSearch);

	return bFindOk;
}

BOOL CWaferTable::SearchFileAndFolder(CString& szOutFileName, CString &szOutPath, CString szInName)
{
	CString szReadPath, szReadName, szFileName;
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;

	if( m_szMapFileExtension.GetLength()>0 )
		szFileName = szInName + "." + m_szMapFileExtension;
	else
		szFileName = szInName;
	hSearch = FindFirstFile(szOutPath + "\\" + "*", &FileData);
	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return FALSE;
	}

	do 
	{
		szReadName = FileData.cFileName;
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if( (szReadName != ".") && (szReadName != "..") )
			{
				szReadPath = szOutPath + "\\" + szReadName;
				if (SearchFileAndFolder(szOutFileName, szReadPath, szInName))
				{
					szOutPath = szReadPath;
					return TRUE;
				}
			}
		}
		else 
		{
			if (szReadName.CompareNoCase(szFileName)==0 )
			{
				szOutFileName = szReadName;
				return TRUE;
			}
		}
	} while (FindNextFile(hSearch, &FileData));
 
	// Close the search handle. 
	FindClose(hSearch);
	return FALSE;
}

BOOL CWaferTable::IsNeedCheckBatchIdFile()
{
	IPC_CServiceMessage stMsg;
	int		nConvID = 0;
	BOOL	bReturn;

	if (CMS896AStn::m_bEnableBatchIdFileCheck == FALSE)
	{
		return FALSE;
	}

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "IsNeedCheckBatchIDFile", stMsg);
	
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

BOOL CWaferTable::CheckBatchIdFile(CString szFilename)
{
	if (CMS896AStn::m_bEnableBatchIdFileCheck == FALSE)
	{
		return TRUE;
	}

	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	INT nMsgLength;
	char* pFilename;

	nMsgLength = (szFilename.GetLength() + 1) * sizeof(CHAR);
	pFilename = new char[nMsgLength];
	strcpy_s(pFilename, nMsgLength, szFilename);
	
	stMsg.InitMessage(nMsgLength, pFilename);
	
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckBatchIDInAutoMode", stMsg);
	
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

	delete [] pFilename;
	return bReturn;
}

LONG CWaferTable::LogItems(LONG lEventNo)
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

LONG CWaferTable::GetLogItemsString(LONG lEventNo, CString& szMsg)
{
	CString szTemp;
	CString szDieType;
	CString szDieNo;
	CString szPath, szAlgorithm, szSortingMode;
	CString szHomeDieRow, szHomeDieCol;
	CString szPrescanPRDelay;
	

	switch(lEventNo)
	{

	case MAP_DIE_TYPE_CHECK:
		if (m_bEnableMapDieTypeCheck)
		{
			szMsg = "Map Die Type Check Function,On";
		}
		else
		{
			szMsg = "Map Die Type Check Function,Off";
		}
		break;

	case MAP_DIE_TYPE_CHECK_DIE_TYPE:
		szMsg = "Map Die Type Check Die Type," + m_szUserDefineDieType;
		break;

	case WAFERMAP_ALGO_PATH:
		m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPath);
		szMsg = "Algorithm Path," + szPath;
		break;

	case WAFERMAP_ALGO_TOUR:
		m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPath);
		szMsg = "Algorithm Tour," + szAlgorithm;
		break;

	case WAFERMAP_ALGO_MODE:
		if (m_WaferMapWrapper.GetPickMode() == WAF_CDieSelectionAlgorithm::IN_GRADE_ORDER)
		{
			szSortingMode = "In Grade";
		}
		else
		{
			szSortingMode = "Mix";
		}
		szMsg = "Algorithm Mode," + szSortingMode;
		break;

	case WT_HOME_DIE_OPTION:
		if (m_ulAlignOption == 0)
		{
			szMsg = "Home Die Input,Manual Input";
		}
		else if (m_ulAlignOption == 1)
		{
			szMsg = "Home Die Input,Use Map File";	
		}	
		else
		{
			szMsg = "Home Die Input,Use SCN File";
		}
		break;

	case WT_HOME_DIE_ROW:
		if (m_ulAlignOption == 0)
		{
			szHomeDieRow.Format("%d", m_lHomeDieMapRow);
		}
		else if (m_ulAlignOption == 1)
		{
			szHomeDieRow.Format("%d", m_lOrgMapRowOffset);
		}	
		else
		{
			szHomeDieRow.Format("%d", m_lScnMapRowOffset);
		}

		szMsg = "Home Die Row No," + szHomeDieRow;
		break;

	case WT_HOME_DIE_COL:
		if (m_ulAlignOption == 0)
		{
			szHomeDieCol.Format("%d", m_lHomeDieMapCol) ;
		}
		else if (m_ulAlignOption == 1)
		{
			szHomeDieCol.Format("%d", m_lOrgMapColOffset);		
		}	
		else
		{
			szHomeDieCol.Format("%d", m_lScnMapColOffset);
		}

		szMsg = "Home Die Col No," + szHomeDieCol;
		break;

	case WT_PRESCAN_DELAY:
		szPrescanPRDelay.Format("%d", m_lPrescanPrDelay);
		szMsg = "Prescan PR Delay," + szPrescanPRDelay;
		break;

	case WT_GRADE_SORT_ORDER:
		if (m_lSortingMode == 0)
		{
			szMsg = "Grade Sorting Order,Default";
		}
		else if (m_lSortingMode == 1)
		{
			szMsg = "Grade Sorting Order,Ascending";
		}
		else if (m_lSortingMode == 2)
		{
			szMsg = "Grade Sorting Order,Descending";
		}
		else
		{
			szMsg = "Grade Sorting Order,User Define";
		}
		break;

	case WT_ADAPTIVE_PREDICTION:

		if (m_bEnableAdaptPredict)
		{
			szMsg = "Adaptive Prediction,On";
		}
		else
		{
			szMsg = "Adaptive Prediction,Off";
		}
		break;

	case WT_PRESCAN:
		
		if (IsPrescanEnable())
		{
			szMsg = "Prescan,On";
		}
		else
		{
			szMsg = "Prescan,Off";
		}
		break;
	}

	return 1;
}

BOOL CWaferTable::MotionTest(VOID)
{
	if( m_bIsMotionTestWaferTableX == FALSE && 
		m_bIsMotionTestWaferTableY == FALSE &&
		m_bIsMotionTestWaferTable2X == FALSE && 
		m_bIsMotionTestWaferTable2Y == FALSE )
	{
		return TRUE;
	}

	BOOL bLogAxis = FALSE;
	CString szTemp;
	INT nTestDistance = (INT)m_lMotionTestDist;

	if ((m_bMoveDirection == TRUE  && m_lMotionTestDist >= 0) || 
		(m_bMoveDirection == FALSE && m_lMotionTestDist < 0))
	{
		bLogAxis = TRUE;
	}

	if (m_bIsMotionTestWaferTableX == TRUE)
	{
	#ifdef NU_MOTION		
		if (labs(nTestDistance) > GetSlowProfileDist())
		{
			X1_Profile(LOW_PROF);
		}
	#endif

		if (m_bMoveDirection)
		{
		}
		else
		{
			nTestDistance = -1* nTestDistance;
		}

		LaserTestOuputBitTrigger(FALSE);

		OpCalculateMoveTableMotionTime(labs(nTestDistance), 0, 0, FALSE);
		if (X1_Move(nTestDistance) != gnOK)
		{
			m_bIsMotionTestWaferTableX = FALSE;
		}

		LaserTestOuputBitTrigger(TRUE);

	#ifdef NU_MOTION
		X1_Profile(NORMAL_PROF);
	#endif

	}
	else if (m_bIsMotionTestWaferTableY == TRUE)
	{
	#ifdef NU_MOTION

		if (m_stWTAxis_Y.m_bUseSCFProfile == FALSE)
		{
			if (labs(nTestDistance) > GetSlowProfileDist())
			{
				Y1_Profile(LOW_PROF);
			}
		}
		else
		{
			// use normal profile string for scf profile control
			Y1_Profile(NORMAL_PROF);
		}
	#endif

		if (m_bMoveDirection)
		{
		}
		else
		{
			nTestDistance = -1* nTestDistance;
		}

		LaserTestOuputBitTrigger(FALSE);

		OpCalculateMoveTableMotionTime(0, labs(nTestDistance), 0, FALSE);
		if (m_stWTAxis_Y.m_bUseSCFProfile == FALSE)
		{
			if (Y1_Move(nTestDistance) != gnOK )
			{
				m_bIsMotionTestWaferTableY = FALSE;
			}
		}
		else
		{
			if (Y1_ScfProfileMove(nTestDistance) != gnOK)
			{
				m_bIsMotionTestWaferTableY = FALSE;	
			}
		}

		LaserTestOuputBitTrigger(TRUE);

	#ifdef NU_MOTION
		Y1_Profile(NORMAL_PROF);
	#endif
	}
	// 4.24TX1
	else if (m_bIsMotionTestWaferTable2X == TRUE)
	{
	#ifdef NU_MOTION		
		if (labs(nTestDistance) > GetSlowProfileDist())
		{
			X2_Profile(LOW_PROF);
		}
	#endif

		if (m_bMoveDirection)
		{
		}
		else
		{
			nTestDistance = -1* nTestDistance;
			
		}

		LaserTestOuputBitTrigger(FALSE);

		OpCalculateMoveTableMotionTime(labs(nTestDistance), 0, 0, FALSE);
		if (X2_Move(nTestDistance) != gnOK)
		{
			m_bIsMotionTestWaferTable2X = FALSE;
		}

		LaserTestOuputBitTrigger(TRUE);

	#ifdef NU_MOTION
		X2_Profile(NORMAL_PROF);
	#endif

	}
	else if (m_bIsMotionTestWaferTable2Y == TRUE)
	{
	#ifdef NU_MOTION
		if (labs(nTestDistance) > GetSlowProfileDist())
		{
			Y2_Profile(LOW_PROF);
		}
	#endif


		if (m_bMoveDirection)
		{
		}
		else
		{
			nTestDistance = -1* nTestDistance;
		}

		LaserTestOuputBitTrigger(FALSE);
	
		OpCalculateMoveTableMotionTime(0, labs(nTestDistance), 0, FALSE);
		if (Y2_Move(nTestDistance) != gnOK )
		{
			m_bIsMotionTestWaferTable2Y = FALSE;
		}

		LaserTestOuputBitTrigger(TRUE);

	#ifdef NU_MOTION
		Y2_Profile(NORMAL_PROF);
	#endif
	}


	//reverse the direction
	m_bMoveDirection = !m_bMoveDirection;

	if (m_lMotionTestDelay > 0)
	{
		Sleep(m_lMotionTestDelay);
	}

	LONG lOffsetX, lOffsetY;
	WftSearchNormalDie(lOffsetX, lOffsetY, FALSE);

	// for motion settling and reverse direction
	if( !IsES101() && !IsES201() )
	{
		Sleep(500);
	}

	return TRUE;
}

VOID CWaferTable::SetLockJoystick(BOOL bLock)
{
	m_bLockJoystick = bLock;
}

BOOL CWaferTable::GetLockJoystick()
{
	return m_bLockJoystick;
}

BOOL CWaferTable::ReVerifyAllReferDie()
{
	ULONG ulMapRowMax = 0, ulMapColMax = 0, ulRow, ulCol;
	LONG lPhyX = 0, lPhyY = 0;

	if (!m_pWaferMapManager->GetWaferMapDimension(ulMapRowMax, ulMapColMax))
	{
		return TRUE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End Check refer die");
	LONG lCheckPrID = 1;
	BOOL bCheckAllReferDie = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CheckAllReferDie"];
	if( bCheckAllReferDie )
		lCheckPrID = 0;

	for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol)!=TRUE )
			{
				continue;
			}
			if( GetDieValidPrescanPosn(ulRow, ulCol, 2, lPhyX, lPhyY, TRUE)!=TRUE )
			{
				continue;
			}

			if( pApp->GetCustomerName()=="Cree" && m_bWaferEndUncheckRefer==FALSE && IsBurnIn() == FALSE && m_bFindAllRefer4Scan )
			{
				BOOL bIsInList = FALSE;
				for(UINT i=0; i<pUtl->GetNumOfReferPoints(); i++)
				{
					LONG lReferRow = 0, lReferCol = 0, lEncX = 0, lEncY = 0;
					if( pUtl->GetReferPosition(i, lReferRow, lReferCol, lEncX, lEncY) )
					{
						if( lReferRow==ulRow && lReferCol==ulCol )
						{
							bIsInList = TRUE;
							break;
						}
					}
				}

				if( bIsInList==FALSE )
				{
					continue;
				}
			}	//	cree, after wafer end, check reference die exist or not.

			if( WftSearchReferDie(ulRow, ulCol, lPhyX, lPhyY, lCheckPrID)==FALSE )
			{
				CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End check refer die fail");
				HmiMessage_Red_Back("Wafer end reverify all refer die fail", "Wafer Table");
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CWaferTable::WftSearchReferDie(ULONG ulRow, ULONG ulCol, LONG lEncX, LONG lEncY, LONG lPrID)
{
	ChangeCameraToWafer();

	m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
	Sleep(100);
	m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);

	if( XY_SafeMoveTo(lEncX, lEncY)==FALSE )
	{
		return FALSE;
	}

	Sleep(50);

	SRCH_TYPE	stSrchInfo;
	REF_TYPE	stInfo;
	IPC_CServiceMessage stMsg;
	int nConvID;
	LONG lDieNo, lRefDieStart, lRefDieStop;
	lRefDieStart = 1;
	lRefDieStop  = (*m_psmfSRam)["WaferPr"]["RefDie"]["Count"];
	if( lPrID>0 )
	{
		lRefDieStart = lRefDieStop  = lPrID;
	}

	for ( lDieNo=lRefDieStart; lDieNo<=lRefDieStop; lDieNo++ )
	{
		stSrchInfo.bShowPRStatus		= FALSE;
		stSrchInfo.bNormalDie			= FALSE;
		stSrchInfo.lRefDieNo			= lDieNo;
		stSrchInfo.bDisableBackupAlign	= TRUE;

		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		if( stInfo.bFullDie == TRUE )
		{
			if( stInfo.bStatus == FALSE )
			{
				return FALSE;
			}
			return TRUE;
		}
	}

	return FALSE;
}

//v4.13T3
VOID CWaferTable::SetEJTSlideUpDn(BOOL bUp)
{
	if (!m_fHardware)
		return;
	//if (m_bDisableWT)
	//	return;
	//if (m_bDisableWLWithExp)
	//	return;
	if (!CMS896AApp::m_bMS100Plus9InchOption)
		return;

	CMS896AStn::MotionSetOutputBit(WT_SO_EJT_SLIDEUP, bUp);
	CMS896AStn::MotionSetOutputBit(WT_SO_EJT_SLIDEDN, !bUp);
}

VOID CWaferTable::SwitchEncoderReader(CONST BOOL bWT2)
{
	if (!m_fHardware)
		return ;

	if( IsES101()==FALSE )
	{
		return ;
	}

	CMS896AStn::MotionSetOutputBit(WT_SO_TABLE_X_READER_SWITCH, bWT2);
	CMS896AStn::MotionSetOutputBit(WT_SO_TABLE_Y_READER_SWITCH, bWT2);
}

BOOL CWaferTable::IsEJTAtUnloadPosn()
{
	if (!m_fHardware)
		return FALSE;
	if (!CMS896AApp::m_bMS100Plus9InchOption)
		return FALSE;
	return CMS896AStn::MotionReadInputBit(WT_SI_EJT_ULimit);
}

VOID CWaferTable::SetWaferClamp(BOOL bOn)
{
	//ONly available for MS899EL for Walsin CHina	//v4.26T1
	//Special upgrade kit for wafer clamp on wafer table
#ifndef NU_MOTION
	if (!m_fHardware)
		return;
	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "Walsin")
		return;

	if (m_bDisableWL)	// && m_bDisableBL & m_bDisableBH)
	{
		CString szLog;
		szLog.Format("WT: SetWaferClamp - %d", bOn);
//HmiMessage(szLog);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		CMS896AStn::MotionSetOutputBit(WT_SO_WAF_CLAMP, bOn);
	}
#endif
}

BOOL CWaferTable::MoveES101EjtElvtZToUpDown(BOOL bToUp)
{
	if( IsEjtElvtInUse() )	// 4.24TX2
	{
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(BOOL), &bToUp);
		INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "MoveEjtElvtToUpDown", stMsg);
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

	return TRUE;
}

BOOL CWaferTable::MoveFocusToWafer(BOOL bWft)
{
	if( IsWprWithAF() )
	{
		SaveScanTimeEvent("    WFT: down focus to wafer");
		BOOL bMove = FALSE;
		LONG lCounter = 0;
		if( bWft )
		{
			switch( m_lFocusZState )
			{
			case 22:
				break;
			case 11:
				while( 1 )
				{
					Sleep(10);
					if( m_lFocusZState!=11 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
				break;
			case 0:
			default:
				bMove = TRUE;
				break;
			}
		}
		else
		{
			switch( m_lFocusZState )
			{
			case 2:
				break;
			case 1:
				while( 1 )
				{
					Sleep(10);
					if( m_lFocusZState!=1 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
				break;
			case 0:
			default:
				bMove = TRUE;
				break;
			}
		}

		if( bMove )
		{
			INT nConvID;
			IPC_CServiceMessage stMsg;
			stMsg.InitMessage(sizeof(BOOL), &bWft);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "GotoTableFocus", stMsg);
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
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "FocusZSync", stMsg);
			while (1)	//	4.24TX 4
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
			if( bWft )
				m_lFocusZState = 22;
			else
				m_lFocusZState = 2;
		}
		SaveScanTimeEvent("    WFT: down focus to wafer done");
	}

	return TRUE;
}

BOOL CWaferTable::MoveES101BackLightZUpDn(BOOL bUp)
{
	BOOL bReturn = TRUE;

	BOOL bMove = FALSE;
	if( IsBLInUse() )	//	4.24TX 4
	{
		if( bUp )
		{
			if( m_lBackLightZStatus==1 )
			{
				SaveScanTimeEvent("    WFT: up BLZ");
				LONG lCounter = 0;
				while( 1 )
				{
					Sleep(10);
					if( m_lBackLightZStatus==2 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
			}
			if( m_lBackLightZStatus==3 )
			{
				SaveScanTimeEvent("    WFT: up BLZ");
				LONG lCounter = 0;
				while( 1 )
				{
					Sleep(10);
					if( m_lBackLightZStatus==0 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
			}
			if( m_lBackLightZStatus==0 )
			{
				bMove = TRUE;
			}
		}
		else
		{
			if( m_lBackLightZStatus==1 )
			{
				SaveScanTimeEvent("    WFT: down BLZ");
				LONG lCounter = 0;
				while( 1 )
				{
					Sleep(10);
					if( m_lBackLightZStatus==2 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
			}
			if( m_lBackLightZStatus==3 )
			{
				SaveScanTimeEvent("    WFT: down BLZ");
				LONG lCounter = 0;
				while( 1 )
				{
					Sleep(10);
					if( m_lBackLightZStatus==0 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
			}
			if( m_lBackLightZStatus==2 )
			{
				bMove = TRUE;
			}
		}
		//	4.24TX 4
		if( IsBurnIn() )
			bMove = TRUE;
	}

	if( bMove )	// 4.24TX 3
	{
	//	if( bUp )
	//		SaveScanTimeEvent("    WFT: BLZ to UP by IPC");
	//	else
	//		SaveScanTimeEvent("    WFT: BLZ to DN by IPC");
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(BOOL), &bUp);
		INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MoveBackLightToUpStandby", stMsg);
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
		if( m_lBackLightTravelTime>0 )
			Sleep(m_lBackLightTravelTime);	// between up and standby
		if( bUp )
			m_lBackLightZStatus = 2;	//	4.24TX 4
		else
			m_lBackLightZStatus = 0;	//	4.24TX 4
		stMsg.GetMsg(sizeof(BOOL), &bReturn);
		if( bReturn==FALSE )	//	4.24TX 3
		{
			SetMotionCE(TRUE, "MoveES101BackLightZUpDn");
		}
		SaveScanTimeEvent("    WFT: move BL by IPC complete");
	}

	return bReturn;
}

BOOL CWaferTable::GenerateConfigData()
{
	CString szConfigFile = (*m_psmfSRam)["MS896A"]["ParameterListPath"];
		//_T("c:\\MapSorter\\UserData\\Parameters.csv");
	CStdioFile oFile;
	CString szLine;

	if (oFile.Open(szConfigFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		return FALSE;
	}
	oFile.SeekToEnd();


	oFile.WriteString("\n[WAFER TABLE]\n");

	szLine.Format("Slow-Profile Trigger dist,%ld\n",	GetSlowProfileDist());
	oFile.WriteString(szLine);
	szLine.Format("Long-Jump Delay,%ld\n",				m_lLongJumpDelay);
	oFile.WriteString(szLine);
	szLine.Format("Long-Jump Motion Time,%ld\n",		m_lLongJumpMotTime);
	oFile.WriteString(szLine);

	oFile.WriteString("\n[WAFER TABLE / MAP Paths]\n");
	szLine = "Current Map Path," + m_szMapFilePath + "\n";
	oFile.WriteString(szLine);
	szLine = "Map Extension," + m_szMapFileExtension + "\n";
	oFile.WriteString(szLine);
	szLine = "Current Map Name," + GetMapNameOnly() + "\n";
	oFile.WriteString(szLine);
	szLine = "Current Map Display," + m_szMapDisplayFilename + "\n";
	oFile.WriteString(szLine);
	szLine = "Backup Map Path," + m_szBackupMapPath + "\n";
	oFile.WriteString(szLine);
	szLine = "Backup Map Extension," + m_szBackupMapExtension + "\n";
	oFile.WriteString(szLine);
	szLine = "Prescan Backup Map Path," + m_szPrescanBackupMapPath + "\n";
	oFile.WriteString(szLine);

	oFile.WriteString("\n[WAFER TABLE / MAP Settings]\n");
	szLine = "MAP Tour," + m_szMapTour + "\n";
	oFile.WriteString(szLine);
	szLine = "MAP Path," + m_szMapPath + "\n";
	oFile.WriteString(szLine);
	szLine.Format("Map PICK Mode,%d\n",				m_ucMapPickMode);
	oFile.WriteString(szLine);
	szLine.Format("Map Die Step,%lu\n",				m_ulMapDieStep);
	oFile.WriteString(szLine);
	szLine.Format("Map Rotation,%d\n",				m_ucMapRotation);
	oFile.WriteString(szLine);
	szLine.Format("Wafer Rotation,%d\n",			m_ucAutoWaferRotation);
	oFile.WriteString(szLine);
	szLine.Format("ALIGN Option,%lu\n",				m_ulAlignOption);
	oFile.WriteString(szLine);
	szLine.Format("ALIGN HOME-DIE Row,%ld\n",		m_lHomeDieMapRow);
	oFile.WriteString(szLine);
	szLine.Format("ALIGN HOME-DIE Column,%ld\n",	m_lHomeDieMapCol);
	oFile.WriteString(szLine);

	CMS896AApp *pApp = (CMS896AApp*)AfxGetApp();
	if (pApp->GetCustomerName() == "EverVision")
	{
		oFile.WriteString("[Wafer Table Setup]\n");
		
		szLine.Format("Wafer Limit/Inch,%.2f\n",   m_dWaferDiameter);
		oFile.WriteString(szLine);
	}

	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		szLine.Format("AutoSearchHome,%ld\n",	m_bAutoSrchHome);
		oFile.WriteString(szLine);
		szLine.Format("WaferName,%s\n",	GetMapFileName());
		oFile.WriteString(szLine);

		CString szAlgorithm, szTemp;
		m_WaferMapWrapper.GetAlgorithm(szAlgorithm,szTemp);
		szLine.Format("Algorithm,%s\n",	szAlgorithm);
		oFile.WriteString(szLine);

		szLine.Format("Map Die Pos Error,%f,%f\n",	m_lPitchX_Tolerance, m_lPitchY_Tolerance);
		oFile.WriteString(szLine);
		szLine.Format("Enable Die Pitch Check, %d\n",	m_bDiePitchCheck);
		oFile.WriteString(szLine);
		szLine.Format("Enable PR Check, %d\n",	m_bScnCheckByPR);
		oFile.WriteString(szLine);
		szLine.Format("Enable SCN Check, %d\n",	m_bEnableSCNCheck);
		oFile.WriteString(szLine);
		szLine.Format("Use Ref Die, %d\n",	m_bScnCheckIsRefDie);
		oFile.WriteString(szLine);
	}
	oFile.Close();
	return TRUE;
}

BOOL CWaferTable::MoveToUnloadLoadDualTable()	// ES
{
	LONG nReturn;
	CString szText;
	CStringList szSelection;
	LONG lSelection = 0, lResult;

	//szSelection.AddTail(szText);

	szSelection.AddTail("WaferTable 1 Camera Position");
	szSelection.AddTail("WaferTable 1 Unload Position");
	szSelection.AddTail("WaferTable 1 Ejector Position");
	if( IsESDualWT() )
	{
		szSelection.AddTail("WaferTable 2 Camera Position");
		szSelection.AddTail("WaferTable 2 Unload Position");
		szSelection.AddTail("WaferTable 2 Ejector Position");
	}

	lResult = HmiSelection("Please select item", "Motor Selection", szSelection, lSelection);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001

	//v2.78T1
	if (IsEnableWL())
	{
		if ( IsWL1ExpanderSafeToMove() == FALSE )
		{
			return TRUE;
		}

		if( IsESDualWL() )
		{
			if ( IsWL2ExpanderSafeToMove() == FALSE )
			{
				return TRUE;
			}
		}
	}
	
	SetJoystickOn(FALSE);

	//StartLoadingAlert();	// Show a alert box
	if (MoveBackLightToSafeLevel() == FALSE)
	{	
		return TRUE;
	}

	if (MoveEjectorElevatorToSafeLevel() == FALSE)
	{
		return TRUE;
	}

	// WT1 Move To Camera
	if (lResult == 0)
	{
		HomeTable2(FALSE);
		SaveScanTimeEvent("WFT: from unload to home begin");
	#ifdef NU_MOTION
		X1_Profile(LOW_PROF1);
		Y1_Profile(LOW_PROF1);
	#endif

		LONG lX = 0, lY = 0;
		GetHomeDiePhyPosn(lX, lY);
		nReturn = XY1_MoveTo(lX, lY, SFM_WAIT);

	#ifdef NU_MOTION
		X1_Profile(NORMAL_PROF);
		Y1_Profile(NORMAL_PROF);
	#endif
		SaveScanTimeEvent("WFT: from unload to home end");
		RotateWFTTUnderCam(FALSE);
	} 
	// WT1 Move To Unload
	else if (lResult == 1)
	{
	#ifdef NU_MOTION
		X1_Profile(LOW_PROF1);
		Y1_Profile(LOW_PROF1);
	#endif

		WFTTGoHomeUnderCam(FALSE);
		nReturn = XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);

	#ifdef NU_MOTION
		X1_Profile(NORMAL_PROF);
		Y1_Profile(NORMAL_PROF);
	#endif
	}
	// WT1 Move To Ejector
	else if (lResult == 2)
	{
		LONG lPosX, lPosY;
		HomeTable2(FALSE);
	
	#ifdef NU_MOTION
		X1_Profile(LOW_PROF1);
		Y1_Profile(LOW_PROF1);
	#endif

		lPosX = m_lHomeDiePhyPosX + m_lEjtOffsetX;
		lPosY = m_lHomeDiePhyPosY + m_lEjtOffsetY;

		nReturn = XY1_MoveTo(lPosX, lPosY, SFM_WAIT);

	#ifdef NU_MOTION
		X1_Profile(NORMAL_PROF);
		Y1_Profile(NORMAL_PROF);
	#endif	
	
	}
	// WT2 Move To Camera
	else if (lResult == 3)
	{
		LONG lPosX, lPosY;
		HomeTable1(FALSE);
	
	#ifdef NU_MOTION
		X2_Profile(LOW_PROF1);
		Y2_Profile(LOW_PROF1);
	#endif

		GetWT2HomeDiePhyPosn(lPosX, lPosY);

		nReturn = XY2_MoveTo(lPosX, lPosY, SFM_WAIT);

		RotateWFTTUnderCam(TRUE);
	#ifdef NU_MOTION
		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);
	#endif
	}
	// move WT2 to unload
	else if (lResult == 4)
	{
	//	HomeTable1(FALSE);
	
	#ifdef NU_MOTION
		X2_Profile(LOW_PROF1);
		Y2_Profile(LOW_PROF1);
	#endif

		WFTTGoHomeUnderCam(TRUE);
		nReturn = XY2_MoveTo(m_lUnloadPhyPosX2, m_lUnloadPhyPosY2, SFM_WAIT);

	#ifdef NU_MOTION
		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);
	#endif
	}
	// move to unload
	else if (lResult == 5)
	{
		LONG lPosX, lPosY;
		HomeTable1(FALSE);
	
	#ifdef NU_MOTION
		X2_Profile(LOW_PROF1);
		Y2_Profile(LOW_PROF1);
	#endif

		lPosX = m_lHomeDiePhyPosX + m_lWT2OffsetX + m_lEjtOffsetX;
		lPosY = m_lHomeDiePhyPosY + m_lWT2OffsetY + m_lEjtOffsetY;

		nReturn = XY2_MoveTo(lPosX, lPosY, SFM_WAIT);

	#ifdef NU_MOTION
		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);
	#endif
	}

	return TRUE;
}

BOOL CWaferTable::IsWL1ExpanderSafeToMove()
{
	BOOL bReturn = TRUE;
if( IsESDualWL() )
{
}
else
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "IsExpanderSafeToMove", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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
	return bReturn;
}

BOOL CWaferTable::IsWL2ExpanderSafeToMove()
{
	BOOL bReturn = TRUE;
	if( IsESDualWL() )
	{
		CWinApp* pApp = AfxGetApp();
		int nStatus = 0;
		nStatus = pApp->GetProfileInt(gszPROFILE_SETTING, gszEXPANDER2_STATUS, 0);	//Klocwork	//v4.41
		if( nStatus==1 )
		{
			bReturn = FALSE;
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 is opened!!");
			SetErrorMessage("Expander2 is opened!!");
			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
		}
	}

	return bReturn;
}

VOID CWaferTable::SetupMapSequenceParameters()
{
	m_WaferMapWrapper.SetAlgorithmParameter("Use Category",  m_bUseWaferMapCategory);
}

BOOL CWaferTable::CheckIgnoreGradeCount()
{
	CString szTitle;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (CMS896AStn::m_bCheckIgnoreGradeCount == FALSE)
	{
		return TRUE;
	}

	if (m_bIsIgnoreGradeCountCheckOk == TRUE)
	{
		return TRUE;
	}

	if (m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		return TRUE;
	}

	CString szIgnoreGradeCount;
	ULONG ulLeft, ulPicked, ulTotal;
	if( pApp->GetCustomerName()=="BlueLight" )
	{
		m_WaferMapWrapper.GetStatistics((unsigned char)(m_lGradeCount + m_WaferMapWrapper.GetGradeOffset()), ulLeft, ulPicked, ulTotal);
	}
	else
	{
		m_WaferMapWrapper.GetStatistics((unsigned char)(CMS896AStn::m_ulIgnoreGrade + m_WaferMapWrapper.GetGradeOffset()), ulLeft, ulPicked, ulTotal);
	}
	szTitle.Format("Please input grade:%d die count", /*CMS896AStn::m_ulIgnoreGrade*/m_lGradeCount);
	
	if (HmiStrInputKeyboard(szTitle, szIgnoreGradeCount) == FALSE)
	{
		szTitle.LoadString(HMB_WT_COMPARE_DIE_COUNT_FAIL);
		HmiMessage_Red_Back(szTitle);
		return FALSE;
	}
	


	if (atoi(szIgnoreGradeCount) != ulTotal)
	{
		szTitle.LoadString(HMB_WT_COMPARE_DIE_COUNT_FAIL);
		HmiMessage_Red_Back(szTitle);
		return FALSE;
	}

	SetIgnoreGradeCountCheckStatus(TRUE);
	
	return TRUE;
}

BOOL CWaferTable::SelectWaferCamera()
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SelectWaferCamera", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

BOOL CWaferTable::CheckPrefixNameForEpitop(CString szBarcode,CString szPrefixName)
{
	int nPrefixName = szPrefixName.GetLength();
	int nBarcode = szBarcode.GetLength();

	if (nBarcode >= nPrefixName)
		szBarcode = szBarcode.Left(nPrefixName);

	CString szTemp1,szTemp2;
	for (int i = 0;i < nPrefixName; i ++)
	{
		szTemp1 = szPrefixName.GetAt(i);
		szTemp2 = szBarcode.GetAt(i);
		if (szTemp1 != szTemp2 && szTemp1 != "*")
		{
			return FALSE;
		}
	}
		
	return TRUE;
}


ULONG CWaferTable::CP_GetGridRows()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulGridRows = pApp->GetProfileInt(gszPROFILE_SETTING, _T("CP Grid Rows"), 40);
	if( ulGridRows<1 )
	{
		ulGridRows = 10;
	}
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("CP Grid Rows"), ulGridRows);

	return ulGridRows;
}

ULONG CWaferTable::CP_GetGridCols()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulGridCols = pApp->GetProfileInt(gszPROFILE_SETTING, _T("CP Grid Cols"), 40);
	if( ulGridCols<1 )
	{
		ulGridCols = 10;
	}
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("CP Grid Cols"), ulGridCols);

	return ulGridCols;
}

BOOL CWaferTable::SelectBondCamera()
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SelectBondCamera", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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


VOID CWaferTable::WriteUserParameterLog(CString szMessage)
{
	WriteParameterLog(m_bHmiOperationLogOption, szMessage);
}

BOOL CWaferTable::SwitchColletOffset(BOOL bBH1, BOOL bAddOffset)
{
	LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
	LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
	LONG lOffsetX,lOffsetY;
	GetColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, (m_dXYRes * 1000), TRUE);

	if (bBH1)//already move BH2 collet offset, switch to BH1 collet offset
	{
		lOffsetX = -lCollet2OffsetX + lCollet1OffsetX;
		lOffsetY = -lCollet2OffsetY + lCollet1OffsetY;
	}
	else//already move BH1 collet offset, switch to BH2 collet offset
	{
		lOffsetX = -lCollet1OffsetX + lCollet2OffsetX;
		lOffsetY = -lCollet1OffsetY + lCollet2OffsetY;
	}

	if (bAddOffset)
	{
		lOffsetX = lOffsetX + 100;
		lOffsetY = lOffsetY + 50;
	}
	else
	{
		lOffsetX = lOffsetX - 100;
		lOffsetY = lOffsetY - 50;
	}

	X_Move(lOffsetX, SFM_NOWAIT);
	Y_Move(lOffsetY, SFM_WAIT);
	CString szTemp;
	szTemp.Format("WT - Siwtch collet offset, BH1,%d,X,%d,Y,%d,Collet1Offset(%d,%d),Collet2Offset(%d,%d)",
		bBH1,lOffsetX,lOffsetY,lCollet1OffsetX,lCollet1OffsetY,lCollet2OffsetX,lCollet2OffsetY);
	DisplaySequence(szTemp);

	return TRUE;
}

BOOL CWaferTable::GoToNoDiePos(INT nMode)
{
	X_MoveTo(m_lWTNoDiePosX, SFM_NOWAIT);
	Y_MoveTo(m_lWTNoDiePosY, SFM_NOWAIT);

	if (nMode == SFM_WAIT)
	{
		X_Sync();
		Y_Sync();
	}
	return TRUE;
}

BOOL CWaferTable::GoTo(LONG lX, LONG lY, INT nMode)
{
	GetEncoderValue();
	LONG lDistX = m_lEnc_X - lX;
	LONG lDistY = m_lEnc_Y - lY;
	m_dDelayTime_X = X_ProfileTime(NORMAL_PROF, lDistX, lDistX);
	m_dDelayTime_Y = Y_ProfileTime(NORMAL_PROF, lDistY, lDistY);
	
	m_dWTStartTime = GetCurTime();
	X_MoveTo(lX, SFM_NOWAIT);
	Y_MoveTo(lY, SFM_NOWAIT);

	if (nMode == SFM_WAIT)
	{
		X_Sync();
		Y_Sync();
	}
	
	return TRUE;
}


BOOL CWaferTable::WTMove(LONG lX, LONG lY, INT nMode)
{
	LONG lDistX = lX;
	LONG lDistY = lY;
	m_dDelayTime_X = X_ProfileTime(NORMAL_PROF, lDistX, lDistX);
	m_dDelayTime_Y = Y_ProfileTime(NORMAL_PROF, lDistY, lDistY);
	
	m_dWTStartTime = GetCurTime();
	X_Move(lX, SFM_NOWAIT);
	Y_Move(lY, SFM_NOWAIT);

	if (nMode == SFM_WAIT)
	{
		X_Sync();
		Y_Sync();
	}
	
	return TRUE;
}


BOOL CWaferTable::UpdateWaferYield(VOID)
{
	m_ulGoodDieTotal = m_lTimeSlot[23];//	23.	good die total 
	m_ulTotalDieCounter = m_lTimeSlot[24];//	24. total die counter 
	m_dWaferYield = ((double)m_ulGoodDieTotal + (double)m_ulNewPickCounter) / (double)m_ulTotalDieCounter * 100;

	return TRUE;
}


BOOL CWaferTable::LoadPackageMsdWTData(VOID)
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

	//	Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	//	WT Prescan Setting
	m_ulPrescanMethod		= (*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_PRESCAN_MODE];
	m_lPrescanPrDelay		= (*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_PRESCAN_PR_DELAY];

	//	Rescan Setting
	m_lPredictMethod		= (*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_RESCAN_MODE];
	m_bAutoRescanAllDice	= (BOOL)(LONG)(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_AUTO_RESCAN_ALL_DICE];
	m_ulAdvStage1StartCount	= (*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_1_START];
	m_ulAdvStage1SampleSpan	= (*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_1_FREQ];
	m_ulAdvStage2StartCount	= (*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_2_START];
	m_ulAdvStage2SampleSpan	= (*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_2_FREQ];
	m_ulAdvStage3StartCount	= (*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_3_START];
	m_ulAdvStage3SampleSpan	= (*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_3_FREQ];

	m_lPitchX_Tolerance		= (*psmf)[WT_DATA][WT_PITCH_X_TOLERANCE];
	m_lPitchY_Tolerance		= (*psmf)[WT_DATA][WT_PITCH_Y_TOLERANCE];
	
	m_ulRescanSampleSpan	= (*psmf)[WT_ALIGN_DATA][WT_RESCAN_DEB_SAMPLE_SPAN];
	m_ulRescanStartCount	= (*psmf)[WT_ALIGN_DATA][WT_RESCAN_DEB_SAMPLE_START];
	m_lLFPitchToleranceX	= (*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LF_PITCH_TOL_X];
	m_lLFPitchToleranceY	= (*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LF_PITCH_TOL_Y];
	m_lLFPitchErrorLimit	= (*psmf)[WT_PSCAN_OPTIONS][WT_LF_PE_LIMIT];
	

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to load WT data from PackageData.msd");

	(*m_psmfSRam)["WaferTable"]["Prescan Pr Delay"] = m_lPrescanPrDelay;
	// close config file
    pUtl->ClosePackageDataConfig();

	return TRUE;
}


BOOL CWaferTable::SavePackageMsdWTData(VOID)
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

	//	Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	//	WT Prescan Setting
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_PRESCAN_MODE]		= m_ulPrescanMethod;
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_PRESCAN_PR_DELAY]	= m_lPrescanPrDelay;

	//	Rescan Setting
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_RESCAN_MODE]		= m_lPredictMethod;
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_AUTO_RESCAN_ALL_DICE] = m_bAutoRescanAllDice;
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_1_START]		= m_ulAdvStage1StartCount;
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_1_FREQ]		= m_ulAdvStage1SampleSpan;
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_2_START]		= m_ulAdvStage2StartCount;
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_2_FREQ]		= m_ulAdvStage2SampleSpan;
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_3_START]		= m_ulAdvStage3StartCount;
	(*psmf)[WT_DATA][WT_PRESCAN_SETTING][WT_STAGE_3_FREQ]		= m_ulAdvStage3SampleSpan;

	(*psmf)[WT_DATA][WT_PITCH_X_TOLERANCE]						= m_lPitchX_Tolerance;
	(*psmf)[WT_DATA][WT_PITCH_Y_TOLERANCE]						= m_lPitchY_Tolerance;
	
	(*psmf)[WT_ALIGN_DATA][WT_RESCAN_DEB_SAMPLE_SPAN]	= m_ulRescanSampleSpan;
	(*psmf)[WT_ALIGN_DATA][WT_RESCAN_DEB_SAMPLE_START]	= m_ulRescanStartCount;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LF_PITCH_TOL_X]	= m_lLFPitchToleranceX;
	(*psmf)[WT_PSCAN_OPTIONS][WT_PSCAN_LF_PITCH_TOL_Y]	= m_lLFPitchToleranceY;
	(*psmf)[WT_PSCAN_OPTIONS][WT_LF_PE_LIMIT]			= m_lLFPitchErrorLimit;

	(*m_psmfSRam)["WaferTable"]["Prescan Pr Delay"] = m_lPrescanPrDelay;
	pUtl->UpdatePackageDataConfig();	
	// close config file
    pUtl->ClosePackageDataConfig();
	
	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to save WT data from PackageData.msd");

	return TRUE;
}


BOOL CWaferTable::UpdateWTPackageList(VOID)
{
	CStdioFile fTemp;
	CString szContent, szText, szTemp;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\WTPackageList.csv"), CFile::modeCreate| CFile::modeReadWrite))
	{
		return FALSE;
	}

	//	WT Prescan Setting
	szText = WT_PRESCAN_MODE;
	szText = szText + ",";
	szTemp.Format("%d,", m_ulPrescanMethod);
	szContent = szContent + szText + szTemp;

	szText = WT_PRESCAN_PR_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lPrescanPrDelay);
	szContent = szContent + szText + szTemp;

	//	Rescan Setting
	szText = WT_RESCAN_MODE;
	szText = szText + ",";
	szTemp.Format("%d,", m_lPredictMethod);
	szContent = szContent + szText + szTemp;

	szText = WT_AUTO_RESCAN_ALL_DICE;
	szText = szText + ",";
	szTemp.Format("%d,", m_bAutoRescanAllDice);
	szContent = szContent + szText + szTemp;

	szText = WT_STAGE_1_START;
	szText = szText + ",";
	szTemp.Format("%d,", m_ulAdvStage1StartCount);
	szContent = szContent + szText + szTemp;

	szText = WT_STAGE_1_FREQ;
	szText = szText + ",";
	szTemp.Format("%d,", m_ulAdvStage1SampleSpan);
	szContent = szContent + szText + szTemp;

	szText = WT_STAGE_2_START;
	szText = szText + ",";
	szTemp.Format("%d,", m_ulAdvStage2StartCount);
	szContent = szContent + szText + szTemp;

	szText = WT_STAGE_2_FREQ;
	szText = szText + ",";
	szTemp.Format("%d,", m_ulAdvStage2SampleSpan);
	szContent = szContent + szText + szTemp;

	szText = WT_STAGE_3_START;
	szText = szText + ",";
	szTemp.Format("%d,", m_ulAdvStage3StartCount);
	szContent = szContent + szText + szTemp;

	szText = WT_STAGE_3_FREQ;
	szText = szText + ",";
	szTemp.Format("%d,", m_ulAdvStage3SampleSpan);
	szContent = szContent + szText + szTemp;

	szText = WT_PITCH_X_TOLERANCE;
	szText = szText + ",";
	szTemp.Format("%d,", m_lPitchX_Tolerance);
	szContent = szContent + szText + szTemp;

	szText = WT_PITCH_Y_TOLERANCE;
	szText = szText + ",";
	szTemp.Format("%d,", m_lPitchY_Tolerance);
	szContent = szContent + szText + szTemp;

	fTemp.WriteString(szContent);
	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to update WT package data to WTPackageList.csv");
	
	fTemp.Close();

	return TRUE;
}

VOID CWaferTable::WTPRFailureCaseLog(LONG lImageNo)
{
	CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
	if (1/*pPRFailureCaseLog->IsEnableFailureCaselog()*/)//2019.04.08 Matt Hardcode for field study
	{
		//***2019.03.27 Matt Log PR Case
		CloseAlarm();
		CString szMsg;
		szMsg = "Log PR Failure Case...";
		SetAlert_WarningMsg(IDS_MS_WARNING_LOG_PR_FAILURE_CASE, szMsg);
		pPRFailureCaseLog->LogFailureCaseDumpFile(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, lImageNo);
		if (State() == IDLE_Q)
		{
			SetAlarmLamp_Yellow(FALSE, TRUE);
		}
		else
		{
			SetAlarmLamp_Green(FALSE, TRUE);
		}
		CloseAlarm();
	}
}
