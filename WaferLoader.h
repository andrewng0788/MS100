#pragma once

#include "MS896AStn.h"
#include <BinSetupWrapper.h>
#include "WL_Constant.h"
#include "SfmSerialPort.h"
#include "LogFileUtil.h"
#include "resource.h"


class CWaferLoader : public CMS896AStn
{
	DECLARE_DYNCREATE(CWaferLoader)

protected:

	#define MS_WL_AUTO_LINE_MGZN_NUM				1
	#define MS_WL_AUTO_LINE_MGZN_SLOT				8
	#define MS_WL_AUTO_LINE_INPUT_START_MGZN_SLOT	1
	#define MS_WL_AUTO_LINE_INPUT_END_MGZN_SLOT		3
	#define MS_WL_AUTO_LINE_OUTPUT_START_MGZN_SLOT	4
	#define MS_WL_AUTO_LINE_OUTPUT_END_MGZN_SLOT	6
	#define MS_WL_AUTO_LINE_WIP_START_MGZN_SLOT		7
	#define MS_WL_AUTO_LINE_WIP_END_MGZN_SLOT		8

	enum // slot usage state
	{
		WL_SLOT_USAGE_UNUSE = 0,
		WL_SLOT_USAGE_FULL,
		WL_SLOT_USAGE_EMPTY,
		WL_SLOT_USAGE_ACTIVE,
		WL_SLOT_USAGE_ACTIVE2FULL,		// just for run time use, at setup, no such option
		WL_SLOT_USAGE_INUSE,			//New for Mode-H only (Yealy)	//v4.34T1
		WL_SLOT_USAGE_SORTING
	};

	typedef struct
	{
		LONG    m_lTopLevel_Z;
		DOUBLE  m_dSlotPitch;
		LONG    m_lNoOfSlots;							// not really used indeed, only get/set
		LONG    m_lSlotWIPCounter[WL_MAX_MGZN_SLOT];	// -1: full wafer
														// Wafer WIP: current remaining die number
		LONG    m_lSlotUsage[WL_MAX_MGZN_SLOT];			// full, empty, active(buffer)
		CString m_SlotBCName[WL_MAX_MGZN_SLOT];			// barcode name(with ext)

	} WL_MAGAZINE;

	// Axis Info
	CMSNmAxisInfo	m_stWLAxis_X;
	CMSNmAxisInfo	m_stWLAxis_Z;
	CMSNmAxisInfo	m_stWLExpAxis_Z;
	CMSNmAxisInfo	m_stWLAxis_X2;
	CMSNmAxisInfo	m_stWLAxis_Z2;
	CMSNmAxisInfo	m_stWLExpAxis_Z2;

	//Common
	BOOL	m_bScopeLevel;
	BOOL	m_bMagazineExist;
	BOOL	m_bMagazineExist2;			//ES101 dual tables	//v4.24
	BOOL	m_bFrameDetect;
	BOOL	m_bFrameDetect2;
	BOOL	m_bWaferFrameDetect;
	BOOL	m_bWaferFrameDetect2;
	BOOL    m_bNoSensorCheck;

	//Get from Gripper Motor Limit sensor
	BOOL	m_bFrameJam;
	BOOL	m_bFrameJam2;

	//Gear Version
	BOOL	m_bExpanderClose;
	BOOL	m_bExpanderOpen;
	BOOL	m_bExpanderLock;
	BOOL	m_bFrameExist;
	BOOL	m_bFrameExistOnExp;		//v2.64
	BOOL	m_bFrameExistOnExp2;	//v4.24		//ES101
	BOOL	m_bExpanderStatus;
	BOOL	m_bExpander2Status;		//v4.24		//ES101
	BOOL	m_bIsMagazineFull;
	BOOL	m_bIsMagazine2Full;

	BOOL	m_bFrontGateSensor;
	BOOL	m_bBackGateSensor;

	//v4.24T4	//ES101 dual-table config
	BOOL	m_bExpander2Close;
	BOOL	m_bExpander2Open;	
	BOOL	m_bExpander2Lock;	
	BOOL	m_bFrameExist2;		
	BOOL	m_bLeftLoaderCoverSensor;		//v4.30
	BOOL	m_bRightLoaderCoverSensor;		//v4.30
	//ES101		//AUTOBOND variables
	LONG	m_lFrameToBeUnloaded;			//v4.24T11
	LONG	m_lFrameToBePreloaded;			//v4.24T11
	BOOL	m_bIsFramebePreloaded;			//v4.24T11	
	BOOL	m_bFrameOperationInAutoMode;
	BOOL	m_bFrameLoadedOnWT1;
	BOOL	m_bFrameLoadedOnWT2;
	//MS100 Single-Loader option BUffer Table Variables
	BOOL	m_bFrameInUpperSlot;
	BOOL	m_bFrameInLowerSlot;

	//Vacuum Version
	BOOL	m_bFramePosition;
	BOOL	m_bFrameProtection;
	BOOL	m_bExpanderVacuum;
	BOOL	m_bExpanderLevel;
	//Yealy MS100 BUffer Table sensors
	BOOL	m_bFrameExistInUpperSlot;
	BOOL	m_bFrameExistInLowerSlot;
	BOOL	m_bFrameExistOnBufferTable;

	BOOL	m_bUpdateOutput;	// Update Sensor & encoder in HMI
	BOOL	m_bComm_X;			// X Comm flag (for servo motor only)	//v3.59
	BOOL	m_bComm_Z;			// Z Comm flag (for servo motor only in ES101)	//v4.24
	BOOL	m_bComm_Z2;			// Z2 Comm flag (for servo motor only in ES101)	//v4.24
	BOOL	m_bHome_Z;			// Z Home flag
	BOOL	m_bHome_X;			// X Home flag
	BOOL	m_bComm_X2;			// X2 Comm flag (for servo motor only)	//v3.59
	BOOL	m_bHome_Z2;			// Z2 Home flag
	BOOL	m_bHome_X2;			// X2 Home flag
	BOOL	m_bSel_Z;			// Select Z flag
	BOOL	m_bSel_X;			// Select X flag
	BOOL	m_bSel_Z2;			// Select Z2 flag
	BOOL	m_bSel_X2;			// Select X2 flag
	BOOL	m_bIsEnabled;		
	BOOL	m_bIsExpDCMotorExist;	//New Expander DC Motor in NuMotion MS100+		//v4.01
	BOOL	m_bExpDCMotorUseEncoder;//v4.38T6	//Knowles Penang	
	BOOL	m_bWLXHasEncoder;
	BOOL	m_bUseDualTablesOption;	//ES100 v2 dual wafertable option				//v4.24T4
	BOOL	m_bES201DisableTable2;	//ES201		//v4.34T9
	BOOL	m_bUseBLAsLoaderZ;		//Yearly MS100Plus uses BL as input loader		//v4.31T10
	BOOL	m_bIsGearType;		// Gear or vacuum type expander		
	DOUBLE	m_dZRes;			// Z Resolution (mm / motor step)
	DOUBLE	m_dZ2Res;			// Z2 Resolution (mm / motor step)


	/*--- Server Variables ---*/
	// Sensor State
	BOOL	m_bHomeSnr_Z;		// Z Home Sensor
	BOOL	m_bHomeSnr_X;		// X Home Sensor
	BOOL	m_bULimitSnr_X;		// X Upper Sensor
	BOOL	m_bLLimitSnr_X;		// X Lower Sensor
	BOOL	m_bULimitSnr_Z;		// Z Upper Limit Sensor
	BOOL	m_bLLimitSnr_Z;		// Z Lower Limit Sensor

	BOOL	m_bHomeSnr_X2;
	BOOL	m_bULimitSnr_X2;
	BOOL	m_bLLimitSnr_X2;

	BOOL	m_bHomeSnr_Z2;
	BOOL	m_bULimitSnr_Z2;
	BOOL	m_bLLimitSnr_Z2;

