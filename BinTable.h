/////////////////////////////////////////////////////////////////
// BinTable.cpp : interface of the CBinTable class
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

#ifndef _BIN_TABLE_H_
#define _BIN_TABLE_H_

#include "MS896AStn.h"
#include <WaferMapWrapper.h>
#include <BinSetupWrapper.h>
#include <Winspool.h>
#include "BT_Constant.h"
#include "BT_PhyBlkMain.h"
#include "BT_PhyBlk.h"
#include "BT_BinBlkMain.h"
#include "BT_BinBlk.h"
#include "BT_BinGradeData.h"
#include "LogFileUtil.h"
#include "PreBondEvent.h"
#include "MESConnector.h"
#include "BT_BinSummaryFile.h"
//#include "TSCPrinter.h"
#include "LabelDatabase.h"
#include "LabelFactory.h"
#include "Printer.h"
#include "InputCountSetupFile.h"
#include "BT_MatrixOffset.h"
#include "stdafx.h"

#ifndef MS_DEBUG		//v4.47T5
	using namespace AsmSw;
#endif

class CLookAheadMap: public CObject
{
public:
	CLookAheadMap() 
	{
		m_ulLookAheadDirection = BT_PR_CENTER;
		m_ulRow = 0;
		m_ulCol = 0;
		m_lPitchX = 0;
		m_lPitchY = 0;
	};
	virtual	~CLookAheadMap()
	{
	};
	
	CLookAheadMap &CLookAheadMap::operator=(const CLookAheadMap &rValue)
	{
		m_ulLookAheadDirection	= rValue.m_ulLookAheadDirection;
		m_ulRow					= rValue.m_ulRow;
		m_ulCol					= rValue.m_ulCol;
		m_lPitchX				= rValue.m_lPitchX;
		m_lPitchY				= rValue.m_lPitchY;
		return (*this);
	}

public:
	ULONG m_ulLookAheadDirection;
	ULONG m_ulRow;
	ULONG m_ulCol;
	LONG  m_lPitchX;
	LONG  m_lPitchY;
};

class CBinTable : public CMS896AStn
{
	DECLARE_DYNCREATE(CBinTable)

public:
	// Init (BinTable.cpp)
	CBinTable();
	virtual	~CBinTable();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();
	virtual BOOL InitData();			// Init the data from SMF and NVRAM
	virtual VOID ClearData();			// Clear data for cold-start
	virtual VOID FlushMessage();		//Flush IPC Message
	virtual VOID UpdateStationData();

	VOID GetWafflePadSettings();
	BOOL CheckIfNeedToRestoreNVRAMData();
	VOID DecodeSubBinSetting();

	BOOL GetUseFilenameAsWaferId();
	BOOL ClearBatchIDInformation();

	//Measurement unit conversion functions (unique to BinTable Stn)
	LONG ConvertXEncoderValueForDisplay(LONG lXEncoderValue);
	LONG ConvertYEncoderValueForDisplay(LONG lYEncoderValue);
	LONG ConvertFileUnitToXEncoderValue(DOUBLE dXFileValue);
	LONG ConvertFileUnitToYEncoderValue(DOUBLE dYFileValue);
	LONG ConvertMilUnitToXEncoderValue(DOUBLE dXFileValue);		//v3.62
	LONG ConvertMilUnitToYEncoderValue(DOUBLE dYFileValue);		//v3.62

	//Assist in grabbing postbond results
	VOID GetBinBlkNoOfRowsAndCols(ULONG ulBinBlkId, ULONG &ulNoOfRows, ULONG &ulNoOfCols); 

	//Breakdown of Clear Bin Counter function
	BOOL ClrBinCntCmdSubTasks(ULONG ulPhyBlkId, CString szClrBinCntMode, CString szBinOutputFileFormat, BOOL bShowMessage, BOOL bAutoClear, BOOL& bNeedToClear, 
		BOOL bResetDieCount = TRUE, BOOL bForSummary = FALSE);

	BOOL ClrBinCntCmdSubTasksByGrade(UCHAR ucGrade, CString szClrBinCntMode, CString szBinOutputFileFormat, CDWordArray& dwaBinBlkIds, BOOL bShowMessage,
		BOOL bAutoClear, BOOL bResetDieCount = TRUE, BOOL bForSummary = FALSE);

	BOOL SaveBondedCountsByBlock(ULONG ulBondedBlkId);

	CString GetSmartInlineShareFolder();
	CString GetSlotTempFileName(const LONG lBlkId);
	LONG GetNoOfSortedDie(const UCHAR lBlkId);
	CString GetCustomOutputFileName();
	VOID BT_ClearBinCounter(const ULONG ulBlkId);

	BOOL ClearBPRAccEmptyCounter(ULONG *ulBinBlk);
	
	BOOL IsAllMotorsHomed();
	BOOL IsAllMotorsEnable();
	LONG GetBTRemainMoveDelay();
	LONG MoveToNextLastDiePosn();
protected:

	// Update (BinTable.cpp)
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	virtual VOID RegisterVariables();
	VOID RegisterBTErrorMapVariables();		//andrewng //2020-0805

	// State Operation (BT_State.cpp)
	VOID Operation();
	VOID RunOperation();

	virtual	VOID IdleOperation();
	virtual	VOID DiagOperation();
	virtual	VOID InitOperation();
	virtual VOID PreStartOperation();
	virtual	VOID AutoOperation();
	virtual	VOID DemoOperation();
	virtual	VOID StopOperation();

	// Profile Functions (BT_Profile.cpp)
	INT X_Profile(INT nProfile);
	INT Y_Profile(INT nProfile);
	INT X2_Profile(INT nProfile);
	INT Y2_Profile(INT nProfile);
	CString X_ProfName(INT nProfile);
	CString Y_ProfName(INT nProfile);
	VOID LogProfileTime();						//v3.61T1
	BOOL X_SelectControl(CONST INT nCtrl);		//v3.67T5
	BOOL Y_SelectControl(CONST INT nCtrl);		//v3.67T5
	BOOL X2_SelectControl(CONST INT nCtrl);	
	BOOL Y2_SelectControl(CONST INT nCtrl);	

	//Check Axis Power
	BOOL X_IsPowerOn();
	BOOL Y_IsPowerOn();
	BOOL X2_IsPowerOn();		//9INCH_MS
	BOOL Y2_IsPowerOn();		//9INCH_MS

	VOID SelectXProfile(const LONG lDistX);
	VOID SelectYProfile(const LONG lDistY);
	VOID SelectXYProfile(const LONG lDistX, const LONG lDistY);
	LONG MoveXYTo(const LONG lPosnX, const LONG lPosnY);
	// Move Functions (BT_Move.cpp)
	INT X_Home();
	INT X_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT X_Move(INT nPos, INT nMode = SFM_WAIT);
	VOID GetCurXYPosn();
	INT X_SearchPosLimit(VOID);
	INT X_SearchNegLimit(VOID);
	INT X_SearchBarcode(LONG lDir, LONG lSearchDist, INT nMode = SFM_WAIT);		//v4.39T7
	INT X_Sync();
	INT X_PowerOn(BOOL bOn = TRUE);
	INT X_Comm();

	INT Y_Home();
	INT Y_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Y_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Y_SearchPosLimit(VOID);
	INT Y_SearchNegLimit(VOID);
	INT Y_SearchBarcode(LONG lDir, LONG lSearchDist, INT nMode = SFM_WAIT);		//v4.39T7
	INT Y_Sync();
	INT Y_PowerOn(BOOL bOn = TRUE);
	INT Y_Comm();

	//v4.39T7	//MS100 Nichia
	INT T_Home();
	INT T_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT T_Move(INT nPos, INT nMode = SFM_WAIT);
	INT T_Sync();
	INT T_PowerOn(BOOL bOn = TRUE);
	BOOL T_IsPowerOn();
	BOOL T_MoveByDegree(DOUBLE dDegree, INT nMode = SFM_WAIT);
	INT T_SearchBarcode(LONG lDir, LONG lSearchDist, INT nMode = SFM_WAIT);		//v4.39T7
	INT T_Profile(INT nProfile);
	CString T_ProfName(INT nProfile);

	INT X2_Home();
	INT X2_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT X2_Move(INT nPos, INT nMode = SFM_WAIT);
	INT X2_SearchPosLimit(VOID);
	INT X2_SearchNegLimit(VOID);
	INT X2_Sync();
	INT X2_PowerOn(BOOL bOn = TRUE);
	INT X2_Comm();

	INT Y2_Home();
	INT Y2_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT Y2_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Y2_SearchPosLimit(VOID);
	INT Y2_SearchNegLimit(VOID);
	INT Y2_Sync();
	INT Y2_PowerOn(BOOL bOn = TRUE);
	INT Y2_Comm();

	INT Y12_Home();		//MS100 9 Inch dual table Y1 & Y2 homeing sequence
	//INT X12_Home();		//MS100 9Inch dual table X1 & X2 homing sequence

	INT XY_Home();
	INT XY_MoveTo(INT nPosX, INT nPosY, INT nMode = SFM_WAIT);
	INT XY_Move(INT nPosX, INT nPosY, INT nMode = SFM_WAIT);
	INT XY2_Home();
	INT XY2_MoveTo(INT nPosX, INT nPosY, INT nMode = SFM_WAIT);
	INT XY2_Move(INT nPosX, INT nPosY, INT nMode = SFM_WAIT);

	BOOL HomeTable1();
	BOOL HomeTable2();
	BOOL IsWithinTable1Limit(LONG lX, LONG lY);
	BOOL IsWithinTable2Limit(LONG lX, LONG lY);
	BOOL IsTable1InBondRegion();
	BOOL IsTable2InBondRegion();

	BOOL PortableBinBlockSetup();
	BOOL UpdatedPKGDisplayRecord();
	BOOL BT_GetFrameLevel();

	VOID BackupTempFile(ULONG ulBlkId);
	VOID BackupAllTempFile();

	VOID SetJoystickOn(BOOL bOn, BOOL bCheckLimit=TRUE, BOOL bUseBT2=FALSE);	// Turn on or off the joystick

	// Sub-State Functions (BT_SubState.cpp)
	INT		OpInitialize();
	INT		OpPreStart();
	INT		OpIndex(BOOL bLogMsg=FALSE);
	VOID	UpdateWTGradefromBinLoader();
	INT		OpIndex_MS109_AfterChangeGrade(BOOL bLogMsg=FALSE);		//v4.42T16
	INT     OpIndex_MS90();
	
	INT		OpOsramIndex();
	INT     OpUpdateOsramStopPoint();
	INT     OpOsramNotIndex();
	
	INT     OpSemitekCheckEmptyFrame(CONST ULONG ulBlkInUse, CString szBarcode);

	INT		OpNextDie();
	INT		OpMoveTable();
	INT		OpMoveTable2();				//MS100 9Inch BT2
	INT		OpMoveBack();
	INT		OpMoveBack2();				//MS100 9Inch BT2
	INT		OpBackupMap();
	INT		OpWaferEndBackupMap();
	VOID	OpUpdateDieIndex();	
	BOOL	OpGenAllTempFile(BOOL bBackupTempFile = FALSE);
	VOID	OpGenWaferStopInfo();
	INT		OpMoveToUnload();
	ULONG	OpGetNextGradeBlk();
	ULONG	OpGetNextNextGradeBlkInMixOrder(CONST ULONG ulCurrBlk);		//v3.94
	ULONG	OpGetNextNextGradeBlk(CONST ULONG ulCurrBlk);					//v2.82T1
	BOOL	OpPreChangeNextGrade();						//v2.71
	BOOL	OpGetBlkIsNearFull();
	BOOL	OpPrPreBondLFPad();							//CSP
	BOOL	OpPrPreBondAlignPad();						//v3.79
	BOOL	OpAddPostBondOffset(LONG &lX, LONG &lY, LONG lDebug=0);		//v3.86T5
	BOOL	OpAddBHAtPrePickOffset(LONG &lX, LONG &lY);					//v4.55A10
	BOOL	OpUpdateMS100BinRealignAngle(LONG lEncX, LONG lEncY);		//v4.44T2	//Semitek
	BOOL	m_bIfNeedUpdateBinFrameAngle;				//v4.44T2	//Semitek
	BOOL	OpPerformUplookBTCompensation();			//v4.52A16
	LONG	m_lBTCompCounter;

	// Region Prescan to get next available grade (has die in this region)
	BOOL	OpGetRegionNextGrade(CONST INT iIndex, CUIntArray &aulSortGradeList, UCHAR &ucNextGrade);
	ULONG	OpGetRegionGradeDieNum(UCHAR ucNextGrade);

	//Joystick Functions
	BOOL SaveEncoderValue();
    VOID InitVariable(VOID);

	// Load/ Save Last State
	BOOL BT_LoadLastState();
	BOOL SaveLastState();

	//Bond Functions
	ULONG GrabBondXYPosn(UCHAR ucGrade, DOUBLE& dXPosn, DOUBLE& dYPosn, LONG& lRow, LONG& lCol, LONG& lSortDir);	//v4.57A7	//CSP
	BOOL IfBondOK(ULONG ulBondedBlk, LONG lWaferX, LONG lWaferY, LONG lWafEncX, LONG lWafEncY, LONG lBtEncX, LONG lBtEncY);
	BOOL SaveBondResult(UCHAR ucGrade, ULONG ulBondedBlk, LONG lBinTableX, LONG lBinTableY,
								LONG lWaferX, LONG lWaferY);

	//Save & Init BinTable limit
 	BOOL InitBinTableRunTimeData();
	BOOL InitBinTableData();
	BOOL InitBinTableSetupData();
	BOOL SaveBinTableData(VOID);
	BOOL SaveBinTableSetupData();
	BOOL GenerateConfigData();			//v3.86
	BOOL SaveBinGradeWithIndex(BOOL bCheckAll=TRUE);			// save specific grade in bin block
	BOOL SaveByPassBinMap(BOOL bEnable);		//v4.48A11	//Semitek, 3E DL

	BOOL SaveReferenceCross(DOUBLE dBinTwoCrossXDistance, DOUBLE dBinTwoCrossYDistance, LONG lEncRefCross_X, LONG lEncRefCross_Y);

	//Physical blocks drawing
	VOID PhyBlksDrawing();
	VOID PhyBlksDrawing_BL(ULONG ulBlkId);

	//Bin blocks drawing
	VOID BinBlksDrawing();
	VOID BinBlksDrawing_BL(ULONG ulBlkId);
	VOID UpdateBinDisplay(ULONG ulBlkId, UCHAR ucMode=0);

	//Grade Legend
	VOID WriteGradeLegend();

	//Wafer Statistics
	VOID LoadWaferStatistics();
	VOID UpdateWaferStatistics(UCHAR ucGrade);
	VOID AOILoadMapGradesLegend(BOOL bCreate);			//	for AOI macine to display die meaning

	//Grade RankID
	INT LoadGradeRank();
	BOOL ReadRankIDFile(CString szFilename);
	VOID LoadGradeRankID();
	VOID UpdateGradeRankID();

	// BinBlk Setup function
	BOOL ShowBinBlkTemplateData();
	BOOL AutoSetupBinBlkUsingGradeRankFile();
	BOOL SetupSingleBinBlkUsingTemplate(CString szTemplateName,ULONG ulNoOfBinBlk,
										ULONG ulBinBlkToSetup, ULONG ulSourcePhyBlk,
										UCHAR ucGrade);

	BOOL AutoGenerateBinBlkSummary();
	BOOL UpdateGenerateBinBlkSummaryTime();

	VOID UpdateAmiFileChecking();

	BOOL SaveGradeMapTableData(UCHAR ucGrade, USHORT usOriginalGrade);
	BOOL DynamicGradeAssign(CArray<BIN_FILE_INFO, BIN_FILE_INFO>& aGradeInfo);
	BOOL UpdateLotDieCount(CArray<BIN_FILE_INFO, BIN_FILE_INFO>& aGradeInfo);
	BOOL SelectGradeToBond(CArray<BIN_FILE_INFO, BIN_FILE_INFO>& aGradeInfo);
	BOOL ApplyOptimizeBinCount(CString szFullPath, BOOL bEnableBinSumWaferLotCheck, ULONG ulMinLotCount);
	LONG OptimizeBinCount(CString szFilename, BOOL bEnableBinSumWaferLotCheck, ULONG ulMinLotCount);
	BOOL ResetOptimizeBinCountStatus();

