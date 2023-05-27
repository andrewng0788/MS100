/////////////////////////////////////////////////////////////////
// BinLoader.h : interface of the CBinLoader class
//
//	Description:
//		
//
//	Date:		12 August 2004
//	Revision:	1.00
//
//	By:			
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., .
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

#include "MS896A.h"
#include "MS896AStn.h"
#include "BL_Constant.h"
#include "BL_Struct.h"
#include "SfmSerialPort.h"
#include "LogFileUtil.h"
#include "resource.h"
#include "MESConnector.h"
#include "BinGripperBuffer.h"


class CBinLoader : public CMS896AStn
{
	DECLARE_DYNCREATE(CBinLoader)
public:
	BOOL IsBTFramePlatformDown();
protected:
	#define BIN_Z_HOME_TIMEOUT_HOUR					2

	#define TRANSFER_WIP_NONE_FRAME					0
	#define TRANSFER_WIP_FULL_BIN_FRAME				1
	#define TRANSFER_WIP_NOT_SELECTED_BIN_FRAME		2
	#define TRANSFER_WIP_ONLY_ONE_BIN_FRAME			3

	#define MS_BL_AUTO_LINE_MGZN_NUM				8
	#define MS_BL_AUTO_LINE_INPUT_START_MGZN_SLOT	1
	#define MS_BL_AUTO_LINE_INPUT_END_MGZN_SLOT		15 //5
	#define MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT	16 //6
	#define MS_BL_AUTO_LINE_OUTPUT_END_MGZN_SLOT	25 //10
	#define MS_BL_AUTO_LINE_WIP_START_MGZN_SLOT		1  //for other magazine, //11 

	enum // slot usage state
	{
		BL_SLOT_USAGE_UNUSE = 0,
		BL_SLOT_USAGE_FULL,
		BL_SLOT_USAGE_EMPTY,
		BL_SLOT_USAGE_ACTIVE,
		BL_SLOT_USAGE_ACTIVE2FULL,		// just for run time use, at setup, no such option
		BL_SLOT_USAGE_INUSE,			//New for Mode-H only (Yealy)	//v4.34T1
		BL_SLOT_USAGE_SORTING
	};

	// Operation state in Auto State
	enum {	WAIT_OPERATE_Q,
			CHANGE_GRADE_Q,
			BIN_FULL_Q,
			HOUSE_KEEPING_Q,
			DBUFFER_LOAD_FRAME_Q,
			DBUFFER_UNLOAD_FRAME_Q,
			DBUFFER_CHANGE_GRADE_Q,
			DBUFFER_BIN_FULL_Q,
			DBUFFER_PRELOAD_EMPTY_FRAME_Q,
			DBUFFER_UNLOAD_FULL_FRAME_Q,
			//v4.59A45
			UDB_PRELOAD_FRAME_Q,
			UDB_UNLOAD_FRAME_Q,
			UDB_CHANGE_GRADE_Q,
			UDB_BIN_FULL_Q,		
			UDB_PRELOAD_EMPTY_Q,	
			UDB_UNLOAD_FULL_FRAME_Q
		};

protected: // for variables and elements

	// Axis Info
	CMSNmAxisInfo	m_stBLAxis_X;
	CMSNmAxisInfo	m_stBLAxis_X2;
//	CMSNmAxisInfo	m_stBLAxis_Y;
	CMSNmAxisInfo	m_stBLAxis_Z;
	CMSNmAxisInfo	m_stBLAxis_ExArm;
	CMSNmAxisInfo	m_stBLAxis_Theta;
	CMSNmAxisInfo	m_stBLAxis_Upper;
	CMSNmAxisInfo	m_stBLAxis_Lower;

    // status variable
	BOOL	m_bFrameInClampDetect;
	BOOL	m_bFrameInClampDetect2;
	BOOL	m_bFrameOnBTPosition;
	BOOL	m_bFrameOnBTPosition2;
	BOOL	m_bFrameProtection;
	BOOL	m_bFrameProtection2;
	BOOL	m_bFrameInMagazine;
	BOOL	m_bFrameJam;
	BOOL	m_bFrameJam2;
	//BOOL	m_bFrameOnBTDetect;
    BOOL    m_bMagazineExist[MS_BL_MGZN_NUM];
	BOOL    m_bMagazine2Exist;

	BOOL	m_bBufferSafetySensor_HMI;
	BOOL    m_bElevatorDoorCover_HMI;
	BOOL	m_bFrontRightElevatorDoorCover_HMI;

	BOOL	m_bFrameOutOfMgz;

	BOOL	m_bFrameLevel;
	BOOL	m_bFrameLevel2;
	BOOL	m_bFrameVacuum;
	BOOL	m_bFrameVacuum2;
	BOOL	m_bFrontArmFrameExist;
	BOOL	m_bRearArmFrameExist;
	BOOL	m_bFrontArmReady;
	BOOL	m_bRearArmReady;
	BOOL	m_bBufferFrameExist;
	BOOL	m_bPressureSensor;		//v4.59A38
	//Dual Buffer Table Sensors
	BOOL	m_bDualBufferUpperExist;
	BOOL	m_bDualBufferLowerExist;
	BOOL	m_bDualBufferRightProtect;
	BOOL	m_bDualBufferLeftProtect;
	BOOL	m_bDualBufferLevel;
	//v4.59A40	//MS90 new Buffer Pusher design for Finisar TX MS90
	BOOL	m_bEnableMS90TablePusher;
	BOOL	m_bMS90PusherX;
	BOOL	m_bMS90PusherZ;
	BOOL	m_bMS90PusherLimitX;

	BOOL	m_bUpdateOutput;	// Update Sensor & encoder in HMI
	BOOL	m_bHome_Z;			// Z Home flag
	BOOL	m_bHome_X;			// X Home flag
	BOOL	m_bHome_X2;			// X Home flag
//	BOOL	m_bHome_Y;			// X Home flag
	BOOL	m_bHome_Arm;		// Arm Home flag
	BOOL    m_bHome_Theta;
	BOOL    m_bHome_Upper;
	BOOL    m_bHome_Lower;


	BOOL	m_bComm_Z;			// Z Communtate flag
	//BOOL	m_bComm_Y;			// Y Communtate flag
	BOOL	m_bComm_Arm;		// ExArm (DLA) Commutate flag

	BOOL	m_bSel_Z;			// Select Z flag
	BOOL	m_bSel_X;			// Select X flag
//	BOOL	m_bSel_Y;			// Select Y flag
	BOOL	m_bSel_Arm;			// Select Arm flag
	BOOL    m_bSel_Theta;
	BOOL    m_bSel_Upper;
	BOOL    m_bSel_Lower;

	BOOL	m_bIsEnabled;		
	BOOL	m_bIsFirstCycle;
	BOOL	m_bShowDebugMessage;
	BOOL	m_bIsExChgArmExist;
	BOOL	m_bDisableLoaderY;		//MS60 AutoLine	//v4.55A7
	BOOL	m_bUseLargeBinArea;		//Large Bin Area so WT may need to move away during UNLOAD/LOAD //v3.61
	LONG	m_lBinLoaderConfig;		//0=DL; 1=DLA; 2=DL with Buffer Table
	BOOL	m_bUseBLAsLoaderZ;		//v4.31T11	//Yealy MS100Plus
	BOOL	m_bIsDualBufferExist;
	BOOL	m_bDualBufferPreloadLevel;
	BOOL	m_bIsNewDualBuffer;
	BOOL	m_bUseTimeBeltMotorY;	//v3.94
	BOOL	m_bGripperUseEncoder;	//v4.59A39

	DOUBLE	m_dZRes;			// Z Resolution (mm / motor step)

	BOOL	m_bNewZConfig;			//2.54
	BOOL	m_bFastBcScanMethod;	//v2.71
	DOUBLE	m_dGripperXRes;			//v2.83T2
	DOUBLE	m_dThetaXRes;
	LONG	m_lBIN_THETA_MISSING_STEP_TOL;

	/*--- Server Variables ---*/
	// Sensor State
	BOOL	m_bHomeSnr_X;		// X Home Sensor
	BOOL	m_bHomeSnr_X2;		// X2 Home Sensor
	BOOL	m_bHomeSnr_Y;		// Y Home Sensor
	BOOL	m_bHomeSnr_Z;		// Z Home Sensor
	BOOL	m_bHomeSnr_Arm;		// Arm Home Sensor
	BOOL	m_bULimitSnr_X;		// X Upper Limit Sensor
	BOOL	m_bLLimitSnr_X;		// X Lower Limit Sensor	
	BOOL	m_bULimitSnr_Y;		// Y Upper Limit Sensor
	BOOL	m_bLLimitSnr_Y;		// Y Lower Limit Sensor
	BOOL	m_bULimitSnr_Z;		// Z Upper Limit Sensor
	BOOL	m_bLLimitSnr_Z;		// Z Lower Limit Sensor

	BOOL	m_bHomeSnr_Upper; 
	BOOL	m_bHomeSnr_Lower; 
	BOOL	m_bHomeSnr_Theta; 

	// Encoder count
	LONG	m_lEnc_X;			// X Encoder count(Upper)
	LONG	m_lEnc_X2;			// X2 Encoder count(Lower)		//MS100 9Inch
	LONG	m_lEnc_Y;			// Y Encoder count
	LONG	m_lEnc_Z;			// Z Encoder count
	LONG	m_lEnc_Arm;			// Arm Encoder count
	LONG	m_lEnc_T;			// Theta

    // debug variables
    BOOL    m_bStepMode;

	/*--- Variable need to be saved in file & as Server Variables---*/
//	barcode options
	BOOL	m_bWarningDisabledBarcodeScanner;
	BOOL	m_bUseBarcode;			
	BOOL	m_bUseExtension;
	BOOL	m_bCheckBarcode;
	BOOL	m_bCompareBarcode;
	BOOL	m_bStopChgGradeScan;	//	Unload frame (not full, only change grade), skip BC scan.
	BOOL	m_bUseEmptyFrame;
	BOOL	m_bCheckBCSkipEmpty;
	LONG	m_lScanRange;			
    LONG    m_lTryLimits;
	CString	m_szExtName;
	CString	m_szBCName;
	CString	m_szBCName2;
	CString m_szBinBarcodePrefix;
	LONG m_lBinBarcodeLength;
	CString m_szDBPreScanBCName;	//pllm
	//	m_szLoadReadBarcode	BC when load from magazine, keep for BT
	UCHAR	m_ucCommPortNo;	//	useless
	UCHAR	m_ucBarcodeModel;
//	barcode options

	BOOL	m_bWaferToBinResort;

	LONG	m_lTemperature;

	//Bin Gripper Position
    LONG    m_lPreUnloadPos_X;
	LONG	m_lUnloadPos_X;		
	LONG	m_lLoadMagPos_X;
	LONG	m_lUnloadMagPos_X;
	LONG	m_lReadyPos_X;		
	LONG	m_lBarcodePos_X;
	LONG	m_lBufferUnloadPos_X;
	LONG	m_lSIS_Top1MagClampOffsetX;	
	LONG	m_lTop2MagClampOffsetX;		
	BOOL	m_bHomeGripper;

    LONG    m_lPreUnloadPos_X2;
	LONG	m_lUnloadPos_X2;		
	LONG	m_lLoadMagPos_X2;
	LONG	m_lUnloadMagPos_X2;
	LONG	m_lReadyPos_X2;		
	LONG	m_lBarcodePos_X2;
	LONG	m_lBufferUnloadPos_X2;
	LONG	m_lSIS_Top1MagClampOffsetX2;	
	LONG	m_lTop2MagClampOffsetX2;		
	BOOL	m_bHomeGripper2;

	//v4.59A40	//MS50
	LONG	m_lLoadPos1_T;		
	LONG	m_lLoadPos2_T;	
	LONG	m_lReadyPos_T;

	//v4.31T11		//MS100 Single-Loader option	//Yealy MS100Plus
	BL_WL_MAGAZINE m_stWaferMgzn[MS_BL_WL_MGZN_NUM];
	LONG	m_lCurrWLMgzn;
	LONG	m_lCurrWLSlot;

	//Bin Loader magazine Position
    //BL_MAGAZINE m_stMgznRT[MS_BL_MGZN_NUM]; // magazine run time settings
    BL_MAGAZINE m_stMgznOM[MS_BL_MGZN_NUM]; // magazine operation mode
	LONG	m_lCurrMgzn;
	LONG	m_lCurrSlot;
	LONG	m_lOMRT;
    LONG    m_lOMSP;
	LONG	m_lSetupMagPos;
    LONG    m_lReadyPosY;
    LONG    m_lReadyLvlZ;
	//MS50_01
    LONG    m_lUnloadOffset; 
	LONG	m_lUpperToLowerBufferOffsetZ;	
	LONG	m_lUseLBufferGripperForSetup;

