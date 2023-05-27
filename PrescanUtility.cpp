#include "stdafx.h"
#include "PrescanUtility.h"
#include "LogFileUtil.h"
#include "Mmsystem.h"
#include "MS896A.h"
#include "PRFailureCaseLog.h"

CMSPrescanUtility* CMSPrescanUtility::m_pInstance = NULL;


CMSPrescanUtility::CMSPrescanUtility()
{
	InitPrescanVariables();
} //end constructor


CMSPrescanUtility::~CMSPrescanUtility()
{
} //end destructor


CMSPrescanUtility* CMSPrescanUtility::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CMSPrescanUtility();
	return m_pInstance;

} //end Instance




VOID CMSPrescanUtility::SetAoiScanMethod(ULONG ulMethod)
{
	BOOL bAccurateMap		= FALSE;
	BOOL bBarWafer			= FALSE;

	switch( ulMethod )
	{
	case ES_SCAN_NOMAP_WAFER:	// no map, find all die with correct coordinate, no sorting, for AOI
		bAccurateMap		= TRUE;
		break;
	case ES_SCAN_BAR_WAFER:
		bBarWafer		= TRUE;
		break;
	case ES_SCAN_NOMAP_BAR_WAFER:
		bAccurateMap		= TRUE;
		bBarWafer		= TRUE;
		break;
	case ES_SCAN_MAP_WAFER:
	default:
		break;
	}

	m_bPrescanRegionMode	= FALSE;
	m_bDummyMapScan			= bAccurateMap;		// dummy map created by ctrl s/w
	m_bPrescanAoiWafer		= bAccurateMap;
	m_bScanAreaPickMode		= FALSE;
	m_bBarWaferScan			= bBarWafer;
}

VOID CMSPrescanUtility::SetProberScanMethod(ULONG ulMethod)
{
	BOOL bRegionScan		= FALSE;
	BOOL bExactMap		= FALSE;

	switch( ulMethod )
	{
	case CP_SCAN_MAP_REGION_WAFER:	// has map, scan and probe region by region
		bRegionScan = TRUE;
		break;

	case CP_SCAN_PROBE_EXACT_MAP:	// no map, find all with correct coordinate, and probe all die in wafer
		bExactMap	= TRUE;
		break;
	case CP_SCAN_CONT_PROBE_WAFER:
	default:
		break;
	}

	m_bPrescanRegionMode	= bRegionScan;
	m_bDummyMapScan			= bExactMap;		// dummy map created by ctrl s/w
	m_bPrescanAoiWafer		= bExactMap;
	m_bScanAreaPickMode		= FALSE;
	m_bBarWaferScan			= FALSE;
}

VOID CMSPrescanUtility::SetPrescanMethod(ULONG ulMethod)
{
	BOOL bRegionScan		= FALSE;
	BOOL bScannedMap		= FALSE;
	BOOL bExactWaferScan	= FALSE;
	BOOL bScanAreaPicking	= FALSE;
	BOOL bBarWafer			= FALSE;

	switch( ulMethod )
	{
	case SCAN_METHOD_MAP_REGION_WAFER:	// has map, scan and sort region by region
		bRegionScan = TRUE;
		break;
	case SCAN_METHOD_DUMMY_SORT_ALL:	// no map, find and sort all die in wafer
		bScannedMap	= TRUE;
		break;
	case SCAN_METHOD_DUMMY_EXACT_MAP:	// no map, find all die with correct coordinate, no sorting, for AOI
		bScannedMap	= TRUE;
		bExactWaferScan = TRUE;
		break;
	case SCAN_METHOD_DUMMY_AREA_PICKING:
		bScannedMap	= TRUE;
		bScanAreaPicking	= TRUE;
		break;
	case SCAN_METHOD_BAR_WAFER:
		bBarWafer = TRUE;
		break;
	case SCAN_METHOD_MAP_WHOLE_WAFER:
	default:
		break;
	}

	m_bPrescanRegionMode	= bRegionScan;
	m_bDummyMapScan			= bScannedMap;		// dummy map created by ctrl s/w
	m_bPrescanAoiWafer		= bExactWaferScan;
	m_bScanAreaPickMode		= bScanAreaPicking;
	m_bBarWaferScan			= bBarWafer;
}

