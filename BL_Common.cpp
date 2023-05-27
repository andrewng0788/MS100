/////////////////////////////////////////////////////////////////
// BL_Common.cpp : Common functions of the CBinLoader class
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
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinLoader.h"
#include "math.h"
#include "FileUtil.h"
#include "MS_SecCommConstant.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CBinLoader::InitVariable(VOID)
{
	m_bUseBLAsLoaderZ		= FALSE;
	m_bDisableLoaderY		= FALSE;	//v4.55A7

    m_bComm_Z               = FALSE;
    //m_bComm_Y               = FALSE;
    m_bComm_Arm             = FALSE;
	m_bHome_Z				= FALSE;
	m_bHome_X				= FALSE;
	m_bHome_X2				= FALSE;
	//m_bHome_Y				= FALSE;
	m_bHome_Lower			= FALSE;
	m_bHome_Upper			= FALSE;
	m_bHome_Theta			= FALSE;

	m_bSel_Z				= TRUE;
	m_bSel_X				= TRUE;
	//v4.59A40	//MS50
	m_bSel_Theta			= FALSE;
	m_bSel_Upper			= FALSE;
	m_bSel_Lower			= FALSE;

	m_bIsEnabled			= FALSE;
	m_bIsFirstCycle			= FALSE;
	m_bShowDebugMessage		= FALSE;

	m_lEnc_Z				= 0;
	m_lEnc_X				= 0;
	m_lEnc_X2				= 0;
	m_lEnc_Y				= 0;
	m_lEnc_Arm				= 0;
	m_lEnc_T				= 0;
	m_bUpdateOutput			= FALSE;
	m_dGripperXRes			= 0.8;
	m_bGripperUseEncoder	= FALSE;	//v4.59A39

	/*--- Variable need to be saved in file & as Server Variables---*/
	m_bHomeSnr_X			= FALSE;
	m_bHomeSnr_X2			= FALSE;
	m_bHomeSnr_Y			= FALSE;
	m_bHomeSnr_Z			= FALSE;
	m_bHomeSnr_Arm			= FALSE;
	m_bULimitSnr_X			= FALSE;
	m_bLLimitSnr_X			= FALSE;
	m_bULimitSnr_Y			= FALSE;
	m_bLLimitSnr_Y			= FALSE;
	m_bULimitSnr_Z			= FALSE;
	m_bLLimitSnr_Z			= FALSE;
	
	m_bHomeSnr_Upper		= FALSE; 
	m_bHomeSnr_Lower		= FALSE;
	m_bHomeSnr_Theta		= FALSE; 

	m_bFrameInClampDetect	= FALSE;
	m_bFrameInClampDetect2	= FALSE;
	m_bFrameJam				= FALSE;
	m_bFrameJam2			= FALSE;
	m_bFrameOnBTPosition	= FALSE;
	m_bFrameOnBTPosition2	= FALSE;
	m_bFrameProtection		= FALSE;
	m_bFrameProtection2		= FALSE;
	m_bFrameInMagazine		= FALSE;
	//m_bFrameOnBTDetect			= FALSE;

	m_bBufferSafetySensor_HMI			= FALSE;
    m_bElevatorDoorCover_HMI			= FALSE;
	m_bFrontRightElevatorDoorCover_HMI	= FALSE;
    for (LONG i = 0; i < MS_BL_MGZN_NUM; i++)
	{
        m_bMagazineExist[i] = FALSE;
	}

	m_bMagazine2Exist		= FALSE;

	m_bFrameOutOfMgz		= FALSE;
	m_bFrameLevel			= FALSE;
	m_bFrameLevel2			= FALSE;
	m_bFrameVacuum			= FALSE;
	m_bFrameVacuum2			= FALSE;
	m_bFrontArmFrameExist	= FALSE;
	m_bRearArmFrameExist	= FALSE;
	m_bFrontArmReady		= FALSE;
	m_bRearArmReady			= FALSE;	
	m_bBufferFrameExist		= FALSE;
	m_bUseLargeBinArea		= FALSE;		//v3.61		//TynTek MS899DLA-180 V2.0 to avoid WT & BT collision during LOAD/UNLOAD

	m_bUseTimeBeltMotorY	= FALSE;		//v3.94T3

	//Dual Arm Buffer Table Sensors
	m_bDualBufferUpperExist		= FALSE;
	m_bDualBufferLowerExist		= FALSE;
	m_bDualBufferRightProtect	= FALSE;
	m_bDualBufferLeftProtect	= FALSE;
	m_bDualBufferLevel			= FALSE;

    m_bStepMode             = FALSE;
    m_lCurrMgzn             = 0;
    m_lCurrSlot             = 0;
	m_lCurrHmiMgzn			= m_lCurrMgzn;	
	m_lCurrHmiSlot			= m_lCurrSlot + 1;
	m_lCurrBufferMgzn		= 0;
	m_lCurrBufferSlot		= 0;

    m_lCurrMgzn2            = 0;
    m_lCurrSlot2            = 0;
	m_lCurrHmiSlot2			= m_lCurrSlot2 + 1;
	m_bChangeOMRecordFileName		= FALSE;
	//Bin Gripper Position
    m_lPreUnloadPos_X       = 0;
	m_lUnloadPos_X          = 0;		
	m_lLoadMagPos_X         = 0;
	m_lUnloadMagPos_X       = 0;
	m_lReadyPos_X           = 0;		
	m_lBarcodePos_X         = 0;
	m_lBufferUnloadPos_X	= 0;

    m_lPreUnloadPos_X2      = 0;
	m_lUnloadPos_X2         = 0;		
	m_lLoadMagPos_X2        = 0;
	m_lUnloadMagPos_X2      = 0;
	m_lReadyPos_X2          = 0;		
	m_lBarcodePos_X2        = 0;
	m_lBufferUnloadPos_X2	= 0;

	m_lAutoLineUnloadZ		= 0;	//v4.56A1
	m_lAutoLineUnloadSlotID = 0;

	//Bin Loader magazine Position
	m_lOMRT                 = 0;
	m_lOMSP					= 0;
	m_szOMSP				= _T("A");
	if (m_bBLOut8MagConfig)	//v3.82
	{
		m_lOMSP		= 5;
		m_szOMSP	= _T("F");
	}
	m_lSetupMagPos					= 0;
    m_lReadyPosY					= 0;
    m_lReadyLvlZ					= 0;
    m_lUnloadOffset					= 1000;
	m_lUpperToLowerBufferOffsetZ	= 0;
	m_lUseLBufferGripperForSetup	= 0;
	m_lUnloadOffsetX				= 150;
    m_lUnloadOffsetY				= 1000;
	m_lUnloadPusherOffsetX			= 0;			//v2.93T2
	m_bIsExArmGripperAtUpPosn		= FALSE;	//v2.93T2
	m_lLoadSearchInClampOffsetX		= 0;
	m_lSIS_Top1MagClampOffsetX		= 0;	
	m_lTop2MagClampOffsetX			= 0;
	m_lSIS_Top1MagClampOffsetX2		= 0;	
	m_lTop2MagClampOffsetX2			= 0;
	m_bSemitekBLMode				= FALSE;

	//Bin Table
    m_lBTCurrentBlock       = 0;
    m_lBTCurrentBlock2      = 0;
	//m_lBTUnloadPos_X        = 0;
	//m_lBTUnloadPos_Y        = 0;		
	m_lBTUnloadPos_X2       = 0;
	m_lBTUnloadPos_Y2       = 0;		
	m_lBTBarcodePos_X       = 0;
	m_lBTBarcodePos_Y       = 0;		
	m_lBTAlignFrameCount	= 2;
	m_lBTAlignFrameDelay	= 200;
	m_lBTVacuumDelay		= 100;
	m_lBT_FrameUpBlowTime	= 2000;
	m_lExArmUpDelay			= 0;			//v2.93T2
	m_lBTLoadOffset_Y		= 0;			//v3.79
	m_bBinFrameCheckBCFail	= FALSE;
	m_lBTExArmOffsetX		= 0;
	m_lBTExArmOffsetY		= 0;
	m_lBUTAlignFrameCount = 2;
	m_lBUTAlignFrameDelay  = 200;
	m_lLoadFrameRetryCount	= 0;
	m_bDualDLPreloadEmpty	= FALSE;
	m_nRenesasBLConfigMode_Temp		= 0;
	m_nRenesasBLConfigMode	= 0;

	m_bIsClearAllBinFrame	= FALSE;

	//Exchange Arm Position
	m_bExArmBarcodeOnGripper = FALSE;
	m_lExArmPickPos			= 0;
	m_lExArmPlacePos		= 0;
	m_lExArmReadyPos		= 0;
	m_lExArmPickDelay		= 250;
	m_lExArmVacuumDelay		= 100;

	m_lBLGeneral_1			= 0;		
    m_lBLGeneral_2			= 0;		
    m_lBLGeneral_3			= 0;		
    m_lBLGeneral_4			= 0;		
    m_lBLGeneral_5			= 0;		
    m_lBLGeneral_6			= 0;		
    m_lBLGeneral_7          = 0;
	m_lBLGeneral_8			= 0;
	m_lBLGeneral_TmpA		= 0;
	m_lSelMagazineID		= IsMSAutoLineMode() ? BL_MGZ_MID_1 : 0;
	m_dSlotPitch			= 0.0;

    // bar code option and settings
	m_bWarningDisabledBarcodeScanner = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->m_bMSAutoLineMode == 1)
	{
		m_bWarningDisabledBarcodeScanner = FALSE;	//v4.57A15
	}

	m_bUseBarcode           = FALSE;
	m_bUseExtension         = FALSE;
	m_bCheckBarcode			= FALSE;
	m_bCompareBarcode		= TRUE;
	m_bStopChgGradeScan		= FALSE;
	m_bCheckBCSkipEmpty		= FALSE;
	m_bUseEmptyFrame		= FALSE;
	m_lScanRange            = 0;
    m_lTryLimits            = 0;
    m_ucCommPortNo          = 4;
	m_ucBarcodeModel		= BL_BAR_SYMBOL;
	m_bWaferToBinResort		= FALSE;
	m_bEnableMS90TablePusher	= FALSE;

	m_lTemperature			=0;
    m_szExtName.Empty();
    m_szBCName.Empty();
    m_szBCName2.Empty();
	m_szDBPreScanBCName.Empty();

	m_szBinBarcodePrefix	= "";
	m_lBinBarcodeLength		= 0;

	/*
    for(LONG i = 0; i < MS_BL_MGZN_NUM; i++)
    {
        m_stMgznRT[i].m_lMidPosnY	= 0;
        m_stMgznRT[i].m_lMidPosnY2  = 0;
        m_stMgznRT[i].m_lTopLevel	= 0;
        m_stMgznRT[i].m_lTopLevel2  = 0;
        m_stMgznRT[i].m_lSlotPitch	= 19000;
        m_stMgznRT[i].m_lNoOfSlots	= MS_BL_MGZN_SLOT;
        m_stMgznRT[i].m_lMgznUsage	= BL_MGZN_USAGE_UNUSE;
        m_stMgznRT[i].m_lMgznState	= BL_MGZN_STATE_OK;
        
		for (LONG j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
        {
            m_stMgznRT[i].m_lSlotBlock[j] = 0;
            m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
			m_stMgznRT[i].m_lTransferSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
            m_stMgznRT[i].m_SlotBCName[j].Empty();
			m_stMgznRT[i].m_SlotSN[j].Empty();
			m_stMgznRT[i].m_SlotLotNo[j].Empty();
        }
    }*/

    for (LONG i = 0; i < MS_BL_WL_MGZN_NUM; i++)
	{
		m_stWaferMgzn[i].m_lMgznNo		= i;
		m_stWaferMgzn[i].m_lTopLevel_Z	= 0;
		m_stWaferMgzn[i].m_lMidPosnY	= 0;
		m_stWaferMgzn[i].m_lNoOfSlots	= 25;
		m_stWaferMgzn[i].m_dSlotPitch	= 0.00;

		//for (j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
        //{
        //    m_stWaferMgzn[i].m_SlotBCName[j].Empty();
        //}
	}


	m_dZRes	= GetChannelResolution(MS896A_CFG_CH_BINLOADER_Z);

    // magazine usage definition
    m_szBLMgznUsageDef[BL_MGZN_USAGE_UNUSE] = "UNUSE";
    m_szBLMgznUsageDef[BL_MGZN_USAGE_FULL]  = " FULL ";
    m_szBLMgznUsageDef[BL_MGZN_USAGE_EMPTY] = "EMPTY";
    m_szBLMgznUsageDef[BL_MGZN_USAGE_ACTIVE]= "ACTIVE";
    m_szBLMgznUsageDef[BL_MGZN_USAGE_MIXED] = "MIXED";

	m_bMotionFail           = FALSE;
    m_bBurnInTestRun        = FALSE;
    m_bRealignBinFrame      = TRUE;
    m_bSetSlotBlockEnable   = FALSE;
	m_bNoSensorCheck		= FALSE;
	m_bCheckElevatorCover	= TRUE;
	m_bUseBinTableVacuum	= TRUE;	
	m_bCheckFrameIsAligned	= FALSE;
	m_bHomeGripper			= FALSE;
	m_bUnloadDone			= FALSE;	//v3.84
	m_bFrameToBeUnloadAtIdleState = FALSE;	

	if (m_bDisableBL)	//v3.94
	{
		m_bNoSensorCheck = TRUE;
	}

	m_szMagzFullFilePath	= BL_SUMMARY_PATH; 
	m_bEnableMagzFullFile	= FALSE;
	m_bExChgFullMgznOutputFile = FALSE;
	
	m_bClearAllFrameState	= FALSE;
	m_ulMinClearAllCount	= 0;
	m_lClearAllFrameMode	= 0;

	m_bNewZConfig				= FALSE;
	m_bExArmPreLoadEmptyFrame	= FALSE;		//v2.67
	m_szAutoPreLoadBarCode		= "";		//v2.67
	m_bExArmUnloadBufferFrame	= FALSE;	//v2.68
	m_bFastBcScanMethod			= FALSE;		//v2.71

	m_bDisplaySingleMgznSnr	= FALSE;	//v3.30T2
	m_bDisplayMultiMgznSnrs	= FALSE;	//v3.30T2

	m_bGenBinFullFile = FALSE;
	
	m_szBinFrameBarcode = "";
	m_szBufferFrameBarcode = "";

	//Dual buffer for upper&lower gripper
	m_clUpperGripperBuffer.InitBufferInfo();
	m_clLowerGripperBuffer.InitBufferInfo();
	m_lTestBufferBlock 			= 0;
	m_lTestTableBlock 			= 0;
	m_bDualBufferPreLoadEmptyFrame	= FALSE;	
	m_bDualBufferUnloadBufferFrame	= FALSE;
	m_bIsNewDualBuffer = FALSE;

	m_lDualBufferTestMgzn = 0;

	m_lFrameLevelTestCount		 = 0;
	m_bStartManualLoadUnloadTest = FALSE;
	m_lTestBlkID				 = 0;
	m_lTestCycle				 = 0;
	
	m_lUnloadToFullTimeSpan		=  14;		//v4.60A2
	m_bLoadUnloadBtToggle		= FALSE;
	m_lBTBackupBlock			= 0;

	m_bBarcodeReadAbort		= FALSE;		//v3.71T3
	//m_bEnableCreeBinBcSummary	= FALSE;	//v3.75
	m_szCreeBinBcSummaryPath	= _T("");	//v3.75

	m_bEnableBLMotionLog	= FALSE;
	m_bDisableBinFrameStatusSummaryFile = FALSE;	//v4.53A5	//Dicon

	m_bFrontGateSensor		= FALSE;
	m_bBackGateSensor		= FALSE;

	for(LONG i = 1; i <= 200; i++)
	{
		m_bClearBin[i]		= m_bClearBinHmi[i] = FALSE;
	}

	m_bAutoLineLoadDone			= FALSE;
	m_bAutoLineUnloadDone		= FALSE;
	m_bTriggerTransferBinFrame	= FALSE;

	m_lLoadUnloadObject			= 0;
	m_lLoadUnloadSECSCmdObject	= 0;

	m_bAutoLoadUnloadTest		= FALSE;
	m_lSecsGemHostCommandErr	= 0;

	m_bBurnInEnable				= FALSE;
	m_lLoopTestCounter			= 0;

	m_bFrameLevelOn = FALSE;

	m_clUpperGripperBuffer.SetName(_T("Upper"));
	m_clUpperGripperBuffer.SetType(BL_BUFFER_UPPER);
	m_clUpperGripperBuffer.SetFrameInClampSensorName(BL_SI_FrameDetect);
	m_clUpperGripperBuffer.SetFrameJamSensorName(BL_SI_FRAMEJAM1);
	m_clUpperGripperBuffer.SetFrameExistSensorName(BL_SI_DualBufferUpperExist);
	m_clUpperGripperBuffer.SetGripperClampName(BL_SO_GripperState);
	m_clUpperGripperBuffer.SetAddrReadyPos_X(&m_lReadyPos_X);
	m_clUpperGripperBuffer.SetAddrPreUnloadPos_X(&m_lPreUnloadPos_X);
	m_clUpperGripperBuffer.SetAddrLoadMagPos_X(&m_lLoadMagPos_X);
	m_clUpperGripperBuffer.SetAddrUnloadMagPos_X(&m_lUnloadMagPos_X);
	m_clUpperGripperBuffer.SetAddrBarcodePos_X(&m_lBarcodePos_X);
	m_clUpperGripperBuffer.SetAddrUnloadPos_X(&m_lUnloadPos_X);	
	m_clUpperGripperBuffer.SetAddrSIS_Top1UnloadMagClampOffsetX(&m_lSIS_Top1MagClampOffsetX);
	m_clUpperGripperBuffer.SetAddrTop2UnloadMagClampOffsetX(&m_lTop2MagClampOffsetX);

	m_clLowerGripperBuffer.SetName(_T("Lower"));
	m_clLowerGripperBuffer.SetType(BL_BUFFER_LOWER);
	m_clLowerGripperBuffer.SetFrameInClampSensorName(BL_SI_FrameDetect2);
	m_clLowerGripperBuffer.SetFrameJamSensorName(BL_SI_FRAMEJAM2);
	m_clLowerGripperBuffer.SetFrameExistSensorName(BL_SI_DualBufferLowerExist);
	m_clLowerGripperBuffer.SetGripperClampName(BL_SO_Gripper2State);
	m_clLowerGripperBuffer.SetAddrReadyPos_X(&m_lReadyPos_X2);
	m_clLowerGripperBuffer.SetAddrPreUnloadPos_X(&m_lPreUnloadPos_X2);
	m_clLowerGripperBuffer.SetAddrLoadMagPos_X(&m_lLoadMagPos_X2);
	m_clLowerGripperBuffer.SetAddrUnloadMagPos_X(&m_lUnloadMagPos_X2);
	m_clLowerGripperBuffer.SetAddrBarcodePos_X(&m_lBarcodePos_X2);
	m_clLowerGripperBuffer.SetAddrUnloadPos_X(&m_lUnloadPos_X2);
	//m_clLowerGripperBuffer.SetAddrSIS_Top1UnloadMagClampOffsetX(&m_lSIS_Top1MagClampOffsetX2);
	//m_clLowerGripperBuffer.SetAddrTop2UnloadMagClampOffsetX(&m_lTop2MagClampOffsetX2);
	m_clLowerGripperBuffer.SetAddrSIS_Top1UnloadMagClampOffsetX(&m_lSIS_Top1MagClampOffsetX);
	m_clLowerGripperBuffer.SetAddrTop2UnloadMagClampOffsetX(&m_lTop2MagClampOffsetX);//2019.04.25 use same input

	m_dZHomeTime = 0;
	m_lTransferingStandaloneToInline = FALSE;
	m_lTransferingInlineToStandalone1 = FALSE;
	m_lTransferingInlineToStandalone2 = FALSE;
	m_lTransferingInlineToStandalone3 = FALSE;


	m_dThetaXRes = 1.6; //standard theta
	m_lBIN_THETA_MISSING_STEP_TOL = 20;
}

CString CBinLoader::GetMgznUsage(LONG lIndex)
{
    if( lIndex < 0 || lIndex > 4)
	{
        lIndex = 0;
	}
    return m_szBLMgznUsageDef[lIndex];
}

CString CBinLoader::GetSlotUsage(const LONG lSlotUsageType)
{
	switch (lSlotUsageType)
	{
	case BL_SLOT_USAGE_UNUSE:
		return CString(_T("unuse"));
	case BL_SLOT_USAGE_FULL:
		return CString(_T(" full "));
	case BL_SLOT_USAGE_EMPTY:
		return CString(_T("empty"));
	case BL_SLOT_USAGE_ACTIVE:
		return CString(_T("active"));
	case BL_SLOT_USAGE_ACTIVE2FULL:
		return CString(_T("act2F"));
	case BL_SLOT_USAGE_INUSE:
		return CString(_T("inuse"));
	case BL_SLOT_USAGE_SORTING:
		return CString(_T(" Sort "));
	default:
		return CString(_T("unuse"));
	}
	return CString(_T("unuse"));
}

CString CBinLoader::GetOMSlotUsage(ULONG ulMgzn, ULONG ulSlot)
{
    return GetSlotUsage(m_stMgznOM[ulMgzn].m_lSlotUsage[ulSlot]);
}

CString CBinLoader::GetRTSlotUsage(ULONG ulMgzn, ULONG ulSlot)
{
    return GetSlotUsage(m_stMgznRT[ulMgzn].m_lSlotUsage[ulSlot]);
}

CString CBinLoader::GetMagazineName(ULONG ulMgzn)
{
    CString str;
    switch( ulMgzn )
    {
		case BL_MGZ_TOP_1:
			str = "top 1";
			break;

		case BL_MGZ_MID_1:
			str = "mid 1";
			break;

		case BL_MGZ_BTM_1:
			str = "btm 1";
			break;

		case BL_MGZ_TOP_2:
			str = "top 2";
			break;

		case BL_MGZ_MID_2:
			str = "mid 2";
			break;

		case BL_MGZ_BTM_2:
			str = "btm 2";
			break;

		case BL_MGZ8_BTM_1:			//MS100 8mag config		//v3.82
			str = "8mag btm 1";
			break;

		case BL_MGZ8_BTM_2:			//MS100 8mag config		//v3.82
			str = "8mag btm 2";
			break;

		default:
			//str = "no such magazine";
			str = "NONE";			//v4.17T1
			break;
    }

    return str;
}

BOOL CBinLoader::SaveMgznOMData(VOID)
{
    CString szMgzName, szModeName, szSlotName;
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	CString szSMF_NameBLOM,szMsg;
	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckEmptyFrameBC	= pApp->GetFeatureStatus(MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK);

	if( bCheckEmptyFrameBC)
	{
		//Change Name
		if(pUtl->SaveMSDFileNameByMgznOMWithPkg(szPKGFilename,szSMF_NameBLOM) == FALSE)
		{
			szMsg = "**PKG File name not existed";
			BL_DEBUGBOX(szMsg);
			SetErrorMessage(szMsg);
			HmiMessage(szMsg);
		}
		else
		{
			szMsg.Format("Save (empty grade with pkg:%s)OM file: %s",szPKGFilename, szSMF_NameBLOM);
			
			BL_DEBUGBOX(szMsg);
			HmiMessage(szMsg);
		}
	}

    // open config file
    if (pUtl->LoadBLOMConfig() == FALSE)
		return FALSE;

    // get file pointer
	psmf = pUtl->GetBLOMConfigFile();
	
	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

    // update data
    for(LONG i = 0; i < MS_BL_MGZN_NUM; i++)
    {
		szMgzName = GetMagazineName(i);
		szModeName.Format("%s %c", BL_MODE, m_lOMSP + 'A');
		
		(*psmf)[BL_MGZN_DEFAULT][szMgzName][szModeName][BL_MGZN_USAGE] = m_stMgznOM[i].m_lMgznUsage;

        for(LONG j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
        {
			szSlotName.Format("%s %d", BL_SLOT, j+1); 

			(*psmf)[BL_MGZN_DEFAULT][szMgzName][szModeName][szSlotName][BL_SLOT_BLOCK]	= m_stMgznOM[i].m_lSlotBlock[j];
			(*psmf)[BL_MGZN_DEFAULT][szMgzName][szModeName][szSlotName][BL_SLOT_USAGE]	= m_stMgznOM[i].m_lSlotUsage[j];
        }
    }

	//Check Load/Save Data
    pUtl->UpdateBLOMConfig();

	// close config file
    pUtl->CloseBLOMConfig();

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
	case 7:		//MODE H
		m_szOMSP = _T("H");
		break;
	case 0:
	default:
		m_szOMSP = _T("A");
		break;
	}

    return TRUE;
}

BOOL CBinLoader::LoadMgznOMData(VOID)
{
    short i, j;
    CString szMgzName, szModeName, szSlotName;
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckEmptyFrameBC	= pApp->GetFeatureStatus(MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK);
	CString szMsg, szSMF_NameBLOM;

	if( bCheckEmptyFrameBC)
	{
		if (pUtl->LoadMSDFileNameByMgznOMWithPkg( szPKGFilename, szSMF_NameBLOM) == FALSE)
		{
			szMsg.Format("*There is not(empty grade with pkg:%s)OM file record: %s", szPKGFilename,szSMF_NameBLOM);
			BL_DEBUGBOX(szMsg);
			SetErrorMessage(szMsg);

		}
		else
		{
			szMsg.Format("Load(empty grade with pkg:%s)OM file: %s", szPKGFilename,szSMF_NameBLOM);
			BL_DEBUGBOX(szMsg);
			if(m_bChangeOMRecordFileName)
				HmiMessage(szMsg);
		}
		m_bChangeOMRecordFileName = FALSE;
	}	


    // open config file
    if (pUtl->LoadBLOMConfig() == FALSE)
		return FALSE;

    // get file pointer
    psmf = pUtl->GetBLOMConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

    // retrive data
    for(i=0; i<MS_BL_MGZN_NUM; i++)
    {
		szMgzName = GetMagazineName(i);
		szModeName.Format("%s %c", BL_MODE, m_lOMSP + 'A');
		
		m_stMgznOM[i].m_lMgznUsage = (*psmf)[BL_MGZN_DEFAULT][szMgzName][szModeName][BL_MGZN_USAGE];

        for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
        {
			szSlotName.Format("%s %d", BL_SLOT, j+1); 

			m_stMgznOM[i].m_lSlotBlock[j] = (*psmf)[BL_MGZN_DEFAULT][szMgzName][szModeName][szSlotName][BL_SLOT_BLOCK];
			m_stMgznOM[i].m_lSlotUsage[j] = (*psmf)[BL_MGZN_DEFAULT][szMgzName][szModeName][szSlotName][BL_SLOT_USAGE];
        }
    }

    // close config file
    pUtl->CloseBLOMConfig();

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
	case 7:		//MODE H
		m_szOMSP = _T("H");
		break;
	case 0:
	default:
		m_szOMSP = _T("A");
		break;
	}

    return TRUE;
}

VOID CBinLoader::ResetAllMagazine()
{
	m_lBTCurrentBlock	= 0;
	m_lExArmBufferBlock = 0;
	m_szBinFrameBarcode = "";
	m_szBufferFrameBarcode = "";
	//v2.67
	m_lCurrMgzn			= 0;
	m_lCurrSlot			= 0;
	m_lCurrHmiMgzn		= 0;	
	m_lCurrHmiSlot		= 0;
	m_szCurrMgznDisplayName = GetMagazineName(m_lCurrMgzn).MakeUpper();		//v4.17T1

	m_clUpperGripperBuffer.InitBufferInfo();
	m_clUpperGripperBuffer.SetBinFull(FALSE);
	m_clUpperGripperBuffer.SetMgznSlotNo(0, 0);

	m_clLowerGripperBuffer.InitBufferInfo();
	m_clLowerGripperBuffer.SetBinFull(FALSE);
	m_clLowerGripperBuffer.SetMgznSlotNo(0, 0);

	//v4.02T6	//WH Semitek special LoadUnloadBtCmd fcn
	m_bLoadUnloadBtToggle	= FALSE;
	m_lBTBackupBlock		= 0;

	if (m_lOMRT == BL_MODE_F)
	{
		SetOperationMode_F(); // Reset
		SaveMgznOMData();
	}

	for(INT i = 0; i < MS_BL_MGZN_NUM; i++)
	{
		ResetMagazine(i, TRUE);
	}

	//v2.68
	//Reset SRAM such that will trigger Grade-Change in AUTOBOND
	(*m_psmfSRam)["BinTable"]["LastBlkInUse"] = 0;
	(*m_psmfSRam)["BinTable"]["BlkInUse"] = 0;
	
	SaveData();

	SetStatusMessage("Reset All Magazine Complete");
	BL_DEBUGBOX("Reset All Magazine Complete\n");
}


BOOL CBinLoader::ClearMagazine(ULONG ulMgzn)	 //Reset slot and clear-bin		//v4.51A20
{
	if (m_stMgznRT[ulMgzn].m_lMgznUsage != BL_MGZN_USAGE_ACTIVE)
		return FALSE;

	BOOL bResetEmpty = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bMagResetEmpty = pApp->GetFeatureStatus(MS896A_FUNC_BL_RESET_GRADEMAG_EMPTY);
	if (bMagResetEmpty)
	{
		bResetEmpty = TRUE;
	}

    for (INT i = 0; i < MS_BL_MAX_MGZN_SLOT; i++)
    {
		LONG lBlockID = m_stMgznRT[ulMgzn].m_lSlotBlock[i];
		SaveBarcodeData(lBlockID, m_stMgznRT[ulMgzn].m_SlotBCName[i], ulMgzn, i);

		//1. Clear bin counter
		if (ClearBinFrameCounter(lBlockID, m_stMgznRT[ulMgzn].m_SlotBCName[i], FALSE) == TRUE)		
		{	
			CString szMsg;
			szMsg.Format("ClearMagazine (%s, Slot=%ld): Clear bin (%d) - OK", 
								GetMagazineName(ulMgzn), i, lBlockID);
			BL_DEBUGBOX(szMsg);
			LogFrameFullInformation(TRUE, ulMgzn, i+1, m_stMgznRT[ulMgzn].m_SlotBCName[i]);
		
			//2. Then reset slot status
			m_stMgznRT[ulMgzn].m_lSlotBlock[i] = m_stMgznOM[ulMgzn].m_lSlotBlock[i];
			m_stMgznRT[ulMgzn].m_lSlotUsage[i] = m_stMgznOM[ulMgzn].m_lSlotUsage[i];
			m_stMgznRT[ulMgzn].m_SlotBCName[i].Empty();
			m_stMgznRT[ulMgzn].m_SlotSN[i].Empty();
			m_stMgznRT[ulMgzn].m_SlotLotNo[i].Empty();
			if (bResetEmpty)
			{
				m_stMgznRT[ulMgzn].m_lSlotUsage[i] = BL_SLOT_USAGE_ACTIVE2FULL;
				m_stMgznRT[ulMgzn].m_lTransferSlotUsage[i] = BL_SLOT_USAGE_UNUSE;
			}
		}
    }

	try {
		SaveMgznRTData();
	}
	catch(CFileException e) {
		BL_DEBUGBOX("BL SaveMgznRTData Exception in Clear Magazine");
	}

	return TRUE;
}

