/////////////////////////////////////////////////////////////////
// MS896A.h : main header file for the MS896A application
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, June 04, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "InitOperation.h"
#include "StringMapFile.h"
#include "TakeTime.h"
#include "BondResult.h"
#include "MS896AStn.h"
#include "GenerateDatabase.h"
#include "GenerateWaferDatabase.h"
#include "QueryWaferDatabase.h"
#include "DevFile.h"
#include "LogFileUtil.h"
#include "EquipTime.h"
#include "MESConnector.h"
#include "ToolsUsageRecord.h"
#include "AmiFileDecode.h"
#include "NichiaMapHeaderConstant.h"
#include <Winspool.h>

typedef struct
{
	CString szAlarmCode;
	CString szDecription;
	LONG lAction;

} ALARM_INFO;


typedef struct
{
	LONG lDiePixelCenterX;
	LONG lDiePixelCenterY;
	LONG lDieEncCenterX;
	LONG lDieEncCenterY;
	DOUBLE dDieRotation;
	DOUBLE dDieScore;
}
SingleDieInfo;


typedef struct
{
	LONG lNumberOfDie;
	SingleDieInfo stDieInfo[200];
}
MultiSearchStruct;

#define	HMI_SECURITY_PW_ADM			"Admin"
#define	HMI_SECURITY_PW_SER			"Servi"
#define	HMI_SECURITY_PW_SUP			"Super"
#define	HMI_SECURITY_PW_ENG			"Engin"
#define	HMI_SECURITY_PW_TEC			"Techn"

class CPrGeneral;
class CHostComm;
class CWaferTable;
class CBondHead;
class CBinTable;
class CWaferPr;
class CBondPr;
class CSecsComm;
class CWaferLoader;
class CSafety;
class CBinLoader;
class CNVCLoader;


class CMS896AApp : public SFM_CModule
{
private:
	// Critical section
	static CCriticalSection	m_csDebugLog;
	static CCriticalSection	m_csCycleLog;


private:
	CString		m_szStatusMessage;			// Status message for Title section
	CString		m_szMeasurementUnit;		// Measurement unit used in the machine
	FILE		*m_pCycleLog;				// Cycle Log

	BOOL m_bStopLoadUnloadLoopTest;
	LONG m_lCurTime;

	CWaferTable *m_pWaferTable;
	CBondHead	*m_pBondHead;
	CBinTable	*m_pBinTable;
	CWaferPr	*m_pWaferPr;
	CBondPr		*m_pBondPr;
	CSecsComm	*m_pSecsComm;
	CWaferLoader *m_pWaferLoader;
	CSafety		*m_pSafety;
	CBinLoader	*m_pBinLoader;
	CPrGeneral	*m_pPrGeneral;
	CNVCLoader	*m_pNVCLoader;

//Attribute
protected:
	enum {NULL_OP, AUTO_OP, MANUAL_OP, CYCLE_OP, DEMO_OP};
	enum {MOTION_RUNNING, MOTION_COMPLETE, MOTION_ERROR};

	LONG m_lTime;
	BOOL m_bChangeMaxCountWithAmi;
	BOOL m_bEverVisionUser;
	CString	m_szGroupID;
	CString	m_szDataPath;

	// Message Log variables
	CString	m_szLogFile;
	CString	m_szLogFilePath;
	BOOL	m_fLogFileCreated;
	BOOL	m_fLogMessage;
	BOOL	m_fCanDisplay;
	BOOL	m_fStopMsg;
	CMutex	m_csDispMsg;

	//State
	CString	m_szState;	// Main state name
	INT		m_qState;	// Main state
	INT		m_nOperation;
	LONG	m_lAction;

	BOOL	m_bPreStartCycleOK;		//v4.59A16	//Osram MS109 EJT talbe collision when pressing START
	
	//Flag
	BOOL	m_fDeleteMarkIDE;
	BOOL	m_fExecuteHmi;
	BOOL	m_bRunState;
	BOOL	m_fAutoStart;
	BOOL	m_bHmiExecuted;
	
	//HW Dialog attributes
	BOOL	m_fEnableHardware;
	BOOL	m_bDisableBHModule;
	BOOL	m_bDisableBTModule;
	BOOL	m_bDisableBLModule;
	BOOL	m_bDisableNLModule;				//andrewng //2020-0708
	BOOL	m_bDisableWTModule;
	BOOL	m_bDisableWLModule;
	BOOL	m_bDisableWLModuleWithExp;

	//Software/ Machine info
	CString m_szOSImageReleaseNo;
	CString m_szSoftVersion;
	CString m_szMachineModel;
	CString m_szSoftwareReleaseDate;
	CString m_szSoftwareReleaseTime;
	CString m_szMachineName;		//v4.40T1	//Machine Prefix //Nichia MS100+
	CString m_szMachineNo;
	CString m_szSWReleaseNo;
	CString m_szHiPECVersion;
	CString m_szHiPECVersion2;		//v3.65
	CString	m_szProductID;			//v3.24T1	//PLLM Traceability
	CString m_szLotNumberPrefix;	//v4.35T1	//PLLM Rebel/Flash
	CString m_szRecipeFilePath;		//v4.42T3	//PLLM Flash by KY Tan
	CString m_szShiftID;			//v4.48A21	//Avago

	CString m_szBpLabelFileName;	
	CString m_szLabelFileBarcode;

	//Lot Info
	CString m_szBinLotDirectory;
	CString m_szLotNumber;
	CString m_szLotStartTime;
	CString m_szLotEndTime;
	BOOL	m_bStartNewLot;			//v2.71 DLA

	//Map Lot No
	CString m_szMapLotNumber;

	//Bin table data
	CString m_szBinSpecVersion;

	// Bin Lot Info
	CString m_szBinWorkNo;
	CString m_szBinFileNamePrefix;
	CString m_szMapSubFolderName;

	BOOL	m_bIsAvago;				//v4.48A21
	BOOL	m_bSaveMapFile;
	BOOL	m_bIsNichia;			//v4.40T1
	// PLLM Die Fab Start Lot Info
	BOOL	m_bIsPLLMDieFab;
	BOOL	m_bIsPLLMLumiramic;		//v4.35
	BOOL	m_bIsPLLMRebel;			//v4.35
	CString m_szPLLMDieFabMESLot;

	BOOL	m_bIsElectech3E;
	BOOL	m_bIsYealy;				//v4.46T6
	BOOL	m_bIsRenesas;			//v4.59A12	//MS90

	// Machine Time
	CString m_szMachineBondedUnit;
	CString m_szMachineUpTime;
	CString m_szMachineRunTime;
	CString m_szMachineDownTime;
	CString m_szMachineAlarmTime;
	CString m_szMachineAssistTime;
	CString m_szMachineSetupTime;
	CString m_szMachineWaitingSetupTime; //v4.51D5 SanAn(XA) 14
	CString m_szMachineIdleTime;
	CString m_szMachineAlarmCount;
	CString m_szMachineAssistCount;
	UCHAR	m_ucEqTimeReportMode;
	BOOL	m_bEqTimeAutoReset;
	BOOL	m_bEqTimeAutoResetDieCounts;	//v2.83T2

	//Bond info
	CString m_szBondStartDate;
	CString m_szBondStartTime;
	CString m_szBondStopDate;
	CString m_szBondStopTime;

	int m_nMapStartYear;
	int m_nMapStartMonth;
	int m_nMapStartDay;
	int m_nMapStartHour;
	int m_nMapStartMinute;
	int m_nMapStartSecond;
	int m_nMapRunTime;

	BOOL	m_bCycleNotStarted;		// Flag to indicate that auto-cycle is not started (For enabling HMI menu items)
	BOOL	m_bDialogDeviceFile;	// Browse device file via dialog box	
	BOOL	m_bLoadPkgWithBin;		// Load package file within bin block
	BOOL	m_bResetBinSerialNo;	// Reset Bin serial no
	BOOL	m_bFixDevicePath;		// Fix device path for operator	
	BOOL	m_bCycleFKey;			// Flag to control Function Key enable
	BOOL	m_bShortCutKey;
	BOOL	m_bChangeBinLotNo;
	BOOL	m_bNewLotStarted;
	BOOL	m_bPpkgIncludeDelay;
	BOOL	m_bPpkgIncludePrescanSettings;
	BOOL	m_bPpkgIncludeRegionSettings;
	BOOL	m_bPpkgIncludePickSettingWODelay;
	// User account flag
	BOOL	m_bTechnician;
	BOOL	m_bEngineer;
	BOOL	m_bAdministrator;
	BOOL	m_bFirstExecute;
	CString	m_szUsername;
	CString m_szTechnicianPassword;	//v4.38T5	//PLLM Lumiramic
	CString m_szEngineerPassword;
	CString m_szAdminPassword;
	CString m_szPLLMRebelPassword;	//v3.70T3	//Special password for PLLM REBEL line
	CString m_szPasswordFilePath;	//v4.17T7	//SanAn password file fcn
	CString m_szUserLogOn;

	// customized hmi security check access level password.
	CString m_szHmiAccessPwAdmin;
	CString m_szHmiAccessPwService;
	CString m_szHmiAccessPwSuper;
	CString m_szHmiAccessPwEngineer;
	CString m_szHmiAccessPwTechnician;
	LONG	m_lInUseAccessLevel;
	LONG	m_lHmiAccessLevel;

	CDialog	*m_pLoadingDialog;		// Pointer to loading dialog
	RECT	m_stWinRect;			// Window dimension

	// For Feature enable/disable
	BOOL	m_bEnableDEBFcn2016;		//v4.53A22
	LONG	m_lDelayBeforeTurnOffEJVac;
	LONG	m_lCameraBlockDelay;
	LONG	m_lRTUpdateMissingDieThreshold;
	BOOL	m_bEnablePreBond;			//v3.80		//Cree PreBOnd PAD alignment fcn
	BOOL	m_bEnablePostBond;
	BOOL	m_bEnableRefDie;
	BOOL	m_bEnableCharRefDie;
	BOOL	m_bEnableAoiOcrDie;
	BOOL	m_bEnableThermalEjector;
	BOOL	m_bEnableColorCamera;
	BOOL	m_bDisplaySortBinItemMsg;
	BOOL	m_bSearchCompleteFilename;
	BOOL	m_bLoadMapSearchDeepInFolder;
	BOOL	m_bEnableSCNFile;
	BOOL	m_bEnableBatchIdFileCheck;
	BOOL	m_bDisableWaferMapFivePointCheckMsgSelection;
	BOOL	m_bEnablePsmFileExistCheck;
	BOOL	m_bEnableBlockFunc;
	LONG	m_lBlockPickMode;			//v3.28T1
	BOOL	m_bEnableSepGrade;
	BOOL	m_bEnableEmptyCheck;		//Block2
	BOOL	m_bMapIgnoreGrade0;			//v3.28T1	//For Lumileds Lumiramic
	BOOL	m_bEnableBinMap;			//v4.03		//PLLM REBEL binmap support
	BOOL	m_bEnableManualInputBC;		//v4.06
	BOOL	m_bEnableWPRBackLight;		//v4.46T28	//Cree HuiZhou