BOOL CMSPrescanUtility::GetPrescanDummyMap()	// dummy map created by controller s/w
{
	return GetPrescanEnable() && m_bDummyMapScan;
}

BOOL CMSPrescanUtility::GetPrescanAoiWafer()	// dummy wafer scan, create dummy map, but build like wafer scan
{
	return GetPrescanEnable() && m_bPrescanAoiWafer;
}

BOOL CMSPrescanUtility::GetPrescanBarWafer()	// bar shape wafer scan
{
	return GetPrescanEnable() && m_bBarWaferScan;
}

BOOL CMSPrescanUtility::GetPrescanRegionMode()	// for prescan region mode
{
	return GetPrescanEnable() && m_bPrescanRegionMode;
}

VOID CMSPrescanUtility::SetRegionPickMode(CONST BOOL bEnable)	// region pick mode
{
	m_bRegionPickMode = bEnable;
}

BOOL CMSPrescanUtility::GetRegionPickMode()	// region pick mode
{
	return m_bRegionPickMode;
}

BOOL CMSPrescanUtility::GetPrescanAreaPickMode()	//	for prescan 1 line and then sorting for dummy wafer
{
	return GetPrescanEnable() && m_bScanAreaPickMode;
}

VOID CMSPrescanUtility::SetRealignPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY)
{
	m_lPrescanRealignMapRow = lRow;
	m_lPrescanRealignMapCol = lCol;
	m_lPrescanRealignWftPnX = lEncX;
	m_lPrescanRealignWftPnY = lEncY;
}

VOID CMSPrescanUtility::GetRealignPosition(LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY)
{
	lRow = m_lPrescanRealignMapRow;
	lCol = m_lPrescanRealignMapCol;
	lEncX= m_lPrescanRealignWftPnX;
	lEncY= m_lPrescanRealignWftPnY;
}


VOID CMSPrescanUtility::SetAlignPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY)
{
	m_lAlignMapPosnX = lRow;
	m_lAlignMapPosnY = lCol;
	m_lAlignWftPosnX = lEncX;
	m_lAlignWftPosnY = lEncY;
	m_ulAlignPrFrameID	= 0;
	m_ulSoraaAlign2PrFrameID	= 0;
}

VOID CMSPrescanUtility::GetAlignPosition(LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY)
{
	lRow = m_lAlignMapPosnX;
	lCol = m_lAlignMapPosnY;
	lEncX = m_lAlignWftPosnX;
	lEncY = m_lAlignWftPosnY;
}

ULONG CMSPrescanUtility::GetAlignPrFrameID()
{
	return m_ulAlignPrFrameID;
}

VOID  CMSPrescanUtility::SetAlignPrFrameID(ULONG ulFrameID)
{
	m_ulAlignPrFrameID	= ulFrameID;
}

ULONG CMSPrescanUtility::GetSoraaAlign2PrFrameID()
{
	return m_ulSoraaAlign2PrFrameID;
}

VOID  CMSPrescanUtility::SetSoraaAlign2PrFrameID(ULONG ulFrameID)
{
	m_ulSoraaAlign2PrFrameID	= ulFrameID;
}



VOID CMSPrescanUtility::SetPrescanMapIndexIdle(CONST BOOL bIsIdle)
{
	m_bMultiSrchInIdle = bIsIdle;
}

BOOL CMSPrescanUtility::GetPrescanMapIndexIdle()
{
	return m_bMultiSrchInIdle;
}

VOID CMSPrescanUtility::PrescanMoveLog(CString szMessage, CONST BOOL bLog)
{
	if( GetPrescanDebug() || (bLog&&CMSLogFileUtility::Instance()->GetEnableMachineLog()) )
	{
		CSingleLock slLock(&m_CsMoveLog);
		slLock.Lock();
		CString szLogFile;

		szLogFile	= m_szPrescanLogPath + PRESCAN_MOVE_POSN;

		FILE *fp = NULL;
		errno_t nErr = fopen_s(&fp, szLogFile, "a+");
		if ((nErr == 0) && (fp != NULL))
		{
			CTime theTime = CTime::GetCurrentTime();
			fprintf(fp, "%02d:%02d:%02d - %s\n", 
				theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LPCTSTR) szMessage);
			fclose(fp);
		}
		slLock.Unlock();
	}
} // prescan move log