LONG CBinLoader::ResetMagazine(ULONG lMgzn, BOOL bIsManual, BOOL bClearBin)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMS896AStn::m_bRealignFrameDone  = TRUE;
	CMS896AStn::m_bRealignFrame2Done = TRUE;

	CString szMsg;
	szMsg.Format("Reset Magazine %d", lMgzn);
	BL_DEBUGBOX(szMsg);
    if (lMgzn >= MS_BL_MGZN_NUM)
	{
        return FALSE;
	}


    m_lOMSP = m_lOMRT;
    LoadMgznOMData();
    m_stMgznRT[lMgzn].m_lMgznState = BL_MGZN_STATE_OK;
    m_stMgznRT[lMgzn].m_lMgznUsage = m_stMgznOM[lMgzn].m_lMgznUsage;

	BOOL bResetEmpty = FALSE;
	BOOL bIsMSAutoLineMode = IsMSAutoLineMode();
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	BOOL bMagResetEmpty = pAppMod->GetFeatureStatus(MS896A_FUNC_BL_RESET_GRADEMAG_EMPTY);	//v3.70T1
	
	if ((bMagResetEmpty) && (m_stMgznRT[lMgzn].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE))
	{
		bResetEmpty = TRUE;
	}

    for (ULONG i = 0; i < MS_BL_MAX_MGZN_SLOT; i++)
    {
		ULONG ulBlock	= m_stMgznRT[lMgzn].m_lSlotBlock[i];
		LONG lSlotUsage = m_stMgznRT[lMgzn].m_lSlotUsage[i];
		BOOL bClearBinStatus = TRUE;

		BOOL bMinClearAllCount = TRUE;
		if (bIsManual)
		{
			bMinClearAllCount = FALSE;
			if (GetBinBlkBondedCount((ULONG)ulBlock) >= m_ulMinClearAllCount)
			{
				bMinClearAllCount = TRUE;
			}
		}

		if (bMinClearAllCount && bClearBin && (ulBlock != 0))
		{
			//v4.51A22
			CString szLog;
			szLog.Format("ResetMagazine (ClearBin #%ld): BC = %s ", ulBlock, m_stMgznRT[lMgzn].m_SlotBCName[i]);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			SaveBarcodeData(ulBlock, m_stMgznRT[lMgzn].m_SlotBCName[i], lMgzn, i);
			bClearBinStatus = ClearBinFrameCounter(ulBlock, m_stMgznRT[lMgzn].m_SlotBCName[i], FALSE);		
		}

		if (!bClearBin || bMinClearAllCount && (bClearBin && bClearBinStatus))
		{
			m_stMgznRT[lMgzn].m_lSlotBlock[i] = m_stMgznOM[lMgzn].m_lSlotBlock[i];
			m_stMgznRT[lMgzn].m_lSlotUsage[i] = m_stMgznOM[lMgzn].m_lSlotUsage[i];
			m_stMgznRT[lMgzn].m_SlotBCName[i].Empty();
			m_stMgznRT[lMgzn].m_SlotSN[i].Empty();
			m_stMgznRT[lMgzn].m_SlotLotNo[i].Empty();
			m_stMgznRT[lMgzn].m_lSlotWIPCounter[i] = 0;

			if (bIsMSAutoLineMode)
			{
				m_stMgznRT[lMgzn].m_lTransferSlotUsage[i] = BL_SLOT_USAGE_UNUSE;
			}
			
			//v3.70T1	//set each grade slot to "no frame" and require sequence to retrieve from EMPTY magazine
			if (!bIsMSAutoLineMode && bResetEmpty)
			{
				if (!bIsManual)		//v4.50A12	//Cree HZ
				{
					m_stMgznRT[lMgzn].m_lSlotUsage[i] = BL_SLOT_USAGE_ACTIVE2FULL;
				}
				else  if (CheckIfBinIsCleared(ulBlock))	//v4.50A12	//Cree HZ	
				{
					m_stMgznRT[lMgzn].m_lSlotUsage[i] = BL_SLOT_USAGE_ACTIVE2FULL;
					m_stMgznRT[lMgzn].m_lTransferSlotUsage[i] = BL_SLOT_USAGE_UNUSE;
				}
			}

			SECS_UpdateCassetteSlotInfo(lMgzn, i + 1);
		}
    }

	try
	{
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL SaveMgznRTData Exception in Reset Magazine");
	}

    return TRUE;
}

LONG CBinLoader::ResetMagazineEmpty(ULONG lMgzn)
{
	CMS896AStn::m_bRealignFrameDone  = TRUE;
	CMS896AStn::m_bRealignFrame2Done = TRUE;

	CString szMsg;
	szMsg.Format("Reset Magazine %d MIXED", lMgzn);
	BL_DEBUGBOX(szMsg);
    if( lMgzn>=MS_BL_MGZN_NUM )
        return FALSE;

    m_lOMSP = m_lOMRT;
    LoadMgznOMData();
    m_stMgznRT[lMgzn].m_lMgznState = BL_MGZN_STATE_OK;
    m_stMgznRT[lMgzn].m_lMgznUsage = m_stMgznOM[lMgzn].m_lMgznUsage;

	//v3.70T1	//Lexter
	BOOL bResetEmpty = FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	BOOL bMagResetEmpty = pAppMod->GetFeatureStatus(MS896A_FUNC_BL_RESET_GRADEMAG_EMPTY);	//v3.70T1
	if ( (bMagResetEmpty) && (m_stMgznRT[lMgzn].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE) )
	{
		bResetEmpty = TRUE;
	}

    for( ULONG i = 0; i < MS_BL_MAX_MGZN_SLOT; i++)
    {
		//v4.24T1	//bug fix for Cree HuiZhou
        m_stMgznRT[lMgzn].m_lSlotUsage[i] = m_stMgznOM[lMgzn].m_lSlotUsage[i];	

		if (m_stMgznRT[lMgzn].m_lSlotUsage[i] != BL_SLOT_USAGE_EMPTY)
			continue;

		m_stMgznRT[lMgzn].m_lSlotBlock[i] =	0;			//Reset FULL slot to contain grade-0
        m_stMgznRT[lMgzn].m_SlotBCName[i].Empty();
		
		ULONG ulBlock = m_stMgznRT[lMgzn].m_lSlotBlock[i];

		//v3.70T1	//set each grade slot to "no frame" and require sequence to retrieve from EMPTY magazine
		if ( bResetEmpty && 
			 (m_stMgznRT[lMgzn].m_lSlotUsage[i] == BL_SLOT_USAGE_EMPTY) &&
			 CheckIfBinIsCleared(ulBlock))	
		{
			m_stMgznRT[lMgzn].m_lSlotUsage[i] = BL_SLOT_USAGE_ACTIVE2FULL;
		}
    }

	try
	{
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL SaveMgznRTData Exception in Reset Magazine EMPTY");
	}

    return TRUE;
}

LONG CBinLoader::ResetMagazineFull(ULONG lMgzn)
{
    ULONG i;

	CMS896AStn::m_bRealignFrameDone  = TRUE;
	CMS896AStn::m_bRealignFrame2Done = TRUE;

	CString szMsg;
	szMsg.Format("Reset Magazine %d MIXED FULL", lMgzn);
	BL_DEBUGBOX(szMsg);
    if( lMgzn>=MS_BL_MGZN_NUM )
        return FALSE;

    // m_lOMSP = m_lOMRT;
    //LoadMgznOMData();
    //m_stMgznRT[lMgzn].m_lMgznState = BL_MGZN_STATE_OK;
    //m_stMgznRT[lMgzn].m_lMgznUsage = m_stMgznOM[lMgzn].m_lMgznUsage;

	if (m_lOMRT != BL_MODE_D)		//Only available for MODE D (Cree HuiZhou)
	{
		return FALSE;
	}

	for (i=0; i<MS_BL_MAX_MGZN_SLOT; i++)
    {
		if (m_stMgznRT[lMgzn].m_lSlotUsage[i] != BL_SLOT_USAGE_FULL)
			continue;

		CString szLog;
		szLog.Format("ResetMagazineFull (Mode D) at Mgzn %d Slot %d", lMgzn, i+1);
		BL_DEBUGBOX(szLog);

		m_stMgznRT[lMgzn].m_lSlotBlock[i] = 0;
        m_stMgznRT[lMgzn].m_SlotBCName[i].Empty();
    }

	try
	{
		SaveMgznRTData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL SaveMgznRTData Exception in Reset Magazine FULL");
	}
    return TRUE;
}

LONG CBinLoader::ResetMgznByPhyBlock(ULONG ulPhyBlock)
{
	switch( m_lOMRT )
	{
	case BL_MODE_A:
		break;
	case BL_MODE_B:
		break;
	case BL_MODE_C:
		INT i, j;
        for(i=0; i<MS_BL_MGZN_NUM; i++)
        {
            if( m_stMgznRT[i].m_lMgznUsage!=BL_MGZN_USAGE_MIXED )
                continue;
            for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
            {
                if( m_stMgznRT[i].m_lSlotBlock[j]==ulPhyBlock )
                {
					if( m_stMgznRT[i].m_lSlotUsage[j]==BL_SLOT_USAGE_FULL )
					{
						m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
						m_stMgznRT[i].m_lTransferSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
					}
                }
            }
        }
	case BL_MODE_D:
		break;
	}
	return 1;
}

//v4.50A12	//Cree HZ
BOOL CBinLoader::CheckIfBinIsCleared(ULONG ulBlock)		
{
	BOOL bIsCleared = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//if (pApp->GetCustomerName() != "Cree")
	//{
	//	return TRUE;
	//}

	int nConvID = 0;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(ULONG), &ulBlock);

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("CheckIsBinCleared"), stMsg);

	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bIsCleared);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bIsCleared;
}

BOOL CBinLoader::LoadMgznRTData(VOID)
{
	/*
    short i, j;
    CString szMgzName, szModeName, szSlotName;
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	//andrewng //2020-0708
	//v4.51A14		//Cree HZ
	//CSingleLock slLock(&m_CSRTMgznCurrent);
	//slLock.Lock();

    // open config file
    if (pUtl->LoadBLRTConfig() == FALSE)
	{
		//slLock.Unlock();
		return FALSE;	
	}

    // get file pointer
    psmf = pUtl->GetBLRTConfigFile();
	if (psmf == NULL)
	{
		//slLock.Unlock();
		return FALSE;
	}


    // retrive data
    for (i=0; i<MS_BL_MGZN_NUM; i++)
    {
		szMgzName = GetMagazineName(i);
		szModeName.Format("%s %c", BL_MODE, m_lOMRT + 'A');
		
		m_stMgznRT[i].m_lMgznUsage = (*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][BL_MGZN_USAGE];
		m_stMgznRT[i].m_lMgznState = (*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][BL_MGZN_STATE];

        for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
        {
			szSlotName.Format("%s %d", BL_SLOT, j+1); 

			m_stMgznRT[i].m_lSlotBlock[j]		= (*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_BLOCK];
			m_stMgznRT[i].m_lSlotWIPCounter[j]	= (*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_WIP];
			m_stMgznRT[i].m_lSlotUsage[j]		= (*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_USAGE];
			m_stMgznRT[i].m_lTransferSlotUsage[j] = (*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_TRANSFER_SLOT_USAGE];
			m_stMgznRT[i].m_SlotBCName[j]		= (*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_BCODE];
			m_stMgznRT[i].m_SlotSN[j]			= (*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_SN];
			m_stMgznRT[i].m_SlotLotNo[i]		= (*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_LOT_NO];
        }
	}

//Reset
//================================================2017.10.19=====================
/* 
	for(j=0; j<5; j++)
        {
			szSlotName.Format("%s %d", BL_SLOT, j+1); 

			m_stMgznRT[1].m_lSlotBlock[j]		= j+1;
			m_stMgznRT[1].m_lSlotWIPCounter[j]	= (j+1)*100;
			m_stMgznRT[1].m_lSlotUsage[j]		= BL_SLOT_USAGE_ACTIVE;
			m_stMgznRT[1].m_SlotBCName[j]		= szSlotName;
        }
 
	for(j=10; j<MS_BL_MAX_MGZN_SLOT; j++)
        {
			szSlotName.Format("%s %d", BL_SLOT, j+1); 

			m_stMgznRT[1].m_lSlotBlock[j]		= 0;
			m_stMgznRT[1].m_lSlotWIPCounter[j]	= 0;
			m_stMgznRT[1].m_lSlotUsage[j]		= BL_SLOT_USAGE_EMPTY;
			m_stMgznRT[1].m_SlotBCName[j]		= "";
        }

//===============================================================================
    // close config file
    pUtl->CloseBLRTConfig();

	//slLock.Unlock();*/
    return TRUE;
}


BOOL CBinLoader::SaveMgznRTData(VOID)
{

	/*
    short i, j;
    CString szMgzName, szModeName, szSlotName;
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	//andrewng //2020-0708
	//v4.51A14		//Cree HZ
	//CSingleLock slLock(&m_CSRTMgznCurrent);
	//slLock.Lock();

    // open config file
    if (pUtl->LoadBLRTConfig() == FALSE)		//v4.06		//SanAn
	{
		//slLock.Unlock();
		return FALSE;		//v3.02T6
	}

    // get file pointer
	psmf = pUtl->GetBLRTConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		//slLock.Unlock();
		return FALSE;
	}

    // update data
    for (i=0; i<MS_BL_MGZN_NUM; i++)
    {
		szMgzName = GetMagazineName(i);
		szModeName.Format("%s %c", BL_MODE, m_lOMRT + 'A');

		(*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][BL_MGZN_USAGE] = m_stMgznRT[i].m_lMgznUsage;
		(*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][BL_MGZN_STATE] = m_stMgznRT[i].m_lMgznState;

        for (j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
        {
			szSlotName.Format("%s %d", BL_SLOT, j+1); 

			(*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_BLOCK]	= m_stMgznRT[i].m_lSlotBlock[j];
			(*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_WIP]	= m_stMgznRT[i].m_lSlotWIPCounter[j];
			(*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_USAGE]	= m_stMgznRT[i].m_lSlotUsage[j];
			(*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_TRANSFER_SLOT_USAGE] = m_stMgznRT[i].m_lTransferSlotUsage[j];
			(*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_BCODE]	= m_stMgznRT[i].m_SlotBCName[j];
			(*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_SN]		= m_stMgznRT[i].m_SlotSN[j];
			(*psmf)[BL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][BL_SLOT_LOT_NO] = m_stMgznRT[i].m_SlotLotNo[j];
        }
    }

    pUtl->UpdateBLRTConfig();
    pUtl->CloseBLRTConfig();		// close config file
	//slLock.Unlock();*/
    return TRUE;
}