	BOOL	m_bEnableOPMenuShortcut;	//v4.19T1
	BOOL	m_bEnableBHShortcut;
	BOOL	m_bEnableWPRShortcut;
	BOOL	m_bEnableBPRShortcut;
	BOOL	m_bEnableBinBlkShortcut;
	BOOL	m_bEnableWTShortcut;
	BOOL	m_bEnablePRRecordShortcut;
	BOOL	m_bEnableBinTableShortcut;
	BOOL	m_bDisablePKGShortcut;
	BOOL	m_bEnablePicknPlaceShortcut;	//v4.20
	BOOL	m_bEnableOptionsTabShortcut;
	BOOL	m_bDisableOptionsShortcut;
	BOOL	m_bEnableSystemServiceShortcut;
	BOOL	m_bEnableStepMoveShortcut;
	BOOL	m_bEnablePhyBlockShortcut;
	BOOL	m_bEnableOperationModeShortcut;
	BOOL	m_bAllowMinimizeHmi;
	BOOL	m_bToggleWaferResortScanMode;
	BOOL	m_bEnableWaferToBinResort;
	
	BOOL	m_bEnableSECSComm;
	BOOL	m_bEnableTCPComm;
	BOOL	m_bOthersConnectionType;
	BOOL	m_EnableEMMode;
	BOOL	m_bGenRefDieStatusFile;
	BOOL	m_bEnableMultipleMapHeaderPage;
	BOOL	m_bGenLabel;
	BOOL	m_bGenLabelWithXMLFile;
	BOOL	m_bGenBinLotDirectory;
	BOOL	m_bEnableBinWorkNo;
	BOOL	m_bEnableFileNamePrefix;
	BOOL	m_bEnableAppendInfoInWaferId;
	BOOL	m_bAddWaferIdInMsgSummaryFile;
	BOOL	m_bEnableGradeMappingFile;
	BOOL	m_bEnablePickNPlaceOutputFile;

	BOOL	m_bBTRealignWithFFMode;
	BOOL	m_bEnablePolygonDie;
	BOOL	m_bEnableChineseMenu;		//v2.56
	BOOL	m_bDisableClearCountFormat;
	BOOL	m_bEnableDynMapHeaderFile;
	BOOL	m_bEnableMapColumnsCheck;	//v4.40T13	//BlueLight
	BOOL	m_bBackupTempFile;
	BOOL	m_bBackupOutputTempFile;
	BOOL	m_bForceDisableHaveOtherFile;
	BOOL	m_bEnableManualUploadBinSummary;
	BOOL	m_bGenerateAccumulateDieCountReport;
	BOOL	m_bDisableBinSNRFormat;
	BOOL	m_bEnablePRDualColor;
	BOOL	m_bEnablePRAdaptWafer;
	BOOL	m_bEnableRefDieFaceValueCheck;		//v4.48A26		//Avago
	BOOL	m_bEnableNGPick;					//v4.51A19		//Silan MS90
	BOOL	m_bUseColletOffsetWoEjtXY;			//v4.52A14		//Osram Germany
	BOOL	m_bEnableSemiAutoMode;				//v3.35T8		//Avago
	BOOL	m_bCounterCheckWithNormalDieRecord;
	BOOL	m_bEnableMotorizedZoom;
	BOOL	m_bPrCircleDetection;		//v2.78T2
	BOOL	m_bPrAutoLearnRefDie;		//v2.78T2
	BOOL	m_bWtCheckMasterPitch;		//v2.78T2
	BOOL	m_bEnable1stDieFinder;		//v3.15T5
	BOOL	m_bPrIMInterface;			//v2.78T4
	BOOL	m_bPrLineInspection;		//v3.17T1	//PLLM MS899EL
	BOOL	m_bPrEnableWpr2Lighting;	//v3.44T1	//SanAn
	BOOL	m_bUseOptBinCountDynAssignGrade;
	BOOL	m_bOpenMatchPkgNameCheck;
	//BOOL	m_bNewCtEnhancement;		//v2.83T2
	BOOL	m_bUseNewStepperEnc;		//v2.83T2
	BOOL	m_bDisableDefaultSearchWnd;
	BOOL	m_bUseBinMultiMgznSnrs;		//v3.30T2	
	BOOL	m_bExArmDisableEmptyPreload;	//v3.34		//For Cree MS899DLA
	BOOL	m_bUseNewBHMount;				//v3.34		//For MS810 New BH mounting
	BOOL	m_bEnableBinBlkTemplate;
	CString m_szRankIDFileExt;
	CString m_szRankIDFileNameInMap;
	BOOL	m_bEnableAutoLoadNamingFile;
	BOOL	m_bEnableLoadRankIDFromMap;
	BOOL	m_bEnableBTPt5DiePitch;		//v4.42T8	//Citizen
	BOOL	m_bEnableBTBondAreaOffset;	//v4.42T8	//Citizen
	BOOL	m_bBTGelPadSupport;			//v4.49A11
	BOOL	m_bEnableBTNewLotEmptyRowFcn;	//v4.52A6	//CYOptics, Inari
	BOOL	m_bSupportPortablePKGFile;
	BOOL	m_bPortablePKGInfoPage;
	BOOL	m_bBinFrameStatusSummary;
	BOOL	m_bEnableSummaryPage;
	BOOL	m_bEnableBondAlarmPage;
	BOOL	m_bEnableWaferLotLoadedMapCheck;
	BOOL	m_bWaferLotWithBinSummaryFormat;
	BOOL	m_bEnableEmptyBinFrameCheck;
	BOOL	m_bEnableResetMagzCheck;
	BOOL	m_bEnableWaferLotFileProtection;
	BOOL	m_bForceClearMapAfterWaferEnd;
	BOOL	m_bEnableOptimizeBinCountFunc;
	BOOL	m_bOptimizeBinCountPerWftFunc;
	BOOL	m_bEnableOpBinCntInLoadPath;		//v4.44T5	//Genesis 3F PkgSort
	BOOL	m_bEnableBHZVacNeutralState;		//v4.00T1
	BOOL	m_bWafflePadBondDie;
	BOOL	m_bWafflePadIdentification;
	BOOL	m_bGroupSetupAlarmAssitTimeToIdle;
	BOOL	m_bEnableMachineTimeDetailsReport;
	ULONG	m_lMachineTimeReportFormat;
	CString	m_szMachineTimeReportExt;
	BOOL	m_bEnableRestoreBinRunTimeData;
	BOOL	m_bDisableOKSelectionInAlertMsg;	//v2.83T2
	BOOL	m_bEnableReVerifyRefDie;					//Cree
	BOOL	m_bNonBlkPkEndVerifyRefDieApp;
	BOOL	m_bManualAlignReferDie;
	BOOL	m_bPrescanDiePitchCheck;
	BOOL	m_bEnablePrAutoEnlargeSrchWnd;

	BOOL	m_bEnablePrPostSealOptics;		//v3.71T5
	BOOL	m_bEnableBHPostSealOptics;		//v4.39T11	//PLLM MS109
	BOOL	m_bEnableBHUplookPrFcn;			//v4.52A16	//MS60/90 CSP
	BOOL	m_bUseBHUplookPrFcn;			//v4.58A3
	BOOL	m_bEnableESContourCamera;
	BOOL	m_bEnableDualPathCamera;
	BOOL	m_bMS60SortingFFMode;

	BOOL	m_bOfflinePostbondTest;			//v4.11T3	//Lumileds Lumiramic
	BOOL	m_bEnableAlignWaferImageLog;
	BOOL	m_bHomeFPCUseHmiMap;
	BOOL	m_bBLBCUseOldContinueStop;
	BOOL	m_bBLBCEmptyScanTwice;			//v4.51A17	//XM/XA SanAn
	BOOL	m_bBLEmptyFrameBCCheck;
	BOOL	m_bBLBCUse29MagSlots;			//v4.19		//Cree HuiZhou
	BOOL	m_bEnableMultiCOROffset;		//v4.24T8	//Osram, Genesis MS100 9Inch
	BOOL	m_bEnableNewBLMagExistCheck;	//v3.57
	BOOL	m_bCheckBinVacMeter;			//v3.58
	BOOL	m_bEnable2DBarCode;				//v3.60

