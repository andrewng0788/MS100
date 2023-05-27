#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"

#pragma once

#define BINBLK_MAX_SIZE		176			//v3.83

#define MAX_BINMAP_SIZE_X	1020
#define MAX_BINMAP_SIZE_Y	1020

enum {BBX_FRAME_ROTATE_0 = 0, BBX_FRAME_ROTATE_90 = 1, BBX_FRAME_ROTATE_180 = 2, BBX_FRAME_ROTATE_270 = 3};

//v4.65A1
typedef struct
{
	LONG lInUse;
	LONG lBlock;
	LONG lIndex;
	LONG lWaferMapRow[500];
	LONG lWaferMapCol[500];
	LONG lBinIndex[500];

} BT_NVTEMPDATA;
/****************************** Class CBinBlk Start ******************************/

class CBinBlk: public CObject
{
public:

	/***********************************/
	/*     Constructor/Destructor      */
	/***********************************/
	CBinBlk();
	virtual ~CBinBlk();

	/***********************************/
	/*        Get/Set functions        */
	/***********************************/
	LONG _round(double val);
	LONG GetUpperLeftX();
	BOOL SetUpperLeftX(LONG lUpperLeftX);

	LONG GetUpperLeftXFromSetup();
	BOOL SetUpperLeftXFromSetup(LONG lUpperLeftXFromSetup);

	LONG GetUpperLeftY();
	BOOL SetUpperLeftY(LONG lUpperLeftY);

	LONG GetUpperLeftYFromSetup();
	BOOL SetUpperLeftYFromSetup(LONG lUpperLeftYFromSetup);

	LONG GetLowerRightX();
	BOOL SetLowerRightX(LONG lLowerRightX);

	LONG GetLowerRightXFromSetup();
	BOOL SetLowerRightXFromSetup(LONG lLowerRightXFromSetup);

	LONG GetLowerRightY();
	BOOL SetLowerRightY(LONG lLowerRightY);

	LONG GetLowerRightYFromSetup();
	BOOL SetLowerRightYFromSetup(LONG lLowerRightYFromSetup);

	ULONG GetWidth();
	BOOL SetWidth();

	ULONG GetWidthFromSetup();
	BOOL SetWidthFromSetup();

	ULONG GetHeight();
	BOOL SetHeight();

	ULONG GetHeightFromSetup();
	BOOL SetHeightFromSetup();

	UCHAR GetGrade();
	BOOL SetGrade(UCHAR ucGrade);

	UCHAR GetOriginalGrade();
	BOOL SetOriginalGrade(UCHAR ucGrade);

	//v4.59A18	//MS90 with newest 0.1um encoder
	BOOL SetDDiePitchX(DOUBLE dDiePitchX);   
	BOOL SetDDiePitchY(DOUBLE dDiePitchY);   
	DOUBLE GetDDiePitchX();
	DOUBLE GetDDiePitchY();

	//v4.42T5	//Nichia
	BOOL GetPoint5UmInDiePitchX();
	VOID SetPoint5UmInDiePitchX(CONST BOOL bEnable);
	BOOL GetPoint5UmInDiePitchY();
	VOID SetPoint5UmInDiePitchY(CONST BOOL bEnable);
	DOUBLE GetBondAreaOffsetX();
	DOUBLE GetBondAreaOffsetY();
	VOID SetBondAreaOffset(DOUBLE dOffsetX, DOUBLE dOffsetY);
	//v4.59A22	//David Ma
	DOUBLE GetThermalDeltaPitchX();
	DOUBLE GetThermalDeltaPitchY();
	VOID SetThermalDeltaPitch(DOUBLE dX, DOUBLE dY);
	DOUBLE CalculateThermalDeltaPitchX(LONG lCurrCol);
	DOUBLE CalculateThermalDeltaPitchY(LONG lCurrRow);
	//v4.48A11	//SEmitek, 3E DL
	BOOL GetByPassBinMap();
	VOID SetByPassBinMap(CONST BOOL bEnable);

