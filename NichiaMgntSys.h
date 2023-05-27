/////////////////////////////////////////////////////////////////
// NichiaMgntSys.h : Header file for Nichia Management Sub-system Class
/////////////////////////////////////////////////////////////////
#pragma once

#include "StringMapFile.h"


//=================================
// DEFINE macros (General)
//=================================
#define MS_NICHIA_RMUR_FILE_EXTENSION			".txt"
#define MS_NICHIA_MACHINELOG_FILE_EXTENSION		".csv"

#define NICHIA_DATA			"Nichia Data"
#define NC_PRM				"PRM Parameters"
#define NC_MCLOG			"Machine Log"

#define NC_RMUM_PATH		"RMUM Path"
#define NC_STATELOG_PATH	"StateLog Path"
#define NC_MCLOG_PATH		"Machine Log Path"
#define NC_BINSHEET_PATH	"Bin Sheet Path"

#define NC_PROCESS_MODE		"Process Mode"
#define NC_MAX_WAFER		"Max Wafer"
#define NC_WAFER_COUNT		"Wafer Count"
#define NC_BIN_SHAPE		"Bin Shape"
#define NC_BIN_ORDER		"Bin Order"
#define NC_ENABLE_OPSTATELOG	"Enable OpState Log"

#define NC_MAP				"Map Data"
#define NC_WAFER_TYPE		"Wafer Type"
#define NC_WAFER_TEG_TYPE	"Wafer TEG Type"
#define NC_BASE_PT_POS		"Map BasePt Pos"
#define NC_BASE_PT_COL_WIDTH	"Map BasePt ColWidth"
#define NC_BASEPT_INFO		"Base Pt Info"
#define NC_REG_CODE			"Regulatory Code"

#define NC_RAW_ID_LIST		"Raw Material ID List"
#define NC_COLLET_TYPE		"Collet Type"
#define NC_COLLET2_TYPE		"Collet2 Type"
#define NC_NEEDLE_TYPE		"Needle Type"
#define NC_SHEET_TYPE		"Sheet Type"

#define NC_COLLET_TYPE_PRM		"Collet Type PRM"
#define NC_COLLET2_TYPE_PRM		"Collet2 Type PRM"
#define NC_NEEDLE_TYPE_PRM		"Needle Type PRM"
#define NC_SHEET_TYPE_PRM		"Sheet Type PRM"

#define NC_COLLET_START_TIME	"Collet Start Time"
#define NC_COLLET2_START_TIME	"Collet2 Start Time"
#define NC_NEEDLE_START_TIME	"Needle Start Time"

#define NC_WAFER_ACCESS				"Wafer Access Time"
#define NC_WAFER_LOAD				"Wafer Load Time"
#define NC_WAFER_INFO				"Wafer Info Time"
#define NC_MAP_ACCESS				"Map Access Time"
#define NC_MAPPING_START			"Mapping Start Time"
#define NC_MAPPING_END				"Mapping End Time"
#define NC_SORT_START				"Sort Start Time"
#define NC_SORT_START2				"Sort Start Time 2"
#define NC_SORT_END					"Sort End Time"
#define NC_WAFER_UNLOAD				"Wafer Unload Time"
#define NC_MOTION_END				"Motion End Time"
#define NC_TRAY_COUNT				"Bin Tray Count"
#define NC_ERR_COUNT				"Error Count"
#define NC_DOWNTIME_BEFORENIGHT		"DownTime Before MNight"
#define NC_DOWNTIME_AFTERNIGHT		"DownTime After MNight"
#define NC_SORTTIME_BEFORENIGHT		"SortTime Before MNight"
#define NC_SORTTIME_AFTERNIGHT		"SortTime After MNight"
#define NC_DOWN_TIME_START			"Down Time Start"
#define NC_IS_DOWN_TIME_START		"Is Down Time Start"

#define NC_MAP_BASEPT_CENTER		0
#define NC_MAP_BASEPT_LEFT			1
#define NC_MAP_BASEPT_RIGHT			2

#define NC_OPSTATE_GREEN			1
#define NC_OPSTATE_RED				2
#define NC_OPSTATE_YELLOW			3
#define NC_OPSTATE_BLINKYELLOW		4

#define NC_OPCODE_IDLE				1
#define NC_OPCODE_AUTOBOND_START	2

#define NC_MAX_NO_AC				100
#define NC_AC_ENABLE				"Enable ARR_CODE"
#define NC_ARRCODE					"ARR_CODE"
#define NC_AC_TYPE					"Type"
#define NC_AC_AREA_X				"Bin Area X"
#define NC_AC_AREA_Y				"Bin Area Y"
#define NC_AC_START_POS				"Bin Start Pos"
#define NC_AC_MAX_LOAD				"Bin Max Load"
#define NC_AC_MAX_CAP				"Bin Max Cap"
#define NC_AC_PITCH_X				"Bin Pitch X"
#define NC_AC_PITCH_Y				"Bin Pitch Y"
#define NC_AC_RETURN_TRAVEL			"Bin Return travel"

#define NC_BIN_ARRCODE				"Bin ARR_CODE"


