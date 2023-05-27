/////////////////////////////////////////////////////////////////
// DevFile.cpp : Device File Class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, January 21, 2005
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2005.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h" 
#include "ToolsUsageRecord.h"
#include "stdio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CColletRecord::CColletRecord()
{
	m_bShowColletDiagPage = FALSE;
	m_szShiftNo = "";
	m_szOperatorId = "";
	m_szReason = "";
	m_szType = "";
	m_szRemark = ""; 
	m_szColletMode		= "";
	m_szCollet1Mode		= "";
	m_szCollet2Mode		= "";
	m_szColletModeOld	= "";
	m_szTypeOld			= "";
	m_szRemarkOld		= "";
	m_szPickLevel = "";
	m_szBondLevel = "";
	m_szOperatorLogInId = "";
	m_szColletBondedDieCount = "";
	m_szMachineNo = "";
	m_szOperatorId = "";
	m_szBondArmNo = "";
	m_szProductType = "";
}


CColletRecord::~CColletRecord()
{

}

CEjectorRecord::CEjectorRecord()
{
	m_bShowEjectorDiagPage = FALSE;
	m_szShiftNo = "";
	m_szOperatorId = "";
	m_szReason = "";
	m_szType = "";
	m_szRemark = "";
	m_szEjtTypeOld		= "";
	m_szEjtRemarkOld	= "";
	m_szEjectorUpLevel	= "";
	m_szOperatorLogInId = "";
	m_szEjectorPinCount = "";
	m_szProductType		= "";
	m_szUsageType		= "";
}

CEjectorRecord::~CEjectorRecord()
{

}

CDataBlock::CDataBlock()
{
}

CDataBlock::~CDataBlock()
{
}

CString CDataBlock::ConvertBoolToOnOff(BOOL bEnable)
{
	if (bEnable)
	{
		return "On";
	}
	else
	{
		return "Off";
	}
}

//----- CMS896ADataBlock-------------------------------
//

CMS896ADataBlock::CMS896ADataBlock()
{
	m_szMachineNo = "";
	m_szLoginID = "";
	m_szVersion = "";
}

CMS896ADataBlock::~CMS896ADataBlock()
{

}

//--------------------------------------------------------------


//----- CBinTableDataBlock -------------------------------
//
CBinTableDataBlock::CBinTableDataBlock()
{
	m_szReAlignMethod = "";
	m_szBinBlockDieQuantities = "";
	m_szBinBlockWalkPath = "";
	m_szNoOfBinBlocks = "";
	m_szMaxDieQuantities = "";
	m_szFrameReAlignLastDieOffsetX = "";
	m_szFrmaeRealignLastDieOffsetY = "";
	m_szChangeGradeLimit = "";
}

CBinTableDataBlock::~CBinTableDataBlock()
{

}

//---------------------------------------------------------------

//----- CBinLoaderDataBlock -------------------------------
CBinLoaderDataBlock::CBinLoaderDataBlock()
{
	m_szEnableBinTableVacuum = "";
	m_szFrameReAlignDelay = "";
	m_szFrameReAlignRetryCount = "";
	m_szBinVacuumSettleDelay = "";
	m_szBinVacuumDelay = "";
	m_szEnableFrameRealign = "";
	m_szCheckElevatorCover = "";
}

CBinLoaderDataBlock::~CBinLoaderDataBlock()
{

}

//---------------------------------------------------------------

//----- CBondHeadDataBlock -------------------------------

CBondHeadDataBlock::CBondHeadDataBlock()
{
	m_szEnableMissingDie = "";
	m_szMissingDieRetryCount = "";
	m_szColletCleanLimit = "";
	m_szPickPosition = "";
	m_szBondPosition = "";
	m_szPickLevelZ1 = "";
	m_szBondLevelZ1 = "";
	m_szPickLevelZ2 = "";
	m_szBondLevelZ2 = "";
	m_szEnableSyncPick = "";
	m_szSyncMoveTriggerLevel = "";
	m_szSyncMovePreload = "";
	m_szEjectorUpLevel = "";
	m_szEnableColletJam = "";
	m_szColletJamRetryCount = "";
	m_szCleanColletPosition = "";
	m_szCleanColletLevel = "";
	m_szStandbyLevel_Ej = "";
	m_szEnableAutoCleanCollet = "";
	m_szEnabelCheckCoverSensor = "";

	m_szPickDelay = "";
	m_szBondDelay = "";
	m_szArmPickDelay = "";
	m_szArmBondDelay = "";
	m_szHeadPickDelay = "";
	m_szHeadBondDelay = "";
	m_szHeadPrePickDelay = "";
	m_szSyncPickDelay = "";
	m_szEjectorUpDelay = "";
	m_szEjectorDownDelay = "";
	m_szEjReadyDelay = "";
	m_szEjVacOffDelay = "";
	m_szPRDelay = "";
	m_szWTTDelay = "";
	m_szBinTableDelay = "";
}

CBondHeadDataBlock::~CBondHeadDataBlock()
{

}


//---------------------------------------------------------------

//----- CBondHeadDataBlock -------------------------------
CWaferTableDataBlock::CWaferTableDataBlock()
{
	m_szEnableDiePitchCheck = "";
	m_szEnableMapDieTypeCheck = "";
	m_szMapDieTypeCheckDieType = "";
	
	m_szEnablePreScan = "";
	m_szEnableAdaptPredict = "";
	m_szPreScanPrDelay = "";
	m_szWaferMapPath = "";
	m_szWaferMapTour = "";
	m_szWaferMapMode = "";

	m_szSortingMode = "";
	m_szAlignOption = "";
	m_szHomeDieRow = "";
	m_szHomeDieCol = "";

	m_szEnableDEBAdaptive	= "";
	m_szEnableAdvOffset		= "";
	m_szEnableRescanWafer	= "";
	m_szStage1Start			= "";
	m_szStage1Period		= "";
	m_szStage2Start			= "";
	m_szStage2Period		= "";
	m_szStage3Start			= "";
	m_szStage3Period		= "";
	m_szRegionGridRow		= "";
	m_szRegionGridCol		= "";
}

CWaferTableDataBlock::~CWaferTableDataBlock()
{
}

//---------------------------------------------------------------

//----- CWaferPrDataBlock -------------------------------
CWaferPrDataBlock::CWaferPrDataBlock()
{
	m_szPrLrnCoaxLightLevel = "";
	m_szPrLrnRingLightLevel = "";
	m_szPrLrnSideLightLevel = "";
	m_szPrDiePassScore = "";
	m_szRefDiePRPassScore = "";
	m_szDefectCheckChipPercent = "";
	m_szSingleDefectCheckPercent = "";
	m_szTotalDefectCheckPercent = "";
	m_szGreyLevelDefectType = "";
	m_szEnableCheckDefect = "";
	m_szEnableCheckChip = "";
	m_szDieCheckDefectThreshold = "";
	m_szBadCutDieDetectionXTolerance = "";
	m_szBadCutDieDetectionYTolerance = "";
	m_szEnableLookForward = "";
	m_szEnableThetaCorrection = "";
	m_szNoDieMaxCount = "";
	m_szPRDieMaxSkipCount = "";
	m_szPreScanSearchDieScore = "";
	m_szNormalDieSearchAreaX = "";
	m_szNormalDieSearchAreaY = "";
	m_szReferenceDieSearchAreaX = "";
	m_szReferenceDieSearchAreaY = "";
}

