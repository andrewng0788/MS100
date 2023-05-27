

#pragma once

#include "MS896AStn.h"
#include "MS896A.h"
#include "WT_SubRegion.h"
#include "WaferSrch_Var.h"

#define	SCN_ADD_REF_MAX		50
#define	SCN_POINTA_NUM		2500

#define	WT_SRCH_EDGE_RGT	0
#define	WT_SRCH_EDGE_T_R	1
#define	WT_SRCH_EDGE_TOP	2
#define	WT_SRCH_EDGE_T_L	3
#define	WT_SRCH_EDGE_LFT	4
#define	WT_SRCH_EDGE_B_L	5
#define	WT_SRCH_EDGE_BTM	6
#define	WT_SRCH_EDGE_B_R	7
#define	WT_SRCH_EDGE_MAX	8.0

#define	WT_SRCH_EDGE_LMT	1.01

#define	LOCALHD_MAP_PATH	gszROOT_DIRECTORY + "\\Exe\\Map"

// Temperature Controller	Thermal Ejector
#define MS899_TC_SERIAL_PORT_NO_1			1
#define	MS899_TC_EJECTOR_CHANNEL			7

#define lsbbit0 0x0001                  
#define lsbbit1 0x0002
#define lsbbit2 0x0004
#define lsbbit3 0x0008
#define lsbbit4 0x0010
#define lsbbit5 0x0020
#define lsbbit6 0x0040
#define lsbbit7 0x0080

#define msbbit0 0x0100
#define msbbit1 0x0200
#define msbbit2 0x0400
#define msbbit3 0x0800
#define msbbit4 0x1000
#define msbbit5 0x2000
#define msbbit6 0x4000
#define msbbit7 0x8000


typedef struct 
{
	BOOL bIsSetupComplete;
	UCHAR ucWaferLimitType;					// Circle / Polygon Wafer limit
	LONG lWaferPolyLimitNo;
	LONG lWaferPolyLimitX[MS_WAFER_LIMIT_POINTS];
	LONG lWaferPolyLimitY[MS_WAFER_LIMIT_POINTS];
} WAFER_TABLE_LIMIT_TEMPLATE;

#define		WT_LA_CTR_DIE	4	//	6	xu_semitek_2
#define		WT_LA_MAX_DIE	(2*WT_LA_CTR_DIE+1)
typedef struct
{
	SHORT	m_sDieState[WT_LA_MAX_DIE][WT_LA_MAX_DIE];
	LONG	m_sDiePosnX[WT_LA_MAX_DIE][WT_LA_MAX_DIE];
	LONG	m_sDiePosnY[WT_LA_MAX_DIE][WT_LA_MAX_DIE];
} WT_LA_DIE_RESULT;

#define	MSD_MAX_DIM		5
typedef struct {
	BOOL	bDieState[MSD_MAX_DIM];
} MULTI_SRCH_RESULT;

typedef struct 
{
	BOOL	m_bDieAFound;
	ULONG	m_ulDieARow;
	ULONG	m_ulDieACol;
	LONG	m_lDieAScanX;
	LONG	m_lDieAScanY;
	LONG	m_lDieAMsPrX;
	LONG	m_lDieAMsPrY;
	BOOL	m_bAngleFound;
	DOUBLE	m_dMsPrAngle_X;
	DOUBLE	m_dMsPrAngle_Y;
	DOUBLE	m_dScanAngle_X;
	DOUBLE	m_dScanAngle_Y;
} SCN_ALN_POINT_ADJUST;

// Die Info
typedef	struct 
{
	LONG	lX;
	LONG	lY;
	LONG	lX_Enc;
	LONG	lY_Enc;
	LONG	lT_Enc;
	ULONG	ulStatus; 
	unsigned char	ucGrade;
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
	DOUBLE	dDegree;
} DIE_INFO;

// Look Forward Info
typedef struct {
	LONG	lDirection;
	BOOL	bRotate;
	BOOL	bFullDie;
	BOOL	bGoodDie;		//v2.96T3
	LONG	lX;
	LONG	lY;
	DOUBLE	dTheta;
	LONG	lWM_X;
	LONG	lWM_Y;
} LF_INFO;

typedef struct
{
//	LONG	m_lSrcRow;
//	LONG	m_lSrcCol;
	LONG	m_lAsmRow;
	LONG	m_lAsmCol;
	LONG	m_lWftPosX;
	LONG	m_lWftPosY;
	BOOL	m_bOnMap;
	BOOL	m_bOnWaf;

} ReferDieMapWaferData;

typedef struct TC_Data_Struct
{
	BOOL	m_bTCEnable;
	BOOL	m_bOnPower;
	BOOL	m_bCheckTmp;
	BOOL	m_bCheckAlarmBit;

	LONG	m_lTargetTmp;
	LONG	m_lTmpTolerance;

	LONG	m_lTuneTmp;
	LONG	m_lActualTmp;
	BOOL	m_bTuneCalibrated;
} TC_Data;

static CString g_aszWTState[40] =
{	
	"Get Wafer POS",					// GET_WAFER_POS_Q
	"Wait Ej Ready",					// WAIT_EJECTOR_READY_Q
	"Move Table",						// MOVE_TABLE_Q
	"Wait PR Delay",					// WAIT_PR_DELAY_Q
	"Wait Die Ready",					// WAIT_DIE_READY_Q
	"Prescan Get Wafer POS",			// PRESCAN_GET_WAFER_POS_Q
	"Prescan Move Table",				// PRESCAN_MOVE_TABLE_Q
	"Prescan Wait PR Delay",			// PRESCAN_WAIT_PR_DELAY_Q
	"Prescan Wait Die Ready",			// PRESCAN_WAIT_DIE_READY_Q
	"Prescan Wait All Done",			// PRESCAN_WAIT_ALL_DONE_Q
	"Prescan AP Get Wafer Pos",			// PRESCAN_AP_GET_WAFER_POS_Q
	"prescan Area Pick Next Die"		// PRESCAN_AP_NEXT_DIE_Q
	"Prescan Area Pick Move Table",		// PRESCAN_AP_MOVE_TABLE_Q,
	"Prescan Area Pick Wait Die Ready",	// PRESCAN_AP_WAIT_READY_Q,
	"Check Info Read",					// CHECK_INFO_READ_Q
	"Compensation",						// COMPENSATION_Q
	"Wait Compensation",				// WAIT_COMPENSATION_COMPLETE_Q
	"Bad Die Compensation",				// BADDIE_COMPENSATION_Q
	"Wait Info Read",					// WAIT_INFO_READ_Q
	"Wait LF Ready",					// WAIT_LF_READY_Q
	"Next Die",							// WT_NEXT_DIE_Q
	"Wafer End",						// WAFER_END_HANDLING_Q
	"Home",								// HOME_Q
	"Wait Bin Full",					// WT_899_WAIT_BIN_FULL_Q
	"House Keeping",					// HOUSE_KEEPING_Q
	"Update Last Die"					// UPDATE_LAST_DIE_Q
};

enum
{
	WTMC_ENUM_START = 0,
	WTMC_RscnPitchUpTol = 0,
	WTMC_MWMV_DieLimit,	//	MWM	Map Wafer Match Verify
	WTMC_MWMV_PassScore,
	WTMC_MWMV_DoLimit,
	WTMC_DAR_IdleLowLmt,
	WTMC_DAR_IdleUpLmt,
	WTMC_AdvUpdUpPcntLmt,
	WTMC_AdvSmpJumpLmt,
	WTMC_AdvSmpCtrToEdge,
	WTMC_AdvSmpCtrFrqc,
	WTMC_AdvSmpCtrBase,
	WTMC_AP_C_MIN_T,
	WTMC_AP_R_MIN_T,
	WTMC_ENUM_END
};