VOID CMSPrescanUtility::RegionOrderLog(CString szMsg, CONST BOOL bLog)
{
	if( GetRegionPickMode() || GetPrescanRegionMode() || bLog )
	{
		CSingleLock slLock(&m_CsRegionOrderLog);
		slLock.Lock();
		FILE	*pPScanLog = NULL;
		CString szLogFile;
		CTime theTime = CTime::GetCurrentTime();

		szLogFile = m_szPrescanLogPath + PRESCAN_REGION_LOG;
		errno_t nErr = fopen_s(&pPScanLog, szLogFile, "a+");
		if ((nErr == 0) && (pPScanLog != NULL))
		{
			if( szMsg.IsEmpty() )
			{
				fprintf(pPScanLog, "\n");
			}
			else
			{
				fprintf(pPScanLog, "%2d (%2d:%2d:%2d) - %s\n", theTime.GetDay(),
					theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LPCTSTR) szMsg);  
			}
			fclose(pPScanLog);		//Klocwork
		}
		slLock.Unlock();
	}
}

VOID CMSPrescanUtility::ScanImageHistory(CString szMsg, CONST BOOL bNewOne)
{
	CString szLogFile	=  gszUSER_DIRECTORY + "\\History\\ScanImagesHistory.txt";
	if( bNewOne )
	{
		DeleteFile(szLogFile);
	}

	if( CMSLogFileUtility::Instance()->GetEnableMachineLog() )
	{
//		CMS896AApp			*pApp = (CMS896AApp*) AfxGetApp();
//		if( pApp->GetProfileInt(gszPROFILE_SETTING, _T("Asm special debug flag"), 0)>0 )
//		CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
//		if (pPRFailureCaseLog->IsEnableFailureCaselog())
		{
			CSingleLock slLock(&m_CsRegionOrderLog);
			slLock.Lock();

			FILE *fp = NULL;
			errno_t nErr = fopen_s(&fp, szLogFile, "a+");
			if ((nErr == 0) && (fp != NULL))
			{
				SYSTEMTIME sysTm;
				GetLocalTime(&sysTm);

				fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d.%03d - %s\n",
						sysTm.wYear, sysTm.wMonth, sysTm.wDay, sysTm.wHour, sysTm.wMinute, sysTm.wSecond, sysTm.wMilliseconds, (LPCTSTR) szMsg);

//				CTime theTime = CTime::GetCurrentTime();
//				fprintf(fp, "%02d:%02d:%02d - %s\n", 
//					theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LPCTSTR) szMsg);
				fclose(fp);
			}
			slLock.Unlock();
		}
	}
}


VOID CMSPrescanUtility::AddAllReferPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY)
{
	m_dwaReferRow.Add(lRow);
	m_dwaReferCol.Add(lCol);
	m_dwaReferWfX.Add(lEncX);
	m_dwaReferWfY.Add(lEncY);

	m_dwaPrescanReRefRow.Add(lRow);
	m_dwaPrescanReRefCol.Add(lCol);
	m_dwaPrescanReRefWfX.Add(lEncX);
	m_dwaPrescanReRefWfY.Add(lEncY);

	m_unNumOfReferPoints++;
}

VOID CMSPrescanUtility::RemoveAllReferPoints()
{
	m_dwaReferRow.RemoveAll();
	m_dwaReferCol.RemoveAll();
	m_dwaReferWfX.RemoveAll();
	m_dwaReferWfY.RemoveAll();

	m_dwaPrescanReRefRow.RemoveAll();
	m_dwaPrescanReRefCol.RemoveAll();
	m_dwaPrescanReRefWfX.RemoveAll();
	m_dwaPrescanReRefWfY.RemoveAll();

	m_unNumOfReferPoints = 0;
}