CWaferPrDataBlock::~CWaferPrDataBlock()
{

}

//---------------------------------------------------------------

//----- CBondPrDataBlock -------------------------------
CBondPrDataBlock::CBondPrDataBlock()
{
	m_szEnableDieRotationAverageAngleCheck = "";
	m_szDieRotationAverageAngle = "";
	m_szDieRotationMaxAngle = "";
	m_szEnableDieRotationMaxAngleCheck = "";
	m_szDieRotationMaxAngleMaxDieAllow = "";
	m_szEnableDiePlacementCheck = "";
	m_szEnableEmptyDieCheck = "";
	m_szPostBondDiePlacementMaxXShift = "";
	m_szPostBondDiePlacementMaxYShift = "";
	m_szPostBondDiePlacementMaxDieAllow = "";
	m_szEnablePostBondInspection = "";
	m_szBondDiePassScore = "";
	m_szEnableEmptyDieCheck = "";
	m_szEmptyDieCheckMaxDieAllow = "";
}

CBondPrDataBlock::~CBondPrDataBlock()
{
}

CSafetyDataBlock::CSafetyDataBlock()
{
	m_szEnableLanConnectionCheck = "";
}

CSafetyDataBlock::~CSafetyDataBlock()
{

}

//---------------------------------------------------------------



CToolsUsageRecord::CToolsUsageRecord()
{
	CreateDirectory(ToolsUsageRecordFileBackupPath, NULL);
	m_szRecordPath		= _T("");
	m_szRecordPath2		= _T("");
}

CToolsUsageRecord::~CToolsUsageRecord()
{

}

VOID CToolsUsageRecord::SetRecordPath(CString szRecordPath)
{
	m_szRecordPath = szRecordPath;
}

VOID CToolsUsageRecord::SetRecordPath2(CString szRecordPath2)
{
	m_szRecordPath2 = szRecordPath2;
}

VOID CToolsUsageRecord::SetMachineNo(CString szMachineNo)
{
	m_szMachineNo = szMachineNo;
}

VOID CToolsUsageRecord::SetOperatorId(CString szOperatorId)
{
	m_szOperatorId = szOperatorId;
}

VOID CToolsUsageRecord::SetMS896ADataBlock(CMS896ADataBlock& oMS896ADataBlk)
{
	m_oMS896ADataBlk.m_szLoginID = oMS896ADataBlk.m_szLoginID;
	m_oMS896ADataBlk.m_szMachineNo = oMS896ADataBlk.m_szMachineNo;
	m_oMS896ADataBlk.m_szVersion = oMS896ADataBlk.m_szVersion;
}

VOID CToolsUsageRecord::SetBinTableDataBlock(CBinTableDataBlock& oBinTableDataBlk)
{
	m_oBinTableDataBlk.m_szBinBlockDieQuantities = 	oBinTableDataBlk.m_szBinBlockDieQuantities;
	m_oBinTableDataBlk.m_szBinBlockWalkPath = oBinTableDataBlk.m_szBinBlockWalkPath;
	m_oBinTableDataBlk.m_szFrameReAlignLastDieOffsetX = oBinTableDataBlk.m_szFrameReAlignLastDieOffsetX;
	m_oBinTableDataBlk.m_szFrmaeRealignLastDieOffsetY = oBinTableDataBlk.m_szFrmaeRealignLastDieOffsetY;
	m_oBinTableDataBlk.m_szMaxDieQuantities = oBinTableDataBlk.m_szMaxDieQuantities;
	m_oBinTableDataBlk.m_szNoOfBinBlocks = oBinTableDataBlk.m_szNoOfBinBlocks;
	m_oBinTableDataBlk.m_szReAlignMethod = oBinTableDataBlk.m_szReAlignMethod;

	m_oBinTableDataBlk.m_szChangeGradeLimit = oBinTableDataBlk.m_szChangeGradeLimit;
	m_oBinTableDataBlk.m_szBHZ2BondPosOffsetX	= oBinTableDataBlk.m_szBHZ2BondPosOffsetX;
	m_oBinTableDataBlk.m_szBHZ2BondPosOffsetY	= oBinTableDataBlk.m_szBHZ2BondPosOffsetY;
}

VOID CToolsUsageRecord::SetBondHeadDataBlock(CBondHeadDataBlock& oBondHeadDataBlk)
{
	m_oBondHeadDataBlk.m_szAvgCycleTime = oBondHeadDataBlk.m_szAvgCycleTime;
	m_oBondHeadDataBlk.m_szBondLevelZ1 = oBondHeadDataBlk.m_szBondLevelZ1;
	m_oBondHeadDataBlk.m_szPickLevelZ1 = oBondHeadDataBlk.m_szPickLevelZ1;
	m_oBondHeadDataBlk.m_szBondLevelZ2 = oBondHeadDataBlk.m_szBondLevelZ2;
	m_oBondHeadDataBlk.m_szPickLevelZ2 = oBondHeadDataBlk.m_szPickLevelZ2;
	m_oBondHeadDataBlk.m_szEnableSyncPick = oBondHeadDataBlk.m_szEnableSyncPick;
	m_oBondHeadDataBlk.m_szSyncMoveTriggerLevel = oBondHeadDataBlk.m_szSyncMoveTriggerLevel;
	m_oBondHeadDataBlk.m_szSyncMovePreload = oBondHeadDataBlk.m_szSyncMovePreload;
	m_oBondHeadDataBlk.m_szBondPosition = oBondHeadDataBlk.m_szBondPosition;
	m_oBondHeadDataBlk.m_szPickPosition = oBondHeadDataBlk.m_szPickPosition;
	m_oBondHeadDataBlk.m_szColletCleanLimit = oBondHeadDataBlk.m_szColletCleanLimit;
	m_oBondHeadDataBlk.m_szCycleTime = oBondHeadDataBlk.m_szCycleTime;
	m_oBondHeadDataBlk.m_szEjectorUpLevel = oBondHeadDataBlk.m_szEjectorUpLevel;
	m_oBondHeadDataBlk.m_szEjStandbyLevel = oBondHeadDataBlk.m_szEjStandbyLevel;
	m_oBondHeadDataBlk.m_szEnableColletJam = oBondHeadDataBlk.m_szEnableColletJam;
	m_oBondHeadDataBlk.m_szColletJamRetryCount = oBondHeadDataBlk.m_szColletJamRetryCount;
	m_oBondHeadDataBlk.m_szEnableMissingDie = oBondHeadDataBlk.m_szEnableMissingDie;
	m_oBondHeadDataBlk.m_szMissingDieRetryCount = oBondHeadDataBlk.m_szMissingDieRetryCount;

	m_oBondHeadDataBlk.m_szCleanColletPosition = oBondHeadDataBlk.m_szCleanColletPosition;
	m_oBondHeadDataBlk.m_szCleanColletLevel = oBondHeadDataBlk.m_szCleanColletLevel ;
	m_oBondHeadDataBlk.m_szStandbyLevel_Ej = oBondHeadDataBlk.m_szStandbyLevel_Ej;
	m_oBondHeadDataBlk.m_szEnableAutoCleanCollet = oBondHeadDataBlk.m_szEnableAutoCleanCollet;
	m_oBondHeadDataBlk.m_szEnabelCheckCoverSensor = oBondHeadDataBlk.m_szEnabelCheckCoverSensor;

	m_oBondHeadDataBlk.m_szPickDelay		= oBondHeadDataBlk.m_szPickDelay;
	m_oBondHeadDataBlk.m_szBondDelay		= oBondHeadDataBlk.m_szBondDelay;
	m_oBondHeadDataBlk.m_szArmPickDelay		= oBondHeadDataBlk.m_szArmPickDelay;
	m_oBondHeadDataBlk.m_szArmBondDelay		= oBondHeadDataBlk.m_szArmBondDelay;
	m_oBondHeadDataBlk.m_szHeadPickDelay	= oBondHeadDataBlk.m_szHeadPickDelay;
	m_oBondHeadDataBlk.m_szHeadBondDelay	= oBondHeadDataBlk.m_szHeadBondDelay;
	m_oBondHeadDataBlk.m_szHeadPrePickDelay	= oBondHeadDataBlk.m_szHeadPrePickDelay;
	m_oBondHeadDataBlk.m_szSyncPickDelay	= oBondHeadDataBlk.m_szSyncPickDelay;
	m_oBondHeadDataBlk.m_szEjectorUpDelay	= oBondHeadDataBlk.m_szEjectorUpDelay;
	m_oBondHeadDataBlk.m_szEjectorDownDelay	= oBondHeadDataBlk.m_szEjectorDownDelay;
	m_oBondHeadDataBlk.m_szEjReadyDelay		= oBondHeadDataBlk.m_szEjReadyDelay;
	m_oBondHeadDataBlk.m_szEjVacOffDelay	= oBondHeadDataBlk.m_szEjVacOffDelay;
	m_oBondHeadDataBlk.m_szPRDelay			= oBondHeadDataBlk.m_szPRDelay;
	m_oBondHeadDataBlk.m_szWTTDelay			= oBondHeadDataBlk.m_szWTTDelay;
	m_oBondHeadDataBlk.m_szBinTableDelay	= oBondHeadDataBlk.m_szBinTableDelay;
}

