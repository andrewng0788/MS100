/////////////////////////////////////////////////////////////////
// MS896AStn.h : interface of the MS896AStn class
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

#include "HipecClass.h"
#include "AppStation.h"
#include "StringMapFile.h"
#include "HpMotorSensor.h"
#include "MS896A_Constant.h"
#include <WaferMapWrapper.h>
#include "GenerateDatabase.h"
#include "GenerateWaferDatabase.h"
#include "QueryWaferDatabase.h"
#include "QueryDatabase.h"
#include "PreBondEvent.h"
#include "GemStation.h"
// prescan relative code
#include "PrescanConstant.h"
#include "PrescanUtility.h"
#include <prheader.h>
#include <prmsg.h>
#include <prstatus.h>
#include <prtype.h>
#include "PrGeneral.h"

#ifndef MS50_64BIT
	#include "Lextar.h"
#endif

#include "MS896a_NuMotion.h"

#include "SfmSerialPort.h"
#include "NichiaMgntSys.h"
#include "SemitekMgntSys.h"
#include "WT_Constant.h"
//#include "PstnModel.h"

#ifdef NU_MOTION
#include <gmp_fun_ext.h>
#include <gmp_error.h>
#endif


//=================================
// DEFINE macros (General)
//=================================
#define		MS_MAX_BIN					175		//150			//v3.83
#define		MS_WAFER_LIMIT_POINTS		15
#define		MS_WAFER_CIRCULAR_LIMIT_PTS	3

#define		MS_PR_INIT_SEND_ID			0x40
#define		MS_PR_INIT_RECV_ID			0xa0
#define		MS899_GEN_SEND_ID			0x43
#define		MS899_GEN_RECV_ID			0xa3
#define		MS_MOUSE_CLICK_SEND_ID		0x45
#define		MS_MOUSE_CLICK_RECV_ID		0xa5

//	wafer PR config IDs
#define		MS899_WAF_CAM_SEND_ID		0x41
#define		MS899_WAF_CAM_RECV_ID		0xa1
#define		MS899_WAF_CAM_ID			PR_CAMERA_1
#define		MS899_WAF_GEN_PURPOSE		PR_PURPOSE_CAMERA_1
#define		MS899_WAF_INSP_PURPOSE		PR_WAF_INSP

//	MS Wafer Dual Path 5M	//	camera sender/receiver/purpose ID
#define		MS_DP_WAF_5M_SEND_ID		0x46
#define		MS_DP_WAF_5M_RECV_ID		0xa6
#define		MS_DP_WAF_5M_CAM_ID			PR_CAMERA_6
#define		MS_DP_WAF_5M_GN_PURPOSE		PR_PURPOSE_CAMERA_6
#define		MS_DP_WAF_5M_PR_PURPOSE		PR_PURPOSE_2D_CODE_4

//	bond PR config IDs
#define		MS899_BOND_CAM_SEND_ID		0x42
#define		MS899_BOND_CAM_RECV_ID		0xa2
#define		MS899_BOND_CAM_ID			PR_CAMERA_2
#define		MS899_BOND_GEN_PURPOSE		PR_PURPOSE_CAMERA_2
#define		MS899_BOND_PB_PURPOSE		PR_POST_DIE_INSP
//#define		MS899_BOND_CAM_SEND_ID		0x41
//#define		MS899_BOND_CAM_RECV_ID		0xa1
//#define		MS899_BOND_CAM_ID			PR_CAMERA_1
//#define		MS899_BOND_GEN_PURPOSE		PR_PURPOSE_CAMERA_1
//#define		MS899_BOND_PB_PURPOSE		PR_WAF_INSP

//v3.99T3	//FOr WaferPR & BondPR classes
#define		PSPR_SENDER_ID					0x44
#define		PSPR_RECV_ID					0xa4

#define		MS899_POSTSEAL_CAM_ID			PR_CAMERA_3
#define		MS899_POSTSEAL_GEN_PURPOSE		PR_PURPOSE_CAMERA_3
#define		MS899_POSTSEAL_PR_PURPOSE		PR_PURPOSE_2D_CODE_1

#define		MS899_POSTSEAL_2D_BH1_CAM_ID	PR_CAMERA_4
#define		MS899_POSTSEAL_BH1_GEN_PURPOSE	PR_PURPOSE_CAM_A3
#define		MS899_POSTSEAL_BH1_PR_PURPOSE	PR_PURPOSE_2D_CODE_2

#define		MS899_POSTSEAL_2D_BH2_CAM_ID	PR_CAMERA_5
#define		MS899_POSTSEAL_BH2_GEN_PURPOSE	PR_PURPOSE_CAM_A4
#define		MS899_POSTSEAL_BH2_PR_PURPOSE	PR_PURPOSE_2D_CODE_3

#define		MS899_UPLOOK_REF_DIENO_BH1		11  //4.52D17ID
#define		MS899_UPLOOK_REF_DIENO_BH2		12

//	ES contour camera definition
#define		ES_CONTOUR_CAM_SEND_ID			0x4e
#define		ES_CONTOUR_CAM_RECV_ID			0xae

#define		ES_CONTOUR_WL1_CAM_ID			PR_CAMERA_2
#define		ES_CONTOUR_WL2_CAM_ID			PR_CAMERA_3

#define		ES_CONTOUR_WL1_GN_PURPOSE		PR_PURPOSE_2D_CODE_2
#define		ES_CONTOUR_WL1_PR_PURPOSE		PR_PURPOSE_2D_CODE_2
#define		ES_CONTOUR_WL2_GN_PURPOSE		PR_PURPOSE_2D_CODE_3
#define		ES_CONTOUR_WL2_PR_PURPOSE		PR_PURPOSE_2D_CODE_3

//andrewng //2020-0618
#define NVC_WAF_BURNIN_DIE_PITCH_XX		1000
#define NVC_WAF_BURNIN_DIE_PITCH_XY		0
#define NVC_WAF_BURNIN_DIE_PITCH_YX		0
#define NVC_WAF_BURNIN_DIE_PITCH_YY		1000

enum {	MS_PR_DISPLAY_DEFAULT	= 0,
		MS_PR_DISPLAY_COLLET1	= 1,
		MS_PR_DISPLAY_COLLET2	= 2};

enum {	MS_FRAME_NO_ERR						= 0,
		MS_FRAME_SEARCH_FAILURE				= 1,
		MS_FRAME_NO_BARCODE					= 2,
		MS_FRAME_BARCODE_NOT_MATCHED_SIS	= 3
};

enum {
		SCAN_ALIGN_WAFER_DISABLE = 0,
		SCAN_ALIGN_WAFER_WITHOUT_REF = 1,
		SCAN_ALIGN_WAFER_WITH_REF = 2
	};
//=================================
// STATIC macros/constants (general)
//=================================
// Constants for NVRAM
static const LONG glNVRAM_BinTable_Start	= 0;		// Bin Table start
static const LONG glNVRAM_BinTable_Size		= 16384;	//12288; 	// Bin Table size	//v3.83
static const LONG glNVRAM_WaferMap_Start	= glNVRAM_BinTable_Size;	// Wafer Map start
static const LONG glNVRAM_WaferMap_Size		= 10240;					// Wafer Map size
static const LONG glNVRAM_BondHead_Start	= glNVRAM_BinTable_Size + glNVRAM_WaferMap_Size; //Bond Head start
static const LONG glNVRAM_BondHead_Size		= 1024;						//Bond Head size
static const LONG glNVRAM_WaferTable_Start	= glNVRAM_BinTable_Size + glNVRAM_WaferMap_Size + glNVRAM_BondHead_Size;	//Wafer Table start
static const LONG glNVRAM_WaferTable_Size	= 1024;						//Wafer Table start
//v4.65A1
static const LONG glNVRAM_TEMPDATA_Start	= glNVRAM_BinTable_Size + glNVRAM_WaferMap_Size + glNVRAM_BondHead_Size + glNVRAM_WaferTable_Size + 8;
static const LONG glNVRAM_TEMPDATA_Size		= 6020;						//TEMP Data sizestatic const LONG glNVRAM_WaferTable_Size	= 1024;						//Wafer Table size

//=================================
// BL related Definitions
//=================================
//v4.39T7	..Moved from BL to base class here
// definition for BL magazine data struct
#define MS_BL_MGZN_NUM				8		//6		//v3.82
#define MS_BL_MGZN_SLOT				25
#define MS_BL_MAX_MGZN_SLOT			29		//New special slots for Cree HuiZhou	//v4.19
#define MS_BL_MGZ_MIN_PITCH			5000
#define MS_BL_MGZ_MAX_PITCH			35000

typedef struct
{
	LONG    m_lTopLevel;
	LONG    m_lTopLevel2;
	LONG    m_lMidPosnY;
	LONG    m_lMidPosnY2;
	LONG    m_lMidPosnZ1;						//andrewng //2020-0908
	LONG    m_lMidPosnZ2;						//andrewng //2020-0908
	LONG    m_lSlotPitch;
	LONG    m_lNoOfSlots;						// not really used indeed, only get/set
	LONG    m_lMgznState;						// state can be ok=0, full=1, empty = 2.
	LONG    m_lMgznUsage;						// this magazine is used as full, empty or active or mixed or unused
	LONG    m_lSlotBlock[MS_BL_MAX_MGZN_SLOT];	// -1, no bin frame, 0 no die bin frame, 1--... bonded 
	LONG    m_lSlotWIPCounter[MS_BL_MAX_MGZN_SLOT];	//-1: Empty Bin / full wafer
												// Bin WIP: (full bin number ?current bin number)
												// Wafer WIP: current remaining die number
	LONG    m_lSlotUsage[MS_BL_MAX_MGZN_SLOT];	// full, empty, active(buffer)
	LONG    m_lTransferSlotUsage[MS_BL_MAX_MGZN_SLOT];	// full, empty, active(buffer)
	CString m_SlotBCName[MS_BL_MAX_MGZN_SLOT];	// barcode name(with ext)
	CString m_SlotSN[MS_BL_MAX_MGZN_SLOT];		// Slot SN
	CString m_SlotLotNo[MS_BL_MAX_MGZN_SLOT];	//Lot No
} BL_MAGAZINE;

//=================================
// Prober related Definitions
//=================================

typedef struct
{
	LONG	lX;
	LONG	lY;
	LONG	lZ;
} PROBE_TABLE_LEVEL_DATA;

typedef struct
{
	LONG lX;
	LONG lY;

} PROBE_TABLE_LEARN_POINT;




//////////////////////////////////////////////////////////////
// CLASS Definitions
//////////////////////////////////////////////////////////////
class CMS896AStn : public CAppStation  
{
	DECLARE_DYNCREATE(CMS896AStn)

public:

	//======================================================================
	// Class Constructor/Destructor Fcns
	//======================================================================
	CMS896AStn();
	virtual	~CMS896AStn();
	virtual	BOOL	InitInstance();
	virtual	INT		ExitInstance();

	VOID NeedReset(BOOL bSet);
	BOOL NeedReset();
	VOID SetComplete(BOOL bState = TRUE);
	virtual BOOL IsComplete(INT nTimeout = LOCK_TIMEOUT);
	virtual BOOL InitData();			// Init the data from SMF and NVRAM
	virtual VOID ClearData();			// Clear data for cold-start
	virtual VOID FlushMessage();		// Flush IPC message
	virtual VOID UpdateStationData();
	virtual VOID ReloadMachineConstant();
	virtual VOID ExportMachineConstant();
	virtual VOID ImportMachineConstant();

	//==============================
	// Public Status-Checking Fcns
	//==============================
	BOOL	IsHardwareReady();		// Check whether the hardware is ready
	BOOL	AllowMove();			// For step move
	BOOL	IsWprWithAF();
	BOOL	IsBLEnable();
	BOOL	IsNLEnable();			//andrewng //2020-0720
	BOOL	IsEnableWL();
	BOOL	IsAOIOnlyMachine();
	BOOL	IsOcrAOIMode();

	BOOL	GetPsmEnable();
	BOOL	GetScnEnable();	// to generate SCN file or not, default is enable.
	BOOL	IsCharDieInUse();
	BOOL	IsPrescanEnable();
	BOOL	IsEnablePNP();
	BOOL	IsOnlyRegionEnd();
	BOOL	IsPrescanning();
	BOOL	IsPrescanEnded();
	BOOL	IsPrescanMapIndex();
	BOOL	IsFastHomeScan();
	BOOL	IsPrescanBlkPick();
	BOOL	IsBlkPickAlign();
	LONG	IsScanAlignWafer();
	BOOL	IsMS90HalfSortDummyScanAlign();
	BOOL	IsScanWithWaferLimit();
	BOOL	IsScanThenDivideMap();
	BOOL	IsErrMapInUse();			//andrewng //2020-0805
	BOOL	IsBlkFuncEnable();
	BOOL	IsBlkFunc1Enable();
	BOOL	IsBlkFunc2Enable();
	BOOL	IsEnableFPC();
	BOOL	IsScanMapNgGrade(CONST UCHAR ucMapGrade);
	BOOL	IsScanMapAOINgGrade(CONST UCHAR ucMapGrade);
	BOOL	IsDieUnpickSort(CONST ULONG ulDieState);
	BOOL	IsDieUnpickScan(CONST ULONG ulDieState);
	ULONG	GetMapDieState(ULONG ulRow, ULONG ulCol);
	BOOL	IsScanEmptyGrade(ULONG ulRow, ULONG ulCol);
	BOOL	IsScanBadCutGrade(ULONG ulRow, ULONG ulCol);
	BOOL	IsScannedDefectGrade(ULONG ulRow, ULONG ulCol);
	BOOL	IsInSelectedGrades(ULONG ulRow, ULONG ulCol);
	BOOL	IsDieUnpickAll(CONST ULONG ulDieState);
	BOOL	IsDieEmpty(CONST ULONG ulDieState);

	BOOL	Is180Arm6InchWT();
	VOID	UpdateBackLightDownState();
	VOID	ObtainMapValidRange();
	BOOL	IsInMapValidRange(ULONG ulRow, ULONG ulCol);
	BOOL	SaveMapBitmapImage(CString szMapImagePath);
	BOOL	IsScnLoaded();
	BOOL	IsWLManualMode();
	BOOL	IsWLSemiAutoMode();
	BOOL	IsWLAutoMode();

	UCHAR	GetScanDefectGrade();
	UCHAR	GetScanBadcutGrade();

