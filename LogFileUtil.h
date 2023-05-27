#ifndef LOG_FILE_UTIL_H
#define LOG_FILE_UTIL_H

#pragma once
#include "io.h"
#include "MS896A_ConStant.h"
#include "StringMapFile.h"

#define		LOG_FILE_SIZE_LIMIT			1024*1024
#define		LOG_FILES_NEED_TO_BACKUP	13	

#define		MS_OPERATION_LOG			"\\History\\ms_operation.log"
#define		MS_MD_OPERATION_LOG			"\\History\\md_operation.log"				//v4.52A10
#define		MS_OPERATION_LOG_SANAN		"\\Error\\ms_operation.log"					//v4.48A2
#define		MS_NUMOTION_LOG				"\\History\\ms_numotion.log"
#define		MS_SECSGEM_LOG				"\\History\\SECSGEM_operation.log"			//v4.11T1
#define		MS_WAFER_STATISTIC_PRETASK_LOG	"\\History\\StatisticPreTask.log"
#define		NVC_AUTO_BOND_OPERATION_LOG	"\\History\\NVC_AutoBondOperation.log"

#define		WT_STATUS_LOG				"\\History\\WTStatus.log"
#define		WT_LEVEL_LOG				"\\History\\WTLevel.log"

#define		WT_GENMAP_LOG				"\\History\\GenMap.log"

#define		WL_STATUS_LOG				"\\History\\WLStatus.log"
#define		WL_DEBUG_LOG				"\\History\\WLDebug.log"
#define		WL_BARCODE_TEST_LOG			"\\History\\BarcodeTest.log"

#define		BT_BACKUP_MAP_LOG			"\\History\\BackupMap.log"
#define		BT_STATUS_LOG				"\\History\\BT.log"
#define		BT_CLEAR_BIN_LOG			"\\History\\ClearBin.log"
#define		BT_GEN_TEMP_FILE_LOG		"\\History\\GenTemp.log"
#define		BT_REALIGN_BIN_FRAME_LOG	"\\History\\Realign.log"
#define		BT_TABLE_INDEX_LOG			"\\History\\BT_Index.log"			//v4.44A4	//SEmitek
#define		BT_DLA_STATUS_LOG			"\\History\\DLAStatus.log"
#define		BT_OPTIMIZE_GRADE_LOG		"\\History\\OptimizeGrade.log"
#define		BT_CLEAR_BIN_COUNTER_LOG	"\\History\\ClearBinCounter.log"
#define		BT_EXCHANGE_LOG				"\\History\\BT_Exchange.log"		//v3.34
#define		BT_BIN_REMAIN_COUNT_LOG		"\\History\\BT_RemainCount.log"
#define		BT_SERIAL_LOG				"\\History\\BT_Serial.log"
#define		BT_POSTBOND_LOG				"\\History\\BT_PostBond.log"
#define		BT_THETA_CORRECTION_LOG		"\\History\\BT_ThetaCorrection.log"

#define		BH_STATUS_LOG				"\\History\\BH_"
#define		BH_DIE_COUNTER_LOG			"\\History\\UpdateDieCounter.log"
#define		BHT_THERMAL					"\\History\\BHT_Thermal"
#define		BH_ILC_LOG					"\\History\\ILC.log"
#define		BH_ILC_EVENT_LOG			"\\History\\ILC_Event.log"

#define		MAP_GRAD_LOG				"\\History\\MapGrade.log"

#define		BL_STATUS_LOG				"\\History\\BLStatus.log"
#define		BL_CHANGE_GRADE_LOG			"\\History\\Bl_ChangeGrade.log"
#define		BL_EXCHANGE_TIME_LOG		"\\History\\Bl_ExTime.log"
#define		BL_LOAD_UNLOAD_LOG			"\\History\\Bl_ExLoadUnload.log"

#define		BL_HISTORY_NAME					"C:\\MapSorter\\UserData\\Statistic\\BL_History.txt"

#define		BL_BARCODE_LOG				"\\History\\BL_ExBarCode.log"

