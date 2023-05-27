#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BT_BinBlk.h"
#include "BT_BinGradeData.h"
#include "Dpr_Ram.h"

#pragma once

/**************************** Class CBinBlkMain Start ****************************/

//#define MAX_RT_LIMIT	500
#ifdef NU_MOTION_MS60			//v4.58A3		//compatible with MS100Plus2/3
	#define MAX_RT_LIMIT	500
#else
	#define MAX_RT_LIMIT	500
#endif

#define CLEAR_BIN_BY_PHY_BLK		1
#define CLEAR_BIN_BY_GRADE			2
#define CLEAR_BIN_BY_LOGICAL_BLK	3

#define BT_MAX_BINBLK_NO	175	//150		//v3.83
#define BT_MAX_BINBLK_SIZE	176				//v3.83


class CBinBlkMain: public CObject
{
	public:

		/***********************************/
		/*     Constructor/Destructor      */
		/***********************************/
		CBinBlkMain();
		virtual ~CBinBlkMain();

		VOID SetBinTable(CBinTable* pBinTable);

		//CCopyBinGradeData();
		//CRemoveCopyBinGradeData();
		BOOL m_bEnableLogging;
		LONG m_lEnableLogCount;
		ULONG m_ulBondCountToGenTempFile;

		LONG _round(double val);
		BOOL IsUseBinMapBondArea(ULONG ulBlkToUse);
		/***********************************/
		/*        Get/Set functions        */
		/***********************************/
		VOID SetBinSerialDefault(BOOL bDisableBinSNRFormat,ULONG ulDefaultBinSNRFormat);
		VOID SetClearCountDefault(BOOL bDisableClearCountFormat,ULONG ulDefaultClearCountFormat);

		BOOL IsDisableBinSNRFormat();
		ULONG GetDefaultBinSNRFormat();

		BOOL IsDisableClearCountFormat();
		ULONG GetDefaultClearCountFormat();

		ULONG GetNoOfBlk();
		BOOL SetNoOfBlk(ULONG ulNoOfBlk);
		
		ULONG GetBlkPitchX();
		BOOL SetBlkPitchX(LONG lBlkPitchX);
		
		ULONG GetBlkPitchY();
		BOOL SetBlkPitchY(LONG lBlkPitchY);

		ULONG GetMsgCode();
		BOOL SetMsgCode(ULONG ulMsgCode);

		VOID SetNVRamPtr(void *pvNVRAM);

		VOID SetNVRamBinTableStart(LONG lNVRAM_BinTable_Start);

		VOID SetNVRamTempDataStart(void *pvNVRAM_HW, LONG lNVRAM_TEMPDATA_Start);	//v4.65A1
		BOOL ResetNVTempFileData();													//v4.65A1
		BOOL SetNVTempFileData(LONG lBlock, LONG lMapRow, LONG lMapCol, LONG lBinIndex);	//v4.65A1
		BOOL IsNVTempDataNeedToRestoreAtWarmStart(LONG &lBlock, LONG &lDieCount);	//v4.65A1
		BOOL RestoreNVTempFileDataAtWarmStart();									//v4.65A1

		/***********************************/
		/*     Init from file functions    */
		/***********************************/
		BOOL InitBinBlkMainData(BOOL bLoadClearCount=TRUE);
		BOOL InitBinBlkData(ULONG ulBlkId, BOOL bLoadSNR=TRUE, BOOL bCloseFile=TRUE);
		BOOL InitAllBinBlkData(BOOL bLoadSNR=TRUE);		//v4.46T15
		
		BOOL InitRandomHoleData(ULONG ulBlkId);
		BOOL SaveRandomHoleData(ULONG ulBlkId);

		BOOL LoadCurHoleDieNum(ULONG ulBlkId);
		BOOL SaveCurHoleDieNum(ULONG ulBlkId);

		BOOL InitAndLoadAllRandomHoleData();			//v4.46T15

		BOOL InitBinGradeData();
		BOOL InitOptimizeBinCountData();
		BOOL InitOptimizeBinCountRunTimeData();

		/***********************************/
		/*  Init Non-volatile RAM pointer */
		/***********************************/
		VOID InitNVBTDataPtr();

		/***********************************/
		/*  Clear NVRAM for cold-start	   */
		/***********************************/
		VOID ClearNVRAM(VOID *pvNVRAM, LONG lOffset);

		//v34.03	//PLLM/PLSG 
		/***********************************/
		/*  Bin Map fcns				   */
		/***********************************/
		INT m_nBinMap[MAX_BINMAP_SIZE_X][MAX_BINMAP_SIZE_Y];
		BOOL m_bIsBinMapLastRowDieExist;
		BOOL IsDieInBinMap(CONST LONG lRow, CONST LONG lCol);
		LONG IsDieInOSRAMBinMap(CONST LONG lRow, CONST LONG lCol, UCHAR ucGrade);
		ULONG Get1stDieIndexIn1stBMapRow(ULONG ulBlkToUse, ULONG &ulDieRow, ULONG &ulDieCol);				//v4.36	//v4.42T7
		ULONG GetLastDieIndexIn1stBMapRow(ULONG ulBlkToUse, ULONG& ulNoOfDiesIn1stRow, ULONG &ulDieRow, ULONG &ulDieCol);	//v4.06
		VOID FindLastColInRow(const LONG lRow, const LONG lDirection,
							  ULONG &ulDieCount, ULONG &ulIndex, ULONG &ulDieRow, ULONG &ulDieCol);
		VOID FindLastRowInColumn(const LONG lCol, const LONG lDirection,
								 ULONG &ulDieCount, ULONG &ulIndex, ULONG &ulDieRow, ULONG &ulDieCol);

