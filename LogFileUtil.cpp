#include "stdafx.h"
#include "LogFileUtil.h"
#include "PrescanUtility.h"
#include "MS896a.h"
#include <direct.h>

CMSLogFileUtility* CMSLogFileUtility::m_pInstance = NULL;
CCriticalSection CMSLogFileUtility::m_csDebugLog;

#define	MS_AC_LOG_PATH_NAME		PRESCAN_RESULT_FULL_PATH + "AI Log\\CurrentAuto"

CMSLogFileUtility::CMSLogFileUtility()
{
	m_bEnableMachineLog = FALSE;
	m_bEnableWtMachineLog = FALSE;
	m_bBurnin = FALSE;
	m_bEnableTableIndexLog	= FALSE;

	m_fMapLog = NULL;
	m_fIdxLog = NULL;
	m_fPznLog	= NULL;
	m_fAutoCycleLog		= NULL;
	CTime ctTime = CTime::GetCurrentTime();
	m_szACLogOpenTime = ctTime.Format("_%m%d_%H%M%S");
	m_fAdvRgnOffsetLog	= NULL;
	m_fAdvRgnOffsetGetLog	= NULL;
	m_fAdvRgnSampleLog	= NULL;
	m_fPrLog  = NULL;
	m_fMylarShiftLog	= NULL;
	m_fMS60Log	= NULL;
	m_fMissingDieThresholdLog1 = NULL;
	m_fMissingDieThresholdLog2 = NULL;
	m_fCP100PinLog = NULL;

	m_fBHZ1Log		= NULL;
	m_fBHZ2Log		= NULL;
	m_fBPRArm1Log = NULL;
	m_fBPRArm2Log = NULL;

	m_fBinTableMapIndexLog = NULL;

	//v4.06
	m_fCTLog	= NULL;
	m_szCT		= "";
	m_lCTLog_LastMapRow			= 0;
	m_lCTLog_LastMapCol			= 0;
	m_lCTLog_TotalLookForward	= 0;
	m_lCTLog_TotalLookForward1	= 0;
	m_lCTLog_TotalLookForward2	= 0;
	m_lCTLog_TotalLookForward3	= 0;
	m_lCTLog_TotalLookForward4	= 0;
	m_lCTLog_TotalLookForward5	= 0;
	m_lCTLog_TotalLookForward6	= 0;
	m_lCTLog_TotalLookForward7	= 0;
	m_lCTLog_TotalTheta			= 0;
	m_lCTLog_TotalPick			= 0;
	m_lCTLog_TotalLongMotion	= 0;
	m_lCTLog_TotalDices			= 0;

	m_szCustomerName	= "default";
	m_nLogBackup = 10;

	//v4.06
	if (CreateDirectory(gszUSER_DIRECTORY + "\\History\\CT", NULL) == 0)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			RemoveFilesInFolder(gszUSER_DIRECTORY + "\\History\\CT");
		}
	}
}

CMSLogFileUtility::~CMSLogFileUtility()
{
	if (m_fMapLog != NULL)
		WT_GetDieLogClose();
	if (m_fIdxLog != NULL)
		WT_GetIdxLogClose();
	if( m_fPznLog!=NULL )
		WT_ClosePznLog();
	if (m_fMS60Log != NULL)
		MS60_LogClose();
	if (m_fMissingDieThresholdLog1 != NULL)
	{
		MissingDieThreshold_LogClose(FALSE);
	}
	if (m_fMissingDieThresholdLog2 != NULL)
	{
		MissingDieThreshold_LogClose(TRUE);
	}

	CloseAutoCycleLog(FALSE);
	if( m_fAdvRgnOffsetLog!=NULL )
		WT_GetAdvRgnOffsetLogClose();
	if( m_fAdvRgnOffsetGetLog!=NULL )
		WT_SetAdvRgnOffsetLogClose();
	if( m_fAdvRgnSampleLog!=NULL )
		WT_GetAdvRgnSampleLogClose();
	if (m_fPrLog != NULL)
		WT_GetPrLogClose();

	if (m_fBPRArm1Log != NULL)
		BPR_Arm1LogClose();
	if (m_fBPRArm2Log != NULL)
		BPR_Arm2LogClose();

	if (m_fBinTableMapIndexLog != NULL)
		BT_BinTableMapIndexLogClose();

	if( m_fMylarShiftLog!=NULL )
		WT_CloseMylarShiftLog();
}


//--------------------------------------//
//		Log File For MS896a				//
//--------------------------------------//

LONG CMSLogFileUtility::LogCycleStartStatus(CString szMessage)
{
	if ( (GetEnableMachineLog() == FALSE) && (m_bBurnin == FALSE) )	//v3.87		//Need to enable StopCycle log for BURNIN
	{
		return 1;
	}

	static LONG lCount = 0;

	FILE *fp;
	CTime theTime = CTime::GetCurrentTime();


	CSingleLock slLock(&m_csDebugLog);
	slLock.Lock();

	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\Startcycle.log";

	errno_t nErr = fopen_s(&fp, szLogFileName, "a+");
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%d-%2d-%2d (%2d:%2d:%2d) - %s\n",  
				theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(),
				theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(),
				(LPCTSTR) szMessage);

		fclose(fp);
	}

	slLock.Unlock();
	return 1;
}

BOOL CMSLogFileUtility::MS_LogOperation(CONST CString szText)
{
	//if (GetEnableMachineLog() == FALSE )
	//	return TRUE;
	CSingleLock slLock(&m_CsOperation);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + MS_OPERATION_LOG;

	//v4.48A2
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "SanAn")
	{
		CString szMachineNo = pApp->GetMachineNo();
		szLogFileName = gszUSER_DIRECTORY +	"\\Error\\" + szMachineNo + "_ms_operation.log";	//MS_OPERATION_LOG_SANAN;
	}

	BOOL bStatus = TRUE;
	
	if (szText.GetLength() == 0)	//v4.49A10
	{
		//bStatus = LogWithoutDateAndTime(szLogFileName,	szText + "\n" ,"a+");
	}
	else
		bStatus = LogWithDateAndTime(szLogFileName,		szText + "\n" ,"a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::MS_LogMDOperation(CONST CString szText)
{
	BOOL bStatus = TRUE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "SanAn")
	{
		CSingleLock slLock(&m_CsMdOperation);
		slLock.Lock();

		CString szNewText = szText + "\n";
		szNewText = "," + szNewText;

		CString szLogFileName = gszUSER_DIRECTORY + MS_MD_OPERATION_LOG;
		if (szText.GetLength() == 0)
			bStatus = LogWithoutDateAndTime(szLogFileName, szNewText, "a+");
		else
			bStatus = LogWithDateAndTime(szLogFileName,	szNewText, "a+");
		
		slLock.Unlock();
	}
	return bStatus;
}

BOOL CMSLogFileUtility::MS_LogCtmOperation(CONST CString szText)
{

	CSingleLock slLock(&m_CsCtmOperation);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\" + m_szCustomerName + ".log";
	BOOL bStatus = LogWithDateAndTime(szLogFileName, szText + "\n" ,"a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::MS_BackupLogCtmOperation()
{	
	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\" + m_szCustomerName + ".log";
	return CheckAndBackupSingleLogFile(szLogFileName);
}

VOID CMSLogFileUtility::SetCustomerName(CString szName)
{
	m_szCustomerName = szName;
}

VOID CMSLogFileUtility::SetMachineNo(CString szMcNo)	//	427TX	1
{
	m_szMachineNo = szMcNo;
}

BOOL CMSLogFileUtility::AppendLogWithTime(CONST CString szLogFileName, CONST CString szText)
{
	return LogWithDateAndTime(szLogFileName, szText +"\n", "a+");
}


BOOL CMSLogFileUtility::MS_LogCycleState(CONST CString szText)	//	427TX	1
{
	CSingleLock slLock(&m_CsLogCycleState);
	slLock.Lock();

	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format("%Y-%m-%d (%H:%M:%S)");
	CString szName = theTime.Format("%Y%m");

	CString szLogFileName = gszUSER_DIRECTORY + "\\CycleState" + "\\" + m_szMachineNo +"_status_" + szName + ".txt";
	FILE* fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFileName, "a+");
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%s - %s\n", (LPCTSTR) szTime, (LPCTSTR) szText); 
		fclose(fp);
	}
	slLock.Unlock();

	return TRUE;
}