#define		WT_PITCH_ALARM_LOG			"\\History\\PitchAlarm.log"
#define		WT_ADV_RGN_OFFSET_GET_LOG	gszUSER_DIRECTORY + "\\History\\AdvGetOffset.log"	
#define		WT_ADV_RGN_OFFSET_SET_LOG	gszUSER_DIRECTORY + "\\History\\AdvSetOffset.log"	
#define		WT_ADV_RGN_SAMPLE_LOG		"\\History\\AdvRgnSample.log"	
#define		WT_GET_DIE_LOG				"\\History\\GetDie.log"
#define		WT_GET_IDX_LOG				"\\History\\GetIdx.log"	
#define		WT_PRESCAN_GET_IDX_LOG		"_GetIdx.log"
#define		WT_PRESCAN_GET_DIE_LOG		"_GetDie.log"
#define		WT_GET_PZN_LOG				"\\History\\GetPsn.log"
#define		WT_SCAN_GET_PZN_LOG			"_GetPsn.log"
#define		WT_GET_PR_LOG				"\\History\\DiePrResult.log"		//v3.41

#define		WT_MAP_DIE_TYPE_CHECK_LOG	"\\History\\DieTypeCheck.log"
#define		WT_GET_CT_LOG				"\\History\\CT\\CT"					//v4.06

#define		BPR_ARM1_LOG				"\\History\\BPR_Arm1.log"
#define		BPR_ARM2_LOG				"\\History\\BPR_Arm2.log"

#define		BH_Z1_LOG					"\\History\\BH_Arm1.log"			//v4.52A11
#define		BH_Z2_LOG					"\\History\\BH_Arm2.log"			//v4.52A11

#define		MS60_LOG					"\\History\\MS60.log"				//v4.47T1
#define		MissingDieThreshold_LOG1	"\\MissingDieLog\\BHZ1MissingDieThreshold.log"
#define		MissingDieThreshold_LOG2	"\\MissingDieLog\\BHZ2MissingDieThreshold.log"

#define		CP100_PIN_LOG				"_Pin.log"			//v1.01T600

#define		WPR_ALIGN_WAFER_LOG			"\\History\\WT_ALIGN_WAFER.log"
#define		WPR_LEARN_PITCH_LOG			"\\Diagnostics\\WPR\\LearnPitch.txt"
#define		WPR_DIE_OFFSET_LOG			"\\History\\WPRDieOffset.txt"

#define		WT_NVRAM_HISTORY_LOG		"\\NVRAM\\NV_History.txt"			//v4.22T1



class CMSLogFileUtility : public CObject
{
	
	public:

		static CMSLogFileUtility* Instance();
		VOID SetEnableMachineLog(BOOL bSet);
		BOOL GetEnableMachineLog();
		VOID SetEnableWtMachineLog(BOOL bSet);					//v3.67T4
		BOOL GetEnableWtMachineLog();
		VOID SetEnableBurnIn(BOOL bSet);						//v3.87
		BOOL GetEnableTableIndexLog();
		VOID SetEnableTableIndexLog(BOOL bSet);					//v4.59A12

		//------------------------------------------//
		//		Log File For MS896a Class			//
		//------------------------------------------//
		LONG LogCycleStartStatus(CString szMessage);			//v3.93

		BOOL MS_LogOperation(CONST CString szText);
		BOOL MS_BackupLogOperation();

		BOOL NVC_AutoBondLogOperation(CONST CString szText);
		
		BOOL MS_LogMDOperation(CONST CString szText);			//v4.52A10	//XM SanAn
		BOOL MS_BackupMDOperation();

		BOOL MS_LogCycleState(CONST CString szText);
		VOID SetMachineNo(CString szMcNo);
		BOOL AppendLogWithTime(CONST CString szLogFileName, CONST CString szText);
		BOOL CheckAndBackupSingleLogFile(CONST CString szLogFileName, ULONGLONG nFileSizeLimit = LOG_FILE_SIZE_LIMIT);

		BOOL MS_LogSECSGEM(CONST CString szText);				//v4.11T1
		BOOL MS_BackupSECSGEMLog();

		BOOL MS_LogNuMotionOperation(CONST CString szText);		//v3.64
		BOOL MS_BackupNuMotionOperation();						//v3.64

		BOOL MS_LogNVRAMHistory(CONST CString szText);			//v4.22T1
		BOOL MS_BackupNVRAMHistory();

		BOOL WT_LogStatus(CONST CString szText);
		BOOL WT_BackupLogStatus();

		BOOL WT_WaferTableLevelLog(CONST CString szText);
		BOOL WT_BackupWaferTableLevelLog();

		BOOL ProberGenMapFileLog(CONST CString szText);
		BOOL WT_BackupGenFileLog();