    LONG    m_lUnloadOffsetX;			//v2.71
    LONG    m_lUnloadOffsetY; 
	LONG	m_lCurrBufferMgzn;			//v2.67
	LONG	m_lCurrBufferSlot;			//v2.67
    LONG    m_lUnloadPusherOffsetX;		//v2.93T2
	LONG	m_lLoadSearchInClampOffsetX;	//v3.59
	//BOOL	m_bSemitekBLMode;			//v4.42T9	//moved to MS896aStn base class
	CString	m_szCurrMgznDisplayName;	//v4.17T1

	//MS60 AutoLine //v4.56A1
	LONG	m_lAutoLineUnloadZ;			
	LONG	m_lAutoLineUnloadSlotID;	

	LONG	m_lCurrMgzn2;
	LONG	m_lCurrSlot2;
	LONG	m_lCurrHmiSlot2;
	CString	m_szCurrMgznDisplayName2;
	BOOL	m_bChangeOMRecordFileName;
	//Bin Table
    LONG    m_lBTCurrentBlock;
    LONG    m_lBTCurrentBlock2;			//MS100 9Inch dual-table config		//v4.17
	//LONG	m_lBTUnloadPos_X2;
	//LONG	m_lBTUnloadPos_Y2;
	LONG	m_lBTAlignFrameCount;
	LONG	m_lBTAlignFrameDelay;
	LONG	m_lBTVacuumDelay;
	LONG	m_lBT_FrameUpBlowTime;
	LONG	m_lVacSettleDelay;			//v3.67T1	//Semitek
	LONG	m_lBTLoadOffset_Y;			//v3.79	
	LONG	m_lOpBTForUnload;			//v4.21T1	//MS100 9Inch dual-table config		//0=none, 1=BT1, 2=BT2
	LONG	m_lBTExArmOffsetX;			//v3.74T41
	LONG	m_lBTExArmOffsetY;			//v3.74T41

	// Buffer Table
	LONG	m_lBUTAlignFrameCount;
	LONG	m_lBUTAlignFrameDelay;
	LONG	m_lLoadFrameRetryCount;		//v4.43T13	//SeoulOpto

	//Bin Exchange Arm Position
	BOOL	m_bLoadFromEmpty;
	LONG	m_lExArmBufferBlock;
	LONG	m_lExArmPickPos;
	LONG	m_lExArmPlacePos;
	LONG	m_lExArmReadyPos;
	LONG	m_lExArmPickDelay;
	LONG	m_lExArmVacuumDelay;
	LONG	m_lExArmUpDelay;			//v2.93T2
	
	BOOL	m_bIsClearAllBinFrame;

	//Dual Table 
	BOOL	m_bDualDLPreloadEmpty;		//v4.48A15	//Freescale

	BOOL	m_bMotionFail;
	BOOL	m_bCoverSensorAbort;
    BOOL    m_bUnloadDone;
    BOOL    m_bBurnInEnable;
    BOOL    m_bSetSlotBlockEnable;
    BOOL    m_bBurnInTestRun;
    LONG    m_bRealignBinFrame;
    BOOL    m_bNoSensorCheck;
	BOOL	m_bUseBinTableVacuum;
	BOOL	m_bCheckElevatorCover;
	BOOL	m_bCheckFrameIsAligned;

	BOOL	m_bExArmBarcodeOnGripper;
	//LONG    m_lBurnInBlock;

	/*--- Variable NO need to be saved (Server Variables only)---*/
	//Process Setup  
    LONG    m_lBLGeneral_1;		// Gripper preUnload 
    LONG    m_lBLGeneral_2;		// Gripper unload BT
    LONG    m_lBLGeneral_3;		// Gripper scan bar code
    LONG    m_lBLGeneral_4;		// Gripper load magzine
	LONG    m_lBLGeneral_5;		// gripper ready
    LONG    m_lBLGeneral_6;		// bin table unload x
    LONG    m_lBLGeneral_7;		// bin table unload y
	LONG    m_lBLGeneral_8;		// Gripper Buffer Unload
	LONG    m_lBLGeneral_9;		// BT Barcode pos X		//v4.39T7	//Nichia MS100+
	LONG    m_lBLGeneral_10;	// BT Barcode pos Y		//v4.39T7	//Nichia MS100+
	LONG    m_lBLGeneral_11;	// BL UNLOAD level Z for AutoLine		//v4.46A1	//Nichia MS60 AutoLine
	LONG    m_lBLGeneral_12;	// BL Load1 T for MS50	//v4.59A43
	LONG    m_lBLGeneral_13;	// BL Load2 T for MS50
	LONG    m_lBLGeneral_14;	// BL Ready T for MS50
	LONG	m_lBLGeneral_15;	// Gripper unload Magazine X
	LONG    m_lBLGeneral_TmpA;	//Temp. variable
	LONG    m_lSelMagazineID;
	DOUBLE	m_dSlotPitch;		// Slot Pitch in mm
	LONG	m_lCurrHmiMgzn;		// Current Mgzn no for DLA display only		//v2.67 
	LONG	m_lCurrHmiSlot;		// Current Slot no 

	CString m_szBLConfigFileName;
	CString m_szBLConfigFileName_Bp;	//v4.06		//SanAn
    CString m_szBLMgznOMFileName;
    CString m_szBLMgznRTFileName;
    CString m_szBLMgznRTFileName_Bp;	//v4.06		//SanAn

    // for operation mode setup
    CString m_szBLMgznUsageDef[5];

	CString m_szOMSP;					//v3.82
    CString m_szBLMgznUse[MS_BL_MGZN_NUM];
    CString m_szBLSlotUsage;

    LONG	m_ulMgznSelected;
    LONG	m_ulSlotSelected;
    LONG	m_ulSlotPhyBlock;

	CString	m_szMagzFullFilePath;
	BOOL	m_bEnableMagzFullFile; 
	BOOL	m_bExChgFullMgznOutputFile;	//v4.15T9	//HPO

	BOOL	m_bClearAllFrameState;
	ULONG	m_ulMinClearAllCount;
	LONG	m_lClearAllFrameMode;

	CString m_szAutoPreLoadBarCode;		//v2.67
	BOOL	m_bExArmPreLoadEmptyFrame;	//v2.67
	BOOL	m_bExArmUnloadBufferFrame;	//v2.68

	BOOL	m_bGripperPusher;			//v2.93T2
	BOOL	m_bIsExArmGripperAtUpPosn;	//v2.93T2
	BOOL	m_bBarcodeReadAbort;		//v2.83T45		//Used in Autobond mode when barocde failure ABORT is triggered

	BOOL	m_bDisplaySingleMgznSnr;	//v3.30T2		//Display single magazine sensor in HMI
	BOOL	m_bDisplayMultiMgznSnrs;	//v3.30T2		//Display multiple magazine sensor in HMI

	BOOL	m_bLoadUnloadBtToggle;		//v4.02T1		//WH Semitek
	LONG	m_lBTBackupBlock;			//v4.02T1		//WH Semitek

	BOOL	m_bGenBinFullFile;

	CString m_szBinFrameBarcode;
	CString m_szBinFrame2Barcode;
	CString m_szBufferFrameBarcode;
	//BOOL	m_bDebugCheckSensor;		//v3.35

	// Dual Buffer Table
	CBinGripperBuffer m_clUpperGripperBuffer;
	CBinGripperBuffer m_clLowerGripperBuffer;
	LONG	m_lTestBufferBlock;	//for Buffer Test
	LONG	m_lTestTableBlock;	//for Buffer Test
	BOOL	m_bDualBufferPreLoadEmptyFrame;
	BOOL	m_bDualBufferUnloadBufferFrame;

	LONG	m_lDualBufferTestMgzn;	//for Buffer Test

	//BOOL	m_bEnableCreeBinBcSummary;			//v3.75
	CString m_szCreeBinBcSummaryPath;			//v3.75

	BOOL	m_bEnableBLMotionLog;
	CBinLoaderDataBlock m_oBinLoaderDataBlock;

	BOOL	m_bFrontGateSensor;
	BOOL	m_bBackGateSensor;
	//v4.59A20	//Renesas MS90 BL special Config mode
	INT m_nRenesasBLConfigMode_Temp;
	INT m_nRenesasBLConfigMode;
	INT		m_nNoOfSubBlk;
	INT		m_nNoOfSubGrade;
	INT     m_nSubGrade[5][5];
	INT		m_nSubBlk[5];

	//For Generate Empty Frame Text File, GenerateEmptyFrameTextFile()
	CString m_szEmptyFramePath;

private:

	CEvent	m_evBLOperate;			// Bin Loader Operate
	CEvent	m_evBLReady;			// Bin Loader Ready
	CEvent  m_evBLPreChangeGrade;	// Bin loader pre-chagne grade for DL CT enhancement	//v2.71
	CEvent	m_evBLAOperate;			// Bin Loader - A Operate
	CEvent	m_evBLPreOperate;		// Bin Loader Pre Operate
	CEvent	m_evBLFrameLevelDn;		// Bin table frame level platfrom down checking			//v4.01


protected: // functions
	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	virtual VOID RegisterVariables();
	
	VOID RunOperation();
	virtual	VOID IdleOperation();
	virtual	VOID InitOperation();
	virtual	VOID PreStartOperation();
	virtual	VOID AutoOperation();
	virtual	VOID DemoOperation();
	virtual	VOID CycleOperation();
	virtual	VOID StopOperation();

	VOID GetSensorValue();
	VOID GetEncoderValue();
	VOID GetAxisInformation();	//NuMotion

	//=============================================================================
	BOOL IsMotorPowerOn(const CString& szAxis, CMSNmAxisInfo* pAxisInfo);
	INT SetMotorPower(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, BOOL bPowerOn);
	LONG GetEncoder(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, const double dResolution);
	BOOL IsHomeSensorHigh(const CString& szAxis, CMSNmAxisInfo* pAxisInfo);

	INT Gripper_X_Home(const BOOL bSelMotor, const CString &szAxis, const CString& szProfileID, CMSNmAxisInfo *pAxisInfo, BOOL &bMotorHomed);
	BOOL IsMotorMissingStep(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, CString szProfile = "");
	BOOL HomeGripperMotor(const CString& szAxis);
	BOOL IsReachTriggerThetaHomeLimit(const CString& szAxis);
	VOID ResetContTriggerThetaHomeCounter(const CString& szAxis);
	VOID DisplayMissingStepError(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, BOOL bIsSync, CString szProfile);
	INT MotorMoveTo(const BOOL bSelMotor, const CString& szAxis, const CString &szProfileID, CMSNmAxisInfo* pAxisInfo,
					INT nPos, INT nMode, BOOL &bMotorHomed, BOOL bUseEncoder = FALSE, ULONG ulTimeout = 10000, BOOL bMissingStepCheck = TRUE);
	INT SubMotorMoveTo(const BOOL bSelMotor, const CString& szAxis, const CString &szProfileID, CMSNmAxisInfo* pAxisInfo,
					INT nPos, INT nMode, BOOL &bMotorHomed, BOOL bUseEncoder, ULONG ulTimeout);
	INT MotorSync(const BOOL bSelMotor, const CString& szAxis, CMSNmAxisInfo* pAxisInfo, const CString szProfileID = "", BOOL bUseEncoder = FALSE, ULONG ulTimeout = 10000);
	INT SubMotorSync(const BOOL bSelMotor, const CString& szAxis, CMSNmAxisInfo* pAxisInfo, BOOL bUseEncoder, ULONG ulTimeout);
	
	BOOL Upper_IsPowerOn();
	BOOL Lower_IsPowerOn();
	BOOL X_IsPowerOn();
	BOOL X2_IsPowerOn();
	BOOL Arm_IsPowerOn();
	BOOL Z_IsPowerOn();
	BOOL Theta_IsPowerOn();

	INT SetLoaderXPower(BOOL bSet);
	INT SetLoaderX2Power(BOOL bSet);
	INT SetLoaderZPower(BOOL bSet);
	INT SetLoaderTPower(BOOL bSet);

	LONG GetGripperCommandPosition();
	LONG GetGripper2CommandPosition();
    LONG GetGripperEncoder();
    LONG GetGripper2Encoder();

	BOOL IsGripperMotionComplete();
	VOID GripperMotionStop();
	//=============================================================================

	INT Z_Home();
	INT Z_SerachUpperLimitSensor();
	INT Z_SerachHomeLimitSensor();
//	INT Z_Home_AutoLine();		//v4.56A1
	INT X_Home();
	INT X2_Home();
	INT Arm_Home();

	INT Z_Profile(INT nProfile=BL_NORMAL_PROF);		
	INT Z_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Z_SMoveTo(INT nPos, INT nMode = SFM_WAIT);	
	INT Z_Sync_Normal(BOOL bCheckCover = FALSE);
	INT	Z_Sync(BOOL bCheckCover = FALSE);
	INT	Z_Sync_Auto();		//v4.50A23

