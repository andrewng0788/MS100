/////////////////////////////////////////////////////////////////
// Safety.cpp : interface of the CSafety class
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
#include "FileUtil.h"
#include "LogFileUtil.h"
#include "ToolsUsageRecord.h"

#define JS_BT_AXIS_X						"BinTableXAxis"
#define JS_BT_AXIS_Y						"BinTableYAxis"
#define JS_WT_AXIS_X						"WaferXAxis"
#define JS_WT_AXIS_Y						"WaferYAxis"
#define JS_WT_AXIS_X2						"WaferX2Axis"
#define JS_WT_AXIS_Y2						"WaferY2Axis"

enum eST_EVENTID {CHECK_LAN_CONNECTION = 1};

class CSafety : public CMS896AStn
{
	DECLARE_DYNCREATE(CSafety)

public:

	//=======================================================
	// Class Basic fcns
	CSafety();
	virtual	~CSafety();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();
	virtual BOOL InitData();			
	VOID Operation();


protected:		//Class fcns

	//=======================================================
	// Update fcns
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	VOID CheckLANConnection();
	VOID CheckFileCopyingQueue();
	ULONG	m_ulAutoUploadCounter;
	CTime	m_stLastAccessTime;
	BOOL AutoUploadOutputFiles();
	BOOL LoopUploadOutputFiles(CString szSrcPath, CString szTgtPath);

	BOOL JoyStickMove(VOID);			//v3.76
	BOOL MouseJoystickMove();
	BOOL MouseClickMove();
	BOOL MouseDragMove();

	BOOL InitBondHeadTPreHeat();
	BOOL CheckBondHeadTThermal();

	virtual VOID RegisterVariables();	
	virtual VOID UpdateStationData();

	VOID GetAxisInformation();

	//=======================================================
	//State fcns
	VOID RunOperation();
	virtual	VOID AutoOperation();
	virtual	VOID DemoOperation();
    virtual VOID StopOperation();

	//=======================================================
	//Class Supporting fcns
	BOOL LoadData(VOID);
	BOOL SaveData(VOID);
	VOID SetLamp(LONG lLevel);			// 0 = OFF, 1 = Green, 2 = Yellow, 3 = Red
	BOOL PrintLabelInAutoBondMode();	//v2.64
	BOOL ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY);
	BOOL CalculateMouseClickPos(PR_COORD stDieOffset, int *siStepX, int *siStepY);
	BOOL TableMoveTo(LONG lTargetX, LONG lTargetY);
	BOOL TableMove(LONG lDiff_X, LONG lDiff_Y);
	BOOL GetTableEncoder(LONG *lEnc_X, LONG *lEnc_Y);
	VOID SetBackupNVRam(BOOL bState = TRUE);
	BOOL WaitBackupNVRam(INT nTimeout = LOCK_TIMEOUT);

	LONG LogItems(LONG lEventNo);
	LONG GetLogItemsString(LONG lEventNo, CString& szMsg);

	BOOL LightTowerOperate();

	BOOL	MonitorFalseAutoRunning();
	BOOL	MonitorThermalControlAutoRunning();
	BOOL	ST_EnableThermalControl(BOOL bEnable);
	BOOL	ST_UpdateThermalTemperature();
	LONG	m_lMS60ThermalStateBH;
	ULONG	m_ulMonitorPickCounter;
	ULONG	m_ulAutoMaxPauseTime;
	UINT	m_unEnableAIDebugLog;
	CTime	m_stPickTime;

protected:		//IPC HMI commands

	//=======================================================
	// IPC Registered Services
	LONG SrvAlarmEvent(IPC_CServiceMessage& svMsg);
	LONG SrvStopAutoAlign(IPC_CServiceMessage& svMsg);

	//=======================================================
	/* Set Alarm Lamp
		Input Parameter: LONG lLevel - 0 : Green, 1 : Yellow, 2 : Red
		Output Parameter: Nothing
	*/
	LONG PressStopButton(IPC_CServiceMessage& svMsg);
	LONG PressStartButton(IPC_CServiceMessage& svMsg);
	LONG PressStandByButton(IPC_CServiceMessage& svMsg);
	LONG SetAlarmLamp(IPC_CServiceMessage& svMsg);
	LONG SetAlarmLampTest(IPC_CServiceMessage& svMsg);
	
	//=======================================================
	// PR commands
	LONG PreOnSelectShowPrImage(IPC_CServiceMessage& svMsg);
	LONG UserSearchDie(IPC_CServiceMessage& svMsg);
	LONG HotKeySearchRefDie(IPC_CServiceMessage& svMsg);		//v3.80
	LONG SelectCamera(IPC_CServiceMessage& svMsg);				//v3.99T1
	LONG UserPRLoopTest(IPC_CServiceMessage& svMsg);//Matt: PR loop test

	LONG PrescanGoToSelectCamPreTask(IPC_CServiceMessage& svMsg);
	
	//=======================================================
	// Other motor commands
	LONG SaveMachineData(IPC_CServiceMessage& svMsg);
	LONG UpdateAction(IPC_CServiceMessage& svMsg);

	//=======================================================
	// Wafermap commands
	LONG TestWaferMap(IPC_CServiceMessage& svMsg);
	LONG TestBinMap(IPC_CServiceMessage& svMsg);
	LONG	TestPrbMap2(IPC_CServiceMessage& svMsg);
	LONG	TestPrbMap3(IPC_CServiceMessage& svMsg);
	LONG	TestPrbMap4(IPC_CServiceMessage& svMsg);

	LONG LogItems(IPC_CServiceMessage& svMsg);


private:	//Class attributes

	// Alarm Lamp Bit
	//SFM_CHipecDigitalOutput*	m_poAlarmLampRed;
	//SFM_CHipecDigitalOutput*	m_poAlarmLampYellow;
	//SFM_CHipecDigitalOutput*	m_poAlarmLampGreen;
	//SFM_CHipecDigitalOutput*	m_poAlarmSound;

	// Axis Info
	CMSNmAxisInfo	m_stWTAxis_X;
	CMSNmAxisInfo	m_stWTAxis_Y;
	CMSNmAxisInfo	m_stWTAxis_X2;
	CMSNmAxisInfo	m_stWTAxis_Y2;

	CMSNmAxisInfo	m_stBTAxis_X;
	CMSNmAxisInfo	m_stBTAxis_Y;
	CMSNmAxisInfo	m_stBTAxis_X2;		//MS100 9Inch dual-table config		//v4.17T1
	CMSNmAxisInfo	m_stBTAxis_Y2;		//MS100 9Inch dual-table config		//v4.17T1

	//LONG	m_lProgressBarStep;	
	//LONG	m_lProgressBarLimit;
	BOOL	m_bSetAlarmBlinkTimer;
	CTime	m_ctAlarmBlinkTimer;

	LONG	m_lBHTThermalSetPoint;
	BOOL	m_bInitBondHeadPreHeat;

	//Check LAN Connection is exist or not
	BOOL	m_bCheckLANConnection;
	BOOL	m_bIsLANEnabled;

	CEvent  m_evBackupNVRam;
	BOOL	m_bEnableBinMapFcn;		//v4.03		//PLLM/PLSG binmap fcn

	CSafetyDataBlock m_oSafetyDataBlk;
};