		//v4.42T11
		BOOL MS_LogCtmOperation(CONST CString szText);
		BOOL MS_BackupLogCtmOperation();
		VOID SetCustomerName(CString szName);

		//------------------------------------------//
		//		Log File For WaferLoader Class		//
		//------------------------------------------//
		BOOL WL_LogStatus(CONST CString szText);
		BOOL WL_BackupLogStatus();

		BOOL WL_DebugLog(CONST CString szText, BOOL bReset = FALSE);
		BOOL WL_BackupDebugLog();

		BOOL WL_BarcodeTestLog(CONST CString szText);

		BOOL MS_WaferStatisticsPreTaskLog(CONST CString szText);
		//v4.52A11
		BOOL BH_IsZ1LogOpen();
		BOOL BH_Z1LogOpen();
		BOOL BH_Z1Log(CONST CString szText, CONST BOOL bUpdate);
		BOOL BH_Z1LogClose();
		//v4.52A11
		BOOL BH_IsZ2LogOpen();
		BOOL BH_Z2LogOpen();
		BOOL BH_Z2Log(CONST CString szText, CONST BOOL bUpdate);
		BOOL BH_Z2LogClose();

		BOOL BPR_IsArm1LogOpen();
		BOOL BPR_Arm1LogOpen();
		BOOL BPR_Arm1Log(CONST CString szText);
		BOOL BPR_Arm1LogClose();

		BOOL BPR_IsArm2LogOpen();
		BOOL BPR_Arm2LogOpen();
		BOOL BPR_Arm2Log(CONST CString szText);
		BOOL BPR_Arm2LogClose();

		BOOL MAP_LogGradeMapping(CONST CString szText);
		BOOL MAP_BackupGradeLog();

		//------------------------------------------//
		//		Log File For BinLoader Class		//
		//------------------------------------------//
		BOOL BL_LogStatus(CONST CString szText);
		BOOL BL_BackupLogStatus();

		BOOL BL_BarcodeLog(CONST CString szText);
		BOOL BL_BackupBarcodeLog();
		
		BOOL BL_ChangeGradeLog(CONST LONG lExArmBufferBlock, CONST LONG lNextBlk);
		BOOL BL_BackupChangeGradeLog();

		BOOL BL_LoadUnloadTimeLog(CONST CString szText);
		BOOL BL_BackupLoadUnloadTimeLog();

		BOOL BL_LoadUnloadLog(CONST INT nMode, CONST LONG lMgzn, CONST LONG lSlot, CONST LONG lBlkNo);
		BOOL BL_BackupLoadUnloadLog();

		//CString BL_GetBinFrameStatusSummaryFilename();
		VOID BL_SetEnableBinFrameStatusFile(BOOL bSet);
		VOID BL_SetBinFrameStatusFilename(CString szFilename);
		VOID BL_SetBinFrameStatusTitle(CString szTitle);
		BOOL BL_BinFrameStatusSummaryFile(CString szStatus);
		BOOL BL_BinFrameStatusSummaryFile(CString szStatus, CString szMgzn, LONG lSlot, 
													 CString szBarcode, CString szGrade, LONG lCount,
													 BOOL bUseBC=TRUE);		//v4.46T9
		BOOL BL_RemoveBinFrameStatusSummaryFile();

		//------------------------------------------//
		//		Log File For BinTable Class			//
		//------------------------------------------//
		BOOL BT_LogStatus(CONST CString szText);
		BOOL BT_BackupLogStatus();

		BOOL BT_BackUpMapLog(CONST CString szText);
		BOOL BT_BackupTheMapLog();

		BOOL BT_ClearBinLog(CONST CString szText);
		BOOL BT_BackupClearBinLog();

		BOOL BT_GenTempLog(CONST CString szText, BOOL bWithTime = FALSE);
		BOOL BT_BackupGenTempLog();

		BOOL BT_ReAlignBinFrameLog(CONST CString szText, CString szMode);
		BOOL BT_BackupReAlignBinFrameLog();

		//v4.44A4	//Semitek
		BOOL BT_TableIndexLog(CONST CString szText);
		BOOL BT_BackupTableIndexLog();

		BOOL BT_DLALogStatus(CONST CString szText);
		BOOL BT_BackupDLALogStatus();

		BOOL BT_OptimizeGradeLog(CONST CString szText, BOOL bWithTime = FALSE);
		BOOL BT_BackupOptimizeGradeLog();

