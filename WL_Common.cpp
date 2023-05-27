/////////////////////////////////////////////////////////////////
// WL_Common.cpp : Common functions of the CWaferLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, November 25, 2004
//	Revision:	1.00
//
//	By:			BarryChu
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "math.h"
#include "FileUtil.h"
#include "MS896A.h"
#include "BondHead.h"
#include "WaferTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CWaferLoader::InitVariable(VOID)
{
	//m_pStepper_Z			= NULL;
	//m_pStepper_X			= NULL;

	m_bUpdateOutput			= FALSE;
	m_bHome_Z				= FALSE;
	m_bHome_X				= FALSE;
	m_bComm_X				= FALSE;
	m_bComm_Z				= FALSE;
	m_bComm_Z2				= FALSE;

	m_bSel_Z				= TRUE;
	m_bSel_Z2				= FALSE;
	m_bSel_X				= TRUE;
	m_bSel_X2				= FALSE;
	m_bIsEnabled			= FALSE;

	m_bHomeSnr_Z			= FALSE;
	m_bHomeSnr_X			= FALSE;
	m_bULimitSnr_X			= FALSE;
	m_bLLimitSnr_X			= FALSE;
	m_bULimitSnr_Z			= FALSE;
	m_bLLimitSnr_Z			= FALSE;
	m_bIsGearType			= FALSE;
	m_bIsExpDCMotorExist	= FALSE;
	m_bExpDCMotorUseEncoder	= FALSE;
	m_bWLXHasEncoder		= FALSE;
	m_bUseDualTablesOption	= FALSE;
	m_bES201DisableTable2	= FALSE;			//ES201		//v4.34T9
	m_bUseBLAsLoaderZ		= FALSE;
	m_lExpanderType			= WL_EXP_GEAR_FF;	//WL_EXP_VACUUM;

	m_lEnc_Z				= 0;
	m_lEnc_X				= 0;
	m_lEnc_Z2				= 0;
	m_lEnc_X2				= 0;
	m_lEnc_ExpZ				= 0;

	// let first time to load the current slot must set it to TRUE 
	//4.31 
	m_bIsGetCurrentSlot		= TRUE;
	m_bIsGetCurrentSlot2	= TRUE;

	//Commom
	m_bMagazineExist		= FALSE;
	m_bMagazineExist2		= FALSE;
	m_bFrameDetect			= FALSE;
	m_bFrameDetect2			= FALSE;
	m_bFrameJam				= FALSE;
	m_bFrameJam2			= FALSE;
	m_bWaferFrameDetect		= FALSE;
	m_bWaferFrameDetect2	= FALSE;
	m_bScopeLevel			= FALSE;

	//Gear version
	m_bExpanderClose		= FALSE;
	m_bExpanderOpen			= FALSE;
	m_bExpanderLock			= FALSE;
	m_bFrameExist			= FALSE;
	m_bFrameExistOnExp		= TRUE;			//v2.64
	m_bFrameExistOnExp2		= TRUE;			//v2.64
	m_bExpanderStatus		= WL_DOWN;
	m_bIsMagazineFull	= FALSE;
	m_bIsMagazine2Full	= FALSE;

	m_bDisableWT2InAutoBondMode	= FALSE;	//v4.24T11
	m_bIsFramebePreloaded	= FALSE;
	m_lFrameToBeUnloaded	= 0;			//0=none; 1=WT1; 2=WT2		//ES101		//v4.24T11
	m_lFrameToBePreloaded	= 0;			//0=none; 1=WT1; 2=WT2		//ES101		//v4.24T11
	m_bFrameOperationInAutoMode	= FALSE;	// 4.24TX1
	m_bFrameLoadedOnWT1		= FALSE;
	m_bFrameLoadedOnWT2		= FALSE;
	//MS100 Single Loader Buffer Table variables	//v4.31T11
	m_bFrameInUpperSlot		= FALSE;
	m_bFrameInLowerSlot		= FALSE;
	m_szUpperSlotBarcode	= _T("");
	m_szLowerSlotBarcode	= _T("");

	m_bExpander2Close		= FALSE;
	m_bExpander2Open		= FALSE;	
	m_bExpander2Lock		= FALSE;	
	m_bFrameExist2			= FALSE;		
	m_bExpander2Status		= FALSE;	
	m_bLeftLoaderCoverSensor	= FALSE;
	m_bRightLoaderCoverSensor	= FALSE;

	m_bFrontGateSensor		= FALSE;
	m_bBackGateSensor		= FALSE;

	//Vacuum version
	m_bFramePosition		= FALSE;
	m_bFrameProtection		= FALSE;
	m_bExpanderVacuum		= FALSE;
	m_bExpanderLevel		= FALSE;

	m_ulSequenceMode		= WL_AUTO_MODE;
	m_lUnloadPos_X			= 0;		
	m_lLoadPos_X			= 0;		
	m_lReadyPos_X			= 0;		
	m_lBarcodePos_X			= 0;
	m_lBarcodeCheckPos_X	= 0;
	m_l2DBarcodePos_X		= 0;
	m_lBufferLoadPos_X		= 0;
	m_bHomeGripper			= FALSE;
	
	m_lTopSlotLevel_Z		= 0;		
	m_dSlotPitch			= 0.0;
	m_lTotalSlotNo			= 0;
	m_lSkipSlotNo			= 0;			
	m_lCurrentSlotNo		= 1;
	m_lCurrentMagNo			= 1;
	m_lUnloadOffset			= 1000;

	m_lCurrentMagNo2		= 1;

	m_lUnloadPhyPosX		= 0;		
	m_lUnloadPhyPosY		= 0;		
	m_lHotBlowOpenTime		= 0;		
	m_lHotBlowCloseTime		= 0;	
	m_lMylarCoolTime		= 0;		
	m_lOpenDriveInTime		= 0;
	m_lCloseDriveInTime		= 0;
	m_lCloseDriveInTime2	= 150;
	m_lUnloadOpenExtraTime	= 0;		//v4.59A17
	m_lExpDCMotorDacValue	= 20000;
	m_lExpDCMotorDirection	= -1;
	m_lWTAlignFrameCount	= 2;
	m_lExpEncoderOpenOffsetZ	= 0;
	m_lExpEncoderCloseOffsetZ	= 0;

	m_lEjrCapCleanLimit		= 0;
	m_lEjrCapCleanCount		= 0;

	m_bEnableContourGT		= FALSE;
	m_bContourPreloadMap	= FALSE;
	m_dContourExtraMarginX	= 0.5;
	m_dContourExtraMarginY	= 0.5;
	m_bUseContour			= FALSE;
	m_bUseBarcode			= FALSE;
	m_bBarcodeCheck			= FALSE;
	m_bEnable2DBarcodeCheck	= FALSE;
	m_bCompareBarcode		= FALSE;
	m_ucCommPortNo			= 3;
	m_ucBarcodeModel		= WL_BAR_SYMBOL;
	m_lScanRange			= 0;
	m_lBarcodeCheckLength	= 0;
	m_bRotateBackAfterScan	= FALSE;
	m_lWftYScanRange		= 0;
	m_dWftTScanRange		= 0;

	m_lHomeDiePhyPosX		= 0;		
	m_lHomeDiePhyPosY		= 0;		
	m_lWT2HomeDiePhyPosX	= 0;		
	m_lWT2HomeDiePhyPosY	= 0;		
	m_lAngleLHSCount		= 10;		
	m_lAngleRHSCount		= 10;		
	m_lSprialSize			= 0;	
	m_lPRSrchID				= 0;
	m_lAlignWaferAngleMode	= 0;	//Horiz mode

	m_bSearchHomeOption		= WL_SPIRAL_SEARCH;
	m_ulCornerSearchOption	= WL_CORNER_TL;
	m_ucPLLMWaferAlignOption= 0;	//0=DEFAULT, 1=REBEL_TILE, 2=REBEL_U2U	

	m_lWLGeneral_1			= 0;		
    m_lWLGeneral_2			= 0;		
    m_lWLGeneral_3			= 0;		
    m_lWLGeneral_4			= 0;		
    m_lWLGeneral_5			= 0;		
    m_lWLGeneral_6			= 0;		
    m_lWLGeneral_7			= 0;
	m_lWLGeneral_8			= 0;
	m_lWLGeneral_TmpA		= 0;

	m_lStepOnLoadPos		= -150;
	m_lStepOnUnloadPos		= -150;
	m_lVacuumBefore			= 5000;
	m_lVacuumAfter			= 3000;

	m_szLotInfoLotNo		= "";
	m_szLotInfoFilePath		= "";
	m_szLotInfoFileName		= "";
	m_szLotInfoFileExt		= "";
	m_szBarcodeName			= "txt";
	m_dZRes					= GetChannelResolution(MS896A_CFG_CH_WAFLOADER_Z);
	m_dZ2Res				= GetChannelResolution(MS896A_CFG_CH_WAFLOADER2_Z);

	m_bNoSensorCheck		= FALSE;

	// wafer table theta res
	m_dThetaRes				= GetChannelResolution(MS896A_CFG_CH_WAFTABLE_T);	
	m_lThetaMotorDirection	= GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MOTOR_DIRECTION);

	m_bReadBarCodeOnTable	= FALSE;
	
	m_bOpenGripperInUnload	= FALSE;
	m_bOpenGripper2InUnload = FALSE;
	m_bManualLoadFrame		= FALSE;

	m_bUseBLAsLoaderZDisablePreload	= FALSE;

	m_bAutoAdpatWafer		= FALSE;

	//ANDREW_SC
	m_bMapWfrCass			= FALSE;

	m_bWaferAlignComplete	= TRUE;
	m_bUseBCInCassetteFile = FALSE;
	m_szBCInCassetteFilename = "";
	m_szLastLoadPkgName.Empty();

	m_nWaferBarcodeMoveDirection = 1;
	m_lWaferBarcodeTestCount	= 0;
	m_lBarcodeTestMoveStep		= 0;		
	m_lWaferBarcodeReverseCount	= 0;
	m_bStartWaferBarcodeTest	= FALSE;
	m_bContLoadUnloedStart		= FALSE;

	m_lCurrentMagazineX1SlotNo	= 1;
	m_lCurrentMagazineX2SlotNo	= 1;
	// yealy:the "X" magazine 
	m_bUseMagazine3AsX			= FALSE;
	m_lMagazineX_No				= 3;

	m_bUseAMagazineAsG			= FALSE;
	m_lMagazineG_No				= 3;
	m_lMagazineG1CurrentSlotNo	= 1;
	m_lMagazineG2CurrentSlotNo	= 1;

	m_uwWL1ContourRecordID	= 0;
	m_uwWL2ContourRecordID	= 0;

	m_bAutoLineLoadDone			= FALSE;
	m_bAutoLineUnloadDone		= FALSE;

	m_lLoadUnloadObject			= 0;
	m_lLoadUnloadSECSCmdObject	= 0;

	m_lLoadedSlotNo = m_lCurrentMagNo;

	m_bAutoLoadUnloadTest		= FALSE;
	m_lSecsGemHostCommandErr	= 0;

	m_lLoopTestCounter			= 0;
}

BOOL CWaferLoader::LoadWaferLoaderOption()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


    // open config file
    if (pUtl->LoadWLConfig() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
    psmf = pUtl->GetWLConfigFile();
	
	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

    // retrive data
		//v3.70T2
	m_bHomeGripper			= (BOOL)(LONG)(*psmf)[WL_DATA][WL_GRIPPER][WL_POWER_ON_HOME];
	
	m_bOpenGripperInUnload	= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_OPEN_GRIPPER_IN_UNLOAD];
	m_bOpenGripper2InUnload = (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER2][WL_OPEN_GRIPPER_IN_UNLOAD];
	m_bUseBLAsLoaderZDisablePreload	= (BOOL)((LONG)(*psmf)[WL_DATA][WL_LOADER][WL_BL_DISABLE_PRELOAD]);	

	if( m_lExpanderType==100 )
	{
		m_lExpanderType			= (*psmf)[WL_DATA][WL_EXPANDER][WL_EXPANDER_TYPE];
		pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_TYPE, (int)GetExpType());
	}
	m_lExpDCMotorDacValue	= (*psmf)[WL_DATA][WL_EXPANDER][WL_DCMOTOR_DAC];
	m_lExpDCMotorDacValue	= 20000;	//hardcoded value
	m_lWTAlignFrameCount	= (*psmf)[WL_DATA][WL_EXPANDER][WL_WT_ALN_COUNT];
	m_lEjrCapCleanLimit		= (*psmf)[WL_DATA][WL_EXPANDER][WL_EJR_CAP_CLEAN_LIMIT];		//v3.28T2
	m_lEjrCapCleanCount		= (*psmf)[WL_DATA][WL_EXPANDER][WL_EJR_CAP_CLEAN_COUNT];		//v3.28T2

	m_bContourPreloadMap	= (BOOL)((LONG)(*psmf)[WL_DATA][WL_ES_CONTOUR_PRELOAD_MAP]);
	m_bEnableContourGT		= (BOOL)((LONG)(*psmf)[WL_DATA][WL_ES_CONTOUR_DO_GT]);
	m_dContourExtraMarginX	= (*psmf)[WL_DATA][WL_ES_CONTOUR_EXTRA_MARGIN_X];
	m_dContourExtraMarginY	= (*psmf)[WL_DATA][WL_ES_CONTOUR_EXTRA_MARGIN_Y];
	m_bUseContour			= (BOOL)((LONG)(*psmf)[WL_DATA][WL_ES_USE_CONTOUR]);
	m_bUseBarcode			= (BOOL)((LONG)(*psmf)[WL_DATA][WL_BARCODE][WL_USE_BARCODE]);
	m_bCompareBarcode		= FALSE;//(BOOL)((LONG)(*psmf)[WL_DATA][WL_BARCODE][WL_COMPARE_BARCODE]);
	m_bUseBCInCassetteFile	= FALSE;//(BOOL)((LONG)(*psmf)[WL_DATA][WL_BARCODE][WL_USE_BARCODE_FILE]);
	m_bBarcodeCheck			= FALSE;//(BOOL)((LONG)(*psmf)[WL_DATA][WL_BARCODE][WL_NULL_BARCODE_CHECK]);
	m_bEnable2DBarcodeCheck	= (BOOL)((LONG)(*psmf)[WL_DATA][WL_BARCODE][WL_2D_BARCODE_CHECK]);
	m_ucCommPortNo			= (*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_COM_PORT];			
	m_lScanRange			= (*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_RANGE];	
	m_lBarcodeCheckLength	= (*psmf)[WL_DATA][WL_BARCODE][WL_BC_CHECK_LENGTH];
	m_lBarcodeCheckLengthUpperBound = (*psmf)[WL_DATA][WL_BARCODE][WL_BC_CHECK_LENGTH_UPPER_BOUND];
	m_szBarcodeName			= (*psmf)[WL_DATA][WL_BARCODE][WL_BARCODE_NAME];

	if (m_lBarcodeCheckLength == 0)
	{
		m_lBarcodeCheckLengthUpperBound = 0;
	}
	if( m_lBarcodeCheckLengthUpperBound>0 && m_lBarcodeCheckLengthUpperBound<m_lBarcodeCheckLength )
	{
		m_lBarcodeCheckLengthUpperBound = m_lBarcodeCheckLength;
	}

	m_bRotateBackAfterScan	= (BOOL)((LONG)(*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_ROTATE_BACK]);
	m_lWftYScanRange		= (*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_WFT_Y_RANGE];
	m_dWftTScanRange		= (*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_WFT_T_ANGLE];
	if( m_ucBarcodeModel==100 )
	{
		m_ucBarcodeModel		= (*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_MODEL];			
		pApp->WriteProfileInt(gszPROFILE_SETTING, gszWL_BC_READER_MODEL, (int)GetBCModel());
	}
	m_bReadBarCodeOnTable	= FALSE;//(BOOL)((LONG)(*psmf)[WL_DATA][WL_BARCODE][WL_READ_ON_TABLE]);	
	m_lAngleLHSCount		= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_LHS_IDX_COUNT];		
	m_lAngleRHSCount		= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_RHS_IDX_COUNT];		
	m_lSprialSize			= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_MATRIX_SIZE];	
	m_ulCornerSearchOption	= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_CORNER_SRCH_OPTION];	
	m_ucPLLMWaferAlignOption= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_PLLM_WAFER_ALIGN_OPTION];	
	m_bSearchHomeOption		= (BOOL)((LONG)(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_SRCH_HOME_OPTION]);
	m_lAlignWaferAngleMode	= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_ALIGN_ANGLE_MODE];
	m_bAutoAdpatWafer		= (BOOL)(LONG)(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_AUTO_ADAPT_WAFER];
	if( IsMS90HalfSortMode() )
	{
		if( m_ulCornerSearchOption!=WL_CORNER_TL && m_ulCornerSearchOption!=WL_CORNER_TR )
			m_ulCornerSearchOption = WL_CORNER_TL;
	}

	m_szLotInfoFilePath		= (*psmf)[WL_DATA][WL_WAFER_LOT_INFO][WL_LOT_FILE_PATH];

	//v4.46T20	//PLSG
	m_lHotBlowOpenTime		= (*psmf)[WL_DATA][WL_EXPANDER][WL_HOTAIR_OP_TIME];		
	m_lHotBlowCloseTime		= (*psmf)[WL_DATA][WL_EXPANDER][WL_HOTAIR_CL_TIME];	
	m_lMylarCoolTime		= (*psmf)[WL_DATA][WL_EXPANDER][WL_MYLARCOOL_TIME];	
	m_lHomeDiePhyPosX		= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_HOME_PHY_X];		
	m_lHomeDiePhyPosY		= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_HOME_PHY_Y];		

	m_ulSequenceMode		= (*psmf)[WL_DATA][WL_MISC_OPTION][WL_AUTO_SEQUENCE];
	if (m_bDisableWLWithExp || (GetExpType() == WL_EXP_NONE))	//v4.39T10
	{
		m_ulSequenceMode = WL_MANUAL_MODE;
	}

    // close config file
    pUtl->CloseWLConfig();

	if (GetExpType() == WL_EXP_VACUUM)
	{
		m_bIsGearType = FALSE;
	}
	else
	{
		m_bIsGearType = TRUE;
	}

	if ( m_lWTAlignFrameCount <= 0 )
	{
		m_lWTAlignFrameCount = 2;
	}

	//default COM Port = 3
	m_ucCommPortNo = 3;

	//Update SRAM setting
	if( IsWLManualMode() )
	{
		if(pApp->GetCustomerName() != "ChangeLight")
			SetWT2InUse(FALSE);
	}

	//v4.46T7	//Knowles
	BOOL bPostSealOptics	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_POSTSEAL_OPTICS);
	if (!bPostSealOptics)
	{
		m_bEnable2DBarcodeCheck = FALSE;
	}

	//v4.46T21	//Genesis
	//Restore HMI variable
	m_lWLGeneral_1		= m_lUnloadPos_X;		
	m_lWLGeneral_2		= m_lLoadPos_X;		
	m_lWLGeneral_3		= m_lReadyPos_X;		
	m_lWLGeneral_4		= m_lBarcodePos_X;
	
	LONG lMagazineNo = 0;
	if (IsESMachine() || m_bUseBLAsLoaderZ)
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

    return TRUE;
}

