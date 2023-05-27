/////////////////////////////////////////////////////////////////
// BondHead.cpp : interface of the CBondHead class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once
#include "MS896A.h"
#include "MS896AStn.h"
#include "BH_Constant.h"
#include "resource.h"
#include "LogFileUtil.h"
#include "Plane.h"
#include "PB_MultiProbeLevel.h"

#define COLLET_VAC_OFF_TIME_BEFOR_Z_TO_BOND_LEVEL  5 //ms
#define COLLET_VAC_ON_TIME_AFTER_Z_TO_SWING_LEVEL  5  //ms
#define	COLLET_VAC_OFF_PROCESS_BLK	0
#define WEAK_BLOW_ON_PROCESS_BLK	1
#define	COLLET_VAC_ON_PROCESS_BLK	2

// Offset for NVRAM (in ULONG)
#define	BH_COLLETCOUNT_OFFSET		0		/* Collet Count */
#define	BH_EJECTCOUNT_OFFSET		1		/* Ejector Count */
#define	BH_PICKDIE_OFFSET			2		/* Number of Picked Die */
#define	BH_BONDDIE_OFFSET			3		/* Number of Bonded Die */
#define	BH_DEFECTDIE_OFFSET			4		/* Number of Defective Die */
#define	BH_BADCUTDIE_OFFSET			5		/* Number of Bad-cut Die */
#define	BH_MISSINGDIE_OFFSET		6		/* Number of Missing Die */
#define	BH_ROTATEDIE_OFFSET			7		/* Number of Rotate Die */
#define	BH_EMPTYDIE_OFFSET			8		/* Number of Empty Die */

#define	BH_CURWAF_BONDEDDIE_OFFSET	9		/* Number of Bonded Die		(Current wafer)*/
#define	BH_CURWAF_DEFECTDIE_OFFSET	10		/* Number of Defective Die	(Current wafer)*/
#define	BH_CURWAF_BADCUTDIE_OFFSET	11		/* Number of Bad-cut Die	(Current wafer)*/
#define	BH_CURWAF_ROTATEDIE_OFFSET	12		/* Number of Rotate Die		(Current wafer)*/
#define	BH_CURWAF_EMPTYDIE_OFFSET	13		/* Number of Empty Die		(Current wafer)*/
#define	BH_CLEANCOUNT_OFFSET		14		/* Clean Collet Count */
#define BH_CURWAF_MISSINGDIE_OFFSET	15		/* Number of Missing Die	(Current wafer)*/	
#define BH_CURWAF_CHIPDIE_OFFSET	16		/* Number of Chip Die		(Current wafer)*/	
#define BH_CURWAF_INKDIE_OFFSET		17		/* Number of INK Die		(Current wafer)*/	

#define BH_CUSTOM_COUNT_1			18		/* Custom Counter #1 */
#define BH_CUSTOM_COUNT_2			19		/* Custom Counter #2 */
#define BH_CUSTOM_COUNT_3			20		/* Custom Counter #3 */

#define	BH_COLLET2COUNT_OFFSET		21		/* Collet Count */		//v3.92	//MS100

#define	BH_PROBEPINCOUNT_OFFSET				22
#define	BH_PROBEPINCLEANCOUNT_OFFSET		23

#define BH_MAX_THRESHOLD_DAIRFLOW			20

#define	MAX_WAFFLE_PADS				50	//	25 for 0 degree and 25 for 180 degree.
typedef struct
{
	BOOL	m_bDefined;
	LONG	m_lPadULX;
	LONG	m_lPadULY;
	LONG	m_lPadLRX;
	LONG	m_lPadLRY;
	BOOL	m_bZ1Learnt;
	LONG	m_lZ1PadBT_X;
	LONG	m_lZ1PadBT_Y;
	LONG	m_lZ1BondLevel;
	BOOL	m_bZ2Learnt;
	LONG	m_lZ2PadBT_X;
	LONG	m_lZ2PadBT_Y;
	LONG	m_lZ2BondLevel;
}	WAFFLE_PAD_LEVELS;

typedef struct
{
#ifndef ES101
#ifdef NU_MOTION
	CTRL_ILC_CONFIG_STRUCT ILCConfigInfo;
#endif
#endif
	BOOL bIsInit;
	BOOL bIsAutoLearnComplete;
	BOOL bIsAutoLearnConverged;
	BOOL bIsRestartUpdateConverged;
	LONG ApplyingCycle;
	LONG ApplyingUpdateCount;
	LONG ChannelNo;

} ILC_STRUCT;

typedef struct
{
	FLOAT	fMovePos[BA_CONTOUR_MOVE_BUF_SIZE];
	FLOAT	fMoveVel[BA_CONTOUR_MOVE_BUF_SIZE];
	FLOAT	fMoveAcc[BA_CONTOUR_MOVE_BUF_SIZE];
	FLOAT	fMoveJerk[BA_CONTOUR_MOVE_BUF_SIZE];

} ILC_MOVE_STRUCT;

typedef struct
{
	INT nPos[0x8000];
	INT nEnc[0x8000];
	INT nChannelMode[0x8000];
	INT nNoOfSampleUpload;
	LONG lBondArmMode;
	BOOL bEnable;
	BOOL bHasData;
	BOOL bStartILCUpdate;
	LONG lDataLogCycleCount;
} ILC_DATALOG_STRUCT;

class AD_CBHAirFlowCompensation;

class CBondHead : public CMS896AStn
{
	DECLARE_DYNCREATE(CBondHead)

public:
	// Init (BondHead.cpp)
	CBondHead();
	virtual	~CBondHead();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();
	virtual BOOL InitData();			// Init the data from SMF and NVRAM
	virtual VOID ClearData();			// Clear data for cold-start
	virtual VOID FlushMessage();		//Flush IPC Message	
	virtual VOID UpdateStationData();

	BOOL InitMinMachineDelays();		//v3.62
	BOOL InitEjTableXY();				//v4.41T5

	//Current Machine information
	BOOL	SaveMachineStatistic(VOID);

	//Current wafer information
	BOOL	LogWaferInformation(BOOL bOnLoadMap);
	BOOL	ResetCurrWaferCounter(VOID);
	BOOL	UpdateDieCounter(VOID);

	INT EjX_Home();
	INT EjY_Home();
	INT  MS50ChgColletZ_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	LONG MoveChgColletTPitch(const LONG lIndex, INT nMode = SFM_WAIT);
	BOOL IsMS50ChgColletZAtHome();

	BOOL IsEnaAutoChangeCollet();
	BOOL IsCoverOpen();

	VOID MissingDieThreshold_Log(const BOOL bBHZ2, CString szText);
	VOID LogMissingDieThresholdValue(const BOOL bBHZ1);
	VOID LogAutoCleanColletThresholdValue(const BOOL bBHZ1);
	VOID LogColletJamThresholdValue(const BOOL bBHZ1);
	VOID RuntimeUpdateColletJamThresholdValue();
	VOID RuntimeUpdateMissingDieThresholdValue(BOOL bBHZ1ToPick);
	VOID SetBHPickVacuum(const BOOL bBHZ2, const BOOL bSet);
	VOID SetBHStrongBlow(const BOOL bBHZ2, const BOOL bSet);
	LONG GetColletHoleEnc_T();
	LONG GetColletHoleEnc_Z();
	LONG GetColletHoleEnc_Z2();
	VOID	BackupBurnInStatus();
	VOID	RestoreBurnInStatus();
	LONG GetBondCount()
	{
		return m_lBondDieCount;
	}

	BOOL LoadPackageMsdBHData(VOID);
	BOOL SavePackageMsdBHData(VOID);
	BOOL UpdateBHPackageList(VOID);

	//for wafer and bin ejector page
	BOOL DisableEjectorDisplay(UINT unStandbyLevel, LONG lMotorType, BOOL bReset = FALSE);

	DOUBLE	GetEjrLifeTimePercentage();
	LONG IsColletJamForBothBH();
	BOOL CreateResetColletEjectorCountOutputFileSelectionFolder();
	BOOL IsEnableBHMark();
	BOOL IsEnableColletHole();
	VOID EjectorMoveToStandBy();
	BOOL CleanDirtEjectorPin(const LONG lCleanDirtPinUpLevel, const LONG lCycleCount);

	LONG GetMissingDieRetryCount();
	LONG GetMDCycleLimit();

	BOOL OpAutoLearnBHZPickLevel(BOOL bBHZ2);
	BOOL m_bAutoLearnPickLevelZ1;
	BOOL m_bAutoLearnPickLevelZ2;

	LONG	EjElevatorZGoToDownPosition(IPC_CServiceMessage &svMsg);
	LONG	EjElevatorZGoToUpPosition(IPC_CServiceMessage &svMsg);
	LONG	BinEjElevatorZGoToDownPosition(IPC_CServiceMessage &svMsg);
	LONG	BinEjElevatorZGoToUpPosition(IPC_CServiceMessage &svMsg);
	LONG	NVCSingleBondDie(IPC_CServiceMessage &svMsg);
	LONG	EjectorThetaShakeVisionTest(IPC_CServiceMessage &svMsg);

protected:

	// Set Event (BH_Event.cpp)
	VOID SetAutoChangeColletDone(BOOL bState);
	VOID SetEOT_Done(BOOL bState);
	VOID SetProbeZReady(BOOL bState);
	VOID SetEjectorReady(BOOL bState = TRUE, CString szLog = "");	//v4.48A29
	VOID SetPRLatched(BOOL bState = TRUE);
	VOID SetPRStart(BOOL bState = TRUE);
	VOID SetBadDieForT(BOOL bState = TRUE);
	VOID SetBTReady(BOOL bState = TRUE);
	VOID SetBhTReady(BOOL bState = TRUE, CString szLog = "");
	VOID SetBhTReadyForBPR(BOOL bState = TRUE);		//v2.60
	VOID SetBhTReadyForWPR(BOOL bState = TRUE);		//v3.34
	VOID SetBhTReadyForWT(BOOL bState = TRUE, CString szLog = "");		//v4.46T22	//v4.59A26
	VOID SetWTReady(BOOL bState = TRUE, CString szTemp = "SetWTReady");
	VOID SetWTStartMove(BOOL bState = TRUE);		//v3.66	//DBH
	VOID SetCompDone(BOOL bState = TRUE);			//v4.34T10
	BOOL WaitDieBonded(INT nTimeout);
	VOID SetDiePicked(BOOL bState = TRUE);
	VOID SetDieBonded(BOOL bState = TRUE);
	VOID SetDieBondedForWT(BOOL bState = TRUE);
	VOID SetMoveBack(BOOL bState = TRUE);
	VOID SetWTBacked(BOOL bState = TRUE);
	VOID SetBHInit(BOOL bState = TRUE);
	VOID SetConfirmSrch(BOOL bState = TRUE);
	VOID SetBhToPrePick(BOOL bState = TRUE);
	VOID SetBTStartMoveWithoutDelay(BOOL bState);
	VOID SetBTStartMove(BOOL bState = TRUE);
	VOID SetBackupNVRam(BOOL bState = TRUE);
	VOID SetBLFrameLevelDn(BOOL bState = TRUE);			//v4.01
	VOID SetBTReSrchDie(BOOL bState = TRUE);				//v4.xx
	VOID SetBPRPostBondDone(BOOL bState = TRUE);		//v4.40T6
	VOID SetBTCompensate(BOOL bState = TRUE);			//v4.52A16
	VOID SetSearchMarkReady(BOOL bState);
	VOID SetSearchMarkDone(BOOL bState);
	VOID SetBhReadyForWPRHwTrigger(BOOL bState = TRUE);
	VOID SetBhReadyForBPRHwTrigger(BOOL bState = TRUE);
	