	BOOL	m_bEnable2nd1DBarCode;			
	BOOL	m_bBLRealTimeCoverSensorCheck;	//v3.60T1
	BOOL	m_bBLRealTimeMgznExchange;		//v4.40T13	//Nichia
	BOOL	m_bEnableCoverLock;				//v3.60
	BOOL	m_bToPickDefect;				//v3.54T4	//ATS Mapping algorithm
	BOOL	m_bMS100OriginalBHSequence;
	BOOL	m_bEnableWaferSizeSelect;
	BOOL	m_bEnableColletCleanPocket;		//v3.65		//Lexter
	BOOL	m_bCleanEjectorCapStop;
	BOOL	m_bEnableWaferClamp;			//v4.26T1	//Walsin China MS810EL-90
	BOOL	m_bEnableStartupCoverSensorCheck;
	BOOL	m_bCoverSensorProtectionMessage;
	BOOL	m_bEnableBinLoderCoverSensorCheck;
	BOOL	m_bBLResetGradeMagToEmpty;		//v3.70T1
	BOOL	m_bUsePLLM;						//v3.94		//PLLM(S) General fcns
	BOOL	m_bUsePLLMSpecialFcn;			//v3.70T2	//PLLM machine buy-off
	BOOL	m_buseSlowEjProfile;			//v4.08		//FOr MS100/Plus
	BOOL	m_bReplaceEjAtWaferEnd;			//v4.39T10	//Silan (WanYiMing)
	BOOL	m_bReplaceEjAtWaferStart;		//v4.59A3	//Foxconn TW & Testar TW
	BOOL	m_bManualCloseExpReadBc;		//v4.40T14	//TJ Sanan
	//BOOL	m_bEnableMouseControl;
	BOOL	m_bNoPRRecordForPKGFile;
	BOOL	m_bEnableToolsUsageRecord;
	LONG	m_lToolsUsageRecordFormat;
	BOOL	m_bEnableMachineReport;
	BOOL	m_bEnableItemLog;
	BOOL	m_bEnablePkgFileList;			//v4.21T7	//Walsin China
	BOOL	m_bEnableAlarmLampBlink;
	BOOL	m_bEnableAmiFile;
	BOOL	m_bEnableBarcodeLengthCheck;
	BOOL	m_bEnableInputCountSetupFile;
	BOOL	m_bNewLotCreateDirectory;
	BOOL	m_bWLBarcodeSelection;			//v4.38T2	//Sanan with new barcode scanner support
	BOOL	m_bOnOff2DBarcodeScanner;
	BOOL	m_bForceClearBinBeforeNewLot;
	BOOL	m_bLoadAmiFileStatus;
	BOOL	m_bUseManualPRJsSpeed;			//v4.16T6	//Huga
	BOOL	m_bWaferPRMxNLFWnd;				//v4.43T2	//SanAn
	BOOL	m_bWaferPrMS60LFSequence;		//MS60 LF sequence	//v4.47T1
	BOOL	m_bEnableBinOutputFilePath2;
	BOOL	m_bMultiGradeSortToSingleBin;	//v4.15T8	//Osram Germany Layer-sort fcn
	BOOL	m_bEnableCMLTReworkFcn;			//v4.15T9	//CMLT TW for REWORK wafer
	BOOL	m_bNoIgnoreGradeOnInterface;
	ULONG	m_ulIgnoreGrade;
	BOOL	m_bCheckIgnoreGradeCount;
	BOOL	m_bEnableOsramResortMode;		//v4.21T3	//Osram Penang RESORT mode
	BOOL	m_bRenameMapNameAfterEnd;
	BOOL	m_bAlwaysLoadMapFileLocalHarddisk;
	LONG	m_lBHZ1HomeOffset;				//v4.44A5	//Cree
	LONG	m_lBHZ2HomeOffset;				//v4.44A5	//Cree

	LONG	m_lSetDetectSkipMode;
	BOOL	m_bEnableAssocFile;	
	CString	m_szAssocFileExt;
	CString m_szAssociateFile;

	BOOL	m_bWaferEndFileGenProtection;
	BOOL	m_bAutoGenWaferEndFile;
	BOOL	m_bAutoGenWaferEndAfterPrescan;	//v4.40T14
	BOOL	m_bUseClearBinByGrade;
	BOOL	m_bOutputFormatSelInNewLot;
	BOOL	m_bChangeGradeBackupTempFile;
	BOOL	m_bKeepOutputFileLog;
	BOOL	m_bAutoGenBinBlkCountSummary;
	BOOL	m_bRemoveBackupOutputFile;

	//For Mouse Joystick 
	INT		m_nMouseMode;
	DOUBLE	m_dMouseClickX;	
	DOUBLE	m_dMouseClickY;
	INT		m_nMouseDragState;
	DOUBLE	m_dMouseDragDist;
	DOUBLE	m_dMouseDragAngle;
	LONG	m_lMouseDragDirection;

	// For Feature value
	CString	m_szCustomer;
	CString	m_szProductLine;
	CString m_szFuncFileVersion;
	ULONG	m_ulDefaultBinSNRFormat;
	ULONG	m_ulDefaultClearCountFormat;
	ULONG	m_ulPLLMProduct;				//xyz123
	BOOL	m_bLoadPkgForNewWaferFrame;

	// For HMI control container access mode
	BOOL	m_bAcBHMotorSetup;
	BOOL	m_bAcBondArmSetup;
	BOOL	m_bAcBondHeadSetup;
	BOOL	m_bAcDelaySetup;
	BOOL	m_bAcEjectorSetup;
	BOOL	m_bAcBinBlkEditSetup;
	BOOL	m_bAcBinBlkClearSetup;
	BOOL	m_bAcClearBinFileSettings;
	BOOL	m_bAcClearBinCounter;
	BOOL	m_bAcNoGenOutputFile;
	BOOL	m_bAcWLLoaderSetup;
	BOOL	m_bAcWLExpanderSetup;
	BOOL	m_bAcWLAlignmentSetup;
	BOOL	m_bAcWLOthersSetup;
	BOOL	m_bAcWLMotorSetup;
	BOOL	m_bAcBLGripperSetup;
	BOOL	m_bAcBLMagazineSetup;
	BOOL	m_bAcBLOthersSetup;
	BOOL	m_bAcBLOthersManualOp;
	BOOL	m_bAcBLMotorSetup;
	BOOL	m_bAcMapPathSetting;
	BOOL	m_bAcAlignWafer;
	BOOL	m_bAcDisableSCNSettings;
	BOOL	m_bAcDisableManualAlign;
	BOOL	m_bAcMapSetting;
	BOOL	m_bAcMapOptions;
	BOOL	m_bAcLoadMap;
	BOOL	m_bAcClearMap;
	BOOL	m_bAcWaferTableSetup;
	BOOL	m_bAcCollectnEjSetup;

	BOOL	m_bAcDieCheckOptions;
	BOOL	m_bAcWaferEndOptions;

	// huga
	BOOL	m_bAcWaferPrAdvSetup;
	BOOL	m_bAcWaferPrLrnRefProtect;
	BOOL	m_bAcWaferPrLrnAlgrProtect;

	BOOL	m_bAcBondPrAdvSetup;
	BOOL	m_bAcBondPrLrnRefProtect;

	BOOL	m_bAcBinTableLimit;
	BOOL	m_bAcBinTableColletOffset;

	BOOL	m_bAcPKGFileSettings;

	BOOL	m_bAcWaferLotSettings;

	// For HMI Display accees status
	BOOL	m_bAsBHMotorSetup;
	BOOL	m_bAsBondArmSetup;
	BOOL	m_bAsBondHeadSetup;
	BOOL	m_bAsDelaySetup;
	BOOL	m_bAsEjectorSetup;
	BOOL	m_bAsBinBlkEditSetup;
	BOOL	m_bAsBinBlkClearSetup;
	BOOL	m_bAsClearBinFileSettings;
	BOOL	m_bAsClearBinCounter;
	BOOL	m_bAsNoGenOutputFile;		//v4.48A11
	BOOL	m_bAsWLLoaderSetup;
	BOOL	m_bAsWLExpanderSetup;
	BOOL	m_bAsWLAlignmentSetup;
	BOOL	m_bAsWLOthersSetup;
	BOOL	m_bAsWLMotorSetup;
	BOOL	m_bAsBLGripperSetup;
	BOOL	m_bAsBLMagazineSetup;
	BOOL	m_bAsBLOthersSetup;
	BOOL	m_bAsBLOthersManualOp;
	BOOL	m_bAsBLMotorSetup;
	BOOL	m_bAsMapPathSetting;
	BOOL	m_bAsAlignWafer;
	BOOL	m_bAsDisableSCNSettings;
	BOOL	m_bAsDisableManualAlign;
	BOOL	m_bAsMapSetting;
	BOOL	m_bAsMapOptions;
	BOOL	m_bAsLoadMap;
	BOOL	m_bAsClearMap;

	BOOL	m_bAsWaferTableSetup;
	BOOL	m_bAsCollectnEjSetup;
	BOOL	m_bAsWaferMapSetup;
	BOOL	m_bAcWaferMapSetup;

	BOOL	m_bAsDieCheckOptions;
	BOOL	m_bAsWaferEndOptions;

	// For Cp100
	BOOL	m_bIsCP100NewScreen;
	BOOL	m_bIsRedTowerSignal;
	BOOL	m_bIsYellowTowerSignal;
	BOOL	m_bIsGreenTowerSignal;

	// huga
	BOOL	m_bAsWaferPrAdvSetup;
	BOOL	m_bAsWaferPrLrnRefProtect;
	BOOL	m_bAsWaferPrLrnAlgrProtect;
	BOOL	m_bAsBondPrAdvSetup;
	BOOL	m_bAsBondPrLrnRefProtect;
	BOOL	m_bAsBinTableLimit;
	BOOL	m_bAsBinTableColletOffset;
	BOOL	m_bAsPKGFileSettings;
	BOOL	m_bAsWaferLotSettings;
	// For HMI Display Comm Option
	BOOL	m_bEnableCommOption;
	BOOL	m_bEnableBondTaskBar;		//SanAn BOND menu task bar	//v4.33T5
	BOOL	m_bEnableScanTaskBar;		//SanAn BOND menu task bar	//v4.33T5

	//Host Communication	
	CString m_szHostLotNo;
	CString m_szHostAddress;
	ULONG	m_ulHostPortNo;
	CString m_szHostMachineID;
	CString m_szHostCommandID;
	BOOL	m_bHostAutoConnect;
	BOOL	m_bHostCommEnabled;
	BOOL	m_bHostCommConnected;
	BOOL	m_bHostAutoReConnect;
	BOOL	m_bHostReConnectting;
	CString m_szHostCommStatus;

	// Machine Time Report variable
	CString m_szReportStartTime[EQUIP_NO_OF_RECORD_TIME];
	CString m_szReportEndTime[EQUIP_NO_OF_RECORD_TIME];
	INT		m_lNoOfReportPeriod;
	CString		m_szMachineTimeReportPath;
	CString		m_szMachineTimeReportPath2;
	CString		m_szTimePerformancePath;
	CString		m_szMachineSerialNo;

	CString		m_szAlarmLampStatusReportPath;

	BOOL		m_bIsCopyGmpErrorLog;

//Attribute
public:
	CInitOperation	*m_pInitOperation;	//InitOperation Object - Created by Mark Wizard

	BOOL	m_bAppInit;					//v3.97
	BOOL	m_bUseWideScreen;			//v4.29
	BOOL	m_bUseTouchScreen;			//v4.50A3

	BOOL	m_bEnableLoadTSFWhenLoadPKG;
	BOOL	m_bEnableTesterList;

