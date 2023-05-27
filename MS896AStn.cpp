/////////////////////////////////////////////////////////////////
// MS896AStn.cpp : interface of the MS896AStn class
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

#include "stdafx.h"
#include "MS896A.h"
#include "MS896AStn.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"
#include <math.h>
#include <sys/timeb.h>
#include "GenerateDatabase.h"
#include "GenerateWaferDatabase.h"
#include "QueryWaferDatabase.h"
#include "QueryDatabase.h"
#include "OutputFileFactory.h"
#include "FactoryMap.h"
#include "OutputFileInterface.h"
#include "WaferEndFileFactory.h"
#include "WaferEndFactoryMap.h"
#include "WaferEndFileInterface.h"
//#include "Encryption.h"
#include "PrescanInfo.h"
#include "SC_Constant.h"			//OsramTrip 8/22
#include "MS_SecCommConstant.h"		//OsramTrip 8/22
#include "AlarmCodeTable.h"
#include "WT_SubRegion.h"
#include "WaferMap.h"
#include <gdiplus.h>
#include <AtlImage.h>
#include "MathFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Static member variables of CMS896AStn Class
BOOL	CMS896AStn::m_bIfGenWaferEndFile	= FALSE;
BOOL	CMS896AStn::m_bAutoLoadWaferMap	= FALSE;
CWaferMapWrapper CMS896AStn::m_WaferMapWrapper;			// Wafer Map Object
CPreBondEvent CMS896AStn::m_WaferMapEvent;				// Wafer map event
CWaferMapWrapper CMS896AStn::m_BinMapWrapper;			// Bin Map Object
CPreBondEventBin	CMS896AStn::m_BinMapEvent;				// Wafer map event
CString CMS896AStn::m_szOsramSDF = "";

BOOL CMS896AStn::m_bWaferMapWarmStartFail = FALSE;		//v4.22T2
BOOL CMS896AStn::m_bSECSWaferMapLoadFinish	= FALSE;	//v4.59A13
BOOL CMS896AStn::m_bSECSWaferMapUpLoadFinish= FALSE;	//v4.59A13
BOOL CMS896AStn::m_bSECSWaferMapUpLoadAbort	= FALSE;	//v4.59A13

CStringArray		CMS896AStn::m_szaGradeItemList;
BOOL				CMS896AStn::m_bCPItemMapToTop;

UCHAR			CMS896AStn::m_ucMS100PrDisplayID = MS_PR_DISPLAY_DEFAULT;

LONG			CMS896AStn::m_lAPLastDone	= 0;

//Output File Generating
CObArray CMS896AStn::m_objaGenerateDatabase;
CObArray CMS896AStn::m_objaQueryDatabase;

CGenerateWaferDatabase	CMS896AStn::m_GenerateWaferDatabase;
CQueryWaferDatabase		CMS896AStn::m_QueryWaferDatabase;

BOOL CMS896AStn::m_bDisableBH	= FALSE;
BOOL CMS896AStn::m_bDisableBT	= FALSE;
BOOL CMS896AStn::m_bDisableBL	= FALSE;
BOOL CMS896AStn::m_bDisableNL	= FALSE;				//andrewng //2020-0707
BOOL CMS896AStn::m_bDisableWL	= FALSE;
BOOL CMS896AStn::m_bDisableWT	= FALSE;
BOOL CMS896AStn::m_bDisableWLWithExp = FALSE;
BOOL CMS896AStn::m_bAppInitialized	= FALSE;			//v4.02T3
//	Wafer Loader Operation Option
ULONG	CMS896AStn::m_ulSequenceMode	= 0;
CTime	CMS896AStn::m_stMachineStopTime;				//v4.59A41	//Finisar TX
LONG	CMS896AStn::m_lABDummyCounter	= 0;			//andrewng //2020-0727

BOOL CMS896AStn::m_bIfGenRefDieStatusFile;
BOOL CMS896AStn::m_bIfGenLabel;
BOOL CMS896AStn::m_bUseRefDieCheck;
BOOL CMS896AStn::m_bUsePostBond;
BOOL CMS896AStn::m_bIfEnableCharDie;
BOOL CMS896AStn::m_bUseTcpIp;
BOOL CMS896AStn::m_bEnablePolyonDie;
BOOL CMS896AStn::m_bEnableRhombusDie= FALSE;
BOOL CMS896AStn::m_bEnableBlockFunc;

BOOL CMS896AStn::m_bAoiEnableOcrDie;
BOOL CMS896AStn::m_bRebelManualAlign;
int	 CMS896AStn::m_siBarPitchX = 0;				// Bar Pitch X Enc Pos
int	 CMS896AStn::m_siBarPitchY = 0;				// Bar Pitch Y Enc Pos

BOOL	CMS896AStn::m_bEnablePickAndPlace;
BOOL	CMS896AStn::m_bReadyToRealignOnWft;
//	prescan relative variables
LONG	CMS896AStn::m_nPrescanLastWftPosnX	= 0;
LONG	CMS896AStn::m_nPrescanLastWftPosnY	= 0;
LONG	CMS896AStn::m_nPrescanNextMapRow	= 0;
LONG	CMS896AStn::m_nPrescanNextMapCol	= 0;
LONG	CMS896AStn::m_nPrescanLastMapRow	= 0;
LONG	CMS896AStn::m_nPrescanLastMapCol	= 0;

LONG	CMS896AStn::m_lAlignFrameRow	= 0;
LONG	CMS896AStn::m_lAlignFrameCol	= 0;
LONG	CMS896AStn::m_lNextFrameRow		= 0;
LONG	CMS896AStn::m_lNextFrameCol		= 0;
LONG	CMS896AStn::m_lLastFrameRow		= 0;
LONG	CMS896AStn::m_lLastFrameCol		= 0;
LONG	CMS896AStn::m_lWftScanIndexAndDelayTime = 0;
LONG	CMS896AStn::m_lScanFrameMinRow	= 0;
LONG	CMS896AStn::m_lScanFrameMinCol	= 0;
LONG	CMS896AStn::m_lScanFrameMaxRow	= 100;
LONG	CMS896AStn::m_lScanFrameMaxCol	= 100;

BOOL	CMS896AStn::m_bAFGridSampling		= FALSE;
LONG	CMS896AStn::m_lPrescanLastGrabRow	= 0;
LONG	CMS896AStn::m_lPrescanLastGrabCol	= 0;
BOOL	CMS896AStn::m_bPrescanLastGrabbed	= FALSE;
LONG	CMS896AStn::m_lAreaPickStage		= 0;	// 0 start or new map, 1 just align or prescanning, 2 area picking
UCHAR	CMS896AStn::m_ucPrescanEdgeSize		= 0;
UCHAR	CMS896AStn::m_ucPrescanEdgeGrade	= 0;
INT		CMS896AStn::m_nPickListIndex		= 0;
INT		CMS896AStn::m_nDieSizeX	= 0;
INT		CMS896AStn::m_nDieSizeY	= 0;

//	4.24TX 3
LONG	CMS896AStn::m_lFocusZState			= 0;
LONG	CMS896AStn::m_lBackLightZStatus		= 0;	// 0 default(down), 1 from down to up, 2 up position, 3 from up to down
LONG	CMS896AStn::m_lBackLightTravelTime 	= 5000;	// between up and standby
DOUBLE  CMS896AStn::m_dBackLightZDownTime	= 0;
DOUBLE	CMS896AStn::m_dScanStartTime		= 0;
DOUBLE	CMS896AStn::m_dScanEndTime			= 0;
CTime	CMS896AStn::m_stScanStartCTime		= CTime::GetCurrentTime();
CTime	CMS896AStn::m_stScanEndCTime		= CTime::GetCurrentTime();
LONG	CMS896AStn::m_lTimeSlot[];
LONG	CMS896AStn::m_lMapValidMinRow		= 0;
LONG	CMS896AStn::m_lMapValidMinCol		= 0;
LONG	CMS896AStn::m_lMapValidMaxRow		= 0;
LONG	CMS896AStn::m_lMapValidMaxCol		= 0;
LONG	CMS896AStn::m_lPrescanSortingTotal	= 0; 

LONG	CMS896AStn::m_lWT1PositionStatus = 0;	// 0 default(unload), 1 from unload to home, 2 home position, 3 from home to unload
LONG	CMS896AStn::m_lWT2PositionStatus = 0;	// 0 default(unload), 1 from unload to home, 2 home position, 3 from home to unload
BOOL	CMS896AStn::m_bFastHomeScanEnable = FALSE;
BOOL	CMS896AStn::m_bIMPrescanReferDie;
LONG	CMS896AStn::m_lPrescanHomeDieRow;
LONG	CMS896AStn::m_lPrescanHomeDieCol;
LONG	CMS896AStn::m_lPrescanHomeWaferX	= 0;
LONG	CMS896AStn::m_lPrescanHomeWaferY	= 0;
LONG	CMS896AStn::m_lAtBondDieGrade		= -1;
LONG	CMS896AStn::m_lAtBondDieRow			= -1;
LONG	CMS896AStn::m_lAtBondDieCol			= -1;

DOUBLE	CMS896AStn::m_dPrescanLFSizeX;
DOUBLE	CMS896AStn::m_dPrescanLFSizeY;
LONG	CMS896AStn::m_lPrescanMovePitchX	= 0;
LONG	CMS896AStn::m_lPrescanMovePitchY	= 0;
BOOL	CMS896AStn::m_bEnablePrescan;
BOOL	CMS896AStn::m_bAOINgPickPartDie;
BOOL	CMS896AStn::m_bNGPick				= FALSE;			
BOOL	CMS896AStn::m_bMS60NGPick			= FALSE;			
BOOL	CMS896AStn::m_bAutoUploadOutputFile	= FALSE;
BOOL	CMS896AStn::m_bWaferPrUseMxNLFWnd	= FALSE;			//v4.43T2	//SanAn MS100PlusII
BOOL	CMS896AStn::m_bWaferPrMS60LFSequence	= FALSE;		//v4.47T3	//MS60
BOOL	CMS896AStn::m_bMS90WTPrVibrateTest	= FALSE;			//v4.59A40
//4.53D18
BOOL CMS896AStn::m_bWaferMapUseDieMixing		= FALSE;
LONG CMS896AStn::m_lBuildShortPathInScanning	= 0;
BOOL CMS896AStn::m_bPrescanWaferEnd4ShortPath	= FALSE;
BOOL CMS896AStn::m_bShowNgDieSubGrade		= FALSE;
BOOL CMS896AStn::m_bSaveNgDieSubGrade		= TRUE;
LONG CMS896AStn::m_lWftAdvSamplingStage		= 0;

BOOL CMS896AStn::m_bEnableAutoCycleLog		= FALSE;
DOUBLE	CMS896AStn::m_dPreviousUpdateTime 	= 0;

CString	CMS896AStn::m_szCycleMessageWT		= "";
CString	CMS896AStn::m_szCycleMessageWP		= "";
CString	CMS896AStn::m_szCycleMessageWL		= "";
CString	CMS896AStn::m_szCycleMessageBT		= "";
CString	CMS896AStn::m_szCycleMessageBP		= "";
CString	CMS896AStn::m_szCycleMessageBH		= "";
CString	CMS896AStn::m_szCycleMessageBL		= "";
CString	CMS896AStn::m_szCycleMessageST		= "";
CString	CMS896AStn::m_szCycleMessageSG		= "";	
CString	CMS896AStn::m_szCycleMessageNL		= "";		//andrewng //2020-0708

LONG CMS896AStn::m_lMapPrescanAlignWafer;
BOOL CMS896AStn::m_bMapDummyPrescanAlign;
BOOL CMS896AStn::m_bIsPrescanning;
BOOL CMS896AStn::m_bIsPrescanned;
BOOL CMS896AStn::m_bPrescanBlkPickAlign;
BOOL CMS896AStn::m_bEnableSCNFile;
BOOL CMS896AStn::m_bEnablePsmFileExistCheck;
BOOL CMS896AStn::m_bEnableBatchIdFileCheck;
BOOL CMS896AStn::m_bDisableWaferMapFivePointCheckMsgSelection;
BOOL CMS896AStn::m_bUseOptBinCountDynAssignGrade;
BOOL CMS896AStn::m_bOpenMatchPkgNameCheck;
BOOL CMS896AStn::m_bIsPostBondDataExported;
BOOL CMS896AStn::m_bDisplaySortBinItemMsg;
BOOL CMS896AStn::m_bSearchCompleteFilename;
BOOL CMS896AStn::m_bLoadMapSearchDeepInFolder;
BOOL CMS896AStn::m_bDisableBinSNRFormat;
BOOL CMS896AStn::m_bDisableClearCountFormat;
BOOL CMS896AStn::m_bEnableDynMapHeaderFile;
BOOL CMS896AStn::m_bBackupTempFile;
BOOL CMS896AStn::m_bBackupOutputTempFile;
BOOL CMS896AStn::m_bForceDisableHaveOtherFile;
BOOL CMS896AStn::m_bEnableManualUploadBinSummary;
BOOL CMS896AStn::m_bGenerateAccumulateDieCountReport;
BOOL CMS896AStn::m_bEnablePRDualColor;
BOOL CMS896AStn::m_bEnablePRAdaptWafer;
BOOL CMS896AStn::m_bCounterCheckWithNormalDieRecord;
BOOL CMS896AStn::m_bAddWaferIdInMsgSummaryFile;
BOOL CMS896AStn::m_bWaferEndFileGenProtection;
BOOL CMS896AStn::m_bAutoGenWaferEndFile;
BOOL CMS896AStn::m_bEnableAlignWaferImageLog;
BOOL CMS896AStn::m_bPrescanLogNgImage;
BOOL CMS896AStn::m_bEnableBinOutputFilePath2;
BOOL CMS896AStn::m_bEnableGradeMappingFile;
BOOL CMS896AStn::m_bEnablePickNPlaceOutputFile;
BOOL CMS896AStn::m_bAddDieTimeStamp;
BOOL CMS896AStn::m_bCheckProcessTime;
BOOL CMS896AStn::m_bAddSortingSequence;
BOOL CMS896AStn::m_bUseClearBinByGrade;
BOOL CMS896AStn::m_bOutputFormatSelInNewLot;
BOOL CMS896AStn::m_bBTAskBLChangeGrade	= FALSE;			//v4.43T9	//WH SanAn
LONG CMS896AStn::m_bBTAskBLBinFull		= FALSE;			//v4.43T9	//WH SanAn
BOOL CMS896AStn::m_bKeepOutputFileLog;
BOOL CMS896AStn::m_bChangeGradeBackupTempFile;
BOOL CMS896AStn::m_bRemoveBackupOutputFile;
BOOL CMS896AStn::m_bAutoGenBinBlkCountSummary;
BOOL CMS896AStn::m_bEnableExtraClearBinInfo;
BOOL CMS896AStn::m_bEnableEmptyBinFrameCheck;
BOOL CMS896AStn::m_bEnableResetMagzCheck;
BOOL CMS896AStn::m_bEnableWaferLotFileProtection;
BOOL CMS896AStn::m_bForceClearMapAfterWaferEnd;
BOOL CMS896AStn::m_bWafflePadBondDie;
BOOL CMS896AStn::m_bWafflePadIdentification;
BOOL CMS896AStn::m_bEnableToolsUsageRecord;
BOOL CMS896AStn::m_bEnableItemLog;
BOOL CMS896AStn::m_bEnableMachineReport;
BOOL CMS896AStn::m_bEnableAlarmLampBlink;
BOOL CMS896AStn::m_bEnableAmiFile;
BOOL CMS896AStn::m_bEnableBarcodeLengthCheck;
BOOL CMS896AStn::m_bEnableInputCountSetupFile;
BOOL CMS896AStn::m_bForceClearBinBeforeNewLot;
BOOL CMS896AStn::m_bNewLotCreateDirectory;
BOOL CMS896AStn::m_bNoPRRecordForPKGFile;
BOOL CMS896AStn::m_bBLOut8MagConfig		= FALSE;				//v3.82	//MS100 8mag config
LONG CMS896AStn::m_lUnloadPhyPosX;
LONG CMS896AStn::m_lUnloadPhyPosY;
LONG CMS896AStn::m_lUnloadPhyPosX2;
LONG CMS896AStn::m_lUnloadPhyPosY2;
LONG CMS896AStn::m_lSprialSize;	
LONG CMS896AStn::m_lBTUnloadPos_X = 0;
LONG CMS896AStn::m_lBTUnloadPos_Y = 0;
LONG CMS896AStn::m_lBTUnloadPos_X2 = 0;
LONG CMS896AStn::m_lBTUnloadPos_Y2 = 0;
LONG CMS896AStn::m_lBTBarcodePos_X	= 0;
LONG CMS896AStn::m_lBTBarcodePos_Y	= 0;
LONG CMS896AStn::m_lBT2OffsetX	= 0;
LONG CMS896AStn::m_lBT2OffsetY	= 0;

//BOOL CMS896AStn::m_bEnableMouseControl;
BOOL CMS896AStn::m_bCEMark				= FALSE;				//v4.06
BOOL CMS896AStn::m_bEMOChecking			= FALSE;				//v3.91		//MS100
BOOL CMS896AStn::m_bDBHHeatingCoilFcn	= FALSE;				//v4.49A5	//MS100P3/MS60/MS90
BOOL CMS896AStn::m_bDBHHeatingCoilFcnInit	= FALSE;			//v4.59A21
BOOL CMS896AStn::m_bDBHThermostat		= FALSE;				//v4.49A5	//MS100P3/MS60/MS90
BOOL CMS896AStn::m_bAutoChangeCollet	= FALSE;				//v4.50A5	//MS60 AutoChgCollet Fcn
BOOL CMS896AStn::m_bPBErrorCleanCollet	= FALSE;				//v4.52A1	//Semitek & SanAn
ULONG CMS896AStn::m_ulJoyStickMode		= MS899_JS_MODE_HW;		//v3.76		//0=HW-JS, 1=MOVE-JC, 2=PR-JS
LONG CMS896AStn::m_lJsTableMode			= 0;					// 0=WT,  1=BT
LONG CMS896AStn::m_lJsBinTableInUse		= 0;					// 0=BT1, 1=BT2		//v4.17T1
LONG CMS896AStn::m_lJsWftInUse			= 0;					// 0=WT1, 1=WF2
BOOL CMS896AStn::m_bCheckWaferLimit		= TRUE;					//
LONG CMS896AStn::m_lJoystickLevel		= 1;					// Joystick Speed Level
BOOL CMS896AStn::m_bJoystickOn			= FALSE;				// Joystick state
BOOL CMS896AStn::m_bXJoystickOn			= FALSE;				// X-joystick state
BOOL CMS896AStn::m_bYJoystickOn			= FALSE;				// Y-joystick state
BOOL CMS896AStn::m_bIsCircleLimit		= TRUE;
UCHAR CMS896AStn::m_ucWaferLimitType	= 0;
LONG CMS896AStn::m_lWaferPolyLimitNo	= 3;
LONG CMS896AStn::m_lWaferPolyLimitX[MS_WAFER_LIMIT_POINTS];
LONG CMS896AStn::m_lWaferPolyLimitY[MS_WAFER_LIMIT_POINTS];
LONG CMS896AStn::m_lWaferCenterX		= 0;			
LONG CMS896AStn::m_lWaferCenterY		= 0;
LONG CMS896AStn::m_lWaferSize			= 0;
//	ES101_XU	to base and need offset need table in use and ejector or not
LONG CMS896AStn::m_lWT2OffsetX			= (-487 - 407) * 1000 * 2;	// back light 407mm to left WT1 and -487 to right WT2
LONG CMS896AStn::m_lWT2OffsetY			= 0;
LONG CMS896AStn::m_lEjtOffsetX			= 0;// 80*1000*2;			// 80 mm at right hand of back light
LONG CMS896AStn::m_lEjtOffsetY			= 0;
LONG CMS896AStn::m_lProbeOffsetX		= 0;
LONG CMS896AStn::m_lProbeOffsetY		= 0;
LONG CMS896AStn::m_lHomeDiePhyPosX		= 0;
LONG CMS896AStn::m_lHomeDiePhyPosY		= 0;
LONG CMS896AStn::m_lWT2HomeDiePhyPosX	= 0;
LONG CMS896AStn::m_lWT2HomeDiePhyPosY	= 0;
INT	 CMS896AStn::m_nWTInUse				= 0;					//0=WT1 (1=WT2 for ES101)
BOOL CMS896AStn::m_bDisableWT2InAutoBondMode = FALSE;	//v4.24T11
BOOL CMS896AStn::m_bContourPreloadMap	= FALSE;
LONG CMS896AStn::m_lHomeDieMapRow		= 0;
LONG CMS896AStn::m_lHomeDieMapCol		= 0;
//v4.50A1
LONG CMS896AStn::m_lWafXNegLimit		= 0;
LONG CMS896AStn::m_lWafXPosLimit		= 0;
LONG CMS896AStn::m_lWafYNegLimit		= 0;
LONG CMS896AStn::m_lWafYPosLimit		= 0;

BOOL	CMS896AStn::m_bRenameMapNameAfterEnd;
BOOL	CMS896AStn::m_bAlwaysLoadMapFileLocalHarddisk;

LONG	CMS896AStn::m_lSetDetectSkipMode = 0;
BOOL	CMS896AStn::m_bEnableAssocFile;
CString CMS896AStn::m_szAssocFileExt;

BOOL CMS896AStn::m_bUseBinMultiMgznSnrs					= FALSE;
BOOL CMS896AStn::m_bEjNeedReplacement					= FALSE;
BOOL CMS896AStn::m_bEjPinNeedReplacementAtUnloadWafer	= FALSE;

ULONG CMS896AStn::m_ulIgnoreGrade = 0;
BOOL CMS896AStn::m_bCheckIgnoreGradeCount = FALSE;
BOOL CMS896AStn::m_bNoIgnoreGradeOnInterface = FALSE;

//Sync Move Via Ej Cmd Dist	//MS100
LONG CMS896AStn::m_lEjectMoveLength = 0;
FLOAT CMS896AStn::m_fFifthOrderPos_Ej[4096];
FLOAT CMS896AStn::m_fFifthOrderVel_Ej[4096];
FLOAT CMS896AStn::m_fFifthOrderAcc_Ej[4096];
FLOAT CMS896AStn::m_fFifthOrderJerk_Ej[4096];

//For Mouse Joystick
INT		CMS896AStn::m_nMouseMode			= MOUSE_STOP;
DOUBLE	CMS896AStn::m_dMouseClickX			= 0.0;	
DOUBLE	CMS896AStn::m_dMouseClickY			= 0.0;
INT		CMS896AStn::m_nMouseDragState		= PR_MOUSE_EVENT_STATE_UP;
DOUBLE	CMS896AStn::m_dMouseDragDist		= 0.0;
DOUBLE	CMS896AStn::m_dMouseDragAngle		= 0.0;
LONG	CMS896AStn::m_lMouseJoystickSpeed	= MOUSE_JOY_SPEED_MEDIUM;
LONG	CMS896AStn::m_lMouseDragDirection	= MOUSE_JOY_DIRECTION_NONE;

BOOL CMS896AStn::m_bBinFrameStatusSummary;
BOOL CMS896AStn::m_bPortablePKGInfoPage;
BOOL CMS896AStn::m_bSupportPortablePKGFile;
BOOL CMS896AStn::m_bEnableSummaryPage;

BOOL CMS896AStn::m_bWaferLotWithBinSummaryFormat;
BOOL CMS896AStn::m_bEnableWaferLotLoadedMapCheck;

BOOL CMS896AStn::m_bEnableAutoDieTypeFieldnameChek;
BOOL CMS896AStn::m_bClearDieTypeFieldDuringClearAllBin;
CString CMS896AStn::m_szDieTypeFieldnameInMap;
LONG	CMS896AStn::m_lOTraceabilityFormat	= 0;	//v3.19T1	//0=NONE. 1=LED, 2=SUBMOUNT

BOOL	CMS896AStn::m_bPackageFileMapHeaderCheckFunc;
CString CMS896AStn::m_szPackageFileMapHeaderCheckString;
BOOL	CMS896AStn::m_bUpdateWaferMapHeader;

ULONG CMS896AStn::m_ulDefaultBinSNRFormat;
ULONG CMS896AStn::m_ulDefaultClearCountFormat;

BOOL CMS896AStn::m_bUseEmptyCheck;				//Block2
BOOL CMS896AStn::m_bUseAutoManualAlign;			//Block2
BOOL CMS896AStn::m_bUseReVerifyRefDie;			//Cree
BOOL CMS896AStn::m_bNonBlkPkEndVerifyRefDie;
BOOL CMS896AStn::m_bManualAlignReferDie;
BOOL CMS896AStn::m_bPrescanDiePitchCheck;
LONG CMS896AStn::m_lMnNoDieGrade;
BOOL CMS896AStn::m_bUseRefDieFaceValueCheck;	//v4.48A26	//Avago

BOOL	CMS896AStn::m_bAlarmTwice		= FALSE;
BOOL	CMS896AStn::m_bEnableAlarmLog	= FALSE;
BOOL	CMS896AStn::m_bACF_AlarmLogPath	= FALSE;
CString	CMS896AStn::m_szAlarmLogPath	= "";

BOOL CMS896AStn::m_bCoverSensorAlwaysOn		= FALSE;
BOOL CMS896AStn::m_bEnableFrontCoverLock	= FALSE;
BOOL CMS896AStn::m_bEnableSideCoverLock		= FALSE;
BOOL CMS896AStn::m_bEnableBinElevatorCoverLock = FALSE;
LONG CMS896AStn::m_lBinMixCount = 0;
// prescan relative code	B
BOOL	CMS896AStn::m_bMapOHFlip	= FALSE;
BOOL	CMS896AStn::m_bMapOVFlip	= FALSE;
USHORT	CMS896AStn::m_usMapAngle	= 0;
BOOL CMS896AStn::m_bPrescanBlkPickEnabled	= FALSE;
BOOL CMS896AStn::m_bBlkFuncEnable	= FALSE;
BOOL CMS896AStn::m_bFullRefBlock	= FALSE;
BOOL CMS896AStn::m_bAdaptPredictAlgorithm;
BOOL CMS896AStn::m_bGeneralStopCycle	= FALSE;
BOOL CMS896AStn::m_bPrescan2ndTimeStart	= FALSE;
BOOL CMS896AStn::m_bPrescanTwiceEnable	= FALSE;
BOOL CMS896AStn::m_bHmiToUseCharDie		= TRUE;
BOOL CMS896AStn::m_bOCRBarwaferNoMap	= FALSE;
CString	CMS896AStn::m_szAoiOcrPrValue	= "??????";
LONG CMS896AStn::m_lPrescanVerifyResult	= 0;
BOOL CMS896AStn::m_bWprWithAF_Z;
BOOL CMS896AStn::m_bBinFrameCheckBCFail	= FALSE;
BOOL CMS896AStn::m_bPsmEnableState		= FALSE;
BOOL CMS896AStn::m_bDisablePsm			= FALSE;
BOOL CMS896AStn::m_bSmallMapSortRpt		= FALSE;
BOOL CMS896AStn::m_bEnable6InchWaferTable = FALSE;
BOOL CMS896AStn::m_bES100ByMapIndex		= FALSE;
LONG CMS896AStn::m_nPrescanIndexStepRow	= 1;
LONG CMS896AStn::m_nPrescanIndexStepCol	= 1;
BOOL CMS896AStn::m_bES100DisableSCN		= FALSE;
DOUBLE	CMS896AStn::m_dStartWaferAlignTime	= 0;
BOOL	CMS896AStn::m_bEnableSCNCheck			= FALSE;	
LONG	CMS896AStn::m_lOutputScanSummary		= 0;
CString	CMS896AStn::m_szOutputScanSummaryPath	= gszROOT_DIRECTORY + _T("\\OutputFiles\\PrescanSummary");
CString CMS896AStn::m_szScanAoiMapFormat;
BOOL	CMS896AStn::m_bScnLoaded		= FALSE;			// Flag to indicate whether SCN file is loaded


BOOL	CMS896AStn::m_bNgPickBadCut				= FALSE;
BOOL	CMS896AStn::m_bNgPickDefect				= FALSE;
BOOL	CMS896AStn::m_bNgPickEdgeDie			= FALSE;
BOOL	CMS896AStn::m_bToPickDefectDie			= FALSE;
BOOL	CMS896AStn::m_bCheckDieOrientation		= FALSE;
BOOL	CMS896AStn::m_bEnableCheckDieOrientation = FALSE;
UCHAR	CMS896AStn::m_ucDummyPrescanPNPGrade	= 151;
BOOL	CMS896AStn::m_bPrescanEmptyAction		= FALSE;
BOOL	CMS896AStn::m_bPrescanEmptyToNullBin	= FALSE;
UCHAR	CMS896AStn::m_ucPrescanEmptyGrade		= 93;
BOOL	CMS896AStn::m_bPrescanDefectAction		= FALSE;
BOOL	CMS896AStn::m_bPrescanBadCutAction		= FALSE;
BOOL	CMS896AStn::m_bPrescanBadCutToNullBin	= FALSE;
UCHAR	CMS896AStn::m_ucPrescanBadCutGrade		= 94;
BOOL	CMS896AStn::m_bPrescanDefectToNullBin	= FALSE;
UCHAR	CMS896AStn::m_ucPrescanDefectGrade		= 92;
BOOL	CMS896AStn::m_bNgPickExtra				= FALSE;
BOOL	CMS896AStn::m_bScanExtraDieAction		= FALSE;
UCHAR	CMS896AStn::m_ucScanExtraGrade			= 95;
BOOL	CMS896AStn::m_bScanDetectFakeEmpty		= FALSE;
UCHAR	CMS896AStn::m_ucScanFakeEmptyGrade		= 95;
LONG	CMS896AStn::m_lPrescanFinishedAction	= 0;
// Electronics NG grade
UCHAR	CMS896AStn::m_ucScanMapStartNgGrade		= 100;
UCHAR	CMS896AStn::m_ucScanMapEndNgGrade		= 100;
UCHAR	CMS896AStn::m_ucScanMapNgList[];
UCHAR	CMS896AStn::m_ucScanMapNgListSize		= 0;
BOOL	CMS896AStn::m_bPrescanSkipNgGrade		= FALSE;
//	AOI PR NG grade
UCHAR	CMS896AStn::m_ucScanMapStartAOINgGrade	= 100;
UCHAR	CMS896AStn::m_ucScanMapEndAOINgGrade	= 100;
UCHAR	CMS896AStn::m_ucScanMapAOINgList[];
UCHAR	CMS896AStn::m_ucScanMapAOINgListSize	= 0;
BOOL	CMS896AStn::m_bPrescanSkipAOINgGrade	= FALSE;


UCHAR	CMS896AStn::m_ucScanWalkTour;
BOOL	CMS896AStn::m_bPrescanGoodAction;
UCHAR	CMS896AStn::m_ucPrescanGoodGrade		= 0;
BOOL	CMS896AStn::m_bToUploadScanMapFile		= FALSE;
BOOL	CMS896AStn::m_bScnCheckByPR;
BOOL	CMS896AStn::m_bScnCheckIsRefDie;
BOOL	CMS896AStn::m_bErrorChooseGoFPC;
BOOL	CMS896AStn::m_bKeepSCNAlignDie;
LONG	CMS896AStn::m_lTotalSCNCount;
LONG	CMS896AStn::m_lScnCheckRow[WT_ALN_MAXCHECK_SCN];
LONG	CMS896AStn::m_lScnCheckCol[WT_ALN_MAXCHECK_SCN];

LONG	CMS896AStn::m_lScnCheckRefDieNo;
CString	CMS896AStn::m_szSaveImagePath;
CString CMS896AStn::m_szSaveMapImagePath;
CString CMS896AStn::m_szScanNgImageLogPath;
DOUBLE	CMS896AStn::m_dPrescanGoodPassScore;
DOUBLE	CMS896AStn::m_dPrescanFailPassScore;
DOUBLE	CMS896AStn::m_dCurrentScore;
CString	CMS896AStn::m_szCurrentGrade;
ULONG	CMS896AStn::m_ulMapBaseNum;

BOOL	CMS896AStn::m_bPrescanJustFinishToProbe	= FALSE;

// ES101: auto learn wafer
BOOL	CMS896AStn::m_bIsAutoLearnWaferNOTDone	= FALSE;


// prescan relative code	E
ULONG	CMS896AStn::m_ulMS90HalfBorderMapRow	= 0;
LONG	CMS896AStn::m_lMS90HalfDivideMapOrgRow	= 0;

ULONG	CMS896AStn::m_ulMS90HalfBorderMapCol	= 0;
LONG	CMS896AStn::m_lMS90HalfDivideMapOrgCol	= 0;

BOOL	CMS896AStn::m_bScanWithWaferLimit		= FALSE;
BOOL	CMS896AStn::m_bScanToHalfMap			= FALSE;

BOOL CMS896AStn::m_bMS90HalfSortMode	= FALSE;
BOOL CMS896AStn::m_bSortGoingTo2ndPart	= FALSE;
BOOL CMS896AStn::m_b2Parts1stPartDone	= FALSE;
BOOL CMS896AStn::m_b2Parts2ndPartStart	= FALSE;	//v4.59A17
BOOL CMS896AStn::m_b2PartsAllDone		= FALSE;
BOOL CMS896AStn::m_bSoraaSortMode		= FALSE;
BOOL CMS896AStn::m_bDualWayTipWafer		= FALSE;
BOOL CMS896AStn::m_bOsramResortMode		= FALSE;
BOOL CMS896AStn::m_bMS90RotatedBT		= FALSE;
LONG CMS896AStn::m_lBinCalibX			= 0;
LONG CMS896AStn::m_lBinCalibY			= 0;

BOOL CMS896AStn::m_bOsramDenyHMI		= TRUE;
BOOL CMS896AStn::m_bEnableOSRAM			= FALSE;
BOOL CMS896AStn::m_bEnableSyncMove		= FALSE;
LONG CMS896AStn::m_lSyncDistance		= 0;
DOUBLE CMS896AStn::m_dSyncSpeed			= 0;
UCHAR CMS896AStn::m_ucMapRotation		= 0;		//0 = 0; 1 = 90; 2 = 180; 3 = 270

DOUBLE		CMS896AStn::m_dWL1CalibXX;
DOUBLE		CMS896AStn::m_dWL1CalibXY;
DOUBLE		CMS896AStn::m_dWL1CalibYY;
DOUBLE		CMS896AStn::m_dWL1CalibYX;
LONG		CMS896AStn::m_lWL1WaferOffsetX;
LONG		CMS896AStn::m_lWL1WaferOffsetY;
DOUBLE		CMS896AStn::m_dWL2CalibXX;
DOUBLE		CMS896AStn::m_dWL2CalibXY;
DOUBLE		CMS896AStn::m_dWL2CalibYY;
DOUBLE		CMS896AStn::m_dWL2CalibYX;
LONG		CMS896AStn::m_lWL2WaferOffsetX;
LONG		CMS896AStn::m_lWL2WaferOffsetY;
BOOL		CMS896AStn::m_bWL1Calibrated;
BOOL		CMS896AStn::m_bWL2Calibrated;

// Temperature Controller
BOOL CMS896AStn::m_bColorCamera;
BOOL CMS896AStn::m_bTCLayerPick;
BOOL CMS896AStn::m_bTCInUse	= FALSE;
BOOL CMS896AStn::m_bTEInUse	= FALSE;
LONG CMS896AStn::m_lTCUpPreheatTime;
LONG CMS896AStn::m_lTCDnOnVacDelay;
LONG CMS896AStn::m_lTCUpHeatingTime;

LONG CMS896AStn::m_lLevel;

BOOL CMS896AStn::m_bFreezeCamera_HMI = FALSE;

// Static member variables for Step Move
BOOL CMS896AStn::m_bStepMoveEnable	= FALSE;		// Flag to enable step move feature
LONG CMS896AStn::m_lOrderCode		= 0;			// Code to indicate the sequence
BOOL CMS896AStn::m_bHeapCheck		= FALSE;		// Flag to control whether heap check is performed
//BOOL CMS896AStn::m_bBurnInMode	= FALSE;		// Flag to indicate whether it is burn-in mode
BOOL CMS896AStn::m_bBhInBondState	= FALSE;
BOOL CMS896AStn::m_bPostBondAtBond	= FALSE;
BOOL CMS896AStn::m_bPreBondAtPick	= FALSE;
BOOL CMS896AStn::m_bRealignFrameDone = FALSE;
BOOL CMS896AStn::m_bRealignFrame2Done = FALSE;
BOOL CMS896AStn::m_bIsWaferEnded	= FALSE;		//v4.20		//reposition from WaferTable class to here
BOOL CMS896AStn::m_bIsWaferEndedFor3EDL	= FALSE;	//v4.45T3	//3E DL

BOOL CMS896AStn::m_bOfflinePrintLabel	= FALSE;	//v2.66
BOOL CMS896AStn::m_bStPrintLabelInBkgd	= FALSE;	//v2.64
BOOL CMS896AStn::m_bStPrintLabelInABMode = FALSE;	//v2.64
BOOL CMS896AStn::m_bStStartPrintLabel	= FALSE;	//v2.64
BOOL CMS896AStn::m_bIsAlwaysPrintLabel	= FALSE;
BOOL CMS896AStn::m_bEnableMultipleMapHeaderPage = FALSE;

BOOL CMS896AStn::m_bDisableOKSelectionInAlertMsg = FALSE;
BOOL CMS896AStn::m_bDisableDefaultSearchWnd = FALSE;

BOOL CMS896AStn::m_bEnableAutoLoadNamingFile = FALSE;
BOOL CMS896AStn::m_bEnableLoadRankIDFromMap = FALSE;
CString CMS896AStn::m_szRankIDFileNameInMap = FALSE;
CString CMS896AStn::m_szRankIDFileExt;
BOOL CMS896AStn::m_bEnableBinBlkTemplate = FALSE;
BOOL CMS896AStn::m_bEnableOptimizeBinCountFunc = FALSE;
BOOL CMS896AStn::m_bOptimizeBinCountPerWftFunc = FALSE;
BOOL CMS896AStn::m_bEnableRestoreBinRunTimeData = FALSE;