VOID CToolsUsageRecord::SetWaferTableDataBlock(CWaferTableDataBlock& oWaferTableDataBlk)
{
	m_oWaferTableDataBlk.m_szEnableDiePitchCheck = oWaferTableDataBlk.m_szEnableDiePitchCheck;
	m_oWaferTableDataBlk.m_szMapDieTypeCheckDieType = oWaferTableDataBlk.m_szMapDieTypeCheckDieType;
	m_oWaferTableDataBlk.m_szEnableMapDieTypeCheck = oWaferTableDataBlk.m_szEnableMapDieTypeCheck;

	m_oWaferTableDataBlk.m_szEnablePreScan = oWaferTableDataBlk.m_szEnablePreScan;
	m_oWaferTableDataBlk.m_szEnableAdaptPredict = oWaferTableDataBlk.m_szEnablePreScan;
	m_oWaferTableDataBlk.m_szPreScanPrDelay = oWaferTableDataBlk.m_szPreScanPrDelay;
	m_oWaferTableDataBlk.m_szWaferMapPath = oWaferTableDataBlk.m_szWaferMapPath;
	m_oWaferTableDataBlk.m_szWaferMapTour = oWaferTableDataBlk.m_szWaferMapTour;
	m_oWaferTableDataBlk.m_szWaferMapMode = oWaferTableDataBlk.m_szWaferMapMode;

	m_oWaferTableDataBlk.m_szSortingMode = oWaferTableDataBlk.m_szSortingMode;
	m_oWaferTableDataBlk.m_szAlignOption = oWaferTableDataBlk.m_szAlignOption;
	m_oWaferTableDataBlk.m_szHomeDieRow = oWaferTableDataBlk.m_szHomeDieRow;
	m_oWaferTableDataBlk.m_szHomeDieCol = oWaferTableDataBlk.m_szHomeDieCol;

	m_oWaferTableDataBlk.m_szEnableAdvOffset	= oWaferTableDataBlk.m_szEnableAdvOffset;
	m_oWaferTableDataBlk.m_szEnableRescanWafer	= oWaferTableDataBlk.m_szEnableRescanWafer;
	m_oWaferTableDataBlk.m_szEnableDEBAdaptive	= oWaferTableDataBlk.m_szEnableDEBAdaptive;
	m_oWaferTableDataBlk.m_szStage1Start		= oWaferTableDataBlk.m_szStage1Start;
	m_oWaferTableDataBlk.m_szStage1Period		= oWaferTableDataBlk.m_szStage1Period;
	m_oWaferTableDataBlk.m_szStage2Start		= oWaferTableDataBlk.m_szStage2Start;
	m_oWaferTableDataBlk.m_szStage2Period		= oWaferTableDataBlk.m_szStage2Period;
	m_oWaferTableDataBlk.m_szStage3Start		= oWaferTableDataBlk.m_szStage3Start;
	m_oWaferTableDataBlk.m_szStage3Period		= oWaferTableDataBlk.m_szStage3Period;
	m_oWaferTableDataBlk.m_szRegionGridRow		= oWaferTableDataBlk.m_szRegionGridRow;
	m_oWaferTableDataBlk.m_szRegionGridCol		= oWaferTableDataBlk.m_szRegionGridCol;

	m_oWaferTableDataBlk.m_szAutoSrchHome		= oWaferTableDataBlk.m_szAutoSrchHome;
	m_oWaferTableDataBlk.m_szWaferID			= oWaferTableDataBlk.m_szWaferID;
	m_oWaferTableDataBlk.m_szPitchX_Tolerance	= oWaferTableDataBlk.m_szPitchX_Tolerance;
	m_oWaferTableDataBlk.m_szPitchY_Tolerance	= oWaferTableDataBlk.m_szPitchY_Tolerance;
	m_oWaferTableDataBlk.m_szDiePitchCheck		= oWaferTableDataBlk.m_szDiePitchCheck;
	m_oWaferTableDataBlk.m_szScnCheckByPR		= oWaferTableDataBlk.m_szScnCheckByPR;
	m_oWaferTableDataBlk.m_szEnableScnCheck		= oWaferTableDataBlk.m_szEnableScnCheck;
	m_oWaferTableDataBlk.m_szScnCheckIsRefDie	= oWaferTableDataBlk.m_szScnCheckIsRefDie;
	//m_oWaferTableDataBlk.m_szPrescanDiePassScore = oWaferTableDataBlk.m_szPrescanDiePassScore;
}