	//4.53D18
	BOOL	m_bWaferMapDieMixing;
	//Machine File and Device File
	// Alarm Wait Engineer Time (AWET), let Engineer to do setup
	BOOL	m_bAWET_Enabled;			//	option in feature.msd also the visible for recover button
	BOOL	m_bAWET_Triggered;			//	when defined alarm pops and trigger engineer to fix.
	CString	m_szAWET_AlarmCode;			//	display trigger alarm code
	BOOL	m_bAWET_CtrlOpContainer;	//	screen accessable for operator container
	BOOL	m_bAWET_CtrlEngContainer;	//	screen accessable for engineer container
	BOOL	m_bAWET_CtrlExitButton;		//	screen accessable for exit button
	BOOL	m_bAWET_AutoSetScreen;		//	let APP auto into recover screen, if by button, should be reset.
	LONG	m_lAWET_EngineerFlowState;	//	engineer operation flow state 0, default; 1, alarm; 2, logged in, begin to setup; 3, done, let OP to run.
	ULONG	m_ulAWET_LastCounter;
	ULONG	m_ulAWET_LastRunTime;
	CString m_szAWET_LastCode;
	BOOL	AWET_CheckAlarmCode(CString szCode);

	CString				m_szWaitingSetTimeAlarmMsg; // v4.51D5 SanAn(XA) 15
	BOOL				m_bWaitingSetTimeClosed;
	BOOL				m_bWaitingSetTimeContainer;


	CString				m_szRecoverEngineerID; // v4.51D2 //Electech3E(DL) 13
	CString				m_szRecoverEngineerPassword; 
	CString				m_szRecoverPasswordPath;
	CString				m_szRecoverGeneratedFilePath;
	CString				m_szFaultOperatorID; 
	CString				m_szFaultOperatorPassword;
	CString				m_szFaultFilePath; 
	BOOL				m_bFaultButton; 
	BOOL				m_bFaultContainer; 	
	BOOL				m_bFaultButtonColor; 
	BOOL				m_bRecoverButton; 
	BOOL				m_bRecoverContainer;
	BOOL				m_bRecoverGoBackScreen;
	BOOL				m_bRecoverFinished;

	LONG				m_lBondTabPageSelect;

	CGemStation        *m_pAppGemStation;
	CStringMapFile		m_smfMachine;
	CStringMapFile		m_smfDevice;
	CCriticalSection	m_csMachine;
	CCriticalSection	m_csDevice;
	CCriticalSection	m_csMachineAlarmLamp;
	CString				m_szPKGPreviewPath;
	
	LONG				m_lDeviceFileType;
	UCHAR				m_ucDeviceFileType;
	CString				m_szDeviceFile;
	CString				m_szDevicePathDisplay;
	CString				m_szPortablePKGPath;
	CString				m_szDevicePath;
	CString				m_szDeviceCount;
	CString				m_szPkgFileListPath;		//Walsin China	//v4.21T7
	BOOL				m_bAmiPitchUpdateBlock;		// Load die pitch from AMI file, update bin block die pitch and calculate row/col/total die
	CString				m_szAmiFilePath;

	CString				m_szMapFilePath;
	CString				m_szMapFileExt;
	CString				m_szOutputFilePath;
	CString				m_szOutputFileFormat;
	CString				m_szWaferEndPath;
	CString				m_szWaferEndFormat;
	CString				m_szLoadMapDate;
	CString				m_szLoadMapTime;
	CString				m_szMapStartDate;
	CString				m_szMapStartTime;
	CString				m_szAppSWVersion;

	CStringMapFile		m_smfSRam;		// Global area for data exchange
	CDeviceFile			m_DeviceFile;

	CString				m_szBinStoragePath;
	CString				m_szBinStorageFilename;
	BOOL				m_bDialogStorageFile;

	BOOL				m_bWaferLabelFile; 

	void				*m_pvNVRAM;			// Non-volatile RAM
	void				*m_pvNVRAM_HW;		// Non-volatile RAM (Hardware)
	CStdioFile			m_VNRAM_SW;			//Store NVRAm data on hard disk		//v3.66		//NU_MOTION
	INT					m_nNVRAMOption;		//0=DRAM, 1=MFB4, 2=NuMotion

	BOOL	m_bMachineStarted;				// Flag to indicate machine is started

	// Flag to indicate that auto-cycle is starting
	BOOL	m_bCycleStarted;		

	BOOL	m_bDisableStopButton;			//v4.13T1	//Cree US to temporarily disble STOp button on AUTOBOND screen

	//For BurnIn
	BOOL	m_bBurnIn;
	BOOL	m_bBurnInGrabImage;

	//ANDREW_SC
	BOOL	m_bOnBondMode;
	BOOL	m_bInBondPage; // 4.51D1 Secs

	//For Application closing down
	BOOL	m_bAppClosing;
	
	static HHOOK	m_hKeyboardHook;		//v4.08		//v4.38T1
	static CString	m_szKeyboard;			//v4.38T1
	static BOOL		m_bEnableKeyboardHook;	//v4.38T1

	static BOOL m_bIsLoadingPKGFile;
	static BOOL	m_bRuntimeLoadPPKG;

	//First Cycle flag
	static BOOL m_bIsFirstBondCycle;

	static BOOL m_bBondHeadILCFirstCycle;

	//Load map OK flag
	static BOOL m_bMapLoadingFinish;
	static BOOL m_bMapLoadingAbort;
	static LONG m_lPreBondEventReply;
	
	//Machine Hardware info
	static LONG m_lMotionPlatform;		//v3.61
	static LONG m_lVisionPlatform;		//v4.05
	static LONG m_lCycleSpeedMode;		//v3.61
	static LONG	m_lCheckCoverSnr;
	static LONG	m_lBarCode1ComPort;		//WT
	static LONG	m_lBarCode2ComPort;		//BT
	static LONG	m_lBarCode3ComPort;		//BT2
	static LONG	m_lBarCode4ComPort;		//WT2
	static LONG m_lMainCoverLock;
	static LONG m_lSideCoverLock;
	static LONG	m_lBinElevatorCoverLock;
	static LONG m_lHardwareConfigNo;
	static LONG m_lBondHeadConfig;
	static LONG m_lBondHeadMounting;
	static BOOL m_bBondHeadTThermalControl;
	static BOOL	m_bUseSlowBTControlProfile;			//v3.67T5
	static BOOL m_bNoStaticControlForBTJoystick;	//for Cree MS896-DL only
	static BOOL m_bIsPrototypeMachine;				//prototype MS100
	static BOOL m_bIsNuMotionSingleArmMachine;
	static BOOL m_b180Arm6InchWaferTable;
	static BOOL	m_bEnableBHSuckingHead;				//v4.05
	static BOOL	m_bUseALBondArm;					//default is fiducial Mark BondArm 1.10, AL BondArm is original bond arm
	static BOOL	m_bTableXYMagneticEncoder;			//for Wafer Table and Bin Table 1.10
	static BOOL	m_bBinTableTHighResolution;			//for Bin Table T 1.11 2018.9.18
	static BOOL	m_bUseExpanderWaferTable;			// for Wafer table with Expander 2019.03.11
	static BOOL	m_bEnableThetaWithBrake;			// for Bin table Theta with Brake 2019.03.27
	static BOOL m_bNewAutoCleanColletSystem;		//for new auto clean collet system 1.12 version
	static BOOL m_bMS100Plus;						//v4.35T1
	static BOOL m_bMS100Plus9InchOption;			//v4.16T3
	static BOOL m_bES100v2DualWftOption;			//v4.24T4
	static BOOL m_bMS50;							//v4.59A41
	static BOOL m_bMS60;							//v4.46T21
	static BOOL m_bMS90;							//v4.48A27
	static BOOL m_bMSAutoLineMode;					//v4.55A7
	static BOOL m_bMS_SISMode_HMI;
	static BOOL m_bMS_SISStandaloneMode_HMI;
	static BOOL m_bMS_StandByButtonPressed_HMI;
	static LONG m_lChangeColletMaxIndex;

	//static LONG	m_lProberContactSensors;
	static BOOL m_bESAoiSingleWT;
	static BOOL	m_bESAoiDualWT;
	static BOOL	m_bESAoiBackLight;
	static BOOL m_bMS100SingleLoaderOption;				//v4.31T10	//Yearly MS100Plus use BL as WL

	static BOOL m_bEnableGripperLoadUnloadTest;
	static BOOL m_bEnableLaserTestOutputPortTrigger;
	static BOOL m_bDownloadPortablePackageFileImage;
	static BOOL m_bErasePortablePackageFileImage;

	static BOOL m_bBinFrameNewRealignMethod;
	static BOOL m_bBinFrameUseSoftRealign;			//v4.59A35
	static BOOL m_bBinMultiSearchFirstDie;  //4.51D20 
	static BOOL m_bProRataData;//M69
	static BOOL m_bBPRErrorCleanCollet;

	static BOOL m_bEnableAlarmTowerBlink;
	static LONG m_lAlarmBlinkStatus;
	static LONG m_lAlarmLampStatus;

	static UCHAR m_ucNuSimCardType;

	BOOL	m_bAutoUploadOutputFile;
	BOOL	m_bCheckPKGKeyParameters;
	BOOL	m_bEnableAutoMapDieTypeCheckFunc;

	BOOL	m_bDisableBuzzer;
	CString m_szMapDieTypeCheckString[MS896A_DIE_TYPE_CHECK_LIMIT];

	// Nu Motion I/O Port and Channel Port
	static CMSNmIoPort m_NmSIPort[UM_MAX_SI_PORT];
	static CMSNmIoPort m_NmSOPort[UM_MAX_SO_PORT];
	static CMSNmChPort m_NmCHPort[UM_MAX_CH_PORT];
	static CMSNmSwPort m_NmSWPort[UM_MAX_SW_PORT];

	//Stop Alignment
	static BOOL m_bStopAlign;

	//For Logging
	static BOOL	m_bEnableMachineLog;
	static BOOL	m_bEnableWtMachineLog;		//v4.06
	static BOOL	m_bEnableTableIndexLog;		//v4.59A12
	BOOL	m_bEnableAutoRecoverPR;
	
	//For indicate change wafer
	static BOOL	m_bIsChangeWafer;

	static BOOL	m_bEnableSubBin;
	static BOOL m_bNGWithBinMap;
	static BOOL m_bEnableErrMap;			//andrewng //2020-0805

