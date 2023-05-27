
/////////////////////////////////////////////////////////////////
// WaferTable.cpp : interface of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include <math.h>
#include <BinSetupWrapper.h>
#include "WaferTable_Var.h"		//v4.47T5
#include "WT_Constant.h"
#include "WT_BlkFunc.h"
#include "WT_1stDieFinder.h"
#include "FlushMessageThread.h"
#include "LastScanPosition.h"
#include "WT_BlkFunc2.h"
// prescan relative code
#include "PrescanConstant.h"
//#include "PstnModel.h"
// Temperature Controller	Thermal Ejector
//#include "LiTECWin32.h"
// Temperature Controller	Thermal Ejector
#include "WAF_CWaferZoomWindowView.h"
#include "gdiplus.h"
#include "WaferMap.h"
#include "EjPinCleanRegion.h"

class CWaferTable : public CWaferTable_Var		//CMS896AStn	//v4.47T5
{
	DECLARE_DYNCREATE(CWaferTable)

public:
	// Init (WaferTable.cpp)
	CWaferTable();
	virtual	~CWaferTable();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();
	virtual BOOL InitData();			// Init the data from SMF and NVRAM
	virtual VOID ClearData();			// Clear data content for cold-start
	virtual VOID FlushMessage();		// Flush IPC Message	
	virtual VOID UpdateStationData();
	BOOL	LoadWftData();
	virtual VOID ReloadMachineConstant();
	virtual VOID ExportMachineConstant();
	virtual VOID ImportMachineConstant();

	virtual VOID UpdateAllSGVariables();

	VOID SaveMachineConstant();

	VOID ScnLoaded(BOOL bLoaded);

	VOID ChangeRegionGrade(LONG lState);
	BOOL RealignBinFrame();
	BOOL IsCheckRepeatMap();
	VOID MapLoaded();
	BOOL CheckNichiaCriteraInMapLoaded();
	BOOL SetupRenesasMapInMapLoaded();								//v4.59A15

	BOOL CheckMapNamePrefixSuffix(CONST CString szFileName);		//v4.39T7	//Silan
	BOOL CheckMapSpecNamePrefix(CONST CString szFileName);			//4.52D15
	CString GetWaferMapFormat();
	VOID SetWaferMapFormat(CString szWaferMapFormat);
	VOID SetEnableSmartWalk(CONST BOOL bEnable);			//v2.99T1

	// 4.53D16 SeoulSemi Comparing data fnc 
	BOOL ComparingProjectHeaderWithPackgeName();

	// for press the die on map and let the table move
	VOID ClickMapAndTableGo();
	//For Block Function
	VOID SetBlkFuncEnable(BOOL bBlkFunc);
	VOID BlkGoToPosition();
	IPC_CClientCom& GetIPCClientCom(); 

	BOOL XY_MoveToPrescanPosn();
	BOOL XY_SafeMoveTo(LONG lPosX, LONG lPosY, BOOL bUsePosOffset = FALSE);
	BOOL XY_SafeMoveTo(LONG lPosX, LONG lPosY, DOUBLE dScale, BOOL bUsePosOffset = FALSE);
	BOOL XY1_SafeMoveTo(LONG lPosX, LONG lPosY, DOUBLE dScale, BOOL bUsePosOffset = FALSE);
	BOOL XY2_SafeMoveTo(LONG lPosX, LONG lPosY, DOUBLE dScale, BOOL bUsePosOffset = FALSE);
	BOOL XY_SlowMoveTo(LONG lPosX, LONG lPosY);
	BOOL GetRtnForCheckWaferLimit(LONG lX, LONG lY);
	INT  GetRtnForXMoveTo(INT nPos, INT nMode);
	INT  GetRtnForYMoveTo(INT nPos, INT nMode);
	INT  GetRtnForTMoveTo(INT nPos, INT nMode);		//Block2
	BOOL GetRtnForRotateWaferTheta(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree);

	BOOL GetDiePitchX(LONG *lX, LONG *lY);
	BOOL GetDiePitchY(LONG *lX, LONG *lY);
	VOID GetEncoder(LONG *lX, LONG *lY, LONG *lT);
	LONG GetCurrX();
	LONG GetCurrY();
	LONG GetCurrCmdY();
	LONG GetCurrT();
	LONG GetCurrX1();	// 4.24TX1
	LONG GetCurrY1();
	LONG GetCurrCmdY1();
	LONG GetCurrT1();
	LONG GetCurrX2();
	LONG GetCurrY2();
	LONG GetCurrCmdY2();
	LONG GetCurrT2();
	VOID GetThetaRes(DOUBLE *dRes);
	VOID GetRtnForSetJoystick(BOOL bOn);
	BOOL GetRtnForMachineNo(CString *strMchNo);
	BOOL GetRtnForSwVerNo(CString *strSwVerNo);
	BOOL GetRtnForMapFileNm(CString *strMapFileNm);
	BOOL GetRtnSetCtmLogMessage(const CString& szStatus);	//Block2
	BOOL GetRtnSetErrorLogMessage(const CString& szStatus);	//Block2
	LONG GetRtnForGetLightTower();
	BOOL WftSearchNormalDie(LONG &lPosX, LONG &lPosY, BOOL bDoComp);
	LONG GetRtnForHmiMessage(const CString& szText,	const CString& szTitle = _T(""),
							LONG lType = glHMI_MBX_CLOSE,
							LONG lAlignment = glHMI_ALIGN_CENTER,
							LONG lTimeout = 36000000,
							LONG lMsgMode = glHMI_MSG_MODAL,
							LONG* plMsgID = NULL,
							LONG lWidth = -1,
							LONG lHeight = -1,
							CString* pszBitmapFile = NULL,
							CString* pszCustomText1 = NULL,
							CString* pszCustomText2 = NULL,
							CString* pszCustomText3 = NULL);

	LONG GetRtnForHmiMessageEx(const CString& szText,	const CString& szTitle = _T(""),
							LONG lType = glHMI_MBX_CLOSE,
							LONG lAlignment = glHMI_ALIGN_CENTER,
							LONG lTimeout = 36000000,
							LONG lMsgMode = glHMI_MSG_MODAL,
							LONG* plMsgID = NULL,
							LONG lWidth = -1,
							LONG lHeight = -1,
							CString* pszBitmapFile = NULL,
							CString* pszCustomText1 = NULL,
							CString* pszCustomText2 = NULL,
							CString* pszCustomText3 = NULL);
	BOOL	SetMapPhyPosn(ULONG ulRow, ULONG ulCol, LONG lPhyX, LONG lPhyY);

	BOOL SpiralSearchHomeDie();
	BOOL BlockPickAlignHalfWafer();
	BOOL SearchAndAlignDie(CONST BOOL bNormalDie, CONST BOOL bSrch1DieWnd, CONST BOOL bDoComp, CONST UCHAR ucDieID=1);
	LONG IsAutoAlignElCornerWafer(IPC_CServiceMessage &svMsg);
	BOOL SetAutoBondMode(BOOL bAutoBondMode);

	VOID SetupMapSequenceParameters();
	BOOL CheckIgnoreGradeCount();

	//1st-die Map Finder fcns
	VOID DisplayCurrentDie();	
	BOOL LookAroundPosn(ULONG ulRow, ULONG ulCol, ULONG ulCtrRow, ULONG ulCtrCol, CONST BOOL bLatch=TRUE);

	INT  OpBegin1stDieFinder();
	INT	 OpFinish1stDieFinder(ULONG ulMapRow, ULONG ulMapCol);
	INT  OpStart1stDieFinderToPick();
	INT  OpCycle1stDieFinderToPick();
	INT  OpAbort1stDieFinderToPick();

	/* Set joystick speed
		Input Parameter: LONG lLevel
		Output Parameter: BOOL bOK
	*/
	LONG SetJoystickSpeedCmd(IPC_CServiceMessage& svMsg);

	//Sub Region pick mode
	LONG SetupSubRegionMode(BOOL bLoadMap = FALSE, BOOL bSecondHalfSortMode = FALSE);
	BOOL GetRegionSortOuter();
	VOID RegionPickLog(CONST CString szLogMsg);
	VOID CheckRegionScanMode(CString szAlgorithm);
	BOOL m_bSortOuterHmi;

	//Block Function		//Block2
	DOUBLE	ConvertMotorStepToUnit(LONG lStep);
	BOOL	OneDieIndexGetDieValidPosn(LONG ulIntRow, LONG ulIntCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap);
	BOOL CheckCORTolerance(LONG lOffsetX, LONG lOffsetY);
	BOOL CheckCOROffset(LONG lOffsetX, LONG lOffsetY);
	BOOL CheckCOR(DOUBLE dAngle);					
	LONG DisplayErrorMessage(CONST CString szMsg);
	LONG DisplayRtMessage(CONST CString szMsg);
	BOOL GetWaferCalibXY(LONG& lCorX, LONG& lCorY);
	BOOL IsCheckCOREnabled() CONST;
	VOID SetRefDieCounts(CONST LONG lTotalCount, CONST LONG lCurrCount);

	BOOL IsDisableRefDie();			//v2.78T1
	VOID CheckResetManualAlignRegion();

	//PLLM Grade-Map Table fcns		//v2.83T31
	BOOL RestoreGradeMapTable();
	BOOL SaveGradeMapTable();	
	BOOL DeleteGradeMapTable();	
	BOOL DeleteLastLocalMapFile();
	
	INT	MapDieTypeCheck();
	BOOL IsMapDieCheckOk();

	INT PackageFileCheck();
	BOOL IsPackageFileCheckOk();

	//AutoMapDieTypeCheck (for Ubilux)	//v3.31T1
	BOOL AutoMapDieTypeCheck(BOOL bCheck);
	BOOL IsAutoMapDieCheckOk();

	INT  MESWaferIDCheck(INT nCode, CString szMsg);
	BOOL IsMESWaferIDCheckOk();
	INT ClearMESWaferIDCheck();

	BOOL LoadWaferMapErrorChecking();

	BOOL AutoLoadRankIDFile();

	BOOL CheckWExpanderLock();		//v3.48
	BOOL CheckWExpander2Lock();

	BOOL SearchMapFileInFolder(CString& szMapFileName, CString &szMapPath, CString szBarCode);
	BOOL SearchFolderInMapPath(CString& szSubFolderPath, CString szRootMapPath, CString szSrchName);
	BOOL SearchFileAndFolder(CString& szOutFileName, CString &szOutPath, CString szInName);

	LONG LoadBinSummaryFile(CString szFilename);
	BOOL LotInformation(BOOL bUseFilenameAsWaferID, CString szOperatorID);

	//Knowles MS109		//v4.35T2
	BOOL InitKnowlesNGMap();

	VOID BackupBurnInStatus();
	VOID RestoreBurnInStatus();

	BOOL PrepareNextRegionStatistics();
	LONG XY_SafeMoveToCmd(IPC_CServiceMessage& svMsg);
	LONG GetEncoderCmd(IPC_CServiceMessage& svMsg);
	LONG GetCommanderCmd(IPC_CServiceMessage& svMsg);

protected:
	// Update (WaferTable.cpp)
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	virtual VOID RegisterVariables();
	VOID RegisterVariables2();
	VOID RegisterVariables_EjPinClean();

	//State Operation (WT_State.cpp)
	VOID Operation();
	VOID RunOperation();
	VOID RunOperationWft_ES();
	VOID RunOperationWft_Scan();

	virtual	VOID IdleOperation();
	virtual	VOID DiagOperation();
	virtual	VOID InitOperation();
	virtual VOID PreStartOperation();
	virtual	VOID AutoOperation();
	virtual	VOID DemoOperation();
	virtual	VOID StopOperation();
	virtual	VOID ManualOperation();			//v3.13T2

	// Profile Functions (WT_Profile.cpp)
	INT X_Profile(INT nProfile);
	INT Y_Profile(INT nProfile);
	INT T_Profile(INT nProfile);
	LONG X_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	LONG Y_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	LONG Y_ScfProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	LONG T_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	LONG X1_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	LONG X2_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	LONG Y1_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	LONG Y2_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	LONG T1_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	LONG T2_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist);
	INT X1_Profile(INT nProfile);
	INT X1_SelectControlByDistance(LONG lX);
	INT Y1_Profile(INT nProfile);
	INT Y1_SelectControlByDistance(LONG lY);
	INT T1_Profile(INT nProfile);
	INT X2_Profile(INT nProfile);
	INT Y2_Profile(INT nProfile);
	INT T2_Profile(INT nProfile);
	CString X_ProfName(INT nProfile);
	CString Y_ProfName(INT nProfile);
	CString T_ProfName(INT nProfile);

	//Check Axis Power
	BOOL X_IsPowerOn();
	BOOL Y_IsPowerOn();
	BOOL T_IsPowerOn();
	BOOL X2_IsPowerOn();
	BOOL Y2_IsPowerOn();
	BOOL T2_IsPowerOn();

	// Move Functions (WT_Move.cpp)
	INT X_Home();
	INT X12_Home();			//v4.24
	INT X_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT X_Move(INT nPos, INT nMode = SFM_WAIT);
	INT	X_Sync();
	INT X_PowerOn(BOOL bOn = TRUE);
	INT X_Comm();
	//v4.24		//ES101
	INT X2_Home();
	INT X2_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT X2_Move(INT nPos, INT nMode = SFM_WAIT);
	INT	X2_Sync();
	INT X2_PowerOn(BOOL bOn = TRUE);
	INT X2_Comm();

	INT Y_Home();
	//INT Y1_Home();
	INT Y_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Y_ScfProfileMove(INT nPos, INT nMode = SFM_WAIT);
	INT Y_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Y_Sync();
	INT Y_PowerOn(BOOL bOn = TRUE);
	INT Y_Comm();
	//v4.24		//ES101
	INT Y2_Home();
	INT Y2_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Y2_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Y2_ScfProfileMove(INT nPos, INT nMode);
	INT Y2_Sync();
	INT Y2_PowerOn(BOOL bOn = TRUE);
	INT Y2_Comm();

	LONG	m_lPLLMWaferRow;
	LONG	m_lPLLMWaferCol;

	LONG    m_lWaitEjectorUp;

	INT T_Home(BOOL bFirstHome=FALSE, BOOL bResetAlignFlag = TRUE);
	INT T_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT T_Move(INT nPos, INT nMode = SFM_WAIT);
	INT T_Sync();
	INT T_PowerOn(BOOL bOn = TRUE);
	INT T_Comm();
	//v4.24		//ES101
	INT T2_Home(BOOL bFirstHome=FALSE);
	INT T2_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT T2_Move(INT nPos, INT nMode = SFM_WAIT);
	INT T2_Sync();
	INT T2_PowerOn(BOOL bOn = TRUE);
	INT T2_Comm();

	BOOL HomeTable1(BOOL bToHome=TRUE);
	BOOL HomeTable2(BOOL bToHome=TRUE);

	INT XY_Home();
	INT XY_SMoveTo(INT nPosX, INT nPosY, INT nMode = SFM_WAIT);		//v4.11T3	//PLLM REBEL
	INT XY_MoveTo(INT nPosX, INT nPosY, INT nMode = SFM_WAIT);
	INT XY_Move(INT nPosX, INT nPosY, INT nMode = SFM_WAIT, BOOL b4WT2=FALSE);

	BOOL XY_IsPowerOff();
	INT X1_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT X1_Move(INT nPos, INT nMode = SFM_WAIT);
	INT	X1_Sync();
	INT Y1_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Y1_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Y1_ScfProfileMove(INT nPos, INT nMode = SFM_WAIT);
	INT Y1_Sync();
	INT T1_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT T1_Move(INT nPos, INT nMode = SFM_WAIT, BOOL bProberManual = FALSE);
	INT T1_Sync();
	INT XY1_MoveTo(INT nPosX, INT nPosY, INT nMode = SFM_WAIT);
	INT XY2_MoveTo(INT nPosX, INT nPosY, INT nMode = SFM_WAIT);
	
	INT XYT_Home();
	INT XYT_MoveTo(INT nPosX, INT nPosY, INT nPosT, INT nMode = SFM_WAIT);
	INT XYT_Move(INT nPosX, INT nPosY, INT nPosT, INT nMode = SFM_WAIT);

	VOID SetJoystickOn(BOOL bOn, BOOL bIsWT2 = FALSE);			// Turn on or off the joystick
	VOID X_SetJoystickOn(BOOL bXOn, BOOL bIsWT2 = FALSE);		// Turn on or off x-direction of joystick
	VOID Y_SetJoystickOn(BOOL bYOn, BOOL bIsWT2 = FALSE);		// Turn on or off y-direction of joystick

	//Control Function
	INT X_SelectControl(INT nControlID);
	INT X2_SelectControl(INT nControlID);
	INT Y_SelectControl(INT nControlID);
	INT Y2_SelectControl(INT nControlID);

	//v4.59A39	//MS50
	BOOL EjX_IsPowerOn();
	BOOL EjY_IsPowerOn();
	INT EjX_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT EjY_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT EjX_Sync();
	INT EjY_Sync();
	BOOL OpMoveEjectorTableXY(BOOL bStart, INT nMode = SFM_WAIT);

	//v4.13T3	//MS100 9Inch Options
	VOID SetEJTSlideUpDn(BOOL bUp);
	BOOL IsEJTAtUnloadPosn();
	LONG SetEJTSlideUpCmd(IPC_CServiceMessage& svMsg);