BOOL CMSLogFileUtility::MS_BackupLogOperation()
{	
	CSingleLock slLock(&m_CsOperation);		//v4.47T4
	slLock.Lock();

	//v4.48A2
	CString szLogFileName = gszUSER_DIRECTORY + MS_OPERATION_LOG;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	BOOL bStatus = CheckAndBackupSingleLogFile(szLogFileName, 1024*1024*2);

	if (pApp->GetCustomerName() == "SanAn")
	{
		CString szMachineNo = pApp->GetMachineNo();
		szLogFileName = gszUSER_DIRECTORY +	"\\Error\\" + szMachineNo + "_ms_operation.log";	//MS_OPERATION_LOG_SANAN;
		//szLogFileName = gszUSER_DIRECTORY + MS_OPERATION_LOG_SANAN;
		bStatus = CheckAndBackupSingleLogFile(szLogFileName, 1024*1024*2);
	}

	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::MS_BackupMDOperation()
{	
	CSingleLock slLock(&m_CsMdOperation);	
	slLock.Lock();

	BOOL bStatus = TRUE;
	CString szLogFileName = gszUSER_DIRECTORY + MS_MD_OPERATION_LOG;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "SanAn")
	{
		bStatus = CheckAndBackupSingleLogFile(szLogFileName);
	}

	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::MS_LogSECSGEM(CONST CString szText)			//v4.11T1
{
	CSingleLock slLock(&m_CsSECSGEM);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + MS_SECSGEM_LOG;
	BOOL bStatus = LogWithDateAndTime(szLogFileName, szText + "\n" ,"a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::MS_BackupSECSGEMLog()
{
	CSingleLock slLock(&m_CsSECSGEM);		//v4.47T4
	slLock.Lock();
	BOOL bStatus = CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + MS_SECSGEM_LOG);
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::MS_LogNuMotionOperation(CONST CString szText)
{
	return TRUE;
}

BOOL CMSLogFileUtility::MS_BackupNuMotionOperation()
{	
	return TRUE;	//CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + MS_NUMOTION_LOG);
}

BOOL CMSLogFileUtility::MS_LogNVRAMHistory(CONST CString szText)
{
	//if (GetEnableMachineLog() == FALSE )
	//	return TRUE;
	CSingleLock slLock(&m_CsNVRam);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + WT_NVRAM_HISTORY_LOG;
	BOOL bStatus = LogWithDateAndTime(szLogFileName, szText + "\n" ,"a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::MS_BackupNVRAMHistory()
{	
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WT_NVRAM_HISTORY_LOG);
}


//--------------------------------------//
//		Log File For Bondhead			//
//--------------------------------------//

BOOL CMSLogFileUtility::BH_Z1LogOpen()
{
	CString szLogFileName = gszUSER_DIRECTORY + BH_Z1_LOG;

	if( BH_IsZ1LogOpen() )
		return TRUE;
	
	CSingleLock slLock(&m_CsBH_Z1Log);	
	slLock.Lock();
	DeleteFile(szLogFileName);	
	BOOL bStatus = OpenLogFile(m_fBHZ1Log, szLogFileName, "a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BH_Z1Log(CONST CString szText, CONST BOOL bUpdate)
{
	if (!BH_IsZ1LogOpen())
		return FALSE;
	
	BOOL bStatus = TRUE;

	CSingleLock slLock(&m_CsBH_Z1Log);
	slLock.Lock();
	bStatus =  LogWithoutDateAndTime(m_fBHZ1Log, szText + "\n" );	

	if (bUpdate)
	{
		//m_fBHZ1Log.Flush();
	}

	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BH_Z1LogClose()
{
	CSingleLock slLock(&m_CsBH_Z1Log);
	slLock.Lock();
	BOOL bStatus = CloseLogFile(m_fBHZ1Log);
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BH_IsZ1LogOpen()
{
	return IsLogFileOpen(m_fBHZ1Log);
}

BOOL CMSLogFileUtility::BH_Z2LogOpen()
{
	CString szLogFileName = gszUSER_DIRECTORY + BH_Z2_LOG;

	if( BH_IsZ2LogOpen() )
		return TRUE;
	
	CSingleLock slLock(&m_CsBH_Z2Log);	
	slLock.Lock();
	DeleteFile(szLogFileName);	
	BOOL bStatus = OpenLogFile(m_fBHZ2Log, szLogFileName, "a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BH_Z2Log(CONST CString szText, CONST BOOL bUpdate)
{
	if (!BH_IsZ2LogOpen())
		return FALSE;
	
	BOOL bStatus = TRUE;

	CSingleLock slLock(&m_CsBH_Z2Log);
	slLock.Lock();
	bStatus =  LogWithoutDateAndTime(m_fBHZ2Log, szText + "\n" );	

	//if (bUpdate)
	//{
	//	m_fBHZ2Log.Flush();
	//}

	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BH_Z2LogClose()
{
	CSingleLock slLock(&m_CsBH_Z2Log);
	slLock.Lock();
	BOOL bStatus = CloseLogFile(m_fBHZ2Log);
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BH_IsZ2LogOpen()
{
	return IsLogFileOpen(m_fBHZ2Log);
}

//--------------------------------------//
//		Log File For BondPr				//
//--------------------------------------//

BOOL CMSLogFileUtility::BPR_Arm1LogOpen()
{
	CString szLogFileName = gszUSER_DIRECTORY + BPR_ARM1_LOG;

	if( BPR_IsArm1LogOpen() )
		return TRUE;
	
	CSingleLock slLock(&m_CsBPR_Arm1Log);		//v4.47T4
	slLock.Lock();
	DeleteFile(szLogFileName);					//v4.42T16
	BOOL bStatus = OpenLogFile(m_fBPRArm1Log, szLogFileName, "a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BPR_Arm1Log(CONST CString szText)
{
	if (!BPR_IsArm1LogOpen())
		return FALSE;
	
	BOOL bStatus = TRUE;

	CSingleLock slLock(&m_CsBPR_Arm1Log);
	slLock.Lock();
	bStatus =  LogWithoutDateAndTime(m_fBPRArm1Log, szText + "\n" );	
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BPR_Arm1LogClose()
{
	CSingleLock slLock(&m_CsBPR_Arm1Log);
	slLock.Lock();
	BOOL bStatus = CloseLogFile(m_fBPRArm1Log);
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BPR_IsArm1LogOpen()
{
	return IsLogFileOpen(m_fBPRArm1Log);
}

BOOL CMSLogFileUtility::BPR_Arm2LogOpen()
{
	CString szLogFileName = gszUSER_DIRECTORY + BPR_ARM2_LOG;
	if( BPR_IsArm2LogOpen() )
		return TRUE;

	DeleteFile(szLogFileName);	//v4.42T16
	BOOL bStatus = OpenLogFile(m_fBPRArm2Log, szLogFileName, "a+");
	return bStatus;
}

BOOL CMSLogFileUtility::BPR_Arm2Log(CONST CString szText)
{
	if (!BPR_IsArm2LogOpen())
		return FALSE;

	BOOL bStatus = TRUE;
	bStatus = LogWithoutDateAndTime(m_fBPRArm2Log, szText + "\n" );		//v4.48A11
	return bStatus;
}

BOOL CMSLogFileUtility::BPR_Arm2LogClose()
{
	return CloseLogFile(m_fBPRArm2Log);
}

BOOL CMSLogFileUtility::BPR_IsArm2LogOpen()
{
	return IsLogFileOpen(m_fBPRArm2Log);
}


//--------------------------------------//
//		Log File For WaferLoader		//
//--------------------------------------//
BOOL CMSLogFileUtility::WL_LogStatus(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CSingleLock slLock(&m_CsWLStatus);		//v4.24T1
	slLock.Lock();

	CString szLogFileName = gszUSER_DIRECTORY + WL_STATUS_LOG;
	BOOL bStatus = LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");

	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::WL_BackupLogStatus()
{	
	CSingleLock slLock(&m_CsWLStatus);		//v4.47T4
	slLock.Lock();
	BOOL bStatus = CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WL_STATUS_LOG, 2*LOG_FILE_SIZE_LIMIT);
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::WL_DebugLog(CONST CString szText, BOOL bReset)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + WL_DEBUG_LOG;
	
	if ( bReset == TRUE )
	{
		DeleteFile(szLogFileName);
	}

	return LogWithDateAndTime(szLogFileName,szText + "\n","a+");
}

BOOL CMSLogFileUtility::WL_BackupDebugLog()
{	
	//return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WL_DEBUG_LOG);
	return DeleteFile(gszUSER_DIRECTORY + WL_DEBUG_LOG);	//v4.31T8
}

BOOL CMSLogFileUtility::WL_BarcodeTestLog(CONST CString szText)
{
	CString szPath;	
	CString szYear, szMonth;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());

	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}

	szPath = gszUSER_DIRECTORY + WL_BARCODE_TEST_LOG;	// + szMonth + szYear + ".log";	//v4.21T6
	
	return LogWithDateAndTime(szPath, szText + "\n", "a+");

}

BOOL CMSLogFileUtility::MS_WaferStatisticsPreTaskLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szPath;	
	CString szYear, szMonth;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());

	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}

	szPath = gszUSER_DIRECTORY + MS_WAFER_STATISTIC_PRETASK_LOG;	// + szMonth + szYear + ".log";	//v4.21T6
	
	return LogWithDateAndTime(szPath, szText + "\n", "a+");

}

BOOL CMSLogFileUtility::MAP_LogGradeMapping(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + MAP_GRAD_LOG;

	return LogWithoutDateAndTime(szLogFileName, szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::MAP_BackupGradeLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + MAP_GRAD_LOG, 2*LOG_FILE_SIZE_LIMIT);
}

//--------------------------------------//
//		Log File For BinLoader			//
//--------------------------------------//

// used in CBinLoader::LogStatusInfo
BOOL CMSLogFileUtility::BL_LogStatus(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CSingleLock slLock(&m_CsBLStatus);		//v4.19
	slLock.Lock();

	CString szLogFileName = gszUSER_DIRECTORY + BL_STATUS_LOG;
	BOOL bStatus = LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");
	
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BL_BackupLogStatus()
{
	CSingleLock slLock(&m_CsBLStatus);		//v4.47T4
	slLock.Lock();
	BOOL bStatus = CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BL_STATUS_LOG, LOG_FILE_SIZE_LIMIT*4);
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BL_BarcodeLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BL_BARCODE_LOG;

	return LogWithDateAndTime(szLogFileName,szText + "\n", "a+");
	
}

BOOL CMSLogFileUtility::BL_BackupBarcodeLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BL_BARCODE_LOG);
}

BOOL CMSLogFileUtility::BL_ChangeGradeLog(CONST LONG lExArmBufferBlock, CONST LONG lNextBlk)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szText;
	CString szLogFileName = gszUSER_DIRECTORY + BL_CHANGE_GRADE_LOG;

	szText.Format("Buffer = %d;   Next Blk = %d", lExArmBufferBlock, lNextBlk);

	return LogWithDateAndTime(szLogFileName,szText + "\n", "a+");
}

BOOL CMSLogFileUtility::BL_BackupChangeGradeLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BL_CHANGE_GRADE_LOG);
}

BOOL CMSLogFileUtility::BL_LoadUnloadTimeLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BL_EXCHANGE_TIME_LOG;

	//v4.20
	CSingleLock slLock(&m_CsBLLoadUnloadTime);
	slLock.Lock();
	BOOL bStatus = LogWithDateAndTime(szLogFileName,szText + "\n", "a+");
	slLock.Unlock();

	return bStatus;
}

BOOL CMSLogFileUtility::BL_BackupLoadUnloadTimeLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BL_EXCHANGE_TIME_LOG);
}

BOOL CMSLogFileUtility::BL_LoadUnloadLog(CONST INT nMode, CONST LONG lMgzn, CONST LONG lSlot, CONST LONG lBlkNo)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BL_LOAD_UNLOAD_LOG;
	CString szText = "";

	switch (nMode)
	{
	case 0:
		szText.Format("EXCHANGE :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
		break;
	case 1:
		szText.Format("LOAD     :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
		break;
	case 2:
		szText.Format("UNLOAD   :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
		break;
	case 3:
		szText.Format("PRELOAD-E:  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
		break;
	case 4:
		szText.Format("PRELOAD  :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
		break;
	case 5:
		szText.Format("CLEAR    :  Mgzn=%d  SLOT=%d  ID=%d", lMgzn, lSlot+1, lBlkNo);
		break;
	default:
		break;
	}

	CSingleLock slLock(&m_CsBLLoadUnload);
	slLock.Lock();
	BOOL bStatus = LogWithDateAndTime(szLogFileName,szText + "\n", "a+");
	slLock.Unlock();

	return bStatus;
}

BOOL CMSLogFileUtility::BL_BackupLoadUnloadLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BL_LOAD_UNLOAD_LOG);
}

VOID CMSLogFileUtility::BL_SetEnableBinFrameStatusFile(BOOL bSet)
{
	m_bEnableBinFrameStatusSummary = bSet;
}

VOID CMSLogFileUtility::BL_SetBinFrameStatusFilename(CString szFilename)
{
	m_szBinFrameStatusSummaryFilename = szFilename;
}
	
VOID CMSLogFileUtility::BL_SetBinFrameStatusTitle(CString szTitle)
{
	m_szBinFrameStatusSummaryTitle = szTitle;
}

BOOL CMSLogFileUtility::BL_BinFrameStatusSummaryFile(CString szStatus)
{
	CString szLogFileName = m_szBinFrameStatusSummaryFilename;
	CString szText = "", szTime = "";

	if (m_bEnableBinFrameStatusSummary == FALSE)
	{
		return TRUE;
	}

	if ((_access(szLogFileName, 0)) == -1)
	{
		CString szFileTitle = m_szBinFrameStatusSummaryTitle;
		LogWithoutDateAndTime(szLogFileName, szFileTitle + "\n", "a+");
	}

	CTime theTime = CTime::GetCurrentTime();
	szTime = theTime.Format("%Y/%m/%d %H:%M:%S");

	szText = szTime + "," + szStatus;
	
	return LogWithoutDateAndTime(szLogFileName, szText + "\n", "a+");
}

 BOOL CMSLogFileUtility::BL_BinFrameStatusSummaryFile(CString szStatus, CString szMgzn, LONG lSlot, 
													 CString szBarcode , CString szGrade, LONG lCount,
													 BOOL bUseBC)
{
	CString szLogFileName = m_szBinFrameStatusSummaryFilename;
	CString szText = "", szSlot = "", szcount = "";

	if (m_bEnableBinFrameStatusSummary == FALSE)
	{
		return TRUE;
	}

	if ((_access(szLogFileName, 0)) == -1)
	{
		CString szFileTitle = m_szBinFrameStatusSummaryTitle;
		LogWithoutDateAndTime(szLogFileName, szFileTitle + "\n", "a+");
	}

	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format("%Y/%m/%d %H:%M:%S");

	szSlot.Format("%d", lSlot);
	szcount.Format("%d", lCount);

	CString szBC = szBarcode;
	if (!bUseBC)
		szBC = "";		//v4.46T9
	szText = szTime + "," + szStatus + "," + szMgzn + "," + szSlot + "," + szBC + "," + szGrade + "," + szcount;
	
	return LogWithoutDateAndTime(szLogFileName, szText + "\n", "a+");
}

BOOL CMSLogFileUtility::BL_RemoveBinFrameStatusSummaryFile()
{
	return DeleteFile(m_szBinFrameStatusSummaryFilename);
}

//--------------------------------------//
//		Log File For BinTable			//
//--------------------------------------//

// Using in the fucntion CBinTable::LOG_BT
BOOL CMSLogFileUtility::BT_LogStatus(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_STATUS_LOG;
	
	return LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");	
}

BOOL CMSLogFileUtility::BT_BackupLogStatus()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_STATUS_LOG);
}


// Using in the fucntion CBinTable::OpBackupMap()
BOOL CMSLogFileUtility::BT_BackUpMapLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;
	CString szLogFileName = gszUSER_DIRECTORY + BT_BACKUP_MAP_LOG;
	return LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::BT_BackupTheMapLog()
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_BACKUP_MAP_LOG);
}

// Will be used in UpdateSerialNoGivenBinBlkId
BOOL CMSLogFileUtility::BT_ClearBinLog(CONST CString szText)
{
	//if (GetEnableMachineLog() == FALSE )
	//	return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_CLEAR_BIN_LOG;
	return LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::BT_BackupClearBinLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_CLEAR_BIN_LOG);
}

