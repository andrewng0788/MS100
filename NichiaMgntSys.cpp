/////////////////////////////////////////////////////////////////
// NichiaMgntSys.cpp : 
//
//	Description:
//
//	Date:		
//	Revision:	
//
//	By:			Andrew Ng
//
//	Copyright @ ASM Assembly Automation Ltd., 2013.
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NichiaMgntSys.h"
#include "MS896a.h"
#include "LogFileUtil.h"
#include "MS896A_Constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//================================================================
// Constructor / destructor implementation section
//================================================================

CMSNichiaMgntSubSystem::CMSNichiaMgntSubSystem()
{
	m_bIsEnabled = FALSE;
	CleanUp();
}

CMSNichiaMgntSubSystem::~CMSNichiaMgntSubSystem()
{
	CleanUp();
}

VOID CMSNichiaMgntSubSystem::CleanUp()
{
	m_bSortTimeStart = FALSE;
	m_tSortTimeStart = CTime::GetCurrentTime();
	m_bDownTimeStart = FALSE;
	m_tDownTimeStart = CTime::GetCurrentTime();
	//m_szDownTimeStart = _T("");

	m_ucProcessMode			= 0;	//0=Batch(Continuous); 1=Single
	m_ulMaxNoOfWafer		= 0;
	m_ulCurrWaferCount		= 0;
	m_ucBinShape			= 0;
	m_ucBinOrder			= 0;
	m_bIsOpStateLogEnabled	= FALSE;

	m_ucWaferType			= 0;		//0=800x300, 1=800x600, 2=600x600
	m_ucWaferTEGType		= 0;		//
	m_szMapBasePt			= "";
	m_lMapBaseColWidth		= 0;
	//m_szMapBasePtInfo		= _T("");
	m_szMapRegulatoryCode	= _T("");

	m_szRawMaterialList		= _T("");
	m_szColletType			= _T("");
	m_szCollet2Type			= _T("");
	m_szNeedleType			= _T("");
	m_szBinSheetType		= _T("");
	m_szColletTypePRM		= _T("");
	m_szCollet2TypePRM		= _T("");
	m_szNeedleTypePRM		= _T("");
	m_szBinSheetTypePRM		= _T("");
	m_szErrMsg				= _T("");

	m_szLastColletType		= _T("");
	m_szLastCollet2Type		= _T("");
	m_szLastNeedleType		= _T("");
	m_szLastBinSheetType	= _T("");

	m_szTimeColletStart		= _T("");
	m_szTimeCollet2Start	= _T("");
	m_szTimeNeedleStart		= _T("");

	m_szTimeWaferAccess		= _T("");				//2
	m_szTimeWaferLoad		= _T("");				//3
	m_szTimeWaferInfoAcq	= _T("");				//4
	m_szTimeMapAccess		= _T("");				//5
	m_szTimeMappingStart	= _T("");				//6
	m_szTimeMappingEnd		= _T("");				//7
	m_szTimeSortingStart	= _T("");				//8
	m_szTimeSortingStart2	= _T("");				//8.2
	m_szTimeSortingEnd		= _T("");				//9
	m_szTimeWaferUnload		= _T("");				//10
	m_szTimeMotionEnd		= _T("");				//11
	m_szTotalDownTimeBeforeMidNightInSec= _T("");	//13
	m_szTotalDownTimeAfterMidNightInSec	= _T("");	//14
	m_szTotalSortTimeBeforeMidNightInSec= _T("");	//15
	m_szTotalSortTimeAfterMidNightInSec	= _T("");	//16
	m_lBinTrayCount			= 0;					//21
	m_lErrorCount			= 0;					//22

	m_bEnableArrCodeFcn		= FALSE;
	for (INT i=0; i<NC_MAX_NO_AC; i++)
	{
		m_ucArrCodeType[i]		= 0;
		m_dArrCodeAreaX[i]		= 0;
		m_dArrCodeAreaY[i]		= 0;
		m_ucArrCodeStartPos[i]	= 0;
		m_ulArrCodeMaxLoad[i]	= 0;
		m_ulArrCodeMaxCap[i]	= 0;
		m_ulArrCodePitchX[i]	= 0;
		m_ulArrCodePitchY[i]	= 0;
		m_bArrCodeReturnTravel[i] = FALSE;
	}

	for (INT i=0; i<NC_MAX_NO_AC; i++)
	{
		m_lBinArrCode[i]	= -1;
		m_lMapBinArrCode[i] = -1;
	}

	m_ucArrCodeIndex	= 0;
			
	m_ucArrCodeCurrType		= m_ucArrCodeType[m_ucArrCodeIndex];
	m_dArrCodeCurrAreaX		= m_dArrCodeAreaX[m_ucArrCodeIndex];
	m_dArrCodeCurrAreaY		= m_dArrCodeAreaY[m_ucArrCodeIndex];
	m_ucArrCodeCurrStartPos	= m_ucArrCodeStartPos[m_ucArrCodeIndex];
	m_ulArrCodeCurrMaxLoad	= m_ulArrCodeMaxLoad[m_ucArrCodeIndex];
	m_ulArrCodeCurrMaxCap	= m_ulArrCodeMaxCap[m_ucArrCodeIndex];
	m_ulArrCodeCurrPitchX	= m_ulArrCodePitchX[m_ucArrCodeIndex];
	m_ulArrCodeCurrPitchY	= m_ulArrCodePitchY[m_ucArrCodeIndex];
	m_bArrCodeCurrRetTravel	= m_bArrCodeReturnTravel[m_ucArrCodeIndex];

}

BOOL CMSNichiaMgntSubSystem::LoadData(BOOL bLoadPRMFile)
{
	if (!m_bIsEnabled)
		return FALSE;

	CMSLogFileUtility::Instance()->MS_LogCtmOperation("NICHIA SYS: LoadData");
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_psmfSRam = &(pApp->m_smfSRam);

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	if (pUtl->LoadNichiaConfig() == FALSE)
	{
		m_szErrMsg = _T("Fails to load Nichia MSD");
		return FALSE;
	}

	psmf = pUtl->GetNichiaConfigFile();
	if (psmf == NULL)
	{
		m_szErrMsg = _T("Fails to retrieve Nichia MSD pointer");
		return FALSE;
	}

	// Get data
	m_szFilePath_RawMaterialMgntSys = (*psmf)[NICHIA_DATA][NC_PRM][NC_RMUM_PATH];
	m_szFilePath_OpStateLog			= (*psmf)[NICHIA_DATA][NC_PRM][NC_STATELOG_PATH];
	m_szFilePath_MachineLog			= (*psmf)[NICHIA_DATA][NC_PRM][NC_MCLOG_PATH];
	m_szFilePath_BinSheet			= (*psmf)[NICHIA_DATA][NC_PRM][NC_BINSHEET_PATH];

	m_ucProcessMode			= (*psmf)[NICHIA_DATA][NC_PRM][NC_PROCESS_MODE];
	m_ulMaxNoOfWafer		= (*psmf)[NICHIA_DATA][NC_PRM][NC_MAX_WAFER];
	m_ulCurrWaferCount		= (*psmf)[NICHIA_DATA][NC_PRM][NC_WAFER_COUNT];	
	m_ucBinShape			= (*psmf)[NICHIA_DATA][NC_PRM][NC_BIN_SHAPE];
	m_ucBinOrder			= (*psmf)[NICHIA_DATA][NC_PRM][NC_BIN_ORDER];
	m_bIsOpStateLogEnabled	= (BOOL)(LONG)(*psmf)[NICHIA_DATA][NC_PRM][NC_ENABLE_OPSTATELOG];
	
	m_ucWaferType			= (*psmf)[NICHIA_DATA][NC_MAP][NC_WAFER_TYPE];
	m_ucWaferTEGType		= (*psmf)[NICHIA_DATA][NC_MAP][NC_WAFER_TEG_TYPE];
	m_szMapBasePt			= (*psmf)[NICHIA_DATA][NC_MAP][NC_BASE_PT_POS];
	m_lMapBaseColWidth		= (*psmf)[NICHIA_DATA][NC_MAP][NC_BASE_PT_COL_WIDTH];
	//m_szMapBasePtInfo		= (*psmf)[NICHIA_DATA][NC_MAP][NC_BASEPT_INFO];
	m_szMapRegulatoryCode	= (*psmf)[NICHIA_DATA][NC_MAP][NC_REG_CODE];

	m_szRawMaterialList		= (*psmf)[NICHIA_DATA][NC_PRM][NC_RAW_ID_LIST];
	if (!bLoadPRMFile)		//v4.42T12
	{
		m_szColletType		= (*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET_TYPE];
		m_szCollet2Type		= (*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET2_TYPE];
		m_szNeedleType		= (*psmf)[NICHIA_DATA][NC_PRM][NC_NEEDLE_TYPE];
	}

	m_szBinSheetType		= (*psmf)[NICHIA_DATA][NC_PRM][NC_SHEET_TYPE];

	m_szLastColletType		= m_szColletType;
	m_szLastCollet2Type		= m_szCollet2Type;
	m_szLastNeedleType		= m_szNeedleType;
	m_szLastBinSheetType	= m_szNeedleType;

	//v4.41T3
	m_szTimeColletStart		= (*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET_START_TIME];
	m_szTimeCollet2Start	= (*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET2_START_TIME];
	m_szTimeNeedleStart		= (*psmf)[NICHIA_DATA][NC_PRM][NC_NEEDLE_START_TIME];

	m_szColletTypePRM		= (*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET_TYPE_PRM];
	m_szCollet2TypePRM		= (*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET2_TYPE_PRM];
	m_szNeedleTypePRM		= (*psmf)[NICHIA_DATA][NC_PRM][NC_NEEDLE_TYPE_PRM];
	m_szBinSheetTypePRM		= (*psmf)[NICHIA_DATA][NC_PRM][NC_SHEET_TYPE_PRM];

	m_szTimeWaferAccess		= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_WAFER_ACCESS];		//2
	m_szTimeWaferLoad		= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_WAFER_LOAD];		//3
	m_szTimeWaferInfoAcq	= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_WAFER_INFO];		//4
	m_szTimeMapAccess		= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_MAP_ACCESS];		//5
	m_szTimeMappingStart	= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_MAPPING_START];		//6
	m_szTimeMappingEnd		= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_MAPPING_END];		//7
	m_szTimeSortingStart	= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORT_START];		//8
	m_szTimeSortingStart2	= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORT_START2];		//8.2
	m_szTimeSortingEnd		= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORT_END];			//9
	m_szTimeWaferUnload		= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_WAFER_UNLOAD];		//10
	m_szTimeMotionEnd		= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_MOTION_END];		//11
	m_lBinTrayCount			= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_TRAY_COUNT];		//21
	m_lErrorCount			= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_ERR_COUNT];			//22

	m_szTotalDownTimeBeforeMidNightInSec= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_DOWNTIME_BEFORENIGHT];		//13
	m_szTotalDownTimeAfterMidNightInSec	= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_DOWNTIME_AFTERNIGHT];		//14
	m_szTotalSortTimeBeforeMidNightInSec= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORTTIME_BEFORENIGHT];		//15
	m_szTotalSortTimeAfterMidNightInSec	= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORTTIME_AFTERNIGHT];		//16
	//m_szDownTimeStart		= (*psmf)[NICHIA_DATA][NC_MCLOG][NC_DOWN_TIME_START];	
	//m_bDownTimeStart		= (BOOL)(LONG)(*psmf)[NICHIA_DATA][NC_MCLOG][NC_IS_DOWN_TIME_START];

	m_bEnableArrCodeFcn		= (BOOL)(LONG)(*psmf)[NICHIA_DATA][NC_ARRCODE][NC_AC_ENABLE];

	for (INT i=0; i<NC_MAX_NO_AC; i++)
	{
		m_ucArrCodeType[i]		= (*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_TYPE];
		m_dArrCodeAreaX[i]		= (*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_AREA_X];
		m_dArrCodeAreaY[i]		= (*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_AREA_Y];
		m_ucArrCodeStartPos[i]	= (*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_START_POS];
		m_ulArrCodeMaxLoad[i]	= (*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_MAX_LOAD];
		m_ulArrCodeMaxCap[i]	= (*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_MAX_CAP];
		m_ulArrCodePitchX[i]	= (*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_PITCH_X];
		m_ulArrCodePitchY[i]	= (*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_PITCH_Y];
		m_bArrCodeReturnTravel[i] = (BOOL)(LONG)(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_RETURN_TRAVEL];
	}

	for (INT m=0; m<=NC_BT_MAX_BINBLK_NO; m++)
	{
		m_lBinArrCode[m]	= (*psmf)[NICHIA_DATA][NC_BIN_ARRCODE][m];
	}

	pUtl->CloseNichiaConfig();

	//m_ucArrCodeIndex	= 0;
	m_ucArrCodeCurrType		= m_ucArrCodeType[m_ucArrCodeIndex];
	m_dArrCodeCurrAreaX		= m_dArrCodeAreaX[m_ucArrCodeIndex];
	m_dArrCodeCurrAreaY		= m_dArrCodeAreaY[m_ucArrCodeIndex];
	m_ucArrCodeCurrStartPos	= m_ucArrCodeStartPos[m_ucArrCodeIndex];
	m_ulArrCodeCurrMaxLoad	= m_ulArrCodeMaxLoad[m_ucArrCodeIndex];
	m_ulArrCodeCurrMaxCap	= m_ulArrCodeMaxCap[m_ucArrCodeIndex];
	m_ulArrCodeCurrPitchX	= m_ulArrCodePitchX[m_ucArrCodeIndex];
	m_ulArrCodeCurrPitchY	= m_ulArrCodePitchY[m_ucArrCodeIndex];
	m_bArrCodeCurrRetTravel	= m_bArrCodeReturnTravel[m_ucArrCodeIndex];

	//GetTimeFromNichiaTimeFormat(m_szDownTimeStart, m_tDownTimeStart);

	return TRUE;
}