// Operation sub-state
enum {	GET_WAFER_POS_Q,
		WAIT_EJECTOR_READY_Q,
		MOVE_TABLE_Q,
		WAIT_PR_DELAY_Q,
		WAIT_DIE_READY_Q,

		PRESCAN_GET_WAFER_POS_Q,
		PRESCAN_MOVE_TABLE_Q,
		PRESCAN_WAIT_PR_DELAY_Q,
		PRESCAN_WAIT_DIE_READY_Q,
		PRESCAN_WAIT_ALL_DONE_Q,
		PRESCAN_AP_GET_WAFER_POS_Q,
		PRESCAN_AP_NEXT_DIE_Q,
		PRESCAN_AP_MOVE_TABLE_Q,
		PRESCAN_AP_WAIT_READY_Q,

		CHECK_INFO_READ_Q,
		COMPENSATION_Q,
		BADDIE_COMPENSATION_Q,
		WAIT_COMPENSATION_COMPLETE_Q,
		WAIT_INFO_READ_Q,
		WAIT_LF_READY_Q,
		WT_NEXT_DIE_Q,
		WAFER_END_HANDLING_Q,
		REPEAT_CYCLE_Q,
		WAIT_LOAD_MAP_Q,
		HOME_Q,
		WT_899_WAIT_BIN_FULL_Q,
		HOUSE_KEEPING_Q,
		UPDATE_LAST_DIE_Q,
		DIE1_FINDER_Q,
		WAIT_PR_DELAY_MS60_Q,
		WAIT_GRAB_READY_MS60_Q
	};

#define	CP_GET_WAFER_POS_Q		GET_WAFER_POS_Q
#define	CP_WAIT_PROBEZ_READY_Q	WAIT_EJECTOR_READY_Q
#define	CP_MOVE_TABLE_Q			MOVE_TABLE_Q
#define	CP_WAIT_PR_DELAY_Q		WAIT_PR_DELAY_Q
#define	CP_WT_NEXT_DIE_Q		WT_NEXT_DIE_Q
#define	CP_WAFER_END_HANDLING_Q	WAFER_END_HANDLING_Q
#define	CP_WT_HOUSE_KEEPING_Q	HOUSE_KEEPING_Q
#define	CP_UPDATE_LAST_DIE_Q	UPDATE_LAST_DIE_Q


#define		WAFERMAPTEMP_BACKUP_FOLDER	"C:\\MapSorter\\UserData\\WMTBK"
#define		WAFERMAPTEMP_FOLDER			"C:\\MapSorter\\EXE\\WaferMapTemp"


class CWaferTable_Var : public CMS896AStn
{
//	DECLARE_DYNCREATE(CWaferTable_Var)

public:
	CWaferTable_Var()	{};
	virtual	~CWaferTable_Var() {};
	LONG	m_lPresortWftStatus;	// before sorting, the align, prescan ... status, if not ok, no sorting.
	INT		m_nMS60CycleCount;

protected:
	CString m_szGradeDisplayMode; // 4.50D10
	LONG	m_lGradeDisplayMode;
	BOOL    m_bChangeColorToggle;
	BOOL	m_bUseDualTablesOption;	//ES101 option						//v4.24
	BOOL	m_bUseES101v11TableConfig;		//ES101 new WT config with only INDEX sensor	//v4.28T4
	LONG	m_lChangeLightSCNContinue;

	BOOL	m_bEJTLLimit;
	BOOL	m_bEJTULimit;

	LONG	m_lX_ProfileType;
	LONG	m_lY_ProfileType;

	BOOL	m_bUpdateOutput;	// Update Sensor & encoder in HMI
	BOOL	m_bHome_X;			// X Home flag
	BOOL	m_bHome_Y;			// Y Home flag
	BOOL	m_bHome_T;			// T Home flag
	BOOL	m_bComm_X;			// X Communtate flag
	BOOL	m_bComm_Y;			// Y Communtate flag
	BOOL	m_bComm_T;			// T Communtate flag
	BOOL	m_bHome_X2;			// X Home flag
	BOOL	m_bHome_Y2;			// Y Home flag
	BOOL	m_bHome_T2;			// T Home flag
	BOOL	m_bComm_X2;			// X Communtate flag
	BOOL	m_bComm_Y2;			// Y Communtate flag
	BOOL	m_bComm_T2;			// T Communtate flag

	BOOL	m_bSel_X;			// Select X flag
	BOOL	m_bSel_Y;			// Select Y flag
	BOOL	m_bSel_T;			// Select T flag
	BOOL	m_bSel_X2;			// Select X flag
	BOOL	m_bSel_Y2;			// Select Y flag
	BOOL	m_bSel_T2;			// Select T flag

	//v4.24T9	//ES101 dual table config
	LONG	m_lXNegLimit;
	LONG	m_lXPosLimit;
	LONG	m_lYNegLimit;
	LONG	m_lYPosLimit;
	LONG	m_lX2NegLimit;
	LONG	m_lX2PosLimit;
	LONG	m_lY2NegLimit;
	LONG	m_lY2PosLimit;

	BOOL	m_bStopBinLoader;	

	LONG	m_lLastPickRow;
	LONG	m_lLastPickCol;
	BOOL	m_bLastPickSet;

	DIE_INFO	m_stCurrentDie;
	DIE_INFO	m_stLastDie;
	LF_INFO		m_stLFDie;
	LF_INFO		m_stLastLFDie;				//v3.01T1
	LONG		m_lGetNextDieRow;
	LONG		m_lGetNextDieCol;

	UCHAR		m_ucLastPickDieGrade;
	BOOL		m_bLastDieUpdate;
	BOOL		m_bMapWaferVerifyOK;
	ULONG		m_ulVerifyMapWaferCounter;
	BOOL		m_bStartGoToAlignDie;		//v3.30T1

	// Die Compensation Info
	LONG	m_lCompen_X;
	LONG	m_lCompen_Y;
	LONG	m_lCompen_T;
	LONG	m_lOrgDieCompen_X;
	LONG	m_lOrgDieCompen_Y;

	// Start encoder count of Die for cycle
	LONG	m_lStart_X;
	LONG	m_lStart_Y;
	LONG	m_lStart_T;
	// Move Position without collet offset
	LONG	m_lX_NoEjOffset;
	LONG	m_lY_NoEjOffset;

	// Sensor State
	BOOL	m_bHomeSnr_X;		// X Home Sensor
	BOOL	m_bHomeSnr_Y;		// Y Home Sensor
	BOOL	m_bHomeSnr_T;		// T Home Sensor
	BOOL	m_bLimitSnr_X;		// X Limit Sensor
	BOOL	m_bLimitSnr_Y;		// Y Limit Sensor

	BOOL	m_bHomeSnr_X2;		// X Home Sensor
	BOOL	m_bHomeSnr_Y2;		// Y Home Sensor
	BOOL	m_bHomeSnr_T2;		// T Home Sensor
	BOOL	m_bLimitSnr_X2;		// X Limit Sensor
	BOOL	m_bLimitSnr_Y2;		// Y Limit Sensor

	// Encoder count
	LONG	m_lEnc_X;			// X Encoder count
	LONG	m_lEnc_Y;			// Y Encoder count
	LONG	m_lCmd_X;			// Y Cnd Value
	LONG	m_lCmd_Y;			// Y Cnd Value
	LONG	m_lEnc_T;			// T Encoder count
	LONG	m_lEnc_X1;			// X Encoder count
	LONG	m_lEnc_Y1;			// Y Encoder count
	LONG	m_lCmd_Y1;			// Y Cmd Value
	LONG	m_lEnc_T1;			// T Encoder count
	LONG	m_lEnc_X2;			// X Encoder count
	LONG	m_lEnc_Y2;			// Y Encoder count
	LONG	m_lCmd_Y2;			// Y Cmd Value
	LONG	m_lEnc_T2;			// T Encoder count