	static BOOL	CopyFileWithQueue(LPCTSTR szSourceFile, LPCTSTR szTargetFile, BOOL bDelSrcAtLast);
	BOOL	CopyFileWithRetry(LPCTSTR szSourceFile, LPCTSTR szTargetFile, BOOL bDelSrcAtLast);
	BOOL	RenameFile(LPCTSTR szSourceFile, LPCTSTR szTargetFile);	//	must be same folder

	BOOL	GetIPCReply(int nConvID, LONG &lRet);
	BOOL	GetIPCReplyMsg(int nConvID, IPC_CServiceMessage &stMsg);
	BOOL	SendGeneralRequest(const CString mszRecvStationName, const CString szFuncName, const BOOL bInValue);
	LONG	_round(double val);
	LONG	GetCurTime();
	LONG	elapse_time(LONG lStartTime);
	LONG	GetTimeInterval(const LARGE_INTEGER &StartTime, LARGE_INTEGER &EndTime);
	LONG	GetTimeInterval(const LARGE_INTEGER &StartTime);

	VOID	IncreaseRotateDieCounter();
	VOID	IncreaseEmptyDieCounter();
	VOID	IncreaseMissingDieCounter();
	VOID	IncreasePBIShiftCounter();
	VOID	IncreasePBIAngleCounter();
	BOOL	UserHmiUpdateData(const CString &strVariableName, long input);
	BOOL	UserHmiNumericKeys(const CString &strTitle, const CString &strVariableName, double maxv, double minv, long *input);
	VOID	WriteParameterLog(const BOOL bHmiOperationLogOption, CString szMessage);

	BOOL	IsPathExist(const CString szPath);
	BOOL	IsFileExist(const CString szFile);

	VOID	SetPsmEnable(BOOL bEnable);
	VOID	LaserTestOuputBitTrigger(BOOL bSet, DWORD dDelay = 100);
	VOID	SetSuckingHead(BOOL bSet);				//v4.05		//MS100 BH oil leaking problem
	BOOL	LoadWaferMap(LPCTSTR strFileName, LPCTSTR strFormatName, const BOOL bMenuLoad = FALSE);
	BOOL	IsWaferMapValid();
	CString GetMapFileName();
	CString	GetMapNameOnly();
	// to the dispaly on HMI but user value
	BOOL	ConvertAsmToHmiUser(ULONG ulAsmRow, ULONG ulAsmCol, LONG &lHmiRow, LONG &lHmiCol);
	BOOL	ConvertHmiUserToAsm(LONG lHmiRow, LONG lHmiCol, ULONG &ulAsmRow, ULONG &ulAsmCol);
	// to original map file value
	static BOOL	ConvertAsmToOrgUser(ULONG ulAsmRow, ULONG ulAsmCol, LONG &lUserRow, LONG &lUserCol);
	static BOOL	ConvertOrgUserToAsm(LONG lUserRow, LONG lUserCol, ULONG &ulAsmRow, ULONG &ulAsmCol);
	BOOL	GetMapPhyPosn(ULONG ulRow, ULONG ulCol, LONG &lPhyX, LONG &lPhyY);
	BOOL	GetPrescanWftPosn(ULONG ulRow, ULONG ulCol, LONG &lPhyX, LONG &lPhyY);
	BOOL	GetWftPosn(BOOL bByMap, LONG ulRow, LONG ulCol, LONG &lPhyX, LONG &lPhyY);
	BOOL	GetDieValidInX(LONG ulIntRow, LONG ulIntCol, LONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap);
	BOOL	GetDieValidPrescanPosn(LONG ulIntRow, LONG ulIntCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap = TRUE);
	BOOL	PredictGetDieValidPosn(LONG ulIntRow, LONG ulIntCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap);
	BOOL	FindValidMapDieInBlock(ULONG ulLoop, ULONG &ulOutRow, ULONG &ulOutCol);
	BOOL	GetValidMapDieInBlock(SHORT sGrade, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol, ULONG &ulOutRow, ULONG &ulOutCol);
	BOOL	CheckAlignMapWaferMatchHasDie(LONG lRow, LONG lCol);
	BOOL	CheckValidDieInBlock(LONG lRow, LONG lCol, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol, SHORT sGetGrade = -1);
	BOOL	IsWithinMapLimit(LONG lRow, LONG lCol, LONG lMinRow, LONG lMinCol, LONG lMaxRow, LONG lMaxCol, DOUBLE dRatio = 1.0);
	BOOL	IsWithinValidMapLimit(LONG lRow, LONG lCol, DOUBLE dRatio = 1.0);
	BOOL	ObtainEmcoreBarColumnStart();
	BOOL	IsMapDetectSkipMode();
	LONG	GetMapValidMinRow();
	LONG	GetMapValidMinCol();
	LONG	GetMapValidMaxRow();
	LONG	GetMapValidMaxCol();
	VOID	GetMapValidSize(ULONG &ulMapValidMaxRow, ULONG &ulMapValidMinRow,
							ULONG &ulMapValidMaxCol, ULONG &ulMapValidMinCol);
	VOID	GetMapValidSize(LONG &lMaxRow, LONG &lMinRow,
							LONG &lMaxCol, LONG &lMinCol);
	BOOL	CheckDieAssignedAround(ULONG ulIntRow, ULONG ulIntCol, ULONG ulLoop, LONG lChkX, LONG lChkY, BOOL bByMap = TRUE);
	BOOL	GetDieInvalidNearMapPosn(ULONG ulIntRow, ULONG ulIntCol, ULONG ulLoop, LONG &lRow, LONG &lCol, BOOL bByMap = TRUE);
	BOOL	GetDieValidNearMapPosn(ULONG ulIntRow, ULONG ulIntCol, ULONG ulLoop, LONG &lRow, LONG &lCol, BOOL bByMap);
	VOID	ResetMapPhyPosn();
	UCHAR	GetMapGrade(const ULONG ulRow, const ULONG ulCol);
	BOOL	PeekMapDie(ULONG ulSkipDice, ULONG &ulRow, ULONG &ulCol, UCHAR &ucGrade, 
						WAF_CDieSelectionAlgorithm::WAF_EDieAction& eAction, BOOL &bEndOfWafer);
	BOOL	GetMapDie(ULONG &ulRow, ULONG &ulCol, UCHAR &ucGrade,
						WAF_CDieSelectionAlgorithm::WAF_EDieAction &eAction, BOOL &bEndOfWafer);
	BOOL	UpdateMapDie(const ULONG ulRow, const ULONG ulCol, const UCHAR ucGrade, const WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction, 
			const ULONG ulDieState=0);

	CString GetMachineNo();
	//==============================
	// sort mode
	//==============================
	BOOL	IsMS90Sorting2ndPart();
	BOOL	IsSortTo2ndPart();
	BOOL	IsAllSorted();
	BOOL	IsSorraSortMode();
	VOID	DWTDSortLog(CString szMsg);
	BOOL	IsRowModeSeparateHalfMap();
	BOOL	IsMS90HalfSortMode();
	BOOL	Is2PhasesSortMode();
	BOOL	IsOutMS90SortingPart(ULONG ulRow, ULONG ulCol);
	ULONG	GetMS90HalfMapMaxRow();
	ULONG	GetMS90HalfMapMaxCol();
	BOOL	IsMS90BTRotated();
	BOOL	RotateBinTable180(LONG lOldX, LONG lOldY, LONG &lNewX, LONG &lNewY);

	BOOL	IsOsramResortMode();			//Osram Penang RESORT mode	//v4.21T3

	BOOL	IsESMachine();
	BOOL	IsES101();
	BOOL	IsWT2InUse();
	VOID	SetWT2InUse(CONST BOOL bWT2InUse);
	BOOL	IsESDualWT();
	BOOL	IsESDualWL();
	BOOL	IsES201();
	BOOL	IsBLInUse();
	BOOL	IsEjtElvtInUse();
	VOID	GetHomeDiePhyPosn(LONG &lX, LONG &lY);
	VOID	GetWT2HomeDiePhyPosn(LONG &lX, LONG &lY);
	BOOL	IsWLExpanderOpen();

	BOOL	IsMS50();				//v4.59A41
	BOOL	IsMS90();				//v4.48A27
	BOOL	IsMS60();				//v4.47T1
	BOOL	IsProber();
	BOOL	IsBurnInCP();
	BOOL	IsBurnIn();
	BOOL	IsMotionCE();			// Motion system critical error, need to reset/home again.
	VOID	SetMotionCE(CONST BOOL bSet=TRUE, CONST CString szMsg="");			//v4.59A19
	BOOL	IsMSAutoLineMode();
	BOOL	IsMSAutoLineStandloneMode();
	BOOL	IsTableXYMagneticEncoder();
	BOOL	IsBinTableTHighResolution();
	BOOL	IsNewAutoCleanColletSystem();
	BOOL	IsWaferTableWithExpander();
	LONG	GetCGMaxIndex();

	//	Thermal Control Functions
	BOOL	OpUpdateBHTThermalRMSValues(INT nFrequency, BOOL bLog=FALSE);
	BOOL	BH_EnableThermalControl(BOOL bEnable);
	BOOL	EnableBHTThermalControl(BOOL bEnable, BOOL bUseBHFan=FALSE);		//v4.49A5
	BOOL	IsMS60TempertureOverLoaded(BOOL bAuto=FALSE, BOOL bCheckNow=FALSE);	//v4.49A5
	BOOL	IsMS60TempertureOverHeated(CONST ULONG ulTemperature);				//v4.49A18
	VOID 	SetBondHeadFan(BOOL bOn);				//v4.09

	VOID WT_SpecialLog(CString szLogMsg);

	LONG GetScanErrorCode(LONG lScanErrorType);
	LONG GetDiePitchX_X();
	LONG GetDiePitchX_Y();
	LONG GetDiePitchY_Y();
	LONG GetDiePitchY_X();
	LONG GetDieSizeX();
	LONG GetDieSizeY();
	LONG GetBarPitchX();
	LONG GetBarPitchY();

	LONG GetAlignFrameRow();
	LONG GetAlignFrameCol();
	LONG GetScanLastPosnX();
	LONG GetScanLastPosnY();
	LONG GetPrescanPitchX();
	LONG GetPrescanPitchY();
	LONG GetMapIndexStepRow();
	LONG GetMapIndexStepCol();
	LONG GetLastFrameRow();
	LONG GetLastFrameCol();
	BOOL GetMapIndexTolerance(DOUBLE &dRowTol, DOUBLE &dColTol);

	//==============================
	// Temperature Controller fcns
	//==============================
	// ejector vacuum on only when ejector up, after up and heating time,
	// trigger bond head down to pick
	BOOL IsEjtUpToHeatLayer();
	// before down bond head z to pick, after check jam die, turn off collet vacuum.
	// after collet on layer(ejector up already), before begin pick delay, turn on collet vacuum
	BOOL IsBHZOnVacuumOnLayer();
	// TO grab after ejector up firstly and heating complete,
	// get the x and y offset to update bin table index, will not compensation as ejector is up
	// if look forward enable, need to get previous image that is normal for compensation.
	BOOL IsEjtUpAndPreheat();
	BOOL IsLayerPicking();
	BOOL IsBLBCRepeatCheckFail();

	//==============================
	// Other Public Utility Fcns
	//==============================
	LONG DispQuestion(char *format, ...);
	LONG DispQuestion(CString str, CString szTitle = "");
	VOID DispMsg(char *szMsg, LONG lTop = 400, LONG lLeft = 300);
	VOID DispMsg(CString szMsg, LONG lTop = 400, LONG lLeft = 300);

	static	BOOL	m_bAlarmTwice;
	static	BOOL	m_bEnableAlarmLog;
	static	BOOL	m_bACF_AlarmLogPath;
	static	CString	m_szAlarmLogPath;
	BOOL PrintPartScreen(CString szImageFile, LONG lULX, LONG lULY, LONG lLRX, LONG lLRY);
	BOOL PrintScreen(CString szImageFile);
	LONG HmiMessage_Red_Red(const CString &szText,
							const CString &szTitle = _T(""),
							LONG lType = glHMI_MBX_CLOSE,
							LONG lAlignment = glHMI_ALIGN_CENTER,
							LONG lTimeout = 36000000,
							LONG lMsgMode = glHMI_MSG_MODAL,
							LONG *plMsgID = NULL,
							LONG lWidth = -1,
							LONG lHeight = -1,
							CString *pszBitmapFile = NULL,
							CString *pszCustomText1 = NULL,
							CString *pszCustomText2 = NULL,
							CString *pszCustomText3 = NULL,
							LONG lTop = -1,
							LONG lLeft = -1);
	LONG HmiMessage_Red_Yellow(const CString &szText,
							   const CString &szTitle = _T(""),
							   LONG lType = glHMI_MBX_CLOSE,
							   LONG lAlignment = glHMI_ALIGN_CENTER,
							   LONG lTimeout = 36000000,
							   LONG lMsgMode = glHMI_MSG_MODAL,
							   LONG *plMsgID = NULL,
							   LONG lWidth = -1,
							   LONG lHeight = -1,
							   CString *pszBitmapFile = NULL,
							   CString *pszCustomText1 = NULL,
							   CString *pszCustomText2 = NULL,
							   CString *pszCustomText3 = NULL,
							   LONG lTop = -1,
							   LONG lLeft = -1);
	LONG HmiMessage_Red_Back(const CString &szText,
							 const CString &szTitle = _T(""),
							 LONG lType = glHMI_MBX_CLOSE,
							 LONG lAlignment = glHMI_ALIGN_CENTER,
							 LONG lTimeout = 36000000,
							 LONG lMsgMode = glHMI_MSG_MODAL,
							 LONG *plMsgID = NULL,
							 LONG lWidth = -1,
							 LONG lHeight = -1,
							 CString *pszBitmapFile = NULL,
							 CString *pszCustomText1 = NULL,
							 CString *pszCustomText2 = NULL,
							 CString *pszCustomText3 = NULL,
							 LONG lTop = -1,
							 LONG lLeft = -1);
	LONG HmiMessage_Red_Green(const CString &szText,
							  const CString &szTitle = _T(""),
							  LONG lType = glHMI_MBX_CLOSE,
							  LONG lAlignment = glHMI_ALIGN_CENTER,
							  LONG lTimeout = 36000000,
							  LONG lMsgMode = glHMI_MSG_MODAL,
							  LONG *plMsgID = NULL,
							  LONG lWidth = -1,
							  LONG lHeight = -1,
							  CString *pszBitmapFile = NULL,
							  CString *pszCustomText1 = NULL,
							  CString *pszCustomText2 = NULL,
							  CString *pszCustomText3 = NULL,
							  LONG lTop = -1,
							  LONG lLeft = -1);
	LONG HmiMessageEx_Timeout_Red_Back(const CString &szText,
									   const CString &szTitle = _T(""),
									   LONG lType = glHMI_MBX_CLOSE,
									   LONG lAlignment = glHMI_ALIGN_CENTER,
									   LONG lTimeout = 36000000,
									   LONG lMsgMode = glHMI_MSG_MODAL,
									   LONG *plMsgID = NULL,
									   LONG lWidth = -1,
									   LONG lHeight = -1,
									   CString *pszBitmapFile = NULL,
									   CString *pszCustomText1 = NULL,
									   CString *pszCustomText2 = NULL,
									   CString *pszCustomText3 = NULL,
									   LONG lTop = -1,
									   LONG lLeft = -1);
	LONG HmiMessageEx_Red_Back(const CString &szText,
							   const CString &szTitle = _T(""),
							   LONG lType = glHMI_MBX_CLOSE,
							   LONG lAlignment = glHMI_ALIGN_CENTER,
							   LONG lTimeout = 36000000,
							   LONG lMsgMode = glHMI_MSG_MODAL,
							   LONG *plMsgID = NULL,
							   LONG lWidth = -1,
							   LONG lHeight = -1,
							   CString *pszBitmapFile = NULL,
							   CString *pszCustomText1 = NULL,
							   CString *pszCustomText2 = NULL,
							   CString *pszCustomText3 = NULL,
							   LONG lTop = -1,
							   LONG lLeft = -1);
	BOOL SetAlert_Red_Yellow(UINT unCode);
	BOOL SetAlert_NoMaterial(UINT unCode);			//v4.59A16	//Renesas MS90
	BOOL SetAlert_SE_Red_Yellow(UINT unCode);
	BOOL SetAlert_Yellow_Green(UINT unCode);		//4.51D8
	BOOL SetAlert_Red_Back(UINT unCode);
	virtual LONG	SetAlert_Msg_Red_Yellow(UINT unCode, CString &szMsg, 
											CString szBtnText1 = "", 
											CString szBtnText2 = "", 
											CString szBtnText3 = "", 
											LONG lAlign = glHMI_ALIGN_CENTER);
	virtual LONG	SetAlert_Msg_NoMaterial(UINT unCode, CString &szMsg,	//v4.59A16	//Renesas MS90
											CString szBtnText1 = "", 
											CString szBtnText2 = "", 
											CString szBtnText3 = "", 
											LONG lAlign = glHMI_ALIGN_CENTER);