BOOL CWaferLoader::LoadData(VOID)
{
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	BOOL bExcludeMachineParam	= (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Exclude Machine Param"];	//v4.33T1	//PLSG
	BOOL bManualLoadPkg			= (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Manual Load Pkg"];		//v4.33T1
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


    // open config file
    if (pUtl->LoadWLConfig() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
    psmf = pUtl->GetWLConfigFile();
	
	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	m_lWT2HomeDiePhyPosX		= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_WT2_HOME_PHY_X];		
	m_lWT2HomeDiePhyPosY		= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_WT2_HOME_PHY_Y];		

	//v4.46T20	//PLSG
	//else
	//{
		// retrive data
		m_lUnloadPos_X			= (*psmf)[WL_DATA][WL_GRIPPER][WL_UNLOAD_POS];
		m_lLoadPos_X			= (*psmf)[WL_DATA][WL_GRIPPER][WL_LOAD_POS];		
		m_lReadyPos_X			= (*psmf)[WL_DATA][WL_GRIPPER][WL_READY_POS];		
		m_lBarcodePos_X			= (*psmf)[WL_DATA][WL_GRIPPER][WL_SCAN_POS];
		m_lBarcodeCheckPos_X	= (*psmf)[WL_DATA][WL_GRIPPER][WL_SCAN_CHECK_POS];
		m_l2DBarcodePos_X		= (*psmf)[WL_DATA][WL_GRIPPER][WL_2DSCAN_POS];					//v3.70T2
		m_lBufferLoadPos_X		= (*psmf)[WL_DATA][WL_GRIPPER][WL_BUFLOAD_POS];					//v4.31T11
		m_lStepOnUnloadPos		= (*psmf)[WL_DATA][WL_GRIPPER][WL_STEPON_UNLOAD_POS];			//v2.93T2
		m_lStepOnLoadPos		= (*psmf)[WL_DATA][WL_GRIPPER][WL_STEPON_LOAD_POS];				//v2.93T2
		m_bIsMagazineFull		= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_MAGAZINE_FULL];
		m_lFrameToBeUnloaded	= (*psmf)[WL_DATA][WL_LOADER][WL_FRAME_TOBEUNLOADED];			//v4.24T11
		m_bIsFramebePreloaded	= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_IS_FRAME_PRELOADED];		//v4.24T11
		m_bDisableWT2InAutoBondMode	= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_DISABLE_WT2];	//v4.24T11
		if( IsESDualWL()==FALSE )
		{
			m_bDisableWT2InAutoBondMode = TRUE;
		}
		m_bFrameLoadedOnWT1		= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_LAOD_ON_WT1];
		m_bFrameLoadedOnWT2		= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_LAOD_ON_WT2];
		//WL Machine Status
		m_bFrameExistOnExp		= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_EXIST];		//v2.64
		m_bFrameInUpperSlot		= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_EXIST_IN_UBUFFER];
		m_bFrameInLowerSlot		= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_EXIST_IN_LBUFFER];
		m_szUpperSlotBarcode	= (*psmf)[WL_DATA][WL_LOADER][WL_BC_IN_UBUFFER];
		m_szLowerSlotBarcode	= (*psmf)[WL_DATA][WL_LOADER][WL_BC_IN_LBUFFER];

		m_lBinLoader_Y			= (*psmf)[WL_DATA][WL_LOADER][BL_LOADER_Y];	
		m_lBufTableUSlot_Z		= (*psmf)[WL_DATA][WL_LOADER][WL_BUF_UPSLOT_Z];				
		m_lBufTableLSlot_Z		= (*psmf)[WL_DATA][WL_LOADER][WL_BUF_DNSLOT_Z];				

		m_lUnloadPos_X2			= (*psmf)[WL_DATA][WL_GRIPPER2][WL_UNLOAD_POS];
		m_lLoadPos_X2			= (*psmf)[WL_DATA][WL_GRIPPER2][WL_LOAD_POS];		
		m_lReadyPos_X2			= (*psmf)[WL_DATA][WL_GRIPPER2][WL_READY_POS];		
		m_lBarcodePos_X2		= (*psmf)[WL_DATA][WL_GRIPPER2][WL_SCAN_POS];
		m_lBarcodeCheckPos_X2	= (*psmf)[WL_DATA][WL_GRIPPER2][WL_SCAN_CHECK_POS];
		m_l2DBarcodePos_X2		= (*psmf)[WL_DATA][WL_GRIPPER2][WL_2DSCAN_POS];					//v3.70T2
		m_lStepOnUnloadPos2		= (*psmf)[WL_DATA][WL_GRIPPER2][WL_STEPON_UNLOAD_POS];			//v2.93T2
		m_lStepOnLoadPos2		= (*psmf)[WL_DATA][WL_GRIPPER2][WL_STEPON_LOAD_POS];				//v2.93T2

		m_bFrameExistOnExp2		= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER2][WL_FRAME_EXIST];		//v2.64
		m_bIsMagazine2Full		= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER2][WL_MAGAZINE_FULL];

		for (INT i=0; i<WL_MAG_NO ; i++)
		{
			CString szStr;
			szStr.Format("%s%d", WL_MAG, i+1);

			m_stWaferMagazine[i].m_lTopLevel_Z		= (*psmf)[WL_DATA][WL_LOADER][szStr][WL_TOP_SLOT_POS];
			m_stWaferMagazine[i].m_dSlotPitch		= (*psmf)[WL_DATA][WL_LOADER][szStr][WL_SLOT_PITCH];
			
			m_stWaferMagazine[i].m_lNoOfSlots		= (*psmf)[WL_DATA][WL_LOADER][szStr][WL_TOTAL_SLOT];
			if( m_stWaferMagazine[i].m_lNoOfSlots>WL_MAX_MAG_SLOT || m_stWaferMagazine[i].m_lNoOfSlots<1 )
				m_stWaferMagazine[i].m_lNoOfSlots = WL_MAX_MAG_SLOT;

			//v4.56A11	//AutoLine
			CString szStr2;
			for (INT j=0; j<WL_MAX_MGZN_SLOT; j++)
			{
				szStr2.Format("Slot%d", j+1);
				m_stWaferMagazine[i].m_lSlotWIPCounter[j] = (*psmf)[WL_DATA][WL_LOADER][szStr][szStr2][WL_SLOT_WIP_COUNTER];
				m_stWaferMagazine[i].m_lSlotUsage[j] = (*psmf)[WL_DATA][WL_LOADER][szStr][szStr2][WL_SLOT_USAGE];
				m_stWaferMagazine[i].m_SlotBCName[j] = (*psmf)[WL_DATA][WL_LOADER][szStr][szStr2][WL_SLOT_BARCODE];
			}
		}

		for (INT i=0; i<WL_MAG_NO ; i++)
		{
			CString szStr;
			szStr.Format("%s%d", WL_MAG, i+1);

			m_stWaferMagazine2[i].m_lTopLevel_Z		= (*psmf)[WL_DATA][WL_LOADER2][szStr][WL_TOP_SLOT_POS];
			m_stWaferMagazine2[i].m_dSlotPitch		= (*psmf)[WL_DATA][WL_LOADER2][szStr][WL_SLOT_PITCH];
			
			m_stWaferMagazine2[i].m_lNoOfSlots		= (*psmf)[WL_DATA][WL_LOADER2][szStr][WL_TOTAL_SLOT];
			if (m_stWaferMagazine2[i].m_lNoOfSlots>WL_MAX_MAG_SLOT || m_stWaferMagazine2[i].m_lNoOfSlots<1)
				m_stWaferMagazine2[i].m_lNoOfSlots = WL_MAX_MAG_SLOT;

			//v4.56A11	//AutoLine
			CString szStr2;
			for (INT j=0; j<WL_MAX_MGZN_SLOT; j++)
			{
				szStr2.Format("Slot%d", j+1);
				
				m_stWaferMagazine2[i].m_lSlotUsage[j] = (*psmf)[WL_DATA][WL_LOADER2][szStr][szStr2][WL_SLOT_USAGE];
				m_stWaferMagazine2[i].m_SlotBCName[j] = (*psmf)[WL_DATA][WL_LOADER2][szStr][szStr2][WL_SLOT_BARCODE];
			}
		}

		m_lSkipSlotNo2			= (*psmf)[WL_DATA][WL_LOADER2][WL_SKIP_SLOT];			
		m_lCurrentMagNo2		= (*psmf)[WL_DATA][WL_LOADER2][WL_CUR_MGZN];			
		m_lCurrentSlotNo2		= (*psmf)[WL_DATA][WL_LOADER2][WL_CUR_SLOT];			
		m_lUnloadOffset2		= (*psmf)[WL_DATA][WL_LOADER2][WL_UNLOAD_OFFSET];

		m_lVacuumBefore			= (*psmf)[WL_DATA][WL_EXPANDER][WL_VACUUM_BF_TIME];
		m_lVacuumAfter			= (*psmf)[WL_DATA][WL_EXPANDER][WL_VACUUM_AF_TIME];	

		m_lHotBlowOpenTime		= (*psmf)[WL_DATA][WL_EXPANDER][WL_HOTAIR_OP_TIME];		
		m_lHotBlowCloseTime		= (*psmf)[WL_DATA][WL_EXPANDER][WL_HOTAIR_CL_TIME];	
		m_lMylarCoolTime		= (*psmf)[WL_DATA][WL_EXPANDER][WL_MYLARCOOL_TIME];	
		m_lOpenDriveInTime		= (*psmf)[WL_DATA][WL_EXPANDER][WL_OPEN_DRIVEIN];
		m_lCloseDriveInTime		= (*psmf)[WL_DATA][WL_EXPANDER][WL_CLOSE_DRIVEIN];
		m_lUnloadOpenExtraTime	= (*psmf)[WL_DATA][WL_EXPANDER][WL_UNLOAD_DRIVEIN];		//v4.59A17
		m_lExpEncoderOpenOffsetZ	= (*psmf)[WL_DATA][WL_EXPANDER][WL_OPEN_DRIVEIN_OFFSETZ];
		m_lExpEncoderCloseOffsetZ	= (*psmf)[WL_DATA][WL_EXPANDER][WL_CLOSE_DRIVEIN_OFFSETZ];

		m_lHotBlowOpenTime2		= (*psmf)[WL_DATA][WL_EXPANDER2][WL_HOTAIR_OP_TIME];		
		m_lHotBlowCloseTime2	= (*psmf)[WL_DATA][WL_EXPANDER2][WL_HOTAIR_CL_TIME];	
		m_lMylarCoolTime2		= (*psmf)[WL_DATA][WL_EXPANDER2][WL_MYLARCOOL_TIME];	
		m_lOpenDriveInTime2		= (*psmf)[WL_DATA][WL_EXPANDER2][WL_OPEN_DRIVEIN];
		m_lCloseDriveInTime2	= (*psmf)[WL_DATA][WL_EXPANDER2][WL_CLOSE_DRIVEIN];

		m_lPRSrchID				= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_HOME_SEARCH_ID];

		m_lALUnload_Z			= (*psmf)[WL_DATA][WL_LOADER][WL_AL_UNLOAD_Z];	//autoline1
		m_lTopSlotLevel_Z		= (*psmf)[WL_DATA][WL_LOADER][WL_TOP_SLOT_POS];
		m_dSlotPitch			= (*psmf)[WL_DATA][WL_LOADER][WL_SLOT_PITCH];			
		m_lTotalSlotNo			= (*psmf)[WL_DATA][WL_LOADER][WL_TOTAL_SLOT];			
		m_lSkipSlotNo			= (*psmf)[WL_DATA][WL_LOADER][WL_SKIP_SLOT];			
		m_lCurrentMagNo			= (*psmf)[WL_DATA][WL_LOADER][WL_CUR_MGZN];	
		m_lCurrentSlotNo		= (*psmf)[WL_DATA][WL_LOADER][WL_CUR_SLOT];	
		m_lUnloadOffset			= (*psmf)[WL_DATA][WL_LOADER][WL_UNLOAD_OFFSET];

		//m_lUnloadPhyPosX		= (*psmf)[WL_DATA][WL_EXPANDER][WL_UNLOAD_PHY_X];		
		//m_lUnloadPhyPosY		= (*psmf)[WL_DATA][WL_EXPANDER][WL_UNLOAD_PHY_Y];

		m_lUnloadPhyPosX2		= (*psmf)[WL_DATA][WL_EXPANDER2][WL_UNLOAD_PHY_X];		
		m_lUnloadPhyPosY2		= (*psmf)[WL_DATA][WL_EXPANDER2][WL_UNLOAD_PHY_Y];		

		m_lHomeDiePhyPosX		= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_HOME_PHY_X];		
		m_lHomeDiePhyPosY		= (*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_HOME_PHY_Y];		
	//}

	m_lCurrentMagazineX1SlotNo	= (*psmf)[WL_DATA][WL_LOADER][WL_CURRENT_SLOT_OF_X];
	m_lCurrentMagazineX2SlotNo	= (*psmf)[WL_DATA][WL_LOADER2][WL_CURRENT_SLOT_OF_X2];
	m_bUseMagazine3AsX			= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_ES_MAGAZINE3_AS_X];
	if( m_lCurrentMagazineX1SlotNo<=0 )
		m_lCurrentMagazineX1SlotNo = 1;
	if( m_lCurrentMagazineX2SlotNo<=0 )
		m_lCurrentMagazineX2SlotNo = 1;

	m_bUseAMagazineAsG			= (BOOL)(LONG)(*psmf)[WL_DATA][WL_LOADER][WL_ES_MAGAZINE_AS_G];
	m_lMagazineG1CurrentSlotNo	= (*psmf)[WL_DATA][WL_LOADER][WL_ES_G1_CUR_SLOTNO];
	m_lMagazineG2CurrentSlotNo	= (*psmf)[WL_DATA][WL_LOADER][WL_ES_G2_CUR_SLOTNO];
	if( m_lMagazineG1CurrentSlotNo<=0 )
		m_lMagazineG1CurrentSlotNo = 1;
	if( m_lMagazineG2CurrentSlotNo<=0 )
		m_lMagazineG2CurrentSlotNo = 1;
	if( m_bUseAMagazineAsG && m_bUseMagazine3AsX )
		m_lMagazineG_No	= 2;
	else
		m_lMagazineG_No	= 3;


    // close config file
    pUtl->CloseWLConfig();

	if (m_lCurrentMagNo < 1)
		m_lCurrentMagNo = 1;
	if (m_lCurrentMagNo > GetWL1MaxMgznNo())
		m_lCurrentMagNo = GetWL1MaxMgznNo();

	if (m_lCurrentMagNo2 < 1)
		m_lCurrentMagNo2 = 1;
	if (m_lCurrentMagNo2 > GetWL2MaxMgznNo())
		m_lCurrentMagNo2 = GetWL2MaxMgznNo();

	(*m_psmfSRam)["WaferTable"]["WT1LoadMgzn"]	= m_lCurrentMagNo;
	(*m_psmfSRam)["WaferTable"]["WT1LoadSlot"]	= m_lCurrentSlotNo;
	(*m_psmfSRam)["WaferTable"]["WT2LoadMgzn"]	= m_lCurrentMagNo2;
	(*m_psmfSRam)["WaferTable"]["WT2LoadSlot"]	= m_lCurrentSlotNo2;

	m_lLoadedSlotNo = m_lCurrentSlotNo;
    return TRUE;
}