VOID CToolsUsageRecord::SetWaferPrDataBlock(CWaferPrDataBlock& oWaferPrDataBlk)
{
	m_oWaferPrDataBlk.m_szBadCutDieDetectionXTolerance = oWaferPrDataBlk.m_szBadCutDieDetectionXTolerance;
	m_oWaferPrDataBlk.m_szBadCutDieDetectionYTolerance = oWaferPrDataBlk.m_szBadCutDieDetectionYTolerance;
	m_oWaferPrDataBlk.m_szDefectCheckChipPercent = oWaferPrDataBlk.m_szDefectCheckChipPercent;
	m_oWaferPrDataBlk.m_szDieCheckDefectThreshold = oWaferPrDataBlk.m_szDieCheckDefectThreshold;
	m_oWaferPrDataBlk.m_szEnableLookForward = oWaferPrDataBlk.m_szEnableLookForward;
	m_oWaferPrDataBlk.m_szEnableThetaCorrection = oWaferPrDataBlk.m_szEnableThetaCorrection;
	m_oWaferPrDataBlk.m_szGreyLevelDefectType = oWaferPrDataBlk.m_szGreyLevelDefectType;
	m_oWaferPrDataBlk.m_szEnableCheckDefect = oWaferPrDataBlk.m_szEnableCheckDefect;
	m_oWaferPrDataBlk.m_szEnableCheckChip = oWaferPrDataBlk.m_szEnableCheckChip;
	m_oWaferPrDataBlk.m_szNoDieMaxCount = oWaferPrDataBlk.m_szNoDieMaxCount;
	m_oWaferPrDataBlk.m_szPRDieMaxSkipCount = oWaferPrDataBlk.m_szPRDieMaxSkipCount;
	m_oWaferPrDataBlk.m_szPrDiePassScore = oWaferPrDataBlk.m_szPrDiePassScore;
	m_oWaferPrDataBlk.m_szPrLrnCoaxLightLevel = oWaferPrDataBlk.m_szPrLrnCoaxLightLevel;
	m_oWaferPrDataBlk.m_szPrLrnRingLightLevel = oWaferPrDataBlk.m_szPrLrnRingLightLevel;
	m_oWaferPrDataBlk.m_szPrLrnSideLightLevel = oWaferPrDataBlk.m_szPrLrnSideLightLevel;
	
	m_oWaferPrDataBlk.m_szNormalDieSearchAreaX = oWaferPrDataBlk.m_szNormalDieSearchAreaX;
	m_oWaferPrDataBlk.m_szNormalDieSearchAreaY = oWaferPrDataBlk.m_szNormalDieSearchAreaY;
	m_oWaferPrDataBlk.m_szReferenceDieSearchAreaX = oWaferPrDataBlk.m_szReferenceDieSearchAreaX;
	m_oWaferPrDataBlk.m_szReferenceDieSearchAreaY = oWaferPrDataBlk.m_szReferenceDieSearchAreaY;

	m_oWaferPrDataBlk.m_szRefDiePRPassScore = oWaferPrDataBlk.m_szRefDiePRPassScore;
	m_oWaferPrDataBlk.m_szSingleDefectCheckPercent = oWaferPrDataBlk.m_szSingleDefectCheckPercent;
	m_oWaferPrDataBlk.m_szTotalDefectCheckPercent = oWaferPrDataBlk.m_szTotalDefectCheckPercent;
	m_oWaferPrDataBlk.m_szPreScanSearchDieScore = oWaferPrDataBlk.m_szPreScanSearchDieScore;

	m_oWaferPrDataBlk.m_szBadCutDetection = oWaferPrDataBlk.m_szBadCutDetection;
	m_oWaferPrDataBlk.m_szEjrCapCleanLimit = oWaferPrDataBlk.m_szEjrCapCleanLimit;

}

VOID CToolsUsageRecord::SetBondPrDataBlock(CBondPrDataBlock& oBondPrDataBlk)
{
	m_oBondPrDataBlk.m_szBondDiePassScore = oBondPrDataBlk.m_szBondDiePassScore;
	m_oBondPrDataBlk.m_szEnablePostBondInspection = oBondPrDataBlk.m_szEnablePostBondInspection;
	m_oBondPrDataBlk.m_szPostBondDiePlacementMaxDieAllow = oBondPrDataBlk.m_szPostBondDiePlacementMaxDieAllow;
	m_oBondPrDataBlk.m_szEnableDieRotationAverageAngleCheck = oBondPrDataBlk.m_szEnableDieRotationAverageAngleCheck;
	m_oBondPrDataBlk.m_szDieRotationAverageAngle = oBondPrDataBlk.m_szDieRotationAverageAngle;
	m_oBondPrDataBlk.m_szDieRotationMaxAngle = oBondPrDataBlk.m_szDieRotationMaxAngle;
	m_oBondPrDataBlk.m_szEnableDieRotationMaxAngleCheck = oBondPrDataBlk.m_szEnableDieRotationMaxAngleCheck;
	m_oBondPrDataBlk.m_szDieRotationMaxAngleMaxDieAllow = oBondPrDataBlk.m_szDieRotationMaxAngleMaxDieAllow;
	m_oBondPrDataBlk.m_szEnableDiePlacementCheck = oBondPrDataBlk.m_szEnableDiePlacementCheck;
	m_oBondPrDataBlk.m_szPostBondDiePlacementMaxXShift = oBondPrDataBlk.m_szPostBondDiePlacementMaxXShift;
	m_oBondPrDataBlk.m_szPostBondDiePlacementMaxYShift = oBondPrDataBlk.m_szPostBondDiePlacementMaxYShift;
	m_oBondPrDataBlk.m_szEnableEmptyDieCheck = oBondPrDataBlk.m_szEnableEmptyDieCheck;
	m_oBondPrDataBlk.m_szEmptyDieCheckMaxDieAllow = oBondPrDataBlk.m_szEmptyDieCheckMaxDieAllow;
	m_oBondPrDataBlk.m_szPRCenterX = oBondPrDataBlk.m_szPRCenterX;
	m_oBondPrDataBlk.m_szPRCenterY = oBondPrDataBlk.m_szPRCenterY;
}

VOID CToolsUsageRecord::SetBinLoaderDatatBlock(CBinLoaderDataBlock& oBinLoaderDataBlk)
{
	m_oBinLoaderDataBlk.m_szEnableBinTableVacuum = oBinLoaderDataBlk.m_szEnableBinTableVacuum;
	m_oBinLoaderDataBlk.m_szBinVacuumDelay = oBinLoaderDataBlk.m_szBinVacuumDelay;
	m_oBinLoaderDataBlk.m_szBinVacuumSettleDelay = oBinLoaderDataBlk.m_szBinVacuumSettleDelay;
	m_oBinLoaderDataBlk.m_szFrameReAlignDelay = oBinLoaderDataBlk.m_szFrameReAlignDelay;
	m_oBinLoaderDataBlk.m_szFrameReAlignRetryCount = oBinLoaderDataBlk.m_szFrameReAlignRetryCount;
	m_oBinLoaderDataBlk.m_szEnableFrameRealign	= oBinLoaderDataBlk.m_szEnableFrameRealign;

	m_oBinLoaderDataBlk.m_szCheckElevatorCover = oBinLoaderDataBlk.m_szCheckElevatorCover;
}