	LONG	m_lMotorDirectionX;
	LONG	m_lMotorDirectionY;

	// Tolerance for Die Pitch Check
	LONG	m_lCorMovePitchTolX;
	LONG	m_lCorMovePitchTolY;
	LONG	m_lCorBackPitchTolX;
	LONG	m_lCorBackPitchTolY;

	LONG	m_lPitchX_Tolerance;
	LONG	m_lPitchY_Tolerance;
	LONG	m_lPredScanPitchTolX;
	LONG	m_lPredScanPitchTolY;		
	LONG	m_lRunPredScanPitchTolX;
	LONG	m_lRunPredScanPitchTolY;		
	LONG	m_lRealignDieNum;		//Block2

	// XY Table & Theta resolution
	DOUBLE	m_dEjtXYRes;
	DOUBLE	m_dXYRes;
	DOUBLE	m_dThetaRes;
	LONG	m_lThetaMotorDirection;

	LONG	m_lRotateCount;		// Encoder count for rotation test
	LONG	m_lJoystickLevel;	// Joystick Speed Level
	BOOL	m_bStartPoint;		// Flag to indicate whether it is starting at selected die
	BOOL	m_bDiePitchCheck;	// Flag to indicate whether die pitch check
	BOOL	m_bAbsDiePitchCheck;// Flag to indicate whether die pitch check		//v2.78T2

	//String map file name
	CString m_szWTConfigFileName;

	INT m_nProfile_X;
	INT m_nProfile_Y;
	INT m_nProfile_X2;
	INT m_nProfile_Y2;
	INT m_nProfile_T;
	INT m_nProfile_T2;
	
	//v4.47T3	
	BOOL	m_nChangeColletOnWT;
	BOOL	m_bUseMS60NewLFCycle;
	BOOL	m_bMS60NewLFInLastCycle;
	DIE_INFO	m_stMS60NewLFDie;
	DIE_INFO	m_stMS60LastLFDie;
	BOOL	m_bSyncWTAndEjectorInBond;
	BOOL	m_bUseLF2ndAlignment;
	BOOL	m_bUseLF2ndAlignNoMotion;	//andrewng //2020-05-19

	// Motion Time
	LONG	m_lTime_WT;
	DOUBLE  m_dWTStartTime;
	DOUBLE	m_dDelayTime_X;
	DOUBLE	m_dDelayTime_Y;

	BOOL	m_bDieIsGet;			// Indicate Die is get from wafermap
	BOOL	m_bLoadMap;				// Flag to indicate whether the map is loaded
	BOOL	m_bFirst;				// First cycle flag
	BOOL	m_bFirstInAutoCycle;	//	first time run in auto cycle, to check disk free space.
	BOOL	m_bAlignedWafer;		// Wafer is aligned
	BOOL	m_bJustAlign;			// Just align wafer
	BOOL	m_bIsUnload;			// Is wafer table in unload pos
	BOOL	m_bResetStartPos;		// Reset map start position
	BOOL	m_bManualAddReferPoints;

	// Delay
	LONG	m_lReadyDelay;			// Delay for BH Z down (include theta delay)
	LONG	m_lMinDelay;			// Minimum Delay for BH Z down (not include theta delay)
	LONG	m_lTDelay;				// Wafer Table Theta Delay per degree
	
	LONG	m_lLongJumpDelay;		//MS100Plus
	LONG	m_lLongJumpMotTime;		//MS100Plus

	// Motor count after table move
	LONG	m_lCurrent_X;			
	LONG	m_lCurrent_Y;			
	
	LONG	m_lTime_CP;				// Time of Compensation
	BOOL	m_bGoodDieFound;		// Whether at least one good die is found
	BOOL	m_bGoodDie;				// Current die is a good die
	BOOL	m_bInfoWrite;			// Flag to indicate whether the die info is written
	BOOL	m_bBTInfoRead;
	BOOL	m_bStartCalibrate;
	BOOL	m_bStartCalibrate2;
	BOOL	m_bRotate;				// Whether WT Rotation is needed
	BOOL	m_bConfirmSrch;			// Flag to indicate whether Confirm Search is required
	
	// Power On/Off
	BOOL	m_bIsPowerOn_X;			//Whether X axis is power on
	BOOL	m_bIsPowerOn_Y;			//Whether Y axis is power on
	BOOL	m_bIsPowerOn_T;			//Whether T axis is power on
	BOOL	m_bIsPowerOn_X2;		//Whether X axis is power on
	BOOL	m_bIsPowerOn_Y2;		//Whether Y axis is power on
	BOOL	m_bIsPowerOn_T2;		//Whether T axis is power on
	LONG	m_lWaferUnloadX;			
	LONG	m_lWaferUnloadY;
	LONG	m_lWaferCalibX;
	LONG	m_lWaferCalibY;
	LONG	m_lWaferBeforeCalibT;
	LONG	m_lWaferCalibX2;
	LONG	m_lWaferCalibY2;
	LONG	m_lWaferBeforeCalibT2;

	LONG	m_lWaferType;
	LONG	m_lWaferWorkingAngle;
	LONG	m_lWaferCurrentWorkingAngle;		
	DOUBLE	m_dWaferDiameter;
	LONG	m_lWaferIndexPath;
	LONG	m_lWaferEmptyLimit;
	LONG	m_lPNPWftIndexLimit;
	
	LONG	m_lWaferTmpX[MS_WAFER_CIRCULAR_LIMIT_PTS];		
	LONG	m_lWaferTmpY[MS_WAFER_CIRCULAR_LIMIT_PTS];		

	//v3.44	//SHowaDenko COR calculation
	BOOL	m_bUseMultiCorOffset;
	DOUBLE	m_dCorOffsetA1;		
	DOUBLE	m_dCorOffsetB1;
	DOUBLE	m_dCorOffsetA2;
	DOUBLE	m_dCorOffsetB2;
	DOUBLE	m_dACorOffsetA1;
	DOUBLE	m_dACorOffsetB1;
	DOUBLE	m_dACorOffsetA2;
	DOUBLE	m_dACorOffsetB2;

	ULONG	m_ulCurrentRow;
	ULONG	m_ulCurrentCol;
	ULONG	m_ulAlignRow;
	ULONG	m_ulAlignCol;
	LONG	m_lAlignPosX;
	LONG	m_lAlignPosY;
//	LONG	m_lLHSIndexCount;
//	LONG	m_lRHSIndexCount;
	BOOL	m_bAutoAlignWafer;
	// for semitek to find global theta at another position
	BOOL	m_bNewGTPosition;
	LONG	m_lNewGTPositionX;
	LONG	m_lNewGTPositionY;
	LONG	m_lNewGTPositionX2;
	LONG	m_lNewGTPositionY2;
	LONG	m_bNewGTUse2;

	// five points check for SCN and prescan	begin
	ULONG	m_ulScnRefDieRow;
	ULONG	m_ulScnRefDieCol;
	BOOL	m_bScnCheckDieAsRefDie;

	LONG	m_lSCNIndex_X;
	LONG	m_lSCNIndex_Y;

	BOOL	m_bScnCheckNormalOnRefer;
	BOOL	m_bCheckSCNLoaded;
	BOOL	m_bEnableScnCheckAdjust;
	LONG	m_lRefHomeDieRowOffset;
	LONG	m_lRefHomeDieColOffset;
	LONG	m_lRefHomeDieRow;
	LONG	m_lRefHomeDieCol;
	LONG	m_lScnCheckXTol;
	LONG	m_lScnCheckYTol;
	LONG	m_lScnCheckAlarmLimit;
	BOOL	m_bCheckNullBinInMap;
	BOOL	m_bCheckMapHomeNullBin;