	// Wait Event (BH_Event.cpp)
	BOOL WaitPRLatched(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitPRStart(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBadDieForT(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBTReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitWTReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitCompDone(INT nTimeout = LOCK_TIMEOUT);			//v4.34T10
	BOOL WaitWTStartMove(INT nTimeout = LOCK_TIMEOUT);		//v3.66	//DBH
	BOOL WaitDiePicked(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitMoveBack(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitWTBacked(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitConfirmSrch(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBTStartMoveWithoutDelay(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBTStartMove(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitExpInit(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBLFrameLevelDn(INT nTimeout = LOCK_TIMEOUT);	//v4.01
	BOOL WaitBTReSrchDie(INT nTimeout = LOCK_TIMEOUT);		//v4.xx
	BOOL WaitBPRPostBondDone(INT nTimeout = LOCK_TIMEOUT);	//v4.40T6
	BOOL WaitBTCompensate(INT nTimeout = LOCK_TIMEOUT);		//v4.52A16
	BOOL WaitSearchMarkReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitSearchMarkDone(INT nTimeout);

	// Supporting Functions (BH_SubState.cpp)
	BOOL ReadBH1AirFlowValue();
	BOOL ReadBH2AirFlowValue();
	BOOL IsMS60EjtHome();
	BOOL IsMissingDie();
	BOOL IsMissingDieZ2();
	BOOL IsColletJam();
	BOOL IsColletJamZ2();
	BOOL IsBHContact();	
	BOOL IsBHContactZ2();	
	BOOL IsLowVacuumFlow();
	BOOL IsLowPressure();
	BOOL IsLowPressure1();						//v4.59A12
	BOOL IsLowPressure2();						//v4.59A12
	BOOL IsLowPressureEj();						//v4.59A12
	BOOL IsInputIonizerErrOn();					//v4.59A12
	BOOL IsOutputIonizerErrOn();				//v4.59A12
	BOOL IsCatchAndBlowYOn();					//v4.59A12
	BOOL IsScopeDown();							//v4.59A12	//Originally from WL task

	BOOL IsCleanColletSafetySensor();
	BOOL IsMainCoverOpen(const CString& szBitId, const CString& szBitId2);
	BOOL IsFrontCoverOpen();
	BOOL IsFrontLeftCoverOpen();
	BOOL IsFrontMiddleCoverOpen();
	BOOL IsRearLeftCoverOpen();
	BOOL IsRearRightCoverOpen();
	BOOL IsLeftCoverOpen();

	BOOL IsRearElevatorCoverOpen();
	BOOL IsFrontRightCoverOpen();
	BOOL IsFrontCoverLocked();
	BOOL IsSideCoverLocked();
	BOOL IsBinElevatorCoverLocked();
	BOOL IsEMOTriggered();						//v3.91
	BOOL IsMS60TempertureUnderHeat(BOOL bCheckNow=FALSE);			//v4.53A19
	BOOL IsBTChngGrd();
	BOOL IsBinFull();
	//BOOL IsBHFanTurnOn();						//v4.08
	BOOL IsDBHThermalAlarm(LONG &lAlarmCode);	//v4.26		//MS100Plus v2.2 
	VOID SetPickVacuum(BOOL bSet);
	VOID SetPickVacuumZ2(BOOL bSet);
	VOID SetStrongBlow(BOOL bSet);
	VOID SetStrongBlowZ2(BOOL bSet);
	VOID SetFrontCoverLock(BOOL bSet);
	VOID SetSideCoverLock(BOOL bSet);
	VOID SetBinElevatorCoverLock(BOOL bSet);
	VOID SetCleanColletPocket(BOOL bOn);		//v3.65		//Lexter
	VOID SetEjectorVacuum(BOOL bSet);			//v3.79
	VOID SetTestBit(BOOL bOn);					//v4.36	//For internal test purpose only
	//v4.59A12	//Renesas MS90
	VOID SetBlowAndCatchSolY(BOOL bSet);
	VOID SetBlowAndCatchSolZ(BOOL bSet);
	VOID SetBlowAndCatchSolBlow(BOOL bSet);

	VOID SetEjectorCapVacuum(BOOL bSet);

	INT T_SafeMoveToInAuto(INT nPos, CString szWhere = "");
	INT T_SafeMoveToLoop(INT nPos, CString szWhere = "");
	BOOL LoopCheckCoverOpen(CString szEvent);
	BOOL IsBondArmAbleToMove();					//v4.55A5
	BOOL CheckCoverOpenInManual(CString szEvent);
	BOOL CheckCoverOpenInAuto(CString szEvent);

	//ES101 NGPick module IO fcns				//v4.24T8
	BOOL IsNGPickAtDnPos();
	BOOL IsNGPickAtUpPos();
	BOOL IsTensionCheck();
	BOOL IsTapeEmpty();
	VOID SetNGPickReelStop(BOOL bOn);
	VOID SetNGPickUpDn(BOOL bDn);

	BOOL IsWT1UnderEjectorPos();
	BOOL IsWT2UnderEjectorPos();
	VOID GetES101WTEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis, BOOL bWT2);

	BOOL BH_MBL_LogLevel();
	BOOL BH_GetLearnLevel_BT(BOOL bBHZ2, LONG &lBondZ);
	BOOL BH_GetCurntLevel_BT(LONG lBTX, LONG lBTY, LONG &lBondZ);
	BOOL BH_BT_CalculateLevel(BOOL bBTRotated, LONG lX, LONG lY, LONG &lPL_Z);
	LONG GetBTLevelOffset(BOOL bBHZ2, LONG lBT_CurrX, LONG lBT_CurrY);
	LONG GetZ1BondLevel(BOOL bAutoCycle, BOOL bUseBT);
	LONG GetZ2BondLevel(BOOL bAutoCycle, BOOL bUseBT);
	BOOL SetBondPadLevel(CONST BOOL bBHZ2);
	LONG GetBondPadLevel(CONST BOOL bBHZ2, LONG lBT_X, LONG lBT_Y, BOOL &bReturn);
	// Update (BondHead.cpp)
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	VOID UpdateSafetySensor();
	virtual VOID RegisterVariables();

	// State Operation (BH_State.cpp)
	VOID Operation();
	VOID RunOperation();
	VOID WaitForDelayToEjectorUpMove(const LONG lTime_Ej, const LONG lEjectorUpDelay, const DOUBLE s_dbStartZ2PK);
	LONG GetActualPickDelay(const DOUBLE dTime_Ej, const LONG lEjectorUpDelay, const LONG lPickDelay, const DOUBLE s_dbStartZ2PK, const DOUBLE dCurTime);
	VOID WaitForPickDelay(const DOUBLE lTime_Ej, const LONG lEjectorUpDelay, const LONG lPickDelay, const DOUBLE s_dbStartZ2PK);
	VOID WaitForDelay(const LONG lDelayTime, const DOUBLE dStartTime);
	LONG GetActualBondDelay(const DOUBLE dBondZDownStart, const DOUBLE dCurTime);
	VOID WaitForBondDelay(const DOUBLE dBondZDownStart);
	VOID WaitTurnOffStrongBlow(const LONG lActualNeutralDelay, const DOUBLE dBondZDownStart);
	BOOL CheckColletJamCount(BOOL &bIsColletJam, BOOL &bReachedColletJamCount);
//	VOID RuntimeUpdateColletJamThresholdValue();
//	VOID RuntimeUpdateMissingDieThresholdValue();
	LONG GetCheckBHMarkCycle();
	VOID RunOperation_dbh();					//v3.66
	VOID RunOperation_ES();
	VOID RunOperation_AP();
	VOID RunOperation_MegaDa();

	virtual	VOID IdleOperation();
	virtual	VOID DiagOperation();
	virtual	VOID InitOperation();
	virtual	VOID PreStartOperation();
	virtual	VOID AutoOperation();
	virtual	VOID DemoOperation();
	virtual VOID CycleOperation();
	virtual	VOID StopOperation();

	// Profile Functions (BH_Profile.cpp)
	INT Z_Profile(INT nProfile);
	INT T_Profile(INT nProfile);
	INT Ej_Profile(INT nProfile);
	LONG CalculateEjQueryTime(const LONG lMaxTravelDist, const LONG lQueryDist);
	LONG CalculateEjTime(LONG lDistance = 0);

	INT Z_SelectObwProfile(LONG lRelDistance, CONST BOOL bAtBond = FALSE);
	INT Z2_SelectObwProfile(LONG lRelDistance, CONST BOOL bAtBond = FALSE);
	INT T_SelectObwProfile(LONG lRelDistance, CString szUserProfile = "");
	INT T_UpdatePrePickProfile(LONG lRelDistance);

	INT Ej_SelectObwProfile(LONG lRelDistance);		//Linear Ej
	INT BinEj_SelectObwProfile(LONG lRelDistance);		//Linear Ej

	INT EjT_SelectObwProfile(LONG lRelDistance);
	INT BinEjT_SelectObwProfile(LONG lRelDistance);

	INT EjCap_SelectObwProfile(LONG lRelDistance);
	INT BinEjCap_SelectObwProfile(LONG lRelDistance);

	INT Ej_SelectControl(LONG lRelDistance, BOOL bUseShortTravel = FALSE);		//andrewng //2020-0605
	INT BinEj_SelectControl(LONG lRelDistance, BOOL bUseShortTravel = FALSE);	//andrewng //2020-0605
	INT EjT_SelectControl(LONG lRelDistance);
	INT BinEjT_SelectControl(LONG lRelDistance);

	// Move Functions (BH_Move.cpp)
	BOOL ZHomeSearch(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, const CString& szProfileID);
	INT Z_Home();
	INT Z2_Home();
	INT Z_MoveTo(INT nPos, INT nMode = SFM_WAIT, BOOL bForceToMove = FALSE, BOOL bToBond = FALSE, short sProcListSaveID = -1);
	INT Z2_MoveTo(INT nPos, INT nMode = SFM_WAIT, BOOL bForceToMove = FALSE, BOOL bToBond = FALSE, short sProcListSaveID = -1);
	INT Z_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Z2_Move(INT nPos, INT nMode = SFM_WAIT);
	INT  Z_MoveToSearch(INT nPos, INT nMode = SFM_WAIT, BOOL bIsBond = FALSE);
	INT Z2_MoveToSearch(INT nPos, INT nMode = SFM_WAIT, BOOL bIsBond = FALSE);
	INT Z1_MoveToSearchBond(INT nPos, INT nMode = SFM_WAIT);
	INT Z2_MoveToSearchBond(INT nPos, INT nMode = SFM_WAIT);
	INT AC_Z1P_MoveToOrSearch(INT nPos, INT nMode = SFM_WAIT);
	INT AC_Z1B_MoveToOrSearch(INT nPos, INT nMode = SFM_WAIT, short sProcListSaveID = -1);
	INT AC_Z2P_MoveToOrSearch(INT nPos, INT nMode = SFM_WAIT);
	INT AC_Z2B_MoveToOrSearch(INT nPos, INT nMode = SFM_WAIT, short sProcListSaveID = -1);
	INT AC_Z1_MoveTo(INT nPos, INT nMode = SFM_WAIT, BOOL bToBond = FALSE, short sProcListSaveID = -1);
	INT AC_Z2_MoveTo(INT nPos, INT nMode = SFM_WAIT, BOOL bToBond = FALSE, short sProcListSaveID = -1);
	INT Z_Sync();
	INT Z2_Sync();
	INT Z_PowerOn(BOOL bOn = TRUE);
	INT Z2_PowerOn(BOOL bOn = TRUE);
	INT Z_Comm();
	INT Z2_Comm();
	BOOL Z_IsComplete();
	BOOL Z2_IsComplete();
	BOOL Z_IsPowerOn();	
	BOOL Z2_IsPowerOn();
	INT  Z_MoveToHome();
	INT  Z2_MoveToHome();

	INT	 Z_SelectControl(LONG lRelDistance, CONST BOOL bAtBond);
	INT	 Z2_SelectControl(LONG lRelDistance, CONST BOOL bAtBond);

	//Open DAC with velocity damping
	INT Z_OpenDACwithVelDamping(LONG lDACValue, LONG lHoldTime);
	INT Z2_OpenDACwithVelDamping(LONG lDACValue, LONG lHoldTime);

	INT Z_SetOpenDACSwPort(BOOL bState);
	INT Z2_SetOpenDACSwPort(BOOL bState);

	BOOL IsBHZ1ToPick();
	BOOL IsBHZ2ToPick();
	INT T_Home(BOOL bProgramInit=FALSE);				//v4.55A8
	INT GetContourMoveState(LONG &lMoveState, INT nPos, INT &lRelativeDistance);
	INT	BA_MoveTo(INT nPos, INT nMode = SFM_WAIT, BOOL bCheckMotionSync = TRUE);
	INT T_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT T_SMoveTo(INT nPos, INT nMode = SFM_WAIT);		//v4.53A14
	INT T_Move(INT nPos, INT nMode = SFM_WAIT, CString szProfile = "");
	INT T_Sync();
	INT T_PowerOn(BOOL bOn = TRUE);
	INT T_Comm();
	BOOL T_IsComplete();
	BOOL T_IsContourMoveComplete(BOOL bDisableILCCleanLog=FALSE);
	BOOL T_IsPowerOn();			//v3.13T3
	BOOL T_SetContourProfile(FLOAT *fPos, FLOAT *fVel, FLOAT *fAcc, FLOAT *fJerk, UINT uiNoofPoint, UINT uiStartOffset);
	INT T_SelectControl(LONG lRelDistance, LONG lMoveMode);
	
	INT  T_ContourMove(INT nPos, INT nDataLength, INT nStartOffset, INT nMode = SFM_WAIT, BOOL bCheckMotionSync = FALSE);

	BOOL HomeEjModule();
	BOOL HomeBinEjModule();

	INT Ej_Home();
	INT Ej_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Ej_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Ej_Sync();
	INT Ej_PowerOn(BOOL bOn = TRUE);
	INT Ej_Comm();
	BOOL Ej_IsComplete();
	BOOL Ej_IsPowerOn();

	INT BinEj_Home();
	INT BinEj_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT BinEj_Move(INT nPos, INT nMode = SFM_WAIT);
	INT BinEj_Sync();
	INT BinEj_PowerOn(BOOL bOn = TRUE);
	INT BinEj_Comm();
	BOOL BinEj_IsComplete();
	BOOL BinEj_IsPowerOn();

	INT EjT_Home();
	INT EjT_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT EjT_Sync();
	INT EjT_PowerOn(BOOL bOn = TRUE);
	INT EjT_Comm();
	BOOL EjT_IsPowerOn();

	INT BinEjT_Home();
	INT BinEjT_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT BinEjT_Sync();
	INT BinEjT_PowerOn(BOOL bOn = TRUE);
	INT BinEjT_Comm();
	BOOL BinEjT_IsPowerOn();

	INT EjCap_Home();
	INT EjCap_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT EjCap_Sync();
	INT EjCap_PowerOn(BOOL bOn = TRUE);
	INT EjCap_Comm();
	BOOL EjCap_IsPowerOn();

	INT BinEjCap_Home();
	INT BinEjCap_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT BinEjCap_Sync();
	INT BinEjCap_PowerOn(BOOL bOn = TRUE);
	INT BinEjCap_Comm();
	BOOL BinEjCap_IsPowerOn();

//	INT EjX_Home();
//	INT EjY_Home();
	INT EjX_PowerOn(BOOL bOn = TRUE);
	INT EjY_PowerOn(BOOL bOn = TRUE);
	INT EjX_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT EjY_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT EjX_Move(INT nPos, INT nMode = SFM_WAIT);
	INT EjY_Move(INT nPos, INT nMode = SFM_WAIT);
	INT EjX_Sync();
	INT EjY_Sync();
	BOOL EjX_IsPowerOn();
	BOOL EjY_IsPowerOn();


	//ES101 EJ Elevator Z fcns	//v4.24
	BOOL InitES101Hardware();
	INT EjElevator_Home();
	INT EjElevator_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT EjElevator_Sync();
	INT EjElevator_PowerOn(BOOL bOn = TRUE);
	BOOL EjElevator_IsPowerOn();
	INT  EjElv_MoveToHome();
	BOOL AutoLearnEjElvLevelUsingAirFlowSensor(BOOL bBHZ2 = FALSE);	//v4.51A6

	INT BinEjElevator_Home();
	INT BinEjElevator_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT BinEjElevator_Sync();
	INT BinEjElevator_PowerOn(BOOL bOn = TRUE);
	BOOL BinEjElevator_IsPowerOn();

	BOOL NGPickT_Home();
	BOOL NGPickT_IsPowerOn();
	INT	 NGPickT_PowerOn(BOOL bOn = TRUE);
	INT  NGPickT_Move(INT nPos, INT nMode = SFM_WAIT);
	INT  NGPickT_Sync();

	//v4.59A44	//MS50 AutoChgCollet module
	BOOL MS50ChgColletT_Home();
	BOOL MS50ChgColletZ_Home();

	INT  MS50ChgColletT_PowerOn(BOOL bOn);
	INT	 MS50ChgColletT_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT  MS50ChgColletT_Move(INT nPos, INT nMode = SFM_WAIT);
	INT	 MS50ChgColletT_Sync();

	INT  MS50ChgColletZ_PowerOn(BOOL bOn);
//	INT  MS50ChgColletZ_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT  MS50ChgColletZ_Move(INT nPos, INT nMode = SFM_WAIT);
	INT	 MS50ChgColletZ_Sync();

	LONG BH_MoveChgColletPosition(const LONG lPosition, const LONG lMovePosn);
	VOID BH_SetChgColletPosition(const LONG lPosition, const LONG lMovePosn);
	LONG BH_GetChgColletPosition(const LONG lPosition, const LONG lMoveOffset);
	LONG BH_ChangeChgColletPosition(IPC_CServiceMessage &svMsg);
	LONG BH_KeyInChgColletPosition(IPC_CServiceMessage &svMsg);
	LONG BH_MovePosChgColletPosition(IPC_CServiceMessage &svMsg);
	LONG BH_MoveNegChgColletPosition(IPC_CServiceMessage &svMsg);
	LONG CancelChgColletSetup(IPC_CServiceMessage& svMsg);
	LONG ConfirmChgColletSetup(IPC_CServiceMessage& svMsg);
	LONG SetupAutoLearnBondLevelBlks(Hp_Search_input &hp_LrnBondLevelInput_Z);

	// Sub-State Operation (BH_SubState.cpp)
	INT		OpInitialize();
	VOID	ReadUpdateBHZ1ColletThresholdValue(BOOL bCheckThresholdLimit);
	VOID	ReadUpdateBHZ2ColletThresholdValue(BOOL bCheckThresholdLimit);
	INT		SetProcessBlockList();
	INT		OpPreStart();
	INT		OpBadDie(BOOL bWait);		// Handle Bad Die Situation
	INT		OpColletJam();				// Handle Collet Jam Situation
	INT		OpReplaceDie();				// Handle Replace Die Situation
	BOOL	OpCheckColletJam();			// Check whether collet jam occurs
	BOOL	OpCheckMissingDie();		// Check whether missing die occurs
	BOOL	OpCheckMissingDie_PLLM();		// Check whether missing die occurs		//v4.54A1
	BOOL	OpCheckMissingDieResponseTime();		//v3.94
	BOOL	OpMissingDieUplookPrFailToStop(CONST LONG lCode);	//v4.59A5
	BOOL	ReachMissingDieCountLimit();
	BOOL	OpMissingDieUplookPrFailHandling(BOOL bUpLookFailAtMD, BOOL bRetryHandling);		//v4.59A5
	VOID	OpUpdateDieIndex();			// Update die index
	BOOL	IsMaxColletEjector(CONST BOOL bWaferEnd=FALSE);		// Check whether the collet and ejector hit the max
	BOOL	IsMaxColletEjector_CheckOnly(CONST BOOL bWaferEnd);	//v4.50A15
	INT		IsCleanCollet();			// Preform clean collet in MS100 only	//v3.86T5
	BOOL	OpManualCleanCollet();		//v3.64		//Original mechanism
	BOOL	OpManualCleanCollet_MDCJ_MS90();		//v4.59A7
	BOOL	OpAutoCleanCollet();		//v3.64		//New mechanism for Lexter (TW)

	VOID	MoveBHZToHome(const BOOL bBHZ2);
	VOID	MoveBHZToSwingLevel(const BOOL bBHZ2);
	VOID	BHZStrongBlowOn(const BOOL bBHZ2, const LONG lAirStrongBlowCount, const LONG lStrongBlowOnDelay, const LONG lStrongBlowOffDelay);
	BOOL	MoveToHolderStrongBlow(const BOOL bBHZ2, const BOOL bStrongBlowOn, const LONG lAirStrongBlowCount, const LONG lStrongBlowOnDelay, const LONG lStrongBlowOffDelay);

	BOOL	MoveToPrepick(const BOOL bBHZ2);
	BOOL	MoveToPrepickStrongBlow(const BOOL bBHZ2, const LONG lAirStrongBlowCount, const LONG lStrongBlowOnDelay, const LONG lStrongBlowOffDelay);
	BOOL	BHStrongBlowColletDieAtPrepick(const BOOL bBHZ2);
	VOID	BHZMoveToCleanColletZLevel(const BOOL bBHZ2);
	VOID	BHZMoveToRemoveDirtZLevel(const BOOL bBHZ2);
	BOOL	MoveBinTableToBondPosn(BOOL bToBond);
	BOOL	BTMoveToACCLiquid();
	BOOL	BTMoveToACCClean();
	BOOL	BTCycleACCOperation(const BOOL bBHZ2);
	BOOL	BTMoveToRemoveDirt();
	BOOL	BTCycleRemoveDirtOperation(const BOOL bBHZ2);

	BOOL	OpBTAutoCleanCollet(const BOOL bBHZ2, BOOL &bReturn);
	VOID	BHZAirFlowLog(const BOOL bBHZ2);
	BOOL	IsDoCleanCollet(const BOOL bBHZ2);
	BOOL	OpMS100AutoCleanCollet(const BOOL bBHZ2);
	BOOL	OpMS100AutoCleanCollet_MDCJ(BOOL bMissingDie);	//v4.44A1	//SEmitek
	LONG	OpCalCompen_Ej();			// Calculate the compensation for wear out of ejector pin
	LONG	OpCalCompen_Z(BOOL bBHZ2);	// Calculate the compensation for Z Drive-in at PICK due to MD	//v4.53A25
	BOOL	OpIncSKOffsetForBHZMD(BOOL bBHZ2);	//v4.53A25	//Semitek
	LONG	OpIncEjKOffsetForBHZMD();	// Calculate the compensation for MD issue		//v4.52A10
	BOOL	OpResetEjKOffsetAtWaferEnd();	//v4.43T8	//Semitek
	VOID	OpUpdateBondedUnit(const DOUBLE dCycleTime);		// Update bonded unit
	INT		OpSyncMove();				// Z & Ejector sync move
	INT		OpCheckThermalTest();
	INT		OpUpdateMachineCounters();	//v3.68T3
	INT		OpUpdateMachineCounters2();	//v3.68T3
	VOID	OpUpdateMS100EjectorCounter(INT nState);	//v4.40T12
	VOID	OpUpdateMS100ColletCounter();	//v4.40T12
	BOOL	OpAlarmCodeHandler();		//v3.86		//MS100 only
	BOOL	OpSetMS100AlarmCode(CONST INT nCode);	//v3.98T1	//MS100
	VOID	OpCheckColletsB4Autobond();	//v3.87		//MS100 only
	VOID	OpEMOStopAutobond();		//v3.91		//MS100 only
	BOOL	OpIsThermalAlarmTriggered();	//v4.26T1
	BOOL	OpCheckAlarmCodeOKToPick(BOOL bUseLastCode = FALSE);	//v4.15T1	
	BOOL	OpIsNextDieChangeGrade();	//v4.15T1	//CMLT
	BOOL	OpIsNextDieBinFull();		//v4.21T2	//Cree HuiZhou
	BOOL	OpIsBinTableLevelAtDnLevel();		//v4.22T8	//Walsin China 
	BOOL	OpIsLumiledsSpecialMDCheck();		//v4.54A7
	BOOL	OpLumiledsSpecialMDCheck();			//v4.54A1
	BOOL	OpUpdateColletHoleShiftTestCounters();	//v4.59A30
	//NGPick Fcns	//v4.24T8
	BOOL	OpPreStartNGPickModule(BOOL bStart);
	BOOL	OpNGPickIndexReelT();		//v4.27
	BOOL	OpNGPickCheckSensorStatus();//v4.27
	BOOL	OpReloadNGPickReelT();		//v4.27
	BOOL	OpCheckValidAccessMode(BOOL bCheckInBondCycle);
	BOOL    OpCheckStartLotCount();
	//EJT XY //v4.42T3
	BOOL	OpMoveEjectorTableXY(BOOL bStart = FALSE, INT nMode = SFM_NOWAIT);
	BOOL	OpSwitchEjectorTableXY(BOOL bBH1, INT nMode = SFM_NOWAIT);
	BOOL	OpSyncEjectorTableXY();
	BOOL	OpCheckEjtXYBHPos(BOOL bIsBH1ToPick);		//v4.46T10
	LONG	OpCalKOffset_BH1();
	LONG	OpCalKOffset_BH2();

	BOOL	OpPrestartColletHoleCheck(BOOL bBH1, BOOL bPreStart = TRUE);	//Wafer
	BOOL	OpPrestartBPRColletHoleCheck(BOOL bBH1);//Bond
	BOOL	CheckBPRErrorNeedAlarm(LONG lMark);
	//MS100P3	//v4.48A1
	BOOL	OpCheckMS60BHFanOK();
	BOOL	OpAutoLearnPickBondLevels(CONST BOOL bAuto);
	BOOL	HasBHZ2();
	//SanAn (WH) PostBond Empty Cnt BHZ Offset Fcn
	BOOL	OpCheckPostBondEmptyCountToIncBHZ();
	BOOL	OpAutoLearnBHZPickBondLevel(BOOL bBHZ2, BOOL bLearnGelPad = FALSE);			//After AutoCleanCollet	//v4.49A6
	BOOL	OpAutoLearnBHZPickBondLevel2(BOOL bBHZ2=FALSE);					//v4.50A12
	BOOL	OpCheckColletExistOnBH(BOOL bBHZ2=FALSE);						//v4.51A5
	BOOL	OpCheckSPCAlarmTwice(BOOL bBHZ2, CString szLog);				//v4.52A7

	VOID	OpPreStartInitDAirFlowThresold();								//v4.52A11
	VOID	OpDisplayAlarmPage(BOOL bDisplay);								//v4.53A13
	VOID	LogPBFailureCaseDumpFile();
	VOID	GetWaferTableEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis);
	BOOL	m_bDetectDAirFlowAtPrePick;
	INT		m_nDAirFlowBHZ1ThresFailCount;
	INT		m_nDAirFlowBHZ2ThresFailCount;
	BOOL	OpUpdateDAirFlowThresholdValue(BOOL bBHZ2=FALSE);				//v4.52A11
	BOOL	BH_Z_AirFlowLog(CONST BOOL bIsZ1, CONST CString szText);
	BOOL	m_bAutoCleanColletNow1;		//v4.50A4	//For PB EMPTY limit	//SanAn
	BOOL	m_bAutoCleanColletNow2;		//v4.50A4	//For PB EMPTY limit	//SanAn
	BOOL	m_bEnableAutoCCNowCounter1;
	BOOL	m_bEnableAutoCCNowCounter2;
	LONG	m_lAutoCCNowCounter;
	
	//v4.48A10	//MS60
	BOOL	InitMS60ThermalCtrl();
	BOOL	OpBurnInPreStartBHTThermalValues();
	BOOL	OpBurnInCalculateBHTThermalValues();
	
	//AutoChangeCollet Fcns	//v4.50A5
	VOID SetChgColletClamp(BOOL bSet);
	VOID SetChgColletPusher(BOOL bSet);
	BOOL IsColletPusherAtSafePos();				//v4.51A17
	VOID SetChgColletUploadPusher(BOOL bSet);
	VOID SetChgColletHolderVacuum(BOOL bSet);
	VOID SetColletHolderStrongBlow(BOOL bSet);

	BOOL IsBHBTReady(CString &szErrorMess);
	BOOL IsAutoChangeColletReady();
	VOID TurnOffPickVacuum(const BOOL bUseBHZ2);
	LONG MoveToChangeColletSafePosn();
	BOOL BTMoveToAGCClampPos(BOOL bBHZ2);
	BOOL BTMoveToAGCUPLUploadPos();
	BOOL BTMoveToAGCHolderUploadPos();
	BOOL BTMoveToAGCHolderInstallPos(BOOL bBHZ2);
	BOOL BTMoveToAGCPusher3Pos(BOOL bBHZ2);
	BOOL BTMoveToAGCUplookPos(BOOL bBHZ2);
	BOOL BTMoveToBondPosn(BOOL bToBond);
	BOOL BTMoveToAGCBondPosn(BOOL bToBond);
	BOOL MoveBTForAutoCColletAutoLearnZ(BOOL bShift);

	BOOL DetachCollet(const BOOL bBHZ2 = FALSE);
	BOOL UploadCollet();
	BOOL IsColletClogged(const BOOL bBHZ2);
	BOOL InstallCollet(const BOOL bBHZ2, const BOOL bAuto);
	BOOL SearchUploadCollet(LONG &lOffsetX_UM, LONG &lOffsetY_UM);
	BOOL SearchCollet(const BOOL bBHZ2 = FALSE);
	BOOL OpAutoChangeColletZ(const BOOL bBHZ2, CString &szErrMess);
	BOOL OpAutoChangeColletZ1();				//v4.50A15
	BOOL OpAutoChangeColletZ2();				//v4.50A15
	BOOL AGC_UpdateWprEjtOffset(BOOL bBHZ2, LONG lOffsetXinStep, LONG lOffsetYinStep);
	BOOL AGC_WTMoveDiff(BOOL bBHZ2, LONG lDiffX, LONG lDiffY);

	BOOL TurnOnBlowAndCatchModule(BOOL bOn);	//v4.59A12

	ULONG GetBHZ1MissingDieThresholdValue(const ULONG ulBHZ1DAirFlowUnBlockValue);
	ULONG GetBHZ1MissingDieThresholdValue(const ULONG ulBHZ1DAirFlowUnBlockValue, const ULONG ulBHZ1DAirFlowBlockValue);
	ULONG GetBHZ2MissingDieThresholdValue(const ULONG ulBHZ2DAirFlowUnBlockValue);
	ULONG GetBHZ2MissingDieThresholdValue(const ULONG ulBHZ2DAirFlowUnBlockValue, const ULONG ulBHZ2DAirFlowBlockValue);

	ULONG GetBHZ1ColletJamThresholdValue(const ULONG ulBHZ1DAirFlowUnBlockValue);
	ULONG GetBHZ2ColletJamThresholdValue(const ULONG ulBHZ2DAirFlowUnBlockValue);


	BOOL	m_bAutoChgCollet1Fail;				//v4.51A4
	BOOL	m_bAutoChgCollet2Fail;				//v4.51A4
	LONG	m_lAGCClampLevelOffsetZ;
	LONG	m_lAGCUplookPRLevelOffsetZ;
	LONG	m_lMS50ChgCollet1stPos_T;			//v4.59A45
	LONG	m_lMS50NoOfCollets;					//v4.59A45
	LONG	m_lMS50CurrColletIndex;				//v4.59A45
	//LONG	m_lAGCCollet1OffsetX;
	//LONG	m_lAGCCollet1OffsetY;
	//LONG	m_lAGCCollet2OffsetX;
	//LONG	m_lAGCCollet2OffsetY;

	BOOL	m_bMS60DetectBHFan;				//v4.48A33
	DOUBLE	m_dThermalSetValue;
	//v4.53A20
	DOUBLE	m_dMS60Thermal_P;
	DOUBLE	m_dMS60Thermal_I;
	DOUBLE	m_dMS60Thermal_D;

	BOOL    m_bILCSlowProfile;

#ifdef NU_MOTION
	GMP_CHAR m_gszPB1DetectPortName[128];
	GMP_CHAR m_gszPB1SourcePortName[128],	m_gszPB1DestinationPortName[128];
	GMP_CHAR m_gszPB2DetectPortName[128],	m_gszPB2ChannelName[128];
	GMP_CHAR m_gszPB3DetectPort1[128],		m_gszPB3DetectPort2[128];
	GMP_CHAR m_gszPB4DetectPortName[128],	m_gszPB4ChannelName[128];
#endif

	//	CP100 ACP fcn
	BOOL	MoveWaferTableNoCheck(LONG lXAxis, LONG lYAxis);
	BOOL	IsBT1FrameLevel();
	BOOL	IsBT2FrameLevel();
private:

	VOID GetSensorValue();		// Get all sensor values from hardware
	VOID GetEncoderValue();		// Get all encoder values from hardware
	VOID GetCommanderValue();
	VOID GetAxisInformation();	//NuMotion
	VOID GetEjAxisInformation();	//M94

	//LONG CheckThetaMotorType(void);		// Check Bonhead / ejector Motor type (Servo or Stepper) 
	//LONG CheckZMotorType(void);
	//LONG CheckEjectorMotorType(void);

	BOOL IsMotionHardwareReady();		// Check whether the motion hardware are ready
	BOOL IsAllMotorsEnable();
	BOOL IsZMotorsEnable();

	BOOL SetWaferTableJoystick(BOOL bState);
	BOOL LockPRMouseJoystick(BOOL bLock);
	BOOL LockWaferTableJoystick(BOOL bLock);
	BOOL CheckWExpanderSafeToMove();		//v2.74
	//andrewng //2020-0727
	BOOL MoveWaferTableToDummyPos(BOOL bMove);
	BOOL MoveBinTableToDummyPos(BOOL bMove);

	BOOL SwitchPrAOICamera();				//v3.93

	BOOL ResetColletCount();
	BOOL ResetCollet2Count();				//v3.92
	BOOL ResetColletCountSelection(int nCollet);
	BOOL ResetEjectorSelection();
	BOOL ResetCleanColletCount();
	VOID ResetEjectorKCount();
	BOOL ResetEjectorCount();

	BOOL AutoLearnBondLevelUsingCTSensor(LONG lPosition);
	BOOL AutoLearnBHZ1BondLevel(LONG &lEnc, BOOL bHome = TRUE, LONG lOldEnc = 0, LONG lMaxOffset = 0);		//v3.94T5	//v4.00T1
	BOOL AutoLearnBondLevelUsingCTSensorZ2(LONG lPosition);
	BOOL AutoLearnBHZ2BondLevel(LONG &lEnc, LONG lOldEnc = 0, LONG lMaxOffset = 0);						//v3.94T5

	BOOL AutoLearnEjUpLevelWithAitFlowSnr(LONG &lEnc, LONG lBHZOffsetZ);	//v4.48A3

	VOID SendCE_ToolLife(VOID);
	VOID SendCE_BondCounter(VOID);
	VOID SendCE_CurrWFCounter(VOID);
	VOID SendCE_EjKOffset(VOID);
	
	BOOL BH_LOG_OPEN();
	BOOL BH_LOG(CONST CString szMsg);
	VOID BH_LOG_CLOSE();
	BOOL BH_LOG_MESSAGE(CONST CString szMsg);
	BOOL		m_bIsBHLogOpened;
	CStdioFile	m_oBHLogFile;

	INT EnableBHZOpenDac(CONST BOOL bEnable);		//andrew
	
	//Open DAC function
	LONG AutoLearnDAC(BOOL bIsPickSide, LONG lCurrBHZ, BOOL bIsCalib = FALSE);
	LONG OpenDACCalibration(LONG lCurrBHZ);
	LONG OpenDACForceCheck(LONG lCurrBHZ);
	LONG ConvertFromForceToDAC(LONG lForce, LONG lCurrBHZ);
	LONG ConvertFromDACToDistance(LONG lDAC, LONG lCurrBHZ);

	VOID AllowColletJamStable();

	//BOOL LogBondHeadStatus(CString szMsg);
	VOID LogProfileTime();

	LONG SetUBHZChannelProtection(BOOL bEnable);
	LONG SetUBHTChannelProtection(BOOL bEnable);

	BOOL GenerateAccumlateDieCountReport();
	
	BOOL SetPrAutoBondMode(BOOL bAutoBondMode);
	BOOL Search2DBarCodePattern(CString &szCode, BOOL bBH2 = FALSE, BOOL bRetry = TRUE);		//v4.40T1
	BOOL IsCheckPr2DCode();					//v4.41T3
	BOOL UplookPrSearchDie(BOOL bBH2);		//v4.52A16
	BOOL UplookPrSearchDie_Rpy1(BOOL bBH2);		//v4.52D17
	BOOL UplookPrSearchDie_Rpy2(BOOL bBH2);	
	BOOL AutoUpLookSearchDie_Rpy2(LONG lBH2, LONG& lDieCenterXInPixel, LONG lDieCenterYInPixel, DOUBLE& dDieAngle);
	PR_UWORD UpLookExtractDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, 
		BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, 
		PR_REAL *fDieScore);
	BOOL DieIsAlignable(PR_UWORD usDieType);
	PR_UWORD SearchUpLookDieCmd(BOOL bBHZ2);

	BOOL AreAllBinsCleared();
	LONG LogItems(LONG lEventNo);
	LONG GetLogItemsString(LONG lEventNo, CString &szMsg);

protected:
	// Operation state
	enum {	WAIT_WT_Q,
			HOUSE_KEEPING_Q,
			WAIT_BT_CHANGE_GRADE,
			WAIT_BIN_FULL_Q,

			WAIT_WT_READY_Q,
			WAIT_BT_READY_Q,
			EJ_THETA_UP_Q,
			BOND_Z_WAIT_BT_READY_Q,
			PICK_Z2_DOWN_Q,
			EJ_UP_Q,
			WAIT_PICK_DELAY_Q,
			EJ_DOWN_Q,
			EJ_THETA_DOWN_Q,
			MOVE_T_Q,
			WAIT_Z_EJ_COMPLETE_Q,

			MOVE_T_PREPICK_TO_PICK_Q,
			PICK_Z_DOWN_Q,	
			MOVE_Z_PICK_UP_Q,
			MOVE_T_THEN_EJ_Q,
			MOVE_EJ_THEN_T_Q,
			POSTBOND_FAIL_Q,
			SEARCH_COLLET_Q,
			ILC_RESTART_UPDATE_Q,
			SEARCH_MARK_Q,
			BOND_Z_TO_SWING_Q,
			WAIT_PR_START_Q,
			MOVE_T_TO_PICK_Q,
			EJ_UP_Q_PRESCAN_AREA_PICK,
			MOVE_Z_TO_BOND_Q,
			WAIT_BOND_DELAY_Q,
			WAIT_BT_START_MOVE_Q,
			MOVE_Z_EJ_SYNC_Q,
			AP_BOND_LAST_DIE_Q
		 };

protected:

	//SFM_CHipecDcServo	*m_pDcServo_Z;	

	// Axis Info	//NuMotion
	CMSNmAxisInfo		m_stBHAxis_Z;
	CMSNmAxisInfo		m_stBHAxis_Z2;

	CMSNmAxisInfo		m_stBHAxis_Ej;
	CMSNmAxisInfo		m_stBHAxis_EjElevator;			
	CMSNmAxisInfo		m_stBHAxis_EjX;					
	CMSNmAxisInfo		m_stBHAxis_EjY;					
	CMSNmAxisInfo		m_stBHAxis_EjT;					
	CMSNmAxisInfo		m_stBHAxis_EjCap;

	CMSNmAxisInfo		m_stBHAxis_BinEj;
	CMSNmAxisInfo		m_stBHAxis_BinEjElevator;			
	CMSNmAxisInfo		m_stBHAxis_BinEjX;					
	CMSNmAxisInfo		m_stBHAxis_BinEjY;					
	CMSNmAxisInfo		m_stBHAxis_BinEjT;			
	CMSNmAxisInfo		m_stBHAxis_BinEjCap;

	CMSNmAxisInfo		m_stBHAxis_NGPick;				
	CMSNmAxisInfo		m_stBHAxis_ProberZ;

	//v4.59A44
	CMSNmAxisInfo		m_stBHAxis_ChgColletT;
	CMSNmAxisInfo		m_stBHAxis_ChgColletZ;

	BOOL				m_bES101Configuration;			//v4.24
	//BOOL				m_bMS100EjtXY;					//v4.41T5
	BOOL				m_bMS100DigitalAirFlowSnr;		//v4.43T8
	BOOL				m_bMS60EjElevator;				//v4.51A5	//MS60
	BOOL				m_bMS50ChgColletZT;				//v4.59A44

	DOUBLE				m_dEjtXYRes;
	DOUBLE				m_dEjRes;
	DOUBLE				m_dEjElevatorRes;

	BOOL				m_bUpdateOutput;				// Update Sensor & encoder in HMI
	BOOL				m_bHome_Z;						// Z Home flag
	BOOL				m_bHome_Z2;						// Z Home flag
	BOOL				m_bHome_T;						// T Home flag

	BOOL				m_bHome_Ej;						// Ejector Home flag
	BOOL				m_bHome_EjX;					// Ejector X Home flag
	BOOL				m_bHome_EjY;					// Ejector Y Home flag
	BOOL				m_bHome_EjElevator;				// Ejector Elevator Home flag	
	BOOL				m_bHome_EjCap;					// Ejector Cap Home flag
	BOOL				m_bHome_EjT;					// Ejector Theta Home flag

	BOOL				m_bHome_BinEj;					// Bin Ejector Home flag
	BOOL				m_bHome_BinEjElevator;			// Bin Ejector Elevator Home flag
	BOOL				m_bHome_BinEjCap;				// Bin Ejector Cap Home flag
	BOOL				m_bHome_BinEjT;					// Bin Ejector Theta Home flag

	BOOL				m_bHome_NGPick;					// NGPick stepper Home flag	//v4.24 (no HOME sensor)
	BOOL				m_bHome_ChgColletT;
	BOOL				m_bHome_ChgColletZ;

	BOOL				m_bComm_Z;						// Z Communtate flag
	BOOL				m_bComm_Z2;						// Z Communtate flag
	BOOL				m_bComm_T;						// T Communtate flag
	BOOL				m_bComm_Ej;						// Ejector Communtate flag
	BOOL				m_bComm_BinEj;	
	BOOL				m_bComm_EjCap;
	BOOL				m_bComm_BinEjCap;
	BOOL				m_bComm_EjT;
	BOOL				m_bComm_BinEjT;
	BOOL				m_bComm_ChgColletT;
	BOOL				m_bComm_ChgColletZ;

	BOOL				m_bMissingDieSnr;				// Missing Die Sensor
	BOOL				m_bMissingDieSnrZ2;
	BOOL				m_bPressureSnr;					// Machine pressure sensor
	BOOL				m_bVacuumFlowSnr;

	BOOL				m_bCleanColletSafetySensor_HMI;
	BOOL				m_bMainFrontCover_HMI;		// Machine cover sensor
	BOOL				m_bMainFrontLeftCover_HMI;
	BOOL				m_bMainFrontMiddleCover_HMI;
	BOOL				m_bMainRearLeftCover_HMI;
	BOOL				m_bMainRearRightCover_HMI;
	BOOL				m_bMainLeftCover_HMI;

	BOOL				m_bIsFrontCoverExist;			// Machine Front cover lock
	BOOL				m_bIsSideCoverExist;			// Machine Side cover lock
	BOOL				m_bIsBinElevatorCoverExist;		// MS109
	LONG				m_lBHArmMaxTravel;	

	// Select module flag
	BOOL				m_bSel_Z;			
	BOOL				m_bSel_Z2;			
	BOOL				m_bSel_T;			
	BOOL				m_bSel_Ej;				
	BOOL				m_bSel_BinEj;

	// Power 
	BOOL				m_bIsPowerOn_Z;
	BOOL				m_bIsPowerOn_Z2;
	BOOL				m_bIsPowerOn_T;

	BOOL				m_bIsPowerOn_Ej;
	BOOL				m_bIsPowerOn_EjX;
	BOOL				m_bIsPowerOn_EjY;
	BOOL				m_bIsPowerOn_EjElevator;	//v4.24
	BOOL				m_bIsPowerOn_EjCap;
	BOOL				m_bIsPowerOn_EjT;

	BOOL				m_bIsPowerOn_BinEj;
	BOOL				m_bIsPowerOn_BinEjElevator;	//v4.24
	BOOL				m_bIsPowerOn_BinEjCap;
	BOOL				m_bIsPowerOn_BinEjT;

	BOOL				m_bIsPowerOn_NGPickT;
	BOOL				m_bIsPowerOn_ChgColletT;
	BOOL				m_bIsPowerOn_ChgColletZ;

	// Is Arm 2 Exist (for DBH)
	BOOL				m_bIsArm2Exist;
	BOOL				m_bNuMotionSystem;

	CString				m_szEjSoftTouch;
	CString				m_szEjCapSoftTouch;
	CString				m_szEjSoftTouchMsg;			//v4.59A15

	// Event (BH_Event.cpp)
	CEvent				m_evAutoChangeColletDone;
	CEvent				m_evPRLatched;		// WPR latched the image
	CEvent				m_evPRStart;		// WPR start latching the image
	CEvent				m_evBadDieForT;			// WPR inspected a bad die
	CEvent				m_evEjectorReady;	// Ejector in the level that Wafer Table can move
	CEvent				m_evBTReady;		// Bin Table ready for bonding
	CEvent				m_evBhTReadyForBPR;	// BondHead T Ready for BPR Post-Bond at Bond		//v2.60
	CEvent				m_evBhTReadyForWPR;	// BondHead T Ready for BPR Post-Bond at Bond		//v3.34
	CEvent				m_evBhTReadyForWT;	// BondHead T Ready for WT							//v4.46T22
	CEvent				m_evWTReady;		// Wafer Table is ready for Bond Head Z down
	CEvent				m_evWTStartMove;	// for DBH only; triggered by WT when WT starts INDEX-MOVE to next die	//v3.66
	CEvent				m_evCompDone;		//For DBH only; wait to check if WT CP@ is done
	CEvent				m_evMoveBack;		// Wafer Table is requested to move back
	CEvent				m_evWTBacked;		// Wafer Table is moved back
	CEvent				m_evBhTReady;		// T ready for BPR start grabbing
	CEvent				m_evDiePicked;		// Die is picked
	CEvent				m_evDieBonded;		// Die is Bonded
	CEvent				m_evDieBondedForWT;	// Die is Bonded for Wafer Table
	CEvent				m_evBHInit;			// Bond Head is initiated event
	CEvent				m_evConfirmSrch;	// Confirm Search event from Wafer Table
	CEvent				m_evBhToPrePick;	// Trigger WPR after Confirm-Search, when BH to Pre-Pick
	CEvent				m_evBTStartMove;	// Bin Table Start Move event
	CEvent				m_evBTStartMoveWithoutDelay; // Bin Table Start Move without delay event
	CEvent				m_evExpInit;		// Expander event to trigger T Home		//andrew
	CEvent				m_evBackupNVRam;
	CEvent				m_evBLFrameLevelDn;	// Bintable frame level platform at DOWN pos 
	CEvent				m_evBTReSrchDie;	// Confirm Search event		//v4.xx
	CEvent				m_evBPRPostBondDone;	//v4.40T6
	CEvent				m_evBTCompensate;	// trigger BH Uplook Pr compensation by BT	//v4.52A16
	CEvent				m_evSearchMarkReady;
	CEvent				m_evSearchMarkDone;
	CEvent				m_evBhReadyForWPRHwTrigger;
	CEvent				m_evBhReadyForBPRHwTrigger;

	//Ejector Cap Motion Time
	LONG                m_lUpTime_Ej_Cap;
	LONG                m_lDnTime_Ej_Cap;

	//Ejector Theta Motion Time
	LONG                m_lUpTime_Ej_Theta;// To Bond Level
	LONG                m_lDnTime_Ej_Theta;// To Standby Level

	//Bin Ejector Theta Motion Time
	LONG                m_lUpTime_BinEj_Theta;// To Bond Level
	LONG                m_lDnTime_BinEj_Theta;// To Standby Level

	//Ejector Pin Motion Time
	LONG                m_lTime_Ej;
	LONG				m_lSlowUpTime_Ej;
	LONG                m_lDnTime_Ej;			//v4.43T5	//MS100PlusII
	
	//Bin Ejector Pin Motion Time
	LONG                m_lTime_BinEj;
	LONG                m_lDnTime_BinEj;

	// Time
	LONG                m_lTime_T;
	LONG				m_lPrePickTime_T;		//Time from Pre-pick to Pick
	LONG				m_lBondTime_T;			//Time from Bond to Pick
	LONG                m_lPickTime_Z;
	LONG                m_lBondTime_Z;
	LONG                m_lPickUpTime_Z;		//v3.61
	LONG                m_lBondUpTime_Z;		//v3.61
	LONG                m_lPickTime_Z2;
	LONG                m_lBondTime_Z2;
	LONG                m_lPickUpTime_Z2;
	LONG                m_lBondUpTime_Z2;

	// Ejector Level
	LONG				m_lEjectLevel_Ej;	// Ejector Pin Up Level
	LONG				m_lPreEjectLevel_Ej;
	LONG				m_lStandbyLevel_Ej;
	LONG				m_lContactLevel_Ej;
	LONG				m_lCurLevel_Ej;
	LONG				m_lCurLevel_EjT;
	LONG				m_lCurLevel_EjCap;
	LONG				m_lCurLevel_EjElv;

	//	Bin Ejector Level
	LONG				m_lPreEjectLevel_BinEj;
	LONG				m_lContactLevel_BinEj;
	LONG				m_lCurLevel_BinEj;
	LONG				m_lCurLevel_BinEjT;
	LONG				m_lCurLevel_BinEjCap;
	LONG				m_lCurLevel_BinEjElv;

	// Ejector Cap Level
	LONG				m_lStandby_EJ_Cap;
	LONG				m_lUpLevel_EJ_Cap;
	LONG				m_lDnOffset_EJ_Cap;
	// Ejector Elv Level
	LONG				m_lStandby_EJ_Elevator;
	LONG				m_lUpLevel_EJ_Elevator;
	// Ejector Theta Level
	LONG				m_lStandby_EJ_Theta;
	LONG				m_lUpLevel_EJ_Theta;

	//Enable and Disable Display for wafer and bin ejector page
	BOOL				m_bLearnStandby_EJ_Pin;
	BOOL				m_bLearnStandby_EJ_Cap;
	BOOL				m_bLearnStandby_EJ_Elevator;
	BOOL				m_bLearnStandby_EJ_Theta;
	BOOL				m_bLearnUpLevel_EJ_Pin;
	BOOL				m_bLearnUpLevel_EJ_Cap;
	BOOL				m_bLearnUpLevel_EJ_Elevator;
	BOOL				m_bLearnUpLevel_EJ_Theta;
	BOOL				m_bLearnContact_EJ_Pin;			//andrewng //2020-0604

	// Bin Ejector Cap Level
	LONG				m_lStandby_Bin_EJ_Cap;
	LONG				m_lUpLevel_Bin_EJ_Cap;
	// Bin Ejector Elv Level
	LONG				m_lStandby_Bin_EJ_Elevator;
	LONG				m_lUpLevel_Bin_EJ_Elevator;
	// Bin Ejector Theta Level
	LONG				m_lStandby_Bin_EJ_Theta;
	LONG				m_lUpLevel_Bin_EJ_Theta;
	// Bin Ejector Pin Level
	LONG				m_lStandby_Bin_EJ_Pin;
	LONG				m_lUpLevel_Bin_EJ_Pin;

	// Ejector Table XY 
	LONG				m_lCDiePos_EjX;
	LONG				m_lCDiePos_EjY;
	LONG				m_lArm1Pos_EjX;
	LONG				m_lArm1Pos_EjY;
	LONG				m_lArm2Pos_EjX;
	LONG				m_lArm2Pos_EjY;
	// Theta Position
	LONG				m_lCurPos_T;
	LONG				m_lPrePickPos_T;
	LONG				m_lPickPos_T;
	LONG				m_lPreBondPos_T;
	LONG				m_lBondPos_T;
	LONG                m_lCleanColletPos_T;
	LONG                m_lChangeCarrierPos_T;
	// Z level
	LONG				m_lCurLevel_Z;
	LONG				m_lPickLevel_Z;
	LONG				m_lReplaceLevel_Z;
	LONG				m_lBondLevel_Z;
	LONG				m_lSwingLevel_Z;
	LONG				m_lSwingOffset_Z;
	LONG				m_lZ1BondLevelBT_X;
	LONG				m_lZ1BondLevelBT_Y;
	LONG				m_lZ2BondLevelBT_X;
	LONG				m_lZ2BondLevelBT_Y;
	BOOL				m_bZ1BondLevelBT_R;	//	Z1 BT rotated or not (180).
	BOOL				m_bZ2BondLevelBT_R;	//	Z2 BT rotated or not (180).
	CString				m_szLearnLevelBT;
	WAFFLE_PAD_LEVELS	m_stPadLevels[MAX_WAFFLE_PADS];
	BOOL				m_bBeginPadsLevel;
	LONG				m_lWPadsNum;
	LONG				m_lWPadsLevelOffsetZ1;
	LONG				m_lWPadsLevelOffsetZ2;
	// Auto Clean Collet
	LONG				m_lAutoCleanCollet_Z;
	LONG				m_lAutoCleanCollet_T;
	LONG				m_lBPAccPosT;
	LONG				m_lBPAccLvlZ;
	LONG				m_lAutoCleanOffsetZ;
	LONG				m_lAutoCleanAirBlowLimit;		//v4.16T5	//EverVision by Leo Hung
	LONG				m_lAutoCleanAutoLearnZLimit;	//v4.46T21
	LONG				m_lAutoCleanAutoLearnZCounter;	//v4.46T21
	LONG				m_lAutoCleanAutoLearnZOffset;	//v4.46T21
	LONG				m_lAutoCleanAutoLearnZ;
	BOOL				m_bAutoCleanAutoLearnZLevels;	//v4.49A6	//WH SanAn
	// Z2 level
	LONG				m_lCurLevel_Z2;
	LONG				m_lPickLevel_Z2;
	LONG				m_lReplaceLevel_Z2;
	LONG				m_lBondLevel_Z2;
	LONG				m_lSwingLevel_Z2;
	LONG				m_lSwingOffset_Z2;
	//MS100 9Inch dual-table config		//v4.21
	LONG				m_lBT2OffsetZ;
	//ES101 NGPick attributes			//v4.24
	LONG				m_lNGPickIndexT;
	//AutoLearn Option
	LONG				m_lPickDriveIn;
	LONG				m_lBondDriveIn;
	CString				m_szPickDriveInDisplay;		//v4.59A15	//Renesas MS90
	CString				m_szBondDriveInDisplay;		//v4.59A15	//Renesas MS90
	LONG				m_lEjectorDriveIn;
	LONG				m_lEjectorLimitType;
	BOOL				m_bOnVacuumAtPick;
	BOOL				m_bUseBHAutoLearn;			
	BOOL				m_bUseEjAutoLearn;			
	BOOL				m_bUseCTSensorLearn;		//v2.96T4
	// bonding delay
	LONG                m_lBinTableDelay;
	LONG                m_lPRDelay;
	LONG                m_lWTTDelay;		// Wafer Table Theta Delay (per degree)
	LONG				m_lWTSettlingDelay;
	LONG                m_lWeakBlowOffDelay;
	LONG                m_lHighBlowTime;
	LONG				m_lArmPickDelay;	// At bond: Z up start -> T move start
	LONG				m_lHeadPickDelay;	// At pick: T move end -> Z down end
	LONG				m_lEjectorUpDelay;	// At pick: Z down end -> Ej up end
	LONG				m_lPickDelay;		// Pick Delay
	LONG				m_lArmBondDelay;	// At pick: Z up start -> T move start
//	LONG				m_lEjectorDownDelay;// At pick: Ej up start -> WT move start
	LONG				m_lEjectorDownDelay;// At pick: Z up start -> Ej down start
	LONG				m_lHeadBondDelay;	// At bond: T move end -> Z down end
	LONG				m_lBondDelay;		// Bond Delay
	LONG				m_lFloatBlowDelay;	// Bond, soft touch, move to done, delay and on blow, then soft search to bond level.
	LONG				m_lEjReadyDelay;	// WT start before Ej end
	LONG				m_lEjVacOffDelay;	// Ej vac off -> WT INDEX	//v3.78
	LONG				m_lHeadPrePickDelay;	//v3.78
	LONG				m_lSyncPickDelay;	//v3.83
	LONG				m_lBlockCameraDelay;	//v4.52A3
	LONG				m_lUplookDelay;
	LONG				m_lNeutralDelay;
	//v3.62
	LONG                m_lMinBinTableDelay;
	LONG                m_lMinPRDelay;
	LONG                m_lMinWTTDelay;	
	LONG				m_lMinArmPickDelay;
	LONG				m_lMinHeadPickDelay;	
	LONG				m_lMinEjectorUpDelay;
	LONG				m_lMinPickDelay;	
	LONG				m_lMinArmBondDelay;
	LONG				m_lMinEjectorDownDelay;
	LONG				m_lMinHeadBondDelay;
	LONG				m_lMinBondDelay;
	LONG				m_lMinEjReadyDelay;
	LONG				m_lAutoCleanColletSwingTime;	//v3.77
	LONG				m_lAccLastDropDownTime;
	// Life Time Counters
	ULONG				m_ulColletCount;		// Replace/Change Collet
	ULONG				m_ulCollet2Count;		// Replace/Change Collet for BHZ2
	ULONG				m_ulCleanCount;			// Clean Collet
	DOUBLE				m_dLastPickTime;		// for auto clean collet, log last pick time.
	ULONG				m_ulCleanColletTimeOut;	// for auto clean collet, check time out. in seconds
	ULONG				m_ulEjectorCount;		// Ejector
	ULONG				m_ulEjectorCountForKOffset;		// Ejector K counter	//v4.43T8
	ULONG				m_ulEjCountForSubRegionKOffset;		//v4.45T4
	ULONG				m_ulEjCountForSubRegionSKOffset;	//v4.46T1
	ULONG				m_ulZ1CountForSmartKOffset;			//v4.53A25	//SEmitek
	ULONG				m_ulZ2CountForSmartKOffset;			//v4.53A25	//SEmitek
	ULONG				m_ulMaxColletCount;		// Maximum Collet count
	ULONG				m_ulMaxCollet2Count;	// Maximum Collet count for BHZ2
	ULONG				m_ulMaxEjectorCount;	// Maximum Ejector count
	ULONG				m_ulMaxCleanCount;		// Maximum Clean Collet count
	// Ejector Level Backup		//v4.44T4
	LONG				m_lBondHead1LearnLevelBackup;
	LONG				m_lBondHead2LearnLevelBackup;
	LONG				m_lBondHead1TempLearnLevel;
	LONG				m_lBondHead2TempLearnLevel;
	LONG				m_lBondHeadToleranceLevel;
	LONG				m_lZPBLevelOffsetLimit;
	// Custom Counter (SanAn)	//v3.68T3
	BOOL				m_bEnableCustomCount1;
	BOOL				m_bEnableCustomCount2;
	BOOL				m_bEnableCustomCount3;
	ULONG				m_ulCustomCounter1;
	ULONG				m_ulCustomCounter2;
	ULONG				m_ulCustomCounter3;
	ULONG				m_ulCustomCounter1Limit;
	ULONG				m_ulCustomCounter2Limit;
	ULONG				m_ulCustomCounter3Limit;
	LONG				m_lEjectorKOffset;			// Ejector K-Offset
	LONG				m_lEjectorKCount;			// Ejector K-Offset Count
	LONG				m_lEjectorKOffsetLimit;		// Ejector K-Offset Limit
	LONG				m_lEjectorKOffsetTotal;		// Ejector K-Offset Total
	LONG				m_lEjectorKOffsetForBHZ1MD;		//v4.52A10
	LONG				m_lEjectorKOffsetBHZ1Total;		//v4.52A10	
	LONG				m_lEjectorKOffsetForBHZ2MD;		//v4.52A10
	LONG				m_lEjectorKOffsetBHZ2Total;		//v4.52A10
	LONG				m_lEjectorKOffsetResetCount;

	LONG				m_lEjectorKOffsetForBHZ1MDCount;	 //M68
	LONG				m_lEjectorKOffsetForBHZ2MDCount;	 //M68
	BOOL				m_bCheckK1K2;						 //M68
	BOOL				m_bAutoChangeColletOnOff;

	BOOL				m_bResetEjKOffsetAtWaferEnd;
	LONG				m_lEjSubRegionKOffset;		// Ejector K-Offset (sub-region mode)
	LONG				m_lEjSubRegionKCount;		// Ejector K-Offset Count (sub-region mode)
	LONG				m_lEjSubRegionKOffsetTotal;	// Ejector K-Offset Total (sub-region mode)
	LONG				m_lEjSubRegionSKOffset;		// Ejector K-Offset (sub-region mode)
	LONG				m_lEjSubRegionSKCount;		// Ejector K-Offset Count (sub-region mode)
	LONG				m_lEjSubRegionSmartValue;	//v4.46T1	//Semitek
	LONG				m_lEjSubRegionSKOffsetTotal;// Ejector K-Offset Total (sub-region mode)
	//v4.53A25	//SEmitek MS100Plus2	//LeoLam & CK Harry		//9-27-2016
	LONG				m_lZ1SmartKOffset;			// Z K-Offset encoder increment step
	LONG				m_lZ1SmartKCount;			// Z K-Offset Counter Limit
	LONG				m_lZ1SmartValue;			// Z K-Offset no. of total steps accumulated
	LONG				m_lZ1SmartKOffsetTotal;		// Z K-Offset actual encoder comp value
	LONG				m_lZ2SmartKOffset;			// Z K-Offset encoder increment step
	LONG				m_lZ2SmartKCount;			// Z K-Offset Counter Limit
	LONG				m_lZ2SmartValue;			// Z K-Offset no. of total steps accumulated
	LONG				m_lZ2SmartKOffsetTotal;		// Z K-Offset actual encoder comp value
	//Digital Airflow meter on MS100PlusII/MS101	//v4.43T11	
	LONG				m_lDAFlowBH1KOffset;		// K-Offset
	LONG				m_lDAFlowBH1KCount;			// K-Offset Count
	LONG				m_lDAFlowBH1KOffsetLimit;	// K-Offset Limit
	LONG				m_lDAFlowBH1KOffsetTotal;	// K-Offset Total
	LONG				m_lDAFlowBH2KOffset;		// K-Offset
	LONG				m_lDAFlowBH2KCount;			// K-Offset Count
	LONG				m_lDAFlowBH2KOffsetLimit;	// K-Offset Limit
	LONG				m_lDAFlowBH2KOffsetTotal;	// K-Offset Total
	// BH Sync move function
	LONG				m_lSyncTime;
	BOOL				IsEnableSyncMove();
	//**Moved to MS896aStn class**
	ULONG				m_ulCurrWaferBondDieCount;		// Bonded Die		(Current wafer)
	ULONG				m_ulCurrWaferDefectDieCount;	// Defective Die	(Current wafer)
	ULONG				m_ulCurrWaferChipDieCount;		// Chip Die			(Current wafer)
	ULONG				m_ulCurrWaferInkDieCount;		// Chip Die			(Current wafer)
	ULONG				m_ulCurrWaferBadCutDieCount;	// Bad-cut Die		(Current wafer)
	ULONG				m_ulCurrWaferRotateDieCount;	// Rotate Die		(Current wafer)
	ULONG				m_ulCurrWaferEmptyDieCount;		// Empty Die		(Current wafer)
	ULONG				m_ulCurrWaferMissingDieCount;	// Missing Die		(Current wafer)
	DOUBLE				m_dCurrWaferSelGradeYield;		// Sel-Grade yield	(Current wafer)	  
	// Missing Die Retry count
	LONG				m_lMissingDie_Retry;	// User Input
	LONG				m_lMD_Count;			// Local Count 
	LONG				m_lMD_Count2;			// Local Count for BH2	(MS100)
	LONG				m_lMDCycleCount1;
	LONG				m_lMDCycleCount2;
	LONG				m_lMDTotalCount1;
	LONG				m_lMDTotalCount2;
	LONG				m_lNOMDCount1;
	LONG				m_lNOMDCount2;
	LONG				m_lCJTotalCount1;
	LONG				m_lCJTotalCount2;
	LONG				m_lCJContinueCounter1;
	LONG				m_lCJContinueCounter2;
	LONG				m_lMDCycleLimit;
	LONG				m_lNGNoDieLimit;
	ULONG				m_ulColletJam_Retry;	// Collet jam retry
	LONG				m_lSortTo2ndPartCounter;	//v4.59A8	//MS90
	//v4.22T1
	LONG				m_lMDRetry1Counter;
	LONG				m_lMDResetCounter;
	BOOL				m_bMDRetryOn;
	LONG				m_lMDRetry2Counter;
	LONG				m_lMD2ResetCounter;
	BOOL				m_bMD2RetryOn;			//v4.24T12
	LONG				m_lCJRetryCounter;
	LONG				m_lCJResetCounter;
	BOOL				m_bCJRetryOn;			//v4.24T12
	LONG				m_lCJ2RetryCounter;
	LONG				m_lCJ2ResetCounter;
	BOOL				m_bCJ2RetryOn;			//v4.24T12
	
	LONG				m_lBHUplookPrFailLimit;	//v4.59A5

	BOOL				m_bMDRetryUseAutoCleanCollet; //v4.49A3
	//v4.43T8
	BOOL	m_bEnableDAFlowRunTimeCheck;		//v4.53A1
	ULONG	m_ulBHZ1DAirFlowSetZeroOffset;
	ULONG	m_ulBHZ2DAirFlowSetZeroOffset;
	ULONG	m_ulBHZ1DAirFlowBlockValue;
	ULONG	m_ulBHZ1DAirFlowUnBlockValue;
	ULONG	m_ulBHZ1DAirFlowThreshold;
	ULONG	m_ulBHZ2DAirFlowBlockValue;
	ULONG	m_ulBHZ2DAirFlowUnBlockValue;
	ULONG	m_ulBHZ2DAirFlowThreshold;
	ULONG	m_ulBHZ1DAirFlowValue;
	ULONG	m_ulBHZ2DAirFlowValue;

	ULONG	m_ulBHZ1DAirFlowCleanColletThreshold;
	ULONG	m_ulBHZ2DAirFlowCleanColletThreshold;
	ULONG	m_lBHZ1ThresholdUpperLimit;

	//v4.52A11
	ULONG	m_ulBHZ1CurrThresholdCount;
	ULONG	m_ulBHZ2CurrThresholdCount;
	ULONG	m_ulBHZ1CurrThreshold[BH_MAX_THRESHOLD_DAIRFLOW];
	ULONG	m_ulBHZ2CurrThreshold[BH_MAX_THRESHOLD_DAIRFLOW];
	//v4.50A1
	DOUBLE	m_dBHZ1ThresholdPercent;
	DOUBLE	m_dBHZ2ThresholdPercent;		//Used for CJ 
	ULONG	m_lBHZ1ThresholdLimit;
	ULONG	m_lBHZ2ThresholdLimit;
	//v4.50A7
	ULONG	m_ulBHZ1DAirFlowThresholdCJ;
	ULONG	m_ulBHZ2DAirFlowThresholdCJ;

	LONG				m_lBHZ1HomeOffset;		//v4.46T26
	LONG				m_lBHZ2HomeOffset;		//v4.46T26
	DOUBLE				m_dBHZ1UplookCalibFactor;
	DOUBLE				m_dBHZ2UplookCalibFactor;
	BOOL				m_bAutoLearningBHZ;
	LONG				m_lBHZAutoLearnDriveInLimit;
	LONG				m_lBHZOffsetbyPBEmptyCheck;		//v4.48A8	//3E DL, WH SanAn
	LONG				m_lCurrOffsetZ1byPBEmptyCheck;	//v4.48A8
	LONG				m_lCurrOffsetZ2byPBEmptyCheck;	//v4.48A8

	BOOL				m_bShowAlarmPage;

	// For cycle time measurement
	LARGE_INTEGER		m_liStart;
	double				m_dFreqClk;
	LARGE_INTEGER		m_liSync;
	double				m_dCycleTime;
	double				m_dAvgCycleTime;
	ULONG				m_ulCycleCount;
	double				m_dLastTime;	
	LONG				m_lAutobondStartTime;	//v2.83T6
	LONG				m_lAutobondTotalTime;	//v2.83T27
	LONG				m_lBH1MarkCount;
	LONG				m_lBH2MarkCount;
	//BOOL				m_bSearchColletNow1;
	//BOOL				m_bSearchColletNow2;
	BOOL				m_bDoColletSearch;
	LONG				m_lBPRErrorToCleanColletCount;
	BOOL				m_bBPRErrorToCleanCollet;

	/*--- Server Variables ---*/
	// Sensor state
	BOOL				m_bHomeSnr_Z;			// Z Home Sensor
	BOOL				m_bHomeSnr_T;			// T Home Sensor
	BOOL				m_bHomeSnr_Ej;			// Ejector Home Sensor
	BOOL				m_bHomeSnr_EjElevatorZ;	// Ejector Elevator Sensor
	BOOL				m_bHomeSnr_EjCap;		// Ejector Cap Home Sensor
	BOOL				m_bHomeSnr_EjT;			// Ejector Theta Home Sensor

	BOOL				m_bHomeSnr_BinEj;			// Bin Ejector Home Sensor
	BOOL				m_bHomeSnr_BinEjElevatorZ;	// Bin Ejector Elevator Sensor
	BOOL				m_bHomeSnr_BinEjCap;		// Bin Ejector Cap Home Sensor
	BOOL				m_bHomeSnr_BinEjT;			// Bin Ejector Theta Home Sensor


	BOOL                m_bContactSensor;
	BOOL                m_bContactSensorZ2;
	BOOL				m_bPosLimitSnr_Z;	// Z Positive Limit Sensor
	BOOL				m_bNegLimitSnr_Z;	// Z Negative Limit Sensor
	BOOL				m_bPosLimitSnr_Z2;	// Z Positive Limit Sensor
	BOOL				m_bNegLimitSnr_Z2;	// Z Negative Limit Sensor
	BOOL				m_bPosLimitSnr_T;	// T Positive Limit Sensor
	BOOL				m_bNegLimitSnr_T;	// T Negative Limit Sensor
	//v4.59A12		//Renesas MS90
	BOOL				m_bBH1PressureSnr;	//New BH pressure sensor for BH1 in MS90 Renesas
	BOOL				m_bBH2PressureSnr;	//New BH pressure sensor for BH2 in MS90 Renesas
	BOOL				m_bEjPressureSnr;	//New EJ pressure sensor for in MS90 Renesas
	BOOL				m_bInputIonizerSnr;
	BOOL				m_bOutputIonizerSnr;
	BOOL				m_bCatchAndBlowSnrZ;
	BOOL				m_bCatchAndBlowSnrY;
	BOOL				m_bCheckEjPressureSnr;	//v4.59A17
	BOOL				m_bCheckBH1PressureSnr;	//v4.59A25
	BOOL				m_bCheckBH2PressureSnr;	//v4.59A25

	BOOL                m_bIsTJoystickOn;
	BOOL                m_bIsZJoystickOn;
	BOOL                m_bIsEJoystickOn;

	BOOL				m_bBHFan;			//MS100Plus

	BOOL				m_bMS90UplookPrDo1MoreCycle;

	//MS60 BHT Thermal Fan sensor bits		//v4.48A10
	BOOL				m_bMS60BHZFan1;
	BOOL				m_bMS60BHZFan2;
	BOOL				m_bMS60BHTUpFan1;
	BOOL				m_bMS60BHTUpFan2;
	BOOL				m_bMS60BHTLowFan1;
	BOOL				m_bMS60BHTLowFan2;

	//v4.24T8	//ES101 NGPick modeule sensors
	BOOL				m_bNGPickDnSnr;
	BOOL				m_bNGPickUpSnr;
	BOOL				m_bTensionCheck;
	BOOL				m_bTapeEmpty;

	// Encoder Count
	LONG				m_lEnc_Z;			
	LONG				m_lEnc_Z2;			
	LONG				m_lEnc_T;			
	LONG				m_lEnc_Ej;
	LONG				m_lEnc_EjElevatorZ;
	LONG				m_lEnc_EjX;
	LONG				m_lEnc_EjY;
	LONG				m_lEnc_EjCap;
	LONG				m_lEnc_EjT;
	LONG				m_lEnc_BinEj;
	LONG				m_lEnc_BinEjElevatorZ;
	LONG				m_lEnc_BinEjCap;
	LONG				m_lEnc_BinEjT;
	LONG				m_lEnc_ChgColletZ;
			
	LONG				m_lCmd_Ej;
	LONG				m_lCmd_EjElevatorZ;
	LONG				m_lCmd_EjCap;
	LONG				m_lCmd_EjT;
	LONG				m_lCmd_BinEj;
	LONG				m_lCmd_BinEjElevatorZ;
	LONG				m_lCmd_BinEjCap;
	LONG				m_lCmd_BinEjT;

	LONG				m_lColletHoleEnc_T;
	LONG				m_lColletHoleEnc_Z;
	LONG				m_lColletHoleEnc_Z2;

// bonding process setup

	//LONG	m_lBPGeneral_0;
	LONG    m_lBPGeneral_1;
	LONG    m_lBPGeneral_2;
	LONG    m_lBPGeneral_3;
	LONG    m_lBPGeneral_4;
	LONG    m_lBPGeneral_5;
	LONG    m_lBPGeneral_6;
	LONG    m_lBPGeneral_7;
	LONG    m_lBPGeneral_8;
	LONG    m_lBPGeneral_9;	//Ejector Pin Up Position
	LONG    m_lBPGeneral_A;	//Ejector Pin Standby Position
	LONG    m_lBPGeneral_B;
	LONG    m_lBPGeneral_C;
	LONG    m_lBPGeneral_D;
	LONG    m_lBPGeneral_E;
	LONG    m_lBPGeneral_F;

	LONG    m_lEjMoveDistance;

	LONG	m_lBPGeneral_G;
	LONG	m_lBPGeneral_H;
	LONG    m_lBPGeneral_TmpA;
	LONG    m_lBPGeneral_TmpB;
	LONG    m_lBPGeneral_TmpC;
	LONG	m_lBPGeneral_TmpD;
	LONG	m_lBPGeneral_TmpE;
	LONG	m_lBPGeneral_BHZ1;
	LONG	m_lBPGeneral_BHZ2;
	LONG	m_lBPGeneral_BHZ3;  // For CP100 ES3
	LONG	m_lBPGeneral_BHZ4;	// For CP100 ES4

//Profile Type
	LONG	m_lT_ProfileType;
	LONG	m_lZ_ProfileType;

	LONG	m_lE_ProfileType;	
	LONG	m_lECap_ProfileType;
	LONG	m_lET_ProfileType;

	LONG	m_lBinE_ProfileType;
	LONG	m_lBinECap_ProfileType;
	LONG	m_lBinET_ProfileType;

//S/W Comm Method
	LONG	m_lT_CommMethod;
	LONG	m_lZ_CommMethod;
	LONG	m_lE_CommMethod;
	LONG	m_lBinE_CommMethod;

	CString m_szBHConfigFileName;

	ULONG	*m_pulStorage;			// Permanent storage

	//Air-flow sensor test
	LONG	m_lTMoveTime;
	LONG	m_lTTravelTime;

	// BH-Z OPEN-DAC Test
	LONG	m_lBHZOpenDacValue;			
	BOOL	m_bEnableOpenDacBondOption;	

	// AUTOBOND sequence variables
	BOOL	m_bFirstCycle;
	BOOL	m_bFirstCycleAlreadyTakeLog;
	BOOL	m_bChangeGrade;				//v4.44A3	//EJTXY
	BOOL	m_bWaitBtNoDelay;			//v2.93T2
	BOOL	m_bMoveFromPrePick;			//v3.79
	DOUBLE	m_dWaitBTStartTime;			//v2.93T2
	BOOL	m_bCleanColletToggle;		//v3.25T2
	INT		m_nSpeedModeCount;
	DOUBLE	m_dEjDnStartTime;			//v4.22T1	//for MS100 sequence
	DOUBLE	m_dBinEjDnStartTime;
	DOUBLE	m_dBHZPickStartTime;			//shiraishi03
	//DBH Config	//v3.66
	BOOL	m_bBHZ2TowardsPick;
	INT		m_nBHAlarmCode;	
	LONG	m_lBHStateCode;
	INT		m_nBHLastAlarmCode;			//v4.15T1
	LONG	m_lPrevDieSPCResult;
	LONG	m_lSPCResult;				//v4.52A2
	BOOL	m_bSPCAlarmTwice;			//v4.51A16	//SanAn XM
	BOOL	m_bSPCAlarmTwiceZ2;			//v4.52A7	//Semitek
	BOOL	m_bPickDownFirst;	
	LONG	m_nBHZ1BHZ2Delay;		//andrew123
	BOOL	m_bCheckPickZ2DownFirst;	//v3.66T1
	BOOL	m_bDBHStop;					//v3.67T2
	LONG	m_lDBHBondHeadMode;			//v3.93			//0=dual-head, 1=BHZ1 only, 2=BHZ2 only
	BOOL	m_bIsES101NGPickStarted;	//v4.24T10		//ES101
	BOOL	m_bIs2DCodeDone;			//v4.40T1
	BOOL	m_bMS100TurnOnVac;			//v4.42T15			
	LONG	m_lUpLookPrStage;
	BOOL	m_bEnableBHUplookPr;
	BOOL	m_bUplookResultFail;		//v4.57A11
	//v4.15T1	//CMLT	//FOr MS100 & MS100Plus only
	ULONG	m_ulLastMapRow;
	ULONG	m_ulLastMapCol;
	UCHAR	m_ucLastMapGrade;
	BOOL	m_bLastMapChecked;

	BOOL	m_bBHZ1HasDie;				//v3.98T3
	BOOL	m_bBHZ2HasDie;				//v3.98T3

	//BHZ Open DAC variables
	BOOL	m_bEnableBHZOpenDac;
	LONG	m_lBHZ1_PickDAC;
	LONG	m_lBHZ1_BondDAC;
	LONG	m_lBHZ2_PickDAC;
	LONG	m_lBHZ2_BondDAC;
	LONG	m_lBHZ1_PickDACOffset;
	LONG	m_lBHZ1_BondDACOffset;
	LONG	m_lBHZ2_PickDACOffset;
	LONG	m_lBHZ2_BondDACOffset;

	LONG	m_lWithBHDown;
	BOOL	m_bChangeEjtX;				//v4.42T2
	BOOL	m_bChangeEjtY;				//v4.42T2

	//Sync Move Via Ej Cmd Dist		//MS100
	BOOL	m_bEnableSyncMotionViaEjCmd;
	LONG	m_lSyncTriggerValue;
	LONG	m_lSyncZOffset;

	LONG	m_lSoftTouchPickDistance;
	LONG	m_lSoftTouchBondDistance;
	LONG	m_lSoftTouchVelocity;

	//Open DAC Calibration 
	LONG	m_lStartOpenDACCalibValue;
	LONG	m_lCurrOpenDACCalibValue;
	LONG	m_lOpenDACCurrBHZ;

	BOOL	m_bIsOpenDACCalibStart;
	BOOL	m_bIsOpenDACForceCheckStart;
	LONG	m_lOpenDACForceCheckValue;

	DOUBLE	m_dBHZ1_OpenDACCalibRatio;
	DOUBLE	m_dBHZ2_OpenDACCalibRatio;

	//DAC/Distance Calibration
	DOUBLE m_dBHZ1_DACDistanceCalibRatio;
	DOUBLE m_dBHZ2_DACDistanceCalibRatio;

	INT m_nHmiResetCollet;

	LONG	m_lPBTZPostiveLimit;
	BOOL	m_bComm_PBTZ;					// PBT Z Communtate flag
	BOOL	m_bHome_PBTZ;					// PBT Z Home flag
	BOOL	m_bSel_PBTZ;	
	BOOL	m_bIsPowerOn_PBTZ;
	
	BOOL	m_bHomeSnr_PBTZ;

	// Prober Sensor part
	LONG	m_lCheckFanCount;


	// Contact Sensor Range
	LONG	m_lStandByLevel_PBTZ;
	LONG	m_lContactLevel_PBTZ;

	// move search during probing.
	ULONG	m_ulProbePinCount;
	ULONG	m_ulProbePinCleanCount;

	BOOL	InitVariablesHWD();
	BOOL	LoadFileHWD();
	BOOL	SaveFileHWD();
	BOOL	MS90RotateBT180(BOOL bTo180);

	BOOL	m_bCycleFirstProbe;

	//Nichia Attributes		//v4.40T1
	UCHAR	m_ucNichiaProcessMode;
	UCHAR	m_ucNichiaMaxNoOfWafers;
	INT		m_nPBDoneWaitCount;

	//New method of datalog
	BOOL	m_bDataLogListInNewMode;

	//The indicator of contact sensor
	LONG	m_lTheTouchedContactSensor;

	//MS60 BH cooling fan
	BOOL	m_bCoolingFanWillTurnOff;
	LONG	m_lBondHeadCoolingSecond;

	//Lumileds Start Lot Inspection
	ULONG   m_ulStartLotCheckCount;


	//MS50 AutoChgCollet module
	LONG	m_lCurPos_ChgColletT;
	LONG	m_lCurPos_ChgColletZ;

	LONG	m_lAGCColletStartAng;
	LONG	m_lAGCColletTPitch;
	LONG	m_lAGCHolderUploadZ;
	LONG	m_lAGCHolderInstallZ;
	LONG	m_lAGCHolderBlowDieZ; //only for clean collet
	LONG	m_lAGCSetupValue;
	LONG	m_lAGCColletIndex;

	//BHMark
	LONG	m_lAutoUpdateBHPickPosnMode;  //0  -- Disable 1 -- Enable BH Mark  2 -- Enable Collet Hole
	BOOL	m_bEnableBHMark_HMI;
	BOOL	m_bEnableAutoUpdateBHPickPosnMode_HMI;
	LONG	m_lCheckBHMarkCycle;
	LONG	m_lCheckBHMarkDelay;

	CString	m_szMHCresult;
	LONG	m_lMHCTime;
	BOOL	m_bIsMHCTrained;
protected:

// bonding process setup function
	VOID	InitVariable(VOID);
	VOID	CheckHeadBondDelay();
	BOOL	LoadBhData(VOID);
	BOOL	LoadBhOption(BOOL bLoadPPKG=FALSE);
	BOOL	SaveBhData(VOID);
	BOOL	GenerateConfigData();									//v3.86
	LONG	GenerateConfigData(IPC_CServiceMessage &svMsg);			//v3.86
	BOOL	GenerateParameterList();
	LONG	GenerateParameterList(IPC_CServiceMessage &svMsg);		//v3.86
	LONG	GeneratePkgDataFile(IPC_CServiceMessage &svMsg);		//v4.21T7	//Walsin China
	LONG	ToggleILCLog(IPC_CServiceMessage &svMsg);
	//LONG	SetupDataLog(VOID);
	//LONG	EnableDataLog(BOOL bLog);

	LONG	SetupDataLogEjector(VOID);
	LONG	EnableDataLogEjector(BOOL bLog);
	LONG	SetupMotionDataLog(Hp_mcb *pModule, short ssChannel);
	LONG	EnableMotionDataLog(BOOL bEnable, Hp_mcb *pModule, short ssChannel);
	LONG	SetupDataLogT(VOID);
	LONG	EnableDataLogT(BOOL bLog);
	LONG	SetupDataLogZ(VOID);			//v2.78T
	LONG	EnableDataLogZ(BOOL bLog);		//v2.78T
	UINT	m_unDataLogOption;				//v3.15T5

	BOOL	NVC_MotionTest(VOID);

	BOOL	ManualCleanColletShortcutFunc();
	BOOL	ManualCleanCollet_PLLM_MS109();	//v4.36T1	//PLLM MS109 Rebel buyoff

// ILC related functions
	BOOL	CycleEnableDataLog(BOOL bEnable, BOOL bDisplay, BOOL bWriteToFile);
	BOOL	m_bEnableNuDataLog;	//v4.50A24

	BOOL	IsEnableILC();

	BOOL	ILC_InitData();
	BOOL	ILC_StructInit(ILC_STRUCT &stILC, LONG lBondHeadState);
	
	VOID	ILC_Clean();

	BOOL	ILC_IsAutoLearnComplete();
	VOID	ILC_ResetAutoLearnComplete();

	BOOL	ILC_IsInitSuccess();

	BOOL	ILC_IsNeedRestartUpdate();

	BOOL	ILC_OpRestartUpdateWarmCool();
	BOOL	ILC_RestartUpdateWarmCoolInitFunc();
	BOOL	ILC_RestartUpdateWarmCoolInit(ILC_STRUCT &stILC);
	BOOL	ILC_RestartUpdateWarmCool();
	BOOL	ILC_SubRestartUpdateWarmCool(ILC_DATALOG_STRUCT *pstPTBWarmDataLog, ILC_DATALOG_STRUCT *pstPTBCoolDataLog,
										 ILC_DATALOG_STRUCT *pstBTPWarmDataLog, ILC_DATALOG_STRUCT *pstBTPCoolDataLog);
	
	BOOL	ILC_OpRestartUpdate(BOOL bNeedUpdate);
	BOOL	ILC_RestartUpdateInitFunc();
	BOOL	ILC_RestartUpdateInit(ILC_STRUCT &stILC, LONG lBondHeadState);
	LONG	ILC_RestartUpdateFunc(BOOL bMoveDirection);
	BOOL	ILC_RestartUpdate();

	BOOL	ILC_AutoUpdateInitFunc();
	LONG	ILC_AutoUpdateInit(ILC_STRUCT &stILC, LONG lBondHeadState);
	BOOL	ILC_AutoUpdateInCycle(LONG lBondHeadState);
	UINT	m_nILCAutoUpdateAccErrCount;		//v4.47T9
	
	BOOL	ILC_AutoUpdateFromDataLog(ILC_STRUCT &stILC, ILC_DATALOG_STRUCT &stBADataLog, LONG& lStatus);	//v4.47T9
	BOOL	ILC_AutoUpdate(LONG lBondHeadState, INT *pPos, INT *pEnc, INT nNoOfSampleUploaded, INT *pChannelMode);

	BOOL	ILC_AutoUpdateApplyingCycleHitLimit();
	VOID	ILC_AutoUpdateApplyingCycleCountInc();

	BOOL	ILC_AutoLearnInitFunc(LONG lBondHeadState);
	BOOL	ILC_AutoLearnInit(ILC_STRUCT &stILC, FLOAT fDistance);
	LONG	ILC_AutoLearnFunc();
	
	LONG	ILC_LearnWarmCoolMove(ILC_STRUCT &stILC, ILC_DATALOG_STRUCT *pstDataLog, LONG lMode);
	LONG	ILC_LearnMove(ILC_STRUCT &stILC, BOOL bIsConverged);
	
	BOOL	ILC_LoadParameters(ILC_STRUCT &stILC, LONG lBondHeadState);
	VOID	ILC_SaveAllResult();

	VOID	ILC_FreeStructMemory(ILC_STRUCT &stILC);

	BOOL	ILC_IsHitDataLogLimit(LONG lBondHeadState);
	BOOL	ILC_RunTimeDataLog(LONG lBondHeadState, BOOL bEnable);
	BOOL	ILC_LogRunTimeUpdateData(LONG lBondHeadState);
	VOID	ILC_CleanDataLog();

	BOOL	ILC_ContourMoveTest();

	BOOL	ILC_UpdateContourMoveProfile();
	BOOL	ILC_RunTimeUpdateContourMoveProfile(LONG lBondHeadUpdateState);
	LONG	ILC_GetContourMoveStartOffset(LONG lBondHeadState);

	VOID	ILC_LogStructData(ILC_STRUCT &stILC);

	BOOL	ILC_LogRunningContourPoints(FLOAT *fPos, FLOAT *fVel, FLOAT *fAcc, 
										FLOAT *fJerk, UINT uiNoofPoint);

	BOOL	ILC_CalculateApplyLength(ILC_STRUCT& stILC, LONG lBondHeadState);		//v4.47T3	//MS60


	LONG	BH_UpdateAction(IPC_CServiceMessage &svMsg);
	LONG	UpdateAction(IPC_CServiceMessage &svMsg);
	LONG	LogItems(IPC_CServiceMessage &svMsg);
	LONG	ToggleLearnPadsLevel(IPC_CServiceMessage &svMsg);

	LONG	BH_BlockButtonCmd(IPC_CServiceMessage &svMsg);
	LONG	BH_ChangeArmPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_KeyInArmPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MovePosArmPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveNegArmPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_ConfirmBondArmSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_CancelBondArmSetup(IPC_CServiceMessage &svMsg);

	LONG	BH_EjtMoveTo(IPC_CServiceMessage &svMsg);
	LONG	BH_AutoLearnHeadPosn(IPC_CServiceMessage &svMsg);
	LONG	BH_ChangeHeadPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_KeyInHeadPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MovePosHeadPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveNegHeadPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_ConfirmBondHeadSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_CancelBondHeadSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_UpdateSwingOffset(IPC_CServiceMessage &svMsg);
	LONG	BH_AutoLearnBHZ1BondLevels(IPC_CServiceMessage &svMsg);			//v3.94T5
	LONG	BH_AutoLearnBHZ1BondLevels_BT2(IPC_CServiceMessage &svMsg);		//v4.34T1
	BOOL	UserSearchDieToPrCenter(BOOL bBHZ2=FALSE);						//v4.44T2
	LONG	BH_AutoLearnLevels(IPC_CServiceMessage &svMsg);
	LONG	BH_UpdateRegistry(IPC_CServiceMessage &svMsg);
	LONG	BH_TnZToPick1(IPC_CServiceMessage &svMsg);
	LONG	BH_TnZToPick2(IPC_CServiceMessage &svMsg);

	LONG	BH_AutoLearnHeadPosnZ2(IPC_CServiceMessage &svMsg);
	LONG	BH_ChangeHeadPositionZ2(IPC_CServiceMessage &svMsg);
	LONG	BH_KeyInHeadPositionZ2(IPC_CServiceMessage &svMsg);
	LONG	BH_MovePosHeadPositionZ2(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveNegHeadPositionZ2(IPC_CServiceMessage &svMsg);
	LONG	BH_ConfirmBondHeadSetupZ2(IPC_CServiceMessage &svMsg);
	LONG	BH_CancelBondHeadSetupZ2(IPC_CServiceMessage &svMsg);
	LONG	BH_UpdateSwingOffsetZ2(IPC_CServiceMessage &svMsg);
	LONG	BH_AutoLearnBHZ2BondLevels(IPC_CServiceMessage &svMsg);			//v3.94T5
	LONG	BH_AutoLearnBHZ2BondLevels_BT2(IPC_CServiceMessage &svMsg);		//v4.34T1

	LONG	BH_AutoLearnBT2OffsetZ(IPC_CServiceMessage &svMsg);			//MS100 9Inch dual table config		//v4.21		

	LONG	BH_AutoLearnEjectorPosn(IPC_CServiceMessage &svMsg);
	LONG	BH_AutoLearnEjUpLevel(IPC_CServiceMessage &svMsg);				//v4.48A3
	LONG	BH_ChangeEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_ChangeEjtCollet1(IPC_CServiceMessage &svMsg);				//Move collet
	LONG	BH_ChangeEjtCollet2(IPC_CServiceMessage &svMsg);				//Move collet
	LONG	BH_Ejt_Z_Move_Up(IPC_CServiceMessage &svMsg);					//v4.43T5
	LONG	BH_Ejt_Z_Move_Down(IPC_CServiceMessage &svMsg);					//v4.43T5
	LONG	BH_EjtCollet_Move_Up(IPC_CServiceMessage &svMsg);
	LONG	BH_EjtCollet_Move_Down(IPC_CServiceMessage &svMsg);
	LONG	BH_EjtCollet_Move_Left(IPC_CServiceMessage &svMsg);
	LONG	BH_EjtCollet_Move_Right(IPC_CServiceMessage &svMsg);
	LONG	BH_ChangeEjectorX(IPC_CServiceMessage &svMsg);					//v4.42T2
	LONG	BH_ChangeEjectorY(IPC_CServiceMessage &svMsg);					//v4.42T2
	LONG	BH_KeyInEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MovePosEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveNegEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MovePosEjectorPositionY(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveNegEjectorPositionY(IPC_CServiceMessage &svMsg);
	LONG	BH_ConfirmEjectorSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_CancelEjectorSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_SyncArmColletVariable(IPC_CServiceMessage &svMsg);

	//NVC Wafer Ejector
	LONG	BH_ChangeNVCEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_KeyInNVCEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveNVCEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_CancelNVCEjectorSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_ConfirmNVCEjectorSetup(IPC_CServiceMessage &svMsg);

	//NVC Bin Ejector
	LONG	BH_ChangeNVCBinEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_KeyInNVCBinEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveNVCBinEjectorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_CancelNVCBinEjectorSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_ConfirmNVCBinEjectorSetup(IPC_CServiceMessage &svMsg);

	LONG	BH_ConfirmSetup(IPC_CServiceMessage &svMsg);
	//1.08S
	LONG	BH_ConfirmMaxColletCountSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_ConfirmMaxCleanCount(IPC_CServiceMessage &svMsg);
	LONG    BH_ConfirmMaxCollet2CountSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_ConfirmMaxEjectorCountSetup(IPC_CServiceMessage &svMsg);

	LONG	BH_ChangeEjElevatorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_KeyInEjElevatorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MovePosEjElevatorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveNegEjElevatorPosition(IPC_CServiceMessage &svMsg);
	LONG	BH_ConfirmEjElevatorSetup(IPC_CServiceMessage &svMsg);
	LONG	BH_CancelEjElevatorSetup(IPC_CServiceMessage &svMsg);

	LONG	BH_MoveEjectorStandbyPosition(IPC_CServiceMessage &svMsg);

	LONG	BH_ColletVacuum(IPC_CServiceMessage &svMsg);
	LONG	BH_ColletVacuumZ2(IPC_CServiceMessage &svMsg);
	LONG	BH_StrongBlow(IPC_CServiceMessage &svMsg);
	LONG	BH_StrongBlowZ2(IPC_CServiceMessage &svMsg);

	LONG	BH_HomeEjector(IPC_CServiceMessage &svMsg);
	LONG	BH_HomeEjElevator(IPC_CServiceMessage &svMsg);
	LONG	BH_HomeBondHead(IPC_CServiceMessage &svMsg);
	LONG	BH_HomeBondArm(IPC_CServiceMessage &svMsg);
	LONG	BH_ResetAll(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveToBlow(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveToPrePick(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveToPick(IPC_CServiceMessage &svMsg);
	LONG	BH_MoveToPickLevel(IPC_CServiceMessage &svMsg);		//v3.80
	LONG	BH_MoveEjToUpLevel(IPC_CServiceMessage &svMsg);		//v4.08
	LONG	BH_MoveBHToPick1(IPC_CServiceMessage &svMsg);		//v3.34
	LONG	BH_MoveBHToPick(IPC_CServiceMessage &svMsg);		//v3.34
	BOOL	BH_SubMoveBHToPick(const BOOL bPick, const LONG lOffsetT);
	LONG	BH_MoveToBond(IPC_CServiceMessage &svMsg);			//v2.60
	LONG	BH_MoveBHToPick_Z2(IPC_CServiceMessage &svMsg);		//Dual Arm learn collet
	LONG	BH_MoveToBond_Z2(IPC_CServiceMessage &svMsg);		//Dual Arm learn collet
	LONG	BH_AutoMoveToPrePick(IPC_CServiceMessage &svMsg);
	LONG	BH_EnableThermalControlCmd(IPC_CServiceMessage &vsMsg);
	LONG	BH_UpdateThermalTemperature(IPC_CServiceMessage &vsMsg);

	LONG	GetCommandPosition(IPC_CServiceMessage &svMsg);
	
	LONG	m_lAtPickPosnT;
	LONG	m_lAtPickPosnEj;
	LONG	m_lAtPickPosnZ;
	LONG	m_lAtPickPosnZ2;

	LONG	GetChannelTravelLimit(IPC_CServiceMessage &svMsg);

	// Reset the Collet and Ejector count
	LONG	BH_ResetColletCountDiag(IPC_CServiceMessage &svMsg);
	LONG	BH_ResetCollet1CountDiag(IPC_CServiceMessage &svMsg);	//v4.53A25	//Semitek
	LONG	BH_ResetCollet2CountDiag(IPC_CServiceMessage &svMsg);	//v4.53A25	//Semitek
	LONG	BH_ResetColletCount(IPC_CServiceMessage &svMsg);
	LONG	BH_ResetCollet2Count(IPC_CServiceMessage &svMsg);
	LONG	BH_ResetEjectorCountDiag(IPC_CServiceMessage &svMsg);
	LONG	BH_ResetEjectorCount(IPC_CServiceMessage &svMsg);
	LONG	BH_ResetAllDieCount(IPC_CServiceMessage &svMsg);
	LONG	BH_ResetAllWaferDieCount(IPC_CServiceMessage &svMsg);
	LONG	BH_ResetCustomCounter(IPC_CServiceMessage &svMsg);		//v3.68T3
	LONG	BH_ResetEjKOffsetCount(IPC_CServiceMessage &svMsg);		//v4.46T15
	LONG	BH_ResetBHZKOffsetCount(IPC_CServiceMessage &svMsg);	//v4.53A25

	// Display wafer information
	LONG	BH_ShowWaferInformation(IPC_CServiceMessage &svMsg);

	// Update current wafer yield
	LONG	BH_UpdateCurrentWaferYield(IPC_CServiceMessage &svMsg);

	// Clean Collet
	LONG	BH_CleanCollet(IPC_CServiceMessage &svMsg);

	// Home moudle motor
	LONG	BH_HomeMotorSelection(IPC_CServiceMessage &svMsg);

	// Check Bondarm safe pos
	LONG	BH_IsBondArmSafe(IPC_CServiceMessage &svMsg);

	// On/Off Front & Side Cover lock
	LONG	BH_FrontCoverLock(IPC_CServiceMessage &svMsg);
	LONG	BH_SideCoverLock(IPC_CServiceMessage &svMsg);
	LONG	BH_BinElvatorCoverLock(IPC_CServiceMessage &svMsg);
	// for binloader
	LONG	FrontCoverLock(IPC_CServiceMessage &svMsg);
	LONG	SideCoverLock(IPC_CServiceMessage &svMsg);
	LONG	BinElevatorCoverLock(IPC_CServiceMessage &svMsg);
	
	//v4.24T10	//ES101 SIO fcns
	LONG	NGPickUpDn(IPC_CServiceMessage &svMsg);
	LONG	NGPick_IndexReelT(IPC_CServiceMessage &svMsg);
	LONG	NGPick_HomeReelT(IPC_CServiceMessage &svMsg);
	LONG	NGPickReelStop(IPC_CServiceMessage &svMsg);
	LONG	NGPickReloadReel(IPC_CServiceMessage &svMsg);			//v4.27
	LONG	StartEjElevator(IPC_CServiceMessage &svMsg);			//v4.24T10
	BOOL	MoveProberDownAndUp();									//v4.37T10	//Prober
	LONG	MoveEjtElvtToUpDown(IPC_CServiceMessage &svMsg);
	LONG	MoveEjeElvtToSafeLevel(IPC_CServiceMessage &svMsg);
	LONG	MoveEjectorElevatorRoutine(IPC_CServiceMessage &svMsg);
	//v4.42T3	//EJT XY fcns
	LONG	GetEjtXYEncoderCmd(IPC_CServiceMessage &svMsg);
	LONG	EjtXY_MoveToCmd(IPC_CServiceMessage &svMsg);
	LONG	EjtXY_MoveToColletOffset(IPC_CServiceMessage &svMsg);
	LONG	EjtXY_Setup(IPC_CServiceMessage &svMsg);
	LONG	EjtXY_SearchBHColletHoles(IPC_CServiceMessage &svMsg);	//v4.48A1

	// Auto Clean Collet
	VOID	CleanColletPocketAlcoholOnce();
	LONG	BHSyringeOnce(IPC_CServiceMessage &svMsg);				//v3.65		//Lexter
	LONG	SetColletCleanPocket(IPC_CServiceMessage &svMsg);		//v3.65		//Lexter
	LONG	BHAccTest(IPC_CServiceMessage &svMsg);
	LONG	BH_ACCMoveToPosnT(IPC_CServiceMessage &svMsg);
	LONG	BH_UpdateACCData(IPC_CServiceMessage &svMsg);
	BOOL	IsReachACCMaxCount();
	BOOL	IsReachACCMaxTimeOut();

	LONG	BH_SetSuckingHead(IPC_CServiceMessage &svMsg);			//v4.05

	// Diagnostic Functions
	LONG Diag_PowerOn_T(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_Z(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_Z2(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_Ej(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_EjCap(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_EjT(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_EjX(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_EjY(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_EjElevator(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_BinEj(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_BinEjCap(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_BinEjT(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_BinEjElevator(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_ChgColletT(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_ChgColletZ(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_All(IPC_CServiceMessage &svMsg);

	LONG Diag_Comm_T(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_Z(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_Z2(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_Ej(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_EjT(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_EjCap(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_BinEj(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_BinEjT(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_BinEjCap(IPC_CServiceMessage &svMsg);

	LONG Diag_Home_T(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_Z(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_Z2(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_Ej(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_EjT(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_EjX(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_EjY(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_EjCap(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_EjElevator(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_BinEj(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_BinEjT(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_BinEjCap(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_BinEjElevator(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_MS50ChgColletT(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_MS50ChgColletZ(IPC_CServiceMessage &svMsg);

	LONG Diag_Move_T(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_Z(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_Z2(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_Ej(IPC_CServiceMessage &svMsg);
	LONG Diag_NegMove_Ej(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_EjT(IPC_CServiceMessage &svMsg);
	LONG Diag_NegMove_EjT(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_EjX(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_EjY(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_EjCap(IPC_CServiceMessage &svMsg);
	LONG Diag_NegMove_EjCap(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_EjElevator(IPC_CServiceMessage &svMsg);
	LONG Diag_NegMove_EjElevator(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_BinEj(IPC_CServiceMessage &svMsg);
	LONG Diag_NegMove_BinEj(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_BinEjT(IPC_CServiceMessage &svMsg);
	LONG Diag_NegMove_BinEjT(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_BinEjCap(IPC_CServiceMessage &svMsg);
	LONG Diag_NegMove_BinEjCap(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_BinEjElevator(IPC_CServiceMessage &svMsg);
	LONG Diag_NegMove_BinEjElevator(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_MS50ChgColletT(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_MS50ChgColletZ(IPC_CServiceMessage &svMsg);

	LONG UpdateOutput(IPC_CServiceMessage &svMsg);

	//andrew
	//BH Open DAC test
	LONG CheckPreBondStatus(IPC_CServiceMessage &svMsg);
	LONG CheckPreBondChecking(IPC_CServiceMessage &svMsg);

	LONG StartTDacTest(IPC_CServiceMessage &svMsg);			//v2.71
	LONG StopTDacTest(IPC_CServiceMessage &svMsg);			//v2.71

	LONG StartAirFlowSensorTest(IPC_CServiceMessage &svMsg);
	LONG UpdateDieCounterCmd(IPC_CServiceMessage &svMsg);

	LONG ContactSearchTest(IPC_CServiceMessage &svMsg);
	LONG OpenDacTest(IPC_CServiceMessage &svMsg);

	LONG EjTest(IPC_CServiceMessage &svMsg);
	LONG ZTest(IPC_CServiceMessage &svMsg);
	LONG TTest(IPC_CServiceMessage &svMsg);

	LONG StartStaticThermalTest(IPC_CServiceMessage &svMsg);
	LONG RestoreStaticThermalTest(IPC_CServiceMessage &svMsg);

	LONG BondHeadTiltAngleTest(IPC_CServiceMessage &svMsg);
	LONG StartUBHZTest(IPC_CServiceMessage &svMsg);
	LONG StopUBHZTest(IPC_CServiceMessage &svMsg);

	LONG CheckStartThermalTest(IPC_CServiceMessage &svMsg);
	LONG StaticMotionLog(IPC_CServiceMessage &svMsg);
	LONG CheckExArmInSafePos(IPC_CServiceMessage &svMsg);

	//Open DAC Calibration (for MS100)
	LONG OpenDACCalibrationCmd(IPC_CServiceMessage &svMsg);
	LONG OpenDACForceCheckCmd(IPC_CServiceMessage &svMsg);
	LONG ResetOpenDACCalibRatio(IPC_CServiceMessage &svMsg);

	//DAC Force Calibration
	LONG DACDistanceCalibrationCmd(IPC_CServiceMessage &svMsg);
	LONG ResetDACDistanceCalibRatio(IPC_CServiceMessage &svMsg);
	LONG CheckMDResponseTime(IPC_CServiceMessage &svMsg);	//v3.94
	// Axis Test commands
	LONG StartBHZ1MotionTest(IPC_CServiceMessage &svMsg);
	LONG StartBHZ2MotionTest(IPC_CServiceMessage &svMsg);
	LONG StartBArmMotionTest(IPC_CServiceMessage &svMsg);
	LONG StartEjMotionTest(IPC_CServiceMessage &svMsg);
	LONG StartPBTZMotionTest(IPC_CServiceMessage &svMsg);
	// ILC related commands
	LONG ILC_AutoLearnCmd(IPC_CServiceMessage &svMsg);
	LONG ILC_RestartUpdateCmd(IPC_CServiceMessage &svMsg);
	LONG ILC_BondArmPreStartCheck(IPC_CServiceMessage &svMsg);
	
	// Prober commands
	LONG Test(IPC_CServiceMessage &svMsg);

	// Check the ejector lifetime					//v4.44T4   sanan
	LONG CheckTheEjectorLifeTime(IPC_CServiceMessage &svMsg);
	// Check the ejector tolerance level			//v4.44T4   sanan
	BOOL CheckBondHead1ToleranceLevel(CONST LONG lCurrLrnLevel);
	BOOL CheckBondHead2ToleranceLevel(CONST LONG lCurrLrnLevel);
	BOOL CheckZ1PickBondLevelOffset();
	BOOL CheckZ2PickBondLevelOffset();
	BOOL CheckNewOldLevelOffset(LONG lOffset);

	LONG	BH_DetectBinTableTilting(IPC_CServiceMessage &svMsg);
	LONG	BH_MBL_GetBondLevel(IPC_CServiceMessage &stMsg);
	LONG	SetBTLevelSampleArea(IPC_CServiceMessage& svMsg);
	LONG	SetBondPadsArea(IPC_CServiceMessage& svMsg);
	LONG	BT_XY_MoveTo(LONG lX, LONG lY);
	LONG	BT_Set_JoyStick(BOOL bEnable);
	LONG	BT_Get_XY_Posn(LONG &lBTX, LONG &lBTY);

	LONG	ReadDigitalAirFlowSensor(IPC_CServiceMessage &svMsg);	//v4.43T8
	BOOL	CheckDigitalAirFlowThresholdLimit(BOOL bIsBHZ2=FALSE);

	LONG	AutoLearnBHZHomeOffset(IPC_CServiceMessage &svMsg);		//v4.46T26	//Cree HuiZhou PkgSort
	
	LONG	SetBondHeadFanCmd(IPC_CServiceMessage &svMsg);				//v4.47T2
	VOID	ResetBondHeadFanTimerAndCounter(BOOL bOn);							//v4.47T3

	LONG	CheckCoverAlarm(IPC_CServiceMessage &svMsg);
	LONG	EjSoftTouch(IPC_CServiceMessage &svMsg);	
	BOOL	EjSoftTouch_CMD();

	LONG	HC_UpdateISValue(IPC_CServiceMessage &svMsg);
	LONG	HC_UpdatePIDValue(IPC_CServiceMessage &svMsg);			//v4.54A1

	//Auto Change Collet Fcn	//v4.50A6
	VOID	RegisterAGCCommand();
	LONG	AGC_SetIndex(IPC_CServiceMessage& svMsg);				//v4.50A8
	LONG	AGC_SetUplookPRLevelOffset(IPC_CServiceMessage &svMsg);
	LONG	AGC_SetClamp(IPC_CServiceMessage &svMsg);
	LONG	AGC_SetPusher(IPC_CServiceMessage &svMsg);
	LONG	AGC_SetUploadPusher(IPC_CServiceMessage &svMsg);
	LONG	AGC_SetHolderVacuum(IPC_CServiceMessage &svMsg);
	LONG	AGC_SetHolderStrongBlow(IPC_CServiceMessage &svMsg);
	LONG	AGC_DetachCollet(IPC_CServiceMessage &svMsg);
	LONG	AGC_UploadCollet(IPC_CServiceMessage &svMsg);
	LONG	AGC_InstallCollet(IPC_CServiceMessage &svMsg);
	LONG	AGC_SearchUploadCollet(IPC_CServiceMessage &svMsg);
	VOID	DisplayError(LONG lErr);
	LONG	AGC_SearchCollet(IPC_CServiceMessage &svMsg);
	LONG	AGC_MoveCollet(IPC_CServiceMessage &svMsg);			//v4.50A30
	BOOL	AGC_SubMoveCollet(const BOOL bMove);
	LONG	AGC_ResetColletOffset(IPC_CServiceMessage &svMsg);	//v4.50A30
	LONG	AGC_AutoChangeCollet(IPC_CServiceMessage &svMsg);
	VOID	ResetColletAndGenRecord(BOOL bCollet2, BOOL bAuto=FALSE);
	VOID	ResetColletRecord();
	VOID	ResetEjectorRecord();

	LONG	ManualPickDieToUpLookPR(IPC_CServiceMessage &svMsg);	//andrewng6
	LONG	BH_BondLeftDie(IPC_CServiceMessage &svMsg);
	LONG	LoopTestBondArm(IPC_CServiceMessage &svMsg);

	LONG	SearchColletHoleTest(IPC_CServiceMessage &svMsg);
	LONG	SearchBondColletEpoxyCmd(IPC_CServiceMessage &svMsg);
	VOID	RunDacTest();

	LONG	GetWTEncoderValue(LONG *lX, LONG *lY, LONG *lT);
	BOOL	CleanColletCmd(IPC_CServiceMessage &svMsg);
	
	LONG	SetDefaultKValue(IPC_CServiceMessage &svMsg);
	LONG	MoveContourProfileLearnCollet(IPC_CServiceMessage &svMsg);
	LONG	MHCTesting(IPC_CServiceMessage &svMsg);

private:	
	
	// Flag to indicate whether the motion is completed
	BOOL	m_bComplete_T;
	//LONG	m_lMotionThetaStartTime;
	BOOL	m_bStartContour_T;
	//LONG	m_lContourMoveMode;
	//LONG	m_lContourMoveStartTime;
	BOOL	m_bComplete_Z;
	BOOL	m_bComplete_Z2;
	BOOL	m_bComplete_Ej;
	BOOL	m_bComplete_EjX;
	BOOL	m_bComplete_EjY;
	BOOL	m_bComplete_BinEj;

	LONG	m_lMinEjectOffset;		//Linear Ej

private:	
	DOUBLE  m_dStartTime; 
	//backup valiable
	BOOL	m_bCheckMissingDie_Backup;
	BOOL	m_bCheckColletJam_Backup;

	LONG	m_lBondCount;
	BOOL	m_bCheckColletJam;		// Flag to select whether collet jam is checking
	BOOL	m_bCheckMissingDie;		// Flag to select whether missing die is checking
	BOOL	m_bPickVacuumOn;		// Flag to indicate whether the Pick Vacuum is on
	BOOL	m_bPickVacuumOnZ2;
	BOOL	m_bEjectorCapVacuumOn;
	BOOL	m_bScanCheckCoverSensor;
	BOOL	m_bCheckCoverSensor;	
	//Moved to MS896aStn class	//v4.36
	//BOOL	m_bEnableFrontCoverLock;
	//BOOL	m_bEnableSideCoverLock;
	//BOOL	m_bEnableBinElevatorCoverLock;
	BOOL	m_bFrontCoverLock;
	BOOL	m_bSideCoverLock;
	BOOL	m_bBinElevatorCoverLock;
	BOOL	m_bUseDefaultDelays;		//v2.97T1
	BOOL	m_bIsLinearEjector;			//Linear Ej
	BOOL	m_bAutoCleanCollet;			//v3.64		//New fcn for Lexter
	BOOL	m_bACCSearchColletHole;		//v4.53A23
	BOOL	m_bNeutralVacState;			//v3.81
	LONG	m_lLiquidExpireTime;
	CTime	m_tSqueezeLiquidTime;

	//PLLM MS109 BH 2D code checking at die bottom
	BOOL	m_bCheckPr2DCode;			//v4.40T1	
	LONG	m_lPr2DCodeCheckLimit;		//v4.41T3
	LONG	m_lPr2DCodeCheckCounter;	//v4.41T3

	LONG	m_bPostBondAtBondCount;		//v2.61
	LONG	m_lPreBondAtPickCount;		//v3.34
	LONG	m_lEMOCheckCounter;			//v3.91
	BOOL	m_bIsEMOTriggered;			//v4.24
	LONG	m_lMS60TempCheckCounter;	//v4.50A6
	BOOL	m_bEnableMDResponseChecking;	//v3.94

	BOOL	m_bBhTDacForwardMove;
	BOOL	m_bStartDacTTest;
	//BOOL	m_bStartAirFlowSensorTest;
	LONG	m_lBondHeadDelay;
	CString	m_szSqOfDacFromPickToBond;
	CString	m_szSqOfDacFromBondToPick;
	DOUBLE	m_dSqOfDacFromBondToPick;
	DOUBLE	m_dSqOfDacFromPickToBond;
	CString	m_szSumOfDac;

	//v3.94T4	//EJ auto-Learn Up-Level fcn
	LONG	m_lWafDieHeight;	
	LONG	m_lEjCTLevelOffset;		//v3.97
	BOOL	m_bRefreshScreen;		
	BOOL	m_bEnableStepMove;
	BOOL	m_bUBHContactSearch;
	BOOL	m_bStartUBHZTest;
	LONG	m_lUBHMoveTime_Z;
	LONG	m_lUBHEnc_Z;

	LONG	m_lUBHPickLevelDac_Z;
	LONG	m_lUBHOpenDacStepSize_Z;
	LONG	m_lUBHOpenDacMax_Z;
	LONG	m_lUBHOpenDacOffset_Z;
	
	LONG	m_lUBHSearchSpeed_Z;
	LONG	m_lUBHSearchDriveInSpeed_Z;
	LONG	m_lUBHSearchDriveInSample_Z;
	LONG	m_lUBHMinPosErrTriggerSample_Z;
	LONG	m_lUBHSearchTriggerPosErr;

	LONG	m_lUBHStepOffset_Z;
	LONG	m_lUBHSettlingTime_T;
	BOOL	m_bEnableSeparteSettlingBlk_T;

	LONG	m_lPreEjectorUpDelay;

	//LONG	m_lEjectorDownDelay;

	Hp_Delay_input m_hpBHDelayInput_T;
	Hp_Delay_input m_hpBHDelayInput_Z;

	Hp_StepMove_input m_hpStepMoveInput_Z;

	Hp_Ki_control m_hpKiControl_Z;
	Hp_PD_control m_hpPDControl_Z;

	CTime	m_ctBondStartTime;
	BOOL	m_bEnableThermalTest;
	LONG	m_lCollectHoleCatpureTime;
	BOOL	m_bThermalTestFirstCycle;

	LONG	m_lPositionErrLimit_Z;

	//Motion Log
	BOOL	m_bEnableMotionLogT;
	BOOL	m_bEnableMotionLogZ;
	BOOL	m_bEnableMotionLogZ2;
	BOOL	m_bEnableMotionLogEJ;
	BOOL	m_bEnableMotionLogEJTX;
	BOOL	m_bEnableMotionLogEJTY;

	BOOL	m_bIsDataLogForBHDebug;

	//BOOL	m_bMotionCycleStop;
	//BOOL	m_bMoveDirection;
	//LONG	m_lMotionTestDelay;
	//UCHAR	m_ucMotionAxis;		//0=NA, 1=Ej, 2=EjT, 3=EjCap, 4=EjElev, 5=BEj, 6=BEjT, 7=BEjCap, 8=BEjElev	
	//LONG	m_lMotionOrigEncPosn;
	//LONG	m_lMotionTarEncPosn;
	//LONG	m_lMotionCycle;
	//LONG	m_lMotionCycleCount;

	//LONG	m_lMotionTestDist;
	//BOOL	m_bILCContourMoveTest;
	//BOOL	m_bIsMotionTestBondHeadZ1;
	//BOOL	m_bIsMotionTestBondHeadZ2;
	//BOOL	m_bIsMotionTestPBTZ;
	//BOOL	m_bIsMotionTestBondArm;
	//BOOL	m_bIsMotionTestEjector;

	BOOL	m_bJustRunTimeUpdate;
	BOOL	m_bTesterTimeOut;


	// ILC variable define
	BOOL	m_bILCFirstTimeStartup;
	LONG	m_lRestartUpdateMinTime;
	CTime	m_ctILCBondingLastUpdateTime;
	LONG	m_lAutoLearnLevelsIdleTime;
	CTime	m_ctBHTAutoCycleLastMoveTime;

	ILC_STRUCT m_stBAILCPTB;
	ILC_STRUCT m_stBAILCBTP;
	
	ILC_MOVE_STRUCT m_BAMove;
	ILC_DATALOG_STRUCT m_BADataLog;
	
	BOOL	m_bILCAutoLearn;
	LONG	m_lILCAutoLearnDelay;
	LONG	m_lILCAutoLearnState;

	//v4.50A9
	BOOL	m_bSetEjectReadyDone;
	UCHAR	m_ucAtPickDieGrade;
	ULONG	m_ulAtPickDieRow;
	ULONG	m_ulAtPickDieCol;

	BOOL	m_bUseMultiProbeLevel;
	PB_CMultiProbeLevelInfo	m_stM_B_L1;
	PB_CMultiProbeLevelInfo	m_stM_B_L2;
	LONG	m_lMBL_UL_X;
	LONG	m_lMBL_UL_Y;
	LONG	m_lMBL_LR_X;
	LONG	m_lMBL_LR_Y;
	CColletRecord m_oColletRecord;
	CEjectorRecord m_oEjectorRecord;
	CBondHeadDataBlock m_oBondHeadDataBlock;
	LONG m_lBondDieCount;
//	BOOL m_bChangeColletDone;

	AD_CBHAirFlowCompensation *m_pBHZ1AirFlowCompensation;
	AD_CBHAirFlowCompensation *m_pBHZ2AirFlowCompensation;
	BOOL m_bILCLog;
};