VOID CToolsUsageRecord::SetSafetyDataBlock(CSafetyDataBlock& oSafetyDataBlk)
{
	m_oSafetyDataBlock.m_szEnableLanConnectionCheck = oSafetyDataBlk.m_szEnableLanConnectionCheck;
}

BOOL CToolsUsageRecord::GenerateColletUsageRecordFile(CColletRecord& oColletRecord, UCHAR ucFormatType, CString szCustomer)
{
	CString szReportPath;
	CString szBackupFilename;
	CStdioFile cfFile;
	CString szDateTime;
	CString szFilename;
	CString szDateOfReplace, szTimeOfReplace;
	CTime curTime = CTime::GetCurrentTime();

	szDateTime = curTime.Format("%Y%m");
	szDateOfReplace = curTime.Format("%Y-%m-%d");
	szTimeOfReplace = curTime.Format("%H:%M:%S");

	szBackupFilename = ToolsUsageRecordFileBackupPath;
	szBackupFilename = szBackupFilename + "\\" + oColletRecord.m_szMachineNo + "_Collet" + szDateTime + ".csv";

	if (cfFile.Open(szBackupFilename, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate) == FALSE)
	{
		return FALSE;	
	}

	cfFile.SeekToEnd();
	
	if (cfFile.GetLength() == 0)
	{
		if( ucFormatType==1 )
		{
			cfFile.WriteString("Machine No,Date of Replacement,Time of Replacement,Arm,Shift of Replacement,Collet Mode,Reasons of Replacement,Collet Type,Collet Bonded Die Count,Pick Level,Bond Level,Staff performed,Login ID,Remarks,Old Collet Type, Old Remarks\n");
		}
		else
		{
			if (szCustomer == "ChangeLight(XM)")
			{
				cfFile.WriteString("Machine No,Date of Replacement,Time of Replacement,Arm,Shift of Replacement,Reasons of Replacement,Collet Type,Collet Bonded Die Count,Pick Level,Bond Level,Staff performed,Login ID,Product Type,Usage Type,Remarks\n");
			}
			else
			{
				cfFile.WriteString("Machine No,Date of Replacement,Time of Replacement,Arm,Shift of Replacement,Reasons of Replacement,Collet Type,Collet Bonded Die Count,Pick Level,Bond Level,Staff performed,Login ID,Remarks\n");
			}
		}
	}

	if( ucFormatType==1 )
	{
		cfFile.WriteString( oColletRecord.m_szMachineNo + "," + 
							szDateOfReplace				+ "," + 
							szTimeOfReplace				+ "," + 
							oColletRecord.m_szBondArmNo + "," + 
							oColletRecord.m_szShiftNo	+ "," + 
							oColletRecord.m_szColletModeOld		+ "," +
							oColletRecord.m_szReason	+ "," + 
							oColletRecord.m_szType		+ "," + 
							oColletRecord.m_szColletBondedDieCount	+ "," + 
							oColletRecord.m_szPickLevel + "," + 
							oColletRecord.m_szBondLevel + "," + 
							oColletRecord.m_szOperatorId		+ "," + 
							oColletRecord.m_szOperatorLogInId	+ "," + 
							oColletRecord.m_szRemark	+ "," + 
							oColletRecord.m_szTypeOld	+ "," + 
							oColletRecord.m_szRemarkOld + 
							"\n");
	}
	else
	{
		if (szCustomer == "ChangeLight(XM)")
		{
			cfFile.WriteString(oColletRecord.m_szMachineNo + "," + szDateOfReplace + "," + szTimeOfReplace + "," + oColletRecord.m_szBondArmNo 
			+ "," + oColletRecord.m_szShiftNo + "," + oColletRecord.m_szReason + "," + oColletRecord.m_szType + "," + oColletRecord.m_szColletBondedDieCount 
			+ "," + oColletRecord.m_szPickLevel + "," + oColletRecord.m_szBondLevel + "," + oColletRecord.m_szOperatorId + 
			"," + oColletRecord.m_szOperatorLogInId + "," + oColletRecord.m_szProductType + "," + oColletRecord.m_szColletMode + "," + oColletRecord.m_szRemark + "\n");
		}
		else
		{
			cfFile.WriteString(oColletRecord.m_szMachineNo + "," + szDateOfReplace + "," + szTimeOfReplace + "," + oColletRecord.m_szBondArmNo 
			+ "," + oColletRecord.m_szShiftNo + "," + oColletRecord.m_szReason + "," + oColletRecord.m_szType + "," + oColletRecord.m_szColletBondedDieCount 
			+ "," + oColletRecord.m_szPickLevel + "," + oColletRecord.m_szBondLevel + "," + oColletRecord.m_szOperatorId + 
			"," + oColletRecord.m_szOperatorLogInId + "," + oColletRecord.m_szRemark + "\n");
		}
	}

	cfFile.Close();

	szReportPath = m_szRecordPath + "\\Collet";
	if (m_szRecordPath2.GetLength() > 0)	//v4.48A4
		szReportPath = m_szRecordPath2 + "\\Collet";

	CreateDirectory(szReportPath, NULL);

	szFilename = szReportPath + "\\" + oColletRecord.m_szMachineNo + "_Collet" + szDateTime + ".csv";
	
	if (szBackupFilename != szFilename)
	{
		CopyFile(szBackupFilename, szFilename, FALSE);
	}
	
	return TRUE;
}