	LONG	m_lReferMapTotal;
	LONG	m_lReferMapRow[WT_ALN_MAXCHECK_SCN];
	LONG	m_lReferMapCol[WT_ALN_MAXCHECK_SCN];

	// SCN align relative settings
	BOOL	m_bAoiOcrLoaded;
	BOOL	m_bLoadedWaferScannedOnce;

	//SCN_ALN_POINT_ADJUST	m_stScnAlnPoints[WT_ALN_MAXCHECK_SCN];
	LONG	m_lScnAlignMethod;
	BOOL	m_bScnAlignDieAsRefer;
	BOOL	m_bScnAlignUseRefer;
	LONG	m_lScnAlignTotalPoints;
	LONG	m_lScnAlignReferNo;
	BOOL	m_bScnPrescanIfBonded;
	LONG	m_laScnAlignRow[WT_ALN_MAXCHECK_SCN];
	LONG	m_laScnAlignCol[WT_ALN_MAXCHECK_SCN];

	LONG	m_lScnHmiRow;
	LONG	m_lScnHmiCol;

	LONG	m_lEsScanDieA_X[SCN_ADD_REF_MAX];
	LONG	m_lEsScanDieA_Y[SCN_ADD_REF_MAX];
	LONG	m_lMsSrchDieA_X[SCN_ADD_REF_MAX];
	LONG	m_lMsSrchDieA_Y[SCN_ADD_REF_MAX];
	DOUBLE	m_dCosThetaTran[SCN_ADD_REF_MAX];
	DOUBLE	m_dSinThetaTran[SCN_ADD_REF_MAX];

	LONG	m_lScanPointIndex;
	LONG	m_lScanDieA_X[SCN_POINTA_NUM];
	LONG	m_lScanDieA_Y[SCN_POINTA_NUM];
	LONG	m_lMsPrDieA_X[SCN_POINTA_NUM];
	LONG	m_lMsPrDieA_Y[SCN_POINTA_NUM];
	DOUBLE	m_dESAngle_X[SCN_POINTA_NUM];
	DOUBLE	m_dESAngle_Y[SCN_POINTA_NUM];
	DOUBLE	m_dMSAngle_X[SCN_POINTA_NUM];
	DOUBLE	m_dMSAngle_Y[SCN_POINTA_NUM];
	DOUBLE	m_dAlnTableTheta;
	DOUBLE	m_dScnTableTheta;

	// SCN file alignment and converting.
	ULONG	m_ulScnDieA_Row;
	ULONG	m_ulScnDieA_Col;
	ULONG	m_ulScnDieB_Row;
	ULONG	m_ulScnDieB_Col;
	// Scanner file die position and angle value
	LONG	m_dScnDieA_X;
	LONG	m_dScnDieA_Y;
	DOUBLE	m_dWS896Angle_X;
	DOUBLE	m_dWS896Angle_Y;
	// mapsorter alignment die position and angle value
	LONG	m_lMsDieA_X;
	LONG	m_lMsDieA_Y;
	LONG	m_lMsDieB_X;
	LONG	m_lMsDieB_Y;
	DOUBLE	m_dMS896Angle_X;
	DOUBLE	m_dMS896Angle_Y;

	LONG	m_lGlobalTheta;			// Global Theta
	BOOL	m_bEnableGlobalTheta;	// Flag to control whether global theta correction is enable
	UCHAR	m_ucAlignStepSize;
	DOUBLE	m_dMoveBackAngle;		//v2.82
	LONG	m_lLookForwardCounter;	//v3.55
	LONG	m_lNoLookForwardCounter;//v3.55
	DIE_INFO m_stPosOnPR;

	BOOL	m_bPerformCompensation;	// Flag to control whether compensation will be performed
	BOOL	m_bMoveBack;			// Flag to indicate it is move back case
	BOOL	m_bLimitCheck;			// Check whether the position is out of wafer limit
	BOOL	m_bLFDieNoCompensate;	// No need to compensate curr die if already check pos by LF	//v2.96T3
	BOOL	m_bPreCompensate;		// Only used for IM project

	BOOL	m_bMS60NGPickWaitCheckEmptyDie;		//v4.54A5

	// Smart Walk In Pick & Place
	BOOL	m_bEnableSmartWalkInPicknPlace;
	LONG	m_lPickAndPlaceWalkPath; //Pick without map
	LONG	m_lWaferEdgeLimit;
	LONG	m_lWaferStreetLimit;

	LONG	m_lBurningInGrade;
	LONG	m_lBurnInMaxGrade;
	LONG	m_lBurnInMapDieCount;
	BOOL	m_bBurnInAutoMap;
	BOOL	m_bBurnInAutoGrade;

	CString m_szBurnInMapFile;		//Map file for Burn In (with path)

	//For wafermap action after map is loaded
	UCHAR	m_ucAutoWaferRotation;	//0 = 0; 1 = 90; 2 = 180; 3 = 270
	UCHAR	m_ucAutoBCRotation;		//0 = 0; 1 = 90; 2 = 180; 3 = 270		//v4.46T20	//PLSG
	UCHAR	m_ucMapEdgeSize;		//0 is disable;
	BOOL	m_bMapHoriFlip;			//Hori Flip
	BOOL	m_bMapVertFlip;			//Vert Flip
	BOOL	m_bShowFirstDie;		
	UCHAR	m_ucMapPickMode;				//v2.78T1
	ULONG	m_ulMapDieStep;					//v2.78T1
	CString	m_szMapTour;
	CString m_szMapPath;
	BOOL	m_bMultiWaferSupport;			//v2.91T1
	BOOL	m_bEnableSmartWalk;				//v2.99T1
	BOOL	m_bEnableSWalk1stDieFinder;		//v3.15T1
	UCHAR	m_ucSpiralPickDirection;		//v2.95T1		//0=horiz, 1=vert
	LONG	m_lSpecialRefDieGrade;			//v3.24T1		//For CMLT
	
	CString m_szDummyWaferMapName;

	CString	m_szLastPkgName;
	BOOL	m_bEnableBarcodeLoadMap;
	BOOL	m_bBarCodeSearchMap;			//v3.54T1
	BOOL	m_bBarCodeSearchFolder;
	BOOL	m_bBarCodeSearchPartMap;
	BOOL	m_bLoadMapAccessFail;
	CString	m_szAccessFailFileName;
	CString m_szSearchFolderName;
	BOOL	m_bEnableSuffix;
	CString m_szSuffixName;
	CString	m_szLastLotNumber;				//v4.40T14	//LatticePower
	BOOL	m_bEnableFixMapName;
	BOOL	m_bEnableLocalLoadMap;			//v3.15T5
	BOOL	m_bUserModeLocalLoadMap;
	CString m_szFixMapName;
	BOOL	m_bEnablePrefix;
	BOOL	m_bEnablePrefixCheck;			//v3.60
	BOOL	m_bEnableSuffixCheck;			//v4.36		//SanAn
	BOOL	m_bMatchFileName;
	CString m_szPrefixName;
	BOOL	m_bEnableAutoLoadBackupMap;
	BOOL	m_bMultiGradeSortingtoSingleBin;	//v4.15T8	//Osram Germany layer-sort
	BOOL	m_bEnableCheckLotNo;	//Lattice Power Check Lot No.

	BOOL	m_bReply;