// Will be used in CBinBlk::GenTempFile
BOOL CMSLogFileUtility::BT_GenTempLog(CONST CString szText, BOOL bWithTime)
{	
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_GEN_TEMP_FILE_LOG;

	if (bWithTime == TRUE)
	{	
		// No EndLine
		return LogWithDateAndTime(szLogFileName,szText,"a+");
	}

	// No EndLine
	return LogWithoutDateAndTime(szLogFileName,szText,"a+");
}

BOOL CMSLogFileUtility::BT_BackupGenTempLog()
{
	CString szLogFileName = gszUSER_DIRECTORY + BT_GEN_TEMP_FILE_LOG;
	ULONGLONG nFileSizeLimit = 2048*2048;

	CFile fLogFile;
	if  (fLogFile.Open(szLogFileName,CFile::modeRead) == FALSE)
	{
		return TRUE;
	}
	ULONGLONG nFileSize = fLogFile.GetLength();
	fLogFile.Close();

	// Case of no need to backup the log file
	if (nFileSize < nFileSizeLimit )
	{
		return TRUE;
	}

	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format("%Y%m%d%H%M%S");
	CString szBpLogFileName = gszUSER_DIRECTORY + "\\History\\GenTemp_" + szTime + ".log";

	try
	{
		CopyFile(szLogFileName,szBpLogFileName,FALSE);
		DeleteFile(szLogFileName);
	}
	catch (CFileException e)
	{
		return FALSE;
	}
	
	return TRUE;
}

// used in LOG_ReAlignBinFrame + BackupReAlignBinLog
BOOL CMSLogFileUtility::BT_ReAlignBinFrameLog(CONST CString szText, CONST CString szMode)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_REALIGN_BIN_FRAME_LOG;

	CSingleLock slLock(&m_CsBTReAlignBinFrame);
	slLock.Lock();
	BOOL bStatus = LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::BT_BackupReAlignBinFrameLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_REALIGN_BIN_FRAME_LOG);
}

// used to log BT indexing enc position for chekcing BH2/PostBOnd/EjtXY offset position on bin side
BOOL CMSLogFileUtility::BT_TableIndexLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	BOOL bStatus = TRUE;
	CString szLogFileName = gszUSER_DIRECTORY + BT_TABLE_INDEX_LOG;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bLog = FALSE;

	//v4.52A14
	if (pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY))
	{
		//bLog = TRUE;
	}
	//v4.59A12
	if (m_bEnableTableIndexLog)
	{
		bLog = TRUE;
	}

	if (bLog)
	{
		CSingleLock slLock(&m_CsBTTableIndex);
		slLock.Lock();
		bStatus = LogWithDateAndTime(szLogFileName, szText + "\n" ,"a+");
		slLock.Unlock();
	}
	return bStatus;
}

BOOL CMSLogFileUtility::BT_BackupTableIndexLog()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bLog = FALSE;

	//v4.52A14
	if (pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY))
	{
		//bLog = TRUE;
	}
	//v4.59A12
	if (m_bEnableTableIndexLog)
	{
		bLog = TRUE;
	}

	BOOL bStatus = TRUE;
	if (bLog)
	{
		CSingleLock slLock(&m_CsBTTableIndex);		//v4.47T7
		slLock.Lock();
		LONG lSize = 1024 * 1024 * 2;
		bStatus = CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_TABLE_INDEX_LOG, lSize);
		slLock.Unlock();
	}
	
	return bStatus;
}

BOOL CMSLogFileUtility::BT_DLALogStatus(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_DLA_STATUS_LOG;

	return LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::BT_BackupDLALogStatus()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_DLA_STATUS_LOG);
}

BOOL CMSLogFileUtility::BT_OptimizeGradeLog(CString szText,BOOL bWithTime)
{
	CString szLogFileName = gszUSER_DIRECTORY + BT_OPTIMIZE_GRADE_LOG;

	if (bWithTime == TRUE)
	{	
		// No EndLine
		return LogWithDateAndTime(szLogFileName,szText,"a+");
	}

	// No EndLine
	return LogWithoutDateAndTime(szLogFileName,szText,"a+");
}

BOOL CMSLogFileUtility::BT_BackupOptimizeGradeLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_OPTIMIZE_GRADE_LOG);
}

BOOL CMSLogFileUtility::BT_ClearBinCounterLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_CLEAR_BIN_COUNTER_LOG;

	return LogWithDateAndTime(szLogFileName, szText + "\n", "a+");
}

BOOL CMSLogFileUtility::BT_BackupClearBinCounterLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_CLEAR_BIN_COUNTER_LOG,2048*2048);
}

BOOL CMSLogFileUtility::BT_ExchangeFrameLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_EXCHANGE_LOG;

	return LogWithDateAndTime(szLogFileName, szText + "\n", "a+");
}

BOOL CMSLogFileUtility::BT_BackupExchangeFrameLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_EXCHANGE_LOG, 2048*2048);
}

BOOL CMSLogFileUtility::BT_BinRemainingCountLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_BIN_REMAIN_COUNT_LOG;

	return LogWithDateAndTime(szLogFileName, szText + "\n", "a+");

}

BOOL CMSLogFileUtility::BT_BackupBinRemaingCountLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_BIN_REMAIN_COUNT_LOG, 2048*2048);
}

BOOL CMSLogFileUtility::BT_BinSerialLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_SERIAL_LOG;

	return LogWithDateAndTime(szLogFileName, szText + "\n", "a+");
}

BOOL CMSLogFileUtility::BT_BackupBinSerialLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_SERIAL_LOG, 2048*2048);
}

BOOL CMSLogFileUtility::BT_PostBondLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_POSTBOND_LOG;

	return LogWithDateAndTime(szLogFileName, szText + "\n", "a+");
}

BOOL CMSLogFileUtility::BT_BackupPostBondLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_POSTBOND_LOG, 2048*2048);
}

BOOL CMSLogFileUtility::BT_ThetaCorrectionLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BT_THETA_CORRECTION_LOG;

	return LogWithDateAndTime(szLogFileName, "," + szText + "\n", "a+");
}

BOOL CMSLogFileUtility::BT_BackupThetaCorrectionLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BT_THETA_CORRECTION_LOG, 2048*2048);
}

BOOL CMSLogFileUtility::BT_BinTableMapIndexLogOpen()
{
	CString szLogFileName = gszUSER_DIRECTORY + "\\BinIndexMap.csv";

	if ( BT_IsBTMapIndexLogOpen() )
	{
		return TRUE;
	}

	return OpenLogFile(m_fBinTableMapIndexLog, szLogFileName, "a+");
}

BOOL CMSLogFileUtility::BT_BinTableMapIndexLog(CONST CString szText)
{
	if (!BT_IsBTMapIndexLogOpen())
		return FALSE;
	
	BOOL bStatus = TRUE;
/*
	CSingleLock slLock(&m_CsBinTableMapIndexLog);
	slLock.Lock();
	bStatus =  LogWithoutDateAndTime(m_fBinTableMapIndexLog, szText + "\n");	
	slLock.Unlock();
*/
	return bStatus;
}

BOOL CMSLogFileUtility::BT_BinTableMapIndexLogClose()
{
	return CloseLogFile(m_fBinTableMapIndexLog);
}

BOOL CMSLogFileUtility::BT_IsBTMapIndexLogOpen()
{
	return IsLogFileOpen(m_fBinTableMapIndexLog);
}

BOOL CMSLogFileUtility::BT_BinTableMapIndexLogDelete()
{
/*
	CString szLogFileName = gszUSER_DIRECTORY + "\\BinTableMap.csv";

	if ( BT_IsBTMapIndexLogOpen()  )
	{
		return FALSE;
	}
	else
	{
		if ( !OpenLogFile(m_fBinTableMapIndexLog, szLogFileName, "a+") )
		{
			return FALSE;
		}
		CloseLogFile(m_fBinTableMapIndexLog);
		BT_BinTableMapIndexLogClose();
		return DeleteFile(szLogFileName);	//v4.42T16
	}
*/	
	return TRUE;
}