BOOL CToolsUsageRecord::GenerateEjectorUsageRecordFile(CEjectorRecord& oEjectorRecord, UCHAR ucFormatType, CString szCustomer)
{
	CString szReportPath;
	CString szBackupFilename;
	CStdioFile cfFile;
	CString szDateTime;
	CString szFilename;
	CString szDateOfReplace, szTimeOfReplace;
	CTime curTime = CTime::GetCurrentTime();

	szDateTime = curTime.Format("%Y%m");
	szDateOfReplace = curTime.Format("%Y-%m-%d");
	szTimeOfReplace = curTime.Format("%H:%M:%S");

	szBackupFilename = ToolsUsageRecordFileBackupPath;
	szBackupFilename = szBackupFilename + "\\" + oEjectorRecord.m_szMachineNo + "_Ejector" + szDateTime + ".csv";

	if (cfFile.Open(szBackupFilename, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate) == FALSE)
	{
		return FALSE;	
	}

	cfFile.SeekToEnd();
	
	if (cfFile.GetLength() == 0)
	{
		if( ucFormatType==1 )
		{
			cfFile.WriteString("Machine No,Date of Replacement,Time of Replacement,Shift of Replacement,Reasons of Replacement,Ejector Pin Type,Ejector Pin Count,Ejector up Level,Staff performed,Login ID,Remarks,Old Ejector Pin Type,Old Remarks\n");
		}
		else
		{
			if (szCustomer == "ChangeLight(XM)")
			{
				cfFile.WriteString("Machine No,Date of Replacement,Time of Replacement,Shift of Replacement,Reasons of Replacement,Ejector Pin Type,Ejector Pin Count,Ejector up Level,Staff performed,Login ID,Product Type,Usage Type,Remarks\n");
			}
			else
			{
				cfFile.WriteString("Machine No,Date of Replacement,Time of Replacement,Shift of Replacement,Reasons of Replacement,Ejector Pin Type,Ejector Pin Count,Ejector up Level,Staff performed,Login ID,Remarks\n");
			}
		}
	}
	
	if( ucFormatType==1 )
		cfFile.WriteString( oEjectorRecord.m_szMachineNo	+ "," + 
						szDateOfReplace						+ "," + 
						szTimeOfReplace						+ "," + 
						oEjectorRecord.m_szShiftNo			+ "," + 
						oEjectorRecord.m_szReason			+ "," + 
						oEjectorRecord.m_szType				+ "," + 
						oEjectorRecord.m_szEjectorPinCount	+ "," + 
						oEjectorRecord.m_szEjectorUpLevel	+ "," + 
						oEjectorRecord.m_szOperatorId		+ "," + 
						oEjectorRecord.m_szOperatorLogInId	+ "," + 
						oEjectorRecord.m_szRemark			+ "," + 
						oEjectorRecord.m_szEjtTypeOld		+ "," + 
						oEjectorRecord.m_szEjtRemarkOld		+ 
						"\n");
	else
	{
		if (szCustomer == "ChangeLight(XM)")
		{
			cfFile.WriteString(oEjectorRecord.m_szMachineNo + "," + szDateOfReplace + "," + szTimeOfReplace + "," + 
				oEjectorRecord.m_szShiftNo + "," + oEjectorRecord.m_szReason + "," + oEjectorRecord.m_szType + "," + 
				oEjectorRecord.m_szEjectorPinCount + "," + oEjectorRecord.m_szEjectorUpLevel + "," + oEjectorRecord.m_szOperatorId + "," + 
				oEjectorRecord.m_szOperatorLogInId + "," + oEjectorRecord.m_szProductType + "," + 
				oEjectorRecord.m_szUsageType + "," + oEjectorRecord.m_szRemark + "\n");
		}
		else
		{
			cfFile.WriteString(oEjectorRecord.m_szMachineNo + "," + szDateOfReplace + "," + szTimeOfReplace + "," + oEjectorRecord.m_szShiftNo + "," + oEjectorRecord.m_szReason + "," + oEjectorRecord.m_szType + "," + oEjectorRecord.m_szEjectorPinCount 
			+ "," + oEjectorRecord.m_szEjectorUpLevel + "," + oEjectorRecord.m_szOperatorId + 
			"," + oEjectorRecord.m_szOperatorLogInId + "," + oEjectorRecord.m_szRemark + "\n");
		}
	}
	cfFile.Close();

	szReportPath = m_szRecordPath + "\\Ejector";
	if (m_szRecordPath2.GetLength() > 0)				//v4.48A4
		szReportPath = m_szRecordPath2 + "\\Ejector";
	CreateDirectory(szReportPath, NULL);

	szFilename = szReportPath + "\\" + oEjectorRecord.m_szMachineNo + "_Ejector" + szDateTime + ".csv";
	
	if (szBackupFilename != szFilename)
	{
		CopyFile(szBackupFilename, szFilename, FALSE);
	}
	
	return TRUE;
}