BOOL CWaferLoader::SaveData(VOID)
{
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

    // open config file
    if (pUtl->LoadWLConfig() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
	psmf = pUtl->GetWLConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

    // update data
	(*psmf)[WL_DATA][WL_GRIPPER][WL_UNLOAD_POS]			= m_lUnloadPos_X;
	(*psmf)[WL_DATA][WL_GRIPPER][WL_LOAD_POS]			= m_lLoadPos_X;		
	(*psmf)[WL_DATA][WL_GRIPPER][WL_READY_POS]			= m_lReadyPos_X;		
	(*psmf)[WL_DATA][WL_GRIPPER][WL_SCAN_POS]			= m_lBarcodePos_X;
	(*psmf)[WL_DATA][WL_GRIPPER][WL_SCAN_CHECK_POS]		= m_lBarcodeCheckPos_X;
	(*psmf)[WL_DATA][WL_GRIPPER][WL_2DSCAN_POS]			= m_l2DBarcodePos_X;
	(*psmf)[WL_DATA][WL_GRIPPER][WL_BUFLOAD_POS]		= m_lBufferLoadPos_X;		//v4.31T11
	(*psmf)[WL_DATA][WL_GRIPPER][WL_POWER_ON_HOME]		= m_bHomeGripper;
	(*psmf)[WL_DATA][WL_GRIPPER][WL_STEPON_UNLOAD_POS]	= m_lStepOnUnloadPos;		//v2.93T2
	(*psmf)[WL_DATA][WL_GRIPPER][WL_STEPON_LOAD_POS]	= m_lStepOnLoadPos;			//v2.93T2

	// gripper2
	(*psmf)[WL_DATA][WL_GRIPPER2][WL_UNLOAD_POS]		= m_lUnloadPos_X2;
	(*psmf)[WL_DATA][WL_GRIPPER2][WL_LOAD_POS]			= m_lLoadPos_X2;		
	(*psmf)[WL_DATA][WL_GRIPPER2][WL_READY_POS]			= m_lReadyPos_X2;		
	(*psmf)[WL_DATA][WL_GRIPPER2][WL_SCAN_POS]			= m_lBarcodePos_X2;
	(*psmf)[WL_DATA][WL_GRIPPER2][WL_SCAN_CHECK_POS]	= m_lBarcodeCheckPos_X2;
	(*psmf)[WL_DATA][WL_GRIPPER2][WL_2DSCAN_POS]		= m_l2DBarcodePos_X2;	
	(*psmf)[WL_DATA][WL_GRIPPER2][WL_POWER_ON_HOME]		= m_bHomeGripper2;
	(*psmf)[WL_DATA][WL_GRIPPER2][WL_STEPON_UNLOAD_POS]	= m_lStepOnUnloadPos2;			//v2.93T2
	(*psmf)[WL_DATA][WL_GRIPPER2][WL_STEPON_LOAD_POS]	= m_lStepOnLoadPos2;			//v2.93T2
	
	(*psmf)[WL_DATA][WL_LOADER][BL_LOADER_Y]			= m_lBinLoader_Y;				//v4.31T11	//Yealy MS100Plus
	(*psmf)[WL_DATA][WL_LOADER][WL_BUF_UPSLOT_Z]		= m_lBufTableUSlot_Z;				
	(*psmf)[WL_DATA][WL_LOADER][WL_BUF_DNSLOT_Z]		= m_lBufTableLSlot_Z;				

	(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_EXIST]				= m_bFrameExistOnExp;			//v2.64
	(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_EXIST_IN_UBUFFER]	= m_bFrameInUpperSlot;
	(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_EXIST_IN_LBUFFER]	= m_bFrameInLowerSlot;
	(*psmf)[WL_DATA][WL_LOADER][WL_BC_IN_UBUFFER]			= m_szUpperSlotBarcode;
	(*psmf)[WL_DATA][WL_LOADER][WL_BC_IN_LBUFFER]			= m_szLowerSlotBarcode;

	(*psmf)[WL_DATA][WL_LOADER][WL_MAGAZINE_FULL]		= m_bIsMagazineFull;
	(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_TOBEUNLOADED]	= m_lFrameToBeUnloaded;			//v4.24T11
	(*psmf)[WL_DATA][WL_LOADER][WL_IS_FRAME_PRELOADED]	= m_bIsFramebePreloaded;		//v4.24T11
	if( IsESDualWL()==FALSE )
	{
		m_bDisableWT2InAutoBondMode = TRUE;
	}
	(*psmf)[WL_DATA][WL_LOADER][WL_DISABLE_WT2]			= m_bDisableWT2InAutoBondMode;	//v4.24T11
	(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_LAOD_ON_WT1]	=	m_bFrameLoadedOnWT1;
	(*psmf)[WL_DATA][WL_LOADER][WL_FRAME_LAOD_ON_WT2]	=	m_bFrameLoadedOnWT2;

	(*psmf)[WL_DATA][WL_LOADER][WL_AL_UNLOAD_Z]			= m_lALUnload_Z;	//autoline1

	(*psmf)[WL_DATA][WL_LOADER][WL_TOP_SLOT_POS]		= m_lTopSlotLevel_Z;
	(*psmf)[WL_DATA][WL_LOADER][WL_SLOT_PITCH]			= m_dSlotPitch;			
	(*psmf)[WL_DATA][WL_LOADER][WL_TOTAL_SLOT]			= m_lTotalSlotNo;

	(*psmf)[WL_DATA][WL_LOADER][WL_SKIP_SLOT]			= m_lSkipSlotNo;			
	(*psmf)[WL_DATA][WL_LOADER][WL_CUR_MGZN]			= m_lCurrentMagNo;	
	(*psmf)[WL_DATA][WL_LOADER][WL_CUR_SLOT]			= m_lCurrentSlotNo;	
	(*psmf)[WL_DATA][WL_LOADER][WL_UNLOAD_OFFSET]		= m_lUnloadOffset;
	(*psmf)[WL_DATA][WL_LOADER][WL_OPEN_GRIPPER_IN_UNLOAD] = m_bOpenGripperInUnload;
	(*psmf)[WL_DATA][WL_LOADER][WL_BL_DISABLE_PRELOAD]	= m_bUseBLAsLoaderZDisablePreload;	
	

	(*psmf)[WL_DATA][WL_LOADER2][WL_OPEN_GRIPPER_IN_UNLOAD] = m_bOpenGripper2InUnload;


	for (INT i=0; i<WL_MAG_NO ; i++)
	{
		CString szStr;
		szStr.Format("%s%d", WL_MAG, i+1);
		if( m_stWaferMagazine[i].m_lNoOfSlots>WL_MAX_MAG_SLOT || m_stWaferMagazine[i].m_lNoOfSlots<1 )
			m_stWaferMagazine[i].m_lNoOfSlots = WL_MAX_MAG_SLOT;

		(*psmf)[WL_DATA][WL_LOADER][szStr][WL_TOP_SLOT_POS] = m_stWaferMagazine[i].m_lTopLevel_Z;
		(*psmf)[WL_DATA][WL_LOADER][szStr][WL_TOTAL_SLOT] = m_stWaferMagazine[i].m_lNoOfSlots;
		(*psmf)[WL_DATA][WL_LOADER][szStr][WL_SLOT_PITCH] = m_stWaferMagazine[i].m_dSlotPitch;

		//v4.56A11	//AutoLine
		CString szStr2;
		for (INT j=0; j<WL_MAX_MGZN_SLOT; j++)
		{
			szStr2.Format("Slot%d", j+1);
			(*psmf)[WL_DATA][WL_LOADER][szStr][szStr2][WL_SLOT_WIP_COUNTER]	= m_stWaferMagazine[i].m_lSlotWIPCounter[j];
			(*psmf)[WL_DATA][WL_LOADER][szStr][szStr2][WL_SLOT_USAGE]	= m_stWaferMagazine[i].m_lSlotUsage[j];
			(*psmf)[WL_DATA][WL_LOADER][szStr][szStr2][WL_SLOT_BARCODE]	= m_stWaferMagazine[i].m_SlotBCName[j];
		}
	}

	for (INT i=0; i<WL_MAG_NO ; i++)
	{
		CString szStr;
		szStr.Format("%s%d", WL_MAG, i+1);
		if( m_stWaferMagazine2[i].m_lNoOfSlots>WL_MAX_MAG_SLOT || m_stWaferMagazine2[i].m_lNoOfSlots<1 )
			m_stWaferMagazine2[i].m_lNoOfSlots = WL_MAX_MAG_SLOT;

		(*psmf)[WL_DATA][WL_LOADER2][szStr][WL_TOP_SLOT_POS] = m_stWaferMagazine2[i].m_lTopLevel_Z;
		(*psmf)[WL_DATA][WL_LOADER2][szStr][WL_TOTAL_SLOT] = m_stWaferMagazine2[i].m_lNoOfSlots;
		(*psmf)[WL_DATA][WL_LOADER2][szStr][WL_SLOT_PITCH] = m_stWaferMagazine2[i].m_dSlotPitch;

		//v4.56A11	//AutoLine
		CString szStr2;
		for (INT j=0; j<WL_MAX_MGZN_SLOT; j++)
		{
			szStr2.Format("Slot%d", j+1);
			(*psmf)[WL_DATA][WL_LOADER2][szStr][szStr2][WL_SLOT_USAGE]		= m_stWaferMagazine2[i].m_lSlotUsage[j];
			(*psmf)[WL_DATA][WL_LOADER2][szStr][szStr2][WL_SLOT_BARCODE]	= m_stWaferMagazine2[i].m_SlotBCName[j];
		}
	}

	(*psmf)[WL_DATA][WL_LOADER2][WL_FRAME_EXIST]		= m_bFrameExistOnExp2;		//v2.64
	(*psmf)[WL_DATA][WL_LOADER2][WL_MAGAZINE_FULL]		= m_bIsMagazine2Full;

	(*psmf)[WL_DATA][WL_LOADER2][WL_SKIP_SLOT]			= m_lSkipSlotNo2;			
	(*psmf)[WL_DATA][WL_LOADER2][WL_CUR_MGZN]			= m_lCurrentMagNo2;	
	(*psmf)[WL_DATA][WL_LOADER2][WL_CUR_SLOT]			= m_lCurrentSlotNo2;	
	(*psmf)[WL_DATA][WL_LOADER2][WL_UNLOAD_OFFSET]		= m_lUnloadOffset2;
	
	(*psmf)[WL_DATA][WL_EXPANDER][WL_HOTAIR_OP_TIME]	= m_lHotBlowOpenTime;		
	(*psmf)[WL_DATA][WL_EXPANDER][WL_HOTAIR_CL_TIME]	= m_lHotBlowCloseTime;	
	(*psmf)[WL_DATA][WL_EXPANDER][WL_MYLARCOOL_TIME]	= m_lMylarCoolTime;
	(*psmf)[WL_DATA][WL_EXPANDER][WL_OPEN_DRIVEIN]		= m_lOpenDriveInTime;
	(*psmf)[WL_DATA][WL_EXPANDER][WL_CLOSE_DRIVEIN]		= m_lCloseDriveInTime;
	(*psmf)[WL_DATA][WL_EXPANDER][WL_UNLOAD_DRIVEIN]	= m_lUnloadOpenExtraTime;	//v4.59A17

	(*psmf)[WL_DATA][WL_EXPANDER][WL_OPEN_DRIVEIN_OFFSETZ]	= m_lExpEncoderOpenOffsetZ;
	(*psmf)[WL_DATA][WL_EXPANDER][WL_CLOSE_DRIVEIN_OFFSETZ]	= m_lExpEncoderCloseOffsetZ;

	(*psmf)[WL_DATA][WL_EXPANDER2][WL_HOTAIR_OP_TIME]	= m_lHotBlowOpenTime2;		
	(*psmf)[WL_DATA][WL_EXPANDER2][WL_HOTAIR_CL_TIME]	= m_lHotBlowCloseTime2;	
	(*psmf)[WL_DATA][WL_EXPANDER2][WL_MYLARCOOL_TIME]	= m_lMylarCoolTime2;
	(*psmf)[WL_DATA][WL_EXPANDER2][WL_OPEN_DRIVEIN]		= m_lOpenDriveInTime2;
	(*psmf)[WL_DATA][WL_EXPANDER2][WL_CLOSE_DRIVEIN]		= m_lCloseDriveInTime2;

	(*psmf)[WL_DATA][WL_EXPANDER][WL_VACUUM_BF_TIME]		= m_lVacuumBefore;
	(*psmf)[WL_DATA][WL_EXPANDER][WL_VACUUM_AF_TIME]		= m_lVacuumAfter;
	//(*psmf)[WL_DATA][WL_EXPANDER][WL_UNLOAD_PHY_X]			= m_lUnloadPhyPosX;		
	//(*psmf)[WL_DATA][WL_EXPANDER][WL_UNLOAD_PHY_Y]			= m_lUnloadPhyPosY;		
	(*psmf)[WL_DATA][WL_EXPANDER][WL_EXPANDER_TYPE]			= GetExpType();
	(*psmf)[WL_DATA][WL_EXPANDER][WL_DCMOTOR_DAC]			= m_lExpDCMotorDacValue;
	(*psmf)[WL_DATA][WL_EXPANDER][WL_WT_ALN_COUNT]			= m_lWTAlignFrameCount;
	(*psmf)[WL_DATA][WL_EXPANDER][WL_EJR_CAP_CLEAN_LIMIT]	= m_lEjrCapCleanLimit;		//v3.28T2
	(*psmf)[WL_DATA][WL_EXPANDER][WL_EJR_CAP_CLEAN_COUNT]	= m_lEjrCapCleanCount;		//v3.28T2

	(*psmf)[WL_DATA][WL_EXPANDER2][WL_UNLOAD_PHY_X]		= m_lUnloadPhyPosX2;		
	(*psmf)[WL_DATA][WL_EXPANDER2][WL_UNLOAD_PHY_Y]		= m_lUnloadPhyPosY2;

	(*psmf)[WL_DATA][WL_ES_CONTOUR_DO_GT]				= m_bEnableContourGT;
	(*psmf)[WL_DATA][WL_ES_CONTOUR_PRELOAD_MAP]			= m_bContourPreloadMap;
	(*psmf)[WL_DATA][WL_ES_CONTOUR_EXTRA_MARGIN_X]		= m_dContourExtraMarginX;
	(*psmf)[WL_DATA][WL_ES_CONTOUR_EXTRA_MARGIN_Y]		= m_dContourExtraMarginY;
	(*psmf)[WL_DATA][WL_ES_USE_CONTOUR]					= m_bUseContour;
	(*psmf)[WL_DATA][WL_BARCODE][WL_USE_BARCODE]		= m_bUseBarcode;
	(*psmf)[WL_DATA][WL_BARCODE][WL_COMPARE_BARCODE]	= m_bCompareBarcode;
	(*psmf)[WL_DATA][WL_BARCODE][WL_USE_BARCODE_FILE]	= m_bUseBCInCassetteFile;
	(*psmf)[WL_DATA][WL_BARCODE][WL_NULL_BARCODE_CHECK]	= m_bBarcodeCheck;
	(*psmf)[WL_DATA][WL_BARCODE][WL_2D_BARCODE_CHECK]	= m_bEnable2DBarcodeCheck;
	(*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_COM_PORT]		= m_ucCommPortNo;			
	(*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_MODEL]			= GetBCModel();
	(*psmf)[WL_DATA][WL_BARCODE][WL_BARCODE_NAME]		= m_szBarcodeName;

	//v2.93T2
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	(*psmf)[WL_DATA][WL_BARCODE][WL_READ_ON_TABLE]		= m_bReadBarCodeOnTable;
	if (!m_bReadBarCodeOnTable && (m_lScanRange>1000))
	{
		m_lScanRange = 1000;
	}
	(*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_RANGE]			= m_lScanRange;	
	(*psmf)[WL_DATA][WL_BARCODE][WL_BC_CHECK_LENGTH]	= m_lBarcodeCheckLength;

	if (m_lBarcodeCheckLength == 0)
	{
		m_lBarcodeCheckLengthUpperBound = 0;
	}
	if( m_lBarcodeCheckLengthUpperBound>0 && m_lBarcodeCheckLengthUpperBound<m_lBarcodeCheckLength )
	{
		m_lBarcodeCheckLengthUpperBound = m_lBarcodeCheckLength;
	}

	(*psmf)[WL_DATA][WL_BARCODE][WL_BC_CHECK_LENGTH_UPPER_BOUND] = m_lBarcodeCheckLengthUpperBound;

	(*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_ROTATE_BACK]	= m_bRotateBackAfterScan;
	(*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_WFT_Y_RANGE]	= m_lWftYScanRange;
	(*psmf)[WL_DATA][WL_BARCODE][WL_SCAN_WFT_T_ANGLE]	= m_dWftTScanRange;

	if( IsMS90HalfSortMode() )
	{
		if( m_ulCornerSearchOption!=WL_CORNER_TL && m_ulCornerSearchOption!=WL_CORNER_TR )
			m_ulCornerSearchOption = WL_CORNER_TL;
	}
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_WT2_HOME_PHY_X]		= m_lWT2HomeDiePhyPosX;		
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_WT2_HOME_PHY_Y]		= m_lWT2HomeDiePhyPosY;		
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_HOME_SEARCH_ID]	= m_lPRSrchID;
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_HOME_PHY_X]		= m_lHomeDiePhyPosX;		
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_HOME_PHY_Y]		= m_lHomeDiePhyPosY;		
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_LHS_IDX_COUNT]	= m_lAngleLHSCount;		
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_RHS_IDX_COUNT]	= m_lAngleRHSCount;		
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_MATRIX_SIZE]	= m_lSprialSize;	
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_CORNER_SRCH_OPTION]	= m_ulCornerSearchOption;
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_PLLM_WAFER_ALIGN_OPTION]	= m_ucPLLMWaferAlignOption;	
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_SRCH_HOME_OPTION]	= m_bSearchHomeOption;
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_ALIGN_ANGLE_MODE]	= m_lAlignWaferAngleMode;
	(*psmf)[WL_DATA][WL_ALIGN_WAFER][WL_AUTO_ADAPT_WAFER] = m_bAutoAdpatWafer;

	(*psmf)[WL_DATA][WL_WAFER_LOT_INFO][WL_LOT_FILE_PATH]	= m_szLotInfoFilePath;

	// yealy: for saving the current slot for magazine "X"	
	(*psmf)[WL_DATA][WL_LOADER][WL_CURRENT_SLOT_OF_X]	= m_lCurrentMagazineX1SlotNo;
	(*psmf)[WL_DATA][WL_LOADER2][WL_CURRENT_SLOT_OF_X2]	= m_lCurrentMagazineX2SlotNo;
	(*psmf)[WL_DATA][WL_LOADER][WL_ES_MAGAZINE3_AS_X]	= m_bUseMagazine3AsX;

	if( m_bUseAMagazineAsG && m_bUseMagazine3AsX )
		m_lMagazineG_No	= 2;
	else
		m_lMagazineG_No	= 3;
	(*psmf)[WL_DATA][WL_LOADER][WL_ES_MAGAZINE_AS_G]	= m_bUseAMagazineAsG;
	(*psmf)[WL_DATA][WL_LOADER][WL_ES_G1_CUR_SLOTNO]	= m_lMagazineG1CurrentSlotNo;
	(*psmf)[WL_DATA][WL_LOADER][WL_ES_G2_CUR_SLOTNO]	= m_lMagazineG2CurrentSlotNo;

	(*psmf)[WL_DATA][WL_MISC_OPTION][WL_AUTO_SEQUENCE]		= m_ulSequenceMode;
	if (m_bDisableWLWithExp || (GetExpType() == WL_EXP_NONE))	//v4.39T10
	{
		m_ulSequenceMode = WL_MANUAL_MODE;
	}

	
	//Check Load/Save Data
    pUtl->UpdateWLConfig() ;

	// close config file
    pUtl->CloseWLConfig();

	
	//Update Expander Type into registry
	pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_TYPE, (int)GetExpType());
	pApp->WriteProfileInt(gszPROFILE_SETTING, gszWL_BC_READER_MODEL, (int)GetBCModel());

	//Update SRAM setting
	(*m_psmfSRam)["WaferTable"]["WT1LoadMgzn"]	= m_lCurrentMagNo;
	(*m_psmfSRam)["WaferTable"]["WT1LoadSlot"]	= m_lCurrentSlotNo;
	(*m_psmfSRam)["WaferTable"]["WT2LoadMgzn"]	= m_lCurrentMagNo2;
	(*m_psmfSRam)["WaferTable"]["WT2LoadSlot"]	= m_lCurrentSlotNo2;

	(*m_psmfSRam)["WaferTable"]["EjectorCapCleanLimit"] = m_lEjrCapCleanLimit;
 	if( IsWLManualMode() )
	{
		if(pApp->GetCustomerName() != "ChangeLight")
			SetWT2InUse(FALSE);
	}

   return TRUE;
}


BOOL CWaferLoader::GenerateConfigData()
{
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


	//Loader
	oFile.WriteString("\n[INPUT LOADER]\n");
	szLine.Format("Operation Mode,%d\n",				m_ulSequenceMode);
	oFile.WriteString(szLine);

	switch (GetExpType())
	{
	case WL_EXP_VACUUM:
		szLine = "Expander Type,VACCUM\n";
		break;
	case WL_EXP_GEAR_FF:
		szLine = "Expander Type,GEAR FF\n";
		break;
	case WL_EXP_GEAR_ROF:
		szLine = "Expander Type,GEAR ROF\n";
		break;
	case WL_EXP_GEAR_DIRECTRING:
		szLine = "Expander Type,DIRECTRING\n";
		break;
	case WL_EXP_GEAR_NUMOTION_DCMOTOR:
		szLine = "Expander Type,DC-MOTOR\n";
		break;
	case WL_EXP_CYLINDER:
		szLine = "Expander Type,CYLINDER\n";
		break;
	case WL_EXP_NONE:
	default:
		szLine = "Expander Type,NONE\n";
		break;
	}
	oFile.WriteString(szLine);


	//Wafer Gripper
	oFile.WriteString("\n[INPUT GRIPPER]\n");
	szLine.Format("Unload Position,%d\n",				m_lUnloadPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Load Position,%d\n",					m_lLoadPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Ready Position,%d\n",				m_lReadyPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Barcode Position,%d\n",				m_lBarcodePos_X);
	oFile.WriteString(szLine);
	szLine.Format("2D Barcode Position,%d\n",			m_l2DBarcodePos_X);
	oFile.WriteString(szLine);
	szLine.Format("BUffer Load Position,%d\n",			m_lBufferLoadPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Unload Offset,%d\n",					m_lStepOnUnloadPos);
	oFile.WriteString(szLine);
	szLine.Format("Load Offset,%d\n",					m_lStepOnLoadPos);
	oFile.WriteString(szLine);

	//Wafer Magazine
	oFile.WriteString("\n[INPUT MAGAZINE]\n");
	szLine.Format("Top Slot Level I,%d\n",				m_lTopSlotLevel_Z);
	oFile.WriteString(szLine);
	szLine.Format("Slot Pitch I (mm),%.1f\n",				m_dSlotPitch);
	oFile.WriteString(szLine);
	szLine.Format("Total Slot I,%d\n",					m_lTotalSlotNo);
	oFile.WriteString(szLine);
	szLine.Format("Unload Offset I,%d\n",					m_lUnloadOffset);
	oFile.WriteString(szLine);
	szLine.Format("WT Unload Position X,%d\n",			m_lUnloadPhyPosX);
	oFile.WriteString(szLine);
	szLine.Format("WT Unload Position Y,%d\n",			m_lUnloadPhyPosY);
	oFile.WriteString(szLine);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp ->GetCustomerName() == "EverVision")
	{
		oFile.WriteString("[Wafer Loader Process Setup]\n");

		szLine.Format("Enable Barcode Scanning ON/OFF,%d\n",			m_bUseBarcode);
		oFile.WriteString(szLine);

		CString szMode;
		if (m_ulSequenceMode == 0)
			szMode = "Auto";
		else if (m_ulSequenceMode == 1)
			szMode = "Manual";
		else
			szMode = "Semi-Auto";
		szLine.Format("Operation Mode Auto/Manual/Semi-Auto," + szMode + "\n");
		oFile.WriteString(szLine);
	}

	oFile.Close();
	szMsg.Format("WL: Generate Configuration Data done");
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	return TRUE;
}



//Common
BOOL CWaferLoader::IsMagazineExist(VOID)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
	if (m_bDisableWLWithExp)	//STD machine with Expander
	{
		return TRUE;
	}
#endif

	//m_bMagazineExist = m_piMagazineExist->IsHigh();
	if (m_bUseDualTablesOption)
	{
		m_bMagazineExist	= !CMS896AStn::MotionReadInputBit(WL_SI_MagExist);
	}
	else
	{
		m_bMagazineExist = CMS896AStn::MotionReadInputBit(WL_SI_MagExist);
	}
	
	return m_bMagazineExist;
}


BOOL CWaferLoader::IsFrameDetect(VOID)
{
	if (m_fHardware == FALSE)
	{
		AfxMessageBox("Fame Is Detected");
		return TRUE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
	
	if (m_bDisableWLWithExp)	//STD machine with Expander
	{
		return FALSE;
	}
#endif
	//m_bFrameDetect = m_piFrameDetect->IsHigh();
		
	m_bFrameDetect = CMS896AStn::MotionReadInputBit(WL_SI_FrameDetect);
	return m_bFrameDetect;
}

BOOL CWaferLoader::IsGripperMissingSteps()
{
	if( m_bWLXHasEncoder )
	{
		GetEncoderValue();
		LONG lCmdX = CMS896AStn::MotionGetCommandPosition(WL_AXIS_X, &m_stWLAxis_X);
		LONG lEncX = CMS896AStn::MotionGetEncoderPosition(WL_AXIS_X, 0.8, &m_stWLAxis_X);
		CString szMsg;
		szMsg.Format("gripper x cmd %ld, enc %ld", lCmdX, lEncX);
		SetAlarmLog(szMsg);
		if( labs(lCmdX-lEncX)>50 )
		{
			HmiMessage_Red_Yellow("Gripper missing steps!", "Gripper Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		//	SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CWaferLoader::IsFrameJam(VOID)
{
	SHORT siResult = HP_SUCCESS;
	LONG lReport = 0;

	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableWL)	//v3.61
	{
		return FALSE;
	}
	if (m_bDisableWLWithExp)	//STD machine with Expander
	{
		return FALSE;
	}
	
#ifdef NU_MOTION
	
	if (m_bUseDualTablesOption) // ES101
	{
		if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
		{
			m_bFrameJam	= !CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X, &m_stWLAxis_X);
			siResult = CMS896AStn::MotionGetLastWarning(WL_AXIS_X, &m_stWLAxis_X);
			lReport = CMS896AStn::MotionReportErrorStatus(WL_AXIS_X, &m_stWLAxis_X);
		}
		else
		{
			m_bFrameJam	= CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X, &m_stWLAxis_X);
			siResult = CMS896AStn::MotionGetLastWarning(WL_AXIS_X, &m_stWLAxis_X);
			lReport = CMS896AStn::MotionReportErrorStatus(WL_AXIS_X, &m_stWLAxis_X);
		}
	}
	else if (IsWprWithAF())	//ES100
	{
		m_bFrameJam = CMS896AStn::MotionReadInputBit("iwFrameJam");
	}
	else // Normal
	{
		if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
		{
			m_bFrameJam	= !CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X, &m_stWLAxis_X);
			siResult = CMS896AStn::MotionGetLastWarning(WL_AXIS_X, &m_stWLAxis_X);
			lReport = CMS896AStn::MotionReportErrorStatus(WL_AXIS_X, &m_stWLAxis_X);
		}
		else
		{
			m_bFrameJam	= CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X, &m_stWLAxis_X);
			siResult = CMS896AStn::MotionGetLastWarning(WL_AXIS_X, &m_stWLAxis_X);
			lReport = CMS896AStn::MotionReportErrorStatus(WL_AXIS_X, &m_stWLAxis_X);
		}
	}

#else
#endif

	return m_bFrameJam;
}


BOOL CWaferLoader::IsScopeDown(VOID)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return FALSE;
	}
#else
	if (IsWprWithAF())	//v4.04
	{
		return FALSE;
	}
#endif

	//m_bScopeLevel = !m_piScopeLevel->IsHigh();
	//m_bScopeLevel = !CMS896AStn::MotionReadInputBit(WL_SI_ScopeLevel);
	m_bScopeLevel = !CMS896AStn::MotionReadInputBit(WL_SI_ScopeLevel);

	return m_bScopeLevel;
}


BOOL CWaferLoader::IsWaferFrameDetect(VOID)
{
	return FALSE;
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return FALSE;
	}

	if (m_bDisableWLWithExp)	//STD machine with Expander
	{
		return FALSE;
	}
#endif
	//m_bWaferFrameDetect = m_piWaferFrameDetect->IsHigh();

	m_bWaferFrameDetect = CMS896AStn::MotionReadInputBit(WL_SI_WafFrameDetect);

	if (m_bUseDualTablesOption)
	{
		m_bWaferFrameDetect = !CMS896AStn::MotionReadInputBit(WL_SI_WafFrameDetect);
	}
	else
	{
		m_bWaferFrameDetect = CMS896AStn::MotionReadInputBit(WL_SI_WafFrameDetect);
	}

	return m_bWaferFrameDetect;
}


VOID CWaferLoader::SetHotAir(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return;
	}
#endif

	CMS896AStn::MotionSetOutputBit(WL_SO_HotAir, bSet);
	/*
	if ( bSet == TRUE )
	{
		// Turn on
		m_poHotAir->SetLow();
	}
	else
	{
		// Turn off
		m_poHotAir->SetHigh();
	}
	*/
}

VOID CWaferLoader::SetHotAir2(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return;
	}
#endif

	CMS896AStn::MotionSetOutputBit(WL_SO_HotAir2, bSet);
	/*
	if ( bSet == TRUE )
	{
		// Turn on
		m_poHotAir->SetLow();
	}
	else
	{
		// Turn off
		m_poHotAir->SetHigh();
	}
	*/
}


VOID CWaferLoader::SetGripperState(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return;
	}

	if (m_bDisableWLWithExp)
	{
		return;
	}
#endif
	CMS896AStn::MotionSetOutputBit(WL_SO_GripperState, bSet);

	/*
	if ( bSet == TRUE )
	{
		// Turn on
		m_poGripperState->SetLow();
	}
	else
	{
		// Turn off
		m_poGripperState->SetHigh();
	}
	*/
}


VOID CWaferLoader::SetGripperLevel(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return;
	}

	if (m_bDisableWLWithExp)
	{
		return;
	}
