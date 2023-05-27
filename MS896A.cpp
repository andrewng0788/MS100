/////////////////////////////////////////////////////////////////
// MS896A.cpp : Defines the class behaviors for the application.
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, June 04, 2004
//	Revision:	1.00
//
//	By:			Andrew NG
//				AAA Software Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mmsystem.h"
#include <sys/stat.h>
#include "Direct.h"
#include "MS896A.h"
#include "WaferLoader.h"
#include "Safety.h"
#include "MainFrm.h"
#include "MS896ADoc.h"
#include "MS896AView.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "HardwareDlg.h"
#include "OptionDlg.h"
#include "HipecClass.h"
#include "AppStation.h"
#include "WaferTable.h"
#include "BondHead.h"
#include "BinTable.h"
#include "WaferPr.h"
#include "BondPr.h"
#include "BinLoader.h"
#include "NVCLoader.h"			//andrewng //2020-0626
#include "SecsComm.h"
#include "CycleState.h"
#include "FileUtil.h"
#include "GenerateDatabase.h"
#include "GenerateWaferDatabase.h"
#include "QueryWaferDatabase.h"
#include "MS896AStn.h"
//#include "NVRAM.h"
#include "PtrFactory.h"  //4.52D10
#include "LblPtr.h"
#include "AlarmCodeTable.h"
#include "PrGeneral.h"
//#ifndef MS_DEBUG
	//#include "matrix32.h"	//v3.33T5
	//#include "KM.h"
//#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Static member variables of CMS896A class
BOOL CMS896AApp::m_bIsLoadingPKGFile = FALSE;
BOOL CMS896AApp::m_bRuntimeLoadPPKG	= FALSE;
BOOL CMS896AApp::m_bIsFirstBondCycle = TRUE;	//First cycle flag	
BOOL CMS896AApp::m_bBondHeadILCFirstCycle = FALSE;
LONG CMS896AApp::m_lCheckCoverSnr = 0;
LONG CMS896AApp::m_lBarCode1ComPort = 3;	//WT
LONG CMS896AApp::m_lBarCode2ComPort = 4;	//BT
LONG CMS896AApp::m_lBarCode3ComPort = 6;	//1;	//BT2	//MS100 9Inch
LONG CMS896AApp::m_lBarCode4ComPort = 5;			//WT2	//ES101
LONG CMS896AApp::m_lMainCoverLock = 0;
LONG CMS896AApp::m_lSideCoverLock = 0;
LONG CMS896AApp::m_lBinElevatorCoverLock = 0;
LONG CMS896AApp::m_lHardwareConfigNo = 0;
LONG CMS896AApp::m_lBondHeadMounting = 0;
LONG CMS896AApp::m_lBondHeadConfig = 0;
BOOL CMS896AApp::m_bMapLoadingFinish = TRUE;
BOOL CMS896AApp::m_bMapLoadingAbort = FALSE;
LONG CMS896AApp::m_lPreBondEventReply = 0;
BOOL CMS896AApp::m_bBondHeadTThermalControl = FALSE;
BOOL CMS896AApp::m_bUseSlowBTControlProfile = FALSE;	//v3.67T5	//WalSin TW
BOOL CMS896AApp::m_bNoStaticControlForBTJoystick = FALSE;	//for Cree MS896-DL only
LONG CMS896AApp::m_lMotionPlatform	= SYSTEM_HIPEC;		//v3.61
LONG CMS896AApp::m_lVisionPlatform	= PR_SYSTEM_BW;		//v4.05
LONG CMS896AApp::m_lCycleSpeedMode = 0;					//v3.61		//0=175ms, 1=160ms
BOOL CMS896AApp::m_bMS100Plus = FALSE;					//v4.35T1
BOOL CMS896AApp::m_bMS100Plus9InchOption = FALSE;		//v4.16T3
BOOL CMS896AApp::m_bES100v2DualWftOption = FALSE;
BOOL CMS896AApp::m_bMS50	= FALSE;					//v4.59A41
BOOL CMS896AApp::m_bMS60	= FALSE;					//v4.46T21
BOOL CMS896AApp::m_bMS90	= FALSE;					//v4.48A27
BOOL CMS896AApp::m_bMSAutoLineMode				= FALSE;
BOOL CMS896AApp::m_bMS_SISMode_HMI				= FALSE;
BOOL CMS896AApp::m_bMS_SISStandaloneMode_HMI	= FALSE;
BOOL CMS896AApp::m_bMS_StandByButtonPressed_HMI	= FALSE;
LONG CMS896AApp::m_lChangeColletMaxIndex		= 20;

BOOL CMS896AApp::m_bESAoiSingleWT		= FALSE;
BOOL CMS896AApp::m_bESAoiDualWT			= FALSE;
BOOL CMS896AApp::m_bESAoiBackLight		= FALSE;
BOOL CMS896AApp::m_bMS100SingleLoaderOption = FALSE;	//v4.31T10	//Yearly MS100Plus use BL as WL
BOOL CMS896AApp::m_bIsPrototypeMachine = FALSE;			//prototype MS100 (default is not prototype)
BOOL CMS896AApp::m_bIsNuMotionSingleArmMachine = FALSE;
BOOL CMS896AApp::m_b180Arm6InchWaferTable = FALSE;
BOOL CMS896AApp::m_bEnableBHSuckingHead = FALSE;		//v4.05
BOOL CMS896AApp::m_bUseALBondArm		= FALSE;
BOOL CMS896AApp::m_bTableXYMagneticEncoder = FALSE;
BOOL CMS896AApp::m_bBinTableTHighResolution = FALSE;
BOOL CMS896AApp::m_bUseExpanderWaferTable = FALSE;
BOOL CMS896AApp::m_bEnableThetaWithBrake	= FALSE;
BOOL CMS896AApp::m_bNewAutoCleanColletSystem = FALSE;
BOOL CMS896AApp::m_bEnableGripperLoadUnloadTest = FALSE;

BOOL CMS896AApp::m_bEnableAlarmTowerBlink = FALSE;
LONG CMS896AApp::m_lAlarmBlinkStatus = 0;
LONG CMS896AApp::m_lAlarmLampStatus = 0; 

BOOL CMS896AApp::m_bEnableLaserTestOutputPortTrigger = FALSE;
BOOL CMS896AApp::m_bDownloadPortablePackageFileImage = FALSE;
BOOL CMS896AApp::m_bErasePortablePackageFileImage = FALSE;
BOOL CMS896AApp::m_bBinFrameNewRealignMethod		= FALSE;
BOOL CMS896AApp::m_bBinFrameUseSoftRealign			= FALSE;
BOOL CMS896AApp::m_bBinMultiSearchFirstDie			= FALSE;  //4.51D20 
BOOL CMS896AApp::m_bProRataData			= FALSE;
BOOL CMS896AApp::m_bBPRErrorCleanCollet	= FALSE;
//Stop Alignment
BOOL CMS896AApp::m_bStopAlign		= FALSE;

//For Logging
BOOL CMS896AApp::m_bEnableMachineLog	= FALSE;
BOOL CMS896AApp::m_bEnableWtMachineLog	= FALSE;		//v4.06
BOOL CMS896AApp::m_bEnableTableIndexLog	= FALSE;		//v4.59A12
BOOL CMS896AApp::m_bIsMSLicenseKeyChecked	= FALSE;	//v4.42T16
BOOL CMS896AApp::m_bMSLicenseKeyFcn1	= FALSE;
BOOL CMS896AApp::m_bMSLicenseKeyFcn2	= FALSE;
BOOL CMS896AApp::m_bMSLicenseKeyFcn3	= FALSE;
BOOL CMS896AApp::m_bMSLicenseKeyFcn4	= FALSE;
BOOL CMS896AApp::m_bMSLicenseKeyFcn5	= FALSE;

#ifdef NU_MOTION
UCHAR CMS896AApp::m_ucNuSimCardType = GMP_NORMAL_SUITE;
#endif

BOOL CMS896AApp::m_bEnableErrMap		= FALSE;		//andrewng //2020-0805
BOOL CMS896AApp::m_bEnableSubBin		= FALSE;
BOOL CMS896AApp::m_bNGWithBinMap		= FALSE;
//For indicate change wafer
BOOL CMS896AApp::m_bIsChangeWafer = FALSE;

CCriticalSection CMS896AApp::m_csDebugLog;
CCriticalSection CMS896AApp::m_csCycleLog;

//Nu Motion I/O Port and channel Port and Software Port
CMSNmIoPort CMS896AApp::m_NmSIPort[UM_MAX_SI_PORT];
CMSNmIoPort CMS896AApp::m_NmSOPort[UM_MAX_SO_PORT];
CMSNmChPort CMS896AApp::m_NmCHPort[UM_MAX_CH_PORT];
CMSNmSwPort CMS896AApp::m_NmSWPort[UM_MAX_SW_PORT];


/////////////////////////////////////////////////////////////////
//	Constant Definitions
/////////////////////////////////////////////////////////////////
const CString gszCOMPANY_NAME			= _T("ASM Assembly Automation Ltd.");

const CString gszMACHINE_MODEL			= _T("NVC");
const CString gszSOFTWARE_VERSION		= _T("0.34");		
const CString gszSOFTWARE_PART_NO		= _T("10-302128");
const CString gszRELEASE_DATE			= _T("Thur, June 11, 2020"); 

// Define default Registry settings' value
const CString gszMODULE_HW_FILE				= _T("MapSorterHardware");
const CString gszDEFAULT_GROUP_ID			= _T("MapSorter");

//Log File
//const CString gszLOG_FILE_PATH			= gszROOT_DIRECTORY + _T("\\OutputFiles\\LogFileDir");
const CString gszLOG_FILE_PATH				= gszROOT_DIRECTORY + _T("\\UserData\\History");
const CString gszLOG_FILE_NAME				= _T("MapSorterMsgLog");
//Mark IDE Data File Path
const CString gszMARK_DATA_PATH				= gszROOT_DIRECTORY + _T("\\Exe\\MarkData\\");
const CString gszMARK_DATA_FILE_NAME		= _T("Mark.mrk");
//Nu Data File Path
const CString gszNU_DATA_PATH				= gszROOT_DIRECTORY + _T("\\Exe\\NuData\\");

//Machine and Device File
const CString gszDEVICE_FILE_PATH			= gszROOT_DIRECTORY + _T("\\OutputFiles\\Param");
const CString gszMAP_FILE_PATH				= gszROOT_DIRECTORY + _T("\\OutputFiles\\Param");
const CString gszMAP_FILE_EXT				= _T("*");
const CString gszDEVICE_FILE_NAME			= _T("PackageFile.pkg");
const CString gszOUTPUT_FILE_PATH			= gszROOT_DIRECTORY + _T("\\OutputFiles");
const CString gszOUTPUT_FILE_FORMAT			= _T("");
const CString gszWAFEREND_FILE_PATH			= gszROOT_DIRECTORY + _T("\\OutputFiles");
const CString gszWAFEREND_FILE_FORMAT		= _T("");

const CString gszLanguage					= _T("English");
const CString gszMACHINE_NO					= _T("");
const CString gszLOT_NO						= _T("");
const CString gszBIN_LOT_DIRECTORY			= _T("");
const CString gszASSO_FILE					= _T("");
const CString gszBIN_SPEC_VERSION			= _T("");

//Bonding Date & Time
const CString gszLOAD_MAP_DATE				= _T("0000-0-0");
const CString gszLOAD_MAP_TIME				= _T("00:00:00");
const CString gszBOND_START_DATE			= _T("0000-0-0");
const CString gszBOND_START_TIME			= _T("00:00:00");
const CString gszBOND_END_DATE				= _T("0000-0-0");
const CString gszBOND_END_TIME				= _T("00:00:00");

BEGIN_MESSAGE_MAP(CMS896AApp, SFM_CModule)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////
//Construction
/////////////////////////////////////////////////////////////////
CMS896AApp::CMS896AApp()
{
	m_lTime			= -1;
	m_bEverVisionUser = 0;
	m_bChangeMaxCountWithAmi = 0;
	m_fEnableHardware	= TRUE;
	m_bDisableBHModule	= FALSE;
	m_bDisableBTModule	= FALSE;
	m_bDisableBLModule	= FALSE;
	m_bDisableNLModule	= FALSE;
	m_bDisableWTModule	= FALSE;
	m_bDisableWLModule	= FALSE;
	m_bDisableWLModuleWithExp = FALSE;

	m_bAppInit			= TRUE;		//v3.97
	m_bIsMSLicenseKeyChecked	= FALSE;	//v4.42T16

	m_nNVRAMOption		= 0;		//DRAM from TXT file;	m_pvNVRAM			= NULL;
	m_pvNVRAM			= NULL;
	m_pvNVRAM_HW		= NULL;

	m_fExecuteHmi		= FALSE;
	m_szState			= gszMODULE_STATE[UN_INITIALIZE_Q];
	m_qState			= UN_INITIALIZE_Q;
	m_nOperation		= NULL_OP;
	m_lAction			= glNULL_COMMAND;
	m_bRunState			= FALSE;
	m_fDeleteMarkIDE	= FALSE;
	m_szSoftVersion		= gszSOFTWARE_VERSION;
	m_szMachineModel	= gszMACHINE_MODEL;
	m_szSoftwareReleaseDate	= gszRELEASE_DATE;
	m_szMachineName		= _T("");
	m_szMachineNo		= gszMACHINE_NO;
	m_szLotNumber		= gszLOT_NO;
	m_szLotNumberPrefix	= _T("M");		//v4.35T1	//PLLM Rebel/Flash
	m_szShiftID			= "";			//v3.32T3
	m_szLotStartTime	= "";	
	m_szLotEndTime		= "";
	m_bStartNewLot		= FALSE;		//v2.71
	m_szMapLotNumber	= gszLOT_NO;
	m_szBinLotDirectory	= gszBIN_LOT_DIRECTORY;
	m_szAssociateFile	= gszASSO_FILE;
	m_szBinSpecVersion	= gszBIN_SPEC_VERSION;
	m_szSWReleaseNo		= gszSOFTWARE_PART_NO + "V" + m_szSoftVersion;
	m_szHiPECVersion	= "";
	m_szHiPECVersion2	= "";
	m_szAppSWVersion	= gszSOFTWARE_VERSION;
	m_bIsLoadingPKGFile = FALSE;
	m_bIsFirstBondCycle	= TRUE;
	m_bHmiExecuted		= FALSE;
	m_bChangeBinLotNo	= FALSE;
	m_bNewMapLoaded		= FALSE;
	m_bNewLotStarted	= FALSE;
	//4.52D10
	m_bWaferLabelFile	= FALSE;	
	m_bMachineStarted	= FALSE;
	m_bIsKeyboardLocked	= FALSE;		//v4.08
	m_bDisableStopButton = FALSE;		//v4.13T1

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_szStatusMessage	= _T("Ready...");
	m_szMeasurementUnit = _T("um");
	m_bBurnIn			= FALSE;
	m_bBurnInGrabImage	= FALSE;		//andrewng //2020-0618
	m_bCycleStarted		= FALSE;
	m_bCycleNotStarted	= FALSE;
	m_bDialogDeviceFile	= FALSE;
	m_bLoadPkgWithBin	= TRUE;
	m_bOnlyLoadBinParam = FALSE;
	m_bOnlyLoadWFTParam	= FALSE;
	m_bOnlyLoadPRParam	= FALSE;
	m_bExcludeMachineParam = FALSE;
	m_bPortablePKGFile	= FALSE;
	m_bRuntimeTwoPkgInUse	= FALSE;
	m_bManualLoadPkgFile	= FALSE;
	m_bRuntimeLoadPPKG		= FALSE;
	
	m_bPpkgIncludeDelay				= FALSE;
	m_bPpkgIncludePrescanSettings	= FALSE;
	m_bPpkgIncludeRegionSettings	= FALSE;
	m_bPpkgIncludePickSettingWODelay =FALSE;
	m_bResetBinSerialNo	= TRUE;
	m_bFixDevicePath	= FALSE;
	m_bOperator			= FALSE;
	m_bOperatorLogOnValidation = FALSE;
	m_bEngineer			= FALSE;
	m_bTechnician		= FALSE;
	m_bSuperUser		= FALSE;
	m_bAdministrator	= FALSE;
	m_bFirstExecute		= TRUE;
	m_bCycleFKey		= FALSE;
	m_bShortCutKey		= FALSE;
	m_bAppClosing		= FALSE;
	m_pLoadingDialog	= NULL;
	m_szProductID		= "";		//v3.24T1
	m_szUsername		= "";
	m_szEngineerPassword	= "engineer";
	m_szTechnicianPassword	= "technician";
	m_szAdminPassword		= "FullControl";
	m_szPLLMRebelPassword	= "ms899dla";
	m_szPasswordFilePath	= gszUSER_DIRECTORY;
	m_szPkgFileListPath	= "";
	m_szAmiFilePath = "";
	m_bAmiPitchUpdateBlock	= FALSE;
	m_szHmiAccessPwAdmin		= HMI_SECURITY_PW_ADM;
	m_szHmiAccessPwService		= HMI_SECURITY_PW_SER;
	m_szHmiAccessPwSuper		= HMI_SECURITY_PW_SUP;
	m_szHmiAccessPwEngineer		= HMI_SECURITY_PW_ENG;
	m_szHmiAccessPwTechnician	= HMI_SECURITY_PW_TEC;
	m_lInUseAccessLevel	= 0;
	m_lHmiAccessLevel	= 0;

	m_szBinStorageFilename = "";
	m_szBinStoragePath		= "";
	m_bDialogStorageFile	= FALSE;

	// Application features
	m_lDelayBeforeTurnOffEJVac = 0;
	m_lCameraBlockDelay		   = 0;
	m_lRTUpdateMissingDieThreshold = 0;
	m_bEnablePreBond		= FALSE;	//v3.80
	m_bEnablePostBond		= FALSE;
	m_bEnableRefDie			= FALSE;
	m_bEnableCharRefDie		= FALSE;
	m_bEnableAoiOcrDie		= FALSE;
	m_bEnableColorCamera	= FALSE;
	m_bEnableThermalEjector	= FALSE;
	m_bDisplaySortBinItemMsg = FALSE;
	m_bLoadMapSearchDeepInFolder = FALSE;
	m_bSearchCompleteFilename	= FALSE;
	m_bEnableSCNFile			= FALSE;
	m_bEnablePsmFileExistCheck	= FALSE;
	m_bEnableBatchIdFileCheck = FALSE;
	m_bEnableBinMap			= FALSE;
	m_bEnableManualInputBC	= FALSE;
	m_bDisableWaferMapFivePointCheckMsgSelection = FALSE;
	m_bMultiGradeSortToSingleBin	= FALSE;
	m_bEnableCMLTReworkFcn	= FALSE;
	m_bNoIgnoreGradeOnInterface = FALSE;
	m_bEnableMapColumnsCheck	= FALSE;
	m_ulIgnoreGrade = 0;
	m_bCheckIgnoreGradeCount = FALSE;
	m_bEnableOsramResortMode = FALSE;
	m_bRenameMapNameAfterEnd	= FALSE;
	m_bAlwaysLoadMapFileLocalHarddisk = FALSE;

	m_bUseOptBinCountDynAssignGrade = FALSE;
	m_bOpenMatchPkgNameCheck		= FALSE;
	m_bEnableSepGrade		= FALSE;
	m_bEnableBlockFunc		= FALSE;
	m_bEnableEmptyCheck		= FALSE;	//Block2
	m_bEnableWPRBackLight	= FALSE;	//Cree HuiZhou	//v4.46T28
	m_bEnableOPMenuShortcut	= FALSE;	
	m_bEnableBHShortcut		= FALSE;
	m_bEnableWPRShortcut	= FALSE;
	m_bEnableBPRShortcut	= FALSE;
	m_bEnableBinBlkShortcut	= FALSE;
	m_bEnableWTShortcut		= FALSE;
	m_bEnableOptionsTabShortcut = FALSE;
	m_bDisableOptionsShortcut = FALSE;
	m_bEnableSystemServiceShortcut = FALSE;
	m_bEnableStepMoveShortcut = FALSE;
	m_bEnablePhyBlockShortcut = FALSE;
	m_bEnableOperationModeShortcut = FALSE;
	m_bAllowMinimizeHmi		= FALSE;
	m_bEnablePRRecordShortcut = FALSE;
	m_bEnableBinTableShortcut = FALSE;
	m_bDisablePKGShortcut	= FALSE;
	m_bEnablePicknPlaceShortcut	= FALSE;
	m_bToggleWaferResortScanMode	= FALSE;
	m_bEnableWaferToBinResort		= FALSE;
	m_bEnableSECSComm		= FALSE;
	m_bEnableTCPComm		= FALSE;
	m_bOthersConnectionType = FALSE;
	m_EnableEMMode			= FALSE;
	m_bEnableBinWorkNo		= FALSE;
	m_bEnableFileNamePrefix	= FALSE;
	m_bEnableAppendInfoInWaferId = FALSE;
	m_bAddWaferIdInMsgSummaryFile = FALSE;
	m_bEnableGradeMappingFile = FALSE;
	m_bEnablePickNPlaceOutputFile = FALSE;
	m_bGenRefDieStatusFile	= FALSE;
	m_bGenLabel				= FALSE;
	m_bGenLabelWithXMLFile	= FALSE;
	m_bGenBinLotDirectory	= FALSE;

	m_bBTRealignWithFFMode	= FALSE;
	m_bEnablePolygonDie		= FALSE;
	m_bEnableChineseMenu	= FALSE;
	m_bDisableBinSNRFormat	= FALSE;
	m_bDisableClearCountFormat	= FALSE;
	m_bEnableDynMapHeaderFile	= FALSE;
	m_bBackupTempFile			= FALSE;
	m_bBackupOutputTempFile		= FALSE;
	m_bForceDisableHaveOtherFile = FALSE;
	m_bNoPRRecordForPKGFile		= FALSE;
	m_bOnOff2DBarcodeScanner	= FALSE;
	m_bEnableToolsUsageRecord	= FALSE;
	m_lToolsUsageRecordFormat	= 0;
	m_bEnableItemLog		= FALSE;
	m_bEnableAlarmLampBlink = FALSE;
	m_bEnablePkgFileList	= FALSE;
	m_bEnableMachineReport	= FALSE;
	m_bEnablePRDualColor	= FALSE;
	m_bPrCircleDetection	= FALSE;		//v2.78T2
	m_bPrAutoLearnRefDie	= FALSE;		//v2.78T2
	m_bWtCheckMasterPitch	= FALSE;		//v2.78T2
	m_bEnable1stDieFinder	= FALSE;
	m_bPrIMInterface		= FALSE;		//v2.82T4
	m_bPrLineInspection		= FALSE;		//v3.17T1
	m_bPrEnableWpr2Lighting	= FALSE;		//v3.44T1
	//m_bNewCtEnhancement		= FALSE;		//v2.83T2
	m_bUseNewStepperEnc		= FALSE;		//v2.83T2
	m_bEnablePRAdaptWafer	= FALSE;
	m_bEnableRefDieFaceValueCheck = FALSE;	//v4.48A26	//Avago
	m_bEnableNGPick			= FALSE;		//v4.51A19	//Silan MS90
	m_bUseColletOffsetWoEjtXY = FALSE;		//v4.52A14	//Osram Germany
	m_bCounterCheckWithNormalDieRecord = FALSE;
	m_bEnableSaveTempFileWithPKG	= FALSE;
	m_bDisableBuzzer		= FALSE;
	m_lSetDetectSkipMode	= 0;

	m_bWaferEndFileGenProtection = FALSE;
	m_bAutoGenWaferEndAfterPrescan	= FALSE;	
	m_bUseClearBinByGrade	= FALSE;
	m_bOutputFormatSelInNewLot	= FALSE;
	m_bRemoveBackupOutputFile	= FALSE;
	m_bAutoGenBinBlkCountSummary = FALSE;
	m_bChangeGradeBackupTempFile = FALSE;
	m_bKeepOutputFileLog	= FALSE;
	m_bEnableMultipleMapHeaderPage = FALSE;
	m_bUseBinMultiMgznSnrs		= FALSE;		//v3.30T2
	m_bExArmDisableEmptyPreload	= FALSE;		//v3.34		//For Cree MS899DLA
	m_bUseNewBHMount		= FALSE;			//v3.34		//For MS810 New BH mounting
	m_bAddDieTimeStamp		= FALSE;
	m_bCheckProcessTime		= FALSE;
	m_bAddSortingSequence	= FALSE;

	m_bEnableExtraClearBinInfo = FALSE;
// prescan relative code	B
	m_bEnablePrescanInterface	= FALSE;
	m_lWaferMapSortingpathCalTime = 0;
	m_bMapAdaptiveAlgor			= FALSE;		//v3.54T4
	m_bPrescanRealignSameMap	= FALSE;
	m_bPrescanEmptyUnmark		= FALSE;
// prescan relative code	E
	m_bEnableReVerifyRefDie	= FALSE;
	m_bNonBlkPkEndVerifyRefDieApp	= FALSE;
	m_bManualAlignReferDie			= FALSE;
	m_bEnableNewBLMagExistCheck	= FALSE;		//v3.57
	m_bCheckBinVacMeter		= FALSE;			//v3.58
	m_bEnableWaferClamp		= FALSE;			//v4.26T1
	m_bEnable2DBarCode		= FALSE;			//v3.60
	//4.52D10
	m_bEnable2nd1DBarCode	= FALSE;			
	m_bBLRealTimeCoverSensorCheck	= FALSE;	//v3.60
	m_bBLRealTimeMgznExchange		= FALSE;	//v4.40T13	//Nichia
	m_bEnableCoverLock		= FALSE;			//v3.60
	m_bToPickDefect			= FALSE;
	m_bUseSlowBTControlProfile	= FALSE;		//v3.67T5
	m_bNoStaticControlForBTJoystick = FALSE;	//for Cree MS896-DL only
	m_bBLResetGradeMagToEmpty	= FALSE;		//v3.70T1
	m_bUsePLLMSpecialFcn		= FALSE;		//v3.70T2
	m_bUsePLLM					= FALSE;
	m_bUseWideScreen			= FALSE;		//v4.29
	m_bUseTouchScreen			= FALSE;		//v4.50A3	//MS60
	m_bIsNichia					= FALSE;		//v4.40T1
	m_bIsElectech3E				= FALSE;
	m_bIsYealy					= FALSE;		//v4.46T6
	m_bIsAvago					= FALSE;		//v4.48A21
	m_bIsRenesas				= FALSE;		//v4.59A12

	m_bEnableWaferSizeSelect	= FALSE;
	m_bMS100OriginalBHSequence	= FALSE;
	m_bReplaceEjAtWaferEnd		= FALSE;
	m_bReplaceEjAtWaferStart	= FALSE;
	m_bManualCloseExpReadBc		= FALSE;
	
	m_bEnableStartupCoverSensorCheck = FALSE;
	m_bEnableBinLoderCoverSensorCheck = FALSE;

	m_bEnableAutoDieTypeFieldnameChek = FALSE;
	m_bClearDieTypeFieldDuringClearAllBin = FALSE;
	m_szDieTypeFieldnameInMap = "";

	m_bPackageFileMapHeaderCheckFunc = FALSE;
	m_bUpdateWaferMapHeader	= FALSE;
	m_szPackageFileMapHeaderCheckString = "";

	m_bEnableReportPathSettingInterface = FALSE;

	m_bBinFrameStatusSummary	= FALSE;
	m_bSupportPortablePKGFile	= FALSE;
	m_bPortablePKGInfoPage		= FALSE;
	m_bEnableEmptyBinFrameCheck	= FALSE;
	m_bEnableResetMagzCheck		= FALSE;
	m_bEnableWaferLotFileProtection = FALSE;
	m_bForceClearMapAfterWaferEnd = FALSE;
	m_bEnableSummaryPage		= FALSE;
	m_bEnableBHZVacNeutralState	= FALSE;
	m_bWafflePadBondDie			= FALSE;
	m_bWafflePadIdentification	= FALSE;
	m_buseSlowEjProfile			= FALSE;
	m_bEnableBondAlarmPage		= FALSE;
	m_bSaveMapFile				= TRUE;

	m_nMouseMode				= MOUSE_STOP;
	m_dMouseClickX				= 0.0;	
	m_dMouseClickY				= 0.0;
	m_nMouseDragState			= PR_MOUSE_EVENT_STATE_UP;
	m_dMouseDragDist			= 0.0;
	m_dMouseDragAngle			= 0.0;
	m_lMouseDragDirection		= MOUSE_JOY_DIRECTION_NONE;

	m_bWaferLotWithBinSummaryFormat = FALSE;
	m_bEnableWaferLotLoadedMapCheck = FALSE;

	m_bEnableAutoLoadNamingFile = FALSE;

	m_szRankIDFileNameInMap		= "";
	m_szRankIDFileExt			= "";
	m_bEnableLoadRankIDFromMap	= FALSE;
	m_bEnableBTPt5DiePitch		= FALSE;
	m_bEnableBTBondAreaOffset	= FALSE;
	m_bEnableBinBlkTemplate		= FALSE;
	m_bBTGelPadSupport			= FALSE;
	m_bEnableBTNewLotEmptyRowFcn	= FALSE;

	m_bEnableOptimizeBinCountFunc	= FALSE;
	m_bOptimizeBinCountPerWftFunc	= FALSE;
	m_bEnableOpBinCntInLoadPath		= FALSE;
	m_bEnableRestoreBinRunTimeData	= FALSE;

	m_bEnablePrAutoEnlargeSrchWnd	= FALSE;
	m_bEnablePrPostSealOptics		= FALSE;
	m_bEnableBHPostSealOptics		= FALSE;
	m_bEnableBHUplookPrFcn			= FALSE;	//v4.52A16
	m_bUseBHUplookPrFcn				= FALSE;	//v4.58A3	//MS90	//Turn ON/OFF by user
	m_bEnableESContourCamera		= FALSE;
	m_bEnableDualPathCamera			= FALSE;
	m_bMS60SortingFFMode			= FALSE;
	m_bEnableAlignWaferImageLog		= FALSE;
	m_bHomeFPCUseHmiMap				= FALSE;
	m_bBLBCUseOldContinueStop		= FALSE;
	m_bBLBCEmptyScanTwice			= FALSE;	//v4.51A17	//XM SanAn
	m_bBLEmptyFrameBCCheck			= FALSE;
	m_bEnableBinOutputFilePath2		= FALSE;
	m_bOfflinePostbondTest			= FALSE;	//v4.11T3
	m_bPrescanDiePitchCheck			= FALSE;
	m_bBLBCUse29MagSlots			= FALSE;	//v4.19
	m_bEnableMultiCOROffset			= FALSE;	//v4.24T8
	m_bWaferPRMxNLFWnd				= FALSE;	//v4.43T2
	m_bWaferPrMS60LFSequence		= FALSE;	//v4.47T3
	//4.53D18
	m_bWaferMapDieMixing			= FALSE;
	m_bEnableMotorizedZoom = FALSE;
	m_ulDefaultBinSNRFormat	= 0;
	m_ulDefaultClearCountFormat = 0;
	m_szCustomer			= "General";
	m_szProductLine			= "";
	m_szFuncFileVersion		= "1.0";
	m_pAppGemStation = NULL;

	// For HMI control container access mode
	m_bAcBHMotorSetup		= FALSE;
	m_bAcBondArmSetup		= FALSE;
	m_bAcBondHeadSetup		= FALSE;
	m_bAcDelaySetup			= FALSE;
	m_bAcEjectorSetup		= FALSE;
	m_bAcBinBlkEditSetup	= FALSE;
	m_bAcBinBlkClearSetup	= FALSE;
	m_bAcClearBinFileSettings = FALSE;
	m_bAcClearBinCounter	= FALSE;
	m_bAcNoGenOutputFile	= FALSE;
	m_bAcWLLoaderSetup		= FALSE;
	m_bAcWLExpanderSetup	= FALSE;
	m_bAcWLAlignmentSetup	= FALSE;
	m_bAcWLOthersSetup		= FALSE;
	m_bAcWLMotorSetup		= FALSE;
	m_bAcBLGripperSetup		= FALSE;
	m_bAcBLMagazineSetup	= FALSE;
	m_bAcBLOthersSetup		= FALSE;
	m_bAcBLOthersManualOp	= FALSE;
	m_bAcBLMotorSetup		= FALSE;
	m_bAcMapPathSetting		= FALSE;
	m_bAcAlignWafer			= FALSE;
	m_bAcDisableSCNSettings = FALSE;
	m_bAcDisableManualAlign = FALSE;
	m_bAcMapSetting			= FALSE;
	m_bAcMapOptions			= FALSE;
	m_bAcLoadMap			= FALSE;
	m_bAcClearMap			= FALSE;
	// huga
	m_bAcWaferPrAdvSetup		= FALSE;
	m_bAcWaferPrLrnRefProtect	= FALSE;
	m_bAcWaferPrLrnAlgrProtect	= FALSE;

	m_bAcBondPrAdvSetup		= FALSE;
	m_bAcBondPrLrnRefProtect = FALSE;

	m_bAcBinTableLimit = FALSE;
	m_bAcBinTableColletOffset = FALSE;

	m_bAcWaferLotSettings = FALSE;

	m_bAcCollectnEjSetup	= FALSE;
	m_bAcPKGFileSettings	= FALSE;

	m_bAcDieCheckOptions	= FALSE;
	m_bAcWaferEndOptions	= FALSE;


	// For HMI Display accees status
	m_bAsBHMotorSetup		= FALSE;
	m_bAsBondArmSetup		= FALSE;
	m_bAsBondHeadSetup		= FALSE;
	m_bAsDelaySetup			= FALSE;
	m_bAsEjectorSetup		= FALSE;
	m_bAsBinBlkEditSetup	= FALSE;
	m_bAsBinBlkClearSetup	= FALSE;
	m_bAsClearBinFileSettings = FALSE;
	m_bAsClearBinCounter	= FALSE;
	m_bAsNoGenOutputFile	= FALSE;
	m_bAsWLLoaderSetup		= FALSE;
	m_bAsWLExpanderSetup	= FALSE;
	m_bAsWLAlignmentSetup	= FALSE;
	m_bAsWLOthersSetup		= FALSE;
	m_bAsWLMotorSetup		= FALSE;
	m_bAsBLGripperSetup		= FALSE;
	m_bAsBLMagazineSetup	= FALSE;
	m_bAsBLOthersSetup		= FALSE;
	m_bAcBLOthersManualOp	= FALSE;
	m_bAsBLMotorSetup		= FALSE;

	m_bAsMapPathSetting		= FALSE;
	m_bAsAlignWafer			= FALSE;
	m_bAsDisableSCNSettings = FALSE;
	m_bAsDisableManualAlign = FALSE;

	m_bAsMapSetting			= FALSE;
	m_bAsMapOptions			= FALSE;
	m_bAsLoadMap			= FALSE;
	m_bAsClearMap			= FALSE;

	m_bAsCollectnEjSetup	= FALSE;
	m_bAsWaferMapSetup		= FALSE;
	m_bAcWaferMapSetup		= FALSE;
	// huga
	m_bAsWaferPrAdvSetup	= FALSE;
	m_bAsWaferPrLrnRefProtect = FALSE;
	m_bAsWaferPrLrnAlgrProtect	= FALSE;

	m_bAsBondPrAdvSetup			= FALSE;
	m_bAsBondPrLrnRefProtect	= FALSE;

	m_bAsBinTableLimit			= FALSE;
	m_bAsBinTableColletOffset	= FALSE;

	m_bAsPKGFileSettings	= FALSE;

	m_bAsWaferLotSettings = FALSE;

	// CP100
	m_bIsCP100NewScreen		= FALSE;
	m_bIsRedTowerSignal		= FALSE;
	m_bIsYellowTowerSignal	= FALSE;
	m_bIsGreenTowerSignal	= FALSE;

	// For Start & End Time
	m_nMapStartYear			= 0;
	m_nMapStartMonth		= 0;
	m_nMapStartDay			= 0;
	m_nMapStartHour			= 0;
	m_nMapStartMinute		= 0;
	m_nMapStartSecond		= 0;
	m_nMapRunTime			= 0;

	// For Host Communication
	m_pHostComm				= NULL;
	m_bHostAutoConnect		= FALSE;
	m_bHostCommEnabled		= FALSE;
	m_bHostCommConnected	= FALSE;
	m_bHostCommAbortByHost	= FALSE;
	m_bHostLotIDMatch		= TRUE;
	m_bHostAutoReConnect	= FALSE;
	m_bHostReConnectting	= FALSE;	
	m_szHostLotNo			= "No Name";
	m_szHostAddress			= "10.101.101.77";
	m_ulHostPortNo			= 6000;
	m_szHostMachineID		= "S01";
	m_szHostCommandID		= "11";
	m_szHostCommStatus		= _T("Ready...");
	
	m_bOnBondMode			= FALSE;

	// For HMI Display Comm Option
	m_bEnableCommOption		= FALSE;
	m_bEnableBondTaskBar	= FALSE;
	m_bEnableScanTaskBar	= FALSE;

	// For machine time
	m_szMachineBondedUnit	= "0";
	m_szMachineUpTime		= "0:00:0";
	m_szMachineRunTime		= "0:00:0";
	m_szMachineDownTime		= "0:00:0";
	m_szMachineAlarmTime	= "0:00:0";
	m_szMachineAssistTime	= "0:00:0";
	m_szMachineSetupTime	= "0:00:0";
	m_szMachineIdleTime		= "0:00:0";
	m_szMachineAlarmCount	= "0";
	m_szMachineAssistCount	= "0";

	m_bAWET_Enabled				= FALSE;
	m_bAWET_Triggered			= FALSE;
	m_bAWET_AutoSetScreen		= FALSE;
	m_bAWET_CtrlEngContainer	= FALSE;
	m_bAWET_CtrlOpContainer		= FALSE;
	m_bAWET_CtrlExitButton		= FALSE;
	m_lAWET_EngineerFlowState	= 0;
	m_szAWET_AlarmCode			= "";
	m_ulAWET_LastCounter		= 0;
	m_ulAWET_LastRunTime		= 0;
	m_szAWET_LastCode			= "";

	m_szMachineWaitingSetupTime	= "0:00:0";  // v4.51D5 SanAn(XA) 1
	m_szWaitingSetTimeAlarmMsg	= "";
	m_bWaitingSetTimeClosed		= FALSE;
	m_bWaitingSetTimeContainer	= FALSE;

	m_bFaultButton				= FALSE; // v4.51D2 //Electech3E(DL)
	m_bFaultContainer			= FALSE; 	
	m_bFaultButtonColor			= FALSE;
	m_bRecoverButton			= FALSE;
	m_bRecoverContainer			= FALSE;

	// For cycle log
	m_pCycleLog				= NULL;

	// For Mulit-Language Selections
	m_lLangSelected = 0;
	m_lLangPrevSelected = -1;
	m_szMultiLanguage = "English";

	for (INT i = 0; i < EQUIP_NO_OF_RECORD_TIME; i++)
	{
		m_szReportStartTime[i] = "";
		m_szReportEndTime[i] = "";
	}

	// Machine Time Report Variable
	m_lNoOfReportPeriod = 0;
	m_szMachineTimeReportPath = "C:\\MapSorter\\UserData\\Statistic";
	m_szMachineTimeReportPath2 = "C:\\MapSorter\\UserData\\Statistic";	//	"C:\\MapSorter\\UserData\\Machine Time"
	m_szMachineReportPath  = "";
	m_szMachineReportPath2 = "";
	m_szAlarmLampStatusReportPath = "";
	m_szTimePerformancePath = "";
	m_szMachineSerialNo	= "";

	m_bIsCopyGmpErrorLog = FALSE;

	m_dPreviewImageSizeX = 0.0;
	m_dPreviewImageSizeY = 0.0;

	m_lBHZ1HomeOffset	= 0;
	m_lBHZ2HomeOffset	= 0;

	CMS896AStn::m_lOTraceabilityFormat = 0;		//NONE
	m_szFileFormatSelectInNewLot = "N/A";

	m_bMsStopButtonPressed = FALSE;

	m_bCheckPKGKeyParameters	= FALSE;
	m_bAutoUploadOutputFile		= FALSE;
	m_bDisableBuzzer			= FALSE;

	SetMS896ADataBlock(m_oMS896ADataBlock);

	m_bStopLoadUnloadLoopTest = FALSE;
	m_lCurTime = 0;

	m_pWaferTable	= NULL;
	m_pBondHead		= NULL;
	m_pBinTable		= NULL;
	m_pWaferPr		= NULL;
	m_pBondPr		= NULL;
	m_pSecsComm		= NULL;
	m_pWaferLoader	= NULL;
	m_pSafety		= NULL;
	m_pBinLoader	= NULL;

	CIgnoreAlarmCodeTable::Instance()->CreateIgnoreAlarmCodeTable();
}

/////////////////////////////////////////////////////////////////
// The one and only CMS896AApp object
/////////////////////////////////////////////////////////////////
CMS896AApp theApp;

//CALLBACK
void CALLBACK callBackTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{ 
	theApp.SetTimers();
}

////////////////////////////////////////////////////////////////
// CMS896AApp initialization
/////////////////////////////////////////////////////////////////
BOOL CMS896AApp::InitInstance()
{
	extern BOOL KillProcess(CString exeName);
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	SFM_CModule::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	AfxInitRichEdit();

	// Check related application is already running or not
	if (IsApplicationRunning("MapSorter.exe", 2) == TRUE)
	{
		//AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
		m_bAppInit = FALSE;
		//return FALSE;
	}

	KillProcess("VISIONNT.exe");
	KillProcess("AsmHmi.exe");

	if (IsApplicationRunning("AsmHmi.exe", 1) == TRUE)
	{
		//AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
		m_bAppInit = FALSE;
		//return FALSE;
	}

	if (IsApplicationRunning("VISIONNT.exe", 1) == TRUE)
	{
		//AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
		m_bAppInit = FALSE;
		//return FALSE;
	}


	INT nTemp;
	
	SetRegistryKey(gszCOMPANY_NAME);
	
	//Get Application features
	CMSLogFileUtility::Instance()->MS_LogOperation("");		//v4.49A10
#ifdef NU_MOTION
	CMSLogFileUtility::Instance()->MS_LogOperation("MS100 Start: v" + m_szSoftVersion);		//v3.98T5
#else
	CMSLogFileUtility::Instance()->MS_LogOperation("MS899 Start: v" + m_szSoftVersion);		//v3.55
#endif
	GetMachineAllFeatures();	//Feature.msd

	if (!CheckVolumeInfo())		//v2.97		//Check CTNewEnhancement flag
	{
		return FALSE;
	}
	
	if (!CheckExpireDate())		//v3.57T2	//SanAn
	{
		return FALSE;
	}

	//Get MS Options File for special Hardware config
	//GetMSOptions();				//MSOptions.msd

	//AutoStart
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszPROFILE_AUTO_START, 0);
	nTemp ? m_fAutoStart = TRUE : m_fAutoStart = FALSE;

	// Initialize the hardware if auto-start is enabled
	if (m_fAutoStart)
	{
		m_bRunState = TRUE;
	}

	m_qState = UN_INITIALIZE_Q;


	//Log File
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszPROFILE_LOG_MSG, 1);
	nTemp ? m_fLogMessage = TRUE : m_fLogMessage = FALSE;

	//Execute Hmi
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszPROFILE_EXECUTE_HMI, 0);
	SetExecuteHmiFlag(nTemp);

	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate *pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMS896ADoc),
		RUNTIME_CLASS(CMainFrame), // main SDI frame window
		RUNTIME_CLASS(CMS896AView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
	{
		return FALSE;
	}

	// Get the Application Window Size
	m_pMainWnd->CenterWindow();
	m_pMainWnd->GetWindowRect(&m_stWinRect);
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	//Create InitOperation
	m_pInitOperation	= new CInitOperation;

	//Event
	m_evStart.ResetEvent();

	GetRegistryEntries();

	m_pInitOperation->SaveLogFilePath(m_szLogFilePath);
	m_pInitOperation->SaveLogFile(m_szLogFile);

	// Retrieve Device loading count & date
	int nCol = 0;
	CString szPKGCount = "";
	if ((m_szDeviceCount = GetProfileString(gszPROFILE_SETTING, gszDEVICE_COUNT_SETTING, _T(""))) == _T(""))
	{
		CTime theTime = CTime::GetCurrentTime();	
		m_szDeviceCount.Format("0:%d-%d-%d", theTime.GetDay(), theTime.GetMonth(), theTime.GetYear());
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_COUNT_SETTING, m_szDeviceCount);
	}

	if ((nCol = m_szDeviceCount.Find(":")) != -1)
	{
		szPKGCount = m_szDeviceCount.Left(nCol);
	}

	//Remove unused setting
	CSingleLock slLock(&m_csDispMsg);
	slLock.Lock();
	m_fCanDisplay = TRUE;
	slLock.Unlock();

	m_lBondTabPageSelect = 0;

	// Create the Message Log file used for debugging
	m_fLogFileCreated = m_pInitOperation->CreateLogFile();


	//Mark IDE Data Path
	LoadData();

	// File for SRAM
	m_smfSRam.Close();	
	//if (!m_smfSRam.Open("Sram.msd", FALSE, TRUE))
	if (!m_smfSRam.Open(gszEXE_DIRECTORY + "\\Sram.msd", FALSE, TRUE))	//v4.50A2
	{
		HmiMessage("Can not create SRam.msd file", "Create SRam", glHMI_MBX_OK);
		//SetAlert(IDS_MS_CANNOTCREATESRAMFILE);

		return FALSE;
	}

	//Update ShareMemory contents
	m_smfSRam["MS896A"]["Current Camera"]		= 0;
	m_smfSRam["MS896A"]["Expander Status"]		= FALSE;
	m_smfSRam["MS896A"]["Motor Power"]			= TRUE;

	m_smfSRam["MS896A"]["DevFileCount"]			= szPKGCount;
	m_smfSRam["MS896A"]["PKG Filename"]			= m_szDeviceFile;
	m_smfSRam["MS896A"]["MapFilePath"]			= m_szMapFilePath;
	m_smfSRam["MS896A"]["MapFileExt"]			= m_szMapFileExt;
	m_smfSRam["MS896A"]["MapSubFolderName"]		= m_szMapSubFolderName;
	m_smfSRam["MS896A"]["Operator Id"]			= m_szUsername;			//v3.24T1
	m_smfSRam["MS896A"]["Product Id"]			= m_szProductID;		//v3.24T1
	m_smfSRam["MS896A"]["Shift Id"]				= m_szShiftID;			//v3.32T3	//Avago
	m_smfSRam["MS896A"]["Software Version"]		= m_szSoftVersion;		//v3.86
	m_smfSRam["MS896A"]["OutputFilePath"]		= m_szOutputFilePath;
	m_smfSRam["MS896A"]["OutputFileFormat"]		= m_szOutputFileFormat;
	m_smfSRam["MS896A"]["WaferEndFilePath"]		= m_szWaferEndPath;
	m_smfSRam["MS896A"]["WaferEndFileFormat"]	= m_szWaferEndFormat;
	m_smfSRam["MS896A"]["MachineName"]			= m_szMachineName;		//v4.40T1	//Nichia
	m_smfSRam["MS896A"]["MachineNo"]			= m_szMachineNo;
	m_smfSRam["MS896A"]["Release Date"]			= gszRELEASE_DATE;		//v4.31T9	//EquipMgr
	m_smfSRam["MS896A"]["LotNumber"]			= m_szLotNumber;
	m_smfSRam["MS896A"]["LotStartTime"]			= m_szLotStartTime;
	m_smfSRam["MS896A"]["LotEndTime"]			= m_szLotEndTime;
	m_smfSRam["MS896A"]["RecipeFilePath"]		= m_szRecipeFilePath;	//v4.46T20	//PLSG 
	m_smfSRam["MS896A"]["DevicePath"]			= m_szDevicePath;		//v4.31T9	//EquipMgr
	m_smfSRam["MS896A"]["Bin Lot Directory"]	= m_szBinLotDirectory;
	m_smfSRam["MS896A"]["Associate File"]		= m_szAssociateFile;
	m_smfSRam["MS896A"]["Bin WorkNo"]			= m_szBinWorkNo;
	m_smfSRam["MS896A"]["Bin FileName Prefix"]	= m_szBinFileNamePrefix;
	m_smfSRam["MS896A"]["PLLMDieFabMESLot"]		= m_szPLLMDieFabMESLot;
	m_smfSRam["MS896A"]["Firmware1"]			= _T("");		//v3.86
	m_smfSRam["MS896A"]["Firmware2"]			= _T("");		//v3.86

	m_smfSRam["WaferTable"]["Load Map Date"]	= m_szLoadMapDate;
	m_smfSRam["WaferTable"]["Load Map Time"]	= m_szLoadMapTime;

	m_smfSRam["WaferTable"]["Map Start Date"]	= m_szMapStartDate;
	m_smfSRam["WaferTable"]["Map Start Time"]	= m_szMapStartTime;
	m_smfSRam["WaferTable"]["Map Run Time"]		= m_nMapRunTime;

	m_oToolsUsageRecord.SetOperatorId(m_szUsername);
	m_oToolsUsageRecord.SetMachineNo(m_szMachineNo);
	m_eqMachine.SetOperatorId(m_szUsername);
	m_eqMachine.SetMachineNo(m_szMachineNo);
	m_eqMachine2.SetOperatorId(m_szUsername);					//WH Sanan	//v4.40T4
	m_eqMachine2.SetMachineNo(m_szMachineNo);					//WH Sanan	//v4.40T4
	CMESConnector::Instance()->SetMachineNo(m_szMachineNo);
	CMSLogFileUtility::Instance()->SetMachineNo(m_szMachineNo);	//	427TX	1
	CMSLogFileUtility::Instance()->MS_LogOperation("User Name	: "	+ m_szUsername);	//v4.46T26
	CMSLogFileUtility::Instance()->MS_LogOperation("Machine Name: " + m_szMachineNo);	//v4.46T26

	//v4.42T16	//v4.48A2
	if (m_bIsMSLicenseRegistered)
	{
		ValidateMSLicenseKey();
	}
	//CreateMSLicenseStringFile();
	//RegisterMSLicenseKey();
	//ValidateMSLicenseKey();
	
	CreatePrinterSelectionFile();

	//Load app. data
	LoadAppData();
	LoadFileFormatSelectInNewLot();

	//Load Wafermap HeaderInfo
	LoadMapHeaderInfo();
	
	//Get Application features
	GetMachineAllFeatures();

	if (m_bOperatorLogOnValidation == TRUE)
	{
		m_szUsername.Empty();
	}

	//if (m_bNewCtEnhancement == TRUE)	//v3.28T1
	//{
	//	DisplayMessage("New SIO configuration is used...");
	//}

	//v4.56 moved to here
	//Get MS Options File for special Hardware config
	GetMSOptions();				//MSOptions.msd

	CreateLoadingAlert();		//v4.02T4

	//Read Screen control access mode
	ReadScreenControlAccessMode();

	//Setup Host Communication
	SetupHostComm();

	LoadAlarmInformation(gszROOT_DIRECTORY + "\\Exe\\AlarmInfo\\AlarmCode.csv");
	//DumpLoadAlarmInformation(gszROOT_DIRECTORY + "\\Exe\\AlarmInfo\\AlarmCodeLog.csv");
	
	CreateDirectory(gszROOT_DIRECTORY + "\\Exe\\WaferEnd", NULL);		//Create WaferEnd directory
	//Create UserData directory
	CreateDirectory(gszUSER_DIRECTORY, NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\OutputFile", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\OutputFile\\ClearBin", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\OutputFile\\WaferEnd", NULL);
	CreateDirectory(gszUSER_DIR_MAPFILE_OUTPUT, NULL);
	CreateDirectory(gszOUTPUT_FILE_TEMP_PATH, NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\MapFile", NULL);

// prescan relative code	B
//	CreateDirectory(gszUSER_DIRECTORY + "\\MapFile\\Prescan", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\PrescanResult", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\Alarm Log", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\PrescanResult\\SaveImages", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\PrescanResult\\AI Log", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\DebResult", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\BackupMSD", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\CycleState", NULL);	//	427TX	1
// prescan relative code	E
	CreateDirectory(gszUSER_DIRECTORY + "\\History", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\Performance", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\Statistic", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\Error", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\NVRAM", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\Diagnostics", NULL);	
	CreateDirectory(gszUSER_DIRECTORY + "\\Diagnostics\\BH", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\Diagnostics\\WPR", NULL);
	CreateDirectory(gszUSER_DIRECTORY + "\\CtmLog", NULL);

	//check HMI security file
	if (_access(HMI_SECURITY_FILE, F_OK) != F_OK)
	{
		FILE *fp;
		if ((fp = fopen(HMI_SECURITY_FILE, "wt")) != NULL)
		{
			fclose(fp);
		}
	}

	SearchAndRemoveFiles(gszUSER_DIRECTORY + "\\Error", 100);
	
	if (!m_bAppInit)	//v3.97
	{
		//Klocwork	//v4.02T5
		CWnd *pWnd = AfxGetMainWnd();
		if (pWnd != NULL)
		{
			pWnd->PostMessage(WM_CLOSE, 0, 0);
		}
	}
	CMSLogFileUtility::Instance()->MS_LogCycleState("machien restart and initialization");	//	427TX	1
	timeSetEvent(1, 0, (LPTIMECALLBACK)callBackTimer, NULL, TIME_PERIODIC);

	return TRUE;
}

VOID CMS896AApp::SetTimers()
{
	m_lCurTime++;
}

LONG CMS896AApp::GetCurTime()
{
	return m_lCurTime;
}

LONG CMS896AApp::elapse_time(LONG lStartTime)
{
	return m_lCurTime - lStartTime;
}

int CMS896AApp::ExitInstance() 
{
	if (m_pInitOperation)
	{
		if (m_fLogFileCreated)
		{
			m_pInitOperation->RenameLogFile();
		}
		
		m_pInitOperation->EnableAllHardware(FALSE);
		delete m_pInitOperation;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("Exit: backup NVRAM");
	BackupToNVRAM();

	CreateLoadingAlert(FALSE);		//v4.02T4

	SetStatusMessage("MapSorter is safely closed");
	if (!m_bAppInit)	//v3.97
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("Exit: application fail to start");
		SetErrorMessage("MapSorter EXE Appliction fail to start");
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("Exit: close MSD files");
	m_smfDevice.Close();
	m_smfMachine.Close();

	m_smfSRam.Update();
	m_smfSRam.Close();

	//Close Machine statistic
	CMSLogFileUtility::Instance()->MS_LogOperation("Exit: close EQUIP TIME");

	m_eqMachine.Close();
	m_eqMachine2.Close();	//v4.40T4

	//Free up Motion control system
	if (m_fEnableHardware)
	{
#ifdef NU_MOTION
		CMSLogFileUtility::Instance()->MS_LogOperation("Exit: free NuMotion");
		FreeNuMotion();
#endif
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("Exit: DONE\n");
	return SFM_CModule::ExitInstance();
}

VOID CMS896AApp::UpdateStationData()
{
	//AfxMessageBox("CMS896AApp", MB_SYSTEMMODAL);

	m_oMS896ADataBlock.m_szLoginID = m_szUsername;
	m_oMS896ADataBlock.m_szMachineNo = m_szMachineNo;
	m_oMS896ADataBlock.m_szVersion	= m_szSoftVersion;

	SetMS896ADataBlock(m_oMS896ADataBlock);

	for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
	{
		CString	szName;
		SFM_CStation *pStation;
		CMS896AStn *pStn;

		m_pStationMap.GetNextAssoc(pos, szName, pStation);
		pStn = dynamic_cast<CMS896AStn*>(pStation);
		pStn->UpdateStationData();
	}
}

BOOL CMS896AApp::GenerateParametersRecord()
{

	UpdateStationData();
	return m_oToolsUsageRecord.GenearteParametersRecordFile();
}

/////////////////////////////////////////////////////////////////
// CMS896AApp message handlers
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//Group ID
/////////////////////////////////////////////////////////////////
CString	CMS896AApp::GetModuleID()
{
	return m_szModuleName;
}

CString	CMS896AApp::GetGroupID()
{
	return m_szGroupID;
}

VOID CMS896AApp::CreateDefaultFolder()
{
	CString szEMDataFolder = ROOT_DIRECTORY;
	szEMDataFolder += _T("\\UserData\\EM");

	if (_access(szEMDataFolder, F_OK) != F_OK)
	{
		_mkdir(szEMDataFolder);
	}

	CString szTempFileBackup;
	szTempFileBackup = gszUSER_DIRECTORY + "\\TempFileBackup";
	if (_access(szTempFileBackup, F_OK) != F_OK)
	{
		_mkdir(szTempFileBackup);
	}

	szTempFileBackup = gszUSER_DIRECTORY + "\\TempFileBackup\\Processing_TempFileBackupSingleBlk";
	if (_access(szTempFileBackup, F_OK) != F_OK)
	{
		_mkdir(szTempFileBackup);
	}

}


/////////////////////////////////////////////////////////////////
//Mark IDE Data Loading
/////////////////////////////////////////////////////////////////
VOID CMS896AApp::LoadData()
{
	try
	{
		CreateDefaultFolder();
		//Load data
		//m_szDataPath	= gszMARK_DATA_PATH + gszMARK_DATA_FILE_NAME;
		m_szMachineModel	= GetMachineInformation(MS896A_CFG_MODEL_NO);
		m_szDataPath		= gszMARK_DATA_PATH + GetMachineInformation(MS896A_CFG_MARK_FILENAME);

		//Get Hardware info
		m_lMotionPlatform	= GetMachineHardwareConfig(MS896A_CFG_HW_MOTION_PLATFORM);		//v3.61
		m_lCheckCoverSnr	= GetMachineHardwareConfig(MS896A_CFG_HW_CHECK_COVER_SNR);
		m_lBarCode1ComPort	= GetMachineHardwareConfig(MS896A_CFG_HW_BARCODE1_COM);
		m_lBarCode2ComPort	= GetMachineHardwareConfig(MS896A_CFG_HW_BARCODE2_COM);
		m_lBarCode3ComPort	= GetMachineHardwareConfig(MS896A_CFG_HW_BARCODE3_COM);
		m_lBarCode4ComPort	= GetMachineHardwareConfig(MS896A_CFG_HW_BARCODE4_COM);
		m_lMainCoverLock	= GetMachineHardwareConfig(MS896A_CFG_HW_MAIN_COVER_LOCK);
		m_lSideCoverLock	= GetMachineHardwareConfig(MS896A_CFG_HW_SIDE_COVER_LOCK);

		m_lBinElevatorCoverLock = GetMachineHardwareConfig(MS896A_CFG_HW_BIN_ELEVATOR_LOCK);
		m_lHardwareConfigNo	= GetMachineHardwareConfig(MS896A_CFG_HW_CONFIG_NO);
		m_lBondHeadConfig	= GetMachineHardwareConfig(MS896A_CFG_HW_BOND_HEAD_CONFIG);
		m_lBondHeadMounting = GetMachineHardwareConfig(MS896A_CFG_HW_BOND_HEAD_MOUNTING);
		//m_bUseSlowBTControlProfile = GetMachineHardwareConfig(MS896A_CFG_HW_SLOW_BT_CTRL);
		m_bIsPrototypeMachine			= GetMachineHardwareConfig(MS896A_CFG_HW_PROTOTYPE_MACHINE);		//prototype MS100
		m_bIsNuMotionSingleArmMachine	= GetMachineHardwareConfig(MS896A_CFG_HW_NUMOTION_SINGLE_ARM);
		m_bEnableBHSuckingHead			= GetMachineHardwareConfig(MS896A_CFG_HW_BOND_HEAD_SUCKINGHEAD);	//v4.05
		m_bUseALBondArm					= GetMachineHardwareConfig(MS896A_CFG_HW_AL_BOND_ARM);
		m_bTableXYMagneticEncoder		= GetMachineHardwareConfig(MS896A_CFG_HW_TABLE_XY_MAGNETIC_ENCODER);
		m_bBinTableTHighResolution		= GetMachineHardwareConfig(MS896A_CFG_HW_BIN_TABLE_T_HIGH_RESOLUTION);
		m_bNewAutoCleanColletSystem		= GetMachineHardwareConfig(MS896A_CFG_HW_NEW_AUTO_CLEAN_COLLET);
		m_bUseExpanderWaferTable		= GetMachineHardwareConfig(MS896A_CFG_HW_WAFERTBALE_WITH_EXPANDER);
		m_bEnableThetaWithBrake			= GetMachineHardwareConfig(MS896A_CFG_HW_THETA_WITH_BRAKE);

		m_lCycleSpeedMode = 0;
		if (m_szMachineModel.Find("HD") != -1)		//v3.68T1	//Use SPEED-MODE for all HD config 90/180
		{
			m_lCycleSpeedMode = 1;
		}

		m_b180Arm6InchWaferTable = FALSE;
		if ((m_szMachineModel.Find("180") != -1) && (m_szMachineModel.Find("DLA") != -1))
		{
			m_b180Arm6InchWaferTable = TRUE;
		}

#ifdef NU_MOTION
		
		m_lCycleSpeedMode = 3;			//Release 100ms FAST-mode for production		//v3.97

		m_bMS100Plus9InchOption		= GetMachineHardwareConfig(MS896A_CFG_HW_MS100_9INCH_OPTION);
		m_bES100v2DualWftOption		= GetMachineHardwareConfig(MS896A_CFG_HW_ES100_DWFT_OPTION);
		m_bMS100SingleLoaderOption	= GetMachineHardwareConfig(MS896A_CFG_HW_MS100_SLOADER_OPTION);
		m_bESAoiSingleWT			= GetMachineHardwareConfig(MS896A_CFG_HW_ES201_SWFT_OPTION);
		m_bMSAutoLineMode			= GetMachineHardwareConfig(MS896A_CFG_HW_MS_AUTOLINE_MODE);
		
		LONG lCGMaxIndex			= GetMachineHardwareConfig(MS896A_CFG_HW_CG_MAX_INDEX); 
		m_lChangeColletMaxIndex		= 20;
		if (lCGMaxIndex != 0)
		{
			m_lChangeColletMaxIndex = lCGMaxIndex;
		}
		m_bMS_StandByButtonPressed_HMI = FALSE;
		if (m_bMSAutoLineMode == 1)
		{
			m_bMS_SISMode_HMI = TRUE;
			m_bMS_SISStandaloneMode_HMI = FALSE;
		}

		if (m_bMSAutoLineMode == 2)
		{
			m_bMS_SISMode_HMI = FALSE;
			m_bMS_SISStandaloneMode_HMI = TRUE;
		}


		//v4.27T1
		LONG lMS100Plus	= GetMachineHardwareConfig(MS896A_CFG_HW_NUMOTION_100PLUS);		//v4.01
		if (lMS100Plus >= 4)				//MS90 65ms cycle time, MS50	//v4.59A41
		{
			m_lCycleSpeedMode = 6;
		}	
		else if (lMS100Plus == 3)			//MS60/MS100P3 65ms cycle time
		{
			m_lCycleSpeedMode = 6;
		}	
		else if (lMS100Plus == 2)			//MS100PlusII 70ms cycle time
		{
			m_lCycleSpeedMode = 5; 
		}	
		else if (lMS100Plus == 1)			//MS100Plus 80ms cycle time
		{
			m_lCycleSpeedMode = 4; 
		}	

		if (lMS100Plus == 5)				//v4.59A41	//MS50
		{
			m_bMS50	= TRUE;
			m_bMS60	= TRUE;
			m_bMS90	= TRUE;
		}
		else if (lMS100Plus == 4)			//v4.48A27	//MS90
		{
			m_bMS60	= TRUE;
			m_bMS90	= TRUE;
		}
		else if (lMS100Plus == 3)			//MS60
		{
			m_bMS60	= TRUE;
			//CMS896AStn::m_bDBHThermalCheck = TRUE;	//v4.49A6
			//m_bWaferPRMxNLFWnd	= TRUE;		//v4.47T1
		}
		if (lMS100Plus > 0)
		{
			m_bMS100Plus	= TRUE;			//v4.35T1
		}		

		// Move the pin from APP to STN

		if (m_bES100v2DualWftOption && (m_bESAoiSingleWT == FALSE))
		{
			m_bESAoiDualWT = TRUE;
		}
		else
		{
			m_bESAoiDualWT = FALSE;
		}

		//v4.52A16	
		if (m_bEnableBHPostSealOptics)	//v4.40T1
		{
			//Now PostSealOptics(Uplook camera) available on MS60/90 for CSP package
			if (!m_bMS100Plus9InchOption && !m_bMS60 && !m_bMS90)
				m_bEnableBHPostSealOptics = FALSE;
		}
		else if (m_bEnableBHUplookPrFcn)		
		{
			m_bEnableBHPostSealOptics = TRUE;
		}

#else
		if (m_lCycleSpeedMode > 0)
		{
			m_szDataPath	= gszMARK_DATA_PATH + _T("Mark_Fast.mrk");    //v3.63
		}						
#endif
		m_pInitOperation->LoadData(m_szDataPath);

		m_szModuleName	= m_pInitOperation->GetModuleID();
		if ((m_szGroupID == gszDEFAULT_GROUP_ID)	&& 
				(m_pInitOperation->GetGroupID() != _T("")))
		{
			m_szGroupID	= m_pInitOperation->GetGroupID();
			SaveGroupId(m_szGroupID);
		}
		else if (m_szGroupID == "")
		{
			(m_pInitOperation->GetGroupID() == _T("")) ?
			m_szGroupID	= gszDEFAULT_GROUP_ID :
						  m_szGroupID	= m_pInitOperation->GetGroupID();

			SaveGroupId(m_szGroupID);
		}

		//v3.60		//Disable CoverLock if it is disabled in Feature file
		if (!m_bEnableCoverLock)
		{
			m_lMainCoverLock = 0;
			m_lSideCoverLock = 0;
			m_lBinElevatorCoverLock = 0;
		}

		CMS896AView *pView = (CMS896AView*)((CMainFrame*) m_pMainWnd)->GetActiveView();
		pView->SetGroupID(m_szGroupID);
		pView->SetModule(m_szModuleName);

		//Display message on dialog
		CString szLog;
		//szLog.Format("MS: Motion Platform = %d", m_lMotionPlatform);
		//v3.87
#ifdef NU_MOTION
		szLog = _T("MS: Motion Platform = NU_MOTION");
		m_lMotionPlatform = SYSTEM_NUMOTION;		//v4.05
#else
		if (m_lMotionPlatform == SYSTEM_NUMOTION)
		{
			szLog = _T("MS: Motion Platform = NU_MOTION");
		}
		else if (m_lMotionPlatform == SYSTEM_HIDRIVE)
		{
			szLog = _T("MS: Motion Platform = HIDRIVE");
		}
		else
		{
			szLog = _T("MS: Motion Platform = HIPEC");
		}
#endif
		DisplayMessage(szLog);
		CMSLogFileUtility::Instance()->MS_LogOperation("    " + szLog);			//v4.46T26

		if (m_lCycleSpeedMode > 0)
		{
			szLog.Format("MS: Speed mode = %d, Change Collet Max Index,%d", m_lCycleSpeedMode, m_lChangeColletMaxIndex);
			DisplayMessage(szLog);
			CMSLogFileUtility::Instance()->MS_LogOperation("    " + szLog);		//v4.46T26
		}

		if (m_bMS50)
		{
			DisplayMessage(_T("MS50 Configuration"));
			CMSLogFileUtility::Instance()->MS_LogOperation("    MS50 Configuration");
		}
		else if (m_bMS60 && m_bMS90)
		{
			DisplayMessage(_T("MS90 Configuration"));			//v4.48A27
			CMSLogFileUtility::Instance()->MS_LogOperation("    MS90 Configuration");
		}
		else if (m_bMS60)
		{
			if (m_bMSAutoLineMode == 1)
			{
				DisplayMessage(_T("MS60 Configuration (AUTOLINE)"));	//v4.56A1
				CMSLogFileUtility::Instance()->MS_LogOperation("    MS60 Configuration (AUTOLINE)");
			}
			else
			{
				DisplayMessage(_T("MS60 Configuration"));				//v4.46T21
				CMSLogFileUtility::Instance()->MS_LogOperation("    MS60 Configuration");
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}


/////////////////////////////////////////////////////////////////
//Hardware Init
/////////////////////////////////////////////////////////////////
BOOL CMS896AApp::InitHardware()
{
	BOOL			bInit = FALSE;
	CString			szStn;
	SFM_CStation	*pStn;
	CAppStation	*pAppStn;
	CMS896AStn		*pTemp;
	BOOL			bInitNVRam;
	try
	{
		//Display Software Ver, Machine model and Release Date
		CTime CurTime = CTime::GetCurrentTime();

		DisplayMessage("Date/Time: " + CurTime.Format("%H:%M:%S %d/%m/%y"));		
		DisplayMessage("OS Image Version: " + m_szOSImageReleaseNo);		
		DisplayMessage("Machine Model: " + m_szMachineModel);
		DisplayMessage("Software Version: " + m_szSoftVersion);
		DisplayMessage("Release Date: " + m_szSoftwareReleaseDate);
		DisplayMessage("Release Time: " + m_szSoftwareReleaseTime);

		(m_smfSRam)["MS896A"]["Machine Model"] = m_szMachineModel;

		m_pPrGeneral = new CPrGeneral;

		//Setup the IPC comms and Register Variables
		SetupIPC();
		RegisterVariables();

		if (!m_bAppInit)	//v3.97
		{
			m_fEnableHardware = FALSE;
			return FALSE;
		}

		//Init Motion system
		if (m_fEnableHardware)
		{
#ifdef NU_MOTION
			DisplayMessage("Motion Platform: NU MOTION");
			if (InitNuMotion() == FALSE)
			{
				return FALSE;
			}
#else
			DisplayMessage("Motion Platform: HIPEC SYSTEM");
			if (InitHiPECSystem() == FALSE)
			{
				return FALSE;
			}
#endif
		}
		else
		{
			m_pInitOperation->EnableAllHardware(FALSE);
		}

		// fail to init NVRam, Cannot start the application
		bInitNVRam = InitMS899NVRAM();
		if (m_fEnableHardware && bInitNVRam == FALSE)
		{
			DisplayMessage("NVRam:Init NVRam fail");
			return FALSE;
		}

#ifndef NU_MOTION	
		//v3.82		//v3.97	//For MS899 only
		//DisplayMessage("Detecting USB connection ...");
		//CheckMatrixDongle();	//Try to read dougle once again for display purpose
#endif

		//Register Station Class
		//ADD_STATION_HERE, Do not remove or modify this comment, it will be used by MARK-ADD-IN to add a Station
/*
		m_pInitOperation->RegisterStnClass("CWaferTable",		RUNTIME_CLASS(CWaferTable));
		m_pInitOperation->RegisterStnClass("CBondHead",			RUNTIME_CLASS(CBondHead));
		m_pInitOperation->RegisterStnClass("CBinTable",			RUNTIME_CLASS(CBinTable));
		m_pInitOperation->RegisterStnClass("CWaferPr",			RUNTIME_CLASS(CWaferPr));
		m_pInitOperation->RegisterStnClass("CBondPr",			RUNTIME_CLASS(CBondPr));
		m_pInitOperation->RegisterStnClass("CSecsComm",			RUNTIME_CLASS(CSecsComm));
		m_pInitOperation->RegisterStnClass("CWaferLoader",		RUNTIME_CLASS(CWaferLoader));
		m_pInitOperation->RegisterStnClass("CSafety",			RUNTIME_CLASS(CSafety));
		m_pInitOperation->RegisterStnClass("CBinLoader",		RUNTIME_CLASS(CBinLoader));
*/
		m_pWaferTable	= (CWaferTable *)	RegStnClass("CWaferTable",	RUNTIME_CLASS(CWaferTable),		WAFER_TABLE_STN);
		m_pBondHead		= (CBondHead *)		RegStnClass("CBondHead",	RUNTIME_CLASS(CBondHead),		BOND_HEAD_STN);
		m_pBinTable		= (CBinTable *)		RegStnClass("CBinTable",	RUNTIME_CLASS(CBinTable),		BIN_TABLE_STN);
		m_pWaferPr		= (CWaferPr *)		RegStnClass("CWaferPr",		RUNTIME_CLASS(CWaferPr),		WAFER_PR_STN);
		m_pBondPr		= (CBondPr *)		RegStnClass("CBondPr",		RUNTIME_CLASS(CBondPr),			BOND_PR_STN);
		m_pSecsComm		= (CSecsComm *)		RegStnClass("CSecsComm",	RUNTIME_CLASS(CSecsComm),		SECS_COMM_STN);
		m_pWaferLoader	= (CWaferLoader *)	RegStnClass("CWaferLoader",	RUNTIME_CLASS(CWaferLoader),	WAFER_LOADER_STN);
		m_pSafety		= (CSafety *)		RegStnClass("CSafety",		RUNTIME_CLASS(CSafety),			SAFETY_STN);
		m_pBinLoader	= (CBinLoader *)	RegStnClass("CBinLoader",	RUNTIME_CLASS(CBinLoader),		BIN_LOADER_STN);
		m_pNVCLoader	= (CNVCLoader *)	RegStnClass("CNVCLoader",	RUNTIME_CLASS(CNVCLoader),		NVC_LOADER_STN);

		//Create Station and Sub Station
//		m_pInitOperation->CreateStation();
//		DisplayMessage("Completed MS Stations setup ...");

		//Relay to Stn & Sub Stn: Update Pos flag, Profile flag, and m_fHardware
		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			m_pStationMap.GetNextAssoc(pos, szStn, pStn);
			pAppStn = dynamic_cast<CAppStation*>(pStn);

			if (pAppStn)
			{
				pAppStn->SetUpdatePosition(m_fEnableHardware, TRUE);
				pAppStn->SetUpdateProfile(m_fEnableHardware, TRUE);
				pAppStn->SetHardware(m_fEnableHardware, TRUE);
			}
			// Set the Appication pointer to wafer map event for loading map
			// Remark: Set one time is enough. Here is only for convenient
			pTemp = dynamic_cast<CMS896AStn*>(pStn);
			if (pTemp)
			{
				pTemp->m_WaferMapEvent.SetApp(this);
			}
		}

		m_evStart.SetEvent();
		bInit = TRUE;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return bInit;
}

CAppStation *CMS896AApp::RegStnClass(const CString &szClassName, CRuntimeClass *pRunTimeClass, const CString &szStnName)
{
	m_pInitOperation->RegisterStnClass(szClassName, pRunTimeClass);
	SFM_CStation   *pStn = NULL;
	CAppStation    *pAppStn = NULL;
	AddStation(pRunTimeClass, szStnName);

	if ((pStn = GetStation(szStnName)) != NULL)
	{
		pAppStn = (CAppStation*) pStn;
		pAppStn->SetInitOperation(m_pInitOperation);
		DisplayMessage("Added Station - " + szStnName);

		CWinThread *pThread = dynamic_cast<CWinThread*>(pStn); // [514V9-25-60#5 20151020]
		if (pThread != 0) 
		{
			//ofstream ofs("c:\\MapSorter\\UserData\\History\\ThreadID.log", ios::app);
			//ofs << pThread->m_nThreadID << "(0x" << hex << pThread->m_nThreadID << ")\tStn [" << (LPCTSTR)szStnName << "]" << endl;
		}

	}
	return pAppStn;
}

BOOL CMS896AApp::InitHiPECSystem()
{
	if (m_fEnableHardware)
	{
		SFM_CHipecDdbModule *pDDBModule = NULL;
		SFM_CHipecDualDspRevision pHiPECRevision;

		try
		{
			BOOL bIsBinLoaderEnabled = FALSE;
			bIsBinLoaderEnabled = (BOOL)(LONG)(m_smfSRam)["BinLoaderStn"]["Enabled"];

			//Init Hipec hardware
			m_pInitOperation->InitializeHipec();
			SFM_CHipecDdbModule *pDDBModule = m_pInitOperation->GetHipecDdbModule("Module0");
			DisplayMessage("Hipec Ring Network initialized ...");

			//Get HiPEC firmware version
			pDDBModule->ReportDualDspRevision(pHiPECRevision);
			m_szHiPECVersion.Format("Card1: V%1.2f", ((FLOAT)pHiPECRevision.m_wRevision) / 100);
			
			m_smfSRam["MS896A"]["Firmware1"] = m_szHiPECVersion;	//v3.86
			DisplayMessage("Hipec Firmware Version 1 - " + m_szHiPECVersion);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}

		//v3.70T1
		try
		{
			if ((pDDBModule = m_pInitOperation->GetHipecDdbModule("Module1")) != NULL)
			{
				pDDBModule->ReportDualDspRevision(pHiPECRevision);
				m_szHiPECVersion2.Format("Card2: V%1.2f", 
										 ((FLOAT)pHiPECRevision.m_wRevision) / 100);
			}

			m_smfSRam["MS896A"]["Firmware2"] = m_szHiPECVersion2;	//v3.86
			DisplayMessage("Hipec Firmware Version 2 - " + m_szHiPECVersion2);
		}
		catch (CAsmException e)
		{
			//DisplayException(e);
			m_szHiPECVersion2 = _T("");
		}
	}

	return TRUE;
}

VOID CMS896AApp::ConfigSystemActivation(const CString szOrgActPath, const CString szDestActPath)
{
	CStdioFile fIn, fOut;
	char szTemp[256], szTemp0[100], szTemp1[100];
	CString szMsg;
	if (fOut.Open(szDestActPath, CFile::modeCreate | CFile::modeWrite))
	{
		if (fIn.Open(szOrgActPath, CFile::modeRead))
		{
			while (fIn.ReadString(szTemp, 255))
			{
				sscanf(szTemp, "%99s\t%99s", szTemp0, szTemp1);

				//Bin Table T High Resolution
				if (m_bBinTableTHighResolution)
				{
					//Normal Bin Table
					if (strcmp(szTemp1, "\"BINTABLE\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELX\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELY\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}


					//Bin Table T High Resolution
					if (strcmp(szTemp1, "\"BINTABLE(VER_THRES)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELX(VER_THRES)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELY(VER_THRES)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELT(VER_THRES)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
				}
				else
				{
					//Normal Bin Table
					if (strcmp(szTemp1, "\"BINTABLE\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELX\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELY\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}

					//Bin Table T High Resolution
					if (strcmp(szTemp1, "\"BINTABLE(VER_THRES)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELX(VER_THRES)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELY(VER_THRES)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELT(VER_THRES)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
				}
/*
				if (m_bTableXYMagneticEncoder)
				{
					//Normal Wafer Table
					if (strcmp(szTemp1, "\"WAFERTABLE\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELX\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELY\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}

					//Normal Bin Table
					if (strcmp(szTemp1, "\"BINTABLE\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELX\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELY\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}


					//Manetic Encoder Wafer Table
					if (strcmp(szTemp1, "\"WAFERTABLE(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELX(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELY(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELT(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}

					//Manetic Encoder Bin Table
					if (strcmp(szTemp1, "\"BINTABLE(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELX(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELY(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELT(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}

				}
				else
				{
					//Normal Wafer Table
					if (strcmp(szTemp1, "\"WAFERTABLE\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELX\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELY\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}

					//Normal Bin Table
					if (strcmp(szTemp1, "\"BINTABLE\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELX\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELY\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}

*/
					//Manetic Encoder Wafer Table
					if (strcmp(szTemp1, "\"WAFERTABLE(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELX(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELY(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELT(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}

					//Manetic Encoder Bin Table
					if (strcmp(szTemp1, "\"BINTABLE(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELX(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELY(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BINTABLE_CHANNELT(VER_M_ENC)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}

//				}

				//fiducial Mark BondArm
				if (!m_bUseALBondArm)
				{
					//Normal Bond Arm
					if (strcmp(szTemp1, "\"BONDHEAD\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELZ1\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELZ2\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_Z1_MISSING_DIE\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_Z2_MISSING_DIE\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_T_THERMOSTAT\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}

					//fiducial Mark BondArm
					if (strcmp(szTemp1, "\"BONDHEAD(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELT(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELZ1(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELZ2(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_Z1_MISSING_DIE(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_Z2_MISSING_DIE(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_T_THERMOSTAT(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
				}
				else
				{
					//Normal Bond Arm
					if (strcmp(szTemp1, "\"BONDHEAD\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELZ1\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELZ2\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_Z1_MISSING_DIE\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_Z2_MISSING_DIE\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_T_THERMOSTAT\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}

					//fiducial Mark BondArm
					if (strcmp(szTemp1, "\"BONDHEAD(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELT(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELZ1(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_CHANNELZ2(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_Z1_MISSING_DIE(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_Z2_MISSING_DIE(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"BONDHEAD_ADC_PORT_T_THERMOSTAT(VER_FM)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
				}
			

				//wafer table with Expander or Ring
				if (m_bUseExpanderWaferTable)
				{
					//Standard table
					if (strcmp(szTemp1, "\"WAFERTABLE\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELX\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELY\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}

					//Table with expander
					if (strcmp(szTemp1, "\"WAFERTABLE(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELX(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELY(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELT(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELZ(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
				}
				else
				{
					if (strcmp(szTemp1, "\"WAFERTABLE\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELX\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELY\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELT\"") == 0)
					{
						sprintf(szTemp, "1\t%s\n", szTemp1);
					}

					//Table with expander
					if (strcmp(szTemp1, "\"WAFERTABLE(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELX(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELY(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELT(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
					if (strcmp(szTemp1, "\"WAFERTABLE_CHANNELZ(VER_EXPANDER)\"") == 0)
					{
						sprintf(szTemp, "0\t%s\n", szTemp1);
					}
				}


				fOut.WriteString(szTemp);
			}
			fIn.Close();
		}
		fOut.Close();
	}
}


BOOL CMS896AApp::InitNuMotion()
{
	if (!m_fEnableHardware)
	{
		return TRUE;
	}

	try
	{
#ifdef NU_MOTION
/*
		// Sim Card Validation
		ULONG ulSimContent[NU_SIM_CONTENT_SIZE];
		for (INT i = 0; i < NU_SIM_CONTENT_SIZE; i++)	//Klocwork	//v4.02T5
		{
			ulSimContent[i] = 0;
		}

		GMODE_SUITE emSuite;
		//Get Sim card type
		GetSystemSuite(&emSuite);
		m_ucNuSimCardType = (UCHAR)emSuite;

		if (m_ucNuSimCardType == GMODE_INVALID_SUITE)
		{
			DisplayMessage("Start to init Nu Motion fail as Sim Card Type is invalid suite.");
			return FALSE;
		}
		else if (m_ucNuSimCardType == GMP_NORMAL_SUITE)
		{
			DisplayMessage("Start to init Nu Motion");
			GetMcConfig(ulSimContent);
		}

		//v4.40T14
		CString szTemp, szBit;
		for (INT m=0; m<16; m++)
		{
			if (ulSimContent[m] > 99)
				szBit = "NA";
			else
				szBit.Format("%lu", ulSimContent[m]);

			if (m == 0)
				szTemp = "SIM Code = " + szBit;
			else
				szTemp = szTemp + "-" + szBit;
		}
		CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);

		if (m_lCycleSpeedMode >= 5)					//MS100PlusII		//v4.40T14	//v4.50A17
		{
			ULONG ulBit7 = ulSimContent[6];
			ULONG ulBit8 = ulSimContent[7];
			if ( (ulBit7 == 7) && (ulBit8 == 8) )	//MS100+ 72ms mode SIM card
			{
				//v4.50A17  //Check Special Features 
				ULONG ulBit9 = ulSimContent[8];
				if (ulBit9 != 9)	//If not DEFAULT value
				{
					if (ulBit9 == 1)	//Feature #1: Diamond-Die Prescan
					{
						//Enable Diamond Die Bonding fcn

					}
					else
					{
						CTime cTime;
						cTime = CTime::GetCurrentTime();
						if ( (cTime.GetYear() == 2015) && 
							((cTime.GetMonth() == 6) || (cTime.GetMonth() == 7)) )
						{
							//also Enable Diamond-Die Bonding fcn within this period
						}
					}
				}
			}
			else
			{
				DisplayMessage("NU ERROR: Invalid SIM card version !!");
				return FALSE;
			}
		}
		else if (m_lCycleSpeedMode >= 3)			//MS100 120ms CT	//v4.46T19		//Focus Lighting
		{
			ULONG ulBit7 = ulSimContent[6];
			ULONG ulBit8 = ulSimContent[7];
			if ( (ulBit7 == 7) && (ulBit8 == 6) )	//Change MS100 to MS100Plus mode	//Focus Lighting
			{
				DisplayMessage("NU SPEED CODE upgraded from 3 to 4");
				m_lCycleSpeedMode = 4;
			}
		}
		else
		{
			ULONG ulMode = ulSimContent[4];
			if (ulMode > 0)
			{
				CString szTemp;
				szTemp.Format("NU SPEED CODE = %d", ulMode);
				DisplayMessage(szTemp);
			}
		}
*/
#ifndef OFFLINE
		if (!CheckNuMotionSIMCard())	//v4.50A17
		{
			return FALSE;
		}
#endif
//CMS896AStn::m_bEnableRhombusDie = TRUE;

		GMP_S16 ssReturn = 0;
		GMP_CHAR pcSCFFile[255], pcActFile[255], pcActFile1[255];

		errno_t nErr = strcpy_s(pcSCFFile, sizeof(pcSCFFile), gszNU_DATA_PATH + GetMachineInformation(MS896A_CFG_SCF_FILENAME, MS896A_CFG_NU_CONFIG));
		strcpy_s(pcActFile, sizeof(pcActFile), gszNU_DATA_PATH + GetMachineInformation(MS896A_CFG_ACTIVATOIN_FILENAME, MS896A_CFG_NU_CONFIG));

		strcpy_s(pcActFile1, sizeof(pcActFile1), gszNU_DATA_PATH + "Act.ini");

		//ConfigSystemActivation(pcActFile, pcActFile1);

#ifndef OFFLINE
		ssReturn = gmp_init_system(pcSCFFile, pcActFile1);
#endif		
		if (ssReturn != 0)
		{
			CString szPara;

			szPara.Format("%s, %s", pcSCFFile, pcActFile);
			CAsmException e((UINT)ssReturn, "gmp_init_system", szPara);
			throw e;
		}
		else
		{
			//Get I/O config
			CMSFileUtility  *pUtl = CMSFileUtility::Instance();
			CStringMapFile  *psmf = NULL;
			CString szPort, szBit;
			CString szChName;
			CHAR pChName[GMP_CH_NAME_CHAR_LEN];
			CHAR pSubVersion[GMP_REVISION_NUMBER_CHAR_LEN];
			GMP_U32 uiReturn;
			GMP_FLOAT fVersion;
			GMP_FLOAT fBuildVersion;
			GMP_U32 uiApplicationID;

			//load & get config file ptr
			pUtl->LoadMachineConfig(); 

			psmf = pUtl->GetMachineConfigFile();
			if (psmf == NULL)
			{
				DisplayMessage("NU Motion fail to open machine config file");
				return FALSE;
			}

			//Get SI
			for (LONG i = 0; i < UM_MAX_SI_PORT; i++)
			{
				szPort.Format("%s_%d", MS896A_CFG_IO_PORT, i);
				m_NmSIPort[i].m_szName = (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_IO_SI][szPort];

				for (LONG j = 0; j < UM_MAX_IO_BIT; j++)
				{
					if (j < 10)
					{
						szBit.Format("%s_0%d", MS896A_CFG_IO_BIT, j);
					}
					else
					{
						szBit.Format("%s_%d", MS896A_CFG_IO_BIT, j);
					}
					m_NmSIPort[i].m_szBit[j] = (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_IO_SI][szPort][szBit];
				}
			}

			//Get SO
			for (LONG i = 0; i < UM_MAX_SO_PORT; i++)
			{
				szPort.Format("%s_%d", MS896A_CFG_IO_PORT, i);
				m_NmSOPort[i].m_szName = (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_IO_SO][szPort];

				for (LONG j = 0; j < UM_MAX_IO_BIT; j++)
				{
					if (j < 10)
					{
						szBit.Format("%s_0%d", MS896A_CFG_IO_BIT, j);
					}
					else
					{
						szBit.Format("%s_%d", MS896A_CFG_IO_BIT, j);
					}
					m_NmSOPort[i].m_szBit[j] = (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_IO_SO][szPort][szBit];
				}
			}

			//Get CH Port
			m_NmCHPort[MS896A_ENC_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_ENC];
			m_NmCHPort[MS896A_MODE_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_MODE];
			m_NmCHPort[MS896A_CMD_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_CMD];
			m_NmCHPort[MS896A_DAC_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_DAC];
			m_NmCHPort[MS896A_ERR_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_ERR];

			//Get SW Port
			for (LONG i = 0; i < UM_MAX_SW_PORT; i++)
			{
				szPort.Format("%s_%d", MS896A_CFG_IO_PORT, i);
				m_NmSWPort[i].m_szName = (CString)(*psmf)[MS896A_CFG_NU_CONFIG][MS896A_CFG_IO][MS896A_CFG_IO_SW][szPort];
			}

			//Get One of the channel name to get back the RTC version
			szChName = (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][MS896A_CFG_CH_WAFTABLE_Y][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_NAME];	//v4.03

			pUtl->CloseMachineConfig();


			//Get RTC version
			strcpy_s(pChName, sizeof(pChName), szChName);
			for (LONG i = 0; i < GMP_REVISION_NUMBER_CHAR_LEN; i++)
			{
				pSubVersion[i] = '\0';
			}
#ifndef OFFLINE
			uiReturn = gmp_rpt_rtc_version_info(pChName, &uiApplicationID, &fVersion, pSubVersion, &fBuildVersion);
#else
			uiReturn = 0;
#endif
			if (uiReturn != 0)
			{
				CAsmException e(uiReturn, "gmp_rpt_rtc_version_info", szChName);
				throw e;
				return FALSE;
			}


			m_szHiPECVersion.Format("Nu V%3.2f%s%3.2f", fVersion, pSubVersion, fBuildVersion);
			m_smfSRam["MS896A"]["Firmware1"] = m_szHiPECVersion;	//v3.86
			DisplayMessage("Nu Firmware Version " + m_szHiPECVersion);
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		DisplayMessage("Nu Motion exception fail.");
		return FALSE;
	}

	return TRUE;
}

BOOL CMS896AApp::CheckNuMotionSIMCard()			//v4.50A17
{
	return TRUE;
#ifdef NU_MOTION

	// Sim Card Validation
	ULONG ulSimContent[NU_SIM_CONTENT_SIZE];
	for (INT i = 0; i < NU_SIM_CONTENT_SIZE; i++)	//Klocwork	//v4.02T5
	{
		ulSimContent[i] = 0;
	}

#ifndef MS50_64BIT

	GMODE_SUITE emSuite;
	
	//Get Sim card type
	GetSystemSuite(&emSuite);
	m_ucNuSimCardType = (UCHAR)emSuite;

	if (m_ucNuSimCardType == GMODE_INVALID_SUITE)
	{
		DisplayMessage("Start to init Nu Motion fail as Sim Card Type is invalid suite.");
		return FALSE;
	}
	else if (m_ucNuSimCardType == GMP_NORMAL_SUITE)
	{
		DisplayMessage("Start to init Nu Motion");
		GetMcConfig(ulSimContent);
	}

	//v4.40T14
	CString szTemp, szBit;
	for (INT m=0; m<16; m++)
	{
		if (ulSimContent[m] > 99)
			szBit = "NA";
		else
			szBit.Format("%lu", ulSimContent[m]);

		if (m == 0)
			szTemp = "SIM Code = " + szBit;
		else
			szTemp = szTemp + "-" + szBit;
	}
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);


	if (m_lCycleSpeedMode >= 3)			//MS100 120ms CT	//v4.46T19		//Focus Lighting
	{
		ULONG ulBit7 = ulSimContent[6];
		ULONG ulBit8 = ulSimContent[7];

		if ( (ulBit7 == 7) && (ulBit8 == 6) )	//Change MS100 to MS100Plus mode	//Focus Lighting
		{
			DisplayMessage("NU SPEED CODE upgraded from 3 to 4");
			m_lCycleSpeedMode = 4;

			//v4.50A17  //Check Special Features 
			ULONG ulBit9 = ulSimContent[8];
			if (ulBit9 & 0x1)		//Feature #1: Diamond-Die Prescan
			{
				//Enable Diamond Die Bonding fcn
				CMS896AStn::m_bEnableRhombusDie = TRUE;		//Silan
				m_bEnablePolygonDie = TRUE;					//v4.52A8
			}

			CheckDebSortMode(ulBit9);

			szTemp.Format("MS Feature (MS100Plus) (%s): PR-Rhombus-Die(%d), DEB(%d)", 
				GetCustomerName(), CMS896AStn::m_bEnableRhombusDie, CMS896AStn::m_bEnableDEBSort);
			CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
		}
		else if ( (ulBit7 == 7) && (ulBit8 == 8) )	//MS100+ 72ms mode SIM card
		{
			//v4.50A17  //Check Special Features 
			ULONG ulBit9 = ulSimContent[8];
			//if (ulBit9 != 9)	//If not DEFAULT value
			if (ulBit9 & 0x1)	//Feature #1: Diamond-Die Prescan
			{
				//Enable Diamond Die Bonding fcn
				CMS896AStn::m_bEnableRhombusDie = TRUE;		//Silan
				m_bEnablePolygonDie = TRUE;					//v4.52A8
			}

			CheckDebSortMode(ulBit9);

			szTemp.Format("MS Feature (MS100P2 & Above) (%s): PR-Rhombus-Die(%d), DEB(%d)", 
				GetCustomerName(), CMS896AStn::m_bEnableRhombusDie, CMS896AStn::m_bEnableDEBSort);
			CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
		}
		else
		{
			DisplayMessage("NU ERROR: Invalid SIM card version !!");
		}
	}
	else
	{
		ULONG ulMode = ulSimContent[4];
		if (ulMode > 0)
		{
			CString szTemp;
			szTemp.Format("NU SPEED CODE = %d", ulMode);
			DisplayMessage(szTemp);
		}
	}

#endif
#endif

	return TRUE;
}

BOOL CMS896AApp::CheckNuMotionSIMCardInAutoBond()
{
	return TRUE;

#ifdef MS50_64BIT

	return TRUE;

#else

	if (m_ucNuSimCardType != GMP_NORMAL_SUITE)
	{
		return FALSE;
	}

	ULONG ulSimContent[NU_SIM_CONTENT_SIZE];
	for (INT i = 0; i < NU_SIM_CONTENT_SIZE; i++)
	{
		ulSimContent[i] = 0;
	}

	//GMODE_SUITE emSuite;
	GetMcConfig(ulSimContent);

	CString szTemp, szBit;
	for (INT m=0; m<16; m++)
	{
		if (ulSimContent[m] > 99)
			szBit = "NA";
		else
			szBit.Format("%lu", ulSimContent[m]);

		if (m == 0)
			szTemp = "SIM Code (AUTOBOND) = " + szBit;
		else
			szTemp = szTemp + "-" + szBit;
	}
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);


	if (m_lCycleSpeedMode >= 4)					//MS100Plus, MS100PlusII & above
	{
		ULONG ulBit7 = ulSimContent[6];
		ULONG ulBit8 = ulSimContent[7];
		if ( (ulBit7 == 7) && (ulBit8 == 8) )	//MS100+ 72ms mode SIM card
		{
			//v4.50A17  //Check Special Features 
			ULONG ulBit9 = ulSimContent[8];

			if (ulBit9 == 1)	//Feature #1: Diamond-Die Prescan
			{
				//Enable Diamond Die Bonding fcn
				CMS896AStn::m_bEnableRhombusDie = TRUE;		//Silan
				m_bEnablePolygonDie = TRUE;					//v4.52A8
			}
			else
			{
				CMS896AStn::m_bEnableRhombusDie = FALSE;		//Silan
			}

			szTemp.Format("MS Feature: PR Rhombus Die Support (AUTOBOND) - %d (%s)", 
				CMS896AStn::m_bEnableRhombusDie, GetCustomerName());
			CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
		}
	}

#endif

	return TRUE;
}

VOID CMS896AApp::FreeNuMotion()
{
#ifdef NU_MOTION
	gmp_free_system();
#endif
}

/////////////////////////////////////////////////////////////////
// Start execution utilities
/////////////////////////////////////////////////////////////////
BOOL CMS896AApp::AutoStart()
{
	return m_fAutoStart;
}

BOOL CMS896AApp::SetRun()
{
	BOOL bResult = FALSE;

	if (m_qState == UN_INITIALIZE_Q)
	{
		m_bRunState = TRUE;
		bResult = TRUE;
	}
	else
	{
		DisplayMessage("Hardware already been initialized!");
	}

	return bResult;
}

VOID CMS896AApp::SaveAutoStart(BOOL bOption)
{
	INT nTemp = 0;

	if (bOption)
	{
		nTemp = 1;
	}

	WriteProfileInt(gszPROFILE_SETTING, gszPROFILE_AUTO_START, nTemp);
	m_fAutoStart = bOption;
}


/////////////////////////////////////////////////////////////////
// Option & Hardware Selection
/////////////////////////////////////////////////////////////////
VOID CMS896AApp::SetOptions()
{
	COptionDlg	dlg;

	// Initialize the Option dialog attributes
	if (dlg.DoModal() == IDOK)
	{
		// Obtain the selected options and save them
	}
}

VOID CMS896AApp::SelectHardware()
{
	CHardwareDlg hwDlg;

	hwDlg.m_bHardware			= m_fEnableHardware;
	hwDlg.m_bDisableBHModule	= m_bDisableBHModule;
	hwDlg.m_bDisableBTModule	= m_bDisableBTModule;
	hwDlg.m_bDisableBLModule	= m_bDisableBLModule;
	hwDlg.m_bDisableNLModule	= m_bDisableNLModule;
	hwDlg.m_bDisableWLModule	= m_bDisableWLModule;
	hwDlg.m_bDisableWTModule	= m_bDisableWTModule;
	hwDlg.m_bDisableWLModuleWithExp	= m_bDisableWLModuleWithExp;

	if (hwDlg.DoModal() == IDOK)
	{
		m_fEnableHardware = hwDlg.m_bHardware;
		WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ALL_HARDWARE, m_fEnableHardware);

		m_bDisableBHModule	= hwDlg.m_bDisableBHModule;
		WriteProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_BH_MODULE, m_bDisableBHModule);
		
		m_bDisableBTModule	= hwDlg.m_bDisableBTModule;
		WriteProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_BT_MODULE, m_bDisableBTModule);
		
		m_bDisableBLModule	= hwDlg.m_bDisableBLModule;
		WriteProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_BL_MODULE, m_bDisableBLModule);
		
		m_bDisableNLModule	= hwDlg.m_bDisableNLModule;
		WriteProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_NL_MODULE, m_bDisableNLModule);	//andrewng //2020-0707

		m_bDisableWLModule	= hwDlg.m_bDisableWLModule;
		WriteProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_WL_MODULE, m_bDisableWLModule);
		
		m_bDisableWTModule	= hwDlg.m_bDisableWTModule;
		WriteProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_WT_MODULE, m_bDisableWTModule);
		
		m_bDisableWLModuleWithExp	= hwDlg.m_bDisableWLModuleWithExp;
		WriteProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_WL_MODULE_WITH_EXP, m_bDisableWLModuleWithExp);
	}
}


/////////////////////////////////////////////////////////////////
// Log Message
/////////////////////////////////////////////////////////////////
BOOL CMS896AApp::LogMessage()
{
	return m_fLogMessage;
}

VOID CMS896AApp::SetLogMessage(BOOL bEnable)
{
	INT nTemp = 0;

	if (bEnable)
	{
		nTemp = 1;
	}

	WriteProfileInt(gszPROFILE_SETTING, gszPROFILE_LOG_MSG, nTemp);
	m_fLogMessage = bEnable;
}

VOID CMS896AApp::SetTotLogBackupFiles(LONG lUnit)
{
	WriteProfileInt(gszPROFILE_SETTING, gszLOG_BACKUP_SETTING, lUnit);
	m_pInitOperation->SetTotLogBackupFiles(lUnit);
}

VOID CMS896AApp::SetTotLogFileLine(LONG lUnit)
{
	WriteProfileInt(gszPROFILE_SETTING, gszLOG_FILE_LINE, 
					m_pInitOperation->SetTotLogFileLine(lUnit));
}

BOOL CMS896AApp::IsPortablePKGFile()
{
	return m_bPortablePKGFile;
}

BOOL CMS896AApp::IsManualLoadPkgFile()	//v4.53A22
{
	return m_bManualLoadPkgFile;
}

BOOL CMS896AApp::IsRuntime2PkgInUse()
{
	return m_bRuntimeTwoPkgInUse;
}

/////////////////////////////////////////////////////////////////
// Group ID
/////////////////////////////////////////////////////////////////
VOID CMS896AApp::SetGroupId(const CString &szGroup)
{
	m_szGroupID = szGroup;
	WriteProfileString(gszPROFILE_SETTING, gszPROFILE_GROUP_ID, szGroup);
}

VOID CMS896AApp::SaveGroupId(const CString &szGroup)
{
	WriteProfileString(gszPROFILE_SETTING, gszPROFILE_GROUP_ID, szGroup);
}

CString CMS896AApp::GetSoftVersion()
{
	return m_szSoftVersion;
}

CString CMS896AApp::GetMachineModel()
{
	return m_szMachineModel;
}

CString CMS896AApp::GetMachineNo()	//v4.48A4
{
	return m_szMachineNo;
}


/////////////////////////////////////////////////////////////////
// Check for motion completion
/////////////////////////////////////////////////////////////////
INT CMS896AApp::CheckStationMotion()
{
	SFM_CStation *pStation;
	CAppStation  *pAppStation;
	POSITION	  pos;
	CString		  szKey;
	INT			  nResult = MOTION_COMPLETE;

	for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
	{
		m_pStationMap.GetNextAssoc(pos, szKey, pStation);
		pAppStation = dynamic_cast<CAppStation*>(pStation);

		if (pAppStation->InMotion())
		{
			nResult = MOTION_RUNNING;
			break;
		}
	}

	return nResult;
}


/////////////////////////////////////////////////////////////////
// Execute HMI
/////////////////////////////////////////////////////////////////
VOID CMS896AApp::SetExecuteHmiFlag(BOOL bExecute)
{
	INT nTemp = 0;
	
	if (bExecute)
	{
		nTemp = 1;
	}

	WriteProfileInt(gszPROFILE_SETTING, gszPROFILE_EXECUTE_HMI, nTemp);
	m_fExecuteHmi = bExecute;
}

BOOL CMS896AApp::IsExecuteHmi()
{
	return m_fExecuteHmi;
}

VOID CMS896AApp::ExecuteHmi()
{
	if (m_fExecuteHmi)
	{
		STARTUPINFO			startupInfo;
		PROCESS_INFORMATION	processInfo;

		memset(&startupInfo, 0, sizeof(STARTUPINFO)); //set memory to 0
		startupInfo.cb = sizeof(STARTUPINFO);

		CreateProcess(NULL, "..\\AsmHmi\\AsmHmi.exe", NULL, NULL, FALSE, 
					  NORMAL_PRIORITY_CLASS, NULL, "..\\AsmHmi",
					  &startupInfo, &processInfo);

		CloseHandle(processInfo.hThread);		//Klocwork	//v4.27
	}
}

VOID CMS896AApp::ExecuteOfflineMapProgram()		//v4.58A5
{
	CString szProgramPath = gszEXE_DIRECTORY + "\\OfflinePath.exe";

	if (!CMS896AStn::m_WaferMapWrapper.IsMapValid())
	{
HmiMessage("ExecuteOfflineMapProgram: Map is not valid !!");
		SetErrorMessage("MS:ExecuteOfflineMapProgram: Map is not valid");
		return;
	}

	CString szGradeList = "";
	CUIntArray unaAvaGradeList;
	UCHAR ucGrade = 0;
	CString szGrade;

	CMS896AStn::m_WaferMapWrapper.GetSelectedGradeList(unaAvaGradeList);

	for (INT i=0; i<unaAvaGradeList.GetSize(); i++)
	{
		ucGrade = unaAvaGradeList.GetAt(i);
		if (i == (unaAvaGradeList.GetSize() - 1))
			szGrade.Format("%d", ucGrade);
		else
			szGrade.Format("%d,", ucGrade);
		szGradeList += szGrade;
	}

	// The command parameters are: 
	//	[FileName][SubRow][SubCol][CustomText][RegionByRegion][CustomOrder][FirstRegionOnly]
	//		[HoriFactor][VertFactor][CalTime]
	//		[StartRow][StartCol][PickGrades][SortByGradeCount]

	ULONG ulNoOfRows=0, ulNoOfCols=0;
	CMS896AStn::m_WaferMapWrapper.GetMapDimension(ulNoOfRows, ulNoOfCols);

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	LONG lStartRow=0, lStartCol=0;
	LONG lEncX=0, lEncY=0;
	pUtl->GetAlignPosition(lStartRow, lStartCol, lEncX, lEncY);

	CString szParam;
	szParam.Format("%s [%s][%lu][%lu][][TRUE][FALSE][FALSE][1][1][60][%ld][%ld][%s][TRUE]", 
					szProgramPath,
					CMS896AStn::m_WaferMapWrapper.GetFileName(), 
					ulNoOfRows, ulNoOfCols, 
					lStartRow, lStartCol,
					szGradeList);

	CMSLogFileUtility::Instance()->MS_LogOperation("MS: Excuting - "		+ szParam);		//v4.49A5

	if (IsApplicationRunning(szProgramPath, 1, TRUE) != TRUE)
	{
HmiMessage("Executing OfflinePath EXE ....");

		STARTUPINFO StartupInfo;
		PROCESS_INFORMATION ProcInfo; 

		memset(&StartupInfo, 0, sizeof(STARTUPINFO));
		StartupInfo.cb = sizeof(STARTUPINFO);

		BOOL bProcess = CreateProcess(NULL, szParam.GetBuffer(szParam.GetLength() + 1), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, gszEXE_DIRECTORY, &StartupInfo, &ProcInfo);
		//CloseHandle(ProcInfo.hThread);

		if (bProcess)
		{
			HmiMessage("OfflinePath EXE is executed.");
		}
		else
		{
			CString szError;
			szError.Format("OfflinePath EXE fails to execute - ErrCode = %d", GetLastError());
			HmiMessage(szError);
		}
	}
	else
	{
HmiMessage("OfflinePath EXE is already running !!");
	}

}


////////////////////////////////////////////////////////////////////////////////////
//Get Application Features information 
////////////////////////////////////////////////////////////////////////////////////

VOID CMS896AApp::GetMachineAllFeatures(VOID)
{
	LONG lFileListLength;
	CString szTemp;
	CString szFile;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	//load & get config file ptr
	pUtl->LoadAppFeatureConfig(); 
	psmf = pUtl->GetAppFeatureFile();

	//Check Load/Save Data
	if (psmf != NULL)
	{
//Get feature value
		m_szCustomer					= (*psmf)[MS896A_FUNC_CUSTOMER];
		m_szProductLine					= (*psmf)[MS896A_FUNC_PRODUCT];		//v4.33T1	//PLSG
		m_szFuncFileVersion				= (*psmf)[MS896A_FUNC_VERSION];
	
//Get General items
		m_lDelayBeforeTurnOffEJVac		= ((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_DELAY_BEFORE_TURN_OFF_EJ_VAC]);
		m_lCameraBlockDelay				= ((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_CAMERA_BLOCK_DELAY]);
		m_lRTUpdateMissingDieThreshold  = ((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_RT_UPDTAE_MISSING_DIE_THRESHOLD]);
		m_bEnablePreBond				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_PREBOND]);
		m_bEnablePostBond				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_POSTBOND]);
		m_bEnableRefDie					= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_REFDIE]);
		m_bEnableCharRefDie				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_CHAR_REFDIE]);	
		m_bUseNewStepperEnc				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_NEW_STEPPER_ENC]);				//v2.83T2
		m_bDisableOKSelectionInAlertMsg	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_DIS_OK_SELECTION_IN_ALERT_MSG]);
		m_bDisableDefaultSearchWnd		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_DISABLE_DEFAULT_SEARCH_WND]);
	
		m_bBLResetGradeMagToEmpty		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BL_RESET_GRADEMAG_EMPTY]);		//v3.70T1
		m_bUsePLLMSpecialFcn			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_PPLM_SPECIAL_FCNS]);			//v3.70T2
		m_bNoPRRecordForPKGFile			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_NO_PR_RECORD_PKG_FILE]);
		m_bEnableMachineReport			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_MACHINE_REPORT]);
		m_bEnableToolsUsageRecord		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_TOOLS_USAGE_RECORD]);
		m_lToolsUsageRecordFormat		= (*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_TOOLS_RECORD_FORMAT];
		m_bEnableItemLog				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_ITEM_LOG]);
		m_bEnablePkgFileList			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_PKG_FILE_LIST]);
		m_bEnableAlarmLampBlink			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ALARM_LAMP_BLINK]);
		m_bEnableAmiFile				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_AMI_FILE]);
		m_bEnableBarcodeLengthCheck		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BARCODE_LENGTH_CHECK]);
		m_bEnableInputCountSetupFile	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_INPUT_COUNT_SETUP_FILE]);
		m_bForceClearBinBeforeNewLot	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_CLEAR_BIN_BEFORE_NEW_LOT]);
		m_bNewLotCreateDirectory		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_NEW_LOT_CREATE_DIR]);
		m_bWLBarcodeSelection			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_WL_BC_SELECTION]);
		m_bOnOff2DBarcodeScanner		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ON_OFF_2D_BAR_SCANNER]);
		m_bEnableSemiAutoMode			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_SEMI_AUTO]);	//v4.48A21					//v3.35T8
		m_bEnableDEBFcn2016				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_DEB_2016]);
		
		if (m_bES100v2DualWftOption || m_bESAoiSingleWT)
		{
			m_bEnableBarcodeLengthCheck = TRUE;
			m_bWLBarcodeSelection		= TRUE;
		}

		//m_bEnableWaferSizeSelect		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_WAFER_SIZE_SELECT]);
		// enable the function to all customer
		m_bEnableWaferSizeSelect		= TRUE;
		
		if (m_bEnableMachineReport == TRUE || m_bEnableToolsUsageRecord == TRUE || m_bEnableItemLog == TRUE)
		{
			m_bEnableReportPathSettingInterface = TRUE;
		}
		
		m_bEnableThermalEjector			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_THERMAL_EJECTOR]);
		m_bEnableColorCamera			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_COLOR_CAMERA]);	
		// prescan relative code	B
		m_bEnablePrescanInterface		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_PRESCAN_INTERFACE]);
		m_bPrescanEmptyUnmark			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_PRESCAN_EMPTY_UNMARK]);

		m_bOperatorLogOnValidation		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_OPERATOR_VALIDATION]);
		m_bExArmDisableEmptyPreload		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD]);	//v3.34		//Cree
		m_bUseNewBHMount				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_NEW_BH_MOUNT]);	//v3.34
		m_bUseOptBinCountDynAssignGrade = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_OPT_BIN_COUNT_DYN_ASSIGN_GRADE]);
		m_bOpenMatchPkgNameCheck		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_OPEN_MATCH_PKG_NAME_CHECK]);
		m_bBinFrameStatusSummary		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_BINFRAMESTATUSSUM]);
		//m_bSupportPortablePKGFile		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_PORTABLE_PKG_FILE]);
		//m_bDownloadPortablePackageFileImage = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_DOWNLOAD_PORTABLE_PACKAGE_FILE_IMAGE]);
		//m_bDownloadPortablePackageFileImage = TRUE;
		//remove pr records after loaded the package file
		//m_bErasePortablePackageFileImage = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ERASE_PORTABLE_PACKAGE_FILE_IMAGE]);
		//m_bErasePortablePackageFileImage = TRUE;
		m_bEnableWaferLotLoadedMapCheck = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_WAFERLOT_LOADED_MAP_CHECK]);
		m_bEnableSummaryPage			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_SUMMARY_PAGE]);
		m_bWaferLotWithBinSummaryFormat	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_WAFERLOT_WITH_BIN_SUMMARY_FORMAT]);
		m_bUseSlowBTControlProfile		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_SLOW_BT_PROFILE]);		//v3.67T5
		m_bNoStaticControlForBTJoystick = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_NO_STATIC_CONTROL_BT_JOYSTICK]);	//for Cree MS896-DL only
		m_bEnableChineseMenu			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_CHINESE_MENU]);	
		m_bEnableMotorizedZoom			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_PRS_MOTORIZEDZOOM]);
		m_bEnableEmptyBinFrameCheck		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_EMPTY_BINFRAME_CHECK]);
		m_bEnableResetMagzCheck			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_RESET_MAG_CHECK]);
		m_bEnableWaferLotFileProtection = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_WAFER_LOT_FILE_PROTECTION]);
		m_bForceClearMapAfterWaferEnd	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_WAFER_END_CLEAR_MAP]);
		m_bEnableBondAlarmPage			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_BOND_ALARM_PAGE]);
		m_bEnableAlignWaferImageLog		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ALIGN_WAFER_IMAGE_LOG]);
		if( GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) )
		{
			m_bEnableAlignWaferImageLog = TRUE;
		}


		m_bEnableBHZVacNeutralState		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BH_VAC_NEUTRALSTATE]);			//v4.00T1
		m_bWafflePadBondDie				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_WAFFLE_PAD_BOND_DIE]);
		m_bWafflePadIdentification		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_WAFFLE_PAD_IDENTIFICATION]);
		m_bGroupSetupAlarmAssitTimeToIdle = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GROUP_ALARM_ASSIST_SETUP_TO_IDLE]);
		m_bEnableMachineTimeDetailsReport = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_DETAILS_REPORT]);
		m_lMachineTimeReportFormat		= ((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_DETAILS_REPORT_FORMAT]);
		m_szMachineTimeReportExt		= (*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_MACHINE_TIME_REPORT_EXT];
		m_buseSlowEjProfile				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_SLOW_EJ_PROFILE]);				//v4.08
		m_bHomeFPCUseHmiMap				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_HOME_FPC_USE_HMI_MAP]);
		m_bHomeFPCUseHmiMap				= FALSE; //fixed, user can not setup it
		m_bBLBCUseOldContinueStop		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BLBC_USE_OLD_CONTINUE_STOP]);
		m_bBLBCEmptyScanTwice			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BLBC_EMPTY_SCAN_TWICE]);		//v4.51A17
		m_bBLEmptyFrameBCCheck			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK]);		
		m_bEnableBinOutputFilePath2		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_OUTPUT_FILE_PATH2]);
		m_bPrescanDiePitchCheck			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_SCAN_DIE_PITCH_CHECK]);
		m_bBLBCUse29MagSlots			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BL_USE_29_MAGSLOTS]);
		m_bEnableMultiCOROffset			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_MULTI_COR_OFFSET]);
		m_bMS100OriginalBHSequence		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_MS100_ORG_BH_SQ]);
		m_bCoverSensorProtectionMessage = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_COVER_SENSOR_PROTECTION_MSG]);
		m_bReplaceEjAtWaferEnd			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_EJ_REAPLCE_AT_WAFEREND]);	//v4.39T10
		m_bReplaceEjAtWaferStart		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_EJ_REAPLCE_AT_WAFERSTART]);	//v4.59A3
		if( GetCustomerName() == CTM_SANAN || GetCustomerName() == "Electech3E(DL)" )
		{
			m_bReplaceEjAtWaferStart	= TRUE;
		}
		m_bManualCloseExpReadBc			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_MANUAL_CLOSE_EXP_BC]);		//v4.40T14	//TJ Sanan
		m_bBinFrameHaveRefCross			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BIN_FRAME_CROSS]);
		m_bEnableBondHeadTolerance		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ENABLE_BONDHEAD_TOLERANCE_LEVEL]);
		m_bEnableLogColletHoleData		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_ENABLE_LOG_COLLET_HOLE_DATA]);
		//m_lBHZ1HomeOffset				= ((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BHZ1_HOME_OFFSET]);				//v4.44A5
		//m_lBHZ2HomeOffset				= ((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BHZ2_HOME_OFFSET]);				//v4.44A5
		m_bEnableRefDieFaceValueCheck	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_REFDIE_FACE_VALUE]);		//v3.35T7
		m_bEnableNGPick					= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_NGPICK]);			//v4.51A19	//Silan MS90
		CMS896AStn::m_bPBErrorCleanCollet = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_PB_CLEANCOLLET]);	//v4.52A1	
		m_bUseColletOffsetWoEjtXY		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY]);		//v4.52A14
		m_bCheckPKGKeyParameters		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_CHECK_PKG_KEY_PARAMETERS]);
		m_bAWET_Enabled					= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_ALARM_WAIT_ENGINEER_TIME]);
		m_bProRataData					= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_GENERAL_PRORATA]);//M69
		m_bBPRErrorCleanCollet			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BPRERROR_NOCLEANCOLLET]);

//Disable Buzzer 		
		m_bDisableBuzzer				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_DISABLE_BUZZER]);

//Get Wafer Mapping Items
		m_lWaferMapSortingpathCalTime	= ((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_SORTING_PATH_CAL_TIME]);
		m_bMapAdaptiveAlgor				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_ADAPT_ALGORITHM]);
		m_bPrescanRealignSameMap		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_REALIGN]);
		m_bLoadMapSearchDeepInFolder	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_SEARCH_DEEP_IN_FOLDER]);
		m_bSearchCompleteFilename		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_COMPLETE_FILENAME]);
		m_bDisplaySortBinItemMsg		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_DISPLAY_SORT_BIN_ITEM]);
		m_bEnableSCNFile				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_USE_SCN]);
		m_bEnableBlockFunc				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_USE_BLOCK]);
		m_bEnableSepGrade				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_CHK_SEP_GRADE]);
		m_bEnableEmptyCheck				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_EMPTYCHECK]);			//Block2
		m_bWtCheckMasterPitch			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_CHECK_MASTER_PITCH]);	//v2.78T2
		m_bEnable1stDieFinder			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_SWALK_1STDIE]);		//v3.15T5
		m_bMapIgnoreGrade0				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_IGNORE_GRADE_0]);	//v3.28T1
		m_bEnableReVerifyRefDie			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_REVERIFY_REFDIE]);
		m_bNonBlkPkEndVerifyRefDieApp	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_NONBLKPICK_VERIFY_REFDIE]);
		m_bManualAlignReferDie			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_MANUAL_ALIGN_REFER_DIE]);
		m_bEnable2DBarCode				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_2D_BARCODE_CHECK]);		//v3.60
		m_bSaveMapFile					=!(BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_NOT_SAVE_WAFER_MAP]);
		m_bEnable2nd1DBarCode			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_2ND_1D_BARCODE_CHECK]);   
		m_bToPickDefect					= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_PICK_DEFECT_DIE]);	//v3.54T4
		m_lBlockPickMode				= ((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_BLOCK_MODE]);
		m_szDieTypeFieldnameInMap		= (*psmf)[MS896A_FUNC_WAFERMAP][MS896A_DIE_TYPE_FIELD_NAME_IN_MAP];
		m_bPackageFileMapHeaderCheckFunc = (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_PKG_FILE_MAP_HEADER_CHECK_FUNC]);
		m_bUpdateWaferMapHeader			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_UPDATE_WAFER_MAP_HEADER]);
		m_szPackageFileMapHeaderCheckString = (*psmf)[MS896A_FUNC_WAFERMAP][MS896A_PKG_FILE_CHECK_MAP_HEADER_STRING];

		//4.53D18 wafer mapping
		m_bWaferMapDieMixing			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_MAP_DIE_MIX]);

		m_bClearDieTypeFieldDuringClearAllBin = (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_CLEAR_DIE_TYPE_FIELD_AFTER_CLEAR_ALL_BIN]);
		m_bEnableAutoDieTypeFieldnameChek = (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_ENABLE_AUTO_TYPE_FIELD_NAME_CHECK]);	//v3.54T4
		m_bEnablePsmFileExistCheck		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_PSM_EXIST_CHECK]);
		m_bEnableBatchIdFileCheck		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_BATCH_ID_FILE_CHECK]);
		m_bEnableBinMap					= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_ENABLE_BINMAP]);		//PLLM REBEL	//v4.03
		m_bEnableManualInputBC			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_MANUAL_INPUT_BC]);	//v4.06
		m_bEnableAutoMapDieTypeCheckFunc = (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_DIE_TYPE_CHECK][MS896A_FUNC_ENABLE_DIE_TYPE_CHECK_FUNC]);
		m_bDisableWaferMapFivePointCheckMsgSelection = (BOOL)(LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_DISABLE_FIVE_POINT_CHECK_MSG_SEL];
		m_bMultiGradeSortToSingleBin	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_MULTIGRADE_SORT_TO_SINGLEBIN]);	//v4.15T8
		m_bEnableCMLTReworkFcn			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_CMLT_REWORK]);	
		m_bNoIgnoreGradeOnInterface		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_NO_IGNORE_GRADE_ON_INTERFACE]);	
		m_bCheckIgnoreGradeCount		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_CHECK_IGNORE_GRADE_COUNT]);
		m_ulIgnoreGrade					= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_IGNORE_GRADE]);
		m_bEnableMapColumnsCheck		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_CHECK_MAP_COLUMNS]);
		
		// for tong fang map file 
		m_lSetDetectSkipMode			= (LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_DETECT_SKIP_MODE];
		m_bEnableAssocFile				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_ASSOC_FILE_FUNC]);	
		m_szAssocFileExt				= (*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_ASSOC_FILE_EXT];
		m_bEnableOsramResortMode		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_OSRAM_RESORT_MODE]);	
		m_bRenameMapNameAfterEnd		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_WAFERMAP_RENAME_AFTER_END]);
		m_bAlwaysLoadMapFileLocalHarddisk = (BOOL)((LONG)(*psmf)[MS896A_FUNC_WAFERMAP][MS896A_FUNC_ALWAYS_LOAD_MAP_LOCAL_HD]);

		for (INT i = 0 ; i < MS896A_DIE_TYPE_CHECK_LIMIT; i++)
		{
			szTemp = MS896A_FUNC_DIE_TYPE_CHECK_FIELDNAME;
			szTemp.AppendFormat("%d", i + 1);
			m_szMapDieTypeCheckString[i] = (*psmf)[MS896A_FUNC_WAFERMAP][MS896A_DIE_TYPE_CHECK][szTemp];

			if (m_szMapDieTypeCheckString[i] == "")
			{
				m_szMapDieTypeCheckString[i] = "N/A";
			}
		}


//Get Sensor function items
		m_bCleanEjectorCapStop		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_CLEAN_EJECTOR_CAP_STOP_CYCLE]);
		m_bEnableNewBLMagExistCheck = (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_SENSOR_NEW_BL_MAGEXIST_CHECKING]);	//v3.57
		m_bCheckBinVacMeter			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_SENSOR_BT_VAC_METER_CHECKING]);		//v3.58
		m_bBLRealTimeCoverSensorCheck = (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_SENSOR_BL_RT_COVERSENSOR_CHECK]);	//v3.60T1
		m_bBLRealTimeMgznExchange	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_SENSOR_BL_RT_MGZN_EXCHANGE]);
		m_bEnableCoverLock			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_SENSOR_COVER_LOCK]);					//v3.60
		m_bEnableColletCleanPocket	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_COLLET_CLEAN_POCKET]);	
		m_bEnableWaferClamp			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_SO_WAFER_CLAMP]);		//MS810EL-90 Walsin CHina	//v4.26T1
		m_bEnableStartupCoverSensorCheck = (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_START_UP_COVER_SNR_CHECK]);
		m_bEnableBinLoderCoverSensorCheck = (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_BINLOADER_COVER_SNR_CHECK]);
		//m_bEnableBHSuckingHead		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_BH_SUCKING_HEAD]);		//v4.05	
#ifdef NU_MOTION
		m_lMotionPlatform			= SYSTEM_NUMOTION;		//v4.05
		m_bEnableColletCleanPocket	= TRUE;					//v3.86T5
		if (m_bES100v2DualWftOption == TRUE)
		{
			m_bEnableColletCleanPocket	= FALSE;
		}
#endif


//Get Portable PKG file items
		//m_bSupportPortablePKGFile		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_PORTABLE_PACKAGE_FILE][MS896A_FUNC_ENABLE_PORTABLE_PKG_FILE]);
		//m_bSupportPortablePKGFile		= TRUE;
		m_bPortablePKGInfoPage = (BOOL)((LONG)(*psmf)[MS896A_FUNC_PORTABLE_PACKAGE_FILE][MS896A_FUNC_PORTABLE_PKG_INFO_PAGE]);
		m_bDownloadPortablePackageFileImage = (BOOL)((LONG)(*psmf)[MS896A_FUNC_PORTABLE_PACKAGE_FILE][MS896A_FUNC_DOWNLOAD_PORTABLE_PACKAGE_FILE_IMAGE]);
		//remove pr records after loaded the package file
		m_bErasePortablePackageFileImage = (BOOL)((LONG)(*psmf)[MS896A_FUNC_PORTABLE_PACKAGE_FILE][MS896A_FUNC_ERASE_PORTABLE_PACKAGE_FILE_IMAGE]);

		m_bIsPortablePackageFileIgnoreList = (BOOL)((LONG)(*psmf)[MS896A_FUNC_PORTABLE_PACKAGE_FILE][MS896A_FUNC_PORTABLE_PACKAGE_FILE_IGNORE_LIST]);

		lFileListLength = (LONG)(*psmf)[MS896A_FUNC_PORTABLE_PACKAGE_FILE][MS896A_FUNC_PORTABLE_PACKAGE_FILE_FILE_LIST][MS896A_FUNC_PORTABLE_PACAKGE_FILE_FILE_LIST_LENGTH];

		m_szaPortablePackageFileFileList.RemoveAll();	//Nichia//v4.43T7
		for (INT i = 0; i < lFileListLength; i++)
		{
			szTemp = MS896A_FUNC_PORTABLE_PACAKGE_FILE_FILE_NO;
			szTemp.AppendFormat("%d", i + 1);
			szFile = (*psmf)[MS896A_FUNC_PORTABLE_PACKAGE_FILE][MS896A_FUNC_PORTABLE_PACKAGE_FILE_FILE_LIST][szTemp];

			m_szaPortablePackageFileFileList.Add(szFile);
		}

//Get HMI Screen items
		m_bEnableBondTaskBar	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_BOND_TASK_SHORTCUT_BAR]);
		m_bEnableScanTaskBar	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_SCAN_TASK_SHORTCUT_BAR]);
		//Default is TRUE in 2018.7.25 for all customer
		m_bEnableBondTaskBar = TRUE;
		m_bEnableScanTaskBar = TRUE;

		m_bEnableOPMenuShortcut	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_OPMENU_SHORTCUT]);	//v4.19T1	
		m_bEnableBHShortcut		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMIBHSHORTCUT]);	
		m_bEnableWPRShortcut	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMIWPRSHORTCUT]);	
		m_bEnableBPRShortcut	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMIBPRSHORTCUT]);	
		m_bEnableBinBlkShortcut	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMIBINBLKSHORTCUT]);	
		m_bEnableWTShortcut		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_WTSHORTCUT]);	
		m_bEnablePRRecordShortcut	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_PRRECORD_SHORTCUT]);	
		m_bDisablePKGShortcut	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_DISABLE_PKG_SHORTCUT]);	
		m_bEnableBinTableShortcut	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_BINTABLE_SHORTCUT]);
		m_bEnablePicknPlaceShortcut	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMIPICKNPLACESHORTCUT]);
		m_bEnableOptionsTabShortcut = (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_OPTIONSTAB_SHORTCUT]);
		m_bDisableOptionsShortcut = (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_DISABLE_OPTION_SHORTCUT]);
		m_bEnableSystemServiceShortcut = (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_SYSTEMSERVICE_SHORTCUT]);
		m_bEnableStepMoveShortcut = (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_STEP_MOVE_SHORTCUT]);
		m_bEnablePhyBlockShortcut = (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_OPERATION_MODE_SHORTCUT]);
		m_bEnableOperationModeShortcut = (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_PHY_BLOCK_SHORTCUT]);
		m_bAllowMinimizeHmi	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_ALLOW_MINIMIZE_HMI]);
		m_bToggleWaferResortScanMode	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_TOGGLE_WAFER_RESORT_MODE]);
		m_bEnableWaferToBinResort		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HMISCREEN][MS896A_FUNC_HMI_WAFER_BIN_RESORT_MODE]);

//Get Host Communication items.
		m_bEnableSECSComm		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HOST_COMM][MS896A_FUNC_HOST_COMM_SECSGEM]);	
		m_bEnableTCPComm		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HOST_COMM][MS896A_FUNC_HOST_COMM_TCPIP]);
		// Display
		m_bOthersConnectionType = (BOOL)((LONG)(*psmf)[MS896A_FUNC_HOST_COMM][MS896A_FUNC_OTHERS_CONNECTION_TYPE]);
		m_EnableEMMode			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_HOST_COMM][MS896A_FUNC_HOST_COMM_EQUIP_MANAGER]);	//v4.59A32


//Get Output files items
		m_bEnableBinWorkNo				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_BIN_WORKNO]);	
		m_bEnableFileNamePrefix			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLRBINCNT][MS896A_FUNC_FILE_NAME_PREFIX]);
		m_bEnableAppendInfoInWaferId	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_APPEND_IN_WAFERID]);	
		m_bAddWaferIdInMsgSummaryFile	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_WAFERID_IN_MSGSUMMARY]);
		m_bEnableGradeMappingFile		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_GRADE_MAPPING_FILE]);
		m_bEnablePickNPlaceOutputFile	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLRBINCNT][MS896A_FUNC_PICK_N_PLACE_OUTPUT]);
		m_bGenRefDieStatusFile			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_WAFEREND][MS896A_FUNC_REFDIESTATUSFILE]);
		m_bWaferEndFileGenProtection	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_WAFEREND][MS896A_FUNC_WAFERENDFILEPROTECTION]);
		m_bAutoGenWaferEndFile			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_WAFEREND][MS896A_FUNC_AUTO_GEN_WAFEREND]);
		m_bAutoGenWaferEndAfterPrescan	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_WAFEREND][MS896A_FUNC_AUTO_GEN_WAFEREND_PRESCAN]);
		m_bGenLabel						= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLRBINCNT][MS896A_FUNC_LABELPRINTOUT]);
		m_bGenLabelWithXMLFile			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLRBINCNT][MS896A_FUNC_LABELPRINTOUT_XML]);
		m_bGenBinLotDirectory			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_BINNO_DIRECTORY]);
		m_bUseClearBinByGrade			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLR_BIN_BY_GRADE]);
		m_bEnableExtraClearBinInfo		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_EXTRA_BIN_INFO][MS896A_FUNC_ENABLE_EXTRA_CLR_BIN_INTO]);
		m_bAddDieTimeStamp				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_DIE_TIMESTAMP]);
		m_bCheckProcessTime				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_DIE_PROCESSTIME]);
		m_bAddSortingSequence			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_ADD_SORTING_SEQUENCE]);
		m_bEnableMultipleMapHeaderPage	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_MULTIPLE_HEADER_PAGE]);
		m_bChangeGradeBackupTempFile	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CHANGE_GRADE_BACKUPTEMP_FILE]);
		m_bKeepOutputFileLog			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_KEEP_OUTPUT_FILE_LOG]);
		m_bRemoveBackupOutputFile		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_REMOVE_BACKUP_OUTPUTFILE]);
		m_bAutoGenBinBlkCountSummary	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_AUTO_GEN_BIN_BLK_SUMMARY]);
		m_bDisableBinSNRFormat			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLRBINCNT][MS896A_FUNC_BIN_SNR][MS896A_FUNC_BIN_SNR_SEL_FORMAT]);
		m_bDisableClearCountFormat		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLRBINCNT][MS896A_FUNC_BIN_SNR][MS896A_FUNC_SEL_CLEAR_COUNT_FORMAT]);
		m_bEnableDynMapHeaderFile		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLRBINCNT][MS896A_FUNC_DYNMAPHEADER]);
		m_bBackupTempFile				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_BACKUP_TEMP_FILE]);
		m_bBackupOutputTempFile			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_BACKUP_OUTPUT_TEMP_FILE]);
		m_bForceDisableHaveOtherFile	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_FORCE_DISABLE_HAVE_OTHER_FILE]);
		m_bEnableManualUploadBinSummary = (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_MANUAL_UPLOAD_BIN_SUMMARY_FILE]);
		m_bGenerateAccumulateDieCountReport = (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_GEN_ACCUMULATE_DIE_COUNT_REPORT]);
		m_bEnableSaveTempFileWithPKG	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_PKG_SAVE_TEMP_FILES]);
		m_bEnableClearBinCopyTempFile	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_ENABLE_COPY_TEMP_FILE_IN_CLEAR_BIN]);
		m_bAutoUploadOutputFile			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_OUTFILES][MS896A_AUTO_TRANSFER_OUTPUT_FILE]);
		m_ulDefaultBinSNRFormat			= (*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLRBINCNT][MS896A_FUNC_BIN_SNR][MS896A_FUNC_BIN_SNR_DEFAULT_FORMAT];
		m_ulDefaultClearCountFormat		= (*psmf)[MS896A_FUNC_OUTFILES][MS896A_FUNC_CLRBINCNT][MS896A_FUNC_BIN_SNR][MS896A_FUNC_CLEAR_COUNT_DEFAULT_FORMAT];

//Get New Lot Options items
		m_bOutputFormatSelInNewLot		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_NEW_LOT_OPTIONS][MS896A_FUNC_OF_FORMAT_SELECT]);


//Get Vision Items
		m_bBTRealignWithFFMode			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_BT_REALIGN_FF_MODE]);
		m_bEnablePolygonDie				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_GENERAL_POLYGON_DIE]);
		m_bEnablePRDualColor			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_DUAL_COLOR]);		
		m_bPrCircleDetection			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_CIRCLE_DETECTION]);		//v2.78T2
		m_bPrAutoLearnRefDie			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_AUTOLEARN_REFDIE]);		//v2.78T2
		m_bPrLineInspection				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_LINE_INSPECTION]);
		m_bPrEnableWpr2Lighting			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_WPR_2_LIGHTING]);
		m_bEnablePrAutoEnlargeSrchWnd	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_AUTO_ENLARGE_SRCHWND]);	//Block2
		m_bEnablePrPostSealOptics		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_POSTSEAL_OPTICS]);
		if (CMS896AStn::m_bAutoChangeCollet)	//if MS60 AGC fcn is enabled)	//v4.52A3
		{
			m_bEnablePrPostSealOptics = TRUE;
		}
		m_bEnableBHPostSealOptics		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS]);
		m_bEnableBHUplookPrFcn			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_BH_UPLOOK_PR]);			//v4.52A16
		m_bEnableESContourCamera		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][ES_FUNC_VISION_WL_CONTOUR_CAMERA]);
		m_bEnableDualPathCamera			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS60_FUNC_VISION_DUAL_PATH]);
#ifndef	ES101
		m_bEnableESContourCamera = FALSE;
#endif
		if( m_bEnableESContourCamera )
		{
			m_bEnablePrPostSealOptics = TRUE;
		}
		m_bEnablePRAdaptWafer			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_ADAPT_WAFER]);
		m_bCounterCheckWithNormalDieRecord	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_LRN_DIE_COUNTER_CHECK_NORMAL_RECORD]);
		m_bOfflinePostbondTest			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_VISION_OFFLINE_PBTEST]);
		m_bUseManualPRJsSpeed			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_MANUAL_PR_JS_SPEED]);			//v4.16T6	//Huga
		m_bWaferPRMxNLFWnd				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_MxN_LF_WND]);					//v4.43T2	//SanAn
		m_bWaferPrMS60LFSequence		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_MS60_LF_SEQ]);					//v4.47T3
		m_bWaferPrMS60LFSequence		= TRUE; //hardcode
		m_bEnableWPRBackLight			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_WPR_BACK_LIGHT]);				//v4.46T28	//Cree HuiZhou
		m_bMS60SortingFFMode			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS60_VISION_5M_SORTING_FF_MODE]);
		m_bEnableAoiOcrDie				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_VISION][MS896A_FUNC_GENERAL_AOI_OCR_DIE]);
#ifndef VS_5MCAM
		m_bMS60SortingFFMode			= FALSE;
#endif
		//v3.17T2		
		m_lVisionPlatform		= PR_SYSTEM_BW;			//v4.05
		m_bPrIMInterface		= FALSE;


//Get Bin Block items
		m_bEnableBinBlkTemplate			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_TEMPLATE]);
		m_szRankIDFileNameInMap			= (*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_RANKID_NAME_IN_MAP];
		m_szRankIDFileExt				= (*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_RANKID_FILE_EXT];
		m_bEnableAutoLoadNamingFile		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_AUTOLOAD_NAMING_FILE]);
		m_bEnableLoadRankIDFromMap		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_LOAD_RANKID_FROM_MAP]);
		m_bEnableOptimizeBinCountFunc	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT]);
		m_bOptimizeBinCountPerWftFunc	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT_PER_WAFER]);
		m_bEnableOpBinCntInLoadPath		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_OPBC_LOADPATH]);			//v4.44T5
		m_bEnableRestoreBinRunTimeData	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_ENABLE_RESTORE_BINRUMTIME]);
		m_bBinFrameNewRealignMethod		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_NEW_REALIGN]);
		m_bBinFrameUseSoftRealign		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_SOFT_REALIGN]);			//v4.59A35
		m_bBinMultiSearchFirstDie		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_MULTI_SEARCH_FIRST_DIE]); //4.51D20
		m_bEnableBTPt5DiePitch			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_USE_PT5_DIEPITCH]);
		m_bEnableBTBondAreaOffset		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_BONDAREA_OFFSET]);
		m_bBTGelPadSupport				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_GELPAD]);
		m_bEnableBTNewLotEmptyRowFcn	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_BINBLK][MS896A_FUNC_BINBLK_NEWLOT_EMPTYROW]);

	}

	//close config file ptr
	pUtl->CloseAppFeatureConfig();

	//For display HMI Comm control only
	m_bEnableCommOption		= (m_bEnableTCPComm || m_bEnableSECSComm || m_bOthersConnectionType);

	if (m_bES100v2DualWftOption)
	{
		m_bEnableBondTaskBar = TRUE;
		m_bEnableScanTaskBar = TRUE;
	}

	//Empty Sub folder if WorkNo is disabled
	if (m_bEnableBinWorkNo == FALSE)
	{
		m_szMapSubFolderName.Empty();
		m_smfSRam["MS896A"]["MapSubFolderName"] = m_szMapSubFolderName;
	}

	//if (m_bMS60)	//v4.47T1
	//{
	//	m_bWaferPRMxNLFWnd = TRUE;
	//}

	m_bIsPLLMRebel		= FALSE;

	if (GetCustomerName() == CTM_NICHIA)			//v4.40T1
	{
		m_bIsNichia		= TRUE;
	}
	else if ((GetCustomerName() == "Electech3E") || (GetCustomerName() == "Electech3E(DL)"))	//v4.44A6
	{
		m_bIsElectech3E = TRUE;
	}
	else if (GetCustomerName() == "Yealy")		//v4.46T6
	{		
		m_bIsYealy		= TRUE;
	}
	else if (GetCustomerName() == "Avago")		//v4.48A21
	{
		m_bIsAvago		= TRUE;
	}

	if( (GetCustomerName()==CTM_SANAN		&& GetProductLine()=="XA") ||
		(GetCustomerName()==CTM_SEMITEK	&& GetProductLine()=="ZJG") )
	{
		m_bSaveMapFile = FALSE;
	}

	if (CMS896AStn::m_bCEMark == TRUE)		//v4.09
	{
		m_bBLRealTimeCoverSensorCheck = TRUE;
	}

	//CT speed-up option
	m_smfSRam["MS896A"]["New CT"] = TRUE;	//m_bNewCtEnhancement;	//v4.50A2


	//v3.55	//Machine option logging
	CString szLog;
	szLog.Format("    FEATURE Option: NGPick=%d", m_bEnableNGPick);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	szLog.Format("    IM Option: %d", m_bPrIMInterface);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	szLog.Format("    Wafer PR Option: MxN=%d, MS60LF=%d", m_bWaferPRMxNLFWnd, m_bWaferPrMS60LFSequence);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (m_bUsePLLM)
	{
		szLog.Format("    PLLM Config: DieFab=%d, Lumiramic=%d, Rebel=%d", m_bIsPLLMDieFab, m_bIsPLLMLumiramic, m_bIsPLLMRebel);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}

	if (m_bMSAutoLineMode == 1)
	{
		m_bEnableBinOutputFilePath2 = TRUE;
	}

	struct __stat64 fs;
	struct tm *t;
	char software_date[33];
	char software_time[33];

	if (_stat64("MapSorter.exe", &fs) == 0)
	{
		t = _localtime64(&fs.st_mtime);
		sprintf(software_date, "%4d-%02d-%02d", (t->tm_year + 1900), (t->tm_mon + 1), t->tm_mday);
		sprintf(software_time, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
	}
	else
	{
		software_date[0] = '\0';
		software_time[0] = '\0';
	}
	m_szSoftwareReleaseDate = software_date;
	m_szSoftwareReleaseTime = software_time;

}

VOID CMS896AApp::GetMSOptions(VOID)
{
	CString szTemp;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	BOOL bMouseJoystick = FALSE;

	//load & get config file ptr
	pUtl->LoadMSOptionsConfig(); 
	psmf = pUtl->GetMSOptionsFile();

	//Check Load/Save Data
	if (psmf != NULL)
	{
		bMouseJoystick = (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_PR_MOUSE_JOYSTICK]);
		if (bMouseJoystick)
		{
			CMS896AStn::m_ulJoyStickMode = MS899_JS_MODE_PR;
		}

		CMS896AStn::m_bEMOChecking			= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_EMO]);					//v3.91

		CMS896AStn::m_bUseBinMultiMgznSnrs	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_BIN_6_MGZN_SNRS]);
		CMS896AStn::m_bCEMark				= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_CEMARK]);				//v4.06
		CMS896AStn::m_bDBHThermalCheck		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_DBH_THERMAL_CHECK]);		//v4.26T1
		CMS896AStn::m_bDBHHeatingCoilFcn	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_DBH_THERMAL_CHECK][MS896A_FUNC_DBH_HEATINGCOIL]);	//v4.49A5
		CMS896AStn::m_bDBHThermostat		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_DBH_THERMAL_CHECK][MS896A_FUNC_DBH_THERMOSTAT]);		//v4.49A5
		CMS896AStn::m_bAutoChangeCollet		= (BOOL)((LONG)(*psmf)[MS896A_FUNC_SENSOR][MS896A_FUNC_AUTO_CHG_COLLET]);		//v4.50A6


		//v3.82		//MS100 new 150/175 8mag config
		// 0 -> default 6mag config
		// 1 -> new MS100 8mag config
		// 2 -> new all-die-sort 9mag config
		LONG lBLOutMagConfig = (LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_BL_OUTMAG_MODE];
		if (lBLOutMagConfig > 0)
		{
			CMS896AStn::m_bBLOut8MagConfig = TRUE;
		}
		else
		{
			CMS896AStn::m_bBLOut8MagConfig = FALSE;
		}

		if (m_bMS50)
		{
			CMS896AStn::m_bUseBinMultiMgznSnrs = TRUE;
			CMS896AStn::m_bBLOut8MagConfig = TRUE;
		}

		m_bUseWideScreen	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_WIDE_SCREEN_MONITOR]);			//v4.29
		m_bUseTouchScreen	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_TOUCH_SCREEN_MONITOR]);			//v4.50A3

		//v4.56 Installer added BH Uplook PR option
		BOOL bUseBHUplookPr	= (BOOL)((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_FUNC_MSOPTIONS_BH_UPLOOK_CAMERA]);	//v4.56
		if (bUseBHUplookPr)
		{
			m_bEnableBHPostSealOptics	= TRUE;
			m_bEnableBHUplookPrFcn		= TRUE;
		}

		if (CMS896AStn::m_bAutoChangeCollet)	//v4.58A6
		{
			m_bEnablePrPostSealOptics	= TRUE;
		}
	}

	//close config file ptr
	pUtl->CloseAppFeatureConfig();

	//v4.46T26
	CString szLog;
	szLog.Format("    Retrieve MS-Option: PR-JS=%d, Mult-Sen=%d, ThChk=%d (HtCoil=%d, Thermostat=%d), 8Mag=%d, CE=%d, AGC=%d", 
				bMouseJoystick, CMS896AStn::m_bUseBinMultiMgznSnrs, 
				CMS896AStn::m_bDBHThermalCheck, CMS896AStn::m_bDBHHeatingCoilFcn, CMS896AStn::m_bDBHThermostat,
				CMS896AStn::m_bBLOut8MagConfig, CMS896AStn::m_bCEMark, CMS896AStn::m_bAutoChangeCollet);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
}


VOID CMS896AApp::GetRegistryEntries()
{
	INT nTemp = 0;

	//Group ID
	m_szGroupID	= GetProfileString(gszPROFILE_SETTING, gszPROFILE_GROUP_ID, gszDEFAULT_GROUP_ID);

	ULONG ulTimeOut	= GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT,		0);
	ULONG ulRetry	= GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT,	0);
	if (ulTimeOut == 0)
	{
		ulTimeOut = 100;
	}
	if (ulRetry == 0)
	{
		ulRetry = 10;
	}
	WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT,		ulTimeOut);
	WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT,	ulRetry);
	int nDays = GetProfileInt(gszPROFILE_SETTING, _T("PrescanResultKeepDays"), 60);
	if (nDays == 0)
	{
		nDays = 60;
	}
	if( IsPLLMRebel() )
	{
		nDays = 30;
	}
	WriteProfileInt(gszPROFILE_SETTING, _T("PrescanResultKeepDays"), nDays);
	//	grab and save images kept days.
	UINT unImageDays = GetProfileInt(gszPROFILE_SETTING, _T("Grab Save Image Keep Days"), 0);
	if (unImageDays == 0)
	{
		unImageDays = nDays;
	}
	WriteProfileInt(gszPROFILE_SETTING, _T("Grab Save Image Keep Days"), unImageDays);

	//Retrieve Tot Log File Backup
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszLOG_BACKUP_SETTING, 5);//Default values, 5 Backup files
	SetTotLogBackupFiles(nTemp); 

	//Total Number of line for Log File 
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszLOG_FILE_LINE, 50000);//Default values, 50000 lines
	SetTotLogFileLine(nTemp); 

	// Retrieve Message Log File Path
	//m_szLogFilePath = GetProfileString(gszPROFILE_SETTING, gszLOG_PATH_SETTING, _T(""));
	m_szLogFilePath = gszLOG_FILE_PATH;
	WriteProfileString(gszPROFILE_SETTING, gszLOG_PATH_SETTING, gszLOG_FILE_PATH);

	if ((m_szLogFile = GetProfileString(gszPROFILE_SETTING, gszLOG_FILE_SETTING, _T(""))) == _T(""))
	{
		m_szLogFile = gszLOG_FILE_NAME;
		WriteProfileString(gszPROFILE_SETTING, gszLOG_FILE_SETTING, gszLOG_FILE_NAME);
	}

	m_ucDeviceFileType = (INT)(GetProfileInt(gszPROFILE_SETTING, gszDEVICE_FILE_TYPE, MS_PACKAGE_FILE_TYPE));//Default values, 5 Backup files
	
	// Retrieve Device path
	if ((m_szDevicePath = GetProfileString(gszPROFILE_SETTING, gszDEVICE_PATH_SETTING, _T(""))) == _T(""))
	{
		m_szDevicePath = gszDEVICE_FILE_PATH;
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_PATH_SETTING, gszDEVICE_FILE_PATH);
	}

	if ((m_szPortablePKGPath = GetProfileString(gszPROFILE_SETTING, gszPORTABLE_DEVICE_PATH_SETTING, _T(""))) == _T(""))
	{
		m_szPortablePKGPath = gszDEVICE_FILE_PATH;
		WriteProfileString(gszPROFILE_SETTING, gszPORTABLE_DEVICE_PATH_SETTING, gszDEVICE_FILE_PATH);
	}

	//Retrieve Map File Path
	if ((m_szMapFilePath = GetProfileString(gszPROFILE_SETTING, gszMAPFILE_PATH_SETTING, _T(""))) == _T(""))
	{
		m_szMapFilePath = gszMAP_FILE_PATH;
		WriteProfileString(gszPROFILE_SETTING, gszMAPFILE_PATH_SETTING, gszMAP_FILE_PATH);
	}

	//Retrieve Map File Ext
	if ((m_szMapFileExt = GetProfileString(gszPROFILE_SETTING, gszMAPFILE_EXT_SETTING, _T(""))) == _T(""))
	{
		m_szMapFileExt = gszMAP_FILE_EXT;
		WriteProfileString(gszPROFILE_SETTING, gszMAPFILE_EXT_SETTING, gszMAP_FILE_EXT);
	}
	
	// Retrieve Output File Path
	if ((m_szOutputFilePath = GetProfileString(gszPROFILE_SETTING, gszOUTPUT_PATH_SETTING, _T(""))) == _T(""))
	{
		m_szOutputFilePath	= gszOUTPUT_FILE_PATH;
		WriteProfileString(gszPROFILE_SETTING, gszOUTPUT_PATH_SETTING, gszOUTPUT_FILE_PATH);
	}

	//Retrieve & Check Output File Format
	if ((m_szOutputFileFormat = GetProfileString(gszPROFILE_SETTING, gszOUTPUT_FORMAT_SETTING, _T(""))) == _T(""))
	{
		m_szOutputFileFormat = gszOUTPUT_FILE_FORMAT;
		WriteProfileString(gszPROFILE_SETTING, gszOUTPUT_FORMAT_SETTING, gszOUTPUT_FILE_FORMAT);
	}
	CheckOutputFileFormatName(m_szOutputFileFormat);

	// Retrieve Waferend File Path
	if ((m_szWaferEndPath = GetProfileString(gszPROFILE_SETTING, gszWAFEREND_PATH_SETTING, _T(""))) == _T(""))
	{
		m_szWaferEndPath	= gszWAFEREND_FILE_PATH;
		WriteProfileString(gszPROFILE_SETTING, gszWAFEREND_PATH_SETTING, gszWAFEREND_FILE_PATH);
	}

	//Retrieve & Check Waferend File Format
	if ((m_szWaferEndFormat = GetProfileString(gszPROFILE_SETTING, gszWAFEREND_FORMAT_SETTING, _T(""))) == _T(""))
	{
		m_szWaferEndFormat = gszWAFEREND_FILE_FORMAT;
		WriteProfileString(gszPROFILE_SETTING, gszWAFEREND_FORMAT_SETTING, gszWAFEREND_FILE_FORMAT);
	}
	CheckWaferEndFileFormatName(m_szWaferEndFormat);

	// Retrieve Recipe File Path (PLLM Flash & Rebel)	//v4.42T3
	if ((m_szRecipeFilePath = GetProfileString(gszPROFILE_SETTING, gszRECIPE_TABLE_PATH_SETTING, _T(""))) == _T(""))
	{
		m_szRecipeFilePath	= gszROOT_DIRECTORY + "\\Exe";
		WriteProfileString(gszPROFILE_SETTING, gszRECIPE_TABLE_PATH_SETTING, m_szRecipeFilePath);
	}

	//Retrieve Machine Name & No
	if ((m_szMachineNo = GetProfileString(gszPROFILE_SETTING, gszMACHINE_NO_SETTING, _T(""))) == _T(""))
	{
		m_szMachineNo = gszMACHINE_NO;
		WriteProfileString(gszPROFILE_SETTING, gszMACHINE_NO_SETTING, gszMACHINE_NO);
	}
	//v4.40T1	//Nichia
	if ((m_szMachineName = GetProfileString(gszPROFILE_SETTING, gszMACHINE_NAME_SETTING, _T(""))) == _T(""))
	{
		m_szMachineName = _T("");
		WriteProfileString(gszPROFILE_SETTING, gszMACHINE_NAME_SETTING, m_szMachineName);
	}

	//Retrieve Lot No
	if ((m_szLotNumber = GetProfileString(gszPROFILE_SETTING, gszLOT_NO_SETTING, _T(""))) == _T(""))
	{
		m_szLotNumber = gszLOT_NO;
		WriteProfileString(gszPROFILE_SETTING, gszLOT_NO_SETTING, gszLOT_NO);
	}
	if ((m_szLotNumberPrefix = GetProfileString(gszPROFILE_SETTING, gszLOT_NO_PREFIX_SETTING, _T("M"))) == _T(""))
	{
		m_szLotNumberPrefix = _T("M");
		WriteProfileString(gszPROFILE_SETTING, gszLOT_NO_PREFIX_SETTING, _T("M"));
	}

	//Retrieve Lot Directory
	if ((m_szBinLotDirectory = GetProfileString(gszPROFILE_SETTING, gszBIN_LOT_DIRECTORY_SETTING, _T(""))) == _T(""))
	{
		m_szBinLotDirectory = gszBIN_LOT_DIRECTORY;
		WriteProfileString(gszPROFILE_SETTING, gszBIN_LOT_DIRECTORY_SETTING, gszBIN_LOT_DIRECTORY);
	}

	//Retrieve Lot Directory
	if ((m_szAssociateFile = GetProfileString(gszPROFILE_SETTING, gszASSO_FILE_SETTING, _T(""))) == _T(""))
	{
		m_szAssociateFile = gszASSO_FILE;
		WriteProfileString(gszPROFILE_SETTING, gszASSO_FILE_SETTING, gszASSO_FILE);
	}

	//Retrieve Lot Start Time
	if ((m_szLotStartTime = GetProfileString(gszPROFILE_SETTING, gszLOT_START_TIME, _T(""))) == _T(""))
	{
		m_szLotStartTime = gszLOT_NO;
		WriteProfileString(gszPROFILE_SETTING, gszLOT_START_TIME, gszLOT_NO);
	}

	//Retrieve Lot End Time
	if ((m_szLotStartTime = GetProfileString(gszPROFILE_SETTING, gszLOT_END_TIME, _T(""))) == _T(""))
	{
		m_szLotEndTime = gszLOT_NO;
		WriteProfileString(gszPROFILE_SETTING, gszLOT_END_TIME, gszLOT_NO);
	}

	//Retrieve Bin WorkNo
	if ((m_szBinWorkNo = GetProfileString(gszPROFILE_SETTING, gszBIN_WORK_NO, _T(""))) == _T(""))
	{
		m_szBinWorkNo = gszBIN_SPEC_VERSION;
		WriteProfileString(gszPROFILE_SETTING, gszBIN_WORK_NO, gszBIN_SPEC_VERSION);
	}

	//Retrieve Bin FileName Prefix
	if ((m_szBinFileNamePrefix = GetProfileString(gszPROFILE_SETTING, gszBIN_FILENAME_PREFIX, _T(""))) == _T(""))
	{
		m_szBinFileNamePrefix = gszBIN_SPEC_VERSION;
		WriteProfileString(gszPROFILE_SETTING, gszBIN_FILENAME_PREFIX, gszBIN_SPEC_VERSION);
	}

	//Retrieve Map sub-folder name
	if ((m_szMapSubFolderName = GetProfileString(gszPROFILE_SETTING, gszMAP_SUBFOLDER_NAME, _T(""))) == _T(""))
	{
		m_szMapSubFolderName = gszBIN_SPEC_VERSION;
		WriteProfileString(gszPROFILE_SETTING, gszMAP_SUBFOLDER_NAME, gszBIN_SPEC_VERSION);
	}

	//v3.24T1
	//Retrieve Map sub-folder name
	if ((m_szUsername = GetProfileString(gszPROFILE_SETTING, gszMAP_USER_NAME, _T(""))) == _T(""))
	{
		//m_szUsername = "";
		//WriteProfileString(gszPROFILE_SETTING, gszMAP_USER_NAME, "");
	}
	if ((m_szProductID = GetProfileString(gszPROFILE_SETTING, gszMAP_PRODUCT_ID, _T(""))) == _T(""))
	{
		//m_szProductID = "";
		//WriteProfileString(gszPROFILE_SETTING, gszMAP_PRODUCT_ID, "");
	}
	//m_szProductID = m_szUsername;		//v4.35T4	//Lumiramic MS109


	//Retrieve Load Map Date
	if ((m_szLoadMapDate = GetProfileString(gszPROFILE_SETTING, gszLOADMAP_DATE_SETTING, _T(""))) == _T(""))
	{
		m_szLoadMapDate = gszLOAD_MAP_DATE;
		WriteProfileString(gszPROFILE_SETTING, gszLOADMAP_DATE_SETTING, gszLOAD_MAP_DATE);
	}

	//Retrieve Load Map Time
	if ((m_szLoadMapTime = GetProfileString(gszPROFILE_SETTING, gszLOADMAP_TIME_SETTING, _T(""))) == _T(""))
	{
		m_szLoadMapTime = gszLOAD_MAP_TIME;
		WriteProfileString(gszPROFILE_SETTING, gszLOADMAP_TIME_SETTING, gszLOAD_MAP_TIME);
	}

	//Retrieve Map Start Date (Map file 1st started date)
	if ((m_szMapStartDate = GetProfileString(gszPROFILE_SETTING, gszMAPSTART_DATE_SETTING, _T(""))) == _T(""))
	{
		m_szMapStartDate = gszLOAD_MAP_DATE;
		WriteProfileString(gszPROFILE_SETTING, gszMAPSTART_DATE_SETTING, gszLOAD_MAP_DATE);
	}

	//Retrieve Map Start Time (Map file 1st started time)
	if ((m_szMapStartTime = GetProfileString(gszPROFILE_SETTING, gszMAPSTART_TIME_SETTING, _T(""))) == _T(""))
	{
		m_szMapStartTime = gszLOAD_MAP_TIME;
		WriteProfileString(gszPROFILE_SETTING, gszMAPSTART_TIME_SETTING, gszLOAD_MAP_TIME);
	}

	//Retrieve Map Run Time (in seconds)
	m_nMapRunTime = (int)GetProfileInt(gszPROFILE_SETTING, gszMAPRUN_TIME_SETTING, 0);
	if (m_nMapRunTime == 0)
	{
		WriteProfileInt(gszPROFILE_SETTING, gszMAPRUN_TIME_SETTING, m_nMapRunTime);
	}

	// Retrieve Default Device File
	if ((m_szDeviceFile = GetProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, _T(""))) == _T(""))
	{
		m_szDeviceFile = gszDEVICE_FILE_NAME;
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, gszDEVICE_FILE_NAME);
	}

	CString szTemp;
	// Retrieve Passwords
	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszENGINEER_PASSWORD, _T(""))) == _T(""))
	{
		m_szEngineerPassword = "engineer";
		WriteProfileString(gszPROFILE_SETTING, gszENGINEER_PASSWORD, CodePassword(m_szEngineerPassword, gnEngineerShift));
	}
	else
	{
		m_szEngineerPassword = CodePassword(szTemp, -gnEngineerShift);
	}
	//v4.38T5
	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszTECHNICIAN_PASSWORD, _T(""))) == _T(""))
	{
		m_szTechnicianPassword = "technician";
		WriteProfileString(gszPROFILE_SETTING, gszTECHNICIAN_PASSWORD, CodePassword(m_szTechnicianPassword, gnEngineerShift));
	}
	else
	{
		m_szTechnicianPassword = CodePassword(szTemp, -gnEngineerShift);
	}

	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszADMIN_PASSWORD, _T(""))) == _T(""))
	{
		m_szAdminPassword = "FullControl";
		WriteProfileString(gszPROFILE_SETTING, gszADMIN_PASSWORD, CodePassword(m_szAdminPassword, gnAdminShift));
	}
	else
	{
		m_szAdminPassword = CodePassword(szTemp, -gnAdminShift);
	}

	// Hmi Access password retrive
	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszHmiAccessCodeAdmin, _T(""))) == _T(""))
	{
		m_szHmiAccessPwAdmin		= HMI_SECURITY_PW_ADM;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeAdmin, EncodePassword(m_szHmiAccessPwAdmin));
	}
	else
	{
		m_szHmiAccessPwAdmin = DecodePassword(szTemp);
	}
	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszHmiAccessCodeServi, _T(""))) == _T(""))
	{
		m_szHmiAccessPwService		= HMI_SECURITY_PW_SER;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeServi, EncodePassword(m_szHmiAccessPwService));
	}
	else
	{
		m_szHmiAccessPwService = DecodePassword(szTemp);
	}
	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszHmiAccessCodeSuper, _T(""))) == _T(""))
	{
		m_szHmiAccessPwSuper		= HMI_SECURITY_PW_SUP;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeSuper, EncodePassword(m_szHmiAccessPwSuper));
	}
	else
	{
		m_szHmiAccessPwSuper = DecodePassword(szTemp);
	}
	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszHmiAccessCodeEngin, _T(""))) == _T(""))
	{
		m_szHmiAccessPwEngineer		= HMI_SECURITY_PW_ENG;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeEngin, EncodePassword(m_szHmiAccessPwEngineer));
	}
	else
	{
		m_szHmiAccessPwEngineer = DecodePassword(szTemp);
	}
	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszHmiAccessCodeTechn, _T(""))) == _T(""))
	{
		m_szHmiAccessPwTechnician		= HMI_SECURITY_PW_TEC;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeTechn, EncodePassword(m_szHmiAccessPwTechnician));
	}
	else
	{
		m_szHmiAccessPwTechnician = DecodePassword(szTemp);
	}
	// Hmi Access password retrive

	//v3.70T3
	//Specially for PLLM REBEL MS899dla only
	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszPLLM_REBEL_PASSWORD, _T(""))) == _T(""))
	{
		m_szPLLMRebelPassword = "ms899dla";
		WriteProfileString(gszPROFILE_SETTING, gszPLLM_REBEL_PASSWORD, m_szPLLMRebelPassword);
	}
	else
	{
		m_szPLLMRebelPassword = szTemp;
	}

	//v4.17T7	//SanAn password file path
	if ((szTemp = GetProfileString(gszPROFILE_SETTING, gszPLLM_PASSWORD_FILE_PATH, _T(""))) == _T(""))
	{
		m_szPasswordFilePath = gszUSER_DIRECTORY;
		WriteProfileString(gszPROFILE_SETTING, gszPLLM_PASSWORD_FILE_PATH, m_szPasswordFilePath);
	}
	else
	{
		m_szPasswordFilePath = szTemp;
	}

	szTemp = GetProfileString(gszPROFILE_SETTING, "AOI OCR die map format", "B2B2B2");
	WriteProfileString(gszPROFILE_SETTING, "AOI OCR die map format", szTemp);

	//xyz
	if ((m_ulPLLMProduct = GetProfileInt(gszPROFILE_SETTING, MS896A_PLLM_PRODUCT, 0)) == 0)
	{
		WriteProfileInt(gszPROFILE_SETTING, MS896A_PLLM_PRODUCT, 0);
		m_ulPLLMProduct = PLLM_REBEL;
	}

	if ((m_bLoadPkgForNewWaferFrame = GetProfileInt(gszPROFILE_SETTING, gszLoadPkgForNewWafer, 0)) == 0)
	{
		WriteProfileInt(gszPROFILE_SETTING, gszLoadPkgForNewWafer, 0);
		m_bLoadPkgForNewWaferFrame = FALSE;
	}

	//m_bIsPLLMDieFab = FALSE;
	//if ( (m_bIsPLLMDieFab = ReadLabelConfig()) == TRUE )
	//{
	//for Lumileds Singapore Die Fab Line
	//	if ((m_szPLLMDieFabMESLot = GetProfileString(gszPROFILE_SETTING, gszDIE_FAB_MES_LOT, _T(""))) == _T(""))
	//	{
	//		m_szPLLMDieFabMESLot = gszBIN_SPEC_VERSION;
	//		WriteProfileString(gszPROFILE_SETTING, gszDIE_FAB_MES_LOT, gszBIN_SPEC_VERSION);
	//	}
	//
	//	CMS896AStn::m_bIsAlwaysPrintLabel = TRUE;
	//}

	//Get Expander Type & status
	if (GetProfileInt(gszPROFILE_SETTING, gszEXPANDER_TYPE, 0) == 0)
	{
		WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_TYPE, 0);
	}
	else
	{
		if (GetProfileInt(gszPROFILE_SETTING, gszEXPANDER_STATUS, 0) == 1)
		{
			//andrew
			//if ( AfxMessageBox("Expander is opened!\nContinue to start?", MB_YESNO|MB_SYSTEMMODAL|MB_DEFBUTTON2|MB_ICONQUESTION) == IDNO )
			//{
			//	return FALSE;
			//}
		}
	}

	//v4.48A2
	m_bIsMSLicenseRegistered = GetProfileInt(gszPROFILE_SETTING, gszLOG_MS_REGISTER_LICENSE, 0);

	WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER_STATUS,  0);
	WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER2_STATUS, 0);

	nTemp = GetProfileInt(gszPROFILE_SETTING, gszWprDoFPC, 0);
	WriteProfileInt(gszPROFILE_SETTING, gszWprDoFPC, nTemp);

	//Get Lot is started
	m_bNewLotStarted = (BOOL)GetProfileInt(gszPROFILE_SETTING, gszSTART_NEW_LOT, 0);
	if (m_bNewLotStarted == 0)
	{
		WriteProfileInt(gszPROFILE_SETTING, gszSTART_NEW_LOT, 0);
	}

	//4.52D10 Get Profile Int
	m_bWaferLabelFile = (BOOL)GetProfileInt(gszPROFILE_SETTING, gszENABLE_WAFER_LABEL_FILE, 0);
	if (m_bWaferLabelFile == 0)
	{
		WriteProfileInt(gszPROFILE_SETTING, gszENABLE_WAFER_LABEL_FILE, 0);
	}
	//Get Shift ID	//v3.32T3	//Avago
	m_szShiftID			= GetProfileString(gszPROFILE_SETTING, gszSHIFT_ID_SETTING, _T(""));

	//Retrieve EnableHardware Option
	//Note that EnableAllHardware() should be called after LoadData();
	m_fEnableHardware	= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ALL_HARDWARE,	1);
	m_bDisableBHModule	= GetProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_BH_MODULE,		0);
	m_bDisableBTModule	= GetProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_BT_MODULE,		0);
	m_bDisableBLModule	= GetProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_BL_MODULE,		0);
	m_bDisableNLModule	= GetProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_NL_MODULE,		0);		//andrewng //2020-0708
	m_bDisableWLModule	= GetProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_WL_MODULE,		0);
	m_bDisableWTModule	= GetProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_WT_MODULE,		0);
	m_bDisableWLModuleWithExp = GetProfileInt(gszPROFILE_HW_CONFIG, gszDISABLE_WL_MODULE_WITH_EXP,		0);

	//Retrieve OS Image version
	m_szOSImageReleaseNo = GetProfileString(gszPROFILE_HW_CONFIG, gszOS_IMAGE_VERSION, _T(""));
	if (m_szOSImageReleaseNo == _T(""))		//v2.95T1
	{
		m_szOSImageReleaseNo = _T("10-M00093V1-00");	//Dual-core CPU image
		WriteProfileString(gszPROFILE_HW_CONFIG, gszOS_IMAGE_VERSION, m_szOSImageReleaseNo);
	}

	//Retrieve Machine logging option
	m_bEnableMachineLog = (BOOL)GetProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_OPTION, 0);
	CMSLogFileUtility::Instance()->SetEnableMachineLog(m_bEnableMachineLog);
	WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_OPTION, (int)m_bEnableMachineLog);
	//v4.59A12
	m_bEnableTableIndexLog = (BOOL)GetProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_TABLEINDEX_OPTION, 0);
	CMSLogFileUtility::Instance()->SetEnableTableIndexLog(m_bEnableTableIndexLog);
	WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_TABLEINDEX_OPTION, (int)m_bEnableTableIndexLog);
	//v4.06
	//m_bEnableWtMachineLog = (BOOL)GetProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_WT_OPTION, 0);
	m_bEnableWtMachineLog = FALSE;
	CMSLogFileUtility::Instance()->SetEnableWtMachineLog(m_bEnableWtMachineLog);
	WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_WT_OPTION, (int)m_bEnableWtMachineLog);

	//v4.46T26	//Cree HuiZhou pkg dies
	m_lBHZ1HomeOffset	= GetProfileInt(gszPROFILE_SETTING, gszBHZ1_HOME_OFFSET, 0);
	m_lBHZ2HomeOffset	= GetProfileInt(gszPROFILE_SETTING, gszBHZ2_HOME_OFFSET, 0);

	// CP100 generate map file options
	nTemp = (INT) GetProfileInt(gszPROFILE_SETTING, _T("CP100 2 Output Files"), 0);
	WriteProfileInt(gszPROFILE_SETTING, _T("CP100 2 Output Files"), nTemp);

	INT nRotateUnderCam = GetProfileInt(gszPROFILE_SETTING, gszES_ROTATE_T_UNDER_CAM, 0);
	if( GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA) )
		nRotateUnderCam = 2;
	WriteProfileInt(gszPROFILE_SETTING, gszES_ROTATE_T_UNDER_CAM, nRotateUnderCam);

	int nPass = (int) GetProfileInt(gszPROFILE_SETTING, _T("SemitekPassword"), 201277);
	if( nPass<=0 )
	{
		nPass = 201277;
	}
	if( nPass<99999 )
	{
		nPass = 100000 + nPass;
	}
	WriteProfileInt(gszPROFILE_SETTING, _T("SemitekPassword"), nPass);

	UINT unItem = GetProfileInt(gszPROFILE_SETTING, _T("CP100Item4"), 1);
	WriteProfileInt(gszPROFILE_SETTING, _T("CP100Item4"), unItem);
	unItem = GetProfileInt(gszPROFILE_SETTING, _T("CP100Item2"), 1);
	WriteProfileInt(gszPROFILE_SETTING, _T("CP100Item2"), unItem);

	UINT unEnable = GetProfileInt(gszPROFILE_SETTING, _T("ScanPassScorePassWord"), 0);
	if (GetCustomerName() == CTM_SEMITEK)
	{
		unEnable = 1;
	}
	WriteProfileInt(gszPROFILE_SETTING, _T("ScanPassScorePassWord"), unEnable);

	unEnable = GetProfileInt(gszPROFILE_SETTING, _T("Stop if prescan Lower than PassScore"), 0);
	WriteProfileInt(gszPROFILE_SETTING, _T("Stop if prescan Lower than PassScore"), unEnable);

	unEnable = GetProfileInt(gszPROFILE_SETTING, _T("Cree Check Map Empty In Wafer"), 0);
	WriteProfileInt(gszPROFILE_SETTING, _T("Cree Check Map Empty In Wafer"), unEnable);

	UINT unNoDieExtra = GetProfileInt(gszPROFILE_SETTING, _T("PR no die extra limit"), 0);
	WriteProfileInt(gszPROFILE_SETTING, _T("PR no die extra limit"), unNoDieExtra);

	CString szCut = GetProfileString(gszPROFILE_SETTING, _T("CP FOV sub cut num(double)"), "1.0");
	DOUBLE dGet = fabs(atof(szCut));
	if ( dGet > 1.0 )
		dGet = 1.0/dGet;
	else if( dGet==0 )
		dGet = 1.0;
	szCut.Format("%.2f", dGet);
	WriteProfileString(gszPROFILE_SETTING, _T("CP FOV sub cut num(double)"), szCut);

	unNoDieExtra = GetProfileInt(gszPROFILE_SETTING, _T("BPR FOV ROI Shrink Factor"), 100);
	WriteProfileInt(gszPROFILE_SETTING, _T("BPR FOV ROI Shrink Factor"), unNoDieExtra);

	UINT nPerRatio = GetProfileInt(gszPROFILE_SETTING, _T("Rescan Total Loss Ration in 10000"), 0);
	WriteProfileInt(gszPROFILE_SETTING, _T("Rescan Total Loss Ration in 10000"), nPerRatio);
//	WriteProfileInt(gszPROFILE_SETTING, _T("Asm special debug flag"), 0);
	m_bEnableAutoRecoverPR = (BOOL)GetProfileInt(gszPROFILE_SETTING, _T("Auto Retry for scan PR error"), 0);
	WriteProfileInt(gszPROFILE_SETTING, _T("Auto Retry for scan PR error"), m_bEnableAutoRecoverPR);

	UINT unDelay = GetProfileInt(gszPROFILE_SETTING, _T("Sample Begin Delay"), 0);
	WriteProfileInt(gszPROFILE_SETTING, _T("Sample Begin Delay"), unDelay);

	UINT unWarmLogin = GetProfileInt(gszPROFILE_SETTING, _T("Warm Re-log in no need align"), 0);	//	registry, warm log in, no alignment again.
	WriteProfileInt(gszPROFILE_SETTING, _T("Warm Re-log in no need align"), unWarmLogin);

	UINT unDataLog = GetProfileInt(gszPROFILE_SETTING, _T("Auto Cycle NuMotion Data Log"), 0);	//	registry, auto cycle, do data log for ADV
	WriteProfileInt(gszPROFILE_SETTING, _T("Auto Cycle NuMotion Data Log"), unDataLog);

	unDataLog = GetProfileInt(gszPROFILE_SETTING, _T("Gen Temp File Bond Die Counter"), 500);
	WriteProfileInt(gszPROFILE_SETTING, _T("Gen Temp File Bond Die Counter"), unDataLog);

	szTemp = GetProfileString(gszPROFILE_SETTING, _T("MS Aln Table Theta"), "0.0");	//	init when start up.
	WriteProfileString(gszPROFILE_SETTING, _T("MS Aln Table Theta"), szTemp);		//	init when start up.

	//v4.46T26
	CMSLogFileUtility::Instance()->MS_LogOperation("    Retrieve system Registry done");
}

CString CMS896AApp::GetCustomerName()
{
	return m_szCustomer;
}

CString CMS896AApp::GetProductLine()
{
	return m_szProductLine;
}

BOOL CMS896AApp::IsPLLMRebel()
{
	return m_bIsPLLMRebel;
}

BOOL CMS896AApp::IsToSaveMap()
{
	return m_bSaveMapFile;
}

LONG CMS896AApp::CheckLoginLevel()	// 0 OP; 1 Tech; 2 Engr 3: Admin
{
	if( m_bOperator )
		return 0;
	if( m_bTechnician )
		return 1;
	if( m_bEngineer )
		return 2;
	if( m_bAdministrator )
		return 3;

	return 0;
}	// rebel special

BOOL CMS896AApp::GetFeatureStatus(CString szFuncName)
{
	if (szFuncName == MS896A_FUNC_DISABLE_BUZZER)
	{
		return m_bDisableBuzzer;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_PREBOND)
	{
		return m_bEnablePreBond;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_POSTBOND)
	{
		return m_bEnablePostBond;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_REFDIE)
	{
		return m_bEnableRefDie;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_CHAR_REFDIE)
	{
		return m_bEnableCharRefDie;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_REFDIE_FACE_VALUE)		//v4.48A26	//Avago
	{
		return m_bEnableRefDieFaceValueCheck;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_NGPICK)					//v4.51A19	//Silan MS90
	{
		return m_bEnableNGPick;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY)	//v4.52A14	//Osram Germany
	{
		return m_bUseColletOffsetWoEjtXY;
	}

	if( szFuncName == MS896A_FUNC_GENERAL_AOI_OCR_DIE )
	{
		return m_bEnableAoiOcrDie;
	}	// AOI ocr die option

	if (szFuncName == MS896A_FUNC_ENABLE_THERMAL_EJECTOR)
	{
		return m_bEnableThermalEjector;
	}

	if (szFuncName == MS896A_FUNC_ENABLE_COLOR_CAMERA)
	{
		return m_bEnableColorCamera;
	}

	if (szFuncName == MS896A_FUNC_DISPLAY_SORT_BIN_ITEM)
	{
		return m_bDisplaySortBinItemMsg;
	}

	if (szFuncName == MS896A_FUNC_SEARCH_DEEP_IN_FOLDER)
	{
		return m_bLoadMapSearchDeepInFolder;
	}

	if (szFuncName == MS896A_FUNC_COMPLETE_FILENAME)
	{
		return m_bSearchCompleteFilename;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_USE_SCN)
	{
		return m_bEnableSCNFile;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_PSM_EXIST_CHECK)
	{
		return m_bEnablePsmFileExistCheck;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_BATCH_ID_FILE_CHECK)
	{
		return m_bEnableBatchIdFileCheck;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_DISABLE_FIVE_POINT_CHECK_MSG_SEL)
	{
		return m_bDisableWaferMapFivePointCheckMsgSelection;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_USE_BLOCK)
	{
		return m_bEnableBlockFunc;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_CHK_SEP_GRADE)
	{
		return m_bEnableSepGrade;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_EMPTYCHECK)		//Block2
	{
		return m_bEnableEmptyCheck;
	}

	if (szFuncName == MS896A_FUNC_WPR_BACK_LIGHT)			//v4.46T28	//Cree HuiZhou
	{
		return m_bEnableWPRBackLight;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_ENABLE_BINMAP)	//PLLM REBEL binmap support
	{
		return m_bEnableBinMap;
	}

	if (szFuncName == MS896A_FUNC_MULTIGRADE_SORT_TO_SINGLEBIN)
	{
		return m_bMultiGradeSortToSingleBin;				//v4.15T8	//Osram Germany Layer-sort
	}

	if (szFuncName == MS896A_FUNC_CMLT_REWORK)
	{
		return m_bEnableCMLTReworkFcn;						//v4.15T9	//CMLT Rework 
	}

	if (szFuncName == MS896A_FUNC_NO_IGNORE_GRADE_ON_INTERFACE)
	{
		return m_bNoIgnoreGradeOnInterface;
	}

	if (szFuncName == MS896A_FUNC_CHECK_IGNORE_GRADE_COUNT)
	{
		return m_bCheckIgnoreGradeCount;
	}

	if (szFuncName == MS896A_FUNC_CHECK_MAP_COLUMNS)		//v4.40T13	//BlueLight
	{
		return m_bEnableMapColumnsCheck;
	}

	if (szFuncName == MS896A_FUNC_OSRAM_RESORT_MODE)		//v4.21T3	//Osram Penang RESORT mode
	{
		return m_bEnableOsramResortMode;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_MANUAL_INPUT_BC)	//v4.06
	{
		return m_bEnableManualInputBC;
	}

	if (szFuncName == MS896A_FUNC_NO_PR_RECORD_PKG_FILE)
	{
		return m_bNoPRRecordForPKGFile;
	}

	if (szFuncName == MS896A_FUNC_TOOLS_USAGE_RECORD)
	{
		return m_bEnableToolsUsageRecord;
	}

	if (szFuncName == MS896A_FUNC_ENABLE_ITEM_LOG)
	{
		return m_bEnableItemLog;
	}

	if (szFuncName == MS896A_FUNC_ALARM_LAMP_BLINK)
	{
		return m_bEnableAlarmLampBlink;
	}

	if (szFuncName == MS896A_FUNC_AMI_FILE)
	{
		return m_bEnableAmiFile;
	}

	if (szFuncName == MS896A_FUNC_BARCODE_LENGTH_CHECK)
	{
		return m_bEnableBarcodeLengthCheck;
	}

	if (szFuncName == MS896A_FUNC_INPUT_COUNT_SETUP_FILE)
	{
		return m_bEnableInputCountSetupFile;
	}

	if (szFuncName == MS896A_FUNC_CLEAR_BIN_BEFORE_NEW_LOT)
	{
		return m_bForceClearBinBeforeNewLot;
	}

	if (szFuncName == MS896A_FUNC_NEW_LOT_CREATE_DIR)
	{
		return m_bNewLotCreateDirectory;
	}

	if (szFuncName == MS896A_FUNC_WL_BC_SELECTION)			//v4.38T2	//SanAn ES101 new bc scanner support
	{
		return m_bWLBarcodeSelection;
	}

	if( szFuncName == MS896A_FUNC_ON_OFF_2D_BAR_SCANNER)
	{
		return m_bOnOff2DBarcodeScanner;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_SEMI_AUTO)		//v3.35T8		//Avago
	{
		return m_bEnableSemiAutoMode;
	}

	if (szFuncName == MS896A_FUNC_MACHINE_REPORT)
	{
		return m_bEnableMachineReport;
	}

	if (szFuncName == MS896A_FUNC_HMI_OPMENU_SHORTCUT)		//v4.19
	{
		return m_bEnableOPMenuShortcut;			
	}

	if (szFuncName == MS896A_FUNC_HMIBHSHORTCUT)
	{
		return m_bEnableBHShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMIWPRSHORTCUT)
	{
		return m_bEnableWPRShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMIBPRSHORTCUT)
	{
		return m_bEnableBPRShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMIBINBLKSHORTCUT)
	{
		return m_bEnableBinBlkShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMI_WTSHORTCUT)
	{
		return m_bEnableWTShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMI_PRRECORD_SHORTCUT)
	{
		return m_bEnablePRRecordShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMI_BINTABLE_SHORTCUT)
	{
		return m_bEnableBinTableShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMI_DISABLE_PKG_SHORTCUT)
	{
		return m_bDisablePKGShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMIPICKNPLACESHORTCUT)
	{
		return m_bEnablePicknPlaceShortcut;		//v4.20		//SanAn
	}

	if (szFuncName == MS896A_FUNC_HMI_TOGGLE_WAFER_RESORT_MODE)
	{
		return m_bToggleWaferResortScanMode;
	}

	if (szFuncName == MS896A_FUNC_HMI_WAFER_BIN_RESORT_MODE)
	{
		return m_bEnableWaferToBinResort;
	}

	if (szFuncName == MS896A_FUNC_HMI_OPTIONSTAB_SHORTCUT)
	{
		return m_bEnableOptionsTabShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMI_DISABLE_OPTION_SHORTCUT)
	{
		return m_bDisableOptionsShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMI_SYSTEMSERVICE_SHORTCUT)
	{
		return m_bEnableSystemServiceShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMI_STEP_MOVE_SHORTCUT)
	{
		return m_bEnableStepMoveShortcut;
	}

	if (szFuncName == MS896A_FUNC_HMI_PHY_BLOCK_SHORTCUT)
	{
		return m_bEnablePhyBlockShortcut; 
	}

	if (szFuncName == MS896A_FUNC_HMI_OPERATION_MODE_SHORTCUT)
	{
		return m_bEnableOperationModeShortcut;
	}
	
	if (szFuncName == MS896A_FUNC_HOST_COMM_SECSGEM)
	{
		return m_bEnableSECSComm;
	}

	if (szFuncName == MS896A_FUNC_HOST_COMM_TCPIP)
	{
		return m_bEnableTCPComm;
	}

	if (szFuncName == MS896A_FUNC_REFDIESTATUSFILE)
	{
		return m_bGenRefDieStatusFile;
	}

	if (szFuncName == MS896A_FUNC_LABELPRINTOUT)
	{
		return m_bGenLabel;
	}

	if (szFuncName == MS896A_FUNC_LABELPRINTOUT_XML)
	{
		return m_bGenLabelWithXMLFile;
	}

	if (szFuncName == MS896A_FUNC_BINNO_DIRECTORY)
	{
		return m_bGenBinLotDirectory;
	}

	if (szFuncName == MS896A_FUNC_BIN_WORKNO)
	{
		return m_bEnableBinWorkNo;
	}

	if (szFuncName == MS896A_FUNC_FILE_NAME_PREFIX)
	{
		return m_bEnableFileNamePrefix;
	}

	if (szFuncName == MS896A_FUNC_VISION_BT_REALIGN_FF_MODE)
	{
		return m_bBTRealignWithFFMode;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_POLYGON_DIE) 
	{
		return m_bEnablePolygonDie;
	}

	if (szFuncName == MS896A_FUNC_OPT_BIN_COUNT_DYN_ASSIGN_GRADE) 
	{
		return m_bUseOptBinCountDynAssignGrade;
	}
	
	if (szFuncName == MS896A_FUNC_OPEN_MATCH_PKG_NAME_CHECK) 
	{
		return m_bOpenMatchPkgNameCheck;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_CHINESE_MENU) 
	{
		return m_bEnableChineseMenu;
	}

	if (szFuncName == MS896A_FUNC_ENABLE_SUMMARY_PAGE)
	{
		return m_bEnableSummaryPage;
	}

	if( szFuncName == MS896A_FUNC_ENABLE_BOND_ALARM_PAGE)
	{
		return m_bEnableBondAlarmPage;
	}

	if (szFuncName == MS896A_FUNC_ENABLE_EMPTY_BINFRAME_CHECK)
	{
		return m_bEnableEmptyBinFrameCheck;
	}

	if (szFuncName == MS896A_FUNC_ENABLE_RESET_MAG_CHECK)
	{
		return m_bEnableResetMagzCheck;
	}

	if (szFuncName == MS896A_FUNC_WAFER_LOT_FILE_PROTECTION)
	{
		return m_bEnableWaferLotFileProtection;
	}

	if (szFuncName == MS896A_FUNC_WAFER_END_CLEAR_MAP)
	{
		return m_bForceClearMapAfterWaferEnd;
	}

	if (szFuncName == MS896A_FUNC_BH_VAC_NEUTRALSTATE)
	{
		return m_bEnableBHZVacNeutralState;
	}

	if (szFuncName == MS896A_FUNC_WAFFLE_PAD_BOND_DIE)
	{
		return m_bWafflePadBondDie;
	}

	if (szFuncName == MS896A_FUNC_SLOW_EJ_PROFILE)
	{
		return m_buseSlowEjProfile;
	}

	if (szFuncName == MS896A_FUNC_WAFFLE_PAD_IDENTIFICATION)
	{
		return m_bWafflePadIdentification;
	}
	//if (szFuncName == MS896A_FUNC_ENABLE_MOUSE_CONTROL)
	//{
	//	return m_bEnableMouseControl;
	//}

	if (szFuncName == MS896A_FUNC_WAFERLOT_WITH_BIN_SUMMARY_FORMAT)
	{
		return m_bWaferLotWithBinSummaryFormat;
	}

	if (szFuncName == MS896A_FUNC_WAFERLOT_LOADED_MAP_CHECK)
	{
		return m_bEnableWaferLotLoadedMapCheck;
	}

	if (szFuncName == MS896A_FUNC_ENABLE_PORTABLE_PKG_FILE)
	{
		return m_bSupportPortablePKGFile;
	}

	if (szFuncName == MS896A_FUNC_PORTABLE_PKG_INFO_PAGE)
	{
		return m_bPortablePKGInfoPage;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_BINFRAMESTATUSSUM)
	{
		return m_bBinFrameStatusSummary;
	}

	if (szFuncName == MS896A_FUNC_BIN_SNR_SEL_FORMAT)
	{
		return m_bDisableBinSNRFormat;
	}

	if (szFuncName == MS896A_FUNC_SEL_CLEAR_COUNT_FORMAT)
	{
		return m_bDisableClearCountFormat;
	}

	if (szFuncName == MS896A_FUNC_DYNMAPHEADER)
	{
		return m_bEnableDynMapHeaderFile;
	}

	if (szFuncName == MS896A_FUNC_BACKUP_TEMP_FILE)
	{
		return m_bBackupTempFile;
	}

	if (szFuncName == MS896A_FUNC_BACKUP_OUTPUT_TEMP_FILE)
	{
		return m_bBackupOutputTempFile;
	}

	if (szFuncName == MS896A_FUNC_FORCE_DISABLE_HAVE_OTHER_FILE)
	{
		return m_bForceDisableHaveOtherFile;
	}

	if (szFuncName == MS896A_FUNC_MANUAL_UPLOAD_BIN_SUMMARY_FILE)
	{
		return m_bEnableManualUploadBinSummary;
	}

	if (szFuncName == MS896A_FUNC_GEN_ACCUMULATE_DIE_COUNT_REPORT)
	{
		return m_bGenerateAccumulateDieCountReport;
	}

	if (szFuncName == MS896A_FUNC_VISION_DUAL_COLOR) 
	{
		return m_bEnablePRDualColor;
	}

	if (szFuncName == MS896A_FUNC_PRS_MOTORIZEDZOOM)
	{
		return m_bEnableMotorizedZoom;
	}

	//v2.78T2
	if (szFuncName == MS896A_FUNC_VISION_CIRCLE_DETECTION) 
	{
		return m_bPrCircleDetection;
	}

	if (szFuncName == MS896A_FUNC_VISION_AUTOLEARN_REFDIE) 
	{
		return m_bPrAutoLearnRefDie;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_CHECK_MASTER_PITCH) 
	{
		return m_bWtCheckMasterPitch;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_SWALK_1STDIE) //v3.15T5
	{
		return m_bEnable1stDieFinder;
	}

	if (szFuncName == MS896A_FUNC_VISION_LINE_INSPECTION)//v3.17T1
	{
		return m_bPrLineInspection;
	}

	if (szFuncName == MS896A_FUNC_VISION_WPR_2_LIGHTING)//v3.44T1
	{
		return m_bPrEnableWpr2Lighting;
	}

	if (szFuncName == MS896A_FUNC_VISION_AUTO_ENLARGE_SRCHWND)		//Block2
	{
		return m_bEnablePrAutoEnlargeSrchWnd;
	}

	if (szFuncName == MS896A_FUNC_ALIGN_WAFER_IMAGE_LOG)
	{
		return m_bEnableAlignWaferImageLog;
	}

	if (szFuncName == MS896A_FUNC_VISION_POSTSEAL_OPTICS)	//v3.71T5
	{
		return m_bEnablePrPostSealOptics;
	}

	if (szFuncName == MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS)//v4.39T11
	{
		//if (!m_bMS100Plus9InchOption)
		//	return FALSE;
		//else if (!m_bIsPLLMRebel)
		//	return FALSE;
		//else
			return m_bEnableBHPostSealOptics;		//Only available for PLLM MS109
	}

	if (szFuncName == MS896A_FUNC_VISION_BH_UPLOOK_PR)
	{
		//v4.58A3	//MS90	//Allow user to Turn ON/OFF this fcn on menu
		//return m_bEnableBHUplookPrFcn;
		if (m_bEnableBHUplookPrFcn && m_bUseBHUplookPrFcn)
			return TRUE;
		else
			return FALSE;
	}
	if (szFuncName == MS896A_FUNC_VISION_BH_UPLOOK_PR_ONLY)
	{
		return m_bEnableBHUplookPrFcn;
	}


	if( szFuncName == ES_FUNC_VISION_WL_CONTOUR_CAMERA)
	{
		return m_bEnableESContourCamera;
	}

	if( szFuncName == MS60_FUNC_VISION_DUAL_PATH )
	{
		return m_bEnableDualPathCamera;
	}	//	Dual Path

	if (szFuncName == MS896A_FUNC_VISION_OFFLINE_PBTEST)	//v4.11T3
	{
		return m_bOfflinePostbondTest;		
	}

	if (szFuncName == MS896A_FUNC_MANUAL_PR_JS_SPEED)		//v4.16T6	//Huga fcn
	{
		return m_bUseManualPRJsSpeed;		
	}

	if (szFuncName == MS896A_FUNC_MxN_LF_WND)				//v4.43T2	//SanAn
	{
		return m_bWaferPRMxNLFWnd;		
	}
	//4.53D18
	if (szFuncName == MS896A_FUNC_MAP_DIE_MIX)
	{
		return m_bWaferMapDieMixing;		
	}

	if (szFuncName == MS896A_FUNC_MS60_LF_SEQ)				//v4.47T3
	{
		return m_bWaferPrMS60LFSequence;		
	}

	if( szFuncName == MS60_VISION_5M_SORTING_FF_MODE )
	{
		return m_bMS60SortingFFMode;
	}

	//if (szFuncName == MS896A_FUNC_NEW_CT_ENHANCEMENT)		//v2.83T2
	//{
	//	return m_bNewCtEnhancement;
	//}

	if (szFuncName == MS896A_FUNC_NEW_STEPPER_ENC)			//v2.83T2
	{
		return m_bUseNewStepperEnc;
	}

	if (szFuncName == MS896A_FUNC_APPEND_IN_WAFERID)
	{
		return m_bEnableAppendInfoInWaferId;
	}

	if (szFuncName == MS896A_FUNC_WAFERID_IN_MSGSUMMARY)
	{
		return m_bAddWaferIdInMsgSummaryFile;
	}

	if (szFuncName == MS896A_FUNC_DIS_OK_SELECTION_IN_ALERT_MSG)
	{
		return	m_bDisableOKSelectionInAlertMsg;
	}

	if (szFuncName == MS896A_FUNC_GRADE_MAPPING_FILE)
	{
		return m_bEnableGradeMappingFile;
	}

	if (szFuncName == MS896A_FUNC_PICK_N_PLACE_OUTPUT)
	{
		return m_bEnablePickNPlaceOutputFile;
	}

	if (szFuncName == MS896A_FUNC_DISABLE_DEFAULT_SEARCH_WND)
	{
		return m_bDisableDefaultSearchWnd;
	}

	if (szFuncName == MS896A_FUNC_WAFERENDFILEPROTECTION)
	{
		return m_bWaferEndFileGenProtection;
	}

	if (szFuncName == MS896A_FUNC_AUTO_GEN_WAFEREND)
	{
		return m_bAutoGenWaferEndFile;
	}

	if (szFuncName == MS896A_FUNC_AUTO_GEN_WAFEREND_PRESCAN)
	{
		return m_bAutoGenWaferEndAfterPrescan;		//v4.40T14	//3E
	}

	if (szFuncName == MS896A_FUNC_CLR_BIN_BY_GRADE)
	{
		return m_bUseClearBinByGrade;
	}

	if (szFuncName == MS896A_FUNC_ENABLE_EXTRA_CLR_BIN_INTO)
	{
		return m_bEnableExtraClearBinInfo;
	}

	if (szFuncName == MS896A_FUNC_DIE_TIMESTAMP)
	{
		return m_bAddDieTimeStamp;
	}

	if (szFuncName == MS896A_FUNC_DIE_PROCESSTIME)
	{
		return m_bCheckProcessTime;
	}

	if (szFuncName == MS896A_FUNC_ADD_SORTING_SEQUENCE)
	{
		return m_bAddSortingSequence;
	}

	if (szFuncName == MS896A_FUNC_OF_FORMAT_SELECT)
	{
		return m_bOutputFormatSelInNewLot;
	}

	if (szFuncName == MS896A_FUNC_MULTIPLE_HEADER_PAGE)
	{
		return m_bEnableMultipleMapHeaderPage;
	}

	if (szFuncName == MS896A_FUNC_AUTO_GEN_BIN_BLK_SUMMARY)
	{
		return m_bAutoGenBinBlkCountSummary;
	}

	if (szFuncName == MS896A_REMOVE_BACKUP_OUTPUTFILE)
	{
		return m_bRemoveBackupOutputFile;
	}

	if (szFuncName == MS896A_FUNC_CHANGE_GRADE_BACKUPTEMP_FILE)
	{
		return m_bChangeGradeBackupTempFile;
	}

	if (szFuncName == MS896A_KEEP_OUTPUT_FILE_LOG)
	{
		return m_bKeepOutputFileLog;
	}

	if (szFuncName == MS896A_FUNC_OTHERS_CONNECTION_TYPE)
	{
		return m_bOthersConnectionType;
	}

	if (szFuncName == MS896A_FUNC_HOST_COMM_EQUIP_MANAGER)
	{
		return m_EnableEMMode;			//v4.59A32	//Equipment Manager
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_IGNORE_GRADE_0)		//v3.28T1
	{
		return m_bMapIgnoreGrade0;
	}

	if (szFuncName == MS896A_UPDATE_WAFER_MAP_HEADER)
	{
		return m_bUpdateWaferMapHeader;
	}

	if (szFuncName == MS896A_FUNC_BL_RESET_GRADEMAG_EMPTY)		//v3.70T1
	{
		return m_bBLResetGradeMagToEmpty;
	}

	if (szFuncName == MS896A_FUNC_PPLM_SPECIAL_FCNS)			//v3.70T2
	{
		return m_bUsePLLMSpecialFcn;
	}

// prescan relative code	B
	if (szFuncName == MS896A_FUNC_WAFERMAP_ADAPT_ALGORITHM)		//v3.54T4
	{
		return m_bMapAdaptiveAlgor;
	}
	if (szFuncName == MS896A_FUNC_WAFERMAP_REALIGN)
	{
		return m_bPrescanRealignSameMap;
	}
	if (szFuncName == MS896A_FUNC_PRESCAN_EMPTY_UNMARK)
	{
		return m_bPrescanEmptyUnmark;
	}
// prescan relative code	E

	if (szFuncName == MS896A_FUNC_WAFERMAP_REVERIFY_REFDIE)		//Cree
	{
		return m_bEnableReVerifyRefDie;
	}

	if (szFuncName == MS896A_FUNC_NONBLKPICK_VERIFY_REFDIE)
	{
		return m_bNonBlkPkEndVerifyRefDieApp;
	}

	if (szFuncName == MS896A_FUNC_MANUAL_ALIGN_REFER_DIE)
	{
		return m_bManualAlignReferDie;
	}
	//v3.34
	if (szFuncName == MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD)
	{
		return m_bExArmDisableEmptyPreload;
	}

	if (szFuncName == MS896A_FUNC_NEW_BH_MOUNT)
	{
		return m_bUseNewBHMount;
	}

	if (szFuncName == MS896A_FUNC_BINBLK_TEMPLATE)
	{
		return m_bEnableBinBlkTemplate;
	}

	if (szFuncName == MS896A_FUNC_BINBLK_LOAD_RANKID_FROM_MAP)
	{
		return m_bEnableLoadRankIDFromMap;
	}
	
	if (szFuncName == MS896A_FUNC_BINBLK_USE_PT5_DIEPITCH)		//v4.42T8	//Citizen
	{
		return m_bEnableBTPt5DiePitch;
	}

	if (szFuncName == MS896A_FUNC_BINBLK_BONDAREA_OFFSET)		//v4.42T8	//Citizen
	{
		return m_bEnableBTBondAreaOffset;
	}

	if (szFuncName == MS896A_FUNC_BINBLK_GELPAD)				//v4.49A11
	{
		return m_bBTGelPadSupport;
	}

	if (szFuncName == MS896A_FUNC_BINBLK_NEWLOT_EMPTYROW)		//v4.52A6	//CYOptics, Inari
	{
		return m_bEnableBTNewLotEmptyRowFcn;
	}

	if (szFuncName == MS896A_FUNC_AUTOLOAD_NAMING_FILE)
	{
		return m_bEnableAutoLoadNamingFile;
	}

	if (szFuncName == MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT)
	{
		return m_bEnableOptimizeBinCountFunc;
	}

	if (szFuncName == MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT_PER_WAFER)
	{
		return m_bOptimizeBinCountPerWftFunc;
	}

	if (szFuncName == MS896A_FUNC_BINBLK_OPBC_LOADPATH)			
	{
		return m_bEnableOpBinCntInLoadPath;						//v4.44T5	//Genesis 3F PkgSort
	}

	if (szFuncName == MS896A_FUNC_ENABLE_RESTORE_BINRUMTIME)
	{
		return m_bEnableRestoreBinRunTimeData;
	}

	if (szFuncName == MS896A_FUNC_VISION_ADAPT_WAFER)
	{
		return m_bEnablePRAdaptWafer;
	}

	if (szFuncName == MS896A_FUNC_VISION_LRN_DIE_COUNTER_CHECK_NORMAL_RECORD)
	{
		return m_bCounterCheckWithNormalDieRecord;
	}

	if (szFuncName == MS896A_FUNC_SENSOR_NEW_BL_MAGEXIST_CHECKING)	//v3.57		//OsramGSB
	{
		return TRUE;	//m_bEnableNewBLMagExistCheck;	//Generalized for production usage in v3.58 release
	}

	if (szFuncName == MS896A_FUNC_BLBC_EMPTY_SCAN_TWICE)	//v4.51A17		//XM SanAn
	{
		return m_bBLBCEmptyScanTwice;	
	}

	if (szFuncName == MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK)
	{
		return m_bBLEmptyFrameBCCheck;	
	}

	if (szFuncName == MS896A_FUNC_SENSOR_BT_VAC_METER_CHECKING)
	{
		return m_bCheckBinVacMeter;
	}

	if (szFuncName == MS896A_FUNC_BL_USE_29_MAGSLOTS)				//v4.19		//Cree HuiZhou
	{
		return m_bBLBCUse29MagSlots;
	}

	if (szFuncName == MS896A_FUNC_MULTI_COR_OFFSET)					//v4.24T8	//Osram, Genesis MS100 9Inch
	{
		return m_bEnableMultiCOROffset;
	}

	if (szFuncName == MS896A_FUNC_SENSOR_BL_RT_COVERSENSOR_CHECK)	//v3.60T1	//Fatc
	{
		return m_bBLRealTimeCoverSensorCheck;
	}

	if (szFuncName == MS896A_FUNC_SENSOR_BL_RT_MGZN_EXCHANGE)		//v4.40T13
	{
		return m_bBLRealTimeMgznExchange;
	}

	if (szFuncName == MS896A_FUNC_SENSOR_COVER_LOCK)				//v3.60		//Cree, Lumileds (for Front & Side cover lock fcn)
	{
		return m_bEnableCoverLock;
	}

	if (szFuncName == MS896A_FUNC_SO_WAFER_CLAMP)					//v4.26T1	//Walsin China MS810EL-90
	{
		return m_bEnableWaferClamp;
	}

	if (szFuncName == MS896A_FUNC_COLLET_CLEAN_POCKET)	//v3.65		//Lexter
	{
		return m_bEnableColletCleanPocket;
	}

	if (szFuncName == MS896A_FUNC_CLEAN_EJECTOR_CAP_STOP_CYCLE)
	{
		return m_bCleanEjectorCapStop;
	}

	if (szFuncName == MS896A_FUNC_WAFER_SIZE_SELECT)
	{
		return m_bEnableWaferSizeSelect;
	}

	if (szFuncName == MS896A_FUNC_EJ_REAPLCE_AT_WAFEREND)	//v4.39T10	//Silan (WanYiMing)
	{
		return m_bReplaceEjAtWaferEnd;
	}

	if (szFuncName == MS896A_FUNC_EJ_REAPLCE_AT_WAFERSTART)	//v4.59A3	//Foxconn & Testar TW (Tom Yen)
	{
		return m_bReplaceEjAtWaferStart;
	}
	if (szFuncName == MS896A_FUNC_MANUAL_CLOSE_EXP_BC)		//v4.40T14	//TJ Sanan
	{
		return m_bManualCloseExpReadBc;
	}

	if (szFuncName == MS896A_FUNC_MS100_ORG_BH_SQ)
	{
		return m_bMS100OriginalBHSequence;
	}

	if (szFuncName == MS896A_FUNC_2D_BARCODE_CHECK)
	{
		return m_bEnable2DBarCode;
	}

	//4.52D10 Get Feature Status
	if (szFuncName == MS896A_FUNC_2ND_1D_BARCODE_CHECK)		
	{
		return m_bEnable2nd1DBarCode;
	}

	if (szFuncName == MS896A_FUNC_START_UP_COVER_SNR_CHECK)
	{
		return m_bEnableStartupCoverSensorCheck;
	}

	if (szFuncName == MS896A_FUNC_BINLOADER_COVER_SNR_CHECK)
	{
		return m_bEnableBinLoderCoverSensorCheck;
	}

	//HW options 
	if (szFuncName == gszDISABLE_BH_MODULE)
	{
		return m_bDisableBHModule;
	}
	if (szFuncName == gszDISABLE_BT_MODULE)
	{
		return m_bDisableBTModule;
	}
	if (szFuncName == gszDISABLE_BL_MODULE)
	{
		return m_bDisableBLModule;
	}
	if (szFuncName == gszDISABLE_NL_MODULE)				//andrewng //2020-0707
	{
		return m_bDisableNLModule;	
	}
	if (szFuncName == gszDISABLE_WT_MODULE)
	{
		return m_bDisableWTModule;
	}
	if (szFuncName == gszDISABLE_WL_MODULE)
	{
		return m_bDisableWLModule;
	}

	if (szFuncName == gszDISABLE_WL_MODULE_WITH_EXP)
	{
		return m_bDisableWLModuleWithExp;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_PICK_DEFECT_DIE)
	{
		return m_bToPickDefect;
	}

	if (szFuncName == MS896A_ENABLE_AUTO_TYPE_FIELD_NAME_CHECK)
	{
		return m_bEnableAutoDieTypeFieldnameChek;
	}

	if (szFuncName == MS896A_PKG_FILE_MAP_HEADER_CHECK_FUNC)
	{
		return m_bPackageFileMapHeaderCheckFunc;
	}

	if (szFuncName == MS896A_CLEAR_DIE_TYPE_FIELD_AFTER_CLEAR_ALL_BIN)
	{
		return m_bClearDieTypeFieldDuringClearAllBin;
	}
	// for 2nd output file path
	if (szFuncName == MS896A_FUNC_OUTPUT_FILE_PATH2)
	{
		return m_bEnableBinOutputFilePath2;
	}	// for 2nd output file path

	if (szFuncName == MS896A_FUNC_SCAN_DIE_PITCH_CHECK)
	{
		return m_bPrescanDiePitchCheck;
	}

	if (szFuncName == MS896A_FUNC_ASSOC_FILE_FUNC)
	{
		return m_bEnableAssocFile;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_RENAME_AFTER_END)
	{
		return m_bRenameMapNameAfterEnd;
	}

	if (szFuncName == MS896A_FUNC_ALWAYS_LOAD_MAP_LOCAL_HD)
	{
		return m_bAlwaysLoadMapFileLocalHarddisk;
	}
    
	if (szFuncName == MS896A_FUNC_BIN_FRAME_CROSS)
	{
		return m_bBinFrameHaveRefCross;
	}

	if (szFuncName == MS896A_FUNC_BPRERROR_NOCLEANCOLLET)
	{
		return m_bBPRErrorCleanCollet;
	}

	if (szFuncName == MS896A_FUNC_ENABLE_COPY_TEMP_FILE_IN_CLEAR_BIN)
	{
		return m_bEnableClearBinCopyTempFile;
	}

	if (szFuncName == MS896A_AUTO_TRANSFER_OUTPUT_FILE)
	{
		return m_bAutoUploadOutputFile;
	}	//	for auto upload output files

	if (szFuncName == MS896A_FUNC_ENABLE_BONDHEAD_TOLERANCE_LEVEL)
	{
		return m_bEnableBondHeadTolerance;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_ENABLE_LOG_COLLET_HOLE_DATA)
	{
		return m_bEnableLogColletHoleData;
	}

	return FALSE;
}


ULONG CMS896AApp::GetFeatureValue(CString szFuncName)
{
	if (szFuncName == MS896A_FUNC_SORTING_PATH_CAL_TIME)
	{
		return (ULONG)m_lWaferMapSortingpathCalTime;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_DELAY_BEFORE_TURN_OFF_EJ_VAC)
	{
		return (ULONG)m_lDelayBeforeTurnOffEJVac;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_CAMERA_BLOCK_DELAY)
	{
		return (ULONG)m_lCameraBlockDelay;
	}

	if (szFuncName == MS896A_FUNC_GENERAL_RT_UPDTAE_MISSING_DIE_THRESHOLD)
	{
		return m_lRTUpdateMissingDieThreshold;
	}

	if (szFuncName == MS896A_FUNC_BIN_SNR_DEFAULT_FORMAT)
	{
		return m_ulDefaultBinSNRFormat;
	}

	if (szFuncName == MS896A_FUNC_CLEAR_COUNT_DEFAULT_FORMAT)
	{
		return m_ulDefaultClearCountFormat;
	}

	if (szFuncName == MS896A_FUNC_WAFERMAP_BLOCK_MODE)		//v3.28T1
	{
		return (ULONG) m_lBlockPickMode;
	}

	if (szFuncName == MS896A_FUNC_IGNORE_GRADE)
	{
		return m_ulIgnoreGrade;
	}

	if (szFuncName == MS896A_FUNC_DETECT_SKIP_MODE)
	{
		return (ULONG)m_lSetDetectSkipMode;
	}

	if (szFuncName == MS896A_PLLM_PRODUCT)		//xyz
	{
		return (ULONG) m_ulPLLMProduct;		//0=PLLM_REBEL, 1=PLLM_FLASH, 2=PLLM_DIEFAB
	}

	return 0;
}

CString CMS896AApp::GetFeatureStringValue(CString szFuncName)
{
	if (szFuncName == MS896A_DIE_TYPE_FIELD_NAME_IN_MAP)
	{
		return m_szDieTypeFieldnameInMap;
	}
	else if (szFuncName == MS896A_PKG_FILE_CHECK_MAP_HEADER_STRING)
	{
		return m_szPackageFileMapHeaderCheckString;
	}
	else if (szFuncName == MS896A_FUNC_ASSOC_FILE_EXT)
	{
		return m_szAssocFileExt;
	}
	else if (szFuncName == MS896A_FUNC_BINBLK_RANKID_NAME_IN_MAP)
	{
		return m_szRankIDFileNameInMap;
	}
	else if (szFuncName == MS896A_FUNC_BINBLK_RANKID_FILE_EXT)
	{
		return m_szRankIDFileExt;
	}

	for (INT i = 0; i < MS896A_DIE_TYPE_CHECK_LIMIT; i++)
	{
		CString szTemp;
		//szTemp.Format("Die Type Check Fieldname%d", i+1);
		szTemp = MS896A_FUNC_DIE_TYPE_CHECK_FIELDNAME;
		szTemp.AppendFormat("%d", i + 1);

		if (szFuncName == szTemp)
		{
			return m_szMapDieTypeCheckString[i];
		}
	}

	return "";
}

LONG CMS896AApp::GetBHZ1HomeOffset()
{
	return m_lBHZ1HomeOffset;
}

LONG CMS896AApp::GetBHZ2HomeOffset()
{
	return m_lBHZ2HomeOffset;
}

VOID CMS896AApp::SetBHZ1HomeOffset(LONG lOffsetZ)		//v4.46T26
{
	m_lBHZ1HomeOffset = lOffsetZ;
	WriteProfileInt(gszPROFILE_SETTING, gszBHZ1_HOME_OFFSET, m_lBHZ1HomeOffset);
}

VOID CMS896AApp::SetBHZ2HomeOffset(LONG lOffsetZ)
{
	m_lBHZ2HomeOffset = lOffsetZ;
	WriteProfileInt(gszPROFILE_SETTING, gszBHZ2_HOME_OFFSET, m_lBHZ2HomeOffset);
}

BOOL CMS896AApp::CheckPLLMRebelPassword(CString szPassword)	//v3.70T3
{
	if ((m_szPLLMRebelPassword != _T("")) && (m_szPLLMRebelPassword == szPassword))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CMS896AApp::CheckSanAnProductionMode()
{
	if( (GetCustomerName() == CTM_SANAN) || 
		(GetCustomerName()==CTM_SEMITEK && GetProductLine()=="ZJG")	||
		(GetCustomerName() == "Electech3E(DL)")  || GetCustomerName() == "ChangeLight(XM)" )	//v4.50A15 // <- Andrew: add this line
	{
		if (!m_bOperator)
		{
			return FALSE;    //Only OPERATOR can run AUTOBOND contiuously in SanAn	//v4.33T1
		}
		return TRUE;
	}
	else
	{
//		if (!m_bOperator)
//		{
//			return FALSE;    //Only OPERATOR can run AUTOBOND contiuously
//		}
	}
	return TRUE;
}

VOID CMS896AApp::SaveLoadMapDate(CString szLoadMapDate)
{
	m_smfSRam["WaferTable"]["Load Map Date"] = szLoadMapDate;

	WriteProfileString(gszPROFILE_SETTING, gszLOADMAP_DATE_SETTING, szLoadMapDate);	
} 

VOID CMS896AApp::SaveLoadMapTime(CString szLoadMapTime)
{
	m_smfSRam["WaferTable"]["Load Map Time"] = szLoadMapTime;

	WriteProfileString(gszPROFILE_SETTING, gszLOADMAP_TIME_SETTING, szLoadMapTime);
}

VOID CMS896AApp::SaveMapStartDateTime()
{
	int nYear, nMonth, nDay, nHour, nMinute, nSecond; 
	CString szHour, szMinute, szSecond;
	CString szDay, szMonth;
	CTime theTime;

	//Get Wafer End Date and Time
	theTime = CTime::GetCurrentTime();

	//Save Date
	nYear = theTime.GetYear();

	nMonth = theTime.GetMonth();
	szMonth.Format("%d", nMonth);
	if (nMonth < 10)
	{
		szMonth = "0" + szMonth;
	}

	nDay = theTime.GetDay();
	szDay.Format("%d", nDay);
	if (nDay < 10)
	{
		szDay = "0" + szDay;
	}

	m_szMapStartDate.Format("%d", nYear);
	m_szMapStartDate = m_szMapStartDate + "-" + szMonth + "-" + szDay;

	//Save Time
	nHour = theTime.GetHour();
	szHour.Format("%d", nHour);
	if (nHour < 10)
	{
		szHour = "0" + szHour;
	}

	nMinute = theTime.GetMinute();
	szMinute.Format("%d", nMinute);
	if (nMinute < 10)
	{
		szMinute = "0" + szMinute;
	}

	nSecond = theTime.GetSecond();
	szSecond.Format("%d", nSecond);
	if (nSecond < 10)
	{
		szSecond = "0" + szSecond;
	}

	m_szMapStartTime = szHour + ":" + szMinute + ":" + szSecond;


	//Update into SRAM & Registry
	m_nMapRunTime		= 0;
	m_nMapStartYear		= nYear;
	m_nMapStartMonth	= nMonth;
	m_nMapStartDay		= nDay;
	m_nMapStartHour		= nHour;
	m_nMapStartMinute	= nMinute;
	m_nMapStartSecond	= nSecond;

	m_smfSRam["WaferTable"]["Map Start Date"]	= m_szMapStartDate;
	m_smfSRam["WaferTable"]["Map Start Time"]	= m_szMapStartTime;
	m_smfSRam["WaferTable"]["Map Run Time"]		= m_nMapRunTime;

	WriteProfileString(gszPROFILE_SETTING, gszMAPSTART_DATE_SETTING, m_szMapStartDate);	
	WriteProfileString(gszPROFILE_SETTING, gszMAPSTART_TIME_SETTING, m_szMapStartTime);	
	WriteProfileInt(gszPROFILE_SETTING, gszMAPRUN_TIME_SETTING, m_nMapRunTime);
}

VOID CMS896AApp::MapIsStarted()
{
	if (m_bNewMapLoaded == TRUE)
	{
		m_bNewMapLoaded = FALSE;
		SaveMapStartDateTime();
		return;
	}

	CTime CurrentTime;

	//Get current Date and Time
	CurrentTime			= CTime::GetCurrentTime();
	m_nMapStartYear		= CurrentTime.GetYear();
	m_nMapStartMonth	= CurrentTime.GetMonth();
	m_nMapStartDay		= CurrentTime.GetDay();
	m_nMapStartHour		= CurrentTime.GetHour();
	m_nMapStartMinute	= CurrentTime.GetMinute();
	m_nMapStartSecond	= CurrentTime.GetSecond();
}


VOID CMS896AApp::MapIsStopped()
{
	CTime CurrentTime;
	CTime StartTime, EndTime;
	CTimeSpan TimeDiff;
	int nYear, nMonth, nDay, nHour, nMinute, nSecond; 

	//Get current Date and Time
	CurrentTime	= CTime::GetCurrentTime();
	nYear		= CurrentTime.GetYear();
	nMonth		= CurrentTime.GetMonth();
	nDay		= CurrentTime.GetDay();
	nHour		= CurrentTime.GetHour();
	nMinute		= CurrentTime.GetMinute();
	nSecond		= CurrentTime.GetSecond();


	//Calcualte time different for map is run
	StartTime	= CTime::CTime(m_nMapStartYear, m_nMapStartMonth, m_nMapStartDay, 
							   m_nMapStartHour, m_nMapStartMinute, m_nMapStartSecond, 0);
	
	EndTime		= CTime::CTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, 0);

	TimeDiff = EndTime - StartTime;

	//Update into SRAM & Registry
	m_nMapRunTime += (int)TimeDiff.GetTotalSeconds();
	m_smfSRam["WaferTable"]["Map Run Time"] = m_nMapRunTime;
	WriteProfileInt(gszPROFILE_SETTING, gszMAPRUN_TIME_SETTING, m_nMapRunTime);
}


LONG CMS896AApp::LoadMapHeaderInfo()
{
	return MS_LoadLastState();
}


LONG CMS896AApp::SaveAppData()
{
	CStringMapFile pFile;

	if (pFile.Open(MSD_APP_DATA_FILE, FALSE, TRUE) == 1)
	{
		(pFile)["AppData"]["Machine Time"]["Rpt Method"]		= m_ucEqTimeReportMode;
		(pFile)["AppData"]["Machine Time"]["Auto Reset"]		= m_bEqTimeAutoReset;
		(pFile)["AppData"]["Package File"]["Load BT SMF"]		= m_bLoadPkgWithBin;
		(pFile)["AppData"]["Package File"]["Exclude Machine Param"]	= m_bExcludeMachineParam;
		(pFile)["AppData"]["Package File"]["Only Bin Param"]	= m_bOnlyLoadBinParam;
		(pFile)["AppData"]["Package File"]["Only WFT Param"]	= m_bOnlyLoadWFTParam;
		(pFile)["AppData"]["Package File"]["Only PR Param"]		= m_bOnlyLoadPRParam;
		(pFile)["AppData"]["Package File"]["Portable PKG File"]	= m_bPortablePKGFile;
		(pFile)["AppData"]["Package File"]["Reset Bin SNR"]		= m_bResetBinSerialNo;
		(pFile)["AppData"]["Package File"]["File List Path"]	= m_szPkgFileListPath;				//v4.21T7
		(pFile)["AppData"]["Package File"]["Ami File Path"]		= m_szAmiFilePath;
		(pFile)["AppData"]["Package File"]["Ami Pitch Update Block"]		= m_bAmiPitchUpdateBlock;
		(pFile)["AppData"]["Package File"]["Runtime 2 PKG in use"]		= m_bRuntimeTwoPkgInUse;

		(pFile)["AppData"]["Ami File"]["Ami File Status"]		= m_bLoadAmiFileStatus;
		(pFile)["AppData"]["Ami File"]["Change Max Count With Ami"]		= m_bChangeMaxCountWithAmi;
		(pFile)["AppData"]["PPKG File"]["Include Sort Delay"]	= m_bPpkgIncludeDelay;
		(pFile)["AppData"]["PPKG File"]["Include Scan Setting"]	= m_bPpkgIncludePrescanSettings;
		(pFile)["AppData"]["PPKG File"]["Include Region"]		= m_bPpkgIncludeRegionSettings;
		(pFile)["AppData"]["PPKG File"]["Include Pick setting"]	= m_bPpkgIncludePickSettingWODelay;
		
		(pFile)["AppData"]["Others"]["Start Lot"]					= m_bStartNewLot;		//v2.71
		(pFile)["AppData"]["Others"]["UseUplookPR"]					= m_bUseBHUplookPrFcn;	//v4.58A3	//MS90
		(pFile)["AppData"]["Others"]["EnableSubBin"]				= m_bEnableSubBin;
		//(pFile)["AppData"]["Others"]["NG No Bin Map"]				= m_bNGNoBinMap;
		(pFile)["AppData"]["Machine Time"]["Auto Reset Die Counts"]	= m_bEqTimeAutoResetDieCounts;	//v2.83T2
		(pFile)["AppData"]["Machine Time"]["No Of Report Period"]	= (LONG) m_lNoOfReportPeriod;

		for (INT i = 0; i < EQUIP_NO_OF_RECORD_TIME; i++)
		{
			(pFile)["AppData"]["Machine Time"]["Report Start Time"][i]	= m_szReportStartTime[i];
			(pFile)["AppData"]["Machine Time"]["Report End Time"][i]	= m_szReportEndTime[i];
		}

		(pFile)["AppData"]["Machine Time"]["Report Path"] = m_szMachineTimeReportPath;
		UINT unEnable = GetProfileInt(gszPROFILE_SETTING, _T("Equip Machine Time 2"), 0);
		if( unEnable==0 )
			m_szMachineTimeReportPath2 = m_szMachineTimeReportPath;
		(pFile)["AppData"]["Machine Time"]["Report Path2"] = m_szMachineTimeReportPath2;
		(pFile)["AppData"]["Machine Time"]["Performance Time"]	= m_szTimePerformancePath;
		(pFile)["AppData"]["Machine Time"]["Machine Serial No"]	= m_szMachineSerialNo;

		(pFile)["AppData"]["Machine"]["Report Path"] = m_szMachineReportPath;
		(pFile)["AppData"]["Machine"]["Report Path2"] = m_szMachineReportPath2;
		(pFile)["AppData"]["Machine"]["AlarmReport Path"] = m_szAlarmLampStatusReportPath;

		(pFile)["AppData"]["Bin Storage File"]["Path"] = m_szBinStoragePath;
		(pFile)["AppData"]["Bin Storage File"]["Filename"] = m_szBinStorageFilename;

		(pFile)["AppData"]["RecoverPath"]["FilePath"] = m_szRecoverGeneratedFilePath;  // v4.51D2 //Electech3E(DL) 1

		pFile.Update();
		pFile.Close();
		m_smfSRam["MS896A"]["AmiFile"]["ChangeMaxCountWithAmi"] = m_bChangeMaxCountWithAmi;
		m_smfSRam["MS896A"]["Include Sort Delay"]	= m_bPpkgIncludeDelay;
		m_smfSRam["MS896A"]["Include Scan Setting"]	= m_bPpkgIncludePrescanSettings;
		m_smfSRam["MS896A"]["Include Region"]		= m_bPpkgIncludeRegionSettings;
		m_smfSRam["MS896A"]["Include Pick Setting"]	= m_bPpkgIncludePickSettingWODelay;
		
		//Update SRAM data
		m_smfSRam["MS896A"]["Reset Bin Serial No"]		= m_bResetBinSerialNo;
		m_smfSRam["MS896A"]["Exclude Machine Param"]	= m_bExcludeMachineParam;	//v4.33T1	//PLSG
		m_smfSRam["MS896A"]["Machine"]["Report Path"]	= m_szMachineTimeReportPath;
		m_smfSRam["MS896A"]["Machine"]["AlarmReport Path"] = m_szAlarmLampStatusReportPath;

		return TRUE;
	}

	//Check Load/Save Data
	CMSFileUtility::Instance()->LogFileError("Cannot Save File: " + MSD_APP_DATA_FILE);

	return FALSE;
}


LONG CMS896AApp::LoadAppData()
{
	CStringMapFile pFile;

	if (pFile.Open(MSD_APP_DATA_FILE, FALSE, TRUE) == 1)
	{
		CString szReportPath;

		m_ucEqTimeReportMode	= (UCHAR)(LONG)(pFile)["AppData"]["Machine Time"]["Rpt Method"];
		m_bEqTimeAutoReset		= (BOOL)(LONG)(pFile)["AppData"]["Machine Time"]["Auto Reset"];
		m_bStartNewLot			= (BOOL)(LONG)(pFile)["AppData"]["Others"]["Start Lot"];	//v2.71
		m_bUseBHUplookPrFcn		= (BOOL)(LONG)(pFile)["AppData"]["Others"]["UseUplookPR"];	//v4.58A3	//MS90
		m_bEqTimeAutoResetDieCounts	= (BOOL)(LONG)(pFile)["AppData"]["Machine Time"]["Auto Reset Die Counts"];	//v2.83T2
		m_bEnableSubBin			= (BOOL)(LONG)(pFile)["AppData"]["Others"]["EnableSubBin"];
		//m_bNGNoBinMap			= (BOOL)(LONG)(pFile)["AppData"]["Others"]["NG No Bin Mapn"];
		if ((!((pFile)["AppData"]["Package File"]["Load BT SMF"])) == FALSE)
		{
			m_bLoadPkgWithBin	= (BOOL)(LONG)(pFile)["AppData"]["Package File"]["Load BT SMF"];
		}
		
		m_bExcludeMachineParam	= (BOOL)(LONG)(pFile)["AppData"]["Package File"]["Exclude Machine Param"];
		m_bOnlyLoadBinParam		= (BOOL)(LONG)(pFile)["AppData"]["Package File"]["Only Bin Param"];
		m_bOnlyLoadPRParam		= (BOOL)(LONG)(pFile)["AppData"]["Package File"]["Only PR Param"];
		m_bOnlyLoadWFTParam		= (BOOL)(LONG)(pFile)["AppData"]["Package File"]["Only WFT Param"];
		m_bPortablePKGFile		= (BOOL)(LONG)(pFile)["AppData"]["Package File"]["Portable PKG File"];
		m_szPkgFileListPath		= (pFile)["AppData"]["Package File"]["File List Path"];				//v4.21T7
		m_szAmiFilePath			= (pFile)["AppData"]["Package File"]["Ami File Path"];
		m_bAmiPitchUpdateBlock	= (BOOL)(LONG)(pFile)["AppData"]["Package File"]["Ami Pitch Update Block"];
		m_bLoadAmiFileStatus	= (BOOL)(LONG)(pFile)["AppData"]["Ami File"]["Ami File Status"];
		m_bRuntimeTwoPkgInUse	= (BOOL)(LONG)(pFile)["AppData"]["Package File"]["Runtime 2 PKG in use"];
		m_bChangeMaxCountWithAmi = (BOOL)(LONG)(pFile)["AppData"]["Ami File"]["Change Max Count With Ami"];

		if ((!((pFile)["AppData"]["Package File"]["Reset Bin SNR"])) == FALSE)
		{
			m_bResetBinSerialNo	= (BOOL)(LONG)(pFile)["AppData"]["Package File"]["Reset Bin SNR"];
		}

		m_lNoOfReportPeriod = (LONG)(pFile)["AppData"]["Machine Time"]["No Of Report Period"]; 

		for (INT i = 0; i < EQUIP_NO_OF_RECORD_TIME; i++)
		{
			m_szReportStartTime[i] = (pFile)["AppData"]["Machine Time"]["Report Start Time"][i];
			m_szReportEndTime[i] = (pFile)["AppData"]["Machine Time"]["Report End Time"][i];

		}
		
		szReportPath = (pFile)["AppData"]["Machine Time"]["Report Path"];
	
		if (szReportPath.IsEmpty() == FALSE)
		{
			m_szMachineTimeReportPath = szReportPath;
		}

		UINT unEnable = GetProfileInt(gszPROFILE_SETTING, _T("Equip Machine Time 2"), 0);
		szReportPath = (pFile)["AppData"]["Machine Time"]["Report Path2"];
		if (szReportPath.IsEmpty() == FALSE)
		{
			m_szMachineTimeReportPath2 = szReportPath;
		}
		else if( unEnable==0 )
		{
			m_szMachineTimeReportPath2 = m_szMachineTimeReportPath;
		}
		m_szTimePerformancePath		= (pFile)["AppData"]["Machine Time"]["Performance Time"];
		m_szMachineSerialNo			= (pFile)["AppData"]["Machine Time"]["Machine Serial No"];

		m_szMachineReportPath	= (pFile)["AppData"]["Machine"]["Report Path"];
		m_szMachineReportPath2	= (pFile)["AppData"]["Machine"]["Report Path2"];

		m_szAlarmLampStatusReportPath = (pFile)["AppData"]["Machine"]["AlarmReport Path"];

		m_szBinStoragePath		= (pFile)["AppData"]["Bin Storage File"]["Path"];
		m_szBinStorageFilename	= (pFile)["AppData"]["Bin Storage File"]["Filename"];

		m_bPpkgIncludeDelay				= (BOOL)(LONG)(pFile)["AppData"]["PPKG File"]["Include Sort Delay"];
		m_bPpkgIncludePrescanSettings	= (BOOL)(LONG)(pFile)["AppData"]["PPKG File"]["Include Scan Setting"];
		m_bPpkgIncludeRegionSettings	= (BOOL)(LONG)(pFile)["AppData"]["PPKG File"]["Include Region"];
		m_bPpkgIncludePickSettingWODelay= (BOOL)(LONG)(pFile)["AppData"]["PPKG File"]["Include Pick setting"];

		m_szRecoverGeneratedFilePath	= (pFile)["AppData"]["RecoverPath"]["FilePath"];  // v4.51D2 //Electech3E(DL) 2

		pFile.Close();
		
		m_smfSRam["MS896A"]["Include Sort Delay"]	= m_bPpkgIncludeDelay;
		m_smfSRam["MS896A"]["Include Scan Setting"]	= m_bPpkgIncludePrescanSettings;
		m_smfSRam["MS896A"]["Include Region"]		= m_bPpkgIncludeRegionSettings;
		m_smfSRam["MS896A"]["Include Pick Setting"]	= m_bPpkgIncludePickSettingWODelay;

		//Update SRAM data
		m_smfSRam["MS896A"]["Reset Bin Serial No"]		= m_bResetBinSerialNo;
		m_smfSRam["MS896A"]["Exclude Machine Param"]	= m_bExcludeMachineParam;	//v4.33T1	//PLSG
		m_smfSRam["MS896A"]["Machine"]["Report Path"]	= m_szMachineTimeReportPath;

		CMSLogFileUtility::Instance()->MS_LogOperation("AppData MSD loaded");	//v4.46T26
		return TRUE;
	}

	//Check Load/Save Data
	CMSFileUtility::Instance()->LogFileError("Cannot Open File: " + MSD_APP_DATA_FILE);
	return FALSE;
}

BOOL CMS896AApp::UpdateMapHeaderInfo(VOID)
{
	CString szCurrentSpecVersion = "";
	CString szBinParameter = "";
	CString szTemp = "";
	CString szMapHeaderTemp = "";
	CString szType = "";
	CString szSortBinItem = "";
	CString szProduct = "";
	CString szMapOther1 = "";
	CString szMapOther2 = "";
	CString szMapOther3 = "";
	CString szMapOther4 = "";
	CString szMapOther5 = "";
	CString szMapOther6 = "";
	CString szSortSetupFile = "";
	CString szSortBinFileName = "";
	CString szLotNumber = "";
	CString szBinTable = "";
	CString szProberMachineNo = "";
	CString szProberDateTime = "";
	CString szBinTableSerialNo = "";
	CString szMapDate = "";
	CString szInternalProductNo = "";	
	CString szMode = "";				
	CString szSpecification = "";		
	CString szOther = "";
	CString szMapBinTable = "";
	CString szMapSerialNumber = "";
	CString szMapESDVoltage = "";
	CString szTestTime = "";
	CString szWO = "";
	CString szSubstarteID = "";
	CString szCSTID = "";
	CString szRecipeID = "";
	CString szRecipeName = "";
	CString szOperator = "";
	CString szRemark1 = "";
	CString szRemark2 = "";
	CString szPartNo = "";
	CString szDN = "";
	CString szWN = "";
	CString szCM1 = "";
	CString szCM2 = "";
	CString szCM3 = "";

	CString szUN	= "";
	CString szCOND	= "";
	CString szLO	= "";
	CString szUP	= "";
	CString szFT	= "";
	CString szLIM	= "";
	CString szCOEF1 = "";
	CString szCOEF2 = "";
	CString szCOEF3 = "";

	CString szRemark3 = "";

	CString szMapEPID = "";
	CString szMapResortingBin = "";

	CString szControlRank, szRankNo, szAt;

	CString szWafType, szBinName, szU1, szU2, szU3;		//v4.13T1	//APlusEpi
	CString szHoops;		//v4.30T3	//Cyoptics

	CString szCenterPoint = "", szLifeTime = "", szESD = "";
	CString szPadColor = "", szBinCode = "";
	CString szCategory = "";
	// SeoulSemi Output //4.53D16
	CString szHeaderProject;
	CString szHeaderProductCode;

	CString szHeaderDesign = ""; // 4.53D5

	const CMapStringToString *szMapHeaderInfo;
	szMapHeaderInfo = CMS896AStn::m_WaferMapWrapper.GetHeaderInfo();
	Sleep(500); // ensure can load map

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	if (pUtl->LoadLastState() == FALSE)
	{
		return FALSE;
	}

	psmf = pUtl->GetLastStateFile();		

	if (szMapHeaderInfo != NULL)
	{
		//Get Spec Version
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_SPEC, szCurrentSpecVersion);

		if (szCurrentSpecVersion != m_szBinSpecVersion)
		{
			szTemp = "Spec Version changed!\n";
			szTemp = szTemp + "Old: " + m_szBinSpecVersion + "\n" ;
			szTemp = szTemp + "Now: " + szCurrentSpecVersion;
			AfxMessageBox(szTemp, MB_ICONSTOP | MB_SYSTEMMODAL);
			szTemp.Empty();
		}

		m_szBinSpecVersion = szCurrentSpecVersion; 

		//Get Bin Parameter
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_BINPARA, szTemp);
		szBinParameter = szTemp.TrimLeft(",");
		szBinParameter = szTemp.TrimRight(",");

		//Get Wafermap "Type" & "SortBinItem" (For HPO)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_TYPE, szType);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_SORTBIN, szSortBinItem);

		//Get Wafermap "Product" (For Unitex)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_PRODUCT, szProduct);

		if (szProduct.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_PRODUCTNAME, szProduct);
		}

		// Get Wafermap "Prober Machine no" (For )
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_PROBER_MACHINE_NO, szProberMachineNo);

		if (szProberMachineNo.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_PROBER_MACHINE_NO_2, szProberMachineNo);
		}

		// Get Wafermap "Prober Date Time" (For )
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_PROBER_DATE_TIME, szProberDateTime);
		
		if (szProberDateTime.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_PROBER_TEST_TIME, szProberDateTime);

			if (szProberDateTime.IsEmpty() == TRUE)
			{
				szMapHeaderInfo->Lookup(WT_MAP_HEADER_PROBER_TEST_TIME_2, szProberDateTime);
			}
		}	

		// Get Wafermap "Bin Table Serial No." (For Ubilux)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_BIN_TABLE_SERIAL_NO, szBinTableSerialNo);

		if (szBinTableSerialNo.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_BIN_TABLE_NO, szBinTableSerialNo);
		}
		
		if (szBinTableSerialNo.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_BIN_TABLE_SERIAL_NO_2, szBinTableSerialNo);
		}

		if (szBinTableSerialNo.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_BIN_TABLE_SERIAL_NO_3, szBinTableSerialNo);
		}

		// Get Wafermap "Date"
		szMapHeaderInfo->Lookup(WT_DATE_IN_MAP_FILE, szMapDate);
		
		// Get wafermap "Other1, Other2, ..... Other10 for testar"
		for (INT i = 1; i <= 10; i++)
		{	
			szMapHeaderTemp = "";
			szTemp.Format("Other%d", i);
			szMapHeaderInfo->Lookup(szTemp, szMapHeaderTemp);
			
			if (i == 1)
			{
				szOther = szMapHeaderTemp;
			}
			else
			{
				szOther = szOther + "," + szMapHeaderTemp;
			}
		}
		
		//Get Wafermap "internal product no" (For Ubilux)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_INTERNAL_PRODUCT_NO, szInternalProductNo);	
		
		//Get Wafermap "mode" (For Ubilux)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_MODE, szMode);	

		if (szMode.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_MODE2, szMode);	
		}
		//Get Wafermap "Specification" (For Ubilux)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_SPECIFICATION, szSpecification);	
		if (szSpecification.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_SPECIFICATION_2, szSpecification);	
		}

		// Get Wafermap "Bin Table" (For Walsin)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_BINTABLE, szMapBinTable);

		if (szMapBinTable.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_BINTABLE_2, szMapBinTable);
		}
		
		if (szMapBinTable.IsEmpty() == TRUE)
		{
			if (GetCustomerName() == "ChangeLight(XM)")
			{
				szMapHeaderInfo->Lookup("BINTABLEName", szMapBinTable);
			}
			else
			{
				szMapHeaderInfo->Lookup(WT_MAP_HEADER_BINTABLENAME, szMapBinTable);
			}
		}

		// Get Wafermap "Remark 1" (LiteStar)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_REMARK1, szRemark1);
		if (szRemark1.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_REMARK1_2, szRemark1);
		}

		// Get Wafermap "Remark 2" (LiteStar)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_REMARK2, szRemark2);
		if (szRemark2.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_REMARK2_2, szRemark2);
		}

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_TEST_TIME, szTestTime);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_WO, szWO);
		
		if (szWO.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_WO_2, szWO);
		}
		
		if (szWO.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_WO_3, szWO);
		}

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_SUBSTRATE_ID, szSubstarteID);
		
		if (szSubstarteID.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_SUBSTRATE_ID_2, szSubstarteID);
		}

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_CST_ID, szCSTID);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_RECIPE_ID, szRecipeID);
		if (szRecipeID.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_RECIPE_ID_2, szRecipeName);
		}

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_RECIPE_NAME, szRecipeName);
		if (szRecipeName.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_RECIPE_NAME_2, szRecipeName);
		}
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_OPERATOR, szOperator);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_PART_NO, szPartNo);
		szMapHeaderInfo->Lookup(WT_MAP_SERIAL_NUMBER, szMapSerialNumber);


		//Get Wafermap "DN" (Product Name) (for Sharp)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_DN, szDN);
		//Get Wafermap "WN" (Wafer No.) (for Sharp)
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_WN, szWN);
		//Get Wafermap "CM" (Comment) for Sharp
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_CM1, szCM1);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_CM2, szCM2);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_CM3, szCM3);

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_REMARK3, szRemark3);
		
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_UN,		szUN);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_COND,		szCOND);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_LO,		szLO);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_UP,		szUP);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_FT,		szFT);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_LIM,		szLIM);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_COEF1,	szCOEF1);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_COEF2,	szCOEF2);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_COEF3,	szCOEF3);
		
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_EPIID, szMapEPID);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_RESORTINGBIN, szMapResortingBin);

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_CENTERPOINT,	szCenterPoint);
		//AfxMessageBox("Center Point," + szCenterPoint, MB_SYSTEMMODAL);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_LIFETIME,		szLifeTime);
		//AfxMessageBox("LIfe Time," + szLifeTime, MB_SYSTEMMODAL);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_ESD,			szESD);
		//AfxMessageBox("ESD," + szESD, MB_SYSTEMMODAL);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_PAD_COLOR,	szPadColor);
		//AfxMessageBox("PAD Color," + szPadColor, MB_SYSTEMMODAL);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_BIN_CODE,		szBinCode);
		if (szBinCode.GetLength() == 0)
		{
			//v4.40T14	//AoYang
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_AOYANG_BIN_GRADE_NAME,	szBinCode);
		}

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_CONTROL_RANK, szControlRank);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_RANK_NO, szRankNo);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_AT,	 szAt);

		//v4.13T1	//DengCheng -> APlusEpi
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_WAFTYPE,		szWafType);
		
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_BINNAME,		szBinName);
		if (szBinName.IsEmpty())
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_BINNAME_2,	szBinName);
		}

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_U1,			szU1);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_U2,			szU2);
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_U3,			szU3);

		if (GetCustomerName() == "CyOptics")		//v4.30T3
		//		|| GetCustomerName() == "Inari")		//v4.51A24	
		{
			CString szOldHoops = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CYOPTICS_HOOPS];
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_CYOPTICS_HOOPS,	szHoops);

			//CString szTemp;
			//szTemp = "CyOptics new Hoops = " + szHoops + "  (" + szOldHoops + ")";
			//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
			if ((szOldHoops.GetLength() > 0) && (szHoops.GetLength() > 0) && (szHoops != szOldHoops))
			{
				m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CYOPTICS_HOOPS] = TRUE;
			}
			else
			{
				m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CYOPTICS_HOOPS] = FALSE;
			}

			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CYOPTICS_HOOPS]	= szHoops;
		}
		
		if (GetCustomerName() == "DeLi")		
		{
			CString szBinGradeDL;
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_DELI_BINGRADE,szBinGradeDL);
			m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DELI_BINGRADE]		= szBinGradeDL;
			//AfxMessageBox(szBinGradeDL);
			CString szVF1;
			szMapHeaderInfo->Lookup("VF1",szVF1);
			m_smfSRam["MS896A"][WT_MAP_HEADER]["VF1"]		= szVF1;
			//AfxMessageBox(szVF1);

		}
//AfxMessageBox(GetCustomerName());
		if (GetCustomerName() == "Solidlite")
		{
			CString szItem1,szItem2,szItem3;
			szMapHeaderInfo->Lookup("P/N",szItem1);
			szMapHeaderInfo->Lookup("Lot #",szItem2);
			szMapHeaderInfo->Lookup("QA",szItem3);
			m_smfSRam["MS896A"][WT_MAP_HEADER]["SOLIDLITEPN"]		= szItem1;
			m_smfSRam["MS896A"][WT_MAP_HEADER]["SOLIDLITELOT"]		= szItem2;
			m_smfSRam["MS896A"][WT_MAP_HEADER]["SOLIDLITEQA"]		= szItem3;
		//AfxMessageBox(szItem1 + "," + szItem2 + "," + szItem3);
		}

		if (GetCustomerName() == "TianXin" || GetCustomerName() == "BabyLighting")    // v4.51D3 // TianXin
		{
			CString szTianXinItem1;
			CString szTianXinItem2;
			CString szTianXinItem3;
			CString szTianXinItem4;
			CString szTianXinItem5;
			CString szTianXinItem6;
			CString szTianXinItem7;
			CString szTianXinItem8;

			szMapHeaderInfo->Lookup("P/N",				szTianXinItem1);
			szMapHeaderInfo->Lookup("Lot #",			szTianXinItem2);
			szMapHeaderInfo->Lookup("PCS",				szTianXinItem3);
			szMapHeaderInfo->Lookup("START Date",		szTianXinItem4);
			szMapHeaderInfo->Lookup("SETUP PACKAGE",	szTianXinItem5);
			szMapHeaderInfo->Lookup("PR PACKAGE",		szTianXinItem6);
			szMapHeaderInfo->Lookup("TESTER COND File", szTianXinItem7);
			szMapHeaderInfo->Lookup("Source",			szTianXinItem8);

			m_smfSRam["MS896A"][WT_MAP_HEADER]["TIANXIN_PN"]					= szTianXinItem1;
			m_smfSRam["MS896A"][WT_MAP_HEADER]["TIANXIN_LOT"]					= szTianXinItem2;
			m_smfSRam["MS896A"][WT_MAP_HEADER]["TIANXIN_PCS"]					= szTianXinItem3;

			m_smfSRam["MS896A"][WT_MAP_HEADER]["TIANXIN_START_DATE"]			= szTianXinItem4;
			m_smfSRam["MS896A"][WT_MAP_HEADER]["TIANXIN_SETUP_PACKAGE"]			= szTianXinItem5;
			m_smfSRam["MS896A"][WT_MAP_HEADER]["TIANXIN_PR_PACKAGE"]			= szTianXinItem6;
			m_smfSRam["MS896A"][WT_MAP_HEADER]["TIANXIN_TESTER_COND_FILE"]		= szTianXinItem7;
			m_smfSRam["MS896A"][WT_MAP_HEADER]["TIANXIN_SOURCE"]				= szTianXinItem8;
			//AfxMessageBox("GetCustomerName TianXin:"+ szTianXinItem1 + "," + szTianXinItem2 + "," + szTianXinItem3);
		}

		if (GetCustomerName() == "FiberOptics")    // v4.51D10 // Dicon
		{
			CString szSortBin;
			szMapHeaderInfo->Lookup("Sort Bin",				szSortBin);
			m_smfSRam["MS896A"][WT_MAP_HEADER]["SORT_BIN"]					= szSortBin;
		}

		if (GetCustomerName() == "Macom")   
		{
			CString szOddRowID;
			CString szEvenRowID;
			szMapHeaderInfo->Lookup("OddRowID",					szOddRowID); // 4.52D7
			szMapHeaderInfo->Lookup("EvenRowID",				szEvenRowID);

			CString szMsg;
			szMsg.Format("Update Map Header --- Odd Row Id:%s, Even Row Id:%s",szOddRowID, szEvenRowID );  
			//AfxMessageBox(szMsg);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			if( (szOddRowID == "1" && szEvenRowID == "2" ) || (szOddRowID == "2" || szEvenRowID == "1" ))
			{
				m_smfSRam["MS896A"][WT_MAP_HEADER]["ODD_ROW_ID"]					= szOddRowID;
				m_smfSRam["MS896A"][WT_MAP_HEADER]["EVEN_ROW_ID"]					= szEvenRowID;
				m_smfSRam["MS896A"][WT_MAP_HEADER]["EXIST_ROW_ID"]					= TRUE;
			}
			else
			{
				m_smfSRam["MS896A"][WT_MAP_HEADER]["EXIST_ROW_ID"]					= FALSE;
				//return FALSE;
			}
		}


		//v2.56
		//Sort Setup file for Genesis
		BOOL bSortSetupFile = FALSE;
		CString szOldFile = "";

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_SORTSETUPFILE, szSortSetupFile);
		if (szSortSetupFile != "")
		{
			if (psmf != NULL)
			{
				szOldFile = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTSETUPFILE];
			}

			bSortSetupFile = DisplaySortFileContent(HMB_MS_SORT_SETUP_FILE, szOldFile, szSortSetupFile);
		}

		//Get Wafermap "SortBINFileName" for Epileds
		BOOL bSortBinFile = FALSE;

		CString szOldSortBinFile = "", szNewSortBinFile = "";
		if (psmf != NULL)
		{
			szOldSortBinFile = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTBINFNAME];
		}
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_SORTBINFNAME, szSortBinFileName);
		szNewSortBinFile = szSortBinFileName;

		if( szNewSortBinFile != "" )
		{
			bSortBinFile = DisplaySortFileContent(HMB_MS_SORT_BIN_FILE, szOldSortBinFile, szNewSortBinFile);
		}

		//Get Wafermap "LotNumber" for Full Sun
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_LOTNUMBER, szLotNumber);

		//v4.40T14
		if (GetCustomerName() == "LatticePower" && (szLotNumber.GetLength() > 0))
		{
			szLotNumber = szLotNumber.Mid(1);	//Remove 1st comma
			INT nIndex  = szLotNumber.Find(",");
			if (nIndex != -1)
				szLotNumber = szLotNumber.Left(szLotNumber.Find(","));
		}

		szMapHeaderInfo->Lookup(WT_MAP_HEADER_ESD_VOLTAGE, szMapESDVoltage);

		if (szMapESDVoltage.IsEmpty() == TRUE)
		{
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_ESD_VOLTAGE_2, szMapESDVoltage);
		}

		if (GetCustomerName() == "SeoulSemi")	
		{
			szMapHeaderInfo->Lookup("Project", szHeaderProject);
			szMapHeaderInfo->Lookup("Product Code", szHeaderProductCode);

			//Get All of the RANK Name
			SeoulSemiUpdateAllRankName();
		}
		
		//4.53D5 find Header
		szMapHeaderInfo->Lookup(WT_MAP_HEADER_DESIGN, szHeaderDesign);
		
		if (psmf != NULL)
		{
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SPEC]		= m_szBinSpecVersion;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BINPARA]	= szBinParameter;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_TYPE]		= szType;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTBIN]	= szSortBinItem;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PRODUCT]	= szProduct;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LOTNUMBER] = szLotNumber;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_ESD_VOLTAGE] = szMapESDVoltage;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PROBER_MACHINE_NO] = szProberMachineNo;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PROBER_DATE_TIME] = szProberDateTime;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BIN_TABLE_SERIAL_NO] = szBinTableSerialNo;
			(*psmf)[WT_MAP_HEADER_INFO][WT_DATE_IN_MAP_FILE] = szMapDate;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_MAP_OTHER] = szOther;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_INTERNAL_PRODUCT_NO] = szInternalProductNo;	
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_MODE] = szMode;								
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SPECIFICATION] = szSpecification;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BINTABLE] = szMapBinTable;

			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_REMARK1] = szRemark1;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_REMARK2] = szRemark2;

			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_TEST_TIME] = szTestTime;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WO] = szWO;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SUBSTRATE_ID] = szSubstarteID;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CST_ID] = szCSTID;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_RECIPE_ID] = szRecipeID;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_RECIPE_NAME] = szRecipeName;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_OPERATOR] = szOperator;

			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PART_NO] = szPartNo;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_SERIAL_NUMBER] = szMapSerialNumber;	
			
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_DN]	= szDN;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WN]	= szWN;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CM1]	= szCM1;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CM2]	= szCM2;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CM3]	= szCM3;

			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_REMARK3]	= szRemark3;

			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_EPIID]		= szMapEPID;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_RESORTINGBIN]	= szMapResortingBin;

			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_UN]	= szUN;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_COND]	= szCOND;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LO]	= szLO;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_UP]	= szUP;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_FT]	= szFT;			
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LIM]	= szLIM;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_COEF1]	= szCOEF1;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_COEF2]	= szCOEF2;			
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_COEF3]	= szCOEF3;

			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CENTERPOINT]	= szCenterPoint;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LIFETIME]		= szLifeTime;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_ESD]			= szESD;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PAD_COLOR]	= szPadColor;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BIN_CODE]	= szBinCode;

			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CONTROL_RANK]	= szControlRank;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_RANK_NO]	= szRankNo;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_AT] = szAt;
			
			//SeoulSemi
			(*psmf)[WT_MAP_HEADER_INFO]["Project"]	= szHeaderProject;
			(*psmf)[WT_MAP_HEADER_INFO]["Product Code"] = szHeaderProductCode;

			//v4.13T1	//DengCheng -> APlusEpi
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WAFTYPE]	= szWafType;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BINNAME]	= szBinName;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_U1]		= szU1;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_U2]		= szU2;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_U3]		= szU3;

			if (bSortSetupFile == TRUE)
			{
				(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTSETUPFILE] = szSortSetupFile;
			}

			if (bSortBinFile == TRUE)
			{
				(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTBINFNAME] = szSortBinFileName;
			}
		}

		//Update in SRAM
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SPEC]		= m_szBinSpecVersion;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINPARA]	= szBinParameter;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_TYPE]		= szType;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTBIN]	= szSortBinItem;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PRODUCT]	= szProduct;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTNUMBER]	= szLotNumber;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LATTICE_LOTNUMBER]	= szLotNumber;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_ESD_VOLTAGE] = szMapESDVoltage;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PROBER_MACHINE_NO] = szProberMachineNo;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PROBER_DATE_TIME] = szProberDateTime;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BIN_TABLE_SERIAL_NO] = szBinTableSerialNo;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_DATE_IN_MAP_FILE] = szMapDate;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MAP_OTHER] = szOther;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_INTERNAL_PRODUCT_NO] = szInternalProductNo;	
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MODE] = szMode;								
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SPECIFICATION] = szSpecification;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTBINFNAME] = szSortBinFileName;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINTABLE] = szMapBinTable;

		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK1] = szRemark1;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK2] = szRemark2;

		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_TEST_TIME] = szTestTime;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WO] = szWO;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SUBSTRATE_ID] = szSubstarteID;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CST_ID] = szCSTID;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RECIPE_ID] = szRecipeID;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RECIPE_NAME] = szRecipeName;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_OPERATOR] = szOperator;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PART_NO] = szPartNo;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_SERIAL_NUMBER] = szMapSerialNumber;	

		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DN]	= szDN;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WN]	= szWN;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM1]	= szCM1;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM2]	= szCM2;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM3]	= szCM3;

		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_EPIID] = szMapEPID;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RESORTINGBIN] = szMapResortingBin;

		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK3] = szRemark3;

		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_UN]	= szUN;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COND]	= szCOND;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LO]	= szLO;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_UP]	= szUP;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_FT]	= szFT;			
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LIM]	= szLIM;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF1]	= szCOEF1;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF2]	= szCOEF2;			
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF3]	= szCOEF3;

		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CENTERPOINT]	= szCenterPoint;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LIFETIME]		= szLifeTime;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_ESD]			= szESD;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PAD_COLOR]	= szPadColor;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BIN_CODE]	= szBinCode;

		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CONTROL_RANK]	= szControlRank;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RANK_NO]	= szRankNo;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_AT]		= szAt;

		//v4.13T1	//DengCheng -> APlusEpi
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WAFTYPE]	= szWafType;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINNAME]	= szBinName;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_U1]		= szU1;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_U2]		= szU2;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_U3]		= szU3;

		//SeoulSemi
		m_smfSRam["MS896A"][WT_MAP_HEADER]["Project"]	= szHeaderProject;
		m_smfSRam["MS896A"][WT_MAP_HEADER]["Product Code"] = szHeaderProductCode;

		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DESIGN] = szHeaderDesign;
	}


	if (CMS896AStn::m_WaferMapWrapper.GetReader() != NULL)
	{
		szCategory = CMS896AStn::m_WaferMapWrapper.GetReader()->GetHeader().GetCategory();

		CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_WAFER, szMapOther1);
		CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_DEVICE, szMapOther2);
		CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_WAFERBIN, szMapOther3);
		CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_MODELNO, szMapOther4);
		CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup("SORTINGBIN", szMapOther6);		//Osram Penang RESORT map "SORTINGBIN" header field	//v4.21T3	

		if (szMapOther2 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_DEVICENUMBER, szMapOther2);
		}
		if (szMapOther2 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_DEVICE2, szMapOther2);
		}
		if (szMapOther2 == "")
		{
			//Osram Penang RESORT map "CHIP" header field	//v4.21T3
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup("CHIP", szMapOther2);
		}
		if (szMapOther2 == "")
		{
			//v4.40T14	//AoYang
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup("BinGradeName", szMapOther2);
		}
		if (szMapOther2 == "")
		{
			CString szDevice;
			// Nichia header getting methods
			//unsigned char LotNo[] = {219, 175, 196, 148, 212, 141, 134, 0};
			szDevice.Format("%s", ucaMapHeaderSortTitle);
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(szDevice, szMapOther2);
		}
		
		if (szMapOther4 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_MODELNO2, szMapOther4);
		}

		if (szMapOther4 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_MODEL, szMapOther4);
		}

		if (szMapOther4 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_MODEL2, szMapOther4);
		}

		szMapOther5 = CMS896AStn::m_WaferMapWrapper.GetReader()->GetHeader().GetLotID();
		
		if (szMapOther5 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup("Lot ID", szMapOther5);
		}

		if (szMapOther5 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_LOT_NO, szMapOther5);
		}

		if (szMapOther5 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_LOT_ID, szMapOther5);
		}

		if (szMapOther5 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_LOT_NO_2, szMapOther5);
		}

		if (szMapOther5 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_LOT_NAME, szMapOther5);	//Mitsubishi
		}

		if (szMapOther5 == "")
		{
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup("LOT NO", szMapOther5);	//4.51D11 Inari

		}

		if (szMapOther5 == "")
		{
			CString szLotNo;
			// Nichia header getting methods
			szLotNo.Format("%s", ucaMapHeaderLotNo);
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(szLotNo, szMapOther5);
			CMSLogFileUtility::Instance()->MS_LogOperation("Nichia: Lot ID retrieved from map = " + szMapOther5);
//AfxMessageBox("Nichia Lot No from map = " + szMapOther5, MB_SYSTEMMODAL);
		}

		if (psmf != NULL)
		{
			(*psmf)[WT_MAP_HEADER_INFO]["Map Header Category"]  = szCategory;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WAFER]	= szMapOther1;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_DEVICE]	= szMapOther2;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WAFERBIN]	= szMapOther3;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_MODELNO]	= szMapOther4;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LOTID]	= szMapOther5;
			(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTINGBIN]	= szMapOther6;
		}

		//Update in SRAM
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WAFER]		= szMapOther1;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DEVICE]	= szMapOther2;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WAFERBIN]	= szMapOther3;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MODELNO]	= szMapOther4;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID]		= szMapOther5;
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTINGBIN]	= szMapOther6;
	}

	if (psmf != NULL)
	{
		pUtl->UpdateLastState();
		pUtl->CloseLastState();
	}
	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="Epileds" )
	{

		CStringMapFile HeaderRecord;
		CString szLot;
		CString szTesterCondFile;
		CString szType1;
		CString szSource;
		CString szSetupPackage;
		CString szPR;
		CString szStartDate;
		CString szOperator1;
		CString szWaferID;
		CString szCorrelation;

		CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferID);

		szMapHeaderInfo->Lookup("Lot #", szLot);
		szMapHeaderInfo->Lookup("TESTER COND File", szTesterCondFile);
		szMapHeaderInfo->Lookup("Type", szType1);
		szMapHeaderInfo->Lookup("Source", szSource);
		szMapHeaderInfo->Lookup("CORRELATION FILE NAME", szCorrelation);
		szMapHeaderInfo->Lookup("SETUP PACKAGE", szSetupPackage);
		szMapHeaderInfo->Lookup("PR PACKAGE", szPR);
		szMapHeaderInfo->Lookup("START Date", szStartDate);
		szMapHeaderInfo->Lookup("Operator", szOperator1);
remove(MSD_WAFER_HEADER);
		if (HeaderRecord.Open(MSD_WAFER_HEADER, FALSE, TRUE) == 1)
		{
			(HeaderRecord)[szWaferID][WT_MAP_ADD_LOTNUMBER]	= szLot;
			(HeaderRecord)[szWaferID][WT_MAP_ADD_CONDFILE]	= szTesterCondFile;
			(HeaderRecord)[szWaferID][WT_MAP_ADD_TYPE]		= szType1;
			(HeaderRecord)[szWaferID][WT_MAP_ADD_SOURCE]		= szSource;
			(HeaderRecord)[szWaferID][WT_MAP_ADD_CORRELATION]		= szCorrelation;
			(HeaderRecord)[szWaferID][WT_MAP_ADD_SETUP]		= szSetupPackage;
			(HeaderRecord)[szWaferID][WT_MAP_ADD_PR]			= szPR;
			(HeaderRecord)[szWaferID][WT_MAP_ADD_START]		= szStartDate;
			(HeaderRecord)[szWaferID][WT_MAP_ADD_OP]			= szOperator1;
			HeaderRecord.Update();
			HeaderRecord.Close();
		}
	}

	if(pApp->GetCustomerName()=="Inari")  // 4.51D11 Inari 
	{
		CStringMapFile HeaderRecord;
		CString szWaferID;
		CString szLotNo;
		szMapHeaderInfo->Lookup("LOT NO", szLotNo);

		CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferID);
		if (HeaderRecord.Open(MSD_WAFER_HEADER, FALSE, TRUE) == 1)
		{
			(HeaderRecord)[szWaferID][WT_LOT_NO]	= szLotNo;
			HeaderRecord.Update();
			HeaderRecord.Close();
		}

	}
	// 4.53D5 Update VP Value in Map Header
	if(pApp->GetCustomerName()=="BSE") 
	{

		CStringMapFile HeaderRecord;
		CString szWaferID;
		CString szLotVP;
		szLotVP = m_smfSRam["MS896A"]["SummaryFile"]["VPValue"];
		
		CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferID);
		
		if(szWaferID.Find(".txt") != -1)
		{
			szWaferID = szWaferID.Left(szWaferID.Find(".txt"));
		}
CString szMsg;
szMsg.Format("Start Update VP by Wafer ID: %s", szWaferID );
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
HmiMessage(szMsg);

		CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferID);
		if (HeaderRecord.Open(MSD_WAFER_HEADER, FALSE, TRUE) == 1)
		{
			(HeaderRecord)[szWaferID][WT_LOT_VP]	= szLotVP;
			HeaderRecord.Update();
			HeaderRecord.Close();
		}
	}

	return TRUE;
}

BOOL CMS896AApp::SeoulSemiUpdateAllRankName()
{
	CString szElectricInfoLine = "";
	CString szMapFileName;
	CStdioFile oFile;
	
	if ( CMS896AStn::m_WaferMapWrapper.GetReader() == NULL )
	{
		AfxMessageBox("The WaferMapReader is NULL"); 
		return FALSE;
	}

	if ( !CMS896AStn::m_WaferMapWrapper.IsMapValid() )
	{
		AfxMessageBox("The WaferMap is NOT Vaild"); 
		return FALSE;
	}

	szMapFileName = MS_LCL_CURRENT_MAP_FILE;
	if ( _access(szMapFileName, 0) == -1 )
	{
		AfxMessageBox("CurrentMap.txt is missing!");
		return FALSE;
	}
	if ( oFile.Open(szMapFileName, CFile::modeRead|CFile::shareDenyNone) )
	{
		ULONG ulNumOfRows=0, ulNumOfCols=0;
		UCHAR cTempGrade, szGradeOffset;
		CUIntArray aulAllGradeList;
		CMS896AStn::m_WaferMapWrapper.GetAvailableGradeList(aulAllGradeList);
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		if (pBTfile == NULL)
		{
			//AfxMessageBox("FALSE");
			return FALSE;
		}

		CMS896AStn::m_WaferMapWrapper.GetMapDimension(ulNumOfRows, ulNumOfCols);

		// save rank name in list
		CString szTempRank;
		szGradeOffset = CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
		for ( ULONG i = 0 ; i < ulNumOfRows ; i++ )
		{
			for ( ULONG j = 0 ; j < ulNumOfRows ; j++ )
			{
				cTempGrade = CMS896AStn::m_WaferMapWrapper.GetGrade(i, j) - szGradeOffset;
				for ( ULONG k = 0 ; k < (ULONG)aulAllGradeList.GetSize() ; k++ )
				{
					if ( (ULONG)cTempGrade == aulAllGradeList.GetAt(k) - szGradeOffset )
					{
						CMS896AStn::m_WaferMapWrapper.GetExtraInformation(&oFile, i, j, szElectricInfoLine);
						ExtractDataInCSVRowLine(szElectricInfoLine, szTempRank, 2, ",");
						//save data in bintable.msd
						CString szRank;
						szRank.Format( "%d", (ULONG)cTempGrade);
						//AfxMessageBox(szRank);
						(*pBTfile)["BinBlock"][szRank]["Bin Rank Name"] = szTempRank;
						//save data in SRAM 
						m_smfSRam["MS896A"][WT_MAP_HEADER]["Rank Name"][szRank] = szTempRank;
					}
				}
			}
		}

		oFile.Close();
		CMSFileUtility::Instance()->SaveBTConfig();
	}
	
	return TRUE;
}

BOOL CMS896AApp::ExtractDataInCSVRowLine(CString szInputLine, CString &szOutputWord, ULONG ulIndex, CString szRegex)
{
	/*
		Get the CSV in line, szRegex = ","
		Example 1 : aa,bbb,cccc,ddddd,eeeeee		Index 2 -> cccc
		Example 2 : ,aa,bbb,cccc,ddddd,eeeeee		Index 2 -> bbb
		return FALSE if no COMMA(szRegex) in the line
	*/
	if ( szInputLine == "" )
	{
		return FALSE;
	}
	//AfxMessageBox("ExtractData");
	LONG lTemp = szInputLine.Find(szRegex);
	BOOL bReturn = FALSE;
	if ( lTemp == -1 )
	{
		szOutputWord = szInputLine;
		return FALSE;
	}
	else 
	{
		CString szTempLine = szInputLine;
		CString szData;
		ULONG i = 0;
		lTemp = szTempLine.Find(szRegex);
		while ( lTemp != -1 )
		{
			szData = szTempLine.Left(lTemp);
			szTempLine = szTempLine.Right(szTempLine.GetLength() - lTemp - 1);
			if ( i == ulIndex )
			{
				szOutputWord = szData;
				bReturn = TRUE;
				break;
			}
			i++;
			lTemp = szTempLine.Find(szRegex);
		}
	}
	return bReturn;
}

BOOL CMS896AApp::UpdateMapLotNo(CString szLotNo)
{
	m_szMapLotNumber = szLotNo;
	UpdateHostLotID(m_szMapLotNumber);

	//v4.42T6	//Testar to retrieve lotNum from MESConnector
	
	if (szLotNo.GetLength() > 0)	//v4.42T18
	{
		CMSFileUtility  *pUtl = CMSFileUtility::Instance();
		CStringMapFile  *psmf;
		if (pUtl->LoadLastState())
		{
			psmf = pUtl->GetLastStateFile();		
			if (psmf != NULL)
			{
				(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LOTNUMBER] = szLotNo;
				pUtl->UpdateLastState();
				pUtl->CloseLastState();
			}
		}
	
		//v4.42T6	//Update SRam as well
		m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTNUMBER]	= szLotNo;
	}
	return TRUE;
}


VOID CMS896AApp::CheckOutputFileFormatName(CString &szFormatName)
{
	CStdioFile cfFileFormat;
	CString szName;
	BOOL bIfFileExists;
	int siIndex = 0;

	bIfFileExists = cfFileFormat.Open(gszROOT_DIRECTORY + "\\Exe\\BinOutputFileFormatSelection.txt", 
									  CFile::modeRead | CFile::shareExclusive | CFile::typeText);

	if (bIfFileExists)
	{
		cfFileFormat.SeekToBegin();

		if (cfFileFormat.ReadString(szName) == TRUE)
		{
			if ((szFormatName.IsEmpty() == TRUE) || (szName.Find(szFormatName) == -1) || (szFormatName == " "))
			{
				//Update current name if not found
				siIndex = szName.Find(",");
				if (siIndex != -1)
				{
					szFormatName = szName.Left(siIndex);
				}
				else
				{
					szFormatName = szName;
				}
				WriteProfileString(gszPROFILE_SETTING, gszOUTPUT_FORMAT_SETTING, szFormatName);
			}
		}

		cfFileFormat.Close();
	}
	else
	{
		szFormatName = "";
		WriteProfileString(gszPROFILE_SETTING, gszOUTPUT_FORMAT_SETTING, szFormatName);
	}

	//Also update member variable name as bintable stn will call this function
	m_szOutputFileFormat = szFormatName;
}


VOID CMS896AApp::CheckWaferEndFileFormatName(CString &szFormatName)
{
	CStdioFile cfFileFormat;
	CString szName;
	BOOL bIfFileExists;
	int siIndex = 0;

	bIfFileExists = cfFileFormat.Open(gszROOT_DIRECTORY + "\\Exe\\WaferEndFileFormatSelection.txt", 
									  CFile::modeRead | CFile::shareExclusive | CFile::typeText);

	if (bIfFileExists)
	{
		cfFileFormat.SeekToBegin();

		if (cfFileFormat.ReadString(szName) == TRUE)
		{
			if ((szFormatName.IsEmpty() == TRUE) || (szName.Find(szFormatName) == -1) || (szFormatName == " "))
			{
				//Update current name if not found
				siIndex = szName.Find(",");
				if (siIndex != -1)
				{
					szFormatName = szName.Left(siIndex);
				}
				else
				{
					szFormatName = szName;
				}
				WriteProfileString(gszPROFILE_SETTING, gszWAFEREND_FORMAT_SETTING, szFormatName);
			}
		}

		cfFileFormat.Close();
	}
	else
	{
		szFormatName = "";
		WriteProfileString(gszPROFILE_SETTING, gszWAFEREND_FORMAT_SETTING, szFormatName);
	}

	//Also update member variable name as bintable stn will call this function
	m_szWaferEndFormat = szFormatName;
}


// Set progress bar limit & update percentage
BOOL CMS896AApp::SetProgressBarLimit(LONG lLimit)
{
	if (m_bBurnIn == TRUE)
	{
		return FALSE;
	}

	IPC_CServiceMessage stMsg;
	BOOL bReply = FALSE;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(LONG), &lLimit);
	nConvID = m_comClient.SendRequest(SECS_COMM_STN, "SetProgressBarLimit", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
	
			stMsg.GetMsg(sizeof(BOOL), &bReply);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bReply;
}

BOOL CMS896AApp::UpdateProgressBar(LONG lPercent)
{
	if (m_bBurnIn == TRUE)
	{
		return FALSE;
	}

	IPC_CServiceMessage stMsg;
	BOOL bReply = FALSE;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(LONG), &lPercent);
	nConvID = m_comClient.SendRequest(SECS_COMM_STN, "UpdateProgressBar", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);

			stMsg.GetMsg(sizeof(BOOL), &bReply);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bReply;
}

LONG CMS896AApp::LoadSelectedLanguage()
{
	return MS_LoadLastState();
}


BOOL CMS896AApp::CheckVolumeInfo()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	LONG lSerial;

	DWORD dwSysFlags;            // flags that describe the file system
	char FileSysNameBuf[100];
	DWORD dwSerial;


	if (m_lVisionPlatform == PR_SYSTEM_AOI)		//v4.05	//Klocwork
	{
		//m_bNewCtEnhancement = TRUE;				//AOI default CT = FAST
		return TRUE;
	}

	if (m_lMotionPlatform == SYSTEM_NUMOTION)	//v4.05	//Klocwork
	{
		//m_bNewCtEnhancement = TRUE;				//NU_MOTION defatul CT = FAST		//v3.66
		return TRUE;
	}

	BOOL bStatus = GetVolumeInformation(NULL, NULL, 0, &dwSerial, NULL, &dwSysFlags, FileSysNameBuf, 100);
	if (!bStatus)
	{
		AfxMessageBox("Fail to get volume info!  Application is aborted.", MB_SYSTEMMODAL);
		return FALSE;
	}

	lSerial	= (LONG) dwSerial;

	//load & get config file ptr
	pUtl->LoadAppFeatureConfig(); 
	psmf = pUtl->GetAppFeatureFile();
	LONG lCurrSerial = 0;
	if (psmf != NULL)		//Klocwork	//v4.02T5
	{
		lCurrSerial	= ((LONG)(*psmf)[MS896A_FUNC_GENERAL][MS896A_SERIAL_VERSION]);
		pUtl->CloseAppFeatureConfig();
	}

	m_lHardwareConfigNo	= GetMachineHardwareConfig(MS896A_CFG_HW_CONFIG_NO);
	m_lBondHeadConfig	= GetMachineHardwareConfig(MS896A_CFG_HW_BOND_HEAD_CONFIG);

	//v3.28T1
	if ((m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) ||			//UBH
			(m_lHardwareConfigNo == MS896A_DDBS_WITH_RING_MASTER))	//New SIO COnfig
	{
		//m_bNewCtEnhancement = TRUE;
	}
	else
	{
		//if (CheckMatrixDongle())	//Apart from Mew SIO Config, ON for MS899 with Dongle		//v3.33T5
		//{
		//	m_bNewCtEnhancement = TRUE;
		//}
		//else
		//{
		//	m_bNewCtEnhancement = FALSE;
		//}
	}

	//v4.46T26
	CString szLog;
	//szLog.Format("    Check HD Volume: HWConfig = %ld, NewCT = %d", m_lHardwareConfigNo, m_bNewCtEnhancement);
	szLog.Format("    Check HD Volume: HWConfig = %ld", m_lHardwareConfigNo);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return TRUE;
}

BOOL CMS896AApp::ReadAutorizedPrinterList(CStringArray &szaAuthorizedPrinterList)
{
	LONG lTotalNoOfPrinters = 0;
	CString szTemp;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	
	CString szPrinterModel;

	//load & get config file ptr
	pUtl->LoadAuthorizedPrinterListFile();
	psmf = pUtl->GetAuthorizedPrinterListFile();

	//Check Load/Save Data
	if (psmf != NULL)
	{
		lTotalNoOfPrinters = (LONG)(*psmf)[MS896A_LABEL_PRINTERS][MS896A_LABEL_PRINTERS_NUMBER];

		for (INT i = 0; i < (INT)lTotalNoOfPrinters; i++)
		{
			szTemp.Format("%d", i + 1);
			szPrinterModel = (*psmf)[MS896A_LABEL_PRINTERS][MS896A_PRINTER_MODEL][szTemp];
			szaAuthorizedPrinterList.Add(szPrinterModel);
		}
	}

	//close config file ptr
	pUtl->CloseAuthorizedPrinterListFile();

	return TRUE;
}

BOOL CMS896AApp::CreatePrinterSelectionFile()
{
	// Get Printer from
	BOOL bIsAuthorized;
	CString szPrinterModel = "";
	CString szLocalPrinter = "", szTempPrinterNmae = "";
	CStringArray szaLabelPrinterModels;
	CStringArray szaAuthorizedPrinterModels;
	
	char pBuffer[10000];
	DWORD nNumPrinters = 0;
	DWORD nSizeNeeded = 0;

	ReadAutorizedPrinterList(szaAuthorizedPrinterModels);
	
	if (EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, (LPBYTE)pBuffer, 10000, &nSizeNeeded, &nNumPrinters) == TRUE)
	{
		PRINTER_INFO_5 *pInfo = (PRINTER_INFO_5*)pBuffer;
		int i;

		for (i = 0; i < (INT) nNumPrinters; i++)
		{
			bIsAuthorized = FALSE;
			szLocalPrinter = pInfo[i].pPrinterName;
			szLocalPrinter = szLocalPrinter.MakeUpper();

			//if (szLocalPrinter.Find("DYMO") == -1)
			//continue;

			for (INT j = 0; j < szaAuthorizedPrinterModels.GetSize(); j++)
			{
				szTempPrinterNmae = szaAuthorizedPrinterModels.GetAt(j);
				szTempPrinterNmae = szTempPrinterNmae.MakeUpper();
				szLocalPrinter = szLocalPrinter.MakeUpper();

				if (szTempPrinterNmae == szLocalPrinter)
				{
					bIsAuthorized = TRUE;
				}
			}

			if (bIsAuthorized == TRUE)
			{
				szaLabelPrinterModels.Add(pInfo[i].pPrinterName);	
			}
		}
	}

	CString szFilename;
	szFilename = gszROOT_DIRECTORY + "\\Exe\\PrinterSelection.txt";

	DeleteFile(szFilename);

	CStdioFile cfLabelPtrModelsFile;
	
	if (cfLabelPtrModelsFile.Open(szFilename, 
								  CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText) == FALSE)
	{
		return FALSE;
	}
	
	for (INT i = 0; i < szaLabelPrinterModels.GetSize(); i++)
	{
		if (i == 0)
		{
			cfLabelPtrModelsFile.WriteString(szaLabelPrinterModels.GetAt(i));
		}
		else
		{
			cfLabelPtrModelsFile.WriteString("," + szaLabelPrinterModels.GetAt(i));
		}
	}
	
	cfLabelPtrModelsFile.Close();
	
	CMSLogFileUtility::Instance()->MS_LogOperation("Created Printer selection file");	//v4.46T26
	return TRUE;
}


BOOL CMS896AApp::CheckExpireDate()
{
	WriteProfileInt(gszPROFILE_SETTING, _T("Expire M"), 0);
/*
	CTime time = CTime::GetCurrentTime();
	INT nMonth = time.GetMonth();

	INT nSaveMonth = GetProfileInt(gszPROFILE_SETTING, _T("Expire M"), 0);
	if (nSaveMonth == 0)
	{
		WriteProfileInt(gszPROFILE_SETTING, _T("Expire M"), nMonth);
	}
	else
	{
		if (nMonth != nSaveMonth)
			return FALSE;
	}
*/

	//v3.99T1
#ifdef NU_MOTION
/*
	CTime curTime = CTime::GetCurrentTime();
	CTime oldTime = CTime::CTime(2011, 3, 1, 0, 0, 0);
	CTimeSpan ts = curTime - oldTime;

	if (ts.GetDays() <= 3)
	{
		m_lCycleSpeedMode = 4;
	}
	else
	{
		m_lCycleSpeedMode = 3;
	}
*/
#endif

	return TRUE;
}

//Call the Matrix Dongle for checking to enable the New Cycle Time Config			//v3.33T5
BOOL CMS896AApp::CheckMatrixDongle()
{
	/*
#ifndef MS_DEBUG
	INT nRetCode = Init_MatrixAPI();

	if (nRetCode < 0)
	{
		//DisplayMessage("    Init USB fail");	//v3.82
		return FALSE;
	}

	ULONG pulKey[4] = {0x000DB86C, 0x00000000, 0x00000000, 0x00000000};	//0x000DB86C = 899180
	ULONG pulData[2], pulApp[2];
			
	srand((unsigned)time(NULL));
	for (INT i = 0; i < 2; i++)
	{
		pulData[i] = pulApp[i] = (unsigned long)rand();
	}

	register unsigned long delta = 0x9E3779B9;
	register unsigned long sum = 0;
	short cnt = 32;
	
	while (cnt-- > 0)
	{
		pulData[0] += (pulData[1] << 4 ^ pulData[1] >> 5) + pulData[1] ^ sum + pulKey[sum & 3];
		sum += delta;
		pulData[1] += (pulData[0] << 4 ^ pulData[0] >> 5) + pulData[0] ^ sum + pulKey[sum >> 11 & 3];
	}

	ULONG ulUserCode = 0x81a8;
	SHORT sPort = Dongle_Find();
	Dongle_EncryptData(ulUserCode, (long*)pulApp, 1, sPort);		//	Encypt the pulDng by USB key

	for (INT i = 0 ; i < 2 ; i++)
	{
		if (pulData[i] != pulApp[i])								//	Check both are equal or not.
		{
			Release_MatrixAPI();
			//DisplayMessage("    USB fail");	//v3.82
			return FALSE;					//	FAIL
		}
	}

	Release_MatrixAPI();
	DisplayMessage("USB Dongle is enabled");	//v3.82
#endif
	*/
	return TRUE;
}


BOOL CMS896AApp::CreateMSLicenseStringFile()
{
	/*
#ifndef MS_DEBUG	//v4.47T3

	int ret = 0;
	char szFilePath[KM_PATH_LEN] = _T("C:\\MapSorter\\UserData");
	char szMachineID[KM_MID_LEN] = _T("MS100.lcs");

	CString szFileName =  _T("C:\\MapSorter\\UserData\\MS100.lcs");
	DeleteFile(szFileName);		//v4.46T26

	CMSLogFileUtility::Instance()->MS_LogOperation("Create MS License Key ...");	//v4.46T26

	ret = KmFunction0(szFilePath, szMachineID);
	if(ret != KM_OK){
		DisplayMessage("Fail to create MS License string file");
		return FALSE;
	}
#endif

	DisplayMessage("Created MS License string file");
	CMSLogFileUtility::Instance()->MS_LogOperation("MS License string file created");
	*/
	return TRUE;
}

BOOL CMS896AApp::RegisterMSLicenseKey()
{
	/*
	WIN32_FIND_DATA FileData;
	CString szFileName, szFoundName;
	BOOL bLicFileFound = FALSE;
	
	HANDLE hSearch = FindFirstFile("c:\\MapSorter\\UserData\\*", &FileData);
	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return FALSE;
	}

	do 
	{
		// if it is a folder recurive call to remove file
		szFoundName = FileData.cFileName;
		if( (szFoundName.CompareNoCase(".") == 0) || (szFoundName.Compare("..") == 0) )
		{
			continue;
		}

		if ((FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			continue;
		}

		if (szFoundName.Find(".lic") != -1)
		{
			bLicFileFound = TRUE;
			break;
		}

	} while (FindNextFile(hSearch, &FileData)); 
	FindClose(hSearch);

	if (!bLicFileFound)
	{
		HmiMessage("LIC Key file is not found in \\UserData folder.");
		return FALSE;
	}

	szFileName = "c:\\MapSorter\\UserData\\" + szFoundName;

	//v4.46T26
	CMSLogFileUtility::Instance()->MS_LogOperation("Register License Key File: " + szFileName);	

#ifndef MS_DEBUG	//v4.47T3

	CStdioFile oFile;
	if (oFile.Open(szFileName, CFile::modeNoTruncate|CFile::modeRead|CFile::typeText) == FALSE)
	{
		return FALSE;
	}

	CString szLicenseKey	= _T("");
	CString szLine			= _T("");

	while (oFile.ReadString(szLine))
	{
		szLicenseKey = szLicenseKey + szLine;
	}

	oFile.Close();
	//AfxMessageBox("RegisterMSLicenseKey - " + szLicenseKey, MB_SYSTEMMODAL);
	DisplayMessage("RegisterMSLicenseKey registered");
	CMSLogFileUtility::Instance()->MS_LogOperation("RegisterMSLicenseKey - " + szLicenseKey);


	int	ret = 0;		
	char pSWName[KM_NAME_LEN] = MS_LICENSE_SW_NAME;		//_T("MS100");
	char pKeyBuf[KM_KEY_LEN+1];
	strcpy_s(pKeyBuf, sizeof(pKeyBuf), (LPCTSTR) szLicenseKey);

	ret = KmFunction1(MS_KM_GROUP_NO_COB, pSWName, pKeyBuf);
	if (ret == KM_OK)
	{
		DisplayMessage("MS License Key Registration is successfull");
		CMSLogFileUtility::Instance()->MS_LogOperation("MS License Key Registration is OK");
		DeleteFile(szFileName);
	}
	else 
	{
		CString szErr;
		szErr.Format("RegisterMSLicenseKey fail - %d", ret);
		CMSLogFileUtility::Instance()->MS_LogOperation("MS License Key Registration fails");
		DisplayMessage(szErr);
	}
#endif
	*/
	return TRUE;
}

void LicenseCheckCallbackFunc(int ret, int nLeft)
{
	/*
#ifndef MS_DEBUG	//v4.47T5
	CString szMsg;
	switch(ret)
	{
	case KM_NOACTIVATED:
		AfxMessageBox("MS License: KEY is not actived!  Please register.", MB_SYSTEMMODAL);
		CMSLogFileUtility::Instance()->MS_LogOperation("KM: license key is not actived!  Please register!");
		break;

	case KM_EVALFAIL:
		AfxMessageBox("MS License: KEY has expired! Please register new license.", MB_SYSTEMMODAL);
		CMSLogFileUtility::Instance()->MS_LogOperation("KM: Production license has expired! Please register new license!");
		break;

	case KM_CHWFAIL:
		AfxMessageBox("MS License: invalid KEY! Please register new license", MB_SYSTEMMODAL);
		CMSLogFileUtility::Instance()->MS_LogOperation("KM: invalid license key! Please register new license!");
		break;

	case KM_OK:
		CMS896AApp::m_bIsMSLicenseKeyChecked = TRUE;
		//AfxMessageBox("KM license key is OK !", MB_SYSTEMMODAL);
		szMsg.Format("KM license: KEY validation RPY OK; time left = #%d days", nLeft);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		break;
	}
#endif
	*/
}

BOOL CMS896AApp::ValidateMSLicenseKey()
{
	/*
#ifndef MS_DEBUG	//v4.47T3
	char pSWName[KM_NAME_LEN] = MS_LICENSE_SW_NAME;		//_T("MS100");

//AfxMessageBox("ValidateMSLicenseKey ...", MB_SYSTEMMODAL);

	int ret = KmFunction2(MS_KM_GROUP_NO_COB, pSWName, &LicenseCheckCallbackFunc);
	if (ret != KM_OK)
	{
		CString szErr;
		szErr.Format("ValidateMSLicenseKey fail - %d", ret);
		DisplayMessage(szErr);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
		m_bIsMSLicenseKeyChecked = FALSE;	
		return FALSE;
	}

	//m_bIsMSLicenseKeyChecked = TRUE;	
	//DisplayMessage("ValidateMSLicenseKey is Done");
	CMSLogFileUtility::Instance()->MS_LogOperation("ValidateMSLicenseKey is Done");
#else
	//m_bIsMSLicenseKeyChecked = FALSE;	
#endif
	*/
	return TRUE;
}


BOOL CMS896AApp::ValidateMSLicenseKeyFeature()
{
	/*
#ifndef MS_DEBUG
	//AfxMessageBox("ValidateMSLicenseKeyFeature ...", MB_SYSTEMMODAL);
	char pSWName[KM_NAME_LEN] = MS_LICENSE_SW_NAME;		//_T("MS100");
	INT nModuleNum = 0;
	CString szMsg;

	m_bMSLicenseKeyFcn1 = FALSE;
	m_bMSLicenseKeyFcn2 = FALSE;
	m_bMSLicenseKeyFcn3 = FALSE;
	m_bMSLicenseKeyFcn4 = FALSE;
	m_bMSLicenseKeyFcn5 = FALSE;

	for (int j=0; j<5; j++) 
	{
		INT ret = KmFunction3(MS_KM_GROUP_NO_COB, pSWName, nModuleNum, j);
		if (ret == KM_OK)
		{
			szMsg.Format("MS License: Support Module #%d Function #%d ", nModuleNum, j+1);
			DisplayMessage(szMsg);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

			switch (j)
			{
			case 0:
				m_bMSLicenseKeyFcn1 = TRUE;
				break;
			case 1:
				m_bMSLicenseKeyFcn2 = TRUE;
				break;
			case 2:
				m_bMSLicenseKeyFcn3 = TRUE;
				break;
			case 3:
				m_bMSLicenseKeyFcn4 = TRUE;
				break;
			case 4:
				m_bMSLicenseKeyFcn5 = TRUE;
				break;
			}
		}
	}
#endif
	*/
	return TRUE;
}


// prescan relative code	B
UINT CMS896AApp::GetPrescanWaferEdgeNum()
{
	UINT nTemp;

	nTemp = GetProfileInt(gszPROFILE_SETTING, REG_PRESCAN_WAFER_EDGE_NUM, 1);

	if (nTemp <= 0)
	{
		nTemp = 1;
	}
	if (nTemp > 20)
	{
		nTemp = 1;
	}

	WriteProfileInt(gszPROFILE_SETTING, REG_PRESCAN_WAFER_EDGE_NUM, nTemp);
	return nTemp;
}

BOOL CMS896AApp::GetScanMapIndexTolerance(DOUBLE &dRowTol, DOUBLE &dColTol)
{
	UINT nRowTemp = 25, nColTemp = 25;
	nRowTemp = GetProfileInt(gszPROFILE_SETTING, _T("Prescan map index row tolerance%"), 25);
	if (nRowTemp <= 0 || nRowTemp > 100)
	{
		nRowTemp = 25;
	}
	WriteProfileInt(gszPROFILE_SETTING, _T("Prescan map index row tolerance%"), nRowTemp);

	nColTemp = GetProfileInt(gszPROFILE_SETTING, _T("Prescan map index col tolerance%"), 25);
	if (nColTemp <= 0 || nColTemp > 100)
	{
		nColTemp = 25;
	}
	WriteProfileInt(gszPROFILE_SETTING, _T("Prescan map index col tolerance%"), nColTemp);

	dRowTol = (DOUBLE)((DOUBLE)nRowTemp / 100.0);	// 0.25
	dColTol = (DOUBLE)((DOUBLE)nColTemp / 100.0);	// 0.25

	return TRUE;
}
// prescan relative code	E

BOOL CMS896AApp::IsStopAlign()			//v4.05	//Klocwork
{
	return m_bStopAlign;
}

BOOL CMS896AApp::IsUseHmiMap4HomeFPC()
{
	return m_bHomeFPCUseHmiMap;
}

BOOL CMS896AApp::IsBLBCUseOldGoStop()
{
	return m_bBLBCUseOldContinueStop;
}

VOID CMS896AApp::GetAlarmCodeAction(CString szAlarmCode, LONG &lAction)
{
	ALARM_INFO stInfo;

	lAction = ALARM_RED_EVENT;
	
	for (INT i = 0; i < m_aAlarmInformation.GetSize(); i++)
	{
		stInfo = m_aAlarmInformation.GetAt(i);

		if (stInfo.szAlarmCode == szAlarmCode)
		{
			lAction = stInfo.lAction;
			return;
		}
	}
}

BOOL CMS896AApp::LoadAlarmInformation(CString szFilename)
{
	INT nCol = -1;
	CString szStr;
	CString szErrorCode, szDescription, szAction;
	CStdioFile cfFile;
	ALARM_INFO stInfo;

	if (cfFile.Open(szFilename, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	m_aAlarmInformation.RemoveAll();

	while (cfFile.ReadString(szStr) == TRUE)
	{
		stInfo.lAction = ALARM_RED_EVENT;
		stInfo.szDecription = "";
		stInfo.szAlarmCode = "";

		nCol = szStr.Find(",");

		if (nCol != -1)
		{
			szErrorCode = szStr.Left(nCol);
			szAction = szStr.Right(szStr.GetLength() - nCol - 1);

			stInfo.szAlarmCode = szErrorCode;
			stInfo.lAction = atoi(szAction);

			m_aAlarmInformation.Add(stInfo);
		}
	}

	return TRUE;
}

VOID CMS896AApp::DumpLoadAlarmInformation(CString szFilename)
{
	CStdioFile cfFile;
	ALARM_INFO stInfo;
	CString szStr;

	if (cfFile.Open(szFilename, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite) == FALSE)
	{
		return;
	}

	for (INT i = 0; i < m_aAlarmInformation.GetSize(); i++)
	{
		stInfo = m_aAlarmInformation.GetAt(i);
		szStr.Format("%s, %d", stInfo.szAlarmCode, stInfo.lAction);
		cfFile.WriteString(szStr + "\n");
	}
	cfFile.Close();
}

VOID CMS896AApp::SetAlarmLamp_Red_Only(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_RED_NO_BUZZER, bBlink, bLockAlarmLamp);
}

VOID CMS896AApp::SetAlarmLamp_RedYellow_Only(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_REDYELLOW_NO_BUZZER, bBlink, bLockAlarmLamp);
}

VOID CMS896AApp::SetAlarmLamp_Red(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_RED, bBlink, bLockAlarmLamp);
}

VOID CMS896AApp::SetAlarmLamp_Yellow(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_YELLOW, bBlink, bLockAlarmLamp); 
}

VOID CMS896AApp::SetAlarmLamp_Green(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_GREEN, bBlink, bLockAlarmLamp); 
}

VOID CMS896AApp::SetAlarmLamp_RedYellow(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_REDYELLOW_BUZZER, bBlink, bLockAlarmLamp); 
}

VOID CMS896AApp::SetAlarmLamp_YellowGreen(BOOL bBlink, BOOL bLockAlarmLamp) //4.51D8
{
	SetAlarmLamp_Status(ALARM_YELLOWGREEN_BUZZER, bBlink, bLockAlarmLamp); 
}

VOID CMS896AApp::SetAlarmLamp_YellowGreenNoBuzzer(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_YELLOWGREEN_NO_BUZZER, bBlink, bLockAlarmLamp); 
}


VOID CMS896AApp::SetAlarmLamp_OffLight(BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_OFF_LIGHT, FALSE, bLockAlarmLamp);
}

VOID CMS896AApp::SetAlarmLamp_YellowBuzzer(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_YELLOW_BUZZER, bBlink, bLockAlarmLamp); 
}

VOID CMS896AApp::SetAlarmLamp_Back(LONG lOrgStatus, BOOL bBlink, BOOL bLockAlarmLamp)
{
	CString szMsg;

	if (lOrgStatus == ALARM_YELLOW)
	{
		SetAlarmLamp_Yellow(bBlink, bLockAlarmLamp);
	}
	else
	{
		SetAlarmLamp_Green(bBlink, bLockAlarmLamp);
	}
}


LONG CMS896AApp::GetAlarmLamp_Status()
{
	return CMS896AApp::m_lAlarmLampStatus;
}

VOID CMS896AApp::SetAlarmLamp_Status(LONG lLevel, BOOL bBlink, BOOL bLockAlarmLamp)
{
	CSingleLock slLock(&m_csMachineAlarmLamp);

	if (bLockAlarmLamp)
	{
		slLock.Lock();
	}

	LONG lNewLevel = lLevel;

	CMS896AApp::m_lAlarmLampStatus = lNewLevel;
	CMS896AApp::m_lAlarmBlinkStatus = lNewLevel;
	CMS896AApp::m_bEnableAlarmTowerBlink = bBlink;

	if (bLockAlarmLamp)
	{
		slLock.Unlock();
	} 
}

//andrewng
BOOL CMS896AApp::IsUsingEngineerMode()
{
	if (m_bEngineer || m_bAdministrator)
		return TRUE;
	return FALSE;
}

VOID CMS896AApp::SetEqTimeMapDieType(CString szType)
{
	m_eqMachine.SetMapDieType(szType);
	m_eqMachine2.SetMapDieType(szType);
}

//4.52D10 fnc print wafer label file
BOOL CMS896AApp::PrintRebealLabel()
{ 
	CString szMsg;
	CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel --  Start Print Rebeal Label file");

	CStdioFile cfBarcodeFile;
	CString szLabelName, szLineData, szFont;
	CString szPrinterName;
	CString szMachineType;
	CPtrFactory PtrFactory;
	RECT TextRect, BarcodeRect;

	BOOL bFileOpened = FALSE;
	int nTextTop, nTextLeft, nTextBottom, nTextRight, nTextMid;  
	int nFontSize;
	
	CLblPtr* m_pPtrObj;
	m_pPtrObj = PtrFactory.Create(0);

	if (m_pPtrObj == NULL)
	{
	//	return FALSE;	//v2.83T69
	}


	//Get machine type (1=Standard, 2=DL)
	//szMachineType = ptrQueryDatabase->GetMachineInfo("Machine Type");
	szFont = "Tahoma";
	nFontSize = 6;

	//useing 24.5 x 24.5mm label
	//Set Default value
	szPrinterName	= "DYMO LabelWriter 450 Turbo";
	szLabelName	= "Lever Arch File - Large labels, Portrait (99019)";
	//szLabelName		= "Name Badge Labels (99014)";

	nTextMid	= 3200;
	nTextTop	= 2000;
	nTextLeft	= 20;
	nTextBottom	= 4800;
	nTextRight	= 300;

	//v3.75
	//*************************************//
	//Default label format
	RECT TRect_BinTapeID;
	TRect_BinTapeID.top			= 100;
	TRect_BinTapeID.bottom		= 4800;
	TRect_BinTapeID.left		= 120;
	TRect_BinTapeID.right		= 400;

	RECT BcRect_BinTapeID;
	BcRect_BinTapeID.top		= 100;
	BcRect_BinTapeID.bottom		= 4800;
	BcRect_BinTapeID.left		= 400;
	BcRect_BinTapeID.right		= 550;

	RECT TRect_BinType;
	TRect_BinType.top			= 100;
	TRect_BinType.bottom		= 1250;
	TRect_BinType.left			= 420;
	TRect_BinType.right			= 700;

	RECT BcRect_BinType;
	BcRect_BinType.top			= 100;
	BcRect_BinType.bottom		= 1350;
	BcRect_BinType.left			= 680;
	BcRect_BinType.right		= 830;

	RECT TRect_TapeType;				// New item
	TRect_TapeType.top			= 100;
	TRect_TapeType.bottom		= 1350;
	TRect_TapeType.left			= 680;
	TRect_TapeType.right		= 830;

	RECT BcRect_TapeType;
	BcRect_TapeType.top			= 100;    
	BcRect_TapeType.bottom		= 1350;		//design later
	BcRect_TapeType.left		= 680;
	BcRect_TapeType.right		= 830;

	RECT TRect_LotID;
	TRect_LotID.top				= 2000;
	TRect_LotID.bottom			= 4800;
	TRect_LotID.left			= 600;
	TRect_LotID.right			= 880;

	RECT BcRect_LotID;
	BcRect_LotID.top			= 100;
	BcRect_LotID.bottom			= 4800;
	BcRect_LotID.left			= 870;
	BcRect_LotID.right			= 1050;

	RECT TRect_TileID;
	TRect_TileID.top			= 2000;  // New Item
	TRect_TileID.bottom			= 4800;
	TRect_TileID.left			= 1020;
	TRect_TileID.right			= 1300;

	RECT BcRect_TileID;
	BcRect_TileID.top			= 2000;
	BcRect_TileID.bottom		= 4800;
	BcRect_TileID.left			= 1300;
	BcRect_TileID.right			= 1450;

	RECT TRect_PName;
	TRect_PName.top				= 2000;
	TRect_PName.bottom			= 4800;
	TRect_PName.left			= 1020;
	TRect_PName.right			= 1300;

	RECT BcRect_PName;
	BcRect_PName.top			= 2000;
	BcRect_PName.bottom			= 4800;
	BcRect_PName.left			= 1300;
	BcRect_PName.right			= 1450;

	RECT TRect_QtyIn;
	TRect_QtyIn.top				= 4000;
	TRect_QtyIn.bottom			= 4800;
	TRect_QtyIn.left			= 1470;
	TRect_QtyIn.right			= 1750;

	RECT TRect_Block;	//xyz
	TRect_Block.top				= 100;	//4000;
	TRect_Block.bottom			= 4800;
	TRect_Block.left			= 1720;
	TRect_Block.right			= 2000;	//2010;

	RECT TRect_OutputID;
	TRect_OutputID.top			= 100;
	TRect_OutputID.bottom		= 2000;
	TRect_OutputID.left			= 1020;
	TRect_OutputID.right		= 1300;

	RECT BcRect_OutputID;
	BcRect_OutputID.top			= 100;
	BcRect_OutputID.bottom		= 2000;
	BcRect_OutputID.left		= 1300;
	BcRect_OutputID.right		= 1450;

	RECT TRect_CatCode;	//xyz
	TRect_CatCode.top			= 2000;
	TRect_CatCode.bottom		= 3500;	//3700;
	TRect_CatCode.left			= 1470;
	TRect_CatCode.right			= 1750;

	RECT TRect_VEN;
	TRect_VEN.top				= 100;
	TRect_VEN.bottom			= 2000;
	TRect_VEN.left				= 1470;
	TRect_VEN.right				= 1750;

	RECT BcRect_CatCode;
	BcRect_CatCode.top			= 2000;
	BcRect_CatCode.bottom		= 3700;
	BcRect_CatCode.left			= 1730;
	BcRect_CatCode.right		= 1900;

	RECT TRect_VQOut;
	TRect_VQOut.top				= 100;
	TRect_VQOut.bottom			= 2000;
	TRect_VQOut.left			= 1720;
	TRect_VQOut.right			= 2010;
	//*************************************//


	//Open format file to read label setting
	bFileOpened = cfBarcodeFile.Open("C:\\MapSorter\\Exe\\LabelFormat.txt", 
		CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone|CFile::typeText);

	if ( bFileOpened == TRUE )
	{
		cfBarcodeFile.SeekToBegin();

		//check file header
		cfBarcodeFile.ReadString(szLineData);
		if ( szLineData == "[Dymo Printer Name]" )
		{
			//Read printer name
			cfBarcodeFile.ReadString(szLineData);
			szPrinterName = szLineData;
			
			//Read label name
			cfBarcodeFile.ReadString(szLineData);
			if ( szLineData == "[Label Name]" )
			{
				cfBarcodeFile.ReadString(szLineData);
				szLabelName = szLineData;
			}

			//Read Font Size
			cfBarcodeFile.ReadString(szLineData);
			if ( szLineData == "[Font]" )
			{
				cfBarcodeFile.ReadString(szLineData);
				//szFont		= szLineData.Left(szLineData.Find(","));
				//szLineData	= szLineData.Mid(szLineData.Find(",") + 1);
				//nFontSize	= atoi(szLineData);
			}

			//Empty Line
			cfBarcodeFile.ReadString(szLineData);

			//Content
			cfBarcodeFile.ReadString(szLineData);
			if ( szLineData == "[Content]" )
			{
				CString szData, szType;
				INT nPos;

				while (cfBarcodeFile.ReadString(szLineData))
				{
					if (szLineData == "[End of File]")
						break;
					if (szLineData == "")
					{
						continue;
					}

					szData	= szLineData;
					szType	= szData.Left(szData.Find(","));
					szData  = szData.Mid(szData.Find(",") + 1);
				
					if (szType == "B")			//Barcode
					{
						if ((nPos = szLineData.Find("BinTapeID")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("BinTapeID B", szData, BcRect_BinTapeID);
						}
						else if ((nPos = szLineData.Find("BinType")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("BinTypevB", szData, BcRect_BinType);
						}
						else if ((nPos = szLineData.Find("LotID")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("LotID B", szData, BcRect_LotID);
						}
						else if ((nPos = szLineData.Find("PName")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("PName B", szData, BcRect_PName);
						}
						else if ((nPos = szLineData.Find("OutputID")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("OutputID B", szData, BcRect_OutputID);
						}
						else if ((nPos = szLineData.Find("CatCode")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("CatCode B", szData, BcRect_CatCode);
						}
					}
					else if (szType == "T")		//Text
					{
						if ((nPos = szLineData.Find("BinTapeID")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("BinTapeID T", szData, TRect_BinTapeID);
						}
						else if ((nPos = szLineData.Find("BinType")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("BinType T", szData, TRect_BinType);
						}
						else if ((nPos = szLineData.Find("LotID")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("LotID T", szData, TRect_LotID);
						}
						else if ((nPos = szLineData.Find("PName")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("PName T", szData, TRect_PName);
						}
						else if ((nPos = szLineData.Find("QtyIn")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("QtyIn T", szData, TRect_QtyIn);
						}
						else if ((nPos = szLineData.Find("Block")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("Block T", szData, TRect_Block);
						}
						else if ((nPos = szLineData.Find("OutputID")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("OutputID T", szData, TRect_OutputID);
						}
						else if ((nPos = szLineData.Find("CatCode")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("CatCode T", szData, TRect_CatCode);
						}
						else if ((nPos = szLineData.Find("VEN")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("VEN T", szData, TRect_VEN);
						}
						else if ((nPos = szLineData.Find("VQOut")) != -1)
						{
							szData	= szLineData.Mid(nPos + 2);
							ReadRectFromLine("VQOut T", szData, TRect_VQOut);
						}
					}
				}
			}
		}

		cfBarcodeFile.Close();
	}

	//Define label name & rotation
	if (m_pPtrObj != NULL)
	{
   		m_pPtrObj->SelectLabel(szLabelName);
		m_pPtrObj->SetRotation(90);
	}

	if (m_pPtrObj != NULL)
	{
		m_pPtrObj->SetHAlign(0);
		m_pPtrObj->SetTextVAlign(1);
		m_pPtrObj->SetFont(szFont, nFontSize, TRUE, FALSE, FALSE, FALSE);
		m_pPtrObj->SetBCTextPos(0);
		m_pPtrObj->SetBCType(2);
		m_pPtrObj->SetBCSize(0);
	}
	
	BOOL bLabelFileHeader = FALSE;
	bLabelFileHeader = SaveLabelFileHeader(szPrinterName, szLabelName, 90, "Tahoma", 6, 2, 0);
	if(bLabelFileHeader == FALSE)
	{
		return FALSE;
	}

	CString szWorkNo;
	CString szUseBarcode;
	CString szMachineNo;
	CString szBinTapeID;
	BOOL	bUseBarcode;
	CString szCatCode, szOptionCode; 
	CString szPartName;		// from Map file -- BASEPART 
	CString szTapeType;		// Hard code --- Saw Tape
	CString szTileID;		// Wafer Barcode ID
	CString szSummaryBin;	// From SummaryFile --- Bin For Block
	UCHAR	ucSummaryBin;
	CString szQtyIn;		// Total dice of SummaryFil Bin
	ULONG ulQtyIn;

	//4.52D10 2nd scan as Ring ID
	CString szBarcodeRingId = m_smfSRam["WaferLoaderStn"]["2nd 1D Barcode"]; 

	GetOneRowExtraInformation(2,szPartName); // 2rd column gets the BASEPART as szPartName
	//GetOneRowExtraInformation(3,szCatCode); // 3rd column gets the CatCode
	GetOneRowExtraInformation(4,szOptionCode); // 4th column gets the Option Code
	GetWaferSubFolderSummaryFileContent(szSummaryBin, szCatCode); //Get the Bin and CatCode from Summary File

	if(szSummaryBin.IsEmpty() == FALSE)
	{
		ULONG ulPick=0, ulLeft=0, ulTotal=0;
		ucSummaryBin = atoi(szSummaryBin) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
		CMS896AStn::m_WaferMapWrapper.GetStatistics(ucSummaryBin, ulLeft, ulPick, ulTotal);
		ulQtyIn = ulTotal;
		szQtyIn.Format("%ld", ulQtyIn);
		//AfxMessageBox("Qty Summary from wafer:" + szQtyIn,MB_SYSTEMMODAL);
	}

	szTapeType		= "Saw Tape";

	//CString szWaferID;
	//CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferID);
	//szTileID = szWaferID;
	szTileID		= m_smfSRam["WaferTable"]["WT1InBarcode"];//4.53D78
	if(szTileID.IsEmpty() == TRUE) //4.53D82
	{
		szTileID		= m_smfSRam["WaferTable"]["PPLM_WT1InBarcode"];

		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Updated TileID :" +  szTileID);

		if(szTileID.IsEmpty() == TRUE)
		{
			CString szTitle		= "Please input map barcode: ";
			BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginPassword", FALSE, szTileID);
			if(!bStatus )
			{
				szMsg = "Error: TileID KeyIn Fail";
				SetErrorMessage(szMsg);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			}
			if( szTileID.IsEmpty() == TRUE)
			{
				szMsg = "Error: TileID KeyIn is empty";
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				SetErrorMessage(szMsg);
				HmiMessage(szMsg);
				szTileID = "NO_TileID";
			}
		}

		m_smfSRam["WaferTable"]["PPLM_WT1InBarcode"] = "";
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel --  Start Print Rebeal Label file");

	szWorkNo		= m_smfSRam["MS896A"]["Bin WorkNo"];
	
	szMachineNo		= GetMachineNo();
	bUseBarcode			= (BOOL)(LONG)m_smfSRam["BinLoader"]["UseBracode"];
	if (bUseBarcode == TRUE)
	{
		szUseBarcode = "1";
	}
	else
	{
		szUseBarcode = "0";
	}

	szWorkNo		= szWorkNo.TrimRight(" ");
	//szBinTapeID		= szWorkNo + "-" + szMachineNo + "-" + szUseBarcode;
	szBinTapeID		= szWorkNo + "-" + szMachineNo + "-" + szBarcodeRingId;

	//BinTape ID
	//TextRect.top		= 100;		
	//TextRect.left		= 120;	
	//TextRect.bottom	= 4800;	 
	//TextRect.right	= 400;
	TextRect.top	= TRect_BinTapeID.top;		
	TextRect.left	= TRect_BinTapeID.left;	
	TextRect.bottom	= TRect_BinTapeID.bottom;	 
	TextRect.right	= TRect_BinTapeID.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Bin Tape ID: " + szBinTapeID, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Bin Tape ID: " + szBinTapeID); 


	//<Barcoded Bin Tape ID>
	//BarcodeRect.top		= 100;
	//BarcodeRect.left		= 400;	
	//BarcodeRect.bottom	= 4800;	 
	//BarcodeRect.right		= 550;	
	BarcodeRect.top		= BcRect_BinTapeID.top;
	BarcodeRect.left	= BcRect_BinTapeID.left;	
	BarcodeRect.bottom	= BcRect_BinTapeID.bottom;	 
	BarcodeRect.right	= BcRect_BinTapeID.right;	
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddBarcode(szBinTapeID, BarcodeRect);
	SaveLabelFileData(BarcodeRect.top, BarcodeRect.left, BarcodeRect.bottom, BarcodeRect.right, FALSE, szBinTapeID); 


	//"Bin Type:" (OPTIONCODES)
	//TextRect.top		= 100;
	//TextRect.left		= 420;
	//TextRect.bottom	= 1250;
	//TextRect.right	= 700;
	TextRect.top	= TRect_BinType.top;
	TextRect.left	= TRect_BinType.left;
	TextRect.bottom	= TRect_BinType.bottom;
	TextRect.right	= TRect_BinType.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Bin Type: " + szOptionCode, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Bin Type: " + szOptionCode); 


	////<Barcoded "Bin Type:">	//v3.34
	////BarcodeRect.top		= 100;
	////BarcodeRect.left		= 680;
	////BarcodeRect.bottom	= 1350;
	////BarcodeRect.right		= 830;
	//BarcodeRect.top		= BcRect_BinType.top;
	//BarcodeRect.left	= BcRect_BinType.left;
	//BarcodeRect.bottom	= BcRect_BinType.bottom;
	//BarcodeRect.right	= BcRect_BinType.right;
	//if (m_pPtrObj != NULL)
	//	m_pPtrObj->AddBarcode(szOptionCode, BarcodeRect);
	//SaveLabelFileData(BarcodeRect.top, BarcodeRect.left, BarcodeRect.bottom, BarcodeRect.right, FALSE, szOptionCode);


	//"Tape Type:" 
	TextRect.top	= TRect_TapeType.top;
	TextRect.left	= TRect_TapeType.left;
	TextRect.bottom	= TRect_TapeType.bottom;
	TextRect.right	= TRect_TapeType.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Tape Type: " + szTapeType, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Tape Type: " + szTapeType); 

	////<Barcoded "Tape Type:">
	//BarcodeRect.top		= BcRect_TapeType.top;
	//BarcodeRect.left	= BcRect_TapeType.left;
	//BarcodeRect.bottom	= BcRect_TapeType.bottom;
	//BarcodeRect.right	= BcRect_TapeType.right;
	//if (m_pPtrObj != NULL)
	//	m_pPtrObj->AddBarcode(szTapeType, BarcodeRect);
	//SaveLabelFileData(BarcodeRect.top, BarcodeRect.left, BarcodeRect.bottom, BarcodeRect.right, FALSE, szTapeType);


	//v3.27T2
	//CString szLotNum = ptrQueryDatabase->GetOthers("Lot No");
	CString szLotNum = m_smfSRam["MS896A"]["LotNumber"];
	szLotNum.Replace("\t", "");
	szLotNum.Replace(" ", "");


	
	// Concate Lot# with the cat no
	//"Lot#:"
	//TextRect.top		= 2000;
	//TextRect.left		= 600;
	//TextRect.bottom	= 4800;
	//TextRect.right	= 880;
	TextRect.top	= TRect_LotID.top;
	TextRect.left	= TRect_LotID.left;
	TextRect.bottom	= TRect_LotID.bottom;
	TextRect.right	= TRect_LotID.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Lot#: " + szLotNum + "-" + szOptionCode + "-" + szCatCode, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Lot#: " + szLotNum + "-" + szOptionCode + "-" + szCatCode); 

	//<Barcoded "Lot#:">
	//BarcodeRect.top		= 100;		
	//BarcodeRect.left		= 870;
	//BarcodeRect.bottom	= 4800;
	//BarcodeRect.right		= 1050;
	BarcodeRect.top		= BcRect_LotID.top;		
	BarcodeRect.left	= BcRect_LotID.left;
	BarcodeRect.bottom	= BcRect_LotID.bottom;
	BarcodeRect.right	= BcRect_LotID.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddBarcode(szLotNum + "-" + szOptionCode + "-" + szCatCode, BarcodeRect);
	SaveLabelFileData(BarcodeRect.top, BarcodeRect.left, BarcodeRect.bottom, BarcodeRect.right, FALSE, szLotNum + "-" + szOptionCode + "-" + szCatCode); 

	//"PART Name:"
	//TextRect.top		= 2000;
	//TextRect.left		= 1020;
	//TextRect.bottom	= 4800;
	//TextRect.right	= 1300;
	TextRect.top	= TRect_PName.top;
	TextRect.left	= TRect_PName.left;
	TextRect.bottom	= TRect_PName.bottom;
	TextRect.right	= TRect_PName.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Part: " + szPartName, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Part: " + szPartName); 
	

	////<Barcoded "PART Name:">
	////BarcodeRect.top		= 2000;		
	////BarcodeRect.left		= 1300;
	////BarcodeRect.bottom	= 4800;
	////BarcodeRect.right		= 1450;
	BarcodeRect.top			= BcRect_PName.top;		
	BarcodeRect.left		= BcRect_PName.left;
	BarcodeRect.bottom		= BcRect_PName.bottom;
	BarcodeRect.right		= BcRect_PName.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddBarcode(szPartName, BarcodeRect);
	SaveLabelFileData(BarcodeRect.top, BarcodeRect.left, BarcodeRect.bottom, BarcodeRect.right, FALSE, szPartName); 

	//"Tile ID:" 
	TextRect.top	= TRect_TileID.top;
	TextRect.left	= TRect_TileID.left;
	TextRect.bottom	= TRect_TileID.bottom;
	TextRect.right	= TRect_TileID.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Tile ID: " + szTileID, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Tile ID: " + szTileID); 

	//<Barcoded "Tile ID:">  // change again 20161102 JC Wong
	//BarcodeRect.top		= BcRect_TileID.top;
	//BarcodeRect.left	= BcRect_TileID.left;
	//BarcodeRect.bottom	= BcRect_TileID.bottom;
	//BarcodeRect.right	= BcRect_TileID.right;
	//if (m_pPtrObj != NULL)
	//	m_pPtrObj->AddBarcode(szTileID, BarcodeRect);
	//SaveLabelFileData(BarcodeRect.top, BarcodeRect.left, BarcodeRect.bottom, BarcodeRect.right, FALSE, szTileID);



	////"Qty In:"
	//TextRect.top		= 4000;		
	//TextRect.left		= 1470;
	//TextRect.bottom	= 4800;
	//TextRect.right	= 1750;	
	TextRect.top	= TRect_QtyIn.top;		
	TextRect.left	= TRect_QtyIn.left;
	TextRect.bottom	= TRect_QtyIn.bottom;
	TextRect.right	= TRect_QtyIn.right;	
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Qty In: " + szQtyIn, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Qty In: " + szQtyIn); 


	////"Block:" 
	CString szBinBlkId	= szSummaryBin;	//Get from Summary file
	//TextRect.top		= 4000;
	//TextRect.left		= 1720;
	//TextRect.bottom	= 4800;
	//TextRect.right	= 2010;
	TextRect.top	= TRect_Block.top;
	TextRect.left	= TRect_Block.left;
	TextRect.bottom	= TRect_Block.bottom;
	TextRect.right	= TRect_Block.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Block: " + szBinBlkId, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Block: "  + szBinBlkId); 

	//// Calculate OutputID
	CString szOutputID = szBarcodeRingId;
	//<"Output ID:">
	//TextRect.top		= 100;		
	//TextRect.left		= 1020;
	//TextRect.bottom	= 2000;
	//TextRect.right	= 1300;
	TextRect.top	= TRect_OutputID.top;		
	TextRect.left	= TRect_OutputID.left;
	TextRect.bottom	= TRect_OutputID.bottom;
	TextRect.right	= TRect_OutputID.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Output ID: " + szOutputID, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Output ID: " + szOutputID); 

	////<Barcoded "OutputID">
	//BarcodeRect.top		= 100;
	//BarcodeRect.left		= 1300;
	//BarcodeRect.bottom	= 2000;
	//BarcodeRect.right		= 1450;	
	BarcodeRect.top		= BcRect_OutputID.top;
	BarcodeRect.left	= BcRect_OutputID.left;
	BarcodeRect.bottom	= BcRect_OutputID.bottom;
	BarcodeRect.right	= BcRect_OutputID.right;	
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddBarcode("L" + szOutputID, BarcodeRect);		//v3.24T1
	SaveLabelFileData(BarcodeRect.top, BarcodeRect.left, BarcodeRect.bottom, BarcodeRect.right, FALSE, "L" + szOutputID);	//v3.24T2

	//"Cat Code:"
	//TextRect.top		= 2000;		
	//TextRect.left		= 1470;
	//TextRect.bottom	= 3700;	 
	//TextRect.right	= 1750;
	TextRect.top	= TRect_CatCode.top;		
	TextRect.left	= TRect_CatCode.left;
	TextRect.bottom	= TRect_CatCode.bottom;	 
	TextRect.right	= TRect_CatCode.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("Cat Code: " + szCatCode, TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "Cat Code: " + szCatCode);
	
	//"V E/N:"
	//TextRect.top		= 100;		
	//TextRect.left		= 1470;	
	//TextRect.bottom	= 2000;	 
	//TextRect.right	= 1750;
	TextRect.top	= TRect_VEN.top;		
	TextRect.left	= TRect_VEN.left;	
	TextRect.bottom	= TRect_VEN.bottom;	 
	TextRect.right	= TRect_VEN.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("V E/N:______________" , TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "V E/N:______________"); 

	//<Barcoded "Cat Code:">
	//BarcodeRect.top		= 2000;
	//BarcodeRect.left		= 1730;
	//BarcodeRect.bottom	= 3700;
	//BarcodeRect.right		= 1900;
	BarcodeRect.top		= BcRect_CatCode.top;
	BarcodeRect.left	= BcRect_CatCode.left;
	BarcodeRect.bottom	= BcRect_CatCode.bottom;
	BarcodeRect.right	= BcRect_CatCode.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddBarcode(szCatCode, BarcodeRect);
	SaveLabelFileData(BarcodeRect.top, BarcodeRect.left, BarcodeRect.bottom, BarcodeRect.right, FALSE, szCatCode); 

	//"V Qty Out:"
	//TextRect.top		= 100;
	//TextRect.left		= 1720;
	//TextRect.bottom	= 2000;
	//TextRect.right	= 2010;
	TextRect.top	= TRect_VQOut.top;
	TextRect.left	= TRect_VQOut.left;
	TextRect.bottom	= TRect_VQOut.bottom;
	TextRect.right	= TRect_VQOut.right;
	if (m_pPtrObj != NULL)
		m_pPtrObj->AddText("V Qty Out:______________" , TextRect);
	SaveLabelFileData(TextRect.top, TextRect.left, TextRect.bottom, TextRect.right, TRUE, "V Qty Out:______________");

	//CString szPrintLabel = ptrQueryDatabase->GetOthers("Print Label");
	//v2.64 Lumileds DLA
	//Print label or just generate label file?
	CString szPrintLabel;
	if (CMS896AStn::m_bOfflinePrintLabel)		// Used in SETUP mode only
	{
		szPrintLabel = "FALSE";
	}
	else
	{
		szPrintLabel = "TRUE";
	}

	CString szOutputFilePath, szLabelFileName, szSubFolder;

	//szSubFolder		= m_smfSRam["MS896A"]["MapSubFolderName"];
	szSubFolder			= m_smfSRam["MS896A"]["Bin WorkNo"]; //change again by JC Wong CTM 20161128

	if( szSubFolder.IsEmpty() == FALSE)
	{
		szLabelFileName = m_szOutputFilePath + "\\" + szSubFolder ;
		CreateDirectory(szLabelFileName, NULL);
		szLabelFileName	 = szLabelFileName + "\\" + m_szLabelFileBarcode + ".lbe";
	}
	else
	{
	   szLabelFileName = m_szOutputFilePath + "\\" + m_szLabelFileBarcode + ".lbe";
	}
	
	szMsg.Format("WaferLabel --  Bp Path of label file :%s, \n\t Output Path of label file :%s",m_szBpLabelFileName , szLabelFileName);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	if( szLabelFileName != m_szBpLabelFileName)
	{
		CopyFile(m_szBpLabelFileName,	szLabelFileName,	FALSE);
	}
	//if ( ((szMachineType == "2") || (szMachineType == "3")) && 			//DL
	//	 (szPrintLabel == "FALSE") )		// Use bkgd printing in EXE
	//{
	//	//v2.64
	//	// Just copy label file to default folder for label print in background task
	//	CopyFile(m_szLabelFileName, "c:\\MapSorter\\Exe\\ms899.lbe", FALSE);
	//}
	//else
	//{
	//	//Print label
	//	if (m_pPtrObj != NULL)
	//		m_pPtrObj->PrintLabel(szPrinterName);
	//	else
	//		return FALSE;
	//}

	return TRUE;
}



BOOL CMS896AApp::SaveLabelFileHeader(CString szPrinter, CString szLabelName, int nAngle, CString szFont, int nFontSize, int nBarcodeType, int nBarcodeSize)
{
	//Remove Labelfile
	//remove(m_szLabelFileName);
	//remove(m_szBpLabelFileName);]

	CString szYear, szMonth, szDay, szHour, szMinute;
	int nYear, nMonth, nDay, nHour, nMinute;
	CString szMsg;

	CTime theTime;
		//Date/time
	theTime = CTime::GetCurrentTime();
	//Year
	nYear = theTime.GetYear();
	szYear.Format("%d", nYear);

	//Month
	nMonth = theTime.GetMonth();
	szMonth.Format("%d", nMonth);
	if (nMonth < 10)
		szMonth = "0" + szMonth;

	//Day
	nDay = theTime.GetDay();
	szDay.Format("%d", nDay);
	if (nDay < 10)
		szDay = "0" + szDay;
	
	//Hour
	nHour = theTime.GetHour();
	szHour.Format("%d", nHour);
	if (nHour < 10)
		szHour = "0" + szHour;

	//Minute
	nMinute = theTime.GetMinute();
	szMinute.Format("%d", nMinute);
	if (nMinute < 10)
		szMinute = "0" + szMinute;

	CString szOBarcode   = "Wafer label";
	CString szWaferIdName = "";
	CString szSubFolder	 = "";
	CString szBpLabelFilePath = "";
	//szWaferIdName = CMS896AStn::m_szMapFileName;
	szWaferIdName   = m_smfSRam["WaferLoaderStn"]["2nd 1D Barcode"]; //ring ID as wafer ID
	//szSubFolder		= m_smfSRam["MS896A"]["MapSubFolderName"];
	szSubFolder			= m_smfSRam["MS896A"]["Bin WorkNo"];

	//4.52D10Name 
	if( szWaferIdName.IsEmpty() == FALSE)
	{
		szOBarcode = szWaferIdName;
	}
	else
	{
		szOBarcode			 = szOBarcode + szYear + szMonth + szDay + szHour + szMinute;
	}

	m_szLabelFileBarcode = szOBarcode;

	if( szSubFolder.IsEmpty() == FALSE)
	{
		szBpLabelFilePath = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\" + szSubFolder ;
		CreateDirectory(szBpLabelFilePath, NULL);
		m_szBpLabelFileName	 = szBpLabelFilePath + "\\" + szOBarcode + ".lbe";
	}
	else
	{
	   m_szBpLabelFileName	 = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin\\" + szOBarcode + ".lbe";
	}

	if(_access(m_szBpLabelFileName,0) != -1)
	{
		szMsg.Format("Wafer Label -- the wafer label file exist:%s",m_szBpLabelFileName );
		//HmiMessage(szMsg);
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		return FALSE;
	}

	FILE * fLabelFile = NULL;
	errno_t nErr = fopen_s(&fLabelFile, m_szBpLabelFileName, "a+");		//v3.24T2
	if ((nErr == 0) && (fLabelFile != NULL))
	{
		//Signature
		fprintf(fLabelFile, "[MS899 Label File]\n");

		//Printer model, paper & angle
		fprintf(fLabelFile, "%s\n", szPrinter);
		fprintf(fLabelFile, "%s\n", szLabelName);
		fprintf(fLabelFile, "%d\n", nAngle);

		//Text Font & size
		fprintf(fLabelFile, "%s\n", szFont);
		fprintf(fLabelFile, "%d\n", nFontSize);

		//Barcode type & size
		fprintf(fLabelFile, "%d\n", nBarcodeType);
		fprintf(fLabelFile, "%d\n", nBarcodeSize);

		fclose(fLabelFile);

		return TRUE;
	}
	else
	{
		szMsg = "WaferLabel -- Error: Label file null!";
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	    return FALSE;
	}

	
}

BOOL CMS896AApp::ReadRectFromLine(CString szLabel, CString szData, RECT& rect)
{
	CString szLineData = szData;
	INT nTop, nBottom, nLeft, nRight;

	szLineData  = szLineData.Mid(szLineData.Find(",") + 1);
	nTop		= atoi( szLineData.Left(szLineData.Find(",")) );
	
	szLineData	= szLineData.Mid(szLineData.Find(",") + 1);
	nBottom		= atoi( szLineData.Left(szLineData.Find(",")) );			
	
	szLineData	= szLineData.Mid(szLineData.Find(",") + 1);
	nLeft		= atoi( szLineData.Left(szLineData.Find(",")) );
	
	szLineData	= szLineData.Mid(szLineData.Find(",") + 1);
	nRight		= atoi((LPCTSTR)szLineData);

CString szMsg;
szMsg.Format(" : %d, %d, %d, %d", nTop, nBottom, nLeft, nRight);
AfxMessageBox(szLabel + szMsg, MB_SYSTEMMODAL);

	rect.top	= nTop;
	rect.bottom = nBottom;
	rect.left	= nLeft;
	rect.right	= nRight;

	return TRUE;
}

BOOL CMS896AApp::SaveLabelFileData(int x1, int y1, int x2, int y2, BOOL bFormat, CString szText)
{
	FILE * fLabelFile = NULL;

	errno_t nErr = fopen_s(&fLabelFile, m_szBpLabelFileName, "a+");		//v3.24T2
	if ((nErr == 0) && (fLabelFile != NULL))
	{
		if ( bFormat == TRUE )
		{
			fprintf(fLabelFile, "T:%d,%d,%d,%d,%s\n", x1, y1, x2, y2, szText);
		}
		else
		{
			fprintf(fLabelFile, "B:%d,%d,%d,%d,%s\n", x1, y1, x2, y2, szText);
		}

		fclose(fLabelFile);

		return TRUE;
	}

	return FALSE;
}

//4.52D10 sub-fnc
BOOL CMS896AApp::GetOneRowExtraInformation(INT Column, CString & szMapHeaderRawData) 
{
	CStdioFile cfWaferMapFile;
	CString szLocalFileName, szLogMsg, szWaferId;
	CString szElectricInfo = "";
	BOOL bMapFileOpened;
	CStringArray szaRawDataByCol;
	szaRawDataByCol.RemoveAll();

	if ( CMS896AStn::m_WaferMapWrapper.GetReader() == NULL )
	{
		//AfxMessageBox("The WaferMapReader is NULL", MB_SYSTEMMODAL); 
		szLogMsg = "Wafer Label -- The Wafer Map read fail!";
		
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		HmiMessage(szLogMsg); 
		return FALSE;
	}

	if ( !CMS896AStn::m_WaferMapWrapper.IsMapValid() )
	{
		//AfxMessageBox("The WaferMap is NOT Vaild", MB_SYSTEMMODAL);
		szLogMsg = "Wafer Label -- The Wafer Map is NOT Vaild!";

		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		HmiMessage(szLogMsg); 
		return FALSE;
	}


	szLocalFileName = MS_LCL_CURRENT_MAP_FILE;

	bMapFileOpened = cfWaferMapFile.Open(szLocalFileName, CFile::modeRead|CFile::shareDenyNone);
	if (bMapFileOpened == TRUE) 
	{
		unsigned long ulNumOfRows=0, ulNumOfCols=0;
		CUIntArray aulSelectedGradeList;

		CMS896AStn::m_WaferMapWrapper.GetMapDimension(ulNumOfRows, ulNumOfCols);
		CMS896AStn::m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
		CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferId);

		szLogMsg.Format("WaferLabel -- ID:%s, Loop Extra Info, MapDimension(%ld,%ld)",szWaferId, ulNumOfRows, ulNumOfCols);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);

		for(int i=0; i< (LONG)ulNumOfRows; i++)  //4.53D76
		{
			for(int j=0; j < (LONG)ulNumOfCols; j++)
			{
		//		ucGetGrade		= CMS896AStn::m_WaferMapWrapper.GetGrade(i, j);
		//		ucGetNullBin	= CMS896AStn::m_WaferMapWrapper.GetNullBin();
		//		bReferenceDie	= CMS896AStn::m_WaferMapWrapper.IsReferenceDie(i, j);
		//	
		//		szLogMsg.Format("Loop Grade:%d, Bin:%d,  bRefDie:%d",ucGetGrade,ucGetBullBin, bReferenceDie);
		//		CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg + "\n");

		//		if( ucGetGrade != ucGetNullBin && bReferenceDie==FALSE )
		//		{
		//			for(int k=0; k<aulSelectedGradeList.GetSize(); k++)
		//			{
		//				ucGetSelectedGradeList = aulSelectedGradeList.GetAt(k);
		//				szLogMsg.Format("Loop Selected Grade:%d", ucGetSelectedGradeList);
		//				CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg + "\n");

		//				if( ucGetGrade == ucGetSelectedGradeList )
		//				{
							//only use the data of (1,1) in Map file
							//lMapRow = 1; lMapCol = 1;
							if(CMS896AStn::m_WaferMapWrapper.GetExtraInformation(&cfWaferMapFile, i, j, szElectricInfo) == FALSE)
							{
								szLogMsg.Format("WaferLabel -- Get Extra Information failed, Id:%s (%d,%d)", szWaferId, i, j);
								CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
								SetErrorMessage(szLogMsg);
							}
							else
							{
								szLogMsg.Format("WaferLabel -- Loop Electric Info:%s", szElectricInfo);
								CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
							}

							if(szElectricInfo.IsEmpty() == FALSE)
								break;
		//				}
		//			}
		//		}
			}

			if(szElectricInfo.IsEmpty() == FALSE)
						break;
		}

		//if( szElectricInfo.Empty() != NULL )
		if (szElectricInfo.IsEmpty() == FALSE)
		{
			ParseRawDataTab(szElectricInfo, szaRawDataByCol);
			if( szaRawDataByCol.GetSize()>=1 && Column < szaRawDataByCol.GetSize() )
			szMapHeaderRawData = szaRawDataByCol.GetAt(Column);
		}
		else
		{
			szLogMsg = "WaferLabel -- The electric Info is empty, Please check the Map file!";
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			//HmiMessage(szLogMsg);  //4.53D76
			SetErrorMessage(szLogMsg);
		}
		cfWaferMapFile.Close();
	}
	else
	{
		szLogMsg =" Failed to open CurrentMap.txt";
		SetErrorMessage(szLogMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg);
	}

	return TRUE;
}


BOOL CMS896AApp::ParseRawDataTab(CString szRawData, CStringArray& szaRawDataByCol)	//	data with tab into string array;
{
	CString szData;
	INT nCol = -1;
	CString szMsg;

	szMsg.Format("WaferLabel -- Get Parse Raw Data: %s",szRawData);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	while((nCol = szRawData.Find("\t")) != -1)
	{
		szData = szRawData.Left(nCol);
		szRawData = szRawData.Right(szRawData.GetLength() - nCol -1);
		szMsg.Format("WaferLabel -- Get Raw Data: %s",szData);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		szaRawDataByCol.Add(szData);
	}	

	// add the last item
	if (szRawData != "")
	{
		szMsg.Format("WaferLabel -- Get Raw Data: %s",szRawData);
	    CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		szaRawDataByCol.Add(szRawData);
	}

	return TRUE;
}

BOOL CMS896AApp::ParseRawDataComma(CString szRawData, CStringArray& szaRawDataByCol)	//	data with , into string array;
{
	CString szData;
	INT nCol = -1;

	while((nCol = szRawData.Find(",")) != -1)
	{
		szData = szRawData.Left(nCol);
		szRawData = szRawData.Right(szRawData.GetLength() - nCol -1);
		szaRawDataByCol.Add(szData);
	}

	// add the last item
	if (szRawData != "")
	{
		szaRawDataByCol.Add(szRawData);
	}

	return TRUE;
}

//4.52D10 sub-fnc
BOOL CMS896AApp::GetWaferSubFolderSummaryFileContent(CString &szBinHeaderData, CString &szCatCodeHeaderData)
{

	CString  szUserFile;
	CString	 szContents;
	BOOL bFileOK = FALSE;
	CStdioFile cfSelectionFile;
	INT nCol;
	CString szMzg;

	CString szMapPath = m_szMapFilePath;
	//m_smfSRam["MS896A"]["MapFilePath"];
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szSubFolder		= m_smfSRam["MS896A"]["MapSubFolderName"];
	//CString szSubFolder			= m_smfSRam["MS896A"]["Bin WorkNo"]; //change again by JC Wong 20161219
	CString szWaferLabelSummaryFileName = "";
	//Add subfolder if sub-folder exist
	if ( szSubFolder.IsEmpty() == FALSE )	
	{
		//szMapPath = m_szMapFilePath + "\\" + szSubFolder + "\\summary_file.txt";
		//szMapPath = m_szMapFilePath + "\\" + szSubFolder;
		szWaferLabelSummaryFileName = szSubFolder + ".txt";
		szMapPath = m_szMapFilePath + "\\" + szSubFolder + "\\" + szWaferLabelSummaryFileName;

		szMzg.Format("WaferLabel -- Map Path: %s ", szMapPath);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMzg);
	}
	else
	{
		szMzg.Format("WaferLabel -- Map path %s, cannot find Sub folder name", m_szMapFilePath);
		
		CMSLogFileUtility::Instance()->WL_LogStatus(szMzg);
		SetErrorMessage(szMzg);
		HmiMessage(szMzg);
	}

	szUserFile	= szMapPath ;

	//Check file format is valid is correct or not
	bFileOK = cfSelectionFile.Open(szUserFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText);

	if (bFileOK	== TRUE)
	{
		cfSelectionFile.SeekToBegin();
		cfSelectionFile.ReadString(szContents);

		if ( szContents.Find("BIN,CATCODE,QUANTITY") == -1 )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- There is not BIN,CATCODE,QUANTITY ");
			bFileOK = FALSE;
		}

		cfSelectionFile.ReadString(szContents);

		nCol = szContents.Find(",");
		if(nCol != -1)
		{
			szBinHeaderData = szContents.Left(nCol);
			szContents = szContents.Right(szContents.GetLength() - nCol - 1);
		}
		nCol = szContents.Find(",");

		if(nCol != -1)
		{
			szCatCodeHeaderData = szContents.Left(nCol);
		}

		cfSelectionFile.Close();
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Fail Open Summary File");
	}

	szMzg.Format("WaferLabel -- Summary File Data -- bFileOK:%d, BIN:%s, CATCODE:%s, MapPath:%s",bFileOK, szBinHeaderData, szCatCodeHeaderData, szUserFile);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMzg);

	if (bFileOK == TRUE)
	{

		return TRUE;
	}
	else
	{
		
		return FALSE;
	}

	return TRUE;
}

//4.53D5 fnc VP KeyIn
VOID CMS896AApp::SetVPValueKeyIn()
{ 
	CString szMsg;
	szMsg ="Start KeyIn VP value by next step";
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	HmiMessage(szMsg);
	CString szVPValue;
	CString szTitle	= _T("Please input VP Value of WaferID.");
	BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginPassword", FALSE, szVPValue);
	if(!bStatus )
	{
		SetErrorMessage("Error: VP KeyIn Fail");
	}
	if( szVPValue.IsEmpty() == TRUE)
	{
		szMsg = "Error: VP KeyIn is empty";
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		SetErrorMessage(szMsg);
		HmiMessage(szMsg);
		szVPValue = "NO_VP";
	}	
	m_smfSRam["MS896A"]["SummaryFile"]["VPValue"] = szVPValue;
	szMsg.Format("VP value: %s",szVPValue );
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
}

//4.52D10Name 
BOOL CMS896AApp::BackUpMapFileToOutputPath(CString szExistMapFilePathName)
{

	CString szWaferLabelWithNewBackupMap, szOutputPath, szTempMsg, szRemainderDieMapFileName, szSubFolder;
	CString szBkUpWaferOutputMapPath, szBkUpWaferOutputMapName ;

	szOutputPath				= m_smfSRam["MS896A"]["OutputFilePath"];
	//szRemainderDieMapFileName = CMS896AStn::m_szMapFileName;
	szRemainderDieMapFileName   = m_smfSRam["WaferLoaderStn"]["2nd 1D Barcode"]; //ring ID as wafer ID
	//szSubFolder					= m_smfSRam["MS896A"]["MapSubFolderName"];
	szSubFolder					= m_smfSRam["MS896A"]["Bin WorkNo"];

	szBkUpWaferOutputMapPath = "c:\\MapSorter\\UserData\\OutputFile\\ClearBin";

	if(szOutputPath.IsEmpty() == FALSE)
	{
		if(szRemainderDieMapFileName.IsEmpty() == FALSE)
		{
			if(szSubFolder.IsEmpty() == FALSE)
			{
				szWaferLabelWithNewBackupMap = szOutputPath + "\\" + szSubFolder;
				szBkUpWaferOutputMapName  = szBkUpWaferOutputMapPath + "\\" + szSubFolder;

				if ( CreateDirectory(szWaferLabelWithNewBackupMap, NULL) == 0 )
				{
					if ( GetLastError() != ERROR_ALREADY_EXISTS )
					{
						SetErrorMessage("MS*: Backup Map fails with invalid Output path: " + szWaferLabelWithNewBackupMap);	//v3.89
						//SetAlert_Msg(IDS_BT_INVALID_DIRECTORY, szBackupMapPath);
						return FALSE;
					}
				}

				if ( CreateDirectory(szBkUpWaferOutputMapName, NULL) == 0 )
				{
					if ( GetLastError() != ERROR_ALREADY_EXISTS )
					{
						SetErrorMessage("MS**: Backup Map fails with invalid backup path: " + szBkUpWaferOutputMapName);	//v3.89
						//SetAlert_Msg(IDS_BT_INVALID_DIRECTORY, szBackupMapPath);
						return FALSE;
					}
				}

				szWaferLabelWithNewBackupMap = szWaferLabelWithNewBackupMap + "\\" + szRemainderDieMapFileName + ".txt";
				szBkUpWaferOutputMapName	 = szBkUpWaferOutputMapName + "\\" + szRemainderDieMapFileName + ".txt";
			}
			else
			{
				szWaferLabelWithNewBackupMap = szOutputPath + "\\" + szRemainderDieMapFileName + ".txt";
				szBkUpWaferOutputMapName = szBkUpWaferOutputMapPath + "\\" + szRemainderDieMapFileName + ".txt";
			}
			
			

			szTempMsg.Format("WaferLabel -- Save Bk Map to Output Path: %s , \n\t another Bk Path %s", szWaferLabelWithNewBackupMap, szBkUpWaferOutputMapName);
			CMSLogFileUtility::Instance()->WL_LogStatus(szTempMsg);	

			// Back up to the server path subfolder (output file path with adding a subfolder)
			if( szExistMapFilePathName.CompareNoCase(szWaferLabelWithNewBackupMap)!=0 )
			{
				CopyFile(szExistMapFilePathName, szWaferLabelWithNewBackupMap, FALSE);
			}

			// Back up to the local path subfolder (Back up path with adding a subfolder)
			if( szExistMapFilePathName.CompareNoCase(szBkUpWaferOutputMapName)!=0 )
			{
				CopyFile(szExistMapFilePathName, szBkUpWaferOutputMapName, FALSE);
			}
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Error: Empty Map file name");
			return FALSE;
		}
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Error: Empty Output path");	
		return FALSE;
	}

	return TRUE;

}

BOOL CMS896AApp::CheckDebSortMode(ULONG ulBit9)
{
	if (ulBit9 & 0x10)	//Feature #2: DEB Sort
	{
		//Enable Diamond Die Bonding fcn
		CMS896AStn::m_bEnableDEBSort = TRUE;		//Silan		//v4.50A25
	}
	else
	{
		CTime cTime;
		cTime = CTime::GetCurrentTime();
		//Request by AlexYu				//12/09/2016	//v4.55A8
		//Request by Siu&Andy&Couthie	//12/09/2016	//v4.55A8
		//LeoLam	//09/9/2015		//v4.51A2
		if(	(GetCustomerName().Find(CTM_SANAN) != -1)			|| 
			(GetCustomerName().Find("Electech3E") != -1)	||
			(GetCustomerName().MakeUpper().Find("CHANGELIGHT") != -1)||
			(GetCustomerName() == CTM_SEMITEK)				||
			(GetCustomerName() == "NanoJoin")				||
			(GetCustomerName() == "ZVision")				||
			m_bEnableDEBFcn2016		)
		{
			if ( (cTime.GetYear() <= 2027) )	//Extend Exp. date to end of 2017 
			{
				CMS896AStn::m_bEnableDEBSort = TRUE;
			}
		}
	}

	return TRUE;
}	//	check deb sort mode

BOOL CMS896AApp::CheckPasswordToGo(LONG lUserTpye)	//Matthew20181205
{
	LONG lAccessLevel = CheckLoginLevel();
	BOOL bCancel = FALSE;

	if (lAccessLevel > 0)
	{
		return TRUE;
	}
	else
	{
		for(int i=0; i<3; i++)
		{
			if (CheckSananPasswordFile(TRUE, bCancel, lUserTpye) == TRUE)
			{
				return TRUE;
			}
			else if(bCancel)
			{
				return FALSE;
			}
			else if(i == 2)
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}