/*
	BOOL	m_bEJTLLimit;
	BOOL	m_bEJTULimit;
*/

	//v4.26T1	//MS899EL	//Walsin China
	VOID SetWaferClamp(BOOL bOn);
	LONG SetWaferClampOnOff(IPC_CServiceMessage& svMsg);

	LONG IsExpanderClosed(IPC_CServiceMessage& svMsg);

	//v4.24T9	//ES101
	BOOL MoveES101BackLightZUpDn(BOOL bUp);
	BOOL MoveES101EjtElvtZToUpDown(BOOL bToUp);
	BOOL IsWT1UnderCamera();
	BOOL IsWT2UnderCamera();
	BOOL MoveFocusToWafer(BOOL bWft);

	BOOL IsWT1UnderEjectorPos();
	BOOL IsWT2UnderEjectorPos();

	// Sub-State Functions (WT_SubState.cpp)
	LONG GetDiePROffsetX() const;
	LONG GetDiePROffsetY() const;
	BOOL IsDiePRRotate();
	double GetDiePROffsetT() const;
	INT OpInitialize();
	INT OpPreStart();
	
	VOID OpResetLFCycle();
	INT OpGetWaferPos();
	INT OpGetWaferPos_CheckLFDie(unsigned long ulY, unsigned long ulX);		//v4.47T1
	INT OpGetWaferPos_NotOKHandling(INT nCode);		//v4.47T1
	INT OpGetWaferPos_BlockPick();					//v4.47T1
	INT OpGetWaferPos_Region();
	INT OpGetWaferPos_ES();
	INT	OpMoveTable();
	INT	OpMoveTable_ES();

	BOOL IsNGBlock(const UCHAR ucGrade);
	BOOL IsNGBlock(const LONG lRow, const LONG lCol);
	INT OpUpdateMapViaPR();
	INT OpUpdateLastDie_ES();
	INT OpNextDie_ES();
	LONG OpCalculateMoveTableMotionTime(LONG lMoveWfX, LONG lMoveWfY, LONG lMoveWfT, BOOL bRotate, LONG lX1=0, LONG lX2=0);
	INT OpNextDie();
	INT OpCompensation(BOOL bGoodDie=TRUE);
	INT OpStoreDieInfo();
	INT OpBlkRollBack();
	INT	OpRollBack();
	INT OpUpdateDie();
	INT	OpMoveComplete();
	INT	OpRepeatCycle();
	INT	OpGetLFResult();
	INT OpRotateBack();
	INT OpWaferEndMoveToUnload();
	INT OpCheckEjStatusBeforeWaferEndUnload();	//v4.52A8
	INT OpES101MoveToUnload();
	INT OpES101NextToAlignPosition();
	INT OpDisplayWaferEnd();
	INT OpDisplayWaferEndInfoAlarm();		//v3.57T2
	INT OpDisplayWaferEndAlarm();
	INT OpSkipSEPGradeDie();
	INT OpIsWithinPolygonLimit(unsigned long &ulRow, unsigned long &ulCol, unsigned char &ucGrade, 
							  WAF_CDieSelectionAlgorithm::WAF_EDieAction& eAction,
							  BOOL &bEndOfWafer);
	INT OpCalculateTNoMoveBackOffsetXY(long& lDx, long& lDy, double dTheta, long lDistX, long lDistY);	//v2.82
	INT OpLFDieStatusForSmartWalk();		//v2.99T1
	BOOL OpIsCurrentLFDie();				//v3.89		//MS100
	BOOL OpUpdateNextMotionTime();			//v3.94T4	//MS100
	BOOL OpMoveLargeWftToSafe(CString szState);
	VOID OpStopES101NGPIckCycle();			//v4.28
	VOID OpDebPitchErrorBackup();
	VOID OpBackupLogFiles();
	BOOL OpMS60NGPickPrEmptyDiePos();		//v4.54A5
	BOOL OpMS60CheckPrEmptyDieResult();		//v4.54A5

	VOID OpCalculateEjtOffsetXY(LONG& lMoveWfX, LONG& lMoveWfY, LONG& lMoveWfT, 
									BOOL bIsLF, BOOL bCheckLF, BOOL bDoComp, 
									BOOL bNeedRotate, BOOL bPick, BOOL bGDComp=TRUE);		//v4.42T15
	VOID OpCalculateNextEjtOffsetXY(LONG& lMoveWfX, LONG& lMoveWfY, LONG& lMoveWfT, 
									BOOL bIsLF, BOOL bCheckLF, BOOL bDoComp, 
									BOOL bNeedRotate, BOOL bPick);			//v4.47A7
	LONG OpCalculateLFNextDieDirection(CONST ULONG ulCurRow, CONST ULONG ulCurCol,
										CONST ULONG ulNextRow, CONST ULONG ulNextCol, LONG& lFOVMode);		//v4.43T2	//SanAn MS100PlusII
	BOOL OpSetGradeToEquip(LONG lGrade);
	BOOL UpdateMap(const ULONG ulRow, const ULONG ulCol, 
		const UCHAR ucGrade, const WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction, 
		const ULONG ulDieState=0);
	BOOL PeekMapNextDie(unsigned long ulSkipNumberOfDice,
					 unsigned long &ulRow, unsigned long &ulCol, unsigned char &ucGrade, 
 				     WAF_CDieSelectionAlgorithm::WAF_EDieAction& eAction,
					 BOOL &bEndOfWafer, CString szMsg="");
	// Hmi Registered Command (WT_Command.cpp)
	/* Move XYT
		Input Parameter: LONG X, LONG Y, LONG T
		Output Parameter: LONG encX, LONG encY, LONG encT
	*/
	LONG XYT_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG XYT_MoveToCmd2(IPC_CServiceMessage& svMsg);		//v2.83T2
	LONG XYT_MoveAdaptWaferCallBack(IPC_CServiceMessage& svMsg);
	LONG XYT_MoveCmd(IPC_CServiceMessage& svMsg);

	/* Move XY
		Input Parameter: LONG X, LONG Y
		Output Parameter: LONG encX, LONG encY, LONG encT
	*/
	LONG XY_SafeMoveToUnloadCmd(IPC_CServiceMessage& svMsg);	//v4.16T5	//SUpport MS100 9Inch
	LONG XY_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG XY_MoveToUnloadCmd(IPC_CServiceMessage& svMsg);		//v4.16T5	//SUpport MS100 9Inch
	LONG XY2_MoveToUnloadCmd(IPC_CServiceMessage& svMsg);		//v4.24T9	//SUpport ES101
	LONG XY_MoveCmd(IPC_CServiceMessage& svMsg);

	LONG XY1_MoveToHomeLoadCmd(IPC_CServiceMessage& svMsg);	// ES101 WFT1 slow move with wait motion complete option
	LONG XY2_MoveToHomeLoadCmd(IPC_CServiceMessage& svMsg);	// ES101 WFT2 slow move with wait motion complete option
	/* Move single axis
		Input Parameter: LONG Pos
		Output Parameter: LONG encX, LONG encY, LONG encT
	*/
	LONG X_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG X_MoveCmd(IPC_CServiceMessage& svMsg);
	LONG Y_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG Y_MoveCmd(IPC_CServiceMessage& svMsg);
	LONG T_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG T_MoveCmd(IPC_CServiceMessage& svMsg);

	/* Home Functions
		Input Parameter: Nothing
		Output Parameter: Nothing
	*/
	LONG X_HomeCmd(IPC_CServiceMessage& svMsg);
	LONG Y_HomeCmd(IPC_CServiceMessage& svMsg);
	LONG T_HomeCmd(IPC_CServiceMessage& svMsg);
	LONG T2_HomeCmd(IPC_CServiceMessage& svMsg);
	LONG XY_HomeCmd(IPC_CServiceMessage& svMsg);
	LONG XYT_HomeCmd(IPC_CServiceMessage& svMsg);

	/* On or off the joystick
		Input Parameter: BOOL bOn
		Output Parameter: BOOL bOK
	*/
	LONG SetJoystickCmd(IPC_CServiceMessage& svMsg);

	/* On or off the joystick
		Input Parameter: BOOL bLock
		Output Parameter: BOOL bOK
	*/
	LONG SetLockJoystickCmd(IPC_CServiceMessage& svMsg);

	/* On or off the joystick with checking the current state
		Input Parameter: BOOL bOn
		Output Parameter: BOOL bOK
	*/
	LONG UseJoystickCmd(IPC_CServiceMessage& svMsg);

	/* Set joystick speed
		Input Parameter: LONG lLevel
		Output Parameter: BOOL bOK
	*/

	/* Set max joystick limit
		Output Parameter: BOOL bOK
	*/
	LONG SetJoystickLimitCmd(IPC_CServiceMessage &svMsg);
	// ES101 command2
	LONG XY1_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG XY1_MoveCmd(IPC_CServiceMessage& svMsg);
	LONG XY2_MoveCmd(IPC_CServiceMessage& svMsg);
	LONG T1_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG T2_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG SetJoystickLimitCmd2(IPC_CServiceMessage &svMsg);
	LONG X2_SyncCmd(IPC_CServiceMessage& svMsg);
	LONG Y2_SyncCmd(IPC_CServiceMessage& svMsg);
	LONG T2_SyncCmd(IPC_CServiceMessage& svMsg);
	LONG XY2_SyncCmd(IPC_CServiceMessage& svMsg);			
	LONG GetWT1EncoderCmd(IPC_CServiceMessage& svMsg);
	LONG GetWT2EncoderCmd(IPC_CServiceMessage& svMsg);
	LONG AutoRotateWT(IPC_CServiceMessage& svMsg);

	LONG MoveToCORCmd(IPC_CServiceMessage &svMsg);


	/* Get the encoder values
		Input Parameter: Nothing
		Output Parameter: LONG encX, LONG encY, LONG encT
	*/
	LONG AdaptWaferGetEncoderValue(IPC_CServiceMessage& svMsg);

	LONG GetEncoderCmd2(IPC_CServiceMessage& svMsg);			//v2.83T2

    LONG EnableEjectorVacuum(IPC_CServiceMessage &svMsg);

	LONG OnSelectWaferLimitCmd(IPC_CServiceMessage &svMsg);

	LONG LoadMapFileForBurnIn(IPC_CServiceMessage &svMsg);
	LONG SetTableStartPointForBurnIn(IPC_CServiceMessage &svMsg);
	LONG SetMapStartPointForBurnIn(IPC_CServiceMessage &svMsg);
	LONG BurnInAutoAssignGrade(IPC_CServiceMessage &svMsg);


	LONG UpdatePitchTolerance(IPC_CServiceMessage& svMsg);

	LONG UpdateData(IPC_CServiceMessage& svMsg);
	LONG UpdateAction(IPC_CServiceMessage& svMsg);

	//For Block Function
	LONG ResetBlkFuncPara(IPC_CServiceMessage &svMsg);
	LONG CheckBlkAlignResult(IPC_CServiceMessage &svMsg);
	BOOL GoToAlignDie();				//v3.30T1
	//BOOL m_bStartGoToAlignDie;		//v3.30T1
	BOOL RealignFDCDie(IPC_CServiceMessage& svMsg);
	LONG UpdateMapDieTypeCheck(IPC_CServiceMessage& svMsg);
	LONG UpdateAutoMapDieTypeCheck(IPC_CServiceMessage& svMsg);		//v3.31T1
	LONG LoadDieTypeFromFile(IPC_CServiceMessage& svMsg);
	LONG ResetDieType(IPC_CServiceMessage& svMsg);
	LONG CheckDieType(IPC_CServiceMessage& svMsg);
	LONG AutoCheckCOR(IPC_CServiceMessage& svMsg);
	LONG CheckInputWaferIDHeader(IPC_CServiceMessage& svMsg);

	LONG GoToBlk1stAutoAlignPos(IPC_CServiceMessage& svMsg);		//v3.31

	LONG PrAdaptWafer(IPC_CServiceMessage& svMsg);

	LONG AutoAlignStdWafer(IPC_CServiceMessage& svMsg);

	VOID Blk1SetAlignParameter();
	VOID Blk2SetAlignParameter();
	BOOL Blk2FindAllReferDiePosn();

	BOOL FindWaferHomeDie();		//v4.08
	BOOL FindWaferRegionHomeDie();
	BOOL FindWaferHalfHomeDie();
	BOOL FindMS90HalfHomeDie();
	LONG CornerSearchHalfHomeDie();
	BOOL WftIndexAndSearchDie(LONG *siPosX, LONG *siPosY, LONG lIdxPos, LONG lSrchPos, BOOL bMoveTable, BOOL bDoComp, PR_BOOLEAN bLatch, PR_BOOLEAN bCheckDefect = PR_FALSE);
	BOOL FindGlobalAngle();
	BOOL SearchHomeDie();
	VOID SetGlobalTheta();
	LONG GetGlobalT();
	BOOL FullAutoAlignWafer(BOOL bAlignMethod, ULONG ulCornerSearchOption);
	BOOL AlignUpdatePickAndPlacePosn();
	//BOOL AutoLearnBHZPickLevel(LONG lColletPosn);

	LONG SetWftHomeDiePosition(IPC_CServiceMessage& svMsg);
	LONG ConfirmWftHomeDiePosition(IPC_CServiceMessage& svMsg);
	LONG FindWaferAngle(IPC_CServiceMessage& svMsg);
	LONG AutoAlignStandardWafer(IPC_CServiceMessage& svMsg);
    VOID SendCE_ScanSetting(VOID);
	LONG WT_GetNewGTPosition(IPC_CServiceMessage& svMsg);
	LONG WT_SetNewGTPosition(IPC_CServiceMessage& svMsg);
	LONG WT_ConfirmNewGTPosition(IPC_CServiceMessage& svMsg);
	LONG ChangeMapColorToOneColor(IPC_CServiceMessage& svMsg);
	LONG CheckClearBinResetSuffixName(IPC_CServiceMessage& svMsg);

	LONG ToggleScanRegionMarkDieCaseLog(IPC_CServiceMessage &svMsg);
	LONG ChangePosition(IPC_CServiceMessage& svMsg);
	LONG ConfirmSetup(IPC_CServiceMessage& svMsg);
	LONG CancelSetup(IPC_CServiceMessage& svMsg);
	LONG ResetCleanDirtEjectorPinCount(IPC_CServiceMessage& svMsg);
	LONG CleanDirtEjectorPinTest(IPC_CServiceMessage& svMsg);