	BOOL CompareWithPkgName(CString szName);

	BOOL UpdateGradeInputCountUsingTemplate(CString szTemplateName, UCHAR ucGrade);

	//Auto Clear Bin Counter
	BOOL GetBinBlksContainedInPhyBlk(ULONG ulPhyBlkId, CDWordArray &dwaBinBlkIdsList);


	//Wafer End File generating
	BOOL CheckWaferIsPicked();
	BOOL WaferEndYieldCheck();

	BOOL WaferEndFileGenerating_Finisar();
	BOOL ScanSummaryGenerating();
	CString CalculateWaferEndDownTime();
	CString CalculateWaferTotalTime();

	//Get Wafer End Info for Send to Host
	BOOL SendWaferEndInfoToHost();

	//Get Clear Bin Info for Send to Host
	BOOL SendClearBinInfoToHost(ULONG ulPhyBlkId);

	// huga
	LONG AlignEmptyFrame(ULONG ulBlkID);
//	LONG AlignEmptyFrame2(ULONG ulBlkID);
    
	//Realign bin block function
	LONG FirstSearchDieInBinMapWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const UCHAR ucCorner);
	LONG FirstSearchDieNormalWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize);
	LONG FirstSearchDieWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const ULONG ulDieRow, const ULONG ulDieCol);
	LONG SecondSearchDieInBinMapWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const UCHAR ucCorner);
	LONG SecondSearchDieNormalWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize);
	LONG SecondSearchDieWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const ULONG ulDieRow, const ULONG ulDieCol);
	VOID GetBinBlockXYDistance(const ULONG ulBlkInUse, LONG &lLogicalXBinBlockDistance, LONG &lLogicalYBinBlockDistance);
	DOUBLE CalcDieABAngle(ULONG ulBlkInUse, 
						  const LONG DieALogical_x, const LONG DieALogical_y, const LONG DieAPhysical_x, const LONG DieAPhysical_y, 
						  const LONG DieBLogical_x, const LONG DieBLogical_y, const LONG DieBPhysical_x, const LONG DieBPhysical_y,
						  const LONG lDiePitchX, const LONG lDiePitchY,
						  LONG &lXDistance, LONG &lYDistance, LONG &lLogicalXDistance, LONG &lLogicalYDistance,
						  double &dCheckRealignmentAngle);
	VOID FindNextDieOfFirstDie(const ULONG ulBlkInUse, const DOUBLE dFOVSize,
							   const BOOL bNewAlignMethod, const BOOL bUsePR, 
							   const ULONG ulNoOfSortedDie, const ULONG ulFirstDieIndex, const ULONG ulDiePerRow,
							   BOOL &bUse2ndOffsetXY, BOOL &bUse3rdOffsetXY, 
							   LONG &lNewXOffset, LONG &lNewYOffset,
							   LONG &DieALogical_x, LONG &DieALogical_y, LONG &DieAPhysical_x, LONG &DieAPhysical_y);
	ULONG FindFirstDieOfGrade(const ULONG ulBlkInUse, const ULONG ulNoOfSortedDie, ULONG &ulRow, ULONG &ulCol);
	ULONG FindLastDieOfFirstRowInGrade(const ULONG ulBlkInUse, const ULONG ulNoOfSortedDie, const ULONG ulFirstDieIndex,
									   const ULONG ulDiePerRow, const ULONG ulDiePerCol, ULONG &ulLastDieRow, ULONG &ulLastDieCol);
	VOID GetDieLogicalEncoderValue(const ULONG ulBlkInUse, const ULONG ulDieIndex, LONG &lDieLogical_x, LONG &lDieLogical_y, BOOL bLog = FALSE);

	LONG RealignBinBlock(ULONG ulBinBlockID, BOOL bAuto, BOOL bAutoTeach, BOOL bDisableAlarm=FALSE, BOOL bDummyRun=FALSE, USHORT sRetry=0, BOOL bSecondRealign = FALSE);
	LONG Realign2PtBinBlock(ULONG ulBinBlockID, BOOL bAuto, BOOL bAutoTeach);
	LONG Realign1PtBinBlock(ULONG ulBinBlockID, BOOL bAuto, BOOL bAutoTeach);	//v3.33
	LONG ConvertBinTablePosn(LONG &lX, LONG &lY, ULONG ulBlk, DOUBLE dAngle, LONG lXOffset, LONG lYOffset, BOOL bLog=FALSE);
	LONG ConvertBinTableDPosn(DOUBLE &dX, DOUBLE &dY, ULONG ulBlk, DOUBLE dAngle, LONG lXOffset, LONG lYOffset, BOOL bLog=FALSE);
	VOID Rotate(const DOUBLE dOrgX, const DOUBLE dOrgY, DOUBLE dAngle, DOUBLE &dX, DOUBLE &dY);
	VOID Rotate(const LONG lOrgX, const LONG lOrgY, DOUBLE dAngle, LONG &lX, LONG &lY);

	VOID SwitchToBPR(VOID);
	VOID SwitchToPbBPR(VOID);
	BOOL BT_SearchFirstDie(BOOL &bResult, LONG &lSiStepX, LONG &lSiStepY);
	LONG SearchFirstGoodDie(ULONG ulBlkInUse, BOOL bDoGlobalTheta = FALSE);
	UCHAR GetRealignWithFirstCorner(const ULONG ulWalkPath, const ULONG ulDieRow, const ULONG ulDieCol);
	UCHAR GetRealignWithSecondCorner(const ULONG ulWalkPath, const ULONG ulDieRow, const ULONG ulDieCol);
	LONG MultiSearchFirstGoodDie(ULONG ulBlkInUse, ULONG ulDieRow, ULONG ulDieCol, BOOL bDoGlobalTheta=FALSE); //4.51D20
	LONG SpiralSearchMove(INT nMultiUnfondCounter,LONG lX, LONG lY, LONG lMoveCountX , LONG lMoveCountY);

	LONG SearchFirstGoodDie_Semitek(ULONG ulBlkInUse, BOOL bDoGlobalTheta=FALSE, BOOL bCCWSearch=FALSE);	//v4.50A10
	BOOL BT_FindContinue8Die(const LONG lPitchX, const LONG lPitchY, const ULONG ulWalkPath, const BOOL bUseWafflePad,
							LONG &lLHSDieX2, LONG &lLHSDieY2, LONG &lRHSDieX, LONG &lRHSDieY, LONG &lFoundCounter);
	LONG BT_FindGlobalTheta(ULONG ulBlkInUse);		//v4.39T7	//Nichia BT T
	BOOL LookAhead3rdGoodDie(CONST ULONG ulBinBlockID, CONST BOOL bCheckLeftDie, DOUBLE dFOVSize, CString& szError);
	BOOL LookAround1stDie(CONST ULONG ulBinBlockID, DOUBLE dFOVSize, const ULONG ulDieRow, const ULONG ulDieCol);	//v4.50A23
	BOOL LookAround2ndDie(CONST ULONG ulBinBlockID, DOUBLE dFOVSize, const ULONG ulDieRow, const ULONG ulDieCol);	//v4.44T3
	BOOL LookAhead2ndDieOffset(CONST ULONG ulBinBlockID, DOUBLE dFOVSize, 
								BOOL& bUse2ndOffsetXY, LONG& lOffsetX, LONG& lOffsetY);		//v4.27T1	//Semitek
	BOOL Check3rdGoodDieRelOffsetXYinFOV(CONST ULONG ulBinBlockID, DOUBLE dFOVSize, 
								CString& szError, BOOL& bTopDieOK, BOOL& bSideDieOK);		//v4.26T1	//Semitek
	//LONG Search3rdGoodDie(BOOL& bPlacement, DOUBLE& dX, DOUBLE& dY);
	LONG Search3rdGoodDie(BOOL& bPlacement, DOUBLE& dX, DOUBLE& dY, BOOL &bDXFail, BOOL &bDYFail);		//5432123
	LONG SearchGoodDie(BOOL bMove=TRUE, BOOL bUseBT2=FALSE);
	BOOL Calculate1PtRealignAngleInFOV(DOUBLE& dAngle, ULONG ulBinBlockID, DOUBLE dFOVSize);		//v4.46T9
    LONG CheckBPRStatus(VOID);

	VOID CreteBondedDieInfoInBinMap(ULONG ulBlkInUse);
	BOOL AdjustGlobalThetaUsingLastRowCol(const ULONG ulBlkInUse, const ULONG ulDieIndex,	 
										  const DOUBLE dCheckRealignmentAngle, const BOOL bSecondRealign,
										  DOUBLE& dNewAngle, DOUBLE& dRealignmentNewAngle, 
										  LONG& lNewXOffset, LONG& lNewYOffset);

	LONG MovePRSearchIndexPosn(ULONG ulBlkInUse, ULONG ulIndex, 
							   DOUBLE dNewAngle, LONG lXOffset, LONG lYOffset, BOOL bDisplayBondPrSearchWindow,
							   const LONG lRow, const LONG lCol,
							   LONG &lIndexPhysical_x, LONG &lIndexPhysical_y);
	VOID CalcAngle(const double dDiffY, const double dDiffX, double &dThetaAngle, double &dThetaAngleInRadian);
	VOID ReverseAngle(double &dThetaAngle, double &dThetaAngleInRadian);
	LONG FindCurrentDieCenter(const LONG lX, const LONG lY, const LONG lRow, const LONG lCol, const BOOL bMovetoDieCenter, const BOOL bRetry,
		 					  LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4]);
	BOOL ConfirmLastRow(const ULONG ulBlkInUse, const CLookAheadMap acLookAheadDie[9], const LONG lRow, const LONG lCol,
						LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4],BOOL bNoRetry = FALSE);
	BOOL ConfirmFirstLastColOnRow(const ULONG ulBlkInUse, const CLookAheadMap acLookAheadDie[9], const LONG lLookAheadIndex, const LONG lRow, const LONG lCol,
								  LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4]);
	LONG ConfirmMovetoLastRow(ULONG ulBlkInUse, const ULONG ulIndex, 
							 const DOUBLE dNewAngle, const LONG lXOffset, const LONG lYOffset,
							 const LONG lRow, const LONG lCol, const ULONG ulCurBinMapPath, 
							 LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4],BOOL bNoRetry = FALSE);
	LONG LookAhead4DiePRSearchIndexPosn(ULONG ulBlkInUse, const ULONG ulIndex, 
									    const DOUBLE dNewAngle, const LONG lXOffset, const LONG lYOffset,
									    const LONG lRow, const LONG lCol, const ULONG ulCurBinMapPath, BOOL &bConfirmLastRow,
									    LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4]);
	LONG MoveToGoodDieIndexPosn(const ULONG ulCurIndex, const ULONG ulStartIndex,
								LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4]);
	VOID GetLookAheadPRAverageOffset(ULONG ulBlkInUse, ULONG ulCurIndex, ULONG ulPRLookAheadDirection, const LONG lCurEncX, const LONG lCurEncY,  const LONG lOffsetX, const LONG lOffsetY, 
									 const LONG lRowDir, const LONG lColDir, const LONG lLookAheadNum, const BOOL bLookAheadYDirection, LONG &lAvgOffsetX, LONG &lAvgOffsetY);
	VOID GetLookAheadValidData(const ULONG ulPRLookAheadDirection, LONG lOffsetX, LONG lOffsetY, LONG &lNewOffsetX, LONG &lNewOffsetY);

	ULONG GetCurBinPath(const ULONG ulWalkPath, const ULONG ulIndex, const ULONG ulDividend);
	VOID GetLookAheadYDirection(const ULONG ulWalkPath, const BOOL bRotate180, const ULONG ulRow, const ULONG ulCol, 
								const LONG lDiePitchX, const LONG lDiePitchY, CLookAheadMap acLookAheadDie[9]);
	ULONG GetLookAheadYDirectionWtihPreviousDie(const ULONG ulWalkPath, const BOOL bRotate180, LONG &lRowDir, LONG &lColDir);
	VOID GetLookAheadDirection(const ULONG ulWalkPath, const LONG lDiePitchX, const LONG lDiePitchY, CLookAheadMap &clLookAheadDirection);
	//v4.59A41
	BOOL FindLastRowColumnFrameAngle(const BOOL bDoCenterLineCompensate, const BOOL b1stTimeGobalAngle, const ULONG ulBinBlockID, const ULONG ulDieIndex, 
									 const DOUBLE dNewAngle, 
									 const BOOL bUpdateOffset, LONG& lNewXOffset, LONG& lNewYOffset, 
									 DOUBLE &dNewLastRowColAngle, DOUBLE& dNewCheckRealignmentAngle);			//v4.59A31
	
	BOOL RetryFindGoodDie(const LONG lPitchX, const LONG lPitchY, const LONG lDirX, const LONG lDirY, 
			  			  const LONG lDirection1, const LONG lDirection2, const LONG lDirection3, const BOOL bUseBT2, const BOOL bRetry);

	LONG FindTopLeftDie(const LONG lPitchX, const LONG lPitchY, const DOUBLE dFOVSize=1, const BOOL nUseBT2=FALSE, const BOOL bRetry = TRUE);
    LONG FindTopRightDie(const ULONG ulWalkPath, const LONG lPitchX, const LONG lPitchY, const DOUBLE dFOVSize=1, const BOOL nUseBT2=FALSE, const BOOL bRetry = TRUE);
    LONG FindBottomLeftDie(const ULONG ulWalkPath, const LONG lPitchX, const LONG lPitchY, const DOUBLE dFOVSize=1, const BOOL nUseBT2=FALSE, const BOOL bRetry = TRUE);
    LONG FindBottomRightDie(const LONG lPitchX, const LONG lPitchY, const DOUBLE dFOVSize=1, const BOOL nUseBT2=FALSE, const BOOL bRetry = TRUE);
	LONG Find9MatrixDieState(LONG lMethod, LONG lPitchX, LONG lPitchY, DOUBLE dFOVSize=1, BOOL nUseBT2=FALSE);
    LONG SearchDieInTopLeft	(const ULONG ulBlkInUse, const DOUBLE dFOVSize=1, const BOOL bUseBT2=FALSE, const BOOL bRetry = TRUE);
    LONG SearchDieInTopRight(const ULONG ulBlkInUse, const DOUBLE dFOVSize=1, const BOOL bUseBT2=FALSE, const BOOL bRetry = TRUE);
    LONG SearchDieInBottomLeft(const ULONG ulBlkInUse, const DOUBLE dFOVSize=1, const BOOL bUseBT2=FALSE, const BOOL bRetry = TRUE);
    LONG SearchDieInBottomRight(const ULONG ulBlkInUse, const DOUBLE dFOVSize=1, const BOOL bUseBT2=FALSE, const BOOL bRetry = TRUE);
	
	LONG ConvertXEncoderValueToFileUnit(LONG lXEncoderValue);
	LONG ConvertYEncoderValueToFileUnit(LONG lYEncoderValue);
    VOID GetXYEncoderValue(LONG &lX, LONG &lY, BOOL bUseBT2=FALSE);

    LONG BT_MoveTo(LONG lX, LONG lY, CONST BOOL bWait=TRUE, BOOL bUseBT2=FALSE);
    LONG BT_MoveToIndex(ULONG ulBlk, ULONG ulIndex);
	DOUBLE GetBPRFOVSize(ULONG ulBlkInUse);
	BOOL IsLargeDieSize();
	BOOL SearchGoodDieinFOV(LONG lDirection, LONG lLookAheadDieNum = 1);
	BOOL SearchGoodDieXYinFOV(LONG& lX, LONG& lY, LONG lDirection, LONG lLookAheadDieNum = 1, LONG lSearchRangeRatio = 1, BOOL bLookAheadDieOffset = FALSE);
	VOID DisplayBondPrSearchWindow(CONST BOOL bDisplay, CONST BOOL bLF, CONST LONG lDirection);	
	VOID DisplayBondPrDieSizeWindow(BOOL bDisplayHomeCursor);

	//MS100 9Inch dual-table realignment
