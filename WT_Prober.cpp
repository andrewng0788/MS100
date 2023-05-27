#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "math.h"
#include "FileUtil.h"
#include "MS896A.h"
#include "PrescanUtility.h"
#include "StrInputDlg.h"
#include "WL_Constant.h"
#include "WaferPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CWaferTable::InitProberVariables()
{
	m_bES100Machine		= FALSE;
	m_lThetaLockDelay	= 2000;
	m_lThetaUnlockDelay	= 2000;
	m_lThetaSettleDelay	= 100;
	m_bLockT_PowerOff	= FALSE;
	m_lWTDelay = 0;
	m_lCameraRefPosX = 0;
	m_lCameraRefPosY = 0;
	m_lProbeRefPosX = 0;
	m_lProbeRefPosY = 0;
	m_szProberOutputMapPath		= "";
	m_szPrbCsvDataOutputPath	= "";
	m_szPrbDataOutputPath		= "";
	m_szPrbDCOutputPath			= "";
	m_bDCFailStop				= FALSE;
	m_bDCResultFail				= FALSE;
	m_szProberOutputMapFilename = "";
	m_szProberOperatorID		= "";
	m_szPrbTesterModel			= "LED627WH";
	m_szTesterSpecName			= "2000V";
	// Key Files Operation
	m_szMapDisplayItemFileName	= "";
	m_szMapGradeColorFileName	= "";
	m_szCurrentKeyPkgFileName	= "";
	m_szKeyFilesPath			= "";
	m_ucPrbKeyFilesMode			= 0;
	m_bEnableLoadTSFWhenLoadPKG = FALSE;
	m_bEnableTesterList			= FALSE;
	m_bStopUnloadInWaferEnd		= FALSE;
	m_bReCheckAfterWaferEnd		= FALSE;
	m_szRemark1Name				= "";
	m_szTesterTSFPath			= "";
	m_szResortingFilePath		= "";
	m_szConditionFilePath		= "";
	m_szOutputMapFullPath		= "";
	m_szSourceMapFullPath		= "";
	m_bProbedMapHoriFlip		= FALSE;
	m_bProbedMapVertFlip		= FALSE;
	m_ucProbedMapRotation		= 0;
	m_szPrevGradeItemName		= "";
	m_szCurrGradeItemName		= "";
	m_szNextGradeItemName		= "";
	m_ulUpdateBinMapCounter		= 0;
	for(int i=0; i<CP_ITEM_MAX_LIMIT; i++)
		m_bBinMapFileOpen[i]	= false;

	m_bIsPODone	= FALSE;
	m_bIsDCDone	= FALSE;
	m_bHmiPOState	= FALSE;
	m_bHmiDCState	= FALSE;
	m_lCPClbPrDelay = 100;
	m_lPrbCamOffsetX = 0;
	m_lPrbCamOffsetY = 0;
	m_bBlockProbingToRight	= TRUE;
	m_lBlockProbeLoopRow	= 0;
	m_lBlockProbeLoopCol	= 0;

	m_bWaferNonStopTest	= FALSE;
	m_bWaferRepeatTest	= FALSE;
	m_lRepeatTestLimit	= 0;
	m_lCpProbeDiePath	= 0;
	m_lCpProbeDieLimit	= 0;
	m_bPrevMapPageOn	= FALSE;

	m_bSPEnable			= FALSE;
	m_ucSPGrade			= 0;
	m_ulSPJumpRow		= 0;
	m_ulSPJumpCol		= 0;
	for(int j=0; j<CP_ITEM_MAX_LIMIT; j++)
	{
		m_dMinTested[j]	= CP_ITEM_INVALID_VALUE;
		m_dMaxTested[j]	= CP_ITEM_INVALID_VALUE;
		m_dTolTested[j]	= CP_ITEM_INVALID_VALUE;
		m_szaItemList[j] = "";
		m_daBinMapItemMin[j]	= 0;
		m_daBinMapItemMax[j]	= 0;
		m_daBinMapItemStep[j]	= 0;
	}

	// CP100 ACP init
	m_lACPTableT		= 0;
	m_lACPUpLeftX		= 0;
	m_lACPUpLeftY		= 0;
	m_lACPUpRightX		= 0;
	m_lACPUpRightY		= 0;
	m_lACPLowLeftX		= 0;
	m_lACPLowLeftY		= 0;
	m_lACPCornerX		= 0;
	m_lACPCornerY		= 0;
	m_szACPCornerX		= "Up Left X";
	m_szACPCornerY		= "Up Left Y";
	m_lACPMatrixRow		= 0;
	m_lACPMatrixCol		= 0;
	m_lCPinPositionX	= 0;
	m_lCPinPositionY	= 0;

	m_bIsThetaUnlocked		= TRUE;
	m_bIsEnableCheckAlerm	= TRUE;

	m_bProbeUsingESD			= FALSE;
	m_bIsWaferProbed			= FALSE;

	m_lBlockProbeRow	= 1;
	m_lBlockProbeCol	= 1;
	m_lBlockProbeSpanRow	= 0;
	m_lBlockProbeSpanCol	= 0;

	m_ucPrbItemMapDisplayMode	= 0;
	m_lPrbKeyTableColumns		= 5;
}

VOID CWaferTable::CP_LoadOptions(CStringMapFile  *psmf)
{
}

VOID CWaferTable::LoadProberFromWFT(CStringMapFile  *psmf)
{
	m_bEnableUseProbeTableLevelByRegion = (BOOL)(LONG)(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][USE_PROBE_TABLE_LEVEL_BY_REGION];
	m_lProbeTableDataSamplesX = (*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][SIZE_X];
	m_lProbeTableDataSamplesY = (*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][SIZE_Y];
	m_lProbeTableDataSamplesPitchX = (*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][PITCH_X];
	m_lProbeTableDataSamplesPitchY = (*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][PITCH_Y];

	m_lWTDelay				= (*psmf)[WT_PROCESS_DATA][WT_WAFERTABLE_DELAY];
	(*m_psmfSRam)["WaferTable"]["CpWTDelay"]			= m_lWTDelay;
	m_lThetaLockDelay		= (*psmf)[WT_PRB_OPTIONS][CP_PLUS_T_LOCK_DELAY];
	m_lThetaUnlockDelay		= (*psmf)[WT_PRB_OPTIONS][CP_PLUS_T_UNLOCK_DELAY];
	m_lThetaSettleDelay		= (*psmf)[WT_PRB_OPTIONS][CP_PLUS_T_SETTLE_DELAY];
	m_bLockT_PowerOff		= (BOOL)(LONG)(*psmf)[WT_PRB_OPTIONS][CP_PLUS_T_LOCK_OFF];
	if( m_lThetaLockDelay<1000 )
	{
		m_lThetaLockDelay	= 1000;
	}
	if( m_lThetaUnlockDelay<1000 )
	{
		m_lThetaUnlockDelay	= 1000;
	}

	int i;
	CString szTemp;
	for (i=0; i<MS_LEARN_LEVEL_CORNER_POINT; i++)
	{
		szTemp.Format("%d", i+1);
		m_stLearnLevelCornerPoints[i].lX = (*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_LEARN_CONRER][szTemp]["X"];
		m_stLearnLevelCornerPoints[i].lY = (*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_LEARN_CONRER][szTemp]["Y"];
	}

	for (i=0; i<MS_PROBETABLE_LEVEL_DATA_NO; i++ )
	{
		szTemp.Format("%d", i+1);
		m_stProbeTableLevelData[i].lX = (*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][szTemp]["X"];
		m_stProbeTableLevelData[i].lY = (*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][szTemp]["Y"];
		m_stProbeTableLevelData[i].lZ = (*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][szTemp]["Z"];
	}

	m_ucProbedMapRotation		= (UCHAR)(LONG)(*psmf)[WT_OPTION][WT_PRB_OUTPUT_MAP_ROTATION];
	m_bProbedMapHoriFlip		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRB_OUTPUT_MAP_FLIP_HORI];
	m_bProbedMapVertFlip		= (BOOL)(LONG)(*psmf)[WT_OPTION][WT_PRB_OUTPUT_MAP_FLIP_VERT];

	m_szRemark1Name				= (*psmf)[WT_PRB_OPTIONS][WT_PRB_REMARK1_NAME];
	m_bSPEnable					= (BOOL)(LONG)(*psmf)[WT_PRB_OPTIONS][WT_PRB_SP_ENABLE];
	m_ucSPGrade					= (UCHAR)(LONG)(*psmf)[WT_PRB_OPTIONS][WT_PRB_SP_GRADE];
	m_ulSPJumpRow				= (*psmf)[WT_PRB_OPTIONS][WT_PRB_SP_JUMP_ROW];
	m_ulSPJumpCol				= (*psmf)[WT_PRB_OPTIONS][WT_PRB_SP_JUMP_COL];

	m_szTesterTSFPath			= (*psmf)[WT_OPTION][WT_TESTER_TSF_FILE_PATH];
	m_szResortingFilePath		= (*psmf)[WT_OPTION][WT_RESORTING_SPEC_FILE_PATH];
	m_szConditionFilePath		= (*psmf)[WT_OPTION][WT_TEST_CONDITION_FILE_PATH];
	m_szOutputMapFullPath		= (*psmf)[WT_OPTION][WT_PSC_PROBED_MAP_FILE];
	m_szSourceMapFullPath		= (*psmf)[WT_OPTION][WT_PSC_SOURCE_MAP_FILE];

	m_bProbeUsingESD			= (BOOL)(LONG)(*psmf)[WT_OPTION]["CP is use ESD"];
	m_lBlockProbeRow			= (*psmf)[WT_PRB_OPTIONS][WT_PRB_BLOCK_PROBE_ROW];
	m_lBlockProbeCol			= (*psmf)[WT_PRB_OPTIONS][WT_PRB_BLOCK_PROBE_COL];
	m_lBlockProbeSpanRow		= (*psmf)[WT_PRB_OPTIONS][WT_PRB_BLOCK_PROBE_SPAN_ROW];
	m_lBlockProbeSpanCol		= (*psmf)[WT_PRB_OPTIONS][WT_PRB_BLOCK_PROBE_SPAN_COL];
	if( m_lBlockProbeRow<=1 )
		m_lBlockProbeSpanRow = 0;
	if( m_lBlockProbeCol<=1 )
		m_lBlockProbeSpanCol = 0;

	CString szItemName = "";
	m_ulCurrMapItemIndex = 0;
	m_szaGradeItemList.RemoveAll();
	for(int i=0; i<CP_ITEM_MAX_LIMIT; i++)
	{
		m_dMinTested[i]			= (*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_MIN_VALUE][i];
		m_dMaxTested[i]			= (*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_MAX_VALUE][i];
		m_dTolTested[i]			= (*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_TOL_VALUE][i];
		m_szaItemList[i]		= (*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_ITEM_TITLE][i];
		szItemName				= (*psmf)[WT_OPTION][WT_PRB_ITEM_GRADE_MAP_TITLE][i];
		if( szItemName.IsEmpty()==FALSE )
			m_szaGradeItemList.Add(szItemName);
		m_daBinMapItemMin[i]	= (*psmf)[WT_PRB_OPTIONS][WT_PRB_BINMAP_ITEM_MIN][i];
		m_daBinMapItemMax[i]	= (*psmf)[WT_PRB_OPTIONS][WT_PRB_BINMAP_ITEM_MAX][i];
		m_daBinMapItemStep[i]	= (*psmf)[WT_PRB_OPTIONS][WT_PRB_BINMAP_ITEM_STEP][i];
	}
	m_lContinueFailLimit		= (*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_FAIL_LIMIT];
	m_lCpProbeDiePath			= (*psmf)[WT_PRB_OPTIONS][WT_PRB_SEQUENCE_PROBE_PATH];
	m_lCpProbeDieLimit			= (*psmf)[WT_PRB_OPTIONS][WT_PRB_SEQUENCE_PROBE_LIMIT];

	m_szProberOutputMapFilename = (*psmf)[WT_PRB_OUTMAP][WT_OM_MAP_FILENAME];
	m_szProberOutputMapPath		= (*psmf)[WT_PRB_OUTMAP][WT_OM_OUTPUT_PATH];
	m_szPrbCsvDataOutputPath	= (*psmf)[WT_PRB_OUTMAP][WT_OM_CSVDATA_PATH];
	m_szPrbDataOutputPath		= (*psmf)[WT_PRB_OUTMAP][WT_OM_DATA_PATH];
	m_szPrbDCOutputPath			= (*psmf)[WT_PRB_OUTMAP][WT_OM_DC_OUT_PATH];
	m_bDCFailStop				= (BOOL)(LONG)((*psmf)[WT_PRB_OUTMAP][WT_DC_STOP_IF_FAIL]);
	m_szPrbTesterModel			= (*psmf)[WT_PRB_OUTMAP][WT_PRB_TESTER_MODEL];
	m_szTesterSpecName			= (*psmf)[WT_PRB_OUTMAP][WT_PRB_TESTER_SPEC_NAME];
	m_szMapDisplayItemFileName	= (*psmf)[WT_PRB_OUTMAP][WT_PRB_MAPDISPLAY_ITEM_FILENAMAE];
	m_szMapGradeColorFileName	= (*psmf)[WT_PRB_OUTMAP][WT_PRB_MAP_GRADE_COLOR_FILENAME];
	m_szCurrentKeyPkgFileName	= (*psmf)[WT_PRB_OPTIONS][WT_PRB_CURR_KEYPKG_FILENAME];
	m_szKeyFilesPath			= (*psmf)[WT_PRB_OPTIONS][WT_PRB_KEYFILES_PATH];
	if( m_szKeyFilesPath=="" )
	{
		m_szKeyFilesPath = gszUSER_DIRECTORY + "\\OutputFile";
	}

	m_bStopUnloadInWaferEnd		= (BOOL)(LONG)((*psmf)[WT_OPTION][WT_STOP_UNLOAD_WAFEREND]);
	m_bReCheckAfterWaferEnd		= (BOOL)(LONG)((*psmf)[WT_PRB_OPTIONS][WT_PRB_RECHECK_AFTER_WAFEREND]);
	m_ucPrbItemMapDisplayMode	= (UCHAR)(LONG)((*psmf)[WT_PRB_OPTIONS][WT_PRB_ITEM_MAP_DISPLAY_MODE]);
	if( m_szPrbTesterModel.IsEmpty() )
		m_szPrbTesterModel = "LED627WH";

	// CP100 ACP load
	if( m_lACPMatrixRow<=0 )
		m_lACPMatrixRow = 1;
	if( m_lACPMatrixCol<=0 )
		m_lACPMatrixCol = 1;
	m_lCPinPositionX	= (*psmf)[WP_DATA][WP_ACPP][WP_CP_POSITION_X];
	m_lCPinPositionY	= (*psmf)[WP_DATA][WP_ACPP][WP_CP_POSITION_Y];
	m_lACPUpLeftX		= (*psmf)[WP_DATA][WP_ACPP][WP_UPLEFT_X];
	m_lACPUpLeftY		= (*psmf)[WP_DATA][WP_ACPP][WP_ULLEFT_Y];
	m_lACPUpRightX		= (*psmf)[WP_DATA][WP_ACPP][WP_UPRIGHT_X];
	m_lACPUpRightY		= (*psmf)[WP_DATA][WP_ACPP][WP_UPRIGHT_Y];
	m_lACPLowLeftX		= (*psmf)[WP_DATA][WP_ACPP][WP_LOWLEFT_X];
	m_lACPLowLeftY		= (*psmf)[WP_DATA][WP_ACPP][WP_LOWLEFT_Y];
	m_lACPTableT		= (*psmf)[WP_DATA][WP_ACPP][WP_TABLE_T];
	m_lACPMatrixRow		= (*psmf)[WP_DATA][WP_ACPP][WP_MATRIX_ROW];
	m_lACPMatrixCol		= (*psmf)[WP_DATA][WP_ACPP][WP_MATRIX_COL];

	m_bEnableAutoSearchWL_inDailyCheck	= (BOOL)(LONG)((*psmf)[WP_DATA][PB_ENABLE_AUTO_SEARCH_WAFER_LIMIT_IN_DAILY_CHECK]);
	m_bEnableAutoSearchWE_inDailyCheck	= (BOOL)(LONG)((*psmf)[WP_DATA][PB_ENABLE_AUTO_SEARCH_WAFER_EDGE_IN_DAILY_CHECK]);


	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	UINT unItem = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Enable Check Wafer Contact Sensor"), 1);
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("Enable Check Wafer Contact Sensor"), unItem);
	m_bIsEnableCheckAlerm = (BOOL)unItem;
}