BOOL CBinLoader::LoadBinLoaderRunTimeData()
{
    CString szName1, szName2, szName3, szName4;
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;


    // open config file
    if (pUtl->LoadBLConfig() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
    psmf = pUtl->GetBLConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	(*m_psmfSRam)["BinLoader"]["CurrMgzn"]		= (*psmf)[BL_DATA][BL_TABLE][BL_BTCURR_MGZN];
	(*m_psmfSRam)["BinLoader"]["CurrSlot"]		= (*psmf)[BL_DATA][BL_TABLE][BL_BTCURR_SLOT];

	m_lBTCurrentBlock       = (*psmf)[BL_DATA][BL_TABLE][BL_BTCURR_BLOCK];
	m_lBTCurrentBlock2      = (*psmf)[BL_DATA][BL_TABLE][BL_BTCURR_BLOCK2];
	m_szBinFrameBarcode		= (*psmf)[BL_DATA][BL_TABLE][BL_BIN_FRAME_BARCODE];	
	m_szBinFrame2Barcode	= (*psmf)[BL_DATA][BL_TABLE][BL_BIN_FRAME2_BARCODE];	

	m_lExArmBufferBlock		= (*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_CURR_BLOCK];
	m_szBufferFrameBarcode	= (*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_BUF_FRAME_BARCODE];
	
	//Dual Buffer Table
	m_clUpperGripperBuffer.m_lBufferBlock		= (*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_U_BUFFER_CURR_BLOCK];
	m_clLowerGripperBuffer.m_lBufferBlock		= (*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_L_BUFFER_CURR_BLOCK];
	m_clUpperGripperBuffer.m_szBufferBarcode	= (*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_U_BUFFER_CURR_BARCODE];
	m_clLowerGripperBuffer.m_szBufferBarcode	= (*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_L_BUFFER_CURR_BARCODE];

    // close config file
    pUtl->CloseBLConfig();

	BL_DEBUGBOX("load bl rt data");
    return TRUE;
}

BOOL CBinLoader::LoadBinLoaderOption(VOID)
{
    CString szName1, szName2, szName3, szName4;
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;


    // open config file
    if (pUtl->LoadBLConfig() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
    psmf = pUtl->GetBLConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

    // retrive data
    // bar code
    // new added
    m_bUseBarcode           = (BOOL)((LONG)(*psmf)[BL_DATA][BL_BARCODE][BL_USE_BARCODE]);
    m_bUseExtension         = (BOOL)((LONG)(*psmf)[BL_DATA][BL_BARCODE][BL_USE_EXTENSION]);
	m_bCheckBarcode			= (BOOL)((LONG)(*psmf)[BL_DATA][BL_BARCODE][BL_CHK_BARCODE]);
	m_bCheckBCSkipEmpty		= (BOOL)((LONG)(*psmf)[BL_DATA][BL_BARCODE][BL_CHK_BC_SKIP_EMPTY]);
	m_bCompareBarcode		= (BOOL)((LONG)(*psmf)[BL_DATA][BL_BARCODE][BL_CMP_BARCODE]);
	m_szBinBarcodePrefix	= (*psmf)[BL_DATA][BL_BARCODE][BL_BARCODE_PREFIX];
	m_lBinBarcodeLength		= (*psmf)[BL_DATA][BL_BARCODE][BL_BARCODE_LENGTH];
	m_bStopChgGradeScan		= (BOOL)((LONG)(*psmf)[BL_DATA][BL_BARCODE][BL_STOP_CHGGRADE]);
    m_szExtName             = (*psmf)[BL_DATA][BL_BARCODE][BL_EXTENSION_NAME];
    m_lScanRange            = (*psmf)[BL_DATA][BL_BARCODE][BL_SCAN_RANGE];
    m_lTryLimits            = (*psmf)[BL_DATA][BL_BARCODE][BL_RETRY_COUNTER];
    m_ucCommPortNo          = (*psmf)[BL_DATA][BL_BARCODE][BL_SCAN_COM_PORT];
	m_ucBarcodeModel		= (*psmf)[BL_DATA][BL_BARCODE][BL_SCAN_MODEL];			
	m_bWaferToBinResort		= (BOOL)(LONG)((*psmf)[BL_DATA][BL_TABLE][BL_WAFER_TO_BIN_RESORT]);
	m_bEnableMS90TablePusher	= (BOOL)(LONG)((*psmf)[BL_DATA][BL_TABLE][BL_MS90_TABLE_PUSHER]);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
                                                                              
	m_bHomeGripper			= (BOOL)(LONG)(*psmf)[BL_DATA][BL_GRIPPER][BL_POWER_ON_HOME];

	m_lTemperature			=	(*psmf)[BL_DATA][BL_TABLE][BL_TEMPERATURE_DISPLAY];
    // bin table
	m_lBTAlignFrameCount	= (*psmf)[BL_DATA][BL_TABLE][BL_BT_ALN_COUNT];
	m_lBUTAlignFrameCount	= (*psmf)[BL_DATA][BL_TABLE][BL_BUT_ALN_COUNT];

	m_bRealignBinFrame      = (BOOL)((LONG)(*psmf)[BL_DATA][BL_TABLE][BL_REALIGN_BF]);
	m_bBinFrameCheckBCFail	= (BOOL)((LONG)(*psmf)[BL_DATA][BL_TABLE][BL_CHECK_BC_SAME_FAIL]);
	//v3.70T2	//PLLM special feature
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
	{
		//m_bRealignBinFrame = TRUE;	//v4.35T4
	}
	if (pApp->GetCustomerName() == "Cree")
	{
		m_bRealignBinFrame = TRUE;		//v4.46T27
	}

	m_bUseBinTableVacuum	= (BOOL)((LONG)(*psmf)[BL_DATA][BL_TABLE][BL_USE_VACUUM]);	
	m_bCheckFrameIsAligned	= (BOOL)((LONG)(*psmf)[BL_DATA][BL_TABLE][BL_CHK_BF_ALIGNED]);
	m_bCheckElevatorCover	= (BOOL)((LONG)(*psmf)[BL_DATA][BL_ELEVATOR][BL_CHK_ELEV_COVER]);		//v3.68T1

	if (IsMSAutoLineMode())		//autoline1
	{
		m_bCheckElevatorCover = FALSE;
	}

	//Magazine summary file
	m_bEnableMagzFullFile		= (BOOL)((LONG)(*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_MGZ_FILE_ENABLE]);
	m_bExChgFullMgznOutputFile	= (BOOL)((LONG)(*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_MGZ_FILE_FULL_ENABLE]);
	m_szMagzFullFilePath		= (*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_MGZ_FILE_PATH]; 
	m_szCreeBinBcSummaryPath	= (*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_CREE_BC_FILE_PATH];		//v3.75
	//m_bEnableCreeBinBcSummary	= (BOOL)(LONG)(*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_ENABLE_CREE_BC_FILE];	//v3.75

	//For Generate Empty Frame Text File, GenerateEmptyFrameTextFile()
	m_szEmptyFramePath		= (*psmf)[BL_DATA][BL_EMPTY_FRAME_FILE][BL_EMPTY_FRAME_PATH]; 

	//Min ClearAll count
	m_ulMinClearAllCount	= (*psmf)[BL_DATA][BL_MISC_OPTION][BL_MIN_CLRALL_COUNT];
	m_lClearAllFrameMode	= (*psmf)[BL_DATA][BL_MISC_OPTION][BL_CLEAR_ALL_FRAME_MODE];
	m_bFastBcScanMethod		= (BOOL)((LONG)(*psmf)[BL_DATA][BL_MISC_OPTION][BL_FAST_BC_SCAN]);		//v2.71
	
	m_bGenBinFullFile		= (BOOL)((LONG)(*psmf)[BL_DATA][BL_GEN_BIN_FULL_FILE]);

	//Exchnage arm
	m_bExArmBarcodeOnGripper = (BOOL)(LONG)(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_SCAN_BARCODE_ON_GRIPPER];	
	m_bExArmPreLoadEmptyFrame = (BOOL)((LONG)(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_PRELOAD_EMPTY_ON_BUFFER]);	//v2.67

	//Dual Buffer Table
	m_bDualBufferPreLoadEmptyFrame = (BOOL)((LONG)(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_DB_PRELOAD_EMPTY_ON_BUFFER]);	//v2.67
	m_bFastBcScanMethod		= (BOOL)((LONG)(*psmf)[BL_DATA][BL_MISC_OPTION][BL_FAST_BC_SCAN]);		//v2.71

	m_nRenesasBLConfigMode	= (BOOL)((LONG)(*psmf)[BL_DATA][BL_MISC_OPTION][BL_RENESAS_CONFIG_MODE]);

    // close config file
    pUtl->CloseBLConfig();

	CMSLogFileUtility::Instance()->BL_SetBinFrameStatusFilename(BL_BINFRAME_STATUS_SUMMARY);
	CMSLogFileUtility::Instance()->BL_SetBinFrameStatusTitle(BL_BINFRAME_STATUS_SUMMARY_TITLE);
	CMSLogFileUtility::Instance()->BL_SetEnableBinFrameStatusFile(CMS896AStn::m_bBinFrameStatusSummary);

    return TRUE;
}

BOOL CBinLoader::LoadData(VOID)
{
    short i;
    CString szName1, szName2, szName3, szName4, szName5, szName6;
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_bSemitekBLMode = (pApp->GetCustomerName()== "Semitek"	|| 
						pApp->GetCustomerName()== "TongHui"	||
						pApp->GetCustomerName()== "NSS"	||
						pApp->GetCustomerName()== "HongYi"	||				//v4.50A29
						pApp->GetCustomerName()== "SanAn"	||				//v4.42T17
						pApp->GetCustomerName()== "Electech3E(DL)"	||		//v4.44A6
						pApp->GetCustomerName()== "Electech3E"	);			//v4.22T2
    // open config file
    if (pUtl->LoadBLConfig() == FALSE)
		return FALSE;		//v3.02T6

    // get file pointer
    psmf = pUtl->GetBLConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	// new added
    // gripper
	BOOL bManualLoadPkg			= (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Manual Load Pkg"];	
	if (!bManualLoadPkg || pApp->GetProductLine() != "Rebel")
	{
		m_lPreUnloadPos_X       = (*psmf)[BL_DATA][BL_GRIPPER][BL_PREUNLOAD_POS];
		m_lUnloadPos_X			= (*psmf)[BL_DATA][BL_GRIPPER][BL_UNLOAD_POS];
		m_lLoadMagPos_X			= (*psmf)[BL_DATA][BL_GRIPPER][BL_LOAD_MAG_POS];
		m_lUnloadMagPos_X		= (*psmf)[BL_DATA][BL_GRIPPER][BL_UNLOAD_MAG_POS];
		if (m_lUnloadMagPos_X == 0)
		{
			m_lUnloadMagPos_X = m_lLoadMagPos_X;
		}
		m_lReadyPos_X			= (*psmf)[BL_DATA][BL_GRIPPER][BL_READY_POS];		
		m_lBarcodePos_X			= (*psmf)[BL_DATA][BL_GRIPPER][BL_SCAN_POS];
		m_lBufferUnloadPos_X	= (*psmf)[BL_DATA][BL_GRIPPER][BL_BUFFER_UNLOAD_POS];
	}

//	if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.17T1
//	{
	m_lPreUnloadPos_X2      = (*psmf)[BL_DATA][BL_GRIPPER2][BL_PREUNLOAD_POS];
	m_lUnloadPos_X2			= (*psmf)[BL_DATA][BL_GRIPPER2][BL_UNLOAD_POS];
	m_lLoadMagPos_X2		= (*psmf)[BL_DATA][BL_GRIPPER2][BL_LOAD_MAG_POS];
	m_lUnloadMagPos_X2		= (*psmf)[BL_DATA][BL_GRIPPER2][BL_UNLOAD_MAG_POS];
	if (m_lUnloadMagPos_X2 == 0)
	{
		m_lUnloadMagPos_X2 = m_lLoadMagPos_X2;
	}
	m_lReadyPos_X2			= (*psmf)[BL_DATA][BL_GRIPPER2][BL_READY_POS];		
	m_lBarcodePos_X2		= (*psmf)[BL_DATA][BL_GRIPPER2][BL_SCAN_POS];
	m_lBufferUnloadPos_X2	= (*psmf)[BL_DATA][BL_GRIPPER2][BL_BUFFER_UNLOAD_POS];
//	}

    // elevator
    m_lReadyPosY            = (*psmf)[BL_DATA][BL_ELEVATOR][BL_READY_POS_Y];
    m_lReadyLvlZ            = (*psmf)[BL_DATA][BL_ELEVATOR][BL_READY_LVL_Z];
	m_lAutoLineUnloadZ		= (*psmf)[BL_DATA][BL_ELEVATOR][BL_AUTOLINE_UNLOAD_Z];

	m_lLoadPos1_T			= (*psmf)[BL_DATA][BL_ELEVATOR][BL_LOAD_POS1_T];
	m_lLoadPos2_T			= (*psmf)[BL_DATA][BL_ELEVATOR][BL_LOAD_POS2_T];
	m_lReadyPos_T			= (*psmf)[BL_DATA][BL_ELEVATOR][BL_READY_POS_T];
	if ((m_lLoadPos1_T < -9000) || (m_lLoadPos1_T > 1000))
	{
		m_lLoadPos1_T = 0;
	}
	if ((m_lLoadPos2_T < -9000) || (m_lLoadPos2_T > 1000))
	{
		m_lLoadPos2_T = 0;
	}
	if ((m_lReadyPos_T < -9000) || (m_lReadyPos_T > 1000))
	{
		m_lReadyPos_T = 0;
	}

    m_lUnloadOffset					= (*psmf)[BL_DATA][BL_ELEVATOR][BL_UNLOAD_OFFSET];
	m_lUpperToLowerBufferOffsetZ	= (*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_LOWTOUP_OFFSET_Z];
    m_lUnloadOffsetX				= (*psmf)[BL_DATA][BL_ELEVATOR][BL_UNLOAD_OFFSET_X];	//v2.71
    m_lUnloadPusherOffsetX			= (*psmf)[BL_DATA][BL_ELEVATOR][BL_UNLOAD_PUSHER_OFFSET_X];	//v2.83T14
	m_lLoadSearchInClampOffsetX		= (*psmf)[BL_DATA][BL_ELEVATOR][BL_LOAD_SEARCHINCLAMP_OFFSET_X];	//v3.59
	m_lSIS_Top1MagClampOffsetX		= (*psmf)[BL_DATA][BL_ELEVATOR][BL_SIS_TOP1_LOAD_CLAMP_OFFSET_X];	
	m_lTop2MagClampOffsetX			= (*psmf)[BL_DATA][BL_ELEVATOR][BL_TOP2_LOAD_CLAMP_OFFSET_X];	
	m_lSIS_Top1MagClampOffsetX2		= (*psmf)[BL_DATA][BL_ELEVATOR][BL_SIS_TOP1_LOAD_CLAMP_OFFSET_X2];	
	m_lTop2MagClampOffsetX2			= (*psmf)[BL_DATA][BL_ELEVATOR][BL_TOP2_LOAD_CLAMP_OFFSET_X2];	
    m_lUnloadOffsetY        = (*psmf)[BL_DATA][BL_ELEVATOR][BL_UNLOAD_OFFSET_Y];
	m_lOMRT		            = (*psmf)[BL_DATA][BL_ELEVATOR][BL_OPERATION_MODE];
	m_lCurrMgzn 	        = (*psmf)[BL_DATA][BL_ELEVATOR][BL_CUR_MAGAZINE];
	m_lCurrSlot     		= (*psmf)[BL_DATA][BL_ELEVATOR][BL_CUR_SLOT];

	m_lCurrMgzn2 	        = (*psmf)[BL_DATA][BL_ELEVATOR][BL_CUR_MAGAZINE2];
	m_lCurrSlot2     		= (*psmf)[BL_DATA][BL_ELEVATOR][BL_CUR_SLOT2];

	/*
    for(i=0; i<MS_BL_MGZN_NUM; i++)
    {
        szName1.Format("%s %d %s", BL_MGZN, i, BL_SLOT_TOP_LVL);
        szName2.Format("%s %d %s", BL_MGZN, i, BL_SLOT_MID_POS);
        szName3.Format("%s %d %s", BL_MGZN, i, BL_SLOT_PITCH);
        szName4.Format("%s %d %s", BL_MGZN, i, BL_TOTAL_SLOT);
        szName5.Format("%s %d %s", BL_MGZN2, i, BL_SLOT_TOP_LVL);		//MS100 9Inch dual-table
        szName6.Format("%s %d %s", BL_MGZN2, i, BL_SLOT_MID_POS);		//MS100 9Inch dual-table

        m_stMgznRT[i].m_lTopLevel     = (*psmf)[BL_DATA][BL_ELEVATOR][szName1];
        m_stMgznRT[i].m_lMidPosnY     = (*psmf)[BL_DATA][BL_ELEVATOR][szName2];
        m_stMgznRT[i].m_lSlotPitch    = (*psmf)[BL_DATA][BL_ELEVATOR][szName3];
        m_stMgznRT[i].m_lNoOfSlots    = (*psmf)[BL_DATA][BL_ELEVATOR][szName4];
        m_stMgznRT[i].m_lTopLevel2    = (*psmf)[BL_DATA][BL_ELEVATOR][szName5];
        m_stMgznRT[i].m_lMidPosnY2    = (*psmf)[BL_DATA][BL_ELEVATOR][szName6];
    }*/

    for (i=0; i<MS_BL_WL_MGZN_NUM; i++)
	{
		szName1.Format("%d", i+1);
		m_stWaferMgzn[i].m_lMgznNo		= i+1;	//(*psmf)[BL_DATA][BL_WL_ELEVATOR][szName1];
		m_stWaferMgzn[i].m_lTopLevel_Z	= (*psmf)[BL_DATA][BL_WL_ELEVATOR][szName1][BL_SLOT_TOP_LVL];
		m_stWaferMgzn[i].m_lMidPosnY	= (*psmf)[BL_DATA][BL_WL_ELEVATOR][szName1][BL_SLOT_MID_POS];
		m_stWaferMgzn[i].m_lNoOfSlots	= (*psmf)[BL_DATA][BL_WL_ELEVATOR][szName1][BL_TOTAL_SLOT];
		m_stWaferMgzn[i].m_dSlotPitch	= (*psmf)[BL_DATA][BL_WL_ELEVATOR][szName1][BL_SLOT_PITCH];
	}

    // bin table
	//m_lBTUnloadPos_X		= (*psmf)[BL_DATA][BL_TABLE][BL_UNLOAD_PHY_X];		
	//m_lBTUnloadPos_Y		= (*psmf)[BL_DATA][BL_TABLE][BL_UNLOAD_PHY_Y];
	m_lBTUnloadPos_X2		= (*psmf)[BL_DATA][BL_TABLE][BL_UNLOAD_PHY_X2];		
	m_lBTUnloadPos_Y2		= (*psmf)[BL_DATA][BL_TABLE][BL_UNLOAD_PHY_Y2];
	m_lBTBarcodePos_X		= (*psmf)[BL_DATA][BL_TABLE][BL_BARCODE_POS_X];
	m_lBTBarcodePos_Y		= (*psmf)[BL_DATA][BL_TABLE][BL_BARCODE_POS_Y];

	//m_lBTCurrentBlock       = (*psmf)[BL_DATA][BL_TABLE][BL_BTCURR_BLOCK];
	//m_szBinFrameBarcode		= (*psmf)[BL_DATA][BL_TABLE][BL_BIN_FRAME_BARCODE];
	m_lBTLoadOffset_Y		= (*psmf)[BL_DATA][BL_TABLE][BL_LOAD_OFFSET_Y];	

	m_lBTAlignFrameDelay	= (*psmf)[BL_DATA][BL_TABLE][BL_BT_ALN_DELAY];
	m_lBUTAlignFrameDelay	= (*psmf)[BL_DATA][BL_TABLE][BL_BUT_ALN_DELAY];
	m_lBTExArmOffsetX		= (*psmf)[BL_DATA][BL_TABLE][BL_EXARM_OFFSET_X];		//v3.74T41	
	m_lBTExArmOffsetY		= (*psmf)[BL_DATA][BL_TABLE][BL_EXARM_OFFSET_Y];		//v3.74T41

	m_lBTVacuumDelay		= (*psmf)[BL_DATA][BL_TABLE][BL_VACUUM_DELAY];
	m_lBT_FrameUpBlowTime	= (*psmf)[BL_DATA][BL_TABLE][BL_FRAME_UPBLOW_TIME];
	m_lVacSettleDelay		= (*psmf)[BL_DATA][BL_TABLE][BL_VACUUM_SETTLE_DELAY];

	m_lExArmVacuumDelay		= (*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_VACUUM_DELAY];

	//Min ClearAll count
	m_ulMinClearAllCount	= (*psmf)[BL_DATA][BL_MISC_OPTION][BL_MIN_CLRALL_COUNT];
	m_lClearAllFrameMode	= (*psmf)[BL_DATA][BL_MISC_OPTION][BL_CLEAR_ALL_FRAME_MODE];
	m_bNewZConfig			= (BOOL)((LONG)(*psmf)[BL_DATA][BL_MISC_OPTION][BL_NEW_Z_CONFIG]);
	m_lLoadFrameRetryCount	= (*psmf)[BL_DATA][BL_MISC_OPTION][BL_LOAD_EMPTY_RETRY_COUNT];			//v4.43T13
	
	m_bDualDLPreloadEmpty	= (BOOL)((LONG)(*psmf)[BL_DATA][BL_DUAL_DL][BL_DUAL_DL_PRELOAD_EMPTY]);	//v4.48A15	//Freescale

	m_lExArmPickPos			= (*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_PICK_POS];	
	m_lExArmPlacePos		= (*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_PLACE_POS];	
	m_lExArmReadyPos		= (*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_READY_POS];
	m_lExArmPickDelay		= (*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_PICK_DELAY];
	m_lExArmVacuumDelay		= (*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_VACUUM_DELAY];
	m_lExArmUpDelay			= (*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_UP_DELAY];		//v2.93T2

	m_bClearAllFrameState	= (BOOL)(LONG)(*psmf)[BL_DATA][BL_RUNTIME][BL_CLEAR_ALL_FRAME_STATE];
	m_lUnloadToFullTimeSpan = (*psmf)[BL_DATA][BL_RUNTIME][BL_UNLOAD_TO_FULL];
	m_lTransferingStandaloneToInline = (BOOL)(LONG)(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_TRANSFER_STANDALONE_TO_LINE];
	m_lTransferingInlineToStandalone1 = (BOOL)(LONG)(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_TRANSFER_LINE_TO_STANDALONE1];
	m_lTransferingInlineToStandalone2 = (BOOL)(LONG)(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_TRANSFER_LINE_TO_STANDALONE2];
	m_lTransferingInlineToStandalone3 = (BOOL)(LONG)(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_TRANSFER_LINE_TO_STANDALONE3];	

	m_clUpperGripperBuffer.ReadData(&(*psmf)[BL_DATA]["UpperGripperBuffer"]);
	m_clLowerGripperBuffer.ReadData(&(*psmf)[BL_DATA]["LowerGripperBuffer"]);
    // close config file
    pUtl->CloseBLConfig();

	(*m_psmfSRam)["BinLoader"]["UseBracode"] = m_bUseBarcode;

	CMSLogFileUtility::Instance()->BL_SetBinFrameStatusFilename(BL_BINFRAME_STATUS_SUMMARY);
	CMSLogFileUtility::Instance()->BL_SetBinFrameStatusTitle(BL_BINFRAME_STATUS_SUMMARY_TITLE);
	CMSLogFileUtility::Instance()->BL_SetEnableBinFrameStatusFile(CMS896AStn::m_bBinFrameStatusSummary);

	//==================Check Data Validation========================
	//MS50 hardcode MODE F=======================
	m_lOMRT = BL_MODE_F;
	//===========================================

	//Update Gripper ReadyPos not > max Pos limit
	if ( m_lReadyPos_X > BL_GRIPPER_MAX_POS_LIMIT )
	{
		m_lReadyPos_X = BL_GRIPPER_MAX_POS_LIMIT;
	}

	if ( m_lBTAlignFrameDelay < 200 )
	{
		m_lBTAlignFrameDelay = 200;
	}

	if ( m_lBTVacuumDelay < 100 )
	{
		m_lBTVacuumDelay = 100;
	}
	if( m_lBT_FrameUpBlowTime<20 )
	{
		m_lBT_FrameUpBlowTime = 20;
	}
	if ( m_lExArmPickDelay < 250 )
	{
		m_lExArmPickDelay = 250;
	}

	if ( m_lExArmVacuumDelay < 100 )
	{
		m_lExArmVacuumDelay = 100;
	}

	m_lLoadFrameRetryCount = 3;		//v4.43T13	//SeoulOpto

	//Update Default Magazine setting = Current Setting
    m_lOMSP = m_lOMRT;
	(*m_psmfSRam)["BinLoader"]["OM"]	= m_lOMRT;				//v4.11T1

	(*m_psmfSRam)["BinLoader"]["RealignBinFrame"]			= m_bRealignBinFrame;	//v4.40T5
	(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= FALSE;				//v4.42T12

	//Update HMI variable
	m_lBLGeneral_1		= m_lPreUnloadPos_X;
	m_lBLGeneral_2		= m_lUnloadPos_X;
	m_lBLGeneral_3		= m_lBarcodePos_X;	
    m_lBLGeneral_4		= m_lLoadMagPos_X;
    m_lBLGeneral_15		= m_lUnloadMagPos_X;
	m_lBLGeneral_5		= m_lReadyPos_X;
    m_lBLGeneral_6      = m_lBTUnloadPos_X;
    m_lBLGeneral_7      = m_lBTUnloadPos_Y;
	m_lBLGeneral_8		= m_lBufferUnloadPos_X;
	m_lBLGeneral_9		= m_lBTBarcodePos_X;
	m_lBLGeneral_10		= m_lBTBarcodePos_Y;
	m_lBLGeneral_11		= m_lAutoLineUnloadZ;	//v4.56A1
	m_lBLGeneral_TmpA	= 0;
	m_lSelMagazineID	= IsMSAutoLineMode() ? BL_MGZ_MID_1 : 0;
	m_lCurrHmiMgzn		= m_lCurrMgzn;	
	m_lCurrHmiSlot		= m_lCurrSlot + 1;
	m_lCurrHmiSlot2		= m_lCurrSlot2 + 1;
	if (m_lBTCurrentBlock == 0)		//v3.94
		m_lCurrHmiSlot = 0;
	
	m_szCurrMgznDisplayName  = GetMagazineName(m_lCurrMgzn).MakeUpper();		//v4.17T1
	m_szCurrMgznDisplayName2 = GetMagazineName(m_lCurrMgzn2).MakeUpper();		//v4.17T1

	if (m_bIsExChgArmExist)
		UpdateExArmMgzSlotNum(GetCurrBinOnBT());		//v2.67

    return TRUE;
}

BOOL CBinLoader::SaveData(VOID)
{
    short i;
    CString szName1, szName2, szName3, szName4, szName5, szName6;
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	//MS50 hardcode MODE F=======================
	m_lOMRT = BL_MODE_F;
	//===========================================

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//v4.47T8
	m_bSemitekBLMode = (pApp->GetCustomerName()== "Semitek"	|| 
						pApp->GetCustomerName()== "TongHui"	||
						pApp->GetCustomerName()== "NSS"	||
						pApp->GetCustomerName()== "SanAn"	||				//v4.42T17
						pApp->GetCustomerName()== "HongYi"	||				//v4.50A29
						pApp->GetCustomerName()== "Electech3E(DL)"	||		//v4.44A6
						pApp->GetCustomerName()== "Electech3E"	);			//v4.22T2

    // open config file
    if (pUtl->LoadBLConfig() == FALSE)	//v4.06		//SanAn
		return FALSE;		//v3.02T6

    // get file pointer
	psmf = pUtl->GetBLConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

    // update data
    // bar code
    // new added
    (*psmf)[BL_DATA][BL_BARCODE][BL_USE_BARCODE]    = m_bUseBarcode;
    (*psmf)[BL_DATA][BL_BARCODE][BL_USE_EXTENSION]  = m_bUseExtension;
	(*psmf)[BL_DATA][BL_BARCODE][BL_CHK_BARCODE]	= m_bCheckBarcode;
	(*psmf)[BL_DATA][BL_BARCODE][BL_CHK_BC_SKIP_EMPTY]	= m_bCheckBCSkipEmpty;
	(*psmf)[BL_DATA][BL_BARCODE][BL_CMP_BARCODE]	= m_bCompareBarcode;
	(*psmf)[BL_DATA][BL_BARCODE][BL_STOP_CHGGRADE]	= m_bStopChgGradeScan;
    (*psmf)[BL_DATA][BL_BARCODE][BL_EXTENSION_NAME] = m_szExtName;
    (*psmf)[BL_DATA][BL_BARCODE][BL_SCAN_RANGE]     = m_lScanRange;
    (*psmf)[BL_DATA][BL_BARCODE][BL_RETRY_COUNTER]  = m_lTryLimits;
    (*psmf)[BL_DATA][BL_BARCODE][BL_SCAN_COM_PORT]  = m_ucCommPortNo;
	(*psmf)[BL_DATA][BL_BARCODE][BL_SCAN_MODEL]		= m_ucBarcodeModel;
	(*psmf)[BL_DATA][BL_BARCODE][BL_BARCODE_PREFIX] = m_szBinBarcodePrefix;
	(*psmf)[BL_DATA][BL_BARCODE][BL_BARCODE_LENGTH] = m_lBinBarcodeLength;
	(*psmf)[BL_DATA][BL_TABLE][BL_WAFER_TO_BIN_RESORT]	= (BOOL) m_bWaferToBinResort;
	(*psmf)[BL_DATA][BL_TABLE][BL_MS90_TABLE_PUSHER]	= (BOOL) m_bEnableMS90TablePusher;
	(*psmf)[BL_DATA][BL_TABLE][BL_TEMPERATURE_DISPLAY] = m_lTemperature;
    // new added
    // gripper
    (*psmf)[BL_DATA][BL_GRIPPER][BL_PREUNLOAD_POS]  = m_lPreUnloadPos_X;
	(*psmf)[BL_DATA][BL_GRIPPER][BL_UNLOAD_POS]		= m_lUnloadPos_X;
	(*psmf)[BL_DATA][BL_GRIPPER][BL_LOAD_MAG_POS]	= m_lLoadMagPos_X;
	(*psmf)[BL_DATA][BL_GRIPPER][BL_UNLOAD_MAG_POS]	= m_lUnloadMagPos_X;
	(*psmf)[BL_DATA][BL_GRIPPER][BL_READY_POS]		= m_lReadyPos_X;		
	(*psmf)[BL_DATA][BL_GRIPPER][BL_SCAN_POS]		= m_lBarcodePos_X;
	(*psmf)[BL_DATA][BL_GRIPPER][BL_BUFFER_UNLOAD_POS] = m_lBufferUnloadPos_X;
	(*psmf)[BL_DATA][BL_GRIPPER][BL_POWER_ON_HOME]	= m_bHomeGripper;

//	if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.17T1
//	{
	(*psmf)[BL_DATA][BL_GRIPPER2][BL_PREUNLOAD_POS]		= m_lPreUnloadPos_X2;
	(*psmf)[BL_DATA][BL_GRIPPER2][BL_UNLOAD_POS]		= m_lUnloadPos_X2;
	(*psmf)[BL_DATA][BL_GRIPPER2][BL_LOAD_MAG_POS]		= m_lLoadMagPos_X2;
	(*psmf)[BL_DATA][BL_GRIPPER2][BL_UNLOAD_MAG_POS]	= m_lUnloadMagPos_X2;
	(*psmf)[BL_DATA][BL_GRIPPER2][BL_READY_POS]			= m_lReadyPos_X2;		
	(*psmf)[BL_DATA][BL_GRIPPER2][BL_SCAN_POS]			= m_lBarcodePos_X2;
	(*psmf)[BL_DATA][BL_GRIPPER2][BL_BUFFER_UNLOAD_POS] = m_lBufferUnloadPos_X2;
	(*psmf)[BL_DATA][BL_GRIPPER2][BL_POWER_ON_HOME]		= m_bHomeGripper2;
//	}

    // elevator
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_CUR_MAGAZINE]					= m_lCurrMgzn;
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_CUR_SLOT]						= m_lCurrSlot;		
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_OPERATION_MODE]				= m_lOMRT;			
    (*psmf)[BL_DATA][BL_ELEVATOR][BL_READY_POS_Y]					= m_lReadyPosY;
    (*psmf)[BL_DATA][BL_ELEVATOR][BL_READY_LVL_Z]					= m_lReadyLvlZ;
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_AUTOLINE_UNLOAD_Z]				= m_lAutoLineUnloadZ;

	(*psmf)[BL_DATA][BL_ELEVATOR][BL_LOAD_POS1_T]					= m_lLoadPos1_T;
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_LOAD_POS2_T]					= m_lLoadPos2_T;
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_READY_POS_T]					= m_lReadyPos_T;

    (*psmf)[BL_DATA][BL_ELEVATOR][BL_UNLOAD_OFFSET]					= m_lUnloadOffset;
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_LOWTOUP_OFFSET_Z]			= m_lUpperToLowerBufferOffsetZ;
    (*psmf)[BL_DATA][BL_ELEVATOR][BL_UNLOAD_OFFSET_X]				= m_lUnloadOffsetX;		//v2.71
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_UNLOAD_OFFSET_Y]				= m_lUnloadOffsetY;
    (*psmf)[BL_DATA][BL_ELEVATOR][BL_UNLOAD_PUSHER_OFFSET_X]		= m_lUnloadPusherOffsetX;	//v2.93T2
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_LOAD_SEARCHINCLAMP_OFFSET_X]	= m_lLoadSearchInClampOffsetX;	//v3.59
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_SIS_TOP1_LOAD_CLAMP_OFFSET_X]	= m_lSIS_Top1MagClampOffsetX;	
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_TOP2_LOAD_CLAMP_OFFSET_X]		= m_lTop2MagClampOffsetX;	
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_SIS_TOP1_LOAD_CLAMP_OFFSET_X2]	= m_lSIS_Top1MagClampOffsetX2;	
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_TOP2_LOAD_CLAMP_OFFSET_X2]		= m_lTop2MagClampOffsetX2;	

	//MS100 9Inch dual-table config		//v4.17T1
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_CUR_MAGAZINE2]	= m_lCurrMgzn2;
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_CUR_SLOT2]		= m_lCurrSlot2;		

	/*
    for(i=0; i<MS_BL_MGZN_NUM; i++)
    {
        szName1.Format("%s %d %s", BL_MGZN, i, BL_SLOT_TOP_LVL);
        szName2.Format("%s %d %s", BL_MGZN, i, BL_SLOT_MID_POS);
        szName3.Format("%s %d %s", BL_MGZN, i, BL_SLOT_PITCH);
        szName4.Format("%s %d %s", BL_MGZN, i, BL_TOTAL_SLOT);
        szName5.Format("%s %d %s", BL_MGZN2, i, BL_SLOT_TOP_LVL);
        szName6.Format("%s %d %s", BL_MGZN2, i, BL_SLOT_MID_POS);

        (*psmf)[BL_DATA][BL_ELEVATOR][szName1]   = m_stMgznRT[i].m_lTopLevel;
        (*psmf)[BL_DATA][BL_ELEVATOR][szName2]   = m_stMgznRT[i].m_lMidPosnY;
        (*psmf)[BL_DATA][BL_ELEVATOR][szName3]   = m_stMgznRT[i].m_lSlotPitch;
        (*psmf)[BL_DATA][BL_ELEVATOR][szName4]   = m_stMgznRT[i].m_lNoOfSlots;
        (*psmf)[BL_DATA][BL_ELEVATOR][szName5]   = m_stMgznRT[i].m_lTopLevel2;		//MS100 9Inch dual-table
        (*psmf)[BL_DATA][BL_ELEVATOR][szName6]   = m_stMgznRT[i].m_lMidPosnY2;		//MS100 9Inch dual-table
    }*/

    for (i=0; i<MS_BL_WL_MGZN_NUM; i++)
	{
		szName1.Format("%d", i+1);
		//m_stWaferMgzn[i].m_lMgznNo		= i+1;	
		(*psmf)[BL_DATA][BL_WL_ELEVATOR][szName1][BL_SLOT_TOP_LVL]	= m_stWaferMgzn[i].m_lTopLevel_Z;
		(*psmf)[BL_DATA][BL_WL_ELEVATOR][szName1][BL_SLOT_MID_POS]	= m_stWaferMgzn[i].m_lMidPosnY;
		(*psmf)[BL_DATA][BL_WL_ELEVATOR][szName1][BL_TOTAL_SLOT]	= m_stWaferMgzn[i].m_lNoOfSlots;
		(*psmf)[BL_DATA][BL_WL_ELEVATOR][szName1][BL_SLOT_PITCH]	= m_stWaferMgzn[i].m_dSlotPitch;
	}

    // bin table
	(*psmf)[BL_DATA][BL_TABLE][BL_UNLOAD_PHY_X]		= m_lBTUnloadPos_X;		
	(*psmf)[BL_DATA][BL_TABLE][BL_UNLOAD_PHY_Y]		= m_lBTUnloadPos_Y;		
	(*psmf)[BL_DATA][BL_TABLE][BL_UNLOAD_PHY_X2]	= m_lBTUnloadPos_X2;		
	(*psmf)[BL_DATA][BL_TABLE][BL_UNLOAD_PHY_Y2]	= m_lBTUnloadPos_Y2;		
	(*psmf)[BL_DATA][BL_TABLE][BL_BARCODE_POS_X]	= m_lBTBarcodePos_X;		
	(*psmf)[BL_DATA][BL_TABLE][BL_BARCODE_POS_Y]	= m_lBTBarcodePos_Y;		
    (*psmf)[BL_DATA][BL_TABLE][BL_BTCURR_BLOCK]     = m_lBTCurrentBlock;
    (*psmf)[BL_DATA][BL_TABLE][BL_BTCURR_BLOCK2]    = m_lBTCurrentBlock2;

	(*psmf)[BL_DATA][BL_TABLE][BL_BTCURR_MGZN]		= (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
	(*psmf)[BL_DATA][BL_TABLE][BL_BTCURR_SLOT]		= (*m_psmfSRam)["BinLoader"]["CurrSlot"];

	(*psmf)[BL_DATA][BL_TABLE][BL_BIN_FRAME_BARCODE]	= m_szBinFrameBarcode;
	(*psmf)[BL_DATA][BL_TABLE][BL_BIN_FRAME2_BARCODE]	= m_szBinFrame2Barcode;
	(*psmf)[BL_DATA][BL_TABLE][BL_BT_ALN_COUNT]		= m_lBTAlignFrameCount;
	(*psmf)[BL_DATA][BL_TABLE][BL_BT_ALN_DELAY]		= m_lBTAlignFrameDelay;
	(*psmf)[BL_DATA][BL_TABLE][BL_BUT_ALN_COUNT]	= m_lBUTAlignFrameCount;
	(*psmf)[BL_DATA][BL_TABLE][BL_BUT_ALN_DELAY]	= m_lBUTAlignFrameDelay;
	(*psmf)[BL_DATA][BL_TABLE][BL_LOAD_OFFSET_Y]	= m_lBTLoadOffset_Y;		//v3.79
	(*psmf)[BL_DATA][BL_TABLE][BL_VACUUM_DELAY]		= m_lBTVacuumDelay;
	(*psmf)[BL_DATA][BL_TABLE][BL_FRAME_UPBLOW_TIME]= m_lBT_FrameUpBlowTime;
	(*psmf)[BL_DATA][BL_TABLE][BL_VACUUM_SETTLE_DELAY]	= m_lVacSettleDelay;	//v3.67T1
	(*psmf)[BL_DATA][BL_TABLE][BL_EXARM_OFFSET_X]	= m_lBTExArmOffsetX;		//v3.74T41	
	(*psmf)[BL_DATA][BL_TABLE][BL_EXARM_OFFSET_Y]	= m_lBTExArmOffsetY;		//v3.74T41


	if ( !m_bRealignBinFrame && (pApp->GetCustomerName() == "Cree") )
	{
		m_bRealignBinFrame = TRUE;		//v4.46T27
	}

    (*psmf)[BL_DATA][BL_TABLE][BL_REALIGN_BF]       = m_bRealignBinFrame;
	(*psmf)[BL_DATA][BL_TABLE][BL_USE_VACUUM]		= m_bUseBinTableVacuum;
	(*psmf)[BL_DATA][BL_TABLE][BL_CHK_BF_ALIGNED]	= m_bCheckFrameIsAligned;
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_CHK_ELEV_COVER]	= m_bCheckElevatorCover;		//v3.68T1
	(*psmf)[BL_DATA][BL_ELEVATOR][BL_CHECK_BC_SAME_FAIL]	= m_bBinFrameCheckBCFail;

	//Magazine summary file
	(*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_MGZ_FILE_PATH]		= m_szMagzFullFilePath;
	(*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_MGZ_FILE_ENABLE]		= m_bEnableMagzFullFile;
	(*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_MGZ_FILE_FULL_ENABLE]	= m_bExChgFullMgznOutputFile;

	(*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_CREE_BC_FILE_PATH]	= m_szCreeBinBcSummaryPath;			//v3.75
	//(*psmf)[BL_DATA][BL_MGZN_SUMMARY_FILE][BL_ENABLE_CREE_BC_FILE]	= m_bEnableCreeBinBcSummary;	//v3.75

	//For Generate Empty Frame Text File, GenerateEmptyFrameTextFile()
	(*psmf)[BL_DATA][BL_EMPTY_FRAME_FILE][BL_EMPTY_FRAME_PATH]		= m_szEmptyFramePath;

	//Min ClearAll count
	(*psmf)[BL_DATA][BL_RUNTIME][BL_CLEAR_ALL_FRAME_STATE]		= m_bClearAllFrameState;
	(*psmf)[BL_DATA][BL_RUNTIME][BL_UNLOAD_TO_FULL]				= m_lUnloadToFullTimeSpan;	//v4.60A2

	(*psmf)[BL_DATA][BL_MISC_OPTION][BL_MIN_CLRALL_COUNT]		= m_ulMinClearAllCount;
	(*psmf)[BL_DATA][BL_MISC_OPTION][BL_CLEAR_ALL_FRAME_MODE]	= m_lClearAllFrameMode;
	(*psmf)[BL_DATA][BL_MISC_OPTION][BL_RENESAS_CONFIG_MODE]	= m_nRenesasBLConfigMode;	//v4.59A20
	

	//Always TRUE for machine newer than 2.0
	if ( (CMS896AApp::m_lHardwareConfigNo == MS896A_DDBS_WITH_RING_MASTER) || (pApp->GetCustomerName() == "Sharp") )
	{
		m_bNewZConfig =  TRUE;
	}
	(*psmf)[BL_DATA][BL_MISC_OPTION][BL_NEW_Z_CONFIG]		= m_bNewZConfig;	
	(*psmf)[BL_DATA][BL_MISC_OPTION][BL_FAST_BC_SCAN]		= m_bFastBcScanMethod;			//v2.71
	(*psmf)[BL_DATA][BL_MISC_OPTION][BL_LOAD_EMPTY_RETRY_COUNT]	= m_lLoadFrameRetryCount;	//v4.43T13

	(*psmf)[BL_DATA][BL_DUAL_DL][BL_DUAL_DL_PRELOAD_EMPTY]	= m_bDualDLPreloadEmpty;		//v4.48A15	//Freescale

	(*psmf)[BL_DATA][BL_GEN_BIN_FULL_FILE]	= m_bGenBinFullFile;

	//Exchnage arm 
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_SCAN_BARCODE_ON_GRIPPER] = m_bExArmBarcodeOnGripper;	
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_PICK_POS]		= m_lExArmPickPos;
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_PLACE_POS]		= m_lExArmPlacePos;	
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_READY_POS]		= m_lExArmReadyPos;
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_PICK_DELAY]		= m_lExArmPickDelay;
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_VACUUM_DELAY]	= m_lExArmVacuumDelay;	
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_UP_DELAY]		= m_lExArmUpDelay;				//v2.93T2
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_CURR_BLOCK]		= m_lExArmBufferBlock;
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_BUF_FRAME_BARCODE]	= m_szBufferFrameBarcode;
	(*psmf)[BL_DATA][BL_EXCHG_ARM][BL_EXARM_PRELOAD_EMPTY_ON_BUFFER] = m_bExArmPreLoadEmptyFrame;	//v2.67

	//Dual Buffer Table
	//Dual Buffer Table
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_U_BUFFER_CURR_BLOCK]		= m_clUpperGripperBuffer.GetBufferBlock();
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_L_BUFFER_CURR_BLOCK]		= m_clLowerGripperBuffer.GetBufferBlock();
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_U_BUFFER_CURR_BARCODE]		= m_clUpperGripperBuffer.GetBufferBarcode();
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_L_BUFFER_CURR_BARCODE]		= m_clLowerGripperBuffer.GetBufferBarcode();
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_DB_PRELOAD_EMPTY_ON_BUFFER] = m_bDualBufferPreLoadEmptyFrame;
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_TRANSFER_STANDALONE_TO_LINE] = m_lTransferingStandaloneToInline;
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_TRANSFER_LINE_TO_STANDALONE1] = m_lTransferingInlineToStandalone1;
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_TRANSFER_LINE_TO_STANDALONE2] = m_lTransferingInlineToStandalone2;
	(*psmf)[BL_DATA][BL_DUAL_BUFFER][BL_TRANSFER_LINE_TO_STANDALONE3] = m_lTransferingInlineToStandalone3;	

	m_clUpperGripperBuffer.WriteData(&(*psmf)[BL_DATA]["UpperGripperBuffer"]);
	m_clLowerGripperBuffer.WriteData(&(*psmf)[BL_DATA]["LowerGripperBuffer"]);

	//Check Load/Save Data
    pUtl->UpdateBLConfig();
    pUtl->CloseBLConfig();	// close config file

	(*m_psmfSRam)["BinLoader"]["UseBracode"]	= m_bUseBarcode;
	(*m_psmfSRam)["BinLoader"]["OM"]			= m_lOMRT;							//v4.11T1
	(*m_psmfSRam)["BinLoader"]["RealignBinFrame"]	= m_bRealignBinFrame;			//v4.40T5

	m_szCurrMgznDisplayName  = GetMagazineName(m_lCurrMgzn).MakeUpper();			//v4.17T1
	m_szCurrMgznDisplayName2 = GetMagazineName(m_lCurrMgzn2).MakeUpper();			//v4.17T1

    return TRUE;
}


BOOL CBinLoader::GenerateConfigData()
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


	//Gripper
	oFile.WriteString("\n[OUTPUT GRIPPER]\n");
	szLine.Format("PreUnload Position O,%d\n",		m_lPreUnloadPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Unload Position O,%d\n",			m_lUnloadPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Load Magazine Position O,%d\n",	m_lLoadMagPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Unload Magazine Position O,%d\n", m_lUnloadMagPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Ready Position O,%d\n",			m_lReadyPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Barcode Position O,%d\n",		m_lBarcodePos_X);
	oFile.WriteString(szLine);
	szLine.Format("Unload Offset O,%d\n",			m_lUnloadOffsetX);		//v3.87
	oFile.WriteString(szLine);
	szLine.Format("OBuffer Unload Position,%d\n",	m_lBufferUnloadPos_X);
	oFile.WriteString(szLine);
	
    // bin table
	oFile.WriteString("\n[OUTPUT GRIPPER / BINTABLE]\n");
	szLine.Format("Unload Position X,%d\n",			m_lBTUnloadPos_X);
	oFile.WriteString(szLine);
	szLine.Format("Unload Position Y,%d\n",			m_lBTUnloadPos_Y);
	oFile.WriteString(szLine);
	szLine.Format("BT Load Offset Y,%d\n",			m_lBTLoadOffset_Y);
	oFile.WriteString(szLine);
	szLine.Format("Barcode Position X,%d\n",		m_lBTBarcodePos_X);
	oFile.WriteString(szLine);
	szLine.Format("Barcode Position Y,%d\n",		m_lBTBarcodePos_Y);
	oFile.WriteString(szLine);
	szLine.Format("BT Load Offset Y,%d\n",			m_lBTLoadOffset_Y);
	
	//Elevator
	oFile.WriteString("\n[OUTPUT ELEVATOR]\n");
	szLine.Format("Operation Mode,%c\n",			m_lOMRT + 'A');
	oFile.WriteString(szLine);
	szLine.Format("READY Position Y,%d\n",			m_lReadyPosY);
	oFile.WriteString(szLine);
	szLine.Format("READY Position Z,%d\n",			m_lReadyLvlZ);
	oFile.WriteString(szLine);
	szLine.Format("Unload Offset Y,%d\n",			m_lUnloadOffsetY);
	oFile.WriteString(szLine);
	szLine.Format("Unload Offset Z,%d\n",			m_lUnloadOffset);
	oFile.WriteString(szLine);

	//Magazine
	oFile.WriteString("\n[OUTPUT MAGAZINE]\n");
    for(INT i=0; i<MS_BL_MGZN_NUM; i++)
    {
		szLine.Format("%s %d %s,%d\n", BL_MGZN, i+1, BL_SLOT_TOP_LVL,		m_stMgznRT[i].m_lTopLevel);
		oFile.WriteString(szLine);
        szLine.Format("%s %d %s,%d\n", BL_MGZN, i+1, BL_SLOT_MID_POS,		m_stMgznRT[i].m_lMidPosnY);
		oFile.WriteString(szLine);
        szLine.Format("%s %d %s,%.1f (mm)\n", BL_MGZN, i+1, BL_SLOT_PITCH,	m_stMgznRT[i].m_lSlotPitch * m_dZRes);
		oFile.WriteString(szLine);
        szLine.Format("%s %d %s,%d\n", BL_MGZN, i+1, BL_TOTAL_SLOT,			m_stMgznRT[i].m_lNoOfSlots);
		oFile.WriteString(szLine);
    }

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp ->GetCustomerName() == "EverVision")
	{
		oFile.WriteString("\n[Bin Loader Process Setup]\n");

		szLine.Format("Enable Barcode Scanning ON/OFF,%d\n",			m_bUseBarcode);
		oFile.WriteString(szLine);

		szLine.Format("Operation Mode Selection,%d\n",			m_lOMRT);
		oFile.WriteString(szLine);

		szLine.Format("Disable Sensor Check ON/OFF,%d\n",			m_bNoSensorCheck);
		oFile.WriteString(szLine);

		szLine.Format("Enable Frame Re-alignment ON/OFF,%d\n",			m_bRealignBinFrame);
		oFile.WriteString(szLine);

		//szLine.Format("Re-Align Delay(ms),%d\n",			m_ulSequenceMode);
		//oFile.WriteString(szLine);

		szLine.Format("Frame Align Retry Count,%d\n",			m_lBTAlignFrameCount);
		oFile.WriteString(szLine);

		szLine.Format("Frame Align Retry Delay,%d\n",			m_lBTAlignFrameDelay);
		oFile.WriteString(szLine);

		//szLine.Format("Last Die Offset(X mil),%d\n",			m_dAlignLastDieOffsetX);
		//oFile.WriteString(szLine);

		//szLine.Format("Last Die Offset(Y mil),%d\n",			m_dAlignLastDieOffsetY);
		//oFile.WriteString(szLine);

		//szLine.Format("Relative Die Offset X (FOV),%d\n",			m_dAlignLastDieRelOffsetX);
		//oFile.WriteString(szLine);

		//szLine.Format("Relative Die Offset Y (FOV),%d\n",			m_dAlignLastDieRelOffsetY);
		//oFile.WriteString(szLine);

		szLine.Format("Clear Frame Mode,%d\n",			m_lClearAllFrameMode);
		oFile.WriteString(szLine);

		//szLine.Format("Realign Method DEFAULT/1-PT,%d\n",			m_lRealignBinFrameOption);
		//oFile.WriteString(szLine);

	}

	oFile.Close();
	return TRUE;
}