	virtual LONG	SetAlert_Msg_Red_Back(UINT unCode, CString &szMsg, 
										  CString szBtnText1 = "", 
										  CString szBtnText2 = "", 
										  CString szBtnText3 = "", 
										  LONG lAlign = glHMI_ALIGN_CENTER);
	int AfxMessageBox_Red_Back(LPCTSTR lpszText, UINT nType = MB_OK, UINT nIDHelp = 0);

	BOOL HmiStrInputKeyboard(CString szTitle, CString &szContent);
	BOOL UpdateHmiVariableData(CString szVariableName, CString szValue);
	BOOL CheckOperatorPassWord(CString szPassword);
	BOOL Check201277PasswordToGo();

	VOID SetErrorMessage(const CString &szStatus);

	//v4.51A17	//updated to PUBLIC members
	//v4.22T1	//andrew	//BLBarcode MSD file access now moved to main station with Critical Section protected
	BOOL ValidateBLBarcodeData();
	BOOL SaveBLBarcodeData(CONST LONG lBlkNo, CONST CString szBarcode, CONST LONG lMgzNo, CONST LONG lSlotNo);
	CString GetBLBarcodeData(CONST LONG lBlkNo);
	BOOL WriteUnpickInfo(const unsigned long ulUnpickType, long lrow, long lcol);

	virtual VOID UpdateAllSGVariables();
	virtual BOOL UpdateSGEquipmentConst(INT nECID);
	CMS896AStn *GetStation(CString szStationName);

	VOID SetAlert_WarningMsg(const UINT unCode, const CString szMsg);
	VOID SetEquipState(LONG lEquipState);
	VOID SetProcessState(SHORT hProcessState);
	VOID SetEquipmentManagementInfo(BOOL bSend);			//v4.31T9	//EquipMgr
protected:

	//State Operation
	virtual	VOID	IdleOperation();
	virtual	VOID	DiagOperation();
	virtual	VOID	InitOperation();
	virtual	VOID	PreStartOperation();
	virtual	VOID	AutoOperation();
	virtual	VOID	DemoOperation();
	virtual VOID	CycleOperation();
	virtual	VOID	ManualOperation();
	virtual	VOID	ErrorOperation();
	virtual	VOID	StopOperation();
	virtual VOID	DeInitialOperation();
	//Error
	virtual BOOL	SetAlert(UINT unCode);
	virtual BOOL	SetError(UINT unCode);
	virtual LONG	SetAlert_Msg(UINT unCode, CString &szMsg, 
								 CString szBtnText1 = "", 
								 CString szBtnText2 = "", 
								 CString szBtnText3 = "", 
								 LONG lAlign = glHMI_ALIGN_CENTER, LONG lTop = -1, LONG lLeft = -1);
//	VOID SetAlert_WarningMsg(const CString szMsg);
	LONG	SetAlert_NoMsg(UINT unCode, CString &szMsg);
	virtual BOOL	WaitingSetAlertToSanAnEngineerAlarm(CString szCode); // v4.51D5 SanAn(XA) 17
	virtual BOOL Signal(INT nSignal);	//Signalling

	// Stop the cycle
	BOOL SignalInform(CString szStation, INT nSignal);
	VOID StopCycle(CString szStation);
	BOOL SetHouseKeepingToken(CString szMessage, BOOL bStatus);
	BOOL GetHouseKeepingToken(CString szMessage);

	VOID GetAlarmCodeAction(CString szAlarmCode, LONG &lAction);

	virtual VOID RegisterVariables();	//Register Variables

	VOID BackupToNVRAM();				// Backup to NVRAM
	VOID MotionErrorBackupNVRam();
	BOOL InitWaferMap();				// Init the wafer map object
	VOID InitOutputFile();				// Init the Output File related objects
	BOOL InitNichiaSubSystem();			//v4.40T1
	VOID SetFeatureStatus();			//Set Application Features information

	VOID DisplaySequence(const CString &szMessage);
	VOID DisplayMessage(const CString &szMessage);
	VOID SetAutoCycleLog(CONST BOOL bEnable);

	// Set the status message for the screen and log error message into file
	VOID SetStatusMessage(const CString &szStatus);
	VOID SetLogItems(CString szStatus);
	VOID WftToSafeLog(const CString &szStatus, BOOL bLogNow=FALSE);
	VOID SetCtmLogMessage(const CString &szStatus);
	VOID SetAlarmLog(const CString &szStatus);
	VOID LogCycleStopState(CString szLogMessage);
	VOID AlarmLampStatusChangeLog(LONG lAlarmOrgStatus, LONG lAlarmNewStatus, CString szAlarmCode);