	//For output file generating
	BOOL	m_bEnableOutputBtn;
	LONG	m_lCurOutputFilePageNo;
	LONG	m_lPrevOutputFilePageNo;
	LONG	m_lNoOfOutFileCols;

	CString m_szOutputFormatFileName;
	CString m_szOutFileCol[WT_MAX_OUTPUT_COL];
	BOOL	m_bOutFileColMin[WT_MAX_OUTPUT_COL];
	BOOL	m_bOutFileColMax[WT_MAX_OUTPUT_COL];
	BOOL	m_bOutFileColAvg[WT_MAX_OUTPUT_COL];
	BOOL	m_bOutFileColStd[WT_MAX_OUTPUT_COL];

	CString m_szAllOutFileCol[WT_MAX_OUTPUT_COL_ALL];
	BOOL	m_bAllOutFileColMin[WT_MAX_OUTPUT_COL_ALL];
	BOOL	m_bAllOutFileColMax[WT_MAX_OUTPUT_COL_ALL];
	BOOL	m_bAllOutFileColAvg[WT_MAX_OUTPUT_COL_ALL];
	BOOL	m_bAllOutFileColStd[WT_MAX_OUTPUT_COL_ALL];
	BOOL	m_bIfEnableOutColCtrl[WT_MAX_OUTPUT_COL];
	LONG	m_lOutColCtrlIndex[WT_MAX_OUTPUT_COL];

	CStringArray m_szaAllRefOutputFileCol;
	CStringArray m_szaAllRefOutFileColMin;
	CStringArray m_szaAllRefOutFileColAvg;
	CStringArray m_szaAllRefOutFileColStd;
	CStringArray m_szaAllRefOutFileColMax;

	LONG	m_lNoOfWaferMapCols;
	BOOL	m_bIfEnableWaferMapColCtrl[WT_MAX_MAP_COL];
	LONG	m_lWaferMapColCtrlIndex[WT_MAX_MAP_COL];
	BOOL	m_bEnableHeaderBtn;
	LONG	m_lCurDefineHeaderPageNo;
	LONG	m_lPrevDefineHeaderPageNo;
	CString m_szMapHeaderFileName;
	CString m_szWaferMapCol[WT_MAX_MAP_COL];
	CString m_szAllWaferMapCol[WT_MAX_MAP_COL_ALL];

	//Bar Code Reader
	CString m_szMapFilePath;
	CString m_szMapFileExtension;
	CString m_szBCMapFilename;
	CString m_szMapDisplayFilename;

	CString m_szBackupMapPath;
	CString m_szBackupMapExtension;
	CString m_szUploadBarcodeIDFilePath;

	CString	m_szPrescanBackupMapPath;
	CString	m_szMapServerFullPath;
	BOOL	m_bPrescanBackupMap;
	CString m_szScanMA1FilePath;
	DOUBLE	m_dMA1ScanYieldLimit;

	BOOL	m_bHmiOperationLogOption;
	CString	m_szHmiOperationLogPath;

	BOOL	m_bAutoSrchHome;
	BOOL	m_bResetMapStatus;
	BOOL	m_bCheckCOR;			//Block2
	LONG	m_lTotalRefDieCount;	//Block2
	LONG	m_lCurrRefDieCount;		//Block2

	//For Block Function
	LONG	m_lBlkHomeRow;
	LONG	m_lBlkHomeCol;
	LONG	m_lBlkHomeX;
	LONG	m_lBlkHomeY;
	LONG	m_lMnSrchRefGrid;
	LONG	m_lMnSrchHomeGrid;
	LONG	m_lMnMaxJumpCtr;
	LONG	m_lMnMaxJumpEdge;
	LONG	m_lEmptyGradeMinCount;
	LONG	m_lMnAlignReset;
	LONG	m_lMnMinDieForPick;
	LONG	m_lMnAlignCheckRow;
	LONG	m_lMnAlignCheckCol;
	ULONG	m_ulInspectGoType;
	ULONG	m_ulAlertType;
	BOOL	m_bMnPositionSwitch;
	LONG	m_lNumOfRetry;
	LONG	m_lMapRetryDelay;
	CString	m_szBlkFileName;
	BOOL	m_bAutoUpdateRefDieOffset;	//Block2
	//BLOCKPICK
	BOOL	m_bFirstCycleWT;
	LONG	m_lBlkAutoAlignPosX;	//v3.31		//OptoTech
	LONG	m_lBlkAutoAlignPosY;	//v3.31		//OptoTech
	LONG	m_lBlkAutoAlignPosT;	//v3.31		//OptoTech
	ULONG	m_ulBlkAutoAlignRow;	//v3.31		//OptoTech
	ULONG	m_ulBlkAutoAlignCol;	//v3.31		//OptoTech

	BOOL	m_bIs1stDieFinderStarted;		
	BOOL	m_bIs1stDieAFinderStarted;		

	//v3.00T1
	BOOL	m_bOCRDigitalF;
	BOOL	m_bMnEdgeAlign;
	LONG	m_lMnPassPercent;

	//Wafer Map
	CString m_szWaferMapFormat;

	//Cross check variable
	LONG	m_lXCheckToleranceX;
	LONG	m_lXCheckToleranceY;
	BOOL	m_bEnableXCheck;
	BOOL	m_bPickCrossCheckDie;		//v3.57T1	//Testar

	//Reference cross variable
	LONG	m_lRefCrossRow;
	LONG	m_lRefCrossCol;
	LONG	m_lRefCrossWidth;

	//Region by region pick variable
	LONG	m_lSubSortMode;

	//Ignore Region 
	BOOL	m_bIgnoreRegion;
	LONG	m_lCurrentSelectRegion;							//v3.50T1
	LONG	m_ulSelectRegionULX[WT_MAX_SELECT_REGION];		//v3.49T5
	LONG	m_ulSelectRegionULY[WT_MAX_SELECT_REGION];
	LONG	m_ulSelectRegionLRX[WT_MAX_SELECT_REGION];
	LONG	m_ulSelectRegionLRY[WT_MAX_SELECT_REGION];
	UCHAR	m_ucSelectChangeGrade;
	BOOL	m_bGetAvgFOVOffset;
	LONG	m_lAvgFovOffsetX;
	LONG	m_lAvgFovOffsetY;
	LONG	m_lAdvMostMatchRow;
	LONG	m_lAdvMostMatchCol;

	//Sort die mode; 0 = Normal, 1 = minium die grade 1st
	LONG	m_lSortingMode;

	//Map Sync Move
	BOOL	m_bMapSyncMove;

	//Update Map's Color
	UCHAR	m_ucChangeStartGrade;
	UCHAR	m_ucChangeEndGrade;
	UCHAR	m_ucChangeTargetGrade;

	LONG	m_lGradeCount;	
	
	//4.53D16 mathcing header
	BOOL m_bEnableMatchMapHeader;

	//Wafermap control page no
	BOOL	m_bWaferControlPageNo;

	//Enable Adapative die step;
	BOOL	m_bPitchAlarmOnce;
	BOOL	m_bPitchError;
	BOOL	m_bThisDieNeedVerify;
	ULONG	m_ulAdvSampleJumpDistance;

	DOUBLE	m_dIdleDiffTime;

	DOUBLE	m_dStopTime;
	DOUBLE	m_dCurrTime;
	DOUBLE	m_dScanTime;
	DOUBLE	m_dScanMoveTime;
	CTime	m_stPrescanStartTime;
	// Adv region die offset
	BOOL	m_bAdvSmartSample;
	ULONG	m_ulPdcRegionSizeRow;
	ULONG	m_ulPdcRegionSizeCol;
	ULONG	m_ulPdcRegionSampleCounter;
	