	ULONG GetWalkPath();
	ULONG GetWalkPath(const ULONG ulCurWalkPath);
	ULONG GetOriginalWalkPath();	//v4.50A24
	BOOL SetWalkPath(ULONG ulWalkPath);

	//v4.49A9
	BOOL SetFrameRotation(UCHAR ucRotation, LONG lBinCalibX, LONG lBinCalibY);
	LONG GetBinCalibX();
	LONG GetBinCalibY();

	BOOL SetFrameRotationOnly(UCHAR ucRotation);
	UCHAR GetFrameRotation();								//v4.50A24
	BOOL RotateBondPos(DOUBLE& dX, DOUBLE& dY);				//v4.59A1	//v4.59A19
	BOOL RotateBlkCorners(LONG& lULX, LONG& lULY, LONG& lLRX, LONG& lLRY);
	//BOOL FrameRotateTransformPosXY(LONG& lX, LONG& lY);		//v4.50A24
	BOOL CalculateFrameRotateXY(LONG lX, LONG lY, LONG& lNewX, LONG& lNewY);

	ULONG GetDiePerRow();
	BOOL SetDiePerRow(ULONG ulNoOfDiePerRow);

	ULONG GetDiePerCol();
	BOOL SetDiePerCol(ULONG ulNoOfDiePerCol);

	ULONG GetDiePerBlk();
	BOOL SetDiePerBlk(ULONG ulNoOfDiePerBlk);

	BOOL GetIsDisableFromSameGradeMerge();
	BOOL SetIsDisableFromSameGradeMerge(BOOL bIsDisableFromSameGradeMerge);

	ULONG GetPhyBlkId();
	BOOL SetPhyBlkId(ULONG ulPhyBlkId); 

	BOOL GetIsAssignedWithSlot();
	BOOL SetIsAssignedWithSlot(BOOL bIsAssignedWithSlot);

	BOOL GetIsSetup();
	BOOL SetIsSetup(BOOL bIsSetup);

	UCHAR GetStatus();
	BOOL SetStatus(UCHAR ucStatus);

	UCHAR GetSortDirection();		//v3.62

	BOOL GetIsDatabaseGenerated();
	BOOL SetIsDatabaseGenerated(BOOL bIsDatabaseGenerated);

	ULONG GetSerialNo();
	BOOL SetSerialNo(ULONG ulSerialNo);

	CString GetLastResetSerialNoDate();
	BOOL SetLastResetSerialNoDate(CString szLastResetSerialNoDate);

	BOOL SetPSTNVBinBlkData(void *pvNVRAM, LONG lNVRAM_BinTable_Start, ULONG ulBlkId);
	BOOL SetPSTNVBinBlkData_HW(	void *pvNVRAM_HW,	LONG lNVRAM_BinTable_Start, ULONG ulBlkId);	//v4.65A3

	ULONG GetNVNoOfBondedDie();
	BOOL SetNVNoOfBondedDie(ULONG ulNvNoOfBondedDie);

	ULONG GetNVCurrBondIndex();
	BOOL SetNVCurrBondIndex(ULONG ulNvCurrBondIndex);

	ULONG GetNVLastFileSaveIndex();
	BOOL SetNVLastFileSaveIndex(ULONG ulNvLastFileSaveIndex);

	BOOL GetNVIsFull();
	BOOL SetNVIsFull(BOOL bNvIsFull);

	BOOL GetNVIsAligned();
	BOOL SetNVIsAligned(BOOL bIsAligned);

	LONG GetNVXOffset();
	BOOL SetNVXOffset(LONG lXOffset);

	LONG GetNVYOffset();
	BOOL SetNVYOffset(LONG lYOffset);

	DOUBLE GetNVRotateAngleX();
	BOOL SetNVRotateAngleX(DOUBLE dRotateAngleX);

	CString GetExtraInfoFormat();
	BOOL SetExtraInfoFormat(CString szExtraInfoFormat);