	INT X_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT	X_Sync();
	INT X2_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT	X2_Sync();

	INT Arm_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT	Arm_Sync();


	//MS50 motor fcns	//v4.59A40
	INT Theta_Home();
	INT Theta_SlowMoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT	Theta_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Theta_Sync();


	INT Upper_Home();
	INT	Upper_MoveTo(INT nPos, INT nMode = SFM_WAIT, BOOL bMissingStepCheck = TRUE);
	INT Upper_Sync();


	INT Lower_Home();
	INT	Lower_MoveTo(INT nPos, INT nMode = SFM_WAIT, BOOL bMissingStepCheck = TRUE);
	INT Lower_Sync();

	VOID InitVariable(VOID);
	BOOL LoadBinLoaderOption(VOID);
	BOOL LoadData(VOID);
	BOOL LoadBinLoaderRunTimeData(VOID);
	BOOL SaveData(VOID);
	BOOL GenerateConfigData();				//v3.85
    BOOL LoadMgznOMData(VOID);
    BOOL SaveMgznOMData(VOID);
    
	BOOL LoadMgznRTData(VOID);
    BOOL SaveMgznRTData(VOID);
	CCriticalSection	m_CSRTMgznCurrent;

//	barcode functions
	UCHAR GetBCModel();
    BOOL SaveBarcodeData(LONG lBlkNo, CString szBarcode, LONG lMgzNo=0, LONG lSlotNo=0);
    CString GetBarcodeData(LONG lBlkNo);	//v3.65		Semitek

	BOOL CreateBarcodeScanner(VOID);
	LONG StartBarCodeOnTable(BOOL bStart);		//v2.83T6

	LONG ScanningBarcode(CBinGripperBuffer *pGripperBuffer, BOOL bScanTwice = FALSE);	//v4.51A18
	LONG ScanningBarcodeOnBufferTable(VOID);	//v2.68
	LONG ScanningBarcodeOnBufferTableCCD(VOID);	//v2.74

	//For MS100 9Inch dual-table config
	BOOL CreateBarcodeScanner2(VOID);
	LONG ScanningBarcode2(VOID);

	LONG ExArmScanBarcodeOnGripper();
	LONG StartBarcodeScanner(BOOL bOn);
	LONG ReadBarcodeScanner(CString& szBarcode);

	//Existing bar code function for Symbol
	BOOL MoveGripperScanningPosn(CBinGripperBuffer *pGripperBuffer, INT siDirection, LONG lScanRange);
	LONG MoveGripperScanningByBarcode(CBinGripperBuffer *pGripperBuffer, const BOOL bScanTwice, 
											  const INT siDirection, const LONG lScanRange, 
											  BOOL &bPerform2ndScan, CString &sz1stScanBC, BOOL &bResult);
	LONG ScanningBySymbol(CBinGripperBuffer *pGripperBuffer, BOOL bScanTwice=FALSE);		//v4.51A18
	LONG ScanningBySymbol2(VOID);
	BOOL ReadBarcode(CString *szData, int nTimeOut = 100);
	BOOL ReadBarcode2(CString *szData, int nTimeOut = 100);

	//New bar code function for keyence
	LONG ScanningByKeyence(VOID);
	BOOL StartBarcodeKeyence(BOOL bStart);
	BOOL ReadBarcodeValueKeyence(CString *szData);

	LONG ScanningByCCD();			
	BOOL StartBarcodeCCD(BOOL bStart);
	BOOL ReadBarcodeValueCCD(CString *szData);

	LONG ScanningByRegal(VOID);
	BOOL StartBarcodeRegal(BOOL bStart);
	BOOL ClearComContentRegal();
	BOOL ReadBarcodeValueRegal(CString *szData, int nTimeout =100, int nRetry = 10);

	//Manual input 
	BOOL ManualInputBarcode(CString *szBarcode);

	BOOL AlertMsgForManualInputBarcode(LONG lCurrentBlock, LONG lMagzNo, LONG lSlotNo, CString& szBarcodeName, CString szErrMsg, 
									   BOOL bEnableInputBarcodeSelection = TRUE, BOOL bAllowEmptyBarcode = TRUE);
	BOOL AlertMsgForManualInputBarcode_NoSaveToRecord(CString& szBarcodeName, CString szErrMsg,
													  BOOL bEnableInputBarcodeSelection = TRUE, BOOL bAllowEmptyBarcode = TRUE);
	
    LONG BarcodeTest(IPC_CServiceMessage& svMsg);
    LONG Barcode2Test(IPC_CServiceMessage& svMsg);

	//BIn BARCODE Summary file fcns
	BOOL CheckBcHistoryInCurrLot(CONST LONG lBlock, CONST CString szBCName, CONST BOOL bLoad, CString& szErrCode);	//v2.83T70	//v3.70T3	//PLLM&Cree
	BOOL BL_CheckBcInMgzs(CONST LONG lBlock, CONST CString szBCName, BOOL bBURNIN=FALSE, BOOL bUseBT2=FALSE);
	LONG CompareBarcodeFromEmptyMgzs(CONST LONG lBlock, CONST CString szBCName, BOOL bBURNIN=FALSE, BOOL bUseBT2=FALSE, BOOL bCheckEmptyFrame = FALSE);
	BOOL CheckBarcodeInMES(CONST LONG lBlock, CONST CString szBCName);		//v4.42T16
	BOOL CheckBarcodeCharacters(CString szBarcode);
	BOOL CopyCreeBinBcSummaryFile(); //v3.75		//Cree (China) only 
	BOOL ValidateBinFrameBarcode(CString szBarcode);
//	barcode functions

	VOID SetGripperState(CBinGripperBuffer *pGripperBuffer, BOOL bSet);
    VOID SetGripperState(BOOL bSet);
    VOID SetGripper2State(BOOL bSet);
	VOID SetGripperLevel(BOOL bSet);
	VOID SetGripper2Level(BOOL bSet);
	VOID SetGripperPusher(BOOL bSet);
	VOID SetFrameVacuum(BOOL bSet);
	VOID SetFrameVacuum2(BOOL bSet);
	VOID SetFrameLevel(BOOL bSet);
	BOOL IsFrameLevelOn();
	VOID SetFrameLevel2(BOOL bSet);
	VOID SetFrameAlign(BOOL bSet);
	VOID SetFrameAlign2(BOOL bSet);
	VOID SetBufferAlign(BOOL bSet);
	VOID SetFontArmLevel(BOOL bSet);
	VOID SetFontArmVacuum(BOOL bSet);
	VOID SetRearArmLevel(BOOL bSet);
	VOID SetRearArmVacuum(BOOL bSet);
	BOOL SetBHFrontCoverLock(CONST BOOL bOpen);		//v2.67
	BOOL SetBHSideCoverLock(CONST BOOL bOpen);		//v2.67
	VOID SetBinElevatorCoverLock(BOOL bSet);		//v4.36		//PLLM MS109
	VOID SetSideCoverLock(BOOL bSet);				//v4.42T4	//PLLM MS109
	VOID SetBufferLevel(BOOL bSet); //Dual Arm Buffer Table Sensors
	VOID SetALFrontGate(BOOL bSet);
	VOID SetALBackGate(BOOL bSet);
//	VOID SetALMgznClamp(BOOL bSet);
	VOID SetMS90BufPusherZ(BOOL bSet);
	VOID SetMS90BufPusherX(BOOL bSet);		//v4.59A40

	BOOL IsFrontGateSensorOn();
	BOOL IsBackGateSensorOn();

	BOOL IsFrameOnBTPosition(VOID); // on bin table, at right side
	BOOL IsFrameOnBT2Position(VOID); // on bin table, at right side
	//BOOL IsFrameOnBTDetect(VOID); // ?? on bin table, at left side ??

	BOOL IsFrameJam(const CString szBL_SI_FrameJam);
	BOOL IsFrameJam(VOID);			// jam sensor on gripper clamp
	BOOL IsFrameJam2(VOID);			// jam sensor on gripper2 clamp	(MS100 9Inch)

	BOOL IsFrameInClamp(const CString szBL_SI_FrameDetect);
	BOOL IsFrameInClamp(VOID);		// frame in gripper clamp
	BOOL IsFrameInClamp2(VOID);		// frame in gripper clamp

	BOOL IsFrameInMagazine(VOID);	// where the sensor
    BOOL IsMagazineExist(LONG lMagazine, BOOL bUseBT2=FALSE, BOOL bNeedByPassDisableAlarm=FALSE); //v4.41T2		// for load/unload frame only, show warning message
	BOOL IsAllMagazineExist(CONST BOOL bNeedByPassDisableAlarm = FALSE);
	BOOL IsBufferSafetySensor();
	BOOL IsElevatorCoverOpen();
	BOOL IsElevatorDoorCoverOpen();
	BOOL IsFrontRightElevatorDoorCoverOpen();

	BOOL IsCoverOpen();

	BOOL IsFrameProtection(VOID);		// for what
	BOOL IsFrameProtection2(VOID);		// for what
	BOOL IsFrameOutOfMgz();
	BOOL IsRearArmFrameExist();
	BOOL IsFrontArmFrameExist();
	BOOL CheckFrameExistByVacuum();		//v3.57T3
	BOOL CheckFrameExistByVacuum2();
	BOOL IsExchangeArmReady();
	BOOL IsBondArmSafe();
	BOOL IsLimitSensorHit();			//v3.57
	BOOL IsBinElevatorCoverLocked();	//v4.39T10 //Cree US	//For cover lock option only
	BOOL IsPressureSensorAlarmOn();		//v4.59A38
	
	//Dual Buffer Table Sensors
	BOOL IsDualBufferExist(const CString szBL_SI_DualBufferExistName);
	BOOL IsDualBufferUpperExist();
	BOOL IsDualBufferLowerExist();
	BOOL IsDualBufferRightProtect();
	BOOL IsDualBufferLeftProtect();
	BOOL IsDualBufferLevelDown();
	//v4.59A40	//MS90 New table Pusher XZ for Finisar TX
	BOOL IsMS90BufPusherZAtSafePos();
	BOOL IsMS90BufPusherXAtHomePos();
	BOOL IsMS90BufPusherXAtLimitPos();
	BOOL CheckMS90BugPusherAtSafePos(BOOL bForceCheck=FALSE);

	//Get data from other station
	VOID SetBinTableJoystick(BOOL bState, BOOL bUseBT2=FALSE);
	BOOL MoveBinTableThetaToZero();
	BOOL MoveBinTable(LONG lXAxis, LONG lYAxis, INT nToUnload=0, BOOL bWait=TRUE);
	BOOL MoveBinTable2(LONG lXAxis, LONG lYAxis, INT nToUnload=0, BOOL bWait=TRUE);
	BOOL MoveWaferTableToSafePosn(CONST BOOL bSafe, 
						CONST BOOL bOffline=TRUE, CONST BOOL bWaitWT=TRUE);		//v3.61	//v4.53A21
	// huga
    BOOL RealignBinFrame(ULONG ulBlkID, BOOL bUseBT2=FALSE);
    BOOL RealignBinFrameReq(ULONG ulBlkID, BOOL bUseEmpty=FALSE, BOOL bUseBT2=FALSE);		//v4.37T5	//v2.72a5	//v2.93T2
    BOOL RealignBinFrameRpy(CONST BOOL bCheckOnce=FALSE, CONST BOOL bSetBLReady=TRUE);		//v2.72a5	//v2.93T2
	BOOL ResetFrameIsAligned(ULONG ulBlkID);		//v4.42T10
	INT	m_nExArmReAlignBinConvID;					//v2.72a5
	BOOL ClearBinFrameCounter(ULONG ulBlkID, CString szBarCodeName, BOOL bHouseKeeping=TRUE);
	ULONG GetBinCount(ULONG ulBlkID);
	VOID GetBinTableEncoder(LONG *lXAxis, LONG *lYAxis);
	VOID GetBinTable2Encoder(LONG *lXAxis, LONG *lYAxis);
    BOOL IsToStopTest(VOID);
	BOOL HomeBinTable(ULONG ulTable);				//v4.17T6

	BOOL IsAllMotorsEnable();
	BOOL IsMagazineSafeToMove();
	BOOL IsMagazine2SafeToMove();
	BOOL IsGripperSafeToMove();		
	LONG CheckFrameOnBinTable(VOID);
	LONG CheckFrameOnBinTable2(VOID);
	BOOL CheckFrameOnBufferTable(VOID);
	BOOL CheckElevatorCover(CONST BOOL bStop=FALSE);
	BOOL CheckCover(CONST BOOL bStop=FALSE);

