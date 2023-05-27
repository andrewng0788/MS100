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
#include "Utility.h"
//#include "Encryption.h"

//#ifndef MS_DEBUG	//v4.47T7
//	#include "spswitch.h"
//	using namespace AsmSw;
//#endif
static long gslThisMachineHasRptFile = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferTable::DEB_CheckEnableFeature(BOOL bSaved)		//v4.50A25
{
	if( bSaved==FALSE )
	{
		m_lPredictMethod = 0;
		if (DEB_IsEnable())
		{
			m_lPredictMethod = 1;
		}
		if (IsAdvRegionOffset())
		{
			m_lPredictMethod = 2;
		}

		if (IsAutoRescanEnable())
		{
			m_lPredictMethod = 3;
		}

		if (IsSamplingRescanEnable())
		{
			m_lPredictMethod = 5;
		}
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="Semitek" )
	{
		if( pApp->GetProductLine()!="ZJG" )
		{
			m_bScnCheckByPR			= IsEnableFPC();
			m_bScnCheckIsRefDie		= IsEnableFPC();
		}
		if( m_lPredictMethod==0 )
			m_lPredictMethod = 2;	// AdV
	}

	if( IsAOIOnlyMachine() )
	{
		m_lPredictMethod = 0;
	}

	CString szVersion = m_szDEB_LibVersion;
	if (szVersion != "3.46")
	{
		if (CMS896AStn::m_bEnableDEBSort != TRUE)
		{
			if( (m_lPredictMethod==1) )
				m_lPredictMethod = 0;
			if( (m_lPredictMethod==4) )
				m_lPredictMethod = 3;
		}
	}

	if( m_lPredictMethod >= 6)
	{
		m_lPredictMethod = 0;
	}
	m_bEnableAdaptPredict	= (m_lPredictMethod==1 || m_lPredictMethod==4);
	m_bAdvRegionOffset		= (m_lPredictMethod==2);
	m_bAutoRescanWafer		= (m_lPredictMethod == 3 || m_lPredictMethod == 4 || m_lPredictMethod == 5);

	if( !IsAutoRescanEnable() && !DEB_IsEnable() )
		m_bAutoSampleAfterPR = FALSE;
	if( !IsAutoSampleSort() )
		m_bAutoSampleAfterPE = FALSE;
	return TRUE;
}

VOID CWaferTable::DEB_EnableLog()
{
}