BOOL CBinLoader::SaveBarcodeData(LONG lBlkNo, CString szBarcode, LONG lMgzNo, LONG lSlotNo)
{
	return SaveBLBarcodeData(lBlkNo, szBarcode, lMgzNo, lSlotNo);
}


CString CBinLoader::GetBarcodeData(LONG lBlkNo)	//v3.65
{
	return GetBLBarcodeData(lBlkNo);
}



// setup operation mode
VOID CBinLoader::SetOperationMode_A(VOID)
{
    short i=0, j=0;

    // top1
    m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[0].m_lSlotBlock[j] = 0;
        m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
    }
    // middle1
    m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[1].m_lSlotBlock[j] = j+1;
        m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // bottom1
    m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[2].m_lSlotBlock[j] = j+26;
        m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // top2
    m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_FULL;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[3].m_lSlotBlock[j] = 0;
        m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
    }
    // middle2
    m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[4].m_lSlotBlock[j] = j+51;
        m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // bottom2
    m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[5].m_lSlotBlock[j] = j+76;
        m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }

	//v4.16T5
    // 8Mag bTM1 & 2, etc	//remaining mgzn struct all unused in ModeA for MS899
	for(i=6; i<MS_BL_MGZN_NUM; i++)
	{
		m_stMgznOM[i].m_lMgznUsage = BL_SLOT_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[i].m_lSlotBlock[j] = 0;
			m_stMgznOM[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
	}
}

VOID CBinLoader::ReAssignSlotBlock_ModeA()		//v4.52A16	//Cree HZ	
{
	INT nCurrBlockNo = 0;

    // top1
    //m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;

    // middle1
    if (m_stMgznOM[1].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
	{
		for (INT j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			//m_stMgznOM[1].m_lSlotBlock[j] = j+1;
			if (m_stMgznOM[1].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
			{
				m_stMgznOM[1].m_lSlotBlock[j] = 0;
			}
			else
			{
				nCurrBlockNo++;
				m_stMgznOM[1].m_lSlotBlock[j] = nCurrBlockNo;
			}
		}
	}

    // bottom1
    if (m_stMgznOM[2].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
	{
		for (INT j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			//m_stMgznOM[2].m_lSlotBlock[j] = j+26;
			if (m_stMgznOM[2].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
			{
				m_stMgznOM[2].m_lSlotBlock[j] = 0;
			}
			else
			{
				nCurrBlockNo++;
				m_stMgznOM[2].m_lSlotBlock[j] = nCurrBlockNo;
			}
		}
	}

    // top2
    //m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_FULL;

    // middle2
    if (m_stMgznOM[4].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
	{
		for (INT j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			//m_stMgznOM[4].m_lSlotBlock[j] = j+51;
			if (m_stMgznOM[4].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
			{
				m_stMgznOM[4].m_lSlotBlock[j] = 0;
			}
			else
			{
				nCurrBlockNo++;
				m_stMgznOM[4].m_lSlotBlock[j] = nCurrBlockNo;
			}
		}
	}

    // bottom2
    if (m_stMgznOM[5].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
	{
		for (INT j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			//m_stMgznOM[5].m_lSlotBlock[j] = j+76;
			if (m_stMgznOM[5].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
			{
				m_stMgznOM[5].m_lSlotBlock[j] = 0;
			}
			else
			{
				nCurrBlockNo++;
				m_stMgznOM[5].m_lSlotBlock[j] = nCurrBlockNo;
			}
		}
	}
}

VOID CBinLoader::SetOperationMode_B(VOID)
{
    LONG i, j;

    // preset empty magazine
    for(i=0; i<3; i++)
    {
        m_stMgznOM[i].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;
        for(j=0; j<MS_BL_MGZN_SLOT; j++)
        {
            m_stMgznOM[i].m_lSlotBlock[j] = 0;
            m_stMgznOM[i].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
        }
    }

    // preset full magazine
    for(i=3; i<MS_BL_MGZN_NUM; i++)
    {
        m_stMgznOM[i].m_lMgznUsage = BL_MGZN_USAGE_FULL;
        for(j=0; j<MS_BL_MGZN_SLOT; j++)
        {
            m_stMgznOM[i].m_lSlotBlock[j] = 0;
            m_stMgznOM[i].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
        }
    }
}

VOID CBinLoader::SetOperationMode_C(VOID)
{
    short i, j;

    for(i=0; i<MS_BL_MGZN_NUM; i++)
    {
        m_stMgznOM[i].m_lMgznUsage = BL_MGZN_USAGE_MIXED;
        for(j=0; j<MS_BL_MGZN_SLOT; j++)
        {
            m_stMgznOM[i].m_lSlotBlock[j] = j/5+1+i*5;
            m_stMgznOM[i].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
        }
    }
}

VOID CBinLoader::SetOperationMode_D(VOID)
{
    short j, i;

	//v4.16T5	//New ModeD for Cree China new 145 bins request on MS899DLA

    // top1
    m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_MIXED;
    for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
    {
		if (j==0)
		{
			m_stMgznOM[0].m_lSlotBlock[0] = 0;
			m_stMgznOM[0].m_lSlotUsage[0] = BL_SLOT_USAGE_EMPTY;
		}
		else if (j==1)
		{
			m_stMgznOM[0].m_lSlotBlock[1] = 0;
			m_stMgznOM[0].m_lSlotUsage[1] = BL_SLOT_USAGE_FULL;
		}
		else	//2-24
		{
			m_stMgznOM[0].m_lSlotBlock[j] = j+146;	//146-172	//126-148
			m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
    }
    // middle1
    m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
    {
        m_stMgznOM[1].m_lSlotBlock[j] = j+1;
        m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // bottom1
    m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
    {
        m_stMgznOM[2].m_lSlotBlock[j] = j+30;
        m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }

    // top2
    m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
    {
        m_stMgznOM[3].m_lSlotBlock[j] = j+59;
        m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // middle2
    m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
    {
        m_stMgznOM[4].m_lSlotBlock[j] = j+88;
        m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // bottom2
    m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
    {
        m_stMgznOM[5].m_lSlotBlock[j] = j+117;
        m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }

    // 8Mag bTM1 & 2, etc	//unused in new ModeD for MS899
	for(i=6; i<MS_BL_MGZN_NUM; i++)
	{
		m_stMgznOM[i].m_lMgznUsage = BL_SLOT_USAGE_UNUSE;
		for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
		{
			m_stMgznOM[i].m_lSlotBlock[j] = 0;
			m_stMgznOM[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
	}
}

VOID CBinLoader::ReAssignSlotBlock_ModeD()
{
	INT nCurrBlockNo = 0;

    // middle1
    if (m_stMgznOM[1].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
	{
		for (INT j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
		{
			//m_stMgznOM[1].m_lSlotBlock[j] = j+1;
			if (m_stMgznOM[1].m_lSlotUsage[j] != BL_SLOT_USAGE_UNUSE)
			{
				nCurrBlockNo++;
				m_stMgznOM[1].m_lSlotBlock[j] = nCurrBlockNo;
			}
			else
			{
				m_stMgznOM[1].m_lSlotBlock[j] = 0;
			}
		}
	}

    // bottom1
    if (m_stMgznOM[2].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
	{
		for (INT j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
		{
			//m_stMgznOM[2].m_lSlotBlock[j] = j+30;
			if (m_stMgznOM[2].m_lSlotUsage[j] != BL_SLOT_USAGE_UNUSE)
			{
				nCurrBlockNo++;
				m_stMgznOM[2].m_lSlotBlock[j] = nCurrBlockNo;
			}
			else
			{
				m_stMgznOM[2].m_lSlotBlock[j] = 0;
			}
		}
	}

    // top2
    if (m_stMgznOM[3].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
	{
		for (INT j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
		{
			//m_stMgznOM[3].m_lSlotBlock[j] = j+59;
			if (m_stMgznOM[3].m_lSlotUsage[j] != BL_SLOT_USAGE_UNUSE)
			{
				nCurrBlockNo++;
				m_stMgznOM[3].m_lSlotBlock[j] = nCurrBlockNo;
			}
			else
			{
				m_stMgznOM[3].m_lSlotBlock[j] = 0;
			}
		}
	}

    // middle2
    if (m_stMgznOM[4].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
	{
		for (INT j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
		{
			//m_stMgznOM[4].m_lSlotBlock[j] = j+88;
			if (m_stMgznOM[4].m_lSlotUsage[j] != BL_SLOT_USAGE_UNUSE)
			{
				nCurrBlockNo++;
				m_stMgznOM[4].m_lSlotBlock[j] = nCurrBlockNo;
			}
			else
			{
				m_stMgznOM[4].m_lSlotBlock[j] = 0;
			}
		}
	}

    // bottom2
    if (m_stMgznOM[5].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
	{
		for (INT j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
		{
			//m_stMgznOM[5].m_lSlotBlock[j] = j+117;
			if (m_stMgznOM[5].m_lSlotUsage[j] != BL_SLOT_USAGE_UNUSE)
			{
				nCurrBlockNo++;
				m_stMgznOM[5].m_lSlotBlock[j] = nCurrBlockNo;
			}
			else
			{
				m_stMgznOM[5].m_lSlotBlock[j] = 0;
			}
		}
	}

    // top1
    if (m_stMgznOM[0].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
	{
		for (INT j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
		{
			if (j==0)
			{
				m_stMgznOM[0].m_lSlotBlock[0] = 0;
			}
			else if (j==1)
			{
				m_stMgznOM[0].m_lSlotBlock[1] = 0;
			}
			else	//2-24
			{
				//m_stMgznOM[0].m_lSlotBlock[j] = j+146;	//146-172
				if (m_stMgznOM[0].m_lSlotUsage[j] != BL_SLOT_USAGE_UNUSE)
				{
					nCurrBlockNo++;
					m_stMgznOM[0].m_lSlotBlock[j] = nCurrBlockNo;
				}
				else
				{
					m_stMgznOM[0].m_lSlotBlock[j] = 0;
				}
			}
		}
	}
}

VOID CBinLoader::SetOperationMode_E(VOID)
{
    short j;

    // top1
    m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_MIXED;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
		if (j == 0)
		{
			m_stMgznOM[0].m_lSlotBlock[j] = 0;
			m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
		}
		else
		{
			m_stMgznOM[0].m_lSlotBlock[j] = 0;
			m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
		}
    }
    // middle1
    m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[1].m_lSlotBlock[j] = j+1;
        m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // bottom1
    m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[2].m_lSlotBlock[j] = j+26;
        m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // top2
    m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[3].m_lSlotBlock[j] = j+51;
        m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // middle2
    m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[4].m_lSlotBlock[j] = j+76;
        m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // bottom2
    m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[5].m_lSlotBlock[j] = j+101;
        m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }

    // 8mag bottom1		//MS100 8mag config		//v3.82
    m_stMgznOM[6].m_lMgznUsage = BL_SLOT_USAGE_UNUSE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[6].m_lSlotBlock[j] = 0;
        m_stMgznOM[6].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
    }
    // 8mag bottom2		//MS100 8mag config		//v3.82
    m_stMgznOM[7].m_lMgznUsage = BL_SLOT_USAGE_UNUSE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[7].m_lSlotBlock[j] = 0;
        m_stMgznOM[7].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
    }
}

VOID CBinLoader::SetOperationMode_F(VOID)
{
	if (IsMSAutoLineMode())
	{
		return SetOperationMode_F_AutoLine();
	}

	return SetOperationMode_Standard_F();
}


VOID CBinLoader::SetOperationMode_Standard_F()
{
    short j;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( (pApp->GetCustomerName() == CTM_NICHIA) && 	//v4.40T7
		 (pApp->GetProductLine() == _T("")) )			//v4.59A34
	{
		// top1
		m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[0].m_lSlotBlock[j] = 0;
			m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
		}
		// middle1
		m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[1].m_lSlotBlock[j] = j+1;
			m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// bottom1
		m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[2].m_lSlotBlock[j] = j+26;
			m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// Mode8 bottom1
		m_stMgznOM[6].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[6].m_lSlotBlock[j] = j+51;
			m_stMgznOM[6].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}

		// top2
		m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[3].m_lSlotBlock[j] = 0;
			m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
		}
		// middle2
		m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_FULL;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[4].m_lSlotBlock[j] = 0;
			m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
		}
		// bottom2
		m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_FULL;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[5].m_lSlotBlock[j] = 0;
			m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
		}
		// Mode8 bottom2
		m_stMgznOM[7].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[7].m_lSlotBlock[j] = j+76;
			m_stMgznOM[7].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}

	}
	else
	{
		// top1
		if (IsMSAutoLineStandloneMode())
		{
			// top1 as WIP magazine under SIS-standlone machine
			m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
			for(j = 0; j < MS_BL_MGZN_SLOT; j++)
			{
				m_stMgznOM[0].m_lSlotBlock[j] = j + 26;
				m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
			}
		}
		else
		{
			// top1 as FULL magazine under standlone machine
			m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_FULL;
			for(j = 0; j < MS_BL_MGZN_SLOT; j++)
			{
				m_stMgznOM[0].m_lSlotBlock[j] = 0;
				m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
			}
		}

		// middle1
		m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[1].m_lSlotBlock[j] = j + 1;
			m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}

		// bottom1
		m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[2].m_lSlotBlock[j] = j + 51;
			m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// Mode8 bottom1
		m_stMgznOM[6].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[6].m_lSlotBlock[j] = j + 101;
			m_stMgznOM[6].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}

		// top2
		if (IsMSAutoLineStandloneMode())
		{
			// top2 as FULL Magazine under SIS-standlone
			m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_FULL;
			for(j = 0; j < MS_BL_MGZN_SLOT; j++)
			{
				m_stMgznOM[3].m_lSlotBlock[j] = 0;
				m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
			}
		}
		else
		{
			// top2 as Empty Magazine under standlone
			m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;
			for(j = 0; j < MS_BL_MGZN_SLOT; j++)
			{
				m_stMgznOM[3].m_lSlotBlock[j] = 0;
				m_stMgznOM[3].m_lSlotUsage[j] = IsMSAutoLineMode() ? BL_SLOT_USAGE_UNUSE : BL_SLOT_USAGE_EMPTY;
			}
		}

		// middle2
		if (IsMSAutoLineStandloneMode())
		{
			// middle2 as EMPTY Magazine under SIS-standlone
			m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;
			for(j = 0; j < MS_BL_MGZN_SLOT; j++)
			{
				m_stMgznOM[4].m_lSlotBlock[j] = 0;
				m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
			}
		}
		else
		{
			// middle2 as WIP Magazine under standlone
			m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
			for(j = 0; j < MS_BL_MGZN_SLOT; j++)
			{
				m_stMgznOM[4].m_lSlotBlock[j] = j + 26;
				m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
			}
		}
		// bottom2
		m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[5].m_lSlotBlock[j] = j + 76;
			m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// Mode8 bottom2
		m_stMgznOM[7].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[7].m_lSlotBlock[j] = j + 126;
			m_stMgznOM[7].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
	}
}


VOID CBinLoader::SetMgznOMSlotStatus(const LONG lMgzn, const LONG lSlot, const LONG lSlotGradeBlock,
									 const LONG lSlotWIPCounter, const LONG lSlotUsageStatus,
									 const CString szSlotBCName)
{
	m_stMgznOM[lMgzn].m_lSlotBlock[lSlot] = lSlotGradeBlock;
	m_stMgznOM[lMgzn].m_lSlotUsage[lSlot] = lSlotUsageStatus;
//	m_stMgznOM[lMgzn].m_SlotBCName[lSlot] = szSlotBCName;
//	m_stMgznOM[lMgzn].m_lSlotWIPCounter[lSlot] = lSlotWIPCounter;
//	m_stMgznOM[lMgzn].m_lTransferSlotUsage[lSlot] = BL_SLOT_USAGE_UNUSE;
}


VOID CBinLoader::SetOperationMode_F_AutoLine(VOID)
{
    short j;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	// top1
	m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for(j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetMgznOMSlotStatus(0, j, 0, 0, BL_SLOT_USAGE_EMPTY, "");
	}

	// middle1		//The only mag available for LOAD/UNLOAD
	m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for(j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetMgznOMSlotStatus(1, j, j + 1, 0, BL_SLOT_USAGE_EMPTY, "");
	}

	// bottom1
	m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for(j=0; j<MS_BL_MGZN_SLOT; j++)
	{
		SetMgznOMSlotStatus(2, j, j + 51, 0, BL_SLOT_USAGE_EMPTY, "");
	}
	// Mode8 bottom1
	m_stMgznOM[6].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for(j=0; j<MS_BL_MGZN_SLOT; j++)
	{
		SetMgznOMSlotStatus(6, j, j + 101, 0, BL_SLOT_USAGE_EMPTY, "");
	}

	// top2 --- not used
	m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
	for(j=0; j<MS_BL_MGZN_SLOT; j++)
	{
		SetMgznOMSlotStatus(3, j, 0, 0, BL_SLOT_USAGE_UNUSE, "");
	}
	// middle2
	m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetMgznOMSlotStatus(4, j, j + 26, 0, BL_SLOT_USAGE_EMPTY, "");
	}

	// bottom2
	m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetMgznOMSlotStatus(5, j, j + 76, 0, BL_SLOT_USAGE_EMPTY, "");
	}
	// Mode8 bottom2
	m_stMgznOM[7].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetMgznOMSlotStatus(7, j, j + 126, 0, BL_SLOT_USAGE_EMPTY, "");
	}
}

VOID CBinLoader::SetOperationMode_F_Renesas(CONST UINT nMode)	//v4.56A18
{
	INT j = 0;

	if (nMode == 1)
	{
		m_nRenesasBLConfigMode_Temp = 1;

		// top1
		m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[0].m_lSlotBlock[j] = 0;
			m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
		}
		// middle1		//This mgzn is for all Grade 1 frames
		m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_FULL;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[1].m_lSlotBlock[j] = 0;
			m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
		}
		// bottom1		//This mgzn is for all Grade 2 frames
		m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_FULL;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[2].m_lSlotBlock[j] = 0;
			m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
		}
		// Mode8 bottom1
		m_stMgznOM[6].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[6].m_lSlotBlock[j] = j+1;
			m_stMgznOM[6].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;
		}


		// top2
		m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[3].m_lSlotBlock[j] = 0;
			m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
		// middle2
		m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[4].m_lSlotBlock[j] = 0;
			m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
		// bottom2
		m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[5].m_lSlotBlock[j] = 0;
			m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
		// Mode8 bottom2
		m_stMgznOM[7].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[7].m_lSlotBlock[j] = 0;
			m_stMgznOM[7].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
	}
	else if (nMode == 2)
	{
		m_nRenesasBLConfigMode_Temp = 2;

		// top1
		m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[0].m_lSlotBlock[j] = 0;
			m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
		// middle1
		m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[1].m_lSlotBlock[j] = 0;
			m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
		// bottom1
		m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[2].m_lSlotBlock[j] = 0;
			m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
		// Mode8 bottom1
		m_stMgznOM[6].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[6].m_lSlotBlock[j] = 0;
			m_stMgznOM[6].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}


		// top2
		m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[3].m_lSlotBlock[j] = 0;
			m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
		}
		// middle2		//This mgzn is for all Grade 1 frames
		m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_FULL;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[4].m_lSlotBlock[j] = 0;
			m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
		}
		// bottom2		//This mgzn is for all Grade 2 frames
		m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_FULL;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[5].m_lSlotBlock[j] = 0;
			m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
		}
		// Mode8 bottom2
		m_stMgznOM[7].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[7].m_lSlotBlock[j] = j+1;
			m_stMgznOM[7].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;
		}
	}
	else	// = 0
	{
		m_nRenesasBLConfigMode_Temp = 0;
		SetOperationMode_F();
	}
}

VOID CBinLoader::SetOperationMode_G(VOID)
{
    short j;

	if (m_bUseBLAsLoaderZ == TRUE)		//v4.36T2	//Yealy MS100Plus new ModeG support
	{
		// top1
		m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_MIXED;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			if (j == 0)
			{
				m_stMgznOM[0].m_lSlotBlock[j] = 0;
				m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
			}
			else
			{
				m_stMgznOM[0].m_lSlotBlock[j] = 0;
				m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
			}
		}

		// middle1
		m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[1].m_lSlotBlock[j] = j+1;
			m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// bottom1
		m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[2].m_lSlotBlock[j] = j+26;
			m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// Mode8 bottom1
		m_stMgznOM[6].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[6].m_lSlotBlock[j] = j+51;
			m_stMgznOM[6].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}

		// top2
		m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for (j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[3].m_lSlotBlock[j] = 0;
			m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
		// middle2
		m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[4].m_lSlotBlock[j] = 0;
			m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
		// bottom2
		m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[5].m_lSlotBlock[j] = 0;
			m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
		// Mode8 bottom2
		m_stMgznOM[7].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[7].m_lSlotBlock[j] = 0;
			m_stMgznOM[7].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
		}
	}
	else
	{
		// top1
		m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[0].m_lSlotBlock[j] = j+1;
			m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// middle1
		m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[1].m_lSlotBlock[j] = j+51;
			m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// bottom1
		m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[2].m_lSlotBlock[j] = j+101;
			m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// Mode8 bottom1
		m_stMgznOM[6].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[6].m_lSlotBlock[j] = j+151;
			m_stMgznOM[6].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}

		// top2
		m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_MIXED;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			if (j == 0)
			{
				m_stMgznOM[3].m_lSlotBlock[j] = 0;
				m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
			}
			else
			{
				m_stMgznOM[3].m_lSlotBlock[j] = 0;
				m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
			}
		}
		// middle2
		m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[4].m_lSlotBlock[j] = j+26;
			m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// bottom2
		m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[5].m_lSlotBlock[j] = j+76;
			m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
		// Mode8 bottom2
		m_stMgznOM[7].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
		for(j=0; j<MS_BL_MGZN_SLOT; j++)
		{
			m_stMgznOM[7].m_lSlotBlock[j] = j+126;
			m_stMgznOM[7].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
		}
	}
}

VOID CBinLoader::SetOperationMode_H(VOID)
{
    short j;

    // top1
    m_stMgznOM[0].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[0].m_lSlotBlock[j] = 1;	//j+1;	//v4.44T1
        m_stMgznOM[0].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // middle1
    m_stMgznOM[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[1].m_lSlotBlock[j] = 1;	//j+26;	//v4.44T1
        m_stMgznOM[1].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // bottom1
    m_stMgznOM[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[2].m_lSlotBlock[j] = 1;	//j+51;	//v4.44T1
        m_stMgznOM[2].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }
    // Mode8 bottom1
    m_stMgznOM[6].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[6].m_lSlotBlock[j] = 1;	//j+76;	//v4.44T1
        m_stMgznOM[6].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE;
    }

    // top2
    m_stMgznOM[3].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
    for (j=0; j<MS_BL_MGZN_SLOT; j++)
    {
		m_stMgznOM[3].m_lSlotBlock[j] = 0;
		m_stMgznOM[3].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
    }
    // middle2
    m_stMgznOM[4].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[4].m_lSlotBlock[j] = 0;
        m_stMgznOM[4].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
    }
    // bottom2
    m_stMgznOM[5].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[5].m_lSlotBlock[j] = 0;
        m_stMgznOM[5].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
    }
    // Mode8 bottom2
    m_stMgznOM[7].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
    for(j=0; j<MS_BL_MGZN_SLOT; j++)
    {
        m_stMgznOM[7].m_lSlotBlock[j] = 0;
        m_stMgznOM[7].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
    }
}

LONG CBinLoader::BL_DEBUGBOX(CString str)
{
	CMSLogFileUtility::Instance()->BL_LogStatus(str);

    if( m_bStepMode==TRUE )
        return HmiMessage(str); // debug display
    else 
        return FALSE;
}

LONG CBinLoader::BL_DEBUGMESSAGE(CString str)
{
	if (m_bShowDebugMessage == TRUE)
	{
		DisplayMessage(str);
	}

	return TRUE;
}

//LONG CBinLoader::BL_WARNBOX(CString str)
//{
//    return HmiMessage(str, "Bin Loader"); // warn display, maybe with lighting tower
//}
//
//LONG CBinLoader::BL_WARNBOX(CString str, LONG l)
//{
//    str.AppendFormat(" %d", l);
//	return BL_WARNBOX(str);
//}

LONG CBinLoader::BL_YES_NO(CString strText, CString strTitle)
{
	SetAlertTime(TRUE, EQUIP_ASSIST_TIME, strTitle);

	if( HmiMessage_Red_Back(strText, strTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL)!=glHMI_YES )
	{
		//SetAlarmLamp_Yellow();
		SetAlertTime(FALSE, EQUIP_ASSIST_TIME, strTitle);
		return FALSE;
	}
    else
	{
		SetAlertTime(FALSE, EQUIP_ASSIST_TIME, strTitle);
		return TRUE;
	}
}


LONG CBinLoader::BL_YES_NO(ULONG ulAppMsgCode, ULONG ulHmiMsgCode, BOOL bDiableOkButton, BOOL bNoMaterial)
{
	LONG lReturn =0;
	CString szContent = " ";

	szContent.LoadString(ulAppMsgCode);

	if (bDiableOkButton == FALSE)
	{
		if (bNoMaterial)	//v4.59A16	//Renesas MS90
			lReturn = SetAlert_Msg_NoMaterial(ulHmiMsgCode, szContent, "Yes", "No");
		else
			lReturn = SetAlert_Msg_Red_Back(ulHmiMsgCode, szContent, "Yes", "No");
	}
	else
	{
		if (bNoMaterial)	//v4.59A16	//Renesas MS90
			lReturn = SetAlert_Msg_NoMaterial(ulHmiMsgCode, szContent, "No");
		else
			lReturn = SetAlert_Msg_Red_Back(ulHmiMsgCode, szContent, "No");
	}


	if ( lReturn != 1 )
	{
		//SetAlarmLamp_Yellow();
		return FALSE;
	}
    else
	{
		CMSLogFileUtility::Instance()->BL_LogStatus("Comment --- Alarm pass Set Alert Yellow Green"); //4.52M84
		//SetAlert_Yellow_Green(ulHmiMsgCode);// 4.51D8 BL0052 
		return TRUE;
	}
}


