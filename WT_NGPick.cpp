//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "MS896A.h"
#include "PrescanInfo.h"
#include "WT_Log.h"
#include "LogFileUtil.h"
#include "PrescanUtility.h"
#include "WT_RegionPredication.h"
//#include "Encryption.h"
#include "WL_Constant.h"

//#ifndef MS_DEBUG	//v4.47T7
//	#include "spswitch.h"
//	using namespace AsmSw;
//#endif

//v4.59A46
#ifdef MS50_64BIT

typedef struct
{
	UINT  uiRow;
	UINT  uiCol;
	LONG  lEncPreX;
	LONG  lEncPreY;
	LONG  lEncX;
	LONG  lEncY;
	INT   iStatus;
	INT   ErrCode;
}DIEINFO;

#endif



BOOL CWaferTable::NCP_Auto_ScanPickedDieResort()
{	//	must make sure die picked by BH already and BH at prepick position.
//	m_bReSampleAsError		= FALSE;	//	picked die scan begin
//	m_bToDoSampleOncePE		= FALSE;
//	m_bWaferAlignComplete	= FALSE;
//	(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 0;	//	picked die scan begin
//
//	CString szTgtSavePath, szOldSavePath;
//	CString szLogPath = m_szPrescanLogPath;
//	szOldSavePath = szLogPath + "_KeyDie.csv";
//	szTgtSavePath.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), "_KeyDie.csv");
//	RenameFile(szOldSavePath, szTgtSavePath);
//
//	szOldSavePath	= szLogPath + PRESCAN_MOVE_POSN;
//	szTgtSavePath.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), PRESCAN_MOVE_POSN);
//	RenameFile(szOldSavePath, szTgtSavePath);
//
//	// Move theta home
//	INT nPrePickCount = 0;
//	while (IsBHAtPrePick() != 1)
//	{
//		Sleep(10);
//		nPrePickCount++;
//		if (nPrePickCount >= 1000)
//			break;
//	}
//
//	X_Sync();
//	Y_Sync();
//	T_Sync();
//	SetEjectorVacuum(FALSE);
//	Sleep(100);
//	GetEncoderValue();
//
//	LONG lOrigX = GetCurrX();
//	LONG lOrigY = GetCurrY();
//	LONG lOrigT = GetCurrT();
//
//	WftMoveBondArmToSafe(TRUE);
//
//	T_MoveTo(GetGlobalT(), SFM_WAIT);
//	Sleep(50);
//	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
//	UINT unDelay = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Sample Begin Delay"), 0);
//	if( unDelay>0 )
//	{
//		Sleep(unDelay);
//	}
//
//	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
//	m_WaferMapWrapper.StopMap();
//	// should set all look forward result to none in case wrong result
//	CString szMsg = "";
//#define	MS_MAX_NG_RESORT	15000
//	UINT unNumOfNgDie = 0;
//	DIEINFO		paNgDiePick[MS_MAX_NG_RESORT];
//	INT			paNgDieDone[MS_MAX_NG_RESORT];
//	//for (int i=0; i<MS_MAX_NG_RESORT; i++)
//	//{
//	//	paNgDiePick[i].uiRow = 0;
//	//	paNgDiePick[i].uiCol = 0;
//	//	paNgDiePick[i].iStatus = MS_Empty;
//	//	paNgDieDone[i] = MS_Empty;
//	//}
//
//	ULONG ulMapValidMinRow = 0, ulMapValidMaxRow = 0;
//	ULONG ulMapValidMinCol = 0, ulMapValidMaxCol = 0;
//	GetMapValidSize(ulMapValidMaxRow, ulMapValidMinRow,	ulMapValidMaxCol, ulMapValidMinCol);
//
//	CUIntArray aulSortGradeList;
//	m_WaferMapWrapper.GetSelectedGradeList(aulSortGradeList);
//	LONG lUserRow = 0, lUserCol = 0;
//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("");
//	for(ULONG ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
//	{
//		for(ULONG ulCol=ulMapValidMinCol; ulCol<=ulMapValidMaxCol; ulCol++)
//		{
//			LONG lPhyX = 0, lPhyY = 0;
//			if( GetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY)==FALSE )
//			{
//				continue;
//			}
//			UCHAR ucGrade = m_WaferMapWrapper.GetOriginalDieInformation(ulRow, ulCol);
//			for(INT i=0; i<aulSortGradeList.GetSize(); i++)
//			{
//				if( ucGrade==aulSortGradeList.GetAt(i) )
//				{
//					paNgDiePick[unNumOfNgDie].uiRow = ulRow;
//					paNgDiePick[unNumOfNgDie].uiCol = ulCol;
//					paNgDiePick[unNumOfNgDie].iStatus = MS_Align;
//					paNgDiePick[unNumOfNgDie].lEncX = lPhyX;
//					paNgDiePick[unNumOfNgDie].lEncY = lPhyY;
//
//					paNgDieDone[unNumOfNgDie] = MS_Align;
//					unNumOfNgDie++;
//
//					ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
//					szMsg.Format("%4d,%4d,%4d,%6d,%6d,%2d,%8ld,%8ld", 
//						i+1, ulRow, ulCol, lUserRow, lUserCol, MS_Align, lPhyX, lPhyY);
//					CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
//					break;
//				}
//			}
//			if( unNumOfNgDie>=MS_MAX_NG_RESORT )
//			{
//				break;
//			}
//		}
//		if( unNumOfNgDie>=MS_MAX_NG_RESORT )
//		{
//			break;
//		}
//	}
//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("");
//
//
//	DelScanKeyDie();
//	ClearGoodInfo();
//	MultiSrchInitNmlDie1(FALSE);
//
//	int nConvID = 0;
//	IPC_CServiceMessage stMsg;
//	BOOL bToScan = TRUE;
//	stMsg.InitMessage(sizeof(BOOL), &bToScan);
//	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ToggleScanSortZoom", stMsg);
//	while(1)
//	{
//		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
//		{
//			m_comClient.ReadReplyForConvID(nConvID, stMsg);
//			break;
//		}
//		else
//		{
//			Sleep(10);
//		}
//	}
//
//	CDWordArray	aScanRow, aScanCol;
//	aScanRow.RemoveAll();
//	aScanCol.RemoveAll();
//	LONG lStepCol = GetMapIndexStepCol();
//	LONG lStepRow = GetMapIndexStepRow();
//	INT nGrabImageLoops = 1;
//	for(UINT iKey=0; iKey<unNumOfNgDie; iKey++)
//	{
//		if( paNgDieDone[iKey] != MS_Align )
//		{
//			continue;
//		}
//
//		UINT unTgtRow = paNgDiePick[iKey].uiRow;
//		UINT unTgtCol = paNgDiePick[iKey].uiCol;
//
//		CDWordArray	aFovListRow, aFovListCol, aFovListIndex;
//		aFovListRow.RemoveAll();		aFovListCol.RemoveAll();		aFovListIndex.RemoveAll();
//		aFovListRow.Add(unTgtRow);		aFovListCol.Add(unTgtCol);		aFovListIndex.Add(iKey);
//		UINT unMaxRow = unTgtRow;
//		UINT unMaxCol = unTgtCol;
//		UINT unMinRow = unTgtRow;
//		UINT unMinCol = unTgtCol;
//		// loop find die within FOV at Up/Left//Right/Down
//		for(UINT jLoop=iKey+1; jLoop<unNumOfNgDie; jLoop++)
//		{
//			if( paNgDieDone[jLoop] != MS_Align )
//				continue;
//			UINT unCheckRow = paNgDiePick[jLoop].uiRow;
//			UINT unCheckCol = paNgDiePick[jLoop].uiCol;
//			if( labs(unCheckRow-unTgtRow)<lStepRow &&
//				labs(unCheckCol-unTgtCol)<lStepCol )
//			{
//				aFovListRow.Add(unCheckRow);		aFovListCol.Add(unCheckCol);		aFovListIndex.Add(jLoop);
//				unMaxRow = max(unMaxRow, unCheckRow);
//				unMinRow = min(unMinRow, unCheckRow);
//				unMaxCol = max(unMaxCol, unCheckCol);
//				unMinCol = min(unMinCol, unCheckCol);
//			}
//		}
//
//		if( (unMaxRow-unMinRow)<lStepRow )
//		{
//			// left at least, find most right but in FOV
//			if( (unMaxCol-unMinCol)>=lStepCol )
//			{
//				unMaxCol = unTgtCol;
//				for(UINT unLoop=0; unLoop<aFovListIndex.GetSize(); unLoop++)
//				{
//					if( (aFovListCol.GetAt(unLoop)-unMinCol)<lStepCol )
//					{
//						unMaxCol = max(unMaxCol, aFovListCol.GetAt(unLoop));
//					}
//				}
//			}
//		}
//		else
//		{
//			// top at least, find the most down but in FOV
//			if( (unMaxCol-unMinCol)<lStepCol )
//			{
//				unMaxRow = unTgtRow;
//				for(UINT unLoop=0; unLoop<aFovListIndex.GetSize(); unLoop++)
//				{
//					if( (aFovListRow.GetAt(unLoop)-unMinRow)<lStepRow )
//					{
//						unMaxRow = max(unMaxRow, aFovListRow.GetAt(unLoop));
//					}
//				}
//			}
//			else
//			{
//				// find target die Up and Left most die
//				unMaxRow = unMinRow = unTgtRow;
//				unMaxCol = unMinCol = unTgtCol;
//				for(UINT unLoop=0; unLoop<aFovListIndex.GetSize(); unLoop++)
//				{
//					if( aFovListRow.GetAt(unLoop)<unTgtRow && aFovListCol.GetAt(unLoop)<unTgtCol )
//					{
//						unMinRow = min(unMinRow, aFovListRow.GetAt(unLoop));
//						unMinCol = min(unMinCol, aFovListCol.GetAt(unLoop));
//					}
//				}
//				// use target Up and Left key die to find most down and right down within FOV
//				for(UINT unLoop=0; unLoop<aFovListIndex.GetSize(); unLoop++)
//				{
//					if( (aFovListRow.GetAt(unLoop)-unMinRow)<lStepRow && (aFovListCol.GetAt(unLoop)-unMinCol)<lStepCol )
//					{
//						unMaxRow = max(unMaxRow, aFovListRow.GetAt(unLoop));
//						unMaxCol = max(unMaxCol, aFovListCol.GetAt(unLoop));
//					}
//				}
//			}
//		}
//
//		CString szTemp, szTgtMsg;
//		szMsg.Format("scan die tot,%4d, UL(%4d,%4d), min(%4d,%4d), max(%4d,%4d) in same FOV",
//					aFovListIndex.GetSize(), unTgtRow, unTgtCol, unMinRow, unMinCol, unMaxRow, unMaxCol);
//		for(UINT unLoop=0; unLoop<aFovListIndex.GetSize(); unLoop++)
//		{
//			UINT unCheckRow = aFovListRow.GetAt(unLoop);
//			UINT unCheckCol = aFovListCol.GetAt(unLoop);
//			szTemp.Format(" (%4d,%d)", unCheckRow, unCheckCol);
//			szMsg += szTemp;
//		}
//		CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
//
//		LONG lScanWftX = paNgDiePick[iKey].lEncX;
//		LONG lScanWftY = paNgDiePick[iKey].lEncY;
//
//		szTgtMsg.Format("%4d,%4d, at %8ld,%8ld,", unTgtRow, unTgtCol, lScanWftX, lScanWftY);
//
//		LONG lMoveMinX = lScanWftX, lMoveMaxX = lScanWftX, lMoveMinY = lScanWftY, lMoveMaxY = lScanWftY;
//		INT lFovDieCounter = 1;
//		for(UINT unLoop=0; unLoop<aFovListIndex.GetSize(); unLoop++)
//		{
//			UINT unCheckRow = aFovListRow.GetAt(unLoop);
//			UINT unCheckCol = aFovListCol.GetAt(unLoop);
//			if( unCheckRow>=unMinRow && unCheckRow<=unMaxRow &&
//				unCheckCol>=unMinCol && unCheckCol<=unMaxCol )
//			{
//				UINT unCheckPoint = aFovListIndex.GetAt(unLoop);
//				paNgDieDone[unCheckPoint] = MS_Empty;
//				LONG lChkWfX = lScanWftX, lChkWfY = lScanWftY;
//				szTemp.Format("scan die key,%4d, ", unCheckPoint+1);
//				if( unCheckRow==unTgtRow && unCheckCol==unTgtCol )
//				{
//					szMsg = szTemp + szTgtMsg;
//				}
//				else
//				{
//					lChkWfX = paNgDiePick[unCheckPoint].lEncX;
//					lChkWfY = paNgDiePick[unCheckPoint].lEncY;
//					szMsg.Format("%4d,%4d, at %8ld,%8ld",
//						unCheckRow, unCheckCol, lChkWfX, lChkWfY);
//					szMsg = szTemp + szMsg;
//
//					if( (IsWithinWaferLimit(lChkWfX, lChkWfY) == FALSE) )
//					{
//						paNgDiePick[unCheckPoint].iStatus = MS_Empty;
//						szTemp = " get out of position or RETURN false";
//						szMsg += szTemp;
//						CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
//						continue;
//					}
//					LONG lWftX = 0, lWftY = 0;
//					if( GetPrescanWftPosn(unCheckRow, unCheckCol, lWftX, lWftY)==FALSE )
//					{
//						paNgDiePick[unCheckPoint].iStatus = MS_Empty;
//						szTemp = " no physical die in table";
//						szMsg += szTemp;
//						CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
//						continue;
//					}
//				}
//				CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
//
//				SetScanKeyDie(lFovDieCounter, nGrabImageLoops, lChkWfX, lChkWfY);
//				lFovDieCounter++;
//				lMoveMinX = min(lMoveMinX, lChkWfX);
//				lMoveMaxX = max(lMoveMaxX, lChkWfX);
//				lMoveMinY = min(lMoveMinY, lChkWfY);
//				lMoveMaxY = max(lMoveMaxY, lChkWfY);
//			}
//		}
//
//		lScanWftX = (lMoveMinX+lMoveMaxX)/2;
//		lScanWftY = (lMoveMinY+lMoveMaxY)/2;
//		SetScanKeyDie(0, nGrabImageLoops, lScanWftX, lScanWftY);
//		aScanRow.Add(unTgtRow);
//		aScanCol.Add(unTgtCol);
//		szMsg.Format("scan die tgt,%4d, at,%8ld,%8ld, grab,%4d\n", iKey+1, lScanWftX, lScanWftY, nGrabImageLoops);
//		CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
//		nGrabImageLoops++;
//	}
//
//	m_lGetSetLogic = 2;	//	1, move table, 2, all done
//	for(INT nLoop=1; nLoop<nGrabImageLoops; nLoop++)
//	{
//		LONG lTimer = 0;
//		while( 1 )
//		{
//			if( m_lGetSetLogic>=1 )
//			{
//				break;
//			}
//			Sleep(10);
//			lTimer++;
//			if( lTimer>1000 )
//			{
//				break;
//			}
//		}
//
//		INT nGet min(nLoop-1, aScanRow.GetSize()-1);
//		UINT unTgtRow = aScanRow.GetAt(nGet);
//		UINT unTgtCol = aScanCol.GetAt(nGet);
//		m_WaferMapWrapper.SetCurrentPosition(unTgtRow, unTgtCol);
//		LONG lGrabX = 0, lGrabY = 0;
//		if( GetScanKeyDie(0, nLoop, lGrabX, lGrabY)==false )
//		{
//			continue;
//		}
//		szMsg.Format("Scan Sample move %d at %ld,%ld", nLoop, lGrabX, lGrabY);
//		pUtl->PrescanMoveLog(szMsg);
//		XY_SafeMoveTo(lGrabX, lGrabY);
//		Sleep(10);
//
//		lTimer = 0;
//		while( 1 )
//		{
//			if( m_lGetSetLogic>=2 )
//			{
//				break;
//			}
//			Sleep(10);
//			lTimer++;
//			if( lTimer>1000 )
//			{
//				break;
//			}
//		}
//		m_lGetSetLogic = 0;
//
//		szMsg.Format("Scan Sample send %d", nLoop);
//		pUtl->PrescanMoveLog(szMsg);
//		stMsg.InitMessage(sizeof(INT), &nLoop);
//		m_comClient.SendRequest(WAFER_PR_STN, "ScanSampleKeyDice", stMsg);
//	}
//
//	LONG	lTimer = 0;
//	while( 1 )
//	{
//		if( m_lGetSetLogic>=2 )
//		{
//			break;
//		}
//		Sleep(10);
//		lTimer++;
//		if( lTimer>1000 )
//		{
//			break;
//		}
//	}
//
//	bToScan = FALSE;
//	stMsg.InitMessage(sizeof(BOOL), &bToScan);
//	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ToggleScanSortZoom", stMsg);
//	while(1)
//	{
//		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
//		{
//			m_comClient.ReadReplyForConvID(nConvID, stMsg);
//			break;
//		}
//		else
//		{
//			Sleep(10);
//		}
//	}
//
//	szMsg.Format("find die loop in total %d", GetGoodTotalDie());
//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
//	LONG lDiePitchX_X	= GetDiePitchX_X();
//	LONG lDiePitchY_Y	= GetDiePitchY_Y();
//	for(UINT iKey=0; iKey<unNumOfNgDie; iKey++)
//	{
//		if( paNgDiePick[iKey].iStatus != MS_Align )
//		{
//			continue;
//		}
//
//		BOOL bLoopFind = FALSE;
//		LONG lOldX = paNgDiePick[iKey].lEncX;
//		LONG lOldY = paNgDiePick[iKey].lEncY;
//		LONG lFindX = lOldX, lFindY = lOldY;
//		LONG lMinDist = GetDiePitchX_X()*GetDiePitchX_X()*2 + GetDiePitchY_Y()*GetDiePitchY_Y()*2;
//		for(ULONG ulLoop=0; ulLoop<GetGoodTotalDie(); ulLoop++)
//		{
//			LONG lPosX, lPosY;
//			DOUBLE dDieAngle = 0;
//			if( GetGoodPosnOnly(ulLoop, lPosX, lPosY, dDieAngle) )
//			{
//				LONG lOffsetX = lPosX - lOldX;
//				LONG lOffsetY = lPosY - lOldY;
//				BOOL bXCheck = labs(lOffsetX)<=labs(lDiePitchX_X*40/100);
//				BOOL bYCheck = labs(lOffsetY)<=labs(lDiePitchY_Y*40/100);
//				if( bXCheck==FALSE || bYCheck==FALSE )
//				{
//					continue;
//				}
//			//	LONG lDist = lOffsetX*lOffsetX + lOffsetY*lOffsetY;
//			//	if( lMinDist>lDist )
//			//	{
//			//		lMinDist = lDist;
//					lFindX = lPosX;
//					lFindY = lPosY;
//					bLoopFind = TRUE;
//			//	}
//			}
//		}
//
//		paNgDiePick[iKey].lEncX = lFindX;
//		paNgDiePick[iKey].lEncY = lFindY;
//		ULONG ulInRow = paNgDiePick[iKey].uiRow;
//		ULONG ulInCol = paNgDiePick[iKey].uiCol;
//		if( bLoopFind==FALSE )
//		{
//			paNgDiePick[iKey].iStatus = MS_Empty;
//		}
//		else
//		{
//			paNgDiePick[iKey].iStatus = MS_Align;
//			m_WaferMapWrapper.ClearDieStatus(ulInRow, ulInCol);
//			m_WaferMapWrapper.ChangeGrade(ulInRow, ulInCol, aulSortGradeList[0]);
//			SetMapPhyPosn(ulInRow, ulInCol, lFindX, lFindY);
//		}
//		LONG lUserRow = 0, lUserCol = 0;
//		ConvertAsmToOrgUser(ulInRow, ulInCol, lUserRow, lUserCol);
//		szMsg.Format("find die key,%4d, map,%4d,%d,(%6d,%6d), old (%8d,%8d), new (%8d,%8d),%2d", 
//			iKey+1, ulInRow, ulInCol, lUserRow, lUserCol, lOldX, lOldY, lFindX, lFindY, paNgDiePick[iKey].iStatus);
//		CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
//	}
//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("");
//
//	DelScanKeyDie();
//	ClearGoodInfo();
//
//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("Call set update key die position");
//	UCHAR aNgPickGrades[50];
//	UCHAR ucNgTotal = 0;
//	for(int i=0; i<aulSortGradeList.GetSize(); i++)
//	{
//		aNgPickGrades[i] = aulSortGradeList.GetAt(i);
//		ucNgTotal++;
//		if( ucNgTotal>=50 )
//		{
//			break;
//		}
//	}
//
//	m_WaferMapWrapper.SetAlgorithmParameter("PickGrade", (LONG)aulSortGradeList[0]);
//	m_WaferMapWrapper.SelectGrade(aNgPickGrades, ucNgTotal);
//	m_WaferMapWrapper.ResumeAlgorithmPreparation(TRUE);
//	m_WaferMapWrapper.StartMap();
//
//	T_MoveTo(lOrigT, SFM_NOWAIT);
//	XY_SafeMoveTo(lOrigX, lOrigY);
//	T_Sync();
//	Sleep(50);
//	if( unDelay>0 )
//	{
//		Sleep(unDelay);
//	}
//	if( m_bStop==FALSE )
//	{
//		WftMoveBondArmToSafe(FALSE);
//	}
//
//	m_bWaferAlignComplete	= TRUE;
//	m_bFirstInAutoCycle = TRUE;			//	re-scan NG picked die done

	return TRUE;
}	//	end to scan sample key die