BOOL CMSNichiaMgntSubSystem::SaveData(VOID)
{
	if (!m_bIsEnabled)
		return FALSE;

	CMSLogFileUtility::Instance()->MS_LogCtmOperation("NICHIA SYS: SaveData");

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	CString szData;

	if (pUtl->LoadNichiaConfig() == FALSE)
	{
		m_szErrMsg = _T("Fails to load Nichia MSD");
		return FALSE;
	}

	psmf = pUtl->GetNichiaConfigFile();
	if (psmf == NULL)
	{
		m_szErrMsg = _T("Fails to retrieve Nichia MSD pointer");
		return FALSE;
	}
		
	// update data
	(*psmf)[NICHIA_DATA][NC_PRM][NC_RMUM_PATH]		= m_szFilePath_RawMaterialMgntSys;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_STATELOG_PATH]	= m_szFilePath_OpStateLog;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_MCLOG_PATH]		= m_szFilePath_MachineLog;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_BINSHEET_PATH]	= m_szFilePath_BinSheet;

	(*psmf)[NICHIA_DATA][NC_PRM][NC_PROCESS_MODE]	= m_ucProcessMode;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_MAX_WAFER]		= m_ulMaxNoOfWafer;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_WAFER_COUNT]	= m_ulCurrWaferCount;	
	(*psmf)[NICHIA_DATA][NC_PRM][NC_BIN_SHAPE]		= m_ucBinShape;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_BIN_ORDER]		= m_ucBinOrder;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_ENABLE_OPSTATELOG]	= m_bIsOpStateLogEnabled;

	(*psmf)[NICHIA_DATA][NC_MAP][NC_WAFER_TYPE]		= m_ucWaferType;
	(*psmf)[NICHIA_DATA][NC_MAP][NC_WAFER_TEG_TYPE]	= m_ucWaferTEGType;
	(*psmf)[NICHIA_DATA][NC_MAP][NC_BASE_PT_POS]	= m_szMapBasePt;
	(*psmf)[NICHIA_DATA][NC_MAP][NC_BASE_PT_COL_WIDTH]	= m_lMapBaseColWidth;

	//(*psmf)[NICHIA_DATA][NC_MAP][NC_BASEPT_INFO]	= m_szMapBasePtInfo;
	(*psmf)[NICHIA_DATA][NC_MAP][NC_REG_CODE]		= m_szMapRegulatoryCode;

	(*psmf)[NICHIA_DATA][NC_PRM][NC_RAW_ID_LIST]	= m_szRawMaterialList;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET_TYPE]	= m_szColletType;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET2_TYPE]	= m_szCollet2Type;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_NEEDLE_TYPE]	= m_szNeedleType;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_SHEET_TYPE]		= m_szBinSheetType;

	//v4.41T3
	(*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET_START_TIME]	= m_szTimeColletStart;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET2_START_TIME]	= m_szTimeCollet2Start;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_NEEDLE_START_TIME]	= m_szTimeNeedleStart;

	(*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET_TYPE_PRM]	= m_szColletTypePRM;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_COLLET2_TYPE_PRM]	= m_szCollet2TypePRM;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_NEEDLE_TYPE_PRM]	= m_szNeedleTypePRM;
	(*psmf)[NICHIA_DATA][NC_PRM][NC_SHEET_TYPE_PRM]		= m_szBinSheetTypePRM;

	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_WAFER_ACCESS]	= m_szTimeWaferAccess;		//2
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_WAFER_LOAD]	= m_szTimeWaferLoad;		//3
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_WAFER_INFO]	= m_szTimeWaferInfoAcq;		//4
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_MAP_ACCESS]	= m_szTimeMapAccess;		//5
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_MAPPING_START]= m_szTimeMappingStart;		//6
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_MAPPING_END]	= m_szTimeMappingEnd;		//7
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORT_START]	= m_szTimeSortingStart;		//8
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORT_START2]	= m_szTimeSortingStart2;	//8.2
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORT_END]		= m_szTimeSortingEnd;		//9
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_WAFER_UNLOAD]	= m_szTimeWaferUnload;		//10
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_MOTION_END]	= m_szTimeMotionEnd;		//11
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_TRAY_COUNT]	= m_lBinTrayCount;			//21
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_ERR_COUNT]	= m_lErrorCount;			//22
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_DOWNTIME_BEFORENIGHT]	= m_szTotalDownTimeBeforeMidNightInSec;	//13
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_DOWNTIME_AFTERNIGHT]	= m_szTotalDownTimeAfterMidNightInSec;	//14
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORTTIME_BEFORENIGHT]	= m_szTotalSortTimeBeforeMidNightInSec;	//15
	(*psmf)[NICHIA_DATA][NC_MCLOG][NC_SORTTIME_AFTERNIGHT]	= m_szTotalSortTimeAfterMidNightInSec;	//16

	(*psmf)[NICHIA_DATA][NC_ARRCODE][NC_AC_ENABLE]	= m_bEnableArrCodeFcn;

	//(*psmf)[NICHIA_DATA][NC_MCLOG][NC_DOWN_TIME_START]		= m_szDownTimeStart;
	//(*psmf)[NICHIA_DATA][NC_MCLOG][NC_IS_DOWN_TIME_START]		= m_bDownTimeStart;
	
/*
	for (INT i=0; i<NC_MAX_NO_AC; i++)
	{
		(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_AREA_X]		= m_dArrCodeAreaX[i];
		(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_AREA_Y]		= m_dArrCodeAreaY[i];
		(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_START_POS]	= m_ucArrCodeStartPos[i];
		(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_MAX_LOAD]		= m_ulArrCodeMaxLoad[i];
		(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_PITCH_X]		= m_ulArrCodePitchX[i];
		(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_PITCH_Y]		= m_ulArrCodePitchY[i];
	}
*/
	for (INT m=0; m<=NC_BT_MAX_BINBLK_NO; m++)
	{
		(*psmf)[NICHIA_DATA][NC_BIN_ARRCODE][m]	= m_lBinArrCode[m];
	}

	pUtl->UpdateNichiaConfig();
	pUtl->CloseNichiaConfig();

	//m_ucArrCodeIndex	= 0;
	m_ucArrCodeCurrType		= m_ucArrCodeType[m_ucArrCodeIndex];
	m_dArrCodeCurrAreaX		= m_dArrCodeAreaX[m_ucArrCodeIndex];
	m_dArrCodeCurrAreaY		= m_dArrCodeAreaY[m_ucArrCodeIndex];
	m_ucArrCodeCurrStartPos	= m_ucArrCodeStartPos[m_ucArrCodeIndex];
	m_ulArrCodeCurrMaxLoad	= m_ulArrCodeMaxLoad[m_ucArrCodeIndex];
	m_ulArrCodeCurrMaxCap	= m_ulArrCodeMaxCap[m_ucArrCodeIndex];
	m_ulArrCodeCurrPitchX	= m_ulArrCodePitchX[m_ucArrCodeIndex];
	m_ulArrCodeCurrPitchY	= m_ulArrCodePitchY[m_ucArrCodeIndex];
	m_bArrCodeCurrRetTravel	= m_bArrCodeReturnTravel[m_ucArrCodeIndex];

	return TRUE;
}


//================================================================
// Public GET/SET function implementation section
//================================================================

VOID CMSNichiaMgntSubSystem::SetEnabled(CONST BOOL bEnable)
{
	m_bIsEnabled = bEnable;
}

BOOL CMSNichiaMgntSubSystem::IsEnabled()
{
	return m_bIsEnabled;
}

CString CMSNichiaMgntSubSystem::GetLastError()
{
	return m_szErrMsg;
}

VOID CMSNichiaMgntSubSystem::SetWaferType(CONST UCHAR ucType)
{
	m_ucWaferType = ucType;		//0=800x300, 1=800x600, 2=600x600
}

VOID CMSNichiaMgntSubSystem::SetWaferTEGType(CONST UCHAR ucType)
{
	m_ucWaferTEGType = ucType;	
}

VOID CMSNichiaMgntSubSystem::SetMapBasePt(CONST CString szBasePtInfo)
{
	CString szInfo = szBasePtInfo;
	CString szBasePtPos;

	if (szInfo.Find(",") != -1)
	{
		szBasePtPos = szInfo.Left(szInfo.Find(","));
		szBasePtPos = szBasePtPos.MakeUpper();	
		
		m_szMapBasePt = szBasePtPos;
/*
		if ( (szBasePtPos == "B") )	//|| (szBasePtPos == "E") )		//Left
		{
			m_ucMapBasePt = NC_MAP_BASEPT_LEFT;
		}
		else if ( (szBasePtPos == "D") )							//Right
		{
			m_ucMapBasePt = NC_MAP_BASEPT_RIGHT;
		}
		else														//Center
		{
			m_ucMapBasePt = NC_MAP_BASEPT_CENTER;
		}
*/
//CString szLog;
//szLog.Format("Base Pt position = %d; Char = " + szBasePtPos, m_ucMapBasePt);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);
		//SaveData();
	}
}

CString CMSNichiaMgntSubSystem::GetMapBasePt()
{
	return m_szMapBasePt;
}

VOID CMSNichiaMgntSubSystem::SetMapBasePtColWidth(CONST LONG lColWidth)
{
	m_lMapBaseColWidth = lColWidth;
	//SaveData();
}

LONG CMSNichiaMgntSubSystem::GetMapBasePtColWidth()
{
	return m_lMapBaseColWidth;
}