	ULONG GetSkipUnit();
	BOOL SetSkipUnit(ULONG ulSkipNo);

	ULONG GetMaxUnit();
	BOOL SetMaxUnit(ULONG ulMaxNo);

	ULONG GetEmptyUnit();
	BOOL SetEmptyUnit(ULONG ulEmptyNo);

	CString GetStartDate();
	BOOL SetStartDate(CString szDate);

	ULONG GetAccmulatedCount();
	BOOL SetAccmulatedCount(ULONG ulCount);

	BOOL SetUserRowCol(ULONG ulDiePerUserRow, ULONG ulDiePerUserCol);
	BOOL GetUserRowCol(ULONG& ulDiePerUserRow, ULONG& ulDiePerUserCol);

	BOOL GetIsTeachWithPhysicalBlk();
	VOID SetTeachWithPhysicalBlk(BOOL bTeachWithPhysicalBlk);	

	BOOL GetIsCentralizedBondArea();
	BOOL SetIsCentralizedBondArea(BOOL bCentralizedBondArea);
		
	LONG GetCentralizedOffsetX();
	BOOL SetCentralizedOffsetX(LONG lCentralizedOffsetX);

	LONG GetCentralizedOffsetY();
	BOOL SetCentralizedOffsetY(LONG lCentralizedOffsetY);

	VOID SetEnableWafflePad(BOOL bEnable);
	BOOL GetIsEnableWafflePad();

	VOID SetWafflePadSizeX(LONG lSizeX);
	LONG GetWafflePadSizeX();

	VOID SetWafflePadSizeY(LONG lSizeY);
	LONG GetWafflePadSizeY();

	VOID SetWafflePadDistX(LONG lDistX);
	LONG GetWafflePadDistX();
	
	VOID SetWafflePadDistY(LONG lDistY);
	LONG GetWafflePadDistY();

	//v3.70T3	//Circular bin sorting fcn (PLLM REBEL)
	BOOL GetUseCircularArea();
	LONG GetRadius();
	LONG GetCirBinCenterX();
	LONG GetCirBinCenterY();
	VOID SetUseCircularArea(BOOL bEnable, LONG lRadius);
	VOID SetCirBinCenterXY(LONG lCX, LONG lCY);

	/***************************************/
	/*		Bin Map Functions	//v4.42T6  */
	/***************************************/
	BOOL	GetEnableBinMapOffset();
	LONG	GetBinMapRowOffset();
	LONG	GetBinMapColOffset();
	VOID	SetBinMapOffset(BOOL bEnable, LONG lRowOffset, LONG lColOffset);
	ULONG	GetBinMapWalkPath();
	BOOL	SetBinMapWalkPath(ULONG ulWalkPath);
	DOUBLE	GetBinMapCircleRadius();
	VOID	SetBinMapCircleRadius(DOUBLE dRadiusInUm);
	LONG	GetBinMapTopEdge();
	LONG	GetBinMapBtmEdge();
	LONG	GetBinMapLeftEdge();
	LONG	GetBinMapRightEdge();
	VOID	SetBinMapEdgeSize(LONG lTEdge, LONG lBEdge, LONG lLEdge, LONG lREdge);