BOOL CMS896AStn::m_bEnableBinLoderCoverSensorCheck = FALSE;
BOOL CMS896AStn::m_bEnableStartupCoverSensorCheck = FALSE;
BOOL CMS896AStn::m_bEnableWaferSizeSelect = FALSE;

BOOL CMS896AStn::m_bMS100OriginalBHSequence = FALSE;

CString CMS896AStn::m_szWaferId;
CString CMS896AStn::m_szMapFileName;
CString CMS896AStn::m_szMapFileExt1;
CString CMS896AStn::m_szOnlyMapFileFullPath;

CString CMS896AStn::m_szLoadReadBarcode;

// Critical Section
CCriticalSection CMS896AStn::m_csBackupNVRam;
CCriticalSection CMS896AStn::m_csBackupNVRamMotionErr;
CCriticalSection CMS896AStn::m_CSect;			
CCriticalSection CMS896AStn::m_csMachineStop;
CCriticalSection CMS896AStn::m_csStopToken;
//CCriticalSection CMS896AStn::m_csMachineAlarm;
CCriticalSection CMS896AStn::m_csMachineAlert;
CCriticalSection CMS896AStn::m_csLastTestLock;
CCriticalSection CMS896AStn::m_csBLBarcode;				//v4.22T1	//andrew
CCriticalSection CMS896AStn::m_csBLNichiaWafIDList;		//v4.40T6	

CMutex CMS896AStn::m_csDispMsg(FALSE, "Display Message", NULL);

BOOL CMS896AStn::m_bEnableWafflePadBonding = FALSE;

//Signal others station to stop
BOOL CMS896AStn::m_bSignalStopCycle		= FALSE;
BOOL CMS896AStn::m_bAllowHouseKeeping	= TRUE;
LONG CMS896AStn::m_lBondPrStopped		= 0;
LONG CMS896AStn::m_lBondHeadAtSafePos	= 0;
LONG CMS896AStn::m_lBinTableStopped		= 0;
BOOL CMS896AStn::m_bStopAllMotion		= 0;
BOOL CMS896AStn::m_bBackupNVRamMotionErr = FALSE;
BOOL CMS896AStn::m_bSearchHomeOption	= FALSE;
ULONG CMS896AStn::m_ulCornerSearchOption	= 0;
UCHAR CMS896AStn::m_ucPLLMWaferAlignOption	= 0;		//0=DEFAULT, 1=REBEL_TILE, 2=REBEL_U2U	
BOOL CMS896AStn::m_bRestoreNVRamInBondPage	= FALSE;	//v4.22T2

BOOL CMS896AStn::m_bMotionCycleStop		= FALSE;
BOOL CMS896AStn::m_bMoveDirection		= TRUE;
LONG CMS896AStn::m_lMotionTestDelay		= 0;
UCHAR CMS896AStn::m_ucMotionAxis		= 0;			//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
LONG CMS896AStn::m_lMotionOrigEncPosn	= 0;
LONG CMS896AStn::m_lMotionTarEncPosn	= 0;
LONG CMS896AStn::m_lMotionCycle			= 0;
LONG CMS896AStn::m_lMotionCycleCount	= 0;

DOUBLE	CMS896AStn::m_dMachineUPH		= 0.00;			//v4.31T9	//EquipMgr

//andrew
//BH Counters
ULONG	CMS896AStn::m_lPBIShiftCounter	= 0;			//only for ES ES Report
ULONG	CMS896AStn::m_lPBIAngleCounter	= 0;			//only for ES ES Report
ULONG	CMS896AStn::m_ulPickDieCount	= 0;			// Good Die
ULONG	CMS896AStn::m_ulBondDieCount	= 0;			// Bonded Die
ULONG	CMS896AStn::m_ulDefectDieCount	= 0;			// Defective Die
ULONG	CMS896AStn::m_ulBadCutDieCount	= 0;			// Bad-cut Die
ULONG	CMS896AStn::m_ulMissingDieCount	= 0;			// Missing Die
ULONG	CMS896AStn::m_ulRotateDieCount	= 0;			// Rotate Die
ULONG	CMS896AStn::m_ulEmptyDieCount	= 0;			// Empty Die
ULONG	CMS896AStn::m_ulWaferDefectDieCount		= 0;
ULONG	CMS896AStn::m_ulWaferMaxDefectCount		= 0;
ULONG	CMS896AStn::m_ulBH1UplookPrFailCount	= 0;	//v4.59A4	
ULONG	CMS896AStn::m_ulBH2UplookPrFailCount	= 0;	//v4.59A4	

ULONG	CMS896AStn::m_ulTotalSorted				= 0;

DOUBLE	CMS896AStn::m_dWaferGoodDieRatio	= 0;		// at least % of good die at current wafer
CStringArray CMS896AStn::m_szaSDX1;
CStringArray CMS896AStn::m_szaSDX2;

//andrew
//Air-flow sensor test
BOOL CMS896AStn::m_bEnableAirFlowTest	= FALSE;
BOOL CMS896AStn::m_bStartAirFlowMdCheck	= FALSE;
BOOL CMS896AStn::m_bStartAirFlowJcCheck	= FALSE;
LONG CMS896AStn::m_lAirFlowTestTimeOut	= 100;
LONG CMS896AStn::m_lAirFlowMdTime		= 0;
LONG CMS896AStn::m_lAirFlowJcTime		= 0;
LONG CMS896AStn::m_lAirFlowStartTime	= 0;

BOOL CMS896AStn::m_bStopGoToBlkDie		= FALSE;	//v3.30T1
BOOL CMS896AStn::m_bBlkLongJump			= FALSE;	//v3.25T16
//CStringArray CMS896AStn::m_StrBinBarCodeArray;	//v2.58
LONG CMS896AStn::m_lAutobondTimeInMin	= 0;		//v2.83T6
BOOL CMS896AStn::m_bExArmUnloadLastFrame = TRUE;	//v2.93T2
BOOL CMS896AStn::m_bExArmPreloadFrame	= FALSE;	//v4.31T6	//Lextar
BOOL CMS896AStn::m_bIsBLIdleinABMode	= FALSE;	//v4.42T7	//Testar MS109
BOOL CMS896AStn::m_bBhAutoCleanCollet	= FALSE;	//v4.31T6	//Lextar
BOOL CMS896AStn::m_bDLAChangeGrade		= 0;		//v3.10T3
BOOL CMS896AStn::m_bIsWaferPrRPY1Done	= FALSE;	//v3.31
BOOL CMS896AStn::m_bCleanColletPocketOn = FALSE;	//v3.68T4	//Lexter auto-Clean-Collet fcn
BOOL CMS896AStn::m_bWaferAlignComplete	= TRUE;
BOOL CMS896AStn::m_bPrAoiSwitchCamera	= FALSE;	//v3.93
BOOL CMS896AStn::m_bWLFrameToBeUnloadOnBuffer	= FALSE;
BOOL CMS896AStn::m_bWLFrameNeedPreloadToBuffer	= FALSE;
BOOL CMS896AStn::m_bWLReadyToUnloadBufferFrame	= FALSE;
BOOL CMS896AStn::m_bEnableAutoMapDieTypeCheckFunc;
CString CMS896AStn::m_szMapDieTypeCheckString[MS896A_DIE_TYPE_CHECK_LIMIT];
BOOL CMS896AStn::m_bAutoGenerateWaferEndFile = FALSE;

// Prober
LONG	CMS896AStn::m_lPSCMode	= 0;
CString	CMS896AStn::m_szOutputMapFullPath;
CString	CMS896AStn::m_szSourceMapFullPath;
DOUBLE	CMS896AStn::m_dPrDieCenterX;
DOUBLE	CMS896AStn::m_dPrDieCenterY;

//v4.39T7	//Nichia MS100+
CSfmSerialPort      CMS896AStn::m_oCommPort;
CSfmSerialPort      CMS896AStn::m_oCommPort2;
CMSNichiaMgntSubSystem	CMS896AStn::m_oNichiaSubSystem;			//v4.40T1
CMSSemitekMgntSubSystem  CMS896AStn::m_oSemitekSubSystem;
ULONG CMS896AStn::m_ulCommInterval	= 0;						//v4.40T9
UCHAR CMS896AStn::m_ucCommRetry		= 0;						//v4.40T9
BL_MAGAZINE			CMS896AStn::m_stMgznRT[MS_BL_MGZN_NUM];		// BL magazine run time settings

BOOL CMS896AStn::m_bEnableUseProbeTableLevelByRegion;
PROBE_TABLE_LEARN_POINT CMS896AStn::m_stLearnLevelCornerPoints[MS_LEARN_LEVEL_CORNER_POINT];
PROBE_TABLE_LEVEL_DATA	CMS896AStn::m_stProbeTableLevelData[MS_PROBETABLE_LEVEL_DATA_NO];
LONG CMS896AStn::m_lProbeTableDataSamplesX;
LONG CMS896AStn::m_lProbeTableDataSamplesY;
LONG CMS896AStn::m_lProbeTableDataSamplesPitchX;
LONG CMS896AStn::m_lProbeTableDataSamplesPitchY;
CString CMS896AStn::m_szBtTBarCode	= _T("");
BOOL CMS896AStn::m_bMS100EjtXY			= FALSE;	//v4.42T3
BOOL CMS896AStn::m_bEnableMS100EjtXY	= FALSE;	//v4.42T3
BOOL CMS896AStn::m_bSemitekBLMode		= FALSE;	//v4.42T9
UINT CMS896AStn::m_nWTAtColletPos		= 0;		//v4.42T15
UINT CMS896AStn::m_nWTLastColletPos		= 0;		//v4.42T15
//BOOL CMS896AStn::m_bBTIsMoveCollet		= TRUE;
BOOL CMS896AStn::m_bChangeCollet1		= FALSE;
BOOL CMS896AStn::m_bChangeCollet2		= FALSE;

//NVC Ejector
BOOL CMS896AStn::m_bEnableEjectorTheta	= FALSE;
BOOL CMS896AStn::m_bEnableBinEjector	= FALSE;

BOOL CMS896AStn::m_bEnableWTTheta		= FALSE;

BOOL CMS896AStn::m_bEnableClearBinCopyTempFile	= FALSE;

CString CMS896AStn::m_szWT1YieldState	= "";	//ES101(contour): saving the current wafer status for seperate "X" out 
CString CMS896AStn::m_szWT2YieldState	= "";	//ES101(contour): saving the current wafer status for seperate "X" out 
BOOL	CMS896AStn::m_bUseContour		= FALSE;
LONG	CMS896AStn::m_lWL1CenterX		= 0;
LONG	CMS896AStn::m_lWL1CenterY		= 0;
LONG	CMS896AStn::m_lWL2CenterX		= 0;
LONG	CMS896AStn::m_lWL2CenterY		= 0;
BOOL	CMS896AStn::m_bSubBinFull		= FALSE;
LONG	CMS896AStn::m_lSubBinGrade		= 1;
CString CMS896AStn::m_szAlarmLampStatusReportPath = "";

CAgmpCtrlPIDMotorThermalCtrl* CMS896AStn::m_pThermalCtrl = NULL;
CPrGeneral *CMS896AStn::m_pPrGeneral	= NULL;

DATA_LOG_INFO CMS896AStn::m_stDataLog;


IMPLEMENT_DYNCREATE(CMS896AStn, CAppStation)

CMS896AStn::CMS896AStn()
{
	m_qState				= IDLE_Q;
	m_qSubOperation			= 0;
	m_qPreviousSubOperation	= 0;
	m_bDebugPrint			= FALSE;
	m_bHardwareReady		= FALSE;

	m_lStepMove				= -1;
	//v4.65 from MS90 sw crash issue at AMC
	m_psmfMachine		= NULL;
	m_psmfDevice		= NULL;
	m_psmfSRam			= NULL;
	m_pvNVRAM			= NULL;
	m_pvNVRAM_HW		= NULL;

	// secs gem relative default in constructor
	m_bIsSecsGemInit		= FALSE;
	m_bIsSecsCommEnabled	= FALSE;
	m_lEquipmentStatus		= glSG_SCHEDULE_DOWN;
	m_lEquipState			= glSG_NONSCHEDULED;
	m_lPreEquipState		= glSG_NONSCHEDULED;
	m_bUseSecsGem			= FALSE;
	m_bUseTcpIp				= FALSE;
	m_pGemStation			= NULL;
	m_bEquipmentManager		= FALSE;	//v4.59A32

	//m_bStopByCommand		= FALSE;
	m_bUseEmptyCheck		= FALSE;	//Block2
	m_bUseAutoManualAlign	= FALSE;	//Block2

	m_bBackupNVRamMotionErr = FALSE;

	m_bIsSIOLinkFailed = FALSE;
}

CMS896AStn::~CMS896AStn()
{
}

BOOL CMS896AStn::InitInstance()
{
	try
	{
		Create();

		CSingleLock aLock(m_pModule->StartEvent());
		aLock.Lock();

		if (m_fHardware)
		{
#ifndef NU_MOTION
			Initialize();
#endif
		}

		//Get Device & Machine File pointer from the Module
		CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
		m_psmfMachine		= &(pAppMod->m_smfMachine);
		m_psmfDevice		= &(pAppMod->m_smfDevice);
		m_psmfSRam			= &(pAppMod->m_smfSRam);
		m_pvNVRAM			= pAppMod->m_pvNVRAM;
		m_pvNVRAM_HW		= pAppMod->m_pvNVRAM_HW;	//v4.65A3

		//Register Variables
		RegisterVariables();

		m_WaferMapEvent.SetWaferMapWrapper(&m_WaferMapWrapper);

		m_BinMapEvent.SetMapWrapperBin(&m_BinMapWrapper);

		//Application features
		SetFeatureStatus();
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	SetThreadPriority(THREAD_PRIORITY_HIGHEST);

	return TRUE;
}

INT CMS896AStn::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAppStation::ExitInstance();
}

BOOL CMS896AStn::InitData()			// Init the data from SMF and NVRAM
{
	return TRUE;
}

VOID CMS896AStn::ClearData()		// Clear Data for cold-start
{
	return;
}

VOID CMS896AStn::InitOutputFile()
{
	CFactoryMap::GetInstance()->InstallLibrary("C:\\MapSorter\\Exe\\BinOutputFiles");
	CWaferEndFactoryMap::GetInstance()->InstallLibrary("C:\\MapSorter\\Exe\\WaferEndFiles");

	m_objaGenerateDatabase.SetSize(MS_MAX_BIN + 1);		//v3.83
	m_objaQueryDatabase.SetSize(MS_MAX_BIN + 1);	//v3.83

	int i;

	for (i = 1; i <= MS_MAX_BIN; i++) //hardcode for now
	{
		m_objaGenerateDatabase.SetAt(i, new CGenerateDatabase);
		m_objaQueryDatabase.SetAt(i, new CQueryDatabase);
	}
}

BOOL CMS896AStn::InitNichiaSubSystem()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->GetCustomerName() == CTM_SEMITEK)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("Initialize Semitek sub-system");
		m_oSemitekSubSystem.SetEnabled(TRUE);
	}

	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return TRUE;
	}

	if (pApp->GetProductLine() != "")		//v4.59A34
	{
		return TRUE;
	}

	BOOL bStatus = TRUE;
	m_oNichiaSubSystem.SetEnabled(TRUE);
	CMSLogFileUtility::Instance()->MS_LogOperation("Initialize Nichia sub-system");
	m_oNichiaSubSystem.LogOpState(NC_OPCODE_IDLE, NC_OPSTATE_YELLOW);		//IDLE
	bStatus = m_oNichiaSubSystem.LoadData();

	return bStatus;
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CMS896AStn::IdleOperation()
{
	switch (Command())
	{
		case glNULL_COMMAND:
			Sleep(20);
			break;

		case glDIAGNOSTIC_COMMAND:
			State(DIAGNOSTICS_Q);
			Command(glNULL_COMMAND);
			break;

		case glINITIAL_COMMAND:
			State(SYSTEM_INITIAL_Q);
			Command(glNULL_COMMAND);
			break;

		case glPRESTART_COMMAND:
			State(PRESTART_Q);
			Command(glNULL_COMMAND);
			break;

		case glAUTO_COMMAND:
			State(AUTO_Q);
			Command(glNULL_COMMAND);
			break;

		case glMANUAL_COMMAND:
			State(MANUAL_Q);
			Command(glNULL_COMMAND);
			break;

		case glDEMO_COMMAND:
			State(DEMO_Q);
			Command(glNULL_COMMAND);
			break;

		case glDEINITIAL_COMMAND:
			State(DE_INITIAL_Q);
			Command(glNULL_COMMAND);
			break;

		case glSTOP_COMMAND:
		case glRESET_COMMAND:
			State(IDLE_Q);
			Motion(FALSE);
			Command(glNULL_COMMAND);
			break;

		default:
			Command(glNULL_COMMAND);
			State(IDLE_Q);
			Result(gnNOTOK);
			SetAlert(IDS_SYS_INVALID_COMD);
			break;
	}
}

VOID CMS896AStn::DiagOperation()
{
	INT		m_nLastError	= gnOK;
	LONG	lAxis, lAction, lUnit;
	
	AxisAction(lAxis, lAction, lUnit); 
	
	if (m_fHardware)
	{
		try
		{
			CString	szAxisID	= GetAxisID(lAxis);

			switch (lAction)
			{
				case glPOSITION_ACTION:
					if (lUnit == HOME_POS)
					{
						m_nLastError = MoveHome(szAxisID); //lUnit = 0;
					}
					else
					{
						m_nLastError = MoveActuator(szAxisID, GetPositionID(szAxisID, NORMAL_POS, lUnit)); //lUnit = 1,2,3, etc
					}
					break;
				case glRELATIVE_ACTION:
					m_nLastError	= MoveRelative(szAxisID, lUnit, SFM_WAIT, 0);
					break;
				case glABSOLUTE_ACTION:
					m_nLastError	= MoveAbsolute(szAxisID, lUnit, SFM_WAIT, 0);
					break;
				case  glFAST_HOME_ACTION:
					m_nLastError	= MoveActuator(szAxisID, GetPositionID(szAxisID, FAST_HOME_POS, lUnit));  //lUnit = 1,2,3,etc
					break;
				case glSEARCH_ACTION:
					break;
				case glINDEX_ACTION:
					break;
				case glPOWER_ON_ACTION:
					PowerOnAxis(szAxisID);
					break;
				case glPOWER_OFF_ACTION:
					PowerOffAxis(szAxisID);
					break;			
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
		}
	}
	
	Motion(FALSE);
	State(IDLE_Q);
}

VOID CMS896AStn::InitOperation()
{
	//CString szMsg = _T("   ") + GetName() + " --- InitOperation Completed";
	//DisplayMessage(szMsg);
	
	Motion(FALSE);
	State(IDLE_Q);
}

VOID CMS896AStn::PreStartOperation()
{
	//CString szMsg = _T("   ") + GetName() + " --- PreStartOperation Completed";
	//DisplayMessage(szMsg);
	m_bSignalStopCycle = FALSE;
	m_bAllowHouseKeeping = TRUE;
	m_bBackupNVRamMotionErr = FALSE;

//	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
//	m_bBurnInMode = pAppMod->m_bBurnIn;
//	CMSLogFileUtility::Instance()->SetEnableBurnIn(m_bBurnInMode);	//v3.87
	
	ResetCode();

	Motion(FALSE);
	State(IDLE_Q);
}

VOID CMS896AStn::AutoOperation()
{
	if (Command() == glSTOP_COMMAND)
	{
		State(STOPPING_Q);
	}
	else
	{
		Sleep(100);
	}
}

VOID CMS896AStn::DemoOperation()
{
	if (Command() == glSTOP_COMMAND)
	{
		State(STOPPING_Q);
	}
	else
	{
		Sleep(100);
	}
}

VOID CMS896AStn::CycleOperation()
{
	if (Command() == glSTOP_COMMAND)
	{
		State(STOPPING_Q);
	}
	else
	{
		Sleep(100);
	}
}

VOID CMS896AStn::ManualOperation()
{
	if (Command() == glSTOP_COMMAND)
	{
		State(STOPPING_Q);
	}
	else
	{
		Sleep(100);
	}
}

VOID CMS896AStn::ErrorOperation()
{
	if (Command() == glSTOP_COMMAND)
	{
		State(STOPPING_Q);
	}
	else if (Command() == glRESET_COMMAND) 
	{
		Command(glNULL_COMMAND);
		RestoreState();
	}
}

VOID CMS896AStn::StopOperation()
{
//	Sleep(100);
	//CString szMsg = _T("   ") +  GetName() + "--- Operation Stop";
	//DisplayMessage(szMsg);

//	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
//	m_bBurnInMode = pAppMod->m_bBurnIn;
//	CMSLogFileUtility::Instance()->SetEnableBurnIn(m_bBurnInMode);	//v3.87

	State(IDLE_Q);
	Command(glNULL_COMMAND);
	Motion(FALSE);
}

VOID CMS896AStn::DeInitialOperation()
{
	SFM_CStation *pStation;
	CAppStation  *pAppStn;
	CString		  szKey;

	try
	{
		//De-Initialize
		DeleteContents();
		//DisplayMessage(_T("    Reset station --- ") + GetName());
			
		if (m_fHardware == TRUE)
		{
#ifndef NU_MOTION
			//Re-Initialize
			Initialize();
			//DisplayMessage(_T("    Re-Initialized --- ") + GetName());
#endif
		}

		//DeInitialise All Sub Stn
		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			m_pStationMap.GetNextAssoc(pos, szKey, pStation);

			if ((pAppStn = dynamic_cast<CAppStation*>(pStation)) != NULL)
			{
				pAppStn->Command(glDEINITIAL_COMMAND);
				pAppStn->ReInitialized(FALSE);
				pAppStn->Result(gnOK);
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	//Set Reinitialize flag to true if all sub stn have been ReInitialized
	INT	nCount = 0;
	while (nCount != m_pStationMap.GetCount())
	{
		nCount = 0;
		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			m_pStationMap.GetNextAssoc(pos, szKey, pStation);
			pAppStn = dynamic_cast<CAppStation*>(pStation);

			if (!pAppStn->IsReInitialized())
			{
				break;
			}

			nCount++;
		}
	}

	ReInitialized(TRUE);
	Motion(FALSE);
	State(IDLE_Q);
}


/////////////////////////////////////////////////////////////////
//Station Signalling
/////////////////////////////////////////////////////////////////
BOOL CMS896AStn::Signal(INT nSignal)
{
	BOOL				bResult = FALSE;

	if (m_szModuleName != _T(""))
	{
/*
		try
		{
			IPC_CServiceMessage svMsg;
			CString szService = _T("SrvSig");
			szService += m_szStationName;

			svMsg.InitMessage(sizeof(INT), &nSignal);
			m_comClient.SendRequest(m_szModuleName, szService, svMsg, 0);
			bResult = TRUE;
		}
		catch (CAsmException e)
		{
			DisplayException(e);
		}
*/
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////
//Alarm
/////////////////////////////////////////////////////////////////
BOOL CMS896AStn::SetAlert(UINT unCode)
{
	CString		szCode;
	INT			bResult;
	LONG		lResponse;
	CTime BeginTime = BeginTime.GetCurrentTime();
	if ((unCode == gnOK) || (unCode == IDS_SYS_ABORT))
	{
		return TRUE;
	}

	CSingleLock slLock(&m_csMachineAlert);
	slLock.Lock();

	if ((bResult = szCode.LoadString(unCode)) != FALSE)
	{
		try
		{
//			SendAlarm(szCode, ALM_DONT_LOG, 3000, gszALM_CLASS_ALERT);
			EquipStateProductiveToUnscheduleDown();
			SendLexter_S5F65(80, unCode, FALSE);						//v4.13T1

			CMS896AStn::m_oNichiaSubSystem.LogOpState(unCode, 2);		//v4.40T3
			CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(unCode);	//anichia004
			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);		//anichia001

			if (IsEquipmentUnscheduleDown() || (State() != IDLE_Q))
			{
				if (CIgnoreAlarmCodeTable::Instance()->IsInIgnoreAlarmCodeTable(szCode))
				{
					SetAlertTime(TRUE, EQUIP_SETUP_TIME, szCode); //SCHEDULE DOWN TIME
				}
				else
				{
					SetAlarm(unCode, TRUE);			//v4.59A22	//Osram, modified from FALSE to TRUE
					SetAlertTime(TRUE, EQUIP_ALARM_TIME, szCode);
				}
			}

			SendAlarmResponse(szCode, &lResponse, ALM_LOG_AS_SET, 86400000, gszALM_CLASS_ALERT);

			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE);
			CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szCode);				

			if (IsEquipmentUnscheduleDown() || (State() != IDLE_Q))
			{
				(*m_psmfSRam)["MS896A"]["AlarmCode"] = szCode; // v4.51D2 //Electech3E(DL) 3
				if (CIgnoreAlarmCodeTable::Instance()->IsInIgnoreAlarmCodeTable(szCode))
				{
					SetAlertTime(FALSE, EQUIP_SETUP_TIME, szCode); //SCHEDULE DOWN TIME
				}
				else
				{
					SetAlertTime(FALSE, EQUIP_ALARM_TIME, szCode);
					(*m_psmfSRam)["MS896A"]["AlarmCode"] = szCode; // v4.51D2 //Electech3E(DL) 3
					ClearAlarm(unCode, TRUE);		//v4.59A22	//Osram, modified from FALSE to TRUE
				}
			}
			EquipStateUnscheduleDwonToProductive();
			//SendLexter_S5F65(34, unCode, FALSE);					//v4.13T1

			if( WaitingSetAlertToSanAnEngineerAlarm(szCode) )	// v4.51D5 SanAn(XA) 2 in Set Alert
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("[SetAlert] After WaitingSetAlertToSanAnEngineerAlarm() with alarm code: " + szCode);
			}
		}
		catch (CAsmException e)
		{
			TRACE1(_T("SendError Fail - %s\n"), szCode);
		}
	}
	else
	{
		//szCode.Format(_T("%u: Alarm Code Not Found! From %s station"), unCode,m_szStationName);
		//DisplayMessage(szCode);
	}

	slLock.Unlock();
	//szCode = "Error: " + szCode;
	//DisplayMessage(szCode);

	return bResult;
}


BOOL CMS896AStn::WaitingSetAlertToSanAnEngineerAlarm(CString szCode)  // v4.51D5 SanAn(XA) 3
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->m_bAWET_Enabled ) // test name first
	{
		return pApp->AWET_CheckAlarmCode(szCode);
	}

	CStringArray szaWaitingSetTime;
	szaWaitingSetTime.RemoveAll();
	CStdioFile cfReadWaitingSetTimeAlarmListFile;
	CString szWaitingSetTimeAlarmListPath = gszUSER_DIRECTORY + "\\WaitingSetTimeAlarmList.txt" ;
	CString szRead;
	bool bFindWaitingSetTimeCodeResult = FALSE; 
	int i;
	
	if( pApp -> GetCustomerName() == CTM_SANAN && pApp ->GetProductLine() == "XA") // test name first
	{
		if(cfReadWaitingSetTimeAlarmListFile.Open(szWaitingSetTimeAlarmListPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == FALSE)
		{
			HmiMessage("*[WaitingSetAlertToSanAnEngineerAlarm]  No exist/ Cannot Open WaitingSetTimeAlarmList.txt!");
			SetErrorMessage("*[WaitingSet]  No exist/ Cannot Open WaitingSetTimeAlarmList.txt!");
			return FALSE;
		}
		else
		{
			cfReadWaitingSetTimeAlarmListFile.ReadString(szRead);
			if(szRead == "")
			{
				HmiMessage("*[WaitingSetAlertToSanAnEngineerAlarm] The WaitingSetTimeAlarmList.txt is Null, Please input the value. ");
				SetErrorMessage("*[WaitingSet] The WaitingSetTimeAlarmList.txt is Null, Please input the value. ");
				return FALSE;
			}
			else
			{
				do
				{
					szaWaitingSetTime.Add(szRead);  // load Alarm List to array
					//cfReadWaitingSetTimeAlarmListFile.SeekToEnd();	//v4.53A29
					cfReadWaitingSetTimeAlarmListFile.ReadString(szRead);
				} while(szRead != "");
			}

			cfReadWaitingSetTimeAlarmListFile.Close();
		}

		int nSizeWaitingSetTime = (int)szaWaitingSetTime.GetSize();
		CString szSizeWaitingSetTime;
		szSizeWaitingSetTime.Format("%d", nSizeWaitingSetTime );
		CMSLogFileUtility::Instance()->MS_LogOperation("[WaitingSet] Array Size WaitingSetTime: " + szSizeWaitingSetTime );


		for(i = 0; i < nSizeWaitingSetTime; i++)
		{
			if( szaWaitingSetTime[i] == szCode)  // Compare AlarmList with alarm msg
			{
				bFindWaitingSetTimeCodeResult = TRUE;
				break;
			}
		}

		
		if( bFindWaitingSetTimeCodeResult == TRUE )
		{

			CMSLogFileUtility::Instance()->MS_LogOperation("[WaitingSet] FindWaitingSetTimeCodeResult == True");

			pApp->m_bWaitingSetTimeContainer  = TRUE; // open Alarm container
			pApp->m_szWaitingSetTimeAlarmMsg  = szCode;

			CString szStartStopSetupTime = (*m_psmfSRam)["MS896A"]["StartStop Setup Time"]; 
			if(szStartStopSetupTime == "Start" )
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("[WaitingSet] szStartStopSetupTime == Start, with Closed Setup Time");
				pApp->m_eqMachine.SetTime (FALSE, EQUIP_SETUP_TIME, szCode);// Stop SetTime
				
			}

			pApp->m_eqMachine.SetTime (TRUE, EQUIP_WAITING_SETUP_TIME, szCode); // Start WaitingSetTime;
			CMSLogFileUtility::Instance()->MS_LogOperation("[WaitingSet] Opened EQUIP_WAITING_SETUP_TIME ");

		}
		else
		{
			SetErrorMessage("[WaitingSet] FindWaitingSetTimeCodeResult == FALSE");
		}
	}
	else
	{
		return FALSE;
	}
	
		
	return TRUE;
}


LONG CMS896AStn::SetAlert_Msg(UINT unCode, CString &szMsg, CString szBtnText1, CString szBtnText2, CString szBtnText3, LONG lAlign, LONG lTop, LONG lLeft)
{
	CString	szCode;
	LONG	lResponse = 0;
	LONG	lMsgType = glHMI_MBX_CLOSE;	
	LONG	lCustomBtn = 0;

	if ((unCode == gnOK) || (unCode == IDS_SYS_ABORT))
	{
		return 0;
	}

	CSingleLock slLock(&m_csMachineAlert);
	slLock.Lock();


	if (szCode.LoadString(unCode) != FALSE)
	{
		UCHAR ucType = EQUIP_ASSIST_TIME;
		try
		{
			lCustomBtn = (LONG)(!szBtnText1.IsEmpty()) + (LONG)(!szBtnText2.IsEmpty()) + (LONG)(!szBtnText3.IsEmpty());
			switch (lCustomBtn)
			{
				default:
					lMsgType = glHMI_MBX_CLOSE;	
					ucType = EQUIP_ALARM_TIME;
					break;

				case 1:
					lMsgType = glHMI_MBX_SINGLEBUTTON;	
					break;

				case 2:
					lMsgType = glHMI_MBX_DOUBLEBUTTON;	
					break;

				case 3:
					lMsgType = glHMI_MBX_TRIPLEBUTTON;	
					break;
			}

			EquipStateProductiveToUnscheduleDown();
			SendLexter_S5F65(80, unCode, FALSE);					//v4.13T1

			if (IsEquipmentUnscheduleDown() || (State() != IDLE_Q))
			{
				if (CIgnoreAlarmCodeTable::Instance()->IsInIgnoreAlarmCodeTable(szCode))
				{
					SetAlertTime(TRUE, EQUIP_SETUP_TIME, szCode, szMsg);  //SCHEDULE DOWN TIME
				}
				else
				{
					SetAlarm(unCode, TRUE);		//v4.59A22	//Osram, modified from FALSE to TRUE
					SetAlertTime(TRUE, ucType, szCode, szMsg);
				}
			}
			CMS896AStn::m_oNichiaSubSystem.LogOpState(unCode, 2);		//v4.40T3
			CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia004
			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);		//anichia001

			SendAlarm(szCode, ALM_LOG_AS_SET, 86400000, gszALM_CLASS_ALERT, 
					  glHMI_MSG_MODAL, &lResponse, szMsg, lMsgType, lAlign,
					  -1, -1, -1, -1, -1, -1, -1,
					  &szBtnText1, &szBtnText2, &szBtnText3, lTop, lLeft);

			//anichia001			
			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE);
			CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szCode);			
			if (IsEquipmentUnscheduleDown() || (State() != IDLE_Q))
			{
				(*m_psmfSRam)["MS896A"]["AlarmCode"] = szCode; // v4.51D2 //Electech3E(DL) 4
				if (CIgnoreAlarmCodeTable::Instance()->IsInIgnoreAlarmCodeTable(szCode))
				{
					SetAlertTime(FALSE, EQUIP_SETUP_TIME, szCode, szMsg);  //SCHEDULE DOWN TIME
				}
				else
				{
					SetAlertTime(FALSE, ucType, szCode, szMsg);
					(*m_psmfSRam)["MS896A"]["AlarmCode"] = szCode; // v4.51D2 //Electech3E(DL) 4
					ClearAlarm(unCode, TRUE);		//v4.59A22	//Osram, modified from FALSE to TRUE
				}
			}
			EquipStateUnscheduleDwonToProductive();
			//SendLexter_S5F65(34, unCode, FALSE);			//v4.13T1

			if( WaitingSetAlertToSanAnEngineerAlarm(szCode) )	// v4.51D5 SanAn(XA) 4 in Set Alert Msg
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("[SetAlert_Msg] After WaitingSetAlertToSanAnEngineerAlarm() with alarm code: " + szCode);
			}
		}
		catch (CAsmException e)
		{
			TRACE1(_T("SendError Fail - %s\n"), szCode);
		}
	}
	else
	{
		//szCode.Format(_T("%u: Alarm Code Not Found! From %s station"), unCode,m_szStationName);
		//DisplayMessage(szCode);
	}

	slLock.Unlock();

	return lResponse;
}


VOID CMS896AStn::SetAlert_WarningMsg(const UINT unCode, const CString szMsg)
{
	CSingleLock slLock(&m_csMachineAlert);
	slLock.Lock();

	SetAlarmLamp_YellowGreenNoBuzzer(FALSE, FALSE);


	//szCode = "Warning";
	//szCode = "SYS0001";
	CString szCode;
	if (szCode.LoadString(unCode) != FALSE)
	{
		try
		{
			SendAlarm(szCode, ALM_LOG_AS_SET, 1, szCode, 
					  glHMI_MSG_MODAL, NULL, szMsg, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER,
					  -1, -1, -1, -1, -1, -1, -1,
					  NULL, NULL, NULL, -1, -1);
		}
		catch (CAsmException e)
		{
			TRACE1(_T("SendError Fail - %s\n"), szCode);
		}
	}
	slLock.Unlock();
}


BOOL CMS896AStn::SetError(UINT unCode)
{
	CString		szCode;
	BOOL		bResult;

	if (unCode == gnOK)
	{
		return TRUE;
	}

	if ((bResult = szCode.LoadString(unCode)) != FALSE)
	{
		try
		{
			EquipStateProductiveToUnscheduleDown();
			SendLexter_S5F65(80, unCode, FALSE);					//v4.13T1
			CMS896AStn::m_oNichiaSubSystem.LogOpState(unCode, 2);	//v4.40T3
			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);	//anichia001

			if (IsEquipmentUnscheduleDown() || (State() != IDLE_Q)) 
			{
				if (!CIgnoreAlarmCodeTable::Instance()->IsInIgnoreAlarmCodeTable(szCode))
				{
					SetAlarm(unCode, TRUE);		//v4.59A22	//Osram, modified from FALSE to TRUE
				}
			}
			SendAlarm(szCode, ALM_FORCE_LOG, 3000, gszALM_CLASS_ERROR);

			//anichia001			
			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE);
			CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szCode);					
			if (IsEquipmentUnscheduleDown() || (State() != IDLE_Q))
			{
				(*m_psmfSRam)["MS896A"]["AlarmCode"] = szCode; // v4.51D2 //Electech3E(DL) 5
				if (!CIgnoreAlarmCodeTable::Instance()->IsInIgnoreAlarmCodeTable(szCode))
				{
					ClearAlarm(unCode, TRUE);	//v4.59A22	//Osram, modified from FALSE to TRUE
				}
			}
			EquipStateUnscheduleDwonToProductive();
			
			if( WaitingSetAlertToSanAnEngineerAlarm(szCode) )	// v4.51D5 SanAn(XA) 5	in Set Error
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("[SetError] After WaitingSetAlertToSanAnEngineerAlarm() with alarm code: " + szCode);
			}
			//SendLexter_S5F65(34, unCode, FALSE);					//v4.13T1
		}
		catch (CAsmException e)
		{
			TRACE1(_T("SendError Fail - %s\n"), szCode);
		}
	}
	else
	{
		//szCode.Format(_T("%u: Alarm Code Not Found! From %s station"), unCode,m_szStationName);
		//DisplayMessage(szCode);
	}

	//szCode = "Error: " + szCode;
	//DisplayMessage(szCode);

	return bResult;
}