	//MS License Key Fcn
	static BOOL m_bMSLicenseKeyFcn1;
	static BOOL m_bMSLicenseKeyFcn2;
	static BOOL m_bMSLicenseKeyFcn3;
	static BOOL m_bMSLicenseKeyFcn4;
	static BOOL m_bMSLicenseKeyFcn5;

	//For other station to check is Operator or Not
	BOOL	m_bOperator;
	BOOL	m_bSuperUser;			//v4.28T6	//andrewxx
	BOOL	m_bOperatorLogOnValidation;

	//Host Communication	
	CHostComm *m_pHostComm;
	BOOL	m_bHostCommAbortByHost;
	BOOL	m_bHostLotIDMatch;

	BOOL		m_bNewMapLoaded;

	BOOL		m_bRuntimeTwoPkgInUse;
	BOOL		m_bPortablePKGFile;
	BOOL		m_bOnlyLoadPRParam;
	BOOL		m_bOnlyLoadBinParam;
	BOOL		m_bOnlyLoadWFTParam;
	BOOL		m_bExcludeMachineParam;
	BOOL		m_bManualLoadPkgFile;		//v4.53A22

	// For Multi-Language
	LONG		m_lLangSelected;
	LONG		m_lLangPrevSelected;
	CString		m_szMultiLanguage;


	BOOL		m_bAddDieTimeStamp;
	BOOL		m_bCheckProcessTime;
	BOOL		m_bAddSortingSequence;
	// Enable Extra Info for customer in the output file
	BOOL		m_bEnableExtraClearBinInfo;
	BOOL		m_bEnableSaveTempFileWithPKG;

	BOOL		m_bBinFrameHaveRefCross;
	// check BondHead Tolerance Leve1
	BOOL		m_bEnableBondHeadTolerance;

	BOOL		m_bEnableLogColletHoleData;
	// check Disk Space
	CHAR		m_cDiskLabel;
	DOUBLE		m_dWarningEmptySpacePercent;


	BOOL		m_bEnableClearBinCopyTempFile;

	// 
	CString		m_szFileFormatSelectInNewLot;

	BOOL		m_bClearDieTypeFieldDuringClearAllBin;
	BOOL		m_bEnableAutoDieTypeFieldnameChek;
	CString		m_szDieTypeFieldnameInMap;

	BOOL		m_bPackageFileMapHeaderCheckFunc;
	BOOL		m_bUpdateWaferMapHeader;

	CString		m_szPackageFileMapHeaderCheckString;

	CEquipTime	m_eqMachine;
	CEquipTime	m_eqMachine2;				//WH Sanan only	//v4.40T4

	CString		m_szMachineReportPath;
	CString		m_szMachineReportPath2;		//v4.48A4	//3E DL
	CToolsUsageRecord m_oToolsUsageRecord;
	CMS896ADataBlock m_oMS896ADataBlock;

	BOOL		m_bMsStopButtonPressed;		//v3.65


	DOUBLE		m_dPreviewImageSizeX;
	DOUBLE		m_dPreviewImageSizeY;

	BOOL		m_bIsPortablePackageFileIgnoreList;
	CStringArray m_szaPortablePackageFileFileList;

	BOOL		m_bEnableReportPathSettingInterface;

	CArray<ALARM_INFO, ALARM_INFO> m_aAlarmInformation;

//Operation
private:
	BOOL CheckPKGFileExtension(BOOL bIsPortablePKGFile, CString szFileExt);
	BOOL SaveToDevice(CString szDevicePath, CString szDeviceFile);		// Save to Device File
	BOOL RestoreFromDevice(	CString szDevicePath, CString szDeviceFile, 
							BOOL bPopUpConfirmDialog, BOOL bPopupCompleteDialog,
							BOOL bEnableSECSGEM=TRUE);	//v4.51A12
	BOOL StorePKGPreviewImage(CString szDevicePath , CString szDeviceFile);
	BOOL DeletePKGPreviewImage(CString szDevicePath , CString szDeviceFile);
	VOID OnSelectPackageFile();
	VOID ChangePkgFilePreviewDiagram(CString szDevicePath , CString szDeviceFile);
	BOOL GeneratePkgDataFile(CONST CString szFilePath, CONST CString szPkgName);		//Walsin China	//v4.21T7
	BOOL GeneratePkgFileList(CONST CString szFilePath);		//Walsin China	//v4.21T7
	
	BOOL SaveBinRunTimeData(CString szFilePath, CString szFilename);
	BOOL RestoreBinRunTimeData(CString szFilePath, CString szFilename);
	BOOL MachineWarmStart(CONST BOOL bLoadMSD, BOOL bNoResetNVRAM = FALSE);

	BOOL ResetScreenControlAccessMode();
	BOOL ReadScreenControlAccessMode();
	BOOL WriteScreenControlAccessMode();

	// Host communication
	BOOL ReadHostCommConfig();
	BOOL WriteHostCommConfig();
	BOOL SaveHostCommContent(CString szName, CString szValue);
	BOOL SaveHostCommStatus(CString szName, BOOL bStatus);
	VOID SetupHostComm();
	VOID RetryHostComm();
	VOID ConnectHostComm(BOOL bConnect);
	BOOL CheckHostCommStatus();
	BOOL SendMessageToHost(CString szMessage);
	LONG UpdateHostLotID(CString szMessage);

	//Check Application is running or not
	VOID LogStatusInfo(CString szText);		//v2.93T2

	LONG LoadSelectedLanguage();
	BOOL CheckVolumeInfo();					//v2.97T1
	BOOL CreatePrinterSelectionFile();
	BOOL ReadAutorizedPrinterList(CStringArray &szaAuthorizedPrinterList);
	BOOL CheckExpireDate();					//v3.57T2	//SanAn
	BOOL SteerPkgKeyParameters(LONG lAction);
	BOOL CheckPkgKeyParameters();
	//BOOL IsNormalNuSimCard();	
	LONG PreBondEventReplyCheck();

protected:
	//Initialisation
	VOID CreateDefaultFolder();
	VOID LoadData();
	BOOL InitHardware();
	VOID SetupIPC();
	CAppStation* RegStnClass(const CString &szClassName, CRuntimeClass *pRunTimeClass, const CString &szStnName);
	BOOL InitHiPECSystem();
	VOID ConfigSystemActivation(const CString szOrgActPath, const CString szDestActPath);
	BOOL InitNuMotion();
	VOID FreeNuMotion();
	BOOL CheckNuMotionSIMCard();
	BOOL CheckNuMotionSIMCardInAutoBond();
	BOOL CheckDebSortMode(ULONG ulBit9);

	//Check for Motion Completion
	INT CheckStationMotion();

	//Scan Input & Update Output
	VOID UpdateOutput();
	VOID ScanInput();

	//Refresh machine time
	VOID RefreshMachineTime();
    VOID CopyAlarmReportEveryHour();
	//Operation
	VOID UnInitialOperation();
	VOID IdleOperation();
	VOID DiagOperation();
	VOID SystemInitialOperation();
	VOID PreStartOperation();
	VOID AutoOperation();
	VOID DemoOperation();
	VOID CycleOperation();
	VOID ManualOperation();
	VOID StopOperation();
	VOID DeInitialOperation();
	VOID AfterStopOperation();

	// IPC Registered Services
	LONG SrvInitialize(IPC_CServiceMessage &svMsg);
	LONG SrvSystemInitialize(IPC_CServiceMessage &svMsg);
	LONG SrvDiagCommand(IPC_CServiceMessage &svMsg);
	LONG SrvAutoCommand(IPC_CServiceMessage &svMsg);
	LONG SrvManualCommand(IPC_CServiceMessage &svMsg);
	LONG SrvDemoCommand(IPC_CServiceMessage &svMsg);
	LONG SrvStopCommand(IPC_CServiceMessage &svMsg);
	LONG SrvStopAutoBond(IPC_CServiceMessage &svMsg);			//andrewng //2020-0817
	LONG SrvResetCommand(IPC_CServiceMessage &svMsg);
	LONG SrvDeInitialCommand(IPC_CServiceMessage &svMsg);
	LONG SaveMapFilePath(IPC_CServiceMessage &svMsg);
	LONG SavePasswordFilePath(IPC_CServiceMessage &svMsg);		//Sanan password fcn	//v4.17T7
	LONG SaveMapFileExt(IPC_CServiceMessage &svMsg);
	LONG SaveOutputFilePath(IPC_CServiceMessage &svMsg);
	LONG SaveOutputFileFormat(IPC_CServiceMessage &svMsg);
	LONG SaveWaferEndFilePath(IPC_CServiceMessage &svMsg);
	LONG SaveWaferEndFileFormat(IPC_CServiceMessage &svMsg);
	LONG SaveMachineNo(IPC_CServiceMessage &svMsg);
	LONG SaveMachineNo2(IPC_CServiceMessage &svMsg);
	LONG SaveMachineName(IPC_CServiceMessage &svMsg);
	LONG SaveLotNumber(IPC_CServiceMessage &svMsg);
	LONG SaveLotNumberPrefix(IPC_CServiceMessage &svMsg);		//v4.35T1	//PLLM Rebel/Flash
	LONG SaveShiftID(IPC_CServiceMessage& svMsg);				//v3.32T3	//Avago	//v4.48A21
	LONG SaveRecipeTableFilePath(IPC_CServiceMessage &svMsg);	//v4.42T3	//PLLM Rebel/Flash
	LONG SetRecipeTableFilePath(IPC_CServiceMessage &svMsg);	//v4.46T20	//PLLM Rebel/Flash
	LONG ResetLotNumber(IPC_CServiceMessage &svMsg);
	LONG SaveBinLotDirectory(IPC_CServiceMessage &svMsg);
	LONG ResetBinLotDirectory(IPC_CServiceMessage &svMsg);
	LONG SaveAssociateFile(IPC_CServiceMessage &svMsg);
	LONG ResetAssociateFile(IPC_CServiceMessage &svMsg);
	LONG GetDeviceFileName(IPC_CServiceMessage &svMsg);
	LONG UpdatePackageFileInterface(IPC_CServiceMessage &svMsg);
	LONG PackageFileSetupPreRoutine(IPC_CServiceMessage &svMsg);
	LONG SaveDeviceFile(IPC_CServiceMessage &svMsg);
	LONG RestoreDeviceFile(IPC_CServiceMessage &svMsg);
	LONG DeletePackageFile(IPC_CServiceMessage &svMsg);
	LONG LoadAmiFile(IPC_CServiceMessage &svMsg);
	LONG CheckIfSubmountLot(IPC_CServiceMessage &svMsg);		//v3.27T1	//PLLM 
	LONG SavePLLMProduct(IPC_CServiceMessage &svMsg);			//xyz
	BOOL SaveToGenericDevice();
	LONG SaveGenericDeviceFile(IPC_CServiceMessage &svMsg);
	LONG CheckGenericDeviceFile(IPC_CServiceMessage &svMsg);	//v4.59A15	//Osram
	LONG LoadGenericDeviceFile(IPC_CServiceMessage &svMsg);
	LONG SetLoadPkgForNewWaferFrame(IPC_CServiceMessage &svMsg);