LONG CBinLoader::BL_OK_CANCEL(CString strText, CString strTitle)
{
	SetAlertTime(TRUE, EQUIP_ASSIST_TIME, strTitle);

	EquipStateProductiveToUnscheduleDown();		//v4.48A30
	
	LONG lStatus = HmiMessage_Red_Back(strText, strTitle, glHMI_MBX_OKCANCEL|0x80000000,	//v3.98T5
		glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	
	EquipStateUnscheduleDwonToProductive();		//v4.48A30

	if (lStatus != glHMI_OK)
	{
		//SetAlarmLamp_Yellow();
		SetAlertTime(FALSE, EQUIP_ASSIST_TIME, strTitle);
		return FALSE;
	}
    else
	{
		SetAlertTime(FALSE, EQUIP_ASSIST_TIME, strTitle);
		return TRUE;
	}
}


LONG CBinLoader::BL_OK_CANCEL(ULONG ulAppMsgCode, ULONG ulHmiMsgCode, BOOL bDiableOkButton)
{
	LONG lReturn =0;
	CString szContent = " ";
	szContent.LoadString(ulAppMsgCode);

	EquipStateProductiveToUnscheduleDown();		//v4.48A30

	if (bDiableOkButton == FALSE)
	{
		lReturn = SetAlert_Msg_Red_Back(ulHmiMsgCode, szContent, "Ok", "Cancel");
	}
	else if (bDiableOkButton == 2)
	{
		lReturn = SetAlert_Msg_Red_Back(ulHmiMsgCode, szContent, "OK");
	}
	else
	{
		lReturn = SetAlert_Msg_Red_Back(ulHmiMsgCode, szContent, "Cancel");
	}

	EquipStateUnscheduleDwonToProductive();		//v4.48A30

	if ( lReturn != 1 )
	{
		//SetAlarmLamp_Yellow();
		return FALSE;
	}
    else
	{
		return TRUE;
	}
}


VOID CBinLoader::GenerateOMSPTableFile(ULONG ulMgzn)
{
    int i, j;
    CString str;
	CStdioFile fTemp;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\bl_omsetup.csv"), CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText))
	{
		return;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bUse29Slots = pApp->GetFeatureStatus(MS896A_FUNC_BL_USE_29_MAGSLOTS);

	if ( bUse29Slots && (m_lOMSP == BL_MODE_D) )		//Mode D for Cree HuiZhou	//v4.19
	{
		for(j=0; j<15; j++)			//v4.19		//Cree HuiZhou
		{
			i = j;
			// slot id
			str.Format("%d,", i+1);
			fTemp.WriteString(str);
			// usage
			str.Format( "%s,", GetOMSlotUsage(ulMgzn, i) );
			fTemp.WriteString(str);
			// physical block
			str.Format("%d,", m_stMgznOM[ulMgzn].m_lSlotBlock[i]);
			fTemp.WriteString(str);
			// space for separator
			fTemp.WriteString(",");
	 
			i = j+15;
			if( i==MS_BL_MAX_MGZN_SLOT )
			{
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString("\n");
				break;
			}
			// slot
			str.Format("%d,", i+1);
			fTemp.WriteString(str);
			// usage
			str.Format("%s,", GetOMSlotUsage(ulMgzn, i) );
			fTemp.WriteString(str);
			// physical
			str.Format("%d,", m_stMgznOM[ulMgzn].m_lSlotBlock[i]);
			fTemp.WriteString(str);
		}
	}
	else
	{
		for(j=0; j<13; j++)
		{
			i = j;
			// slot id
			str.Format("%d,", i+1);
			fTemp.WriteString(str);
			// usage
			str.Format( "%s,", GetOMSlotUsage(ulMgzn, i) );
			fTemp.WriteString(str);
			// physical block
			str.Format("%d,", m_stMgznOM[ulMgzn].m_lSlotBlock[i]);
			fTemp.WriteString(str);
			// space for separator
			fTemp.WriteString(",");
	 
			i = j+13;
			if( i==MS_BL_MGZN_SLOT )
			{
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString("\n");
				break;
			}
			// slot
			str.Format("%d,", i+1);
			fTemp.WriteString(str);
			// usage
			str.Format("%s,", GetOMSlotUsage(ulMgzn, i) );
			fTemp.WriteString(str);
			// physical
			str.Format("%d,", m_stMgznOM[ulMgzn].m_lSlotBlock[i]);
			fTemp.WriteString(str);
		}
	}

    fTemp.Close();
}

VOID CBinLoader::GenerateOMRTTableFile(ULONG ulMgzn)
{
    int i, j;
    CString str;
	CStdioFile fTemp;

	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\bl_ominform.csv"), CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText))
	{
		return;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bUse29Slots = pApp->GetFeatureStatus(MS896A_FUNC_BL_USE_29_MAGSLOTS);

	if ( bUse29Slots && (m_lOMRT == BL_MODE_D) )		//Mode D for Cree HuiZhou	//v4.19
	{
		for(j=0; j<15; j++)
		{
			i = j;
			// slot id
			str.Format("%d,", i+1); 
			fTemp.WriteString(str);
			// usage
			str.Format("%s,", GetRTSlotUsage(ulMgzn, i));
			fTemp.WriteString(str);
			// physical block
			str.Format("%d,", m_stMgznRT[ulMgzn].m_lSlotBlock[i]);
			fTemp.WriteString(str);
			// bar code
			str.Format("%s,", m_stMgznRT[ulMgzn].m_SlotBCName[i]);
			fTemp.WriteString(str);
			// wip counter
			str.Format("%d,", m_stMgznRT[ulMgzn].m_lSlotWIPCounter[i]);
			fTemp.WriteString(str);
			// space for separator
			fTemp.WriteString(",");
	 
			i = j+15;
			if( i==MS_BL_MAX_MGZN_SLOT )
			{
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString("\n");
				break;
			}
			// slot
			str.Format("%d,", i+1);
			fTemp.WriteString(str);
			// usage
			str.Format("%s,", GetRTSlotUsage(ulMgzn, i));
			fTemp.WriteString(str);
			// physical block
			str.Format("%d,", m_stMgznRT[ulMgzn].m_lSlotBlock[i]);
			fTemp.WriteString(str);
			// bar code
			str.Format("%s,", m_stMgznRT[ulMgzn].m_SlotBCName[i]);
			fTemp.WriteString(str);
			// wip counter
			str.Format("%d,", m_stMgznRT[ulMgzn].m_lSlotWIPCounter[i]);
			fTemp.WriteString(str);
		}
	}
	else
	{
		for(j=0; j<13; j++)
		{
			i = j;
			// slot id
			str.Format("%d,", i+1); 
			fTemp.WriteString(str);
			// usage
			str.Format("%s,", GetRTSlotUsage(ulMgzn, i));
			fTemp.WriteString(str);
			// physical block
			str.Format("%d,", m_stMgznRT[ulMgzn].m_lSlotBlock[i]);
			fTemp.WriteString(str);
			// bar code
			str.Format("%s,", m_stMgznRT[ulMgzn].m_SlotBCName[i]);
			fTemp.WriteString(str);
			// wip counter
			str.Format("%d,", m_stMgznRT[ulMgzn].m_lSlotWIPCounter[i]);
			fTemp.WriteString(str);
			// space for separator
			fTemp.WriteString(",");
	 
			i = j+13;
			if( i==MS_BL_MGZN_SLOT )
			{
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString(",");
				fTemp.WriteString("\n");
				break;
			}
			// slot
			str.Format("%d,", i+1);
			fTemp.WriteString(str);
			// usage
			str.Format("%s,", GetRTSlotUsage(ulMgzn, i));
			fTemp.WriteString(str);
			// physical block
			str.Format("%d,", m_stMgznRT[ulMgzn].m_lSlotBlock[i]);
			fTemp.WriteString(str);
			// bar code
			str.Format("%s,", m_stMgznRT[ulMgzn].m_SlotBCName[i]);
			fTemp.WriteString(str);
			// wip counter
			str.Format("%d,", m_stMgznRT[ulMgzn].m_lSlotWIPCounter[i]);
			fTemp.WriteString(str);
		}
    }

    fTemp.Close();
}


VOID CBinLoader::FlushMessage()
{
	if (m_comServer.ScanRequest(10))
		m_comServer.ReadRequest();
}

VOID CBinLoader::UpdateStationData()
{
	//AfxMessageBox("CBinLoader", MB_SYSTEMMODAL);
	m_oBinLoaderDataBlock.m_szEnableBinTableVacuum = m_oBinLoaderDataBlock.ConvertBoolToOnOff(m_bUseBinTableVacuum);
	m_oBinLoaderDataBlock.m_szBinVacuumDelay.Format("%d", m_lBTVacuumDelay);
	m_oBinLoaderDataBlock.m_szBinVacuumSettleDelay.Format("%d", m_lVacSettleDelay);
	m_oBinLoaderDataBlock.m_szFrameReAlignDelay.Format("%d", m_lBTAlignFrameDelay);
	m_oBinLoaderDataBlock.m_szFrameReAlignRetryCount.Format("%d",m_lBTAlignFrameCount);
	m_oBinLoaderDataBlock.m_szEnableFrameRealign = m_oBinLoaderDataBlock.ConvertBoolToOnOff(m_bRealignBinFrame);
	m_oBinLoaderDataBlock.m_szCheckElevatorCover = m_oBinLoaderDataBlock.ConvertBoolToOnOff(m_bCheckElevatorCover);
	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SetBinLoaderDatatBlock(m_oBinLoaderDataBlock);
}

BOOL CBinLoader::LogFrameFullInformation(BOOL bLogFile, LONG lMagNo, LONG lSlot, CString szBarcodeName)
{
	int nYear, nMonth, nDay, nHour, nMinute, nSecond;
	CStdioFile cfDataFile;
	BOOL bFileOpen = FALSE;
	BOOL bFileExist = FALSE;
	CString szOutputFileName;
	CString szFileName = "";
	CString szYear, szMonth, szDay, szHour, szMinute, szSecond;
	CString szDateFormat, szText;
	CString szData1, szData2, szWaferId; // szWaferId for second last die
		//Get current time;
	CTime theTime = CTime::GetCurrentTime();

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


	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//Get info from other stn
	UCHAR ucGrade			=	(*m_psmfSRam)["BinTable"]["ClearBin"]["Grade"];
	ULONG ulBonded			=	(*m_psmfSRam)["BinTable"]["ClearBin"]["Bonded"];
	ULONG ulSNRNo			=	(*m_psmfSRam)["BinTable"]["ClearBin"]["SNR No"];
	CString szBinFullCount  =	(*m_psmfSRam)["BinTable"]["ClearBin"]["Bin Full Count"];
	

	//Get output file name from file
	szOutputFileName.IsEmpty();
	szFileName = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\CurrentName.txt";

	if (cfDataFile.Open(szFileName, CFile::modeRead|CFile::shareExclusive|CFile::typeText) == TRUE)
	{
		try
		{
			cfDataFile.ReadString(szText);				//Title = [File]
			cfDataFile.ReadString(szOutputFileName);	//Content

			// Second Last Die Wafer ID For SanAn
			// Reading Wafer IDs and their Die info from CurrentName.txt
			if (CMS896AStn::m_bAddWaferIdInMsgSummaryFile)
			{
				CStringArray szaWaferId;
				INT nNoOfWaferId=0;
				cfDataFile.ReadString(szText); // Read Header [Start of Wafer Id Info]
				cfDataFile.ReadString(szText); // Read Header [No of Wafer Id]
				cfDataFile.ReadString(szText); // Read No of Wafer Id
				nNoOfWaferId = atoi(szText);
				if ((nNoOfWaferId < 0) || (nNoOfWaferId > 10000))		//Klocwork
					nNoOfWaferId = 0;
				INT* naDieNo = new INT[nNoOfWaferId];

				for (int j=0; j<nNoOfWaferId; j++)
				{
					cfDataFile.ReadString(szText);	// Read Header [Wafer Id]
					cfDataFile.ReadString(szText); // Read Wafer Id
					szaWaferId.Add(szText);
					cfDataFile.ReadString(szText);	// Readr No of Die
					naDieNo[j] = atoi(szText);
				}

				szWaferId = szaWaferId.GetAt(nNoOfWaferId-1);

				if (naDieNo[nNoOfWaferId-1] < 2)
				{
					if (nNoOfWaferId >= 2)
						szWaferId = szaWaferId.GetAt(nNoOfWaferId-2);
				}

				delete [] naDieNo;		//Klocwork
			}
		}
		catch(...)
		{
		}

		cfDataFile.Close();
	}
	szFileName = "";


	//Fit ouptut format
	szDateFormat = szHour + ":" + szMinute + ":" + szSecond + "@" + szDay + "-" + szMonth + "-" + szYear;
	
	//Fit data format 
	szData1.Format("%2d, ", lSlot);  
	szData2.Format("%3d, %3d, %5d", ucGrade, ulSNRNo, ulBonded);  
	
	if ( szOutputFileName.IsEmpty() == FALSE )
	{
		szData2 = szData2 + ", " + szOutputFileName;
	}


	//v4.20		//JDSU buy-off request
	if ( (pApp->GetCustomerName() == "JDSU") && !m_bUseBarcode )
	{
		CString szCurrWaferID;
		if (m_WaferMapWrapper.IsMapValid())
			m_WaferMapWrapper.GetWaferID(szCurrWaferID);
		else
			szCurrWaferID = szWaferId;
		CString szBC;
		szBC.Format("_%d_%d", ucGrade, ulSNRNo);
		szBarcodeName = szCurrWaferID + szBC;		//Use wafer ID as BC if BC option is disabled
	}


	//1. Log date into History file 
	if (bLogFile == TRUE)
	{
		szFileName = BL_HISTORY_NAME;

		if ((_access(szFileName, 0 )) == -1)
		{
			bFileExist = FALSE;
		}
		else
		{
			bFileExist = TRUE;
		}

		//Get Title & list out data
		bFileOpen = cfDataFile.Open(szFileName, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
		
		if ( bFileOpen == TRUE )
		{
			cfDataFile.SeekToEnd();

			if ( bFileExist == FALSE)
			{
				cfDataFile.WriteString(BL_HISTORY_TITLE);
				cfDataFile.WriteString("\n");
			}

			//Format (Date/Time, MagName, SlotNo, BarcodeName, Grade, SNR No, Bonded, OutputFileName(If avaliable))
			szText = szDateFormat + ", " + GetMagazineName(lMagNo).MakeUpper() + ", " + szData1 + szBarcodeName + ", " + szData2 + "\n";

			cfDataFile.WriteString(szText);
			cfDataFile.Close();
		}
	}


	//2. Log data into Temp. Magazine file
	szFileName.Format("%d.txt", lMagNo);
	szFileName = BL_TEMP_MAG_NAME + szFileName;

	if ((_access(szFileName, 0)) == -1)
	{
		bFileExist = FALSE;
	}
	else
	{
		bFileExist = TRUE;
	}


	bFileOpen = cfDataFile.Open(szFileName, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
	
	if ( bFileOpen == TRUE )
	{
		cfDataFile.SeekToEnd();

		if ( bFileExist == FALSE)
		{
			if (CMS896AStn::m_bAddWaferIdInMsgSummaryFile == FALSE)
			{
				cfDataFile.WriteString(BL_SUMMARY_TITLE);
			}
			else
			{
				if (pApp->GetCustomerName() == CTM_SANAN && pApp ->GetProductLine() == "XA")
					cfDataFile.WriteString(BL_SUMMARY_TITLE3);
				else
					cfDataFile.WriteString(BL_SUMMARY_TITLE2);
			}

			cfDataFile.WriteString("\n");
		}
		
		if (CMS896AStn::m_bAddWaferIdInMsgSummaryFile == FALSE)
		{
			szText = szDateFormat + ", " + szData1 + szBarcodeName + ", " + szData2 + "\n";
		}
		else
		{
			// Wafer ID for SanAn
			if (pApp->GetCustomerName() == CTM_SANAN && pApp ->GetProductLine() == "XA")
				szText = szDateFormat + ", " + szData1 + szBarcodeName + ", " + szData2 +"," + szWaferId + "," + szBinFullCount + "\n";
			else
				szText = szDateFormat + ", " + szData1 + szBarcodeName + ", " + szData2 +"," + szWaferId +"\n";
		}

		cfDataFile.WriteString(szText);
		cfDataFile.Close();
	}


	return TRUE;
}


BOOL CBinLoader::OutputMagzFullSummaryFile(LONG lMagNo, BOOL bKeepFile)
{
	CString szFileName, szTargetFileName, szMagName;
	CString szYear, szMonth, szDay, szHour, szMinute, szSecond;
	CString szMachineNo;
	int nYear, nMonth, nDay, nHour, nMinute, nSecond;


	//Get current time
	CTime theTime = CTime::GetCurrentTime();

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


	//Get machine no
	szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];

	szFileName.Format("%d.txt", lMagNo);
	szFileName = BL_TEMP_MAG_NAME + szFileName;

	//Get Magazine Name
	szMagName = GetMagazineName(lMagNo).MakeUpper();
	szMagName.Remove(' ');

	//File format = MachineNo + Magazine Name + Time (DD-MM-YY@HH:MM:SS)
	szTargetFileName = m_szMagzFullFilePath + "\\" + 
						"M" + szMachineNo + "_" + 
						szMagName + "_" +
						szDay + szMonth + szYear + "_" + 
						szHour + szMinute + szSecond + ".CSV";

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if(pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		CString szGetContent = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\GetContent.txt";
		if (_access(szGetContent,0) == -1)
		{
			HmiMessage("No Bin Information, please Clear Bin First. [ChangeLight(XM)]");
			return TRUE;
		}
		//Time
		CString szXMYear;
		szXMYear.Format("%d", theTime.GetYear());
		szXMYear = szXMYear.Right(2);

		CString szXMMonth;
		szXMMonth.Format("%d", theTime.GetMonth());
		if (szXMMonth == "10")
			szXMMonth = "A";
		if (szXMMonth == "11")
			szXMMonth = "B";
		if (szXMMonth == "12")
			szXMMonth = "C";
		
		CString szXMDay;
		szXMDay.Format("%d", theTime.GetDay());
		if ( theTime.GetDay() < 10 )
		{
			szXMDay = "0" + szXMDay;
		}

		//SerialNo
		int nSerial;
		CString szSerial;
		BOOL bXMOpen;
		CStdioFile cfSerial;
		CString szGetSerialNo = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\GetSerialNo.txt";
		if (_access(szGetSerialNo,0) != -1)
		{
			bXMOpen = cfSerial.Open(szGetSerialNo, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if (bXMOpen)
			{
				cfSerial.ReadString(szSerial);
				nSerial = atoi((LPCTSTR)szSerial);
				if (nSerial > 998)
				{

					szSerial = "001";
					cfSerial.Close();
					remove(szGetSerialNo);
				}
				else
				{
					nSerial ++;
					szSerial.Format("%d",nSerial);
					cfSerial.SeekToBegin();
					cfSerial.WriteString(szSerial);
					if( szSerial.GetLength()<2)
						szSerial = "00" + szSerial;
					if( szSerial.GetLength()<3)
						szSerial = "0"  + szSerial;
					cfSerial.Close();
				}
			}
		}
		else
		{
			bXMOpen = cfSerial.Open(szGetSerialNo, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if (bXMOpen)
			{
				cfSerial.WriteString("1");
			}
			szSerial = "001";
			cfSerial.Close();
		}

		//MachineNo
		CString szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
		szMachineNo = szMachineNo.Right(3);
		CString szXMSummaryName = "XSor" +szMachineNo + "C" + szSerial + szXMYear + szXMMonth + szXMDay + szHour + szMinute;
		CString szXMPath        = m_szMagzFullFilePath + "\\" + szXMSummaryName + ".txt";
		CString szBackUpPath    = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\MagSummary";
		CString szBackUpPathName = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\MagSummary\\"  + szXMSummaryName + ".txt";
		
		CreateDirectory(szBackUpPath, NULL);
		CStdioFile cfXM;
		CStdioFile cfRead;


		bXMOpen = cfXM.Open(szXMPath, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
		if (bXMOpen)
		{
			cfXM.WriteString("CASSETENAME," + szXMSummaryName + "\n");
			CString szOutTime = theTime.Format("%Y/%m/%d %H:%M");
			cfXM.WriteString("OUTTIME," + szOutTime + "\n");
			cfXM.WriteString("\n");
			cfXM.WriteString("No,SN,Barcode,LotNo\n");

			bXMOpen = cfRead.Open(szGetContent,CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if(bXMOpen)
			{
				BOOL bRead;
				CString szContent;
				do
				{	
					bRead = cfRead.ReadString(szContent);
					cfXM.SeekToEnd();
					cfXM.WriteString(szContent);
					cfXM.WriteString("\n");
				}
				while (bRead);
				cfRead.Close();
				remove(szGetContent);
			}
			cfXM.Close();
			CopyFile(szXMPath,szBackUpPathName,FALSE);
		}
		return TRUE;
	}


	if ((_access(szFileName, 0)) == -1)
	{
		return FALSE;
	}
	else
	{
		if ( CopyFile(szFileName, szTargetFileName, FALSE) == TRUE )
		{
			if ( bKeepFile == FALSE )
			{
				DeleteFile(szFileName);
			}
		}
	}

	return TRUE;
}


VOID CBinLoader::HouseKeeping(LONG lSafePos, BOOL bEnableBT, BOOL bIsMissingStep, 
							  BOOL bByPassOutOfMagCheck, BOOL bCheckBTframe)
{
/*
	SetGripperState(FALSE);
	Sleep(200);
	SetGripperLevel(FALSE);
	Sleep(200);
	//BinGripperMoveTo(m_lPreUnloadPos_X);
	BinGripperMoveTo(lSafePos);
	Sleep(200);
*/
//v4.52A6
CString szLog;
szLog.Format("BL HouseKeeping: EnableBT = %d, bCheckBTframe = %d", 
				bEnableBT, bCheckBTframe);
BL_DEBUGBOX(szLog);


	LONG lRetryLimit = 0;

	//1st down bintable platform for safe
	if ( bEnableBT == TRUE )
	{
szLog.Format("BL HouseKeeping: EnableBT, CheckFrameSnr = %d, NoSnrChk = %d, SnrStatus = %d", 
	bCheckBTframe, m_bNoSensorCheck, IsFrameProtection());
BL_DEBUGBOX(szLog);

		SetFrameVacuum(FALSE);
		SetFrameAlign(FALSE);
		Sleep(m_lBTAlignFrameDelay);

		//v4.52A3	//Fabrinet Thailand
		if (bCheckBTframe)
		{
			INT nCount = 0;
			while (!m_bNoSensorCheck && !IsFrameProtection())
			{
				CString szContent;
				szContent.LoadString(HMB_BL_REMOVE_BT_FRAME);	
				HmiMessage_Red_Yellow(szContent);
	
szLog.Format("BL HouseKeeping - Check FrameProtect Snr fail #%d",  nCount+1);
BL_DEBUGBOX(szLog);

				nCount++;
				if (nCount > 3)
				{
					HmiMessage("Warning: Bin Table level will be DONW disregard of protection sensor ...");
szLog.Format("BL HouseKeeping - Check FrameProtect Snr ABORT #%d",  nCount+1);
BL_DEBUGBOX(szLog);
					break;
				}
			}
		}
		SetFrameLevel(FALSE);
	}
	

	if (bIsMissingStep == TRUE)
	{
		if (!m_bNoSensorCheck && IsFrameInClamp())		//v3.84		//IsFrameInClamp() always return TRUE when m_bNoSensorCheck=TRUE
		{
			do
			{
				SetGripperState(FALSE);		//Off gripper
				SetAlert_Red_Yellow(IDS_BL_FRMAE_DETECTED_IN_BL_ERR_HANDLE);
				lRetryLimit = lRetryLimit + 1;

				if (lRetryLimit == 3)
				{
					return;
				}

			}while(IsFrameInClamp());

		}

		SetGripperState(FALSE);				//1. Off gripper
		Sleep(200);

		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
		{
			//xyz
			SetBufferLevel(FALSE);
			Sleep(300);

			if ( IsDualBufferUpperExist() )
			{
				SetErrorMessage("BL Frame exists in Upper Buffer");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
				return;
			}
			else if ( IsDualBufferLowerExist() )
			{
				if ( IsDualBufferLevelDown() == FALSE )
				{
					SetErrorMessage("BL Frame exists in Lower Buffer");
					SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
					return;
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

		BinGripperMoveTo(lSafePos);		//2. Move to safe pos
		SetGripperLevel(FALSE);			//3. Up gripper
		Sleep(200);

		//v4.42T15
		if (!bByPassOutOfMagCheck)			//v4.22T6	//PLLM
		{
			//v4.22T2
			INT nCount = 0;
			while ((IsFrameOutOfMgz() == TRUE))
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
				SetErrorMessage("BL Frame is out of magazine in housekeeping");
				Sleep(500);

				nCount++;
				if (nCount > 3)
				{
					if (BL_OK_CANCEL("OutOfMag sensor checking fails; continue?", "BL Housekeeping") == FALSE)
					{
						return;
					}
					break;
				}
			}
		}
	}
	else
	{
		SetGripperState(FALSE);			//1. Off gripper
		Sleep(200);

		if (CheckMS90BugPusherAtSafePos())	//v4.59A42
		{
			if (!X_IsPowerOn())				//v4.33T2	//Cree HuiZhou
			{
				X_Home();
				SetGripperLevel(FALSE);			//Up gripper first
				Sleep(200);
				BinGripperMoveTo(lSafePos);		//2. Move to safe pos
			}
			else
			{
				BinGripperMoveTo(lSafePos);		//2. Move to safe pos
			}
		}

		Sleep(200);
		SetGripperLevel(FALSE);			//3. Up gripper
		Sleep(200);

		if (!bByPassOutOfMagCheck)		//v4.22T6	//PLLM
		{
			//v4.42T2
			INT nCount = 0;
			while ((IsFrameOutOfMgz() == TRUE))
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
				SetErrorMessage("BL Frame is out of magazine in housekeeping");
				Sleep(500);

				nCount++;
				if (nCount > 3)
				{
					if (BL_OK_CANCEL("OutOfMag sensor checking fails; continue?", "BL Housekeeping") == FALSE)
					{
						return;
					}
					break;
				}
			}
		}
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		if (IsFrameInClamp() == FALSE && IsFrameOutOfMgz() == FALSE )
		{
			SetBufferLevel(FALSE);
		}
	}
}

VOID CBinLoader::HouseKeeping2(LONG lSafePos, BOOL bEnableBT, BOOL bIsMissingStep, BOOL bByPassOutOfMagCheck)
{
	LONG lRetryLimit = 0;

	//1st down bintable platform for safe
	if ( bEnableBT == TRUE )
	{
//		SetFrameVacuum2(FALSE);
//		SetFrameAlign2(FALSE);
		SetFrameVacuum(FALSE);
		SetFrameAlign(FALSE);
		Sleep(m_lBTAlignFrameDelay);

//		SetFrameLevel2(FALSE);
		SetFrameLevel(FALSE);
	}
	
	
	if (bIsMissingStep == TRUE)
	{
		if (!m_bNoSensorCheck && IsFrameInClamp2())		//v3.84		//IsFrameInClamp() always return TRUE when m_bNoSensorCheck=TRUE
		{
			do
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_DETECTED_IN_BL_ERR_HANDLE);
				lRetryLimit = lRetryLimit + 1;

				if (lRetryLimit == 3)
				{
					return;
				}

			}while(IsFrameInClamp2());

		}

		SetGripper2State(FALSE);			//1. Off gripper
		Sleep(200);

		X2_Home();

		BinGripper2MoveTo(lSafePos);			//2. Move to safe pos
		SetGripper2Level(FALSE);			//3. Up gripper
		Sleep(200);

	}
	else
	{
		SetGripper2State(FALSE);			//1. Off gripper
		Sleep(200);
		BinGripper2MoveTo(lSafePos);		//2. Move to safe pos
		Sleep(200);
		SetGripper2Level(FALSE);			//3. Up gripper
		Sleep(200);
	}
}

BOOL CBinLoader::IsAllMotorsEnable()
{
	BOOL bMotorsOn = FALSE;	
	
	if (!m_fHardware)
	{	
		return TRUE;
	}
	if (m_bDisableBL)		//v3.60
	{
		return TRUE;
	}

	bMotorsOn = (Upper_IsPowerOn() && Lower_IsPowerOn() && Theta_IsPowerOn() && Z_IsPowerOn());
	
	if (!bMotorsOn)
	{
		SetMotionCE(TRUE, "Bin Loader Motor Not ON");
	}

	return bMotorsOn;
}


BOOL CBinLoader::IsMagazineSafeToMove()
{
	BOOL bSafe = TRUE;

	if (!m_fHardware)
	{	
		return TRUE;
	}

	if (m_bDisableBL)
	{
		return TRUE;
	}

	//** Should be put before sensor reading in case of sensor problem to avoid crash!!
	if ( (m_bNoSensorCheck == TRUE) || (m_bBurnInEnable == TRUE) )
	{
		return TRUE;
	}

	if (GetGripperEncoder() > BL_GRIPPER_MAX_POS_LIMIT)
	{
		bSafe = FALSE;
	}

	if (GetGripper2Encoder() > BL_GRIPPER_MAX_POS_LIMIT)
	{
		bSafe = FALSE;
	}

	return bSafe;
}

BOOL CBinLoader::IsMagazine2SafeToMove()
{
	BOOL bSafe = TRUE;
	LONG lEncPos = 0;

	if (!m_fHardware)
	{
		return TRUE;
	}

	if (m_bDisableBL)
	{
		return TRUE;
	}

	if (m_lBinLoaderConfig != BL_CONFIG_DUAL_DL)
	{
		return TRUE;
	}

	//** Should be put before sensor reading in case of sensor problem to avoid crash!!
	if ( (m_bNoSensorCheck == TRUE) || (m_bBurnInEnable == TRUE) )
	{
		return TRUE;
	}

	//MS100 9Inch: HOME pos is far away from mgzn side (close to + limit sensor)
	//SW Limit: (mgzn) 13500 <---> -100
	//if ( GetGripper2Encoder() > BL_DTABLE_GRIPPER_MAX_POS_LIMIT )	
	//	bSafe = FALSE;
	if (GetGripper2Encoder() > (m_lLoadMagPos_X2 - 4000))	
		bSafe = FALSE;

	return bSafe;
}

BOOL CBinLoader::IsGripperSafeToMove()
{
	BOOL bSafe = TRUE;

	if (!m_fHardware)
	{	
		return TRUE;
	}
	if (m_bDisableBL)		//v3.60
		return TRUE;

	LONG lBtX = m_pInitOperation->GetAcServoChannel("BinTableXChannel")->GetEncoderPosition();
	if ( lBtX < (m_lBTUnloadPos_X + 1000) )
		bSafe = FALSE;

	return bSafe;
}


ULONG CBinLoader::GetBinBlkBondedCount(ULONG ulBinBlkId)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	ULONG ulBondedCount = 0;

	stMsg.InitMessage(sizeof(ULONG), &ulBinBlkId);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("GetBinBlkBondedCount"), stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	stMsg.GetMsg(sizeof(ULONG), &ulBondedCount);

	return ulBondedCount;	
}


UCHAR CBinLoader::GetBinBlkGrade(ULONG ulBinBlkId)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	UCHAR ucBinBlkGrade = '0';

	stMsg.InitMessage(sizeof(ULONG), &ulBinBlkId);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("GetBinGrade"), stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	stMsg.GetMsg(sizeof(UCHAR), &ucBinBlkGrade);

	return ucBinBlkGrade;

}

/*
BOOL CBinLoader::ClearAllBinFrame(BOOL bToFull)
{
	CString pszText1 = "Manual";	// == 1
	CString pszText2 = "Auto";		// == 5
	CString pszText3 = "Cancel";	// == 8
	CString szBarcodeName;
	CString szText;
	CString szTitle, szContent;
	BOOL bFull = bToFull;			//v3.98T5

	LONG lFrameOnBT = 0;
	LONG lUseReply = 0;
	LONG lCurrentBlock = 0;
	LONG lMagzNo = 0;
	LONG lSlotNo = 0;

	LONG lSlot = 0;
	LONG lMag = 0;

	BOOL bBufferLevel = DB_PreloadLevel();

	BOOL bAutoClear = TRUE;
	BOOL bNeedClear = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//BOOL bClearAllWithEmptyReplace = TRUE;

	szTitle.LoadString(HMB_BL_CLEAR_ALL_FRAME);

	szContent.LoadString(HMB_BL_CLEAR_ALL_MODE);

	lUseReply = HmiMessage(szContent, szTitle, glHMI_MBX_TRIPLEBUTTON , glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, &pszText1, &pszText2, &pszText3);
	switch(lUseReply)
	{
		case 1:
			bAutoClear = FALSE;
			break;

		case 5:
			bAutoClear = TRUE;
			break;

		default:
			return TRUE;
	}

	if ( bAutoClear == TRUE )
	{
		//Clear by Auto	
		if ( IsAllMotorsEnable() == FALSE )
		{
			SetErrorMessage("Motor is OFF");
			SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);

			return FALSE;
		}

		// check frame on bin table physically
		lFrameOnBT = CheckFrameOnBinTable();

		if ( m_bNoSensorCheck == TRUE )
		{
			lFrameOnBT = BL_FRAME_NOT_EXIST;
		}
		
		if( lFrameOnBT != BL_FRAME_NOT_EXIST )
		{
			szContent.LoadString(HMB_BL_REMOVE_BT_FRAME);	

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
			return FALSE;
		}

		SetBinTableJoystick(FALSE);

		if ( m_bIsExChgArmExist == TRUE )
		{
			// Protection for clear all frame
			if (GetCurrBinOnBT() != 0 || m_lExArmBufferBlock != 0)
			{
				szContent.LoadString(HMB_BT_FRAME_EX_ON_BUF_OR_BT);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
				return FALSE;
			}

			if ( MoveBinTable(0, 0) == FALSE )
			{
				return FALSE;
			}
		}
		

		for (lMagzNo=0; lMagzNo<MS_BL_MGZN_NUM; lMagzNo++)
		{
			for (lSlotNo=0; lSlotNo<MS_BL_MGZN_SLOT; lSlotNo++)
			{
				lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];

				if ( (m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE) && 
					 (m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0) && 
					 (GetBinBlkBondedCount((ULONG)lCurrentBlock) > m_ulMinClearAllCount) )
				{
					bNeedClear = TRUE;
					bFull = bToFull;	//v3.98T5
					m_lBTCurrentBlock = lCurrentBlock;
					bDBufferLevel = BL_BUFFER_LOWER;		//v3.98T5	//For DBuffer config only


					if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)		//DBuffer DLA	//v3.98T5
					{
						if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )		//v3.82
						{
							if (GetNextNullMgznSlot(lMag, lSlot) == FALSE)
							{
								m_lBTCurrentBlock = 0;
								m_szBinFrameBarcode = "";
								SetAlert_Red_Yellow(IDS_BL_NOEPYSLOT_IN_EPYMAG);
								SetErrorMessage("BL No empty slot next time in empty magazine");
								return TRUE;
							}
						}

						if (GetFullMgznSlot(lMag, lSlot) == FALSE)
						{
							m_lBTCurrentBlock = 0;
							m_szBinFrameBarcode = "";
							return TRUE;
						}

						if ( DB_LoadFromMgzToBuffer(FALSE, lCurrentBlock, TRUE, bDBufferLevel) == FALSE )
						{
							if (bDBufferLevel == BL_BUFFER_UPPER)
							{
								m_clUpperGripperBuffer.InitBufferInfo();
							}
							else
							{
								m_clLowerGripperBuffer.InitBufferInfo();
							}
							return FALSE;
						}

						if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )		//v3.82
						{
							//Unload this into empty Magazine
							if ( DB_UnloadFromBufferToMgz(FALSE, bFull, TRUE, FALSE, FALSE, bDBufferLevel) == FALSE )
							{
								if (bDBufferLevel == BL_BUFFER_UPPER)
								{
									m_clUpperGripperBuffer.InitBufferInfo();
								}
								else
								{
									m_clLowerGripperBuffer.InitBufferInfo();
								}
								return FALSE;
							}

							//Load this load empty frame
							if ( DB_LoadFromMgzToBuffer(FALSE, lCurrentBlock, TRUE, bDBufferLevel) == FALSE )
							{
								if (bDBufferLevel == BL_BUFFER_UPPER)
								{
									m_clUpperGripperBuffer.InitBufferInfo();
								}
								else
								{
									m_clLowerGripperBuffer.InitBufferInfo();
								}
								return FALSE;
							}

							bFull = FALSE;
						}

						if ( DB_UnloadFromBufferToMgz(FALSE, bFull, bFull, FALSE, FALSE, bDBufferLevel) == FALSE )
						{
							if (bDBufferLevel == BL_BUFFER_UPPER)
							{
								m_clUpperGripperBuffer.InitBufferInfo();
							}
							else
							{
								m_clLowerGripperBuffer.InitBufferInfo();
							}
							return FALSE;
						}

					}
					else if ( m_bIsExChgArmExist == FALSE )		//DL
					{

						if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )		//v3.82
						{
							if (GetNextNullMgznSlot(lMag, lSlot) == FALSE)
							{
								SetAlert_Red_Yellow(IDS_BL_NOEPYSLOT_IN_EPYMAG);		
								SetErrorMessage("BL No empty slot next time in empty magazine");
								return TRUE;
							}
						}

						//Load current Active Slot
						if ( LoadBinFrame(FALSE, &m_clUpperGripperBuffer, lCurrentBlock, TRUE, TRUE) == FALSE )
						{
							return FALSE;
						}

						if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )	//v3.82
						{
							//Unload this into empty Magazine
							if ( UnloadBinFrame(FALSE, &m_clLowerGripperBuffer, bFull, TRUE, TRUE) == FALSE )
							{
								return FALSE;
							}
							
							//Load this load empty frame
							if ( LoadBinFrame(FALSE, &m_clUpperGripperBuffer, lCurrentBlock, TRUE, TRUE) == FALSE )
							{
								return FALSE;
							}

							bFull = FALSE;
						}

						if ( UnloadBinFrame(FALSE, &m_clLowerGripperBuffer, bFull, TRUE, TRUE) == FALSE )
						{
							return FALSE;
						}	
						
					}
					else		//ExArm DLA
					{
						if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )		//v3.82
						{
							if (GetNextNullMgznSlot(lMag, lSlot) == FALSE)
							{
								m_lBTCurrentBlock = 0;
								m_szBinFrameBarcode = "";
								SetAlert_Red_Yellow(IDS_BL_NOEPYSLOT_IN_EPYMAG);
								SetErrorMessage("BL No empty slot next time in empty magazine");
								return TRUE;
							}
						}
						
						if (GetFullMgznSlot(lMag,lSlot) == FALSE)
						{
							m_lBTCurrentBlock = 0;
							m_szBinFrameBarcode = "";
							return TRUE;
						}

						//Load current Active Slot
						if ( ExArmLoadBinFrame(FALSE, lCurrentBlock, TRUE) == FALSE )
						{
							m_lBTCurrentBlock = 0;
							m_szBinFrameBarcode = "";
							return FALSE;
						}

						if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )		//v3.82
						{
							//Unload this into empty Magazine
							//if ( ExArmUnloadBinFrame(FALSE, bFull, FALSE, FALSE, TRUE, TRUE) == FALSE )
							if ( ExArmUnloadBinFrame(FALSE, bFull, TRUE, FALSE, FALSE, TRUE) == FALSE )
							{
								m_lBTCurrentBlock = 0;
								m_szBinFrameBarcode = "";
								return FALSE;
							}

							//Load this load empty frame
							if ( ExArmLoadBinFrame(FALSE, lCurrentBlock, TRUE, TRUE) == FALSE )
							{
								m_lBTCurrentBlock = 0;
								m_szBinFrameBarcode = "";
								return FALSE;
							}

							//bIsEmpty = TRUE;
							bFull = FALSE;
						}

						if ( ExArmUnloadBinFrame(FALSE, bFull, bFull, FALSE, FALSE, FALSE) == FALSE )
						{
							m_lBTCurrentBlock = 0;
							m_szBinFrameBarcode = "";
							return FALSE;
						}
					}
					
					try
					{
						SaveData();
						SaveMgznRTData();
					}
					catch(CFileException e)
					{
						BL_DEBUGBOX("BL File Exception Clear All Bin Frame");
					}

					if ( pApp->IsStopAlign() )
					{
						m_lBTCurrentBlock = 0;
						m_szBinFrameBarcode = "";

						szContent.LoadString(HMB_GENERAL_STOPBYUSER);	

						HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
						return TRUE;
					}
				}
			}
		}

		DownElevatorToReady();

		//Move Table back to Home
		if ( MoveBinTable(0, 0) == FALSE )
		{
			return FALSE;
		}

		SetBinTableJoystick(TRUE);
	}
	else
	{
		//Clear by Manual
		for (lMagzNo=0; lMagzNo<MS_BL_MGZN_NUM; lMagzNo++)
		{
			for (lSlotNo=0; lSlotNo<MS_BL_MGZN_SLOT; lSlotNo++)
			{
				lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];

				if ( (m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE) && 
					 (m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0) && 
					 (GetBinBlkBondedCount((ULONG)lCurrentBlock) > m_ulMinClearAllCount) )
				{
					bNeedClear = TRUE;
					szBarcodeName = m_stMgznRT[lMagzNo].m_SlotBCName[lSlotNo];

					if (szBarcodeName == BL_DEFAULT_BARCODE)
					{
						
						szText.Format("No Barcode for Bin Block %d",lCurrentBlock);
						AlertMsgForManualInputBarcode(lCurrentBlock,lMagzNo,lSlotNo,szBarcodeName,szText);
					}

					if ( ClearBinFrameCounter(lCurrentBlock, szBarcodeName, FALSE) == FALSE )
					{
						return FALSE;
					}
					else
					{
						// Log Frame Info into Temp Mag Summary Files respectively 
						LogFrameFullInformation(TRUE,lMagzNo,lSlotNo,szBarcodeName);
						//Reset barcode name to default
						SaveBarcodeData(lCurrentBlock, "", lMagzNo, lSlotNo);
					}
				}
			}
		}
	
		// Merge All Temp Mag Summary File and Output the summary file to
		// user sepecific location
		OutputMagSummaryFileForAllMag();
	}

	if ( bNeedClear == TRUE )
	{
		//v3.27T1

		//table & buffer table frame index no matter RESET or not
		m_lBTCurrentBlock = 0;
		m_lExArmBufferBlock = 0;

		m_szBinFrameBarcode = "";
		m_szBufferFrameBarcode = "";

		m_szUBufferBarcode = "";
		m_szLBufferBarcode = "";
		
		m_lCurrMgzn			= 0;
		m_lCurrSlot			= 0;
		m_lCurrHmiMgzn		= 0;	
		m_lCurrHmiSlot		= 0;

		//Reset SRAM such that will trigger Grade-Change in AUTOBOND
		(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= 0;
		(*m_psmfSRam)["BinTable"]["BlkInUse"]		= 0;


		szContent.LoadString(HMB_BL_RESET_ALL_MAGZ);
		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300) == glHMI_YES)
		{
			// don't allow to reset magazine if there still some die count
			if (CMS896AStn::m_bEnableResetMagzCheck == TRUE)
			{
				if (pApp->CheckIsAllBinClearedNoMsg() == FALSE)
				{
					szContent.LoadString(HMB_BL_RESET_MAG_BIN_NOT_CLEARED);
					HmiMessage(szContent, szTitle);
					return FALSE;	
				}
			}

			for (lMagzNo=0; lMagzNo<MS_BL_MGZN_NUM; lMagzNo++)
			{
				ResetMagazine(lMagzNo);
			}
		}

		szContent.LoadString(HMB_GENERAL_COMPLETED);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
	}
	else
	{
		szContent.LoadString(HMB_BL_NO_MORE_FRAME_CLR);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
	}

	BT_ResetOptimizeBinCountStatus();
	ResetMapDieType();

	return TRUE;
}
*/

BOOL CBinLoader::CheckAllWaferLoaded()
{
	CString szContent, szTitle;
	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	LONG lOption = 0;

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, _T("CheckAllWaferLoadedCmd"), stMsg);
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
	
	if (bResult == FALSE)
	{
		szTitle.LoadString(HMB_BL_CLEAR_ALL_FRAME);
		szContent.LoadString(IDS_BL_CHECK_ALL_WAFER_LOADED_FAIL);
		
		lOption = HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 550, 400, NULL, NULL, NULL, NULL);

		if (lOption == glHMI_STOP)
		{
			return FALSE;
		}
		
		LONG lAccessLevel = MS_ENGINEER_ACCESS_LEVEL;

		stMsg.InitMessage(sizeof(LONG), &lAccessLevel);
		nConvID = m_comClient.SendRequest("MapSorter", _T("CheckAccessRight"), stMsg);
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

	return bResult;
}

BOOL CBinLoader::IsClearAllState()
{
	CString szTitle, szContent;

	if (m_bClearAllFrameState == TRUE)
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

		if (pApp->m_bMSAutoLineMode == 1)
		{
			m_bClearAllFrameState = FALSE;		//v4.57A15
		}
		else
		{
			szTitle.LoadString(HMB_BL_CLEAR_ALL_FRAME);
			szContent.LoadString(HMB_BL_CLEAR_ALL_FRAME_NOT_COMPLETE);	
			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBinLoader::ClearAllBinFrame(BOOL bToFull)
{
	CString pszText1 = "Manual";	// == 1
	CString pszText2 = "Auto";		// == 5
	CString pszText3 = "Cancel";	// == 8
	CString szBarcodeName;
	CString szText;
	CString szTitle, szContent;
	BOOL bFull = bToFull;			//v3.98T5

	LONG lFrameOnBT = 0;
	LONG lUseReply = 0;
	LONG lCurrentBlock = 0;
	LONG lMagzNo = 0;
	LONG lSlotNo = 0;

	LONG lSlot = 0;
	LONG lMag = 0;

	INT nTotalFrameNo = 0;

	FRAME_DETAILS stInfo, stTempInfo;
	CArray<FRAME_DETAILS, FRAME_DETAILS> aFrameInfo;

	BOOL bBufferLevel = DB_PreloadLevel();

	BOOL bAutoClear = TRUE;
	BOOL bNeedClear = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//BOOL bClearAllWithEmptyReplace = TRUE;
	szTitle.LoadString(HMB_BL_CLEAR_ALL_FRAME);

	szContent.LoadString(HMB_BL_CLEAR_ALL_MODE);

	lUseReply = HmiMessage(szContent, szTitle, glHMI_MBX_TRIPLEBUTTON , glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, &pszText1, &pszText2, &pszText3);
	switch(lUseReply)
	{
		case 1:
			bAutoClear = FALSE;
			break;

		case 5:
			bAutoClear = TRUE;
			break;

		default:
			return TRUE;
	}

	m_bClearAllFrameState = TRUE;
/*
	//v4.53A4	//Dicon TW
	if (pApp->GetCustomerName() == "Dicon" || pApp->GetCustomerName() == "FiberOptics")
	{
		//#define BL_MGZ_TOP_1				0
		//#define BL_MGZ_MID_1				1
		//#define BL_MGZ_BTM_1				2
		//#define BL_MGZ_TOP_2				3
		//#define BL_MGZ_MID_2				4
		//#define BL_MGZ_BTM_2				5
		//#define BL_MGZ8_BTM_1				6
		//#define BL_MGZ8_BTM_2				7

		lMagzNo = 0;	//TOP1
		for (lSlotNo=0; lSlotNo<MS_BL_MGZN_SLOT; lSlotNo++)
		{
			lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];
			if ( (m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE)	&& 
				 (m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0)						&& 
				 (GetBinBlkBondedCount((ULONG)lCurrentBlock) > m_ulMinClearAllCount) )
			{
				stInfo.lMgzNo = lMagzNo;
				stInfo.lSlotNo = lSlotNo;
				stInfo.lDieCount = GetBinBlkBondedCount((ULONG)lCurrentBlock);
				aFrameInfo.Add(stInfo);
			}
		}

		lMagzNo = 3;	//TOP2
		for (lSlotNo=0; lSlotNo<MS_BL_MGZN_SLOT; lSlotNo++)
		{
			lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];
			if ( (m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE)	&& 
				 (m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0)						&& 
				 (GetBinBlkBondedCount((ULONG)lCurrentBlock) > m_ulMinClearAllCount) )
			{
				stInfo.lMgzNo = lMagzNo;
				stInfo.lSlotNo = lSlotNo;
				stInfo.lDieCount = GetBinBlkBondedCount((ULONG)lCurrentBlock);
				aFrameInfo.Add(stInfo);
			}
		}

		lMagzNo = 1;	//MID1
		for (lSlotNo=0; lSlotNo<MS_BL_MGZN_SLOT; lSlotNo++)
		{
			lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];
			if ( (m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE)	&& 
				 (m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0)						&& 
				 (GetBinBlkBondedCount((ULONG)lCurrentBlock) > m_ulMinClearAllCount) )
			{
				stInfo.lMgzNo = lMagzNo;
				stInfo.lSlotNo = lSlotNo;
				stInfo.lDieCount = GetBinBlkBondedCount((ULONG)lCurrentBlock);
				aFrameInfo.Add(stInfo);
			}
		}

		lMagzNo = 4;	//MID2
		for (lSlotNo=0; lSlotNo<MS_BL_MGZN_SLOT; lSlotNo++)
		{
			lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];
			if ( (m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE)	&& 
				 (m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0)						&& 
				 (GetBinBlkBondedCount((ULONG)lCurrentBlock) > m_ulMinClearAllCount) )
			{
				stInfo.lMgzNo = lMagzNo;
				stInfo.lSlotNo = lSlotNo;
				stInfo.lDieCount = GetBinBlkBondedCount((ULONG)lCurrentBlock);
				aFrameInfo.Add(stInfo);
			}
		}

		lMagzNo = 2;	//BTM1
		for (lSlotNo=0; lSlotNo<MS_BL_MGZN_SLOT; lSlotNo++)
		{
			lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];
			if ( (m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE)	&& 
				 (m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0)						&& 
				 (GetBinBlkBondedCount((ULONG)lCurrentBlock) > m_ulMinClearAllCount) )
			{
				stInfo.lMgzNo = lMagzNo;
				stInfo.lSlotNo = lSlotNo;
				stInfo.lDieCount = GetBinBlkBondedCount((ULONG)lCurrentBlock);
				aFrameInfo.Add(stInfo);
			}
		}

		//lMagzNo = 5;	//BTM2
		//lMagzNo = 6;	//8Mgzn1	
		//lMagzNo = 7;	//8Mgzn2
		for (lMagzNo=5; lMagzNo<MS_BL_MGZN_NUM; lMagzNo++)
		{
			for (lSlotNo=0; lSlotNo<MS_BL_MGZN_SLOT; lSlotNo++)
			{
				lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];
				if ( (m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE)	&& 
					 (m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0)						&& 
					 (GetBinBlkBondedCount((ULONG)lCurrentBlock) > m_ulMinClearAllCount) )
				{
					stInfo.lMgzNo = lMagzNo;
					stInfo.lSlotNo = lSlotNo;
					stInfo.lDieCount = GetBinBlkBondedCount((ULONG)lCurrentBlock);
					aFrameInfo.Add(stInfo);
				}
			}
		}
	}
	else
*/
	{
		for (lMagzNo = 0; lMagzNo < MS_BL_MGZN_NUM; lMagzNo++)
		{
			for (lSlotNo = 0; lSlotNo < MS_BL_MGZN_SLOT; lSlotNo++)
			{
				lCurrentBlock = m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo];

				if ((m_stMgznRT[lMagzNo].m_lSlotUsage[lSlotNo] == BL_SLOT_USAGE_ACTIVE) && 
					(m_stMgznRT[lMagzNo].m_lSlotBlock[lSlotNo] > 0) && 
					(GetBinBlkBondedCount((ULONG)lCurrentBlock) > m_ulMinClearAllCount) )
				{
					stInfo.lMgzNo = lMagzNo;
					stInfo.lSlotNo = lSlotNo;
					stInfo.lDieCount = GetBinBlkBondedCount((ULONG)lCurrentBlock);
					aFrameInfo.Add(stInfo);
				}
			}
		}
	}

	nTotalFrameNo = (INT)aFrameInfo.GetSize();

	 for (INT i = 0; i < nTotalFrameNo; i++)
	 {
		for (INT j = nTotalFrameNo-1; j > i; j--)
		{
			if (m_lClearAllFrameMode == BL_CLEAR_FRAME_MODE_ASC)
			{
				if (aFrameInfo[j-1].lDieCount > aFrameInfo[j].lDieCount)
				{
					stTempInfo = aFrameInfo[j-1];
					aFrameInfo[j-1] = aFrameInfo[j];
					aFrameInfo[j] = stTempInfo;
				}	
			}
			else if (m_lClearAllFrameMode == BL_CLEAR_FRAME_MODE_DEC)
			{
				if (aFrameInfo[j-1].lDieCount < aFrameInfo[j].lDieCount)
				{
					stTempInfo = aFrameInfo[j-1];
					aFrameInfo[j-1] = aFrameInfo[j];
					aFrameInfo[j] = stTempInfo;
				}
			}
		}
	}

	if ( bAutoClear == TRUE )
	{
		//Clear by Auto	
		if ( IsAllMotorsEnable() == FALSE )
		{
			SetErrorMessage("Motor is OFF");
			SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);

			return FALSE;
		}

		// check frame on bin table physically
		lFrameOnBT = CheckFrameOnBinTable();

		if ( m_bNoSensorCheck == TRUE )
		{
			lFrameOnBT = BL_FRAME_NOT_EXIST;
		}
		
		if( lFrameOnBT != BL_FRAME_NOT_EXIST )
		{
			szContent.LoadString(HMB_BL_REMOVE_BT_FRAME);	

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
			return FALSE;
		}

		SetBinTableJoystick(FALSE);

		if ( m_bIsExChgArmExist == TRUE )
		{
			// Protection for clear all frame
			if (GetCurrBinOnBT() != 0 || m_lExArmBufferBlock != 0)
			{
				szContent.LoadString(HMB_BT_FRAME_EX_ON_BUF_OR_BT);
				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
				return FALSE;
			}

			if ( MoveBinTable(0, 0) == FALSE )
			{
				return FALSE;
			}
		}
		
		//v4.49A13	//Testar MES message for EndLot
		if (CMESConnector::Instance()->IsMESConnectorEnable())
		{
			CMESConnector::Instance()->SendMessage("2", "Clean Bin Start");

			INT nCode = 0;
			CString szMsg;
			INT nCount = 0;
			while (1)
			{
				nCode = CMESConnector::Instance()->RecvMessage(szMsg);	
				if ( (nCode == TRUE) )	//&& (szMsg.GetLength() > 0) )
				{
					break;
				}
				else
				{
					Sleep(100);
					nCount++;
					if (nCount > 20)
					{
						szContent = "MES connection timeout for \"Clean Bin Start\" message!";
						HmiMessage_Red_Yellow(szContent);
						SetErrorMessage(szContent);
						return FALSE;
					}
				}
			}
		}

		for (INT i=0; i<aFrameInfo.GetSize(); i++)
		{
			lCurrentBlock = m_stMgznRT[aFrameInfo[i].lMgzNo].m_lSlotBlock[aFrameInfo[i].lSlotNo];

			bNeedClear = TRUE;
			bFull = bToFull;	//v3.98T5
			//m_lBTCurrentBlock = lCurrentBlock;
			BOOL bDBufferLevel = BL_BUFFER_UPPER;		//v4.19T1	//CMLT
			CString szMsg;
			szMsg.Format("Clear all bin frame data updated block id to %d", GetCurrBinOnBT());
			BL_DEBUGBOX(szMsg);

/*
			if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)		//DBuffer DLA	//v3.98T5
			{
				if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )		//v3.82
				{
					if (ClearAllFrameResetNullMagazine() == FALSE)
					{
						BL_DEBUGBOX("dbuffer clear all null mgzn set cur block to 0");
						m_lBTCurrentBlock = 0;
						m_szBinFrameBarcode = "";
						return TRUE;
					}
				}

				if (ClearAllFrameResetFullMagazine() == FALSE)
				{
					BL_DEBUGBOX("dbuffer clear all full mgzn cur block to 0");
					m_lBTCurrentBlock = 0;
					m_szBinFrameBarcode = "";
					return TRUE;
				}
					
				if ( DB_LoadFromMgzToBuffer(FALSE, lCurrentBlock, TRUE, bDBufferLevel) == FALSE )
				{
					if (bDBufferLevel == BL_BUFFER_UPPER)
					{
						m_clUpperGripperBuffer.InitBufferInfo();
					}
					else
					{
						m_clLowerGripperBuffer.InitBufferInfo();
					}
					return FALSE;
				}

				if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )		//v3.82
				{
					//Unload this into empty Magazine
					if ( DB_UnloadFromBufferToMgz(FALSE, bFull, TRUE, FALSE, FALSE, bDBufferLevel) == FALSE )
					{
						if (bDBufferLevel == BL_BUFFER_UPPER)
						{
							m_clUpperGripperBuffer.InitBufferInfo();
						}
						else
						{
							m_clLowerGripperBuffer.InitBufferInfo();
						}
						return FALSE;
					}

					//Load this load empty frame
					if ( DB_LoadFromMgzToBuffer(FALSE, lCurrentBlock, TRUE, bDBufferLevel) == FALSE )
					{
						if (bDBufferLevel == BL_BUFFER_UPPER)
						{
							m_clUpperGripperBuffer.InitBufferInfo();
						}
						else
						{
							m_clLowerGripperBuffer.InitBufferInfo();
						}
						return FALSE;
					}

					bFull = FALSE;
				}

				if ( DB_UnloadFromBufferToMgz(FALSE, bFull, bFull, FALSE, FALSE, bDBufferLevel) == FALSE )
				{
					if (bDBufferLevel == BL_BUFFER_UPPER)
					{
						m_clUpperGripperBuffer.InitBufferInfo();
					}
					else
					{
						m_clLowerGripperBuffer.InitBufferInfo();
					}
					return FALSE;
				}

			}
			else 
*/
			if ( m_bIsExChgArmExist == FALSE )		//DL
			{
/*
				if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )		//v3.82
				{
					if (ClearAllFrameResetNullMagazine() == FALSE)
					{
						BL_DEBUGBOX(" dl reset null mgzn cur block to 0");
						m_lBTCurrentBlock = 0;
						m_szBinFrameBarcode = "";
						return TRUE;
					}
				}
*/
				if (ClearAllFrameResetFullMagazine() == FALSE)
				{
					BL_DEBUGBOX("dl reset full mgzn cur block to 0");
					m_lBTCurrentBlock = 0;
					m_szBinFrameBarcode = "";
					return TRUE;
				}
				
				//For SIS, machine will clear all WIP frame to the TOP directly with FULL mode
				if (IsMSAutoLineMode())
				{
					if (UDB_TransferFrame(FALSE, lCurrentBlock, &m_clUpperGripperBuffer, bFull, FALSE, FALSE, TRUE) == FALSE)
					{
						return FALSE;
					}
				}
				else
				{
					//Load current Active Slot
					/*if (LoadBinFrame(FALSE, &m_clUpperGripperBuffer, lCurrentBlock, TRUE, TRUE) == FALSE)
					{
						return FALSE;
					}*/

					if (LoadFrameToFullMgz(&m_clUpperGripperBuffer, lCurrentBlock, bFull, TRUE, TRUE) == FALSE)
					{
						return FALSE;
					}
/*
					//MS50 only use BL_MODE_F, it can be ignored
					if ( (m_lOMRT == BL_MODE_E) || (m_lOMRT == BL_MODE_G) )	//v3.82
					{
						//Unload this into empty Magazine
						if ( UnloadBinFrame(FALSE, &m_clLowerGripperBuffer, bFull, TRUE, TRUE) == FALSE )
						{
							return FALSE;
						}
						//Load this load empty frame
						if ( LoadBinFrame(FALSE, &m_clUpperGripperBuffer, lCurrentBlock, TRUE, TRUE) == FALSE )
						{
							return FALSE;
						}
						bFull = FALSE;
					}
*/
					/*if (UnloadBinFrame(FALSE, &m_clLowerGripperBuffer, bFull, TRUE, TRUE) == FALSE)
					{
						return FALSE;
					}*/
				}
				
			}
			else		//ExArm DLA
			{
			}
			
			try
			{
				SaveData();
				//SaveMgznRTData();
			}
			catch(CFileException e)
			{
				BL_DEBUGBOX("BL File Exception Clear All Bin Frame");
			}

			try
			{
				//SaveData();
				SaveMgznRTData();	//v4.06
			}
			catch(CFileException e)
			{
				BL_DEBUGBOX("BL SaveMgznRTData Exception Clear All Bin Frame");
			}

			if (pApp->IsStopAlign())	//v4.05	//Klocwork
			{
				BL_DEBUGBOX("stop align cur block to 0");
				m_lBTCurrentBlock = 0;
				m_szBinFrameBarcode = "";

				szContent.LoadString(HMB_GENERAL_STOPBYUSER);	

				HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
				return FALSE;
			}
		}

		CBinGripperBuffer *pGripperBuffer = &m_clLowerGripperBuffer;
		if (!DownElevatorToReady(pGripperBuffer))
		{
			return FALSE;
		}

		//Move Table back to Home
		if ( MoveBinTable(0, 0) == FALSE )
		{
			return FALSE;
		}

		//v4.49A13	//Testar MES message for EndLot
		if (CMESConnector::Instance()->IsMESConnectorEnable())
		{
			CMESConnector::Instance()->SendMessage("2", "Clean Bin Finish");

			INT nCode = 0;
			CString szMsg;
			INT nCount = 0;
			while (1)
			{
				nCode = CMESConnector::Instance()->RecvMessage(szMsg);	
				if ( (nCode == TRUE) )	//&& (szMsg.GetLength() > 0) )	//v4.50A1
				{
					break;
				}
				else
				{
					Sleep(100);
					nCount++;
					if (nCount > 20)
					{
						szContent = "MES connection timeout for \"Clean Bin Finish\" message!";
						HmiMessage_Red_Yellow(szContent);
						SetErrorMessage(szContent);
						return FALSE;
					}
				}
			}
		}

		SetBinTableJoystick(TRUE);
	}
	else
	{
		//Clear by Manual

		for (INT i=0; i<aFrameInfo.GetSize(); i++)
		{
			lCurrentBlock = m_stMgznRT[aFrameInfo[i].lMgzNo].m_lSlotBlock[aFrameInfo[i].lSlotNo];

			bNeedClear = TRUE;
			szBarcodeName = m_stMgznRT[aFrameInfo[i].lMgzNo].m_SlotBCName[aFrameInfo[i].lSlotNo];

			if (szBarcodeName == BL_DEFAULT_BARCODE)
			{
				
				szText.Format("No Barcode for Bin Block %d",lCurrentBlock);
				AlertMsgForManualInputBarcode(lCurrentBlock,aFrameInfo[i].lMgzNo,aFrameInfo[i].lSlotNo,szBarcodeName,szText);
			}

			if ( ClearBinFrameCounter(lCurrentBlock, szBarcodeName, FALSE) == FALSE )
			{
				return FALSE;
			}
			else
			{
				// Log Frame Info into Temp Mag Summary Files respectively 
				LogFrameFullInformation(TRUE,aFrameInfo[i].lMgzNo, aFrameInfo[i].lSlotNo,szBarcodeName);
				//Reset barcode name to default
				SaveBarcodeData(lCurrentBlock, "", aFrameInfo[i].lMgzNo, aFrameInfo[i].lSlotNo);
			}
		}

		// Merge All Temp Mag Summary File and Output the summary file to
		// user sepecific location
		OutputMagSummaryFileForAllMag();
	}

	if ( bNeedClear == TRUE )
	{
		//v3.27T1

		BL_DEBUGBOX("need to clear cur block to 0");
		//table & buffer table frame index no matter RESET or not
		m_lBTCurrentBlock = 0;
		m_lExArmBufferBlock = 0;

		m_szBinFrameBarcode = "";
		m_szBufferFrameBarcode = "";

		m_clUpperGripperBuffer.SetBufferBarcode("");
		m_clLowerGripperBuffer.SetBufferBarcode("");
	
		m_lCurrMgzn			= 0;
		m_lCurrSlot			= 0;
		m_lCurrHmiMgzn		= 0;	
		m_lCurrHmiSlot		= 0;

		//Reset SRAM such that will trigger Grade-Change in AUTOBOND
		(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= 0;
		(*m_psmfSRam)["BinTable"]["BlkInUse"]		= 0;

/*
		szContent.LoadString(HMB_BL_RESET_ALL_MAGZ);
		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300) == glHMI_YES)
		{
			// don't allow to reset magazine if there still some die count
			if (CMS896AStn::m_bEnableResetMagzCheck == TRUE)
			{
				if (pApp->CheckIsAllBinClearedNoMsg() == FALSE)
				{
					szContent.LoadString(HMB_BL_RESET_MAG_BIN_NOT_CLEARED);
					HmiMessage(szContent, szTitle);
					return FALSE;	
				}
			}
*/

			for (lMagzNo=0; lMagzNo<MS_BL_MGZN_NUM; lMagzNo++)
			{
				BOOL bResetMagazine = TRUE;
				if (IsMSAutoLineMode() && ((lMagzNo == BL_MGZ_TOP_1) || (lMagzNo == BL_MGZ_TOP_2)))
				{
					bResetMagazine = FALSE;
				}
				else if (IsMSAutoLineStandloneMode() && (lMagzNo == BL_MGZ_TOP_2))
				{
					bResetMagazine = FALSE;
				}
			
				if (bResetMagazine)
				{
					ResetMagazine(lMagzNo, TRUE, TRUE);
				}
			}
		//}

		szContent.LoadString(HMB_GENERAL_COMPLETED);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
	}
	else
	{
		szContent.LoadString(HMB_BL_NO_MORE_FRAME_CLR);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200);
	}

	if (m_ulMinClearAllCount == 0)
	{
		if (pApp->CheckIsAllBinClearedNoMsg() == TRUE)
		{
			m_bClearAllFrameState = FALSE;		
		}
	}
	else
	{
		if (pApp->CheckIsAllBinClearedWithDieCountConstraint(m_ulMinClearAllCount) == TRUE)
		{
			m_bClearAllFrameState = FALSE;		
		}
	}
	
	if (pApp->GetCustomerName() == "JianSen")
	{
		CString szJSTemp;
		for( int nJSTemp = 0; nJSTemp < 152; nJSTemp ++)
		{
			szJSTemp.Format("%d",nJSTemp);
			remove( "c:\\MapSorter\\UserData\\SerialNo\\" + szJSTemp + "_SerialNo.txt");
		}
	}

	if (IsMSAutoLineMode() || IsMSAutoLineStandloneMode())
	{
		ResetFullMagazine(BL_MGZ_TOP_2);
	}
	else
	{
		ResetFullMagazine(BL_MGZ_TOP_1);
	}

	BT_ResetOptimizeBinCountStatus();
	ResetMapDieType();

	SaveData();

	return TRUE;
}


BOOL CBinLoader::BT_ResetOptimizeBinCountStatus()
{
	BOOL bReturn = TRUE;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;

	//stMsg.InitMessage(sizeof(BOOL), &bReturn);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetOptimizeBinCountStatus", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

BOOL CBinLoader::ResetMapDieType()
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

BOOL CBinLoader::ShutDown()
{
	if (m_bDisableBL)		//v3.60
		return TRUE;

	//Gripper move to home position
	if ( m_fHardware)
	{
		//if ( (m_pStepper_X->IsPowerOn() == TRUE) && (m_bMotionFail == FALSE) )
		if ( (X_IsPowerOn() == TRUE) && (m_bMotionFail == FALSE) )
		{
			if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
			{
				if (IsDualBufferUpperExist() == FALSE && IsDualBufferLowerExist() == FALSE )
				{
					X_MoveTo(0);
				}
			}
			else
			{
				Upper_MoveTo(0);
			}
		}
	}

	//Move Z to 4mm before home position
	if ( m_fHardware && !m_bNewZConfig)	//andrew
	{
		//if ( (m_pServo_Z->IsPowerOn() == TRUE) && (IsMagazineSafeToMove() == TRUE) && (m_bMotionFail == FALSE) )
		if ( (Z_IsPowerOn() == TRUE) && (IsMagazineSafeToMove() == TRUE) && (m_bMotionFail == FALSE) )
		{
			if ( m_dZRes > 0.0 )
			{
				Z_MoveTo(-(INT)(4.0 / m_dZRes));
			}
			else
			{
				Z_MoveTo(-10000);
			}	
		}
	}

	return TRUE;
}


BOOL CBinLoader::IsRearArmFrameExist()
{
	//return m_piRearArmFrameExist->IsLow();
	if (m_bDisableBL)		//v3.60
		return TRUE;
	return !CMS896AStn::MotionReadInputBit(BL_SI_RArmFrameExist);
}


BOOL CBinLoader::IsFrontArmFrameExist()
{
	//return m_piFrontArmFrameExist->IsLow();
	if (m_bDisableBL)		//v3.60
		return TRUE;
	return !CMS896AStn::MotionReadInputBit(BL_SI_FArmFrameExist);
}

BOOL CBinLoader::CheckFrameExistByVacuum()
{
	//if (m_piFrameVacuum == NULL)
	if (m_bDisableBL)		//v3.60
		return TRUE;
	if ( m_bNoSensorCheck == TRUE )
		return TRUE;	//OK -> assume frame exist
	if (!m_bUseBinTableVacuum)
		return TRUE;	//OK -> assume frame exist

    CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckVacMeter = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_BT_VAC_METER_CHECKING);
	if (!bCheckVacMeter)
	{
		return TRUE;	//OK -> assume frame exist
	}


	//If BT-Vac On
		//return FALSE -> LightOn -> no frame on table
		//return TRUE  -> LightOff -> Frame exist
	//If BT-Vac Off
		//always LightOn -> no frame state!

	//BOOL bFrameExist = !m_piFrameVacuum->IsLow();
	BOOL bFrameExist = CMS896AStn::MotionReadInputBit(BL_SI_FrameVac);
	if (!bFrameExist)
	{
		Sleep(200);
		//bFrameExist = !m_piFrameVacuum->IsLow();	//Retry after 200ms
		bFrameExist = CMS896AStn::MotionReadInputBit(BL_SI_FrameVac);	//Retry after 200ms
	}
	return bFrameExist;
}

BOOL CBinLoader::CheckFrameExistByVacuum2()
{
	if (m_bDisableBL)		//v3.60
		return TRUE;
	if ( m_bNoSensorCheck == TRUE )
		return TRUE;	//OK -> assume frame exist
	if (!m_bUseBinTableVacuum)
		return TRUE;	//OK -> assume frame exist
	if (m_lBinLoaderConfig != BL_CONFIG_DUAL_DL)
		return TRUE;

    CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckVacMeter = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_BT_VAC_METER_CHECKING);
	if (!bCheckVacMeter)
	{
		return TRUE;	//OK -> assume frame exist
	}


	//If BT-Vac On
		//return FALSE -> LightOn -> no frame on table
		//return TRUE  -> LightOff -> Frame exist
	//If BT-Vac Off
		//always LightOn -> no frame state!

	//BOOL bFrameExist = !m_piFrameVacuum->IsLow();
	BOOL bFrameExist = CMS896AStn::MotionReadInputBit(BL_SI_FrameVac2);
	if (!bFrameExist)
	{
		Sleep(200);
		//bFrameExist = !m_piFrameVacuum->IsLow();	//Retry after 200ms
		bFrameExist = CMS896AStn::MotionReadInputBit(BL_SI_FrameVac2);	//Retry after 200ms
	}
	return bFrameExist;
}

BOOL CBinLoader::IsExchangeArmReady()
{
	if ( m_bNoSensorCheck == TRUE )
	{
		return TRUE;
	}
	if (m_bDisableBL)		//v3.60
		return TRUE;

	if ( CMS896AStn::MotionReadInputBit(BL_SI_FArmReady) )
	{
		SetAlert_Red_Yellow(IDS_BL_FRONTARM_NOT_READY);
		SetErrorMessage("FrontArm not in ready position");
		return FALSE;
	}
		
	if ( CMS896AStn::MotionReadInputBit(BL_SI_RArmReady) )
	{
		SetAlert_Red_Yellow(IDS_BL_REARARM_NOT_READY);
		SetErrorMessage("RearArm not in ready position");
		return FALSE;
	}

	return TRUE;
}


BOOL CBinLoader::IsBondArmSafe()
{
	IPC_CServiceMessage stMsg;
	BOOL bSafe = TRUE;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, _T("BH_IsBondArmSafe"), stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bSafe);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if ( bSafe == FALSE )
	{
		SetAlert_Red_Yellow(IDS_BL_BONDARM_NOT_SAFE);
		SetErrorMessage("BondArm is not in safe position");
	}

	return bSafe;	
}


BOOL CBinLoader::IsLimitSensorHit()
{
	if (m_bDisableBL)		//v3.60
		return FALSE;

	if ( m_bNoSensorCheck == TRUE )
	{
		return FALSE;
	}

	return FALSE;
}

BOOL CBinLoader::IsBinElevatorCoverLocked()
{
	if (m_fHardware == FALSE)
		return TRUE;
	
	BOOL bBinElevatorCoverLock = FALSE;

	if ( (CMS896AApp::m_lBinElevatorCoverLock == TRUE) && (m_bEnableBinElevatorCoverLock == TRUE) )
	{
		bBinElevatorCoverLock = CMS896AStn::MotionReadInputBit("iBinElevatorClose");
	}
	else
	{
		bBinElevatorCoverLock = TRUE;
	}

	return bBinElevatorCoverLock;
}

BOOL CBinLoader::IsPressureSensorAlarmOn()
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableBL)
		return FALSE;
	if ( m_bNoSensorCheck == TRUE )
		return FALSE;

	m_bPressureSensor = CMS896AStn::MotionReadInputBit("iPressureSensor");
	return m_bPressureSensor;		//TRUE		= ON	= not enough pressure !!
									//FALSE		= OFF	= pressure is OK
}