UINT CMSPrescanUtility::GetNumOfReferPoints()
{
	return m_unNumOfReferPoints;
}

BOOL CMSPrescanUtility::GetReferPosition(ULONG lIndex, LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY)
{
	if( lIndex>=GetNumOfReferPoints() )
		return FALSE;

	lRow = m_dwaReferRow.GetAt(lIndex);
	lCol = m_dwaReferCol.GetAt(lIndex);
	lEncX = m_dwaReferWfX.GetAt(lIndex);
	lEncY = m_dwaReferWfY.GetAt(lIndex);
	return TRUE;
}

VOID CMSPrescanUtility::SetPickDefectDie(CONST BOOL bPickDefect)
{
	m_bPickDefectDie = bPickDefect;
}

BOOL CMSPrescanUtility::GetPickDefectDie()
{
	return m_bPickDefectDie;
}

VOID CMSPrescanUtility::SetSpcReferGrade(LONG lGrade)
{
	m_lSpecailReferDieGrade = lGrade;
}

LONG CMSPrescanUtility::GetSpcReferGrade()
{
	return m_lSpecailReferDieGrade;
}

VOID CMSPrescanUtility::SetNoDieGrade(LONG lGrade)
{
	m_lNoDieGrade = lGrade;
}

LONG CMSPrescanUtility::GetNoDieGrade()
{
	return m_lNoDieGrade;
}

VOID CMSPrescanUtility::SetAlignDieRoation(LONG lRotation)
{
	m_lAlignDieRotation = lRotation;
}

LONG CMSPrescanUtility::GetAlignDieRotation()
{
	return m_lAlignDieRotation;
}
/*
VOID CMSPrescanUtility::SetSearchDieRoation(LONG lRotation)
{
	m_lSearchDieRotation = lRotation;
}

LONG CMSPrescanUtility::GetSearchDieRotation()
{
	return m_lSearchDieRotation;
}
*/
VOID CMSPrescanUtility::InitPrescanVariables()
{
//	m_lSearchDieRotation	= 0;
	m_lAlignDieRotation	= 0;

	m_bPrescanRegionMode	= FALSE;
	m_bDummyMapScan			= FALSE;		// dummy map created by ctrl s/w
	m_bPrescanAoiWafer		= FALSE;
	m_bScanAreaPickMode		= FALSE;
	m_bBarWaferScan			= FALSE;

	m_lAlignWftPosnX	= 0;
	m_lAlignWftPosnY	= 0;
	m_lAlignMapPosnX	= 0;
	m_lAlignMapPosnY	= 0;
	m_ulAlignPrFrameID	= 0;
	m_ulSoraaAlign2PrFrameID	= 0;
	m_lPrescanRealignMapRow = 0;
	m_lPrescanRealignMapCol = 0;
	m_lPrescanRealignWftPnX = 0;
	m_lPrescanRealignWftPnY = 0;
	m_bPickDefectDie	= FALSE;
	m_lSpecailReferDieGrade	= 0;
	m_lNoDieGrade		= 17;
	m_bMarkDieUnpick	= FALSE;
	RemoveAllReferPoints();
	DelAssistPoints();
	DelRescanBasePoints();
	DelReferMapPoints();
	m_szPrescanLogPath = PRESCAN_RESULT_FULL_PATH;
	m_szPsmMapMcNo.Empty();
	m_lAlignGlobalTheta	= 0;
	m_lReAlignGlobalTheta	= 0;
	m_bPrAbnormalError	= FALSE;
	m_bBlk2DebugLog		= FALSE;
	m_bAutoCycleLog		= FALSE;
	m_bVisionCtrlLog	= FALSE;
	m_bVisionThrdLog	= FALSE;
	m_bPrescanDebug		= FALSE;
	m_bAoiStopSending	= FALSE;
	m_bAoiEndSending	= FALSE;
	m_bPrescanEnable	= FALSE;
	m_bPickandPlaceMode	= FALSE;
	m_bRegionPickMode	= FALSE;
}