VOID CWaferTable::SaveProberIntoWFT(CStringMapFile  *psmf)
{
	(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][USE_PROBE_TABLE_LEVEL_BY_REGION] = m_bEnableUseProbeTableLevelByRegion;
	(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][SIZE_X] = m_lProbeTableDataSamplesX;
	(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][SIZE_Y] = m_lProbeTableDataSamplesY;
	(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][PITCH_X] = m_lProbeTableDataSamplesPitchX;
	(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][PITCH_Y] = m_lProbeTableDataSamplesPitchY;

	if( m_lThetaLockDelay<1000 )
	{
		m_lThetaLockDelay	= 1000;
	}
	if( m_lThetaUnlockDelay<1000 )
	{
		m_lThetaUnlockDelay	= 1000;
	}
	(*psmf)[WT_PROCESS_DATA][WT_WAFERTABLE_DELAY]	= m_lWTDelay;
	(*m_psmfSRam)["WaferTable"]["CpWTDelay"]		= m_lWTDelay;
	(*psmf)[WT_PRB_OPTIONS][CP_PLUS_T_LOCK_DELAY]	= m_lThetaLockDelay;
	(*psmf)[WT_PRB_OPTIONS][CP_PLUS_T_UNLOCK_DELAY]	= m_lThetaUnlockDelay;
	(*psmf)[WT_PRB_OPTIONS][CP_PLUS_T_SETTLE_DELAY]	= m_lThetaSettleDelay;
	(*psmf)[WT_PRB_OPTIONS][CP_PLUS_T_LOCK_OFF]		= m_bLockT_PowerOff;

	int i;
	CString szTemp;
	for (i=0; i<MS_LEARN_LEVEL_CORNER_POINT; i++)
	{
		szTemp.Format("%d", i+1);
		(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_LEARN_CONRER][szTemp]["X"] = m_stLearnLevelCornerPoints[i].lX;
		(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_LEARN_CONRER][szTemp]["Y"] = m_stLearnLevelCornerPoints[i].lY;
	}

	for (i=0; i<MS_PROBETABLE_LEVEL_DATA_NO; i++ )
	{
		szTemp.Format("%d", i+1);
		(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][szTemp]["X"] = m_stProbeTableLevelData[i].lX;
		(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][szTemp]["Y"] = m_stProbeTableLevelData[i].lY;
		(*psmf)[WT_PROCESS_DATA][WT_PROBE_TABLE_LEVEL_DATA][szTemp]["Z"] = m_stProbeTableLevelData[i].lZ;
	}

	(*psmf)[WT_OPTION][WT_PRB_OUTPUT_MAP_ROTATION]		= m_ucProbedMapRotation;
	(*psmf)[WT_OPTION][WT_PRB_OUTPUT_MAP_FLIP_HORI]		= m_bProbedMapHoriFlip;
	(*psmf)[WT_OPTION][WT_PRB_OUTPUT_MAP_FLIP_VERT]		= m_bProbedMapVertFlip;

	(*psmf)[WT_PRB_OPTIONS][WT_PRB_REMARK1_NAME]	= m_szRemark1Name;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_SP_ENABLE]		= m_bSPEnable;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_SP_GRADE]		= m_ucSPGrade;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_SP_JUMP_ROW]		= m_ulSPJumpRow;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_SP_JUMP_COL]		= m_ulSPJumpCol;

	(*psmf)[WT_OPTION][WT_TESTER_TSF_FILE_PATH]		= m_szTesterTSFPath;
	(*psmf)[WT_OPTION][WT_RESORTING_SPEC_FILE_PATH] = m_szResortingFilePath;
	(*psmf)[WT_OPTION][WT_TEST_CONDITION_FILE_PATH] = m_szConditionFilePath;
	(*psmf)[WT_OPTION][WT_PSC_PROBED_MAP_FILE]		= m_szOutputMapFullPath;
	(*psmf)[WT_OPTION][WT_PSC_SOURCE_MAP_FILE]		= m_szSourceMapFullPath;

	(*psmf)[WT_OPTION]["CP is use ESD"]				= m_bProbeUsingESD;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_BLOCK_PROBE_SPAN_ROW]	= m_lBlockProbeSpanRow;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_BLOCK_PROBE_SPAN_COL]	= m_lBlockProbeSpanCol;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_BLOCK_PROBE_ROW]			= m_lBlockProbeRow;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_BLOCK_PROBE_COL]			= m_lBlockProbeCol;
	if( m_lBlockProbeRow<=1 )
		m_lBlockProbeSpanRow = 0;
	if( m_lBlockProbeCol<=1 )
		m_lBlockProbeSpanCol = 0;

	for(int i=0; i<CP_ITEM_MAX_LIMIT; i++)
	{
		(*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_MIN_VALUE][i]	=	m_dMinTested[i];
		(*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_MAX_VALUE][i]	=	m_dMaxTested[i];
		(*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_TOL_VALUE][i]	=	m_dTolTested[i];
		(*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_ITEM_TITLE][i]	=	m_szaItemList[i];
		CString szItemName = "";
		if( i<m_szaGradeItemList.GetSize() )
		{
			szItemName = m_szaGradeItemList.GetAt(i);
		}
		(*psmf)[WT_OPTION][WT_PRB_ITEM_GRADE_MAP_TITLE][i]	= szItemName;
		(*psmf)[WT_PRB_OPTIONS][WT_PRB_BINMAP_ITEM_MIN][i]	= m_daBinMapItemMin[i];
		(*psmf)[WT_PRB_OPTIONS][WT_PRB_BINMAP_ITEM_MAX][i]	= m_daBinMapItemMax[i];
		(*psmf)[WT_PRB_OPTIONS][WT_PRB_BINMAP_ITEM_STEP][i]	= m_daBinMapItemStep[i];
	}
	(*psmf)[WT_OPTION][WT_PRB_NEAR_CHECK_FAIL_LIMIT]	= m_lContinueFailLimit;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_SEQUENCE_PROBE_PATH]	= m_lCpProbeDiePath;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_SEQUENCE_PROBE_LIMIT]= m_lCpProbeDieLimit;

	(*psmf)[WT_OPTION][WT_STOP_UNLOAD_WAFEREND]	= m_bStopUnloadInWaferEnd;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_RECHECK_AFTER_WAFEREND]	= m_bReCheckAfterWaferEnd;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_ITEM_MAP_DISPLAY_MODE]	= m_ucPrbItemMapDisplayMode;

	(*psmf)[WT_PRB_OUTMAP][WT_OM_MAP_FILENAME]	= m_szProberOutputMapFilename;
	(*psmf)[WT_PRB_OUTMAP][WT_OM_OUTPUT_PATH]	= m_szProberOutputMapPath;
	(*psmf)[WT_PRB_OUTMAP][WT_OM_CSVDATA_PATH]	= m_szPrbCsvDataOutputPath;
	(*psmf)[WT_PRB_OUTMAP][WT_OM_DATA_PATH]		= m_szPrbDataOutputPath;
	(*psmf)[WT_PRB_OUTMAP][WT_OM_DC_OUT_PATH]	= m_szPrbDCOutputPath;
	(*psmf)[WT_PRB_OUTMAP][WT_DC_STOP_IF_FAIL]	= m_bDCFailStop;
	if( m_szPrbTesterModel.IsEmpty() )
		m_szPrbTesterModel = "LED627WH";
	(*psmf)[WT_PRB_OUTMAP][WT_PRB_TESTER_MODEL]		= m_szPrbTesterModel;
	(*psmf)[WT_PRB_OUTMAP][WT_PRB_TESTER_SPEC_NAME]	= m_szTesterSpecName;
	(*psmf)[WT_PRB_OUTMAP][WT_PRB_MAPDISPLAY_ITEM_FILENAMAE]	= m_szMapDisplayItemFileName;
	(*psmf)[WT_PRB_OUTMAP][WT_PRB_MAP_GRADE_COLOR_FILENAME]		= m_szMapGradeColorFileName;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_CURR_KEYPKG_FILENAME]	= m_szCurrentKeyPkgFileName;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_KEYFILES_PATH]			= m_szKeyFilesPath;
	(*psmf)[WT_PRB_OPTIONS][WT_PRB_KEYFILES_OPERATION_MODE]	= m_ucPrbKeyFilesMode;

	// CP100 ACP save
	(*psmf)[WP_DATA][WP_ACPP][WP_CP_POSITION_X]	= m_lCPinPositionX;
	(*psmf)[WP_DATA][WP_ACPP][WP_CP_POSITION_Y]	= m_lCPinPositionY;
	(*psmf)[WP_DATA][WP_ACPP][WP_TABLE_T]		= m_lACPTableT;
	(*psmf)[WP_DATA][WP_ACPP][WP_UPLEFT_X]		= m_lACPUpLeftX;
	(*psmf)[WP_DATA][WP_ACPP][WP_ULLEFT_Y]		= m_lACPUpLeftY;
	(*psmf)[WP_DATA][WP_ACPP][WP_UPRIGHT_X]		= m_lACPUpRightX;
	(*psmf)[WP_DATA][WP_ACPP][WP_UPRIGHT_Y]		= m_lACPUpRightY;
	(*psmf)[WP_DATA][WP_ACPP][WP_LOWLEFT_X]		= m_lACPLowLeftX;
	(*psmf)[WP_DATA][WP_ACPP][WP_LOWLEFT_Y]		= m_lACPLowLeftY;
	(*psmf)[WP_DATA][WP_ACPP][WP_MATRIX_ROW]	= m_lACPMatrixRow;
	(*psmf)[WP_DATA][WP_ACPP][WP_MATRIX_COL]	= m_lACPMatrixCol;

	(*psmf)[WP_DATA][PB_ENABLE_AUTO_SEARCH_WAFER_LIMIT_IN_DAILY_CHECK]	= m_bEnableAutoSearchWL_inDailyCheck;
	(*psmf)[WP_DATA][PB_ENABLE_AUTO_SEARCH_WAFER_EDGE_IN_DAILY_CHECK]	= m_bEnableAutoSearchWE_inDailyCheck;

	if( m_lACPMatrixRow<=0 )
		m_lACPMatrixRow = 1;
	if( m_lACPMatrixCol<=0 )
		m_lACPMatrixCol = 1;
}