#endif

	if ( IsMS60() && (GetExpType() == WL_EXP_VACUUM) )	//v4.46T28
		return;

	CMS896AStn::MotionSetOutputBit(WL_SO_GripperLevel, bSet);
}

//end common


//For vacuum version
BOOL CWaferLoader::IsFramePosition(VOID)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
#endif
	//m_bFramePosition = m_piFramePosition->IsHigh();
	m_bFramePosition = CMS896AStn::MotionReadInputBit(WL_SI_FramePos);
	return m_bFramePosition;
	//return TRUE;
}


BOOL CWaferLoader::IsFrameProtection(VOID)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
#endif
	//m_bFrameProtection = m_piFrameProtection->IsHigh();
	m_bFrameProtection = CMS896AStn::MotionReadInputBit(WL_SI_FrameProtection);
	return m_bFrameProtection;
}


BOOL CWaferLoader::IsExpanderVacuum(VOID)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
#endif

	if (IsMS60())	//v4.46T28
		return TRUE;
	//m_bExpanderVacuum = m_piExpanderVacuum->IsHigh();
	m_bExpanderVacuum = CMS896AStn::MotionReadInputBit(WL_SI_ExpanderClose);
	return m_bExpanderVacuum;
}


BOOL CWaferLoader::IsExpanderLevel(VOID)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
#endif

	m_bExpanderLevel = CMS896AStn::MotionReadInputBit(WL_SI_PusherLock);
	//m_bExpanderLevel = m_piExpanderLevel->IsHigh();
	return m_bExpanderLevel;
}

BOOL CWaferLoader::IsUpperSlotFrameExist(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
	m_bFrameExistInUpperSlot = CMS896AStn::MotionReadInputBit(BL_SI_UpperSlotFrameExist);
	return m_bFrameExistInUpperSlot;
}

BOOL CWaferLoader::IsLowerSlotFrameExist(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
	m_bFrameExistInLowerSlot = CMS896AStn::MotionReadInputBit(BL_SI_LowerSlotFrameExist);
	return m_bFrameExistInLowerSlot;
}

BOOL CWaferLoader::IsBufferFrameProtectSafe(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
	m_bFrameExistOnBufferTable = CMS896AStn::MotionReadInputBit(BL_SI_BufferFrameProtect);
	return m_bFrameExistOnBufferTable;
}


VOID CWaferLoader::SetFrameVacuum(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return;
	}
#endif

	if (IsMS60())	//v4.46T28
		return;

	CMS896AStn::MotionSetOutputBit(WL_SO_FrameVac, bSet);
}


VOID CWaferLoader::SetFrameLevel(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return;
	}
#endif
	CMS896AStn::MotionSetOutputBit(WL_SO_FrameLevel, bSet);

	/*
	if ( bSet == TRUE )
	{
		// Turn on
		m_poFrameLevel->SetLow();
	}
	else
	{
		// Turn off
		m_poFrameLevel->SetHigh();
	}
	*/

	//Update Expander Status
    (*m_psmfSRam)["MS896A"]["Expander Status"] = bSet;
	m_bExpanderStatus = bSet;
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_STATUS, (int)m_bExpanderStatus);
}

VOID CWaferLoader::SetEjectorVacuum(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
	if (m_bDisableWT)
	{
		return;
	}

	CMS896AStn::MotionSetOutputBit("oEjectorVacuum", bSet);
}
VOID CWaferLoader::SetFrameAlign(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return;
	}
#endif

	if (IsMS60() && (GetExpType() == WL_EXP_VACUUM))		//v4.46T28
		return;

	CMS896AStn::MotionSetOutputBit(WL_SO_FrameAlign, bSet);
}
//end vacuum 

VOID CWaferLoader::SetALFrontGate(BOOL bSet)
{
	if (!m_fHardware || m_bDisableWL || !IsMS60())
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(WL_SO_AL_FRONT_GATE, bSet);
}

VOID CWaferLoader::SetALBackGate(BOOL bSet)
{
	if (!m_fHardware || m_bDisableWL || !IsMS60())
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(WL_SO_AL_BACK_GATE, bSet);
}
	
VOID CWaferLoader::SetALMgznClamp(BOOL bSet)
{
	if (!m_fHardware || m_bDisableWL || !IsMS60())
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(WL_SO_AL_MGZN_CLAMP, bSet);
}

BOOL CWaferLoader::IsFrontGateSensorOn()
{
	if (!m_fHardware || m_bDisableWL || !IsMS60())
	{
		return FALSE;
	}
	m_bFrontGateSensor = !CMS896AStn::MotionReadInputBit(WL_SI_FrontGateSnr);
	//if Cut the sensor, the status of sensor is 0
	//if leave the sensor, the status of sensor is 1
	return m_bFrontGateSensor;
}

BOOL CWaferLoader::IsBackGateSensorOn()
{
	if (!m_fHardware || m_bDisableWL || !IsMS60())
	{
		return FALSE;
	}
	m_bBackGateSensor = !CMS896AStn::MotionReadInputBit(WL_SI_BackGateSnr);
	//if Cut the sensor, the status of sensor is 0
	//if leave the sensor, the status of sensor is 1
	return m_bBackGateSensor;
}

//Gear version
BOOL CWaferLoader::IsExpanderOpen(VOID)
{
	if (m_fHardware == FALSE)
		return FALSE;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return FALSE;
	}
#endif
	//m_bExpanderOpen = m_piExpanderOpen->IsHigh();
	m_bExpanderOpen = CMS896AStn::MotionReadInputBit(WL_SI_ExpanderOpen);
	return m_bExpanderOpen;
}

BOOL CWaferLoader::IsExpander2Open(VOID)
{
	if (m_fHardware == FALSE)
		return FALSE;
#ifndef NU_MOTION
	if (m_bDisableWL)
	{
		return FALSE;
	}
#endif
	m_bExpander2Open = CMS896AStn::MotionReadInputBit(WL_SI_Expander2Open);
	return m_bExpander2Open;
}


BOOL CWaferLoader::IsExpanderClose(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
#endif
	//m_bExpanderClose = m_piExpanderClose->IsHigh();
	m_bExpanderClose = CMS896AStn::MotionReadInputBit(WL_SI_ExpanderClose);
	return m_bExpanderClose;
}

BOOL CWaferLoader::IsExpander2Close(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
#ifndef NU_MOTION
	if (m_bDisableWL)
	{
		return TRUE;
	}
#endif
	m_bExpander2Close = CMS896AStn::MotionReadInputBit(WL_SI_Expander2Close);
	return m_bExpander2Close;
}


BOOL CWaferLoader::IsFrameExist(VOID)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
#endif
	//m_bFrameExist = m_piFrameExist->IsHigh();
	m_bFrameExist = CMS896AStn::MotionReadInputBit(WL_SI_FrameExist);
	return m_bFrameExist;
}

BOOL CWaferLoader::IsFrameExist2(VOID)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
#ifndef NU_MOTION
	if (m_bDisableWL)
	{
		return TRUE;
	}
#endif
	m_bFrameExist2 = CMS896AStn::MotionReadInputBit(WL_SI_FrameExist2);
	return m_bFrameExist2;
}

BOOL CWaferLoader::IsExpanderLock(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
		return TRUE;
#endif
	//m_bExpanderLock = m_piExpanderLock->IsHigh();
	m_bExpanderLock = CMS896AStn::MotionReadInputBit(WL_SI_PusherLock);
	return m_bExpanderLock;
}

BOOL CWaferLoader::IsExpander2Lock(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
#ifndef NU_MOTION
	if (m_bDisableWL)
		return TRUE;
#endif
	m_bExpander2Lock = CMS896AStn::MotionReadInputBit(WL_SI_PusherLock2);
	return m_bExpander2Lock;
}

BOOL CWaferLoader::IsLeftLoaderCoverOpen(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (!m_bUseDualTablesOption)
		return FALSE;
	//TURE = Open; FALSE = Close
	BOOL bCoverSensor = CMS896AStn::MotionReadInputBit(WL_SI_LeftCover);
	return bCoverSensor;
}

BOOL CWaferLoader::IsRightLoaderCoverOpen(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (!m_bUseDualTablesOption)
		return FALSE;
	//TURE = Open; FALSE = Close
	BOOL bCoverSensor = CMS896AStn::MotionReadInputBit(WL_SI_RightCover);
	return bCoverSensor;
}

VOID CWaferLoader::SetDCMPower(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
		return;
#endif
	if ((GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)||(GetExpType() == WL_EXP_GEAR_DIRECTRING) )		
	{
		if (bSet)
		{
			LONG lDacValue = m_lExpDCMotorDirection * m_lExpDCMotorDacValue;

			CMS896AStn::MotionPowerOn(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
			Sleep(500);
			CMS896AStn::OpenDac(&m_stWLExpAxis_Z, lDacValue, 10, 1, WL_EXP_TIMEOUT * 20, 1);
		}
		else
		{
			CMS896AStn::MotionStop(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
			Sleep(500);
			CMS896AStn::MotionPowerOff(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
		}
	}
	else if (GetExpType() == WL_EXP_NONE)	//v4.39T10	//Knowles MS109
	{
	}
	else
	{
		CMS896AStn::MotionSetOutputBit(WL_SO_DCPower, bSet);
	}
}


VOID CWaferLoader::SetDCMDirecction(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
		return;
#endif
	if ((GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)||(GetExpType() == WL_EXP_GEAR_DIRECTRING))		
	{
		m_lExpDCMotorDirection = -1* m_lExpDCMotorDirection;
	}
	else
	{
		CMS896AStn::MotionSetOutputBit(WL_SO_DCMDirection, bSet);
	}
}


VOID CWaferLoader::SetExpanderLock(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
		return;
#endif
	//CMS896AStn::MotionSetOutputBit(WL_SO_FrameAlign, bSet);
	CMS896AStn::MotionSetOutputBit(WL_SO_FrameLevel, bSet);  //for wafer table with expander 2019.3.20
}

VOID CWaferLoader::SetExpander2Lock(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableWL)
		return;
#endif
	CMS896AStn::MotionSetOutputBit(WL_SO_FrameAlign2, bSet);
}


VOID CWaferLoader::SetPushUpTable(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableWL)	//v3.61
		return;
#endif
	CMS896AStn::MotionSetOutputBit(WL_SO_FrameAlign, bSet);
}


INT CWaferLoader::SetGripperPower(BOOL bSet)
{
	if (!m_fHardware)
		return TRUE;
	if (m_bDisableWL)	//v3.61
		return TRUE;
	if (m_bDisableWLWithExp)	//STD machine with Expander
	{
		return TRUE;
	}

	try
	{
		if ( bSet == TRUE )
		{
			//m_pStepper_X->PowerOn();		
			CMS896AStn::MotionPowerOn(WL_AXIS_X, &m_stWLAxis_X);

		}
		else
		{
			//m_pStepper_X->PowerOff();
			CMS896AStn::MotionPowerOff(WL_AXIS_X, &m_stWLAxis_X);
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

INT CWaferLoader::SetGripper2Power(BOOL bSet)
{
	if (!m_fHardware)
		return TRUE;
	if (m_bDisableWL)
		return TRUE;
	if (m_bDisableWLWithExp)	//STD machine with Expander
		return TRUE;
	if ( IsESDualWL()==FALSE )
		return FALSE;

	try
	{
		if ( bSet == TRUE )
		{
			CMS896AStn::MotionPowerOn(WL_AXIS_X2, &m_stWLAxis_X2);

		}
		else
		{
			CMS896AStn::MotionPowerOff(WL_AXIS_X2, &m_stWLAxis_X2);
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

INT CWaferLoader::SetLoaderZPower(BOOL bSet)
{
	if (!m_fHardware)
		return TRUE;
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
	if (m_bDisableWLWithExp)	//STD machine with Expander
	{
		return TRUE;
	}

	try
	{
		if ( bSet == TRUE )
		{
			//m_pStepper_Z->PowerOn();		
			CMS896AStn::MotionPowerOn(WL_AXIS_Z, &m_stWLAxis_Z);		//NuMotion
		}
		else
		{
			//m_pStepper_Z->PowerOff();		
			CMS896AStn::MotionPowerOff(WL_AXIS_Z, &m_stWLAxis_Z);		//NuMotion
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

INT CWaferLoader::SetLoaderZ2Power(BOOL bSet)
{
	if (!m_fHardware)
		return TRUE;
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
	if (m_bDisableWLWithExp)	//STD machine with Expander
	{
		return TRUE;
	}
	if ( IsESDualWL()==FALSE )
		return TRUE;

	try
	{
		if ( bSet == TRUE )
		{	
			CMS896AStn::MotionPowerOn(WL_AXIS_Z2, &m_stWLAxis_Z2);		//NuMotion
		}
		else
		{
			CMS896AStn::MotionPowerOff(WL_AXIS_Z2, &m_stWLAxis_Z2);		//NuMotion
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

INT CWaferLoader::MoveToBLMagazineSlot(LONG lMagazineNo, LONG lSlotNo, BOOL bUseUnloadOffset, BOOL bCheckSensors, BOOL bUseUpperSlot)
{
	if (bCheckSensors == TRUE)
	{
		if (IsWaferFrameDetect() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Frame is detected on track");
			SetErrorMessage("Frame is detected on track");

			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONTRACK);
			return Err_FrameExistOnTrack;
		}

		if ( IsMagazineSafeToMove() == FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is not in safe position");
			SetErrorMessage("Gripper is not in safe position");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
			return Err_GripperNotInSafePos;
		}
	}

	if (!m_bUseBLAsLoaderZ)
	{
		return FALSE;
	}

	if (bUseUpperSlot)	
		Z_MoveTo(m_lBufTableUSlot_Z, FALSE);
	else
		Z_MoveTo(m_lBufTableLSlot_Z, FALSE);

	typedef struct {
		LONG lMgzn;
		LONG lSlot;
		LONG lUseUnloadOffset;
	} BL_WL_MGZN_STRUCT;

	BL_WL_MGZN_STRUCT stData;
	stData.lMgzn = lMagazineNo;
	stData.lSlot = lSlotNo;
	if (bUseUnloadOffset)
		stData.lUseUnloadOffset = 1;
	else
		stData.lUseUnloadOffset = 0;

	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BL_WL_MGZN_STRUCT), &stData);

	INT nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "MovetoWLSlot", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

	Z_Sync();

	if (!bResult)
		return FALSE;
	return TRUE;
}

BOOL CWaferLoader::MoveToBLMagazineSlot_Sync()
{
	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;
	//stMsg.InitMessage(sizeof(BL_WL_MGZN_STRUCT), &stData);

	INT nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "MovetoWLSlot_Sync", stMsg);

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

	if (!bResult)
		return FALSE;
	return TRUE;
}

LONG CWaferLoader::GetSlotNoPitchCount(const LONG lSlotNo, const double dSlotPitch, const double dZRes)
{
	LONG lPitch = _round(((lSlotNo - 1) * dSlotPitch) / dZRes);
	
	if (IsMSAutoLineMode())
	{
		lPitch = -lPitch;
	}

	return lPitch;
}

INT CWaferLoader::MoveToMagazineSlot(LONG lMagazineNo, LONG lSlotNo, LONG lOffset, BOOL bWait, BOOL bCheckSensors)
{
	LONG lPitch = 0;		

	if (bCheckSensors == TRUE)
	{
		if (IsWaferFrameDetect() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Frame is detected on track");
			SetErrorMessage("Frame is detected on track");

			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONTRACK);
			return Err_FrameExistOnTrack;
		}

		if ( IsMagazineSafeToMove() == FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is not in safe position");
			SetErrorMessage("Gripper is not in safe position");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
			return Err_GripperNotInSafePos;
		}
	}

	INT nMoveState = gnOK;
	BOOL bCheckMgzn = TRUE;
	LONG lMaxMagNo = GetWL1MaxMgznNo();
	if( m_bUseMagazine3AsX && m_szWT1YieldState == "N" && m_lMagazineX_No==lMagazineNo )
	{
		bCheckMgzn = FALSE;
	}
	if( m_bUseAMagazineAsG && m_szWT1YieldState == "Y" && m_lMagazineG_No==lMagazineNo )
	{
		bCheckMgzn = FALSE;
	}
	if( bCheckMgzn )
	{
		if (lMagazineNo > lMaxMagNo || lMagazineNo < 0)
		{
			CString szMsg;
			szMsg.Format("To magazine %d over limit %d", lMagazineNo, lMaxMagNo);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			return FALSE;
		}
	}
	else
	{
		lMaxMagNo = lMagazineNo;
	}

	if (m_bUseBLAsLoaderZ)		//v4.31T11
	{
		if (lOffset	== 0)	
			MoveToBLMagazineSlot(lMagazineNo, lSlotNo);
		else
			MoveToBLMagazineSlot(lMagazineNo, lSlotNo, TRUE);
	}
	else if (m_bUseDualTablesOption)
	{
		LONG lTopSlotLevel_Z; 
		DOUBLE dSlotPitch;

		lMagazineNo = lMagazineNo-1;
		lMagazineNo = max(lMagazineNo, 0);				//Klocwork	//v4.24T11
		lMagazineNo = min(lMagazineNo, lMaxMagNo-1);	//Klocwork	//v4.24T11

		lTopSlotLevel_Z =	m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z;

		dSlotPitch	= m_stWaferMagazine[lMagazineNo].m_dSlotPitch;
		lPitch = GetSlotNoPitchCount(lSlotNo, dSlotPitch, m_dZRes);

		if( Z_IsPowerOn()==FALSE )
		{
			SetErrorMessage("Wafer Loader Z power off");
			HmiMessage_Red_Back("Wafer Loader Z power is off\nPlease check!", "Wafer Loader");
			return FALSE;
		}

		if( IsLeftLoaderCoverOpen() )	//	|| (IsESDualWL() IsRightLoaderCoverOpen() )
		{
			HmiMessage_Red_Back("Wafer Loader left cover open!", "Wafer Loader");
			return FALSE;
		}

		//v2.93T2
		if (!bWait)
		{
			nMoveState = Z_MoveTo(lTopSlotLevel_Z + lPitch + lOffset, SFM_NOWAIT);	
		}
		else
		{
			nMoveState = Z_MoveTo(lTopSlotLevel_Z + lPitch + lOffset);
		}	
	}
	else
	{
		lPitch = GetSlotNoPitchCount(lSlotNo, m_dSlotPitch, m_dZRes);
		if( Z_IsPowerOn()==FALSE )
		{
			SetErrorMessage("Wafer Loader Z power off");
			HmiMessage_Red_Back("Wafer Loader Z power is off\nPlease check!", "Wafer Loader");
			return FALSE;
		}
		//v2.93T2
		if (!bWait)
		{
			nMoveState = Z_MoveTo(m_lTopSlotLevel_Z + lPitch + lOffset, SFM_NOWAIT);	
		}
		else
		{
			nMoveState = Z_MoveTo(m_lTopSlotLevel_Z + lPitch + lOffset);
		}
	}

	if (nMoveState != gnOK) 
	{
		HmiMessage_Red_Back("Wafer Loader Z Move Error", "Wafer Loader");
	}

	return (nMoveState == gnOK );
}



VOID CWaferLoader::SetWaferTableJoystick(BOOL bState, BOOL bWT2)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(BOOL), &bState);

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
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetMouseControlCmd", stMsg);
	}
	else
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetJoystickCmd", stMsg);
	}

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

VOID CWaferLoader::SetWaferTableJoystickSpeed(LONG lLevel)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(LONG), &lLevel);

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetJoystickSpeedCmd", stMsg);

	// Get the reply
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID,stMsg);
}



VOID CWaferLoader::SetJoystickLimit(BOOL bMax, BOOL bWT2)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(BOOL), &bMax);

	if( IsESDualWT() && bWT2 )
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetJoystickLimitCmd2", stMsg);
	}
	else
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetJoystickLimitCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

//V2.83T4 (With Syn/Without Syn Choice)
BOOL CWaferLoader::MoveWaferTableNoCheck(LONG lXAxis, LONG lYAxis, BOOL bToUnload, INT nMode, BOOL bWT2)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} RELPOS;
 	RELPOS stPos;

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lUnload;
	} RELPOS2;
 	RELPOS2 stPos2;

	stPos.lX	= lXAxis;
	stPos.lY	= lYAxis;
	stPos2.lX	= lXAxis;
	stPos2.lY	= lYAxis;

	if (bToUnload)				//v4.20
		stPos2.lUnload = 1;		//to UNLOAD
	else
		stPos2.lUnload = 0;		//to LOAD
	
	if (CMS896AApp::m_bMS100Plus9InchOption)		//v4.16T3	//MS100 9Inch
	{
		stMsg.InitMessage(sizeof(RELPOS2), &stPos2);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_MoveToUnloadCmd",	stMsg);
	}
	else if (m_bUseDualTablesOption)	//IsES101()		//v4.24T9
	{
		if (IsES201() && bWT2)		//v4.34T10
		{
			return FALSE;
		}

		stMsg.InitMessage(sizeof(RELPOS2), &stPos2);
		if (bWT2)
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY2_MoveToUnloadCmd",	stMsg);
		else
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_MoveToUnloadCmd",	stMsg);
	}
	else
	{
		stMsg.InitMessage(sizeof(RELPOS), &stPos);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_MoveToCmd",			stMsg);
	}

	if (nMode == SFM_WAIT)
	{
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
		//v3.60		//Check Table status	//Production usage
		stMsg.GetMsg(sizeof(BOOL), &bResult);
	}

	return bResult;
}

BOOL CWaferLoader::MoveWaferTable(LONG lXAxis, LONG lYAxis)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} RELPOS;
 	RELPOS stPos;


	stPos.lX = lXAxis;
	stPos.lY = lYAxis;

	stMsg.InitMessage(sizeof(RELPOS), &stPos);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_SafeMoveToCmd", stMsg);

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

	stMsg.GetMsg(sizeof(BOOL), &bResult);
	return bResult;
}