//--------------------------------------//
//		Log File For BondHead 			//
//--------------------------------------//

// used in CBondHead::LogBondHeadStatus
BOOL CMSLogFileUtility::BH_LogStatus(CONST CString szText)
{
	CString szPath;	
	CString szYear, szMonth;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());

	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}

	szPath = gszUSER_DIRECTORY + BH_STATUS_LOG + szMonth + szYear + ".log";
	return LogWithDateAndTime(szPath, szText + "\n","a+");
}

BOOL CMSLogFileUtility::BH_BackupLogStatus()	//v4.50A6
{
	CString szPath;	
	CString szYear, szMonth;	
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);
	
	szMonth.Format("%d", CurTime.GetMonth());
	if (CurTime.GetMonth() < 10)
	{
		szMonth = "0" + szMonth;
	}

	szPath = gszUSER_DIRECTORY + BH_STATUS_LOG + szMonth + szYear + ".log";
	return CheckAndBackupSingleLogFile(szPath);
}

// used in CBondHead::UpdateDieCounter(VOID)
BOOL CMSLogFileUtility::BH_DieCounterLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + BH_DIE_COUNTER_LOG;
	
	return LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::BH_BackupDieCounterLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + BH_DIE_COUNTER_LOG);
}

BOOL CMSLogFileUtility::BH_LogBondHeadTThermal(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szPath;	
	CString szYear, szMonth;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());

	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}

	szPath = gszUSER_DIRECTORY + BHT_THERMAL ".log";
	
	return LogWithDateAndTime(szPath,szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::BH_ILCLog(CONST CString szText)
{
	CString szPath;	
	CString szYear, szMonth;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());

	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}

	szPath = gszUSER_DIRECTORY + BH_ILC_LOG;
	
	return LogWithDateAndTime(szPath,szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::BH_BackupILCEventLog()
{
	CString szPath;	
	CString szYear, szMonth;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());

	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}

	szPath = gszUSER_DIRECTORY + BH_ILC_EVENT_LOG;
	return CheckAndBackupSingleLogFile(szPath);
}

BOOL CMSLogFileUtility::BH_ILCEventLog(CONST CString szText)
{
	if (!GetEnableWtMachineLog())
		return FALSE;
	CString szPath;	
	CString szYear, szMonth;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());

	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}

	szPath = gszUSER_DIRECTORY + BH_ILC_EVENT_LOG;
	
	return LogWithDateAndTime(szPath,szText + "\n" ,"a+");
}

//--------------------------------------//
//		Log File For WaferTable			//
//--------------------------------------//

// used in CWaferTable::PreStartOperation()
// using fMapLog

BOOL CMSLogFileUtility::WT_GetDieLogOpen()
{
	CString szLogFileName = gszUSER_DIRECTORY + WT_GET_DIE_LOG;
	CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pUtl->GetPrescanRegionMode() || pApp->GetCustomerName()=="Semitek" )
	{
		CString szLogPath;
		pUtl->GetPrescanLogPath(szLogPath);
		szLogFileName = szLogPath + WT_PRESCAN_GET_DIE_LOG;
	}

	if( WT_IsGetDieLogOpen() )
		return TRUE;

	CSingleLock slLock(&m_CsMapLog);
	slLock.Lock();
	BOOL bReturn = OpenLogFile(m_fMapLog, szLogFileName, "a+");
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_GetDieLog(CONST CString szText)
{
	if (!GetEnableWtMachineLog())	//v4.06
		return FALSE;

	if (!WT_IsGetDieLogOpen())
		return FALSE;

	CSingleLock slLock(&m_CsMapLog);
	slLock.Lock();
	BOOL bReturn = FALSE;
	if( szText=="Start" || szText=="End" )
		bReturn = LogWithDateAndTime(m_fMapLog, szText + "\n" );	
	else
		bReturn = LogWithTimeOnly(m_fMapLog, szText + "\n" );
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_GetDieLogClose()
{
	CSingleLock slLock(&m_CsMapLog);
	slLock.Lock();
	BOOL bReturn = CloseLogFile(m_fMapLog);
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_IsGetDieLogOpen()
{
	return IsLogFileOpen(m_fMapLog);
}


BOOL CMSLogFileUtility::WT_PitchAlarmLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + WT_PITCH_ALARM_LOG;

	return LogWithDateAndTime(szLogFileName, szText + "\n", "a+");
}

BOOL CMSLogFileUtility::WT_BackupPitchAlarmLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WT_PITCH_ALARM_LOG, 2048*2048);
}