	LONG GetPreUnloadPosn(CBinGripperBuffer *pGripperBuffer);
	LONG GetReadyPosn(CBinGripperBuffer *pGripperBuffer);
	LONG GetLoadMagPosn(CBinGripperBuffer *pGripperBuffer);
	LONG GetUnloadMagPosn(CBinGripperBuffer *pGripperBuffer);
	LONG GetBarcodePosn(CBinGripperBuffer *pGripperBuffer);
	LONG GetUnloadPosn(CBinGripperBuffer *pGripperBuffer);

	INT SetGripperPower(CBinGripperBuffer *pGripperBuffer, BOOL bOn);

	BOOL IsGripperFrameExist(const CString szBL_SI_DualBufferExistName);
	VOID GripperMotionStop(CBinGripperBuffer *pGripperBuffer);
	VOID GripperHouseKeeping(CBinGripperBuffer *pGripperBuffer,
							 BOOL bEnableBT = TRUE, BOOL bIsMissingStep = FALSE, BOOL bByPassOutOfMagCheck = FALSE);

    LONG BinGripperMoveTo(INT nPos, BOOL bReadBarCode=FALSE);
    LONG BinGripper2MoveTo(INT nPos, BOOL bReadBarCode=FALSE);
	LONG BinGripperSearchScan(INT siDirection, LONG lSearchDistance);
	LONG BinGripper2SearchScan(INT siDirection, LONG lSearchDistance);

	LONG BinGripperSearchJam(INT iDirection, LONG lSearchPosition, LONG lSearchVelocity=850, BOOL bWait=SFM_WAIT);
	LONG BinGripperSearchBTPosition(INT iDirection, LONG lSearchPosition, LONG lSearchVelocity=850, BOOL bWait=SFM_WAIT);
    LONG BinGripperSearchInClamp (INT iDirection, LONG lSearchDistance, LONG lDriveInDistance = 400, BOOL bWaitComplete=SFM_WAIT, BOOL bReadBarcode=FALSE, BOOL bSync = TRUE);
    LONG BinGripper2SearchInClamp(INT iDirection, LONG lSearchDistance, LONG lDriveInDistance = 400, BOOL bWaitComplete=SFM_WAIT, BOOL bReadBarcode=FALSE, BOOL bSync = TRUE);
    LONG BinGripperMoveTo_Auto (INT nPos, INT nWait);
	LONG BinUpperGripperSynMove_Auto();
    LONG BinGripper2MoveTo_Auto(INT nPos, INT nWait);
    LONG BinGripperSynMove_Auto (VOID);
    LONG BinGripper2SynMove_Auto(VOID);
	LONG BinGripperMoveSearchJam (INT nPos, BOOL bCheckRealign=FALSE, BOOL bReadBarCode=FALSE, BOOL bUnload = FALSE);			//pllm	//v4.02T6
	LONG BinGripper2MoveSearchJam(INT nPos, BOOL bCheckRealign=FALSE, BOOL bReadBarCode=FALSE, BOOL bUnload = FALSE);
	LONG CheckIfGripperUnloadAlarm(BOOL bUnload);
	LONG CheckIfGripper2UnloadAlarm(BOOL bUnload);
	LONG ExArmBinGripperMoveSearchJam(INT nPos, BOOL bCheckBinAlign, BOOL& bBinAlignRpy);				//v2.83T2

	LONG GripperMoveSearchJam(CBinGripperBuffer *pGripperBuffer,
							  INT nPos, BOOL bCheckReAlign = FALSE, BOOL bReadBarCode = FALSE, BOOL bUnload = FALSE);
	LONG GripperSearchInClamp(CBinGripperBuffer *pGripperBuffer,
							  INT siDirection, LONG lSearchDistance, LONG lDriveInDistance = 400,
							  BOOL bWaitComplete = SFM_WAIT, BOOL bReadBarcode = FALSE, BOOL bSync = TRUE);
	LONG GripperSearchScan(CBinGripperBuffer *pGripperBuffer, INT siDirection, LONG lSearchDistance);
	LONG GripperMoveTo(CBinGripperBuffer *pGripperBuffer, INT nPos, BOOL bReadBarCode = FALSE);
	LONG GripperMoveToReadyPosn(CBinGripperBuffer *pGripperBuffer, BOOL bWait);
	LONG GripperMoveToPreUnloadPosn(CBinGripperBuffer *pGripperBuffer, BOOL bWait);
	VOID HomeGripper(CBinGripperBuffer *pGripperBuffer);
	BOOL GripperSyncMove_Auto(CBinGripperBuffer *pGripperBuffer);
	LONG GripperSync(CBinGripperBuffer *pGripperBuffer);

    LONG BL_DEBUGBOX(CString str); // for step mode only
    LONG BL_DEBUGMESSAGE(CString str); // show display message window
    LONG BL_OK_CANCEL(CString strText, CString strTitle);
    LONG BL_YES_NO(CString strText, CString strTitle);

	LONG BL_OK_CANCEL(ULONG ulAppMsgCode, ULONG ulHmiMsgCode, BOOL bDiableOkButton = FALSE);
	LONG BL_YES_NO(ULONG ulAppMsgCode, ULONG ulHmiMsgCode, BOOL bDisableOkButton = FALSE, BOOL bNoMaterial = FALSE);

    //LONG BL_WARNBOX(CString str);
    //LONG BL_WARNBOX(CString str, LONG l);

	//==========================AutoLine===================================================================================
	BOOL InitMSAutoLineBLModule();
	virtual VOID UpdateAllSGVariables();
	BOOL SECS_InitCassetteSlotInfo();
	LONG GetSlotIndex(const LONG lMgznNo, const LONG lSlotNo);
	BOOL SECS_UpdateCassetteSlotInfo(CONST LONG lMgznNo, CONST LONG lSlotNo, BOOL bIsLoad = FALSE);
	BOOL SECS_UpdateLoadUnloadStatus(CONST LONG lBlock, LONG lMagzNo, LONG lSlotNo, BOOL bIsLoad, LONG lStatus);
	VOID SECS_UpdateInputSlotGradeWIPCounter();
	//AutoLine SECS/GEM Event
	CString GetCassettePositionName(const LONG lMgzn);
	VOID SendResetSlotEvent_7018(const LONG lMgzn, const LONG lSlot, BOOL bResetAll = FALSE);
	VOID SendGripperEvent_7050(const LONG lPreUnloadPos_X, const LONG lUnloadPos_X, const LONG lLoadPos_X, const LONG lReadyPos_X,
							   const LONG lBarcodePos_X, const LONG lBTUnloadPos_X, const LONG lBTUnloadPos_Y);
	VOID SendMagazineEvent_7051(const LONG lMagazineNo, const LONG lReadyPosY, const LONG lReadyLvlZ);
	VOID SendBarcodeEvent_7052(const BOOL bUseBarcode, const LONG lTryLimits, const LONG lScanRange,
							   const BOOL bUseExtension, const CString szExtName);
	VOID SendOMRuntimeEvent_7053(const LONG lOMRT);
	VOID SendBinTableLoadEvent_8010(const LONG lMgznNo, const LONG lSlot, const CString szBCName, const CString szCassettePos,
									const CString szCassetteType);
	VOID SendBinTableUnloadEvent_8011(const LONG lMgzn, const LONG lSlot, const CString szCassettePos);
	VOID SendBinTableUnloadEvent_8011(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassettePos, const LONG lWIPCountNo);
	VOID SendBinTableUnloadCompletedEvent_8012(const LONG lMgzn, const LONG lSlot, const CString szCassettePos);
	VOID SendBinTableUnloadCompletedEvent_8012(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassettePos);
	VOID SendBinCasseteLoadWIPEvent_8014(const LONG lMgzn, const LONG lSlot, const CString szCassettePos);
	VOID SendBinCasseteLoadWIPEvent_8014(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassettePos);
	VOID SendBinCasseteUnloadWIPEvent_8015(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassettePos);
	VOID SendClearBin_8018(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassetePos, const ULONG ulBlkID,
						   const ULONG ulDieCount);
		VOID SendBinTransferEvent_8024(const LONG lPreMgzn, const LONG lPreSlot, const LONG lCurMgzn, const LONG lCurSlot);
	VOID SendBarcodeRejectEvent_8026(const LONG lCurMgzn, const LONG lCurSlot, const LONG lRejectCode);
	VOID SetSlotRejectStatus(const LONG lCurrMgzn, const LONG lCurSlot, const LONG lRejectCode, const CString szBarCode);
	//=====================================================================================================================

	BOOL IsInputSlot_AutoLine(const LONG lMgzn, const LONG lSlot);
	BOOL IsOutputSlot_AutoLine(const LONG lMgzn, const LONG lSlot);
	BOOL GetGradeSlot_AutoLine(const LONG lBlock, LONG &lMgzn, LONG &lSlot);
	LONG GetAvailableInputSlot_AutoLine(const LONG lCurrMgzn);
	BOOL GetAvailableInputSlot_AutoLine(LONG &lMgzn, LONG &lSlot);
	LONG GetMgzSlot_AutoLine(const LONG lBlock, LONG &lMgzn, LONG &lSlot);
    LONG GetLoadMgzSlot_AutoLine(const LONG lPhysicalBlock, BOOL &bUseEmpty);

	BOOL IsWIPSlot_AutoLine(const LONG lMgzn, const LONG lSlot);
	BOOL IsWIPFullActiveSlot_AutoLine(const LONG lMgzn, const LONG lSlot);
	BOOL IsWIPActiveSlot_AutoLine(const CString szBCName, const LONG lGradeBlock, const LONG lMgzn, const LONG lSlot);
	BOOL GetWIPSlotWithStatus_AutoLine(const LONG lSlotUsage, LONG &lMgzn, LONG &lSlot);
	BOOL GetWIPSlotWithStatus_AutoLine(const LONG lSlotUsage, const CString szBCName, const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot);
	BOOL GetWIPFullSlot_AutoLine(LONG &lMgzn, LONG &lSlot);
	BOOL GetWIPActiveSlot_AutoLine(LONG &lMgzn, LONG &lSlot);
	BOOL GetWIPEmptySlot_AutoLine(const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot);
	BOOL IsWIPSortingSlotExisted_AutoLine(const LONG lGradeBlock);
	BOOL GetWIPSortingSlot_AutoLine(const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot);
	BOOL IsSlotGradeInSet_AutoLine(const LONG lGradeBlock, const BOOL bASCIIGradeDisplayMode, CUIntArray &aulSelectdGradeList, CUIntArray &aulSelectdGradeLeftDieCountList);
	BOOL GetWIPActiveSlotWithoutGradeList_AutoLine(const BOOL bASCIIGradeDisplayMode, CUIntArray &aulSelectdGradeList, CUIntArray &aulSelectdGradeLeftDieCountList, LONG &lMgzn, LONG &lSlot);
	BOOL GetWIPEmptySortingSlotWithGrade_AutoLine(const CString szBCName, const LONG lBlock, LONG &lMgzn, LONG &lSlot);
	LONG GetUnloadMgznSlot_AutoLine(const CString szBCName, const LONG lBlock, LONG &lMgzn, LONG &lSlot);
	LONG GetUnloadMgznSlotWithUpdate_AutoLine(const CString szBCName, const LONG lPhysicalBlock, const ULONG ulUnloadDieGradeCount, LONG &lMgzn, LONG &lSlot);		//v4.59A11

	LONG GetNullMgznSlot_AutoLine(const LONG lPhysicalBlock, LONG &lMagazine, LONG &lSlot);	//v4.59A12

	BOOL GetOutputSlotWithStatus_AutoLine(const LONG lSlotUsage, LONG &lMgzn, LONG &lSlot);
	BOOL GetOutputSlotWithStatus_AutoLine(const LONG lSlotUsage, const CString szBCName, const LONG lSlotGrade, LONG &lMgzn, LONG &lSlot);
	BOOL GetOutputSlotWithStatusBarcode_AutoLine(const LONG lSlotUsage, const CString szBarcode, LONG &lMgzn, LONG &lSlot);
	BOOL IsOutputEmptySlot_AutoLine();
	BOOL GetAvailableOutputSlot_AutoLine(const CString szBCName, const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot);
	BOOL GetEmptyOutputSlot_AutoLine(const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot);
	BOOL GetEmptyOutputWIPSlot_AutoLine(const CString szBCName, const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot);
    LONG GetFullMgznSlot_AutoLine(LONG &lMagazine, LONG &lSlot);
	LONG GetFullMgznSlotWithUpdate_AutoLine(const BOOL bClearAllFrameTOP2Mode, const LONG ulUnloadDieGradeCount, const BOOL bTransferWIPSlot, LONG &lMgzn, LONG &lSlot);