BOOL CWaferLoader::MoveWaferTableLoadUnload(LONG lXAxis, LONG lYAxis, BOOL bUnload)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lUnload;
	} RELPOS2;
 	RELPOS2 stPos;


	stPos.lX = lXAxis;
	stPos.lY = lYAxis;
	if (bUnload)
		stPos.lUnload = 1;
	else
		stPos.lUnload = 0;

	stMsg.InitMessage(sizeof(RELPOS2), &stPos);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_SafeMoveToUnloadCmd", stMsg);

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

	stMsg.GetMsg(sizeof(BOOL), &bResult);
	return bResult;
}

VOID CWaferLoader::GetWaferTableEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis)
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
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetEncoderCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
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

VOID CWaferLoader::GetWaferTable2Encoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis)
{
	IPC_CServiceMessage stMsg;

	typedef struct {
		LONG lX;
		LONG lY;
		LONG lT;
		DOUBLE dT;
	} WT_XYT_STRUCT2;

 	WT_XYT_STRUCT2 stEnc;
	int nConvID = 0;

	if (IsES201())		//v4.34T10
	{
		return;
	}

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetEncoderCmd2", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(WT_XYT_STRUCT2), &stEnc);
	
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

VOID CWaferLoader::MoveWaferThetaTo(LONG lStep, BOOL bWT2)
{
	IPC_CServiceMessage stMsg;

	int nConvID = 0;
	LONG lStepT = lStep;

#ifdef NU_MOTION

	LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);
	//v4.16T3
	//Assume it is moveing to UNLOAD ENGAGE position T
	if ( CMS896AApp::m_bMS100Plus9InchOption  && (lStep == 0) && (lEnableEngageOffset!=0))
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		int nPosnT = (int) pApp->GetProfileInt(_T("Settings"), _T("WTEngagePositionT"), 0);
		lStepT = nPosnT;

		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	
		if (lStepT > lMaxPos)
			lStepT = 0;
		if (lStepT < lMinPos)
			lStepT = 0;
	}

	if (GetExpType() == WL_EXP_CYLINDER)
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		int nPosnT = -6413;
		lStepT = nPosnT;
	}
#endif 

	stMsg.InitMessage(sizeof(LONG), &lStepT);

	// Get the reply for the encoder value
	if( IsESDualWT() )
	{
		if( bWT2 )
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T2_MoveToCmd", stMsg);
		else
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T1_MoveToCmd", stMsg);
	}
	else
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T_MoveToCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

BOOL CWaferLoader::HomeWaferTheta(BOOL bWT2)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;


	//v4.46T13	//OsramPenang
	if (bWT2)
	{
		if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER && GetExpType() != WL_EXP_NONE) && 
			IsExpander2Lock() )
		{
			SetExpander2Lock(WL_OFF);
			Sleep(500);
			if (IsExpander2Lock())
			{
				SetErrorMessage("Expander 2 unlock fail at HomeWaferTheta");
				SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
				return FALSE;
			}
		}
	}
	else
	{
		if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER && GetExpType() != WL_EXP_NONE) && 
			IsExpanderLock() )
		{
			SetExpanderLock(WL_OFF);
			Sleep(500);
			if (IsExpanderLock())
			{
				SetErrorMessage("Expander unlock fail at HomeWaferTheta");
				SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
				return FALSE;
			}
		}
	}


	// Get the reply for the encoder value
	if ( (CMS896AApp::m_bMS100Plus9InchOption == FALSE) )	// || ((CMS896AApp::m_bMS100Plus9InchOption == TRUE) && (lEnableEngageOffset==0)))	//v4.20
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T_HomeCmd", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

	if (GetExpType() == WL_EXP_CYLINDER)
	{
		CMSNmAxisInfo stWTAxis_T;
		InitAxisData(stWTAxis_T);
		stWTAxis_T.m_szName				= "WaferTAxis";
		stWTAxis_T.m_szTag				= MS896A_CFG_CH_WAFTABLE_T;
		stWTAxis_T.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(stWTAxis_T);

		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		int nPosnT = -6413;
		
		CString szTemp;
		//szTemp.Format("WT Expander T ENGAGE T retrieved = %d (%ld, %ld)", nPosnT, lMinPos, lMaxPos);
		//DisplayMessage(szTemp);

		CMS896AStn::MotionMoveTo("WaferTAxis", nPosnT, SFM_WAIT, &stWTAxis_T);
		Sleep(500);
	}

#ifdef NU_MOTION

	if (CMS896AApp::m_bMS100Plus9InchOption)	// && lEnableEngageOffset!=0)	//v4.16		//MS100Plus 9Inch machine
	{
		CMSNmAxisInfo stWTAxis_T;
		InitAxisData(stWTAxis_T);
		stWTAxis_T.m_szName				= "WaferTAxis";
		stWTAxis_T.m_szTag				= MS896A_CFG_CH_WAFTABLE_T;
		stWTAxis_T.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(stWTAxis_T);

		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		int nPosnT = (int) pApp->GetProfileInt(_T("Settings"), _T("WTEngagePositionT"), 0);
		
		CString szTemp;
		//szTemp.Format("WT Expander T ENGAGE T retrieved = %d (%ld, %ld)", nPosnT, lMinPos, lMaxPos);
		//DisplayMessage(szTemp);

		if (nPosnT > lMaxPos)
			nPosnT = 0;
		if (nPosnT < lMinPos)
			nPosnT = 0;

		CMS896AStn::MotionMoveTo("WaferTAxis", nPosnT, SFM_WAIT, &stWTAxis_T);
		Sleep(500);
	}
#endif
	return TRUE;
}


BOOL CWaferLoader::BondArmMoveToBlow(VOID)
{
	IPC_CServiceMessage stMsg;
	BOOL bReturn = TRUE;
	int nConvID = 0;

	//No BHT in MegaDa
	return bReturn;

	if (m_bDisableBH)
		return bReturn;

	if (m_bUseDualTablesOption == TRUE)
	{
		return TRUE;
	}

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveToBlow", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
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

//V2.83T4 (With Sync/ Without Sync Choice)
VOID CWaferLoader::BondArmMoveToPrePick(INT nMode)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveToPrePick", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 36000) == TRUE )
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

BOOL CWaferLoader::CheckRepeatMap()
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bStatus = TRUE;


	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "CheckRepeatMap", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 36000) == TRUE )
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

	CString szMsg;
	szMsg.Format("Expander CLOSE: checking repeat map done = %d", bStatus);
	SetErrorMessage(szMsg);

	return bStatus;
}

VOID CWaferLoader::ResetGripperHitLimit(VOID)
{
	/*
	m_pStepper_X->DisableProtection(HP_LIMIT_SENSOR);
	m_pStepper_X->GetLastWarning();	
	m_pStepper_X->ReportErrorStatus();
	m_pStepper_X->ClearError(HP_LIMIT_SENSOR);
	*/
#ifndef NU_MOTION	//v3.86
	CMS896AStn::MotionEnableProtection(WL_AXIS_X,HP_LIMIT_SENSOR, TRUE, FALSE, &m_stWLAxis_X);
#endif
	CMS896AStn::MotionGetLastWarning(WL_AXIS_X, &m_stWLAxis_X);
	CMS896AStn::MotionReportErrorStatus(WL_AXIS_X, &m_stWLAxis_X);
#ifndef NU_MOTION	//v3.86
	CMS896AStn::MotionClearError(WL_AXIS_X, HP_LIMIT_SENSOR, &m_stWLAxis_X);
#endif
}

VOID CWaferLoader::ResetGripper2HitLimit(VOID)
{
	if ( IsESDualWL()==FALSE )
		return ;
	CMS896AStn::MotionGetLastWarning(WL_AXIS_X2,	&m_stWLAxis_X2);
	CMS896AStn::MotionReportErrorStatus(WL_AXIS_X2, &m_stWLAxis_X2);
}



INT CWaferLoader::ClearMapFile()
{
	IPC_CServiceMessage stSetMsg;
	int nConvID = 0;
	// Clear previous map file fist in case previous map file loaded
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ClearWaferMap", stSetMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 3600000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stSetMsg);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	return nConvID;
}


INT CWaferLoader::LoadMapFileWithoutSyn(CString szData)
{
	IPC_CServiceMessage stSetMsg;
	IPC_CServiceMessage stRetMsg;
	BOOL	bResult = FALSE;	
	int		nConvID = 0;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	CMSLogFileUtility::Instance()->WL_LogStatus("Load MapFile WithoutSyn -> " + szData);
	(*m_psmfSRam)["MS896A"]["PKG RunTime Loading done"]	= FALSE;
	if ((szData.IsEmpty() == TRUE) && pUtl->GetPrescanDummyMap()==FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("No barcode found");
		SetErrorMessage("No barcode found");
		SetAlert_Red_Yellow(IDS_WL_MAP_NO_NAME);
		return nConvID;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="HPO" && pApp->IsRunTimeLoadPKGFile() )
	{
		CString szPkgName = szData;
		int nIndex = szData.ReverseFind('_');
		if( nIndex !=-1 )
		{
			szPkgName = szData.Left(nIndex);
		}
		BOOL bLoadPkg = FALSE;
		if( m_szLastLoadPkgName.IsEmpty() )
		{
			bLoadPkg = TRUE;
		}
		else
		{
			if( m_szLastLoadPkgName.CompareNoCase(szPkgName)!=0 )
			{
				bLoadPkg = TRUE;
			}
		}
		CMSLogFileUtility::Instance()->WL_LogStatus(m_szLastLoadPkgName + " is old and new is " + szPkgName);
		if( bLoadPkg )
		{
			pApp->RestorePrDataFromDevice(szPkgName, TRUE, TRUE);
		}
		m_szLastLoadPkgName = szPkgName;
	}

	//Convert CString into char array
	LPTSTR lpsz = new TCHAR[255];

	strcpy_s(lpsz, 255, (LPCTSTR)szData);
	CMSLogFileUtility::Instance()->WL_LogStatus("set wafer table map file name " + szData);
	//Set filename into wafer table station
	stSetMsg.InitMessage(sizeof(TCHAR)*255, lpsz);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetMapFileName", stSetMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 3600000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stSetMsg);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	delete[] lpsz;

//==================================================================
//   2018.5.31
//==================================================================
	BOOL bCheckRepeatMap = FALSE;
	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	if (pWaferTable)
	{
		bCheckRepeatMap = pWaferTable->IsCheckRepeatMap();
	}

	if (!IsBurnIn() && (pApp->GetCustomerName() == "ChangeLight(XM)"))
	{
		//Check repeat wafer map
		CString szOBarcode = szData;
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
			CMSLogFileUtility::Instance()->WL_LogStatus("Manual load frame, same frame and not finished, don't load map.");
			if( HmiMessage_Red_Yellow("Map is repeated!\nContinue Load Map?", "Wafer Load Map", glHMI_MBX_YESNO)==glHMI_YES )
			{
				SetErrorMessage("Map is repeated!  Please check");		//v2.78T2
				SetStatusMessage("Map is repeated!  Please check");
			}
			else
			{
				SetErrorMessage("Map is repeated!  Please check");		//v2.78T2
				SetStatusMessage("Map is repeated!  Please check");
				m_WaferMapWrapper.InitMap();	
				CMS896AApp::m_bMapLoadingAbort = TRUE;
				//bMapLoadAbort = TRUE;		//v4.40T10
				return 0;//Duplicated BC found in current lot!
			}
		}

		if (pSmfFile.Open(MSD_WAFER_BARCODE_FILE, FALSE, TRUE) == 1) 
		{
			(pSmfFile)[szOBarcode]["IsUsed"] = TRUE;
			pSmfFile.Update();
			pSmfFile.Close();
		}
	}
//=============================================================================

	BOOL bIsWaferEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferEnd"];
	if( (bIsWaferEnd==FALSE && IsPrescanEnded() && pApp->GetCustomerName()=="OSRAM") && IsES101()==FALSE )
	{
		CString szMapFile = GetMapFileName();
		szMapFile = szMapFile.MakeLower();
		CString szBarcode = szData.MakeLower();
		if( szMapFile.Find(szBarcode)>=0 )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Manual load frame, same frame and not finished, don't load map.");
			return 0;
		}
	}

	//Set MapLoading Flag
	CMS896AApp::m_bMapLoadingFinish = FALSE;
	CMS896AApp::m_bMapLoadingAbort	= FALSE;		//anichia005

	//Load map file
	CMSLogFileUtility::Instance()->WL_LogStatus("Load map start");
	CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(4);	//WafInfoAcq	//v4.40T5	//v4.40T11

	BOOL	bOffline = FALSE;
	stRetMsg.InitMessage(sizeof(BOOL), &bOffline);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "LoadMapWithBarCode", stRetMsg);
	
	return nConvID;
}

BOOL CWaferLoader::SyncLoadMapFile(INT &nConvID)
{
	IPC_CServiceMessage stRetMsg;
	BOOL bResult = FALSE;

	CString szMsg;
	szMsg.Format("Sync Load Map File, ConvID %d", nConvID);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	while(nConvID!=0)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stRetMsg);
			stRetMsg.GetMsg(sizeof(BOOL), &bResult);
			if (bResult==FALSE)
			{
				nConvID = 0;
				return FALSE;
			}
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	nConvID = 0;
	if( WaitMapValid()==FALSE )
	{
		return FALSE;
	}

	// Checking Load Map Reply
	if (WaferMapHeaderChecking() != TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("wafer map header checking fail");
		CString szMsg;
		CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q,szMsg);
		return FALSE;
	}

	if (CheckLoadCurrentMapStatus() != TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("check cur load map status fail");
		return FALSE;
	}

	// Auto Load Rank File
	if (AutoLoadRankIDFile() != TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("auto load rank id pk");
		return FALSE;
	}

	if( m_bUseBarcode )
	{
		if (CheckBatchIdFile(m_szLoadAlignBarcodeName) == FALSE)
		{
			SetErrorMessage("Check Batch Id File Failed");
			return FALSE;
		}
	}

/*
	//After load map and create wafer end file
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//2018.9.6 After load map and create wafer end file
		if (WaferEndFileGeneratingCheckingCmd(TRUE) == FALSE)
		{
			SetErrorMessage("Wafer end file generating check failed");
			CMSLogFileUtility::Instance()->WL_LogStatus("Wafer end file generating check failed");
			//return Err_WaferEndGenFail;
			return FALSE;
		}
	}
*/
	return TRUE;
}

BOOL CWaferLoader::WaitMapValid()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDummyMap() )
	{
		if( IsMS90HalfSortDummyScanAlign() )
		{
		}
		else
		{
			return TRUE;
		}
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("To check map loaded on wafer table side");
	INT nMaxTimeout = 6000;

	INT nCount = 0;
	while (1)
	{
		Sleep(100);
		//v2.93T2
		nCount++;
		if ( CMS896AApp::m_bMapLoadingFinish )
		{
			if (m_WaferMapWrapper.IsMapValid() == TRUE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("map loading finish");
				break;
			}
			else if (nCount > nMaxTimeout)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("WL Map is not valid");
				SetErrorMessage("WL Map is not valid");
				SetAlert_Red_Yellow(IDS_WL_MAP_NOT_VALID);
				return FALSE;
			}
		}

		if (CMS896AApp::m_bMapLoadingAbort)	//v4.40T10	//Nichia
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Load Map is aborted");
			SetErrorMessage("Load Map is aborted");
			HmiMessage_Red_Yellow("Load map is aborted!");
			CMS896AApp::m_bMapLoadingAbort = FALSE;
			return FALSE;
		}
		if (nCount > nMaxTimeout)			// 10-sec
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Load Map timeout");
			SetErrorMessage("Load Map timeout");

			HmiMessage_Red_Yellow("Load map timeout!");
			CMS896AApp::m_bMapLoadingAbort = TRUE;		//v4.51A11
			return FALSE;
		}
		else if (m_bStop && (nCount>10000))		//if AUTOBOND stop		//pllm
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Load Map timeout after AUTOBOND stop");
			SetErrorMessage("Load Map timeout after AUTOBOND stop");

			HmiMessage_Red_Yellow("Load map timeout after AUTOBOND mode stop!");
			return FALSE;
		}
	}

	return TRUE;
}

VOID CWaferLoader::FlushMessage()
{
	if (m_comServer.ScanRequest(10))
		m_comServer.ReadRequest();
}

VOID CWaferLoader::UpdateStationData()
{
	//AfxMessageBox("CWaferLoader", MB_SYSTEMMODAL);
}


BOOL CWaferLoader::SaveBarcodeInCassette(CString szFilename)
{
	CStdioFile cfFile;

	if (cfFile.Open(szFilename, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	DeleteFile(MSD_WL_BC_IN_CASSETTE_FILE);
	DeleteFile(MSD_WL_BC_IN_CASSETTE_BKF);

	BOOL bHeaderFound = FALSE;
	CStringMapFile*	pBarcodeInCassette;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	pUtl->LoadBarcodeInCassetteFile();
	pBarcodeInCassette = pUtl->GetBarcodeInCassetteFile();

	if (pBarcodeInCassette == NULL)
		return FALSE;
	
	CString szStr;
	CString szSlotNo, szBarcode;
	LONG lTotalNoOfSlot = 0;
	INT nCol;

	// Read the header
	cfFile.ReadString(szStr);

	if (szStr != WL_BC_FILE_MAG_CONFIG)
	{
		cfFile.Close();
		return FALSE;
	}

	while (cfFile.ReadString(szStr))
	{
		if (szStr == WL_BC_FILE_MAG_BEGIN)
		{
			bHeaderFound = TRUE;
			break;
		}
	}
	
	if (bHeaderFound == FALSE)
	{
		return FALSE;
	}

	while(cfFile.ReadString(szStr))
	{
		nCol = szStr.Find(',');
		if (nCol == -1)
		{
			cfFile.Close();
			//pUtl->CloseBarcodeCassetteFile();
			pUtl->UpdateBarcodeInCassetteFile();
			pUtl->CloseBarcodeCassetteFile();

			DeleteFile(MSD_WL_BC_IN_CASSETTE_FILE);
			DeleteFile(MSD_WL_BC_IN_CASSETTE_BKF);
			m_szBCInCassetteFilename = "";

			return FALSE;
		}

		szSlotNo = szStr.Left(nCol);
		szBarcode = szStr.Mid(nCol + 1);
		lTotalNoOfSlot = lTotalNoOfSlot + 1;

		//BarcodeInCassette[WL_BC_FILE_MAGAZINE][lTotalNoOfSlot][WL_BC_FILE_SLOT_NO] = (LONG) atoi(szSlotNo);
		//BarcodeInCassette[WL_BC_FILE_MAGAZINE][lTotalNoOfSlot][WL_BC_FILE_BARCODE] = szBarcode;

		(*pBarcodeInCassette)[WL_BC_FILE_MAGAZINE][lTotalNoOfSlot][WL_BC_FILE_SLOT_NO] = (LONG) atoi(szSlotNo);
		(*pBarcodeInCassette)[WL_BC_FILE_MAGAZINE][lTotalNoOfSlot][WL_BC_FILE_BARCODE] = szBarcode;
	}

	(*pBarcodeInCassette)[WL_BC_FILE_MAGAZINE][WL_BC_FILE_TOTAL_SLOT] = lTotalNoOfSlot;
	//BarcodeInCassette[WL_BC_FILE_MAGAZINE][WL_BC_FILE_TOTAL_SLOT] = lTotalNoOfSlot;
	(*pBarcodeInCassette)[WL_BC_FILE_MAGAZINE][WL_CASS_FILENAME] = m_szBCInCassetteFilename;

	cfFile.Close();

	//BarcodeInCassette.Update();
	//BarcodeInCassette.Close();

	pUtl->UpdateBarcodeInCassetteFile();
	pUtl->CloseBarcodeCassetteFile();

	ShowBarcodeInCassette();

	return TRUE;
}

BOOL CWaferLoader::GetBarcodeInCassetteFilename(CString& szFilename)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile*	pBarcodeInCassette;
	pUtl->LoadBarcodeInCassetteFile();
	pBarcodeInCassette = pUtl->GetBarcodeInCassetteFile();
	
	if (pBarcodeInCassette == NULL)
		return FALSE;

	szFilename = (*pBarcodeInCassette)[WL_BC_FILE_MAGAZINE][WL_CASS_FILENAME];

	pUtl->CloseBarcodeCassetteFile();

	return FALSE;

}

BOOL CWaferLoader::GetBarcodeInCassette(CString& szBarcode, LONG lSlotNo)
{	
	szBarcode.Empty();

	if (m_szBCInCassetteFilename.IsEmpty() == TRUE)
	{
		return FALSE;
	}

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile*	pBarcodeInCassette;
	pUtl->LoadBarcodeInCassetteFile();
	pBarcodeInCassette = pUtl->GetBarcodeInCassetteFile();
	
	if (pBarcodeInCassette == NULL)
		return FALSE;

	LONG lSlotTemp;
	CString szBarcodeTemp;

	LONG lTotalSlotNo = (*pBarcodeInCassette)[WL_BC_FILE_MAGAZINE][WL_BC_FILE_TOTAL_SLOT];
	//LONG lTotalSlotNo = BarcodeInCassette[WL_BC_FILE_MAGAZINE][WL_BC_FILE_TOTAL_SLOT];
	
	for (INT i=1;  i<= (INT)lTotalSlotNo; i++)
	{
		lSlotTemp = (*pBarcodeInCassette)[WL_BC_FILE_MAGAZINE][i][WL_BC_FILE_SLOT_NO];

		//lSlotTemp = BarcodeInCassette[WL_BC_FILE_MAGAZINE][i][WL_BC_FILE_SLOT_NO];

		if (lSlotTemp == lSlotNo)
		{
			szBarcode = (*pBarcodeInCassette)[WL_BC_FILE_MAGAZINE][i][WL_BC_FILE_BARCODE];
			//szBarcode = BarcodeInCassette[WL_BC_FILE_MAGAZINE][i][WL_BC_FILE_BARCODE];
			//BarcodeInCassette.Close();
			pUtl->CloseBarcodeCassetteFile();
			return TRUE;
		}
	}

	m_szBCInCassetteFilename = (*pBarcodeInCassette)[WL_BC_FILE_MAGAZINE][WL_CASS_FILENAME];
	
	//BarcodeInCassette.Close();

	pUtl->CloseBarcodeCassetteFile();

	return FALSE;
}


BOOL CWaferLoader::ShowBarcodeInCassette()
{
	BOOL bFileExist = TRUE;
	CString szBarcode;
	CStringArray szaData, szaHeader;
	CString szTotalSlotNum;
	LONG lTotalWafer = 0;
	CString szFilename;

	if ((_access(MSD_WL_BC_IN_CASSETTE_FILE, 0 )) == -1)
	{
		bFileExist = FALSE;
	}
	else
	{
		GetBarcodeInCassetteFilename(szFilename);
		m_szBCInCassetteFilename = szFilename;
	}

	m_WfrBarcodeInCassette.Initialize("BarcodeInCasstte");
	m_WfrBarcodeInCassette.SetScrollBarSize(15);
	m_WfrBarcodeInCassette.RemoveAllData();
	
	//Statistics Header
	szaHeader.Add("Name");
	m_WfrBarcodeInCassette.SetHeader(8404992,10987412,"Wafer", 15, szaHeader);

	for (INT i = 1; i <= WL_MAX_MAG_SLOT ; i++)
	{
		szBarcode = "";
		
		if (bFileExist == TRUE)
		{
			if (GetBarcodeInCassette(szBarcode, i) == TRUE)
			{
				lTotalWafer = lTotalWafer + 1;
			}
		}

		szaData.Add(szBarcode);
		m_WfrBarcodeInCassette.AddData(8404992, 10987412, (UCHAR)i, 15, szaData);
		szaData.RemoveAll();
	}

	szTotalSlotNum.Format("%d", lTotalWafer);
	szaData.Add(szTotalSlotNum);

	m_WfrBarcodeInCassette.SetSum(8404992, 10987412, "Total Wafer", 15, szaData);
	m_WfrBarcodeInCassette.UpdateTotal();

	if ( bFileExist == FALSE )
	{
		//m_bUseBCInCassetteFile = FALSE;
		m_szBCInCassetteFilename = "";
		return FALSE;
	}

	return bFileExist;
}


BOOL CWaferLoader::ResetBarcodeInCassette()
{
	//m_bUseBCInCassetteFile = FALSE;
	m_szBCInCassetteFilename = "";

	DeleteFile(MSD_WL_BC_IN_CASSETTE_FILE);
	DeleteFile(MSD_WL_BC_IN_CASSETTE_BKF);

	ShowBarcodeInCassette();

	return TRUE;
}

BOOL CWaferLoader::CheckWaferLotData(CString szFilename, BOOL bWT2)
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	INT nMsgLength;
	char* pFilename;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->GetCustomerName() == "Primaopto")
	{
		return TRUE;
	}
	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		CMSFileUtility  *pUtl = CMSFileUtility::Instance();
		CStringMapFile  *psmf;
		if ( pUtl->LoadWaferEndDataStrMapFile() == FALSE )
		{
			HmiMessage( "Load 'WaferEndData.msd' failed!(C1)" );
			return FALSE;
		}
		psmf = pUtl->GetWaferEndDataStrMapFile();
		if ( psmf == NULL )
		{
			HmiMessage( "Load 'WaferEndData.msd' failed!(C2)" );
			return FALSE;
		}
		else 
		{
			CString szBinID;
			szBinID = (*psmf)["Yealy"][szFilename]["Corresponse BinID"];
			CMSLogFileUtility::Instance()->WL_LogStatus("Yealy reverse load back BinID: " + szBinID);
			pUtl->CloseWaferEndDataStrMapFile();

			nMsgLength = (szBinID.GetLength() + 1) * sizeof(CHAR);
			pFilename = new char[nMsgLength];
			strcpy_s(pFilename, nMsgLength, szBinID);
			
			stMsg.InitMessage(nMsgLength, pFilename);
		}		
	}
	else
	{
		nMsgLength = (szFilename.GetLength() + 1) * sizeof(CHAR);
		pFilename = new char[nMsgLength];
		strcpy_s(pFilename, nMsgLength, szFilename);
		
		stMsg.InitMessage(nMsgLength, pFilename);
	}

	INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "CheckWaferLotDataInAutoMode", stMsg);
	
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

	CString szArrTemp[4];
	szArrTemp[0] = (*m_psmfSRam)["WaferTable"]["YealyFilePath_1Layer"];  // Top == First
	szArrTemp[1] = (*m_psmfSRam)["WaferTable"]["YealyFilePath_2Layer"];
	szArrTemp[2] = (*m_psmfSRam)["WaferTable"]["YealyFilePath_3Layer"];
	szArrTemp[3] = (*m_psmfSRam)["WaferTable"]["YealyFilePath_LowestLayer"];

	delete [] pFilename;
	return bReturn;
}