LONG CWaferTable::DEB_SampleKeyDie(IPC_CServiceMessage& svMsg)
{
	if( DEB_IsUseable() )
	{
		DEB_AutoRealignKeyDie();	// button press on HMI
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}


LONG CWaferTable::DEB_SetLogEnable(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable;

	svMsg.GetMsg(sizeof(bEnable), &bEnable);

	BOOL bReturn=TRUE;
	if( DEB_IsEnable() )
	{
		if( CheckOperatorPassWord(PRESCAN_PASSWORD_SUPER_USER) )
			m_bEnableAtcLog = bEnable;
		else
			m_bEnableAtcLog = !bEnable;
		if( m_bEnableAtcLog )
		{
			HmiMessage("Enable DEB log");
		}
		else
		{
			HmiMessage("Disable DEB Log, all DEB log files would be removed");
		}
	}
	else
	{
		bReturn=TRUE;
		m_bEnableAtcLog = FALSE;
		HmiMessage("DEB not enable, please ENABLE ADAPTIVE PREDICTION");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

INT CWaferTable::DEB_UpdateAllDicePos()
{
//	ULONG ulNumRow = 0, ulNumCol = 0;
//	ULONG ulRow, ulCol;
//	LONG lPhyX = 0, lPhyY = 0;
//
//	m_bDEB_INIT_OK	= FALSE;
//	if( !DEB_IsEnable() )
//	{
//		return TRUE;
//	}
//
//	m_bDEB_INIT_OK	= TRUE;
//	m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);
//
//	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
//	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
//	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
//
//	remove(m_szPrescanLogPath+PRESCAN_MSLOG_INIT);
//	remove(m_szPrescanLogPath+PRESCAN_MSLOG_KEYC);
//	remove(m_szPrescanLogPath+PRESCAN_MSLOG_XY);
//	BOOL bPrescan = FALSE;
//	if( GetNewPickCount()==0 )
//	{
//		bPrescan = TRUE;
//	}
//
//	if( bPrescan )
//	{
//		m_oPstModel.SetLogEnable(m_bEnableAtcLog);
//		m_oPstModel.SetLogFolder(gszUSER_DIRECTORY + "\\PrescanResult");
//		m_oPstModel.SetMapfileName(GetMapNameOnly());
//#ifdef	RESCAN_DEB_MIX
//		m_oPstnModel.ResetEncModel();
//#endif
//	}
//
//	m_oPstModel.InitEncModel();
//
//	if( bPrescan )	// rescan+deb XXX
//	{
//		m_stTop.lY = ulNumRow+2;
//		m_stBtm.lY = -1;
//		m_stLeft.lX = ulNumCol+2;
//		m_stRight.lX = -1;
//	}
//
//	BOOL bTopDone = FALSE, bBtmDone = FALSE, bLeftDone = FALSE, bRightDone = FALSE;
//	for(ulRow=0; ulRow<=ulNumRow; ulRow++)
//	{
//		for(ulCol=0; ulCol<=ulNumCol; ulCol++)
//		{
//			if (IsMapNullBin(ulRow, ulCol))
//				continue;
//
//			if( pUtl->GetPrescanRegionMode() )
//			{
//				if( pSRInfo->IsWithinThisRegion(ulTgtRegion, ulRow, ulCol)==FALSE )
//					continue;
//			}
//			if( IsOutMS90SortingPart(ulRow, ulCol) )
//			{
//				continue;
//			}
//
//			if (GetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY))
//			{
//				if( bPrescan )
//				{
//					if( m_stTop.lY>(LONG)ulRow )
//					{
//						m_stTop.lY = ulRow;
//						m_stTop.lX = ulCol;
//						m_stTop.lX_Enc = lPhyX;
//						m_stTop.lY_Enc = lPhyY;
//						m_stTop.ulStatus = 1;
//					}
//					if( m_stBtm.lY<(LONG)ulRow )
//					{
//						m_stBtm.lY = ulRow;
//						m_stBtm.lX = ulCol;
//						m_stBtm.lX_Enc = lPhyX;
//						m_stBtm.lY_Enc = lPhyY;
//						m_stBtm.ulStatus = 1;
//					}
//					if( m_stLeft.lX>(LONG)ulCol )
//					{
//						m_stLeft.lY = ulRow;
//						m_stLeft.lX = ulCol;
//						m_stLeft.lX_Enc = lPhyX;
//						m_stLeft.lY_Enc = lPhyY;
//						m_stLeft.ulStatus = 1;
//					}
//					if( m_stRight.lX<(LONG)ulCol )
//					{
//						m_stRight.lY = ulRow;
//						m_stRight.lX = ulCol;
//						m_stRight.lX_Enc = lPhyX;
//						m_stRight.lY_Enc = lPhyY;
//						m_stRight.ulStatus = 1;
//					}
//				}
//				DEB_UpdateDiePos(ulRow, ulCol, lPhyX, lPhyY, MS_Scan);
//				if( m_stTop.ulStatus==1 && m_stTop.lY==ulRow && bTopDone==FALSE )
//				{
//					bTopDone = TRUE;
//					m_stTop.lX = ulCol;
//					m_stTop.lX_Enc = lPhyX;
//					m_stTop.lY_Enc = lPhyY;
//				}
//				if( m_stBtm.ulStatus==1 && m_stBtm.lY==ulRow && bBtmDone==FALSE )
//				{
//					bBtmDone = TRUE;
//					m_stBtm.lX = ulCol;
//					m_stBtm.lX_Enc = lPhyX;
//					m_stBtm.lY_Enc = lPhyY;
//				}
//				if( m_stLeft.ulStatus==1 && m_stLeft.lX==ulCol && bLeftDone==FALSE )
//				{
//					bLeftDone = FALSE;
//					m_stLeft.lY = ulRow;
//					m_stLeft.lX_Enc = lPhyX;
//					m_stLeft.lY_Enc = lPhyY;
//				}
//				if( m_stRight.ulStatus==1 && m_stRight.lX==ulCol && bRightDone==FALSE )
//				{
//					bRightDone = FALSE;
//					m_stRight.lY = ulRow;
//					m_stRight.lX_Enc = lPhyX;
//					m_stRight.lY_Enc = lPhyY;
//				}
//			}
//		}
//	}
//
//#ifndef	RESCAN_DEB_MIX
//	CString szLog;
//	if( bTopDone==FALSE )
//	{
//		ulRow = m_stTop.lY;
//		ulCol = m_stTop.lX;
//		lPhyX = m_stTop.lX_Enc;
//		lPhyY = m_stTop.lY_Enc;
//		DEB_UpdateDiePos(ulRow, ulCol, lPhyX, lPhyY, MS_Scan);
//		szLog.Format("Top map %lu,%lu, wft %ld,%ld", 
//			ulRow, ulCol, lPhyX, lPhyY);
//		SaveScanTimeEvent(szLog);
//	}
//	if( bBtmDone==FALSE )
//	{
//		ulRow = m_stBtm.lY;
//		ulCol = m_stBtm.lX;
//		lPhyX = m_stBtm.lX_Enc;
//		lPhyY = m_stBtm.lY_Enc;
//		DEB_UpdateDiePos(ulRow, ulCol, lPhyX, lPhyY, MS_Scan);
//		szLog.Format("Btm map %lu,%lu, wft %ld,%ld", 
//			ulRow, ulCol, lPhyX, lPhyY);
//		SaveScanTimeEvent(szLog);
//	}
//	if( bLeftDone==FALSE )
//	{
//		ulRow = m_stLeft.lY;
//		ulCol = m_stLeft.lX;
//		lPhyX = m_stLeft.lX_Enc;
//		lPhyY = m_stLeft.lY_Enc;
//		DEB_UpdateDiePos(ulRow, ulCol, lPhyX, lPhyY, MS_Scan);
//		szLog.Format("Left map %lu,%lu, wft %ld,%ld", 
//			ulRow, ulCol, lPhyX, lPhyY);
//		SaveScanTimeEvent(szLog);
//	}
//	if( bRightDone==FALSE )
//	{
//		ulRow = m_stRight.lY;
//		ulCol = m_stRight.lX;
//		lPhyX = m_stRight.lX_Enc;
//		lPhyY = m_stRight.lY_Enc;
//		DEB_UpdateDiePos(ulRow, ulCol, lPhyX, lPhyY, MS_Scan);
//		szLog.Format("Right map %lu,%lu, wft %ld,%ld", 
//			ulRow, ulCol, lPhyX, lPhyY);
//		SaveScanTimeEvent(szLog);
//	}
//#endif

	return TRUE;
}


BOOL CWaferTable::DEB_CheckKeyDicesOnWafer()
{
	//if (!DEB_IsEnable())
	//{
	//	return TRUE;
	//}

	//CString szLog;

	//LONG lRow, lCol;
	//LONG lPhyX, lPhyY;

	//UINT i;
	//UINT unNumOfKeyDie = 0;
	//DIEINFO		paDebKeyDie[MS_MaxKeyNum];
	//for (i=0; i<MS_MaxKeyNum; i++)
	//{
	//	paDebKeyDie[i].uiRow = 0;
	//	paDebKeyDie[i].uiCol = 0;
	//}
	//
	//if( m_oPstModel.GetAllKeyDie(unNumOfKeyDie, paDebKeyDie)==FALSE )
	//{
	//	szLog = "Get Key Die return FALSE";
	//	HmiMessage_Red_Back(szLog, "Prescan");
	//	SetErrorMessage(szLog);
	//	SaveScanTimeEvent(szLog);
	//	m_bDEB_INIT_OK = FALSE;
	//	return FALSE;
	//}

	//if( unNumOfKeyDie==0 )
	//{
	//	szLog = "Num Of Key Die is 0";
	//	HmiMessage_Red_Back(szLog, "Prescan");
	//	SetErrorMessage(szLog);
	//	SaveScanTimeEvent(szLog);
	//	m_bDEB_INIT_OK = FALSE;
	//	return FALSE;
	//}

	//for(i=0; i<unNumOfKeyDie; i++)
	//{
	//	lRow = paDebKeyDie[i].uiRow;
	//	lCol = paDebKeyDie[i].uiCol;

	//	if (!GetMapPhyPosn(lRow, lCol, lPhyX, lPhyY))
	//	{
	//		paDebKeyDie[i].iStatus = MS_Empty;
	//		continue;
	//	}

	//	paDebKeyDie[i].iStatus = MS_Align;

	//	paDebKeyDie[i].lEncX = lPhyX;
	//	paDebKeyDie[i].lEncY = lPhyY;
	//	CString szMsg;
	//	LONG lUserRow = 0, lUserCol = 0;
	//	ConvertAsmToOrgUser(lRow, lCol, lUserRow, lUserCol);
	//	szMsg.Format("prescan key die,%4d,%4d,%4d,%4d,%4d,%2d,%8d,%8d", 
	//		i+1, lRow, lCol, lUserRow, lUserCol, MS_Align, lPhyX, lPhyY);
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//}

	//if( m_oPstModel.SetKeyDiePosition(unNumOfKeyDie, paDebKeyDie)==FALSE )
	//{
	//	szLog = "Set Key Die Position Failure";
	//	SetErrorMessage(szLog);
	//	SaveScanTimeEvent(szLog);
	//	m_bDEB_INIT_OK = FALSE;
	//	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//	if( pApp->GetCustomerName()=="SanAn" && IsPrescanEnable() )
	//	{ 
	//		return TRUE;
	//	}

	//	HmiMessage_Red_Back(szLog);
	//	return FALSE;
	//}

	//if( GetNewPickCount()==0 )
	//{
	//	m_unNumOfKeyDie = unNumOfKeyDie;
	//	memcpy(m_paDebKeyDie, paDebKeyDie, sizeof(paDebKeyDie));
	//}
	//else
	//{
	//	if( m_unNumOfKeyDie!=unNumOfKeyDie )
	//	{
	//		m_bDEB_INIT_OK = FALSE;
	//		CString szMsg;
	//		szMsg.Format("    WFT: list key die lost,new %4d, old %4d",
	//			unNumOfKeyDie, m_unNumOfKeyDie);
	//		CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//		return FALSE;
	//	}
	//	else
	//	{
	//		BOOL bMismatch = FALSE;
	//		for(i=0; i<m_unNumOfKeyDie; i++)
	//		{
	//			BOOL bFind = FALSE;
	//			ULONG ulRow = m_paDebKeyDie[i].uiRow;
	//			ULONG ulCol = m_paDebKeyDie[i].uiCol;
	//			for(UINT j=0; j<unNumOfKeyDie; j++)
	//			{
	//				if( paDebKeyDie[j].uiRow==ulRow && paDebKeyDie[j].uiCol==ulCol )
	//				{
	//					bFind = TRUE;
	//					bMismatch = TRUE;
	//					break;
	//				}
	//			}
	//			if( bFind==FALSE && m_WaferMapWrapper.GetReader()!=NULL )
	//			{
	//				CString szMsg;
	//				LONG lUserRow = 0, lUserCol = 0;
	//				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
	//				szMsg.Format("    WFT: list key die lost,%4d,%4d,%4d,%4d,%d",
	//					ulRow, ulCol, lUserRow, lUserCol, m_WaferMapWrapper.GetGrade(ulRow, ulCol)-m_WaferMapWrapper.GetGradeOffset());
	//				CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//			}
	//		}
	//		if( bMismatch )
	//		{
	//			return FALSE;
	//		}
	//	}
	//}

	return TRUE;
}

//	begin move table to do sampling
BOOL CWaferTable::DEB_AutoRealignKeyDie_old()
{
	//if( DEB_IsEnable()==FALSE )
	//{
	//	return FALSE;
	//}
	//CString szLogPath = m_szPrescanLogPath;
	//CString szLogFileName = szLogPath + "_KeyDie.csv";
	//CString szBkuFileName;
	//szBkuFileName.Format("%s_KeyDie_%03d.csv", szLogPath, m_ulPdcRegionSampleCounter);
	//RenameFile(szLogFileName, szBkuFileName);

	//m_bWaferAlignComplete	= FALSE;

	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("DEB begin sample key die offset");
	//// Move theta home
	//INT nPrePickCount = 0;
	//while (IsBHAtPrePick() != 1)
	//{
	//	Sleep(10);
	//	nPrePickCount++;
	//	if (nPrePickCount >= 100)
	//		break;
	//}

	//X_Sync();
	//Y_Sync();
	//T_Sync();
	//SetEjectorVacuum(FALSE);
	//Sleep(100);
	//GetEncoderValue();

	//LONG lOrigX = GetCurrX();
	//LONG lOrigY = GetCurrY();
	//LONG lOrigT = GetCurrT();

	//WftMoveBondArmToSafe(TRUE);

	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//UINT unDelay = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Sample Begin Delay"), 0);
	//if( unDelay>0 )
	//	Sleep(unDelay);
	//T_MoveTo(GetGlobalT(), SFM_WAIT);

	//// should set all look forward result to none in case wrong result
	//CTime stUpdtTime;
	//stUpdtTime = CTime::GetCurrentTime();
	////Search RefDie

	////Search RefDie
	//DIEINFO		paDebKeyDieOld[MS_MaxKeyNum];
	//UINT		unNumOfKeyDieOld = 0;
	//UINT uiRowExt = 3, uiColExt = 3;
	//for (int i=0; i<MS_MaxKeyNum; i++)
	//{
	//	paDebKeyDieOld[i].uiRow = 0;
	//	paDebKeyDieOld[i].uiCol = 0;
	//}
	//m_oPstModel.GetUpdatedKeyDie(unNumOfKeyDieOld, paDebKeyDieOld, uiRowExt, uiColExt);

	//CString szMsg;
	//szMsg.Format("DEB auto update key dice begin time(%d:%d:%d), ext %d,%d",
	//	stUpdtTime.GetHour(), stUpdtTime.GetMinute(), stUpdtTime.GetSecond(), uiRowExt, uiColExt);
	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//SetStatusMessage(szMsg);

	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("");
	//LONG lUserRow = 0, lUserCol = 0;
	//for(UINT i=0; i<unNumOfKeyDieOld; i++)
	//{
	//	ULONG ulInRow = paDebKeyDieOld[i].uiRow;
	//	ULONG ulInCol = paDebKeyDieOld[i].uiCol;
	//	ConvertAsmToOrgUser(ulInRow, ulInCol, lUserRow, lUserCol);
	//	szMsg.Format("%3d,%3d,%3d,%4d,%4d,%8d,%8d,%d", 
	//		i+1, ulInRow, ulInCol, lUserRow, lUserCol, paDebKeyDieOld[i].lEncPreX, paDebKeyDieOld[i].lEncPreY, paDebKeyDieOld[i].iStatus);
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//}
	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("");

	//for(UINT i=0; i<unNumOfKeyDieOld; i++)
	//{
	//	if( paDebKeyDieOld[i].iStatus != MS_Align )
	//	{
	//		continue;
	//	}
	//	ULONG ulTgtRow = paDebKeyDieOld[i].uiRow;
	//	ULONG ulTgtCol = paDebKeyDieOld[i].uiCol;

	//	LONG lWfX = 0, lWfY = 0;
	//	CString szTemp;
	//	ConvertAsmToOrgUser(ulTgtRow, ulTgtCol, lUserRow, lUserCol);
	//	szMsg.Format("%3d,%3d,%3d,%4d,%4d,", 
	//		i+1, ulTgtRow, ulTgtCol, lUserRow, lUserCol);
	//	if( GetPrescanWftPosn(ulTgtRow, ulTgtCol, lWfX, lWfY)==FALSE )
	//	{
	//		szTemp = "DEB no physical die in table";
	//		szMsg += szTemp;
	//		CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//		paDebKeyDieOld[i].iStatus = MS_Empty;
	//		continue;
	//	}

	//	lWfX = paDebKeyDieOld[i].lEncPreX;
	//	lWfY = paDebKeyDieOld[i].lEncPreY;
	//	LONG lOldX = lWfX;
	//	LONG lOldY = lWfY;
	//	szTemp.Format("%8d,%8d,", lWfX, lWfY);
	//	szMsg += szTemp;
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//	if( (IsWithinWaferLimit(lWfX, lWfY) == FALSE) )
	//	{
	//		paDebKeyDieOld[i].iStatus = MS_Empty;
	//		szTemp = "DEB get out of position or return false";
	//		szMsg += szTemp;
	//		CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//		continue;
	//	}

	//	if( WftMoveSearchDie(lWfX, lWfY, FALSE)==FALSE )
	//	{
	//		szTemp = "PR search die is empty";
	//		szMsg += szTemp;
	//		CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//		paDebKeyDieOld[i].iStatus = MS_Empty;
	//		continue;
	//	}

	//	LONG lOffsetX = lWfX - lOldX;
	//	LONG lOffsetY = lWfY - lOldY;
	//	BOOL bXCheck = CheckPitchX(lOffsetX);
	//	BOOL bYCheck = CheckPitchY(lOffsetY);
	//	BOOL bXUp = CheckPitchUpX(lOffsetX);
	//	BOOL bYUp = CheckPitchUpY(lOffsetY);
	//	szTemp.Format("PR found %ld,%ld,", lWfX, lWfY);
	//	szMsg += szTemp;
	//	if( bXUp==FALSE || bYUp==FALSE )
	//	{
	//		szTemp = "Over up limit(49)";
	//		szMsg += szTemp;
	//		CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//		paDebKeyDieOld[i].iStatus = MS_Empty;
	//		continue;
	//	}

	//	if( bXCheck==FALSE || bYCheck==FALSE )
	//	{
	//		// maybe need to check the match status even die is found and within die pitch tolerance.
	//		szTemp = "Over Pitch Limit, go on";
	//		szMsg += szTemp;
	//		CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//	//	paDebKeyDieOld[i].iStatus = MS_Empty;
	//	//	continue;
	//	}

	//	paDebKeyDieOld[i].lEncX = lWfX;
	//	paDebKeyDieOld[i].lEncY = lWfY;
	//	paDebKeyDieOld[i].uiRow = ulTgtRow;
	//	paDebKeyDieOld[i].uiCol = ulTgtCol;
	//	paDebKeyDieOld[i].iStatus = MS_Align;

	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//}

	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("");
	//for(UINT i=0; i<unNumOfKeyDieOld; i++)
	//{
	//	ULONG ulInRow = paDebKeyDieOld[i].uiRow;
	//	ULONG ulInCol = paDebKeyDieOld[i].uiCol;
	//	ConvertAsmToOrgUser(ulInRow, ulInCol, lUserRow, lUserCol);
	//	szMsg.Format("%3d,%3d,%3d,%4d,%4d,%8d,%8d,%d", 
	//		i+1, ulInRow, ulInCol, lUserRow, lUserCol, paDebKeyDieOld[i].lEncX, paDebKeyDieOld[i].lEncY, paDebKeyDieOld[i].iStatus);
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//}

	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("Call set update key die position");
	//if( m_oPstModel.SetUpdatedKeyDiePosition(unNumOfKeyDieOld, paDebKeyDieOld)==FALSE )
	//{
	//	szMsg = "DEB update all false";
	//	HmiMessage_Red_Back("DEB update all key dice false", "DEB realign");
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//}


	//T_MoveTo(lOrigT, SFM_NOWAIT);
	//XY_SafeMoveTo(lOrigX, lOrigY);
	//T_Sync();
	//Sleep(50);

	//if( unDelay>0 )
	//{
	//	Sleep(unDelay);
	//}
	//if( m_bStop==FALSE )
	//{
	//	WftMoveBondArmToSafe(FALSE);
	//}

	//stUpdtTime = CTime::GetCurrentTime();
	//szMsg.Format("DEB auto update key dice end time(%d:%d:%d)",
	//	stUpdtTime.GetHour(), stUpdtTime.GetMinute(), stUpdtTime.GetSecond());
	//SetStatusMessage(szMsg);
	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);

	//m_bWaferAlignComplete	= TRUE;
	//m_bFirstInAutoCycle = TRUE;	//	deb sampling wafer done

	return TRUE;
}

BOOL CWaferTable::DEB_AutoRealignKeyDie()
{
	//if( DEB_IsEnable()==FALSE )
	//{
	//	return FALSE;
	//}
	//m_bReSampleAsError		= FALSE;	//	key die sample begin
	//(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 0;	//	key die sample begin
	//m_lRescanPECounter		= GetNewPickCount();	//	DEB done, remember last counter.
	//m_bToDoSampleOncePE		= FALSE;
	//if( m_bDebKeyDieSampleOld )
	//{
	//	return DEB_AutoRealignKeyDie_old();
	//}

	//CString szLogPath = m_szPrescanLogPath;
	//CString szLogFileName = szLogPath + "_KeyDie.csv";
	//CString szBkuFileName;
	//szBkuFileName.Format("%s_KeyDie_%03d.csv", szLogPath, m_ulPdcRegionSampleCounter);
	//RenameFile(szLogFileName, szBkuFileName);

	//CString szTgtSavePath, szOldSavePath;
	//szOldSavePath	= szLogPath + PRESCAN_MOVE_POSN;
	//szTgtSavePath.Format("%s_%07lu%s", szLogPath, GetNewPickCount(), PRESCAN_MOVE_POSN);
	//RenameFile(szOldSavePath, szTgtSavePath);

	//m_bWaferAlignComplete	= FALSE;

	//// Move theta home
	//INT nPrePickCount = 0;
	//while (IsBHAtPrePick() != 1)
	//{
	//	Sleep(10);
	//	nPrePickCount++;
	//	if (nPrePickCount >= 1000)
	//		break;
	//}

	//X_Sync();
	//Y_Sync();
	//T_Sync();
	//SetEjectorVacuum(FALSE);
	//Sleep(100);
	//GetEncoderValue();

	//LONG lOrigX = GetCurrX();
	//LONG lOrigY = GetCurrY();
	//LONG lOrigT = GetCurrT();

	//WftMoveBondArmToSafe(TRUE);

	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//UINT unDelay = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Sample Begin Delay"), 0);
	//if( unDelay>0 )
	//	Sleep(unDelay);
	//T_MoveTo(GetGlobalT(), SFM_WAIT);

	//// should set all look forward result to none in case wrong result
	//CTime stUpdtTime;
	//stUpdtTime = CTime::GetCurrentTime();
	//CString szMsg = "";

	//UINT uiRowExt = 3, uiColExt = 3;
	//UINT unNumOfKeyDieR = 0;
	//DIEINFO		paDebKeyDieR[MS_MaxKeyNum];
	//DIEINFO		paKeyDieList[MS_MaxKeyNum];
	//for (int i=0; i<MS_MaxKeyNum; i++)
	//{
	//	paDebKeyDieR[i].uiRow = 0;
	//	paDebKeyDieR[i].uiCol = 0;
	//	paKeyDieList[i].uiRow = 0;
	//	paKeyDieList[i].uiCol = 0;
	//	paKeyDieList[i].iStatus = MS_Empty;
	//}

	//m_oPstModel.GetUpdatedKeyDie(unNumOfKeyDieR, paDebKeyDieR, uiRowExt, uiColExt);

	//LONG lStepCol = GetMapIndexStepCol();
	//LONG lStepRow = GetMapIndexStepRow();
	//szMsg.Format("DEB auto scan update key dice begin time(%d:%d:%d), ext %d,%d, scan step %ld,%ld",
	//	stUpdtTime.GetHour(), stUpdtTime.GetMinute(), stUpdtTime.GetSecond(), uiRowExt, uiColExt, lStepRow, lStepCol);
	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//SetStatusMessage(szMsg);

	//LONG lUserRow = 0, lUserCol = 0;
	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("");
	//for(UINT i=0; i<unNumOfKeyDieR; i++)
	//{
	//	if( paDebKeyDieR[i].iStatus!=MS_Align )
	//	{
	//		continue;
	//	}

	//	UINT ulInRow = paDebKeyDieR[i].uiRow;
	//	UINT ulInCol = paDebKeyDieR[i].uiCol;
	//	INT  nStatus = paDebKeyDieR[i].iStatus;
	//	ConvertAsmToOrgUser(ulInRow, ulInCol, lUserRow, lUserCol);
	//	szMsg.Format("%4d,%4d,%4d,%6d,%6d,%2d,%8ld,%8ld", 
	//		i+1, ulInRow, ulInCol, lUserRow, lUserCol, nStatus, paDebKeyDieR[i].lEncPreX, paDebKeyDieR[i].lEncPreY);
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//	paKeyDieList[i].uiRow	= ulInRow;
	//	paKeyDieList[i].uiCol	= ulInCol;
	//	paKeyDieList[i].iStatus = nStatus;
	//}
	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("");

	//DelScanKeyDie();
	//ClearGoodInfo();
	//INT nGrabImageLoop = 1;
	//MultiSrchInitNmlDie1(FALSE);

	//IPC_CServiceMessage stMsg;
	//BOOL bToScan = TRUE;
	//stMsg.InitMessage(sizeof(BOOL), &bToScan);
	//int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ToggleScanSortZoom", stMsg);
	//while(1)
	//{
	//	if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
	//	{
	//		m_comClient.ReadReplyForConvID(nConvID, stMsg);
	//		break;
	//	}
	//	else
	//	{
	//		Sleep(10);
	//	}
	//}

	//CDWordArray	aScanRow, aScanCol;
	//aScanRow.RemoveAll();
	//aScanCol.RemoveAll();

	//for(UINT iKey=0; iKey<unNumOfKeyDieR; iKey++)
	//{
	//	if( paKeyDieList[iKey].iStatus != MS_Align )
	//	{
	//		continue;
	//	}

	//	UINT unTgtRow = paDebKeyDieR[iKey].uiRow;
	//	UINT unTgtCol = paDebKeyDieR[iKey].uiCol;

	//	CDWordArray	aListRow, aListCol, aListUse;
	//	aListRow.RemoveAll();		aListCol.RemoveAll();		aListUse.RemoveAll();
	//	aListRow.Add(unTgtRow);		aListCol.Add(unTgtCol);		aListUse.Add(iKey);
	//	UINT unMaxRow = unTgtRow;
	//	UINT unMaxCol = unTgtCol;
	//	UINT unMinRow = unTgtRow;
	//	UINT unMinCol = unTgtCol;
	//	// loop find die within FOV at Up/Left//Right/Down
	//	for(UINT jLoop=iKey+1; jLoop<unNumOfKeyDieR; jLoop++)
	//	{
	//		if( paKeyDieList[jLoop].iStatus != MS_Align )
	//			continue;
	//		UINT unCheckRow = paKeyDieList[jLoop].uiRow;
	//		UINT unCheckCol = paKeyDieList[jLoop].uiCol;
	//		if( labs(unCheckRow-unTgtRow)<lStepRow &&
	//			labs(unCheckCol-unTgtCol)<lStepCol )
	//		{
	//			aListRow.Add(unCheckRow);		aListCol.Add(unCheckCol);		aListUse.Add(jLoop);
	//			unMaxRow = max(unMaxRow, unCheckRow);
	//			unMinRow = min(unMinRow, unCheckRow);
	//			unMaxCol = max(unMaxCol, unCheckCol);
	//			unMinCol = min(unMinCol, unCheckCol);
	//		}
	//	}

	//	if( (unMaxRow-unMinRow)<lStepRow )
	//	{
	//		// left at least, find most right but in FOV
	//		if( (unMaxCol-unMinCol)>=lStepCol )
	//		{
	//			unMaxCol = unTgtCol;
	//			for(UINT unLoop=0; unLoop<aListUse.GetSize(); unLoop++)
	//			{
	//				if( (aListCol.GetAt(unLoop)-unMinCol)<lStepCol )
	//				{
	//					unMaxCol = max(unMaxCol, aListCol.GetAt(unLoop));
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		// top at least, find the most down but in FOV
	//		if( (unMaxCol-unMinCol)<lStepCol )
	//		{
	//			unMaxRow = unTgtRow;
	//			for(UINT unLoop=0; unLoop<aListUse.GetSize(); unLoop++)
	//			{
	//				if( (aListRow.GetAt(unLoop)-unMinRow)<lStepRow )
	//				{
	//					unMaxRow = max(unMaxRow, aListRow.GetAt(unLoop));
	//				}
	//			}
	//		}
	//		else
	//		{
	//			// find target die Up and Left most die
	//			unMaxRow = unMinRow = unTgtRow;
	//			unMaxCol = unMinCol = unTgtCol;
	//			for(UINT unLoop=0; unLoop<aListUse.GetSize(); unLoop++)
	//			{
	//				if( aListRow.GetAt(unLoop)<unTgtRow && aListCol.GetAt(unLoop)<unTgtCol )
	//				{
	//					unMinRow = min(unMinRow, aListRow.GetAt(unLoop));
	//					unMinCol = min(unMinCol, aListCol.GetAt(unLoop));
	//				}
	//			}
	//			// use target Up and Left key die to find most down and right down within FOV
	//			for(UINT unLoop=0; unLoop<aListUse.GetSize(); unLoop++)
	//			{
	//				if( (aListRow.GetAt(unLoop)-unMinRow)<lStepRow && (aListCol.GetAt(unLoop)-unMinCol)<lStepCol )
	//				{
	//					unMaxRow = max(unMaxRow, aListRow.GetAt(unLoop));
	//					unMaxCol = max(unMaxCol, aListCol.GetAt(unLoop));
	//				}
	//			}
	//		}
	//	}

	//	CString szTemp, szTgtMsg;
	//	szMsg.Format("scan die tot,%4d, UL(%4d,%4d), min(%4d,%4d), max(%4d,%4d) in same FOV",
	//				aListUse.GetSize(), unTgtRow, unTgtCol, unMinRow, unMinCol, unMaxRow, unMaxCol);
	//	for(UINT unLoop=0; unLoop<aListUse.GetSize(); unLoop++)
	//	{
	//		UINT unCheckRow = aListRow.GetAt(unLoop);
	//		UINT unCheckCol = aListCol.GetAt(unLoop);
	//		szTemp.Format(" (%4d,%d)", unCheckRow, unCheckCol);
	//		szMsg += szTemp;
	//	}
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);

	//	LONG lTgtWfX = paDebKeyDieR[iKey].lEncPreX;
	//	LONG lTgtWfY = paDebKeyDieR[iKey].lEncPreY;

	//	szTgtMsg.Format("%4d,%4d, at %8ld,%8ld,", unTgtRow, unTgtCol, lTgtWfX, lTgtWfY);

	//	LONG lMoveMinX = lTgtWfX, lMoveMaxX = lTgtWfX, lMoveMinY = lTgtWfY, lMoveMaxY = lTgtWfY;
	//	INT lFoundCounter = 1;
	//	for(UINT unLoop=0; unLoop<aListUse.GetSize(); unLoop++)
	//	{
	//		UINT unCheckRow = aListRow.GetAt(unLoop);
	//		UINT unCheckCol = aListCol.GetAt(unLoop);
	//		if( unCheckRow>=unMinRow && unCheckRow<=unMaxRow &&
	//			unCheckCol>=unMinCol && unCheckCol<=unMaxCol )
	//		{
	//			UINT unCheckPoint = aListUse.GetAt(unLoop);
	//			paKeyDieList[unCheckPoint].iStatus = MS_Empty;
	//			LONG lChkWfX = lTgtWfX, lChkWfY = lTgtWfY;
	//			szTemp.Format("scan die key,%4d, ", unCheckPoint+1);
	//			if( unCheckRow==unTgtRow && unCheckCol==unTgtCol )
	//			{
	//				szMsg = szTemp + szTgtMsg;
	//			}
	//			else
	//			{
	//				lChkWfX = paDebKeyDieR[unCheckPoint].lEncPreX;
	//				lChkWfY = paDebKeyDieR[unCheckPoint].lEncPreY;
	//				szMsg.Format("%4d,%4d, at %8ld,%8ld",
	//					unCheckRow, unCheckCol, lChkWfX, lChkWfY);
	//				szMsg = szTemp + szMsg;

	//				if( (IsWithinWaferLimit(lChkWfX, lChkWfY) == FALSE) )
	//				{
	//					paDebKeyDieR[unCheckPoint].iStatus = MS_Empty;
	//					szTemp = " get out of position or return false";
	//					szMsg += szTemp;
	//					CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//					continue;
	//				}
	//				LONG lWftX = 0, lWftY = 0;
	//				if( GetPrescanWftPosn(unCheckRow, unCheckCol, lWftX, lWftY)==FALSE )
	//				{
	//					paDebKeyDieR[unCheckPoint].iStatus = MS_Empty;
	//					szTemp = " no physical die in table";
	//					szMsg += szTemp;
	//					CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//					continue;
	//				}
	//			}
	//			CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);

	//			SetScanKeyDie(lFoundCounter, nGrabImageLoop, lChkWfX, lChkWfY);
	//			lFoundCounter++;
	//			lMoveMinX = min(lMoveMinX, lChkWfX);
	//			lMoveMaxX = max(lMoveMaxX, lChkWfX);
	//			lMoveMinY = min(lMoveMinY, lChkWfY);
	//			lMoveMaxY = max(lMoveMaxY, lChkWfY);
	//		}
	//	}

	//	lTgtWfX = (lMoveMinX+lMoveMaxX)/2;
	//	lTgtWfY = (lMoveMinY+lMoveMaxY)/2;
	//	SetScanKeyDie(0, nGrabImageLoop, lTgtWfX, lTgtWfY);
	//	aScanRow.Add(unTgtRow);
	//	aScanCol.Add(unTgtCol);
	//	szMsg.Format("scan die tgt,%4d, at,%8ld,%8ld, grab,%4d\n", iKey+1, lTgtWfX, lTgtWfY, nGrabImageLoop);
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//	nGrabImageLoop++;
	//}

	//m_lGetSetLogic = 2;	//	1, move table, 2, all done
	//CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	//for(INT nLoop=1; nLoop<nGrabImageLoop; nLoop++)
	//{
	//	LONG lTimer = 0;
	//	while( 1 )
	//	{
	//		if( m_lGetSetLogic>=1 )
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

	//	INT nGet min(nLoop-1, aScanRow.GetSize()-1);
	//	UINT unTgtRow = aScanRow.GetAt(nGet);
	//	UINT unTgtCol = aScanCol.GetAt(nGet);
	//	m_WaferMapWrapper.SetCurrentPosition(unTgtRow, unTgtCol);
	//	LONG lGrabX = 0, lGrabY = 0;
	//	if( GetScanKeyDie(0, nLoop, lGrabX, lGrabY)==false )
	//	{
	//		continue;
	//	}
	//	szMsg.Format("Scan Sample move %d at %ld,%ld", nLoop, lGrabX, lGrabY);
	//	pUtl->PrescanMoveLog(szMsg);
	//	XY_SafeMoveTo(lGrabX, lGrabY);
	//	Sleep(10);

	//	lTimer = 0;
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
	//	m_lGetSetLogic = 0;

	//	szMsg.Format("Scan Sample send %d", nLoop);
	//	pUtl->PrescanMoveLog(szMsg);
	//	stMsg.InitMessage(sizeof(INT), &nLoop);
	//	m_comClient.SendRequest(WAFER_PR_STN, "ScanSampleKeyDice", stMsg);
	//}

	//LONG	lTimer = 0;
	//while( 1 )
	//{
	//	if( m_lGetSetLogic>=2 )
	//	{
	//		break;
	//	}
	//	Sleep(10);
	//	lTimer++;
	//	if( lTimer>1000 )
	//	{
	//		break;
	//	}
	//}

	//bToScan = FALSE;
	//stMsg.InitMessage(sizeof(BOOL), &bToScan);
	//nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ToggleScanSortZoom", stMsg);
	//while(1)
	//{
	//	if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
	//	{
	//		m_comClient.ReadReplyForConvID(nConvID, stMsg);
	//		break;
	//	}
	//	else
	//	{
	//		Sleep(10);
	//	}
	//}

	//szMsg.Format("find die loop in total %d", GetGoodTotalDie());
	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//LONG lDiePitchX_X	= GetDiePitchX_X();
	//LONG lDiePitchY_Y	= GetDiePitchY_Y();
	//for(UINT iKey=0; iKey<unNumOfKeyDieR; iKey++)
	//{
	//	if( paDebKeyDieR[iKey].iStatus != MS_Align )
	//	{
	//		continue;
	//	}

	//	BOOL bLoopFind = FALSE;
	//	LONG lOldX = paDebKeyDieR[iKey].lEncPreX;
	//	LONG lOldY = paDebKeyDieR[iKey].lEncPreY;
	//	LONG lFindX = lOldX, lFindY = lOldY;
	//	LONG lMinDist = GetDiePitchX_X()*GetDiePitchX_X()*2 + GetDiePitchY_Y()*GetDiePitchY_Y()*2;
	//	for(ULONG ulLoop=0; ulLoop<GetGoodTotalDie(); ulLoop++)
	//	{
	//		LONG lPosX, lPosY;
	//		DOUBLE dDieAngle = 0;
	//		if( GetGoodPosnOnly(ulLoop, lPosX, lPosY, dDieAngle) )
	//		{
	//			LONG lOffsetX = lPosX - lOldX;
	//			LONG lOffsetY = lPosY - lOldY;
	//			BOOL bXCheck = labs(lOffsetX)<=labs(lDiePitchX_X*40/100);
	//			BOOL bYCheck = labs(lOffsetY)<=labs(lDiePitchY_Y*40/100);
	//			if( bXCheck==FALSE || bYCheck==FALSE )
	//			{
	//				continue;
	//			}
	//		//	LONG lDist = lOffsetX*lOffsetX + lOffsetY*lOffsetY;
	//		//	if( lMinDist>lDist )
	//		//	{
	//		//		lMinDist = lDist;
	//				lFindX = lPosX;
	//				lFindY = lPosY;
	//				bLoopFind = TRUE;
	//		//	}
	//		}
	//	}

	//	paDebKeyDieR[iKey].lEncX = lFindX;
	//	paDebKeyDieR[iKey].lEncY = lFindY;
	//	if( bLoopFind==FALSE )
	//	{
	//		paDebKeyDieR[iKey].iStatus = MS_Empty;
	//	}
	//	else
	//	{
	//		paDebKeyDieR[iKey].iStatus = MS_Align;
	//	}
	//	ULONG ulInRow = paDebKeyDieR[iKey].uiRow;
	//	ULONG ulInCol = paDebKeyDieR[iKey].uiCol;
	//	LONG lUserRow = 0, lUserCol = 0;
	//	ConvertAsmToOrgUser(ulInRow, ulInCol, lUserRow, lUserCol);
	//	szMsg.Format("find die key,%4d, map,%4d,%d,(%6d,%6d), old (%8d,%8d), new (%8d,%8d),%2d", 
	//		iKey+1, ulInRow, ulInCol, lUserRow, lUserCol, lOldX, lOldY, lFindX, lFindY, paDebKeyDieR[iKey].iStatus);
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//}
	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("");

	//DelScanKeyDie();
	//ClearGoodInfo();

	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog("Call set update key die position");
	//if( m_oPstModel.SetUpdatedKeyDiePosition(unNumOfKeyDieR, paDebKeyDieR)==FALSE )
	//{
	//	szMsg = "DEB update all false";
	//	HmiMessage_Red_Back("DEB update all key dice false", "DEB realign");
	//	CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
	//}

	//T_MoveTo(lOrigT, SFM_NOWAIT);
	//XY_SafeMoveTo(lOrigX, lOrigY);
	//T_Sync();
	//Sleep(50);

	//if( unDelay>0 )
	//{
	//	Sleep(unDelay);
	//}
	//if( m_bStop==FALSE )
	//{
	//	WftMoveBondArmToSafe(FALSE);
	//}

	//stUpdtTime = CTime::GetCurrentTime();
	//szMsg.Format("DEB auto scan update key dice   end time(%d:%d:%d)",
	//	stUpdtTime.GetHour(), stUpdtTime.GetMinute(), stUpdtTime.GetSecond());
	//SetStatusMessage(szMsg);
	//CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);

	//m_bWaferAlignComplete	= TRUE;
	//m_bFirstInAutoCycle = TRUE;	//	deb sampling wafer done

	return TRUE;
}	//	end to scan sample key die


BOOL CWaferTable::GoCheckDebMapDie(ULONG ulRow, ULONG ulCol)
{
	BOOL bGoDone = FALSE;
	INT nDebStatus = 0;
	LONG lScanX = 0, lScanY = 0, encX = 0, encY = 0;
	CString szMsg;

	if (IsMapNullBin(ulRow, ulCol) ||
		m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol))
	{
		szMsg.Format("check asm map %d,%d null bin or reference die", ulRow, ulCol);
		SetAlarmLog(szMsg);
		return FALSE;
	}

	if( GetMapPhyPosn(ulRow, ulCol, lScanX, lScanY) &&
		DEB_GetDiePos(ulRow, ulCol, encX, encY, nDebStatus) )
	{
		szMsg.Format("check asm map %d,%d at wft %d,%d", ulRow, ulCol, encX, encY);
		SetAlarmLog(szMsg);
		if( XY_SafeMoveTo(encX, encY, FALSE) )
		{
			Sleep(100);
			//if( WftMoveSearchDie(encX, encY, FALSE) )
			//	nDebStatus = MS_Align;
			//else
			//	nDebStatus = MS_Empty;
			//DEB_UpdateDiePos(ulRow, ulCol, encX, encY, nDebStatus);
		}
		if( ( m_ulPitchAlarmGoCheckCount<m_ulPitchAlarmGoCheckLimit) )
		{
			BOOL bDoneBefore = FALSE;
			LONG lCheckTotal = min((LONG)m_aUIGoCheckRow.GetSize(), (LONG)m_aUIGoCheckCol.GetSize());
			for(LONG i=0; i<lCheckTotal; i++)
			{
				if( labs(ulRow-m_aUIGoCheckRow.GetAt(i))<=0 && 
					labs(ulCol-m_aUIGoCheckCol.GetAt(i))<=0 )
				{
					bDoneBefore = TRUE;
					break;
				}
			}

			if( bDoneBefore==FALSE )
			{
				m_aUIGoCheckRow.Add(ulRow);
				m_aUIGoCheckCol.Add(ulCol);
				m_ulPitchAlarmGoCheckCount++;
			}
		}	// handle the pitch go check points

		if( m_bEnableAlignWaferImageLog )		//v4.37T10
		{
			(*m_psmfSRam)["MS896A"]["WaferTableX"] = encX;
			(*m_psmfSRam)["MS896A"]["WaferTableY"] = encY;
			(*m_psmfSRam)["MS896A"]["WaferMapRow"] = ulRow;
			(*m_psmfSRam)["MS896A"]["WaferMapCol"] = ulCol;
			GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_UGO);	// user check scan result by go button
		}
		if( m_ulPitchAlarmGoCheckCount>=m_ulPitchAlarmGoCheckLimit )
		{
			bGoDone = TRUE;
		}
	}

	return bGoDone;
}