		BOOL BT_ClearBinCounterLog(CONST CString szText);
		BOOL BT_BackupClearBinCounterLog();

		BOOL BT_ExchangeFrameLog(CONST CString szText);
		BOOL BT_BackupExchangeFrameLog();

		BOOL BT_BinRemainingCountLog(CONST CString szText);
		BOOL BT_BackupBinRemaingCountLog();

		BOOL BT_BinSerialLog(CONST CString szText);
		BOOL BT_BackupBinSerialLog();

		BOOL BT_PostBondLog(CONST CString szText);
		BOOL BT_BackupPostBondLog();

		BOOL BT_ThetaCorrectionLog(CONST CString szText);
		BOOL BT_BackupThetaCorrectionLog();

		// bin table debug
		BOOL BT_BinTableMapIndexLog(CONST CString szText);
		BOOL BT_BinTableMapIndexLogClose();
		BOOL BT_BinTableMapIndexLogDelete();
		BOOL BT_BinTableMapIndexLogOpen();
		BOOL BT_IsBTMapIndexLogOpen();

		// pitch check alarm log
		BOOL WT_PitchAlarmLog(CONST CString szText);
		BOOL WT_BackupPitchAlarmLog();

		BOOL WT_GetAdvRgnOffsetLogOpen();
		BOOL WT_GetAdvRgnOffsetLog(CONST CString szText);
		BOOL WT_GetAdvRgnOffsetLogClose();

		BOOL WT_SetAdvRgnOffsetLogOpen();
		BOOL WT_SetAdvRgnOffsetLog(CONST CString szText);
		BOOL WT_SetAdvRgnOffsetLogClose();

		BOOL WT_GetAdvRgnSampleLogOpen();
		BOOL WT_GetAdvRgnSampleLog(CONST CString szText);
		BOOL WT_GetAdvRgnSampleLogClose();

		BOOL WT_OpenMylarShiftLog(CTime stTime);
		BOOL WT_WriteMylarShiftLog(CONST CString szText);
		BOOL WT_CloseMylarShiftLog();

		BOOL WT_WriteDebLog(CONST CString szText);
		BOOL WT_WriteKeyDieLog(CONST CString szText);

		//------------------------------------------//
		//		Log File For WaferTable Class		//
		//------------------------------------------//
		BOOL WT_GetDieLogOpen();
		BOOL WT_GetDieLog(CONST CString szText);
		BOOL WT_GetDieLogClose();
		BOOL WT_IsGetDieLogOpen();

		BOOL WT_GetIdxLogOpen();
		BOOL WT_GetIdxLog(CONST CString szText);
		BOOL WT_GetIdxLogClose();
		BOOL WT_IsGetIdxLogOpen();

		BOOL WT_OpenPznLog();
		BOOL WT_AddPznLog(CONST CString szText);
		BOOL WT_ClosePznLog();
		BOOL WT_IsPznLogOpen();

		BOOL WriteAutoCycleLog(CONST CString szText);
		BOOL CloseAutoCycleLog(BOOL bReOpen);

		CCriticalSection m_CsAcTimeLog;
		BOOL AC_NoProductionTimeLog(CTime stStartTime, CONST CString szTypeText);

		//v4.06
		BOOL WT_GetCTLogOpen(CString szMapFileName = "");		//v4.46T13
		BOOL WT_GetCTLog(CONST CString szText);
		BOOL WT_GetCTLog_Update(CONST LONG lMapRow, CONST LONG lMapCol, CONST LONG lMotionTime, CONST BOOL bLookForward, CONST BOOL bTheta, CONST BOOL bPick);
		BOOL WT_GetCTLog_CT_NoUpdate(CONST DOUBLE dCycleTime);
		BOOL WT_GetCTLogClose();
		BOOL WT_IsGetCTLogOpen();
		BOOL BackupCTLogAtWaferEnd(CONST CString szWaferID);	//Xu Zhi Jin

		BOOL WT_PrLogOpen();
		BOOL WT_GetPrLog(CONST CString szText);
		BOOL WT_GetPrLogClose();
		BOOL WT_IsPrLogOpen();

		BOOL WT_MapDieTypeCheckLog(CONST CString szText);

		VOID ChangeFileNewName(const CString szLogFile, const CString szNewLogFile, const BOOL bEnableLog);
		VOID CreatePath(const CString szFileName, CString &szPath, CString &szName);
		BOOL CheckLogStatus(const CString szFileName);