#define NC_BT_MAX_BINBLK_NO			175		//150		//v3.83


//////////////////////////////////////////////////////////////
// CLASS Definitions
//////////////////////////////////////////////////////////////

class CMSNichiaMgntSubSystem: public CObject
{
public:
	
	//======================================================================
	// Class Constructor/Destructor Fcns
	//======================================================================
	CMSNichiaMgntSubSystem();
	~CMSNichiaMgntSubSystem();

	VOID CleanUp();
	BOOL LoadData(BOOL bLoadPRMFile=FALSE);
	BOOL SaveData(VOID);

	//======================================================================
	// Public GET/SET Fcns
	//======================================================================
	VOID SetEnabled(CONST BOOL bEnable);
	BOOL IsEnabled();
	CString GetLastError();

	//======================================================================
	// Public Supporting Fcns
	VOID SetWaferType(CONST UCHAR ucType);
	VOID SetWaferTEGType(CONST UCHAR ucType);
	VOID SetMapBasePt(CONST CString szBasePtInfo);
	CString GetMapBasePt();
	VOID SetMapBasePtColWidth(CONST LONG lColWidth);
	LONG GetMapBasePtColWidth();
	//VOID SetMapBasePtInfo(CONST CString szInfo);
	CString GetRegulatoryCode();
	VOID SetRegulatoryCode(CONST CString szCode);

	//======================================================================
	//Output Summary Data
	BOOL ResetLastRawMaterialBeforeRegister(CONST UCHAR ucType);		//54321234
	BOOL RegisterRawMaterial(CONST UCHAR ucType, CONST CString szMaterialID, LONG& lCurrCount, 
								LONG& lMaterialLimit, LONG& lExpireDays);
	BOOL AddRawMaterialUsageRecord(	CONST CString szMaterialID, CONST LONG lCurrCount);
	BOOL CheckRawMaterialExpireDate(CONST CString szMaterialID);
	BOOL RestoreLastMaterialType(CONST UCHAR ucType);
	BOOL ResetLastMaterialType(CONST UCHAR ucType);
	VOID UpdateRawMaterialList(CONST UCHAR ucType=5);
	//BIn Sheet fcn
	BOOL CheckBinSheetType(CONST CString szFrameBarCode);
	//Logging Data
	BOOL LogOpState(CONST UINT unCode, CONST UCHAR ucLTSignal);
	
	BOOL WriteMachineLog(CONST BOOL bReset=FALSE, CONST CString szCode=_T(""));
	VOID UpdateMachineLog_Time(CONST UCHAR ucLogItem);
	CString GetMachineLog_Time(CONST UCHAR ucLogItem);		//v4.22T6
	VOID AddBinTrayCount();
	VOID MLog_AddErrorCount(UINT unCode=0);
	VOID MLog_AddErrorCount(CONST CString szMsg);
	BOOL MLog_LogSortTime(CONST BOOL bStart=TRUE, BOOL bForceStart=FALSE);
	BOOL MLog_LogDownTime(CONST BOOL bStart=TRUE);

	//======================================================================
	//Arrangement Code Fcns
	UCHAR GetMapArrCode(CONST ULONG ulBinBlkID);
	UCHAR GetBinArrCode(CONST ULONG ulBinBlkID);
	BOOL UpdateMapArrCode(CONST CString szCodeString);
	BOOL CompareMapAndBinArrCode(CONST ULONG ulBinBlkID, 
									LONG& lBinArrCode, 
									LONG& lMapArrCode, 
									BOOL& bNeedUpdateBin);
	BOOL UpdateBinArrCode(CONST ULONG ulBinBlkID, CONST LONG lCode);
	BOOL SaveArrCode(CONST UCHAR ucCodeIndex);
	BOOL LoadArrCode(CONST UCHAR ucCodeIndex);
	BOOL GetArrCode(CONST ULONG ulBinBlkID, BOOL bUseMapCode,
					UCHAR& ucType, DOUBLE& dAreaX, DOUBLE& dAreaY,
					UCHAR& ucStartPos, ULONG& ulMaxLoad, ULONG& ulPitchX, ULONG& ulPitchY,
					BOOL& bRetTravel);
	BOOL GetArrCodeByIndex(CONST UCHAR ucIndex,
					UCHAR& ucType, DOUBLE& dAreaX, DOUBLE& dAreaY,
					UCHAR& ucStartPos, ULONG& ulMaxLoad, ULONG& ulPitchX, ULONG& ulPitchY,
					BOOL& bRetTravel);


protected:

	//======================================================================
	// Other Utility functions
	//======================================================================
	CString GetCurrTimeInNichiaTimeFormat();
	BOOL GetTimeFromNichiaTimeFormat(CONST CString szNichiaTime, CTime& tTime);


private:

	//======================================================================
	// Critical sections
	//======================================================================
	CCriticalSection	m_CsStateLog;
	CCriticalSection	m_CsRMUR;
	CCriticalSection	m_CsMachineLog;
	CCriticalSection	m_CsBinSheet;
	CCriticalSection	m_CsSortTimeLog;
	CCriticalSection	m_CsDownTimeLog;