	// Encoder count
	LONG	m_lEnc_Z;			// Z Encoder count
	LONG	m_lEnc_X;			// X Encoder count
	LONG	m_lEnc_Z2;			// Z2 Encoder count
	LONG	m_lEnc_X2;			// X2 Encoder count
	LONG	m_lEnc_ExpZ;		// Expander Z (DC motor; optional for Knowles MS109)

	BOOL	m_bIsGetCurrentSlot2;
	BOOL	m_bIsGetCurrentSlot;

	/*--- Variable need to be saved in file & as Server Variables---*/

	//Wafer Gripper Position
	LONG	m_lUnloadPos_X;		
	LONG	m_lLoadPos_X;		
	LONG	m_lReadyPos_X;		
	LONG	m_lBarcodePos_X;
	LONG	m_lBarcodeCheckPos_X;
	LONG	m_l2DBarcodePos_X;		//v3.70T3
	LONG	m_lBufferLoadPos_X;		//v4.31T11	//Yealy MS100Plus with single loader 
	BOOL	m_bHomeGripper;

	//Wafer Gripper 2 Position		//v4.24
	LONG	m_lUnloadPos_X2;		
	LONG	m_lLoadPos_X2;		
	LONG	m_lReadyPos_X2;		
	LONG	m_lBarcodePos_X2;
	LONG	m_lBarcodeCheckPos_X2;
	LONG	m_l2DBarcodePos_X2;	
	BOOL	m_bHomeGripper2;

	//Wafer Loader Z Position
	LONG	m_lTopSlotLevel_Z;		
	DOUBLE	m_dSlotPitch;
	LONG	m_lTotalSlotNo;			
	
	LONG	m_lSkipSlotNo;
	LONG	m_lCurrentMagNo;
	LONG	m_lCurrentSlotNo;
	LONG	m_lLoadedSlotNo;
	LONG    m_lUnloadOffset;

	//v4.31T11	//MS100Plus Single Loader option
	LONG	m_lBinLoader_Y;					
	LONG	m_lBufTableUSlot_Z;			
	LONG	m_lBufTableLSlot_Z;			

	LONG	m_lALUnload_Z;		//autoline1

	LONG	m_lSkipSlotNo2;
	LONG	m_lCurrentMagNo2;
	LONG	m_lCurrentSlotNo2;		
	LONG    m_lUnloadOffset2;

	WL_MAGAZINE m_stWaferMagazine[WL_MAG_NO];
	WL_MAGAZINE m_stWaferMagazine2[WL_MAG_NO];

	//Expander Option
	LONG	m_lExpanderType;
	LONG	m_lHotBlowOpenTime;		
	LONG	m_lHotBlowCloseTime;	
	LONG	m_lMylarCoolTime;		
	LONG	m_lOpenDriveInTime;
	LONG	m_lCloseDriveInTime;
	LONG	m_lUnloadOpenExtraTime;			//v4.59A17	//MS90
	LONG	m_lWTAlignFrameCount;
	LONG	m_lExpEncoderOpenOffsetZ;		//v4.38T6	//Knowles MS109 Penang
	LONG	m_lExpEncoderCloseOffsetZ;		//v4.38T6	//Knowles MS109 Penang

	LONG	m_lExpDCMotorDacValue;			//v4.01		//For MS100 v2.1 DC motor type
	LONG	m_lExpDCMotorDirection;

	LONG	m_lHotBlowOpenTime2;
	LONG	m_lHotBlowCloseTime2;
	LONG	m_lMylarCoolTime2;
	LONG	m_lOpenDriveInTime2;
	LONG	m_lCloseDriveInTime2;

	CString m_szLastLoadPkgName;

//	barcode options	B
	// Scanner COM Port
	CSfmSerialPort m_oCommPort;
	CSfmSerialPort m_oCommPort2;

	UCHAR	m_ucCommPortNo;	//	useless
	UCHAR	m_ucBarcodeModel;

	CString	m_szBarcodeName;			//
	CString	m_szBarcodeName2;			//v4.24T4	//ES100v2
	CString m_szLoadAlignBarcodeName;	// 4.24TX1
	CString m_sz2nd1DBarcodeName;		

	CString m_szUpperSlotBarcode;
	CString m_szLowerSlotBarcode;

	//Barcode Option
	BOOL	m_bUseBarcode;			
	BOOL	m_bBarcodeCheck;
	BOOL	m_bCompareBarcode;

	BOOL	m_bEnable2DBarcodeCheck;	//Knowles	//PostSealOptics
	LONG	m_lBarcodeCheckLength;
	LONG	m_lBarcodeCheckLengthUpperBound;

	// Barcode in cassette File
	BOOL m_bUseBCInCassetteFile;
	CString m_szBCInCassetteFilename;
	CBinTotalWrapper m_WfrBarcodeInCassette;

	//Map Wafer Cassette	//ANDREW_SC
	BOOL	m_bMapWfrCass;
	CString	m_szWfrBarcodeInCass[WL_MAX_MAG_SLOT+5];

	BOOL	m_bReadBarCodeOnTable;
	INT		m_nWaferBarcodeMoveDirection;
	CString m_szWaferBarcodeTestPrevBarcode;
	LONG	m_lWaferBarcodeTestCount;
	LONG	m_lWaferBarcodeReverseCount;
	LONG	m_lBarcodeTestMoveStep;
	BOOL	m_bStartWaferBarcodeTest;

	LONG	m_lScanRange;
	BOOL	m_bRotateBackAfterScan;
	LONG	m_lWftYScanRange;
	DOUBLE	m_dWftTScanRange;
//	barcode options	E

	//Align Wafer Option
	LONG	m_lPRSrchID;
	LONG	m_lAngleLHSCount;		
	LONG	m_lAngleRHSCount;		
	LONG	m_lAlignWaferAngleMode;		//v3.02T7

	//v3.28T2
	LONG	m_lEjrCapCleanLimit;
	LONG	m_lEjrCapCleanCount;

	//WaferLot Info Option
	CString m_szLotInfoLotNo;
	CString m_szLotInfoFilePath;
	CString m_szLotInfoFileName;
	CString m_szLotInfoFileExt;

	// yealy: the slot number of X Magazine
	BOOL	m_bUseMagazine3AsX;
	LONG	m_lMagazineX_No;	//ES101(contour): the Magazine choose position of "X"
	LONG	m_lCurrentMagazineX1SlotNo;
	LONG	m_lCurrentMagazineX2SlotNo;
	BOOL	m_bUseAMagazineAsG;
	LONG	m_lMagazineG_No;	// the Magazine choose position of "G"
	LONG	m_lMagazineG1CurrentSlotNo;
	LONG	m_lMagazineG2CurrentSlotNo;

	// WaferLoader events	
	CEvent	m_evWTInitT;			// T init event for wafer expander checking		//andrew
	CEvent	m_evExpInit;			// Expander INIT event for BH T
	CEvent	m_evExpInitForWt;		// Expander INIT event for WT XY
	CEvent	m_evInitAFZ;			// AF Z init event for WT T and WL gripper X
	CEvent	m_evES101WldPreloadDone;	// in auto cycle, next prescan frame preloaded to table