	LONG CheckOpClearBinCounterAcMode(IPC_CServiceMessage &svMsg);
	LONG LoadBinBlockRunTimeData(IPC_CServiceMessage &svMsg);
	LONG SaveBinBlockRunTimeData(IPC_CServiceMessage &svMsg);
	BOOL GetBinStoragePath(IPC_CServiceMessage &svMsg);
	BOOL GetPkgFileListPath(IPC_CServiceMessage &svMsg);		//v4.21T7	//Walsin China
	BOOL GetAmiFilePath(IPC_CServiceMessage &svMsg);

	LONG SrvBurnInCommand(IPC_CServiceMessage &svMsg);
	LONG SrvStopBurnInCommand(IPC_CServiceMessage &svMsg);

	BOOL CheckPasswordWithTitle(CString szPassword, CString szTitle = "Please Enter YOUR Password");
	LONG LoadHmiSecuritySettings();
	LONG ChangeHmiSecurityLevel(LONG lAccessLevel);
	LONG ChangeHmiSecurityRoleName();
	LONG SetHmiSecurityUpperLowerLimit();
	LONG WarmStart(IPC_CServiceMessage &svMsg);
	LONG ColdStart(IPC_CServiceMessage &svMsg);
	LONG LogOff(IPC_CServiceMessage &svMsg);
	LONG CheckAccessRight(IPC_CServiceMessage &svMsg);
	LONG CheckAccessRight_ColletEjLmts(IPC_CServiceMessage &svMsg);
	LONG CheckAccessRight_MinimizeHmi(IPC_CServiceMessage &svMsg);
	LONG CreeCheckAccessRight(IPC_CServiceMessage &svMsg);
	LONG CheckAccessRight_FixOne(IPC_CServiceMessage &svMsg);

	LONG ReloadMachineProfileCmd(IPC_CServiceMessage &svMsg);
	LONG ReloadMachineProfile();
	LONG ExportMachineProfile(IPC_CServiceMessage &svMsg);
	LONG ImportMachineProfile(IPC_CServiceMessage &svMsg);
	LONG ManualGenerateParametersRecord(IPC_CServiceMessage &svMsg);
	LONG SaveOperatorId(IPC_CServiceMessage &svMsg);
	LONG SaveLumiledsOperatorId(IPC_CServiceMessage &svMsg);
	LONG UpdatePassword(IPC_CServiceMessage &svMsg);
	LONG ResetPassword(IPC_CServiceMessage &svMsg);
	LONG UpdatePackageFilePath(IPC_CServiceMessage &svMsg);
	LONG GetPackagePath(IPC_CServiceMessage &svMsg);
	LONG RebootMachine(IPC_CServiceMessage &svMsg);
	LONG ClearWholeNVRAM(IPC_CServiceMessage &svMsg);
	LONG ResetHiPEC(IPC_CServiceMessage &svMsg);
	LONG UpdateAccessMode(IPC_CServiceMessage &svMsg);
	LONG UpdateAction(IPC_CServiceMessage &svMsg);
	LONG UpdateAppData(IPC_CServiceMessage &svMsg);
	LONG ChangeHmiAccessLevel(IPC_CServiceMessage &svMsg);
	LONG UpdateHmiAccessPw(IPC_CServiceMessage &svMsg);
	LONG ResetHmiAccessPw(IPC_CServiceMessage &svMsg);
	//LONG LogItems(IPC_CServiceMessage& svMsg);

	LONG Exit(IPC_CServiceMessage &svMsg);

	// Service for press start or stop control in HMI 
	LONG IsPickAndPlaceOn();
	BOOL RealignBinFrame();
	VOID PreStartCycleFailHandling();
	VOID SendStartStopEvent_SIS(const BOOL bStart);
	VOID BackupBurnInStatus();
	VOID RestoreBurnInStatus();
	LONG PreStartCycle(IPC_CServiceMessage &svMsg);
	BOOL SubPreStartCycle();
	LONG CheckIfAllTaskInIdleState(IPC_CServiceMessage &svMsg);

	// Host Comm (TCP/IP) Service
	LONG UpdateHostCommSetting(IPC_CServiceMessage &svMsg);
	LONG HostCommConnection(IPC_CServiceMessage &svMsg);
	LONG HostCommTest(IPC_CServiceMessage &svMsg);

	// send help page information to host
	LONG SC_CmdVisionSWVersionNo(IPC_CServiceMessage &svMsg);
	LONG SC_CmdVisionSWReleaseNo(IPC_CServiceMessage &svMsg);
	LONG SC_CmdHelpScreenInfo(IPC_CServiceMessage &svMsg);

	// Bin Lot Info
	LONG SaveBinWorkNo(IPC_CServiceMessage &svMsg);
	LONG SaveBinFileNamePrefix(IPC_CServiceMessage &svMsg);

	// Map sub folder name
	LONG SaveMapSubFolderName(IPC_CServiceMessage &svMsg);

	// PLLM Die Fab MES Lot no
	LONG SavePLLMDieFabMESLot(IPC_CServiceMessage &svMsg);
	
	// For machine logging
	LONG EnableMachineLog(IPC_CServiceMessage &svMsg);
	LONG EnableWtMachineLog(IPC_CServiceMessage &svMsg);		//v4.06
	LONG EnableAutoRecoverPR(IPC_CServiceMessage &svMsg);
	LONG EnableMsMachineLog(IPC_CServiceMessage &svMsg);
	LONG EnableMsTableIndexLog(IPC_CServiceMessage &svMsg);
	LONG EnableMappingLog(IPC_CServiceMessage &svMsg);

	// New Lot Start	//PLLM
	LONG CheckStartNewLot(IPC_CServiceMessage &svMsg);			//v4.16T5
	LONG StartNewLot(IPC_CServiceMessage &svMsg);

	//For machine time
	LONG SaveAndResetTime(IPC_CServiceMessage &svMsg);
	LONG StartSetupTime(IPC_CServiceMessage &svMsg);
	LONG StopSetupTime(IPC_CServiceMessage &svMsg);
	LONG WaitingSetupButtom(IPC_CServiceMessage &svMsg); // v4.51D5 SanAn(XA) 16
	// AWET
	LONG AWET_IntoRecoverScreen(IPC_CServiceMessage &svMsg);
	LONG AWET_EngineerLogIn(IPC_CServiceMessage &svMsg);
	LONG AWET_OperatorLogIn(IPC_CServiceMessage &svMsg); 
	LONG AWET_OperatorAskHelp(IPC_CServiceMessage &svMsg); 

	LONG CompareFaultOperatorID(IPC_CServiceMessage &svMsg);   //v4.51D2 //Electech3E(DL) 14
	LONG CompareRecoverEngineerID(IPC_CServiceMessage &svMsg); 
	//For machine data
	LONG SaveMachineData(IPC_CServiceMessage &svMsg);
	// Set Machine Time Report Path Function
	LONG SetMachineTimeReportPath(IPC_CServiceMessage &svMsg);
	LONG SetMachineTimeReportPath2(IPC_CServiceMessage &svMsg);
	LONG SetTimePerformancePathCmd(IPC_CServiceMessage &svMsg);

	LONG SetMachineReportPath(IPC_CServiceMessage &svMsg);
	LONG SetMachineReportPath2(IPC_CServiceMessage &svMsg);		//v4.48A4	//3E DL
	LONG SetAlarmLampStatusReportPath(IPC_CServiceMessage &svMsg);

	// Genesis Sort Setup File
	LONG ClearSortSetupFile(IPC_CServiceMessage &svMsg);

	// Command For Multi-Lang Selection
	LONG MultiLangSelections(IPC_CServiceMessage &svMsg);
	LONG MultiLangSelectionsToggle(IPC_CServiceMessage &svMsg);
	LONG SaveSelectedLanguage(IPC_CServiceMessage &svMsg);
	LONG SaveReportStartEndTime(IPC_CServiceMessage &svMsg);

	LONG OutputFileFormatSelectInNewLot(IPC_CServiceMessage &svMsg);

	LONG HmiPassword(IPC_CServiceMessage &svMsg);							//v4.17T7	//SanAn
	BOOL CheckMSLoginPassword(BOOL bAdmin);
	LONG CheckPasswordFile(IPC_CServiceMessage &svMsg);						//v4.17T7	//SanAn
	LONG CheckUserLoginName();
	BOOL CheckSananPasswordFile_XMChangeLight(); //1.08S
	BOOL CheckPasswordFile_SemitekZJG(BOOL bInputUserID, LONG lInAccessLevel = 0);
	LONG EncryptPasswordFile(IPC_CServiceMessage &svMsg);
	LONG DecryptPasswordFile(IPC_CServiceMessage &svMsg);
	LONG CodeSanAnPasswordFile(IPC_CServiceMessage &svMsg);					//SanAn
	LONG OperatorLogOnValidation(IPC_CServiceMessage &svMsg);
	LONG ClearOperatorId(IPC_CServiceMessage &svMsg);
	LONG StopLoadUnloadLoopTest(IPC_CServiceMessage &svMsg);

	BOOL LoadPackageMsdData(IPC_CServiceMessage &svMsg);
	BOOL SavePackageMsdData(IPC_CServiceMessage &svMsg);
	BOOL UpdatePackageList(IPC_CServiceMessage &svMsg);
	
	LONG SetAOTDefaultSetting(IPC_CServiceMessage &svMsg);
	LONG ZipErrorLog(IPC_CServiceMessage &svMsg);

	LONG SetWideScreenMode(IPC_CServiceMessage &svMsg);			//v4,29

	LONG BurnInSetup(IPC_CServiceMessage &svMsg);