BOOL CMSLogFileUtility::WT_GetAdvRgnOffsetLogOpen()
{
	if (!GetEnableWtMachineLog())
		return FALSE;

	CString szLogFileName = WT_ADV_RGN_OFFSET_GET_LOG;

	CSingleLock slLock(&m_CsAdvRgnOffsetLog);
	slLock.Lock();
	BOOL bReturn = OpenLogFile(m_fAdvRgnOffsetLog, szLogFileName,"a+");
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_GetAdvRgnOffsetLog(CONST CString szText)
{
	if (!GetEnableWtMachineLog())
		return FALSE;

	//	get,map,region,offset,counter (-1, use last nearby die)
	CSingleLock slLock(&m_CsAdvRgnOffsetLog);
	slLock.Lock();
	BOOL bReturn = LogWithDateAndTime(m_fAdvRgnOffsetLog,szText + "\n" );
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_GetAdvRgnOffsetLogClose()
{
	if (!GetEnableWtMachineLog())
		return FALSE;

	CSingleLock slLock(&m_CsAdvRgnOffsetLog);
	slLock.Lock();
	BOOL bReturn = CloseLogFile(m_fAdvRgnOffsetLog);
	slLock.Unlock();
	return bReturn;
}


BOOL CMSLogFileUtility::WT_SetAdvRgnOffsetLogOpen()
{
	if (!GetEnableWtMachineLog())
		return FALSE;

	CSingleLock slLock(&m_CsAdvRgnOffsetGetLog);
	slLock.Lock();
	BOOL bReturn = OpenLogFile(m_fAdvRgnOffsetGetLog, WT_ADV_RGN_OFFSET_SET_LOG, "a+");
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_SetAdvRgnOffsetLog(CONST CString szText)
{
	if (!GetEnableWtMachineLog())
		return FALSE;
	//	set,map,region,offset,
	CSingleLock slLock(&m_CsAdvRgnOffsetGetLog);
	slLock.Lock();
	BOOL bReturn = LogWithDateAndTime(m_fAdvRgnOffsetGetLog, szText + "\n");
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_SetAdvRgnOffsetLogClose()
{
	if (!GetEnableWtMachineLog())
		return FALSE;

	CSingleLock slLock(&m_CsAdvRgnOffsetGetLog);
	slLock.Lock();
	BOOL bReturn = CloseLogFile(m_fAdvRgnOffsetGetLog);
	slLock.Unlock();
	return bReturn;
}



BOOL CMSLogFileUtility::WT_GetAdvRgnSampleLogOpen()
{
	CSingleLock slLock(&m_CsAdvRgnSampleLog);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + WT_ADV_RGN_SAMPLE_LOG;

	BOOL bReturn = OpenLogFile(m_fAdvRgnSampleLog, szLogFileName, "a+");
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_GetAdvRgnSampleLog(CONST CString szText)
{
	CSingleLock slLock(&m_CsAdvRgnSampleLog);
	slLock.Lock();
	BOOL bReturn = LogWithDateAndTime(m_fAdvRgnSampleLog, szText + "\n" );
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_GetAdvRgnSampleLogClose()
{
	CSingleLock slLock(&m_CsAdvRgnSampleLog);
	slLock.Lock();
	BOOL bReturn = CloseLogFile(m_fAdvRgnSampleLog);
	slLock.Unlock();
	return bReturn;
}



BOOL CMSLogFileUtility::WT_OpenMylarShiftLog(CTime stTime)
{
	CString szLogPath;
	CString szTime = stTime.Format("_MS_%Y%m%d%H%M%S.csv");
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	pUtl->GetPrescanLogPath(szLogPath);
	CString szLogFileName = szLogPath + szTime;

	if( IsLogFileOpen(m_fMylarShiftLog) )
	{
		return TRUE;
	}

	CSingleLock slLock(&m_CsMylarShiftLog);
	slLock.Lock();
	BOOL bReturn = OpenLogFile(m_fMylarShiftLog, szLogFileName, "a+");
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_WriteMylarShiftLog(CONST CString szText)
{
	if( !IsLogFileOpen(m_fMylarShiftLog) )
		return TRUE;

	CSingleLock slLock(&m_CsMylarShiftLog);
	slLock.Lock();
	BOOL bReturn = LogWithoutDateAndTime(m_fMylarShiftLog, szText + "\n" );
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_CloseMylarShiftLog()
{
	if( !IsLogFileOpen(m_fMylarShiftLog) )
		return TRUE;

	CSingleLock slLock(&m_CsMylarShiftLog);
	slLock.Lock();
	BOOL bReturn = CloseLogFile(m_fMylarShiftLog);
	slLock.Unlock();
	return bReturn;
}	// Log mylar shift under prescan mode.

BOOL CMSLogFileUtility::WT_WriteDebLog(CONST CString szText)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDebug() )
	{
		CString szLogPath;
		pUtl->GetPrescanLogPath(szLogPath);
		CString szLogFileName = szLogPath + "_deb.csv";
		FILE *fp = NULL;
		errno_t nErr = fopen_s(&fp, szLogFileName, "a+");
		if ((nErr == 0) && (fp != NULL))
		{
			CTime theTime = CTime::GetCurrentTime();
			CString szTime = theTime.Format("%H:%M:%S");
			fprintf(fp, "%s,%s\n", (LPCTSTR) szTime, (LPCTSTR) szText); 
			fclose(fp);
		}
	}

	return TRUE;
}

BOOL CMSLogFileUtility::WT_WriteKeyDieLog(CONST CString szText)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( GetEnableMachineLog() )
	{
		CString szLogPath;
		pUtl->GetPrescanLogPath(szLogPath);
		CString szLogFileName = szLogPath + "_KeyDie.csv";

		FILE* fp = NULL;
		errno_t nErr = fopen_s(&fp, szLogFileName, "a+");
		if ((nErr == 0) && (fp != NULL))
		{
			CTime theTime = CTime::GetCurrentTime();
			CString szTime = theTime.Format("%H:%M:%S");
			if( szText=="" )
				fprintf(fp, "\n");
			else
				fprintf(fp, "%s,%s\n", (LPCTSTR) szTime, (LPCTSTR) szText); 
			fclose(fp);
		}
	}

	return TRUE;
}

BOOL CMSLogFileUtility::WriteAutoCycleLog(CONST CString szText)
{
	CSingleLock slLock(&m_CsAutoCycleLog);
	slLock.Lock();
	BOOL bReturn = FALSE;
	if (m_fAutoCycleLog!=NULL)
	{
		bReturn = LogWithTimeOnly(m_fAutoCycleLog, szText + "\n" );
	}
	else
	{
		FILE *fp = NULL;
		CString szLogFileName = MS_AC_LOG_PATH_NAME + ".log";
		errno_t nErr = fopen_s(&fp, szLogFileName, "a");
		if ((nErr == 0) && (fp != NULL))
		{
			LogWithTimeOnly(fp, szText + "\n" );
			fclose(fp);
			fp = NULL;
		}
	}
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::CloseAutoCycleLog(BOOL bReOpen)
{
	CSingleLock slLock(&m_CsAutoCycleLog);
	slLock.Lock();

	BOOL bReturn = FALSE;
	CloseLogFile(m_fAutoCycleLog);

	if( GetEnableMachineLog() && bReOpen && m_fAutoCycleLog==NULL )
	{
		CString szLogFileName = MS_AC_LOG_PATH_NAME + ".log";

		CString szOldFileName = MS_AC_LOG_PATH_NAME + m_szACLogOpenTime + ".log";
		if (_access(szLogFileName, 0) != -1)
		{
			DeleteFile(szOldFileName);
			CStdioFile::Rename(szLogFileName, szOldFileName);
		}

		errno_t nErr = fopen_s(&m_fAutoCycleLog, szLogFileName, "w");
		CTime ctTime = CTime::GetCurrentTime();
		m_szACLogOpenTime = ctTime.Format("_%m%d_%H%M%S");
		bReturn = (m_fAutoCycleLog!=NULL);
	}

	slLock.Unlock();
	return bReturn;
}

// make sure the log file closed before this.
BOOL CMSLogFileUtility::AC_NoProductionTimeLog(CTime stStartTime, CONST CString szTypeText)
{
	CSingleLock slLock(&m_CsAcTimeLog);
	slLock.Lock();

	CString szDate = theApp.m_eqMachine.GetCreateEquipTimeDate();
	if (szDate.IsEmpty())
	{
		MS_LogOperation("Open EquipTime.msd Fails");
	}
	else
	{
		MS_LogOperation("Get Time From EquipTime.msd:" + szDate);
	}


	CTime stCurrTime = CTime::GetCurrentTime();
	CString szNameTime = stCurrTime.Format("_NP_%y%m%d");
	if( CMS896AApp::m_bESAoiSingleWT || CMS896AApp::m_bES100v2DualWftOption )
		szNameTime = stCurrTime.Format("_NP_%y%m");
	CString szStartTime = stStartTime.Format("%H:%M:%S");
	CString szEndTime = stCurrTime.Format("%H:%M:%S");
	CTimeSpan stSpan = stCurrTime - stStartTime;
	CString szUsedTime;
	szUsedTime.Format("%d", stSpan.GetTotalSeconds());

	INT nCurrHour = stCurrTime.GetHour();
	INT nCurrMin  = stCurrTime.GetMinute();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szTimePeriod = "";
	CString szExt = ".csv";
	LONG lTimeStage = -1;
	for(int i=0; i<pApp->GetNoOfTimeReportPeriod(); i++)
	{
		lTimeStage = 0;
		INT nStartHour = 0, nStartMinute = 0;
		LONG lTimeMinutes = pApp->GetReportPeriodStartTime(i, nStartHour, nStartMinute);
		if(  lTimeMinutes==0 )
			continue;
		if( (nCurrHour<nStartHour) || (nCurrHour==nStartHour && nCurrMin<=nStartMinute) )
		{
			lTimeStage = i+1;
			szTimePeriod.Format("%02d%02d", nStartHour, nStartMinute);
			break;
		}
	}
	if( lTimeStage>=1 )
	{
		szNameTime += szTimePeriod;
		CString szDummyName = gszUSER_DIRECTORY + "\\Performance\\" + m_szMachineNo + "_dummy" + szExt;
		if (lTimeStage==1 && _access(szDummyName, 0) != -1)
		{
			if (szDate != "")
			{
				szNameTime = "_NP_" + szDate;
			}
			CString szLogFileName = gszUSER_DIRECTORY + "\\Performance\\" + m_szMachineNo + szNameTime + ".csv"/*+ szExt*/;
			CopyFile(szDummyName, szLogFileName, FALSE);
			DeleteFile(szDummyName);
		}
	}
	if( lTimeStage==0 )
	{
		szNameTime = "_dummy";
	}

	if (szDate != "")
	{
		szNameTime = "_NP_" + szDate;
	}
	CString szLogFileName = gszUSER_DIRECTORY + "\\Performance\\" + m_szMachineNo + szNameTime  + ".csv"/*+ szExt*/;
	BOOL bNewFile = FALSE;
	if (_access(szLogFileName, 0) == -1)
		bNewFile = TRUE;

	if( bNewFile )
		LogWithoutDateAndTime(szLogFileName, "type,start time,end time,duration seconds\n", "a+");
	CString szText;
	szText.Format("%s,%s,%s,%s\n", szTypeText, szStartTime, szEndTime, szUsedTime);
	BOOL bStatus = LogWithoutDateAndTime(szLogFileName, szText, "a+");

	//MS_LogOperation("NoProduction Copy to server start");
	pApp->CopyPerformanceToServer(szLogFileName, m_szMachineNo + szNameTime);
	//MS_LogOperation("NoProduction Copy to server end");
	slLock.Unlock();

	return bStatus;
}



// used in CWaferTable::PreStartOperation()
// using fIdxLog
BOOL CMSLogFileUtility::WT_GetIdxLogOpen()
{
	if( WT_IsGetIdxLogOpen() )
		return TRUE;

	CSingleLock slLock(&m_CsIdxLog);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + WT_GET_IDX_LOG;
	CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pUtl->GetPrescanRegionMode() )
	{
		CString szLogPath;
		pUtl->GetPrescanLogPath(szLogPath);
		szLogFileName = szLogPath + WT_PRESCAN_GET_IDX_LOG;
	}

	BOOL bReturn = OpenLogFile(m_fIdxLog, szLogFileName,"a+");
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_GetIdxLog(CONST CString szText)
{
	if (!GetEnableWtMachineLog())
		return FALSE;

	if (!WT_IsGetIdxLogOpen())
		return FALSE;

	CSingleLock slLock(&m_CsIdxLog);
	slLock.Lock();
	BOOL bReturn = FALSE;
	if( szText=="Start" || szText=="End" )
		bReturn = LogWithDateAndTime(m_fIdxLog,szText + "\n" );
	else
		bReturn = LogWithTimeOnly(m_fIdxLog,szText + "\n" );
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_GetIdxLogClose()
{
	CSingleLock slLock(&m_CsIdxLog);
	slLock.Lock();
	BOOL bReturn = CloseLogFile(m_fIdxLog);
	slLock.Unlock();
	return bReturn;
}

BOOL CMSLogFileUtility::WT_IsGetIdxLogOpen()
{
	return IsLogFileOpen(m_fIdxLog);
}


BOOL CMSLogFileUtility::WT_OpenPznLog()
{
	if( WT_IsPznLogOpen() )
	{
		return TRUE;
	}

	CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
	CSingleLock slLock(&m_CsPznLog);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + WT_GET_PZN_LOG;
	if( pUtl->GetPrescanRegionMode() )
	{
		CString szLogPath;
		pUtl->GetPrescanLogPath(szLogPath);
		szLogFileName = szLogPath + WT_SCAN_GET_PZN_LOG;
	}

	BOOL bReturn = OpenLogFile(m_fPznLog, szLogFileName, "a+");
	slLock.Unlock();

	return bReturn;
}

BOOL CMSLogFileUtility::WT_AddPznLog(CONST CString szText)
{
	if (!GetEnableWtMachineLog())
		return FALSE;

	if (!WT_IsPznLogOpen())
		return FALSE;

	CSingleLock slLock(&m_CsPznLog);
	slLock.Lock();
	BOOL bReturn = LogWithoutDateAndTime(m_fPznLog, szText + "\n" );
	slLock.Unlock();

	return bReturn;
}

BOOL CMSLogFileUtility::WT_ClosePznLog()
{
	CSingleLock slLock(&m_CsPznLog);
	slLock.Lock();
	BOOL bReturn = CloseLogFile(m_fPznLog);
	slLock.Unlock();

	return bReturn;
}

BOOL CMSLogFileUtility::WT_IsPznLogOpen()
{
	return IsLogFileOpen(m_fPznLog);
}

//Xu Zhi Jin
BOOL CMSLogFileUtility::WT_GetCTLogOpen(CString szMapFileName)
{
	CTime theTime	= CTime::GetCurrentTime();
	CString szTime	= theTime.Format("_%Y%m%d_%H%M%S");

	//v4.46T13
	//CString szLogFileName = gszUSER_DIRECTORY + WT_GET_CT_LOG + szTime + ".log";
	CString szLogFileName = gszUSER_DIRECTORY + WT_GET_CT_LOG + "_" + szMapFileName + ".log";
	if (szMapFileName == "")
		szLogFileName = gszUSER_DIRECTORY + WT_GET_CT_LOG + szTime + ".log";

	m_szCT = "";
	m_lCTLog_LastMapRow			= 0;
	m_lCTLog_LastMapCol			= 0;
	m_lCTLog_TotalLookForward	= 0;
	m_lCTLog_TotalLookForward1	= 0;
	m_lCTLog_TotalLookForward2	= 0;
	m_lCTLog_TotalLookForward3	= 0;
	m_lCTLog_TotalLookForward4	= 0;
	m_lCTLog_TotalLookForward5	= 0;
	m_lCTLog_TotalLookForward6	= 0;
	m_lCTLog_TotalLookForward7	= 0;
	m_lCTLog_TotalTheta			= 0;
	m_lCTLog_TotalPick			= 0;
	m_lCTLog_TotalLongMotion	= 0;
	m_lCTLog_TotalDices			= 0;
	m_dCTLog_TotalDiePitchDist	= 0.00;

	CSingleLock slLock(&m_CsCTLog);			//v4.47T4
	slLock.Lock();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SearchAndRemoveFiles(gszUSER_DIRECTORY, 7, 0, ".log", FALSE);	//v4.46T13

	BOOL bStatus = OpenLogFile(m_fCTLog, szLogFileName, "a+");

	if (bStatus && WT_IsGetCTLogOpen())
	{
		WT_GetCTLog("\n\nNo,MapRow,MapCol,Dist,LookForward,Rotate,Pick,WT_Time(ms),LookForward(3x3),LookForward(5x5),LookForward(7x7),no LF,CT(ms)");
		WT_GetCTLog("=======================================================");
	}
	
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::WT_GetCTLog(CONST CString szText)
{
	//if (!GetEnableMachineLog())
	//	return FALSE;
	if (!WT_IsGetCTLogOpen())
		return FALSE;
	return LogWithoutDateAndTime(m_fCTLog, szText + "\n" );
}


BOOL CMSLogFileUtility::WT_GetCTLog_Update(	CONST LONG lMapRow, 
											CONST LONG lMapCol, 
											CONST LONG lMotionTime,
											CONST BOOL bLookForward,
											CONST BOOL bTheta,
											CONST BOOL bPick)
{
	if (!WT_IsGetCTLogOpen())
		return FALSE;
	if ( (lMapRow == m_lCTLog_LastMapRow) && (lMapCol == m_lCTLog_LastMapCol) )
		return TRUE;


	LONG lRow = labs(lMapRow - m_lCTLog_LastMapRow);
	LONG lCol = labs(lMapCol - m_lCTLog_LastMapCol);

	DOUBLE dDist = sqrt( pow((lMapRow - m_lCTLog_LastMapRow), 2.0) + pow((lMapCol - m_lCTLog_LastMapCol), 2.0) );


	m_lCTLog_LastMapRow	= lMapRow;
	m_lCTLog_LastMapCol	= lMapCol;
	
	BOOL bIsFov3 = 0, bIsFov5 = 0, bIsFov7 = 0, bIsNoLF = 1;

	if( bLookForward )
	{
		bIsNoLF = 0;
		LONG lMaxOffset = max(lRow, lCol);
		if( lMaxOffset<=1 )
		{
			bIsFov3 = 1;
		}
		else if( lMaxOffset<=2 )
		{
			bIsFov5 = 1;
		}
		else
		{
			bIsFov7 = 1;
		}
	}

	//v4.36T1
	if (dDist <= 1.415) 
	{
		m_lCTLog_TotalLookForward1++;
	}
	else if (dDist <= 2.83)
	{
		m_lCTLog_TotalLookForward2++;
	}
	else if (dDist <= 4.25)		//3-die-pitch
	{
		m_lCTLog_TotalLookForward3++;
	}
	else if (dDist <= 5.66)		//4-die-pitch
	{
		m_lCTLog_TotalLookForward4++;
	}
	else if (dDist <= 7.072)	//5-die-pitch
	{
		m_lCTLog_TotalLookForward5++;
	}
	else if (dDist <= 8.49)		//6-die-pitch
	{
		m_lCTLog_TotalLookForward6++;
	}
	else
	{
		m_lCTLog_TotalLookForward7++;
	}


	m_lCTLog_TotalDices++;
	if (bLookForward)
		m_lCTLog_TotalLookForward++;
	if (bTheta)
		m_lCTLog_TotalTheta++;
	if (bPick)
		m_lCTLog_TotalPick++;
	if (lMotionTime > 35)
		m_lCTLog_TotalLongMotion++;

	m_dCTLog_TotalDiePitchDist = m_dCTLog_TotalDiePitchDist + dDist;

	CString szData;
	szData.Format("%ld,%ld,%ld,%.2f,%d,%d,%d,%ld,%d,%d,%d,%d", 
		m_lCTLog_TotalDices+1, lMapRow, lMapCol, dDist, bLookForward, bTheta, bPick, lMotionTime, 
		bIsFov3, bIsFov5, bIsFov7, bIsNoLF);
	if (m_szCT.GetLength() == 0)
		m_szCT = ",0";
	szData = szData + m_szCT;

	CSingleLock slLock(&m_CsCTLog);			//v4.47T4
	slLock.Lock();
	BOOL bStatus = LogWithoutDateAndTime(m_fCTLog, szData + "\n");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::WT_GetCTLog_CT_NoUpdate(CONST DOUBLE dCycleTime)
{
	if (!WT_IsGetCTLogOpen())
		return FALSE;

	LONG lTime = (LONG) dCycleTime;
	m_szCT.Format(",%ld", lTime);

	return TRUE;
}

BOOL CMSLogFileUtility::WT_GetCTLogClose()
{
	if (!WT_IsGetCTLogOpen())
		return FALSE;

	DOUBLE dLFDiePercent  = 0;
	DOUBLE dLFDiePercent1 = 0;
	DOUBLE dLFDiePercent2 = 0;
	DOUBLE dLFDiePercent3 = 0;
	DOUBLE dLFDiePercent4 = 0;
	DOUBLE dLFDiePercent5 = 0;
	DOUBLE dLFDiePercent6 = 0;
	DOUBLE dLFDiePercent7 = 0;
	DOUBLE dTPercent = 0;
	DOUBLE dPickDiePercent = 0;
	DOUBLE dLMotionPercent = 0;
	DOUBLE dDistPerDie = 0.00;

	if (m_lCTLog_TotalDices > 0)
	{
		dLFDiePercent	= 100.00 * m_lCTLog_TotalLookForward	/ m_lCTLog_TotalDices;
		dTPercent		= 100.00 * m_lCTLog_TotalTheta			/ m_lCTLog_TotalDices;
		dPickDiePercent = 100.00 * m_lCTLog_TotalPick			/ m_lCTLog_TotalDices;
		dLMotionPercent = 100.00 * m_lCTLog_TotalLongMotion		/ m_lCTLog_TotalDices;
		dDistPerDie		= m_dCTLog_TotalDiePitchDist			/ m_lCTLog_TotalDices;
		
		dLFDiePercent1	= 100.00 * m_lCTLog_TotalLookForward1	/ m_lCTLog_TotalDices;
		dLFDiePercent2	= 100.00 * m_lCTLog_TotalLookForward2	/ m_lCTLog_TotalDices;
		dLFDiePercent3	= 100.00 * m_lCTLog_TotalLookForward3	/ m_lCTLog_TotalDices;
		dLFDiePercent4	= 100.00 * m_lCTLog_TotalLookForward4	/ m_lCTLog_TotalDices;
		
		dLFDiePercent5	= 100.00 * m_lCTLog_TotalLookForward5	/ m_lCTLog_TotalDices;
		dLFDiePercent6	= 100.00 * m_lCTLog_TotalLookForward6	/ m_lCTLog_TotalDices;
		dLFDiePercent7	= 100.00 * m_lCTLog_TotalLookForward7	/ m_lCTLog_TotalDices;
	}

	CSingleLock slLock(&m_CsCTLog);
	slLock.Lock();

	WT_GetCTLog("=======================================================\n");
	
	CString szLine;
	szLine.Format("Total no of die Indexing = %ld", m_lCTLog_TotalDices);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of PICK dices = %ld (%.2f pct)", m_lCTLog_TotalPick, dPickDiePercent);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of LOOKFORWARD dices = %ld (%.2f pct)", m_lCTLog_TotalLookForward, dLFDiePercent);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of ROTATE dices = %ld (%.2f pct)", m_lCTLog_TotalTheta, dTPercent);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of WT LONG-MOTION (>35ms) = %ld (%.2f pct)", m_lCTLog_TotalLongMotion, dLMotionPercent);
	WT_GetCTLog(szLine);
	szLine.Format("Total Distance (row-col pitch) = %.2f (per-die = %.4f)", m_dCTLog_TotalDiePitchDist, dDistPerDie);
	WT_GetCTLog(szLine);

	szLine.Format("\nTotal no of  1-die-pitch LF dices (3x3 FOV)  = %ld (%.2f pct)", m_lCTLog_TotalLookForward1, dLFDiePercent1);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of  2-die-pitch LF dices (5x5 FOV)   = %ld (%.2f pct)", m_lCTLog_TotalLookForward2, dLFDiePercent2);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of  3-die-pitch LF dices (7x7 FOV)	 = %ld (%.2f pct)", m_lCTLog_TotalLookForward3, dLFDiePercent3);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of  4-die-pitch LF dices (9x9 FOV)   = %ld (%.2f pct)", m_lCTLog_TotalLookForward4, dLFDiePercent4);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of  5-die-pitch LF dices (11x11 FOV) = %ld (%.2f pct)", m_lCTLog_TotalLookForward5, dLFDiePercent5);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of  6-die-pitch LF dices (13x13 FOV) = %ld (%.2f pct)", m_lCTLog_TotalLookForward6, dLFDiePercent6);
	WT_GetCTLog(szLine);
	szLine.Format("Total no of >6-die-pitch LF dices             = %ld (%.2f pct)", m_lCTLog_TotalLookForward7, dLFDiePercent7);
	WT_GetCTLog(szLine);

	BOOL bStatus = CloseLogFile(m_fCTLog);
	
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::WT_IsGetCTLogOpen()
{
	return IsLogFileOpen(m_fCTLog);
}


BOOL CMSLogFileUtility::BackupCTLogAtWaferEnd(CONST CString szWaferID)
{
	CString szLogFileName = gszUSER_DIRECTORY + WT_GET_CT_LOG + ".log";
	CString szNewFileName = gszUSER_DIRECTORY + WT_GET_CT_LOG + "_" + szWaferID + ".log";

	if (_access(szLogFileName, 0) == -1)
	{
		return TRUE;
	}

	if (CopyFile(szLogFileName, szNewFileName, FALSE))
	{
		DeleteFile(szLogFileName);
	}
	return TRUE;
}


BOOL CMSLogFileUtility::WT_PrLogOpen()
{
	CString szLogFileName = gszUSER_DIRECTORY + WT_GET_PR_LOG;
	if( WT_IsPrLogOpen() )
		return TRUE;
	return OpenLogFile(m_fPrLog, szLogFileName, "w+");
}

BOOL CMSLogFileUtility::WT_GetPrLog(CONST CString szText)
{
	if (!GetEnableWtMachineLog())
		return FALSE;

	if (!WT_IsPrLogOpen())
		return FALSE;

	return LogWithDateAndTime(m_fPrLog, szText + "\n" );
}

BOOL CMSLogFileUtility::WT_GetPrLogClose()
{
	return CloseLogFile(m_fPrLog);
}

BOOL CMSLogFileUtility::WT_IsPrLogOpen()
{
	return IsLogFileOpen(m_fPrLog);
}


BOOL CMSLogFileUtility::WT_MapDieTypeCheckLog(CONST CString szText)
{
	CString szLogFileName = gszUSER_DIRECTORY + WT_MAP_DIE_TYPE_CHECK_LOG;
	
	return LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");
}

//--------------------------------------//
//		Log File For WPR				//
//--------------------------------------//

// used in Log_AlignWafer
BOOL CMSLogFileUtility::WPR_AlignWaferLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + WPR_ALIGN_WAFER_LOG;

	return LogWithDateAndTime(szLogFileName,szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::WPR_BackupAlignWaferLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WPR_ALIGN_WAFER_LOG);
}

//used in CWaferPr::FindDiePitch
BOOL CMSLogFileUtility::WPR_LearnPitchLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE)
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + WPR_LEARN_PITCH_LOG;
	
	return LogWithoutDateAndTime(szLogFileName,szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::WPR_BackupLearnPitchLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WPR_LEARN_PITCH_LOG);
}

//used in LookForward Compensation Die
BOOL CMSLogFileUtility::WPR_WaferDieOffsetLog(CONST CString szText, BOOL bLogAlways)
{
	if ( (GetEnableMachineLog() == FALSE) && !bLogAlways )	//andrewng //2020-1020
	{
		return TRUE;
	}

	CString szLogFileName = gszUSER_DIRECTORY + WPR_DIE_OFFSET_LOG;
	return LogWithoutDateAndTime(szLogFileName,szText + "\n" ,"a+");
}

BOOL CMSLogFileUtility::WPR_BackupWaferDieOffsetLog()
{
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WPR_DIE_OFFSET_LOG);
}

//--------------------------------------//
//	Log File Utility General Function	//
//--------------------------------------//

CMSLogFileUtility* CMSLogFileUtility::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CMSLogFileUtility();

	return m_pInstance;
}


VOID CMSLogFileUtility::SetEnableMachineLog(BOOL bSet)
{
	m_bEnableMachineLog		= bSet;
}

VOID CMSLogFileUtility::SetEnableWtMachineLog(BOOL bSet)
{
	m_bEnableWtMachineLog  = bSet;
}

BOOL CMSLogFileUtility::GetEnableMachineLog()
{
	return m_bEnableMachineLog;
}

BOOL CMSLogFileUtility::GetEnableWtMachineLog()
{
	return m_bEnableWtMachineLog;
}

VOID CMSLogFileUtility::SetEnableBurnIn(BOOL bSet)				//v3.87
{
	m_bBurnin  = bSet;
}

BOOL CMSLogFileUtility::GetEnableTableIndexLog()
{
	return m_bEnableTableIndexLog;
}

VOID CMSLogFileUtility::SetEnableTableIndexLog(BOOL bSet)
{
	m_bEnableTableIndexLog  = bSet;
}

BOOL CMSLogFileUtility::LogWithoutDateAndTime(CONST CString szLogFileName, CONST CString szText, CONST CString szOpenFileMode)
{
	FILE* fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFileName, szOpenFileMode);

	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%s", (LPCTSTR) szText); 
		fclose(fp);
		return TRUE;
	}

	return FALSE;
}

