/////////////////////////////////////////////////////////////////
// WT_CycleState_MS60.cpp : Cycle Operation State of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tue, June 24, 2014
//	Revision:	1.00
//
//	By:			Andrew Ng
//
//	Copyright @ ASM Assembly Automation Ltd., 2014.
//	ALL rights reserved.
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
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//if next cycle do auto-rescan or clean ejector pin and wafer table must will leave the current position,
//require that BHT move to the prepick position
VOID CWaferTable::CheckNextCycleBHTMovetoPrepick()
{
	if (m_pEjPinCleanRegion->IsReachCleanDirtEJPin() && (GetWftCleanEjPinState() == 0))
	{
		CString szMsg;

		szMsg = "WT - MS60 get wafer pos ready to Clean Ejector Pin 0->1";
		m_lReadyToCleanEjPinOnWft = 1;
		//trigger BHZ to move to pre-pick position after pick the curretn die
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
		CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		DisplaySequence(szMsg);
	}
	else if (IsAutoSampleSetupOK() && IsMapPosnOK() && IsAutoSampleSort() && (GetWftSampleState() == 0))
	{
		CString szMsg;

		WM_CMarkDieRegionManager *pMarkDieRegionManager = WM_CMarkDieRegionManager::Instance();
		pMarkDieRegionManager->SetScanRegionMarkDie(IsAutoRescanAndSampling());
		if (GetNextXXXCounter() > 0 && GetNewPickCount()>=GetNextXXXCounter() )
		{
			szMsg = "WT - 1MS60 get wafer pos ready to sample offset 0->1";
			m_lReadyToSampleOnWft = 1;	// ADV or DEB or auto rescan or sampling
		}
		else if (pMarkDieRegionManager->IsScanRegionMarkDie() &&
				 GetNextRscnLimit()>0 && GetNewPickCount() >= GetNextRscnLimit())
		{
			szMsg = "WT - 2MS60 get wafer pos ready to sample offset 0->1";
			m_lReadyToSampleOnWft = 1;
		}
		else if( IsAutoRescanAndDEB() && GetNextRscnLimit()>0 && GetNewPickCount()>=GetNextRscnLimit() )
		{
			szMsg = "WT - MS60 get wafer pos ready to DEB rescan, offset 0->1";
			m_lReadyToSampleOnWft = 1;	//	auto rescan
		}

		if (m_lReadyToSampleOnWft == 1)
		{
			//trigger BHZ to move to pre-pick position after pick the curretn die
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);
			DisplaySequence(szMsg);
		}
	}
}


VOID CWaferTable::CheckResetLFCycle()
{
	if (IsAutoSampleSort() && IsAutoSampleSetupOK() && (GetWftSampleState() >= 1))
	{	
		//	auto rescan sorting wafer	check ejector ready, as BH at prepick already. into move table q
		if (GetWftSampleState() == 1)
		{
			Sleep(200);
		}

		CString szMsg;
		WM_CMarkDieRegionManager *pMarkDieRegionManager = WM_CMarkDieRegionManager::Instance();
		if (pMarkDieRegionManager->IsScanRegionMarkDie() &&
			GetNextRscnLimit()>0 && GetNewPickCount() >= GetNextRscnLimit())
		{
			szMsg.Format("MS60 Cycle Mark die sampling reset to #%d", m_nMS60CycleCount);
		}
		else
		{
			szMsg.Format("MS60 Cycle Rescan reset to #%d", m_nMS60CycleCount);
		}

		ResetLFCycle();
		CMSLogFileUtility::Instance()->MS60_Log(szMsg);
	}
	else if (m_pEjPinCleanRegion->IsReachCleanDirtEJPin() && (GetWftCleanEjPinState() >= 1))
	{
		//	auto rescan sorting wafer	check ejector ready, as BH at prepick already. into move table q
		if (GetWftCleanEjPinState() == 1)
		{
			Sleep(200);
		}

		CString szMsg;
		szMsg.Format("MS60 Cycle Clean EJ Pin reset to #%d", m_nMS60CycleCount);

		ResetLFCycle();
		CMSLogFileUtility::Instance()->MS60_Log(szMsg);
	}
}