	LONG CheckDiscUsage(IPC_CServiceMessage &svMsg);			//v4.46T30
	LONG SuperCP2in1Button(IPC_CServiceMessage &svMsg);			//v4.46T30
	LONG CombineToProbeAndPLevel(IPC_CServiceMessage &svMsg);
	//v4.08
	LONG EnableKeyboard(IPC_CServiceMessage &svMsg);
	BOOL m_bIsKeyboardLocked;				
	//v4.11T1
	LONG ExportNVRAMDataToFile(IPC_CServiceMessage &svMsg);
	LONG ImportNVRAMDataFromFile(IPC_CServiceMessage &svMsg);
	//v4.48A2
	LONG CreateMSLicenseFile(IPC_CServiceMessage &svMsg);
	LONG RegisterMSLicenseFile(IPC_CServiceMessage &svMsg);

	LONG HmiCornerSearchHomeDie(IPC_CServiceMessage &svMsg);
	LONG IsOPMenuEnabled(IPC_CServiceMessage &svMsg);			//v4.50A11
	LONG IsUnderAutoCycle(IPC_CServiceMessage &svMsg);

	//Get Machine config file  
	CString GetMachineInformation(const CString &szTitle, const CString &szGroup = "");
	LONG GetMachineHardwareConfig(const CString &szTitle);
	VOID SavePackageInfo(LONG lSaveMode = MS_PKG_SAVE_ALL);
	CString CodePassword(CString &szPass, INT nShift);	// Code and decode password
	CString EncodePassword(CString &szPass);	// save
	CString DecodePassword(CString &szPass);	// read

	VOID SetProRata(IPC_CServiceMessage &svMsg); //M69
	//Register Hmi Variables
	VOID RegisterVariables();

	//Get Application Features information
	VOID GetMachineAllFeatures(VOID);

	//Get MS Options File for special Hardware config
	VOID GetMSOptions(VOID);
	VOID GetRegistryEntries();
	BOOL CheckAllHardwareReady(CString &szFailStnName);		// Check whether all hardware initiated
	BOOL DeleteAllFiles();				// Delete all SMF files
	// Change wafer frame

	LONG ChangeWaferFrame(BOOL bBurnIn, BOOL bEnable);
	BOOL AutoStopBinLoader(BOOL bBurnIn);	//v4.42T4
	LONG LoadMapHeaderInfo();
	LONG SaveAppData();
	LONG LoadAppData();


//Operation
public:

	CMS896AApp();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual VOID UpdateStationData();

	VOID SetTimers();
	LONG GetCurTime();
	LONG elapse_time(LONG lStartTime);

	BOOL GenerateParametersRecord();
	BOOL RestoreNichiaPRM(CString szDeviceFile, BOOL bNeedWarmStart=FALSE);		//Nichia//v4.43T7

	INT State();
	VOID Operation();

	//Group ID and ModuleID
	CString GetModuleID();
	CString	GetGroupID();
	VOID SaveGroupId(const CString &szGroup);
	VOID SetGroupId(const CString &szGroup);

	CString GetSoftVersion();
	CString GetMachineModel();
	CString GetMachineNo();		//v4.48A4
	CString GetBondStartDate();
	CString GetBondStartTime();
	CString GetBondStopDate();
	CString GetBondStopTime();

	BOOL LoadPackageMsdMSData(VOID);
	BOOL SavePackageMsdMSData(VOID);
	BOOL UpdateMSPackageList(VOID);

	BOOL IsPortablePKGFile();
	BOOL IsManualLoadPkgFile();	//v4.53A22
	BOOL IsRuntime2PkgInUse();
	
	//Start Execution
	BOOL AutoStart();
	BOOL SetRun();
	VOID SaveAutoStart(BOOL bOption);

	//Hardware & Option Selection
	VOID SelectHardware();
	VOID SetOptions();

	//Msg Log
	BOOL LogMessage();
	VOID SetLogMessage(BOOL bEnable);
	VOID SetTotLogBackupFiles(LONG lUnit);
	VOID SetTotLogFileLine(LONG lUnit);

	//Msg Display
	VOID DisplayException(CAsmException &e);
	VOID DisplayMessage(const CString &szMessage);

	//Execute HMI
	VOID ExecuteHmi();
	VOID SetExecuteHmiFlag(BOOL bExecute = TRUE);
	BOOL IsExecuteHmi();

	VOID ExecuteOfflineMapProgram();	//v4.58A6

	//Set the status/error 
	VOID SetStatusMessage(const CString &szStatus);
	CString GetExtraStatus(); // v4.46T30 // yealy

	VOID SetLogItems(CString szStatus);

	VOID SetErrorMessage(const CString &szStatus);
	BOOL CheckAndBackupErrLogFile(ULONGLONG nFileSizeLimit = LOG_FILE_SIZE_LIMIT);	//v4.46T15

	//Set Custom Log
	VOID SetCtmLogMessage(const CString &szStatus);

	//Machine Startup
	VOID MachineStarted();
	VOID HmiControlAccessMode();

	//Set the machine measurement unit
	VOID SetMeasurementUnit(const CString &szMeasurementUnit);

	//Get the machine measurement unit
	CString GetMeasurementUnit();

	//Measurement Unit Conversions
	LONG ConvertFileUnitToDisplayUnit(DOUBLE dValue);
	LONG ConvertDisplayUnitToFileUnit(DOUBLE dValue);

	//SanAn attrivute to EquipTIme
	VOID SetEqTimeMapDieType(CString szType);

	//BSE VP Value of KeyIn 
	VOID SetVPValueKeyIn();	//4.53D5 
	//4.52D10 //Lumilets Label generate lbe file
	BOOL PrintRebealLabel(); 
	BOOL SaveLabelFileHeader(CString szPrinter, CString szLabelName, int nAngle, CString szFont, int nFontSize, int nBarcodeType, int nBarcodeSize);
	BOOL ReadRectFromLine(CString szLabel, CString szData, RECT& rect);
	BOOL SaveLabelFileData(int x1, int y1, int x2, int y2, BOOL bFormat, CString szText);
	BOOL GetOneRowExtraInformation(INT Column, CString &RawData);
	BOOL ParseRawDataTab(CString szRawData, CStringArray& szaRawDataByCol);
	BOOL ParseRawDataComma(CString szRawData, CStringArray& szaRawDataByCol);
	BOOL GetWaferSubFolderSummaryFileContent(CString &szBinHeaderData, CString &szCatCodeHeaderData);
	BOOL BackUpMapFileToOutputPath(CString szExistMapFilePathName);

	// Show and close the loading alert box
	VOID CreateLoadingAlert(BOOL bCreate = TRUE);		//v4.02T4
	VOID StartLoadingAlert();
	VOID CloseLoadingAlert();

	VOID OpenWaitAlert();
	VOID CloseWaitAlert();

	// Load from and backup to NVRAM (Hardware)
	BOOL InitMS899NVRAM();			//v3.74
	BOOL InitNuMotionNVRAM();		//v3.74
	VOID LoadFromNVRAM();
	VOID BackupToNVRAM();
	VOID ClearNVRAM();				// Clear the whole NVRAM

	//Application Features
	CString GetCustomerName();							//v2.63
	CString GetProductLine();							//v4.33T1	//PLSG
	BOOL GetFeatureStatus(CString szFuncName);	
	ULONG GetFeatureValue(CString szFuncName); 
	CString GetFeatureStringValue(CString szFuncName);
	BOOL CheckPLLMRebelPassword(CString szPassword);	//v3.70T3
	BOOL CheckSanAnProductionMode();					//v4.33T1
	BOOL IsPLLMRebel();
	BOOL IsToSaveMap();
	BOOL IsMapDetectSkipMode();
	LONG CheckLoginLevel();
	LONG GetBHZ1HomeOffset();							//v4.44A5
	LONG GetBHZ2HomeOffset();
	VOID SetBHZ1HomeOffset(LONG lOffsetZ);				//v4.46T26
	VOID SetBHZ2HomeOffset(LONG lOffsetZ);

// prescan relative code	B
	LONG	GetNoOfTimeReportPeriod();
	LONG	CopyPerformanceToServer(CString szSrcFile, CString szFileName);
	LONG	GetReportPeriodStartTime(LONG lIndex, INT &nHour, INT &nMinute);	// time in minutes
	BOOL	m_bEnablePrescanInterface;
	LONG	m_lWaferMapSortingpathCalTime;
	BOOL	m_bMapAdaptiveAlgor;
	BOOL	m_bPrescanRealignSameMap;
	BOOL	m_bPrescanEmptyUnmark;
	UINT GetPrescanWaferEdgeNum();
	BOOL GetScanMapIndexTolerance(DOUBLE &dRowTol, DOUBLE &dColTol);
	BOOL	IsRunTimeLoadPKGFile();	// to load packgae file after scan barcode and a new frame(map) loaded
// prescan relative code	E
	BOOL IsApplicationRunning(CString szAppName, UCHAR ucLimit, CONST BOOL bShowMsg = TRUE);

	//Bonding-related Date & Time
	VOID SaveLoadMapDate(CString szLoadMapDate);
	VOID SaveLoadMapTime(CString szLoadMapTime);

	VOID SaveMapStartDateTime();
	VOID MapIsStarted();
	VOID MapIsStopped();

	BOOL GetPath(CString &szPath);			// Get path via pop-up dialog
	VOID ShowApp(BOOL bMinimized = FALSE);	// Show application on top
	VOID ShowHmi();							// Show HMI on top
	BOOL GetColorWithDialog(COLORREF &ulReturnColour);

	//Update Map header info (Bin Spec Version & BinParamter)
	BOOL UpdateMapHeaderInfo(VOID);

	//SeoulSemi Update the RANK name
	BOOL SeoulSemiUpdateAllRankName();
	//Cut CSV
	BOOL ExtractDataInCSVRowLine(CString szInputLine, CString & szOutputWord, ULONG ulIndex, CString szRegex);
	
	BOOL CheckSananPasswordFile_old_3EDL();		//v4.51A3	//3eDL

	//Update Map LotNo
	BOOL UpdateMapLotNo(CString szLotNo);

	//Set & update progress bar
	BOOL SetProgressBarLimit(LONG lLimit);
	BOOL UpdateProgressBar(LONG lPercent);