//VOID CMSNichiaMgntSubSystem::SetMapBasePtInfo(CONST CString szInfo)
//{
//	m_szMapBasePtInfo = szInfo;
//}

CString CMSNichiaMgntSubSystem::GetRegulatoryCode()
{
	return m_szMapRegulatoryCode;
}

VOID CMSNichiaMgntSubSystem::SetRegulatoryCode(CONST CString szCode)
{
	m_szMapRegulatoryCode	= szCode;
}


//================================================================
// Public function implementation section
//================================================================

BOOL CMSNichiaMgntSubSystem::ResetLastRawMaterialBeforeRegister(CONST UCHAR ucType)
{
	CString szMaterialID;
	switch (ucType)
	{
	case 1:	
		szMaterialID = m_szLastCollet2Type;
		break;
	case 2:
		szMaterialID = m_szLastNeedleType;
		break;
	case 3:
		szMaterialID = m_szLastBinSheetType;
		break;
	default:
		szMaterialID = m_szLastColletType;
		break;
	}

	if (szMaterialID.GetLength() == 0)
		return TRUE;

	CString szFullFileName	= m_szFilePath_RawMaterialMgntSys + "\\" + szMaterialID + MS_NICHIA_RMUR_FILE_EXTENSION;
	CString szlocalRMURFile = gszUSER_DIRECTORY + "\\RMUR\\" + szMaterialID + MS_NICHIA_RMUR_FILE_EXTENSION;
	CString szlocalRMURDir	= gszUSER_DIRECTORY + "\\RMUR";
	CString szFullRMURDir	= m_szFilePath_RawMaterialMgntSys;

	if (_access(szFullRMURDir, 0) == -1)
	{
		m_szErrMsg = _T("Nichia RMUR network path is not accessible at: ") + szFullRMURDir;
		return FALSE;
	}

	CreateDirectory(szlocalRMURDir, NULL);

	if (_access(szlocalRMURFile, 0) != -1)
	{
		if (!CopyFile(szlocalRMURFile, szFullFileName, FALSE))
		{
			m_szErrMsg.Format("Fails to upload RMUR file (%s) from local HD to network:\n" + szFullFileName, szMaterialID);
			return FALSE;
		}
		else
		{
			m_szErrMsg.Format("RMUR file (%s) uploaded from %s to " + szFullFileName, szMaterialID, szlocalRMURFile);
			CMSLogFileUtility::Instance()->MS_LogCtmOperation(m_szErrMsg);
			DeleteFile(szlocalRMURFile);
		}
	}
	else
	{
		m_szErrMsg.Format("Local RMUR file (%s) cannot be found at: " + szlocalRMURFile, szMaterialID);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(m_szErrMsg);
		//AfxMessageBox(m_szErrMsg
	}

	return TRUE;

}

BOOL CMSNichiaMgntSubSystem::RegisterRawMaterial(CONST UCHAR ucType,
												 CONST CString szMaterialID, 
												 LONG& lCurrCount,
												 LONG& lMaterialLimit, LONG& lExpireDays)
{
	if (!m_bIsEnabled)
	{
		m_szErrMsg = _T("Nichia sub-system is not enabled");
		return FALSE;
	}
	if (m_szFilePath_RawMaterialMgntSys.GetLength() == 0)
	{
		m_szErrMsg = _T("Nichia Raw-Material Usage Record path is not available");
		return FALSE;
	}


	//Try to access if material usage record is available
	CString szFullFileName	= m_szFilePath_RawMaterialMgntSys + "\\" + szMaterialID + MS_NICHIA_RMUR_FILE_EXTENSION;
	CString szlocalRMURFile = gszUSER_DIRECTORY + "\\RMUR\\" + szMaterialID + MS_NICHIA_RMUR_FILE_EXTENSION;
	CString szlocalRMURDir	= gszUSER_DIRECTORY + "\\RMUR";

	if (_access(szFullFileName, 0) == -1)
	{
		m_szErrMsg = _T("Nichia RMUR file is not accessible at: ") + szFullFileName;
		return FALSE;
	}
	if (_access(szFullFileName, 2) == -1)
	{
		m_szErrMsg = _T("Nichia RMUR file is not writable at: ") + szFullFileName;
		return FALSE;
	}

	CString szMaterialType = m_szColletTypePRM;
	switch (ucType)
	{
	case 1:	
		szMaterialType = m_szCollet2TypePRM;
		if ((szMaterialID == m_szColletType) || (szMaterialID == m_szNeedleType) || (szMaterialID == m_szBinSheetType))
		{
			m_szErrMsg = _T("Registerted Material file currently in use!");
			return FALSE;
		}
		break;
	case 2:
		szMaterialType = m_szNeedleTypePRM;
		if ((szMaterialID == m_szColletType) || (szMaterialID == m_szCollet2Type) || (szMaterialID == m_szBinSheetType))
		{
			m_szErrMsg = _T("Registerted Material file currently in use!");
			return FALSE;
		}
		break;
	case 3:
		szMaterialType = m_szBinSheetTypePRM;
		if ((szMaterialID == m_szColletType) || (szMaterialID == m_szCollet2Type) || (szMaterialID == m_szNeedleType))
		{
			m_szErrMsg = _T("Registerted Material file currently in use!");
			return FALSE;
		}
		break;
	default:
		if ((szMaterialID == m_szCollet2Type) || (szMaterialID == m_szNeedleType) || (szMaterialID == m_szBinSheetType))
		{
			m_szErrMsg = _T("Registerted Material file currently in use!");
			return FALSE;
		}
		break;
	}


	CreateDirectory(szlocalRMURDir, NULL);
	//{
	//	if ( GetLastError() != ERROR_ALREADY_EXISTS )
	//	{
	//		m_szErrMsg = _T("Nichia RUMR local path cannot be created: ") + szlocalRMURDir;
	//		return FALSE;
	//	}
	//}

	if (!CopyFile(szFullFileName, szlocalRMURFile, FALSE))
	{
		m_szErrMsg = _T("Fails to copy RMUR file from netork to local HD - ") + szMaterialID;
		return FALSE;
	}

	//Try to retrieve material data from file
	CStdioFile oRawMatlUsageFile;
	if (!oRawMatlUsageFile.Open(szlocalRMURFile, CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		m_szErrMsg = _T("Nichia RMUR file cannot be opened at: ") + szFullFileName;
		return FALSE;
	}


	CString szLimit, szExpireDays;
	CString szLine;

	oRawMatlUsageFile.ReadString(szLine);				//1. Raw Material ID
	oRawMatlUsageFile.ReadString(szLine);				//2. Manufacture Date
	
	oRawMatlUsageFile.ReadString(szLine);				//3. Material Type
	CString szTypeKey;
	szTypeKey.Format("%s", ucaRMUMType);
	CString szMType = szLine.Mid(szLine.Find(",") + 1);
	CString szMTypeNum = szMType.Mid(szMType.Find(szTypeKey) + 6);
//AfxMessageBox("Material Type = " + szMType, MB_SYSTEMMODAL);
//AfxMessageBox("Material Type Num = " + szMTypeNum, MB_SYSTEMMODAL);

	if (szMTypeNum != szMaterialType)
	{
		m_szErrMsg = _T("Nichia RMUR Material Type is not matched: ") + szMTypeNum + _T(" - ") + szMaterialType;
		return FALSE;
	}


	oRawMatlUsageFile.ReadString(szLine);				//4. Limit
//AfxMessageBox("Limit Line = " + szLine, MB_SYSTEMMODAL);
	szLimit = szLine.Mid(szLine.Find(",") + 1);
	lMaterialLimit = atol(szLimit);

	oRawMatlUsageFile.ReadString(szLine);				//5. Expire Days
	szExpireDays = szLine.Mid(szLine.Find(",")+1);
	lExpireDays = atol(szExpireDays);

	BOOL bStatus = TRUE;
	LONG lLineCount = 0;
	CString szLastLine;
	while (oRawMatlUsageFile.ReadString(szLine))
	{
		lLineCount++;
		if (szLine.GetLength() > 0)
			szLastLine = szLine;
	}
	
	oRawMatlUsageFile.Close();

//AfxMessageBox("Last Line = " + szLastLine, MB_SYSTEMMODAL);
	LONG lLastCount = 0;
	if (szLastLine.Find(",") != -1)
	{
		CString szLastCount = szLastLine.Left(szLastLine.Find(","));
		lLastCount = atol(szLastCount);
		if ( (lLastCount < 0) || (lLastCount > 9999999) )
			lLastCount = 0;
	}
	lCurrCount = lLastCount;	//Retrieve last record counter from file

	CString szLog;
	szLog.Format("Limit=%ld; Count=%ld; ExpDays=%ld", lMaterialLimit, lLastCount, lExpireDays);
	szLog = "RMUR registered OK (Nichia): ID=" + szMaterialID + "; " + szLog;
	CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);

	if (lLastCount >= lMaterialLimit)
	{
		bStatus = FALSE;
		m_szErrMsg.Format("Current material count (%ld)  already exceeds its limit (%ld)" , lLastCount, lMaterialLimit);
	}
	if (lMaterialLimit == 0)
	{
		bStatus = FALSE;
		m_szErrMsg.Format("Material limit (%ld) is not valid", lMaterialLimit);
	}

	CTime theTime			= CTime::GetCurrentTime();
	CString szCurrTime		= theTime.Format("%Y/%m/%d %H:%M:%S");

	if (bStatus)
	{
		switch (ucType)
		{
		case 1:	
			UpdateRawMaterialList(1);
			m_szLastCollet2Type		= m_szCollet2Type;
			m_szTimeCollet2Start	= _T("");	//m_szTimeSortingStart2;	//szCurrTime;		//v4.41T3
			break;
		case 2:
			UpdateRawMaterialList(2);
			m_szLastNeedleType		= m_szNeedleType;
			m_szTimeNeedleStart		= _T("");	//m_szTimeSortingStart2;	//szCurrTime;		//v4.41T3
			break;
		case 3:
			UpdateRawMaterialList(3);
			m_szLastBinSheetType	= m_szBinSheetType;
			break;
		default:
			UpdateRawMaterialList(0);
			m_szLastColletType		= m_szColletType;
			m_szTimeColletStart		= _T("");	//m_szTimeSortingStart2;	//szCurrTime;		//v4.41T3
			break;
		}

		//SaveData();		//v4.41T4
	}

	return bStatus;
}