BOOL CWaferLoader::CheckBatchIdFile(CString szFilename)
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

BOOL CWaferLoader::ClearBatchIDInformation()
{
	if (CMS896AStn::m_bEnableBatchIdFileCheck == FALSE)
	{
		return TRUE;
	}

	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;

	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ClearBatchIDInformation", stMsg);
	
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

//================================================================
// CheckExpander()
//   Created-By  : Andrew Ng
//   Date        : 2/1/2007 2:56:01 PM
//   Description : Used in motor init sequence to check expander status
//   Remarks     : To prevent BH & W-gripper collision with expander
//================================================================
INT CWaferLoader::CheckExpander(BOOL bMoveT)
{
	SFM_CHipecChannel* pWaferMotorT = NULL;
	CMSNmAxisInfo stWTAxis_T;

	try
	{
#ifdef NU_MOTION

		if (GetExpType() == WL_EXP_CYLINDER)
		{
			if (IsExpanderOpen() == TRUE)
			{
				return ExpanderCylinderMotorPlatform(FALSE, FALSE, FALSE, FALSE);
			}

			return TRUE;
		}

		InitAxisData(stWTAxis_T);
		stWTAxis_T.m_szName				= "WaferTAxis";
		stWTAxis_T.m_szTag				= MS896A_CFG_CH_WAFTABLE_T;
		stWTAxis_T.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(stWTAxis_T);

		if (bMoveT)			//v4.16T3
		{
			LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);

			if (CMS896AApp::m_bMS100Plus9InchOption && lEnableEngageOffset!=0)	//v4.16		//MS100Plus 9Inch machine
			{
				LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
				LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				int nPosnT = (int) pApp->GetProfileInt(_T("Settings"), _T("WTEngagePositionT"), 0);
				
				CString szTemp;
				szTemp.Format("WT Expander T ENGAGE T retrieved = %d (%ld, %ld)", nPosnT, lMinPos, lMaxPos);
				DisplayMessage(szTemp);

				if (nPosnT > lMaxPos)
					nPosnT = 0;
				if (nPosnT < lMinPos)
					nPosnT = 0;

				CMS896AStn::MotionMoveTo("WaferTAxis", nPosnT, SFM_WAIT, &stWTAxis_T);
				Sleep(1000);
			}
			else
			{
				if (IsMS90())	//v4.51A17	//MS90 has no T encoder		//v4.51A20
				{
					CMS896AStn::MotionMoveTo("WaferTAxis", 0, SFM_WAIT, &stWTAxis_T);
					Sleep(1000);
				}
				else
				{
					if ( abs(CMS896AStn::MotionGetEncoderPosition("WaferTAxis", 1, &stWTAxis_T)) > 10 )
					{
						CMS896AStn::MotionMoveTo("WaferTAxis", 0, SFM_WAIT, &stWTAxis_T);
						Sleep(1000);
					}
				}
			}
		}

#else
		pWaferMotorT = m_pInitOperation->GetHipecStepper("WaferTableStn", "srvWaferT");
		if (pWaferMotorT != NULL) 
		{
			if (bMoveT &&		//v4.16T3
				abs(pWaferMotorT->GetEncoderPosition()) > 10)
			{
				pWaferMotorT->MoveTo(0);
				Sleep(1000);
			}
		}
#endif
	}  
	catch (CAsmException e) 
	{
		DisplayException(e);
	}


	SetExpanderLock(WL_ON);
	Sleep(1000);

	if (IsExpanderLock() == FALSE)
	{
		SetExpanderLock(WL_OFF);
		return Err_ExpanderLockFailed;
	}

	if (IsExpanderOpen() == TRUE)
	{
		//SetExpanderLock(WL_OFF);
		if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||	//v4.01
			 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )			//v4.28T4
		{
			return ExpanderDCMotorPlatform(FALSE, FALSE, FALSE, FALSE, 20000);
		}
		else if (GetExpType() == WL_EXP_NONE)	//v4.39T10	//Knowles MS109
		{
			return TRUE;
		}
		else
		{
			return CloseExpander();		//Err_ExpanderAlreadyOpen;
		}
	}

	if (IsExpanderClose() == TRUE)
	{
		SetExpanderLock(WL_OFF);
		return Err_ExpanderAlreadyClose;
	}

	SetExpanderLock(WL_OFF);
	return Err_ExpanderLockFailed;
}


INT CWaferLoader::CheckVacuumExpander()
{
	SetFrameLevel(WL_DOWN);
	return 0;
}

	
//================================================================
// ManualCheckExpander()
//   Created-By  : Andrew Ng
//   Date        : 2/1/2007 4:22:17 PM
//   Description : 
//   Remarks     : 
//================================================================
INT CWaferLoader::ManualCheckExpander()
{
	SFM_CHipecChannel* pWaferMotorT = NULL;
	CMSNmAxisInfo stWTAxis_T;

	try
	{
#ifdef NU_MOTION
		InitAxisData(stWTAxis_T);
		stWTAxis_T.m_szName				= "WaferTAxis";
		stWTAxis_T.m_szTag				= MS896A_CFG_CH_WAFTABLE_T;
		stWTAxis_T.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(stWTAxis_T);

		CMS896AStn::MotionPowerOff("WaferTAxis", &stWTAxis_T);
#else
		pWaferMotorT = m_pInitOperation->GetHipecStepper("WaferTableStn", "srvWaferT");
		if (pWaferMotorT == NULL) 
		{
			AfxMessageBox("Fail to get WT T motor in WaferLoader!");
			return 0;
		}
		pWaferMotorT->PowerOff();
#endif
	}  
	catch (CAsmException e) 
	{
		DisplayException(e);
		return Err_ExpanderLockFailed;				// ENGAGE fails!!
	}

	INT nStatus = Err_ExpanderLockFailed;
	INT nCount = 0;
	do 
	{
		AfxMessageBox("Expander checking fails!  Please adjust the wafer T motor manually to ENGAGE position!");
		nStatus = CheckExpander(FALSE);
		nCount++;

		//v4.16T3	//MS100 9Inch Machine
		LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);

		if ( CMS896AApp::m_bMS100Plus9InchOption && (nStatus == Err_ExpanderAlreadyClose) && (lEnableEngageOffset != 0))
		{
			LONG lEnc_T = 0;
			CString szTemp;

			if (IsMS90())	//v4.51A18
			{
				//No encoder T for MS90
				lEnc_T = CMS896AStn::MotionGetCommandPosition("WaferTAxis", &stWTAxis_T);
				szTemp.Format("WT Expander T ENGAGE T saved (CMD) = %d", lEnc_T);
			}
			else
			{
				if (CMS896AStn::MotionIsServo("WaferTAxis", &stWTAxis_T))
					lEnc_T = CMS896AStn::MotionGetEncoderPosition("WaferTAxis", 1, &stWTAxis_T);
				else
					lEnc_T = CMS896AStn::MotionGetEncoderPosition("WaferTAxis", 1.6, &stWTAxis_T);
				szTemp.Format("WT Expander T ENGAGE T saved = %d", lEnc_T);
			}

			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			pApp->WriteProfileInt(_T("Settings"), _T("WTEngagePositionT"), lEnc_T);
			DisplayMessage(szTemp);
		}

		if (nCount > 2)
			break;
	} while (nStatus != Err_ExpanderAlreadyClose);	// continue if still cannot be closed!

#ifdef NU_MOTION
	CMS896AStn::MotionPowerOn("WaferTAxis", &stWTAxis_T);
#else
	pWaferMotorT->PowerOn();
#endif

	//v2.58
	if (nStatus != Err_ExpanderAlreadyClose) {
		if (AfxMessageBox("Use SAFE mode to by-pass waferloader checking?", MB_YESNO) == IDYES)
			nStatus = Err_ExpanderAlreadyClose;
	}

	return nStatus;
}


//================================================================
// CloseExpander()
//   Created-By  : Andrew Ng
//   Date        : 2/1/2007 2:56:08 PM
//   Description : Used in motor init sequence to close expander
//   Remarks     : 
//================================================================
INT CWaferLoader::CloseExpander()
{
	//AfxMessageBox("Please close expander manually to continue!");
	//return Err_ExpanderAlreadyClose;

	SetExpanderLock(WL_ON);
	Sleep(1000);
	LONG lHotAirTime	= m_lHotBlowCloseTime;
	LONG lDriveInTime = m_lCloseDriveInTime;

	//Start to acces DC motor
	SetDCMDirecction(TRUE);			//Open = FALSE; Close = TRUE
	SetDCMPower(WL_ON);
	LONG lRunTime = 1;

	//Check Hot Air is need or not
	BOOL bEnableHotAir = TRUE;
	if (lHotAirTime > 0)
	{
		bEnableHotAir = TRUE;
		SetHotAir(WL_ON);
	}
	else
	{
		bEnableHotAir = FALSE;
		lHotAirTime = 0;
	}

	//DC Motor openning or closing
	while (1)
	{
		if ( (IsExpanderOpen() == FALSE) && (IsExpanderClose() == TRUE) )
			break;

		Sleep(10);	//v3.93
		lRunTime++;

		//Protect expander for over drive
		if (lRunTime > WL_EXP_TIMEOUT) 
		{
			if (bEnableHotAir == TRUE)
				SetHotAir(WL_OFF);
			SetDCMPower(WL_OFF);
			Sleep(500);
			SetExpanderLock(WL_OFF);
			return Err_ExpanderLockFailed;	//Err_ExpanderOpenCloseFailed;
		}

		//Stop Hot Air if hit time
		if (bEnableHotAir && (lRunTime >= lHotAirTime))
			SetHotAir(WL_OFF);
	}

	//Off DCM
	SetDCMPower(WL_OFF);

	//Continue Hot air if necessary
	if (bEnableHotAir)
	{
		if (lHotAirTime > lRunTime)
		{
			Sleep(lHotAirTime - lRunTime);
			SetHotAir(WL_OFF);
		}
		else
			SetHotAir(WL_OFF);
	}

	//DriveIn Expander
	if (lDriveInTime > 0) {
		SetDCMPower(WL_ON);
		Sleep(lDriveInTime);
		SetDCMPower(WL_OFF);
	}

	//Delay for mylar paper cool down
	if (m_lMylarCoolTime > 0)
		Sleep(m_lMylarCoolTime);

	if (IsExpanderLock() == FALSE)
	{
		SetExpanderLock(WL_OFF);
		return Err_ExpanderLockFailed;
	}

	if (IsExpanderOpen() == TRUE)
	{
		SetExpanderLock(WL_OFF);
		return Err_ExpanderAlreadyOpen;
	}

	if (IsExpanderClose() == TRUE)
	{
		SetExpanderLock(WL_OFF);

		//Update Expander Status
		(*m_psmfSRam)["MS896A"]["Expander Status"] = FALSE;
		m_bExpanderStatus = FALSE;
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_STATUS, (int)m_bExpanderStatus);

		return Err_ExpanderAlreadyClose;
	}

	SetExpanderLock(WL_OFF);
	return Err_ExpanderLockFailed;
}

	
BOOL CWaferLoader::WaitWTInitT(INT nTimeout)
{
	CSingleLock slLock(&m_evWTInitT);
	return slLock.Lock(nTimeout);
}

BOOL CWaferLoader::WaitWprInitAFZ(INT nTimeout)
{
	if( IsWprWithAF() )
	{
		CSingleLock slLock(&m_evInitAFZ);
		return slLock.Lock(nTimeout);
	}

	return TRUE;
}

VOID CWaferLoader::SetES101PreloadFrameDone(BOOL bState)
{
	if (bState)
	{
		m_evES101WldPreloadDone.SetEvent();
	}
	else
	{
		m_evES101WldPreloadDone.ResetEvent();
	}
}

VOID CWaferLoader::SetExpInit(BOOL bState)
{
	if (bState)
	{
		m_evExpInit.SetEvent();
	}
	else
	{
		m_evExpInit.ResetEvent();
	}
}

VOID CWaferLoader::SetExpInitForWt(BOOL bState)
{
	if (bState)
	{
		m_evExpInitForWt.SetEvent();
	}
	else
	{
		m_evExpInitForWt.ResetEvent();
	}
}

BOOL CWaferLoader::IsAllMotorsEnable()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
	if (m_bDisableWLWithExp)
	{
		return TRUE;
	}

	BOOL bMotorsOn = FALSE;

	bMotorsOn = (X_IsPowerOn() && Z_IsPowerOn());

	return bMotorsOn;
}

BOOL CWaferLoader::IsAllMotorsEnable2()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWL)
	{
		return TRUE;
	}
	if (m_bDisableWLWithExp)
	{
		return TRUE;
	}

	BOOL bMotorsOn = FALSE;

	bMotorsOn = (X2_IsPowerOn() && Z2_IsPowerOn());
	return bMotorsOn;
}