	/*--- Variable NO need to be saved (Server Variables only)---*/
	//Process Setup  
    LONG    m_lWLGeneral_1;		//Gripper Unload 
    LONG    m_lWLGeneral_2;		//Gripper load 
    LONG    m_lWLGeneral_3;		//Gripper Ready
    LONG    m_lWLGeneral_4;		//Gripper Scan bar code position
	LONG    m_lWLGeneral_5;		//Loader Z Top slot level
    LONG    m_lWLGeneral_6;		//Loader Z Bottom slot level
    LONG    m_lWLGeneral_7;		//2D Barcode positiion		//v3.70T3
	LONG	m_lWLGeneral_8;		// check barcode
	LONG	m_lWLGeneral_9;		// no of slot
	DOUBLE	m_dWLGeneral_10;	// pitch
	LONG	m_lWLGeneral_11;	// Bin Loader Y				//For single loader option only	//v4.31T11
	LONG	m_lWLGeneral_12;	// Buffer Table USlot Z		//For single loader option only	//v4.31T11
	LONG	m_lWLGeneral_13;	// Buffer Table LSlot Z		//For single loader option only	//v4.31T11
	LONG	m_lWLGeneral_14;	// Gripper Buffer Load X	//For single loader option only	//v4.31T11
	LONG	m_lWLGeneral_15;	// AL Unload Z	//autoline1
    LONG    m_lWLGeneral_TmpA;	//Temp. variable

	CString m_szWLConfigFileName;

	LONG	m_lStepOnLoadPos;
	LONG	m_lStepOnLoadPos2;
	LONG	m_lStepOnUnloadPos;
	LONG	m_lStepOnUnloadPos2;
	LONG	m_lVacuumBefore;
	LONG	m_lVacuumAfter;

	BOOL	m_bOpenGripperInUnload;
	BOOL	m_bOpenGripper2InUnload;
	BOOL	m_bManualLoadFrame;
	BOOL	m_bUseBLAsLoaderZDisablePreload;	//v4.39T9

	BOOL	m_bAutoAdpatWafer;

	DOUBLE	m_dThetaRes;
	LONG	m_lThetaMotorDirection;
	//AutoLine	//v4.56A11
	LONG	m_lAutoLineUnloadSlotID;
	BOOL	m_bAutoLineLoadDone;
	BOOL	m_bAutoLineUnloadDone;

	//CCriticalSection m_csLoaderZMove;
	CMutex  m_csLoadUnloadMutex;
	LONG	m_lLoadUnloadObject;
	LONG	m_lLoadUnloadSECSCmdObject;

	BOOL	m_bAutoLoadUnloadTest;
	LONG	m_lSecsGemHostCommandErr;

	LONG	m_lLoopTestCounter;
protected:

	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	virtual VOID RegisterVariables();

	VOID GetSensorValue();
	VOID UpdateWLMgzSensor(); // //4.51D1 SecsGem
	VOID GetEncoderValue();
	VOID GetAxisInformation();	//NuMotion

	BOOL X_IsPowerOn();
	BOOL X2_IsPowerOn();
	BOOL Z_IsPowerOn();
	BOOL Z2_IsPowerOn();

	INT X_Home(INT nMode=SFM_WAIT);
	INT X2_Home(INT nMode=SFM_WAIT);
	INT Z_Home();
	INT Z2_Home();
	INT Z_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Z2_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT	Z_Sync();
	INT	Z2_Sync();
	INT X_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT X_MoveTo_Auto(INT nPos, INT nMode = SFM_WAIT, BOOL bFrameDetect=FALSE);
	INT X2_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT X_Move(INT nPos, INT nMode = SFM_WAIT);
	INT X2_Move(INT nPos, INT nMode = SFM_WAIT);
	INT	X_Sync();
	INT	X_Sync_Auto();
	INT	X2_Sync();

	VOID InitVariable(VOID);
	BOOL LoadData(VOID);
	BOOL LoadWaferLoaderOption(VOID);
	BOOL SaveData(VOID);
	BOOL GenerateConfigData();		//v3.86
	BOOL SaveBarcodeName(VOID);
	BOOL IsOpNeedReplaceEjrPinOrCollet();

	//BOOL SaveExitData(VOID);		//v2.64

	//Common
	VOID SetHotAir(BOOL bSet);
	VOID SetHotAir2(BOOL bSet);
	VOID SetGripperState(BOOL bSet);
	VOID SetGripper2State(BOOL bSet);
	VOID SetGripperLevel(BOOL bSet);
	VOID SetGripper2Level(BOOL bSet);
	//Gear version
	VOID SetDCMPower(BOOL bSet);
	VOID SetDCMDirecction(BOOL bSet);
	VOID SetExpanderLock(BOOL bSet);
	VOID SetExpander2Lock(BOOL bSet);		//ES100v2	//v4.24T4
	VOID SetPushUpTable(BOOL bSet);
	VOID SetPushUpTable2(BOOL bSet);
	//Vacuum version
	VOID SetFrameVacuum(BOOL bSet);
	VOID SetFrameLevel(BOOL bSet);
	VOID SetFrameAlign(BOOL bSet);
	VOID SetEjectorVacuum(BOOL bSet);
	//MS60 Autoline
	VOID SetALFrontGate(BOOL bSet);
	VOID SetALBackGate(BOOL bSet);
	VOID SetALMgznClamp(BOOL bSet);
	BOOL IsFrontGateSensorOn();
	BOOL IsBackGateSensorOn();

	//Gripper
	INT SetGripperPower(BOOL bSet);
	INT SetGripper2Power(BOOL bSet);
	INT SetLoaderZPower(BOOL bSet);
	INT SetLoaderZ2Power(BOOL bSet);

	//Common
	BOOL IsAllMotorsEnable();
	BOOL IsAllMotorsEnable2();
	BOOL IsMagazineExist(VOID);
	BOOL IsMagazineExist2(VOID);
	BOOL IsFrameDetect(VOID);
	BOOL IsFrameDetect2(VOID);
	BOOL IsFrameJam(VOID);
	BOOL IsGripperMissingSteps();
	BOOL IsFrameJam2(VOID);
	BOOL IsWaferFrameDetect(VOID);
	BOOL IsWaferFrameDetect2(VOID);
	BOOL IsScopeDown(VOID);
	BOOL IsMagazineSafeToMove(VOID);
	BOOL IsMagazineSafeToMove2(VOID);
	BOOL IsGripperAtSafePos(LONG lCheckPos);	//v3.82
	BOOL IsGripperAtSafePos2(LONG lCheckPos);
	BOOL WPR_MoveFocusToSafe();
	BOOL WPR_MoveToFocusLevel();
	BOOL WPR_DoAutoFocus(BOOL  bFullyAuto);

	BOOL IsCoverOpen();

	BOOL MoveFocusToWafer(BOOL bWft);
	BOOL MoveES101BackLightZUpDn(BOOL bUp);
	BOOL MoveES101EjtElvtZToUpDown(BOOL bToUp);
	BOOL MoveBackLightToSafeLevel();
	BOOL MoveBackLightToUpLevel();
	
	BOOL MoveEjectorElevatorToSafeLevel();

	LONG GetExpType();
	LONG GetCurrSlotNo();
	//Gear version
	BOOL IsExpanderOpen(VOID);
	BOOL IsExpanderClose(VOID);
	BOOL IsFrameExist(VOID);
	BOOL IsExpanderLock(VOID);
	//ES100v2	//v4.24T4
	BOOL IsExpander2Open(VOID);
	BOOL IsExpander2Close(VOID);
	BOOL IsFrameExist2(VOID);
	BOOL IsExpander2Lock(VOID);
	BOOL IsLeftLoaderCoverOpen(VOID);
	BOOL IsRightLoaderCoverOpen(VOID);
	//Vacuum version
	BOOL IsFramePosition(VOID);
	BOOL IsFrameProtection(VOID);
	BOOL IsExpanderVacuum(VOID);
	BOOL IsExpanderLevel(VOID);
	//Yealy MS100Plus with single loader & buffer table config
	BOOL IsUpperSlotFrameExist(VOID);
	BOOL IsLowerSlotFrameExist(VOID);
	BOOL IsBufferFrameProtectSafe(VOID);