VOID CMSPrescanUtility::UpdateReRefPosition(UINT unIndex, LONG lRow, LONG lCol, LONG lEncX, LONG lEncY)
{
	if (m_dwaPrescanReRefRow.GetSize() <= unIndex)
	{
		m_dwaPrescanReRefRow.SetSize(unIndex + 1);
	}
	if (m_dwaPrescanReRefCol.GetSize() <= unIndex)
	{
		m_dwaPrescanReRefCol.SetSize(unIndex + 1);
	}
	if (m_dwaPrescanReRefWfX.GetSize() <= unIndex)
	{
		m_dwaPrescanReRefWfX.SetSize(unIndex + 1);
	}
	if (m_dwaPrescanReRefWfY.GetSize() <= unIndex)
	{
		m_dwaPrescanReRefWfY.SetSize(unIndex + 1);
	}

	m_dwaPrescanReRefRow.SetAt(unIndex, lRow);
	m_dwaPrescanReRefCol.SetAt(unIndex, lCol);
	m_dwaPrescanReRefWfX.SetAt(unIndex, lEncX);
	m_dwaPrescanReRefWfY.SetAt(unIndex, lEncY);
}

BOOL CMSPrescanUtility::GetReRefPosition(ULONG lIndex, LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY)
{
	if (lIndex >= GetNumOfReferPoints())
	{
		return FALSE;
	}

	if (lIndex < m_dwaPrescanReRefRow.GetSize())
	{
		lRow = m_dwaPrescanReRefRow.GetAt(lIndex);
	}
	else
	{
		lRow = 0;
	}

	if (lIndex < m_dwaPrescanReRefCol.GetSize())
	{
		lCol = m_dwaPrescanReRefCol.GetAt(lIndex);
	}
	else
	{
		lCol = 0;
	}

	if (lIndex < m_dwaPrescanReRefWfX.GetSize())
	{
		lEncX= m_dwaPrescanReRefWfX.GetAt(lIndex);
	}
	else
	{
		lEncX = 0;
	}

	if (lIndex < m_dwaPrescanReRefWfY.GetSize())
	{
		lEncY = m_dwaPrescanReRefWfY.GetAt(lIndex);
	}
	else
	{
		lEncY = 0;
	}

	return TRUE;
}

VOID CMSPrescanUtility::SetPrescanLogPath(CString szLogPath)
{
	m_szPrescanLogPath = szLogPath;
}

VOID CMSPrescanUtility::GetPrescanLogPath(CString &szLogPath)
{
	szLogPath.Empty();
	if( !m_szPrescanLogPath.IsEmpty() )
		szLogPath = m_szPrescanLogPath;
}

VOID CMSPrescanUtility::SetPrescanGlobalTheta(LONG lTheta)
{
	m_lAlignGlobalTheta = lTheta;
}

LONG CMSPrescanUtility::GetPrescanGlobalTheta()
{
	return m_lAlignGlobalTheta;
}

VOID CMSPrescanUtility::SetRealignGlobalTheta(LONG lTheta)
{
	m_lReAlignGlobalTheta = lTheta;
}

LONG CMSPrescanUtility::GetRealignGlobalTheta()
{
	return m_lReAlignGlobalTheta;
}

VOID CMSPrescanUtility::SetPrescanDebug(CONST BOOL bEnable)
{
	m_bPrescanDebug = bEnable;
}

BOOL CMSPrescanUtility::GetPrescanDebug()
{
	return GetPrescanEnable() && m_bPrescanDebug;
}

VOID CMSPrescanUtility::SetPrAbnormal(CONST BOOL bError, CString szLogMsg)
{
	if( m_bPrAbnormalError==FALSE && bError )
	{
		CSingleLock slLock(&m_CsRegionOrderLog);
		slLock.Lock();
		FILE	*pfPrError;
		CTime theTime = CTime::GetCurrentTime();

		CString szLogFile = gszUSER_DIRECTORY + "\\History\\PrScanError.log";
		errno_t nErr = fopen_s(&pfPrError, szLogFile, "a+");
		if ((nErr == 0) && (pfPrError != NULL))
		{
			CString szTime = theTime.Format("%Y%m%d%H%M%S");
			fprintf(pfPrError, "%s - %s\n", (LPCTSTR) szTime, (LPCTSTR) szLogMsg);  
			fclose(pfPrError);
		}
		slLock.Unlock();
	}	//	log this when happens first time in scanning cycle

	m_bPrAbnormalError = bError;
}