LONG CMS896AStn::SetAlert_NoMsg(UINT unCode, CString &szMsg)
{
	if ((unCode == gnOK) || (unCode == IDS_SYS_ABORT))
	{
		return 0;
	}

	CSingleLock slLock(&m_csMachineAlert);
	slLock.Lock();

	CString	szCode;
	if (szCode.LoadString(unCode) != FALSE)
	{
		try
		{
			UCHAR ucType = EQUIP_ALARM_TIME;

			EquipStateProductiveToUnscheduleDown();
			SendLexter_S5F65(80, unCode, FALSE);
			//v4.13T1
			if (IsEquipmentUnscheduleDown() || (State() != IDLE_Q))
			{
				if (CIgnoreAlarmCodeTable::Instance()->IsInIgnoreAlarmCodeTable(szCode))
				{
					SetAlertTime(TRUE, EQUIP_SETUP_TIME, szCode, szMsg);  //SCHEDULE DOWN TIME
				}
				else
				{
					SetAlarm(unCode, TRUE);			//v4.59A22	//Osram, modified from FALSE to TRUE
					SetAlertTime(TRUE, ucType, szCode, szMsg);
				}
			}
			CMS896AStn::m_oNichiaSubSystem.LogOpState(unCode, 2);		//v4.40T3
			CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia004
			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);		//anichia001

			Sleep(100);
	//		SendAlarm(szCode, ALM_LOG_AS_SET, 86400000, gszALM_CLASS_ALERT, 
	//				  glHMI_MSG_MODAL, &lResponse, szMsg, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER,
	//				  -1, -1, -1, -1, -1, -1, -1,
	//				  &, &, &, -1, -1);

			CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE);
			CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szCode);
			if (IsEquipmentUnscheduleDown() || (State() != IDLE_Q))
			{
				if (CIgnoreAlarmCodeTable::Instance()->IsInIgnoreAlarmCodeTable(szCode))
				{
					SetAlertTime(FALSE, EQUIP_SETUP_TIME, szCode, szMsg);  //SCHEDULE DOWN TIME
				}
				else
				{
					SetAlertTime(FALSE, ucType, szCode, szMsg);
					ClearAlarm(unCode, TRUE);		//v4.59A22	//Osram, modified from FALSE to TRUE
				}
			}
			EquipStateUnscheduleDwonToProductive();
		}
		catch (CAsmException e)
		{
			TRACE1(_T("SendError Fail - %s\n"), szCode);
		}
	}

	slLock.Unlock();

	return 0;
}

VOID CMS896AStn::MotionErrorBackupNVRam()
{
	CSingleLock slLock(&m_csBackupNVRamMotionErr);
	slLock.Lock();
	
	if (m_bBackupNVRamMotionErr == FALSE)
	{
		BackupToNVRAM();
		m_bBackupNVRamMotionErr = TRUE;
	}

	slLock.Unlock();
}

/////////////////////////////////////////////////////////////////
//Stop Cycle
/////////////////////////////////////////////////////////////////
VOID CMS896AStn::StopCycle(CString szStation)
{
	CSingleLock slLock(&m_csMachineStop);
	slLock.Lock();

	//Log into file
	if (m_bSignalStopCycle == FALSE)
	{
		CString szMsg = szStation + " to stop AutoBond by Stop Cycle";
		LogCycleStopState(szMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		IPC_CServiceMessage svMsg;

		m_bSignalStopCycle = TRUE;

		try
		{
			m_comClient.SendRequest(_T("MapSorter"), gszSTOP_COMMAND, svMsg, 0);	//	SrvStopCommand	by one of station
		}
		catch (CAsmException e)
		{
			DisplayException(e);
		}
	}
	else
	{
		LogCycleStopState(szStation + " is visiting");
	}

	slLock.Unlock();
}

BOOL CMS896AStn::SignalInform(CString szStation, INT nSignal)
{
	LogCycleStopState(szStation + " Signal Complete");
	return Signal(nSignal);
}

/////////////////////////////////////////////////////////////////
//Update Profile
/////////////////////////////////////////////////////////////////


VOID CMS896AStn::UpdatePosUnit(const CString &szAxis, const CString &szPos)
{
	//Syntax: m_smfDevice[szStation][szAxis][gszMRK_POS][szPos]
	//for Contour position: m_smfDevice[szStation][szAxis][gszMRK_POS][szPos][nIndex]
	CMapElement	&rElement = (*m_psmfDevice)[m_szStationName][szAxis][gszMRK_POS][szPos];
	SFM_CPosition	*pPos	= GetPosition(szAxis, szPos);

	if (pPos->GetPositionType() == SFM_CONTOUR)
	{
		INT		nMaxUnit = pPos->GetNumOfUnit();
		SHORT	*pList	= (SHORT*) malloc(nMaxUnit * sizeof(SHORT));

		if (pList != NULL)		//Klocwork	//v4.02T5
		{
			for (INT i = 0; i < nMaxUnit; i++)
			{
				LONG lUnit = rElement[gszMRK_POS_UNIT][i];
				pList[i] = (SHORT) lUnit;
			}

			pPos->SetUnitList(nMaxUnit, pList);
			free(pList);
		}
	}
	else
	{
		pPos->SetUnit((LONG) rElement[gszMRK_POS_UNIT]);
	}
}

VOID CMS896AStn::RegisterVariables()
{ 
	/* Virtual */ 
}


VOID CMS896AStn::SetAutoCycleLog(CONST BOOL bEnable)
{
	CSingleLock slLock(&m_csDispMsg);
	slLock.Lock();

	if( CMSLogFileUtility::Instance()->GetEnableMachineLog() )
	{
		CMSLogFileUtility::Instance()->CloseAutoCycleLog(bEnable);
	}

	slLock.Unlock();
}

VOID CMS896AStn::DisplaySequence(const CString &szMessage)
{
	CSingleLock slLock(&m_csDispMsg);
	slLock.Lock();

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (CMSLogFileUtility::Instance()->GetEnableMachineLog() || pUtl->GetAutoCycleLog())
	{
		//if (szMessage.Find("WT - ") != -1	|| 
		//	szMessage.Find("WFT - ") != -1	||
		//	szMessage.Find("WTNG - ") != -1 ||
		//	szMessage.Find("CWT - ") != -1 )
		if (szMessage.Find("WT - ") != -1	|| 
			szMessage.Find("WFT - ") != -1)
		{
			if (m_szCycleMessageWT.CompareNoCase(szMessage) == 0)
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageWT = szMessage;
		}

		if (szMessage.Find("BT - ") != -1)
		{
			if (m_szCycleMessageBT.CompareNoCase(szMessage) == 0)
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageBT = szMessage;
		}

		if (szMessage.Find("BPR - ") != -1)
		{
			if (m_szCycleMessageBP.CompareNoCase(szMessage) == 0)
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageBP = szMessage;
		}

		if (szMessage.Find("WPR - ") != -1)
		{
			if (m_szCycleMessageWP.CompareNoCase(szMessage) == 0 )
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageWP = szMessage;
		}

		if (szMessage.Find("WL - ") != -1)
		{
			if (m_szCycleMessageWL.CompareNoCase(szMessage) == 0)
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageWL = szMessage;
		}

		if (szMessage.Find("BL - ") != -1)
		{
			if( m_szCycleMessageBL.CompareNoCase(szMessage) == 0 )
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageBL = szMessage;
		}

		//andrewng //2020-0708
		if (szMessage.Find("NL - ") != -1)
		{
			if (m_szCycleMessageNL.CompareNoCase(szMessage) == 0)
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageNL = szMessage;
		}

		if ( szMessage.Find("STS - ") != -1 )
		{
			if( m_szCycleMessageST.CompareNoCase(szMessage) == 0 )
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageST = szMessage;
		}

		//if (szMessage.Find("BH - ") != -1		|| 
		//	szMessage.Find("CPZ - ") != -1		||
		//	szMessage.Find("BHNG - ") != -1 )
		if (szMessage.Find("BH - ") != -1)
		{
			if ( m_szCycleMessageBH.CompareNoCase(szMessage) == 0 )
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageBH = szMessage;
		}

		if (szMessage.Find("SG - ") != -1)
		{
			if (m_szCycleMessageSG.CompareNoCase(szMessage) == 0)
			{
				slLock.Unlock();
				return;
			}
			m_szCycleMessageSG = szMessage;
		}

		if ( CMSLogFileUtility::Instance()->GetEnableMachineLog() )
		{
			CMSLogFileUtility::Instance()->WriteAutoCycleLog(szMessage);
		}
	}

	if (pUtl->GetAutoCycleLog())
		OutputDebugString(szMessage);
	if (m_bDebugPrint == TRUE) 
	{
		DisplayMessage(szMessage);
	}

	slLock.Unlock();
}

VOID CMS896AStn::SetComplete(BOOL bState)
{
	if (bState)
	{
		m_evComplete.SetEvent();
	}
	else
	{
		m_evComplete.ResetEvent();
	}
}

BOOL CMS896AStn::IsComplete(INT nTimeout)
{
	CSingleLock slLock(&m_evComplete);
	return slLock.Lock(nTimeout);
}

BOOL CMS896AStn::NeedReset()
{
	CSingleLock		slLock(&m_csResource);
	slLock.Lock();

	return m_bNeedReset;
}

VOID CMS896AStn::NeedReset(BOOL bSet)
{
	CSingleLock slLock(&m_csResource);
	slLock.Lock();

	m_bNeedReset		= bSet;

	slLock.Unlock();
}

VOID CMS896AStn::SetStatusMessage(const CString &szStatus)
{
	m_CSect.Lock();

	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	pAppMod->SetStatusMessage(szStatus);

	m_CSect.Unlock();
}

VOID CMS896AStn::SetLogItems(CString szStatus)
{
	m_CSect.Lock();
	
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	pAppMod->SetLogItems(szStatus);

	m_CSect.Unlock();
}

VOID CMS896AStn::SetErrorMessage(const CString &szStatus)
{
	m_CSect.Lock();

	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	pAppMod->SetErrorMessage(szStatus);

	m_CSect.Unlock();
}

VOID CMS896AStn::WftToSafeLog(const CString &szStatus, BOOL bLogNow)
{
	m_CSect.Lock();

	if ( CMSLogFileUtility::Instance()->GetEnableMachineLog() && 
		 (Is180Arm6InchWT() || bLogNow) )
	{
		CString szLogFile;

		CTime theTime = CTime::GetCurrentTime();
		szLogFile = "C:\\MapSorter\\UserData\\History\\6InchWft.txt";

		FILE *pPScanLog = NULL;
		errno_t nErr = fopen_s(&pPScanLog, szLogFile, "a+");
		if ((nErr == 0) && (pPScanLog != NULL))
		{
			fprintf(pPScanLog, "%2d (%2d:%2d:%2d) - %s\n", theTime.GetDay(),
					theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LPCTSTR) szStatus);  
			fclose(pPScanLog);
		}
	}

	m_CSect.Unlock();
}

VOID CMS896AStn::LogCycleStopState(CString szLogMessage)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	pAppMod->LogCycleStopStatus(szLogMessage);
	DisplaySequence(szLogMessage);
}

VOID CMS896AStn::SetAlarmLog(const CString &szStatus)
{
	if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
	{
		return ;
	}

//	m_CSect.Lock();

	FILE *fpRecord = NULL;
	errno_t nErr = fopen_s(&fpRecord, gszUSER_DIRECTORY + "\\History\\Alarm.log", "a+");
	if ((nErr == 0) && (fpRecord != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(fpRecord, "%2d (%2d:%2d:%2d) - %s\n", theTime.GetDay(),
				theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LPCTSTR) szStatus);	//Klocwork  
		fclose(fpRecord);
	}	
//	m_CSect.Unlock();
}

//1.08S
VOID CMS896AStn::AlarmLampStatusChangeLog(LONG lAlarmOrgStatus, LONG lAlarmNewStatus , CString szAlarmCode)
{
	CString szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
	CString szOperatorId = (*m_psmfSRam)["MS896A"]["Operator Id"];
	CString szPKGFilename =(*m_psmfSRam)["MS896A"]["PKG Filename"];
	CTime theTime = CTime::GetCurrentTime();
	CString szTimeName = theTime.Format("%Y%m%d%H%M%S");
	CString szDateTime = theTime.Format("%Y-%m-%d %H:%M:%S");
	CString szFileName = "";
	CString szLocalFileName = gszUSER_DIRECTORY + "\\History\\" + szMachineNo + "_" + szTimeName + ".txt";
	m_szAlarmLampStatusReportPath = (*m_psmfSRam)["MS896A"]["Machine"]["AlarmReport Path"];
	if (m_szAlarmLampStatusReportPath.IsEmpty())
	{
		szFileName = gszUSER_DIRECTORY + "\\History\\" + szMachineNo + "_" + szTimeName + ".txt";
	}
	else
	{
		szFileName = m_szAlarmLampStatusReportPath + "\\" + szMachineNo + "_" + szTimeName + ".txt";
	}
	CString szAlarmOrgStatus;
	CString szAlarmNewStatus;
	switch(lAlarmOrgStatus)
	{
		case ALARM_GREEN:
			{
				szAlarmOrgStatus = _T("G");
				break;
			}
		case ALARM_YELLOW:
		case ALARM_YELLOW_BUZZER:
			{
				szAlarmOrgStatus = _T("Y");
				break;
			}
		case ALARM_RED:
		case ALARM_RED_NO_BUZZER:
			{
				szAlarmOrgStatus = _T("R");
				break;
			}
		case ALARM_REDYELLOW_BUZZER:
		case ALARM_REDYELLOW_NO_BUZZER:
			{
				szAlarmOrgStatus = _T("RY");
				break;
			}
		case ALARM_YELLOWGREEN_BUZZER:
		case ALARM_YELLOWGREEN_NOMATERIAL:
			{
				szAlarmOrgStatus = _T("YG");
				break;
			}
		case ALARM_GREEN_YELLOW_RED_ON:
			{
				szAlarmOrgStatus = _T("GYR");
				break;
			}
		default:
			{
				szAlarmOrgStatus = _T("OFF");
				break;
			}
	}

	switch(lAlarmNewStatus)
	{
		case ALARM_GREEN:
			{
				szAlarmNewStatus = _T("G");
				break;
			}
		case ALARM_YELLOW:
		case ALARM_YELLOW_BUZZER:
			{
				szAlarmNewStatus = _T("Y");
				break;
			}
		case ALARM_RED:
		case ALARM_RED_NO_BUZZER:
			{
				szAlarmNewStatus = _T("R");
				break;
			}
		case ALARM_REDYELLOW_BUZZER:
		case ALARM_REDYELLOW_NO_BUZZER:
			{
				szAlarmNewStatus = _T("RY");
				break;
			}
		case ALARM_YELLOWGREEN_BUZZER:
		case ALARM_YELLOWGREEN_NOMATERIAL:
			{
				szAlarmNewStatus = _T("YG");
				break;
			}
		case ALARM_GREEN_YELLOW_RED_ON:
			{
				szAlarmNewStatus = _T("GYR");
				break;
			}
		default:
			{
				szAlarmNewStatus = _T("OFF");
				break;
			}
	}

	CStdioFile cfAlarmLampStatusChangeFile;
	CFileException e;
	if (! cfAlarmLampStatusChangeFile.Open(szLocalFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite, &e))
	{
		return;
	}
	else
	{
		try
		{
			cfAlarmLampStatusChangeFile.SeekToBegin();
			cfAlarmLampStatusChangeFile.WriteString(szMachineNo + "," + szAlarmOrgStatus + ","+ szAlarmNewStatus  + "," + szDateTime + "," + szAlarmCode + "," + szPKGFilename + "," + szOperatorId + "\n");
			cfAlarmLampStatusChangeFile.Close();
		}
		catch(CFileException* e)
		{
			if( e->m_cause == CFileException::badPath )
			{
			}
			e->Delete();
			return;
		}
	}

	TRY 
	{
		CopyFile(szLocalFileName, szFileName, FALSE);
	}
	CATCH (CFileException, e)
	{
		SetErrorMessage("Fail to upload AlarmLampStatusChangeLog to server path.");
		return;
	}
	END_CATCH

	TRY 
	{
		DeleteFile(szLocalFileName);
	}
	CATCH (CFileException, e)
	{
		SetErrorMessage("Fail to delete local AlarmLampStatusChangeLog");
	}
	END_CATCH
}

VOID CMS896AStn::SetCtmLogMessage(const CString &szStatus)
{
	m_CSect.Lock();

	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	pAppMod->SetCtmLogMessage(szStatus);
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	pUtl->RegionOrderLog(szStatus);
	m_CSect.Unlock();
}

VOID CMS896AStn::DisplayMessage(const CString &szMessage)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	pAppMod->DisplayMessage(szMessage);
}


VOID CMS896AStn::SetMeasurementUnit(const CString &szMeasurementUnit)
{
	m_CSect.Lock();

	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	pAppMod->SetMeasurementUnit(szMeasurementUnit);

	m_CSect.Unlock();
} //end SetMeasurementUnit


CString CMS896AStn::GetMeasurementUnit()
{
	CString szUnit;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	szUnit = pApp->GetMeasurementUnit();

	return szUnit;
} //end GetMeasurementUnit


LONG CMS896AStn::ConvertFileUnitToDisplayUnit(DOUBLE dValue)
{
	LONG lConvertedValue;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	lConvertedValue = pApp->ConvertFileUnitToDisplayUnit(dValue);

	return lConvertedValue;
} //end ConvertFileUnitToDisplayUnit

DOUBLE CMS896AStn::ConvertDFileUnitToDisplayUnit(DOUBLE dValue)		//v4.59A19
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	DOUBLE dConvertedValue = 0;	

	//convert value in file (in um) into the selected display unit (mil or um)
	if (pApp->GetMeasurementUnit() == "um")				//um -> um
		dConvertedValue = dValue;
	else if (pApp->GetMeasurementUnit() == "mil")		//um ->mil
	{
		if (dValue == 0)
			dConvertedValue = 0;
		else
			dConvertedValue = dValue / 25.4;
	}
	return dConvertedValue;
} //end ConvertFileUnitToDisplayUnit

LONG CMS896AStn::ConvertDisplayUnitToFileUnit(DOUBLE dValue)
{
	LONG lConvertedValue;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	lConvertedValue = pApp->ConvertDisplayUnitToFileUnit(dValue);

	return lConvertedValue;
} //end ConvertDisplayUnitToFileUnit

DOUBLE CMS896AStn::ConvertDisplayUnitToDFileUnit(DOUBLE dValue)
{
	DOUBLE dConvertedValue;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//lConvertedValue = pApp->ConvertDisplayUnitToFileUnit(lValue);

	if (pApp->GetMeasurementUnit() == "um")				//um -> um
		dConvertedValue = dValue;
	else if (pApp->GetMeasurementUnit() == "mil")		//mil -> um
	{
		if (dValue == 0)
			dConvertedValue = 0;
		else
			dConvertedValue = dValue * 25.4;
	}

	return dConvertedValue;
}


/////////////////////////////////////////////////////////////////
// Calculate Profile Time
/////////////////////////////////////////////////////////////////
//int Wh_Hp_CalProfileTime(Hp_Move_profile profile, int rel_dist)

/////////////////////////////////////////////////////////////////
// For Step Move
/////////////////////////////////////////////////////////////////
BOOL CMS896AStn::AllowMove()
{
	BOOL bReturn = FALSE;

	if (m_bStepMoveEnable == FALSE)		// Whether the step move feature is enable
	{
		bReturn = TRUE;
	}
	else if (m_lStepMove == -1)			// Whether the move is performed
	{
		m_lStepMove = GetCode();
		bReturn = FALSE;
	}
	else if (m_lStepMove == 0)			// Whether the step move flag is changed
	{
		BackCode();
		m_lStepMove = -1;
		bReturn = TRUE;
	}

	if (bReturn == FALSE)
	{
		Sleep(10);
	}

	return bReturn;
}

LONG CMS896AStn::GetCode()
{
	m_CSect.Lock();
	m_lOrderCode++;
	m_CSect.Unlock();
	return m_lOrderCode;
}

VOID CMS896AStn::BackCode()
{
	m_CSect.Lock();
	if (m_lOrderCode > 0) 
	{
		m_lOrderCode--;
	}
	m_CSect.Unlock();
}

VOID CMS896AStn::ResetCode()
{
	m_CSect.Lock();
	m_lOrderCode = 0;
	m_CSect.Unlock();
}




VOID CMS896AStn::CheckResult(const INT nResult, const CString &szMsg)
{
	//v2.60
	//if ( nResult != gnOK )
	//	throw CAsmException(gnEXCEPTION, szMsg, m_szStationName);
}

////////////////////////////////////////////////////////////////////////////////////
//Get Machine config file channel information 
////////////////////////////////////////////////////////////////////////////////////

LONG CMS896AStn::GetChannelInformation(const CString &szChannel, const CString &szData)
{
	CSingleLock slLock(&m_CSect);
	slLock.Lock();

	LONG lValue = -1; 
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	//load & get config file ptr
	pUtl->LoadMachineConfig(); 
	psmf = pUtl->GetMachineConfigFile();

	//Check Load/Save Data
	if (psmf != NULL)
		//Get target value
	{
		lValue	= (*psmf)[MS896A_CFG_CHANNEL_ATTRIB][szChannel][szData];
	}

	//close config file ptr
	pUtl->CloseMachineConfig();

	slLock.Unlock();
	return lValue;
}


//andrewng //2020-0626
LONG CMS896AStn::GetChannelInformation(CStringMapFile *psmf, const CString &szChannel, const CString &szData)
{
	LONG lValue = -1; 
	if (psmf == NULL)
	{
		return lValue;
	}

	lValue	= (*psmf)[MS896A_CFG_CHANNEL_ATTRIB][szChannel][szData];
	return lValue;
}


DOUBLE CMS896AStn::GetChannelResolution(const CString &szChannel)
{
	CSingleLock slLock(&m_CSect);
	slLock.Lock();

	DOUBLE dValue = 0;
	LONG lResolutionFactor = 0;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	//load & get config file ptr
	pUtl->LoadMachineConfig(); 
	psmf = pUtl->GetMachineConfigFile();

	if (psmf != NULL)
	{
		//Get target value
		dValue	= (*psmf)[MS896A_CFG_CHANNEL_ATTRIB][szChannel][MS896A_CFG_CH_RESOLUTION];
		lResolutionFactor = (LONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][szChannel][MS896A_CFG_CH_RESOLUTION_FACTOR];

		if (lResolutionFactor <= 0)
		{
			lResolutionFactor = 1;
		}
		
		dValue = (DOUBLE)dValue / (DOUBLE)lResolutionFactor;
	}

	//close config file ptr
	pUtl->CloseMachineConfig();

	slLock.Unlock();
	return dValue;
}

LONG CMS896AStn::GetDefaultMachineDelay(const CString &szChannel, const CString &szData)
{
	CSingleLock slLock(&m_CSect);
	slLock.Lock();

	LONG lValue = -1; 
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	//load & get config file ptr
	pUtl->LoadMachineConfig(); 
	psmf = pUtl->GetMachineConfigFile();

	//Check Load/Save Data
	if (psmf != NULL)
		//Get target value
	{
		lValue	= (*psmf)[MS896A_CFG_CHANNEL_ATTRIB][szChannel]["Others"][szData];
	}

	//close config file ptr
	pUtl->CloseMachineConfig();

	slLock.Unlock();
	return lValue;
}



////////////////////////////////////////////////////////////////////////////////////
//     Set Application Features information
////////////////////////////////////////////////////////////////////////////////////
VOID CMS896AStn::SetFeatureStatus()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	//v3.60T1	//andrew	//Individual module ENABLE/DISABLE
	m_bDisableBH				= pApp->GetFeatureStatus(gszDISABLE_BH_MODULE);
	m_bDisableBT				= pApp->GetFeatureStatus(gszDISABLE_BT_MODULE);
	m_bDisableBL				= pApp->GetFeatureStatus(gszDISABLE_BL_MODULE);
	m_bDisableNL				= pApp->GetFeatureStatus(gszDISABLE_NL_MODULE);				//andrewng //2020-0707
	m_bDisableWL				= pApp->GetFeatureStatus(gszDISABLE_WL_MODULE);
	m_bDisableWT				= pApp->GetFeatureStatus(gszDISABLE_WT_MODULE);
	m_bDisableWLWithExp			= pApp->GetFeatureStatus(gszDISABLE_WL_MODULE_WITH_EXP);

	//Get Features status
	m_bIfGenRefDieStatusFile	= pApp->GetFeatureStatus(MS896A_FUNC_REFDIESTATUSFILE);
	m_bIfGenLabel				= pApp->GetFeatureStatus(MS896A_FUNC_LABELPRINTOUT);
	m_bUseRefDieCheck			= pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_REFDIE);
	//m_bUsePostBond			= pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_POSTBOND);		//v4.35T1
	if (m_bDisableBT)
	{
		m_bUsePostBond = FALSE;
	}
	m_bBinFrameStatusSummary	= pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_BINFRAMESTATUSSUM);
	m_bPortablePKGInfoPage		= pApp->GetFeatureStatus(MS896A_FUNC_PORTABLE_PKG_INFO_PAGE);
	m_bSupportPortablePKGFile	= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_PORTABLE_PKG_FILE);
	m_bEnableSummaryPage		= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_SUMMARY_PAGE);
	m_bEnableEmptyBinFrameCheck	= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_EMPTY_BINFRAME_CHECK);
	m_bEnableResetMagzCheck		= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_RESET_MAG_CHECK);
	m_bEnableWaferLotFileProtection = pApp->GetFeatureStatus(MS896A_FUNC_WAFER_LOT_FILE_PROTECTION);
	m_bForceClearMapAfterWaferEnd	= pApp->GetFeatureStatus(MS896A_FUNC_WAFER_END_CLEAR_MAP);
	m_bWafflePadBondDie				= pApp->GetFeatureStatus(MS896A_FUNC_WAFFLE_PAD_BOND_DIE);
	m_bWafflePadIdentification	= pApp->GetFeatureStatus(MS896A_FUNC_WAFFLE_PAD_IDENTIFICATION);
	//m_bEnableMouseControl		= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_MOUSE_CONTROL);
	m_bWaferLotWithBinSummaryFormat = pApp->GetFeatureStatus(MS896A_FUNC_WAFERLOT_WITH_BIN_SUMMARY_FORMAT);
	m_bEnableWaferLotLoadedMapCheck = pApp->GetFeatureStatus(MS896A_FUNC_WAFERLOT_LOADED_MAP_CHECK);

	m_bUseSecsGem = pApp->GetFeatureStatus(MS896A_FUNC_HOST_COMM_SECSGEM);
	m_bUseTcpIp					= pApp->GetFeatureStatus(MS896A_FUNC_HOST_COMM_TCPIP);
	m_bIfEnableCharDie			= pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_CHAR_REFDIE);
	m_bEnablePolyonDie			= pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_POLYGON_DIE);
	m_bEnableBlockFunc			= pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_USE_BLOCK);
	m_bDisplaySortBinItemMsg	= pApp->GetFeatureStatus(MS896A_FUNC_DISPLAY_SORT_BIN_ITEM);
	m_bSearchCompleteFilename	= pApp->GetFeatureStatus(MS896A_FUNC_COMPLETE_FILENAME);
	m_bLoadMapSearchDeepInFolder = pApp->GetFeatureStatus(MS896A_FUNC_SEARCH_DEEP_IN_FOLDER);
	m_bNoPRRecordForPKGFile		= pApp->GetFeatureStatus(MS896A_FUNC_NO_PR_RECORD_PKG_FILE);
	m_bEnableToolsUsageRecord	= pApp->GetFeatureStatus(MS896A_FUNC_TOOLS_USAGE_RECORD);
	m_bEnableAssocFile			= pApp->GetFeatureStatus(MS896A_FUNC_ASSOC_FILE_FUNC);
	m_lSetDetectSkipMode		= pApp->GetFeatureValue(MS896A_FUNC_DETECT_SKIP_MODE);
	m_bAoiEnableOcrDie			= pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_AOI_OCR_DIE);
	m_bWaferPrUseMxNLFWnd		= pApp->GetFeatureStatus(MS896A_FUNC_MxN_LF_WND);		//v4.43T2
	m_bWaferPrMS60LFSequence	= pApp->GetFeatureStatus(MS896A_FUNC_MS60_LF_SEQ);		//v4.47T1
	//4.53D18
	m_bWaferMapUseDieMixing		= pApp->GetFeatureStatus(MS896A_FUNC_MAP_DIE_MIX);

	m_bEnableAlarmLampBlink		= pApp->GetFeatureStatus(MS896A_FUNC_ALARM_LAMP_BLINK);
	m_bEnableAmiFile			= pApp->GetFeatureStatus(MS896A_FUNC_AMI_FILE);
	m_bEnableBarcodeLengthCheck = pApp->GetFeatureStatus(MS896A_FUNC_BARCODE_LENGTH_CHECK);
	m_bEnableInputCountSetupFile = pApp->GetFeatureStatus(MS896A_FUNC_INPUT_COUNT_SETUP_FILE);
	m_bForceClearBinBeforeNewLot = pApp->GetFeatureStatus(MS896A_FUNC_CLEAR_BIN_BEFORE_NEW_LOT);
	m_bNewLotCreateDirectory = pApp->GetFeatureStatus(MS896A_FUNC_NEW_LOT_CREATE_DIR);
	
	m_szAssocFileExt			= pApp->GetFeatureStringValue(MS896A_FUNC_ASSOC_FILE_EXT);

	m_bRenameMapNameAfterEnd	= pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_RENAME_AFTER_END);
	m_bAlwaysLoadMapFileLocalHarddisk = pApp->GetFeatureStatus(MS896A_FUNC_ALWAYS_LOAD_MAP_LOCAL_HD);

	m_bEnableMachineReport		= pApp->GetFeatureStatus(MS896A_FUNC_MACHINE_REPORT);
	m_bEnableItemLog			= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_ITEM_LOG);
	m_bEnableAlignWaferImageLog	= pApp->GetFeatureStatus(MS896A_FUNC_ALIGN_WAFER_IMAGE_LOG);
	m_bTEInUse					= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_THERMAL_EJECTOR);
	m_bTCInUse					= FALSE;
	m_bColorCamera				= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_COLOR_CAMERA);
#ifdef VS_5MCAM
	m_bColorCamera				= TRUE;
#endif
	m_bEnableBinOutputFilePath2	= pApp->GetFeatureStatus(MS896A_FUNC_OUTPUT_FILE_PATH2);
	m_bPrescanDiePitchCheck		= pApp->GetFeatureStatus(MS896A_FUNC_SCAN_DIE_PITCH_CHECK);
	m_bEnableWaferSizeSelect	= pApp->GetFeatureStatus(MS896A_FUNC_WAFER_SIZE_SELECT);
	m_bMS100OriginalBHSequence	= pApp->GetFeatureStatus(MS896A_FUNC_MS100_ORG_BH_SQ);
	m_bUseOptBinCountDynAssignGrade = pApp->GetFeatureStatus(MS896A_FUNC_OPT_BIN_COUNT_DYN_ASSIGN_GRADE);
	m_bOpenMatchPkgNameCheck		= pApp->GetFeatureStatus(MS896A_FUNC_OPEN_MATCH_PKG_NAME_CHECK);
	m_bNoIgnoreGradeOnInterface = pApp->GetFeatureStatus(MS896A_FUNC_NO_IGNORE_GRADE_ON_INTERFACE);
	m_ulIgnoreGrade				= pApp->GetFeatureValue(MS896A_FUNC_IGNORE_GRADE);
	m_bCheckIgnoreGradeCount	= pApp->GetFeatureStatus(MS896A_FUNC_CHECK_IGNORE_GRADE_COUNT);
	m_bEnableSCNFile			= pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_USE_SCN);
	m_bEnablePsmFileExistCheck	= pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_PSM_EXIST_CHECK);
	m_bEnableBatchIdFileCheck	= pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_BATCH_ID_FILE_CHECK);
	m_bDisableWaferMapFivePointCheckMsgSelection = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_DISABLE_FIVE_POINT_CHECK_MSG_SEL);
	m_bDisableBinSNRFormat		= pApp->GetFeatureStatus(MS896A_FUNC_BIN_SNR_SEL_FORMAT);
	m_bDisableClearCountFormat	= pApp->GetFeatureStatus(MS896A_FUNC_SEL_CLEAR_COUNT_FORMAT);
	m_bEnableDynMapHeaderFile	= pApp->GetFeatureStatus(MS896A_FUNC_DYNMAPHEADER);
	m_bBackupTempFile			= pApp->GetFeatureStatus(MS896A_FUNC_BACKUP_TEMP_FILE);
	m_bBackupOutputTempFile		= pApp->GetFeatureStatus(MS896A_FUNC_BACKUP_OUTPUT_TEMP_FILE);
	m_bForceDisableHaveOtherFile = pApp->GetFeatureStatus(MS896A_FUNC_FORCE_DISABLE_HAVE_OTHER_FILE);
	m_bEnableManualUploadBinSummary = pApp->GetFeatureStatus(MS896A_FUNC_MANUAL_UPLOAD_BIN_SUMMARY_FILE);
	m_bGenerateAccumulateDieCountReport = pApp->GetFeatureStatus(MS896A_FUNC_GEN_ACCUMULATE_DIE_COUNT_REPORT);
	m_bEnablePRDualColor		= pApp->GetFeatureStatus(MS896A_FUNC_VISION_DUAL_COLOR);
	m_bEnablePRAdaptWafer		= pApp->GetFeatureStatus(MS896A_FUNC_VISION_ADAPT_WAFER);
	m_bCounterCheckWithNormalDieRecord = pApp->GetFeatureStatus(MS896A_FUNC_VISION_LRN_DIE_COUNTER_CHECK_NORMAL_RECORD);
	m_bAddWaferIdInMsgSummaryFile = pApp->GetFeatureStatus(MS896A_FUNC_WAFERID_IN_MSGSUMMARY);
	m_bDisableOKSelectionInAlertMsg = pApp->GetFeatureStatus(MS896A_FUNC_DIS_OK_SELECTION_IN_ALERT_MSG);
	m_bEnableGradeMappingFile	= pApp->GetFeatureStatus(MS896A_FUNC_GRADE_MAPPING_FILE);
	m_bEnablePickNPlaceOutputFile	= pApp->GetFeatureStatus(MS896A_FUNC_PICK_N_PLACE_OUTPUT);
	m_bDisableDefaultSearchWnd = pApp->GetFeatureStatus(MS896A_FUNC_DISABLE_DEFAULT_SEARCH_WND);
	m_bWaferEndFileGenProtection = pApp->GetFeatureStatus(MS896A_FUNC_WAFERENDFILEPROTECTION);
	m_bAutoGenWaferEndFile = pApp->GetFeatureStatus(MS896A_FUNC_AUTO_GEN_WAFEREND);
	m_bUseClearBinByGrade		= pApp->GetFeatureStatus(MS896A_FUNC_CLR_BIN_BY_GRADE);
	m_bRemoveBackupOutputFile	= pApp->GetFeatureStatus(MS896A_REMOVE_BACKUP_OUTPUTFILE);
	m_bAutoGenBinBlkCountSummary = pApp->GetFeatureStatus(MS896A_FUNC_AUTO_GEN_BIN_BLK_SUMMARY);
	m_bChangeGradeBackupTempFile = pApp->GetFeatureStatus(MS896A_FUNC_CHANGE_GRADE_BACKUPTEMP_FILE);
	m_bKeepOutputFileLog		= pApp->GetFeatureStatus(MS896A_KEEP_OUTPUT_FILE_LOG);
	m_bEnableMultipleMapHeaderPage = pApp->GetFeatureStatus(MS896A_FUNC_MULTIPLE_HEADER_PAGE);
	m_bOutputFormatSelInNewLot = pApp->GetFeatureStatus(MS896A_FUNC_OF_FORMAT_SELECT);
	m_bEnableExtraClearBinInfo	= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_EXTRA_CLR_BIN_INTO);
	m_bEnableBinBlkTemplate		= pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_TEMPLATE);
	m_bEnableLoadRankIDFromMap	= pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_LOAD_RANKID_FROM_MAP);
	m_bEnableAutoLoadNamingFile = pApp->GetFeatureStatus(MS896A_FUNC_AUTOLOAD_NAMING_FILE);
	m_szRankIDFileNameInMap		= pApp->GetFeatureStringValue(MS896A_FUNC_BINBLK_RANKID_NAME_IN_MAP);
	m_szRankIDFileExt			= pApp->GetFeatureStringValue(MS896A_FUNC_BINBLK_RANKID_FILE_EXT);
	m_bEnableOptimizeBinCountFunc = pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT);
	m_bOptimizeBinCountPerWftFunc = pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT_PER_WAFER);
	m_bEnableRestoreBinRunTimeData = pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_RESTORE_BINRUMTIME);
	m_bAddDieTimeStamp			= pApp->GetFeatureStatus(MS896A_FUNC_DIE_TIMESTAMP);
	m_bCheckProcessTime			= pApp->GetFeatureStatus(MS896A_FUNC_DIE_PROCESSTIME);
	m_bAddSortingSequence		= pApp->GetFeatureStatus(MS896A_FUNC_ADD_SORTING_SEQUENCE);

	m_bEnableStartupCoverSensorCheck = pApp->GetFeatureStatus(MS896A_FUNC_START_UP_COVER_SNR_CHECK);
	m_bEnableBinLoderCoverSensorCheck = pApp->GetFeatureStatus(MS896A_FUNC_BINLOADER_COVER_SNR_CHECK);

	//Get Feature value
	m_ulDefaultBinSNRFormat		= pApp->GetFeatureValue(MS896A_FUNC_BIN_SNR_DEFAULT_FORMAT);
	m_ulDefaultClearCountFormat = pApp->GetFeatureValue(MS896A_FUNC_CLEAR_COUNT_DEFAULT_FORMAT);
	m_bEnableAutoDieTypeFieldnameChek = pApp->GetFeatureStatus(MS896A_ENABLE_AUTO_TYPE_FIELD_NAME_CHECK);
	m_bClearDieTypeFieldDuringClearAllBin = pApp->GetFeatureStatus(MS896A_CLEAR_DIE_TYPE_FIELD_AFTER_CLEAR_ALL_BIN);
	m_szDieTypeFieldnameInMap = pApp->GetFeatureStringValue(MS896A_DIE_TYPE_FIELD_NAME_IN_MAP);

	m_bPackageFileMapHeaderCheckFunc = pApp->GetFeatureStatus(MS896A_PKG_FILE_MAP_HEADER_CHECK_FUNC);
	m_szPackageFileMapHeaderCheckString = pApp->GetFeatureStringValue(MS896A_PKG_FILE_CHECK_MAP_HEADER_STRING);
	m_bUpdateWaferMapHeader = pApp->GetFeatureStatus(MS896A_UPDATE_WAFER_MAP_HEADER);

	m_bEnableAutoMapDieTypeCheckFunc = pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_DIE_TYPE_CHECK_FUNC);

	for (INT i = 0; i < MS896A_DIE_TYPE_CHECK_LIMIT; i++)
	{
		CString szTemp;
		//szTemp.Format("Die Type Check Fieldname%d", i+1);
		szTemp = MS896A_FUNC_DIE_TYPE_CHECK_FIELDNAME;
		szTemp.AppendFormat("%d", i + 1);
		m_szMapDieTypeCheckString[i] = pApp->GetFeatureStringValue(szTemp);
	}

	m_bUseEmptyCheck			= pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_EMPTYCHECK);			//Block2
	m_bUseAutoManualAlign		= pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_AUTO_MANUAL_ALIGN);	//Block2
	m_bAdaptPredictAlgorithm	= TRUE;	//	pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ADAPT_ALGORITHM);		//v3.54T4
	m_bUseReVerifyRefDie		= pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_REVERIFY_REFDIE);		//Cree
	m_bNonBlkPkEndVerifyRefDie	= pApp->GetFeatureStatus(MS896A_FUNC_NONBLKPICK_VERIFY_REFDIE);
	m_bManualAlignReferDie		= pApp->GetFeatureStatus(MS896A_FUNC_MANUAL_ALIGN_REFER_DIE);
	m_bUseRefDieFaceValueCheck	= pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_REFDIE_FACE_VALUE);	//v4.48A26	//Avago
	m_bEnableClearBinCopyTempFile	= pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_COPY_TEMP_FILE_IN_CLEAR_BIN);
	m_bMS60NGPick				= pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_NGPICK);				//v4.54A4
	m_bAutoUploadOutputFile		= pApp->GetFeatureStatus(MS896A_AUTO_TRANSFER_OUTPUT_FILE);
} //end SetFeatureStatus