	// Host communication
	VOID DisplayHostCommMessage(const CString &szStatus, BOOL bDisplay);
	BOOL SendHostMachineIDAck();
	BOOL SendHostLotNoAck();
	BOOL SendHostWaferEnd(CString szMessage);
	BOOL SendHostClearBin(CString szMessage);

	BOOL DecodeHostLotNo(CString szMessage);
	BOOL ReceiveHostUnknownMessage();

	//Log for cycle stop
	BOOL CheckAndBackupCycleStopLog(ULONGLONG nFileSizeLimit, BOOL bStopLog=TRUE);		//v4.42T5
	LONG LogCycleStopStatus(CString szMessage);

	//Burn In prestart action
	LONG BurnInPreStartOperation();
	
	//Check Format name
	VOID CheckOutputFileFormatName(CString &szFormatName);
	VOID CheckWaferEndFileFormatName(CString &szFormatName);

	//Cycle Log
	VOID EnableCycleLog(CString szMessage);

	// For Genesis & Epileds in 		//v2.56
	BOOL DisplaySortFileContent(LONG lTitleID, CONST CString szOldFile, CONST CString szNewFile);
	
	// Machine Time Functions
	BOOL ValidateReportTimeInput(CString szDate);
	BOOL InitMachineTime();
	BOOL InitMachineTime2();

	BOOL InitToolsUsageRecord();

	CString GetPKGFilename();
	CString GetPPKGFilenameByLotNo(CString szLotNo);
	BOOL LoadPKGFile(BOOL bIsPortablePKGFile , CString szFilename, 
					 BOOL bPopupConfirmDialog, BOOL bPopupCompleteDialog, 
					 BOOL bIsAutoMode, BOOL bEnableSECSGEM = TRUE);
	BOOL LoadPKGFileByTraceabilityERPNo_PLSG_DieFab(CString szERPPartNo);		//PLSG traceability
	BOOL LoadPKGFileByTraceabilityERPNo_PLLM(CString szERPPartNo);				//PLLM traceability		//v4.33T1
	BOOL LoadPKGFileByTraceabilityERPNo_PLLM_Lumiramic(CString szERPPartNo);	//PLLM traceability		//v4.35T1
	BOOL RestorePrDataFromDevice(CString szDeviceFile, BOOL bLoadWPR = TRUE, BOOL bLoadWFT = TRUE);
	BOOL DeletePrRecordFiles(CONST BOOL bWaferPr);
	BOOL LoadPPKGFileByWaferTypeID(CString szTypeID, CString &szErrorMsg);
	BOOL LoadPPKGFile(CString szFilename, BOOL bNoResetNVRam=FALSE);
	BOOL LoadBinFile(CString szFilename);
	BOOL RestorePPKG(CString szDevicePath, CString szDeviceFile, BOOL bNoResetNVRam=FALSE);

	BOOL ReadAmiFile(CAmiFileDecode &oAmiFileDecoder, CString szFilename);
	BOOL AmiFileChecking(CAmiFileDecode &oAmiFileDecoder);
	LONG LoadBinSummaryFile(CString szFilename);

	BOOL SearchAndRemoveFiles(CString szPath, USHORT usDiffDays = 30,
		BOOL bClearNow = FALSE, CString szFileExtension = "*", BOOL bSearchSubFolder = TRUE);		//v4.46T13

	LONG LoadFileFormatSelectInNewLot();
	LONG SaveFileFormatSelectInNewLot();

	BOOL MS_LoadLastState();
	BOOL MS_SaveLastState();

	//v3.98T5
	VOID SaveStopBondTsToRegistry(const BOOL bBHZ1);
	INT CheckRegTsForPostBond(const BOOL bBHZ1);

	BOOL CheckDiskSize(LPCSTR lpDiskName, ULONG &lDiskSpaceUsed, ULONG &lDiskSpaceRemain);

	// Set Machine Time Wafer Id
	VOID SetMachineTimeLoadNewMap(BOOL bLoadNewMap, CString szWaferId);

	BOOL GenerateColletUsageRecordFile(CColletRecord &oColletRecord);
	BOOL GenerateEjectorUsageRecordFile(CEjectorRecord &oEjectorRecord);

	VOID SetMS896ADataBlock(CMS896ADataBlock &oMS896ADataBlk);
	VOID SetBinTableDataBlock(CBinTableDataBlock &oBinTableDataBlk);
	VOID SetBondHeadDataBlock(CBondHeadDataBlock &oBondHeadDataBlk);
	VOID SetWaferTableDataBlock(CWaferTableDataBlock &oWaferTableDataBlk);
	VOID SetWaferPrDataBlock(CWaferPrDataBlock &oWaferPrDataBlk);
	VOID SetBondPrDataBlock(CBondPrDataBlock &oBondPrDataBlk);
	VOID SetBinLoaderDatatBlock(CBinLoaderDataBlock &oBinLoaderDataBlk);
	VOID SetSafetyDataBlock(CSafetyDataBlock &oSafetyDataBlk);

	BOOL GetMachineTimes(CString &szMacUpTime, CString &szMacIdleTime, CString &szMacSetupTime, 
						 CString &szMacRunTime, CString &szMacAlarmTime, CString &szMacAssitTime,
						 CString &szMacDownTime);

	BOOL GenerateFaultRecoverAlarmFile(CString szName, CString szNameID, CString szStation);  //v4.51D2 //Electech3E(DL) 15

	CString GetUsername();
	CString GetPasswordFilePath(); //1.08S

	BOOL CheckIsAllBinCleared();
	BOOL CheckIsAllBinClearedNoMsg();
	BOOL CheckIsAllBinClearedWithDieCountConstraint(ULONG ulDieCount);

	BOOL GetGmpErrorLogToDrive();

	BOOL CheckMatrixDongle();				//v3.33T5
	
	BOOL IsUsingEngineerMode();		//andrewng
	CString GetEngineerPassword();			//v4.51A2

	//v4.42T16
	BOOL CreateMSLicenseStringFile();
	BOOL RegisterMSLicenseKey();
	//void LicenseCheckCallbackFunc(int ret, int nLeft);
	BOOL ValidateMSLicenseKey();
	BOOL ValidateMSLicenseKeyFeature();
	BOOL m_bIsMSLicenseRegistered;			//v4.48A2
	static BOOL m_bIsMSLicenseKeyChecked;

	BOOL ManualInputHmiInfo(CString *szInfo, ULONG ulAppMsgCode, CString szHmiTemp);
	BOOL HmiStrInputKeyboard_OnlyContent(CString szTitle, CString &szContent, BOOL bPasswordMode=FALSE);
	BOOL HmiStrInputKeyboard(CString szTitle, CString szVarName, BOOL bPasswordMode, CString &szContent);		//v4.17T7
	BOOL UpdateHmiVariableData(CString szVariableName, CString szValue);
	LONG ReadLabelConfig();

	BOOL IsStopAlign();			//v4.05	//Klocwork
	BOOL IsBLBCUseOldGoStop();
	BOOL IsUseHmiMap4HomeFPC();

	VOID GetAlarmCodeAction(CString szAlarmCode, LONG &lAction);
	BOOL LoadAlarmInformation(CString szFilename);
	VOID DumpLoadAlarmInformation(CString szFilename);

	VOID SetAlarmLamp_Red_Only(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_RedYellow_Only(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_Red(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_Yellow(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_Green(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_RedYellow(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_YellowGreen(BOOL bBlink, BOOL bLockAlarmLamp); //4.51D8
	VOID SetAlarmLamp_YellowGreenNoBuzzer(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_OffLight(BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_YellowBuzzer(BOOL bBlink, BOOL bLockAlarmLamp);

	VOID SetAlarmLamp_Back(LONG lOrgStatus, BOOL bBlink, BOOL bLockAlarmLamp);
	LONG GetAlarmLamp_Status();
	VOID SetAlarmLamp_Status(LONG lLevel, BOOL bBlink , BOOL bLockAlarmLamp);
	
	BOOL ExportNVRAMDataToTxtFile(CONST CString szFileName);	//v4.40T15	//Cree HuiZhou
	BOOL ImportNVRAMDataFromTxtFile(CONST CString szFileName);

	UINT GetEnableBMapBondAreaFromRegistry();					//v4.47T9	//WH SanAn
	VOID SetEnableBMapBondAreaToRegistry(UINT nOption);			//v4.47T9	//WH SanAn

	BOOL EnableScreenButton(BOOL bTest);  // 4.51D1 Secs

	//SECS/GEM
	BOOL IsSecsGemInit();
	VOID UpdateAllSGVariables();
	VOID UpdateSGEquipmentConst(int nECID);

    LONG SC_CmdDownloadPPG(IPC_CServiceMessage& svMsg);  //Called by menu
	LONG SC_CmdRequestPPG(IPC_CServiceMessage& svMsg);  //Called by menu

	BOOL CheckPasswordToGo(LONG lUserTpye = 0);

	//v4.62A21	//ChangeLight XM
	BOOL CheckSananPasswordFile(BOOL bInputUserID, BOOL &bCancel, LONG lAccessLevel = 0);
	BOOL CheckSananPasswordFile_old(BOOL bInputUserID, LONG lAccessLevel = 0);

	BOOL IsStopLoadUnloadLoopTest()
	{
		return m_bStopLoadUnloadLoopTest;
	}
	VOID ClearStopLoadUnloadLoopTest()
	{
		m_bStopLoadUnloadLoopTest = FALSE;
	}

	LONG _round(double val);

	CWaferTable *GetWaferTable()
	{
		return m_pWaferTable;
	}

	CBondHead *GetBondHead()
	{
		return m_pBondHead;
	}

	CBinTable *GetBinTable()
	{
		return m_pBinTable;
	}

	CWaferPr *GetWaferPr()
	{
		return m_pWaferPr;
	}

	CBondPr *GetBondPr()
	{
		return m_pBondPr;
	}

	CSecsComm *GetSecsComm()
	{
		return m_pSecsComm;
	}

	CWaferLoader *GetWaferLoader()
	{
		return m_pWaferLoader;
	}

	CSafety *GetSafety()
	{
		return m_pSafety;
	}

	CBinLoader *GetBinLoader()
	{
		return m_pBinLoader;
	}

	CPrGeneral	*GetPrGeneral()
	{
		return m_pPrGeneral;
	}

	BOOL IsHmiExecuted()
	{
		return m_bHmiExecuted;
	}


	DECLARE_MESSAGE_MAP()
};

extern CMS896AApp theApp;