VOID CMSPrescanUtility::SetBlk2DebugLog(CONST BOOL bEnable)
{
	m_bBlk2DebugLog = bEnable;
}

VOID CMSPrescanUtility::SetAutoCycleDebugLog(CONST BOOL bEnable)
{
	m_bAutoCycleLog = bEnable;
}

VOID CMSPrescanUtility::SetVisionCtrlLog(CONST BOOL bEnable)
{
	m_bVisionCtrlLog = bEnable;
}

VOID CMSPrescanUtility::SetVisionThrdLog(CONST BOOL bEnable)
{
	m_bVisionThrdLog = bEnable;
}

BOOL CMSPrescanUtility::GetPrAbnormal()
{
	return m_bPrAbnormalError;
}

BOOL CMSPrescanUtility::GetBlk2DebugLog()
{
	return m_bBlk2DebugLog;
}

BOOL CMSPrescanUtility::GetAutoCycleLog()
{
	return m_bAutoCycleLog;
}

BOOL CMSPrescanUtility::GetVisionCtrlLog()
{
	return m_bVisionCtrlLog;
}

BOOL CMSPrescanUtility::GetVisionThrdLog()
{
	return m_bVisionThrdLog;
}

VOID CMSPrescanUtility::SetAoiStopSending(CONST BOOL bYes)
{
	m_bAoiStopSending = bYes;
}

BOOL CMSPrescanUtility::GetAoiStopSending()
{
	return m_bAoiStopSending;
}

VOID CMSPrescanUtility::SetAoiEndSending(CONST BOOL bYes)
{
	m_bAoiEndSending = bYes;
}

BOOL CMSPrescanUtility::GetAoiEndSending()
{
	return m_bAoiEndSending;
}

VOID CMSPrescanUtility::SetMarkDieUnpick(CONST BOOL bUnpick)
{
	m_bMarkDieUnpick = bUnpick;
}

BOOL CMSPrescanUtility::GetMarkDieUnpick()
{
	return m_bMarkDieUnpick;
}

VOID CMSPrescanUtility::SetPsmMapMcNo(CString szPsmMcNo)
{
	m_szPsmMapMcNo = szPsmMcNo;
}

CString CMSPrescanUtility::GetPsmMapMachineNo()
{
	return m_szPsmMapMcNo;
}

VOID  CMSPrescanUtility::DelAssistPoints()
{
	m_dwaAssistRow.RemoveAll();
	m_dwaAssistCol.RemoveAll();
	m_dwaAssistWfX.RemoveAll();
	m_dwaAssistWfY.RemoveAll();

	m_unAssistPointsNum = 0;
}

VOID  CMSPrescanUtility::AddAssistPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY)
{
	m_dwaAssistRow.Add(lRow);
	m_dwaAssistCol.Add(lCol);
	m_dwaAssistWfX.Add(lEncX);
	m_dwaAssistWfY.Add(lEncY);

	m_unAssistPointsNum++;
}

VOID  CMSPrescanUtility::TrimAssistPoints(ULONG ulNum)
{
	ULONG ulIndex = 0, ulStart, ulEnd;
	ulStart = GetAssistPointsNum() - 1;
	ulEnd = ulNum - 1;
	for(ulIndex=ulStart; ulIndex>ulEnd; ulIndex--)
	{
		m_dwaAssistRow.RemoveAt(ulIndex);
		m_dwaAssistCol.RemoveAt(ulIndex);
		m_dwaAssistWfX.RemoveAt(ulIndex);
		m_dwaAssistWfY.RemoveAt(ulIndex);
		m_unAssistPointsNum--;
	}
}

BOOL  CMSPrescanUtility::GetAssistPosition(ULONG lIndex, LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY)
{
	if( lIndex>=GetAssistPointsNum() )
		return FALSE;

	lRow = m_dwaAssistRow.GetAt(lIndex);
	lCol = m_dwaAssistCol.GetAt(lIndex);
	lEncX = m_dwaAssistWfX.GetAt(lIndex);
	lEncY = m_dwaAssistWfY.GetAt(lIndex);
	return TRUE;
}