		BOOL MissingDieThreshold_IsLogOpen(const BOOL bBHZ2);
		BOOL MissingDieThreshold_LogOpen(const BOOL bBHZ2);
		BOOL MissingDieThreshold_Log(const BOOL bBHZ2, CONST CString szText);
		BOOL MissingDieThreshold_LogClose(const BOOL bBHZ2);

		//v4.47T1
		BOOL MS60_IsLogOpen();
		BOOL MS60_LogOpen();
		BOOL MS60_Log(CONST CString szText);
		BOOL MS60_LogClose();

		//v1.01T600	//CP100
		BOOL CP100_IsPinLogOpen();
		BOOL CP100_PinLogOpen();
		BOOL CP100_PinLog(CONST CString szText);
		BOOL CP100_PinLogClose();

		//------------------------------------------//
		//		Log File For BondHead Class			//
		//------------------------------------------//
		BOOL BH_LogStatus(CONST CString szText);
		BOOL BH_BackupLogStatus();		//v4.50A6
		BOOL Uplook_LogDieDataStatus(CONST CString szText);

		BOOL BH_DieCounterLog(CONST CString szText);
		BOOL BH_BackupDieCounterLog();

		BOOL BH_LogBondHeadTThermal(CONST CString szText);

		BOOL BH_ILCLog(CONST CString szText);
		
		BOOL BH_BackupILCEventLog();		//v4.52A13
		BOOL BH_ILCEventLog(CONST CString szText);

		//------------------------------------------//
		//		Log File For WPR Class				//
		//------------------------------------------//
		BOOL WPR_AlignWaferLog(CONST CString szText);
		BOOL WPR_BackupAlignWaferLog();

		BOOL WPR_LearnPitchLog(CONST CString szText);
		BOOL WPR_BackupLearnPitchLog();

		BOOL WPR_WaferDieOffsetLog(CONST CString szText, BOOL bLogAlways = FALSE);
		BOOL WPR_BackupWaferDieOffsetLog();

		//------------------------------------------//
		//	Check the file size of all Log Files	//
		//	Backup it if the file size excess		//
		//	certain size							//
		//------------------------------------------//
		VOID CheckAndBackupAllLogFiles();


	private:

		//======================================================
		// Constructor / Destructor
		CMSLogFileUtility();
		virtual ~CMSLogFileUtility();

		BOOL LogWithoutDateAndTime(CONST CString szLogFileName, CONST CString szText, CONST CString szOpenFileMode);
		BOOL LogWithDateAndTime(CONST CString szLogFileName, CONST CString szText, CONST CString szOpenFileMode);

		//------------------------------------------//
		//	Log Functions for logging data without	//
		//	closing the logging file				//
		//------------------------------------------//

		BOOL LogWithTimeOnly(FILE* &fp, CONST CString szText);
		BOOL LogWithDateAndTime(FILE* &fp, CONST CString szText);
		BOOL LogWithoutDateAndTime(FILE* &fp, CONST CString szText);	//v4.06
		BOOL IsLogFileOpen(FILE* &fp);
		BOOL OpenLogFile(FILE* &fp,CONST CString szLogFileName, CONST CString szOpenFileMode);
		BOOL CloseLogFile(FILE* &fp);

		//------------------------------------------//
		//	Check the file size of all Log Files	//
		//	Backup it if the file size excess		//
		//	certain size							//
		//------------------------------------------//

		BOOL CheckAndBackupSingleLogFileWithAppend(CONST CString szLogFileName, ULONGLONG nFileSizeLimit = LOG_FILE_SIZE_LIMIT);
	
		BOOL RemoveFilesInFolder(CString szFilePath);	//v4.06


		static CMSLogFileUtility* m_pInstance;
		BOOL m_bEnableMachineLog;
		BOOL m_bEnableWtMachineLog;				//v3.67T4
		BOOL m_bBurnin;							//v3.87
		BOOL m_bEnableTableIndexLog;			//v4.59A12