// Function to Log Data With Date And Time With Open & Close File
BOOL CMSLogFileUtility::LogWithDateAndTime(CONST CString szLogFileName, CONST CString szText, CONST CString szOpenFileMode)
{
	FILE* fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFileName, szOpenFileMode);
	
	if ((nErr == 0) && (fp != NULL))
	{
		SYSTEMTIME sysTm;
		GetLocalTime(&sysTm);
		CString szTime;
		szTime.Format("%04d-%02d-%02d (%02d:%02d:%02d.%03d)  ", 
				sysTm.wYear, sysTm.wMonth, sysTm.wDay, sysTm.wHour, sysTm.wMinute, sysTm.wSecond, sysTm.wMilliseconds);
		if( szText=="\n" )
			fprintf(fp, "%s", (LPCTSTR) szText); 
		else
			fprintf(fp, "%s - %s", (LPCTSTR) szTime, (LPCTSTR) szText); 
		fclose(fp);
		return TRUE;
	}

	return FALSE;
}

// Function to Log Data With Date And Time Without Open & Close File 
BOOL CMSLogFileUtility::LogWithDateAndTime(FILE* &fp, CONST CString szText)
{
	if (fp == NULL)
	{
		return FALSE;
	}

	//CTime theTime = CTime::GetCurrentTime();
	//CString szTime = theTime.Format("%Y-%m-%d (%H:%M:%S)");
	SYSTEMTIME sysTm;
	GetLocalTime(&sysTm);
	CString szTime;
	szTime.Format("%04d-%02d-%02d (%02d:%02d:%02d.%03d)  ", 
			sysTm.wYear, sysTm.wMonth, sysTm.wDay, sysTm.wHour, sysTm.wMinute, sysTm.wSecond, sysTm.wMilliseconds);
	if( szText == "\n" )
		fprintf(fp, "\n");
	else
		fprintf(fp, "%s - %s", (LPCTSTR) szTime, (LPCTSTR) szText); 
	
	return TRUE;
	
}