	//Get data from other station
	VOID SetWaferTableJoystick(BOOL bState, BOOL bWT2=FALSE);
	VOID SetWaferTableJoystickSpeed(LONG lLevel);
	VOID SetJoystickLimit(BOOL bMax, BOOL bWT2=FALSE);
	//V2.83T4
	BOOL MoveWaferTableNoCheck(LONG lXAxis, LONG lYAxis, BOOL bToUnload=TRUE, INT nMode = SFM_WAIT, BOOL bWT2=FALSE);
	BOOL MoveWaferTable(LONG lXAxis, LONG lYAxis);							//v3.10T1
	BOOL MoveWaferTableLoadUnload(LONG lXAxis, LONG lYAxis, BOOL bUnload);	//v4.16T5	//MS100 9Inch
	BOOL ES101MoveWaferTable(LONG lXAxis, LONG lYAxis, INT nMode = SFM_WAIT, BOOL bWT2=FALSE);	//v2.93T2
	VOID GetWaferTableEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis);
	VOID GetWaferTable2Encoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis);
	VOID MoveWaferThetaTo(LONG lStep, BOOL bWT2=FALSE);
	VOID MoveWafer2Theta(LONG lStep);
	BOOL HomeWaferTheta(BOOL bWT2=FALSE);	//v4.46T13
	BOOL BondArmMoveToBlow(VOID);
	VOID BondArmMoveToPrePick(INT nMode = SFM_WAIT);
	BOOL CheckRepeatMap();		//PLLM v3.74T33
	VOID GetES101WTEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis, BOOL bWT2=FALSE);
	INT CheckExpander2(BOOL bMoveT = TRUE);
	INT ManualCheckExpander2();
	BOOL MoveToWTNoCheck2(LONG lXAxis, LONG lYAxis, BOOL bWT2);
	BOOL IsWT1UnderCamera();
	BOOL IsWT2UnderCamera();
	BOOL IsWT1UnderEjectorPos();
	BOOL IsWT2UnderEjectorPos();
	BOOL ES101MoveTableToUnload(INT nMode, BOOL bWT2);
	BOOL ES101MoveTableToHomeDie(INT nMode, BOOL bWT2);

	//Reset Gripper Hit Limit error
	VOID ResetGripperHitLimit(VOID);
	VOID ResetGripper2HitLimit(VOID);

	BOOL LoadMapFile(CString szData);

//	barcode functions	B
	//Existing barcode function for Symbol barcode
	UCHAR GetBCModel();

	//Barcode Scanner function
	BOOL CreateBarcodeScanner(VOID);
	BOOL StartBarcode(BOOL bStart);	//	on/off
	BOOL ReadBarcode(CString *szData, int nTimeOut = 100, int nRetry = 10);
	BOOL ReadBarcode_Keyence(CString *szData);
	BOOL ReadBarcode_DefaultCom (CString *szData, int nTimeOut, int nRetry);
	//	regal barcode reader
	BOOL StartBarcode_Regal(BOOL bStart);
	BOOL ReadBarcode_Regal(CString *szData, int nRetry = 1);
	BOOL PurgeBarcode_Regal();

	BOOL	CheckBarcodeLength(CString szBarcode);

	INT Scan2DBarCode();		//v2.83T61		//v3.70T2
	INT ScanByRegal(BOOL bAllowSearch, BOOL bIsCheckBarcode, BOOL bIsCheckBarcodeFucEnabled);

	INT Scan2nd1DBarCode(BOOL bAllowSearch, BOOL bIsCompareBarcode);		

	INT ScanByDefault();		//v4.38T1
	INT ScanByDefault2();		//v4.38T2

	BOOL EnableKeyboardHook(CONST BOOL bEnable);

	BOOL WL_InputBarcode(INT nTable, CString &szBarcode);	//	1 for WFT1, 2 for WFT2
	//New barcode function for keyence barcode
	INT ScanByKeyence(BOOL bAllowSearch, BOOL bIsCompareBarcode);
	
	INT ScanBySymbol(BOOL bAllowSearch, BOOL bIsCheckBarcode);
	INT ScanBySymbolWith2ndPosn(BOOL bAllowSearch, BOOL bIsCheckBarcode); 

	INT	ScanBySymbolOnBufferTable(BOOL bAllowSearch, CString& szBarcodeName);

	BOOL GetWaferIDInXML(CString *szData);

	
	INT  ScanBySymbol2(BOOL bAllowSearch, BOOL bIsCheckBarcode);

	BOOL CreateBarcodeScanner2(VOID);
//	BOOL ReadBarcode2(BOOL bStart);
	BOOL ReadBarcode2(CString *szData, int nTimeOut = 100, int nRetry = 10);
	BOOL ReadBarcodeValue2(CString *szData);
	BOOL ReadBarcode2_DefaultCom(CString *szData, int nTimeOut = 100, int nRetry = 10);

	//v4.40T10	//Sanan
	INT  ScanByDefaultCom();
	INT  ScanByDefault2Com();
	BOOL ReadBarcode_DefaultCom(CString *szData);
	BOOL ReadBarcode_4All(CString *szData, int nTimeOut = 100, int nRetry = 10);
	BOOL ReadBarcode2_4All(CString *szData, int nTimeOut = 100, int nRetry = 10);

	BOOL Read2DBarCodeFromWafer(CString *szData);	//v4.50A2

	//Barcode in Cassette File functions
	BOOL SaveBarcodeInCassette(CString szFilename);
	BOOL GetBarcodeInCassetteFilename(CString& szFilename);
	BOOL GetBarcodeInCassette(CString& szBarcode, LONG lSlotNo);
	BOOL ResetBarcodeInCassette();
	BOOL ShowBarcodeInCassette();
//	barcode functions	E

	BOOL CheckWaferLotData(CString szFilename, BOOL bWT2=FALSE);

	BOOL CheckBatchIdFile(CString szFilename);
	BOOL ClearBatchIDInformation();

	INT CheckExpander(BOOL bMoveT = TRUE);
	INT CheckVacuumExpander();		//v2.56
	INT ManualCheckExpander();
	INT CloseExpander();

	BOOL WaitWTInitT(INT nTimeout = LOCK_TIMEOUT);
	VOID SetExpInit(BOOL bState = TRUE);
	VOID SetExpInitForWt(BOOL bState = TRUE);
	BOOL WaitWprInitAFZ(INT nTimeout = LOCK_TIMEOUT);
	VOID	SetES101PreloadFrameDone(BOOL bState);

	//v4.55A7
	VOID SendResetSlotEvent_7018(const LONG lSlot, BOOL bResetAll = FALSE);
	VOID SendGripperEvent_7100(const LONG lUnloadPos_X, const LONG lStepOnUnloadPos, const LONG lLoadPos_X, const LONG lStepOnLoadPos,
							   const LONG lReadyPos_X, const LONG lBarcodePos_X, const LONG lTopSlotLevel_Z, const double dSlotPitch,
							   const LONG lTotalSlotNo, const LONG lCurrentSlotNo, const LONG lSkipSlotNo);
	VOID SendGripper2Event_7100(const LONG lUnloadPos_X, const LONG lStepOnUnloadPos, const LONG lLoadPos_X, const LONG lStepOnLoadPos,
								const LONG lReadyPos_X, const LONG lBarcodePos_X, const LONG lMagazineNo, const LONG lCurrentSlotNo, const LONG lSkipSlotNo);
	VOID SendSetupEvent_7101();
	VOID SendWTUnloadPosnEvent_7102(const LONG lUnloadPhyPosX, const LONG lUnloadPhyPosY);
	VOID SendWTUnloadPosn2Event_7102(const LONG lUnloadPhyPosX, const LONG lUnloadPhyPosY);
	VOID SendHomeDieTablePosnEvent_7103(const LONG lHomeDiePhyPosX, const LONG lHomeDiePhyPosY);
	VOID SendCassetteMappedEvent_8001();
	VOID SendLoadCassetteEvent_8002();
	VOID SendUnloadCassetteEvent_8003();

	VOID SendBarcodeRejectEvent_8026(const LONG lCurSlot, const LONG lRejectCode);
	VOID SetSlotRejectStatus(const LONG lCurrMgzn, const LONG lCurSlot, const LONG lRejectCode);
	//Wafer Slot State
	CString GetSlotUsage(const LONG lSlotUsageType);
	VOID GenerateOMRTTableFile(ULONG ulMgzn);
	//SECSGEM fcns
	BOOL InitMSAutoLineWLModule();
	virtual VOID UpdateAllSGVariables();
	BOOL SECS_InitCassetteSlotInfo();

	BOOL CheckHostLoadSlotUsageStatus();
	BOOL IsHostUnloadSlot(const LONG nSlotIndex, const CString szBC);
	BOOL IsOnlyOneSlot();
	BOOL CheckHostUnloadSlotIDBarcode();
	BOOL SubUpdateBulkLoadSlots(BOOL bUnload, CString szBC, LONG lID, LONG lWIP);
	BOOL SECS_UpdateCassetteSlotInfo(CONST LONG lSlotNo, BOOL bIsLoad=FALSE, BOOL bStatus=TRUE, BOOL bFull=FALSE);
	BOOL SECS_UpdateLoadUnloadStatus(CONST LONG lSlotNo, BOOL bIsLoad, LONG lStatus);
	BOOL SECS_UploadbackupMapinUnload();	//v4.59A13

	LONG GetEmptyWIPSlot_AutoLine(const LONG lCurrMgzn);
	LONG GetFullWIPSlot_AutoLine(const LONG lCurrMgzn, const LONG lCurrSlot);

	BOOL IsInputSlot_AutoLine(const LONG lMgzn, const LONG lSlot);
	LONG GetAvailableInputSlot_AutoLine(const LONG lCurrMgzn);
	BOOL GetLoadMgzSlot_AutoLine(const BOOL bGetWIPMgzSlot_AutoLine);