BOOL CMSNichiaMgntSubSystem::CheckRawMaterialExpireDate(CONST CString szMaterialID)
{
	if (!m_bIsEnabled)
	{
		m_szErrMsg = _T("Nichia sub-system is not enabled");
		return TRUE;
	}

	//54321
	if (szMaterialID.GetLength() == 0)
	{
		m_szErrMsg = _T("Nichia RMUR Material ID is not valid : ") + szMaterialID;
		return FALSE;
	}

	//Try to access if material usage record is available
	CString szFullFileName	= m_szFilePath_RawMaterialMgntSys + "\\" + szMaterialID + MS_NICHIA_RMUR_FILE_EXTENSION;
	CString szlocalRMURFile = gszUSER_DIRECTORY + "\\RMUR\\" + szMaterialID + MS_NICHIA_RMUR_FILE_EXTENSION;
	CString szlocalRMURDir	= gszUSER_DIRECTORY + "\\RMUR";
	
	if (_access(szlocalRMURFile, 0) == -1)
	{
		m_szErrMsg = _T("Nichia RMUR file is not accessible at: ") + szlocalRMURFile;
		return FALSE;
	}


	//Try to retrieve material data from file
	CStdioFile oRawMatlUsageFile;
	if (!oRawMatlUsageFile.Open(szlocalRMURFile, CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		m_szErrMsg = _T("Nichia RMUR file cannot be opened at: ") + szlocalRMURFile;
		return FALSE;
	}


	CString szLimit, szExpireDays;
	CString szLine;

	oRawMatlUsageFile.ReadString(szLine);				//1. Raw Material ID
	oRawMatlUsageFile.ReadString(szLine);				//2. Manufacture Date
	oRawMatlUsageFile.ReadString(szLine);				//3. Material Type
	oRawMatlUsageFile.ReadString(szLine);				//4. Limit

	oRawMatlUsageFile.ReadString(szLine);				//5. Expire Days
//AfxMessageBox("Expire Date = " + szLine, MB_SYSTEMMODAL);
	szExpireDays = szLine.Mid(szLine.Find(",")+1);
//AfxMessageBox("Expire Date 2 = " + szExpireDays, MB_SYSTEMMODAL);
	LONG lExpireDays = atol(szExpireDays);

	oRawMatlUsageFile.ReadString(szLine);				//6. Header
	

	INT nIndex = 0;
	CString szDateAndTime, szTemp;
	CString szTime = _T("");
	INT nCount = 0;

	while (oRawMatlUsageFile.ReadString(szLine))		//7. 1st record & after
	{
		nCount++;
		if (szLine.GetLength() == 0)
			continue;
		nIndex = szLine.Find(",");
		if (nIndex == -1)
			continue;

		szLine = szLine.Mid(szLine.Find(",") + 1);
		
		nIndex = szLine.Find(",");
		if (nIndex == -1)
			continue;

		szDateAndTime = szLine.Left(nIndex);

		szTemp.Format("Line #%d: dateandtime = " + szDateAndTime, nCount);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		if (szDateAndTime.GetLength() > 0)
		{
			szTime = szLine.Left(szLine.Find(" "));
			break;
		}
	}

	oRawMatlUsageFile.Close();

//AfxMessageBox("Final Date to check Expire date = " + szTime, MB_SYSTEMMODAL);
	if (szTime.GetLength() == 0)
	{
		//No previous record found, so no need to check expire date!
		return TRUE;
	}
	if (szTime.Find("/") == -1)
	{
		m_szErrMsg.Format("Material ID = %s Expire-Date checking detects invalid date = " + szTime, szMaterialID);
		return FALSE;
	}

	CString szYear	= szTime.Left(szTime.Find("/"));
	szTime = szTime.Mid(szTime.Find("/") + 1);
	CString szMonth = szTime.Left(szTime.Find("/"));
	szTime = szTime.Mid(szTime.Find("/") + 1);
	CString szDay = szTime;

	INT nYear	= atoi(szYear);
	INT nMonth	= atoi(szMonth);
	INT nDay	= atoi(szDay);
	CTime tStartTime(nYear, nMonth, nDay, 0,0,0);
	CTimeSpan cTimeDiff = CTime::GetCurrentTime() - tStartTime;

	CString szLog;
	szLog.Format("Nichia RMUR Expire Date checking; ID = %s; Start Date = %d/%d/%d; ExpireDays = %ld; diff=%d",
					szMaterialID, nYear, nMonth, nDay, lExpireDays, cTimeDiff.GetDays());
	CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);

	if (cTimeDiff.GetDays() > lExpireDays)
	{
		m_szErrMsg.Format("Material ID = %s is expired by %d days (%d)", szMaterialID, cTimeDiff.GetDays(), lExpireDays);
		return FALSE;
	}

	return TRUE;
}


BOOL CMSNichiaMgntSubSystem::RestoreLastMaterialType(CONST UCHAR ucType)
{
	switch (ucType)
	{
	case 1:	
		m_szCollet2Type	= m_szLastCollet2Type;
		break;
	case 2:
		m_szNeedleType	= m_szLastNeedleType;
		break;
	case 3:
		m_szBinSheetType= m_szLastBinSheetType;
		break;
	default:
		m_szColletType	= m_szLastColletType;
		break;
	}

	SaveData();
	return TRUE;
}

BOOL CMSNichiaMgntSubSystem::ResetLastMaterialType(CONST UCHAR ucType)
{
	switch (ucType)
	{
	case 1:	
		m_szCollet2Type		= _T("");
		m_szTimeCollet2Start= _T("");
		break;
	case 2:
		m_szNeedleType		= _T("");
		m_szTimeNeedleStart	= _T("");
		break;
	case 3:
		m_szBinSheetType	= _T("");
		break;
	default:
		m_szColletType		= _T("");
		m_szTimeColletStart = _T("");
		break;
	}
	return TRUE;
}

BOOL CMSNichiaMgntSubSystem::AddRawMaterialUsageRecord(CONST CString szMaterialID, CONST LONG lCurrCount)
{
	if (!m_bIsEnabled)
	{
		m_szErrMsg = _T("Nichia sub-system is not enabled");
		return FALSE;
	}
	if (m_szFilePath_RawMaterialMgntSys.GetLength() == 0)
	{
		m_szErrMsg = _T("Nichia Raw-Material Usage Record path is not available");
		return FALSE;
	}
	if (m_psmfSRam == NULL)
	{
		m_szErrMsg = _T("Nichia Static RAM memory is not accessible");
		return FALSE;
	}


	CString szFullFileName = m_szFilePath_RawMaterialMgntSys + "\\" + szMaterialID + MS_NICHIA_RMUR_FILE_EXTENSION;
	CString szlocalRMURFile = gszUSER_DIRECTORY + "\\RMUR\\" + szMaterialID + MS_NICHIA_RMUR_FILE_EXTENSION;
	CString szlocalRMURDir	= gszUSER_DIRECTORY + "\\RMUR";

	if (_access(szlocalRMURFile, 0) == -1)
	{
		m_szErrMsg = _T("Nichia RMUR file is not accessible at: ") + szlocalRMURFile;
		return FALSE;
	}
	if (_access(szlocalRMURFile, 2) == -1)
	{
		m_szErrMsg = _T("Nichia RMUR file is not writable at: ") + szlocalRMURFile;
		return FALSE;
	}

	CSingleLock slLock(&m_CsRMUR);
	slLock.Lock();
	
	CStdioFile oRawMatlUsageFile;
	if (!oRawMatlUsageFile.Open(szlocalRMURFile, CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		m_szErrMsg = _T("Nichia RMUR file cannot be opened at: ") + szFullFileName;
		slLock.Unlock();
		return FALSE;
	}

	oRawMatlUsageFile.SeekToEnd();

	CString szMachineName	= (*m_psmfSRam)["MS896A"]["MachineNo"];
	//CString szMachineName	= (*m_psmfSRam)["MS896A"]["MachineName"];
	CString szLotID			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];	
								//(*m_psmfSRam)["MS896A"]["LotNumber"];

	CString szStartDateAndTime	= _T("");	//m_szTimeSortingStart2;	//nichia004
	if (szMaterialID == m_szCollet2Type)
	{
		if (m_szTimeCollet2Start.GetLength() > 0)
			szStartDateAndTime	= m_szTimeCollet2Start;		//If matl register in this supply lot
		else
			szStartDateAndTime	= m_szTimeSortingStart;		//else use default T8 if not register within this supply lot
	}
	else if (szMaterialID == m_szNeedleType)
	{
		if (m_szTimeNeedleStart.GetLength() > 0)
			szStartDateAndTime	= m_szTimeNeedleStart;		//If matl register in this supply lot
		else
			szStartDateAndTime	= m_szTimeSortingStart;		//else use default T8 if not register within this supply lot
	}
	else if (szMaterialID == m_szColletType)
	{
		if (m_szTimeColletStart.GetLength() > 0)
			szStartDateAndTime	= m_szTimeColletStart;		//If matl register in this supply lot
		else
			szStartDateAndTime	= m_szTimeSortingStart;		//else use default T8 if not register within this supply lot
	}

	CTime theTime			= CTime::GetCurrentTime();
	CString szCurrTime		= theTime.Format("%Y/%m/%d %H:%M:%S");
/*	
	if (theTime.GetMonth() < 10)
	{
		if (theTime.GetDay() < 10)
			szCurrTime	= theTime.Format("%Y/0%m/0%d %H:%M:%S");
		else
			szCurrTime	= theTime.Format("%Y/0%m/%d %H:%M:%S");
	}
	else if (theTime.GetDay() < 10)
	{
		szCurrTime	= theTime.Format("%Y/0%m/0%d %H:%M:%S");
	}
*/

	CString szCount;
	szCount.Format("%ld", lCurrCount);

	CString szLine	= szCount + "," + 
					  szStartDateAndTime + "," + 
					  szCurrTime + "," + 
					  szMachineName + "," + 
					  szLotID;

	oRawMatlUsageFile.WriteString(szLine + "\n");
	CMSLogFileUtility::Instance()->MS_LogCtmOperation("Nichia RMUR Update: " + szLine);
	oRawMatlUsageFile.Close();


	if (_access(szFullFileName, 0) == -1)
	{
		m_szErrMsg = _T("Nichia RMUR file is not accessible at: ") + szFullFileName;
		slLock.Unlock();
		return FALSE;
	}

	if (!CopyFile(szlocalRMURFile, szFullFileName, FALSE))
	{
		m_szErrMsg.Format("Fails to upload RMUR file (%s) from local HD to network: " + szFullFileName, szMaterialID);
		slLock.Unlock();
		return FALSE;
	}
	else
	{
		m_szErrMsg.Format("RMUR file (%s) uploaded from %s to " + szFullFileName, szMaterialID, szlocalRMURFile);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(m_szErrMsg);
		//DeleteFile(szlocalRMURFile);
	}

	slLock.Unlock();
	return TRUE;
}


BOOL CMSNichiaMgntSubSystem::LogOpState(CONST UINT unCode, CONST UCHAR ucLTSignal)
{
	if (!m_bIsEnabled)
		return FALSE;
	if (!m_bIsOpStateLogEnabled)
		return TRUE;

	m_szErrMsg = _T("");

	CSingleLock slLock(&m_CsStateLog);
	slLock.Lock();

	CString szStateLogFileName;

	//ucLTSignal:  1=Green, 2=RED, 3=Yellow, 4=Blink Yellow
	CString szCode, szStateCode;
	szCode.Format("%d", ucLTSignal);
	szStateCode.Format("%d", unCode);
	if (szStateCode.GetLength() < 7)
	{
		for (INT i=0; i<=(7-szStateCode.GetLength()); i++)
		{
			szStateCode = "0" + szStateCode;
		}
	}
	szStateLogFileName = szCode + szStateCode;
	CString szFullFileName = m_szFilePath_OpStateLog + "\\" + szStateLogFileName;
	

	CMSLogFileUtility::Instance()->MS_LogCtmOperation("Nichia: Write OpState Log at: " + szFullFileName);

	//Delete all previous StateLog files if present in the folder
	if (_access(m_szFilePath_OpStateLog, 0) != -1)
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		pApp->SearchAndRemoveFiles(m_szFilePath_OpStateLog, 30, TRUE);
		//CreateDirectory(szPBTopFolder, NULL);
	}
	if (_access(m_szFilePath_OpStateLog, 0) == -1)
	{
		CreateDirectory(m_szFilePath_OpStateLog, NULL);
	}

	CStdioFile oStateLogFile;
	if (!oStateLogFile.Open(szFullFileName, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		m_szErrMsg = _T("Nichia StateLog file cannot be opened at: ") + szFullFileName;
		slLock.Unlock();
		return FALSE;
	}
	
	oStateLogFile.Close();
	slLock.Unlock();
	return TRUE;
}