BOOL CBinLoader::CheckElevatorCover(CONST BOOL bStop)
{
	if (CMS896AApp::m_bIsPrototypeMachine == TRUE)
	{
		return TRUE;
	}

	if ( IsElevatorCoverOpen() == TRUE )
	{
		LONG lOption = 0;		//Klocwork	//v4.29
		CString szText = " ";
		LONG lRetryLimit = 5;	//v4.41T1
		LONG lCount = 0;

		//v4.35T2	//pllm lUMIRAMIC ms109
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		BOOL bRtCheckCoverSensor = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_BL_RT_COVERSENSOR_CHECK);

		if (bStop)
		{
			lOption = 0;
		}
		else
		{
			if (pApp->GetCustomerName() == "OSRAM")		//v4.08
			{
				//Osram would like direct bond cycle stop instead of CONTINUE here
				lOption = SetAlert_Red_Back(IDS_BL_ELEVATOR_COVER_OPEN);
				//return FALSE;	//STOP cycle!!
			}
			else if (bRtCheckCoverSensor)	//v4.35T2	//PLLM Lumiramic MS109
			{
				SetAlert_Red_Back(IDS_BL_ELEVATOR_COVER_OPEN);
			}
			else
			{
				//lOption = SetAlert_Msg_Red_Back(IDS_BL_ELEVATOR_COVER_OPEN, szText, "Yes", "No", NULL, glHMI_ALIGN_LEFT);
				//lOption = SetAlert_Red_Back(IDS_BL_ELEVATOR_COVER_OPEN);
				SetAlert_Red_Back(IDS_BL_ELEVATOR_COVER_OPEN);

				while (1)
				{
					if (IsElevatorCoverOpen() == FALSE)
					{
						break;
					}

					if (lCount >= lRetryLimit)
					{
						break;
					}

					SetAlert_Red_Back(IDS_BL_ELEVATOR_COVER_OPEN);
					lCount = lCount + 1;
				}

				//Y_Home();
				//Z_Home();
				//return FALSE;	//STOP cycle!!
			}
		}

		//if ( lOption != 1 )
		//{
		if ((lCount >= lRetryLimit) || IsElevatorCoverOpen())
			return FALSE;				//Klocwork	//v4.29
		else
			return TRUE;				//v4.41T1	//Nichia
		//}
	}

	return TRUE;
}