BOOL CToolsUsageRecord::GenearteParametersRecordFile()
{
	CString szReportPath;
	CString szBackupFilename;
	CStdioFile cfFile;
	CString szDateTime;
	CString szFilename;
	CString szDateOfReplace, szTimeOfReplace;
	CTime curTime = CTime::GetCurrentTime();

	szDateTime = curTime.Format("%Y%m");
	
	szDateOfReplace = curTime.Format("%Y-%m-%d");
	szTimeOfReplace = curTime.Format("%H:%M:%S");

	szBackupFilename = ToolsUsageRecordFileBackupPath;
	szBackupFilename = szBackupFilename + "\\" + m_oMS896ADataBlk.m_szMachineNo + "_Parameters" + szDateTime + ".csv";

	if (cfFile.Open(szBackupFilename, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate) == FALSE)
	{
		return FALSE;	
	}

	cfFile.SeekToEnd();

	if (cfFile.GetLength() == 0)
	{
		/*
		cfFile.WriteString("Date,Time,Machine No,Login ID,Cycle Time,Average Time,");

		cfFile.WriteString("Missing Die Check(On/Off),Missing Die Retry Count,Collet Jam Check(On/Off),Collet Jam Retry Count,Enable Look Forward(On/Off),");
		cfFile.WriteString("Enable Theta Correction(On/Off),Enable Post Bond Inspection(On/Off),Enable Die Pitch Check(On/Off),No Die Max. Skip Count,PR Die Max. Skip Count,");
		cfFile.WriteString("PR Learn Die Lighting(Coaxial),PR Learn Die Lighting(Ring),PR Learn Die Lighting(Side),");
		cfFile.WriteString("PR Die Pass Score,Ref. Die PR Pass Score,Pre-scan PR Die Pass Score,");
		cfFile.WriteString("Check Chip(On/Off),Check Defect(On/Off),Defect Check(Chip),Defect Check(Single Defect),Defect Check(Total),");
		
		cfFile.WriteString("Map Die Type Check(On/Off),Map Die Type Check.Die Type,");
		
		cfFile.WriteString("Grey Level Defect Type,Die Check Defect Threshold,Bad Cut Die Detection X Tolerance,Bad Cut Die Detection Y Tolerance,");
		
		cfFile.WriteString("Bond Die Pass Score,Die Rotation Check.Check Average Angle(On/Off),Die Rotation Check.Check Average Angle.Average Angle Allowed. Die Allow,");
		cfFile.WriteString("Die Rotation Check.Check Maximun Angle(On/Off),Die Rotation Check.Check Maximun Angle.Max Angle,Die Rotation Check.Check Maximun Angle.Max Die Allowed,");
		cfFile.WriteString("Die Placemen Check.Check XY Shift (On/Off),Post Bond Die Placement Check X(mil),Post Bond Die Placement Check Y(mil),Post Bond Die Placement Check Max. Die Allow,");
		cfFile.WriteString("Empty Die Check.Check Empty(On/Off),Empty Die Check.Check Empty.Max Die Allowed,");
		
		cfFile.WriteString("No. of Bin Blocks,Bin Block Die Quantities,Max. Die Quantities in Bin,Bin Block Walk Path,Realign Method,");
		cfFile.WriteString("Frame Realignment Last Die Offset X(mil),Frame Realignment Last Die Offset Y(mil),Bin Frame Realignment(On/Off),");
		cfFile.WriteString("Frame Realignment Delay(ms),Frame Realignment Retry Counts,Bin Table Vacuum(On/Off),Vacuum Delay,Vacuum Settle delay,");
		
		cfFile.WriteString("Collet Clean Limit,Ejector Up Level,Pick Position,Bond Position,Pick Level,Bond Level,");
		cfFile.WriteString("Pick Level2,Bond Level2,sync move setup.enablesync move(On/Off),sync move setup.trigger level max,sync move setup.preload max");

		*/


		cfFile.WriteString("Date,Time,Machine No,Login ID,Cycle Time,Average Time,");

		cfFile.WriteString("Missing Die Check(On/Off),Missing Die Retry Count,Collet Jam Check(On/Off),Collet Jam Retry Count,Enable Look Forward(On/Off),");
		cfFile.WriteString("Enable Theta Correction(On/Off),Enable Post Bond Inspection(On/Off),Enable Die Pitch Check(On/Off),No Die Max. Skip Count,PR Die Max. Skip Count,");
		cfFile.WriteString("PR Learn Die Lighting(Coaxial),PR Learn Die Lighting(Ring),PR Learn Die Lighting(Side),");
		//New
		cfFile.WriteString("Normal Die Wafer Pr Search Area X,Normal Die Wafer Pr Search Area Y,Reference Die Wafer Pr Search Area X,Reference Die Wafer Pr Search Area Y,");
		//
		cfFile.WriteString("PR Die Pass Score,Ref. Die PR Pass Score,Enable Pre-sacn(On/Off),Enable Adaptive Predition(On/Off),Pre-scan PR Delay,Pre-scan PR Die Pass Score,");
		cfFile.WriteString("Check Chip(On/Off),Check Defect(On/Off),Defect Check(Chip),Defect Check(Single Defect),Defect Check(Total),");
		
		cfFile.WriteString("Map Die Type Check(On/Off),Map Die Type Check.Die Type,");
		
		cfFile.WriteString("Grey Level Defect Type,Die Check Defect Threshold,Bad Cut Die Detection X Tolerance,Bad Cut Die Detection Y Tolerance,");
		
		cfFile.WriteString("Bond Die Pass Score,Die Rotation Check.Check Average Angle(On/Off),Die Rotation Check.Check Average Angle.Average Angle Allowed. Die Allow,");
		cfFile.WriteString("Die Rotation Check.Check Maximun Angle(On/Off),Die Rotation Check.Check Maximun Angle.Max Angle,Die Rotation Check.Check Maximun Angle.Max Die Allowed,");
		cfFile.WriteString("Die Placemen Check.Check XY Shift (On/Off),Post Bond Die Placement Check X(mil),Post Bond Die Placement Check Y(mil),Post Bond Die Placement Check Max. Die Allow,");
		cfFile.WriteString("Empty Die Check.Check Empty(On/Off),Empty Die Check.Check Empty.Max Die Allowed,");
		
		cfFile.WriteString("No. of Bin Blocks,Bin Block Die Quantities,Max. Die Quantities in Bin,Bin Block Walk Path,Realign Method,");
		cfFile.WriteString("Frame Realignment Last Die Offset X(mil),Frame Realignment Last Die Offset Y(mil),Bin Frame Realignment(On/Off),");
		cfFile.WriteString("Frame Realignment Delay(ms),Frame Realignment Retry Counts,Bin Table Vacuum(On/Off),Vacuum Delay,Vacuum Settle delay,");
		
		cfFile.WriteString("Collet Clean Limit,Auto Clean-Collet(On/Off),Ejector Up Level,Ejector Standby Level,Pick Position,Bond Position,Clean Collet Position,Pick Level,Bond Level,Clean Collet Level,");
		cfFile.WriteString("Pick Level2,Bond Level2,sync move setup.enablesync move(On/Off),sync move setup.trigger level max,sync move setup.preload max,");

		cfFile.WriteString("Algorithm Path,Algorithm Tour,Algorithm Mode,Grade Sorting Order,Home Die Input,Home Die Row No,Home Die Col No,Check Machine Cover(On/Off),Check Lan Connection(On/Off),Check Elavator Cover(On/Off),Buffer Min Die Count for Preload");

		cfFile.WriteString(",Adv Die Offset,Rescan,CD Prediction");
		cfFile.WriteString(",Stage1 Start,Stage1 Period,Stage2 Start,Stage2 Period,Stage3 Start,Stage3 Period");
        cfFile.WriteString(",Grid Row,Grid Col");
        cfFile.WriteString(",Pick Delay,Bond Delay,ArmPick Delay,ArmBond Delay,HeadPick Delay,HeadBond Delay");
		cfFile.WriteString(",HeadPrePick Delay,SyncPick Delay,EjectorUp Delay,EjectorDown Delay,EjReady Delay");
		cfFile.WriteString(",EjVacOff Delay,PR Delay,WTT Delay,BinTable Delay");
		cfFile.WriteString(",Auto Search Home, Pitch X Tolerance, Pitch Y Tolerance, Enable Die Pitch Check, Enable PR Check, Enable Scn Check, Use Ref Die");
		cfFile.WriteString(",Bond PR Center X, Bond PR Center Y, Bond2 Offset X, Bond2 Offset Y");
		cfFile.WriteString(",Bad Cut Detection, Ejector Cap Clean Limit,software version");
		cfFile.WriteString("\n");
	}

	cfFile.WriteString(szDateOfReplace);
	cfFile.WriteString("," + szTimeOfReplace);
	cfFile.WriteString("," + m_oMS896ADataBlk.m_szMachineNo);
	cfFile.WriteString("," + m_oMS896ADataBlk.m_szLoginID);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szCycleTime);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szAvgCycleTime);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEnableMissingDie);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szMissingDieRetryCount);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEnableColletJam);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szColletJamRetryCount);

	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szEnableLookForward);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szEnableThetaCorrection);

	cfFile.WriteString("," + m_oBondPrDataBlk.m_szEnablePostBondInspection);

	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szEnableDiePitchCheck);

	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szNoDieMaxCount);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szPRDieMaxSkipCount);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szPrLrnCoaxLightLevel);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szPrLrnRingLightLevel);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szPrLrnSideLightLevel);

	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szNormalDieSearchAreaX);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szNormalDieSearchAreaY);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szReferenceDieSearchAreaX);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szReferenceDieSearchAreaY);

	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szPrDiePassScore);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szRefDiePRPassScore);

	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szEnablePreScan);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szEnableAdaptPredict);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szPreScanPrDelay);


	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szPreScanSearchDieScore);

	//cfFile.WriteString("," + m_oWaferTableDataBlk.m_szPrescanDiePassScore);

	
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szEnableCheckChip);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szEnableCheckDefect);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szDefectCheckChipPercent);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szSingleDefectCheckPercent);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szTotalDefectCheckPercent);

	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szEnableMapDieTypeCheck);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szMapDieTypeCheckDieType);

	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szGreyLevelDefectType);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szDieCheckDefectThreshold);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szBadCutDieDetectionXTolerance);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szBadCutDieDetectionYTolerance);

	cfFile.WriteString("," + m_oBondPrDataBlk.m_szBondDiePassScore);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szEnableDieRotationAverageAngleCheck);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szDieRotationAverageAngle);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szEnableDieRotationMaxAngleCheck);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szDieRotationMaxAngle);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szDieRotationMaxAngleMaxDieAllow);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szEnableDiePlacementCheck);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szPostBondDiePlacementMaxXShift);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szPostBondDiePlacementMaxYShift);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szPostBondDiePlacementMaxDieAllow);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szEnableEmptyDieCheck);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szEmptyDieCheckMaxDieAllow);

	cfFile.WriteString("," + m_oBinTableDataBlk.m_szNoOfBinBlocks);
	cfFile.WriteString("," + m_oBinTableDataBlk.m_szBinBlockDieQuantities);
	cfFile.WriteString("," + m_oBinTableDataBlk.m_szMaxDieQuantities);
	cfFile.WriteString("," + m_oBinTableDataBlk.m_szBinBlockWalkPath);
	cfFile.WriteString("," + m_oBinTableDataBlk.m_szReAlignMethod);
	cfFile.WriteString("," + m_oBinTableDataBlk.m_szFrameReAlignLastDieOffsetX);
	cfFile.WriteString("," + m_oBinTableDataBlk.m_szFrmaeRealignLastDieOffsetY);

	cfFile.WriteString("," + m_oBinLoaderDataBlk.m_szEnableFrameRealign);
	cfFile.WriteString("," + m_oBinLoaderDataBlk.m_szFrameReAlignDelay);
	cfFile.WriteString("," + m_oBinLoaderDataBlk.m_szFrameReAlignRetryCount);
	cfFile.WriteString("," + m_oBinLoaderDataBlk.m_szEnableBinTableVacuum);
	cfFile.WriteString("," + m_oBinLoaderDataBlk.m_szBinVacuumDelay);
	cfFile.WriteString("," + m_oBinLoaderDataBlk.m_szBinVacuumSettleDelay);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szColletCleanLimit);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEnableAutoCleanCollet);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEjectorUpLevel);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEjStandbyLevel);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szPickPosition);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szBondPosition);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szCleanColletPosition);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szPickLevelZ1);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szPickLevelZ2);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szCleanColletLevel);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szBondLevelZ1);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szBondLevelZ2);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEnableSyncPick);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szSyncMoveTriggerLevel);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szSyncMovePreload);


	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szWaferMapPath);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szWaferMapTour);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szWaferMapMode);

	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szSortingMode);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szAlignOption);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szHomeDieRow);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szHomeDieCol);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEnabelCheckCoverSensor);
	cfFile.WriteString("," + m_oSafetyDataBlock.m_szEnableLanConnectionCheck);
	cfFile.WriteString("," + m_oBinLoaderDataBlk.m_szCheckElevatorCover);

	cfFile.WriteString("," + m_oBinTableDataBlk.m_szChangeGradeLimit);

	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szEnableDEBAdaptive);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szEnableAdvOffset);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szEnableRescanWafer);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szStage1Start);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szStage1Period);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szStage2Start);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szStage2Period);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szStage3Start);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szStage3Period);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szRegionGridRow);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szRegionGridCol);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szPickDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szBondDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szArmPickDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szArmBondDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szHeadPickDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szHeadBondDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szHeadPrePickDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szSyncPickDelay);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEjectorUpDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEjectorDownDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEjReadyDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szEjVacOffDelay);

	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szPRDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szWTTDelay);
	cfFile.WriteString("," + m_oBondHeadDataBlk.m_szBinTableDelay);
