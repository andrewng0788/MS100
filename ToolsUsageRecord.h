#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "MS896A_Constant.h"

#define ToolsUsageRecordFileBackupPath "C:\\MapSorter\\UserData\\Tools Usage Record Backup"	 

class CColletRecord: public CObject
{
	public:
		CColletRecord();
		~CColletRecord();

		BOOL	m_bShowColletDiagPage;
		CString m_szShiftNo;
		CString m_szOperatorId;
		CString m_szReason;
		CString m_szType;
		CString m_szRemark;
		CString m_szColletMode;
		CString m_szCollet1Mode;
		CString m_szCollet2Mode;
		CString m_szColletModeOld;
		CString m_szTypeOld;
		CString m_szRemarkOld;
		CString m_szPickLevel;
		CString m_szBondLevel;
		CString m_szOperatorLogInId;
		CString m_szMachineNo;
		CString	m_szColletBondedDieCount;
		CString m_szBondArmNo;
		CString m_szProductType;
};

class CEjectorRecord: public CObject
{
	public:
		CEjectorRecord();
		~CEjectorRecord();

		BOOL	m_bShowEjectorDiagPage;
		CString m_szShiftNo;
		CString m_szOperatorId;
		CString m_szReason;
		CString m_szType;
		CString m_szRemark;
		CString m_szEjtTypeOld;
		CString m_szEjtRemarkOld;
		CString m_szEjectorUpLevel;
		CString m_szOperatorLogInId;
		CString m_szMachineNo;
		CString	m_szEjectorPinCount;
		CString m_szProductType;
		CString m_szUsageType;
};

class CDataBlock: public CObject
{
	public:
		CDataBlock();
		~CDataBlock();
		CString ConvertBoolToOnOff(BOOL bEnable);
};

class CMS896ADataBlock: public CDataBlock
{
	public:
		CMS896ADataBlock();
		~CMS896ADataBlock();
	
		CString m_szMachineNo;
		CString m_szLoginID;
		CString m_szVersion;
		
};

class CBinTableDataBlock: public CDataBlock
{
	public:
		CBinTableDataBlock();
		~CBinTableDataBlock();

		CString GetRealignMethodName(LONG lRealignMethod);

		CString m_szReAlignMethod;
		CString m_szBinBlockDieQuantities;
		CString m_szBinBlockWalkPath;
		CString m_szNoOfBinBlocks;
		CString m_szMaxDieQuantities;
		CString m_szFrameReAlignLastDieOffsetX;
		CString m_szFrmaeRealignLastDieOffsetY;

		CString m_szChangeGradeLimit;
		CString		m_szBHZ2BondPosOffsetX;
		CString		m_szBHZ2BondPosOffsetY;
};

class CBinLoaderDataBlock: public CDataBlock
{
	public: 
		CBinLoaderDataBlock();
		~CBinLoaderDataBlock();
		
		CString m_szEnableBinTableVacuum;
		CString m_szBinVacuumSettleDelay;
		CString m_szBinVacuumDelay;
		CString m_szFrameReAlignDelay;
		CString m_szFrameReAlignRetryCount;
		CString m_szEnableFrameRealign;
		
		CString m_szCheckElevatorCover;
};

class CBondHeadDataBlock: public CDataBlock
{
	public:
		CBondHeadDataBlock();
		~CBondHeadDataBlock();

		CString m_szEnableMissingDie;
		CString m_szMissingDieRetryCount;
		CString m_szColletCleanLimit;
		CString m_szPickPosition;
		CString m_szBondPosition;
		CString m_szPickLevelZ1;
		CString m_szBondLevelZ1;
		CString m_szPickLevelZ2;
		CString m_szBondLevelZ2;
		
		CString m_szEjStandbyLevel;

		CString m_szEnableSyncPick;
		CString m_szSyncMoveTriggerLevel;
		CString m_szSyncMovePreload;
		CString m_szEjectorUpLevel;
		CString m_szEnableColletJam;
		CString m_szColletJamRetryCount;
		CString m_szCycleTime;
		CString m_szAvgCycleTime;
		
		CString m_szCleanColletPosition;
		CString m_szCleanColletLevel;
		CString m_szStandbyLevel_Ej;
		CString m_szEnableAutoCleanCollet;
		CString m_szEnabelCheckCoverSensor;
		
		CString		m_szPickDelay;
		CString		m_szBondDelay;
		CString		m_szArmPickDelay;
		CString		m_szArmBondDelay;
		CString		m_szHeadPickDelay;
		CString		m_szHeadBondDelay;
		CString		m_szHeadPrePickDelay;
		CString		m_szSyncPickDelay;
		CString		m_szEjectorUpDelay;
		CString		m_szEjectorDownDelay;
		CString		m_szEjReadyDelay;
		CString		m_szEjVacOffDelay;
		CString		m_szPRDelay;
		CString		m_szWTTDelay;
		CString		m_szBinTableDelay;
};


class CWaferLoaderDataBlock: public CDataBlock
{	

};

class CWaferTableDataBlock: public CDataBlock
{
	public:
		CWaferTableDataBlock();
		~CWaferTableDataBlock();

		CString m_szEnableDiePitchCheck;
		CString m_szEnableMapDieTypeCheck;
		CString m_szMapDieTypeCheckDieType;
		
		CString m_szEnablePreScan;
		CString m_szEnableAdaptPredict;
		CString m_szPreScanPrDelay;
		CString m_szWaferMapPath;
		CString m_szWaferMapTour;
		CString m_szWaferMapMode;

		CString m_szSortingMode;
		CString m_szAlignOption;
		CString m_szHomeDieRow;
		CString m_szHomeDieCol;