VOID CMS896AStn::SetLampDirect(LONG lLevel, BOOL bLockAlarmLamp)
{
	
	if (m_fHardware)
	{
		CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
		CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
		BOOL bDisableBuzzer	= pAppMod->GetFeatureStatus(MS896A_FUNC_DISABLE_BUZZER);
	
		UCHAR ucRed		= 0;
		UCHAR ucYellow	= 0;
		UCHAR ucGreen	= 0;
		UCHAR ucBuzzer	= 0;

		if (bLockAlarmLamp == TRUE)
		{
			slLock.Lock();
		}

		if (CMSLogFileUtility::Instance()->GetEnableMachineLog() == TRUE)
		{
			CString szLog;
			szLog.Format(" - fm %d to %d", m_lLevel, lLevel);
			SetAlarmLog(szLog);
		}

		CString szAlarmCode = "1";
		switch (lLevel)
		{
		case ALARM_GREEN:		// Green
			/*MotionSetOutputBit("oAlarmLampGreen",	TRUE);
			MotionSetOutputBit("oAlarmLampYellow",	FALSE);
			MotionSetOutputBit("oAlarmLampRed",		FALSE);
			MotionSetOutputBit("oAlarmSound",		FALSE);*/
			m_lLevel	= ALARM_GREEN;
			ucRed		= 0;
			ucYellow	= 0;
			ucGreen		= 1;
			ucBuzzer	= 0;
			szAlarmCode = "0";
			break;

		case ALARM_YELLOW:		// Yellow
			/*MotionSetOutputBit("oAlarmLampGreen",	FALSE);
			MotionSetOutputBit("oAlarmLampYellow",	TRUE);
			MotionSetOutputBit("oAlarmLampRed",		FALSE);
			MotionSetOutputBit("oAlarmSound",		FALSE);*/
			m_lLevel	= ALARM_YELLOW;
			ucRed		= 0;
			ucYellow	= 1;
			ucGreen		= 0;
			ucBuzzer	= 0;
			szAlarmCode = "1";
			break;

		case ALARM_RED:		// Red
			/*MotionSetOutputBit("oAlarmLampGreen",	FALSE);
			MotionSetOutputBit("oAlarmLampYellow",	FALSE);
			MotionSetOutputBit("oAlarmLampRed",		TRUE);
			MotionSetOutputBit("oAlarmSound",		bDisableBuzzer ? FALSE : TRUE);*/
			m_lLevel	= ALARM_RED;
			ucRed		= 1;
			ucYellow	= 0;
			ucGreen		= 0;
			ucBuzzer	= bDisableBuzzer ? 0 : 1;
			szAlarmCode = "2";
			break;

		case ALARM_GREEN_YELLOW_RED_ON:	//	Green + Yellow + Red on
			/*MotionSetOutputBit("oAlarmLampGreen",	TRUE);
			MotionSetOutputBit("oAlarmLampYellow",	TRUE);
			MotionSetOutputBit("oAlarmLampRed",		TRUE);
			MotionSetOutputBit("oAlarmSound",		FALSE);*/
			m_lLevel	= ALARM_GREEN_YELLOW_RED_ON;
			ucRed		= 1;
			ucYellow	= 1;
			ucGreen		= 1;
			ucBuzzer	= 0;
			szAlarmCode = "2";
			break;

		case ALARM_RED_NO_BUZZER:		// red but no buzzer
			/*MotionSetOutputBit("oAlarmLampGreen",	FALSE);
			MotionSetOutputBit("oAlarmLampYellow",	FALSE);
			MotionSetOutputBit("oAlarmLampRed",		TRUE);
			MotionSetOutputBit("oAlarmSound",		FALSE);*/
			m_lLevel	= ALARM_RED_NO_BUZZER;
			ucRed		= 1;
			ucYellow	= 0;
			ucGreen		= 0;
			ucBuzzer	= 0;
			szAlarmCode = "2";
			break;

		case ALARM_REDYELLOW_BUZZER:	// red yellow with buzzer
			/*MotionSetOutputBit("oAlarmLampGreen",	FALSE);
			MotionSetOutputBit("oAlarmLampYellow",	TRUE);
			MotionSetOutputBit("oAlarmLampRed",		TRUE);
			MotionSetOutputBit("oAlarmSound",		bDisableBuzzer ? FALSE : TRUE);*/
			m_lLevel	= ALARM_REDYELLOW_BUZZER;
			ucRed		= 1;
			ucYellow	= 1;
			ucGreen		= 0;
			ucBuzzer	= bDisableBuzzer ? 0 : 1;
			szAlarmCode = "1";
			break;

		case ALARM_REDYELLOW_NO_BUZZER:	// red yellow no buzzer
		/*	MotionSetOutputBit("oAlarmLampGreen",	FALSE);
			MotionSetOutputBit("oAlarmLampYellow",	TRUE);
			MotionSetOutputBit("oAlarmLampRed",		TRUE);
			MotionSetOutputBit("oAlarmSound",		FALSE);*/
			m_lLevel	= ALARM_REDYELLOW_NO_BUZZER;
			ucRed		= 1;
			ucYellow	= 1;
			ucGreen		= 0;
			ucBuzzer	= 0;
			szAlarmCode = "2";
			break;
	
		case ALARM_OFF_LIGHT: // off all light
		/*	MotionSetOutputBit("oAlarmLampGreen",	FALSE);
			MotionSetOutputBit("oAlarmLampYellow",	FALSE);
			MotionSetOutputBit("oAlarmLampRed",		FALSE);*/
			m_lLevel	= ALARM_OFF_LIGHT;
			ucRed		= 0;
			ucYellow	= 0;
			ucGreen		= 0;
			ucBuzzer	= 0;
			szAlarmCode = "3";
			break;
	
		case ALARM_YELLOW_BUZZER: // yellow with buzzer
			/*MotionSetOutputBit("oAlarmLampGreen",	FALSE);
			MotionSetOutputBit("oAlarmLampYellow",	TRUE);
			MotionSetOutputBit("oAlarmLampRed",		FALSE);
			MotionSetOutputBit("oAlarmSound",		bDisableBuzzer ? FALSE : TRUE);*/
			m_lLevel = ALARM_YELLOW_BUZZER;
			ucRed		= 0;
			ucYellow	= 1;
			ucGreen		= 0;
			ucBuzzer	= bDisableBuzzer ? 0 : 1;
			szAlarmCode = "1";
			break;

		case ALARM_YELLOWGREEN_BUZZER: // yellow green with buzzer // 4.51D8
		/*	MotionSetOutputBit("oAlarmLampGreen",	TRUE);
			MotionSetOutputBit("oAlarmLampYellow",	TRUE);
			MotionSetOutputBit("oAlarmLampRed",		FALSE);
			MotionSetOutputBit("oAlarmSound",		bDisableBuzzer ? FALSE : TRUE);*/
			m_lLevel	= ALARM_YELLOWGREEN_BUZZER;
			ucRed		= 0;
			ucYellow	= 1;
			ucGreen		= 1;
			ucBuzzer	= bDisableBuzzer ? 0 : 1;
			szAlarmCode = "1";
			break;

		case ALARM_YELLOWGREEN_NO_BUZZER: // yellow green without buzzer
			/*MotionSetOutputBit("oAlarmLampGreen",	TRUE);
			MotionSetOutputBit("oAlarmLampYellow",	TRUE);
			MotionSetOutputBit("oAlarmLampRed",		FALSE);
			MotionSetOutputBit("oAlarmSound",		FALSE);*/
			m_lLevel	= ALARM_YELLOWGREEN_NO_BUZZER;
			ucRed		= 0;
			ucYellow	= 1;
			ucGreen		= 1;
			ucBuzzer	= 0;
			szAlarmCode = "1";
			break;

		case ALARM_YELLOWGREEN_NOMATERIAL:	//v4.59A15	//Renesas MS90
			/*MotionSetOutputBit("oAlarmLampGreen",	TRUE);
			MotionSetOutputBit("oAlarmLampYellow",	TRUE);
			MotionSetOutputBit("oAlarmLampRed",		FALSE);
			MotionSetOutputBit("oAlarmSound",		bDisableBuzzer ? FALSE : TRUE);*/
			m_lLevel = ALARM_YELLOWGREEN_NOMATERIAL;
			ucRed		= 0;
			ucYellow	= 1;
			ucGreen		= 1;
			ucBuzzer	= bDisableBuzzer ? 0 : 1;
			szAlarmCode = "1";
			break;

		case ALARM_OFF_NOMATERIAL:			//v4.59A15	//Renesas MS90
			/*MotionSetOutputBit("oAlarmLampGreen",	TRUE);
			MotionSetOutputBit("oAlarmLampYellow",	FALSE);
			MotionSetOutputBit("oAlarmLampRed",		FALSE);
			MotionSetOutputBit("oAlarmSound",		bDisableBuzzer ? FALSE : TRUE);*/
			m_lLevel = ALARM_OFF_NOMATERIAL;
			ucRed		= 0;
			ucYellow	= 0;
			ucGreen		= 1;
			ucBuzzer	= bDisableBuzzer ? 0 : 1;
			break;

		case ALARM_OFF_ALL:
		default:
			/*MotionSetOutputBit("oAlarmLampGreen",	FALSE);
			MotionSetOutputBit("oAlarmLampYellow",	FALSE);
			MotionSetOutputBit("oAlarmLampRed",		FALSE);
			MotionSetOutputBit("oAlarmSound",		FALSE);*/
			m_lLevel = ALARM_OFF_ALL;
			ucRed		= 0;
			ucYellow	= 0;
			ucGreen		= 0;
			ucBuzzer	= 0;
			szAlarmCode = "3";
			break;
		}

		if( m_bEnableAlarmLog )
		{
			CTime theTime = CTime::GetCurrentTime();
			CString szMcNo = GetMachineNo();
			CString szTime = theTime.Format("%Y%m%d%H%M%S");
			CString szFileName = m_szAlarmLogPath + "\\" + szMcNo + "_" + szTime + "_" + szAlarmCode + ".txt";
			//v4.50 Klocwork
			//FILE *fp;
			//fp = fopen(szFileName, "w");
			//fclose(fp);
		}

		//OsramTrip 8/22
		SetGemValue(SG_SECS_NAME_ALARM_SIGNAL_RED,		ucRed);
		SetGemValue(SG_SECS_NAME_ALARM_SIGNAL_YELLOW,	ucYellow);
		SetGemValue(SG_SECS_NAME_ALARM_SIGNAL_GREEN,	ucGreen);
		SetGemValue(SG_SECS_NAME_ALARM_SIGNAL_BUZZER,	ucBuzzer);
		SendEvent(SG_CEID_ALARM_TOWER_SIGNAL);

		if (bLockAlarmLamp == TRUE)
		{
			slLock.Unlock();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Set Alarm Lamp
// Remark: The idle state of CSafety will check the value and set the lamp properly
////////////////////////////////////////////////////////////////////////////////////
VOID CMS896AStn::SetAlarmLamp_Red_Only(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_RED_NO_BUZZER, bBlink, bLockAlarmLamp);
}

VOID CMS896AStn::SetAlarmLamp_RedYellow_Only(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_REDYELLOW_NO_BUZZER, bBlink, bLockAlarmLamp);
}

VOID CMS896AStn::SetAlarmLamp_Red(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_RED, bBlink, bLockAlarmLamp);
}

VOID CMS896AStn::SetAlarmLamp_Yellow(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_YELLOW, bBlink, bLockAlarmLamp); 
}

VOID CMS896AStn::SetAlarmLamp_Green(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_GREEN, bBlink, bLockAlarmLamp); 
}

VOID CMS896AStn::SetAlarmLamp_RedYellow(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_REDYELLOW_BUZZER, bBlink, bLockAlarmLamp); 
}

VOID CMS896AStn::SetAlarmLamp_NoMaterial(BOOL bBlink, BOOL bLockAlarmLamp)
{
	//v4.59A16	//Renesas MS90
	SetAlarmLamp_Status(ALARM_YELLOWGREEN_NOMATERIAL, bBlink, bLockAlarmLamp);	
}

VOID CMS896AStn::SetAlarmLamp_YellowGreen(BOOL bBlink, BOOL bLockAlarmLamp) //4.51D8
{
	SetAlarmLamp_Status(ALARM_YELLOWGREEN_BUZZER, bBlink, bLockAlarmLamp); 
}

VOID CMS896AStn::SetAlarmLamp_YellowGreenNoBuzzer(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_YELLOWGREEN_NO_BUZZER, bBlink, bLockAlarmLamp); 
}

VOID CMS896AStn::SetAlarmLamp_OffLight(BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_OFF_LIGHT, FALSE, bLockAlarmLamp);
}

VOID CMS896AStn::SetAlarmLamp_YellowBuzzer(BOOL bBlink, BOOL bLockAlarmLamp)
{
	SetAlarmLamp_Status(ALARM_YELLOW_BUZZER, bBlink, bLockAlarmLamp); 
}

VOID CMS896AStn::SetAlarmLamp_Back(LONG lOrgStatus, BOOL bBlink, BOOL bLockAlarmLamp)
{
	CString szMsg;
	LONG lNewStatus = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (lOrgStatus == ALARM_YELLOW)
	{
		SetAlarmLamp_Yellow(bBlink, bLockAlarmLamp);
		lNewStatus = ALARM_YELLOW;
	}
	else if (lOrgStatus == ALARM_OFF_LIGHT)		//shiraishi01
	{
		//SetAlarmLamp_OffLight(bLockAlarmLamp);
		SetAlarmLamp_Yellow(bBlink, bLockAlarmLamp);
		lNewStatus = ALARM_YELLOW;
	}
	else if (lOrgStatus == ALARM_OFF_ALL)		//shiraishi01
	{
		//SetAlarmLamp_OffLight(bLockAlarmLamp);
		SetAlarmLamp_Yellow(bBlink, bLockAlarmLamp);
		lNewStatus = ALARM_YELLOW;
	}
	else
	{
		SetAlarmLamp_Green(bBlink, bLockAlarmLamp);
		lNewStatus = ALARM_GREEN;
	}

	szMsg.Format("SetAlarmLamp_Back Org Status: %d, New: %d", lOrgStatus, lNewStatus);
	SetAlarmLog(szMsg);
}

LONG CMS896AStn::GetAlarmLamp_Status()
{
	return CMS896AApp::m_lAlarmLampStatus;
}

VOID CMS896AStn::SetAlarmLamp_Status(LONG lLevel, BOOL bBlink, BOOL bLockAlarmLamp)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	
	if (bLockAlarmLamp == TRUE)
	{
		slLock.Lock();
	}

	LONG lNewLevel = lLevel;

	CMS896AApp::m_bEnableAlarmTowerBlink	= bBlink;
	CMS896AApp::m_lAlarmBlinkStatus			= lNewLevel;
	CMS896AApp::m_lAlarmLampStatus			= lNewLevel;

	SetLampDirect(lLevel, FALSE);	// xuzhijin_plsg	
	if (bLockAlarmLamp == TRUE)
	{
		slLock.Unlock();
	}
}


//barry added new function



BOOL  CMS896AStn::IsHardwareReady()		// Check whether the hardware is ready
{
	return m_bHardwareReady;
}


// Show and close the loading alert box
VOID CMS896AStn::StartLoadingAlert()
{
	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);

	if (pApp != NULL)
	{
		pApp->StartLoadingAlert();    // Show a alert box
	}	
}

VOID CMS896AStn::CloseLoadingAlert()
{
	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);

	if (pApp != NULL)
	{
		pApp->CloseLoadingAlert();    // Show a alert box
	}	
}

VOID CMS896AStn::OpenWaitingAlert()
{
	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);

	if (pApp != NULL)
	{
		pApp->OpenWaitAlert();    // Show a alert box
	}	
}

VOID CMS896AStn::CloseWaitingAlert()
{
	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);

	if (pApp != NULL)
	{
		pApp->CloseWaitAlert();    // Show a alert box
	}	
}

// Backup to NVRAM (Hardware)
VOID CMS896AStn::BackupToNVRAM()
{
	CSingleLock slLock(&m_csBackupNVRam);
	slLock.Lock();

	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);

	if (pApp != NULL)
	{
		pApp->BackupToNVRAM(); 
	}	

	slLock.Unlock();
}


// Set progress bar limit & update percentage
BOOL CMS896AStn::SetProgressBarLimit(LONG lLimit)
{
	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);
	BOOL bReply = FALSE;

	if (pApp != NULL)
	{
		bReply = pApp->SetProgressBarLimit(lLimit);	
	}

	return bReply;
}

BOOL CMS896AStn::UpdateProgressBar(LONG lPercent)
{
	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);
	BOOL bReply = FALSE;

	if (pApp != NULL)
	{
		bReply = pApp->UpdateProgressBar(lPercent);	
	}

	return bReply;
}

VOID CMS896AStn::GetAlarmCodeAction(CString szAlarmCode, LONG &lAction)
{
	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);
	
	if (pApp != NULL)
	{
		pApp->GetAlarmCodeAction(szAlarmCode, lAction);
	}
}

// Set & Get Housekeeping sequence token
BOOL CMS896AStn::SetHouseKeepingToken(CString szMessage, BOOL bStatus)
{
	m_csStopToken.Lock();
	m_bAllowHouseKeeping = bStatus;
	if( bStatus==TRUE )
	{
		LogCycleStopState(szMessage + " return House Keeping token");
	}
	m_csStopToken.Unlock();

	return TRUE;
}

BOOL CMS896AStn::GetHouseKeepingToken(CString szMessage)
{
	BOOL bStatus;

	m_csStopToken.Lock();
	bStatus = m_bAllowHouseKeeping;

	if (m_bAllowHouseKeeping == TRUE)
	{
		LogCycleStopState(szMessage + " obtain House Keeping token");
		m_bAllowHouseKeeping = FALSE;
	}

	m_csStopToken.Unlock();

	return bStatus;
}


VOID CMS896AStn::FlushMessage()
{
}

VOID CMS896AStn::UpdateStationData()
{
}

VOID CMS896AStn::ReloadMachineConstant()
{
}

VOID CMS896AStn::ExportMachineConstant()
{
}

VOID CMS896AStn::ImportMachineConstant()
{
}

BOOL CMS896AStn::SetAlertTime(BOOL bStart, UCHAR ucType, CString szCode, CString szContent)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	szCode.Replace(",", ";");
	szContent.Replace(",", ";");
	szContent.Replace("\n", " ");

	if (pApp->m_bCycleStarted == TRUE)
	{
		pApp->m_eqMachine2.SetTime(bStart, ucType, szCode, szContent);		//WH Sanan	//v4.40T4
		return pApp->m_eqMachine.SetTime(bStart, ucType, szCode, szContent);
	}

	return TRUE;
}

BOOL CMS896AStn::IsLoadingPKGFile()
{
	return CMS896AApp::m_bIsLoadingPKGFile;
}

BOOL CMS896AStn::IsLoadingPortablePKGFile()
{
	BOOL bLoadingPortablePKGFile = FALSE;

	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);

	if (pApp != NULL)	
	{
		bLoadingPortablePKGFile = pApp->IsPortablePKGFile();
	}
	
	return bLoadingPortablePKGFile;
}

BOOL CMS896AStn::RemoveFilesInFolder(CString szFilePath)
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString szFilename = "";
	
	hSearch = FindFirstFile(szFilePath + "\\" + "*", &FileData);

	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		return FALSE;
	}

	do 
	{
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;
        if ( (strcmp(FileData.cFileName, ".")==0) || (strcmp(FileData.cFileName, "..")==0) )
            continue;

		szFilename = szFilePath + "\\" + FileData.cFileName;
		DeleteFile(szFilename);
				
	} 
	while (FindNextFile(hSearch, &FileData) == TRUE);

	// Close the search handle. 
	if (FindClose(hSearch) == FALSE) 
	{ 
		return FALSE;
	} 
	
	return TRUE;
}

LONG CMS896AStn::GetWaferCenterX()
{
	LONG lWt2OffsetX = 0;
	LONG lEjtOffsetX = 0;
	LONG lProbeOffsetX = 0;

	return m_lWaferCenterX + lWt2OffsetX + lEjtOffsetX + lProbeOffsetX;
}

LONG CMS896AStn::GetWaferCenterY()
{
	LONG lWt2OffsetY = 0;
	LONG lEjtOffsetY = 0;
	LONG lProbeOffsetY = 0;

	return m_lWaferCenterY + lWt2OffsetY + lEjtOffsetY + lProbeOffsetY;
}

//	4.24TX1
LONG CMS896AStn::GetWft1CenterX(BOOL bIsPosOffset)
{
	LONG lWaferCenterX = m_lWaferCenterX;

	if (bIsPosOffset)
	{
		lWaferCenterX = m_lWaferCenterX + m_lEjtOffsetX;
	}

	return lWaferCenterX;
}

LONG CMS896AStn::GetWft1CenterY(BOOL bIsPosOffset)
{
	LONG lWaferCenterY = m_lWaferCenterY;

	if (bIsPosOffset)
	{
		lWaferCenterY = m_lWaferCenterY + m_lEjtOffsetY;
	}

	return lWaferCenterY;
}

LONG CMS896AStn::GetWft2CenterX(BOOL bIsPosOffset)
{
	LONG lWaferCenterX = m_lWaferCenterX + m_lWT2OffsetX;
	
	if (bIsPosOffset)
	{
		lWaferCenterX = m_lWaferCenterX + m_lWT2OffsetX + m_lEjtOffsetX;
	}

	return lWaferCenterX;
}

LONG CMS896AStn::GetWft2CenterY(BOOL bIsPosOffset)
{
	LONG lWaferCenterY = m_lWaferCenterY + m_lWT2OffsetY;

	if (bIsPosOffset)
	{
		lWaferCenterY = m_lWaferCenterY + m_lWT2OffsetY + m_lEjtOffsetY;
	}

	return lWaferCenterY;
}

LONG CMS896AStn::GetWaferDiameter()
{
	return m_lWaferSize;
}

BOOL CMS896AStn::IsWithinInputWaferLimit(LONG lX, LONG lY, DOUBLE dScale)
{
	if (m_ucWaferLimitType == WT_CIRCLE_LIMIT)
	{
		if (IsMS90())	//v4.50A1
		{
			if ( (m_lWafXPosLimit != 0) && (m_lWafXNegLimit != 0) )
			{
				if ( (lX > m_lWafXPosLimit) || (lX < m_lWafXNegLimit) )
				{
					CString szErr;
					szErr.Format("WT: IsWithinInputWaferLimit(MS90) - X is out of range; XEnc=%ld (%ld, %ld)",
						lX, m_lWafXNegLimit, m_lWafXPosLimit);
					//SetErrorMessage(szErr);
					return FALSE;
				}
			}

			if ( (m_lWafYPosLimit != 0) && (m_lWafYNegLimit != 0) )
			{
				if ( (lY > m_lWafYPosLimit) || (lY < m_lWafYNegLimit) )
				{
					CString szErr;
					szErr.Format("WT: IsWithinInputWaferLimit(MS90) - Y is out of range; YEnc=%ld (%ld, %ld)",
						lY, m_lWafYNegLimit, m_lWafYPosLimit);
					//SetErrorMessage(szErr);
					return FALSE;
				}
			}
		}


		LONG lWaferRadius = GetWaferDiameter() / 2;
		DOUBLE	dDistance = 0.0;

		dDistance = sqrt(((double)abs(lX - GetWaferCenterX()) * (double)abs(lX - GetWaferCenterX()))
						 + ((double)abs(lY - GetWaferCenterY()) * (double)abs(lY - GetWaferCenterY())));

		if (dDistance <= ((DOUBLE)(lWaferRadius) * dScale))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return IsInsidePolygon(lX, lY);
	}

	return FALSE;
}


BOOL CMS896AStn::IsWithinWT1WaferLimit(LONG lX, LONG lY, DOUBLE dScale, BOOL bIsPosOffset)	// 4.24TX2
{
	if (m_ucWaferLimitType == WT_CIRCLE_LIMIT)
	{
		if (IsMS90())	//v4.50A1
		{
			if ( (m_lWafXPosLimit != 0) && (m_lWafXNegLimit != 0) )
			{
				if ( (lX > m_lWafXPosLimit) || (lX < m_lWafXNegLimit) )
				{
					CString szErr;
					szErr.Format("WT: IsWithinInputWaferLimit(MS90) - X is out of range; XEnc=%ld (%ld, %ld)",
						lX, m_lWafXNegLimit, m_lWafXPosLimit);
					//SetErrorMessage(szErr);
					return FALSE;
				}
			}

			if ( (m_lWafYPosLimit != 0) && (m_lWafYNegLimit != 0) )
			{
				if ( (lY > m_lWafYPosLimit) || (lY < m_lWafYNegLimit) )
				{
					CString szErr;
					szErr.Format("WT: IsWithinInputWaferLimit(MS90) - Y is out of range; YEnc=%ld (%ld, %ld)",
						lY, m_lWafYNegLimit, m_lWafYPosLimit);
					//SetErrorMessage(szErr);
					return FALSE;
				}
			}
		}

		LONG lWaferRadius = GetWaferDiameter() / 2;
		DOUBLE	dDistance = 0.0;

		dDistance = sqrt(((double)abs(lX - GetWft1CenterX(bIsPosOffset)) * (double)abs(lX - GetWft1CenterX(bIsPosOffset))) +
						 ((double)abs(lY - GetWft1CenterY(bIsPosOffset)) * (double)abs(lY - GetWft1CenterY(bIsPosOffset))));

		if (dDistance <= ((DOUBLE)(lWaferRadius) * dScale))
		{
			return TRUE;
		}
		else
		{
			if (IsWT2InUse() == FALSE)
			{
				CString szErr;
				szErr.Format("ERROR: IsWithinWT1WaferLimit exceeds wafer limit %d", bIsPosOffset);
				//	SetErrorMessage(szErr);		//v4.28
			}
			return FALSE;
		}
	}
	else
	{
		return IsInsidePolygon(lX, lY);
	}

	return FALSE;
}

BOOL CMS896AStn::IsWithinWT2WaferLimit(LONG lX, LONG lY, DOUBLE dScale, BOOL bIsPosOffset)	// 4.24TX2
{
	if (m_ucWaferLimitType == WT_CIRCLE_LIMIT)
	{
		LONG lWaferRadius = GetWaferDiameter() / 2;
		DOUBLE	dDistance = 0.0;

		dDistance = sqrt(((double)abs(lX - GetWft2CenterX(bIsPosOffset)) * (double)abs(lX - GetWft2CenterX(bIsPosOffset))) +
						 ((double)abs(lY - GetWft2CenterY(bIsPosOffset)) * (double)abs(lY - GetWft2CenterY(bIsPosOffset))));

		if (dDistance <= ((DOUBLE)(lWaferRadius) * dScale))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return IsInsidePolygon(lX, lY);
	}

	return FALSE;
}

DOUBLE CMS896AStn::GetDistance(DOUBLE pt1x, DOUBLE pt1y, DOUBLE pt2x, DOUBLE pt2y)
{
	DOUBLE dX = pt1x - pt2x;
	DOUBLE dY = pt1y - pt2y;

	return sqrt(dX*dX + dY*dY);
}

BOOL CMS896AStn::IsScanNgPick()
{
	if( IsESMachine() )
	{
		if( GetScanAction()==PRESCAN_FINISHED_SORT_BY_GRADE )
		{
			return TRUE;
		}
	}
	return GetScanAction()==PRESCAN_FINISHED_PICK_NG;
}

LONG CMS896AStn::GetScanAction()
{
	if( m_lPrescanFinishedAction==PRESCAN_FINISHED_PICK_NG )
	{
		if( m_bNgPickBadCut==FALSE && m_bNgPickEdgeDie==FALSE )
			m_bNgPickDefect = TRUE;
	}

	if( IsESMachine() )
	{
		if( m_lPrescanFinishedAction==PRESCAN_FINISHED_SORT_BY_GRADE )
		{
			if( m_bNgPickBadCut==FALSE && m_bNgPickEdgeDie==FALSE )
				m_bNgPickDefect = TRUE;
		}
	}

	return m_lPrescanFinishedAction;
}	//	get prescan finished action

BOOL CMS896AStn::IsBHStopped()
{
	return (BOOL)m_lBondHeadAtSafePos;
}

BOOL CMS896AStn::IsBHAtPrePick()
{
	return (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];
}