	//======================================================================
	// Class Attributes
	//======================================================================
	CStringMapFile		*m_psmfSRam;

	BOOL		m_bIsEnabled;
	CString		m_szErrMsg;
	//CString	m_szMapBasePtInfo;
	CString		m_szMapRegulatoryCode;
	CString		m_szMapBasePt;				//v4.40T13
	LONG		m_lMapBaseColWidth;			//v4.41T4

	//MachineLog attributes
	CString		m_szTimeWaferAccess;		//2
	CString		m_szTimeWaferLoad;			//3
	CString		m_szTimeWaferInfoAcq;		//4
	CString		m_szTimeMapAccess;			//5
	CString		m_szTimeMappingStart;		//6
	CString		m_szTimeMappingEnd;			//7
	CString		m_szTimeSortingStart;		//8
	CString		m_szTimeSortingStart2;		//8.2
	CString		m_szTimeSortingEnd;			//9
	CString		m_szTimeWaferUnload;		//10
	CString		m_szTimeMotionEnd;			//11
	LONG		m_lBinTrayCount;			//21
	LONG		m_lErrorCount;				//22
	
	BOOL		m_bSortTimeStart;
	BOOL		m_bDownTimeStart;
	//CString	m_szDownTimeStart;
	CTime		m_tDownTimeStart;
	CTime		m_tSortTimeStart;
	CString		m_szTotalDownTimeBeforeMidNightInSec;	//13
	CString		m_szTotalDownTimeAfterMidNightInSec;	//14
	CString		m_szTotalSortTimeBeforeMidNightInSec;	//15
	CString		m_szTotalSortTimeAfterMidNightInSec;	//16

	CString		m_szLastColletType;
	CString		m_szLastCollet2Type;
	CString		m_szLastNeedleType;
	CString		m_szLastBinSheetType;
	
	CString		m_szTimeColletStart;
	CString		m_szTimeCollet2Start;
	CString		m_szTimeNeedleStart;

	LONG		m_lBinArrCode[NC_BT_MAX_BINBLK_NO+1];
	LONG		m_lMapBinArrCode[NC_BT_MAX_BINBLK_NO+1];	//info from current map file

	UCHAR		m_ucArrCodeType[NC_MAX_NO_AC];				//0=rect, 1=Circular
	DOUBLE		m_dArrCodeAreaX[NC_MAX_NO_AC];
	DOUBLE		m_dArrCodeAreaY[NC_MAX_NO_AC];
	UCHAR		m_ucArrCodeStartPos[NC_MAX_NO_AC];
	ULONG		m_ulArrCodeMaxLoad[NC_MAX_NO_AC];
	ULONG		m_ulArrCodeMaxCap[NC_MAX_NO_AC];			//5432123
	ULONG		m_ulArrCodePitchX[NC_MAX_NO_AC];
	ULONG		m_ulArrCodePitchY[NC_MAX_NO_AC];
	BOOL		m_bArrCodeReturnTravel[NC_MAX_NO_AC];		//FALSE=no return, TRUE=return


public:

	//======================================================================
	// Class Attributes
	//======================================================================
	CString		m_szFilePath_RawMaterialMgntSys;
	CString		m_szFilePath_OpStateLog;
	CString		m_szFilePath_MachineLog;	//WaferEnd
	CString		m_szFilePath_BinSheet;

	//Machine RPM content
	UCHAR		m_ucProcessMode;			//0=Batch; 1=Single
	ULONG		m_ulMaxNoOfWafer;			//No of continuous Supply Lot for each bin frame
	ULONG		m_ulCurrWaferCount;			//Current Supply Lot count
	UCHAR		m_ucBinShape;				//0=rect; 1=circular
	UCHAR		m_ucBinOrder;				//

	UCHAR		m_ucWaferType;				//v4.40T10
	UCHAR		m_ucWaferTEGType;			//v4.40T13
	BOOL		m_bIsOpStateLogEnabled;		//
	CString		m_szRawMaterialList;		//WaferEnd

	//Material Type file names
	CString		m_szColletType;
	CString		m_szCollet2Type;
	CString		m_szNeedleType;
	CString		m_szBinSheetType;

	//Actual material type to be saved in PRM
	CString		m_szColletTypePRM;
	CString		m_szCollet2TypePRM;
	CString		m_szNeedleTypePRM;
	CString		m_szBinSheetTypePRM;

	//Curr ArrangeCode for display purpose
	BOOL		m_bEnableArrCodeFcn;
	UCHAR		m_ucArrCodeIndex;
	UCHAR		m_ucArrCodeCurrType;
	DOUBLE		m_dArrCodeCurrAreaX;
	DOUBLE		m_dArrCodeCurrAreaY;
	UCHAR		m_ucArrCodeCurrStartPos;
	ULONG		m_ulArrCodeCurrMaxLoad;
	ULONG		m_ulArrCodeCurrMaxCap;	//54321
	ULONG		m_ulArrCodeCurrPitchX;
	ULONG		m_ulArrCodeCurrPitchY;
	BOOL		m_bArrCodeCurrRetTravel;


};	//End of CMSNichiaMgntSubSystem