//	LONG GetMgzNoOfInputSlots_AutoLine(const LONG lNoOfSlots);

	BOOL GetOutputWIPSlotWithStatus_AutoLine(const LONG lSlotUsage, LONG &lMgzn, LONG &lSlot);
	LONG GetAvailableOutputSlot_AutoLine(const LONG lCurrMgzn);
	LONG GetEmptyOutputSlot_AutoLine(const LONG lCurrMgzn);
	VOID TransferUnloadMgznSlot_AutoLine(const LONG lFromMagNo, const LONG lFromSlotNo,
										 const LONG lUnloadMagNo, const LONG lUnloadSlotNo, const BOOL bGetWIPMgzSlot_AutoLine);
	BOOL GetUnloadMgzSlot_AutoLine(const BOOL bGetWIPMgzSlot_AutoLine, LONG &lCurrentSlotNo);
	BOOL MoveToLoadMgzSlot_AutoLine(const BOOL bGetWIPMgzSlot_AutoLine);

	LONG TransferBondingMgznSlot_AutoLine(LONG lMgzn, LONG lSlot, LONG lNewMgzn, LONG lNewSlot);
	LONG GetCassetteSlotWIPCounter(const LONG lMgzn, const LONG lSlot);
	LONG GetCassetteSlotUsage(const LONG lMgzn, const LONG lSlot);
	CString GetCassetteSlotBCName(const LONG lMgzn, const LONG lSlot);
	VOID SetCassetteSlotStatus(const LONG lMgzn, const LONG lSlot,
							   const LONG lSlotWIPCounter, const LONG lSlotUsageStatus,
							   const CString szSlotBCName);
public:

	CWaferLoader();
	virtual	~CWaferLoader();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();
	virtual BOOL InitData();			// Init the data from SMF and NVRAM
	virtual VOID ClearData();			// Clear data content for cold-start
	virtual VOID FlushMessage();		// Flush IPC Message	
	virtual VOID UpdateStationData();
	
	BOOL IsLoadUnloadSECSCmdLocked();
	BOOL CheckHostLoadBarcodeID(const CString szSlotIDTitle, const CString szFrameBarCodeTitle, 
								const LONG lSlotID, const CString szBarcode, CString &szParaErrName);
	BOOL CheckHostUnloadBarcodeID(const BOOL bOnlyOneSlot, const BOOL bFirstSlot, 
								  const CString szSlotIDTitle, const CString szFrameBarCodeTitle,  
								  const LONG lSlotID, const CString szBarcode, CString &szParaErrName);
	LONG WL_GetCassCurSlot();
	BOOL IsUseBarcode()
	{
		return m_bUseBarcode;
	}
	BOOL LoadPackageMsdWLData(VOID);
	BOOL SavePackageMsdWLData(VOID);
	BOOL UpdateWLPackageList(VOID);

	LONG GetAngleLHSCount()
	{
		if (this == NULL)
		{
			return 0;
		}
		return m_lAngleLHSCount;
	}

	LONG GetAngleRHSCount()
	{
		if (this == NULL)
		{
			return 0;
		}
		return m_lAngleRHSCount;
	}
	
protected:

	VOID Operation();
	VOID RunOperation();

	virtual	VOID PreStartOperation();
	virtual	VOID AutoOperation();
	virtual	VOID DemoOperation();
	virtual	VOID StopOperation();

	BOOL OpCheckIfFrameNeedToBeUnload();
	BOOL OpCheckIfFrameNeedToBePreload();
	BOOL OpUnloadLastFilmFrame();
	BOOL OpPreloadNextFilmFrame();

	// Operation state
	enum {	WAIT_LOADUNLOAD_Q,
			UNLOAD_FRAME_Q,
			PRELOAD_FRAME_Q,
			HOUSE_KEEPING_Q
		};