	//==============================
	// Alarm tower fcns
	//==============================
	VOID SetAlarmLamp_Red_Only(BOOL bBlink, BOOL bLockAlarmLamp);
	//VOID SetAlarmLamp_Yellow_Only(BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_RedYellow_Only(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_Red(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_Green(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_Yellow(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_RedYellow(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_NoMaterial(BOOL bBlink, BOOL bLockAlarmLamp);		//v4.59A16	//Renesas MS90
	VOID SetAlarmLamp_YellowGreen(BOOL bBlink, BOOL bLockAlarmLamp);	//4.51D8
	VOID SetAlarmLamp_YellowGreenNoBuzzer(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_OffLight(BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_YellowBuzzer(BOOL bBlink, BOOL bLockAlarmLamp);
	VOID SetAlarmLamp_Back(LONG lOrgStatus, BOOL bBlink, BOOL bLockAlarmLamp);
	LONG GetAlarmLamp_Status();
	VOID SetAlarmLamp_Status(LONG lLevel, BOOL bBlink, BOOL bLockAlarmLamp);

	VOID SetLampDirect(LONG lLevel, BOOL bLockAlarmLamp);

	//Get Machine config file channel information
	LONG GetChannelInformation(const CString &szChannel, const CString &szData);
	LONG GetChannelInformation(CStringMapFile *psmf, const CString &szChannel, const CString &szData);
	DOUBLE GetChannelResolution(const CString &szChannel);

	// Show and close the loading alert box
	VOID StartLoadingAlert();
	VOID CloseLoadingAlert();
	VOID OpenWaitingAlert();
	VOID CloseWaitingAlert();
	BOOL SetProgressBarLimit(LONG lLimit);
	BOOL UpdateProgressBar(LONG lPercent);

	// For machine set time
	BOOL SetAlertTime(BOOL bStart, UCHAR ucType, CString szCode, CString szContent = "");

	//==============================
	// Status checking
	//==============================
	LONG GetWft1CenterX(BOOL bIsPosOffset = FALSE);
	LONG GetWft1CenterY(BOOL bIsPosOffset = FALSE);
	LONG GetWft2CenterX(BOOL bIsPosOffset = FALSE);
	LONG GetWft2CenterY(BOOL bIsPosOffset = FALSE);

	LONG GetWaferCenterX();
	LONG GetWaferCenterY();
	LONG GetWaferDiameter();
	BOOL IsLoadingPKGFile();
	BOOL IsLoadingPortablePKGFile();
	BOOL IsWithinInputWaferLimit(LONG lX, LONG lY, DOUBLE dScale = 1);
	BOOL IsWithinWT1WaferLimit(LONG lX, LONG lY, DOUBLE dScale = 1, BOOL bIsPosOffset = FALSE);
	BOOL IsWithinWT2WaferLimit(LONG lX, LONG lY, DOUBLE dScale = 1, BOOL bIsPosOffset = FALSE);
	BOOL IsInsidePolygon(LONG lX, LONG lY);
	BOOL IsWithinBinTableLimit(LONG lX, LONG lY);
	BOOL IsWithinBinTable2Limit(LONG lX, LONG lY);
	DOUBLE	GetDistance(DOUBLE pt1x, DOUBLE pt1y, DOUBLE pt2x, DOUBLE pt2y);
	BOOL IsScanNgPick();
	LONG GetScanAction();
	BOOL IsBHStopped();
	BOOL IsBHAtPrePick();
	BOOL IsWaferEnded();

	//==============================
	// Other Utility functions
	//==============================
	LONG GetDefaultMachineDelay(const CString &szChannel, const CString &szData);	//v3.62
	VOID CheckResult(const INT nResult, const CString &szMsg);	// If the given result is not gnOK, throw exception
	BOOL RemoveFilesInFolder(CString szFilePath);
	BOOL LogWaferEndInformation(LONG lSlot = 0, CString szBarcodeName = "");
	BOOL SaveTheWaferEndTime();
	VOID SetMeasurementUnit(const CString &szMeasurementUnit);
	CString GetMeasurementUnit();
	LONG ConvertFileUnitToDisplayUnit(DOUBLE dValue);
	DOUBLE ConvertDFileUnitToDisplayUnit(DOUBLE dValue);		//v4.59A19
	LONG ConvertDisplayUnitToFileUnit(DOUBLE dValue);
	DOUBLE ConvertDisplayUnitToDFileUnit(DOUBLE dValue);		//v4.59A19
	VOID CalculateProfileTime(const CString &szProfile, LONG lRel_Dist, LONG &lTime);	// Calculate the profile time

	LONG HmiMessageEx(const CString &szText,
					  const CString &szTitle = _T(""),
					  LONG lType = glHMI_MBX_CLOSE,
					  LONG lAlignment = glHMI_ALIGN_CENTER,
					  LONG lTimeout = 36000000,
					  LONG lMsgMode = glHMI_MSG_MODAL,
					  LONG *plMsgID = NULL,
					  LONG lWidth = -1,
					  LONG lHeight = -1,
					  CString *pszBitmapFile = NULL,
					  CString *pszCustomText1 = NULL,
					  CString *pszCustomText2 = NULL,
					  CString *pszCustomText3 = NULL,
					  LONG lTop = -1,
					  LONG lLeft = -1);

	//v4.40T6	//Nichia MS100+
	BOOL InitNichiaWafIDList(CONST LONG lMaxWaferIDs);
	BOOL AddNichiaWafIDList(CONST ULONG ulBlkID, CONST CString szWaferID);
	BOOL CheckIfNichiaWafIDListExceedLimit(CONST ULONG ulBlkID, CONST BOOL bIsUnload=FALSE);
	BOOL ResetNichiaWafIDList(CONST ULONG ulBlkID);


	//======================================================================
	// SECSGEM station cmd fcns
	//======================================================================

	// Init the gem station object
	BOOL IsSecsGemInit(VOID);
	BOOL IsSecsGemStarted(VOID);

	// set/get gem station object value
	// CSTRING
	VOID SetGemValue(CString szVName, CString szValue);
	VOID SetGemValue(INT nVID, CString szValue);
	VOID GetGemValue(CString szVName, CString &szValue);
	VOID GetGemValue(INT nVID, CString &szValue);
	// CHAR/UCHAR
	VOID SetGemValue(CString szVName, CHAR cValue);
	VOID SetGemValue(INT nVID, CHAR cValue);
	VOID SetGemValue(CString szVName, UCHAR ucValue);
	VOID SetGemValue(INT nVID, UCHAR ucValue);
	VOID GetGemValue(CString szVName, UCHAR &ucValue);
	VOID GetGemValue(INT nVID, UCHAR &ucValue);
	// INT
	VOID SetGemValue(CString szVName, INT iValue);
	VOID SetGemValue(INT nVID, INT iValue);
	VOID GetGemValue(CString szVName, INT &iValue);
	VOID GetGemValue(INT nVID, INT &iValue);
	// UINT
	VOID SetGemValue(CString szVName, UINT uiValue);
	VOID SetGemValue(INT nVID, UINT uiValue);
	VOID GetGemValue(CString szVName, UINT &uiValue);
	VOID GetGemValue(INT nVID, UINT &uiValue);
	// LONG
	VOID SetGemValue(CString szVName, ULONG ulValue);
	VOID SetGemValue(INT nVID, ULONG ulValue);
	VOID SetGemValue(CString szVName, LONG lValue);
	VOID SetGemValue(INT nVID, LONG LValue);
	VOID GetGemValue(CString szVName, LONG &lValue);
	VOID GetGemValue(INT nVID, LONG &lValue);
	// FLOAT/DOUBLE
	VOID SetGemValue(CString szVName, FLOAT fValue);
	VOID SetGemValue(INT nVID, FLOAT fValue);
	VOID SetGemValue(CString szVName, DOUBLE dValue);
	VOID SetGemValue(INT nVID, DOUBLE dValue);
	VOID GetGemValue(CString szVName, DOUBLE &dValue);
	VOID GetGemValue(INT nVID, DOUBLE &dValue);
	// SHORT/USHORT
	VOID SetGemValue(CString szVName, SHORT sValue);
	VOID SetGemValue(INT nVID, SHORT sValue);
	VOID SetGemValue(CString szVName, USHORT usValue);
	VOID SetGemValue(INT nVID, USHORT usValue);
	VOID GetGemValue(CString szVName, USHORT &usValue);
	VOID GetGemValue(INT nVID, USHORT &usValue);

	// set equipment status
	USHORT GetProcessState();
	BOOL IsEquipmentUnscheduleDown();
	VOID EquipStateProductiveToUnscheduleDown();
	VOID EquipStateUnscheduleDwonToProductive();
	//Equipment time statistics 
	//VOID SetEquipmentStatistic(CTime curTime, CTime lastTime, ULONG ulNotifyInterval, BOOL bSend);	//v4.31T9	//v4.37T5	//EquipMgr

	VOID UpdateEquipmentBasicStatistics();
	VOID SendEquipmentBasicStatistics();
	VOID DownloadPkgFile(CString szPPID);					//v4.31T9	//EquipMgr
	VOID UploadPkgFile(CString szPPID, BOOL bFromHost);		//v4.31T9	//EquipMgr
	// send event
	VOID SendEvent(INT nCEID, BOOL bReply = TRUE);			// S6F11
	// set alarm
	VOID SetAlarm(int nAlarmID, BOOL bReply = TRUE);		// S5F1
	VOID SetAlarm(CString szHmiCode, BOOL bReply = TRUE);	// S5F1
	VOID ClearAlarm(int nAlarmID , BOOL bReply = TRUE);		// S5F1
	VOID ClearAlarm(CString szHmiCode , BOOL bReply = TRUE);	// S5F1
	
	//==============================
	// Lexter-specific SECSGEM fcns
	//==============================
	VOID EnableLextarSecsGemFcns(BOOL bEnable);
	VOID UpdateLextarGemData(BOOL bUpdate);						//UPdate LEXTAR struct from/to SECSGEM library internal structure
	BOOL SendLextarStreamMessage(int nStream, int nFunction);	//Send Lextar custom SECSGEM Stream messages to HOST side
	//v4.13T1
	BOOL SendLexter_S1F65(UCHAR ucMachineState = 0);
	BOOL SendLexter_S5F65(UCHAR ucAlarmCode, LONG lAlarmID, BOOL bSerious = FALSE);

	//=========================================================================
	//	HIPEC & NuMotion Motion Commands
	//=========================================================================

	//Axis Data
	VOID InitAxisData(CMSNmAxisInfo	&pAxisInfo);
	BOOL GetAxisData(CMSNmAxisInfo &pAxisInfo);
	VOID SetAxisMoveSrchProfileRatio(CMSNmAxisInfo &pAxisInfo, const double dRatio);
	BOOL PrintAxisData(CMSNmAxisInfo &pAxisInfo, UCHAR ucIndex=0);	//for debug only	//v4.59A25
	BOOL PrintNuConfigData();						//for debug only

	//Update Profile
	VOID UpdateSearchProfile(const CString &szAxis, const CString &szProfile, CMSNmAxisInfo *pAxisInfo);
	VOID UpdateMotionProfile(const CString &szAxis, const CString &szProfile, CMSNmAxisInfo *pAxisInfo);
	VOID UpdatePosUnit(const CString &szAxis, const CString &szPos);


	BOOL MotionIsServo(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionIsDcServo(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionIsStepper(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);

	BOOL MotionIsPowerOn(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionPowerOn(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionPowerOff(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionResetController(const CString& szAxis, CMSNmAxisInfo* pAxisInfo);

	BOOL MotionEnableEncoder(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	LONG MotionGetCommandPosition(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	LONG MotionGetEncoderPosition(const CString &szAxis, DOUBLE dScale, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionSetEncoderDirection(const CString &szAxis, BYTE ucDirection, CMSNmAxisInfo *pAxisInfo);

	BOOL MotionReverseMotorDirection(const CString &szAxis, BYTE ucReverse, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionSetSamplingFreqNew(const CString &szAxis, BYTE ucFreq, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionIsComplete(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	INT MotionMoveHome(const CString &szAxis, BOOL bMode, INT nSimWait, CMSNmAxisInfo *pAxisInfo, BOOL bDirectionReverse = FALSE, BOOL bActiveStateReverse = FALSE);
	//v4.50A23
	INT MotionLeaveHomeSensor(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	INT MotionMoveHome2(const CString &szAxis, BOOL bMode, INT nSimWait, CMSNmAxisInfo *pAxisInfo);
	LONG MotionSetPosition(const CString &szAxis, LONG lPos, CMSNmAxisInfo *pAxisInfo);
	INT MotionSetContourProfile(const CString &szAxis, FLOAT *fPos, FLOAT *fVel, FLOAT *fAcc, 
								FLOAT *fJerk, UINT uiNoofPoint, UINT uiStartOffset , CMSNmAxisInfo *pAxisInfo);
	INT MotionContourMove(const CString &szAxis, UINT uiNoofPoint, UINT uiStartOffset, BOOL bWait, CMSNmAxisInfo *pAxisInfo);
	INT MotionScfProfileMove(const CString &szAxis, INT nPos, INT nMode, CMSNmAxisInfo *pAxisInfo);
	INT MotionMove(const CString &szAxis, INT nPos, INT nMode, CMSNmAxisInfo *pAxisInfo);
	INT MotionMoveTo(const CString &szAxis, INT nPos, INT nMode, CMSNmAxisInfo *pAxisInfo, short sProcListSaveID = -1);
	INT MotionObwMove(const CString &szAxis, INT nPos, INT nMode, CMSNmAxisInfo *pAxisInfo);
	INT MotionObwMoveTo(const CString &szAxis, INT nPos, INT nMode, CMSNmAxisInfo *pAxisInfo, short sProcListSaveID = -1);
	INT MotionMoveToSearch(const CString &szAxis, INT nPos, INT nMode, CMSNmAxisInfo *pAxisInfo, USHORT usDebounce = 0);
	INT MotionStop(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	INT MotionQuickStop(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	INT MotionSearch(const CString &szAxis, INT nDirection, BOOL fWait, CMSNmAxisInfo *pAxisInfo, const CString &szProfileID = "", BOOL bDirectionReverse = FALSE, BOOL bEnaErrChk = FALSE, BOOL bActiveStateReverse = FALSE);
	INT MotionSearchInRange(const CString &szAxis, INT nDirection, BOOL fWait, CMSNmAxisInfo *pAxisInfo, const CString &szProfileID = "");
	INT MotionFastHomeSearch(const CString &szAxis, INT nDirection, BOOL fWait, CMSNmAxisInfo *pAxisInfo, const CString &szProfileID = "", const CString &szSrchProfileID = "");	//v3.94
	INT MotionSetPositionErrorLimit(const CString &szAxis, SHORT sPositionLimit, CMSNmAxisInfo *pAxisInfo);

	INT OpenCommutateServo(const CString &szServoChannel,
						   INT nPole, INT nEncoderCnt,
						   INT nCurrentLimit, INT nTime,
						   INT nInitRamp = 500, INT nRampTime = 500,
						   INT nHoldTime = 600,
						   INT nDacLimit = 0, INT nDacTime = 0x500,
						   INT nOrgAngle = 60, INT nStepAngle = 1);


	INT DirectionalCommutateServo(const CString &szServoChannel, 
								  INT nPole, INT nEncoderCnt, INT nDirection);

	INT CommutateServo(const CString &szAxis, CMSNmAxisInfo *pAxisInfo = NULL);
	
	INT MotionOpenCommutateServo(const CString &szServoChannel,
								 CMSNmAxisInfo *pAxisInfo,
								 INT nPole, INT nEncoderCnt,
								 INT nCurrentLimit, INT nTime,
								 INT nInitRamp = 500, INT nRampTime = 500,
								 INT nHoldTime = 600,
								 INT nDacLimit = 0, INT nDacTime = 0x500,
								 INT nOrgAngle = 60, INT nStepAngle = 1);

	INT MotionDirectionalCommutateServo(const CString &szServoChannel, 
										INT nPole, INT nEncoderCnt, INT nDirection, CMSNmAxisInfo *pAxisInfo);

	INT MotionCommutateServo(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);

	VOID MotionClearError(const CString &szAxis, LONG lProtectionType, CMSNmAxisInfo *pAxisInfo);
	INT MotionClearError(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	BOOL ClearServoError(const CString &szServo);
	BOOL ClearDcServoError(const CString &szServo);
	BOOL ClearStepperError(const CString &szStepper);

	LONG MotionReportErrorStatus(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	LONG ReportUserErrorStatus(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, long lStartErrorCode = 1);
	SHORT MotionGetLastWarning(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	INT MotionSetLastWarning(const CString &szAxis, SHORT sLastWarn, CMSNmAxisInfo *pAxisInfo);

	BOOL MotionEnableProtection(const CString &szAxis, LONG lProtectionType, BOOL fTriggerState, BOOL bEnable, CMSNmAxisInfo *pAxisInfo);

	//Set Software Limit
	BOOL MotionSetSoftwareLimit(const CString &szAxis, LONG lUpperLimit, LONG lLowerLimit, CMSNmAxisInfo *pAxisInfo);

	INT MotionAbortMotion(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);

	INT MotionSync(const CString &szAxis, ULONG ulTimeout, CMSNmAxisInfo *pAxisInfo);
	INT MotionWait(const CString &szAxis, LONG lWaitTime, CMSNmAxisInfo *pAxisInfo);	//v4.44A5
	INT MotionControlWait(const CString &szAxis, LONG lWaitTime, UCHAR ucControlID, CMSNmAxisInfo *pAxisInfo);
	INT MotionCommandPosSync(const CString &szAxis, LONG lCommandPos, ULONG ulTimeout, CMSNmAxisInfo *pAxisInfo);
	INT	SyncServo(SFM_CHipecAcServo *pServo, int nTimeOut = 10000);	
	INT SyncDcServo(SFM_CHipecDcServo *pServo, int nTimeOut = 10000);	//UBH
	INT	SyncStepper(SFM_CHipecStepper *pStepper, int nTimeOut);

	BOOL MotionSetJoystickConfig(const CString &szModuleID, const CString &szJoystickID, 
								 const CString &szInputBit1ID, const CString &szInputBit2ID);
	
	VOID MotionJoyStickOn(const CString &szAxis, BOOL bOn, CMSNmAxisInfo *pAxisInfo);
	VOID MotionSetIncrementParam(const CString &szAxis, const CString &szIncrementParamID, 
								 LONG lVelocity, LONG lVelocityLow, LONG lAcceleration, LONG lDeceleration, CMSNmAxisInfo *pAxisInfo);
	VOID MotionSelectIncrementParam(const CString &szAxis, const CString &szIncrementParamID, CMSNmAxisInfo *pAxisInfo);
	VOID MotionSelectJoystickConfig(const CString &szAxis, const CString &szJoyStickConfigID, CMSNmAxisInfo *pAxisInfo);
	VOID MotionSelectJoystickFactor(const CString &szAxis, SHORT sScaleFactor, CMSNmAxisInfo *pAxisInfo);
	VOID MotionSetJoystickPositionLimit(const CString &szAxis, LONG lLowLimit, LONG lHighLimit, CMSNmAxisInfo *pAxisInfo);

	INT MotionGetInputBitType(const CString &szBitId);
	INT MotionGetOutputBitType(const CString &szBitId);
	BOOL MotionReadInputBit(const CString &szBitId);		//v4.48A1	//Changed Return Type
	INT  MotionSetOutputBit(const CString &szBitId, BOOL bSetState);
	BOOL MotionGetOutputBitInfo(const CString& szBitId, CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN], GMP_U32 &uiMask);

	UINT MotionReadInputADCPort(const CString szPortName);	//v4.43T3

	INT MotionIsNegativeLimitHigh(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	INT MotionIsPositiveLimitHigh(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionIsHomeSensorHigh(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);		//v4.05	//Klocwork

	VOID MotionGetSearchProfile(const CString &szAxis, const CString &szProfile, CMSNmAxisInfo *pAxisInfo);
	VOID MotionSelectProfile(const CString &szAxis, const CString &szProfileID, CMSNmAxisInfo *pAxisInfo);
	VOID MotionSelectObwProfile(const CString &szAxis, const CString &szProfileID, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionGetSearchProfileParameters(const CString &szAxis, CString szProfileID, DOUBLE &dSpeed, LONG &lDistance, DOUBLE &dDriveInSpeed,
										  LONG &lDriveInDistance, CMSNmAxisInfo *pAxisInfo);
	VOID MotionSelectSearchProfile(const CString &szAxis, const CString &szProfileID, CMSNmAxisInfo *pAxisInfo);
	VOID MotionUpdateSearchProfile(const CString &szAxis, const CString &szProfileID, 
								   DOUBLE dVelocity, LONG lDistance, CMSNmAxisInfo *pAxisInfo);
	VOID MotionUpdateSearchProfile(const CString &szAxis, const CString &szProfileID, 
								   DOUBLE dVelocity, LONG lDistance, DOUBLE dDriveInSpeed,
								   ULONG ulDriveInDistance, CMSNmAxisInfo *pAxisInfo);
	VOID MotionUpFullSearchProfile(const CString &szAxis, const CString &szProfileID, ULONG ulMask, ULONG ulAState, 
								   DOUBLE dVelocity, LONG lDistance, DOUBLE dDriveInSpeed,
								   ULONG ulDriveInDistance, CMSNmAxisInfo *pAxisInfo);
	VOID MotionUpdateMotionProfile(const CString &szAxis, const CString &szProfile, CMSNmAxisInfo *pAxisInfo);	
	BOOL MotionUpdateProfile(const CString &szAxis, const CString &szProfileID, DOUBLE dVmax, DOUBLE dAcc, 
							 DOUBLE dDec, DOUBLE dJerk, DOUBLE dFinalVel, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionUpdateObwProfile(const CString &szAxis, const CString &szProfileID, DOUBLE dVmax, 
								DOUBLE dAmax, DOUBLE dDmax, DOUBLE dFinalVel, CMSNmAxisInfo *pAxisInfo);
	LONG MotionGetSCFProfileTime(const CString &szAxis, const CString &szProfileID, LONG lMaxTravelDist, LONG lQueryDist, LONG lSamplingRate, 
								 CMSNmAxisInfo *pAxisInfo);
	LONG MotionGetProfileTime(const CString &szAxis, const CString &szProfileID,
							  LONG lMaxTravelDist, LONG lQueryDist, LONG lSamplingRate, CMSNmAxisInfo *pAxisInfo);
#ifndef ES101
#ifdef NU_MOTION
	LONG MotionGenerateFifthOrderProfileUsedForILC(const CString &stAxis, const CString &szProfileID, CTRL_ILC_CONFIG_STRUCT stILC , float fDistance, CMSNmAxisInfo *pAxisInfo = NULL);
#endif
#endif

	LONG MotionGetFifthOrderProfileTime(const CString &stAxis, const CString &szProfileID, float flDistance, LONG lSamplingRate, CMSNmAxisInfo *pAxisInfo);
	LONG MotionGetObwProfileTime(const CString &szAxis, const CString &szProfileID, LONG lMaxTravelDist, LONG lQueryDist, LONG lSamplingRate, CMSNmAxisInfo *pAxisInfo);
	VOID MotionCalculateObwProfileData(const CString &szAxis, const CString &szProfileID, LONG lMotionTime, LONG lTravelDist, LONG lSamplingRate, DOUBLE &dAccMax, DOUBLE &dDecMax, CMSNmAxisInfo *pAxisInfo);
	DWORD MotionSelectControlType(const CString &szAxis, LONG lControlNo, LONG lControlType, CMSNmAxisInfo *pAxisInfo);
	CString MotionSelectControlParam(const CString &szAxis, const CString &szControlParaID, CMSNmAxisInfo *pAxisInfo, const CString &szNuControlParaID = "default");
	CString MotionSelectStaticControl(const CString &szAxis, CMSNmAxisInfo *pAxisInfo = NULL, const INT nControlID = PL_STATIC);
	CString MotionSelectSettlingControl(const CString &szAxis, CMSNmAxisInfo *pAxisInfo = NULL, const INT nControlID = PL_SETTLING, const LONG lSettlingTime = 10);
	BOOL MoitonAddControlParam(const CString &szModuleID, const CString &szAxis, const CString &szControlParaID, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionSetControlParam(const CString &szAxis, const CString &szControlParaID, CMSNmAxisInfo *pAxisInfo);
	
	// 32 bit port type is default
	BOOL MotionWriteSoftwarePort(const CString &szPortID, USHORT usBit, BOOL bState);
	BOOL MotionReadSoftwarePort(const CString &szPortID, LONG &lBit);
	BOOL MotionResetSoftwarePort(const CString &szPortID);
	BOOL MotionDirectReadSWPort(const CString &szPortID, LONG &lValue);
	BOOL MotionDirectResetSWPort(const CString &szPortID);
	short WriteSoftwarePort(Hp_mcb *hp_Module, short sPortNo, LONG lData, LONG lPortType = 0xffffffff);
	short ReadSoftwarePort(Hp_mcb *hp_Module, short sPortNo, LONG &lData);

	BOOL MotionCheckResult(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	// Check HiPEC result after motion command is called	
	BOOL CheckHiPECResult(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);

	//Get/Set Dac Limit
	SHORT MotionGetDacLimit(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	SHORT MotionGetDacTimeLimit(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);
	BOOL MotionSetDacLimit(const CString &szAxis, SHORT ssCurrDacLimit, SHORT ssOrgDacLimit, CMSNmAxisInfo *pAxisInfo);

	//Update Drive In / Debounce value
	VOID MotionUpdateDriveIn(const CString &szAxis, const CString &szProfileID, LONG lDriveIn, CMSNmAxisInfo *pAxisInfo);
	VOID MotionUpdateDebounce(const CString &szAxis, const CString &szProfileID, LONG lDebounce, CMSNmAxisInfo *pAxisInfo);

	//Set Velocity Filter
	BOOL MotionSetVelocityFilter(const CString &szAxis, SHORT ssVelFilterGainX, SHORT ssVelFilterWeightX, CMSNmAxisInfo *pAxisInfo);
	VOID MotionSetSearchLimit(const CString &szAxis, const CString &szProfileID, LONG lDistance, CMSNmAxisInfo *pAxisInfo);
	LONG MotionGetPosition(const CString &szAxis, CMSNmAxisInfo *pAxisInfo);

	//Axis Performance
	BOOL LogAxisPerformance(const CString &szModuleID, CMSNmAxisInfo *pAxisInfo, BOOL bEnable, LONG lSettleTime, DATA_LOG_INFO &stLogInfo, UCHAR ucLogMode = 0, 
							BOOL bDisplay = FALSE, BOOL bWriteToFile = TRUE);
	BOOL LogAxisPerformance2(const CString &szModuleID1, const CString &szModuleID2, CMSNmAxisInfo *pAxisInfo1, CMSNmAxisInfo *pAxisInfo2, BOOL bEnable, BOOL bDisplay = FALSE, 
							 BOOL bSpecialLog = FALSE, BOOL bWriteToFile = TRUE);
	BOOL LogAxisPerformance3(const CString &szModuleID1, const CString &szModuleID2, const CString &szModuleID3, CMSNmAxisInfo *pAxisInfo1, CMSNmAxisInfo *pAxisInfo2, CMSNmAxisInfo *pAxisInfo3, 
							 BOOL bEnable, BOOL bWriteToFile = TRUE);
	BOOL NuMotionDataLog(const CString &szModuleID, CMSNmAxisInfo &cAxisInfo, BOOL bEnable, LONG lSettleTime, DATA_LOG_INFO &stLogInfo, UCHAR ucLogMode, BOOL bDisplay = FALSE, 
						 BOOL bWriteToFile = TRUE);
	BOOL NuMotionDataLog_DAC(const CString &szModuleID, CMSNmAxisInfo &cAxisInfo, BOOL bEnable, LONG lSettleTime, DATA_LOG_INFO &stLogInfo, BOOL bPickToBond = TRUE);	//v4.16T1
	BOOL NuMotionDataLog2(const CString &szModuleID1, const CString &szModuleID2, CMSNmAxisInfo &cAxisInfo1, CMSNmAxisInfo &cAxisInfo2, BOOL bEnable, BOOL bDisplay = FALSE, BOOL bSpecialLog = FALSE,
						  BOOL bWriteToFile = TRUE);	
	BOOL NuMotionDataLog3(const CString &szModuleID1, const CString &szModuleID2, const CString &szModuleID3, CMSNmAxisInfo &cAxisInfo1, CMSNmAxisInfo &cAxisInfo2, CMSNmAxisInfo &cAxisInfo3, BOOL bEnable,
						  BOOL bWriteToFile = TRUE);
	BOOL NuMotionDataLogForBHDebug(BOOL bEnable);

	// Using external file to enable and disable the data log
	BOOL NuMotionDataLogExt(BOOL bEnable);
	BOOL NuMotionDataLogExt_MS100(BOOL bEnable, BOOL bWriteToFile);
	BOOL NuMotionDataLogAdv(BOOL bEnable);

	BOOL GetNuMotionDataLogData(const CString &szModuleID, CMSNmAxisInfo &stAxis, BOOL bEnable, 
								INT &nNoOfSampleUpload, INT *pPos, INT *pEnc, INT *pChannelMode);

	//Open DAC with velocity damping
	INT MotionOpenDACwithVelDamping(const CString &szAxis, CMSNmAxisInfo *pAxisInfo = NULL, LONG lOpenDACValue = 0, LONG lHoldTime = 1, INT nSwPort = 0);

	//Sync Move Via Ej Cmd Dist	//MS100
	INT MotionSyncViaEjCmdDistance(const CString &szAxis, CMSNmAxisInfo *pAxisInfo = NULL, CMSNmAxisInfo *pAxisInfo2 = NULL, LONG lTriggerValue = 0, LONG lZOffset = 0);
	INT MotionSetupSyncViaEjCmdDistance(const CString &szAxis, CMSNmAxisInfo *pAxisInfo = NULL, LONG lDistance = 0);

	INT OpenDac(CMSNmAxisInfo *pAxisInfo, LONG lHoldValue, LONG lEndValue, LONG lRampUpTime, LONG lHoldTime, LONG lRampDownTime);

	//Thermal DAC fcn	//MS60	//MS100P3	//v4.48A10	
	BOOL MotionGetMotorRMSValues(const CString &szAxis, const UINT unLevel,
								 DOUBLE& dTimeInSec, DOUBLE& dRmsR, DOUBLE& dRmsS, DOUBLE& dRmsT,
								 CMSNmAxisInfo *pAxisInfo);
	BOOL MotionSetMotorThermalCompDAC(const CString &szAxis, USHORT usDAC, CMSNmAxisInfo *pAxisInfo);
	//v4.59A17
	BOOL MotionSetThermalCtrlTimer(const CString &szAxis, CONST USHORT usTempInDegree, CMSNmAxisInfo *pAxisInfo);


	// tester functions
	BOOL ConnectTesterTerminal();
	BOOL SendRequestToTesterTerminal(CString szCmd, CString &szReply);
	BOOL DisconnectTesterTerminal();
	BOOL SendStartToTester();
	// prober sample check
	LONG GetPSCMode();

#ifdef NU_MOTION_MS60		//v4.50A16	//compatible with MS100Plus2/3
	GMP_U32 CalibratePrepickPositionForPreHeat(const CString &szAxis, CMSNmAxisInfo *pAxisInfo = NULL);
#endif

	//v4.50A24	//Osram Germany MS109 CEMark
	BOOL IsCEMarkMotors(const CString szAxis);
	BOOL CheckCEMarkCoverSensors();

	BOOL IsAxisExist(CMSNmAxisInfo &stAxis);
	BOOL CheckSIOLinkStatus(GMP_S16 ssErrorCode);
	BOOL IsMTRHomeActive(CMSNmAxisInfo &stAxis);
	INT SetMotorPower(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, BOOL bPowerOn);
	short SetProcessProf(CMSNmAxisInfo* pAxisInfo, short sSaveID, unsigned short usBlkNum, unsigned short usNextBlk, int nTime,
						 char *szDestination, unsigned int unOutput, unsigned int unMask, byte ucRelTimeMode = 0);
	void SetProcessList(CMSNmAxisInfo* pAxisInfo, short sSaveID, byte ucSize, unsigned short *psSaveIDList);

	LONG AgmpMotionHealthCheck(CMSNmAxisInfo* pAxisInfo, CString &szMHCResult, BOOL IsMHCTrained);

	VOID SD_GetData(double dX1, double dX2);
	VOID SD_Calculate(DOUBLE &dMax1,DOUBLE &dMin1,DOUBLE &dMax2,DOUBLE &dMin2,DOUBLE &dSD1,DOUBLE &dSD2);

	VOID SD_GetDataAdv(CStringArray &szaData,double dX);
	VOID SD_CalculateAdv(CStringArray &szaData, DOUBLE &dMax, DOUBLE &dMin, DOUBLE &dSD);

	VOID SaveEjtCollet1Offset(const LONG lEjtCollet1OffsetX, const LONG lEjtCollet1OffsetY, const double dEjtXYRes);
	VOID SaveEjtCollet2Offset(const LONG lEjtCollet2OffsetX, const LONG lEjtCollet2OffsetY, const double dEjtXYRes);
	LONG GetEjtCollet1OffsetX();
	LONG GetEjtCollet1OffsetY();
	LONG GetEjtCollet2OffsetX();
	LONG GetEjtCollet2OffsetY();

	VOID GetBHMarkOffset_um(double &dBH1MarkOffsetX_um, double &dBH1MarkOffsetY_um, double &dBH2MarkOffsetX_um, double &dBH2MarkOffsetY_um);
	VOID GetBHColletHoleOffset_um(double &dBH1ColletHoleOffsetX_um, double &dBH1ColletHoleOffsetY_um, double &dBH2ColletHoleOffsetX_um, double &dBH2ColletHoleOffsetY_um);
	VOID GetColletOffset_um(double &dCollet1OffsetX_um, double &dCollet1OffsetY_um, double &dCollet2OffsetX_um, double &dCollet2OffsetY_um);
	VOID GetEjtColletOffsetCount(const LONG lEjtX, const LONG lEjtY, LONG& lCollet1OffsetX, LONG& lCollet1OffsetY, LONG& lCollet2OffsetX, LONG& lCollet2OffsetY, const DOUBLE dEJTResolution, const BOOL bColletMarkOffset, const BOOL bBH2, const LONG lFlag);

	LONG ConvertUMToCount(const double dValue_um, const double dResultion);
	DOUBLE ConvertCountToUM(const LONG lCount, const double dResolution);
	VOID GetColletOffsetCount(LONG& lCollet1OffsetX, LONG& lCollet1OffsetY, LONG& lCollet2OffsetX, LONG& lCollet2OffsetY, const DOUBLE dResultion, BOOL bColletMarkOffset);

//	VOID SaveBTAdjCollet1Offset(const LONG lBHZ1AdjBondPosOffsetX, const LONG lBHZ1AdjBondPosOffsetY, const double dXYRes);
//	VOID SaveBTAdjCollet2Offset(const LONG lBHZ2AdjBondPosOffsetX, const LONG lBHZ2AdjBondPosOffsetY, const double dXYRes);
	VOID SaveBTCollet1Offset(const LONG lBHZ1BondPosOffsetX, const LONG lBHZ1BondPosOffsetY, const double dEjtXYRes);
	VOID SaveBTCollet2Offset(const LONG lBHZ2BondPosOffsetX, const LONG lBHZ2BondPosOffsetY, const double dEjtXYRes);
	VOID SaveBTBond180DegOffset(const double dBond180DegOffsetX_um, const double dBond180DegOffsetY_um);
	VOID GetBTColletOffset_um(double &dCollet1OffsetX_um, double &dCollet1OffsetY_um, double &dCollet2OffsetX_um, double &dCollet2OffsetY_um);
	VOID GetBTBond180DegOffset(double &dBond180DegOffsetX_um, double &dBond180DegOffsetY_um);
	VOID GetBTColletOffsetCount(LONG& lCollet1OffsetX, LONG& lCollet1OffsetY, LONG& lCollet2OffsetX, LONG& lCollet2OffsetY, const DOUBLE dResultion, const BOOL bColletMarkOffset);

private:

	// For Step Move
	LONG GetCode();
	VOID BackCode();
	VOID ResetCode();


public:
	static CStringArray     m_szaSDX1;
	static CStringArray     m_szaSDX2;

	static CPrGeneral	*m_pPrGeneral;
	//==============================
	// Wafermap memory		//v3.60T1
	//==============================
	static	BOOL	m_bIfGenWaferEndFile;
	static	BOOL	m_bAutoLoadWaferMap;
	static CWaferMapWrapper m_WaferMapWrapper;		// Wafer Map Object
	static CPreBondEvent	m_WaferMapEvent;		// Wafer map event
	static CWaferMapWrapper m_BinMapWrapper;		// Wafer Map Object		//PLLM REBEL	//v4.03
	static CPreBondEventBin	m_BinMapEvent;			// Wafer map event		//PLLM REBEL	//v4.03
	static CString	m_szOsramSDF;

	static BOOL m_bWaferMapWarmStartFail;			//v4.22T2
	static BOOL m_bSECSWaferMapLoadFinish;			//v4.59A13
	static BOOL m_bSECSWaferMapUpLoadFinish;		//v4.59A13
	static BOOL m_bSECSWaferMapUpLoadAbort;			//v4.59A13
	static BOOL	m_bRestoreNVRamInBondPage;			//v4.22T2
	static LONG	m_lAPLastDone;

	static	CStringArray	m_szaGradeItemList;
	static	BOOL			m_bCPItemMapToTop;

	static UCHAR	m_ucMS100PrDisplayID;	//0=DEFAULT, 1=Collet1, 2=Collet2, etc.	//v4.50A26


	//==============================
	// HW Module Flags		//v3.60T1
	//==============================
	static BOOL m_bDisableBH;
	static BOOL m_bDisableBT;
	static BOOL m_bDisableBL;
	static BOOL m_bDisableNL;						//andrewng //2020-0707
	static BOOL m_bDisableWL;
	static BOOL m_bDisableWT;
	static BOOL m_bDisableWLWithExp;
	static BOOL m_bAppInitialized;					//v4.02T3

	//	Wafer Loader Operation Option
	static ULONG	m_ulSequenceMode;
	static CTime	m_stMachineStopTime;			//v4.59A41	//Finisar TX
	static LONG		m_lABDummyCounter;				//andrewng //2020-0727

	//==============================
	// SECSGEM station attributes
	//==============================
	static 	CGemStation     *m_pGemStation;
	static BOOL m_bIsSecsGemInit;
	static BOOL m_bIsSecsCommEnabled;
	static LONG m_lEquipmentStatus;
#ifndef MS50_64BIT
	static Lextar	m_stLexter;
#endif
	static BOOL		m_bEnableLextarSecsGem;
//	static BOOL		m_bStopByCommand;
//	static ULONG	m_ulEMLastOutput;
//	static ULONG	m_ulEMLastProdTime;
//	static ULONG	m_ulEMLastStandbyTime;
//	static ULONG	m_ulEMLastDownTime;
//	static ULONG	m_ulEMLastNumOfAssists;
//	static ULONG	m_ulEMLastNumOfFailures;
//	static ULONG	m_ulEMLastSetupTime;

	//==============================
	// Output File Generating
	//==============================
	static CObArray m_objaGenerateDatabase;	
	static CObArray m_objaQueryDatabase;

	static CGenerateWaferDatabase m_GenerateWaferDatabase;
	static CQueryWaferDatabase m_QueryWaferDatabase;

	static DATA_LOG_INFO m_stDataLog;

	static BOOL m_bIfGenRefDieStatusFile;
	static BOOL m_bIfGenLabel;
	static BOOL m_bUseRefDieCheck;
	static BOOL m_bUsePostBond;
	static BOOL m_bUseSecsGem;
	static BOOL m_bUseTcpIp;
	static BOOL m_bIfEnableCharDie;
	static BOOL m_bEnablePolyonDie;
	static BOOL m_bEnableRhombusDie;		//Silan		//v4.50A17
	static BOOL	m_bEnableBlockFunc;
	static BOOL m_bEnable6InchWaferTable;

	// AOI learn and use OCR die
	static BOOL m_bAoiEnableOcrDie;
	static BOOL	m_bRebelManualAlign;
	static int	m_siBarPitchX;				// Bar Pitch X Enc Pos
	static int	m_siBarPitchY;				// Bar Pitch Y Enc Pos

	static BOOL	m_bEnablePickAndPlace;

	// prescan relative variables
	static BOOL m_bReadyToRealignOnWft;

	static LONG	m_nPrescanLastWftPosnX;
	static LONG	m_nPrescanLastWftPosnY;
	static LONG	m_nPrescanNextMapRow;
	static LONG	m_nPrescanNextMapCol;
	static LONG	m_nPrescanLastMapRow;
	static LONG	m_nPrescanLastMapCol;

	static LONG	m_lAlignFrameRow;
	static LONG	m_lAlignFrameCol;
	static LONG m_lNextFrameRow;
	static LONG m_lNextFrameCol;
	static LONG m_lLastFrameRow;
	static LONG m_lLastFrameCol;
	static LONG m_lWftScanIndexAndDelayTime;
	static LONG	m_lScanFrameMinRow;
	static LONG	m_lScanFrameMinCol;
	static LONG	m_lScanFrameMaxRow;
	static LONG	m_lScanFrameMaxCol;

	static BOOL	m_bAFGridSampling;
	static LONG	m_lPrescanLastGrabRow;
	static LONG	m_lPrescanLastGrabCol;
	static BOOL	m_bPrescanLastGrabbed;

	static LONG	m_lAreaPickStage;	// 0 start or new map, 1 just align or prescanning, 2 area picking
	static INT		m_nPickListIndex;
	static INT		m_nDieSizeX;
	static INT		m_nDieSizeY;
	static DOUBLE	m_dPrescanLFSizeX;
	static DOUBLE	m_dPrescanLFSizeY;
	//	4.24TX 3
	static BOOL m_bFastHomeScanEnable;
	static BOOL m_bIMPrescanReferDie;
	static LONG	m_lPrescanHomeDieRow;
	static LONG	m_lPrescanHomeDieCol;
	static LONG	m_lPrescanHomeWaferX;
	static LONG	m_lPrescanHomeWaferY;
	static LONG m_lBackLightZStatus;	// 0 default(down), 1 from down to up, 2 up position, 3 from up to down
	static LONG	m_lBackLightTravelTime;	// between up and standby
	static DOUBLE  m_dBackLightZDownTime;
	static DOUBLE	m_dScanStartTime;
	static DOUBLE	m_dScanEndTime;
	static CTime	m_stScanStartCTime;
	static CTime	m_stScanEndCTime;

	static LONG	m_lTimeSlot[50];
	static LONG	m_lMapValidMinRow;
	static LONG	m_lMapValidMinCol;
	static LONG	m_lMapValidMaxRow;
	static LONG	m_lMapValidMaxCol;
	static LONG	m_lPrescanSortingTotal; 
	static LONG m_lFocusZState;			// 0 default(HOME), 1 WT1 from HOME to WF, 2 WF position, 3 from WF to HOME
	static LONG	m_lWT1PositionStatus;
	static LONG	m_lWT2PositionStatus;

	static	LONG	m_lAtBondDieGrade;
	static	LONG	m_lAtBondDieRow;
	static	LONG	m_lAtBondDieCol;

	static LONG	m_lPrescanMovePitchX;
	static LONG	m_lPrescanMovePitchY;

	static BOOL	m_bEnablePrescan;
	static BOOL m_bIsPrescanned;
	static BOOL	m_bIsPrescanning;
	static BOOL	m_bPrescanBlkPickAlign;
	static LONG	m_lMapPrescanAlignWafer;
	static BOOL	m_bMapDummyPrescanAlign;
	static BOOL	m_bDisplaySortBinItemMsg;
	static BOOL	m_bSearchCompleteFilename;
	static BOOL	m_bLoadMapSearchDeepInFolder;
	static BOOL	m_bEnableSCNFile;
	static BOOL m_bEnablePsmFileExistCheck;
	static BOOL m_bEnableBatchIdFileCheck;
	static BOOL m_bDisableWaferMapFivePointCheckMsgSelection;
	static BOOL m_bDisableClearCountFormat;
	static BOOL	m_bEnableDynMapHeaderFile;
	static BOOL	m_bBackupTempFile;
	static BOOL m_bBackupOutputTempFile;
	static BOOL	m_bForceDisableHaveOtherFile;
	static BOOL m_bEnableManualUploadBinSummary;
	static BOOL m_bGenerateAccumulateDieCountReport;
	static BOOL m_bDisableBinSNRFormat;
	static BOOL	m_bEnablePRDualColor;
	static BOOL m_bEnablePRAdaptWafer;
	static BOOL m_bCounterCheckWithNormalDieRecord;
	static BOOL m_bAddWaferIdInMsgSummaryFile;
	static BOOL m_bDisableOKSelectionInAlertMsg;
	static BOOL m_bEnableGradeMappingFile;
	static BOOL m_bEnablePickNPlaceOutputFile;
	static BOOL m_bDisableDefaultSearchWnd;
	static BOOL m_bUseOptBinCountDynAssignGrade;
	static BOOL	m_bOpenMatchPkgNameCheck;
	static BOOL m_bIsPostBondDataExported;	//v3.93
	static BOOL	m_bAOINgPickPartDie;

	static BOOL m_bNGPick;					//v4.51A19	//Silan MS90
	static BOOL m_bMS60NGPick;				//v4.54A4	//
	static BOOL m_bAutoUploadOutputFile;
	static BOOL m_bMS90WTPrVibrateTest;		//v4.59A40

// wafer mylar prediction
	static	LONG	m_lPredictMethod;	//	0, none; 1, DEB; 2, ADV; 3, Rescan; 4 DEB+Rescan
	BOOL	IsAutoSampleSort();

	static	BOOL	m_bAutoSampleAfterPE;
	static	BOOL	m_bAutoSampleAfterPR;
	static	ULONG	m_ulNextAdvSampleCount;
	static	LONG	m_lWftAdvSamplingStage;
	static	LONG	m_lReadyToSampleOnWft;
	static	LONG	m_lReadyToCleanEjPinOnWft;
	static	ULONG	m_ulNewPickCounter;
	static	LONG	m_ulLastRealignDieCount;

	ULONG			m_ulGoodDieTotal;
	ULONG			m_ulTotalDieCounter;
	DOUBLE			m_dWaferYield;

	ULONG	GetNewPickCount();
	ULONG	GetNextXXXCounter();	// next realign key die/adv.sample/rescan wafer.
	LONG	GetWftSampleState();
	LONG	GetWftCleanEjPinState();

	static	BOOL		m_bEnableDEBSort;			//Silan		//v4.50A25
	static	BOOL		m_bEnableAdaptPredict;
	static	BOOL		m_bDEB_INIT_OK;
	//static	PstnModel	m_oPstModel;
	static	LONG		m_lGetSetLogic;
	static	BOOL		m_bGiveUpDebKeyDie;
	static	UCHAR		m_ucDEBKeyDieGrade;
	BOOL	DEB_IsEnable();
	BOOL	DEB_IsUseable();
	BOOL	DEB_UpdateDiePos(CONST LONG lRow, CONST LONG lCol, CONST LONG lPhyX, CONST LONG lPhyY, CONST INT nStatus);
	BOOL	DEB_GetDiePos(CONST LONG lRow, CONST LONG lCol, LONG& lPhyX, LONG& lPhyY, INT& nStatus);
	BOOL	DEB_GetDiePosnOnly(CONST LONG lRow, CONST LONG lCol, LONG& lPhyX, LONG& lPhyY);

	static	BOOL	m_bAdvRegionOffset;
	static	BOOL	m_bPitchCheckMapWafer;
	BOOL	IsAdvRegionOffset();
	BOOL	IsVerifyMapWaferEnable();
	BOOL	IsSamplingRescanEnable();

	static	BOOL	m_bAutoRescanWafer;
	static	LONG	m_lRescanRunStage;
	BOOL	IsAutoRescanEnable();
	BOOL	IsAutoRescanWafer();
	BOOL	IsAutoRescanAndDEB();
	BOOL	IsAutoRescanAndSampling();
	BOOL	IsAutoRescanOnly();
// wafer mylar prediction

	static BOOL m_bCheckIgnoreGradeCount;
	static ULONG m_ulIgnoreGrade;
	static BOOL	m_bNoIgnoreGradeOnInterface;
	
	static LONG m_lBinMixCount;
	static BOOL m_bHeapCheck;
//	static BOOL	m_bBurnInMode;				// Flag for indicate it is burn-in mode
	static BOOL m_bBhInBondState;			//v2.56
	static BOOL	m_bPostBondAtBond;			//v2.60
	static BOOL m_bPreBondAtPick;			//v3.34
	static BOOL m_bRealignFrameDone;
	static BOOL m_bRealignFrame2Done;		//v4.17T5	//MS100 9Inch BT2 alignment

	static BOOL m_bOfflinePrintLabel;		//v2.66		//Used in SETUP mode to generate label file only
	static BOOL m_bStPrintLabelInBkgd;		//v2.64		//Used in AUTOBOND mode to generate label in Safety stn
	static BOOL m_bStPrintLabelInABMode;	//v2.64
	static BOOL m_bStStartPrintLabel;		//v2.64
	static BOOL m_bIsAlwaysPrintLabel;

	static ULONG m_ulDefaultClearCountFormat;
	static ULONG m_ulDefaultBinSNRFormat;
	static BOOL  m_bExArmUnloadLastFrame;		//v2.93T2
	static BOOL	 m_bExArmPreloadFrame;			//v4.31T6	//Lextar to avoid gripper collision with BT in auto-clean collet
	static BOOL	 m_bIsBLIdleinABMode;			//v4.42T7	//Testar MS109
	static BOOL	 m_bBhAutoCleanCollet;			//v4.31T6	//Lextar to avoid gripper collision with BT in auto-clean collet
	static BOOL	m_bWLFrameToBeUnloadOnBuffer;	//v4.31T12	//Yealy MS100Plus with single loader & buffer table config
	static BOOL	m_bWLFrameNeedPreloadToBuffer;	//v4.31T12	//Yealy MS100Plus with single loader & buffer table config
	static BOOL m_bWLReadyToUnloadBufferFrame;	//v4.31T12
	static BOOL	m_bIsWaferEnded;				//v4.20		//reposition from WaferTable class to here
	static BOOL	m_bIsWaferEndedFor3EDL;			//v4.45T3
	static BOOL m_bWaferEndFileGenProtection;
	static BOOL m_bAutoGenWaferEndFile;
	static BOOL	m_bUseClearBinByGrade;
	static BOOL m_bEnableExtraClearBinInfo;
	static BOOL m_bAddDieTimeStamp;
	static BOOL m_bCheckProcessTime;
	static BOOL m_bAddSortingSequence;
	static BOOL m_bRemoveBackupOutputFile;
	static BOOL m_bAutoGenBinBlkCountSummary;
	static BOOL m_bChangeGradeBackupTempFile;
	static BOOL m_bKeepOutputFileLog;
	static BOOL m_bEnableMultipleMapHeaderPage;
	static BOOL m_bOutputFormatSelInNewLot;
	static BOOL m_bBTAskBLChangeGrade;			//v4.43T9	//WH SanAn
	static LONG m_bBTAskBLBinFull;				//v4.43T9	//WH SanAn

	static CString m_szWaferId;
	static CString m_szMapFileName;
	static CString m_szMapFileExt1;
	static CString m_szOnlyMapFileFullPath;

	static CString m_szLoadReadBarcode;

	static BOOL m_bDLAChangeGrade;			//v3.10T3

	static BOOL m_bClearDieTypeFieldDuringClearAllBin;
	static BOOL m_bEnableAutoDieTypeFieldnameChek;
	static CString m_szDieTypeFieldnameInMap;

	static BOOL	m_bPackageFileMapHeaderCheckFunc;
	static BOOL m_bUpdateWaferMapHeader;

	static CString m_szPackageFileMapHeaderCheckString;

	static BOOL		m_bEnableAutoMapDieTypeCheckFunc;
	static CString	m_szMapDieTypeCheckString[MS896A_DIE_TYPE_CHECK_LIMIT];
	static BOOL		m_bAutoGenerateWaferEndFile;	//v4.40T14	//3E
	static LONG	m_lOTraceabilityFormat;		//v3.19T1

	static BOOL m_bMS100OriginalBHSequence;
	static BOOL	m_bEnableWaferSizeSelect;

	static BOOL m_bEnableBinLoderCoverSensorCheck;
	static BOOL m_bEnableStartupCoverSensorCheck;

	static BOOL m_bEnableBinBlkTemplate;
	static CString m_szAutoLoadNaming;

	static CString m_szRankIDFileNameInMap;
	static CString m_szRankIDFileExt;
	static BOOL m_bEnableAutoLoadNamingFile;
	static BOOL m_bEnableLoadRankIDFromMap;
	static BOOL m_bBinFrameStatusSummary;
	static BOOL m_bPortablePKGInfoPage;
	static BOOL m_bSupportPortablePKGFile;
	static BOOL	m_bEnableSummaryPage;
	static BOOL m_bEnableWaferLotLoadedMapCheck;
	static BOOL m_bWaferLotWithBinSummaryFormat;
	static BOOL	m_bEnableEmptyBinFrameCheck;
	static BOOL	m_bEnableResetMagzCheck;
	static BOOL m_bEnableWaferLotFileProtection;
	static BOOL m_bWafflePadBondDie;
	static BOOL	m_bWafflePadIdentification;
	static BOOL	m_bForceClearMapAfterWaferEnd;
	static BOOL m_bEnableOptimizeBinCountFunc;
	static BOOL m_bOptimizeBinCountPerWftFunc;
	static BOOL m_bEnableRestoreBinRunTimeData;
	static BOOL m_bUseEmptyCheck;			//Block2
	static BOOL m_bUseAutoManualAlign;		//Block2
	static BOOL m_bUseReVerifyRefDie;		//Cree
	static BOOL m_bNonBlkPkEndVerifyRefDie;
	static BOOL m_bEnableAlarmLampBlink;
	static BOOL m_bEnableAmiFile;
	static BOOL m_bEnableBarcodeLengthCheck;
	static BOOL m_bEnableInputCountSetupFile;
	static BOOL m_bNewLotCreateDirectory;
	static BOOL m_bForceClearBinBeforeNewLot;
	static BOOL	m_bNoPRRecordForPKGFile;
	static BOOL m_bManualAlignReferDie;
	static BOOL m_bPrescanDiePitchCheck;
	static BOOL m_bCoverSensorAlwaysOn;		//v4.28T5
	static BOOL m_bWaferPrUseMxNLFWnd;		//v4.43T2	//SanAn MS100PlusII
	static BOOL m_bWaferPrMS60LFSequence;	//v4.47T3	//MS60
	static BOOL m_bUseRefDieFaceValueCheck;	//v4.48A26	//Avago
	//4.53D18 
	static BOOL m_bWaferMapUseDieMixing;	//	Use Die Mixng Map for Sorting

	static LONG	m_lMnNoDieGrade;

	//v4.36	//Moved from BH class to base class
	static BOOL	m_bEnableFrontCoverLock;
	static BOOL	m_bEnableSideCoverLock;
	static BOOL	m_bEnableBinElevatorCoverLock;

	static BOOL m_bEnableToolsUsageRecord;
	static BOOL m_bEnableMachineReport;
	static BOOL	m_bEnableItemLog;

	static BOOL m_bEnableAlignWaferImageLog;
	static BOOL m_bEnableBinOutputFilePath2;
	static BOOL m_bPrescanLogNgImage;

	static BOOL m_bBLOut8MagConfig;						//MS100 8mag config		//v3.82

	static	LONG	m_lUnloadPhyPosX;		
	static	LONG	m_lUnloadPhyPosY;
	static	LONG	m_lUnloadPhyPosX2;
	static	LONG	m_lUnloadPhyPosY2;

	static	LONG	m_lBTUnloadPos_X;
	static	LONG	m_lBTUnloadPos_Y;
	static	LONG	m_lBTUnloadPos_X2;
	static	LONG	m_lBTUnloadPos_Y2;
	static  LONG	m_lBTBarcodePos_X;					//v4.39T7	//Nichia MS100+
	static  LONG	m_lBTBarcodePos_Y;					//v4.39T7	//Nichia MS100+
	
	//MS109 BInTable 2 OFFSET XY (from BT1 taught positions)		//v4.17T3
	static	LONG	m_lBT2OffsetX;
	static	LONG	m_lBT2OffsetY;
	
	static	LONG	m_lSprialSize;	

	//v3.76	//andrew
	//static BOOL m_bEnableMouseControl;
	static BOOL m_bCEMark;								//v4.06
	static BOOL	m_bEMOChecking;							//v3.91
	//	Thermal Control Variables
	static DOUBLE	m_dID;
	static DOUBLE	m_dIS;
	static DOUBLE	m_dIQ;
	static DOUBLE	m_dMS60ThermalUpdateTime;
	static DOUBLE	m_dRmsRSum;
	static DOUBLE	m_dRmsSSum;
	static DOUBLE	m_dRmsTSum;
	static INT		m_nRmsCount;
	static ULONG	m_ulBHTThermostatCounter;
	static ULONG	m_ulBHTThermostatReading;		//in degree		//v4.49A5
	static DOUBLE	m_dBHTThermostatReading;
	static ULONG	m_ulBHTThermostatADC;			//0-32767		//v4.49A5
	static UINT		m_unMS60TargetIdleTemperature;	//v4.54A4
	static	CMSNmAxisInfo		m_stBHAxis_T;
	static BOOL	m_bEnableMS60ThermalCheck;
	static BOOL	m_bMS60ThermalCtrl;
	static BOOL m_bDBHThermalCheck;						//v4.26T1	//MS100Plus v2.2
	static BOOL m_bDBHHeatingCoilFcn;					//v4.49A5	//MS100P3/MS60/MS90
	static BOOL m_bDBHHeatingCoilFcnInit;				//v4.59A21	//MS100P3/MS60/MS90
	static BOOL m_bDBHThermostat;						//v4.49A5	//MS100P3/MS60/MS90

	static BOOL m_bAutoChangeCollet;					//v4.50A5	//MS60
	static BOOL m_bPBErrorCleanCollet;					//v4.52A1	//Semitek & SanAn
	static ULONG m_ulJoyStickMode;						//v3.76
	static LONG m_lJsTableMode;							// WT or BT
	static LONG m_lJsBinTableInUse;						// BT1 or BT2 (MS100 9Inch dual-table config
	static LONG m_lJsWftInUse;							// WFT1 or WFT2 (ES101 dual table)
	static BOOL m_bCheckWaferLimit;						//
	static LONG	m_lJoystickLevel;						// Joystick Speed Level
	static BOOL	m_bJoystickOn;							// Joystick state
	static BOOL	m_bXJoystickOn;							// X-joystick state
	static BOOL	m_bYJoystickOn;							// Y-joystick state
	static BOOL	m_bIsCircleLimit;
	static UCHAR m_ucWaferLimitType;					// Circle / Polygon Wafer limit
	static LONG	m_lWaferPolyLimitNo;
	static LONG	m_lWaferPolyLimitX[MS_WAFER_LIMIT_POINTS];
	static LONG	m_lWaferPolyLimitY[MS_WAFER_LIMIT_POINTS];
	static LONG	m_lWaferCenterX;			
	static LONG	m_lWaferCenterY;
	static LONG	m_lWaferSize;
	static LONG m_lWT2OffsetX;
	static LONG m_lWT2OffsetY;
	static LONG m_lEjtOffsetX;	// ejector to back light offset, need to change limit when setup and move
	static LONG m_lEjtOffsetY;	// another factor need to concern.
	static LONG m_lProbeOffsetX;
	static LONG m_lProbeOffsetY;
	static INT	m_nWTInUse;		// 0=WT(DEfault); 1=WT2(ES101)		//v4.24
	//v4.50A1	//MS90
	static LONG	m_lWafXNegLimit;
	static LONG	m_lWafXPosLimit;
	static LONG	m_lWafYNegLimit;
	static LONG	m_lWafYPosLimit;

	static BOOL	m_bDisableWT2InAutoBondMode;	//v4.24T11
	static BOOL	m_bContourPreloadMap;
	static LONG	m_lHomeDieMapRow;
	static LONG	m_lHomeDieMapCol;
	static LONG m_lHomeDiePhyPosX;
	static LONG m_lHomeDiePhyPosY;
	static LONG m_lWT2HomeDiePhyPosX;
	static LONG m_lWT2HomeDiePhyPosY;
	static BOOL m_bUseBinMultiMgznSnrs;
	static BOOL m_bEjNeedReplacement;			//v4.39T10	//Silan (WanYiMing)
	static BOOL m_bEjPinNeedReplacementAtUnloadWafer;	//	Need Replace Ejr pin or not at unloading wafer

	//Sync Move Via Ej Cmd Dist	//MS100
	static LONG m_lEjectMoveLength;
	static FLOAT m_fFifthOrderPos_Ej[4096];
	static FLOAT m_fFifthOrderVel_Ej[4096];
	static FLOAT m_fFifthOrderAcc_Ej[4096];
	static FLOAT m_fFifthOrderJerk_Ej[4096];

	//For Mouse Joystick
	static INT		m_nMouseMode;
	static DOUBLE	m_dMouseClickX;	
	static DOUBLE	m_dMouseClickY;
	static INT		m_nMouseDragState;
	static DOUBLE	m_dMouseDragDist;
	static DOUBLE	m_dMouseDragAngle;
	static LONG		m_lMouseJoystickSpeed;
	static LONG		m_lMouseDragDirection;

	// For Step Move
	static BOOL m_bStepMoveEnable;						// Flag to enable step move feature
	LONG m_lStepMove;				

	static DOUBLE	m_dMachineUPH;

	//Grade Information
	static ULONG	m_lPBIShiftCounter;					//only for ES ES Report
	static ULONG	m_lPBIAngleCounter;					//only for ES ES Report

	static ULONG	m_ulPickDieCount;					// Good Die
	static ULONG	m_ulBondDieCount;					// Bonded Die
	static ULONG	m_ulDefectDieCount;					// Defective Die
	static ULONG	m_ulBadCutDieCount;					// Bad-cut Die
	static ULONG	m_ulMissingDieCount;				// Missing Die
	static ULONG	m_ulRotateDieCount;					// Rotate Die
	static ULONG	m_ulEmptyDieCount;					// Empty Die
	static ULONG	m_ulWaferDefectDieCount;			// wafer Defective Die
	static ULONG	m_ulWaferMaxDefectCount;			// max not good die count of current wafer
	static DOUBLE	m_dWaferGoodDieRatio;				// at least % of good die at current wafer
	static ULONG	m_ulBH1UplookPrFailCount;			//v4.59A4	
	static ULONG	m_ulBH2UplookPrFailCount;			//v4.59A4
	static ULONG	m_ulTotalSorted;

	//Air flow sensor test
	static BOOL		m_bEnableAirFlowTest;
	static BOOL		m_bStartAirFlowMdCheck;
	static BOOL		m_bStartAirFlowJcCheck;
	static LONG		m_lAirFlowTestTimeOut;
	static LONG		m_lAirFlowMdTime;
	static LONG		m_lAirFlowJcTime;	
	static LONG		m_lAirFlowStartTime;

	//static CStringArray	m_StrBinBarCodeArray;		//v2.58
	static LONG		m_lAutobondTimeInMin;				//v2.83T6
	static BOOL		m_bStopGoToBlkDie;					//v3.30
	static BOOL		m_bBlkLongJump;						//v3.25T16
	static BOOL		m_bIsWaferPrRPY1Done;				//v3.31		//PR RPY flag for IM PR logging
	static BOOL		m_bWaferAlignComplete;				// false, dim stop button, true, enable (default is enable)

	static BOOL	m_bCleanColletPocketOn;					//v3.68T4
	static BOOL m_bPrAoiSwitchCamera;					//v3.93

// prescan relative code	B
	static DOUBLE	m_dStartWaferAlignTime;
	static BOOL		m_bMapOHFlip;
	static BOOL		m_bMapOVFlip;
	static USHORT	m_usMapAngle;

	static BOOL		m_bSmallMapSortRpt;
	static BOOL		m_bES100DisableSCN;
	static BOOL		m_bWprWithAF_Z;
	static BOOL		m_bPsmEnableState;
	static BOOL		m_bDisablePsm;
	static BOOL		m_bHmiToUseCharDie;
	static BOOL		m_bOCRBarwaferNoMap;
	static BOOL		m_bES100ByMapIndex;
	static LONG		m_nPrescanIndexStepRow;
	static LONG		m_nPrescanIndexStepCol;
	static	CString	m_szAoiOcrPrValue;

	static BOOL		m_bBinFrameCheckBCFail;
	static BOOL		m_bPrescanBlkPickEnabled;
	static BOOL		m_bBlkFuncEnable;
	static BOOL		m_bFullRefBlock;
	static BOOL		m_bGeneralStopCycle;
	static BOOL		m_bAdaptPredictAlgorithm;			//ATS Adapt Predict Algorithm to avoid mlyer-shift
	static BOOL		m_bPrescan2ndTimeStart;
	static BOOL		m_bPrescanTwiceEnable;
	static LONG		m_lPrescanVerifyResult;
	static BOOL		m_bEnableWafflePadBonding;
	static BOOL		m_bCheckDieOrientation;
	static BOOL		m_bEnableCheckDieOrientation;
	static UCHAR	m_ucDummyPrescanPNPGrade;

	static BOOL		m_bNgPickBadCut;
	static BOOL		m_bNgPickDefect;
	static BOOL		m_bNgPickEdgeDie;
	static BOOL		m_bToPickDefectDie;
	static BOOL		m_bPrescanEmptyAction;
	static BOOL		m_bPrescanEmptyToNullBin;
	static UCHAR	m_ucPrescanEmptyGrade;
	static LONG		m_lPrescanFinishedAction;
	static	BOOL	m_bPrescanDefectAction;
	static BOOL		m_bPrescanBadCutAction;
	static BOOL		m_bPrescanBadCutToNullBin;
	static UCHAR	m_ucPrescanBadCutGrade;
	static BOOL		m_bPrescanDefectToNullBin;
	static UCHAR	m_ucPrescanDefectGrade;
	static UCHAR	m_ucScanMapStartNgGrade;
	static UCHAR	m_ucScanMapEndNgGrade;

	static	BOOL	m_bNgPickExtra;
	static	BOOL	m_bScanExtraDieAction;
	static UCHAR	m_ucScanExtraGrade;
	static	BOOL	m_bScanDetectFakeEmpty;
	static UCHAR	m_ucScanFakeEmptyGrade;
	static UCHAR	m_ucScanMapStartAOINgGrade;
	static UCHAR	m_ucScanMapEndAOINgGrade;
	static UCHAR	m_ucScanMapNgList[255];
	static UCHAR	m_ucScanMapNgListSize;
	static UCHAR	m_ucScanMapAOINgList[255];
	static UCHAR	m_ucScanMapAOINgListSize;
	static BOOL		m_bPrescanSkipNgGrade;
	static BOOL		m_bPrescanSkipAOINgGrade;
	static UCHAR	m_ucPrescanEdgeSize;
	static UCHAR	m_ucPrescanEdgeGrade;

	static	BOOL	m_bEnableAutoCycleLog;
	static	DOUBLE	m_dPreviousUpdateTime;

	static CString	m_szCycleMessageWT;
	static CString	m_szCycleMessageWP;
	static CString	m_szCycleMessageWL;
	static CString	m_szCycleMessageBH;
	static CString	m_szCycleMessageBT;
	static CString	m_szCycleMessageBP;
	static CString	m_szCycleMessageBL;
	static CString	m_szCycleMessageST;
	static CString	m_szCycleMessageSG;
	static CString	m_szCycleMessageNL;		//andrewng //2020-0708

	static	LONG	m_lBuildShortPathInScanning;
	static	BOOL	m_bPrescanWaferEnd4ShortPath;
	static	BOOL	m_bShowNgDieSubGrade;
	static	BOOL	m_bSaveNgDieSubGrade;

	static UCHAR	m_ucScanWalkTour;
	static	BOOL	m_bPrescanGoodAction;
	static UCHAR	m_ucPrescanGoodGrade;
	static	BOOL	m_bToUploadScanMapFile;
	static	BOOL	m_bScnCheckByPR;
	static	BOOL	m_bScnCheckIsRefDie;
	static	BOOL	m_bErrorChooseGoFPC;
	static	BOOL	m_bKeepSCNAlignDie;
	static	LONG	m_lTotalSCNCount;
	static	LONG	m_lScnCheckRow[WT_ALN_MAXCHECK_SCN];  //Original User
	static	LONG	m_lScnCheckCol[WT_ALN_MAXCHECK_SCN];
	static	LONG	m_lScnCheckRefDieNo;

	static	CString	m_szOutputScanSummaryPath;
	static	CString	m_szSaveImagePath;		// dubug purpos, log special case image/files/screen.
	static	CString m_szSaveMapImagePath;
	static	CString	m_szScanNgImageLogPath;
	static	DOUBLE	m_dPrescanGoodPassScore;
	static	DOUBLE	m_dPrescanFailPassScore;
	static	DOUBLE	m_dCurrentScore;
	static	CString	m_szCurrentGrade;
	static	ULONG	m_ulMapBaseNum;
	BOOL GetFPCMapPositionWpr(LONG lIndex, ULONG &ulAsmRow, ULONG &ulAsmCol, LONG &lUserRow, LONG &lUserCol);
	BOOL GetErrorChooseGoFPC();

	static BOOL		m_bPrescanJustFinishToProbe;

	static BOOL		m_bIsAutoLearnWaferNOTDone;

	static LONG		m_lBondPrStopped;
	static LONG		m_lBondHeadAtSafePos;
	static LONG		m_lBinTableStopped;
	static BOOL		m_bStopAllMotion;

	static BOOL		m_bRenameMapNameAfterEnd;

	static BOOL		m_bAlwaysLoadMapFileLocalHarddisk;

	static LONG		m_lSetDetectSkipMode;
	static BOOL		m_bEnableAssocFile;
	static CString	m_szAssocFileExt;

	static BOOL	m_bScnLoaded;			// Flag to indicate whether SCN file is loaded

	static	BOOL	m_bEnableSCNCheck;	
	static	LONG	m_lOutputScanSummary;
	static 	CString m_szScanAoiMapFormat;

	//==============================
	// sort mode attributes
	//==============================
	static ULONG	m_ulMS90HalfBorderMapRow;
	static LONG		m_lMS90HalfDivideMapOrgRow;
	static ULONG	m_ulMS90HalfBorderMapCol;
	static LONG		m_lMS90HalfDivideMapOrgCol;

	static BOOL		m_bScanWithWaferLimit;
	static BOOL		m_bScanToHalfMap;

	static BOOL	m_bMS90HalfSortMode;			//MS90
	static BOOL	m_b2Parts1stPartDone;			//MS90
	static BOOL	m_b2Parts2ndPartStart;			//MS90	//v4.59A17
	static BOOL	m_bSortGoingTo2ndPart;
	static BOOL	m_b2PartsAllDone;
	static BOOL	m_bSoraaSortMode;
	static BOOL m_bDualWayTipWafer;
	static BOOL m_bSearchHomeOption;
	static ULONG	m_ulCornerSearchOption;
	static UCHAR	m_ucPLLMWaferAlignOption;	//0=DEFAULT, 1=REBEL_TILE, 2=REBEL_U2U		//v4.46T25
	static BOOL	m_bOsramResortMode;				//Osram Penang RESORT mode	//v4.21T3
	static BOOL	m_bMS90RotatedBT;
	static LONG	m_lBinCalibX;
	static LONG	m_lBinCalibY;

	static UCHAR	m_ucMapRotation;		//0 = 0; 1 = 90; 2 = 180; 3 = 270
	static BOOL		m_bOsramDenyHMI;
	static BOOL		m_bEnableOSRAM;
	static	BOOL	m_bEnableSyncMove;
	static	LONG	m_lSyncDistance;
	static	DOUBLE	m_dSyncSpeed;

	static	DOUBLE		m_dWL1CalibXX;
	static	DOUBLE		m_dWL1CalibXY;
	static	DOUBLE		m_dWL1CalibYY;
	static	DOUBLE		m_dWL1CalibYX;
	static	LONG		m_lWL1WaferOffsetX;
	static	LONG		m_lWL1WaferOffsetY;
	static	DOUBLE		m_dWL2CalibXX;
	static	DOUBLE		m_dWL2CalibXY;
	static	DOUBLE		m_dWL2CalibYY;
	static	DOUBLE		m_dWL2CalibYX;
	static	LONG		m_lWL2WaferOffsetX;
	static	LONG		m_lWL2WaferOffsetY;
	static	BOOL		m_bWL1Calibrated;
	static	BOOL		m_bWL2Calibrated;
	LONG GetPrInContour(BOOL bWT2, LONG lX, LONG lY, DOUBLE &dPrX, DOUBLE &dPrY);
	VOID ContourLog(CString szMsg, CONST BOOL bWT2, CONST BOOL bNew=FALSE);
	static  BOOL CheckDiskSize(LPCSTR lpDiskName, ULONG &lDiskSpaceUsed, ULONG &lDiskSpaceRemain);
	BOOL CheckDiskSpace(CString szDiskLabel, DOUBLE dFreeSpacePct);		//v4.50A3

	//==============================
	// Temperature Controller attributes
	//==============================
	static LONG	m_lTCUpHeatingTime;
	static LONG	m_lTCDnOnVacDelay;
	static LONG m_lTCUpPreheatTime;
	static BOOL m_bTCInUse;		// LiTec ATS thermal heater use RS232 to control. not used in deed
	static BOOL m_bTEInUse;		// thermal eject, use OMRON heater and an alarm bit to check heater status.
	static BOOL	m_bColorCamera;
	static BOOL m_bTCLayerPick;
	static LONG	m_lLevel;					// Current alarm lamp level


	static BOOL m_bFreezeCamera_HMI;

	//======================== Prober======================== //
	static LONG		m_lPSCMode;
	static	CString	m_szOutputMapFullPath;
	static	CString	m_szSourceMapFullPath;
	static	DOUBLE	m_dPrDieCenterX;
	static	DOUBLE	m_dPrDieCenterY;

	//v4.39T7	//Nichia MS100+
	//============== BL related static objects ============== //
	static CSfmSerialPort      m_oCommPort;
	static CSfmSerialPort      m_oCommPort2;				//FOr MS100 9Inch dual table config
	static BL_MAGAZINE m_stMgznRT[MS_BL_MGZN_NUM];			// magazine run time settings
	static CMSNichiaMgntSubSystem	m_oNichiaSubSystem;		//v4.40T1
	static CMSSemitekMgntSubSystem	 m_oSemitekSubSystem;
	static ULONG	m_ulCommInterval;
	static UCHAR	m_ucCommRetry;

	static BOOL m_bEnableUseProbeTableLevelByRegion;
	static PROBE_TABLE_LEARN_POINT m_stLearnLevelCornerPoints[MS_LEARN_LEVEL_CORNER_POINT];
	static PROBE_TABLE_LEVEL_DATA	m_stProbeTableLevelData[MS_PROBETABLE_LEVEL_DATA_NO];
	static LONG m_lProbeTableDataSamplesX;
	static LONG m_lProbeTableDataSamplesY;
	static LONG m_lProbeTableDataSamplesPitchX;
	static LONG m_lProbeTableDataSamplesPitchY;

	static CString	m_szBtTBarCode;			//v4.40T5
	static	BOOL	m_bMS100EjtXY;			//v4.42T3
	static	BOOL	m_bEnableMS100EjtXY;	//v4.42T3
	static BOOL		m_bSemitekBLMode;		//v4.42T9
	
	static BOOL		m_bEnableEjectorTheta;
	static BOOL		m_bEnableBinEjector;

	static BOOL		m_bEnableWTTheta;

	static UINT		m_nWTAtColletPos;
	static UINT		m_nWTLastColletPos;		//v4.47T2

//	static BOOL		m_bBTIsMoveCollet;
	static BOOL		m_bChangeCollet1;
	static BOOL		m_bChangeCollet2;

	static BOOL		m_bEnableClearBinCopyTempFile;

	static	CString m_szWT1YieldState;	//ES101(contour): saving the current wafer status for seperate "X" out 
	static	CString m_szWT2YieldState;	//ES101(contour): saving the current wafer status for seperate "X" out 
	static	BOOL	m_bUseContour;
	static	LONG	m_lWL1CenterX;
	static	LONG	m_lWL1CenterY;
	static	LONG	m_lWL2CenterX;
	static	LONG	m_lWL2CenterY;
	static  BOOL	m_bSubBinFull;//enablesubbin
	static  LONG	m_lSubBinGrade;
	static CString m_szAlarmLampStatusReportPath;

protected:

	CStringMapFile	*m_psmfMachine;
	CStringMapFile	*m_psmfDevice;
	CStringMapFile	*m_psmfSRam;

	void			*m_pvNVRAM;				// Non-volatile RAM (SW memory)
	void			*m_pvNVRAM_HW;			// Non-volatile RAM (HW)

	CEvent			m_evComplete;
	BOOL			m_bNeedReset;
	CString			m_szEventPrefix;
	CString			m_szSubStationName;
	LONG			m_lManualAction;
	BOOL			m_bDebugPrint;			// For debug print
	BOOL			m_bHardwareReady;
	LONG            m_lEquipState;
	LONG            m_lPreEquipState;
	BOOL			m_bEquipmentManager;	//v4.59A32
	// for data log
	UINT			m_uiMotionDataLogSize;

	static			BOOL m_bSignalStopCycle;	//v4.52A11	//from private to protected member

	static CAgmpCtrlPIDMotorThermalCtrl *m_pThermalCtrl;

	//NVC Motion Test
	static BOOL	m_bMotionCycleStop;
	static BOOL	m_bMoveDirection;
	static LONG	m_lMotionTestDelay;
	static UCHAR m_ucMotionAxis;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
	static LONG	m_lMotionOrigEncPosn;
	static LONG	m_lMotionTarEncPosn;
	static LONG	m_lMotionCycle;
	static LONG	m_lMotionCycleCount;


private:
	static CMutex	m_csDispMsg;
	//==============================
	// Critical section
	//==============================
	static CCriticalSection m_CSect;
	static CCriticalSection	m_csMachineStop;
	static CCriticalSection	m_csStopToken;
	static CCriticalSection m_csBackupNVRam;
	static CCriticalSection m_csBackupNVRamMotionErr;
	static CCriticalSection m_csMachineAlert;
	static CCriticalSection m_csLastTestLock;
	static CCriticalSection m_csBLBarcode;		//v4.22T1	//andrew
	static CCriticalSection m_csBLNichiaWafIDList;		//v4.40T6	
	//static CCriticalSection m_csMachineAlarmLamp;

	static LONG	m_lOrderCode;		
	static BOOL	m_bAllowHouseKeeping;
	static BOOL	m_bBackupNVRamMotionErr;

	BOOL m_bIsSIOLinkFailed;

};