	VOID	GetRowColWithTLH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								  LONG &lBinRow, LONG &lBinCol);
	VOID	GetRowColWithTRH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								  LONG &lBinRow, LONG &lBinCol);
	VOID	GetRowColWithBLH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								  LONG &lBinRow, LONG &lBinCol);
	VOID	GetRowColWithBRH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								  LONG &lBinRow, LONG &lBinCol);

	VOID	GetRowColWithTLV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								  LONG &lBinRow, LONG &lBinCol);
	VOID	GetRowColWithTRV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								  LONG &lBinRow, LONG &lBinCol);
	VOID	GetRowColWithBLV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								  LONG &lBinRow, LONG &lBinCol);
	VOID	GetRowColWithBRV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
								  LONG &lBinRow, LONG &lBinCol);

	VOID	GetRowColWithIndexInBinMap(const ULONG ulBinIndex, const ULONG ulWalkPath, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
									   LONG &lBinRow, LONG &lBinCol);
	VOID	InitBondedDieInBinMap();
	BOOL	IsUseBinMap();
	BOOL	CreateBondedDieInBinMap(const ULONG ulGradeCapacity, const ULONG ulLastIndex);
	BOOL	IsBondedDieInBinMap(const ULONG ulRow, const ULONG ulCol);

	BOOL	GetNextDieIndexInBinMap(ULONG ulGradeCapacity, ULONG &ulNextIndex, LONG &lBMapRow, LONG &lBMapCol);
	BOOL	GetDieIndexInBinMapWithRowCol(ULONG ulGradeCapacity, const ULONG ulRow, const ULONG ulCol, ULONG &ulIndex);
	BOOL	GetDieIndexInBinMap(ULONG ulGradeCapacity, ULONG &ulIndex, LONG &lBMapRow, LONG &lBMapCol);
	ULONG	GetLastDieIndexIn1stBMapRow(ULONG &ulNoOfDiesIn1stRow);			//Nichia//v4.43T7
	BOOL	IsDieInBinMap(CONST LONG lRow, CONST LONG lCol);
	BOOL	IsWithinOvalBinMapLimit(DOUBLE dCX, DOUBLE dCY, DOUBLE dMapX, DOUBLE dMapY, DOUBLE dRadiusX, DOUBLE dRadiusY);
	BOOL	FindBinMapTEdge1stRow(ULONG ulDicesOnRow, LONG &lStartRow);
	BOOL	FindBinMapLEdge1stCol(ULONG ulDicesOnCol, LONG &lStartCol);		//Nichia//v4.43T7

	BOOL GetIsUseBlockCornerAsFirstDiePos();
	BOOL SetIsUseBlockCornerAsFirstDiePos(BOOL bUseBlockCornerAsFirstDiePos);

	BOOL GetNoReturnTravel() CONST;
	VOID SetNoReturnTravel(CONST BOOL bEnable);

	/*******************************************/
	/*         Bin Block Setup functions       */
	/*******************************************/
	BOOL CheckBlockSize(LONG lBondXOffset, LONG lBondYOffset);
	ULONG CalulateAcutalDiePerBlk(LONG lBondXOffset, LONG lBondYOffset);
	ULONG CalculateDiePerRow(DOUBLE dBondXOffset);
	ULONG CalculateDiePerCol(DOUBLE dBondYOffset);
	ULONG CalculateDiePerBlk();
	BOOL  CalculateCentralizedBondArea();
	BOOL  CalculateCentralizedBondAreaFromPhyLimit(LONG lPhyULX, LONG lPhyULY, LONG lPhyLRX, LONG lPhyLRY);

	/******************************************/
	/*              Step Move                 */
	/******************************************/
	BOOL FindBinTableXYPosnGivenIndex(ULONG ulIndex, DOUBLE& dXPosn, DOUBLE &dYPosn, BOOL bLog=FALSE);	//v4.57A7

	/*****************************************/
	/*         Save Die Info functions       */
	/*****************************************/		
	VOID CalculateDieInfoDbCapacity(ULONG ulDiePerBlk, BOOL bAddDieTimeStamp, BOOL bCheckProcessTime);
	BOOL SaveDieInfo(ULONG ulBondIndex, LONG lWaferMapX, LONG lWaferMapY, 
					 BOOL bAddDieTimeStamp, BOOL bCheckProcessTime, 
					 LONG lWafEncX = 0, LONG lWafEncY = 0,		//andrewng //2020-0619
					 LONG lBtEncX = 0, LONG lBtEncY = 0);		//andrewng //2020-0630

	/****************************************/
	/*       Output File Manipulations      */
	/****************************************/
	BOOL FindBondRowColGivenIndex(ULONG ulIndex, LONG& lRow, LONG& lCol, BOOL bUseOrigPath=FALSE);		//v4.57A4	
	BOOL IsContentEmpty(const CString szElectricInfo);
	BOOL GenTempFileFromNVRAM(BT_NVTEMPDATA* pNVData, ULONG ulBlkId, BOOL bFilenameASWaferID, BOOL bAddDieTimeStamp , BOOL bAddSortingSequence);	//v4.65A1
	LONG GenTempFile(ULONG ulBlkId, BOOL bFilenameASWaferID, BOOL bAddDieTimeStamp , BOOL bCheckProcessTime,BOOL bAddSortingSequence, BOOL& bHasUpdatedFile,
					 BOOL &bWaferMapElectricInfoEmpty, BOOL bUseNVData=FALSE, BOOL bForceDisableHaveOtherFile = FALSE);

	VOID BackupTempFile(ULONG ulBlkId);

	/***************************************/
	/* First Row/Col Skip Unit function    */
	/***************************************/
	BOOL IsFirstRowLastDieSkipDie();
	ULONG GetFirstRowColSkipPatTotalSkipUnitCount(ULONG ulInputCount);
	BOOL IsFirstRowColSkipBondPatternIndex(ULONG ulIndex);
	ULONG GetSkippedUnitForFirstRowColSkipPattern(ULONG ulLastIndex, ULONG ulCurrentIndex);
	BOOL GetFirstRowColSkipBondPatternIndex(ULONG ulOrigIndex, ULONG& ulUpdatedIndex);

	/***************************************/
	/*       LSBond index function         */	//PLLM Lumiramic MS899/MS899DLA/MS109
	/***************************************/
	BOOL  GetLSBondIndex(ULONG ulOrigIndex, ULONG &ulLSBondIndex);
	ULONG RecalculateLSBondIndex(ULONG ulOrigIndex);				//v4.35T3	//PLLM MS109 Lumiramic
	ULONG GetOriginalIndex(ULONG ulIndex);

	/***************************************/
	/*       Set Mapped Grade				*/
	/***************************************/
	VOID SetMappedGrade(CString szMappedGrade);

	VOID GenerateRandomHoleDieIndex(ULONG ulBlkToUse, ULONG ulHoleDieNum, ULONG ulInputCount);	//v3.35
	ULONG GetRandomHoleDieIndex(ULONG ulBlkToUse, ULONG ulIndex);
	VOID SetRandomHoleDieIndex(ULONG ulBlkToUse, ULONG ulIndex, ULONG ulRandomNum);
	VOID SetEmptyHoleDieNum(ULONG ulHoleDieNum);

	VOID SetFirstRowColSkipPattern(BOOL bEnalblePattern);
	BOOL GetFirstRowColSkipPattern();
	VOID SetFirstRowColSkipUnit(ULONG ulRowColSkipUnit);
	ULONG GetFirstRowColSkipUnit();

	/****************************************/
	/*  Circular Pattern sorting fcn (PLLM) */
	/****************************************/
	//v3.70T2
	BOOL IsWithinCircularArea(DOUBLE dDiex, DOUBLE dDiey, DOUBLE& dDist);
	//ULONG CalculateNoOfDiePreCirBlk();
	BOOL FindBinTableXYCirPosnGivenIndex(ULONG ulIndex, LONG& lXPosn, LONG &lYPosn);
	ULONG CalculateDiePerCircularBlk();
	BOOL FindCirBondRowColGivenIndex(ULONG ulIndex, LONG& lRow, LONG& lCol);	//pllm

	/****************************************/
	/*  Waffle Pad FUNC */
	/****************************************/
	BOOL IsWafflePadSkipPos(ULONG ulIndex);
	BOOL GetWafflePadSkipIndex(ULONG ulIndex, ULONG& ulUpdatedIndex);
	ULONG GetSkippedUnitForWafflePad(ULONG ulLastIndex, ULONG ulCurrentIndex);
	ULONG GetWafflePadTotalSkipUnitCount(ULONG ulInputCount);
	BOOL FindBinTableXYWafflePadPosnGivenIndex(ULONG ulIndex, DOUBLE& dXPosn, DOUBLE &dYPosn);		//v4.59A19	//Change LONG to DOUBLE
	BOOL FindWafflePadBondRowColGivenIndex(ULONG ulIndex, LONG& lRow, LONG &lCol);