BOOL CMSLogFileUtility::LogWithTimeOnly(FILE* &fp, CONST CString szText)
{
	if (fp == NULL)
	{
		return FALSE;
	}

	SYSTEMTIME sysTm;
	GetLocalTime(&sysTm);
	CString szTime;
	szTime.Format("%04d-%02d-%02d (%02d:%02d:%02d.%03d)  ", 
			sysTm.wYear, sysTm.wMonth, sysTm.wDay, sysTm.wHour, sysTm.wMinute, sysTm.wSecond, sysTm.wMilliseconds);

	if( szText == "\n" )
		fprintf(fp, "\n");
	else
		fprintf(fp, "%s - %s", (LPCTSTR) szTime, (LPCTSTR) szText); 
	
	return TRUE;
	
}

BOOL CMSLogFileUtility::LogWithoutDateAndTime(FILE* &fp, CONST CString szText)
{
	if (fp == NULL)
	{
		return FALSE;
	}

	if( szText == "\n" )
		fprintf(fp, "\n");
	else
		fprintf(fp, "%s", (LPCTSTR) szText); 
	
	return TRUE;
	
}

BOOL CMSLogFileUtility::IsLogFileOpen(FILE* &fp)
{
	if (fp == NULL)
		return FALSE;

	return TRUE;
}

// Function to open log file
BOOL CMSLogFileUtility::OpenLogFile(FILE* &fp, CONST CString szLogFileName, CONST CString szOpenFileMode)
{
	// File is already open
	if (fp != NULL)
	{
		return TRUE;
	}
	
	errno_t nErr = fopen_s(&fp, szLogFileName, szOpenFileMode);

	// Open the File & success
	if ((nErr == 0) && (fp != NULL))
	{
		return TRUE;
	}

	fp = NULL;
	// Fail to open file
	return FALSE;
}

// Function to close log file
BOOL CMSLogFileUtility::CloseLogFile(FILE* &fp)
{
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	
	return TRUE;

}

