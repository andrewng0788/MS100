/////////////////////////////////////////////////////////////////
// WT_CycleState.cpp : Cycle Operation State of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Saturday, December 4, 2004
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
#include "WaferTable.h"
#include "CycleState.h"
#include "WT_Log.h"
#include "LogFileUtil.h"
#include "PrescanUtility.h"
#include "PrescanInfo.h"
#include "MarkDieRegionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CWaferTable::AutoOperation()
{
	RunOperation();
}

VOID CWaferTable::DemoOperation()
{
	RunOperation();
}

VOID CWaferTable::ManualOperation()
{
	State(IDLE_Q);
}

VOID CWaferTable::RunOperation()
{
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( m_bFirstInAutoCycle )	//	scan wafer done
	{
		m_bFirstInAutoCycle = FALSE;
		ULONG lDiskUsed = 0, lDiskFree = 0;
		DOUBLE dStartTime = GetTime();
		if( CheckDiskSize("C:", lDiskUsed, lDiskFree) )	// MB
		{
			if( lDiskFree<512 )
			{
				szMsg.Format("C: Harddisk free space is %luMB.", lDiskFree);
				if( lDiskFree < 200 )
				{
					szMsg += "\nPlease clean up at once!";
					m_qSubOperation	= HOUSE_KEEPING_Q;
					m_nLastError	= gnNOTOK;
				}
				LONG lUsedTime = (LONG)(GetTime() - dStartTime);
				CString szTest;
				szTest.Format("Used Time %d FreeBytes:%lu, used %lu", lUsedTime, lDiskFree, lDiskUsed);
				SetErrorMessage(szTest);
				HmiMessage_Red_Back(szMsg, "C: checking");
			}
		}
		else
		{
			szMsg.Format("C: Harddisk free space check fail.");
			HmiMessage_Red_Yellow(szMsg, "C: checking");
			m_qSubOperation	= HOUSE_KEEPING_Q;
			m_nLastError	= gnNOTOK;
		}
	}

	if( m_qSubOperation==PRESCAN_WAIT_ALL_DONE_Q	|| 
		m_qSubOperation==PRESCAN_GET_WAFER_POS_Q	|| 
		m_qSubOperation==PRESCAN_WAIT_PR_DELAY_Q	|| 
		m_qSubOperation==PRESCAN_MOVE_TABLE_Q		|| 
		m_qSubOperation==PRESCAN_WAIT_DIE_READY_Q	|| 
		m_qSubOperation==PRESCAN_AP_GET_WAFER_POS_Q	|| 
		m_qSubOperation==PRESCAN_AP_NEXT_DIE_Q		|| 
		m_qSubOperation==PRESCAN_AP_MOVE_TABLE_Q	|| 
		m_qSubOperation==PRESCAN_AP_WAIT_READY_Q	)
	{
		if (m_pWaferMapManager->IsUserRegionPickingMode())
		{
			if (!m_pWaferMapManager->IsAlgorithmSuspended())
			{
				m_pWaferMapManager->SuspendAlgorithmPreparation();
			}
		}
		pApp->m_lBondTabPageSelect = 0; // Change the tab page selection do not show the bin map tab page.
		return RunOperationWft_Scan();
	}

	//if (m_bWaferPrMS60LFSequence)		//v4.47T5
	//{
	return RunOperation_MS60();			//MS60 sequence		
	//}

/*
	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP))
	{
		OpAbort1stDieFinderToPick();

		DisplaySequence("WT - STOP");
		State(STOPPING_Q);
		return ;
	}

	try
	{
		static LONG lTryLimit = 0;
		m_nLastError	= gnOK;
		BOOL bNewCT		= (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["New CT"];	//v2.83T7

		if (m_bStopAllMotion == TRUE)
		{
			if (m_qSubOperation != UPDATE_LAST_DIE_Q)
				m_qSubOperation = HOUSE_KEEPING_Q;

			bNoMotionHouseKeeping = TRUE;
		}

		// Operation state
		switch (m_qSubOperation)
		{
		case DIE1_FINDER_Q:			//v3.15T1
			if (m_bStop)
			{
				OpAbort1stDieFinderToPick();
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (OpCycle1stDieFinderToPick() != gnOK)
			{
				OpAbort1stDieFinderToPick();
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			break;

		case GET_WAFER_POS_Q:		// Get the die position from Wafer Map
			m_nLastError = OpGetWaferPos();
			szMsg.Format("WT - Get Wafer Position, grade = %d", GetCurrDieGrade()-m_WaferMapWrapper.GetGradeOffset());
			DisplaySequence(szMsg);		//v4.50A12

			if ((m_nLastError) == gnOK)
			{
				LONG lWprFatalErr = (*m_psmfSRam)["WaferPr"]["WprFatalErr"];	//	-1 check
				if( m_bAutoSampleAfterPR && lWprFatalErr == -1 )
				{
					BOOL bToHKQ = FALSE;
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					UINT unNoDieExtra = pApp->GetProfileInt(gszPROFILE_SETTING, _T("PR no die extra limit"), 0);
					if( unNoDieExtra==1 )
					{
						bToHKQ	= TRUE;	//	this will not SAMPLE, but alarm and stop, if press start button, do rescan automatically.
					}
					if( GetNewPickCount()<(ULONG)(m_lRescanPECounter + m_lRescanPELimit) )
					{
						bToHKQ	= TRUE;	
					}

					if( IsAutoRescanEnable() && bToHKQ==FALSE )	//	WPR too many no die, let wft to do rescan
					{
						m_lReadyToSampleOnWft = 1;
						(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = -2;		//v3.76
						ChangeNextSampleLimit(0);
						DisplaySequence("WT - PR too many no die, to rescan");
					}	//	PR too many no die, auto rescan
					else if( DEB_IsUseable() && bToHKQ==FALSE )
					{
						(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = -2;
						m_ulNextAdvSampleCount = GetNewPickCount();
						DisplaySequence("WT - PR too many no die, to resample key die");
					}	//	PR too many no die, auto key die sampling

					if( bToHKQ )
					{
						m_bReSampleAsError = TRUE;	//	house keeping, PR fail
						SetAlert_SE_Red_Yellow(IDS_WPR_MAX_NODIE_SKIPCOUNT);
						SetErrorMessage("Too many no die (bToHKQ)");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}

				if ( m_bFirst == TRUE )
				{
					StartTime(GetRecordNumber());

					m_qSubOperation = MOVE_TABLE_Q;
				}
				else
				{
					if ( GetCurrDieEAct() != WAF_CDieSelectionAlgorithm::PICK )
					{
						StartTime(GetRecordNumber());
					}

					OpUpdateNextMotionTime();	//Update next WT motion time for DBH calcuation

					if( IsMapPosnOK() && IsAutoSampleSetupOK() && IsAutoSampleSort() && GetWftSampleState()==0 )
					{	//	auto rescan sorting wafer	prepare into re-scan when get pos.
						WM_CMarkDieRegionManager *pMarkDieRegionManager = WM_CMarkDieRegionManager::Instance();
						pMarkDieRegionManager->SetScanRegionMarkDie(IsAutoRescanAndSampling());
						if (pMarkDieRegionManager->IsScanRegionMarkDie() &&
							GetNextRscnLimit()>0 && GetNewPickCount() >= GetNextRscnLimit())
						{
							szMsg = "WT - MS60 get wafer pos ready to sample offset 0->1";
							CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
							DisplaySequence(szMsg);
							m_lReadyToSampleOnWft = 1;
							Sleep(200);
						}
						else if ( GetNewPickCount()>=GetNextXXXCounter() && GetNextXXXCounter()>0 )
						{
							szMsg = "WT - get wafer pos ready to sample offset 0->1";	// Rescan or DEB or ADV
							CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
							DisplaySequence(szMsg);
							m_lReadyToSampleOnWft = 1;	//	ADV or DEB	//	auto rescan
							Sleep(200);
						}
						else if( IsAutoRescanAndDEB() && GetNextRscnLimit()>0 && GetNewPickCount()>=GetNextRscnLimit() )
						{
							szMsg = "WT - get wafer pos ready to DEB rescan, offset 0->1";	// Rescan or DEB or ADV
							CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
							DisplaySequence(szMsg);
							m_lReadyToSampleOnWft = 1;	//	ADV or DEB	//	auto rescan
							Sleep(200);
						}
					}	// update next motion time, inform BH to prepick

					m_qSubOperation = WAIT_EJECTOR_READY_Q;
				}
			}
			else
			{
				if (!m_bEnableSmartWalk )	//v2.99T1
				{
					OpUpdateDie();			// Update the wafer map
					m_bDieIsGet = FALSE;
				}

				if ( m_nLastError == Err_WTHitSeparationGrade )
				{
					(*m_psmfSRam)["WaferTable"]["Hit SEP Grade"] = 1;
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else if ( m_nLastError == Err_WTBlkLongJump )	//v3.25T16
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					OpGetWaferPos_NotOKHandling(m_nLastError);
					m_qSubOperation = WAFER_END_HANDLING_Q;
				}
			}
			break;


		case WAIT_EJECTOR_READY_Q:	
			DisplaySequence("WT - wait EJECTOR ready q");
			// Temperature Controller	// to check temperature with a given time 5 second 	also check alarm is enable
			if( TC_IsEnable() && TC_IsCheckTmp() && TC_IsOnPower() )
			{
				CTimeSpan stTimeDiff = CTime::GetCurrentTime() - m_stReadTmpTime;
				if( stTimeDiff.GetTotalSeconds()> 5 )
				{
					if( TC_CheckEjectorLimit()!=0 )
					{
						HmiMessage_Red_Yellow("Thermal Ejector Temperature over tolerance limit!");
						SetErrorMessage("Thermal Ejector Temperature over tolerance limit!");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
					m_stReadTmpTime = CTime::GetCurrentTime();
				}
			}

			if( IsBurnIn()==FALSE && TC_CheckAlarmOn() )
			{
				CString szMsg;
				szMsg = "TC heater alarm signal triggered during sorting!";
				HmiMessage_Red_Yellow(szMsg);
				SetErrorMessage(szMsg);
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
#ifdef NU_MOTION
				m_bDieIsGet = TRUE;		//To make sure Update Die is called in HouseKeeping since Get NextDie is already called
#endif
			}
			else if (WaitMoveBack())	// Need to check move back event first
			{
				SetPRStart(FALSE);		// Clear the last PR start event
				SetMoveBack(FALSE);
				OpRollBack();			// Roll back wafer map
				OpRollBack();			// Roll back wafer map
				SetEjectorReady(TRUE, "Wait Ej @ Move back");
				m_bMoveBack = TRUE;
				m_qSubOperation = GET_WAFER_POS_Q;		// Get last position again
				DisplaySequence("WT - Wait Ej ready - wait move back --> Get Wafer Pos");
			}
			else if (WaitEjectorReady())
			{
				SetEjectorReady(FALSE, "6");

TakeTime(LC2);		//v4.47T6

				if ( m_bMoveBack == TRUE )
				{
					m_bMoveBack = FALSE;
				}
				else if (m_bEnableSmartWalk)		//v2.99T1
				{
				}
				else
				{
					if( IsBlkFuncEnable()==FALSE )
						OpUpdateDie();				// Update the wafer map
					m_bDieIsGet = FALSE;
				}

				if( IsAutoSampleSort() && IsAutoSampleSetupOK() && GetWftSampleState()==1 )
				{	//	auto rescan sorting wafer	check ejector ready, as BH at prepick already. into move table q
					Sleep(200);
				}

				OpMS60NGPickPrEmptyDiePos();		//v4.54A5

				m_qSubOperation = MOVE_TABLE_Q;
			}
			else if (m_bGoodDie == FALSE)
			{
				if (IsBHAtPrePick() == 1)
				{
					SetEjectorReady(TRUE, "Wait Ej @ BH at prepick and good die false");
				}
			}
			else if( m_bFirst && IsAutoRescanWafer() && IsAutoSampleSetupOK() )
			{	//	auto rescan sorting wafer	check ejector ready, as BH at prepick already. into move table q
				if (GetWftSampleState()==2 && GetNewPickCount()>=GetNextRscnLimit() )	// before move table to do scan
				{
					DisplaySequence("WT - BH at prepick, into move table directly");
					m_qSubOperation = MOVE_TABLE_Q;
				}
			}	//	auto rescan
			break;


		case MOVE_TABLE_Q:
			szMsg.Format("WT - map (%ld,%ld)%ld pick %d Move Table",
				GetCurrentRow(), GetCurrentCol(), GetCurrDieGrade() - m_WaferMapWrapper.GetGradeOffset(),
				GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK);
			DisplaySequence(szMsg);

			if (m_bMS60NGPickWaitCheckEmptyDie && !WaitWPREmptyCheckDone())	//v4.54A5
			{
				DisplaySequence("WT - wait WPR EMPTY Die result ...");
				break;
			}

			if ( AllowMove() == TRUE )
			{
				// Disable this BINFULL state for MS100 to cause the DBH hangup; actually no need to handle 
				// BINFULL in WT sequence;
#ifndef NU_MOTION
				bFull = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinFull"];
				if ( bFull == TRUE )
				{
					m_qSubOperation = WT_899_WAIT_BIN_FULL_Q;
					break;
				}
#endif
				TakeTime(EV2);			
				SetEjectorVacuum(FALSE);		// Release the vacuum

				//VAC off delay re-added for Excelitas
				LONG lVacOffDelay = (*m_psmfSRam)["WaferTable"]["VacOffDelay"];
				if ( (lVacOffDelay > 0) && (lVacOffDelay < 5000) )	//10000) )	//v4.47T6
				{
					Sleep(lVacOffDelay);
				}

				if( IsAutoSampleSetupOK() )
				{	//	auto rescan sorting wafer	before move table, into prescan state
					if( GetWftSampleState()==1 )
					{	//	if already 1 and not 2, just wait BH at prepick here should wait BH at prepick (at safe).
						Sleep(100);
					}
					LONG lWprFatalErr = (*m_psmfSRam)["WaferPr"]["WprFatalErr"];
					if( lWprFatalErr == -2 )
					{
						m_lReadyToSampleOnWft = 2;
					}	//	PR too many no die, auto rescan

					if( IsAutoRescanEnable() && GetWftSampleState()==2 && GetNewPickCount()>=GetNextRscnLimit() )
					{	//	auto rescan sorting wafer	before move table, into prescan state
						szMsg = "WT - Begin rescan wafer after BH at prepick 2";
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						DisplaySequence(szMsg);
						RescanSortingWafer();
						SaveScanTimeEvent("    WFT: rescan into move table q");
						m_qSubOperation	= PRESCAN_MOVE_TABLE_Q;
						break;
					}	// auto rescan sorting wafer	before move table, into prescan state

					if( DEB_IsUseable() && GetWftSampleState()==2 )
					{	//	auto rescan sorting wafer	before move table, into prescan state
						szMsg = "WT - Begin KS wafer after BH at prepick 2";
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						DisplaySequence(szMsg);
					}	// auto key sampling sorting wafer	before move table, into prescan state
				}

				if( GetNewPickCount()>=1000 && GetNewPickCount()<1005 )
				{
					StoreWaferBonded();
				}

				if (!OpMS60CheckPrEmptyDieResult())		//v4.54A5
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if ((m_nLastError = OpMoveTable()) == gnOK) // move table to target position
				{
					m_bPerformCompensation = TRUE;
					TakeTime(WT2);		// Take Time
					m_qSubOperation = WAIT_PR_DELAY_Q;
				}
				else
				{
					if ( (m_nLastError == Err_WTableDiePitchFailed) ||
						 (m_nLastError == WT_PITCH_ERR_ABS_NEARBY) )
					{
						SetAlert_SE_Red_Yellow(IDS_WT_FAILEXCEEDPITCH);
						m_bPitchError = TRUE;
					}
					else if (m_nLastError == WT_PITCH_ERR_ADV_SAMPLE ||	//	move table
							 m_nLastError == WT_PITCH_ERR_DEB_SAMPLE ||	//	move table
							 m_nLastError == WT_PITCH_ERR_ADV_UPDATE ||	//	move table
							 m_nLastError == WT_PITCH_ERR_SCN_PREDCT)	//	move table
					{
						SetAlert_Red_Yellow(IDS_WT_FAILEXCEEDCROSSPITCH);
						m_bPitchError = TRUE;
					}
					else if (m_nLastError == WT_CLEAN_EJ_PIN_FAILED)
					{
						SetAlert_Red_Yellow(IDS_WT_CLEAN_EJ_PIN_FAILED);
					}
					else if (m_nLastError == HMB_BH_EJTXY_NO_POWER)
					{
						SetErrorMessage("ERROR: WT OpMoveEjectorTableXY fail in AUTOBOND mode");
						SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER);
					}
					else
					{
						SetErrorMessage("Invalid die position OR die outside Wafer Limit");
						SetAlert_Red_Yellow(IDS_WT_INVALID_POS);		// Set Alarm
					}

					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			break;


		case WAIT_PR_DELAY_Q:
			BOOL bPickAction;
			bPickAction = (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK);
			szMsg.Format("WT - Wait PR Delay to pick %d", bPickAction);
			DisplaySequence(szMsg);
			LONG	lTemp;
			LONG lmPRDelay;
			lTemp = (*m_psmfSRam)["WaferMap"]["ComingDieDirection"];
			(*m_psmfSRam)["WaferMap"]["NextDieDirection"] = lTemp;
			lmPRDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];

			if (m_bNewEjectorSequence)		//v3.80
				SetEjectorVacuum(TRUE);

			if( IsLayerPicking() && bPickAction )	//&& IsPrescanEnable() )
			{
				X_Sync();
				Y_Sync();
				T_Sync();
				CMSLogFileUtility::Instance()->WT_GetIdxLog("Layer preheat in wt pr delay");
				IPC_CServiceMessage stMsg;
				BOOL bReturn = TRUE;
				int nConvID = 0;
				Sleep(m_lWaitEjectorUp);
				bReturn = TRUE;
				stMsg.InitMessage(sizeof(BOOL), &bReturn);
				nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_EjtMoveTo", stMsg);

				while(1)
				{
					if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID,stMsg);
						break;
					}
					else
					{
						Sleep(1);
					}	
				}

#define	BA_MOVE_TIME_HALF		34

				CString szTemp;

				long lPrDelay = lmPRDelay;
				long lTotalDelay = lPrDelay + m_lTCUpPreheatTime;
				if( lTotalDelay<=BA_MOVE_TIME_HALF )
				{
					SetPRStart(TRUE);	// trigger bond arm to pick
					Sleep(m_lTCUpPreheatTime);
				}
				else
				{
					if( lPrDelay>BA_MOVE_TIME_HALF )
					{
						Sleep(m_lTCUpPreheatTime);
					}
					else
					{
						Sleep(lTotalDelay-BA_MOVE_TIME_HALF);
						SetPRStart(TRUE);	// trigger bond arm to pick
						Sleep(BA_MOVE_TIME_HALF-lPrDelay);
					}
				}

				bReturn = FALSE;
				stMsg.InitMessage(sizeof(BOOL), &bReturn);
				nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_EjtMoveTo", stMsg);
				if( lPrDelay>BA_MOVE_TIME_HALF )
				{
					Sleep(lPrDelay-BA_MOVE_TIME_HALF);
					SetPRStart(TRUE);	// trigger bond arm to pick
					Sleep(BA_MOVE_TIME_HALF);
				}
				else
				{
					Sleep(lPrDelay);
				}
			}

			LONG lDelay;
			lDelay = lmPRDelay;
			if( lDelay==0 && m_bDisableBH )
				lDelay = 10;

			bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];	

#ifdef NU_MOTION
			//Add extra 10ms for LF cycle since more idle time is available for WT task					
			if (bLFNoComp && (lmPRDelay < 20) && (CMS896AApp::m_lCycleSpeedMode < 4))	//v4.05	//Added MS100+ mode
			{
				lDelay = lmPRDelay + 5;		//10;	//v3.86	
			}

			//Added extra 10ms PR delay for long-travel motion
			if (CMS896AApp::m_lCycleSpeedMode >= 6)		//MS100P3/MS60/MS90
			{
				LONG lWTIndexTime	= (LONG)(*m_psmfSRam)["WaferTable"]["IndexTime"];
				if ( (m_lLongJumpMotTime >= 25) && (lWTIndexTime >= m_lLongJumpMotTime) )
				{
					lDelay = lDelay + m_lLongJumpDelay - 2;
				}
				else
				{
					if ((lDelay - 2) > 0)
					{
						lDelay = lDelay - 2;	//Used to compensate Event delay from WT to WPR CYcleState
					}
				}
			}
			else if (CMS896AApp::m_lCycleSpeedMode >= 4)		//MS100+
			{
				LONG lWTIndexTime	= (LONG)(*m_psmfSRam)["WaferTable"]["IndexTime"];
				if ( (m_lLongJumpMotTime >= 30) && (lWTIndexTime >= m_lLongJumpMotTime) )
				{
					lDelay = lDelay + m_lLongJumpDelay - 3;		//v4.16T3
				}
				else
				{
					if ((lDelay - 2) > 0)
					{
						lDelay = lDelay - 2;	//Used to compensate Event delay from WT to WPR CYcleState
						// to resolve camera blocking in MS100+
					}
				}
			}

			//MS100 speedup - to reduce extra overhead of head-bond/pick delay
			if ( bPickAction && (CMS896AApp::m_lCycleSpeedMode == 3) )
			{
				if( IsThisDieDirectPick() )
				{
					SetWTReady(TRUE);		//Allow BH Z1/2 to move down earilier because no need to wait for compensation  
				}
				else if (IsBurnIn())	// && (CMS896AApp::m_lCycleSpeedMode >= 4))		//v4.28T6	//Make availabe for MS100I BURNIN also
				{
					SetWTReady(TRUE);		//v4.01T1	//Allow 100ms BURNIN cycle even without LF data
				}
			}
#endif

			if( m_lTCUpPreheatTime<0 || IsLayerPicking()==FALSE )
			{
				CMS896AStn::MotionWait(WT_AXIS_X, lDelay, &m_stWTAxis_X);	//v4.44A5
			}

			(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)bPickAction;

//TakeTime(LC3);		//v4.48A4

			//v4.49A4	//Reset events before triggering WPR
			SetDieReady(FALSE);
			SetBadDie(FALSE);
			if ( bPickAction && bLFNoComp )
			{
				m_bGoodDie = TRUE;
			}
			SetWTStable(TRUE);				// Trigger Wafer PR 

			GetEncoderValue();	
			m_stCurrentDie.lX_Enc = GetCurrX();
			m_stCurrentDie.lY_Enc = GetCurrY();
			m_stCurrentDie.lT_Enc = GetCurrT();
			(*m_psmfSRam)["WaferPr"]["WTEncX"] = GetCurrX();	//v4.42T15
			(*m_psmfSRam)["WaferPr"]["WTEncY"] = GetCurrY();	//v4.42T15
			//BLOCKPICK
			m_stPosOnPR.lX_Enc	= GetCurrX();
			m_stPosOnPR.lY_Enc	= GetCurrY();
			m_stPosOnPR.lT_Enc	= GetCurrT();
			m_qSubOperation = WAIT_DIE_READY_Q;

			if( IsScnLoaded() )
			{
				szMsg.Format(", %3d,%3d, Grab %6d,%6d,%6d", GetCurrentRow(), GetCurrentCol(), GetCurrX(), GetCurrY(), GetCurrT());
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			}
			if( (GetTime() - m_dIdleDiffTime)>=5*60*1000 )	//	427TX	1
			{
				m_dIdleDiffTime = GetTime();
				CMSLogFileUtility::Instance()->MS_LogCycleState("running");	//	427TX	1
			}
			if( IsBlkFuncEnable() && m_bFirst==FALSE && m_bLastDieUpdate==FALSE )
			{
				OpUpdateDie();				// Update the wafer map
			}
			break;


		case WAIT_DIE_READY_Q:
			DisplaySequence("WT - wait die ready q");
			if (IsBHStopped())	//v3.67T3
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			// skip wait pr checking for prober
			else if (WaitDieReady() )		// Good Die
			{
				SetDieReady(FALSE);
				if (m_bNewEjectorSequence)		//v3.80
					SetEjectorVacuum(FALSE);	

				// Update current die's PR status as Good value
				m_stCurrentDie.ulStatus =  m_WaferMapWrapper.GetDieState(GetCurrentRow() , GetCurrentCol());

				//Re-update current die pos if confirm search
				m_bConfirmSrch = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CFS"]["Searched"];

				if ( m_bConfirmSrch == TRUE )
				{
					GetEncoderValue();	
					m_stCurrentDie.lX_Enc = GetCurrX();
					m_stCurrentDie.lY_Enc = GetCurrY();
					m_stCurrentDie.lT_Enc = GetCurrT();
				}

				//BlockPick (with regular ref die)
				LONG lRefDieCheck	= (*m_psmfSRam)["DieInfo"]["RefDie"]["Check"];
				if ( IsBlkFunc2Enable() && m_bAutoUpdateRefDieOffset && lRefDieCheck )	//Block2
				{
					LONG lCompen_X = GetDiePROffsetX();
					LONG lCompen_Y = GetDiePROffsetY();

					LONG	lX,lY,lT;
					lX = GetCurrentEncX() + lCompen_X;
					lY = GetCurrentEncY() + lCompen_Y;

					DOUBLE	dThetaAngle = 0;
					dThetaAngle = -(DOUBLE)(GetGlobalT() - m_stCurrentDie.lT_Enc) * m_dThetaRes;
					RotateWaferTheta(&lX, &lY, &lT, dThetaAngle);

					//Update grid die positions around ref-die with offset XY
					LONG lDiePitchX_X	= GetDiePitchX_X(); 
					LONG lDiePitchX_Y	= GetDiePitchX_Y();
					LONG lDiePitchY_X	= GetDiePitchY_X();
					LONG lDiePitchY_Y	= GetDiePitchY_Y();

					if ( (labs(lCompen_X) > (lDiePitchX_X/10)) || (labs(lCompen_Y) > (lDiePitchY_Y/10)) )
					{
						LONG lOffsetX, lOffsetY;
						LONG	lOrigX, lOrigY, lOrigT;

						//Convert original phy position (X,Y,T) to home angle.
						lOrigX = GetCurrentEncX();
						lOrigY = GetCurrentEncY();
						RotateWaferTheta(&lOrigX, &lOrigY, &lOrigT, dThetaAngle);

						//Find Offset between original and compensated phy position under same HOME angle
						lOffsetX = lX - lOrigX;
						lOffsetY = lY - lOrigY;

						m_pBlkFunc2->Blk2UpdateRefDieOffset(lOffsetX, lOffsetY, GetCurrentRow(), GetCurrentCol());
					}
				}

				bool bPickAction = GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK;

				if ( bPickAction )
				{
					m_stCurrentDie.ulStatus = WT_MAP_DS_PICK;
					m_qSubOperation = CHECK_INFO_READ_Q;
					if ( DieIsNoPick(GetCurrentRow(), GetCurrentCol()) == TRUE )		//Check die is marked or not
					{
						m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::ALIGN; 
						ULONG ulDieState = GetMapDieState(GetCurrentRow(), GetCurrentCol());
						if( !IsDieUnpickAll(ulDieState) )
							ulDieState = WT_MAP_DIESTATE_UNPICK;
						m_stCurrentDie.ulStatus = ulDieState;

						m_bPerformCompensation = FALSE;
						m_bGoodDie = FALSE;
						m_bInfoWrite = TRUE;
						SetEjectorReady(TRUE, "Wait Die Ready @ Map Die is NG");
						m_qSubOperation = COMPENSATION_Q;
					}
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					if( pApp->GetCustomerName()==CTM_FINISAR && m_bIfGenWaferEndFile )
					{
						LONG lRow = GetCurrentRow();
						LONG lCol = GetCurrentCol();
						LONG encX = 0, encY = 0;
						double dDieAngle = 0;
						CString szDieScore = "0";
						USHORT usDieState = 0;
						if( GetScanInfo(lRow, lCol, encX, encY, dDieAngle, szDieScore, usDieState) )
						{
							LONG lScore = (LONG)(*m_psmfSRam)["WaferPr"]["DieScore"];
							szDieScore.Format("%d", lScore);
							SetScanInfo(lRow, lCol, encX, encY, dDieAngle, szDieScore, usDieState);
						}
					}	//	for good die die score info
				}
				else
				{
					m_bPerformCompensation = TRUE;
					m_bGoodDie = FALSE;
					m_bInfoWrite = TRUE;
					SetEjectorReady(TRUE, "Wait Die Ready @ Map die is Not Pick");
					m_qSubOperation = COMPENSATION_Q;

					//v2.91T1
					if ( AlertRefDieCheckOnGDStatus() == WT_CHK_REFDIE_STOP )
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}

					if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::CHECK)
					{
						CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Reference Die Check OK");
					}
				}
			}
			else if (WaitBadDie())		// Bad Die
			{
				SetBadDie(FALSE);

				if (m_bNewEjectorSequence)		//v3.80
					SetEjectorVacuum(FALSE);	

				// Update current die's PR status as default value
				m_stCurrentDie.ulStatus = WT_MAP_DS_DEFAULT;
				LONG lRow = GetCurrentRow();
				LONG lCol = GetCurrentCol();

				CString szLog;
				UCHAR ucBadDieGrade = GetCurrDieGrade();
				szLog.Format("Get Bad go on %d,%d,%d ", lRow, lCol, ucBadDieGrade); 

				// Check the die status and set the property die action
				ULONG lDieStatus = (ULONG)(LONG)(*m_psmfSRam)["WaferPr"]["DieResult"];
				if( (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK) ||
					(GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::ALIGN) )
				{
					//v3.41		//For PR status logging purpose only
					WAF_CDieSelectionAlgorithm::WAF_EDieAction eBpAction;
					eBpAction = GetCurrDieEAct();	

					// Update current die's PR status
					m_stCurrentDie.ulStatus = (ULONG)lDieStatus;
					BOOL bIncreaseCounter = TRUE;
					if( IsBlkFunc2Enable() )
					{
						if (m_WaferMapWrapper.GetReader() != NULL)
						{
							UCHAR ucInvalidGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetInvalidBin();
							if( ucBadDieGrade==ucInvalidGrade || m_WaferMapWrapper.GetGrade(lRow, lCol)==ucInvalidGrade )
							{
								bIncreaseCounter = FALSE;
							}
						}
					}

					BOOL bUnpickInfo = FALSE;
					LONG lUpStatus = lDieStatus;
					switch (lDieStatus)
					{
					case WT_MAP_DS_PR_DEFECT: 
						m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::DEFECTIVE;
						bBadDieCompensate = TRUE;
						if( bIncreaseCounter )
						{
							m_ulDefectDieCount++;
							bUnpickInfo = TRUE;
						}
						szLog += "Defect ";
						break;

					case WT_MAP_DS_PR_CHIP:
						m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::DEFECTIVE;
						bBadDieCompensate = TRUE;
						if( bIncreaseCounter )
						{
							m_ulDefectDieCount++;
							bUnpickInfo = TRUE;
						}
						szLog += "Chip ";
						break;

					case WT_MAP_DS_PR_BADCUT:
						m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::DEFECTIVE;
						bBadDieCompensate = TRUE;
						if( bIncreaseCounter )
						{
							m_ulBadCutDieCount++;
							bUnpickInfo = TRUE;
						}
						szLog += "Badcut ";
						break;

					case WT_MAP_DS_PR_ROTATE:
						m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::DEFECTIVE;
						bBadDieCompensate = TRUE;
						if( bIncreaseCounter )
						{
							IncreaseRotateDieCounter();
							bUnpickInfo = TRUE;
						}
						szLog += "Rotate ";
						break;

					case WT_MAP_DS_PR_INK:	
						m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::INKED;
						bBadDieCompensate = TRUE;
						if( bIncreaseCounter )
						{
							m_ulDefectDieCount++;
							bUnpickInfo = TRUE;
						}
						szLog += "Ink ";
						break;

					case WT_MAP_DS_PR_EMPTY:	
						m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::MISSING;
						bBadDieCompensate = FALSE;
						if( bIncreaseCounter )
						{
							IncreaseEmptyDieCounter();
							bUnpickInfo = TRUE;
						}
						szLog += "Empty ";
						break;

					default:
						//m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::INVALID;
						m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::MISSING;	//v4.43T7
						bBadDieCompensate = FALSE;
						szLog.Format("Andrew: WAIT_DIE_READY_Q encounters INVALID die = %ld", lDieStatus);
						SetErrorMessage(szLog);		//v4.43T7
						if( bIncreaseCounter )
						{
							IncreaseEmptyDieCounter();
							lUpStatus = WT_MAP_DIESTATE_EMPTY;
							bUnpickInfo = TRUE;
						}
						szLog += "Invalid ";
						break;
					}

					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					if( bUnpickInfo && pApp->GetCustomerName()==CTM_FINISAR && m_bIfGenWaferEndFile )
					{
						LONG encX = 0, encY = 0;
						double dDieAngle = 0;
						CString szDieScore = "0";
						USHORT usDieState = 0;
						if( GetScanInfo(lRow, lCol, encX, encY, dDieAngle, szDieScore, usDieState) )
						{
							LONG lScore = (LONG)(*m_psmfSRam)["WaferPr"]["DieScore"];
							szDieScore.Format("%d", lScore);
							SetScanInfo(lRow, lCol, encX, encY, dDieAngle, szDieScore, usDieState);
						}
						WriteUnpickInfo(lUpStatus, lRow, lCol);
					}	//	for unpick info

					//v3.41	//PR logging for each bad-die status
					if ( eBpAction == WAF_CDieSelectionAlgorithm::PICK )
						szLog += "FAIL,PICK ";
					else if ( eBpAction == WAF_CDieSelectionAlgorithm::ALIGN )
						szLog += "FAIL,ALGN ";
					else
						szLog += "error,UNKNOWN ";

					CString szTemp;
					szTemp.Format(" %d, ", m_stCurrentDie.ulStatus);
					szLog += szTemp;
					if( bBadDieCompensate )
					{
						szLog += "No Comm ";
					}

					if ( IsBlkFunc2Enable()	&& GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::DEFECTIVE )
					{
					}
					else
					{
						//Check die is marked or not
						if ( DieIsNoPick(lRow, lCol) == TRUE )
						{
							m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::ALIGN; 
							ULONG ulDieState = GetMapDieState(lRow, lCol);
							if( !IsDieUnpickAll(ulDieState) )
								ulDieState = WT_MAP_DIESTATE_UNPICK;
							m_stCurrentDie.ulStatus = ulDieState;
						}
					}
					(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"]	= GetCurrDieEAct();	//v4.15T2
				}
				else if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::CHECK )
				{
					szLog += "Get Bad Check ";

					//Alert reference die status
					if ( AlertRefDieStatus() == WT_CHK_REFDIE_STOP )
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}
				else 
				{
					m_stCurrentDie.ulStatus = (ULONG)lDieStatus;
					szLog += "Not Check+Pick+Align ";
				}

				CMSLogFileUtility::Instance()->WT_GetIdxLog(szLog);

				m_bGoodDie = FALSE;

				SetCompDone(TRUE);
				if (bBadDieCompensate == FALSE)
				{
					DisplaySequence("WT - die ready to Wait LF Ready");
					m_qSubOperation = WAIT_LF_READY_Q;
				}
				else
				{
					m_bPerformCompensation = FALSE;
					m_qSubOperation = BADDIE_COMPENSATION_Q;
				}

				//Re-update current die pos if confirm search
				m_bConfirmSrch = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CFS"]["Searched"];
				if ( m_bConfirmSrch == TRUE )
				{
					GetEncoderValue();	
					m_stCurrentDie.lX_Enc = GetCurrX();
					m_stCurrentDie.lY_Enc = GetCurrY();
					m_stCurrentDie.lT_Enc = GetCurrT();
				}

				bSpRefGradeCheckResult = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["RefGradeCheck"]["Result"];

				if (bSpRefGradeCheckResult == FALSE )
				{
					// Reset it
					(*m_psmfSRam)["WaferPr"]["RefGradeCheck"]["Result"] = TRUE;
					SetAlert_Red_Yellow(IDS_WT_REFDIE_FAIL);
					SetErrorMessage("Special Reference Grade Check Failed");
					CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Reference Die 2 Check Failed");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			break;


		case CHECK_INFO_READ_Q:
			DisplaySequence("WT - Check Die Info Read");

			// must be pick die as it is set from
			// Check whether the die info is read
			if (WaitDieInfoRead() && WaitDieUnReadyForBT() )	//	V450X16
			{
				SetDieInfoRead(FALSE);
				OpStoreDieInfo();
				SetDieReadyForBT(TRUE);
				m_bInfoWrite = TRUE;
				DisplaySequence("WT - Check Die Info Read done");
			}
			else
			{
				m_bInfoWrite = FALSE;
			}
			m_qSubOperation = COMPENSATION_Q;
			break;

		case COMPENSATION_Q:
			DisplaySequence("WT - Compensation");

			if ( AllowMove() == TRUE )
			{
				if (!m_bPreCompensate)	//No need to do compensation if already pre-compensated		//v2.96T3
				{
					TakeTime(CP1);
					m_nLastError = OpCompensation();
					
				}
				else
					m_bPreCompensate = FALSE;

				if (m_nLastError == gnOK)
				{
					m_qSubOperation = WAIT_COMPENSATION_COMPLETE_Q;
				}
				else
				{
					// Fail to perform compensation
					if (m_nLastError == Err_WTableDiePitchFailed)
					{
						SetAlert_SE_Red_Yellow(IDS_WT_FAILEXCEEDPITCH);
						m_bPitchError = TRUE;
					}
					else if (m_nLastError == WT_PITCH_ERR_ADV_UPDATE)	//	compensation
					{
						SetAlert_Red_Yellow(IDS_WT_FAILEXCEEDCROSSPITCH);
						m_bPitchError = TRUE;
					}
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			break;


		case WAIT_COMPENSATION_COMPLETE_Q:
			DisplaySequence("WT - Wait Compensation Complete");
			lPickTime_Z = (*m_psmfSRam)["BondHead"]["Z_PickTime"];

			if ( m_lReadyDelay > lPickTime_Z )	//	m_lReadyDelay is 10 at most case and lPickTime_Z is 25 (bh dn
			{
				// Wait the motion complete and then wait
				OpMoveComplete();
				TakeTime(CP2);			// Take Time
				if( m_bMapWaferVerifyOK )
					SetCompDone(TRUE);		//v4.34T10
				if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
				{
					if( !IsLayerPicking() )
						SetEjectorVacuum(TRUE);			// Turn-on Ejector Vacuum after comp.
					TakeTime(EV1);	

					Sleep(m_lReadyDelay - lPickTime_Z);

					if (OpCheckSortingCheckPoints() != 1)
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
#ifdef NU_MOTION						
					if (CMS896AApp::m_lCycleSpeedMode >= 3)		//v3.89	//MS100 speedup to 100ms
					{
						if( IsThisDieDirectPick()==FALSE && m_bMapWaferVerifyOK )
						{
							SetWTReady(TRUE);			// Trigger BH to PICK
						}
					}
					else
#endif
						if( m_bMapWaferVerifyOK )
							SetWTReady(TRUE);				// Trigger BH to PICK
				}
			}
			else
			{
				//v2.96T3
				//Calculate total Pre-Compensation time
				BOOL dPreCompTotalTime = 0;

				if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
				{
#ifdef NU_MOTION		
					if (CMS896AApp::m_lCycleSpeedMode >= 4)		//v4.10T6
					{
						//v4.28T6	//Changed from 17 to 12
						m_lReadyDelay = 20; //12;	//17;	//-> at least 12ms settling, assume BH & WT reach at the same time

						//Add more delay for WT so it has enough settling time before BH to PICK level
						if ( (m_lTime_CP + m_lReadyDelay) > lPickTime_Z )
							Sleep(m_lTime_CP + m_lReadyDelay - lPickTime_Z);

						if (CMS896AApp::m_lCycleSpeedMode >= 6)		//MS100P3 only
						{
							//For DBH error problem found in TJ Sanan by Long;
							//Reset once more here
							SetEjectorReady(FALSE, "10");		//v4.48A22
						}
					}
					else
#endif
						if (bNewCT)		//v2.83T7
						{
							if ( (m_lTime_CP - dPreCompTotalTime) > (lPickTime_Z + 5) )	
							{
								Sleep(m_lTime_CP - dPreCompTotalTime - lPickTime_Z - 5);
							}
						}
						else
						{
							if ( (m_lTime_CP + m_lReadyDelay) > lPickTime_Z )
								Sleep(m_lTime_CP + m_lReadyDelay - lPickTime_Z);
						}

						if (OpCheckSortingCheckPoints() != 1)
						{
							m_qSubOperation = HOUSE_KEEPING_Q;
							break;
						}
#ifdef NU_MOTION		
						if (CMS896AApp::m_lCycleSpeedMode >= 3)		//v3.89	//MS100 speedup to 100ms
						{
							if( IsThisDieDirectPick()==FALSE )
							{
								BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
								if (bLFNoComp && (CMS896AApp::m_lCycleSpeedMode == 5))
								{
									//Already set to TRUE in Op MoveTable(); no need to set again
								}
								else if( m_bMapWaferVerifyOK )
								{
									SetWTReady(TRUE);
								}
							}
						}
						else
#endif
							if( m_bMapWaferVerifyOK )
								SetWTReady(TRUE);				// Trigger BH to PICK
				}
				OpMoveComplete();
				TakeTime(CP2);
				if( m_bMapWaferVerifyOK )
					SetCompDone(TRUE);		//v4.34T10
				if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
				{
					if( !IsLayerPicking() )
						SetEjectorVacuum(TRUE);			// Turn-on Ejector Vacuum after comp.
					TakeTime(EV1);
				}
				Sleep(15); //wafer table encoder settling, it is used for next die
			}

			m_bGoodDie = TRUE;
			if ( m_bInfoWrite == TRUE )				// Check whether the die info is written
			{
				m_qSubOperation = WAIT_LF_READY_Q;
			}
			else
			{
				m_qSubOperation = WAIT_INFO_READ_Q;
				DisplaySequence("WT - COMP COMPLETE to WAIT Die Info Read");
			}
			break;


		case BADDIE_COMPENSATION_Q:
			DisplaySequence("WT - BadDie Compensation");
			if ( AllowMove() == TRUE )
			{
				if (!m_bPreCompensate)	//No need to do compensation if already pre-compensated	
				{
					m_nLastError = OpCompensation(FALSE);	//v4.48A25	//Added bGoodDie parameter for EjtXY config
				}
				else
					m_bPreCompensate = FALSE;

				if (m_nLastError == gnOK)
				{
					OpMoveComplete();	// Wait table move complete
					DisplaySequence("WT - bad comp to Wait LF Ready");
					m_qSubOperation = WAIT_LF_READY_Q;
				}
				else
				{
					if (m_nLastError == Err_WTableDiePitchFailed)
					{
						SetAlert_SE_Red_Yellow(IDS_WT_FAILEXCEEDPITCH);
						m_bPitchError = TRUE;
					}
					else if (m_nLastError == WT_PITCH_ERR_ADV_UPDATE)	//	bad die compensation
					{
						SetAlert_Red_Yellow(IDS_WT_FAILEXCEEDCROSSPITCH);
						m_bPitchError = TRUE;
					}

					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			break;

		case WAIT_INFO_READ_Q:
			DisplaySequence("WT - wait info read q");
			if (IsBHStopped())
			{ 
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if ( GetCurrDieEAct() != WAF_CDieSelectionAlgorithm::PICK )
			{
				m_qSubOperation = WAIT_LF_READY_Q;
			}
			else if (WaitDieInfoRead()&& WaitDieUnReadyForBT() )	//	V450X16
			{
				SetDieInfoRead(FALSE);
				OpStoreDieInfo();
				SetDieReadyForBT(TRUE);

				m_qSubOperation = WAIT_LF_READY_Q;
				DisplaySequence("WT - wait info read q done");
			}
			break;

		case WAIT_LF_READY_Q:
			DisplaySequence("WT - Wait LF Ready");
			if (IsBHStopped())
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (WaitLFReady())
			{
				SetLFReady(FALSE);
				OpGetLFResult();
				m_qSubOperation = WT_NEXT_DIE_Q;
			}
			break;	

		case WT_NEXT_DIE_Q:
			DisplaySequence("WT - Next Die");
			m_bFirst = FALSE;
			if ((m_nLastError = OpNextDie()) == gnOK)
			{
				m_bDieIsGet = TRUE;
				m_qSubOperation = GET_WAFER_POS_Q;

				if (m_bEnableSmartWalk)	
				{
					////////////////////////////////////////////////////////////
					// Call Update() immediately after Get NextDie in , such that Smart-Walk
					// can calculate next posn; must call OpLFDieStatus ForSmartWalk()
					// such that Smart-Walk can calculate the new path based on new LF status;
					OpLFDieStatusForSmartWalk();
					OpUpdateDie();
					m_bDieIsGet = FALSE;
				}
			}
			else
			{
				m_stLastDie = m_stCurrentDie;
				CMSLogFileUtility::Instance()->WT_GetIdxLog("OP NEXT fail last restore to current");

				if( m_nLastError==WT_PITCH_ERR_ADV_UPDATE )	//	next die
				{
					SetAlert_Red_Yellow(IDS_WT_FAILEXCEEDCROSSPITCH);
					m_bPitchError = TRUE;
					DisplaySequence("WT - WFT next  die => house keeping q");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					OpUpdateDie();		// Update the wafer map
					m_bDieIsGet = FALSE;
					m_qSubOperation = WAFER_END_HANDLING_Q;
					DisplaySequence("WT - WFT next  die => wafer end q");
				}
			}
			break;

		case WAFER_END_HANDLING_Q:
			if( IsMS90() && !m_bFirst && (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK) )
			{
				BOOL bFirstPickDie = (BOOL)(LONG) ((*m_psmfSRam)["BondHead"]["First Pick Die"]);
				if( !m_bStop && !bFirstPickDie )
				{
					break;
				}
			}
			SaveScanTimeEvent("    WFT: all done into wafer end state");
			DisplaySequence("WT - Wafer End");

			remove("c:\\mapsorter\\userdata\\history\\WaferPrMsd.txt");

			// sort mode, 1st part wafer end
			if (Is2PhasesSortMode() && m_b2Parts1stPartDone == FALSE)
			{
				// now not the real half part end, but region end.
				BOOL bIs1stHalfPartEnd = TRUE;
				if (IsOnlyRegionEnd())
				{
					if (pSRInfo->IsRightAllRegionSorted(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(), GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol()))
					{
						bIs1stHalfPartEnd = TRUE;
						if (pSRInfo->IsManualAlignRegion())
						{
							if (HmiMessage_Red_Back("Is 1st Half Wafer Finish", "Confirm", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
							{
								bIs1stHalfPartEnd = TRUE;
							}
							else
							{
								bIs1stHalfPartEnd = FALSE;
							}
						}
					}
					else
					{
						bIs1stHalfPartEnd = FALSE;
					}
				}
				
				if (bIs1stHalfPartEnd)
				{
					if (IsRowModeSeparateHalfMap() && ((LONG)GetMS90HalfMapMaxRow() > GetMapValidMaxRow()))
					{
						DisplaySequence("WT - all done after 2nd wafer end");
						m_b2PartsAllDone = TRUE;
					}
					else if (!IsRowModeSeparateHalfMap() && ((LONG)GetMS90HalfMapMaxCol() > GetMapValidMaxCol()))
					{
						DisplaySequence("WT - all done after 2nd wafer end");
						m_b2PartsAllDone = TRUE;
					}
					else
					{
						DisplaySequence("WT - in wafer end handling, 1st part ended");
						MS90Set1stDone(TRUE);
						m_bSortGoingTo2ndPart = TRUE;
						SetWTReady(FALSE);
					}
				}
			}
			else
			{
				if (!pUtl->GetPrescanRegionMode() || pSRInfo->IsRightAllRegionSorted(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(), GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol()))
				{
					DisplaySequence("WT - all done after 2nd wafer end");
					m_b2PartsAllDone = TRUE;
				}
				else
				{
					SetWTReady(FALSE);
				}
			}

			if (m_bStop)
			{
				SaveScanTimeEvent("    WFT: stop into house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else
			{
				LONG lX	= (LONG)(*m_psmfSRam)["BinTable"]["Die Bonded X"];
				LONG lY	= (LONG)(*m_psmfSRam)["BinTable"]["Die Bonded Y"];
				UpdateWaferLotLoadStatus();

				BOOL bWaferEnded = FALSE;
#ifdef NU_MOTION
				//Cannot check BT last-die, else may dead-lock		//v3.68T5
				if (!pSRInfo->IsRegionEnd() && IsAllSorted())
				{
					bWaferEnded = TRUE;
				}
				DisplaySequence("WT - wafer end to house keeping");
				SaveScanTimeEvent("    WFT: wafer end into house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
#else
				// temp to disable it as it will create last die not bond bug
				if ( (GetLastDieRow() == lY) && (GetLastDieCol() == lX) )
				{
					// sort mode, real wafer end, all sorted
					if (!pSRInfo->IsRegionEnd() && IsAllSorted() )
					{
						CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End Display 1");
						bWaferEnded = TRUE;
					}
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					if ((GetLastDieEAct() != WAF_CDieSelectionAlgorithm::PICK) || m_bIsWaferEnded || pSRInfo->IsRegionEnd())
					{
						// sort mode, real wafer end, all sorted
						if (!pSRInfo->IsRegionEnd() && IsAllSorted() )
						{
							CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End Display 2");
							bWaferEnded = TRUE;
						}
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
				}
#endif
				if( bWaferEnded )
				{
					SaveScanTimeEvent("    WFT: to display wafer end");
					OpDisplayWaferEnd();
				}
			}

			if( (!IsOnlyRegionEnd()) && IsAllSorted() )
			{
				DisplaySequence("WT - set wafer end psmf_sram");
				(*m_psmfSRam)["WaferTable"]["WaferEnd"] = TRUE;
			}
			break;

		case WT_899_WAIT_BIN_FULL_Q:
			DisplaySequence("WT - Wait Bin Full");

			// Wait for bin full alert issued by Bin Table Station
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else
			{
				bBinLoader = IsBLEnable();
				if ( m_bStopBinLoader == TRUE )
					bBinLoader = FALSE;			
				if ( bBinLoader == TRUE )
				{
					bFull = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinFull"];
					if ( bFull == FALSE )
					{
						SetEjectorReady(TRUE, "899 Wait Bin Full");
						SetBPRLatched(TRUE);
						m_qSubOperation = GET_WAFER_POS_Q;
					}
				}
				Sleep(1);
			}
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("WT - House Keeping Q");

			if( lTryLimit==0 )
			{
				//szMsg.Format("WT - HKQ picked die %lu", GetNewPickCount());
				//DisplaySequence(szMsg);	//v4.59A31
				SetAutoCycleLog(FALSE);
				pUtl->SetPickDefectDie(FALSE);

				m_stStopTime = CTime::GetCurrentTime();	// machine sorting stopped
				m_stMachineStopTime = m_stStopTime;		//v4.59A41

				// sort mode, to align 2nd part wafer, 1st wafer end occured
				if( IsSortTo2ndPart() )
				{
					DisplaySequence("WT - house keeping, 1st end, begin to align 2nd part wafer");
					SetAlignmentStatus(FALSE);
					ClearPrescanRawData(FALSE, TRUE);	// region prescan, region sorting complete
					(*m_psmfSRam)["WaferTable"]["WaferFirstPartDone"] = TRUE;	// 1st part end
				}

				if (GetHouseKeepingToken("WaferTableStn") == TRUE)
				{
					SetHouseKeepingToken("WaferTableStn", FALSE);
				}
				else
				{
					Sleep(10);
					break;
				}

				SaveScanTimeEvent("    WFT: to stop wafer table cycle");
				// prescan relative code
				StopCycle("WaferTableStn");
				Result(gnNOTOK);

				//Disable 1st die finder if machine stop
				OpAbort1stDieFinderToPick();
				SetHouseKeepingToken("WaferTableStn", TRUE);

				if( IsBLInUse() )
				{
					SaveScanTimeEvent("    WFT: down BL again to sure it is safe");
					MoveES101BackLightZUpDn(FALSE);	// in HOUSE KEEPING, DOWN BL Z
				}

				if( m_bPitchError )
				{
					m_bPitchAlarmOnce = TRUE;
					m_ulPitchAlarmCount++;
					if( GetNewPickCount()>0 && m_bAutoSampleAfterPE )
					{
						ChangeNextSampleLimit(0);
						m_bReSampleAsError	= TRUE;
					}
				}

				if( IsAutoRescanWafer() && GetNewPickCount()>0 && pUtl->GetPrAbnormal() )	//	rescan unfinished because of error, redo it.
				{
					SaveScanTimeEvent("WFT: rescan error and stopped.");
					m_bReSampleAsError = TRUE;	//	house keeping, rescan fail
					ChangeNextSampleLimit(0);
				}

				if( m_bPitchError == TRUE && DEB_IsUseable() )
				{
					OpDebPitchErrorBackup();
				}	// picth error and copy log files to Deb result folder.

				OpBackupLogFiles();
			}

			//v4.02T1	//Moved to after so that DBH can reach HouseKeeping 
			// state to home Ej & BHZ before WT to UNLOAD during WaferEnd!!
			if( bNoMotionHouseKeeping == FALSE )	//	4.24TX 4
			{
				if( lTryLimit==0 )
				{
					LogCycleStopState("WT - waiting BH at safe");
				}
				//Must wait DBH to HouseKeeping to make sure Ej at safe pos before moving WT
				if( IsBHStopped()!=1 )
				{
					Sleep(100);
					lTryLimit++;
					if( lTryLimit<30000 )	// over 3000 seconds
					{
						break;
					}
				}

				if (m_bIsWaferEnded)
				{
					LONG lBTStop		= (*m_psmfSRam)["BinTable"]["AtSafePos"];
					if (lBTStop != 1)
					{
						Sleep(100);
						lTryLimit++;
						if( lTryLimit<30000 )	// over 3000 seconds
						{
							break;
						}
					}
				}

				LogCycleStopState("WT - after BH at safe");

				SetEjectorVacuum(FALSE);		// Release the vacuum after bh at safe
				Sleep(100);

				if( pUtl->GetPrescanRegionMode() && (pSRInfo->IsRegionEnd() || m_bIsWaferEnded) )
				{
					BOOL bReferDie = TRUE;
					if (IsRegionAlignRefBlk() == FALSE)
					{
						SetAlignmentStatus(FALSE);	// based on option, for block pick or refer die in region no need reset
					}

					if (pSRInfo->IsManualAlignRegion())
					{
						bReferDie = FALSE;
						//reset "m_bRegionAligned"
						//if there has not reference die in the next region and m_bManualRegionScanSort is TRUE,
						//it should reset "m_bManualAlignRegion" and "m_bWaferAlign"
						CheckResetManualAlignRegion();
					}
					else
					{
						if( IsRegionAlignRefBlk() )
						{
							pUtl->RegionOrderLog("REGN END reverify refer");
							bReferDie = ReverifyBlockPickReferDie();
							if( bReferDie==FALSE )
							{
								pUtl->RegionOrderLog("REGN END reverify refer over FAILURE");
							}
							pUtl->RegionOrderLog("REGN END reverify refer over");
						}
						if (pSRInfo->IsRegionManualConfirm())
						{
							bReferDie = FALSE;
							pSRInfo->SetRegionManualConfirm(FALSE);
						}
					}

					ClearPrescanRawData(FALSE, bReferDie);	// region prescan, region sorting complete
					if( bReferDie && IsOnlyRegionEnd() )
						(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"] = TRUE;
					pUtl->RegionOrderLog("REGN wafer house keeping Q: after clear prescan data");
				}	// end if region mode

				OpUpdateMapViaPR();

				if( IsPrescanEnded() || IsPrescanEnable()==FALSE )
				{
					OpRotateBack();
				}


				if (m_bIsWaferEnded)	//	4.24TX 4
				{
					DisplaySequence("WFT: wafer end, move table to unload");
					OpDisplayWaferEndAlarm();
					OpWaferEndMoveToUnload();
				}
			}

			(*m_psmfSRam)["WaferTable"]["AtSafePos"] = 1;	//v3.67T3

			CMSLogFileUtility::Instance()->MS_LogCycleState("stop sorting");	//	427TX	1
			if( IsPrescanning() )	//	4.24TX 4
			{
				SaveScanTimeToFile(GetNewPickCount());
			}

			m_qSubOperation = UPDATE_LAST_DIE_Q;
			lTryLimit = 0;
			bBTStopped = TRUE;
			DisplaySequence("WT - House Keeping ==> Update Last Die");
			break;

		case UPDATE_LAST_DIE_Q:
			DisplaySequence("WT - update last die");
			if( bBTStopped && IsOnlyRegionEnd() )
			{
				bBTStopped = FALSE;
				State(STOPPING_Q);
				SaveScanTimeEvent("WFT: - to trigger region stop command");
			}	// let trigger the stop early

			if ( CMS896AStn::m_lBinTableStopped == 1 )
			{
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				LogCycleStopState("WT - Update Last Die");
				CMSLogFileUtility::Instance()->WT_GetIdxLog("WT Update last die");

				if ( m_bDieIsGet == TRUE )
				{
					CMSLogFileUtility::Instance()->WT_GetIdxLog("WT update die at stop, need to check, maybe update twice for same die");
					OpUpdateDie();

					//v3.84		//For logging purpose only
					WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
					ULONG ulRow, ulCol;
					UCHAR ucGrade;
					BOOL bEndOfWafer;
					CMSLogFileUtility::Instance()->WT_GetIdxLog("WT peek die at stop");
					PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);

					LONG lX = (LONG)(*m_psmfSRam)["BinTable"]["Die Bonded X"];
					LONG lY = (LONG)(*m_psmfSRam)["BinTable"]["Die Bonded Y"];
					if ( (GetLastDieRow() != lY) || (GetLastDieCol() != lX) )
					{
						OpRollBack();
#ifdef NU_MOTION
						//v3.84		//MS100 DBH only
						//May require double-Rollback, by checking if LastDie is equal to next PeekDie or not
						//** Next PeekDie must be the same die as LastDie for next AUTOBOND cycle **
						CMSLogFileUtility::Instance()->WT_GetIdxLog("WT peek die check at stop");
						PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
						//Rollback once more if LastDIe not matched in DBH config
						if ((GetLastDieRow() != ulRow) || (GetLastDieCol() != ulCol) && IsBlkFunc2Enable()==FALSE )
							OpRollBack();
#endif
					}

					//v3.84		//For logging purpose only
					CMSLogFileUtility::Instance()->WT_GetIdxLog("WT peek die again at stop");
					PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
				}

				m_szNextLocation = "";
				if( m_bPitchError == TRUE )
				{
					ULONG ulRow, ulCol;
					UCHAR ucGrade;
					BOOL bEndOfWafer;
					WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
					PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
					m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
					m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
					LONG lUserRow = 0, lUserCol = 0;
					ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
					m_szNextLocation.Format("%ld,%ld", lUserRow, lUserCol);
					if( DEB_IsUseable() )
					{
						if( m_ulPrescanGoCheckLimit>0 )
							m_ulPitchAlarmGoCheckLimit = m_ulPrescanGoCheckLimit;
						else
							m_ulPitchAlarmGoCheckLimit = 5;
						m_ulPitchAlarmGoCheckCount = 0;
						m_aUIGoCheckRow.RemoveAll();
						m_aUIGoCheckCol.RemoveAll();
					}
				}

				//v3.76		//PLLM REBEL
				//reset wafer alignment status if any PR SKIP die error is triggered on WPR station
				BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
				if( bPLLMSpecialFcn )
				{
					LONG lWprFatalErr = (*m_psmfSRam)["WaferPr"]["WprFatalErr"];
					if (lWprFatalErr > 0)
					{
						SetAlignmentStatus(FALSE);
					}
				}

				if( m_bVerifyMapWaferFail && IsAdvRegionOffset() )
				{
					ULONG ulRow = 0, ulCol = 0;
					if( ConvertOrgUserToAsm(m_lVerifyMapWaferRow, m_lVerifyMapWaferCol, ulRow, ulCol) )
					{
						m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
						m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
						LONG lScanX = 0, lScanY = 0;
						if( GetPrescanWftPosn(ulRow, ulCol, lScanX, lScanY) )
						{
							LONG encX = 0, encY = 0;
							if( GetRegionDiePosition(ulRow, ulCol, lScanX, lScanY, encX, encY) )
							{
								XY_SafeMoveTo(encX, encY);
								CString szMsg = "PCI Pitch error, verification fail, please check on map.";
								HmiMessage_Red_Back(szMsg, "Auto Sort");
							}
						}
						Sleep(200);
						m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
					}
				}	// end to display verify fail position

				UpdateSMS(m_bIsWaferEnded);

				LogCycleStopState("WT - Stop completely");
				State(STOPPING_Q);
			}
			else
			{
				Sleep(10);
			}
			break;

		default:
			DisplaySequence("WT - Unknown");
			m_nLastError = glINVALID_STATE;
			break;
		}

		if ( m_bHeapCheck == TRUE )
		{
			// Check the heap
			INT nHeapStatus = _heapchk();
			if ( nHeapStatus != _HEAPOK )
			{
				CString szMsg;
				szMsg.Format("WT - Heap corrupted (%d) before WT [%s]", nHeapStatus, 
					g_aszWTState[m_qSubOperation]);
				DisplayMessage(szMsg);

				LogCycleStopState(szMsg);
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		Result(gnNOTOK);

		State(STOPPING_Q);

		NeedReset(TRUE);
		StopCycle("WaferTableStn");
		SaveScanTimeEvent("    WFT: exception executed");
		(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
	}
*/
}