BOOL CMS896AStn::IsWaferEnded()
{
	return (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferEnd"];
}

BOOL CMS896AStn::IsInsidePolygon(LONG lX, LONG lY)
{
	LONG nFactor = 1000;
	LONG nM11, nM12, nM13;
	LONG nResult;
	LONG nStart, nFinih;
	LONG nPoint = 0;
	LONG nInRegion[WT_MAX_POLYGON_PT];
	LONG nInside = 0;

	memset(&nInRegion, 0, sizeof(nInRegion));

	for (nPoint = 0; nPoint < m_lWaferPolyLimitNo; nPoint++)
	{
		nStart = nPoint;
		nFinih = nPoint + 1;
		if (nFinih > m_lWaferPolyLimitNo - 1)
		{
			nFinih = 0;
		}

		nM11 = (m_lWaferPolyLimitX[nStart] / nFactor) * ((m_lWaferPolyLimitY[nFinih] / nFactor) - (lY / nFactor));
		nM12 = (m_lWaferPolyLimitX[nFinih] / nFactor) * ((m_lWaferPolyLimitY[nStart] / nFactor) - (lY / nFactor));
		nM13 = (lX / nFactor) * ((m_lWaferPolyLimitY[nStart] / nFactor) - (m_lWaferPolyLimitY[nFinih] / nFactor));
		nResult = nM11 - nM12 + nM13;

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

	for (nPoint = 1; nPoint < m_lWaferPolyLimitNo; nPoint++)
	{
		nInside = nInside & nInRegion[nPoint];
	}

	if (nInside == 1)
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CMS896AStn::IsWithinBinTableLimit(LONG lX, LONG lY)
{
	LONG lTableXPosLimit = (*m_psmfSRam)["BinTable"]["Limit"]["X Pos"];
	LONG lTableXNegLimit = (*m_psmfSRam)["BinTable"]["Limit"]["X Neg"];
	LONG lTableYPosLimit = (*m_psmfSRam)["BinTable"]["Limit"]["Y Pos"];
	LONG lTableYNegLimit = (*m_psmfSRam)["BinTable"]["Limit"]["Y Neg"];

	if ((lX > lTableXPosLimit) || (lX < lTableXNegLimit))
	{
		return FALSE;
	}

	if ((lY > lTableYPosLimit) || (lY < lTableYNegLimit))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMS896AStn::IsWithinBinTable2Limit(LONG lX, LONG lY)
{
	LONG lTableXPosLimit = (*m_psmfSRam)["BinTable2"]["Limit"]["X Pos"];
	LONG lTableXNegLimit = (*m_psmfSRam)["BinTable2"]["Limit"]["X Neg"];
	LONG lTableYPosLimit = (*m_psmfSRam)["BinTable2"]["Limit"]["Y Pos"];
	LONG lTableYNegLimit = (*m_psmfSRam)["BinTable2"]["Limit"]["Y Neg"];

	if ((lX > lTableXPosLimit) || (lX < lTableXNegLimit))
	{
		return FALSE;
	}

	if ((lY > lTableYPosLimit) || (lY < lTableYNegLimit))
	{
		return FALSE;
	}

	return TRUE;
}

LONG CMS896AStn::HmiMessageEx(const CString &szText,
							  const CString &szTitle,
							  LONG lType,
							  LONG lAlignment,
							  LONG lTimeout,
							  LONG lMsgMode,
							  LONG *plMsgId,
							  LONG lWidth,
							  LONG lHeight,
							  CString *pszBitmapFile,
							  CString *pszCustomText1,
							  CString *pszCustomText2,
							  CString *pszCustomText3,
							  LONG lTop,
							  LONG lLeft)
{
	IPC_CServiceMessage       svMsg;
	PCHAR pBuffer = NULL;
	LONG lReply = glHMI_ERROR;
	INT nLength = 0;
	INT nMsgIndex = 0;

	const INT nTCharSize = sizeof(TCHAR);

	if (szText == _T(""))
	{
		return glHMI_ERROR;
	}

	nLength = (szText.GetLength() + 1) * nTCharSize;
	nLength += (szTitle.GetLength() + 1) * nTCharSize;
	nLength += (sizeof(LONG) * 5);
	BOOL bPack = FALSE;

	if (lLeft >= 0)
	{
		nLength += sizeof(LONG);
		bPack = TRUE;
	}

	if ((lTop >= 0) || bPack)
	{
		nLength += sizeof(LONG);
		bPack = TRUE;
	}

	if (pszCustomText3 != NULL)
	{
		nLength += (pszCustomText3->GetLength() + 1) * nTCharSize;
		bPack = TRUE;
	}
	else if (bPack)
	{
		nLength += nTCharSize;
	}

	if (pszCustomText2 != NULL)
	{
		nLength += (pszCustomText2->GetLength() + 1) * nTCharSize;
		bPack = TRUE;
	}
	else if (bPack)
	{
		nLength += nTCharSize;
	}

	if (pszCustomText1 != NULL)
	{
		nLength += (pszCustomText1->GetLength() + 1) * nTCharSize;
		bPack = TRUE;
	}
	else if (bPack)
	{
		nLength += nTCharSize;
	}

	if (pszBitmapFile != NULL)
	{
		nLength += (pszBitmapFile->GetLength() + 1) * nTCharSize;
	}
	else if (bPack)
	{
		nLength += nTCharSize;
	}

	pBuffer = new CHAR[nLength];
	memset(pBuffer, 0x00, nLength);
	strncpy_s(pBuffer, nLength, szText, szText.GetLength() + 1);
	nMsgIndex = (szText.GetLength() + 1) * nTCharSize;

	strncpy_s(pBuffer + nMsgIndex, nLength - nMsgIndex, szTitle, szTitle.GetLength() + 1);
	nMsgIndex += (szTitle.GetLength() + 1) * nTCharSize;

	*((LONG*)(pBuffer + nMsgIndex)) = lType;
	nMsgIndex += sizeof(LONG);

	*((LONG*)(pBuffer + nMsgIndex)) = lAlignment;
	nMsgIndex += sizeof(LONG);

	*((LONG*)(pBuffer + nMsgIndex)) = lWidth;
	nMsgIndex += sizeof(LONG);

	*((LONG*)(pBuffer + nMsgIndex)) = lHeight;
	nMsgIndex += sizeof(LONG);

	LONG lTemp = lTimeout / 1000;
	*((LONG*)(pBuffer + nMsgIndex)) = lTemp;
	nMsgIndex += sizeof(LONG);

	if (pszBitmapFile != NULL)
	{
		strncpy_s(pBuffer + nMsgIndex, nLength - nMsgIndex, *pszBitmapFile, pszBitmapFile->GetLength() + 1);
		nMsgIndex += (pszBitmapFile->GetLength() + 1) * nTCharSize;
	}
	else if (nLength > nMsgIndex)
	{
		pBuffer[nMsgIndex] = '\0';
		nMsgIndex += nTCharSize;
	}

	if (pszCustomText1 != NULL)
	{
		strncpy_s(pBuffer + nMsgIndex, nLength - nMsgIndex, *pszCustomText1, pszCustomText1->GetLength() + 1);
		nMsgIndex += (pszCustomText1->GetLength() + 1) * nTCharSize;
	}
	else if (nLength > nMsgIndex)
	{
		pBuffer[nMsgIndex] = '\0';
		nMsgIndex += nTCharSize;
	}

	if (pszCustomText2 != NULL)
	{
		strncpy_s(pBuffer + nMsgIndex,nLength - nMsgIndex, *pszCustomText2, pszCustomText2->GetLength() + 1);
		nMsgIndex += (pszCustomText2->GetLength() + 1) * nTCharSize;
	}
	else if (nLength > nMsgIndex)
	{
		pBuffer[nMsgIndex] = '\0';
		nMsgIndex += nTCharSize;
	}

	if (pszCustomText3 != NULL)
	{
		strncpy_s(pBuffer + nMsgIndex, nLength - nMsgIndex, *pszCustomText3, pszCustomText3->GetLength() + 1);
		nMsgIndex += (pszCustomText3->GetLength() + 1) * nTCharSize;
	}
	else if (nLength > nMsgIndex)
	{
		pBuffer[nMsgIndex] = '\0';
		nMsgIndex += nTCharSize;
	}

	if (lTop >= 0)
	{
		*((LONG*)(pBuffer + nMsgIndex)) = lTop;
		nMsgIndex += sizeof(LONG);
	}
	else if (nLength > nMsgIndex)
	{
		*((LONG*)(pBuffer + nMsgIndex)) = -1;
		nMsgIndex += sizeof(LONG);
	}

	if (lLeft >= 0)
	{
		*((LONG*)(pBuffer + nMsgIndex)) = lLeft;
	}

	try
	{

		svMsg.InitMessage(nLength, pBuffer);
		delete[] pBuffer;
		pBuffer = NULL;

		int nTimeCnt = 0;
		int nTimeDur = lTimeout / 10;
		//INT nConvId = m_comClient.SendRequest(gszHMI_USER_SERVICE, gszHMI_MESSAGE_BOX, svMsg);
		INT nConvId = m_comClient.SendRequest(gszHMI_USER_SERVICE, "HmiMessageBoxEx", svMsg);
		CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szText);		//anichia001	//v4.43T10

		while (1)
		{
			if (m_comClient.ReadReplyForConvID(nConvId, svMsg, NULL, NULL, 0))
			{
				if (svMsg.GetMsgLen() == sizeof(LONG))
				{
					svMsg.GetMsg(sizeof(LONG), &lReply);
				}
				break;
			}
			else if (nTimeCnt++ > nTimeDur)
			{

				lReply = glHMI_TIMEOUT;
				break;
			}
			else
			{
				while (m_comServer.ProcessRequest());
				Sleep(10);
			}
		}
	}
	catch (CAsmException e)
	{
		if (pBuffer != NULL)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
	}

	return lReply;
}


BOOL CMS896AStn::IsDieUnpickSort(CONST ULONG ulDieState)
{
	if (ulDieState == WT_MAP_DIESTATE_UNPICK)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CMS896AStn::IsDieUnpickScan(CONST ULONG ulDieState)
{
	if (ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY ||
			ulDieState == WT_MAP_DS_UNPICK_REGRAB_EMPTY ||
			ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT ||
			ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CMS896AStn::IsDieUnpickAll(CONST ULONG ulDieState)
{
	if (ulDieState == WT_MAP_DIESTATE_UNPICK	||
			ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY ||
			ulDieState == WT_MAP_DS_UNPICK_REGRAB_EMPTY ||
			ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT ||
			ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CMS896AStn::IsDieEmpty(CONST ULONG ulDieState)
{
	return (ulDieState == WT_MAP_DIESTATE_EMPTY);
}

BOOL CMS896AStn::IsScanEmptyGrade(ULONG ulRow, ULONG ulCol)
{
	if (IsPrescanEnable() == FALSE)
	{
		return FALSE;
	}

	if (m_bPrescanEmptyAction == FALSE)
	{
		return FALSE;
	}

	if (m_bPrescanEmptyToNullBin)
	{
		return FALSE;
	}

	if (m_WaferMapWrapper.GetGrade(ulRow, ulCol) == (m_ucPrescanEmptyGrade + m_WaferMapWrapper.GetGradeOffset()))
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOL CMS896AStn::IsScanBadCutGrade(ULONG ulRow, ULONG ulCol)
{
	if (IsPrescanEnable() == FALSE)
	{
		return FALSE;
	}

	if (m_bPrescanBadCutAction == FALSE)
	{
		return FALSE;
	}

	if (m_bPrescanBadCutToNullBin)
	{
		return FALSE;
	}

	if (m_WaferMapWrapper.GetGrade(ulRow, ulCol) == (GetScanBadcutGrade() + m_WaferMapWrapper.GetGradeOffset()))
	{
		return TRUE;
	}
	
	return FALSE;
}

UCHAR CMS896AStn::GetScanDefectGrade()
{
	return m_ucPrescanDefectGrade;
}

UCHAR CMS896AStn::GetScanBadcutGrade()
{
	return m_ucPrescanBadCutGrade;
}

BOOL CMS896AStn::IsScannedDefectGrade(ULONG ulRow, ULONG ulCol)
{
	if (IsPrescanEnable() == FALSE)
	{
		return FALSE;
	}

	if (m_bPrescanDefectAction == FALSE)
	{
		return FALSE;
	}

	if (m_bPrescanDefectToNullBin)
	{
		return FALSE;
	}

	if (m_WaferMapWrapper.GetGrade(ulRow, ulCol) == (GetScanDefectGrade() + m_WaferMapWrapper.GetGradeOffset()))
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOL CMS896AStn::IsWprWithAF()
{
	return m_bWprWithAF_Z;
}

BOOL CMS896AStn::IsBLEnable()
{
	if (m_bDisableBL)
	{
		return FALSE;
	}

	return (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["Enabled"];
}

BOOL CMS896AStn::IsNLEnable()
{
	if (m_bDisableNL)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMS896AStn::IsEnableWL()
{
	if( m_bDisableWL )
	{
		return FALSE;
	}
	return (BOOL)(LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"];
}

BOOL CMS896AStn::IsOcrAOIMode()
{
	return m_bAoiEnableOcrDie;
}

BOOL CMS896AStn::IsAOIOnlyMachine()
{
	// call this command once when application starts
	if ((m_bDisableBH && m_bDisableBT && m_bDisableBL))
	{
		return TRUE;
	}
	else if (CMS896AApp::m_bES100v2DualWftOption == TRUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CMS896AStn::GetScnEnable()	// to generate SCN file or not, default is enable.
{
	return m_bES100DisableSCN==FALSE;
}

BOOL CMS896AStn::GetPsmEnable()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (pUtl->GetPrescanDummyMap() && pUtl->GetPrescanAoiWafer() == FALSE)
	{
		return FALSE;
	}
	
	if (m_bDisablePsm)
	{
		return FALSE;
	}

	return m_bPsmEnableState;
}

VOID CMS896AStn::SetPsmEnable(BOOL bEnable)
{
	BOOL bPsmEnable = TRUE;

	bPsmEnable = bEnable;
	
	if (m_bDisablePsm)
	{
		bPsmEnable = FALSE;
	}

	m_bPsmEnableState = bPsmEnable;
}


BOOL CMS896AStn::IsEnableFPC()
{
	return m_bEnableSCNCheck;
}

BOOL CMS896AStn::IsScanMapNgGrade(CONST UCHAR ucMapGrade)
{
	if (m_ucScanMapStartNgGrade > 0 && ucMapGrade >= m_ucScanMapStartNgGrade)
	{
		if (m_ucScanMapEndNgGrade >= m_ucScanMapStartNgGrade)
		{
			if (ucMapGrade <= m_ucScanMapEndNgGrade)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	else if (m_bPrescanSkipNgGrade && m_ucScanMapStartNgGrade == 0 && ucMapGrade == 0)
	{
		return TRUE;
	}

	if( m_ucScanMapStartNgGrade==255 && m_ucScanMapEndNgGrade==255 && m_ucScanMapNgListSize>0 )
	{
		for(int i=0; i<m_ucScanMapNgListSize; i++)
		{
			if( ucMapGrade == m_ucScanMapNgList[i] )
				return TRUE;
		}
	}

	return IsScanMapAOINgGrade(ucMapGrade);
}

BOOL CMS896AStn::IsScanMapAOINgGrade(CONST UCHAR ucMapGrade)
{
	if (m_ucScanMapStartAOINgGrade > 0 && ucMapGrade >= m_ucScanMapStartAOINgGrade)
	{
		if (m_ucScanMapEndAOINgGrade >= m_ucScanMapStartAOINgGrade)
		{
			if (ucMapGrade <= m_ucScanMapEndAOINgGrade)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	else if (m_bPrescanSkipAOINgGrade && m_ucScanMapStartAOINgGrade == 0 && ucMapGrade == 0)
	{
		return TRUE;
	}

	if( m_ucScanMapStartAOINgGrade==255 && m_ucScanMapEndAOINgGrade==255 && m_ucScanMapAOINgListSize>0 )
	{
		for(int i=0; i<m_ucScanMapAOINgListSize; i++)
		{
			if( ucMapGrade == m_ucScanMapAOINgList[i] )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CMS896AStn::IsOnlyRegionEnd()	// one of regions end, but whole wafer not end.
{
	CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	return pUtl->GetPrescanRegionMode() && pSRInfo->IsRegionEnd();
}

BOOL CMS896AStn::IsEnablePNP()
{
	return m_bEnablePickAndPlace;
}


BOOL CMS896AStn::IsPrescanEnable()
{
	if ( IsBurnIn())
	{
		return FALSE;
	}

	if (IsEnablePNP())
	{
		return FALSE;
	}

	if (IsOsramResortMode())
	{
		return FALSE;
	}

	return m_bEnablePrescan;
}

BOOL CMS896AStn::IsPrescanning()
{
	return IsPrescanEnable() && m_bIsPrescanning;
}

BOOL CMS896AStn::IsPrescanEnded()
{
	return IsPrescanEnable() && m_bIsPrescanned;
}

BOOL CMS896AStn::IsPrescanMapIndex()
{
	if( IsAutoRescanEnable() && GetNewPickCount()>0 )
	{
		return FALSE;
	}

	return IsPrescanEnable() && m_bES100ByMapIndex;
}

BOOL CMS896AStn::IsFastHomeScan()
{
	return FALSE;
	return m_bFastHomeScanEnable && m_bIMPrescanReferDie;
}

LONG CMS896AStn::IsScanAlignWafer()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if ( pUtl->GetPrescanDummyMap() )
	{
		if( IsMS90HalfSortMode()==FALSE )	//	SiLan 
		{
			return SCAN_ALIGN_WAFER_DISABLE;
		}
	}

	//return (IsPrescanEnable() && IsBurnIn()==FALSE && m_bMapDummyPrescanAlign)
	if (IsPrescanEnable() && IsBurnIn()==FALSE && (m_lMapPrescanAlignWafer == SCAN_ALIGN_WAFER_WITHOUT_REF))
	{
		return SCAN_ALIGN_WAFER_WITHOUT_REF;  //scan align wafer without reference die
	}
	if (IsPrescanEnable() && IsBurnIn()==FALSE && (m_lMapPrescanAlignWafer == SCAN_ALIGN_WAFER_WITH_REF))
	{
		return SCAN_ALIGN_WAFER_WITH_REF;  //scan align wafer with reference die
	}

	return SCAN_ALIGN_WAFER_DISABLE; //disble scan align wafer
}

BOOL CMS896AStn::IsMS90HalfSortDummyScanAlign()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	return IsMS90HalfSortMode() && IsScanAlignWafer() && !IsBurnIn() && pUtl->GetPrescanDummyMap();
}

BOOL CMS896AStn::IsScanWithWaferLimit()
{
	if( IsMS90HalfSortDummyScanAlign() )
	{
		return FALSE;
	}

	return m_bScanWithWaferLimit;
}

BOOL CMS896AStn::IsScanThenDivideMap()
{
	return /*(IsScanAlignWafer() && IsScanWithWaferLimit()) ||*/ m_bScanToHalfMap;
}


BOOL CMS896AStn::GetMapIndexTolerance(DOUBLE &dRowTol, DOUBLE &dColTol)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	pApp->GetScanMapIndexTolerance(dRowTol, dColTol);

	if( dRowTol<0.70 )
		dRowTol = 0.7;
	if( dColTol<0.7 )
		dColTol = 0.7;

	return TRUE;
}

LONG CMS896AStn::GetMapIndexStepRow()
{
	DOUBLE dRowTol = 0, dColTol = 0;
	GetMapIndexTolerance(dRowTol, dColTol);
	if (m_nPrescanIndexStepRow == 0 || m_nPrescanIndexStepRow > (LONG)(m_dPrescanLFSizeY - dRowTol))
	{
		m_nPrescanIndexStepRow = (LONG)(m_dPrescanLFSizeY - dRowTol);
	}

	if( m_nPrescanIndexStepRow%2==0 )
	{
		m_nPrescanIndexStepRow = m_nPrescanIndexStepRow -1;
	}

	if (m_nPrescanIndexStepRow < 1)
	{
		m_nPrescanIndexStepRow = 1;
	}

	return m_nPrescanIndexStepRow;
}

LONG CMS896AStn::GetMapIndexStepCol()
{
	DOUBLE dRowTol = 0, dColTol = 0;
	GetMapIndexTolerance(dRowTol, dColTol);
	if (m_nPrescanIndexStepCol == 0 || m_nPrescanIndexStepCol > (LONG)(m_dPrescanLFSizeX - dColTol))
	{
		m_nPrescanIndexStepCol = (LONG)(m_dPrescanLFSizeX - dColTol);
	}

	if( m_nPrescanIndexStepCol%2==0 )
	{
		m_nPrescanIndexStepCol = m_nPrescanIndexStepCol -1;
	}

	if (m_nPrescanIndexStepCol < 1)
	{
		m_nPrescanIndexStepCol = 1;
	}

	return m_nPrescanIndexStepCol;
}


BOOL CMS896AStn::IsPrescanBlkPick()	// block pick alignment only; block pick algorithm not enable.
{
	if( IsMS90HalfSortMode() )
	{
		return FALSE;
	}
	// m_bPrescanBlkPickEnabled if true, then the real block pick method enabled, otherwise, it is prescan block pick alignment only
	return IsPrescanEnable() && m_bPrescanBlkPickAlign && (m_bPrescanBlkPickEnabled == FALSE);
}


BOOL CMS896AStn::IsBlkPickAlign()
{
	return IsPrescanEnable() && m_bPrescanBlkPickAlign && (m_bPrescanBlkPickEnabled == FALSE);
}


BOOL CMS896AStn::IsCharDieInUse()
{
	if( IsOcrAOIMode() )
		return m_bHmiToUseCharDie;
	else
		return m_bHmiToUseCharDie && m_bIfEnableCharDie;
}

BOOL CMS896AStn::SaveTheWaferEndTime()
{
	//chris temp use
	CStdioFile cfDataFile;

	CString szYear, szMonth, szDay, szHour, szMinute, szSecond;
	CString szDateFormat, szText;
	CString szWaferID;
	
	BOOL bFileOpen = FALSE;

	//1. Log date into History file 
	CString szFileName = WL_WAFER_END_TIME_LOG_NAME;

	//Get current time;
	CTime theTime = CTime::GetCurrentTime();

	szYear.Format("%d",		theTime.GetYear());
	szYear = szYear.Right(2);
	szMonth.Format("%02d",	theTime.GetMonth());
	szDay.Format("%02d",	theTime.GetDay());
	szHour.Format("%02d",	theTime.GetHour());
	szMinute.Format("%02d", theTime.GetMinute());
	szSecond.Format("%02d", theTime.GetSecond());

	//Fit ouptut format
	szDateFormat = szHour + ":" + szMinute + ":" + szSecond + " " + szDay + "-" + szMonth + "-" + szYear;

	//Fit data format 
	m_WaferMapWrapper.GetWaferID(szWaferID);
	int nCol = szWaferID.Find(".");
	if (nCol != -1)
	{
		szWaferID = szWaferID.Left(nCol);
	}
/*
	bFileOpen = cfDataFile.Open(szFileName, 
								CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText);

	if (bFileOpen == TRUE)
	{
		cfDataFile.SeekToEnd();

		//Format (Date/Time, SlotNo, BarcodeName, WaferID, Bonded, Total Selected, Yield)
		szText = szDateFormat + "," + szWaferID + ", \n";

		cfDataFile.WriteString(szText);
		cfDataFile.Close();
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("open wl_history.txt error\n");
		return FALSE;
	}
*/
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	//load & get config file ptr
	pUtl->LoadAppFeatureConfig(); 
	pUtl->LoadWaferEndDataStrMapFile();
	psmf = pUtl->GetWaferEndDataStrMapFile();
	if (psmf != NULL)	//v4.46T20	//Klocwork
		(*psmf)["Wafer End Time(By Wafer ID)"][szWaferID] = szDateFormat;

	pUtl->UpdateWaferEndDataStrMapFile();
	return TRUE;
}

BOOL CMS896AStn::LogWaferEndInformation(LONG lSlot, CString szBarcodeName)
{
	CStdioFile cfDataFile;
	BOOL bFileOpen = FALSE;
	BOOL bFileExist = FALSE;
	CString szFileName = "";
	CString szYear, szMonth, szDay, szHour, szMinute, szSecond;
	CString szDateFormat, szText;
	CString szSlotNo, szStatistics, szWaferID;
	DOUBLE dYield = 0.0;
	CUIntArray aulSelectedGradeList;
	ULONG ulAccumulatedPicked = 0, ulAccumulatedTotal = 0;
	ULONG ulLeft = 0, ulPicked = 0, ulTotal = 0;

	if (IsWaferMapValid() == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Map Invalid when to log wafer end info\n");
		return FALSE;
	}

	//Get current time;
	CTime theTime = CTime::GetCurrentTime();

	szYear.Format("%d",		theTime.GetYear());
	szYear = szYear.Right(2);
	szMonth.Format("%02d",	theTime.GetMonth());
	szDay.Format("%02d",	theTime.GetDay());
	szHour.Format("%02d",	theTime.GetHour());
	szMinute.Format("%02d", theTime.GetMinute());
	szSecond.Format("%02d", theTime.GetSecond());

	//Fit ouptut format
	szDateFormat = szHour + ":" + szMinute + ":" + szSecond + "@" + szDay + "-" + szMonth + "-" + szYear;

	//Fit data format 
	m_WaferMapWrapper.GetWaferID(szWaferID);

	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
		
	for (INT i = 0; i < aulSelectedGradeList.GetSize(); i++)
	{
		m_WaferMapWrapper.GetStatistics(aulSelectedGradeList.GetAt(i), ulLeft, ulPicked, ulTotal);

		ulAccumulatedPicked += ulPicked;
		ulAccumulatedTotal += ulTotal;
	}

	if (ulAccumulatedTotal == 0)
	{
		dYield = 0.0;
	}
	else
	{
		dYield = ((DOUBLE) ulAccumulatedPicked) / ((DOUBLE) ulAccumulatedTotal) * 100;
	}

	if (lSlot != 0)
	{
		szSlotNo.Format("%2d, ", lSlot);
	}
	else
	{
		szSlotNo = ",";
	}
	
	szStatistics.Format("%5d, %5d, %3.2f", ulAccumulatedPicked, ulAccumulatedTotal, dYield);
	
	//1. Log date into History file 
	szFileName = WL_HISTORY_NAME;

	if ((_access(szFileName, 0)) == -1)
	{
		bFileExist = FALSE;
	}
	else
	{
		bFileExist = TRUE;
	}

	//Get Title & list out data
	bFileOpen = cfDataFile.Open(szFileName, 
								CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText);
	
	if (bFileOpen == TRUE)
	{
		cfDataFile.SeekToEnd();

		if (bFileExist == FALSE)
		{
			cfDataFile.WriteString(WL_HISTORY_TITLE);
			cfDataFile.WriteString("\n");
		}

		//Format (Date/Time, SlotNo, BarcodeName, WaferID, Bonded, Total Selected, Yield)
		szText = szDateFormat + ", " + szSlotNo + szBarcodeName + ", " + szWaferID + ", " + szStatistics + "\n";

		cfDataFile.WriteString(szText);
		cfDataFile.Close();
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("open wl_history.txt error\n");
		return FALSE;
	}

	return TRUE;
}

// Temperature Controller
BOOL CMS896AStn::IsLayerPicking()
{
	return m_bTCLayerPick && m_bTEInUse;	//	controlled by HMI
}

BOOL CMS896AStn::IsBHZOnVacuumOnLayer()
{
	return IsLayerPicking() && m_lTCDnOnVacDelay >= 0;
}

BOOL CMS896AStn::IsEjtUpAndPreheat()
{
	return IsLayerPicking() && m_lTCUpPreheatTime >= 0;
}

LONG CMS896AStn::DispQuestion(char *format, ...)
{
	va_list pArgs;
	char szMsg[256];
	CString str;

	va_start(pArgs, format);
	vsprintf(szMsg, format, pArgs);
	va_end(pArgs);
	str = szMsg;
	return DispQuestion(str);
}


LONG CMS896AStn::DispQuestion(CString str, CString szTitle)
{
	return HmiMessage(str, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
}

VOID CMS896AStn::DispMsg(char *szMsg, LONG lTop, LONG lLeft)
{
	HmiMessageEx(szMsg, "System Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, -1, -1, NULL, NULL, NULL, NULL, lTop, lLeft);
}

VOID CMS896AStn::DispMsg(CString szMsg, LONG lTop, LONG lLeft)
{
	HmiMessageEx(szMsg, "System Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, -1, -1, NULL, NULL, NULL, NULL, lTop, lLeft);
}

LONG CMS896AStn::SetAlert_Msg_Red_Yellow(UINT unCode, CString &szMsg, CString szBtnText1, CString szBtnText2, CString szBtnText3, LONG lAlign)
{
	CloseAlarm();
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();

	LONG lAction;
	LONG lReturn = 0;
	CString szAlarmCode;
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	szAlarmCode.LoadString(unCode);
	GetAlarmCodeAction(szAlarmCode, lAction);

	if (lAction == ALARM_REDYELLOW_EVENT)
	{
		SetAlarmLamp_RedYellow(FALSE, FALSE);
	}
	else if (lAction == ALARM_RED_BLINK_EVENT)
	{
		SetAlarmLamp_Red(TRUE, FALSE);
	}
	else if (lAction == ALARM_YELLOW_BLINK_EVENT)
	{
		SetAlarmLamp_YellowBuzzer(TRUE, FALSE);
	}
	else
	{
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	}

	CString szLog;
	szLog.Format("AlertMsg red to yellow: %d, %s", unCode, szMsg);
	SetAlarmLog(szLog);
	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szAlarmCode);
	}

	lReturn = SetAlert_Msg(unCode, szMsg, szBtnText1, szBtnText2, szBtnText3, lAlign);
	if (m_bAlarmTwice)
	{
		if (lAction == ALARM_REDYELLOW_EVENT)
		{
			SetAlarmLamp_RedYellow_Only(FALSE, FALSE);
		}
		else if (lAction == ALARM_RED_BLINK_EVENT)
		{
			SetAlarmLamp_Red_Only(TRUE, FALSE);
		}
		else if (lAction == ALARM_YELLOW_BLINK_EVENT)
		{
			SetAlarmLamp_Yellow(TRUE, FALSE);
		}
		else
		{
			SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		}

		lReturn = SetAlert_Msg(unCode, szMsg, szBtnText1, szBtnText2, szBtnText3, lAlign);
	}

	SetAlarmLamp_Yellow(FALSE, FALSE);
	LONG lAlarmFinalStatus = GetAlarmLamp_Status();

	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmFinalStatus,szAlarmCode);
	}
	slLock.Unlock();

	return lReturn;
}

LONG CMS896AStn::SetAlert_Msg_NoMaterial(UINT unCode, CString &szMsg, CString szBtnText1, CString szBtnText2, CString szBtnText3, LONG lAlign)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();

//	LONG lAction;
	LONG lReturn = 0;
	CString szAlarmCode;
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();

	szAlarmCode.LoadString(unCode);

	SetAlarmLamp_NoMaterial(TRUE, FALSE);

	CString szLog;
	szLog.Format("AlertMsg NO-MATERIAL: %d, %s", unCode, szMsg);
	SetAlarmLog(szLog);
	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szAlarmCode);
	}

	lReturn = SetAlert_Msg(unCode, szMsg, szBtnText1, szBtnText2, szBtnText3, lAlign);

	//SetAlarmLamp_Yellow(FALSE, FALSE);
	SetAlarmLamp_Back(lAlarmOrgStatus, FALSE, FALSE);
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmOrgStatus,szAlarmCode);
	}
	slLock.Unlock();
	return lReturn;
}

LONG CMS896AStn::SetAlert_Msg_Red_Back(UINT unCode, CString &szMsg, CString szBtnText1, CString szBtnText2, CString szBtnText3, LONG lAlign)
{
	CloseAlarm();
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();

	LONG lAction;
	LONG lReturn = 0;
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	CString szAlarmCode;

	szAlarmCode.LoadString(unCode);
	GetAlarmCodeAction(szAlarmCode, lAction);

	if (lAction == ALARM_REDYELLOW_EVENT)
	{
		SetAlarmLamp_RedYellow(FALSE, FALSE);
	}
	else if (lAction == ALARM_RED_BLINK_EVENT)
	{
		SetAlarmLamp_Red(TRUE, FALSE);
	}
	else if (lAction == ALARM_YELLOW_BLINK_EVENT)
	{
		SetAlarmLamp_YellowBuzzer(TRUE, FALSE);
	}
	else
	{
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	}

	//SetAlarmLamp_Red(FALSE);
	CString szLog;
	szLog.Format("AlertMsg red to back: %d, %s", unCode, szMsg);
	SetAlarmLog(szLog);
	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szAlarmCode);
	}


	lReturn = SetAlert_Msg(unCode, szMsg, szBtnText1, szBtnText2, szBtnText3, lAlign);

	if (m_bAlarmTwice)
	{
		if (lAction == ALARM_REDYELLOW_EVENT)
		{
			SetAlarmLamp_RedYellow_Only(FALSE, FALSE);
		}
		else if (lAction == ALARM_RED_BLINK_EVENT)
		{
			SetAlarmLamp_Red_Only(TRUE, FALSE);
		}
		else if (lAction == ALARM_YELLOW_BLINK_EVENT)
		{
			SetAlarmLamp_Yellow(TRUE, FALSE);
		}
		else
		{
			SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		}

		lReturn = SetAlert_Msg(unCode, szMsg, szBtnText1, szBtnText2, szBtnText3, lAlign);
	}

	SetAlarmLamp_Back(lAlarmOrgStatus, FALSE, FALSE);

	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmOrgStatus,szAlarmCode);
	}

	slLock.Unlock();
	return lReturn;
}

BOOL CMS896AStn::SetAlert_Red_Yellow(UINT unCode)
{
	CloseAlarm();
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();
	CTime BeginTime = BeginTime.GetCurrentTime();
	LONG lAction;
	BOOL bReturn = TRUE;
	CString szAlarmCode;
	
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	
	szAlarmCode.LoadString(unCode);
	GetAlarmCodeAction(szAlarmCode, lAction);

	if (lAction == ALARM_REDYELLOW_EVENT)
	{
		SetAlarmLamp_RedYellow(FALSE, FALSE);
	}
	else if (lAction == ALARM_RED_BLINK_EVENT)
	{
		SetAlarmLamp_Red(TRUE, FALSE);
	}
	else if (lAction == ALARM_YELLOW_BLINK_EVENT)
	{
		SetAlarmLamp_YellowBuzzer(TRUE, FALSE);
	}
	else
	{
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	}

	CString szLog;
	szLog.Format("Alert red to yellow: %d", unCode);
	SetAlarmLog(szLog);
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//v4.40T12	//v4.43T10
	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szAlarmCode);
	}

	bReturn = SetAlert(unCode);
	
	if (m_bAlarmTwice)
	{
		if (lAction == ALARM_REDYELLOW_EVENT)
		{
			SetAlarmLamp_RedYellow_Only(FALSE, FALSE);
		}
		else if (lAction == ALARM_RED_BLINK_EVENT)
		{
			SetAlarmLamp_Red_Only(TRUE, FALSE);
		}
		else if (lAction == ALARM_YELLOW_BLINK_EVENT)
		{
			SetAlarmLamp_Yellow(TRUE, FALSE);
		}
		else
		{
			SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		}

		bReturn = SetAlert(unCode);
	}

	if( pAppMod->GetCustomerName()==CTM_SANAN && pAppMod->GetProductLine()=="XA" )
	{
		SetAlarmLamp_Back(lAlarmOrgStatus, FALSE, FALSE);
	}
	else
	{
		SetAlarmLamp_Yellow(FALSE, FALSE);
	}
	LONG lAlarmFinalStatus = GetAlarmLamp_Status();
	
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmFinalStatus,szAlarmCode);
	}
	slLock.Unlock();

	return bReturn;
}

BOOL CMS896AStn::SetAlert_NoMaterial(UINT unCode)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();
	CTime BeginTime = BeginTime.GetCurrentTime();
//	LONG lAction;
	BOOL bReturn = TRUE;
	CString szAlarmCode;
	
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	
	szAlarmCode.LoadString(unCode);

		
	SetAlarmLamp_NoMaterial(TRUE, FALSE);

	CString szLog;
	szLog.Format("Alert NO-MATERIAL: %d", unCode);
	SetAlarmLog(szLog);
	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szAlarmCode);
	}

	bReturn = SetAlert(unCode);
	

	//SetAlarmLamp_Yellow(FALSE, FALSE);
	SetAlarmLamp_Back(lAlarmOrgStatus, FALSE, FALSE);
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmOrgStatus,szAlarmCode);
	}
	slLock.Unlock();
	return bReturn;
}

BOOL CMS896AStn::SetAlert_SE_Red_Yellow(UINT unCode)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);

	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();

	CTime BeginTime = BeginTime.GetCurrentTime();
	LONG lAction;
	BOOL bReturn = TRUE;
	CString szAlarmCode;
	
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	
	szAlarmCode.LoadString(unCode);
	GetAlarmCodeAction(szAlarmCode, lAction);

	if (lAction == ALARM_REDYELLOW_EVENT)
	{
		SetAlarmLamp_RedYellow(FALSE, FALSE);
	}
	else if (lAction == ALARM_RED_BLINK_EVENT)
	{
		SetAlarmLamp_Red(TRUE, FALSE);
	}
	else if (lAction == ALARM_YELLOW_BLINK_EVENT)
	{
		SetAlarmLamp_YellowBuzzer(TRUE, FALSE);
	}
	else
	{
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	}

	CString szLog;
	szLog.Format("Alert red to yellow: %d", unCode);
	SetAlarmLog(szLog);
	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szAlarmCode);
	}

	bReturn = SetAlert(unCode);

	BOOL bCheckPassword = FALSE;
	if( pAppMod->GetCustomerName()==CTM_SANAN && pAppMod->GetProductLine()=="XA" )
	{
		bCheckPassword = TRUE;
	}

	if (bCheckPassword || m_bAlarmTwice)
	{
		if (lAction == ALARM_REDYELLOW_EVENT)
		{
			SetAlarmLamp_RedYellow_Only(FALSE, FALSE);
		}
		else if (lAction == ALARM_RED_BLINK_EVENT)
		{
			SetAlarmLamp_Red_Only(TRUE, FALSE);
		}
		else if (lAction == ALARM_YELLOW_BLINK_EVENT)
		{
			SetAlarmLamp_Yellow(TRUE, FALSE);
		}
		else
		{
			SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		}
	}

	if (m_bAlarmTwice && bCheckPassword==FALSE)
	{
		bReturn = SetAlert(unCode);
	}

	while( bCheckPassword )
	{
		IPC_CServiceMessage stMsg;
		LONG lAccessLevel = MS_ENGINEER_ACCESS_LEVEL;
		stMsg.InitMessage(sizeof(LONG), &lAccessLevel);
		INT nConvID = m_comClient.SendRequest("MapSorter", _T("CheckAccessRight"), stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
			{	
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		BOOL bResult = FALSE;
		stMsg.GetMsg(sizeof(BOOL), &bResult);
		if( bResult )
		{
			break;
		}

		bReturn = SetAlert(unCode);
	}
	
	if( pAppMod->GetCustomerName()==CTM_SANAN && pAppMod->GetProductLine()=="XA" )
	{
		SetAlarmLamp_Back(lAlarmOrgStatus, FALSE, FALSE);
	}
	else
	{
		SetAlarmLamp_Yellow(FALSE, FALSE);
	}
	LONG lAlarmFinalStatus = GetAlarmLamp_Status();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmFinalStatus,szAlarmCode);
	}
	slLock.Unlock();

	return bReturn;
}	//	alert and request password.

BOOL CMS896AStn::SetAlert_Yellow_Green(UINT unCode)  //4.51D8
{
	CloseAlarm();
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();
	CTime BeginTime = BeginTime.GetCurrentTime();
	LONG lAction;
	BOOL bReturn = TRUE;
	CString szAlarmCode;
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	szAlarmCode.LoadString(unCode);
	GetAlarmCodeAction(szAlarmCode, lAction);

	if (lAction == ALARM_REDYELLOW_EVENT)
	{
		SetAlarmLamp_RedYellow(FALSE, FALSE);
	}
	else if (lAction == ALARM_RED_BLINK_EVENT)
	{
		SetAlarmLamp_Red(TRUE, FALSE);
	}
	else if (lAction == ALARM_YELLOW_BLINK_EVENT)
	{
		SetAlarmLamp_YellowBuzzer(TRUE, FALSE);
	}
	else if (lAction == ALARM_YELLOWGREEN_EVENT)
	{
		SetAlarmLamp_YellowGreen(TRUE, FALSE);
	}
	else
	{
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	}
	
	CString szLog;
	szLog.Format("Alert yellow to green uncode: %d, szAlarmCode:%s, lAction: %d", unCode , szAlarmCode, lAction);
	SetAlarmLog(szLog);
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//v4.40T12	//v4.43T10

	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szAlarmCode);
	}
	bReturn = SetAlert(unCode);
	
	if (m_bAlarmTwice)
	{
		if (lAction == ALARM_REDYELLOW_EVENT)
		{
			SetAlarmLamp_RedYellow_Only(FALSE, FALSE);
		}
		else if (lAction == ALARM_RED_BLINK_EVENT)
		{
			SetAlarmLamp_Red_Only(TRUE, FALSE);
		}
		else if (lAction == ALARM_YELLOW_BLINK_EVENT)
		{
			SetAlarmLamp_Yellow(TRUE, FALSE);
		}
		else if (lAction == ALARM_YELLOWGREEN_EVENT)
		{
			SetAlarmLamp_YellowGreen(TRUE, FALSE);
		}
		else
		{
			SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		}

		bReturn = SetAlert(unCode);
	}
	
	//SetAlarmLamp_YellowGreen(TRUE, TRUE); //4.51D8
	SetAlarmLamp_Yellow(FALSE, FALSE);
	LONG lAlarmFinalStatus = GetAlarmLamp_Status();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmFinalStatus,szAlarmCode);
	}
	slLock.Unlock();

	return bReturn;
}

BOOL CMS896AStn::SetAlert_Red_Back(UINT unCode)
{
	CloseAlarm();
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();
	CTime BeginTime = BeginTime.GetCurrentTime();
	LONG lAction;
	BOOL bReturn = TRUE;
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	CString szAlarmCode;

	szAlarmCode.LoadString(unCode);
	GetAlarmCodeAction(szAlarmCode, lAction);

	if (lAction == ALARM_REDYELLOW_EVENT)
	{
		SetAlarmLamp_RedYellow(FALSE, FALSE);
	}
	else if (lAction == ALARM_RED_BLINK_EVENT)
	{
		SetAlarmLamp_Red(TRUE, FALSE);
	}
	else if (lAction == ALARM_YELLOW_BLINK_EVENT)
	{
		SetAlarmLamp_YellowBuzzer(TRUE, FALSE);
	}
	else
	{
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	}

	CString szLog;
	szLog.Format("Alert red to back: %d", unCode);
	SetAlarmLog(szLog);
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//v4.40T12	//v4.43T10
	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szAlarmCode);
	}
	bReturn = SetAlert(unCode);
	if (m_bAlarmTwice)
	{
		if (lAction == ALARM_REDYELLOW_EVENT)
		{
			SetAlarmLamp_RedYellow_Only(FALSE, FALSE);
		}
		else if (lAction == ALARM_RED_BLINK_EVENT)
		{
			SetAlarmLamp_Red_Only(TRUE, FALSE);
		}
		else if (lAction == ALARM_YELLOW_BLINK_EVENT)
		{
			SetAlarmLamp_Yellow(TRUE, FALSE);
		}
		else
		{
			SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		}

		bReturn = SetAlert(unCode);
	}

	SetAlarmLamp_Back(lAlarmOrgStatus, FALSE, FALSE);
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		CString szUnCode;
		szUnCode.Format("%d",unCode);
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmOrgStatus,szAlarmCode);
	}
	slLock.Unlock();
	return bReturn;
}

LONG CMS896AStn::HmiMessage_Red_Yellow(const CString &szText,
									   const CString &szTitle,
									   LONG lType,
									   LONG lAlignment,
									   LONG lTimeout,
									   LONG lMsgMode,
									   LONG *plMsgID,
									   LONG lWidth,
									   LONG lHeight,
									   CString *pszBitmapFile,
									   CString *pszCustomText1,
									   CString *pszCustomText2,
									   CString *pszCustomText3,
									   LONG lTop,
									   LONG lLeft)
{
	CloseAlarm();
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	CTime BeginTime = BeginTime.GetCurrentTime();
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();

	LONG lReturn = 0;

	//Set Assistance time
	SetAlertTime(TRUE, EQUIP_ASSIST_TIME, szTitle.IsEmpty() ? "Machine Assist" : szTitle);

	SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	EquipStateProductiveToUnscheduleDown();						//v4.50A12

	CString szLog;
	szLog.Format("HmiMsg red to yellow: %s", szText);
	SetAlarmLog(szLog);
	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szText);
	}

	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szText);	//v4.43T10

	lReturn = HmiMessage(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgID,
						 lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	if (m_bAlarmTwice)
	{
		SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		lReturn = HmiMessage(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgID,
							 lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	}
	
	EquipStateUnscheduleDwonToProductive();						//v4.50A12
	SetAlarmLamp_Yellow(FALSE, FALSE);
	CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szText);		//anichia003
	LONG lAlarmFinalStatus = GetAlarmLamp_Status();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmFinalStatus,szText);
	}

	//Clear Assistance time
	SetAlertTime(FALSE, EQUIP_ASSIST_TIME, szTitle.IsEmpty() ? "Machine Assist" : szTitle);
	slLock.Unlock();

	return lReturn;
}