	BOOL	m_bVerifyMapWaferFail;
	LONG	m_lVerifyMapWaferRow;
	LONG	m_lVerifyMapWaferCol;
	LONG	m_laPCIVerifyFailRow[10];
	LONG	m_laPCIVerifyFailCol[10];
	LONG	m_lWftAdvSamplingResult;	// also for region update offset over limit
	
	//Seperation Grade 
	BOOL	m_bCheckSepGrade;

	//SearchHome option
	LONG	m_lReferPitchRow;
	LONG	m_lReferPitchCol;
	BOOL	m_bFindHomeDieFromMap;
	ULONG	m_ulHomePatternRow;
	ULONG	m_ulHomePatternCol;
	LONG	m_ulHomePitchRow;
	LONG	m_ulHomePitchCol;
	ULONG	m_ulHomeKeyDieRow;
	ULONG	m_ulHomeKeyDieCol;
	ULONG	m_ulAlignOption;
	ULONG	m_ulCrossHomeDieRow;
	ULONG	m_ulCrossHomeDieCol;
	LONG	m_lOrgMapRowOffset;
	LONG	m_lOrgMapColOffset;
	LONG	m_lScnMapRowOffset;
	LONG	m_lScnMapColOffset;
	LONG	m_lHomeDieWftPosnX;
	LONG	m_lHomeDieWftPosnY;
	LONG	m_lAlignWaferAngleMode_Std;
	LONG	m_lLastScanGrabPosnX;
	LONG	m_lLastScanGrabPosnY;

	//Die sharp
	UCHAR	m_ucDieShape; 

	//Next die path
	CString m_szNextPath;

	//Repeat load map checking
	BOOL	m_bCheckRepeatMap;
	CString	m_szCurrentMapPath;
	ULONG	m_ulMapIndexLoop;

	//Store wafer end signal
	BOOL	m_bDisableRefDie;			//v2.78T1
	BOOL	m_bCheckGDOnRefDiePos;		//v2.91T1
	BOOL	m_bSelOfUpdateMapPos;

	// For Wafer Lot File
	BOOL	m_bCheckLotWaferName;
	CString m_szLotInfoFilePath;
	CString m_szLotInfoFileName;
	CString m_szLotInfoFileExt;
	CString m_szLotInfoOpID;
	CString	m_szOutWaferInfoFilePath;
	
	CString m_szSampleElectricInfo;

	INT  m_nCornerHomeDiePosX;
	INT  m_nCornerHomeDiePosY;

	//v2.92T1
	//Continue to pick the previous picked grade if new wafer is loaded
	BOOL	m_bKeepLastUnloadGrade;
	BOOL	m_bNewEjectorSequence;		//v2.96T4

	// Array to store Map Row & Map Col that need to Check
	// Map Row & Map Col are store as <Map Row,Map Col> in the array
	BOOL	m_bEnableSortingCheckPts;
	CStringArray m_szaSortingCheckPts;

	CString m_szUserDefineDieType;
	BOOL	m_bEnableMapDieTypeCheck;
	BOOL	m_bIsMapDieTypeCheckOk;

	BOOL	m_bEnableAutoMapDieTypeCheck;	//Control Auto Map Die Type Check (for Ubilux)	//v3.31T1
	BOOL	m_bSelectAutoCheckLotID;
	BOOL	m_bSelectAutoCheckSerialNum;
	BOOL	m_bSelectAutoCheckProductNum;
	BOOL	m_bSelectAutoCheckMode;

	CString m_szAutoCheckLotID;			//save previous wafer info (for Ubilux)		//v3.31T1	
	CString m_szAutoCheckSerialNum;
	CString m_szAutoCheckProductNum;
	CString m_szAutoCheckMode;

	CString m_szSortBinItem;

	BOOL	m_bIsAutoCheckLotIDOk;	//check the selected field (for Ubilux)		//v3.31T1
	BOOL	m_bIsAutoCheckSerialNumOk;
	BOOL	m_bIsAutoCheckProductNumOk;
	BOOL	m_bIsAutoCheckModeOk;

	BOOL	m_bIsCheckSortBinItemOk;

	BOOL	m_bEnablePackageFileCheck;
	BOOL	m_bIsPackageFileCheckOk;

	BOOL	m_bMapGradeMappingOK;

	BOOL	m_bIsIgnoreGradeCountCheckOk;

	BOOL	m_bEnableMapHeaderCheck;
	CString	m_szMapHeaderCheckWaferId;

	BOOL	m_bMESWaferIDCheckOk;
	CString m_szMESWaferIDCheckMsg;

	LONG	m_lStepBurnInT;			//BurnInT
	LONG	m_lCountBurnInT;

	BOOL		m_bEnableMESConnection;
	CString		m_szMESIncomingPath;
	CString		m_szMESOutgoingPath;
	INT			m_lMESTimeout;

	BOOL	m_bEnableTwoDimensionsBarcode;		//v3.33T1	//Cree Rect wafer
	BOOL	m_b2DBarCodeCheckGrade99;			//v3.59
	BOOL	m_bOTraceabilityDisableLotID;		//v3.68T3	//PLLM Lumiramic LED Traceability

	BOOL m_bBlkFuncPage;	//Block2
	BOOL m_bBlkFuncPage2;

	LONG	m_lSlowProfileTrigger;	//DBH
	LONG	m_lFastProfileTrggerY;

	//Motion Log
	BOOL	m_bEnableWTMotionLog;

	// Axis Test Variable
	BOOL	m_bMoveDirection;
	LONG	m_lMotionTestDelay;
	LONG	m_lMotionTestDist;
	BOOL	m_bIsMotionTestWaferTableX;
	BOOL	m_bIsMotionTestWaferTableY;	
	BOOL	m_bIsMotionTestWaferTable2X;
	BOOL	m_bIsMotionTestWaferTable2Y;	

	BOOL	m_bLockJoystick;

	CWaferTableDataBlock m_oWaferTableDataBlk;

	DIE_INFO	m_stTop;
	DIE_INFO	m_stBtm;
	DIE_INFO	m_stLeft;
	DIE_INFO	m_stRight;
	//The CP100 Item
	BOOL	m_bIsCP100Item2Enable;
	BOOL	m_bIsCP100Item4Enable;

	// Daily Check Parameter
	BOOL	m_b2PartsSortAutoAlignWay;
	LONG	m_lMS902ndHomeDieMapRow;
	LONG	m_lMS902ndHomeDieMapCol;
	LONG	m_lMS902ndHomeOffsetRow;
	LONG	m_lMS902ndHomeOffsetCol;
	BOOL	m_bMS90DisplayAndSelect;
	BOOL	m_bMS90DisplayWholeOnly;

	DOUBLE	m_dPrescanResultRemoveTime;
	BOOL	m_bACF_SaveImages;		//	ACF		Auto Clean Folder, check and remove folder files over a period.
	BOOL	m_bACF_MapBackupPath;	//	ACF		Auto Clean Folder, check and remove folder files over a period.
	BOOL	m_bPrescanStarted;

	COLORREF	m_dwGradeColor[256];

	BOOL	m_bMapDummyScanCornerCheck;
	BOOL	m_bPrescanCheckDefect;
	BOOL	m_bPrescanCheckBadCut;
	CString m_szPrescanLogPath;	// folder + name as a whole prefix for logging
	CString	m_szPrescanMapName;
	BOOL	m_bPrescanRealignDone;
	UCHAR	m_ucPrescanRealignMethod;
	BOOL	m_bEnableIM; // inspect machine
	BOOL	m_bEnableMapDMFile;
	BOOL	m_bPrescanNml3AtHomeDie;
	LONG	m_lScanYieldSpinIndex;
	BOOL	m_bOfflineUploadScanFile;