INT CWaferLoader::GetNextFilmFrame(BOOL bGetWIPMgzSlot_AutoLine, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bDisplayMsg, BOOL& bIsMgznEmpty)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bFrameDetected = FALSE;
	int nStatus = TRUE;
	INT lMagStatus;
	LONG lSearchDistanceX;
	BOOL bAllMagazineComplete = FALSE;
	CString szStr;

	//Searching FilmFrame until end
	int nRetryCount = 0;
	while(1)
	{
		nRetryCount++;
		if (pApp->GetCustomerName()==CTM_NICHIA && pApp->IsStopAlign() && nRetryCount>=2)
		{
			SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
			CMSLogFileUtility::Instance()->WL_LogStatus("Get next frame aborted: by stop button.");
			nStatus = Err_MagazineFull;
			break;
		}

		if (bSearchFrameInMagazine == TRUE)
		{
			if (IsMSAutoLineMode())
			{
				m_lCurrentMagNo = 1;
				//Get next frame in input Slot
				if (!GetLoadMgzSlot_AutoLine(bGetWIPMgzSlot_AutoLine))
				{
					bAllMagazineComplete = TRUE;
				}
			}
			else if (m_bUseDualTablesOption || m_bUseBLAsLoaderZ)	//v4.31T11
			{
				if ( m_bIsGetCurrentSlot == FALSE )
				{
					m_lCurrentSlotNo += (m_lSkipSlotNo + 1);
					szStr.Format("GetNext FilmFrame - Get next Slot : #%d", m_lCurrentSlotNo);
					CMSLogFileUtility::Instance()->WL_LogStatus(szStr);
				}
				else
				{
					m_bIsGetCurrentSlot = FALSE;
					szStr.Format("GetNext FilmFrame - Get Current Slot : #%d", m_lCurrentSlotNo);
					CMSLogFileUtility::Instance()->WL_LogStatus(szStr);
				}

				LONG lMaxMagNo = GetWL1MaxMgznNo();

				if (m_lCurrentMagNo <= 0 || m_lCurrentMagNo > lMaxMagNo)
				{
					bAllMagazineComplete = TRUE;
				}
				else
				{
					//Klocwork	//v4.24T11
					LONG lCurrMagNo = m_lCurrentMagNo;
					lCurrMagNo = min(lCurrMagNo, lMaxMagNo);
					lCurrMagNo = max(lCurrMagNo, 1);

					// current magazine full, find next magazine to start
					if (m_lCurrentSlotNo > m_stWaferMagazine[lCurrMagNo-1].m_lNoOfSlots)
					{
						LONG lNextMagNo = m_lCurrentMagNo + 1;

						while (lNextMagNo <= lMaxMagNo)
						{
							if (m_stWaferMagazine[lNextMagNo-1].m_lNoOfSlots > 0)
							{
								break;
							}

							lNextMagNo = lNextMagNo + 1;
						}

						if (lNextMagNo >  lMaxMagNo)
						{
							bAllMagazineComplete = TRUE;
						}
						else
						{
							//TO next available magazine
							m_lCurrentSlotNo = 1;
							m_lCurrentMagNo = lNextMagNo;
						}
					}
				}
			}
			else
			{
				m_lCurrentMagNo = 1;

				if ( m_bIsGetCurrentSlot == FALSE )
				{
					m_lCurrentSlotNo += (m_lSkipSlotNo + 1);
				}
				else
				{
					m_bIsGetCurrentSlot = FALSE;
				}

				if (m_lCurrentSlotNo > m_lTotalSlotNo)
				{
					bAllMagazineComplete = TRUE;
				}
			}

			if (bAllMagazineComplete == FALSE)
			{
				//v4.31T11
				szStr.Format("GetNext FilmFrame - MoveTo MagazineSlot  Mag=%ld Slot=%ld", m_lCurrentMagNo, m_lCurrentSlotNo);
				CMSLogFileUtility::Instance()->WL_LogStatus(szStr);

				lMagStatus = MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
				if ( lMagStatus != TRUE )
				{
					HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);

					nStatus = lMagStatus;
					break;
				}

				// check magazine exist and if not let it as the last slot of that magazine 
				//so that it will move to next magazine in next cycle
				if ((m_lCurrentSlotNo ==  1) && (IsMagazineExist() == FALSE) && (IsBurnIn() == FALSE))
				{
					m_lCurrentSlotNo = m_stWaferMagazine[m_lCurrentMagNo-1].m_lNoOfSlots;
					continue;
				}
			}
//2017.10.25
			else if (IsMSAutoLineMode() && !bGetWIPMgzSlot_AutoLine)
			{
				HouseKeeping(WL_ON, FALSE, FALSE, TRUE);
				return ERR_NOT_AVALIABLE_INPUT_SLOTS;
			}
			else
			{
				if (!IsMagazineSafeToMove())	//v3.82
				{
					SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
					HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
					SetErrorMessage("WL gripper not at safe pos while Z HOME in GetNext FilmFrame");
					CMSLogFileUtility::Instance()->WL_LogStatus("Gripper not at safe pos while Z HOME in GetNext FilmFrame");
					nStatus = ERR_MAG_NOT_SAFE;
					break;
				}

				m_lCurrentMagNo	= 1;
				m_lCurrentSlotNo = 1;
				bAllMagazineComplete = FALSE;
				bIsMgznEmpty = TRUE;	//v4.59A16

				MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
				Z_Home();
				MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);

				if (m_bUseBCInCassetteFile == TRUE)
				{
					ResetBarcodeInCassette();
					SetAlert_Red_Yellow(IDS_WL_BC_FILE_MAG_FULL);
					HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
					SetStatusMessage("WL Magazine is full");
					nStatus = ERR_CASS_FILE;
					break;
				}

				if (bBurnIn == FALSE && CMS896AApp::m_bEnableGripperLoadUnloadTest == FALSE)
				{
					LONG lOption;
					CString szText = " ";
							
					if ( (m_bUseDualTablesOption || m_bUseBLAsLoaderZ) && m_bDisableWT2InAutoBondMode == FALSE)
					{
						// both full case
						if (m_bIsMagazine2Full == TRUE)
						{
							m_bIsMagazineFull = TRUE;
							SaveData();
							SetAlert_Red_Yellow(IDS_WL_MAGS_FULL);
							HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
							SetStatusMessage("WL All Magazines are full");
							CMSLogFileUtility::Instance()->WL_LogStatus("WL All Magazines are full in GetNext FilmFrame");
							nStatus = ERR_ALL_MAG_FULL;
							break;
						}
						else
						{
							m_bIsMagazineFull = TRUE;
							SaveData();
							// for autoloadwaferframe only
							if (bDisplayMsg == TRUE)
							{
								SetAlert_Red_Yellow(IDS_WL_MAG1_FULL);
							}
							HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
							SetStatusMessage("WL Magazine is full");
							CMSLogFileUtility::Instance()->WL_LogStatus("WL Magazine 1 full in GetNext FilmFrame");
							nStatus = Err_MagazineFull;
							break;
						}
					}
					else
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Magazine is full");
						SetErrorMessage("Magazine is full");
						
						lOption = SetAlert_Msg_NoMaterial(IDS_WL_MAG_FULL, szText, "No", "Yes", NULL);
						
						szStr.Format("WL Mgzn is FULL; user action = %d", lOption);
						CMSLogFileUtility::Instance()->WL_LogStatus(szStr);
						SetStatusMessage("WL - Magazine 1 is full to clear wafer lot data");
						IPC_CServiceMessage stMsg;

						//	auto cycle, prestart, clear wafer lot data
						int	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ClearWaferLotDataCmd", stMsg);
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

						if ( lOption != 2 )
						{
							SaveData();
							HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
							SetStatusMessage("WL Magazine is full");
							nStatus = Err_MagazineFull;
							break;
						}
					}
				}
			}
		}


		//Move Gripper to load pos & do searching
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper move to load position");
		if (WaferGripperMoveSearchJam(m_lLoadPos_X) == FALSE)		//v3.89
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ get next frame");
			SetErrorMessage("Gripper is Jam @ to load");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		if (!IsAllMotorsEnable())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at LOAD");
			SetErrorMessage("Gripper is OFF @ at LOAD");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}
		if( IsGripperMissingSteps() )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is missing steps @ to load");
			SetErrorMessage("Gripper is missing steps @ to load");
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);

			return Err_FrameJammed;
		}
		if (IsFrameJam() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ get next frame2");
			SetErrorMessage("Gripper is Jam @ to load");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);

			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);

			return Err_FrameJammed;
		}

		//v3.68T4	//andrew: suggested by Sing to check the frame-exist sensor before further drive-in
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected == TRUE)
		{
			break;
		}
		else	//v3.86
		{
			Sleep(200);
			bFrameDetected = IsFrameDetect();
			if (bFrameDetected == TRUE)
			{
				break;
			}
		}

		if (GetExpType() == WL_EXP_CYLINDER)
		{
			lSearchDistanceX = WL_GRIPPER_CYLINDER_SEARCH_DIST;	
		}
		else 
		{
			lSearchDistanceX = WL_GRIPPER_SEARCH_DIST;
		}
		
		if (FilmFrameSearchOnTable() == gnNOTOK)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Move Search Frame fails at LOAD");
			SetErrorMessage("Move Search Frame fails at LOAD");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}
/*
		if (MoveSearchFrame(-1 * (lSearchDistanceX )) != TRUE)		//v3.89
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Move Search Frame fails at LOAD");
			SetErrorMessage("Move Search Frame fails at LOAD");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}
		//April 12, 2017	//By LeoLam request		//v4.58A2
		//FilmFrameSearch();
*/
		if (!IsAllMotorsEnable())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at LOAD");
			SetErrorMessage("Gripper is OFF @ at LOAD");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}
		if( IsGripperMissingSteps() )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is missing steps @ to load");
			SetErrorMessage("Gripper is missing steps @ to load");
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}
		if (IsFrameJam() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ get next frame3");
			SetErrorMessage("Gripper is Jam @ to load");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		//Check Frame detect sensor
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected != TRUE)		//v3.86
		{
			Sleep(200);
			bFrameDetected = IsFrameDetect();
		}

		if (bFrameDetected == TRUE)
		{
			break;
		}
		else
		{
			if (bBurnIn == TRUE)
			{
				bFrameDetected = TRUE;
				break;
			}

			if (bSearchFrameInMagazine == TRUE)
			{
				X_MoveTo(m_lReadyPos_X);

				if (!IsMagazineSafeToMove())		//v3.82
				{
					SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
					CMSLogFileUtility::Instance()->WL_LogStatus("Gripper misssing-step to READY pos");
					HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
					return Err_FrameJammed;
				}

				if (IsFrameDetect() == TRUE && m_bNoSensorCheck == FALSE)
				{
					SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
					HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
					return Err_FrameExistInGripper;
				}
				
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper Search the empty slot");
				if (IsMSAutoLineMode())
				{
					// set as reject status for the current slot
					//Search Failure
					SetSlotRejectStatus(m_lCurrentMagNo - 1, m_lCurrentSlotNo - 1, MS_FRAME_SEARCH_FAILURE);
				}
			}
			else
			{
				if (IsMSAutoLineMode())
				{
					// set as reject status for the current slot
					//Search Failure
					SetSlotRejectStatus(m_lCurrentMagNo - 1, m_lCurrentSlotNo - 1, MS_FRAME_SEARCH_FAILURE);
				}
				//only for testing&manual-loading
				break;
			}
		}
	}//End search FilmFrame


	if ( nStatus != TRUE )
	{
		//v4.46T28
		szStr.Format("GetNext FilmFrame fails; Err Status = %d, ExpType = %ld", nStatus, GetExpType());
		CMSLogFileUtility::Instance()->WL_LogStatus(szStr);

		//Close expander
		if ( GetExpType() == WL_EXP_VACUUM )
		{
			ExpanderVacuumPlatform(WL_DOWN, FALSE);
		}
		else
		{
			INT nStatus = 0;
			if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) ||	//v4.01	
				 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )			//v4.28T4
			{
				nStatus = ExpanderDCMotorPlatform(WL_DOWN, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
			}
			else if (GetExpType() == WL_EXP_CYLINDER)
			{
				nStatus = ExpanderCylinderMotorPlatform(WL_DOWN, TRUE, FALSE, FALSE);
			}
			else
			{
				nStatus = ExpanderGearPlatform(WL_DOWN, TRUE, FALSE, FALSE);
			}

			if ( nStatus != 1 )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander not closed in GetNextFilmFrame");
				SetErrorMessage("Expander not closed in GetNextFilmFrame");
				SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);
			}
		}

		HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
		return nStatus;
	}

	return bFrameDetected;
}


INT CWaferLoader::PushFrameBack()
{
	//Push frame back into magazine
	CMSLogFileUtility::Instance()->WL_LogStatus("Start Push Frame Back");

	MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, m_lUnloadOffset, TRUE, FALSE);


#ifndef NU_MOTION
#else

	CMSLogFileUtility::Instance()->WL_LogStatus("Start Push Frame Move X");

	X_MoveTo_Auto(m_lLoadPos_X);	//v4.50A17

	CMSLogFileUtility::Instance()->WL_LogStatus("PushFrameBack - Before Check Jam");

	if (IsGripperMissingSteps())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is missing steps @ push back");
		SetErrorMessage("Gripper is missing steps @ push back");
		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ push back");
		SetErrorMessage("Gripper is Jam @ push back");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("PushFrameBack - Check Jam Complete");

#endif

	//Off Clip
	Sleep(100);
	SetGripperState(WL_OFF);
	Sleep(100);

	return Err_No_Error;
}

INT CWaferLoader::PushFrameBackFromTable(BOOL bStartFromHome, BOOL bBurnIn)
{
	LONG lUnloadPosX;
	BOOL bFrameDetected = FALSE;
	INT lStatus;

	CMSLogFileUtility::Instance()->WL_LogStatus("Unload start");

	if( m_bFrameOperationInAutoMode==FALSE )
	{
		if( WPR_MoveFocusToSafe()==FALSE )
		{
			return Err_AutoFocus_Z_Fail;
		}
	}

	//Check Banana scope
	if ( IsScopeDown() == TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Scope is down");
		SetErrorMessage("Scope is down");
		SetAlert_Red_Yellow(IDS_WL_SCOPE_DOWN);
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return Err_ScopeDown;
	}

	//Check Magazine is inside the loader Z
	if ( (IsMagazineExist() == FALSE) && (bBurnIn == FALSE) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Magazine not exist");
		SetErrorMessage("Magazine not exist");
		SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return Err_NoMagazineExist;
	}

	//Move Elevator to current slot + unload offset
	lStatus = MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, m_lUnloadOffset);
	if ( lStatus != TRUE )
	{
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return lStatus;
	}

	if (GetExpType() == WL_EXP_VACUUM)
	{	
		INT nStatus = FilmFrameVacuumReady();
		if ( nStatus != TRUE)
		{
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderOpenCloseFailed;
		}
		ExpanderVacuumPlatform(WL_UP, TRUE);

		//Align Frame
		AlignWaferFrame();
	}
	else
	{
		FilmFrameGearReady();
		INT nStatus = 0;
		
		if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||	//v4.01
			 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )			//v4.28T4
		{
			nStatus = ExpanderDCMotorPlatform(WL_UP, TRUE, TRUE, FALSE, m_lExpDCMotorDacValue);
		}
		else if (GetExpType() == WL_EXP_CYLINDER)
		{
			nStatus = ExpanderCylinderMotorPlatform(WL_UP, TRUE, TRUE, FALSE);
		}
		else
		{
			nStatus = ExpanderGearPlatform(WL_UP, TRUE, TRUE, FALSE);
		}
		
		if ( nStatus == Err_ExpanderAlreadyOpen )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander already open");
			SetErrorMessage("Expander already open");
			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderAlreadyOpen;
		}
		else if ( nStatus != TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot open");
			SetErrorMessage("Expander cannot open");
			SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderOpenCloseFailed;
		}
	}


	//Enable Limit sensor protection
	//m_pStepper_X->EnableProtection(HP_LIMIT_SENSOR, TRUE);
#ifndef NU_MOTION	//v3.86
	CMS896AStn::MotionEnableProtection(WL_AXIS_X, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stWLAxis_X);
#endif

	if (GetExpType() == WL_EXP_CYLINDER)
	{
		lUnloadPosX = m_lUnloadPos_X + WL_GRIPPER_CYLINDER_SEARCH_DIST;	
	}
	else if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
	{
		lUnloadPosX = m_lUnloadPos_X +  WL_GRIPPER_DIRECTRING_SEARCH_DIST_ON_TABLE;
	}
	else 
	{
		lUnloadPosX = m_lUnloadPos_X + WL_GRIPPER_SEARCH_DIST;
	}
	
	if (lUnloadPosX > 0)
	{
		lUnloadPosX = 0;
	}

	//Move Gripper to unload pos & do searching
	X_MoveTo(lUnloadPosX);
	if (IsGripperMissingSteps())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is missing steps @ to Unload");
		SetErrorMessage("Gripper is missing steps @ to Unload");
		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to Unload");
		SetErrorMessage("Gripper is Jam @ to Unload");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
	{
		FilmFrameSearchOnTable();

		bFrameDetected = IsFrameDetect();

		if (!bFrameDetected)
		{
			Sleep(200);
			FilmFrameSearchOnTable();
			Sleep(200);
			bFrameDetected = IsFrameDetect();
		}
	}
	else
	{
		FilmFrameSearch();
	}
	m_bFrameExistOnExp = FALSE;			//v2.64
	SaveData();

	//Check Frame detect sensor
	bFrameDetected = IsFrameDetect();

	if (bBurnIn == TRUE)
	{
		bFrameDetected = TRUE;
	}

	if (bFrameDetected == TRUE || m_bNoSensorCheck == TRUE)
	{
		if (IsFrameJam() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ frame detected");
			SetErrorMessage("Gripper is Jam @ frame detected");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		//Offset for Unload Position
		X_Move(m_lStepOnUnloadPos);

		//On Clip
		SetGripperState(WL_ON);

		//Move Gripper to load pos
		X_MoveTo(m_lLoadPos_X);
		if (IsGripperMissingSteps())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is missing steps @ to load");
			SetErrorMessage("Gripper is missing steps @ to load");

			HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		if (IsFrameJam() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to load push back");
			SetErrorMessage("Gripper is Jam @ to load");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping(WL_ON, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}


		//Off Clip
		Sleep(100);
		SetGripperState(WL_OFF);
		Sleep(100);

		//Move Gripper to load pos
		X_MoveTo(m_lReadyPos_X);
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("No frame is detected");
		SetErrorMessage("No frame is detected");
		SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
		HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
		return Err_NoFrameDetected;
	}
	return TRUE;
	
}

INT CWaferLoader::PushFrameBackFromTable2(BOOL bStartFromHome, BOOL bBurnIn)
{
	LONG lUnloadPosX;
	BOOL bFrameDetected = FALSE;
	INT lStatus;

	CMSLogFileUtility::Instance()->WL_LogStatus("Unload start");

	if ( IsESDualWL()==FALSE )
		return TRUE;
	if( m_bFrameOperationInAutoMode==FALSE )
	{
		if( WPR_MoveFocusToSafe()==FALSE )
		{
			return Err_AutoFocus_Z_Fail;
		}
	}

	//Check Banana scope
	if ( IsScopeDown() == TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Scope is down");
		SetErrorMessage("Scope is down");
		SetAlert_Red_Yellow(IDS_WL_SCOPE_DOWN);
		HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
		return Err_ScopeDown;
	}

	//Check Magazine is inside the loader Z
	if ( (IsMagazineExist2() == FALSE) && (bBurnIn == FALSE) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Magazine2 not exist");
		SetErrorMessage("Magazine2 not exist");
		SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);
		HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
		return Err_NoMagazineExist;
	}

	//Move Elevator to current slot + unload offset
	lStatus = MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2, m_lUnloadOffset2);
	if ( lStatus != TRUE )
	{
		HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
		return lStatus;
	}

	FilmFrameGearReady2();
	INT nStatus = 0;
	
	if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||	//v4.01
			(GetExpType() == WL_EXP_GEAR_DIRECTRING) )			//v4.28T4
	{
		nStatus = ExpanderDCMotor2Platform(WL_UP, TRUE, TRUE, FALSE, m_lExpDCMotorDacValue);
	}
	else if (GetExpType() == WL_EXP_NONE)	//v4.39T10	//Knowles MS109
	{
		nStatus = TRUE;
	}
	else
	{
		nStatus = ExpanderCylinderMotorPlatform2(WL_UP, TRUE, TRUE, FALSE);
	}

	if ( nStatus == Err_ExpanderAlreadyOpen )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 already open");
		SetErrorMessage("Expander2 already open");
		SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
		HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
		return Err_ExpanderAlreadyOpen;
	}
	else if ( nStatus != TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 cannot open");
		SetErrorMessage("Expander2 cannot open");
		SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);
		HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
		return Err_ExpanderOpenCloseFailed;
	}
	
	//Enable Limit sensor protection
	//m_pStepper_X->EnableProtection(HP_LIMIT_SENSOR, TRUE);
#ifndef NU_MOTION	//v3.86
	CMS896AStn::MotionEnableProtection(WL_AXIS_X2, HP_LIMIT_SENSOR, TRUE, TRUE, &m_stWLAxis_X2);
#endif

	if (GetExpType() == WL_EXP_CYLINDER)
	{
		lUnloadPosX = m_lUnloadPos_X2 + WL_GRIPPER_CYLINDER_SEARCH_DIST;	
	}
	else if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
	{
		lUnloadPosX = m_lUnloadPos_X2 +  WL_GRIPPER_DIRECTRING_SEARCH_DIST_ON_TABLE;
	}
	else 
	{
		lUnloadPosX = m_lUnloadPos_X2 + WL_GRIPPER_SEARCH_DIST;
	}
	
	if (lUnloadPosX > 0)
	{
		lUnloadPosX = 0;
	}

	//Move Gripper to unload pos & do searching
	X2_MoveTo(lUnloadPosX);
	if (IsFrameJam2() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to Unload");
		SetErrorMessage("Gripper2 is Jam @ to Unload");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping_WT2(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	FilmFrameSearchOnTable2();

	bFrameDetected = IsFrameDetect2();

	if (!bFrameDetected)
	{
		Sleep(200);
		FilmFrameSearchOnTable2();
		Sleep(200);
		bFrameDetected = IsFrameDetect2();
	}
	
	m_bFrameExistOnExp2 = FALSE;			//v2.64
	SaveData();

	//Check Frame detect sensor
	bFrameDetected = IsFrameDetect2();

	if (bBurnIn == TRUE)
	{
		bFrameDetected = TRUE;
	}

	if (bFrameDetected == TRUE || m_bNoSensorCheck == TRUE)
	{
		if (IsFrameJam2() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ frame detected");
			SetErrorMessage("Gripper2 is Jam @ frame detected");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping_WT2(WL_ON, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		//Offset for Unload Position
		X2_Move(m_lStepOnUnloadPos2);

		//On Clip
		SetGripper2State(WL_ON);

		//Move Gripper to load pos
		X2_MoveTo(m_lLoadPos_X2);
		if (IsFrameJam2() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to load");
			SetErrorMessage("Gripper2 is Jam @ to load");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping_WT2(WL_ON, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		//Off Clip
		Sleep(100);
		SetGripper2State(WL_OFF);
		Sleep(100);

		//Move Gripper to load pos
		X2_MoveTo(m_lReadyPos_X2);
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("No frame2 is detected");
		SetErrorMessage("No frame2 is detected");
		SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
		HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);
		return Err_NoFrameDetected;
	}

	return TRUE;
	
}


BOOL CWaferLoader::IsMagazineSafeToMove(VOID)
{
	BOOL bSafe = TRUE;

	if (!m_fHardware)
	{	
		return TRUE;
	}
	if (m_bDisableWL)	//v3.61
	{
		return TRUE;
	}
	if (m_bDisableWLWithExp)
	{
		return TRUE;
	}

	//bSafe = !m_pStepper_X->IsPosLimitSensorHigh();
#ifdef NU_MOTION		//v3.86
	bSafe = TRUE;
#else
	bSafe = !CMS896AStn::MotionIsPositiveLimitHigh(WL_AXIS_X, &m_stWLAxis_X);
#endif

	//v4.02T6	//Gripper protection against magazine Z motion		//SanAn
	if (bSafe && !IsBurnIn())
	{
		GetEncoderValue();
		if ( (m_lLoadPos_X < -10000) && ((m_lEnc_X - m_lLoadPos_X) < 1000) )
		{
			bSafe = FALSE;
		}
	}

	if ( IsBurnIn() == TRUE )
	{
		return TRUE;
	}

	return bSafe;
}


BOOL CWaferLoader::IsGripperAtSafePos(LONG lCheckPos)
{
	if (!IsAllMotorsEnable())
		return FALSE;

	GetEncoderValue();
	if ( MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) )
	{
		if (m_lEnc_X < (lCheckPos + 1000))
			return FALSE;
	}
	else
	{
		if (m_lEnc_X < (lCheckPos + 500))
			return FALSE;
	}

	return TRUE;
}

BOOL CWaferLoader::WaferMapHeaderChecking()
{
	IPC_CServiceMessage stMsg;
	INT nConvID =0;
	BOOL bReturn = FALSE;

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "WaferMapHeaderCheckingCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(1);		
		}
	}

	return bReturn;
}

BOOL CWaferLoader::CheckLoadCurrentMapStatus()
{
	IPC_CServiceMessage stMsg;
	INT nConvID =0;
	BOOL bReturn = FALSE;

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "CheckLoadCurrentMapCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(1);		
		}
	}

	return bReturn;
}