BOOL CBinLoader::CheckCover(CONST BOOL bStop)
{
	if (CMS896AStn::m_bEnableBinLoderCoverSensorCheck == FALSE)
	{
		return TRUE;
	}

	if ( IsCoverOpen() == TRUE )
	{
		LONG lOption = 0;				//Klocwork	//v4.29
		CString szText = " ";
		LONG lRetryLimit = 3;
		LONG lCount = 0;

		if (bStop)
		{
			lOption = 0;
		}
		else
		{
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			if (pApp->GetCustomerName() == "OSRAM"|| pApp->GetCustomerName() == "Semitek")		//v4.08
			{
				//Osram would like direct bond cycle stop instead of CONTINUE here
				lOption = SetAlert_Red_Back(IDS_BL_COVER_OPEN);
				//return FALSE;	//STOP cycle!!
			}
			else
			{
				//lOption = SetAlert_Msg_Red_Back(IDS_BL_COVER_OPEN, szText, "Yes", "No", NULL, glHMI_ALIGN_LEFT);
				SetAlert_Red_Back(IDS_BL_COVER_OPEN);

				while (1)
				{
					if (IsCoverOpen() == FALSE)
					{
						break;
					}

					if (lCount >= lRetryLimit)
					{
						break;
					}

					SetAlert_Red_Back(IDS_BL_COVER_OPEN);
					lCount = lCount + 1;
				}

				//Y_Home();
				//Z_Home();
				//return FALSE;
			}
		}

		//if ( lOption != 1 )
		//{
		return FALSE;			//Klocwork	//v4.29
		//}
	}

	return TRUE;
}

/*
VOID CBinLoader::LOG_LoadUnloadTime(CONST CString szMsg, CONST CString szMode)
{
	if ( CMS896AApp::m_bEnableMachineLog == TRUE )
	{
		FILE *fp;
		if ( (fp = fopen(gszUSER_DIRECTORY + "\\History\\Bl_ExTime.log", szMode)) != NULL)
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
VOID CBinLoader::LOG_LoadUnload(CONST INT nMode, CONST LONG lMgzn, CONST LONG lSlot, CONST LONG lBlkNo)
{
	if ( CMS896AApp::m_bEnableMachineLog == TRUE )
	{
		CString szMsg = "";
		switch (nMode)
		{
		case 0:
			szMsg.Format("EXCHANGE :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
			break;
		case 1:
			szMsg.Format("LOAD     :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
			break;
		case 2:
			szMsg.Format("UNLOAD   :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
			break;
		case 3:
			szMsg.Format("PRELOAD-E:  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
			break;
		case 4:
			szMsg.Format("PRELOAD  :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
			break;
		case 5:
			szMsg.Format("CLEAR    :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
			break;
		default:
			break;
		}


		FILE *fp;
		if ( (fp = fopen(gszUSER_DIRECTORY + "\\History\\Bl_ExLoadUnload.log", "a+")) != NULL)
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
//--------------------------------------------//
//	Merge Five Temp  Mag Summary File		  //
//  to create a detailed Mag Summary File	  //
//--------------------------------------------//

BOOL CBinLoader::OutputMagSummaryFileForAllMag()
{
	CString szTime, szSrcPath, szTargetPath;
	CString szMachineNo, szLotNo;	
	CTime curTime(CTime::GetCurrentTime());
	BOOL bOpenSrcFile;
	CStdioFile cfMagFile,cfSrcFile;
	CString szMagFilePath;
	ULONG uFileLength = 0;
	BOOL bNeedAddHeader = FALSE;
	const INT nNoOfFiles = 5;

	CString szFileOpenSequence[nNoOfFiles] = {"3.txt","1.txt","2.txt","4.txt","5.txt"};
	CString szMagazineName[nNoOfFiles] = {"top2","mid 1","btm 1","mid 2","btm 2"};

	szTime = curTime.Format("%d%m%y_%H%M%S");
	
	//Get machine no
	szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
	
	// Get the LotNo
	szLotNo = (*m_psmfSRam)["MS896A"]["LotNumber"];
	
	//File format = MachineNo + LotNo + Time (DD-MM-YY@HH:MM:SS)
	szMagFilePath = m_szMagzFullFilePath + "\\" + 
						"M" + szMachineNo + "_" + szLotNo + "_" +szTime + ".CSV";

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if(pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		CString szGetContent = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\GetContent.txt";
		if (_access(szGetContent,0) == -1)
		{
			HmiMessage("No Bin Information, please Clear Bin First. [ChangeLight(XM)]");
			return TRUE;
		}
		CString szYear, szMonth, szDay, szHour, szMinute, szSecond;
		int nYear, nMonth, nDay, nHour, nMinute, nSecond;
		//Get current time
		CTime theTime = CTime::GetCurrentTime();
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
		//Time
		CString szXMYear;
		szXMYear.Format("%d", theTime.GetYear());
		szXMYear = szXMYear.Right(2);

		CString szXMMonth;
		szXMMonth.Format("%d", theTime.GetMonth());
		if (szXMMonth == "10")
			szXMMonth = "A";
		if (szXMMonth == "11")
			szXMMonth = "B";
		if (szXMMonth == "12")
			szXMMonth = "C";
		
		CString szXMDay;
		szXMDay.Format("%d", theTime.GetDay());
		if ( theTime.GetDay() < 10 )
		{
			szXMDay = "0" + szXMDay;
		}

		//SerialNo
		int nSerial;
		CString szSerial;
		BOOL bXMOpen;
		CStdioFile cfSerial;
		CString szGetSerialNo = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\GetSerialNo.txt";
		if (_access(szGetSerialNo,0) != -1)
		{
			bXMOpen = cfSerial.Open(szGetSerialNo, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if (bXMOpen)
			{
				cfSerial.ReadString(szSerial);
				nSerial = atoi((LPCTSTR)szSerial);
				if (nSerial > 998)
				{

					szSerial = "001";
					cfSerial.Close();
					remove(szGetSerialNo);
				}
				else
				{
					nSerial ++;
					szSerial.Format("%d",nSerial);
					cfSerial.SeekToBegin();
					cfSerial.WriteString(szSerial);
					if( szSerial.GetLength()<2)
						szSerial = "00" + szSerial;
					if( szSerial.GetLength()<3)
						szSerial = "0"  + szSerial;
					cfSerial.Close();
				}
			}
		}
		else
		{
			bXMOpen = cfSerial.Open(szGetSerialNo, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if (bXMOpen)
			{
				cfSerial.WriteString("1");
			}
			szSerial = "001";
			cfSerial.Close();
		}

		//MachineNo
		CString szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
		szMachineNo = szMachineNo.Right(3);
		//FileName
		CString szXMSummaryName = "XSor" +szMachineNo + "C" + szSerial + szXMYear + szXMMonth + szXMDay + szHour + szMinute;
		CString szXMPath        = m_szMagzFullFilePath + "\\" + szXMSummaryName + ".txt";
		CString szBackUpPath    = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\MagSummary";
		CString szBackUpPathName = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\MagSummary\\"  + szXMSummaryName + ".txt";
		
		CreateDirectory(szBackUpPath, NULL);
		CStdioFile cfXM;
		CStdioFile cfRead;

		bXMOpen = cfXM.Open(szXMPath, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
		if (bXMOpen)
		{
			cfXM.WriteString("CASSETENAME," + szXMSummaryName + "\n");
			CString szOutTime = theTime.Format("%Y/%m/%d %H:%M");
			cfXM.WriteString("OUTTIME," + szOutTime + "\n");
			cfXM.WriteString("\n");
			cfXM.WriteString("No,SN,Barcode,LotNo\n");

			bXMOpen = cfRead.Open(szGetContent,CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
			if(bXMOpen)
			{
				BOOL bRead;
				CString szContent;
				do
				{	
					bRead = cfRead.ReadString(szContent);
					cfXM.SeekToEnd();
					cfXM.WriteString(szContent);
					cfXM.WriteString("\n");
				}
				while (bRead);
				cfRead.Close();
				remove(szGetContent);
			}
			cfXM.Close();
			CopyFile(szXMPath,szBackUpPathName,FALSE);
		}
		return TRUE;
	}

	if  (cfMagFile.Open(szMagFilePath,CFile::modeCreate|CFile::modeWrite) == TRUE)
	{

		for (INT i=0 ;i<nNoOfFiles ; i++)
		{
			BYTE *pbData;
			CString szSrcFilePath = BL_TEMP_MAG_NAME + szFileOpenSequence[i];
			bOpenSrcFile = cfSrcFile.Open(szSrcFilePath,CFile::modeRead);

			if (i == 0 && bOpenSrcFile == FALSE)
			{
				bNeedAddHeader = TRUE;
			}

			if (bOpenSrcFile == TRUE)
			{	
				// Writing the Header if the Full Magazine info is not exist
				if (i == 1 && bNeedAddHeader == TRUE)
					cfMagFile.WriteString(BL_HISTORY_TITLE);

				if (i != 0)
				{
					CString szTemp;
					cfMagFile.WriteString(szMagazineName[i].MakeUpper()+"\n");
					//dummy read to remove header
					cfSrcFile.ReadString(szTemp);
				}
					
				uFileLength = (ULONG) cfSrcFile.GetLength();
				pbData = new BYTE[uFileLength];

				// Get the data from the src file
				uFileLength = cfSrcFile.Read(pbData,uFileLength);
				cfMagFile.Write(pbData,uFileLength);
				
				delete [] pbData;
				cfSrcFile.Close();

				//Remove the temp files after Merging except the full magazine log
				DeleteFile(szSrcFilePath);
			}
		}
		cfMagFile.Close();

	}
	
	return TRUE;
}

/*
BOOL CBinLoader::UpdateExArmUnloadFrameMagazineStatus(LONG lMgzn, LONG lSlot, BOOL bBinFull, BOOL bClearBin, BOOL lFullStatus, BOOL bIsEmptyFrame, BOOL bBurnInUnload)
{
	CString str, szText1, szText2;
	INT i = 0;
	
	CString szBarcode = GetCurrMgznSlotBC();

	if( bBinFull==TRUE )
    {
		BL_DEBUGBOX("Bin full - clear bin");

		if (bClearBin && !CreateOutputFileWithClearBin(lMgzn, lSlot, bFullStatus, m_lExArmBufferBlock, GetCurrMgznSlotBC()))
		{
			return FALSE;
		}

		switch(m_lOMRT)
		{
			case BL_MODE_A:
			case BL_MODE_F:		//MS100 8mag config		//v3.82
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = m_lExArmBufferBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				
				if (bClearBin)	//v2.67
				{
					m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
					m_stMgznRT[m_lCurrMgzn].m_lTransferSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_UNUSE;
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				}

				break;

			case BL_MODE_B:
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = m_lExArmBufferBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				break;

			case BL_MODE_C:
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_FULL;
				break;

			case BL_MODE_D:
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = m_lExArmBufferBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
			
//				if( GetNullMgznSlot(lMgzn, lSlot)==FALSE )
//				{
//					return FALSE;
//				}
//				str.Format("Load filmframe from M%d, S%d to M%d, S%d", lMgzn, lSlot, m_lCurrMgzn, m_lCurrSlot+1);
//				BL_DEBUGMESSAGE(str);
//				
//				if ( LoadFrameFromSlotToSlot(bBurnInUnload, lMgzn, lSlot, m_lCurrMgzn, m_lCurrSlot) == FALSE )
//				{
//					return FALSE;
//				}
//				m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_FULL;

				break;

			case BL_MODE_E:		//v3.45
			case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = m_lExArmBufferBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				
				if (bClearBin)
				{
					m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				}

				break;
		}


		if (m_lOMRT == BL_MODE_D)
		{
			szText1.Format("S%d", lSlot+1);
			szText2.Format("S%d", m_lCurrSlot+1);

			str = "BL FilmFrame is loadeded from " + 
					GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + " to " +
				 	GetMagazineName(m_lCurrMgzn).MakeUpper() + ", " + szText2 + ", BLBarcode = " + szBarcode;
		}
		else
		{
			szText1.Format("S%d, B%d", lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
			str = "#1 BL FilmFrame is unloaded to " + 
					GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + " (F) , BLBarcode = " + szBarcode;
		}
	}
	else
	{
		szText1.Format("S%d, B%d", lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		str = "#2 BL FilmFrame is unloaded to " + 
					GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + ", BLBarcode = " + szBarcode;

		//v4.48A15	//Cree HuiZhou
		CString szLog;
		szLog.Format(", Bin=%ld, TO-EMPTY=%d, Mgzn=%d, Slot=%d", 
			m_lExArmBufferBlock, bIsEmptyFrame, lMgzn+1, lSlot+1);
		str = str + szLog;

		//v2.67
		if (bIsEmptyFrame)
		{
			CString szLog;
			szLog.Format("UpdateExArmUnloadFrameMgznStatus Mgzn=%ld, Slot=%ld to UNUSE", lMgzn+1, lSlot+1);
			BL_DEBUGBOX(szLog);

			m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_UNUSE;
		}
		// temp to comment it	//v4.48A15	//RE-enable for Cree HuiZhou MS899 DLA-180 ExArm
		else
		{
			m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_ACTIVE;
		}
	}

	SetStatusMessage(str);
    BL_DEBUGBOX(str);

	if( bBurnInUnload==TRUE )
    {
		BOOL bTemp = FALSE;
		switch( m_lOMRT )
		{
			case BL_MODE_A:
			case BL_MODE_B:
			case BL_MODE_E:		//v3.45
			case BL_MODE_F:		//MS100 8mag 150bins config		//v3.82
			case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
				for( i=0; i<MS_BL_MGZN_NUM; i++)
				{
					if( m_stMgznRT[i].m_lMgznState==BL_MGZN_STATE_FULL )
					{
						ResetMagazine(i);
						m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
					}
				}
				break;

			case BL_MODE_C:
				if( GetLoadMgzSlot(bTemp, m_lExArmBufferBlock)==FALSE )
				{
					ResetMgznByPhyBlock(m_lExArmBufferBlock);
				}
				break;

			case BL_MODE_D:
				break;
		}
    }

	return TRUE;
}
*/

LONG CBinLoader::GenerateSpecialCommunicationFileForCree(LONG lLoadBlk)
{
	if (m_bGenBinFullFile == FALSE)
	{
		return TRUE;
	}

	BL_DEBUGBOX("Cree: GenerateSpecialCommunicationFileForCree");		//v4.39T4

	//v4.50A5	//support dynamic grademapping
	LONG lBinNo = lLoadBlk;
	if (IsMapDetectSkipMode())
	{
		lBinNo = (LONG)(CMS896AStn::m_WaferMapWrapper.GetOriginalGrade((UCHAR)(lLoadBlk + CMS896AStn::m_WaferMapWrapper.GetGradeOffset()))
						- CMS896AStn::m_WaferMapWrapper.GetGradeOffset());
	}

	CStdioFile ctStdFile;
	CString szFilePath, szFilename, szBlkID;
	
	szFilePath = (*m_psmfSRam)["MS896A"]["OutputFilePath"];
	szBlkID.Format("%d", lBinNo);

	if (lBinNo < 10)
		szBlkID = "00" + szBlkID;
	else if (lBinNo < 100)
		szBlkID = "0" + szBlkID;

	szFilename = "BIN_FULL_"+ szBlkID + ".txt";
	szFilename = szFilePath + "\\" + szFilename; 

	if (ctStdFile.Open(szFilename, CFile::modeCreate|CFile::modeWrite) == FALSE)
	{
		SetErrorMessage("Fail to write the communication file, Path: " + szFilePath + " Blk ID:" + szBlkID);
		return FALSE;	
	}
	
	szBlkID.Format("%d", lBinNo);
	ctStdFile.WriteString(szBlkID);
	ctStdFile.Close();

    if (m_lOMRT == BL_MODE_D)	//v4.17T1	//Cree China new ModeD 145 bins support for MS899
	{
		for(INT i=0; i<MS_BL_MGZN_NUM; i++) 
		{
			if( m_stMgznRT[i].m_lMgznUsage==BL_MGZN_USAGE_MIXED )
			{
				ResetMagazineEmpty(i);	//Reset only 1st EMPTY & 2nd FULL slots in new Mode-D
			}
		}
	}
	else
	{
		for(INT i=0; i<MS_BL_MGZN_NUM; i++) 
		{
			if( m_stMgznRT[i].m_lMgznUsage==BL_MGZN_USAGE_EMPTY )
			{
				ResetMagazine(i);
			}
		}
	}


	//v4.50A5
	BL_DEBUGBOX("Cree: Generate BIN File");

	CStdioFile ctBinFile;
	szFilename = "BIN.txt";
	szFilename = szFilePath + "\\" + szFilename; 

	if (ctBinFile.Open(szFilename, CFile::modeCreate|CFile::modeWrite) != FALSE)
	{
		szBlkID.Format("%ld = %ld", lLoadBlk, lBinNo);
		ctBinFile.WriteString(szBlkID);
		ctBinFile.Close();

		BL_DEBUGBOX("Cree: Generate BIN File Done at: " + szFilename);
		CMSLogFileUtility::Instance()->MS_LogOperation("CREE: BIN file created at: " + szFilename);
	}

	return TRUE;
}


BOOL CBinLoader::UploadBackupOutputBinSummary()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	INT nCol = -1;
	CStdioFile ctStdFile;
	CString szFilename = "";
	CString szStr = "";
	CString szFieldName = "", szContent = "";
	CString szBackupSummaryFilePath = "", szOutputSummaryFilePath = "";
	CString szBackupToLocalSummaryPath = "";
	BOOL bIsGeneralFormat = FALSE;

	// default check lextar file
	szFilename = BL_BIN_SUMMARY_INFO_FILE_PATH;

	if (_access(szFilename, 0) == -1)
	{
		// else check other customer file
		szFilename = BL_BIN_SUMMARY_INFO_FILE_PATH_GENERAL;
	
		if (_access(szFilename, 0) == -1)
		{
			return FALSE;
		}

		bIsGeneralFormat = TRUE;
	}

	if (ctStdFile.Open(szFilename, CFile::modeRead) == FALSE)
	{
		return FALSE;	
	}

	while(ctStdFile.ReadString(szStr))
	{
		nCol = szStr.Find(",");
		if (nCol != -1)
		{
			szFieldName = szStr.Left(nCol);
			szContent = szStr.Right(szStr.GetLength() - nCol -1);
		}

		if (szFieldName == BL_BIN_BACKUP_SUMMARY_PATH_IND)
		{
			szBackupSummaryFilePath = szContent;
		}
		else if (szFieldName == BL_BIN_OUTPUT_SUMMARY_PATH_IND)
		{
			szOutputSummaryFilePath = szContent;
		}
		else if (szFieldName == BL_BIN_BACKUP_LOCAL_SUMMARY_PATH_IND)
		{
			szBackupToLocalSummaryPath = szContent;
		}
	}

	ctStdFile.Close();

	if (szBackupSummaryFilePath.IsEmpty() == FALSE && szOutputSummaryFilePath.IsEmpty() == FALSE)
	{
		if (pApp ->GetCustomerName() != "ChangeLight(XM)")
		{
			if (CopyFile(szBackupSummaryFilePath, szOutputSummaryFilePath, FALSE) == FALSE)
			{
				return FALSE;
			}
		}

		// copy a backup to local harddisk
		if (szBackupToLocalSummaryPath.IsEmpty() == FALSE)
		{
			if (CopyFile(szBackupSummaryFilePath, szBackupToLocalSummaryPath, FALSE) == FALSE)
			{
				return FALSE;
			}
		}

		DeleteFile(szFilename);
		DeleteFile(MSD_SUMMARY_DATA_FILE);
	}

	if (bIsGeneralFormat == TRUE)
	{
		if (pApp != NULL)
		{
			pApp->SearchAndRemoveFiles(BL_BIN_SUMMARY_PATH, 0 , TRUE);
		}
	}

	return TRUE;
}


LONG CBinLoader::DualBufferAutoClearBin(CBinGripperBuffer *pGripperBuffer)
{
	BL_DEBUGBOX("Bin full - clear bin (Dual Buffer Exchange)");

	LONG lFullMgzn = pGripperBuffer->GetMgznNo();
	LONG lFullSlot = pGripperBuffer->GetSlotNo();
	LONG lBufferBlock = pGripperBuffer->GetBufferBlock(); //m_clUpperGripperBuffer.GetBufferBlock();
	LONG lFullStatus = FALSE;
	if (IsMSAutoLineMode())
	{
		CString szBCName = GetCassetteSlotBCName(lFullMgzn, lFullSlot);
		lFullStatus = GetAvailableOutputSlot_AutoLine(szBCName, lBufferBlock, lFullMgzn, lFullSlot);
	}
	else
	{
		lFullStatus = GetFullMgznSlot(lFullMgzn, lFullSlot, TRUE);		//Disable FULL mag alarm
	}
	if (lFullStatus == FALSE)
	{
        return FALSE;
	}

	LONG lCurrMgzn = 0, lCurrSlot = 0;

	if (IsMSAutoLineMode())
	{
		lCurrMgzn = pGripperBuffer->GetMgznNo();
		lCurrSlot = pGripperBuffer->GetSlotNo();
	}
	else
	{
		GetMgznSlot(lBufferBlock, lCurrMgzn, lCurrSlot);
	}

	//xxx log value and status and barcode.
	CString szCurrBC = m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot];
	CString szFullBC = m_stMgznRT[lFullMgzn].m_SlotBCName[lFullSlot];

	CString szTemp;
	szTemp.Format("Inside Dual Buffer AutoClearBin Mgzn=%d(%d), Slot=%d(%d) BC = %s(%s)",
		lCurrMgzn, lCurrSlot+1, lFullMgzn, lFullSlot+1, szCurrBC, szFullBC);
	CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog(szTemp);
	szTemp.Format("Up Buffer %d BC %s, Low Buffer %d BC %s, BT %d BC %s",
		lBufferBlock, m_clUpperGripperBuffer.GetBufferBarcode(), 
		m_clLowerGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBarcode(), GetCurrBinOnBT(), m_szBinFrameBarcode);
	CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog(szTemp);
