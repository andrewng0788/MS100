/////////////////////////////////////////////////////////////////
// WPR_SubState.cpp : SubState for Auto-cycle of the CWaferPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "PrescanInfo.h"
#include "GallerySearchController.h"
#include "PRFailureCaseLog.h"
#include "PrZoomSensor.h"
#include "BondHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Sequence Operations
////////////////////////////////////////////
VOID CWaferPr::BackupBurnInStatus()
{
	if (IsBurnIn())
	{
		m_bSrchEnableLookForward_Backup = m_bSrchEnableLookForward;
		m_bSrchEnableLookForward = TRUE;

		m_ulMaxNoDieSkipCount_Backup = m_ulMaxNoDieSkipCount;
		m_ulMaxNoDieSkipCount = 0;
	}
}

VOID CWaferPr::RestoreBurnInStatus()
{
	if (IsBurnIn())
	{
		m_bSrchEnableLookForward = m_bSrchEnableLookForward_Backup;
		m_ulMaxNoDieSkipCount = m_ulMaxNoDieSkipCount_Backup;
	}
}

INT CWaferPr::OpInitialize()
{
	INT nResult					= gnOK;

	m_qPreviousSubOperation	= -1;
	m_qSubOperation			= 0;

	NeedReset(FALSE);

	DisplaySequence("WPR - Operation Initialize");
	return nResult;
}


BOOL CWaferPr::IsPrestartFinished()
{
	if ((m_qSubOperation == WAIT_WT_STABLE_Q) ||
		(m_qSubOperation == PRESCAN_WAIT_WT_STABLE_Q) ||
		(m_qSubOperation == HOUSE_KEEPING_Q))
	{
		return TRUE;
	}

	return FALSE;
}