	LONG	m_lAllReferSpiralLoop;
	BOOL	m_bAoiWaferRescan;
	BOOL	m_bRescanLogPrImage;
	BOOL	m_bCaptureScanMapImage;
	LONG	m_lTabletPixelX;
	LONG	m_lTabletPixelY;
	LONG	m_lPngDieExtraEdgeE;
	LONG	m_lPngDieExtraEdgeY;	// extra value (%) to increase the die size, show in PNG bigger.
	LONG	m_lPngDieExtraEdgeN;	
	LONG	m_lPngDieExtraEdgeX;	
	DOUBLE	m_dTabletDimInch;
	DOUBLE	m_dTabletDimInch_X;
	DOUBLE	m_dTabletDimInch_Y;
	LONG	m_lPngPixelX;
	LONG	m_lPngPixelY;
	DOUBLE	m_dTabletXPPI;
	DOUBLE	m_dTabletYPPI;

	BOOL	m_bPngEnableGrid;
	LONG	m_lPngGridRow;
	LONG	m_lPngGridCol;
	LONG	m_lPngGridThick;

	BOOL		m_bGridColourOn1;
	BOOL		m_bGridColourOn2;
	BOOL		m_bGridColourOn3;
	BOOL		m_bGridColourOn4;
	BOOL		m_bGridColourOn5;
	COLORREF	m_ulGridColourArray1;
	COLORREF	m_ulGridColourArray2;
	COLORREF	m_ulGridColourArray3;
	COLORREF	m_ulGridColourArray4;
	COLORREF	m_ulGridColourArray5;

	COLORREF	m_ulBoundaryDieColour;
	COLORREF	m_ulNGDieColourY;
	COLORREF	m_ulNGDieColourN;
	COLORREF	m_ulNGDieColourX;
	COLORREF	m_ulNGDieColourE;

	BOOL	m_bScanLowYield;
	BOOL	m_bFindAllRefer4Scan;
	DOUBLE	m_dPrescanRotateDegree;
	LONG	m_lBlk4ReferCheckScore;
	BOOL	m_bWaferEndUncheckRefer;
	BOOL	m_bManualRegionScanSort;
	BOOL	m_bSingleHomeRegionScan;
	BOOL	m_bScanPreunloadTable;
	UCHAR	m_ucPSMLoadAction;
	BOOL	m_bGenRptOfSMS;
	BOOL	m_bCheckPSMExist;
	BOOL	m_bCheckPSMLoaded;
	ULONG	m_ulPrescanRefPoints;
	bool	m_bPrescanMapDisplay;
	CString m_szOemCustomerName;
	CString m_szOemProductName;
	CString	m_szOemTestSystem;
	CString	m_szPsmMapMachineNo;
	CString	m_szDieState;
	LONG	m_lMa3AHomeRow;	// Align
	LONG	m_lMa3AHomeCol;	// Align
	LONG	m_lMa3RHomeRow;	// Refer Home, map home
	LONG	m_lMa3RHomeCol;	// Refer Home, map home
	LONG	m_lMa3MapDirX;
	LONG	m_lMa3MapDirY;
	BOOL	m_bSetDummyMa3HomeDie;
	BOOL	m_bEnableClickMapAndTableGo;
	CString	m_szScanYieldWaferName;
	LONG	m_lMC[20];

	//	Wafer Corner Pattern Check
	UCHAR	m_ucCornerPatternCheckMode;
	UCHAR	m_ucCornerReferPrID;
	BOOL	m_bCornerPatternCheckAll;
	LONG	m_lReferToCornerDistX;
	LONG	m_lReferToCornerDistY;

	LONG	m_lRescanLastDiePosnX;
	LONG	m_lRescanLastDiePosnY;
	LONG	m_lRescanStartWftX;
	LONG	m_lRescanStartWftY;
	LONG	m_lRescanStartRow;
	LONG	m_lRescanStartCol;
	BOOL 	m_bScnModeWaferAlignTwice;
	BOOL	m_bAutoRescanAllDice;
	LONG	m_lOrgDieA_X;
	LONG	m_lOrgDieA_Y;
	LONG	m_lOrgDieB_X;
	LONG	m_lOrgDieB_Y;
	DOUBLE	m_dOrgAngleCos_X;
	DOUBLE	m_dOrgAngleSin_Y;

	LONG	m_lRlnDieA_X;
	LONG	m_lRlnDieA_Y;
	LONG	m_lRlnDieB_X;
	LONG	m_lRlnDieB_Y;
	DOUBLE	m_dRlnAngleCos_X;
	DOUBLE	m_dRlnAngleSin_Y;
	LONG	m_lAutoWaferCenterX;
	LONG	m_lAutoWaferCenterY;
	LONG	m_lAutoWaferDiameter;
	LONG	m_lAutoWaferWidth;
	LONG	m_lAutoWaferHeight;
	BOOL	m_bAutoWaferLimitOnce;
	BOOL	m_bAutoSearchWaferLimit;
	BOOL	m_bAutoWaferLimitOk;
	BOOL	m_bDummyQuarterWafer;
	BOOL	m_bScanMapUseBackupExt;
	BOOL	m_bScanMapAddTimeStamp;
	UCHAR	m_ucQuarterWaferCenter;
	DOUBLE	m_dPrescanAreaDiameter;
	DOUBLE	m_dBarBridgeLength;
	BOOL	m_bPrescanningMode;
	BOOL	m_bIntoNgPickCycle;
	BOOL	m_bScanAreaConfig;
	LONG	m_lScanAreaUpLeftX;
	BOOL	m_bFixDummyMapCenter;
	LONG	m_lScanAreaUpLeftY;
	LONG	m_lScanAreaLowRightX;
	LONG	m_lScanAreaLowRightY;
	LONG	m_lAreaPickDiePosnX;
	LONG	m_lAreaPickDiePosnY;
	LONG	m_lAreaPickDieMoveT;
	LONG	m_lAreaPickScanUL_X;
	LONG	m_lAreaPickScanLR_X;
	LONG	m_lAreaPickScanDirection;
	LONG	m_lReadFromWFT;

	LONG	m_lFindMapEdgeDieLimit;
	BOOL	m_bSearchWaferLimitDone;	// false when load map or s/w start

	LONG	m_lCPClbPrDelay;
	BOOL	m_bEnableAtcLog;
	BOOL	m_bRegionPrescan;
	
	// die encoder base predication
	BOOL		m_bDebKeyDieSampleOld;
	//DIEINFO		m_paDebKeyDie[MS_MaxKeyNum];
	CString		m_szDEB_LibVersion;
	UINT		m_unNumOfKeyDie;
	BOOL	m_bAlignNeedRescan;
	BOOL	m_bPrescanAutoDiePitch;
	LONG	m_lPrescanDiePitchX_X;
	LONG	m_lPrescanDiePitchX_Y;
	LONG	m_lPrescanDiePitchY_Y;
	LONG	m_lPrescanDiePitchY_X;
	ULONG	m_ulPrescanGoToType;
	