LONG CMS896AStn::HmiMessage_Red_Green(const CString &szText,
									  const CString &szTitle,
									  LONG lType,
									  LONG lAlignment,
									  LONG lTimeout,
									  LONG lMsgMode,
									  LONG *plMsgID,
									  LONG lWidth,
									  LONG lHeight,
									  CString *pszBitmapFile,
									  CString *pszCustomText1,
									  CString *pszCustomText2,
									  CString *pszCustomText3,
									  LONG lTop,
									  LONG lLeft)
{
	CloseAlarm();
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();
	CTime BeginTime = BeginTime.GetCurrentTime();
	LONG lReturn = 0;

	//Set Assistance time
	SetAlertTime(TRUE, EQUIP_ASSIST_TIME, szTitle.IsEmpty() ? "Machine Assist" : szTitle);

	SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	EquipStateProductiveToUnscheduleDown();						//v4.50A12
	CString szLog;
	szLog.Format("HmiMsg red to green: %s", szText);
	SetAlarmLog(szLog);

	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szText);
	}

	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szText);			//v4.43T10
	CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szText);		//anichia004			

	lReturn = HmiMessage(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgID,
						 lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	if (m_bAlarmTwice)
	{
		SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		lReturn = HmiMessage(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgID,
							 lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	}

	EquipStateUnscheduleDwonToProductive();						//v4.50A12
	SetAlarmLamp_Green(FALSE, FALSE);
	LONG lAlarmFinalStatus = GetAlarmLamp_Status();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmFinalStatus,szText);
	}
	//Clear Assistance time
	SetAlertTime(FALSE, EQUIP_ASSIST_TIME, szTitle.IsEmpty() ? "Machine Assist" : szTitle);

	slLock.Unlock();

	return lReturn;
}

LONG CMS896AStn::HmiMessage_Red_Back(const CString &szText,
									 const CString &szTitle,
									 LONG lType,
									 LONG lAlignment,
									 LONG lTimeout,
									 LONG lMsgMode,
									 LONG *plMsgID,
									 LONG lWidth,
									 LONG lHeight,
									 CString *pszBitmapFile,
									 CString *pszCustomText1,
									 CString *pszCustomText2,
									 CString *pszCustomText3,
									 LONG lTop,
									 LONG lLeft)
{
	CloseAlarm();
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();
	CTime BeginTime = BeginTime.GetCurrentTime();
	LONG lReturn = 0;
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();

	SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	//Set Assistance time
	SetAlertTime(TRUE, EQUIP_ALARM_TIME, szTitle.IsEmpty() ? "Machine Alarm" : szTitle);
	//Error handling
	EquipStateProductiveToUnscheduleDown();

	CString szLog;
	szLog.Format("HmiMsg red to back: %s", szText);
	SetAlarmLog(szLog);
	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szText);
	}

	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szText);			//v4.43T10
	CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szText);		//anichia003			

	lReturn = HmiMessage(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgID,
						 lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	if (m_bAlarmTwice)
	{
		SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		lReturn = HmiMessage(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgID,
							 lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	}

	SetAlarmLamp_Back(lAlarmOrgStatus, FALSE, FALSE);

	EquipStateUnscheduleDwonToProductive();

	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmOrgStatus,szText);
	}
	//Clear Alarm time
	SetAlertTime(FALSE, EQUIP_ALARM_TIME, szTitle.IsEmpty() ? "Machine Alarm" : szTitle);

	slLock.Unlock();

	return lReturn;
}

LONG CMS896AStn::HmiMessageEx_Timeout_Red_Back(const CString &szText,
		const CString &szTitle,
		LONG lType,
		LONG lAlignment,
		LONG lTimeout,
		LONG lMsgMode,
		LONG *plMsgID,
		LONG lWidth,
		LONG lHeight,
		CString *pszBitmapFile,
		CString *pszCustomText1,
		CString *pszCustomText2,
		CString *pszCustomText3,
		LONG lTop,
		LONG lLeft)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();

	LONG lReturn = 0;
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();

	SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	//Set Assistance time
	SetAlertTime(TRUE, EQUIP_ALARM_TIME, szTitle.IsEmpty() ? "Machine Alarm" : szTitle);
	//Error handling
	EquipStateProductiveToUnscheduleDown();

	CString szLog;
	szLog.Format("HmiMsgEx red to timeout: %s", szText);
	SetAlarmLog(szLog);

	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szText);
	}

	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szText);			//v4.43T10
	CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szText);		//anichia004			

	lReturn = HmiMessageEx(szText, szTitle, lType, lAlignment, 60000, lMsgMode, plMsgID,
						   lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	if (lReturn == glHMI_TIMEOUT)
	{
		SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		lReturn = HmiMessageEx(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgID,
							   lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	}

	SetAlarmLamp_Back(lAlarmOrgStatus, FALSE, FALSE);
	EquipStateUnscheduleDwonToProductive();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmOrgStatus,szText);
	}
	//Clear Alarm time
	SetAlertTime(FALSE, EQUIP_ALARM_TIME, szTitle.IsEmpty() ? "Machine Alarm" : szTitle);

	slLock.Unlock();
	return lReturn;
}

LONG CMS896AStn::HmiMessageEx_Red_Back(const CString &szText,
									   const CString &szTitle,
									   LONG lType,
									   LONG lAlignment,
									   LONG lTimeout,
									   LONG lMsgMode,
									   LONG *plMsgId,
									   LONG lWidth,
									   LONG lHeight,
									   CString *pszBitmapFile,
									   CString *pszCustomText1,
									   CString *pszCustomText2,
									   CString *pszCustomText3,
									   LONG lTop,
									   LONG lLeft)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();

	LONG lReturn = 0;
	LONG lAlarmOrgStatus = GetAlarmLamp_Status();

	SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	//Set Assistance time
	SetAlertTime(TRUE, EQUIP_ALARM_TIME, szTitle.IsEmpty() ? "Machine Alarm" : szTitle);
	//Error handling
	EquipStateProductiveToUnscheduleDown();


	CString szLog;
	szLog.Format("HmiMsgEx red to back: %s", szText);
	SetAlarmLog(szLog);

	LONG lAlarmMidStatus = GetAlarmLamp_Status();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmOrgStatus, lAlarmMidStatus,szText);
	}

	//SendAlarm(szText, ALM_FORCE_LOG, 3000, gszALM_CLASS_ERROR); // 4.53D26 add log into Hmi

	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szText);			//v4.43T10
	CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szText);		//anichia004			

	lReturn = HmiMessageEx(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgId,
						   lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	if (m_bAlarmTwice)
	{
		SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		lReturn = HmiMessageEx(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgId,
							   lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	}

	SetAlarmLamp_Back(lAlarmOrgStatus, FALSE, FALSE);
	EquipStateUnscheduleDwonToProductive();

	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Song add for changelight
		AlarmLampStatusChangeLog(lAlarmMidStatus, lAlarmOrgStatus,szText);
	}

	//Clear Alarm time
	SetAlertTime(FALSE, EQUIP_ALARM_TIME, szTitle.IsEmpty() ? "Machine Alarm" : szTitle);

	slLock.Unlock();

	return lReturn;
}

int CMS896AStn::AfxMessageBox_Red_Back(LPCTSTR lpszText, UINT nType, UINT nIDHelp)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();

	LONG lAlarmStatus = GetAlarmLamp_Status();
	int nReturn = 0;

	SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	//Set Assistance time
	SetAlertTime(TRUE, EQUIP_ALARM_TIME, "Machine Alarm");
	//Error handling
	EquipStateProductiveToUnscheduleDown();

	CString szLog;
	szLog.Format("AfxMsg red to back: %s", lpszText);
	SetAlarmLog(szLog);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(lpszText);		//v4.43T10

	nReturn = AfxMessageBox(lpszText, nType, nIDHelp);

	if (m_bAlarmTwice)
	{
		SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		nReturn = AfxMessageBox(lpszText, nType, nIDHelp);
	}

	SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);
	EquipStateUnscheduleDwonToProductive();
	//Clear Alarm time
	SetAlertTime(FALSE, EQUIP_ALARM_TIME, "Machine Alarm");

	slLock.Unlock();

	return nReturn;
}


LONG CMS896AStn::HmiMessage_Red_Red(const CString &szText,
									const CString &szTitle,
									LONG lType,
									LONG lAlignment,
									LONG lTimeout,
									LONG lMsgMode,
									LONG *plMsgID,
									LONG lWidth,
									LONG lHeight,
									CString *pszBitmapFile,
									CString *pszCustomText1,
									CString *pszCustomText2,
									CString *pszCustomText3,
									LONG lTop,
									LONG lLeft)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
	slLock.Lock();
	CTime BeginTime = BeginTime.GetCurrentTime();
	LONG lReturn = 0;

	SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
	//Set Assistance time
	SetAlertTime(TRUE, EQUIP_ALARM_TIME, szTitle.IsEmpty() ? "Machine Alarm" : szTitle);
	//Error handling
	EquipStateProductiveToUnscheduleDown();

	CString szLog;
	szLog.Format("HmiMsg red to timeout: %s", szText);
	SetAlarmLog(szLog);
	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szText);			//v4.43T10
	CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(FALSE, szText);		//anichia003			

	lReturn = HmiMessage(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgID,
						 lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	if (m_bAlarmTwice)
	{
		SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
		lReturn = HmiMessage(szText, szTitle, lType, lAlignment, lTimeout, lMsgMode, plMsgID,
							 lWidth, lHeight, pszBitmapFile, pszCustomText1, pszCustomText2, pszCustomText3, lTop, lLeft);
	}

	EquipStateUnscheduleDwonToProductive();
	//Clear Alarm time
	SetAlertTime(FALSE, EQUIP_ALARM_TIME, szTitle.IsEmpty() ? "Machine Alarm" : szTitle);

	slLock.Unlock();
	return lReturn;
}


BOOL CMS896AStn::UpdateHmiVariableData(CString szVariableName, CString szValue)
{
	INT nIndex = 0;
	IPC_CServiceMessage svMsg;
	unsigned char acBuffer[200];

	memcpy(acBuffer, (LPCTSTR)szVariableName, szVariableName.GetLength() + 1);
	nIndex += szVariableName.GetLength() + 1;
	memcpy(&acBuffer[nIndex], (LPCTSTR)szValue, szValue.GetLength() + 1);
	nIndex += szValue.GetLength() + 1;
	svMsg.InitMessage(nIndex, acBuffer);
	m_comClient.SendRequest("HmiUserService", "HmiUpdateData", svMsg, 0);

	return TRUE;
}


BOOL CMS896AStn::CheckOperatorPassWord(CString szPassword)
{
	BOOL bEnable = FALSE;
/*
	CHAR acPar[200], *pTemp;
	ULONG ulSize;
	IPC_CServiceMessage stMsg;

	strcpy_s(acPar, sizeof(acPar), "Please Enter YOUR Password");
	ulSize = strlen(acPar) + 1;
	pTemp = acPar + ulSize;
	strcpy_s(pTemp, sizeof(acPar) - ulSize, szPassword);
	ulSize += szPassword.GetLength() + 1;

	stMsg.InitMessage(ulSize, acPar);
	INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiPassword", stMsg);

	// Get the reply
	while ( m_comClient.ScanReplyForConvID(nConvID, 500) == 0);

	m_comClient.ReadReplyForConvID(nConvID, stMsg);
	stMsg.GetMsg(sizeof(BOOL), &bEnable);
	Sleep(2000);
*/
	//v4.37T12	//Genesis MS100
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->UpdateHmiVariableData("szLoginID",		"");
	pApp->UpdateHmiVariableData("szLoginPassword",	"");

	CString szTitle			= _T("User Logon");
	CString szInputUserID	= _T("");

	szTitle = _T("Please Enter YOUR Password");
	bEnable = pApp->HmiStrInputKeyboard(szTitle, "szLoginID", TRUE, szInputUserID);

	if (szInputUserID == szPassword)
		bEnable = TRUE;
	else
		bEnable = FALSE;

	return bEnable;
}

BOOL CMS896AStn::Check201277PasswordToGo()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	int nPass = (int) pApp->GetProfileInt(gszPROFILE_SETTING, _T("SemitekPassword"), 201277);
	CString szPass;
	szPass.Format("%d", nPass);
	return CheckOperatorPassWord(szPass);
}

BOOL CMS896AStn::HmiStrInputKeyboard(CString szTitle, CString &szContent)
{
	BOOL bResult	= TRUE;
	BOOL bReturn	= TRUE;
	BOOL bMask		= FALSE;
	int nIndex		= 0;

	CString szOutput = _T("");
	char *pBuffer;	// = new char[rReqMsg2.GetMsgLen()];

	IPC_CServiceMessage rReqMsg;
	CHAR acPar[200];

	CString szTemp = "szWaferIdInput";		//This STRING HMI var has to be defined in HMI data list
	strcpy_s(acPar, sizeof(acPar), (LPCTSTR) szTemp);
	nIndex += szTemp.GetLength() + 1;

	//Append input box title
	szTemp = szTitle;
	strcpy_s(&acPar[nIndex], sizeof(acPar) - nIndex, (LPCTSTR) szTemp);
	nIndex += szTemp.GetLength() + 1;

	memcpy(&acPar[nIndex], &bMask, sizeof(BOOL));
	nIndex += sizeof(BOOL);
	memcpy(&acPar[nIndex], &bReturn, sizeof(BOOL));
	nIndex += sizeof(BOOL);

	CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);			//nichia001

	rReqMsg.InitMessage(nIndex, acPar);				
	INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiAlphaKeys", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
	
			pBuffer = new char[rReqMsg.GetMsgLen()];
			rReqMsg.GetMsg(rReqMsg.GetMsgLen(), pBuffer);
			memcpy(&bResult, pBuffer, sizeof(BOOL));
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	//CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE);		//anichia001

	if (bResult == TRUE)
	{
		//Get input string
		szContent = &pBuffer[sizeof(BOOL)]; 
	}

	delete[] pBuffer;
	return bResult;
}


VOID CMS896AStn::LaserTestOuputBitTrigger(BOOL bSet, DWORD dDelay)
{
	CSingleLock slLock(&m_csLastTestLock);
	slLock.Lock();

	if (CMS896AApp::m_bEnableLaserTestOutputPortTrigger == FALSE)
	{
		return;
	}

	MotionSetOutputBit(MS_LASERTEST_TRIGGER_BIT, bSet);

	if (dDelay > 0)
	{
		Sleep(dDelay);
	}

	slLock.Unlock();
}

VOID CMS896AStn::SetSuckingHead(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
	if (m_bDisableBH)
	{
		return;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->m_bEnableBHSuckingHead)
	{
		return;
	}

	if (bSet)
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("Sucking Head is ON");
	}
	else
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("Sucking Head is Off");	//v4.42T15
	}

	MotionSetOutputBit(BH_SO_SUCKING_HEAD, bSet);
}

BOOL CMS896AStn::PrintScreen(CString szScreenFile)
{
	HDC hdcSrc = GetDC(NULL);
	if (hdcSrc == NULL)		//v4.04	//Klocwork
	{
		return FALSE;
	}
	int nBitPerPixel = GetDeviceCaps(hdcSrc, BITSPIXEL);
	int nWidth = GetDeviceCaps(hdcSrc, HORZRES);
	int nHeight = GetDeviceCaps(hdcSrc, VERTRES);
	CImage image;
	image.Create(nWidth, nHeight, nBitPerPixel);
	BitBlt(image.GetDC(), 0, 0, nWidth, nHeight, hdcSrc, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hdcSrc);
	image.ReleaseDC();
	image.Save(szScreenFile, Gdiplus::ImageFormatJPEG);

	return TRUE;
}

BOOL CMS896AStn::PrintPartScreen(CString szImageFile, LONG lULX, LONG lULY, LONG lLRX, LONG lLRY)
{
	HDC hdcSrc = GetDC(NULL);
	if (hdcSrc == NULL)		//v4.04	//Klocwork
	{
		return FALSE;
	}
	int nBitPerPixel = GetDeviceCaps(hdcSrc, BITSPIXEL);
	int nWidth = lLRX - lULX;
	int nHeight = lLRY - lULY;

	CImage image;
	image.Create(nWidth, nHeight, nBitPerPixel);
	BitBlt(image.GetDC(), 0, 0, nWidth, nHeight, hdcSrc, lULX, lULY, SRCCOPY);
	ReleaseDC(NULL, hdcSrc);
	image.ReleaseDC();
	image.Save(szImageFile, Gdiplus::ImageFormatJPEG);

	return TRUE;
}

BOOL CMS896AStn::IsBLBCRepeatCheckFail()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->GetCustomerName() != CTM_OSRAM)
	{
		return FALSE;
	}

	return m_bBinFrameCheckBCFail;
}

BOOL CMS896AStn::IsScnLoaded()
{
	if( IsPrescanEnable()==FALSE && m_bEnableSCNFile )
		return m_bScnLoaded;
	else
		return FALSE;
}



BOOL CMS896AStn::IsWLManualMode()
{
	return m_ulSequenceMode == WL_MANUAL_MODE;
}

BOOL CMS896AStn::IsWLSemiAutoMode()
{
	return m_ulSequenceMode == WL_SEMI_AUTO_MODE;
}

BOOL CMS896AStn::IsWLAutoMode()
{
	return m_ulSequenceMode == WL_AUTO_MODE;
}

// sort mode
BOOL CMS896AStn::IsSortTo2ndPart()
{
	return Is2PhasesSortMode() && m_bSortGoingTo2ndPart;
}

BOOL CMS896AStn::IsMS90Sorting2ndPart()
{
	return IsMS90HalfSortMode() && m_b2Parts1stPartDone;
}

ULONG CMS896AStn::GetMS90HalfMapMaxRow()
{
	return m_ulMS90HalfBorderMapRow;
}

ULONG CMS896AStn::GetMS90HalfMapMaxCol()
{
	return m_ulMS90HalfBorderMapCol;
}

BOOL CMS896AStn::IsOutMS90SortingPart(ULONG ulRow, ULONG ulCol)
{
	if (IsMS90HalfSortMode())
	{
		if (IsRowModeSeparateHalfMap())
		{
			if (ulRow >= GetMS90HalfMapMaxRow())
			{
				return TRUE;
			}
		}
		else
		{
			if (ulCol < GetMS90HalfMapMaxCol())
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CMS896AStn::IsAllSorted()
{
	if( Is2PhasesSortMode() )
	{
		return m_b2PartsAllDone;
	}

	return TRUE;
}

BOOL CMS896AStn::IsSorraSortMode()
{
	if (IsBurnIn())
	{
		return FALSE;
	}
	return m_bSoraaSortMode;
}

BOOL CMS896AStn::IsRowModeSeparateHalfMap()
{
	return FALSE;
}

BOOL CMS896AStn::IsMS90HalfSortMode()
{
	return m_bMS90HalfSortMode;
}

BOOL CMS896AStn::RotateBinTable180(LONG lOldX, LONG lOldY, LONG &lNewX, LONG &lNewY)
{
	lNewX = lOldX;
	lNewY = lOldY;

	if( IsMS90() && (m_lBinCalibX != 0) && (m_lBinCalibY != 0) )
	{
		lNewX = 2 * m_lBinCalibX - lOldX;
		lNewY = 2 * m_lBinCalibY - lOldY;
	}

	return TRUE;
}

BOOL CMS896AStn::IsMS90BTRotated()
{
	return IsMS90() && m_bMS90RotatedBT;
}	//	MS90 table rotated

BOOL CMS896AStn::Is2PhasesSortMode()
{
	return IsMS90HalfSortMode() || IsSorraSortMode();
}

BOOL CMS896AStn::IsOsramResortMode()
{
	if (IsBurnIn())
	{
		return FALSE;
	}
	return m_bOsramResortMode;
}

VOID CMS896AStn::DWTDSortLog(CString szMsg)
{
	m_CSect.Lock();

	if (CMSLogFileUtility::Instance()->GetEnableMachineLog() && Is2PhasesSortMode())	// log file
	{
		CString szLogFile;

		CTime theTime = CTime::GetCurrentTime();
		szLogFile = "C:\\MapSorter\\UserData\\History\\TwoPhasesSort.txt";

		FILE *pPScanLog = NULL;
		errno_t nErr = fopen_s(&pPScanLog, szLogFile, "a+");
		if ((nErr == 0) && (pPScanLog != NULL))
		{
			fprintf(pPScanLog, "%2d (%2d:%2d:%2d) - %s\n", theTime.GetDay(),
					theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LPCTSTR) szMsg);  
			fclose(pPScanLog);
		}
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	}

	m_CSect.Unlock();
}
// sort mode

BOOL CMS896AStn::IsWithinMapLimit(LONG lRow, LONG lCol, LONG lMinRow, LONG lMinCol, LONG lMaxRow, LONG lMaxCol, DOUBLE dRatio)
{
	if ( m_ucWaferLimitType == WT_CIRCLE_LIMIT )
	{
		DOUBLE dRowSpan	= fabs((double)(lMaxRow - lMinRow));
		DOUBLE dColSpan	= labs(lMaxCol - lMinCol);
		DOUBLE dCtrRow	= (lMaxRow + lMinRow)/2;
		DOUBLE dCtrCol	= (lMaxCol + lMinCol)/2;
		DOUBLE dRowDist	= fabs((double)(lRow - dCtrRow));
		DOUBLE dColDist	= fabs((double)(lCol - dCtrCol));
		DOUBLE dDist	= sqrt(dRowDist*dRowDist + (dColDist*dRowSpan/dColSpan)*(dColDist*dRowSpan/dColSpan));
		DOUBLE dLimit	= sqrt(dRowSpan*dRowSpan/4 + dRowSpan*dRowSpan/4)*dRatio;
		if( dDist < dLimit )
		{
			DOUBLE dRowLow	= dCtrRow - dRowSpan/2*dRatio;
			DOUBLE dRowHigh = dCtrRow + dRowSpan/2*dRatio;
			DOUBLE dColLow	= dCtrCol - dColSpan/2*dRatio;
			DOUBLE dColHigh = dCtrCol + dColSpan/2*dRatio;
			DOUBLE dRow = lRow, dCol = lCol;
			if( dRow>=dRowLow && dRow<dRowHigh &&
				dCol>=dColLow && dCol<dColHigh )
			{
				return TRUE;
			}
		}
	}
	else
	{
		if( lRow>=lMinRow && lRow<lMaxRow && lCol>=lMinCol && lCol<lMaxCol )
		{
			return TRUE;
		}
	}

	return FALSE;
}

LONG	CMS896AStn::GetMapValidMinRow()
{
	return m_lMapValidMinRow;
}

LONG	CMS896AStn::GetMapValidMinCol()
{
	return m_lMapValidMinCol;
}

LONG	CMS896AStn::GetMapValidMaxRow()
{
	return m_lMapValidMaxRow;
}

LONG	CMS896AStn::GetMapValidMaxCol()
{
	return m_lMapValidMaxCol;
}

VOID CMS896AStn::GetMapValidSize(ULONG &ulMapValidMaxRow, ULONG &ulMapValidMinRow,
							   ULONG &ulMapValidMaxCol, ULONG &ulMapValidMinCol)
{
	ulMapValidMinRow	= GetMapValidMinRow();
	ulMapValidMaxRow	= GetMapValidMaxRow();
	ulMapValidMinCol	= GetMapValidMinCol();
	ulMapValidMaxCol	= GetMapValidMaxCol();
	if (IsMS90HalfSortMode())
	{
		if (IsRowModeSeparateHalfMap())
		{
			ulMapValidMaxRow	= GetMS90HalfMapMaxRow();
		}
		else
		{
			ulMapValidMinCol	= GetMS90HalfMapMaxCol();
		}
	}
}

VOID CMS896AStn::GetMapValidSize(LONG &lMaxRow, LONG &lMinRow,
								 LONG &lMaxCol, LONG &lMinCol)
{
	lMinRow	= GetMapValidMinRow();
	lMaxRow	= GetMapValidMaxRow();
	lMinCol	= GetMapValidMinCol();
	lMaxCol	= GetMapValidMaxCol();
	if (IsMS90HalfSortMode())
	{
		if (IsRowModeSeparateHalfMap())
		{
			lMaxRow	= (LONG)GetMS90HalfMapMaxRow();
		}
		else
		{
			lMinCol	= (LONG)GetMS90HalfMapMaxCol();
		}
	}
}

BOOL CMS896AStn::IsWithinValidMapLimit(LONG lRow, LONG lCol, DOUBLE dRatio)
{
	ULONG ulMinRow = GetMapValidMinRow();
	ULONG ulMinCol = GetMapValidMinCol();
	ULONG ulMaxRow = GetMapValidMaxRow();
	ULONG ulMaxCol = GetMapValidMaxCol();

	return IsWithinMapLimit(lRow, lCol, ulMinRow, ulMinCol, ulMaxRow, ulMaxCol, dRatio);
}//	within current map limit

BOOL CMS896AStn::CheckAlignMapWaferMatchHasDie(LONG lRow, LONG lCol)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulNumRow = 0, ulNumCol = 0;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulNumRow, ulNumCol);

	if( lRow<0 || lRow>(LONG)ulNumRow ||
		lCol<0 || lCol>(LONG)ulNumCol )
	{
		return FALSE;
	}

	UCHAR ucNullGrade	= m_WaferMapWrapper.GetNullBin();
	UCHAR ucBinOffset	= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGrade		= m_WaferMapWrapper.GetGrade(lRow, lCol);
	if( pApp->GetCustomerName()==CTM_SEMITEK && IsScanMapNgGrade(ucGrade-ucBinOffset) )
	{
		return FALSE;
	}

	if( ucGrade==ucNullGrade || m_WaferMapWrapper.IsReferenceDie(lRow, lCol) )
	{
		return FALSE;
	}

	ULONG ulDieState = GetMapDieState(lRow, lCol);
	if( ulDieState==WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY || ulDieState==WT_MAP_DS_UNPICK_REGRAB_EMPTY )
	{
		return FALSE;
	}

	if( ulDieState==WT_MAP_DIESTATE_SKIP_PREDICTED ||
		ulDieState==WAF_CDieSelectionAlgorithm::INVALID ||
		ulDieState==WAF_CDieSelectionAlgorithm::MISSING )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMS896AStn::CheckValidDieInBlock(LONG lRow, LONG lCol, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol, SHORT sGetGrade)
{
	if( (lRow < (LONG)ulULRow) || (lRow >= (LONG)ulLRRow) || (lCol < (LONG)ulULCol) || (lCol >= (LONG)ulLRCol) )
	{
		return FALSE;
	}

	if( sGetGrade>=0 )
	{
		UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lRow, lCol) - m_WaferMapWrapper.GetGradeOffset();
		if( ucGrade!=sGetGrade )
		{
			return FALSE;
		}
	}

	LONG lPhyX = 0, lPhyY = 0;
	if( CheckAlignMapWaferMatchHasDie(lRow, lCol) &&
		GetPrescanWftPosn(lRow, lCol, lPhyX, lPhyY) )
	{
		if( IsScanBadCutGrade(lRow, lCol) ||
			GetMapDieState(lRow, lCol)==WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT )
		{
			return FALSE;
		}

		if( IsScannedDefectGrade(lRow, lCol) ||
			GetMapDieState(lRow, lCol)==WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT )
		{
			return FALSE;
		}

		if( GetMapDieState(lRow, lCol)==WT_MAP_DIESTATE_SKIP_PREDICTED )
		{
			return FALSE;
		}

		return	IsInSelectedGrades(lRow, lCol);
	}

	return FALSE;
}

BOOL CMS896AStn::FindValidMapDieInBlock(ULONG ulLoop, ULONG &ulOutRow, ULONG &ulOutCol)
{
	ULONG ulULRow = 0, ulULCol = 0;
	if( ulOutRow>=ulLoop )
		ulULRow = ulOutRow - ulLoop;
	if( ulOutCol>=ulLoop )
		ulULCol = ulOutCol - ulLoop;
	ULONG ulLRRow = ulOutRow + ulLoop;
	ULONG ulLRCol = ulOutCol + ulLoop;

	return GetValidMapDieInBlock(-1, ulULRow, ulULCol, ulLRRow, ulLRCol, ulOutRow, ulOutCol);
}

BOOL CMS896AStn::GetValidMapDieInBlock(SHORT sGrade, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol, ULONG &ulOutRow, ULONG &ulOutCol)
{
	BOOL	bFindTgtDie = FALSE;
	LONG	lTmpRow, lTmpCol;
	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;
	LONG lPhyX = 0, lPhyY = 0;
	ULONG ulNumRow = 0, ulNumCol = 0, ulLoop;
	if(!WM_CWaferMap::Instance()->GetWaferMapDimension(ulNumRow, ulNumCol))
	{
		return FALSE;
	}

	if( ulLRRow>ulNumRow )
		ulLRRow = ulNumRow;
	if( ulLRCol>ulNumCol )
		ulLRCol = ulNumCol;

	ULONG ulIntRow = (ulULRow + ulLRRow)/2;
	ULONG ulIntCol = (ulULCol + ulLRCol)/2;

	ulLoop = max(ulLRRow-ulULRow, ulLRCol-ulULCol);
	if( ulLoop>max(ulNumRow, ulNumCol) )
		ulLoop = max(ulNumRow, ulNumCol);
	ulLoop = ulLoop/2;

	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
		CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol, sGrade)	)
	{
		ulOutRow = lTmpRow;
		ulOutCol = lTmpCol;
		return TRUE;
	}

	for (ulCurrentLoop=1; ulCurrentLoop<=ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop*2 + 1);
		lCol = (ulCurrentLoop*2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		if( lTmpRow>=0 && lTmpCol>=0 )
		{
			if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
				CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol, sGrade) )
			{
				bFindTgtDie = TRUE;
				break;
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
					CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol, sGrade) )
				{
					bFindTgtDie = TRUE;
					break;
				}
			}
		}
		if( bFindTgtDie )
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
					CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol, sGrade) )
				{
					bFindTgtDie = TRUE;
					break;
				}
			}
		}
		if( bFindTgtDie )
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
					CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol, sGrade) )
				{
					bFindTgtDie = TRUE;
					break;
				}
			}
		}
		if( bFindTgtDie )
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				if( GetMapPhyPosn(lTmpRow, lTmpCol, lPhyX, lPhyY) && 
					CheckValidDieInBlock(lTmpRow, lTmpCol, ulULRow, ulULCol, ulLRRow, ulLRCol, sGrade) )
				{
					bFindTgtDie = TRUE;
					break;
				}
			}
		}
		if( bFindTgtDie )
		{
			break;
		}
	}

	if( bFindTgtDie )
	{
		ulOutRow = lTmpRow;
		ulOutCol = lTmpCol;
	}

	return bFindTgtDie;
}


BOOL CMS896AStn::GetDieValidInX(LONG ulIntRow, LONG ulIntCol, LONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap)
{
	BOOL	bFindNearDie = FALSE;
	LONG	lTmpRow, lTmpCol;
	BOOL	lStatus;
	LONG ulCurrentLoop = 1;

	lStatus = TRUE;

	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if( GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY) )
	{
		return TRUE;
	}

	while( 1 )
	{
		if( ulLoop>0 )
			lTmpCol++;
		else
			lTmpCol--;

		if( lTmpRow>=0 && lTmpCol>=0 )
		{
			if( GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY) )
			{
				bFindNearDie = TRUE;
				break;
			}
		}
		ulCurrentLoop++;
		if( ulCurrentLoop>labs(ulLoop) )
		{
			break;
		}
	}

	if ( bFindNearDie == FALSE )
	{
		lStatus = FALSE;
	}
	else
	{
		LONG lDiePitchX_X = GetDiePitchX_X(); 
		LONG lDiePitchX_Y = GetDiePitchX_Y();

		LONG lDiff_X = ulIntCol - lTmpCol;

		//Calculate original no grade die pos from surrounding die position
		BOOL bPhyStatus = FALSE;
		if( lTmpRow>=0 && lTmpCol>=0 )
			bPhyStatus = GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY);
		if( bPhyStatus )
		{
			lPhyX = lPhyX - lDiff_X * lDiePitchX_X;
			lPhyY = lPhyY - lDiff_X * lDiePitchX_Y;
		}
		else
		{
			lStatus = FALSE;
		}
	}

	return lStatus;
}

BOOL CMS896AStn::GetDieValidPrescanPosn(LONG ulIntRow, LONG ulIntCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap)
{
	LONG lTmpWfX = 0, lTmpWfY = 0;
	BOOL	bFindNearValid = FALSE;
	LONG	lTmpRow, lTmpCol;
	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
	{
		lPhyX = lTmpWfX;
		lPhyY = lTmpWfY;
		return TRUE;
	}

	for (ulCurrentLoop = 1; ulCurrentLoop <= ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop * 2 + 1);
		lCol = (ulCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		if (lTmpRow >= 0 && lTmpCol >= 0)
		{
			if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
			{
				bFindNearValid = TRUE;
				break;
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
				{
					bFindNearValid = TRUE;
					break;
				}
			}
		}

		if (bFindNearValid)
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
				{
					bFindNearValid = TRUE;
					break;
				}
			}
		}

		if (bFindNearValid)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
				{
					bFindNearValid = TRUE;
					break;
				}
			}
		}

		if (bFindNearValid)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
				{
					bFindNearValid = TRUE;
					break;
				}
			}
		}

		if (bFindNearValid)
		{
			break;
		}
	}

	if (bFindNearValid)
	{
		LONG lDiePitchX_X = GetDiePitchX_X(); 
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_Y = GetDiePitchY_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();

		LONG lDiff_X = ulIntCol - lTmpCol;
		LONG lDiff_Y = ulIntRow - lTmpRow;

		//Calculate original no grade die pos from surrounding die position
		lPhyX = lTmpWfX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lPhyY = lTmpWfY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;

	}

	return bFindNearValid;
}

BOOL CMS896AStn::GetPrescanWftPosn(unsigned long ulRow, unsigned long ulCol, LONG &lPhyX, LONG &lPhyY)
{
	return GetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
}

BOOL CMS896AStn::GetWftPosn(BOOL bByMap, LONG ulRow, LONG ulCol, LONG &lPhyX, LONG &lPhyY)
{
	if( IsInMapValidRange(ulRow, ulCol)==FALSE )
	{
		return FALSE;
	}
	if (bByMap)
	{
		return GetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY);
	}
	else
	{
		return GetPrescanWftPosn(ulRow, ulCol, lPhyX, lPhyY);
	}
}

BOOL CMS896AStn::GetDieInvalidNearMapPosn(ULONG ulIntRow, ULONG ulIntCol, ULONG ulLoop, LONG &lTmpRow, LONG &lTmpCol, BOOL bByMap)
{
	LONG lPhyX = 0, lPhyY = 0;
	BOOL	bFindNearDie = FALSE;
	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if (!GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
	{
		return TRUE;
	}

	for (ulCurrentLoop = 1; ulCurrentLoop <= ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop * 2 + 1);
		lCol = (ulCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		if (lTmpRow >= 0 && lTmpCol >= 0)
		{
			if (!GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
			{
				bFindNearDie = TRUE;
				break;
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (!GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (!GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (!GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (!GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}
	}

	return bFindNearDie;
}

BOOL CMS896AStn::CheckDieAssignedAround(ULONG ulIntRow, ULONG ulIntCol, ULONG ulLoop, LONG lChkX, LONG lChkY, BOOL bByMap)
{
	LONG lPhyX, lPhyY;
	BOOL	bFindSameDie = FALSE;
	LONG	lTmpRow, lTmpCol;
	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
	{
		if (labs(lPhyX - lChkX) <= labs(GetDieSizeX() / 2) &&
				labs(lPhyY - lChkY) <= labs(GetDieSizeY() / 2))
		{
			return TRUE;
		}
	}

	for (ulCurrentLoop = 1; ulCurrentLoop <= ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop * 2 + 1);
		lCol = (ulCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		if (lTmpRow >= 0 && lTmpCol >= 0)
		{
			if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
			{
				if (labs(lPhyX - lChkX) <= labs(GetDieSizeX() / 2) &&
						labs(lPhyY - lChkY) <= labs(GetDieSizeY() / 2))
				{
					bFindSameDie = TRUE;
					break;
				}
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					if (labs(lPhyX - lChkX) <= labs(GetDieSizeX() / 2) &&
							labs(lPhyY - lChkY) <= labs(GetDieSizeY() / 2))
					{
						bFindSameDie = TRUE;
						break;
					}
				}
			}
		}

		if (bFindSameDie)
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					if (labs(lPhyX - lChkX) <= labs(GetDieSizeX() / 2) &&
							labs(lPhyY - lChkY) <= labs(GetDieSizeY() / 2))
					{
						bFindSameDie = TRUE;
						break;
					}
				}
			}
		}

		if (bFindSameDie)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					if (labs(lPhyX - lChkX) <= labs(GetDieSizeX() / 2) &&
							labs(lPhyY - lChkY) <= labs(GetDieSizeY() / 2))
					{
						bFindSameDie = TRUE;
						break;
					}
				}
			}
		}

		if (bFindSameDie)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lPhyX, lPhyY))
				{
					if (labs(lPhyX - lChkX) <= labs(GetDieSizeX() / 2) &&
							labs(lPhyY - lChkY) <= labs(GetDieSizeY() / 2))
					{
						bFindSameDie = TRUE;
						break;
					}
				}
			}
		}

		if (bFindSameDie)
		{
			break;
		}
	}

	return bFindSameDie;
}

BOOL CMS896AStn::IsEjtElvtInUse()
{
	return (IsES101() || IsES201()) && m_bDisableBH == FALSE;
}

BOOL CMS896AStn::IsBLInUse()	//	BACK LIGHT OF ES
{
	return FALSE;
}

BOOL CMS896AStn::IsESDualWT()
{
	return CMS896AApp::m_bESAoiDualWT;
}

BOOL CMS896AStn::IsESDualWL()
{
	return CMS896AApp::m_bESAoiDualWT;
}