UINT  CMSPrescanUtility::GetAssistPointsNum()
{
	return m_unAssistPointsNum;
}


VOID  CMSPrescanUtility::DelRescanBasePoints()
{
	m_dwaRescanBaseRow.RemoveAll();
	m_dwaRescanBaseCol.RemoveAll();
	m_dwaRescanBaseWfX.RemoveAll();
	m_dwaRescanBaseWfY.RemoveAll();

	m_unRescanBaseNum = 0;
}

VOID  CMSPrescanUtility::CutRescanBasePoint(ULONG ulIndex)
{
	if( ulIndex>=GetRescanBasePointsNum() )
	{
		return ;
	}

	m_dwaRescanBaseRow.RemoveAt(ulIndex);
	m_dwaRescanBaseCol.RemoveAt(ulIndex);
	m_dwaRescanBaseWfX.RemoveAt(ulIndex);
	m_dwaRescanBaseWfY.RemoveAt(ulIndex);
	m_unRescanBaseNum--;
}

VOID  CMSPrescanUtility::AddRescanBasePoint(LONG lRow, LONG lCol, LONG lWfX, LONG lWfY)
{
	m_dwaRescanBaseRow.Add(lRow);
	m_dwaRescanBaseCol.Add(lCol);
	m_dwaRescanBaseWfX.Add(lWfX);
	m_dwaRescanBaseWfY.Add(lWfY);

	m_unRescanBaseNum++;
}

BOOL  CMSPrescanUtility::GetRescanBasePoint(ULONG lIndex, ULONG &ulRow, ULONG &ulCol, LONG &lWfX, LONG &lWfY)
{
	if (lIndex>=GetRescanBasePointsNum())
	{
		return FALSE;
	}

	ulRow = m_dwaRescanBaseRow.GetAt(lIndex);
	ulCol = m_dwaRescanBaseCol.GetAt(lIndex);
	lWfX = m_dwaRescanBaseWfX.GetAt(lIndex);
	lWfY = m_dwaRescanBaseWfY.GetAt(lIndex);
	return TRUE;
}

UINT  CMSPrescanUtility::GetRescanBasePointsNum()
{
	return m_unRescanBaseNum;
}

VOID CMSPrescanUtility::DelReferMapPoints()
{
	m_dwaReferMapRow.RemoveAll();
	m_dwaReferMapCol.RemoveAll();
	m_dwaReferMapWfX.RemoveAll();
	m_dwaReferMapWfY.RemoveAll();

	m_unReferMapNum = 0;
}

VOID CMSPrescanUtility::AddReferMapPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY)
{
	m_dwaReferMapRow.Add(lRow);
	m_dwaReferMapCol.Add(lCol);
	m_dwaReferMapWfX.Add(lEncX);
	m_dwaReferMapWfY.Add(lEncY);

	m_unReferMapNum++;
}

BOOL  CMSPrescanUtility::GetReferMapPosition(ULONG lIndex, LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY)
{
	if( lIndex>=GetReferMapPointsNum() )
		return FALSE;

	lRow = m_dwaReferMapRow.GetAt(lIndex);
	lCol = m_dwaReferMapCol.GetAt(lIndex);
	lEncX = m_dwaReferMapWfX.GetAt(lIndex);
	lEncY = m_dwaReferMapWfY.GetAt(lIndex);
	return TRUE;
}

UINT  CMSPrescanUtility::GetReferMapPointsNum()
{
	return m_unReferMapNum;
}

VOID CMSPrescanUtility::SetPickAndPlaceMode(CONST BOOL bEnable)
{
	m_bPickandPlaceMode	= bEnable;
}

VOID CMSPrescanUtility::SetPrescanEnable(CONST BOOL bEnable)
{
	m_bPrescanEnable = bEnable;
}

BOOL CMSPrescanUtility::GetPrescanEnable()
{
	if( m_bPickandPlaceMode )
		return FALSE;

	return m_bPrescanEnable;
}