BOOL CWaferTable::AutoGoPitchErrorAround()
{
	if( m_szNextLocation.IsEmpty() || DEB_IsUseable()==FALSE || m_ulPitchAlarmGoCheckLimit<=0 )
	{
		m_ulPitchAlarmGoCheckCount = m_ulPitchAlarmGoCheckLimit;
		return TRUE;
	}

	ULONG ulNextRow = 0, ulNextCol = 0;

	CStringArray szDataList;
	CUtility::Instance()->ParseRawData(m_szNextLocation, szDataList);
	if( szDataList.GetSize()>=2 )
	{
		LONG lUserRow = atoi(szDataList.GetAt(0));
		LONG lUserCol = atoi(szDataList.GetAt(1));
		if( ConvertOrgUserToAsm(lUserRow, lUserCol, ulNextRow, ulNextCol) )
		{
			m_WaferMapWrapper.SetCurrentPosition(ulNextRow, ulNextCol);
			m_WaferMapWrapper.SetSelectedPosition(ulNextRow, ulNextCol);
		}
	}
	else
	{
		m_ulPitchAlarmGoCheckCount = m_ulPitchAlarmGoCheckLimit;
		return TRUE;
	}

	if (CMS896AApp::m_bMS100Plus9InchOption && IsEJTAtUnloadPosn())		//v4.47T12
	{
		SetAlert_Red_Yellow(IDS_WT_BL_NOT_IN_SAFE_POS);
		SetErrorMessage("AutoAlign: WT at UNLOAD position (MS109)");
		return FALSE;
	}

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	LONG encT = pUtl->GetRealignGlobalTheta();
	SetJoystickOn(FALSE);
	GetEncoderValue();
	if( abs(GetCurrT()-encT)>=5 )
		T_MoveTo(encT, SFM_WAIT);
	T_Sync();

	ULONG ulRow = 0, ulCol = 0;

	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	ulRow = ulNextRow;
	ulCol = ulNextCol;
	BOOL bGoDone = GoCheckDebMapDie(ulRow, ulCol);

	for (ulCurrentLoop = 1; ulCurrentLoop <= 5; ulCurrentLoop++)
	{
		if( bGoDone )
		{
			break;
		}
		lRow = (ulCurrentLoop * 2 + 1);
		lCol = (ulCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		ulRow += lDiff_X;
		ulCol += lDiff_Y;

		if (ulRow >= 0 && ulCol >= 0)
		{
			bGoDone = GoCheckDebMapDie(ulRow, ulCol);
			if( bGoDone )
			{
				break;
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			ulRow += lDiff_X;
			ulCol += lDiff_Y;

			if (ulRow >= 0 && ulCol >= 0)
			{
				bGoDone = GoCheckDebMapDie(ulRow, ulCol);
				if( bGoDone )
				{
					break;
				}
			}
		}

		if (bGoDone)
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			ulRow += lDiff_X;
			ulCol += lDiff_Y;

			if (ulRow >= 0 && ulCol >= 0)
			{
				bGoDone = GoCheckDebMapDie(ulRow, ulCol);
				if( bGoDone )
				{
					break;
				}
			}
		}

		if (bGoDone)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			ulRow += lDiff_X;
			ulCol += lDiff_Y;

			if (ulRow >= 0 && ulCol >= 0)
			{
				bGoDone = GoCheckDebMapDie(ulRow, ulCol);
				if( bGoDone )
				{
					break;
				}
			}
		}

		if (bGoDone)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			ulRow += lDiff_X;
			ulCol += lDiff_Y;

			if (ulRow >= 0 && ulCol >= 0)
			{
				bGoDone = GoCheckDebMapDie(ulRow, ulCol);
				if( bGoDone )
				{
					break;
				}
			}
		}

		if (bGoDone)
		{
			break;
		}
	}

	m_ulPitchAlarmGoCheckCount = m_ulPitchAlarmGoCheckLimit;
	return TRUE;
}