public:
	static char m_cBondedDieMap[MAX_BINMAP_SIZE_X][MAX_BINMAP_SIZE_Y];
	static INT m_nBinMap[MAX_BINMAP_SIZE_X][MAX_BINMAP_SIZE_Y];		
	BOOL m_bEnableBinMap;		//Changed from static to member variables	//v4.42T6

private:

	BOOL AppendInfoInWaferID(CString& szWaferId, CString szCustomer);
	DOUBLE CalculateCurrBondOffsetX(ULONG ulCurrRow, ULONG ulMaxRow);
	DOUBLE CalculateCurrBondOffsetY(ULONG ulCurrCol, ULONG ulMaxCol, UCHAR ucSortDir);

	LONG	m_lUpperLeftX;	//"real" upper left x to use
	LONG	m_lUpperLeftXFromSetup; //upper left x inputted from setup
	LONG	m_lUpperLeftY; //"real" upper left y to use
	LONG	m_lUpperLeftYFromSetup; //upper left y inputted from setup
	LONG	m_lLowerRightX; //"real" lower right x to use
	LONG	m_lLowerRightXFromSetup; //lower right x inputted from setup
	LONG	m_lLowerRightY; //"real" lower right y to use
	LONG	m_lLowerRightYFromSetup; //lower right y inputted from setup
		
	ULONG	m_ulWidth; //"real" width to use
	ULONG	m_ulWidthFromSetup; //width calculated from setup data
	ULONG	m_ulHeight; //"real" height to use
	ULONG	m_ulHeightFromSetup; //height calculated from setup data
	UCHAR	m_ucGrade;
	UCHAR	m_ucOriginalGrade;
	DOUBLE	m_dDiePitchX;				//v4.59A19
	DOUBLE	m_dDiePitchY;				//v4.59A19
	BOOL	m_bPoint5UmInDiePitchX;		//v4.42T5
	BOOL	m_bPoint5UmInDiePitchY;		//v4.42T5
	DOUBLE	m_dBondAreaOffsetX;			//v4.42T9
	DOUBLE	m_dBondAreaOffsetY;			//v4.42T9
	DOUBLE	m_dThermalDeltaPitchX;		//v4.59A22			
	DOUBLE	m_dThermalDeltaPitchY;		//v4.59A22			
	ULONG	m_ulWalkPath;
	ULONG	m_ulDiePerRow;
	ULONG	m_ulDiePerCol;
	ULONG	m_ulDiePerUserRow;
	ULONG	m_ulDiePerUserCol;
	ULONG	m_ulDiePerBlk;
	ULONG	m_ulSkipUnit;
	ULONG	m_ulMaxUnit;	
	BOOL	m_bIsDisableFromSameGradeMerge;
	ULONG	m_ulPhyBlkId;
	BOOL	m_bIsAssignedWithSlot;
	BOOL	m_bIsSetup;
	UCHAR	m_ucStatus;
	UCHAR	m_ucSortDirection;		//v3.62
	BOOL	m_bByPassBinMap;		//v4.48A11	//Semitek, 3E DL

	BOOL	m_bUseCircularPattern;	//v3.70T2
	LONG	m_lRadius;				//v3.70T3
	LONG	m_lCenterX;				//v3.70T3
	LONG	m_lCenterY;				//v3.70T3

	//v4.42T6	//Other new BinMap attribures
	BOOL	m_bEnableBinMapOffset;
	LONG	m_lBinMapRowOffset;
	LONG	m_lBinMapColOffset;
	ULONG	m_ulBinMapWalkPath;
	DOUBLE	m_dBinMapCircleRadius;
	LONG	m_lBinMapTopEdge;
	LONG	m_lBinMapBtmEdge;
	LONG	m_lBinMapLeftEdge;
	LONG	m_lBinMapRightEdge;

	BOOL	m_bFirstTimeWriteTempFile;

	BOOL	m_bIsDatabaseGenerated;
	CString m_szOutputFilename;
	ULONG	m_ulSerialNo;
	CString m_szLastResetSerialNoDate;
	CString m_szExtraInfoFormat;
	CString m_szStartDate;
	ULONG	m_ulAccmulatedCount;

	ULONG	m_ulEmptyUnit;

	//ULONG m_ulFirstRowColSkipUnit;
	UCHAR	m_ucFrameRotation;		//v4.49A8
	LONG	m_lBinCalibX;			//v4.49A9	//in file unit (um)
	LONG	m_lBinCalibY;			//v4.49A9	//in file unit (um)

	CStringArray m_szaDieBondTime;
	CStringArray m_szaDieProcessTime;
	CDWordArray m_dwaWMapX;
	CDWordArray m_dwaWMapY;
	CDWordArray m_dwaWMapSeqNo;		//Nichia//v4.43T7
	
	//CDWordArray m_dwaWMapEncX;			//andrewng //2020-0619
	//CDWordArray m_dwaWMapEncY;			//andrewng //2020-0619
	//CDWordArray m_dwaBtEncX;				//andrewng //2020-0630
	//CDWordArray m_dwaBtEncY;				//andrewng //2020-0630
	CArray<long, long> m_dwaWMapEncX;		//andrewng //2020-0706
	CArray<long, long> m_dwaWMapEncY;		//andrewng //2020-0706
	CArray<long, long> m_dwaBtEncX;			//andrewng //2020-0706
	CArray<long, long> m_dwaBtEncY;			//andrewng //2020-0706
		
	CString m_szMappedGrade;

	typedef struct
	{
		ULONG ulNoOfBondedDie;
		ULONG ulCurrBondIndex;
		ULONG ulLastFileSaveIndex;
		BOOL bIsFull;
		BOOL bIsAligned;
		LONG lXOffset;
		LONG lYOffset;
		DOUBLE dRotateAngleX;
	} BT_NVBINBLKDATA;

	BT_NVBINBLKDATA *m_pstNvBinBlkData;
	BT_NVBINBLKDATA *m_pstNvBinBlkData_HW;			//v4.65A3

	ULONG	m_ulHoleDieIndex[BINBLK_MAX_SIZE][10];	//hold the random hole die index generated for each bin		//v3.33T3
	ULONG	m_ulHoleDieNum;		//v3.33T3

	BOOL	m_bEnableFirstRowColSkipPattern;
	ULONG	m_ulFirstRowColSkipUnit;

	BOOL	m_bTeachWithPhysicalBlk;

	//Upper Left X, Y after centerizing the bond area
	BOOL	m_bCentralizedBondArea;
	LONG	m_lCentralizedOffsetX;
	LONG	m_lCentralizedOffsetY;
	LONG	m_lCentralizedUpperLeftX;
	LONG	m_lCentralizedUpperLeftY;

	BOOL	m_bEnableWafflePad;
	LONG	m_lWafflePadDistX;
	LONG	m_lWafflePadDistY;
	LONG	m_lWafflePadSizeX;
	LONG	m_lWafflePadSizeY;

	//Use Block Corner as First Die position (for Cree Waffle Pad)
	BOOL	m_bUseBlockCornerAsFirstDiePos;

	//Nichia Arrangement Code fcns	//v4.42T1
	BOOL	m_bNoReturnTravel;		

	DOUBLE	m_dCycleTime;
	DOUBLE	m_dLastTime;
}; //end Class CBinBlk

/******************************* Class CBinBlk End *******************************/