// Function to check and backup the log file
// If the file size of the log file > LOG_FILE_SIZE_LIMIT
// The log file will renamed to *.bak
BOOL CMSLogFileUtility::CheckAndBackupSingleLogFile(CString szLogFileName, ULONGLONG nFileSizeLimit)
{
	INT nCol =0;
	ULONGLONG nFileSize =0;
	CString szBpLogFileName;
	CFile fLogFile;

	if  (fLogFile.Open(szLogFileName, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	nFileSize = fLogFile.GetLength();
	fLogFile.Close();

	// Case of no need to backup the log file
	if (nFileSize < nFileSizeLimit )
	{
		return TRUE;
	}

	// Start backup log file
	nCol = szLogFileName.ReverseFind('.');

	if (nCol != -1)
		szBpLogFileName = szLogFileName.Left(nCol) + ".bak";
	else
		szBpLogFileName = szLogFileName + ".bak";

	try
	{
		CopyFile(szLogFileName,szBpLogFileName,FALSE);
		DeleteFile(szLogFileName);
	}
	catch (CFileException e)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMSLogFileUtility::CheckAndBackupSingleLogFileWithAppend(CString szLogFileName, ULONGLONG nFileSizeLimit)
{
	INT nCol =0;
	ULONGLONG nFileSize =0;
	CString szBpLogFileName;
	CFile fLogFile;
	CStdioFile cfStatusFile;
	CStdioFile cfBackupStatusFile;
	BOOL bStatusFileExist = FALSE;
	BOOL bBackuupStatusFileExist = FALSE;
	CString szContent;

	if  (fLogFile.Open(szLogFileName,CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	nFileSize = fLogFile.GetLength();
	fLogFile.Close();

	// Case of no need to backup the log file
	if (nFileSize < nFileSizeLimit )
	{
		return TRUE;
	}

	nCol = szLogFileName.ReverseFind('.');

	if (nCol != -1)
		szBpLogFileName = szLogFileName.Left(nCol) + ".bak";
	else
		szBpLogFileName = szLogFileName + ".bak";
	
	bStatusFileExist = cfStatusFile.Open(szLogFileName, 
								CFile::modeRead|CFile::shareExclusive|CFile::typeText);
	
	bBackuupStatusFileExist = cfBackupStatusFile.Open(szBpLogFileName, 
									CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone|CFile::typeText);

	if ( (bStatusFileExist == TRUE) && (bBackuupStatusFileExist == TRUE) )
	{
		cfStatusFile.SeekToBegin();
		cfBackupStatusFile.SeekToEnd();
		
		//Start in next line
		cfBackupStatusFile.WriteString("\n");

		while(cfStatusFile.ReadString(szContent))
		{
			szContent = szContent + "\n";
			cfBackupStatusFile.WriteString(szContent);
		}
	}


	if ( bStatusFileExist == TRUE )
	{
		cfStatusFile.Close();
	}

	if ( bBackuupStatusFileExist == TRUE )
	{
		cfBackupStatusFile.Close();
	}

	try
	{
		BOOL bOK = DeleteFile(szLogFileName);
	}
	catch (CFileException e)
	{
		return FALSE;
	}

	return TRUE;
}

// Function to check and backup all log files

VOID CMSLogFileUtility::CheckAndBackupAllLogFiles()
{

	CString szLogFileNames[LOG_FILES_NEED_TO_BACKUP] = {WL_STATUS_LOG, WL_DEBUG_LOG, BT_BACKUP_MAP_LOG , BT_STATUS_LOG,
			BT_CLEAR_BIN_LOG, BT_GEN_TEMP_FILE_LOG, BT_REALIGN_BIN_FRAME_LOG, BH_STATUS_LOG, BH_DIE_COUNTER_LOG,
			BL_STATUS_LOG, WPR_ALIGN_WAFER_LOG, WPR_LEARN_PITCH_LOG, MS_WAFER_STATISTIC_PRETASK_LOG};

	for (INT i=0; i<LOG_FILES_NEED_TO_BACKUP; i++)
		CheckAndBackupSingleLogFile(gszUSER_DIRECTORY+szLogFileNames[i]);

}


BOOL CMSLogFileUtility::RemoveFilesInFolder(CString szFilePath)
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString szFilename = "";
	
	hSearch = FindFirstFile(szFilePath + "\\" + "*", &FileData);

	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return FALSE;
	}

	do 
	{
		// if it is a folder recurive call to remove file
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;
        if ( (strcmp(FileData.cFileName, ".")==0) || (strcmp(FileData.cFileName, "..")==0) )
            continue;

		szFilename = szFilePath + "\\" + FileData.cFileName;
		DeleteFile(szFilename);
				
	} while (FindNextFile(hSearch, &FileData) == TRUE);
 
	// Close the search handle. 
	if ( FindClose(hSearch) == FALSE) 
	{ 
		return FALSE;
	} 
	
	return TRUE;
}


BOOL CMSLogFileUtility::WT_BackupLogStatus()
{	
	return CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WT_STATUS_LOG);
}

BOOL CMSLogFileUtility::WT_LogStatus(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + WT_STATUS_LOG;
	return LogWithDateAndTime(szLogFileName, "WT " + szText + "\n", "a+");
}

BOOL CMSLogFileUtility::WT_BackupWaferTableLevelLog()
{	
	CSingleLock slLock(&m_CsWaferTableLevelLog);
	slLock.Lock();
	BOOL bStatus = CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WT_LEVEL_LOG);
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::WT_WaferTableLevelLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;

	CSingleLock slLock(&m_CsWaferTableLevelLog);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + WT_LEVEL_LOG;
	BOOL bStatus = LogWithDateAndTime(szLogFileName, szText + "\n", "a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::WT_BackupGenFileLog()
{
	CSingleLock slLock(&m_CsProberGenMapFileLog);
	slLock.Lock();
	BOOL bStatus = CheckAndBackupSingleLogFile(gszUSER_DIRECTORY + WT_GENMAP_LOG);
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::ProberGenMapFileLog(CONST CString szText)
{
	if (GetEnableMachineLog() == FALSE )
		return TRUE;
	
	CSingleLock slLock(&m_CsProberGenMapFileLog);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + WT_GENMAP_LOG;
	BOOL bStatus = LogWithDateAndTime(szLogFileName, szText + "\n", "a+");
	slLock.Unlock();
	return bStatus;
}

//====================================================================================================
VOID CMSLogFileUtility::ChangeFileNewName(const CString szLogFile, const CString szNewLogFile, const BOOL bEnableLog)
{
	FILE *fp = NULL;
	BOOL bChangFileName = FALSE;
	LONG len;

	if (!bEnableLog)
	{
		return;
	}

	errno_t nErr = fopen_s(&fp, szLogFile, "a+t");
	if ((nErr == 0) && (fp != NULL))
	{
		if ((len = _filelength(_fileno(fp))) > 1024 * 1024 * 2)
		{
			bChangFileName = TRUE;
		}
		fclose(fp);
	}

	if (bChangFileName)
	{
		CString szNewFileName;

		struct tm *t;
		time_t tNow;
		time(&tNow);
		t = localtime(&tNow);
		if (t != NULL)
		{
			szNewFileName.Format("%s\\MissingDieLog\\%s%04d%02d%02d%02d%02d%02d.txt", (const char*)gszUSER_DIRECTORY, (const char*)szNewLogFile, 1900 + t->tm_year, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		}
		::CopyFile(szLogFile, szNewFileName, FALSE);
		remove(szLogFile);
		CheckLogStatus(szLogFile);
	}
}


VOID CMSLogFileUtility::CreatePath(const CString szFileName, CString &szPath, CString &szName)
{
	int n = 0;
	if (n = szFileName.ReverseFind('\\'))
	{
		szPath = szFileName.Left(n);
		szName = szFileName.Right(szFileName.GetLength() - n - 1);
	}
	else
	{
		szPath = "";
		szName = szFileName;
	}
}


BOOL CMSLogFileUtility::CheckLogStatus(const CString szFileName)
{
	CString szPath, szName, szFile;

	CreatePath(szFileName, szPath, szName);

	CFile File;
	CFileFind FileFinder;
	CStringArray arrFile;
	BOOL bFindFile = FALSE;	

	CSingleLock	slLock(&m_csResource);
	slLock.Lock();
	
	TRY
	{
		INT nLeft, nIdx = szName.Find(".");
		CString szExt = "";

		if (nIdx == -1)
		{
			if (szName == "")
			{
				szName += ".txt";			
			}
		}

		nLeft = szName.GetLength() - nIdx;
		szExt = szName.Right(nLeft - 1);

		CString szTemp = szName;
		if (szName.GetLength() >= nLeft) 
		{
			szTemp = szName.Left(szName.GetLength() - nLeft);
		}

		szFile = szPath + _T("\\") + szTemp + _T("*") + _T(".") + szExt;	
		bFindFile = FileFinder.FindFile(szFile);

		while (bFindFile)
		{
			bFindFile = FileFinder.FindNextFile();
			arrFile.Add(FileFinder.GetFilePath());
		}
		
		int nStartIndex, nIndex = 0, nFile = (int)arrFile.GetSize();

		if (nFile > m_nLogBackup)
		{
			CString	szMin = arrFile[0];

			if (szMin == szFileName)
			{
				szMin = arrFile[1];
				nStartIndex = 2;
				nIndex = 1;
			}
			else
			{
				nStartIndex = 1;
				nIndex = 0;
			}
		
			for (int i = nStartIndex; i < nFile; i++)
			{
				if (szMin > arrFile[i] && arrFile[i] != szFileName)
				{
					szMin = arrFile[i];
					nIndex = i;
				}
			}

			CFile::Remove(szMin);
			arrFile.RemoveAll();
		}
	}
	CATCH(CFileException, e)
	{
		TCHAR   szCause[255];
		CString szError;
		e->GetErrorMessage(szCause, 500);
		szError.Format("\nCFileException with Error:%s", szCause);
		TRACE(szError);
	}
	END_CATCH

	return TRUE;
}


BOOL CMSLogFileUtility::MissingDieThreshold_IsLogOpen(const BOOL bBHZ2)
{
	return IsLogFileOpen(bBHZ2? m_fMissingDieThresholdLog2 : m_fMissingDieThresholdLog1);
}

BOOL CMSLogFileUtility::MissingDieThreshold_LogOpen(const BOOL bBHZ2)
{
	CString szLogFileName = gszUSER_DIRECTORY + (bBHZ2 ? MissingDieThreshold_LOG2 : MissingDieThreshold_LOG1);

	CString szPath, szName;

	CreatePath(szLogFileName, szPath, szName);
	if (_access(szPath, 0) != 0)
	{
		_mkdir(szPath);
	}

	if (MissingDieThreshold_IsLogOpen(bBHZ2))
	{
		return TRUE;
	}

	ChangeFileNewName(szLogFileName, bBHZ2? "MissingDieThreshold2" : "MissingDieThreshold1", TRUE);
	BOOL bStatus = OpenLogFile(bBHZ2? m_fMissingDieThresholdLog2 : m_fMissingDieThresholdLog1, szLogFileName, "a+");
	return bStatus;
}

BOOL CMSLogFileUtility::MissingDieThreshold_Log(const BOOL bBHZ2, CONST CString szText)
{
	if (!m_bEnableMachineLog)
	{
		return FALSE;
	}

	if (!MissingDieThreshold_IsLogOpen(bBHZ2))
	{
		return FALSE;
	}
	
	BOOL bStatus = TRUE;

	bStatus =  LogWithDateAndTime(bBHZ2? m_fMissingDieThresholdLog2 : m_fMissingDieThresholdLog1, szText + "\n" );
	return bStatus;
}


BOOL CMSLogFileUtility::MissingDieThreshold_LogClose(const BOOL bBHZ2)
{
	BOOL bStatus = CloseLogFile(bBHZ2 ? m_fMissingDieThresholdLog2 : m_fMissingDieThresholdLog1);
	return bStatus;
}

//=========================================================================================
BOOL CMSLogFileUtility::MS60_IsLogOpen()
{
	return IsLogFileOpen(m_fMS60Log);
}

BOOL CMSLogFileUtility::MS60_LogOpen()
{
	CString szLogFileName = gszUSER_DIRECTORY + MS60_LOG;

	if( MS60_IsLogOpen() )
		return TRUE;
	CSingleLock slLock(&m_CsMS60Log);
	slLock.Lock();
	DeleteFile(szLogFileName);
	BOOL bStatus = OpenLogFile(m_fMS60Log, szLogFileName, "a+");
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::MS60_Log(CONST CString szText)
{
	if (!m_bEnableMachineLog)
	{
		return FALSE;
	}

	if (!MS60_IsLogOpen())
		return FALSE;
	
	BOOL bStatus = TRUE;

	CSingleLock slLock(&m_CsMS60Log);
	slLock.Lock();
	bStatus =  LogWithDateAndTime(m_fMS60Log, szText + "\n" );	
	slLock.Unlock();
	return bStatus;
}

BOOL CMSLogFileUtility::MS60_LogClose()
{
	CSingleLock slLock(&m_CsMS60Log);
	slLock.Lock();
	BOOL bStatus = CloseLogFile(m_fMS60Log);
	slLock.Unlock();
	return bStatus;
}

// CP100 Pin Log/		/v1.01T600
BOOL CMSLogFileUtility::CP100_IsPinLogOpen()
{
	return IsLogFileOpen(m_fCP100PinLog);
}

BOOL CMSLogFileUtility::CP100_PinLogOpen()
{
	CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);
	CString szLogFileName = szLogPath + CP100_PIN_LOG;

	if ( CP100_IsPinLogOpen() == TRUE )
	{
		return TRUE;
	}

	CSingleLock slLock(&m_CsCP100PinLog);
	slLock.Lock();
	BOOL bStatus = OpenLogFile(m_fCP100PinLog, szLogFileName, "a+");
	slLock.Unlock();

	return bStatus;
}

BOOL CMSLogFileUtility::CP100_PinLog(CONST CString szText)
{
	if ( CP100_IsPinLogOpen() == FALSE )
	{
		return FALSE;
	}

	CSingleLock slLock(&m_CsCP100PinLog);
	slLock.Lock();
	BOOL bStatus =  LogWithDateAndTime(m_fCP100PinLog, szText + "\n" );	
	slLock.Unlock();

	return bStatus;
}

BOOL CMSLogFileUtility::CP100_PinLogClose()
{
	if ( CP100_IsPinLogOpen() == FALSE )
	{
		return FALSE;
	}

	CSingleLock slLock(&m_CsCP100PinLog);
	slLock.Lock();
	BOOL bStatus = CloseLogFile(m_fCP100PinLog);
	slLock.Unlock();

	return bStatus;
}


BOOL CMSLogFileUtility::Uplook_LogDieDataStatus(CONST CString szText)
{
/*
	CString szPath;	
	CString szYear, szMonth, szDay,szHour;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());
	szDay.Format("%d",	CurTime.GetDay());
	szHour.Format("%d", CurTime.GetHour());
	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}
	if ( CurTime.GetDay() < 10 )
	{
		szDay = "0" + szDay;
	}
	if ( CurTime.GetHour() < 10 )
	{
		szHour = "0" + szHour;
	}
	szPath = gszUSER_DIRECTORY + UPLOOK_STATUS_LOG + "_DieData_"+ szYear+ szMonth + szDay+ szHour+".csv";
	return LogWithDateAndTime(szPath, szText + "\n","a+");
*/
	return TRUE;
}


BOOL CMSLogFileUtility::NVC_AutoBondLogOperation(CONST CString szText)
{

	CSingleLock slLock(&m_CsOperation);
	slLock.Lock();
	CString szLogFileName = gszUSER_DIRECTORY + NVC_AUTO_BOND_OPERATION_LOG;

	BOOL bStatus = TRUE;
	
	if (szText.GetLength() != 0)
	{
		bStatus = LogWithDateAndTime(szLogFileName,		szText + "\n" ,"a+");
	}

	slLock.Unlock();
	return bStatus;
}