private:
	BOOL MoveToEjPinCleanDirt();
	BOOL MoveEjectorTableToHomePosn();
	BOOL CleanDirtEjectorPin();

	BOOL IsMapNullBin(const ULONG ulRow, const ULONG ulCol);
	BOOL CreatePrepareNextRegionStatisticsThread(const UCHAR ucSortingGrade);

	// Set Event (WT_Event.cpp)
	VOID SetAutoChangeColletDone(BOOL bState);
	VOID SetEOT_Done(BOOL bState);
	VOID SetProbeZReady(BOOL bState);
	VOID SetEjectorReady(BOOL bState = TRUE, CString szLog="");
	VOID SetBhTReadyForWT(BOOL bState = TRUE);
	VOID SetWTStable(BOOL bState = TRUE);
	VOID SetWTReady(BOOL bState = TRUE, CString szTemp = "SetWTReady");
	VOID SetWTStartMove(BOOL bState = TRUE);				//v3.66	//DBH only
	VOID SetWTBacked(BOOL bState = TRUE);
	VOID SetDieReady(BOOL bState = TRUE);
	VOID SetDieReadyForBT(BOOL bState = TRUE);
	VOID SetCompDone(BOOL bState = TRUE, CString szLog = "");
	VOID SetDieInfoRead(BOOL bState = TRUE);
	VOID SetBadDie(BOOL bState = TRUE);
	VOID SetMoveBack(BOOL bState = TRUE);
	VOID SetDieBondedForWT(BOOL bState = TRUE);
	VOID SetLFReady(BOOL bState = TRUE);
	VOID SetPRStart(BOOL bState = TRUE);
	VOID SetBPRLatched(BOOL bState = TRUE);
	VOID SetWTinitT(BOOL bState = TRUE);
	VOID SetPreCompensate(BOOL bState = TRUE);				//v2.96T3
	VOID SetWTReadyForWPREmptyCheck(BOOL bState = TRUE);	//v4.54A5
	VOID SetWPREmptyCheckDone(BOOL bState = TRUE);			//v4.54A5
	VOID SetPRTesting(BOOL bState = FALSE);
	VOID SetWTStartToMoveForBT(BOOL bState = TRUE, CString szTemp = "SetWTStartToMoveForBT");

	// Wait Event (WT_Event.cpp)
	BOOL WaitEOT_Done(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitAutoChangeColletDone(INT nTimeout);
	BOOL WaitProbeZReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitEjectorReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBhTReadyForWT(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitDieReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBadDie(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitDiePicked(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitMoveBack(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitDieBondedForWT(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitDieInfoRead(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitDieUnReadyForBT(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBHInit(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitLFReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitExpInitForWt(INT nTimeout = LOCK_TIMEOUT);		//v2.63
	BOOL WaitPreCompensate(INT nTimeout = LOCK_TIMEOUT);	//v2.96T3
	BOOL WaitWprInitAFZ(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitWTReadyForWPREmptyCheck(INT nTimeout);			//v4.54A5
	BOOL WaitWPREmptyCheckDone(INT nTimeout = LOCK_TIMEOUT);//v4.54A5

	// Supporting Functions (WT_SubState.cpp)
	VOID SetEjectorVacuum(BOOL bSet);

	// Hardware status (WaferTable.cpp)
	VOID GetSensorValue();		// Get all sensor values from hardware
	VOID GetCmdValue();
	VOID GetEncoderValue();		// Get all encoder values from hardware
	VOID GetAxisInformation();	//NuMotion

	// Check Wafer Theta Motor type (Servo or Stepper)
	LONG CheckThetaMotorType(void);
	LONG CheckXMotorType(void);
	LONG CheckYMotorType(void);

	BOOL IsMotionHardwareReady();		// Check whether the motion hardware are ready

protected:

	// Axis Info
	CMSNmAxisInfo	m_stWTAxis_X;
	CMSNmAxisInfo	m_stWTAxis_Y;
	CMSNmAxisInfo	m_stWTAxis_T;
	//v4.59A39	//MS50
	CMSNmAxisInfo	m_stBHAxis_EjX;
	CMSNmAxisInfo	m_stBHAxis_EjY;

	//ES100G2	//2nd table
	CMSNmAxisInfo	m_stWTAxis_X2;
	CMSNmAxisInfo	m_stWTAxis_Y2;
	CMSNmAxisInfo	m_stWTAxis_T2;
	
	//v4.42T15
	BOOL	m_bDoCompInLastCycle;
	VOID	SetWTAtColletPos(CONST UINT unPos);

	LONG	m_lWTNoDiePosX, m_lWTNoDiePosY;		//Coor of WT No Die
	LONG	m_lLastGradeSetToEquip;

public:
	//BLOCKPICK
	LONG	m_lCurrBlockNum;		//v3.30T3
	
public:
	CEjectorPinCleanRegion *m_pEjPinCleanRegion;
	WM_CWaferMap *m_pWaferMapManager;

private:
	// Event 
	CEvent	m_evAutoChangeColletDone;
	CEvent	m_evEjectorReady;		// Ejector in the level that Wafer Table can move
	CEvent	m_evBhTReadyForWT;		//v4.46T22
	CEvent	m_evWTStable;			// Wafer Table is stable for WPR start
	CEvent	m_evWTReady;			// Wafer Table is ready for Bond Head Z down
	CEvent  m_evWTStartMove;		// For DBH only; triggers BH T to MOVE		//v3.66
	CEvent	m_evMoveBack;			// Wafer Table is required to move back
	CEvent	m_evWTBacked;			// Wafer Table is backed
	CEvent	m_evNextDie;			// Bin Table notify that next die can be get
	CEvent	m_evDieReady;			// WPR inspected a good die
	CEvent	m_evDieReadyForBT;		// Die Info is ready for BinTable
	CEvent	m_evDieInfoRead;		// Die Info is read by Bin Table
	CEvent	m_evBadDie;				// WPR inspected a bad die
	CEvent	m_evDiePicked;			// Die is picked event
	CEvent	m_evDieBondedForWT;		// Die is bonded event
	CEvent	m_evBHInit;				// Bond Head is initiated event
	CEvent	m_evWTInitT;			// T init event for wafer expander checking
	CEvent	m_evLFReady;			// WPR Look-Forward Result is ready 
	CEvent	m_evPRStart;			// WPR Start Event
	CEvent	m_evBPRLatched;			// BPR Grab Done
	CEvent  m_evExpInitForWt;		// Wait Exp down before XY home
	CEvent  m_evPreCompensate;		// Wait WaferPr to trigger pre-compensation event	//v2.96T3
	CEvent	m_evInitAFZ;			// AF Z init event for WT T and WL gripper X
	CEvent	m_evCompDone;			// WT compensation done for DBH		//v4.34T10
	CEvent	m_evWTReadyForWPREmptyCheck;	//v4.54A5	//MS60 NGPick Empty die checking
	CEvent	m_evWPREmptyCheckDone;			//v4.54A5	//MS60 NGPick Empty die checking
	CEvent	m_evAllPrescanDone;
	CEvent	m_evES101WldPreloadDone;	// in auto cycle, next prescan frame preloaded to table
	CEvent  m_evSetPRTesting;
	CEvent	m_evWTStartToMoveForBT;	//Toggle BT move for NVC

	SCN_ALN_POINT_ADJUST	m_stScnAlnPoints[WT_ALN_MAXCHECK_SCN];
	BOOL GetScnCheckAdjustOffset(LONG &lRefHomeDieRowOffset, LONG &lRefHomeDieColOffset);
	BOOL GetScnFPCMapPosition(LONG lIndex, ULONG &ulAsmRow, ULONG &ulAsmCol, LONG &lUserRow, LONG &lUserCol);

	// five points check for SCN and prescan	end
	LONG UpdateScnAlignInfo(IPC_CServiceMessage& svMsg);		// Display & update check SCN info

	CWT_BlkFunc*		m_pBlkFunc;
	CWT_BlkFunc2*		m_pBlkFunc2;		//Block2
	CWT_1stDieFinder*	m_p1stDieFinder;	//v3.13T2

	LONG	m_ulRescanAtBondDieCount;
	BOOL	m_bForceRescan;
	BOOL m_bStopBonding;

	BOOL	IsThisDieDirectPick();
	LONG	GetCurrentRow();
	LONG	GetCurrentCol();
	LONG	GetCurrentEncX();
	LONG	GetCurrentEncY();
	WAF_CDieSelectionAlgorithm::WAF_EDieAction GetCurrDieEAct();
	WAF_CDieSelectionAlgorithm::WAF_EDieAction GetLastDieEAct();
	UCHAR	GetCurrDieGrade();

	LONG	GetLastDieRow();
	LONG	GetLastDieCol();
	LONG	GetLastDieEncX();
	LONG	GetLastDieEncY();

	BOOL	UpdateRegionDieOffset(LONG lRow, LONG lCol, LONG lOffsetX, LONG lOffsetY, BOOL bToAll = FALSE);
	BOOL	GetRegionDieOffset(LONG lRow, LONG lCol, LONG &lOffsetX, LONG &lOffsetY);
	BOOL	GetRegionDiePosition( LONG lRow, LONG lCol, LONG lScanX, LONG lScanY, LONG &lOutPhX, LONG &lOutPhY);
	BOOL	UpdateAllRegionDieOffset(SHORT sTargetGrade, CONST BOOL bVerifyByFov=FALSE);
	LONG	WaferTableDoAdvSampling(IPC_CServiceMessage &svMsg);
	LONG	WprLetWftMapMathcWafer(IPC_CServiceMessage &svMsg);
	BOOL	IsTimeToUpdateAllRegionOffset();
	BOOL	IsTobeToUpdateAllRegionOffset();
	BOOL	InitAdvRegionPredication();
	LONG	GetRegionPredicationRow();
	LONG	GetRegionPredicationCol();
	BOOL	IsLFSizeOK();

	VOID LogScanDetailTime(CString szLogMsg);

	BOOL	CheckAndFindTargetDie(CONST LONG lRow, CONST LONG lCol, LONG &lTgtX, LONG &lTgtY);
	BOOL	CheckMapWaferMatch(LONG lRow, LONG lCol, LONG lOrgX, LONG lOrgY, LONG lTgtX, LONG lTgtY, CONST BOOL bAvgOffset = FALSE);
	BOOL	CheckMapAndFindWaferMatch(LONG lRow, LONG lCol, LONG &lTgtX, LONG &lTgtY, BOOL bXError, BOOL bYError, BOOL bCtrFovOnly=FALSE);
	BOOL	MatchMapWaferByMultiSearch(LONG lRow, LONG lCol, LONG &lTgtX, LONG &lTgtY, BOOL bCtrFovOnly);
	BOOL	CheckMapWaferByMultiSearch(LONG lRow, LONG lCol, LONG &lTgtX, LONG &lTgtY, BOOL bCtrFovOnly=FALSE);
	BOOL	CheckSubMapWaferByMultiSearch(LONG lRow, LONG lCol, LONG &lGetX, LONG &lGetY, BOOL bCtrBlock, INT iMatchTry);
	BOOL	LookAroundByMultiSearch(WT_LA_DIE_RESULT	&stWafDieState);
	BOOL	CheckSubMapWaferMatch(LONG lRow, LONG lCol, CString szState, BOOL bCtrBlock, BOOL &bCtrEnough);
	BOOL	WftMoveBondArmToSafe(BOOL bToSafe);
	LONG	FindExistMapDieInBlock(ULONG ulHalfL, ULONG ulHalfR, ULONG ulHalfU, ULONG ulHalfD, ULONG &ulOutRow, ULONG &ulOutCol);
	LONG	CheckExistDieInBlock(LONG lRow, LONG lCol, ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol);
	SHORT	CheckDieStatus(LONG lRow, LONG lCol);
	BOOL	PopVerifyMatchCheckMapError(LONG lAsmRow, LONG lAsmCol);

	BOOL	CheckMapWaferMatch_Align(LONG lBaseRow, LONG lBaseCol, LONG lBaseX, LONG lBaseY);
	BOOL	CheckSubMapWaferMatch_Align(INT nDir, LONG lBaseRow, LONG lBaseCol, LONG lBaseX, LONG lBaseY);
	BOOL	CheckEmptyAroundDie(LONG lRow, LONG lCol, LONG lLoopRow, LONG lLoopCol);
	BOOL	GoCheckDebMapDie(ULONG ulRow, ULONG ulCol);
	BOOL	AutoGoPitchErrorAround();
	CBinTotalWrapper m_WaferLotInfoWrapper;

	// For Electric Info
	//CString m_szSampleElectricInfo;
	BOOL	CheckHeaderElectricData(CHAR seperator = ',');
	VOID	CutElectricData( CString &szTempString , LONG &lnumber , CHAR seperator );
	BOOL	IsMapHeaderCheckOK();
	BOOL	IsMapLoadingOk();
	/*--- For Wafer Map Control ---*/

	BOOL m_bEnablePrescan_Backup;
	BOOL m_bMS90HalfSortMode_Backup;

private:

	LONG Test(IPC_CServiceMessage& svMsg);


public:
	// For Wafer Map (WT_Map.cpp)
	BOOL UpdMapLastDie(CString szWhere);
	BOOL GetMapNextDie(unsigned long &ulRow, unsigned long &ulCol, unsigned char &ucGrade,
						WAF_CDieSelectionAlgorithm::WAF_EDieAction& eAction, BOOL &bEndOfWafer);
	BOOL SetMapNextDie(const ULONG ulRow, const ULONG ulCol, 
				const UCHAR ucGrade, const WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction, 
				const ULONG ulDieState=0);


	//Set Load Map Date and Time
	VOID SetLoadMapDateAndTime(int nYear, int nMonth, int nDay, 
						       int nHour, int nMinute, int nSecond);
	VOID RotateMap(VOID);
	VOID FlipMap(VOID);
	VOID SetDieSharp(VOID);
	VOID MapEdgeSize(VOID);
	VOID DisplayNextDirection(ULONG ulRow, ULONG ulCol);
	BOOL CheckOptimizeBinCountPerWft();

	BOOL IsASCIIGradeDisplayMode();
	VOID GetWaferMapSelectedGradeList(CUIntArray &aulSelectdGradeList, CUIntArray &aulSelectdGradeLeftDieCountList);
	VOID SendWaferMapSelectedGradeList(BOOL bSendEvent = TRUE);
	VOID CreateUploadBarcodeIDFile();
	VOID UpdateSortingMode(VOID);
	VOID UpdateSortSequenceAfterScan(VOID);
	VOID ReverseSortingMode(VOID);	//xyz
	BOOL UpdateLastGradeSortingMode(VOID);				//v4.43T9	//Cree HuiZhou
	BOOL Select1stLFSortGrade();						//v4.59A15	//SanAn & Semitek
	BOOL CheckIfDisableLFSortGrade(UCHAR ucCurrGrade);	//v4.59A15	//SanAn & Semitek
	VOID UpdateWaferMapControlTitle(LONG lPageNo);
	BOOL MatchMapFile(CString szFilePath);
	VOID SetAlignmentStatus(BOOL bFinish);
	BOOL CheckGradeMappingStatus(BOOL bForceCheck=FALSE);		//v4.50A11	//Cree HuiZhou GradeMapping fcn
	BOOL IsAlignedWafer();
	BOOL AutoLoadMap(CString szMapFullName, CString szFormatName, BOOL bMenuLoad = FALSE);		//v4.40T8
	BOOL DeleteCurrentMapPsmScn();
	VOID WaferMapOnModifyDieAction();

	// Store the wafer map algorithm settings
	VOID SaveWaferMapAlgorithmSettings();
	VOID LoadWaferMapAlgorithmSettings();

	BOOL SetCtmRefDieOptions();			//v3.02T5

	UCHAR GetPsmLoadAction();

	//For SCN data use	begin
	BOOL InitScnData(VOID);
	BOOL SetScnRefDie(ULONG ulRow, ULONG ulCol, DOUBLE dScnTableTheta);
	BOOL GetScnRefDie(ULONG *ulRow, ULONG *ulCol);
	BOOL SetScnData(ULONG ulRow, ULONG ulCol, LONG lX, LONG lY, UCHAR ucGrade);
	BOOL GetScnData(ULONG ulRow, ULONG ulCol, LONG *lX, LONG *lY);
	VOID ConvertWSDataToPrescanAndMap();

	BOOL GetScnData_Refer(ULONG ulRow, ULONG ulCol, LONG &lX, LONG &lY);
	VOID ConvertWSDataToMS_Refer();
	BOOL ConvertScanPosnByNear(ULONG ulDieRow, ULONG ulDieCol, LONG &lPhyPosX, LONG &lPhyPosY);
	BOOL ConvertPhysicalPos_Refer(ULONG ulCurrRow, ULONG ulCurrCol, LONG &lPhyPosX, LONG &lPhyPosY);
	BOOL ScnAlignPoint_Refer(LONG ulDieRow, LONG ulDieCol, ULONG ulGridLoop);
	VOID ScnAlignSpiralToOuter();

	BOOL ConvertScnPosnByFPC(ULONG ulDieRow, ULONG ulDieCol, LONG &lPhyPosX, LONG &lPhyPosY, LONG &lScnIndex, BOOL bUseAngle);
	BOOL ScanLShapeDieB(LONG lDiffRow, LONG lDiffCol);
	VOID ScanAlignWithFivePoints();
	BOOL ConvertScanPosnByQuadrant(ULONG ulDieRow, ULONG ulDieCol, LONG &lPhyPosX, LONG &lPhyPosY, LONG &lIndex);
	VOID ObtainMS_ES_Angle(DOUBLE lMsAB_X, DOUBLE lMsAB_Y, DOUBLE lEsAB_X, DOUBLE lEsAB_Y, DOUBLE &dSinTheta, DOUBLE &dCosTheta);
	BOOL ConvertScanPosnByTheta(ULONG ulDieRow, ULONG ulDieCol, LONG &lPhyPosX, LONG &lPhyPosY);
	VOID ScanAlignInToOutSpiral();
	BOOL ScanAlignPoint(LONG ulDieRow, LONG ulDieCol, BOOL bCheckInner=TRUE);
	BOOL IsInnerDie(LONG lDieRow, LONG lDieCol, ULONG ulStart, ULONG ulEnd);
	VOID CalculateES101Angle(LONG lScanAX, LONG lScanAY, LONG lScanBX, LONG lScanBY, DOUBLE &dAngleX, DOUBLE &dAngleY);
	BOOL DumpScnData(BOOL bDumpFile);
	VOID FivePointCheckLog(CString szLogMsg);
	LONG WPR_ZoomSensor(LONG lZoomMode);
	BOOL FivePointCheckDiePosition(VOID);
	BOOL WaferCornerPatternCheck();
	BOOL PrescanGoodDiePercentageCheck();

	VOID GetMapOffset(LONG &lRowOffset, LONG &lColOffset);
	VOID GetAsmMapHomeDie(ULONG &ulAlignRow, ULONG &ulAlignCol, BOOL bAlarm = FALSE, BOOL bSecondPart=FALSE);
	VOID GetMapAlignHomeDie(ULONG &ulHomeRow, ULONG &ulHomeCol, BOOL bAlarm = TRUE);
	BOOL GetKeepScnAlignDie();
	BOOL ScnCheckDiePrResult(LONG lScnCheckRow, LONG lScnCheckCol, BOOL &bPromptMessage, CString &szText, CString &ListErrorDie, INT &ErrorCount);
	BOOL GetFPCMapPosition(LONG lIndex, ULONG &ulAsmRow, ULONG &ulAsmCol, LONG &lUserRow, LONG &lUserCol);
	BOOL GetDieValidScnPosn(LONG ulRow, LONG ulCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY);
	BOOL CheckSCNFileLoaded();
	//Check SCN file is loaded
//For SCN data use	end

	BOOL AddBcHistoryInCurrLot(CONST CString szBCName);		//PLLM v3.74T31
	BOOL CheckBcHistoryInCurrLot(CONST CString szBCName);	//PLLM v3.74T31

	//-------- For Wafer Lot File ---------//
	BOOL SaveWaferLotData();
	BOOL SaveWaferLotData_Osram();
	CString GetPathByFileDialog(CString szDefaultPath, CString szDefaultFileName, CString szDefaultExt); // Yealy
	BOOL ReadLIFFile(CString szFile);
	BOOL ReadLIFFileZVision(CString szFile);
	BOOL ReadLextarLotFile(CString szFileName, CString& szBinSummaryFilename,CString& szPKGFilename);
	//BOOL ReadCSVFile(CString szFile);
	BOOL ShowWaferLotData();
	VOID UpdateWaferLotLoadStatus();
	BOOL CheckAllWaferLoaded(CStringArray& szaWaferIds);
	BOOL CompareWaferLotDatabase(CString szMapFileName);
	BOOL SearchWaferIDAndSaveDataFromXML(CString szWaferID);
	BOOL MakeWaferIDListFromXML(CString szLocalXMLPath);

	VOID SetWaferId(CString szWaferId);

	VOID SetLoadCurrentMapStatus(BOOL bSet);
	BOOL GetLoadCurrentMapStatus();
	BOOL CheckLoadCurrentMapStatus();
	VOID SetIgnoreGradeCountCheckStatus(BOOL bSet);

	BOOL DynCreateHeaderAndOutputFileFormat();
	BOOL ReadDynWaferMapOutputFileRefFile();
	BOOL DynCreateWaferMapHeaderFile(CStringArray& szaColHead);
	BOOL DynCreateWaferMapColumnsFile(CStringArray& szaColHead);
	BOOL DecodeColumnHeadInMapFile(CStringArray& szaColHead);

	BOOL IsDisableWaferMapGradeSelect();
	VOID CP_SyncItemMapViewer(UCHAR ucMapSource);
	VOID WriteUserParameterLog(CString szMessage);

	//Scan Region Mark Die
	BOOL MD_SearchNormalDie(LONG &lPosX, LONG &lPosY, LONG &lOffsetX_Count, LONG &lOffsetY_Count, BOOL bDoComp);
	BOOL ScanRegionMarkDie();
	VOID UpdateAllDiePosnByMarkDiePosn();

	LONG GetWaferSortPart();	//	1, 1st half; 2, 2nd halft; other, abnormal by check current WFT Theta encoder to ideal wafer orientation in 1st or 2nd part.
	BOOL SwitchColletOffset(BOOL bBHZ2,BOOL bAddOffset);
	BOOL GoToNoDiePos(INT nMode = SFM_WAIT);
	BOOL GoTo(LONG lX, LONG lY, INT nMode = SFM_WAIT);
	BOOL WTMove(LONG lX, LONG lY, INT nMode = SFM_WAIT);
private:

	// ===========================================================
	// WaferTable HMI Commands
	LONG LogItems(IPC_CServiceMessage& svMsg);

	LONG TeachWaferLimit(IPC_CServiceMessage &svMsg);
	LONG CancelWaferLimit(IPC_CServiceMessage &svMsg);
	LONG ShowWaferLimit(IPC_CServiceMessage &svMsg);

	LONG LearnCOR();
	LONG TeachCalibration(IPC_CServiceMessage &svMsg);
	LONG StopCalibration(IPC_CServiceMessage &svMsg);

	LONG RemoveAllSCNPSN(IPC_CServiceMessage &svMsg);

	LONG RotateAngularDie(IPC_CServiceMessage &svMsg);
	LONG IsPickAndPlaceOn(IPC_CServiceMessage &svMsg);
	LONG UpdatePickAndPlacePos(IPC_CServiceMessage &svMsg);
	LONG UpdatePickAndPlacePath(IPC_CServiceMessage &svMsg);
	LONG UpdatePickAndPlaceGrade(IPC_CServiceMessage &svMsg);
	LONG CheckPickAndPlaceGrade(IPC_CServiceMessage &svMsg);
	BOOL CheckDummyBinGradeHasNoDie(UCHAR ucGrade);
	LONG EnablePickAndPlace(IPC_CServiceMessage &svMsg);
	LONG MoveToUnload(IPC_CServiceMessage& svMsg);
	LONG SetUnloadPosition(IPC_CServiceMessage& svMsg);
	LONG ConfirmUnloadPosition(IPC_CServiceMessage& svMsg);
	LONG MoveToUnloadLoad(IPC_CServiceMessage& svMsg);
	LONG PreStartCheckGlobalTheta(IPC_CServiceMessage& svMsg);
	LONG MoveWaferTableToSafePosn(IPC_CServiceMessage& svMsg);		//v3.61
	LONG MoveWftToSafePosn(BOOL bSafe);
	LONG MoveWaferTableToSafePosn2(IPC_CServiceMessage& svMsg);		//v3.61
	LONG m_lBpX, m_lBpY, m_lBpT;
	
	//ES101 dual-table configuration
	LONG TeachCalibration2(IPC_CServiceMessage &svMsg);
	LONG StopCalibration2(IPC_CServiceMessage &svMsg);
	LONG TeachWT2OffsetXY(IPC_CServiceMessage &svMsg);				//v4.24T10
	LONG TeachNGPickOffsetXY(IPC_CServiceMessage &svMsg);			//v4.24T10
	LONG TestNGPick(IPC_CServiceMessage &svMsg);						//v4.28
	LONG ResetWTInUse(IPC_CServiceMessage& svMsg);					//v4.24T12
	LONG MoveTable1ToEjtOffset(IPC_CServiceMessage& svMsg);			//v4.28
	LONG MoveTable1ToHome(IPC_CServiceMessage& svMsg);				//v4.28

	//Collet Offset
	LONG MoveWToffset(IPC_CServiceMessage& svMsg);

	//Draw Picture
	LONG TestXMLFunction(IPC_CServiceMessage& svMsg);
	LONG GenerateWaferDiagram(IPC_CServiceMessage& svMsg);
	VOID GenerateTabletPicture();
	LONG ChangeBoundaryDieColour(IPC_CServiceMessage& svMsg);
	LONG ChangeNGDieColour(IPC_CServiceMessage& svMsg);
	LONG ChangeGridLineColour(IPC_CServiceMessage& svMsg);
	BOOL ES101SetPngGridLineColour(COLORREF &ulGridColourArray, const BOOL &bGridColourOn);
	BOOL GetColorByColourDialog(COLORREF &ulReturnColour);
	BOOL ReformTheGridColourArray(CUIntArray &aulColourArray);

	//Reset align point
	BOOL CP_ResetTheAlignOffset(IPC_CServiceMessage& svMsg);
	int  GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
	Gdiplus::GdiplusStartupInput	m_gdiplusStartupInput;
	ULONG_PTR	m_gdiplusToken;

	//	CP_HC
	BOOL	InitUpDownCheckItems();
	BOOL	GetTestingItems(CStringArray &szaTestItemList);
	BOOL	CheckRuntimeUpRowProbeResult();
	BOOL	GetCP100GainAndOffset(CString &szValueString); // nanojoin
	BOOL	LoadCreatePSCMapFile();

	// Key Files Operation
	LONG	CpLoadMapGradeColorFile(IPC_CServiceMessage& svMsg);
	LONG	CpSaveMapGradeColorFile(IPC_CServiceMessage& svMsg);
	LONG	CpLoadMapDisplayItemsFile(IPC_CServiceMessage& svMsg);
	LONG	CpOutputMapHeaderPreTask(IPC_CServiceMessage &svMsg);
	LONG	CpSaveOutputMapColumns(IPC_CServiceMessage &svMsg);

	LONG	IntoItemGradeSetupPage(IPC_CServiceMessage& svMsg);
	LONG	ConfirmBinMapItemRange(IPC_CServiceMessage& svMsg);

	// CP100 ACP cmd
	LONG	SetCleanPinUpLeftCorner(IPC_CServiceMessage &svMsg);
	LONG	MoveTableToCleanPinPoint(IPC_CServiceMessage& svMsg);
	LONG	MoveTableToCleanPinTheta(IPC_CServiceMessage& svMsg);

	//Align wafer	
	BOOL AOISpiralSearchOcrDie(LONG &lStartX, LONG &lStartY, CString &szOcrValue);
	BOOL AOISearchOcrDie(LONG &lOffsetX, LONG &lOffsetY, CString &szOcrPrValue, CString &szMsg);
	BOOL AOIFindBarwaferOCRInRow();
	BOOL ManualAlignWaferMap(BOOL bSecondTime);
	LONG AutoAlignWafer(IPC_CServiceMessage &svMsg);
	LONG ManualAlignWafer(IPC_CServiceMessage &svMsg);
	LONG UpdateGThetaMode(IPC_CServiceMessage &svMsg);
	LONG UpdateHomeDiePhyPosition(IPC_CServiceMessage &svMsg);
	LONG UpdateSearchHomeOption(IPC_CServiceMessage &svMsg);
	LONG UpdateCornerSearchOption(IPC_CServiceMessage& svMsg);
	LONG ReprobeAtSelectMap(IPC_CServiceMessage& svMsg);
	LONG ReprobeWholeMap(IPC_CServiceMessage& svMsg);
	LONG CheckIsBlockProbe(IPC_CServiceMessage& svMsg);

	//Align wafer (called from other stations)	
	LONG AlignInputWafer(IPC_CServiceMessage& svMsg);
	LONG CheckRepeatMap(IPC_CServiceMessage& svMsg);				//PLLM v3.74T33
	//Output file generating
	LONG WaferMapHeadersPreTask(IPC_CServiceMessage &svMsg);
	LONG OnClickNoOfMapColsInput(IPC_CServiceMessage &svMsg);
	LONG LoadWaferMapColumnsFile(IPC_CServiceMessage &svMsg);
	LONG SaveWaferMapColumnsFile(IPC_CServiceMessage &svMsg);
	LONG OnClickChangeHeaderPage(IPC_CServiceMessage &svMsg);
	LONG OutputFileFormatPreTask(IPC_CServiceMessage &svMsg);
	LONG OnClickNoOfOutputColsInput(IPC_CServiceMessage &svMsg);
	LONG OnClickChangeOutputFilePage(IPC_CServiceMessage &svMsg);
	LONG LoadOutputFileFormatFile(IPC_CServiceMessage &svMsg);
	LONG SaveOutputFileFormatFile(IPC_CServiceMessage &svMsg);
	BOOL SetDummyMapForNichia(IPC_CServiceMessage &svMsg);
	// Rotation Test
	LONG StartRotationTest(IPC_CServiceMessage &svMsg);
	LONG StopRotationTest(IPC_CServiceMessage &svMsg);
	LONG StartPickAndPlace(IPC_CServiceMessage& svMsg);
	LONG StartBurnIn(IPC_CServiceMessage &svMsg);
	LONG WTBurnInSetup(IPC_CServiceMessage &svMsg);
	LONG SearchFilePath(IPC_CServiceMessage &svMsg);
	LONG BrowseLoadMapFile(IPC_CServiceMessage &svMsg);
	LONG LoadMapWithBarCode(IPC_CServiceMessage& svMsg);		//Bar Code Reader
	LONG LoadFixMap(IPC_CServiceMessage& svMsg);				//Load FixMap
	LONG LoadMapWithBarCodeName(CString szMapBCFileName);
	LONG LoadMapWithBarCodeName_PLLM(CString szMapBCFileName);	//v4.39T7
	CString GetMapFilePath();
	//Map file
	LONG SaveMapFilePath(IPC_CServiceMessage& svMsg);
	LONG SaveMapFileExt(IPC_CServiceMessage& svMsg);
	LONG IsWaferAligned(IPC_CServiceMessage& svMsg);			//Check whether the wafer is aligned
	LONG GetMapPath(IPC_CServiceMessage &svMsg);				// Get the map path via pop-up dialog
	LONG GetBackupMapPath(IPC_CServiceMessage &svMsg);			// Get the backup map path via pop-up dialog
	LONG GetUploadBarcodeIDPath(IPC_CServiceMessage &svMsg);
	LONG GetPrescanBackupMapPath(IPC_CServiceMessage &svMsg);	// Get the prescan backup map path via pop-up dialog
	LONG GetPrescanSummaryPath(IPC_CServiceMessage &svMsg);		// Get the prescan summary output path via pop-up dialog

	LONG AutoCaptureScreenInCleanPin(IPC_CServiceMessage &svMsg);

	LONG GetMA1FilePath(IPC_CServiceMessage &svMsg);		// Get the MA1 file path via pop-up dialog
	LONG InputSaveMapImagePath(IPC_CServiceMessage &svMsg);
	LONG GetSaveMapImagePath(IPC_CServiceMessage &svMsg);
	LONG InputLogPrNgImagePath(IPC_CServiceMessage &svMsg);
	LONG GetLogPrNgImagePath(IPC_CServiceMessage &svMsg);
	LONG SelectMA3Format(IPC_CServiceMessage &svMsg);
	LONG SetHmiOperationLogPath(IPC_CServiceMessage &svMsg);
	LONG GetHmiOperationLogPath(IPC_CServiceMessage &svMsg);

	
	LONG GoSampleCheckScnPoints(IPC_CServiceMessage& svMsg);
	LONG LoadAllMapNewReferDie(IPC_CServiceMessage& svMsg);
	LONG ClearWaferMap(IPC_CServiceMessage &svMsg);	// Get the prescan backup map path via pop-up dialog
	LONG SetMapFileName(IPC_CServiceMessage &svMsg);			// Set Map file name from other station
	LONG UpdateCheckSCNInfo(IPC_CServiceMessage& svMsg);		// Display & update check SCN info
	LONG GoToAllSCNDiePos(IPC_CServiceMessage& svMsg);
	LONG LoadAllScnCheckPosn(IPC_CServiceMessage& svMsg);
	LONG UpdateRefCrossInfo(IPC_CServiceMessage& svMsg);		// Update Reference cross parameter
	LONG UpdateGradeDisplayMode(IPC_CServiceMessage& svMsg);
	LONG SetWaferGlobalAnlge(IPC_CServiceMessage& svMsg);		// Set Wafer Global Angle
	LONG UpdateSubRegionInfo(IPC_CServiceMessage& svMsg);		// Update Sub Region Info
	LONG UpdateIgnoreRegionInfo(IPC_CServiceMessage& svMsg);	// Update Ignore Region Info
	LONG UpdateSelectRegion(IPC_CServiceMessage &svMsg);		//3.50T1
	LONG VerifySelectRegion(IPC_CServiceMessage &svMsg);		//3.49T5
	LONG EnableMapSyncMove(IPC_CServiceMessage& svMsg);			// Enable/Disable Map sync move 
	LONG UpdateMapGradeColor(IPC_CServiceMessage &svMsg);		//Map grade color option
	LONG ResetGradeMapping(IPC_CServiceMessage &svMsg);			//Grade mapping function
	//Set CEID
	LONG SetStartBondEvent(IPC_CServiceMessage &svMsg); 
	LONG SetStopBondEvent(IPC_CServiceMessage &svMsg);
	// Diagnostic Function
	LONG Diag_PowerOn_X(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_Y(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_T(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_X2(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_Y2(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_T2(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_All(IPC_CServiceMessage &svMsg);
	//4.24
	LONG Diag_Comm_X(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_Y(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_T(IPC_CServiceMessage &svMsg);
	
	LONG Diag_Comm_X2(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_Y2(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_T2(IPC_CServiceMessage &svMsg);

	LONG Diag_Home_X(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_Y(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_T(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_X2(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_Y2(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_T2(IPC_CServiceMessage &svMsg);
	
	LONG Diag_Move_X(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_Y(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_T(IPC_CServiceMessage &svMsg);

	LONG Diag_Move_X2(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_Y2(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_T2(IPC_CServiceMessage &svMsg);

	LONG UpdateOutput(IPC_CServiceMessage& svMsg);
	// Burn-In Map File Path
	LONG CheckBurnInMapFilePath(IPC_CServiceMessage& svMsg);
	LONG GetBurnInMapFilePath(IPC_CServiceMessage& svMsg);
	//SCN Function
	LONG GoToSCNDiePos(IPC_CServiceMessage& svMsg);
	LONG GoToMapPos(IPC_CServiceMessage& svMsg);			//v3.01T5
	//Set Wafer table working angle
	LONG SetWorkingAngle(IPC_CServiceMessage &svMsg);
	LONG ConfirmWorkingAngle(IPC_CServiceMessage &svMsg);
	LONG EndLotClearMap(IPC_CServiceMessage &svMsg);
	//Manual rotate wafer theta motor
	LONG ManualRotateWaferTheta(IPC_CServiceMessage &svMsg);
	LONG ManualRotateWaferTheta2(IPC_CServiceMessage &svMsg);

	LONG SetDisableRefDie(IPC_CServiceMessage &svMsg);		//v2.78T1
	LONG GetOutWaferInfoFilePath(IPC_CServiceMessage& svMsg);
	LONG UpdateWaferMapPath(IPC_CServiceMessage &svMsg);
	//------------- Command For Wafer Lot File ---------------//
	LONG GetWaferLotInfoFilePath(IPC_CServiceMessage& svMsg);
	LONG EnableCheckWaferName(IPC_CServiceMessage& svMsg);
	LONG ReadWaferLotInfoFile(IPC_CServiceMessage& svMsg);
	LONG ReadWaferLotInfoFile_SECSGEM(IPC_CServiceMessage& svMsg);		//OsramTrip 8/22
	LONG ShowWaferLotDataCmd(IPC_CServiceMessage& svMsg);
	LONG ClearWaferLotDataCmd(IPC_CServiceMessage& svMsg);
	LONG UpdateWaferLotLoadStatusCmd(IPC_CServiceMessage& svMsg);
	LONG CheckAllWaferLoadedCmd(IPC_CServiceMessage& svMsg);
	LONG CheckWaferLotDataInManualMode(IPC_CServiceMessage& svMsg);
	LONG CheckWaferLotDataInAutoMode(IPC_CServiceMessage& svMsg);
	//---- Moving the Wafer Table for searching the barcode ---//
	LONG ReadBarCodeOnTable(IPC_CServiceMessage& svMsg);
	LONG ReadBarCodeOnTable2(IPC_CServiceMessage& svMsg);
	LONG X_SyncCmd(IPC_CServiceMessage& svMsg);
	LONG Y_SyncCmd(IPC_CServiceMessage& svMsg);
	LONG T_SyncCmd(IPC_CServiceMessage& svMsg);
	LONG XY_SyncCmd(IPC_CServiceMessage& svMsg);			
	LONG UpdateDirectCornerSearchPos(IPC_CServiceMessage& svMsg);
	LONG DirectCornerSearchDie(IPC_CServiceMessage& svMsg);
	LONG IsUpperLeftCornerComplete(IPC_CServiceMessage& svMsg);
	LONG ClearMapDisplayFilename(IPC_CServiceMessage& svMsg);
	LONG UpdateMapFileName(IPC_CServiceMessage& svMsg);
	LONG UpdateNewEjrSeqOption(IPC_CServiceMessage& svMsg);
	// Others cmds
	LONG TestFcn(IPC_CServiceMessage& svMsg);
	LONG UpdateSortingCheckPointsOption(IPC_CServiceMessage& svMsg);
	LONG LoadWaferMapHeader(IPC_CServiceMessage &svMsg);
	LONG LoadOutputFileFormat(IPC_CServiceMessage &svMsg);
	//------------- SWalk 1st-Die Finder cmds ---------------//
	LONG Start1stDieFinderTest(IPC_CServiceMessage& svMsg);			//v3.13T2
	LONG Stop1stDieFinderTest(IPC_CServiceMessage& svMsg);			//v3.13T2
	LONG Start1stDieAFinderTest(IPC_CServiceMessage& svMsg);		//v3.13T2
	LONG Stop1stDieAFinderTest(IPC_CServiceMessage& svMsg);			//v3.13T2

	LONG WaferMapHeaderCheckingCmd(IPC_CServiceMessage& svMsg);
	LONG CheckLoadCurrentMapCmd(IPC_CServiceMessage& svMsg);

	LONG LoadWaferMapErrorCheckingCmd(IPC_CServiceMessage& svMsg);

	// -------- MES Connector Command ----------------//
	LONG EnableMESConnection(IPC_CServiceMessage& svMsg);
	LONG GetMESIncomingPath(IPC_CServiceMessage& svMsg);
	LONG GetMESOutgoingPath(IPC_CServiceMessage& svMsg);
	LONG SetMESTimeout(IPC_CServiceMessage& svMsg);

	// Command for rotate the wafer after loaded the frame
	LONG RotateWFTTUnderCam(CONST BOOL bWT2);
	LONG WFTTGoHomeUnderCam(CONST BOOL bWT2);
	LONG AutoRotateWaferT1(BOOL bKeepBarcodeAngle);
	LONG AutoRotateWaferT2(BOOL bKeepBarcodeAngle);
	LONG AutoRotateWafer(IPC_CServiceMessage& svMsg);
	LONG AutoRotateWafer2(IPC_CServiceMessage& svMsg);
	BOOL AutoCheckWaferRotation(LONG lEncT);
	BOOL AutoCheckWaferRotation2();
	LONG AutoRotateWaferT1ForBC(BOOL bKeepBarcodeAngle);			//v4.46T20	//PLSG
	LONG AutoRotateWaferForBC(IPC_CServiceMessage& svMsg);			//v4.46T20	//PLSG
	BOOL AutoCheckWaferRotationForBC(LONG lEncT);					//v4.46T20	//PLSG
	LONG RotateWaferCmd(IPC_CServiceMessage& svMsg);
	LONG MS90SortTo2ndHalf(IPC_CServiceMessage& svMsg);
	LONG MS90SetWaferLimitY(IPC_CServiceMessage& svMsg);
	LONG MS90Set2ndHomePosition(IPC_CServiceMessage& svMsg);
	VOID MS90Set1stDone(BOOL bSet);

	INT  GetAutoWaferT();
	BOOL TwoDimensionsBarcodeChecking(BOOL bAuto=TRUE);	//v3.33T1
	
	//Block Function	//Blcok2
	LONG SwitchBlockFuncPage(IPC_CServiceMessage& svMsg);
	LONG CalculateCOROffset(IPC_CServiceMessage &svMsg);			//v3.44		//ShowaDenko

	LONG RemoveLotInfoFile(IPC_CServiceMessage &svMsg);

	BOOL ResetAlignStatus(IPC_CServiceMessage &svMsg);

	//Generate Log file when missing reference is detected during auto-bond 	(block2)
	BOOL ExportRefCheckFailFile(IPC_CServiceMessage &svMsg);

	// Axis Test commands
	LONG StartWTXMotionTest(IPC_CServiceMessage& svMsg);
	LONG StartWTYMotionTest(IPC_CServiceMessage& svMsg);
	// 4.24TX1
	LONG StartWT2XMotionTest(IPC_CServiceMessage& svMsg);
	LONG StartWT2YMotionTest(IPC_CServiceMessage& svMsg);

	LONG PkgKeyParametersTask(IPC_CServiceMessage &svMsg);
	LONG GeneratePkgDataFile(IPC_CServiceMessage &svMsg);		//v4.22T1	//Walsin China
	LONG GenerateConfigData(IPC_CServiceMessage &svMsg);		//v4.24T5
	BOOL GenerateConfigData();									//v4.24T5

	LONG FaceValueExistsInMap(IPC_CServiceMessage& svMsg);		//v4.48A26	//Avago

	LONG UpdateWaferDiameter(IPC_CServiceMessage& svMsg);		//v4.49A1	//MS90

	BOOL MoveToUnloadLoadDualTable();

	// Axis Test functions
	BOOL MotionTest(VOID);

	VOID SetLockJoystick(BOOL bLock);
	BOOL GetLockJoystick();

	// Check Ejt Life Time
	BOOL WTCheckEjtLife();

	LONG WTChangeNoDiePosition(IPC_CServiceMessage& svMsg);
	LONG WTConfirmNoDiePositionSetup(IPC_CServiceMessage& svMsg);
	LONG SaveSCNData(IPC_CServiceMessage& svMsg);
	LONG CheckSamplingRescanIgnoreGradeList(IPC_CServiceMessage& svMsg);
	LONG WTMoveFromTempFile(IPC_CServiceMessage &svMsg);		//andrewng //2020-0630
	LONG MoveTableToDummyPos(IPC_CServiceMessage &svMsg);		//andrewng //2020-0727
	LONG m_lBackupDummyPosX;
	LONG m_lBackupDummyPosY;


protected:

    VOID InitVariable(VOID);
	BOOL SetWaferIndexingPathParameters();
	BOOL LoadWaferTblData(CONST BOOL bOnlyPPKG);
	BOOL SaveWaferTblData(VOID);
	BOOL UpdateTablePosition(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree);
	BOOL CalculateRotateWaferTheta(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree, LONG lCalibX, LONG lCalibY);
	BOOL RotateWaferTheta(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree);
	BOOL RotateWaferThetaAngle(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree, BOOL bIsWT2);
	BOOL IsWithinWaferLimit(LONG lX, LONG lY, DOUBLE dScale = 1, BOOL bUsePosOffset=FALSE);
	BOOL IsWithinMapDieLimit(LONG lX, LONG lY, DOUBLE dScale = 1);
	LONG GetCircleWaferMinX();
	LONG GetCircleWaferMaxX();
	LONG GetCircleWaferMinY();
	LONG GetCircleWaferMaxY();

	LONG SetWaferLimit(LONG lPoint);
	LONG MoveWaferLimit(VOID);
	LONG SetPolygonWaferLimit(VOID);
	LONG MovePolygonWaferLimit(VOID);

	LONG OnSelectWaferLimit(UCHAR ucTemplateNo);

	LONG MoveWaferTheta(VOID);
	LONG StartWaferCalibration(VOID);
	LONG StopWaferCalibration(LONG lStop);

	LONG MoveWafer2Theta(VOID);
	LONG StartWaferCalibration2(VOID);
	LONG StopWaferCalibration2(LONG lStop);

	BOOL NVC_MotionTest(VOID);

	/////////////////////////////////////////////////////
	// Wafermap ALIGNMENT Functions
	/////////////////////////////////////////////////////

	//Align wafer function
	LONG SetAlignStartPoint(BOOL bDoPR, LONG lCurrX, LONG lCurrY, unsigned long ulCurrRow, unsigned long ulCurrCol);
	BOOL GetAlignStartPoint(LONG *lCurrX, LONG *lCurrY, unsigned long *ulCurrRow, unsigned long *ulCurrCol);
	BOOL AligningWafer(VOID);
	BOOL ManualAlignGlobalTheta(ULONG ulStartMapRow, ULONG ulStartMapCol, BOOL bHomeT=FALSE);		//v4.42T12	//PLLM
	LONG SetAlignIndexPoint(BOOL bDoPR, LONG lDirection, LONG lStepSize, BOOL bCheckMapAndGrade=TRUE, BOOL bCheckNullGrade=FALSE);
	LONG AlignNormalWafer(VOID);
	LONG AlignBlockWafer(VOID);
	LONG AlignRectWafer(ULONG ulCornerSearchOption = 0);
	LONG AlignRectWafer_Resort_000();
	LONG AlignRectWafer_Resort_090();
	LONG AlignRectWafer_Resort_180();
	LONG AlignRectWafer_Resort_270();
	BOOL AlignRectWaferURCorner(CONST LONG lOption);		//v4.21T3	//Osram RESORT
	BOOL AlignRectWaferLowerCorner(CONST BOOL bIsLRCorner, CONST ULONG ulAlignRow, CONST ULONG ulAlignCol);		//v4.13T5
	BOOL AlignAllRefDiePositions();		//v3.03
	BOOL AlignU2UReferenceDie();
	LONG ManualAlignU2UReferenceDie();
	BOOL FindLeftmostCreeHorz4RefDice(LONG& lEncX, LONG& lEncY);	//v3.92		//Cree US Waffle Pack machine buyoff
	BOOL ManualLocate3Corners();		//v3.74T23	//PLLM REBEL & FLASH
	BOOL RebelManualAskPasswordToGo();
	LONG CheckRebel_N2RC_Position(ULONG ulCornerType);
	LONG CheckRebel_N2R4_Relation(ULONG ulCornerType);
	BOOL CheckRebel_NormalOut(ULONG ulCornerType);

	//** specific map ALIGN patterns for fully-auto wafer alignment purpose **//
	//NeoNeon	//v3.42T1
	BOOL SearchAndAlignPattern();	
	BOOL CheckCreeHorz4RefDice();				//v3.82		//Cree US Waffle Pack machine buyoff

	BOOL SearchRectPatternFromMap(ULONG& ulULRow, ULONG& ulULCol, eWT_Pattern ePattern=P3x3, 
		CONST UCHAR ucPatternNum=1, CONST BOOL bIsPartialWafer=FALSE);	//v3.64
	BOOL Get3x3HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST LONG lStartRow=0, CONST LONG lStartCol=0);
	BOOL Get3x4HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST LONG lStartRow=0, CONST LONG lStartCol=0);
	BOOL Get4x3HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST LONG lStartRow=0, CONST LONG lStartCol=0);
	BOOL Get1x3HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST LONG lStartRow=0, CONST LONG lStartCol=0);
	//SUnrich	//v3.64
	BOOL FindSunrich1x3AlignPatternFromMap();	//Type #1023 
	BOOL FindSunrich3x3AlignPatternFromMap();	//Type #1212
	BOOL FindSunrich3x4AlignPatternFromMap();	//Type #1213
	// BYD auto search home die if it is enable
	BOOL FindAutoAlignHomeDieFromMapBYD(BOOL bAlarm=TRUE);
	BOOL FindAutoAlignHomeDieFromMap(BOOL bAlarm=TRUE);
	BOOL AutoFindHomeDieFromMap(BOOL bAlarm=TRUE);
	BOOL FindReferDieFromMapSemitek(BOOL bAlarm=TRUE);
	BOOL FindReferDieFromMapSanAn(BOOL bAlarm=TRUE);
	BOOL FindReferDieFromMapRenesas();
	//Soraa map
	BOOL MoveSoraaMapByOneDieUpDnFromHomeDie(BOOL& bIsDnDie);	//v4.08		//Soraa
	BOOL MoveSoraaOneDieUpDnFromAlign(BOOL& bIsDown);
	BOOL SearchAlignDieSoraaDie(CONST BOOL bNormalDie, CONST BOOL bSrchWnd, CONST BOOL bDoComp, CONST BOOL bRotateSearch);
	//Nichia TEG align fcn
	BOOL FindNichiaTEGPatternFromMap(ULONG& ulAsmRow, ULONG& ulAsmCol, CONST UCHAR ucIndex, 
		CONST BOOL bCreate=FALSE, CONST BOOL bPartialWafer=FALSE);					//v4.41T5
	BOOL GetNichia8030HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST UCHAR ucTEGType=0, CONST LONG lStartRow=0, CONST LONG lStartCol=0);
	BOOL GetNichia8060HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST UCHAR ucTEGType=0, CONST LONG lStartRow=0, CONST LONG lStartCol=0);	//v4.40T13
	BOOL WriteNichiaTEGPatternToAlignFile(CONST ULONG ulStartRow, CONST ULONG ulStartCol, CONST BOOL bAppendFile=FALSE, 
												CONST BOOL bPartialWafer=FALSE);	//v4.41T5
	BOOL FindNichiaTEGRefDieOnMap(CONST UCHAR ucIndex, ULONG& ulRow, ULONG& ulCol);

	LONG ConvertUnitToMotorStep(DOUBLE dUnit);

	//SCN function
	VOID ScnAlignLog(CString szMsg);
	LONG AlignScannerWafer(VOID);
	LONG AlignScannerWafer_Manual(VOID);
	LONG AlignScannerWafer_Refer(VOID);
	BOOL ConvertPhysicalPos(unsigned long ulCurrRow, unsigned long ulCurrCol, LONG *lPhyPosX, LONG *lPhyPosY);
	BOOL CalculateWS896Angle(VOID);
	BOOL IndexToSCNOffsetPos(ULONG *ulStartRow, ULONG *ulStartCol);

	BOOL SearchDieInSearchWindow(BOOL bShowPrStatus, BOOL bNormalDie, LONG lRefDieNo, BOOL bDisableBackupAlign, REF_TYPE& stInfo);
	BOOL SearchCurrentDie(BOOL bShowPrStatus, BOOL bNormalDie, LONG lRefDieNo, BOOL bDisableBackupAlign, REF_TYPE& stInfo);


	/////////////////////////////////////////////////////
	// MAPPING Functions
	/////////////////////////////////////////////////////
	
	//Cross check function
	BOOL FreeCrossCheck(VOID);
	
	//Reference cross function
	LONG SetupReferenceCross(VOID);
	LONG CheckReferenceCross(VOID);
	
	//** Special CMLT TW fcns for RESORT purpose **//
	BOOL MarkCMLTGrade99DicesInResortMap();		//v4.15T3
	
	//Check current die is no pick 
	BOOL DieIsNoPick(ULONG ulRow, ULONG ulCol);
	BOOL ResetNoPickDieState(VOID);
	BOOL DieIsSpecialRefGrade(ULONG ulRow, ULONG ulCol);	//v3.24T1
	
	//Reset Wafermap status
	BOOL ResetMapStauts(VOID);
	
	//Map Sync move with wafer table
	BOOL MapSyncMove(VOID);
	VOID InitLFInfo(LF_INFO& stInfo);		// Init Look-Forward Info
	BOOL SaveData(VOID);
	BOOL LoadData(VOID);
	BOOL RestoreOutputFileSetting(VOID);

	//Block Function
	BOOL SaveBlkData(VOID);
	BOOL LoadBlkData(VOID);

	//Update Map's grade color
	UCHAR UpdateWaferGradeColor(BOOL bReset);
	
	BOOL CreateDummyMapForPicknPlace(LONG lNoOfRows,LONG lNoOfCols, LONG lHoriDiePitchX, LONG lHoriDiePitchY, LONG lVertDiePitchX, LONG lVertDiePitchY);
	BOOL CreateDummyMapForNichia(LONG lNoOfRows,LONG lNoOfCols, LONG lHoriDiePitchX, LONG lHoriDiePitchY, LONG lVertDiePitchX, LONG lVertDiePitchY);
	BOOL CrateDummyMapForSmartWalkWithPicknPlace(LONG lNoOfRows,LONG lNoOfCols, LONG lHoriDiePitchX, LONG lHoriDiePitchY, LONG lVertDiePitchX, LONG lVertDiePitchY);
	BOOL CreateBurnInMapWith2Grades(LONG lNoOfRows,LONG lNoOfCols, LONG lGrade1, LONG lGrade2);	//andrewng //2020-0622

	// Polygon checking
	BOOL IsConvexPolygon();

	BOOL MoveBinTableToSafePosn(CONST BOOL bSafe);		//v3.70T1

	//Reference die file output function
	LONG AlertRefDieStatus(VOID);
	LONG AlertRefDieCheckOnGDStatus(VOID);		//v2.91T1
	
	// huga
	LONG AlertSpecailRefDieCheckStatus(LONG lY, LONG lX);
	BOOL OutputRefDieStatusFile(VOID);

	//LOG
	VOID LOG_DIEPITCH_DIFF(CONST CString szMsg);		//v2.78T2
	VOID BackupDiePitchLog(CONST CString szWaferID);	//v2.78T2
	VOID LOG_BLOCK_ALIGN(CONST CString szMsg);			//v3.25T16
	VOID REMOVE_BLOCK_ALIGN();							//v3.25T16

	// for table 1 search barcode
	INT SearchBarcodeMoveX(INT siDirectionX, LONG lSearchDistanceX, BOOL bWaitComplete);
	INT SearchBarcodeMoveY(INT siDirectionY, LONG lSearchDistanceY, BOOL bWaitComplete);
	INT SearchBarcodeMoveT(INT siDirectionT, LONG lSearchDistanceT, BOOL bWaitComplete);

	// for table 2 search barcode
	INT SearchBarcodeMoveX2(INT siDirectionX, LONG lSearchDistanceX, BOOL bWaitComplete);
	INT SearchBarcodeMoveY2(INT siDirectionY, LONG lSearchDistanceY, BOOL bWaitComplete);
	INT SearchBarcodeMoveT2(INT siDirectionT, LONG lSearchDistanceT, BOOL bWaitComplete);
	
	// For sorting check points function
	BOOL ReadSortingCheckPoints(CString szFilename);
	BOOL SaveSortingCheckPoints(CString szFilename);
	INT OpCheckSortingCheckPoints();

	BOOL UpdateOutputFileCtrl(LONG lNoOfOutputCols);
	BOOL LoadnCheckWaferMapColumnsFile(CString szColumnFilename);
	BOOL UpdateWaferMapHeaderCtrl(LONG lNoOfMapCols);
	BOOL LoadnCheckOutputFileFormatFile(CString szFormatFilename);

	BOOL m_bLoadCurrentMapStatus;
	BOOL IsUseLF();

	CString GetCPOutputLibraryVersion();
	BOOL ProberReadWaferMapHeader(CStringArray& szWaferMapHeader);
	VOID GetMaxBinNumber(CString& szMaxBinNumber);
	VOID GetTPExt(CString szFieldname, CString szDiodeNumber, CString& szResult);

	BOOL WriteProbeMap_Semitek(CONST BOOL bMapOrientation, CString szOutMapName);
	BOOL WriteProbeMap_NationStar(CONST BOOL bMapOrientation, CString szOutMapName);
	BOOL WriteProbeMap_NanoJoin(CONST BOOL bMapOrientation, CString szOutMapName);
	BOOL WriteProbeMap_Orient(CONST BOOL bMapOrientation, CString szOutMapName);
	BOOL WriteProbeMap_KeWei(CONST BOOL bMapOrientation, CString szOutMapName);
	BOOL WriteProbeMap_HuaMao(CONST BOOL bMapOrientation, CString szOutMapName);
	BOOL WriteProbeMap_Payton(CONST BOOL bMapOrientation, CString szOutMapName);
	BOOL WriteProbeMap_ZhongGu(CONST BOOL bMapOrientation, CString szOutMapName, ULONG ulSampleLeft, ULONG ulTotalTested);

	VOID GetHighLowLimitList(CStringArray& szaLowLimitList, CStringArray& szaHighLimitList);
	VOID GetTestConditionList(CStringArray& szaTestConditionList);
	VOID GetMapTS(CString &szString);

	VOID SetWaferTableVacuum(BOOL bOn);

	BOOL SelectWaferCamera();
	BOOL SelectBondCamera();

// prescan relative code	B
protected:

	// sorra sort mode
	LONG	SearchNml3AtAlignPointForPrescan(LONG lX, LONG lY);
	BOOL	AutoAlignMS90WaferSecondPart();
	LONG	AutoAlign2PhasesSortSecondPart(IPC_CServiceMessage &svMsg);
	VOID	CalcMS90HalfMapMaxRowCol(const BOOL bLoadingMap, const BOOL bRowModeSeparateHalfMap);
	VOID	SetLoadingRowModeHalfMapValid(ULONG ulMaxRow, ULONG ulMaxCol);
	VOID	SetLoadingColumnModeHalfMapValid(ULONG ulMaxRow, ULONG ulMaxCol);
	VOID	MS90HalfSortMapAction(BOOL bLoadingMap);
	VOID	GetMapAllGradeList(CUIntArray &aulAllGradeList);
	VOID	GetMapAllGradeColor();

	BOOL	IsDieInMapArea(LONG ulRow, LONG ulCol, DOUBLE dRatio = 1.0);
	BOOL	AF_GridSampleFocusLevel();
	BOOL	AF_DetectTargetFocusLevel(ULONG ulRow, ULONG ulCol);
	BOOL	AF_PredicateNextScanFocusLevel(ULONG ulRow, ULONG ulCol);

	BOOL	OpCheckMA1ToMA3();
	INT		OpSavePrescanMap();
	INT		OpUpdateHuaLeiMap(CString szLocalMapPath);
	INT		OpUpdateHugaMap(CString szLocalMapPath);
	INT		OpUpdateHPOMap(CString szLocalMapPath);
	INT		OpUpdateFocusMap(CString szLocalMapPath);
	INT		OpRemoveMapHeader(CString szLocalMapPath);
	BOOL	OpCreateScanOutputMap(CString szLocalMapPath);
	BOOL	OpCreateEpiTechMap(CString szLocalMapPath);
	BOOL	KeepPrescanAlignDie();
	VOID	SetAllPrescanDone(BOOL bState);
	BOOL	WaitAllPrescanDone(INT nTimeout = LOCK_TIMEOUT);
	VOID	SetES101PreloadFrameDone(BOOL bState);
	BOOL	WaitES101PreloadFrameDone(INT nTimeout = LOCK_TIMEOUT);

	VOID	SetPrescanAutoAlignMap();

	INT  OpGetWaferPos_AreaPick();
	INT	 OpMoveTable_AreaPick();
	INT  OpGetWaferPos_PrescanAreaPick(LONG lX, LONG lY);
	BOOL ConstructAreaPickMap();
	INT  OpMoveTable_Prescan();
	INT  OpGetWaferPos_Prescan();
	INT	 CheckPrescanPitch(LONG lX, LONG lY);
	INT	 CheckDebScanPitch();
	INT	 CheckAdvRegionPitch();
	BOOL IsDiePitchCheck();
	BOOL IsDiePitchCheck(const LONG lRow, const LONG lCol);
	BOOL CheckPitchX(LONG lDeltaX);
	BOOL CheckPitchY(LONG lDeltaY);
	BOOL CheckLFPitchX(LONG lDeltaX, LONG lTolerance);
	BOOL CheckLFPitchY(LONG lDeltaY, LONG lTolerance);
	BOOL CheckLongJumpPitchX(LONG lDeltaX, LONG lJumpSpanCol);
	BOOL CheckLongJumpPitchY(LONG lDeltaY, LONG lJumpSpanRow);
	BOOL CheckAlignPitchX(LONG lDeltaX, LONG lPassInTolerance = 0);
	BOOL CheckAlignPitchY(LONG lDeltaY, LONG lPassInTolerance = 0);
	BOOL CheckPitchUpX(LONG lDeltaX);
	BOOL CheckPitchUpY(LONG lDeltaY);
	BOOL CheckPredScanPitchX(LONG lDeltaX);
	BOOL CheckPredScanPitchY(LONG lDeltaY);
	BOOL CheckTekCoreAutoAlignResult(LONG lRow, LONG lCol);
	BOOL GetES101PrescanSortPosn(LONG &lCalcX, LONG &lCalcY, LONG &lCalcT);							//v4.28
	BOOL GetBlk2SortPosn(LONG lCurrRow, LONG lCurrCol, LONG lLastRow, LONG lLastCol, LONG &lMoveX, LONG &lMoveY, LONG &lMoveT);
	BOOL GetPrescanSortPosn(LONG &lCalcX, LONG &lCalcY, LONG &lCalcT, BOOL bDisableDEB = FALSE);	//v4.02T4
	BOOL CheckPrescanNgDieOnWafer(LONG lCheckMode);
	BOOL TogglePrescanMapDisplay(LONG lCheckMode, BOOL bNgMode);
	BOOL CheckDieMapWaferPosition(ULONG ulRow, ULONG ulCol, LONG lEncX, LONG lEncY);
	BOOL IsPosnWithinMapRange(INT nPosnX, INT nPosnY);
	BOOL IsPosnWithinTableLimit(INT nPosnX, INT nPosnY, DOUBLE dScale=1.0);
	BOOL OpGetScanNextFrame(CONST SHORT sDown, CONST LONG lLastRow, CONST LONG lLastCol, LONG &lNextRow, LONG &lNextCol);
	BOOL PrescanOpGetTablePosn();
	BOOL DefineAreaAndWalkPath_Rescan();
	BOOL DefineAreaAndWalkPath_Prescan();
	BOOL DefineAreaAndWalkPath_RegnScan();
	BOOL DefineAreaAndWalkPath_HalfSortScan();
	BOOL DefineAreaAndWalkPath_BarWafer();
	BOOL GetMapScanArea();
	BOOL GetMS90HalfSortMapScanArea();
	BOOL IsScanFrameInWafer(LONG lFrameRow, LONG lFrameCol);
	BOOL PrescanOpGetPosn_1DieIndex(const short sDown=1, const BOOL bVirtualMove = FALSE);
	BOOL PrescanOpGetPosn_MapIndex(LONG lOldRow, LONG lOldCol, LONG lOldWfX, LONG lOldWfY, 
									LONG &lTgtRow, LONG &lTgtCol, LONG &lTgtWfX, LONG &lTgtWfY, const LONG sDir=0);
	BOOL OpCalculateADEWaferLimit();
	BOOL OpGetScanPosn_MapIndexFastHome(const short sDown=1);
	BOOL PrescanOpGetPosn_SprialIndex();
	BOOL OpGetScanPosn_Sprial();
	BOOL PrescanUpdateWaferAlignment(INT lX, INT lY, ULONG lRow, ULONG lCol, USHORT usAlignEntry);
	BOOL PrescanRealignWafer();
	VOID SetPrescanBlkFuncEnable(BOOL bBlkFunc);
	BOOL PrescanResultVerify();
	BOOL PrescanUpdateRegionAlign(INT lX, INT lY, ULONG lRow, ULONG lCol);
	BOOL UpdateSMS(CONST BOOL bWaferEnd);
	BOOL CheckEjectorPinLifeB4Start(BOOL bAlign=TRUE);

	LONG Enable6InchWaferTable(IPC_CServiceMessage& svMsg);
	BOOL FindAndAlignAllReferDie();
	BOOL FindCrossReferDieAlignWafer();
	LONG IndexAndSearchRefer(DiePhyPos2 *InputPos, LONG lPitchRow, LONG lPitchCol, LONG lLoop=0);
	// alignment refer die position and status and OCR value, useless after alignment complete

// mylar shift prediction
	//	auto rescan sorting wafer
	BOOL	IsAutoSampleSetupOK();
	VOID	RescanAdvOffsetNextLimit();
	VOID	ChangeNextSampleLimit(ULONG ulPostCount=0);
	INT		RescanSortingWafer();
	LONG	RescanAllDoneOnWft();
	BOOL	RescanGetDieValidPosn(LONG ulIntRow, LONG ulIntCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap);
	ULONG	GetNextRscnLimit();
// mylar shift prediction

	VOID SwitchEncoderReader(CONST BOOL bWT2);

	// for align wafer button pressed and prescan complete
	VOID UpdatePrescanPosnAfterWaferRealign();
	VOID UpdatePrescanPosnAfterBlkPkRealign();

	// for blk realign button or auto realign during sorting (wafer aligned and prescanned)
	VOID UpdatePrescanPosnAfterBlockRealign();

	VOID CalculateOrgAngle(VOID);
	VOID CalculateRlnAngle(VOID);
	BOOL ConvertPrescanPosn(unsigned long ulCurrRow, unsigned long ulCurrCol, LONG &lPhyPosX, LONG &lPhyPosY);
	BOOL GetBlockRealignDiePosn(unsigned long ulCurrRow, unsigned long ulCurrCol);
	BOOL GetPrescanRunWftPosn(ULONG ulRow, ULONG ulCol, LONG &lPhyX, LONG &lPhyY);
	BOOL IsRealignScnnedWafer();

	BOOL Check201277PasswordWithWeek();
	LONG CheckSemitekPasswordWithWeek(IPC_CServiceMessage& svMsg);
	LONG CheckSemitekPassword(IPC_CServiceMessage& svMsg);
	LONG CheckCreePassword(IPC_CServiceMessage& svMsg);
	LONG CheckES100Machine(IPC_CServiceMessage& svMsg);
	LONG CheckPassword201277(IPC_CServiceMessage& svMsg);
	LONG LoadES100Screen(IPC_CServiceMessage& svMsg);
	LONG CheckRebelPassword(IPC_CServiceMessage& svMsg);
	LONG CheckRebelAndStop(IPC_CServiceMessage& svMsg);

	LONG InspectSelectType(IPC_CServiceMessage& svMsg);
	LONG InspectGotoTarget(IPC_CServiceMessage& svMsg);

	LONG ESMoveTablesToUnload(IPC_CServiceMessage& svMsg);
	LONG SetWT1JoystickLimitCmd(IPC_CServiceMessage &svMsg);
	LONG SetWT2JoystickLimitCmd(IPC_CServiceMessage &svMsg);
	LONG WT_Contour_Move(IPC_CServiceMessage &svMsg);
	LONG WT_SetContourTable(IPC_CServiceMessage &svMsg);
	LONG ShowContourEdge(IPC_CServiceMessage &svMsg);
	LONG GetYSlowMoveTime(IPC_CServiceMessage &svMsg);
	LONG GenMapPhyPosition(IPC_CServiceMessage &svMsg);
	LONG DeleteMapPsmScnCmd(IPC_CServiceMessage &svMsg);
	LONG TickMultiSortToOnCmd(IPC_CServiceMessage &svMsg);

	LONG PrintScreenCmd(IPC_CServiceMessage& svMsg);
	LONG ForceToRescanWafer(IPC_CServiceMessage& svMsg);
	LONG GetAlarmLogPath(IPC_CServiceMessage& svMsg);
	LONG ConfirmAlarmLogSettings(IPC_CServiceMessage& svMsg);
	LONG TestAlertRedYellow(IPC_CServiceMessage& svMsg);
	LONG SetPrescanMapIndexStep(IPC_CServiceMessage& svMsg);
	LONG CheckOsramPrescanEnable(IPC_CServiceMessage& svMsg);
	VOID SetPrescanOption(CONST BOOL bEnable);
	LONG CheckPrescanEnable(IPC_CServiceMessage& svMsg);
	LONG EnablePrescan(IPC_CServiceMessage& svMsg);
	LONG PrescanES101Check(IPC_CServiceMessage& svMsg);
	LONG CheckAOIOnlyMachine(IPC_CServiceMessage& svMsg);
	LONG GotoNextToSortDie(IPC_CServiceMessage& svMsg);
	LONG PrescanGotoTargetDie(IPC_CServiceMessage& svMsg);
	LONG SelectCheckCenterDie(IPC_CServiceMessage& svMsg);
	LONG ScanCheckDieOffset(IPC_CServiceMessage &svMsg);
	LONG FOVCheckDieOffset(IPC_CServiceMessage &svMsg);
	LONG AGC_WTMoveDiff(IPC_CServiceMessage &svMsg);
	LONG WftLoadBinFile(IPC_CServiceMessage &svMsg);
	LONG PrescanGotoHome(IPC_CServiceMessage& svMsg);
	LONG WftSetPrescanPrMethod(IPC_CServiceMessage &svMsg);
	LONG WftSetDummyScanGrade(IPC_CServiceMessage &svMsg);
	LONG WftSetPsmLoadAction(IPC_CServiceMessage &svMsg);
	LONG ManualAddPrescanRefPoints(IPC_CServiceMessage &svMsg);
	LONG AdjustReferPoints(IPC_CServiceMessage &svMsg);
	LONG EnablePrescanBlkPickAlign(IPC_CServiceMessage &svMsg);
	LONG AutoShowPrescanNgDieOnWafer(IPC_CServiceMessage& svMsg);
	VOID SavePrescanInfoMap(CONST INT nState);
	BOOL RealignBlkFunc2RefDiePos(CONST BOOL bAutoMode=FALSE);
	LONG RePrescanWafer(IPC_CServiceMessage& svMsg);
	LONG GrabAndSaveImage(CONST BOOL bDieType, CONST LONG lDiePrID, LONG lCheckType);
	INT	 ES101ToAlignPosition();
	BOOL WFT_MoveToFocusLevel();
	LONG SetDummyPrescanArea(IPC_CServiceMessage& svMsg);
	LONG ShowScanYieldResult(IPC_CServiceMessage& svMsg);
	LONG ClearScanYieldResult(IPC_CServiceMessage& svMsg);
	LONG UploadSelectedWaferFile(IPC_CServiceMessage& svMsg);
	LONG SpinScanYieldDisplay(IPC_CServiceMessage& svMsg);
	LONG PassWLCurrentSlotNo(IPC_CServiceMessage& svMsg);
	BOOL UpdateScanYield();
	BOOL IsWL1ExpanderSafeToMove();
	BOOL IsWL2ExpanderSafeToMove();

	LONG	GetSlowProfileDist();
	LONG	GetVerySlowDist();
	LONG	GetPrescanCenterX();
	LONG	GetPrescanCenterY();
	LONG	GetPrescanDiameter();
	BOOL	IsWithinPrescanLimit(LONG lX, LONG lY, DOUBLE dScale = 1);
	BOOL	IsWithinLimit(LONG lX, LONG lY, LONG lCtrX, LONG lCtrY, LONG lDiameter, BOOL bAllType=FALSE);
	BOOL	IsToAutoSearchWaferLimit();
	BOOL	IsPrescanSizeSet();	// define the diameter in inch.
	BOOL	IsAutoDetectEdge();
	BOOL	IsPrescanAreaDefined();
	VOID	AutoSearchWaferLimit();
	VOID	AutoSearchRectWaferEdge();
	BOOL	SeearchWaferEdge(SHORT sEdge, LONG &lEdgeX, LONG &lEdgeY);
	BOOL	CalcWaferCircle();
	BOOL	CalculateCircle(DOUBLE pt1x, DOUBLE pt1y, DOUBLE pt2x, DOUBLE pt2y, DOUBLE pt3x, DOUBLE pt3y);

	LONG	GetScanRadius();
	LONG	GetScanCtrX();
	LONG	GetScanCtrY();
	LONG	GetScanUL_X();
	LONG	GetScanUL_Y();
	LONG	GetScanLR_X();
	LONG	GetScanLR_Y();
	BOOL	CreeAutoSearchMapLimit();

	VOID FindBlkReferDieAngle(LONG lReferRow, LONG lReferCol, LONG &lX, LONG &lY, DOUBLE &dDegree);
	VOID FindBlkReferDieOffset(LONG lReferRow, LONG lReferCol, LONG &lX, LONG &lY);

	BOOL WftCheckCurrentGoodDie();
	BOOL WftMoveSearchDie(LONG &lX, LONG &lY, BOOL bDoComp=FALSE);
	BOOL WftMoveSearchReferDie(LONG &lX, LONG &lY, BOOL bDoComp=FALSE, UCHAR ucReferID = 1);
	BOOL WftSearchReferDie(ULONG ulRow, ULONG ulCol, LONG lEncX, LONG lEncY, LONG lPrID);

	VOID RegPrescanVarFunc();
	VOID InitPrescanVariables();
	VOID CheckMapPrescanAlignWaferOption();
	VOID SavePrescanDataWf(CStringMapFile  *psmf);
	VOID LoadPrescanDataWf(CStringMapFile  *psmf);
	VOID LoadPrescanOptionFromWFT(CStringMapFile  *psmf);
	VOID SavePrescanOptionFromWFT(CStringMapFile  *psmf);

	//BOOL	m_bPrResultFPC;				// for semitek only
	LONG PrescanAllDoneOnWft();
	// daily check or scan -> index finish ->probing and calculating
	VOID RunOperation_CP(CONST BOOL bFastDaily);	// normal scan -> finish -> probing
	VOID RunOperation_MS60();	

	INT RescanAoiWafer();
	VOID CheckNextCycleBHTMovetoPrepick();
	VOID CheckResetLFCycle();
	VOID ResetLFCycle();
#ifdef PROBER
	CBinTotalWrapper m_PrbWrapperItem1;
	CBinTotalWrapper m_PrbWrapperItem2;
	CBinTotalWrapper m_PrbWrapperItem3;
#endif
	ULONG CP_GetGridRows();
	ULONG CP_GetGridCols();
	LONG CP_TogglePOState(IPC_CServiceMessage &svMsg);
	LONG CP_ToggleDCState(IPC_CServiceMessage &svMsg);
	//	Chip Prober probing position offset PO

	BOOL WFT_SearchPrDie(LONG &lOffsetX, LONG &lOffsetY);
	BOOL PeekProbeNextDie_Fast(ULONG &ulOutRow, ULONG &ulOutCol);
	BOOL OpGetBlockProbeCenter(CONST LONG lProbeX, CONST LONG lProbeY, LONG &lCtrMoveX, LONG &lCtrMoveY);
	BOOL GoToPrescanDieCheckPosition();
	BOOL EmptyDieGradeCheck();
	BOOL ReverifyBlockPickReferDie();
	BOOL BlkFunc2SearchCurrDie(LONG *lPosX, LONG *lPosY, BOOL bWholeWindow=TRUE);
	BOOL SearchCurrReferDie(LONG *lPosX, LONG *lPosY);
	BOOL RectWaferCornerCheck(VOID);
	BOOL RectWaferSingleCornerCheck(ULONG ulRow, ULONG ulCol, UCHAR ucCorner);
	BOOL HmiAlarmContinueMessage(CString szMsg, CString szTitle);
	LONG UpdateScnOption(IPC_CServiceMessage &svMsg);
	BOOL GetValidReferInBlock(ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol, ULONG &ulOutRow, ULONG &ulOutCol);
	BOOL HasMapValidDieAround(ULONG ulRow, ULONG ulCol, ULONG ulLoop);
	BOOL ReVerifyAllReferDie();
	BOOL RectWaferCornerDieCheck(UCHAR ucCorner);	//0 = TL; 1 = TR; 2 = BL; 3 = BR
	LONG RectWaferAroundDieCheck();
	VOID MultiSrchInitNmlDie1(BOOL bInspect=TRUE);
	BOOL GetFeaturePatternInBlock(ULONG ulULRow, ULONG ulULCol, ULONG ulLRRow, ULONG ulLRCol, ULONG &ulOutRow, ULONG &ulOutCol);

	VOID ChangeCameraToWafer();
	VOID ChangeCameraToWaferBondMode(CONST BOOL bBondMode);

	BOOL CreateDummyMapForBurnIn(LONG lWftX, LONG lWftY, CString szFileName);
	BOOL CreateDummyMapForPrescanPicknPlace(CONST BOOL bAlignWithRefer = FALSE, CONST BOOL bVirtualHome = FALSE);
	BOOL SwapContourPreloadMap();
	BOOL SwapCachedMapTidyUp();
	LONG EnablePrescanDebug(IPC_CServiceMessage &svMsg);
	BOOL SearchAndRemoveKeyFiles(CString szPath, CString szFileKeyWord);
	LONG EnablePrescanTwice(IPC_CServiceMessage &svMsg);
	LONG CheckScnPrescanOption(IPC_CServiceMessage &svMsg);
	LONG CheckSuperUserPassword(IPC_CServiceMessage &svMsg);
	LONG CheckBWMachinePassword(IPC_CServiceMessage &svMsg);
	LONG CheckSemitekSUPW(IPC_CServiceMessage &svMsg);

	BOOL ManualDisplaySemitekMapName(BOOL bFullyAuto = FALSE);

	BOOL IsNeedCheckBatchIdFile();
	BOOL CheckBatchIdFile(CString szFilename);

	LONG LogItems(LONG lEventNo);
	LONG GetLogItemsString(LONG lEventNo, CString& szMsg);

	BOOL MoveBackLightToSafeLevel();
	BOOL MoveEjectorElevatorToSafeLevel();

	BOOL CheckIsMagazineComplete();

	ULONG GetScanMethod();

	VOID DEB_EnableLog();
	BOOL DEB_AutoRealignKeyDie_old();
	BOOL DEB_AutoRealignKeyDie();
	LONG DEB_SetLogEnable(IPC_CServiceMessage& svMsg);
	LONG DEB_SampleKeyDie(IPC_CServiceMessage& svMsg);
	INT  DEB_UpdateAllDicePos();
	BOOL DEB_CheckKeyDicesOnWafer();
	BOOL DEB_CheckEnableFeature(BOOL bSaved);	//v4.50A25

	LONG	GetScanNextPosnX();
	LONG	GetScanNextPosnY();
	BOOL	GetPrescanDiePitch();

	UCHAR	m_ucScanMapTreatment;

	LONG ChangeRegionSortOrder(IPC_CServiceMessage &svMsg);
	LONG AutoRegionAlignStdWafer(IPC_CServiceMessage &svMsg);
	LONG AutoRegionAlignBlock(IPC_CServiceMessage &svMsg);
	BOOL AutoRegionAlignRefBlk();
	BOOL AutoRegionAlignManual();
	BOOL IsRegionAlignRefBlk();
	VOID SaveRegionStateAndIndex();
	BOOL IsNextRegionAutoAlign();
	INT	 OpRegionScanStarter(ULONG ulMapRow, ULONG ulMapCol);

	BOOL IsScanPosnOK();
	BOOL IsMapPosnOK();

	BOOL SpiralSearchRegionReferDie(CONST LONG lMaxLoopCount, LONG lReferPRID);
	BOOL SearchAndAlignReferDie(LONG lReferPRID);
	LONG SetSoraaModeOption(IPC_CServiceMessage &svMsg);		//v4.28T5

	BOOL	BlkFunc2ReferDieCheck(CONST ULONG ulY, CONST ULONG ulX);
	VOID	UpdateDefectDie(CONST ULONG ulY, CONST ULONG ulX, CONST UCHAR ucGrade);
	BOOL	RestartDefectDieSort();
	LONG	GetPrescanAlignPosnX();
	LONG	GetPrescanAlignPosnY();
	VOID	ObtainContourEdgeDetail();
	BOOL	NCP_Auto_ScanPickedDieResort();
public:

	BOOL	m_bChangeGrade4PrNG;
	BOOL	m_bSortMultiToOne;
	ULONG	m_ulMinGradeCount;
	ULONG	m_ulMinGradeBinNo;			//v4.38T7
	CUIntArray	m_unArrMinGradeBin;		//v4.38T7	//Mixed bin sorting

	VOID ClearPrescanRawData(CONST BOOL bLoadMap=FALSE, CONST BOOL bClearPosn=TRUE);

	VOID UiSaveMapFile(CString szFileNamePath);
	VOID SetPsmMapMachineNo(CONST BOOL bNew, CONST CString szMcNo);
	VOID SetMapName();
	BOOL CheckWaferBonded();
	BOOL StoreWaferBonded();
	BOOL CheckWaferBondedSize();
	VOID DisplayDieState(ULONG ulRow, ULONG ulCol);
	VOID UpdateDieStateSummary();

	BOOL UpdateWaferYield(VOID);

	BOOL LoadPackageMsdWTData(VOID);
	BOOL SavePackageMsdWTData(VOID);
	BOOL UpdateWTPackageList(VOID);

	BOOL PsmFileExistCheck(CString szRemotePSMFile);

	VOID ConvertMapAsmToHmi(ULONG ulAsmRow, ULONG ulAsmCol, LONG &lHmiRow, LONG &lHmiCol);

	VOID SetScanDieNum(ULONG ulGoodNum, ULONG ulDefectNum, ULONG ulBadCutNum, ULONG ulExtraNum, ULONG ulEmptyNum);

	VOID WTPRFailureCaseLog(LONG lImageNo = 5);

// prescan relative code	E
protected:
	//======================================================
	// Constructor Functions
	VOID TC_InitVariables();
	BOOL TC_InitInstance();
	VOID TC_ExitInstance();
	VOID TC_RegisterVariables();
	VOID TC_LoadOption(CStringMapFile  *psmf);
	VOID TC_SaveData(CStringMapFile  *psmf);

    //======================================================
	// Member Functions
	VOID TC_ReadTemperature();
	VOID TC_FixConvertion();
	VOID TC_SetTargetAndLimit();
	VOID TC_TurnAlarmOn(BOOL bOn);
	VOID TC_TurnPowerOn(BOOL bOn);
	VOID TC_TuneAbort();

	BOOL TC_IsEnable();
	BOOL TC_CheckEjectorState(BOOL bPopMsg);
	LONG TC_CheckEjectorLimit();	// should read temperature firstly
	BOOL TC_CheckAlarmOn();

	VOID TC_LogMessage(CString szMsg);

	BOOL	TC_IsOnPower();
	BOOL	TC_IsCheckTmp();
	BOOL	TC_IsCheckAlarm();
	SHORT	TC_GetTargetTmp();
	SHORT	TC_GetTmpToler();
	SHORT	TC_GetTuneTmp();
	SHORT	TC_GetRealTmp();
	BOOL	TC_IsTuneCalib();

	VOID TC_AutoTuneEjector();
	BOOL CheckPrefixNameForEpitop(CString szBarcode,CString szPrefixName);
	//======================================================
	// Member Variables
	//CLiTEC	m_oTCtrl;
	TC_Data		m_stTCEjt;

protected:
	//======================================================
	// SERVICE COMMAND Functions
	LONG Tc_SetTmpConvertion(IPC_CServiceMessage& svMsg);

	LONG Tc_PowerOnHeater(IPC_CServiceMessage& svMsg);
	LONG Tc_CheckAlarmBit(IPC_CServiceMessage& svMsg);
	LONG Tc_SetTargetTmp(IPC_CServiceMessage& svMsg);
	LONG Tc_SetTuneActualTmp(IPC_CServiceMessage& svMsg);
	LONG Tc_SetTempTolerance(IPC_CServiceMessage& svMsg);
	LONG Tc_StartTuneEjector(IPC_CServiceMessage& svMsg);
// Temperature Controller	Thermal Ejector
};