	VOID UpdateBTCurrMgznSlot(const LONG lCurrMgzn, const LONG lCurrSlot);
	BOOL TransferSortingMgznSlotFromWIPToOutput_AutoLine();
	BOOL AdjustOutputSlotSortingPosn_AutoLine();
	LONG TransferMgznSlot_AutoLine(LONG lMgzn, LONG lSlot, LONG lNewMgzn, LONG lNewSlot);
	LONG TransferBondingMgznSlot_AutoLine(LONG lMgzn, LONG lSlot, LONG lNewMgzn, LONG lNewSlot);
	LONG CreateWIPOutputFileWithBarcode(const LONG lCurrMgzn, const LONG lCurrSlot);
	LONG CreateWIPTempFile(const LONG lCurrMgzn, const LONG lCurrSlot);
	LONG GetNoOfSortedDie(const UCHAR lBlkId);
	CString GetCustomOutputFileName();
	BOOL HaveEmptyOutputSlot_AutoLine();
	BOOL IsFullOutputSlot_AutoLine();

	BOOL IsFullSlotInMgzn(const LONG lMgzn);
	BOOL IsClearFrameOutputFullMgazSlot_AutoLine(LONG &lMgzn, LONG &lSlot);
	BOOL GetClearFrameOutputSlotWithStatus_AutoLine(const LONG lSlotUsage, LONG &lMgzn, LONG &lSlot);
	BOOL GetClearFrameOutputSlotWithStatus_AutoLine(const LONG lSlotUsage, const CString szBCName, const LONG lSlotGrade, LONG &lMgzn, LONG &lSlot);
	BOOL GetAvailableClearFrameOutputSlot_AutoLine(const CString szBCName, const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot);
	LONG GetClearFrameFullMgznSlot_AutoLine(LONG &lMgzn, LONG &lSlot);

	LONG GetCassetteSlotGradeBlock(const LONG lMgzn, const LONG lSlot);
	LONG GetCassetteSlotWIPCounter(const LONG lMgzn, const LONG lSlot);
	LONG GetCassetteSlotUsage(const LONG lMgzn, const LONG lSlot);
	CString GetCassetteSlotBCName(const LONG lMgzn, const LONG lSlot);
	CString GetCassetteSlotSN(const LONG lMgzn, const LONG lSlot);
	CString GetCassetteSlotLotNo(const LONG lMgzn, const LONG lSlot);
	VOID SetCassetteSlotGradeBlock(const LONG lMgzn, const LONG lSlot, const LONG lGradeBlock);
	VOID SetCassetteSlotUsage(const LONG lMgzn, const LONG lSlot, const LONG lSlotUsageStatus);
	VOID SetCassetteSlotBCName(const LONG lMgzn, const LONG lSlot, const CString szSlotBCName);
	VOID SetCassetteSlotStatus(const LONG lMgzn, const LONG lSlot, const LONG lSlotGradeBlock,
							   const LONG lSlotWIPCounter, const LONG lSlotUsageStatus,
							   const CString szSlotBCName,
							   const CString szSlotSN, const CString szSlotLotNo, const BOOL bCopy = FALSE);
	BOOL IsTransferFullWIPtoOutput();
	//=====================================================================================

    LONG GetLoadMgzSlot(BOOL &bUseEmpty, LONG lPhysicalBlock, BOOL bUseBT2=FALSE, BOOL bDisableModeHSkipSlot=FALSE);
    LONG GetMgznSlot(LONG lPhysicalBlock, LONG& lMgzn, LONG& lSlot);
	
	LONG ClearAllFrameResetNullMagazine();
	LONG GetNullMgznNo(LONG& lMgzn, CONST LONG lMgznToStart=0);
	LONG GetNullMgznSlot(LONG &lMagazine, LONG &lSlot, LONG lBlock);
	LONG GetNextNullMgznSlot(LONG &lMagazine, LONG &lSlot);		//v2.93T2
	BOOL RestoreNullMgznSlot(LONG lMgzn, LONG lSlot);			//v4.39T8	
	
	BOOL ResetFullMagazine(LONG lMgzn);
	LONG ClearAllFrameResetFullMagazine();
	LONG GetFullMgznNo(LONG &lMgzn, CONST LONG lMgznToStart=0);
	LONG GetNextFullMgazSlot(LONG &lMgzn, LONG &lSlot);
	LONG GetFullMgznSlotWithoutUpdateSlotInfo(LONG &lMgzn, LONG &lSlot, BOOL bDisableFullCheck = FALSE, LONG lPhysicalBlock = 0, BOOL bTransferWIPSlot = FALSE);
    LONG GetFullMgznSlot(LONG &lMagazine, LONG &lSlot, 
							BOOL bDisableFullCheck = FALSE,	
							LONG lPhysicalBlock = 0	);

	BOOL GetFullMgznSlot_Renesas(LONG &lMagazine, LONG &lSlot,
									BOOL bDisableFullCheck=FALSE,
									LONG lPhysicalBlock=0);			//v4.59A20	

    VOID SeparateClamp_BT(VOID);
    VOID SettleDownBeforeUnload(BOOL bOffline=TRUE);
	VOID AlignBinFrame();
	LONG GetThetaLoadPosn(const LONG lMgzn);
	VOID StopZMotion();
	BOOL CheckSafeToMove(const BOOL bAskCoverOpen = FALSE);
	BOOL CheckCoverOpen();
	LONG MoveElevatorToLoad(LONG lMgzn, LONG lSlot, CBinGripperBuffer *pGripperBuffer, BOOL bUseBT2 = FALSE, BOOL bMoveGripperToReady = TRUE);
	LONG MoveElevatorToNextLoad(LONG lMgzn, LONG lSlot, BOOL bBufferLevel, BOOL bWait = TRUE, BOOL bUseBT2 = FALSE);
	LONG MoveElevatorToUnload(LONG lMgzn, LONG lSlot, BOOL bBufferLevel, BOOL bMoveOffset = TRUE, BOOL bWait = TRUE, BOOL bDisableOutOfMgznCheck = FALSE, BOOL bUseBT2 = FALSE);
    LONG DownElevatorToReady(CBinGripperBuffer *pGripperBuffer, BOOL bWait = TRUE);
	BOOL YZ_Sync_OutOfMagChecking(BOOL bCheckRealignRpy=FALSE);		//v4.49A6
    //LONG MoveElevatorToLoad_Auto(LONG lMagazine, LONG lSlot);
    //LONG MoveElevatorToUnload_Auto(LONG lMagazine, LONG lSlot, BOOL bMoveOffset=TRUE);
    LONG ElevatorZMoveTo_Auto(LONG lZLevel);
    LONG DownElevatorToReady_Auto(VOID);
    LONG ElevatorZSynMove_Auto(VOID);
	BOOL AbortElevatorMotionIfOutOfMag();		//v4.18T1	

    LONG LoadFrameFromSlotToSlot(BOOL bBurnIn, LONG lFromMgz, LONG lFromSlot, LONG lToMgz, LONG lToSlot);
	LONG UnloadBinFrame(BOOL bBurnInUnload, CBinGripperBuffer *pGripperBuffer, BOOL bBinFull, BOOL bOffline=FALSE, BOOL bClearAllMode=FALSE, BOOL bClearBin=TRUE, BOOL bCheckRealignRpy=FALSE, BOOL bTransferWIPSlot = FALSE);
//	LONG SubUnloadBinFrame(BOOL bBurnInUnload, BOOL bBinFull, BOOL bOffline, BOOL bClearAllMode, 
//						   BOOL bClearBin, BOOL bCheckRealignRpy, BOOL bTransferWIPSlot);
	
	LONG LoadFrameToFullMgz(CBinGripperBuffer *pGripperBuffer,LONG lLoadBlock, BOOL bBinFull, BOOL bOffline = FALSE, BOOL bClearAllMode = FALSE);

	LONG TransferWIPBinFullFrame(BOOL bBurnInLoad, LONG lLoadBlock, BOOL bOffline = FALSE, BOOL bClearAllMode = FALSE, BOOL bDirectUnload = FALSE, 
							 BOOL bDoNotHomeafterLoad=FALSE, BOOL bBinFullLoadEmpty = FALSE, BOOL bManualChangeFrame = FALSE);
	LONG LoadBinFrame(BOOL bBurnInLoad, CBinGripperBuffer *pGripperBuffer, LONG lLoadBlock, BOOL bOffline = FALSE, BOOL bClearAllMode = FALSE, BOOL bDirectUnload = FALSE, 
					  BOOL bDoNotHomeafterLoad=FALSE, BOOL bBinFullLoadEmpty = FALSE, BOOL bManualChangeFrame = FALSE);
	LONG SubLoadBinFrameWithLock(BOOL bBurnInLoad, CBinGripperBuffer *pGripperBuffer, LONG lLoadBlock, BOOL bOffline, BOOL bClearAllMode, 
								 BOOL bDirectUnload, BOOL bDoNotHomeafterLoad, BOOL bBinFullLoadEmpty,
								 BOOL bManualChangeFrame, LONG lTransferWIPSlotMode = TRANSFER_WIP_NONE_FRAME);
	BOOL WaitForAutoLineToRemoveOutputWIPFrame(const BOOL bManualChangeFrame);
	BOOL WaitForAutoLineToInputFrame();
//	LONG SubLoadBinFrame(BOOL bBurnInLoad, LONG lLoadBlock, BOOL bOffline, BOOL bClearAllMode, 
//						 BOOL bDirectUnload, BOOL bDoNotHomeafterLoad, BOOL bBinFullLoadEmpty,
//						 BOOL bManualChangeFrame, LONG lTransferWIPSlotMode);

	LONG PushBackFrameFromGripper_DL(BOOL bBurnInUnload);		//v3.75
	LONG PushBackFrameFromGripper2_DL(BOOL bBurnInUnload);
	LONG PushBackFrameFromGripper(LONG lLoadBlock, BOOL bBurnInUnload, LONG nMgzn,LONG nSlot,BOOL bToFullMagzn);
	LONG PushBackFrameFromBinTable(LONG lLoadBlock, BOOL bBurnInUnload, LONG nMgzn,LONG nSlot,BOOL bToFullMagzn,BOOL bOffline=FALSE);
	LONG CheckFramePosOnBinTable(BOOL bBurnInUnload, BOOL bOffline);
	LONG NoBarCodeHandler(LONG lLoadBlock, BOOL bBurnInLoad,LONG lMgzn, LONG lSlot,BOOL bUseEmpty,BOOL bOffline=FALSE, BOOL bClearAllMode=FALSE,BOOL bFromBinTable=FALSE,BOOL bBT2=FALSE);

	BOOL UpdateExArmMgzSlotNum(LONG lBlock, CString szBCName = "");			//v2.67

	//Dual Buffer Load/Unload Functions
//	LONG DB_LoadFromMgzToBuffer(BOOL bBurnInLoad, LONG lLoadBlock, BOOL bClearAllMode, BOOL bBufferLevel=BL_BUFFER_UPPER, BOOL bBinFullLoadEmpty = FALSE, BOOL bIsPreLoad = FALSE);
	LONG DB_UnloadFromBufferToMgz(BOOL bBurnInUnload, BOOL bBinFull, BOOL bClearBin, BOOL bIsEmptyFrame, BOOL bIsMgzPreMove, 
									BOOL bBufferLevel=BL_BUFFER_LOWER, BOOL bCheckRealignRpy=FALSE, BOOL bXStopAtHome = FALSE,
									BOOL bBinFullExchange=FALSE, BOOL bIsManualUnload = FALSE);	//xyz
	LONG DB_LoadFromBufferToTable(BOOL bBurnInLoad, BOOL bOffline, BOOL bBufferLevel=BL_BUFFER_UPPER, BOOL bExchangingFrame=FALSE, BOOL bPreScanBarcode=FALSE);	//andrew1234
	LONG DB_UnloadFromTableToBuffer(BOOL bBurnInLoad, BOOL bOffline, BOOL bBufferLevel=BL_BUFFER_LOWER, BOOL bExchangingFrame=FALSE);
	LONG DualBufferAutoClearBin(CBinGripperBuffer *pGripperBuffer);
	BOOL CreateOutputFileWithClearBin(LONG lMgzn, LONG lSlot, BOOL bFullStatus, ULONG ulBlkID, CString szBarCodeName);
	BOOL UpdateDualBufferUnloadFrameMagazineStatus(LONG lMgzn, LONG lSlot, BOOL bBinFull, BOOL bClearBin, BOOL lFullStatus, BOOL bIsEmptyFrame, BOOL bBurnInUnload, CBinGripperBuffer *pGripperBuffer);
	BOOL AlignBinFrameOnTable_MS90();	//v4.59A40	//Finisar TX MS90 new BT align method