//	LONG RealignBinBlock2(ULONG ulBinBlockID, BOOL bAuto, BOOL bAutoTeach, BOOL bDisableAlarm=FALSE, BOOL bDummyRun=FALSE);
//	LONG Realign1PtBinBlock2(ULONG ulBinBlockID, BOOL bAuto, BOOL bAutoTeach);
 //   LONG BT2_MoveTo(LONG lX, LONG lY, CONST BOOL bWait=TRUE);
//	LONG SearchBT2FirstGoodDie(ULONG ulBlkInUse);
//	LONG Search3rdGoodDie2(BOOL& bPlacement, DOUBLE& dX, DOUBLE& dY);


	//Learn collet offset
	BOOL LearnColletOffset(VOID);

	//Check Position is within table limit
	LONG CheckOutTableLimit(LONG lX, LONG lY);
	LONG CheckOutTable2Limit(LONG lX, LONG lY);

	//Slot assignment checking
	VOID CheckSlotAssignment();

	//Select & re-print label
	BOOL ReprintTSCLabel(CString szFileName, CString& szRetCode);
	BOOL ReprintDymoLabel(CString szFileName, CString& szRetCode);
	BOOL ReprintLabel(CString szFileName, CString& szRetCode);				//v2.72a5
	BOOL ReprintLabelWithXMLLayout(CString szFileName, CString& szRetCode);
	BOOL ReprintLabelWithSecondXMLLayout(CString szFileName, CString& szRetCode);
	BOOL InputBinBarcodeLabelToPrint();

	//Logging
	//VOID LOG_ReAlignBinFrame(CONST CString szMsg, CONST CString szMode);
	//VOID LOG_LoadUnloadTime(CONST CString szMsg, CONST CString szMode);		//v2.67
	//BOOL BackupReAlignBinLog();		//v2.93T2
	//BOOL BackupGenTempLog();		//v2.93T2

	ULONG GetGradeLeftOnWaferMap();

	LONG CheckWtPickAndPlaceOn();		//av2.83T2

	BOOL ClearSPCData(ULONG ulBlkId, BOOL bWaitReply, BOOL bClearAll);

	BOOL UpdateWaferLotLoadStatus();

	LONG ClrAllPhyBinCtr(CString szBinOutputFileFormat);

	BOOL MoveWaferTableToSafePosn(CONST BOOL bSafe);	//v3.61
	BOOL LetWaferTableDoAdvSampling(LONG lTgtGrade);

	BOOL UpdateBinBlkSetupParameters(ULONG ulBlkId);
	BOOL SetupBinBlk(CString szSetupMode, BOOL bIsMannualMode, BOOL bUpdateGradeData);
	BOOL SetupBinBlkForArrCode(ULONG ulBinBlkID, UCHAR ucType,
								DOUBLE dAreaXInUm, DOUBLE dAreaYInUm,
								UCHAR ucStartPos, ULONG ulMaxLoad, LONG lBinPitchX, LONG lBinPitchY,
								BOOL bReturnTravel);		//v4.42T11	//Nichia
	BOOL LoadBatchIdData();
	BOOL IsNeedCheckBatchIDFile();
	BOOL CheckBatchIdFile(CString szFilename);
	BOOL GetBatchIdInfoFilename(CString& szExtraInfoFilename);
	BOOL ReadBatchIdInfoFile(CString szFilename, CString szBarcodeNumber, CString& szBatchNo, CString& szBatchID, 
							CString& szProductName);

	VOID DecodeBatchIdInfoStr(CString szStr, CString& szBatchNo, CString& szBatchID, CString& szBarcodeNo, 
		CString& szProductName, CString& szStation);
	
	VOID ParseString(CString szStr, CString& szFirstPart, CString& szSecPart, CString szToken);

	//v3.86T5	//PB Runtime logging
	FILE* m_fBtLog;
	BOOL OPENCLOSE_BT_LOG(BOOL bOpen);
	BOOL LOG_BT(CONST CString szMsg);

	LONG LogItems(LONG lEventNo);
	LONG GetLogItemsString(LONG lEventNo, CString& szMsg);

	//v3.94	//PLLM Flat File implementation request
	BOOL AddPLLMFlatFileEntry(CONST ULONG ulBlkId);
	BOOL UploadPLLMFlatFileToServer(CONST CString szServerPath, CONST CString szBackupPath);
	
	//v4.50A10	//Cree HuiZhou GradeMapping Fcn
	BOOL ResetCreeGradeMappingEntry(CONST ULONG ulBlkID, BOOL bPrompt=TRUE, BOOL bResetSlotStatus=TRUE);

	//v4.39T7	//Nichia MS100+
	LONG ScanningBarcode(ULONG ulBlkID, BOOL bScanWithTheta=FALSE);
	LONG ScanBySymbol(CString& szBarcode, BOOL bScanWithTheta);
	BOOL ReadBarcode(CString *szData, int nTimeOut = 100);
	BOOL ManualInputBarcode(CString *szBarcode);

	//v4.49A4	//Cree HuiZhou
	BOOL CreateCreeStatisticReport(LONG lFrequency);
	LONG m_lCreeLastGenReportHour;
	BOOL m_lCreeStatisticsReportCounter;

	VOID LogCpkValue(ULONG ulBlkId, CString szFilePath, CString szBarcode);	//OsramTrip 8/22
	BOOL GetFrameSlotID(ULONG ulBlkNo, LONG &lMagNo, LONG &lSlotNo);
	BOOL CheckIfChangeFrameForSubBin(ULONG ulLast, ULONG ulCurrent);
	BOOL CheckIfOverlapForSubBin(ULONG X1, ULONG Y1, ULONG X2, ULONG Y2,ULONG X3, ULONG Y3, ULONG X4, ULONG Y4);
	LONG GetBTCurrentBlock();

	BOOL CalculateRotateBinTheta(LONG *lX, LONG *lY, LONG *lTheta, DOUBLE dDegree, LONG lCalibX, LONG lCalibY);

public:

	//For Wafer Map Event
//	unsigned char* GetListOfGradesWithBinBlksAssigned();
	BOOL GetIsBinBlkSetup(ULONG ulBinBlkId);
	UCHAR GetBinBlkGrade(ULONG ulBinBlkId);
	BOOL GetIfGradeIsAssigned(UCHAR ucGrade);
	unsigned long GetNoOfBinBlks();
	BOOL IfAllBinCountsAreCleared();
	BOOL IfNGBinCountsAreCleared();
	UCHAR IfAllBinCountsAreNotFull();		//v2.78T1
	VOID SetIsAllBinCntCleared(BOOL bIsAllBinCntCleared);
	BOOL OptimizeBinGrade(VOID);
	BOOL OptimizeBinCountPerWft();
	BOOL CheckLastSecondFrameDieFull(ULONG ulFullDieCountInput,ULONG ulMinDieCountInput, ULONG ulCurTotalDie,ULONG& ulResetFullDieCount, ULONG ulGradeDieRemainCount);
	VOID DisplayOptimizeInfo(VOID);
	
	VOID ResetWaferEndFileData(VOID);
	VOID SetNotGenerateWaferEndFile(BOOL bNotGenFile);
	BOOL WaferEndFileGeneratingChecking();
	BOOL WaferEndFileGenerating(CString szMsg, BOOL bCheckYield = FALSE);
	BOOL AutoWaferEndFileGenerating();								//v4.40T14
	BOOL CheckValidRankIDFromCurrentMap(BOOL bClearMap=TRUE);		//v4.21T4	//TongFang
	BOOL LoadRankIDFromMap();
	VOID CheckNewHoopsToLeaveEmptyRow();							//v4.30T3	//CyOptics US
	BOOL CheckNichiaArrCodeInMap();									//v4.42T2	//Nichia
	LONG ValidateNichiaArrCodeInMap(IPC_CServiceMessage& svMsg);	//v4.42T12
	LONG CalculateBinCapacityForArrCode(IPC_CServiceMessage& svMsg);	//Nichia//v4.43T7
	LONG DrawBinMap(IPC_CServiceMessage& svMsg);						//Nichia//v4.43T7

	BOOL UpdateInputCountFromSetupFile(CInputCountSetupFile& oCountSetupFile);
	BOOL UpdateWaferMapHeader();
	VOID SetBT1FrameLevel(BOOL bSet);	//v4.42T7
	BOOL IsBT1FrameLevel(VOID);			//v4.42T7

	LONG GetBHZ1BondPosOffsetX();
	LONG GetBHZ1BondPosOffsetY();
	LONG GetBHZ2BondPosOffsetX();
	LONG GetBHZ2BondPosOffsetY();

	VOID SetAOTClearBinCounterDefaultSetting();

	BOOL LoadPackageMsdBTData(VOID);
	BOOL SavePackageMsdBTData(VOID);
	BOOL UpdateBTPackageList(VOID);

	VOID GenerateWaferEndTime();
	VOID SetRealignBinFrameReq(const BOOL bRealignBinFrameReq, const ULONG ulRealignBinBlkID, const BOOL bUseEmpty, const BOOL bUseBT2);
	VOID GetGradeCapacity(const LONG lSelectedGradeNum, UCHAR aucGrade[BT_MAX_BINBLK_NO], ULONG aulGradeCapacity[BT_MAX_BINBLK_NO]);
	VOID SaveBTAdjCollet1Offset_um(const DOUBLE dBHZ1AdjBondPosOffsetX_um, const DOUBLE dBHZ1AdjBondPosOffsetY_um);
	VOID SaveBTAdjCollet2Offset_um(const DOUBLE dBHZ2AdjBondPosOffsetX_um, const DOUBLE dBHZ2AdjBondPosOffsetY_um);

	LONG BTChangeNoDiePosition(IPC_CServiceMessage& svMsg);
	LONG BTConfirmNoDiePositionSetup(IPC_CServiceMessage& svMsg);

	BOOL IsBinFrameRotated180();
	LONG GetBlock(UCHAR ucGrade);
	VOID SetUseBinMapBondArea();
	BOOL UpdateBondPosOffsetAtBPR(BOOL bBH2,int siStepX,int siStepY);
	BOOL MoveTableToEmptyPosition();
	BOOL CheckIfGenerateWaferEnd(CString szWaferId, LONG lPickTotal, LONG lMapTotal);
	DOUBLE GetWaferYield(CString szMsg);
	BOOL BinTableThetaCompensation(DOUBLE dDegree);