		CCriticalSection m_CsOperation;			//v4.11
		CCriticalSection m_CsMdOperation;		//v4.52A10
		CCriticalSection m_CsCtmOperation;		//v4.22T11
		CCriticalSection m_CsSECSGEM;			//v4.11
		CCriticalSection m_CsNVRam;				//v4.22T2
		CCriticalSection m_CsBTReAlignBinFrame;	//v4.20
		CCriticalSection m_CsBTTableIndex;		//v4.44A4	//Semitek
		CCriticalSection m_CsWLStatus;			//v4.24T1
		CCriticalSection m_CsBLStatus;			//v4.19
		CCriticalSection m_CsBLLoadUnload;		//v4.20
		CCriticalSection m_CsBLLoadUnloadTime;	//v4.20
		CCriticalSection m_CsLogCycleState;
		CCriticalSection m_CsBPR_Arm1Log;
		CCriticalSection m_CsBinTableMapIndexLog;
		CCriticalSection m_CsMS60Log;			//v4.47T1
		CCriticalSection m_CsCTLog;				//v4.47T4
		CCriticalSection m_CsProberGenMapFileLog;	//v4.47T7
		CCriticalSection m_CsWaferTableLevelLog;	//v4.47T7
		CCriticalSection m_CsBH_Z1Log;			//v4.52A11
		CCriticalSection m_CsBH_Z2Log;			//v4.52A11
		CCriticalSection m_csResource;
		LONG			 m_nLogBackup;

		CString			 m_szMachineNo;
		CString			 m_szCustomerName;		

		static CCriticalSection	m_csDebugLog;

		BOOL	m_bEnableBinFrameStatusSummary;
		CString m_szBinFrameStatusSummaryFilename;
		CString m_szBinFrameStatusSummaryTitle;

		//------------------------------------------//
		//	Member Variable for	WaferTable			//
		//	Log File								//
		//------------------------------------------//
		CCriticalSection m_CsMapLog;
		FILE* m_fMapLog;
		CCriticalSection m_CsIdxLog;
		FILE* m_fIdxLog;
		CCriticalSection m_CsPznLog;
		FILE* m_fPznLog;

		FILE* m_fPrLog;		//v3.41

		CCriticalSection m_CsAdvRgnOffsetLog;
		FILE* m_fAdvRgnOffsetLog;
		CCriticalSection m_CsAdvRgnOffsetGetLog;
		FILE* m_fAdvRgnOffsetGetLog;
		CCriticalSection m_CsAdvRgnSampleLog;
		FILE* m_fAdvRgnSampleLog;
		//CCriticalSection m_CsAutoCycleLog;
		CMutex m_CsAutoCycleLog;
		FILE* m_fAutoCycleLog;
		CString m_szACLogOpenTime;	// last opened normal log full name with time stamp.
		CCriticalSection m_CsMylarShiftLog;
		FILE* m_fMylarShiftLog;

		//v4.06
		FILE* m_fCTLog;

		FILE* m_fBHZ1Log;
		FILE* m_fBHZ2Log;
		FILE* m_fBPRArm1Log;
		FILE* m_fBPRArm2Log;
		
		FILE* m_fMS60Log;		//v4.47T1
		FILE* m_fMissingDieThresholdLog1;
		FILE* m_fMissingDieThresholdLog2;

		CCriticalSection m_CsCP100PinLog;		//v1.01T600
		FILE* m_fCP100PinLog;	//v1.01T600

		// BPR Debug
		FILE* m_fBinTableMapIndexLog;

		CString m_szCT;
		LONG m_lCTLog_LastMapRow;
		LONG m_lCTLog_LastMapCol;
		LONG m_lCTLog_TotalLookForward;	
		LONG m_lCTLog_TotalLookForward1;		//Lookforward with 1-die-pitch apart
		LONG m_lCTLog_TotalLookForward2;		//Lookforward with 2-die-pitch apart
		LONG m_lCTLog_TotalLookForward3;		//Lookforward with 3-die-pitch apart
		LONG m_lCTLog_TotalLookForward4;		//Lookforward with 4-die-pitch
		LONG m_lCTLog_TotalLookForward5;		//Lookforward with 5-die-pitch
		LONG m_lCTLog_TotalLookForward6;		//Lookforward with 6-die-pitch
		LONG m_lCTLog_TotalLookForward7;		//Lookforward with >6-die-pitch
		LONG m_lCTLog_TotalTheta;
		LONG m_lCTLog_TotalPick;
		LONG m_lCTLog_TotalLongMotion;
		LONG m_lCTLog_TotalDices;
		DOUBLE m_dCTLog_TotalDiePitchDist;		//v4.13T2

};

#endif