private:

	LONG UpdateAction(IPC_CServiceMessage& svMsg);
	LONG LogItems(IPC_CServiceMessage& svMsg);
	LONG UpdateExpanderType(IPC_CServiceMessage& svMsg);

	//Common
	LONG HotAir(IPC_CServiceMessage& svMsg);
	LONG GripperState(IPC_CServiceMessage& svMsg);
	LONG GripperLevel(IPC_CServiceMessage& svMsg);

	LONG HotAir2(IPC_CServiceMessage& svMsg);
	LONG Gripper2State(IPC_CServiceMessage& svMsg);
	LONG Gripper2Level(IPC_CServiceMessage& svMsg);
	LONG Expander2Lock(IPC_CServiceMessage& svMsg);

	//Gear version
	LONG DCMPower(IPC_CServiceMessage& svMsg);
	LONG DCMDirection(IPC_CServiceMessage& svMsg);
	LONG ExpanderLock(IPC_CServiceMessage& svMsg);
	LONG CheckExpanderLock(IPC_CServiceMessage& svMsg);
	LONG WLCheckPrestartStatus(IPC_CServiceMessage& svMsg);
	
	LONG CheckExpander2Lock(IPC_CServiceMessage& svMsg);

	//Vacuum version
	LONG FrameVacuum(IPC_CServiceMessage& svMsg);
	LONG FrameLevel(IPC_CServiceMessage& svMsg);
	LONG FrameAlign(IPC_CServiceMessage& svMsg);
	//MS60 AutoLine
	LONG ALFrontGate(IPC_CServiceMessage& svMsg);
	LONG ALBackGate(IPC_CServiceMessage& svMsg);
	LONG ALMgznClamp(IPC_CServiceMessage& svMsg);

	LONG PowerX(IPC_CServiceMessage& svMsg);
	LONG PowerZ(IPC_CServiceMessage& svMsg);
	LONG HomeX(IPC_CServiceMessage& svMsg);
	LONG HomeZ(IPC_CServiceMessage& svMsg);

	LONG PowerX2(IPC_CServiceMessage& svMsg);
	LONG PowerZ2(IPC_CServiceMessage& svMsg);
	LONG HomeX2(IPC_CServiceMessage& svMsg);
	LONG HomeZ2(IPC_CServiceMessage& svMsg);

	LONG ChangePosition(IPC_CServiceMessage& svMsg);
	LONG KeyInPosition(IPC_CServiceMessage& svMsg);
	LONG MovePosPosition(IPC_CServiceMessage& svMsg);
	LONG MoveNegPosition(IPC_CServiceMessage& svMsg);
	LONG ConfirmSetup(IPC_CServiceMessage& svMsg);
	LONG CancelSetup(IPC_CServiceMessage& svMsg);
	LONG SelectMagazine(IPC_CServiceMessage& svMsg);
	// for wafer gripper 2
	LONG ChangePosition2(IPC_CServiceMessage& svMsg);
	LONG KeyInPosition2(IPC_CServiceMessage& svMsg);
	LONG MovePosPosition2(IPC_CServiceMessage& svMsg);
	LONG MoveNegPosition2(IPC_CServiceMessage& svMsg);
	LONG ConfirmSetup2(IPC_CServiceMessage& svMsg);
	LONG CancelSetup2(IPC_CServiceMessage& svMsg);
	LONG SelectMagazine2(IPC_CServiceMessage& svMsg);

	LONG SaveSetupData(IPC_CServiceMessage& svMsg);
	LONG GenerateConfigData(IPC_CServiceMessage& svMsg);	//v3.86

	LONG MoveToPosition(IPC_CServiceMessage& svMsg);
	LONG SetUnloadPosition(IPC_CServiceMessage& svMsg);
	LONG ConfirmUnloadPosition(IPC_CServiceMessage& svMsg);
	LONG MoveToPosition2(IPC_CServiceMessage& svMsg);
	LONG SetUnloadPosition2(IPC_CServiceMessage& svMsg);
	LONG ConfirmUnloadPosition2(IPC_CServiceMessage& svMsg);

	LONG SetMS902ndHomePosn(IPC_CServiceMessage& svMsg);
	LONG ConfirmMS902ndHomePosn(IPC_CServiceMessage& svMsg);

	LONG SetHomeDiePosition(IPC_CServiceMessage& svMsg);
	LONG ConfirmHomeDiePosition(IPC_CServiceMessage& svMsg);
	LONG GetHomeDieRecordID(IPC_CServiceMessage& svMsg);

	LONG MoveToSlot(IPC_CServiceMessage& svMsg);
	LONG MoveToSlot2(IPC_CServiceMessage& svMsg);

	//Testing function
	LONG ManualChangeFilmFrame(IPC_CServiceMessage& svMsg);
	LONG ManualChangeFilmFrame_WT2(IPC_CServiceMessage& svMsg);
	LONG App_AutoChangeFilmFrame(IPC_CServiceMessage& svMsg);
	LONG App_OpenCloseExpander(IPC_CServiceMessage& svMsg);
	LONG ResetSingleWaferSlotInfoCmd(IPC_CServiceMessage &svMsg);
	LONG ResetWaferSlotInfoCmd(IPC_CServiceMessage& svMsg);
	LONG DisplayWaferSlotInfo(IPC_CServiceMessage &svMsg);
	LONG AutoChangeFilmFrame(IPC_CServiceMessage& svMsg);
	LONG LoopTestCounter(IPC_CServiceMessage &svMsg);
	LONG AutoLoadUnloadTest(IPC_CServiceMessage& svMsg);
	LONG AutoLoadUnloadTest2(IPC_CServiceMessage& svMsg);
	LONG ManualSearchHomeDie(IPC_CServiceMessage& svMsg);

//	barcode functions	B
	LONG ManualReadBarCodeOnTable(IPC_CServiceMessage& svMsg);		//v4.42T6
	LONG TestBarcode(IPC_CServiceMessage& svMsg);
	LONG ReadStaticBarcode(CString &szBarcode);
	LONG TestBarcode2(IPC_CServiceMessage& svMsg);
	// Barcode in cassettle File
	LONG LoadBarcodeInCassetteFile(IPC_CServiceMessage& svMsg);
	LONG ResetBarcodeInCassette(IPC_CServiceMessage& svMsg);
	LONG WL1InputBarcodeLoadMap(IPC_CServiceMessage& svMsg);
	LONG WL2InputBarcodeLoadMap(IPC_CServiceMessage& svMsg);
	LONG AutoReadBarcodeOnTable(IPC_CServiceMessage &svMsg);		//v4.44A6	//3E DL
	LONG WaferBarcodeTest(IPC_CServiceMessage& svMsg);
	VOID WaferBarcodeMoveScanTest();
//	barcode functions	E

	LONG FindWaferAngle(IPC_CServiceMessage& svMsg);
	LONG FindWaferAngleFirstButton(IPC_CServiceMessage& svMsg);
	LONG AutoRegionAlignWafer(IPC_CServiceMessage& svMsg);
	LONG AutoAlignFrameWafer(IPC_CServiceMessage& svMsg);
	BOOL ResetFullMagazine(IPC_CServiceMessage& svMsg);
	BOOL ResetXMagazine(IPC_CServiceMessage& svMsg);
	BOOL ResetGMagazine(IPC_CServiceMessage& svMsg);

	//v4.45T1
	LONG StartContLoadUnloadTest(IPC_CServiceMessage& svMsg);		//v4.45T1
	BOOL m_bContLoadUnloedStart;
	BOOL ContLoadUnloadTest();

	LONG UpDownFrameLevel(BOOL bOpen);
	LONG OpenCloseExpander(BOOL bOpen);
	LONG ManualOpenCloseExpander(IPC_CServiceMessage& svMsg);
	BOOL ManualCloseExpLoadMap();		//v4.48A16	//Testar
	LONG ManualOpenCloseExpander2(IPC_CServiceMessage& svMsg);

	LONG WLBurnInSetup(IPC_CServiceMessage& svMsg);
	LONG WaferLoaderBurnInSetup(IPC_CServiceMessage& svMsg);

	//Load wafer frame
	LONG AutoLoadWaferFrame(IPC_CServiceMessage& svMsg);
	LONG ES101AutoLoadWaferFrame(BOOL	bBurnIn);
	LONG ES101AutoChangeFilmFrame(BOOL	bBurnIn);

	LONG AlignFrameWafer(BOOL bFrameDetected, BOOL bFrameExisted, BOOL bSearhHomeDie, BOOL bFullyAuto=TRUE, INT nLoadMapID=0);
	LONG ES101LoadOfflineMapFile(CString szMapFileName);

	LONG UpdateOutput(IPC_CServiceMessage& svMsg);

	// show WL slot history
	LONG ShowHistory(IPC_CServiceMessage& svMsg);

	// WaferLot Info 
	LONG ReadWaferLotInfoFile(IPC_CServiceMessage& svMsg);
	LONG GetWaferLotInfoFilePath(IPC_CServiceMessage& svMsg);

	// Map Cassette
	LONG WL_MapCassette(IPC_CServiceMessage& svMsg);
	LONG WL_LoadUnloadCassette(IPC_CServiceMessage& svMsg);
	BOOL SaveCassetteInfo();
	LONG WL_GetCassInfo(IPC_CServiceMessage& svMsg);

	//Check expander is lock (use for other stations)
	LONG IsExpanderSafeToMove(IPC_CServiceMessage& svMsg);
	LONG IsExpander2SafeToMove(IPC_CServiceMessage& svMsg);
	LONG CheckExpanderSafeToMove(IPC_CServiceMessage& svMsg);		//v2.74
	LONG ResetEjectorCapCount(IPC_CServiceMessage& svMsg);			//v3.28T2

	//Yealy MS100Plus Single Loader & Buffer Table config			//v4.31T11
	LONG ManualLoadFromMgznToBuffer(IPC_CServiceMessage& svMsg);
	LONG ManualUnloadFromBufferToMgzn(IPC_CServiceMessage& svMsg);
	LONG ManualLoadFromBufferToTable(IPC_CServiceMessage& svMsg);
	LONG ManualUnloadFromTableToBuffer(IPC_CServiceMessage& svMsg);
	LONG GetCurrentMgznSlotNo(IPC_CServiceMessage& svMsg);			//v4.31T12
	LONG SetCurrentMgznSlotNo(IPC_CServiceMessage& svMsg);			//v4.31T12
	LONG AutoPreloadFrameFromMgznToBuffer(IPC_CServiceMessage& svMsg);
	LONG AutoUnloadFrameFromBufferToMgzn(IPC_CServiceMessage& svMsg);
	LONG ResetBufferTable(IPC_CServiceMessage& svMsg);

	LONG UpdateHMIData(IPC_CServiceMessage& svMsg);
	LONG UpdateHMIData2(IPC_CServiceMessage& svMsg);

	LONG SearchWTContourCmd(IPC_CServiceMessage &svMsg);
	LONG SearchWT1ContourCmd(IPC_CServiceMessage &svMsg);
	LONG SearchWT2ContourCmd(IPC_CServiceMessage &svMsg);

	//Smart Inline System(AutoLine)
	VOID SECS_SetSecsGemHostCommandErr(const LONG lRet);
	LONG SECS_LoadFrameCmd(IPC_CServiceMessage& svMsg);	
	LONG SECS_LoadFrameDoneCmd(IPC_CServiceMessage& svMsg);	
	LONG SECS_UnloadFrameCmd(IPC_CServiceMessage& svMsg);	
	LONG SECS_UnloadFrameDoneCmd(IPC_CServiceMessage& svMsg);
	BOOL IsCloseALBackGate();
	BOOL CloseALFrontGateWithLock();
	BOOL OpenALFrontGateWithLock();
	BOOL OpenALFrontGate();
	BOOL CloseALFrontGate();
	BOOL CloseALBackGate(BOOL bOpenFrontGate);
	BOOL OpenALBackGate();
	LONG OpMoveLoaderZToUnload_AutoLine(CONST BOOL bIsLoad, CONST LONG lSlotID);
	BOOL UpdateBulkLoadSlots(BOOL bUnload=FALSE);					//v4.59A11
	BOOL ResetBulkLoadData();										//v4.59A11

	//LONG TestExpander(IPC_CServiceMessage& svMsg);
	LONG Test(IPC_CServiceMessage& svMsg);							//v2.81

	//For Load/unload sync
	BOOL WaitForLoadUnloadSECSCmdObject();
	VOID LoadUnloadSECSCmdLock();
	VOID LoadUnloadSECSCmdUnlock();

	BOOL WaitForLoadUnloadObject();
	BOOL LoadUnloadLockMutex();
	VOID LoadUnloadLock();
	VOID LoadUnloadUnlock();

	BOOL IsAutoLoadUnloadTest();
	LONG CheckClosedALBackGate();