	ULONG	m_ulPrescanMethod;
	ULONG   m_ulPrescanXYStripMode;
	UCHAR	m_ucRunScanWalkTour;
	INT		m_nPrescanDirection;
	INT		m_nPrescanAlignScanStage;
	BOOL	m_bRunTimeDetectEdge;
	LONG	m_lADEEmptyFrameLimit;
	LONG	m_lDetectEdgeState;
	LONG	m_lScannedWaferUL_X;
	LONG	m_lScannedWaferUL_Y;
	LONG	m_lScannedWaferLR_X;
	LONG	m_lScannedWaferLR_Y;
	LONG	m_lScannedMapMinRow;
	LONG	m_lScannedMapMinCol;
	LONG	m_lScannedMapMaxRow;
	LONG	m_lScannedMapMaxCol;
	BOOL	m_baScanMatrix[SCAN_MAX_MATRIX_ROW+1][SCAN_MAX_MATRIX_COL+1];
	LONG	m_laScanPosnX[SCAN_MAX_MATRIX_ROW+1][SCAN_MAX_MATRIX_COL+1];
	LONG	m_laScanPosnY[SCAN_MAX_MATRIX_ROW+1][SCAN_MAX_MATRIX_COL+1];
	LONG	m_nPrescanNextWftPosnX;
	LONG	m_nPrescanNextWftPosnY;
	LONG	m_lFastHome1stFrameRow;
	LONG	m_lFastHome1stFrameCol;
	BOOL	m_lFastHome1stFrameDone;
	LONG	m_lDummyMapMaxRow;
	LONG	m_lDummyMapMaxCol;
	LONG	m_lFastHomeIndexOffsetRow;
	LONG	m_lFastHomeIndexOffsetCol;
	INT		m_nPrescanAlignMapCol;
	INT		m_nPrescanAlignMapRow;
	INT		m_nPrescanAlignPosnX;
	INT		m_nPrescanAlignPosnY;
	INT		m_nPrescanAlign2ndDistY;

	ULONG	m_ulAlignHomeDieRow;
	ULONG	m_ulAlignHomeDieCol;
	INT		m_nAlignHomeDieWftX;
	INT		m_nAlignHomeDieWftY;

	BOOL	m_bUseWaferMapCategory;

	BOOL	m_bRegionBlkPickOrder;
	BOOL	m_bFirstRegionInit;

	LONG	m_lRegionPickIndex;
	LONG	m_lSubRegionState[WT_MAX_SUBREGIONS_LIMIT];
	ULONG	m_ulReverifyReferRegion;
	INT		m_nMapLastPrescanPosnX;
	INT		m_nMapLastPrescanPosnY;
	LONG	m_lMapLastPrescanRow;
	LONG	m_lMapLastPrescanCol;

	// prescan relative variables
	LONG	m_lPrescanPrDelay;

	INT		m_nPrescanMapCtrX;
	INT		m_nPrescanMapCtrY;
	INT		m_nPrescanMapRadius;
	INT		m_nPrescanMapWidth;
	INT		m_nPrescanMapHeight;

	UCHAR	m_ucPrescanMapShapeType;
	BOOL	m_bHmiDefectPick;
	BOOL	m_bSortingDefectDie;
	BOOL	m_bScanPickedDieResort;
	UCHAR	m_ucDefectiveDieGrade;
	ULONG	m_ulPrescanGoCheckLimit;
	ULONG	m_ulPrescanGoCheckCount;
	LONG	m_ulGrade151Total;
	BOOL	m_bChangeRegionGrade;
	BOOL	m_bAutoGoAroundPE;	// Pitch Error
	BOOL	m_bCheckDiePrIn4X;
	ULONG	m_ulPitchAlarmGoCheckLimit;
	ULONG	m_ulPitchAlarmGoCheckCount;
	CString m_szNextLocation;
	CDWordArray	m_aUIGoCheckRow;
	CDWordArray	m_aUIGoCheckCol;

	BOOL	m_bContourEdge;
	ULONG	m_ulContourEdgePoints;
	LONG	m_lCntrEdgeY_C[SCAN_MAX_MATRIX_ROW+1];
	LONG	m_lCntrEdgeX_L[SCAN_MAX_MATRIX_ROW+1];
	LONG	m_lCntrEdgeX_R[SCAN_MAX_MATRIX_ROW+1];

	//	Sample Probing (SP) for CP100
	ULONG	m_ulTotalProbeDie;

	BOOL	m_bDisableWaferMapGradeSelect;
	BOOL	m_bPrescanDebugEnable;

	BOOL	m_bPrescanPosnOK;
	BOOL	m_bMapPositionOK;
	BOOL	m_bThisDieLFed;
	LONG	m_lGetAtcWftPosX;
	LONG	m_lGetAtcWftPosY;
	ULONG	m_ulGetMveMapRow;
	ULONG	m_ulGetMveMapCol;
	LONG	m_lGetScnWftPosX;
	LONG	m_lGetScnWftPosY;
	LONG	m_lGetMveWftPosX;
	LONG	m_lGetMveWftPosY;
	LONG	m_lGetRgnWftPosX;
	LONG	m_lGetRgnWftPosY;

	BOOL	m_bFirstMapIsOldMap;
	CTime	m_stLastPickTime;
	CTime	m_stRescanStartTime;
	CTime	m_stLastAdvSampleTime;
	LONG	m_lLFPitchToleranceX;
	LONG	m_lLFPitchToleranceY;
	LONG	m_lLFPitchErrorCount;
	LONG	m_lLFPitchErrorLimit;
	LONG	m_lLFPitchErrorCount2;

	ULONG	m_ulPitchAlarmCount;
	ULONG	m_ulPitchAlarmLimit;

//	mylar shift prediction
	LONG	m_ulNextRescanCount;
	LONG	m_lRescanPELimit;
	LONG	m_lRescanPECounter;	//	last sample die picked counter
	LONG	m_ulLastRscnPickCount;
	BOOL	m_bReSampleAsError;
	BOOL	m_bToDoSampleOncePE;

	ULONG	m_ulAdvOffsetUpdMinTime;	// if current time to last update time < sampling min. time, just skip this region. no need to smaple and update offset
	ULONG	m_ulAdvOffsetExpireTime;	// if current time to last update time >= predicate min time, then this region not used for predication
	ULONG	m_ulAdvOffsetUpdMinVary;	// if sample offset compare to last update < this tolerance, then the nearby region no need to sample and directly updated by this offset
	ULONG	m_ulAdvStage1StartCount;
	ULONG	m_ulAdvStage1SampleSpan;
	ULONG	m_ulAdvStage2StartCount;
	ULONG	m_ulAdvStage2SampleSpan;
	ULONG	m_ulAdvStage3StartCount;
	ULONG	m_ulAdvStage3SampleSpan;
	ULONG	m_ulRescanStartCount;
	ULONG	m_ulRescanSampleSpan;
	BOOL	m_bWftDoSamplingByBT;
//	mylar shift prediction

	BOOL	m_bThisDieNotLFed;
	ULONG	m_ulPickingBlockIndex;	// for OSRAM only and not burn in and not layer
	
	CString m_szaMapDieTypeCheckCurValue[MS896A_DIE_TYPE_CHECK_LIMIT];
	BOOL	m_baIsSelectMapDieTypeCheck[MS896A_DIE_TYPE_CHECK_LIMIT];
	BOOL	m_baIsMapDieTypeCheckOk[MS896A_DIE_TYPE_CHECK_LIMIT];

	CString		m_szTCLibraryVer;
	CString		m_szTCFirmwareVer;
	BOOL		m_bHMIEjectorHeaterOn;
	BOOL		m_bIsConnected;
	BOOL		m_bTCTuneStart;
	LONG		m_lCurrentTmp;
	LONG		m_lHeaterInTmp;
	BOOL		m_bAlarmBitOn;
	CTime		m_stReadTmpTime;
	CTime		m_stStopTime;

	UCHAR m_ucWaferLimitTemplateNoSel;
	WAFER_TABLE_LIMIT_TEMPLATE m_oWaferTableLimitTmp[WT_WAFER_LIMIT_TEMPLATE_NO];

	//v4.59A17	//SanAn & Semitek by Roy Ip
	INT			m_n1stLFGrade;		
	CString 	m_sz1stLFGrade;		
	INT			m_n1stLFCurrGrade;		

};