private:

	VOID GetSensorValue();		// Get all sensor values from hardware
	VOID GetEncoderValue();		// Get all encoder values from hardware
	LONG GetTEncoderValue(CString szAxis);
	VOID GetAxisInformation();	//NuMotion

	// Set Event (BT_Event.cpp)
	VOID SetBTReady(BOOL bState = TRUE);
	VOID SetBTStable(BOOL bState = TRUE);
	VOID SetDieReadyForBT(BOOL bState = TRUE);
	VOID SetBPRLatched(BOOL bState = TRUE);
	VOID SetDieBonded(BOOL bState = TRUE);
	VOID SetDieInfoRead(BOOL bState = TRUE);
	VOID SetBLOperate(BOOL bState = TRUE);
	VOID SetBLReady(BOOL bState = TRUE);
	VOID SetBhTReady(BOOL bState = TRUE);
	VOID SetBTStartMoveWithoutDelay(BOOL bState = TRUE);
	VOID SetBTStartMove(BOOL bState = TRUE);
	VOID SetBLAOperate(BOOL bState = TRUE);
	VOID SetBLPreOperate(BOOL bState = TRUE);
	VOID SetBLPreChangeGrade(BOOL bState = TRUE);	//v2.71
	VOID SetBTCompensate(BOOL bState = TRUE);		//v4.52A16
	VOID SetWTStartToMoveForBT(BOOL bState = TRUE, CString szTemp = "SetWTStartToMoveForBT");
	BOOL SaveWaferDieCount();
	//andrewng //2020-0708
	VOID SetNLReady(BOOL bState = TRUE);
	VOID SetNLOperate(BOOL bState = TRUE);
	VOID SetNLPreOperate(BOOL bState = TRUE);

	// Wait Event (BT_Event.cpp)
	BOOL WaitDieReadyForBT(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitDieInfoUnRead(INT nTimeout = LOCK_TIMEOUT);
	UCHAR GetWTGrade();			//	map display grade on HMI.
	ULONG GetBTBlock();			//v4.52A6	//Modified UCHAR to ULONG
	BOOL WaitMoveComplete(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBPRLatched(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitDieBonded(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBHInit(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBLReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBLAOperate(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBLPreOperate(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBLPreChangeGrade(INT nTimeout = LOCK_TIMEOUT);		//v2.71
	BOOL WaitBTCompensate(INT nTimeout = LOCK_TIMEOUT);			//v4.52A16
	BOOL WaitWTStartToMoveForBT(INT nTimeout = LOCK_TIMEOUT);
	//andrewng //2020-0708
	BOOL WaitNLReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitNLPreOperate(INT nTimeout = LOCK_TIMEOUT);

	BOOL IsMotorXPowerOn();
	BOOL IsMotorYPowerOn();
	BOOL IsMotorXYReady();
	BOOL IsMotionHardwareReady();		// Check whether the motion hardware are ready

	//BOOL LOG_BT(CONST CString szMsg);
	//BOOL Backup_BT_LOG();		//v2.93T2
	BOOL LoadWaferDieCount();
	
	// Load Extra Info from feature for clear bin and output file
	BOOL LoadExtraClearBinInfo();

	BOOL IM_WaferEndChecking();
	BOOL ResetMapDieType();

	// Axis Test functions
	BOOL MotionTest(VOID);

	//v3.74T45	//v4.03	//PLLM/PLSG Binmap fcn
	BOOL InitBinBlkBinMap();	//Nichia//v4.43T7
	BOOL CreateBinMap(CONST BOOL bCreate=TRUE);
	BOOL CreateBinMap2(CONST LONG lCX, CONST LONG lCY, CONST DOUBLE dBondRadius, CONST BOOL bCreate=TRUE);				//v4.36
	BOOL IsWithinCircularBinMapLimit(DOUBLE dRadius, DOUBLE dCX, DOUBLE dCY, DOUBLE dMapX, DOUBLE dMapY);				//v4.36
	BOOL IsWithinCircularBinMapLimit2(DOUBLE dRadius, DOUBLE dCX, DOUBLE dCY, DOUBLE dDieX, DOUBLE dDieY);				//v4.36
	BOOL IsWithinOvalBinMapLimit(DOUBLE dCX, DOUBLE dCY, DOUBLE dMapX, DOUBLE dMapY, 
									DOUBLE dRadiusX, DOUBLE dRadiusY);	//v4.36T2
	BOOL SetBinMapEdgeDices();
	BOOL LoadBinMap(const BOOL bInInitData, ULONG &ulBinCount);
	BOOL SaveBinMap(ULONG &ulNewInputCount);
	VOID BackupBinMap(BOOL bFromInputPathToCur = TRUE);
	BOOL LoadBinMapData(ULONG ulBlkID);					//v4.42T6
	ULONG GetBinMapCount(ULONG ulBlkID);				//v4.49A1
	//v4.47T13	//Osram Germany
	BOOL CreateOsramBinMixMap(BOOL bCreate=TRUE);	
	BOOL LoadOsramBinMixMap();							//v4.47A6
	BOOL SaveOsramBinMixMap(ULONG &ulNewInputCount);	//v4.47A6

	CString GetWaferPrDeviceID();
	//ChangeLight request 1:	//v4.60A2
	BOOL RecordNewBinTimeToFile(CString szFileName);

	LONG DoRealignBinFrame();
	LONG BPR_ZoomSensor(LONG lZoomMode);
	BOOL BPR_IsSensorZoomFFMode();
	
	VOID SetBHStrongBlow(const BOOL bBHZ2, const BOOL bSet);
	VOID SetBHPickVacuum(const BOOL bBHZ2, const BOOL bSet);
	VOID BTColletCleanMove(const BOOL bBHZ2, const LONG lX1, const LONG lY1, const LONG lX2, const LONG lY2);
protected:

	// Operation sub-state
	enum {	WAIT_DIE_READY_Q,
			INDEX_Q,
			WAIT_BPR_LATCHED_Q,
			MOVE_TABLE_Q,
			WAIT_BT_READY_Q,
			WAIT_DIE_BONDED_Q,
			NEXT_DIE_Q,
			MOVE_BACK_Q,
			BIN_FULL_Q,
			BL_CHANGE_GRADE_Q,
			WAIT_BL_CHANGE_READY_Q,
			BL_BIN_FULL_Q,
			WAIT_BL_BIN_FULL_READY_Q,
			HOUSE_KEEPING_Q,
			MOVE_TO_UNLOAD_Q,
			BL_CHANGE_GRADE_WAIT_BH_Q,			//v3.70T1	//FOr DBH sequence only
			BL_BIN_FULL_WAIT_BH_Q
		};


protected:

	// Axis Info
	CMSNmAxisInfo	m_stBTAxis_X;
	CMSNmAxisInfo	m_stBTAxis_Y;
	CMSNmAxisInfo	m_stBTAxis_X2;	//9INCH_MS		//v4.16T1
	CMSNmAxisInfo	m_stBTAxis_Y2;	//9INCH_MS		//v4.16T1
	CMSNmAxisInfo	m_stBTAxis_T;	//MS100 Nichia

	INT				m_nBTInUse;		// 0=BT(DEfault); 1=BT2(9INCH_MS)	//v4.16T1

	BOOL	m_bIsPowerOn_X;			//Whether X axis is on
	BOOL	m_bIsPowerOn_Y;			//Whehter Y axis is on
	BOOL	m_bIsPowerOn_T;			//Whehter T axis is on
	BOOL	m_bHome_X;				// X Home flag
	BOOL	m_bHome_Y;				// Y Home flag
	BOOL	m_bHome_T;				// T Home flag
	BOOL	m_bComm_X;				// X Communtate flag
	BOOL	m_bComm_Y;				// Y Communtate flag
	BOOL	m_bComm_T;				// T Communtate flag
	BOOL	m_bSel_X;				// Select X flag
	BOOL	m_bSel_Y;				// Select Y flag
	BOOL	m_bSel_T;				// Select T flag

	BOOL	m_bIsPowerOn_X2;		//Whether X2 axis is on
	BOOL	m_bIsPowerOn_Y2;		//Whehter Y2 axis is on
	BOOL	m_bHome_X2;				// X2 Home flag
	BOOL	m_bHome_Y2;				// Y2 Home flag
	BOOL	m_bComm_X2;				// X2 Communtate flag
	BOOL	m_bComm_Y2;				// Y2 Communtate flag
	BOOL	m_bSel_X2;				// Select X2 flag
	BOOL	m_bSel_Y2;				// Select Y2 flag

	LONG	m_lCurPos_T;			//T curr Encoder/CMD position	//v4.59A33

	BOOL	m_bUpdateOutput;		// Update Sensor & encoder in HMI
	BOOL	m_bCreated;				// Physical Block creation flag
	BOOL	m_bBinBlkCreated;		// Bin Block creation flag
	BOOL	m_bStopBinLoader;	
	BOOL	m_bIsSetupBinBlk;

	// Resort Mode
	BOOL	m_bIfEnableResortDie;

	// Event 
	CEvent	m_evDieReadyForBT;			// WPR inspected a good die
	CEvent	m_evBTReady;			// Bin Table ready for bonding
	CEvent	m_evBTStable;			// Bin Table stable for BPR
	CEvent	m_evBPRLatched;			// BPR Grab Done
	CEvent	m_evDieBonded;			// Die is Bonded
	CEvent	m_evDieInfoRead;		// Die Info is read
	CEvent	m_evBHInit;				// Bond Head is initiated event
	CEvent	m_evBhTReady;			// T ready for BPR start grabbing
	CEvent	m_evBLOperate;			// Bin Loader Operate event
	CEvent	m_evBLReady;			// Bin Loader Ready event
	CEvent	m_evBTStartMove;		// Bin Table Start Move event
	CEvent	m_evBTStartMoveWithoutDelay; // Bin Table Start Move without delay event
	CEvent	m_evBLAOperate;			// Bin Loader - A Operate event
	CEvent	m_evBLPreOperate;		// Bin Loader Pre Operate event
	CEvent  m_evBLPreChangeGrade;	// Bin Loader Pre-load next grade frame for DL config
	CEvent	m_evBTCompensate;		// BH to trigger thie event for BT compensation		//v4.52A16
	CEvent	m_evWTStartToMoveForBT;	// WT toggle BT move for NVC
	//andrewng //2020-0708
	CEvent	m_evNLOperate;			// NVC Loader - A Operate event
	CEvent	m_evNLReady;			// NVC Loader Ready event
	CEvent	m_evNLPreOperate;		// NVC Loader Pre Operate event

	// Delay
	LONG	m_lBTDelay;				// Delay for Bin Table stable (Long Distance = Normal Profile)
	LONG	m_lBTDelay_Short;		// Delay for Bin Table stable (Short Distance = Fast Profile)

	// Motion Time
	LONG	m_lTime_BT;
	DOUBLE  m_dBTStartTime;
	LONG	m_lBTMoveDelayTime;
	DOUBLE	m_dDelayTime_X;
	DOUBLE	m_dDelayTime_Y;

	// Min. travel dist
	LONG	m_lMinTravel_X;
	LONG	m_lMinTravel_Y;

	// Move position 
	LONG	m_lX;
	LONG	m_lY;
	LONG	m_lRow;
	LONG	m_lCol;
	LONG	m_lLastX;
	LONG	m_lLastY;
	LONG	m_lLastX2;
	LONG	m_lLastY2;
	BOOL	m_bFirstDie;
	INT		m_nMultiSeachCounter; //4.51D20
	INT		m_nMultiUnfondCounter; //4.51D20b
	LONG	m_lDoubleCheckFirstDie;
	LONG	m_lTempMultiSearchX;
	LONG	m_lTempMultiSearchY;
	LONG	m_lAGCX;
	LONG	m_lAGCY;

	// Profile selected
	INT		m_nProfile_X;
	INT		m_nProfile_Y;
	INT		m_nProfile_X2;
	INT		m_nProfile_Y2;

	/*--- Server Variables ---*/
	// Sensor state
	BOOL	m_bHomeSnr_X;		// X Home Sensor
	BOOL	m_bHomeSnr_Y;		// Y Home Sensor
	BOOL	m_bPosLimitSnr_X;	// X Positive Limit Sensor
	BOOL	m_bNegLimitSnr_X;	// X Negative Limit Sensor
	BOOL	m_bPosLimitSnr_Y;	// Y Positive Limit Sensor
	BOOL	m_bNegLimitSnr_Y;	// Y Negative Limit Sensor
	BOOL	m_bPosLimitSnr_X2;	// X Positive Limit Sensor
	BOOL	m_bNegLimitSnr_X2;	// X Negative Limit Sensor
	BOOL	m_bPosLimitSnr_Y2;	// Y Positive Limit Sensor
	BOOL	m_bNegLimitSnr_Y2;	// Y Negative Limit Sensor

	// Encoder counter	
	LONG	m_lEnc_X;			// X Encoder count
	LONG	m_lEnc_Y;			// Y Encoder count
	LONG	m_lEnc_T;			// T Encoder count
	LONG	m_lEnc_X2;			// X2 Encoder count		//9INCH_MS
	LONG	m_lEnc_Y2;			// Y2 Encoder count		//9INCH_MS
	LONG	m_lCmd_X;			// X Encoder count		//CSP
	LONG	m_lCmd_Y;			// Y Encoder count		//CSP

	// Wafer Table Information
	LONG	m_lWT_Col;			// Wafer Map Column
	LONG	m_lWT_Row;			// Wafer Map Row
	UCHAR	m_ucWTGrade;		// Wafer Map Grade
	ULONG	m_ulBondedBlk;		// Block to be bonded
	ULONG	m_ulBpBondedBlk;	// Previous backup block ID for pre-fetch frame checking

	// BT1 Encoder backup
	LONG	m_lBT1X;
	LONG	m_lBT1Y;

	//v4.51A19	//Silan MS90 NGPick
	LONG	m_lNGPickPocketX;
	LONG	m_lNGPickPocketY;

	CString m_szBTConfigFileName;
	ULONG m_ulMachineType;	//1 = Standard Bin Table; 2 = Bin Loader (for temporary use only); 3 = dual-Tables

	//Encoder Resolution
	DOUBLE	m_dXResolution_UM_CNT;
	DOUBLE	m_dYResolution_UM_CNT;
	DOUBLE	m_dXResolution;
	DOUBLE	m_dYResolution;
	DOUBLE	m_dThetaRes;		//v4.39T7	//Nichia MS100+
	DOUBLE	m_BT_T_RESOLUTION;
	LONG	m_lThetaMotorDirection;

	LONG	m_lX_ProfileType;
	LONG	m_lY_ProfileType;

	//BinTable Limit
	LONG	m_lXNegLimit;
	LONG	m_lXPosLimit;
	LONG	m_lYNegLimit;
	LONG	m_lYPosLimit;
	LONG	m_lX2NegLimit;
	LONG	m_lX2PosLimit;
	LONG	m_lY2NegLimit;
	LONG	m_lY2PosLimit;

	//BT Machine Configuration
	BOOL	m_bUseLargeBinArea;
	BOOL	m_bUseDualTablesOption;		//MS100 9Inch dual-table option	
	BOOL	m_bEnable_T;				//MS100 Nichia
	BOOL	m_bUseTEncoder;
	
	BOOL	m_bUseFrameCrossAlignment;	//v4.59A1	//MS90 WolfSpeed

	//LONG	m_lJoystickLevel;	// Joystick Speed Level
	BOOL	m_bJoystickOn;		// Joystick state

	//Block Setup Display
	LONG	m_lBlocksDisplayResol; 
	CBinSetupWrapper m_BinDisplayWrapper;
	CBinSetupWrapper m_BinSetupWrapper;
	CBinSetupWrapper m_PhySetupWrapper;
	CGradeLegendWrapper m_GradeLegendWrapper;
	//Wafer Statistics
	CBinTotalWrapper m_BinTotalWrapper;
	//Grade RankingID
	CBinTotalWrapper m_GradeRankWrapper;

	//The Reference Cross (Citizen)
	LONG	m_lEncRefCross_X;
	LONG	m_lEncRefCross_Y;
	LONG	m_lEncRefCross_T;
	DOUBLE	m_dBinTwoCrossXDistance;
	DOUBLE	m_dBinTwoCrossYDistance;
	DOUBLE	m_dThetaOffsetByCross;

	//Output File
	CString m_szOutputFilePath;
	CString m_szOutputFilePath2;
	CString m_szOutputFilePath3;		//v4.44T2
	CString m_szOutputFilePath4;		//v4.44T2
	CString m_szBinOutputFileFormat;
	CString m_szLabelPrinterSel;
	BOOL	m_bValidOutputFilePath2;
	BOOL	m_bCheckOutputFileRepeat; // SanAn 4.51D6
	CString m_szOutputFileBlueTapeNo; // v4.51D10 Dicon
	BOOL	m_bUseBackupPath;

	//Output wafer end file
	CString m_szWaferEndPath;
	CString m_szWaferEndFileFormat;

	CString m_szOutputFileSummaryPath;
	CString	m_szWaferEndSummaryPath;

	//Bin Summary Output File
	CString	m_szBinSummaryOutputPath;

	//Bin Count Check
	BOOL m_bIsAllBinCntCleared;

    //For bin loader load/unload
    BOOL m_bBinLoadState;

    //For Realign bin block
	BOOL m_bUseSameView;
    BOOL m_bMatrix_9[9];
	BOOL m_bAlignBinInAlive;
	LONG m_lAlignBinDelay;
	BOOL m_bCheckAlignBinResult;
	DOUBLE m_dAlignLastDieOffsetX;		//v2.63
	DOUBLE m_dAlignLastDieOffsetY;		//v2.63
	DOUBLE m_dAlignLastDieRelOffsetX;	//v4.26T1	//Semitek
	DOUBLE m_dAlignLastDieRelOffsetY;	//v4.26T1	//Semitek
	BOOL m_bManualConfirmLastDieResult1;	//v4.36		//PLLM MS109
	BOOL m_bManualConfirmLastDieResult2;	//v4.36		//PLLM MS109
	LONG m_lManualConfirmXOffset;
	LONG m_lManualConfirmYOffset;

	//For Auto clear bin
	ULONG m_ulBinBlkFull;
	BOOL m_bIsUnload;
	BOOL m_bAutoClearBin;

	//Display binblk image when binfull
	BOOL m_bShowBinBlkImage;

	BOOL	m_bNVRamUpdatedInIfBondOK;		//v4.59A4	//MS60 CT slow donw issue at every 500 dices

	//For DLA
	ULONG	m_ulGradeChangeLimit;
	BOOL	m_bPreLoadFullFrame;			//v2.67
	BOOL	m_bPreLoadChangeFrame;			//v2.67
	BOOL	m_bDisableClearAllPrintLabel;	//v2.70
	BOOL	m_bPreLoadNextMapGradeFrame;	//v2.78T1

	CTime	m_stStartChangeTime;
	UINT	m_unChangeStage;
	UINT	m_ulNewBondedCounter;

	BOOL	m_bEnable2DBarcodeOutput;	//Enable bin output 2D barcode (for Cree) //v3.33T3
	ULONG	m_ulHoleDieNum;		//Number of hole in bin output 2D barcode		//v3.33T3

	LONG	m_lRealignBinFrameOption;		//v3.70T3	//PLLM
	BOOL	m_b1ptRealignComp;				//v3.86

	BOOL	m_bPrPreBondAlignment;			//v3.79
	//CSP
	LONG	m_lPreBondTableOffsetX;
	LONG	m_lPreBondTableOffsetY;
	LONG	m_lCollet1PadOffsetX;
	LONG	m_lCollet1PadOffsetY;
	LONG	m_lCollet2PadOffsetX;
	LONG	m_lCollet2PadOffsetY;

	// Axis Test Variable
	BOOL	m_bMoveDirection;
	LONG	m_lMotionTestDelay;
	LONG	m_lMotionTestDist;
	BOOL	m_bIsMotionTestBinTableX;
	BOOL	m_bIsMotionTestBinTableX2;
	BOOL	m_bIsMotionTestBinTableY;
	BOOL	m_bIsMotionTestBinTableY2;

	CBinTableDataBlock m_oBinTableDataBlock;

	// Last Wafer End Time
	CTime	m_stLastWaferEndTime;
	BOOL	m_bIsLastWaferEndTimeSaved;

	// Process Debug
	LONG	m_lBinTableDebugCounter;

	// Copy Temp File Only
	BOOL	m_bClearBinInCopyTempFileMode;
	//BOOL	m_bEnableClearBinCopyTempFile;

	LONG	m_lBTNoDiePosX, m_lBTNoDiePosY;		//Coor of BT No Die

	//andrewng //2020-0630
	LONG	m_lTempFileBinCol;
	LONG	m_lTempFileBinRow;
	LONG	m_lTempFileWTEncX;
	LONG	m_lTempFileWTEncY;
	LONG	m_lTempFileBTEncX;
	LONG	m_lTempFileBTEncY;
	//andrewng //2020-0902
	LONG	m_lUnloadPhyPosX;		
	LONG	m_lUnloadPhyPosY;
	LONG	m_lUnloadPhyPosT;


private:

	/***************************/
	/*     HMI Reg Commands    */
	/***************************/
	LONG UpdateOutput(IPC_CServiceMessage& svMsg);

	LONG UpdateAction(IPC_CServiceMessage& svMsg);
	LONG LogItems(IPC_CServiceMessage& svMsg);

	/********** Check Grade Assignment before cycle start **********/
	LONG CheckGradeAssignment(IPC_CServiceMessage& svMsg);

	/********** For Warm Start after package file restore **********/
	LONG ClearAllBinCounters(IPC_CServiceMessage& svMsg);
	LONG CheckIsAllBinCleared(IPC_CServiceMessage& svMsg);
	LONG CheckIsBinCleared(IPC_CServiceMessage& svMsg);				//v4.50A12	//Cree HZ
	LONG CheckIsAllBinCleared_PKG(IPC_CServiceMessage& svMsg);
	LONG CheckIsAllBinClearedNoMsg(IPC_CServiceMessage& svMsg);
	LONG CheckIsAllBinClearedWithDieCountConstraint(IPC_CServiceMessage& svMsg);
	LONG CheckIsAllBinNotFull(IPC_CServiceMessage& svMsg);			//v2.78T1

	/********** For Pick and Place use **********/
	LONG IfBinCntIsClrB4PickAndPlace(IPC_CServiceMessage& svMsg);

	/********** Wafer Statistics *********/
	LONG WaferStatisticsPreTask(IPC_CServiceMessage& svMsg);

	/********** Bin Summary Output *********/
	LONG BinSummaryOutput(IPC_CServiceMessage& svMsg);

	/********** Machine No *********/
	LONG SaveMachineNo(IPC_CServiceMessage& svMsg);

	/********** Clear Block Settings **********/
	LONG IfIsClearAllSettings(IPC_CServiceMessage& svMsg);

	/********** Clear Physical Block Settings **********/
	LONG SubmitClrPhyBlkSettings(IPC_CServiceMessage& svMsg);

	/********** Clear Bin Block Settings **********/
	LONG SubmitClrBinBlkSettings(IPC_CServiceMessage& svMsg);

	/********** Physical Block Setup **********/
	LONG PhyBlkSetupPreRoutine(IPC_CServiceMessage& svMsg);
	LONG OnChangePhyBlkSetupModeSelection(IPC_CServiceMessage& svMsg);
	LONG OnClickNoOfPhyBlkInput(IPC_CServiceMessage& svMsg);
	LONG OnClickPhyBlkToSetupInput(IPC_CServiceMessage& svMsg);
	LONG CheckIfNoOfPhyBlksIsValid(IPC_CServiceMessage& svMsg);
	LONG CheckIfPhyBlkToSetupIsValid(IPC_CServiceMessage& svMsg);
	LONG CheckIfContainsBinBlks(IPC_CServiceMessage& svMsg);
	LONG SetPhyBlkSetupUL(IPC_CServiceMessage& svMsg);
	LONG ConfirmPhyBlkSetupUL(IPC_CServiceMessage& svMsg);
	LONG SetPhyBlkSetupLR(IPC_CServiceMessage& svMsg);
	LONG ConfirmPhyBlkSetupLR(IPC_CServiceMessage& svMsg);
	LONG SubmitPhyBlkSetup(IPC_CServiceMessage& svMsg);
	LONG CancelPhyBlkSetup(IPC_CServiceMessage& svMsg);
	LONG DisplayPhyBlkData(IPC_CServiceMessage& svMsg);
	LONG MoveToPhyBlk(IPC_CServiceMessage& svMsg);
	LONG CheckIfClrBlkSettingsPossible(IPC_CServiceMessage& svMsg);
	LONG SubmitSubBinPhyBlkSetup(IPC_CServiceMessage& svMsg);
	LONG SubBinReset(IPC_CServiceMessage& svMsg);
	LONG SubBinCheckAZone(IPC_CServiceMessage& svMsg);
	LONG SubBinCheckBZone(IPC_CServiceMessage& svMsg);
	LONG SubBinCheckCZone(IPC_CServiceMessage& svMsg);
	LONG SubBinCheckDZone(IPC_CServiceMessage& svMsg);

	/********** Bin Block Setup **********/
	LONG BinBlkSetupPreRoutine(IPC_CServiceMessage& svMsg);
	LONG IfProceedWithCommandsBelow(IPC_CServiceMessage& svMsg);
	LONG CheckIfNoOfBinBlksIsValid(IPC_CServiceMessage& svMsg);
	LONG CheckIfBinBlkToSetupIsValid(IPC_CServiceMessage& svMsg);
	LONG CheckIfBondingInProcess(IPC_CServiceMessage& svMsg);
	LONG SetBinBlkSetupUL(IPC_CServiceMessage& svMsg);
	LONG ConfirmBinBlkSetupUL(IPC_CServiceMessage& svMsg);
	LONG SetBinBlkSetupLR(IPC_CServiceMessage& svMsg);
	LONG ConfirmBinBlkSetupLR(IPC_CServiceMessage& svMsg);
	LONG CheckIfBlockHasDie(IPC_CServiceMessage& svMsg);
	LONG SetWafflePadDestX(IPC_CServiceMessage& svMsg);
	LONG ResetWafflePadDestX(IPC_CServiceMessage& svMsg);
	LONG SetWafflePadDestY(IPC_CServiceMessage& svMsg);
	LONG ResetWafflePadDestY(IPC_CServiceMessage& svMsg);
	LONG SubmitBinBlkSetup(IPC_CServiceMessage& svMsg);

	VOID OnBinBlkSetupNo(ULONG ulBinBlkId);
	VOID OnBinBlkSetupMode(CString szSetupMode);
	LONG AllBlocksSetup(IPC_CServiceMessage& svMsg);

	LONG CancelBinBlkSetup(IPC_CServiceMessage& svMsg);
	LONG ShowBinBlkTemplateData(IPC_CServiceMessage& svMsg);
	LONG LoadBinBlkTemplate(IPC_CServiceMessage& svMsg);
	ULONG GetWalkPathWithHMIName(CString szWalkPathHMI);
	CString GetWalkPathHMIName(ULONG ulWalkPath);
	LONG SaveBinBlkTemplate(IPC_CServiceMessage& svMsg);
	LONG RemoveBinBlkTemplate(IPC_CServiceMessage& svMsg);
	LONG DisplayBinBlkData(IPC_CServiceMessage& svMsg);
	LONG MoveToBinBlk(IPC_CServiceMessage& svMsg);
	LONG MoveToBinBlk2(IPC_CServiceMessage& svMsg);						//MS100 9Inch	//v4.21T1
	LONG MoveToBinBlk1stDie(IPC_CServiceMessage& svMsg);				//v2.78T1
	LONG OnChangeBinBlkSetupModeSelection(IPC_CServiceMessage& svMsg);
	LONG OnClickNoOfBinBlkInput(IPC_CServiceMessage& svMsg);
	LONG OnClickBinBlkToSetupInput(IPC_CServiceMessage& svMsg);
	LONG TeachCircularAreaCenterPos(IPC_CServiceMessage& svMsg);		//v3.71T9
	LONG Test(IPC_CServiceMessage& svMsg);								//v3.67T5
	LONG GenerateConfigData(IPC_CServiceMessage &svMsg);				//v3.86
	LONG GeneratePkgDataFile(IPC_CServiceMessage &svMsg);				//v4.21T7	//Walsin China
	LONG UpdateBondAreaOffset(IPC_CServiceMessage &svMsg);				//v4.42T9	//Citizen
	LONG PkgKeyParametersTask(IPC_CServiceMessage &svMsg);

	//All Bin Blocks Setup
	LONG IfAllBinBlksSetupIsAllowed(IPC_CServiceMessage& svMsg);

	//Single Bin Block Setup
	LONG IfBondingInProcess(IPC_CServiceMessage& svMsg);
	LONG IfDisabledFromSameGradeMerge(IPC_CServiceMessage& svMsg);

	/************* Clear Bin Counter (by Physical Block) ************/
	LONG ClrPhyBinCtrPreRoutine(IPC_CServiceMessage& svMsg);
	LONG ClrPhyBinCtr(IPC_CServiceMessage& svMsg);
	LONG OnChangeClrBinCntModeSelection(IPC_CServiceMessage& svMsg);
	LONG SaveOutputFilePath(IPC_CServiceMessage& svMsg);
	LONG SaveOutputFileFormat(IPC_CServiceMessage& svMsg);
	LONG SaveOutputFileBlueTapeNoFormat(IPC_CServiceMessage& svMsg); // v4.51D10 Dicon
	LONG SubmitGenerateSummaryFile(IPC_CServiceMessage& svMsg);
	LONG SubmitClrPhyBinCtr(IPC_CServiceMessage& svMsg);
	LONG InputOutputFilePath(IPC_CServiceMessage& svMsg);
	LONG InputOutputFileFormat(IPC_CServiceMessage& svMsg);
	LONG GetOutputFilePath(IPC_CServiceMessage &svMsg);
	LONG GetOutputFileSummaryPath(IPC_CServiceMessage& svMsg);
	LONG InputOutputFilePath2(IPC_CServiceMessage& svMsg);
	LONG GetOutputFilePath2(IPC_CServiceMessage &svMsg);
	LONG GetOutputFilePath3(IPC_CServiceMessage &svMsg);
	LONG GetOutputFilePath4(IPC_CServiceMessage &svMsg);
	LONG SaveOutputFilePath2(IPC_CServiceMessage& svMsg);
	LONG CheckPhyBlkID(IPC_CServiceMessage& svMsg);			//v4.1T1	//Ubiliux

	/************* Clear Bin Counter (by Grade) *****************/
	// Without Generating OutputFile
	LONG ClrBinCtrByGrade(IPC_CServiceMessage& svMsg);
	// With Generating OutputFile
	LONG SubmitClrBinCtrByGrade(IPC_CServiceMessage& svMSg);

	/************ Clear Bin Counter (by both Grade and Physical Block) ***************/
	LONG ClrBinCtrPreRoutine(IPC_CServiceMessage& svMsg);

	/************* Clear All Bin Counter (For BurnIn Use) ***************/
	LONG BurnInClearCounter(IPC_CServiceMessage& svMsg);

	/************* Clear Bin Counter (for DL machine) *************/
	LONG AutoClrBinCnt(IPC_CServiceMessage& svMsg);
	LONG AutoClrBinCnt_SECSGEM(IPC_CServiceMessage& svMsg);			//v4.59A19
	LONG GetBinNumber(IPC_CServiceMessage& svMsg);	
	LONG GetBinGrade(IPC_CServiceMessage& svMsg);

	/************* Generate Bin Summary Output File *************/
	LONG InputBinSummaryOutputPath(IPC_CServiceMessage& svMsg);	
	LONG SaveBinSummaryOutputPath(IPC_CServiceMessage& svMsg);
	LONG GetBinSummaryOutputPath(IPC_CServiceMessage &svMsg);

	/********************* Step Move ************************/
	LONG StepMovePreRoutine(IPC_CServiceMessage& svMsg);
	LONG DisplayBinBlkInfoForStepMove(IPC_CServiceMessage& svMsg);
	LONG MoveToNewIndex(IPC_CServiceMessage& svMsg);
	LONG UpdateBlkIndex(IPC_CServiceMessage& svMsg);
	LONG SetEnableMS90Rotation180(IPC_CServiceMessage& svMsg);	
	LONG BinSetMS90Rotation(BOOL bRoation180, BOOL bMsgBox=FALSE);
	LONG EnterBinStepMovePage(IPC_CServiceMessage& svMsg);
	LONG ExitBinStepMovePage(IPC_CServiceMessage& svMsg);
	LONG MultiRealignFirstDie(IPC_CServiceMessage& svMsg);	
	LONG MS90RotateBT180(IPC_CServiceMessage& svMsg);

	LONG GoAndSearchAllBinBlockDie(IPC_CServiceMessage &svMsg);
	LONG GoAndSearchAllBinBlockDie2(IPC_CServiceMessage &svMsg);		//andrewng //2020-0803
	LONG StopOfflinePostbondTest(IPC_CServiceMessage &svMsg);			//andrewng //2020-0804
	BOOL OfflinePostBondTest1();
	BOOL OfflinePostBondTest2();
	BOOL m_bDoOfflinePostbondTest;
	BOOL m_bDoOfflinePostbondTest2;
	LONG m_lIndexToMoveTo;
	LONG m_lOffPBTestLFStepX;
	LONG m_lOffPBTestLFStepY;
	BOOL m_bOffPBTestLFDieResult;

	LONG SelectBinRowColFromTempFile(IPC_CServiceMessage &svMsg);		//andrewng //2020-0630
	LONG BTMoveFromTempFile(IPC_CServiceMessage &svMsg);				//andrewng //2020-0630
	LONG MoveTableToDummyPos(IPC_CServiceMessage &svMsg);				//andrewng //2020-0727
	LONG m_lBackupDummyPosX;
	LONG m_lBackupDummyPosY;

	/********************* Grade Information ************************/
	LONG GradeInfoPreRoutine(IPC_CServiceMessage& svMsg);
	LONG DisplayGradeInfo(IPC_CServiceMessage& svMsg);
	LONG SubmitGradeInfo(IPC_CServiceMessage& svMsg);
	BOOL ResetOptimizeBinCountPerWft(IPC_CServiceMessage& svMsg);
	BOOL EnableOptimizeBinCountPerWft(IPC_CServiceMessage& svMsg);
	LONG CopyAllGradeInfo(IPC_CServiceMessage& svMsg);
	LONG CopyPartialGradeInfo(IPC_CServiceMessage& svMsg);
	LONG LoadGradeRankFile(IPC_CServiceMessage& svMsg);
	LONG ResetGradeRankID(IPC_CServiceMessage& svMsg);

	/********************* Bin Count Checking ************************/
	LONG CheckIfAllBinCntCleared(IPC_CServiceMessage& svMsg);
	LONG UpdateIfAllBinCountsAreCleared(IPC_CServiceMessage& svMsg);		//v4.08		//Osram ClearBin menu page

	/********** Lot Number & Lot Directory change checking **********/
	LONG PreLotNoChangeChecking(IPC_CServiceMessage& svMsg);
	LONG PreLotDirectoryChangeChecking(IPC_CServiceMessage& svMsg);
	LONG PreAssociateFileChangeChecking(IPC_CServiceMessage& svMsg);

	/***************** Set & Reset Serial No ****************/
	LONG SetBinSerialNoFormat(IPC_CServiceMessage& svMsg);
	LONG SetBinSerialNoMaxLimit(IPC_CServiceMessage& svMsg);
	LONG ResetBinSerialNo(IPC_CServiceMessage& svMsg);

	/***************** Set & Reset Bin Clear counter ****************/
	LONG SetBinClearFormat(IPC_CServiceMessage& svMsg);
	LONG SetBinClearMaxLimit(IPC_CServiceMessage& svMsg);
	LONG SetBinClearInitCount(IPC_CServiceMessage& svMsg);
	LONG ResetBinClearedCount(IPC_CServiceMessage& svMsg);

	/***************** Set / Reset Store Bin accumulated bonded count ****************/
	LONG EnableBinAccBondedCount(IPC_CServiceMessage& svMsg);
	LONG ResetBinAccBondedCount(IPC_CServiceMessage& svMsg);

	/***************** Select Optimize Bin Grade ***************/
	LONG EnableOptimizeBinGrade(IPC_CServiceMessage& svMsg);

	/***************** Select LS Bond Pattern ***************/
	LONG EnableLSBondPattern(IPC_CServiceMessage& svMsg);

	/********** Wafer End file path **********/
	LONG GetWaferEndFilePath(IPC_CServiceMessage& svMsg);
	LONG GetWaferEndSummaryFilePath(IPC_CServiceMessage& svMsg);
	LONG InputWaferEndFilePath(IPC_CServiceMessage& svMsg);
	LONG InputWaferEndFileFormat(IPC_CServiceMessage& svMsg);
	LONG SaveWaferEndFilePath(IPC_CServiceMessage& svMsg);
	LONG SaveWaferEndFileFormat(IPC_CServiceMessage& svMsg);

	/********** Supplementary for Bin Loader Module **********/
	LONG CheckIfPhyBlkIsSetup(IPC_CServiceMessage& svMsg);
	LONG GetBinBlkBondedCount(IPC_CServiceMessage& svMsg);

	/********** Output /Show LotSummary File **********/
	LONG OutputLotSummary(IPC_CServiceMessage& svMsg);
	LONG OutputLotSummary_Avago(IPC_CServiceMessage& svMsg);		//v4.48A21	
	LONG ShowLotSummary(IPC_CServiceMessage& svMsg);

	BOOL CopyMagazineSummary();		//for Epigap/Jenoptic	//v4.43T8

	LONG BT_GetPhyBinBlockCorner(IPC_CServiceMessage &svMsg);
	LONG BT_GetCurrPosnInZero(IPC_CServiceMessage &svMsg);
	/* Move XY
		Input Parameter: LONG X, LONG Y
		Output Parameter: LONG encX, LONG encY
	*/
	LONG XY_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG XY2_MoveToCmd(IPC_CServiceMessage& svMsg);		//MS109		//v4.35T3
	LONG XY_MoveCmd(IPC_CServiceMessage& svMsg);

	/* Move XY for BinLoader
		Input Parameter: LONG X, LONG Y
		Output Parameter: BOOL bReturn
	*/
	LONG MoveToCOR(const CString szLogMess);
	LONG T_MoveToZeroCmd_ForBL(IPC_CServiceMessage& svMsg);
	LONG XY_MoveToCmd_ForBL(IPC_CServiceMessage& svMsg);
	LONG XY_MoveToCmd_ForBL_XThenY(IPC_CServiceMessage& svMsg);		//v4.11	//Walsin&Lexter
	LONG XY_MoveToCmd_ForBL_YThenX(IPC_CServiceMessage& svMsg);		//v4.11	//Walsin&Lexter
	LONG XY2_MoveToCmd_ForBL(IPC_CServiceMessage& svMsg);
	LONG XY2_MoveToCmd_ForBL_XThenY(IPC_CServiceMessage& svMsg);	//v4.17	//MS100 9Inch
	LONG XY2_MoveToCmd_ForBL_YThenX(IPC_CServiceMessage& svMsg);	//v4.17	//MS100 9Inch
	LONG XY_Sync_ForBL (IPC_CServiceMessage& svMsg);				//v4.37T3	//MS109
	LONG XY2_Sync_ForBL(IPC_CServiceMessage& svMsg);				//v4.37T3	//MS109

	/* Move single axis
		Input Parameter: LONG Pos
		Output Parameter: LONG encX, LONG encY
	*/
	LONG X_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG X_MoveCmd(IPC_CServiceMessage& svMsg);
	LONG Y_MoveToCmd(IPC_CServiceMessage& svMsg);
	LONG Y_MoveCmd(IPC_CServiceMessage& svMsg);

	/* Home Functions
		Input Parameter: Nothing
		Output Parameter: Nothing
	*/
	LONG X_HomeCmd(IPC_CServiceMessage& svMsg);
	LONG Y_HomeCmd(IPC_CServiceMessage& svMsg);
	LONG XY_HomeCmd(IPC_CServiceMessage& svMsg);

	/* On or off the joystick
		Input Parameter: BOOL bOn
		Output Parameter: BOOL bOK
	*/
	LONG SetJoystickCmd(IPC_CServiceMessage& svMsg);
	LONG SwitchTableForPRJoyStick(IPC_CServiceMessage& svMsg);
	/* On or off the joystick
		Input Parameter: BOOL bOn
		Output Parameter: BOOL bOK
	*/
	LONG SetJoystickCmdWithoutSrCheck(IPC_CServiceMessage& svMsg);

	/* On or off the joystick with checking the current state
		Input Parameter: BOOL bOn
		Output Parameter: BOOL bOK
	*/
	LONG UseJoystickCmd(IPC_CServiceMessage& svMsg);

	/* Set joystick speed
		Input Parameter: LONG lLevel
		Output Parameter: BOOL bOK
	*/
	LONG SetJoystickSpeedCmd(IPC_CServiceMessage& svMsg);


	/* Get the encoder values
		Input Parameter: Nothing
		Output Parameter: LONG encX, LONG encY
	*/
	LONG GetEncoderCmd(IPC_CServiceMessage& svMsg);
	LONG GetEncoderCmd2(IPC_CServiceMessage& svMsg);

    // Bin loader load/unload
    LONG SetBinLoadTestState(IPC_CServiceMessage& svMsg);
    LONG GetBinLoadTestState(IPC_CServiceMessage& svMsg);

    // Realign bin block
	LONG ResetFrameAlign(IPC_CServiceMessage& svMsg);				//v4.59A31
    LONG RealignBinBlockCmd(IPC_CServiceMessage& svMsg);
    LONG RealignPhysicalBlockCmd(IPC_CServiceMessage& svMsg);
	LONG SubRealignPhysicalBlockCmd(ULONG ulPhysicalBlock);
//    LONG RealignPhysicalBlock2Cmd(IPC_CServiceMessage& svMsg);
//	BOOL SubRealignPhysicalBlock2Cmd(const ULONG ulPhysicalBlock);
	LONG RealignEmptyFrameCmd(IPC_CServiceMessage& svMsg);
	LONG SubRealignEmptyFrameCmd(const ULONG ulPhysicalBlock);
	LONG ResetFrameIsAligned(IPC_CServiceMessage& svMsg);			//v4.42T10
	LONG HomeTableForRealign_BL(IPC_CServiceMessage& svMsg);		//v4.17T6
	LONG IsBT2UnderCamera(IPC_CServiceMessage& svMsg);				//v4.35T4
	//LONG AlignBTGlobalTheta(IPC_CServiceMessage& svMsg);			//v4.39T7
	BOOL AlignGlobalThetaAngle(ULONG ulBlkID);
	BOOL AlignFrameOrientation(ULONG ulBlkID);		//shiraishi01
	BOOL AlignGlobalThetaAngleByBinFrame(ULONG ulBlkID, BOOL bIsEmpty = FALSE);			// The Reference Cross
	LONG AlignEmptyFrameWithPRCmd(IPC_CServiceMessage& svMsg);
	LONG ExArmRealignEmptyFrameWithPRCmd(IPC_CServiceMessage& svMsg);
	LONG SubExArmRealignEmptyFrameWithPRCmd(const ULONG ulPhysicalBlock);

	// The Reference Cross (Citizen)
	LONG SaveReferenceCrossEncoderValue(IPC_CServiceMessage& svMsg);
	LONG TEST_AlignGlobalThetaAngleByBinFrame(IPC_CServiceMessage& svMsg);

	//BT T	//v4.49A9
	LONG MoveToCORCmd(IPC_CServiceMessage &svMsg);
	LONG TeachCalibration(IPC_CServiceMessage &svMsg);
	LONG StopCalibration(IPC_CServiceMessage &svMsg);
	LONG UpdateCOR(IPC_CServiceMessage &svMsg);
	LONG StartBinCalibration();
	LONG StopBinCalibration(LONG lStop);
	LONG MoveWaferTheta();
	BOOL BT_SetFrameRotation(ULONG ulBlkId, BOOL bMoveToOrgPosn);
	BOOL BT_SetFrameRotation(ULONG ulBlkId, BOOL bEnable180Rotation, BOOL bMoveToOrgPosn);
	BOOL RotateBinToOrg(LONG lOldX, LONG lOldY, LONG &lNewX, LONG &lNewY);
	BOOL RotateBinTo180(LONG lOldX, LONG lOldY, LONG &lNewX, LONG &lNewY);
	BOOL	m_bStartCalibrate;
	BOOL	m_bIsRotate180Deg;
	BOOL	m_bIsRotate180DegHMI;

	// -- Search Bintable Limit --
	// Search Positive limit sensor then negative limit sensor
	LONG ResetXLimitCmd(IPC_CServiceMessage& svMsg);
	LONG SearchXLimitCmd(IPC_CServiceMessage& svMsg);
	LONG SearchYLimitCmd(IPC_CServiceMessage& svMsg);
	LONG SearchX2LimitCmd(IPC_CServiceMessage& svMsg);
	LONG SearchY2LimitCmd(IPC_CServiceMessage& svMsg);
	LONG ConfirmBinTableLimit(IPC_CServiceMessage& svMsg);
	LONG CancelBinTableLimit(IPC_CServiceMessage& svMsg);

	// -- Teach bintable load/unload position
	LONG TeachLoadPosition(IPC_CServiceMessage& svMsg);
	LONG ConfirmLoadPosition(IPC_CServiceMessage& svMsg);
	LONG CancelLoadPosition(IPC_CServiceMessage& svMsg);
	//LONG MoveLoadPosition(IPC_CServiceMessage& svMsg);
	//LONG CompleteLoadPosition(IPC_CServiceMessage& svMsg);
	LONG MoveToUnloadLoad(IPC_CServiceMessage& svMsg);

	// -- Teach Collet Offset --
	LONG TeachColletOffset(IPC_CServiceMessage& svMsg);
	LONG ConfirmColletOffset(IPC_CServiceMessage& svMsg);

	//MS100 9Inch dual-table offset XY
	LONG TeachBinTable2OffsetXY(IPC_CServiceMessage& svMsg);

	// -- Store Bintable data --		
	LONG StoreBinTableData(IPC_CServiceMessage& svMsg);

	// -- Manual gen all temp file 
	//LONG ManualGenAllTempFiles(IPC_CServiceMessage& svMsg);

	LONG SetAlignBinMode(IPC_CServiceMessage& svMsg);

	// -- Manual gen WaferEnd file
	LONG ManualWaferEnd(IPC_CServiceMessage& svMsg);

	// -- Get Label file path & reprint 
	LONG GetLabelFilePath(IPC_CServiceMessage& svMsg);
	LONG SelectLabelToPrint(IPC_CServiceMessage& svMsg);
	LONG BinBarcodeSelectLabelToPrint(IPC_CServiceMessage& svMsg);

	// -- Restore NVRam Runtime data
	LONG RestoreNVRunTimeData(IPC_CServiceMessage& svMsg);
	LONG RestoreNVRunTimeDataInHmi(IPC_CServiceMessage& svMsg);

	// -- Reset NVRam Runtime data
	LONG ClearNVRunTimeData(IPC_CServiceMessage& svMsg);

	// -- Setup binblock pre & post function
	LONG SetupBinBlock(IPC_CServiceMessage& svMsg);

	// -- Display Bonding BinBlk
	LONG DisplayBondingBinBlk(IPC_CServiceMessage& svMsg);

	// Diagnostic Functions
	LONG Diag_PowerOn_X(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_Y(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_T(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_X2(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_Y2(IPC_CServiceMessage &svMsg);
	LONG Diag_PowerOn_All(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_X(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_Y(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_X2(IPC_CServiceMessage &svMsg);
	LONG Diag_Comm_Y2(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_X(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_Y(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_T(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_X2(IPC_CServiceMessage &svMsg);
	LONG Diag_Home_Y2(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_X(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_Y(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_T(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_X2(IPC_CServiceMessage &svMsg);
	LONG Diag_Move_Y2(IPC_CServiceMessage &svMsg);

	LONG GetBinDiePitch(IPC_CServiceMessage& svMsg);		//v2.58
	LONG GetNewMapBlockInUse(IPC_CServiceMessage& svMsg);	//v2.68
	LONG GetNextGradeBlk(IPC_CServiceMessage& svMsg);		//v2.82T1

	// --- Execut Print Label Utility --- //
	LONG ExecutPrintLabelUtility(IPC_CServiceMessage& svMsg);

	LONG SaveBinTableDataCmd(IPC_CServiceMessage& svMsg);

	// Update the bin grade
	LONG UpdateOneBinGrade(IPC_CServiceMessage& svMsg);
	//Update ByPassBinMap option in BinBlk Setup
	LONG UpdateBlkByPassBinMap(IPC_CServiceMessage& svMsg);	//v4.48A11	//Semitek, 3E DL

	// A function to genearte output summary file
	// magazine summary file & reset magazine status
	LONG ClearAllBin(IPC_CServiceMessage& svMsg);

	LONG GetRankIDFilePath(IPC_CServiceMessage &svMsg);
	LONG ResetRankIDFile(IPC_CServiceMessage &svMsg);
	LONG AutoLoadRankIDFile(IPC_CServiceMessage& svMsg);

	// Bin RunTime Data Functions
	LONG UploadBinBlkRunTimeData(IPC_CServiceMessage& svMsg);
	LONG DownloadBinBlkRunTimeData(IPC_CServiceMessage& svMsg);
	LONG GenBinBlkRunTimeSummary(IPC_CServiceMessage& svMsg);

	LONG RemoveAllTempFile(IPC_CServiceMessage& svMsg);
 	LONG SaveGenSummaryFileTime(IPC_CServiceMessage& svMsg);

	LONG GetBinLotSummaryFilePath(IPC_CServiceMessage& svMsg);
	LONG GetBinLotSummaryFilePath2(IPC_CServiceMessage& svMsg);
	LONG LoadBinLotSummaryFile(IPC_CServiceMessage& svMsg);
	LONG LoadBinLotSummaryFile2(IPC_CServiceMessage& svMsg);			//v4.42T16	//3E
	LONG AutoLoadBinLotSummaryFile(IPC_CServiceMessage& svMsg);

	LONG UpdateOptimizeBinGradeScreen(IPC_CServiceMessage& svMsg);
	LONG OnClickChangeOptimizeBinGrade(IPC_CServiceMessage& svMsg);
	LONG OnClickUpdateOptimzeMinCountByRange(IPC_CServiceMessage& svMsg);
	LONG OnClickUpdateAllOptimizeMinCount(IPC_CServiceMessage& svMsg);
	LONG OnClickUpdateOptimizeMaxCountByRange(IPC_CServiceMessage& svMsg);
	LONG OnClickUpdateAllOptimizeMaxCount(IPC_CServiceMessage& svMsg);

	//LONG OnClickMinFrameDieCount(IPC_CServiceMessage& svMsg);
	//LONG OnClickMaxFrameDieCount(IPC_CServiceMessage& svMsg);
	//LONG OnClickUpdateSingleFrameDieCount(IPC_CServiceMessage& svMsg);
	LONG ResetOptimizeBinCountStatus(IPC_CServiceMessage& svMsg);
	//LONG OnClickUpdateAllOptimizeMinCount(IPC_CServiceMessage& svMsg);
	//LONG OnClickUpdateAllOptimizeMaxCount(IPC_CServiceMessage& svMsg);

	LONG WaferEndFileGeneratingCheckingCmd(IPC_CServiceMessage& svMsg);

	BOOL SendRequestReply(CString szStnName, CString szFuncName, IPC_CServiceMessage &stMsg);
	BOOL BT_SearchDieInMaxFOV(int &siStepX, int &siStepY);
	BOOL AutoLearnBHZBondPosOffset(LONG &lBHZBondPosOffsetX, LONG &lBHZBondPosOffsetY);
	BOOL ConfirmBHZBondPosOffsetByCollet(const CString szFuncName, LONG &lBHZBondPosOffsetX, LONG &lBHZBondPosOffsetY);

	BOOL ResetBHZ1BondPosOffset(IPC_CServiceMessage& svMsg);
	BOOL AutoLearnBHZ1BondPosOffset(IPC_CServiceMessage& svMsg);
	BOOL TeachBHZ1BondPosOffsetByCollet(IPC_CServiceMessage& svMsg);
	BOOL ConfirmBHZ1BondPosOffsetByCollet(IPC_CServiceMessage& svMsg);


//	BOOL ManualLearnBHZ2BondPosOffset(IPC_CServiceMessage& svMsg);
	BOOL ResetBHZ2BondPosOffset(IPC_CServiceMessage& svMsg);
	BOOL AutoLearnBHZ2BondPosOffset(IPC_CServiceMessage& svMsg);
	BOOL TeachBHZ2BondPosOffsetByCollet(IPC_CServiceMessage& svMsg);
	BOOL ConfirmBHZ2BondPosOffsetByCollet(IPC_CServiceMessage& svMsg);

	BOOL ConfirmBHZ1OrBHZ2BondPosOffsetByCollet(IPC_CServiceMessage& svMsg);

	LONG MoveBinTableToSafePosn(IPC_CServiceMessage& svMsg);		//v3.70T1
	LONG MoveBinTableToBondPosn(IPC_CServiceMessage& svMsg);		//v3.86	//XU
	LONG MoveBinTableToMS90GBinPosn(IPC_CServiceMessage& svMsg);	//v4.59A7
	LONG MoveToBinArea4Corners(IPC_CServiceMessage& svMsg);			//v3.94T5
	LONG MoveToBinArea4Corners2(IPC_CServiceMessage& svMsg);		//v4.21
	LONG MoveBTForAutoCColletAutoLearnZ(IPC_CServiceMessage& svMsg);	//v4.49A7	//WH SanAn
	LONG AGCMoveBinTableToBondPosn(IPC_CServiceMessage& svMsg);

	LONG IsNeedCheckBatchIDFile(IPC_CServiceMessage& svMsg);
	LONG CheckBatchIDInAutoMode(IPC_CServiceMessage& svMsg);
	LONG ClearBatchIDInformation(IPC_CServiceMessage& svMsg);
	
	//v3.74T45		//v4.03	//PLLM/PLSG Binmap fcn
	LONG CreateBinMapFcn(IPC_CServiceMessage& svMsg);		
	LONG CreateBinMapFcn2(IPC_CServiceMessage& svMsg);				//v4.36	
	LONG LoadBinMapFcn(IPC_CServiceMessage& svMsg);
	LONG LoadNGBinMap(IPC_CServiceMessage& svMsg);	
	LONG SaveBinMapFcn(IPC_CServiceMessage& svMsg);
	VOID UpdateBinMapCapacity(const ULONG ulNewInputCount);
	LONG SaveBinMapFcn_pkg(IPC_CServiceMessage& svMsg);
	LONG LoadBinMapFcn_pkg(IPC_CServiceMessage& svMsg);
	LONG EnableBinMapFcn(IPC_CServiceMessage& svMsg);	
	LONG SetSpGradeToByPassBinMap(IPC_CServiceMessage& svMsg);		//v4.48A4
	LONG ReLoadBinMapFcn(IPC_CServiceMessage& svMsg);
	LONG NGBlockBinBlkSetup(IPC_CServiceMessage& svMsg);

	//v4.47T13
	LONG EnableOsramBinMixMap(IPC_CServiceMessage& svMsg);	
	LONG CreateOsramBinMixMap(IPC_CServiceMessage& svMsg);
	LONG SaveOsramBinMixMap(IPC_CServiceMessage& svMsg);

	// Axis Test commands
	LONG StartBTXMotionTest(IPC_CServiceMessage& svMsg);
	LONG StartBT2XMotionTest(IPC_CServiceMessage& svMsg);
	LONG StartBTYMotionTest(IPC_CServiceMessage& svMsg);
	LONG StartBT2YMotionTest(IPC_CServiceMessage& svMsg);
	//shiraishi02
	LONG SetBinFrameOrientXY(IPC_CServiceMessage& svMsg);

	LONG	m_lBpX;
	LONG	m_lBpY;

	//v4.11T1	//Lumileds
	LONG StartOfflinePostbondTest(IPC_CServiceMessage& svMsg);
	VOID RunOfflinePostBondTest();
	LONG m_lStartBondIndex;	
	LONG m_lCurrBondIndex;
	LONG m_lNoOfPostBondDices;
	LONG m_lCurrBlkID;
	BOOL m_bOfflinePostBondTest;
	BOOL m_bStopOfflinePostBondTest;
	CStdioFile m_oOfflinePostBondFile;


    VOID SendCE_Setup(VOID);
	LONG BTBackupMapCommand(IPC_CServiceMessage& svMsg);
	LONG BTGenAllTempFiles(IPC_CServiceMessage& svMsg);

	// Auto Clean Collet
	BOOL SetBTMoveLowProfile();
	LONG BTMoveToACCLiquid(IPC_CServiceMessage& svMsg);
	LONG BTMoveToACCBrush(IPC_CServiceMessage& svMsg);
	LONG BTCycleACCOperation(IPC_CServiceMessage& svMsg);
	LONG BTCycleACCOperation2(IPC_CServiceMessage& svMsg);	//v4.44A1	//SEmitek
	LONG BTChangePosition(IPC_CServiceMessage& svMsg);
	LONG BTCancelSetup(IPC_CServiceMessage& svMsg);
	LONG BTConfirmSetup(IPC_CServiceMessage& svMsg);
	LONG BTMoveToACCClean(IPC_CServiceMessage& svMsg);
	LONG BTAccConfirmSetup(IPC_CServiceMessage& svMsg);
	LONG BTAccReset(IPC_CServiceMessage& svMsg);

	LONG BTMoveToRemoveDirt(IPC_CServiceMessage& svMsg);
	LONG BTCycleRemoveDirtOperation(IPC_CServiceMessage& svMsg);

	BOOL CheckTableOutLimitX(LONG lX);
	BOOL CheckTableOutLimitY(LONG lY);
	BOOL ACCGetTableMovePosition(LONG &lX, LONG &lY, LONG lOption=0);
	//AutoChangeCollet Fcns //v4.50A5	//MS60
	LONG BTChangeAGCPosition(IPC_CServiceMessage& svMsg);
	LONG BTConfirmAGCSetup(IPC_CServiceMessage& svMsg);
	LONG BTCancelAGCSetup(IPC_CServiceMessage& svMsg);
	VOID GetBHXYOffset(const CString szTitle, const BOOL bBHZ2, const BOOL bCalcEJColletOffset, LONG &lX, LONG &lY);
	BOOL IsBHBTReady(CString &szErrorMess);
	BOOL IsAutoChangeColletReady();
	LONG BTMoveToAGCClampPos(IPC_CServiceMessage& svMsg);
	LONG BTMoveToAGCUPLUploadPos(IPC_CServiceMessage& svMsg);
	LONG BTMoveToAGCHolderUploadPos(IPC_CServiceMessage& svMsg);
	LONG BTMoveToAGCHolderInstallPos(IPC_CServiceMessage& svMsg);
	LONG BTMoveToAGCPusher3Pos(IPC_CServiceMessage& svMsg);			//Pusher pos to install collet
	LONG BTMoveToAGCUpLookPos(IPC_CServiceMessage& svMsg);			//v4.50A11
	LONG AGC_IncrementIndex(IPC_CServiceMessage& svMsg);			//v4.53A12

	LONG CheckBinEmptyStatus(IPC_CServiceMessage &svMsg);			//v4.14T1
	LONG FATC_CheckBinEmptyStatus(IPC_CServiceMessage &svMsg);
	LONG ManualCleanCollet_PLLM_MS109(IPC_CServiceMessage& svMsg);	//v4.36T1

	LONG GetInputCountSetupFilePath(IPC_CServiceMessage& svMsg);
	LONG LoadInputCountSetupFile(IPC_CServiceMessage& svMsg);

	LONG OsramCalculateFunction(IPC_CServiceMessage& svMsg);//osram bin mix//4.49 benableosrambinmixmap
	LONG SemitekGenerateOutputFileWhenReadWaferBarcode(IPC_CServiceMessage& svMsg);

	LONG SetFrameAlignOffset(IPC_CServiceMessage& svMsg);			//v4.59A21	//DavidMa
	LONG SetFrameAlignAngle(IPC_CServiceMessage& svMsg);			//v4.57A9	//DavidMa
	LONG SetThermalDeltaPitch(IPC_CServiceMessage& svMsg);			//v4.59A21	//DavidMa
	LONG RotateFrameAngle(IPC_CServiceMessage& svMsg);			//v4.59A1
	LONG BT_BondLeftDie(IPC_CServiceMessage& svMsg);
	LONG BT_GetGradeBinBlock(IPC_CServiceMessage& svMsg);
	LONG BT_SetCountByGrade(IPC_CServiceMessage& svMsg);

	LONG RotateBinAngularDie(IPC_CServiceMessage &svMsg);
	LONG FindCenterOfRotation(IPC_CServiceMessage &svMsg);

	//***ErrMap***//	//andrewng //2020-0805
	LONG ErrMapCalThetaCmd(IPC_CServiceMessage& svMsg);
	LONG ErrMapSamplingCmd(IPC_CServiceMessage& svMsg);
	LONG ErrMapSetRefCmd(IPC_CServiceMessage& svMsg);
	LONG ErrMapSearchRecord(IPC_CServiceMessage& svMsg);
	LONG ErrMapVerificationCmd(IPC_CServiceMessage& svMsg);
	LONG ErrMapGoToRefCmd(IPC_CServiceMessage& svMsg);
	LONG ErrMapLoadRecordCmd(IPC_CServiceMessage& svMsg);
	LONG ErrMapClearDataCmd(IPC_CServiceMessage& svMsg);

	VOID SaveErrMapEvent(CString szMsg);
	BOOL IsMatrixMarksInUse();
	LONG XUM_ToEncoder(DOUBLE dUM_X);	//	input um, return encoder of distance
	LONG YUM_ToEncoder(DOUBLE dUM_Y);	//	input um, return encoder of distance
	BOOL BT_BM_CalculateOffset(LONG lBT_X, LONG lBT_Y, LONG &lOffsetX, LONG &lOffsetY, BOOL bRotate);
	BOOL BM_GetNewXY(LONG &lBT_X, LONG &lBT_Y, BOOL bRatate = FALSE, BOOL bVerifyOnly = FALSE);		//v4.71A21
	BOOL ParseRawData(CString szRawData, CStringArray& szaRawDataByCol);

	LONG ErrMapCalTheta(BOOL bUseCurrEncoderPos = FALSE);
	LONG ErrMapSampling(BOOL bRotate);
	LONG ErrMapGenFile(ULONG ulRow, ULONG ulCol, LONG lX, LONG lY, LONG lOffsetX, LONG lOffsetY, BOOL bOK, LONG lPixelX, LONG lPixelY);
	LONG ErrMapSetLimitToFile(LONG X1,LONG Y1,LONG X2,LONG Y2);
	LONG ErrMapLoadRecord();
	DOUBLE ErrMapGetSlope(LONG lTotalIndex, CArray<double, double> &aX, CArray<double, double> &aY);
	BOOL BT_SearchErrMap(LONG &lOffsetX, LONG &lOffsetY, LONG &lPixelX, LONG &lPixelY);

	//andrewng //2020-0902
	LONG MoveToUnload(IPC_CServiceMessage& svMsg);
	LONG SetUnloadPosition(IPC_CServiceMessage& svMsg);



	BOOL	m_bIsAllBinsEmptyStatus;
	//Nichia//v4.43T7
	BOOL	m_bEnableNichiaOutputFile;
	BOOL	m_bEnableNichiaOTriggerFile;
	BOOL	m_bEnableNichiaRPTFile;

	LONG	m_lACCLiquidX;
	LONG	m_lACCLiquidY;
	LONG	m_lACCBrushX;
	LONG	m_lACCBrushY;

	LONG	m_lACCRangeX;
	LONG	m_lACCRangeY;
	LONG	m_lACCCycleCount;

	LONG	m_lACCMatrixRow;
	LONG	m_lACCMatrixCol;
	LONG	m_lACCAreaSizeX;
	LONG	m_lACCAreaSizeY;
	LONG	m_lACCAreaLimit;
	LONG	m_lACCAreaCount;
	LONG	m_lACCAreaIndex;
	LONG	m_lACCReplaceCount;
	LONG	m_lACCReplaceLimit;
	BOOL	m_bACCToggleBHVacuum;


	// Clean Dirt Row & Column
	LONG	m_lCleanDirtMatrixRow;
	LONG	m_lCleanDirtMatrixCol;
	LONG	m_lCleanDirtUpleftPosX;
	LONG	m_lCleanDirtUpleftPosY;
	LONG	m_lCleanDirtLowerRightPosX;
	LONG	m_lCleanDirtLowerRightPosY;
	LONG	m_lCleanDirtRowIndex;
	LONG	m_lCleanDirtColIndex;

	//v4.59A7	//MS90 only
	LONG	m_lMS90MCCGarbageBinX;
	LONG	m_lMS90MCCGarbageBinY;
	//shiraishi02
	BOOL	m_bCheckFrameOrientation;
	LONG	m_lFrameOrientCheckX;
	LONG	m_lFrameOrientCheckY;

	//v4.50A5	//MS60 AutoChgCollet Fcn
	LONG	m_lChgColletClampPosX;
	LONG	m_lChgColletClampPosY;
	LONG	m_lChgColletHolderInstallPosX;
	LONG	m_lChgColletHolderInstallPosY;
	LONG	m_lChgColletHolderUploadPosX;
	LONG	m_lChgColletHolderUploadPosY;
	LONG	m_lChgColletPusher3PosX;
	LONG	m_lChgColletPusher3PosY;
	LONG	m_lChgColletUpLookPosX;
	LONG	m_lChgColletUpLookPosY;
	LONG	m_lChgColletUPLUploadPosX;
	LONG	m_lChgColletUPLUploadPosY;

	BOOL	m_bEnableBTMotionLog;

public:
	BOOL IsIdle();
	void AddDie(unsigned long ulRow, unsigned long ulCol, char cGrade);

public:
	BOOL	m_bWaferEndUploadMapMpd;
	BOOL	m_bEnableBinOutputFile;
	BOOL	m_bFinForBinOutputFile;
	BOOL	m_bEnableBinMapFile;
	BOOL	m_bFin2ForBinMapFile;
	INT		m_nNoOfSubBlk;
	INT		m_nNoOfSubGrade;
	INT     m_nSubGrade[5][5];
	INT		m_nSubBlk[5];

	LONG	m_lBTMoveNextDieDelay;
protected:

	//Load Position
	LONG	m_lMotorTestPt1;
	LONG	m_lMotorTestPt2;
	ULONG	m_ulMotorTestRepeatDelay;
	ULONG	m_ulMotorTestRepeatCycles;
	ULONG	m_ulTestRunDirection;
	ULONG	m_ulTestRunRepeatDelay;
	ULONG	m_ulTestRunRepeatCycles;

	ULONG	m_ulJoystickFlag; //indicates where joystick is now refering to

	//Wafer Statistics
	ULONG	m_ulTotalSortedFromAllGrades;

	/******************************/
	/*     HMI Reg Variables      */
	/******************************/
	BOOL m_bReply;

	//Enability of Hmi Controls
	BOOL m_bIfEnableTBEditBlkSettings;
	BOOL m_bIfEnableTBClrBlkSettings;

	BOOL m_bIfEnableCBClrBlkSettingsSelection;
	BOOL m_bIfEnablePIBlkToClrSettings;
	BOOL m_bIfEnableTBSubmitClrBlkSettings;

	BOOL m_bIfEnableCBSetupMode;
	BOOL m_bIfEnablePINoOfBlk;
	BOOL m_bIfEnablePIBlkToSetup;
	BOOL m_bIfEnableTBSetUL;
	BOOL m_bIfEnableTBSetLR;
	BOOL m_bIfEnableTBConfirmUL;
	BOOL m_bIfEnableTBConfirmLR;
	BOOL m_bIfEnableDDUpperLeftX;
	BOOL m_bIfEnableDDUpperLeftY;
	BOOL m_bIfEnableDDLowerRightX;
	BOOL m_bIfEnableDDLowerRightY;
	BOOL m_bIfEnablePIBlkPitchX;
	BOOL m_bIfEnablePIBlkPitchY;
	BOOL m_bIfEnablePIGrade;
	BOOL m_bIfEnablePIDiePitchX;
	BOOL m_bIfEnablePIDiePitchY;
	BOOL m_bIfEnableCBWalkPath;
	BOOL m_bIfEnableVDNoOfDiePerBlk;
	BOOL m_bIfEnableVDNoOfDiePerRow;
	BOOL m_bIfEnablePIDiePerRow;
	BOOL m_bIfEnablePIDiePerCol;
	BOOL m_bIfEnablePIDiePerBlk;
	BOOL m_bIfEnableChkBAutoAssignGrade;
	
	//Enable2DBarcodeOutput option can be modified only when all bins are cleared	//v3.33T3
	BOOL m_bIfEnable2DBarcodeOutput;
	BOOL m_bIfEnableTemplateSetup;

	BOOL m_bIfEnableFirstRowColSkipPattern;
	BOOL m_bIfEnableCentralizedBondArea;
	
	BOOL m_bIfEnableWafflePad;
	BOOL m_bIfEnableWafflePadCheckBox;
	BOOL m_bIfEnablePadPitchSetX;
	BOOL m_bIfEnablePadPitchSetY;
	BOOL m_bIfEnablePadPitchResetX;
	BOOL m_bIfEnablePadPitchResetY;
	BOOL m_bIfEnablePadDimX;
	BOOL m_bIfEnablePadDimY;
	BOOL m_bIfEnableWafflePadPageBack;

	BOOL m_bIfEnableTeachWithPhysicalBlk;

	BOOL m_bIfUseBlockCornerAsFirstDiePos;
	
	BOOL m_bIfEnableTBSubmit;
	BOOL m_bIfEnableTBCancel;

	//BinTable Limit
	LONG m_lTableXNegLimit;
	LONG m_lTableXPosLimit;
	LONG m_lTableYNegLimit;
	LONG m_lTableYPosLimit;

	LONG m_lTableX2NegLimit;
	LONG m_lTableX2PosLimit;
	LONG m_lTableY2NegLimit;
	LONG m_lTableY2PosLimit;
	LONG m_lTableUnloadX2Pos;
	LONG m_lTableUnloadY2Pos;

	//Machine No
	//CString m_szMachineNo;

	//Clear Block Settings
	CString m_szClrBlkSettingsSelection;

	//Clear Physical Block Settings
	ULONG m_ulPhyBlkToClrSettings;

	//Clear Bin Block Settings
	ULONG m_ulBinBlkToClrSettings;

	//Block Setup
	CString m_szSetupMode;
	BOOL m_bFirstTimeSetUL;
	BOOL m_bFirstTimeSetLR;

	//Physical Block Setup
	ULONG m_ulNoOfPhyBlk;	
	ULONG m_ulPhyBlkToSetup;
	LONG m_lPhyBlkPitchX;
	LONG m_lPhyBlkPitchY;
	LONG m_lPhyUpperLeftX;
	LONG m_lPhyUpperLeftY;
	LONG m_lPhyLowerRightX;
	LONG m_lPhyLowerRightY;

	//Bin Block Setup
	CString m_szBinBlkTemplate;
	CString m_szBinBlkTemplateSel;
	ULONG m_ulTemplateSetupInputCount;
	ULONG m_ulNoOfBinBlk;
	ULONG m_lBinBlkPitchX;
	ULONG m_lBinBlkPitchY;
	LONG m_lBinUpperLeftX;
	LONG m_lBinUpperLeftY;
	LONG m_lBinLowerRightX;
	LONG m_lBinLowerRightY;
	UCHAR m_ucGrade;
	DOUBLE m_dDiePitchX;				//v4.59A19	//MS90 with 0.1um encoder
	DOUBLE m_dDiePitchY;				//v4.59A19	//MS90 with 0.1um encoder
	BOOL m_bUsePt5UmInDiePitchX;		//v4.42T8	//Citizen
	BOOL m_bUsePt5UmInDiePitchY;		//v4.42T8	//Citizen
	DOUBLE m_dBondAreaOffsetXInUm;		//v4.42T8	//Citizen
	DOUBLE m_dBondAreaOffsetYInUm;		//v4.42T8	//Citizen
	DOUBLE m_dThermalDeltaPitchXInUm;		//v4.59A22	//David Ma
	DOUBLE m_dThermalDeltaPitchYInUm;		//v4.59A22	//David Ma
	CString m_szWalkPath;
	ULONG m_ulWalkPath;
	ULONG m_ulDiePerBlk;
	ULONG m_ulDiePerRow;
	ULONG m_ulDiePerCol;
	ULONG m_ulDiePerUserRow;
	ULONG m_ulDiePerUserCol;
	ULONG m_ulSkipUnit;
	ULONG m_ulMaxUnit;
	BOOL  m_bEnableFirstRowColSkipPattern;
	ULONG m_ulFirstRowColSkipUnit;
	ULONG m_ulBinBlkToSetup;
	//Circular BOND area	//PLLM REBEL		//v3.71T1
	BOOL m_bEnableCircularBondArea;
	LONG m_lCirAreaCenterX;
	LONG m_lCirAreaCenterY;
	DOUBLE m_dCirAreaRadius;
	//ByPass BinMap option	//Semitek, 3E DL	//v4.48A11
	BOOL m_bByPassBinMap;
	
	BOOL m_bPkgLoadBinMap;

	BOOL m_bEnableBinMapBondArea;		//New PLLM/PLSG bin map fcn			//v4.03
	BOOL m_bEnableBinMapCDieOffset;		//v4.42T1	//Nichia
	DOUBLE m_dBinMapCircleRadiusInMm;	//v4.36
	LONG m_lBinMapEdgeSize;				//v4.36T9
	LONG m_lBinMapBEdgeSize;			//v4.36T10
	LONG m_lBinMapLEdgeSize;			//v4.36T10
	LONG m_lBinMapREdgeSize;			//v4.36T10
	LONG m_lBinMapXOffset;				//v4.36
	LONG m_lBinMapYOffset;				//v4.36
	LONG m_lBinMapROffset;				//v4.36
	LONG m_lBinMapCDieRowOffset;		//v4.42T1	//Nichia
	LONG m_lBinMapCDieColOffset;		//v4.42T1	//Nichia
	CString	m_szBinMapFilePath;			//v4.37T11
	UCHAR m_ucSpGrade1ToByPassBinMap;	//v4.48A2	//3E DL
		//v4.47T13	//Osram BinMix fcn
	BOOL	m_bEnableOsramBinMixMap;
	UCHAR   m_ucBinMixCurrentGrade;
	LONG	m_ulBinMixPatternType;
	UCHAR	m_ucBinMixTypeAGrade;
	UCHAR	m_ucBinMixTypeBGrade;
	BOOL    m_bBinMixOrder;
	LONG	m_lBinMixTypeAQty;
	LONG	 m_lBinMixTypeBQty;
	LONG	 m_lBinMixTypeAStopPoint;
	LONG	 m_lBinMixTypeBStopPoint;
	LONG	 m_lBinMixPatternQty;
	LONG    m_lBinMixLessCommonType;
	LONG    m_lBinMixCountTemp;
	LONG    m_lBinMixSetQty;				//v4.48     //For OsramBinMixCount // m_bEnableOsramBinMixMap
	
	BOOL m_bEnableAutoLoadRankFile;
	BOOL m_bLoadRankIDFromMap;
	BOOL m_bLoadRankIDFile;				//TOngFang RankID detection			//v4.21T4

	CString m_szRankIDFilename;
	CString m_szRankIDFilePath;

	BOOL m_bAutoBinBlkSetupWithTempl;
	BOOL m_bShowBinBlkTemplateSetup;
	CBinTotalWrapper m_BinBlkTemplateWrapper;

	// For Centralized Bond Area feature
	BOOL  m_bTeachWithPhysicalBlk;
	BOOL  m_bCentralizedBondArea;	
	LONG  m_lCentralizedOffsetX;
	LONG  m_lCentralizedOffsetY;
	LONG  m_lCentralizedUpperLeftX;
	LONG  m_lCentralizedUpperLeftY;
	DOUBLE  m_dSpeedModeOffsetX;		//v3.62		//in mil
	DOUBLE  m_dSpeedModeOffsetY;		//v3.62		//in mil

	//Use Block Corner as First Die position (for Cree Waffle Pad)
	BOOL m_bUseBlockCornerAsFirstDiePos;
	BOOL m_bNoReturnTravel;				//v4.42T1	//Nichia

	CString m_szPKGFilenameDisplay;
	ULONG m_ulPKGDiePitchXDisplay;
	ULONG m_ulPKGDiePitchYDisplay;
	ULONG m_ulPKGInputCountDisplay;
	ULONG m_ulPKGDiePerRowDisplay;
	ULONG m_ulPKGDiePerColDisplay;
	ULONG m_ulPKGBlkCapacityDisplay;
	ULONG m_ulPKGNoOfBinBlkDisplay;

	// For Assign Grade For All Bin Blk 
	BOOL m_bAutoAssignGrade;
	BOOL m_bAssignSameGradeForAllBin;

	UCHAR m_ucStartingGrade;
	ULONG m_ulSourcePhyBlk;
	ULONG m_ulBinSerialNoFormat;
	ULONG m_ulBinSerialNoMaxLimit;
	ULONG m_ulBinClearFormat;
	ULONG m_ulBinClearMaxLimit;
	ULONG m_ulBinClearInitCount;
	BOOL m_bSaveBinBondedCount;
	BOOL m_bOptimizeBinGrade;
	BOOL m_bLSBondPattern;

	LONG m_lMagSummaryYear;
	LONG m_lMagSummaryMonth;
	LONG m_lMagSummaryDay;

	//Clear Bin Counter (by Physical Block)
	CString m_szClrBinCntMode;
	BOOL m_bIfIsClrAllBinCtr;
	ULONG m_ulPhyBlkToClrCount;
	
	BOOL m_bIfGenOutputFile;
	BOOL m_bGenOFileIfClrByGrade;
	BOOL m_bUseUniqueSerialNum;			//v3.23T1

	BOOL m_bUseMultipleSerialCounter;

	BOOL m_bIfGenLabel;
	BOOL m_bIfPrintLabel;
	BOOL m_bUseBarcodeAsFileName;
	CString m_szFileNo;
	CString m_szBlueTapeNo;
	CString m_szOutputFilename;
	CString m_szSpecVersion;
	//CString m_szLabelOrientation;
	CString m_szLabelFilePath;

	//Clear Bin Counter (by Grade)
	CString m_szClrBinByGradeMode;
	BOOL m_bIsClrAllGrades;
	UCHAR m_ucGradeToClrCount;

	//Step Move
	ULONG m_ulTableToStepMove;
	ULONG m_ulBinBlkToStepMove;
	ULONG m_ulCurrBondIndex;
	ULONG m_ulNoOfBondedDie;
	LONG m_lTempIndex;
	ULONG m_ulHowManySteps;
	CString m_szStepMoveMsg;

	//Grade Information
	ULONG m_ulNoOfSortedDie;
	ULONG m_ulBinBlkInUse;
	ULONG m_ulSortedDieIndex;
	ULONG m_ulGradeCapacity;
	ULONG m_ulInputCount;

	ULONG m_ulFullDieCountInput;
	ULONG m_ulMinDieCountInput;
	BOOL m_bEnableDieCountInput;
	UCHAR m_ucAliasGrade;
	BOOL m_bEnableAliasGrade;

	//SDS map information
	BOOL m_bSPCStatus;
	DOUBLE m_dSPCAverageAngle;
	LONG m_lSPCTotalDieCount;
	LONG m_lSPCGoodDieCount;

	//Collet Offset Information;
	LONG m_lColletOffsetX;
	LONG m_lColletOffsetY;

	//Use Filename as Wafer ID
	BOOL m_bFilenameAsWaferID;

	//***ErrMap***//	//andrewng //2020-0805
	BT_CMatrixOffsetInfo		m_stBM_Data1;
	BT_CMatrixOffsetInfo		m_stErrMap1;
	BT_CMatrixOffsetInfo		m_stErrMap2;
	BT_CErrMapBTMarkCompInfo	m_stErrMapBTMarkComp;
	ULONG m_ulErrMapNoOfRow;
	ULONG m_ulErrMapNoOfCol;
	DOUBLE m_dErrMapPitchX;
	DOUBLE m_dErrMapPitchY;
	ULONG m_ulErrMapDelay;
	ULONG m_ulErrMapCount;
	DOUBLE m_dErrMapSamplingTolX;		//in um	
	DOUBLE m_dErrMapSamplingTolY;		//in um	
	ULONG m_ulErrMapSamplingTolLimit;
	LONG m_lErrMapRefX1;
	LONG m_lErrMapRefY1;
	LONG m_lErrMapRefX1InUm;			//andrewng //2020-0820
	LONG m_lErrMapRefY1InUm;
	LONG m_lErrMapLimitX1;
	LONG m_lErrMapLimitY1;
	LONG m_lErrMapLimitX2;
	LONG m_lErrMapLimitY2;
	LONG m_lErrMapOffsetLimitX;
	LONG m_lErrMapOffsetLimitY;
	DOUBLE m_dErrMapSlope;
	
	ULONG	m_ulMarksPrAccLimit;
	ULONG	m_ulMarksPrConLimit;

	/******************************/
	/*  Class Object declaration  */
	/******************************/
	CPhyBlkMain m_oPhyBlkMain;
	CBinBlkMain m_oBinBlkMain;

	// Extra Information for clear bin
	// It is in the bond page --> clear bin ---> extra info
	BOOL	m_bEnableExtraBinInfo[BT_EXTRA_BIN_INFO_ITEM_NO];
	CString m_szaExtraBinInfo[BT_EXTRA_BIN_INFO_ITEM_NO]; // store extra information for clear bin
	CString m_szaExtraBinInfoField[BT_EXTRA_BIN_INFO_ITEM_NO]; // store extra information for clear bin field name

	LONG m_lGenSummaryPeriodNum;
	CString m_szLastGenSummaryTime;
	CString m_szBinSummaryGenTime[BT_SUMMARY_TIME_NO];

	CString m_szInputCountSetupFilePath;
	CString m_szInputCountSetupFilename;

	CString m_szXMLFile;

	CString m_szBinLotSummaryPath;
	CString GetBinLostSummaryPath();
	CString m_szBinLotSummaryPath2;
	CString m_szBinLotSummaryFilename;
	CBinSummaryFile m_BinSummaryFile;

	UCHAR	m_ucOptimizeBinCountMode;
	BOOL	m_bEnableBinSumWaferLotCheck;
	ULONG	m_ulEnableOptimizeBinCount;
	UCHAR	m_ucOpimizeBinSetupGrade;
	UCHAR	m_ucOpimizeBinSetupGradeLimit;
	ULONG	m_ulMinFrameDieCount;
	ULONG	m_ulMaxFrameDieCount;
	ULONG	m_ulMinLotCount;
	BOOL	m_bNotGenerateWaferEndFile;
	BOOL	m_bMachineFirstStart;

	DOUBLE	m_dWaferEndSortingYield;

	//DBH Only		//v3.66
	LONG	m_lBHZ2BondPosOffsetX;
	LONG	m_lBHZ2BondPosOffsetY;

	LONG	m_lBHZ1BondPosOffsetX;
	LONG	m_lBHZ1BondPosOffsetY;

	//v4.55A9	//All in motor steps
	LONG	m_lBHZ1PrePickToBondOffsetX;
	LONG	m_lBHZ1PrePickToBondOffsetY;
	LONG	m_lBHZ2PrePickToBondOffsetX;
	LONG	m_lBHZ2PrePickToBondOffsetY;
	LONG	m_lBHZ1CurrPrePickOffsetX;
	LONG	m_lBHZ2CurrPrePickOffsetX;

	double	m_dBond180DegOffsetX_um;
	double	m_dBond180DegOffsetY_um;
	//v4.37	//Move to MS896aStn base class
	//MS100 9Inch BInTable 2 OFFSET XY (from BT1 taught positions)		//v4.17T3
	//LONG	m_lBT2OffsetX;
	//LONG	m_lBT2OffsetY;

	LONG	m_lBinBarcodeMinDigit;

	BOOL	m_bFirstCycleBT;		//v3.66
	LONG	m_lLoopCount;

	BOOL	m_bTestRealign;
	LONG	m_lFrameXOffsetForTesting;		//v4.59A21
	LONG	m_lFrameYOffsetForTesting;		//v4.59A21
	DOUBLE	m_dFrameAlignAngleForTesting;	//v4.57A9	//David Ma
	
	BOOL	m_bEnableWafflePad;
	LONG	m_lWafflePadDistX;
	LONG	m_lWafflePadDistY;
	LONG	m_lWafflePadSizeX;
	LONG	m_lWafflePadSizeY;

	LONG UpdateBinStatus(IPC_CServiceMessage &svMsg);
	LONG QueryBinStatus(IPC_CServiceMessage &svMsg);
	LONG ResetBinStatus(IPC_CServiceMessage &svMsg);
	BOOL m_bBinStatus[251];		//andrew

	ULONG m_ulSubBinSRowA;
	ULONG m_ulSubBinSRowB;
	ULONG m_ulSubBinSRowC;
	ULONG m_ulSubBinSRowD;

	ULONG m_ulSubBinERowA;
	ULONG m_ulSubBinERowB;
	ULONG m_ulSubBinERowC;
	ULONG m_ulSubBinERowD;

	ULONG m_ulSubBinSColA;
	ULONG m_ulSubBinSColB;
	ULONG m_ulSubBinSColC;
	ULONG m_ulSubBinSColD;

	ULONG m_ulSubBinEColA;
	ULONG m_ulSubBinEColB;
	ULONG m_ulSubBinEColC;
	ULONG m_ulSubBinEColD;

private:
	LONG	m_lCurXPosn;
	LONG	m_lCurYPosn;
	LONG	m_lCurX2Posn;
	LONG	m_lCurY2Posn; 
	BOOL	m_bRealignBinFrameReq;
	ULONG	m_ulRealignBinBlkID;
	BOOL	m_bRealignEmptyFrame;
	BOOL	m_bRealignUseBT2;

	LONG	m_qNextSubOperation;
}; //end class CBinTable


#endif