INT CWaferPr::OpPreStart()
{
	INT nResult	= gnOK;

	SetPRLatched(FALSE);
	SetPRStart(FALSE);
	SetDieReady(FALSE);
	SetBadDie(FALSE);
	SetBadDieForT(FALSE);
	SetLFReady(FALSE);
	SetConfirmSrch(FALSE);
	SetPreCompensate(FALSE);	
	SetBhTReadyForWPR(FALSE);
	
	//For NuMotion Tuning Tools use
	MotionSetOutputBit(WPR_SO_SEARCH_DIE_CMD, FALSE);
	MotionSetOutputBit(WPR_SO_SEARCH_DIE_REPLY_1, FALSE);
	MotionSetOutputBit(WPR_SO_SEARCH_DIE_REPLY_2, FALSE);
	MotionSetOutputBit(WPR_SO_SEARCH_DIE_REPLY_3, FALSE);

	SaveScanTimeEvent("WPR: opprestart begin");
	OpPrescanInit(GetPrescanPrID());	// download when press start button
	IM_InitPrHighLevelInfo(TRUE);
	CalculateLFSearchArea();
	m_bOcrConfirmControl = FALSE;
	m_ucOcrBarMapStage = 0;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	//No Use
	if (IsBLInUse())
	{
		LONG lQueryDist = labs(m_lBackLightElevatorUpLevel - m_lBackLightElevatorStandByLevel);
		m_lBackLightTravelTime = CMS896AStn::MotionGetProfileTime(WAFERPR_BACKLIGHT_Z, WPR_MP_BACKLIGHT_Z_NORMAL, lQueryDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stBackLight_Z);
		CString szLog;
		szLog.Format("BLZ Travel up(%d) standby(%d) time %d", 
			m_lBackLightElevatorUpLevel, m_lBackLightElevatorStandByLevel, m_lBackLightTravelTime);
		pUtl->PrescanMoveLog(szLog);
	}
	//No Use

	m_bPrAoiSwitchCamera = FALSE;		//v3.93		//Used in MS100 AOI only
	if (!IsAOIOnlyMachine() || !m_bSetAutoBondScreen)
	{
		m_bSetAutoBondScreen = TRUE;
	}

	if (m_bAOINgPickPartDie)
	{
		m_bThetaCorrection = FALSE;
		m_bConfirmSearch = FALSE;
		m_bSrchEnableLookForward = FALSE;
	}

	TurnOffPSLightings();

	if (m_bSetAutoBondScreen)
	{
		AutoBondScreen(TRUE);
		m_bSetAutoBondScreen = FALSE;
	}

	m_lLetBPRGrabFirstImage	= FALSE;
	m_ulWaferMaxDefectCount	= 0;
	m_ulStartSkipCount		= 0;
	m_ulSkipCount			= 0;
	m_ulNoDieSkipCount		= 0;
	m_ulPRSkipCount			= 0;
	m_ulDefectSkipCount		= 0;
	m_ulBadCutSkipCount		= 0;
	m_bAutoLearnRefDieDone	= FALSE;		//v2.78T2 
	m_bIsWaferPrRPY1Done	= FALSE;		//v3.31		//IM PR logging only
	m_bRefCheckRst			= TRUE;
	m_bNeedTurnOffLighting	= TRUE;
	m_ulAutoSwitchPrCount	= GetNewPickCount();
	m_ulLogPRCaseInHQ		= 0;
	m_unHWGrabState			= 0;			//andrewng //2020-0616

	m_lMaxSortNmlDieID = 1;
	if (m_bCheckAllNmlDie)
	{
		if (IsThisDieLearnt(1))
		{
			if (IsPrescanEnable())
			{
				if(GetPrescanPrID() == 3)
				{
					m_lMaxSortNmlDieID = 2;
				}
				else
				{
					m_lMaxSortNmlDieID = 3;
				}
			}
			else
			{
				m_lMaxSortNmlDieID = 2;
				if( IsThisDieLearnt(2) )
				{
					m_lMaxSortNmlDieID = 3;
				}
			}
		}
	}

	//v3.70T2	//PLLM special feature
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && (nPLLM == PLLM_REBEL) && 		//xyz
		!IsBurnIn() && !m_bRefDieCheck)
	{
		m_bRefDieCheck = TRUE;
	}

	(*m_psmfSRam)["WaferPr"]["CFS"]["ConfirmSearch"] = m_bConfirmSearch;	//Let WT to check	//v2.96T3
	(*m_psmfSRam)["WaferPr"]["RefGradeCheck"]["Result"] = TRUE;
	(*m_psmfSRam)["WaferPr"]["RefDie"]["Check"] = m_bRefDieCheck;
	(*m_psmfSRam)["WaferPr"]["RefDie"]["Count"] = m_lLrnTotalRefDie;
	(*m_psmfSRam)["WaferPr"]["RPY1"] = GetTime();
	(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 0;
	(*m_psmfSRam)["WaferPr"]["NGPick"]["IsEmpty"] = TRUE;

	//Hard code die pitch if burnin mode AND (pitch not learnt OR die not learnt)
	if ( (IsBurnIn()) && ((m_bPitchLearnt == FALSE) || (IsNormalDieLearnt() == FALSE)) )
	{
		(*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"] = 1000;
		(*m_psmfSRam)["WaferPr"]["DiePitchX"]["Y"] = 0;
		(*m_psmfSRam)["WaferPr"]["DiePitchY"]["X"] = 0;
		(*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"] = 1000;
		(*m_psmfSRam)["WaferPr"]["RefDie Check"]	= FALSE;
	}
	else if (IsBurnIn() && !pApp->m_bBurnInGrabImage)	//andrewng //2020-0617
	{
		(*m_psmfSRam)["WaferPr"]["DiePitchX"]["X"] = 1000;
		(*m_psmfSRam)["WaferPr"]["DiePitchY"]["Y"] = 1000;
		(*m_psmfSRam)["WaferPr"]["DiePitchX"]["Y"] = 0;
		(*m_psmfSRam)["WaferPr"]["DiePitchY"]["X"] = 0;
	}

	CheckLookForward();


	//BLOCKPICK
	if( IsBlkFunc1Enable() )
	{
		//v3.30
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieIsAlign"]		= FALSE;
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieIsGood"]			= FALSE;
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieTNeedRotate"]	= FALSE;
	}

// prescan relative code	B
	SetAllPrescanDone(FALSE);

	if( IsPrescanning() )	//	426TX	1
	{
		m_bWprInAutoSortingMode	= FALSE;
		if( pUtl->GetPrescanAreaPickMode() && m_lAreaPickStage>=2 )
			m_bWprInAutoSortingMode = TRUE;
		WPR_GetEncoderValue();
		m_lAF_RuntimeLevel = m_lEnc_AF;

		if( IsEnableZoom() )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WPR - Prestart, prescan, switch to scn zoom mode");
			if (WPR_ToggleZoom(TRUE, TRUE, 16) == FALSE)
			{
				m_qSubOperation			= HOUSE_KEEPING_Q;
				DisplaySequence("WPR_ToggleZoom - pre-start ==> house keeping q");				
			}
			Sleep(20);
		}

		if( IsAutoRescanWafer() && m_lRescanRunStage==1 )	
		{	//	auto rescan sorting wafer	;	Wait WT stable to prescan
			SaveScanTimeEvent("WPR: prestart rescan into auto cycle");
			m_dwaRsnBaseRow.RemoveAll();
			m_dwaRsnBaseCol.RemoveAll();
			m_dwaRsnBaseWfX.RemoveAll();
			m_dwaRsnBaseWfY.RemoveAll();

			m_lRescanRunStage		= 2;	//	MS rescan
			WSClearScanRemainDieList();
		}	//	auto rescan sorting wafer	;	Wait WT stable to prescan

		m_qSubOperation			= PRESCAN_WAIT_WT_STABLE_Q;
	}
	else
	{
		m_bWprInAutoSortingMode = TRUE;
		m_qSubOperation			= WAIT_WT_STABLE_Q;
		DisplaySequence("WPR - Prescan sort ==> Wait WT Stable");
	}

	if( IsMotionHardwareReady()==FALSE )
	{
		m_qSubOperation			= HOUSE_KEEPING_Q;
		DisplaySequence("WPR - pre-start ==> house keeping q");
	}

	int nImageLimit = m_pPrescanPrCtrl->GetBufferImageLimit();
	CString szMsg;
	szMsg.Format("WPR: opprestart done Image Limit %d, buffer size %d", 
		nImageLimit, m_lPrescanImageLimit);
	SaveScanTimeEvent(szMsg);

	DisplaySequence("WPR - Operation Prestart");
	return nResult;
}

VOID CWaferPr::OpUpdateDieIndex()		// Update Die Index
{
	ULONG ulIndex = (*m_psmfSRam)["DieInfo"]["WPR_Index"];
	ulIndex++;
	(*m_psmfSRam)["DieInfo"]["WPR_Index"] = ulIndex;

	//CString szMsg;
	//szMsg.Format("WPR - Index = %d", ulIndex);
	//DisplayMessage(szMsg);
}

BOOL CWaferPr::OpIsMaxSkipCount()
{
	BOOL bReturn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG lMaxNoDieSkipCount = m_ulMaxNoDieSkipCount;
	ULONG ulRow = (ULONG)(LONG) (*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"];
	ULONG ulCol = (ULONG)(LONG) (*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"];
	if (IsNGBlock(ulRow, ulCol))
	{
		lMaxNoDieSkipCount = 200;//30;
	}

	if( m_dWaferGoodDieRatio>0.0 && m_ulWaferMaxDefectCount>0 && (m_ulWaferDefectDieCount>=m_ulWaferMaxDefectCount))
	{
		HmiMessage_Red_Yellow("Defect die accumulative count over wafer max limit!", "Auto Sort");
		bReturn = TRUE;
		SetErrorMessage("Too many accumulative defective die that over wafer max limit");
		(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 6;
	}

	if( m_ulMaxStartSkipLimit>0 && (m_ulStartSkipCount >= m_ulMaxStartSkipLimit) )
	{
		CString szMsg = "NG die start accumulative count over its limit!";
		HmiMessage_Red_Yellow(szMsg, "Auto Sort");
		bReturn = TRUE;
		SetErrorMessage(szMsg);
		(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 7;
	}

	if ( (m_ulMaxSkipCount > 0) && (m_ulSkipCount >= m_ulMaxSkipCount) )
	{
		SetAlert_Red_Yellow(IDS_WPR_MAX_SKIPCOUNT);
		bReturn = TRUE;
		SetErrorMessage("Too many skip die");
		(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 1;		//v3.76
	}
	else if ( (/*m_ulMaxNoDieSkipCount*/lMaxNoDieSkipCount > 0) && (m_ulNoDieSkipCount >= lMaxNoDieSkipCount/*m_ulMaxNoDieSkipCount*/) )
	{
		if( m_bAutoSampleAfterPR )
		{
			LONG lWprFatalErr = (*m_psmfSRam)["WaferPr"]["WprFatalErr"];
			if( lWprFatalErr!=-2 && lWprFatalErr!=-1 )		//	auto sample if PR too many no die
			{
				(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = -1;	//	resample again
				DisplaySequence("WPR - PR too many no die, trigger to auto sample at WFT");
			}
		}
		else
		{
			//CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
			//if (1/*pPRFailureCaseLog->IsEnableFailureCaselog()*/)//2019.04.08 Matt Hardcode for field study
			//{
			//	CloseAlarm();
			//	CString szMsg;
			//	szMsg = "Log PR Failure Case...";
			//	SetAlert_WarningMsg(IDS_MS_WARNING_LOG_PR_FAILURE_CASE, szMsg);
			//	pPRFailureCaseLog->LogFailureCaseDumpFile(GetRunSenID(), GetRunRecID(), 5);
			//	if (State() == IDLE_Q)
			//	{
			//		SetAlarmLamp_Yellow(FALSE, TRUE);
			//	}
			//	else
			//	{
			//		SetAlarmLamp_Green(FALSE, TRUE);
			//	}
			//	CloseAlarm();
			//}
			m_ulLogPRCaseInHQ = 1;
			SetAlert_SE_Red_Yellow(IDS_WPR_MAX_NODIE_SKIPCOUNT);
			bReturn = TRUE;
			SetErrorMessage("Too many no die (OpIsMaxSkipCount)");
			(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 2;		//v3.76
			DisplaySequence("Too many no die (OpIsMaxSkipCount)");
		}
	}
	else if ( (m_ulMaxPRSkipCount > 0) && (m_ulPRSkipCount >= m_ulMaxPRSkipCount) )
	{
		SetAlert_Red_Yellow(IDS_WPR_MAX_PR_SKIPCOUNT);
		bReturn = TRUE;
		SetErrorMessage("Too many no PR skip die");
		(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 3;		//v3.76
	}
	else if ( (m_ulMaxBadCutSkipCount > 0) && (m_ulBadCutSkipCount >= m_ulMaxBadCutSkipCount) )
	{
		SetAlert_Red_Yellow(IDS_WPR_MAX_BADCUT_SKIPCOUNT);
		bReturn = TRUE;
		SetErrorMessage("Too many no badcut skip die");
		(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 4;		//v3.76
	}
	else if( (m_ulMaxDefectSkipCount>0) && (m_ulDefectSkipCount>=m_ulMaxDefectSkipCount) )
	{
		HmiMessage_Red_Yellow("Defect die accumulative count over limit!", "Auto Sort");
		bReturn = TRUE;
		SetErrorMessage("Too many accumulative defective die");
		(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = 5;
	}
	else if( (m_bRefCheckRst == FALSE) && IsBlkFunc2Enable() )	//Block2
	{
		CString strTmp;
		BOOL bResetAlign = FALSE;

		if ( IsCharDieInUse() && (m_lLrnTotalRefDie > WPR_GEN_CDIE_OFFSET) )
		{
			LONG lMapDig1,lMapDig2;

			lMapDig1 = m_iMapDig1;
			lMapDig2 = m_iMapDig2;
			if (lMapDig1!=-1 && lMapDig2!=-1
				&& (lMapDig1 != m_iWafDig1 || lMapDig2 != m_iWafDig2) )
			{
				if ( (m_iWafDig1 == -2) && (m_iWafDig2 == -2) )
				{
					strTmp.Format("Check reference die failure. Map(%d%d) PRS(no die)",lMapDig1,lMapDig2);
				}
				else
				{
					strTmp.Format("Check reference die failure. Map(%d%d) PRS(%d%d)",lMapDig1,lMapDig2,m_iWafDig1,m_iWafDig2);
				}
				SetCtmLogMessage(strTmp);
				SetStatusMessage(strTmp);

				if( pApp->GetCustomerName()!="OSRAM" )
					bResetAlign = TRUE;
				if( bResetAlign )
					strTmp = strTmp + "\n\nPlease align wafer again.";
				else
					strTmp = strTmp + "\n\nPlease check OCR die manually.";
				HmiMessage_Red_Yellow(strTmp,"Check Reference");
			}
		}
		else
		{
			if( pApp->GetCustomerName()=="Cree" )
			{
				//Generate Log file when missing reference is detected during auto-bond (Cree)
				IPC_CServiceMessage rReqMsg;
				INT nConvID = 0;
				BOOL bResult = TRUE;
				nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ExportRefCheckFailFile", rReqMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
						rReqMsg.GetMsg(sizeof(BOOL), &bResult);
						bReturn = TRUE;
						break;
					}
					else
					{
						Sleep(10);
					}
				}
			}

			strTmp.Format("Check reference die failure.");
			SetCtmLogMessage(strTmp);
			SetStatusMessage(strTmp);
			SetErrorMessage(strTmp);

			if( pApp->GetCustomerName()!="Cree" || IsPrescanEnable()==FALSE )
			{
				strTmp = strTmp + "\n\nPlease align wafer again.";
				HmiMessage_Red_Yellow(strTmp,"Check Reference");
				bResetAlign = TRUE;
			}
		}

		if( bResetAlign )
		{
			//Reset Wafer Align Status
			IPC_CServiceMessage rReqMsg;
			INT nConvID = 0;
			BOOL bResult = TRUE;
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ResetAlignStatus", rReqMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
					rReqMsg.GetMsg(sizeof(BOOL), &bResult);
					bReturn = TRUE;
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			m_bRefCheckRst = TRUE;
			bReturn = TRUE;
		}
	}

	return bReturn;
}

BOOL CWaferPr::OpSearchMultipleDie()
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	int	i;
	BOOL bSrchDieGood = FALSE;
	CString szTemp;

	if ( ConfirmRefDieCheck() )
	{
		if( (m_lLrnTotalRefDie <= 1) || (!m_bAllRefDieCheck) )
		{
			return bSrchDieGood;
		}
	}
	else
	{
		if ( !m_bCheckAllNmlDie )
		{
			return bSrchDieGood;
		}
	}

	szTemp = "WPR - do multiple die search, let bond arm to prepick";
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
	DisplaySequence(szTemp);
	//Move bondarm to pre-pick
	SetConfirmSrch(TRUE);		//v2.56

	//v4.10T3
	//Wait until BH to PREPICK posn
	//WaitBhToPrePick(1000);
#ifdef NU_MOTION
	WaitBhToPrePick(150);		//Reduce Waiting time from 2000ms to 150ms
#else
	WaitBhToPrePick(300);		//Reduce Waiting time from 2000ms to 500ms
#endif
	SetBhToPrePick(FALSE);


	//v3.70T3	//Cree MS899 China for camera blocking issue
	INT nPrePickCount = 0;
	LONG lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];
	while (lBHAtPrePick != 1)
	{
		Sleep(100);
		lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];		
		nPrePickCount++;
		if (nPrePickCount >= 10)
			break;
	}

	szTemp = "WPR - do multiple die search, after bond arm to prepick";
	DisplaySequence(szTemp);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
	if ( ConfirmRefDieCheck() == TRUE )
	{
		if ( (m_lLrnTotalRefDie > 1) && (m_bAllRefDieCheck == TRUE) )
		{
			CMSLogFileUtility::Instance()->WT_GetIdxLog("WPR srch multiple refer die");

			for (i=2; i<=m_lLrnTotalRefDie; i++)
			{
				m_lCurrSrchRefDieId = i;

				//v4.43T11
				if (m_bEnableMS100EjtXY && (m_lCurrSrchRefDieId == 2))	// Ej Cap pattern
				{
					continue;
				}
				if (m_bEnableMS100EjtXY && (m_lCurrSrchRefDieId == 3))	// Collet hole
				{
					continue;
				}

				Sleep(20);						//v4.10T3

				if ( AutoSearchDie() == TRUE )
				{
					Sleep(20);	//100);			//v4.10T3
					if ( AutoGrabDone() == TRUE )
					{
						Sleep(30);	//100);		//v4.10T3

						if ( AutoDieResult() == TRUE )
						{
							bSrchDieGood = TRUE;
							szTemp.Format("Pass of search die result for backup ref ID %d", i);
							CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
							break;
						}
						else
						{
							szTemp.Format("Fail of search die result for backup ref ID %d", i);
							CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
						}
					}
					else
					{
						szTemp.Format("Fail to wait grab done for backup Ref ID %d", i);
						SetStatusMessage(szTemp);
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
						break;
					}
				}
				else
				{
					szTemp.Format("Fail to start search for backup Ref ID %d", i);
					SetStatusMessage(szTemp);
					CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
					break;
				}
			}

			m_lCurrSrchRefDieId = 1;

			if (bSrchDieGood)
			{
				m_bRefCheckRst = TRUE;
			}
			else
			{
				m_bRefCheckRst = FALSE;
				CMSLogFileUtility::Instance()->WT_GetIdxLog("WPR srch Multiple refer die search fail");
			}
		}
	}
	else
	{
		szTemp = "WPR - srch multiple normal die";
		DisplaySequence(szTemp);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
		for (i=2; i<=m_lMaxSortNmlDieID; i++)
		{
			if( IsThisDieLearnt(i-1)!=TRUE )
			{
				szTemp.Format("WPR - multiple search normal die %d not learnt.", i);
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
				DisplaySequence(szTemp);
				continue;
			}

			m_lCurrSrchDieId = i;
			if ( AutoSearchDie() == TRUE )
			{
				Sleep(20);	//100);			//v4.10T3
				if ( AutoGrabDone() == TRUE )
				{
					Sleep(30);	//100);			//v4.10T3
					if ( AutoDieResult() == TRUE )
					{
						bSrchDieGood = TRUE;
						szTemp.Format("WPR - Result of multiple normal die %d ok", i);
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
						DisplaySequence(szTemp);
						break;
					}
					else
					{
						szTemp.Format("WPR - result of multiple normal die %d fail", i);
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
						DisplaySequence(szTemp);
					}
				}
				else
				{
					szTemp.Format("WPR - Auto grab multiple normal die %d fail", i);
					CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
					SetStatusMessage("Fail to wait grab done for backup ID");
					DisplaySequence(szTemp);
					break;
				}
			}
			else
			{
				SetStatusMessage("Fail to start search for backup ID");
				szTemp.Format("WPR - start multiple search normal die %d fail", i);
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
				DisplaySequence(szTemp);
				break;
			}
		}

		m_lCurrSrchDieId = 1;
		if (bSrchDieGood==FALSE)
		{
			szTemp = "WPR - srch Multiple normal die search fail";
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
			DisplaySequence(szTemp);
#ifndef NU_MOTION
			OpResearchNormalDieAtReferPosn();	//	multiple search done, regrab for pr1.
#endif
		}
		else
		{
			OpResearchNormalDieAtReferPosn();	//	multiple search done, regrab for pr1.
		}
	}

	DisplaySequence("WPR - do multiple die search, all done");
	return bSrchDieGood;
}


BOOL CWaferPr::OpConfirmSearch()
{
	BOOL bResult = FALSE;
	BOOL bEnableLF = IsUseLF();		// and Look-forward option is enabled
	BOOL bNewEjSequence = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["NewEjSequence"];

	(*m_psmfSRam)["WaferPr"]["CFS"]["Searched"] = FALSE;

	//if ( bEnableLF == TRUE )			// if in Look-forward mode
	//{
	//	return FALSE;
	//}

	//v2.97T4
	if (!m_bConfirmSearch)
	{
		return FALSE;
	}
	if (DieIsOutAngle(m_fDieAngle))
	{
		return FALSE;
	}

	if( DieNeedRotate(m_fDieAngle) != TRUE )
	{
		return FALSE;
	}

	LONG lPRDelay		= (*m_psmfSRam)["WaferTable"]["PRDelay"];
	LONG lBhBondDelay	= (*m_psmfSRam)["BondProcess"]["BondDelay"]["BP_BondDelay"];
	LONG lWTTDelay		= (*m_psmfSRam)["WaferTable"]["TDelay"];

	DisplaySequence("WPR - do confirm search");
	//Move bondarm to pre-pick
	SetConfirmSrch(TRUE);		//v2.56

	if (bNewEjSequence)			//v3.85
	{
		SetEjectorVacuum(FALSE);
		Sleep(20);
	}

	//Comp. for current die	
	int	siStpX = 0, siStpY = 0, siOrgX = 0, siOrgY = 0, siOrgT = 0;
	CalculateDieCompenate(m_stDieCenter, &siStpX, &siStpY);
	CalculateNDieOffsetXY(siStpX, siStpY);
	GetWaferTableEncoder(&siOrgX, &siOrgY, &siOrgT);
	int siTgtX = siOrgX + siStpX;
	int siTgtY = siOrgY + siStpY;
	WprCompensateRotateDie(siTgtX, siTgtY, (DOUBLE)m_fDieAngle);
	CString szMsg;
	szMsg.Format("WPR - Confirm Srch from %d,%d,%d to %d,%d by offset %d,%d and angle %f",
		siOrgX, siOrgY, siOrgT, siTgtX, siTgtY, siStpX, siStpY, (DOUBLE)m_fDieAngle);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);

	//v4.42T15
	if (m_bEnableMS100EjtXY)
	{
		int siOrigX=0, siOrigY=0, siOrigT=0;
		GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
		(*m_psmfSRam)["WaferPr"]["WTEncX"] = siOrigX;
		(*m_psmfSRam)["WaferPr"]["WTEncY"] = siOrigY;
	}
	
	if (bNewEjSequence)			//v3.85
		SetEjectorVacuum(TRUE);

	if (lWTTDelay > 0)
		Sleep(lWTTDelay);	//v3.68T1


	//Wait until BH to PREPICK posn
#ifdef NU_MOTION
	WaitBhToPrePick(150);		//Reduce Waiting time from 2000ms to 150ms	//v3.94T4
#else
	WaitBhToPrePick(300);		//Reduce Waiting time from 2000ms to 500ms	//v3.94T4
#endif
	SetBhToPrePick(FALSE);

	//v3.70T3	//Cree MS899 China for camera blocking issue
	INT nPrePickCount = 0;
	LONG lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];
	while (lBHAtPrePick != 1)
	{
		Sleep(50);			//Reduce interval from 100 to 50 ms		//v3.94T4
		lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];		
		nPrePickCount++;
		if (nPrePickCount >= 20)
			break;
	}


	//Re-search again
	if ( AutoSearchDie() == TRUE )
	{
		//Sleep(5);
		if ( AutoGrabDone() == TRUE )
		{
			//v2.83T6
			bResult = AutoDieResult();
			if (!bResult)
			{
				LONG lAction = (LONG)(*m_psmfSRam)["WaferMap"]["CurrDie"]["Action"];
				LONG lRow = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"];
				LONG lCol = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"];

				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(lRow, lCol, lUserRow, lUserCol);

				CString szErr;
				szErr.Format("WPR: ConfirmSearch fail at (%d, %d) Action=%d", lUserCol,lUserRow,lAction);
				SetErrorMessage(szErr);
			}
		}
		else
		{
			SetStatusMessage("Fail to wait grab done for confirm search");
		}
	}
	else
		bResult = FALSE;
	(*m_psmfSRam)["WaferPr"]["CFS"]["Searched"] = TRUE;

	return bResult;
}


BOOL CWaferPr::OpAutoResearchDie1()		//v4.41T5
{
	DisplaySequence("WPR - Auto Research die");

	SetConfirmSrch(TRUE);

	BOOL bResult = TRUE;
	CString szTemp;

	//Wait until BH to PREPICK posn
	WaitBhToPrePick(150);		//Reduce Waiting time from 2000ms to 150ms
	SetBhToPrePick(FALSE);

	//Cree MS899 China for camera blocking issue
	INT nPrePickCount = 0;
	LONG lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];
	while (lBHAtPrePick != 1)
	{
		Sleep(50);			//Reduce interval from 100 to 50 ms	
		lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];		
		nPrePickCount++;
		if (nPrePickCount >= 20)
			break;
	}

	BOOL bNeedMultiSearchNormalDie = FALSE;

	//Re-search again
	if ( AutoSearchDie() == TRUE )
	{
		Sleep(5);
		if ( AutoGrabDone() == TRUE )
		{
			bResult = AutoDieResult();
			if (!bResult)
			{
				bNeedMultiSearchNormalDie = TRUE;	//v4.42T3
				szTemp = "WPR - Auto Research die 1st try fail";
			}
			else
			{
				szTemp = "WPR - Auto Research die 1st try ok";
			}
		}
		else
		{
			bResult = FALSE;
			szTemp = "Fail to wait grab done for OpAuto ResearchDie1";
			SetErrorMessage(szTemp);
		}
	}
	else
	{
		bResult = FALSE;
		szTemp = "Fail to grab image for OpAuto ResearchDie1";
		SetErrorMessage(szTemp);
	}
	DisplaySequence("WPR - " + szTemp);
	
	//v4.42T3
	//If Die 2 & 3 also learnt with multi-serach nml die option enabled
	if (!bResult && m_bCheckAllNmlDie && bNeedMultiSearchNormalDie)
	{
		szTemp = "WPR - OpAutoResearchDie 1 use multiNmlDice search ...";
		CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
		DisplaySequence(szTemp);

		for (INT i=2; i<=m_lMaxSortNmlDieID; i++)
		{
			if( IsThisDieLearnt(i-1) != TRUE )
			{
				szTemp.Format("WPR - OpAutoResearchDie 1 skip EMPTY Die Record #%d", i);
				CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
				DisplaySequence(szTemp);
				continue;
			}

			m_lCurrSrchDieId = i;

			if ( AutoSearchDie() == TRUE )
			{
				Sleep(20);
				if ( AutoGrabDone() == TRUE )
				{
					Sleep(30);

					bResult = AutoDieResult();
				}
			}

			szTemp.Format("WPR - OpAutoResearchDie 1 use Die Record #%d - result = %d", i, bResult);
			CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
			DisplaySequence(szTemp);

			if (bResult == TRUE)
			{
				break;
			}
		}

		m_lCurrSrchDieId = 1;
		OpResearchNormalDieAtReferPosn();	//	search normal pr1 after auto research die done.
	}

	//v4.52A1
	//v4.42T15
	//szTemp.Format("WPR: AutoResearchDie done; result = %d", bResult);
				
	//v4.48A8
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//if (pApp->GetCustomerName() == "Electech3E(DL)" ||			//v4.48A21 Changed && to ||
	//	CMSLogFileUtility::Instance()->GetEnableMachineLog())
	//{
	//	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
	//}

	return bResult;
}


BOOL CWaferPr::OpAutoResearchDie2()		//v4.41T5
{
	SetErrorMessage("OpAutoResearchDie 2 ... ");
	return FALSE;
}


BOOL CWaferPr::SearchBHColletHole(const BOOL bEpoxyPattern)
{
	CStdioFile oLogFile;
	CString szLine;
	CString szMsg;

	BOOL bBH2 = (BOOL)(LONG)((*m_psmfSRam)["BondHead"]["ColletTest"]["BH Mark 2"]);
	LONG lDieNo =  bBH2 ? WPR_REFERENCE_PR_DIE_INDEX8 : WPR_REFERENCE_PR_DIE_INDEX7;

	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset,stEjtOffsetInPixel;
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	//SRCH_RESULT		stResult;
	
	PR_COORD coCorner1temp = {PR_MAX_WIN_ULC_X, PR_MAX_WIN_ULC_Y};

	PR_COORD coCorner2temp = {PR_MAX_WIN_LRC_X, PR_MAX_WIN_LRC_Y};

	BOOL bSrchOK = FALSE;
	if (bEpoxyPattern)
	{
		UINT lSrchEjtColletCoorX = 0;
		UINT lSrchEjtColletCoorY = 0;
		ULONG ulEpoxyAreaSize = 0;
		CString szErrMsg = "";
		BOOL bEpoxyStatus = AutoSearchEpoxy(TRUE, TRUE, FALSE, 0, 0, lSrchEjtColletCoorX, lSrchEjtColletCoorY, ulEpoxyAreaSize, &m_evSearchMarkReady,szErrMsg);
		DisplaySequence("auto seach BH Collet begin");
		if (bEpoxyStatus)
		{
			double dStepX_um = 0, dStepY_um = 0;
			if (bBH2 == 0)
			{
				stEjtOffsetInPixel.x = (PR_WORD)(lSrchEjtColletCoorX - m_lEjtCollet1CoorX);
				stEjtOffsetInPixel.y = (PR_WORD)(lSrchEjtColletCoorY - m_lEjtCollet1CoorY);

				ConvertFFPixelToUM(stEjtOffsetInPixel, dStepX_um, dStepY_um);
				int siStepX = ConvertUMToCount(dStepX_um, m_dEjtXYRes);
				int siStepY = ConvertUMToCount(dStepY_um, m_dEjtXYRes);

				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleX1_um"] = dStepX_um;
				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleY1_um"] = dStepY_um;

				szMsg.Format("Srch BHColletHole1 OK,PR(%d,%d),ref1(%d,%d),PixelOffset(%d,%d), EjtCountOffset(%d,%d), EjtCountOffset(%.1f,%.1f) um",
					lSrchEjtColletCoorX, lSrchEjtColletCoorY, m_lEjtCollet1CoorX, m_lEjtCollet1CoorY, stEjtOffsetInPixel.x, stEjtOffsetInPixel.y, siStepX, siStepY, dStepX_um, dStepY_um);
				SaveBHMark1(szMsg);
			}
			else
			{
				stEjtOffsetInPixel.x = (PR_WORD)(lSrchEjtColletCoorX - m_lEjtCollet2CoorX);
				stEjtOffsetInPixel.y = (PR_WORD)(lSrchEjtColletCoorY - m_lEjtCollet2CoorY);

				ConvertFFPixelToUM(stEjtOffsetInPixel, dStepX_um, dStepY_um);
				int siStepX = ConvertUMToCount(dStepX_um, m_dEjtXYRes);
				int siStepY = ConvertUMToCount(dStepY_um, m_dEjtXYRes);

				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleX2_um"] = dStepX_um;
				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleY2_um"] = dStepY_um;

				szMsg.Format("Srch BHColletHole2 OK,PR(%d,%d),ref2(%d,%d),PixelOffset(%d,%d), EjtCountOffset(%d,%d), EjtCountOffset(%.1f,%.1f) um",
					lSrchEjtColletCoorX, lSrchEjtColletCoorY, m_lEjtCollet2CoorX, m_lEjtCollet2CoorY, stEjtOffsetInPixel.x, stEjtOffsetInPixel.y, siStepX, siStepY, dStepX_um, dStepY_um);
					
				SaveBHMark2(szMsg);
			}
		}
		else
		{
			if (bBH2 == 0)
				SaveBHMark1(szErrMsg);
			else
				SaveBHMark2(szErrMsg);
		}
	}
	else
	{
		PR_WORD wResult = ManualSearchDie(WPR_REFERENCE_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, coCorner1temp, coCorner2temp);
		DisplaySequence("auto seach collet hole begin");
		if (wResult != -1)
		{
			if (DieIsAlignable(usDieType) == TRUE)
			{
				double dStepX_um = 0, dStepY_um = 0;
				if (bBH2 == 0)
				{
					stEjtOffsetInPixel.x = (PR_WORD)(stDieOffset.x - m_lBHColletHoleRef1X);
					stEjtOffsetInPixel.y = (PR_WORD)(stDieOffset.y - m_lBHColletHoleRef1Y);
					ConvertFFPixelToUM(stEjtOffsetInPixel, dStepX_um, dStepY_um);
					int siStepX = ConvertUMToCount(dStepX_um, m_dEjtXYRes);
					int siStepY = ConvertUMToCount(dStepY_um, m_dEjtXYRes);

					(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleX1_um"] = dStepX_um;
					(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleY1_um"] = dStepY_um;

					szMsg.Format("Srch BHColletHole1 OK,PR(%d,%d),ref1(%d,%d),PixelOffset(%d,%d), EjtCountOffset(%d,%d), EjtCountOffset(%.1f,%.1f) um",
						stDieOffset.x, stDieOffset.y, m_lBHColletHoleRef1X, m_lBHColletHoleRef1Y, stEjtOffsetInPixel.x, stEjtOffsetInPixel.y, siStepX, siStepY, dStepX_um, dStepY_um);
					SaveBHMark1(szMsg);
				}
				else
				{
					stEjtOffsetInPixel.x = (PR_WORD)(stDieOffset.x - m_lBHColletHoleRef2X);
					stEjtOffsetInPixel.y = (PR_WORD)(stDieOffset.y - m_lBHColletHoleRef2Y);
					ConvertFFPixelToUM(stEjtOffsetInPixel, dStepX_um, dStepY_um);
					int siStepX = ConvertUMToCount(dStepX_um, m_dEjtXYRes);
					int siStepY = ConvertUMToCount(dStepY_um, m_dEjtXYRes);

					(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleX2_um"] = dStepX_um;
					(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleY2_um"] = dStepY_um;

					szMsg.Format("Srch BHColletHole2 OK,PR(%d,%d),ref2(%d,%d),PixelOffset(%d,%d), EjtCountOffset(%d,%d), EjtCountOffset(%.1f,%.1f) um",
						stDieOffset.x, stDieOffset.y, m_lBHColletHoleRef2X, m_lBHColletHoleRef2Y, stEjtOffsetInPixel.x, stEjtOffsetInPixel.y, siStepX, siStepY, dStepX_um, dStepY_um);
					
					SaveBHMark2(szMsg);
				}
			}
			else
			{
				szMsg = "Die not alignable";
				if (bBH2 == 0)
					SaveBHMark1(szMsg);
				else
					SaveBHMark2(szMsg);
			}
		}
		else
		{
			szMsg = "Search Failure";
			if (bBH2 == 0)
				SaveBHMark1(szMsg);
			else
				SaveBHMark2(szMsg);
		}
	}

	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	DisplaySequence(szMsg);

	return TRUE;
}


BOOL CWaferPr::SearchBHMark()
{
	CStdioFile oLogFile;
	CString szLine;
	CString szMsg;

	BOOL bBH2 = (BOOL)(LONG)((*m_psmfSRam)["BondHead"]["ColletTest"]["BH Mark 2"]);
	LONG lDieNo =  bBH2 ? WPR_REFERENCE_PR_DIE_INDEX6 : WPR_REFERENCE_PR_DIE_INDEX5;
	
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset,stEjtOffsetInPixel;
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	//SRCH_RESULT		stResult;
	PR_WORD wResult = 0;
	PR_COORD coCorner1temp = {PR_MAX_WIN_ULC_X, PR_MAX_WIN_ULC_Y};
	PR_COORD coCorner2temp = {PR_MAX_WIN_LRC_X, PR_MAX_WIN_LRC_Y};

	LONG lCollet1OffsetX	= GetEjtCollet1OffsetX();
	LONG lCollet1OffsetY	= GetEjtCollet1OffsetY();
	LONG lCollet2OffsetX	= GetEjtCollet2OffsetX();
	LONG lCollet2OffsetY	= GetEjtCollet2OffsetY();

	LONG lNewCollet1OffsetX, lNewCollet1OffsetY, lNewCollet2OffsetX, lNewCollet2OffsetY;
	wResult = ManualSearchDie(WPR_REFERENCE_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, coCorner1temp, coCorner2temp);
	DisplaySequence("auto seach BH mark begin");
	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			LONG lColletHoleEnc_T = 0;
			LONG lColletHoleEnc_Z = 0;
			LONG lBondCount = 0;
			CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
			if (pBondHead != NULL)
			{
				lColletHoleEnc_T = pBondHead->GetColletHoleEnc_T();
				lColletHoleEnc_Z = pBondHead->GetColletHoleEnc_Z();
				lBondCount = pBondHead->GetBondCount();
			}

			double dStepX_um = 0, dStepY_um = 0;
			if (bBH2 == 0)
			{
				stEjtOffsetInPixel.x = (PR_WORD)(stDieOffset.x - m_lBHMarkRef1X);
				stEjtOffsetInPixel.y = (PR_WORD)(stDieOffset.y - m_lBHMarkRef1Y);
				ConvertFFPixelToUM(stEjtOffsetInPixel, dStepX_um, dStepY_um);
				int siStepX = ConvertUMToCount(dStepX_um, m_dEjtXYRes);
				int siStepY = ConvertUMToCount(dStepY_um, m_dEjtXYRes);
				if (lBondCount > 2)
				{
					if ((fabs((double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX1_um"] - dStepX_um) < 100) && 
						(fabs((double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY1_um"] - dStepY_um) < 100))
					{
						(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej1_CurValueValid"] = 1;
					}
					else
					{
						(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej1_CurValueValid"] = 0;
					}
				}

				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX1_Pixel"]		= stEjtOffsetInPixel.x;
				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY1_Pixel"]		= stEjtOffsetInPixel.y;

				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX1_um"]			= dStepX_um;
				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY1_um"]			= dStepY_um;

				lNewCollet1OffsetX = lCollet1OffsetX + siStepX;
				lNewCollet1OffsetY = lCollet1OffsetY + siStepY;
				szMsg.Format("Srch BHMark1 OK,PR(%d,%d),ref1(%d,%d),PixelOffset(%d,%d),Ejt(%d,%d),Ejt(%.1f,%.1f)um,Ori ColletOffset(%d,%d),updated ColletOffset(%d,%d), BHT enc(%d), BHZ1(%d)",
					stDieOffset.x, stDieOffset.y, m_lBHMarkRef1X, m_lBHMarkRef1Y, stEjtOffsetInPixel.x, stEjtOffsetInPixel.y, siStepX, siStepY, dStepX_um, dStepY_um,
					lCollet1OffsetX, lCollet1OffsetY, lNewCollet1OffsetX, lNewCollet1OffsetY, lColletHoleEnc_T, lColletHoleEnc_Z);
				SaveBHMark1(szMsg);
			}
			else
			{
				stEjtOffsetInPixel.x = (PR_WORD)(stDieOffset.x - m_lBHMarkRef2X);
				stEjtOffsetInPixel.y = (PR_WORD)(stDieOffset.y - m_lBHMarkRef2Y);
				ConvertFFPixelToUM(stEjtOffsetInPixel, dStepX_um, dStepY_um);
				int siStepX = ConvertUMToCount(dStepX_um, m_dEjtXYRes);
				int siStepY = ConvertUMToCount(dStepY_um, m_dEjtXYRes);

				if (lBondCount > 2)
				{
					if ((fabs((double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX2_um"] - dStepX_um) < 10) && 
						(fabs((double)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY2_um"] - dStepY_um) < 10))
					{
						(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej2_CurValueValid"] = 1;
					}
					else
					{
						(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej2_CurValueValid"] = 0;
					}
				}

				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX2_Pixel"]		= stEjtOffsetInPixel.x;
				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY2_Pixel"]		= stEjtOffsetInPixel.y;

				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX2_um"]			= dStepX_um;
				(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY2_um"]			= dStepY_um;

				lNewCollet2OffsetX = lCollet2OffsetX + siStepX;
				lNewCollet2OffsetY = lCollet2OffsetY + siStepY;
				szMsg.Format("Srch BHMark2 OK,PR(%d,%d),ref2(%d,%d),PixelOffset(%d,%d),Ejt(%d,%d),Ejt(%.1f,%.1f)um,Ori ColletOffset(%d,%d),updated ColletOffset(%d,%d), BHT enc(%d), BHZ2(%d)",
					stDieOffset.x, stDieOffset.y, m_lBHMarkRef2X, m_lBHMarkRef2Y, stEjtOffsetInPixel.x, stEjtOffsetInPixel.y, siStepX, siStepY, dStepX_um, dStepY_um,
					lCollet2OffsetX, lCollet2OffsetY, lNewCollet2OffsetX, lNewCollet2OffsetY, lColletHoleEnc_T, lColletHoleEnc_Z);
				SaveBHMark2(szMsg);
			}
		}
		else
		{
			szMsg = "Die not alignable";
			if (bBH2 == 0)
				SaveBHMark1(szMsg);
			else
				SaveBHMark2(szMsg);
		}
	}
	else
	{
		szMsg = "Search Failure";
		if (bBH2 == 0)
			SaveBHMark1(szMsg);
		else
			SaveBHMark2(szMsg);
	}

	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	DisplaySequence(szMsg);

	return TRUE;
}

BOOL CWaferPr::OpAutoSearchBHMark()
{
	TakeTime(CJO);

	CMS896AApp			*pApp = (CMS896AApp*) AfxGetApp();
	BOOL bLogColletHoleData = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_ENABLE_LOG_COLLET_HOLE_DATA);
	if (bLogColletHoleData)
	{
		SearchBHColletHole(TRUE);
	}

	LONG lOldZoom = GetRunZoom();
	LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_FF, FALSE, FALSE, 23);

	if (bLogColletHoleData)
	{
//		SearchBHColletHole(TRUE);
	}
	BOOL bRet = SearchBHMark();
	
	LiveViewZoom(lOldZoom, FALSE, FALSE, 24);

	TakeTime(MDO);
	return bRet;
}


BOOL CWaferPr::SearchColletHoleWithEpoxyPattern()
{
	CStdioFile oLogFile;
	CString szLine;
	CString szMsg;

	BOOL bBH2 = (BOOL)(LONG)((*m_psmfSRam)["BondHead"]["ColletTest"]["BH Mark 2"]);
	LONG lDieNo = WPR_REFERENCE_PR_DIE_INDEX4;  //Collet Hole with Epoxy Pattern
	
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stEjtOffsetInPixel;

	LONG lCollet1OffsetX	= GetEjtCollet1OffsetX();
	LONG lCollet1OffsetY	= GetEjtCollet1OffsetY();
	LONG lCollet2OffsetX	= GetEjtCollet2OffsetX();
	LONG lCollet2OffsetY	= GetEjtCollet2OffsetY();

	m_lChangeColletTempCenter1X = m_lEjtCollet1CoorX;
	m_lChangeColletTempCenter1Y = m_lEjtCollet1CoorY;
	m_lChangeColletTempCenter2X = m_lEjtCollet2CoorX;
	m_lChangeColletTempCenter2Y = m_lEjtCollet2CoorY;

	UINT lSrchEjtColletCoorX = 0;
	UINT lSrchEjtColletCoorY = 0;
	ULONG ulEpoxyAreaSize = 0;
	CString szErrMsg = "";
	BOOL bEpoxyStatus = AutoSearchEpoxy(TRUE, TRUE, FALSE, 0, 0, lSrchEjtColletCoorX, lSrchEjtColletCoorY, ulEpoxyAreaSize, &m_evSearchMarkReady,szErrMsg);
	SetSearchMarkReady(TRUE);//Set Ready Here in case AutoSearchEpoxy has error in reply1(cause software hang)
	szMsg.Format("WPR - auto seach BH%d Collet begin,Status%d,ErrMsg,%s",bBH2,bEpoxyStatus,szErrMsg);
	DisplaySequence(szMsg);
	if (bEpoxyStatus)
	{
		LONG lColletHoleEnc_T = 0;
		LONG lColletHoleEnc_Z = 0;
		LONG lBondCount = 0;
		CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
		if (pBondHead != NULL)
		{
			lColletHoleEnc_T = pBondHead->GetColletHoleEnc_T();
			lColletHoleEnc_Z = pBondHead->GetColletHoleEnc_Z();
			lBondCount = pBondHead->GetBondCount();
		}

		LONG lStepX = 0, lStepY = 0;
		DOUBLE dRatio = 1;
		if (m_szGenDieZoomMode[10 + WPR_GEN_RDIE_OFFSET] == "1X")
		{
			dRatio = 0.25;
		}
		else if (m_szGenDieZoomMode[10 + WPR_GEN_RDIE_OFFSET] == "2X")
		{
			dRatio = 0.5;
		}

		CalcEjtColletOffsetCount(lSrchEjtColletCoorX, lSrchEjtColletCoorY, lStepX, lStepY);
		lStepX = _round(lStepX * dRatio);
		lStepY = _round(lStepY * dRatio);

		int siStepX = 0, siStepY = 0, nOK = 0;
		double dStepX_um = 0, dStepY_um = 0;

		if (bBH2 == 0)
		{
			//if (lBondCount > 2)
			//{
				if ((abs(/*lCollet1OffsetX - lStepX*/siStepX) < 20) && (abs(/*lCollet1OffsetY - lStepY*/siStepY) < 20))
				{
					(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej1_CurValueValid"] = 1;
					nOK = 1;

					m_lEjtCollet1CoorX = lSrchEjtColletCoorX;
					m_lEjtCollet1CoorY = lSrchEjtColletCoorY;
					CalcEjtColletOffset(dRatio);
					CalcBTColletOffset(dRatio,
									   m_lChangeColletTempCenter1X, m_lChangeColletTempCenter1Y,
									   m_lChangeColletTempCenter2X, m_lChangeColletTempCenter2Y);

					//Log Only
					stEjtOffsetInPixel.x = (PR_WORD)(lSrchEjtColletCoorX - m_lChangeColletTempCenter1X);
					stEjtOffsetInPixel.y = (PR_WORD)(lSrchEjtColletCoorY - m_lChangeColletTempCenter1Y);
					ConvertPixelToEjtMotorStep(stEjtOffsetInPixel, &siStepX, &siStepY);
					dStepX_um = ConvertCountToUM(siStepX, m_dEjtXYRes);
					dStepY_um = ConvertCountToUM(siStepY, m_dEjtXYRes);
				}
				else
				{
					(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej1_CurValueValid"] = 0;
				}
			//}

			LONG lNewCollet1OffsetX	= GetEjtCollet1OffsetX();
			LONG lNewCollet1OffsetY	= GetEjtCollet1OffsetY();
			szMsg.Format("Srch ColletHole1 WithinRange,%d, newPR(%d,%d),oldPR(%d,%d),PixelOffset(%d,%d),Ejt(%d,%d),Ejt(%.1f,%.1f)um,Ori ColletOffset(%d,%d),updated ColletOffset(%d,%d), BHT enc(%d), BHZ1(%d),lStep(%d,%d),ColletLife,%d",
						nOK,lSrchEjtColletCoorX, lSrchEjtColletCoorY, m_lChangeColletTempCenter1X, m_lChangeColletTempCenter1Y, lSrchEjtColletCoorX - m_lChangeColletTempCenter1X, lSrchEjtColletCoorY - m_lChangeColletTempCenter1Y,
						siStepX, siStepY, dStepX_um, dStepY_um,
						lCollet1OffsetX, lCollet1OffsetY, lNewCollet1OffsetX, lNewCollet1OffsetY, lColletHoleEnc_T, lColletHoleEnc_Z,lStepX,lStepY,lBondCount);
			SaveBHMark1(szMsg);
		}
		else
		{
			//if (lBondCount > 2)
			//{
				if ((abs(/*lCollet2OffsetX - lStepX*/siStepX) < 20) && (abs(/*lCollet2OffsetY - lStepY*/siStepY) < 20))
				{
					(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej2_CurValueValid"] = 1;

					m_lEjtCollet2CoorX = lSrchEjtColletCoorX;
					m_lEjtCollet2CoorY = lSrchEjtColletCoorY;
					CalcEjtColletOffset(dRatio);
					CalcBTColletOffset(dRatio, 
									   m_lChangeColletTempCenter1X, m_lChangeColletTempCenter1Y,
									   m_lChangeColletTempCenter2X, m_lChangeColletTempCenter2Y);

					//Log Only
					stEjtOffsetInPixel.x = (PR_WORD)(lSrchEjtColletCoorX - m_lChangeColletTempCenter2X);
					stEjtOffsetInPixel.y = (PR_WORD)(lSrchEjtColletCoorY - m_lChangeColletTempCenter2Y);
					ConvertPixelToEjtMotorStep(stEjtOffsetInPixel, &siStepX, &siStepY);
					dStepX_um = ConvertCountToUM(siStepX, m_dEjtXYRes);
					dStepY_um = ConvertCountToUM(siStepY, m_dEjtXYRes);
				}
				else
				{
					(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej2_CurValueValid"] = 0;
				}
			//}

			LONG lNewCollet2OffsetX	= GetEjtCollet1OffsetX();
			LONG lNewCollet2OffsetY	= GetEjtCollet1OffsetY();
			szMsg.Format("Srch ColletHole2 WithinRange,%d,newPR(%d,%d),oldPR(%d,%d),PixelOffset(%d,%d),Ejt(%d,%d),Ejt(%.1f,%.1f)um,Ori ColletOffset(%d,%d),updated ColletOffset(%d,%d), BHT enc(%d), BHZ2(%d),lStep(%d,%d),ColletLife,%d",
					nOK,lSrchEjtColletCoorX, lSrchEjtColletCoorY, m_lChangeColletTempCenter2X, m_lChangeColletTempCenter2Y, lSrchEjtColletCoorX - m_lChangeColletTempCenter2X, lSrchEjtColletCoorY - m_lChangeColletTempCenter2Y,
					siStepX, siStepY, dStepX_um, dStepY_um,
					lCollet2OffsetX, lCollet2OffsetY, lNewCollet2OffsetX, lNewCollet2OffsetY, lColletHoleEnc_T, lColletHoleEnc_Z,lStepX,lStepY,lBondCount);
			SaveBHMark2(szMsg);
		}
	}
	else
	{
		szMsg = "Search Failure";
		if (bBH2 == 0)
			SaveBHMark1(szErrMsg);
		else
			SaveBHMark2(szErrMsg);
	}

	//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	DisplaySequence(szMsg);

	return TRUE;
}


BOOL CWaferPr::OpAutoSearchColletHoleWithEpoxyPattern()
{
	TakeTime(CJO);
	LONG lOldZoom = GetRunZoom();
	LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_4X, FALSE, FALSE, 25);

	BOOL bRet = SearchColletHoleWithEpoxyPattern();
	
	LiveViewZoom(lOldZoom, FALSE, FALSE, 26);
	TakeTime(MDO);
	return bRet;
}

BOOL CWaferPr::OpAutoSearchEmptyDie()		//v4.54A5	//MS60 New NGPick EmptyDieCheck
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
//	PR_COORD		stDieOffset; 
//	PR_REAL			fDieRotate; 
//	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
//	PR_WORD			rtnPR;

	LONG lUserRow	= (*m_psmfSRam)["WaferMap"]["LastDie"]["UserRow"];
	LONG lUserCol	= (*m_psmfSRam)["WaferMap"]["LastDie"]["UserCol"];

	UCHAR ucColletID = 0;
	PR_COORD	stSrchCorner1 = GetSrchArea().coCorner1;
	PR_COORD	stSrchCorner2 = GetSrchArea().coCorner2;

	PR_COORD	stSrchCenter;
	stSrchCenter.x = (PR_WORD)GetPrCenterX();
	stSrchCenter.y = (PR_WORD)GetPrCenterY();

	if (CMS896AStn::m_nWTAtColletPos == 1)			//BH1
	{
		ucColletID = 1;
		stSrchCorner1.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
		stSrchCorner1.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
		stSrchCorner2.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
		stSrchCorner2.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;

		stSrchCenter.x	+= (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
		stSrchCenter.y	+= (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
	}
	else if (CMS896AStn::m_nWTAtColletPos == 2)		//BH2
	{
		ucColletID = 2;
		stSrchCorner1.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
		stSrchCorner1.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
		stSrchCorner2.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
		stSrchCorner2.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;

		stSrchCenter.x	+= (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
		stSrchCenter.y	+= (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
	}

/*
	rtnPR = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE+2, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
				stSrchCorner1, stSrchCorner2);

	CString szMsg;
	szMsg.Format("OpAutoSearchEmptyDie at (%ld, %ld): ColletID = %d, Return = %d, result = %d, EMPTY=%d", 
					lUserCol, lUserRow, ucColletID, rtnPR, usDieType, DieIsAlignable(usDieType));
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	if (rtnPR != -1)
	{
		if (!DieIsAlignable(usDieType) == TRUE)
		{
			(*m_psmfSRam)["WaferPr"]["NGPick"]["IsEmpty"] = FALSE;
			SetWPREmptyCheckDone(TRUE);
			return FALSE;
		}
	}
*/
	UINT nX = 0, nY = 0;
	ULONG lEpoxySize = 0;
	CString szErrMsg = "";
	BOOL bEproxyStatus = AutoSearchEpoxy(TRUE, TRUE, FALSE, stSrchCenter.x, stSrchCenter.y, nX, nY, lEpoxySize, NULL,szErrMsg);
	CString szMsg;
	szMsg.Format("OpAutoSearchEmptyDie at (%ld, %ld): ColletID = %d, Return = %d, Size = %d", 
					lUserCol, lUserRow, ucColletID, bEproxyStatus, lEpoxySize);
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);


	if (!bEproxyStatus)
	{
		//Epoxy not found -> no EMPTY HOLE -> fails !!!!
		(*m_psmfSRam)["WaferPr"]["NGPick"]["IsEmpty"] = FALSE;
		SetWPREmptyCheckDone(TRUE);
		return FALSE;
	}

	(*m_psmfSRam)["WaferPr"]["NGPick"]["IsEmpty"] = TRUE;
	SetWPREmptyCheckDone(TRUE);

	return TRUE;	//TRUE -> OK -> current pos is EMPTY -> die is picked successfully!
}

BOOL CWaferPr::OpResearchNormalDieAtReferPosn()
{
	if( IsBurnIn() )
	{
		return TRUE;
	}

	LONG lRefDieCheck	= (*m_psmfSRam)["DieInfo"]["RefDie"]["Check"];
	LONG lSpRefDieCheck = (*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"];

	(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]		= 0;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"]	= 0;

	CString szMsg;
	szMsg = "WPR - At die research normal die image";
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	DisplaySequence(szMsg);

	// Get the reply for the encoder value
	IPC_CServiceMessage stMsg;
	int nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveToPrePick", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}	
	}

	BOOL bSrchDieGood = FALSE;
	BOOL		bDieType = WPR_NORMAL_DIE;
	LONG		lRefDieNo = 1;//WPR_NORMAL_DIE;
	PR_UWORD	usSearchResult;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	usSearchResult = SearchDieCmd(bDieType, lRefDieNo, ubSID, ubRID, GetSrchArea().coCorner1, GetSrchArea().coCorner2, PR_TRUE, PR_TRUE, PR_FALSE, PR_NO_DISPLAY);

	if( usSearchResult == PR_ERR_NOERR )
	{
		Sleep(50);
		PR_SRCH_DIE_RPY1	stSrchRpy1;
		usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);

		if ( usSearchResult == PR_ERR_NOERR )
		{
			Sleep(50);
			PR_SRCH_DIE_RPY2	stSrchRpy2;
			if (SearchDieRpy2(ubSID, &stSrchRpy2) != PR_COMM_ERR)
			{
				bSrchDieGood = TRUE;
				szMsg = "WPR - research normal good";
			}
			else
			{
				szMsg = "WPR - research normal rpy2 fail";
			}
		}
		else
		{
			szMsg = "WPR - Fail to wait grab done for normal at refer position";
		}
	}
	else
	{
		if (usSearchResult == IDS_WPR_DIENOTLEARNT)
		{
			szMsg.Format("WPR Not Learnt");
		}
		else
		{
			szMsg = "WPR - Fail to re search for normal at refer position";
		}
	}

	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	DisplaySequence(szMsg);
	(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]		= lRefDieCheck;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"]	= lSpRefDieCheck;

	return bSrchDieGood;
}


BOOL CWaferPr::IsMotionHardwareReady()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	if( IsWprWithAF() )
	{
		if( CMS896AStn::MotionCheckResult(WAFERPR_AXIS_Z, &m_stZoomAxis_Z) == FALSE ||
			!m_bZoomHome_Z )
		{
			StopCycle("WaferPrStn");
			SetMotionCE(TRUE, "WPR IsMotionHardwareReady1");

			CString szText = "\nFocus Z";
			SetStatusMessage("Wafer PR focus NOT home yet");
			SetErrorMessage("Wafer PR focus NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);

			return FALSE;
		}
	}

	if ( IsBLInUse() && m_bSelBL_Z )
	{
		if( CMS896AStn::MotionCheckResult(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z) == FALSE ||
			!m_bBackLightHome_Z )
		{
			StopCycle("WaferPrStn");
			SetMotionCE(TRUE, "WPR IsMotionHardwareReady2");

			CString szText = "\nBack Light Z";
			SetStatusMessage("Wafer PR Back Light NOT home yet");
			SetErrorMessage("Wafer PR Back Light NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);

			return FALSE;
		}
	}

	return TRUE;
}


UCHAR CWaferPr::GetPrescanPrID()
{
	if( m_ucPrescanDiePrID<=0 || m_ucPrescanDiePrID>3 )
	{
		m_ucPrescanDiePrID = 1;
	}

	if( IsEnableZoom() )
	{
#ifdef VS_5MCAM
	//	m_ucPrescanDiePrID = 3;	//	zoomview
#endif
	}

	return m_ucPrescanDiePrID;
}

UCHAR CWaferPr::GetPrescan2ndPrID()
{
	if( m_bEnable2ndPrSearch )
	{
		if( m_lSelect2ndPrID==1 )
			return WPR_PSCN_2NDPR_RECORD_ID;
		else
			return WPR_PSCN_2NDPR_DEFAULT_ID;
	}

	return 0;
}

BOOL CWaferPr::OpSetPRTesting()
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	PR_WORD			rtnPR;

	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX()-1, GetPRWinLRY()-1};


	int siRefX = 0;
	int siRefY = 0;
	int siRefX1 = 0,siRefY1 = 0;


	LONG lDieNo = WPR_NORMAL_DIE;
	rtnPR = ManualSearchDie(WPR_NORMAL_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2,
								FALSE);
	CString szMsg;
	BOOL bUseNewLF = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["Use New LF Cycle"];
	szMsg.Format("Set PR Testing,New LF,%d,(%d,%d),Rotate,%f,Score,%f",bUseNewLF,stDieOffset.x,stDieOffset.y,fDieRotate,fDieScore);
	SaveColletSizeEvent(szMsg);
	return TRUE;
}

BOOL CWaferPr::OpAutoLogPRCase(BOOL bForceEnable, ULONG ulPRCase)
{
	CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
	if (pPRFailureCaseLog->IsEnableFailureCaselog() || bForceEnable)
	{
		CloseAlarm();
		CString szMsg;
		szMsg = "Log PR Failure Case...";
		SetAlert_WarningMsg(IDS_MS_WARNING_LOG_PR_FAILURE_CASE, szMsg);
		pPRFailureCaseLog->LogFailureCaseDumpFile(GetRunSenID(), GetRunRecID(), ulPRCase);
		if (State() == IDLE_Q)
		{
			SetAlarmLamp_Yellow(FALSE, TRUE);
		}
		else
		{
			SetAlarmLamp_Green(FALSE, TRUE);
		}
	}
	return TRUE;
}