BOOL CMSNichiaMgntSubSystem::WriteMachineLog(CONST BOOL bReset, CONST CString szCode)
{
	CString szTemp;
	m_szErrMsg = _T("");

	if (!m_bIsEnabled)
	{
		m_szErrMsg = _T("Nichia sub-system is not enabled");
		return FALSE;
	}
	if (m_szFilePath_MachineLog.GetLength() == 0)
	{
		m_szErrMsg = _T("Nichia Machine Log path is not available; path = ") + m_szFilePath_MachineLog;
		return FALSE;
	}
	if (m_psmfSRam == NULL)
	{
		m_szErrMsg = _T("Nichia Static RAM memory is not accessible");
		return FALSE;
	}
	if (m_szTimeWaferAccess.GetLength() == 0)
	{
		m_szErrMsg = _T("Nichia Machine Log path is not generated because wafer Access time is not valid - ")+ m_szTimeWaferAccess;
		return FALSE;
	}

	MLog_LogSortTime(FALSE);	

	CString szMachineNo		= (*m_psmfSRam)["MS896A"]["MachineNo"];
	CString szMachineName	= (*m_psmfSRam)["MS896A"]["MachineName"];
	
	//Use waferAccess time to generate machine log file name	//nichia005
	CTime tFileTime			= CTime::GetCurrentTime();
	if (!GetTimeFromNichiaTimeFormat(m_szTimeWaferAccess, tFileTime))
	{
		m_szErrMsg = _T("Nichia Wafer Access time is not valid in Machine Log generation - " + m_szTimeWaferAccess);
		return FALSE;
	}

	CString szTime			= tFileTime.Format("%y%m_");
	CString szFileName		= szTime + szMachineName + "_" + szMachineNo;
	CString szFullFileName	= m_szFilePath_MachineLog + "\\" + szFileName + MS_NICHIA_MACHINELOG_FILE_EXTENSION;

	//CMSLogFileUtility::Instance()->MS_LogCtmOperation("Nichia: Write Machine Log at: " + szFullFileName);

	CSingleLock slLock(&m_CsMachineLog);
	slLock.Lock();

	CStdioFile oMachineLogFile;
	if (!oMachineLogFile.Open(szFullFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		m_szErrMsg = _T("Nichia Machine Log file cannot be opened at: ") + szFullFileName;
		slLock.Unlock();
		return FALSE;
	}


	CString szLotNo					= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];	//["MS896A"]["LotNumber"];
	szLotNo							= szLotNo				+ _T(","); 
	CString szTimeWaferAccess		= m_szTimeWaferAccess	+ _T(",");		//#2
	CString szTimeWaferLoad			= m_szTimeWaferLoad		+ _T(",");		//#3
	CString szTimeWaferInfoAcq		= m_szTimeWaferInfoAcq	+ _T(",");		//#4
	CString szTimeMapAccess			= m_szTimeMapAccess		+ _T(",");		//#5
	CString szTimeMappingStart		= m_szTimeMappingStart	+ _T(",");		//#6
	CString szTimeMappingEnd		= m_szTimeMappingEnd	+ _T(",");		//#7
	CString szTimeSortingStart		= m_szTimeSortingStart	+ _T(",");		//#8
	CString szTimeSortingEnd		= m_szTimeSortingEnd	+ _T(",");		//#9
	CString szTimeWaferUnload		= m_szTimeWaferUnload	+ _T(",");		//#10
	CString szTimeMotionEnd			= m_szTimeMotionEnd		+ _T(",");		//#11

	CString szTimeTact				= _T(",");								//#12
	
	if (m_szTotalDownTimeBeforeMidNightInSec.GetLength() == 0)
		m_szTotalDownTimeBeforeMidNightInSec = _T("0");
	CString szDownTimeBeforeMNight	= m_szTotalDownTimeBeforeMidNightInSec	+ _T(",");	//#13	
	
	if (m_szTotalDownTimeAfterMidNightInSec.GetLength() == 0)
		m_szTotalDownTimeAfterMidNightInSec = _T("0");
	CString szDownTimeAfterMNight	= m_szTotalDownTimeAfterMidNightInSec	+ _T(",");	//#14
	
	if (m_szTotalSortTimeBeforeMidNightInSec.GetLength() == 0)
		m_szTotalSortTimeBeforeMidNightInSec = _T("0");
	CString szSortTimeBeforeMNight	= m_szTotalSortTimeBeforeMidNightInSec	+ _T(",");	//#15						

	if (m_szTotalSortTimeAfterMidNightInSec.GetLength() == 0)
		m_szTotalSortTimeAfterMidNightInSec = _T("0");
	CString szSortTimeAfterMNight	= m_szTotalSortTimeAfterMidNightInSec	+ _T(",");	//#16						


	CString szNoOfGoodDicesOnMap	= (*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGdMapDices"];	
	LONG lNoOfGdDicesOnMap			= atol(szNoOfGoodDicesOnMap);
	szNoOfGoodDicesOnMap			= szNoOfGoodDicesOnMap + _T(",");		//#17
	CString szNoOfGoodDicesInAB		= (*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGdDicesForSort"];	//After Prescan
	szNoOfGoodDicesInAB				= szNoOfGoodDicesInAB + _T(",");		//#18
	CString szNoOfGoodDicesSorted	= (*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfSortedDices"];
	szNoOfGoodDicesSorted			= szNoOfGoodDicesSorted + _T(",");		//#19
	CString szNoOfGrades			= (*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGrades"];
	szNoOfGrades					= szNoOfGrades + _T(",");				//#20
	CString szNoOfBinTraysUsed		= _T(",");			
	szNoOfBinTraysUsed.Format("%ld,",	m_lBinTrayCount);					//#21
	CString szNoOfErrors			= _T(",");			
	szNoOfErrors.Format("%ld",			m_lErrorCount);						//#22


	CTime tTimeWaferAccess, tTimeWaferUnload, tTimeMotionEnd;
	BOOL bTime2  = GetTimeFromNichiaTimeFormat(m_szTimeWaferAccess, tTimeWaferAccess);
	BOOL bTime10 = GetTimeFromNichiaTimeFormat(m_szTimeWaferUnload,	tTimeWaferUnload);
	BOOL bTime11 = GetTimeFromNichiaTimeFormat(szTimeMotionEnd,		tTimeMotionEnd);
	if (bTime2 && bTime10)
	{
		DOUBLE dTact = 0;
		CTimeSpan TimeDiff;
		TimeDiff	= tTimeWaferUnload - tTimeWaferAccess;

		szTemp.Format("Tact Total sec=%ld, Dice=%ld", TimeDiff.GetTotalSeconds(), lNoOfGdDicesOnMap);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		if (lNoOfGdDicesOnMap > 0)
		{
			dTact = 1000.0 * TimeDiff.GetTotalSeconds() / lNoOfGdDicesOnMap;
		}
		else
		{
			dTact = 0;
		}
		szTimeTact.Format("%.2f", dTact);
		szTimeTact = szTimeTact + _T(",");
	}

//CString szTemp2;
//szTemp2.Format("WriteMachineLog: %d %d - " + m_szTimeWaferAccess, bTime2, bTime11);
//CMSLogFileUtility::Instance()->MS_LogCtmOperation(szTemp2);

	if (bTime2 && bTime11)
	{
		if (tTimeMotionEnd.GetDay() == tTimeWaferAccess.GetDay())
		{
			CTimeSpan TimeDiff;
			TimeDiff	= tTimeMotionEnd - tTimeWaferAccess;

			LONG lTotalSec = (LONG)TimeDiff.GetTotalSeconds();	// - atol((LPCTSTR) m_szTotalSortTimeBeforeMidNightInSec);
			LONG lTotalSortSec = atol((LPCTSTR) m_szTotalSortTimeBeforeMidNightInSec) - 
										atol((LPCTSTR) m_szTotalDownTimeBeforeMidNightInSec);
			LONG lTotalDownSec = lTotalSec - lTotalSortSec;
			if (lTotalDownSec < 0)
				lTotalDownSec = 0;

			m_szTotalSortTimeBeforeMidNightInSec.Format("%ld", lTotalSortSec);
			m_szTotalSortTimeAfterMidNightInSec = _T("0");
			m_szTotalDownTimeBeforeMidNightInSec.Format("%ld", lTotalDownSec);
			m_szTotalDownTimeAfterMidNightInSec = _T("0");
		}
		else
		{
			CTime tMidNight(tTimeMotionEnd.GetYear(), 
							tTimeMotionEnd.GetMonth(),
							tTimeMotionEnd.GetDay(),
							0,0,0);

			CTimeSpan TimeDiffBeforeMNight;
			TimeDiffBeforeMNight	= tMidNight - tTimeWaferAccess;
			CTimeSpan TimeDiffAfterMNight;
			TimeDiffAfterMNight		= tTimeMotionEnd - tMidNight;

			LONG lTotalSecBefore	= (LONG)TimeDiffBeforeMNight.GetTotalSeconds();
			LONG lTotalSortBefore = atol((LPCTSTR) m_szTotalSortTimeBeforeMidNightInSec) - 
										atol((LPCTSTR) m_szTotalDownTimeBeforeMidNightInSec);
			LONG lTotalDownBefore = lTotalSecBefore - lTotalSortBefore;
			if (lTotalDownBefore < 0)
				lTotalDownBefore = 0;
			
			m_szTotalDownTimeBeforeMidNightInSec.Format("%ld", lTotalDownBefore);
			m_szTotalSortTimeBeforeMidNightInSec.Format("%ld", lTotalSortBefore);

			LONG lTotalSecAfter		= (LONG)TimeDiffAfterMNight.GetTotalSeconds();
			LONG lTotalSortAfter	= atol((LPCTSTR) m_szTotalSortTimeAfterMidNightInSec) - 
										atol((LPCTSTR) m_szTotalDownTimeAfterMidNightInSec);
			LONG lTotalDownAfter	= lTotalSecAfter - lTotalSortAfter;
			if (lTotalDownAfter < 0)
				lTotalDownAfter = 0;
			
			m_szTotalDownTimeAfterMidNightInSec.Format("%ld",  lTotalDownAfter);
			m_szTotalSortTimeAfterMidNightInSec.Format("%ld",  lTotalSortAfter);
		}

		szDownTimeBeforeMNight	= m_szTotalDownTimeBeforeMidNightInSec	+ _T(",");	//#13						
		szDownTimeAfterMNight	= m_szTotalDownTimeAfterMidNightInSec	+ _T(",");	//#14
		szSortTimeBeforeMNight	= m_szTotalSortTimeBeforeMidNightInSec	+ _T(",");	//#15						
		szSortTimeAfterMNight	= m_szTotalSortTimeAfterMidNightInSec	+ _T(",");	//#16						
	}


	CString szLine;
	szLine = szLotNo + 
				szTimeWaferAccess +			//2
				szTimeWaferLoad +			//3
				szTimeWaferInfoAcq +		//4
				szTimeMapAccess +			//5
				szTimeMappingStart +		//6
				szTimeMappingEnd +			//7
				szTimeSortingStart +		//8
				szTimeSortingEnd +			//9
				szTimeWaferUnload +			//10
				szTimeMotionEnd +			//11
				szTimeTact +				//12
				szDownTimeBeforeMNight +	//13
				szDownTimeAfterMNight +		//14
				szSortTimeBeforeMNight +	//15
				szSortTimeAfterMNight +		//16
				szNoOfGoodDicesOnMap +		//17
				szNoOfGoodDicesInAB +		//18
				szNoOfGoodDicesSorted +		//19
				szNoOfGrades +				//20
				szNoOfBinTraysUsed +		//21
				szNoOfErrors +				//22
				"\n";

	oMachineLogFile.SeekToEnd();
	oMachineLogFile.WriteString(szLine);
	oMachineLogFile.Close();


	if (bReset)
	{
		m_szTimeWaferAccess		= _T("");				//2
		m_szTimeWaferLoad		= _T("");				//3
		m_szTimeWaferInfoAcq	= _T("");				//4
		m_szTimeMapAccess		= _T("");				//5
		m_szTimeMappingStart	= _T("");				//6
		m_szTimeMappingEnd		= _T("");				//7
		m_szTimeSortingStart	= _T("");				//8
		m_szTimeSortingStart2	= _T("");				//8.2
		m_szTimeSortingEnd		= _T("");				//9
		m_szTimeWaferUnload		= _T("");				//10
		m_szTimeMotionEnd		= _T("");				//11
		m_szTotalDownTimeBeforeMidNightInSec= _T("");	//13
		m_szTotalDownTimeAfterMidNightInSec	= _T("");	//14
		m_szTotalSortTimeBeforeMidNightInSec= _T("");	//15
		m_szTotalSortTimeAfterMidNightInSec	= _T("");	//16
		m_lBinTrayCount			= 0;					//21
		m_lErrorCount			= 0;					//22

		(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGdMapDices"]		= _T("");
		(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGdDicesForSort"]	= _T("");
		(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfSortedDices"]		= _T("");
		(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGrades"]			= _T("");

		m_tSortTimeStart = CTime::GetCurrentTime();
		m_bSortTimeStart = FALSE;

		m_szTimeColletStart		= _T("");
		m_szTimeCollet2Start	= _T("");
		m_szTimeNeedleStart		= _T("");

		SaveData();
	}

	slLock.Unlock();
	szTemp.Format("Machine Log updated - %d (ERR = %s)", bReset, szCode);	//nichia004
	CMSLogFileUtility::Instance()->MS_LogCtmOperation(szTemp);
	return TRUE;
}

VOID CMSNichiaMgntSubSystem::UpdateMachineLog_Time(CONST UCHAR ucLogItem)
{
	if (!m_bIsEnabled)
	{
		m_szErrMsg = _T("Nichia sub-system is not enabled");
		return;
	}
	
	BOOL bNeedReset = FALSE;
	CSingleLock slLock(&m_CsMachineLog);
	slLock.Lock();

	switch (ucLogItem)
	{
	case 2:	
		m_szTimeWaferAccess		= GetCurrTimeInNichiaTimeFormat();
		bNeedReset = TRUE;
		break;
	case 3:	
		m_szTimeWaferLoad		= GetCurrTimeInNichiaTimeFormat();
		break;
	case 4:	
		m_szTimeWaferInfoAcq	= GetCurrTimeInNichiaTimeFormat();
		break;
	case 5:	
		m_szTimeMapAccess		= GetCurrTimeInNichiaTimeFormat();
		break;
	case 6:	
		m_szTimeMappingStart	= GetCurrTimeInNichiaTimeFormat();
		break;
	case 7:	
		m_szTimeMappingEnd		= GetCurrTimeInNichiaTimeFormat();
		break;
	case 8:	
		m_szTimeSortingStart	= GetCurrTimeInNichiaTimeFormat();
		m_szTimeSortingStart2	= m_szTimeSortingStart;				//Initially, Start2 = Start in AUTO mode
		if ( (m_szColletType.GetLength() > 0) &&  (m_szTimeColletStart == _T(""))  )	//v4.42T9
			m_szTimeColletStart = m_szTimeSortingStart2;
		if ( (m_szCollet2Type.GetLength() > 0) && (m_szTimeCollet2Start == _T("")) )							//v4.42T9
			m_szTimeCollet2Start = m_szTimeSortingStart2;
		if ( (m_szNeedleType.GetLength() > 0) &&  (m_szTimeNeedleStart == _T(""))  )							//v4.42T9
			m_szTimeNeedleStart = m_szTimeSortingStart2;
		break;
	case 9:	
		m_szTimeSortingEnd		= GetCurrTimeInNichiaTimeFormat();
		break;
	case 10:
		m_szTimeWaferUnload		= GetCurrTimeInNichiaTimeFormat();
		break;
	case 11:
		m_szTimeMotionEnd		= GetCurrTimeInNichiaTimeFormat();
		break;
	case 20:	
		m_szTimeSortingStart2	= GetCurrTimeInNichiaTimeFormat();	//When OP press STOP/START in middle of supply lot, this time will change
		if ( (m_szColletType.GetLength() > 0) &&  (m_szTimeColletStart == _T(""))  )	//v4.42T9
			m_szTimeColletStart = m_szTimeSortingStart2;
		if ( (m_szCollet2Type.GetLength() > 0) && (m_szTimeCollet2Start == _T("")) )							//v4.42T9
			m_szTimeCollet2Start = m_szTimeSortingStart2;
		if ( (m_szNeedleType.GetLength() > 0) &&  (m_szTimeNeedleStart == _T(""))  )							//v4.42T9
			m_szTimeNeedleStart = m_szTimeSortingStart2;
		break;

	default:
		break;
	}

	if (bNeedReset)
	{
		m_szTimeWaferLoad		= _T("");					//3
		m_szTimeWaferInfoAcq	= _T("");					//4
		m_szTimeMapAccess		= _T("");					//5
		m_szTimeMappingStart	= _T("");					//6
		m_szTimeMappingEnd		= _T("");					//7
		m_szTimeSortingStart	= _T("");					//8
		m_szTimeSortingStart2	= _T("");					//8.2
		m_szTimeSortingEnd		= _T("");					//9
		m_szTimeWaferUnload		= _T("");					//10
		m_szTimeMotionEnd		= _T("");					//11
		//m_szTotalDownTimeBeforeMidNightInSec	= _T("");	//13
		//m_szTotalDownTimeAfterMidNightInSec	= _T("");	//14
		//m_szTotalSortTimeBeforeMidNightInSec	= _T("");	//15
		//m_szTotalSortTimeAfterMidNightInSec	= _T("");	//16
		m_lBinTrayCount			= 0;						//21
		m_lErrorCount			= 0;						//22

		(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGdMapDices"]		= _T("");
		(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGdDicesForSort"]	= _T("");
		(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfSortedDices"]		= _T("");
		(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGrades"]			= _T("");

		//m_tSortTimeStart = CTime::GetCurrentTime();
		//m_bSortTimeStart = FALSE;
	}

	SaveData();
	slLock.Unlock();
}

	
CString CMSNichiaMgntSubSystem::GetMachineLog_Time(CONST UCHAR ucLogItem)
{
	CString szDate = _T("");

	if (!m_bIsEnabled)
	{
		m_szErrMsg = _T("GetMachineLog_Time: Nichia sub-system is not enabled");
		return szDate;
	}
	
	switch (ucLogItem)
	{
	case 2:	
		szDate	= m_szTimeWaferAccess;
		break;
	case 3:	
		szDate	= m_szTimeWaferLoad;
		break;
	case 4:	
		szDate	= m_szTimeWaferInfoAcq;
		break;
	case 5:	
		szDate	= m_szTimeMapAccess;
		break;
	case 6:	
		szDate	= m_szTimeMappingStart;
		break;
	case 7:	
		szDate	= m_szTimeMappingEnd;
		break;
	case 8:	
		szDate	= m_szTimeSortingStart;
		break;
	case 9:	
		szDate	= m_szTimeSortingEnd;
		break;
	case 10:
		szDate	= m_szTimeWaferUnload;
		break;
	case 11:
		szDate	= m_szTimeMotionEnd;
		break;
	case 20:	
		szDate	= m_szTimeSortingStart2;
		break;
	default:
		szDate	= _T("");
		break;
	}
	return szDate;
}


VOID CMSNichiaMgntSubSystem::AddBinTrayCount()
{
	if (!m_bIsEnabled)
		return;
	//Increment by 1 whenever a frame is pulled to bin table
	m_lBinTrayCount = m_lBinTrayCount + 1;
}

VOID CMSNichiaMgntSubSystem::MLog_AddErrorCount(UINT unCode)
{
	if (!m_bIsEnabled)
		return;
	m_lErrorCount = m_lErrorCount + 1;

	//v4.43T10
	//CString szErrCode;
	//szErrCode.Format("ErrCode: %lu", unCode);
	//CMSLogFileUtility::Instance()->MS_LogCtmOperation(szErrCode);
}

VOID CMSNichiaMgntSubSystem::MLog_AddErrorCount(CONST CString szMsg)
{
	if (!m_bIsEnabled)
		return;
	m_lErrorCount = m_lErrorCount + 1;

	//v4.43T10
	//CString szErrCode;
	//szErrCode = "ErrCode: " + szMsg;
	//CMSLogFileUtility::Instance()->MS_LogCtmOperation(szErrCode);
}

BOOL CMSNichiaMgntSubSystem::MLog_LogSortTime(CONST BOOL bStart, BOOL bForceStart)
{
	if (!m_bIsEnabled)
		return TRUE;
	
	CSingleLock slLock(&m_CsSortTimeLog);
	slLock.Lock();

	if (bStart)
	{
		if (m_bSortTimeStart)
		{
			slLock.Unlock();
			return TRUE;
		}
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if ( !bForceStart && (pApp->m_bCycleStarted != TRUE) )
		{
			slLock.Unlock();
			return FALSE;
		}

CMSLogFileUtility::Instance()->MS_LogCtmOperation("MLog_LogSortTime: Start ....");
		m_bSortTimeStart = TRUE;
		m_tSortTimeStart = CTime::GetCurrentTime();
	}
	else
	{
		if (!m_bSortTimeStart)
		{
			slLock.Unlock();
			return TRUE;
		}

		CTime tStartLot;
		//Use WaferLoad time as StartLot time
		BOOL bStartLot = GetTimeFromNichiaTimeFormat(m_szTimeWaferLoad, tStartLot);
		
//CString szTemp3;
//szTemp3.Format("MLog_LogSortTime WaferLoad Time = (%d) - " + m_szTimeWaferLoad, bStartLot);
//CMSLogFileUtility::Instance()->MS_LogCtmOperation(szTemp3);

		CTime tCurrTime = CTime::GetCurrentTime();

		if (tCurrTime.GetDay() == m_tSortTimeStart.GetDay())
		{
			LONG lTotalSec = 0;
			CTimeSpan cTimeDiff;
			cTimeDiff = tCurrTime - m_tSortTimeStart;

			BOOL bAfterMidNight = FALSE;
			if (bStartLot)
			{
				if (tStartLot.GetDay() != tCurrTime.GetDay())		//If current sorting is AFTER midnight
					bAfterMidNight = TRUE;
			}
			if (tCurrTime.GetDay() != m_tSortTimeStart.GetDay())	//If sorting starts before miidnight
			{
				bAfterMidNight = TRUE;
			}


			if (bAfterMidNight)
			{
				if (m_szTotalSortTimeAfterMidNightInSec.GetLength() > 0)
					lTotalSec = atol((LPCTSTR) m_szTotalSortTimeAfterMidNightInSec);
				lTotalSec = lTotalSec + (LONG)cTimeDiff.GetTotalSeconds();
//CString szTemp2;
//szTemp2.Format("MLog_LogSortTime after Mnight 1: previous = %ld; add = %ld - " + m_szTotalSortTimeAfterMidNightInSec, 
//			   lTotalSec, cTimeDiff.GetTotalSeconds());
//CMSLogFileUtility::Instance()->MS_LogCtmOperation(szTemp2);

				m_szTotalSortTimeAfterMidNightInSec.Format("%ld", lTotalSec);
			}
			else
			{
				if (m_szTotalSortTimeBeforeMidNightInSec.GetLength() > 0)
					lTotalSec = atol((LPCTSTR) m_szTotalSortTimeBeforeMidNightInSec);
				lTotalSec = lTotalSec + (LONG)cTimeDiff.GetTotalSeconds();

//CString szTemp2;
//szTemp2.Format("MLog_LogSortTime before Mnight 1: previous = %ld; add = %ld - " + m_szTotalSortTimeBeforeMidNightInSec, 
//			   lTotalSec, cTimeDiff.GetTotalSeconds());
//CMSLogFileUtility::Instance()->MS_LogCtmOperation(szTemp2);

				m_szTotalSortTimeBeforeMidNightInSec.Format("%ld", lTotalSec);
			}
		}
		else
		{
			LONG lTotalSecBefore=0, lTotalSecAfter=0;
			CTimeSpan cTimeDiffBefore, cTimeDiffAfter;
			CTime tMidNight(tCurrTime.GetYear(), 
							tCurrTime.GetMonth(), 
							tCurrTime.GetDay(),
							0,0,0);

			cTimeDiffBefore = tMidNight - m_tSortTimeStart;
			cTimeDiffAfter	= tCurrTime - tMidNight;

			if (m_szTotalSortTimeBeforeMidNightInSec.GetLength() > 0)
				lTotalSecBefore = atol((LPCTSTR) m_szTotalSortTimeBeforeMidNightInSec);
			lTotalSecBefore = lTotalSecBefore + (LONG)cTimeDiffBefore.GetTotalSeconds();
			m_szTotalSortTimeBeforeMidNightInSec.Format("%ld", lTotalSecBefore);

//CString szTemp2;
//szTemp2.Format("MLog_LogSortTime before Mnight 2: previous = %ld; add = %ld - " + m_szTotalSortTimeBeforeMidNightInSec, 
//			   lTotalSecBefore, cTimeDiffBefore.GetTotalSeconds());
//CMSLogFileUtility::Instance()->MS_LogCtmOperation(szTemp2);

			if (m_szTotalSortTimeAfterMidNightInSec.GetLength() > 0)
				lTotalSecAfter = atol((LPCTSTR) m_szTotalSortTimeAfterMidNightInSec);
			lTotalSecAfter = lTotalSecAfter + (LONG)cTimeDiffAfter.GetTotalSeconds();
			m_szTotalSortTimeAfterMidNightInSec.Format("%ld", lTotalSecAfter);

//szTemp2.Format("MLog_LogSortTime after Mnight 2: previous = %ld; add = %ld - " + m_szTotalSortTimeAfterMidNightInSec, 
//			   lTotalSecAfter, cTimeDiffAfter.GetTotalSeconds());
//CMSLogFileUtility::Instance()->MS_LogCtmOperation(szTemp2);
		}


		LONG lTotalSec = 0;

		//Determine if it is BEFORE or AFTER mid-night

		m_bSortTimeStart = FALSE;
		SaveData();
	}
		
	slLock.Unlock();
	return TRUE;
}


BOOL CMSNichiaMgntSubSystem::MLog_LogDownTime(CONST BOOL bStart)
{
	if (!m_bIsEnabled)
		return TRUE;
/*
	CSingleLock slLock(&m_CsDownTimeLog);
	slLock.Lock();

	if (bStart)
	{
		if (m_bDownTimeStart)
		{
			slLock.Unlock();
			return TRUE;
		}
		m_bDownTimeStart	= TRUE;
		m_tDownTimeStart	= CTime::GetCurrentTime();
		//m_szDownTimeStart	= GetCurrTimeInNichiaTimeFormat();
	}
	else
	{
		if (!m_bDownTimeStart)
		{
			slLock.Unlock();
			return TRUE;
		}

		CTime tStartLot;
		BOOL bStartLot = FALSE;
		//Use WaferLoad time as StartLot time
		if (m_szTimeWaferLoad.GetLength() > 0)
			bStartLot = GetTimeFromNichiaTimeFormat(m_szTimeWaferLoad, tStartLot);
		
		CTime tCurrTime = CTime::GetCurrentTime();
		CTimeSpan cTimeDiff = tCurrTime - m_tDownTimeStart;
		LONG lTotalSec = 0;

		//Determine if it is BEFORE or AFTER mid-night
		if (tCurrTime.GetDay() == m_tDownTimeStart.GetDay())	//If sorting starts before miidnight
		{
			BOOL bAfterMidNight = FALSE;
			if (bStartLot)
			{
				if (tStartLot.GetDay() != tCurrTime.GetDay())		//If current sorting is AFTER midnight
					bAfterMidNight = TRUE;
			}

			if (bAfterMidNight)
			{
				if (m_szTotalDownTimeAfterMidNightInSec.GetLength() > 0)
					lTotalSec = atol((LPCTSTR) m_szTotalDownTimeAfterMidNightInSec);
				lTotalSec = lTotalSec + cTimeDiff.GetTotalSeconds();
				m_szTotalDownTimeAfterMidNightInSec.Format("%ld", lTotalSec);
			}
			else
			{
				if (m_szTotalDownTimeBeforeMidNightInSec.GetLength() > 0)
					lTotalSec = atol((LPCTSTR) m_szTotalDownTimeBeforeMidNightInSec);
				lTotalSec = lTotalSec + cTimeDiff.GetTotalSeconds();
				m_szTotalDownTimeBeforeMidNightInSec.Format("%ld", lTotalSec);
			}
		}
		else
		{
			LONG lTotalSecBefore=0, lTotalSecAfter=0;
			CTimeSpan cTimeDiffBefore, cTimeDiffAfter;
			CTime tMidNight(tCurrTime.GetYear(), 
							tCurrTime.GetMonth(), 
							tCurrTime.GetDay(),
							0,0,0);

			cTimeDiffBefore = tMidNight - m_tDownTimeStart;
			cTimeDiffAfter	= tCurrTime - tMidNight;

			if (m_szTotalDownTimeBeforeMidNightInSec.GetLength() > 0)
				lTotalSecBefore = atol((LPCTSTR) m_szTotalDownTimeBeforeMidNightInSec);
			lTotalSecBefore = lTotalSecBefore + cTimeDiffBefore.GetTotalSeconds();
			m_szTotalDownTimeBeforeMidNightInSec.Format("%ld", lTotalSecBefore);

			if (m_szTotalDownTimeAfterMidNightInSec.GetLength() > 0)
				lTotalSecAfter = atol((LPCTSTR) m_szTotalDownTimeAfterMidNightInSec);
			lTotalSecAfter = lTotalSecAfter + cTimeDiffAfter.GetTotalSeconds();
			m_szTotalDownTimeAfterMidNightInSec.Format("%ld", lTotalSecAfter);
		}

		m_bDownTimeStart = FALSE;
		SaveData();
	}
		
	slLock.Unlock();
*/
	return TRUE;
}


BOOL CMSNichiaMgntSubSystem::CheckBinSheetType(CONST CString szFrameBarCode)
{
	if (!m_bIsEnabled)
	{
		m_szErrMsg = _T("Nichia sub-system is not enabled");
		return TRUE;	//v4.40T9
	}
	if (m_szFilePath_BinSheet.GetLength() == 0)
	{
		m_szErrMsg = _T("Nichia Bin Sheet path is not available");
		return FALSE;
	}
	if (m_szBinSheetType.GetLength() == 0)
	{
		m_szErrMsg = _T("Nichia Bin Sheet type disabled");
		return TRUE;
	}

	CSingleLock slLock(&m_CsBinSheet);
	slLock.Lock();

	BOOL bStatus = TRUE;
	CString szFullFileName	= m_szFilePath_BinSheet + "\\" + szFrameBarCode + MS_NICHIA_RMUR_FILE_EXTENSION;

	CStdioFile oBinSheetFile;
	if (!oBinSheetFile.Open(szFullFileName, CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		m_szErrMsg.Format("Nichia BinSheet file %s cannot be opened at: " + szFullFileName, szFrameBarCode);
		slLock.Unlock();
		return FALSE;
	}

	CString szLine;
	oBinSheetFile.ReadString(szLine);

	//v4.42T12
/*
	if (szLine.Find(m_szBinSheetType) == -1)	//Check BInSheet code in file exist or not
	{
		bStatus = FALSE;
	}
*/
	if (m_szBinSheetType.Find(szLine) == -1)	//Check BInSheet code in file exist or not
	{
		bStatus = FALSE;
	}

	oBinSheetFile.Close();
	slLock.Unlock();
	return bStatus;
}

VOID CMSNichiaMgntSubSystem::UpdateRawMaterialList(CONST UCHAR ucType)
{
	CString szCollet1ID		= _T("");
	CString szCollet2ID		= _T("");
	CString szNeedleID		= _T("");
	CString szBinSheetID	= _T("");

	switch (ucType)
	{
	case 0:	
		szCollet1ID	= m_szColletType;
		break;
	case 1:	
		szCollet2ID	= m_szCollet2Type;
		break;
	case 2:
		szNeedleID	= m_szNeedleType;
		break;
	case 3:
		szBinSheetID= m_szBinSheetType;
		break;
	default:
		m_szRawMaterialList = _T("");
		szCollet1ID = m_szColletType;
		szCollet2ID = m_szCollet2Type;
		szNeedleID	= m_szNeedleType;
		break;
	}

	CString szList = szCollet1ID + "," + szCollet2ID + "," + szNeedleID;
	if (m_szRawMaterialList.GetLength() > 0)
		szList = "," + szList;

	m_szRawMaterialList = m_szRawMaterialList + szList;
	//SaveData();
}

UCHAR CMSNichiaMgntSubSystem::GetMapArrCode(CONST ULONG ulBinBlkID)
{
	UCHAR ucCode = 0;

	if (m_bIsEnabled && m_bEnableArrCodeFcn)
	{
		if (ulBinBlkID <= NC_BT_MAX_BINBLK_NO)
		{
			ucCode = (UCHAR)m_lMapBinArrCode[ulBinBlkID];
		}
	}
	return ucCode;
}

UCHAR CMSNichiaMgntSubSystem::GetBinArrCode(CONST ULONG ulBinBlkID)
{
	UCHAR ucCode = 0;

	if (m_bIsEnabled && m_bEnableArrCodeFcn)
	{
		if (ulBinBlkID <= NC_BT_MAX_BINBLK_NO)
		{
			ucCode = (UCHAR)m_lBinArrCode[ulBinBlkID];
		}
	}
	return ucCode;
}

BOOL CMSNichiaMgntSubSystem::UpdateMapArrCode(CONST CString szCodeString)
{
	CString szCode = szCodeString;
	if (!m_bIsEnabled)
		return FALSE;

	BOOL bUpdate = TRUE;
	if (szCode.GetLength() == 0)
	{
		bUpdate = FALSE;
	}
	INT nIndex = szCode.Find(",");
	if (nIndex == -1)
	{
		bUpdate = FALSE;
	}

	CString szBinArrCode, szTemp;
	LONG lBinCode = 0;
	INT i=1;

	if (bUpdate)
	{
		while (nIndex != -1)
		{
			szBinArrCode = szCode.Left(nIndex);
			szCode = szCode.Mid(nIndex + 1);

			lBinCode = atol((LPCTSTR) szBinArrCode);
			m_lMapBinArrCode[i] = lBinCode;

			i++;
			nIndex = szCode.Find(",");
		}

		if (szCode.GetLength() > 0)
		{
			lBinCode = atol((LPCTSTR) szCode);
			m_lMapBinArrCode[i] = lBinCode;
//szTemp.Format("Arr Code #%d added - %lu", i, m_lMapBinArrCode[i]);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
		}
	}

	for (INT m=i; m<=NC_BT_MAX_BINBLK_NO; m++)
	{
		m_lMapBinArrCode[m] = -1;
	}

	//SaveData();
	return TRUE;
}

BOOL CMSNichiaMgntSubSystem::CompareMapAndBinArrCode(CONST ULONG ulBinBlkID, 
														LONG& lBinArrCode, 
														LONG& lMapArrCode, 
														BOOL& bNeedUpdateBin)
{
	//TRUE	: means OK; no change is necessary
	//FALSE	: not OK
	if (!m_bIsEnabled)
		return FALSE;
	if (ulBinBlkID > NC_BT_MAX_BINBLK_NO)
		return FALSE;

	LONG lBinCode = m_lBinArrCode[ulBinBlkID];
	LONG lMapCode = m_lMapBinArrCode[ulBinBlkID];

	lBinArrCode = lBinCode;
	lMapArrCode = lMapCode;

	if (lBinCode == lMapCode)
		return TRUE;
	return FALSE;
}

BOOL CMSNichiaMgntSubSystem::SaveArrCode(CONST UCHAR ucCodeIndex)
{
	if (!m_bIsEnabled)
		return FALSE;

	CString szLog;
	szLog.Format("NICHIA SYS: Save ArrCode #%d", ucCodeIndex);
	CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);

	if (ucCodeIndex >= NC_MAX_NO_AC)
	{
		m_szErrMsg.Format("Fails to save ArrCode #%d", ucCodeIndex);
		return FALSE;
	}

	//543212
	if (m_ucArrCodeCurrType == 1)	//Circular Shape)
	{
		if ( (m_ucArrCodeCurrStartPos != 1) && (m_ucArrCodeCurrStartPos != 5) )
		{
			m_szErrMsg = _T("Incorrect Walking-Path for Circular bin type (1 or 5)");
			return FALSE;
		}

		if ( (m_dArrCodeCurrAreaX != 0) && (m_dArrCodeCurrAreaX >= m_dArrCodeCurrAreaY * 2) )
		{
			m_szErrMsg = _T("Circular width (X) cannot be larger than its diameter");
			return FALSE;
		}
	}

	//if (!m_bEnableArrCodeFcn)
	//{
	//	m_szErrMsg.Format("Fails to save ArrCode #%d because fcn is not enabled", ucCodeIndex + 1);
	//	return FALSE;
	//}
//CString szLog;
//szLog.Format("Saving Arrangement Code #%d ...", ucCodeIndex);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);


	//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	if (pUtl->LoadNichiaConfig() == FALSE)
	{
		m_szErrMsg = _T("Fails to load Nichia MSD");
		return FALSE;
	}

	psmf = pUtl->GetNichiaConfigFile();
	if (psmf == NULL)
	{
		m_szErrMsg = _T("Fails to retrieve Nichia MSD pointer");
		return FALSE;
	}

	INT i = ucCodeIndex;

	m_ucArrCodeType[i]		= m_ucArrCodeCurrType;
	m_dArrCodeAreaX[i]		= m_dArrCodeCurrAreaX;
	m_dArrCodeAreaY[i]		= m_dArrCodeCurrAreaY;
	m_ucArrCodeStartPos[i]	= m_ucArrCodeCurrStartPos;
	m_ulArrCodeMaxLoad[i]	= m_ulArrCodeCurrMaxLoad;
	m_ulArrCodeMaxCap[i]	= m_ulArrCodeCurrMaxCap;
	m_ulArrCodePitchX[i]	= m_ulArrCodeCurrPitchX;
	m_ulArrCodePitchY[i]	= m_ulArrCodeCurrPitchY;
	m_bArrCodeReturnTravel[i]	= m_bArrCodeCurrRetTravel;

	//54321
	//ULONG ulDiePerRow		= (ULONG) (m_dArrCodeCurrAreaX / m_ulArrCodeCurrPitchX + 1.0);
	//ULONG ulDiePerCol		= (ULONG) (m_dArrCodeCurrAreaY / m_ulArrCodeCurrPitchY + 1.0);
	//m_ulArrCodeCurrMaxCap	= ulDiePerRow * ulDiePerCol;

	(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_TYPE]			= m_ucArrCodeType[i];
	(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_AREA_X]		= m_dArrCodeAreaX[i];
	(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_AREA_Y]		= m_dArrCodeAreaY[i];
	(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_START_POS]	= m_ucArrCodeStartPos[i];
	(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_MAX_LOAD]		= m_ulArrCodeMaxLoad[i];
	(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_MAX_CAP]		= m_ulArrCodeMaxCap[i];
	(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_PITCH_X]		= m_ulArrCodePitchX[i];
	(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_PITCH_Y]		= m_ulArrCodePitchY[i];
	(*psmf)[NICHIA_DATA][NC_ARRCODE][i][NC_AC_RETURN_TRAVEL]= m_bArrCodeReturnTravel[i];

	pUtl->UpdateNichiaConfig();
	pUtl->CloseNichiaConfig();
	return TRUE;
}

BOOL CMSNichiaMgntSubSystem::UpdateBinArrCode(CONST ULONG ulBinBlkID, CONST LONG lCode)
{
	if (ulBinBlkID > NC_BT_MAX_BINBLK_NO)
		return FALSE;
	m_lBinArrCode[ulBinBlkID] = lCode;

//CString szLog;
//szLog.Format("ARR-CODE for Bin %lu is updated to %ld", ulBinBlkID, lCode);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);
	SaveData();
	return TRUE;
}

BOOL CMSNichiaMgntSubSystem::LoadArrCode(CONST UCHAR ucCodeIndex)
{
	if (ucCodeIndex >= NC_MAX_NO_AC)
	{
		m_szErrMsg.Format("Fails to save ArrCode #%d", ucCodeIndex + 1);
		return FALSE;
	}

//CString szLog;
//szLog.Format("Loading Arrangement Code #%d ...", ucCodeIndex);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

	m_ucArrCodeIndex		= ucCodeIndex;
	m_ucArrCodeCurrType		= m_ucArrCodeType[m_ucArrCodeIndex];
	m_dArrCodeCurrAreaX		= m_dArrCodeAreaX[m_ucArrCodeIndex];
	m_dArrCodeCurrAreaY		= m_dArrCodeAreaY[m_ucArrCodeIndex];
	m_ucArrCodeCurrStartPos	= m_ucArrCodeStartPos[m_ucArrCodeIndex];
	m_ulArrCodeCurrMaxLoad	= m_ulArrCodeMaxLoad[m_ucArrCodeIndex];
	m_ulArrCodeCurrMaxCap	= m_ulArrCodeMaxCap[m_ucArrCodeIndex];
	m_ulArrCodeCurrPitchX	= m_ulArrCodePitchX[m_ucArrCodeIndex];
	m_ulArrCodeCurrPitchY	= m_ulArrCodePitchY[m_ucArrCodeIndex];
	m_bArrCodeCurrRetTravel	= m_bArrCodeReturnTravel[m_ucArrCodeIndex];
	return TRUE;
}

BOOL CMSNichiaMgntSubSystem::GetArrCode(CONST ULONG ulBinBlkID, BOOL bUseMapCode,
										UCHAR& ucType,
										DOUBLE& dAreaX, DOUBLE& dAreaY,
										UCHAR& ucStartPos,
										ULONG& ulMaxLoad, ULONG& ulPitchX, ULONG& ulPitchY,
										BOOL& bRetTravel)
{
	if (ulBinBlkID >= NC_BT_MAX_BINBLK_NO)
	{
		m_szErrMsg.Format("Fails to GetArrCode #%lu", ulBinBlkID);
		return FALSE;
	}

	UCHAR ucIndex = 0;
	
	if (bUseMapCode)
		ucIndex = (UCHAR) m_lMapBinArrCode[ulBinBlkID];
	else
		ucIndex = (UCHAR) m_lBinArrCode[ulBinBlkID];
	
	//Klocwork	//v4.46
	//if (ucIndex == -1)
	//{
	//	m_szErrMsg.Format("GetArrCode fail: Code Index for Bin #%lu is not defined", ulBinBlkID);
	//	return FALSE;
	//}

	//m_ucArrCodeIndex		= ucCodeIndex;
	ucType		= m_ucArrCodeType[ucIndex];
	dAreaX		= m_dArrCodeAreaX[ucIndex];
	dAreaY		= m_dArrCodeAreaY[ucIndex];
	ucStartPos	= m_ucArrCodeStartPos[ucIndex];
	ulMaxLoad	= m_ulArrCodeMaxLoad[ucIndex];
	ulPitchX	= m_ulArrCodePitchX[ucIndex];
	ulPitchY	= m_ulArrCodePitchY[ucIndex];
	bRetTravel	= m_bArrCodeReturnTravel[ucIndex];
	return TRUE;
}

BOOL CMSNichiaMgntSubSystem::GetArrCodeByIndex(CONST UCHAR ucIndex,
										UCHAR& ucType,
										DOUBLE& dAreaX, DOUBLE& dAreaY,
										UCHAR& ucStartPos,
										ULONG& ulMaxLoad, ULONG& ulPitchX, ULONG& ulPitchY,
										BOOL& bRetTravel)
{
	if (ucIndex >= 100)
	{
		m_szErrMsg.Format("Fails to GetArrCode Index #%d", ucIndex);
		return FALSE;
	}

	UCHAR ucCode = ucIndex;

	//m_ucArrCodeIndex		= ucCodeIndex;
	ucType		= m_ucArrCodeType[ucCode];
	dAreaX		= m_dArrCodeAreaX[ucCode];
	dAreaY		= m_dArrCodeAreaY[ucCode];
	ucStartPos	= m_ucArrCodeStartPos[ucCode];
	ulMaxLoad	= m_ulArrCodeMaxLoad[ucCode];
	ulPitchX	= m_ulArrCodePitchX[ucCode];
	ulPitchY	= m_ulArrCodePitchY[ucCode];
	bRetTravel	= m_bArrCodeReturnTravel[ucCode];
	return TRUE;
}


//================================================================
// Protected / Private function implementation section
//================================================================

CString CMSNichiaMgntSubSystem::GetCurrTimeInNichiaTimeFormat()
{
	CString szTime;
	CTime tTime = CTime::GetCurrentTime();
	szTime = tTime.Format("%Y/%m/%d %H:%M:%S");
	return szTime;
}

BOOL CMSNichiaMgntSubSystem::GetTimeFromNichiaTimeFormat(CONST CString szNichiaTime, CTime& tTime)
{
	if (szNichiaTime.GetLength() == 0)
		return FALSE;
	if (szNichiaTime.Find(" ") == -1)
		return FALSE;

	CString szDate = szNichiaTime.Left(szNichiaTime.Find(" "));
	CString szTime = szNichiaTime.Mid(szNichiaTime.Find(" ") + 1);

//AfxMessageBox("GetTimeFromNichiaTimeFormat: Date = " + szDate + "; Time = " + szTime, MB_SYSTEMMODAL);

	CString szYear  = szDate.Left(szDate.Find("/"));
	szDate = szDate.Mid(szDate.Find("/")+1);
	CString szMonth = szDate.Left(szDate.Find("/"));
	szDate = szDate.Mid(szDate.Find("/")+1);
	CString szDay   = szDate;

	CString szHour   = szTime.Left(szTime.Find(":"));
	szTime = szTime.Mid(szTime.Find(":")+1);
	CString szMinute = szTime.Left(szTime.Find(":"));
	szTime = szTime.Mid(szTime.Find(":")+1);
	CString szSecond = szTime;

	INT nYear	= atoi(szYear);
	INT nMonth	= atoi(szMonth);
	INT nDay	= atoi(szDay);
	INT nHour	= atoi(szHour);
	INT nMin	= atoi(szMinute);
	INT nSec	= atoi(szSecond);

	CString szLog;
	szLog.Format("Y=%d, m=%d, d=%d, H=%d, M=%d, S=%d", nYear, nMonth, nDay, nHour, nMin, nSec);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

	CTime t(nYear, nMonth, nDay, nHour, nMin, nSec);
	tTime = t;
	return TRUE;
}


//================================================================
// End of NichiaMgntSys.cpp
//================================================================