	//MS50_01
	VOID BTFramePlatformUp();
	VOID BTFramePlatformDown();
	//BOOL IsBTFramePlatformDown();
	LONG UDB_SubUnloadBinFrame(BOOL bBurnInUnload, CBinGripperBuffer *pGripperBuffer, BOOL bBinFull, BOOL bOffline, 
							   BOOL bClearBin, BOOL bCheckRealignRpy, BOOL bTransferWIPSlot, BOOL bClearAllFrameTOP2Mode);

	LONG UDB_UnloadFromTableToBuffer(BOOL bBurnInLoad, BOOL bBinFull, CBinGripperBuffer *pGripperBuffer, BOOL bOffline);
	LONG UDB_UnloadFromBufferToMgzWithLock(BOOL bBurnInUnload, 
										   BOOL bBinFull, 
										   CBinGripperBuffer *pGripperBuffer,
										   BOOL bClearBin = FALSE,
										   BOOL bCheckRealignRpy = FALSE,
										   BOOL bTransferWIPSlot = FALSE);
	LONG UDB_UnloadFromBufferToMgz(BOOL bBurnInUnload, BOOL bBinFull, CBinGripperBuffer *pGripperBuffer,
								   BOOL bClearBin = FALSE, BOOL bCheckRealignRpy = FALSE, BOOL bTransferWIPSlot = FALSE, BOOL bClearAllFrameTOP2Mode = FALSE);
	LONG UDB_UnloadFromBufferToMgzSlot(BOOL bBurnInUnload, LONG lBinNo, BOOL lFullStatus, LONG lMgzn, LONG lSlot, CBinGripperBuffer *pGripperBuffer,
									   BOOL bClearBin, BOOL bCheckRealignRpy, BOOL bTransferWIPSlot, BOOL bUpdateMagazineStatus, BOOL bRealignOutputSlot);

	LONG UDB_LoadFromMgzToBufferWithLock(BOOL bBurnInLoad, 
										 LONG lLoadBlock, 
										 CBinGripperBuffer *pGripperBuffer,
										 BOOL bManualChangeFrame = FALSE);
	LONG UDB_TransferFrame(BOOL bBurnInLoad,
						   LONG lLoadBlock,
						   CBinGripperBuffer *pGripperBuffer,
						   BOOL bBinFull = FALSE,
						   BOOL bManualChangeFrame = FALSE,
						   BOOL bTransferWIPSlot = FALSE,
						   BOOL bClearAllMode = FALSE);
	LONG UDB_SubLoadFromMgzToBufferWithLock(BOOL bBurnInLoad,
											LONG lLoadBlock,
											CBinGripperBuffer *pGripperBuffer);

	LONG UDB_SubLoadBinFrame(BOOL bBurnInLoad, CBinGripperBuffer *pGripperBuffer, LONG lLoadBlock, BOOL bOffline,
							 BOOL bManualChangeFrame, LONG lTransferWIPSlotMode, BOOL bClearAllFrameTOP2Mode);
	LONG UDB_LoadFromMgzToBuffer(BOOL bBurnInLoad, LONG lLoadBlock, CBinGripperBuffer *pGripperBuffer,
								 BOOL bManualChangeFrame = FALSE, LONG lTransferWIPSlotMode = FALSE, BOOL bReScanBarcodeOnly = FALSE);
	LONG UDB_LoadFromMgzSlotToBuffer(BOOL bBurnInLoad, LONG lLoadBlock, LONG lCurrMgzn, LONG lCurrSlot, BOOL bUseEmpty, BOOL bReScanBarcodeOnly, CBinGripperBuffer *pGripperBuffer);

	LONG UDB_LoadFromBufferToTable(BOOL bBurnInLoad, CBinGripperBuffer *pGripperBuffer, BOOL bOffline=TRUE, LONG lTransferWIPSlotMode = 0);
	LONG UDB_MgznSlotToMgznSlot(CBinGripperBuffer *pGripperBuffer, const LONG lSourceMgznNo, const LONG lSourceSlotNo,
								const LONG lTargetMgznNo, const LONG lTargetSlotNo, const BOOL bReScanBarcodeOnly = FALSE);
	BOOL BinLoaderUnloadJamMessage_UDB(BOOL bBurnInUnload, CBinGripperBuffer *pGripperBuffer);

	LONG UDB_ReadBarCodeAtUnload(BOOL bBinFull, BOOL bBurnIn, CBinGripperBuffer *pGripperBuffer);
	BOOL GripperRescanningUnloadBarCode(CBinGripperBuffer *pGripperBuffer);
	BOOL CheckUnloadBarCodeValid(CBinGripperBuffer *pGripperBuffer, BOOL bBinFull);
	LONG UDB_ReadBarCodeAtLoad(const BOOL bLoadCompareBarcode, const BOOL bBurnIn, CBinGripperBuffer *pGripperBuffer, const LONG lLoadBlock);
	LONG UDB_ReadBarCode(const BOOL bLoadUnload, const BOOL bLoadCompareBarcode, const BOOL bBurnIn, CBinGripperBuffer *pGripperBuffer);
	BOOL GripperScanningBarcode(CBinGripperBuffer *pGripperBuffer, const BOOL bLoadUnload, const BOOL bLoadCompareBarcode);
	BOOL GripperPushBackFrame(CBinGripperBuffer *pGripperBuffer, const BOOL bLoadUnload);
	LONG GripperPushBackFrame(CBinGripperBuffer *pGripperBuffer, LONG lPosnX);

	LONG UDB_DownElevatorToReady(CBinGripperBuffer *pGripperBuffer, BOOL bWait=TRUE);
	LONG MoveElevatorToWithCheckMissingStep(const CString szTitle, const LONG nPosT, const LONG nLevelZ, const BOOL bWait, const BOOL bDisableOutOfMgznCheck = FALSE);
	LONG MoveElevatorTo(const CString szTitle, const LONG nPosT, const LONG nLevelZ, const BOOL bWait, const BOOL bDisableOutOfMgznCheck = FALSE);

    LONG PreStartInit(VOID);

	VOID ResetAllMagazine();
    LONG ResetMagazine(ULONG ulMgzn, BOOL bIsManual=FALSE, BOOL bClearBin=FALSE);		// reset
    LONG ResetMagazineEmpty(ULONG ulMgzn);	// reset	//FOr Cree China New ModeD
    LONG ResetMagazineFull(ULONG ulMgzn);	// reset	//FOr Cree China New ModeD
	LONG ResetMgznByPhyBlock(ULONG ulPhyBlock);
    BOOL ClearMagazine(ULONG ulMgzn);		 //Reset slot and clear-bin		//v4.51A20
    LONG SelectMgznUsage_UNUSE_TARGET(LONG lUsageOld, LONG lUsageTarget);
    LONG SelectMgznUsage_UNUSE_TARGET2(LONG lUsageOld, LONG lUsageTarget1, LONG lUsageTarget2);
    LONG SelectSlotUsage_UNUSE_TARGET(LONG lUsageOld, LONG lUsageTarget);
    LONG SelectSlotUsage_All(LONG lUsageOld);
    BOOL CheckPhysicalBlockValid(ULONG ulBlock);
	BOOL CheckIfBinIsCleared(ULONG ulBlock);						//v4.50A12	//Cree HZ
	VOID ChangeOMMgznSlotUsage(LONG lMgznIndex, LONG lUsageNew);	//v4.46T28

    VOID GenerateOMSPTableFile(ULONG ulMgzn);
    VOID GenerateOMRTTableFile(ULONG ulMgzn);

    CString GetMgznUsage(LONG lIndex);
    CString GetSlotUsage(LONG lIndex);
    CString GetOMSlotUsage(ULONG ulMgzn, ULONG ulSlot);
    CString GetRTSlotUsage(ULONG ulMgzn, ULONG ulSlot);
    CString GetMagazineName(ULONG ulMagazine);
    
    // set default settings
    VOID SetOperationMode_A(VOID);
	VOID ReAssignSlotBlock_ModeA();			//v4.52A16	//Cree HZ	
    VOID SetOperationMode_B(VOID);
    VOID SetOperationMode_C(VOID);
    VOID SetOperationMode_D(VOID);
	VOID ReAssignSlotBlock_ModeD();			//v4.52A16	//Cree HZ	
    VOID SetOperationMode_E(VOID);
    VOID SetOperationMode_F(VOID);				//MS100 8mag 150bins STANDARD config	//v3.82
	VOID SetOperationMode_Standard_F();
	VOID SetMgznOMSlotStatus(const LONG lMgzn, const LONG lSlot, const LONG lSlotGradeBlock,
							 const LONG lSlotWIPCounter, const LONG lSlotUsageStatus,
							 const CString szSlotBCName);
    VOID SetOperationMode_F_AutoLine(VOID);		//MS60 AutoLine single-mag config		//v4.56A1
    VOID SetOperationMode_F_Renesas(CONST UINT nMode);		//v4.56A18
    VOID SetOperationMode_G(VOID);				//MS100 8mag 175bins STANDARD config	//v3.82
    VOID SetOperationMode_H(VOID);				//MS100 4mag 100bins STANDARD config	//v4.31T10	//YEarly MS100Plus
	

	//Log Progress info into file
	//VOID LogStatusInfo(CString szText);
	//BOOL BackupStatusInfo();		//v2.93T2

	//Log Frame Full info
	BOOL LogFrameFullInformation(BOOL bLogFile, LONG lMagNo, LONG lSlot, CString szBarcodeName);

	//Output magazine full file
	BOOL OutputMagzFullSummaryFile(LONG lMagNo, BOOL bKeepFile);

	//Clear All Bin Frame
	ULONG GetBinBlkBondedCount(ULONG ulBinBlkId);
	UCHAR GetBinBlkGrade(ULONG ulBinBlkId);

	BOOL CheckAllWaferLoaded();

	BOOL IsClearAllState();
	BOOL ClearAllBinFrame(BOOL bToFull=TRUE);

	BOOL IsSIS_StandardMode();
	BOOL IsSIS_StandaloneMode();
	LONG SIS_TransferMgznToMgzn(const LONG lSrcMgzn, const LONG lTargetMgzn);
	LONG SIS_TransferEmptyFrameMgznToMgzn(const LONG lSrcMgzn, const LONG lTargetMgzn);
	LONG SIS_TransferInlineToStandalone(IPC_CServiceMessage& svMsg);
	LONG SIS_TransferStandaloneToInline(IPC_CServiceMessage& svMsg);
	VOID SetOperationMode_SIS_Standalone_F();
	VOID SetEmptySlotStatus(const LONG lMgzn, const LONG lSlot, const LONG lSlotGradeBlock);
	VOID SetOperationMode_SIS_F();

	BOOL BT_ResetOptimizeBinCountStatus();
	BOOL ResetMapDieType();

	VOID HouseKeeping(LONG lSafePos, BOOL bEnableBT=TRUE, BOOL bIsMissingStep=FALSE, BOOL bByPassOutOfMagCheck=FALSE, BOOL bCheckBTframe=FALSE);
	VOID HouseKeeping2(LONG lSafePos, BOOL bEnableBT=TRUE, BOOL bIsMissingStep=FALSE, BOOL bByPassOutOfMagCheck=FALSE);
	BOOL ShutDown();

	//VOID LOG_BARCODE(CONST CString szMsg);	//v2.78T2

	BOOL OutputMagSummaryFileForAllMag();

//	BOOL UpdateExArmUnloadFrameMagazineStatus(LONG lMgzn, LONG lSlot, BOOL bBinFull, BOOL bClearBin, BOOL lFullStatus, BOOL bIsEmptyFrame, BOOL bBurnInUnload);

	LONG GenerateSpecialCommunicationFileForCree(LONG lLoadBlk);

	BOOL UploadBackupOutputBinSummary();
	//BOOL UploadBinSummaryGeneral();
	//BOOL UpLoadBinSummaryForLextar();

	BOOL BinLoaderUnloadJamMessage(BOOL bBurnInUnload, BOOL bUseBT2=FALSE);
	BOOL BinLoaderUnloadJamMessage_DB(CBinGripperBuffer *pGripperBuffer, BOOL bBurnInUnload);	//Temp used for Dual Buffer machine

	//Check Exchange Arm in pick/place pos to allow arms to down
	BOOL IsExArmInPickPlacePos();

	BOOL IsMagazineCanReset(ULONG lMgzn, BOOL bIsCheckAll);

	//v3.71T1
	BOOL CheckBTBLModtorStatus();
	BOOL CheckLoadUnloadSafety();
	BOOL ManualLoadUnloadFilmFrame(BOOL bLoad);
	BOOL m_bStartManualLoadUnloadTest;
	LONG m_lTestBlkID;
	LONG m_lTestCycle;

	LONG LogItems(LONG lEventNo);
	LONG GetLogItemsString(LONG lEventNo, CString& szMsg);

public:

	CBinLoader();
	virtual	~CBinLoader();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();
	virtual BOOL InitData();			// Init the data from SMF and NVRAM
	virtual VOID ClearData();			// Clear data for cold-start
	virtual VOID FlushMessage();		//Flush IPC Message	
	virtual VOID UpdateStationData();