		CString m_szEnableDEBAdaptive;
		CString m_szEnableAdvOffset;
		CString m_szEnableRescanWafer;
		CString m_szStage1Start;
		CString m_szStage1Period;
		CString m_szStage2Start;
		CString m_szStage2Period;
		CString m_szStage3Start;
		CString m_szStage3Period;
		CString m_szRegionGridRow;
		CString m_szRegionGridCol;

		CString m_szAutoSrchHome;
		CString m_szWaferID;
		CString m_szPitchX_Tolerance;
		CString m_szPitchY_Tolerance;
		CString m_szDiePitchCheck;
		CString m_szScnCheckByPR;
		CString m_szEnableScnCheck;
		CString m_szScnCheckIsRefDie;

		//CString m_szPrescanDiePassScore;
};

class CWaferPrDataBlock: public CDataBlock
{
	public:
		CWaferPrDataBlock();
		~CWaferPrDataBlock();

		CString m_szPrLrnCoaxLightLevel;
		CString m_szPrLrnRingLightLevel;
		CString m_szPrLrnSideLightLevel;
		CString m_szPrDiePassScore;
		CString m_szRefDiePRPassScore;
		CString m_szEnableCheckChip;
		CString m_szDefectCheckChipPercent;
		CString m_szSingleDefectCheckPercent;
		CString m_szTotalDefectCheckPercent;
		CString m_szGreyLevelDefectType;
		CString m_szEnableCheckDefect;
		CString m_szDieCheckDefectThreshold;
		CString m_szBadCutDieDetectionXTolerance;
		CString m_szBadCutDieDetectionYTolerance;
		CString m_szEnableLookForward;
		CString m_szEnableThetaCorrection;
		CString m_szNoDieMaxCount;
		CString m_szPRDieMaxSkipCount;
		CString m_szPreScanSearchDieScore;
		CString m_szNormalDieSearchAreaX;
		CString m_szNormalDieSearchAreaY;
		CString m_szReferenceDieSearchAreaX;
		CString m_szReferenceDieSearchAreaY;
		
		CString m_szBadCutDetection;
		CString m_szEjrCapCleanLimit;
};

class CBondPrDataBlock: public CDataBlock
{
	public:
		CBondPrDataBlock();
		~CBondPrDataBlock();

		CString m_szEnableDieRotationAverageAngleCheck;
		CString m_szDieRotationAverageAngle;
		CString m_szEnableDieRotationMaxAngleCheck;
		CString m_szDieRotationMaxAngle;
		CString m_szDieRotationMaxAngleMaxDieAllow;
		CString m_szEnableDiePlacementCheck;
		CString m_szPostBondDiePlacementMaxXShift;
		CString m_szPostBondDiePlacementMaxYShift;
		CString m_szPostBondDiePlacementMaxDieAllow;
		CString m_szEnableEmptyDieCheck;
		CString m_szEmptyDieCheckMaxDieAllow;
		CString m_szEnablePostBondInspection;
		CString m_szBondDiePassScore;

		CString m_szPRCenterX;
		CString m_szPRCenterY;
};

class CSafetyDataBlock: public CDataBlock
{
	public:
		CSafetyDataBlock();
		~CSafetyDataBlock();
	
	CString m_szEnableLanConnectionCheck;
};



class CToolsUsageRecord: public CObject
{
	public:
		CToolsUsageRecord();
		~CToolsUsageRecord();

		VOID SetRecordPath(CString szRecordPath);
		VOID SetRecordPath2(CString szRecordPath2);
		VOID SetMachineNo(CString szMachineNo);
		VOID SetOperatorId(CString szOperatorId);

		BOOL GenerateColletUsageRecordFile(CColletRecord& oColletRecord, UCHAR ucFormatType, CString szCustomer = "");
		BOOL GenerateEjectorUsageRecordFile(CEjectorRecord& oEjectorRecord, UCHAR ucFormatType, CString szCustomer = "");
		BOOL GenearteParametersRecordFile();

		VOID SetMS896ADataBlock(CMS896ADataBlock& oMS896ADataBlk);
		VOID SetBinTableDataBlock(CBinTableDataBlock& oBinTableDataBlk);
		VOID SetBondHeadDataBlock(CBondHeadDataBlock& oBondHeadDataBlk);
		VOID SetWaferTableDataBlock(CWaferTableDataBlock& oWaferTableDataBlk);
		VOID SetWaferPrDataBlock(CWaferPrDataBlock& oWaferPrDataBlk);
		VOID SetBondPrDataBlock(CBondPrDataBlock& oBondPrDataBlk);
		VOID SetBinLoaderDatatBlock(CBinLoaderDataBlock& oBinLoaderDataBlk);
		VOID SetSafetyDataBlock(CSafetyDataBlock& oSafetyDataBlk);

		VOID SetLogItems(CString szStatus);
	
	private:
		CMS896ADataBlock		m_oMS896ADataBlk;
		CBinTableDataBlock		m_oBinTableDataBlk;
		CBondHeadDataBlock		m_oBondHeadDataBlk;
		CWaferTableDataBlock	m_oWaferTableDataBlk;
		CWaferPrDataBlock		m_oWaferPrDataBlk;
		CBondPrDataBlock		m_oBondPrDataBlk;
		CBinLoaderDataBlock		m_oBinLoaderDataBlk;
		CSafetyDataBlock		m_oSafetyDataBlock;

		CString m_szMachineNo;
		CString m_szOperatorId;
		CString m_szRecordPath;
		CString m_szRecordPath2;	//v4.48A5	//3E DL
};
