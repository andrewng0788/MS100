/////////////////////////////////////////////////////////////////
// BH_Common.cpp : Common functions of the CBondHead class
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
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "FileUtil.h"
#include  "io.h"
#include "MS_SecCommConstant.h"
#include "WaferPr.h"
#include "WT_SubRegion.h"
#include "BinLoader.h"
#include "WaferMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CBondHead::InitVariable(VOID)
{
	m_dEjtXYRes					= GetChannelResolution(MS896A_CFG_CH_EJECTOR_X) * 1000.0; // M/Count ==> um/count
	m_dEjRes					= GetChannelResolution(MS896A_CFG_CH_EJECTOR);
	m_dEjElevatorRes			= GetChannelResolution(MS896A_CFG_CH_EJ_ELEVATOR);

	m_lBondDieCount				= 0;
	m_lBondCount				= 0;
	m_bES101Configuration		= FALSE;		//v4.24		//ES101
	m_bMS100EjtXY				= FALSE;		//v4.41T5	//MS100 EJT XY
	m_bShowAlarmPage			= FALSE;	//	default value

	m_bHomeSnr_Z				= FALSE;
	m_bHomeSnr_T				= FALSE;
	m_bHomeSnr_Ej				= FALSE;
	m_bHomeSnr_EjElevatorZ		= FALSE;
	m_bHomeSnr_EjT				= FALSE;
	m_bHomeSnr_EjCap			= FALSE;
	m_bHomeSnr_BinEj			= FALSE;
	m_bHomeSnr_BinEjElevatorZ	= FALSE;
	m_bHomeSnr_BinEjT			= FALSE;
	m_bHomeSnr_BinEjCap			= FALSE;

	m_bPosLimitSnr_Z			= FALSE;
	m_bNegLimitSnr_Z			= FALSE;
	m_bPosLimitSnr_Z2			= FALSE;
	m_bNegLimitSnr_Z2			= FALSE;
	m_bPosLimitSnr_T			= FALSE;
	m_bNegLimitSnr_T			= FALSE;
	m_bBHFan					= FALSE;
	m_bMS60BHZFan1				= FALSE;
	m_bMS60BHZFan2				= FALSE;
	m_bMS60BHTUpFan1			= FALSE;
	m_bMS60BHTUpFan2			= FALSE;
	m_bMS60BHTLowFan1			= FALSE;
	m_bMS60BHTLowFan2			= FALSE;
	m_bNGPickDnSnr				= FALSE;
	m_bNGPickUpSnr				= FALSE;
	m_bTensionCheck				= FALSE;
	m_bTapeEmpty				= FALSE;
	m_bCheckK1K2				= FALSE;
	m_bCheckEjPressureSnr		= FALSE;
	m_bCheckBH1PressureSnr		= FALSE;	//v4.59A26
	m_bCheckBH2PressureSnr		= FALSE;	//v4.59A26
	//m_pServo_Z				= NULL;
	//m_pServo_T				= NULL;
	//m_pServo_Ej				= NULL;
	//m_pDcServo_Z				= NULL;

	m_bSel_Z					= TRUE;
	m_bSel_Z2					= FALSE;	//FALSE by default
	m_bSel_T					= TRUE;
	m_bSel_Ej					= TRUE;
	m_bSel_BinEj				= TRUE;
	m_bSel_PBTZ					= FALSE;

	m_bIsPowerOn_Z				= TRUE;
	m_bIsPowerOn_Z2				= TRUE;
	m_bIsPowerOn_T				= TRUE;

	m_bIsPowerOn_Ej				= FALSE;
	m_bIsPowerOn_EjCap			= FALSE;
	m_bIsPowerOn_EjT			= FALSE;
	m_bIsPowerOn_EjX			= FALSE;
	m_bIsPowerOn_EjY			= FALSE;
	m_bIsPowerOn_EjElevator		= FALSE;

	m_bIsPowerOn_BinEj			= FALSE;
	m_bIsPowerOn_BinEjElevator	= FALSE;
	m_bIsPowerOn_BinEjCap		= FALSE;
	m_bIsPowerOn_BinEjT			= FALSE;

	m_bIsArm2Exist				= FALSE;	//FALSE by default
	m_bIsPowerOn_NGPickT		= FALSE;
	m_bIsPowerOn_PBTZ			= FALSE;
	m_bNuMotionSystem			= FALSE;
	m_bMS100DigitalAirFlowSnr	= FALSE;
	m_bEnableDAFlowRunTimeCheck	= FALSE;
	m_bMS50ChgColletZT			= FALSE;

	m_lEnc_Z					= 0;
	m_lEnc_Z2					= 0;
	m_lEnc_T					= 0;
	m_lEnc_Ej					= 0;
	m_lEnc_EjX					= 0;
	m_lEnc_EjY					= 0;
	m_lEnc_EjElevatorZ			= 0;
	m_lEnc_Z					= 0;
	m_lEnc_ChgColletZ			= 0;
	m_lEnc_EjCap				= 0;
	m_lEnc_EjT					= 0;
	m_lEnc_BinEj				= 0;
	m_lEnc_BinEjElevatorZ		= 0;
	m_lEnc_BinEjCap				= 0;
	m_lEnc_BinEjT				= 0;
	
	m_lCmd_Ej					= 0;
	m_lCmd_EjElevatorZ			= 0;
	m_lCmd_EjCap				= 0;
	m_lCmd_EjT					= 0;
	m_lCmd_BinEj				= 0;
	m_lCmd_BinEjElevatorZ		= 0;
	m_lCmd_BinEjCap				= 0;
	m_lCmd_BinEjT				= 0;

	m_lColletHoleEnc_T			= 0;
	m_lColletHoleEnc_Z			= 0;
	m_lColletHoleEnc_Z2			= 0;

	m_bUpdateOutput				= FALSE;
	m_bHome_Z					= FALSE;
	m_bHome_Z2					= FALSE;
	m_bHome_T					= FALSE;
	m_bHome_Ej					= FALSE;
	m_bHome_EjX					= FALSE;
	m_bHome_EjY					= FALSE;
	m_bHome_EjElevator			= FALSE;
	m_bHome_NGPick				= FALSE;
	m_bHome_PBTZ				= FALSE;
	
	m_bComm_Z					= FALSE;
	m_bComm_Z2					= FALSE;
	m_bComm_T					= FALSE;
	m_bComm_Ej					= FALSE;
	m_bComm_EjCap				= FALSE;
	m_bComm_EjT					= FALSE;
	//m_bComm_PBTZ				= FALSE;
	m_bComm_BinEj				= FALSE;
	m_bComm_BinEjCap			= FALSE;
	m_bComm_BinEjT				= FALSE;

	m_bIsTJoystickOn = FALSE;
	m_bIsZJoystickOn = FALSE;
	m_bIsEJoystickOn = FALSE;

	//m_lCurLevel_Z				= 0;
	//m_lCurLevel_Z2			= 0;
	//m_lCurPos_T				= 0;
	m_lCurLevel_Ej				= 0;
	m_lCurLevel_EjT				= 0;
	m_lCurLevel_EjCap			= 0;
	m_lCurLevel_BinEj			= 0;
	m_lCurLevel_BinEjT			= 0;
	m_lCurLevel_BinEjCap		= 0;
	
	m_dCycleTime				= 0.0;
	m_dAvgCycleTime				= 0.0;
	m_ulCycleCount				= 0;
	m_lBH1MarkCount				= -1;
	m_lBH2MarkCount				= -1;
	m_bDoColletSearch			= FALSE;
	//m_bSearchColletNow1			= FALSE;
	//m_bSearchColletNow2			= FALSE;

	m_lBondPos_T				= -11219;
	m_lPreBondPos_T				= -11000;
	m_lPickPos_T				= 745;
	m_lPrePickPos_T				= -300;
	m_lCleanColletPos_T = -5000;
	m_lChangeCarrierPos_T = 0;

	m_lPickLevel_Z				= -7470;
	m_lBondLevel_Z				= -7680;
	m_lReplaceLevel_Z = -7600;
	m_lSwingLevel_Z				= -2500;
	m_lSwingOffset_Z			= 4000;
	m_lMinEjectOffset			= 0;
	m_lAutoCleanCollet_Z		= 0;
	m_lAutoCleanCollet_T		= 0;
	m_lAutoCleanOffsetZ			= 0;
	m_lZ1BondLevelBT_X			= 0;
	m_lZ1BondLevelBT_Y			= 0;
	m_lZ2BondLevelBT_X			= 0;
	m_lZ2BondLevelBT_Y			= 0;
	memset(m_stPadLevels, 0, sizeof(m_stPadLevels));
	m_bBeginPadsLevel			= FALSE;
	m_lWPadsNum					= 0;
	m_lWPadsLevelOffsetZ1		= 0;
	m_lWPadsLevelOffsetZ2		= 0;
	m_lBT2OffsetX				= 0;
	m_lBT2OffsetY				= 0;
	m_lBT2OffsetZ				= 0;
	m_lNGPickIndexT				= 0;

	m_lPickLevel_Z2				= -7470;
	m_lBondLevel_Z2				= -7680;
	m_lReplaceLevel_Z2 = -7600;
	m_lSwingLevel_Z2			= -2500;
	m_lSwingOffset_Z2			= 4000;

	m_lEjectLevel_Ej			= 1500;
	m_lPreEjectLevel_Ej			= 1000;
	m_lStandbyLevel_Ej			= 300;
	m_lContactLevel_Ej			= 0;		//v3.94T4

//	m_lUpLevel_El				= 0;
	m_lStandby_EJ_Cap			= 0;
	m_lUpLevel_EJ_Cap			= 0;
	m_lDnOffset_EJ_Cap			= 0;
	m_lCDiePos_EjX				= 0;
	m_lCDiePos_EjY				= 0;

	// Ejector Elv Level
	m_lStandby_EJ_Elevator		= 0;
	m_lUpLevel_EJ_Elevator		= 0;
	// Ejector Theta Level
	m_lStandby_EJ_Theta			= 0;
	m_lUpLevel_EJ_Theta			= 0;

	m_bLearnStandby_EJ_Pin		= TRUE;
	m_bLearnStandby_EJ_Cap		= TRUE;
	m_bLearnStandby_EJ_Elevator = TRUE;
	m_bLearnStandby_EJ_Theta	= TRUE;
	m_bLearnUpLevel_EJ_Pin		= TRUE;
	m_bLearnUpLevel_EJ_Cap		= TRUE;
	m_bLearnUpLevel_EJ_Elevator = TRUE;
	m_bLearnUpLevel_EJ_Theta	= TRUE;

	// Bin Ejector
	m_lStandby_Bin_EJ_Cap		= 0;
	m_lUpLevel_Bin_EJ_Cap		= 0;
	m_lStandby_Bin_EJ_Elevator	= 0;
	m_lUpLevel_Bin_EJ_Elevator	= 0;
	m_lStandby_Bin_EJ_Theta		= 0;
	m_lUpLevel_Bin_EJ_Theta		= 0;
	m_lStandby_Bin_EJ_Pin		= 0;
	m_lUpLevel_Bin_EJ_Pin		= 0;

	m_lBinTableDelay			= 0;
	m_lPRDelay					= 0;
	m_lWTTDelay					= 0;
	m_lWTSettlingDelay			= 0;
	m_lWeakBlowOffDelay			= 0;
	m_lHighBlowTime				= 0;

	m_bOnVacuumAtPick			= FALSE;
	m_lArmPickDelay				= 5;
	m_lHeadPickDelay			= 5;
	m_lEjectorUpDelay			= 20;
	m_lPickDelay				= 10;
	m_lArmBondDelay				= 5;
	m_lEjectorDownDelay			= 0;
	m_lHeadBondDelay			= 5;
	m_lBondDelay				= 10;
	m_lFloatBlowDelay			= 0;
	m_lHeadPrePickDelay			= 0;
	m_lTCUpPreheatTime			= -1;
	m_lTCDnOnVacDelay			= -1;
	m_lTCUpHeatingTime			= -1;
	m_bTCLayerPick				= FALSE;
	m_lUplookDelay				= 0;
	m_lNeutralDelay				= 0;
#ifdef NU_MOTION
	m_lEjReadyDelay				= 5;		//v3.66		//DBH only
#else
	m_lEjReadyDelay				= 10;
#endif
	//v3.62
	m_lMinBinTableDelay			= 0;
	m_lMinPRDelay				= 0;
	m_lMinWTTDelay				= 0;	
	m_lMinArmPickDelay			= 0;
	m_lMinHeadPickDelay			= 0;	
	m_lMinEjectorUpDelay		= 0;
	m_lMinPickDelay				= 0;	
	m_lMinArmBondDelay			= 0;
	m_lMinEjectorDownDelay		= 0;
	m_lMinHeadBondDelay			= 0;
	m_lMinBondDelay				= 0;
	m_lMinEjReadyDelay			= 0;
	m_lEjVacOffDelay			= 0;
	m_lSyncPickDelay			= 0;	

	m_lUpTime_Ej_Cap			= 10;
	m_lDnTime_Ej_Cap			= 10;
	m_lUpTime_Ej_Theta			= 10;
	m_lDnTime_Ej_Theta			= 10;
	m_lUpTime_BinEj_Theta		= 10;
	m_lDnTime_BinEj_Theta		= 10;
	m_lTime_BinEj				= 10;
	m_lDnTime_BinEj				= 10;
	m_lTime_T					= 50;	//70;
	m_lBondTime_T				= 50;	//70;
	m_lPrePickTime_T			= 50;	//70;
	m_lPickTime_Z				= 30;	//45;
	m_lBondTime_Z				= 30;	//45;
	m_lPickTime_Z2				= 30;	//45;
	m_lBondTime_Z2				= 30;	//45;
	m_lTime_Ej					= 30;	//34;
	m_lSlowUpTime_Ej				= 30;
	m_lPickUpTime_Z				= 30;	//v3.61
	m_lBondUpTime_Z				= 30;	//v3.61
	m_lPickUpTime_Z2			= 30;	//v3.61
	m_lBondUpTime_Z2			= 30;	//v3.61

	m_lPickDriveIn				= 0;
	m_lBondDriveIn				= 0;
	m_lEjectorDriveIn			= 0;
	m_lEjectorLimitType			= 0;

	m_lBlockCameraDelay			= 0;	//v4.57A4

	m_bCleanColletSafetySensor_HMI	= FALSE;
	m_bMainFrontCover_HMI		= FALSE;
	m_bMainFrontLeftCover_HMI	= FALSE;
	m_bMainFrontMiddleCover_HMI	= FALSE;
	m_bMainRearLeftCover_HMI	= FALSE;
	m_bMainRearRightCover_HMI	= FALSE;
	m_bMainLeftCover_HMI		= FALSE;

	m_bCheckMissingDie_Backup	= FALSE;
	m_bCheckColletJam_Backup	= FALSE;
	m_bEjectorCapVacuumOn		= FALSE;
	m_bCheckColletJam			= TRUE;		
	m_bCheckMissingDie			= FALSE;
	m_bPickVacuumOn				= FALSE;
	m_bPickVacuumOnZ2			= FALSE;
	m_bCheckCoverSensor			= TRUE;
	m_bScanCheckCoverSensor		= FALSE;
	m_bEnableFrontCoverLock		= FALSE;
	m_bEnableSideCoverLock		= FALSE;
	m_bEnableBinElevatorCoverLock = FALSE;
	m_bFrontCoverLock			= FALSE;
	m_bSideCoverLock			= FALSE;
	m_bBinElevatorCoverLock		= FALSE;
	m_bUseCTSensorLearn			= FALSE;	//v2.96T4
	m_bUseDefaultDelays			= FALSE;	//v2.97T1
	m_bIsLinearEjector			= FALSE;			
	m_bAutoCleanCollet			= FALSE;	//v3.64
	m_bACCSearchColletHole		= FALSE;
	m_lLiquidExpireTime			= 0;
	m_tSqueezeLiquidTime		= CTime::GetCurrentTime();
	m_bNeutralVacState			= FALSE;	//v3.81
	m_bIsES101NGPickStarted		= FALSE;	//v4.24T10
	m_bEnableMDResponseChecking	= FALSE;	//v3.94
	m_bCheckPr2DCode			= FALSE;	//v4.40T1
	m_lPr2DCodeCheckLimit		= 0;		//v4.41T3
	m_lPr2DCodeCheckCounter		= 0;		//v4.41T3
	m_lUpLookPrStage			= 0;
	m_bUplookResultFail			= FALSE;	//v4.55
	
	m_ulColletCount				= 0;
	m_ulCollet2Count			= 0;
	m_ulCleanCount				= 0;
	m_dLastPickTime				= 0;
	m_ulCleanColletTimeOut		= 0;
	m_ulEjectorCount			= 0;
	m_ulEjectorCountForKOffset	= 0;		//v4.43T8	//Semitek
	m_ulMaxColletCount			= 0;
	m_ulMaxCollet2Count			= 0;
	m_ulMaxEjectorCount			= 0;
	m_ulMaxCleanCount			= 0;

	m_lEjectorKOffset			= 0;
	m_lEjectorKCount			= 0;
	m_lEjectorKOffsetLimit		= 0;
	m_lEjectorKOffsetTotal		= 0;

	m_lEjectorKOffsetForBHZ1MD	= 0;	//v4.52A10
	m_lEjectorKOffsetBHZ1Total	= 0;	//v4.52A10	
	m_lEjectorKOffsetForBHZ2MD	= 0;	//v4.52A10
	m_lEjectorKOffsetBHZ2Total	= 0;	//v4.52A10		
	m_bResetEjKOffsetAtWaferEnd	= 0;
	m_lEjSubRegionKOffset		= 0;
	m_lEjSubRegionKCount		= 0;
	m_lEjSubRegionKOffsetTotal	= 0;
	m_lEjSubRegionSKOffset		= 0;
	m_lEjSubRegionSKCount		= 0;
	m_lEjSubRegionSKOffsetTotal	= 0;
	m_lZ1SmartKOffset			= 0;
	m_lZ1SmartKCount			= 0;
	m_lZ1SmartKOffsetTotal		= 0;
	m_lZ2SmartKOffset			= 0;
	m_lZ2SmartKCount			= 0;
	m_lZ2SmartKOffsetTotal		= 0;
	m_lEjectorKOffsetResetCount = 0;	//M69

	m_lDAFlowBH1KOffset			= 0;
	m_lDAFlowBH1KCount			= 0;
	m_lDAFlowBH1KOffsetLimit	= 0;
	m_lDAFlowBH1KOffsetTotal	= 0;
	m_lDAFlowBH2KOffset			= 0;
	m_lDAFlowBH2KCount			= 0;
	m_lDAFlowBH2KOffsetLimit	= 0;
	m_lDAFlowBH2KOffsetTotal	= 0;

	m_bResetEjKOffsetAtWaferEnd	= FALSE;
	m_lMissingDie_Retry			= 3;
	m_ulColletJam_Retry			= 3;

	m_lMDCycleLimit				= 20;	//Default value by Matthew 20190410
	m_lNGNoDieLimit				= 0;
	m_lMDCycleCount1			= 0;
	m_lMDCycleCount2			= 0;
	m_lMDTotalCount1			= 0;
	m_lMDTotalCount2			= 0;
	m_lCJTotalCount1			= 0;
	m_lCJTotalCount2			= 0;
	m_lCJContinueCounter1		= 0;
	m_lCJContinueCounter2		= 0;
	
	m_lBHUplookPrFailLimit		= 0;

	m_lMDRetry1Counter			= 0;
	m_lMDResetCounter			= 0;
	m_bMDRetryOn				= FALSE;
	m_lMDRetry2Counter			= 0;
	m_lMD2ResetCounter			= 0;
	m_bMD2RetryOn				= FALSE;
	
	m_lCJRetryCounter			= 0;
	m_lCJResetCounter			= 0;
	m_bCJRetryOn				= FALSE;
	m_lCJ2RetryCounter			= 0;
	m_lCJ2ResetCounter			= 0;
	m_bCJ2RetryOn				= FALSE;

	m_bMDRetryUseAutoCleanCollet	= FALSE;
	m_lAutoCleanColletSwingTime	= 100;		//v3.77
	m_lAccLastDropDownTime		= 500;

	m_bEnableCustomCount1		= FALSE;
	m_bEnableCustomCount2		= FALSE;
	m_bEnableCustomCount3		= FALSE;
	m_ulCustomCounter1			= 0;
	m_ulCustomCounter2			= 0;
	m_ulCustomCounter3			= 0;
	m_ulCustomCounter1Limit		= 0;
	m_ulCustomCounter2Limit		= 0;
	m_ulCustomCounter3Limit		= 0;

	m_ulBHZ1DAirFlowSetZeroOffset	= 0;
	m_ulBHZ2DAirFlowSetZeroOffset	= 0;
	m_ulBHZ1DAirFlowBlockValue		= 0;
	m_ulBHZ1DAirFlowUnBlockValue	= 0;
	m_ulBHZ2DAirFlowBlockValue		= 0;
	m_ulBHZ2DAirFlowUnBlockValue	= 0;
	m_ulBHZ1DAirFlowThreshold		= 0;
	m_ulBHZ2DAirFlowThreshold		= 0;
	m_ulBHZ1DAirFlowThresholdCJ		= 0;
	m_ulBHZ2DAirFlowThresholdCJ		= 0;
	m_ulBHZ1DAirFlowValue			= 10;
	m_ulBHZ2DAirFlowValue			= 20;
	m_dBHZ1ThresholdPercent			= 50;
	m_dBHZ2ThresholdPercent			= 50;
	m_lBHZ1ThresholdLimit			= 0;
	m_lBHZ2ThresholdLimit			= 0;
	m_ulBHZ1DAirFlowCleanColletThreshold = 0;
	m_ulBHZ2DAirFlowCleanColletThreshold = 0;
	m_lBHZ1ThresholdUpperLimit = 0;

	//ADC Thermostat fcn
	m_ulBHTThermostatCounter		= 0;
	m_ulBHTThermostatReading		= 0;
	m_dBHTThermostatReading			= 0;
	m_ulBHTThermostatADC			= 0;
	m_dBHZ1UplookCalibFactor	= 0;
	m_dBHZ2UplookCalibFactor	= 0;
	m_bEnableSyncMove			= FALSE;
	m_lSyncDistance				= 0;
	m_dSyncSpeed				= 0;

	m_lT_ProfileType			= 0;
	m_lZ_ProfileType			= 0;
	
	m_lE_ProfileType			= 0;		//Linear Ej
	m_lECap_ProfileType			= 0;
	m_lET_ProfileType			= 0;
	m_lBinE_ProfileType			= 0;
	m_lBinECap_ProfileType		= 0;
	m_lBinET_ProfileType		= 0;
	
	m_lT_CommMethod				= 0;
	m_lZ_CommMethod				= 0;
	m_lE_CommMethod				= 0;

	m_ulPickDieCount			= 0;		// Picked Die
	m_ulBondDieCount			= 0;		// Bonded Die
	m_ulDefectDieCount			= 0;		// Defective Die
	m_ulBadCutDieCount			= 0;		// Bad-cut Die
	m_ulMissingDieCount			= 0;		// Missing Die
	m_ulRotateDieCount			= 0;		// Rotate Die
	m_ulEmptyDieCount			= 0;		// Empty Die
	m_ulTotalSorted				= 0;


	//Current wafer
	m_ulCurrWaferBondDieCount	= 0;		// Bond Die			
	m_ulCurrWaferDefectDieCount	= 0;		// Defective Die	
	m_ulCurrWaferChipDieCount	= 0;		// Chip Die
	m_ulCurrWaferInkDieCount	= 0;		// Ink Die
	m_ulCurrWaferBadCutDieCount	= 0;		// Bad-cut Die		
	m_ulCurrWaferRotateDieCount	= 0;		// Rotate Die		
	m_ulCurrWaferEmptyDieCount	= 0;		// Empty Die		
	m_dCurrWaferSelGradeYield	= 0.0;		// Yield
	m_ulCurrWaferMissingDieCount = 0;

	m_bIsBHLogOpened = FALSE;
	m_bPostBondAtBondCount		= 0;		//v2.61
	m_lPreBondAtPickCount		= 0;		//v3.34

	//BH-Z Open-DAC option		//andrew
	m_lBHZOpenDacValue	= 0;
	m_bEnableOpenDacBondOption = FALSE;	

	m_bFirstCycle				= FALSE;
	m_bFirstCycleAlreadyTakeLog	= FALSE;

	m_bStartDacTTest = FALSE;
	m_lBHZOffsetbyPBEmptyCheck		= 0;
	m_lCurrOffsetZ1byPBEmptyCheck	= 0;
	m_lCurrOffsetZ2byPBEmptyCheck	= 0;
	m_lBHZAutoLearnDriveInLimit		= 0;
	m_bAutoLearningBHZ				= FALSE;
		
	//v3.94T4
	m_lWafDieHeight				= 0;	
	m_lEjCTLevelOffset			= 0;
	m_bRefreshScreen			= FALSE;

	m_lBondHeadDelay = 500;
	m_szSqOfDacFromBondToPick = "0";
	m_szSqOfDacFromPickToBond = "0";
	m_szSumOfDac = "0";

	//v2.83T27
	m_lAutobondStartTime	= 0;
	m_lAutobondTotalTime	= 0;
	m_unDataLogOption		= 0;
	m_bEnableStepMove		= FALSE;
	m_bStartUBHZTest		= FALSE;
	m_bUBHContactSearch		= FALSE;
	m_lUBHMoveTime_Z		= 0;
	m_lUBHEnc_Z				= 0;

	m_lUBHStepOffset_Z		= 0;
	m_lUBHOpenDacOffset_Z	= 0;
	m_lUBHSettlingTime_T	= 10;

	m_bEnableSeparteSettlingBlk_T = FALSE;
	m_lPositionErrLimit_Z = 0;
	m_lUBHSearchSpeed_Z		= 0;
	m_lUBHSearchDriveInSpeed_Z = 0;
	m_lUBHSearchDriveInSample_Z = 0;
	m_lUBHMinPosErrTriggerSample_Z = 0;

	m_bCleanColletToggle = FALSE;		//v3.25T2

	m_bEnableThermalTest = FALSE;
	m_bThermalTestFirstCycle = FALSE;
	m_lCollectHoleCatpureTime = 0;

	m_bEnableMotionLogT		= FALSE;
	m_bEnableMotionLogZ		= FALSE;
	m_bEnableMotionLogZ2	= FALSE;
	m_bEnableMotionLogEJ	= FALSE;
	m_bEnableMotionLogEJTX	= FALSE;
	m_bEnableMotionLogEJTY	= FALSE;
	m_bEnableNuDataLog		= FALSE;	//v4.50A24
	m_bIsDataLogForBHDebug	= FALSE;

	m_bEnableBHZOpenDac		= FALSE;
	m_lBHZ1_PickDAC			= -2000;
	m_lBHZ1_BondDAC			= -2000;
	m_lBHZ2_PickDAC			= -2000;
	m_lBHZ2_BondDAC			= -2000;
	m_lBHZ1_PickDACOffset	= 0;
	m_lBHZ1_BondDACOffset	= 0;
	m_lBHZ2_PickDACOffset	= 0;
	m_lBHZ2_BondDACOffset	= 0;

	m_lWithBHDown			= 0;
	m_bChangeEjtX			= FALSE;
	m_bChangeEjtY			= FALSE;

	m_bChangeCollet2		= FALSE;
	m_bChangeCollet1		= FALSE;

	m_bEnableSyncMotionViaEjCmd = FALSE;
	m_lSyncTriggerValue			= 10;
	m_lSyncZOffset				= 0;

	m_lSoftTouchPickDistance		= 0;
	m_lSoftTouchBondDistance		= 0;
	m_lSoftTouchVelocity		= 1;
	m_szEjSoftTouchMsg			= "";

	CMS896AStn::m_ucMotionAxis			= 0;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
	CMS896AStn::m_bMotionCycleStop		= 0;
	CMS896AStn::m_bMoveDirection		= FALSE;
	CMS896AStn::m_lMotionTestDelay		= 0;
	CMS896AStn::m_lMotionOrigEncPosn	= 0;
	CMS896AStn::m_lMotionTarEncPosn		= 0;
	CMS896AStn::m_lMotionCycle			= 0;
	CMS896AStn::m_lMotionCycleCount		= 0;
	/*m_lMotionTestDist	= 0;
	m_bIsMotionTestBondHeadZ1 = FALSE;
	m_bIsMotionTestBondHeadZ2 = FALSE;
	m_bIsMotionTestPBTZ = FALSE;
	m_bILCContourMoveTest = FALSE;
	m_bIsMotionTestBondArm = FALSE;
	m_bIsMotionTestEjector = FALSE;
	*/

	m_lStartOpenDACCalibValue		= 0;
	m_lCurrOpenDACCalibValue		= 0;
	m_lOpenDACCurrBHZ				= BH_MS100_BHZ1;

	m_bIsOpenDACCalibStart			= FALSE;
	m_bIsOpenDACForceCheckStart		= FALSE;
	m_lOpenDACForceCheckValue		= 0;

	m_dBHZ1_OpenDACCalibRatio		= BH_OPENDAC_CALIBRATION_RATIO;
	m_dBHZ2_OpenDACCalibRatio		= BH_OPENDAC_CALIBRATION_RATIO;

	m_dBHZ1_DACDistanceCalibRatio	= BH_DACDISTANCE_CALIBRATION_RATIO;
	m_dBHZ2_DACDistanceCalibRatio	= BH_DACDISTANCE_CALIBRATION_RATIO;

	m_lDBHBondHeadMode		= 0;	//v3.93		//0=dual-head, 1=BHZ1 only, 2=BHZ2 only

	if (GetChannelResolution(MS896A_CFG_CH_EJECTOR) != 0)		//Linear Ej
	{
		m_lMinEjectOffset = (LONG)(BH_LEJT_TRAVEL_RANGE / GetChannelResolution(MS896A_CFG_CH_EJECTOR));
		if (IsMS60())
		{
			m_lMinEjectOffset = 100;	//v4.54A6	//CKHarry & LeoLam for Semitek
		}
	}

	m_nHmiResetCollet = 0;	//0=none, 1=BHZ1, 2=BHZ2(MS100)


	m_bILCFirstTimeStartup = TRUE;
	m_ctILCBondingLastUpdateTime = CTime::CTime(2000, 12, 25, 12, 25, 00);
	m_lRestartUpdateMinTime = 0;
	m_ctBHTAutoCycleLastMoveTime = CTime::CTime(2000, 12, 25, 12, 25, 00);
	m_lAutoLearnLevelsIdleTime	= 0;
	m_bILCAutoLearn = FALSE;
	m_lILCAutoLearnDelay = 0;
	m_lILCAutoLearnState = BH_T_PICK_TO_BOND;
	m_bStartContour_T = FALSE;

	for (int i = 0; i < BA_CONTOUR_MOVE_BUF_SIZE; i++)
	{
		m_BAMove.fMovePos[i] = 0;
		m_BAMove.fMoveVel[i] = 0;
		m_BAMove.fMoveAcc[i] = 0;
		m_BAMove.fMoveJerk[i] = 0;
	}

	m_bCycleFirstProbe		= TRUE;
	m_bUseMultiProbeLevel	= TRUE;	//Default value to TRUE by Matthew 20190410
	InitVariablesHWD();

	//Nichia Attributes
	m_ucNichiaProcessMode		= 0;
	m_ucNichiaMaxNoOfWafers		= 0;

	m_bDataLogListInNewMode = FALSE;

	m_lBondHeadCoolingSecond	= 0;
	m_lCheckFanCount			= 0;

	m_bMS60DetectBHFan			= FALSE;		//v4.48A33
	m_bMS60ThermalCtrl			= FALSE;
	m_bEnableMS60ThermalCheck	= FALSE;
	m_dRmsRSum = m_dRmsSSum = m_dRmsTSum = 0;
	m_nRmsCount	= 0;
	m_dMS60ThermalUpdateTime = GetTime();
	m_dID = m_dIS = m_dIQ = 0;
	m_dThermalSetValue	= 0;
	m_dMS60Thermal_P	= 0;
	m_dMS60Thermal_I	= 0;
	m_dMS60Thermal_D	= 0;
	m_unMS60TargetIdleTemperature = 0;				//v4.54A5

	m_lAutoCleanAutoLearnZCounter	= 0;
	m_bAutoCleanAutoLearnZLevels	= FALSE;		//v4.49A6
	m_lAGCClampLevelOffsetZ			= 0;
	m_lAGCUplookPRLevelOffsetZ		= 0;
	m_bAutoChgCollet1Fail			= FALSE;		//v4.51A4
	m_bAutoChgCollet2Fail			= FALSE;		//v4.51A4

	m_lMS50ChgCollet1stPos_T		= 0;			//v4.59A45
	m_lMS50NoOfCollets				= 0;			//v4.59A45
	m_lMS50CurrColletIndex			= 0;			//v4.59A45

	//m_lAGCCollet1OffsetX			= 0;
	//m_lAGCCollet1OffsetY			= 0;
	//m_lAGCCollet2OffsetX			= 0;
	//m_lAGCCollet2OffsetY			= 0;
	m_bBHZ1HasDie		= FALSE;
	m_bBHZ2HasDie		= FALSE;
	m_ucAtPickDieGrade	= 0;	//v4.50A9
	m_ulAtPickDieRow = m_ulAtPickDieCol = 0;
	m_lAtBondDieGrade = 0;
	m_lAtBondDieRow = m_lAtBondDieCol = 0;

	m_lCurPos_ChgColletT		= 0;
	m_lCurPos_ChgColletZ		= 0;

	m_lAGCColletStartAng		= 0;
	m_lAGCColletTPitch			= 320; //Hardcode
	m_lAGCHolderUploadZ			= 0;
	m_lAGCHolderInstallZ		= 0;
	m_lAGCHolderBlowDieZ		= 0;
	m_lAGCSetupValue			= 0;
	m_lAGCColletIndex			= 1;

	//BHMark
	m_lAutoUpdateBHPickPosnMode				= 0;
	m_bEnableBHMark_HMI						= FALSE;
	m_bEnableAutoUpdateBHPickPosnMode_HMI	= FALSE;
	m_lCheckBHMarkCycle						= 0;
	m_lCheckBHMarkDelay						= 0;
//	m_bChangeColletDone						= FALSE;

	m_bIsMHCTrained				= FALSE;

	m_bILCLog					= FALSE;
}


VOID CBondHead::CheckHeadBondDelay()
{
	if (m_lPickDelay == m_lBondDelay)
	{
		return;
	}

	Ej_Profile(NORMAL_PROF);

	LONG lSamplingRate = ULTRA_BH_SAMPLE_RATE;
	LONG lTempMotionTime = 0;

	LONG lDnOffset_BIN_EJ_Theta	= m_lUpLevel_Bin_EJ_Theta - m_lStandby_Bin_EJ_Theta;
	//Ej Pin start to move when Ej T within Ej Cap
	//Ej Pin Up Time -  Ej T 1150 CNT motion time
	lTempMotionTime = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_BIN_EJ_T, BH_MP_BIN_E_T_OBW_DEFAULT, lDnOffset_BIN_EJ_Theta, 1150, lSamplingRate, &m_stBHAxis_BinEjT);
	LONG lMinHeadBondDelay = m_lTime_BinEj - lTempMotionTime;
	if (m_lHeadBondDelay < lMinHeadBondDelay)
	{
		m_lHeadBondDelay = lMinHeadBondDelay;
	}

	LONG lDnOffset_EJ_Theta	= m_lUpLevel_EJ_Theta - m_lStandby_EJ_Theta;
	lTempMotionTime = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ_T, BH_MP_E_T_OBW_DEFAULT, lDnOffset_EJ_Theta, 1150, lSamplingRate, &m_stBHAxis_EjT);
	LONG lMinHeadPickDelay	= m_lTime_Ej - lTempMotionTime;
	if (m_lHeadPickDelay < lMinHeadPickDelay)
	{
		m_lHeadPickDelay = lMinHeadPickDelay;
	}

	//Ej Pin arrive standby level when Ej T start to leave Ej Cap
	//Ej Pin Down Time - Ej T 1150 CNT motion time
	lTempMotionTime = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_BIN_EJ_T, BH_MP_BIN_E_T_OBW_DEFAULT, lDnOffset_BIN_EJ_Theta, 1150, lSamplingRate, &m_stBHAxis_BinEjT);
	LONG lMinArmBondDelay	= m_lDnTime_BinEj - lTempMotionTime;
	if (lMinArmBondDelay > 0 && m_lArmBondDelay < lMinArmBondDelay)
	{
		m_lArmBondDelay = lMinArmBondDelay;
	}

	//Make sure Ej Pin does not touch mylar
	if (m_lArmBondDelay < 2)
	{
		m_lArmBondDelay = 2;
	}

	lTempMotionTime = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ_T, BH_MP_E_T_OBW_DEFAULT, lDnOffset_EJ_Theta, 1150, lSamplingRate, &m_stBHAxis_EjT);
	LONG lMinArmPickDelay	= m_lDnTime_Ej - lTempMotionTime;
	if (lMinArmPickDelay > 0 && m_lArmPickDelay < lMinArmPickDelay)
	{
		m_lArmPickDelay = lMinArmPickDelay;
	}
	if (m_lArmPickDelay < 2)
	{
		m_lArmPickDelay = 2;
	}

	if ((m_lHeadPickDelay + m_lPickDelay) > (m_lHeadBondDelay + m_lBondDelay))
	{
		m_lHeadBondDelay = m_lHeadPickDelay + m_lBondDelay - m_lPickDelay;
	}
	else
	{
		m_lHeadPickDelay = m_lHeadBondDelay + m_lBondDelay - m_lPickDelay;
	}

	if (m_lPickDelay < 0)
	{
		m_lPickDelay = 0;
	}

	if (m_lBondDelay < 0)
	{
		m_lBondDelay = 0;
	}

	if (m_lPRDelay < 0)
	{
		m_lPRDelay = 0;
	}

	//andrewng //2020-05-20
	m_lEjectorUpDelay = m_lHeadPickDelay - m_lHeadBondDelay;
	if (m_lEjectorUpDelay < 0)
	{
		m_lEjectorUpDelay = 0;
	}

}


BOOL CBondHead::LoadBhOption(BOOL bLoadPPKG)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	BOOL bExcludeMachineParam	= (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Exclude Machine Param"];	//v4.33T1	//PLSG
	BOOL bManualLoadPkg			= (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Manual Load Pkg"];		//v4.33T1
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	// open config file
	if (pUtl->LoadBPConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetBPConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}
	// retrive data
	// bonding delay

	if ((pApp->GetCustomerName() == "Lumileds") && 
			(pApp->GetProductLine() == "DieFab") && 
			bExcludeMachineParam && bManualLoadPkg)		//v4.33T1	//PLSG
	{
		m_bEnableSyncMotionViaEjCmd	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_Z_ENABLE_SYNC_MOTION_VIA_EJ_CMD]);
		m_ulMaxEjectorCount			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXEJECTOR];
		m_ulMaxColletCount			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET];
		m_ulMaxCollet2Count			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET2];
		m_ulMaxCleanCount			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXCLEAN];
	}
	else
	{
		m_lAutobondTimeInMin		= (*psmf)[BP_DATA][BP_DELAY][BP_MC_ALARM];					//v2.83T27
		m_bUseDefaultDelays			= (BOOL)(LONG)((*psmf)[BP_DATA][BP_DELAY][BP_DEFAULT]);		//v2.97T1
		m_bTCLayerPick				= (BOOL)(LONG)((*psmf)[BP_DATA][BP_DELAY]["TCLayerPicking"]);
		// bond head position
		m_bUseBHAutoLearn			= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_AUTOLEARN]);
		m_bUseCTSensorLearn			= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_CTSENSOR_LEARN]);		//v2.96T4
		m_bEnableSeparteSettlingBlk_T = (BOOL)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_UBHSEPARATESETTLING_T];
		m_bUseEjAutoLearn			= (BOOL)(LONG)((*psmf)[BP_DATA][BP_EJECTOR][BP_EJT_AUTOLEARN]);
		m_lWafDieHeight				= (*psmf)[BP_DATA][BP_EJECTOR][BP_DIEHEIGHT];		//v3.94T4
		// Maximum Count
		m_ulCleanColletTimeOut		= (*psmf)[BP_DATA][BP_COUNT][BP_CLEAN_COLLET_TIME_OUT];
		m_ulMaxEjectorCount			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXEJECTOR];
		m_ulMaxColletCount			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET];
		m_ulMaxCollet2Count			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET2];	//v3.92
		m_ulMaxCleanCount			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXCLEAN];
		m_lEjectorKOffset			= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET];
		m_lEjectorKCount			= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KCOUNT];
		m_lEjectorKOffsetLimit		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_LIMIT];
		m_lEjectorKOffsetForBHZ1MD	= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_BHZ1_MD];
		m_lEjectorKOffsetForBHZ2MD	= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_BHZ2_MD];
		m_lEjSubRegionKOffset		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_KOFFSET];
		m_lEjSubRegionKCount		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_KCOUNT];
		m_lEjSubRegionSKOffset		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_SKOFFSET];
		m_lEjSubRegionSKCount		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_SKCOUNT];
		//v4.53A25
		m_lZ1SmartKOffset			= (*psmf)[BP_DATA][BP_COUNT][BP_Z1_SKOFFSET];
		m_lZ1SmartKCount			= (*psmf)[BP_DATA][BP_COUNT][BP_Z1_SKCOUNT];
		m_lZ2SmartKOffset			= (*psmf)[BP_DATA][BP_COUNT][BP_Z2_SKOFFSET];
		m_lZ2SmartKCount			= (*psmf)[BP_DATA][BP_COUNT][BP_Z2_SKCOUNT];
		
		m_lDAFlowBH1KOffset			= (*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH1_KOFFSET];
		m_lDAFlowBH1KCount			= (*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH1_KCOUNT];
		m_lDAFlowBH1KOffsetLimit	= (*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH1_KOFFSET_LIMIT];
		m_lDAFlowBH2KOffset			= (*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH2_KOFFSET];
		m_lDAFlowBH2KCount			= (*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH2_KCOUNT];
		m_lDAFlowBH2KOffsetLimit	= (*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH2_KOFFSET_LIMIT];
		m_bResetEjKOffsetAtWaferEnd	= (BOOL)(LONG)(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_RESET_WAFEREND];	//v4.43T8
		m_lEjectorKOffsetResetCount = (*psmf)[BP_DATA][BP_COUNT]["Ejector K Offset Reset Count"];//M69
		m_bCheckK1K2				= (BOOL)(LONG)(*psmf)[BP_DATA][BP_COUNT]["Reset K1K2"]; //M70
		m_bAutoChangeColletOnOff    = (BOOL)(LONG)(*psmf)[BP_DATA][BP_COUNT]["Auto Change Collet On Off HMI"];
	
		m_lAutoCleanColletSwingTime	= (*psmf)[BP_DATA][BP_DELAY][BP_BHZ_AUTO_CLEANCOLLET_SWING_TIME];	//v3.77
		m_lAccLastDropDownTime		= (*psmf)[BP_DATA][BP_DELAY][BP_BHZ_ACC_LAST_DROP_DOWN_TIME];

		m_bEnableCustomCount1	= (BOOL)(LONG)(*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_1];
		m_bEnableCustomCount2	= (BOOL)(LONG)(*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_2];
		m_bEnableCustomCount3	= (BOOL)(LONG)(*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_3];
		m_ulCustomCounter1Limit	= (*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_1_LIMIT];
		m_ulCustomCounter2Limit	= (*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_2_LIMIT];
		m_ulCustomCounter3Limit	= (*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_3_LIMIT];

		// Missing die & collet jam count
		m_bCheckColletJam		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_CHECK_COLLETJAM]);		
		m_bCheckMissingDie		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_CHECK_MISSDIE]);
		//m_lMDCycleLimit		= (*psmf)[BP_DATA][BP_COUNT][BP_MAX_CYCLE_MD_LIMIT];	//Default value by Matthew 20190410
		m_lNGNoDieLimit			= (*psmf)[BP_DATA][BP_COUNT]["NG No Die Limit"];
		m_lMissingDie_Retry		= (*psmf)[BP_DATA][BP_COUNT][BP_MAX_MISSDIE_RETRY];
		m_ulColletJam_Retry		= (*psmf)[BP_DATA][BP_COUNT][BP_MAX_COLLETJAM_RETRY];
		m_bAutoCleanCollet		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CLEAN_COLLET]);
		m_bACCSearchColletHole	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_ACC_SEARCH_COLLET_HOLE]);
		m_bNeutralVacState		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_VAC_NEUTRAL_STATE]);
		m_bMDRetryUseAutoCleanCollet= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_MD_RETRY_AUTOCLEAN]);
		m_lLiquidExpireTime		= (*psmf)[BP_DATA][BP_DELAY][BP_BHZ_ACC_LIQUID_EXPIRE_TIME];
		m_ulStartLotCheckCount  = (*psmf)[BP_DATA][BP_DELAY]["Start Lot Check Count"];
		m_lBHUplookPrFailLimit	= (*psmf)[BP_DATA][BP_COUNT][BP_MAX_UPLOOK_PR_FAIL_LIMIT];	//v4.59A5
		// Check Cover Sensor
		m_bScanCheckCoverSensor	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_SCAN_CHECK_COVERSNR]);	
		m_bCheckCoverSensor		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_CHECK_COVERSNR]);	
		m_bCoverSensorAlwaysOn	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_COVER_SENSOR_ON]);			//v4.28T6
		m_bEnableSyncMove		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_SYNC_MOVE][BP_SYNC_MOVE_ENABLE]);
		m_bEnableFrontCoverLock	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_FRONT_COVER_LOCK]);
		m_bEnableSideCoverLock	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_SIDE_COVER_LOCK]);
		m_bEnableBinElevatorCoverLock = (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_BIN_EL_COVER_LOCK]);
		m_bCheckPr2DCode		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_PR_2DCODE]);
		m_lPr2DCodeCheckLimit	= (LONG)((*psmf)[BP_DATA][BP_HEAD][BP_PR_2DCODE_LIMIT]);
		//v4.59A26
		m_bCheckEjPressureSnr	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_CHECK_EJ_PRESSURE]);		
		m_bCheckBH1PressureSnr	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_CHECK_BH1_PRESSURE]);
		m_bCheckBH2PressureSnr	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_CHECK_BH2_PRESSURE]);

		m_bEnableBHZOpenDac		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_BHZ_OPEN_DAC_ENABLE]);
		m_bEnableBHZOpenDac		= FALSE;

		m_bEnableSyncMotionViaEjCmd	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_Z_ENABLE_SYNC_MOTION_VIA_EJ_CMD]);
		//m_bEnableSyncMotionViaEjCmd = FALSE;	//andrewng //2020-05-20

        //4.53D42 //4.54T10
		BOOL bPPKGPickSetting = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Include Pick Setting"];
		if(bLoadPPKG && !bPPKGPickSetting)	//v4.55A6
		{
		}
		else
		{
	   	   m_lSoftTouchBondDistance	= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SOFT_TOUCH_BOND_DISTANCE];
		   m_lSoftTouchPickDistance	= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SOFT_TOUCH_PICK_DISTANCE];
		   m_lSoftTouchVelocity		= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SOFT_TOUCH_VELOCITY];
		}

		//v4.55A6	
		m_lPickDriveIn			= (*psmf)[BP_DATA][BP_HEAD][BP_PICKDRIVEIN];
		m_lBondDriveIn			= (*psmf)[BP_DATA][BP_HEAD][BP_BONDDRIVEIN];
		m_szEjSoftTouch			= (*psmf)[BP_DATA][BP_HEAD][BP_EJ_SOFT_TOUCH];
        
		if (m_szEjSoftTouch == "" || m_szEjSoftTouch.GetLength() == 0 || m_szEjSoftTouch == "0")
		{
			m_szEjSoftTouch == "100";
		}
		//v4.54A6
		m_szEjCapSoftTouch			= (*psmf)[BP_DATA][BP_HEAD][BP_EJCAP_SOFT_TOUCH];
		if (m_szEjCapSoftTouch == "" || m_szEjCapSoftTouch.GetLength() == 0 || m_szEjCapSoftTouch == "0")
		{
			m_szEjCapSoftTouch == "100";
		}

		EjSoftTouch_CMD();
		
		//BondMark
		m_lAutoUpdateBHPickPosnMode = (LONG)((*psmf)[BP_DATA][BP_HEAD]["Auto Update BH Pick Posn"]);
		m_lCheckBHMarkCycle = (*psmf)[BP_DATA][BP_HEAD]["Check BH Mark Cycle"];
		m_lCheckBHMarkDelay = (*psmf)[BP_DATA][BP_HEAD]["Check BH Mark Delay"];

		m_bIsMHCTrained		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD]["Is MHC Trained"]);

		(*m_psmfSRam)["BondHead"]["BHPickPosnMode"] = m_lAutoUpdateBHPickPosnMode;

		BOOL bPPKGDelay = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Include Sort Delay"];
		if( (pApp->GetCustomerName() == CTM_NICHIA) || bPPKGDelay)	//v4.40T5
		{
			m_lPickDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_PICKDELAY];
			m_lArmPickDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_APDELAY];
			m_lHeadPickDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_HPDELAY];
			m_lBondDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_BONDDELAY];
			m_lArmBondDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_ABDELAY];
			m_lHeadBondDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_HBDELAY];
			m_lEjectorUpDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_EJUPDELAY];
			m_lEjectorDownDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_EJDOWNDELAY];
			m_lWeakBlowOffDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_WBOFFDELAY];
			m_lHighBlowTime			= (*psmf)[BP_DATA][BP_DELAY][BP_HBTIME];
			m_lPRDelay				= (*psmf)[BP_DATA][BP_DELAY][BP_PRDELAY];
			m_lWTTDelay				= (*psmf)[BP_DATA][BP_DELAY][BP_WTTDELAY];
			m_lWTSettlingDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_WTSETTLINGDELAY];
			m_lBinTableDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_BTDELAY];			
			m_bUseDefaultDelays		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_DELAY][BP_DEFAULT]);	
			m_lEjVacOffDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_EJVACOFFELAY];	
			m_lHeadPrePickDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_HPPDELAY];		
			m_lSyncPickDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_SYNPICKDELAY];	
			m_lTCUpPreheatTime		= (*psmf)[BP_DATA][BP_DELAY]["TCEjtUpPrDelay"];
			m_lTCDnOnVacDelay		= (*psmf)[BP_DATA][BP_DELAY]["TCDnOnVacDelay"];
			m_lTCUpHeatingTime		= (*psmf)[BP_DATA][BP_DELAY]["TCUpHeatingTime"];
			m_lPreEjectorUpDelay	= (*psmf)[BP_DATA][BP_DELAY][BP_UBHPREEJUPDELAY];

			CMS896AStn::m_ucCommRetry	= (*psmf)[BP_DATA][BP_COMM_RETRY];
			CMS896AStn::m_ulCommInterval= (*psmf)[BP_DATA][BP_COMM_INTERVAL];

			CheckHeadBondDelay();

			m_lSyncTriggerValue		= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SYNC_TRIGGER_VALUE];
			m_lSyncZOffset			= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SYNC_Z_OFFSET];

			if (m_lSyncTriggerValue >= (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
			{
				m_lSyncTriggerValue = m_lEjectLevel_Ej - m_lStandbyLevel_Ej - 1;
			}	

			if (m_lSyncZOffset >= (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
			{
				m_lSyncZOffset = m_lEjectLevel_Ej - m_lStandbyLevel_Ej - 1;
			}
		}
	}

	// close config file
	pUtl->CloseBPConfig();

	//v4.42T8
#ifdef NU_MOTION
	if ( (m_ulMaxEjectorCount > 0) && (m_ulMaxEjectorCount < 1000) )
	{
		m_ulMaxEjectorCount = 1000;
	}
	//2018.4.4 change to 20 from 200
	if ( (m_ulMaxColletCount > 0) && (m_ulMaxColletCount < 20) )	//v4.50A12
	{
		m_ulMaxColletCount = 20;
	}
	if ( (m_ulMaxCollet2Count > 0) && (m_ulMaxCollet2Count < 20) )
	{
		m_ulMaxCollet2Count = 20;
	}
	if ( (m_ulMaxCleanCount > 0) && (m_ulMaxCleanCount < 10) )
	{
		m_ulMaxCleanCount = 10;
	}
#endif

	//v4.00T1
	//NEUTRAL state fcn now moved to Feature instead
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_bNeutralVacState = pApp->GetFeatureStatus(MS896A_FUNC_BH_VAC_NEUTRALSTATE);

	m_bTCLayerPick = m_bTCLayerPick && m_bTEInUse;

	if (!pApp->m_bSuperUser)	//v4.28T6
	{
		m_bCheckCoverSensor		= TRUE;	
	}
	(*m_psmfSRam)["BondHead"]["EnableCoverSensor"] = m_bCheckCoverSensor;

	//v4.40T1	//PLLM REBEL
	BOOL bEnableBHPostSeal	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS);
	if (!bEnableBHPostSeal)
	{
		m_bCheckPr2DCode = FALSE;
	}

	if (!IsEnaAutoChangeCollet() && m_bACCSearchColletHole)		//v4.53A23
	{
		m_bACCSearchColletHole = FALSE;
	}

	m_szPickDriveInDisplay.Format("= %.4f um", m_lPickDriveIn * 0.5);
	m_szBondDriveInDisplay.Format("= %.4f um", m_lBondDriveIn * 0.5);

	if( IsLayerPicking() )
		m_lTCDnOnVacDelay = -1;

	return TRUE;
}


BOOL CBondHead::LoadBhData(VOID)//Only PKG
{
	CString szData;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	BOOL bExcludeMachineParam	= (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Exclude Machine Param"];	//v4.33T1	//PLSG
	BOOL bManualLoadPkg			= (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Manual Load Pkg"];		//v4.33T1
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	// open config file
	if (pUtl->LoadBPConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetBPConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	if ((pApp->GetCustomerName() == "Lumileds") && 
			(pApp->GetProductLine() == "DieFab") && 
			bExcludeMachineParam && bManualLoadPkg)		//v4.33T1	//PLSG
	{
		m_lPickDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_PICKDELAY];
		m_lNeutralDelay			= (*psmf)[BP_DATA][BP_DELAY]["Neutral Delay"];
		m_lUplookDelay			= (*psmf)[BP_DATA][BP_DELAY]["Uplook Delay"];
		m_lArmPickDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_APDELAY];
		m_lHeadPickDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_HPDELAY];
		m_lBondDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_BONDDELAY];
		m_lArmBondDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_ABDELAY];
		m_lHeadBondDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_HBDELAY];
		m_lEjectorUpDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_EJUPDELAY];
		m_lEjectorDownDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_EJDOWNDELAY];
		m_lWeakBlowOffDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_WBOFFDELAY];
		m_lHighBlowTime			= (*psmf)[BP_DATA][BP_DELAY][BP_HBTIME];
		m_lPRDelay				= (*psmf)[BP_DATA][BP_DELAY][BP_PRDELAY];
		m_lWTTDelay				= (*psmf)[BP_DATA][BP_DELAY][BP_WTTDELAY];
		m_lWTSettlingDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_WTSETTLINGDELAY];
		m_lBinTableDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_BTDELAY];			
		m_bUseDefaultDelays		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_DELAY][BP_DEFAULT]);	
		m_lEjVacOffDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_EJVACOFFELAY];	
		m_lHeadPrePickDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_HPPDELAY];		
		m_lSyncPickDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_SYNPICKDELAY];	
		m_lTCUpPreheatTime		= (*psmf)[BP_DATA][BP_DELAY]["TCEjtUpPrDelay"];
		m_lTCDnOnVacDelay		= (*psmf)[BP_DATA][BP_DELAY]["TCDnOnVacDelay"];
		m_lTCUpHeatingTime		= (*psmf)[BP_DATA][BP_DELAY]["TCUpHeatingTime"];
		m_lPreEjectorUpDelay	= (*psmf)[BP_DATA][BP_DELAY][BP_UBHPREEJUPDELAY];

		CheckHeadBondDelay();

		//v4.43T6
		if (m_bEnableSyncMotionViaEjCmd && (m_lEjectorUpDelay < m_lTime_Ej))
		{
			//m_lEjectorUpDelay = m_lTime_Ej;
		}

		m_lSyncTriggerValue		= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SYNC_TRIGGER_VALUE];
		m_lSyncZOffset			= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SYNC_Z_OFFSET];

		if (m_lSyncTriggerValue >= (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
		{
			m_lSyncTriggerValue = m_lEjectLevel_Ej - m_lStandbyLevel_Ej - 1;
		}	

		if (m_lSyncZOffset >= (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
		{
			m_lSyncZOffset = m_lEjectLevel_Ej - m_lStandbyLevel_Ej - 1;
		}
	}
	else
	{
		// retrive data
		// bonding delay
		m_bOnVacuumAtPick		= (BOOL)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_ONVACUUM_AT_PICK];
		m_lPickDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_PICKDELAY];
		m_lNeutralDelay			= (*psmf)[BP_DATA][BP_DELAY]["Neutral Delay"];
		m_lUplookDelay			= (*psmf)[BP_DATA][BP_DELAY]["Uplook Delay"];
		m_lArmPickDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_APDELAY];
		m_lHeadPickDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_HPDELAY];
		m_lBondDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_BONDDELAY];
		m_lFloatBlowDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_FLOATBLOWDELAY];
		m_lArmBondDelay			= (*psmf)[BP_DATA][BP_DELAY][BP_ABDELAY];
		m_lHeadBondDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_HBDELAY];
		m_lEjectorUpDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_EJUPDELAY];
		m_lEjectorDownDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_EJDOWNDELAY];
		m_lWeakBlowOffDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_WBOFFDELAY];
		m_lHighBlowTime			= (*psmf)[BP_DATA][BP_DELAY][BP_HBTIME];
		m_lPRDelay				= (*psmf)[BP_DATA][BP_DELAY][BP_PRDELAY];
		m_lWTTDelay				= (*psmf)[BP_DATA][BP_DELAY][BP_WTTDELAY];
		m_lWTSettlingDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_WTSETTLINGDELAY];
		m_lBinTableDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_BTDELAY];				//v2.83T27
		m_bUseDefaultDelays		= (BOOL)(LONG)((*psmf)[BP_DATA][BP_DELAY][BP_DEFAULT]);		//v2.97T1
		m_lEjVacOffDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_EJVACOFFELAY];				//v3.78
		m_lHeadPrePickDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_HPPDELAY];					//v3.79
		m_lSyncPickDelay		= (*psmf)[BP_DATA][BP_DELAY][BP_SYNPICKDELAY];				//v3.83
		m_lTCUpPreheatTime		= (*psmf)[BP_DATA][BP_DELAY]["TCEjtUpPrDelay"];
		m_lTCDnOnVacDelay		= (*psmf)[BP_DATA][BP_DELAY]["TCDnOnVacDelay"];
		m_lTCUpHeatingTime		= (*psmf)[BP_DATA][BP_DELAY]["TCUpHeatingTime"];

		CMS896AStn::m_bNGPick	= (BOOL)(LONG)(*psmf)[BP_DATA][BP_HEAD]["NGPICK"];		//v4.51A19
		m_lAGCHolderUploadZ		= (LONG)(*psmf)[BP_DATA][BP_HEAD]["AGCHolderUploadZ"];
		m_lAGCHolderInstallZ	= (LONG)(*psmf)[BP_DATA][BP_HEAD]["AGCHolderInstallZ"];
	
		m_lAGCColletStartAng	= (LONG)(*psmf)[BP_DATA][BP_HEAD]["AGCColletStartAng"];
		m_lAGCColletIndex		= (LONG)(*psmf)[BP_DATA][BP_HEAD][AGC_COLLET_INDEX];

		if ((m_lAGCColletIndex <= 0) || (m_lAGCColletIndex > GetCGMaxIndex()))
		{
			m_lAGCColletIndex = 1;
			(*psmf)[BP_DATA][BP_HEAD][AGC_COLLET_INDEX] = m_lAGCColletIndex;
		}

		// bond head position
		m_lWPadsLevelOffsetZ1	= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_PADSLEVEL_OFFSET_Z1];
		m_lWPadsLevelOffsetZ2	= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_PADSLEVEL_OFFSET_Z2];
		m_lZ1BondLevelBT_X		= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_BONDLEVEL_BT_X];
		m_lZ1BondLevelBT_Y		= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_BONDLEVEL_BT_Y];
		m_lZ2BondLevelBT_X		= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_BONDLEVEL_BT_X];
		m_lZ2BondLevelBT_Y		= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_BONDLEVEL_BT_Y];
		m_bZ1BondLevelBT_R		= (BOOL)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_Z1_BONDLEVEL_BT_R];
		m_bZ2BondLevelBT_R		= (BOOL)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_Z2_BONDLEVEL_BT_R];
		for(INT i=0; i<MAX_WAFFLE_PADS; i++)	//	load
		{
			CString szTemp;
			szTemp.Format("Pad %d", i);
			m_stPadLevels[i].m_bZ1Learnt	= (BOOL)(LONG)	(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ1_LEARNT];
			m_stPadLevels[i].m_lZ1BondLevel =				(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ1_LEVEL];
			m_stPadLevels[i].m_lZ1PadBT_X	=				(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ1_BT_X];
			m_stPadLevels[i].m_lZ1PadBT_Y	=				(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ1_BT_Y];
			m_stPadLevels[i].m_bZ2Learnt	= (BOOL)(LONG)	(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ2_LEARNT];
			m_stPadLevels[i].m_lZ2BondLevel =				(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ2_LEVEL];
			m_stPadLevels[i].m_lZ2PadBT_X	=				(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ2_BT_X];
			m_stPadLevels[i].m_lZ2PadBT_Y	=				(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ2_BT_Y];
		}

		if (!bManualLoadPkg || pApp->GetProductLine() != "Rebel")
		{
			m_lPrePickPos_T			= (*psmf)[BP_DATA][BP_HEAD][BP_PREPICKPOS];
			m_lPickPos_T			= (*psmf)[BP_DATA][BP_HEAD][BP_PICKPOS];
			m_lPreBondPos_T			= (*psmf)[BP_DATA][BP_HEAD][BP_PREBONDPOS];
			m_lBondPos_T			= (*psmf)[BP_DATA][BP_HEAD][BP_BONDPOS];
			m_lCleanColletPos_T		= (*psmf)[BP_DATA][BP_HEAD][BP_CCTPOS];
			m_lChangeCarrierPos_T	= (*psmf)[BP_DATA][BP_HEAD][BP_CCRPOS];
		}
		m_lPickLevel_Z			= (*psmf)[BP_DATA][BP_HEAD][BP_PICKLEVEL];
		m_lBondLevel_Z			= (*psmf)[BP_DATA][BP_HEAD][BP_BONDLEVEL];
		m_lReplaceLevel_Z		= (*psmf)[BP_DATA][BP_HEAD][BP_REPLACELEVEL];
		m_lSwingOffset_Z		= (*psmf)[BP_DATA][BP_HEAD][BP_SWINGOFFSET];
		m_lAutoCleanCollet_Z	= (*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_Z];
		m_lAutoCleanCollet_T	= (*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_T];
		m_lAutoCleanAirBlowLimit = (*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_AIRBLOW_LIMIT];		//v4.16T5
		m_lAutoCleanOffsetZ		= (*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_OFFSET_Z];
		m_lAutoCleanAutoLearnZLimit		= (*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_AUTOLEARNZ_LIMIT];	//v4.46T21
		m_lAutoCleanAutoLearnZCounter	= (*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_AUTOLEARNZ_COUNT];	//v4.46T21
		m_lAutoCleanAutoLearnZOffset	= (*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_AUTOLEARNZ_OFFSET];	//v4.46T21
		m_bAutoCleanAutoLearnZLevels	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CLEAN_AUTO_LEARN_Z]);	//v4.49A6
		if (m_lAutoCleanOffsetZ != 0)
		{
			m_lAutoCleanCollet_Z	= m_lBondLevel_Z + m_lAutoCleanOffsetZ;
		}
		m_lBPAccLvlZ = m_lAutoCleanCollet_Z;
		
		//v4.50A6	//AGC
		m_lAGCClampLevelOffsetZ			= (*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_CLAMP_LEVEL_OFFSET_Z];
		m_lAGCUplookPRLevelOffsetZ		= (*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_UPLOOK_PR_LEVEL_OFFSET_Z];
		//v4.59A44	//MS50 AGC module	
		m_lMS50ChgCollet1stPos_T		= (*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_MS50_1ST_CPOS_T];
		m_lMS50NoOfCollets				= (*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_MS50_TOTAL_COLLETS];	
		m_lMS50CurrColletIndex			= (*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_MS50_COLLET_INDEX];		
		m_lAGCHolderBlowDieZ			= (LONG)(*psmf)[BP_DATA][BP_HEAD]["AGCHolderBlowDieZ"]; //Load for PKG only Matthew20190418

		//v4.46T21	//SanAn
		if ((m_lAutoCleanAutoLearnZOffset <= 0) && !m_bAutoCleanAutoLearnZLevels)	//v4.49A10
			m_lAutoCleanAutoLearnZOffset = 500;

		m_lBT2OffsetZ			= (*psmf)[BP_DATA][BP_HEAD][BP_BT2_OFFSET_Z];				//MS100 9Inch dual table config
		m_lBT2OffsetX			= (*psmf)[BP_DATA][BP_HEAD][BP_BT2_OFFSET_X];				//MS109
		m_lBT2OffsetY			= (*psmf)[BP_DATA][BP_HEAD][BP_BT2_OFFSET_Y];				//MS109
		m_lNGPickIndexT			= (*psmf)[BP_DATA][NGPICK][NG_INDEX_T];	

		m_lPickLevel_Z2			= (*psmf)[BP_DATA][BP_HEAD][BP_PICKLEVEL_Z2];
		m_lBondLevel_Z2			= (*psmf)[BP_DATA][BP_HEAD][BP_BONDLEVEL_Z2];
		m_lReplaceLevel_Z2		= (*psmf)[BP_DATA][BP_HEAD][BP_REPLACELEVEL_Z2];
		m_lSwingOffset_Z2		= (*psmf)[BP_DATA][BP_HEAD][BP_SWINGOFFSET_Z2];

		m_lPickDriveIn			= (*psmf)[BP_DATA][BP_HEAD][BP_PICKDRIVEIN];
		m_lBondDriveIn			= (*psmf)[BP_DATA][BP_HEAD][BP_BONDDRIVEIN];

		m_lBondHeadToleranceLevel	= (*psmf)[BP_DATA][BP_HEAD][BP_LOW_BHTOLERANCE_LEVEL];
		m_lZPBLevelOffsetLimit		= (*psmf)[BP_DATA][BP_HEAD][BP_Z_PB_LEVEL_OFFSET_LIMIT];

		m_lPreEjectorUpDelay	= (*psmf)[BP_DATA][BP_DELAY][BP_UBHPREEJUPDELAY];
		m_lUBHOpenDacOffset_Z		= (*psmf)[BP_DATA][BP_HEAD][BP_UBHOPENDACOFFSET];
		m_lUBHStepOffset_Z			= (*psmf)[BP_DATA][BP_HEAD][BP_UBHSTEPMOVESTEP];
		m_lUBHPickLevelDac_Z		= (LONG)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_UBHPICKLEVELDACZ];
		m_lUBHOpenDacStepSize_Z	= (LONG)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_UBHOPENDACSTEPSIZE];
		m_lUBHOpenDacMax_Z			= (LONG)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_UBHOPENDACMAXDAC];

		m_lUBHSearchTriggerPosErr = (LONG)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_UBHSEARCHTRIGGERERR];
		m_lUBHSearchSpeed_Z		= (LONG)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_UBHSEARCHSPEED];
		m_lUBHSearchDriveInSpeed_Z = (LONG)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_UBHDRIVEINSPEED];
		m_lUBHSearchDriveInSample_Z = (LONG)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_UBHDRIVEINSAMPLE];
		m_lUBHMinPosErrTriggerSample_Z = (LONG)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_UBHMINSEARCHRIGGERSAMPLE];
		
		m_lUBHSettlingTime_T = (*psmf)[BP_DATA][BP_HEAD][BP_UBHSETTLINGTIME_T];
		// ejector level
		m_lEjectLevel_Ej		= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJUPLEVEL];
		m_lStandbyLevel_Ej		= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJSBLEVEL];
		m_lContactLevel_Ej		= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJCTLEVEL];		//v3.94T4
		m_lEjectorDriveIn		= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJT_UPDRIVEIN];
		m_lEjectorLimitType		= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJT_UP_LIMIT_TYPE];

		m_lEjCTLevelOffset		= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJCTLEVEL_OFFSET];

		// Ejector Cap Level
//		m_lUpLevel_El			= (*psmf)[BP_DATA][BP_EJECTOR_ELEVATOR][BP_EJECTOR_EL_UPLEVEL];
		m_lStandby_EJ_Cap		= (*psmf)[BP_DATA][BP_EJECTOR_ELEVATOR][BP_EJECTOR_EL_SBLEVEL];
		m_lDnOffset_EJ_Cap		= (*psmf)[BP_DATA][BP_EJECTOR_ELEVATOR][BP_EJECTOR_EL_DNOFFSET];
		m_lUpLevel_EJ_Cap		= (*psmf)[BP_DATA][BP_EJECTOR_ELEVATOR][BP_EJECTOR_EL_UPLEVEL];
		// Ejector Elv Level
		m_lStandby_EJ_Elevator	= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJECTOR_ELV_SBLEVEL];
		m_lUpLevel_EJ_Elevator	= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJECTOR_ELV_UPLEVEL];
		// Ejector Theta Level
		m_lStandby_EJ_Theta		= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJECTOR_THETA_SBLEVEL];
		m_lUpLevel_EJ_Theta		= (*psmf)[BP_DATA][BP_EJECTOR][BP_EJECTOR_THETA_UPLEVEL];

		// Bin Ejector Pin Level
		m_lStandby_Bin_EJ_Pin		= (*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_SBLEVEL];
		m_lUpLevel_Bin_EJ_Pin		= (*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_UPLEVEL];
		// Bin Ejector Cap Level
		m_lStandby_Bin_EJ_Cap		= (*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_CAP_SBLEVEL];
		m_lUpLevel_Bin_EJ_Cap		= (*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_CAP_UPLEVEL];
		// Bin Ejector Elv Level
		m_lStandby_Bin_EJ_Elevator	= (*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_ELV_SBLEVEL];
		m_lUpLevel_Bin_EJ_Elevator	= (*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_ELV_UPLEVEL];
		// Bin Ejector Theta Level
		m_lStandby_Bin_EJ_Theta		= (*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_THETA_SBLEVEL];
		m_lUpLevel_Bin_EJ_Theta		= (*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_THETA_UPLEVEL];
		

		m_bEnableMS100EjtXY		= (BOOL)(LONG)(*psmf)[BP_DATA][BP_EJECTOR_TABLE][EJT_ENABLE];
		m_lCDiePos_EjX			= (*psmf)[BP_DATA][BP_EJECTOR_TABLE][BP_EJT_CDIE_POS_X];
		m_lCDiePos_EjY			= (*psmf)[BP_DATA][BP_EJECTOR_TABLE][BP_EJT_CDIE_POS_Y];

		//Sync Move 
		if (m_lSyncDistance > (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
		{
			m_lSyncDistance = m_lEjectLevel_Ej - m_lStandbyLevel_Ej;
		}
		
		m_lSyncDistance			= (*psmf)[BP_DATA][BP_SYNC_MOVE][BP_SYNC_MOVE_DIST];
		m_dSyncSpeed			= (*psmf)[BP_DATA][BP_SYNC_MOVE][BP_SYNC_MOVE_SPEED];

		m_lSyncTime = 0;
		if (m_dSyncSpeed != 0)
		{
			DOUBLE dDistance;

			dDistance = (DOUBLE)m_lSyncDistance;
			m_lSyncTime	= (LONG)((dDistance / m_dSyncSpeed));
		}

		m_lBHZ1_PickDAC			= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_PICK_DAC];
		m_lBHZ1_BondDAC			= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_BOND_DAC];
		m_lBHZ2_PickDAC			= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_PICK_DAC];
		m_lBHZ2_BondDAC			= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_BOND_DAC];
		m_lBHZ1_PickDACOffset	= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_PICK_DAC_OFFSET];
		m_lBHZ1_BondDACOffset	= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_BOND_DAC_OFFSET];
		m_lBHZ2_PickDACOffset	= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_PICK_DAC_OFFSET];
		m_lBHZ2_BondDACOffset	= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_BOND_DAC_OFFSET];

		m_lSyncTriggerValue			= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SYNC_TRIGGER_VALUE];
		m_lSyncZOffset				= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SYNC_Z_OFFSET];

		if (m_lSyncTriggerValue >= (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
		{
			m_lSyncTriggerValue = m_lEjectLevel_Ej - m_lStandbyLevel_Ej - 1;
		}	

		if (m_lSyncZOffset >= (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
		{
			m_lSyncZOffset = m_lEjectLevel_Ej - m_lStandbyLevel_Ej - 1;
		}

		m_lSoftTouchBondDistance	= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SOFT_TOUCH_BOND_DISTANCE];
		m_lSoftTouchPickDistance	= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SOFT_TOUCH_PICK_DISTANCE];
		m_lSoftTouchVelocity		= (*psmf)[BP_DATA][BP_HEAD][BP_Z_SOFT_TOUCH_VELOCITY];

		m_dBHZ1_OpenDACCalibRatio		= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_OPEN_DAC_CALIB_RATIO];
		m_dBHZ2_OpenDACCalibRatio		= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_OPEN_DAC_CALIB_RATIO];
		m_dBHZ1_DACDistanceCalibRatio	= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAC_DIST_CALIB_RATIO];
		m_dBHZ2_DACDistanceCalibRatio	= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAC_DIST_CALIB_RATIO];

		m_bEnableDAFlowRunTimeCheck		= (BOOL)(LONG)(*psmf)[BP_DATA][BP_HEAD][BP_DAIRFLOW_RUNTIME_CHECK];
		m_ulBHZ1DAirFlowSetZeroOffset	= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_SETZERO_OFFSET];
		m_ulBHZ2DAirFlowSetZeroOffset	= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_SETZERO_OFFSET];
		m_ulBHZ1DAirFlowBlockValue		= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_BLOCK_VALUE];
		m_ulBHZ1DAirFlowUnBlockValue	= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_UNBLOCK_VALUE];
		m_ulBHZ1DAirFlowThreshold		= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_THRESHOLD];
		m_ulBHZ1DAirFlowThresholdCJ		= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_THRESHOLD_CJ];
		m_ulBHZ2DAirFlowBlockValue		= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_BLOCK_VALUE];
		m_ulBHZ2DAirFlowUnBlockValue	= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_UNBLOCK_VALUE];
		m_ulBHZ2DAirFlowThreshold		= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_THRESHOLD];
		m_ulBHZ2DAirFlowThresholdCJ		= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_THRESHOLD_CJ];
		//v4.50A2
		m_dBHZ1ThresholdPercent			= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_THRESHOLD_PCT];
		m_dBHZ2ThresholdPercent			= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_THRESHOLD_PCT];

		m_dBHZ1UplookCalibFactor		= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_UPLOOK_CALIB_FACTOR];
		m_dBHZ2UplookCalibFactor		= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_UPLOOK_CALIB_FACTOR];

		if ( (m_dBHZ1ThresholdPercent < 10) || (m_dBHZ1ThresholdPercent > 90) )
			m_dBHZ1ThresholdPercent = 50;
		if ( (m_dBHZ2ThresholdPercent < 10) || (m_dBHZ2ThresholdPercent > 90) )
			m_dBHZ2ThresholdPercent = 50;

		m_lBHZ1ThresholdLimit			= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_THRESHOLD_LIMIT];
		m_lBHZ2ThresholdLimit			= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_THRESHOLD_LIMIT];
		
		m_lBHZ1ThresholdUpperLimit					= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_THRESHOLD_UPPER_LIMIT];
		m_ulBHZ1DAirFlowCleanColletThreshold		= (*psmf)[BP_DATA][BP_HEAD][BP_Z1_THRESHOLD_CLEAN__COLLET];
		m_ulBHZ2DAirFlowCleanColletThreshold		= (*psmf)[BP_DATA][BP_HEAD][BP_Z2_THRESHOLD_CLEAN__COLLET];


		m_lBHZOffsetbyPBEmptyCheck		= (*psmf)[BP_DATA][BP_HEAD][BP_Z_OFFSET_PB_EMPTYCHECK];		//v4.48A8
		m_lBHZAutoLearnDriveInLimit		= (*psmf)[BP_DATA][BP_HEAD][BP_Z_AUTO_LEARN_DRIVEIN_LIMIT];

		//m_bUseMultiProbeLevel			= (BOOL)(LONG)(*psmf)[PB_PROCESS][PB_USE_MULTI_PROBE_LEVEL];	//Default value to TRUE by Matthew 20190410

		m_lRestartUpdateMinTime			= (*psmf)[BP_DATA][BP_HEAD][BP_T_ILC_RESTART_UPDATE_MIN_TIME];
	}
	DOUBLE	dAvgTime = (*psmf)[BH_MACHINE_STATISITC][BH_AVERAGE_CYCLE_TIME];
	(*m_psmfSRam)[BH_MACHINE_STATISITC][BH_AVERAGE_CYCLE_TIME] = dAvgTime;

	// close config file
	pUtl->CloseBPConfig();

	//v4.00T1
	//NEUTRAL state fcn now moved to Feature instead
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_bNeutralVacState	= pApp->GetFeatureStatus(MS896A_FUNC_BH_VAC_NEUTRALSTATE);
	m_lEjMoveDistance	= m_lEjectLevel_Ej - m_lStandbyLevel_Ej;	//v4.47A4

	//Update HMI variable
	//m_lBPGeneral_0	= 0;
	m_lBPGeneral_1	= m_lPrePickPos_T;
	m_lBPGeneral_2	= m_lPickPos_T;
	m_lBPGeneral_3	= m_lPreBondPos_T;
	m_lBPGeneral_4	= m_lBondPos_T;	
	m_lBPGeneral_5	= m_lCleanColletPos_T;
	m_lBPGeneral_6	= m_lPickLevel_Z;
	m_lBPGeneral_7	= m_lBondLevel_Z;
	m_lBPGeneral_8	= m_lReplaceLevel_Z;
	m_lBPGeneral_9	= m_lEjectLevel_Ej;
	m_lBPGeneral_A	= m_lStandbyLevel_Ej;
	m_lBPGeneral_B	= m_lContactLevel_Ej;		//m_lPickLevel_Z2;		//andrewng //2020-0708
	m_lBPGeneral_C	= m_lBondLevel_Z2;
	m_lBPGeneral_D	= m_lReplaceLevel_Z2;
//	m_lBPGeneral_E	= m_lUpLevel_El;
	m_lBPGeneral_F	= m_lStandby_EJ_Cap;
	m_lBPGeneral_G	= m_lContactLevel_PBTZ;
	m_lBPGeneral_H	= m_lStandByLevel_PBTZ;
	
	//v4.43T6
	m_lBPGeneral_BHZ1	= m_lPickLevel_Z  + 500;
	m_lBPGeneral_BHZ2	= m_lPickLevel_Z2 + 500;
	
	m_lBPAccPosT	= m_lAutoCleanCollet_T;
	m_lBPAccLvlZ	= m_lAutoCleanCollet_Z;

	m_lBPGeneral_TmpA = 0;
	m_lBPGeneral_TmpB = 0;
	m_lBPGeneral_TmpC = 0;

	//Update swing level for SVO motor only
	if (m_fHardware && !m_bDisableBH)
	{
		if (CMS896AStn::MotionIsServo(BH_AXIS_Z, &m_stBHAxis_Z))
		{
			if (m_lSwingOffset_Z < 4000)
			{
				m_lSwingOffset_Z = 4000;
			}
		}
	
		m_lSwingLevel_Z		= max(m_lPickLevel_Z, m_lBondLevel_Z) + m_lSwingOffset_Z;
		m_lSwingLevel_Z2	= max(m_lPickLevel_Z2, m_lBondLevel_Z2) + m_lSwingOffset_Z2;	//v3.65

		//v4.51A17
		//if (m_bMS60EjElevator)		//v4.51A6
		//{
		//	m_lSwingLevel_Z		= 0;	// - m_lSwingOffset_Z;		//v4.51A8
		//	m_lSwingLevel_Z2	= 0;	// - m_lSwingOffset_Z2;		//v4.51A8
		//}
	}

	if (m_bEnableMS100EjtXY && !CMS896AStn::m_bMS100EjtXY)
	{
		BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);
		if (!bColletOffsetWoEjtXY)
		{
			m_bEnableMS100EjtXY = FALSE;
		}
	}

	//Check if operator, force to enable "Missing Die Check" & "Collet Jam Check"
	//CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	//if ( pAppMod->m_bOperator == TRUE )
	//{
	//m_bCheckColletJam		= TRUE;		
	//m_bCheckMissingDie	= TRUE;
	//}
	(*m_psmfSRam)["WaferTable"]["PRDelay"]			= m_lPRDelay + m_lMinPRDelay;
	(*m_psmfSRam)["WaferTable"]["TDelay"]			= m_lWTTDelay;
	(*m_psmfSRam)["WaferTable"]["SettlingDelay"]	= m_lWTSettlingDelay;
	(*m_psmfSRam)["WaferTable"]["VacOffDelay"]		= m_lEjVacOffDelay;			//v3.78
	(*m_psmfSRam)["BinTable"]["BTDelay"]			= m_lBinTableDelay + m_lMinBinTableDelay;
	(*m_psmfSRam)["BinTable"]["BT2OffsetZ"]			= m_lBT2OffsetZ;			//v4.36T1
	(*m_psmfSRam)["BondHead"]["HeadPrePickDelay"]	= m_lHeadPrePickDelay;	//v4.57A13
	(*m_psmfSRam)["BondHead"]["StandByLvl_Ej"]		= m_lStandbyLevel_Ej;		//v4.59A36
	//v4.59A39
	(*m_psmfSRam)["BondHead"]["CDiePos_EjX"]		= m_lCDiePos_EjX;
	(*m_psmfSRam)["BondHead"]["CDiePos_EjY"]		= m_lCDiePos_EjY;

	(*m_psmfSRam)["BondHead"]["FloatBlowDelay"]		= m_lFloatBlowDelay;		//andrewng //2020-0727

	m_bEnableBHMark_HMI = IsEnableBHMark();
	m_bEnableAutoUpdateBHPickPosnMode_HMI = IsEnableBHMark() || IsEnableColletHole();


	//Sync Move Via Ej Cmd Dist	//for MS100
	if (m_bEnableSyncMotionViaEjCmd == TRUE)
	{
		//M94
		//Ej_Profile(NORMAL_PROF);
		EjSoftTouch_CMD();
		Ej_SelectObwProfile(m_lEjectLevel_Ej - m_lStandbyLevel_Ej);
		Ej_SelectControl(m_lEjectLevel_Ej - m_lStandbyLevel_Ej);

		//Ej Travel cannot be zero
		if ((m_lEjectLevel_Ej - m_lStandbyLevel_Ej) <= 0)
		{
			m_bEnableSyncMotionViaEjCmd = FALSE;
		}
		else
		{
			MotionSetupSyncViaEjCmdDistance(BH_AXIS_EJ, &m_stBHAxis_Ej, (m_lEjectLevel_Ej - m_lStandbyLevel_Ej));
			if (m_lEjectorUpDelay < m_lTime_Ej)
			{
				m_lEjectorUpDelay = m_lTime_Ej;
			}
		}

		CheckHeadBondDelay();
	}

	m_lBHZ1HomeOffset = pApp->GetBHZ1HomeOffset();
	m_lBHZ2HomeOffset = pApp->GetBHZ2HomeOffset();

	LoadFileHWD();
	return TRUE;
}


BOOL CBondHead::SaveBhData(VOID)
{
	CString szData;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadBPConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetBPConfigFile();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}
		
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	m_bTCLayerPick = m_bTCLayerPick && m_bTEInUse;

	//v4.43T6	//Reposition from btm of fcn to top of fcn
	//Sync Move Via Ej Cmd Dist	//for MS100
	if (m_bEnableSyncMotionViaEjCmd == TRUE)
	{
		//M94
		//Ej_Profile(NORMAL_PROF);
		EjSoftTouch_CMD();
		Ej_SelectObwProfile(m_lEjectLevel_Ej - m_lStandbyLevel_Ej);
		Ej_SelectControl(m_lEjectLevel_Ej - m_lStandbyLevel_Ej);

		//Ej Travel cannot be zero
		if ((m_lEjectLevel_Ej - m_lStandbyLevel_Ej) <= 0)
		{
			m_bEnableSyncMotionViaEjCmd = FALSE;
		}
		else
		{
			MotionSetupSyncViaEjCmdDistance(BH_AXIS_EJ, &m_stBHAxis_Ej, (m_lEjectLevel_Ej - m_lStandbyLevel_Ej));
			if (m_lEjectorUpDelay < m_lTime_Ej)
			{
				m_lEjectorUpDelay = m_lTime_Ej;
			}
		}
	}

	CheckHeadBondDelay();

	//v4.43T13
	if (m_bEnableMS100EjtXY && !CMS896AStn::m_bMS100EjtXY)
	{
		//v4.52A14
		BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);
		if (!bColletOffsetWoEjtXY)
		{
			m_bEnableMS100EjtXY = FALSE;
		}
	}

	if (!IsEnaAutoChangeCollet() && m_bACCSearchColletHole)		//v4.53A23
	{
		m_bACCSearchColletHole = FALSE;
	}


	// update data
	// bonding delay
	(*psmf)[BP_DATA][BP_HEAD][BP_ONVACUUM_AT_PICK]	= m_bOnVacuumAtPick;
	(*psmf)[BP_DATA][BP_DELAY][BP_PICKDELAY]		= m_lPickDelay;
	(*psmf)[BP_DATA][BP_DELAY]["Neutral Delay"]		= m_lNeutralDelay;
	(*psmf)[BP_DATA][BP_DELAY]["Uplook Delay"]		= m_lUplookDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_APDELAY]			= m_lArmPickDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_HPDELAY]			= m_lHeadPickDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_BONDDELAY]		= m_lBondDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_FLOATBLOWDELAY]	= m_lFloatBlowDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_ABDELAY]			= m_lArmBondDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_HBDELAY]			= m_lHeadBondDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_EJUPDELAY]		= m_lEjectorUpDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_EJDOWNDELAY]		= m_lEjectorDownDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_WBOFFDELAY]		= m_lWeakBlowOffDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_HBTIME]			= m_lHighBlowTime;
	(*psmf)[BP_DATA][BP_DELAY][BP_PRDELAY]			= m_lPRDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_WTTDELAY]			= m_lWTTDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_WTSETTLINGDELAY]	= m_lWTSettlingDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_BTDELAY]			= m_lBinTableDelay;
	(*psmf)[BP_DATA][BP_DELAY][BP_MC_ALARM]			= m_lAutobondTimeInMin;		//v2.83T27
	(*psmf)[BP_DATA][BP_DELAY][BP_DEFAULT]			= m_bUseDefaultDelays;		//v2.97T1
	(*psmf)[BP_DATA][BP_DELAY][BP_EJVACOFFELAY]		= m_lEjVacOffDelay;			//v3.78
	(*psmf)[BP_DATA][BP_DELAY][BP_HPPDELAY]			= m_lHeadPrePickDelay;		//v3.79
	(*psmf)[BP_DATA][BP_DELAY][BP_SYNPICKDELAY]		= m_lSyncPickDelay;			//v3.83
	(*psmf)[BP_DATA][BP_DELAY]["TCEjtUpPrDelay"]	= m_lTCUpPreheatTime;
	(*psmf)[BP_DATA][BP_DELAY]["TCDnOnVacDelay"]	= m_lTCDnOnVacDelay;
	(*psmf)[BP_DATA][BP_DELAY]["TCUpHeatingTime"]	= m_lTCUpHeatingTime;
	(*psmf)[BP_DATA][BP_DELAY]["TCLayerPicking"]	= m_bTCLayerPick;
	for (INT i = 0; i < MAX_WAFFLE_PADS; i++)
	{
		CString szTemp;
		szTemp.Format("Pad %d", i);
		(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ1_LEARNT]	= m_stPadLevels[i].m_bZ1Learnt;
		(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ1_LEVEL]	= m_stPadLevels[i].m_lZ1BondLevel;
		(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ1_BT_X]		= m_stPadLevels[i].m_lZ1PadBT_X;
		(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ1_BT_Y]		= m_stPadLevels[i].m_lZ1PadBT_Y;
		(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ2_LEARNT]	= m_stPadLevels[i].m_bZ2Learnt;
		(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ2_LEVEL]	= m_stPadLevels[i].m_lZ2BondLevel;
		(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ2_BT_X]		= m_stPadLevels[i].m_lZ2PadBT_X;
		(*psmf)[BP_DATA][BP_HEAD][BP_WAFFLE_PAD][szTemp][BHZ2_BT_Y]		= m_stPadLevels[i].m_lZ2PadBT_Y;
	}

	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_PADSLEVEL_OFFSET_Z1]	= m_lWPadsLevelOffsetZ1;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_PADSLEVEL_OFFSET_Z2]	= m_lWPadsLevelOffsetZ2;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_BONDLEVEL_BT_R]		= m_bZ1BondLevelBT_R;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_BONDLEVEL_BT_R]		= m_bZ2BondLevelBT_R;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_BONDLEVEL_BT_X]		= m_lZ1BondLevelBT_X;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_BONDLEVEL_BT_Y]		= m_lZ1BondLevelBT_Y;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_BONDLEVEL_BT_X]		= m_lZ2BondLevelBT_X;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_BONDLEVEL_BT_Y]		= m_lZ2BondLevelBT_Y;
	(*psmf)[BP_DATA][BP_HEAD][BP_PREPICKPOS]		= m_lPrePickPos_T;
	(*psmf)[BP_DATA][BP_HEAD][BP_PICKPOS]			= m_lPickPos_T;
	(*psmf)[BP_DATA][BP_HEAD][BP_PREBONDPOS]		= m_lPreBondPos_T;
	(*psmf)[BP_DATA][BP_HEAD][BP_BONDPOS]			= m_lBondPos_T;  
	(*psmf)[BP_DATA][BP_HEAD][BP_CCTPOS]			= m_lCleanColletPos_T;
	(*psmf)[BP_DATA][BP_HEAD][BP_CCRPOS]			= m_lChangeCarrierPos_T;
	(*psmf)[BP_DATA][BP_HEAD][BP_PICKLEVEL]			= m_lPickLevel_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_BONDLEVEL]			= m_lBondLevel_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_REPLACELEVEL]		= m_lReplaceLevel_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_SWINGOFFSET]		= m_lSwingOffset_Z;		
	(*psmf)[BP_DATA][BP_HEAD][BP_PICKDRIVEIN]		= m_lPickDriveIn;
	(*psmf)[BP_DATA][BP_HEAD][BP_BONDDRIVEIN]		= m_lBondDriveIn;
	(*psmf)[BP_DATA][BP_HEAD][BP_AUTOLEARN]			= m_bUseBHAutoLearn;
	(*psmf)[BP_DATA][BP_HEAD][BP_CTSENSOR_LEARN]	= m_bUseCTSensorLearn;		//v2.96T4
	(*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_Z]	= m_lAutoCleanCollet_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_T]	= m_lAutoCleanCollet_T;
	(*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_AIRBLOW_LIMIT]	= m_lAutoCleanAirBlowLimit;			//v4.16T5
	(*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_OFFSET_Z]	= m_lAutoCleanOffsetZ;
	(*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_AUTOLEARNZ_LIMIT] = m_lAutoCleanAutoLearnZLimit;		//v4.46T21
	(*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_AUTOLEARNZ_COUNT]	= m_lAutoCleanAutoLearnZCounter;	//v4.46T21
	(*psmf)[BP_DATA][BP_HEAD][BP_CLEAN_COLLET_AUTOLEARNZ_OFFSET]= m_lAutoCleanAutoLearnZOffset;		//v4.46T21
	(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CLEAN_AUTO_LEARN_Z]		= m_bAutoCleanAutoLearnZLevels;		//v4.49A6

	// the bondhead tolerance level for sanan
	(*psmf)[BP_DATA][BP_HEAD][BP_LOW_BHTOLERANCE_LEVEL]		= m_lBondHeadToleranceLevel; //v4.44T6
	(*psmf)[BP_DATA][BP_HEAD][BP_Z_PB_LEVEL_OFFSET_LIMIT]	= m_lZPBLevelOffsetLimit;

	if (m_lAutoCleanOffsetZ != 0)
	{
		m_lAutoCleanCollet_Z	= m_lBondLevel_Z + m_lAutoCleanOffsetZ;
	}
	m_lBPAccLvlZ = m_lAutoCleanCollet_Z;

	//v4.50A6	//Auto-Change-Collet parameters
	(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_CLAMP_LEVEL_OFFSET_Z]	= m_lAGCClampLevelOffsetZ;
	(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_UPLOOK_PR_LEVEL_OFFSET_Z] = m_lAGCUplookPRLevelOffsetZ;
	//v4.59A44	//MS50 AGC module	
	(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_MS50_1ST_CPOS_T]		= m_lMS50ChgCollet1stPos_T;
	(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_MS50_TOTAL_COLLETS]	= m_lMS50NoOfCollets;	
	(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_MS50_COLLET_INDEX]		= m_lMS50CurrColletIndex;		
	//(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_C1_OFFSET_X_UM]		= m_lAGCCollet1OffsetX;
	//(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_C1_OFFSET_Y_UM]		= m_lAGCCollet1OffsetY;
	//(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_C2_OFFSET_X_UM]		= m_lAGCCollet2OffsetX;
	//(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CHG_COLLET][BP_C2_OFFSET_Y_UM]		= m_lAGCCollet2OffsetY;


	(*psmf)[BP_DATA][BP_HEAD][BP_BT2_OFFSET_Z]		= m_lBT2OffsetZ;			//MS100 9Inch dual table config
	(*psmf)[BP_DATA][BP_HEAD][BP_BT2_OFFSET_X]		= m_lBT2OffsetX;			//v4.37		//MS109
	(*psmf)[BP_DATA][BP_HEAD][BP_BT2_OFFSET_Y]		= m_lBT2OffsetY;			//v4.37		//MS109
	(*psmf)[BP_DATA][NGPICK][NG_INDEX_T]			= m_lNGPickIndexT;	

	(*psmf)[BP_DATA][BP_HEAD][BP_PICKLEVEL_Z2]		= m_lPickLevel_Z2;
	(*psmf)[BP_DATA][BP_HEAD][BP_BONDLEVEL_Z2]		= m_lBondLevel_Z2;
	(*psmf)[BP_DATA][BP_HEAD][BP_REPLACELEVEL_Z2]	= m_lReplaceLevel_Z2;
	(*psmf)[BP_DATA][BP_HEAD][BP_SWINGOFFSET_Z2]	= m_lSwingOffset_Z2;

	(*psmf)[BP_DATA][BP_DELAY][BP_UBHPREEJUPDELAY]		= m_lPreEjectorUpDelay;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHSTEPMOVESTEP]		= m_lUBHStepOffset_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHOPENDACOFFSET]		= m_lUBHOpenDacOffset_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHPICKLEVELDACZ]		= m_lUBHPickLevelDac_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHOPENDACSTEPSIZE]	= m_lUBHOpenDacStepSize_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHOPENDACMAXDAC]		= m_lUBHOpenDacMax_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHSEARCHTRIGGERERR]	= m_lUBHSearchTriggerPosErr;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHSEARCHSPEED]		= m_lUBHSearchSpeed_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHDRIVEINSPEED]		= m_lUBHSearchDriveInSpeed_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHDRIVEINSAMPLE]		= m_lUBHSearchDriveInSample_Z;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHMINSEARCHRIGGERSAMPLE] = m_lUBHMinPosErrTriggerSample_Z ;

	(*psmf)[BP_DATA][BP_HEAD][BP_UBHSETTLINGTIME_T]	= m_lUBHSettlingTime_T;
	(*psmf)[BP_DATA][BP_HEAD][BP_UBHSEPARATESETTLING_T] = m_bEnableSeparteSettlingBlk_T;


	// ejector level
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJUPLEVEL]		= m_lEjectLevel_Ej;
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJSBLEVEL]		= m_lStandbyLevel_Ej;
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJCTLEVEL]		= m_lContactLevel_Ej;		//v3.94T4
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJT_UPDRIVEIN]	= m_lEjectorDriveIn;
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJT_UP_LIMIT_TYPE]	= m_lEjectorLimitType;
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJT_AUTOLEARN]	= m_bUseEjAutoLearn;
	(*psmf)[BP_DATA][BP_EJECTOR][BP_DIEHEIGHT]		= m_lWafDieHeight;			//v3.94T4
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJCTLEVEL_OFFSET]	= m_lEjCTLevelOffset;
	(*psmf)[BH_MACHINE_STATISITC][BH_AVERAGE_CYCLE_TIME] = m_dAvgCycleTime;
	(*m_psmfSRam)[BH_MACHINE_STATISITC][BH_AVERAGE_CYCLE_TIME] = m_dAvgCycleTime;

	// Ejector Pin Level
//	(*psmf)[BP_DATA][BP_EJECTOR_ELEVATOR][BP_EJECTOR_EL_UPLEVEL]	= m_lUpLevel_El;
	(*psmf)[BP_DATA][BP_EJECTOR_ELEVATOR][BP_EJECTOR_EL_SBLEVEL]	= m_lStandby_EJ_Cap;
	(*psmf)[BP_DATA][BP_EJECTOR_ELEVATOR][BP_EJECTOR_EL_DNOFFSET]	= m_lDnOffset_EJ_Cap;
	(*psmf)[BP_DATA][BP_EJECTOR_ELEVATOR][BP_EJECTOR_EL_UPLEVEL]	= m_lUpLevel_EJ_Cap;
	// Ejector Elv Level
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJECTOR_ELV_SBLEVEL]			= m_lStandby_EJ_Elevator;
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJECTOR_ELV_UPLEVEL]			= m_lUpLevel_EJ_Elevator;
	// Ejector Theta Level
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJECTOR_THETA_SBLEVEL]			= m_lStandby_EJ_Theta;
	(*psmf)[BP_DATA][BP_EJECTOR][BP_EJECTOR_THETA_UPLEVEL]			= m_lUpLevel_EJ_Theta;

	// Bin Ejector Pin Level
	(*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_SBLEVEL]		= m_lStandby_Bin_EJ_Pin;
	(*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_UPLEVEL]		= m_lUpLevel_Bin_EJ_Pin;
	// Bin Ejector Cap Level
	(*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_CAP_SBLEVEL]	= m_lStandby_Bin_EJ_Cap;
	(*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_CAP_UPLEVEL]	= m_lUpLevel_Bin_EJ_Cap;
	// Bin Ejector Elv Level
	(*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_ELV_SBLEVEL]	= m_lStandby_Bin_EJ_Elevator;
	(*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_ELV_UPLEVEL]	= m_lUpLevel_Bin_EJ_Elevator;
	// Bin Ejector Theta Level
	(*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_THETA_SBLEVEL]	= m_lStandby_Bin_EJ_Theta;
	(*psmf)[BP_DATA][BP_BIN_EJECTOR][BP_BIN_EJECTOR_THETA_UPLEVEL]	= m_lUpLevel_Bin_EJ_Theta;
	

	(*psmf)[BP_DATA][BP_EJECTOR_TABLE][EJT_ENABLE]					= m_bEnableMS100EjtXY;
	(*psmf)[BP_DATA][BP_EJECTOR_TABLE][BP_EJT_CDIE_POS_X]			= m_lCDiePos_EjX;
	(*psmf)[BP_DATA][BP_EJECTOR_TABLE][BP_EJT_CDIE_POS_Y]			= m_lCDiePos_EjY;

	//v4.42T8
#ifdef NU_MOTION
	if ( (m_ulMaxEjectorCount > 0) && (m_ulMaxEjectorCount < 1000) )
	{
		m_ulMaxEjectorCount = 1000;
	}
	//2018.4.4 change to 20 from 200
	if ( (m_ulMaxColletCount > 0) && (m_ulMaxColletCount < 20) )
	{
		m_ulMaxColletCount = 20;
	}
	if ( (m_ulMaxCollet2Count > 0) && (m_ulMaxCollet2Count < 20) )
	{
		m_ulMaxCollet2Count = 20;
	}
	if ( (m_ulMaxCleanCount > 0) && (m_ulMaxCleanCount < 10) )
	{
		m_ulMaxCleanCount = 10;
	}
#endif

	// Maximum Count
	(*psmf)[BP_DATA][BP_COUNT][BP_CLEAN_COLLET_TIME_OUT]= m_ulCleanColletTimeOut;
	(*psmf)[BP_DATA][BP_COUNT][BP_MAXEJECTOR]			= m_ulMaxEjectorCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET]			= m_ulMaxColletCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET2]			= m_ulMaxCollet2Count;				//v3.92
	(*psmf)[BP_DATA][BP_COUNT][BP_MAXCLEAN]				= m_ulMaxCleanCount;

	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET]					= m_lEjectorKOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KCOUNT]					= m_lEjectorKCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_LIMIT]				= m_lEjectorKOffsetLimit;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_BHZ1_MD]			= m_lEjectorKOffsetForBHZ1MD;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_BHZ2_MD]			= m_lEjectorKOffsetForBHZ2MD;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_KOFFSET]			= m_lEjSubRegionKOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_KCOUNT]			= m_lEjSubRegionKCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_SKOFFSET]		= m_lEjSubRegionSKOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_SKCOUNT]			= m_lEjSubRegionSKCount;

	//v4.53A25
	(*psmf)[BP_DATA][BP_COUNT][BP_Z1_SKOFFSET]		= m_lZ1SmartKOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_Z1_SKCOUNT]		= m_lZ1SmartKCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_Z2_SKOFFSET]		= m_lZ2SmartKOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_Z2_SKCOUNT]		= m_lZ2SmartKCount;

	(*psmf)[BP_DATA][BP_COUNT]["Ejector K Offset Reset Count"]	= m_lEjectorKOffsetResetCount;// M69
	(*psmf)[BP_DATA][BP_COUNT]["Reset K1K2"]					= m_bCheckK1K2;//M70
	(*psmf)[BP_DATA][BP_COUNT]["Auto Change Collet On Off HMI"]	= m_bAutoChangeColletOnOff;

	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_RESET_WAFEREND] = m_bResetEjKOffsetAtWaferEnd;	//v4.43T8
	(*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH1_KOFFSET]		= m_lDAFlowBH1KOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH1_KCOUNT]		= m_lDAFlowBH1KCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH1_KOFFSET_LIMIT]= m_lDAFlowBH1KOffsetLimit;
	(*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH2_KOFFSET]		= m_lDAFlowBH2KOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH2_KCOUNT]		= m_lDAFlowBH2KCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_DAF_BH2_KOFFSET_LIMIT]= m_lDAFlowBH2KOffsetLimit;
	(*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_1]		= m_bEnableCustomCount1;
	(*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_2]		= m_bEnableCustomCount2;
	(*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_3]		= m_bEnableCustomCount3;
	(*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_1_LIMIT]	= m_ulCustomCounter1Limit;
	(*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_2_LIMIT]	= m_ulCustomCounter2Limit;
	(*psmf)[BP_DATA][BP_COUNT][BP_CUSTOM_COUNT_3_LIMIT]	= m_ulCustomCounter3Limit;
	(*psmf)[BP_DATA][BP_DELAY][BP_BHZ_AUTO_CLEANCOLLET_SWING_TIME]		= m_lAutoCleanColletSwingTime;		//v3.77
	(*psmf)[BP_DATA][BP_DELAY][BP_BHZ_ACC_LAST_DROP_DOWN_TIME]			= m_lAccLastDropDownTime;			//v3.77
	(*psmf)[BP_DATA][BP_DELAY][BP_BHZ_ACC_LIQUID_EXPIRE_TIME]	= m_lLiquidExpireTime;
	(*psmf)[BP_DATA][BP_DELAY]["Start Lot Check Count"]	= m_ulStartLotCheckCount;
	// Missing die & collet jam count
	(*psmf)[BP_DATA][BP_HEAD][BP_CHECK_COLLETJAM]		= m_bCheckColletJam;
	(*psmf)[BP_DATA][BP_HEAD][BP_CHECK_MISSDIE]			= m_bCheckMissingDie;
	//(*psmf)[BP_DATA][BP_COUNT][BP_MAX_CYCLE_MD_LIMIT]	= m_lMDCycleLimit;	//Default value by Matthew 20190410
	(*psmf)[BP_DATA][BP_COUNT]["NG No Die Limit"]		= m_lNGNoDieLimit;
	(*psmf)[BP_DATA][BP_COUNT][BP_MAX_MISSDIE_RETRY]	= m_lMissingDie_Retry; 
	(*psmf)[BP_DATA][BP_COUNT][BP_MAX_COLLETJAM_RETRY]	= m_ulColletJam_Retry;
	(*psmf)[BP_DATA][BP_HEAD][BP_AUTO_CLEAN_COLLET]		= m_bAutoCleanCollet;	
	(*psmf)[BP_DATA][BP_HEAD][BP_ACC_SEARCH_COLLET_HOLE]= m_bACCSearchColletHole;
	(*psmf)[BP_DATA][BP_COUNT][BP_MAX_UPLOOK_PR_FAIL_LIMIT]	= m_lBHUplookPrFailLimit;	//v4.59A5

	(*psmf)[BP_DATA][BP_HEAD][BP_VAC_NEUTRAL_STATE]		= m_bNeutralVacState;
	(*psmf)[BP_DATA][BP_HEAD][BP_MD_RETRY_AUTOCLEAN]	= m_bMDRetryUseAutoCleanCollet;	//v4.49A3
	//v4.59A26
	(*psmf)[BP_DATA][BP_HEAD][BP_CHECK_EJ_PRESSURE]		= m_bCheckEjPressureSnr;		
	(*psmf)[BP_DATA][BP_HEAD][BP_CHECK_BH1_PRESSURE]	= m_bCheckBH1PressureSnr;
	(*psmf)[BP_DATA][BP_HEAD][BP_CHECK_BH2_PRESSURE]	= m_bCheckBH2PressureSnr;
	// Check Cover Sensor
	(*psmf)[BP_DATA][BP_HEAD][BP_SCAN_CHECK_COVERSNR]	= m_bScanCheckCoverSensor;
	(*psmf)[BP_DATA][BP_HEAD][BP_COVER_SENSOR_ON]		= m_bCoverSensorAlwaysOn;		//v4.28T6
	if (!pApp->m_bSuperUser)			//v4.28T6
	{
		m_bCheckCoverSensor = TRUE;		//v4.28T6
	}
	(*psmf)[BP_DATA][BP_HEAD][BP_CHECK_COVERSNR]		= m_bCheckCoverSensor;		
	(*psmf)[BP_DATA][BP_HEAD][BP_FRONT_COVER_LOCK]		= m_bEnableFrontCoverLock;
	(*psmf)[BP_DATA][BP_HEAD][BP_SIDE_COVER_LOCK]		= m_bEnableSideCoverLock;
	(*psmf)[BP_DATA][BP_HEAD][BP_BIN_EL_COVER_LOCK]		= m_bEnableBinElevatorCoverLock;
	
	//v4.40T1	//PLLM REBEL
	BOOL bEnableBHPostSeal	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS);
	if (!bEnableBHPostSeal)
	{
		m_bCheckPr2DCode		= FALSE;
		m_lPr2DCodeCheckLimit	= 0;
	}	
	(*psmf)[BP_DATA][BP_HEAD][BP_PR_2DCODE]				= m_bCheckPr2DCode;
	(*psmf)[BP_DATA][BP_HEAD][BP_PR_2DCODE_LIMIT]		= m_lPr2DCodeCheckLimit;

	//Sync Move 
	(*psmf)[BP_DATA][BP_SYNC_MOVE][BP_SYNC_MOVE_ENABLE]	= m_bEnableSyncMove;
	(*psmf)[BP_DATA][BP_SYNC_MOVE][BP_SYNC_MOVE_DIST]	= m_lSyncDistance;
	(*psmf)[BP_DATA][BP_SYNC_MOVE][BP_SYNC_MOVE_SPEED]	= m_dSyncSpeed;
	if (m_lSyncDistance > (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
	{
		m_lSyncDistance = m_lEjectLevel_Ej - m_lStandbyLevel_Ej;
	}


	m_lSyncTime = 0;
	if (m_dSyncSpeed != 0)
	{
		DOUBLE dDistance;

		dDistance = (DOUBLE)m_lSyncDistance;
		m_lSyncTime	= (LONG)((dDistance / m_dSyncSpeed));
	}


	(*psmf)[BP_DATA][BP_HEAD][BP_BHZ_OPEN_DAC_ENABLE]	= m_bEnableBHZOpenDac;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_PICK_DAC]			= m_lBHZ1_PickDAC;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_BOND_DAC]			= m_lBHZ1_BondDAC;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_PICK_DAC]			= m_lBHZ2_PickDAC;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_BOND_DAC]			= m_lBHZ2_BondDAC;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_PICK_DAC_OFFSET]	= m_lBHZ1_PickDACOffset;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_BOND_DAC_OFFSET]	= m_lBHZ1_BondDACOffset;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_PICK_DAC_OFFSET]	= m_lBHZ2_PickDACOffset;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_BOND_DAC_OFFSET]	= m_lBHZ2_BondDACOffset;

	if (m_lSyncTriggerValue >= (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
	{
		m_lSyncTriggerValue = m_lEjectLevel_Ej - m_lStandbyLevel_Ej - 1;
	}	

	if (m_lSyncZOffset >= (m_lEjectLevel_Ej - m_lStandbyLevel_Ej))
	{
		m_lSyncZOffset = m_lEjectLevel_Ej - m_lStandbyLevel_Ej - 1;
	}

	(*psmf)[BP_DATA][BP_HEAD][BP_Z_SOFT_TOUCH_PICK_DISTANCE]		= m_lSoftTouchPickDistance;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z_SOFT_TOUCH_BOND_DISTANCE]		= m_lSoftTouchBondDistance;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z_SOFT_TOUCH_VELOCITY]				= m_lSoftTouchVelocity;

	(*psmf)[BP_DATA][BP_HEAD][BP_Z_ENABLE_SYNC_MOTION_VIA_EJ_CMD]	= m_bEnableSyncMotionViaEjCmd;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z_SYNC_TRIGGER_VALUE]				= m_lSyncTriggerValue;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z_SYNC_Z_OFFSET]					= m_lSyncZOffset;

	(*psmf)[BP_DATA][BP_HEAD][BP_EJ_SOFT_TOUCH]						= m_szEjSoftTouch;
	(*psmf)[BP_DATA][BP_HEAD][BP_EJCAP_SOFT_TOUCH]					= m_szEjCapSoftTouch;	//v4.54A6

	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_OPEN_DAC_CALIB_RATIO]	= m_dBHZ1_OpenDACCalibRatio;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_OPEN_DAC_CALIB_RATIO]	= m_dBHZ2_OpenDACCalibRatio;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAC_DIST_CALIB_RATIO]	= m_dBHZ1_DACDistanceCalibRatio;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAC_DIST_CALIB_RATIO]	= m_dBHZ2_DACDistanceCalibRatio;

	(*psmf)[BP_DATA][BP_HEAD][BP_T_ILC_RESTART_UPDATE_MIN_TIME]	= m_lRestartUpdateMinTime;
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("ILC Update Expire Time"), m_lRestartUpdateMinTime);

	//(*psmf)[PB_PROCESS][PB_USE_MULTI_PROBE_LEVEL]			= m_bUseMultiProbeLevel;	//Default value to TRUE by Matthew 20190410

	//v4.40T9	//Nichia
	(*psmf)[BP_DATA][BP_COMM_RETRY]		= CMS896AStn::m_ucCommRetry;
	(*psmf)[BP_DATA][BP_COMM_INTERVAL]	= CMS896AStn::m_ulCommInterval;

	(*psmf)[BP_DATA][BP_HEAD][BP_DAIRFLOW_RUNTIME_CHECK]	= m_bEnableDAFlowRunTimeCheck;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_SETZERO_OFFSET]= m_ulBHZ1DAirFlowSetZeroOffset;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_SETZERO_OFFSET]= m_ulBHZ2DAirFlowSetZeroOffset;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_BLOCK_VALUE]	= m_ulBHZ1DAirFlowBlockValue;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_UNBLOCK_VALUE]	= m_ulBHZ1DAirFlowUnBlockValue;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_THRESHOLD]		= m_ulBHZ1DAirFlowThreshold;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_DAIRFLOW_THRESHOLD_CJ]	= m_ulBHZ1DAirFlowThresholdCJ;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_BLOCK_VALUE]	= m_ulBHZ2DAirFlowBlockValue;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_UNBLOCK_VALUE]	= m_ulBHZ2DAirFlowUnBlockValue;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_THRESHOLD]		= m_ulBHZ2DAirFlowThreshold;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_DAIRFLOW_THRESHOLD_CJ]	= m_ulBHZ2DAirFlowThresholdCJ;
	//v4.50A2
	if ( (m_dBHZ1ThresholdPercent < 10) || (m_dBHZ1ThresholdPercent > 90) )
		m_dBHZ1ThresholdPercent = 50;
	if ( (m_dBHZ2ThresholdPercent < 10) || (m_dBHZ2ThresholdPercent > 90) )
		m_dBHZ2ThresholdPercent = 50;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_THRESHOLD_PCT]			= m_dBHZ1ThresholdPercent;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_THRESHOLD_PCT]			= m_dBHZ2ThresholdPercent;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_THRESHOLD_LIMIT]		= m_lBHZ1ThresholdLimit;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_THRESHOLD_LIMIT]		= m_lBHZ2ThresholdLimit;

	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_THRESHOLD_UPPER_LIMIT]	 = m_lBHZ1ThresholdUpperLimit;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_THRESHOLD_CLEAN__COLLET] = m_ulBHZ1DAirFlowCleanColletThreshold;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_THRESHOLD_CLEAN__COLLET] = m_ulBHZ2DAirFlowCleanColletThreshold;

	(*psmf)[BP_DATA][BP_HEAD][BP_Z1_UPLOOK_CALIB_FACTOR]	= m_dBHZ1UplookCalibFactor;
	(*psmf)[BP_DATA][BP_HEAD][BP_Z2_UPLOOK_CALIB_FACTOR]	= m_dBHZ2UplookCalibFactor;

	(*psmf)[BP_DATA][BP_HEAD][BP_Z_OFFSET_PB_EMPTYCHECK]	= m_lBHZOffsetbyPBEmptyCheck;		//v4.48A8
	(*psmf)[BP_DATA][BP_HEAD][BP_Z_AUTO_LEARN_DRIVEIN_LIMIT]	= m_lBHZAutoLearnDriveInLimit;

	(*psmf)[BP_DATA][BP_HEAD]["NGPICK"]	= CMS896AStn::m_bNGPick;		//v4.51A19
	(*psmf)[BP_DATA][BP_HEAD]["AGCHolderUploadZ"]	= m_lAGCHolderUploadZ;
	(*psmf)[BP_DATA][BP_HEAD]["AGCHolderInstallZ"]	= m_lAGCHolderInstallZ;
	(*psmf)[BP_DATA][BP_HEAD]["AGCHolderBlowDieZ"]	= m_lAGCHolderBlowDieZ;
	(*psmf)[BP_DATA][BP_HEAD]["AGCColletStartAng"]	= m_lAGCColletStartAng;

	(*psmf)[BP_DATA][BP_HEAD][AGC_COLLET_INDEX]		= m_lAGCColletIndex;

	//BH Mark
	(*psmf)[BP_DATA][BP_HEAD]["Auto Update BH Pick Posn"] = m_lAutoUpdateBHPickPosnMode;	
	(*psmf)[BP_DATA][BP_HEAD]["Check BH Mark Cycle"] = m_lCheckBHMarkCycle;
	(*psmf)[BP_DATA][BP_HEAD]["Check BH Mark Delay"] = m_lCheckBHMarkDelay;

	(*psmf)[BP_DATA][BP_HEAD]["Is MHC Trained"]	= m_bIsMHCTrained;

	// close config file
	//Check Load/Save Data
	pUtl->UpdateBPConfig();
	pUtl->CloseBPConfig();

	//v4.47T12
	m_lSwingLevel_Z		= max(m_lPickLevel_Z, m_lBondLevel_Z) + m_lSwingOffset_Z;
	m_lSwingLevel_Z2	= max(m_lPickLevel_Z2, m_lBondLevel_Z2) + m_lSwingOffset_Z2;
	m_lEjMoveDistance	= m_lEjectLevel_Ej - m_lStandbyLevel_Ej;	//v4.47A4

	(*m_psmfSRam)["BondHead"]["EnableCoverSensor"] = m_bCheckCoverSensor;
	// Save Wafer Table to common area
	(*m_psmfSRam)["WaferTable"]["PRDelay"]			= m_lPRDelay + m_lMinPRDelay;
	(*m_psmfSRam)["WaferTable"]["TDelay"]			= m_lWTTDelay;
	(*m_psmfSRam)["WaferTable"]["SettlingDelay"]	= m_lWTSettlingDelay;
	(*m_psmfSRam)["WaferTable"]["VacOffDelay"]		= m_lEjVacOffDelay;			//v3.78
	(*m_psmfSRam)["BinTable"]["BTDelay"]			= m_lBinTableDelay + m_lMinBinTableDelay;
	(*m_psmfSRam)["BinTable"]["BT2OffsetZ"]			= m_lBT2OffsetZ;			//v4.36T1
	(*m_psmfSRam)["BondHead"]["HeadPrePickDelay"]	= m_lHeadPrePickDelay;	//v4.57A13
	(*m_psmfSRam)["BondHead"]["StandByLvl_Ej"]		= m_lStandbyLevel_Ej;		//v4.59A36
	//v4.59A39
	(*m_psmfSRam)["BondHead"]["CDiePos_EjX"]		= m_lCDiePos_EjX;
	(*m_psmfSRam)["BondHead"]["CDiePos_EjY"]		= m_lCDiePos_EjY;

	(*m_psmfSRam)["BondHead"]["BHPickPosnMode"] = m_lAutoUpdateBHPickPosnMode;
	(*m_psmfSRam)["BondHead"]["FloatBlowDelay"]		= m_lFloatBlowDelay;		//andrewng //2020-0727


	//v4.43T6
	m_lBPGeneral_BHZ1	= m_lPickLevel_Z  + 500;
	m_lBPGeneral_BHZ2	= m_lPickLevel_Z2 + 500;

	//v4.59A15	//Renesas MS90
	m_szPickDriveInDisplay.Format("= %.4f um", m_lPickDriveIn * 0.5);
	m_szBondDriveInDisplay.Format("= %.4f um", m_lBondDriveIn * 0.5);

	if (IsEnableILC() == TRUE)	//v4.47T3
	{
		ILC_CalculateApplyLength(m_stBAILCPTB, BH_T_PICK_TO_BOND);
		ILC_CalculateApplyLength(m_stBAILCBTP, BH_T_BOND_TO_PICK);
	}

	//v4.51A17
	//if (m_bMS60EjElevator)		//v4.51A6
	//{
	//	m_lSwingLevel_Z		= 0;	// - m_lSwingOffset_Z;		//v4.51A8
	//	m_lSwingLevel_Z2	= 0;	// - m_lSwingOffset_Z2;		//v4.51A8
	//}

	if (m_bMS60EjElevator)	//v4.53A30
	{
		pApp->WriteProfileInt(gszPROFILE_SETTING, "EjCap Standby Z", (int) m_lStandby_EJ_Cap);
	}

	//v4.46T27
	pApp->SetBHZ1HomeOffset(m_lBHZ1HomeOffset);
	pApp->SetBHZ2HomeOffset(m_lBHZ2HomeOffset);

	return TRUE;
}

BOOL CBondHead::GenerateConfigData()
{
	CString szConfigFile = (*m_psmfSRam)["MS896A"]["ParameterListPath"];
		_T("c:\\MapSorter\\UserData\\Parameters.csv");
	CStdioFile oFile;
	CString szLine;
	CString szMsg;


	if (oFile.Open(szConfigFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText) == FALSE)
	{
		szMsg.Format("BH: Generate Configuration Data open fail");
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		return FALSE;
	}

	szMsg = _T("BH: Generate Configuration Data");
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);		//v4.42T17
	oFile.SeekToEnd();


	//Bond Arm
	oFile.WriteString("\n[BONDARM]\n");
	szLine.Format("PrePick,%d,(PICK>PrePICK>BOND)\n",	m_lPrePickPos_T);
	oFile.WriteString(szLine);
	szLine.Format("Pick,%d,(30000 - 100000)\n",			m_lPickPos_T);
	oFile.WriteString(szLine);
	szLine.Format("Bond,%d,(PICK - 125000)\n",			m_lBondPos_T);
	oFile.WriteString(szLine);
	szLine.Format("Clean Collet,%d,(PICK>CC>BOND)\n",	m_lCleanColletPos_T);
	oFile.WriteString(szLine);


	//BondHead
	oFile.WriteString("\n[BONDHEAD],[HEAD 1]\n");
	szLine.Format("Pick Level,%d,(-4000 - -8000)\n",	m_lPickLevel_Z);
	oFile.WriteString(szLine);
	szLine.Format("Bond Level,%d,(-4000 - -8000)\n",	m_lBondLevel_Z);
	oFile.WriteString(szLine);
	szLine.Format("Relpace Die Level,%d\n",				m_lReplaceLevel_Z);
	oFile.WriteString(szLine);
	szLine.Format("Swing Offset,%d\n",					m_lSwingOffset_Z);
	oFile.WriteString(szLine);
	
	
	//Bonding levels Z2
	if (m_bIsArm2Exist)
	{
		oFile.WriteString("\n[BONDHEAD],[HEAD 2]\n");
		szLine.Format("Pick Level 2,%d,(-4000 - -8000)\n",	m_lPickLevel_Z2);
		oFile.WriteString(szLine);
		szLine.Format("Bond Level 2,%d,(-4000 - -8000)\n",	m_lBondLevel_Z2);
		oFile.WriteString(szLine);
		szLine.Format("Replace Die Level 2,%d\n",			m_lReplaceLevel_Z2);
		oFile.WriteString(szLine);
		szLine.Format("Swing Offset 2,%d\n",				m_lSwingOffset_Z2);
		oFile.WriteString(szLine);

		//v4.31T9
		LONG lBHZ2BondPosOffsetX = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"];
		LONG lBHZ2BondPosOffsetY = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"];
		szLine.Format("BH2 X-offset,%d\n",					lBHZ2BondPosOffsetX);
		oFile.WriteString(szLine);
		szLine.Format("BH2 Y-offset,%d\n",					lBHZ2BondPosOffsetY);
		oFile.WriteString(szLine);
	}


	//Rigid Arm
	oFile.WriteString("\n[BONDHEAD],[RIGID ARM]\n");
	szLine.Format("BH SyncMotion,%d\n",					m_bEnableSyncMotionViaEjCmd);
	oFile.WriteString(szLine);
	szLine.Format("Trigger Level,%d\n",					m_lSyncTriggerValue);
	oFile.WriteString(szLine);
	szLine.Format("Pre-load,%d\n",						m_lSyncZOffset);
	oFile.WriteString(szLine);


	//Ejector
	oFile.WriteString("\n[EJECTOR]\n");
	szLine.Format("Up Level,%d,(1000 - 1800)\n",				m_lEjectLevel_Ej);
	oFile.WriteString(szLine);
	szLine.Format("Up with BH Down Level,%d,(1000 - 1800)\n",	m_lEjectLevel_Ej);
	oFile.WriteString(szLine);
	szLine.Format("Stand By Level,%d\n",						m_lStandbyLevel_Ej);
	oFile.WriteString(szLine);
	szLine.Format("Pre-Eject Level,%d\n",						m_lPreEjectLevel_Ej);
	oFile.WriteString(szLine);
	//v4.49A7	//QC ParaList
	BOOL bNewEjSeq = (BOOL)(LONG)(*m_psmfSRam)["Ejector"]["NewEjSequence"];
	szLine.Format("New Ej Sequence,%d\n",						bNewEjSeq);
	oFile.WriteString(szLine);

	
	if (m_bMS100EjtXY)		//v4.51A14
	{
		//Ejector Table XY
		oFile.WriteString("\n[EJECTOR TABLE]\n");
		szLine.Format("CENTER Position X,%ld\n",				m_lCDiePos_EjX);
		oFile.WriteString(szLine);
		szLine.Format("CENTER Position Y,%ld\n",				m_lCDiePos_EjY);
		oFile.WriteString(szLine);

		LONG lEjtX = m_lCDiePos_EjX;
		LONG lEjtY = m_lCDiePos_EjY;
	}


	//BOnding Delays
	oFile.WriteString("\n[DELAY]\n");
	szLine.Format("Pick Delay,%d,(0 - 10)\n",			m_lPickDelay);
	oFile.WriteString(szLine);
	szLine.Format("Bond Delay,%d,(0 - 10)\n",			m_lBondDelay);
	oFile.WriteString(szLine);
	szLine.Format("Arm Pick Delay,%d,(3 - 15)\n",		m_lArmPickDelay);
	oFile.WriteString(szLine);
	szLine.Format("Arm Bond Delay,%d,(3 - 15)\n",		m_lArmBondDelay);
	oFile.WriteString(szLine);
	szLine.Format("Head Pick Delay,%d,(10 - 30)\n",		m_lHeadPickDelay);
	oFile.WriteString(szLine);
	szLine.Format("Head Bond Delay,%d,(20 - 30)\n",		m_lHeadBondDelay);
	oFile.WriteString(szLine);
	szLine.Format("Ej Up Delay,%d,(10 - 30)\n",			m_lEjectorUpDelay);
	oFile.WriteString(szLine);
	szLine.Format("Ej Down Delay,%d\n",					m_lEjectorDownDelay);
	oFile.WriteString(szLine);
	szLine.Format("Weak Blow Delay,%d,(0 - 30)\n",		m_lWeakBlowOffDelay);
	oFile.WriteString(szLine);
	szLine.Format("High Blow Delay,%d,(0 - 50)\n",		m_lHighBlowTime);
	oFile.WriteString(szLine);
	szLine.Format("PR Delay,%d,(10 - 50)\n",			m_lPRDelay);
	oFile.WriteString(szLine);
	szLine.Format("WT T Delay,%d,(30 - 50)\n",			m_lWTTDelay);
	oFile.WriteString(szLine);
	szLine.Format("BT Delay,%d,(50 - 120)\n",			m_lBinTableDelay);
	oFile.WriteString(szLine);
	szLine.Format("VAC Off Delay,%d\n",					m_lEjVacOffDelay);
	oFile.WriteString(szLine);
	szLine.Format("Head PrePick Delay,%d\n",			m_lHeadPrePickDelay);
	oFile.WriteString(szLine);
	szLine.Format("Sync Pick Delay,%d\n",				m_lSyncPickDelay);
	oFile.WriteString(szLine);

	oFile.Close();
	szMsg.Format("BH: Generate Configuration Data open done");
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	return TRUE;
}


BOOL CBondHead::InitMinMachineDelays()
{
	//v3.62
	m_lMinBinTableDelay		= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "BT Delay");
	m_lMinPRDelay			= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "PR Delay");
	m_lMinWTTDelay			= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "WTT Delay");	

	m_lMinPickDelay			= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "Pick Delay");	
	m_lMinBondDelay			= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "Bond Delay");

	m_lMinArmPickDelay		= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "APick Delay");
	m_lMinHeadPickDelay		= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "HPick Delay");	
	m_lMinArmBondDelay		= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "ABond Delay");
	m_lMinHeadBondDelay		= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "HBond Delay");
	
	m_lMinEjectorUpDelay	= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "EjUp Delay");
	m_lMinEjectorDownDelay	= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "EjDn Delay");
	m_lMinEjReadyDelay		= GetDefaultMachineDelay(MS896A_CFG_CH_BONDHEAD, "EjReady Delay");
/*
	CString szTemp;
	szTemp.Format("BH: %d,  %d,  %d", m_lMinBinTableDelay, m_lMinPRDelay, m_lMinWTTDelay);
	DisplayMessage(szTemp);
	szTemp.Format("BH: %d,  %d", m_lMinPickDelay, m_lMinBondDelay);
	DisplayMessage(szTemp);
	szTemp.Format("BH: %d,  %d,  %d,  %d", m_lMinArmPickDelay, m_lMinHeadPickDelay, m_lMinArmBondDelay, m_lMinHeadBondDelay);
	DisplayMessage(szTemp);
	szTemp.Format("BH: %d,  %d,  %d", m_lMinEjectorUpDelay, m_lMinEjectorDownDelay, m_lMinEjReadyDelay);
	DisplayMessage(szTemp);
*/
	return TRUE;
}


BOOL CBondHead::LogWaferInformation(BOOL bOnLoadMap)
{
	if (IsBurnIn() == TRUE)
	{
		return FALSE;
	}


	if (m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		return FALSE;
	}	
	else
	{
		CString szCurrentAlgorithm, szCurrentPath;

		m_WaferMapWrapper.GetAlgorithm(szCurrentAlgorithm, szCurrentPath);
		
		//No need to log wafer info if this is Pick & Place
		if ((szCurrentAlgorithm == "Pick and Place: TLH-Circle") || (szCurrentAlgorithm == "Pick and Place: BLH-Circle") || 
				(szCurrentAlgorithm == "Pick and Place: TLV-Circle") || (szCurrentAlgorithm == "Pick and Place: TRV-Circle"))
		{
			return FALSE;
		}
	}


	CStringMapFile WaferRecord;
	BOOL bResetValue = FALSE;  	
	BOOL bCreate = FALSE;  	
	BOOL bUpdate = FALSE;  	
	BOOL bFileExist = FALSE;

	ULONG ulTotalWaferNo = 0;	
	ULONG ulTotalDie = 0;
	ULONG ulTotal = 0;
	ULONG ulLeft = 0;
	ULONG ulPick = 0;
	ULONG ulTotalPicked = 0;
	ULONG ulTotalInWafer = 0;
	ULONG ulTotalLeft = 0;
	CUIntArray aulGradeList;
	INT i = 0;
	ULONG ulMaxRow = 0, ulMaxCol = 0;
	ULONG ulRow, ulCol; 
	BOOL bIsNGGrade = FALSE;
	CUIntArray aulSelectedGradeList;
	CUIntArray unaAvaGradeList;
	ULONG ulDieState = 0;

	//Check file exist if this function is not called in load map seqeunce
	if ((_access(MSD_WAFER_RECORD_FILE, 0)) == -1)
	{
		bFileExist = FALSE;
	}
	else
	{
		bFileExist = TRUE;
	}


	if (bOnLoadMap == TRUE)
	{
		bResetValue = TRUE;  	
		bCreate = TRUE;  	
		bUpdate = FALSE; 

		//Check total wafer no is > 1000 or not when loading map
		if (bFileExist == TRUE)
		{
			if (WaferRecord.Open(MSD_WAFER_RECORD_FILE, FALSE, TRUE) == 1)
			{
				ulTotalWaferNo	= (WaferRecord)["Total Wafer"];
				WaferRecord.Close();
			}
			else
			{
				CMSFileUtility::Instance()->LogFileError("Cannot Open File: WaferRecord msd");
			}

			//Move waferrecord file into other folder and the remove file
			if (ulTotalWaferNo >= BH_MAX_WAFER_NO)
			{
				CopyFile(MSD_WAFER_RECORD_FILE, MSD_WAFER_RECORD_HISTORY_BKF, FALSE);
				DeleteFile(MSD_WAFER_RECORD_FILE); 
				bFileExist = FALSE;
				ulTotalWaferNo = 0;		//v3.71T5
			}

			//ulTotalWaferNo = 0;		//v3.71T5	//commented out here for PLLM REBEL bug
		}
	}
	else
	{
		bResetValue = FALSE;  	
		bCreate = !bFileExist;	
		bUpdate = bFileExist; 
	}

	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
	m_WaferMapWrapper.GetAvailableGradeList(unaAvaGradeList);

	if (WaferRecord.Open(MSD_WAFER_RECORD_FILE, FALSE, TRUE) == 1)
	{
		ulTotalWaferNo	= (WaferRecord)["Total Wafer"];

		if (bResetValue == TRUE)
		{
			m_ulCurrWaferBondDieCount	= 0;
			m_ulCurrWaferRotateDieCount	= 0;
			m_ulCurrWaferEmptyDieCount	= 0;
			m_ulCurrWaferDefectDieCount	= 0;
			m_ulCurrWaferChipDieCount	= 0;		
			m_ulCurrWaferInkDieCount	= 0;		
			m_ulCurrWaferBadCutDieCount	= 0;
			m_dCurrWaferSelGradeYield	= 0.0;
			m_ulCurrWaferMissingDieCount = 0;
			

			//(*m_psmfSRam)["BondHead"]["CurrentWafer"]["MissingDie"] = m_ulCurrWaferMissingDieCount;

			if (m_pulStorage != NULL)
			{
				*(m_pulStorage + BH_CURWAF_BONDEDDIE_OFFSET)	= 0;
				*(m_pulStorage + BH_CURWAF_DEFECTDIE_OFFSET)	= 0;
				*(m_pulStorage + BH_CURWAF_CHIPDIE_OFFSET)		= 0;
				*(m_pulStorage + BH_CURWAF_INKDIE_OFFSET)		= 0;
				*(m_pulStorage + BH_CURWAF_BADCUTDIE_OFFSET)	= 0;
				*(m_pulStorage + BH_CURWAF_ROTATEDIE_OFFSET)	= 0;
				*(m_pulStorage + BH_CURWAF_EMPTYDIE_OFFSET)		= 0;
				*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET)	= 0;
			}
		}
		else
		{
			m_ulCurrWaferBondDieCount	= 0;
			m_ulCurrWaferDefectDieCount	= 0;
			m_ulCurrWaferChipDieCount	= 0;		
			m_ulCurrWaferInkDieCount	= 0;		
			m_ulCurrWaferBadCutDieCount	= 0;
			m_ulCurrWaferRotateDieCount	= 0;
			m_ulCurrWaferEmptyDieCount	= 0;
			m_dCurrWaferSelGradeYield	= 0.0;
			m_ulCurrWaferMissingDieCount = 0;
			ulTotalInWafer = 0;
			ulTotalLeft = 0;

			for (i = 0; i < aulSelectedGradeList.GetSize(); i++)
			{
				m_WaferMapWrapper.GetStatistics(aulSelectedGradeList.GetAt(i), ulLeft, ulPick, ulTotal);
				ulTotalPicked += ulPick;
				ulTotalInWafer += ulTotal;
				ulTotalLeft += ulLeft;
			}

			if (m_WaferMapWrapper.IsMapValid() == TRUE)
			{
				if (!WM_CWaferMap::Instance()->GetWaferMapDimension(ulMaxRow, ulMaxCol))
				{
					WaferRecord.Close();
					return FALSE;
				}

				for (ulRow = 0; ulRow < ulMaxRow; ulRow++)
				{
					for (ulCol = 0; ulCol < ulMaxCol; ulCol++)
					{

						ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
						
						// Missing Die Check Since it has no grade so check first
						if (ulDieState > WT_MAP_DIESTATE_MISSING)
						{
							m_ulCurrWaferMissingDieCount++;
							continue;
						}

						bIsNGGrade = TRUE;
					
						for (INT i = 0; i < aulSelectedGradeList.GetSize(); i++)
						{
							if ((UCHAR)(aulSelectedGradeList.GetAt(i)) == m_WaferMapWrapper.GetGrade(ulRow, ulCol))
								
							{
								bIsNGGrade = FALSE;
								break;
							}	
						}
						
						if (bIsNGGrade == TRUE)
						{
							continue;
						}
						
						if (ulDieState == WT_MAP_DIESTATE_DEFECT)
						{
							m_ulCurrWaferDefectDieCount++;
						}

						else if (ulDieState == WT_MAP_DIESTATE_INK)
						{
							m_ulCurrWaferInkDieCount++;
						}

						else if (ulDieState == WT_MAP_DIESTATE_CHIP)
						{
							m_ulCurrWaferChipDieCount++;
						}

						else if (ulDieState == WT_MAP_DIESTATE_BADCUT)
						{
							m_ulCurrWaferBadCutDieCount++;
						}

						else if (IsDieEmpty(ulDieState))
						{
							m_ulCurrWaferEmptyDieCount++;
						}
						
						else if (ulDieState == WT_MAP_DIESTATE_ROTATE)
						{
							m_ulCurrWaferRotateDieCount++;
						}
					}
				}
			}
			
			// Updated the bond die count by cal total picked - missing
			m_ulCurrWaferBondDieCount = ulTotalPicked - m_ulCurrWaferMissingDieCount;
		}

		if (bCreate == TRUE)
		{
			ulTotalWaferNo++;
			//m_WaferMapWrapper.GetAvailableGradeList(aulGradeList);
			m_WaferMapWrapper.GetSelectedGradeList(aulGradeList);

			for (i = 0; i < aulGradeList.GetSize(); i++)
			{
				m_WaferMapWrapper.GetStatistics((UCHAR)aulGradeList[i], ulLeft, ulPick, ulTotal);
				ulTotalDie += ulTotal;
			}

			(WaferRecord)["Total Wafer"]				= ulTotalWaferNo;
			(WaferRecord)[ulTotalWaferNo]["TotalDie"]	= ulTotalDie;
			(WaferRecord)[ulTotalWaferNo]["WaferID"]	= m_szWaferId;
			(WaferRecord)[ulTotalWaferNo]["Empty"]		= m_ulCurrWaferEmptyDieCount;
			(WaferRecord)[ulTotalWaferNo]["Bond"]		= m_ulCurrWaferBondDieCount;
			(WaferRecord)[ulTotalWaferNo]["Defect"]		= m_ulCurrWaferDefectDieCount;
			(WaferRecord)[ulTotalWaferNo]["Chip"]		= m_ulCurrWaferChipDieCount;
			(WaferRecord)[ulTotalWaferNo]["Ink"]		= m_ulCurrWaferInkDieCount;
			(WaferRecord)[ulTotalWaferNo]["Rotate"]		= m_ulCurrWaferRotateDieCount;
			(WaferRecord)[ulTotalWaferNo]["BadCut"]		= m_ulCurrWaferBadCutDieCount;
			(WaferRecord)[ulTotalWaferNo]["MissingDie"]	= m_ulCurrWaferMissingDieCount;
			(WaferRecord)[ulTotalWaferNo]["TotalDieBeforeSCN"] = CPreBondEvent::m_nMapOrginalCount;

			//v3.70T4	//PLLM REBEL only
			BOOL bUse2DBarCode = (BOOL)(LONG)(*m_psmfSRam)["WaferLoaderStn"]["2D Barcode"];
			(WaferRecord)[ulTotalWaferNo]["2DBarcode"]	= bUse2DBarCode;

			if (ulTotalDie == 0)
			{
				m_dCurrWaferSelGradeYield = 0.0;
			}
			else
			{
				m_dCurrWaferSelGradeYield = ((DOUBLE)m_ulCurrWaferBondDieCount / (DOUBLE)ulTotalDie) * 100;
			}

			(WaferRecord)[ulTotalWaferNo]["Yield"] = m_dCurrWaferSelGradeYield;

			//Update Current wafer info from Wafermap
			if (m_pulStorage != NULL)
			{
				*(m_pulStorage + BH_CURWAF_BONDEDDIE_OFFSET)	= m_ulCurrWaferBondDieCount;
				*(m_pulStorage + BH_CURWAF_DEFECTDIE_OFFSET)	= m_ulCurrWaferDefectDieCount;
				*(m_pulStorage + BH_CURWAF_CHIPDIE_OFFSET)		= m_ulCurrWaferChipDieCount;
				*(m_pulStorage + BH_CURWAF_INKDIE_OFFSET)		= m_ulCurrWaferInkDieCount;
				*(m_pulStorage + BH_CURWAF_BADCUTDIE_OFFSET)	= m_ulCurrWaferBadCutDieCount;
				*(m_pulStorage + BH_CURWAF_ROTATEDIE_OFFSET)	= m_ulCurrWaferRotateDieCount;
				*(m_pulStorage + BH_CURWAF_EMPTYDIE_OFFSET)		= m_ulCurrWaferEmptyDieCount;
				*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET)	= m_ulCurrWaferMissingDieCount;
			}
		}


		if (bUpdate == TRUE)
		{
			//Get total die of this wafer
			ulTotalDie = (WaferRecord)[ulTotalWaferNo]["TotalDie"];

			//Update counter
			(WaferRecord)[ulTotalWaferNo]["Empty"]		= m_ulCurrWaferEmptyDieCount;
			(WaferRecord)[ulTotalWaferNo]["Bond"]		= m_ulCurrWaferBondDieCount;
			(WaferRecord)[ulTotalWaferNo]["Defect"]		= m_ulCurrWaferDefectDieCount;
			(WaferRecord)[ulTotalWaferNo]["Chip"]		= m_ulCurrWaferChipDieCount;
			(WaferRecord)[ulTotalWaferNo]["Ink"]		= m_ulCurrWaferInkDieCount;
			(WaferRecord)[ulTotalWaferNo]["Rotate"]		= m_ulCurrWaferRotateDieCount;
			(WaferRecord)[ulTotalWaferNo]["BadCut"]		= m_ulCurrWaferBadCutDieCount;
			(WaferRecord)[ulTotalWaferNo]["MissingDie"]	= m_ulCurrWaferMissingDieCount;
			(WaferRecord)[ulTotalWaferNo]["TotalDieBeforeSCN"] = CPreBondEvent::m_nMapOrginalCount;

			if (ulTotalDie == 0)
			{
				(WaferRecord)[ulTotalWaferNo]["Yield"]		= 0.0;
			}
			else
			{
				(WaferRecord)[ulTotalWaferNo]["Yield"]		= ((DOUBLE)m_ulCurrWaferBondDieCount / (DOUBLE)ulTotalDie) * 100;
			}

			//Update Current wafer into from Wafermap
			if (m_pulStorage != NULL)
			{
				*(m_pulStorage + BH_CURWAF_BONDEDDIE_OFFSET)	= m_ulCurrWaferBondDieCount;
				*(m_pulStorage + BH_CURWAF_DEFECTDIE_OFFSET)	= m_ulCurrWaferDefectDieCount;
				*(m_pulStorage + BH_CURWAF_CHIPDIE_OFFSET)		= m_ulCurrWaferChipDieCount;
				*(m_pulStorage + BH_CURWAF_INKDIE_OFFSET)		= m_ulCurrWaferInkDieCount;
				*(m_pulStorage + BH_CURWAF_BADCUTDIE_OFFSET)	= m_ulCurrWaferBadCutDieCount;
				*(m_pulStorage + BH_CURWAF_ROTATEDIE_OFFSET)	= m_ulCurrWaferRotateDieCount;
				*(m_pulStorage + BH_CURWAF_EMPTYDIE_OFFSET)		= m_ulCurrWaferEmptyDieCount;
				*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET)	= m_ulCurrWaferMissingDieCount;
			}
		}

		WaferRecord.Update();
		WaferRecord.Close();

		return TRUE;
	}

	return FALSE;
}


BOOL CBondHead::UpdateDieCounter(VOID)
{
	ULONG ulMaxRow = 0, ulMaxCol = 0;
	ULONG ulRow, ulCol; 
	CString szLogText;
	BOOL bIsNGGrade = FALSE;
	ULONG ulLeft = 0, ulPick = 0, ulTotal = 0, ulTotalPicked = 0, ulTotalInWafer = 0, ulTotalLeft = 0;
	ULONG ulDieState;


	if (m_pulStorage == NULL)
	{
		return FALSE;
	}

	if (IsBurnIn() == TRUE)
	{
		return FALSE;
	}


	m_ulCurrWaferBondDieCount	= 0;
	m_ulCurrWaferDefectDieCount	= 0;
	m_ulCurrWaferBadCutDieCount	= 0;
	m_ulCurrWaferRotateDieCount	= 0;
	m_ulCurrWaferEmptyDieCount	= 0;
	m_ulCurrWaferMissingDieCount = 0;
	m_ulCurrWaferChipDieCount = 0;
	m_dCurrWaferSelGradeYield	= 0.0;

	if (m_WaferMapWrapper.IsMapValid() == TRUE)
	{
		if (!WM_CWaferMap::Instance()->GetWaferMapDimension(ulMaxRow, ulMaxCol))
		{
			return FALSE;
		}

		szLogText.Format("Map Size %d,%d", ulMaxRow, ulMaxCol);
		CMSLogFileUtility::Instance()->BH_DieCounterLog(szLogText);

		CUIntArray aulSelectedGradeList, unaAvaGradeList;

		m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
		m_WaferMapWrapper.GetAvailableGradeList(unaAvaGradeList);

		for (INT i = 0; i < aulSelectedGradeList.GetSize(); i++)
		{
			m_WaferMapWrapper.GetStatistics(aulSelectedGradeList.GetAt(i), ulLeft, ulPick, ulTotal);
			ulTotalPicked += ulPick;
			ulTotalInWafer += ulTotal;
			ulTotalLeft += ulLeft;
		}

		for (ulRow = 0; ulRow < ulMaxRow; ulRow++)
		{
			for (ulCol = 0; ulCol < ulMaxCol; ulCol++)
			{
				ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
				
				// Missing Die Check Since it has no grade so check first
				if (ulDieState > WT_MAP_DIESTATE_MISSING)
				{
					m_ulCurrWaferMissingDieCount++;
					continue;
				}

				bIsNGGrade = TRUE;
			
				for (INT i = 0; i < aulSelectedGradeList.GetSize(); i++)
				{
					if ((UCHAR)(aulSelectedGradeList.GetAt(i)) == m_WaferMapWrapper.GetGrade(ulRow, ulCol))
						
					{
						bIsNGGrade = FALSE;
						break;
					}	
				}
				
				if (bIsNGGrade == TRUE)
				{
					continue;
				}
				
				if (ulDieState == WT_MAP_DIESTATE_DEFECT)
				{
					m_ulCurrWaferDefectDieCount++;
				}

				else if (ulDieState == WT_MAP_DIESTATE_INK)
				{
					m_ulCurrWaferInkDieCount++;
				}

				else if (ulDieState == WT_MAP_DIESTATE_CHIP)
				{
					m_ulCurrWaferChipDieCount++;
				}

				else if (ulDieState == WT_MAP_DIESTATE_BADCUT)
				{
					m_ulCurrWaferBadCutDieCount++;
				}

				else if (IsDieEmpty(ulDieState))
				{
					m_ulCurrWaferEmptyDieCount++;
				}
				
				else if (ulDieState == WT_MAP_DIESTATE_ROTATE)
				{
					m_ulCurrWaferRotateDieCount++;
				}
			}
		}
		
		// Calculate the bond die count by total picked - missing
		m_ulCurrWaferBondDieCount = ulTotalPicked - m_ulCurrWaferMissingDieCount;
	}

	CMSLogFileUtility::Instance()->BH_DieCounterLog("End Get DieState");

	CMSLogFileUtility::Instance()->BH_DieCounterLog("Update local counter");

	//Update Current wafer counter
	*(m_pulStorage + BH_CURWAF_BONDEDDIE_OFFSET)	= m_ulCurrWaferBondDieCount;
	*(m_pulStorage + BH_CURWAF_DEFECTDIE_OFFSET)	= m_ulCurrWaferDefectDieCount;
	*(m_pulStorage + BH_CURWAF_CHIPDIE_OFFSET)		= m_ulCurrWaferChipDieCount;
	*(m_pulStorage + BH_CURWAF_INKDIE_OFFSET)		= m_ulCurrWaferInkDieCount;
	*(m_pulStorage + BH_CURWAF_BADCUTDIE_OFFSET)	= m_ulCurrWaferBadCutDieCount;
	*(m_pulStorage + BH_CURWAF_ROTATEDIE_OFFSET)	= m_ulCurrWaferRotateDieCount;
	*(m_pulStorage + BH_CURWAF_EMPTYDIE_OFFSET)		= m_ulCurrWaferEmptyDieCount;
	*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET)	= m_ulCurrWaferMissingDieCount;


	//Update Accumulate counter
	*(m_pulStorage + BH_BONDDIE_OFFSET)		= m_ulBondDieCount;			
	*(m_pulStorage + BH_DEFECTDIE_OFFSET)	= m_ulDefectDieCount;
	*(m_pulStorage + BH_BADCUTDIE_OFFSET)	= m_ulBadCutDieCount;
	*(m_pulStorage + BH_ROTATEDIE_OFFSET)	= m_ulRotateDieCount;
	*(m_pulStorage + BH_EMPTYDIE_OFFSET)	= m_ulEmptyDieCount;
	*(m_pulStorage + BH_PICKDIE_OFFSET)		= m_ulPickDieCount;


	CMSLogFileUtility::Instance()->BH_DieCounterLog("Update MEM counter");

	if (IsSecsGemInit() == TRUE)
	{
		SendCE_BondCounter();
		SendCE_CurrWFCounter();
		SendCE_ToolLife();
		SendCE_EjKOffset();

		if (m_lBondCount == 0)
		{
			SendEquipmentBasicStatistics();
		}
		m_lBondCount++;
		if (m_lBondCount == 10)
		{
			m_lBondCount = 0;
		}
	}

	return TRUE;
}


BOOL CBondHead::ResetCurrWaferCounter(VOID)
{
	//Reset Current wafer counter
	m_ulCurrWaferBondDieCount	= 0;
	m_ulCurrWaferDefectDieCount	= 0;
	m_ulCurrWaferBadCutDieCount	= 0;
	m_ulCurrWaferRotateDieCount	= 0;
	m_ulCurrWaferEmptyDieCount	= 0;
	m_ulCurrWaferMissingDieCount = 0;	
	m_dCurrWaferSelGradeYield	= 0.0;

	m_bBHZ2HasDie = FALSE;
	m_bBHZ1HasDie = FALSE;
	//(*m_psmfSRam)["BondHead"]["CurrentWafer"]["MissingDie"] = m_ulCurrWaferMissingDieCount;

	if (m_pulStorage != NULL)
	{
		*(m_pulStorage + BH_CURWAF_BONDEDDIE_OFFSET)	= m_ulCurrWaferBondDieCount;
		*(m_pulStorage + BH_CURWAF_DEFECTDIE_OFFSET)	= m_ulCurrWaferDefectDieCount;
		*(m_pulStorage + BH_CURWAF_CHIPDIE_OFFSET)		= m_ulCurrWaferChipDieCount;
		*(m_pulStorage + BH_CURWAF_INKDIE_OFFSET)		= m_ulCurrWaferInkDieCount;
		*(m_pulStorage + BH_CURWAF_BADCUTDIE_OFFSET)	= m_ulCurrWaferBadCutDieCount;
		*(m_pulStorage + BH_CURWAF_ROTATEDIE_OFFSET)	= m_ulCurrWaferRotateDieCount;
		*(m_pulStorage + BH_CURWAF_EMPTYDIE_OFFSET)		= m_ulCurrWaferEmptyDieCount;
		*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET)	= m_ulCurrWaferMissingDieCount;
	}

	return TRUE;
}


BOOL CBondHead::SaveMachineStatistic(VOID)
{
	return SaveBhData();
}


VOID CBondHead::FlushMessage()
{
	if (m_comServer.ScanRequest(10))
	{
		m_comServer.ReadRequest();
	}
}

VOID CBondHead::UpdateStationData()
{
	m_oBondHeadDataBlock.m_szCycleTime.Format("%.f", m_dCycleTime);
	m_oBondHeadDataBlock.m_szAvgCycleTime.Format("%.f", m_dAvgCycleTime);
	m_oBondHeadDataBlock.m_szColletCleanLimit.Format("%d", m_ulMaxCleanCount);
	m_oBondHeadDataBlock.m_szPickLevelZ1.Format("%d", m_lPickLevel_Z);
	m_oBondHeadDataBlock.m_szPickLevelZ2.Format("%d", m_lPickLevel_Z2);
	m_oBondHeadDataBlock.m_szPickPosition.Format("%d", m_lPickPos_T);
	m_oBondHeadDataBlock.m_szBondLevelZ1.Format("%d", m_lBondLevel_Z);
	m_oBondHeadDataBlock.m_szBondLevelZ2.Format("%d", m_lBondLevel_Z2);
	m_oBondHeadDataBlock.m_szBondPosition.Format("%d", m_lBondPos_T);
	m_oBondHeadDataBlock.m_szEjectorUpLevel.Format("%d", m_lEjectLevel_Ej);
	m_oBondHeadDataBlock.m_szEjStandbyLevel.Format("%d", m_lStandbyLevel_Ej);
	m_oBondHeadDataBlock.m_szEnableColletJam = m_oBondHeadDataBlock.ConvertBoolToOnOff(m_bCheckColletJam);
	m_oBondHeadDataBlock.m_szColletJamRetryCount.Format("%d", m_ulColletJam_Retry);
	m_oBondHeadDataBlock.m_szEnableMissingDie = m_oBondHeadDataBlock.ConvertBoolToOnOff(m_bCheckMissingDie);
	m_oBondHeadDataBlock.m_szMissingDieRetryCount.Format("%d", m_lMissingDie_Retry);
	m_oBondHeadDataBlock.m_szEnableSyncPick = m_oBondHeadDataBlock.ConvertBoolToOnOff(m_bEnableSyncMotionViaEjCmd);
	m_oBondHeadDataBlock.m_szSyncMoveTriggerLevel.Format("%d", m_lSyncTriggerValue);
	m_oBondHeadDataBlock.m_szSyncMovePreload.Format("%d", m_lSyncZOffset);

	m_oBondHeadDataBlock.m_szCleanColletPosition.Format("%d", m_lAutoCleanCollet_T);
	m_oBondHeadDataBlock.m_szCleanColletLevel.Format("%d", m_lAutoCleanCollet_Z);
	m_oBondHeadDataBlock.m_szStandbyLevel_Ej.Format("%d", m_lStandbyLevel_Ej);
	
	m_oBondHeadDataBlock.m_szEnableAutoCleanCollet = m_oBondHeadDataBlock.ConvertBoolToOnOff(m_bAutoCleanCollet);
	m_oBondHeadDataBlock.m_szEnabelCheckCoverSensor = m_oBondHeadDataBlock.ConvertBoolToOnOff(m_bCheckCoverSensor);

	m_oBondHeadDataBlock.m_szPickDelay.Format("%d",			m_lPickDelay);
	m_oBondHeadDataBlock.m_szBondDelay.Format("%d",			m_lBondDelay);
	m_oBondHeadDataBlock.m_szArmPickDelay.Format("%d",		m_lArmPickDelay);
	m_oBondHeadDataBlock.m_szArmBondDelay.Format("%d",		m_lArmBondDelay);
	m_oBondHeadDataBlock.m_szHeadPickDelay.Format("%d",		m_lHeadPickDelay);
	m_oBondHeadDataBlock.m_szHeadBondDelay.Format("%d",		m_lHeadBondDelay);
	m_oBondHeadDataBlock.m_szHeadPrePickDelay.Format("%d",	m_lHeadPrePickDelay);
	m_oBondHeadDataBlock.m_szSyncPickDelay.Format("%d",		m_lSyncPickDelay);

	m_oBondHeadDataBlock.m_szEjectorUpDelay.Format("%d",	m_lEjectorUpDelay);
	m_oBondHeadDataBlock.m_szEjectorDownDelay.Format("%d",	m_lEjectorDownDelay);
	m_oBondHeadDataBlock.m_szEjReadyDelay.Format("%d",		m_lEjReadyDelay);
	m_oBondHeadDataBlock.m_szEjVacOffDelay.Format("%d",		m_lEjVacOffDelay);

	m_oBondHeadDataBlock.m_szPRDelay.Format("%d",			m_lPRDelay);
	m_oBondHeadDataBlock.m_szWTTDelay.Format("%d",			m_lWTTDelay);
	m_oBondHeadDataBlock.m_szBinTableDelay.Format("%d",		m_lBinTableDelay);

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	pApp->SetBondHeadDataBlock(m_oBondHeadDataBlock);
}



BOOL CBondHead::IsFrontCoverLocked()
{
	return TRUE;

	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	//if (m_piFrontCoverClose == NULL)	//v3.60
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return TRUE;
	}
#endif

	if (m_bIsFrontCoverExist == TRUE)
	{
		m_bFrontCoverLock = CMS896AStn::MotionReadInputBit(MAIN_SI_FRONT_COVER);
	}
	else
	{
		m_bFrontCoverLock = TRUE;
	}

	return m_bFrontCoverLock;
}


BOOL CBondHead::IsSideCoverLocked()
{
	return TRUE;
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	//if (m_piSideCoverClose == NULL)	//v3.60
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return TRUE;
	}
#endif

	if (m_bIsSideCoverExist == TRUE)
	{
		m_bSideCoverLock = CMS896AStn::MotionReadInputBit(MAIN_SI_LEFT_DOOR_COVER);
	}
	else
	{
		m_bSideCoverLock = TRUE;
	}

	return m_bSideCoverLock;
}

BOOL CBondHead::IsBinElevatorCoverLocked()
{
	return TRUE;
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	//if (m_piSideCoverClose == NULL)	//v3.60
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return TRUE;
	}
#endif
	
	if (m_bIsBinElevatorCoverExist == TRUE)
	{
		m_bBinElevatorCoverLock = CMS896AStn::MotionReadInputBit(BH_SI_BINH_EL_COVER);
	}
	else
	{
		m_bBinElevatorCoverLock = TRUE;
	}

	return m_bBinElevatorCoverLock;
}

BOOL CBondHead::IsEMOTriggered()
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
#ifdef NU_MOTION
	try
	{
		return CMS896AStn::MotionReadInputBit(BH_SI_EMO);	//For MS100 only
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
#else
	return FALSE;											//Not available for MS899/810
#endif
}
/*
BOOL CBondHead::IsBHFanTurnOn()
{
	if (m_fHardware == FALSE)
		return FALSE;
#ifdef NU_MOTION
	try
	{
		BOOL bBit = CMS896AStn::MotionReadInputBit(BH_SI_BH_FAN);

		return bBit;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
#else
	return FALSE;		//Not available for MS899/810; assume always ON
#endif
}
*/

BOOL CBondHead::IsMS60TempertureUnderHeat(BOOL bCheckNow)		//v4.53A19
{
	BOOL bStatus = FALSE;
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (!IsMS60())
	{
		return FALSE;
	}
	if (!CMS896AStn::m_bDBHThermostat)
	{
		return FALSE;
	}

	if (!bCheckNow)
	{
		//m_ulBHTThermostatCounter++;
		if (m_ulBHTThermostatCounter < 10)
		{
			return FALSE;
		}
		//m_ulBHTThermostatCounter = 0;
	}

	LONG lValue = CMS896AStn::MotionReadInputADCPort(BHT_THERMOSTAT);
	m_ulBHTThermostatADC		= lValue;
	m_dBHTThermostatReading		= (DOUBLE) (250.0 * lValue / 32767.0);
	m_ulBHTThermostatReading	= (ULONG) m_dBHTThermostatReading;

	if ((m_ulBHTThermostatReading > 0) && 
		(m_ulBHTThermostatReading < MS60_BH_MIN_TEMPERATURE_PREHEAT))		
	{
		bStatus = TRUE;
		m_ulBHTThermostatCounter = 0;
		(*m_psmfSRam)["MS"]["ThermalCriticalError"] = FALSE;	//v4.59A18
	}

	if (bStatus)	//Log if only UnderHeat
	{
		CString szLog;
		szLog.Format("BH Temp (UnderHeat) = %lu deg (LLimit=45), ADC = %ld, Status = %d", 
			m_ulBHTThermostatReading, lValue, bStatus);
		CMSLogFileUtility::Instance()->BH_LogStatus(szLog);
	}

	return bStatus;
}

BOOL CBondHead::IsDBHThermalAlarm(LONG &lAlarmCode)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}

	if (!CMS896AStn::m_bDBHThermalCheck)
	{
		return FALSE;
	}

#ifdef NU_MOTION
	try
	{
		BOOL bFrontBit = !CMS896AStn::MotionReadInputBit(BH_SI_FRONT_THERMAL);
		BOOL bRightBit = !CMS896AStn::MotionReadInputBit(BH_SI_RIGHT_THERMAL);

		if (bFrontBit && bRightBit)
		{
			lAlarmCode = 3;
		}
		else if (bFrontBit)
		{
			lAlarmCode = 1;
		}
		else if (bRightBit)
		{
			lAlarmCode = 2;
		}

		return (bFrontBit || bRightBit);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
#else
	return FALSE;		//Not available for MS899/810; assume always ON
#endif
}

VOID CBondHead::SetFrontCoverLock(BOOL bSet)
{
	return;
	if ((m_fHardware == FALSE) || (m_bIsFrontCoverExist == FALSE))
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return;
	}
#endif
	CMS896AStn::MotionSetOutputBit(BH_SO_FRONT_COVER, bSet);
}

VOID CBondHead::SetSideCoverLock(BOOL bSet)
{
	return;

	if ((m_fHardware == FALSE) || (m_bIsSideCoverExist == FALSE))
	{
		return;
	}

#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return;
	}
#endif
	CMS896AStn::MotionSetOutputBit(BH_SO_SIDE_COVER, bSet);
}

VOID CBondHead::SetBinElevatorCoverLock(BOOL bSet)
{
	return;

	if ((m_fHardware == FALSE) || (m_bIsBinElevatorCoverExist == FALSE))
	{
		return;
	}

#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return;
	}
#endif
	CMS896AStn::MotionSetOutputBit(BH_SO_BIN_EL_COVER, bSet);
}

VOID CBondHead::SetCleanColletPocket(BOOL bOn)	// FALSE TO ON VALVE, TRUE TO OFF	//v3.65
{
	return;

	if (m_fHardware == FALSE)
	{
		return;
	}

	if (m_bDisableBH)
	{
		return;
	}
	
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletCleanPocket = pApp->GetFeatureStatus(MS896A_FUNC_COLLET_CLEAN_POCKET);
	if (!bColletCleanPocket)
	{
		return;
	}

	CMS896AStn::MotionSetOutputBit(BH_SO_CLEANCOLLET_POCKET, bOn);

	CMS896AStn::m_bCleanColletPocketOn = bOn;
}

VOID CBondHead::SetEjectorVacuum(BOOL bSet)
{
	return;

	if (!m_fHardware)
	{
		return;
	}
	if (m_bDisableBH)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit("oEjectorVacuum", bSet);
	return;
}

VOID CBondHead::ResetBondHeadFanTimerAndCounter(BOOL bOn)
{
	m_lBondHeadCoolingSecond = 0;			//v4.47T3
	if ( bOn == TRUE )
	{
		m_bCoolingFanWillTurnOff = FALSE;
	}
	else
	{
		m_bCoolingFanWillTurnOff = TRUE;
	}
}

//v4.36
VOID CBondHead::SetTestBit(BOOL bOn)					//v4.36	//For internal test purpose only
{
	if (!m_fHardware)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit("obTest", bOn);
	return;
}

BOOL CBondHead::IsNGPickAtDnPos()
{
	if (!m_fHardware)
	{
		return FALSE;
	}
	if (!m_bES101Configuration)
	{
		return FALSE;
	}
	return CMS896AStn::MotionReadInputBit(NGPICK_SI_DN);
}

BOOL CBondHead::IsNGPickAtUpPos()
{
	if (!m_fHardware)
	{
		return FALSE;
	}
	if (!m_bES101Configuration)
	{
		return FALSE;
	}
	return CMS896AStn::MotionReadInputBit(NGPICK_SI_UP);
}

BOOL CBondHead::IsTensionCheck()
{
	if (!m_fHardware)
	{
		return FALSE;
	}
	if (!m_bES101Configuration)
	{
		return FALSE;
	}
	return !CMS896AStn::MotionReadInputBit(NGPICK_SI_TENSIONCHECK);		//v4.27
}

BOOL CBondHead::IsTapeEmpty()
{
	if (!m_fHardware)
	{
		return FALSE;
	}
	if (!m_bES101Configuration)
	{
		return FALSE;
	}
	return CMS896AStn::MotionReadInputBit(NGPICK_SI_TAPEEMPTY);
}

VOID CBondHead::SetNGPickReelStop(BOOL bOn)
{
	if (!m_fHardware)
	{
		return;
	}
	if (!m_bES101Configuration)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(NGPICK_SO_REELSTOP, bOn);
}

VOID CBondHead::SetNGPickUpDn(BOOL bDn)
{
	if (!m_fHardware)
	{
		return;
	}
	if (!m_bES101Configuration)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(NGPICK_SO_UPDN, bDn);
}

BOOL CBondHead::IsZMotorsEnable()
{
	return TRUE;

	BOOL bZMotorOn = FALSE;
	BOOL bZMotor2On = TRUE;
	
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBH)		//v3.60
	{
		return TRUE;
	}

	if (m_bSel_Z)
	{
		bZMotorOn = CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);
	}
	else
	{
		bZMotorOn = TRUE;    //v4.24T10
	}	

	if (m_bSel_Z2)
	{
		bZMotor2On = CMS896AStn::MotionIsPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2);
	}
	else
	{
		bZMotor2On = TRUE;    //v4.24T10
	}	

	return (bZMotorOn && bZMotor2On);
}

BOOL CBondHead::IsAllMotorsEnable()
{
	BOOL bZMotorOn	= FALSE;
	BOOL bZ2MotorOn = TRUE;		//v3.98T5
	BOOL bTMotorOn	= FALSE;

	return FALSE;

	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableBH)		//v3.60
	{
		return FALSE;
	}

	bZMotorOn	= CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);
	bTMotorOn	= CMS896AStn::MotionIsPowerOn(BH_AXIS_T, &m_stBHAxis_T);

#ifdef NU_MOTION
	if (m_bIsArm2Exist == TRUE)
	{
		bZ2MotorOn	= CMS896AStn::MotionIsPowerOn(BH_AXIS_Z2, &m_stBHAxis_Z2);
	}
#endif

	return (bZMotorOn & bTMotorOn & bZ2MotorOn);
}

BOOL CBondHead::LockPRMouseJoystick(BOOL bLock)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(BOOL), &bLock);

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetLockPrMouseJoystickCmd", stMsg);
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

BOOL CBondHead::LockWaferTableJoystick(BOOL bLock)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(BOOL), &bLock);

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetLockJoystickCmd", stMsg);
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

BOOL CBondHead::MoveWaferTableToDummyPos(BOOL bMove)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	BOOL bMoveToDummyPos = bMove;
	stMsg.InitMessage(sizeof(BOOL), &bMoveToDummyPos);

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "MoveTableToDummyPos", stMsg);
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

BOOL CBondHead::MoveBinTableToDummyPos(BOOL bMove)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	BOOL bMoveToDummyPos = bMove;
	stMsg.InitMessage(sizeof(BOOL), &bMoveToDummyPos);

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveTableToDummyPos", stMsg);
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

BOOL CBondHead::SetWaferTableJoystick(BOOL bState)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(BOOL), &bState);

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetJoystickCmd", stMsg);
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

BOOL CBondHead::AutoLearnBondLevelUsingCTSensor(LONG lPosition)
{
	INT			nStatus;
	LONG		lStepSize = 10;
	LONG		lMinPos = 0;

	//Check min limit value
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);								

	//Test missing die sensor reply
	SetPickVacuum(TRUE);
	Sleep(1000);
	if (IsColletJam() == TRUE)
	{
		OpDisplayAlarmPage(TRUE);	//v4.53A13
		//HmiMessage("Collet jam or no pressure\nPlease check!", "Auto learn level Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_1);	//v4.50A1
		SetPickVacuum(FALSE);
		return FALSE;
	}


	SetPickVacuum(TRUE);

	LONG lBTLevel = 0;
	//Start Learning Missing Die Sensor
	while (1)
	{
		Sleep(10);

		if (m_bUseCTSensorLearn)	//Use Contact Sensor	//v2.96T4
		{
			if (IsBHContact())
			{
				Sleep(10);
				if (IsBHContact())
				{
					break;
				}
			}
		}
		else						//Use air-flow sensor
		{
			if (IsColletJam() == TRUE)
			{
				Sleep(10);
				if (IsColletJam() == TRUE)
				{
					break;
				}
			}
		}

		if (lPosition == 1)			//Bond
		{	
			nStatus = Z_MoveTo((m_lBPGeneral_7 -= lStepSize), SFM_WAIT, TRUE);
			if ((m_lBPGeneral_7 <= lMinPos) || (nStatus != gnOK))
			{
				SetPickVacuum(FALSE);
				//HmiMessage("Learn Failed", "Auto learn level Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
				return FALSE;
				
			}
		}
		else if (lPosition == 2)	//AutoLearn Auto CleanCollet Z level for SanAn	//v4.46T21
		{
			nStatus = Z_MoveTo((m_lAutoCleanAutoLearnZ -= lStepSize), SFM_WAIT, TRUE);
			if ((m_lAutoCleanAutoLearnZ <= lMinPos) || (nStatus != gnOK))
			{
				CString szErr;
				szErr.Format("Auto Learn AutoCleanCollet Z1 level fails at %ld (%ld)", 
					m_lAutoCleanAutoLearnZ, lMinPos);
				SetErrorMessage(szErr);
				
				SetPickVacuum(FALSE);
				SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
				return FALSE;			
			}
		}
		else if( lPosition == 3 )	//	auto detect bin table level.
		{
			lBTLevel -= lStepSize;
			nStatus = Z_MoveTo(lBTLevel, SFM_WAIT, TRUE);
			if( (lBTLevel <= lMinPos) || (nStatus != gnOK) )
			{
				SetPickVacuum(FALSE);
				SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
				return FALSE;
			}
		}
		else	//Pick
		{
			nStatus = Z_MoveTo((m_lBPGeneral_6 -= lStepSize));
			if ((m_lBPGeneral_6 <= lMinPos) || (nStatus != gnOK))
			{
				SetPickVacuum(FALSE);
				//HmiMessage("Learn Failed", "Auto learn level Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
				return FALSE;
			}
		}
	}

	//Off Collet vacuum 
	SetPickVacuum(FALSE);

	if (m_bUseCTSensorLearn)
	{
		SetStatusMessage("Auto learn bondhead level CT Sensor completed");
	}
	else
	{
		SetStatusMessage("Auto learn bondhead level by AF sensor completed");
	}

	return TRUE;
}

BOOL CBondHead::AutoLearnBHZ1BondLevel(LONG &lEnc, BOOL bHome, LONG lOldEnc, LONG lMaxOffset)
{
	INT			nStatus;
	LONG		lStepSize = 10;
	LONG		lMinPos = 0;
	CString		szLog;
	//Check min limit value
	lMinPos = -9000;//GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);	

	LONG lLevel = lEnc;

	//Test missing die sensor reply
	SetPickVacuum(TRUE);
	Sleep(1000);
	if (IsColletJam() == TRUE)
	{
		OpDisplayAlarmPage(TRUE);	//v4.53A13
		SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_1);		//v4.49A12
		SetPickVacuum(FALSE);
		return FALSE;
	}

	SetPickVacuum(TRUE);

	//Start Learning Missing Die Sensor
	while (1)
	{
		Sleep(10);

		if (m_bUseCTSensorLearn)	//Use Contact Sensor	//v2.96T4
		{
			if (IsBHContact())
			{
				Sleep(10);
				if (IsBHContact())
				{
					break;
				}
			}
		}
		else						//Use air-flow sensor
		{
			if (IsColletJam() == TRUE)
			{
				Sleep(10);
				if (IsColletJam() == TRUE)
				{
					break;
				}
			}
		}

		if (!Z_IsPowerOn())
		{
			SetPickVacuum(FALSE);
			HmiMessage_Red_Yellow("ERROR: BHZ1 is not power ON!!!");	//v4.49A12
			Z_Home();
			return FALSE;
		}


		lLevel -= lStepSize;
		nStatus = Z_MoveTo(lLevel);

		if ((lLevel <= lMinPos) || (nStatus != gnOK))
		{
			SetPickVacuum(FALSE);
			Z_Home();
			SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_2);		//v4.49A12
			return FALSE;
		}

		if (lOldEnc != 0 && lMaxOffset != 0 && ((lLevel - lOldEnc) * -1 > lMaxOffset))
		{
			szLog.Format("Auto Learn BH1 Error,Old,%d,New,%d,offset,%d",lOldEnc,lLevel,lMaxOffset);
			SetErrorMessage(szLog);
			SetPickVacuum(FALSE);
			Z_MoveTo(m_lSwingLevel_Z);
			//Z_Home();
			SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_2);		//v4.49A12
			return FALSE;
		}
	}

	if (bHome)
	{
		//Off Collet vacuum 
		SetPickVacuum(FALSE);
		//GetEncoderValue();
		lEnc = lLevel;	//m_lEnc_Z;

		if (!Z_IsPowerOn())		//v4.49A12
		{
			Z_Home();
		}
		else
		{
			Z_MoveTo(m_lSwingLevel_Z);
		}
	}
	else
	{
		lEnc = lLevel;
	}

	return TRUE;
}


BOOL CBondHead::AutoLearnBondLevelUsingCTSensorZ2(LONG lPosition)
{
	INT			nStatus = 0;
	LONG		lStepSize = 10;
	LONG		lMinPos = 0;

	if (m_bIsArm2Exist == FALSE)
	{
		return TRUE;
	}

#ifdef NU_MOTION
	//Check min limit value
	lMinPos = GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);								

	m_lAutoCleanAutoLearnZ = 0;		//v4.46T21	//SanAn

	//Test missing die sensor reply
	SetPickVacuumZ2(TRUE);
	Sleep(1000);
	if (IsColletJamZ2() == TRUE)
	{
		OpDisplayAlarmPage(TRUE);	//v4.53A13
		//HmiMessage("Collet jam or no pressure\nPlease check!", "Auto learn level Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_1);		//v4.50A1
		SetPickVacuumZ2(FALSE);
		return FALSE;
	}


	SetPickVacuumZ2(TRUE);

	//Start Learning Missing Die Sensor
	while (1)
	{
		Sleep(10);

		if (m_bUseCTSensorLearn)	//Use Contact Sensor	//v2.96T4
		{
			if (IsBHContactZ2())
			{
				Sleep(10);
				if (IsBHContactZ2())
				{
					break;
				}
			}
		}
		else						//Use air-flow sensor
		{
			if (IsColletJamZ2() == TRUE)
			{
				Sleep(10);
				if (IsColletJamZ2() == TRUE)
				{
					break;
				}
			}
		}

		if (lPosition == 1)	//Bond
		{	
			nStatus = Z2_MoveTo((m_lBPGeneral_C -= lStepSize), SFM_WAIT, TRUE);
			if ((m_lBPGeneral_C <= lMinPos) || (nStatus != gnOK))
			{
				SetPickVacuumZ2(FALSE);
				//HmiMessage("Learn Failed", "Auto learn level Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
				return FALSE;
				
			}
		}
		else	//Pick
		{
			nStatus = Z2_MoveTo((m_lBPGeneral_B -= lStepSize));
			if ((m_lBPGeneral_B <= lMinPos) || (nStatus != gnOK))
			{
				SetPickVacuumZ2(FALSE);
				//HmiMessage("Learn Failed", "Auto learn level Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
				return FALSE;
			}
		}
	}

	//Off Collet vacuum 
	SetPickVacuumZ2(FALSE);

	if (m_bUseCTSensorLearn)
	{
		SetStatusMessage("Auto learn bondhead level by CT sensor completed");
	}
	else
	{
		SetStatusMessage("Auto learn bondhead level AF Sensor completed");
	}
#endif

	return TRUE;
}


BOOL CBondHead::AutoLearnBHZ2BondLevel(LONG &lEnc, LONG lOldEnc, LONG lMaxOffset)
{
	INT			nStatus = 0;
	LONG		lStepSize = 10;
	LONG		lMinPos = 0;
	CString		szLog;
	LONG lLevel = lEnc;		//v4.49A9

	if (m_bIsArm2Exist == FALSE)
	{
		return TRUE;
	}

#ifdef NU_MOTION
	//Check min limit value
	lMinPos = -9000;//GetChannelInformation(MS896A_CFG_CH_BONDHEAD, MS896A_CFG_CH_MIN_DISTANCE);								

	//Test missing die sensor reply
	SetPickVacuumZ2(TRUE);
	Sleep(1000);
	if (IsColletJamZ2() == TRUE)
	{
		OpDisplayAlarmPage(TRUE);	//v4.53A13
		SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_1);
		SetPickVacuumZ2(FALSE);
		return FALSE;
	}


	SetPickVacuumZ2(TRUE);

	//Start Learning Missing Die Sensor
	while (1)
	{
		Sleep(10);

		if (m_bUseCTSensorLearn)	//Use Contact Sensor	//v2.96T4
		{
			if (IsBHContactZ2())
			{
				Sleep(10);
				if (IsBHContactZ2())
				{
					break;
				}
			}
		}
		else						//Use air-flow sensor
		{
			if (IsColletJamZ2() == TRUE)
			{
				Sleep(10);
				if (IsColletJamZ2() == TRUE)
				{
					break;
				}
			}
		}

		if (!Z2_IsPowerOn())
		{
			SetPickVacuumZ2(FALSE);
			HmiMessage_Red_Yellow("BHZ2 is not power ON !!");	//v4.49A12
			Z2_Home();
			return FALSE;
		}

		lLevel -= lStepSize;
		nStatus = Z2_MoveTo(lLevel);

		if ((lLevel <= lMinPos) || (nStatus != gnOK))
		{
			SetPickVacuumZ2(FALSE);
			Z2_Home();
			SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_2);		//v4.49A12
			return FALSE;
		}

		if (lOldEnc != 0 && lMaxOffset != 0 && ((lLevel - lOldEnc) * -1 > lMaxOffset))
		{
			szLog.Format("Auto Learn BH2 Error,Old,%d,New,%d,offset,%d",lOldEnc,lLevel,lMaxOffset);
			SetErrorMessage(szLog);
			SetPickVacuumZ2(FALSE);
			Z2_MoveTo(m_lSwingLevel_Z2);
			//Z2_Home();
			SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_2);		//v4.49A12
			return FALSE;
		}
	}

	//Off Collet vacuum 
	SetPickVacuumZ2(FALSE);
	//GetEncoderValue();
	lEnc = lLevel;	//m_lEnc_Z2;

	if (!Z2_IsPowerOn())	//v4.49A12
	{
		Z2_Home();
	}
	else
	{
		Z2_MoveTo(m_lSwingLevel_Z2);
	}
#endif

	return TRUE;
}

BOOL CBondHead::AutoLearnEjUpLevelWithAitFlowSnr(LONG &lEnc, LONG lBHZOffsetZ)
{
	INT			nStatus = 0;
	LONG		lStepSize = 10;
	LONG		lMaxPos = 0;

	LONG lLevel = 0;

	//Check min limit value
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MAX_DISTANCE);								

	//Test missing die sensor reply
	SetEjectorVacuum(TRUE);
	SetPickVacuum(TRUE);
	Sleep(500);

	if (IsColletJam() == TRUE)
	{
		Sleep(500);

		if (IsColletJam() == TRUE)
		{
			OpDisplayAlarmPage(TRUE);	//v4.53A13
			SetAlert_Red_Yellow(IDS_BH_AUTO_LRN_FAILED_1);		//v4.50A1
			SetEjectorVacuum(FALSE);
			return FALSE;
		}
	}

	//Start Learning Missing Die Sensor
	while (1)
	{
		Sleep(10);

		if (IsColletJam() == TRUE)
		{
			Sleep(10);
			if (IsColletJam() == TRUE)
			{
				break;
			}
		}

		if (!Z_IsPowerOn())
		{
			SetPickVacuum(FALSE);
			SetEjectorVacuum(FALSE);
			HmiMessage("BHZ is power OFF !!");
			Z_Home();
			Ej_Home();
			return FALSE;
		}

		if (!Ej_IsPowerOn())
		{
			SetPickVacuum(FALSE);
			SetEjectorVacuum(FALSE);
			HmiMessage("Ejector is power OFF !!");
			Z_Home();
			Ej_Home();
			return FALSE;
		}

		lLevel += lStepSize;
		nStatus = Ej_MoveTo(lLevel);

		if ((lLevel >= lMaxPos) || (nStatus != gnOK))
		{
			SetEjectorVacuum(FALSE);
			Z_Home();
			Ej_Home();
			SetAlert(IDS_BH_AUTO_LRN_FAILED_2);
			return FALSE;
		}
	}

	//Off Collet vacuum 
	SetEjectorVacuum(FALSE);
	SetPickVacuum(FALSE);

	//GetEncoderValue();
	lEnc = lLevel - lBHZOffsetZ;
	
	CString szTemp;
	szTemp.Format("Auto-Leaern Ej Z Up level at %ld (old = %ld, stdby = %ld, Max = %ld)", 
		lEnc, m_lEjectLevel_Ej, m_lStandbyLevel_Ej, lMaxPos);
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);

	Z_Home();
	Ej_Home();

	return TRUE;
}


BOOL CBondHead::CheckWExpanderSafeToMove()
{
	BOOL bResult	= TRUE;
	BOOL bWLModule	= FALSE;
	IPC_CServiceMessage svMsg;

	bWLModule = (BOOL)(LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"];
	if (bWLModule == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWT)		//v4.16T1
	{
		return TRUE;
	}

	//v2.78T2
	// Get current T encoder posn before check expander
	typedef struct 
	{
		LONG lX;
		LONG lY;
		LONG lT;
	} WT_XYT_STRUCT;
	WT_XYT_STRUCT stXYT;
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stXYT);
	INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetEncoderCmd", svMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			svMsg.GetMsg(sizeof(WT_XYT_STRUCT), &stXYT);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	LONG lWtT = stXYT.lT;


	//Check expander UP/DN sensors
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "CheckExpanderSafeToMove", svMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			svMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}


	//v2.78T2
	//Move T back to original posn
	Sleep(1000);
	svMsg.InitMessage(sizeof(LONG), &lWtT);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T_MoveToCmd", svMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bResult;
}

BOOL CBondHead::SwitchPrAOICamera()				//v3.93
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	BOOL  bStatus = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bStatus);

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SwitchToTarget_dbh", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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

	return bStatus;
}

BOOL CBondHead::ResetEjectorSelection()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bReturn = TRUE;
	if (pApp ->GetCustomerName() != "Genesis")
	{
		return bReturn;
	}
	SetStatusMessage("Gensis Reset Ejector");
	if (HmiMessage("Reset Ejector Count?", "Ejector Count", 
				   glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		ResetEjectorCount();
	}

	return bReturn;
}

BOOL CBondHead::ResetColletCountSelection(int nCollet)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bReturn = TRUE;
	if (pApp ->GetCustomerName() != "Genesis")
	{
		return bReturn;
	}
	SetStatusMessage("Gensis Reset Collet");
	CString szText = " ";

	if (HmiMessage("Reset Collet Count?", "Collet Count", 
				   glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		if (nCollet == 1)
		{
			ResetColletCount();
		}
		else
		{
			ResetCollet2Count();
		}
	}

	return bReturn;
}

BOOL CBondHead::ResetColletCount()
{
	m_ulColletCount = 0;
	if (m_pulStorage != NULL)
	{
		*(m_pulStorage + BH_COLLETCOUNT_OFFSET) = 0;
	}
	
	m_ulZ1CountForSmartKOffset		= 0;
	m_lZ1SmartKOffsetTotal			= 0;
	m_lZ1SmartValue					= 0;

	SetStatusMessage("Change collet count is reset");
	SendCE_ToolLife();
	return TRUE;
}

BOOL CBondHead::ResetCollet2Count()
{
	m_ulCollet2Count = 0;
	if (m_pulStorage != NULL)
	{
		*(m_pulStorage + BH_COLLET2COUNT_OFFSET) = 0;
	}
	
	m_ulZ2CountForSmartKOffset		= 0;
	m_lZ2SmartKOffsetTotal			= 0;
	m_lZ2SmartValue					= 0;

	SetStatusMessage("Change collet 2 count is reset");
	SendCE_ToolLife();
	return TRUE;
}

BOOL CBondHead::ResetCleanColletCount()
{
//	m_dLastPickTime	= GetTime();	reset counter, no need as level uncertain
	m_ulCleanCount	= 0;
	if (m_pulStorage != NULL)
	{
		*(m_pulStorage + BH_CLEANCOUNT_OFFSET) = 0;
	}
	
	m_ulZ1CountForSmartKOffset		= 0;
	m_lZ1SmartKOffsetTotal			= 0;
	m_lZ1SmartValue					= 0;
	m_ulZ2CountForSmartKOffset		= 0;
	m_lZ2SmartKOffsetTotal			= 0;
	m_lZ2SmartValue					= 0;

	SetStatusMessage("Clean collet count is reset");
	return TRUE;
}

VOID CBondHead::ResetEjectorKCount()
{
	//v4.44A1	//Semitek
	m_lEjectorKOffsetTotal			= 0;
	m_lEjectorKOffsetBHZ1Total		= 0;	//v4.52A10
	m_lEjectorKOffsetBHZ2Total		= 0;	//v4.52A10
	m_ulEjectorCountForKOffset		= 0;
	//v4.45T4
	m_ulEjCountForSubRegionKOffset	= 0;
	m_lEjSubRegionKOffsetTotal		= 0;
	m_ulEjCountForSubRegionSKOffset	= 0;
	m_lEjSubRegionSKOffsetTotal		= 0;
	m_lEjSubRegionSmartValue		= 0;
}


BOOL CBondHead::ResetEjectorCount()
{
	m_ulEjectorCount = 0;
	m_bEjPinNeedReplacementAtUnloadWafer = FALSE;
	if (m_pulStorage != NULL)
	{
		*(m_pulStorage + BH_EJECTCOUNT_OFFSET) = 0;
	}

	ResetEjectorKCount();

	SetStatusMessage("Ejector count is reset");
	SendCE_ToolLife();
	return TRUE;
}


VOID CBondHead::SendCE_ToolLife(VOID)
{
	// 3491
	SetGemValue("AB_EjectorUseLmt", m_ulMaxEjectorCount);
	SetGemValue("AB_ColletUseLmt", m_ulMaxColletCount);
	SetGemValue("AB_ColletUseCnt", m_ulColletCount);
	SetGemValue("AB_EjectorUseCnt", m_ulEjectorCount);
	// 7701
	SendEvent(SG_CEID_AB_TOOLLIFE, FALSE);
}

VOID CBondHead::SendCE_BondCounter(VOID)
{
	// 3492
	SetGemValue("AB_CntPickDies", m_ulPickDieCount);
	SetGemValue("AB_CntBondDies", m_ulBondDieCount);
	SetGemValue("AB_CntDefectDies", m_ulDefectDieCount);
	SetGemValue("AB_CntBadCutDies", m_ulBadCutDieCount);
	SetGemValue("AB_CntMissingDies", m_ulMissingDieCount);
	SetGemValue("AB_CntRotateDies", m_ulRotateDieCount);
	SetGemValue("AB_CntEmptyDies", m_ulEmptyDieCount);
	// 7702
	SendEvent(SG_CEID_AB_COUNTER, FALSE);
}

VOID CBondHead::SendCE_CurrWFCounter(VOID)
{
	// 3493
	SetGemValue("AB_CntWFBondDies", m_ulCurrWaferBondDieCount);
	SetGemValue("AB_CntWFDefecDies", m_ulCurrWaferDefectDieCount);
	SetGemValue("AB_CntWFBadCutDies", m_ulCurrWaferBadCutDieCount);
	SetGemValue("AB_CntWFRotateDies", m_ulCurrWaferRotateDieCount);
	SetGemValue("AB_CntWFEmptyDies", m_ulCurrWaferEmptyDieCount);
	// 7703
	SendEvent(SG_CEID_AB_WFCOUNTER, FALSE);
}

VOID CBondHead::SendCE_EjKOffset(VOID)
{
	// 3476
	SetGemValue("AB_EJK_count", m_lEjectorKCount);
	SetGemValue("AB_EJK_offset", m_lEjectorKOffset);
	SetGemValue("AB_EJK_OffsetLimit", m_lEjectorKOffsetLimit);
	SetGemValue("AB_EJK_OffsetAccum", m_lEjectorKOffsetTotal);
	// 7603
	SendEvent(SG_CEID_EJ_OFFSET, FALSE);
}


BOOL CBondHead::BH_LOG_OPEN()
{
	if (m_bIsBHLogOpened)
	{
		return TRUE;
	}
	BOOL bStatus = m_oBHLogFile.Open("c:\\MapSorter\\UserData\\BH_Log.txt", 
									 //CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
									 CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText);
	if (bStatus)
	{
		m_oBHLogFile.SeekToEnd();
		m_bIsBHLogOpened = TRUE;
		return TRUE;
	}
	return FALSE;
}


BOOL CBondHead::BH_LOG(CONST CString szMsg)
{
	if (!m_bIsBHLogOpened)
	{
		return FALSE;
	}
	m_oBHLogFile.WriteString(szMsg + "\n");	//v3.94
	return TRUE;
}


VOID CBondHead::BH_LOG_CLOSE()
{
	if (m_bIsBHLogOpened)
	{
		m_oBHLogFile.Close();
		m_bIsBHLogOpened = FALSE;
	}
}

BOOL CBondHead::BH_LOG_MESSAGE(CONST CString szMsg)
{
	if (BH_LOG_OPEN())
	{
		BH_LOG(szMsg);
		BH_LOG_CLOSE();
		return TRUE;
	}
	return FALSE;
}


INT CBondHead::EnableBHZOpenDac(CONST BOOL bEnable)
{
	INT nStatus = 0;
	if (bEnable)
	{
		Hp_OpenDac_input input;
		input.dac_time		= -1;
		input.process_blkno = HP_NIL_PROCESS;
		input.dac_value		= (SHORT) m_lBHZOpenDacValue;

		try
		{
			if (GetAxisType(BH_AXIS_Z) == BH_DC_SVO_MOTOR)	
			{
				SFM_CHipecDcServo *pDcServo = GetHipecDcServo(GetActuatorName(BH_AXIS_Z));
				
				if (pDcServo != NULL)
				{
					nStatus = hp_open_dac(&pDcServo->m_HpCcb, &input);
				}
			}
			else if (GetAxisType(BH_AXIS_Z) == BH_SVO_MOTOR)	
			{
				SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_Z));
				
				if (pServo != NULL)
				{
					nStatus = hp_open_dac(&pServo->m_HpCcb, &input);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
		}
	}
	else
	{
		try
		{
			if (GetAxisType(BH_AXIS_Z) == BH_DC_SVO_MOTOR)	
			{
				SFM_CHipecDcServo *pDcServo = GetHipecDcServo(GetActuatorName(BH_AXIS_Z));
				
				if (pDcServo != NULL)
				{
					nStatus = hp_stop(&pDcServo->m_HpCcb, 0);
				}
			}
			else if (GetAxisType(BH_AXIS_Z) == BH_SVO_MOTOR)	
			{
				SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_Z));
				
				if (pServo != NULL)
				{
					nStatus = hp_stop(&pServo->m_HpCcb, 0);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
		}
	}
	return nStatus;
}


LONG CBondHead::AutoLearnDAC(BOOL bIsPickSide, LONG lCurrBHZ, BOOL bIsCalib)
{
	INT nDacValue = 0;

	if (m_fHardware == FALSE)
	{
		return nDacValue;
	}

#ifdef NU_MOTION
	CString szPort;
	GMP_S16 sResult = 0;
	GMP_U32 uiResult = 0;
	CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];
	if (lCurrBHZ == BH_MS100_BHZ1)
	{
		szPort = m_stBHAxis_Z.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
	}
	else if (lCurrBHZ == BH_MS100_BHZ2)
	{
		szPort = m_stBHAxis_Z2.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
	}

	strcpy_s(pcPortID, sizeof(pcPortID), szPort);
	
	Sleep(300);
	try
	{
		if ((sResult = gmp_read_io_port(pcPortID, &uiResult)) != GMP_SUCCESS)
		{
			CAsmException e((UINT)sResult, "gmp_read_DAC_port", szPort);
			throw e;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	if (sResult == GMP_SUCCESS)
	{
		nDacValue = uiResult;
		nDacValue = nDacValue / NU_MOTION_DAC_RATIO;

		if (bIsCalib == FALSE)
		{
			if (lCurrBHZ == BH_MS100_BHZ1)
			{
				if (bIsPickSide == TRUE)
				{
					m_lBHZ1_PickDAC = nDacValue;

					CString szMsg;
					szMsg.Format("BHZ1 DAC value = %d", m_lBHZ1_PickDAC);
					AfxMessageBox(szMsg, MB_SYSTEMMODAL);

				}
				else
				{
					m_lBHZ1_BondDAC = nDacValue;
				}
			}
			else if (lCurrBHZ == BH_MS100_BHZ2)
			{
				if (bIsPickSide == TRUE)
				{
					m_lBHZ2_PickDAC = nDacValue;
				}
				else
				{
					m_lBHZ2_BondDAC = nDacValue;
				}
			}
		}
	}
	else
	{
		return 0;

	}

	//SaveBhData();
#endif
	return nDacValue;
}

LONG CBondHead::OpenDACCalibration(LONG lCurrBHZ)
{
	LONG lPrevOpenDACCalibValue = m_lCurrOpenDACCalibValue;
	m_lCurrOpenDACCalibValue = AutoLearnDAC(TRUE, lCurrBHZ, TRUE);

	if (m_lCurrOpenDACCalibValue > lPrevOpenDACCalibValue)	//The learnt value must be larger than the start value
	{
		m_lCurrOpenDACCalibValue = lPrevOpenDACCalibValue;
	}

	return TRUE;
}

LONG CBondHead::OpenDACForceCheck(LONG lCurrBHZ)
{
	m_lCurrOpenDACCalibValue = AutoLearnDAC(TRUE, lCurrBHZ, TRUE);
	m_lCurrOpenDACCalibValue = m_lCurrOpenDACCalibValue - m_lStartOpenDACCalibValue;

	if (lCurrBHZ == BH_MS100_BHZ1)
	{
		if (m_dBHZ1_OpenDACCalibRatio < 0.0)
		{
			m_lOpenDACForceCheckValue = (LONG)(m_lCurrOpenDACCalibValue / m_dBHZ1_OpenDACCalibRatio);
		}
	}
	else if (lCurrBHZ == BH_MS100_BHZ2)
	{
		if (m_dBHZ2_OpenDACCalibRatio < 0.0)
		{
			m_lOpenDACForceCheckValue = (LONG)(m_lCurrOpenDACCalibValue / m_dBHZ2_OpenDACCalibRatio);
		}
	}

	return TRUE;
}

LONG CBondHead::ConvertFromForceToDAC(LONG lForce, LONG lCurrBHZ)
{
	LONG lConvertedDAC = 0;

	if (lCurrBHZ == BH_MS100_BHZ1)
	{
		lConvertedDAC = (LONG)(lForce * m_dBHZ1_OpenDACCalibRatio);
		
		CString szMsg;
		szMsg.Format("BHZ1 (Force, DAC): %d, %d", lForce, lConvertedDAC);
		SetStatusMessage(szMsg);
	}
	else if (lCurrBHZ == BH_MS100_BHZ2)
	{
		lConvertedDAC = (LONG)(lForce * m_dBHZ2_OpenDACCalibRatio);
		
		CString szMsg;
		szMsg.Format("BHZ2 (Force, DAC): %d, %d", lForce, lConvertedDAC);
		SetStatusMessage(szMsg);
	}

	return lConvertedDAC;
}

LONG CBondHead::ConvertFromDACToDistance(LONG lDAC, LONG lCurrBHZ)
{
	LONG lConvertedDist = 0;

	if (lCurrBHZ == BH_MS100_BHZ1)
	{
		lConvertedDist = (LONG)(lDAC * m_dBHZ1_DACDistanceCalibRatio);
		
		CString szMsg;
		szMsg.Format("BHZ1 (DAC, Dist): %d, %d", lDAC, lConvertedDist);
		SetStatusMessage(szMsg);
	}
	else if (lCurrBHZ == BH_MS100_BHZ2)
	{
		lConvertedDist = (LONG)(lDAC * m_dBHZ2_DACDistanceCalibRatio);
		
		CString szMsg;
		szMsg.Format("BHZ2 (DAC, Dist): %d, %d", lDAC, lConvertedDist);
		SetStatusMessage(szMsg);
	}

	return lConvertedDist;
}

/*
LONG CBondHead::SetupDataLog(VOID)
{
	Hp_mcb	*pModule = m_pServo_T->m_HpCcb.hp_servo_ch.p;

	short	siPortList[1];
	short	ssChannel1 = (short)m_pServo_T->GetChannelId()-1;
	//short	ssChannel2 = (short)m_pServo_Z->GetChannelId()-1;
	short	ssReturn = 0;

	byte	ucByteNo;
	

	siPortList[0] = HP_MIN_MOTION_PORT + ssChannel1;
	//siPortList[1] = HP_MIN_MOTION_PORT + ssChannel2;

	ssReturn = hp_set_datalog_trigger_channel(pModule, ssChannel1+1);
	//ssReturn = hp_set_datalog_trigger_channel(pModule, ssChannel1+2);

	ssReturn = hp_set_datalog_static_sample(pModule, 5000);
	ssReturn = hp_set_datalog_entries(pModule, 1, siPortList, &ucByteNo);
	ssReturn = hp_set_datalog_mode(pModule, 1);

	return (LONG)ssReturn;
}



LONG CBondHead::EnableDataLog(BOOL bLog)
{
	Hp_mcb	*pModule = m_pServo_T->m_HpCcb.hp_servo_ch.p;
	short	ssReturn = 0;


	if (bLog == TRUE)
	{
		ssReturn = hp_datalog(pModule, HP_ENABLE);
	}
	else
	{
		int	siData[5000];
		FILE	*fp;
		int	i,j;

		int siStartCount = -1;
		int siStopCount = -1;

		memset(siData,0, sizeof(siData));

		ssReturn = hp_datalog(pModule, HP_DISABLE);
		ssReturn = hp_upload_datalog(pModule, siData, 5000);

		if ( (fp = fopen("DataLog.txt", "w")) != NULL)
		{
			j = 0;	
			fprintf(fp,"Theta\n");
			
			for (i=0; i<5000 ;i++)
			{
				if (siStartCount == -1)
				{
					if (siData[i] == 1)
					{
						siStartCount = i;
					}
				}
				else
				{
					if (siData[i] == 65535)
					{
						siStopCount = i;
						if ((siStopCount - siStartCount) > 0)
						{
							fprintf(fp,"%d\n", (siStopCount - siStartCount));
						}
						siStartCount = -1;
					}
				}
			}

			fclose(fp);
		}
	}


	return (LONG)ssReturn;
}
*/

LONG CBondHead::SetupDataLogEjector(VOID)
{
	Hp_mcb	*pModule = NULL;
	short	ssChannel1 = 0;
	short	ssReturn = 0;
	byte	ucByteNo;

	//pModule		= m_pServo_Ej->m_HpCcb.hp_servo_ch.p;
	//ssChannel1	= (short) m_pServo_Ej->GetChannelId()-1;
	try
	{
		if (GetAxisType(BH_AXIS_EJ) == BH_SVO_MOTOR)	
		{
			SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_EJ));
			
			if (pServo != NULL)
			{
				pModule		= pServo->m_HpCcb.hp_servo_ch.p;
				ssChannel1	= (short) pServo->GetChannelId() - 1;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	short siPortList[4];
	siPortList[0] = HP_CH_ENCPOS_PORT_0 + ssChannel1;
	siPortList[1] = HP_CH_CMDPOS_PORT_0 + ssChannel1;
	siPortList[2] = HP_CH_MODE_PORT_0 + ssChannel1;
	siPortList[3] = HP_CH_MTRDAC_PORT_0 + ssChannel1;

	ssReturn = hp_set_datalog_trigger_channel(pModule, ssChannel1 + 1);

	ssReturn = hp_set_datalog_static_sample(pModule, 100);
	ssReturn = hp_set_datalog_entries(pModule, 4, siPortList, &ucByteNo);
	ssReturn = hp_set_datalog_mode(pModule, 1);
	//AfxMessageBox("SetupDataLogEjector", MB_SYSTEMMODAL);

	return (LONG)ssReturn;
}

LONG CBondHead::EnableDataLogEjector(BOOL bLog)
{
	Hp_mcb	*pModule = NULL;
	short	ssReturn = 0;
	
	//pModule = m_pServo_Ej->m_HpCcb.hp_servo_ch.p;
	try
	{
		if (GetAxisType(BH_AXIS_EJ) == BH_SVO_MOTOR)	
		{
			SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_EJ));
			
			if (pServo != NULL)
			{
				pModule		= pServo->m_HpCcb.hp_servo_ch.p;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	if (bLog == TRUE)
	{
		//AfxMessageBox("EnableDataLogEjector", MB_SYSTEMMODAL);
		ssReturn = hp_datalog(pModule, HP_ENABLE);
	}
	else
	{
		int	siData[0x8000];
		memset(siData, 0, sizeof(siData));

		int	i;
		unsigned short	usNumOfSamples = 0;
		unsigned char	ucStatus = 0;

		ssReturn = hp_datalog(pModule, HP_DISABLE);
		hp_report_datalog_status(pModule, &usNumOfSamples, &ucStatus);
		ssReturn = hp_upload_datalog(pModule, siData, usNumOfSamples);
		
		CStdioFile oFile;
		CString szData;

		if (oFile.Open(_T("c:\\MapSorter\\UserData\\DataLogEjector.txt"), 
					   CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText))
		{
			CString szSampleSize;
			szSampleSize.Format("%d", usNumOfSamples);
			oFile.WriteString("Sample Size:" + szSampleSize + "\n");
			for (i = 0; i < usNumOfSamples; i++)
			{
				i = min(i, 8191);	//Klocwork		//v4.02T5

				szData.Format("%d,%d,%d,%d\n", siData[i * 4], siData[i * 4 + 1], siData[i * 4 + 2], siData[i * 4 + 3]);
				//szData.Format("%d,%d\n", siData[i*2], siData[i*2+1]);
				oFile.WriteString(szData);
			}

			oFile.Close();
		}
	}

	return (LONG)ssReturn;
}

LONG CBondHead::SetupMotionDataLog(Hp_mcb *pModule, short ssChannel)
{
	//Hp_mcb	*pModule = pAcServo->m_HpCcb.hp_servo_ch.p;

	short	siPortList[2];
	//short	ssChannel1 = (short)pAcServo->GetChannelId()-1;
	short	ssReturn = 0;

	byte	ucByteNo;
	
	siPortList[0] = HP_CH_ENCPOS_PORT_0 + ssChannel;
	siPortList[1] = HP_CH_CMDPOS_PORT_0 + ssChannel;
	//siPortList[2] = HP_CH_MTRDAC_PORT_0 + ssChannel;
	//siPortList[3] = HP_CH_MODE_PORT_0   + ssChannel;

	ssReturn = hp_set_datalog_trigger_channel(pModule, ssChannel - 1);
	//ssReturn = hp_set_datalog_sampling_rate(pModule, 1);
	ssReturn = hp_set_datalog_static_sample(pModule, 10);
	ssReturn = hp_set_datalog_entries(pModule, 2, siPortList, &ucByteNo);
	ssReturn = hp_set_datalog_mode(pModule, 1);

	return (LONG)ssReturn;
}

LONG CBondHead::EnableMotionDataLog(BOOL bEnable, Hp_mcb *pModule, short ssChannel)
{
	//Hp_mcb	*pModule = NULL;
	short	ssReturn = 0;
	
	//pModule = pAcServo->m_HpCcb.hp_servo_ch.p

	if (bEnable == TRUE)
	{
		ssReturn = hp_datalog(pModule, HP_ENABLE);
	}
	else
	{
		int	siData[0x8000];
		memset(siData, 0, sizeof(siData));

		int	i;
		unsigned short	usNumOfSamples = 0;
		unsigned char	ucStatus = 0;

		ssReturn = hp_datalog(pModule, HP_DISABLE);
		hp_report_datalog_status(pModule, &usNumOfSamples, &ucStatus);
		ssReturn = hp_upload_datalog(pModule, siData, usNumOfSamples);

		CStdioFile oFile;
		CString szData;

		if (oFile.Open(_T("c:\\MapSorter\\UserData\\MotionDataLog.txt"), 
					   CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText))
		{
			for (i = 0; i < usNumOfSamples; i++)
			{
				i = min(i, 16383);		//Klocwork	//v4.02T5

				szData.Format("%d,%d\n", siData[i * 2], siData[i * 2 + 1]);
				//szData.Format("%d,%d\n", siData[i*2], siData[i*2+1]);
				oFile.WriteString(szData);
			}

			oFile.Close();
		}
	}

	return (LONG)ssReturn;
}


LONG CBondHead::SetupDataLogT(VOID)
{
	//Hp_mcb	*pModule = m_pServo_T->m_HpCcb.hp_servo_ch.p;
	short	siPortList[1];
	//short	ssChannel1 = (short)m_pServo_T->GetChannelId()-1;
	short	ssReturn = 0;

	byte	ucByteNo;
	
	Hp_mcb	*pModule	= NULL;
	short	ssChannel1	= 0;
	try
	{
		if (GetAxisType(BH_AXIS_T) == BH_SVO_MOTOR)	
		{
			SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_T));
			
			if (pServo != NULL)
			{
				pModule = pServo->m_HpCcb.hp_servo_ch.p;
				ssChannel1 = (short)pServo->GetChannelId() - 1;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	switch (m_unDataLogOption)
	{
		case 1:
			siPortList[0] = HP_CH_ENCPOS_PORT_0 + ssChannel1;
			break;
		case 2:
			siPortList[0] = HP_CH_CMDPOS_PORT_0 + ssChannel1;
			break;
		case 0:
		default:
			siPortList[0] = HP_CH_MTRDAC_PORT_0 + ssChannel1;
			break;
	}

	ssReturn = hp_set_datalog_trigger_channel(pModule, ssChannel1 + 1);
	ssReturn = hp_set_datalog_static_sample(pModule, 100);
	ssReturn = hp_set_datalog_entries(pModule, 1, siPortList, &ucByteNo);
	ssReturn = hp_set_datalog_mode(pModule, 1);

	return (LONG)ssReturn;
}


LONG CBondHead::EnableDataLogT(BOOL bLog)
{
	//Hp_mcb	*pModule = m_pServo_T->m_HpCcb.hp_servo_ch.p;
	short	ssReturn = 0;
	DOUBLE	dSumOfSqOfDacTemp = 0;
	CString szFileHeader;

	Hp_mcb	*pModule	= NULL;
	try
	{
		if (GetAxisType(BH_AXIS_T) == BH_SVO_MOTOR)	
		{
			SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_T));
			
			if (pServo != NULL)
			{
				pModule = pServo->m_HpCcb.hp_servo_ch.p;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	if (bLog == TRUE)
	{
		ssReturn = hp_datalog(pModule, HP_ENABLE);
	}
	else
	{
		int	siData[0x8000];
		memset(siData, 0, sizeof(siData));
		int nData[0x8000];
		memset(nData , 0 , sizeof(nData));


		ssReturn = hp_datalog(pModule, HP_DISABLE);

		unsigned short	usNumOfSamples = 0;
		unsigned char	usStatus = 0;
		unsigned short	usNumOfSamplesUploaded = 0;

		hp_report_datalog_status(pModule, &usNumOfSamples, &usStatus);
		ssReturn = hp_upload_datalog_tail(pModule, siData, usNumOfSamples, &usNumOfSamples);


		// Output to data file
		CStdioFile fLogFile;
		CStdioFile fLogFileForGraph;
		CString szData;
		int nMax = 0, nMin = 0;
		BOOL bOpenFile = FALSE;

		CTime ctCurTime(CTime::GetCurrentTime());

		if (m_bBhTDacForwardMove)
		{
			bOpenFile = (fLogFile.Open(_T("c:\\mapsorter\\UserData\\Diagnostics\\BH\\DacFromPickToBond.csv"), CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeText)) &&
						(fLogFileForGraph.Open(_T("c:\\mapsorter\\UserData\\Diagnostics\\BH\\DacFromPickToBond_Graph.csv"), CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeText));
			szFileHeader = "From Pick To Bond";
		}
		else
		{
			bOpenFile = (fLogFile.Open(_T("c:\\mapsorter\\UserData\\Diagnostics\\BH\\DacFromBondToPick.csv"), CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeText)) &&
						(fLogFileForGraph.Open(_T("c:\\mapsorter\\UserData\\Diagnostics\\BH\\DacFromBondToPick_Graph.csv"), CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeText));
			szFileHeader = "From Bond To Pick";
		}

		if (bOpenFile)
		{
			fLogFileForGraph.WriteString("Log File For Bond Arm DAC Test( " + szFileHeader + " )\n");
			fLogFileForGraph.WriteString(ctCurTime.Format("File Created:%d/%m/%Y %H:%M:%S\n"));
			CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

			if (pAppMod != NULL)
			{
				CString strSwVerNo = pAppMod->m_szAppSWVersion;
				fLogFileForGraph.WriteString("Software Version: " + strSwVerNo + "\n");
			}

			szFileHeader.Format("Profile Time: %d \n", m_lTime_T);
			fLogFileForGraph.WriteString(szFileHeader);


			//v3.15T5
			if (m_unDataLogOption == 0)
			{
				for (int i = 0; i < usNumOfSamples ; i++)
				{
					i = min(i, 32767);		//Klocwork	//v4.02T5

					nData[i] = siData[i];

					if (nData[i] > 32768)
					{
						nData[i] = nData[i] - 65536;
					}
					
					if (i == 0)
					{
						nMax = nData[i];
						nMin = nData[i];
					}
					else
					{
						if (nData[i] > nMax)
						{
							nMax = nData[i];
						}
						if (nData[i] < nMin)
						{
							nMin = nData[i];
						}
					}

					dSumOfSqOfDacTemp = nData[i] * nData[i] + dSumOfSqOfDacTemp;
				}

				szData.Format("Sum Of Sq Of DAC:,%.1f\n\n", dSumOfSqOfDacTemp);
				fLogFileForGraph.WriteString(szData);

				for (int i = 0; i < usNumOfSamples ; i++)
				{
					szData.Format("%d,%d,", i + 1, nData[i]);
					fLogFile.WriteString(szData);
					szData.Format("%d,%d\n", i + 1, nData[i]);
					fLogFileForGraph.WriteString(szData);
				}
			}
			else
			{
				for (int i = 0; i < usNumOfSamples ; i++)
				{
					nData[i] = siData[i];
					szData.Format("%d,%d,", i + 1, nData[i]);
					fLogFile.WriteString(szData);
					szData.Format("%d,%d\n", i + 1, nData[i]);
					fLogFileForGraph.WriteString(szData);
				}
			}

			//Display SQ-of DAC on screen
			if (m_unDataLogOption == 0)		//DAC
			{
				if (m_bBhTDacForwardMove)
				{
					m_dSqOfDacFromPickToBond = dSumOfSqOfDacTemp;
					m_szSqOfDacFromPickToBond.Format("%.1f" , m_dSqOfDacFromPickToBond) ;
				}
				else
				{
					m_dSqOfDacFromBondToPick = dSumOfSqOfDacTemp;
					m_szSqOfDacFromBondToPick.Format("%.1f" , m_dSqOfDacFromBondToPick) ;
				}
				m_szSumOfDac.Format("%.1f", (m_dSqOfDacFromPickToBond + m_dSqOfDacFromBondToPick));
			}
			else
			{
				m_szSqOfDacFromPickToBond	= "";
				m_szSqOfDacFromBondToPick	= "";
				m_szSumOfDac				= "";
			}

			fLogFile.Close();
			fLogFileForGraph.Close();
		}
		
	}

	return (LONG)ssReturn;
}


LONG CBondHead::SetupDataLogZ(VOID)
{
	Hp_mcb	*pModule = NULL;
	short	ssChannel1 = 0;
	short	ssReturn = 0;
	byte	ucByteNo;

	try
	{
		if (GetAxisType(BH_AXIS_Z) == BH_DC_SVO_MOTOR)	
		{
			SFM_CHipecDcServo *pDcServo = GetHipecDcServo(GetActuatorName(BH_AXIS_Z));
			
			if (pDcServo != NULL)
			{
				pModule		= pDcServo->m_HpCcb.hp_servo_ch.p;
				ssChannel1	= (short) pDcServo->GetChannelId() - 1;
			}
		}
		else if (GetAxisType(BH_AXIS_Z) == BH_SVO_MOTOR)	
		{
			SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_Z));
			
			if (pServo != NULL)
			{
				pModule		= pServo->m_HpCcb.hp_servo_ch.p;
				ssChannel1	= (short) pServo->GetChannelId() - 1;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	short ssChannel2 = 0;
	try
	{
		if (GetAxisType(BH_AXIS_EJ) == BH_SVO_MOTOR)	
		{
			SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_EJ));
			
			if (pServo != NULL)
			{
				ssChannel2 = (short)pServo->GetChannelId() - 1;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	short siPortList[8];
	siPortList[0] = HP_CH_ENCPOS_PORT_0 + ssChannel1;
	siPortList[1] = HP_CH_CMDPOS_PORT_0 + ssChannel1;	//v3.68T4
	siPortList[2] = HP_CH_ENCPOS_PORT_0 + ssChannel2;
	siPortList[3] = HP_CH_CMDPOS_PORT_0 + ssChannel2;	//v3.68T4
	siPortList[4] = HP_CH_MODE_PORT_0 + ssChannel1;
	siPortList[5] = HP_CH_MODE_PORT_0 + ssChannel2;
	siPortList[6] = HP_CH_MTRDAC_PORT_0 + ssChannel1;
	siPortList[7] = HP_CH_MTRDAC_PORT_0 + ssChannel2;

	//ssReturn = hp_set_datalog_trigger_channel(pModule, ssChannel1+1);
	ssReturn = hp_set_datalog_trigger_channel(pModule, ssChannel2 + 1);

	ssReturn = hp_set_datalog_static_sample(pModule, 100);
	ssReturn = hp_set_datalog_entries(pModule, 8, siPortList, &ucByteNo);
	ssReturn = hp_set_datalog_mode(pModule, 1);

	return (LONG)ssReturn;
}


LONG CBondHead::EnableDataLogZ(BOOL bLog)
{
	Hp_mcb	*pModule = NULL;
	short	ssReturn = 0;
	
	try
	{
		if (GetAxisType(BH_AXIS_Z) == BH_DC_SVO_MOTOR)	
		{
			SFM_CHipecDcServo *pDcServo = GetHipecDcServo(GetActuatorName(BH_AXIS_Z));
			
			if (pDcServo != NULL)
			{
				pModule		= pDcServo->m_HpCcb.hp_servo_ch.p;
			}
		}
		else if (GetAxisType(BH_AXIS_Z) == BH_SVO_MOTOR)	
		{
			SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_Z));
			
			if (pServo != NULL)
			{
				pModule		= pServo->m_HpCcb.hp_servo_ch.p;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	if (bLog == TRUE)
	{
		ssReturn = hp_datalog(pModule, HP_ENABLE);
	}
	else
	{
		int	siData[0x8000];
		memset(siData, 0, sizeof(siData));

		int	i;
		unsigned short	usNumOfSamples = 0;
		unsigned char	ucStatus = 0;

		ssReturn = hp_datalog(pModule, HP_DISABLE);
		hp_report_datalog_status(pModule, &usNumOfSamples, &ucStatus);
		ssReturn = hp_upload_datalog(pModule, siData, usNumOfSamples);

		CStdioFile oFile;
		CString szData;

		if (oFile.Open(_T("c:\\MapSorter\\UserData\\DataLogZ.txt"), 
					   CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText))
		{
			oFile.WriteString("ZENC,ZCMD,EjENC,EjCMD,ZMODE,EjMODE,ZDAC,EjDAC\n\n");

			for (i = 0; i < usNumOfSamples; i++)
			{
				i = min(i, 4095);		//Klocwork	//v4.02T5

				szData.Format("%d,%d,%d,%d,%d,%d,%d,%d\n", siData[i * 8], siData[i * 8 + 1], siData[i * 8 + 2], siData[i * 8 + 3], 
							  siData[i * 8 + 4], siData[i * 8 + 5], siData[i * 8 + 6], siData[i * 8 + 7]);
				//szData.Format("%d,%d\n", siData[i*2], siData[i*2+1]);
				oFile.WriteString(szData);
			}
			oFile.Close();
		}
	}

	return (LONG)ssReturn;
}

LONG CBondHead::LogItems(LONG lEventNo)
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

LONG CBondHead::GetLogItemsString(LONG lEventNo, CString &szMsg)
{
	switch (lEventNo)
	{
	
		case MISSING_DIE: //1
			if (m_bCheckMissingDie)
			{
				szMsg = "Missing Die Check,On";
			}
			else
			{
				szMsg = "Missing Die Check,Off";
			}
			break;
	
		case COLLECT_JAM: //2
			if (m_bCheckColletJam)
			{
				szMsg = "Collet Jam Check,On";
			}
			else
			{
				szMsg = "Collet Jam Check,Off";
			}
			break;

		case PICK_POSITION: //3
			szMsg.Format("Pick Position,%d", m_lPickPos_T);
			break;

		case BOND_POSITION: //4
			szMsg.Format("Bond Position,%d", m_lBondPos_T);
			break;

		case PICK_LEVEL_Z1: //5
			szMsg.Format("Pick Level Z1,%d", m_lPickLevel_Z);
			break;

		case PICK_LEVEL_Z2: //6
			szMsg.Format("Pick Level Z2,%d", m_lPickLevel_Z2);
			break;

		case BOND_LEVEL_Z1: //7
			szMsg.Format("Bond Level Z1,%d", m_lBondLevel_Z);
			break;

		case BOND_LEVEL_Z2: //8
			szMsg.Format("Bond Level Z2,%d", m_lBondLevel_Z2);
			break;

		case EJECTOR_UP_LEVEL: //9
			szMsg.Format("Ejector Up Level,%d", m_lEjectLevel_Ej);
			break;

		case PICK_DELAY: //10
			szMsg.Format("Pick Delay,%d", m_lPickDelay);
			break;
	
		case BOND_DELAY: //11
			szMsg.Format("Bond Delay,%d", m_lBondDelay);
			break;

		case ARM_PICK_DELAY: //12
			szMsg.Format("Arm Pick Delay,%d", m_lArmPickDelay);
			break;

		case ARM_BOND_DELAY: //13
			szMsg.Format("Arm Bond Delay,%d", m_lArmBondDelay);
			break;
	
		case HEAD_PICK_DELAY: //14
			szMsg.Format("Head Pick Delay,%d", m_lHeadPickDelay);
			break;

		case HEAD_BOND_DELAY: //15
			szMsg.Format("Head Bond Delay,%d", m_lHeadBondDelay);
			break;

		case HEAD_PREPICK_DELAY: //16
			szMsg.Format("Head PrePick Delay,%d", m_lHeadPrePickDelay);
			break;

		case SYNC_PICK_DELAY: //17
			szMsg.Format("Sync Pick Delay,%d", m_lSyncPickDelay);
			break;

		case PR_DELAY: //18
			szMsg.Format("PR Delay,%d", m_lPRDelay);
			break;

		case WT_THETA_DELAY: //19
			szMsg.Format("WaferTable Theta Delay,%d", m_lWTTDelay);
			break;

		case BT_DELAY: //20
			szMsg.Format("BinTable Delay,%d", m_lBinTableDelay);
			break;
	
		case EJ_UP_DELAY: //21
			szMsg.Format("Ejector Up Delay,%d", m_lEjectorUpDelay);
			break;

		case EJ_DOWN_DELAY: //22
			szMsg.Format("Ejector Down Delay,%d", m_lEjectorDownDelay);
			break;

		case COLLECT_JAM_RETRY_COUNT:
			szMsg.Format("Collect Jam Retry Count,%d", m_ulColletJam_Retry);
			break;

		case SYNC_MOVE:
			if (m_bEnableSyncMotionViaEjCmd)
			{
				szMsg = "Bond Head Sync Move,On";
			}
			else
			{
				szMsg = "Bond Head Sync Move,Off";
			}
			break;

		case SYNC_MOVE_TRIGGER_LEVEL:
			szMsg.Format(" BondHead Sync Move Trigger Level,%d", m_lSyncTriggerValue);
			break;

		case SYNC_MOVE_PRELOAD:
			szMsg.Format(" BondHead Sync Move Preload,%d", m_lSyncZOffset);
			break;

		case EJECTOR_STANDBY_LEVEL:
			//szMsg.Format("Ejector Standby Level,%d", m_lStandbyLevel_Ej);
			szMsg.Format("Ejector Standby Level,%d", m_lBPGeneral_A); //1.08S
			
			break;

		case CLEAN_COLLET_LEVEL:
			szMsg.Format("Clean Collet Level,%d", m_lAutoCleanCollet_Z);
			break;

		case AUTO_CLEAN_COLLET:
			if (m_bAutoCleanCollet)
			{
				szMsg = "Auto Clean Collet,On";
			}
			else 
			{
				szMsg = "Auto Clean Collet,Off";
			}
			break;

		case CHECK_MACHINE_COVER:
			if (m_bCheckCoverSensor)
			{
				szMsg = "Check Machine Cover,On";
			}
			else 
			{
				szMsg = "Check Machine Cover,Off";
			}
			break;

		case CLEAN_COLLET_LIMIT: //31
			szMsg.Format("Clean Collet Limit,%d", m_ulMaxCleanCount);
			break;

		case CHANGE_COLLET_LIMIT_Z1: //32
			szMsg.Format("Change Collet Z1 Limit,%d", m_ulMaxColletCount);
			break;

		case CHANGE_COLLET_LIMIT_Z2: //33
			szMsg.Format("Change Collet Z2 Limit,%d", m_ulMaxCollet2Count);
			break;

		case CHANGE_EJECTOR_LIMIT: //34
			szMsg.Format("Change Ejector Limit,%d", m_ulMaxEjectorCount);
			break;
	}

	return 1;
}

BOOL CBondHead::NVC_MotionTest(VOID)
{
	//m_ucMotionAxis	//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
	//m_bMoveDirection		= FALSE;
	//m_lMotionTestDelay	= 0;
	//m_lMotionTestDist		= 0;
	//m_lMotionOrigEncPosn	= 0;
	//m_lMotionTarEncPosn	= 0;
	BOOL bAbortTest = FALSE;

	if (m_ucMotionAxis == 0)
	{
		return TRUE;
	}

	if (m_bMoveDirection)
	{
		CString szLog;
		szLog.Format("Motion Test #%d +ive MOVE = %ld", m_ucMotionAxis, m_lMotionTarEncPosn);
		//HmiMessage(szLog);

		switch (m_ucMotionAxis)	//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
		{
		case 1:		//Ej
			Ej_MoveTo(m_lMotionTarEncPosn);
			break;
		case 2:		//EjT
			EjT_MoveTo(m_lMotionTarEncPosn);
			break;
		case 3:		//EjCap
			EjCap_MoveTo(m_lMotionTarEncPosn);
			break;
		case 4:		//EjElev
			EjElevator_MoveTo(m_lMotionTarEncPosn);
			break;
		case 5:		//BEj
			BinEj_MoveTo(m_lMotionTarEncPosn);
			break;
		case 6:		//BEjT
			BinEjT_MoveTo(m_lMotionTarEncPosn);
			break;
		case 7:		//BEjCap
			BinEjCap_MoveTo(m_lMotionTarEncPosn);
			break;
		case 8:		//BEjElev
			BinEjElevator_MoveTo(m_lMotionTarEncPosn);
			break;
		}

		m_bMoveDirection = FALSE;
	}
	else
	{
		CString szLog;
		szLog.Format("Motion Test #%d -ive MOVE = %ld", m_ucMotionAxis, m_lMotionOrigEncPosn);
		//HmiMessage(szLog);

		switch (m_ucMotionAxis)	//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
		{
		case 1:		//Ej
			Ej_MoveTo(m_lMotionOrigEncPosn);
			break;
		case 2:		//EjT
			EjT_MoveTo(m_lMotionOrigEncPosn);
			break;
		case 3:		//EjCap
			EjCap_MoveTo(m_lMotionOrigEncPosn);
			break;
		case 4:		//EjElev
			EjElevator_MoveTo(m_lMotionOrigEncPosn);
			break;
		case 5:		//BEj
			BinEj_MoveTo(m_lMotionOrigEncPosn);
			break;
		case 6:		//BEjT
			BinEjT_MoveTo(m_lMotionOrigEncPosn);
			break;
		case 7:		//BEjCap
			BinEjCap_MoveTo(m_lMotionOrigEncPosn);
			break;
		case 8:		//BEjElev
			BinEjElevator_MoveTo(m_lMotionOrigEncPosn);
			break;
		}

		m_bMoveDirection = TRUE;

		m_lMotionCycleCount++;
		if ((m_lMotionCycleCount >= m_lMotionCycle) || (m_lMotionCycleCount >= 100))
		{
			m_ucMotionAxis = 0;
			m_lMotionCycleCount = 0;
			bAbortTest = TRUE;
		}

		if (m_bMotionCycleStop)
		{
			m_ucMotionAxis = 0;
			m_lMotionCycleCount = 0;
			m_bMotionCycleStop = FALSE;
			bAbortTest = TRUE;
		}

		if (bAbortTest)
		{
			switch (m_ucMotionAxis)	//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
			{
			case 1:		//Ej
				LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, FALSE, 5, m_stDataLog);
				break;
			case 2:		//EjT
				LogAxisPerformance(BH_AXIS_EJ_T, &m_stBHAxis_EjT, FALSE, 5, m_stDataLog);
				break;
			case 3:		//EjCap
				LogAxisPerformance(BH_AXIS_EJCAP, &m_stBHAxis_EjCap, FALSE, 5, m_stDataLog);
				break;
			case 4:		//EjElev
				LogAxisPerformance(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator, FALSE, 5, m_stDataLog);
				break;
			case 5:		//BEj
				LogAxisPerformance(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj, FALSE, 5, m_stDataLog);
				break;
			case 6:		//BEjT
				LogAxisPerformance(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT, FALSE, 5, m_stDataLog);
				break;
			case 7:		//BEjCap
				LogAxisPerformance(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap, FALSE, 5, m_stDataLog);
				break;
			case 8:		//BEjElev
				LogAxisPerformance(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator, FALSE, 5, m_stDataLog);
				break;
			}
		}

	}

	if ((m_lMotionTestDelay >= 100) && (m_lMotionTestDelay <= 5000))
	{
		Sleep(m_lMotionTestDelay);
	}
	else
	{
		Sleep(1000);
	}

	return TRUE;

	/*
	if (m_bIsMotionTestBondHeadZ1 == FALSE && m_bIsMotionTestBondHeadZ2 == FALSE && 
			m_bIsMotionTestBondArm == FALSE && m_bIsMotionTestEjector == FALSE && m_bILCContourMoveTest == FALSE && m_bIsMotionTestPBTZ == FALSE) 
	{
		return TRUE;
	}

	BOOL bLogAxis = FALSE;

	if ((m_bMoveDirection == TRUE && m_lMotionTestDist >= 0) || (m_bMoveDirection == FALSE && m_lMotionTestDist < 0))
	{
		bLogAxis = TRUE;
	}

	INT nTestDistance = (INT)m_lMotionTestDist;

	if (m_bIsMotionTestBondHeadZ1 == TRUE)
	{
		if (bLogAxis)
		{
			//LogAxisPerformance(BH_AXIS_Z, &m_stBHAxis_Z, TRUE, 10, m_stDataLog);
		}

		if (m_bMoveDirection)
		{
			LaserTestOuputBitTrigger(FALSE);

			if (Z_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBondHeadZ1 = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}
		else
		{
			nTestDistance = -1 * nTestDistance;

			LaserTestOuputBitTrigger(FALSE);
			
			if (Z_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBondHeadZ1 = FALSE;
			}	

			LaserTestOuputBitTrigger(TRUE);
		}
		
		if (bLogAxis)
		{
			//LogAxisPerformance(BH_AXIS_Z, &m_stBHAxis_Z, FALSE, 10, m_stDataLog);
		}
	}
	else if (m_bIsMotionTestBondHeadZ2 == TRUE)
	{
		if (bLogAxis)
		{
			//LogAxisPerformance(BH_AXIS_Z2, &m_stBHAxis_Z2, TRUE, 10, m_stDataLog);
		}

		if (m_bMoveDirection)
		{
			LaserTestOuputBitTrigger(FALSE);

			if (Z2_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBondHeadZ2 = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}
		else
		{
			nTestDistance = -1 * nTestDistance;

			LaserTestOuputBitTrigger(FALSE);

			if (Z2_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestBondHeadZ2 = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}

		if (bLogAxis)
		{
			//LogAxisPerformance(BH_AXIS_Z2, &m_stBHAxis_Z2, FALSE, 10, m_stDataLog);
		}
	}
	else if (m_bILCContourMoveTest == TRUE)
	{
		if (ILC_ContourMoveTest() == FALSE)
		{
			m_bILCContourMoveTest = FALSE;
		}			
	}
	else if (m_bIsMotionTestBondArm == TRUE)
	{
		if (IsCoverOpen())
		{
			SetErrorMessage("Cover open in bond arm motion test");
			SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
			m_bIsMotionTestBondArm = FALSE;
			return TRUE;
		}

		if (m_lMotionTestDist == BH_T_PICK_TO_BOND)
		{
			if (m_bMoveDirection)
			{
				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, TRUE, 10, m_stDataLog);

				LaserTestOuputBitTrigger(FALSE);
	
				if (T_MoveTo(m_lPickPos_T) != gnOK)
				{
					m_bIsMotionTestBondArm = FALSE;
				}
	
				LaserTestOuputBitTrigger(TRUE);

				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
			}
			else
			{
				LaserTestOuputBitTrigger(FALSE);
	
				if (T_MoveTo(m_lBondPos_T) != gnOK)
				{
					m_bIsMotionTestBondArm = FALSE;
				}
	
				LaserTestOuputBitTrigger(TRUE);
			}
		}
		else if (m_lMotionTestDist == BH_T_BOND_TO_PICK)
		{
			if (m_bMoveDirection)
			{
				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, TRUE, 10, m_stDataLog);

				LaserTestOuputBitTrigger(FALSE);

				if (T_MoveTo(m_lBondPos_T) != gnOK)
				{
					m_bIsMotionTestBondArm = FALSE;
				}
	
				LaserTestOuputBitTrigger(TRUE);

				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
			}
			else
			{
				LaserTestOuputBitTrigger(FALSE);
	
				if (T_MoveTo(m_lPickPos_T) != gnOK)
				{
					m_bIsMotionTestBondArm = FALSE;
				}
	
				LaserTestOuputBitTrigger(TRUE);
			}
		}
		else if (m_lMotionTestDist == BH_T_PREPICK_TO_PICK)
		{
			if (m_bMoveDirection)
			{
				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, TRUE, 10, m_stDataLog);

				LaserTestOuputBitTrigger(FALSE);

				if (T_MoveTo(m_lPickPos_T) != gnOK)
				{
					m_bIsMotionTestBondArm = FALSE;
				}

				LaserTestOuputBitTrigger(TRUE);

				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
			}
			else
			{
				LaserTestOuputBitTrigger(FALSE);
	
				if (T_MoveTo(m_lPrePickPos_T) != gnOK)
				{
					m_bIsMotionTestBondArm = FALSE;
				}
	
				LaserTestOuputBitTrigger(TRUE);
			}
		}
		else
		{
			if (m_bMoveDirection)
			{
				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, TRUE, 10, m_stDataLog);

				LaserTestOuputBitTrigger(FALSE);

				if (T_MoveTo(m_lBondPos_T) != gnOK)
				{
					m_bIsMotionTestBondArm = FALSE;
				}
	
				LaserTestOuputBitTrigger(TRUE);

				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
			}
			else
			{
				LaserTestOuputBitTrigger(FALSE);
				
				if (T_MoveTo(m_lPrePickPos_T) != gnOK)
				{
					m_bIsMotionTestBondArm = FALSE;
				}

				LaserTestOuputBitTrigger(TRUE);
			}
		}
	}
	else if (m_bIsMotionTestEjector == TRUE)
	{
		if (bLogAxis)
		{
			//LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, TRUE, 10, m_stDataLog);
		}

		if (m_bMoveDirection)
		{
			LaserTestOuputBitTrigger(FALSE);
			if (Ej_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestEjector = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}
		else
		{
			nTestDistance = -1 * nTestDistance;
			LaserTestOuputBitTrigger(FALSE);

			if (Ej_Move(nTestDistance) != gnOK)
			{
				m_bIsMotionTestEjector = FALSE;
			}

			LaserTestOuputBitTrigger(TRUE);
		}

		if (bLogAxis)
		{
			//LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, FALSE, 10, m_stDataLog);
		}
	}
	else if (m_bIsMotionTestPBTZ == TRUE)
	{
				m_bIsMotionTestPBTZ = FALSE;
	}

	//reverse the direction
	m_bMoveDirection = !m_bMoveDirection;

	if (m_lMotionTestDelay > 0)
	{
		Sleep(m_lMotionTestDelay);
	}

	if (m_bILCContourMoveTest == FALSE)
	{
		Sleep(500);
	}

	// for motion settling and reverse direction
	
	return TRUE;
	*/
}

BOOL CBondHead::ManualCleanColletShortcutFunc()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if (!m_bCleanColletToggle)		//v3.25T2
	{
		Z_MoveTo(0);
#ifdef NU_MOTION
		Z2_MoveTo(0);
#endif

		if ( //(pApp->GetCustomerName() == "Cree") &&	//v4.47A5
			 (pApp->GetBHZ1HomeOffset() > 0) &&
			 (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
#ifdef NU_MOTION
			Z2_Move(pApp->GetBHZ2HomeOffset());
#endif
			Z_Move(pApp->GetBHZ1HomeOffset());
		}

		T_Profile(CLEAN_COLLET_PROF);
		T_SMoveTo(m_lCleanColletPos_T);		//v4.53A14
		T_Profile(NORMAL_PROF);

		SetStrongBlow(TRUE);	
#ifdef NU_MOTION
		SetStrongBlowZ2(TRUE);
#endif
		Sleep(1000);
		SetStrongBlow(FALSE);	
#ifdef NU_MOTION
		SetStrongBlowZ2(FALSE);
#endif

		CString szTitle, szContent;
		szTitle.LoadString(HMB_BH_CLEAN_COLLET);
		szContent.LoadString(HMB_BH_CLEAN_COLLET_OK);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);

		m_bCleanColletToggle = TRUE;
	}
	else
	{
		CString szTitle, szContent;
		szTitle.LoadString(HMB_BH_CLEAN_COLLET);
		szContent = "Home bondhead?";

		if (HmiMessage(szContent, szTitle, glHMI_MBX_OKCANCEL | 0x80000000, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) != glHMI_OK)
		{
			//svMsg.InitMessage(sizeof(BOOL), &bReturn);
			//return 1;
			return FALSE;
		}

		// Home the Bond head in order to prevent its loss torque during clean collet
		//v3.98T5
		if (Z_IsPowerOn())
		{
			Z_MoveTo(0);

			//v4.46T20
			CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
			//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
			//{
				if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
				{
					Sleep(100);
					Z_Move(pApp->GetBHZ1HomeOffset());
				}
			//}
		}
		else
		{
			Z_Home();
		}

#ifdef NU_MOTION
		if (Z2_IsPowerOn())
		{
			Z2_MoveTo(0);

			//v4.46T20
			CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
			//if ( (pApp->GetCustomerName() == "Cree") )	//v4.47A5
			//{
				if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
				{
					Sleep(100);
					Z2_Move(pApp->GetBHZ2HomeOffset());
				}
			//}
		}
		else
		{
			Z2_Home();
		}
#endif

		//v4.46T20
		if ( //(pApp->GetCustomerName() == "Cree") && 	//v4.47A5
				(pApp->GetBHZ1HomeOffset() > 0) &&
				(pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
#ifdef NU_MOTION
			Z2_Move(pApp->GetBHZ2HomeOffset());
#endif
			Z_Move(pApp->GetBHZ1HomeOffset());
		}

		// Check whether all motors is enable in order 
		// to prevent the bond arm collision with the expander
		// when the theta is loss its torque
		if (IsAllMotorsEnable() == FALSE)
		{
			SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
			SetErrorMessage("BondArm module is off power");
			return FALSE;
		}

		//v3.98T5	//Check expander status before moving back to PREPICK
		if (IsWLExpanderOpen() == TRUE)
		{
			SetErrorMessage("Expander not closed");
			return FALSE;
		}

		//v3.98T5	//Check bintable frame level before moving
		if (!IsBT1FrameLevel())
		{
			SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);		
			SetErrorMessage("bin level not down");
			//svMsg.InitMessage(sizeof(BOOL), &bReturn);
			//return 1;
			return FALSE;
		}


		//Move back to pre-pick position
		T_Profile(CLEAN_COLLET_PROF);	
		T_SMoveTo(m_lPrePickPos_T);		//v4.53A14
		T_Profile(NORMAL_PROF);

		m_bCleanColletToggle = FALSE;
	}

	return TRUE;
}

BOOL CBondHead::CycleEnableDataLog(BOOL bEnable, BOOL bDisplay, BOOL bWriteToFile)
{
	//v4.50A24
	if (!m_bEnableNuDataLog && !bEnable)
		return TRUE;
	else if (m_bEnableNuDataLog && bEnable)
		return TRUE;

	m_bEnableNuDataLog = bEnable;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	UINT unDataLog = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Auto Cycle NuMotion Data Log"), 0);
	if( unDataLog==1 )
	{
		return TRUE;
	}

	if ( m_bDataLogListInNewMode == TRUE )
	{ // Add New Datalog Method  // v4.44
		NuMotionDataLogExt_MS100(bEnable, bWriteToFile);
		return TRUE;
	}

	if ( m_bEnableMotionLogEJTX || m_bEnableMotionLogEJTY )
	{
		if (m_bEnableMotionLogEJTX && m_bEnableMotionLogEJTY && m_bEnableMotionLogZ)
		{
			LogAxisPerformance3(BH_AXIS_EJ_X, BH_AXIS_EJ_Y, BH_AXIS_Z, &m_stBHAxis_EjX, &m_stBHAxis_EjY, &m_stBHAxis_Z, bEnable, bWriteToFile);
		}
		else if (m_bEnableMotionLogEJTX && m_bEnableMotionLogEJTY && m_bEnableMotionLogT)
		{
			LogAxisPerformance3(BH_AXIS_EJ_X, BH_AXIS_EJ_Y, BH_AXIS_T, &m_stBHAxis_EjX, &m_stBHAxis_EjY, &m_stBHAxis_T, bEnable, bWriteToFile);
		}
		else if (m_bEnableMotionLogEJTX && m_bEnableMotionLogEJTY && m_bEnableMotionLogEJ)
		{
			LogAxisPerformance3(BH_AXIS_EJ_X, BH_AXIS_EJ_Y, BH_AXIS_EJ, &m_stBHAxis_EjX, &m_stBHAxis_EjY, &m_stBHAxis_Ej, bEnable, bWriteToFile);
		}
		else if (m_bEnableMotionLogEJTX && m_bEnableMotionLogEJTY && m_bEnableMotionLogZ2)
		{
			LogAxisPerformance3(BH_AXIS_EJ_X, BH_AXIS_EJ_Y, BH_AXIS_Z2, &m_stBHAxis_EjX, &m_stBHAxis_EjY, &m_stBHAxis_Z2, bEnable, bWriteToFile);
		}
		else if (m_bEnableMotionLogEJTX && m_bEnableMotionLogEJTY)
		{
			LogAxisPerformance2(BH_AXIS_EJ_X, BH_AXIS_EJ_Y, &m_stBHAxis_EjX, &m_stBHAxis_EjY, bEnable, bDisplay, FALSE, bWriteToFile);
		}
		else if (m_bEnableMotionLogEJTX)
		{
			LogAxisPerformance(BH_AXIS_EJ_X, &m_stBHAxis_EjX, bEnable, 10, m_stDataLog, 0, bDisplay, bWriteToFile);
		}
		else if (m_bEnableMotionLogEJTY)
		{
			LogAxisPerformance(BH_AXIS_EJ_Y, &m_stBHAxis_EjY, bEnable, 10, m_stDataLog, 0, bDisplay, bWriteToFile);
		}
	}
	else 
	{
		if (m_bEnableMotionLogT && m_bEnableMotionLogZ && m_bEnableMotionLogZ2)
		{
			LogAxisPerformance3(BH_AXIS_T, BH_AXIS_Z, BH_AXIS_Z2, &m_stBHAxis_T, &m_stBHAxis_Z, &m_stBHAxis_Z2, bEnable, bWriteToFile);
		}
		else if (m_bEnableMotionLogZ && m_bEnableMotionLogZ2 && m_bEnableMotionLogEJ)	//v4.09
		{
			LogAxisPerformance3(BH_AXIS_Z, BH_AXIS_Z2, BH_AXIS_EJ, &m_stBHAxis_Z, &m_stBHAxis_Z2, &m_stBHAxis_Ej, bEnable, bWriteToFile);
		}
		else if (m_bEnableMotionLogZ && m_bEnableMotionLogEJ)		//Z + EJ
		{
			LogAxisPerformance2(BH_AXIS_Z, BH_AXIS_EJ, &m_stBHAxis_Z, &m_stBHAxis_Ej, bEnable, bDisplay, FALSE, bWriteToFile);
		}
		else if (m_bEnableMotionLogZ2 && m_bEnableMotionLogEJ)	//Z2 + EJ
		{
			LogAxisPerformance2(BH_AXIS_Z2, BH_AXIS_EJ, &m_stBHAxis_Z2, &m_stBHAxis_Ej, bEnable, bDisplay, FALSE, bWriteToFile);
		}
		else if (m_bEnableMotionLogZ && m_bEnableMotionLogT)	//Z + T
		{
			LogAxisPerformance2(BH_AXIS_Z, BH_AXIS_T, &m_stBHAxis_Z, &m_stBHAxis_T, bEnable, bDisplay, FALSE, bWriteToFile);
		}
		else if (m_bEnableMotionLogZ2 && m_bEnableMotionLogT)	//Z2 + T
		{
			LogAxisPerformance2(BH_AXIS_Z2, BH_AXIS_T, &m_stBHAxis_Z2, &m_stBHAxis_T, bEnable, bDisplay, FALSE, bWriteToFile);
		}
		else if (m_bEnableMotionLogT && m_bEnableMotionLogEJ)	//T + EJ
		{
			LogAxisPerformance2(BH_AXIS_T, BH_AXIS_EJ, &m_stBHAxis_T, &m_stBHAxis_Ej, bEnable, bDisplay, FALSE, bWriteToFile);
		}
		else if (m_bEnableMotionLogZ && m_bEnableMotionLogZ2)	//Z + Z2
		{
			if (m_bMS100DigitalAirFlowSnr)	//v4.43T12
			{
				LogAxisPerformance2(BH_AXIS_Z, BH_AXIS_Z2, &m_stBHAxis_Z, &m_stBHAxis_Z2, bEnable, bDisplay, TRUE, bWriteToFile);
			}
			else
			{
				LogAxisPerformance2(BH_AXIS_Z, BH_AXIS_Z2, &m_stBHAxis_Z, &m_stBHAxis_Z2, bEnable, bDisplay, FALSE, bWriteToFile);
			}
		}
		else if (m_bEnableMotionLogZ)
		{
			LogAxisPerformance(BH_AXIS_Z, &m_stBHAxis_Z, bEnable, 10, m_stDataLog, 0, bDisplay, bWriteToFile);
		}
		else if (m_bEnableMotionLogT)
		{
			LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, bEnable, 10, m_stDataLog, 0, bDisplay, bWriteToFile);
		}
		else if (m_bEnableMotionLogEJ)
		{
			LogAxisPerformance(BH_AXIS_EJ, &m_stBHAxis_Ej, bEnable, 10, m_stDataLog, 0, bDisplay, bWriteToFile);
		}
		else if (m_bEnableMotionLogZ2)
		{
			LogAxisPerformance(BH_AXIS_Z2, &m_stBHAxis_Z2, bEnable, 10, m_stDataLog, 0, bDisplay, bWriteToFile);
		}
	}
	return TRUE;
}

BOOL CBondHead::IsWT1UnderEjectorPos()
{
	LONG lX, lY, lT;

	GetES101WTEncoder(&lX, &lY, &lT, FALSE);

	return IsWithinWT1WaferLimit(lX, lY, 1, TRUE);
}

BOOL CBondHead::IsWT2UnderEjectorPos()
{
	LONG lX, lY, lT;

	GetES101WTEncoder(&lX, &lY, &lT, TRUE);

	return IsWithinWT2WaferLimit(lX, lY, 1, TRUE);
}

VOID CBondHead::GetES101WTEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis, BOOL bWT2)
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

BOOL CBondHead::SetPrAutoBondMode(BOOL bAutoBondMode)
{
	IPC_CServiceMessage stMsg;
	int		nConvID = 0;
	BOOL	bReturn;

	if (bAutoBondMode == FALSE)
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetPRSysLiveMode", stMsg);
	}
	else
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetPRSysBondMode", stMsg);
	}

	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
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

// 2D barcode fcn
BOOL CBondHead::Search2DBarCodePattern(CString &szCode, BOOL bBH2, BOOL bRetry)
{
	if (!m_bCheckPr2DCode)
	{
		return FALSE;
	}

	PR_READ_2DCODE_CMD	stRead2DCmd;
	PR_READ_2DCODE_RPY1	stRead2DRpy1;
	PR_READ_2DCODE_RPY2	stRead2DRpy2; 
	PR_InitRead2DCodeCmd(&stRead2DCmd);

	PR_UBYTE	ubSendID	= PSPR_SENDER_ID;
	PR_UBYTE	ubRecvID	= PSPR_RECV_ID;
	PR_PURPOSE	ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
	if (bBH2)
	{
		ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
	}
	stRead2DCmd.emPurpose = ubPrPurPose;

	stRead2DCmd.stSrchWin.coCorner1.x	= PR_MAX_WIN_ULC_X + 1000;
	stRead2DCmd.stSrchWin.coCorner1.y	= PR_MAX_WIN_ULC_Y + 1000;
	stRead2DCmd.stSrchWin.coCorner2.x	= PR_MAX_WIN_LRC_X - 1000;
	stRead2DCmd.stSrchWin.coCorner2.y	= PR_MAX_WIN_LRC_Y - 1000;

	stRead2DCmd.emGraphicInfo			= PR_DISPLAY_CENTRE;
	stRead2DCmd.emSameView				= PR_FALSE;
	stRead2DCmd.emType					= PR_2D_CODE_TYPE_0; 
	stRead2DCmd.emOpMode				= PR_2D_CODE_OPMODE_NORMAL;
	stRead2DCmd.emMirrorImage			= PR_TRUE;

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
		//AfxMessageBox(csMsg, MB_SYSTEMMODAL);
		return FALSE;
	}

	PR_Read2DCodeRpy(ubSendID, ubRecvID, &stRead2DRpy2);

	if ((stRead2DRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) ||
			(stRead2DRpy2.stStatus.uwPRStatus != PR_ERR_NOERR))
	{
		CString csMsg;
		csMsg.Format("WPR PR_Read2DCodeRpy error = 0x%x, 0x%x", stRead2DRpy2.stStatus.uwCommunStatus, stRead2DRpy2.stStatus.uwPRStatus);
		SetErrorMessage(csMsg);
		HmiMessage_Red_Back(csMsg, "BH 2D Code");
		return FALSE;
	}

	szCode = stRead2DRpy2.aubCodeStr;

	CString szFrameBarcode = (*m_psmfSRam)["WaferTable"]["WT1InBarcode"];
	CString szLog;
	
	if (szCode.GetLength() == 0)
	{
		if (bBH2)
			szLog = "Scan BH 2D code fails (BH2)";
		else
			szLog = "Scan BH 2D code fails (BH1)";

		SetErrorMessage(szLog);
		HmiMessage_Red_Back(szLog, "BH 2D Code");
		return FALSE;
	}

	if (szCode != szFrameBarcode)
	{
		if (bBH2)
			szLog.Format("Scan BH 2D code fails (BH2): CODE = %s; frame 2D Code = %s", 
				(LPCTSTR) szCode, (LPCTSTR) szFrameBarcode);
		else
			szLog.Format("Scan BH 2D code fails (BH1): CODE = %s; frame 2D Code = %s", 
				(LPCTSTR) szCode, (LPCTSTR) szFrameBarcode);
		SetErrorMessage(szLog);
		HmiMessage_Red_Back(szLog, "BH 2D Code");
		return FALSE;
	}

	return TRUE;
}

BOOL CBondHead::IsCheckPr2DCode()
{
	if (!m_bCheckPr2DCode)
	{
		return FALSE;
	}
	if (m_lPr2DCodeCheckLimit == 0)
	{
		return FALSE;
	}
	BOOL bWafer2DCodeRead = (BOOL)(LONG)(*m_psmfSRam)["WaferLoaderStn"]["2D Barcode"];
	//if (!bWafer2DCodeRead)		//current wafer 2D code is not read
	//{
	//	return FALSE;
	//}

	m_lPr2DCodeCheckCounter++;

	if (m_lPr2DCodeCheckCounter >= m_lPr2DCodeCheckLimit)
	{
		m_lPr2DCodeCheckCounter = 0;
		m_bIs2DCodeDone = FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL CBondHead::UplookPrSearchDie(BOOL bBH2)		//v4.52A16
{
	BOOL bStatus = TRUE;
	//David: please try to call IPC command to WPR to search die
	// on Uplook PR 1 or 2, based on which BH has die on collet tip here;


	//if bBH2 = TRUE, then search die on Uplook PR2




	//Set event to trigger BT station to perform compensation
	if (bStatus)
	{
		(*m_psmfSRam)["BinTable"]["UplookCompX"] = 0;	//Please update this value in encoder count
		(*m_psmfSRam)["BinTable"]["UplookCompY"] = 0;	//Please update this value in encoder count
		SetBTCompensate(TRUE);
	}

	return bStatus;
}

BOOL CBondHead::AreAllBinsCleared()
{
	BOOL bIsBinAllCleared = TRUE;
	IPC_CServiceMessage rReqMsg;
	INT nConvID = 0;

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIsAllBinCleared", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bIsBinAllCleared);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bIsBinAllCleared;
}

BOOL CBondHead::CheckZ1PickBondLevelOffset()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_BONDHEAD_TOLERANCE_LEVEL) && 
		m_lZPBLevelOffsetLimit>0 )
	{
		if( labs(m_lPickLevel_Z - m_lBondLevel_Z)>m_lZPBLevelOffsetLimit )
		{
			CString szMsg;
			szMsg.Format("Head 1 Pick %d Bond %d.\nOffset over limit %d steps.",
				m_lPickLevel_Z, m_lBondLevel_Z, m_lZPBLevelOffsetLimit);
			HmiMessage_Red_Back(szMsg, "BH Level");
			SetErrorMessage(szMsg);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBondHead::CheckZ2PickBondLevelOffset()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_BONDHEAD_TOLERANCE_LEVEL) && 
		m_lZPBLevelOffsetLimit>0 )
	{
		if( labs(m_lPickLevel_Z2 - m_lBondLevel_Z2)>m_lZPBLevelOffsetLimit )
		{
			CString szMsg;
			szMsg.Format("Head 2 Pick %d Bond %d.\nOffset over limit %d steps.",
			m_lPickLevel_Z2, m_lBondLevel_Z2, m_lZPBLevelOffsetLimit);
			HmiMessage_Red_Back(szMsg, "BH Level");
			SetErrorMessage(szMsg);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBondHead::CheckNewOldLevelOffset(LONG lOffset)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_BONDHEAD_TOLERANCE_LEVEL) && 
		labs(m_lBondHeadToleranceLevel)>0 )
	{
		if ( labs(lOffset) > labs(m_lBondHeadToleranceLevel) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBondHead::EjSoftTouch_CMD()
{
	GetEjAxisInformation();	//M94

	double dIndex = 1;
	if(m_szEjSoftTouch == "100")
		dIndex = 1;
	if(m_szEjSoftTouch == "90")
		dIndex = 0.9;
	if(m_szEjSoftTouch == "80")
		dIndex = 0.8;
	if(m_szEjSoftTouch == "70")
		dIndex = 0.7;
	if(m_szEjSoftTouch == "60")
		dIndex = 0.6;
	if(m_szEjSoftTouch == "50")
		dIndex = 0.5;
	if(m_szEjSoftTouch == "40")
		dIndex = 0.4;
	if(m_szEjSoftTouch == "30")
		dIndex = 0.3;
	if(m_szEjSoftTouch == "20")
		dIndex = 0.2;
	if(m_szEjSoftTouch == "10")
		dIndex = 0.1;

	double dEjCapIndex = 1;
	if (m_szEjCapSoftTouch == "90")
		dEjCapIndex = 0.9;
	else if (m_szEjCapSoftTouch == "80")
		dEjCapIndex = 0.8;
	else if (m_szEjCapSoftTouch == "70")
		dEjCapIndex = 0.7;
	else if (m_szEjCapSoftTouch == "60")
		dEjCapIndex = 0.6;
	else if (m_szEjCapSoftTouch == "50")
		dEjCapIndex = 0.5;
	else if (m_szEjCapSoftTouch == "40")
		dEjCapIndex = 0.4;
	else if (m_szEjCapSoftTouch == "30")
		dEjCapIndex = 0.3;
	else if (m_szEjCapSoftTouch == "20")
		dEjCapIndex = 0.2;
	else if (m_szEjCapSoftTouch == "10")
		dEjCapIndex = 0.1;
	else //if (m_szEjCapSoftTouch == "100")
	{
		dEjCapIndex = 1;
	}

	for (int i=0; i<UM_MAX_MOVE_PROFILE; i++)
	{
		if (m_stBHAxis_Ej.m_stMoveProfile[i].m_szID == BH_MP_E_OBW_DEFAULT)		//v4.53A9	//M92
		{
			m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxVel = m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxVel * dIndex;
			m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxAcc = m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxAcc * dIndex;
			m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxDec = m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxDec * dIndex;
		
			CString szLog;
			szLog.Format("Ejector Speed is Changed - (%s) - VEL=%.2f, ACC=%.2f, DCC=%.2f",
							m_szEjSoftTouch,
							m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxVel,
							m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxAcc,
							m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxDec);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			DOUBLE dVel = m_stBHAxis_Ej.m_stMoveProfile[i].m_dMaxVel;
			dVel = dVel * 0.5 * 8000.0 * 0.001;		// mm/sec

			m_szEjSoftTouchMsg.Format("VEL = %.4f mm/sec", dVel);
		}

		if (IsMS60() && m_bMS60EjElevator)		//v4.54A6	//CKHarry & LeoLam for SEmitek
		{
			if (m_stBHAxis_EjElevator.m_stMoveProfile[i].m_szID == BH_MP_E_OBW_DOWN)		
			{
				DOUBLE dOldMaxVel = m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxVel;
				DOUBLE dOldMaxAcc = m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxAcc;
				DOUBLE dOldMaxDec = m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxDec;

				m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxVel = m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxVel * dEjCapIndex;
				m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxAcc = m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxAcc * dEjCapIndex;
				m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxDec = m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxDec * dEjCapIndex;
			
				CString szLog;
				szLog.Format("EjCap Speed is Changed - (%s) - VEL=%.2f(%.2f), ACC=%.2f(%.2f), DCC=%.2f(%.2f)",
								m_szEjCapSoftTouch,
								m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxVel,		dOldMaxVel,
								m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxAcc,		dOldMaxAcc,
								m_stBHAxis_EjElevator.m_stMoveProfile[i].m_dMaxDec,		dOldMaxDec);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}
		}
	}

	Ej_Profile(NORMAL_PROF);
	return TRUE;
}


BOOL CBondHead::OpAutoLearnPickBondLevels(CONST BOOL bAuto)
{
	//No BHT in Mega Da
	return TRUE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

//	auto cycle mode check
	if( bAuto )
	{
		if( m_lAutoLearnLevelsIdleTime<=0 )
		{
			return TRUE;
		}

		if( m_ctBHTAutoCycleLastMoveTime.GetYear() != 2000 )
		{
			CTime curTime = CTime::GetCurrentTime();
			CTimeSpan TimeDiff = curTime - m_ctBHTAutoCycleLastMoveTime;
			if (TimeDiff.GetTotalMinutes() < m_lAutoLearnLevelsIdleTime )
			{
				return TRUE;
			}
		}
	}

// safety checking.
	if (m_bDisableBH)
	{
		return TRUE;
	}

	BOOL bRealignFrame = (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"];
	if( !bRealignFrame )
	{
		HmiMessage_Red_Back("Bin frame is not realigned, Please turn on Realign Option", "Auto Learn Pick-Bond Levels");
		return FALSE;
	}

	//v4.55A5
	if (!IsBondArmAbleToMove())
	{
		//bReturn = FALSE;
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return FALSE;
	}


// bond head move to home. bond arm to prepick
	CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels Z to safe");
	Z_MoveToHome();
	Z_MoveTo(m_lSwingLevel_Z);
	if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
	{
		Sleep(100);
		Z_Move(pApp->GetBHZ1HomeOffset());
	}

	if( HasBHZ2() )
	{
		Z2_MoveToHome();
		Z2_MoveTo(m_lSwingLevel_Z2);
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
	}

	if (m_bMS60EjElevator && bAuto==FALSE)
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}

	if (!IsBondArmAbleToMove())
	{
		return FALSE;
	}
	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_SMoveTo(m_lPrePickPos_T);

	CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels BT to next offset");
	//	move bin table to a position for bond head learn bond level.
	if (!MoveBTForAutoCColletAutoLearnZ(TRUE))
	{
		return FALSE;
	}

	typedef struct
	{
		LONG lX;
		LONG lY;
	} BT_XY_STRUCT;
	BT_XY_STRUCT	stPosnBT;
	stPosnBT.lX = 0;
	stPosnBT.lY = 0;
	if( m_bUseMultiProbeLevel )	//	auto learn clean collet
	{
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels Get BT block corner");
		//	get bin table current position X and Y.
		BT_Get_XY_Posn(stPosnBT.lX, stPosnBT.lY);
	}

	//	move wafer table to some where and find a good normal die and correct to die center.
	//	get old wafer table x and y and ejector table x and y

	if( bAuto==FALSE )
	{
		UserSearchDieToPrCenter(FALSE);
	}

	LONG lWftX = 0, lWftY = 0, lWftT = 0;

	LONG lBHZ1OffsetX = GetEjtCollet1OffsetX();
	LONG lBHZ1OffsetY = GetEjtCollet1OffsetY();
	LONG lBHZ2OffsetX = GetEjtCollet2OffsetX();
	LONG lBHZ2OffsetY = GetEjtCollet2OffsetY();

	GetES101WTEncoder(&lWftX, &lWftY, &lWftT, FALSE);

	if (m_bMS60EjElevator && bAuto==FALSE)
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}

	LONG lOldPickLevelZ1 = m_lPickLevel_Z;
	LONG lOldBondLevelZ1 = m_lBondLevel_Z;
	LONG lOldPickLevelZ2 = m_lPickLevel_Z2;
	LONG lOldBondLevelZ2 = m_lBondLevel_Z2;
	LONG lNewPickLevelZ1 = lOldPickLevelZ1 + 1000;
	LONG lNewBondLevelZ1 = lOldBondLevelZ1 + 1000;
	LONG lNewPickLevelZ2 = lOldPickLevelZ2 + 1000;
	LONG lNewBondLevelZ2 = lOldBondLevelZ2 + 1000;

	CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels onoff valves");
	SetPickVacuum(FALSE);
	SetStrongBlow(TRUE);
	if( HasBHZ2() )
	{
		SetPickVacuumZ2(FALSE);
		SetStrongBlowZ2(TRUE);
	}
	Sleep(100);
	SetStrongBlow(FALSE);
	if( HasBHZ2() )
	{
		SetStrongBlowZ2(FALSE);
	}
	Sleep(500);
	SetPickVacuum(TRUE);
	if( HasBHZ2() )
	{
		SetPickVacuumZ2(TRUE);
	}
	Sleep(200);

	// BHZ1 to learn pick level and BHZ2 to learn bond level
	CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels T to pick");
	CMSLogFileUtility::Instance()->MS_LogOperation("BH: Auto Learn All Levels start ...");

	if (!IsBondArmAbleToMove())
	{
		return FALSE;
	}
	T_SMoveTo(m_lPickPos_T);
	Sleep(100);

	CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels learn pick z1");
	LONG lAutoLearnState = 0;
	Z_MoveTo(lNewPickLevelZ1);
	if (!AutoLearnBHZ1BondLevel(lNewPickLevelZ1) || !Z_IsPowerOn())
	{
		Z_Home();
		lAutoLearnState = 1;
	}
	lNewPickLevelZ1 += m_lPickDriveIn;
	Z_MoveTo(m_lSwingLevel_Z);
	if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
	{
		Sleep(100);
		Z_Move(pApp->GetBHZ1HomeOffset());
	}
	Sleep(100);

	if( HasBHZ2() && lAutoLearnState==0 )
	{
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels learn bond z2");
		Z2_MoveTo(lNewBondLevelZ2);
		if (!AutoLearnBHZ2BondLevel(lNewBondLevelZ2) || !Z2_IsPowerOn())	// can be used for pick level auto learn too.
		{
			Z2_Home();
			lAutoLearnState = 2;
		}
		lNewBondLevelZ2 += m_lBondDriveIn;
		Z2_MoveTo(m_lSwingLevel_Z2);
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
		Sleep(100);
	}

	CString szMsg;
	//	BHZ2 to learn pick level and BHZ1 to learn bond level
	if( bAuto==FALSE )
	{
		if (!IsBondArmAbleToMove())
		{
			return FALSE;
		}
		T_MoveTo(m_lPrePickPos_T);
	}
	if( HasBHZ2() )
	{
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels move wft for z2");
		LONG lMoveWftX = lWftX + lBHZ2OffsetX - lBHZ1OffsetX;
		LONG lMoveWftY = lWftY + lBHZ2OffsetY - lBHZ1OffsetY;
		MoveWaferTableNoCheck(lMoveWftX, lMoveWftY);
	//	HmiMessage("debug check 2 at BHZ2 pick position");
	}

	if( lAutoLearnState==0 )
	{
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels move T to bond");
		if (!IsBondArmAbleToMove())
		{
			return FALSE;
		}
		T_SMoveTo(m_lBondPos_T);
		Sleep(100);

		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels learn bond z1");
		Z_MoveTo(lNewBondLevelZ1);
		if (!AutoLearnBHZ1BondLevel(lNewBondLevelZ1) || !Z_IsPowerOn())
		{
			Z_Home();
			lAutoLearnState = 3;
		}
		lNewBondLevelZ1 += m_lBondDriveIn;
		Z_MoveTo(m_lSwingLevel_Z);
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}
		Sleep(100);
	}

	if( HasBHZ2() && lAutoLearnState==0 )
	{
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels learn pick z2");
		Z2_MoveTo(lNewPickLevelZ2);
		if (!AutoLearnBHZ2BondLevel(lNewPickLevelZ2) || !Z2_IsPowerOn())	// can be used for pick level auto learn too.
		{
			Z2_Home();
			lAutoLearnState = 4;
		}
		lNewPickLevelZ2 += m_lPickDriveIn;
		Z2_MoveTo(m_lSwingLevel_Z2);
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}
		Sleep(100);
	}

	CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels T to prepick");
	SetPickVacuum(FALSE);
	SetPickVacuumZ2(FALSE);
	if (!IsBondArmAbleToMove())
	{
		return FALSE;
	}
	T_SMoveTo(m_lPrePickPos_T);
	T_Profile(NORMAL_PROF);

//	move bin table and wafer table and ejector table back to previous in auto mode.
	if( m_bMS100EjtXY )
	{
	//	LONG lMoveX = lWftX - lBHZ1OffsetX;
	//	LONG lMoveY = lWftY - lBHZ1OffsetY;
	//	MoveWaferTableNoCheck(lMoveX, lMoveY);
	//	HmiMessage("debug check 3 at PR center.");
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels move WFT back");
		MoveWaferTableNoCheck(lWftX, lWftY);
	}

	if (m_bMS60EjElevator && bAuto==FALSE)
	{
		EjElevator_MoveTo(m_lStandby_EJ_Elevator);
	}

//	move bin table back to previous position.
	CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels move BT back");
	if (!MoveBTForAutoCColletAutoLearnZ(FALSE))
	{
		return FALSE;
	}

	CString szErr;
	switch( lAutoLearnState )
	{
	case 1:
		szErr = "BH: Auto Learn All Levels fail on BHZ1 @pick";
		break;
	case 2:
		szErr = "BH: Auto Learn All Levels fail on BHZ2 @bond";
		break;
	case 3:
		szErr = "BH: Auto Learn All Levels fail on BHZ2 @pick";
		break;
	}
	if( lAutoLearnState!=0 )
	{
		SetErrorMessage(szErr);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
		szErr = "\n" + szErr;
//		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);	//	reduce surplus alarm
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels " + szErr);
		return FALSE;
	}

//	auto learning done, check value range and validity.
	//xx check new/old level difference.
	LONG lOffsetZ = lNewPickLevelZ1 - lOldPickLevelZ1;
	if( /*CheckNewOldLevelOffset(lOffsetZ)==FALSE*/lOffsetZ > 500 )
	{
		szErr.Format("Auto-Learn3 Z1 pick fails because Offset %d > 500 steps!", lOffsetZ);
		HmiMessage_Red_Back(szErr, "Auto Learn 3");
		SetErrorMessage(szErr);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels " + szErr);
		return TRUE;
	}
	lOffsetZ = lNewBondLevelZ1 - lOldBondLevelZ1;
	if( /*CheckNewOldLevelOffset(lOffsetZ)==FALSE*/lOffsetZ > 500  )
	{
		szErr.Format("Auto-Learn3 Z1 bond fails because Offset %d > 500 steps!", lOffsetZ);
		HmiMessage_Red_Back(szErr, "Auto Learn 3");
		SetErrorMessage(szErr);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels " + szErr);
		return TRUE;
	}
	lOffsetZ = lNewPickLevelZ2 - lOldPickLevelZ2;
	if( /*CheckNewOldLevelOffset(lOffsetZ)==FALSE*/lOffsetZ > 500  )
	{
		szErr.Format("Auto-Learn3 Z2 pick fails because Offset %d > 500 steps!", lOffsetZ);
		HmiMessage_Red_Back(szErr, "Auto Learn 3");
		SetErrorMessage(szErr);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels " + szErr);
		return TRUE;
	}
	lOffsetZ = lNewBondLevelZ2 - lOldBondLevelZ2;
	if( /*CheckNewOldLevelOffset(lOffsetZ)==FALSE*/lOffsetZ > 500  )
	{
		szErr.Format("Auto-Learn3 Z2 bond fails because Offset %d > 500 steps!", lOffsetZ);
		HmiMessage_Red_Back(szErr, "Auto Learn 3");
		SetErrorMessage(szErr);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
		CMSLogFileUtility::Instance()->BH_LogStatus("OpAutoPickBondLevels " + szErr);
		return TRUE;
	}
	//xx check new/old level difference.

	m_lBPGeneral_6	= m_lPickLevel_Z	= lNewPickLevelZ1;
	m_lBPGeneral_7	= m_lBondLevel_Z	= lNewBondLevelZ1;
	m_lBPGeneral_B	= m_lPickLevel_Z2	= lNewPickLevelZ2;
	m_lBPGeneral_C	= m_lBondLevel_Z2	= lNewBondLevelZ2;

	//xx check new/old and pick/bond level difference.
	CheckZ1PickBondLevelOffset();
	CheckZ2PickBondLevelOffset();
	//xx check new/old and pick/bond level difference.

	szMsg.Format("Auto Learn PickBond Levels BHZ1 - new PICK=%ld(%ld), BOND=%ld(%ld) at BT(%ld,%ld)",
					lNewPickLevelZ1, lOldPickLevelZ1, lNewBondLevelZ1, lOldBondLevelZ1, stPosnBT.lX, stPosnBT.lY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	CMSLogFileUtility::Instance()->BH_LogStatus(szMsg);
	szMsg.Format("Auto Learn PickBond Levels BHZ2 - new PICK=%ld(%ld), BOND=%ld(%ld) at BT(%ld,%ld)",
					lNewPickLevelZ2, lOldPickLevelZ2, lNewBondLevelZ2, lOldBondLevelZ2, stPosnBT.lX, stPosnBT.lY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	CMSLogFileUtility::Instance()->BH_LogStatus(szMsg);

	if( m_bUseMultiProbeLevel )	//	auto learn clean collet
	{
		m_lZ2BondLevelBT_X = stPosnBT.lX;
		m_lZ2BondLevelBT_Y = stPosnBT.lY;
		m_lZ1BondLevelBT_X = stPosnBT.lX;
		m_lZ1BondLevelBT_Y = stPosnBT.lY;
		m_bZ1BondLevelBT_R = IsMS90BTRotated();
		m_bZ2BondLevelBT_R = IsMS90BTRotated();
		BH_MBL_LogLevel();
	}

	SetBondPadLevel(FALSE);		//	auto learn level
	SetBondPadLevel(TRUE);		//	auto learn level

	m_ctBHTAutoCycleLastMoveTime = CTime::GetCurrentTime();

	SaveBhData();

	return TRUE;
}


BOOL CBondHead::IsBondArmAbleToMove()			//v4.55A5
{
	//Check Wafer Expander
	BOOL bWaferLoader = (BOOL)((LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"]);
	if (bWaferLoader)
	{
		if (IsWLExpanderOpen() == TRUE)
		{
			SetErrorMessage("IsBondArmAbleToMove: WExpander not closed");
			return FALSE;
		}
	}

	//Check Pusher
	if (IsEnaAutoChangeCollet() && !IsColletPusherAtSafePos())
	{
		HmiMessage_Red_Yellow("AGC: Pusher is not at UP position");
		SetErrorMessage("IsBondArmAbleToMove: Pusher is not at UP position");
		return FALSE;
	}

	//Check BT frame level
	BOOL bBinLoader	= IsBLEnable();
	if (bBinLoader)
	{
		if (!IsBT1FrameLevel())
		{
			SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
			SetErrorMessage("BT frame level not DOWN");
			return FALSE;
		}
	}

	//Check Cover Sensor
	if( LoopCheckCoverOpen("CHANGE BA setup")==FALSE )
	{
		return FALSE;
	}

	//v4.55A11
	//Check all BH power
	//if (IsAllMotorsEnable() == FALSE)
	//{
	//	SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
	//	SetErrorMessage("BondArm module is off power");
	//	return FALSE;
	//}

	return TRUE;
}

BOOL CBondHead::UplookPrSearchDie_Rpy1(BOOL bBH2)
{
	CString szLog;
	BOOL bStatus = FALSE;
	LONG lRefDieNo;
	LONG lBHNo;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	//David: please try to call IPC command to WPR to search die
	// on Uplook PR 1 or 2, based on which BH has die on collet tip here;

	//if bBH2 = TRUE, then search die on Uplook PR2

	//v4.57A11
	//Reset all SRAM variables
/*
	(*m_psmfSRam)["BinTable"]["UplookCompX"] = 0;
	(*m_psmfSRam)["BinTable"]["UplookCompY"] = 0;
	(*m_psmfSRam)["BinTable"]["UplookCompT"] = 0;
	(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetX"] = 0;
	(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetY"] = 0;
	(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetX"] = 0;
	(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetY"] = 0;
*/

	if(!bBH2)
	{
		//4.52D17ID
		lRefDieNo = MS899_UPLOOK_REF_DIENO_BH1;
		lBHNo	= 1;
		stMsg.InitMessage(sizeof(LONG), &lBHNo);
	}
	else
	{
		lRefDieNo = MS899_UPLOOK_REF_DIENO_BH2;
		lBHNo	  = 2;

		//CMSLogFileUtility::Instance()->BH_LogStatus("Uplook --- No Search BH2");
		//return TRUE;
		stMsg.InitMessage(sizeof(LONG), &lBHNo);
	}

	szLog.Format("BH - UpLook PR 1 for BH %d", lBHNo);
	DisplaySequence(szLog);

	//szLog.Format("Uplook ---Start Search Die  Rpy1 with BH%d state:%d",lBHNo,bBH2) ;
	//CMSLogFileUtility::Instance()->BH_LogStatus(szLog);

	//4.52D17Move
	//Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
	//Z_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
	
//TakeTime(T1);//4.52D17Time  rpy1 Move outside

	szLog.Format("BH: PrUplook SrchDie #%d ...", lBHNo);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "AutoUpLookSearchDie_Rpy1", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bStatus);
			break;
		}
		else
		{
			Sleep(1);	
		}

	}

	//szLog.Format("Uplook --- Rpy1, Result state:%d", bStatus);
	//CMSLogFileUtility::Instance()->BH_LogStatus(szLog);

	if(!bStatus)
	{
		szLog = "BH Uplook Search Die Error: SrchDieCmd FAIL! Please Check!";
		SetErrorMessage(szLog);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		//HmiMessage_Red_Back(szLog, "Uplook Search Die"); //CSP006b
		return FALSE;
	}
	else
	{
		//v4.57A13	//Now do it in WPR command instead
/*
		//CMSLogFileUtility::Instance()->BH_LogStatus("Uplook --- Rpy1 is ok");
		PR_UBYTE ubSID = PSPR_SENDER_ID;
		PR_SRCH_DIE_RPY1	stSrchRpy1;
		unsigned short usSearchResult = 0;	//SearchDieRpy1(ubSID, &stSrchRpy1);

		PR_SrchDieRpy1(ubSID, &stSrchRpy1);

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
			CString szMsg;
			szMsg.Format("BH Uplook PR: Auto Get Reply1 error = %lu", usSearchResult);
			SetErrorMessage(szMsg);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);
			//SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, szMsg);
			return FALSE;
		}
*/
	}

	return bStatus;
}
//4.52D17auto Rpy2
BOOL CBondHead::UplookPrSearchDie_Rpy2(BOOL bBH2)
{
	CString szLog;
	//szLog.Format("Uplook ---Start Search Die Rpy2  with BH state:%d",bBH2) ;
	//CMSLogFileUtility::Instance()->BH_LogStatus(szLog);

	BOOL bStatus = FALSE;
	LONG lRefDieNo;
	LONG lBHNo;
	LONG lOffsetUplookX, lOffsetUplookY;
	LONG lSearchPRDieCentreX, lSearchPRDieCentreY;
	LONG lLearnPRDieCentreX, lLearnPRDieCentreY;
	DOUBLE dXResolution, dYResolution;
	LONG lConvertedValueX = 0;	
	LONG lConvertedValueY = 0;
	DOUBLE dXFileValue;
	DOUBLE dYFileValue;
	DOUBLE dCalibFactorBH1;
	DOUBLE dCalibFactorBH2;
	
	//DOUBLE dCalibFactorBH1 = 0.27;  // um/pixel from Peter 
	//DOUBLE dCalibFactorBH2 = 0.27;
	
	LONG lLearnUplookDieSizeX		= (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn GenDieSize X"];
	LONG lLearnUplookDieSizeY		= (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn GenDieSize Y"]; 

	//if( m_dBHZ1UplookCalibFactor == 0 || m_dBHZ2UplookCalibFactor == 0) //20161209 comment
	//{
	//	dCalibFactorBH1 = 0.13;  // um/pixel from David Ma  0.092,0.113, 0.15,0.13
	//	dCalibFactorBH2 = 0.13;
	//
	//}
	//else
	//{
		dCalibFactorBH1 = m_dBHZ1UplookCalibFactor;
		dCalibFactorBH2 = m_dBHZ2UplookCalibFactor;
	//}
		//if bBH2 = TRUE, then search die on Uplook PR2

	if(!bBH2)
	{
		lRefDieNo = MS899_UPLOOK_REF_DIENO_BH1;
		lBHNo	= 1;
	}
	else
	{
		lRefDieNo = MS899_UPLOOK_REF_DIENO_BH2;
		lBHNo	= 2;
	}

	szLog.Format("BH - UpLook PR 2 for BH %d", lBHNo);
	DisplaySequence(szLog);

	LONG lDieCX=0, lDieCY=0;
	DOUBLE dDieAngle = 0;

	bStatus = AutoUpLookSearchDie_Rpy2(lBHNo, lDieCX, lDieCY, dDieAngle);

	//szLog.Format("Uplook --- Search Die Rpy2and3 state:%d", bStatus);
	//CMSLogFileUtility::Instance()->BH_LogStatus(szLog);


	if (!bStatus)
	{
		(*m_psmfSRam)["BinTable"]["UplookCompX"] = 0;
		(*m_psmfSRam)["BinTable"]["UplookCompY"] = 0;
		(*m_psmfSRam)["BinTable"]["UplookCompT"] = 0;

		//for post bond offset
		if (!bBH2)
		{
			(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetX"] = 0;
			(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetY"] = 0;
		}
		else
		{
			(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetX"] = 0;
			(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetY"] = 0;
		}

		if (!m_bCheckMissingDie || IsBurnIn())
		{
			SetBTCompensate(TRUE);		
			bStatus = TRUE;
		}
		else
		{
CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: PrUplookResult fails");

			szLog = "BH Uplook Search Die Error: Rpy2 FAIL!";
			CMSLogFileUtility::Instance()->BH_LogStatus(szLog);
			SetErrorMessage(szLog);
			//HmiMessage_Red_Back(szLog, "Uplook Search Die"); //CSP006b
			return FALSE;
		}
	}
	else
	{
		LONG lUPX = (*m_psmfSRam)["BondHead"]["Uplook PR"]["Result"]["UPX"];
		LONG lUPY = (*m_psmfSRam)["BondHead"]["Uplook PR"]["Result"]["UPY"];
		LONG lLRX = (*m_psmfSRam)["BondHead"]["Uplook PR"]["Result"]["LRX"];
		LONG lLRY = (*m_psmfSRam)["BondHead"]["Uplook PR"]["Result"]["LRY"];

		if (!bBH2)
		{
			lSearchPRDieCentreX	= (*m_psmfSRam)["MS896A"]["UpLook BH1 Search PRDieCentre X"];
			lSearchPRDieCentreY	= (*m_psmfSRam)["MS896A"]["UpLook BH1 Search PRDieCentre Y"];

			lLearnPRDieCentreX  = (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre X"];
			lLearnPRDieCentreY  = (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre Y"];

			lOffsetUplookX	= (LONG)((lSearchPRDieCentreX - lLearnPRDieCentreX)*dCalibFactorBH1);
			lOffsetUplookY	= (LONG)((lSearchPRDieCentreY - lLearnPRDieCentreY)*dCalibFactorBH2);

			//lOffsetUplookX = 0 - lOffsetUplookX;
			//lOffsetUplookY = 0 - lOffsetUplookY;

			//for post bond offset
			(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetX"] = lOffsetUplookX;
			(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetY"] = lOffsetUplookY;

			szLog.Format("BH: PRUplookResult (BH1) - DieCenter(%d, %d), UL(%ld, %ld), LR(%ld, %ld), Angle = %.3f, LrnCenter(%d, %d); Factor(%.2f, %.2f), UplookXY(%ld, %ld)",
						lSearchPRDieCentreX, lSearchPRDieCentreY, 
						lUPX, lUPY, lLRX, lLRY,
						dDieAngle, 
						lLearnPRDieCentreX, lLearnPRDieCentreY, 
						dCalibFactorBH1, dCalibFactorBH2,
						lOffsetUplookX, lOffsetUplookY);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

		}
		else
		{
			lSearchPRDieCentreX	= (*m_psmfSRam)["MS896A"]["UpLook BH2 Search PRDieCentre X"];
			lSearchPRDieCentreY	= (*m_psmfSRam)["MS896A"]["UpLook BH2 Search PRDieCentre Y"];

			lLearnPRDieCentreX  = (*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre X"];
			lLearnPRDieCentreY  = (*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre Y"];

			lOffsetUplookX	= (LONG)((lSearchPRDieCentreX - lLearnPRDieCentreX)*dCalibFactorBH1);	//um
			lOffsetUplookY	= (LONG)((lSearchPRDieCentreY - lLearnPRDieCentreY)*dCalibFactorBH2);	//um

			//for post bond offset
			(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetX"] = lOffsetUplookX;
			(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetY"] = lOffsetUplookY;

			//andrewng12
			szLog.Format("BH: PRUplookResult (BH2) - DieCenter(%d, %d), UL(%ld, %ld), LR(%ld, %ld), Angle = %.3f, LrnCenter(%d, %d); Factor(%.2f, %.2f), UplookXY(%ld, %ld)",
						lSearchPRDieCentreX, lSearchPRDieCentreY, 
						lUPX, lUPY, lLRX, lLRY,						
						dDieAngle, 
						lLearnPRDieCentreX, lLearnPRDieCentreY, 
						dCalibFactorBH1, dCalibFactorBH2,
						lOffsetUplookX, lOffsetUplookY);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

		}

		//Convert Pixel -> um -> (encoder count)-> MotorStep 

		//	By CBinTable::ConvertFileUnitToXEncoderValue();

		dXFileValue = lOffsetUplookX;
		dYFileValue = lOffsetUplookY;

		dXResolution	= 1.0 / (GetChannelResolution(MS896A_CFG_CH_BINTABLE_X) * 1000);	
		dYResolution	= 1.0 / (GetChannelResolution(MS896A_CFG_CH_BINTABLE_Y) * 1000);

		if (dXFileValue > 0)
			lConvertedValueX = (LONG)((dXFileValue * dXResolution) + 0.5);
		else if (dXFileValue < 0)
			lConvertedValueX = (LONG)((dXFileValue * dXResolution) - 0.5);
		else 
			lConvertedValueX = 0;
		//lConvertedValueX = 0 - lConvertedValueX; //new added
		
		if (dYFileValue > 0)
			lConvertedValueY = (LONG)((dYFileValue * dYResolution) + 0.5);
		else if (dYFileValue < 0)
			lConvertedValueY = (LONG)((dYFileValue * dYResolution) - 0.5);
		else
			lConvertedValueY = 0;
		//lConvertedValueY = 0 - lConvertedValueY; //new added

		
		//For BT Uplook compensation motion in OpPerformUplookBTCompensation();
		(*m_psmfSRam)["BinTable"]["UplookCompX"] = lConvertedValueX;	
		(*m_psmfSRam)["BinTable"]["UplookCompY"] = lConvertedValueY;
		(*m_psmfSRam)["BinTable"]["UplookCompT"] = (DOUBLE) dDieAngle;
		
		SetBTCompensate(TRUE);		
	}

	szLog.Format(",BH%d,%d,%d,%d,%d,%d,%d,%f,%f,%d,%d,%f,%f,%d,%d,",lBHNo,lOffsetUplookX, lOffsetUplookY,lSearchPRDieCentreX, lSearchPRDieCentreY,lLearnPRDieCentreX, lLearnPRDieCentreY,dCalibFactorBH1,dCalibFactorBH2, lConvertedValueX,lConvertedValueY, dXResolution, dYResolution,lLearnUplookDieSizeX, lLearnUplookDieSizeY);
	CMSLogFileUtility::Instance()->Uplook_LogDieDataStatus(szLog);
	return bStatus;
}

BOOL CBondHead::AutoUpLookSearchDie_Rpy2(LONG lBH2, LONG& lDieCenterXInPixel, LONG lDieCenterYInPixel, DOUBLE& dDieAngle)
{
	LONG lBHNo = lBH2;

	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;	
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
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
		csMsgA.Format("Uplook Seacrch Die Error: Wrong BH%d",lBHNo);
		//SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
		SetErrorMessage(csMsgA);
		HmiMessage_Red_Back(csMsgA, "Uplook Search Die"); //CSP006b
		return FALSE;	
	}


	PR_WIN stSrchArea;

	stSrchArea.coCorner1.x = 0;
	stSrchArea.coCorner1.y = 0;
	stSrchArea.coCorner2.x = 0;
	stSrchArea.coCorner2.y = 0;

	BOOL bDieType			= WPR_REFERENCE_DIE;
	LONG lDieNo				= lSearchDieNo;
	PR_BOOLEAN bLatch		= PR_TRUE; 
	PR_BOOLEAN bAlign		= PR_TRUE; 
	PR_BOOLEAN bInspect		= PR_TRUE;
	PR_COORD stCorner1		= stSrchArea.coCorner1;
	PR_COORD stCorner2		= stSrchArea.coCorner2;
	BOOL bLog = FALSE;

//	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
//	CString				csMsgA;
	CString				csMsgB;
	PR_UWORD			usSearchResult;
    //PR_COORD			stPRCoorCentre;
    //PR_COORD			stPRDieSize;
	BOOL				bGetRpy3 = FALSE;
	//PR_GRAPHIC_INFO		emInfo;
	CString szLog;

	PR_UBYTE ubSID = PSPR_SENDER_ID;
	PR_UBYTE ubRID = PSPR_RECV_ID;

    PR_SrchDieRpy2(ubSID, &stSrchRpy2);
	if (stSrchRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
    {
		usSearchResult = PR_COMM_ERR;
	}
	else
	{
		usSearchResult = stSrchRpy2.stStatus.uwPRStatus;
	}

	if (usSearchResult == PR_COMM_ERR)
	{
		csMsgA.Format("Uplook Search Die Error: Auto Get Reply 2 = %x",usSearchResult);
		//SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
		SetErrorMessage(csMsgA);
		HmiMessage_Red_Back(csMsgA, "Uplook Search Die"); //CSP006b
		return FALSE;	
	}

    usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
	{
        usDieType = PR_ERR_GOOD_DIE;
    }

	UpLookExtractDieResult(stSrchRpy2, stSrchRpy3, FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore);

	if ((DieIsAlignable(usDieType) == TRUE) && 
		(bInspect == PR_TRUE))					//v4.57A12
	{
		bGetRpy3 = TRUE;
		//usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
		PR_SrchDieRpy3(ubSID, &stSrchRpy3);
		if (stSrchRpy3.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			usSearchResult = PR_COMM_ERR;
		}
		else
		{
			usSearchResult = stSrchRpy3.stStatus.uwPRStatus;
		}

		if (usSearchResult != PR_ERR_NOERR) //== PR_COMM_ERR)
		{
			csMsgA.Format("Uplook Search Die Error: Reply 3 = 0x%x", usSearchResult);
			//SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
CMSLogFileUtility::Instance()->BT_TableIndexLog(csMsgA);
			SetErrorMessage(csMsgA);
			//HmiMessage_Red_Back(csMsgA, "Uplook Search Die"); //CSP006b
			return FALSE;	
		}
	}
	else
	{
		csMsgA = "Uplook Search Die Error: Rpy2 Die is NOT Alignable!";

		CString szLog;
		szLog.Format("BH: UpLook PR RPY2 fails; errorcode = %lu, DieType = %lu", usSearchResult, usDieType);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

		//SetErrorMessage(csMsgA);
		//HmiMessage_Red_Back(csMsgA, "Uplook Search Die"); //CSP006b
		return FALSE;	
	}

//TakeTime(LC2); //4.52D17Time rpy2 (4um) done


	//4.52D17cal search die centre reply2
	CString szTempMsg; 
	LONG lPRDieCentreX  = stSrchRpy2.stDieAlign[0].stGen.coDieCentre.x;
	LONG lPRDieCentreY  = stSrchRpy2.stDieAlign[0].stGen.coDieCentre.y;

	//v4.59A1
	LONG lUPX = _round(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].x);
	LONG lUPY = _round(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].y);
	LONG lLRX = _round(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[2].x);
	LONG lLRY = _round(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[2].y);
	(*m_psmfSRam)["BondHead"]["Uplook PR"]["Result"]["UPX"] = lUPX;
	(*m_psmfSRam)["BondHead"]["Uplook PR"]["Result"]["UPY"] = lUPY;
	(*m_psmfSRam)["BondHead"]["Uplook PR"]["Result"]["LRX"] = lLRX;
	(*m_psmfSRam)["BondHead"]["Uplook PR"]["Result"]["LRY"] = lLRY;


	if(lSearchDieNo == MS899_UPLOOK_REF_DIENO_BH1)
	{			
		(*m_psmfSRam)["MS896A"]["UpLook BH1 Search PRDieCentre X"]		= lPRDieCentreX;
		(*m_psmfSRam)["MS896A"]["UpLook BH1 Search PRDieCentre Y"]		= lPRDieCentreY;

		lDieCenterXInPixel	= lPRDieCentreX;
		lDieCenterYInPixel	= lPRDieCentreY;
		dDieAngle			= (DOUBLE) stSrchRpy2.stDieAlign[0].stGen.rDieRot;
		//szTempMsg.Format("Uplook --- Rpy2Auto Search Die, BH1 PRDieCentre(%d,%d) ", lPRDieCentreX,lPRDieCentreY);

	}
	else if(lSearchDieNo == MS899_UPLOOK_REF_DIENO_BH2)
	{
		(*m_psmfSRam)["MS896A"]["UpLook BH2 Search PRDieCentre X"]		= lPRDieCentreX;
		(*m_psmfSRam)["MS896A"]["UpLook BH2 Search PRDieCentre Y"]		= lPRDieCentreY;
		
		lDieCenterXInPixel	= lPRDieCentreX;
		lDieCenterYInPixel	= lPRDieCentreY;
		dDieAngle			= (DOUBLE) stSrchRpy2.stDieAlign[0].stGen.rDieRot;		
		//szTempMsg.Format("Uplook --- Auto Search Die, BH2 PRDieCentre(%d,%d) ", lPRDieCentreX,lPRDieCentreY);
	}
	else
	{	
		lDieCenterXInPixel	= 0;
		lDieCenterYInPixel	= 0;
		dDieAngle			= 0;		

		szTempMsg.Format("Uplook Search Die Error: Rpy2 Auto Search Die Fail with SearchDieNo %d", lSearchDieNo);
		SetErrorMessage(szTempMsg);
		HmiMessage_Red_Back(szTempMsg, "Uplook Search Die"); //CSP006b
		return FALSE;
	}

	return TRUE;
}

PR_UWORD CBondHead::UpLookExtractDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore) 
{
	switch (stSrchRpy2.stStatus.uwPRStatus)
	{
		case PR_ERR_NOERR:
		case PR_WARN_SMALL_SRCH_WIN:
		case PR_WARN_TOO_MANY_DIE:
		case PR_WARN_BACKUP_ALIGNED:
			if (bUseReply3 == TRUE)
			{
				*usDieType		= stSrchRpy3.stStatus.uwPRStatus;
			}
			else
			{
				*usDieType		= stSrchRpy2.stStatus.uwPRStatus;
			}
			*fDieRotate		= stSrchRpy2.stDieAlign[0].stGen.rDieRot;
			*stDieOffset	= stSrchRpy2.stDieAlign[0].stGen.coDieCentre;
			*fDieScore		= stSrchRpy2.stDieAlign[0].stGen.rMatchScore;

			if (*usDieType == PR_ERR_NOERR)
			{
				*usDieType	= PR_ERR_GOOD_DIE;
			}
			break;

		case PR_ERR_LOCATED_DEFECTIVE_DIE:
		case PR_ERR_ROTATION:
			*usDieType		= stSrchRpy2.stStatus.uwPRStatus;
			*fDieRotate		= stSrchRpy2.stDieAlign[0].stGen.rDieRot;
			*stDieOffset	= stSrchRpy2.stDieAlign[0].stGen.coDieCentre;
			*fDieScore		= stSrchRpy2.stDieAlign[0].stGen.rMatchScore;
			break;

		case PR_ERR_DIE_SIZE_RJT:
		case PR_ERR_DEFECTIVE_DIE:
		case PR_ERR_CORNER_ANGLE_FAIL:
		case PR_ERR_NO_DIE:
		case PR_ERR_ROTATION_EXCEED_SPEC:
		case PR_ERR_NON_LOCATED_DEFECTIVE_DIE:
		case PR_ERR_NO_SYS_MEM:
		case PR_ERR_FAIL_CONTRAST:
		case PR_ERR_PID_NOT_LD:
		default:
			*usDieType		= stSrchRpy2.stStatus.uwPRStatus;
			if (*usDieType == PR_ERR_DEFECTIVE_DIE)
			{
				*usDieType = PR_ERR_NON_LOCATED_DEFECTIVE_DIE;
			}

			*fDieRotate		= 0.0;
			stDieOffset->x	= (PR_WORD) PR_DEF_CENTRE_X;
			stDieOffset->y	= (PR_WORD) PR_DEF_CENTRE_Y;
			break;
	}

	return 0;
}


BOOL CBondHead::DieIsAlignable(PR_UWORD usDieType)
{
	if ((usDieType == PR_ERR_GOOD_DIE)						||
		(usDieType == PR_ERR_INK_DIE)						||
        (usDieType == PR_ERR_CHIP_DIE)						||
        (usDieType == PR_ERR_BOND_PAD_SIZE_RJT)				||
        (usDieType == PR_ERR_BOND_PAD_FOREIGN_MAT)			||
        (usDieType == PR_ERR_LIGHT_EMITTING_FOREIGN_MAT)	||
        (usDieType == PR_ERR_DEFECTIVE_DIE)					||
        (usDieType == PR_ERR_EXCEED_INTENSITY_VAR)			||
        (usDieType == PR_WARN_SMALL_SRCH_WIN)				||
        (usDieType == PR_WARN_TOO_MANY_DIE)					||
        (usDieType == PR_WARN_BACKUP_ALIGNED)				||
        (usDieType == PR_ERR_NOERR)							||
        (usDieType == PR_ERR_INSUFF_EPOXY_COVERAGE)			||
		PR_WARN_STATUS(usDieType))
	{
        return TRUE;
    }
    return FALSE;
}

PR_UWORD CBondHead::SearchUpLookDieCmd(BOOL bBHZ2)
{
    PR_2_POINTS_DIE_ALIGN_CMD       st2PointsAlignCmd;
    PR_QUAD_DIE_ALIGN_CMD           stQuadAlignCmd;
    PR_STREET_DIE_ALIGN_CMD         stStreetAlignCmd;
    PR_DIE_ALIGN_PAR                stAlignPar;

    PR_TMPL_DIE_INSP_CMD            stTmplInspCmd;
    PR_LED_DIE_INSP_CMD             stLedInspCmd;

    PR_DIE_ALIGN_CMD                stDieAlignCmd;
	PR_DIE_INSP_CMD 				stDieInspCmd;
	PR_POST_INSP_CMD 			    stBadCutInspCmd;
    PR_POST_INDIV_INSP_CMD			stBadCutCmd;


	PR_SRCH_DIE_CMD                 stSrchCmd; 

	PR_COORD						stCoDieCenter;
	PR_UWORD						lPrSrchID;
	PR_UWORD                        uwCommunStatus;
	PR_BOOLEAN						bBackupAlign = PR_FALSE;
	PR_BOOLEAN						bCheckDefect = PR_TRUE;
	PR_BOOLEAN						bCheckChip = PR_TRUE;
	PR_UBYTE						ucDefectThres = PR_NORMAL_DEFECT_THRESHOLD;
	LONG							lAlignAccuracy;
	LONG							lGreyLevelDefect;
	LONG							lDieNo = 0;
	DOUBLE							dMinChipArea = 0.0;
	DOUBLE							dSingleDefectArea = 0.0;
	DOUBLE							dTotalDefectArea = 0.0;
	PR_CAMERA						emCamID = MS899_POSTSEAL_2D_BH1_CAM_ID;

	PR_UBYTE ubSID			= PSPR_SENDER_ID;
	PR_UBYTE ubRID			= PSPR_RECV_ID;
	PR_BOOLEAN bLatch		= PR_TRUE;
	PR_BOOLEAN bAlign		= PR_TRUE;
	PR_BOOLEAN bInspect		= PR_FALSE;
	PR_GRAPHIC_INFO emInfo	= PR_NO_DISPLAY;

	PR_COORD stDieULC, stDieLRC;

    PR_Init2PointsDieAlignCmd(&st2PointsAlignCmd);
    PR_InitQuadDieAlignCmd(&stQuadAlignCmd);
    PR_InitStreetDieAlignCmd(&stStreetAlignCmd);
    PR_InitTmplDieInspCmd(&stTmplInspCmd);
    PR_InitLedDieInspCmd(&stLedInspCmd);
    PR_InitDieAlignCmd(&stDieAlignCmd);
    PR_InitDieInspCmd(&stDieInspCmd);
    PR_InitPostInspCmd(&stBadCutInspCmd);
    PR_InitPostIndivInspCmd(&stBadCutCmd);
	PR_POST_INDIV_INSP_EXT1_CMD	stBadCutExtCmd = stBadCutInspCmd.stEpoxy.stIndivExt1;
	PR_InitDieAlignPar(&stAlignPar);
    PR_InitSrchDieCmd(&stSrchCmd); 


	if (bBHZ2)
	{
		lDieNo		= 2 + MS899_UPLOOK_REF_DIENO_BH2;
		emCamID		= MS899_POSTSEAL_2D_BH2_CAM_ID;
		
		lPrSrchID	= (PR_UWORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH2"]["RecordID"];
		stDieULC.x	= (PR_WORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH2"]["SearchArea"]["Corner1"]["x"];
		stDieULC.y	= (PR_WORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH2"]["SearchArea"]["Corner1"]["y"];
		stDieLRC.x	= (PR_WORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH2"]["SearchArea"]["Corner2"]["x"];
		stDieLRC.y	= (PR_WORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH2"]["SearchArea"]["Corner2"]["y"];
	}
	else
	{
		lDieNo		= 2 + MS899_UPLOOK_REF_DIENO_BH1;
		emCamID		= MS899_POSTSEAL_2D_BH1_CAM_ID;

		lPrSrchID	= (PR_UWORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH1"]["RecordID"];
		stDieULC.x	= (PR_WORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH1"]["SearchArea"]["Corner1"]["x"];
		stDieULC.y	= (PR_WORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH1"]["SearchArea"]["Corner1"]["y"];
		stDieLRC.x	= (PR_WORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH1"]["SearchArea"]["Corner2"]["x"];
		stDieLRC.y	= (PR_WORD)(LONG)(*m_psmfSRam)["PostSealPr"]["BH1"]["SearchArea"]["Corner2"]["y"];
	}


	lAlignAccuracy		= PR_HIGH_DIE_ALIGN_ACCURACY;	
	lGreyLevelDefect	= PR_DEFECT_ATTRIBUTE_BOTH;
	ucDefectThres		= (PR_UBYTE) 12;
	bCheckDefect		= PR_FALSE;
	bCheckChip			= PR_FALSE;
	bBackupAlign		= PR_FALSE;


	//Calculate chip die area, min & total defect area
	//dMinChipArea		= ((DOUBLE)(GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) / 100) * m_dGenSrchChipArea[lDieNo];
	//dSingleDefectArea	= ((DOUBLE)(GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) / 100) * m_dGenSrchSingleDefectArea[lDieNo];  
	//dTotalDefectArea	= ((DOUBLE)(GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) / 100) * m_dGenSrchTotalDefectArea[lDieNo]; 

	stCoDieCenter.x = (stDieULC.x + stDieLRC.x) / 2;
	stCoDieCenter.y = (stDieULC.y + stDieLRC.y) / 2;

	//Setup Search Die parameter
	st2PointsAlignCmd.emConsistentDiePos		= PR_TRUE;
	st2PointsAlignCmd.emBackupTmpl				= PR_FALSE;      
	st2PointsAlignCmd.emSingleTmplSrch			= PR_FALSE;
	st2PointsAlignCmd.emSingleDieSrch			= PR_TRUE;
	st2PointsAlignCmd.emPartialDieSrch			= PR_FALSE;     
	st2PointsAlignCmd.emDieRotChk				= PR_TRUE;
	st2PointsAlignCmd.emMaxDieSizeChk			= PR_FALSE;
	st2PointsAlignCmd.emMinDieSizeChk			= PR_FALSE;
	st2PointsAlignCmd.emDieEdgeChk				= PR_TRUE;
	st2PointsAlignCmd.emPositionChk				= PR_FALSE;
	st2PointsAlignCmd.coRefPoint.x				= 0;
	st2PointsAlignCmd.coRefPoint.y				= 0;
	st2PointsAlignCmd.rRefAngle					= 0;
	st2PointsAlignCmd.coMaxShift.x				= 0;
	st2PointsAlignCmd.coMaxShift.y				= 0;
	st2PointsAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	st2PointsAlignCmd.szPosConsistency.x		= PR_DEF_POS_CONSISTENCY_X + 10 * PR_SCALE_FACTOR;
	st2PointsAlignCmd.szPosConsistency.y		= PR_DEF_POS_CONSISTENCY_Y + 10 * PR_SCALE_FACTOR;
	st2PointsAlignCmd.stLargeSrchWin.coCorner1	= stDieULC;
	st2PointsAlignCmd.stLargeSrchWin.coCorner2	= stDieLRC;
	//VerifyPRPoint(&stCoDieCenter);
	st2PointsAlignCmd.coSelectPoint				= stCoDieCenter;
	st2PointsAlignCmd.coProbableDieCentre		= stCoDieCenter;
	st2PointsAlignCmd.rDieRotTol				= PR_SRCH_ROT_TOL;

	stStreetAlignCmd.emConsistentDiePos			= PR_TRUE;
	stStreetAlignCmd.emSingleDieSrch			= PR_TRUE;
	stStreetAlignCmd.emPartialDieSrch			= PR_FALSE;
	stStreetAlignCmd.emDieRotChk				= PR_TRUE;
	stStreetAlignCmd.emMaxDieSizeChk			= PR_FALSE;
	stStreetAlignCmd.emMinDieSizeChk			= PR_FALSE;
	stStreetAlignCmd.emDieEdgeChk				= PR_TRUE;
	stStreetAlignCmd.emPositionChk				= PR_FALSE;
	stStreetAlignCmd.coRefPoint.x				= 0;
	stStreetAlignCmd.coRefPoint.y				= 0;
	stStreetAlignCmd.coMaxShift.x				= 0;
	stStreetAlignCmd.coMaxShift.y				= 0;
	stStreetAlignCmd.rRefAngle					= 0;
	stStreetAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	stStreetAlignCmd.szPosConsistency.x			= PR_DEF_POS_CONSISTENCY_X + 10 * PR_SCALE_FACTOR;
	stStreetAlignCmd.szPosConsistency.y			= PR_DEF_POS_CONSISTENCY_Y + 10 * PR_SCALE_FACTOR;
	stStreetAlignCmd.stLargeSrchWin.coCorner1	= stDieULC;
	stStreetAlignCmd.stLargeSrchWin.coCorner2	= stDieLRC;
	//VerifyPRPoint(&stCoDieCenter);
	stStreetAlignCmd.coSelectPoint				= stCoDieCenter;
	stStreetAlignCmd.coProbableDieCentre		= stCoDieCenter;
	stStreetAlignCmd.rDieRotTol					= PR_SRCH_ROT_TOL;

	stTmplInspCmd.emChipDieChk					= bCheckChip;
	stTmplInspCmd.emGeneralDefectChk			= bCheckDefect;
	stTmplInspCmd.emInkOnlyChk					= PR_FALSE;     
	stTmplInspCmd.emInspIncompleteDie			= PR_FALSE;     
	stTmplInspCmd.aeMinSingleDefectArea			= (PR_AREA)dSingleDefectArea;
	stTmplInspCmd.aeMinTotalDefectArea			= (PR_AREA)dTotalDefectArea;  
	stTmplInspCmd.aeMinChipArea					= (PR_AREA)dMinChipArea; 
	stTmplInspCmd.emNewDefectCriteria			= PR_TRUE;
	stTmplInspCmd.ubIntensityVariation			= PR_DEF_INTENSITY_VARIATION;
	stTmplInspCmd.ubDefectThreshold				= ucDefectThres; 
	stTmplInspCmd.rMinInkSize					= PR_FALSE;     
	stTmplInspCmd.emDefectAttribute				= (PR_DEFECT_ATTRIBUTE)(lGreyLevelDefect);
	stDieAlignCmd.st2Points						= st2PointsAlignCmd;
	stDieAlignCmd.stStreet						= stStreetAlignCmd;
	
	stDieInspCmd.stTmpl							= stTmplInspCmd;

	stAlignPar.emEnableBackupAlign				= bBackupAlign;
	stAlignPar.rStartAngle						= PR_SRCH_START_ANGLE;
	stAlignPar.rEndAngle						= PR_SRCH_END_ANGLE;		

	//Update the seach die command
	stSrchCmd.emAlignAlg						= PR_EDGE_POINTS_MATCHING_ON_DIE_EDGES;
	stSrchCmd.emDieInspAlg						= PR_DIE_INSP_ALG_BIN;

	//Setup badcut parameter
	//if ( GetDieShape() == WPR_RECTANGLE_DIE )
	//{
	stBadCutCmd.emEpoxyInsuffChk				= PR_FALSE;
	stBadCutCmd.emEpoxyExcessChk				= PR_FALSE;
	stBadCutCmd.uwMaxInsuffSide					= 0;
	stBadCutInspCmd.emEpoxyChk					= PR_FALSE;
	/*
	stBadCutCmd.stMinPercent.x					= 0;
	stBadCutCmd.stMinPercent.y					= 0;
	stBadCutCmd.szScanWidth.x					= 0;
	stBadCutCmd.szScanWidth.y					= 0;
	stBadCutCmd.stEpoxyInsuffWin.coCorner1		= 0;
	stBadCutCmd.stEpoxyInsuffWin.coCorner2		= 0;
	stBadCutCmd.stEpoxyInsuffWin.coObjCentre.x	= PR_DEF_CENTRE_X;
	stBadCutCmd.stEpoxyInsuffWin.coObjCentre.y	= PR_DEF_CENTRE_Y;
	stBadCutInspCmd.emEpoxyAlg					= PR_INDIV_INSP;
	stBadCutInspCmd.stEpoxy.stIndiv				= stBadCutCmd;
	*/
	//}
	stSrchCmd.stDieAlignPar.rStartAngle				= PR_SRCH_START_ANGLE;
	stSrchCmd.stDieAlignPar.rEndAngle				= PR_SRCH_END_ANGLE;
	stSrchCmd.emRetainBuffer						= bLatch;
	stSrchCmd.emLatch								= bLatch;
	stSrchCmd.emAlign								= bAlign;
	stSrchCmd.emDefectInsp							= bInspect;
	stSrchCmd.emCameraNo							= emCamID;
    stSrchCmd.emVideoSource							= PR_IMAGE_BUFFER_A;
    stSrchCmd.uwNRecordID							= 1;                  
    stSrchCmd.auwRecordID[0]						= (PR_UWORD)(lPrSrchID);       
    stSrchCmd.coProbableDieCentre					= stCoDieCenter;
    stSrchCmd.ulRpyControlCode						= PR_DEF_SRCH_DIE_RPY;
    stSrchCmd.stDieAlign							= stDieAlignCmd;
    stSrchCmd.stDieAlignPar							= stAlignPar;
    stSrchCmd.stDieInsp								= stDieInspCmd;
    stSrchCmd.stDieAlignPar.rMatchScore				= 80.0;
	stSrchCmd.stDieAlignPar.emIsDefaultMatchScore	= PR_FALSE;
    stSrchCmd.emGraphicInfo							= PR_NO_DISPLAY; 
	stSrchCmd.stPostBondInsp						= stBadCutInspCmd;
	stSrchCmd.emPostBondInsp						= PR_FALSE;  


	//No need to do badcut if this die is not normal die
	//m_bGetBadCutReply = FALSE;
	//stSrchCmd.emPostBondInsp = PR_FALSE;  

	PR_SrchDieCmd(&stSrchCmd, ubSID, ubRID, &uwCommunStatus);
	if (uwCommunStatus != PR_COMM_NOERR)
	{
		return PR_COMM_ERR;
	}

	return PR_ERR_NOERR;
}


BOOL CBondHead::MoveWaferTableNoCheck(LONG lXAxis, LONG lYAxis)
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

	stPos2.lUnload = 0;		//to LOAD
	
	if (CMS896AApp::m_bMS100Plus9InchOption)		//v4.16T3	//MS100 9Inch
	{
		stMsg.InitMessage(sizeof(RELPOS2), &stPos2);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_MoveToUnloadCmd",	stMsg);
	}
	else
	{
		stMsg.InitMessage(sizeof(RELPOS), &stPos);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_MoveToCmd",			stMsg);
	}

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


BOOL CBondHead::IsBT1FrameLevel()
{
	CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
	if (pBinLoader)
	{
		return pBinLoader->IsFrameLevel();
	}
	return FALSE;
}

BOOL CBondHead::IsBT2FrameLevel()
{
	CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
	if (pBinLoader)
	{
		return pBinLoader->IsFrameLevel2();
	}
	return FALSE;
}


LONG CBondHead::GetColletHoleEnc_T()
{
	return m_lColletHoleEnc_T;
}

LONG CBondHead::GetColletHoleEnc_Z()
{
	return m_lColletHoleEnc_Z;
}

LONG CBondHead::GetColletHoleEnc_Z2()
{
	return m_lColletHoleEnc_Z2;
}


VOID CBondHead::EjectorMoveToStandBy()
{
	Ej_MoveTo(m_lStandbyLevel_Ej);
	Sleep(200);
}


BOOL CBondHead::CleanDirtEjectorPin(const LONG lCleanDirtPinUpLevel, const LONG lCycleCount)
{
	LONG lCount = 0;

	for (LONG i = 0; i < lCycleCount; i++)
	{
		if (Ej_MoveTo(m_lEjectLevel_Ej + lCleanDirtPinUpLevel) != gnOK)
		{
			Ej_Home();
			return FALSE;
		}
		Sleep(200);
		Ej_MoveTo(m_lStandbyLevel_Ej);
		Sleep(200);
	}

	return TRUE;
}


BOOL CBondHead::LoadPackageMsdBHData(VOID)
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

	// Bond Head Delay Setting
	m_lPickDelay				= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_PICK_DELAY];
	m_lBondDelay				= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_BOND_DELAY];
	m_lArmPickDelay				= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_ARM_PICK_DELAY];
	m_lArmBondDelay				= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_ARM_BOND_DELAY];
	m_lHeadPickDelay			= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_HEAD_PICK_DELAY];
	m_lHeadBondDelay			= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_HEAD_BOND_DELAY];
	m_lHeadPrePickDelay			= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_HEAD_PREPICK_DELAY];
	m_lPRDelay					= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_PR_DELAY];
	m_lEjectorUpDelay			= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_EJECTOR_UP_DELAY];
	m_lEjectorDownDelay			= (*psmf)[BH_DATA][BH_DELAY_SETTING][BH_EJECTOR_DOWN_DELAY];

	// Bond Head Picking Process
	m_bEnableSyncMotionViaEjCmd = (BOOL)(LONG)(*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_ENABLE_SYNC_PICK];
	m_lSyncTriggerValue			= (*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_TRIGGER_LEVEL];
	m_lSyncZOffset				= (*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_PRELOAD_LEVEL];
	m_lPickDriveIn				= (*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_PICK_LEVEL_DRIVE_IN];
	m_lBondDriveIn				= (*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_BOND_LEVEL_DRIVE_IN];
	m_szEjSoftTouch				= (*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_EJECTOR_SPEED];
	//m_bUseMultiProbeLevel		= (BOOL)(LONG)(*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_ENABLE_BT_LEVELING];	//Default value to TRUE by Matthew 20190410
	m_bAutoCleanAutoLearnZLevels = (BOOL)(LONG)(*psmf)[BH_DATA][BH_PICKING_PROCESS]["Enable Auto Clean Auto Learn Z Levels"];

	// Alarm Setting
	m_bCheckMissingDie			= (BOOL)(LONG)(*psmf)[BH_DATA][BH_ALARM_SETTING][BH_ENABLE_MD_ALARM];
	m_lMissingDie_Retry			= (*psmf)[BH_DATA][BH_ALARM_SETTING][BH_MD_RETRY];
	m_bCheckColletJam			= (BOOL)(LONG)(*psmf)[BH_DATA][BH_ALARM_SETTING][BH_ENABLE_CJ_ALARM];
	m_ulColletJam_Retry			= (*psmf)[BH_DATA][BH_ALARM_SETTING][BH_CJ_RETRY];
	m_lAutoCleanAirBlowLimit	= (*psmf)[BH_DATA][BH_ALARM_SETTING]["Auto Clean Air Blow Limit"];
	
	m_ulMaxColletCount			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET];
	m_ulMaxCleanCount			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXCLEAN];
	m_ulMaxCollet2Count			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET2];
	m_ulMaxEjectorCount			= (*psmf)[BP_DATA][BP_COUNT][BP_MAXEJECTOR];

	m_bResetEjKOffsetAtWaferEnd	= (BOOL)(LONG)(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_RESET_WAFEREND];
	m_lEjectorKOffset			= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET];
	m_lEjectorKCount			= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KCOUNT];
	m_lEjectorKOffsetLimit		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_LIMIT];
	m_lEjectorKOffsetForBHZ1MD	= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_BHZ1_MD];
	m_lEjectorKOffsetForBHZ2MD	= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_BHZ2_MD];
	m_lEjSubRegionKOffset		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_KOFFSET];
	m_lEjSubRegionKCount		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_KCOUNT];
	m_lEjSubRegionSKOffset		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_SKOFFSET];
	m_lEjSubRegionSKCount		= (*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_SKCOUNT];
	m_lEjectorKOffsetResetCount = (*psmf)[BP_DATA][BP_COUNT]["Ejector K Offset Reset Count"];//M69
	m_bCheckK1K2				= (BOOL)(LONG)(*psmf)[BP_DATA][BP_COUNT]["Reset K1K2"]; //M70

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to load BH data from PackageData.msd");

	// close config file
    pUtl->ClosePackageDataConfig();

	return TRUE;
}


BOOL CBondHead::SavePackageMsdBHData()
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

	// Bond Head Delay Setting
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_PICK_DELAY]				= m_lPickDelay;
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_BOND_DELAY]				= m_lBondDelay;
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_ARM_PICK_DELAY]			= m_lArmPickDelay;
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_ARM_BOND_DELAY]			= m_lArmBondDelay;
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_HEAD_PICK_DELAY]			= m_lHeadPickDelay;
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_HEAD_BOND_DELAY]			= m_lHeadBondDelay;
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_HEAD_PREPICK_DELAY]		= m_lHeadPrePickDelay;
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_PR_DELAY]					= m_lPRDelay;
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_EJECTOR_UP_DELAY]			= m_lEjectorUpDelay;
	(*psmf)[BH_DATA][BH_DELAY_SETTING][BH_EJECTOR_DOWN_DELAY]		= m_lEjectorDownDelay;

	// Bond Head Picking Process
	(*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_ENABLE_SYNC_PICK]		= m_bEnableSyncMotionViaEjCmd;
	(*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_TRIGGER_LEVEL]			= m_lSyncTriggerValue;
	(*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_PRELOAD_LEVEL]			= m_lSyncZOffset;
	(*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_PICK_LEVEL_DRIVE_IN]	= m_lPickDriveIn;
	(*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_BOND_LEVEL_DRIVE_IN]	= m_lBondDriveIn;
	(*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_EJECTOR_SPEED]			= m_szEjSoftTouch;
	//(*psmf)[BH_DATA][BH_PICKING_PROCESS][BH_ENABLE_BT_LEVELING]	= m_bUseMultiProbeLevel;	//Default value to TRUE by Matthew 20190410
	(*psmf)[BH_DATA][BH_PICKING_PROCESS]["Enable Auto Clean Auto Learn Z Levels"] = m_bAutoCleanAutoLearnZLevels;

	// Alarm Setting
	(*psmf)[BH_DATA][BH_ALARM_SETTING][BH_ENABLE_MD_ALARM]			= m_bCheckMissingDie;
	(*psmf)[BH_DATA][BH_ALARM_SETTING][BH_MD_RETRY]					= m_lMissingDie_Retry;
	(*psmf)[BH_DATA][BH_ALARM_SETTING][BH_ENABLE_CJ_ALARM]			= m_bCheckColletJam;
	(*psmf)[BH_DATA][BH_ALARM_SETTING][BH_CJ_RETRY]					= m_ulColletJam_Retry;
	(*psmf)[BH_DATA][BH_ALARM_SETTING]["Auto Clean Air Blow Limit"]	= m_lAutoCleanAirBlowLimit;

	(*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET]						= m_ulMaxColletCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_MAXCLEAN]							= m_ulMaxCleanCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_MAXCOLLET2]						= m_ulMaxCollet2Count;
	(*psmf)[BP_DATA][BP_COUNT][BP_MAXEJECTOR]						= m_ulMaxEjectorCount;

	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_RESET_WAFEREND] = m_bResetEjKOffsetAtWaferEnd;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET] = m_lEjectorKOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KCOUNT] = m_lEjectorKCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_LIMIT] = m_lEjectorKOffsetLimit;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_BHZ1_MD] = m_lEjectorKOffsetForBHZ1MD;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_KOFFSET_BHZ2_MD] = m_lEjectorKOffsetForBHZ2MD;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_KOFFSET] = m_lEjSubRegionKOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_KCOUNT] = m_lEjSubRegionKCount;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_SKOFFSET] = m_lEjSubRegionSKOffset;
	(*psmf)[BP_DATA][BP_COUNT][BP_EJ_SUBREGION_SKCOUNT] = m_lEjSubRegionSKCount;
	(*psmf)[BP_DATA][BP_COUNT]["Ejector K Offset Reset Count"] = m_lEjectorKOffsetResetCount;//M69
	(*psmf)[BP_DATA][BP_COUNT]["Reset K1K2"] = m_bCheckK1K2; //M70

	pUtl->UpdatePackageDataConfig();	
	// close config file
    pUtl->ClosePackageDataConfig();

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to save BH data from PackageData.msd");
	
	return TRUE;
}


BOOL CBondHead::UpdateBHPackageList(VOID)
{
	CStdioFile fTemp;
	CString szContent, szText, szTemp;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\BHPackageList.csv"), CFile::modeCreate| CFile::modeReadWrite))
	{
		return FALSE;
	}
	
	// Bond Head Delay Setting
	szText = BH_PICK_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lPickDelay);
	szContent = szContent + szText + szTemp;

	szText = BH_BOND_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lBondDelay);
	szContent = szContent + szText + szTemp;

	szText = BH_ARM_PICK_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lArmPickDelay);
	szContent = szContent + szText + szTemp;

	szText = BH_ARM_BOND_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lArmBondDelay);
	szContent = szContent + szText + szTemp;

	szText = BH_HEAD_PICK_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lHeadPickDelay);
	szContent = szContent + szText + szTemp;

	szText = BH_HEAD_BOND_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lHeadBondDelay);
	szContent = szContent + szText + szTemp;

	szText = BH_HEAD_PREPICK_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lHeadPrePickDelay);
	szContent = szContent + szText + szTemp;

	szText = BH_PR_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lPRDelay);
	szContent = szContent + szText + szTemp;

	szText = BH_EJECTOR_UP_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lEjectorUpDelay);
	szContent = szContent + szText + szTemp;

	szText = BH_EJECTOR_DOWN_DELAY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lEjectorDownDelay);
	szContent = szContent + szText + szTemp;

	// Bond Head Picking Process
	szText = BH_ENABLE_SYNC_PICK;
	szText = szText + ",";
	szTemp.Format("%d,", m_bEnableSyncMotionViaEjCmd);
	szContent = szContent + szText + szTemp;

	szText = BH_TRIGGER_LEVEL;
	szText = szText + ",";
	szTemp.Format("%d,", m_lSyncTriggerValue);
	szContent = szContent + szText + szTemp;

	szText = BH_PRELOAD_LEVEL;
	szText = szText + ",";
	szTemp.Format("%d,", m_lSyncZOffset);
	szContent = szContent + szText + szTemp;

	szText = BH_PICK_LEVEL_DRIVE_IN;
	szText = szText + ",";
	szTemp.Format("%d,", m_lPickDriveIn);
	szContent = szContent + szText + szTemp;

	szText = BH_BOND_LEVEL_DRIVE_IN;
	szText = szText + ",";
	szTemp.Format("%d,", m_lBondDriveIn);
	szContent = szContent + szText + szTemp;

	szText = BH_EJECTOR_SPEED;
	szText = szText + ",";
	szContent = szContent + szText + m_szEjSoftTouch + ",";

	szText = BH_ENABLE_BT_LEVELING;
	szText = szText + ",";
	szTemp.Format("%d,", m_bUseMultiProbeLevel);
	szContent = szContent + szText + szTemp;

	szText = "Enable Auto Clean Auto Learn Z Levels";
	szText = szText + ",";
	szTemp.Format("%d,", m_bAutoCleanAutoLearnZLevels);
	szContent = szContent + szText + szTemp;

	// Alarm Setting
	szText = BH_ENABLE_MD_ALARM;
	szText = szText + ",";
	szTemp.Format("%d,", m_bCheckMissingDie);
	szContent = szContent + szText + szTemp;

	szText = BH_MD_RETRY;
	szText = szText + ",";
	szTemp.Format("%d,", m_lMissingDie_Retry);
	szContent = szContent + szText + szTemp;

	szText = BH_ENABLE_CJ_ALARM;
	szText = szText + ",";
	szTemp.Format("%d,", m_bCheckColletJam);
	szContent = szContent + szText + szTemp;

	szText = BH_CJ_RETRY;
	szText = szText + ",";
	szTemp.Format("%d,", m_ulColletJam_Retry);
	szContent = szContent + szText + szTemp;

	szText = "Auto Clean Air Blow Limit";
	szText = szText + ",";
	szTemp.Format("%d,", m_lAutoCleanAirBlowLimit);
	szContent = szContent + szText + szTemp;

	fTemp.WriteString(szContent);
	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to update BH package data to BHPackageList.csv");
	
	fTemp.Close();
	return TRUE;
}


LONG CBondHead::GetWTEncoderValue(LONG *lX, LONG *lY, LONG *lT)
{
	IPC_CServiceMessage svMsg;
	INT nConvID;
	typedef struct 
	{
		LONG lX;
		LONG lY;
		LONG lT;
	} WT_XYT_STRUCT;
	WT_XYT_STRUCT stXYT;

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stXYT);
	nConvID = m_comClient.SendRequest("WaferTableStn", "GetEncoderCmd", svMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			svMsg.GetMsg(sizeof(WT_XYT_STRUCT), &stXYT);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	*lX = stXYT.lX;
	*lY = stXYT.lY;
	*lT = stXYT.lT;

	return 1;
}


DOUBLE CBondHead::GetEjrLifeTimePercentage()
{
	DOUBLE EjrLifeTimePercentage;
	if (m_ulMaxEjectorCount > 0)
	{
		EjrLifeTimePercentage = (m_ulEjectorCount/m_ulMaxEjectorCount) * 100;
	}
	else
	{
		EjrLifeTimePercentage = 100;
	}
	return EjrLifeTimePercentage;
}


LONG CBondHead::IsColletJamForBothBH()
{
	SetPickVacuum(FALSE);
	SetPickVacuumZ2(FALSE);
	SetStrongBlow(TRUE);
	SetStrongBlowZ2(TRUE);
	Sleep(100);
	SetStrongBlow(FALSE);
	SetStrongBlowZ2(FALSE);
	Sleep(100);
	SetPickVacuum(TRUE);
	SetPickVacuumZ2(TRUE);
	Sleep(100);
	BOOL bIsBH1ColletJam = IsColletJam();
	BOOL bIsBH2ColletJam = IsColletJamZ2();
	if (bIsBH1ColletJam && bIsBH2ColletJam)
	{
		return 3; // Both BH Collet Jam
	}
	else if (bIsBH2ColletJam)
	{
		return 2; // BH2 Collet Jam
	}
	else if (bIsBH1ColletJam)
	{
		return 1; // BH1 Collet Jam
	}
	
	return 0; // No Collet Jam in Both BH
}


BOOL CBondHead::CreateResetColletEjectorCountOutputFileSelectionFolder()
{
	CString szFolderPath = "C:\\MapSorter\\UserData\\DropDownSelection";
	CreateDirectory(szFolderPath, NULL);
	CStdioFile cfFile;
	CString szFilePath;

	szFilePath = szFolderPath + "\\ShiftSelection.csv";
	if (_access(szFilePath, 0) == -1)
	{
		cfFile.Open(szFilePath, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
		cfFile.WriteString(",Day,Night");
		cfFile.Close();
	}

	szFilePath = szFolderPath + "\\ReasonSelection.csv";
	if (_access(szFilePath, 0) == -1)
	{
		cfFile.Open(szFilePath, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
		cfFile.WriteString(",Full,Broken,ChangeType,MissingDie,ColletJam");
		cfFile.Close();
	}

	szFilePath = szFolderPath + "\\ColletTypeSelection.csv";
	if (_access(szFilePath, 0) == -1)
	{
		cfFile.Open(szFilePath, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
		cfFile.WriteString(",2.5-mil,3-mil,3.5mil,4-mil,6-mil,8-mil");
		cfFile.Close();
	}

	szFilePath = szFolderPath + "\\EjectorTypeSelection.csv";
	if (_access(szFilePath, 0) == -1)
	{
		cfFile.Open(szFilePath, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
		cfFile.WriteString(",0.013,0.018,0.022");
		cfFile.Close();
	}

	szFilePath = szFolderPath + "\\UsageTypeSelection.csv";
	if (_access(szFilePath, 0) == -1)
	{
		cfFile.Open(szFilePath, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
		cfFile.WriteString(",Test,Normal");
		cfFile.Close();
	}

	return TRUE;
}


BOOL CBondHead::HasBHZ2()
{
	return m_bIsArm2Exist == TRUE;
}


BOOL CBondHead::CheckDigitalAirFlowThresholdLimit(BOOL bIsBHZ2)
{
	if (!m_bMS100DigitalAirFlowSnr)
	{
		return TRUE;
	}

	CString szLog;
	if (bIsBHZ2 && m_bIsArm2Exist)
	{
		if ( (m_lBHZ1ThresholdLimit > 0)	&& 
			 //(m_lBHZ1ThresholdLimit > m_ulBHZ2DAirFlowBlockValue)	&& 
			 (m_ulBHZ2DAirFlowThreshold < m_lBHZ1ThresholdLimit))
		{
			szLog = "BHZ2 Missing-Die Threshold value is too low. Please check collet or change collet";
			HmiMessage_Red_Yellow(szLog);
			SetErrorMessage(szLog);
			return FALSE;
		}

		if ( (m_lBHZ1ThresholdUpperLimit > 0)	&& 
			 (m_ulBHZ2DAirFlowThreshold > m_lBHZ1ThresholdUpperLimit))
		{
			szLog = "BHZ2 Missing-Die Threshold value is too Upper. Please check collet or change collet";
			HmiMessage_Red_Yellow(szLog);
			SetErrorMessage(szLog);
			return FALSE;
		}

		if ( (m_lBHZ2ThresholdLimit > 0)	&& 
			 (m_ulBHZ2DAirFlowThresholdCJ < m_lBHZ2ThresholdLimit) )
		{
			szLog = "BHZ2 CJ Threshold value is too low. Please check collet or change collet";
			HmiMessage_Red_Yellow(szLog);
			SetErrorMessage(szLog);
			return FALSE;
		}

	}
	else
	{
		if ( (m_lBHZ1ThresholdLimit > 0) && 
			 //(m_lBHZ1ThresholdLimit > m_ulBHZ1DAirFlowBlockValue)	&& 
			 (m_ulBHZ1DAirFlowThreshold < m_lBHZ1ThresholdLimit) )
		{
			szLog = "BHZ1 Missing-Die Threshold value is too low. Please check collet or change collet";
			HmiMessage_Red_Yellow(szLog);
			SetErrorMessage(szLog);
			return FALSE;
		}

		if ( (m_lBHZ1ThresholdUpperLimit > 0) && 
			 (m_ulBHZ1DAirFlowThreshold > m_lBHZ1ThresholdUpperLimit) )
		{
			szLog = "BHZ1 Missing-Die Threshold value is too Upper. Please check collet or change collet";
			HmiMessage_Red_Yellow(szLog);
			SetErrorMessage(szLog);
			return FALSE;
		}

		if ( (m_lBHZ2ThresholdLimit > 0) && 
			 (m_ulBHZ1DAirFlowThresholdCJ < m_lBHZ2ThresholdLimit) )
		{
			szLog = "BHZ1 CJ Threshold value is too low. Please check collet or change collet";
			HmiMessage_Red_Yellow(szLog);
			SetErrorMessage(szLog);
			return FALSE;
		}
	}
	return TRUE;
}


VOID CBondHead::CleanColletPocketAlcoholOnce()
{
	// press the alcohol to the cloth
	SetCleanColletPocket(BH_ACC_POCKET_TURN_ON);	
	if (m_lAutoCleanColletSwingTime > 0)
	{
		Sleep(m_lAutoCleanColletSwingTime);	//Swing 
	}
	SetCleanColletPocket(BH_ACC_POCKET_TURN_OFF);
	if (m_lAccLastDropDownTime > 0)
	{
		Sleep(m_lAccLastDropDownTime);
	}
}


//v4.44T2
BOOL CBondHead::UserSearchDieToPrCenter(BOOL bBHZ2)
{
	//SetWaferTableJoystick(FALSE);
	//LockWaferTableJoystick(TRUE);
	//LockPRMouseJoystick(TRUE);
	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;
	BOOL bIsBHZ2 = bBHZ2;
	stMsg.InitMessage(sizeof(BOOL), &bIsBHZ2);
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchDieWithEjt", stMsg);
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


BOOL CBondHead::BH_SubMoveBHToPick(const BOOL bPick, const LONG lOffsetT)
{
	BOOL bReturn	= TRUE;
	INT nCount		= 0;

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed");
		return FALSE;
	}

	if( LoopCheckCoverOpen("Move To Pick Level")==FALSE )
	{
		return FALSE;
	}

	if (bPick)
	{
		Z_MoveTo(0);

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}

		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPickPos_T + lOffsetT);
		T_Profile(NORMAL_PROF);

		Z_MoveTo(m_lPickLevel_Z + 500);		//v4.43T5
	}
	else
	{
		Z_MoveTo(0);

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}

		if( pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR))
		{
			Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
			Z_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
			CMSLogFileUtility::Instance()->MS_LogOperation("Uplook --- Move to Swing Level");
		}
		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
		T_SMoveTo(m_lPrePickPos_T);
		T_Profile(NORMAL_PROF);
	}

	return TRUE;
}

	
BOOL CBondHead::GenerateParameterList()
{
	BOOL bReturn;
	INT nConvID;
	IPC_CServiceMessage rReqMsg;
	CString szMsg, szConfigFile;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//1. Remove old Parameter List

	if (pApp->GetCustomerName() == "EverVision")
	{	
		CString szMachine = (*m_psmfSRam)["MS896A"]["MachineNo"];
		CTime theTime = CTime::GetCurrentTime();

		CString szFileNameTime			 = theTime.Format("%Y_%m_%d_%H_%M_%S");
		CString szMachineTimePath2;// = (*m_psmfSRam)["MS896A"]["OutputFilePath2"];

		CString szPathFile = "c:\\mapsorter\\pathfile.txt";
		CString szTemp;
		CStdioFile cfCheck;
		if( _access(szPathFile,0) != -1)
		{
			if( cfCheck.Open(szPathFile,CFile::modeRead))
			{
				cfCheck.ReadString(szTemp);
				if (szTemp == "")
				{
					HmiMessage("Path 2 Empty");
				}
				else
				{
					szMachineTimePath2 = szTemp;
					if (_access(szMachineTimePath2,0) == -1)
					{
						HmiMessage("Could Not Access Parameter List Path:" + szMachineTimePath2 + ",Save Parameter List Fails");
					}
				}
				cfCheck.Close();		
			}
		}
		else
		{
			HmiMessage("c:\\mapsorter\\pathfile.txt cannot be found!");
		}

		szConfigFile = szMachineTimePath2 + "\\Parameters_" + szMachine + "_" + szFileNameTime + ".csv";
		(*m_psmfSRam)["MS896A"]["ParameterListPath"] = szConfigFile;
		CMSLogFileUtility::Instance()->MS_LogOperation("Parameter List:" + szConfigFile);
	}
	else
	{
		szConfigFile = _T("c:\\MapSorter\\UserData\\Parameters.csv");
		(*m_psmfSRam)["MS896A"]["ParameterListPath"] = szConfigFile;
	}
	DeleteFile(szConfigFile);

	CStdioFile oFile;
	CString szLine;
	if (oFile.Open(szConfigFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText) == TRUE)
	{
		oFile.SeekToEnd();

#ifdef NU_MOTION
		if (IsMS90())
		{
			oFile.WriteString("MS90 Mapping Sorter Machine Parameter List\n\n");
		}
		else if (IsMS60())
		{
			oFile.WriteString("MS60 Mapping Sorter Machine Parameter List\n\n");
		}
		else if (CMS896AApp::m_bMS100Plus9InchOption)
		{
			oFile.WriteString("MS109 Mapping Sorter Machine Parameter List\n\n");
		}
		else
		{
			oFile.WriteString("MS100 Mapping Sorter Machine Parameter List\n\n");
		}
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


		if (pApp->GetCustomerName() == "EverVision" || pApp->GetCustomerName() == "FiberOptics") //4.52D2
		{
			CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];
			CString szCurrentTime;
			CString szAverageTime;
			szCurrentTime.Format("%.2f",m_dCycleTime);
			szAverageTime.Format("%.2f",m_dAvgCycleTime);
			
			oFile.WriteString("[Bond]\n");
			oFile.WriteString("Cycle Time,"		+ szCurrentTime + "\n");
			oFile.WriteString("Average Time,"	+ szAverageTime+ "\n");
			oFile.WriteString("PKG File,"	+ szPKGFilename+ "\n");

			oFile.WriteString("[Bond->Collet & Ejector]\n");
CString szLine;
			szLine.Format("Collet Used Count,%d\n",				m_ulColletCount);
			oFile.WriteString(szLine);

			szLine.Format("Collet Change Limit,%d\n",				m_ulMaxColletCount);
			oFile.WriteString(szLine);

			szLine.Format("Collet2 Used Count,%d\n",				m_ulCollet2Count);
			oFile.WriteString(szLine);

			szLine.Format("Collet2 Change Limit,%d\n",				m_ulMaxCollet2Count);
			oFile.WriteString(szLine);

			szLine.Format("Clean Limit,%d\n",				m_ulMaxCleanCount);
			oFile.WriteString(szLine);

			szLine.Format("Ejector Used,%d\n",				m_ulEjectorCount);
			oFile.WriteString(szLine);

			szLine.Format("Ej Change Limit,%d\n",				m_ulMaxEjectorCount);
			oFile.WriteString(szLine);

			oFile.WriteString("[Misc. Setup]");
			szLine.Format("Auto Clean Collet ON/OFF,%d\n",				m_bAutoCleanCollet);
			oFile.WriteString(szLine);
		}
		oFile.Close();
	}

//AfxMessageBox("//2. Generate BH Config Data");
	//2. Generate BH Config Data
	bReturn = GenerateConfigData();
	if (!bReturn)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("BH: Fail to generate parameter list!");
		return FALSE;
	}

//AfxMessageBox("//3. Wafer Table Config Data");
	//3. Wafer Table Config Data
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GenerateConfigData", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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
	if (bReturn == FALSE)
	{
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return TRUE;
	}

//AfxMessageBox("//4. Bin Table Config Data");
	//4. Bin Table Config Data
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GenerateConfigData", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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
	if (bReturn == FALSE)
	{
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return TRUE;
	}

//AfxMessageBox("//5. WaferLoader Config Data");
	//5. WaferLoader Config Data
	nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "GenerateConfigData", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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
	if (bReturn == FALSE)
	{
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return TRUE;
	}

//AfxMessageBox("//6. BinLoader Config Data");
	//6. BinLoader Config Data
	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "GenerateConfigData", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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
	if (bReturn == FALSE)
	{
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		//return TRUE;
	}
//AfxMessageBox("//WPR");
	//WPR

	//if (pApp ->GetCustomerName() == "EverVision")
	//{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "GenerateConfigData", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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
		if (bReturn == FALSE)
		{
			//svMsg.InitMessage(sizeof(BOOL), &bReturn);
			//return TRUE;
		}

		nConvID = m_comClient.SendRequest(BOND_PR_STN, "GenerateConfigData", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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
		if (bReturn == FALSE)
		{
			//svMsg.InitMessage(sizeof(BOOL), &bReturn);
			//return TRUE;
		}
	//}
	return TRUE;
}


BOOL CBondHead::IsEnableBHMark()
{
	return (m_lAutoUpdateBHPickPosnMode == 1);
}


BOOL CBondHead::IsEnableColletHole()
{
	return (m_lAutoUpdateBHPickPosnMode == 2);
}


BOOL CBondHead::ManualCleanCollet_PLLM_MS109()
{
	return TRUE;
}


BOOL CBondHead::MoveProberDownAndUp()
{
	Z_MoveTo(0);

	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_MoveTo(m_lPickPos_T);
	Sleep(100);
	Z_MoveTo(m_lPickLevel_Z);
	Sleep(2000);
	Z_MoveTo(0);
	T_MoveTo(m_lPrePickPos_T);
	T_Profile(NORMAL_PROF);
	
	return TRUE;
}

BOOL CBondHead::DisableEjectorDisplay(UINT unStandbyLevel, LONG lMotorType, BOOL bReset)
{
	if (bReset)
	{
		m_bLearnStandby_EJ_Pin		= TRUE;
		m_bLearnStandby_EJ_Cap		= TRUE;
		m_bLearnStandby_EJ_Elevator = TRUE;
		m_bLearnStandby_EJ_Theta	= TRUE;
		m_bLearnUpLevel_EJ_Pin		= TRUE;
		m_bLearnUpLevel_EJ_Cap		= TRUE;
		m_bLearnUpLevel_EJ_Elevator = TRUE;
		m_bLearnUpLevel_EJ_Theta	= TRUE;

		return TRUE;
	}
	
	m_bLearnStandby_EJ_Pin		= FALSE;
	m_bLearnStandby_EJ_Cap		= FALSE;
	m_bLearnStandby_EJ_Elevator = FALSE;
	m_bLearnStandby_EJ_Theta	= FALSE;
	m_bLearnUpLevel_EJ_Pin		= FALSE;
	m_bLearnUpLevel_EJ_Cap		= FALSE;
	m_bLearnUpLevel_EJ_Elevator = FALSE;
	m_bLearnUpLevel_EJ_Theta	= FALSE;

	if (unStandbyLevel == CHANGE_EJ_STANDBY)	//Standby Level
	{
		switch (lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				m_bLearnStandby_EJ_Pin = TRUE;
				break;
			case EJ_THETA_LEARN_LEVEL:		
				m_bLearnStandby_EJ_Theta = TRUE;
				break;
			case EJ_CAP_LEARN_LEVEL:		
				m_bLearnStandby_EJ_Cap = TRUE;
				break;	
			default:
				m_bLearnStandby_EJ_Elevator = TRUE;
				break;
		}
	}
	else	//Up Level
	{
		switch (lMotorType)
		{
			case EJ_PIN_LEARN_LEVEL:
				m_bLearnUpLevel_EJ_Pin = TRUE;
				break;
			case EJ_THETA_LEARN_LEVEL:
				m_bLearnUpLevel_EJ_Theta = TRUE;
				break;
			case EJ_CAP_LEARN_LEVEL:
				m_bLearnUpLevel_EJ_Cap = TRUE;
				break;	
			default:
				m_bLearnUpLevel_EJ_Elevator = TRUE;
				break;
		}
	}

	return TRUE;
}