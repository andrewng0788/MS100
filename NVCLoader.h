/////////////////////////////////////////////////////////////////
// NVCLoader.h : interface of the CNVCLoader class
//
//	Description:
//		
//	Date:		23 June 2020
//	Revision:	1.00
//
//	By:	Andrew Ng		
//				
//	Copyright @ ASM Pacific Technology Ltd., .
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////
#pragma once

#include "MS896A.h"
#include "MS896AStn.h"
#include "SfmSerialPort.h"
#include "LogFileUtil.h"
#include "resource.h"
#include "NL_Constant.h"


class CNVCLoader : public CMS896AStn
{
	DECLARE_DYNCREATE(CNVCLoader)

public:

	CNVCLoader();
	virtual	~CNVCLoader();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();
	virtual BOOL InitData();			// Init the data from SMF and NVRAM
	virtual VOID ClearData();			// Clear data for cold-start
	virtual VOID FlushMessage();		//Flush IPC Message	
	virtual VOID UpdateStationData();

	VOID Operation();


protected:

	////////////////////////////////////////////////////////
	// IDLE Mode Setup Display Functions
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	virtual VOID RegisterVariables();
	
	////////////////////////////////////////////////////////
	// Main State Sequence Functions
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
	VOID GetAxisInformation();

	virtual VOID UpdateAllSGVariables();

	// Operation state
	enum {	WAIT_IDLE_Q,			// = 0
			WAIT_LOADUNLOAD_Q,		// = 1
			PRELOAD_I_FRAME_Q,		// = 2
			UNLOAD_I_FRAME_Q,		// = 3
			LOAD_I_FRAME_Q,			// = 4

			PRELOAD_O_FRAME_Q,		// = 
			UNLOAD_O_FRAME_Q,
			LOAD_O_FRAME_Q,
			BINFULL_O_FRAME_Q,		// = 
			CHANGE_O_FRAME_Q,		// = 

			HOUSE_KEEPING_Q
	};


	////////////////////////////////////////////////////////
	// Main Motor Related Functions
	INT X_Home();
	INT Y_Home();
	INT Z1_Home();
	INT Z2_Home();

	BOOL X_IsPowerOn();
	BOOL Y_IsPowerOn();
	BOOL Z1_IsPowerOn();
	BOOL Z2_IsPowerOn();

	INT X_PowerOn(BOOL bOn = TRUE);
	INT Y_PowerOn(BOOL bOn = TRUE);
	INT Z1_PowerOn(BOOL bOn = TRUE);
	INT Z2_PowerOn(BOOL bOn = TRUE);

	INT X_Profile(INT nProfile = NL_NORMAL_PROF);		
	INT Y_Profile(INT nProfile = NL_NORMAL_PROF);		
	INT Z1_Profile(INT nProfile = NL_NORMAL_PROF);		
	INT Z2_Profile(INT nProfile = NL_NORMAL_PROF);		

	INT X_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Y_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Z1_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Z2_Move(INT nPos, INT nMode = SFM_WAIT);

	INT X_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Y_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Z1_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Z2_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT XY_MoveTo(INT nPosX, INT nPosY, INT nMode = SFM_WAIT);

	INT	X_Sync();
	INT	Y_Sync();
	INT	Z1_Sync();
	INT	Z2_Sync();


private:

	// Sub-State Functions (BL_SubState.cpp)
	//BOOL IsMotionHardwareReady();		// Check whether the motion hardware are ready
	VOID InitVariable(VOID);
	INT OpInitialize();
	INT OpPreStart();

	BOOL LoadData(VOID);
	BOOL SaveData(VOID);
	BOOL HomeNLModule();

	BOOL IsAllMotorsEnable();
	BOOL InitMotorSwLimits();
	BOOL IsWithinXLimit(CONST LONG lPosX);
	BOOL IsWithinYLimit(CONST LONG lPosY);
	BOOL IsWithinZ1Limit(CONST LONG lPosZ);
	BOOL IsWithinZ2Limit(CONST LONG lPosZ);
	BOOL IsWithinXYLimit(CONST LONG lPosX, CONST LONG lPosY);

	BOOL CheckXEncCmdPos();
	BOOL CheckYEncCmdPos();