/*
	if (szCurrBC.IsEmpty())
	{
		szCurrBC = m_clUpperGripperBuffer.GetBufferBarcode();
		if (szCurrBC.IsEmpty())
		{
			szCurrBC = m_clLowerGripperBuffer.GetBufferBarcode();
		}
		szTemp.Format("Dual Buffer Auto Clear Bin, BC = %s", szCurrBC);
		CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog(szTemp);
	}
*/

	if (!CreateOutputFileWithClearBin(lFullMgzn, lFullSlot, lFullStatus, lBufferBlock, szCurrBC))
	{
		HouseKeeping(m_lReadyPos_X, FALSE);
		return FALSE;
	}

	if (!IsMSAutoLineMode())
	{
		SaveBarcodeData(lBufferBlock, szCurrBC, lCurrMgzn, lCurrSlot);
	}

	pGripperBuffer->SetBinFullOutputFileCreated(TRUE);
	INT	nConvID;
	IPC_CServiceMessage stMsg;
	LONG lBinNo = m_stMgznRT[m_lCurrMgzn].m_lSlotBlock[m_lCurrSlot];

	stMsg.InitMessage(sizeof(LONG), &lBinNo);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("GetBinNumber"), stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,100*60*1000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	ULONG ulBindieCount = 0;
	stMsg.GetMsg(sizeof(ULONG), &ulBindieCount);

	CString szCassettePos = GetCassettePositionName(m_lCurrMgzn);

	//8011
//	SendBinTableUnloadEvent_8011(m_lCurrMgzn, m_lCurrSlot, m_szBCName, szCassettePos, ulBindieCount);

	//8012
//	SendBinTableUnloadCompletedEvent_8012(m_lCurrMgzn, m_lCurrSlot, m_szBCName, szCassettePos);

	return TRUE;
}


BOOL CBinLoader::CreateOutputFileWithClearBin(LONG lMgzn, LONG lSlot, BOOL bFullStatus, ULONG ulBlkID, CString szBarCodeName)
{
	CString str;

	str.Format("Bin full (DBuffer) - clear bin (Mgzn=%ld, Slot=%ld)", lMgzn + 1, lSlot + 1);
	BL_DEBUGBOX(str);
			
	if (!ClearBinFrameCounter(ulBlkID, szBarCodeName))
	{
		SetErrorMessage("Clear Bin Frame Counter fails.");
		return FALSE;

	}

	BL_DEBUGBOX("Clear bin - OK");

	//Log into History file
	LogFrameFullInformation(TRUE, lMgzn, lSlot + 1, szBarCodeName);

	//Output Magazine summary file
	if ((bFullStatus == BL_MGZ_IS_FULL) && (m_bEnableMagzFullFile == TRUE))
	{
		//Only for Mode F
		OutputMagzFullSummaryFile(BL_MGZ_TOP_1, FALSE);
	}

	return TRUE;
}



BOOL CBinLoader::UpdateDualBufferUnloadFrameMagazineStatus(LONG lMgzn, LONG lSlot, BOOL bBinFull, BOOL bClearBin, BOOL lFullStatus, BOOL bIsEmptyFrame, BOOL bBurnInUnload, CBinGripperBuffer *pGripperBuffer)
{
	CString str, szText1, szText2;
	LONG lTempBufferBlock = 0;

	CString szBarcode = IsMSAutoLineMode() ? GetCassetteSlotBCName(lMgzn, lSlot) : pGripperBuffer->GetBufferBarcode();
	lTempBufferBlock = pGripperBuffer->GetBufferBlock();

	if (!(IsAutoLoadUnloadQCTest() && ((lMgzn == BL_MGZ_TOP_1) || (lMgzn == BL_MGZ_TOP_2) ||
		(IsMSAutoLineStandloneMode() && (lMgzn == BL_MGZ_MID_2)))) && 
		((lTempBufferBlock <=0) || (lTempBufferBlock > MS_MAX_BIN)))
	{
		SetErrorMessage("Clear Bin Error: The Block to be cleared is Zero!");
        return FALSE;
	}

	//v4.50A21		//Cree HZ
	LONG lOrigGrade = 0;
	if (IsMapDetectSkipMode())	
	{
		lOrigGrade = (LONG)(CMS896AStn::m_WaferMapWrapper.GetOriginalGrade((UCHAR)(m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] + CMS896AStn::m_WaferMapWrapper.GetGradeOffset()))
							- CMS896AStn::m_WaferMapWrapper.GetGradeOffset());
	}

	if (bBinFull == TRUE)
    {
		//for autobond, do not clear bin again
		if (!pGripperBuffer->IsBinFullOutputFileCreated() && bClearBin && !CreateOutputFileWithClearBin(lMgzn, lSlot, lFullStatus, lTempBufferBlock, szBarcode))
		{
			return FALSE;
		}

		pGripperBuffer->SetBinFullOutputFileCreated(TRUE);
		//only Mode F
		if (IsMSAutoLineMode())
		{
			m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_FULL;

			//Add SN&LOT ID for SIS Host if unload full bin to magazine
			m_stMgznRT[lMgzn].m_SlotSN[lSlot] =  GetCustomOutputFileName();   //Serial Number (SN)
			m_stMgznRT[lMgzn].m_SlotLotNo[lSlot] =  (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTNUMBER];  //Lot Number
			SECS_UpdateCassetteSlotInfo(lMgzn, lSlot + 1);
		}
		else
		{
			m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lTempBufferBlock;
			m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = szBarcode;
			m_stMgznRT[lMgzn].m_lSlotWIPCounter[lSlot] = pGripperBuffer->GetUnloadDieGradeCount();

			if (bClearBin)
			{
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
			}
		}


/*
		switch(m_lOMRT)
		{
			case BL_MODE_A:
			case BL_MODE_F:		//MS100 8mag config		//v3.82
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lTempBufferBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = szBarcode;
				
				if (!IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())
				{
					m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_FULL;
				}
				else
				{
					if (bClearBin)	//v2.67
					{
						m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
						m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
					}
				}

				break;

			case BL_MODE_B:
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lTempBufferBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				break;

			case BL_MODE_C:
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_FULL;
				break;

			case BL_MODE_D:
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lTempBufferBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
				break;

			case BL_MODE_E:		//v3.45
			case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lTempBufferBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				
				if (bClearBin)
				{
					m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				}
				break;

			case BL_MODE_H:		//MS100 4mag 100bins config		//v4.31T10
				str.Format("Mode-H: (Mgzn=%ld, Slot=%ld) update slot status to #FULL", lMgzn+1, lSlot+1);
				BL_DEBUGBOX(str);
				m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_FULL;
				break;
		}


		if (m_lOMRT == BL_MODE_D)
		{
			szText1.Format("S%d", lSlot+1);
			szText2.Format("S%d", m_lCurrSlot+1);

			str = "BL FilmFrame is loadeded from " + 
					GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + " to " +
				 	GetMagazineName(m_lCurrMgzn).MakeUpper() + ", " + szText2 + ", BLBarcode = " + szBarcode;
		}
		else
		{
*/
			if (lOrigGrade != 0)	//v4.50A21	//Cree HZ
				szText1.Format("S%d, B%d(%ld)", lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot], lOrigGrade);
			else
				szText1.Format("S%d, B%d", lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
			str = "#3 BL FilmFrame is unloaded to " + 
					GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + " (F) , BLBarcode = " + szBarcode;
//		}
	}
	else
	{
		//update for WIP Counter
		if (!IsMSAutoLineMode())
		{
			m_stMgznRT[lMgzn].m_lSlotWIPCounter[lSlot] = pGripperBuffer->GetUnloadDieGradeCount();
		}

		if (lOrigGrade != 0)	//v4.50A21	//Cree HZ
			szText1.Format("S%d, B%d(%ld)", lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot], lOrigGrade);
		else
			szText1.Format("S%d, B%d", lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		str = "#4 BL FilmFrame is unloaded to " + 
					GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + ", BLBarcode = " + szBarcode;

		if (bIsEmptyFrame && (m_lOMRT != BL_MODE_H))	//v4.34T1	//Yealy Mode-H
		{
			CString szLog;
			szLog.Format("UpdateDBufferUnloadFrameMgznStatus Mgzn=%ld, Slot=%ld to UNUSE", lMgzn+1, lSlot+1);
			BL_DEBUGBOX(szLog);

			m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_EMPTY;
		}
	}

	SetStatusMessage(str);
    BL_DEBUGBOX(str);

	if( bBurnInUnload==TRUE )
    {
		BOOL bTemp = FALSE;
		switch( m_lOMRT )
		{
			case BL_MODE_A:
			case BL_MODE_B:
			case BL_MODE_E:		//v3.45
			case BL_MODE_F:		//MS100 8mag 150bins config		//v3.82
			case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			case BL_MODE_H:		//MS100 4mag 100bins config		//v4.31T10	//Yearly MS100Plus
				for (LONG i = 0; i < MS_BL_MGZN_NUM; i++)
				{
					if( m_stMgznRT[i].m_lMgznState==BL_MGZN_STATE_FULL )
					{
						ResetMagazine(i);
						m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
					}
				}
				break;

			case BL_MODE_C:
				if( GetLoadMgzSlot(bTemp, lTempBufferBlock)==FALSE )
				{
					ResetMgznByPhyBlock(lTempBufferBlock);
				}
				break;

			case BL_MODE_D:
				break;
		}
    }

	return TRUE;
}

BOOL CBinLoader::AlignBinFrameOnTable_MS90()
{
	if (!IsMS90())
	{
		return TRUE;
	}

	if (!m_bEnableMS90TablePusher)
	{
		return TRUE;
	}
	//HmiMessage("Andrew: new Frame ALIGN mehtod start .....");

	SetMS90BufPusherZ(TRUE);
	Sleep(1000);

	SetMS90BufPusherX(TRUE);
	Sleep(500);

	if (!IsMS90BufPusherXAtLimitPos())
	{
		Sleep(500);
	}

	SetMS90BufPusherX(FALSE);
	Sleep(500);

	SetMS90BufPusherX(TRUE);
	Sleep(500);

	if (!IsMS90BufPusherXAtLimitPos())
	{
		Sleep(500);
	}

	SetMS90BufPusherX(FALSE);
	Sleep(1000);

	SetMS90BufPusherZ(FALSE);
	Sleep(1000);

	if (!IsMS90BufPusherZAtSafePos())
	{
		Sleep(1000);
		if (!IsMS90BufPusherZAtSafePos())
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBinLoader::BinLoaderUnloadJamMessage(BOOL bBurnInUnload, BOOL bUseBT2)
{
	LONG lReturn = 0;
	LONG lUnloadJamRetryCount = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


	if( bBurnInUnload == TRUE || m_bDisableBL == TRUE)
	{
		return TRUE;
	}

	if (bUseBT2)
	{
		if (pApp->GetCustomerName() == "Cree")		//v4.445T2
		{
			SetMotionCE(TRUE, "Bin Loader BT2 Gripper Jam (BinLoaderUnloadJamMessage)");
		}

		SetLoaderX2Power(FALSE);	//v4.33T2	//Cree HuiZhou prefers to off motor to clear the buffer table 

		while(1)
		{
			if (lUnloadJamRetryCount > BL_JAM_UNLOAD_RETRY_LIMIT)
			{
				return FALSE;
			}

			lUnloadJamRetryCount++;

			lReturn = BL_OK_CANCEL(HMB_BL_PUT_JAM_FRAME, IDS_BL_UNLOAD_FRAME);
			//m_bFrameJam	= CMS896AStn::MotionIsNegativeLimitHigh(BL_AXIS_X, &m_stBLAxis_X);

			//v3.86
			if (lReturn == FALSE)
			{
				if (IsFrameJam2() == TRUE)
				{
					SetAlert_Red_Yellow(IDS_BGP_LOW_GRIPPER_JAM_UNLOAD);
					//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
					SetErrorMessage("BL Gripper 2 Jam quit");
				}
				else if (IsFrameOutOfMgz() == TRUE)
				{
					SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
					SetErrorMessage("BL 2 Frame is out of magazine quit");
				}
				return FALSE;
			}
			

			if (IsFrameJam2() == TRUE)
			{
				SetAlert_Red_Yellow(IDS_BGP_LOW_GRIPPER_JAM_UNLOAD);
				//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL 2 Gripper Jam");
				continue;
			}
			
			if (IsFrameOutOfMgz() == TRUE)
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
				SetErrorMessage("BL 2 Frame is out of magazine");
				continue;
			}

			//v4.33T1	//Osram false alarm issue whtn frame already pulled into slot manually
			if (!m_bNoSensorCheck && IsFrameInClamp2())
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_DETECTED_IN_BL_ERR_HANDLE);
				SetErrorMessage("BL 2 frame is detected in gripper2");
				continue;
			}

			SetLoaderX2Power(TRUE);		//v4.33T2	//Cree HuiZhou 

			// Return to allow sequence to continue
			return TRUE;
		}
	}
	else
	{
		if (pApp->GetCustomerName() == "Cree")		//v4.445T2
		{
			SetMotionCE(TRUE, "Bin Loader BT Gripper Jam (BinLoaderUnloadJamMessage)");
		}
		SetLoaderXPower(FALSE);		//v4.33T2	//Cree HuiZhou prefers to off motor to clear the buffer table 

		while(1)
		{
			if (lUnloadJamRetryCount > BL_JAM_UNLOAD_RETRY_LIMIT)
			{
				return FALSE;
			}

			lUnloadJamRetryCount++;

			lReturn = BL_OK_CANCEL(HMB_BL_PUT_JAM_FRAME, IDS_BL_UNLOAD_FRAME);
			//m_bFrameJam	= CMS896AStn::MotionIsNegativeLimitHigh(BL_AXIS_X, &m_stBLAxis_X);

			//v3.86
			if (lReturn == FALSE)
			{
				if (IsFrameJam() == TRUE)
				{
					SetAlert_Red_Yellow(IDS_BGP_UP_GRIPPER_JAM_UNLOAD);
					//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
					SetErrorMessage("BL Gripper Jam quit");
				}
				else if (IsFrameOutOfMgz() == TRUE)
				{
					SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
					SetErrorMessage("BL Frame is out of magazine quit");
				}
				return FALSE;
			}
			

			if (IsFrameJam() == TRUE)
			{
				SetAlert_Red_Yellow(IDS_BGP_UP_GRIPPER_JAM_UNLOAD);
				//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				continue;
			}
			
			if (IsFrameOutOfMgz() == TRUE)
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
				SetErrorMessage("BL Frame is out of magazine");
				continue;
			}

			//v4.33T1	//Osram false alarm issue whtn frame already pulled into slot manually
			if (!m_bNoSensorCheck && IsFrameInClamp())
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_DETECTED_IN_BL_ERR_HANDLE);
				SetErrorMessage("BL frame is detected in gripper");
				continue;
			}

			SetLoaderXPower(TRUE);		//v4.33T2	//Cree HuiZhou 

			// Return to allow sequence to continue
			return TRUE;
		}
	}

	return TRUE;
}

//Temp used for Dual Buffer machine
BOOL CBinLoader::BinLoaderUnloadJamMessage_DB(CBinGripperBuffer *pGripperBuffer, BOOL bBurnInUnload)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG lReturn = 0;
	LONG lUnloadJamRetryCount = 0;
	LONG lJamRetryLimit = BL_JAM_UNLOAD_RETRY_LIMIT;

	if (bBurnInUnload || m_bDisableBL)
	{
		return TRUE;
	}

	SetGripperPower(pGripperBuffer, FALSE);		//v4.33T2	//Cree HuiZhou prefers to off motor to clear the buffer table 

	while (1)
	{
		if (lUnloadJamRetryCount > lJamRetryLimit)
		{
			return FALSE;
		}

		lUnloadJamRetryCount++;

		lReturn = 0;//Disable Ok Button and do not alert this alarm again!
		SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_BUFFER_TO_MGZ);
		
		if (lReturn == FALSE)
		{
			INT nCount = 0;
			while (IsFrameJam(pGripperBuffer->GetFrameJamSensorName()))		//shiraishi01
			{
				SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_UNLOAD);

				nCount++;
				if (nCount > 3)
				{
					break;
				}
			}

			nCount = 0;
			while (IsFrameOutOfMgz())	//shiraishi01
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
				SetErrorMessage("BL Frame is out of magazine quit");

				nCount++;
				if (nCount > 3)
				{
					break;
				}
			}

			return FALSE;
		}
		
		if (IsFrameJam(pGripperBuffer->GetFrameJamSensorName()))
		{
			SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_UNLOAD);
			continue;
		}
		else if (IsFrameOutOfMgz() == TRUE)
		{
			SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
			SetErrorMessage("BL Frame is out of magazine");
			continue;
		}
		

		if (lReturn  == FALSE)
		{
			// Return to allow safe stoppage of the machine
			return FALSE;
		}
		else
		{
			if (!m_bNoSensorCheck && IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()))		//v3.84
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_DETECTED_IN_BL_ERR_HANDLE);
				SetErrorMessage("BL frame is detected in gripper");
				continue;
			}

			// Return to allow sequence to continue
			SetGripperPower(pGripperBuffer, TRUE);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBinLoader::IsExArmInPickPlacePos()
{
	BOOL bReturn = TRUE;

	if (m_bDisableBL)
	{
		return TRUE;
	}

	BOOL bBinExArm	= (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["ExChgArmExist"];
	if ( bBinExArm == TRUE )
	{
		GetEncoderValue();

		if ( (m_lEnc_Arm < m_lExArmPickPos - BL_EXARM_PICK_PLACE_SAFE_POS_TOLARENCE)
			&& (m_lEnc_Arm > m_lExArmPlacePos + BL_EXARM_PICK_PLACE_SAFE_POS_TOLARENCE) )
		{
			SetAlert_Red_Yellow(IDS_BL_EXARM_NOT_SAFE);
			SetErrorMessage("Exchange Arms are not in Safe Position");

			return FALSE;
		}
	}
	
	return TRUE;
}


// not allow customer to reset magzine if die count > 0
BOOL CBinLoader::IsMagazineCanReset(ULONG lMgzn, BOOL bIsCheckAll)
{
	if (CMS896AStn::m_bEnableResetMagzCheck == FALSE)
	{
		return TRUE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (bIsCheckAll == TRUE)
	{
		if (pApp->CheckIsAllBinClearedNoMsg() == FALSE)
		{
			return FALSE;
		}

		return TRUE;
	}

	switch( m_lOMRT )
	{
		case BL_MODE_A:
		case BL_MODE_F:		//MS100 8mag 150bins config		//v3.82
			if (m_stMgznRT[lMgzn].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
			{
				if (pApp->CheckIsAllBinClearedNoMsg() == FALSE)
				{
					return FALSE;
				}
			}
			break;

		case BL_MODE_E:
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			if (m_stMgznRT[lMgzn].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
			{
				if (pApp->CheckIsAllBinClearedNoMsg() == FALSE)
				{
					return FALSE;
				}
			}
			break;
	}

	return TRUE;
}


LONG CBinLoader::LogItems(LONG lEventNo)
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

LONG CBinLoader::GetLogItemsString(LONG lEventNo, CString& szMsg)
{
	CString szMagazineName;
	CString szTemp;

	switch(lEventNo)
	{
	case BT_UNLOAD_POS_X:
		szMsg.Format("Bin Table Unload Pos X,%d", m_lBTUnloadPos_X);
		break;

	case BT_UNLOAD_POS_Y:
		szMsg.Format("Bin Table Unload Pos Y,%d", m_lBTUnloadPos_Y);
		break;

	case BIN_FRAME_REALIGN:
		if (m_bRealignBinFrame)
		{
			szMsg = "Bin Frame Align,On";
		}
		else 
		{
			szMsg = "Bin Frame Align,Off";
		}
		break;

	case BIN_FRAME_REALIGN_COUNT:
		szMsg.Format("Bin Frame Realign Count,%d", m_lBTAlignFrameCount);
		break;

	case BIN_FRAME_REALIGN_DELAY:
		szMsg.Format("Bin Frame Realign Delay,%d", m_lBTAlignFrameDelay);
		break;

	case BINTABLE_VACUUM:
		if (m_bUseBinTableVacuum)
		{
			szMsg = "BinTable Vacuum,On";
		}
		else 
		{
			szMsg = "BinTable Vacuum,Off";
		}
		break;

	case BINTABLE_VACUUM_DELAY:
		szMsg.Format("BinTable Vacuum Delay,%d", m_lBTVacuumDelay);
		break;

	case EXCHANGE_ARM_VACUUM_SETTLE_DELAY:
		szMsg.Format("Exchange Arm Vacuum Settle Delay,%d", m_lVacSettleDelay);
		break;

	case BIN_GRIPPER_PREUNLOAD_POS:
		szMsg.Format("Bin Gripper PreUnload Position,%d", m_lPreUnloadPos_X);
		break;

	case BIN_GRIPPER_LOAD_MAG_POS:
		szMsg.Format("Bin Gripper Load Magazine Position,%d", m_lLoadMagPos_X);
		break;

	case BIN_GRIPPER_UNLOAD_MAG_POS:
		szMsg.Format("Bin Gripper Unload Magazine Position,%d", m_lUnloadMagPos_X);
		break;

	case BIN_GRIPPER_UNLOAD_POS:
		szMsg.Format("Bin Gripper Unload Position,%d", m_lUnloadPos_X);
		break;

	case BIN_GRIPPER_READY_POS:
		szMsg.Format("Bin Gripper Ready Position,%d", m_lReadyPos_X);
		break;

	case BIN_GRIPPER_BC_POS:
		szMsg.Format("Bin Gripper Barcode Position,%d", m_lBarcodePos_X);
		break;

	case BIN_GRIPPER_UNLOAD_OFFSET:
		szMsg.Format("Bin Gripper Unload Offset,%d", m_lUnloadOffsetX);
		break;

	case BIN_GRIPPER_LOAD_IN_CLAMP_OFFSET_X:
		szMsg.Format("Bin Gripper Load In Clamp Offset,%d", m_lLoadSearchInClampOffsetX);
		break;		

	case EXCHANGE_ARM_PICK_POS:
		szMsg.Format("Exchange Arm Pick Position,%d", m_lExArmPickPos);
		break;

	case EXCHANGE_ARM_PLACE_POS:
		szMsg.Format("Exchange Arm Place Position,%d", m_lExArmPlacePos);
		break;

	case BIN_SCAN_BARCODE:
		if (m_bUseBarcode)
		{
			szMsg = "Bin Barcode,On";
		}
		else
		{
			szMsg = "Bin Barcode,Off";
		}
		break;

	case BIN_CHECK_BARCODE:
		if (m_bCheckBarcode)
		{
			szMsg = "Bin Check Barcode,On";
		}
		else
		{
			szMsg = "Bin Check Barcode,Off";
		}
		break;

	case BL_MAGAZINE_TOP_SLOT_POS:
		szMagazineName = GetMagazineName(m_lSetupMagPos);
		szTemp.Format("%d", m_stMgznRT[m_lSetupMagPos].m_lTopLevel);
		szMsg = "Bin Magazine Top Slot Position (" + szMagazineName+ ")," + szTemp;
		break;

	case BL_MAGAZINE_CENTER_Y_POS:
		szMagazineName = GetMagazineName(m_lSetupMagPos);
		szTemp.Format("%d", m_stMgznRT[m_lSetupMagPos].m_lMidPosnY);
		szMsg = "Bin Magazine Center Y Position (" + szMagazineName+ ")," + szTemp;
		break;

	case BL_MAGAZINE_SLOT_PITCH:
		szMagazineName = GetMagazineName(m_lSetupMagPos);
		szTemp.Format("%d", m_stMgznRT[m_lSetupMagPos].m_lSlotPitch);
		szMsg = "Magazine Slot Pitch (" + szMagazineName+ ")," + szTemp;
		break;

	case ELEVATOR_Y_POS:
		szMsg.Format("Elevator Ready Position Y,%d", m_lReadyPosY);
		break;

	case ELEVATOR_Z_POS:
		szMsg.Format("Elevator Ready Position Z,%d", m_lReadyLvlZ);
		break;

	case ELEVATOR_CHECK:
		if (m_bCheckElevatorCover)
		{
			szMsg = "Check Elevator Cover,On";
		}
		else
		{
			szMsg = "Check Elevator Cover,Off";
		}
		break;
	}

	return 1;
}


BOOL CBinLoader::DB_PreloadLevel()
{
	BOOL bBufferLevel = BL_BUFFER_LOWER;
	if (m_bDualBufferPreloadLevel == BL_BUFFER_UPPER)
	{
		bBufferLevel = BL_BUFFER_UPPER;
	}

	return bBufferLevel;
}

CBinGripperBuffer *CBinLoader::DB_GetPreloadGripperBuffer()
{
	return (m_bDualBufferPreloadLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
}

CBinGripperBuffer *CBinLoader::DB_GetUnloadGripperBuffer()
{
	return (m_bDualBufferPreloadLevel == BL_BUFFER_UPPER) ? &m_clLowerGripperBuffer : &m_clUpperGripperBuffer;
}

LONG CBinLoader::DB_PreloadBlock()
{
	CBinGripperBuffer *pGripperBuffer = DB_GetPreloadGripperBuffer();
	LONG lBufferBlock = pGripperBuffer->GetBufferBlock();
	return lBufferBlock;
}

LONG CBinLoader::GetCurrBinOnBT()
{
	return m_lBTCurrentBlock;
}

CString CBinLoader::GetCurrMgznSlotBC()
{
	return m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot];
}

VOID CBinLoader::DecodeSubBinSetting()
{
	if (CMS896AApp::m_bEnableSubBin == FALSE)
		return;

	if (_access("c:\\mapsorter\\userdata\\SubBin.txt",0) != -1)
	{
		CStdioFile cfRead;
		//LONG lFormatCorrect = 1;
//		int nBlk;
//		int nBlkNo[5];
		if (cfRead.Open("c:\\mapsorter\\userdata\\subbin.txt",CFile::modeRead| CFile::typeText )==TRUE)
		{	
			CString szLine;
			int n;
			BOOL bFormatCorrect = TRUE;
			int nNoOfBlk;
			int nNoOfGrade;
			int nBlk[5];
			int nGrade[5][4];

			//Find Blk & Grade No
			cfRead.ReadString(szLine);
			n = szLine.Find(",");
			if (n != -1)
				nNoOfBlk = atoi((LPCTSTR)(szLine.Mid(n+1,1)));
			else
				HmiMessage("SubBin Setting Wrong1");
			CMSLogFileUtility::Instance()->MS_LogOperation("SubBin Blk No," + szLine.Mid(n+1,1));
			
			cfRead.ReadString(szLine);
			n = szLine.Find(",");
			if (n != -1)
				nNoOfGrade = atoi((LPCTSTR)(szLine.Mid(n+1,1)));
			else
				HmiMessage("SubBin Setting Wrong2");
			CMSLogFileUtility::Instance()->MS_LogOperation("SubBin Grade No," + szLine.Mid(n+1,1));
			
			if (nNoOfBlk > 5)
				HmiMessage("SubBin Setting Wrong,Blk > 5");

			if (nNoOfBlk > 4)
				HmiMessage("SubBin Setting Wrong,Blk > 4");
			m_nNoOfSubBlk = nNoOfBlk;
			m_nNoOfSubGrade = nNoOfGrade;
			//Get each grade
			for (int k = 1; k <= nNoOfBlk ; k ++)
			{
				cfRead.ReadString(szLine);
				n = szLine.Find(",");
				if (n != -1)
					nBlk[k] = atoi((LPCTSTR)(szLine.Mid(n+1,1)));
				m_nSubBlk[k] = nBlk[k];
				szLine = szLine.Mid(n+1);
				for (int j = 1; j<= nNoOfGrade; j++)
				{
					if (j == 1)
					{
						n = szLine.Find(",");
						if (n != -1)
							szLine = szLine.Mid(n+1);
						n = szLine.Find(",");
						if (n != -1)
							nGrade[k][j] = atoi((LPCTSTR)(szLine.Mid(n+1,1)));
						CMSLogFileUtility::Instance()->MS_LogOperation("SubBin Grade1," + szLine.Mid(n+1,1));
						szLine = szLine.Mid(n+3);
					}
					else
					{
						n = szLine.Find(",");
						if (n != -1)
							nGrade[k][j] = atoi((LPCTSTR)(szLine.Left(n)));
						else
							nGrade[k][j] = atoi((LPCTSTR)(szLine));
						CMSLogFileUtility::Instance()->MS_LogOperation("SubBin Grade," + szLine.Left(n));
						szLine = szLine.Mid(n+1);	
					}
					m_nSubGrade[k][j] = nGrade[k][j];
				}
			}
		}
	}
	else
	{
		HmiMessage("Cannot Find SubBin Setting File from,c:\\mapsorter\\userdata\\subbin.txt");
	}
}

BOOL CBinLoader::GenerateEmptyFrameTextFile()	//Matthew 10312018
{
	int nYear, nMonth, nDay, nHour, nMinute, nSecond;
	CString szYear, szMonth, szDay, szHour, szMinute, szSecond;
	CStdioFile Log;
	CString szPath, szLine, szPath2;

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
	
	CString szBarcode = m_szBinFrameBarcode;
	
	szPath = "c:\\MapSorter\\UserData\\OutputFile\\BinLoaderEmptyFrame\\" + szBarcode + "_" + szYear + szMonth + szDay + ".txt";
	szPath2 = m_szEmptyFramePath + "\\" + szBarcode + "_" + szYear + szMonth + szDay + ".txt";
	
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
		_mkdir("c:\\MapSorter\\UserData\\OutputFile\\BinLoaderEmptyFrame");
		Log.Open(szPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite); //open and create file
		Log.SeekToEnd(); //overwrite?
	}
	
	

	//Barcode
	szLine.Format("BarCode No,%s\n\n", szBarcode);
	Log.WriteString(szLine);

	szLine.Format("Date,%s-%s-%s\n", szYear, szMonth, szDay);
	Log.WriteString(szLine);

	szLine.Format("Time,%s:%s:%s\n", szHour, szMinute, szSecond);
	Log.WriteString(szLine);

	//Get PKG Filename
	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];

	szLine.Format("BinTableName,%s\n", szPKGFilename);
	Log.WriteString(szLine);

	//BINCode or ulBlkId
	szLine.Format("BINCode,BIN%d\n", m_lBTCurrentBlock);
	Log.WriteString(szLine);

	//Get machine no
	CString szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
	
	szLine.Format("Machine,%s\n", szMachineNo);
	Log.WriteString(szLine);

	Log.Close(); //close the file

	CopyFile(szPath /*Source*/, szPath2 /*Location*/, FALSE);
	return 1;
}


BOOL CBinLoader::SetGripperJamAlarm(CBinGripperBuffer *pGripperBuffer, UINT UpperGripperAlarmUnCode)
{
	if (pGripperBuffer->IsUpperBuffer() == TRUE)
	{
		SetAlert_Red_Yellow(UpperGripperAlarmUnCode);
		SetErrorMessage("UPPER BL Gripper Jam");
	}
	else
	{
		SetAlert_Red_Yellow(UpperGripperAlarmUnCode + 1); // +1 is same alarm for lower grippper
		SetErrorMessage("LOWER BL Gripper Jam");
	}
	return TRUE;
}


BOOL CBinLoader::SetFrameNotDetectedAlarm(CBinGripperBuffer *pGripperBuffer, UINT UpperGripperAlarmUnCode)
{
	if (pGripperBuffer->IsUpperBuffer() == TRUE)
	{
		SetAlert_Red_Yellow(UpperGripperAlarmUnCode);
		SetErrorMessage("Frame is not detected in UPPER BL Gripper");
	}
	else
	{
		SetAlert_Red_Yellow(UpperGripperAlarmUnCode + 1); // +1 is same alarm for lower grippper
		SetErrorMessage("Frame is not detected in LOWER BL Gripper");
	}
	return TRUE;
}