VOID CWaferTable::RegisterProberCommand()
{
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveSearchToUL"),			MoveSearchToUL);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ScanCheckDieOffset"),		ScanCheckDieOffset);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("FOVCheckDieOffset"),		FOVCheckDieOffset);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CP_GridSampleOffset"),		CP_GridSampleOffset);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CP_SaveData"),				CP_SaveData);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_PitchMoveX"),			WT_PitchMoveX);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("WT_PitchMoveY"),			WT_PitchMoveY);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachProberOffsetXY"),		TeachProberOffsetXY);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckProberOffsetXY"),		CheckProberOffsetXY);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToProberOffsetXY"),		MoveToProberOffsetXY);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveTableToUnloadToCleanProbePin"),		MoveTableToUnloadToCleanProbePin);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWaferTableVacuumCmd"),	SetWaferTableVacuumCmd);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetThetaBrakeCmd"),			SetThetaBrakeCmd);
	
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualGenerateProberMapFile"),	ManualGenerateProberMapFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetProberMapPath"),			GetProberMapPath);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetPrbCsvDataOutPath"),		GetPrbCsvDataOutPath);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetPrbDataOutPath"),		GetPrbDataOutPath);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetPrbDCOutputMapPath"),	GetPrbDCOutputMapPath);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReprobeAtSelectMap"),		ReprobeAtSelectMap);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("RecoverCrashedMap"),		RecoverCrashedMap);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReprobeWholeMap"),			ReprobeWholeMap);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIsBlockProbe"),		CheckIsBlockProbe);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoCaptureScreenInCleanPin"),	AutoCaptureScreenInCleanPin);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualCleanProbePinProcess"),	ManualCleanProbePinProcess);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("WriteSpecialItemToReg"),		WriteSpecialItemToReg);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReadWriteSpecialItemToReg"),	ReadWriteSpecialItemToReg);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoLoadTesterFiles"),			AutoLoadTesterFiles);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CP_TogglePOState"),			CP_TogglePOState);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CP_ToggleDCState"),			CP_ToggleDCState);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateProbeTableLevelByRegion"),	UpdateProbeTableLevelByRegion);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("LearnProbeTableLevelByRegion"),	LearnProbeTableLevelByRegion);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY_MoveToProbeViaScanCmd"),	XY_MoveToProbeViaScanCmd);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpMoveToUnloadLoad"),		CpMoveToUnloadLoad);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CP_MoveToWaferCamera"),		CP_MoveToWaferCamera);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckOutputMapGeneration"),	CheckOutputMapGeneration);
	//	CP100 relative on the map file name.
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetConditionPath"),		GetConditionPath);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadConditionFile"),	LoadConditionFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetResortingPath"),		GetResortingPath);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadResortingFile"),	LoadResortingFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadNearCheckCriteriaFile"),	LoadNearCheckCriteriaFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CreateNearCheckDummyFile"),		CreateNearCheckDummyFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetTesterTSFPath"),				GetTesterTSFPath);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectProberSampleCheckMode"),	SelectProberSampleCheckMode);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadDailyCheckStandard"),		LoadDailyCheckStandard);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SpinItemMapDisplay"),			SpinItemMapDisplay);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectItemMapDisplay"),			SelectItemMapDisplay);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetItemGradesListFromTester"),	GetItemGradesListFromTester);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("IntoItemGradeMapPage"),			IntoItemGradeMapPage);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BeforeIntoMapPage"),			BeforeIntoMapPage);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("IntoItemGradeSetupPage"),		IntoItemGradeSetupPage);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("AddItemToBinMapList"),			AddItemToBinMapList);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("RemoveItemFromBinMapList"),		RemoveItemFromBinMapList);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectBinMapItemToSetRange"),	SelectBinMapItemToSetRange);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmBinMapItemRange"),		ConfirmBinMapItemRange);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BurnInAlignWafer"),				BurnInAlignWafer);

	// Key Files Operation
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetKeyFilesPath"),		GetKeyFilesPath);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpLoadKeyConfigFile"),			CpLoadKeyConfigFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpSkipKeyConfigFile"),			CpSkipKeyConfigFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpSaveKeyConfigFile"),			CpSaveKeyConfigFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpLoadMapGradeColorFile"),		CpLoadMapGradeColorFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpSaveMapGradeColorFile"),		CpSaveMapGradeColorFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpLoadMapDisplayItemsFile"),	CpLoadMapDisplayItemsFile);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpSaveMapDisplayItemsFile"),	CpSaveMapDisplayItemsFile);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpLoadOutputMapColumns"),		CpLoadOutputMapColumns);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpOutputMapHeaderPreTask"),		CpOutputMapHeaderPreTask);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CpSaveOutputMapColumns"),		CpSaveOutputMapColumns);

	// CP100 ACP reg cmd
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ACPSetCorner"),					ACPSetCorner);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetCleanPinUpLeftCorner"),		SetCleanPinUpLeftCorner);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmCleanPinUpLeftCorner"),	ConfirmCleanPinUpLeftCorner);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveTableToCleanPinPoint"),		MoveTableToCleanPinPoint);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveTableToCleanPinTheta"),		MoveTableToCleanPinTheta);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachCleanPinPosition"),		TeachCleanPinPosition);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmCleanPinPosition"),		ConfirmCleanPinPosition);

	// CP100 OUTPUTMAP
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CPOutputLibraryAboutDialog"),	CPOutputLibraryAboutDialog);
}

VOID CWaferTable::RegisterProberVariable()
{
	RegVariable(_T("WT_bEnableMultipleMapHeaderPage"), &m_bEnableMultipleMapHeaderPage);
	RegVariable(_T("WT_lWTDelay"),					&m_lWTDelay);
	RegVariable(_T("WT_lProbeOffsetX"),				&m_lProbeOffsetX);
	RegVariable(_T("WT_lProbeOffsetY"),				&m_lProbeOffsetY);

	RegVariable(_T("WT_szProberOutputMapFilename"),	&m_szProberOutputMapFilename);
	RegVariable(_T("WT_szProberOperatorID"),		&m_szProberOperatorID);
	RegVariable(_T("WT_szProberOutputMapPath"),		&m_szProberOutputMapPath);
	RegVariable(_T("WT_szPrbCsvDataOutputPath"),	&m_szPrbCsvDataOutputPath);
	RegVariable(_T("WT_szPrbDataOutputPath"),		&m_szPrbDataOutputPath);
	RegVariable(_T("WT_szPrbDCOutputPath"),			&m_szPrbDCOutputPath);
	RegVariable(_T("WT_bDCFailStop"),				&m_bDCFailStop);
	RegVariable(_T("WT_bDCResultFail"),				&m_bDCResultFail);
	RegVariable(_T("WT_szPrbTesterModel"),			&m_szPrbTesterModel);
	RegVariable(_T("WT_szTesterSpecName"),			&m_szTesterSpecName);
	RegVariable(_T("WT_bEnableLoadTSFWhenLoadPKG"), &m_bEnableLoadTSFWhenLoadPKG);
	RegVariable(_T("WT_bEnableTesterList"),			&m_bEnableTesterList);
	RegVariable(_T("WT_szRemark1Name"),				&m_szRemark1Name);
	RegVariable(_T("WT_szMapItemsFilename"),		&m_szMapDisplayItemFileName);
	RegVariable(_T("WT_szMapGradeColorFileName"),	&m_szMapGradeColorFileName);
	RegVariable(_T("WT_szCurrentKeyPkgFileName"),	&m_szCurrentKeyPkgFileName);
	RegVariable(_T("WT_szKeyFilesPath"),			&m_szKeyFilesPath);
	RegVariable(_T("WT_ucPrbKeyFilesMode"),			&m_ucPrbKeyFilesMode);

	RegVariable(_T("WT_bEnableUseProbeTableLevelByRegion"),	&m_bEnableUseProbeTableLevelByRegion);
	RegVariable(_T("WT_lProbeTableDataSamplesX"),	&m_lProbeTableDataSamplesX);
	RegVariable(_T("WT_lProbeTableDataSamplesY"),	&m_lProbeTableDataSamplesY);
	RegVariable(_T("WT_bIsPODone"),					&m_bIsPODone);
	RegVariable(_T("WT_bIsDCDone"),					&m_bIsDCDone);
	RegVariable(_T("WT_bHmiPOState"),				&m_bHmiPOState);
	RegVariable(_T("WT_bHmiDCState"),				&m_bHmiDCState);
	RegVariable(_T("WT_lCpClbPrDelay"),				&m_lCPClbPrDelay);
	RegVariable(_T("WT_lPrbCamOffsetX"),			&m_lPrbCamOffsetX);
	RegVariable(_T("WT_lPrbCamOffsetY"),			&m_lPrbCamOffsetY);

	RegVariable(_T("WT_bWaferNonStopTest"),			&m_bWaferNonStopTest);
	RegVariable(_T("WT_bWaferRepeatTest"),			&m_bWaferRepeatTest);
	RegVariable(_T("WT_lRepeatTestLimit"),			&m_lRepeatTestLimit);
	RegVariable(_T("WT_lCpProbeDiePath"),			&m_lCpProbeDiePath);
	RegVariable(_T("WT_lCpProbeDieLimit"),			&m_lCpProbeDieLimit);

	RegVariable(_T("WT_szTesterTSFPath"),			&m_szTesterTSFPath);
	RegVariable(_T("WT_szResortingFilePath"),		&m_szResortingFilePath);
	RegVariable(_T("WT_szConditionFilePath"),		&m_szConditionFilePath);
	RegVariable(_T("WT_lContinueFailLimit"),		&m_lContinueFailLimit);
	RegVariable(_T("WT_dHmiBinItemMin"),			&m_dHmiBinItemMin);
	RegVariable(_T("WT_dHmiBinItemMax"),			&m_dHmiBinItemMax);
	RegVariable(_T("WT_dHmiBinItemStep"),			&m_dHmiBinItemStep);

	RegVariable(_T("WT_bSPEnable"),				&m_bSPEnable);
	RegVariable(_T("WT_ucSPGrade"),				&m_ucSPGrade);
	RegVariable(_T("WT_ulSPJumpRow"),			&m_ulSPJumpRow);
	RegVariable(_T("WT_ulSPJumpCol"),			&m_ulSPJumpCol);

	RegVariable(_T("WT_lProberSampleCheckMode"),	&m_lPSCMode);
	RegVariable(_T("WT_ucPSCGoodGrade"),			&m_ucPSCGoodGrade);
	RegVariable(_T("WT_bProbedMapHoriFlip"),		&m_bProbedMapHoriFlip);
	RegVariable(_T("WT_bProbedMapVertFlip"),		&m_bProbedMapVertFlip);
	RegVariable(_T("WT_ucProbedMapRotation"),		&m_ucProbedMapRotation);
	RegVariable(_T("WT_szPrevGradeItemName"),		&m_szPrevGradeItemName);
	RegVariable(_T("WT_szCurrGradeItemName"),		&m_szCurrGradeItemName);
	RegVariable(_T("WT_szNextGradeItemName"),		&m_szNextGradeItemName);

	// CP100 ACP reg var
	RegVariable(_T("WP_lACPTableT"),		&m_lACPTableT);
	RegVariable(_T("WP_lACPCornerX"),		&m_lACPCornerX);
	RegVariable(_T("WP_lACPCornerY"),		&m_lACPCornerY);
	RegVariable(_T("WP_szACPCornerX"),		&m_szACPCornerX);
	RegVariable(_T("WP_szACPCornerY"),		&m_szACPCornerY);
	RegVariable(_T("WP_lACPMatrixRow"),		&m_lACPMatrixRow);
	RegVariable(_T("WP_lACPMatrixCol"),		&m_lACPMatrixCol);
	RegVariable(_T("WP_lCPinPositionX"),	&m_lCPinPositionX);
	RegVariable(_T("WP_lCPinPositionY"),	&m_lCPinPositionY);

	RegVariable(_T("WP_bIsEnableCheckAlerm"),	&m_bIsEnableCheckAlerm);
	RegVariable(_T("WP_bIsThetaUnlocked"),		&m_bIsThetaUnlocked);
	RegVariable(_T("WP_lThetaLockDelay"),		&m_lThetaLockDelay);
	RegVariable(_T("WP_lThetaUnlockDelay"),		&m_lThetaUnlockDelay);
	RegVariable(_T("WP_lThetaSettleDelay"),		&m_lThetaSettleDelay);
	RegVariable(_T("WP_bLockT_PowerOff"),		&m_bLockT_PowerOff);

	// Contact Sensor Range
	RegVariable(_T("PBT_bEnableAutoSearchWL_inDailyCheck"),	&m_bEnableAutoSearchWL_inDailyCheck);
	RegVariable(_T("PBT_bEnableAutoSearchWE_inDailyCheck"),	&m_bEnableAutoSearchWE_inDailyCheck);

	RegVariable(_T("WP_lBlockProbeRow"),	&m_lBlockProbeRow);
	RegVariable(_T("WP_lBlockProbeCol"),	&m_lBlockProbeCol);
	RegVariable(_T("WP_lBlockProbeSpanRow"),	&m_lBlockProbeSpanRow);
	RegVariable(_T("WP_lBlockProbeSpanCol"),	&m_lBlockProbeSpanCol);

	// ESD Function
	RegVariable(_T("PBT_bProbeUsingESD"),		&m_bProbeUsingESD);
	// Free setting Lot Name
	RegVariable(_T("PBT_szCustomizedLotName"),	&m_szCustomizedLotName);

	RegVariable(_T("PBT_bStopUnloadInWaferEnd"),&m_bStopUnloadInWaferEnd);
	RegVariable(_T("PBT_bWaferEndReCheck"),		&m_bReCheckAfterWaferEnd);
	RegVariable(_T("WT_ucPrbItemMapDisplayMode"),	&m_ucPrbItemMapDisplayMode);
	RegVariable(_T("WT_lPrbKeyTableColumns"),		&m_lPrbKeyTableColumns);
}

BOOL CWaferTable::SaveCP100IndependentData()
{
	return TRUE;
}

BOOL CWaferTable::LoadCP100IndependentData()
{
	return TRUE;
}

BOOL CWaferTable::LoadCP100ComData()
{
	return TRUE;
}

BOOL CWaferTable::SaveCP100ComData()	// Save depends on machine
{
	return TRUE;
}