BOOL CMS896AStn::IsESMachine()
{
	if (CMS896AApp::m_bES100v2DualWftOption == TRUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CMS896AStn::IsES201()
{
	if (CMS896AApp::m_bESAoiSingleWT == TRUE)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CMS896AStn::IsES101()
{
	if (IsES201())
	{
		return FALSE;
	}

	if (CMS896AApp::m_bES100v2DualWftOption == TRUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CMS896AStn::IsWT2InUse()
{
	return FALSE;
}

BOOL CMS896AStn::IsMotionCE()	// Motion system critical error, need to reset/home again.
{
	BOOL bCriticalError = (BOOL)(LONG)(*m_psmfSRam)["MS899"]["CriticalError"];
	return bCriticalError;
}

VOID CMS896AStn::SetMotionCE(CONST BOOL bSet, CONST CString szMsg)	//v4.59A19
{
	if (szMsg.GetLength() > 0)
	{
		CString szLog;
		szLog.Format("CRITICAL ERROR : (%d) - ", bSet);
		szLog = szLog + szMsg;
		SetErrorMessage(szLog);
	}
	(*m_psmfSRam)["MS899"]["CriticalError"] = bSet;

	if (bSet)
	{
		HmiMessage_Red_Yellow("CRITICAL ERROR:" + szMsg);
	}
}

BOOL CMS896AStn::IsWLExpanderOpen()
{
	BOOL bIsExpOpen = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Expander Status"];

	if( bIsExpOpen )
	{
		SetAlert_Red_Yellow(IDS_MS_EXPANDER_OPEN);
	}

	return bIsExpOpen;
}	// check expander open already

VOID CMS896AStn::GetHomeDiePhyPosn(LONG &lX, LONG &lY)
{
	lX = m_lHomeDiePhyPosX;
	lY = m_lHomeDiePhyPosY;
	if( IsMS90Sorting2ndPart() )
	{
		lX = m_lWT2HomeDiePhyPosX;	// for MS90 half sort second part
		lY = m_lWT2HomeDiePhyPosY;	// for MS90 half sort second part
	}
}

VOID CMS896AStn::GetWT2HomeDiePhyPosn(LONG &lX, LONG &lY)
{
	lX = m_lWT2HomeDiePhyPosX;
	if( lX==0 )
	{
		lX = m_lHomeDiePhyPosX + m_lWT2OffsetX;
	}
	lY = m_lWT2HomeDiePhyPosY;
	if( lY==0 )
	{
		lY = m_lHomeDiePhyPosY + m_lWT2OffsetY;
	}
}

BOOL CMS896AStn::IsMS50()
{
	return CMS896AApp::m_bMS50;
}

BOOL CMS896AStn::IsMS90()
{
	return CMS896AApp::m_bMS90;
}

BOOL CMS896AStn::IsMS60()
{
	return CMS896AApp::m_bMS60;
}

BOOL CMS896AStn::IsProber()
{
	return FALSE;
}

BOOL CMS896AStn::IsMSAutoLineMode()		//v4.55A7
{
	return (CMS896AApp::m_bMSAutoLineMode == 1);
}

LONG CMS896AStn::GetCGMaxIndex()
{
	return CMS896AApp::m_lChangeColletMaxIndex;
}

BOOL CMS896AStn::IsMSAutoLineStandloneMode()
{
	return (CMS896AApp::m_bMSAutoLineMode == 2);
}

BOOL CMS896AStn::IsTableXYMagneticEncoder()
{
	return CMS896AApp::m_bTableXYMagneticEncoder;
}

BOOL CMS896AStn::IsBinTableTHighResolution()
{
	return CMS896AApp::m_bBinTableTHighResolution;
}

BOOL CMS896AStn::IsNewAutoCleanColletSystem()
{
	return CMS896AApp::m_bNewAutoCleanColletSystem;
}


BOOL CMS896AStn::IsWaferTableWithExpander()
{
	return CMS896AApp::m_bUseExpanderWaferTable;
}


BOOL CMS896AStn::IsBurnInCP()
{
	return IsProber() && FALSE;
}

BOOL CMS896AStn::IsBurnIn()
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	return pAppMod->m_bBurnIn;
	//return m_bBurnInMode;
}

VOID CMS896AStn::SetWT2InUse(CONST BOOL bWT2InUse)	//	4.24TX
{
	if (IsESDualWT() == FALSE)
	{
		m_nWTInUse	= 0;
	}
	else
	{
		if (m_bDisableWT2InAutoBondMode)
		{
			m_nWTInUse = FALSE;
		}
		else
		{
			m_nWTInUse	= bWT2InUse;
		}
	}
	m_lJsWftInUse = IsWT2InUse();
}

//andrewng //2020-0805
BOOL CMS896AStn::IsErrMapInUse()
{
	//return CMS896AApp::m_bEnableErrMap;
	BOOL volatile bReturn = CMS896AApp::m_bEnableErrMap;			//v4.71A8	//Klocwork
	return bReturn;
}

BOOL CMS896AStn::IsBlkFuncEnable()
{
	return m_bBlkFuncEnable;	// run time enable/disable when load map file/change algorithm to block pick sorting
}

BOOL CMS896AStn::IsBlkFunc1Enable()
{
	return IsBlkFuncEnable() && m_bFullRefBlock == FALSE;
}

BOOL CMS896AStn::IsBlkFunc2Enable()
{
	return IsBlkFuncEnable() && m_bFullRefBlock == TRUE;
}

BOOL CMS896AStn::Is180Arm6InchWT()
{
	return CMS896AApp::m_b180Arm6InchWaferTable;
}

VOID CMS896AStn::WT_SpecialLog(CString szLogMsg)
{
	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\WT_AllRefer.log";

	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFileName, "a");
	if ((nErr == 0) && (fp != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		CString szTime = theTime.Format("%d (%H:%M:%S)");

		fprintf(fp, "%s - %s", (LPCTSTR)szTime, (LPCTSTR)(szLogMsg + "\n")); 
		fclose(fp);
	}
}

LONG CMS896AStn::GetDieSizeX()	//	wafer PR table step x
{
	return m_nDieSizeX;
}

LONG CMS896AStn::GetDieSizeY()	//	wafer PR table step y
{
	return m_nDieSizeY;
}

LONG CMS896AStn::GetBarPitchX()
{
	return m_siBarPitchX;
}

LONG CMS896AStn::GetBarPitchY()
{
	return m_siBarPitchY;
}

LONG CMS896AStn::GetDiePitchX_X()	// Die Pitch X (X) Enc Pos
{
	//andrewng //2020-0618
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//if (IsBurnIn() && !pApp->m_bBurnInGrabImage)	
	//{
	//	return NVC_WAF_BURNIN_DIE_PITCH_XX; 
	//}

	if( m_bRebelManualAlign && (m_bSearchHomeOption != WT_SPIRAL_SEARCH))	//v4.46T9
	{
		return (*m_psmfSRam)["WaferPr"]["NewDiePitchX"]["X"];	//For REBEL Tile wafer only, not U2U!
	}
	return (*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"];
}

LONG CMS896AStn::GetDiePitchX_Y()	// Die Pitch X (Y) Enc Pos
{
	//andrewng //2020-0618
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//if (IsBurnIn() && !pApp->m_bBurnInGrabImage)	
	//{
	//	return NVC_WAF_BURNIN_DIE_PITCH_XY; 
	//}

	if( m_bRebelManualAlign  && (m_bSearchHomeOption != WT_SPIRAL_SEARCH)) //v4.46T9
	{
		return (*m_psmfSRam)["WaferPr"]["NewDiePitchX"]["Y"];	//For REBEL Tile wafer only, not U2U!
	}
	return (*m_psmfSRam)["WaferPr"]["DiePitchX"]["Y"];
}

LONG CMS896AStn::GetDiePitchY_Y()	// Die Pitch Y (Y) Enc Pos
{
	//andrewng //2020-0618
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//if (IsBurnIn() && !pApp->m_bBurnInGrabImage)	
	//{
	//	return NVC_WAF_BURNIN_DIE_PITCH_YY; 
	//}

	if( m_bRebelManualAlign  && (m_bSearchHomeOption != WT_SPIRAL_SEARCH))	//v4.46T9
	{
		return (*m_psmfSRam)["WaferPr"]["NewDiePitchY"]["Y"];	//For REBEL Tile wafer only, not U2U!
	}
	return (*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"];
}

LONG CMS896AStn::GetDiePitchY_X()	// Die Pitch Y (X) Enc Pos
{
	//andrewng //2020-0618
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//if (IsBurnIn() && !pApp->m_bBurnInGrabImage)	
	//{
	//	return NVC_WAF_BURNIN_DIE_PITCH_YX; 
	//}

	if( m_bRebelManualAlign  && (m_bSearchHomeOption != WT_SPIRAL_SEARCH))	//v4.46T9
	{
		return (*m_psmfSRam)["WaferPr"]["NewDiePitchY"]["X"];	//For REBEL Tile wafer only, not U2U!
	}
	return (*m_psmfSRam)["WaferPr"]["DiePitchY"]["X"];
}

LONG CMS896AStn::GetAlignFrameRow()
{
	return m_lAlignFrameRow;
}

LONG CMS896AStn::GetAlignFrameCol()
{
	return m_lAlignFrameCol;
}

LONG CMS896AStn::GetScanLastPosnX()	//	427TX	4
{
	return m_nPrescanLastWftPosnX;
}

LONG CMS896AStn::GetScanLastPosnY()
{
	return m_nPrescanLastWftPosnY;
}

LONG CMS896AStn::GetPrescanPitchX()
{
	return m_lPrescanMovePitchX;
}

LONG CMS896AStn::GetPrescanPitchY()
{
	return m_lPrescanMovePitchY;
}

LONG CMS896AStn::GetLastFrameRow()
{
	return m_lLastFrameRow;
}

LONG CMS896AStn::GetLastFrameCol()
{
	return m_lLastFrameCol;
}

LONG CMS896AStn::GetScanErrorCode(LONG lScanErrorType)
{
	if( lScanErrorType<0 || lScanErrorType>=SCAN_ERR_END )
		return 0;

	return (LONG)pow(2.0, (DOUBLE) lScanErrorType);
}

BOOL CMS896AStn::ValidateBLBarcodeData()
{
	CStringMapFile *pSmfBLBC;
	CString szMsg;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();

	CSingleLock slLock(&m_csBLBarcode);
	slLock.Lock();

	if (pUtl->LoadConfigBLBarcode() == FALSE)
	{
		slLock.Unlock();
		return FALSE;
	}

	pSmfBLBC = pUtl->GetConfigFileBLBarcode();
	if (pSmfBLBC != NULL)
	{
		(*pSmfBLBC)["Enable"] = 1;		//v4.51A17

		pUtl->UpdateConfigBLBarcode();
		pUtl->CloseConfigBLBarcode();
		slLock.Unlock();
		return TRUE;
	}

	slLock.Unlock();
	return FALSE;
}

BOOL CMS896AStn::SaveBLBarcodeData(CONST LONG lBlkNo, CONST CString szBarcode, CONST LONG lMgzNo, CONST LONG lSlotNo)
{
	CStringMapFile *pSmfBLBC;
	CString szMsg;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();


	CSingleLock slLock(&m_csBLBarcode);
	slLock.Lock();

	if (pUtl->LoadConfigBLBarcode() == FALSE)
	{
		slLock.Unlock();
		return FALSE;
	}

	pSmfBLBC = pUtl->GetConfigFileBLBarcode();
	if (pSmfBLBC != NULL)
	{
		szMsg.Format("SaveBarcodeData: BT Scanning Blk %ld Mgzn %ld, Slot %ld, barcode=%s", lBlkNo, lMgzNo, lSlotNo, szBarcode);
		CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);

		(*pSmfBLBC)[lBlkNo]["Barcode"]	= szBarcode;
		(*pSmfBLBC)[lBlkNo]["Magazine"]	= lMgzNo;
		(*pSmfBLBC)[lBlkNo]["SlotNo"]	= lSlotNo;
		(*pSmfBLBC)["Enable"]			= 1;		//v4.51A17

		pUtl->UpdateConfigBLBarcode();
		pUtl->CloseConfigBLBarcode();

		slLock.Unlock();
		return TRUE;
	}

	slLock.Unlock();
	return FALSE;
}


CString CMS896AStn::GetBLBarcodeData(CONST LONG lBlkNo)
{
	CStringMapFile *pSmfBLBC;
	CString szBarCode = _T("");
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();


	CSingleLock slLock(&m_csBLBarcode);
	slLock.Lock();

	if (pUtl->LoadConfigBLBarcode() == FALSE)
	{
		slLock.Unlock();
		return szBarCode;
	}

	LONG lEnable = 0;	
	pSmfBLBC = pUtl->GetConfigFileBLBarcode();
	if (pSmfBLBC != NULL)
	{
		lEnable		= (*pSmfBLBC)["Enable"];		//v4.51A17
		szBarCode	= (*pSmfBLBC)[lBlkNo]["Barcode"];
		pUtl->CloseConfigBLBarcode();
	}

	slLock.Unlock();

	if (lEnable == 0)	//v4.51A17 //XM SanAn
	{
		SetErrorMessage("ERROR: BLBarcode MSD file is corrupted !!");
	}
	return szBarCode;
}


BOOL CMS896AStn::InitNichiaWafIDList(CONST LONG lMaxWaferIDs)
{
	CStringMapFile *pSmfWafIDList;
	CString szMsg;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return TRUE;
	}
	if (pApp->GetProductLine() != _T(""))	//v4.59A34
	{
		return TRUE;
	}

	CSingleLock slLock(&m_csBLNichiaWafIDList);
	slLock.Lock();

	if (pUtl->LoadNichiaWafIDListConfig() == FALSE)
	{
		//slLock.Unlock();
		return FALSE;
	}

	pSmfWafIDList = pUtl->GetNichiaWafIDListConfigFile();

	if (pSmfWafIDList != NULL)
	{
		for (INT i=1; i<=100; i++)
		{
			(*pSmfWafIDList)[i]["MaxCount"]		= lMaxWaferIDs;
			(*pSmfWafIDList)[i]["CurrCount"]	= (LONG) 0;
			
			for (INT j=1; j<=lMaxWaferIDs; j++)
			{
				(*pSmfWafIDList)[i][j]	= _T("");
			}
		}

		pUtl->UpdateNichiaWafIDListConfig();
		pUtl->CloseNichiaWafIDListConfig();
		slLock.Unlock();
		CMSLogFileUtility::Instance()->MS_LogOperation("Init Nichia BL Wafer ID list");
		return TRUE;
	}

	slLock.Unlock();
	return FALSE;
}


BOOL CMS896AStn::AddNichiaWafIDList(CONST ULONG ulBlkID, CONST CString szWaferID)
{
	CStringMapFile  *pSmfWafIDList;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return TRUE;
	}
	if (pApp->GetProductLine() != _T(""))	//v4.59A34
	{
		return TRUE;
	}

	CSingleLock slLock(&m_csBLNichiaWafIDList);
	slLock.Lock();

	if (pUtl->LoadNichiaWafIDListConfig() == FALSE)
	{
		slLock.Unlock();
		return FALSE;
	}

	pSmfWafIDList = pUtl->GetNichiaWafIDListConfigFile();
	BOOL bUpdate = FALSE;
	BOOL bStatus = FALSE;

	if (pSmfWafIDList != NULL)
	{
		LONG lMaxCount	= (*pSmfWafIDList)[ulBlkID]["MaxCount"];
		LONG lCurrCount	= (*pSmfWafIDList)[ulBlkID]["CurrCount"];

		if (lMaxCount > 0)
		{
			CString szID;
			BOOL bWaferIDAlreadyExist = FALSE;

			for (INT j=1; j<=lCurrCount; j++)
			{
				szID = (*pSmfWafIDList)[ulBlkID][j];
				if (szID == szWaferID)
				{
					bWaferIDAlreadyExist = TRUE;
					break;
				}
			}

			if (bWaferIDAlreadyExist)
			{
			}
			else if (szWaferID.GetLength() == 0)
			{
			}
			else if ( (lMaxCount > 0) && (lCurrCount <= lMaxCount) )
			{
				lCurrCount = lCurrCount + 1;
				(*pSmfWafIDList)[ulBlkID][lCurrCount]	= szWaferID;
				(*pSmfWafIDList)[ulBlkID]["CurrCount"]	= lCurrCount;
				bUpdate = TRUE;
			}

			bStatus = TRUE;

			if (bUpdate)
			{
				CString szLog;
				szLog.Format("Nichia WafID-List Add ID to BLK=%lu; Count=%ld(%ld); ID=" + szWaferID, ulBlkID, lCurrCount, lMaxCount); 
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				pUtl->UpdateNichiaWafIDListConfig();
			}
		}
		else
		{
			bStatus = TRUE;
		}

		pUtl->CloseNichiaWafIDListConfig();
		slLock.Unlock();
		return bStatus;
	}

	slLock.Unlock();
	return FALSE;
}


BOOL CMS896AStn::CheckIfNichiaWafIDListExceedLimit(CONST ULONG ulBlkID, CONST BOOL bIsUnload)
{
	CStringMapFile *pSmfWafIDList;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return FALSE;
	}
	if (pApp->GetProductLine() != _T(""))	//v4.59A34
	{
		return TRUE;
	}

	CSingleLock slLock(&m_csBLNichiaWafIDList);
	slLock.Lock();
	if (pUtl->LoadNichiaWafIDListConfig() == FALSE)
	{
		slLock.Unlock();
		return FALSE;
	}

	pSmfWafIDList = pUtl->GetNichiaWafIDListConfigFile();
	BOOL bExceedLimit = FALSE;
	CString szLog;

	if (pSmfWafIDList != NULL)
	{
		LONG lMaxCount	= (*pSmfWafIDList)[ulBlkID]["MaxCount"];
		LONG lCurrCount	= (*pSmfWafIDList)[ulBlkID]["CurrCount"];

		if (lMaxCount <= 0)
		{
			bExceedLimit = FALSE;
		}
		else 
		{
			if (bIsUnload)
			{
				if (lCurrCount >= lMaxCount)
				{
					szLog.Format("Nichia WafID-List exceed limit at UNLOAD for BLK=%lu; Count=%ld(%ld)", ulBlkID, lCurrCount, lMaxCount); 
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
					bExceedLimit = TRUE;
				}
			}
			else
			{
				if (lCurrCount > lMaxCount)
				{
					szLog.Format("Nichia WafID-List exceed limit for BLK=%lu; Count=%ld(%ld)", ulBlkID, lCurrCount, lMaxCount); 
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
					bExceedLimit = TRUE;
				}
			}
		}

		pUtl->CloseNichiaWafIDListConfig();
		slLock.Unlock();
		return bExceedLimit;
	}

	slLock.Unlock();
	return TRUE;
}


BOOL CMS896AStn::ResetNichiaWafIDList(CONST ULONG ulBlkID)
{
	CStringMapFile *pSmfWafIDList;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return TRUE;
	}
	if (pApp->GetProductLine() != _T(""))	//v4.59A34
	{
		return TRUE;
	}

	CSingleLock slLock(&m_csBLNichiaWafIDList);
	slLock.Lock();
	if (pUtl->LoadNichiaWafIDListConfig() == FALSE)
	{
		slLock.Unlock();
		return FALSE;
	}

	pSmfWafIDList = pUtl->GetNichiaWafIDListConfigFile();
	if (pSmfWafIDList != NULL)
	{
		LONG lMaxCount	= (*pSmfWafIDList)[ulBlkID]["MaxCount"];
		(*pSmfWafIDList)[ulBlkID]["CurrCount"]	= 0;
		
		for (INT j=1; j<=lMaxCount+1; j++)
		{
			(*pSmfWafIDList)[ulBlkID][j]	= _T("");		//zero-based
		}

		CString szLog;
		szLog.Format("Nichia WafID-List RESET for BLK=%lu", ulBlkID); 
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		pUtl->UpdateNichiaWafIDListConfig();
		pUtl->CloseNichiaWafIDListConfig();
	}

	slLock.Unlock();
	return TRUE;
}


VOID CMS896AStn::UpdateBackLightDownState()
{
}

BOOL CMS896AStn::SendRequestToTesterTerminal(CString szCmd, CString &szReply)
{
	return TRUE;
}

BOOL CMS896AStn::DisconnectTesterTerminal()
{
	return TRUE;
}

BOOL CMS896AStn::SendStartToTester()
{
	return FALSE;
}


LONG CMS896AStn::GetPSCMode()
{
	return PSC_NONE;
}

BOOL CMS896AStn::GetFPCMapPositionWpr(LONG lIndex, ULONG &ulAsmRow, ULONG &ulAsmCol, LONG &lUserRow, LONG &lUserCol)
{
	//(m_lScnCheckRow, m_lScnCheckCol) is original user coordinate, original user follow map'rotation
	lUserRow = m_lScnCheckRow[lIndex];
	lUserCol = m_lScnCheckCol[lIndex];
	// covert map display (after rotation not in () ) to asm row/col
	BOOL bReturn = TRUE;
//	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
//	if( pApp->IsUseHmiMap4HomeFPC() )
//		bReturn = ConvertHmiUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol);
//	else
		bReturn = ConvertOrgUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol);

	return bReturn;
}

BOOL CMS896AStn::GetErrorChooseGoFPC()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="Huga" )
	{
		return m_bErrorChooseGoFPC;
	}

	return m_bDisableWaferMapFivePointCheckMsgSelection == FALSE;
}

BOOL CMS896AStn::SaveMapBitmapImage(CString szMapImagePath)
{
#define	BLOCK_SIZE	2
	DWORD	m_ulGradeColor[256];
	memset(m_ulGradeColor, 0, sizeof(m_ulGradeColor));

	CStdioFile fFile;
	CString szColorFile = gszAsmHmiColorFile;

	LONG lCounter = 0;
	if( fFile.Open(szColorFile, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szReading;
		while( fFile.ReadString(szReading) )
		{
			//	B16777215	D0	I0	R8421504	P0	G0
			if( szReading.FindOneOf("BDIRPG")!=-1 )
			{
				continue;
			}

			m_ulGradeColor[lCounter] = atoi(szReading);
			lCounter++;
			if( lCounter>=256 )
			{
				break;
			}
		}
	}
	fFile.Close();

	if( lCounter>0 && lCounter<256 )
	{
		for(int i=lCounter; i<256; i++)
		{
			m_ulGradeColor[i] = m_ulGradeColor[i%lCounter];
		}
	}

	if( GetMapValidMaxCol() == GetMapValidMinCol() )
		return TRUE;
	if( GetMapValidMaxRow() == GetMapValidMinRow() )
		return TRUE;

	DOUBLE dRatioRow = 1.0, dRatioCol = 1.0;
	if( (GetMapValidMaxCol() - GetMapValidMinCol())>(GetMapValidMaxRow() - GetMapValidMinRow()) )
		dRatioRow = (DOUBLE)(GetMapValidMaxCol() - GetMapValidMinCol())/(DOUBLE)(GetMapValidMaxRow() - GetMapValidMinRow());
	else
		dRatioCol = (DOUBLE)(GetMapValidMaxRow() - GetMapValidMinRow())/(DOUBLE)(GetMapValidMaxCol() - GetMapValidMinCol());

	LONG lPixelW = _round(((GetMapValidMaxCol() - GetMapValidMinCol())*dRatioCol + 5)*(BLOCK_SIZE));
	LONG lPixelH = _round(((GetMapValidMaxRow() - GetMapValidMinRow())*dRatioRow + 5)*(BLOCK_SIZE));

	int nRowByteW = ((24*lPixelW+31)/32)*4;
	int nDataSize = nRowByteW*lPixelH;
	int nFileSize = 54 + nDataSize;  //w is your image width, h is image height, both int and pixel
	unsigned char *img = NULL;
	img = (unsigned char *)malloc(nDataSize);
	if (img == NULL)
		return FALSE;		//Klocwork	//v4.46
	memset(img, 0, nDataSize);

	unsigned char ucaBmpFileHeader[14] = {
		'B','M', // magic
		0,0,0,0, // size in bytes
		0,0, // app data
		0,0, // app data
		54,0,0,0 // start of data offset 54 = 40 + 14
	};
	ucaBmpFileHeader[ 2] = (unsigned char)(nFileSize    );
	ucaBmpFileHeader[ 3] = (unsigned char)(nFileSize>> 8);
	ucaBmpFileHeader[ 4] = (unsigned char)(nFileSize>>16);
	ucaBmpFileHeader[ 5] = (unsigned char)(nFileSize>>24);

	unsigned char ucaBmpInfoHeader[40] = {
		40,0,0,0, // info hd size
		0,0,0,0, // width
		0,0,0,0, // heigth
		1,0,	// number color planes
		24,0,	 // bits per pixel
		0,0,0,0, // compression is none
		0,0,0,0, // image bits size
		0,0,0,0, // 0x13,0x0B,0,0, horz resoluition in pixel / m
		0,0,0,0, // 0x13,0x0B,0,0, vert resolutions (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
		0,0,0,0, // #colors in pallete
		0,0,0,0 // #important colors
	};
	ucaBmpInfoHeader[ 4] = (unsigned char)(       lPixelW    );
	ucaBmpInfoHeader[ 5] = (unsigned char)(       lPixelW>> 8);
	ucaBmpInfoHeader[ 6] = (unsigned char)(       lPixelW>>16);
	ucaBmpInfoHeader[ 7] = (unsigned char)(       lPixelW>>24);

	ucaBmpInfoHeader[ 8] = (unsigned char)(       lPixelH    );
	ucaBmpInfoHeader[ 9] = (unsigned char)(       lPixelH>> 8);
	ucaBmpInfoHeader[10] = (unsigned char)(       lPixelH>>16);
	ucaBmpInfoHeader[11] = (unsigned char)(       lPixelH>>24);

	ucaBmpInfoHeader[20] = (unsigned char)( nDataSize    );
	ucaBmpInfoHeader[21] = (unsigned char)( nDataSize>> 8);
	ucaBmpInfoHeader[22] = (unsigned char)( nDataSize>>16);
	ucaBmpInfoHeader[23] = (unsigned char)( nDataSize>>24);

	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGoodGrade	= m_ucPrescanGoodGrade;
	UCHAR ucDefectGrade	= m_ucPrescanDefectGrade;
	UCHAR ucReferGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bZhongKe = FALSE;
	if( pApp->GetCustomerName()=="ZhongKe" && m_bPrescanSkipNgGrade )
	{
		bZhongKe = TRUE;
	}
	for(LONG lRow=GetMapValidMinRow(); lRow<=GetMapValidMaxRow(); lRow++)
	{
		for(LONG lCol=GetMapValidMinCol(); lCol<=GetMapValidMaxCol(); lCol++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lRow, lCol);
			if( ucGrade==ucNullBin )
				continue;
			ucGrade = ucGrade - ucOffset;

			COLORREF crColor = m_ulGradeColor[ucGrade];
			UCHAR r = GetRValue(crColor);
			UCHAR g = GetGValue(crColor);
			UCHAR b = GetBValue(crColor);
			LONG lColB = _round((lCol - GetMapValidMinCol())*dRatioCol*BLOCK_SIZE + 2*BLOCK_SIZE);
			LONG lRowB = _round((GetMapValidMaxRow() - lRow)*dRatioRow*BLOCK_SIZE + 3*BLOCK_SIZE);
			for(INT nRow=0; nRow<(BLOCK_SIZE-1); nRow++)
			{
				for(INT nCol=0; nCol<(BLOCK_SIZE-1); nCol++)
				{
					LONG lIndex = (lColB + nCol) * 3 + (lRowB-nRow) * nRowByteW;

					if (img != NULL)	//Klocwork	//v4.46
					{
						img[lIndex+2] = r;
						img[lIndex+1] = g;
						img[lIndex+0] = b;
					}
				}
			}
		}
	}

	FILE *f = NULL;
	errno_t nErr = fopen_s(&f, szMapImagePath,"wb");
	if ((nErr == 0) && (f != NULL))		//Klocwork	//v4.46
	{
		fwrite(ucaBmpFileHeader, 1, 14, f);
		fwrite(ucaBmpInfoHeader, 1, 40, f);

		for(int i=0; i<lPixelH; i++)
		{
			fwrite(img+(nRowByteW * i), 1, nRowByteW, f);
		}

		fclose(f);
	}

	if (img != NULL)
	{
		free(img);	//v4.47 Klocwork
	}
	return TRUE;
}

CString CMS896AStn::GetMachineNo()
{
	CString szMcNo;
	szMcNo = (*m_psmfSRam)["MS896A"]["MachineNo"];

	return szMcNo;
}	// Machine no get function

LONG CMS896AStn::GetPrInContour(BOOL bWT2, LONG lInX, LONG lInY, DOUBLE &dPrX, DOUBLE &dPrY)
{
	dPrX = 0;
	dPrY = 0;
	LONG lCtrX = GetWft1CenterX() + m_lWL1WaferOffsetX;
	LONG lCtrY = GetWft1CenterY() + m_lWL1WaferOffsetY;
	if( bWT2 )
	{
		lCtrX = GetWft2CenterX() + m_lWL2WaferOffsetX;
		lCtrY = GetWft2CenterY() + m_lWL2WaferOffsetY;
	}
	LONG lX = lInX - lCtrX;
	LONG lY = lInY - lCtrY;
	if( bWT2 )
	{
		DOUBLE dDivid = m_dWL2CalibXX*m_dWL2CalibYY - m_dWL2CalibXY*m_dWL2CalibYX;
		if( fabs(dDivid)>0.000001 )
		{
			dPrX = (m_dWL2CalibYY*lX - m_dWL2CalibXY*lY)/dDivid;
			dPrY = (m_dWL2CalibXX*lY - m_dWL2CalibYX*lX)/dDivid;
		}
		else
		{
			if( fabs(m_dWL2CalibXX)>0.000001 && fabs(m_dWL2CalibYY)>0.000001 )
			{
				dPrX = lX/m_dWL2CalibXX;
				dPrY = lY/m_dWL2CalibYY;
			}
		}
	}
	else
	{
		DOUBLE dDivid = m_dWL1CalibXX*m_dWL1CalibYY - m_dWL1CalibXY*m_dWL1CalibYX;
		if( fabs(dDivid)>0.000001 )
		{
			dPrX = (m_dWL1CalibYY*lX - m_dWL1CalibXY*lY)/dDivid;
			dPrY = (m_dWL1CalibXX*lY - m_dWL1CalibYX*lX)/dDivid;
		}
		else
		{
			if( fabs(m_dWL1CalibXX)>0.000001 && fabs(m_dWL1CalibYY)>0.000001 )
			{
				dPrX = lX/m_dWL1CalibXX;
				dPrY = lY/m_dWL1CalibYY;
			}
		}
	}

	DOUBLE dPrCtrX = 4096, dPrCtrY = 4096;
	dPrX = dPrCtrX + dPrX;
	dPrY = dPrCtrY + dPrY;

	return 1;
}