LONG CWaferLoader::AutoRotateWafer(BOOL bKeepBarcodeAngle)
{
	IPC_CServiceMessage stMsg;
	INT nConvID =0;
	LONG lRotateTime = 0;
	
	//v4.46T13
	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER && GetExpType() != WL_EXP_NONE) && 
		IsExpanderLock() )	//v4.15T4	//OsramPenang
	{
		SetExpanderLock(WL_OFF);
		Sleep(500);
		if (IsExpanderLock())
		{
			SetErrorMessage("Expander unlock fail at AutoRotateWafer");
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			return FALSE;
		}
	}

	stMsg.InitMessage(sizeof(BOOL), &bKeepBarcodeAngle);	
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoRotateWafer", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(LONG), &lRotateTime);
			break;
		}
		else
		{
			Sleep(1);		
		}
	}

	if( lRotateTime>0 )
	{
		Sleep(lRotateTime);
	}

	return TRUE;
}

LONG CWaferLoader::AutoRotateWaferForBC(BOOL bKeepBarcodeAngle)
{
	IPC_CServiceMessage stMsg;
	INT nConvID =0;
	LONG lRotateTime = 0;
	
	//v4.46T13
	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER && GetExpType() != WL_EXP_NONE) && 
		IsExpanderLock() )
	{
		SetExpanderLock(WL_OFF);
		Sleep(500);
		if (IsExpanderLock())
		{
			SetErrorMessage("Expander unlock fail at AutoRotateWaferForBC");
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			return FALSE;
		}
	}

	stMsg.InitMessage(sizeof(BOOL), &bKeepBarcodeAngle);	
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoRotateWaferForBC", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(LONG), &lRotateTime);
			break;
		}
		else
		{
			Sleep(1);		
		}
	}

	if( lRotateTime>0 )
	{
		Sleep(lRotateTime);
	}

	return TRUE;
}

BOOL CWaferLoader::AutoLoadRankIDFile()
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
			Sleep(1);
		}
	}

	return bReturn;
}

BOOL CWaferLoader::WaferEndFileGeneratingCheckingCmd(const BOOL bLoadMap)
{
	if ( m_bDisableBT )
	{
		return TRUE;
	}
	
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	BOOL bTempLoadMap = bLoadMap;
	stMsg.InitMessage(sizeof(BOOL), &bTempLoadMap);

	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "WaferEndFileGeneratingCheckingCmd", stMsg);
	
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

LONG CWaferLoader::LogItems(LONG lEventNo)
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

LONG CWaferLoader::GetLogItemsString(LONG lEventNo, CString& szMsg)
{
	switch(lEventNo)
	{
	case GRIPPER_LOAD_POS:
		szMsg.Format("Gripper Load Position:,%d", m_lLoadPos_X);
		break;
	case GRIPPER_UNLOAD_POS:
		szMsg.Format("Gripper Unload Position,%d", m_lUnloadPos_X);
		break;
	case GRIPPER_READY_POS:
		szMsg.Format("Gripper Ready Position,%d", m_lReadyPos_X);
		break;
	case GRIPPER_BC_POS:
		szMsg.Format("Gripper Barcode Position,%d", m_lBarcodePos_X);
		break;
	case MAGAZINE_TOP_SLOT_POS:
		szMsg.Format("Wafer Magazine Top Slot Position,%d", m_lTopSlotLevel_Z);
		break;
	case MAGAZINE_PITCH:
		szMsg.Format("Wafer Magazine Slot Pitch,%.2f", m_dSlotPitch);
		break;
	case WAFER_HOME_DIE_POS_X:
		szMsg.Format("Wafer Home Die Position X,%d", m_lHomeDiePhyPosX);
		break;
	case WAFER_HOME_DIE_POS_Y:
		szMsg.Format("Wafer Home Die Position Y,%d", m_lHomeDiePhyPosY);
		break;
	}

	return 1;
}

BOOL CWaferLoader::CreeGenerateParameterList()
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CStdioFile cfCreePathFile;
	INT		iIndex;
	CString szLog;
	CString szLineData, szOutputLogFilePath, szTempPath, szOutputFile;

	if (pApp->GetCustomerName() != CTM_CREE)
		return TRUE;

	CString szConfigFile = _T("c:\\MapSorter\\UserData\\Parameters.csv");
	CString szMachineNo	= (*m_psmfSRam)["MS896A"]["MachineNo"];

	CMSLogFileUtility::Instance()->MS_LogOperation("\n");
	szLog = "Cree generate parameter list after load map!";
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
	bReturn = WL_GenerateParameterList();
/*	//Klocwork	//v4.04
	if( bReturn==FALSE )
	{
		szLog = "Cree Parameter List failure !";
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
		return FALSE;
	}
*/

	//Get ouput file path
	bReturn = cfCreePathFile.Open(_T("C:\\MapSorter\\Exe\\Cree.Ini"), CFile::modeRead|CFile::shareDenyNone|CFile::typeText);
	if ( !bReturn )
	{
		szLog = "Cree cree.ini not exist";
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
		return FALSE;
	}

	szOutputLogFilePath.Empty();
	cfCreePathFile.SeekToBegin();
	while (cfCreePathFile.ReadString(szLineData))
	{
		if (szLineData == "[Output Log File]")
		{
			cfCreePathFile.ReadString(szTempPath);
			iIndex = szTempPath.Find("=");
			if( iIndex!=-1 )
				szOutputLogFilePath = szTempPath.Mid(iIndex + 2);
		}
	}
	cfCreePathFile.Close();
	if( szOutputLogFilePath.IsEmpty() )
	{
		szLog = "Cree out put log path empty";
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
		return FALSE;
	}

	szOutputFile.Format("%s\\%s_Parameters.csv", szOutputLogFilePath, szMachineNo);
	CopyFile(szConfigFile, szOutputFile, FALSE);
	szLog = "Cree parameter list success to: " + szOutputFile;
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

	return TRUE;
}

BOOL CWaferLoader::WL_GenerateParameterList()
{
	BOOL bReturn;
	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	CString szMsg;

	//1. Remove old Parameter List
	CString szConfigFile = _T("c:\\MapSorter\\UserData\\Parameters.csv");
	DeleteFile(szConfigFile);

	CStdioFile oFile;
	CString szLine;
	if (oFile.Open(szConfigFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == TRUE)
	{
		oFile.SeekToEnd();

#ifdef NU_MOTION
		oFile.WriteString("MS100 Mapping Sorter Machine Parameter List\n\n");
#else
		oFile.WriteString("MS899/810 Mapping Sorter Machine Parameter List\n\n");
#endif
		oFile.WriteString("S/N: \n");

		CString szMachineNo	= (*m_psmfSRam)["MS896A"]["MachineNo"];
		oFile.WriteString("Machine No: " + szMachineNo + "\n");

		CTime CurTime = CTime::GetCurrentTime();
		oFile.WriteString("Date/Time: " + CurTime.Format("%H:%M:%S %d/%m/%y") + "\n");		

		CString szSoftwareVersion	= (*m_psmfSRam)["MS896A"]["Software Version"];
		oFile.WriteString("Software Version: " + szSoftwareVersion + "\n");

		CString szFirmware1 = (*m_psmfSRam)["MS896A"]["Firmware1"];
		CString szFirmware2	= (*m_psmfSRam)["MS896A"]["Firmware2"];
		oFile.WriteString("Motion Firmware Version: " + szFirmware1 + "  " + szFirmware2 + "\n");

		oFile.WriteString("\n=====================================================================\n");
		oFile.WriteString("QB1 / QCM ___ / QPB / QBO / Other : ____ \n");
		oFile.WriteString("=====================================================================\n\n");
		oFile.WriteString("Submitted By: ____________________  Date: __________________\n\n");
		oFile.Close();
	}


	//2. Generate BH Config Data
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "GenerateConfigData", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}


	//3. Bin Table Config Data
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GenerateConfigData", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	//4. WaferLoader Config Data
	GenerateConfigData();

	//5. BinLoader Config Data
	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "GenerateConfigData", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return TRUE;
}

BOOL CWaferLoader::WPR_MoveFocusToSafe()
{
	BOOL bReturn = TRUE;
	INT nConvID;
	IPC_CServiceMessage stMsg;

	if( IsWprWithAF() )
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "GotoHomeLevel", stMsg);
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
			HmiMessage_Red_Back("WL move WPR AF motor to safe fail");
			CMSLogFileUtility::Instance()->WL_LogStatus("WPR move focus to safe fail.");
		}
	}

	return bReturn;
}

BOOL CWaferLoader::WPR_MoveToFocusLevel()
{
	BOOL bReturn = TRUE;
	INT nConvID;
	IPC_CServiceMessage stMsg;

	if( IsWprWithAF() )
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "GotoFocusLevel", stMsg);
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

	return bReturn;
}

BOOL CWaferLoader::WPR_DoAutoFocus(BOOL  bFullyAuto)
{
	BOOL bReturn = TRUE;

	if( IsWprWithAF()==FALSE )
	{
		return bReturn;
	}

	INT nConvID;
	IPC_CServiceMessage stMsg;
	SaveScanTimeEvent("  WFL: align frame wafer to do focus");

	DOUBLE dUsedTime = GetTime();
	if (m_bUseDualTablesOption)			//v4.24T9
	{
		BOOL bWft2 = FALSE;
		LONG lT = 0;
		SaveScanTimeEvent("  WFL: do focus, table to home");
		if( IsWT2InUse() )
		{
			SyncWaferTable2XYT("XY");	//	4.24TX 4
			if( IsWT2UnderCamera()==FALSE )
				MoveES101BackLightZUpDn(FALSE);
			bWft2 = TRUE;
		}
		else
		{
			SyncWaferTableXYT("XY");	//	4.24TX 4
			if( IsWT1UnderCamera()==FALSE )
				MoveES101BackLightZUpDn(FALSE);
			bWft2 = FALSE;
		}
		if( bWft2 )
		{
			if( IsWL2ExpanderSafeToMove()==FALSE )
			{
				WPR_MoveFocusToSafe();
				return FALSE;
			}
		}
		else
		{
			if( IsWL1ExpanderSafeToMove()==FALSE )
			{
				WPR_MoveFocusToSafe();
				return FALSE;
			}
		}

		ES101MoveTableToHomeDie(SFM_NOWAIT, bWft2);	//	4.24TX 4

		MoveFocusToWafer(bWft2);
		SaveScanTimeEvent("  WFL: do focus, chec WFT ok before up BLZ");
		LONG lCheckCounter = 0;
		BOOL bWftUnderCamera = FALSE;
		while( 1 )
		{
			if( bWft2 )
				bWftUnderCamera = IsWT2UnderCamera();
			else
				bWftUnderCamera = IsWT1UnderCamera();
			if( bWftUnderCamera )
			{
				break;
			}
			Sleep(10);
			lCheckCounter++;
			if( lCheckCounter>1000 )
				break;
		}

		if( bWftUnderCamera==FALSE )
		{
			if( bWft2 )
			{
				SyncWaferTable2XYT("XY");	//	or check if encoder with table limit, up blz
			}
			else
			{
				SyncWaferTableXYT("XY");	//	or check if encoder with table limit, up blz
			}
			SaveScanTimeEvent("  WFL: do focus, WFT ok before up BLZ");
		}

		RotateWFTTUnderCam( bWft2 );
		if( m_bUseContour )
		{
			if( bWft2 )
				bWftUnderCamera = IsWT2UnderCamera();
			else
				bWftUnderCamera = IsWT1UnderCamera();
		}

		if( bWftUnderCamera )
		{
			SaveScanTimeEvent("  WFL: do focus, WFT ok begin up BLZ");
			if( bFullyAuto )
				MoveES101BackLightZUpDn(TRUE);	// 4.24TX
			else
				MoveBackLightToUpLevel();
			SaveScanTimeEvent("  WFL: do focus, up BLZ done");
			if( bWft2 )
			{
				SyncWaferTable2XYT("XY");	//	or check if encoder with table limit, up blz
			}
			else
			{
				SyncWaferTableXYT("XY");	//	or check if encoder with table limit, up blz
			}
			SaveScanTimeEvent("  WFL: do focus, WFT ok after up BLZ");
		}
		else
		{
			CString szMsg;
			szMsg = "WFL: WFT not under camera, BL can not up.";
			HmiMessage_Red_Back(szMsg, "Auto Align");
			SaveScanTimeEvent("  WFL: WFT not under camera, BL can not up.");
		}
	}
	else
	{
		if( IsWL1ExpanderSafeToMove()==FALSE )
		{
			WPR_MoveFocusToSafe();
			return FALSE;
		}

		LONG	lCurrentX = 0;
		LONG	lCurrentY = 0;
		GetHomeDiePhyPosn(lCurrentX, lCurrentY);
		MoveWaferTable(lCurrentX, lCurrentY);
		WPR_MoveToFocusLevel();
	}
	m_lTimeSlot[11] = (LONG)(GetTime()-dUsedTime);		//	11.	before do auto focus, up bl and down af, the prepare time.

	dUsedTime = GetTime();
	BOOL bByHmi = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bByHmi);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "AutoFocus", stMsg);
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
	m_lTimeSlot[12] = (LONG)(GetTime()-dUsedTime);		//	12.	auto focus tuning used time.

	return bReturn;
}

BOOL CWaferLoader::MoveEjectorElevatorToSafeLevel()
{
	BOOL bReturn = TRUE;

	if( IsEjtElvtInUse() )	// 4.24TX
	{
		IPC_CServiceMessage stMsg;

		INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "MoveEjeElvtToSafeLevel", stMsg);
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
		stMsg.GetMsg(sizeof(BOOL), &bReturn);

		if (bReturn == FALSE)
		{
			SetAlert_Red_Yellow(IDS_WL_EJEL_NOT_IN_SAFE_POS);
		}
	}

	return bReturn;
}

INT CWaferLoader::MoveBinLoaderY(LONG lY)
{
	if (!m_bUseBLAsLoaderZ)
		return FALSE;

	LONG lBLY = lY;
	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(LONG), &lBLY);

	INT nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "MoveWLoaderY", stMsg);

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 1000) == TRUE )
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

	if (!bResult)
		return FALSE;
	return TRUE;
}

INT CWaferLoader::MoveBinLoaderZ(LONG lZ)
{
	if (!m_bUseBLAsLoaderZ)
		return FALSE;

	LONG lBLZ = lZ;
	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(LONG), &lBLZ);

	INT nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "MoveWLoaderZ", stMsg);

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 1000) == TRUE )
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

	if (!bResult)
		return FALSE;
	return TRUE;
}


typedef struct
{
	LONG	m_lMgznNo;
	LONG    m_lTopLevel_Z;
	LONG    m_lMidPosnY;
	LONG    m_lNoOfSlots;	
	DOUBLE  m_dSlotPitch;
	
} WL_BL_MAGAZINE;

BOOL CWaferLoader::SetWLMagazineToBL(LONG lMgzn)
{
	if (!m_bUseBLAsLoaderZ)
		return FALSE;

	//lMgzn is #1 - #3
	if ( (lMgzn < 1) || (lMgzn > GetWL1MaxMgznNo()) )
		return FALSE;

	WL_BL_MAGAZINE stData;
	stData.m_lMgznNo		= lMgzn;
	stData.m_lTopLevel_Z	= m_stWaferMagazine[lMgzn-1].m_lTopLevel_Z;
	stData.m_lMidPosnY		= m_lBinLoader_Y;
	stData.m_lNoOfSlots		= m_stWaferMagazine[lMgzn-1].m_lNoOfSlots;
	stData.m_dSlotPitch		= m_stWaferMagazine[lMgzn-1].m_dSlotPitch;


	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(WL_BL_MAGAZINE), &stData);

	INT nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "SetWLMagazine", stMsg);

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 1000) == TRUE )
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

	if (!bResult)
		return FALSE;
	return TRUE;
}


BOOL CWaferLoader::ManualClearCurrentMap()			//v4.35T3	//Knowles MS109
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp->GetCustomerName() != "Knowles" )
		return TRUE;

	if (m_WaferMapWrapper.IsMapValid() != TRUE)
		return TRUE;

	CString szContent;
	szContent = _T("Clear current map?");
	
	if (HmiMessage(szContent, "Manual Open Close Expander", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, 
					glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("Clear Map in Manual-Open-Expander");
		CMS896AStn::m_WaferMapWrapper.InitMap();
	}
	return TRUE;
}


BOOL CWaferLoader::GetWaferIDInXML(CString *szData)
{
	if ( szData == NULL )
	{
		HmiMessage("Load binframe ID in XML error(NULL)");
		return FALSE;
	}
	if ( szData->GetLength() == 0 )
	{
		return TRUE; // Yealy want to pass it for new wafer
	}
	if ( _access("WaferEndData.msd", 0 ) == -1 )
	{
		HmiMessage("Please Load the waferlot XML file first!!");
		return FALSE;
	}

	// Open the msd file to get waferID
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	if ( pUtl->LoadWaferEndDataStrMapFile() == FALSE )
	{
		HmiMessage( "Load 'WaferEndData.msd' failed!(1)" );
		return FALSE;
	}
	psmf = pUtl->GetWaferEndDataStrMapFile();
	if ( psmf == NULL )
	{
		HmiMessage( "Load 'WaferEndData.msd' failed!(2)" );
		return FALSE;
	}
	else 
	{
		CString szWaferID;
		szWaferID = (*psmf)["Yealy"][szData->GetString()]["Corresponse WaferID"];
		(*psmf)["Yealy"][szWaferID]["Corresponse BinID"] = szData->GetString();
		CMSLogFileUtility::Instance()->WL_LogStatus("Yealy input binID: " + *szData + "\n\tYealy output waferID:" + szWaferID);
		szData->Format("%s", szWaferID);
		pUtl->UpdateWaferEndDataStrMapFile();
		return TRUE;
	}
}

LONG CWaferLoader::GetCurrSlotNo()
{
	if (m_lCurrentSlotNo < 1)
	{
		m_lCurrentSlotNo = 1;
	}

	if (m_lCurrentSlotNo > WL_MAX_MAG_SLOT)
	{
		m_lCurrentSlotNo = WL_MAX_MAG_SLOT;
	}

	return min(m_lCurrentSlotNo, WL_MAX_MAG_SLOT);
}

LONG CWaferLoader::GetExpType()
{
	if (m_bIsExpDCMotorExist)
	{
		if (m_lExpanderType <= WL_EXP_GEAR_ROF)
		{
			m_lExpanderType	= WL_EXP_GEAR_NUMOTION_DCMOTOR;
		}
	}
	else
	{
		if (m_lExpanderType > WL_EXP_GEAR_ROF)
		{
			m_lExpanderType = WL_EXP_GEAR_ROF;
		}
	}

	return m_lExpanderType;
}

BOOL CWaferLoader::IsCoverOpen()
{
	if (CMS896AStn::m_bCEMark)	
	{
		CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
		if (pBondHead != NULL)
		{
			return pBondHead->IsCoverOpen();
		}
	}

	return FALSE;
}

BOOL CWaferLoader::SaveBarcodeName(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

    // open config file
    if (pUtl->LoadWLConfig() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
	psmf = pUtl->GetWLConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}
	
	(*psmf)[WL_DATA][WL_BARCODE][WL_BARCODE_NAME] = m_szBarcodeName;

	// close config file
    pUtl->CloseWLConfig();
	return TRUE;
}


BOOL CWaferLoader::IsOpNeedReplaceEjrPinOrCollet()
{
	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	DOUBLE dEjectorLifeTimePercentage = pBondHead->GetEjrLifeTimePercentage();

	if (m_bEjPinNeedReplacementAtUnloadWafer || dEjectorLifeTimePercentage > 90)
	{
		SetAlert_Yellow_Green(IDS_BH_REPLACE_EJECTOR);
		return TRUE;
	}

	return FALSE;
}


BOOL CWaferLoader::LoadPackageMsdWLData(VOID)
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
	//content
	m_lEjrCapCleanLimit		= (*psmf)[WL_DATA][WL_EXPANDER][WL_EJR_CAP_CLEAN_LIMIT];
	m_lEjrCapCleanCount		= (*psmf)[WL_DATA][WL_EXPANDER][WL_EJR_CAP_CLEAN_COUNT];

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to load WL data from PackageData.msd");

	// close config file
    pUtl->ClosePackageDataConfig();

	return TRUE;
}


BOOL CWaferLoader::SavePackageMsdWLData(VOID)
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

	//content
	(*psmf)[WL_DATA][WL_EXPANDER][WL_EJR_CAP_CLEAN_LIMIT]	= m_lEjrCapCleanLimit;
	(*psmf)[WL_DATA][WL_EXPANDER][WL_EJR_CAP_CLEAN_COUNT]	= m_lEjrCapCleanCount;

	(*m_psmfSRam)["WaferTable"]["EjectorCapCleanLimit"]		= m_lEjrCapCleanLimit;

	pUtl->UpdatePackageDataConfig();	
	// close config file
    pUtl->ClosePackageDataConfig();

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to save WL data from PackageData.msd");

	return TRUE;
}


BOOL CWaferLoader::UpdateWLPackageList(VOID)
{
	CStdioFile fTemp;
	CString szContent, szText, szTemp;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\WLPackageList.csv"), CFile::modeCreate| CFile::modeReadWrite))
	{
		return FALSE;
	}

	szText = WL_EJR_CAP_CLEAN_LIMIT;
	szText = szText + ",";
	szTemp.Format("%d,", m_lEjrCapCleanLimit);
	szContent = szContent + szText + szTemp;

	szText = WL_EJR_CAP_CLEAN_COUNT;
	szText = szText + ",";
	szTemp.Format("%d,", m_lEjrCapCleanCount);
	szContent = szContent + szText + szTemp;

	fTemp.WriteString(szContent);
	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to update WL package data to WLPackageList.csv");

	fTemp.Close();

	return TRUE;
}