	////////////////////////////////////////////////////////
	// Station Event Functions	
	// Set Event
	VOID SetNLReady(BOOL bState = TRUE);
	VOID SetNLOperate(BOOL bState = TRUE);
	VOID SetNLPreOperate(BOOL bState = TRUE);
	// Wait Event
	BOOL WaitNLReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitNLOperate(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitNLPreOperate(INT nTimeout = LOCK_TIMEOUT);

	////////////////////////////////////////////////////////
	// Main Wafer LOAD UNLOAD Functions
	BOOL OpLoadWaferFrame(LONG lLoadBlock);
	BOOL OpUnloadLastWaferFrame();

	BOOL LoadWaferFrame(LONG lLoadBlock, BOOL bUseGripper1 = FALSE, BOOL bToStandBy = FALSE);
	BOOL UnloadWaferFrame(CONST BOOL bUseGripper1 = FALSE, BOOL bToStandBy = FALSE);
	BOOL LoadWaferFromMgzn(LONG lLoadBlock, BOOL bUseGripper1 = TRUE);
	BOOL LoadWaferToWT(BOOL bUseGripper1 = TRUE, BOOL bToStandBy = FALSE, BOOL bAutoBond = FALSE);
	BOOL UnloadWaferFromWT(BOOL bUseGripper1);
	BOOL UnloadWaferToMgzn(LONG lUnloadBlock, BOOL bUseGripper1, BOOL bToStandBy = FALSE);

	////////////////////////////////////////////////////////
	// Main BIn LOAD UNLOAD Functions
	BOOL OpLoadBinFrame(LONG lLoadBlock);
	BOOL OpUnloadLastBinFrame();

	BOOL LoadBinFrameFromMgzn(LONG lLoadBlock, BOOL bUseGripper1 = TRUE);
	BOOL LoadBinFrameToTable(BOOL bUseGripper1 = TRUE, BOOL bToStandBy = FALSE);
	BOOL UnloadBinFrameFromTable(CONST BOOL bUseGripper1 = TRUE, BOOL bToStandBy = FALSE);
	BOOL UnloadBinFrameToMgzn(CONST BOOL bUseGripper1 = TRUE);

	////////////////////////////////////////////////////////
	// Main Wafer LOAD UNLOAD Functions

	////////////////////////////////////////////////////////
	// BIN LOAD UNLOAD Supporting Functions
	BOOL LoadUnloadWafExpander(BOOL bLoad, INT &nConvID, BOOL bWait = TRUE);
	BOOL LoadUnloadWafExpander_Sync(BOOL bLoad, INT nConvID);
	BOOL LoadUnloadBinExpander(BOOL bLoad, INT &nConvID, BOOL bWait = TRUE);
	BOOL LoadUnloadBinExpander_Sync(BOOL bLoad, INT nConvID);
	BOOL IsWafExpanderOpen();
	BOOL IsBinExpanderOpen();

	BOOL MoveXYToMgzn(LONG lMgzn, LONG lSlot, BOOL bUseGripper1, BOOL bLoad);
	BOOL MoveXYToStandby(BOOL bWait = TRUE, BOOL bMoveZ = FALSE);
	BOOL MoveXYToBinTable(BOOL bUseGripper1, BOOL bLoad, BOOL bWait = TRUE);
	BOOL MoveXYToWaferTable(BOOL bUseGripper1, BOOL bLoad, BOOL bWait = TRUE);
	BOOL IsXYEncoderWithinRange();

	BOOL MoveZDownToMgzn(LONG lMgzn, LONG lSlot, BOOL bLoad, BOOL bUseGripper1);
	BOOL MoveZDownToBinTable(BOOL bUseGripper1, BOOL bLoad);
	BOOL MoveZDownToWafTable(BOOL bUseGripper1, BOOL bLoad);
	BOOL MoveZUpToStandby(BOOL bUseGripper1);
	BOOL MoveZUpToHome(BOOL bUseGripper1);
	BOOL IsGripperHasFrame(BOOL bUseGripper1, BOOL bFailToHome);
	BOOL IsGripperHasNoFrame(BOOL bUseGripper1);

	BOOL ScanningBarcode(CString &szBarcode, BOOL bScanTwice = FALSE);
	BOOL GetNullMgznSlot(LONG &lMgzn, LONG &lSlot, LONG lBlock=0);
    BOOL GetLoadMgzSlot(LONG &lMgzn, LONG &lSlot, BOOL &bUseEmpty, LONG lPhysicalBlock);
    BOOL GetFullMgznSlot(LONG &lMgzn, LONG &lSlot, LONG lPhysicalBlock=0);	
    BOOL GetMgznSlot(LONG lPhysicalBlock, LONG& lMgzn, LONG& lSlot);
	BOOL GetCurrWafMgzSlot(LONG lLoadBlock, LONG &lMgzn, LONG &lSlot);
	BOOL UpdateMgznSlotStatus(BOOL bIsLoad, LONG lMgzn, LONG lSlot, LONG lBlock);

	////////////////////////////////////////////////////////
	// Sensor IO control fcns
	VOID SetBinExpanderOpen(BOOL bSet);
	VOID SetWafExpanderOpen(BOOL bSet);

	BOOL SetWafGripperOpen(BOOL bSet);
	BOOL SetWafGripperClose(BOOL bSet);
	BOOL SetBinGripperOpen(BOOL bSet);
	BOOL SetBinGripperClose(BOOL bSet);

	BOOL IsGripper1FrameExist();
	BOOL IsGripper2FrameExist();

	BOOL CheckGripperFrameExistForXYMove(BOOL bUseGripper1);

	////////////////////////////////////////////////////////
	// Mgzn Setup Functions
	CString GetMagazineName(ULONG ulMagazine);
	CString GetMgznUsage(LONG lIndex);
	CString GetOMSlotUsage(ULONG ulMgzn, ULONG ulSlot);
	CString GetRTSlotUsage(ULONG ulMgzn, ULONG ulSlot);
	CString GetSlotUsage(const LONG lSlotUsageType);

    BOOL LoadMgznOMData(VOID);
    BOOL SaveMgznOMData(VOID);
	BOOL LoadMgznRTData(VOID);
    BOOL SaveMgznRTData(VOID);

	VOID ResetAllMagazine();
	LONG ResetMagazine(ULONG ulMgzn, BOOL bIsManual=FALSE, BOOL bClearBin=FALSE);
	BOOL IsMagazineCanReset(ULONG lMgzn, BOOL bIsCheckAll);
    VOID SetOperationMode_A(VOID);				//MS100 8mag 150bins STANDARD config	//v3.82
	VOID UpdateHmiMgznSlotID();

	VOID GenerateOMSPTableFile(ULONG ulMgzn);
	LONG SelectSlotUsage_UNUSE_TARGET(LONG lUsageOld, LONG lUsageTarget);
	LONG SelectMgznUsage_UNUSE_TARGET(LONG lUsageOld, LONG lUsageTarget);
	LONG SelectMgznUsage_UNUSE_TARGET2(LONG lUsageOld, LONG lUsageTarget1, LONG lUsageTarget2);
	VOID ChangeOMMgznSlotUsage(LONG lMgznIndex, LONG lUsageNew);
	BOOL CheckPhysicalBlockValid(ULONG ulBlock);
	VOID GenerateOMRTTableFile(ULONG ulMgzn);
	
	////////////////////////////////////////////////////////
	// Other Utility Functions
    LONG NL_DEBUGBOX(CString str); 
    LONG NL_DEBUGMESSAGE(CString str); 
    LONG NL_OK_CANCEL(CString strText, CString strTitle);
    LONG NL_YES_NO(CString strText, CString strTitle);
	VOID SetBinTableJoystick(BOOL bState);


private: // IPC command functions

	LONG Test(IPC_CServiceMessage& svMsg);
	LONG StartLoopTest(IPC_CServiceMessage& svMsg);
	LONG StopLoopTest(IPC_CServiceMessage& svMsg);
	BOOL PerformLoopTest();
	BOOL m_bLoopTestStart;
	BOOL m_bLoopTestAbort;
	BOOL m_bLoopTestForward;
	UCHAR m_ucLoopTestNo;
	LONG  m_lLoopTestStartGrade;
	LONG  m_lLoopTestEndGrade;

	LONG UpdateOutput(IPC_CServiceMessage& svMsg);
	LONG UpdateAction(IPC_CServiceMessage& svMsg);
	LONG SaveData(IPC_CServiceMessage& svMsg);
	LONG PowerOnMotor(IPC_CServiceMessage& svMsg);
	LONG PowerOffMotor(IPC_CServiceMessage& svMsg);
	LONG HomeMotor(IPC_CServiceMessage& svMsg);

	LONG Diag_Move_X(IPC_CServiceMessage& svMsg);
	LONG Diag_Move_Y(IPC_CServiceMessage& svMsg);
	LONG Diag_Move_Z1(IPC_CServiceMessage& svMsg);
	LONG Diag_Move_Z2(IPC_CServiceMessage& svMsg);
	LONG Diag_NegMove_X(IPC_CServiceMessage& svMsg);
	LONG Diag_NegMove_Y(IPC_CServiceMessage& svMsg);
	LONG Diag_NegMove_Z1(IPC_CServiceMessage& svMsg);
	LONG Diag_NegMove_Z2(IPC_CServiceMessage& svMsg);
	LONG m_lDiagMoveDistance;

	LONG MovePosLoadUnloadPos(IPC_CServiceMessage& svMsg);
	LONG MoveNegLoadUnloadPos(IPC_CServiceMessage& svMsg);
	LONG KeyInLoadUnloadPos(IPC_CServiceMessage& svMsg);
	LONG TeachLoadUnloadPosXY(IPC_CServiceMessage& svMsg);
	LONG ConfirmLoadUnloadPosXY(IPC_CServiceMessage& svMsg);
	LONG CancelLoadUnloadPosXY(IPC_CServiceMessage& svMsg);
	LONG MoveWTLoadOffsetXY(IPC_CServiceMessage& svMsg);
	LONG TeachLoadUnloadLevel(IPC_CServiceMessage& svMsg);
	LONG ConfirmLoadUnloadLevel(IPC_CServiceMessage& svMsg);
	LONG CancelLoadUnloadLevel(IPC_CServiceMessage& svMsg);

	LONG SelectMagazine(IPC_CServiceMessage& svMsg);
	LONG ChangeMagazineIndex(IPC_CServiceMessage& svMsg);
	LONG KeyInValue(IPC_CServiceMessage& svMsg);
	LONG IncreaseValue(IPC_CServiceMessage& svMsg);
	LONG DecreaseValue(IPC_CServiceMessage& svMsg);
	LONG UpdateSlotPitch(IPC_CServiceMessage& svMsg);
	LONG CancelMagazineSetup(IPC_CServiceMessage& svMsg);
	LONG ConfirmMagazineSetup(IPC_CServiceMessage& svMsg);
	LONG MoveToSlot(IPC_CServiceMessage& svMsg);
	
	LONG SelectMagazineDisplay(IPC_CServiceMessage& svMsg);

    LONG SetOperationMode(IPC_CServiceMessage &svMsg);
    LONG SelectOMMgznSetup(IPC_CServiceMessage &svMsg);
	LONG SetMgznUsage(IPC_CServiceMessage &svMsg);
	LONG SelectOMSlotSetup(IPC_CServiceMessage &svMsg);
    LONG SetSlotUsage(IPC_CServiceMessage &svMsg);
    LONG SetSlotBlock(IPC_CServiceMessage &svMsg);

	LONG LoadOperationMode(IPC_CServiceMessage& svMsg);
	LONG LoadCustomOpMode(IPC_CServiceMessage& svMsg);
	LONG SaveOperationMode(IPC_CServiceMessage& svMsg);

	////////////////////////////////////////////////////////
	// Main NVC BIN Setup Functions
 	LONG ManualLoadFilmFrame(IPC_CServiceMessage &svMsg);
	LONG ManualUnloadFilmFrame(IPC_CServiceMessage &svMsg);
 	LONG ManualLoadFrameFromMgzn(IPC_CServiceMessage &svMsg);
 	LONG ManualLoadFrameToTable(IPC_CServiceMessage &svMsg);
 	LONG ManualUnloadFrameFromTable(IPC_CServiceMessage &svMsg);
 	LONG ManualUnloadFrameToMgzn(IPC_CServiceMessage &svMsg);

	////////////////////////////////////////////////////////
	// Main NVC Wafer Setup Functions
 	LONG ManualLoadWaferFrame(IPC_CServiceMessage &svMsg);
	LONG ManualUnloadWaferFrame(IPC_CServiceMessage &svMsg);
 	LONG ManualLoadWaferFromMgzn(IPC_CServiceMessage &svMsg);
 	LONG ManualLoadWaferToTable(IPC_CServiceMessage &svMsg);
 	LONG ManualUnloadWaferFromTable(IPC_CServiceMessage &svMsg);
 	LONG ManualUnloadWaferToMgzn(IPC_CServiceMessage &svMsg);

	////////////////////////////////////////////////////////
	// Other Main NVC Setup Functions
	LONG ResetMagazineCmd(IPC_CServiceMessage &svMsg);
	LONG RealignBinFrameCmd(IPC_CServiceMessage &svMsg);
	LONG OpenCloseWafExpander(IPC_CServiceMessage &svMsg);
	LONG OpenCloseBinExpander(IPC_CServiceMessage &svMsg);

	////////////////////////////////////////////////////////
	// NVC IO Menu Functions
	LONG SetBinExpanderSol(IPC_CServiceMessage &svMsg);
	LONG SetWafExpanderSol(IPC_CServiceMessage &svMsg);
	LONG OpenCloseWafGripper(IPC_CServiceMessage &svMsg);
	LONG OpenCloseBinGripper(IPC_CServiceMessage &svMsg);
	BOOL SetWafGripper(BOOL bOpen);
	BOOL SetBinGripper(BOOL bOpen);


private:	// for variables and elements

	CMSNmAxisInfo	m_stNLAxis_X;
	CMSNmAxisInfo	m_stNLAxis_Y;
	CMSNmAxisInfo	m_stNLAxis_Z1;
	CMSNmAxisInfo	m_stNLAxis_Z2;

	CEvent	m_evNLOperate;			// NVC Loader - A Operate event
	CEvent	m_evNLReady;			// NVC Loader Ready event
	CEvent	m_evNLPreOperate;		// NVC Loader Pre Operate event

	//BOOL	m_bHardwareReady;
	BOOL	m_bUpdateOutput;	// Update Sensor & encoder in HMI

	BOOL	m_bHome_X;	
	BOOL	m_bHome_Y;	
	BOOL	m_bHome_Z1;
	BOOL	m_bHome_Z2;

	BOOL	m_bSel_X;
	BOOL	m_bSel_Y;
	BOOL	m_bSel_Z1;	
	BOOL	m_bSel_Z2;	

	BOOL	m_bXYUseEncoder;

	BOOL	m_bIsEnabled;		

	DOUBLE	m_dXRes;			// X Resolution (mm / motor step)
	DOUBLE	m_dYRes;			// Y Resolution (mm / motor step)
	DOUBLE	m_dZRes;			// Z Resolution (mm / motor step)

	LONG	m_lLLimit_X;
	LONG	m_lHLimit_X;
	LONG	m_lLLimit_Y;
	LONG	m_lHLimit_Y;
	LONG	m_lLLimit_Z1;
	LONG	m_lHLimit_Z1;
	LONG	m_lLLimit_Z2;
	LONG	m_lHLimit_Z2;

	// Encoder count
	LONG	m_lEnc_X;	
	LONG	m_lEnc_Y;		
	LONG	m_lEnc_Z1;	
	LONG	m_lEnc_Z2;	

	BOOL	m_bIsWafExpanderOpen;
	BOOL	m_bIsBinExpanderOpen;
	BOOL	m_bIsGripper1FrameExist;
	BOOL	m_bIsGripper2FrameExist;

	//Bin Loader magazine Position
    //BL_MAGAZINE m_stMgznRT[MS_BL_MGZN_NUM];
 	BL_MAGAZINE m_stNVCMgznRT[MS_BL_MGZN_NUM];			// magazine run time settings
	BL_MAGAZINE m_stMgznOM[MS_BL_MGZN_NUM];	
	LONG	m_lOMRT;			//For Real-Time
    LONG	m_lOMSP;			//For SetuP
    CString m_szOMSP;
    CString m_szNLMgznUse[MS_BL_MGZN_NUM];
    CString m_szNLSlotUsage;
    BOOL    m_bSetSlotBlockEnable;
    LONG	m_ulMgznSelected;
    LONG	m_ulSlotSelected;
    LONG	m_ulSlotPhyBlock;

	BOOL	m_bNoSensorCheck;
	BOOL	m_bWTUseGripper1ForSetup;
	BOOL	m_bBTUseGripper1ForSetup;

	LONG	m_lCurrMgzn1;
	LONG	m_lCurrMgzn2;
	LONG	m_lCurrSlotID1;
	LONG	m_lCurrSlotID2;
	LONG	m_lCurrBlock1;
	LONG	m_lCurrBlock2;
	LONG	m_lCurrBTBlock;
	LONG	m_lCurrBTMgzn;	
	LONG	m_lCurrBTSlotID;

	LONG	m_lCurrWafSlotID;
	LONG	m_lCurrWTBlock;

	LONG	m_lCurrHmiMgzn1;		// Current Mgzn no for DLA display only	
	LONG	m_lCurrHmiMgzn2;		// Current Mgzn no for DLA display only	
	LONG	m_lCurrHmiSlot1;		// Current Slot no 
	LONG	m_lCurrHmiSlot2;		// Current Slot no 
	CString m_szCurrHmiMgznName1;
	CString m_szCurrHmiMgznName2;
	CString m_szFrameBarcode1;
	CString m_szFrameBarcode2;

	LONG	m_lStandByPos_X;
	LONG	m_lStandByPos_Y;
	LONG	m_lWTLoadUnloadPos1_X;
	LONG	m_lWTLoadUnloadPos1_Y;
	LONG	m_lWTLoadUnloadPos2_X;
	LONG	m_lWTLoadUnloadPos2_Y;
	LONG	m_lBTLoadUnloadPos1_X;
	LONG	m_lBTLoadUnloadPos1_Y;
	LONG	m_lBTLoadUnloadPos2_X;
	LONG	m_lBTLoadUnloadPos2_Y;

	LONG	m_lStandByPos_Z1;
	LONG	m_lStandByPos_Z2;
	LONG	m_lWafLoadPos_Z1;
	LONG	m_lWafLoadPos_Z2;
	LONG	m_lBinLoadPos_Z1;
	LONG	m_lBinLoadPos_Z2;

	LONG	m_lWTLoadOffsetX;
	LONG	m_lWTLoadOffsetY;
	LONG	m_lWTLoadOffsetZ;
	LONG	m_lWTUnloadOffsetX;
	LONG	m_lWTUnloadOffsetY;	
	LONG	m_lWTUnloadOffsetZ;	

	LONG	m_lBTLoadOffsetX;
	LONG	m_lBTLoadOffsetY;
	LONG	m_lBTLoadOffsetZ;
	LONG	m_lBTUnloadOffsetX;
	LONG	m_lBTUnloadOffsetY;
	LONG	m_lBTUnloadOffsetZ;

	LONG	m_lMgznLoadOffsetX;
	LONG	m_lMgznLoadOffsetY;
	LONG	m_lMgznLoadOffsetZ;
	LONG	m_lMgznUnloadOffsetX;
	LONG	m_lMgznUnloadOffsetY;
	LONG	m_lMgznUnloadOffsetZ;

	LONG	m_lBackupPosition;
	LONG	m_lBackupPos_X;
	LONG	m_lBackupPos_Y;
	LONG	m_lBackupPos_Z;

	LONG    m_lSelMagazineID;
	LONG	m_lSelSlotID;
	DOUBLE	m_dSlotPitch;		// Slot Pitch in mm

    LONG    m_lNLGeneral_1;		//Magazine top posn X
    LONG    m_lNLGeneral_2;		//Magazine center posn Y
    LONG    m_lNLGeneral_3;		//Slot pitch
    LONG    m_lNLGeneral_4;		//Total slots
	LONG    m_lNLGeneral_5;		
    LONG    m_lNLGeneral_6;		
    LONG    m_lNLGeneral_7;		
	LONG    m_lNLGeneral_8;		
	LONG    m_lNLGeneral_9;		
	LONG    m_lNLGeneral_10;	
	LONG    m_lNLGeneral_11;	
	LONG    m_lNLGeneral_12;	
	LONG    m_lNLGeneral_TmpA;		//Temp. variable

	//CEvent	m_evBLOperate;	

};