VOID CMS896AStn::ContourLog(CString szMsg, CONST BOOL bWT2, CONST BOOL bNew)
{
	if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
	{
		return ;
	}


	CString szFileName = gszUSER_DIRECTORY + "\\History\\ContourWT1.log";
	if (bWT2)
	{
		szFileName = gszUSER_DIRECTORY + "\\History\\ContourWT2.log";
	}

	if (bNew)
	{
		DeleteFile(szFileName);
	}

	FILE *fpRecord = NULL;
	errno_t nErr = fopen_s(&fpRecord, szFileName, "a+");
	if ((nErr == 0) && (fpRecord != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(fpRecord, "%s\n", (LPCTSTR) szMsg);	//Klocwork  
		fclose(fpRecord);
	}	
}

BOOL CMS896AStn::CheckDiskSize(LPCSTR lpDiskName, ULONG &lDiskSpaceUsed, ULONG &lDiskSpaceRemain)
{
	ULARGE_INTEGER	FreeForCaller,	// Free bytes available to caller
					TotalBytes,		// Total number of bytes
					FreeBytes;		// Total number of free bytes

	BOOL bReturn = GetDiskFreeSpaceEx(lpDiskName, &FreeForCaller, &TotalBytes, &FreeBytes);

	lDiskSpaceRemain = 0;
	ULONG lDiskSpaceTotal = 0, lDiskSpaceFree4Caller = 0;
	if ( bReturn )
	{
		lDiskSpaceTotal			= (ULONG)((ULONGLONG)TotalBytes.QuadPart / (1024 * 1024));
		lDiskSpaceFree4Caller	= (ULONG)((ULONGLONG)FreeForCaller.QuadPart / (1024 * 1024));
		lDiskSpaceRemain		= (ULONG)((ULONGLONG)FreeBytes.QuadPart / (1024 * 1024));
	}
	lDiskSpaceUsed = lDiskSpaceTotal - lDiskSpaceRemain;

	return bReturn; 
}

BOOL CMS896AStn::CheckDiskSpace(CString szDiskLabel, DOUBLE dFreeSpacePct)
{
	BOOL bReturn = TRUE;

    DWORD	dwSectPerClust,
			dwBytesPerSect,
			dwFreeClusters,
			dwTotalClusters;

    unsigned __int64	//i64FreeBytesToCaller,
						i64TotalBytes,
						i64FreeBytes;

	CString szDisk = szDiskLabel;	//"C:\\";

	bReturn = GetDiskFreeSpace (szDisk, 
								&dwSectPerClust,
								&dwBytesPerSect, 
								&dwFreeClusters,
								&dwTotalClusters);
	
	if (bReturn)
	{
		/* force 64-bit math */ 
		i64TotalBytes	= (__int64) dwTotalClusters * dwSectPerClust * dwBytesPerSect / (1024 * 1024);
		i64FreeBytes	= (__int64) dwFreeClusters  * dwSectPerClust * dwBytesPerSect / (1024 * 1024);

		DOUBLE dFreePct = (DOUBLE) 100.00 * i64FreeBytes / i64TotalBytes;

		CString szMsg;
		szMsg.Format("DiskSpace (%s): total = %I64u; free = %I64u (MB); Free pct = %.2f", 
			szDiskLabel, i64TotalBytes, i64FreeBytes, dFreePct);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		if (dFreePct < 5.0)
		{
			szMsg.Format("DiskSpace (%s): total = %I64u; free = %I64u (MB); Free pct = %.2f; status = FAIL", 
				szDiskLabel, i64TotalBytes, i64FreeBytes, dFreePct);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			return FALSE;
		}

		szMsg.Format("DiskSpace (%s): total = %I64u; free = %I64u (MB); Free pct = %.2f; status = OK", 
			szDiskLabel, i64TotalBytes, i64FreeBytes, dFreePct);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		return TRUE;
	}

/*
	ULARGE_INTEGER	FreeForCaller,	// Free bytes available to caller
					TotalBytes,		// Total number of bytes
					FreeBytes;		// Total number of free bytes

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szDisk = "C:\\";

	bReturn = GetDiskFreeSpaceEx(szDisk, &FreeForCaller, &TotalBytes, &FreeBytes);
	if (!bReturn)
	{
		return FALSE;
	}
		
	LONG lDiskSpaceTotal	= (LONG) TotalBytes.QuadPart/(1024*1024);
	LONG lDiskSpaceRemain	= (LONG) FreeForCaller.QuadPart/(1024*1024);
	LONG lDiskSpaceUsed		= (LONG) FreeBytes.QuadPart/(1024*1024);

	CString szMsg;
	szMsg.Format("DiskSpace (%s): total=%ld; used=%ld; free=%ld (MB)", 
		szDiskLabel, lDiskSpaceTotal, lDiskSpaceUsed, lDiskSpaceRemain);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
*/
	return FALSE;
}

BOOL CMS896AStn::GetDieValidNearMapPosn(ULONG ulInRow, ULONG ulInCol, ULONG ulLoop, LONG &lTgtRow, LONG &lTgtCol, BOOL bByMap)
{
	LONG lPhyX = 0, lPhyY = 0;
	BOOL	bFindNearDie = FALSE;
	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	lTgtRow = ulInRow;
	lTgtCol = ulInCol;
	if (GetWftPosn(bByMap, lTgtRow, lTgtCol, lPhyX, lPhyY))
	{
		return TRUE;
	}

	for (ulCurrentLoop = 1; ulCurrentLoop <= ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop * 2 + 1);
		lCol = (ulCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		lTgtRow += lDiff_X;
		lTgtCol += lDiff_Y;

		if (lTgtRow >= 0 && lTgtCol >= 0)
		{
			if (GetWftPosn(bByMap, lTgtRow, lTgtCol, lPhyX, lPhyY))
			{
				bFindNearDie = TRUE;
				break;
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			lTgtRow += lDiff_X;
			lTgtCol += lDiff_Y;

			if (lTgtRow >= 0 && lTgtCol >= 0)
			{
				if (GetWftPosn(bByMap, lTgtRow, lTgtCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			lTgtRow += lDiff_X;
			lTgtCol += lDiff_Y;

			if (lTgtRow >= 0 && lTgtCol >= 0)
			{
				if (GetWftPosn(bByMap, lTgtRow, lTgtCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			lTgtRow += lDiff_X;
			lTgtCol += lDiff_Y;

			if (lTgtRow >= 0 && lTgtCol >= 0)
			{
				if (GetWftPosn(bByMap, lTgtRow, lTgtCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTgtRow += lDiff_X;
			lTgtCol += lDiff_Y;

			if (lTgtRow >= 0 && lTgtCol >= 0)
			{
				if (GetWftPosn(bByMap, lTgtRow, lTgtCol, lPhyX, lPhyY))
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if (bFindNearDie)
		{
			break;
		}
	}

	return bFindNearDie;
}

BOOL CMS896AStn::CopyFileWithQueue(LPCTSTR szSourceFile, LPCTSTR szTargetFile, BOOL bDelSrcAtLast)
{
	BOOL bReturn = CopyFile(szSourceFile, szTargetFile, FALSE);
	if( bReturn )
	{
		if( bDelSrcAtLast )
		{
			DeleteFile(szSourceFile);
		}
	}
	else
	{
		//CSingleLock slLock(&m_csCopyFileQueueList);
		//slLock.Lock();
		//m_saCopySrcNameList.Add(szSourceFile);
		//m_saCopyTgtNameList.Add(szTargetFile);
		//m_baDeleteSrcList.Add(bDelSrcAtLast);
		//slLock.Unlock();
	}

	return bReturn;
}

// file download from server and to be used then, so must be successfully done, i.e. load map file
BOOL CMS896AStn::CopyFileWithRetry(LPCTSTR szSourceFile, LPCTSTR szTargetFile, BOOL bDelSrcAtLast)
{
	BOOL bReturn = TRUE;
	for(int i=0; i<1; i++)
	{
		bReturn = CopyFile(szSourceFile, szTargetFile, FALSE);
		if( bReturn )
		{
			if( bDelSrcAtLast )
			{
				DeleteFile(szSourceFile);
			}
			break;
		}
		Sleep(10);
	}

	return bReturn;
}

BOOL CMS896AStn::RenameFile(LPCTSTR szSourceFile, LPCTSTR szTargetFile)	//	must be same folder
{
	//DeleteFile(szTargetFile);
	//CStdioFile::Rename(szSourceFile, szTargetFile);
	CopyFile(szSourceFile, szTargetFile, FALSE);
	DeleteFile(szSourceFile);
	return TRUE;
}	// rename file name, without copy, it should be fast.

BOOL CMS896AStn::WriteUnpickInfo(const unsigned long ulUnpickType, long lrow, long lcol)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()!=CTM_FINISAR || m_bIfGenWaferEndFile==FALSE )
	{
		return TRUE;
	}

	CTime currTime = CTime::GetCurrentTime();
	CString szcurrTime = currTime.Format("%Y-%m-%d %H:%M:%S");

	CString szUnpickType;
	switch (ulUnpickType)
	{
	case WT_MAP_DIESTATE_DEFECT:
		szUnpickType = "Defective device";
		break;

	case WT_MAP_DIESTATE_INK:
		szUnpickType = "Ink Die";
		break;

	case WT_MAP_DIESTATE_CHIP:
		szUnpickType = "Chip Die";
		break;

	case WT_MAP_DIESTATE_BADCUT:
		szUnpickType = "Bad Cut Die";
		break;

	case WT_MAP_DIESTATE_EMPTY:
		szUnpickType = "No devices available";
		break;

	case WT_MAP_DIESTATE_ROTATE:
		szUnpickType = "Rotation error device";
		break;

	case WT_MAP_DIESTATE_UNPICK:
		szUnpickType = "Unpick Die";
		break;

	case WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY:
		szUnpickType = "Unrecognised devices";
		break;

	case WT_MAP_DS_UNPICK_REGRAB_EMPTY:
		szUnpickType = "Low score device";
		break;

	case WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT:
		szUnpickType = "Unpick Defect Die";
		break;

	case WT_MAP_DS_UNPICK_SCAN_EXTRA:
		szUnpickType = "Unpick Extra Die";
		break;

	case WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT:
		szUnpickType = "Unpick BadCut Die";
		break;

	case IDS_BH_MODULE_NO_POWER:
		szUnpickType = "AirFlowByMD";
		break;

	case 8:
		szUnpickType = "Devices fail in uplook camera";
		break;

	case 0:
		szUnpickType = "Drop device";
		break;

	default:
		szUnpickType = "Unpick Die";
		break;
	}

	LONG encX = 0, encY = 0;
	double dDieAngle = 0;
	CString szDieScore = "0";
	USHORT usDieState = 0;
	GetScanInfo(lrow, lcol, encX, encY, dDieAngle, szDieScore, usDieState);
	if( szDieScore=="-1" )
		szUnpickType = "Unrecognised devices";

	CString szRow;
	CString szCol;
	LONG lMapRow = 0, lMapCol = 0;
	ConvertAsmToOrgUser(lrow, lcol, lMapRow, lMapCol);
	szRow.Format("%d", lMapRow);
	szCol.Format("%d", lMapCol);

	CString szElectricInfoLine;
	m_WaferMapWrapper.GetSpecialDieName(lrow, lcol, szElectricInfoLine);
	szElectricInfoLine.Replace(" ",",");

	CStdioFile cfUnpickDieInfoFile;
	if (cfUnpickDieInfoFile.Open(_T(gszUnpickDieInfoPath), 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone|CFile::typeText) == FALSE)
	{
		return FALSE;
	}

	cfUnpickDieInfoFile.SeekToEnd();

	cfUnpickDieInfoFile.WriteString(szUnpickType + "," + szcurrTime + "," + szElectricInfoLine + "," + szRow + "," + szCol + "," + szDieScore + "\n");

	//Close wafer end file
	cfUnpickDieInfoFile.Close();

	return TRUE;
}


BOOL CMS896AStn::GetIPCReply(int nConvID, LONG &lRet)
{
	IPC_CServiceMessage stMsg;

	try
	{
		CWinApp    *pApp = AfxGetApp();
		MSG               msgWndMsg;
		while (!m_comClient.ScanReplyForConvID(nConvID, 1)) //    Not success
		{
			m_comServer.ProcessRequest();
			while (::PeekMessage(&msgWndMsg, NULL, NULL, NULL, PM_NOREMOVE))
			{
				// pump message, but quit on WM_QUIT
				if (!pApp->PumpMessage())
				{
					return FALSE;
				}
			}
			Sleep(10);
		}
		m_comClient.ReadReplyForConvID(nConvID, stMsg);
	}
	catch (CAsmException e)
	{
		return FALSE;
	}

	stMsg.GetMsg(sizeof(LONG), &lRet);
	return TRUE;
}


BOOL CMS896AStn::GetIPCReplyMsg(int nConvID, IPC_CServiceMessage &stMsg)
{
	BOOL bRet = FALSE;

	try
	{
		CWinApp    *pApp = AfxGetApp();
		MSG               msgWndMsg;
		while (!m_comClient.ScanReplyForConvID(nConvID, 1)) //    Not success
		{
			m_comServer.ProcessRequest();
			while (::PeekMessage(&msgWndMsg, NULL, NULL, NULL, PM_NOREMOVE))
			{
				// pump message, but quit on WM_QUIT
				if (!pApp->PumpMessage())
				{
					return FALSE;
				}
			}
			Sleep(10);
		}
		m_comClient.ReadReplyForConvID(nConvID, stMsg);
	}
	catch (CAsmException e)
	{
		return FALSE;
	}
	return TRUE;
}

//================================================================
// Function Name: 		SendGeneralRequest
// Input arguments:     None
// Output arguments:	None
// Description:   		General IPC function
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CMS896AStn::SendGeneralRequest(const CString mszRecvStationName, const CString szFuncName, const BOOL bInValue)
{
	IPC_CServiceMessage rReqMsg;
	BOOL bValue = bInValue;
	rReqMsg.InitMessage(sizeof(BOOL), &bValue);
	INT nConvID = m_comClient.SendRequest(mszRecvStationName, szFuncName, rReqMsg);
	BOOL bResult = GetIPCReplyMsg(nConvID, rReqMsg);
	if (bResult)
	{
		rReqMsg.GetMsg(sizeof(BOOL), &bResult);
	}
	return bResult;
/*
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	
	return bResult;
*/
}


LONG CMS896AStn::_round(double val)
{
	return CMathFunc::Instance()->_round(val);
}


LONG CMS896AStn::GetCurTime()
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	return pAppMod->GetCurTime();
}

LONG CMS896AStn::elapse_time(LONG lStartTime)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	return pAppMod->elapse_time(lStartTime);
}

LONG CMS896AStn::GetTimeInterval(const LARGE_INTEGER &StartTime, LARGE_INTEGER &EndTime)
{
	LARGE_INTEGER lnFreq;
	QueryPerformanceFrequency(&lnFreq);
	double m_dFreqClk = (double) 1000 / lnFreq.QuadPart;

	QueryPerformanceCounter(&EndTime);
	return _round((EndTime.QuadPart - StartTime.QuadPart) * m_dFreqClk);
}

LONG CMS896AStn::GetTimeInterval(const LARGE_INTEGER &StartTime)
{
	LARGE_INTEGER lnFreq;
	QueryPerformanceFrequency(&lnFreq);
	double m_dFreqClk = (double) 1000 / lnFreq.QuadPart;

	LARGE_INTEGER EndTime;
	QueryPerformanceCounter(&EndTime);
	return _round((EndTime.QuadPart - StartTime.QuadPart) * m_dFreqClk);
}

VOID CMS896AStn::IncreaseRotateDieCounter()
{
	m_ulRotateDieCount++;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEquipmentManager = pApp->GetFeatureStatus(MS896A_FUNC_HOST_COMM_EQUIP_MANAGER);
	if (bEquipmentManager)
	{
		pApp->m_eqMachine.AddEMHourlyOneRotateDieCount();
	}
}


VOID CMS896AStn::IncreaseEmptyDieCounter()
{
	m_ulEmptyDieCount++;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEquipmentManager = pApp->GetFeatureStatus(MS896A_FUNC_HOST_COMM_EQUIP_MANAGER);
	if (bEquipmentManager)
	{
		pApp->m_eqMachine.AddEMHourlyOneEmptyDieCount();
	}
}

VOID CMS896AStn::IncreaseMissingDieCounter()
{
	m_ulMissingDieCount++;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEquipmentManager = pApp->GetFeatureStatus(MS896A_FUNC_HOST_COMM_EQUIP_MANAGER);
	if (bEquipmentManager)
	{
		pApp->m_eqMachine.AddEMHourlyOneMissingDieCount();
	}
}

VOID CMS896AStn::IncreasePBIShiftCounter()
{
	m_lPBIShiftCounter++;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEquipmentManager = pApp->GetFeatureStatus(MS896A_FUNC_HOST_COMM_EQUIP_MANAGER);
	if (bEquipmentManager)
	{
		pApp->m_eqMachine.AddEMHourlyOnePBIShiftCount();
	}
}

VOID CMS896AStn::IncreasePBIAngleCounter()
{
	m_lPBIAngleCounter++;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEquipmentManager = pApp->GetFeatureStatus(MS896A_FUNC_HOST_COMM_EQUIP_MANAGER);
	if (bEquipmentManager)
	{
		pApp->m_eqMachine.AddEMHourlyOnePBIRotationFailureCount();
	}
}

// Set the value of the variable
BOOL CMS896AStn::UserHmiUpdateData(const CString &strVariableName, long input)
{
	IPC_CServiceMessage svMsg;
	char acTemp[1000];
	double d0 = input; //default
	int nIndex = 0;

	strcpy_s(acTemp, sizeof(acTemp), strVariableName); // Variable name in HMI data list
	nIndex += strVariableName.GetLength() + 1;

	memcpy(&acTemp[nIndex], &d0, sizeof(long));
	nIndex += sizeof(long);
	svMsg.InitMessage(nIndex, acTemp);
	INT nConvId = m_comClient.SendRequest("HmiUserService", "HmiUpdateData", svMsg);
	while (!m_comClient.ScanReplyForConvID(nConvId, 1))
	{
		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}
	} 
	m_comClient.ReadReplyForConvID(nConvId, svMsg);	
	return TRUE;
}

BOOL CMS896AStn::UserHmiNumericKeys(const CString &strTitle, const CString &strVariableName, double maxv, double minv, long *input)
{
	LPSTR lpsz = new TCHAR[200];
	short bufPtr = 0;
	BOOL ret = true;

	long lInputDouble = *input;
	lInputDouble = max(lInputDouble, (long)minv);
	lInputDouble = min(lInputDouble, (long)maxv);
	SetHmiVariable(strVariableName);
	UserHmiUpdateData(strVariableName, *input);
//	SetHmiVariable(this->prefixName + ":InputDouble");

	// variable name
//	text.Format(this->prefixName + ":InputDouble"); 
	memcpy(lpsz, strVariableName, strVariableName.GetLength() + 1);
	bufPtr += (strVariableName.GetLength() + 1);

	// title
	memcpy(lpsz + bufPtr, strTitle, strTitle.GetLength() + 1);
	bufPtr += (strTitle.GetLength() + 1);

	// max
	memcpy(lpsz + bufPtr, &maxv, sizeof(double));
	bufPtr += sizeof(double);

	// min
	memcpy(lpsz + bufPtr, &minv, sizeof(double));
	bufPtr += sizeof(double);

	// return 
	memcpy(lpsz + bufPtr, &ret, sizeof(BOOL));
	bufPtr += sizeof(BOOL);

	IPC_CServiceMessage svMsg;
	svMsg.InitMessage((INT)bufPtr, lpsz);
	INT nConvId = m_comClient.SendRequest("HmiUserService", _T("HmiNumericKeys"), svMsg);
	while (!m_comClient.ScanReplyForConvID(nConvId, 1))
	{
		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}
	} 
	m_comClient.ReadReplyForConvID(nConvId, svMsg);	
	delete[] lpsz;


	lpsz = new TCHAR[svMsg.GetMsgLen()];
	svMsg.GetMsg(lpsz, svMsg.GetMsgLen());
	memcpy(&ret, lpsz, sizeof(BOOL));
	bufPtr = sizeof(BOOL);
	if (ret) 
	{
		*input = atoi(lpsz + bufPtr);
	}
	delete[] lpsz;

	return ret ? TRUE : FALSE;
}


// Add Function for Writing Message to Parameter Log
VOID CMS896AStn::WriteParameterLog(const BOOL bHmiOperationLogOption, CString szMessage)
{
	if (bHmiOperationLogOption)  	// Write Parameter Log Only If Parameter Log Enabled
	{
		IPC_CServiceMessage svMsg;
		svMsg.InitMessage(szMessage.GetLength() + 1, szMessage.GetBuffer());
		szMessage.ReleaseBuffer();
		m_comClient.SendRequest("HmiUserService", "HmiOperationLogMessage", svMsg, 0);
	}
}


BOOL CMS896AStn::IsPathExist(const CString szPath)
{
	if (szPath.IsEmpty())
	{
		return FALSE;
	}

	return ((GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

BOOL CMS896AStn::IsFileExist(const CString szFile)
{
	if (szFile.IsEmpty())
	{
		return FALSE;
	}

	DWORD dwAttrib = GetFileAttributes(szFile);

	return ((dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0));
}


VOID CMS896AStn::SD_GetData(double dX1, double dX2)
{
	int nSize1 = (int)max(m_szaSDX1.GetSize(), m_szaSDX2.GetSize());

	CString szTemp;
	szTemp.Format("%lf",dX1);
	m_szaSDX1.Add(szTemp);

	szTemp.Format("%lf",dX2);
	m_szaSDX2.Add(szTemp);

	return;
}

VOID CMS896AStn::SD_Calculate(DOUBLE &dMax1,DOUBLE &dMin1,DOUBLE &dMax2,DOUBLE &dMin2,DOUBLE &dSD1,DOUBLE &dSD2)
{
	double dsum = 0, davg, dsum1,dsd;
	CString szTemp;
	double  dTemp;
	CString szTemp1;
	double  dTemp1;
	CString szMsg;
	double dmax,dmin;
	
	for(int i = 0; i < m_szaSDX1.GetSize(); i++)
	{
		szTemp = m_szaSDX1.GetAt(i);
		dTemp = atof((LPCTSTR)szTemp);
		dsum = dsum + dTemp;
	}

	davg = dsum / m_szaSDX1.GetSize();

	dsum1 = 0;
	for(int k = 0; k < m_szaSDX1.GetSize();k++)
	{	
		szTemp1 = m_szaSDX1.GetAt(k);
		dTemp1 = atof((LPCTSTR)szTemp1);
		dsum1 = dsum1 + (dTemp1 - davg)*(dTemp1 - davg);
	}

	if (m_szaSDX1.GetSize() != 0)
		dsd = sqrt(dsum1/(m_szaSDX1.GetSize()-1));

	szMsg.Format("Xsd,%lf",dsd);
	dSD1 = dsd;

//range
	for(int n = 0; n < m_szaSDX1.GetSize();n++)
	{	
		szTemp1 = m_szaSDX1.GetAt(n);
		dTemp1 = atof((LPCTSTR)szTemp1);

		if (n == 0)
		{
			dmax = dTemp1;
			dmin = dTemp1;
		}
		else
		{
			if (dTemp1 > dmax)
				dmax = dTemp1;
			if (dTemp1 < dmin)
				dmin = dTemp1;
		}
	}
	szMsg.Format("rangeX,%lf,%lf",dmax,dmin);
	dMax1 = dmax;
	dMin1 = dmin;

	//sd Y
	dsum = 0;
	dsd = 0;
	for(int i = 0; i < m_szaSDX2.GetSize(); i++)
	{
		szTemp = m_szaSDX2.GetAt(i);
		dTemp = atof((LPCTSTR)szTemp);
		dsum = dsum + dTemp;
	}

	davg = dsum / m_szaSDX2.GetSize();

	dsum1 = 0;
	for(int k = 0; k < m_szaSDX2.GetSize();k++)
	{	
		szTemp1 = m_szaSDX2.GetAt(k);
		dTemp1 = atof((LPCTSTR)szTemp1);
		dsum1 = dsum1 + (dTemp1 - davg)*(dTemp1 - davg);
	}

	if (m_szaSDX2.GetSize() != 0)
		dsd = sqrt(dsum1/(m_szaSDX2.GetSize()-1));

	szMsg.Format("Ysd,%lf",dsd);
	dSD2 = dsd;

//range Y

	for(int n = 0; n < m_szaSDX2.GetSize();n++)
	{	
		szTemp1 = m_szaSDX2.GetAt(n);
		dTemp1 = atof((LPCTSTR)szTemp1);

		if (n == 0)
		{
			dmax = dTemp1;
			dmin = dTemp1;
		}
		else
		{
			if (dTemp1 > dmax)
				dmax = dTemp1;
			if (dTemp1 < dmin)
				dmin = dTemp1;
		}
	}
	szMsg.Format("rangeY,%lf,%lf",dmax,dmin);
	dMax2 = dmax;
	dMin2 = dmin;

	m_szaSDX1.RemoveAll();
	m_szaSDX2.RemoveAll();
	return;
}

VOID CMS896AStn::SD_GetDataAdv(CStringArray &szaData,DOUBLE dX)
{
	CString szTemp;

	szTemp.Format("%lf",dX);
	szaData.Add(szTemp);

	return;
}

VOID CMS896AStn::SD_CalculateAdv(CStringArray &szaData, DOUBLE &dMax, DOUBLE &dMin, DOUBLE &dSD)
{

	double dsum = 0, davg, dsum1,dsd;
	CString szTemp;
	double  dTemp;
	CString szTemp1;
	double  dTemp1;
	CString szMsg;
	double dmax,dmin;
	
	for(int i = 0; i < szaData.GetSize(); i++)
	{
		szTemp = szaData.GetAt(i);
		dTemp = atof((LPCTSTR)szTemp);
		dsum = dsum + dTemp;
	}

	davg = dsum / szaData.GetSize();

	dsum1 = 0;
	for(int k = 0; k < szaData.GetSize();k++)
	{	
		szTemp1 = szaData.GetAt(k);
		dTemp1 = atof((LPCTSTR)szTemp1);
		dsum1 = dsum1 + (dTemp1 - davg)*(dTemp1 - davg);
	}

	if (szaData.GetSize() != 0)
		dsd = sqrt(dsum1/(szaData.GetSize()-1));

	szMsg.Format("Xsd,%lf",dsd);
	dSD = dsd;

//range
	for(int n = 0; n < szaData.GetSize();n++)
	{	
		szTemp1 = szaData.GetAt(n);
		dTemp1 = atof((LPCTSTR)szTemp1);

		if (n == 0)
		{
			dmax = dTemp1;
			dmin = dTemp1;
		}
		else
		{
			if (dTemp1 > dmax)
				dmax = dTemp1;
			if (dTemp1 < dmin)
				dmin = dTemp1;
		}
	}
	szMsg.Format("rangeX,%lf,%lf",dmax,dmin);
	dMax = dmax;
	dMin = dmin;

	szaData.RemoveAll();
	return;
}


VOID CMS896AStn::SaveEjtCollet1Offset(const LONG lEjtCollet1OffsetX, const LONG lEjtCollet1OffsetY, const double dEjtXYRes)
{
	(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["X"]		= lEjtCollet1OffsetX;
	(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["Y"]		= lEjtCollet1OffsetY;

	(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["X_um"]	= lEjtCollet1OffsetX * dEjtXYRes;
	(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["Y_um"]	= lEjtCollet1OffsetY * dEjtXYRes;
}


VOID CMS896AStn::SaveEjtCollet2Offset(const LONG lEjtCollet2OffsetX, const LONG lEjtCollet2OffsetY, const double dEjtXYRes)
{
	(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["X"]		= lEjtCollet2OffsetX;
	(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["Y"]		= lEjtCollet2OffsetY;

	(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["X_um"]	= lEjtCollet2OffsetX * dEjtXYRes;
	(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["Y_um"]	= lEjtCollet2OffsetY * dEjtXYRes;
}


LONG CMS896AStn::GetEjtCollet1OffsetX()
{
	LONG lEjtCollet1OffsetX = (LONG)(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["X"];
	return lEjtCollet1OffsetX;
}


LONG CMS896AStn::GetEjtCollet1OffsetY()
{
	LONG lEjtCollet1OffsetY = (LONG)(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["Y"];
	return lEjtCollet1OffsetY;
}

LONG CMS896AStn::GetEjtCollet2OffsetX()
{
	LONG lEjtCollet2OffsetX = (LONG)(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["X"];
	return lEjtCollet2OffsetX;
}

LONG CMS896AStn::GetEjtCollet2OffsetY()
{
	LONG lEjtCollet2OffsetY = (LONG)(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["Y"];
	return lEjtCollet2OffsetY;
}


VOID CMS896AStn::GetBHMarkOffset_um(double &dBH1MarkOffsetX_um, double &dBH1MarkOffsetY_um, double &dBH2MarkOffsetX_um, double &dBH2MarkOffsetY_um)
{
	//the value unit is the wafer table X&Y count 
	dBH1MarkOffsetX_um = (double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX1_um"];
	dBH1MarkOffsetY_um = (double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY1_um"];

	dBH2MarkOffsetX_um = (double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX2_um"];
	dBH2MarkOffsetY_um = (double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY2_um"];
}


VOID CMS896AStn::GetBHColletHoleOffset_um(double &dBH1ColletHoleOffsetX_um, double &dBH1ColletHoleOffsetY_um, double &dBH2ColletHoleOffsetX_um, double &dBH2ColletHoleOffsetY_um)
{
	//the value unit is the wafer table X&Y count 
	dBH1ColletHoleOffsetX_um = (double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleX1_um"];
	dBH1ColletHoleOffsetY_um = (double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleY1_um"];

	dBH2ColletHoleOffsetX_um = (double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleX2_um"];
	dBH2ColletHoleOffsetY_um = (double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleY2_um"];
}

VOID CMS896AStn::GetColletOffset_um(double &dCollet1OffsetX_um, double &dCollet1OffsetY_um, double &dCollet2OffsetX_um, double &dCollet2OffsetY_um)
{
	//EJT OFfset XY in motor steps; encoder resolution of EJT is same as 
	//	WFT XY and BT XT of 0.5um/motor-step;
	//the value unit is the ejector table X&Y count in ["WaferPr"]["EjtCollet1Offset"]
	dCollet1OffsetX_um	= (double)(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["X_um"];
	dCollet1OffsetY_um	= (double)(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["Y_um"];
	dCollet2OffsetX_um	= (double)(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["X_um"];
	dCollet2OffsetY_um	= (double)(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["Y_um"];
}


VOID CMS896AStn::GetEjtColletOffsetCount(const LONG lEjtX, const LONG lEjtY, LONG& lCollet1OffsetX, LONG& lCollet1OffsetY, LONG& lCollet2OffsetX, LONG& lCollet2OffsetY, const DOUBLE dEJTResolution, const BOOL bColletMarkOffset, const BOOL bBH2, const LONG lFlag)
{
	double dCollet1OffsetX_um = 0, dCollet1OffsetY_um = 0;
	double dCollet2OffsetX_um = 0, dCollet2OffsetY_um = 0;

	GetColletOffset_um(dCollet1OffsetX_um, dCollet1OffsetY_um, dCollet2OffsetX_um, dCollet2OffsetY_um);

	if (bColletMarkOffset)
	{
		double dBH1ColletHoleOffsetX_um = 0, dBH1ColletHoleOffsetY_um = 0;
		double dBH2ColletHoleOffsetX_um = 0, dBH2ColletHoleOffsetY_um = 0;
		GetBHColletHoleOffset_um(dBH1ColletHoleOffsetX_um, dBH1ColletHoleOffsetY_um, dBH2ColletHoleOffsetX_um, dBH2ColletHoleOffsetY_um);

		double dBH1MarkOffsetX_um = 0, dBH1MarkOffsetY_um = 0;
		double dBH2MarkOffsetX_um = 0, dBH2MarkOffsetY_um = 0;

		GetBHMarkOffset_um(dBH1MarkOffsetX_um, dBH1MarkOffsetY_um, dBH2MarkOffsetX_um, dBH2MarkOffsetY_um);

		lCollet1OffsetX = ConvertUMToCount(dCollet1OffsetX_um + dBH1MarkOffsetX_um, dEJTResolution);
		lCollet1OffsetY = ConvertUMToCount(dCollet1OffsetY_um + dBH1MarkOffsetY_um, dEJTResolution);
		lCollet2OffsetX = ConvertUMToCount(dCollet2OffsetX_um + dBH2MarkOffsetX_um, dEJTResolution);
		lCollet2OffsetY = ConvertUMToCount(dCollet2OffsetY_um + dBH2MarkOffsetY_um, dEJTResolution);

		CString szMsg;
		if (bBH2)
		{
			if ((LONG)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej2_CurValueValid"] == 1)
			{
				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej2_CurValueValid"] = 2;
				szMsg.Format("A(%d) -- Update Mark-Offset2(%.1f, %.1f) um, colletHoleOffset2(%.1f,%.1f) um, NewColletOffset2(%.1f, %.1f) um, OriginalColletOffset2(%.1f, %.1f) um, MoveEJTT: XY (%ld, %ld); NewColletOffset2(%ld, %ld), BH-Temp(%.1f)",
					lFlag, dBH2MarkOffsetX_um, dBH2MarkOffsetY_um, 
					dBH2ColletHoleOffsetX_um, dBH2ColletHoleOffsetY_um,
					dCollet2OffsetX_um + dBH2MarkOffsetX_um , dCollet2OffsetY_um + dBH2MarkOffsetY_um,
					dCollet2OffsetX_um, dCollet2OffsetY_um, lEjtX, lEjtY, lCollet2OffsetX, lCollet2OffsetY, m_dBHTThermostatReading);
			}
			else
			{
				szMsg.Format("A(%d) -- Mark-Offset2(%.1f, %.1f) um, colletHoleOffset2(%.1f,%.1f) um, NewColletOffset2(%.1f, %.1f) um, OriginalColletOffset2(%.1f, %.1f) um, MoveEJTT: XY (%ld, %ld); NewColletOffset2(%ld, %ld), BH-Temp(%.1f)",
					lFlag, dBH2MarkOffsetX_um, dBH2MarkOffsetY_um, 
					dBH2ColletHoleOffsetX_um, dBH2ColletHoleOffsetY_um,
					dCollet2OffsetX_um + dBH2MarkOffsetX_um , dCollet2OffsetY_um + dBH2MarkOffsetY_um,
					dCollet2OffsetX_um, dCollet2OffsetY_um, lEjtX, lEjtY, lCollet2OffsetX, lCollet2OffsetY, m_dBHTThermostatReading);
			}
			SaveEJTMark2(szMsg);
		}
		else
		{
			if ((LONG)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej1_CurValueValid"] == 1)
			{
				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej1_CurValueValid"] = 2;
				szMsg.Format("A(%d) -- Update Mark-Offset1(%.1f, %.1f) um, colletHoleOffset1(%.1f,%.1f) um, NewColletOffset1(%.1f, %.1f) um, OriginalColletOffset1(%.1f, %.1f) um, MoveEJTT: XY (%ld, %ld); NewColletOffset2(%ld, %ld), BH-Temp(%.1f)",
					lFlag, dBH1MarkOffsetX_um, dBH1MarkOffsetY_um, 
					dBH1ColletHoleOffsetX_um, dBH1ColletHoleOffsetY_um,
					dCollet1OffsetX_um + dBH1MarkOffsetX_um , dCollet1OffsetY_um + dBH1MarkOffsetY_um,
					dCollet1OffsetX_um, dCollet1OffsetY_um, lEjtX, lEjtY, lCollet1OffsetX, lCollet1OffsetY, m_dBHTThermostatReading);
			}
			else
			{
				szMsg.Format("A(%d) -- Mark-Offset1(%.1f, %.1f) um, colletHoleOffset1(%.1f,%.1f) um, NewColletOffset1(%.1f, %.1f) um, OriginalColletOffset1(%.1f, %.1f) um, MoveEJTT: XY (%ld, %ld); NewColletOffset2(%ld, %ld), BH-Temp(%.1f)",
					lFlag, dBH1MarkOffsetX_um, dBH1MarkOffsetY_um, 
					dBH1ColletHoleOffsetX_um, dBH1ColletHoleOffsetY_um,
					dCollet1OffsetX_um + dBH1MarkOffsetX_um , dCollet1OffsetY_um + dBH1MarkOffsetY_um,
					dCollet1OffsetX_um, dCollet1OffsetY_um, lEjtX, lEjtY, lCollet1OffsetX, lCollet1OffsetY, m_dBHTThermostatReading);
			}
			SaveEJTMark1(szMsg);
		}
	}
}


LONG CMS896AStn::ConvertUMToCount(const double dValue_um, const double dResolution)
{
	return _round(dValue_um / dResolution);
}

DOUBLE CMS896AStn::ConvertCountToUM(const LONG lCount, const double dResolution)
{
	return lCount * dResolution;
}

VOID CMS896AStn::GetColletOffsetCount(LONG& lCollet1OffsetX, LONG& lCollet1OffsetY, LONG& lCollet2OffsetX, LONG& lCollet2OffsetY, const DOUBLE dResultion, const BOOL bColletMarkOffset)
{
	//EJT OFfset XY in motor steps; encoder resolution of EJT is same as 
	//	WFT XY and BT XT of 0.5um/motor-step;
	//the value unit is the ejector table X&Y count in ["WaferPr"]["EjtCollet1Offset"]
	double dCollet1OffsetX_um = 0, dCollet1OffsetY_um = 0;
	double dCollet2OffsetX_um = 0, dCollet2OffsetY_um = 0;

	GetColletOffset_um(dCollet1OffsetX_um, dCollet1OffsetY_um, dCollet2OffsetX_um, dCollet2OffsetY_um);

	double dBH1MarkOffsetX_um = 0, dBH1MarkOffsetY_um = 0;
	double dBH2MarkOffsetX_um = 0, dBH2MarkOffsetY_um = 0;
	
	if (bColletMarkOffset)
	{
		GetBHMarkOffset_um(dBH1MarkOffsetX_um, dBH1MarkOffsetY_um, dBH2MarkOffsetX_um, dBH2MarkOffsetY_um);
	}

	//BT XY encoder resolution is 0.5um/count
	lCollet1OffsetX = ConvertUMToCount(dCollet1OffsetX_um + dBH1MarkOffsetX_um, dResultion);
	lCollet1OffsetY = ConvertUMToCount(dCollet1OffsetY_um + dBH1MarkOffsetY_um, dResultion);
	lCollet2OffsetX = ConvertUMToCount(dCollet2OffsetX_um + dBH2MarkOffsetX_um, dResultion);
	lCollet2OffsetY = ConvertUMToCount(dCollet2OffsetY_um + dBH2MarkOffsetY_um, dResultion);
}

/*
VOID CMS896AStn::SaveBTAdjCollet1Offset(const LONG lBHZ1AdjBondPosOffsetX, const LONG lBHZ1AdjBondPosOffsetY, const double dXYRes)
{
	(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX"]			= (LONG)(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX"] + lBHZ1AdjBondPosOffsetX;
	(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY"]			= (LONG)(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY"] + lBHZ1AdjBondPosOffsetY;

	(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX_um"]		= (DOUBLE)(LONG)(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX_um"] + lBHZ1AdjBondPosOffsetX * dXYRes;
	(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY_um"]		= (DOUBLE)(LONG)(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY_um"] + lBHZ1AdjBondPosOffsetY * dXYRes;
}

VOID CMS896AStn::SaveBTAdjCollet2Offset(const LONG lBHZ2AdjBondPosOffsetX, const LONG lBHZ2AdjBondPosOffsetY, const double dXYRes)
{
	(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"]			= (LONG)(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"] + lBHZ2AdjBondPosOffsetX;
	(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"]			= (LONG)(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"] + lBHZ2AdjBondPosOffsetY;

	(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX_um"]		= (DOUBLE)(LONG)(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX_um"] + lBHZ2AdjBondPosOffsetX * dXYRes;
	(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY_um"]		= (DOUBLE)(LONG)(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY_um"] + lBHZ2AdjBondPosOffsetY * dXYRes;
}
*/

VOID CMS896AStn::SaveBTCollet1Offset(const LONG lBHZ1BondPosOffsetX, const LONG lBHZ1BondPosOffsetY, const double dXYRes)
{
	(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX"]			= lBHZ1BondPosOffsetX;
	(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY"]			= lBHZ1BondPosOffsetY;

	(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX_um"]		= lBHZ1BondPosOffsetX * dXYRes;
	(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY_um"]		= lBHZ1BondPosOffsetY * dXYRes;
}

VOID CMS896AStn::SaveBTCollet2Offset(const LONG lBHZ2BondPosOffsetX, const LONG lBHZ2BondPosOffsetY, const double dXYRes)
{
	(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"]			= lBHZ2BondPosOffsetX;
	(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"]			= lBHZ2BondPosOffsetY;

	(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX_um"]		= lBHZ2BondPosOffsetX * dXYRes;
	(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY_um"]		= lBHZ2BondPosOffsetY * dXYRes;
}

VOID CMS896AStn::SaveBTBond180DegOffset(const double dBond180DegOffsetX_um, const double dBond180DegOffsetY_um)
{
	(*m_psmfSRam)["BinTable"]["Bond180DegOffsetX_um"]		= dBond180DegOffsetX_um;
	(*m_psmfSRam)["BinTable"]["Bond180DegOffsetY_um"]		= dBond180DegOffsetY_um;
}

VOID CMS896AStn::GetBTColletOffset_um(double &dCollet1OffsetX_um, double &dCollet1OffsetY_um, double &dCollet2OffsetX_um, double &dCollet2OffsetY_um)
{
	//EJT OFfset XY in motor steps; encoder resolution of EJT is same as 
	//	WFT XY and BT XT of 0.5um/motor-step;
	//the value unit is the ejector table X&Y count in ["WaferPr"]["EjtCollet1Offset"]
	dCollet1OffsetX_um	= (double)(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX_um"];
	dCollet1OffsetY_um	= (double)(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY_um"];
	dCollet2OffsetX_um	= (double)(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX_um"];
	dCollet2OffsetY_um	= (double)(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY_um"];
}


VOID CMS896AStn::GetBTBond180DegOffset(double &dBond180DegOffsetX_um, double &dBond180DegOffsetY_um)
{
	dBond180DegOffsetX_um = (*m_psmfSRam)["BinTable"]["Bond180DegOffsetX_um"];
	dBond180DegOffsetY_um = (*m_psmfSRam)["BinTable"]["Bond180DegOffsetY_um"];
}

VOID CMS896AStn::GetBTColletOffsetCount(LONG& lCollet1OffsetX, LONG& lCollet1OffsetY, LONG& lCollet2OffsetX, LONG& lCollet2OffsetY, const DOUBLE dResultion, const BOOL bColletMarkOffset)
{
	//EJT OFfset XY in motor steps; encoder resolution of EJT is same as 
	//	WFT XY and BT XT of 0.5um/motor-step;
	//the value unit is the ejector table X&Y count in ["WaferPr"]["EjtCollet1Offset"]
	double dCollet1OffsetX_um = 0, dCollet1OffsetY_um = 0;
	double dCollet2OffsetX_um = 0, dCollet2OffsetY_um = 0;

	GetBTColletOffset_um(dCollet1OffsetX_um, dCollet1OffsetY_um, dCollet2OffsetX_um, dCollet2OffsetY_um);
	BOOL bRotate180 = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["MS90"]["Rotate180"];
	if (IsMS90() && bRotate180)	
	{
		double dBond180DegOffsetX_um = 0, dBond180DegOffsetY_um = 0;
		GetBTBond180DegOffset(dBond180DegOffsetX_um, dBond180DegOffsetY_um);
		dCollet1OffsetX_um += dBond180DegOffsetX_um;
		dCollet1OffsetY_um += dBond180DegOffsetY_um;
		dCollet2OffsetX_um += dBond180DegOffsetX_um;
		dCollet2OffsetY_um += dBond180DegOffsetY_um;
	}

	double dBH1MarkOffsetX_um = 0, dBH1MarkOffsetY_um = 0;
	double dBH2MarkOffsetX_um = 0, dBH2MarkOffsetY_um = 0;
	
	if (bColletMarkOffset)
	{
		GetBHMarkOffset_um(dBH1MarkOffsetX_um, dBH1MarkOffsetY_um, dBH2MarkOffsetX_um, dBH2MarkOffsetY_um);
	}


	//BT XY encoder resolution is 0.5um/count
	lCollet1OffsetX = -ConvertUMToCount(dCollet1OffsetX_um - dBH1MarkOffsetX_um, dResultion);
	lCollet1OffsetY = -ConvertUMToCount(dCollet1OffsetY_um - dBH1MarkOffsetY_um, dResultion);
	lCollet2OffsetX = -ConvertUMToCount(dCollet2OffsetX_um - dBH2MarkOffsetX_um, dResultion);
	lCollet2OffsetY = -ConvertUMToCount(dCollet2OffsetY_um - dBH2MarkOffsetY_um, dResultion);
}