//ChangeLight(XM)

	if (m_oWaferTableDataBlk.m_szAutoSrchHome == "1")
		cfFile.WriteString(",On");
	else
		cfFile.WriteString(",Off");
	//cfFile.WriteString("," + m_oWaferTableDataBlk.m_szWaferID);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szPitchX_Tolerance);
	cfFile.WriteString("," + m_oWaferTableDataBlk.m_szPitchY_Tolerance);
	if (m_oWaferTableDataBlk.m_szDiePitchCheck == "1")
		cfFile.WriteString(",On");
	else
		cfFile.WriteString(",Off");
	//cfFile.WriteString("," + m_oWaferTableDataBlk.m_szDiePitchCheck);
	if ( m_oWaferTableDataBlk.m_szScnCheckByPR == "1")
		cfFile.WriteString(",On");
	else
		cfFile.WriteString(",Off");
	//cfFile.WriteString("," + m_oWaferTableDataBlk.m_szScnCheckByPR);
	if (m_oWaferTableDataBlk.m_szEnableScnCheck == "1")
		cfFile.WriteString(",On");
	else
		cfFile.WriteString(",Off");
	//cfFile.WriteString("," + m_oWaferTableDataBlk.m_szEnableScnCheck);
	if (m_oWaferTableDataBlk.m_szScnCheckIsRefDie == "1")
		cfFile.WriteString(",On");
	else
		cfFile.WriteString(",Off");
	//cfFile.WriteString("," + m_oWaferTableDataBlk.m_szScnCheckIsRefDie);

	cfFile.WriteString("," + m_oBondPrDataBlk.m_szPRCenterX);
	cfFile.WriteString("," + m_oBondPrDataBlk.m_szPRCenterX);
	cfFile.WriteString("," + m_oBinTableDataBlk.m_szBHZ2BondPosOffsetX);
	cfFile.WriteString("," + m_oBinTableDataBlk.m_szBHZ2BondPosOffsetY);

	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szBadCutDetection);
	cfFile.WriteString("," + m_oWaferPrDataBlk.m_szEjrCapCleanLimit);

	cfFile.WriteString("," + m_oMS896ADataBlk.m_szVersion);

	cfFile.WriteString("\n");

	cfFile.Close();

	szReportPath = m_szRecordPath + "\\Parameters Record";
	CreateDirectory(szReportPath, NULL);

	szFilename = szReportPath + "\\" + m_oMS896ADataBlk.m_szMachineNo + "_Parameters" + szDateTime + ".csv";

	if (szBackupFilename != szFilename)
	{
		CopyFile(szBackupFilename, szFilename, FALSE);
	}
	
	return TRUE;
}

VOID CToolsUsageRecord::SetLogItems(CString szStatus)
{
	CString szReportPath;
	CString szBackupFilename;
	CStdioFile cfFile;
	CString szDateTime;
	CString szLogDate, szLogTime;
	CString szFilename;
	
	CTime curTime = CTime::GetCurrentTime();

	szDateTime = curTime.Format("%Y%m");
	szLogDate = curTime.Format("%Y-%m-%d"); 
	szLogTime = curTime.Format("%H:%M:%S");

	szBackupFilename = ToolsUsageRecordFileBackupPath;
	szBackupFilename = szBackupFilename + "\\" + m_szMachineNo + "_LogItems" + szDateTime + ".csv";

	if (cfFile.Open(szBackupFilename, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate) == FALSE)
	{
		return;	
	}

	cfFile.SeekToEnd();
	
	if (cfFile.GetLength() == 0)
	{	
		cfFile.WriteString("Date,Time,Parameter,To,Modified By\n");
	}

	cfFile.WriteString(szLogDate + "," + szLogTime + "," +  szStatus + "," + m_szOperatorId + "\n");

	cfFile.Close();

	szReportPath = m_szRecordPath + "\\LogItems";
	CreateDirectory(szReportPath, NULL);

	szFilename = szReportPath + "\\" + m_szMachineNo + "_LogItems" + szDateTime + ".csv";
	
	if (szBackupFilename != szFilename)
	{
		CopyFile(szBackupFilename, szFilename, FALSE);
	}
	
	return;
}