	VOID Operation();

	BOOL IsFrameLevel();
	BOOL IsFrameLevel2();
	LONG GetBTCurrentBlock();
	BOOL IsLoadUnloadSECSCmdLocked();
	BOOL CheckHostLoadBarcodeID(const CString szSlotIDTitle, const CString szFrameBarCodeTitle, 
								const LONG lSlotID, const CString szBarcode, CString &szParaErrName);
	BOOL CheckHostUnloadBarcodeID(const BOOL bOnlyOneSlot, const BOOL bFirstSlot, 
								  const CString szSlotIDTitle, const CString szSlotGradeTitle, 
								  const CString szFrameBarCodeTitle, const CString szFrameWIPCounterTitle, 
								  const LONG lSlotID, const LONG lGrade, const CString szBarcode, const long lWIPCounter,
								  CString &szParaErrName);
	LONG SECS_SubLoadFrameCmd(const LONG lSlotID, const LONG lSlotGrade);
	LONG SECS_SubLoadFrameDoneCmd(const LONG lStatus);
	LONG SECS_SubUnloadFrameCmd(const LONG lSlotID, const LONG lSlotGrade);
	LONG SECS_SubUnloadFrameDoneCmd(const LONG lStatus);

private:
	// Set Event (BL_Event.cpp)
	VOID SetBLOperate(BOOL bState = TRUE);
	VOID SetBLReady(BOOL bState = TRUE, INT nLog=0);
	VOID SetBLAOperate(BOOL bState = TRUE);
	VOID SetBLPreOperate(BOOL bState = TRUE);
	VOID SetBLPreChangeGrade(BOOL bState = TRUE);				//v2.71
	VOID SetBLFrameLevelDn(BOOL bState = TRUE);					//v4.01