VOID CWaferTable::ResetLFCycle()
{
	//After rescan wafer or clean ejector pin, table should move to prescan position instead of current die's last position
	m_stLFDie.bGoodDie = FALSE; // current die will not use lookahead mode in move table function
	
	//turn off new LF function
	m_bMS60NewLFInLastCycle = FALSE;
	m_bUseMS60NewLFCycle = FALSE;
	(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"] 		= FALSE;

	//Long-Jump to next PICK die
	m_nMS60CycleCount = 0;	
	(*m_psmfSRam)["WaferMap"]["New LF Counter"] = (LONG)m_nMS60CycleCount;
}


VOID CWaferTable::RunOperation_MS60()
{
	LONG	lPickTime_Z;
	BOOL	bFull = FALSE;
	BOOL    bBinLoader=FALSE;
	BOOL	bBadDieCompensate = FALSE;
	BOOL	bSpRefGradeCheckResult = FALSE;
	BOOL	bNoMotionHouseKeeping = FALSE;
	LONG	lTemp = 0;
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	static	BOOL bBTStopped = FALSE;
	static	LONG lTryLimit = 0;

	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP))
	{
		OpAbort1stDieFinderToPick();

		DisplaySequence("WT - STOP");
		State(STOPPING_Q);
		return ;
	}

	try
	{
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
		case DIE1_FINDER_Q:	
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
			DisplaySequence("WT - GET_WAFER_POS_Q");
			m_nLastError = OpGetWaferPos();
			OpSetGradeToEquip(GetCurrDieGrade() - m_WaferMapWrapper.GetGradeOffset());
			szMsg.Format("WT - MS60 Get Wafer Position, grade = %d", GetCurrDieGrade() - m_WaferMapWrapper.GetGradeOffset());
			DisplaySequence(szMsg);		//v4.50A12

			if ((m_nLastError) == gnOK)
			{
				BOOL bFirstCycleTakeLog = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["FirstCycleTakeLog"];
				if (m_bFirst && bFirstCycleTakeLog)
				{
					CString szTemp;
					szTemp.Format("MegaDa Cycle #%d", GetRecordNumber());
					DisplaySequence(szTemp);
					StartTime(GetRecordNumber());
				}

				LONG lWprFatalErr = (*m_psmfSRam)["WaferPr"]["WprFatalErr"];	//	-1 check
				if( m_bAutoSampleAfterPR && lWprFatalErr == -1 )
				{
					BOOL bToHKQ = FALSE;
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					UINT unNoDieExtra = pApp->GetProfileInt(gszPROFILE_SETTING, _T("PR no die extra limit"), 0);
					if( unNoDieExtra==1 )
					{
						bToHKQ	= TRUE;	//	this will not rescan, but alarm and stop, if press start button, do rescan automatically.
					}
					if( GetNewPickCount()<(ULONG)(m_lRescanPECounter + m_lRescanPELimit) )
					{
						bToHKQ	= TRUE;	
					}

					if( IsAutoRescanEnable() && bToHKQ==FALSE )	//	too many no die, to do rescan
					{
						m_lReadyToSampleOnWft	= 1;	//	PR error
						(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = -2;		//v3.76
						ChangeNextSampleLimit(0);
						DisplaySequence("WT - MS60 PR too many no die, to rescan");
					}	//	PR too many no die, auto rescan
					else if( DEB_IsUseable() && bToHKQ==FALSE )	//	WPR too many no die, let wft to do key die sample
					{
						(*m_psmfSRam)["WaferPr"]["WprFatalErr"] = -2;		//v3.76
						m_ulNextAdvSampleCount = GetNewPickCount();
						DisplaySequence("WT - MS60 PR too many no die, to sample key die");
					}	//	PR too many no die, auto key sampling

					if( bToHKQ )
					{
						m_bReSampleAsError = TRUE;	//	house keeping, rescan fail
						SetAlert_SE_Red_Yellow(IDS_WPR_MAX_NODIE_SKIPCOUNT);
						WTPRFailureCaseLog();
						SetErrorMessage("Too many no die (bToHKQ)");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}	//	have PR error and auto sample enable

				if (m_bFirst)
				{
					//CString szTemp;
					//szTemp.Format("MegaDa Cycle #%d", GetRecordNumber());
					//DisplaySequence(szTemp);
					//StartTime(GetRecordNumber());
					m_qSubOperation = MOVE_TABLE_Q;
				}
				else
				{
					if ( GetCurrDieEAct() != WAF_CDieSelectionAlgorithm::PICK )
					{
						CString szTemp;
						szTemp.Format("MegaDa Cycle #%d", GetRecordNumber());
						DisplaySequence(szTemp);
						StartTime(GetRecordNumber());
					}

					OpUpdateNextMotionTime();	//Update next WT motion time for DBH calcuation		//v3.94T4

					//check whether BHT move to the prepick position or not if do auto-rescan or clean ejector pin 
					CheckNextCycleBHTMovetoPrepick();

					if (m_bUseMS60NewLFCycle)
					{
						SetWTStartMove(TRUE);			//v4.47T5
					}

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
			DisplaySequence("WT - wait EJECTOR ready q MS60");
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				m_bDieIsGet = TRUE;		//To make sure Update Die is called in HouseKeeping since Get NextDie is already called
			}
			/*else if (WaitMoveBack())	// Need to check move back event first
			{
				SetPRStart(FALSE);		// Clear the last PR start event
				SetMoveBack(FALSE);
				OpRollBack();			// Roll back wafer map
				OpRollBack();			// Roll back wafer map
				SetEjectorReady(TRUE, "Wait Ej @ Move Back");
				m_bMoveBack = TRUE;
				m_qSubOperation = GET_WAFER_POS_Q;		// Get last position again
				DisplaySequence("WT - Wait Ej ready - wait move back --> Get Wafer Pos");
			}
			else if (WaitAutoChangeColletDone(0))
			{
				SetAutoChangeColletDone(FALSE);
				SetPRStart(FALSE);		// Clear the last PR start event
				SetMoveBack(FALSE);
				OpResetLFCycle();

				//=======New Add=================================
				//m_nChangeColletOnWT = TRUE;
				//OpUpdateDie();
				//OpRollBack();
				//m_stCurrentDie = m_stLastDie;
				//m_qSubOperation = GET_WAFER_POS_Q;		// Get last position again
				//================================================

				DisplaySequence("WT - Wait Ej ready - Wait Auto Change Collet Done --> Get Wafer Pos");
				CMSLogFileUtility::Instance()->MS60_Log("WT: Auto Change Collet Done");
			}*/
			else if (WaitEjectorReady())
			{
				GetEncoderValue();
				m_stLastDie.lX_Enc = GetCurrX() + m_lOrgDieCompen_X;
				m_stLastDie.lY_Enc = GetCurrY() + m_lOrgDieCompen_Y;
				m_stLastDie.lT_Enc = GetCurrT();

				//szMsg.Format("WT - WaitEjectorReady(): Update m_stLastDie.ENC(%ld, %ld, %ld)", m_stLastDie.lX_Enc, m_stLastDie.lY_Enc, m_stLastDie.lT_Enc);
				//CMSLogFileUtility::Instance()->MS60_Log(szMsg);
				//DisplaySequence(szMsg);

				if (m_bUseMS60NewLFCycle)
				{
					m_stMS60NewLFDie.lX_Enc	= m_lEnc_X;
					m_stMS60NewLFDie.lY_Enc	= m_lEnc_Y;
					m_stMS60NewLFDie.lT_Enc	= m_lEnc_T;
				
					szMsg.Format("WT WaitEjectorReady(): Update m_stMS60NewLFDie.ENC(%ld, %ld, %ld)", m_stMS60NewLFDie.lX_Enc, m_stMS60NewLFDie.lY_Enc, m_stMS60NewLFDie.lT_Enc);
					CMSLogFileUtility::Instance()->MS60_Log(szMsg);
					
					//szMsg.Format("WT - Update m_stMS60NewLFDie (%d,%d,%d) in WaitEjectorReady()", 
					//	m_stMS60NewLFDie.lX_Enc, m_stMS60NewLFDie.lY_Enc, m_stMS60NewLFDie.lT_Enc);
					//DisplaySequence(szMsg);
				}

				m_nChangeColletOnWT = FALSE;
				if (m_bUseMS60NewLFCycle)
				{
					//if (!WaitBhTReadyForWT())
					//{
						//v4.48A16	//If BH not at PrePick, still need to WAIT for MS60 LF cycle ...
						//if (IsBHAtPrePick() != 1)
						//{
						//	break;
						//}
					//}

//CMSLogFileUtility::Instance()->MS60_Log("WT: WaitBhTReadyForWT = TRUE");
//					SetBhTReadyForWT(FALSE);

//TakeTime(LC2);		//v4.57A13
				}
		
				//if (!m_bFirst)
				//{
				//	CString szTemp;
				//	szTemp.Format("MegaDa Cycle #%d", GetRecordNumber());
				//	DisplaySequence(szTemp);
				//	StartTime(GetRecordNumber());	// Restart the time
				//}
				
				m_pEjPinCleanRegion->IncCleanDirtEJPinCount();
				SetEjectorReady(FALSE, "Normal Flow");

				if ( m_bMoveBack == TRUE )
				{
					m_bMoveBack = FALSE;
				}
				else if (m_bEnableSmartWalk)		//v2.99T1
				{
				}
				else
				{
					if (IsBlkFuncEnable() == FALSE)
					{
						OpUpdateDie();				// Update the wafer map
					}
					m_bDieIsGet = FALSE;
				}

				//if do auto-rescan or clean Ejector Pin, it should reset LF cycle as the same as 1st cycle.
				//becasue the current die encode psotion will be changed after do auto-rescan or clean Ejector Pin, then move to the original position of current die
				//and move table instead of lookfroward for next-next die if m_bUseMS60NewLFCycle is TRUE
				CheckResetLFCycle();

				//if (!m_bUseMS60NewLFCycle)
				//{
					//Onnly perform normal PR serach EMPTY die in non-LF cycles
				//	OpMS60NGPickPrEmptyDiePos();	//v4.54A5
				//}

				m_qSubOperation = MOVE_TABLE_Q;
				if (m_bUseMS60NewLFCycle)
				{
					m_qSubOperation = WAIT_PR_DELAY_MS60_Q;
				}
				else
				{
					//CMSLogFileUtility::Instance()->MS60_Log("WT: WaitBhTReadyForWT = TRUE");
					SetBhTReadyForWT(FALSE);
				}
			}
			else if (m_bGoodDie == FALSE)
			{
				if (IsBHAtPrePick() == 1)
				{
					SetEjectorReady(TRUE, "Wait Ej & BH at prepick, good die 0");
				}
			}
			else if (m_bFirst && IsAutoRescanWafer() && IsAutoSampleSetupOK() )
			{	//	auto rescan sorting wafer	check ejector ready, as BH at prepick already. into move table q
				if (GetWftSampleState()==2 && GetNewPickCount()>=GetNextRscnLimit() ) // rescan
				{
					DisplaySequence("WT - BH at prepick, into move table directly");
					m_qSubOperation = MOVE_TABLE_Q;
					if (m_bUseMS60NewLFCycle)
					{
						m_qSubOperation = WAIT_PR_DELAY_MS60_Q;
					}
				}
			}
			else
			{
				Sleep(1);
			}
			break;


		case WAIT_PR_DELAY_MS60_Q:
			lTemp = (*m_psmfSRam)["WaferMap"]["ComingDieDirection"];
			(*m_psmfSRam)["WaferMap"]["NextDieDirection"] = lTemp;

			BOOL bPickAction;
			bPickAction = GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK;
			(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)bPickAction;

			CMSLogFileUtility::Instance()->MS60_Log("WT: WAIT_PR_DELAY_MS60_Q");

			LONG lPRDelay;
			lPRDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];
			if (lPRDelay > 0)
			{
				Sleep(lPRDelay);
				szMsg.Format("WT - Wait lmPRDelay = %d", lPRDelay);
				DisplaySequence(szMsg);
			}

			//if (m_bUseMS60NewLFCycle)
			{
				if (!WaitBhTReadyForWT(200))
				{		
					SetBhTReadyForWT(FALSE);
					szMsg = "WT - WaitBhTReadyForWT() = TRUE";
					CMSLogFileUtility::Instance()->MS60_Log(szMsg);
					DisplaySequence(szMsg);
				}
			}

			DisplaySequence("SetWTStable(TRUE) in WAIT_PR_DELAY_MS60_Q");
			SetWTStable(TRUE);				// Trigger Wafer PR 

			GetEncoderValue();	
			m_stCurrentDie.lX_Enc = GetCurrX();
			m_stCurrentDie.lY_Enc = GetCurrY();
			m_stCurrentDie.lT_Enc = GetCurrT();
			(*m_psmfSRam)["WaferPr"]["WTEncX"] = GetCurrX();
			(*m_psmfSRam)["WaferPr"]["WTEncY"] = GetCurrY();
			szMsg.Format("WT - PR delay ==> Wait GRAB Ready to pick %d", bPickAction);
			DisplaySequence(szMsg);

			m_qSubOperation = WAIT_GRAB_READY_MS60_Q;
			break;


		case WAIT_GRAB_READY_MS60_Q:
			if (IsBHStopped())
			{
				SetErrorMessage("WT60: stop encountered in WAIT_GRAB_READY_MS60_Q");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if ( WaitDieReady() )
			{
				CMSLogFileUtility::Instance()->MS60_Log("WT: WAIT_GRAB_READY_MS60_Q OK");
				m_qSubOperation = MOVE_TABLE_Q;
			}
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
				//2018.07.09 this protect the arm not 
				if (!m_bUseMS60NewLFCycle)
				{
					CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
					ULONG ulDelay = pAppMod->GetFeatureValue(MS896A_FUNC_GENERAL_DELAY_BEFORE_TURN_OFF_EJ_VAC);
					if ((ulDelay > 0) && (ulDelay < 200))
					{
						Sleep(ulDelay);
					}
				}

				TakeTime(EV2);			
				SetEjectorVacuum(FALSE);		// Release the vacuum

				//VAC off delay re-added for Excelitas
				LONG lVacOffDelay = (*m_psmfSRam)["WaferTable"]["VacOffDelay"];
				if ((lVacOffDelay > 0) && (lVacOffDelay < 10000))
				{
					Sleep(lVacOffDelay);
				}
				if (IsAutoSampleSetupOK())
				{	//	auto rescan sorting wafer	before move table, into prescan state
					if (GetWftSampleState() == 1)
					{
						Sleep(100);
					}

					LONG lWprFatalErr = (*m_psmfSRam)["WaferPr"]["WprFatalErr"];
					if( lWprFatalErr == -2 )
					{
						m_lReadyToSampleOnWft = 2;	//	rescan PR error
					}	//	PR too many no die, auto rescan

					if (IsAutoRescanEnable() && GetWftSampleState()==2 && GetNewPickCount()>=GetNextRscnLimit() || m_bForceRescan)
					{	// before move table to do scan	// ejector pin broken case >
						szMsg.Format("WT - MS60 Begin rescan wafer after BH at prepick 2, Rescan reset to #%d", m_nMS60CycleCount);
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						CMSLogFileUtility::Instance()->MS60_Log(szMsg);
						DisplaySequence(szMsg);
						RescanSortingWafer();
						m_bForceRescan = FALSE;
						m_qSubOperation			= PRESCAN_MOVE_TABLE_Q;
						break;
					}	//	auto rescan sorting wafer	before move table, into prescan state

					if( DEB_IsUseable() && GetWftSampleState()==2 )
					{	// before move table to do scan	// ejector pin broken case >
						szMsg = "WT - MS60 Begin KS wafer after BH at prepick 2";
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						DisplaySequence(szMsg);
					}
				}	//	auto rescan sorting wafer	before move table, into prescan state
				if (m_pEjPinCleanRegion->IsReachCleanDirtEJPin() && (GetWftCleanEjPinState() == 1))
				{
					Sleep(100);
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

				SetWTStartToMoveForBT(TRUE, "2");

//andrewng //2020-0617
//CString szLog;
//GetEncoderValue();
//szLog.Format("WT Start Move : EncXYT(%ld, %ld, %ld), LF-Offset(%d, %d)", m_lEnc_X, m_lEnc_Y, m_lEnc_T, m_stLFDie.lX, m_stLFDie.lY);
//CMSLogFileUtility::Instance()->MS60_Log(szLog);
//GetCmdValue();
//szLog.Format("WT Start Move : CMDXYT(%ld, %ld, %ld), LF-Offset(%d, %d)", m_lCmd_X, m_lCmd_Y, m_lEnc_T, m_stLFDie.lX, m_stLFDie.lY);
//CMSLogFileUtility::Instance()->MS60_Log(szLog);

				if ((m_nLastError = OpMoveTable()) == gnOK) // move table to target position
				{
//andrewng //2020-0617
//GetEncoderValue();
//szLog.Format("WT End Move : EncXYT(%ld, %ld, %ld), LF-Offset(%d, %d)", m_lEnc_X, m_lEnc_Y, m_lEnc_T, m_stLFDie.lX, m_stLFDie.lY);
//CMSLogFileUtility::Instance()->MS60_Log(szLog);
					m_bPerformCompensation = TRUE;
					TakeTime(WT2);	
					m_qSubOperation = WAIT_PR_DELAY_Q;

					//Matt: wafer table HW check
					/*CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					LONG lGrabDelay = pApp->GetProfileInt(gszPROFILE_SETTING, _T("WaferTable HW Check"), 0);
					if (lGrabDelay > 0 && IsBurnIn())
					{
						Sleep(lGrabDelay);
						DisplaySequence("StartUserSearch1");
						SetPRTesting(TRUE);
						Sleep(50);
					}*/
										
					if (m_bUseMS60NewLFCycle)
					{
						m_qSubOperation = WAIT_DIE_READY_Q;
					}
				}
				else
				{
//andrewng //2020-0617
//GetEncoderValue();
//szLog.Format("WT (Failure)End Move : EncXYT(%ld, %ld, %ld), LF-Offset(%d, %d)", m_lEnc_X, m_lEnc_Y, m_lEnc_T, m_stLFDie.lX, m_stLFDie.lY );
//CMSLogFileUtility::Instance()->MS60_Log(szLog);

					if ( (m_nLastError == Err_WTableDiePitchFailed) ||
						 (m_nLastError == WT_PITCH_ERR_ABS_NEARBY) )
					{
						if (IsAutoSampleSetupOK() && IsMapPosnOK() && IsAutoSampleSort() && (GetWftSampleState() == 0) &&
							(GetNextXXXCounter() > 0) && (m_ulBondDieCount - m_ulRescanAtBondDieCount > 100))
						{
							m_bForceRescan = TRUE;
							m_lReadyToSampleOnWft = 2;	// ADV or DEB or auto rescan or sampling

							m_ulRescanAtBondDieCount = m_ulBondDieCount;
							CString szMsg;
							szMsg.Format("WT - Move Table exceed die pitch and rescan sample offset 0->2, Bond Die Count,%d",m_ulBondDieCount);
							CMSLogFileUtility::Instance()->MS60_Log(szMsg);
							SaveAutoErrorHandlingLog("Auto Rescan,LF Exceed Pitch1", m_psmfSRam);
							break;
						}
						else
						{
							SetAlert_SE_Red_Yellow(IDS_WT_FAILEXCEEDPITCH);
							m_bPitchError = TRUE;
						}
					}
					else if( m_nLastError == WT_PITCH_ERR_ADV_SAMPLE ||	//	move table
							 m_nLastError == WT_PITCH_ERR_DEB_SAMPLE ||	//	move table
							 m_nLastError == WT_PITCH_ERR_ADV_UPDATE ||	//	move table
							 m_nLastError == WT_PITCH_ERR_SCN_PREDCT )	//	move table
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
						SetAlert_Red_Yellow(IDS_WT_INVALID_POS);		// Set Alarm
						SetErrorMessage("Invalid die position OR die outside Wafer Limit");
					}

					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			break;

		case WAIT_PR_DELAY_Q:
			bPickAction = (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK);
			szMsg.Format("WT - Wait PR Delay to pick %d", bPickAction);
			DisplaySequence(szMsg);
			LONG lmPRDelay;
			lTemp = (*m_psmfSRam)["WaferMap"]["ComingDieDirection"];
			(*m_psmfSRam)["WaferMap"]["NextDieDirection"] = lTemp;
			lmPRDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];

			if (m_bNewEjectorSequence)
			{
				SetEjectorVacuum(TRUE);
			}

			LONG lDelay;
			lDelay = lmPRDelay;
			if( lDelay == 0 && m_bDisableBH )
			{
				lDelay = 10;
			}

			//Add extra 10ms for LF cycle since more idle time is available for WT task					
			BOOL bLFNoComp;
			bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];	

			if (bLFNoComp && (lmPRDelay < 20) && (CMS896AApp::m_lCycleSpeedMode < 4))	//v4.05	//Added MS100+ mode
			{
				lDelay = lmPRDelay + 5;		//10;	//v3.86	
			}

			//v4.10T5	//Added extra 10ms PR delay for long-travel motion
			if (CMS896AApp::m_lCycleSpeedMode >= 6)		//MS100P3/MS60/MS90		//v4.50A24
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
					//v4.39T11
					if ((lDelay - 2) > 0)
					{
						lDelay = lDelay - 2;	//Used to compensate Event delay from WT to WPR CYcleState
						// to resolve camera blocking in MS100+
					}
				}
			}

/*
			//MS100 speedup - to reduce extra overhead of head-bond/pick delay
			if ( bPickAction && CMS896AApp::m_lCycleSpeedMode == 3)
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
*/
			CMS896AStn::MotionWait(WT_AXIS_X, lDelay, &m_stWTAxis_X);

			(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)bPickAction;

			//v4.49A4	//Reset events before triggering WPR
			SetDieReady(FALSE);
			SetBadDie(FALSE);
			if ( bPickAction && bLFNoComp )
			{
				m_bGoodDie = TRUE;
			}

			DisplaySequence("SetWTStable(TRUE) in WAIT_PR_DELAY_Q");
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
			if( (GetTime() - m_dIdleDiffTime) >= 5 * 60 * 1000 )
			{
				m_dIdleDiffTime = GetTime();
				CMSLogFileUtility::Instance()->MS_LogCycleState("running");
			}
			if( IsBlkFuncEnable() && m_bFirst==FALSE && m_bLastDieUpdate==FALSE )
			{
				OpUpdateDie();				// Update the wafer map
			}
			break;


		case WAIT_DIE_READY_Q:
			DisplaySequence("WT - wait die ready q");
			
			if (IsBHStopped())
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			// skip wait pr checking for prober
			else if (WaitDieReady())		// Good Die
			{
				CMSLogFileUtility::Instance()->MS60_Log("WaitDieReady OK");
				SetDieReady(FALSE);

				LONG lLFDir = (*m_psmfSRam)["WaferMap"]["ComingDieDirection"];

				if (lLFDir != -1)
				{
					m_nMS60CycleCount++;			//v4.47T3
					szMsg.Format("MS60 Cycle #%d", m_nMS60CycleCount);
				}
				else
				{
					//Long-Jump to next PICK die
					szMsg.Format("MS60 Cycle reset to #%d (LF=%d)", m_nMS60CycleCount, lLFDir);
					m_nMS60CycleCount = 0;			//v4.47T3
				}
				(*m_psmfSRam)["WaferMap"]["New LF Counter"] = (LONG)m_nMS60CycleCount;
				CMSLogFileUtility::Instance()->MS60_Log(szMsg);
				DisplaySequence(szMsg);

				SetBhTReadyForWT(FALSE);			//v4.47T3
				if (!m_bUseMS60NewLFCycle && m_bNewEjectorSequence)
				{
					SetEjectorVacuum(FALSE);
				}

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

				bool bPickAction = GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK;

				if ( bPickAction )
				{
					m_stCurrentDie.ulStatus = WT_MAP_DS_PICK;
					m_qSubOperation = CHECK_INFO_READ_Q;

					if ( DieIsNoPick(GetCurrentRow(), GetCurrentCol()) == TRUE )		//Check die is marked or not
					{
						m_stCurrentDie.eAction = WAF_CDieSelectionAlgorithm::ALIGN; 
						ULONG ulDieState = GetMapDieState(GetCurrentRow(), GetCurrentCol());
						if (!IsDieUnpickAll(ulDieState))
						{
							ulDieState = WT_MAP_DIESTATE_UNPICK;
						}
						m_stCurrentDie.ulStatus = ulDieState;

						m_bPerformCompensation = FALSE;
						DisplaySequence("WT - WAIT_DIE_READY_Q bPickAction, m_bPerformCompensation = FALSE");

						m_bGoodDie = FALSE;
CMSLogFileUtility::Instance()->BPR_Arm1Log("m_bGoodDie to FALSE 4");	//v4.49A2	
						m_bInfoWrite = TRUE;
						SetEjectorReady(TRUE, "Wait Die Ready @ Map die is NG");

						m_qSubOperation = COMPENSATION_Q;
					}
				}
				else
				{
					m_bPerformCompensation = TRUE;
					DisplaySequence("WT - WAIT_DIE_READY_Q !bPickAction, m_bPerformCompensation = TRUE");
					m_bGoodDie = FALSE;
CMSLogFileUtility::Instance()->BPR_Arm1Log("m_bGoodDie to FALSE 5");	//v4.49A2	
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

				LONG lRow = GetCurrentRow();
				LONG lCol = GetCurrentCol();
				LONG lUserRow=0, lUserCol=0;

				ConvertAsmToOrgUser(lRow, lCol, lUserRow, lUserCol);

				CString szLog;

				if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
				{
					szLog.Format("OK,%d,%d,%d,%d,PICK", lUserRow, lUserCol, GetCurrDieGrade(), m_stCurrentDie.ulStatus); 
				}
				else
				{
					szLog.Format("OK,%d,%d,%d,%d,ALIGN(%d)", lUserRow, lUserCol, GetCurrDieGrade(), m_stCurrentDie.ulStatus, GetCurrDieEAct()); 
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
				if ( (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK) ||
					(GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::ALIGN) )	//v3.24T1
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

				SetCompDone(TRUE);
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szLog);

				m_bGoodDie = FALSE;
CMSLogFileUtility::Instance()->BPR_Arm1Log("m_bGoodDie to FALSE 6");	//v4.49A2	

				if (bBadDieCompensate == FALSE)
				{
					DisplaySequence("WT - die ready to Wait LF Ready");
					m_qSubOperation = WAIT_LF_READY_Q;
				}
				else
				{
					m_bPerformCompensation = FALSE;
					DisplaySequence("WT - bBadDieCompensate == TRUE, m_bPerformCompensation = TRUE");
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
			if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
			{
				// Check whether the die info is read
				if (WaitDieInfoRead() /*&& WaitDieUnReadyForBT() */)	//	V450X16
				{
					BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];

					// If no need to do compensation, simply return
					if (m_bPreCompensate || (!m_bPerformCompensation) || (bLFNoComp) || IsES101() || IsES201())
					{
						TakeTime(LC4);
						SetDieInfoRead(FALSE);
						OpStoreDieInfo();
						SetDieReadyForBT(TRUE);

						m_bInfoWrite = TRUE;
						DisplaySequence("WT - Check Die Info Read done");
					}
					else
					{
						m_bInfoWrite = FALSE;
						DisplaySequence("WT - Check Die Info Read not done");
					}
				}
				else
				{
					m_bInfoWrite = FALSE;
				}
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
						CString szLog;
						GetEncoderValue();
						szLog.Format("WT (Failure)End Comp : EncXYT(%ld, %ld, %ld)", m_lEnc_X, m_lEnc_Y, m_lEnc_T);
						CMSLogFileUtility::Instance()->MS60_Log(szLog);

						if (IsAutoSampleSetupOK() && IsMapPosnOK() && IsAutoSampleSort() && (GetWftSampleState() == 0) &&
							(GetNextXXXCounter() > 0) && (m_ulBondDieCount - m_ulRescanAtBondDieCount > 100))
						{
							m_bForceRescan = TRUE;
							m_lReadyToSampleOnWft = 2;	// ADV or DEB or auto rescan or sampling

							CString szMsg;
							szMsg.Format("WT - Compensation exceed die pitch and rescan sample offset 0->2, Bond Die Count,%d",m_ulBondDieCount);
							CMSLogFileUtility::Instance()->MS60_Log(szMsg);
							SaveAutoErrorHandlingLog("Auto Rescan,Compensation Exceed Pitch", m_psmfSRam);
							m_ulRescanAtBondDieCount = m_ulBondDieCount;

							GetEncoderValue();	
							m_stLastDie.lX_Enc = GetCurrX();
							m_stLastDie.lY_Enc = GetCurrY();
							m_stLastDie.lT_Enc = GetCurrT();

							m_qSubOperation = MOVE_TABLE_Q;
							break;
						}
						else
						{
							SetAlert_SE_Red_Yellow(IDS_WT_FAILEXCEEDPITCH);
							m_bPitchError = TRUE;
						}
					}
					if (m_nLastError == WT_PITCH_ERR_ADV_UPDATE)	//	compensation
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
				if ( m_bMapWaferVerifyOK )
				{
					SetCompDone(TRUE, "WAIT_COMPENSATION_COMPLETE_Q");
				}
				if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
				{
					DisplaySequence("WT - Wait Compensation Complete1 -- Turn On Ejector Vacuum");
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
						if (IsThisDieDirectPick() == FALSE && m_bMapWaferVerifyOK)
						{
							SetWTReady(TRUE, "IsThisDieDirectPick");			// Trigger BH to PICK
						}
					}
					else
					{
						if (m_bMapWaferVerifyOK)
						{
							SetWTReady(TRUE, "m_bMapWaferVerifyOK");				// Trigger BH to PICK
						}
					}
				}
			}
			else
			{
				//Calculate total Pre-Compensation time
				BOOL dPreCompTotalTime = 0;

				if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
				{
					if (CMS896AApp::m_lCycleSpeedMode >= 4)		//v4.10T6
					{
						//Changed from 17 to 12
						m_lReadyDelay = 12;	//17;	//-> at least 12ms settling, assume BH & WT reach at the same time

						//Add more delay for WT so it has enough settling time before BH to PICK level
						if ( (m_lTime_CP + m_lReadyDelay) > lPickTime_Z )
							Sleep(m_lTime_CP + m_lReadyDelay - lPickTime_Z);

						//if (CMS896AApp::m_lCycleSpeedMode >= 6)		//For MS100P3 only
						//{
						//	//For DBH error problem found in TJ Sanan by Long;
						//	//Reset once more here
						//	SetEjectorReady(FALSE, "26");		//v4.48A22
						//}
//Matt: 2018Feb10 Bad Die Comp hang in AMC
					}
					else
					{
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
					}

					if (OpCheckSortingCheckPoints() != 1)
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
		
					if (CMS896AApp::m_lCycleSpeedMode >= 3)		//v3.89	//MS100 speedup to 100ms
					{
						if (IsThisDieDirectPick() == FALSE)
						{
							//SetWTReady(TRUE);
							BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
							if (bLFNoComp && (CMS896AApp::m_lCycleSpeedMode == 5))
							{
								//Already set to TRUE in Op MoveTable(); no need to set again
							}
							else if (m_bMapWaferVerifyOK)
							{
								CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
								if (IsBurnIn() && !pApp->m_bBurnInGrabImage)
								{
								}
								else
								{
									SetWTReady(TRUE,"3");
								}
							}
						}
						else
						{
							if (!m_bUseMS60NewLFCycle && !m_bUseLF2ndAlignment)
							{
								if (m_bSyncWTAndEjectorInBond)
								{
									SetWTReady(TRUE,"116 for non MS50 LF cycle");
								}
							}
						}
					}
					else
					{
						if (m_bMapWaferVerifyOK)
						{
							SetWTReady(TRUE,"4");				// Trigger BH to PICK
						}
					}
				}
#endif
				OpMoveComplete();
				TakeTime(CP2);
				//SetCompDone(TRUE) in OpMoveTable() if m_bUseMS60NewLFCycle == TRUE
				if (m_bMapWaferVerifyOK)
				{
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					if ( !(IsBurnIn() && !pApp->m_bBurnInGrabImage) )
					{
						//In BURNIN mode SetCompDone is done in OpMoveTable for speedup purpose (no LF PR case)
						if (!m_bUseMS60NewLFCycle)
						{
							SetCompDone(TRUE, "After TakeTime(CP2) !m_bUseMS60NewLFCycle");		//v4.34T10
						}
						else if (m_bUseLF2ndAlignment)
						{
							SetCompDone(TRUE, "After TakeTime(CP2) m_bUseLF2ndAlignment");
						}
					}
				}
				if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
				{
					DisplaySequence("WT - Wait Compensation Complete2 -- Turn On Ejector Vacuum");
					SetEjectorVacuum(TRUE);			// Turn-on Ejector Vacuum after comp.
					TakeTime(EV1);
				}

				//Sleep(15); //wafer table encoder settling, it is used for next die
			}

			//andrewng //2020-0619
			GetEncoderValue();
			(*m_psmfSRam)["WaferTable"]["WTEncX"] = GetCurrX();
			(*m_psmfSRam)["WaferTable"]["WTEncY"] = GetCurrY();

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
//CString szLog;
//GetEncoderValue();
//szLog.Format("WT (Failure)End BadDie : EncXYT(%ld, %ld, %ld)", m_lEnc_X, m_lEnc_Y, m_lEnc_T);
//CMSLogFileUtility::Instance()->MS60_Log(szLog);
						SetAlert_SE_Red_Yellow(IDS_WT_FAILEXCEEDPITCH);
						WTPRFailureCaseLog();
						m_bPitchError = TRUE;
					}
					if (m_nLastError == WT_PITCH_ERR_ADV_UPDATE)	//	bad die compensation
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
			else if (WaitDieInfoRead()/* && WaitDieUnReadyForBT() */)	//	V450X16
			{
				TakeTime(LC4);
				SetDieInfoRead(FALSE); 
				OpStoreDieInfo();
				SetDieReadyForBT(TRUE);

				DisplaySequence("WT - wait info read q done");

				m_qSubOperation = WAIT_LF_READY_Q;
			}
			break;

		case WAIT_LF_READY_Q:
			DisplaySequence("WT - Wait LF Ready");

			if (m_bFirst)
			{
				//SetWTReady(TRUE, "WT Compensation Complete");
			}

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
			m_nLastError = OpNextDie();
			if (m_nLastError == gnOK || (m_nLastError == WT_PITCH_ERR_ADV_UPDATE) || (m_nLastError == WT_PITCH_OVER_TOO_MUCH))
			{
				CMSLogFileUtility::Instance()->MS60_Log("OpNextDie OK");
				m_bDieIsGet = TRUE;
				m_qSubOperation = GET_WAFER_POS_Q;

				//v2.99T1
				if (m_nLastError == WT_PITCH_ERR_ADV_UPDATE || (m_nLastError == WT_PITCH_OVER_TOO_MUCH))
				{
					m_bForceRescan = TRUE;
					m_lReadyToSampleOnWft = 2;	// ADV or DEB or auto rescan or sampling

					m_ulRescanAtBondDieCount = m_ulBondDieCount;
					CString szMsg;
					szMsg.Format("WT - LF exceed die pitch and rescan sample offset 0->2, Bond Die Count,%d",m_ulBondDieCount);
					CMSLogFileUtility::Instance()->MS60_Log(szMsg);
					if (m_nLastError == WT_PITCH_ERR_ADV_UPDATE)
						SaveAutoErrorHandlingLog("Auto Rescan,LF Exceed Pitch2", m_psmfSRam);
					else
						SaveAutoErrorHandlingLog("Auto Rescan,LF Exceed Pitch Too much", m_psmfSRam);
				}
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

				if( m_nLastError == WT_PITCH_ERR_ADV_UPDATE )	// next die
				{
					SetAlert_Red_Yellow(IDS_WT_FAILEXCEEDCROSSPITCH);
					m_bPitchError = TRUE;
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				//else if ( m_nLastError == WT_PITCH_OVER_TOO_MUCH )
				//{
				//	HmiMessage_Red_Yellow("Pitch Exist Too Much! Please Get Machine log & PR Dump to production team!");
				//	WTPRFailureCaseLog(20);
				//	m_bPitchError = TRUE;
				//	m_qSubOperation = HOUSE_KEEPING_Q;
				//}
				else
				{
					OpUpdateDie();		// Update the wafer map
					m_bDieIsGet = FALSE;
					m_qSubOperation = WAFER_END_HANDLING_Q;
					pUtl->RegionOrderLog("REGN WFT next  die to wafer end q");
					DisplaySequence("WT - WFT next  die to wafer end q");
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
			pUtl->RegionOrderLog("REGN WFT - Wafer Region End");
			DisplaySequence("WT - Wafer End");
			remove("c:\\mapsorter\\userdata\\history\\WaferPrMsd.txt");

			// sort mode, 1st part wafer end
			if (Is2PhasesSortMode() && (m_b2Parts1stPartDone == FALSE))
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
						DisplaySequence("WT - MS60 in wafer end handling, 1st part ended");
						MS90Set1stDone( TRUE );
						m_bSortGoingTo2ndPart = TRUE;
						SetWTReady(FALSE,"5");
					}
				}
				else if (pSRInfo->IsManualAlignRegion())
				{
					SetWTReady(FALSE,"6");
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
					SetWTReady(FALSE,"7");
				}
			}
			pSRInfo->SetManualAlignRegion(FALSE);

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
				DisplaySequence("WT - MS60 set wafer end psmf_sram");
				(*m_psmfSRam)["WaferTable"]["WaferEnd"] = TRUE;
			}
			break;

		case HOUSE_KEEPING_Q:
			if( lTryLimit==0 )
			{
				szMsg.Format("WT - MS60 HKQ picked die %lu", GetNewPickCount());
				DisplaySequence(szMsg);
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

				SaveScanTimeEvent("    WFT: to stop wafer table cycle(MS60)");
				// prescan relative code
				StopCycle("WaferTableStn");
				Result(gnNOTOK);

				//Disable 1st die finder if machine stop
				OpAbort1stDieFinderToPick();
				SetHouseKeepingToken("WaferTableStn", TRUE);

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
					SaveScanTimeEvent("WFT: MS60 rescan error and stopped.");
					m_bReSampleAsError = TRUE;	//	house keeping, rescan fail
					ChangeNextSampleLimit(0);
				}

				if( m_bPitchError == TRUE && DEB_IsUseable() )
				{
					OpDebPitchErrorBackup();
				}	// picth error and copy log files to Deb result folder.

				OpBackupLogFiles();
			}
			else
			{
				DisplaySequence("WT - MS60 House Keeping Q");
			}

			//v4.02T1	//Moved to after so that DBH can reach HouseKeeping 
			// state to home Ej & BHZ before WT to UNLOAD during WaferEnd!!
			if( bNoMotionHouseKeeping == FALSE )	//	4.24TX 4
			{
				if( lTryLimit==0 )
				{
					LogCycleStopState("WT - waiting BH at safe");
				}
				//v4.02T1	//Must wait DBH to HouseKeeping to make sure Ej at safe pos before moving WT
				if ( IsBHStopped() != 1 )
				{
					Sleep(100);
					lTryLimit++;
					if( lTryLimit < 30000 )	// over 300 seconds
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
						if (IsRegionAlignRefBlk())
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
					if (bReferDie && IsOnlyRegionEnd())
					{
						(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"] = TRUE;
					}
					pUtl->RegionOrderLog("REGN wafer house keeping Q: after clear prescan data");
				}	// end if region mode

				OpUpdateMapViaPR();

				if( IsPrescanEnded() || IsPrescanEnable()==FALSE )
				{
					OpRotateBack();
				}
				if( m_bIsWaferEnded )	//	4.24TX 4
				{
					DisplaySequence("WFT: MS60 wafer end, move table to unload");
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

			CBinTable *pBinTable;
			pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
			if ( CMS896AStn::m_lBinTableStopped == 1 || pBinTable && pBinTable->IsIdle())
			{
				LogCycleStopState("WT - Update Last Die");

				LONG lX = (LONG)(*m_psmfSRam)["BinTable"]["Die Bonded X"];
				LONG lY = (LONG)(*m_psmfSRam)["BinTable"]["Die Bonded Y"];

				if ( m_bDieIsGet == TRUE )
				{
					OpUpdateDie();
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

					//v3.84		//For logging purpose only
					WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
					ULONG ulRow, ulCol;
					UCHAR ucGrade;
					BOOL bEndOfWafer;
					PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);

					if ( (GetLastDieRow() != lY) || (GetLastDieCol() != lX) )
					{
						OpRollBack();
#ifdef NU_MOTION
						//v3.84		//MS100 DBH only
						//May require double-Rollback, by checking if LastDie is equal to next PeekDie or not
						//** Next PeekDie must be the same die as LastDie for next AUTOBOND cycle **
						WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
						ULONG ulRow, ulCol;
						UCHAR ucGrade;
						BOOL bEndOfWafer;
						PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
						//Rollback once more if LastDIe not matched in DBH config
						if ((GetLastDieRow() != ulRow) || (GetLastDieCol() != ulCol) && IsBlkFunc2Enable()==FALSE )
							OpRollBack();
#endif
					}

					//v3.84		//For logging purpose only
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
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
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

				LogCycleStopState("WT - MS60 Stop completely");
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
				szMsg.Format("WT - Heap corrupted (%d) before WT [%s]", nHeapStatus, g_aszWTState[m_qSubOperation]);
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
		SetMotionCE(TRUE, "WT RunOperation_MS60 Stop");
	}
}