protected:

	//Load & unload function
	LONG GetWL1MaxMgznNo();
	LONG GetWL2MaxMgznNo();
	LONG GetSlotNoPitchCount(const LONG lSlotNo, const double dSlotPitch, const double dZRes);
	INT MoveToMagazineSlot(LONG lMagazineNo, LONG lSlotNo, LONG lOffset = 0, BOOL bWait=TRUE, BOOL bCheckSensors = TRUE);	
	INT MoveToMagazineSlot2(LONG lMagazineNo, LONG lSlotNo, LONG lOffset = 0, BOOL bWait=TRUE, BOOL bCheckSensors = TRUE);
	INT MoveToBLMagazineSlot(LONG lMagazineNo, LONG lSlotNo, BOOL bUseUnloadOffset=FALSE, BOOL bCheckSensors=TRUE, BOOL bUseUpperSlot=TRUE);		//v4.31T11
	BOOL MoveToBLMagazineSlot_Sync();

	INT HouseKeeping(BOOL bOnFrameVacuum, BOOL bGripperHitLimit, BOOL bHomeBHT, BOOL bHomeX, BOOL bDownExpanderAlert = FALSE);
	INT HouseKeeping_WT2(BOOL bOnFrameVacuum, BOOL bGripperHitLimit, BOOL bHomeBHT, BOOL bHomeX);		//v4.24
	INT FilmFrameDriveIn(INT siDirection, LONG lSearchDistance, LONG lSearchVelocity, BOOL bWaitComplete);
	INT FilmFrame2DriveIn(INT siDirection, LONG lSearchDistance, LONG lSearchVelocity, BOOL bWaitComplete);
	INT FilmFrameSearch(VOID);
	
	INT FilmFrameSearchOnTable(VOID);
	INT FilmFrameSearchOnTable2(VOID);
	INT MoveSearchFrame(INT nPos);
	INT MoveSearchFrame2(INT nPos);
	INT MoveSearchFrameExistAndJam(INT nPos);
	INT MoveSearchFrameExistAndJam2(INT nPos);
	INT GetNextFilmFrame(BOOL bGetWIPMgzSlot_AutoLine, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bDisplayMsg, BOOL& bIsMgznEmpty);
	INT GetNextFilmFrame2(BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bDisplayMsg);
	INT PushFrameBack();
	INT PushFrameBack2();
	INT PushFrameBackFromTable(BOOL bStartFromHome, BOOL bBurnIn);
	INT PushFrameBackFromTable2(BOOL bStartFromHome, BOOL bBurnIn);

	INT SendGemEvent_WL_Load(BOOL bLoad, INT nStatus=TRUE, BOOL bFull=FALSE);	//v4.59A11
	//Vacuum version
	INT FilmFrameVacuumReady();
	INT ExpanderVacuumPlatform(BOOL bOpen, BOOL bHotAir);
	INT	AlignWaferFrame(BOOL bSearchFrame=FALSE);
	CString	GetAppKeyboard();

	//Gear version (V2.83T4)
	INT FilmFrameGearReady(INT nMode = SFM_WAIT, BOOL bGripperLevelOn = TRUE, BOOL bMoveTheta = TRUE, BOOL bAtReadyPos = FALSE);
	INT FilmFrameGearReady2(INT nMode = SFM_WAIT, BOOL bGripperLevelOn = TRUE, BOOL bMoveTheta = TRUE);
	INT ExpanderGearPlatform(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, BOOL bReadBarCode=FALSE);
	INT ExpanderCylinderMotorPlatform(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, BOOL bReadBarCode=FALSE);
	INT ExpanderCylinderMotorPlatform2(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, BOOL bReadBarCode=FALSE);

	BOOL IsWL1ExpanderSafeToMove();
	BOOL IsWL2ExpanderSafeToMove();
	//NUMOTION v2.1 DC-Motor version (v4.01)
	INT ExpanderDCMotorPlatform(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, 
								LONG lMoveDac, BOOL bReadBarCode=FALSE, BOOL bCheckCover=FALSE, BOOL bIsUnload=FALSE);
	INT ExpanderDCMotor2Platform(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, 
								LONG lMoveDac, BOOL bReadBarCode=FALSE);

	//Align wafer function
	BOOL FindFirstDie(LONG *lPosX, LONG *lPosY);
	BOOL CheckFirstDieAngle();		//xyz
	BOOL CorrectWaferAngle(LONG *lPosX, LONG *lPosY);
	BOOL FindWaferGlobalAngle(CONST BOOL bFullAuto=TRUE);
	BOOL AutoLearnAdaptWafer();
	BOOL FindHomeDie(CONST BOOL bFullAuto=TRUE);
	BOOL AlignInputWafer(BOOL bFullAuto=TRUE);

	INT UnloadFilmFrame(const BOOL bStartFromHome, const BOOL bBurnIn, const BOOL bGetWIPMgzSlot_AutoLine = FALSE, const BOOL bManualChangeWafer = FALSE);
	INT SubUnloadFilmFrameWithLock(const BOOL bStartFromHome, const BOOL bBurnIn, const BOOL bGetWIPMgzSlot_AutoLine);
	INT SubUnloadFilmFrame(const BOOL bStartFromHome, const BOOL bBurnIn, const BOOL bGetWIPMgzSlot_AutoLine);
	INT OpenExpander();
	INT LoadFilmFrame(BOOL bStartFromHome, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bSearhHomeDie, BOOL bBarCodeOnTable=FALSE, BOOL bDisplayMsg = FALSE, BOOL bGetWIPMgzSlot_AutoLine = FALSE, BOOL bManualChangeWafer = FALSE);	//v2.67
	LONG WaitForAutoLineToInputWafer(BOOL bGetWIPMgzSlot_AutoLine);
	BOOL WaitForAutoLineToRemoveOutputFrame();
	INT SubLoadFilmFrameWithLock(BOOL bStartFromHome, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bSearhHomeDie, 
								 BOOL bBarCodeOnTable, BOOL bDisplayMsg, BOOL bGetWIPMgzSlot_AutoLine);
	INT SubLoadFilmFrame(BOOL bStartFromHome, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bSearhHomeDie, BOOL bBarCodeOnTable=FALSE, BOOL bDisplayMsg = FALSE, BOOL bGetWIPMgzSlot_AutoLine = FALSE);	//v2.67
	LONG WaferGripperMoveSearchJam(INT nPos, BOOL bOpenGripper = FALSE, LONG lMotionTime = 0);
	LONG WaferGripper2MoveSearchJam(INT nPos, BOOL bOpenGripper = FALSE, LONG lMotionTime = 0);
	//v4.24	//ES101
	INT UnloadFilmFrame_WT2(BOOL bStartFromHome, BOOL bBurnIn);
	INT LoadFilmFrame_WT2(BOOL bStartFromHome, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bSearhHomeDie, BOOL bBarCodeOnTable=FALSE, BOOL bDisplayMsg = FALSE);	//v2.67
	//v4.31T11	//Yealy MS100Plus Single Loader Buffer Table config
	INT CheckPreLoadUnloadStatus(BOOL bCheckExpander = TRUE);
	LONG WL_LoadFromMgzToBuffer(BOOL bBurnInLoad, BOOL bBufferLevel=WL_BUFFER_LOWER, BOOL bIsAutoBond=FALSE);
	LONG WL_UnloadFromBufferToMgz(BOOL bBurnInUnload, BOOL bBufferLevel=WL_BUFFER_UPPER, BOOL bIsAutoBond=FALSE);
	LONG WL_LoadFromBufferToTable(BOOL bBurnInLoad, BOOL bSearchHomeDie, BOOL bBufferLevel=WL_BUFFER_LOWER, BOOL bIsAutoBond=FALSE);
	LONG WL_UnloadFromTableToBuffer(BOOL bBurnInUnload, BOOL bBufferLevel=WL_BUFFER_UPPER, BOOL bIsAutoBond=FALSE);

	//Log Progress info into file
	//VOID LogStatusInfo(CString szText, BOOL bLogFile = FALSE);
	BOOL BackupStatusInfo();		//v2.93T2