LONG CWaferTable::TeachProberOffsetXY(IPC_CServiceMessage &svMsg)
{
	REF_TYPE stInfo;
	LONG lStep;
	CString szLog;
	BOOL bReturn = TRUE;
	LONG lCurrX, lCurrY;
	
	svMsg.GetMsg(sizeof(LONG), &lStep);

	if (lStep == 0)
	{
		m_bCheckWaferLimit = FALSE;
		SetJoystickOn(TRUE);
	}
	else if (lStep == 1)
	{
		// try to search die if find die align
		SearchCurrentDie(FALSE, TRUE, 1, FALSE, stInfo);
		
		if (stInfo.bFullDie == TRUE)
		{
			GetEncoderValue();
			lCurrX = m_lEnc_X1 + stInfo.lX;	
			lCurrY = m_lEnc_Y1 + stInfo.lY;
			XY_MoveTo(lCurrX, lCurrY);
		}
		else
		{
			CString szText, szTitle;
			szTitle.LoadString(HMB_WT_TEACH_PROBER_OFFSET);
			szText.LoadString(HMB_WT_FIND_DIE_IN_CAM_POS_FAIL);
			HmiMessage(szText, szTitle);
		}
		
		Sleep(500);
		GetEncoderValue();
		m_lCameraRefPosX = GetCurrX();
		m_lCameraRefPosY = GetCurrY();
		LONG lGoX = 0, lGoY = 0;
		GetProbePosition(m_lCameraRefPosX, m_lCameraRefPosY, lGoX, lGoY);
		XY_MoveTo(lGoX, lGoY, SFM_WAIT);
	}
	else if (lStep == 2)
	{
		Sleep(500);
		GetEncoderValue();
		LONG lGoX = GetCurrX(), lOffsetX = 0;
		LONG lGoY = GetCurrY(), lOffsetY = 0;
		BOOL bUpdate = FALSE;
		m_lPrbCamOffsetX = 0;
		m_lPrbCamOffsetY = 0;
		if( WFT_SearchPrDie(lOffsetX, lOffsetY, TRUE) )
		{
			// Get the reply
			lGoX = lGoX + lOffsetX;
			lGoY = lGoY + lOffsetY;
			XY_MoveTo(lGoX, lGoY, SFM_WAIT);
			Sleep(500);
			bUpdate = TRUE;
		}

		Sleep(500);
		GetEncoderValue();
		m_lProbeRefPosX = GetCurrX();
		m_lProbeRefPosY = GetCurrY();

		m_lProbeOffsetX = m_lProbeRefPosX - m_lCameraRefPosX;
		m_lProbeOffsetY = m_lProbeRefPosY - m_lCameraRefPosY;
		WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
		if( bUpdate && pCPInfo->GetPODone() )
		{
			LONG lNewExtraX = m_lPOProbeOffsetX - m_lProbeOffsetX;
			LONG lNewExtraY = m_lPOProbeOffsetY - m_lProbeOffsetY;
			ULONG ulGridRows = CP_GetGridRows();
			ULONG ulGridCols = CP_GetGridCols();
			ULONG ulTotalNum = (ulGridRows+1)*(ulGridCols+1);
			// OldP + OldO = lNewP + lNewO ==> lNewO = OldP-lNewP + OldO
			LONG lScanX, lScanY, lPO_X, lPO_Y;
			for(ULONG i=1; i<=ulTotalNum; i++)
			{
				LONG lState = pCPInfo->GetPointPO(i, lScanX, lScanY, lPO_X, lPO_Y);
				if( lState==1 )
				{
					lPO_X = lPO_X + lNewExtraX;
					lPO_Y = lPO_Y + lNewExtraY;
					pCPInfo->SetPointPO(i, lScanX, lScanY, lPO_X, lPO_Y, lState);
				}
			}
			m_lPOProbeOffsetX	= m_lProbeOffsetX;
			m_lPOProbeOffsetY	= m_lProbeOffsetY;
		}

		SaveCP100ComData();

		m_bCheckWaferLimit = TRUE;
		SetJoystickOn(FALSE);
	}
	else if (lStep == 3)
	{
		m_bCheckWaferLimit = TRUE;
		SetJoystickOn(FALSE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CheckProberOffsetXY(IPC_CServiceMessage &svMsg)
{	// To Probe and To Cam button.
	BOOL bReturn = TRUE;

	bReturn = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CP_TogglePOState(IPC_CServiceMessage &svMsg)
{
	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();

	BOOL bState = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bState);
	pCPInfo->SetPOState(bState);

	CString szMsg;
	if( bState )
	{
		szMsg = "Probe offset compensation is enable";
	}
	else
	{
		szMsg = "Probe offset compensation is disable.\nProbe mark will be floating away!!!\nRemembe enable it after compare test done.";
	}
	HmiMessage_Red_Back(szMsg, "CP100");


	return 1;
}

LONG CWaferTable::CP_ToggleDCState(IPC_CServiceMessage &svMsg)
{
	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();

	BOOL bState = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bState);
	pCPInfo->SetDCState(bState);

	CString szMsg;
	if( bState )
	{
		szMsg = "Lcal die calibration is enable";
	}
	else
	{
		szMsg = "Lcal die calibration is disable.\nScan side die position will be drift!!!\nRemembe enable it after compare test done.";
	}
	HmiMessage_Red_Back(szMsg, "CP100");

	return 1;
}


LONG CWaferTable::MoveToProberOffsetXY(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	bReturn = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::MoveTableToUnloadToCleanProbePin(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lWhereToMove = 0;

	svMsg.GetMsg(sizeof(LONG), &lWhereToMove);

#ifdef NU_MOTION
	X1_Profile(LOW_PROF1);
	Y1_Profile(LOW_PROF1);
#endif

	switch( lWhereToMove )
	{
	case 3:
		if (XY1_MoveTo(m_lCPinPositionX, m_lCPinPositionY, SFM_WAIT) != gnOK)
		{
			bReturn = FALSE;
		}
		break;
	case 2:
		if (XY1_MoveTo(m_lWaferUnloadX, m_lWaferUnloadY, SFM_WAIT) != gnOK)
		{
			bReturn = FALSE;
		}
		break;

	case 0:
		if (XY1_MoveTo(m_stCurrentDie.lX_Enc, m_stCurrentDie.lY_Enc, SFM_WAIT) != gnOK)
		{
			bReturn = FALSE;
		}
		break;

	case 1:
	default:
		if (XY1_MoveTo(m_lHomeDieWftPosnX, m_lHomeDieWftPosnY, SFM_WAIT) != gnOK)
		{
			bReturn = FALSE;
		}
		break;
	}

#ifdef NU_MOTION
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SetWaferTableVacuumCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);
	
	//svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferTable::MoveToHome_ProberZ()
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest("BondHeadStn", "BH_MoveHome_ProberZ_Cmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bReturn;
}

BOOL CWaferTable::MoveToHome_ProberZ0()
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest("BondHeadStn", "BH_MoveHome_ProberZ0_Cmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bReturn;
}
LONG CWaferTable::UpdateProbeTableLevelByRegion(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lFirstIndexPoint=0, lSecondIndexPoint=0, lLastIndexPoint=0;
	LONG lPtX1=0, lPtY1=0, lPtZ1=0;		//Klocwork	//v4.41
	LONG lPtX2=0, lPtY2=0, lPtZ2=0;
	LONG lPtX3=0, lPtY3=0, lPtZ3=0;
	LONG lPosX=0, lPosY=0, lPosZ=0;
	LONG lNewPtZ1=0, lNewPtZ2=0, lNewPtZ3=0;
	LONG lPointNo=0;
	LONG lOffsetZ=0;
	CString szStr;

	// get the first point
	lPtX1 = m_stLearnLevelCornerPoints[0].lX;
	lPtY1 = m_stLearnLevelCornerPoints[0].lY;

	lFirstIndexPoint = 0;
	lSecondIndexPoint = m_lProbeTableDataSamplesX -1;
	lLastIndexPoint = m_lProbeTableDataSamplesY * m_lProbeTableDataSamplesX - 1;

	lPointNo = 0;
	for (INT j=0; j<=m_lProbeTableDataSamplesY; j++)
	{
		for (INT i=0; i<=m_lProbeTableDataSamplesX; i++)
		{
			lPointNo = lPointNo + 1;
			lPosX = lPtX1 - i*m_lProbeTableDataSamplesPitchX;
			lPosY = lPtY1 - j*m_lProbeTableDataSamplesPitchY;
			lPosZ = m_stProbeTableLevelData[lPointNo-1].lZ;
			
			if ((lPointNo - 1) == lFirstIndexPoint)
			{
				lPtX1 = lPosX;
				lPtY1 = lPosY;
				lPtZ1 = lPosZ;
			}
			else if ((lPointNo - 1) == lSecondIndexPoint)
			{
				lPtX2 = lPosX;
				lPtY2 = lPosY;
				lPtZ2 = lPosZ;
			}
			else if ((lPointNo - 1) == lLastIndexPoint)
			{
				lPtX3 = lPosX;
				lPtY3 = lPosY;
				lPtZ3 = lPosZ;
			}	
		}
	}

	XY_MoveTo(lPtX1, lPtX1);

	if (ProberZ_SearchAndGetContactLevel(lNewPtZ1) == FALSE)
	{
		ProberZ_MoveTo(0);
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	szStr.Format("Update table level point1 level Z old:%d new:%d", lPtZ1, lNewPtZ1);
	CMSLogFileUtility::Instance()->WT_WaferTableLevelLog(szStr);

	XY_MoveTo(lPtX2, lPtX2);

	if (ProberZ_SearchAndGetContactLevel(lNewPtZ2) == FALSE)
	{
		ProberZ_MoveTo(0);
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	szStr.Format("Update table level point2 level Z old:%d new:%d", lPtZ2, lNewPtZ2);
	CMSLogFileUtility::Instance()->WT_WaferTableLevelLog(szStr);

	XY_MoveTo(lPtX3, lPtX3);

	if (ProberZ_SearchAndGetContactLevel(lNewPtZ3) == FALSE)
	{
		ProberZ_MoveTo(0);
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	szStr.Format("Update table level point3 level Z old:%d new:%d", lPtZ3, lNewPtZ3);
	CMSLogFileUtility::Instance()->WT_WaferTableLevelLog(szStr);

	// average 3 points
	lOffsetZ = (lNewPtZ1 - lPtZ1 + lNewPtZ2 - lPtZ2, + lNewPtZ3 - lPtZ3)/3;

	szStr.Format("New Offset:%d", lOffsetZ);
	AfxMessageBox(szStr, MB_SYSTEMMODAL);

	szStr.Format("Update table level offset:%d", lOffsetZ);
	CMSLogFileUtility::Instance()->WT_WaferTableLevelLog(szStr);

	lPointNo = 0;
	for (INT j=0; j<=m_lProbeTableDataSamplesY; j++)
	{
		for (INT i=0; i<=m_lProbeTableDataSamplesX; i++)
		{
			lPointNo = lPointNo + 1;
			m_stProbeTableLevelData[lPointNo-1].lZ = m_stProbeTableLevelData[lPointNo-1].lZ + lOffsetZ ;
		}
	}

	SaveWaferTblData();
	CMSLogFileUtility::Instance()->WT_WaferTableLevelLog("Update Table Level Complete");

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::LearnProbeTableLevelByRegion(IPC_CServiceMessage& svMsg)
{
	CString szMsg, szTitle;
	LONG lPtX1=0, lPtY1=0, lPtT1=0;
	LONG lPtX2=0, lPtY2=0, lPtT2=0;
	LONG lPtX3=0, lPtY3=0, lPtT3=0;
	LONG lX=0, lY=0;
	BOOL bReturn = TRUE;
	CString szTemp;
	LONG lReturn=0;

	szTitle.LoadString(HMB_WT_LEARN_PROBE_LEVEL);
	szMsg.LoadString(HMB_WT_MOVE_PROBE_TABLE);

	m_bCheckWaferLimit = FALSE;
	SetJoystickOn(TRUE);

	lX = GetWaferCenterX() + m_lProbeOffsetX;
	lY = GetWaferCenterY() + m_lProbeOffsetY;
	if (IsWithinWaferLimit(lX, lY, 1, TRUE) == TRUE)
	{
		XY_MoveTo(lX, lY);
	}

	lReturn = HmiMessage("Use previous learn coners", szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	
	if (lReturn != glHMI_YES)
	{
		lReturn = HmiMessageEx(szMsg + " 1 (Upper Left Corner)", szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
		if (lReturn != glHMI_YES)
		{
			m_bCheckWaferLimit = TRUE;
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		
		GetEncoder(&lPtX1, &lPtY1, &lPtT1);
		
		// lock the joystick Y
		m_bXJoystickOn = TRUE;
		m_bYJoystickOn = FALSE;
		
		lReturn = HmiMessageEx(szMsg + " 2", szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
		if (lReturn != glHMI_YES)
		{
			m_bCheckWaferLimit = TRUE;
			m_bXJoystickOn = TRUE;
			m_bYJoystickOn = TRUE;
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		GetEncoder(&lPtX2, &lPtY2, &lPtT2);

		// lock the joystick X
		m_bXJoystickOn = FALSE;
		m_bYJoystickOn = TRUE;

		lReturn = HmiMessageEx(szMsg + " 3", szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
		if (lReturn != glHMI_YES)
		{
			m_bCheckWaferLimit = TRUE;
			m_bXJoystickOn = TRUE;
			m_bYJoystickOn = TRUE;
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		GetEncoder(&lPtX3, &lPtY3, &lPtT3);
		
		// save the points
		for (INT i=0; i<MS_LEARN_LEVEL_CORNER_POINT; i++)
		{
			LONG lX, lY;
			if (i == 0)
			{
				lX = lPtX1;
				lY = lPtY1;
			}
			else if (i == 1)
			{
				lX = lPtX2;
				lY = lPtY2;
			}
			else
			{
				lX = lPtX3;
				lY = lPtY3;
			}

			m_stLearnLevelCornerPoints[i].lX = lX;
			m_stLearnLevelCornerPoints[i].lY = lY;
		}

		// resume back
		SaveWaferTblData();
	}
	else
	{
		// restore the position
		for (INT i=0; i<MS_LEARN_LEVEL_CORNER_POINT; i++)
		{
			LONG lX, lY;

			lX = m_stLearnLevelCornerPoints[i].lX;
			lY = m_stLearnLevelCornerPoints[i].lY;

			if (i == 0)
			{
				lPtX1 = lX;
				lPtY1 = lY;
			}
			else if (i == 1)
			{
				lPtX2 = lX;
				lPtY2 = lY;
			}
			else
			{
				lPtX3 = lX;
				lPtY3 = lY;
			}
		}
	}

	m_bXJoystickOn = TRUE;
	m_bYJoystickOn = TRUE;
	m_bCheckWaferLimit = TRUE;
	SetJoystickOn(FALSE);
	
	if (m_lProbeTableDataSamplesX != 0)
	{
		// prevent round off problem
		m_lProbeTableDataSamplesPitchX = ((lPtX2 - lPtX1)/m_lProbeTableDataSamplesX); 
	}
	else
	{
		m_lProbeTableDataSamplesPitchX = lPtX2 - lPtX1;
	}
	m_lProbeTableDataSamplesPitchX = (LONG)(abs(m_lProbeTableDataSamplesPitchX) + 0.5);
	
	if (m_lProbeTableDataSamplesY != 0)
	{
		// prevent round off problem
		m_lProbeTableDataSamplesPitchY = ((lPtY3 - lPtY1)/m_lProbeTableDataSamplesY);
	}
	else
	{
		m_lProbeTableDataSamplesPitchY = lPtY3 - lPtY1;
	}
	m_lProbeTableDataSamplesPitchY = (LONG)(abs(m_lProbeTableDataSamplesPitchY) + 0.5);

	LONG lPosX, lPosY, lPosZ;
	LONG lPointNo = 0;

	CMSLogFileUtility::Instance()->WT_WaferTableLevelLog("Start Learn Table Level");

	for (INT j=0; j<=m_lProbeTableDataSamplesY; j++)
	{
		for (INT i=0; i<=m_lProbeTableDataSamplesX; i++)
		{
			lPointNo = lPointNo + 1;
			lPosX = lPtX1 - i*m_lProbeTableDataSamplesPitchX;
			lPosY = lPtY1 - j*m_lProbeTableDataSamplesPitchY;

			//if (IsWithinWT1WaferLimit(lPosX, lPosY, TRUE) == TRUE)
			//{
				XY_MoveTo(lPosX, lPosY);
			//}
			
			if (ProberZ_SearchAndGetContactLevel(lPosZ) == FALSE)
			{
				ProberZ_MoveTo(0);
				bReturn = TRUE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			//lPosZ = 10000;
			m_stProbeTableLevelData[lPointNo-1].lX = lPosX;
			m_stProbeTableLevelData[lPointNo-1].lY = lPosY;
			m_stProbeTableLevelData[lPointNo-1].lZ = lPosZ;

			szTemp.Format("%d, %d ,%d ,%d", lPointNo, lPosX, lPosY, lPosZ); 
			CMSLogFileUtility::Instance()->WT_WaferTableLevelLog(szTemp);
		}
	}

	ProberZ_MoveTo(0);

	lX = GetWaferCenterX() + m_lProbeOffsetX;
	lY = GetWaferCenterY() + m_lProbeOffsetY;
	if (IsWithinWaferLimit(lX, lY, 1, TRUE) == TRUE)
	{
		XY_MoveTo(lX, lY);
	}

	szMsg.LoadString(HMB_WT_LEARN_PROBE_LEVEL_COMPLETE);
	HmiMessageEx(szMsg, szTitle);

	SaveWaferTblData();
	CMSLogFileUtility::Instance()->WT_WaferTableLevelLog("Learn Table Level Complete");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::ReprobeAtSelectMap(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	if( IsPrescanEnded()==FALSE )
	{
		HmiMessage("It is not a probing wafer, no need to reprobe.", "CP Machine");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	ULONG ulAlnRow, ulAlnCol;
	m_WaferMapWrapper.GetSelectedPosition(ulAlnRow, ulAlnCol);
	ReprobeAtMapToCleanUp(ulAlnRow, ulAlnCol);	// new select during probing

	m_WaferMapWrapper.EnableAutoAlign(FALSE);
	unsigned char aaTempGrades[1];
	aaTempGrades[0] = m_WaferMapWrapper.GetGradeOffset() + m_ucDummyPrescanPNPGrade;

	m_WaferMapWrapper.SetSelectedPosition(ulAlnRow, ulAlnCol);
	m_WaferMapWrapper.SetCurrentPosition(ulAlnRow, ulAlnCol);
	m_WaferMapWrapper.SetStartPosition(ulAlnRow, ulAlnCol);
	m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aaTempGrades[0]);
	m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);
	m_WaferMapWrapper.Redraw();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	// reprobe at map for probing map.

LONG CWaferTable::RecoverCrashedMap(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	HmiMessage("It is NOT a prober machine, NO need to recovery.", "CP Machine");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}

LONG CWaferTable::ReprobeWholeMap(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	if( IsPrescanEnded()==FALSE )
	{
		HmiMessage("It is not a probing wafer, no need to reprobe.", "CP Machine");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	ULONG ulAlnRow = 0, ulAlnCol = 0;
//	m_WaferMapWrapper.GetSelectedPosition(ulAlnRow, ulAlnCol);
	ReprobeAtMapToCleanUp(ulAlnRow, ulAlnCol);	// new select during probing

	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	BOOL bFindFirst = FALSE;

	for(LONG lRow = 0; lRow <= GetMapValidMaxRow(); lRow++)
	{
		for(LONG lCol = 0; lCol <= GetMapValidMaxCol(); lCol++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lRow, lCol) - ucOffset;
			if( ucGrade==m_ucDummyPrescanPNPGrade )
			{
				ulAlnRow = lRow;
				ulAlnCol = lCol;
				bFindFirst = TRUE;
				break;
			}
		}
		if( bFindFirst )
		{
			break;
		}
	}

	m_WaferMapWrapper.EnableAutoAlign(FALSE);
	unsigned char aaTempGrades[1];
	aaTempGrades[0] = m_WaferMapWrapper.GetGradeOffset() + m_ucDummyPrescanPNPGrade;

	m_WaferMapWrapper.SetSelectedPosition(ulAlnRow, ulAlnCol);
	m_WaferMapWrapper.SetCurrentPosition(ulAlnRow, ulAlnCol);
	m_WaferMapWrapper.SetStartPosition(ulAlnRow, ulAlnCol);
	m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aaTempGrades[0]);
	m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);
	m_WaferMapWrapper.Redraw();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	// reprobe whole map.


BOOL CWaferTable::ManualGenerateProberMapFile(IPC_CServiceMessage& svMsg)
{
	LONG lReturn;
	BOOL bReturn = TRUE;
	CString szTitle, szContent;

	szContent.LoadString(HMB_WT_ARE_YOU_SURE_TO_GEN_MAP_FILE);
	szTitle.LoadString(HMB_WT_GEN_MAP);
	
	lReturn = HmiMessage(szContent, szTitle,  glHMI_MBX_OKCANCEL);
	
	if (lReturn == glHMI_CANCEL)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsProbingEnd() && m_bReCheckAfterWaferEnd )
	{
		IPC_CServiceMessage stMsg;
		INT nConvID = m_comClient.SendRequest("BinTableStn", "ManualWaferEnd", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		m_bIsWaferProbed = FALSE;
		m_bIsWaferEnded	 = FALSE;
	}

	if (WriteProberMapFile() == TRUE)	//	manual mode by HMI button
	{
		WaferMapTempOperation(-1);	// manual wafer output map, empty folder and reset flag.
		// clear the map file as temp file is deleted
		m_WaferMapWrapper.InitMap();	//	after generate output map file manually, clear it.
		CreateItemGradeDummyMap();	// manual generate probe output map file
		m_ulNewPickCounter = 0;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CheckIsBlockProbe(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = IsBlockProbe();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	block probe

BOOL CWaferTable::ProberZ_SearchAndGetContactLevel(LONG& lLevelZ)
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	INT nConvID = 0;

	typedef struct
	{
		BOOL bResult;
		LONG lEnc;
	}CONTACT_LEVEL;
 	
	CONTACT_LEVEL stLevel;

	nConvID = m_comClient.SendRequest("BondHeadStn", "BH_ProberZ_SearchAndGetContactLevel", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(CONTACT_LEVEL), &stLevel);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	lLevelZ = stLevel.lEnc;

	return stLevel.bResult;
}

BOOL CWaferTable::ProberZ_MoveTo(LONG lLevelZ)
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	INT nConvID = 0;

	stMsg.InitMessage(sizeof(LONG), &lLevelZ);
	nConvID = m_comClient.SendRequest("BondHeadStn", "BH_MoveTo_ProberZ", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bReturn;
}

//	scan barcode, get map name, check repeat name or not (item 3), check test condition(item 4)
LONG CWaferTable::GetConditionPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		bReturn = pAppMod->GetPath(m_szConditionFilePath);
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::LoadConditionFile(IPC_CServiceMessage &svMsg)
{
	CString szColumnFilename, szFile, szUserFile;
	CString szInitPath;

	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]= "Csv File (*.csv)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "csv", "*.csv", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , szFilters, pMainWnd, 0);

	szInitPath = m_szConditionFilePath;
	dlgFile.m_ofn.lpstrInitialDir = szInitPath;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	if ( nReturn == IDOK )
	{
		szColumnFilename = m_szConditionFilePath + "\\" + dlgFile.GetFileName();
		CString szTestConditionFileName = gszUSER_DIR_MAPFILE_PROBER + "\\" + "HCTestCondition.csv";
		CopyFileWithRetry(szColumnFilename, szTestConditionFileName, FALSE);
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end 

LONG CWaferTable::GetTesterTSFPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		bReturn = pAppMod->GetPath(m_szTesterTSFPath);
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetResortingPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		bReturn = pAppMod->GetPath(m_szResortingFilePath);
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::LoadResortingFile(IPC_CServiceMessage &svMsg)
{
	CString szColumnFilename, szFile, szUserFile;
	CString szInitPath;

	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]= "Csv File (*.csv)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "csv", "*.csv", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , szFilters, pMainWnd, 0);

	szInitPath = m_szResortingFilePath;
	dlgFile.m_ofn.lpstrInitialDir = szInitPath;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	if ( nReturn == IDOK )
	{
		szColumnFilename = m_szResortingFilePath + "\\" + dlgFile.GetFileName();
		CString szResortingFileName = gszUSER_DIR_MAPFILE_PROBER + "\\" + "HCResortingSpec.csv";
		CopyFileWithRetry(szColumnFilename, szResortingFileName, FALSE);
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
} //end 

LONG CWaferTable::LoadNearCheckCriteriaFile(IPC_CServiceMessage &svMsg)
{
	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;
	static char szFilters[]= "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.*", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pMainWnd, 0);
	dlgFile.m_ofn.lpstrInitialDir = gszUSER_DIRECTORY;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	INT nReturn = dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	BOOL bReturn = TRUE;
	if ( nReturn != IDOK )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szFilename = dlgFile.GetPathName();	// full path and name and ext. best.

	m_lContinueFailLimit	= 0;
	m_lItemGradeChangeDir		= 0;
	for(int j=0; j<CP_ITEM_MAX_LIMIT; j++)
	{
		m_dMinTested[j]	= CP_ITEM_INVALID_VALUE;
		m_dMaxTested[j]	= CP_ITEM_INVALID_VALUE;
		m_dTolTested[j]	= CP_ITEM_INVALID_VALUE;
		m_szaItemList[j] = "";
	}

	if( (access(szFilename, 0 )) == -1 )
	{
		HmiMessage_Red_Back("Select file can not access!", "Prober");
	}
	else
	{
		CString szNearCheckFileName = gszUSER_DIR_MAPFILE_PROBER + "\\" + "NearCheck.csv";
		CopyFileWithRetry(szFilename, szNearCheckFileName, FALSE);

		CStdioFile fCheckFile;
		if( fCheckFile.Open(szNearCheckFileName, CFile::modeRead|CFile::shareDenyNone) )
		{
			CString szReading, szItemValue;
			CStringArray szaDataList;
			fCheckFile.SeekToBegin();

			// get the fail counter limit
			if( fCheckFile.ReadString(szReading) )
			{
				szaDataList.RemoveAll();
				ParseRawData(szReading, szaDataList);
				if( szaDataList.GetSize()>=2 )
				{
					szItemValue = szaDataList.GetAt(1);
					m_lContinueFailLimit = atoi(szItemValue);
				}
			}
			// title header for easy reading
			if( fCheckFile.ReadString(szReading) )
			{
				szaDataList.RemoveAll();
				ParseRawData(szReading, szaDataList);
				for(int j=1; j<szaDataList.GetSize(); j++)
				{
					if( j>CP_ITEM_MAX_LIMIT )
						break;
					m_szaItemList[j-1] = szaDataList.GetAt(j);
				}
			}
			// min value part
			if( fCheckFile.ReadString(szReading) )
			{
				szaDataList.RemoveAll();
				ParseRawData(szReading, szaDataList);
				for(int j=1; j<szaDataList.GetSize(); j++)
				{
					if( j>CP_ITEM_MAX_LIMIT )
						break;
					szItemValue = szaDataList.GetAt(j);
					if( szItemValue.IsEmpty()==FALSE )
						m_dMinTested[j-1] = atof(szItemValue);
				}
			}
			// max value part
			if( fCheckFile.ReadString(szReading) )
			{
				szaDataList.RemoveAll();
				ParseRawData(szReading, szaDataList);
				for(int j=1; j<szaDataList.GetSize(); j++)
				{
					if( j>CP_ITEM_MAX_LIMIT )
						break;
					szItemValue = szaDataList.GetAt(j);
					if( szItemValue.IsEmpty()==FALSE )
						m_dMaxTested[j-1] = atof(szItemValue);
				}
			}
			// tol value part
			if( fCheckFile.ReadString(szReading) )
			{
				szaDataList.RemoveAll();
				ParseRawData(szReading, szaDataList);
				for(int j=1; j<szaDataList.GetSize(); j++)
				{
					if( j>CP_ITEM_MAX_LIMIT )
						break;
					szItemValue = szaDataList.GetAt(j);
					if( szItemValue.IsEmpty()==FALSE )
						m_dTolTested[j-1]	= atof(szItemValue);
				}
			}
		}
		fCheckFile.Close();

		SaveWaferTblData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CreateNearCheckDummyFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CStringArray szaTestItemList;
	szaTestItemList.RemoveAll();
	GetTestingItemsForNearCheckDummy(szaTestItemList);
	if( szaTestItemList.GetSize()==0 )
	{
		HmiMessage("Fail to connect test and can not get testing items.", "Prober");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szNearCheckFileName = gszUSER_DIR_MAPFILE_PROBER + "\\" + "NearCheckDummy.csv";
	CStdioFile fCheckFile;
	remove(szNearCheckFileName);
	if( fCheckFile.Open(szNearCheckFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText) )
	{
		CString szReading, szItemValue;
		fCheckFile.SeekToBegin();

		// write fail counter limit
		szReading.Format("fail counter,0\n");	// fail limit
		fCheckFile.WriteString(szReading);

		// write check header title
		szReading = "Item";
		for(int i=0; i<szaTestItemList.GetSize(); i++)
		{
			szItemValue = "," + szaTestItemList.GetAt(i);
			szReading += szItemValue;
		}
		szReading += "\n";
		fCheckFile.WriteString(szReading);

		// write Min limit value, if testing less than this value, just skip
		szReading = "Min";
		for(int i=0; i<szaTestItemList.GetSize(); i++)
		{
			szItemValue = ",";
			szReading += szItemValue;
		}
		szReading += "\n";
		fCheckFile.WriteString(szReading);
		// write Max limit value,	if testing large than this value, just skip
		szReading = "Max";
		for(int i=0; i<szaTestItemList.GetSize(); i++)
		{
			szItemValue = ",";
			szReading += szItemValue;
		}
		szReading += "\n";
		fCheckFile.WriteString(szReading);
		// write Tol limit value, if difference large this tolerance value, counter increase.
		szReading = "Tol";
		for(int i=0; i<szaTestItemList.GetSize(); i++)
		{
			szItemValue = ",";
			szReading += szItemValue;
		}
		szReading += "\n";
		fCheckFile.WriteString(szReading);
		bReturn = TRUE;
		HmiMessage("Success to create file:" + szNearCheckFileName, "Prober");
	}
	else
	{
		HmiMessage("Fail to create file:" + szNearCheckFileName, "Prober");
	}
	fCheckFile.Close();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::LoadDailyCheckStandard(IPC_CServiceMessage &svMsg)
{
	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;
	static char szFilters[]= "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.*", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pMainWnd, 0);
	dlgFile.m_ofn.lpstrInitialDir = gszUSER_DIRECTORY;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	INT nReturn = dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	BOOL bReturn = TRUE;
	if ( nReturn != IDOK )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szFilename = dlgFile.GetPathName();	// full path and name and ext. best.

	if( (access(szFilename, 0 )) == -1 )
	{
		HmiMessage_Red_Back("Select file can not access!", "Prober");
	}
	else
	{
		CopyFileWithRetry(szFilename, gszPrbDCStdFilename, FALSE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// for item 6, check adjacent die testing value difference and alarm.
LONG CWaferTable::SelectProberSampleCheckMode(IPC_CServiceMessage &svMsg)
{
	LONG lNewMode = 0;

	svMsg.GetMsg(sizeof(LONG), &lNewMode);

	m_lPSCMode = lNewMode;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::BeforeIntoMapPage(IPC_CServiceMessage& svMsg)
{
	BOOL bOldState = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOldState);
	m_bPrevMapPageOn = bOldState;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//	CP Map Display
LONG CWaferTable::IntoItemGradeMapPage(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 0;
}

LONG CWaferTable::SpinItemMapDisplay(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lDir = 0;
	svMsg.GetMsg(sizeof(LONG),	&lDir);
	if( lDir == 0 )
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	if( lDir<0 && m_ulCurrMapItemIndex<=0 )
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	if( lDir>0 && (m_ulCurrMapItemIndex+1)>=(ULONG)m_szaGradeItemList.GetSize() )
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	if( lDir<0 )
	{
		m_ulCurrMapItemIndex--;
	}
	else
	{
		m_ulCurrMapItemIndex++;
	}
	if ((State() == AUTO_Q) || (State() == DEMO_Q))
	{
		m_lItemGradeChangeDir = 1;
	}

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::SelectItemMapDisplay(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LPTSTR lpszItemName = new CHAR[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), lpszItemName);
	CString szItemName = &lpszItemName[0];
	delete [] lpszItemName;

	if( m_szaGradeItemList.GetSize() <= 3 )
	{
		m_ulCurrMapItemIndex = 0;
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	BOOL bFindOne = FALSE;
	ULONG ulTgtIndex = m_ulCurrMapItemIndex;
	for(int i=0; i<m_szaGradeItemList.GetSize(); i++)
	{
		if( szItemName == m_szaGradeItemList.GetAt(i) )
		{
			ulTgtIndex = i;
			bFindOne = TRUE;
			break;
		}
	}

	if( ulTgtIndex > (ULONG)(m_szaGradeItemList.GetSize()-3) )
		ulTgtIndex = m_szaGradeItemList.GetSize()-3;

	if( ulTgtIndex==m_ulCurrMapItemIndex )
	{
		bFindOne = FALSE;
	}

	if( bFindOne==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	m_ulCurrMapItemIndex = ulTgtIndex;

	if ((State() == AUTO_Q) || (State() == DEMO_Q))
	{
		m_lItemGradeChangeDir = 1;
	}

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::GetItemGradesListFromTester(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 0;
}

BOOL CWaferTable::CreateItemGradeDummyMap()
{
	SearchAndRemoveKeyFiles(gszUSER_DIR_MAPFILE_PROBER, ".map");
	if( m_szaGradeItemList.GetSize()<=0 )
		return TRUE;

	ObtainMapValidRange();

	UCHAR ucItemMapDivide = GetItemMapDisplayRatio();
	CString szItemName = m_szaGradeItemList.GetAt(0);
	CStdioFile cfFile;
	CString szRootFile = gszUSER_DIR_MAPFILE_PROBER + "\\Map_" + szItemName + ".map";
	DeleteFile(szRootFile);
	if (cfFile.Open(szRootFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		CString szData;
		cfFile.WriteString("DataFileName,,\nLotNumber,,\nDeviceNumber,,\nwafer id=\nTestTime,\nMapFileName,,\nTransferTime,\n\n");
		cfFile.WriteString("map data\n");
		LONG lItemDieRow = GetMapValidMinRow() / ucItemMapDivide;
		LONG lItemDieCol = GetMapValidMinCol() / ucItemMapDivide;
		szData.Format("%ld,%ld,%ld", lItemDieCol, lItemDieRow, 0);
		cfFile.WriteString(szData + "\n");
		lItemDieRow = GetMapValidMaxRow() / ucItemMapDivide;
		lItemDieCol = GetMapValidMaxCol() / ucItemMapDivide;
		szData.Format("%ld,%ld,%ld", lItemDieCol, lItemDieRow, 0);
		cfFile.WriteString(szData + "\n");
		cfFile.Close();
	}

	for(int i=1; i<m_szaGradeItemList.GetSize(); i++)
	{
		CString szTargetFile = gszUSER_DIR_MAPFILE_PROBER + "\\Map_" + m_szaGradeItemList.GetAt(i) + ".map";
		remove(szTargetFile);
		CopyFile(szRootFile, szTargetFile, FALSE);
	}

	return TRUE;
}

LONG CWaferTable::LoadItemGradeMap()
{
	return TRUE;
}




LONG CWaferTable::IntoItemGradeSetupPage(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 0;
}

// Key Files Operation
LONG CWaferTable::CpSkipKeyConfigFile(IPC_CServiceMessage &svMsg)
{
	CString szHmiFile = PB_OUTPUT_KEY_CONFIGHMI_FILE;
	CString szTgtFile = PB_OUTPUT_KEY_CONFIG_FILE;
	CStdioFile cFile;
	if( cFile.Open(szTgtFile, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szMsg;
		CStringArray szAMsg;
		szAMsg.RemoveAll();
		cFile.SeekToBegin();
		cFile.ReadString(szMsg);
		ParseRawData(szMsg, szAMsg);
		m_lPrbKeyTableColumns		= szAMsg.GetSize();
		cFile.Close();
	}
	//Copy user file to replace ASM file
	CopyFile(szTgtFile, szHmiFile, FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::CpSaveKeyConfigFile(IPC_CServiceMessage &svMsg)
{
	CString szHmiFile = PB_OUTPUT_KEY_CONFIGHMI_FILE;
	CString szTgtFile = PB_OUTPUT_KEY_CONFIG_FILE;
	CStdioFile cFile;
	if( cFile.Open(szTgtFile, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szMsg;
		CStringArray szAMsg;
		szAMsg.RemoveAll();
		cFile.SeekToBegin();
		cFile.ReadString(szMsg);
		ParseRawData(szMsg, szAMsg);
		m_lPrbKeyTableColumns		= szAMsg.GetSize();
		cFile.Close();
	}
	//Copy user file to replace ASM file
	CopyFile(szHmiFile, szTgtFile, FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::CpLoadKeyConfigFile(IPC_CServiceMessage &svMsg)
{
	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]= "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.txt", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pMainWnd, 0);

	dlgFile.m_ofn.lpstrInitialDir = m_szKeyFilesPath;	//	"C:\\MapSorter\\UserData";
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	if ( nReturn == IDOK )
	{
		CString szSrcFile = dlgFile.GetPathName();
		CString szTgtFile = PB_OUTPUT_KEY_CONFIG_FILE;
		//Copy user file to replace ASM file
		if( szSrcFile.CompareNoCase(szTgtFile)!=0 )
		{
			CopyFile(szSrcFile, szTgtFile, FALSE);
		}

		CStdioFile cFile;
		if( cFile.Open(szTgtFile, CFile::modeRead|CFile::shareDenyNone) )
		{
			CString szMsg;
			CStringArray szAMsg;
			szAMsg.RemoveAll();
			cFile.SeekToBegin();
			cFile.ReadString(szMsg);
			ParseRawData(szMsg, szAMsg);
			m_lPrbKeyTableColumns		= szAMsg.GetSize();
			cFile.Close();
		}
		CString szHmiFile = PB_OUTPUT_KEY_CONFIGHMI_FILE;
		CopyFile(szTgtFile, szHmiFile, FALSE);
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

LONG CWaferTable::CpLoadMapGradeColorFile(IPC_CServiceMessage &svMsg)
{
	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]= "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.txt", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pMainWnd, 0);

	dlgFile.m_ofn.lpstrInitialDir = m_szKeyFilesPath;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	if ( nReturn == IDOK )
	{
		CString szSrcFile = dlgFile.GetPathName();

		if (_access(szSrcFile,0) != -1)
		{
			m_szMapGradeColorFileName = dlgFile.GetFileName();
			CString szTgtFile = gszAsmHmiColorFile;
			//Copy user file to replace ASM file
			if( szSrcFile.CompareNoCase(szTgtFile)!=0 )
			{
				CopyFile(szSrcFile, szTgtFile, FALSE);
			}

			CpUpdateMapGradeColor();

			SaveData();
		}
		else
		{
			HmiMessage("Grade Color File Missing!" + szSrcFile);
		}
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

LONG CWaferTable::CpSaveMapGradeColorFile(IPC_CServiceMessage& svMsg)
{
	CString szSrcFile = gszAsmHmiColorFile;
	CString szTgtFile = m_szKeyFilesPath + "\\" + m_szMapGradeColorFileName;
	if( szSrcFile.CompareNoCase(szTgtFile)!=0 )
	{
		CopyFile(szSrcFile, szTgtFile, FALSE);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::CpLoadMapDisplayItemsFile(IPC_CServiceMessage& svMsg)
{
	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]= "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.txt", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pMainWnd, 0);

	dlgFile.m_ofn.lpstrInitialDir = m_szKeyFilesPath;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	if ( nReturn != IDOK )
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	CString szFileName = dlgFile.GetPathName();
	BOOL bReturn = LoadMapDisplayItemsFile(szFileName);
	if( bReturn )
	{
		m_szMapDisplayItemFileName = dlgFile.GetFileName();
	}

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

BOOL CWaferTable::LoadMapDisplayItemsFile(CString szFileName)
{
	CStdioFile fCheckFile;
	if( fCheckFile.Open(szFileName, CFile::modeRead|CFile::shareDenyNone) )
	{
		m_szaGradeItemList.RemoveAll();
		memset(m_daBinMapItemMin, 0, sizeof(m_daBinMapItemMin));
		memset(m_daBinMapItemMax, 0, sizeof(m_daBinMapItemMax));
		memset(m_daBinMapItemStep, 0, sizeof(m_daBinMapItemStep));
		CString szReading, szItemName;
		CStringArray szaDataList;
		USHORT usIndex = 0;
		fCheckFile.SeekToBegin();

		// get the fail counter limit
		while( fCheckFile.ReadString(szReading) )
		{
			szaDataList.RemoveAll();
			ParseRawData(szReading, szaDataList);
			if( szaDataList.GetSize()>=4 )
			{
				szItemName = szaDataList.GetAt(0);
				m_szaGradeItemList.Add(szItemName);
				szItemName = szaDataList.GetAt(1);
				m_daBinMapItemMin[usIndex] = atof(szItemName);
				szItemName = szaDataList.GetAt(2);
				m_daBinMapItemMax[usIndex] = atof(szItemName);
				szItemName = szaDataList.GetAt(3);
				m_daBinMapItemStep[usIndex] = atof(szItemName);
			}
			usIndex ++;
			if( usIndex>=CP_ITEM_MAX_LIMIT )
			{
				break;
			}
		}
		fCheckFile.Close();

		CreateItemGradeDummyMap();	//	load item map list file

		SaveWaferTblData();
		UpdateItemMapList();

		return TRUE;
	}

	HmiMessage(szFileName + " is unaccessible.", "Prober");
	return FALSE;
}

LONG CWaferTable::CpSaveMapDisplayItemsFile(IPC_CServiceMessage& svMsg)
{
	CString szFileName = m_szKeyFilesPath + "\\" + m_szMapDisplayItemFileName;
	DeleteFile(szFileName);
	CStdioFile fCheckFile;
	if( fCheckFile.Open(szFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText) )
	{
		for(int i=0; i<m_szaGradeItemList.GetSize(); i++)
		{
			if( i>=CP_ITEM_MAX_LIMIT )
			{
				break;
			}
			CString szText;
			szText.Format("%s,%f,%f,%f\n", m_szaGradeItemList.GetAt(i), 
				m_daBinMapItemMin[i], m_daBinMapItemMax[i], m_daBinMapItemStep[i]);
			fCheckFile.WriteString(szText);
		}
		fCheckFile.Close();
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::GetKeyFilesPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		bReturn = pAppMod->GetPath(m_szKeyFilesPath);
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::AddItemToBinMapList(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LPTSTR lpszItemName = new CHAR[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), lpszItemName);
	CString szItemName = &lpszItemName[0];
	delete [] lpszItemName;

	BOOL bFindOne = FALSE;
	ULONG ulTgtIndex = 0;
	for(int i=0; i<m_szaGradeItemList.GetSize(); i++)
	{
		if( szItemName == m_szaGradeItemList.GetAt(i) )
		{
			ulTgtIndex = i;
			bFindOne = TRUE;
			break;
		}
	}

	if( bFindOne==FALSE )
	{
		m_szaGradeItemList.Add(szItemName);

		SaveWaferTblData();

		UpdateItemMapList();
	}

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::RemoveItemFromBinMapList(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if( m_szaGradeItemList.GetSize() <= 0 )
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}


	ULONG ulTgtIndex = m_ulCurrSetupItemIndex;
	m_szaGradeItemList.RemoveAt(ulTgtIndex);
	m_daBinMapItemMin[ulTgtIndex]	= 0;
	m_daBinMapItemMax[ulTgtIndex]	= 0;
	m_daBinMapItemStep[ulTgtIndex]	= 0;
	if( ulTgtIndex >= (ULONG)(m_szaGradeItemList.GetSize()) && m_szaGradeItemList.GetSize() > 0 )
	{
		m_ulCurrSetupItemIndex = m_szaGradeItemList.GetSize()-1;
	}

	UpdateItemMapList();
	SaveWaferTblData();

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

BOOL CWaferTable::UpdateItemMapList()
{
	CStdioFile cfFile;
	CString szRootFile = gszUSER_DIR_MAPFILE_PROBER + "\\ItemGradeList.csv" ;
	remove(szRootFile);
	if (cfFile.Open(szRootFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		for(int i=0; i<m_szaGradeItemList.GetSize(); i++)
		{
			cfFile.WriteString(m_szaGradeItemList.GetAt(i) + ",");
		}
		cfFile.Close();
	}

	ULONG ulTgtIndex = m_ulCurrSetupItemIndex;

	if( ulTgtIndex<CP_ITEM_MAX_LIMIT )
	{
		m_dHmiBinItemMin	= m_daBinMapItemMin[ulTgtIndex];
		m_dHmiBinItemMax	= m_daBinMapItemMax[ulTgtIndex];
		m_dHmiBinItemStep	= m_daBinMapItemStep[ulTgtIndex];
	}

	return TRUE;
}

UCHAR CWaferTable::GetItemTopGrade(ULONG ulIndex)
{
	UCHAR ucStopGrade = 0;
	if( m_daBinMapItemStep[ulIndex]!=0 )
		ucStopGrade = (UCHAR) ((m_daBinMapItemMax[ulIndex]-m_daBinMapItemMin[ulIndex])/m_daBinMapItemStep[ulIndex]) + 2;

	return ucStopGrade;
}

LONG CWaferTable::SelectBinMapItemToSetRange(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LPTSTR lpszItemName = new CHAR[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), lpszItemName);
	CString szItemName = &lpszItemName[0];
	delete [] lpszItemName;

	m_szCurrGradeItemName = szItemName;
	if( m_szaGradeItemList.GetSize() <= 0 )
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	BOOL bFindOne = FALSE;
	ULONG ulTgtIndex = m_ulCurrSetupItemIndex;
	for(int i=0; i<m_szaGradeItemList.GetSize(); i++)
	{
		if( szItemName == m_szaGradeItemList.GetAt(i) )
		{
			ulTgtIndex = i;
			bFindOne = TRUE;
			break;
		}
	}

	if( bFindOne==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	m_ulCurrSetupItemIndex = ulTgtIndex;

	if( ulTgtIndex<CP_ITEM_MAX_LIMIT )
	{
		m_dHmiBinItemMin	= m_daBinMapItemMin[ulTgtIndex];
		m_dHmiBinItemMax	= m_daBinMapItemMax[ulTgtIndex];
		m_dHmiBinItemStep	= m_daBinMapItemStep[ulTgtIndex];
	}

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CWaferTable::ConfirmBinMapItemRange(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	DOUBLE dDiff = m_dHmiBinItemMax - m_dHmiBinItemMin;
	if( dDiff<0 )
	{
		HmiMessage("Max  <  Min, please set it again!");
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}
	if( m_dHmiBinItemStep==0 )
	{
		HmiMessage("Step is 0, please set it again!");
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}
	if( (m_dHmiBinItemStep*253.0)<=dDiff )
	{
		HmiMessage("Step is too small!\nTotal grade will over 255!\nPlease set it again!");
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	UCHAR ucItemMapDivide = GetItemMapDisplayRatio();
	ULONG ulTgtIndex = m_ulCurrSetupItemIndex;
	if( ulTgtIndex<CP_ITEM_MAX_LIMIT )
	{
		m_daBinMapItemMin[ulTgtIndex]	= m_dHmiBinItemMin;
		m_daBinMapItemMax[ulTgtIndex]	= m_dHmiBinItemMax;
		m_daBinMapItemStep[ulTgtIndex]	= m_dHmiBinItemStep;
		SaveWaferTblData();
	}

	CreateItemGradeDummyMap();	// idle mode, modify range

	BOOL bReDo = FALSE;
	CStdioFile cfRead;
	CString szItemDataFile = CP_ITEM_GRADE_DATA_FILE;	//	change item setting range, regenerate map file
	if (access(szItemDataFile, 0) != -1)
	{
		if( cfRead.Open(szItemDataFile, CFile::modeNoTruncate | CFile::modeRead | CFile::shareExclusive | CFile::typeText) )
		{
			bReDo = TRUE;
		}
		else
		{
			SetAlarmLog(szItemDataFile + " open fail");
		}
	}
	else
	{
		SetAlarmLog(szItemDataFile + " missing");
	}

	if( bReDo && m_szaGradeItemList.GetSize()>0 )
	{
		CString szData;
		cfRead.SeekToBegin();
		while (cfRead.ReadString(szData))
		{
			CStringArray szaList;
			szaList.RemoveAll();
			ParseRawData(szData, szaList);
			if( szaList.GetSize()<=3 )
			{
				continue;
			}

			LONG lAsmMapRow = atoi(szaList.GetAt(0));
			LONG lAsmMapCol = atoi(szaList.GetAt(1));
			ULONG ulItemDieRow = 0, ulItemDieCol = 0;
			if( (m_ucMapRotation > 0)  )
				ConvertOrgUserToAsm(lAsmMapRow, lAsmMapCol, ulItemDieRow, ulItemDieCol);
			else
				ConvertHmiUserToAsm(lAsmMapRow, lAsmMapCol, ulItemDieRow, ulItemDieCol);

			ulItemDieRow = ulItemDieRow / ucItemMapDivide;
			ulItemDieCol = ulItemDieCol / ucItemMapDivide;

			for(int i=0; i<szaList.GetSize()-3; i++)
			{
				if( i >= m_szaGradeItemList.GetSize() )
				{
					break;
				}
				if (i >= CP_ITEM_MAX_LIMIT)		//Klocwork	//v4.46
				{
					break;
				}

				DOUBLE dValue = CP_ITEM_INVALID_VALUE;
				if ( IsItemValid(szaList.GetAt(i+3)) )
				{
					dValue = atof(szaList.GetAt(i+3));
				}

				UCHAR ucItemGrade = 0;
				if (dValue < m_daBinMapItemMin[i])
				{
					ucItemGrade = 0;
				}
				else if (dValue > m_daBinMapItemMax[i])
				{
					ucItemGrade = GetItemTopGrade(i);
				}
				else
				{
					if (m_daBinMapItemStep[i] != 0)
					{
						ucItemGrade = (UCHAR) ((dValue-m_daBinMapItemMin[i])/m_daBinMapItemStep[i]) + 1;
					}
				}

				if( m_bBinMapFileOpen[i] )
				{
					CString szData;
					szData.Format("%ld,%ld,%d", ulItemDieCol, ulItemDieRow, ucItemGrade);
					m_cfBinMapFiles[i].WriteString(szData + "\n");
				}
			}	//	raw data list loop
		}	//	file loop reading
	}
	cfRead.Close();

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}	// confirm item range

BOOL CWaferTable::InitProberMap()
{
	return TRUE;
}

LONG CWaferTable::BurnInAlignWafer(IPC_CServiceMessage &svMsg)
{
#ifdef NU_MOTION
	X1_Profile(LOW_PROF1);
	Y1_Profile(LOW_PROF1);
#endif

	SelectWaferCamera();

	MoveToHome_ProberZ0();
	
	Sleep(200);

	XY1_MoveTo(m_lHomeDieWftPosnX, m_lHomeDieWftPosnY, SFM_WAIT);
	Sleep(200);
	MoveToHome_ProberZ();
#ifdef NU_MOTION
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
#endif

	ManualAlignWafer(svMsg);

	return 1;
}

// CP100 ACP cmd coding
LONG CWaferTable::ACPSetCorner(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;

	LONG lCorner = 0;
	svMsg.GetMsg(sizeof(LONG), &lCorner);

	m_lACPTableT  = GetCurrT1();
	switch( lCorner )
	{
	case 0:	// UL
		m_lACPCornerX	= m_lACPUpLeftX;
		m_lACPCornerY	= m_lACPUpLeftY;
		m_szACPCornerX	= "Up Left X";
		m_szACPCornerY	= "Up Left Y";
		break;
	case 1:	// UR
		m_lACPCornerX	= m_lACPUpRightX;
		m_lACPCornerY	= m_lACPUpRightY;
		m_szACPCornerX	= "Up Right X";
		m_szACPCornerY	= "Up Right Y";
		break;
	case 2:	// LL
		m_lACPCornerX	= m_lACPLowLeftX;
		m_lACPCornerY	= m_lACPLowLeftY;
		m_szACPCornerX	= "Low Left X";
		m_szACPCornerY	= "Low Left Y";
		break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferTable::SetCleanPinUpLeftCorner(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
}

LONG CWaferTable::ConfirmCleanPinUpLeftCorner(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
}

LONG CWaferTable::TeachCleanPinPosition(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
}

LONG CWaferTable::ConfirmCleanPinPosition(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn=TRUE;

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
}

LONG CWaferTable::MoveTableToCleanPinPoint(IPC_CServiceMessage& svMsg)
{
	LONG lPointNo = 0;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lPointNo);

	if( lPointNo>=(m_lACPMatrixCol * m_lACPMatrixRow) )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	GetEncoderValue();
	LONG lFromY = GetCurrY();
#ifdef NU_MOTION
	X1_Profile(LOW_PROF1);
	Y1_Profile(LOW_PROF1);
#endif

	LONG lRowIndex = lPointNo/m_lACPMatrixCol;
	LONG lColIndex = lPointNo%m_lACPMatrixCol;

	LONG lX_X = 0;
	LONG lX_Y = 0;
	if( m_lACPMatrixCol>1 )
	{
		lX_X = (m_lACPUpRightX-m_lACPUpLeftX)/(m_lACPMatrixCol-1);
		lX_Y = (m_lACPUpRightY-m_lACPUpLeftY)/(m_lACPMatrixCol-1);
	}
	LONG lY_Y = 0;
	LONG lY_X = 0;
	if( m_lACPMatrixRow>1 )
	{
		lY_Y = (m_lACPLowLeftY-m_lACPUpLeftY)/(m_lACPMatrixRow-1);
		lY_X = (m_lACPLowLeftX-m_lACPUpLeftX)/(m_lACPMatrixRow-1);
	}

	LONG lMoveToX = m_lACPUpLeftX + lColIndex * lX_X + lRowIndex * lY_X;
	LONG lMoveToY = m_lACPUpLeftY + lRowIndex * lY_Y + lColIndex * lX_Y;

	// the theta should rotate to its zero otherwise, may damage the probe pin in some case.
	if (XY1_MoveTo(lMoveToX, lFromY, SFM_WAIT) != gnOK)
	{
		bReturn = FALSE;
	}
	if (XY1_MoveTo(lMoveToX, lMoveToY, SFM_WAIT) != gnOK)
	{
		bReturn = FALSE;
	}
#ifdef NU_MOTION
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::MoveTableToCleanPinTheta(IPC_CServiceMessage& svMsg)
{
	T_MoveTo(m_lACPTableT, SFM_WAIT);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::GetProberMapPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szProberOutputMapPath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetPrbCsvDataOutPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szPrbCsvDataOutputPath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetPrbDataOutPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szPrbDataOutputPath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::GetPrbDCOutputMapPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn=FALSE;
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}

	if ( pAppMod != NULL )
	{
		pAppMod->GetPath(m_szPrbDCOutputPath);
		bReturn = TRUE;
	}

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::WriteSpecialItemToReg(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("Enable Check Wafer Contact Sensor"), m_bIsEnableCheckAlerm);


	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::ReadWriteSpecialItemToReg(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	UINT unItem = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Enable Check Wafer Contact Sensor"), 1);
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("Enable Check Wafer Contact Sensor"), unItem);
	m_bIsEnableCheckAlerm = (BOOL)unItem;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::AutoCaptureScreenInCleanPin(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CString szScreenName = "C:\\MapSorter\\UserData\\History\\Tom\\";
	if ( _access(szScreenName,0) != -1 )
	{
		CTime stTime = CTime::GetCurrentTime();
		CString szAbsTime;
		szAbsTime.Format( "%d", stTime.GetTime());
		PrintScreen(szScreenName + szAbsTime + "_" + m_szProberOutputMapFilename + ".txt");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::ManualCleanProbePinProcess(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

#ifdef NU_MOTION
	X1_Profile(CP_VLOW_PROFILE);
	Y1_Profile(CP_VLOW_PROFILE);
#endif
	GetEncoderValue();

	LONG lCurrentX = m_lEnc_X;
	LONG lCurrentY = m_lEnc_Y;

	//Check limit range
//	LONG lMinPosX = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_X, MS896A_CFG_CH_MIN_DISTANCE);								
//	LONG lMaxPosX = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_X, MS896A_CFG_CH_MAX_DISTANCE);
//	LONG lMinPosY = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_Y, MS896A_CFG_CH_MIN_DISTANCE);								
//	LONG lMaxPosY = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_Y, MS896A_CFG_CH_MAX_DISTANCE);
	//Move to Mechanical XY Limit - 1000
	if ( XY_MoveTo(m_lCPinPositionX, m_lCPinPositionY, SFM_WAIT) == gnOK )
	{
		HmiMessage("Please clean the probe pin", "Manual Clean Probe Pin", glHMI_MBX_OK);
		XY_MoveTo( lCurrentX ,lCurrentY);
	}
	else
	{
		HmiMessage("Table Move ERROR!", "Manual Clean Probe Pin", glHMI_MBX_OK);
	}


#ifdef NU_MOTION
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::AutoLoadTesterFiles(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szPackageTesterList = "C:\\MapSorter\\UserData\\PackageTesterList.csv";
	CString szDeviceFile = (*m_psmfSRam)["MS896A"]["PKG Filename"];
	m_szProberOutputMapFilename = "";
	m_szProberOperatorID		= "";

	if ( m_bEnableLoadTSFWhenLoadPKG == FALSE )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( szDeviceFile.Find(".pkg") != -1 )
	{
		szDeviceFile.Replace(".pkg","");
	}
	if ( szDeviceFile.Find(".PKG") != -1 )
	{
		szDeviceFile.Replace(".PKG","");
	}

	m_szTesterSpecName = szDeviceFile;

	if ( m_bEnableTesterList == TRUE ) 
	{
		if( access(szPackageTesterList,0) != -1 )
		{
			bReturn = FALSE;
			m_bEnableTesterList = FALSE;
			HmiMessage("PackageTesterList.csv is missing!");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		CStdioFile cfListFile;
		if (cfListFile.Open(szPackageTesterList, CFile::modeRead|CFile::shareExclusive|CFile::typeText) == FALSE)
		{
			bReturn = FALSE;
			m_bEnableTesterList = FALSE;
			HmiMessage("PackageTesterList.csv is having problem!");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		CString szRead;
		while ( cfListFile.ReadString(szRead) != NULL )
		{
			int nPos = szRead.Find(",");
			if ( nPos != -1 )
			{
				CString szPackageName = szRead.Left(nPos);
				CString szTesterSpec  = szRead.Right(nPos-1);
				if ( szPackageName == szDeviceFile )
				{
					m_szTesterSpecName = szTesterSpec;
					break;
				}
			}
		}
	}

	CString szTestSpecPath = m_szTesterTSFPath + "\\" + m_szTesterSpecName + ".tsf";

	if( access(szTestSpecPath,0) != -1 && m_szTesterTSFPath.IsEmpty() == FALSE )
	{
		CString szMsg, szCmd, szRpy;
		if (ConnectTesterTerminal())
		{
			szCmd = "STOP";
			SendRequestToTesterTerminal(szCmd, szRpy);	//	STOP tester before load TSF file
			CMSLogFileUtility::Instance()->ProberGenMapFileLog("Cmd:" + szCmd + " Rpy:" + szRpy);
			szCmd = "CLOSELOT";
			SendRequestToTesterTerminal(szCmd, szRpy);	//	CLOSELOT tester before load TSF file
			CMSLogFileUtility::Instance()->ProberGenMapFileLog("Cmd:" + szCmd + " Rpy:" + szRpy);
			szCmd = "OPEN," + m_szTesterTSFPath + "\\" + m_szTesterSpecName + ".tsf";
			SendRequestToTesterTerminal(szCmd, szRpy);	//	OPEN to auto load TSF file
			CMSLogFileUtility::Instance()->ProberGenMapFileLog("OPEN cmd:" + szCmd + " Rpy:" + szRpy);
			if( szRpy.Find("OPEN,OK") == -1 )
			{
				DisconnectTesterTerminal();
				szMsg = "Tester side check TestCondition alarm!";
				HmiMessage_Red_Back(szMsg, "Prober OPEN");
				SetErrorMessage(szMsg);
				bReturn = FALSE;
			}


			szCmd = "CMPTS," + m_szTesterTSFPath + "\\" + m_szTesterSpecName + ".tsf";
			SendRequestToTesterTerminal(szCmd, szRpy);	//	CMPTS to compare local and server TSF file same or not
			CMSLogFileUtility::Instance()->ProberGenMapFileLog("CMPTS cmd:" + szCmd + " Rpy:" + szRpy);

			DisconnectTesterTerminal();
			if( szRpy.Find("CMPTS,1") == -1 )
			{
				szMsg = "Tester side check TestCondition alarm!";
				HmiMessage_Red_Back(szMsg, "Prober CMPTS");
				SetErrorMessage(szMsg);
				bReturn = FALSE;
			}
		}
		else
		{
			szMsg = "Tester connection fail!";
			HmiMessage_Red_Back(szMsg, "Prober");
			SetErrorMessage(szMsg);
			bReturn = FALSE;
		}
	}
	else
	{
		HmiMessage( m_szTesterSpecName + ".tsf is missing!" );
	}

		
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

VOID CWaferTable::WaferMapTempOperation(LONG lAction)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("CP Need to restore wafer map"), 0);

	RemoveFilesInFolder(WAFERMAPTEMP_BACKUP_FOLDER);	// reset flag for warm start.

	CString szMsg;
	szMsg.Format("WaferMapTemp Folder operation %d", lAction);
	SetErrorMessage(szMsg);
}

VOID CWaferTable::UnlockThetaBrake(BOOL bUnlock)
{
	if (!m_fHardware)
	{
		return;
	}
}

LONG CWaferTable::SetThetaBrakeCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bUnlock = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bUnlock);

	return 1;
}

LONG CWaferTable::CheckOutputMapGeneration(IPC_CServiceMessage& svMsg)
{
	if( IsProbingEnd() && m_bReCheckAfterWaferEnd )
	{
		GenWaferEndOutputMapFile();	//	under bond page, CC_Operation
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferTable::GenWaferEndOutputMapFile()
{
	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest("BinTableStn", "ManualWaferEnd", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if( WriteProberMapFile() )	// wafer end, wait BH done before gen output map, check wafer end.
	{
	//	m_WaferMapWrapper.InitMap();	//	wafer end, generate output map file, clear it.
		CreateItemGradeDummyMap();	// auto generate probe output map file
	}
	WaferMapTempOperation(-1);	// wafer end, empty folder and reset flag.

	m_bIsWaferProbed = FALSE;
	m_bIsWaferEnded	 = FALSE;
	m_ulNewPickCounter = 0;

	return TRUE;
}

LONG CWaferTable::CpMoveToUnloadLoad(IPC_CServiceMessage& svMsg)
{
	BOOL bJustUnload = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bJustUnload);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( IsProbingEnd() && m_bReCheckAfterWaferEnd )
	{
		GenWaferEndOutputMapFile();	//	under bond page, CC_Operation
	}
	else if( pApp->GetCustomerName()=="ZhongGu" && !IsProbingEnd() && IsPrescanEnded() && GetNewPickCount()>0 )
	{
		CString szTitle, szContent;

		szContent.LoadString(HMB_WT_ARE_YOU_SURE_TO_GEN_MAP_FILE);
		szTitle.LoadString(HMB_WT_GEN_MAP);
	
		LONG lReturn = HmiMessage(szContent, szTitle,  glHMI_MBX_OKCANCEL);
		if (lReturn != glHMI_CANCEL)
		{
			if( WriteProberMapFile() )	// press unload table button, Zhong Gu only
			{
			//	m_WaferMapWrapper.InitMap();	//	move to unload, zhong gu generate output map file and clear it.
				CreateItemGradeDummyMap();	// auto generate probe output map file
			}
			WaferMapTempOperation(-1);	// wafer end, empty folder and reset flag.
		}

		m_bIsWaferProbed = FALSE;
		m_bIsWaferEnded	 = FALSE;
		m_ulNewPickCounter = 0;
	}	//	unload wafer, gen output map file

	SetJoystickOn(FALSE);
	if (T_Home() == gnAMS_OK)
		m_bIsPowerOn_T = TRUE;		//andrew

	SetJoystickOn(FALSE);

#ifdef NU_MOTION
	X1_Profile(CP_VLOW_PROFILE);
	Y1_Profile(CP_VLOW_PROFILE);
#endif

	if (bJustUnload == TRUE)
	{
		SelectWaferCamera();

		MoveToHome_ProberZ0();

		XY1_MoveTo(m_lWaferUnloadX, m_lWaferUnloadY, SFM_WAIT);
		T_MoveTo(0);
		MoveToHome_ProberZ();
	}
	else
	{
		CStringList szSelection;
		szSelection.AddTail("WaferTable Camera Position");
		szSelection.AddTail("WaferTable Probe Position");

		LONG lResult=0;
		lResult = HmiSelection("Please select item", "Motor Selection", szSelection, 0);
		CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001

		if (lResult == 0)	// WT1 Move To Camera
		{
			SelectWaferCamera();

			MoveToHome_ProberZ0();
			
			XY1_MoveTo(m_lHomeDieWftPosnX, m_lHomeDieWftPosnY, SFM_WAIT);
			MoveToHome_ProberZ();
		}
		else if (lResult == 1)	// WT1 Move To Prober Position
		{
			LONG lPosX, lPosY;
		
			MoveToHome_ProberZ0();

			lPosX = m_lHomeDieWftPosnX + m_lProbeOffsetX;
			lPosY = m_lHomeDieWftPosnY + m_lProbeOffsetY;
			
			XY1_MoveTo(lPosX, lPosY, SFM_WAIT);
			MoveToHome_ProberZ();
		}
	}

#ifdef NU_MOTION
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
#endif

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CP_MoveToWaferCamera(IPC_CServiceMessage& svMsg)
{
#ifdef NU_MOTION
	X1_Profile(CP_VLOW_PROFILE);
	Y1_Profile(CP_VLOW_PROFILE);
#endif

#ifdef NU_MOTION
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
#endif

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CpLoadOutputMapColumns(IPC_CServiceMessage &svMsg)
{
	// Get the Main Windows
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CWnd *pMainWnd;

	static char szFilters[]= "Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if ( pAppMod != NULL )
		pMainWnd = pAppMod->m_pMainWnd;
	else
		pMainWnd = NULL;

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.*", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pMainWnd, 0);

	dlgFile.m_ofn.lpstrInitialDir = m_szKeyFilesPath;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if ( pAppMod != NULL )
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = dlgFile.DoModal();	// Show the file dialog box

	if ( pAppMod != NULL )
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	if ( nReturn == IDOK )
	{
		CString szSrcFile = dlgFile.GetPathName();
		CString szTgtFile = PB_OUTPUT_MAP_COLUMNS;

		CStdioFile cfFile;
		//Check file format is valid is correct or not
		BOOL bFileOK = cfFile.Open(szSrcFile, 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText);

		if (bFileOK == TRUE)
		{
			CString szSelectionItem;
			cfFile.ReadString(szSelectionItem);
			if (atoi(szSelectionItem) == 0)
			{
				bFileOK = FALSE;
			}
			cfFile.Close();
		}

		CString szTitle, szContent;
		szTitle.LoadString(HMB_WT_MAP_HEADER_FILE);
		szContent.LoadString(HMB_WT_LOAD_HEADER_FAILED);
		if (bFileOK == TRUE)
		{
			m_szOutputHeaderFileName = dlgFile.GetFileName();
			//Copy user file to replace ASM file
			if( szSrcFile.CompareNoCase(szTgtFile)!=0 )
			{
				CopyFile(szSrcFile, szTgtFile, FALSE);
			}

			SaveData();

			szContent.LoadString(HMB_WT_LOAD_HEADER_OK);
		}
		HmiMessage(szContent, szTitle);
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

LONG CWaferTable::CpOutputMapHeaderPreTask(IPC_CServiceMessage &svMsg)
{
	CStdioFile cfWaferMapColFile;
	CString szNoOfWaferMapCols, szColHeader;
	
	m_bEnableHeaderBtn = TRUE;

	m_lCurDefineHeaderPageNo = 1;
	m_lPrevDefineHeaderPageNo = 1;
	for (INT i=1; i<WT_MAX_MAP_COL; i++)
	{
		m_bIfEnableWaferMapColCtrl[i] = FALSE;
		m_lWaferMapColCtrlIndex[i] = i ;
	}

	BOOL bIfFileExists = cfWaferMapColFile.Open(PB_OUTPUT_MAP_COLUMNS,
		CFile::modeRead|CFile::shareExclusive|CFile::typeText);

	if (bIfFileExists)
	{
		cfWaferMapColFile.SeekToBegin();

		if (cfWaferMapColFile.ReadString(szNoOfWaferMapCols))
		{
			m_lNoOfWaferMapCols = atoi((LPCTSTR(szNoOfWaferMapCols)));
		}

		m_lNoOfWaferMapCols = min(m_lNoOfWaferMapCols, 9999);	//Klocwork	//v4.02T5

		for (LONG i=1; i<=m_lNoOfWaferMapCols; i++ )
		{
			i = min(i, WT_MAX_MAP_COL_ALL-1);		//Klocwork	//v4.02T5

			cfWaferMapColFile.ReadString(m_szAllWaferMapCol[i]);
			if (i <= (WT_MAX_MAP_COL - 1))
			{
				m_szWaferMapCol[i] = m_szAllWaferMapCol[i];
				m_bIfEnableWaferMapColCtrl[i] = TRUE;
			}
			if( m_lNoOfWaferMapCols>=WT_MAX_MAP_COL )
				m_bEnableMultipleMapHeaderPage = TRUE;
			else
				m_bEnableMultipleMapHeaderPage = FALSE;
		}

		cfWaferMapColFile.Close();
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

LONG CWaferTable::CpSaveOutputMapColumns(IPC_CServiceMessage &svMsg)
{
	CStdioFile cfWaferMapColFile;
	CString szNoOfCols;

	CString szExeFile		= PB_OUTPUT_MAP_COLUMNS;
	if( m_szOutputHeaderFileName.IsEmpty() )
		m_szOutputHeaderFileName = "OutputMapHeaderFormat.txt";

	DeleteFile(szExeFile);

	UpdateWaferMapHeaderCtrl(m_lNoOfWaferMapCols);

	//Do NOT append
	cfWaferMapColFile.Open(szExeFile, 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);

	szNoOfCols.Format("%d", m_lNoOfWaferMapCols);

	cfWaferMapColFile.WriteString(szNoOfCols + "\n");
	for (LONG i=1; i<=m_lNoOfWaferMapCols; i++ )
	{
		cfWaferMapColFile.WriteString(m_szAllWaferMapCol[i] + "\n");
	}
	cfWaferMapColFile.Close();

	//Duplicate 1 set of file; file name is user input
	CString szTgtFile = m_szKeyFilesPath + "\\" + m_szOutputHeaderFileName;
	if( szExeFile.CompareNoCase(szTgtFile)!=0 )
	{
		CopyFile(szExeFile, szTgtFile, FALSE);
	}

	CString szTitle, szContent;
	szTitle.LoadString(HMB_WT_MAP_HEADER_FILE);
	szContent.LoadString(HMB_WT_SAVE_HEADER_OK);

	HmiMessage(szContent, szTitle, glHMI_MBX_OK);

	SetStatusMessage("New output map header file is created");
	SaveData();

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}

BOOL CWaferTable::IsBlockProbe()
{
	return FALSE;
}	// block probe

UCHAR CWaferTable::GetItemMapDisplayRatio()
{
	UCHAR ucItemMapDivide = 1;

	switch( m_ucPrbItemMapDisplayMode )
	{
	case 1:
		ucItemMapDivide = 3;
		break;
	case 2:
		ucItemMapDivide = 5;
		break;
	case 0:
	default:
		ucItemMapDivide = 1;
		break;
	}

	return ucItemMapDivide;
}

BOOL CWaferTable::CpUpdateMapGradeColor()	//	based the hmi map color file
{
	CString szSrcFile = gszAsmHmiColorFile;
	//2:Grade Color File	// map grade color
	if (_access(szSrcFile,0) == -1)
	{
		HmiMessage("Grade Color File Missing!" + szSrcFile);
		return FALSE;
	}

	CStdioFile cfColorFile;
	CString szContent;
	ULONG ulTempColor = 1;
	//Get current wafer color & update other grades
	
	BOOL bIfFileExists = cfColorFile.Open(szSrcFile, CFile::modeRead|CFile::shareExclusive|CFile::typeText);
	UCHAR ucGrade = 0;
	int nCount = 0;
	UCHAR ucGradeOffset = m_WaferMapWrapper.GetGradeOffset();
	int nNumOfGrade = m_WaferMapWrapper.GetNumberOfGrades();
	if (bIfFileExists)
	{
		//Set selected grade color
		cfColorFile.SeekToBegin();
		while (nCount <= 255)
		{
			cfColorFile.ReadString(szContent);
			ulTempColor = atoi((LPCTSTR)szContent);
			m_WaferMapWrapper.SetGradeColor(ucGrade + ucGradeOffset, (COLORREF)ulTempColor);
			ucGrade++;
			nCount ++;
		}
		cfColorFile.Close();
	}
	m_WaferMapWrapper.UpdateGradeColor();

	return TRUE;
}