		VOID GetRowColWithTLH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								   LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath);
		VOID GetRowColWithTRH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								   LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath);
		VOID GetRowColWithBLH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								   LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath);
		VOID GetRowColWithBRH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								   LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath);

		VOID GetRowColWithTLV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								   LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath);
		VOID GetRowColWithTRV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								   LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath);
		VOID GetRowColWithBLV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								   LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath);
		VOID GetRowColWithBRV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								   LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath);
		VOID GetRowColWithIndexInBinMap(const ULONG ulBinIndex, const ULONG ulWalkPath, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								   LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath);
		ULONG GetCurPathInBinMap(const ULONG ulRow, const ULONG ulCol);
		BOOL CreteBondedDieInBinMap(const ULONG ulBlkToUse, const ULONG ulLastIndex);
		BOOL IsBondedDieInBinMap(const ULONG ulBlkToUse, const ULONG ulRow, const ULONG ulCol);
		BOOL GetNextDieIndexInBinMap(ULONG ulBlkToUse, ULONG& ulNextIndex);
		LONG GetNextDieIndexInOSRAMBinMixMap(ULONG ulBlkToUse, ULONG& ulNextIndex);
		BOOL GetDieIndexInBinMapWithRowCol(const ULONG ulBlkToUse, const ULONG ulRow, const ULONG ulCol, ULONG &ulIndex);
		BOOL GetDieIndexInBinMap(ULONG ulBlkToUse, ULONG& ulIndex, ULONG &ulRow, ULONG &ulCol, BOOL bUpdateWalkPath=FALSE);
		BOOL GetDieIndexInBinMap2(ULONG ulBlkToUse, ULONG &ulIndex, ULONG &ulLastIndex1, ULONG &ulLastIndex2, ULONG &ulCurBinMapPath, BOOL bUpdateWalkPath = FALSE);
		ULONG GetCurRowWalkPath(const ULONG ulWalkPath, const ULONG ulRow);
		ULONG GetCurColumnWalkPath(const ULONG ulWalkPath, const ULONG ulCol);
		BOOL GrabEnableBinMap(ULONG ulBlkId);
		VOID SetEnableBinMap(ULONG ulBlkId);
		BOOL SetUseBinMapBondArea(BOOL bEnable);	
		BOOL SetSpGradeToByPassBinMap(CONST INT ulBlkToUse, CONST UCHAR ucSpGrade);
		UCHAR GetSpGradeToByPassBinMap(CONST INT ulBlkToUse);								//v4.48A4
		BOOL SetUseBinMapOffset(ULONG ulBlkId, BOOL bEnable, LONG lRowOffset, LONG lColOffset);	//v4.42T6
		BOOL SaveAllBinMapData();
		BOOL GetBinMapOffset(ULONG ulBlkId, LONG& lRowOffset, LONG& lColOffset);		//v4.42T6
		VOID UpdateBinMapFile();
		ULONG GrabBinMapWalkPath(ULONG ulBlkId);
		VOID SetBinMapCircleRadius(ULONG ulBlkId, DOUBLE dRadiusInUm);	//v4.42T11
		VOID SetBinMapEdgeSize(ULONG ulBlkId, LONG lTEdgeSize, LONG lBEdgeSize, LONG lLEdgeSize, LONG lREdgeSize);
		BOOL DrawBinMap(ULONG ulBlkId);					//Nichia//v4.43T7
		ULONG CalclateBinMapCapacity(ULONG ulBlkId);	//Nichia//v4.43T7
		UCHAR m_ucSpGrade1ToByPassBinMap;
		
		//OSRAM Mix Bin //4.48 Matt
		UCHAR m_ucBinMixTypeAGrade;
		UCHAR m_ucBinMixTypeBGrade;
		UCHAR m_ucBinMixCurrentGrade;
		BOOL  m_bEnableOsramBinMixMap;
		LONG  m_ulBinMixPatternType;
		BOOL  SetOSRAMBinMixA(CONST INT nIndex, CONST UCHAR ucBinMixA);
		BOOL  SetOSRAMBinMixB(CONST INT nIndex, CONST UCHAR ucBinMixB);
		BOOL  EnableOSRAMBinMixMap(CONST INT nIndex, CONST BOOL bEnableOSRAMBinMixMap);
		BOOL  SetBinMixPatternType(CONST INT nIndex, CONST LONG ulBinMixPatternType);
		/***********************************/
		/*     Save to file functions      */   
		/***********************************/		
		BOOL SaveAllBinBlkSetupData(ULONG ulNoOfBlk, LONG lBlkPitchX, LONG lBlkPitchY, BOOL bUpdateGradeData);
		BOOL SaveSingleBinBlkSetupData(ULONG ulNoOfBlk, ULONG ulBlkToSetup, UCHAR ucBlkOrgGrade, BOOL bUpdateGradeData);
		LONG SaveBinBlkDataAsTemplate(CString szTemplateName, LONG lBinBlkULX, LONG lBinBlkULY,
										   LONG lBinBlkLRX, LONG lBinBlkLRY, CString szWalkPath, 
										   ULONG ulDiePerRow, ULONG ulDiePerCol,
										   DOUBLE dDiePitchX, DOUBLE dDiePitchY,
										   ULONG ulSkipUnit, ULONG ulMaxUnit,
										   ULONG ulInputCount, BOOL bEnableFirstRowColSkipPattern,
										   ULONG ulFirstRowColSkipUnit, BOOL bCentralizedBondArea,
										   BOOL bTeachWithPhysicalBlk, BOOL bUseBlockCornerAsFirstDiePos, BOOL bEnableWafflePad,
										   LONG lWafflePadSizeX, LONG lWafflePadSizeY, LONG	lWafflePadDistX,
										   LONG	lWafflePadDistY);
		
		BOOL LoadBinBlkTemplate(CString szTemplateName, LONG& lBinBlkULX, LONG& lBinBlkULY,
										   LONG& lBinBlkLRX, LONG& lBinBlkLRY, CString& szWalkPath,
										   ULONG& ulDiePerRow, ULONG& ulDiePerCol,
										   DOUBLE& dDiePitchX, DOUBLE& dDiePitchY,
										   ULONG& ulSkipUnit, ULONG& ulMaxUnit, 
										   ULONG& ulInputCount, BOOL& bEnableFirstRowColSkipPattern, 
										   ULONG& ulFirstRowColSkipUnit, BOOL& bCentralizedBondArea,
										   BOOL& bTeachWithPhysicalBlk, BOOL& bUseBlockCornerAsFirstDiePos,
										   BOOL& bEnableWafflePad,
										   LONG& lWafflePadSizeX, LONG& lWafflePadSizeY, LONG&	lWafflePadDistX,
										   LONG& lWafflePadDistY);

		BOOL RemoveBinBlkTemplate(CString szTemplateName);

		/***********************************/
		/*      BinTable Map Display       */
		/***********************************/
		ULONG ConvertUpperLeftX(LONG lBTXNegLimit, ULONG ulBTXOffset, 
					ULONG ulBlkId, LONG lDisplayResol);
		ULONG ConvertUpperLeftY(LONG lBTYNegLimit, ULONG ulBTYOffset, 
					ULONG ulBlkId, LONG lDisplayResol);
		ULONG ConvertLowerRightX(LONG lBTXNegLimit, ULONG ulBTXOffset, 
					ULONG ulBlkId, LONG lDisplayResol);
		ULONG ConvertLowerRightY(LONG lBTYNegLimit, ULONG ulBTYOffset, 
					ULONG ulBlkId, LONG lDisplayResol);

		/**********************************/
		/*         Bin Block Setup        */
		/**********************************/
		BOOL ValidateAllSetupUL(LONG lPhyBlk1ULX, LONG lPhyBlk1ULY, LONG lPhyBlk1LRX, 
								LONG lPhyBlk1LRY, LONG lBinBlkULX, LONG lBinBlkULY);

		LONG ValidateAllSetupLR(LONG lPhyBlk1ULX, LONG lPhyBlk1ULY, LONG lPhyBlk1LRX,
								LONG lPhyBlk1LRY, LONG lBinBlkULX, LONG lBinBlkULY,
								LONG lBinBlkLRX, LONG lBinBlkLRY);

		BOOL SetupAllBlks(CPhyBlkMain* cpPhyBlkMain, ULONG ulNoOfBinBlk, 
							 LONG lBinBlk1ULX, LONG lBinBlk1ULY,
							 LONG lBinBlk1LRX, LONG lBinBlk1LRY,
							 LONG lBinBlkPitchX, LONG lBinBlkPitchY, 
							 DOUBLE dDiePitchX, DOUBLE dDiePitchY,
							 ULONG ulWalkPath, BOOL bAutoAssignGrade,
							 BOOL bAssignSameGrade,UCHAR ucStartingGrade, 
							 ULONG ulSkipUnit,ULONG ulMaxUnit, 
							 BOOL bEnableFirstRowColSkipPattern, ULONG ulFirstRowColSkipUnit, 
							 BOOL bCentralizedBondArea, BOOL bTeachWithPhysicalBlk, BOOL bUpdateGradeData,
							 BOOL bEnableWafflePad, LONG lWafflePadDistX, 
							 LONG lWafflePadDistY, LONG lWafflePadSizeX, LONG lWafflePadSizeY,
							 BOOL bEnableCircularBinArea=FALSE, LONG lRadius=0, 
							 BOOL bUseBlockCornerAsFirstDiePos=FALSE,						//v3.71T10
							 BOOL bNoReturnTravel=FALSE,									//v4.42T1
							 BOOL bUsePt5DiePitchX=FALSE, BOOL bUsePt5DiePitchY=FALSE,
							 DOUBLE dBondAreaOffsetX = 0, DOUBLE dBondAreaOffsetY = 0);

		BOOL SetAllULsAndLRs(CPhyBlkMain* cpPhyBlkMain, ULONG ulNoOfBinBlk, 
							 LONG lBinBlk1ULX, LONG lBinBlk1ULY,
							 LONG lBinBlk1LRX, LONG lBinBlk1LRY,
							 LONG lBinBlkPitchX, LONG lBinBlkPitchY);

		LONG ValidateSingleSetupUL(CPhyBlkMain* cpPhyBlkMain, ULONG ulBinBlkToSetup,
									LONG lBinBlkULX, LONG lBinBlkULY);

		LONG ValidateSingleSetupUL_BL(CPhyBlkMain* cpPhyBlkMain, ULONG ulBinBlkToSetup, 
										LONG lBinBlkULX, LONG lBinBlkULY, ULONG ulSourcePhyBlk);

		LONG ValidateSingleSetupLR(CPhyBlkMain* cpPhyBlkMain, ULONG ulBinBlkToSetup,
									ULONG ulPhyBlkId, LONG lBinBlkULX, LONG lBinBlkULY, 
									LONG lBinBlkLRX, LONG lBinBlkLRY);

		LONG ValidateSingleSetupLR_BL(CPhyBlkMain* cpPhyBlkMain, ULONG ulBinBlkToSetup, 
										LONG lBinBlkULX, LONG lBinBlkULY,
										LONG lBinBlkLRX, LONG lBinBlkLRY, 
										ULONG ulSourcePhyBlk);

		LONG IfSameGradeMerge(ULONG ulBinBlkId, LONG lPhyBlkULY, LONG lPhyBlkLRY);

		BOOL CheckAllBlockSize(ULONG ulBinBlkId);

		BOOL SetupSingleBlk(ULONG ulNoOfBlk, ULONG ulBlkToSetup, LONG lUpperLeftX,
								LONG lUpperLeftY, LONG lLowerRightX, LONG lLowerRightY,
								UCHAR ucGrade, ULONG ulWalkPath, 
								DOUBLE dDiePitchX, DOUBLE dDiePitchY, 
								ULONG ulDiePerRow, ULONG ulPhyBlkId,
								LONG lPhyBlkULY, LONG lPhyBlkLRY, ULONG ulSkipUnit,
								ULONG ulMaxUnit, BOOL bResetBinSerial,
								BOOL bEnableFirstRowColSkipPattern, ULONG ulFirstRowColSkipUnit,
								BOOL bCentralizedBondArea, BOOL bTeachWithPhysicalBlk, BOOL bUpdateGradeData,
								BOOL bUseBlockCornerAsFirstDiePos, BOOL	bEnableWafflePad, LONG lWafflePadDistX, 
								LONG lWafflePadDistY, LONG lWafflePadSizeX, LONG lWafflePadSizeY,
								BOOL bNoReturnTravel, BOOL bUsePt5PitchX, BOOL bUsePt5PitchY,
								DOUBLE dBondAreaOffsetX, DOUBLE dBondAreaOffsetY);								

		//v4.42T11	//Nichia
		BOOL SetupCircularBlkByArrCode(ULONG ulBinBlkID, LONG lULX, LONG lULY, LONG lLRX, LONG lLRY,
									 DOUBLE dAreaXInUm, DOUBLE dAreaYInUm,
									 UCHAR ucStartPos, ULONG ulMaxLoad, LONG lBinPitchX, LONG lBinPitchY,
									 BOOL bReturnTravel);
		BOOL SetupSingleBlkByArrCode(ULONG ulBinBlkID, LONG lPhyULX, LONG lPhyULY, LONG lPhyLRX, LONG lPhyLRY,
									 UCHAR ucType, DOUBLE dAreaXInUm, DOUBLE dAreaYInUm,
									 UCHAR ucStartPos, ULONG ulMaxLoad, LONG lBinPitchX, LONG lBinPitchY,
									 BOOL bReturnTravel);

		BOOL EditBlkInMerge(ULONG ulBinBlkToEdit, UCHAR ucInputGrade, LONG lInputDiePitchX,
							LONG lInputDiePitchY, ULONG ulInputWalkPath, ULONG ulSkipUnit,
							ULONG ulMaxUnit);

		BOOL Edit1stBlkInMerge(ULONG ulBinBlkToEdit, UCHAR ucInputGrade, LONG lInputDiePitchX,
							LONG lInputDiePitchY, ULONG ulInputWalkPath);

		BOOL ReassignBlkGrade(ULONG ulBlkToAssign, UCHAR ucGrade, BOOL bInitial, BOOL bUpdateGradeData);
		BOOL SetUserRowCol(ULONG ulBlkId, ULONG ulDiePerUserRow, ULONG ulDiePerUserCol);
		BOOL SwapBinBlksGrade(ULONG ulBlkId, UCHAR ucTargetedGrade);

		/***********************************/
		/*    Get Bond X, Y Position       */
		/***********************************/
		ULONG FindBondXYPosn(UCHAR ucGrade, DOUBLE& dXPosn, DOUBLE& dYPosn, LONG& lRow, LONG& lCol, LONG& lSortDir);

		BOOL BondOK(ULONG ulBondedBlkId, LONG lWaferMapX, LONG lWaferMapY, 
							LONG lWafEncX = 0, LONG lWafEncY = 0, LONG lBtEncX = 0, LONG lBtEncY = 0);

		BOOL SaveDieInfoToFile(UCHAR ucGrade, ULONG ulBondedBlk, 
								LONG lBTXInDisplayUnit, LONG lBTYInDisplayUnit, 
								LONG lBinTableX, LONG lBinTableY,
								LONG lWaferX, LONG lWaferY);
		
		/*******************************/
		/*          Post-bond          */
		/*******************************/
		BOOL PostBondOK(ULONG ulBinBlkId, ULONG ulIndex, DOUBLE dDieAngle, BOOL bIsGoodDie,
			UCHAR ucDieType);

		/*******************************/
		/*     Clear Block Setting     */
		/*******************************/
		VOID ClrSingleBlkSettings(ULONG ulBlkId);
		VOID ClrAllBlksSettings(BOOL bResetSerial = TRUE);
		VOID ClrBlkSettingsGivenPhyBlkId(ULONG ulPhyBlkId);
		VOID ClrAllGradeInfo();

		/************************************************/
		/*    Clear Bin Counter (by Physical Block)     */
		/************************************************/
		BOOL ClearBinCounterByPhyBlk(ULONG ulBlkToClear, CStringMapFile *pBTMSDfile=NULL);	//v4.52A8
		
		BOOL SaveClearBinCountDataForOutputFileByBlkId(ULONG ulBlkId, BOOL bIfGenOutputFile, BOOL bUseOutputBarcode,
								    CString szMachineNo, CString szLotNo, CString szBinLotDirectory, CString szOutputFileSummaryPath,
									CString szFileNo, CString szBlueTapeNo, 
									CString szMapSpecVersion, CString szBinParameter, 
									CString szOutputFileNamingFormat,
									CString szOutputFilename, CString szUnloadDate, 
									CString szUnloadTime, CString szOutputBarcode,
									CString szLabelFilePath, 
									CString szWorkNo, CString szPrefix, CString szLoadPKGCount,
									CString szType, CString szSortBin, CString szDevice, 
									CString szWaferBin, CString szModelNo, CString szLotID,
									CString szProduct, CString szBTSpecVersion, CString szMachineType,
									CString szSlotID, CString szMapLotNo, CString szOperatorId, CString szBTOrientation,
									CString szaExtraBinInfoField[], CString szaExtraBinInfo[],
									CString szSoftwareVersion, CString szPKGFilename,
									CString szProberMachineNo, CString szProberDateTime,
									CString szBinTableSerialNo, CString szMESOutgoingPath,
									CString szInternalProductNo, CString szMode, CString szSpecification, CString szMapOther,
									CString szSortBinFilename, CString szWaferLotFilename, CString szMapBinTable, 
									CString szDuplicateFileRetryCount, CString szMapESDVoltage,
									CString szTestTime,CString szWO, CString szSubstarteID,CString szCSTID,
									CString szRecipeID,CString szRecipeName,CString szMapFileOperator, 
									CString szMapFileDate, CString szRemark1, CString szRemark2,
									CString szPartNo, CString szBinDieType, LONG lBinBarcodeMinDigit,
									CString szDN, CString szWN, CString szCM1, CString szCM2, CString szCM3,
									CString szUN, CString szCOND, CString szLO, CString szUP, CString szFT, CString szLIM,
									CString szCOEF1, CString szCOEF2, CString szCOEF3, CString szPLLMDieFabMESLot, CString szFullMagSlot, 
									CString szLabelPrinterSel, CString szMapSerialNumber, CString szRetryCount, CString szWaferMapCenterPoint,
									CString szWaferMapLifeTime, CString szWaferMapESD, CString szWaferMapPadColor, CString szWaferMapBinCode,
									CString szMachineModel, CString szPRDeviceID, CString szUseBarcode, CString szOutputFiePath2, BOOL bForSummary,
									CStringMapFile* pSMapFile=NULL);	//v4.13T1
									

		// ---------------------------------------------------------//	
		//  A Controller to decide which block the data will store to //
		//  In CLEAR_BIN_BY_PHY_BLK Mode , ucGrade will be ignored //
		//  In CLEAR_BIN_BY_GRADE Mode, ulBlkId will be ignored    //
		// ---------------------------------------------------------//
		BOOL SaveClearBinCountDataForOutputFile(ULONG ulBlkId, UCHAR ucGrade, BOOL bIfGenOutputFile, BOOL bUseOutputBarcode, 
			CString szMachineNo, CString szLotNo, CString szBinLotDirectory, CString szOutputFileSummaryPath, CString szFileNo, 
			CString szBlueTapeNo, CString szMapSpecVersion, CString szBinParameter, CString szOutputFileNamingFormat,
			CString szOutputFilename, CString szUnloadDate, CString szUnloadTime, 
			CString szOutputBarcode, CString szLabelFilePath,
			CString szWorkNo, CString szPrefix, CString szLoadPKGCount,
			CString szType, CString szSortBin, CString szDevice, 
			CString szWaferBin, CString szModelNo, CString szLotID,
			CString szProduct, CString szBTSpecVersion, CString szMachineType,
			CString szSlotID, CString szMapLotNo, CString szOperatorId, ULONG ulModem,
			CString szaExtraBinInfoField[],CString szaExtraBinInfo[],CString szSoftwareVersion,
			CString szPKGFilename, CString szProberMachineNo, CString szProberDateTime, 
			CString szBinTableSerialNo, CString szMESOutgoingPath,
			CString szInternalProductNo, CString szMode, CString szSpecification, CString szMapOther,
			CString szSortBinFilename, CString szWaferLotFilename, CString szMapBinTable, 
			CString szDuplicateFileRetryCount, CString szMapESDVoltage,
			CString szTestTime,CString szWO, CString szSubstarteID,CString szCSTID,
			CString szRecipeID,CString szRecipeName, CString szMapFileOperator, 
			CString szMapFileDate, CString szRemark1, CString szRemark2,
			CString szPartNo, CString szBinDieType, LONG lBinBarcodeMinDigit,
			CString szDN, CString szWN, CString szCM1, CString szCM2, CString szCM3,
			CString szUN, CString szCOND, CString szLO, CString szUP, CString szFT, CString szLIM,
			CString szCOEF1, CString szCOEF2, CString szCOEF3, CString szPLLMDieFabMESLot, CString szFullMagSlot, 
			CString szLabelPrinterSel, CString szMapSerialNumber, CString szRetryCount,  CString szWaferMapCenterPoint,
			CString szWaferMapLifeTime, CString szWaferMapESD, CString szWaferMapPadColor, CString szWaferMapBinCode,
			CString szMachineModel, CString szPrDeviceID, CString szUseBarcode, CString szOutputFiePath2, BOOL bForSummary,
			CStringMapFile* pSMapFile=NULL);	//v4.13T1
			
		/***************************************/
		/*    Clear Bin Counter (by Grade)     */
		/***************************************/
		BOOL ClearBinCounterByGrade(UCHAR ucGradeToClear, CDWordArray &dwaBinBlkIds);

		/**************************/
		/*    Clear Bin Counter   */
		/**************************/
		VOID ClearBinCounter(ULONG ulBlkId, BOOL bUpdateMSD=TRUE);		//v4.52A8

		BOOL IsBondingInProgress();

		/**********************************/
		/*     Clear All Bin Counter      */
		/**********************************/
		BOOL UpdateBlkInUse(ULONG ulCurrBlkId);

		/****************************************/
		/*     Clear All Grade & Bin Counter    */
		/****************************************/
		VOID ClearAllGradeAndBinCounter();

		/*********************************/
		/*     Clear Bin Count Times     */
		/*********************************/
		VOID UpdateSerialNoGivenBinBlkId(ULONG ulBinBlkId, CStringMapFile *pBTMSDfile=NULL);	//v4.52A8	//Added pBTFile

		//----------------------------------------------//
		//		Function for Multiple Serial			//
		//		i.e different output file format		//
		//		can have different serial counter		//
		//---------------------------------------------	//
		VOID UpdateMultipleBinSerialNo(LONG nIndex);
		VOID ResetMultipleBinSerialNo(LONG nIndex);

		// Update Serial No For Each Bin Blk
		VOID UpdateSerialNo(ULONG ulPhyBlkId,UCHAR ucGrade, ULONG ulMode,
			BOOL bUserMultiSerialCounter, CONST BOOL bUseUSerialNo=FALSE);		//v3.23T1

		VOID ResetSerialNo(ULONG ulBinBlkId, CStringMapFile *pBTMSDfile=NULL);	//v4.52A8	//Speedup OPEN/CLOSE MSD file access

		/**********************************/
		/*            Step Move           */
		/**********************************/
		BOOL StepMove(ULONG ulBlkId, LONG lIndex, LONG& lXPosn, LONG& lYPosn, BOOL bLog=FALSE);
		BOOL StepDMove(ULONG ulBlkId, LONG lIndex, DOUBLE& dXPosn, DOUBLE& dYPosn, BOOL bLog = FALSE);	//v4.59A19		//MS90 with 0.1um encoder
		BOOL SubStepDMove(ULONG ulBlkId, ULONG ulNewIndex, DOUBLE &dXPosn, DOUBLE &dYPosn, BOOL bLog = FALSE);
		BOOL GetIndexMovePosnXY(ULONG ulBlkId, LONG lIndex, DOUBLE& dXPosn, DOUBLE& dYPosn);	//v4.36	//MS109 PLLM	//v4.57A7
		BOOL UpdateCurrBondIndex(ULONG ulBlkId, ULONG ulNewIndex);
		VOID UpdateLastFileSaveIndex(ULONG ulBlkId, ULONG ulNewIndex);

		/**********************************/
		/*        Grade Information       */
		/**********************************/
		BOOL SaveGradeInfo(BOOL bEnableAliasGrade, UCHAR ucGrade, ULONG ulInputCount, 
							UCHAR ucAliasGrade,
							CStringMapFile *pBTMSDfile=NULL);	//v4.52A8	//Reduce OPEN/CLOSE BT MSD file access

		/************************************/
		/*   Assist Physical Block Setup    */
		/************************************/
		BOOL IfDuringBond(ULONG ulPhyBlkId);
		BOOL IfContainBinBlk(ULONG ulPhyBlkId);

		/************************************/
		/*      Generate Output File        */
		/************************************/
		LONG GenAllTempFiles(BOOL bUseNVData=FALSE, BOOL bBackupTempFile = FALSE);
		BOOL BackupTempFile(ULONG ulBlkId);
		BOOL BackupAllTempFiles();
		
		// ----------------------------------------------------------//	
		//  A Controller to decide which block will Gen Output File  //
		//  In CLEAR_BIN_BY_PHY_BLK Mode , ucGrade will be ignored  //
		//  In CLEAR_BIN_BY_GRADE Mode, ulBlkId will be ignored    //
		// ---------------------------------------------------------//
		LONG GenOutputFileDatabase(ULONG ulBlkId, UCHAR ucGrade, CStringArray &szaPopUpMsg, ULONG ulMode);
		LONG GenOutputFileDatabaseByBlkId(ULONG ulBlkId, CStringArray &szaPopUpMsg);
		LONG CleanupGenAndQueryDB(ULONG ulBlkId, UCHAR ucGrade, ULONG ulMode);

		VOID SetCountByGrade(UCHAR ucGrade, ULONG ulGradeCounter);
		ULONG GetCountByGrade(UCHAR ucGrade);
		ULONG GetBlock(UCHAR ucGrade);
		// ----------------------------------------------------------//	
		//  A Controller to decide which block will Gen Output File  //
		//  In CLEAR_BIN_BY_PHY_BLK Mode , ucGrade will be ignored  //
		//  In CLEAR_BIN_BY_GRADE Mode, ulBlkId will be ignored    //
		// ---------------------------------------------------------//
		LONG GenStandardOutputFile(ULONG ulBlkId, UCHAR ucGrade, CString szOutputFilePath, 
								   CString szBinOutputFileFormat, BOOL bPrintLabel,
								   CStringArray &szaPopUpMsg, ULONG ulMode);
		LONG GenStandardOutputFileByBlkId(ULONG ulBlkId,CString szOutputFilePath,
										CString szBinOutputFileFormat, BOOL bPrintLabel, CStringArray &szaPopUpMsg);

		// ----------------------------------------------------------//	
		//  A Controller to decide which block will remove Temp File //
		//  In CLEAR_BIN_BY_PHY_BLK Mode , ucGrade will be ignored  //
		//  In CLEAR_BIN_BY_GRADE Mode, ulBlkId will be ignored    //
		// ---------------------------------------------------------//
		VOID RemoveTempFileAndResetIndex(ULONG ulBlkId, UCHAR ucGrade, ULONG ulMode);
		VOID RemoveTempFileAndResetIndexByBlkId(ULONG ulBlkId);

		// ----------------------------------------------------------//	
		//  A Controller to decide which block will Backup Temp File,// 
		//  Temp File, Header file,etc								//
		//  In CLEAR_BIN_BY_PHY_BLK Mode , ucGrade will be ignored  //
		//  In CLEAR_BIN_BY_GRADE Mode, ulBlkId will be ignored    //
		// ---------------------------------------------------------//
		BOOL BackupClearBinFiles(ULONG ulBlkId, UCHAR ucGrade, BOOL nEnable, CString szTimeStamp, ULONG ulMode);
		BOOL BackupClearBinFilesByBlkId(ULONG ulBlkId, BOOL nEnable, CString szTimeStamp, 
										BOOL bJsutTempFile = FALSE);
		
		/***********************************/
		/*     Generate Wafer End File     */
		/***********************************/
		BOOL GenBinBlkFile(LONG &lPickTotal, LONG &MapTotal);
		BOOL GenBinBlkFile_Finisar();

		/*******************************************/
		/*     Generate Bin Summary Output File    */
		/*******************************************/
		VOID SetBinSummaryGenTime(CString szGenTime[]);
		VOID SetAutoGenSummaryFile(BOOL bAutoGenFile);
		VOID SetBinBlkSummaryPeriodNum(LONG lPeriodNum);
		VOID SetLastGenSummaryTime(CString szLastGenTime);
		VOID UpdateLastGenSummaryTime();
		CString GetLastGenSummaryTime();
		BOOL ConvertStringToDate(CString szDate, INT& nYear, INT& nMonth, INT& nDay);
		BOOL ConvertStringToTime(CString szTime, INT& nHour, INT& nMin);
		BOOL CheckAutoGenerateBinSummary();
		BOOL AutoGenBinSummaryFile(CString szBinSummaryOutputPath, CString szMachineNo);
		VOID GenBinSummaryOutputFileName(CString szBinSummaryOutputPath, CString szMachineNo);
		BOOL GenBinSummaryOutputFile(CString szBinSummaryOutputPath, CString szMachineNo);
		VOID SetBackupTempFilePath(CString szPath);		//v4.48A10	//WH Sanan

		/************************************/
		/*    Grab CBinBlk Class members    */
		/************************************/
		BOOL GrabIsSetup(ULONG ulBlkId);
		UCHAR GrabStatus(ULONG ulBlkId);

		LONG GrabBlkUpperLeftX(ULONG ulBlkId);
		LONG GrabBlkUpperLeftY(ULONG ulBlkId);
		LONG GrabBlkLowerRightX(ULONG ulBlkId);
		LONG GrabBlkLowerRightY(ULONG ulBlkId);

		LONG GrabBlkUpperLeftXFromSetup(ULONG ulBlkId);
		LONG GrabBlkUpperLeftYFromSetup(ULONG ulBlkId);
		LONG GrabBlkLowerRightXFromSetup(ULONG ulBlkId);
		LONG GrabBlkLowerRightYFromSetup(ULONG ulBlkId);

		UCHAR GrabGrade(ULONG ulBlkId);
		UCHAR GrabOriginalGrade(ULONG ulBlkId);

		DOUBLE GrabDDiePitchX(ULONG ulBlkId);
		DOUBLE GrabDDiePitchY(ULONG ulBlkId);
		BOOL SetDDiePitchX(ULONG ulBlkId, DOUBLE dDiePitchX);
		BOOL SetDDiePitchY(ULONG ulBlkId, DOUBLE dDiePitchY);
		BOOL GrabUsePt5UmInDiePitchX(ULONG ulBlkId);	//v4.42T8
		BOOL GrabUsePt5UmInDiePitchY(ULONG ulBlkId);	//v4.42T8
		VOID SetUsePt5UmInDiePitchX(ULONG ulBlkId, BOOL bEnable);	//v4.42T8
		VOID SetUsePt5UmInDiePitchY(ULONG ulBlkId, BOOL bEnable);	//v4.42T8
		DOUBLE GrabBondAreaOffsetX(ULONG ulBlkId);
		DOUBLE GrabBondAreaOffsetY(ULONG ulBlkId);
		VOID SetBondAreaOffset(ULONG ulBlkId, DOUBLE dOffsetXInFileUnit, DOUBLE dOffsetYInFileUnit);
		//v4.48A11	//SEmitek, 3E DL
		BOOL GrabUseByPassBinMap(ULONG ulBlkId);
		VOID SetUseByPassBinMap(ULONG ulBlkId, BOOL bEnable);

		ULONG GrabOriginalWalkPath(ULONG ulBlkId);
		ULONG GrabWalkPath(ULONG ulBlkId);
		ULONG GrabWalkPath(ULONG ulBlkId, const ULONG ulCurWalkPath);

		ULONG GrabNoOfDiePerBlk(ULONG ulBlkId);
		ULONG GrabNoOfDiePerRow(ULONG ulBlkId);
		ULONG GrabNoOfDiePerCol(ULONG ulBlkId);

		BOOL SetNoOfDiePerBlk(ULONG ulBlkId, ULONG ulDiePerBlk);
		BOOL SetNoOfDiePerRow(ULONG ulBlkId, ULONG ulDiePerRow);
		BOOL SetNoOfDiePerCol(ULONG ulBlkId, ULONG ulDiePerCol);

		BOOL GrabIsDisableFromSameGradeMerge(ULONG ulBlkId);

		ULONG GrabPhyBlkId(ULONG ulBlkId);

		ULONG GrabSerialNo(ULONG ulBlkId);

		ULONG GrabSkipUnit(ULONG ulBlkId);

		ULONG GrabMaxUnit(ULONG ulBlkId);

		ULONG GrabNVNoOfBondedDie(ULONG ulBlkId);
		ULONG GrabNVCurrBondIndex(ULONG ulBlkId);
		ULONG GrabNVLastFileSaveIndex(ULONG ulBlkId);
		ULONG GrabBondRowColGivenIndex(ULONG ulBlkId, ULONG Index, LONG &lRow, LONG &lCol);
		ULONG GrabOriginalIndex(ULONG ulBlkId, ULONG Index);
		BOOL GrabNVIsFull(ULONG ulBlkId);

		BOOL GrabIsTeachWithPhysicalBlk(ULONG ulBlkId);

		BOOL GrabIsCentralizedBondArea(ULONG ulBlkId);
		LONG GrabCentralizedOffsetX(ULONG ulBlkId);
		LONG GrabCentralizedOffsetY(ULONG ulBlkId);

		BOOL GrabIsUseBlockCornerAsFirstDiePos(ULONG ulBlkId);
		BOOL GrabNoReturnTravel(ULONG ulBlkId);
		
		BOOL GrabIsEnableWafflePad(ULONG ulBlkId);
		BOOL GrabWafflePadSizeX(ULONG ulBlkId);
		BOOL GrabWafflePadSizeY(ULONG ulBlkId);
		BOOL GrabWafflePadDistX(ULONG ulBlkId);
		BOOL GrabWafflePadDistY(ULONG ulBlkId);
		//v4.59A22
		DOUBLE GrabThermalDeltaPitchX(ULONG ulBlkId);					
		DOUBLE GrabThermalDeltaPitchY(ULONG ulBlkId);		
		DOUBLE FindThermalDeltaPitchX(ULONG ulBlkId, LONG lCurrCol);
		DOUBLE FindThermalDeltaPitchY(ULONG ulBlkId, LONG lCurrRow);
		VOID SetThermalDeltaPitch(ULONG ulBlkId, DOUBLE dX, DOUBLE dY);

		//For re-align bin block
		BOOL GrabNVIsAligned(ULONG ulBlkId);
		LONG GrabNVXOffset(ULONG ulBlkId);
		LONG GrabNVYOffset(ULONG ulBlkId);
		DOUBLE GrabNVRotateAngleX(ULONG ulBlkId);

		VOID SetIsAligned(ULONG ulBlkId, BOOL bIsAligned);
		VOID SetXOffset(ULONG ulBlkId, LONG lXOffset);
		VOID SetYOffset(ULONG ulBlkId, LONG lYOffset);
		VOID SetRotateAngleX(ULONG ulBlkId, DOUBLE dRotateAngleX);

		//v3.71T1	//PLLM REBEL
		BOOL GrabUseCircularArea(ULONG ulBlkId);
		LONG GrabCircularBinRadius(ULONG ulBlkId);
		LONG GrabCircularBinCenterX(ULONG ulBlkId);
		LONG GrabCircularBinCenterY(ULONG ulBlkId);
		BOOL SetUseCircularBinArea(ULONG ulBlkId, BOOL bEnable, LONG lCenterX, LONG lCenterY, LONG lRadius);	//v3.71T1

		UCHAR GrabCurrSortDirection(ULONG ulBlkId);		//v3.62

		//v4.30T3	//CyOptics
		VOID SetLeaveEmptyRowArray(BOOL bSet);
		BOOL CheckIfEmptyRow(ULONG ulBlkId);
		VOID SetLeaveEmptyRow(ULONG ulBlkId, BOOL bSet);
		ULONG GetEmptyRow1stRowIndex(ULONG ulBlkId);		//v4.40T2
		VOID ResetEmptyRow1stRowIndex(ULONG ulBlkId);		//v4.40T2
		BOOL CheckIfNeedToLeaveEmptyRow(ULONG ulBlkToUse, ULONG& ulNextIndex, BOOL bRealign=FALSE);
		BOOL SaveLeaveEmptyRowData();
	
		/*******************************************/
		/*    Grab CBinGradeData Class members     */
		/*******************************************/

		VOID SetAllGradeIsAssigned(BOOL bEnable);

		VOID SetGradeIsAssigned(UCHAR ucGrade, BOOL bEnable, CStringMapFile *pBTfile=NULL);
		BOOL GrabIsAssigned(UCHAR ucGrade);

		ULONG GrabGradeCapacity(UCHAR ucGrade);
		ULONG GrabInputCount(UCHAR ucGrade);
		UCHAR GrabAliasGrade(UCHAR ucGrade);
		ULONG GrabStatRowIndex(UCHAR ucGrade);

		BOOL UpdateSingleGradeCapacity(ULONG ulBlkToSetup, BOOL bCopyFromGrade1);

		ULONG GrabNVNoOfSortedDie(UCHAR ucGrade);
		ULONG GrabNVBlkInUse(UCHAR ucGrade);

		BOOL SetIntputCount(UCHAR ucGrade, ULONG ulInputCount);
		BOOL UpdateGradeInputCount(UCHAR ucGrade, ULONG ulInputCount);
		BOOL SetLotRemainCount(UCHAR ucGrade, ULONG ulRemainCount);

		/***********************************/
		/*    Set CBinBlk Class Members    */
		/***********************************/
		VOID SetIsBinBlkAssignedWithSlot(ULONG ulBinBlkId, BOOL bIsAssignedWithSlot);
		//VOID SetGrade(ULONG ulBlkId, UCHAR ucGrade);
		VOID SetStatus(ULONG ulBlkId, UCHAR ucStatus);

		/******************************************/
		/*    Set CBinGradeData Class members     */
		/******************************************/
		VOID SetStatRowIndexPerGrade(UCHAR ucGrade, ULONG ulStatRowIndex);

		/******************************************/
		/*    Check CBinGradeData Class members   */
		/******************************************/
		BOOL IsLastPos(UCHAR ucGrade);
		BOOL CheckIfGradesAreStillValid();

		
		/******************************************/
		/*    Set & grab BlockSize				  */
		/******************************************/
		BOOL SetBlockSize(UCHAR ucGrade, CString szBlockSize);
		CString GrabBlockSize(UCHAR ucGrade);

		/******************************************/
		/*    Set & grab RankID					  */
		/******************************************/
		BOOL ResetRankID();
		BOOL SetRankID(UCHAR ucGrade, CString szRankID);
		CString GrabRankID(UCHAR ucGrade);
		BOOL CheckValidRankID(UCHAR ucGrade);			//v4.21T4	//TongFang RankID checking fcn

		/******************************************/
		/*    Set & grab Life Time				  */
		/******************************************/
		BOOL SetLifeTime(CString szLifeTime);
		CString GrabLifeTime();


		/******************************************/
		/*    Set & grab ESD					  */
		/******************************************/
		BOOL SetESD(CString szESD);
		CString GrabESD();

		/******************************************/
		/*    Set & grab IF					  */
		/******************************************/
		BOOL SetIF(CString szIF);
		CString GrabIF();

		/******************************************/
		/*    Set & grab TapeID					  */
		/******************************************/
		BOOL SetTapeID(CString szTapeID);
		CString GrabTapeID();
		
		/******************************************/
		/*    Set & grab BinTableFilename		  */
		/******************************************/
		BOOL SetBinTableFilename(CString szBinTableFilename);
		CString GrabBinTableFilename();

		/******************************************/
		/*    Set & grab SerialNoMaxLimit		  */
		/******************************************/
		BOOL SetSerialNoMaxLimit(ULONG ulMaxLimit);
		ULONG GrabSerialNoMaxLimit();


		/******************************************/
		/*    Set & grab SerialNoFormat			  */
		/******************************************/
		BOOL SetSerialNoFormat(ULONG ulSerialFormat);
		ULONG GrabSerialNoFormat();


		/******************************************/
		/*    Set & grab BinClearCountFormat	  */
		/******************************************/
		BOOL SetBinClearFormat(ULONG ulFormat);
		ULONG GrabBinClearFormat();


		/******************************************/
		/*    Set & grab BinClearInitCount & Limit*/
		/******************************************/
		BOOL SetBinClearInitCount(ULONG ulInitCount);
		BOOL SetBinClearMaxLimit(ULONG ulMaxLimit);
		ULONG GrabBinClearInitCount();
		ULONG GrabBinClearMaxLimit();


		/******************************************/
		/*    Grab & Reset BinClearedCount		  */
		/******************************************/
		BOOL ResetBinClearedCount();
		CString GrabBinClearedCount();


		/********************************************/
		/*    NVRAM RunTime Data functions			*/
		/********************************************/
		BOOL SetPSTNVRunTimeData(void *pvNVRAM, LONG lNVRAM_BinTable_Start, LONG lNVRAM_BinTable_Size);
		VOID ResetNVRunTimeData();
		LONG SaveNVRunTimeData(ULONG ulBlkId, UCHAR ucGrade, LONG lWaferX, LONG lWaferY);
		BOOL RestoreNVRunTimeData();
		BOOL RestoreNVRunTimeData_Main();
		BOOL EnableNVRunTimeData(BOOL bEnable);


		/************************************/
		/*  Set use FileName as WaferID     */
		/************************************/
		VOID SetFileNameAsWaferID(BOOL bAsWaferID);


		/*********************************************/
		/*  Set record bin accumulated bonded count  */
		/*********************************************/
		VOID SetAccCountOption(BOOL bStore);
		BOOL GrabAccCountOption();
		VOID SetBinAccBondedCount(ULONG ulBlkId, ULONG ulCount, CStringMapFile *pBTMSDfile=NULL);	//v4.52A8
		ULONG GrabBinAccBondedCount(ULONG ulBlkId);


		/****************************/
		/*  Set Auto assign grade   */
		/****************************/
		VOID SetAutoAssignGrade(BOOL bEnable);
		BOOL GrabAutoAssignGrade();


		/************************************************/
		/*  Read BinBlk barcode from SMF file (DL only) */
		/************************************************/
		CString GetBinBlkCatcode(ULONG ulBlkNo);		//v2.78T1
		BOOL ResetBinBlkCatcode();						//v2.78T2


		/*****************************/
		/*  Set/Get LS bond pattern  */
		/*****************************/
		VOID SetLSBondPattern(BOOL bEnable);
		BOOL GrabLSBondPattern();
		BOOL GrabLSBondPatternIndex(ULONG ulBlkToUse, ULONG ulCurrIndex, ULONG& ulNextIndex);			//v4.01
		BOOL RecalculateLSBondPatternIndex(ULONG ulBlkToUse, ULONG ulDieCount, ULONG& ulLSBondIndex);	//v4.35T3
		VOID SaveEmptyUnitsToMSD();

		/*****************************/
		/*  GradeMapping			 */
		/*****************************/
		BOOL GradeMapping();
		BOOL UpdateGradeMappingInBinBlk(CString& szMapFullPath);
		BOOL CheckNeedUpdateMapping(CString& szMapFullPath);
		BOOL ReadMappedGrade(CString& szMapFullPath, CMapStringToString& szaGradeMapping);

		VOID SubmitRandomHoleDieInfo(BOOL bEnable2DBarcodeOutput, ULONG ulHoleDieNum);
		VOID GenRandomHole(ULONG ulBlkToUse);
		ULONG GrabRandomHoleDieIndex(ULONG ulBlkToUse, ULONG ulIndex);
		ULONG GetCurrHoleDieNum(ULONG ulBlkToUse);
		VOID SetEmptyHoleDieNum(ULONG ulBlkToUse,ULONG ulHoleDieNum);
		ULONG TwoDimensionBarcodeGetTotalNoOfHoleOnFrame(UCHAR ucGrade);
		BOOL IsTwoDimensionBarcodeHole(ULONG ulBondedBlkId);
		BOOL TwoDimensionBarcodeFindNextBondIndex(ULONG ulBlkToUse, ULONG& ulNextIndex);
		BOOL IsNear2DBarcodeBondIndex(ULONG ulBlkToUse, ULONG ulIndex);		//v4.48A20	//For logging purpose only in AUTOBOND cycle

		BOOL IsFirstRowColSkipPatternSkipUnit(ULONG ulBondedBlkId);
		BOOL FirstRowColSkipPatternFindNextBondIndex(ULONG ulBlkToUse,ULONG& ulNextIndex);

		ULONG GetFirstRowColSkipPatternSkippedUnitCount(ULONG ulBlkToUse);
		ULONG GetFirstRowColSkipPatTotalSkipUnitCount(ULONG ulBlkToUse);
		
		BOOL GetFirstRowColSkipPattern(ULONG ulBlkId);
		ULONG GetFirstRowColSkipPatternSkipUnit(ULONG ulBlkId);

		BOOL GetIsWafflePadEnable(ULONG ulBondedBlkId);

		BOOL IsWafflePadSkipUnit(ULONG ulBondedBlkId);
		BOOL WafflePadSkipPosFindNextBondIndex(ULONG ulBlkToUse,ULONG& ulNextIndex);

		ULONG GetWafflePadSkipUnitCount(ULONG ulBlkToUse);
		ULONG GetWafflePadSkipUnitTotalCount(ULONG ulBlkToUse);

		
		/*********************************************/
		/*  load save bin blk run time data			 */
		/*********************************************/

		BOOL CreateBackupTempFileList();
		BOOL SaveBinBlkRunTimeData();
		BOOL LoadBinBlkRunTimeData();

		BOOL SaveBinGradeRunTimeData();
		BOOL LoadBinGradeRunTimeData();

		BOOL CheckOptimizeBinCountSetting(UCHAR& ucGrade);

		BOOL GenerateBinRunTimeSummary(CString szBinRunTimeSummaryFile, CString szMachineNo);

		BOOL UpdateLotRemainCountByGrade(UCHAR ucGrade);
		BOOL UpdateLotRemainCountByPhyBlk(ULONG ulBinBlk);

		ULONG GetLotRemainCount(UCHAR ucGrade);
		BOOL OptimizeBinFrameCountByGrade(UCHAR ucGrade, UCHAR ucOptimizeBinCountMode, ULONG ulOption);

		BOOL OptimizeBinFrameCountByPhyBlk(ULONG ulBinBlk, UCHAR ucOptimizeBinCountMode, ULONG ulOption);
	
		BOOL OptimizeBinFrameCount(UCHAR ucGrade, UCHAR ucOptimizeBinCountMode);
		BOOL OptimizeBinFrameCountByGrade_Luxtar(UCHAR ucGrade, UCHAR ucOptimizeBinCountMode);
		BOOL OptimizeBinFrameCountByPhyBlk_Luxtar(ULONG ulBinBlk, UCHAR ucOptimizeBinCountMode);

		ULONG GetMaxFrameDieCount(UCHAR ucGrade);
		ULONG GetMinFrameDieCount(UCHAR ucGrade);
		
		BOOL SetMinFrameDieCount(UCHAR ucGrade, ULONG ulMinFrameDieCount);
		BOOL SetMaxFrameDieCount(UCHAR ucGrade, ULONG ulMaxFrameDieCount);

		BOOL UpdateAllOptimizeMinCount(ULONG ulMinFrameDieCount);
		BOOL UpdateAllOptimizeMaxCount(ULONG ulMaxFrameDieCount);

		UCHAR GetBinMixCurrentGrade();

		VOID SetFrameRotation(ULONG ulBlkId, BOOL bEnable180Rotation, LONG lBinCalibX, LONG lBinCalibY);		//v4.49A9
		BOOL GetFrameRotation(ULONG ulBlkId);		//v4.49A9
		LONG GetBinCalibX(const ULONG ulBlkId);
		LONG GetBinCalibY(const ULONG ulBlkId);

		CString GetCustomOutputFileName()
		{
			return m_szCustomOutputFileName;
		}
	private:

		BOOL m_bDisableClearCountFormat;
		ULONG m_ulDefaultClearCountFormat;
		
		BOOL m_bDisableBinSNRFormat;
		ULONG m_ulDefaultBinSNRFormat;


		ULONG m_ulNoOfBlk;
		LONG m_lBlkPitchX, m_lBlkPitchY;
		ULONG m_ulMsgCode;

		void *m_pvNVRAM;
		LONG m_lNVRAM_BinTable_Start;
		BOOL m_bFirstTimeInitOutputFile;

		void *m_pvNVRAM_HW;					//v4.65A3
		BOOL m_bUseHWNVRam;					//v4.65A1
		LONG m_lNVRAM_TempData_Start;		//v4.65A1

		//v4.65A3
		BT_NVTEMPDATA *m_pstNvTempFileData_HW;	
		BT_NVTEMPDATA *m_pstNvTempFileData;

		CString m_szLifeTime;
		CString m_szESD;
		CString m_szIF;
		CString m_szTapeID;
		CString m_szBinTableFilename;

		ULONG m_ulSerialNoFormat;
		ULONG m_ulSerialNoMaxLimit;

		CString m_szBinClearedDate;
		ULONG m_ulBinClearFormat;
		ULONG m_ulBinClearedCount;
		ULONG m_ulBinClearInitCount;
		ULONG m_ulBinClearMaxLimit;

		BOOL m_bFilenameASWaferID;
		BOOL m_bEnableNVRunTimeData;

		BOOL m_bStoreBinBondedCount;
		BOOL m_bEnableAutoAssignGrade;
		BOOL m_bEnableLSBondPattern;

		CString m_szMapFilename;
		CString m_szBackupTempFilePath;		//v4.48A10	//WH Sanan

		//GenBinSummaryOutputFileName
		CString m_szBinSummaryOutputPath;	

		BOOL	m_bEnable2DBarcodeOutput;		//v3.33T3
		ULONG	m_ulHoleDieNum;					//v3.33T3
	
		typedef struct
		{
			UCHAR ucBlkNo;
			UCHAR ucGrade;
			SHORT ssWaferMapX;
			SHORT ssWaferMapY;
		} BT_NVRUNTIMEDATA;

		BT_NVRUNTIMEDATA *m_pstNvRunTimeData[MAX_RT_LIMIT];
		ULONG *m_pNvRunTimeCounter;

		BOOL m_bEnableBinMapBondArea;

		CString m_szCustomOutputFileName;
	protected:

		CBinTable* m_pBinTable;	

		CBinBlk m_oBinBlk[BT_MAX_BINBLK_SIZE];
		CBinGradeData m_oBinGradeData[BT_MAX_BINBLK_SIZE];

		//current number of holes generated on the bin for each bin blk		//v3.33T3
		ULONG m_ulCurrHoleDieNum[BT_MAX_BINBLK_SIZE];
		
		LONG m_lGenSummaryPeriodNum;
		CString m_szLastGenSummaryTime;
		CString m_szBinSummaryGenTime[BT_SUMMARY_TIME_NO];

		ULONG m_ulMinFrameDieCount[BT_MAX_BINBLK_SIZE];
		ULONG m_ulMaxFrameDieCount[BT_MAX_BINBLK_SIZE];

		BOOL  m_bLeaveEmptyRow[BT_MAX_BINBLK_SIZE];			//CyOptics	//v4.28T4					
		ULONG m_ul1stRowIndex[BT_MAX_BINBLK_SIZE];			//CyOptics	//v4.40T2					
	

		//CTime m_ctLastGenSummaryTime;
		//CTime m_ctBinSummaryGenTime[BT_SUMMARY_TIME_NO]; 

}; //end Class CBinBlkMain 

/***************************** Class CBinBlkMain End *****************************/