//	barcode functions	B
	INT CheckBarcodeInMgzn(BOOL bStartFromHome, BOOL bSearchFrameInMagazine, BOOL bStopAtHome);		//ANDREW_SC

	INT ScanningBarcode(BOOL bAllowSearch = TRUE, BOOL bIsCompareBarcode = FALSE, BOOL bIsCheckBarcodeFucEnabled = FALSE);
	INT ScanningBarcode2(BOOL bAllowSearch = TRUE, BOOL bIsCompareBarcode = FALSE, BOOL bIsCheckBarcodeFucEnabled = FALSE);
	INT ScanningBarcodeOnTableWithTheta(CString& szBarcode, BOOL bAllowSearch = TRUE);
	INT ScanningBarcodeOnTableWithTheta_KbInput(CString& szBarcode, BOOL bAllowSearch = TRUE);		//v4.38T1
	INT ScanningBarcodeOnTable2WithTheta_KbInput(BOOL bAllowSearch = TRUE);		//v4.38T1
	INT ScanningBarcodeOnTable(BOOL bAllowSearch = TRUE, BOOL bHomeTable = TRUE);	//v4.48A15
	INT ScanningBarcodeOnTable2WithTheta(BOOL bAllowSearch = TRUE);

	BOOL ContinueScanBarcode(CString& szBarcode, LONG lTimeOut=1000);
	BOOL ContinueScanBarcode_KbInput(LONG lTimeOut=1000);					//v4.38T1
	BOOL ContinueScanBarcode2(LONG lTimeOut=1000);
	BOOL ContinueScanBarcode2_KbInput(LONG lTimeOut=1000);					//v4.38T2

	//------ Moving the Wafer Table For searching the barcode -------//
	BOOL MoveWaferTableForBarCode(INT nXYDirection,INT siDirection, LONG lSearchDistance, BOOL bWaitComplete);
	BOOL SyncWaferTableXYT(CString szDirection);
	//------ Moving the Wafer Table 2 For searching the barcode -------//
	BOOL MoveWaferTable2ForBarCode(INT nXYDirection,INT siDirection, LONG lSearchDistance, BOOL bWaitComplete);
	BOOL SyncWaferTable2XYT(CString szDirection);
//	barcode functions	E

	INT ClearMapFile();
	INT LoadMapFileWithoutSyn(CString szData);
	BOOL SyncLoadMapFile(INT &nConvID);
	BOOL WaitMapValid();

	BOOL WaferMapHeaderChecking();
	BOOL	CreeGenerateParameterList();
	BOOL	WL_GenerateParameterList();
	BOOL CheckLoadCurrentMapStatus();
	BOOL ManualClearCurrentMap();			//v4.35T3	//Knowles MS109

	LONG	LearnWLContourAuto(BOOL bWL2);
	DOUBLE	m_dContourExtraMarginX;	// percentage of FOV for X.
	DOUBLE	m_dContourExtraMarginY;	// percentage of FOV for Y.
	PR_UWORD	m_uwWL1ContourRecordID;
	PR_UWORD	m_uwWL2ContourRecordID;
	BOOL	m_bEnableContourGT;
	BOOL	CreatePreloadContourMap(BOOL bWT2);
	LONG ObtainContourEdge(BOOL bWT2, LONG lULX, LONG lULY, LONG lLRX, LONG lLRY, 
		LONG &lOutULX, LONG &lOutULY, LONG &lOutLRX, LONG &lOutLRY);
	LONG ConvertContourPrToPosition(BOOL bWT2, DOUBLE dPrX, DOUBLE dPrY, LONG &lX, LONG &lY);
	VOID ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY, 
								 double dCalibX, double dCalibY, double dCalibXY, double dCalibYX);

	// Rotate Wafer After Loaded Frame into Expander 
	// and before doing any alignemnt
	LONG AutoRotateWafer(BOOL bKeepBarcodeAngle);
	LONG AutoRotateWaferForBC(BOOL bKeepBarcodeAngle);		//v4.46T20	//PLSG
	LONG AutoRotateWafer2(BOOL bKeepBarcodeAngle);
	LONG RotateWFTTUnderCam(CONST BOOL bWT2);
	LONG WFTTGoHomeUnderCam(CONST BOOL bWT2);

	BOOL AutoLoadRankIDFile();
	// AdaptWafer Function
	LONG AutoAdaptWafer();

	BOOL WaferEndFileGeneratingCheckingCmd(const BOOL bLoadMap = FALSE);

	LONG LogItems(LONG lEventNo);
	LONG GetLogItemsString(LONG lEventNo, CString& szMsg);

	//v4.31T11
	INT MoveBinLoaderY(LONG lY);
	INT MoveBinLoaderZ(LONG lZ);
	BOOL SetWLMagazineToBL(LONG lMgzn);

};