	// Wait Event (BL_Event.cpp)
	BOOL WaitBLOperate(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBLAOperate(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBLPreOperate(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBLPreChangeGrade(INT nTimeout = LOCK_TIMEOUT);		//v2.71
	BOOL WaitBLFrameLevelDn(INT nTimeout = LOCK_TIMEOUT);		//v4.01

	// Sub-State Functions (BL_SubState.cpp)
	BOOL IsMotionHardwareReady();		// Check whether the motion hardware are ready
	INT OpInitialize();
	INT OpPreStart();
	INT OpHouseKeeping_Move();
	BOOL OpNeedResetEmptyFullMgzn();
	INT OpMoveToFullMagzine();
	INT OpPreMoveElevatorToNextLoad(CONST ULONG ulCurrBlk, BOOL bUseBT2=FALSE);		//v2.82T1	//v4.21
	INT OpPreMoveElevatorToNextUnLoad(CONST BOOL bFull=FALSE, BOOL bUseBT2=FALSE);	//v2.93T2	//v4.21

	BOOL 	DB_PreloadLevel();
	CBinGripperBuffer *DB_GetPreloadGripperBuffer();
	CBinGripperBuffer *DB_GetUnloadGripperBuffer();
	LONG 	DB_PreloadBlock();
	LONG	GetCurrBinOnBT();
	CString GetCurrMgznSlotBC();
	
	//v4.59A38
//	INT OpDBufferChangeGradeFrames();
	
	INT OpDualBufferChangeGrade(BOOL bUnloadBuffer, BOOL bGoToLoad, BOOL bBufferToTable, BOOL bGoToChange, BOOL bDirectLoadToTable, LONG lLoadBlk);
//	INT OpDualBufferBinFull(BOOL bUnloadBuffer, BOOL bGoToLoad, BOOL bGoToChange, BOOL bGoToUnload, LONG lLoadBlk);
//	INT OpDualBufferBinFull_NoPreload(BOOL bUnloadBuffer, BOOL bGoToLoad, BOOL bGoToChange, BOOL bGoToUnload, LONG lLoadBlk);	//v3.94
	INT OpDualBufferUnloadLastGradeFrame();
	INT OpDualBufferPreloadEmptyFrame(LONG lLoadBlk);
//	INT OpDualBufferPreLoadNextFrame();

	//v4.59A45
	INT OpUDBufferBinFull(BOOL bUnloadBuffer, BOOL bGoToLoad, BOOL bGoToChange, BOOL bGoToUnload, LONG lLoadBlk);
	INT OpUDBufferBinFull_NoPreload(BOOL bUnloadBuffer, BOOL bGoToLoad, BOOL bGoToChange, BOOL bGoToUnload, LONG lLoadBlk);	//v3.94
	INT OpUDBufferUnloadLastGradeFrame();
	INT OpUDBufferUnloadFullFrame();
	BOOL OpUDBufferUnloadFrame();
	INT OpUDBufferPreloadEmptyFrame(LONG lLoadBlk);
	INT OpUDBufferPreLoadNextFrame();
	INT OpUDBufferChangeGradeFrames();
	INT OpDBUPreMoveElevatorToNextLoad(CONST ULONG ulCurrBlk, BOOL bUseBT2=FALSE);	

	BOOL UnloadFrameOnUpperGripper(BOOL bFull, BOOL bClearBin, BOOL bIsManualUnload);
	BOOL UnloadFrameOnLowerGripper(BOOL bFull, BOOL bClearBin, BOOL bIsManualUnload);
	INT OpOutoutMgznSummary(BOOL bMagzineTOP2 = FALSE);							//v4.15T9	//HPO by Leo Hung
	INT OpUnloadFrameOnTable();							//v4.20		//JDSU buyoff request
	BOOL m_bFrameToBeUnloadAtIdleState;					//v4.42T17
	INT OpExerciseGripperClamp();						//TongFang	//v4.21T4	
	INT OpMoveBTToUnload(CONST LONG lBTInUse, BOOL bWait=TRUE);		//v4.21T1	//MS100 9Inch

	//AutoLine	//v4.56A11	
	BOOL OpCheckAutoLineChangeGrade(CONST LONG lLoadBlk);
	BOOL OpCheckAutoLineBinFull(CONST LONG lLoadBlk);
	VOID DecodeSubBinSetting();


private: // ipc command functions

	LONG OsramBinRealignCheck(IPC_CServiceMessage& svMsg);
	LONG SelectLowerBufferGripperForSetup(IPC_CServiceMessage& svMsg);
	LONG SetUpperToLowerBufferOffsetZ(IPC_CServiceMessage& svMsg);
	LONG CheckBinLoaderSetup(IPC_CServiceMessage& svMsg);
	LONG PreBondChecking(IPC_CServiceMessage& svMsg);
	LONG UpdateAction(IPC_CServiceMessage& svMsg);
	LONG LogItems(IPC_CServiceMessage& svMsg);
	LONG GripperState(IPC_CServiceMessage& svMsg);
	LONG Gripper2State(IPC_CServiceMessage& svMsg);
	LONG GripperLevel(IPC_CServiceMessage& svMsg);
	LONG Gripper2Level(IPC_CServiceMessage& svMsg);
	LONG GripperPusher(IPC_CServiceMessage& svMsg);
	LONG FrameVacuum(IPC_CServiceMessage& svMsg);
	LONG FrameVacuum2(IPC_CServiceMessage& svMsg);
	LONG FrameLevel(IPC_CServiceMessage& svMsg);
	LONG FrameLevel2(IPC_CServiceMessage& svMsg);
	LONG GetFrameLevel(IPC_CServiceMessage& svMsg);
	LONG FrameAlign(IPC_CServiceMessage& svMsg);
	LONG FrameAlign2(IPC_CServiceMessage& svMsg);
	LONG BufferAlign(IPC_CServiceMessage& svMsg);
	LONG FontFrameLevel(IPC_CServiceMessage& svMsg);
	LONG FontFrameVacuum(IPC_CServiceMessage& svMsg);
	LONG RearFrameLevel(IPC_CServiceMessage& svMsg);
	LONG RearFrameVacuum(IPC_CServiceMessage& svMsg);
	LONG SideCoverLock(IPC_CServiceMessage& svMsg);
	LONG DualBufferLevel(IPC_CServiceMessage& svMsg);
	LONG ALFrontGate(IPC_CServiceMessage& svMsg);
	LONG ALBackGate(IPC_CServiceMessage& svMsg);
//	LONG ALMgznClamp(IPC_CServiceMessage& svMsg);
	LONG SetMS90PusherZ(IPC_CServiceMessage& svMsg);
	LONG SetMS90PusherX(IPC_CServiceMessage& svMsg);

	LONG PowerX(IPC_CServiceMessage& svMsg);
	LONG PowerX2(IPC_CServiceMessage& svMsg);
	LONG PowerZ(IPC_CServiceMessage& svMsg);
	LONG PowerArm(IPC_CServiceMessage& svMsg);
	LONG PowerTheta(IPC_CServiceMessage& svMsg);

	LONG HomeX(IPC_CServiceMessage& svMsg);
	LONG HomeX2(IPC_CServiceMessage& svMsg);
	LONG HomeY(IPC_CServiceMessage& svMsg);
	LONG HomeZ(IPC_CServiceMessage& svMsg);
	LONG HomeTheta(IPC_CServiceMessage& svMsg);
	LONG HomeToReady(IPC_CServiceMessage& svMsg);	//pllm
    LONG DownElevatorZToReady(IPC_CServiceMessage& svMsg);
	LONG DownElevatorThetaZToHome(IPC_CServiceMessage& svMsg);
	LONG DownElevatorThetaZToPosn(const LONG lZPosn);

    // for gripper and bin table setup
	LONG UpdateHMIData(IPC_CServiceMessage& svMsg);
	LONG ChangePosition(IPC_CServiceMessage& svMsg);
	LONG KeyInPosition(IPC_CServiceMessage& svMsg);
	LONG MovePosPosition(IPC_CServiceMessage& svMsg);
	LONG MoveNegPosition(IPC_CServiceMessage& svMsg);
	LONG ConfirmSetup(IPC_CServiceMessage& svMsg);
	LONG CancelSetup(IPC_CServiceMessage& svMsg);
	LONG SaveSetupData(IPC_CServiceMessage& svMsg);
	LONG CheckSemitekPasswordBC(IPC_CServiceMessage& svMsg);
	//MS100 9Inch fcns
	LONG UpdateHMIData2(IPC_CServiceMessage& svMsg);
	LONG ChangePosition2(IPC_CServiceMessage& svMsg);
	LONG KeyInPosition2(IPC_CServiceMessage& svMsg);
	LONG MovePosPosition2(IPC_CServiceMessage& svMsg);
	LONG MoveNegPosition2(IPC_CServiceMessage& svMsg);
	LONG ConfirmSetup2(IPC_CServiceMessage& svMsg);
	LONG CancelSetup2(IPC_CServiceMessage& svMsg);

    LONG IsSlotAssignedWithBlock(IPC_CServiceMessage &svMsg);
    LONG CheckFrameExistOnBinTable(IPC_CServiceMessage &svMsg);			//v3.70T2

    // for magazine setup
    LONG SelectMagazine(IPC_CServiceMessage& svMsg);
    LONG SelectMagazine2(IPC_CServiceMessage& svMsg);
    LONG ChangeMagazineIndex(IPC_CServiceMessage& svMsg);
    LONG ChangeMagazineIndex2(IPC_CServiceMessage& svMsg);
	LONG KeyInValue(IPC_CServiceMessage& svMsg);
	LONG IncreaseValue(IPC_CServiceMessage& svMsg);
	LONG DecreaseValue(IPC_CServiceMessage& svMsg);
	LONG CancelMagazineSetup(IPC_CServiceMessage& svMsg);
	LONG CancelMagazineSetup2(IPC_CServiceMessage& svMsg);
	LONG ConfirmMagazineSetup(IPC_CServiceMessage& svMsg);
	LONG ConfirmMagazineSetup2(IPC_CServiceMessage& svMsg);
	LONG UpdateSlotPitch(IPC_CServiceMessage& svMsg);

    LONG OSRAMMagazineUnload(IPC_CServiceMessage& svMsg);

	// for Exchange Arm setup
	LONG ChangeArmPosition(IPC_CServiceMessage& svMsg);
	LONG KeyInArmPosition(IPC_CServiceMessage& svMsg);
	LONG MovePosArmPosition(IPC_CServiceMessage& svMsg);
	LONG MoveNegArmPosition(IPC_CServiceMessage& svMsg);
	LONG ConfirmArmSetup(IPC_CServiceMessage& svMsg);
	LONG CancelArmSetup(IPC_CServiceMessage& svMsg);
	LONG MoveArmToChangePos(IPC_CServiceMessage& svMsg);
	LONG ExchangeFrameTest(IPC_CServiceMessage& svMsg);
	LONG ManualLoadBufferFrame(IPC_CServiceMessage& svMsg);
	LONG ManualUnloadBufferFrame(IPC_CServiceMessage& svMsg);
	LONG ExArmPreloadFrameForNextWafer(IPC_CServiceMessage& svMsg);		//v2.68
	LONG ExArmUnloadLastFrame(IPC_CServiceMessage& svMsg);				//v2.93T2

    // test run
    LONG EnableBurnInTestRun(IPC_CServiceMessage& svMsg);
	LONG ManualUnloadFilmFrame(IPC_CServiceMessage& svMsg);
	LONG AutoUnloadFilmFrameToFull(IPC_CServiceMessage& svMsg);
	LONG AutoLoadUnloadFilmFrameToFull(const LONG lBlockID);
	BOOL IsNeedResetFullMgzn();
	LONG AutoUnloadFilmFrameNotToFull(IPC_CServiceMessage& svMsg);
	BOOL m_bDisableBinFrameStatusSummaryFile;							//v4.53A5	//Dicon
	LONG AutoUnloadFilmFrameOsram(IPC_CServiceMessage& svMsg);
	LONG AutoUnloadFilmFrame(IPC_CServiceMessage& svMsg);
    LONG ManualUpdateFrameBarcode(IPC_CServiceMessage& svMsg);	
    LONG ManualLoadFilmFrame(IPC_CServiceMessage& svMsg);
    LONG ToggleStepMode(IPC_CServiceMessage& svMsg);
	LONG AutoChangeFilmFrame(IPC_CServiceMessage& svMsg);
	LONG LoopTestCounter(IPC_CServiceMessage &svMsg);
	LONG AutoLoadUnloadTest(IPC_CServiceMessage &svMsg);
    LONG SelectRTOperationMode(IPC_CServiceMessage& svMsg);
	LONG ResetSingleSlotInfo(IPC_CServiceMessage &svMsg);
	LONG ResetMagazineCmd(IPC_CServiceMessage &svMsg);
	LONG ResetSlotBarcode(IPC_CServiceMessage &svMsg);				//v2.83T65	//PLLM
	LONG LoadUnloadBinTableCmd(IPC_CServiceMessage &svMsg);			//v4.02		//WH Semitek	//F10 shortcut key
    LONG RealignBinFrameCmd(IPC_CServiceMessage &svMsg);			//REALIGN button in BinLoader sub-menu
    LONG RealignBinFrameCmd1(IPC_CServiceMessage &svMsg);			//shiraishi02
    LONG RealignBinFrameCmd_F10(IPC_CServiceMessage &svMsg);		//F10 shortcut key only		//v4.26T1	//Semitek
	LONG CheckBinFrameIsAligned(IPC_CServiceMessage &svMsg);
	LONG CheckDualBufferFrameExist(IPC_CServiceMessage &svMsg);
	//LONG EnableRealign(IPC_CServiceMessage& svMsg);
	LONG UnloadFilmFrame(IPC_CServiceMessage& svMsg);	
	LONG StartLotResetAllMgznCmd(IPC_CServiceMessage& svMsg);		//v2.70
    LONG ManualLoadUnloadRealignTest(IPC_CServiceMessage& svMsg);	//v3.71T1
	BOOL LoadUnloadBT_Semitek();

    // operation information display, for slot state display
    LONG SelectMagazineRTDisplay(IPC_CServiceMessage& svMsg);
    LONG UpdateCurrMgznDisplayName(IPC_CServiceMessage& svMsg);		//v4.17T1

    // operation set up mode
    LONG SetOperationMode(IPC_CServiceMessage& svMsg);
    LONG SetMgznUsage(IPC_CServiceMessage& svMsg);
    LONG SelectOMSetupMgzn(IPC_CServiceMessage& svMsg);
    LONG SelectOMSetupSlot(IPC_CServiceMessage& svMsg);
    LONG SetSlotUsage(IPC_CServiceMessage& svMsg);
    LONG SetSlotBlock(IPC_CServiceMessage& svMsg);
    LONG SetMagazineOMSetupDefault(IPC_CServiceMessage& svMsg);
    LONG LoadMagazineOMSetup(IPC_CServiceMessage& svMsg);		//v3.82
    LONG SaveMagazineOMSetup(IPC_CServiceMessage& svMsg);

	LONG UpdateOutput(IPC_CServiceMessage& svMsg);

	// show BL slot history
	LONG ShowHistory(IPC_CServiceMessage& svMsg);
	LONG BinFrameSummary(IPC_CServiceMessage& svMsg);

	//Output magazine full file
	LONG GetMagzFullFilePath(IPC_CServiceMessage& svMsg);
	LONG GetCreeBinBcSummaryFilePath(IPC_CServiceMessage& svMsg);	//v3.75
	LONG ManualOutputSummaryFile(IPC_CServiceMessage& svMsg);
	LONG ClearAllMagazineFile(IPC_CServiceMessage& svMsg);

	//Clear All Frame
	LONG ClearAllFrame(IPC_CServiceMessage& svMsg);
//	LONG ManualClearFrameForDBuffer(IPC_CServiceMessage& svMsg);	//v4.40T10

	LONG ManualUploadBinSummary(IPC_CServiceMessage& svMsg);

	//Manual move to Slot
	LONG MoveToSlot(IPC_CServiceMessage& svMsg);
//	LONG MoveToSlot2(IPC_CServiceMessage& svMsg);

	// Set Buffer Align Parameters
	//LONG SetBufferAlignParameters(IPC_CServiceMessage& svMsg);

	//Control the Display of Magazine Sensors in the HMI
    LONG MagazineSensorDisplay(IPC_CServiceMessage& svMsg);

	LONG RestoreMagazineRunTimeStatus(IPC_CServiceMessage& svMsg);

	// check frame is out of magazine or there is a frame on gripper
	LONG CheckBinLoaderInSafeStatusCmd(IPC_CServiceMessage &svMsg);

	// for protection between ExArm and BondArm
	LONG CheckExArmInSafePos(IPC_CServiceMessage &svMsg);

	//Dual Buffer Load/Unload Functions
	LONG DB_ManualLoadFromMgzToBuffer(IPC_CServiceMessage& svMsg);
	LONG DB_ManualUnloadFromBufferToMgz(IPC_CServiceMessage& svMsg);
	LONG DB_ManualLoadFromBufferToTable(IPC_CServiceMessage& svMsg);
	LONG DB_ManualUnloadFromTableToBuffer(IPC_CServiceMessage& svMsg);
//	LONG DualBufferTest(IPC_CServiceMessage& svMsg);
	LONG ResetDualBuffer(IPC_CServiceMessage& svMsg);	
	LONG GenerateConfigData(IPC_CServiceMessage &svMsg);	//v3.85
	//MS50	//v4.59A40
//	BOOL	m_bStartDualBufferTest;
//	INT		m_nDualBufferTestStep;
//	LONG	m_lDualBufferMgzn;
//	LONG	m_lDualBufferSlot;
//	LONG	m_lMgznToBeSetup;	

	LONG TestFcn(IPC_CServiceMessage &svMsg);				//v3.57T3
	LONG TestFrameLevel(IPC_CServiceMessage &svMsg);		//v3.70T4
	LONG TestBufferLevel(IPC_CServiceMessage &svMsg);		//v3.70T4
	LONG TestBothLevel(IPC_CServiceMessage &svMsg);
	LONG m_lFrameLevelTestCount;

	//Clear All Bin Selected
	LONG ClearAllBinAdvanced(IPC_CServiceMessage& svMsg);
	LONG ClearAllBinUpdated(IPC_CServiceMessage& svMsg);
	LONG ClearAllBinSelected(IPC_CServiceMessage& svMsg);
	LONG ClearBinResetGradeSlot(IPC_CServiceMessage& svMsg);	//v4.50A12

	LONG SelectClearBins(IPC_CServiceMessage& svMsg);
	LONG DeselectClearBins(IPC_CServiceMessage& svMsg);

	LONG SetBarcodePrefix(IPC_CServiceMessage& svMsg);
	LONG GetCurrBlockID(IPC_CServiceMessage& svMsg);		//v4.11T3

	LONG EnableCoverCheckPassword(IPC_CServiceMessage& svMsg);

	//MS100 single loader config	//Yealy MS100Plus	//v4.31T11
	LONG GetWLMagazine(IPC_CServiceMessage& svMsg);
	LONG SetWLMagazine(IPC_CServiceMessage& svMsg);
	LONG MoveWLoaderZ(IPC_CServiceMessage& svMsg);
	LONG MoveWLoaderY(IPC_CServiceMessage& svMsg);
	LONG MovetoWLSlot(IPC_CServiceMessage& svMsg);
	LONG MovetoWLSlot_Sync(IPC_CServiceMessage& svMsg);

	LONG UnloadTableFrameAtWaferEnd(IPC_CServiceMessage& svMsg);	//v4.42T17

	//ChangeLight request 1:	//v4.60A2
	LONG UnLoadBinFrameToFullMagazine(IPC_CServiceMessage& svMsg);
	LONG CheckFileTimeSpan(CString& szFullFileName);
	LONG CopyCleanBinFile();
	LONG m_lUnloadToFullTimeSpan;
	
	//Smart Inline System(AutoLine)
	VOID SECS_SetSecsGemHostCommandErr(const LONG lRet);
	LONG SECS_LoadFrameCmd(IPC_CServiceMessage& svMsg);
	LONG SECS_LoadFrameDoneCmd(IPC_CServiceMessage& svMsg);
	LONG SECS_UnloadFrameCmd(IPC_CServiceMessage& svMsg);
	LONG SECS_UnloadFrameDoneCmd(IPC_CServiceMessage& svMsg);
	BOOL IsCloseALBackGate();
	BOOL CloseALFrontGateWithLock();
	BOOL OpenALFrontGateWithLock();
	BOOL CloseALFrontGate();
	BOOL OpenALFrontGate();
	BOOL CloseALBackGate(BOOL bOpenFrontGate);
	BOOL OpenALBackGate();
	LONG OpMoveLoaderZToUnload_AutoLine(CONST BOOL bIsLoad, CONST LONG lSlotID);

	BOOL CheckHostLoadSlotUsageStatus();
	BOOL IsHostUnloadSlot(const LONG nSlotIndex, const CString szBC);
	BOOL IsOnlyOneSlot();
	BOOL CheckHostUnloadSlotIDBarcode();
	BOOL SubUpdateBulkLoadSlots(BOOL bUnload, CString szBC, LONG lID, LONG lGrade, LONG lWIP);
	BOOL UpdateBulkLoadSlots(BOOL bUnload=FALSE);			//v4.59A11
	BOOL ResetBulkLoadData();			//v4.59A11

	//For Load/unload sync
	BOOL WaitForLoadUnloadSECSCmdObject();
	VOID LoadUnloadSECSCmdLock();
	VOID LoadUnloadSECSCmdUnlock();

	BOOL WaitForLoadUnloadObject();
	BOOL LoadUnloadLockMutex();
	VOID LoadUnloadLock();
	VOID LoadUnloadUnlock();
	BOOL IsAutoLoadUnloadQCTest();
	BOOL IsAutoLoadUnloadSISTest();
	LONG CheckClosedALBackGate();

	//Generate a file when load an empty frame from empty magazine
	BOOL GenerateEmptyFrameTextFile();	//Matthew 10312018
	LONG SetEmptyFramePath(IPC_CServiceMessage &svMsg);

	LONG BurnInBLZUpperLimit(IPC_CServiceMessage &svMsg);
	LONG FindBLZEncoderFromZeroToHomeSensor(IPC_CServiceMessage &svMsg);

	BOOL SetGripperJamAlarm(CBinGripperBuffer *pGripperBuffer, UINT UpperGripperAlarmUnCode);
	BOOL SetFrameNotDetectedAlarm(CBinGripperBuffer *pGripperBuffer, UINT UpperGripperAlarmUnCode);

private:
	BOOL	m_bClearBin[251];
	BOOL	m_bClearBinHmi[251];
	BOOL	m_bAutoLineLoadDone;
	BOOL	m_bAutoLineUnloadDone;
	BOOL	m_bTriggerTransferBinFrame;

	CMutex  m_csBinSlotInfoMutex;
	CMutex  m_csLoadUnloadMutex;
	LONG	m_lLoadUnloadObject;
	LONG	m_lLoadUnloadSECSCmdObject;

	BOOL	m_bAutoLoadUnloadTest;
	LONG	m_lSecsGemHostCommandErr;

	LONG	m_lLoopTestCounter;
	BOOL	m_bFrameLevelOn;

	DOUBLE	m_dZHomeTime;

	LONG m_lTransferingStandaloneToInline;
	LONG m_lTransferingInlineToStandalone1;
	LONG m_lTransferingInlineToStandalone2;
	LONG m_lTransferingInlineToStandalone3;
};
